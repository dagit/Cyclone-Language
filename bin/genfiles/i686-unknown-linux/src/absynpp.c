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
 struct Cyc_Core_Opt{void*v;};struct _dynforward_ptr Cyc_Core_new_string(
unsigned int);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};typedef struct{int __count;union{unsigned int __wch;char __wchb[
4];}__value;}Cyc___mbstate_t;typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;
typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,struct
_dynforward_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,
struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*
x);int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2);int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2);extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*
f);struct _dynforward_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);struct Cyc_PP_Doc*
Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_blank_doc();struct Cyc_PP_Doc*Cyc_PP_line_doc();
struct Cyc_PP_Doc*Cyc_PP_text(struct _dynforward_ptr s);struct Cyc_PP_Doc*Cyc_PP_textptr(
struct _dynforward_ptr*p);struct Cyc_PP_Doc*Cyc_PP_text_width(struct
_dynforward_ptr s,int w);struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dynforward_ptr);struct Cyc_PP_Doc*Cyc_PP_seq(
struct _dynforward_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_ppseq(
struct Cyc_PP_Doc*(*pp)(void*),struct _dynforward_ptr sep,struct Cyc_List_List*l);
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dynforward_ptr sep,struct Cyc_List_List*l0);
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct
_dynforward_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_group(struct
_dynforward_ptr start,struct _dynforward_ptr stop,struct _dynforward_ptr sep,struct
Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dynforward_ptr start,
struct _dynforward_ptr stop,struct _dynforward_ptr sep,struct Cyc_List_List*l);struct
Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct _dynforward_ptr line;int
line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _dynforward_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple0{union Cyc_Absyn_Nmspace_union
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_bounds_one;struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct _dynforward_ptr Cyc_Absyn_attribute2string(void*);struct _tuple3{void*f1;
struct _tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(void*);struct Cyc_Buffer_t;
unsigned int Cyc_strlen(struct _dynforward_ptr s);int Cyc_strptrcmp(struct
_dynforward_ptr*s1,struct _dynforward_ptr*s2);struct _dynforward_ptr Cyc_strconcat(
struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr Cyc_str_sepstr(
struct Cyc_List_List*,struct _dynforward_ptr);struct _tuple4{unsigned int f1;int f2;}
;struct _tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern
char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct _tuple5{void*f1;
void*f2;};struct _tuple5*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*
d);struct _tuple5*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
le;};void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};extern int Cyc_Absynpp_print_scopes;
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params
Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_tc_params_r;void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,
struct Cyc___cycFILE*f);struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*
d);struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct _dynforward_ptr Cyc_Absynpp_typ2cstring(
void*);struct _dynforward_ptr Cyc_Absynpp_kind2string(void*);struct _dynforward_ptr
Cyc_Absynpp_kindbound2string(void*);struct _dynforward_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dynforward_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct
_dynforward_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct
_dynforward_ptr Cyc_Absynpp_prim2string(void*p);struct _dynforward_ptr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat*p);struct _dynforward_ptr Cyc_Absynpp_scope2string(void*sc);
int Cyc_Absynpp_is_anon_aggrtype(void*t);extern struct _dynforward_ptr Cyc_Absynpp_cyc_string;
extern struct _dynforward_ptr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*);struct _dynforward_ptr Cyc_Absynpp_char_escape(char);struct
_dynforward_ptr Cyc_Absynpp_string_escape(struct _dynforward_ptr);struct
_dynforward_ptr Cyc_Absynpp_prim2str(void*p);int Cyc_Absynpp_is_declaration(struct
Cyc_Absyn_Stmt*s);struct _tuple6{struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;
void*f3;};struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple6*arg);struct _tuple7{
struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};struct _tuple7 Cyc_Absynpp_to_tms(
struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple8{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple8*dp);
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*
Cyc_Absynpp_typ2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*
fields);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e);
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es);
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst_union);
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(
int inprec,void*p,struct Cyc_List_List*es);struct _tuple9{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple9*de);
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*
dopt);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(struct
Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*
fs);struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);static
int Cyc_Absynpp_expand_typedefs;static int Cyc_Absynpp_qvar_to_Cids;static char _tmp0[
4]="Cyc";struct _dynforward_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0 + 4};struct
_dynforward_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;static int Cyc_Absynpp_add_cyc_prefix;
static int Cyc_Absynpp_to_VC;static int Cyc_Absynpp_decls_first;static int Cyc_Absynpp_rewrite_temp_tvars;
static int Cyc_Absynpp_print_all_tvars;static int Cyc_Absynpp_print_all_kinds;static
int Cyc_Absynpp_print_all_effects;static int Cyc_Absynpp_print_using_stmts;static
int Cyc_Absynpp_print_externC_stmts;static int Cyc_Absynpp_print_full_evars;static
int Cyc_Absynpp_generate_line_directives;static int Cyc_Absynpp_use_curr_namespace;
static int Cyc_Absynpp_print_zeroterm;static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=
0;struct Cyc_Absynpp_Params;void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*
fs){Cyc_Absynpp_expand_typedefs=fs->expand_typedefs;Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=fs->add_cyc_prefix;Cyc_Absynpp_to_VC=fs->to_VC;Cyc_Absynpp_decls_first=
fs->decls_first;Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;Cyc_Absynpp_print_all_tvars=
fs->print_all_tvars;Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;Cyc_Absynpp_print_all_effects=
fs->print_all_effects;Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;Cyc_Absynpp_print_externC_stmts=
fs->print_externC_stmts;Cyc_Absynpp_print_full_evars=fs->print_full_evars;Cyc_Absynpp_print_zeroterm=
fs->print_zeroterm;Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;Cyc_Absynpp_curr_namespace=
fs->curr_namespace;}struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,
1,0,0,0,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,0,
0,0,1,0,0,1,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,
0,1,0,0,0,0,0,0,0,0,0,0,0};struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,
0,0,0,0,0,0,0,1,1,0,1,0,0,0};static void Cyc_Absynpp_curr_namespace_add(struct
_dynforward_ptr*v){Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,({
struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v;_tmp1->tl=0;
_tmp1;}));}static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){if(((struct
Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;else{Cyc_Absynpp_suppr_last(&((
struct Cyc_List_List*)_check_null(*l))->tl);}}static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _dynforward_ptr Cyc_Absynpp_char_escape(char c){switch(c){case '\a': _LL0:
return({const char*_tmp2="\\a";_tag_dynforward(_tmp2,sizeof(char),
_get_zero_arr_size(_tmp2,3));});case '\b': _LL1: return({const char*_tmp3="\\b";
_tag_dynforward(_tmp3,sizeof(char),_get_zero_arr_size(_tmp3,3));});case '\f': _LL2:
return({const char*_tmp4="\\f";_tag_dynforward(_tmp4,sizeof(char),
_get_zero_arr_size(_tmp4,3));});case '\n': _LL3: return({const char*_tmp5="\\n";
_tag_dynforward(_tmp5,sizeof(char),_get_zero_arr_size(_tmp5,3));});case '\r': _LL4:
return({const char*_tmp6="\\r";_tag_dynforward(_tmp6,sizeof(char),
_get_zero_arr_size(_tmp6,3));});case '\t': _LL5: return({const char*_tmp7="\\t";
_tag_dynforward(_tmp7,sizeof(char),_get_zero_arr_size(_tmp7,3));});case '\v': _LL6:
return({const char*_tmp8="\\v";_tag_dynforward(_tmp8,sizeof(char),
_get_zero_arr_size(_tmp8,3));});case '\\': _LL7: return({const char*_tmp9="\\\\";
_tag_dynforward(_tmp9,sizeof(char),_get_zero_arr_size(_tmp9,3));});case '"': _LL8:
return({const char*_tmpA="\"";_tag_dynforward(_tmpA,sizeof(char),
_get_zero_arr_size(_tmpA,2));});case '\'': _LL9: return({const char*_tmpB="\\'";
_tag_dynforward(_tmpB,sizeof(char),_get_zero_arr_size(_tmpB,3));});default: _LLA:
if(c >= ' '  && c <= '~'){struct _dynforward_ptr _tmpC=Cyc_Core_new_string(2);({
struct _dynforward_ptr _tmpD=_dynforward_ptr_plus(_tmpC,sizeof(char),0);char _tmpE=*((
char*)_check_dynforward_subscript(_tmpD,sizeof(char),0));char _tmpF=c;if(
_get_dynforward_size(_tmpD,sizeof(char))== 1  && (_tmpE == '\000'  && _tmpF != '\000'))
_throw_arraybounds();*((char*)_tmpD.curr)=_tmpF;});return(struct _dynforward_ptr)
_tmpC;}else{struct _dynforward_ptr _tmp10=Cyc_Core_new_string(5);int j=0;({struct
_dynforward_ptr _tmp11=_dynforward_ptr_plus(_tmp10,sizeof(char),j ++);char _tmp12=*((
char*)_check_dynforward_subscript(_tmp11,sizeof(char),0));char _tmp13='\\';if(
_get_dynforward_size(_tmp11,sizeof(char))== 1  && (_tmp12 == '\000'  && _tmp13 != '\000'))
_throw_arraybounds();*((char*)_tmp11.curr)=_tmp13;});({struct _dynforward_ptr
_tmp14=_dynforward_ptr_plus(_tmp10,sizeof(char),j ++);char _tmp15=*((char*)
_check_dynforward_subscript(_tmp14,sizeof(char),0));char _tmp16=(char)('0' + ((
unsigned char)c >> 6 & 3));if(_get_dynforward_size(_tmp14,sizeof(char))== 1  && (
_tmp15 == '\000'  && _tmp16 != '\000'))_throw_arraybounds();*((char*)_tmp14.curr)=
_tmp16;});({struct _dynforward_ptr _tmp17=_dynforward_ptr_plus(_tmp10,sizeof(char),
j ++);char _tmp18=*((char*)_check_dynforward_subscript(_tmp17,sizeof(char),0));
char _tmp19=(char)('0' + (c >> 3 & 7));if(_get_dynforward_size(_tmp17,sizeof(char))
== 1  && (_tmp18 == '\000'  && _tmp19 != '\000'))_throw_arraybounds();*((char*)
_tmp17.curr)=_tmp19;});({struct _dynforward_ptr _tmp1A=_dynforward_ptr_plus(_tmp10,
sizeof(char),j ++);char _tmp1B=*((char*)_check_dynforward_subscript(_tmp1A,sizeof(
char),0));char _tmp1C=(char)('0' + (c & 7));if(_get_dynforward_size(_tmp1A,sizeof(
char))== 1  && (_tmp1B == '\000'  && _tmp1C != '\000'))_throw_arraybounds();*((char*)
_tmp1A.curr)=_tmp1C;});return(struct _dynforward_ptr)_tmp10;}}}static int Cyc_Absynpp_special(
struct _dynforward_ptr s){int sz=(int)(_get_dynforward_size(s,sizeof(char))- 1);{
int i=0;for(0;i < sz;i ++){char c=*((const char*)_check_dynforward_subscript(s,
sizeof(char),i));if(((c <= ' '  || c >= '~') || c == '"') || c == '\\')return 1;}}
return 0;}struct _dynforward_ptr Cyc_Absynpp_string_escape(struct _dynforward_ptr s){
if(!Cyc_Absynpp_special(s))return s;{int n=(int)(_get_dynforward_size(s,sizeof(
char))- 1);if(n > 0  && *((const char*)_check_dynforward_subscript(s,sizeof(char),n))
== '\000')n --;{int len=0;{int i=0;for(0;i <= n;i ++){char _tmp1D=*((const char*)
_check_dynforward_subscript(s,sizeof(char),i));_LLD: if(_tmp1D != '\a')goto _LLF;
_LLE: goto _LL10;_LLF: if(_tmp1D != '\b')goto _LL11;_LL10: goto _LL12;_LL11: if(_tmp1D != '\f')
goto _LL13;_LL12: goto _LL14;_LL13: if(_tmp1D != '\n')goto _LL15;_LL14: goto _LL16;_LL15:
if(_tmp1D != '\r')goto _LL17;_LL16: goto _LL18;_LL17: if(_tmp1D != '\t')goto _LL19;
_LL18: goto _LL1A;_LL19: if(_tmp1D != '\v')goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp1D
!= '\\')goto _LL1D;_LL1C: goto _LL1E;_LL1D: if(_tmp1D != '"')goto _LL1F;_LL1E: len +=2;
goto _LLC;_LL1F:;_LL20: if(_tmp1D >= ' '  && _tmp1D <= '~')len ++;else{len +=4;}goto
_LLC;_LLC:;}}{struct _dynforward_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));
int j=0;{int i=0;for(0;i <= n;i ++){char _tmp1E=*((const char*)
_check_dynforward_subscript(s,sizeof(char),i));_LL22: if(_tmp1E != '\a')goto _LL24;
_LL23:({struct _dynforward_ptr _tmp1F=_dynforward_ptr_plus(t,sizeof(char),j ++);
char _tmp20=*((char*)_check_dynforward_subscript(_tmp1F,sizeof(char),0));char
_tmp21='\\';if(_get_dynforward_size(_tmp1F,sizeof(char))== 1  && (_tmp20 == '\000'
 && _tmp21 != '\000'))_throw_arraybounds();*((char*)_tmp1F.curr)=_tmp21;});({
struct _dynforward_ptr _tmp22=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp23=*((
char*)_check_dynforward_subscript(_tmp22,sizeof(char),0));char _tmp24='a';if(
_get_dynforward_size(_tmp22,sizeof(char))== 1  && (_tmp23 == '\000'  && _tmp24 != '\000'))
_throw_arraybounds();*((char*)_tmp22.curr)=_tmp24;});goto _LL21;_LL24: if(_tmp1E != '\b')
goto _LL26;_LL25:({struct _dynforward_ptr _tmp25=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp26=*((char*)_check_dynforward_subscript(_tmp25,sizeof(char),0));
char _tmp27='\\';if(_get_dynforward_size(_tmp25,sizeof(char))== 1  && (_tmp26 == '\000'
 && _tmp27 != '\000'))_throw_arraybounds();*((char*)_tmp25.curr)=_tmp27;});({
struct _dynforward_ptr _tmp28=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp29=*((
char*)_check_dynforward_subscript(_tmp28,sizeof(char),0));char _tmp2A='b';if(
_get_dynforward_size(_tmp28,sizeof(char))== 1  && (_tmp29 == '\000'  && _tmp2A != '\000'))
_throw_arraybounds();*((char*)_tmp28.curr)=_tmp2A;});goto _LL21;_LL26: if(_tmp1E != '\f')
goto _LL28;_LL27:({struct _dynforward_ptr _tmp2B=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp2C=*((char*)_check_dynforward_subscript(_tmp2B,sizeof(char),0));
char _tmp2D='\\';if(_get_dynforward_size(_tmp2B,sizeof(char))== 1  && (_tmp2C == '\000'
 && _tmp2D != '\000'))_throw_arraybounds();*((char*)_tmp2B.curr)=_tmp2D;});({
struct _dynforward_ptr _tmp2E=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp2F=*((
char*)_check_dynforward_subscript(_tmp2E,sizeof(char),0));char _tmp30='f';if(
_get_dynforward_size(_tmp2E,sizeof(char))== 1  && (_tmp2F == '\000'  && _tmp30 != '\000'))
_throw_arraybounds();*((char*)_tmp2E.curr)=_tmp30;});goto _LL21;_LL28: if(_tmp1E != '\n')
goto _LL2A;_LL29:({struct _dynforward_ptr _tmp31=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp32=*((char*)_check_dynforward_subscript(_tmp31,sizeof(char),0));
char _tmp33='\\';if(_get_dynforward_size(_tmp31,sizeof(char))== 1  && (_tmp32 == '\000'
 && _tmp33 != '\000'))_throw_arraybounds();*((char*)_tmp31.curr)=_tmp33;});({
struct _dynforward_ptr _tmp34=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp35=*((
char*)_check_dynforward_subscript(_tmp34,sizeof(char),0));char _tmp36='n';if(
_get_dynforward_size(_tmp34,sizeof(char))== 1  && (_tmp35 == '\000'  && _tmp36 != '\000'))
_throw_arraybounds();*((char*)_tmp34.curr)=_tmp36;});goto _LL21;_LL2A: if(_tmp1E != '\r')
goto _LL2C;_LL2B:({struct _dynforward_ptr _tmp37=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp38=*((char*)_check_dynforward_subscript(_tmp37,sizeof(char),0));
char _tmp39='\\';if(_get_dynforward_size(_tmp37,sizeof(char))== 1  && (_tmp38 == '\000'
 && _tmp39 != '\000'))_throw_arraybounds();*((char*)_tmp37.curr)=_tmp39;});({
struct _dynforward_ptr _tmp3A=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp3B=*((
char*)_check_dynforward_subscript(_tmp3A,sizeof(char),0));char _tmp3C='r';if(
_get_dynforward_size(_tmp3A,sizeof(char))== 1  && (_tmp3B == '\000'  && _tmp3C != '\000'))
_throw_arraybounds();*((char*)_tmp3A.curr)=_tmp3C;});goto _LL21;_LL2C: if(_tmp1E != '\t')
goto _LL2E;_LL2D:({struct _dynforward_ptr _tmp3D=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp3E=*((char*)_check_dynforward_subscript(_tmp3D,sizeof(char),0));
char _tmp3F='\\';if(_get_dynforward_size(_tmp3D,sizeof(char))== 1  && (_tmp3E == '\000'
 && _tmp3F != '\000'))_throw_arraybounds();*((char*)_tmp3D.curr)=_tmp3F;});({
struct _dynforward_ptr _tmp40=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp41=*((
char*)_check_dynforward_subscript(_tmp40,sizeof(char),0));char _tmp42='t';if(
_get_dynforward_size(_tmp40,sizeof(char))== 1  && (_tmp41 == '\000'  && _tmp42 != '\000'))
_throw_arraybounds();*((char*)_tmp40.curr)=_tmp42;});goto _LL21;_LL2E: if(_tmp1E != '\v')
goto _LL30;_LL2F:({struct _dynforward_ptr _tmp43=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp44=*((char*)_check_dynforward_subscript(_tmp43,sizeof(char),0));
char _tmp45='\\';if(_get_dynforward_size(_tmp43,sizeof(char))== 1  && (_tmp44 == '\000'
 && _tmp45 != '\000'))_throw_arraybounds();*((char*)_tmp43.curr)=_tmp45;});({
struct _dynforward_ptr _tmp46=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp47=*((
char*)_check_dynforward_subscript(_tmp46,sizeof(char),0));char _tmp48='v';if(
_get_dynforward_size(_tmp46,sizeof(char))== 1  && (_tmp47 == '\000'  && _tmp48 != '\000'))
_throw_arraybounds();*((char*)_tmp46.curr)=_tmp48;});goto _LL21;_LL30: if(_tmp1E != '\\')
goto _LL32;_LL31:({struct _dynforward_ptr _tmp49=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp4A=*((char*)_check_dynforward_subscript(_tmp49,sizeof(char),0));
char _tmp4B='\\';if(_get_dynforward_size(_tmp49,sizeof(char))== 1  && (_tmp4A == '\000'
 && _tmp4B != '\000'))_throw_arraybounds();*((char*)_tmp49.curr)=_tmp4B;});({
struct _dynforward_ptr _tmp4C=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp4D=*((
char*)_check_dynforward_subscript(_tmp4C,sizeof(char),0));char _tmp4E='\\';if(
_get_dynforward_size(_tmp4C,sizeof(char))== 1  && (_tmp4D == '\000'  && _tmp4E != '\000'))
_throw_arraybounds();*((char*)_tmp4C.curr)=_tmp4E;});goto _LL21;_LL32: if(_tmp1E != '"')
goto _LL34;_LL33:({struct _dynforward_ptr _tmp4F=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp50=*((char*)_check_dynforward_subscript(_tmp4F,sizeof(char),0));
char _tmp51='\\';if(_get_dynforward_size(_tmp4F,sizeof(char))== 1  && (_tmp50 == '\000'
 && _tmp51 != '\000'))_throw_arraybounds();*((char*)_tmp4F.curr)=_tmp51;});({
struct _dynforward_ptr _tmp52=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp53=*((
char*)_check_dynforward_subscript(_tmp52,sizeof(char),0));char _tmp54='"';if(
_get_dynforward_size(_tmp52,sizeof(char))== 1  && (_tmp53 == '\000'  && _tmp54 != '\000'))
_throw_arraybounds();*((char*)_tmp52.curr)=_tmp54;});goto _LL21;_LL34:;_LL35: if(
_tmp1E >= ' '  && _tmp1E <= '~')({struct _dynforward_ptr _tmp55=_dynforward_ptr_plus(
t,sizeof(char),j ++);char _tmp56=*((char*)_check_dynforward_subscript(_tmp55,
sizeof(char),0));char _tmp57=_tmp1E;if(_get_dynforward_size(_tmp55,sizeof(char))
== 1  && (_tmp56 == '\000'  && _tmp57 != '\000'))_throw_arraybounds();*((char*)
_tmp55.curr)=_tmp57;});else{({struct _dynforward_ptr _tmp58=_dynforward_ptr_plus(t,
sizeof(char),j ++);char _tmp59=*((char*)_check_dynforward_subscript(_tmp58,sizeof(
char),0));char _tmp5A='\\';if(_get_dynforward_size(_tmp58,sizeof(char))== 1  && (
_tmp59 == '\000'  && _tmp5A != '\000'))_throw_arraybounds();*((char*)_tmp58.curr)=
_tmp5A;});({struct _dynforward_ptr _tmp5B=_dynforward_ptr_plus(t,sizeof(char),j ++);
char _tmp5C=*((char*)_check_dynforward_subscript(_tmp5B,sizeof(char),0));char
_tmp5D=(char)('0' + (_tmp1E >> 6 & 7));if(_get_dynforward_size(_tmp5B,sizeof(char))
== 1  && (_tmp5C == '\000'  && _tmp5D != '\000'))_throw_arraybounds();*((char*)
_tmp5B.curr)=_tmp5D;});({struct _dynforward_ptr _tmp5E=_dynforward_ptr_plus(t,
sizeof(char),j ++);char _tmp5F=*((char*)_check_dynforward_subscript(_tmp5E,sizeof(
char),0));char _tmp60=(char)('0' + (_tmp1E >> 3 & 7));if(_get_dynforward_size(_tmp5E,
sizeof(char))== 1  && (_tmp5F == '\000'  && _tmp60 != '\000'))_throw_arraybounds();*((
char*)_tmp5E.curr)=_tmp60;});({struct _dynforward_ptr _tmp61=_dynforward_ptr_plus(
t,sizeof(char),j ++);char _tmp62=*((char*)_check_dynforward_subscript(_tmp61,
sizeof(char),0));char _tmp63=(char)('0' + (_tmp1E & 7));if(_get_dynforward_size(
_tmp61,sizeof(char))== 1  && (_tmp62 == '\000'  && _tmp63 != '\000'))
_throw_arraybounds();*((char*)_tmp61.curr)=_tmp63;});}goto _LL21;_LL21:;}}return(
struct _dynforward_ptr)t;}}}}static char _tmp64[9]="restrict";static struct
_dynforward_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64 + 9};static char _tmp65[9]="volatile";
static struct _dynforward_ptr Cyc_Absynpp_volatile_string={_tmp65,_tmp65 + 9};static
char _tmp66[6]="const";static struct _dynforward_ptr Cyc_Absynpp_const_str={_tmp66,
_tmp66 + 6};static struct _dynforward_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _dynforward_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _dynforward_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;struct
Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=
0;if(tq.q_restrict)l=({struct Cyc_List_List*_tmp67=_cycalloc(sizeof(*_tmp67));
_tmp67->hd=Cyc_Absynpp_restrict_sp;_tmp67->tl=l;_tmp67;});if(tq.q_volatile)l=({
struct Cyc_List_List*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->hd=Cyc_Absynpp_volatile_sp;
_tmp68->tl=l;_tmp68;});if(tq.print_const)l=({struct Cyc_List_List*_tmp69=
_cycalloc(sizeof(*_tmp69));_tmp69->hd=Cyc_Absynpp_const_sp;_tmp69->tl=l;_tmp69;});
return Cyc_PP_egroup(({const char*_tmp6A="";_tag_dynforward(_tmp6A,sizeof(char),
_get_zero_arr_size(_tmp6A,1));}),({const char*_tmp6B=" ";_tag_dynforward(_tmp6B,
sizeof(char),_get_zero_arr_size(_tmp6B,2));}),({const char*_tmp6C=" ";
_tag_dynforward(_tmp6C,sizeof(char),_get_zero_arr_size(_tmp6C,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,l));}struct _dynforward_ptr Cyc_Absynpp_kind2string(void*k){void*
_tmp6D=k;_LL37: if((int)_tmp6D != 0)goto _LL39;_LL38: return({const char*_tmp6E="A";
_tag_dynforward(_tmp6E,sizeof(char),_get_zero_arr_size(_tmp6E,2));});_LL39: if((
int)_tmp6D != 1)goto _LL3B;_LL3A: return({const char*_tmp6F="M";_tag_dynforward(
_tmp6F,sizeof(char),_get_zero_arr_size(_tmp6F,2));});_LL3B: if((int)_tmp6D != 2)
goto _LL3D;_LL3C: return({const char*_tmp70="B";_tag_dynforward(_tmp70,sizeof(char),
_get_zero_arr_size(_tmp70,2));});_LL3D: if((int)_tmp6D != 3)goto _LL3F;_LL3E: return({
const char*_tmp71="R";_tag_dynforward(_tmp71,sizeof(char),_get_zero_arr_size(
_tmp71,2));});_LL3F: if((int)_tmp6D != 4)goto _LL41;_LL40: return({const char*_tmp72="E";
_tag_dynforward(_tmp72,sizeof(char),_get_zero_arr_size(_tmp72,2));});_LL41: if((
int)_tmp6D != 5)goto _LL36;_LL42: return({const char*_tmp73="I";_tag_dynforward(
_tmp73,sizeof(char),_get_zero_arr_size(_tmp73,2));});_LL36:;}struct Cyc_PP_Doc*
Cyc_Absynpp_kind2doc(void*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
struct _dynforward_ptr Cyc_Absynpp_kindbound2string(void*c){void*_tmp74=Cyc_Absyn_compress_kb(
c);void*_tmp75;void*_tmp76;_LL44: if(*((int*)_tmp74)!= 0)goto _LL46;_tmp75=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp74)->f1;_LL45: return Cyc_Absynpp_kind2string(
_tmp75);_LL46: if(*((int*)_tmp74)!= 1)goto _LL48;_LL47: if(Cyc_PP_tex_output)return({
const char*_tmp77="{?}";_tag_dynforward(_tmp77,sizeof(char),_get_zero_arr_size(
_tmp77,4));});else{return({const char*_tmp78="?";_tag_dynforward(_tmp78,sizeof(
char),_get_zero_arr_size(_tmp78,2));});}_LL48: if(*((int*)_tmp74)!= 2)goto _LL43;
_tmp76=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp74)->f2;_LL49: return Cyc_Absynpp_kind2string(
_tmp76);_LL43:;}struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){void*_tmp79=
Cyc_Absyn_compress_kb(c);void*_tmp7A;void*_tmp7B;_LL4B: if(*((int*)_tmp79)!= 0)
goto _LL4D;_tmp7A=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp79)->f1;_LL4C: return
Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7A));_LL4D: if(*((int*)_tmp79)!= 1)goto
_LL4F;_LL4E: if(Cyc_PP_tex_output)return Cyc_PP_text_width(({const char*_tmp7C="{?}";
_tag_dynforward(_tmp7C,sizeof(char),_get_zero_arr_size(_tmp7C,4));}),1);else{
return Cyc_PP_text(({const char*_tmp7D="?";_tag_dynforward(_tmp7D,sizeof(char),
_get_zero_arr_size(_tmp7D,2));}));}_LL4F: if(*((int*)_tmp79)!= 2)goto _LL4A;_tmp7B=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp79)->f2;_LL50: return Cyc_PP_text(Cyc_Absynpp_kind2string(
_tmp7B));_LL4A:;}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
return Cyc_PP_egroup(({const char*_tmp7E="<";_tag_dynforward(_tmp7E,sizeof(char),
_get_zero_arr_size(_tmp7E,2));}),({const char*_tmp7F=">";_tag_dynforward(_tmp7F,
sizeof(char),_get_zero_arr_size(_tmp7F,2));}),({const char*_tmp80=",";
_tag_dynforward(_tmp80,sizeof(char),_get_zero_arr_size(_tmp80,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){void*
_tmp81=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmp82;void*_tmp83;void*
_tmp84;_LL52: if(*((int*)_tmp81)!= 1)goto _LL54;_LL53: goto _LL55;_LL54: if(*((int*)
_tmp81)!= 0)goto _LL56;_tmp82=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp81)->f1;
if((int)_tmp82 != 2)goto _LL56;_LL55: return Cyc_PP_textptr(tv->name);_LL56: if(*((
int*)_tmp81)!= 2)goto _LL58;_tmp83=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp81)->f2;_LL57: _tmp84=_tmp83;goto _LL59;_LL58: if(*((int*)_tmp81)!= 0)goto _LL51;
_tmp84=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp81)->f1;_LL59: return({struct
Cyc_PP_Doc*_tmp85[3];_tmp85[2]=Cyc_Absynpp_kind2doc(_tmp84);_tmp85[1]=Cyc_PP_text(({
const char*_tmp86="::";_tag_dynforward(_tmp86,sizeof(char),_get_zero_arr_size(
_tmp86,3));}));_tmp85[0]=Cyc_PP_textptr(tv->name);Cyc_PP_cat(_tag_dynforward(
_tmp85,sizeof(struct Cyc_PP_Doc*),3));});_LL51:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List*tvs){return Cyc_PP_egroup(({const char*_tmp87="<";
_tag_dynforward(_tmp87,sizeof(char),_get_zero_arr_size(_tmp87,2));}),({const char*
_tmp88=">";_tag_dynforward(_tmp88,sizeof(char),_get_zero_arr_size(_tmp88,2));}),({
const char*_tmp89=",";_tag_dynforward(_tmp89,sizeof(char),_get_zero_arr_size(
_tmp89,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}static struct
_dynforward_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){return tv->name;}
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);return Cyc_PP_egroup(({const char*_tmp8A="<";
_tag_dynforward(_tmp8A,sizeof(char),_get_zero_arr_size(_tmp8A,2));}),({const char*
_tmp8B=">";_tag_dynforward(_tmp8B,sizeof(char),_get_zero_arr_size(_tmp8B,2));}),({
const char*_tmp8C=",";_tag_dynforward(_tmp8C,sizeof(char),_get_zero_arr_size(
_tmp8C,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct
Cyc_List_List*(*)(struct _dynforward_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));}struct _tuple10{struct Cyc_Absyn_Tqual
f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple10*t){return Cyc_Absynpp_tqtd2doc((*
t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return Cyc_PP_group(({const char*_tmp8D="(";_tag_dynforward(_tmp8D,sizeof(char),
_get_zero_arr_size(_tmp8D,2));}),({const char*_tmp8E=")";_tag_dynforward(_tmp8E,
sizeof(char),_get_zero_arr_size(_tmp8E,2));}),({const char*_tmp8F=",";
_tag_dynforward(_tmp8F,sizeof(char),_get_zero_arr_size(_tmp8F,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){void*_tmp90=att;
_LL5B: if((int)_tmp90 != 0)goto _LL5D;_LL5C: goto _LL5E;_LL5D: if((int)_tmp90 != 1)goto
_LL5F;_LL5E: goto _LL60;_LL5F: if((int)_tmp90 != 2)goto _LL61;_LL60: return Cyc_PP_nil_doc();
_LL61:;_LL62: return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL5A:;}struct
Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){for(0;atts != 0;
atts=atts->tl){void*_tmp91=(void*)atts->hd;_LL64: if((int)_tmp91 != 0)goto _LL66;
_LL65: return Cyc_PP_text(({const char*_tmp92=" _stdcall ";_tag_dynforward(_tmp92,
sizeof(char),_get_zero_arr_size(_tmp92,11));}));_LL66: if((int)_tmp91 != 1)goto
_LL68;_LL67: return Cyc_PP_text(({const char*_tmp93=" _cdecl ";_tag_dynforward(
_tmp93,sizeof(char),_get_zero_arr_size(_tmp93,9));}));_LL68: if((int)_tmp91 != 2)
goto _LL6A;_LL69: return Cyc_PP_text(({const char*_tmp94=" _fastcall ";
_tag_dynforward(_tmp94,sizeof(char),_get_zero_arr_size(_tmp94,12));}));_LL6A:;
_LL6B: goto _LL63;_LL63:;}return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(
struct Cyc_List_List*atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;
atts2 != 0;atts2=atts2->tl){void*_tmp95=(void*)atts2->hd;_LL6D: if((int)_tmp95 != 0)
goto _LL6F;_LL6E: goto _LL70;_LL6F: if((int)_tmp95 != 1)goto _LL71;_LL70: goto _LL72;
_LL71: if((int)_tmp95 != 2)goto _LL73;_LL72: goto _LL6C;_LL73:;_LL74: hasatt=1;goto
_LL6C;_LL6C:;}}if(!hasatt)return Cyc_PP_nil_doc();return({struct Cyc_PP_Doc*_tmp96[
3];_tmp96[2]=Cyc_PP_text(({const char*_tmp9B=")";_tag_dynforward(_tmp9B,sizeof(
char),_get_zero_arr_size(_tmp9B,2));}));_tmp96[1]=Cyc_PP_group(({const char*
_tmp98="";_tag_dynforward(_tmp98,sizeof(char),_get_zero_arr_size(_tmp98,1));}),({
const char*_tmp99="";_tag_dynforward(_tmp99,sizeof(char),_get_zero_arr_size(
_tmp99,1));}),({const char*_tmp9A=" ";_tag_dynforward(_tmp9A,sizeof(char),
_get_zero_arr_size(_tmp9A,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));
_tmp96[0]=Cyc_PP_text(({const char*_tmp97="__declspec(";_tag_dynforward(_tmp97,
sizeof(char),_get_zero_arr_size(_tmp97,12));}));Cyc_PP_cat(_tag_dynforward(
_tmp96,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(
void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(
struct Cyc_List_List*atts){if(atts == 0)return Cyc_PP_nil_doc();if(Cyc_Absynpp_to_VC)
return Cyc_Absynpp_noncallconv2doc(atts);return({struct Cyc_PP_Doc*_tmp9C[2];
_tmp9C[1]=Cyc_PP_group(({const char*_tmp9E="((";_tag_dynforward(_tmp9E,sizeof(
char),_get_zero_arr_size(_tmp9E,3));}),({const char*_tmp9F="))";_tag_dynforward(
_tmp9F,sizeof(char),_get_zero_arr_size(_tmp9F,3));}),({const char*_tmpA0=",";
_tag_dynforward(_tmpA0,sizeof(char),_get_zero_arr_size(_tmpA0,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,
atts));_tmp9C[0]=Cyc_PP_text(({const char*_tmp9D=" __attribute__";_tag_dynforward(
_tmp9D,sizeof(char),_get_zero_arr_size(_tmp9D,15));}));Cyc_PP_cat(
_tag_dynforward(_tmp9C,sizeof(struct Cyc_PP_Doc*),2));});}int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0)return 0;{void*_tmpA1=(void*)tms->hd;_LL76:
if(*((int*)_tmpA1)!= 2)goto _LL78;_LL77: return 1;_LL78: if(*((int*)_tmpA1)!= 5)goto
_LL7A;_LL79: if(!Cyc_Absynpp_to_VC)return 0;return Cyc_Absynpp_next_is_pointer(tms->tl);
_LL7A:;_LL7B: return 0;_LL75:;}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*Cyc_Absynpp_question(){
if(!((unsigned int)Cyc_Absynpp_cache_question)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_question=(
struct Cyc_PP_Doc*)Cyc_PP_text_width(({const char*_tmpA2="{?}";_tag_dynforward(
_tmpA2,sizeof(char),_get_zero_arr_size(_tmpA2,4));}),1);else{Cyc_Absynpp_cache_question=(
struct Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpA3="?";_tag_dynforward(_tmpA3,
sizeof(char),_get_zero_arr_size(_tmpA3,2));}));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output)Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(({
const char*_tmpA4="{\\lb}";_tag_dynforward(_tmpA4,sizeof(char),_get_zero_arr_size(
_tmpA4,6));}),1);else{Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text(({
const char*_tmpA5="{";_tag_dynforward(_tmpA5,sizeof(char),_get_zero_arr_size(
_tmpA5,2));}));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_rb=(
struct Cyc_PP_Doc*)Cyc_PP_text_width(({const char*_tmpA6="{\\rb}";_tag_dynforward(
_tmpA6,sizeof(char),_get_zero_arr_size(_tmpA6,6));}),1);else{Cyc_Absynpp_cache_rb=(
struct Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpA7="}";_tag_dynforward(_tmpA7,
sizeof(char),_get_zero_arr_size(_tmpA7,2));}));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output)Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text_width(({
const char*_tmpA8="\\$";_tag_dynforward(_tmpA8,sizeof(char),_get_zero_arr_size(
_tmpA8,3));}),1);else{Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text(({
const char*_tmpA9="$";_tag_dynforward(_tmpA9,sizeof(char),_get_zero_arr_size(
_tmpA9,2));}));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dynforward_ptr sep,struct Cyc_List_List*
ss){return({struct Cyc_PP_Doc*_tmpAA[3];_tmpAA[2]=Cyc_Absynpp_rb();_tmpAA[1]=Cyc_PP_seq(
sep,ss);_tmpAA[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dynforward(_tmpAA,sizeof(
struct Cyc_PP_Doc*),3));});}static void Cyc_Absynpp_print_tms(struct Cyc_List_List*
tms){for(0;tms != 0;tms=tms->tl){void*_tmpAB=(void*)tms->hd;void*_tmpAC;struct Cyc_List_List*
_tmpAD;_LL7D: if(*((int*)_tmpAB)!= 0)goto _LL7F;_LL7E:({void*_tmpAE[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpAF="Carray_mod ";_tag_dynforward(_tmpAF,sizeof(char),
_get_zero_arr_size(_tmpAF,12));}),_tag_dynforward(_tmpAE,sizeof(void*),0));});
goto _LL7C;_LL7F: if(*((int*)_tmpAB)!= 1)goto _LL81;_LL80:({void*_tmpB0[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpB1="ConstArray_mod ";_tag_dynforward(_tmpB1,sizeof(
char),_get_zero_arr_size(_tmpB1,16));}),_tag_dynforward(_tmpB0,sizeof(void*),0));});
goto _LL7C;_LL81: if(*((int*)_tmpAB)!= 3)goto _LL83;_tmpAC=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpAB)->f1;if(*((int*)_tmpAC)!= 1)goto _LL83;_tmpAD=((struct Cyc_Absyn_WithTypes_struct*)
_tmpAC)->f1;_LL82:({void*_tmpB2[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpB3="Function_mod(";
_tag_dynforward(_tmpB3,sizeof(char),_get_zero_arr_size(_tmpB3,14));}),
_tag_dynforward(_tmpB2,sizeof(void*),0));});for(0;_tmpAD != 0;_tmpAD=_tmpAD->tl){
struct Cyc_Core_Opt*_tmpB4=(*((struct _tuple1*)_tmpAD->hd)).f1;if(_tmpB4 == 0)({
void*_tmpB5[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpB6="?";_tag_dynforward(
_tmpB6,sizeof(char),_get_zero_arr_size(_tmpB6,2));}),_tag_dynforward(_tmpB5,
sizeof(void*),0));});else{({void*_tmpB7[0]={};Cyc_fprintf(Cyc_stderr,*((struct
_dynforward_ptr*)_tmpB4->v),_tag_dynforward(_tmpB7,sizeof(void*),0));});}if(
_tmpAD->tl != 0)({void*_tmpB8[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpB9=",";
_tag_dynforward(_tmpB9,sizeof(char),_get_zero_arr_size(_tmpB9,2));}),
_tag_dynforward(_tmpB8,sizeof(void*),0));});}({void*_tmpBA[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmpBB=") ";_tag_dynforward(_tmpBB,sizeof(char),_get_zero_arr_size(
_tmpBB,3));}),_tag_dynforward(_tmpBA,sizeof(void*),0));});goto _LL7C;_LL83: if(*((
int*)_tmpAB)!= 3)goto _LL85;_LL84:({void*_tmpBC[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmpBD="Function_mod()";_tag_dynforward(_tmpBD,sizeof(char),
_get_zero_arr_size(_tmpBD,15));}),_tag_dynforward(_tmpBC,sizeof(void*),0));});
goto _LL7C;_LL85: if(*((int*)_tmpAB)!= 5)goto _LL87;_LL86:({void*_tmpBE[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpBF="Attributes_mod ";_tag_dynforward(_tmpBF,sizeof(
char),_get_zero_arr_size(_tmpBF,16));}),_tag_dynforward(_tmpBE,sizeof(void*),0));});
goto _LL7C;_LL87: if(*((int*)_tmpAB)!= 4)goto _LL89;_LL88:({void*_tmpC0[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpC1="TypeParams_mod ";_tag_dynforward(_tmpC1,sizeof(
char),_get_zero_arr_size(_tmpC1,16));}),_tag_dynforward(_tmpC0,sizeof(void*),0));});
goto _LL7C;_LL89: if(*((int*)_tmpAB)!= 2)goto _LL7C;_LL8A:({void*_tmpC2[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpC3="Pointer_mod ";_tag_dynforward(_tmpC3,sizeof(char),
_get_zero_arr_size(_tmpC3,13));}),_tag_dynforward(_tmpC2,sizeof(void*),0));});
goto _LL7C;_LL7C:;}({void*_tmpC4[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpC5="\n";
_tag_dynforward(_tmpC5,sizeof(char),_get_zero_arr_size(_tmpC5,2));}),
_tag_dynforward(_tmpC4,sizeof(void*),0));});}struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(
int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);struct Cyc_PP_Doc*p_rest=({
struct Cyc_PP_Doc*_tmp104[3];_tmp104[2]=Cyc_PP_text(({const char*_tmp106=")";
_tag_dynforward(_tmp106,sizeof(char),_get_zero_arr_size(_tmp106,2));}));_tmp104[
1]=rest;_tmp104[0]=Cyc_PP_text(({const char*_tmp105="(";_tag_dynforward(_tmp105,
sizeof(char),_get_zero_arr_size(_tmp105,2));}));Cyc_PP_cat(_tag_dynforward(
_tmp104,sizeof(struct Cyc_PP_Doc*),3));});void*_tmpC6=(void*)tms->hd;struct Cyc_Absyn_Conref*
_tmpC7;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Conref*_tmpC9;void*_tmpCA;
struct Cyc_List_List*_tmpCB;struct Cyc_List_List*_tmpCC;struct Cyc_Position_Segment*
_tmpCD;int _tmpCE;struct Cyc_Absyn_PtrAtts _tmpCF;void*_tmpD0;struct Cyc_Absyn_Conref*
_tmpD1;struct Cyc_Absyn_Conref*_tmpD2;struct Cyc_Absyn_Conref*_tmpD3;struct Cyc_Absyn_Tqual
_tmpD4;_LL8C: if(*((int*)_tmpC6)!= 0)goto _LL8E;_tmpC7=((struct Cyc_Absyn_Carray_mod_struct*)
_tmpC6)->f1;_LL8D: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;return({
struct Cyc_PP_Doc*_tmpD5[2];_tmpD5[1]=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpC7)?Cyc_PP_text(({const char*_tmpD6="[]ZEROTERM ";_tag_dynforward(_tmpD6,
sizeof(char),_get_zero_arr_size(_tmpD6,12));})): Cyc_PP_text(({const char*_tmpD7="[]";
_tag_dynforward(_tmpD7,sizeof(char),_get_zero_arr_size(_tmpD7,3));}));_tmpD5[0]=
rest;Cyc_PP_cat(_tag_dynforward(_tmpD5,sizeof(struct Cyc_PP_Doc*),2));});_LL8E:
if(*((int*)_tmpC6)!= 1)goto _LL90;_tmpC8=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpC6)->f1;_tmpC9=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpC6)->f2;_LL8F:
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*
_tmpD8[4];_tmpD8[3]=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpC9)?Cyc_PP_text(({const char*_tmpDA="]ZEROTERM ";_tag_dynforward(_tmpDA,
sizeof(char),_get_zero_arr_size(_tmpDA,11));})): Cyc_PP_text(({const char*_tmpDB="]";
_tag_dynforward(_tmpDB,sizeof(char),_get_zero_arr_size(_tmpDB,2));}));_tmpD8[2]=
Cyc_Absynpp_exp2doc(_tmpC8);_tmpD8[1]=Cyc_PP_text(({const char*_tmpD9="[";
_tag_dynforward(_tmpD9,sizeof(char),_get_zero_arr_size(_tmpD9,2));}));_tmpD8[0]=
rest;Cyc_PP_cat(_tag_dynforward(_tmpD8,sizeof(struct Cyc_PP_Doc*),4));});_LL90:
if(*((int*)_tmpC6)!= 3)goto _LL92;_tmpCA=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpC6)->f1;_LL91: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{void*
_tmpDC=_tmpCA;struct Cyc_List_List*_tmpDD;int _tmpDE;struct Cyc_Absyn_VarargInfo*
_tmpDF;struct Cyc_Core_Opt*_tmpE0;struct Cyc_List_List*_tmpE1;struct Cyc_List_List*
_tmpE2;struct Cyc_Position_Segment*_tmpE3;_LL99: if(*((int*)_tmpDC)!= 1)goto _LL9B;
_tmpDD=((struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f1;_tmpDE=((struct Cyc_Absyn_WithTypes_struct*)
_tmpDC)->f2;_tmpDF=((struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f3;_tmpE0=((
struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f4;_tmpE1=((struct Cyc_Absyn_WithTypes_struct*)
_tmpDC)->f5;_LL9A: return({struct Cyc_PP_Doc*_tmpE4[2];_tmpE4[1]=Cyc_Absynpp_funargs2doc(
_tmpDD,_tmpDE,_tmpDF,_tmpE0,_tmpE1);_tmpE4[0]=rest;Cyc_PP_cat(_tag_dynforward(
_tmpE4,sizeof(struct Cyc_PP_Doc*),2));});_LL9B: if(*((int*)_tmpDC)!= 0)goto _LL98;
_tmpE2=((struct Cyc_Absyn_NoTypes_struct*)_tmpDC)->f1;_tmpE3=((struct Cyc_Absyn_NoTypes_struct*)
_tmpDC)->f2;_LL9C: return({struct Cyc_PP_Doc*_tmpE5[2];_tmpE5[1]=Cyc_PP_group(({
const char*_tmpE6="(";_tag_dynforward(_tmpE6,sizeof(char),_get_zero_arr_size(
_tmpE6,2));}),({const char*_tmpE7=")";_tag_dynforward(_tmpE7,sizeof(char),
_get_zero_arr_size(_tmpE7,2));}),({const char*_tmpE8=",";_tag_dynforward(_tmpE8,
sizeof(char),_get_zero_arr_size(_tmpE8,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,
_tmpE2));_tmpE5[0]=rest;Cyc_PP_cat(_tag_dynforward(_tmpE5,sizeof(struct Cyc_PP_Doc*),
2));});_LL98:;}_LL92: if(*((int*)_tmpC6)!= 5)goto _LL94;_tmpCB=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpC6)->f2;_LL93: if(!Cyc_Absynpp_to_VC){if(Cyc_Absynpp_next_is_pointer(tms->tl))
rest=p_rest;return({struct Cyc_PP_Doc*_tmpE9[2];_tmpE9[1]=Cyc_Absynpp_atts2doc(
_tmpCB);_tmpE9[0]=rest;Cyc_PP_cat(_tag_dynforward(_tmpE9,sizeof(struct Cyc_PP_Doc*),
2));});}else{if(Cyc_Absynpp_next_is_pointer(tms->tl))return({struct Cyc_PP_Doc*
_tmpEA[2];_tmpEA[1]=rest;_tmpEA[0]=Cyc_Absynpp_callconv2doc(_tmpCB);Cyc_PP_cat(
_tag_dynforward(_tmpEA,sizeof(struct Cyc_PP_Doc*),2));});return rest;}_LL94: if(*((
int*)_tmpC6)!= 4)goto _LL96;_tmpCC=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpC6)->f1;_tmpCD=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpC6)->f2;_tmpCE=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpC6)->f3;_LL95: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;if(_tmpCE)return({struct Cyc_PP_Doc*_tmpEB[2];_tmpEB[1]=Cyc_Absynpp_ktvars2doc(
_tmpCC);_tmpEB[0]=rest;Cyc_PP_cat(_tag_dynforward(_tmpEB,sizeof(struct Cyc_PP_Doc*),
2));});else{return({struct Cyc_PP_Doc*_tmpEC[2];_tmpEC[1]=Cyc_Absynpp_tvars2doc(
_tmpCC);_tmpEC[0]=rest;Cyc_PP_cat(_tag_dynforward(_tmpEC,sizeof(struct Cyc_PP_Doc*),
2));});}_LL96: if(*((int*)_tmpC6)!= 2)goto _LL8B;_tmpCF=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpC6)->f1;_tmpD0=(void*)_tmpCF.rgn;_tmpD1=_tmpCF.nullable;_tmpD2=_tmpCF.bounds;
_tmpD3=_tmpCF.zero_term;_tmpD4=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpC6)->f2;
_LL97: {struct Cyc_PP_Doc*ptr;{void*_tmpED=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmpD2);struct Cyc_Absyn_Exp*_tmpEE;void*_tmpEF;_LL9E: if((int)_tmpED != 0)goto
_LLA0;_LL9F: ptr=Cyc_Absynpp_question();goto _LL9D;_LLA0: if((int)_tmpED != 1)goto
_LLA2;_LLA1: ptr=({struct Cyc_PP_Doc*_tmpF0[2];_tmpF0[1]=Cyc_PP_text(({const char*
_tmpF1="-";_tag_dynforward(_tmpF1,sizeof(char),_get_zero_arr_size(_tmpF1,2));}));
_tmpF0[0]=Cyc_Absynpp_question();Cyc_PP_cat(_tag_dynforward(_tmpF0,sizeof(struct
Cyc_PP_Doc*),2));});goto _LL9D;_LLA2: if(_tmpED <= (void*)2)goto _LLA4;if(*((int*)
_tmpED)!= 0)goto _LLA4;_tmpEE=((struct Cyc_Absyn_Upper_b_struct*)_tmpED)->f1;_LLA3:
ptr=Cyc_PP_text(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,
_tmpD1)?({const char*_tmpF2="*";_tag_dynforward(_tmpF2,sizeof(char),
_get_zero_arr_size(_tmpF2,2));}):({const char*_tmpF3="@";_tag_dynforward(_tmpF3,
sizeof(char),_get_zero_arr_size(_tmpF3,2));}));{unsigned int _tmpF5;int _tmpF6;
struct _tuple4 _tmpF4=Cyc_Evexp_eval_const_uint_exp(_tmpEE);_tmpF5=_tmpF4.f1;
_tmpF6=_tmpF4.f2;if(!_tmpF6  || _tmpF5 != 1)ptr=({struct Cyc_PP_Doc*_tmpF7[4];
_tmpF7[3]=Cyc_Absynpp_rb();_tmpF7[2]=Cyc_Absynpp_exp2doc(_tmpEE);_tmpF7[1]=Cyc_Absynpp_lb();
_tmpF7[0]=ptr;Cyc_PP_cat(_tag_dynforward(_tmpF7,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL9D;}_LLA4: if(_tmpED <= (void*)2)goto _LL9D;if(*((int*)_tmpED)!= 1)goto _LL9D;
_tmpEF=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmpED)->f1;_LLA5: ptr=Cyc_PP_text(((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmpD1)?({const char*
_tmpF8="*";_tag_dynforward(_tmpF8,sizeof(char),_get_zero_arr_size(_tmpF8,2));}):({
const char*_tmpF9="@";_tag_dynforward(_tmpF9,sizeof(char),_get_zero_arr_size(
_tmpF9,2));}));ptr=({struct Cyc_PP_Doc*_tmpFA[4];_tmpFA[3]=Cyc_Absynpp_rb();
_tmpFA[2]=Cyc_Absynpp_typ2doc(_tmpEF);_tmpFA[1]=Cyc_Absynpp_lb();_tmpFA[0]=ptr;
Cyc_PP_cat(_tag_dynforward(_tmpFA,sizeof(struct Cyc_PP_Doc*),4));});goto _LL9D;
_LL9D:;}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr  && ((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmpD3))ptr=({struct Cyc_PP_Doc*_tmpFB[2];_tmpFB[1]=
Cyc_PP_text(({const char*_tmpFC="ZEROTERM ";_tag_dynforward(_tmpFC,sizeof(char),
_get_zero_arr_size(_tmpFC,10));}));_tmpFB[0]=ptr;Cyc_PP_cat(_tag_dynforward(
_tmpFB,sizeof(struct Cyc_PP_Doc*),2));});else{if(is_char_ptr  && !((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpD3))ptr=({struct Cyc_PP_Doc*
_tmpFD[2];_tmpFD[1]=Cyc_PP_text(({const char*_tmpFE="NOZEROTERM ";_tag_dynforward(
_tmpFE,sizeof(char),_get_zero_arr_size(_tmpFE,12));}));_tmpFD[0]=ptr;Cyc_PP_cat(
_tag_dynforward(_tmpFD,sizeof(struct Cyc_PP_Doc*),2));});}}{void*_tmpFF=Cyc_Tcutil_compress(
_tmpD0);_LLA7: if((int)_tmpFF != 2)goto _LLA9;_LLA8: goto _LLA6;_LLA9:;_LLAA: ptr=({
struct Cyc_PP_Doc*_tmp100[3];_tmp100[2]=Cyc_PP_text(({const char*_tmp101=" ";
_tag_dynforward(_tmp101,sizeof(char),_get_zero_arr_size(_tmp101,2));}));_tmp100[
1]=Cyc_Absynpp_typ2doc(_tmpD0);_tmp100[0]=ptr;Cyc_PP_cat(_tag_dynforward(_tmp100,
sizeof(struct Cyc_PP_Doc*),3));});_LLA6:;}ptr=({struct Cyc_PP_Doc*_tmp102[2];
_tmp102[1]=Cyc_Absynpp_tqual2doc(_tmpD4);_tmp102[0]=ptr;Cyc_PP_cat(
_tag_dynforward(_tmp102,sizeof(struct Cyc_PP_Doc*),2));});return({struct Cyc_PP_Doc*
_tmp103[2];_tmp103[1]=rest;_tmp103[0]=ptr;Cyc_PP_cat(_tag_dynforward(_tmp103,
sizeof(struct Cyc_PP_Doc*),2));});}_LL8B:;}}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t){void*_tmp107=Cyc_Tcutil_compress(t);_LLAC: if((int)_tmp107 != 2)goto _LLAE;
_LLAD: return Cyc_PP_text(({const char*_tmp108="`H";_tag_dynforward(_tmp108,sizeof(
char),_get_zero_arr_size(_tmp108,3));}));_LLAE: if((int)_tmp107 != 3)goto _LLB0;
_LLAF: return Cyc_PP_text(({const char*_tmp109="`U";_tag_dynforward(_tmp109,sizeof(
char),_get_zero_arr_size(_tmp109,3));}));_LLB0:;_LLB1: return Cyc_Absynpp_ntyp2doc(
t);_LLAB:;}static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct
Cyc_List_List**effects,void*t){void*_tmp10A=Cyc_Tcutil_compress(t);void*_tmp10B;
struct Cyc_List_List*_tmp10C;_LLB3: if(_tmp10A <= (void*)4)goto _LLB7;if(*((int*)
_tmp10A)!= 19)goto _LLB5;_tmp10B=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp10A)->f1;_LLB4:*rgions=({struct Cyc_List_List*_tmp10D=_cycalloc(sizeof(*
_tmp10D));_tmp10D->hd=Cyc_Absynpp_rgn2doc(_tmp10B);_tmp10D->tl=*rgions;_tmp10D;});
goto _LLB2;_LLB5: if(*((int*)_tmp10A)!= 20)goto _LLB7;_tmp10C=((struct Cyc_Absyn_JoinEff_struct*)
_tmp10A)->f1;_LLB6: for(0;_tmp10C != 0;_tmp10C=_tmp10C->tl){Cyc_Absynpp_effects2docs(
rgions,effects,(void*)_tmp10C->hd);}goto _LLB2;_LLB7:;_LLB8:*effects=({struct Cyc_List_List*
_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->hd=Cyc_Absynpp_typ2doc(t);_tmp10E->tl=*
effects;_tmp10E;});goto _LLB2;_LLB2:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(&
rgions,& effects,t);rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
rgions);effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0  && effects != 0)return Cyc_PP_group(({const char*_tmp10F="";
_tag_dynforward(_tmp10F,sizeof(char),_get_zero_arr_size(_tmp10F,1));}),({const
char*_tmp110="";_tag_dynforward(_tmp110,sizeof(char),_get_zero_arr_size(_tmp110,
1));}),({const char*_tmp111="+";_tag_dynforward(_tmp111,sizeof(char),
_get_zero_arr_size(_tmp111,2));}),effects);else{struct Cyc_PP_Doc*_tmp112=Cyc_Absynpp_group_braces(({
const char*_tmp117=",";_tag_dynforward(_tmp117,sizeof(char),_get_zero_arr_size(
_tmp117,2));}),rgions);return Cyc_PP_group(({const char*_tmp113="";_tag_dynforward(
_tmp113,sizeof(char),_get_zero_arr_size(_tmp113,1));}),({const char*_tmp114="";
_tag_dynforward(_tmp114,sizeof(char),_get_zero_arr_size(_tmp114,1));}),({const
char*_tmp115="+";_tag_dynforward(_tmp115,sizeof(char),_get_zero_arr_size(_tmp115,
2));}),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(effects,({struct Cyc_List_List*_tmp116=_cycalloc(sizeof(*
_tmp116));_tmp116->hd=_tmp112;_tmp116->tl=0;_tmp116;})));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(
void*k){void*_tmp118=k;_LLBA: if((int)_tmp118 != 0)goto _LLBC;_LLBB: return Cyc_PP_text(({
const char*_tmp119="struct ";_tag_dynforward(_tmp119,sizeof(char),
_get_zero_arr_size(_tmp119,8));}));_LLBC: if((int)_tmp118 != 1)goto _LLB9;_LLBD:
return Cyc_PP_text(({const char*_tmp11A="union ";_tag_dynforward(_tmp11A,sizeof(
char),_get_zero_arr_size(_tmp11A,7));}));_LLB9:;}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(
void*t){struct Cyc_PP_Doc*s;{void*_tmp11B=t;struct Cyc_Core_Opt*_tmp11C;struct Cyc_Core_Opt*
_tmp11D;int _tmp11E;struct Cyc_Core_Opt*_tmp11F;struct Cyc_Absyn_Tvar*_tmp120;
struct Cyc_Absyn_TunionInfo _tmp121;void*_tmp122;struct Cyc_List_List*_tmp123;
struct Cyc_Core_Opt*_tmp124;struct Cyc_Absyn_TunionFieldInfo _tmp125;void*_tmp126;
struct Cyc_List_List*_tmp127;void*_tmp128;void*_tmp129;int _tmp12A;struct Cyc_List_List*
_tmp12B;struct Cyc_Absyn_AggrInfo _tmp12C;void*_tmp12D;struct Cyc_List_List*_tmp12E;
void*_tmp12F;struct Cyc_List_List*_tmp130;struct Cyc_List_List*_tmp131;struct
_tuple0*_tmp132;void*_tmp133;struct _tuple0*_tmp134;struct Cyc_List_List*_tmp135;
struct Cyc_Absyn_Typedefdecl*_tmp136;void*_tmp137;int _tmp138;void*_tmp139;void*
_tmp13A;_LLBF: if(_tmp11B <= (void*)4)goto _LLC5;if(*((int*)_tmp11B)!= 7)goto _LLC1;
_LLC0: return Cyc_PP_text(({const char*_tmp13B="[[[array]]]";_tag_dynforward(
_tmp13B,sizeof(char),_get_zero_arr_size(_tmp13B,12));}));_LLC1: if(*((int*)
_tmp11B)!= 8)goto _LLC3;_LLC2: return Cyc_PP_nil_doc();_LLC3: if(*((int*)_tmp11B)!= 
4)goto _LLC5;_LLC4: return Cyc_PP_nil_doc();_LLC5: if((int)_tmp11B != 0)goto _LLC7;
_LLC6: s=Cyc_PP_text(({const char*_tmp13C="void";_tag_dynforward(_tmp13C,sizeof(
char),_get_zero_arr_size(_tmp13C,5));}));goto _LLBE;_LLC7: if(_tmp11B <= (void*)4)
goto _LLD1;if(*((int*)_tmp11B)!= 0)goto _LLC9;_tmp11C=((struct Cyc_Absyn_Evar_struct*)
_tmp11B)->f1;_tmp11D=((struct Cyc_Absyn_Evar_struct*)_tmp11B)->f2;_tmp11E=((
struct Cyc_Absyn_Evar_struct*)_tmp11B)->f3;_tmp11F=((struct Cyc_Absyn_Evar_struct*)
_tmp11B)->f4;_LLC8: if(_tmp11D != 0)return Cyc_Absynpp_ntyp2doc((void*)_tmp11D->v);
else{void*_tmp13D=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp11C))->v;_LLF4:
if((int)_tmp13D != 3)goto _LLF6;_LLF5: s=({struct Cyc_PP_Doc*_tmp13E[2];_tmp13E[1]=
Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp142;_tmp142.tag=
1;_tmp142.f1=(unsigned long)_tmp11E;{void*_tmp140[1]={& _tmp142};Cyc_aprintf(({
const char*_tmp141="%d";_tag_dynforward(_tmp141,sizeof(char),_get_zero_arr_size(
_tmp141,3));}),_tag_dynforward(_tmp140,sizeof(void*),1));}}));_tmp13E[0]=Cyc_PP_text(({
const char*_tmp13F="`r";_tag_dynforward(_tmp13F,sizeof(char),_get_zero_arr_size(
_tmp13F,3));}));Cyc_PP_cat(_tag_dynforward(_tmp13E,sizeof(struct Cyc_PP_Doc*),2));});
goto _LLF3;_LLF6:;_LLF7: s=({struct Cyc_PP_Doc*_tmp143[6];_tmp143[5]=_tmp11C == 0?
Cyc_Absynpp_question(): Cyc_Absynpp_kind2doc((void*)_tmp11C->v);_tmp143[4]=Cyc_PP_text(({
const char*_tmp14A=")::";_tag_dynforward(_tmp14A,sizeof(char),_get_zero_arr_size(
_tmp14A,4));}));_tmp143[3]=(!Cyc_Absynpp_print_full_evars  || _tmp11F == 0)?Cyc_PP_text(({
const char*_tmp149="";_tag_dynforward(_tmp149,sizeof(char),_get_zero_arr_size(
_tmp149,1));})): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)_tmp11F->v);_tmp143[
2]=Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp148;_tmp148.tag=
1;_tmp148.f1=(unsigned long)_tmp11E;{void*_tmp146[1]={& _tmp148};Cyc_aprintf(({
const char*_tmp147="%d";_tag_dynforward(_tmp147,sizeof(char),_get_zero_arr_size(
_tmp147,3));}),_tag_dynforward(_tmp146,sizeof(void*),1));}}));_tmp143[1]=Cyc_PP_text(({
const char*_tmp145="(";_tag_dynforward(_tmp145,sizeof(char),_get_zero_arr_size(
_tmp145,2));}));_tmp143[0]=Cyc_PP_text(({const char*_tmp144="%";_tag_dynforward(
_tmp144,sizeof(char),_get_zero_arr_size(_tmp144,2));}));Cyc_PP_cat(
_tag_dynforward(_tmp143,sizeof(struct Cyc_PP_Doc*),6));});_LLF3:;}goto _LLBE;_LLC9:
if(*((int*)_tmp11B)!= 1)goto _LLCB;_tmp120=((struct Cyc_Absyn_VarType_struct*)
_tmp11B)->f1;_LLCA: s=Cyc_PP_textptr(_tmp120->name);if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_tmp14B[3];_tmp14B[2]=Cyc_Absynpp_kindbound2doc((void*)
_tmp120->kind);_tmp14B[1]=Cyc_PP_text(({const char*_tmp14C="::";_tag_dynforward(
_tmp14C,sizeof(char),_get_zero_arr_size(_tmp14C,3));}));_tmp14B[0]=s;Cyc_PP_cat(
_tag_dynforward(_tmp14B,sizeof(struct Cyc_PP_Doc*),3));});if(Cyc_Absynpp_rewrite_temp_tvars
 && Cyc_Tcutil_is_temp_tvar(_tmp120))s=({struct Cyc_PP_Doc*_tmp14D[3];_tmp14D[2]=
Cyc_PP_text(({const char*_tmp14F=" */";_tag_dynforward(_tmp14F,sizeof(char),
_get_zero_arr_size(_tmp14F,4));}));_tmp14D[1]=s;_tmp14D[0]=Cyc_PP_text(({const
char*_tmp14E="_ /* ";_tag_dynforward(_tmp14E,sizeof(char),_get_zero_arr_size(
_tmp14E,6));}));Cyc_PP_cat(_tag_dynforward(_tmp14D,sizeof(struct Cyc_PP_Doc*),3));});
goto _LLBE;_LLCB: if(*((int*)_tmp11B)!= 2)goto _LLCD;_tmp121=((struct Cyc_Absyn_TunionType_struct*)
_tmp11B)->f1;_tmp122=(void*)_tmp121.tunion_info;_tmp123=_tmp121.targs;_tmp124=
_tmp121.rgn;_LLCC:{void*_tmp150=_tmp122;struct Cyc_Absyn_UnknownTunionInfo _tmp151;
struct _tuple0*_tmp152;int _tmp153;struct Cyc_Absyn_Tuniondecl**_tmp154;struct Cyc_Absyn_Tuniondecl*
_tmp155;struct Cyc_Absyn_Tuniondecl _tmp156;struct _tuple0*_tmp157;int _tmp158;_LLF9:
if(*((int*)_tmp150)!= 0)goto _LLFB;_tmp151=((struct Cyc_Absyn_UnknownTunion_struct*)
_tmp150)->f1;_tmp152=_tmp151.name;_tmp153=_tmp151.is_xtunion;_LLFA: _tmp157=
_tmp152;_tmp158=_tmp153;goto _LLFC;_LLFB: if(*((int*)_tmp150)!= 1)goto _LLF8;
_tmp154=((struct Cyc_Absyn_KnownTunion_struct*)_tmp150)->f1;_tmp155=*_tmp154;
_tmp156=*_tmp155;_tmp157=_tmp156.name;_tmp158=_tmp156.is_xtunion;_LLFC: {struct
Cyc_PP_Doc*_tmp159=Cyc_PP_text(_tmp158?({const char*_tmp15E="xtunion ";
_tag_dynforward(_tmp15E,sizeof(char),_get_zero_arr_size(_tmp15E,9));}):({const
char*_tmp15F="tunion ";_tag_dynforward(_tmp15F,sizeof(char),_get_zero_arr_size(
_tmp15F,8));}));void*r=(unsigned int)_tmp124?(void*)_tmp124->v:(void*)2;{void*
_tmp15A=Cyc_Tcutil_compress(r);_LLFE: if((int)_tmp15A != 2)goto _LL100;_LLFF: s=({
struct Cyc_PP_Doc*_tmp15B[3];_tmp15B[2]=Cyc_Absynpp_tps2doc(_tmp123);_tmp15B[1]=
Cyc_Absynpp_qvar2doc(_tmp157);_tmp15B[0]=_tmp159;Cyc_PP_cat(_tag_dynforward(
_tmp15B,sizeof(struct Cyc_PP_Doc*),3));});goto _LLFD;_LL100:;_LL101: s=({struct Cyc_PP_Doc*
_tmp15C[5];_tmp15C[4]=Cyc_Absynpp_tps2doc(_tmp123);_tmp15C[3]=Cyc_Absynpp_qvar2doc(
_tmp157);_tmp15C[2]=Cyc_PP_text(({const char*_tmp15D=" ";_tag_dynforward(_tmp15D,
sizeof(char),_get_zero_arr_size(_tmp15D,2));}));_tmp15C[1]=Cyc_Absynpp_typ2doc(r);
_tmp15C[0]=_tmp159;Cyc_PP_cat(_tag_dynforward(_tmp15C,sizeof(struct Cyc_PP_Doc*),
5));});goto _LLFD;_LLFD:;}goto _LLF8;}_LLF8:;}goto _LLBE;_LLCD: if(*((int*)_tmp11B)
!= 3)goto _LLCF;_tmp125=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp11B)->f1;
_tmp126=(void*)_tmp125.field_info;_tmp127=_tmp125.targs;_LLCE:{void*_tmp160=
_tmp126;struct Cyc_Absyn_UnknownTunionFieldInfo _tmp161;struct _tuple0*_tmp162;
struct _tuple0*_tmp163;int _tmp164;struct Cyc_Absyn_Tuniondecl*_tmp165;struct Cyc_Absyn_Tuniondecl
_tmp166;struct _tuple0*_tmp167;int _tmp168;struct Cyc_Absyn_Tunionfield*_tmp169;
struct Cyc_Absyn_Tunionfield _tmp16A;struct _tuple0*_tmp16B;_LL103: if(*((int*)
_tmp160)!= 0)goto _LL105;_tmp161=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp160)->f1;_tmp162=_tmp161.tunion_name;_tmp163=_tmp161.field_name;_tmp164=
_tmp161.is_xtunion;_LL104: _tmp167=_tmp162;_tmp168=_tmp164;_tmp16B=_tmp163;goto
_LL106;_LL105: if(*((int*)_tmp160)!= 1)goto _LL102;_tmp165=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp160)->f1;_tmp166=*_tmp165;_tmp167=_tmp166.name;_tmp168=_tmp166.is_xtunion;
_tmp169=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp160)->f2;_tmp16A=*_tmp169;
_tmp16B=_tmp16A.name;_LL106: {struct Cyc_PP_Doc*_tmp16C=Cyc_PP_text(_tmp168?({
const char*_tmp16F="xtunion ";_tag_dynforward(_tmp16F,sizeof(char),
_get_zero_arr_size(_tmp16F,9));}):({const char*_tmp170="tunion ";_tag_dynforward(
_tmp170,sizeof(char),_get_zero_arr_size(_tmp170,8));}));s=({struct Cyc_PP_Doc*
_tmp16D[4];_tmp16D[3]=Cyc_Absynpp_qvar2doc(_tmp16B);_tmp16D[2]=Cyc_PP_text(({
const char*_tmp16E=".";_tag_dynforward(_tmp16E,sizeof(char),_get_zero_arr_size(
_tmp16E,2));}));_tmp16D[1]=Cyc_Absynpp_qvar2doc(_tmp167);_tmp16D[0]=_tmp16C;Cyc_PP_cat(
_tag_dynforward(_tmp16D,sizeof(struct Cyc_PP_Doc*),4));});goto _LL102;}_LL102:;}
goto _LLBE;_LLCF: if(*((int*)_tmp11B)!= 5)goto _LLD1;_tmp128=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp11B)->f1;_tmp129=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp11B)->f2;_LLD0: {
struct _dynforward_ptr sns;struct _dynforward_ptr ts;{void*_tmp171=_tmp128;_LL108:
if((int)_tmp171 != 2)goto _LL10A;_LL109: goto _LL10B;_LL10A: if((int)_tmp171 != 0)goto
_LL10C;_LL10B: sns=({const char*_tmp172="";_tag_dynforward(_tmp172,sizeof(char),
_get_zero_arr_size(_tmp172,1));});goto _LL107;_LL10C: if((int)_tmp171 != 1)goto
_LL107;_LL10D: sns=({const char*_tmp173="unsigned ";_tag_dynforward(_tmp173,
sizeof(char),_get_zero_arr_size(_tmp173,10));});goto _LL107;_LL107:;}{void*
_tmp174=_tmp129;_LL10F: if((int)_tmp174 != 0)goto _LL111;_LL110:{void*_tmp175=
_tmp128;_LL11A: if((int)_tmp175 != 2)goto _LL11C;_LL11B: sns=({const char*_tmp176="";
_tag_dynforward(_tmp176,sizeof(char),_get_zero_arr_size(_tmp176,1));});goto
_LL119;_LL11C: if((int)_tmp175 != 0)goto _LL11E;_LL11D: sns=({const char*_tmp177="signed ";
_tag_dynforward(_tmp177,sizeof(char),_get_zero_arr_size(_tmp177,8));});goto
_LL119;_LL11E: if((int)_tmp175 != 1)goto _LL119;_LL11F: sns=({const char*_tmp178="unsigned ";
_tag_dynforward(_tmp178,sizeof(char),_get_zero_arr_size(_tmp178,10));});goto
_LL119;_LL119:;}ts=({const char*_tmp179="char";_tag_dynforward(_tmp179,sizeof(
char),_get_zero_arr_size(_tmp179,5));});goto _LL10E;_LL111: if((int)_tmp174 != 1)
goto _LL113;_LL112: ts=({const char*_tmp17A="short";_tag_dynforward(_tmp17A,sizeof(
char),_get_zero_arr_size(_tmp17A,6));});goto _LL10E;_LL113: if((int)_tmp174 != 2)
goto _LL115;_LL114: ts=({const char*_tmp17B="int";_tag_dynforward(_tmp17B,sizeof(
char),_get_zero_arr_size(_tmp17B,4));});goto _LL10E;_LL115: if((int)_tmp174 != 3)
goto _LL117;_LL116: ts=({const char*_tmp17C="long";_tag_dynforward(_tmp17C,sizeof(
char),_get_zero_arr_size(_tmp17C,5));});goto _LL10E;_LL117: if((int)_tmp174 != 4)
goto _LL10E;_LL118: ts=Cyc_Absynpp_to_VC?({const char*_tmp17D="__int64";
_tag_dynforward(_tmp17D,sizeof(char),_get_zero_arr_size(_tmp17D,8));}):({const
char*_tmp17E="long long";_tag_dynforward(_tmp17E,sizeof(char),_get_zero_arr_size(
_tmp17E,10));});goto _LL10E;_LL10E:;}s=Cyc_PP_text((struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp182;_tmp182.tag=0;_tmp182.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)ts);{struct Cyc_String_pa_struct _tmp181;
_tmp181.tag=0;_tmp181.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)sns);{
void*_tmp17F[2]={& _tmp181,& _tmp182};Cyc_aprintf(({const char*_tmp180="%s%s";
_tag_dynforward(_tmp180,sizeof(char),_get_zero_arr_size(_tmp180,5));}),
_tag_dynforward(_tmp17F,sizeof(void*),2));}}}));goto _LLBE;}_LLD1: if((int)_tmp11B
!= 1)goto _LLD3;_LLD2: s=Cyc_PP_text(({const char*_tmp183="float";_tag_dynforward(
_tmp183,sizeof(char),_get_zero_arr_size(_tmp183,6));}));goto _LLBE;_LLD3: if(
_tmp11B <= (void*)4)goto _LLE9;if(*((int*)_tmp11B)!= 6)goto _LLD5;_tmp12A=((struct
Cyc_Absyn_DoubleType_struct*)_tmp11B)->f1;_LLD4: if(_tmp12A)s=Cyc_PP_text(({const
char*_tmp184="long double";_tag_dynforward(_tmp184,sizeof(char),
_get_zero_arr_size(_tmp184,12));}));else{s=Cyc_PP_text(({const char*_tmp185="double";
_tag_dynforward(_tmp185,sizeof(char),_get_zero_arr_size(_tmp185,7));}));}goto
_LLBE;_LLD5: if(*((int*)_tmp11B)!= 9)goto _LLD7;_tmp12B=((struct Cyc_Absyn_TupleType_struct*)
_tmp11B)->f1;_LLD6: s=({struct Cyc_PP_Doc*_tmp186[2];_tmp186[1]=Cyc_Absynpp_args2doc(
_tmp12B);_tmp186[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dynforward(_tmp186,
sizeof(struct Cyc_PP_Doc*),2));});goto _LLBE;_LLD7: if(*((int*)_tmp11B)!= 10)goto
_LLD9;_tmp12C=((struct Cyc_Absyn_AggrType_struct*)_tmp11B)->f1;_tmp12D=(void*)
_tmp12C.aggr_info;_tmp12E=_tmp12C.targs;_LLD8: {void*_tmp188;struct _tuple0*
_tmp189;struct _tuple3 _tmp187=Cyc_Absyn_aggr_kinded_name(_tmp12D);_tmp188=_tmp187.f1;
_tmp189=_tmp187.f2;s=({struct Cyc_PP_Doc*_tmp18A[3];_tmp18A[2]=Cyc_Absynpp_tps2doc(
_tmp12E);_tmp18A[1]=Cyc_Absynpp_qvar2doc(_tmp189);_tmp18A[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp188);Cyc_PP_cat(_tag_dynforward(_tmp18A,sizeof(struct Cyc_PP_Doc*),3));});
goto _LLBE;}_LLD9: if(*((int*)_tmp11B)!= 11)goto _LLDB;_tmp12F=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp11B)->f1;_tmp130=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp11B)->f2;_LLDA: s=({
struct Cyc_PP_Doc*_tmp18B[4];_tmp18B[3]=Cyc_Absynpp_rb();_tmp18B[2]=Cyc_PP_nest(2,
Cyc_Absynpp_aggrfields2doc(_tmp130));_tmp18B[1]=Cyc_Absynpp_lb();_tmp18B[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp12F);Cyc_PP_cat(_tag_dynforward(_tmp18B,sizeof(struct Cyc_PP_Doc*),4));});
goto _LLBE;_LLDB: if(*((int*)_tmp11B)!= 13)goto _LLDD;_tmp131=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp11B)->f1;_LLDC: s=({struct Cyc_PP_Doc*_tmp18C[4];_tmp18C[3]=Cyc_Absynpp_rb();
_tmp18C[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp131));_tmp18C[1]=Cyc_Absynpp_lb();
_tmp18C[0]=Cyc_PP_text(({const char*_tmp18D="enum ";_tag_dynforward(_tmp18D,
sizeof(char),_get_zero_arr_size(_tmp18D,6));}));Cyc_PP_cat(_tag_dynforward(
_tmp18C,sizeof(struct Cyc_PP_Doc*),4));});goto _LLBE;_LLDD: if(*((int*)_tmp11B)!= 
12)goto _LLDF;_tmp132=((struct Cyc_Absyn_EnumType_struct*)_tmp11B)->f1;_LLDE: s=({
struct Cyc_PP_Doc*_tmp18E[2];_tmp18E[1]=Cyc_Absynpp_qvar2doc(_tmp132);_tmp18E[0]=
Cyc_PP_text(({const char*_tmp18F="enum ";_tag_dynforward(_tmp18F,sizeof(char),
_get_zero_arr_size(_tmp18F,6));}));Cyc_PP_cat(_tag_dynforward(_tmp18E,sizeof(
struct Cyc_PP_Doc*),2));});goto _LLBE;_LLDF: if(*((int*)_tmp11B)!= 14)goto _LLE1;
_tmp133=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp11B)->f1;_LLE0: s=({
struct Cyc_PP_Doc*_tmp190[3];_tmp190[2]=Cyc_PP_text(({const char*_tmp192=">";
_tag_dynforward(_tmp192,sizeof(char),_get_zero_arr_size(_tmp192,2));}));_tmp190[
1]=Cyc_Absynpp_typ2doc(_tmp133);_tmp190[0]=Cyc_PP_text(({const char*_tmp191="sizeof_t<";
_tag_dynforward(_tmp191,sizeof(char),_get_zero_arr_size(_tmp191,10));}));Cyc_PP_cat(
_tag_dynforward(_tmp190,sizeof(struct Cyc_PP_Doc*),3));});goto _LLBE;_LLE1: if(*((
int*)_tmp11B)!= 16)goto _LLE3;_tmp134=((struct Cyc_Absyn_TypedefType_struct*)
_tmp11B)->f1;_tmp135=((struct Cyc_Absyn_TypedefType_struct*)_tmp11B)->f2;_tmp136=((
struct Cyc_Absyn_TypedefType_struct*)_tmp11B)->f3;_LLE2: s=({struct Cyc_PP_Doc*
_tmp193[2];_tmp193[1]=Cyc_Absynpp_tps2doc(_tmp135);_tmp193[0]=Cyc_Absynpp_qvar2doc(
_tmp134);Cyc_PP_cat(_tag_dynforward(_tmp193,sizeof(struct Cyc_PP_Doc*),2));});
goto _LLBE;_LLE3: if(*((int*)_tmp11B)!= 15)goto _LLE5;_tmp137=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp11B)->f1;_LLE4: s=({struct Cyc_PP_Doc*_tmp194[3];_tmp194[2]=Cyc_PP_text(({
const char*_tmp196=">";_tag_dynforward(_tmp196,sizeof(char),_get_zero_arr_size(
_tmp196,2));}));_tmp194[1]=Cyc_Absynpp_rgn2doc(_tmp137);_tmp194[0]=Cyc_PP_text(({
const char*_tmp195="region_t<";_tag_dynforward(_tmp195,sizeof(char),
_get_zero_arr_size(_tmp195,10));}));Cyc_PP_cat(_tag_dynforward(_tmp194,sizeof(
struct Cyc_PP_Doc*),3));});goto _LLBE;_LLE5: if(*((int*)_tmp11B)!= 18)goto _LLE7;
_tmp138=((struct Cyc_Absyn_TypeInt_struct*)_tmp11B)->f1;_LLE6: s=Cyc_PP_text((
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp199;_tmp199.tag=1;_tmp199.f1=(
unsigned long)_tmp138;{void*_tmp197[1]={& _tmp199};Cyc_aprintf(({const char*
_tmp198="`%d";_tag_dynforward(_tmp198,sizeof(char),_get_zero_arr_size(_tmp198,4));}),
_tag_dynforward(_tmp197,sizeof(void*),1));}}));goto _LLBE;_LLE7: if(*((int*)
_tmp11B)!= 17)goto _LLE9;_tmp139=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp11B)->f1;
_LLE8: s=({struct Cyc_PP_Doc*_tmp19A[3];_tmp19A[2]=Cyc_PP_text(({const char*_tmp19C=">";
_tag_dynforward(_tmp19C,sizeof(char),_get_zero_arr_size(_tmp19C,2));}));_tmp19A[
1]=Cyc_Absynpp_typ2doc(_tmp139);_tmp19A[0]=Cyc_PP_text(({const char*_tmp19B="tag_t<";
_tag_dynforward(_tmp19B,sizeof(char),_get_zero_arr_size(_tmp19B,7));}));Cyc_PP_cat(
_tag_dynforward(_tmp19A,sizeof(struct Cyc_PP_Doc*),3));});goto _LLBE;_LLE9: if((int)
_tmp11B != 3)goto _LLEB;_LLEA: goto _LLEC;_LLEB: if((int)_tmp11B != 2)goto _LLED;_LLEC:
s=Cyc_Absynpp_rgn2doc(t);goto _LLBE;_LLED: if(_tmp11B <= (void*)4)goto _LLEF;if(*((
int*)_tmp11B)!= 21)goto _LLEF;_tmp13A=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp11B)->f1;_LLEE: s=({struct Cyc_PP_Doc*_tmp19D[3];_tmp19D[2]=Cyc_PP_text(({
const char*_tmp19F=")";_tag_dynforward(_tmp19F,sizeof(char),_get_zero_arr_size(
_tmp19F,2));}));_tmp19D[1]=Cyc_Absynpp_typ2doc(_tmp13A);_tmp19D[0]=Cyc_PP_text(({
const char*_tmp19E="regions(";_tag_dynforward(_tmp19E,sizeof(char),
_get_zero_arr_size(_tmp19E,9));}));Cyc_PP_cat(_tag_dynforward(_tmp19D,sizeof(
struct Cyc_PP_Doc*),3));});goto _LLBE;_LLEF: if(_tmp11B <= (void*)4)goto _LLF1;if(*((
int*)_tmp11B)!= 19)goto _LLF1;_LLF0: goto _LLF2;_LLF1: if(_tmp11B <= (void*)4)goto
_LLBE;if(*((int*)_tmp11B)!= 20)goto _LLBE;_LLF2: s=Cyc_Absynpp_eff2doc(t);goto
_LLBE;_LLBE:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct _dynforward_ptr*)vo->v));}
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple5*cmp){struct _tuple5 _tmp1A1;
void*_tmp1A2;void*_tmp1A3;struct _tuple5*_tmp1A0=cmp;_tmp1A1=*_tmp1A0;_tmp1A2=
_tmp1A1.f1;_tmp1A3=_tmp1A1.f2;return({struct Cyc_PP_Doc*_tmp1A4[3];_tmp1A4[2]=Cyc_Absynpp_rgn2doc(
_tmp1A3);_tmp1A4[1]=Cyc_PP_text(({const char*_tmp1A5=" > ";_tag_dynforward(
_tmp1A5,sizeof(char),_get_zero_arr_size(_tmp1A5,4));}));_tmp1A4[0]=Cyc_Absynpp_rgn2doc(
_tmp1A2);Cyc_PP_cat(_tag_dynforward(_tmp1A4,sizeof(struct Cyc_PP_Doc*),3));});}
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){return Cyc_PP_group(({
const char*_tmp1A6="";_tag_dynforward(_tmp1A6,sizeof(char),_get_zero_arr_size(
_tmp1A6,1));}),({const char*_tmp1A7="";_tag_dynforward(_tmp1A7,sizeof(char),
_get_zero_arr_size(_tmp1A7,1));}),({const char*_tmp1A8=",";_tag_dynforward(
_tmp1A8,sizeof(char),_get_zero_arr_size(_tmp1A8,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,
po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple1*t){struct Cyc_Core_Opt*
dopt=(*t).f1 == 0?0:({struct Cyc_Core_Opt*_tmp1A9=_cycalloc(sizeof(*_tmp1A9));
_tmp1A9->v=Cyc_PP_text(*((struct _dynforward_ptr*)((struct Cyc_Core_Opt*)
_check_null((*t).f1))->v));_tmp1A9;});return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,
dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp1AA=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs)_tmp1AA=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1AA,({struct
Cyc_List_List*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB->hd=Cyc_PP_text(({
const char*_tmp1AC="...";_tag_dynforward(_tmp1AC,sizeof(char),_get_zero_arr_size(
_tmp1AC,4));}));_tmp1AB->tl=0;_tmp1AB;}));else{if(cyc_varargs != 0){struct Cyc_PP_Doc*
_tmp1AD=({struct Cyc_PP_Doc*_tmp1AF[3];_tmp1AF[2]=Cyc_Absynpp_funarg2doc(({struct
_tuple1*_tmp1B3=_cycalloc(sizeof(*_tmp1B3));_tmp1B3->f1=cyc_varargs->name;
_tmp1B3->f2=cyc_varargs->tq;_tmp1B3->f3=(void*)cyc_varargs->type;_tmp1B3;}));
_tmp1AF[1]=cyc_varargs->inject?Cyc_PP_text(({const char*_tmp1B1=" inject ";
_tag_dynforward(_tmp1B1,sizeof(char),_get_zero_arr_size(_tmp1B1,9));})): Cyc_PP_text(({
const char*_tmp1B2=" ";_tag_dynforward(_tmp1B2,sizeof(char),_get_zero_arr_size(
_tmp1B2,2));}));_tmp1AF[0]=Cyc_PP_text(({const char*_tmp1B0="...";_tag_dynforward(
_tmp1B0,sizeof(char),_get_zero_arr_size(_tmp1B0,4));}));Cyc_PP_cat(
_tag_dynforward(_tmp1AF,sizeof(struct Cyc_PP_Doc*),3));});_tmp1AA=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1AA,({struct
Cyc_List_List*_tmp1AE=_cycalloc(sizeof(*_tmp1AE));_tmp1AE->hd=_tmp1AD;_tmp1AE->tl=
0;_tmp1AE;}));}}{struct Cyc_PP_Doc*_tmp1B4=Cyc_PP_group(({const char*_tmp1BC="";
_tag_dynforward(_tmp1BC,sizeof(char),_get_zero_arr_size(_tmp1BC,1));}),({const
char*_tmp1BD="";_tag_dynforward(_tmp1BD,sizeof(char),_get_zero_arr_size(_tmp1BD,
1));}),({const char*_tmp1BE=",";_tag_dynforward(_tmp1BE,sizeof(char),
_get_zero_arr_size(_tmp1BE,2));}),_tmp1AA);if(effopt != 0  && Cyc_Absynpp_print_all_effects)
_tmp1B4=({struct Cyc_PP_Doc*_tmp1B5[3];_tmp1B5[2]=Cyc_Absynpp_eff2doc((void*)
effopt->v);_tmp1B5[1]=Cyc_PP_text(({const char*_tmp1B6=";";_tag_dynforward(
_tmp1B6,sizeof(char),_get_zero_arr_size(_tmp1B6,2));}));_tmp1B5[0]=_tmp1B4;Cyc_PP_cat(
_tag_dynforward(_tmp1B5,sizeof(struct Cyc_PP_Doc*),3));});if(rgn_po != 0)_tmp1B4=({
struct Cyc_PP_Doc*_tmp1B7[3];_tmp1B7[2]=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp1B7[1]=
Cyc_PP_text(({const char*_tmp1B8=":";_tag_dynforward(_tmp1B8,sizeof(char),
_get_zero_arr_size(_tmp1B8,2));}));_tmp1B7[0]=_tmp1B4;Cyc_PP_cat(_tag_dynforward(
_tmp1B7,sizeof(struct Cyc_PP_Doc*),3));});return({struct Cyc_PP_Doc*_tmp1B9[3];
_tmp1B9[2]=Cyc_PP_text(({const char*_tmp1BB=")";_tag_dynforward(_tmp1BB,sizeof(
char),_get_zero_arr_size(_tmp1BB,2));}));_tmp1B9[1]=_tmp1B4;_tmp1B9[0]=Cyc_PP_text(({
const char*_tmp1BA="(";_tag_dynforward(_tmp1BA,sizeof(char),_get_zero_arr_size(
_tmp1BA,2));}));Cyc_PP_cat(_tag_dynforward(_tmp1B9,sizeof(struct Cyc_PP_Doc*),3));});}}
struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple6*arg){return({struct _tuple1*
_tmp1BF=_cycalloc(sizeof(*_tmp1BF));_tmp1BF->f1=({struct Cyc_Core_Opt*_tmp1C0=
_cycalloc(sizeof(*_tmp1C0));_tmp1C0->v=(*arg).f1;_tmp1C0;});_tmp1BF->f2=(*arg).f2;
_tmp1BF->f3=(*arg).f3;_tmp1BF;});}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct
_dynforward_ptr*v){return Cyc_PP_text(*v);}struct _dynforward_ptr Cyc_Absynpp_qvar2string(
struct _tuple0*q){struct Cyc_List_List*_tmp1C1=0;int match;{union Cyc_Absyn_Nmspace_union
_tmp1C2=(*q).f1;struct Cyc_List_List*_tmp1C3;struct Cyc_List_List*_tmp1C4;_LL121:
if((_tmp1C2.Loc_n).tag != 0)goto _LL123;_LL122: _tmp1C3=0;goto _LL124;_LL123: if((
_tmp1C2.Rel_n).tag != 1)goto _LL125;_tmp1C3=(_tmp1C2.Rel_n).f1;_LL124: match=0;
_tmp1C1=_tmp1C3;goto _LL120;_LL125: if((_tmp1C2.Abs_n).tag != 2)goto _LL120;_tmp1C4=(
_tmp1C2.Abs_n).f1;_LL126: match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*
cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1C4,Cyc_Absynpp_curr_namespace);
_tmp1C1=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*
_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5->hd=Cyc_Absynpp_cyc_stringptr;
_tmp1C5->tl=_tmp1C4;_tmp1C5;}): _tmp1C4;goto _LL120;_LL120:;}if(Cyc_Absynpp_qvar_to_Cids)
return(struct _dynforward_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1C1,({struct Cyc_List_List*_tmp1C6=
_cycalloc(sizeof(*_tmp1C6));_tmp1C6->hd=(*q).f2;_tmp1C6->tl=0;_tmp1C6;})),({
const char*_tmp1C7="_";_tag_dynforward(_tmp1C7,sizeof(char),_get_zero_arr_size(
_tmp1C7,2));}));else{if(match)return*(*q).f2;else{return(struct _dynforward_ptr)
Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1C1,({struct Cyc_List_List*_tmp1C8=_cycalloc(sizeof(*
_tmp1C8));_tmp1C8->hd=(*q).f2;_tmp1C8->tl=0;_tmp1C8;})),({const char*_tmp1C9="::";
_tag_dynforward(_tmp1C9,sizeof(char),_get_zero_arr_size(_tmp1C9,3));}));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){return Cyc_PP_text(Cyc_Absynpp_qvar2string(
q));}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){struct
_dynforward_ptr _tmp1CA=Cyc_Absynpp_qvar2string(q);if(Cyc_PP_tex_output)return Cyc_PP_text_width((
struct _dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)Cyc_strconcat(({const
char*_tmp1CB="\\textbf{";_tag_dynforward(_tmp1CB,sizeof(char),_get_zero_arr_size(
_tmp1CB,9));}),(struct _dynforward_ptr)_tmp1CA),({const char*_tmp1CC="}";
_tag_dynforward(_tmp1CC,sizeof(char),_get_zero_arr_size(_tmp1CC,2));})),(int)Cyc_strlen((
struct _dynforward_ptr)_tmp1CA));else{return Cyc_PP_text(_tmp1CA);}}struct
_dynforward_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)
return Cyc_Absynpp_qvar2string(v);if(Cyc_Absynpp_use_curr_namespace){union Cyc_Absyn_Nmspace_union
_tmp1CD=(*v).f1;struct Cyc_List_List*_tmp1CE;struct Cyc_List_List*_tmp1CF;_LL128:
if((_tmp1CD.Loc_n).tag != 0)goto _LL12A;_LL129: goto _LL12B;_LL12A: if((_tmp1CD.Rel_n).tag
!= 1)goto _LL12C;_tmp1CE=(_tmp1CD.Rel_n).f1;if(_tmp1CE != 0)goto _LL12C;_LL12B:
return*(*v).f2;_LL12C: if((_tmp1CD.Abs_n).tag != 2)goto _LL12E;_tmp1CF=(_tmp1CD.Abs_n).f1;
_LL12D: if(((int(*)(int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,
_tmp1CF,Cyc_Absynpp_curr_namespace)== 0)return*(*v).f2;else{goto _LL12F;}_LL12E:;
_LL12F: return(struct _dynforward_ptr)Cyc_strconcat(({const char*_tmp1D0="/* bad namespace : */ ";
_tag_dynforward(_tmp1D0,sizeof(char),_get_zero_arr_size(_tmp1D0,23));}),(struct
_dynforward_ptr)Cyc_Absynpp_qvar2string(v));_LL127:;}else{return*(*v).f2;}}
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){return Cyc_PP_text(
Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v){struct _dynforward_ptr _tmp1D1=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output)return Cyc_PP_text_width((struct _dynforward_ptr)Cyc_strconcat((
struct _dynforward_ptr)Cyc_strconcat(({const char*_tmp1D2="\\textbf{";
_tag_dynforward(_tmp1D2,sizeof(char),_get_zero_arr_size(_tmp1D2,9));}),(struct
_dynforward_ptr)_tmp1D1),({const char*_tmp1D3="}";_tag_dynforward(_tmp1D3,sizeof(
char),_get_zero_arr_size(_tmp1D3,2));})),(int)Cyc_strlen((struct _dynforward_ptr)
_tmp1D1));else{return Cyc_PP_text(_tmp1D1);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*t){return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*e){void*_tmp1D4=(void*)e->r;void*_tmp1D5;struct Cyc_Absyn_Exp*
_tmp1D6;struct Cyc_Absyn_Exp*_tmp1D7;_LL131: if(*((int*)_tmp1D4)!= 0)goto _LL133;
_LL132: goto _LL134;_LL133: if(*((int*)_tmp1D4)!= 1)goto _LL135;_LL134: goto _LL136;
_LL135: if(*((int*)_tmp1D4)!= 2)goto _LL137;_LL136: return 10000;_LL137: if(*((int*)
_tmp1D4)!= 3)goto _LL139;_tmp1D5=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D4)->f1;_LL138: {void*_tmp1D8=_tmp1D5;_LL17E: if((int)_tmp1D8 != 0)goto _LL180;
_LL17F: return 100;_LL180: if((int)_tmp1D8 != 1)goto _LL182;_LL181: return 110;_LL182:
if((int)_tmp1D8 != 2)goto _LL184;_LL183: return 100;_LL184: if((int)_tmp1D8 != 3)goto
_LL186;_LL185: goto _LL187;_LL186: if((int)_tmp1D8 != 4)goto _LL188;_LL187: return 110;
_LL188: if((int)_tmp1D8 != 5)goto _LL18A;_LL189: goto _LL18B;_LL18A: if((int)_tmp1D8 != 
6)goto _LL18C;_LL18B: return 70;_LL18C: if((int)_tmp1D8 != 7)goto _LL18E;_LL18D: goto
_LL18F;_LL18E: if((int)_tmp1D8 != 8)goto _LL190;_LL18F: goto _LL191;_LL190: if((int)
_tmp1D8 != 9)goto _LL192;_LL191: goto _LL193;_LL192: if((int)_tmp1D8 != 10)goto _LL194;
_LL193: return 80;_LL194: if((int)_tmp1D8 != 11)goto _LL196;_LL195: goto _LL197;_LL196:
if((int)_tmp1D8 != 12)goto _LL198;_LL197: return 130;_LL198: if((int)_tmp1D8 != 13)
goto _LL19A;_LL199: return 60;_LL19A: if((int)_tmp1D8 != 14)goto _LL19C;_LL19B: return
40;_LL19C: if((int)_tmp1D8 != 15)goto _LL19E;_LL19D: return 50;_LL19E: if((int)_tmp1D8
!= 16)goto _LL1A0;_LL19F: return 90;_LL1A0: if((int)_tmp1D8 != 17)goto _LL1A2;_LL1A1:
return 80;_LL1A2: if((int)_tmp1D8 != 18)goto _LL1A4;_LL1A3: return 80;_LL1A4: if((int)
_tmp1D8 != 19)goto _LL17D;_LL1A5: return 140;_LL17D:;}_LL139: if(*((int*)_tmp1D4)!= 4)
goto _LL13B;_LL13A: return 20;_LL13B: if(*((int*)_tmp1D4)!= 5)goto _LL13D;_LL13C:
return 130;_LL13D: if(*((int*)_tmp1D4)!= 6)goto _LL13F;_LL13E: return 30;_LL13F: if(*((
int*)_tmp1D4)!= 7)goto _LL141;_LL140: return 35;_LL141: if(*((int*)_tmp1D4)!= 8)goto
_LL143;_LL142: return 30;_LL143: if(*((int*)_tmp1D4)!= 9)goto _LL145;_LL144: return 10;
_LL145: if(*((int*)_tmp1D4)!= 10)goto _LL147;_LL146: goto _LL148;_LL147: if(*((int*)
_tmp1D4)!= 11)goto _LL149;_LL148: return 140;_LL149: if(*((int*)_tmp1D4)!= 12)goto
_LL14B;_LL14A: return 130;_LL14B: if(*((int*)_tmp1D4)!= 13)goto _LL14D;_tmp1D6=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1D4)->f1;_LL14C: return Cyc_Absynpp_exp_prec(
_tmp1D6);_LL14D: if(*((int*)_tmp1D4)!= 14)goto _LL14F;_tmp1D7=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1D4)->f1;_LL14E: return Cyc_Absynpp_exp_prec(_tmp1D7);_LL14F: if(*((int*)
_tmp1D4)!= 15)goto _LL151;_LL150: return 120;_LL151: if(*((int*)_tmp1D4)!= 17)goto
_LL153;_LL152: return 15;_LL153: if(*((int*)_tmp1D4)!= 16)goto _LL155;_LL154: goto
_LL156;_LL155: if(*((int*)_tmp1D4)!= 18)goto _LL157;_LL156: goto _LL158;_LL157: if(*((
int*)_tmp1D4)!= 19)goto _LL159;_LL158: goto _LL15A;_LL159: if(*((int*)_tmp1D4)!= 20)
goto _LL15B;_LL15A: goto _LL15C;_LL15B: if(*((int*)_tmp1D4)!= 21)goto _LL15D;_LL15C:
goto _LL15E;_LL15D: if(*((int*)_tmp1D4)!= 22)goto _LL15F;_LL15E: return 130;_LL15F:
if(*((int*)_tmp1D4)!= 23)goto _LL161;_LL160: goto _LL162;_LL161: if(*((int*)_tmp1D4)
!= 24)goto _LL163;_LL162: goto _LL164;_LL163: if(*((int*)_tmp1D4)!= 25)goto _LL165;
_LL164: return 140;_LL165: if(*((int*)_tmp1D4)!= 26)goto _LL167;_LL166: return 150;
_LL167: if(*((int*)_tmp1D4)!= 27)goto _LL169;_LL168: goto _LL16A;_LL169: if(*((int*)
_tmp1D4)!= 28)goto _LL16B;_LL16A: goto _LL16C;_LL16B: if(*((int*)_tmp1D4)!= 29)goto
_LL16D;_LL16C: goto _LL16E;_LL16D: if(*((int*)_tmp1D4)!= 30)goto _LL16F;_LL16E: goto
_LL170;_LL16F: if(*((int*)_tmp1D4)!= 31)goto _LL171;_LL170: goto _LL172;_LL171: if(*((
int*)_tmp1D4)!= 32)goto _LL173;_LL172: goto _LL174;_LL173: if(*((int*)_tmp1D4)!= 33)
goto _LL175;_LL174: goto _LL176;_LL175: if(*((int*)_tmp1D4)!= 34)goto _LL177;_LL176:
goto _LL178;_LL177: if(*((int*)_tmp1D4)!= 35)goto _LL179;_LL178: goto _LL17A;_LL179:
if(*((int*)_tmp1D4)!= 36)goto _LL17B;_LL17A: return 140;_LL17B: if(*((int*)_tmp1D4)
!= 37)goto _LL130;_LL17C: return 10000;_LL130:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(
struct Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmp1D9=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp1DA;struct _tuple0*
_tmp1DB;struct _tuple0*_tmp1DC;void*_tmp1DD;struct Cyc_List_List*_tmp1DE;struct Cyc_Absyn_Exp*
_tmp1DF;struct Cyc_Core_Opt*_tmp1E0;struct Cyc_Absyn_Exp*_tmp1E1;struct Cyc_Absyn_Exp*
_tmp1E2;void*_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E4;struct Cyc_Absyn_Exp*_tmp1E5;
struct Cyc_Absyn_Exp*_tmp1E6;struct Cyc_Absyn_Exp*_tmp1E7;struct Cyc_Absyn_Exp*
_tmp1E8;struct Cyc_Absyn_Exp*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1EA;struct Cyc_Absyn_Exp*
_tmp1EB;struct Cyc_Absyn_Exp*_tmp1EC;struct Cyc_Absyn_Exp*_tmp1ED;struct Cyc_List_List*
_tmp1EE;struct Cyc_Absyn_Exp*_tmp1EF;struct Cyc_List_List*_tmp1F0;struct Cyc_Absyn_Exp*
_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F2;struct Cyc_Absyn_Exp*_tmp1F3;void*_tmp1F4;
struct Cyc_Absyn_Exp*_tmp1F5;struct Cyc_Absyn_Exp*_tmp1F6;struct Cyc_Absyn_Exp*
_tmp1F7;struct Cyc_Absyn_Exp*_tmp1F8;void*_tmp1F9;struct Cyc_Absyn_Exp*_tmp1FA;
void*_tmp1FB;void*_tmp1FC;struct _dynforward_ptr*_tmp1FD;void*_tmp1FE;void*
_tmp1FF;unsigned int _tmp200;struct Cyc_List_List*_tmp201;void*_tmp202;struct Cyc_Absyn_Exp*
_tmp203;struct Cyc_Absyn_Exp*_tmp204;struct _dynforward_ptr*_tmp205;struct Cyc_Absyn_Exp*
_tmp206;struct _dynforward_ptr*_tmp207;struct Cyc_Absyn_Exp*_tmp208;struct Cyc_Absyn_Exp*
_tmp209;struct Cyc_List_List*_tmp20A;struct _tuple1*_tmp20B;struct Cyc_List_List*
_tmp20C;struct Cyc_List_List*_tmp20D;struct Cyc_Absyn_Vardecl*_tmp20E;struct Cyc_Absyn_Exp*
_tmp20F;struct Cyc_Absyn_Exp*_tmp210;struct _tuple0*_tmp211;struct Cyc_List_List*
_tmp212;struct Cyc_List_List*_tmp213;struct Cyc_List_List*_tmp214;struct Cyc_List_List*
_tmp215;struct Cyc_Absyn_Tunionfield*_tmp216;struct _tuple0*_tmp217;struct _tuple0*
_tmp218;struct Cyc_Absyn_MallocInfo _tmp219;int _tmp21A;struct Cyc_Absyn_Exp*_tmp21B;
void**_tmp21C;struct Cyc_Absyn_Exp*_tmp21D;struct Cyc_Core_Opt*_tmp21E;struct Cyc_List_List*
_tmp21F;struct Cyc_Absyn_Stmt*_tmp220;_LL1A7: if(*((int*)_tmp1D9)!= 0)goto _LL1A9;
_tmp1DA=((struct Cyc_Absyn_Const_e_struct*)_tmp1D9)->f1;_LL1A8: s=Cyc_Absynpp_cnst2doc(
_tmp1DA);goto _LL1A6;_LL1A9: if(*((int*)_tmp1D9)!= 1)goto _LL1AB;_tmp1DB=((struct
Cyc_Absyn_Var_e_struct*)_tmp1D9)->f1;_LL1AA: _tmp1DC=_tmp1DB;goto _LL1AC;_LL1AB:
if(*((int*)_tmp1D9)!= 2)goto _LL1AD;_tmp1DC=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp1D9)->f1;_LL1AC: s=Cyc_Absynpp_qvar2doc(_tmp1DC);goto _LL1A6;_LL1AD: if(*((int*)
_tmp1D9)!= 3)goto _LL1AF;_tmp1DD=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D9)->f1;_tmp1DE=((struct Cyc_Absyn_Primop_e_struct*)_tmp1D9)->f2;_LL1AE: s=
Cyc_Absynpp_primapp2doc(myprec,_tmp1DD,_tmp1DE);goto _LL1A6;_LL1AF: if(*((int*)
_tmp1D9)!= 4)goto _LL1B1;_tmp1DF=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1D9)->f1;
_tmp1E0=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1D9)->f2;_tmp1E1=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1D9)->f3;_LL1B0: s=({struct Cyc_PP_Doc*_tmp221[5];_tmp221[4]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E1);_tmp221[3]=Cyc_PP_text(({const char*_tmp224="= ";_tag_dynforward(
_tmp224,sizeof(char),_get_zero_arr_size(_tmp224,3));}));_tmp221[2]=_tmp1E0 == 0?
Cyc_PP_text(({const char*_tmp223="";_tag_dynforward(_tmp223,sizeof(char),
_get_zero_arr_size(_tmp223,1));})): Cyc_Absynpp_prim2doc((void*)_tmp1E0->v);
_tmp221[1]=Cyc_PP_text(({const char*_tmp222=" ";_tag_dynforward(_tmp222,sizeof(
char),_get_zero_arr_size(_tmp222,2));}));_tmp221[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1DF);Cyc_PP_cat(_tag_dynforward(_tmp221,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL1A6;_LL1B1: if(*((int*)_tmp1D9)!= 5)goto _LL1B3;_tmp1E2=((struct Cyc_Absyn_Increment_e_struct*)
_tmp1D9)->f1;_tmp1E3=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp1D9)->f2;
_LL1B2: {struct Cyc_PP_Doc*_tmp225=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E2);{void*
_tmp226=_tmp1E3;_LL1F6: if((int)_tmp226 != 0)goto _LL1F8;_LL1F7: s=({struct Cyc_PP_Doc*
_tmp227[2];_tmp227[1]=_tmp225;_tmp227[0]=Cyc_PP_text(({const char*_tmp228="++";
_tag_dynforward(_tmp228,sizeof(char),_get_zero_arr_size(_tmp228,3));}));Cyc_PP_cat(
_tag_dynforward(_tmp227,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1F5;_LL1F8: if((
int)_tmp226 != 2)goto _LL1FA;_LL1F9: s=({struct Cyc_PP_Doc*_tmp229[2];_tmp229[1]=
_tmp225;_tmp229[0]=Cyc_PP_text(({const char*_tmp22A="--";_tag_dynforward(_tmp22A,
sizeof(char),_get_zero_arr_size(_tmp22A,3));}));Cyc_PP_cat(_tag_dynforward(
_tmp229,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1F5;_LL1FA: if((int)_tmp226 != 1)
goto _LL1FC;_LL1FB: s=({struct Cyc_PP_Doc*_tmp22B[2];_tmp22B[1]=Cyc_PP_text(({const
char*_tmp22C="++";_tag_dynforward(_tmp22C,sizeof(char),_get_zero_arr_size(
_tmp22C,3));}));_tmp22B[0]=_tmp225;Cyc_PP_cat(_tag_dynforward(_tmp22B,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL1F5;_LL1FC: if((int)_tmp226 != 3)goto _LL1F5;
_LL1FD: s=({struct Cyc_PP_Doc*_tmp22D[2];_tmp22D[1]=Cyc_PP_text(({const char*
_tmp22E="--";_tag_dynforward(_tmp22E,sizeof(char),_get_zero_arr_size(_tmp22E,3));}));
_tmp22D[0]=_tmp225;Cyc_PP_cat(_tag_dynforward(_tmp22D,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL1F5;_LL1F5:;}goto _LL1A6;}_LL1B3: if(*((int*)_tmp1D9)!= 6)goto _LL1B5;
_tmp1E4=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1D9)->f1;_tmp1E5=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp1D9)->f2;_tmp1E6=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1D9)->f3;_LL1B4:
s=({struct Cyc_PP_Doc*_tmp22F[5];_tmp22F[4]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp1E6);_tmp22F[3]=Cyc_PP_text(({const char*_tmp231=" : ";_tag_dynforward(
_tmp231,sizeof(char),_get_zero_arr_size(_tmp231,4));}));_tmp22F[2]=Cyc_Absynpp_exp2doc_prec(
0,_tmp1E5);_tmp22F[1]=Cyc_PP_text(({const char*_tmp230=" ? ";_tag_dynforward(
_tmp230,sizeof(char),_get_zero_arr_size(_tmp230,4));}));_tmp22F[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E4);Cyc_PP_cat(_tag_dynforward(_tmp22F,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL1A6;_LL1B5: if(*((int*)_tmp1D9)!= 7)goto _LL1B7;_tmp1E7=((struct Cyc_Absyn_And_e_struct*)
_tmp1D9)->f1;_tmp1E8=((struct Cyc_Absyn_And_e_struct*)_tmp1D9)->f2;_LL1B6: s=({
struct Cyc_PP_Doc*_tmp232[3];_tmp232[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E8);
_tmp232[1]=Cyc_PP_text(({const char*_tmp233=" && ";_tag_dynforward(_tmp233,
sizeof(char),_get_zero_arr_size(_tmp233,5));}));_tmp232[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E7);Cyc_PP_cat(_tag_dynforward(_tmp232,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1A6;_LL1B7: if(*((int*)_tmp1D9)!= 8)goto _LL1B9;_tmp1E9=((struct Cyc_Absyn_Or_e_struct*)
_tmp1D9)->f1;_tmp1EA=((struct Cyc_Absyn_Or_e_struct*)_tmp1D9)->f2;_LL1B8: s=({
struct Cyc_PP_Doc*_tmp234[3];_tmp234[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1EA);
_tmp234[1]=Cyc_PP_text(({const char*_tmp235=" || ";_tag_dynforward(_tmp235,
sizeof(char),_get_zero_arr_size(_tmp235,5));}));_tmp234[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E9);Cyc_PP_cat(_tag_dynforward(_tmp234,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1A6;_LL1B9: if(*((int*)_tmp1D9)!= 9)goto _LL1BB;_tmp1EB=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1D9)->f1;_tmp1EC=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp1D9)->f2;_LL1BA: s=({
struct Cyc_PP_Doc*_tmp236[5];_tmp236[4]=Cyc_PP_text(({const char*_tmp239=")";
_tag_dynforward(_tmp239,sizeof(char),_get_zero_arr_size(_tmp239,2));}));_tmp236[
3]=Cyc_Absynpp_exp2doc(_tmp1EC);_tmp236[2]=Cyc_PP_text(({const char*_tmp238=", ";
_tag_dynforward(_tmp238,sizeof(char),_get_zero_arr_size(_tmp238,3));}));_tmp236[
1]=Cyc_Absynpp_exp2doc(_tmp1EB);_tmp236[0]=Cyc_PP_text(({const char*_tmp237="(";
_tag_dynforward(_tmp237,sizeof(char),_get_zero_arr_size(_tmp237,2));}));Cyc_PP_cat(
_tag_dynforward(_tmp236,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1A6;_LL1BB: if(*((
int*)_tmp1D9)!= 10)goto _LL1BD;_tmp1ED=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp1D9)->f1;_tmp1EE=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1D9)->f2;_LL1BC:
s=({struct Cyc_PP_Doc*_tmp23A[4];_tmp23A[3]=Cyc_PP_text(({const char*_tmp23C=")";
_tag_dynforward(_tmp23C,sizeof(char),_get_zero_arr_size(_tmp23C,2));}));_tmp23A[
2]=Cyc_Absynpp_exps2doc_prec(20,_tmp1EE);_tmp23A[1]=Cyc_PP_text(({const char*
_tmp23B="(";_tag_dynforward(_tmp23B,sizeof(char),_get_zero_arr_size(_tmp23B,2));}));
_tmp23A[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1ED);Cyc_PP_cat(_tag_dynforward(
_tmp23A,sizeof(struct Cyc_PP_Doc*),4));});goto _LL1A6;_LL1BD: if(*((int*)_tmp1D9)!= 
11)goto _LL1BF;_tmp1EF=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1D9)->f1;_tmp1F0=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp1D9)->f2;_LL1BE: s=({struct Cyc_PP_Doc*
_tmp23D[4];_tmp23D[3]=Cyc_PP_text(({const char*_tmp23F=")";_tag_dynforward(
_tmp23F,sizeof(char),_get_zero_arr_size(_tmp23F,2));}));_tmp23D[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp1F0);_tmp23D[1]=Cyc_PP_text(({const char*_tmp23E="(";_tag_dynforward(
_tmp23E,sizeof(char),_get_zero_arr_size(_tmp23E,2));}));_tmp23D[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EF);Cyc_PP_cat(_tag_dynforward(_tmp23D,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL1A6;_LL1BF: if(*((int*)_tmp1D9)!= 12)goto _LL1C1;_tmp1F1=((struct Cyc_Absyn_Throw_e_struct*)
_tmp1D9)->f1;_LL1C0: s=({struct Cyc_PP_Doc*_tmp240[2];_tmp240[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F1);_tmp240[0]=Cyc_PP_text(({const char*_tmp241="throw ";
_tag_dynforward(_tmp241,sizeof(char),_get_zero_arr_size(_tmp241,7));}));Cyc_PP_cat(
_tag_dynforward(_tmp240,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1A6;_LL1C1: if(*((
int*)_tmp1D9)!= 13)goto _LL1C3;_tmp1F2=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp1D9)->f1;_LL1C2: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1F2);goto _LL1A6;_LL1C3:
if(*((int*)_tmp1D9)!= 14)goto _LL1C5;_tmp1F3=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1D9)->f1;_LL1C4: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1F3);goto _LL1A6;_LL1C5:
if(*((int*)_tmp1D9)!= 15)goto _LL1C7;_tmp1F4=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp1D9)->f1;_tmp1F5=((struct Cyc_Absyn_Cast_e_struct*)_tmp1D9)->f2;_LL1C6: s=({
struct Cyc_PP_Doc*_tmp242[4];_tmp242[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F5);
_tmp242[2]=Cyc_PP_text(({const char*_tmp244=")";_tag_dynforward(_tmp244,sizeof(
char),_get_zero_arr_size(_tmp244,2));}));_tmp242[1]=Cyc_Absynpp_typ2doc(_tmp1F4);
_tmp242[0]=Cyc_PP_text(({const char*_tmp243="(";_tag_dynforward(_tmp243,sizeof(
char),_get_zero_arr_size(_tmp243,2));}));Cyc_PP_cat(_tag_dynforward(_tmp242,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL1A6;_LL1C7: if(*((int*)_tmp1D9)!= 16)goto
_LL1C9;_tmp1F6=((struct Cyc_Absyn_Address_e_struct*)_tmp1D9)->f1;_LL1C8: s=({
struct Cyc_PP_Doc*_tmp245[2];_tmp245[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F6);
_tmp245[0]=Cyc_PP_text(({const char*_tmp246="&";_tag_dynforward(_tmp246,sizeof(
char),_get_zero_arr_size(_tmp246,2));}));Cyc_PP_cat(_tag_dynforward(_tmp245,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL1A6;_LL1C9: if(*((int*)_tmp1D9)!= 17)goto
_LL1CB;_tmp1F7=((struct Cyc_Absyn_New_e_struct*)_tmp1D9)->f1;_tmp1F8=((struct Cyc_Absyn_New_e_struct*)
_tmp1D9)->f2;_LL1CA: if(_tmp1F7 == 0)s=({struct Cyc_PP_Doc*_tmp247[2];_tmp247[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F8);_tmp247[0]=Cyc_PP_text(({const char*
_tmp248="new ";_tag_dynforward(_tmp248,sizeof(char),_get_zero_arr_size(_tmp248,5));}));
Cyc_PP_cat(_tag_dynforward(_tmp247,sizeof(struct Cyc_PP_Doc*),2));});else{s=({
struct Cyc_PP_Doc*_tmp249[4];_tmp249[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F8);
_tmp249[2]=Cyc_PP_text(({const char*_tmp24B=") ";_tag_dynforward(_tmp24B,sizeof(
char),_get_zero_arr_size(_tmp24B,3));}));_tmp249[1]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_tmp1F7);_tmp249[0]=Cyc_PP_text(({const char*_tmp24A="rnew(";
_tag_dynforward(_tmp24A,sizeof(char),_get_zero_arr_size(_tmp24A,6));}));Cyc_PP_cat(
_tag_dynforward(_tmp249,sizeof(struct Cyc_PP_Doc*),4));});}goto _LL1A6;_LL1CB: if(*((
int*)_tmp1D9)!= 18)goto _LL1CD;_tmp1F9=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp1D9)->f1;_LL1CC: s=({struct Cyc_PP_Doc*_tmp24C[3];_tmp24C[2]=Cyc_PP_text(({
const char*_tmp24E=")";_tag_dynforward(_tmp24E,sizeof(char),_get_zero_arr_size(
_tmp24E,2));}));_tmp24C[1]=Cyc_Absynpp_typ2doc(_tmp1F9);_tmp24C[0]=Cyc_PP_text(({
const char*_tmp24D="sizeof(";_tag_dynforward(_tmp24D,sizeof(char),
_get_zero_arr_size(_tmp24D,8));}));Cyc_PP_cat(_tag_dynforward(_tmp24C,sizeof(
struct Cyc_PP_Doc*),3));});goto _LL1A6;_LL1CD: if(*((int*)_tmp1D9)!= 19)goto _LL1CF;
_tmp1FA=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp1D9)->f1;_LL1CE: s=({struct Cyc_PP_Doc*
_tmp24F[3];_tmp24F[2]=Cyc_PP_text(({const char*_tmp251=")";_tag_dynforward(
_tmp251,sizeof(char),_get_zero_arr_size(_tmp251,2));}));_tmp24F[1]=Cyc_Absynpp_exp2doc(
_tmp1FA);_tmp24F[0]=Cyc_PP_text(({const char*_tmp250="sizeof(";_tag_dynforward(
_tmp250,sizeof(char),_get_zero_arr_size(_tmp250,8));}));Cyc_PP_cat(
_tag_dynforward(_tmp24F,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1A6;_LL1CF: if(*((
int*)_tmp1D9)!= 20)goto _LL1D1;_tmp1FB=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1D9)->f1;_tmp1FC=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D9)->f2;
if(*((int*)_tmp1FC)!= 0)goto _LL1D1;_tmp1FD=((struct Cyc_Absyn_StructField_struct*)
_tmp1FC)->f1;_LL1D0: s=({struct Cyc_PP_Doc*_tmp252[5];_tmp252[4]=Cyc_PP_text(({
const char*_tmp255=")";_tag_dynforward(_tmp255,sizeof(char),_get_zero_arr_size(
_tmp255,2));}));_tmp252[3]=Cyc_PP_textptr(_tmp1FD);_tmp252[2]=Cyc_PP_text(({
const char*_tmp254=",";_tag_dynforward(_tmp254,sizeof(char),_get_zero_arr_size(
_tmp254,2));}));_tmp252[1]=Cyc_Absynpp_typ2doc(_tmp1FB);_tmp252[0]=Cyc_PP_text(({
const char*_tmp253="offsetof(";_tag_dynforward(_tmp253,sizeof(char),
_get_zero_arr_size(_tmp253,10));}));Cyc_PP_cat(_tag_dynforward(_tmp252,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL1A6;_LL1D1: if(*((int*)_tmp1D9)!= 20)goto _LL1D3;
_tmp1FE=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D9)->f1;_tmp1FF=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D9)->f2;if(*((int*)_tmp1FF)!= 1)goto
_LL1D3;_tmp200=((struct Cyc_Absyn_TupleIndex_struct*)_tmp1FF)->f1;_LL1D2: s=({
struct Cyc_PP_Doc*_tmp256[5];_tmp256[4]=Cyc_PP_text(({const char*_tmp25C=")";
_tag_dynforward(_tmp25C,sizeof(char),_get_zero_arr_size(_tmp25C,2));}));_tmp256[
3]=Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp25B;_tmp25B.tag=
1;_tmp25B.f1=(unsigned long)((int)_tmp200);{void*_tmp259[1]={& _tmp25B};Cyc_aprintf(({
const char*_tmp25A="%d";_tag_dynforward(_tmp25A,sizeof(char),_get_zero_arr_size(
_tmp25A,3));}),_tag_dynforward(_tmp259,sizeof(void*),1));}}));_tmp256[2]=Cyc_PP_text(({
const char*_tmp258=",";_tag_dynforward(_tmp258,sizeof(char),_get_zero_arr_size(
_tmp258,2));}));_tmp256[1]=Cyc_Absynpp_typ2doc(_tmp1FE);_tmp256[0]=Cyc_PP_text(({
const char*_tmp257="offsetof(";_tag_dynforward(_tmp257,sizeof(char),
_get_zero_arr_size(_tmp257,10));}));Cyc_PP_cat(_tag_dynforward(_tmp256,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL1A6;_LL1D3: if(*((int*)_tmp1D9)!= 21)goto _LL1D5;
_tmp201=((struct Cyc_Absyn_Gentyp_e_struct*)_tmp1D9)->f1;_tmp202=(void*)((struct
Cyc_Absyn_Gentyp_e_struct*)_tmp1D9)->f2;_LL1D4: s=({struct Cyc_PP_Doc*_tmp25D[4];
_tmp25D[3]=Cyc_PP_text(({const char*_tmp25F=")";_tag_dynforward(_tmp25F,sizeof(
char),_get_zero_arr_size(_tmp25F,2));}));_tmp25D[2]=Cyc_Absynpp_typ2doc(_tmp202);
_tmp25D[1]=Cyc_Absynpp_tvars2doc(_tmp201);_tmp25D[0]=Cyc_PP_text(({const char*
_tmp25E="__gen(";_tag_dynforward(_tmp25E,sizeof(char),_get_zero_arr_size(_tmp25E,
7));}));Cyc_PP_cat(_tag_dynforward(_tmp25D,sizeof(struct Cyc_PP_Doc*),4));});goto
_LL1A6;_LL1D5: if(*((int*)_tmp1D9)!= 22)goto _LL1D7;_tmp203=((struct Cyc_Absyn_Deref_e_struct*)
_tmp1D9)->f1;_LL1D6: s=({struct Cyc_PP_Doc*_tmp260[2];_tmp260[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp203);_tmp260[0]=Cyc_PP_text(({const char*_tmp261="*";_tag_dynforward(
_tmp261,sizeof(char),_get_zero_arr_size(_tmp261,2));}));Cyc_PP_cat(
_tag_dynforward(_tmp260,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1A6;_LL1D7: if(*((
int*)_tmp1D9)!= 23)goto _LL1D9;_tmp204=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp1D9)->f1;_tmp205=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1D9)->f2;_LL1D8:
s=({struct Cyc_PP_Doc*_tmp262[3];_tmp262[2]=Cyc_PP_textptr(_tmp205);_tmp262[1]=
Cyc_PP_text(({const char*_tmp263=".";_tag_dynforward(_tmp263,sizeof(char),
_get_zero_arr_size(_tmp263,2));}));_tmp262[0]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp204);Cyc_PP_cat(_tag_dynforward(_tmp262,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1A6;_LL1D9: if(*((int*)_tmp1D9)!= 24)goto _LL1DB;_tmp206=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp1D9)->f1;_tmp207=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1D9)->f2;_LL1DA: s=({
struct Cyc_PP_Doc*_tmp264[3];_tmp264[2]=Cyc_PP_textptr(_tmp207);_tmp264[1]=Cyc_PP_text(({
const char*_tmp265="->";_tag_dynforward(_tmp265,sizeof(char),_get_zero_arr_size(
_tmp265,3));}));_tmp264[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp206);Cyc_PP_cat(
_tag_dynforward(_tmp264,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1A6;_LL1DB: if(*((
int*)_tmp1D9)!= 25)goto _LL1DD;_tmp208=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1D9)->f1;_tmp209=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1D9)->f2;_LL1DC: s=({
struct Cyc_PP_Doc*_tmp266[4];_tmp266[3]=Cyc_PP_text(({const char*_tmp268="]";
_tag_dynforward(_tmp268,sizeof(char),_get_zero_arr_size(_tmp268,2));}));_tmp266[
2]=Cyc_Absynpp_exp2doc(_tmp209);_tmp266[1]=Cyc_PP_text(({const char*_tmp267="[";
_tag_dynforward(_tmp267,sizeof(char),_get_zero_arr_size(_tmp267,2));}));_tmp266[
0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp208);Cyc_PP_cat(_tag_dynforward(_tmp266,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL1A6;_LL1DD: if(*((int*)_tmp1D9)!= 26)goto
_LL1DF;_tmp20A=((struct Cyc_Absyn_Tuple_e_struct*)_tmp1D9)->f1;_LL1DE: s=({struct
Cyc_PP_Doc*_tmp269[4];_tmp269[3]=Cyc_PP_text(({const char*_tmp26B=")";
_tag_dynforward(_tmp26B,sizeof(char),_get_zero_arr_size(_tmp26B,2));}));_tmp269[
2]=Cyc_Absynpp_exps2doc_prec(20,_tmp20A);_tmp269[1]=Cyc_PP_text(({const char*
_tmp26A="(";_tag_dynforward(_tmp26A,sizeof(char),_get_zero_arr_size(_tmp26A,2));}));
_tmp269[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dynforward(_tmp269,sizeof(struct
Cyc_PP_Doc*),4));});goto _LL1A6;_LL1DF: if(*((int*)_tmp1D9)!= 27)goto _LL1E1;
_tmp20B=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp1D9)->f1;_tmp20C=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp1D9)->f2;_LL1E0: s=({struct Cyc_PP_Doc*_tmp26C[4];_tmp26C[3]=Cyc_Absynpp_group_braces(({
const char*_tmp26F=",";_tag_dynforward(_tmp26F,sizeof(char),_get_zero_arr_size(
_tmp26F,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp20C));_tmp26C[2]=Cyc_PP_text(({
const char*_tmp26E=")";_tag_dynforward(_tmp26E,sizeof(char),_get_zero_arr_size(
_tmp26E,2));}));_tmp26C[1]=Cyc_Absynpp_typ2doc((*_tmp20B).f3);_tmp26C[0]=Cyc_PP_text(({
const char*_tmp26D="(";_tag_dynforward(_tmp26D,sizeof(char),_get_zero_arr_size(
_tmp26D,2));}));Cyc_PP_cat(_tag_dynforward(_tmp26C,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL1A6;_LL1E1: if(*((int*)_tmp1D9)!= 28)goto _LL1E3;_tmp20D=((struct Cyc_Absyn_Array_e_struct*)
_tmp1D9)->f1;_LL1E2: s=Cyc_Absynpp_group_braces(({const char*_tmp270=",";
_tag_dynforward(_tmp270,sizeof(char),_get_zero_arr_size(_tmp270,2));}),((struct
Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp20D));goto _LL1A6;_LL1E3: if(*((int*)_tmp1D9)
!= 29)goto _LL1E5;_tmp20E=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D9)->f1;
_tmp20F=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D9)->f2;_tmp210=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1D9)->f3;_LL1E4: s=({struct Cyc_PP_Doc*
_tmp271[8];_tmp271[7]=Cyc_Absynpp_rb();_tmp271[6]=Cyc_Absynpp_exp2doc(_tmp210);
_tmp271[5]=Cyc_PP_text(({const char*_tmp274=" : ";_tag_dynforward(_tmp274,sizeof(
char),_get_zero_arr_size(_tmp274,4));}));_tmp271[4]=Cyc_Absynpp_exp2doc(_tmp20F);
_tmp271[3]=Cyc_PP_text(({const char*_tmp273=" < ";_tag_dynforward(_tmp273,sizeof(
char),_get_zero_arr_size(_tmp273,4));}));_tmp271[2]=Cyc_PP_text(*(*_tmp20E->name).f2);
_tmp271[1]=Cyc_PP_text(({const char*_tmp272="for ";_tag_dynforward(_tmp272,
sizeof(char),_get_zero_arr_size(_tmp272,5));}));_tmp271[0]=Cyc_Absynpp_lb();Cyc_PP_cat(
_tag_dynforward(_tmp271,sizeof(struct Cyc_PP_Doc*),8));});goto _LL1A6;_LL1E5: if(*((
int*)_tmp1D9)!= 30)goto _LL1E7;_tmp211=((struct Cyc_Absyn_Struct_e_struct*)_tmp1D9)->f1;
_tmp212=((struct Cyc_Absyn_Struct_e_struct*)_tmp1D9)->f2;_tmp213=((struct Cyc_Absyn_Struct_e_struct*)
_tmp1D9)->f3;_LL1E6: {struct Cyc_List_List*_tmp275=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp213);s=({struct Cyc_PP_Doc*_tmp276[2];_tmp276[1]=Cyc_Absynpp_group_braces(({
const char*_tmp277=",";_tag_dynforward(_tmp277,sizeof(char),_get_zero_arr_size(
_tmp277,2));}),_tmp212 != 0?({struct Cyc_List_List*_tmp278=_cycalloc(sizeof(*
_tmp278));_tmp278->hd=Cyc_Absynpp_tps2doc(_tmp212);_tmp278->tl=_tmp275;_tmp278;}):
_tmp275);_tmp276[0]=Cyc_Absynpp_qvar2doc(_tmp211);Cyc_PP_cat(_tag_dynforward(
_tmp276,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1A6;}_LL1E7: if(*((int*)_tmp1D9)
!= 31)goto _LL1E9;_tmp214=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1D9)->f2;
_LL1E8: s=Cyc_Absynpp_group_braces(({const char*_tmp279=",";_tag_dynforward(
_tmp279,sizeof(char),_get_zero_arr_size(_tmp279,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp214));goto _LL1A6;_LL1E9: if(*((int*)_tmp1D9)!= 32)goto _LL1EB;_tmp215=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp1D9)->f1;_tmp216=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1D9)->f3;_LL1EA: if(_tmp215 == 0)s=Cyc_Absynpp_qvar2doc(_tmp216->name);else{s=({
struct Cyc_PP_Doc*_tmp27A[2];_tmp27A[1]=Cyc_PP_egroup(({const char*_tmp27B="(";
_tag_dynforward(_tmp27B,sizeof(char),_get_zero_arr_size(_tmp27B,2));}),({const
char*_tmp27C=")";_tag_dynforward(_tmp27C,sizeof(char),_get_zero_arr_size(_tmp27C,
2));}),({const char*_tmp27D=",";_tag_dynforward(_tmp27D,sizeof(char),
_get_zero_arr_size(_tmp27D,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,
_tmp215));_tmp27A[0]=Cyc_Absynpp_qvar2doc(_tmp216->name);Cyc_PP_cat(
_tag_dynforward(_tmp27A,sizeof(struct Cyc_PP_Doc*),2));});}goto _LL1A6;_LL1EB: if(*((
int*)_tmp1D9)!= 33)goto _LL1ED;_tmp217=((struct Cyc_Absyn_Enum_e_struct*)_tmp1D9)->f1;
_LL1EC: s=Cyc_Absynpp_qvar2doc(_tmp217);goto _LL1A6;_LL1ED: if(*((int*)_tmp1D9)!= 
34)goto _LL1EF;_tmp218=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp1D9)->f1;_LL1EE: s=
Cyc_Absynpp_qvar2doc(_tmp218);goto _LL1A6;_LL1EF: if(*((int*)_tmp1D9)!= 35)goto
_LL1F1;_tmp219=((struct Cyc_Absyn_Malloc_e_struct*)_tmp1D9)->f1;_tmp21A=_tmp219.is_calloc;
_tmp21B=_tmp219.rgn;_tmp21C=_tmp219.elt_type;_tmp21D=_tmp219.num_elts;_LL1F0: if(
_tmp21A){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp21C)),0);if(_tmp21B == 0)s=({struct Cyc_PP_Doc*_tmp27E[5];_tmp27E[4]=Cyc_PP_text(({
const char*_tmp281=")";_tag_dynforward(_tmp281,sizeof(char),_get_zero_arr_size(
_tmp281,2));}));_tmp27E[3]=Cyc_Absynpp_exp2doc(st);_tmp27E[2]=Cyc_PP_text(({
const char*_tmp280=",";_tag_dynforward(_tmp280,sizeof(char),_get_zero_arr_size(
_tmp280,2));}));_tmp27E[1]=Cyc_Absynpp_exp2doc(_tmp21D);_tmp27E[0]=Cyc_PP_text(({
const char*_tmp27F="calloc(";_tag_dynforward(_tmp27F,sizeof(char),
_get_zero_arr_size(_tmp27F,8));}));Cyc_PP_cat(_tag_dynforward(_tmp27E,sizeof(
struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*_tmp282[7];_tmp282[6]=Cyc_PP_text(({
const char*_tmp286=")";_tag_dynforward(_tmp286,sizeof(char),_get_zero_arr_size(
_tmp286,2));}));_tmp282[5]=Cyc_Absynpp_exp2doc(st);_tmp282[4]=Cyc_PP_text(({
const char*_tmp285=",";_tag_dynforward(_tmp285,sizeof(char),_get_zero_arr_size(
_tmp285,2));}));_tmp282[3]=Cyc_Absynpp_exp2doc(_tmp21D);_tmp282[2]=Cyc_PP_text(({
const char*_tmp284=",";_tag_dynforward(_tmp284,sizeof(char),_get_zero_arr_size(
_tmp284,2));}));_tmp282[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp21B);
_tmp282[0]=Cyc_PP_text(({const char*_tmp283="rcalloc(";_tag_dynforward(_tmp283,
sizeof(char),_get_zero_arr_size(_tmp283,9));}));Cyc_PP_cat(_tag_dynforward(
_tmp282,sizeof(struct Cyc_PP_Doc*),7));});}}else{struct Cyc_Absyn_Exp*new_e;if(
_tmp21C == 0)new_e=_tmp21D;else{new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*
_tmp21C,0),_tmp21D,0);}if(_tmp21B == 0)s=({struct Cyc_PP_Doc*_tmp287[3];_tmp287[2]=
Cyc_PP_text(({const char*_tmp289=")";_tag_dynforward(_tmp289,sizeof(char),
_get_zero_arr_size(_tmp289,2));}));_tmp287[1]=Cyc_Absynpp_exp2doc(new_e);_tmp287[
0]=Cyc_PP_text(({const char*_tmp288="malloc(";_tag_dynforward(_tmp288,sizeof(char),
_get_zero_arr_size(_tmp288,8));}));Cyc_PP_cat(_tag_dynforward(_tmp287,sizeof(
struct Cyc_PP_Doc*),3));});else{s=({struct Cyc_PP_Doc*_tmp28A[5];_tmp28A[4]=Cyc_PP_text(({
const char*_tmp28D=")";_tag_dynforward(_tmp28D,sizeof(char),_get_zero_arr_size(
_tmp28D,2));}));_tmp28A[3]=Cyc_Absynpp_exp2doc(new_e);_tmp28A[2]=Cyc_PP_text(({
const char*_tmp28C=",";_tag_dynforward(_tmp28C,sizeof(char),_get_zero_arr_size(
_tmp28C,2));}));_tmp28A[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp21B);
_tmp28A[0]=Cyc_PP_text(({const char*_tmp28B="rmalloc(";_tag_dynforward(_tmp28B,
sizeof(char),_get_zero_arr_size(_tmp28B,9));}));Cyc_PP_cat(_tag_dynforward(
_tmp28A,sizeof(struct Cyc_PP_Doc*),5));});}}goto _LL1A6;_LL1F1: if(*((int*)_tmp1D9)
!= 36)goto _LL1F3;_tmp21E=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1D9)->f1;
_tmp21F=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1D9)->f2;_LL1F2: s=Cyc_Absynpp_group_braces(({
const char*_tmp28E=",";_tag_dynforward(_tmp28E,sizeof(char),_get_zero_arr_size(
_tmp28E,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp21F));goto _LL1A6;
_LL1F3: if(*((int*)_tmp1D9)!= 37)goto _LL1A6;_tmp220=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp1D9)->f1;_LL1F4: s=({struct Cyc_PP_Doc*_tmp28F[7];_tmp28F[6]=Cyc_PP_text(({
const char*_tmp291=")";_tag_dynforward(_tmp291,sizeof(char),_get_zero_arr_size(
_tmp291,2));}));_tmp28F[5]=Cyc_Absynpp_rb();_tmp28F[4]=Cyc_PP_blank_doc();
_tmp28F[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp220));_tmp28F[2]=Cyc_PP_blank_doc();
_tmp28F[1]=Cyc_Absynpp_lb();_tmp28F[0]=Cyc_PP_text(({const char*_tmp290="(";
_tag_dynforward(_tmp290,sizeof(char),_get_zero_arr_size(_tmp290,2));}));Cyc_PP_cat(
_tag_dynforward(_tmp28F,sizeof(struct Cyc_PP_Doc*),7));});goto _LL1A6;_LL1A6:;}if(
inprec >= myprec)s=({struct Cyc_PP_Doc*_tmp292[3];_tmp292[2]=Cyc_PP_text(({const
char*_tmp294=")";_tag_dynforward(_tmp294,sizeof(char),_get_zero_arr_size(_tmp294,
2));}));_tmp292[1]=s;_tmp292[0]=Cyc_PP_text(({const char*_tmp293="(";
_tag_dynforward(_tmp293,sizeof(char),_get_zero_arr_size(_tmp293,2));}));Cyc_PP_cat(
_tag_dynforward(_tmp292,sizeof(struct Cyc_PP_Doc*),3));});return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_designator2doc(void*d){void*_tmp295=d;struct Cyc_Absyn_Exp*_tmp296;
struct _dynforward_ptr*_tmp297;_LL1FF: if(*((int*)_tmp295)!= 0)goto _LL201;_tmp296=((
struct Cyc_Absyn_ArrayElement_struct*)_tmp295)->f1;_LL200: return({struct Cyc_PP_Doc*
_tmp298[3];_tmp298[2]=Cyc_PP_text(({const char*_tmp29A="]";_tag_dynforward(
_tmp29A,sizeof(char),_get_zero_arr_size(_tmp29A,2));}));_tmp298[1]=Cyc_Absynpp_exp2doc(
_tmp296);_tmp298[0]=Cyc_PP_text(({const char*_tmp299=".[";_tag_dynforward(_tmp299,
sizeof(char),_get_zero_arr_size(_tmp299,3));}));Cyc_PP_cat(_tag_dynforward(
_tmp298,sizeof(struct Cyc_PP_Doc*),3));});_LL201: if(*((int*)_tmp295)!= 1)goto
_LL1FE;_tmp297=((struct Cyc_Absyn_FieldName_struct*)_tmp295)->f1;_LL202: return({
struct Cyc_PP_Doc*_tmp29B[2];_tmp29B[1]=Cyc_PP_textptr(_tmp297);_tmp29B[0]=Cyc_PP_text(({
const char*_tmp29C=".";_tag_dynforward(_tmp29C,sizeof(char),_get_zero_arr_size(
_tmp29C,2));}));Cyc_PP_cat(_tag_dynforward(_tmp29B,sizeof(struct Cyc_PP_Doc*),2));});
_LL1FE:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple9*de){if((*de).f1 == 0)
return Cyc_Absynpp_exp2doc((*de).f2);else{return({struct Cyc_PP_Doc*_tmp29D[2];
_tmp29D[1]=Cyc_Absynpp_exp2doc((*de).f2);_tmp29D[0]=Cyc_PP_egroup(({const char*
_tmp29E="";_tag_dynforward(_tmp29E,sizeof(char),_get_zero_arr_size(_tmp29E,1));}),({
const char*_tmp29F="=";_tag_dynforward(_tmp29F,sizeof(char),_get_zero_arr_size(
_tmp29F,2));}),({const char*_tmp2A0="=";_tag_dynforward(_tmp2A0,sizeof(char),
_get_zero_arr_size(_tmp2A0,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));
Cyc_PP_cat(_tag_dynforward(_tmp29D,sizeof(struct Cyc_PP_Doc*),2));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){return Cyc_PP_group(({
const char*_tmp2A1="";_tag_dynforward(_tmp2A1,sizeof(char),_get_zero_arr_size(
_tmp2A1,1));}),({const char*_tmp2A2="";_tag_dynforward(_tmp2A2,sizeof(char),
_get_zero_arr_size(_tmp2A2,1));}),({const char*_tmp2A3=",";_tag_dynforward(
_tmp2A3,sizeof(char),_get_zero_arr_size(_tmp2A3,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))
Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(
union Cyc_Absyn_Cnst_union c){union Cyc_Absyn_Cnst_union _tmp2A4=c;void*_tmp2A5;char
_tmp2A6;void*_tmp2A7;short _tmp2A8;void*_tmp2A9;int _tmp2AA;void*_tmp2AB;int
_tmp2AC;void*_tmp2AD;int _tmp2AE;void*_tmp2AF;long long _tmp2B0;struct
_dynforward_ptr _tmp2B1;struct _dynforward_ptr _tmp2B2;_LL204: if((_tmp2A4.Char_c).tag
!= 0)goto _LL206;_tmp2A5=(_tmp2A4.Char_c).f1;_tmp2A6=(_tmp2A4.Char_c).f2;_LL205:
return Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp2B5;
_tmp2B5.tag=0;_tmp2B5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_char_escape(
_tmp2A6));{void*_tmp2B3[1]={& _tmp2B5};Cyc_aprintf(({const char*_tmp2B4="'%s'";
_tag_dynforward(_tmp2B4,sizeof(char),_get_zero_arr_size(_tmp2B4,5));}),
_tag_dynforward(_tmp2B3,sizeof(void*),1));}}));_LL206: if((_tmp2A4.Short_c).tag != 
1)goto _LL208;_tmp2A7=(_tmp2A4.Short_c).f1;_tmp2A8=(_tmp2A4.Short_c).f2;_LL207:
return Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp2B8;
_tmp2B8.tag=1;_tmp2B8.f1=(unsigned long)((int)_tmp2A8);{void*_tmp2B6[1]={&
_tmp2B8};Cyc_aprintf(({const char*_tmp2B7="%d";_tag_dynforward(_tmp2B7,sizeof(
char),_get_zero_arr_size(_tmp2B7,3));}),_tag_dynforward(_tmp2B6,sizeof(void*),1));}}));
_LL208: if((_tmp2A4.Int_c).tag != 2)goto _LL20A;_tmp2A9=(_tmp2A4.Int_c).f1;if((int)
_tmp2A9 != 2)goto _LL20A;_tmp2AA=(_tmp2A4.Int_c).f2;_LL209: _tmp2AC=_tmp2AA;goto
_LL20B;_LL20A: if((_tmp2A4.Int_c).tag != 2)goto _LL20C;_tmp2AB=(_tmp2A4.Int_c).f1;
if((int)_tmp2AB != 0)goto _LL20C;_tmp2AC=(_tmp2A4.Int_c).f2;_LL20B: return Cyc_PP_text((
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp2BB;_tmp2BB.tag=1;_tmp2BB.f1=(
unsigned long)_tmp2AC;{void*_tmp2B9[1]={& _tmp2BB};Cyc_aprintf(({const char*
_tmp2BA="%d";_tag_dynforward(_tmp2BA,sizeof(char),_get_zero_arr_size(_tmp2BA,3));}),
_tag_dynforward(_tmp2B9,sizeof(void*),1));}}));_LL20C: if((_tmp2A4.Int_c).tag != 2)
goto _LL20E;_tmp2AD=(_tmp2A4.Int_c).f1;if((int)_tmp2AD != 1)goto _LL20E;_tmp2AE=(
_tmp2A4.Int_c).f2;_LL20D: return Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp2BE;_tmp2BE.tag=1;_tmp2BE.f1=(unsigned int)_tmp2AE;{void*_tmp2BC[1]={&
_tmp2BE};Cyc_aprintf(({const char*_tmp2BD="%u";_tag_dynforward(_tmp2BD,sizeof(
char),_get_zero_arr_size(_tmp2BD,3));}),_tag_dynforward(_tmp2BC,sizeof(void*),1));}}));
_LL20E: if((_tmp2A4.LongLong_c).tag != 3)goto _LL210;_tmp2AF=(_tmp2A4.LongLong_c).f1;
_tmp2B0=(_tmp2A4.LongLong_c).f2;_LL20F: return Cyc_PP_text(({const char*_tmp2BF="<<FIX LONG LONG CONSTANT>>";
_tag_dynforward(_tmp2BF,sizeof(char),_get_zero_arr_size(_tmp2BF,27));}));_LL210:
if((_tmp2A4.Float_c).tag != 4)goto _LL212;_tmp2B1=(_tmp2A4.Float_c).f1;_LL211:
return Cyc_PP_text(_tmp2B1);_LL212: if((_tmp2A4.Null_c).tag != 6)goto _LL214;_LL213:
return Cyc_PP_text(({const char*_tmp2C0="NULL";_tag_dynforward(_tmp2C0,sizeof(char),
_get_zero_arr_size(_tmp2C0,5));}));_LL214: if((_tmp2A4.String_c).tag != 5)goto
_LL203;_tmp2B2=(_tmp2A4.String_c).f1;_LL215: return({struct Cyc_PP_Doc*_tmp2C1[3];
_tmp2C1[2]=Cyc_PP_text(({const char*_tmp2C3="\"";_tag_dynforward(_tmp2C3,sizeof(
char),_get_zero_arr_size(_tmp2C3,2));}));_tmp2C1[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(
_tmp2B2));_tmp2C1[0]=Cyc_PP_text(({const char*_tmp2C2="\"";_tag_dynforward(
_tmp2C2,sizeof(char),_get_zero_arr_size(_tmp2C2,2));}));Cyc_PP_cat(
_tag_dynforward(_tmp2C1,sizeof(struct Cyc_PP_Doc*),3));});_LL203:;}struct Cyc_PP_Doc*
Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*es){struct Cyc_PP_Doc*
ps=Cyc_Absynpp_prim2doc(p);if(p == (void*)((void*)19)){if(es == 0  || es->tl != 0)(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2C4=_cycalloc(sizeof(*
_tmp2C4));_tmp2C4[0]=({struct Cyc_Core_Failure_struct _tmp2C5;_tmp2C5.tag=Cyc_Core_Failure;
_tmp2C5.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp2C8;_tmp2C8.tag=
0;_tmp2C8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_PP_string_of_doc(
ps,72));{void*_tmp2C6[1]={& _tmp2C8};Cyc_aprintf(({const char*_tmp2C7="Absynpp::primapp2doc Size: %s with bad args";
_tag_dynforward(_tmp2C7,sizeof(char),_get_zero_arr_size(_tmp2C7,44));}),
_tag_dynforward(_tmp2C6,sizeof(void*),1));}});_tmp2C5;});_tmp2C4;}));{struct Cyc_PP_Doc*
_tmp2C9=Cyc_Absynpp_exp2doc_prec(inprec,(struct Cyc_Absyn_Exp*)es->hd);return({
struct Cyc_PP_Doc*_tmp2CA[2];_tmp2CA[1]=Cyc_PP_text(({const char*_tmp2CB=".size";
_tag_dynforward(_tmp2CB,sizeof(char),_get_zero_arr_size(_tmp2CB,6));}));_tmp2CA[
0]=_tmp2C9;Cyc_PP_cat(_tag_dynforward(_tmp2CA,sizeof(struct Cyc_PP_Doc*),2));});}}
else{struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es);if(ds == 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2CC=
_cycalloc(sizeof(*_tmp2CC));_tmp2CC[0]=({struct Cyc_Core_Failure_struct _tmp2CD;
_tmp2CD.tag=Cyc_Core_Failure;_tmp2CD.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp2D0;_tmp2D0.tag=0;_tmp2D0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_PP_string_of_doc(ps,72));{void*_tmp2CE[1]={& _tmp2D0};Cyc_aprintf(({const char*
_tmp2CF="Absynpp::primapp2doc: %s with no args";_tag_dynforward(_tmp2CF,sizeof(
char),_get_zero_arr_size(_tmp2CF,38));}),_tag_dynforward(_tmp2CE,sizeof(void*),1));}});
_tmp2CD;});_tmp2CC;}));else{if(ds->tl == 0)return({struct Cyc_PP_Doc*_tmp2D1[3];
_tmp2D1[2]=(struct Cyc_PP_Doc*)ds->hd;_tmp2D1[1]=Cyc_PP_text(({const char*_tmp2D2=" ";
_tag_dynforward(_tmp2D2,sizeof(char),_get_zero_arr_size(_tmp2D2,2));}));_tmp2D1[
0]=ps;Cyc_PP_cat(_tag_dynforward(_tmp2D1,sizeof(struct Cyc_PP_Doc*),3));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));_tmp2D3[0]=({
struct Cyc_Core_Failure_struct _tmp2D4;_tmp2D4.tag=Cyc_Core_Failure;_tmp2D4.f1=(
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp2D7;_tmp2D7.tag=0;_tmp2D7.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_PP_string_of_doc(ps,72));{void*
_tmp2D5[1]={& _tmp2D7};Cyc_aprintf(({const char*_tmp2D6="Absynpp::primapp2doc: %s with more than 2 args";
_tag_dynforward(_tmp2D6,sizeof(char),_get_zero_arr_size(_tmp2D6,47));}),
_tag_dynforward(_tmp2D5,sizeof(void*),1));}});_tmp2D4;});_tmp2D3;}));else{return({
struct Cyc_PP_Doc*_tmp2D8[5];_tmp2D8[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)
_check_null(ds->tl))->hd;_tmp2D8[3]=Cyc_PP_text(({const char*_tmp2DA=" ";
_tag_dynforward(_tmp2DA,sizeof(char),_get_zero_arr_size(_tmp2DA,2));}));_tmp2D8[
2]=ps;_tmp2D8[1]=Cyc_PP_text(({const char*_tmp2D9=" ";_tag_dynforward(_tmp2D9,
sizeof(char),_get_zero_arr_size(_tmp2D9,2));}));_tmp2D8[0]=(struct Cyc_PP_Doc*)ds->hd;
Cyc_PP_cat(_tag_dynforward(_tmp2D8,sizeof(struct Cyc_PP_Doc*),5));});}}}}}struct
_dynforward_ptr Cyc_Absynpp_prim2str(void*p){void*_tmp2DB=p;_LL217: if((int)
_tmp2DB != 0)goto _LL219;_LL218: return({const char*_tmp2DC="+";_tag_dynforward(
_tmp2DC,sizeof(char),_get_zero_arr_size(_tmp2DC,2));});_LL219: if((int)_tmp2DB != 
1)goto _LL21B;_LL21A: return({const char*_tmp2DD="*";_tag_dynforward(_tmp2DD,
sizeof(char),_get_zero_arr_size(_tmp2DD,2));});_LL21B: if((int)_tmp2DB != 2)goto
_LL21D;_LL21C: return({const char*_tmp2DE="-";_tag_dynforward(_tmp2DE,sizeof(char),
_get_zero_arr_size(_tmp2DE,2));});_LL21D: if((int)_tmp2DB != 3)goto _LL21F;_LL21E:
return({const char*_tmp2DF="/";_tag_dynforward(_tmp2DF,sizeof(char),
_get_zero_arr_size(_tmp2DF,2));});_LL21F: if((int)_tmp2DB != 4)goto _LL221;_LL220:
return({const char*_tmp2E0="%";_tag_dynforward(_tmp2E0,sizeof(char),
_get_zero_arr_size(_tmp2E0,2));});_LL221: if((int)_tmp2DB != 5)goto _LL223;_LL222:
return({const char*_tmp2E1="==";_tag_dynforward(_tmp2E1,sizeof(char),
_get_zero_arr_size(_tmp2E1,3));});_LL223: if((int)_tmp2DB != 6)goto _LL225;_LL224:
return({const char*_tmp2E2="!=";_tag_dynforward(_tmp2E2,sizeof(char),
_get_zero_arr_size(_tmp2E2,3));});_LL225: if((int)_tmp2DB != 7)goto _LL227;_LL226:
return({const char*_tmp2E3=">";_tag_dynforward(_tmp2E3,sizeof(char),
_get_zero_arr_size(_tmp2E3,2));});_LL227: if((int)_tmp2DB != 8)goto _LL229;_LL228:
return({const char*_tmp2E4="<";_tag_dynforward(_tmp2E4,sizeof(char),
_get_zero_arr_size(_tmp2E4,2));});_LL229: if((int)_tmp2DB != 9)goto _LL22B;_LL22A:
return({const char*_tmp2E5=">=";_tag_dynforward(_tmp2E5,sizeof(char),
_get_zero_arr_size(_tmp2E5,3));});_LL22B: if((int)_tmp2DB != 10)goto _LL22D;_LL22C:
return({const char*_tmp2E6="<=";_tag_dynforward(_tmp2E6,sizeof(char),
_get_zero_arr_size(_tmp2E6,3));});_LL22D: if((int)_tmp2DB != 11)goto _LL22F;_LL22E:
return({const char*_tmp2E7="!";_tag_dynforward(_tmp2E7,sizeof(char),
_get_zero_arr_size(_tmp2E7,2));});_LL22F: if((int)_tmp2DB != 12)goto _LL231;_LL230:
return({const char*_tmp2E8="~";_tag_dynforward(_tmp2E8,sizeof(char),
_get_zero_arr_size(_tmp2E8,2));});_LL231: if((int)_tmp2DB != 13)goto _LL233;_LL232:
return({const char*_tmp2E9="&";_tag_dynforward(_tmp2E9,sizeof(char),
_get_zero_arr_size(_tmp2E9,2));});_LL233: if((int)_tmp2DB != 14)goto _LL235;_LL234:
return({const char*_tmp2EA="|";_tag_dynforward(_tmp2EA,sizeof(char),
_get_zero_arr_size(_tmp2EA,2));});_LL235: if((int)_tmp2DB != 15)goto _LL237;_LL236:
return({const char*_tmp2EB="^";_tag_dynforward(_tmp2EB,sizeof(char),
_get_zero_arr_size(_tmp2EB,2));});_LL237: if((int)_tmp2DB != 16)goto _LL239;_LL238:
return({const char*_tmp2EC="<<";_tag_dynforward(_tmp2EC,sizeof(char),
_get_zero_arr_size(_tmp2EC,3));});_LL239: if((int)_tmp2DB != 17)goto _LL23B;_LL23A:
return({const char*_tmp2ED=">>";_tag_dynforward(_tmp2ED,sizeof(char),
_get_zero_arr_size(_tmp2ED,3));});_LL23B: if((int)_tmp2DB != 18)goto _LL23D;_LL23C:
return({const char*_tmp2EE=">>>";_tag_dynforward(_tmp2EE,sizeof(char),
_get_zero_arr_size(_tmp2EE,4));});_LL23D: if((int)_tmp2DB != 19)goto _LL216;_LL23E:
return({const char*_tmp2EF="size";_tag_dynforward(_tmp2EF,sizeof(char),
_get_zero_arr_size(_tmp2EF,5));});_LL216:;}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
void*p){return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp2F0=(void*)s->r;_LL240: if(_tmp2F0 <= (void*)1)
goto _LL242;if(*((int*)_tmp2F0)!= 11)goto _LL242;_LL241: return 1;_LL242:;_LL243:
return 0;_LL23F:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st){
struct Cyc_PP_Doc*s;{void*_tmp2F1=(void*)st->r;struct Cyc_Absyn_Exp*_tmp2F2;struct
Cyc_Absyn_Stmt*_tmp2F3;struct Cyc_Absyn_Stmt*_tmp2F4;struct Cyc_Absyn_Exp*_tmp2F5;
struct Cyc_Absyn_Exp*_tmp2F6;struct Cyc_Absyn_Stmt*_tmp2F7;struct Cyc_Absyn_Stmt*
_tmp2F8;struct _tuple2 _tmp2F9;struct Cyc_Absyn_Exp*_tmp2FA;struct Cyc_Absyn_Stmt*
_tmp2FB;struct _dynforward_ptr*_tmp2FC;struct Cyc_Absyn_Exp*_tmp2FD;struct _tuple2
_tmp2FE;struct Cyc_Absyn_Exp*_tmp2FF;struct _tuple2 _tmp300;struct Cyc_Absyn_Exp*
_tmp301;struct Cyc_Absyn_Stmt*_tmp302;struct Cyc_Absyn_Exp*_tmp303;struct Cyc_List_List*
_tmp304;struct Cyc_List_List*_tmp305;struct Cyc_List_List*_tmp306;struct Cyc_Absyn_Decl*
_tmp307;struct Cyc_Absyn_Stmt*_tmp308;struct _dynforward_ptr*_tmp309;struct Cyc_Absyn_Stmt*
_tmp30A;struct Cyc_Absyn_Stmt*_tmp30B;struct _tuple2 _tmp30C;struct Cyc_Absyn_Exp*
_tmp30D;struct Cyc_Absyn_Stmt*_tmp30E;struct Cyc_List_List*_tmp30F;struct Cyc_Absyn_Tvar*
_tmp310;struct Cyc_Absyn_Vardecl*_tmp311;int _tmp312;struct Cyc_Absyn_Stmt*_tmp313;
struct Cyc_Absyn_Exp*_tmp314;_LL245: if((int)_tmp2F1 != 0)goto _LL247;_LL246: s=Cyc_PP_text(({
const char*_tmp315=";";_tag_dynforward(_tmp315,sizeof(char),_get_zero_arr_size(
_tmp315,2));}));goto _LL244;_LL247: if(_tmp2F1 <= (void*)1)goto _LL249;if(*((int*)
_tmp2F1)!= 0)goto _LL249;_tmp2F2=((struct Cyc_Absyn_Exp_s_struct*)_tmp2F1)->f1;
_LL248: s=({struct Cyc_PP_Doc*_tmp316[2];_tmp316[1]=Cyc_PP_text(({const char*
_tmp317=";";_tag_dynforward(_tmp317,sizeof(char),_get_zero_arr_size(_tmp317,2));}));
_tmp316[0]=Cyc_Absynpp_exp2doc(_tmp2F2);Cyc_PP_cat(_tag_dynforward(_tmp316,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL244;_LL249: if(_tmp2F1 <= (void*)1)goto
_LL24B;if(*((int*)_tmp2F1)!= 1)goto _LL24B;_tmp2F3=((struct Cyc_Absyn_Seq_s_struct*)
_tmp2F1)->f1;_tmp2F4=((struct Cyc_Absyn_Seq_s_struct*)_tmp2F1)->f2;_LL24A: if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp2F3))s=({struct Cyc_PP_Doc*_tmp318[7];_tmp318[6]=
Cyc_Absynpp_is_declaration(_tmp2F4)?({struct Cyc_PP_Doc*_tmp319[5];_tmp319[4]=Cyc_PP_line_doc();
_tmp319[3]=Cyc_Absynpp_rb();_tmp319[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp2F4));_tmp319[1]=Cyc_PP_blank_doc();_tmp319[0]=Cyc_Absynpp_lb();Cyc_PP_cat(
_tag_dynforward(_tmp319,sizeof(struct Cyc_PP_Doc*),5));}): Cyc_Absynpp_stmt2doc(
_tmp2F4);_tmp318[5]=Cyc_PP_line_doc();_tmp318[4]=Cyc_Absynpp_rb();_tmp318[3]=Cyc_PP_line_doc();
_tmp318[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2F3));_tmp318[1]=Cyc_PP_blank_doc();
_tmp318[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dynforward(_tmp318,sizeof(struct Cyc_PP_Doc*),
7));});else{if(Cyc_Absynpp_is_declaration(_tmp2F4))s=({struct Cyc_PP_Doc*_tmp31A[
7];_tmp31A[6]=Cyc_PP_line_doc();_tmp31A[5]=Cyc_Absynpp_rb();_tmp31A[4]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp2F4));_tmp31A[3]=Cyc_PP_blank_doc();_tmp31A[2]=Cyc_Absynpp_lb();
_tmp31A[1]=Cyc_PP_line_doc();_tmp31A[0]=Cyc_Absynpp_stmt2doc(_tmp2F3);Cyc_PP_cat(
_tag_dynforward(_tmp31A,sizeof(struct Cyc_PP_Doc*),7));});else{s=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),struct _dynforward_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,({const char*_tmp31B="";_tag_dynforward(
_tmp31B,sizeof(char),_get_zero_arr_size(_tmp31B,1));}),({struct Cyc_List_List*
_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C->hd=_tmp2F3;_tmp31C->tl=({struct Cyc_List_List*
_tmp31D=_cycalloc(sizeof(*_tmp31D));_tmp31D->hd=_tmp2F4;_tmp31D->tl=0;_tmp31D;});
_tmp31C;}));}}}else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),
struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,({
const char*_tmp31E="";_tag_dynforward(_tmp31E,sizeof(char),_get_zero_arr_size(
_tmp31E,1));}),({struct Cyc_List_List*_tmp31F=_cycalloc(sizeof(*_tmp31F));_tmp31F->hd=
_tmp2F3;_tmp31F->tl=({struct Cyc_List_List*_tmp320=_cycalloc(sizeof(*_tmp320));
_tmp320->hd=_tmp2F4;_tmp320->tl=0;_tmp320;});_tmp31F;}));}goto _LL244;_LL24B: if(
_tmp2F1 <= (void*)1)goto _LL24D;if(*((int*)_tmp2F1)!= 2)goto _LL24D;_tmp2F5=((
struct Cyc_Absyn_Return_s_struct*)_tmp2F1)->f1;_LL24C: if(_tmp2F5 == 0)s=Cyc_PP_text(({
const char*_tmp321="return;";_tag_dynforward(_tmp321,sizeof(char),
_get_zero_arr_size(_tmp321,8));}));else{s=({struct Cyc_PP_Doc*_tmp322[3];_tmp322[
2]=Cyc_PP_text(({const char*_tmp324=";";_tag_dynforward(_tmp324,sizeof(char),
_get_zero_arr_size(_tmp324,2));}));_tmp322[1]=_tmp2F5 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp2F5);_tmp322[0]=Cyc_PP_text(({const char*_tmp323="return ";
_tag_dynforward(_tmp323,sizeof(char),_get_zero_arr_size(_tmp323,8));}));Cyc_PP_cat(
_tag_dynforward(_tmp322,sizeof(struct Cyc_PP_Doc*),3));});}goto _LL244;_LL24D: if(
_tmp2F1 <= (void*)1)goto _LL24F;if(*((int*)_tmp2F1)!= 3)goto _LL24F;_tmp2F6=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2F1)->f1;_tmp2F7=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp2F1)->f2;_tmp2F8=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2F1)->f3;_LL24E: {
int print_else;{void*_tmp325=(void*)_tmp2F8->r;_LL26C: if((int)_tmp325 != 0)goto
_LL26E;_LL26D: print_else=0;goto _LL26B;_LL26E:;_LL26F: print_else=1;goto _LL26B;
_LL26B:;}s=({struct Cyc_PP_Doc*_tmp326[8];_tmp326[7]=print_else?({struct Cyc_PP_Doc*
_tmp32A[6];_tmp32A[5]=Cyc_Absynpp_rb();_tmp32A[4]=Cyc_PP_line_doc();_tmp32A[3]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp32C[2];_tmp32C[1]=Cyc_Absynpp_stmt2doc(
_tmp2F8);_tmp32C[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp32C,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp32A[2]=Cyc_Absynpp_lb();_tmp32A[1]=Cyc_PP_text(({
const char*_tmp32B="else ";_tag_dynforward(_tmp32B,sizeof(char),
_get_zero_arr_size(_tmp32B,6));}));_tmp32A[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_dynforward(_tmp32A,sizeof(struct Cyc_PP_Doc*),6));}): Cyc_PP_nil_doc();
_tmp326[6]=Cyc_Absynpp_rb();_tmp326[5]=Cyc_PP_line_doc();_tmp326[4]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp329[2];_tmp329[1]=Cyc_Absynpp_stmt2doc(_tmp2F7);_tmp329[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp329,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp326[3]=Cyc_Absynpp_lb();_tmp326[2]=Cyc_PP_text(({const char*_tmp328=") ";
_tag_dynforward(_tmp328,sizeof(char),_get_zero_arr_size(_tmp328,3));}));_tmp326[
1]=Cyc_Absynpp_exp2doc(_tmp2F6);_tmp326[0]=Cyc_PP_text(({const char*_tmp327="if (";
_tag_dynforward(_tmp327,sizeof(char),_get_zero_arr_size(_tmp327,5));}));Cyc_PP_cat(
_tag_dynforward(_tmp326,sizeof(struct Cyc_PP_Doc*),8));});goto _LL244;}_LL24F: if(
_tmp2F1 <= (void*)1)goto _LL251;if(*((int*)_tmp2F1)!= 4)goto _LL251;_tmp2F9=((
struct Cyc_Absyn_While_s_struct*)_tmp2F1)->f1;_tmp2FA=_tmp2F9.f1;_tmp2FB=((struct
Cyc_Absyn_While_s_struct*)_tmp2F1)->f2;_LL250: s=({struct Cyc_PP_Doc*_tmp32D[7];
_tmp32D[6]=Cyc_Absynpp_rb();_tmp32D[5]=Cyc_PP_line_doc();_tmp32D[4]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp330[2];_tmp330[1]=Cyc_Absynpp_stmt2doc(_tmp2FB);_tmp330[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp330,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp32D[3]=Cyc_Absynpp_lb();_tmp32D[2]=Cyc_PP_text(({const char*_tmp32F=") ";
_tag_dynforward(_tmp32F,sizeof(char),_get_zero_arr_size(_tmp32F,3));}));_tmp32D[
1]=Cyc_Absynpp_exp2doc(_tmp2FA);_tmp32D[0]=Cyc_PP_text(({const char*_tmp32E="while (";
_tag_dynforward(_tmp32E,sizeof(char),_get_zero_arr_size(_tmp32E,8));}));Cyc_PP_cat(
_tag_dynforward(_tmp32D,sizeof(struct Cyc_PP_Doc*),7));});goto _LL244;_LL251: if(
_tmp2F1 <= (void*)1)goto _LL253;if(*((int*)_tmp2F1)!= 5)goto _LL253;_LL252: s=Cyc_PP_text(({
const char*_tmp331="break;";_tag_dynforward(_tmp331,sizeof(char),
_get_zero_arr_size(_tmp331,7));}));goto _LL244;_LL253: if(_tmp2F1 <= (void*)1)goto
_LL255;if(*((int*)_tmp2F1)!= 6)goto _LL255;_LL254: s=Cyc_PP_text(({const char*
_tmp332="continue;";_tag_dynforward(_tmp332,sizeof(char),_get_zero_arr_size(
_tmp332,10));}));goto _LL244;_LL255: if(_tmp2F1 <= (void*)1)goto _LL257;if(*((int*)
_tmp2F1)!= 7)goto _LL257;_tmp2FC=((struct Cyc_Absyn_Goto_s_struct*)_tmp2F1)->f1;
_LL256: s=Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp335;
_tmp335.tag=0;_tmp335.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp2FC);{
void*_tmp333[1]={& _tmp335};Cyc_aprintf(({const char*_tmp334="goto %s;";
_tag_dynforward(_tmp334,sizeof(char),_get_zero_arr_size(_tmp334,9));}),
_tag_dynforward(_tmp333,sizeof(void*),1));}}));goto _LL244;_LL257: if(_tmp2F1 <= (
void*)1)goto _LL259;if(*((int*)_tmp2F1)!= 8)goto _LL259;_tmp2FD=((struct Cyc_Absyn_For_s_struct*)
_tmp2F1)->f1;_tmp2FE=((struct Cyc_Absyn_For_s_struct*)_tmp2F1)->f2;_tmp2FF=
_tmp2FE.f1;_tmp300=((struct Cyc_Absyn_For_s_struct*)_tmp2F1)->f3;_tmp301=_tmp300.f1;
_tmp302=((struct Cyc_Absyn_For_s_struct*)_tmp2F1)->f4;_LL258: s=({struct Cyc_PP_Doc*
_tmp336[11];_tmp336[10]=Cyc_Absynpp_rb();_tmp336[9]=Cyc_PP_line_doc();_tmp336[8]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp33B[2];_tmp33B[1]=Cyc_Absynpp_stmt2doc(
_tmp302);_tmp33B[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp33B,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp336[7]=Cyc_Absynpp_lb();_tmp336[6]=Cyc_PP_text(({
const char*_tmp33A=") ";_tag_dynforward(_tmp33A,sizeof(char),_get_zero_arr_size(
_tmp33A,3));}));_tmp336[5]=Cyc_Absynpp_exp2doc(_tmp301);_tmp336[4]=Cyc_PP_text(({
const char*_tmp339="; ";_tag_dynforward(_tmp339,sizeof(char),_get_zero_arr_size(
_tmp339,3));}));_tmp336[3]=Cyc_Absynpp_exp2doc(_tmp2FF);_tmp336[2]=Cyc_PP_text(({
const char*_tmp338="; ";_tag_dynforward(_tmp338,sizeof(char),_get_zero_arr_size(
_tmp338,3));}));_tmp336[1]=Cyc_Absynpp_exp2doc(_tmp2FD);_tmp336[0]=Cyc_PP_text(({
const char*_tmp337="for(";_tag_dynforward(_tmp337,sizeof(char),_get_zero_arr_size(
_tmp337,5));}));Cyc_PP_cat(_tag_dynforward(_tmp336,sizeof(struct Cyc_PP_Doc*),11));});
goto _LL244;_LL259: if(_tmp2F1 <= (void*)1)goto _LL25B;if(*((int*)_tmp2F1)!= 9)goto
_LL25B;_tmp303=((struct Cyc_Absyn_Switch_s_struct*)_tmp2F1)->f1;_tmp304=((struct
Cyc_Absyn_Switch_s_struct*)_tmp2F1)->f2;_LL25A: s=({struct Cyc_PP_Doc*_tmp33C[8];
_tmp33C[7]=Cyc_Absynpp_rb();_tmp33C[6]=Cyc_PP_line_doc();_tmp33C[5]=Cyc_Absynpp_switchclauses2doc(
_tmp304);_tmp33C[4]=Cyc_PP_line_doc();_tmp33C[3]=Cyc_Absynpp_lb();_tmp33C[2]=Cyc_PP_text(({
const char*_tmp33E=") ";_tag_dynforward(_tmp33E,sizeof(char),_get_zero_arr_size(
_tmp33E,3));}));_tmp33C[1]=Cyc_Absynpp_exp2doc(_tmp303);_tmp33C[0]=Cyc_PP_text(({
const char*_tmp33D="switch (";_tag_dynforward(_tmp33D,sizeof(char),
_get_zero_arr_size(_tmp33D,9));}));Cyc_PP_cat(_tag_dynforward(_tmp33C,sizeof(
struct Cyc_PP_Doc*),8));});goto _LL244;_LL25B: if(_tmp2F1 <= (void*)1)goto _LL25D;if(*((
int*)_tmp2F1)!= 10)goto _LL25D;_tmp305=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp2F1)->f1;if(_tmp305 != 0)goto _LL25D;_LL25C: s=Cyc_PP_text(({const char*_tmp33F="fallthru;";
_tag_dynforward(_tmp33F,sizeof(char),_get_zero_arr_size(_tmp33F,10));}));goto
_LL244;_LL25D: if(_tmp2F1 <= (void*)1)goto _LL25F;if(*((int*)_tmp2F1)!= 10)goto
_LL25F;_tmp306=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp2F1)->f1;_LL25E: s=({
struct Cyc_PP_Doc*_tmp340[3];_tmp340[2]=Cyc_PP_text(({const char*_tmp342=");";
_tag_dynforward(_tmp342,sizeof(char),_get_zero_arr_size(_tmp342,3));}));_tmp340[
1]=Cyc_Absynpp_exps2doc_prec(20,_tmp306);_tmp340[0]=Cyc_PP_text(({const char*
_tmp341="fallthru(";_tag_dynforward(_tmp341,sizeof(char),_get_zero_arr_size(
_tmp341,10));}));Cyc_PP_cat(_tag_dynforward(_tmp340,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL244;_LL25F: if(_tmp2F1 <= (void*)1)goto _LL261;if(*((int*)_tmp2F1)!= 11)goto
_LL261;_tmp307=((struct Cyc_Absyn_Decl_s_struct*)_tmp2F1)->f1;_tmp308=((struct Cyc_Absyn_Decl_s_struct*)
_tmp2F1)->f2;_LL260: s=({struct Cyc_PP_Doc*_tmp343[3];_tmp343[2]=Cyc_Absynpp_stmt2doc(
_tmp308);_tmp343[1]=Cyc_PP_line_doc();_tmp343[0]=Cyc_Absynpp_decl2doc(_tmp307);
Cyc_PP_cat(_tag_dynforward(_tmp343,sizeof(struct Cyc_PP_Doc*),3));});goto _LL244;
_LL261: if(_tmp2F1 <= (void*)1)goto _LL263;if(*((int*)_tmp2F1)!= 12)goto _LL263;
_tmp309=((struct Cyc_Absyn_Label_s_struct*)_tmp2F1)->f1;_tmp30A=((struct Cyc_Absyn_Label_s_struct*)
_tmp2F1)->f2;_LL262: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(
_tmp30A))s=({struct Cyc_PP_Doc*_tmp344[7];_tmp344[6]=Cyc_Absynpp_rb();_tmp344[5]=
Cyc_PP_line_doc();_tmp344[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp30A));
_tmp344[3]=Cyc_PP_line_doc();_tmp344[2]=Cyc_Absynpp_lb();_tmp344[1]=Cyc_PP_text(({
const char*_tmp345=": ";_tag_dynforward(_tmp345,sizeof(char),_get_zero_arr_size(
_tmp345,3));}));_tmp344[0]=Cyc_PP_textptr(_tmp309);Cyc_PP_cat(_tag_dynforward(
_tmp344,sizeof(struct Cyc_PP_Doc*),7));});else{s=({struct Cyc_PP_Doc*_tmp346[3];
_tmp346[2]=Cyc_Absynpp_stmt2doc(_tmp30A);_tmp346[1]=Cyc_PP_text(({const char*
_tmp347=": ";_tag_dynforward(_tmp347,sizeof(char),_get_zero_arr_size(_tmp347,3));}));
_tmp346[0]=Cyc_PP_textptr(_tmp309);Cyc_PP_cat(_tag_dynforward(_tmp346,sizeof(
struct Cyc_PP_Doc*),3));});}goto _LL244;_LL263: if(_tmp2F1 <= (void*)1)goto _LL265;
if(*((int*)_tmp2F1)!= 13)goto _LL265;_tmp30B=((struct Cyc_Absyn_Do_s_struct*)
_tmp2F1)->f1;_tmp30C=((struct Cyc_Absyn_Do_s_struct*)_tmp2F1)->f2;_tmp30D=_tmp30C.f1;
_LL264: s=({struct Cyc_PP_Doc*_tmp348[9];_tmp348[8]=Cyc_PP_text(({const char*
_tmp34B=");";_tag_dynforward(_tmp34B,sizeof(char),_get_zero_arr_size(_tmp34B,3));}));
_tmp348[7]=Cyc_Absynpp_exp2doc(_tmp30D);_tmp348[6]=Cyc_PP_text(({const char*
_tmp34A=" while (";_tag_dynforward(_tmp34A,sizeof(char),_get_zero_arr_size(
_tmp34A,9));}));_tmp348[5]=Cyc_Absynpp_rb();_tmp348[4]=Cyc_PP_line_doc();_tmp348[
3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp30B));_tmp348[2]=Cyc_PP_line_doc();
_tmp348[1]=Cyc_Absynpp_lb();_tmp348[0]=Cyc_PP_text(({const char*_tmp349="do ";
_tag_dynforward(_tmp349,sizeof(char),_get_zero_arr_size(_tmp349,4));}));Cyc_PP_cat(
_tag_dynforward(_tmp348,sizeof(struct Cyc_PP_Doc*),9));});goto _LL244;_LL265: if(
_tmp2F1 <= (void*)1)goto _LL267;if(*((int*)_tmp2F1)!= 14)goto _LL267;_tmp30E=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp2F1)->f1;_tmp30F=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp2F1)->f2;_LL266: s=({struct Cyc_PP_Doc*_tmp34C[12];_tmp34C[11]=Cyc_Absynpp_rb();
_tmp34C[10]=Cyc_PP_line_doc();_tmp34C[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp30F));_tmp34C[8]=Cyc_PP_line_doc();_tmp34C[7]=Cyc_Absynpp_lb();_tmp34C[6]=
Cyc_PP_text(({const char*_tmp34E=" catch ";_tag_dynforward(_tmp34E,sizeof(char),
_get_zero_arr_size(_tmp34E,8));}));_tmp34C[5]=Cyc_Absynpp_rb();_tmp34C[4]=Cyc_PP_line_doc();
_tmp34C[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp30E));_tmp34C[2]=Cyc_PP_line_doc();
_tmp34C[1]=Cyc_Absynpp_lb();_tmp34C[0]=Cyc_PP_text(({const char*_tmp34D="try ";
_tag_dynforward(_tmp34D,sizeof(char),_get_zero_arr_size(_tmp34D,5));}));Cyc_PP_cat(
_tag_dynforward(_tmp34C,sizeof(struct Cyc_PP_Doc*),12));});goto _LL244;_LL267: if(
_tmp2F1 <= (void*)1)goto _LL269;if(*((int*)_tmp2F1)!= 15)goto _LL269;_tmp310=((
struct Cyc_Absyn_Region_s_struct*)_tmp2F1)->f1;_tmp311=((struct Cyc_Absyn_Region_s_struct*)
_tmp2F1)->f2;_tmp312=((struct Cyc_Absyn_Region_s_struct*)_tmp2F1)->f3;_tmp313=((
struct Cyc_Absyn_Region_s_struct*)_tmp2F1)->f4;_LL268: s=({struct Cyc_PP_Doc*
_tmp34F[11];_tmp34F[10]=Cyc_Absynpp_rb();_tmp34F[9]=Cyc_PP_line_doc();_tmp34F[8]=
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp313));_tmp34F[7]=Cyc_PP_line_doc();
_tmp34F[6]=Cyc_Absynpp_lb();_tmp34F[5]=Cyc_Absynpp_qvar2doc(_tmp311->name);
_tmp34F[4]=Cyc_PP_text(({const char*_tmp354=">";_tag_dynforward(_tmp354,sizeof(
char),_get_zero_arr_size(_tmp354,2));}));_tmp34F[3]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp310));_tmp34F[2]=Cyc_PP_text(({const char*_tmp353="<";_tag_dynforward(_tmp353,
sizeof(char),_get_zero_arr_size(_tmp353,2));}));_tmp34F[1]=_tmp312?Cyc_PP_text(({
const char*_tmp351="";_tag_dynforward(_tmp351,sizeof(char),_get_zero_arr_size(
_tmp351,1));})): Cyc_PP_text(({const char*_tmp352="[resetable]";_tag_dynforward(
_tmp352,sizeof(char),_get_zero_arr_size(_tmp352,12));}));_tmp34F[0]=Cyc_PP_text(({
const char*_tmp350="region";_tag_dynforward(_tmp350,sizeof(char),
_get_zero_arr_size(_tmp350,7));}));Cyc_PP_cat(_tag_dynforward(_tmp34F,sizeof(
struct Cyc_PP_Doc*),11));});goto _LL244;_LL269: if(_tmp2F1 <= (void*)1)goto _LL244;
if(*((int*)_tmp2F1)!= 16)goto _LL244;_tmp314=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp2F1)->f1;_LL26A: s=({struct Cyc_PP_Doc*_tmp355[3];_tmp355[2]=Cyc_PP_text(({
const char*_tmp357=");";_tag_dynforward(_tmp357,sizeof(char),_get_zero_arr_size(
_tmp357,3));}));_tmp355[1]=Cyc_Absynpp_exp2doc(_tmp314);_tmp355[0]=Cyc_PP_text(({
const char*_tmp356="reset_region(";_tag_dynforward(_tmp356,sizeof(char),
_get_zero_arr_size(_tmp356,14));}));Cyc_PP_cat(_tag_dynforward(_tmp355,sizeof(
struct Cyc_PP_Doc*),3));});goto _LL244;_LL244:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p){struct Cyc_PP_Doc*s;{void*_tmp358=(void*)p->r;void*_tmp359;
int _tmp35A;char _tmp35B;struct _dynforward_ptr _tmp35C;struct Cyc_Absyn_Vardecl*
_tmp35D;struct Cyc_Absyn_Pat*_tmp35E;struct Cyc_Absyn_Pat _tmp35F;void*_tmp360;
struct Cyc_Absyn_Vardecl*_tmp361;struct Cyc_Absyn_Pat*_tmp362;struct Cyc_Absyn_Tvar*
_tmp363;struct Cyc_Absyn_Vardecl*_tmp364;struct Cyc_List_List*_tmp365;int _tmp366;
struct Cyc_Absyn_Pat*_tmp367;struct Cyc_Absyn_Vardecl*_tmp368;struct Cyc_Absyn_Pat*
_tmp369;struct Cyc_Absyn_Pat _tmp36A;void*_tmp36B;struct Cyc_Absyn_Vardecl*_tmp36C;
struct Cyc_Absyn_Pat*_tmp36D;struct _tuple0*_tmp36E;struct _tuple0*_tmp36F;struct
Cyc_List_List*_tmp370;int _tmp371;struct Cyc_Absyn_AggrInfo _tmp372;void*_tmp373;
struct Cyc_List_List*_tmp374;struct Cyc_List_List*_tmp375;int _tmp376;struct Cyc_Absyn_Enumfield*
_tmp377;struct Cyc_Absyn_Enumfield*_tmp378;struct Cyc_Absyn_Tunionfield*_tmp379;
struct Cyc_List_List*_tmp37A;struct Cyc_Absyn_Tunionfield*_tmp37B;struct Cyc_List_List*
_tmp37C;int _tmp37D;struct Cyc_Absyn_Exp*_tmp37E;_LL271: if((int)_tmp358 != 0)goto
_LL273;_LL272: s=Cyc_PP_text(({const char*_tmp37F="_";_tag_dynforward(_tmp37F,
sizeof(char),_get_zero_arr_size(_tmp37F,2));}));goto _LL270;_LL273: if((int)
_tmp358 != 1)goto _LL275;_LL274: s=Cyc_PP_text(({const char*_tmp380="NULL";
_tag_dynforward(_tmp380,sizeof(char),_get_zero_arr_size(_tmp380,5));}));goto
_LL270;_LL275: if(_tmp358 <= (void*)2)goto _LL277;if(*((int*)_tmp358)!= 7)goto
_LL277;_tmp359=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp358)->f1;_tmp35A=((
struct Cyc_Absyn_Int_p_struct*)_tmp358)->f2;_LL276: if(_tmp359 != (void*)((void*)1))
s=Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp383;_tmp383.tag=
1;_tmp383.f1=(unsigned long)_tmp35A;{void*_tmp381[1]={& _tmp383};Cyc_aprintf(({
const char*_tmp382="%d";_tag_dynforward(_tmp382,sizeof(char),_get_zero_arr_size(
_tmp382,3));}),_tag_dynforward(_tmp381,sizeof(void*),1));}}));else{s=Cyc_PP_text((
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp386;_tmp386.tag=1;_tmp386.f1=(
unsigned int)_tmp35A;{void*_tmp384[1]={& _tmp386};Cyc_aprintf(({const char*_tmp385="%u";
_tag_dynforward(_tmp385,sizeof(char),_get_zero_arr_size(_tmp385,3));}),
_tag_dynforward(_tmp384,sizeof(void*),1));}}));}goto _LL270;_LL277: if(_tmp358 <= (
void*)2)goto _LL279;if(*((int*)_tmp358)!= 8)goto _LL279;_tmp35B=((struct Cyc_Absyn_Char_p_struct*)
_tmp358)->f1;_LL278: s=Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp389;_tmp389.tag=0;_tmp389.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_char_escape(_tmp35B));{void*_tmp387[1]={& _tmp389};Cyc_aprintf(({
const char*_tmp388="'%s'";_tag_dynforward(_tmp388,sizeof(char),_get_zero_arr_size(
_tmp388,5));}),_tag_dynforward(_tmp387,sizeof(void*),1));}}));goto _LL270;_LL279:
if(_tmp358 <= (void*)2)goto _LL27B;if(*((int*)_tmp358)!= 9)goto _LL27B;_tmp35C=((
struct Cyc_Absyn_Float_p_struct*)_tmp358)->f1;_LL27A: s=Cyc_PP_text(_tmp35C);goto
_LL270;_LL27B: if(_tmp358 <= (void*)2)goto _LL27D;if(*((int*)_tmp358)!= 0)goto
_LL27D;_tmp35D=((struct Cyc_Absyn_Var_p_struct*)_tmp358)->f1;_tmp35E=((struct Cyc_Absyn_Var_p_struct*)
_tmp358)->f2;_tmp35F=*_tmp35E;_tmp360=(void*)_tmp35F.r;if((int)_tmp360 != 0)goto
_LL27D;_LL27C: s=Cyc_Absynpp_qvar2doc(_tmp35D->name);goto _LL270;_LL27D: if(_tmp358
<= (void*)2)goto _LL27F;if(*((int*)_tmp358)!= 0)goto _LL27F;_tmp361=((struct Cyc_Absyn_Var_p_struct*)
_tmp358)->f1;_tmp362=((struct Cyc_Absyn_Var_p_struct*)_tmp358)->f2;_LL27E: s=({
struct Cyc_PP_Doc*_tmp38A[3];_tmp38A[2]=Cyc_Absynpp_pat2doc(_tmp362);_tmp38A[1]=
Cyc_PP_text(({const char*_tmp38B=" as ";_tag_dynforward(_tmp38B,sizeof(char),
_get_zero_arr_size(_tmp38B,5));}));_tmp38A[0]=Cyc_Absynpp_qvar2doc(_tmp361->name);
Cyc_PP_cat(_tag_dynforward(_tmp38A,sizeof(struct Cyc_PP_Doc*),3));});goto _LL270;
_LL27F: if(_tmp358 <= (void*)2)goto _LL281;if(*((int*)_tmp358)!= 2)goto _LL281;
_tmp363=((struct Cyc_Absyn_TagInt_p_struct*)_tmp358)->f1;_tmp364=((struct Cyc_Absyn_TagInt_p_struct*)
_tmp358)->f2;_LL280: s=({struct Cyc_PP_Doc*_tmp38C[4];_tmp38C[3]=Cyc_PP_text(({
const char*_tmp38E=">";_tag_dynforward(_tmp38E,sizeof(char),_get_zero_arr_size(
_tmp38E,2));}));_tmp38C[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp363));_tmp38C[
1]=Cyc_PP_text(({const char*_tmp38D="<";_tag_dynforward(_tmp38D,sizeof(char),
_get_zero_arr_size(_tmp38D,2));}));_tmp38C[0]=Cyc_Absynpp_qvar2doc(_tmp364->name);
Cyc_PP_cat(_tag_dynforward(_tmp38C,sizeof(struct Cyc_PP_Doc*),4));});goto _LL270;
_LL281: if(_tmp358 <= (void*)2)goto _LL283;if(*((int*)_tmp358)!= 3)goto _LL283;
_tmp365=((struct Cyc_Absyn_Tuple_p_struct*)_tmp358)->f1;_tmp366=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp358)->f2;_LL282: s=({struct Cyc_PP_Doc*_tmp38F[4];_tmp38F[3]=_tmp366?Cyc_PP_text(({
const char*_tmp392=", ...)";_tag_dynforward(_tmp392,sizeof(char),
_get_zero_arr_size(_tmp392,7));})): Cyc_PP_text(({const char*_tmp393=")";
_tag_dynforward(_tmp393,sizeof(char),_get_zero_arr_size(_tmp393,2));}));_tmp38F[
2]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct
_dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,({
const char*_tmp391=",";_tag_dynforward(_tmp391,sizeof(char),_get_zero_arr_size(
_tmp391,2));}),_tmp365);_tmp38F[1]=Cyc_PP_text(({const char*_tmp390="(";
_tag_dynforward(_tmp390,sizeof(char),_get_zero_arr_size(_tmp390,2));}));_tmp38F[
0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dynforward(_tmp38F,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL270;_LL283: if(_tmp358 <= (void*)2)goto _LL285;if(*((int*)_tmp358)!= 
4)goto _LL285;_tmp367=((struct Cyc_Absyn_Pointer_p_struct*)_tmp358)->f1;_LL284: s=({
struct Cyc_PP_Doc*_tmp394[2];_tmp394[1]=Cyc_Absynpp_pat2doc(_tmp367);_tmp394[0]=
Cyc_PP_text(({const char*_tmp395="&";_tag_dynforward(_tmp395,sizeof(char),
_get_zero_arr_size(_tmp395,2));}));Cyc_PP_cat(_tag_dynforward(_tmp394,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL270;_LL285: if(_tmp358 <= (void*)2)goto _LL287;if(*((
int*)_tmp358)!= 1)goto _LL287;_tmp368=((struct Cyc_Absyn_Reference_p_struct*)
_tmp358)->f1;_tmp369=((struct Cyc_Absyn_Reference_p_struct*)_tmp358)->f2;_tmp36A=*
_tmp369;_tmp36B=(void*)_tmp36A.r;if((int)_tmp36B != 0)goto _LL287;_LL286: s=({
struct Cyc_PP_Doc*_tmp396[2];_tmp396[1]=Cyc_Absynpp_qvar2doc(_tmp368->name);
_tmp396[0]=Cyc_PP_text(({const char*_tmp397="*";_tag_dynforward(_tmp397,sizeof(
char),_get_zero_arr_size(_tmp397,2));}));Cyc_PP_cat(_tag_dynforward(_tmp396,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL270;_LL287: if(_tmp358 <= (void*)2)goto
_LL289;if(*((int*)_tmp358)!= 1)goto _LL289;_tmp36C=((struct Cyc_Absyn_Reference_p_struct*)
_tmp358)->f1;_tmp36D=((struct Cyc_Absyn_Reference_p_struct*)_tmp358)->f2;_LL288: s=({
struct Cyc_PP_Doc*_tmp398[4];_tmp398[3]=Cyc_Absynpp_pat2doc(_tmp36D);_tmp398[2]=
Cyc_PP_text(({const char*_tmp39A=" as ";_tag_dynforward(_tmp39A,sizeof(char),
_get_zero_arr_size(_tmp39A,5));}));_tmp398[1]=Cyc_Absynpp_qvar2doc(_tmp36C->name);
_tmp398[0]=Cyc_PP_text(({const char*_tmp399="*";_tag_dynforward(_tmp399,sizeof(
char),_get_zero_arr_size(_tmp399,2));}));Cyc_PP_cat(_tag_dynforward(_tmp398,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL270;_LL289: if(_tmp358 <= (void*)2)goto
_LL28B;if(*((int*)_tmp358)!= 12)goto _LL28B;_tmp36E=((struct Cyc_Absyn_UnknownId_p_struct*)
_tmp358)->f1;_LL28A: s=Cyc_Absynpp_qvar2doc(_tmp36E);goto _LL270;_LL28B: if(_tmp358
<= (void*)2)goto _LL28D;if(*((int*)_tmp358)!= 13)goto _LL28D;_tmp36F=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp358)->f1;_tmp370=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp358)->f2;
_tmp371=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp358)->f3;_LL28C: {struct
_dynforward_ptr term=_tmp371?({const char*_tmp39E=", ...)";_tag_dynforward(_tmp39E,
sizeof(char),_get_zero_arr_size(_tmp39E,7));}):({const char*_tmp39F=")";
_tag_dynforward(_tmp39F,sizeof(char),_get_zero_arr_size(_tmp39F,2));});s=({
struct Cyc_PP_Doc*_tmp39B[2];_tmp39B[1]=Cyc_PP_group(({const char*_tmp39C="(";
_tag_dynforward(_tmp39C,sizeof(char),_get_zero_arr_size(_tmp39C,2));}),term,({
const char*_tmp39D=",";_tag_dynforward(_tmp39D,sizeof(char),_get_zero_arr_size(
_tmp39D,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp370));_tmp39B[0]=
Cyc_Absynpp_qvar2doc(_tmp36F);Cyc_PP_cat(_tag_dynforward(_tmp39B,sizeof(struct
Cyc_PP_Doc*),2));});goto _LL270;}_LL28D: if(_tmp358 <= (void*)2)goto _LL28F;if(*((
int*)_tmp358)!= 5)goto _LL28F;_tmp372=((struct Cyc_Absyn_Aggr_p_struct*)_tmp358)->f1;
_tmp373=(void*)_tmp372.aggr_info;_tmp374=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp358)->f2;_tmp375=((struct Cyc_Absyn_Aggr_p_struct*)_tmp358)->f3;_tmp376=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp358)->f4;_LL28E: {struct _dynforward_ptr term=
_tmp376?({const char*_tmp3A8=", ...}";_tag_dynforward(_tmp3A8,sizeof(char),
_get_zero_arr_size(_tmp3A8,7));}):({const char*_tmp3A9="}";_tag_dynforward(
_tmp3A9,sizeof(char),_get_zero_arr_size(_tmp3A9,2));});struct _tuple0*_tmp3A1;
struct _tuple3 _tmp3A0=Cyc_Absyn_aggr_kinded_name(_tmp373);_tmp3A1=_tmp3A0.f2;s=({
struct Cyc_PP_Doc*_tmp3A2[4];_tmp3A2[3]=Cyc_PP_group(({const char*_tmp3A6="";
_tag_dynforward(_tmp3A6,sizeof(char),_get_zero_arr_size(_tmp3A6,1));}),term,({
const char*_tmp3A7=",";_tag_dynforward(_tmp3A7,sizeof(char),_get_zero_arr_size(
_tmp3A7,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp375));_tmp3A2[2]=Cyc_PP_egroup(({
const char*_tmp3A3="[";_tag_dynforward(_tmp3A3,sizeof(char),_get_zero_arr_size(
_tmp3A3,2));}),({const char*_tmp3A4="]";_tag_dynforward(_tmp3A4,sizeof(char),
_get_zero_arr_size(_tmp3A4,2));}),({const char*_tmp3A5=",";_tag_dynforward(
_tmp3A5,sizeof(char),_get_zero_arr_size(_tmp3A5,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dynforward_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp374)));_tmp3A2[1]=
Cyc_Absynpp_lb();_tmp3A2[0]=Cyc_Absynpp_qvar2doc(_tmp3A1);Cyc_PP_cat(
_tag_dynforward(_tmp3A2,sizeof(struct Cyc_PP_Doc*),4));});goto _LL270;}_LL28F: if(
_tmp358 <= (void*)2)goto _LL291;if(*((int*)_tmp358)!= 10)goto _LL291;_tmp377=((
struct Cyc_Absyn_Enum_p_struct*)_tmp358)->f2;_LL290: _tmp378=_tmp377;goto _LL292;
_LL291: if(_tmp358 <= (void*)2)goto _LL293;if(*((int*)_tmp358)!= 11)goto _LL293;
_tmp378=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp358)->f2;_LL292: s=Cyc_Absynpp_qvar2doc(
_tmp378->name);goto _LL270;_LL293: if(_tmp358 <= (void*)2)goto _LL295;if(*((int*)
_tmp358)!= 6)goto _LL295;_tmp379=((struct Cyc_Absyn_Tunion_p_struct*)_tmp358)->f2;
_tmp37A=((struct Cyc_Absyn_Tunion_p_struct*)_tmp358)->f3;if(_tmp37A != 0)goto
_LL295;_LL294: s=Cyc_Absynpp_qvar2doc(_tmp379->name);goto _LL270;_LL295: if(_tmp358
<= (void*)2)goto _LL297;if(*((int*)_tmp358)!= 6)goto _LL297;_tmp37B=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp358)->f2;_tmp37C=((struct Cyc_Absyn_Tunion_p_struct*)_tmp358)->f3;_tmp37D=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp358)->f4;_LL296: {struct _dynforward_ptr term=
_tmp37D?({const char*_tmp3AD=", ...)";_tag_dynforward(_tmp3AD,sizeof(char),
_get_zero_arr_size(_tmp3AD,7));}):({const char*_tmp3AE=")";_tag_dynforward(
_tmp3AE,sizeof(char),_get_zero_arr_size(_tmp3AE,2));});s=({struct Cyc_PP_Doc*
_tmp3AA[2];_tmp3AA[1]=Cyc_PP_egroup(({const char*_tmp3AB="(";_tag_dynforward(
_tmp3AB,sizeof(char),_get_zero_arr_size(_tmp3AB,2));}),term,({const char*_tmp3AC=",";
_tag_dynforward(_tmp3AC,sizeof(char),_get_zero_arr_size(_tmp3AC,2));}),((struct
Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp37C));_tmp3AA[0]=Cyc_Absynpp_qvar2doc(
_tmp37B->name);Cyc_PP_cat(_tag_dynforward(_tmp3AA,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL270;}_LL297: if(_tmp358 <= (void*)2)goto _LL270;if(*((int*)_tmp358)!= 14)
goto _LL270;_tmp37E=((struct Cyc_Absyn_Exp_p_struct*)_tmp358)->f1;_LL298: s=Cyc_Absynpp_exp2doc(
_tmp37E);goto _LL270;_LL270:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple8*dp){return({struct Cyc_PP_Doc*_tmp3AF[2];_tmp3AF[1]=Cyc_Absynpp_pat2doc((*
dp).f2);_tmp3AF[0]=Cyc_PP_egroup(({const char*_tmp3B0="";_tag_dynforward(_tmp3B0,
sizeof(char),_get_zero_arr_size(_tmp3B0,1));}),({const char*_tmp3B1="=";
_tag_dynforward(_tmp3B1,sizeof(char),_get_zero_arr_size(_tmp3B1,2));}),({const
char*_tmp3B2="=";_tag_dynforward(_tmp3B2,sizeof(char),_get_zero_arr_size(_tmp3B2,
2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));Cyc_PP_cat(
_tag_dynforward(_tmp3AF,sizeof(struct Cyc_PP_Doc*),2));});}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0  && (void*)(c->pattern)->r
== (void*)((void*)0))return({struct Cyc_PP_Doc*_tmp3B3[2];_tmp3B3[1]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp3B5[2];_tmp3B5[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp3B5[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp3B5,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp3B3[0]=Cyc_PP_text(({const char*_tmp3B4="default: ";_tag_dynforward(
_tmp3B4,sizeof(char),_get_zero_arr_size(_tmp3B4,10));}));Cyc_PP_cat(
_tag_dynforward(_tmp3B3,sizeof(struct Cyc_PP_Doc*),2));});else{if(c->where_clause
== 0)return({struct Cyc_PP_Doc*_tmp3B6[4];_tmp3B6[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp3B9[2];_tmp3B9[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp3B9[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_dynforward(_tmp3B9,sizeof(struct Cyc_PP_Doc*),2));}));_tmp3B6[2]=
Cyc_PP_text(({const char*_tmp3B8=": ";_tag_dynforward(_tmp3B8,sizeof(char),
_get_zero_arr_size(_tmp3B8,3));}));_tmp3B6[1]=Cyc_Absynpp_pat2doc(c->pattern);
_tmp3B6[0]=Cyc_PP_text(({const char*_tmp3B7="case ";_tag_dynforward(_tmp3B7,
sizeof(char),_get_zero_arr_size(_tmp3B7,6));}));Cyc_PP_cat(_tag_dynforward(
_tmp3B6,sizeof(struct Cyc_PP_Doc*),4));});else{return({struct Cyc_PP_Doc*_tmp3BA[6];
_tmp3BA[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3BE[2];_tmp3BE[1]=Cyc_Absynpp_stmt2doc(
c->body);_tmp3BE[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp3BE,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp3BA[4]=Cyc_PP_text(({const char*_tmp3BD=": ";
_tag_dynforward(_tmp3BD,sizeof(char),_get_zero_arr_size(_tmp3BD,3));}));_tmp3BA[
3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));
_tmp3BA[2]=Cyc_PP_text(({const char*_tmp3BC=" && ";_tag_dynforward(_tmp3BC,
sizeof(char),_get_zero_arr_size(_tmp3BC,5));}));_tmp3BA[1]=Cyc_Absynpp_pat2doc(c->pattern);
_tmp3BA[0]=Cyc_PP_text(({const char*_tmp3BB="case ";_tag_dynforward(_tmp3BB,
sizeof(char),_get_zero_arr_size(_tmp3BB,6));}));Cyc_PP_cat(_tag_dynforward(
_tmp3BA,sizeof(struct Cyc_PP_Doc*),6));});}}}struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List*cs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Switch_clause*),struct _dynforward_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,({const char*_tmp3BF="";
_tag_dynforward(_tmp3BF,sizeof(char),_get_zero_arr_size(_tmp3BF,1));}),cs);}
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){if(f->tag
== 0)return Cyc_Absynpp_qvar2doc(f->name);else{return({struct Cyc_PP_Doc*_tmp3C0[3];
_tmp3C0[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));
_tmp3C0[1]=Cyc_PP_text(({const char*_tmp3C1=" = ";_tag_dynforward(_tmp3C1,sizeof(
char),_get_zero_arr_size(_tmp3C1,4));}));_tmp3C0[0]=Cyc_Absynpp_qvar2doc(f->name);
Cyc_PP_cat(_tag_dynforward(_tmp3C0,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dynforward_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,({const char*
_tmp3C2=",";_tag_dynforward(_tmp3C2,sizeof(char),_get_zero_arr_size(_tmp3C2,2));}),
fs);}static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return
Cyc_Absynpp_qvar2doc(v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct
Cyc_List_List*vds){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,({
const char*_tmp3C3=",";_tag_dynforward(_tmp3C3,sizeof(char),_get_zero_arr_size(
_tmp3C3,2));}),vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*
vd){struct Cyc_Absyn_Vardecl _tmp3C5;void*_tmp3C6;struct _tuple0*_tmp3C7;struct Cyc_Absyn_Tqual
_tmp3C8;void*_tmp3C9;struct Cyc_Absyn_Exp*_tmp3CA;struct Cyc_List_List*_tmp3CB;
struct Cyc_Absyn_Vardecl*_tmp3C4=vd;_tmp3C5=*_tmp3C4;_tmp3C6=(void*)_tmp3C5.sc;
_tmp3C7=_tmp3C5.name;_tmp3C8=_tmp3C5.tq;_tmp3C9=(void*)_tmp3C5.type;_tmp3CA=
_tmp3C5.initializer;_tmp3CB=_tmp3C5.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*
sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp3C7);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp3CB);struct Cyc_PP_Doc*beforenamedoc;if(!Cyc_Absynpp_to_VC)beforenamedoc=
attsdoc;else{void*_tmp3CC=Cyc_Tcutil_compress(_tmp3C9);struct Cyc_Absyn_FnInfo
_tmp3CD;struct Cyc_List_List*_tmp3CE;_LL29A: if(_tmp3CC <= (void*)4)goto _LL29C;if(*((
int*)_tmp3CC)!= 8)goto _LL29C;_tmp3CD=((struct Cyc_Absyn_FnType_struct*)_tmp3CC)->f1;
_tmp3CE=_tmp3CD.attributes;_LL29B: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp3CE);
goto _LL299;_LL29C:;_LL29D: beforenamedoc=Cyc_PP_nil_doc();goto _LL299;_LL299:;}s=({
struct Cyc_PP_Doc*_tmp3CF[5];_tmp3CF[4]=Cyc_PP_text(({const char*_tmp3D5=";";
_tag_dynforward(_tmp3D5,sizeof(char),_get_zero_arr_size(_tmp3D5,2));}));_tmp3CF[
3]=_tmp3CA == 0?Cyc_PP_text(({const char*_tmp3D2="";_tag_dynforward(_tmp3D2,
sizeof(char),_get_zero_arr_size(_tmp3D2,1));})):({struct Cyc_PP_Doc*_tmp3D3[2];
_tmp3D3[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp3CA);_tmp3D3[0]=Cyc_PP_text(({
const char*_tmp3D4=" = ";_tag_dynforward(_tmp3D4,sizeof(char),_get_zero_arr_size(
_tmp3D4,4));}));Cyc_PP_cat(_tag_dynforward(_tmp3D3,sizeof(struct Cyc_PP_Doc*),2));});
_tmp3CF[2]=Cyc_Absynpp_tqtd2doc(_tmp3C8,_tmp3C9,({struct Cyc_Core_Opt*_tmp3D0=
_cycalloc(sizeof(*_tmp3D0));_tmp3D0->v=({struct Cyc_PP_Doc*_tmp3D1[2];_tmp3D1[1]=
sn;_tmp3D1[0]=beforenamedoc;Cyc_PP_cat(_tag_dynforward(_tmp3D1,sizeof(struct Cyc_PP_Doc*),
2));});_tmp3D0;}));_tmp3CF[1]=Cyc_Absynpp_scope2doc(_tmp3C6);_tmp3CF[0]=Cyc_Absynpp_to_VC?
attsdoc: Cyc_PP_nil_doc();Cyc_PP_cat(_tag_dynforward(_tmp3CF,sizeof(struct Cyc_PP_Doc*),
5));});return s;}}struct _tuple11{struct Cyc_Position_Segment*f1;struct _tuple0*f2;
int f3;};struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple11*x){struct _tuple0*
_tmp3D7;struct _tuple11 _tmp3D6=*x;_tmp3D7=_tmp3D6.f2;return Cyc_Absynpp_qvar2doc(
_tmp3D7);}struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct
Cyc_PP_Doc*s;{void*_tmp3D8=(void*)d->r;struct Cyc_Absyn_Fndecl*_tmp3D9;struct Cyc_Absyn_Aggrdecl*
_tmp3DA;struct Cyc_Absyn_Vardecl*_tmp3DB;struct Cyc_Absyn_Tuniondecl*_tmp3DC;
struct Cyc_Absyn_Tuniondecl _tmp3DD;void*_tmp3DE;struct _tuple0*_tmp3DF;struct Cyc_List_List*
_tmp3E0;struct Cyc_Core_Opt*_tmp3E1;int _tmp3E2;int _tmp3E3;struct Cyc_Absyn_Pat*
_tmp3E4;struct Cyc_Absyn_Exp*_tmp3E5;struct Cyc_List_List*_tmp3E6;struct Cyc_Absyn_Enumdecl*
_tmp3E7;struct Cyc_Absyn_Enumdecl _tmp3E8;void*_tmp3E9;struct _tuple0*_tmp3EA;
struct Cyc_Core_Opt*_tmp3EB;struct Cyc_Absyn_Typedefdecl*_tmp3EC;struct
_dynforward_ptr*_tmp3ED;struct Cyc_List_List*_tmp3EE;struct _tuple0*_tmp3EF;struct
Cyc_List_List*_tmp3F0;struct Cyc_List_List*_tmp3F1;struct Cyc_List_List*_tmp3F2;
struct Cyc_List_List*_tmp3F3;_LL29F: if(_tmp3D8 <= (void*)2)goto _LL2B7;if(*((int*)
_tmp3D8)!= 1)goto _LL2A1;_tmp3D9=((struct Cyc_Absyn_Fn_d_struct*)_tmp3D8)->f1;
_LL2A0: {void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp3FC=_cycalloc(sizeof(*
_tmp3FC));_tmp3FC[0]=({struct Cyc_Absyn_FnType_struct _tmp3FD;_tmp3FD.tag=8;
_tmp3FD.f1=({struct Cyc_Absyn_FnInfo _tmp3FE;_tmp3FE.tvars=_tmp3D9->tvs;_tmp3FE.effect=
_tmp3D9->effect;_tmp3FE.ret_typ=(void*)((void*)_tmp3D9->ret_type);_tmp3FE.args=((
struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple6*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp3D9->args);_tmp3FE.c_varargs=_tmp3D9->c_varargs;
_tmp3FE.cyc_varargs=_tmp3D9->cyc_varargs;_tmp3FE.rgn_po=_tmp3D9->rgn_po;_tmp3FE.attributes=
0;_tmp3FE;});_tmp3FD;});_tmp3FC;});struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp3D9->attributes);struct Cyc_PP_Doc*inlinedoc;if(_tmp3D9->is_inline){if(Cyc_Absynpp_to_VC)
inlinedoc=Cyc_PP_text(({const char*_tmp3F4="__inline ";_tag_dynforward(_tmp3F4,
sizeof(char),_get_zero_arr_size(_tmp3F4,10));}));else{inlinedoc=Cyc_PP_text(({
const char*_tmp3F5="inline ";_tag_dynforward(_tmp3F5,sizeof(char),
_get_zero_arr_size(_tmp3F5,8));}));}}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*
scopedoc=Cyc_Absynpp_scope2doc((void*)_tmp3D9->sc);struct Cyc_PP_Doc*
beforenamedoc=Cyc_Absynpp_to_VC?Cyc_Absynpp_callconv2doc(_tmp3D9->attributes):
attsdoc;struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp3D9->name);
struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,({
struct Cyc_Core_Opt*_tmp3FA=_cycalloc(sizeof(*_tmp3FA));_tmp3FA->v=({struct Cyc_PP_Doc*
_tmp3FB[2];_tmp3FB[1]=namedoc;_tmp3FB[0]=beforenamedoc;Cyc_PP_cat(
_tag_dynforward(_tmp3FB,sizeof(struct Cyc_PP_Doc*),2));});_tmp3FA;}));struct Cyc_PP_Doc*
bodydoc=({struct Cyc_PP_Doc*_tmp3F8[5];_tmp3F8[4]=Cyc_Absynpp_rb();_tmp3F8[3]=Cyc_PP_line_doc();
_tmp3F8[2]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3F9[2];_tmp3F9[1]=Cyc_Absynpp_stmt2doc(
_tmp3D9->body);_tmp3F9[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp3F9,
sizeof(struct Cyc_PP_Doc*),2));}));_tmp3F8[1]=Cyc_Absynpp_lb();_tmp3F8[0]=Cyc_PP_blank_doc();
Cyc_PP_cat(_tag_dynforward(_tmp3F8,sizeof(struct Cyc_PP_Doc*),5));});s=({struct
Cyc_PP_Doc*_tmp3F6[4];_tmp3F6[3]=bodydoc;_tmp3F6[2]=tqtddoc;_tmp3F6[1]=scopedoc;
_tmp3F6[0]=inlinedoc;Cyc_PP_cat(_tag_dynforward(_tmp3F6,sizeof(struct Cyc_PP_Doc*),
4));});if(Cyc_Absynpp_to_VC)s=({struct Cyc_PP_Doc*_tmp3F7[2];_tmp3F7[1]=s;_tmp3F7[
0]=attsdoc;Cyc_PP_cat(_tag_dynforward(_tmp3F7,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL29E;}}_LL2A1: if(*((int*)_tmp3D8)!= 4)goto _LL2A3;_tmp3DA=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp3D8)->f1;_LL2A2: if(_tmp3DA->impl == 0)s=({struct Cyc_PP_Doc*_tmp3FF[5];_tmp3FF[
4]=Cyc_PP_text(({const char*_tmp400=";";_tag_dynforward(_tmp400,sizeof(char),
_get_zero_arr_size(_tmp400,2));}));_tmp3FF[3]=Cyc_Absynpp_ktvars2doc(_tmp3DA->tvs);
_tmp3FF[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3DA->name);_tmp3FF[1]=Cyc_Absynpp_aggr_kind2doc((
void*)_tmp3DA->kind);_tmp3FF[0]=Cyc_Absynpp_scope2doc((void*)_tmp3DA->sc);Cyc_PP_cat(
_tag_dynforward(_tmp3FF,sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*
_tmp401[12];_tmp401[11]=Cyc_PP_text(({const char*_tmp405=";";_tag_dynforward(
_tmp405,sizeof(char),_get_zero_arr_size(_tmp405,2));}));_tmp401[10]=Cyc_Absynpp_atts2doc(
_tmp3DA->attributes);_tmp401[9]=Cyc_Absynpp_rb();_tmp401[8]=Cyc_PP_line_doc();
_tmp401[7]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp404[2];_tmp404[1]=Cyc_Absynpp_aggrfields2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3DA->impl))->fields);_tmp404[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp404,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp401[6]=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3DA->impl))->rgn_po == 
0?Cyc_PP_nil_doc():({struct Cyc_PP_Doc*_tmp402[2];_tmp402[1]=Cyc_Absynpp_rgnpo2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3DA->impl))->rgn_po);_tmp402[0]=
Cyc_PP_text(({const char*_tmp403=":";_tag_dynforward(_tmp403,sizeof(char),
_get_zero_arr_size(_tmp403,2));}));Cyc_PP_cat(_tag_dynforward(_tmp402,sizeof(
struct Cyc_PP_Doc*),2));});_tmp401[5]=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3DA->impl))->exist_vars);_tmp401[4]=Cyc_Absynpp_lb();_tmp401[3]=
Cyc_PP_blank_doc();_tmp401[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3DA->name);_tmp401[1]=
Cyc_Absynpp_aggr_kind2doc((void*)_tmp3DA->kind);_tmp401[0]=Cyc_Absynpp_scope2doc((
void*)_tmp3DA->sc);Cyc_PP_cat(_tag_dynforward(_tmp401,sizeof(struct Cyc_PP_Doc*),
12));});}goto _LL29E;_LL2A3: if(*((int*)_tmp3D8)!= 0)goto _LL2A5;_tmp3DB=((struct
Cyc_Absyn_Var_d_struct*)_tmp3D8)->f1;_LL2A4: s=Cyc_Absynpp_vardecl2doc(_tmp3DB);
goto _LL29E;_LL2A5: if(*((int*)_tmp3D8)!= 5)goto _LL2A7;_tmp3DC=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp3D8)->f1;_tmp3DD=*_tmp3DC;_tmp3DE=(void*)_tmp3DD.sc;_tmp3DF=_tmp3DD.name;
_tmp3E0=_tmp3DD.tvs;_tmp3E1=_tmp3DD.fields;_tmp3E2=_tmp3DD.is_xtunion;_tmp3E3=
_tmp3DD.is_flat;_LL2A6: if(_tmp3E1 == 0)s=({struct Cyc_PP_Doc*_tmp406[6];_tmp406[5]=
Cyc_PP_text(({const char*_tmp40A=";";_tag_dynforward(_tmp40A,sizeof(char),
_get_zero_arr_size(_tmp40A,2));}));_tmp406[4]=Cyc_Absynpp_ktvars2doc(_tmp3E0);
_tmp406[3]=_tmp3E2?Cyc_Absynpp_qvar2bolddoc(_tmp3DF): Cyc_Absynpp_typedef_name2bolddoc(
_tmp3DF);_tmp406[2]=_tmp3E3?Cyc_PP_text(({const char*_tmp409="__attribute__((flat)) ";
_tag_dynforward(_tmp409,sizeof(char),_get_zero_arr_size(_tmp409,23));})): Cyc_PP_blank_doc();
_tmp406[1]=_tmp3E2?Cyc_PP_text(({const char*_tmp407="xtunion ";_tag_dynforward(
_tmp407,sizeof(char),_get_zero_arr_size(_tmp407,9));})): Cyc_PP_text(({const char*
_tmp408="tunion ";_tag_dynforward(_tmp408,sizeof(char),_get_zero_arr_size(
_tmp408,8));}));_tmp406[0]=Cyc_Absynpp_scope2doc(_tmp3DE);Cyc_PP_cat(
_tag_dynforward(_tmp406,sizeof(struct Cyc_PP_Doc*),6));});else{s=({struct Cyc_PP_Doc*
_tmp40B[11];_tmp40B[10]=Cyc_PP_text(({const char*_tmp410=";";_tag_dynforward(
_tmp410,sizeof(char),_get_zero_arr_size(_tmp410,2));}));_tmp40B[9]=Cyc_Absynpp_rb();
_tmp40B[8]=Cyc_PP_line_doc();_tmp40B[7]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp40F[
2];_tmp40F[1]=Cyc_Absynpp_tunionfields2doc((struct Cyc_List_List*)_tmp3E1->v);
_tmp40F[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp40F,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp40B[6]=Cyc_Absynpp_lb();_tmp40B[5]=Cyc_PP_blank_doc();_tmp40B[4]=
_tmp3E3?Cyc_PP_text(({const char*_tmp40E="__attribute__((flat)) ";_tag_dynforward(
_tmp40E,sizeof(char),_get_zero_arr_size(_tmp40E,23));})): Cyc_PP_blank_doc();
_tmp40B[3]=Cyc_Absynpp_ktvars2doc(_tmp3E0);_tmp40B[2]=_tmp3E2?Cyc_Absynpp_qvar2bolddoc(
_tmp3DF): Cyc_Absynpp_typedef_name2bolddoc(_tmp3DF);_tmp40B[1]=_tmp3E2?Cyc_PP_text(({
const char*_tmp40C="xtunion ";_tag_dynforward(_tmp40C,sizeof(char),
_get_zero_arr_size(_tmp40C,9));})): Cyc_PP_text(({const char*_tmp40D="tunion ";
_tag_dynforward(_tmp40D,sizeof(char),_get_zero_arr_size(_tmp40D,8));}));_tmp40B[
0]=Cyc_Absynpp_scope2doc(_tmp3DE);Cyc_PP_cat(_tag_dynforward(_tmp40B,sizeof(
struct Cyc_PP_Doc*),11));});}goto _LL29E;_LL2A7: if(*((int*)_tmp3D8)!= 2)goto _LL2A9;
_tmp3E4=((struct Cyc_Absyn_Let_d_struct*)_tmp3D8)->f1;_tmp3E5=((struct Cyc_Absyn_Let_d_struct*)
_tmp3D8)->f3;_LL2A8: s=({struct Cyc_PP_Doc*_tmp411[5];_tmp411[4]=Cyc_PP_text(({
const char*_tmp414=";";_tag_dynforward(_tmp414,sizeof(char),_get_zero_arr_size(
_tmp414,2));}));_tmp411[3]=Cyc_Absynpp_exp2doc(_tmp3E5);_tmp411[2]=Cyc_PP_text(({
const char*_tmp413=" = ";_tag_dynforward(_tmp413,sizeof(char),_get_zero_arr_size(
_tmp413,4));}));_tmp411[1]=Cyc_Absynpp_pat2doc(_tmp3E4);_tmp411[0]=Cyc_PP_text(({
const char*_tmp412="let ";_tag_dynforward(_tmp412,sizeof(char),_get_zero_arr_size(
_tmp412,5));}));Cyc_PP_cat(_tag_dynforward(_tmp411,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL29E;_LL2A9: if(*((int*)_tmp3D8)!= 3)goto _LL2AB;_tmp3E6=((struct Cyc_Absyn_Letv_d_struct*)
_tmp3D8)->f1;_LL2AA: s=({struct Cyc_PP_Doc*_tmp415[3];_tmp415[2]=Cyc_PP_text(({
const char*_tmp417=";";_tag_dynforward(_tmp417,sizeof(char),_get_zero_arr_size(
_tmp417,2));}));_tmp415[1]=Cyc_Absynpp_ids2doc(_tmp3E6);_tmp415[0]=Cyc_PP_text(({
const char*_tmp416="let ";_tag_dynforward(_tmp416,sizeof(char),_get_zero_arr_size(
_tmp416,5));}));Cyc_PP_cat(_tag_dynforward(_tmp415,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL29E;_LL2AB: if(*((int*)_tmp3D8)!= 6)goto _LL2AD;_tmp3E7=((struct Cyc_Absyn_Enum_d_struct*)
_tmp3D8)->f1;_tmp3E8=*_tmp3E7;_tmp3E9=(void*)_tmp3E8.sc;_tmp3EA=_tmp3E8.name;
_tmp3EB=_tmp3E8.fields;_LL2AC: if(_tmp3EB == 0)s=({struct Cyc_PP_Doc*_tmp418[4];
_tmp418[3]=Cyc_PP_text(({const char*_tmp41A=";";_tag_dynforward(_tmp41A,sizeof(
char),_get_zero_arr_size(_tmp41A,2));}));_tmp418[2]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp3EA);_tmp418[1]=Cyc_PP_text(({const char*_tmp419="enum ";_tag_dynforward(
_tmp419,sizeof(char),_get_zero_arr_size(_tmp419,6));}));_tmp418[0]=Cyc_Absynpp_scope2doc(
_tmp3E9);Cyc_PP_cat(_tag_dynforward(_tmp418,sizeof(struct Cyc_PP_Doc*),4));});
else{s=({struct Cyc_PP_Doc*_tmp41B[9];_tmp41B[8]=Cyc_PP_text(({const char*_tmp41E=";";
_tag_dynforward(_tmp41E,sizeof(char),_get_zero_arr_size(_tmp41E,2));}));_tmp41B[
7]=Cyc_Absynpp_rb();_tmp41B[6]=Cyc_PP_line_doc();_tmp41B[5]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp41D[2];_tmp41D[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)
_tmp3EB->v);_tmp41D[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp41D,
sizeof(struct Cyc_PP_Doc*),2));}));_tmp41B[4]=Cyc_Absynpp_lb();_tmp41B[3]=Cyc_PP_blank_doc();
_tmp41B[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3EA);_tmp41B[1]=Cyc_PP_text(({const char*
_tmp41C="enum ";_tag_dynforward(_tmp41C,sizeof(char),_get_zero_arr_size(_tmp41C,
6));}));_tmp41B[0]=Cyc_Absynpp_scope2doc(_tmp3E9);Cyc_PP_cat(_tag_dynforward(
_tmp41B,sizeof(struct Cyc_PP_Doc*),9));});}goto _LL29E;_LL2AD: if(*((int*)_tmp3D8)
!= 7)goto _LL2AF;_tmp3EC=((struct Cyc_Absyn_Typedef_d_struct*)_tmp3D8)->f1;_LL2AE: {
void*t;if(_tmp3EC->defn != 0)t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3EC->defn))->v;
else{t=Cyc_Absyn_new_evar(_tmp3EC->kind,0);}s=({struct Cyc_PP_Doc*_tmp41F[4];
_tmp41F[3]=Cyc_PP_text(({const char*_tmp423=";";_tag_dynforward(_tmp423,sizeof(
char),_get_zero_arr_size(_tmp423,2));}));_tmp41F[2]=Cyc_Absynpp_atts2doc(_tmp3EC->atts);
_tmp41F[1]=Cyc_Absynpp_tqtd2doc(_tmp3EC->tq,t,({struct Cyc_Core_Opt*_tmp421=
_cycalloc(sizeof(*_tmp421));_tmp421->v=({struct Cyc_PP_Doc*_tmp422[2];_tmp422[1]=
Cyc_Absynpp_tvars2doc(_tmp3EC->tvs);_tmp422[0]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp3EC->name);Cyc_PP_cat(_tag_dynforward(_tmp422,sizeof(struct Cyc_PP_Doc*),2));});
_tmp421;}));_tmp41F[0]=Cyc_PP_text(({const char*_tmp420="typedef ";
_tag_dynforward(_tmp420,sizeof(char),_get_zero_arr_size(_tmp420,9));}));Cyc_PP_cat(
_tag_dynforward(_tmp41F,sizeof(struct Cyc_PP_Doc*),4));});goto _LL29E;}_LL2AF: if(*((
int*)_tmp3D8)!= 8)goto _LL2B1;_tmp3ED=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp3D8)->f1;_tmp3EE=((struct Cyc_Absyn_Namespace_d_struct*)_tmp3D8)->f2;_LL2B0:
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp3ED);s=({
struct Cyc_PP_Doc*_tmp424[8];_tmp424[7]=Cyc_Absynpp_rb();_tmp424[6]=Cyc_PP_line_doc();
_tmp424[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp426="";_tag_dynforward(_tmp426,sizeof(char),_get_zero_arr_size(
_tmp426,1));}),_tmp3EE);_tmp424[4]=Cyc_PP_line_doc();_tmp424[3]=Cyc_Absynpp_lb();
_tmp424[2]=Cyc_PP_blank_doc();_tmp424[1]=Cyc_PP_textptr(_tmp3ED);_tmp424[0]=Cyc_PP_text(({
const char*_tmp425="namespace ";_tag_dynforward(_tmp425,sizeof(char),
_get_zero_arr_size(_tmp425,11));}));Cyc_PP_cat(_tag_dynforward(_tmp424,sizeof(
struct Cyc_PP_Doc*),8));});if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL29E;_LL2B1: if(*((int*)_tmp3D8)!= 9)goto _LL2B3;_tmp3EF=((struct Cyc_Absyn_Using_d_struct*)
_tmp3D8)->f1;_tmp3F0=((struct Cyc_Absyn_Using_d_struct*)_tmp3D8)->f2;_LL2B2: if(
Cyc_Absynpp_print_using_stmts)s=({struct Cyc_PP_Doc*_tmp427[8];_tmp427[7]=Cyc_Absynpp_rb();
_tmp427[6]=Cyc_PP_line_doc();_tmp427[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp429="";_tag_dynforward(_tmp429,sizeof(char),
_get_zero_arr_size(_tmp429,1));}),_tmp3F0);_tmp427[4]=Cyc_PP_line_doc();_tmp427[
3]=Cyc_Absynpp_lb();_tmp427[2]=Cyc_PP_blank_doc();_tmp427[1]=Cyc_Absynpp_qvar2doc(
_tmp3EF);_tmp427[0]=Cyc_PP_text(({const char*_tmp428="using ";_tag_dynforward(
_tmp428,sizeof(char),_get_zero_arr_size(_tmp428,7));}));Cyc_PP_cat(
_tag_dynforward(_tmp427,sizeof(struct Cyc_PP_Doc*),8));});else{s=({struct Cyc_PP_Doc*
_tmp42A[11];_tmp42A[10]=Cyc_PP_text(({const char*_tmp42F=" */";_tag_dynforward(
_tmp42F,sizeof(char),_get_zero_arr_size(_tmp42F,4));}));_tmp42A[9]=Cyc_Absynpp_rb();
_tmp42A[8]=Cyc_PP_text(({const char*_tmp42E="/* ";_tag_dynforward(_tmp42E,sizeof(
char),_get_zero_arr_size(_tmp42E,4));}));_tmp42A[7]=Cyc_PP_line_doc();_tmp42A[6]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp42D="";_tag_dynforward(_tmp42D,sizeof(char),_get_zero_arr_size(
_tmp42D,1));}),_tmp3F0);_tmp42A[5]=Cyc_PP_line_doc();_tmp42A[4]=Cyc_PP_text(({
const char*_tmp42C=" */";_tag_dynforward(_tmp42C,sizeof(char),_get_zero_arr_size(
_tmp42C,4));}));_tmp42A[3]=Cyc_Absynpp_lb();_tmp42A[2]=Cyc_PP_blank_doc();
_tmp42A[1]=Cyc_Absynpp_qvar2doc(_tmp3EF);_tmp42A[0]=Cyc_PP_text(({const char*
_tmp42B="/* using ";_tag_dynforward(_tmp42B,sizeof(char),_get_zero_arr_size(
_tmp42B,10));}));Cyc_PP_cat(_tag_dynforward(_tmp42A,sizeof(struct Cyc_PP_Doc*),11));});}
goto _LL29E;_LL2B3: if(*((int*)_tmp3D8)!= 10)goto _LL2B5;_tmp3F1=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp3D8)->f1;_LL2B4: if(Cyc_Absynpp_print_externC_stmts)s=({struct Cyc_PP_Doc*
_tmp430[6];_tmp430[5]=Cyc_Absynpp_rb();_tmp430[4]=Cyc_PP_line_doc();_tmp430[3]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp432="";_tag_dynforward(_tmp432,sizeof(char),_get_zero_arr_size(
_tmp432,1));}),_tmp3F1);_tmp430[2]=Cyc_PP_line_doc();_tmp430[1]=Cyc_Absynpp_lb();
_tmp430[0]=Cyc_PP_text(({const char*_tmp431="extern \"C\" ";_tag_dynforward(
_tmp431,sizeof(char),_get_zero_arr_size(_tmp431,12));}));Cyc_PP_cat(
_tag_dynforward(_tmp430,sizeof(struct Cyc_PP_Doc*),6));});else{s=({struct Cyc_PP_Doc*
_tmp433[9];_tmp433[8]=Cyc_PP_text(({const char*_tmp438=" */";_tag_dynforward(
_tmp438,sizeof(char),_get_zero_arr_size(_tmp438,4));}));_tmp433[7]=Cyc_Absynpp_rb();
_tmp433[6]=Cyc_PP_text(({const char*_tmp437="/* ";_tag_dynforward(_tmp437,sizeof(
char),_get_zero_arr_size(_tmp437,4));}));_tmp433[5]=Cyc_PP_line_doc();_tmp433[4]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp436="";_tag_dynforward(_tmp436,sizeof(char),_get_zero_arr_size(
_tmp436,1));}),_tmp3F1);_tmp433[3]=Cyc_PP_line_doc();_tmp433[2]=Cyc_PP_text(({
const char*_tmp435=" */";_tag_dynforward(_tmp435,sizeof(char),_get_zero_arr_size(
_tmp435,4));}));_tmp433[1]=Cyc_Absynpp_lb();_tmp433[0]=Cyc_PP_text(({const char*
_tmp434="/* extern \"C\" ";_tag_dynforward(_tmp434,sizeof(char),
_get_zero_arr_size(_tmp434,15));}));Cyc_PP_cat(_tag_dynforward(_tmp433,sizeof(
struct Cyc_PP_Doc*),9));});}goto _LL29E;_LL2B5: if(*((int*)_tmp3D8)!= 11)goto _LL2B7;
_tmp3F2=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp3D8)->f1;_tmp3F3=((struct
Cyc_Absyn_ExternCinclude_d_struct*)_tmp3D8)->f2;_LL2B6: if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;if(_tmp3F3 != 0)exs_doc=({struct Cyc_PP_Doc*_tmp439[7];
_tmp439[6]=Cyc_Absynpp_rb();_tmp439[5]=Cyc_PP_line_doc();_tmp439[4]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct _tuple11*),struct _dynforward_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,({const char*_tmp43B=",";_tag_dynforward(
_tmp43B,sizeof(char),_get_zero_arr_size(_tmp43B,2));}),_tmp3F3);_tmp439[3]=Cyc_PP_line_doc();
_tmp439[2]=Cyc_Absynpp_lb();_tmp439[1]=Cyc_PP_text(({const char*_tmp43A=" export ";
_tag_dynforward(_tmp43A,sizeof(char),_get_zero_arr_size(_tmp43A,9));}));_tmp439[
0]=Cyc_Absynpp_rb();Cyc_PP_cat(_tag_dynforward(_tmp439,sizeof(struct Cyc_PP_Doc*),
7));});else{exs_doc=Cyc_Absynpp_rb();}s=({struct Cyc_PP_Doc*_tmp43C[6];_tmp43C[5]=
exs_doc;_tmp43C[4]=Cyc_PP_line_doc();_tmp43C[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp43E="";_tag_dynforward(_tmp43E,sizeof(char),
_get_zero_arr_size(_tmp43E,1));}),_tmp3F2);_tmp43C[2]=Cyc_PP_line_doc();_tmp43C[
1]=Cyc_Absynpp_lb();_tmp43C[0]=Cyc_PP_text(({const char*_tmp43D="extern \"C include\" ";
_tag_dynforward(_tmp43D,sizeof(char),_get_zero_arr_size(_tmp43D,20));}));Cyc_PP_cat(
_tag_dynforward(_tmp43C,sizeof(struct Cyc_PP_Doc*),6));});}else{s=({struct Cyc_PP_Doc*
_tmp43F[9];_tmp43F[8]=Cyc_PP_text(({const char*_tmp444=" */";_tag_dynforward(
_tmp444,sizeof(char),_get_zero_arr_size(_tmp444,4));}));_tmp43F[7]=Cyc_Absynpp_rb();
_tmp43F[6]=Cyc_PP_text(({const char*_tmp443="/* ";_tag_dynforward(_tmp443,sizeof(
char),_get_zero_arr_size(_tmp443,4));}));_tmp43F[5]=Cyc_PP_line_doc();_tmp43F[4]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp442="";_tag_dynforward(_tmp442,sizeof(char),_get_zero_arr_size(
_tmp442,1));}),_tmp3F2);_tmp43F[3]=Cyc_PP_line_doc();_tmp43F[2]=Cyc_PP_text(({
const char*_tmp441=" */";_tag_dynforward(_tmp441,sizeof(char),_get_zero_arr_size(
_tmp441,4));}));_tmp43F[1]=Cyc_Absynpp_lb();_tmp43F[0]=Cyc_PP_text(({const char*
_tmp440="/* extern \"C include\" ";_tag_dynforward(_tmp440,sizeof(char),
_get_zero_arr_size(_tmp440,23));}));Cyc_PP_cat(_tag_dynforward(_tmp43F,sizeof(
struct Cyc_PP_Doc*),9));});}goto _LL29E;_LL2B7: if((int)_tmp3D8 != 0)goto _LL2B9;
_LL2B8: s=({struct Cyc_PP_Doc*_tmp445[2];_tmp445[1]=Cyc_Absynpp_lb();_tmp445[0]=
Cyc_PP_text(({const char*_tmp446="__cyclone_port_on__;";_tag_dynforward(_tmp446,
sizeof(char),_get_zero_arr_size(_tmp446,21));}));Cyc_PP_cat(_tag_dynforward(
_tmp445,sizeof(struct Cyc_PP_Doc*),2));});goto _LL29E;_LL2B9: if((int)_tmp3D8 != 1)
goto _LL29E;_LL2BA: s=({struct Cyc_PP_Doc*_tmp447[2];_tmp447[1]=Cyc_Absynpp_lb();
_tmp447[0]=Cyc_PP_text(({const char*_tmp448="__cyclone_port_off__;";
_tag_dynforward(_tmp448,sizeof(char),_get_zero_arr_size(_tmp448,22));}));Cyc_PP_cat(
_tag_dynforward(_tmp447,sizeof(struct Cyc_PP_Doc*),2));});goto _LL29E;_LL29E:;}
return s;}int Cyc_Absynpp_print_scopes=1;struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
void*sc){if(!Cyc_Absynpp_print_scopes)return Cyc_PP_nil_doc();{void*_tmp449=sc;
_LL2BC: if((int)_tmp449 != 0)goto _LL2BE;_LL2BD: return Cyc_PP_text(({const char*
_tmp44A="static ";_tag_dynforward(_tmp44A,sizeof(char),_get_zero_arr_size(
_tmp44A,8));}));_LL2BE: if((int)_tmp449 != 2)goto _LL2C0;_LL2BF: return Cyc_PP_nil_doc();
_LL2C0: if((int)_tmp449 != 3)goto _LL2C2;_LL2C1: return Cyc_PP_text(({const char*
_tmp44B="extern ";_tag_dynforward(_tmp44B,sizeof(char),_get_zero_arr_size(
_tmp44B,8));}));_LL2C2: if((int)_tmp449 != 4)goto _LL2C4;_LL2C3: return Cyc_PP_text(({
const char*_tmp44C="extern \"C\" ";_tag_dynforward(_tmp44C,sizeof(char),
_get_zero_arr_size(_tmp44C,12));}));_LL2C4: if((int)_tmp449 != 1)goto _LL2C6;_LL2C5:
return Cyc_PP_text(({const char*_tmp44D="abstract ";_tag_dynforward(_tmp44D,
sizeof(char),_get_zero_arr_size(_tmp44D,10));}));_LL2C6: if((int)_tmp449 != 5)goto
_LL2BB;_LL2C7: return Cyc_PP_text(({const char*_tmp44E="register ";_tag_dynforward(
_tmp44E,sizeof(char),_get_zero_arr_size(_tmp44E,10));}));_LL2BB:;}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t){void*_tmp44F=t;struct Cyc_Absyn_Tvar*_tmp450;struct Cyc_List_List*_tmp451;
_LL2C9: if(_tmp44F <= (void*)4)goto _LL2CD;if(*((int*)_tmp44F)!= 1)goto _LL2CB;
_tmp450=((struct Cyc_Absyn_VarType_struct*)_tmp44F)->f1;_LL2CA: return Cyc_Tcutil_is_temp_tvar(
_tmp450);_LL2CB: if(*((int*)_tmp44F)!= 20)goto _LL2CD;_tmp451=((struct Cyc_Absyn_JoinEff_struct*)
_tmp44F)->f1;_LL2CC: return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp451);_LL2CD:;_LL2CE: return 0;_LL2C8:;}int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp452=t;void**_tmp453;void*_tmp454;_LL2D0: if(_tmp452 <= (void*)4)goto
_LL2D6;if(*((int*)_tmp452)!= 11)goto _LL2D2;_LL2D1: return 1;_LL2D2: if(*((int*)
_tmp452)!= 13)goto _LL2D4;_LL2D3: return 1;_LL2D4: if(*((int*)_tmp452)!= 16)goto
_LL2D6;_tmp453=((struct Cyc_Absyn_TypedefType_struct*)_tmp452)->f4;if(_tmp453 == 0)
goto _LL2D6;_tmp454=*_tmp453;_LL2D5: return Cyc_Absynpp_is_anon_aggrtype(_tmp454);
_LL2D6:;_LL2D7: return 0;_LL2CF:;}static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(
struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){if(tms != 0  && tms->tl != 
0){struct _tuple5 _tmp456=({struct _tuple5 _tmp455;_tmp455.f1=(void*)tms->hd;_tmp455.f2=(
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp455;});void*_tmp457;
void*_tmp458;_LL2D9: _tmp457=_tmp456.f1;if(*((int*)_tmp457)!= 2)goto _LL2DB;
_tmp458=_tmp456.f2;if(*((int*)_tmp458)!= 3)goto _LL2DB;_LL2DA: return({struct Cyc_List_List*
_tmp459=_region_malloc(r,sizeof(*_tmp459));_tmp459->hd=(void*)((void*)tms->hd);
_tmp459->tl=({struct Cyc_List_List*_tmp45A=_region_malloc(r,sizeof(*_tmp45A));
_tmp45A->hd=(void*)((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd);
_tmp45A->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)
_check_null(tms->tl))->tl);_tmp45A;});_tmp459;});_LL2DB:;_LL2DC: return({struct
Cyc_List_List*_tmp45B=_region_malloc(r,sizeof(*_tmp45B));_tmp45B->hd=(void*)atts;
_tmp45B->tl=tms;_tmp45B;});_LL2D8:;}else{return({struct Cyc_List_List*_tmp45C=
_region_malloc(r,sizeof(*_tmp45C));_tmp45C->hd=(void*)atts;_tmp45C->tl=tms;
_tmp45C;});}}struct _tuple7 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t){void*_tmp45D=t;struct Cyc_Absyn_ArrayInfo _tmp45E;void*_tmp45F;struct
Cyc_Absyn_Tqual _tmp460;struct Cyc_Absyn_Exp*_tmp461;struct Cyc_Absyn_Conref*
_tmp462;struct Cyc_Position_Segment*_tmp463;struct Cyc_Absyn_PtrInfo _tmp464;void*
_tmp465;struct Cyc_Absyn_Tqual _tmp466;struct Cyc_Absyn_PtrAtts _tmp467;struct Cyc_Absyn_FnInfo
_tmp468;struct Cyc_List_List*_tmp469;struct Cyc_Core_Opt*_tmp46A;void*_tmp46B;
struct Cyc_List_List*_tmp46C;int _tmp46D;struct Cyc_Absyn_VarargInfo*_tmp46E;struct
Cyc_List_List*_tmp46F;struct Cyc_List_List*_tmp470;struct Cyc_Core_Opt*_tmp471;
struct Cyc_Core_Opt*_tmp472;int _tmp473;struct _tuple0*_tmp474;struct Cyc_List_List*
_tmp475;void**_tmp476;_LL2DE: if(_tmp45D <= (void*)4)goto _LL2E8;if(*((int*)_tmp45D)
!= 7)goto _LL2E0;_tmp45E=((struct Cyc_Absyn_ArrayType_struct*)_tmp45D)->f1;_tmp45F=(
void*)_tmp45E.elt_type;_tmp460=_tmp45E.tq;_tmp461=_tmp45E.num_elts;_tmp462=
_tmp45E.zero_term;_tmp463=_tmp45E.zt_loc;_LL2DF: {struct Cyc_Absyn_Tqual _tmp478;
void*_tmp479;struct Cyc_List_List*_tmp47A;struct _tuple7 _tmp477=Cyc_Absynpp_to_tms(
r,_tmp460,_tmp45F);_tmp478=_tmp477.f1;_tmp479=_tmp477.f2;_tmp47A=_tmp477.f3;{
void*tm;if(_tmp461 == 0)tm=(void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp47B=
_cycalloc(sizeof(*_tmp47B));_tmp47B[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp47C;_tmp47C.tag=0;_tmp47C.f1=_tmp462;_tmp47C.f2=_tmp463;_tmp47C;});_tmp47B;});
else{tm=(void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp47D=_cycalloc(sizeof(*
_tmp47D));_tmp47D[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp47E;_tmp47E.tag=
1;_tmp47E.f1=(struct Cyc_Absyn_Exp*)_tmp461;_tmp47E.f2=_tmp462;_tmp47E.f3=_tmp463;
_tmp47E;});_tmp47D;});}return({struct _tuple7 _tmp47F;_tmp47F.f1=_tmp478;_tmp47F.f2=
_tmp479;_tmp47F.f3=({struct Cyc_List_List*_tmp480=_region_malloc(r,sizeof(*
_tmp480));_tmp480->hd=(void*)tm;_tmp480->tl=_tmp47A;_tmp480;});_tmp47F;});}}
_LL2E0: if(*((int*)_tmp45D)!= 4)goto _LL2E2;_tmp464=((struct Cyc_Absyn_PointerType_struct*)
_tmp45D)->f1;_tmp465=(void*)_tmp464.elt_typ;_tmp466=_tmp464.elt_tq;_tmp467=
_tmp464.ptr_atts;_LL2E1: {struct Cyc_Absyn_Tqual _tmp482;void*_tmp483;struct Cyc_List_List*
_tmp484;struct _tuple7 _tmp481=Cyc_Absynpp_to_tms(r,_tmp466,_tmp465);_tmp482=
_tmp481.f1;_tmp483=_tmp481.f2;_tmp484=_tmp481.f3;_tmp484=({struct Cyc_List_List*
_tmp485=_region_malloc(r,sizeof(*_tmp485));_tmp485->hd=(void*)((void*)({struct
Cyc_Absyn_Pointer_mod_struct*_tmp486=_cycalloc(sizeof(*_tmp486));_tmp486[0]=({
struct Cyc_Absyn_Pointer_mod_struct _tmp487;_tmp487.tag=2;_tmp487.f1=_tmp467;
_tmp487.f2=tq;_tmp487;});_tmp486;}));_tmp485->tl=_tmp484;_tmp485;});return({
struct _tuple7 _tmp488;_tmp488.f1=_tmp482;_tmp488.f2=_tmp483;_tmp488.f3=_tmp484;
_tmp488;});}_LL2E2: if(*((int*)_tmp45D)!= 8)goto _LL2E4;_tmp468=((struct Cyc_Absyn_FnType_struct*)
_tmp45D)->f1;_tmp469=_tmp468.tvars;_tmp46A=_tmp468.effect;_tmp46B=(void*)_tmp468.ret_typ;
_tmp46C=_tmp468.args;_tmp46D=_tmp468.c_varargs;_tmp46E=_tmp468.cyc_varargs;
_tmp46F=_tmp468.rgn_po;_tmp470=_tmp468.attributes;_LL2E3: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp46A == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp46A->v)){
_tmp46A=0;_tmp469=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp469);}{struct Cyc_Absyn_Tqual _tmp48A;void*_tmp48B;struct Cyc_List_List*_tmp48C;
struct _tuple7 _tmp489=Cyc_Absynpp_to_tms(r,Cyc_Absyn_empty_tqual(0),_tmp46B);
_tmp48A=_tmp489.f1;_tmp48B=_tmp489.f2;_tmp48C=_tmp489.f3;{struct Cyc_List_List*
tms=_tmp48C;if(_tmp470 != 0  && !Cyc_Absynpp_to_VC)tms=Cyc_Absynpp_bubble_attributes(
r,(void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp48D=_cycalloc(sizeof(*
_tmp48D));_tmp48D[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp48E;_tmp48E.tag=
5;_tmp48E.f1=0;_tmp48E.f2=_tmp470;_tmp48E;});_tmp48D;}),tms);tms=({struct Cyc_List_List*
_tmp48F=_region_malloc(r,sizeof(*_tmp48F));_tmp48F->hd=(void*)((void*)({struct
Cyc_Absyn_Function_mod_struct*_tmp490=_cycalloc(sizeof(*_tmp490));_tmp490[0]=({
struct Cyc_Absyn_Function_mod_struct _tmp491;_tmp491.tag=3;_tmp491.f1=(void*)((
void*)({struct Cyc_Absyn_WithTypes_struct*_tmp492=_cycalloc(sizeof(*_tmp492));
_tmp492[0]=({struct Cyc_Absyn_WithTypes_struct _tmp493;_tmp493.tag=1;_tmp493.f1=
_tmp46C;_tmp493.f2=_tmp46D;_tmp493.f3=_tmp46E;_tmp493.f4=_tmp46A;_tmp493.f5=
_tmp46F;_tmp493;});_tmp492;}));_tmp491;});_tmp490;}));_tmp48F->tl=tms;_tmp48F;});
if(Cyc_Absynpp_to_VC)for(0;_tmp470 != 0;_tmp470=_tmp470->tl){void*_tmp494=(void*)
_tmp470->hd;_LL2EB: if((int)_tmp494 != 0)goto _LL2ED;_LL2EC: goto _LL2EE;_LL2ED: if((
int)_tmp494 != 1)goto _LL2EF;_LL2EE: goto _LL2F0;_LL2EF: if((int)_tmp494 != 2)goto
_LL2F1;_LL2F0: tms=({struct Cyc_List_List*_tmp495=_region_malloc(r,sizeof(*_tmp495));
_tmp495->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp496=
_cycalloc(sizeof(*_tmp496));_tmp496[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp497;_tmp497.tag=5;_tmp497.f1=0;_tmp497.f2=({struct Cyc_List_List*_tmp498=
_cycalloc(sizeof(*_tmp498));_tmp498->hd=(void*)((void*)_tmp470->hd);_tmp498->tl=
0;_tmp498;});_tmp497;});_tmp496;}));_tmp495->tl=tms;_tmp495;});goto AfterAtts;
_LL2F1:;_LL2F2: goto _LL2EA;_LL2EA:;}AfterAtts: if(_tmp469 != 0)tms=({struct Cyc_List_List*
_tmp499=_region_malloc(r,sizeof(*_tmp499));_tmp499->hd=(void*)((void*)({struct
Cyc_Absyn_TypeParams_mod_struct*_tmp49A=_cycalloc(sizeof(*_tmp49A));_tmp49A[0]=({
struct Cyc_Absyn_TypeParams_mod_struct _tmp49B;_tmp49B.tag=4;_tmp49B.f1=_tmp469;
_tmp49B.f2=0;_tmp49B.f3=1;_tmp49B;});_tmp49A;}));_tmp499->tl=tms;_tmp499;});
return({struct _tuple7 _tmp49C;_tmp49C.f1=_tmp48A;_tmp49C.f2=_tmp48B;_tmp49C.f3=
tms;_tmp49C;});}}_LL2E4: if(*((int*)_tmp45D)!= 0)goto _LL2E6;_tmp471=((struct Cyc_Absyn_Evar_struct*)
_tmp45D)->f1;_tmp472=((struct Cyc_Absyn_Evar_struct*)_tmp45D)->f2;_tmp473=((
struct Cyc_Absyn_Evar_struct*)_tmp45D)->f3;_LL2E5: if(_tmp472 == 0)return({struct
_tuple7 _tmp49D;_tmp49D.f1=tq;_tmp49D.f2=t;_tmp49D.f3=0;_tmp49D;});else{return Cyc_Absynpp_to_tms(
r,tq,(void*)_tmp472->v);}_LL2E6: if(*((int*)_tmp45D)!= 16)goto _LL2E8;_tmp474=((
struct Cyc_Absyn_TypedefType_struct*)_tmp45D)->f1;_tmp475=((struct Cyc_Absyn_TypedefType_struct*)
_tmp45D)->f2;_tmp476=((struct Cyc_Absyn_TypedefType_struct*)_tmp45D)->f4;_LL2E7:
if((_tmp476 == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(*
_tmp476))return({struct _tuple7 _tmp49E;_tmp49E.f1=tq;_tmp49E.f2=t;_tmp49E.f3=0;
_tmp49E;});else{return Cyc_Absynpp_to_tms(r,tq,*_tmp476);}_LL2E8:;_LL2E9: return({
struct _tuple7 _tmp49F;_tmp49F.f1=tq;_tmp49F.f2=t;_tmp49F.f3=0;_tmp49F;});_LL2DD:;}
static int Cyc_Absynpp_is_char_ptr(void*t){void*_tmp4A0=t;struct Cyc_Core_Opt*
_tmp4A1;struct Cyc_Core_Opt _tmp4A2;void*_tmp4A3;struct Cyc_Absyn_PtrInfo _tmp4A4;
void*_tmp4A5;_LL2F4: if(_tmp4A0 <= (void*)4)goto _LL2F8;if(*((int*)_tmp4A0)!= 0)
goto _LL2F6;_tmp4A1=((struct Cyc_Absyn_Evar_struct*)_tmp4A0)->f2;if(_tmp4A1 == 0)
goto _LL2F6;_tmp4A2=*_tmp4A1;_tmp4A3=(void*)_tmp4A2.v;_LL2F5: return Cyc_Absynpp_is_char_ptr(
_tmp4A3);_LL2F6: if(*((int*)_tmp4A0)!= 4)goto _LL2F8;_tmp4A4=((struct Cyc_Absyn_PointerType_struct*)
_tmp4A0)->f1;_tmp4A5=(void*)_tmp4A4.elt_typ;_LL2F7: L: {void*_tmp4A6=_tmp4A5;
struct Cyc_Core_Opt*_tmp4A7;struct Cyc_Core_Opt _tmp4A8;void*_tmp4A9;void**_tmp4AA;
void*_tmp4AB;void*_tmp4AC;_LL2FB: if(_tmp4A6 <= (void*)4)goto _LL301;if(*((int*)
_tmp4A6)!= 0)goto _LL2FD;_tmp4A7=((struct Cyc_Absyn_Evar_struct*)_tmp4A6)->f2;if(
_tmp4A7 == 0)goto _LL2FD;_tmp4A8=*_tmp4A7;_tmp4A9=(void*)_tmp4A8.v;_LL2FC: _tmp4A5=
_tmp4A9;goto L;_LL2FD: if(*((int*)_tmp4A6)!= 16)goto _LL2FF;_tmp4AA=((struct Cyc_Absyn_TypedefType_struct*)
_tmp4A6)->f4;if(_tmp4AA == 0)goto _LL2FF;_tmp4AB=*_tmp4AA;_LL2FE: _tmp4A5=_tmp4AB;
goto L;_LL2FF: if(*((int*)_tmp4A6)!= 5)goto _LL301;_tmp4AC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4A6)->f2;if((int)_tmp4AC != 0)goto _LL301;_LL300: return 1;_LL301:;_LL302: return
0;_LL2FA:;}_LL2F8:;_LL2F9: return 0;_LL2F3:;}struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){struct _RegionHandle
_tmp4AD=_new_region("temp");struct _RegionHandle*temp=& _tmp4AD;_push_region(temp);{
struct Cyc_Absyn_Tqual _tmp4AF;void*_tmp4B0;struct Cyc_List_List*_tmp4B1;struct
_tuple7 _tmp4AE=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp4AF=_tmp4AE.f1;_tmp4B0=
_tmp4AE.f2;_tmp4B1=_tmp4AE.f3;_tmp4B1=Cyc_List_imp_rev(_tmp4B1);if(_tmp4B1 == 0
 && dopt == 0){struct Cyc_PP_Doc*_tmp4B3=({struct Cyc_PP_Doc*_tmp4B2[2];_tmp4B2[1]=
Cyc_Absynpp_ntyp2doc(_tmp4B0);_tmp4B2[0]=Cyc_Absynpp_tqual2doc(_tmp4AF);Cyc_PP_cat(
_tag_dynforward(_tmp4B2,sizeof(struct Cyc_PP_Doc*),2));});_npop_handler(0);return
_tmp4B3;}else{struct Cyc_PP_Doc*_tmp4B6=({struct Cyc_PP_Doc*_tmp4B4[4];_tmp4B4[3]=
Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(
struct Cyc_PP_Doc*)dopt->v,_tmp4B1);_tmp4B4[2]=Cyc_PP_text(({const char*_tmp4B5=" ";
_tag_dynforward(_tmp4B5,sizeof(char),_get_zero_arr_size(_tmp4B5,2));}));_tmp4B4[
1]=Cyc_Absynpp_ntyp2doc(_tmp4B0);_tmp4B4[0]=Cyc_Absynpp_tqual2doc(_tmp4AF);Cyc_PP_cat(
_tag_dynforward(_tmp4B4,sizeof(struct Cyc_PP_Doc*),4));});_npop_handler(0);return
_tmp4B6;}};_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct
Cyc_Absyn_Aggrfield*f){if(f->width != 0)return({struct Cyc_PP_Doc*_tmp4B7[5];
_tmp4B7[4]=Cyc_PP_text(({const char*_tmp4BA=";";_tag_dynforward(_tmp4BA,sizeof(
char),_get_zero_arr_size(_tmp4BA,2));}));_tmp4B7[3]=Cyc_Absynpp_atts2doc(f->attributes);
_tmp4B7[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));
_tmp4B7[1]=Cyc_PP_text(({const char*_tmp4B9=":";_tag_dynforward(_tmp4B9,sizeof(
char),_get_zero_arr_size(_tmp4B9,2));}));_tmp4B7[0]=Cyc_Absynpp_tqtd2doc(f->tq,(
void*)f->type,({struct Cyc_Core_Opt*_tmp4B8=_cycalloc(sizeof(*_tmp4B8));_tmp4B8->v=
Cyc_PP_textptr(f->name);_tmp4B8;}));Cyc_PP_cat(_tag_dynforward(_tmp4B7,sizeof(
struct Cyc_PP_Doc*),5));});else{return({struct Cyc_PP_Doc*_tmp4BB[3];_tmp4BB[2]=
Cyc_PP_text(({const char*_tmp4BD=";";_tag_dynforward(_tmp4BD,sizeof(char),
_get_zero_arr_size(_tmp4BD,2));}));_tmp4BB[1]=Cyc_Absynpp_atts2doc(f->attributes);
_tmp4BB[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,({struct Cyc_Core_Opt*
_tmp4BC=_cycalloc(sizeof(*_tmp4BC));_tmp4BC->v=Cyc_PP_textptr(f->name);_tmp4BC;}));
Cyc_PP_cat(_tag_dynforward(_tmp4BB,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dynforward_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,({const char*
_tmp4BE="";_tag_dynforward(_tmp4BE,sizeof(char),_get_zero_arr_size(_tmp4BE,1));}),
fields);}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfield2doc(struct Cyc_Absyn_Tunionfield*
f){return({struct Cyc_PP_Doc*_tmp4BF[3];_tmp4BF[2]=f->typs == 0?Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc(f->typs);_tmp4BF[1]=Cyc_Absynpp_typedef_name2doc(f->name);
_tmp4BF[0]=Cyc_Absynpp_scope2doc((void*)f->sc);Cyc_PP_cat(_tag_dynforward(
_tmp4BF,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Tunionfield*),struct _dynforward_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_tunionfield2doc,({const char*_tmp4C0=",";
_tag_dynforward(_tmp4C0,sizeof(char),_get_zero_arr_size(_tmp4C0,2));}),fields);}
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)
tdl->hd),72,f);({void*_tmp4C1[0]={};Cyc_fprintf(f,({const char*_tmp4C2="\n";
_tag_dynforward(_tmp4C2,sizeof(char),_get_zero_arr_size(_tmp4C2,2));}),
_tag_dynforward(_tmp4C1,sizeof(void*),0));});}}struct _dynforward_ptr Cyc_Absynpp_decllist2string(
struct Cyc_List_List*tdl){return Cyc_PP_string_of_doc(Cyc_PP_seql(({const char*
_tmp4C3="";_tag_dynforward(_tmp4C3,sizeof(char),_get_zero_arr_size(_tmp4C3,1));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),72);}struct _dynforward_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _dynforward_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(
Cyc_Absynpp_stmt2doc(s),72);}struct _dynforward_ptr Cyc_Absynpp_typ2string(void*t){
return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}struct _dynforward_ptr Cyc_Absynpp_typ2cstring(
void*t){int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;Cyc_Absynpp_add_cyc_prefix=0;{struct _dynforward_ptr s=
Cyc_Absynpp_typ2string(t);Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
old_add_cyc_prefix;return s;}}struct _dynforward_ptr Cyc_Absynpp_prim2string(void*p){
return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}struct _dynforward_ptr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dynforward_ptr Cyc_Absynpp_scope2string(void*sc){return Cyc_PP_string_of_doc(
Cyc_Absynpp_scope2doc(sc),72);}
