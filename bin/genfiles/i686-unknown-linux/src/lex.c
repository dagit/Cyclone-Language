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
typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;
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
char*tag;struct _dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);extern char Cyc_List_Nth[8];extern char Cyc_Lexing_Error[10];
struct Cyc_Lexing_Error_struct{char*tag;struct _dynforward_ptr f1;};struct Cyc_Lexing_lexbuf{
void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct
_dynforward_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int
lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dynforward_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dynforward_ptr lex_base;struct _dynforward_ptr lex_backtrk;struct
_dynforward_ptr lex_default;struct _dynforward_ptr lex_trans;struct _dynforward_ptr
lex_check;};struct _dynforward_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);
char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);int Cyc_Lexing_lexeme_start(
struct Cyc_Lexing_lexbuf*);int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*
cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);void Cyc_Set_iter(void(*f)(void*),
struct Cyc_Set_Set*s);extern char Cyc_Set_Absent[11];unsigned int Cyc_strlen(struct
_dynforward_ptr s);int Cyc_zstrptrcmp(struct _dynforward_ptr*,struct _dynforward_ptr*);
struct _dynforward_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dynforward_ptr);
struct _dynforward_ptr Cyc_zstrncpy(struct _dynforward_ptr,struct _dynforward_ptr,
unsigned int);struct _dynforward_ptr Cyc_substring(struct _dynforward_ptr,int ofs,
unsigned int n);struct Cyc_Xarray_Xarray{struct _dynforward_ptr elmts;int num_elmts;}
;void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);struct Cyc_Xarray_Xarray*Cyc_Xarray_create(
int,void*);void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);int Cyc_Xarray_add_ind(
struct Cyc_Xarray_Xarray*,void*);struct Cyc_Lineno_Pos{struct _dynforward_ptr
logical_file;struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct Cyc_Position_Error{struct _dynforward_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dynforward_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_lex(
struct Cyc_Position_Segment*,struct _dynforward_ptr);struct Cyc_Position_Error*Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*,struct _dynforward_ptr);extern char Cyc_Position_Nocontext[
14];void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Absyn_Loc_n_struct{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Declaration_spec;struct Cyc_Declarator;
struct Cyc_Abstractdeclarator;struct _tuple3{void*f1;int f2;};struct Cyc_Int_tok_struct{
int tag;struct _tuple3*f1;};struct Cyc_Char_tok_struct{int tag;char f1;};struct Cyc_String_tok_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Stringopt_tok_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_QualId_tok_struct{int tag;struct _tuple0*f1;};struct _tuple4{struct
Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Absyn_Conref*f3;};
struct Cyc_YY1_struct{int tag;struct _tuple4*f1;};struct Cyc_YY2_struct{int tag;
struct Cyc_Absyn_Conref*f1;};struct Cyc_YY3_struct{int tag;struct Cyc_Absyn_Exp*f1;}
;struct Cyc_YY4_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY5_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY6_struct{int tag;void*f1;};struct Cyc_YY7_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY8_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_YY9_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY10_struct{
int tag;struct Cyc_Absyn_Pat*f1;};struct _tuple5{struct Cyc_List_List*f1;int f2;};
struct Cyc_YY11_struct{int tag;struct _tuple5*f1;};struct Cyc_YY12_struct{int tag;
struct Cyc_List_List*f1;};struct _tuple6{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};struct Cyc_YY13_struct{int tag;struct _tuple6*f1;};struct Cyc_YY14_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY15_struct{int tag;struct _tuple5*f1;};
struct Cyc_YY16_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_YY17_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY18_struct{int tag;struct Cyc_Declaration_spec*
f1;};struct _tuple7{struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_YY19_struct{
int tag;struct _tuple7*f1;};struct Cyc_YY20_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY21_struct{int tag;void*f1;};struct Cyc_YY22_struct{int tag;void*f1;};
struct Cyc_YY23_struct{int tag;void*f1;};struct Cyc_YY24_struct{int tag;struct Cyc_Absyn_Tqual
f1;};struct Cyc_YY25_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY26_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY27_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY28_struct{int tag;struct Cyc_Declarator*f1;};struct Cyc_YY29_struct{
int tag;struct Cyc_Abstractdeclarator*f1;};struct Cyc_YY30_struct{int tag;int f1;};
struct Cyc_YY31_struct{int tag;void*f1;};struct Cyc_YY32_struct{int tag;struct Cyc_Absyn_Tunionfield*
f1;};struct Cyc_YY33_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct
Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_YY34_struct{
int tag;struct _tuple8*f1;};struct Cyc_YY35_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY36_struct{int tag;struct _tuple1*f1;};struct Cyc_YY37_struct{int tag;
struct Cyc_List_List*f1;};struct _tuple9{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*
f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_YY38_struct{int tag;
struct _tuple9*f1;};struct Cyc_YY39_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_YY40_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY41_struct{int tag;
void*f1;};struct Cyc_YY42_struct{int tag;void*f1;};struct Cyc_YY43_struct{int tag;
void*f1;};struct Cyc_YY44_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY45_struct{
int tag;void*f1;};struct Cyc_YY46_struct{int tag;struct Cyc_Absyn_Enumfield*f1;};
struct Cyc_YY47_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY48_struct{int
tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY49_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY50_struct{int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_YY51_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YYINITIALSVAL_struct{int tag;};union Cyc_YYSTYPE_union{
struct Cyc_Int_tok_struct Int_tok;struct Cyc_Char_tok_struct Char_tok;struct Cyc_String_tok_struct
String_tok;struct Cyc_Stringopt_tok_struct Stringopt_tok;struct Cyc_QualId_tok_struct
QualId_tok;struct Cyc_YY1_struct YY1;struct Cyc_YY2_struct YY2;struct Cyc_YY3_struct
YY3;struct Cyc_YY4_struct YY4;struct Cyc_YY5_struct YY5;struct Cyc_YY6_struct YY6;
struct Cyc_YY7_struct YY7;struct Cyc_YY8_struct YY8;struct Cyc_YY9_struct YY9;struct
Cyc_YY10_struct YY10;struct Cyc_YY11_struct YY11;struct Cyc_YY12_struct YY12;struct
Cyc_YY13_struct YY13;struct Cyc_YY14_struct YY14;struct Cyc_YY15_struct YY15;struct
Cyc_YY16_struct YY16;struct Cyc_YY17_struct YY17;struct Cyc_YY18_struct YY18;struct
Cyc_YY19_struct YY19;struct Cyc_YY20_struct YY20;struct Cyc_YY21_struct YY21;struct
Cyc_YY22_struct YY22;struct Cyc_YY23_struct YY23;struct Cyc_YY24_struct YY24;struct
Cyc_YY25_struct YY25;struct Cyc_YY26_struct YY26;struct Cyc_YY27_struct YY27;struct
Cyc_YY28_struct YY28;struct Cyc_YY29_struct YY29;struct Cyc_YY30_struct YY30;struct
Cyc_YY31_struct YY31;struct Cyc_YY32_struct YY32;struct Cyc_YY33_struct YY33;struct
Cyc_YY34_struct YY34;struct Cyc_YY35_struct YY35;struct Cyc_YY36_struct YY36;struct
Cyc_YY37_struct YY37;struct Cyc_YY38_struct YY38;struct Cyc_YY39_struct YY39;struct
Cyc_YY40_struct YY40;struct Cyc_YY41_struct YY41;struct Cyc_YY42_struct YY42;struct
Cyc_YY43_struct YY43;struct Cyc_YY44_struct YY44;struct Cyc_YY45_struct YY45;struct
Cyc_YY46_struct YY46;struct Cyc_YY47_struct YY47;struct Cyc_YY48_struct YY48;struct
Cyc_YY49_struct YY49;struct Cyc_YY50_struct YY50;struct Cyc_YY51_struct YY51;struct
Cyc_YYINITIALSVAL_struct YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int
first_line;int first_column;int last_line;int last_column;};extern struct Cyc_Yyltype
Cyc_yylloc;extern union Cyc_YYSTYPE_union Cyc_yylval;struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(
void*,void*));struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,
void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct _tuple10{void*f1;
void*f2;};struct _tuple10*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict
d);struct _tuple10*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
void Cyc_yyerror(struct _dynforward_ptr s){Cyc_Position_post_error(Cyc_Position_mk_err_parse(
Cyc_Position_segment_of_abs(Cyc_yylloc.first_line,Cyc_yylloc.last_line),s));}
struct Cyc_Lex_Trie;struct Cyc_Lex_Zero_struct{int tag;};struct Cyc_Lex_One_struct{
int tag;int f1;struct Cyc_Lex_Trie*f2;};struct Cyc_Lex_Many_struct{int tag;struct Cyc_Lex_Trie**
f1;};union Cyc_Lex_TrieChildren_union{struct Cyc_Lex_Zero_struct Zero;struct Cyc_Lex_One_struct
One;struct Cyc_Lex_Many_struct Many;};struct Cyc_Lex_Trie{union Cyc_Lex_TrieChildren_union
children;int shared_str;};static int Cyc_Lex_num_kws=0;static struct _dynforward_ptr
Cyc_Lex_kw_nums={(void*)0,(void*)(0 + 0)};static struct Cyc_Xarray_Xarray*Cyc_Lex_symbols=
0;static struct Cyc_Lex_Trie*Cyc_Lex_ids_trie=0;static struct Cyc_Lex_Trie*Cyc_Lex_typedefs_trie=
0;static int Cyc_Lex_comment_depth=0;static struct _tuple3 Cyc_Lex_token_int_pair={(
void*)0,0};static char _tmp0[8]="*bogus*";static struct _dynforward_ptr Cyc_Lex_bogus_string={
_tmp0,_tmp0 + 8};static struct _tuple0 Cyc_Lex_token_id_pair={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Lex_bogus_string};static char Cyc_Lex_token_char='\000';
static char _tmp2[1]="";static struct _dynforward_ptr Cyc_Lex_token_string={_tmp2,
_tmp2 + 1};static struct _tuple3*Cyc_Lex_token_int=& Cyc_Lex_token_int_pair;static
struct _tuple0*Cyc_Lex_token_qvar=& Cyc_Lex_token_id_pair;static int Cyc_Lex_runaway_start=
0;static void Cyc_Lex_err(struct _dynforward_ptr msg,struct Cyc_Lexing_lexbuf*lb){
struct Cyc_Position_Segment*s=Cyc_Position_segment_of_abs(Cyc_Lexing_lexeme_start(
lb),Cyc_Lexing_lexeme_end(lb));Cyc_Position_post_error(Cyc_Position_mk_err_lex(s,
msg));}static void Cyc_Lex_runaway_err(struct _dynforward_ptr msg,struct Cyc_Lexing_lexbuf*
lb){struct Cyc_Position_Segment*s=Cyc_Position_segment_of_abs(Cyc_Lex_runaway_start,
Cyc_Lexing_lexeme_start(lb));Cyc_Position_post_error(Cyc_Position_mk_err_lex(s,
msg));}struct _tuple11{struct _dynforward_ptr f1;short f2;int f3;};static char _tmp3[14]="__attribute__";
static char _tmp4[9]="abstract";static char _tmp5[5]="auto";static char _tmp6[6]="break";
static char _tmp7[7]="calloc";static char _tmp8[5]="case";static char _tmp9[6]="catch";
static char _tmpA[5]="char";static char _tmpB[6]="const";static char _tmpC[9]="continue";
static char _tmpD[8]="default";static char _tmpE[3]="do";static char _tmpF[7]="double";
static char _tmp10[5]="else";static char _tmp11[5]="enum";static char _tmp12[7]="export";
static char _tmp13[7]="extern";static char _tmp14[9]="fallthru";static char _tmp15[9]="__flat__";
static char _tmp16[6]="float";static char _tmp17[4]="for";static char _tmp18[6]="__gen";
static char _tmp19[5]="goto";static char _tmp1A[3]="if";static char _tmp1B[7]="inline";
static char _tmp1C[11]="__inline__";static char _tmp1D[4]="int";static char _tmp1E[4]="let";
static char _tmp1F[5]="long";static char _tmp20[7]="malloc";static char _tmp21[10]="namespace";
static char _tmp22[4]="new";static char _tmp23[11]="NOZEROTERM";static char _tmp24[5]="NULL";
static char _tmp25[9]="offsetof";static char _tmp26[20]="__cyclone_port_on__";static
char _tmp27[21]="__cyclone_port_off__";static char _tmp28[8]="rcalloc";static char
_tmp29[9]="region_t";static char _tmp2A[7]="region";static char _tmp2B[8]="regions";
static char _tmp2C[9]="register";static char _tmp2D[13]="reset_region";static char
_tmp2E[9]="restrict";static char _tmp2F[7]="return";static char _tmp30[8]="rmalloc";
static char _tmp31[5]="rnew";static char _tmp32[6]="short";static char _tmp33[7]="signed";
static char _tmp34[7]="sizeof";static char _tmp35[9]="sizeof_t";static char _tmp36[7]="static";
static char _tmp37[7]="struct";static char _tmp38[7]="switch";static char _tmp39[6]="tag_t";
static char _tmp3A[6]="throw";static char _tmp3B[4]="try";static char _tmp3C[7]="tunion";
static char _tmp3D[8]="typedef";static char _tmp3E[6]="union";static char _tmp3F[9]="unsigned";
static char _tmp40[6]="using";static char _tmp41[5]="void";static char _tmp42[9]="volatile";
static char _tmp43[6]="while";static char _tmp44[8]="xtunion";static char _tmp45[9]="ZEROTERM";
static struct _tuple11 Cyc_Lex_rw_array[67]={{{_tmp3,_tmp3 + 14},357,1},{{_tmp4,
_tmp4 + 9},300,0},{{_tmp5,_tmp5 + 5},258,1},{{_tmp6,_tmp6 + 6},290,1},{{_tmp7,_tmp7
+ 7},308,0},{{_tmp8,_tmp8 + 5},277,1},{{_tmp9,_tmp9 + 6},297,1},{{_tmpA,_tmpA + 5},
264,1},{{_tmpB,_tmpB + 6},272,1},{{_tmpC,_tmpC + 9},289,1},{{_tmpD,_tmpD + 8},278,1},{{
_tmpE,_tmpE + 3},286,1},{{_tmpF,_tmpF + 7},269,1},{{_tmp10,_tmp10 + 5},283,1},{{
_tmp11,_tmp11 + 5},292,1},{{_tmp12,_tmp12 + 7},298,0},{{_tmp13,_tmp13 + 7},261,1},{{
_tmp14,_tmp14 + 9},301,0},{{_tmp15,_tmp15 + 9},322,0},{{_tmp16,_tmp16 + 6},268,1},{{
_tmp17,_tmp17 + 4},287,1},{{_tmp18,_tmp18 + 6},317,0},{{_tmp19,_tmp19 + 5},288,1},{{
_tmp1A,_tmp1A + 3},282,1},{{_tmp1B,_tmp1B + 7},279,1},{{_tmp1C,_tmp1C + 11},279,1},{{
_tmp1D,_tmp1D + 4},266,1},{{_tmp1E,_tmp1E + 4},294,0},{{_tmp1F,_tmp1F + 5},267,1},{{
_tmp20,_tmp20 + 7},306,0},{{_tmp21,_tmp21 + 10},303,0},{{_tmp22,_tmp22 + 4},299,0},{{
_tmp23,_tmp23 + 11},318,0},{{_tmp24,_tmp24 + 5},293,0},{{_tmp25,_tmp25 + 9},281,1},{{
_tmp26,_tmp26 + 20},320,0},{{_tmp27,_tmp27 + 21},321,0},{{_tmp28,_tmp28 + 8},309,0},{{
_tmp29,_tmp29 + 9},310,0},{{_tmp2A,_tmp2A + 7},313,0},{{_tmp2B,_tmp2B + 8},315,0},{{
_tmp2C,_tmp2C + 9},259,1},{{_tmp2D,_tmp2D + 13},316,0},{{_tmp2E,_tmp2E + 9},274,1},{{
_tmp2F,_tmp2F + 7},291,1},{{_tmp30,_tmp30 + 8},307,0},{{_tmp31,_tmp31 + 5},314,0},{{
_tmp32,_tmp32 + 6},265,1},{{_tmp33,_tmp33 + 7},270,1},{{_tmp34,_tmp34 + 7},280,1},{{
_tmp35,_tmp35 + 9},311,0},{{_tmp36,_tmp36 + 7},260,1},{{_tmp37,_tmp37 + 7},275,1},{{
_tmp38,_tmp38 + 7},284,1},{{_tmp39,_tmp39 + 6},312,0},{{_tmp3A,_tmp3A + 6},295,0},{{
_tmp3B,_tmp3B + 4},296,0},{{_tmp3C,_tmp3C + 7},304,0},{{_tmp3D,_tmp3D + 8},262,1},{{
_tmp3E,_tmp3E + 6},276,1},{{_tmp3F,_tmp3F + 9},271,1},{{_tmp40,_tmp40 + 6},302,0},{{
_tmp41,_tmp41 + 5},263,1},{{_tmp42,_tmp42 + 9},273,1},{{_tmp43,_tmp43 + 6},285,1},{{
_tmp44,_tmp44 + 8},305,0},{{_tmp45,_tmp45 + 9},319,0}};static int Cyc_Lex_num_keywords(
int include_cyclone_keywords){int sum=0;{unsigned int i=0;for(0;i < 67;i ++){if(
include_cyclone_keywords  || (Cyc_Lex_rw_array[(int)i]).f3)sum ++;}}return sum;}
static int Cyc_Lex_trie_char(int c){if(c >= 95)return c - 59;else{if(c > 64)return c - 55;}
return c - 48;}static struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct Cyc_Lex_Trie*t,
struct _dynforward_ptr buff,int offset,int len){int i=offset;int last=(offset + len)- 1;
while(i <= last){union Cyc_Lex_TrieChildren_union _tmp46=((struct Cyc_Lex_Trie*)
_check_null(t))->children;struct Cyc_Lex_Trie**_tmp47;int _tmp48;struct Cyc_Lex_Trie*
_tmp49;_LL1: if((_tmp46.Many).tag != 2)goto _LL3;_tmp47=(_tmp46.Many).f1;_LL2: {int
ch=Cyc_Lex_trie_char((int)*((const char*)_check_dynforward_subscript(buff,sizeof(
char),i)));if(_tmp47[_check_known_subscript_notnull(64,ch)]== 0)_tmp47[
_check_known_subscript_notnull(64,ch)]=({struct Cyc_Lex_Trie*_tmp4A=_cycalloc(
sizeof(*_tmp4A));_tmp4A->children=(union Cyc_Lex_TrieChildren_union)({union Cyc_Lex_TrieChildren_union
_tmp4B;(_tmp4B.Zero).tag=0;_tmp4B;});_tmp4A->shared_str=0;_tmp4A;});t=_tmp47[
_check_known_subscript_notnull(64,ch)];++ i;goto _LL0;}_LL3: if((_tmp46.One).tag != 
1)goto _LL5;_tmp48=(_tmp46.One).f1;_tmp49=(_tmp46.One).f2;_LL4: if(_tmp48 == *((
const char*)_check_dynforward_subscript(buff,sizeof(char),i)))t=_tmp49;else{
struct Cyc_Lex_Trie**_tmp4C=({unsigned int _tmp51=64;struct Cyc_Lex_Trie**_tmp52=(
struct Cyc_Lex_Trie**)_cycalloc(_check_times(sizeof(struct Cyc_Lex_Trie*),_tmp51));{
unsigned int _tmp53=_tmp51;unsigned int j;for(j=0;j < _tmp53;j ++){_tmp52[j]=0;}}
_tmp52;});_tmp4C[_check_known_subscript_notnull(64,Cyc_Lex_trie_char(_tmp48))]=
_tmp49;{int _tmp4D=Cyc_Lex_trie_char((int)*((const char*)
_check_dynforward_subscript(buff,sizeof(char),i)));_tmp4C[
_check_known_subscript_notnull(64,_tmp4D)]=({struct Cyc_Lex_Trie*_tmp4E=_cycalloc(
sizeof(*_tmp4E));_tmp4E->children=(union Cyc_Lex_TrieChildren_union)({union Cyc_Lex_TrieChildren_union
_tmp4F;(_tmp4F.Zero).tag=0;_tmp4F;});_tmp4E->shared_str=0;_tmp4E;});t->children=(
union Cyc_Lex_TrieChildren_union)({union Cyc_Lex_TrieChildren_union _tmp50;(_tmp50.Many).tag=
2;(_tmp50.Many).f1=_tmp4C;_tmp50;});t=_tmp4C[_check_known_subscript_notnull(64,
_tmp4D)];}}++ i;goto _LL0;_LL5: if((_tmp46.Zero).tag != 0)goto _LL0;_LL6: while(i <= 
last){struct Cyc_Lex_Trie*_tmp54=({struct Cyc_Lex_Trie*_tmp56=_cycalloc(sizeof(*
_tmp56));_tmp56->children=(union Cyc_Lex_TrieChildren_union)({union Cyc_Lex_TrieChildren_union
_tmp57;(_tmp57.Zero).tag=0;_tmp57;});_tmp56->shared_str=0;_tmp56;});((struct Cyc_Lex_Trie*)
_check_null(t))->children=(union Cyc_Lex_TrieChildren_union)({union Cyc_Lex_TrieChildren_union
_tmp55;(_tmp55.One).tag=1;(_tmp55.One).f1=(int)*((const char*)
_check_dynforward_subscript(buff,sizeof(char),i ++));(_tmp55.One).f2=_tmp54;
_tmp55;});t=_tmp54;}return t;_LL0:;}return t;}static int Cyc_Lex_str_index(struct
_dynforward_ptr buff,int offset,int len){struct Cyc_Lex_Trie*_tmp58=Cyc_Lex_trie_lookup(
Cyc_Lex_ids_trie,buff,offset,len);if(_tmp58->shared_str == 0){struct
_dynforward_ptr _tmp59=Cyc_Core_new_string((unsigned int)(len + 1));Cyc_zstrncpy(
_dynforward_ptr_decrease_size(_tmp59,sizeof(char),1),(struct _dynforward_ptr)
_dynforward_ptr_plus(buff,sizeof(char),offset),(unsigned int)len);{int ans=((int(*)(
struct Cyc_Xarray_Xarray*,struct _dynforward_ptr*))Cyc_Xarray_add_ind)((struct Cyc_Xarray_Xarray*)
_check_null(Cyc_Lex_symbols),({struct _dynforward_ptr*_tmp5A=_cycalloc(sizeof(*
_tmp5A));_tmp5A[0]=(struct _dynforward_ptr)_tmp59;_tmp5A;}));_tmp58->shared_str=
ans;}}return _tmp58->shared_str;}static int Cyc_Lex_str_index_lbuf(struct Cyc_Lexing_lexbuf*
lbuf){return Cyc_Lex_str_index((struct _dynforward_ptr)lbuf->lex_buffer,lbuf->lex_start_pos,
lbuf->lex_curr_pos - lbuf->lex_start_pos);}static void Cyc_Lex_insert_typedef(
struct _dynforward_ptr*sptr){struct _dynforward_ptr _tmp5B=*sptr;struct Cyc_Lex_Trie*
_tmp5C=Cyc_Lex_trie_lookup(Cyc_Lex_typedefs_trie,_tmp5B,0,(int)(
_get_dynforward_size(_tmp5B,sizeof(char))- 1));((struct Cyc_Lex_Trie*)_check_null(
_tmp5C))->shared_str=1;return;}static struct _dynforward_ptr*Cyc_Lex_get_symbol(
int symbol_num){return((struct _dynforward_ptr*(*)(struct Cyc_Xarray_Xarray*,int))
Cyc_Xarray_get)((struct Cyc_Xarray_Xarray*)_check_null(Cyc_Lex_symbols),
symbol_num);}static int Cyc_Lex_int_of_char(char c){if('0' <= c  && c <= '9')return c - '0';
else{if('a' <= c  && c <= 'f')return(10 + c)- 'a';else{if('A' <= c  && c <= 'F')return(
10 + c)- 'A';else{(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp5E;_tmp5E.tag=Cyc_Core_Invalid_argument;_tmp5E.f1=({const char*_tmp5F="string to integer conversion";
_tag_dynforward(_tmp5F,sizeof(char),_get_zero_arr_size(_tmp5F,29));});_tmp5E;});
_tmp5D;}));}}}}static struct _tuple3*Cyc_Lex_intconst(struct Cyc_Lexing_lexbuf*lbuf,
int start,int end,int base){unsigned int n=0;int end2=lbuf->lex_curr_pos - end;struct
_dynforward_ptr buff=lbuf->lex_buffer;int i=start + lbuf->lex_start_pos;{int i=start
+ lbuf->lex_start_pos;for(0;i < end2;++ i){char c=*((char*)
_check_dynforward_subscript(buff,sizeof(char),i));switch(c){case 'u': _LL7: goto
_LL8;case 'U': _LL8: return({struct _tuple3*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->f1=(
void*)1;_tmp60->f2=(int)n;_tmp60;});case 'l': _LL9: break;case 'L': _LLA: break;
default: _LLB: n=n * base + (unsigned int)Cyc_Lex_int_of_char(c);break;}}}return({
struct _tuple3*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->f1=(void*)0;_tmp61->f2=(
int)n;_tmp61;});}char Cyc_Lex_string_buffer_v[11]={'x','x','x','x','x','x','x','x','x','x','\000'};
struct _dynforward_ptr Cyc_Lex_string_buffer={(void*)Cyc_Lex_string_buffer_v,(void*)(
Cyc_Lex_string_buffer_v + 11)};int Cyc_Lex_string_pos=0;void Cyc_Lex_store_string_char(
char c){int sz=(int)(_get_dynforward_size(Cyc_Lex_string_buffer,sizeof(char))- 1);
if(Cyc_Lex_string_pos >= sz){int newsz=sz;while(Cyc_Lex_string_pos >= newsz){newsz=
newsz * 2;}{struct _dynforward_ptr str=({unsigned int _tmp62=(unsigned int)newsz;
char*_tmp63=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp62 + 1));struct
_dynforward_ptr _tmp65=_tag_dynforward(_tmp63,sizeof(char),_tmp62 + 1);{
unsigned int _tmp64=_tmp62;unsigned int i;for(i=0;i < _tmp64;i ++){_tmp63[i]=i < sz?*((
char*)_check_dynforward_subscript(Cyc_Lex_string_buffer,sizeof(char),(int)i)):'\000';}
_tmp63[_tmp64]=(char)0;}_tmp65;});Cyc_Lex_string_buffer=str;}}({struct
_dynforward_ptr _tmp66=_dynforward_ptr_plus(Cyc_Lex_string_buffer,sizeof(char),
Cyc_Lex_string_pos);char _tmp67=*((char*)_check_dynforward_subscript(_tmp66,
sizeof(char),0));char _tmp68=c;if(_get_dynforward_size(_tmp66,sizeof(char))== 1
 && (_tmp67 == '\000'  && _tmp68 != '\000'))_throw_arraybounds();*((char*)_tmp66.curr)=
_tmp68;});++ Cyc_Lex_string_pos;}struct _dynforward_ptr Cyc_Lex_get_stored_string(){
struct _dynforward_ptr str=Cyc_substring((struct _dynforward_ptr)Cyc_Lex_string_buffer,
0,(unsigned int)Cyc_Lex_string_pos);Cyc_Lex_string_pos=0;return str;}struct Cyc_Lex_Ldecls{
struct Cyc_Set_Set*typedefs;struct Cyc_Set_Set*namespaces;};struct Cyc_Lex_Lvis{
struct Cyc_List_List*current_namespace;struct Cyc_List_List*imported_namespaces;};
struct Cyc_Lex_Lstate{struct Cyc_List_List*lstack;struct Cyc_Dict_Dict decls;};
static struct Cyc_Core_Opt*Cyc_Lex_lstate=0;static void Cyc_Lex_typedef_init(){
struct Cyc_Lex_Lvis*_tmp69=({struct Cyc_Lex_Lvis*_tmp6F=_cycalloc(sizeof(*_tmp6F));
_tmp6F->current_namespace=0;_tmp6F->imported_namespaces=0;_tmp6F;});struct Cyc_List_List*
_tmp6A=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=_tmp69;
_tmp6E->tl=0;_tmp6E;});struct Cyc_Dict_Dict init_decls=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(
Cyc_Absyn_varlist_cmp),0,({struct Cyc_Lex_Ldecls*_tmp6D=_cycalloc(sizeof(*_tmp6D));
_tmp6D->typedefs=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dynforward_ptr*,struct
_dynforward_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp);_tmp6D->namespaces=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp);
_tmp6D;}));Cyc_Lex_lstate=({struct Cyc_Core_Opt*_tmp6B=_cycalloc(sizeof(*_tmp6B));
_tmp6B->v=({struct Cyc_Lex_Lstate*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->lstack=
_tmp6A;_tmp6C->decls=init_decls;_tmp6C;});_tmp6B;});}static struct Cyc_List_List*
Cyc_Lex_get_absolute_namespace(struct Cyc_List_List*ns){struct _dynforward_ptr*n=(
struct _dynforward_ptr*)ns->hd;{struct Cyc_List_List*ls=(struct Cyc_List_List*)((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack;
for(0;ls != 0;ls=ls->tl){struct Cyc_Lex_Lvis*lv=(struct Cyc_Lex_Lvis*)ls->hd;struct
Cyc_List_List*x=({struct Cyc_List_List*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->hd=
lv->current_namespace;_tmp70->tl=lv->imported_namespaces;_tmp70;});for(0;x != 0;x=((
struct Cyc_List_List*)_check_null(x))->tl){struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(struct Cyc_List_List*)((
struct Cyc_List_List*)_check_null(x))->hd);if(((int(*)(struct Cyc_Set_Set*s,struct
_dynforward_ptr*elt))Cyc_Set_member)(ld->namespaces,n))return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((struct Cyc_List_List*)((
struct Cyc_List_List*)_check_null(x))->hd,(struct Cyc_List_List*)ns);}}}Cyc_yyerror((
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp73;_tmp73.tag=0;_tmp73.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_str_sepstr((struct Cyc_List_List*)
ns,({const char*_tmp74="::";_tag_dynforward(_tmp74,sizeof(char),
_get_zero_arr_size(_tmp74,3));})));{void*_tmp71[1]={& _tmp73};Cyc_aprintf(({const
char*_tmp72="undeclared namespace %s";_tag_dynforward(_tmp72,sizeof(char),
_get_zero_arr_size(_tmp72,24));}),_tag_dynforward(_tmp71,sizeof(void*),1));}}));
return 0;}static void Cyc_Lex_recompute_typedefs(){Cyc_Lex_typedefs_trie=({struct
Cyc_Lex_Trie*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->children=(union Cyc_Lex_TrieChildren_union)({
union Cyc_Lex_TrieChildren_union _tmp76;(_tmp76.Zero).tag=0;_tmp76;});_tmp75->shared_str=
0;_tmp75;});{struct Cyc_List_List*ls=(struct Cyc_List_List*)((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack;for(0;ls != 0;ls=ls->tl){
struct Cyc_Lex_Lvis*lv=(struct Cyc_Lex_Lvis*)ls->hd;struct Cyc_List_List*x=({struct
Cyc_List_List*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->hd=lv->current_namespace;
_tmp77->tl=lv->imported_namespaces;_tmp77;});for(0;x != 0;x=((struct Cyc_List_List*)
_check_null(x))->tl){struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(struct Cyc_List_List*)((
struct Cyc_List_List*)_check_null(x))->hd);((void(*)(void(*f)(struct
_dynforward_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,ld->typedefs);}}}}
static int Cyc_Lex_is_typedef_in_namespace(struct Cyc_List_List*ns,struct
_dynforward_ptr*v){struct Cyc_List_List*ans=Cyc_Lex_get_absolute_namespace(ns);
struct _handler_cons _tmp78;_push_handler(& _tmp78);{int _tmp7A=0;if(setjmp(_tmp78.handler))
_tmp7A=1;if(!_tmp7A){{struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,ans);int _tmp7B=((int(*)(
struct Cyc_Set_Set*s,struct _dynforward_ptr*elt))Cyc_Set_member)(ld->typedefs,v);
_npop_handler(0);return _tmp7B;};_pop_handler();}else{void*_tmp79=(void*)
_exn_thrown;void*_tmp7D=_tmp79;_LLE: if(_tmp7D != Cyc_Dict_Absent)goto _LL10;_LLF:
return 0;_LL10:;_LL11:(void)_throw(_tmp7D);_LLD:;}}}static int Cyc_Lex_is_typedef(
struct Cyc_List_List*ns,struct _dynforward_ptr*v){if(ns != 0)return Cyc_Lex_is_typedef_in_namespace((
struct Cyc_List_List*)ns,v);{struct _dynforward_ptr _tmp7E=*v;int len=(int)(
_get_dynforward_size(_tmp7E,sizeof(char))- 1);struct Cyc_Lex_Trie*t=Cyc_Lex_typedefs_trie;{
int i=0;for(0;i < len;++ i){union Cyc_Lex_TrieChildren_union _tmp7F=((struct Cyc_Lex_Trie*)
_check_null(t))->children;int _tmp80;struct Cyc_Lex_Trie*_tmp81;struct Cyc_Lex_Trie*
_tmp82;struct Cyc_Lex_Trie**_tmp83;_LL13: if((_tmp7F.Zero).tag != 0)goto _LL15;_LL14:
return 0;_LL15: if((_tmp7F.One).tag != 1)goto _LL17;_tmp80=(_tmp7F.One).f1;_tmp81=(
_tmp7F.One).f2;if(!(_tmp80 != *((const char*)_check_dynforward_subscript(_tmp7E,
sizeof(char),i))))goto _LL17;_LL16: return 0;_LL17: if((_tmp7F.One).tag != 1)goto
_LL19;_tmp82=(_tmp7F.One).f2;_LL18: t=_tmp82;goto _LL12;_LL19: if((_tmp7F.Many).tag
!= 2)goto _LL12;_tmp83=(_tmp7F.Many).f1;_LL1A: {struct Cyc_Lex_Trie*_tmp84=_tmp83[
_check_known_subscript_notnull(64,Cyc_Lex_trie_char((int)*((const char*)
_check_dynforward_subscript(_tmp7E,sizeof(char),i))))];if(_tmp84 == 0)return 0;t=
_tmp84;goto _LL12;}_LL12:;}}return((struct Cyc_Lex_Trie*)_check_null(t))->shared_str;}}
void Cyc_Lex_enter_namespace(struct _dynforward_ptr*s){struct Cyc_List_List*ns=((
struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;struct Cyc_List_List*new_ns=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
ns,({struct Cyc_List_List*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->hd=s;_tmp89->tl=
0;_tmp89;}));((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack=({
struct Cyc_List_List*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->hd=({struct Cyc_Lex_Lvis*
_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->current_namespace=new_ns;_tmp86->imported_namespaces=
0;_tmp86;});_tmp85->tl=(struct Cyc_List_List*)((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->lstack;_tmp85;});{struct Cyc_Lex_Ldecls*ld=((
struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,
ns);if(!((int(*)(struct Cyc_Set_Set*s,struct _dynforward_ptr*elt))Cyc_Set_member)(
ld->namespaces,s)){((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*
v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->decls,ns,({struct Cyc_Lex_Ldecls*_tmp87=_cycalloc(sizeof(*
_tmp87));_tmp87->typedefs=ld->typedefs;_tmp87->namespaces=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _dynforward_ptr*elt))Cyc_Set_insert)(ld->namespaces,s);
_tmp87;}));((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*
v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->decls,new_ns,({struct Cyc_Lex_Ldecls*_tmp88=_cycalloc(
sizeof(*_tmp88));_tmp88->typedefs=((struct Cyc_Set_Set*(*)(int(*cmp)(struct
_dynforward_ptr*,struct _dynforward_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp);_tmp88->namespaces=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*)))
Cyc_Set_empty)(Cyc_zstrptrcmp);_tmp88;}));}((void(*)(void(*f)(struct
_dynforward_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,(((
struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,
new_ns))->typedefs);}}void Cyc_Lex_leave_namespace(){((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack=(struct Cyc_List_List*)
_check_null((((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->tl);
Cyc_Lex_recompute_typedefs();}void Cyc_Lex_enter_using(struct _tuple0*q){struct Cyc_List_List*
ns;{union Cyc_Absyn_Nmspace_union _tmp8A=(*q).f1;struct Cyc_List_List*_tmp8B;struct
Cyc_List_List*_tmp8C;_LL1C: if((_tmp8A.Loc_n).tag != 0)goto _LL1E;_LL1D: ns=(struct
Cyc_List_List*)({struct Cyc_List_List*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->hd=(*
q).f2;_tmp8D->tl=0;_tmp8D;});goto _LL1B;_LL1E: if((_tmp8A.Rel_n).tag != 1)goto _LL20;
_tmp8B=(_tmp8A.Rel_n).f1;_LL1F: _tmp8C=_tmp8B;goto _LL21;_LL20: if((_tmp8A.Abs_n).tag
!= 2)goto _LL1B;_tmp8C=(_tmp8A.Abs_n).f1;_LL21: ns=(struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp8C,({struct Cyc_List_List*_tmp8E=_cycalloc(sizeof(*_tmp8E));
_tmp8E->hd=(*q).f2;_tmp8E->tl=0;_tmp8E;})));goto _LL1B;_LL1B:;}{struct Cyc_List_List*
_tmp8F=Cyc_Lex_get_absolute_namespace(ns);struct Cyc_List_List*_tmp90=((struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;((
struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=({struct Cyc_List_List*
_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->hd=_tmp8F;_tmp91->tl=_tmp90;_tmp91;});((
void(*)(void(*f)(struct _dynforward_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,(((
struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,
_tmp8F))->typedefs);}}void Cyc_Lex_leave_using(){struct Cyc_List_List*_tmp92=((
struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;((struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=((
struct Cyc_List_List*)_check_null(_tmp92))->tl;Cyc_Lex_recompute_typedefs();}void
Cyc_Lex_register_typedef(struct _tuple0*q){struct Cyc_List_List*_tmp93=((struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_Dict_Dict _tmp94=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->decls;struct Cyc_Lex_Ldecls*_tmp95=((struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp94,_tmp93);
struct Cyc_Lex_Ldecls*_tmp96=({struct Cyc_Lex_Ldecls*_tmp97=_cycalloc(sizeof(*
_tmp97));_tmp97->typedefs=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_dynforward_ptr*elt))Cyc_Set_insert)(_tmp95->typedefs,(*q).f2);_tmp97->namespaces=
_tmp95->namespaces;_tmp97;});((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(_tmp94,_tmp93,
_tmp96);Cyc_Lex_insert_typedef((*q).f2);}static short Cyc_Lex_process_id(struct Cyc_Lexing_lexbuf*
lbuf){int symbol_num=Cyc_Lex_str_index_lbuf(lbuf);if(symbol_num <= Cyc_Lex_num_kws)
return(short)*((int*)_check_dynforward_subscript(Cyc_Lex_kw_nums,sizeof(int),
symbol_num - 1));{struct _dynforward_ptr*_tmp98=Cyc_Lex_get_symbol(symbol_num);Cyc_Lex_token_string=*
_tmp98;if(Cyc_Lex_is_typedef(0,_tmp98))return 353;return 347;}}static short Cyc_Lex_process_qual_id(
struct Cyc_Lexing_lexbuf*lbuf){int i=lbuf->lex_start_pos;int end=lbuf->lex_curr_pos;
struct _dynforward_ptr s=lbuf->lex_buffer;struct Cyc_List_List*rev_vs=0;while(i < 
end){int start=i;for(0;i < end  && *((char*)_check_dynforward_subscript(s,sizeof(
char),i))!= ':';i ++){;}if(start == i)(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=({struct Cyc_Core_Impossible_struct
_tmp9A;_tmp9A.tag=Cyc_Core_Impossible;_tmp9A.f1=({const char*_tmp9B="bad namespace";
_tag_dynforward(_tmp9B,sizeof(char),_get_zero_arr_size(_tmp9B,14));});_tmp9A;});
_tmp99;}));{int vlen=i - start;struct _dynforward_ptr*v=Cyc_Lex_get_symbol(Cyc_Lex_str_index((
struct _dynforward_ptr)s,start,vlen));rev_vs=({struct Cyc_List_List*_tmp9C=
_cycalloc(sizeof(*_tmp9C));_tmp9C->hd=v;_tmp9C->tl=rev_vs;_tmp9C;});i +=2;}}if(
rev_vs == 0)(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp9D=
_cycalloc(sizeof(*_tmp9D));_tmp9D[0]=({struct Cyc_Core_Impossible_struct _tmp9E;
_tmp9E.tag=Cyc_Core_Impossible;_tmp9E.f1=({const char*_tmp9F="bad namespace";
_tag_dynforward(_tmp9F,sizeof(char),_get_zero_arr_size(_tmp9F,14));});_tmp9E;});
_tmp9D;}));{struct _dynforward_ptr*v=(struct _dynforward_ptr*)rev_vs->hd;struct Cyc_List_List*
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_vs->tl);
Cyc_Lex_token_qvar=({struct _tuple0*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->f1=(
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmpA1;(_tmpA1.Rel_n).tag=
1;(_tmpA1.Rel_n).f1=vs;_tmpA1;});_tmpA0->f2=v;_tmpA0;});if(Cyc_Lex_is_typedef(vs,
v))return 355;return 354;}}int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*);int Cyc_Lex_strng(
struct Cyc_Lexing_lexbuf*);int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*);int Cyc_yylex(
struct Cyc_Lexing_lexbuf*lbuf){int ans=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lex_token)(
lbuf);Cyc_yylloc.first_line=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_start)(
lbuf);Cyc_yylloc.last_line=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_end)(
lbuf);switch(ans){case 347: _LL22: Cyc_yylval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmpA2;(_tmpA2.String_tok).tag=2;(_tmpA2.String_tok).f1=Cyc_Lex_token_string;
_tmpA2;});break;case 353: _LL23: Cyc_yylval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmpA3;(_tmpA3.String_tok).tag=2;(_tmpA3.String_tok).f1=Cyc_Lex_token_string;
_tmpA3;});break;case 354: _LL24: Cyc_yylval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmpA4;(_tmpA4.QualId_tok).tag=4;(_tmpA4.QualId_tok).f1=Cyc_Lex_token_qvar;
_tmpA4;});break;case 355: _LL25: Cyc_yylval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmpA5;(_tmpA5.QualId_tok).tag=4;(_tmpA5.QualId_tok).f1=Cyc_Lex_token_qvar;
_tmpA5;});break;case 352: _LL26: Cyc_yylval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmpA6;(_tmpA6.String_tok).tag=2;(_tmpA6.String_tok).f1=Cyc_Lex_token_string;
_tmpA6;});break;case 356: _LL27: Cyc_yylval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmpA7;(_tmpA7.Int_tok).tag=0;(_tmpA7.Int_tok).f1=Cyc_Lex_token_int;_tmpA7;});
break;case 348: _LL28: Cyc_yylval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmpA8;(_tmpA8.Int_tok).tag=0;(_tmpA8.Int_tok).f1=Cyc_Lex_token_int;_tmpA8;});
break;case 350: _LL29: Cyc_yylval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmpA9;(_tmpA9.Char_tok).tag=1;(_tmpA9.Char_tok).f1=Cyc_Lex_token_char;_tmpA9;});
break;case 351: _LL2A: Cyc_yylval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmpAA;(_tmpAA.String_tok).tag=2;(_tmpAA.String_tok).f1=Cyc_Lex_token_string;
_tmpAA;});break;case 349: _LL2B: Cyc_yylval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmpAB;(_tmpAB.String_tok).tag=2;(_tmpAB.String_tok).f1=Cyc_Lex_token_string;
_tmpAB;});break;default: _LL2C: break;}return ans;}const int Cyc_Lex_lex_base[171]=(
const int[171]){0,113,17,83,16,2,- 3,- 1,- 2,- 17,- 18,4,118,119,- 19,5,- 13,- 12,85,- 14,
- 11,- 4,- 5,- 6,- 7,- 8,- 9,- 10,192,215,111,- 15,166,- 18,- 59,8,30,- 43,18,31,116,139,32,
138,135,137,159,250,325,83,82,84,94,395,85,470,545,90,- 58,- 26,- 32,620,630,700,
110,126,775,785,119,142,855,893,134,146,138,200,192,202,- 5,963,- 27,1038,90,1096,
1171,1246,99,123,- 30,142,- 17,- 35,- 29,- 38,825,1323,363,194,210,292,1333,1363,668,
335,1396,1427,1465,203,219,1535,1573,212,245,237,247,239,249,- 10,- 42,199,- 24,- 41,
6,161,1505,- 37,- 20,- 22,- 36,- 21,- 23,2,1613,169,171,311,174,176,185,186,187,189,
191,193,199,1686,1770,- 56,- 50,- 49,- 48,- 47,- 46,- 45,- 44,- 51,- 54,- 55,386,213,- 52,-
53,- 57,- 31,- 28,- 25,383,- 39,12,- 16,680};const int Cyc_Lex_lex_backtrk[171]=(const
int[171]){- 1,- 1,- 1,5,3,4,- 1,- 1,- 1,- 1,- 1,16,1,19,- 1,2,- 1,- 1,14,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,15,14,- 1,- 1,- 1,- 1,39,58,- 1,58,58,58,58,58,58,58,58,58,9,11,58,58,58,
58,0,58,58,58,58,- 1,- 1,- 1,4,6,7,6,6,4,5,4,4,- 1,3,3,3,5,5,4,4,- 1,1,- 1,0,- 1,- 1,2,2,
- 1,33,- 1,32,- 1,- 1,- 1,- 1,13,11,- 1,11,11,- 1,12,13,- 1,- 1,13,9,10,9,9,- 1,8,8,8,10,10,
9,9,- 1,- 1,- 1,- 1,- 1,40,- 1,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,
- 1,39};const int Cyc_Lex_lex_default[171]=(const int[171]){34,9,3,3,- 1,- 1,0,0,0,0,0,
- 1,- 1,- 1,0,- 1,0,0,- 1,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,0,- 1,0,0,- 1,- 1,0,166,- 1,- 1,131,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,119,0,0,- 1,- 1,- 1,0,0,0,
0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,0,- 1,-
1,0,0,0,0,0,0,166,0,- 1,0,- 1};const int Cyc_Lex_lex_trans[2027]=(const int[2027]){0,
0,0,0,0,0,0,0,0,35,35,35,35,35,33,6,121,170,170,170,170,170,167,0,0,0,0,0,167,0,
0,168,35,36,37,38,0,39,40,41,170,162,42,43,7,44,45,46,47,48,48,48,48,48,48,48,48,
48,49,4,50,51,52,8,5,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,
53,53,53,53,53,53,169,165,130,54,55,56,53,53,53,53,53,53,53,53,53,53,53,53,53,53,
53,53,53,53,53,53,53,53,53,53,53,53,10,57,- 1,11,32,32,6,- 1,32,15,30,30,30,30,30,
30,30,30,93,89,90,91,31,80,12,83,- 1,32,59,7,16,163,33,87,83,17,31,31,31,31,31,31,
31,31,18,18,18,18,18,18,18,18,32,32,164,- 1,32,126,14,19,123,88,124,124,124,124,
124,124,124,124,124,124,24,127,128,32,129,7,118,24,92,22,13,119,125,161,121,160,
20,122,156,60,155,21,22,22,21,120,23,21,23,154,153,152,24,151,24,150,132,149,25,
24,26,22,27,148,28,29,29,29,29,29,29,29,29,29,29,22,21,157,0,21,23,58,29,29,29,
29,29,29,29,29,29,29,29,29,29,29,29,29,6,- 1,0,23,78,78,17,29,29,29,29,29,29,17,0,
27,29,29,29,29,29,29,27,94,26,105,105,105,105,105,105,105,105,106,106,23,78,78,
17,29,29,29,29,29,29,17,96,27,26,20,20,117,117,107,27,0,26,0,0,0,0,0,108,0,0,109,
- 1,100,100,100,100,100,100,100,100,100,100,157,96,0,26,20,20,117,117,107,158,158,
158,158,158,158,158,158,108,0,14,109,94,0,95,95,95,95,95,95,95,95,95,95,104,104,
104,104,104,104,104,104,104,104,167,96,- 1,168,0,0,0,0,97,0,0,0,0,99,0,99,0,98,
100,100,100,100,100,100,100,100,100,100,0,0,0,0,157,96,0,0,0,0,0,0,97,159,159,
159,159,159,159,159,159,98,81,81,81,81,81,81,81,81,81,81,82,0,- 1,0,0,0,0,81,81,
81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,0,0,0,0,
81,0,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,
81,79,79,79,79,79,79,79,79,79,79,0,0,0,0,0,0,0,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,0,0,0,0,79,0,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,61,62,62,62,62,62,62,62,
62,62,0,0,0,0,0,0,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,0,0,0,- 1,63,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,66,66,66,66,66,66,66,66,67,67,62,62,62,62,62,62,62,
62,62,62,0,170,170,170,170,170,0,0,68,0,0,0,0,0,0,0,0,69,64,0,70,0,0,103,170,103,
0,65,104,104,104,104,104,104,104,104,104,104,0,0,68,0,0,0,0,0,0,0,0,69,64,0,70,0,
0,0,0,0,0,65,63,63,63,63,63,63,63,63,63,63,0,0,0,0,0,0,0,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,0,63,0,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,66,66,66,66,66,
66,66,66,67,67,67,67,67,67,67,67,67,67,67,67,0,0,0,0,0,0,0,0,76,0,0,0,0,0,0,0,0,
77,74,0,0,0,0,0,0,0,0,75,0,0,101,101,101,101,101,101,101,101,101,101,76,0,0,0,0,
0,0,0,0,77,74,102,19,0,0,0,0,0,19,75,71,71,71,71,71,71,71,71,71,71,0,0,0,0,0,0,0,
71,71,71,71,71,71,102,19,0,0,0,0,0,19,0,0,0,0,0,0,0,71,71,71,71,71,71,71,71,71,
71,0,71,71,71,71,71,71,71,71,71,71,71,71,0,0,0,0,0,72,0,0,0,0,0,0,0,0,73,0,0,0,0,
0,0,0,0,0,0,0,71,71,71,71,71,71,0,0,0,0,0,72,0,0,0,0,0,0,0,0,73,79,79,79,79,79,
79,79,79,79,79,0,0,0,0,0,0,0,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,0,0,0,0,79,0,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,81,81,81,81,81,81,81,81,81,81,82,0,0,0,0,
0,0,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,
81,0,0,0,0,81,0,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,
81,81,81,81,81,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
84,84,84,84,0,0,0,0,85,0,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,86,0,0,0,0,0,0,84,84,84,84,
84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,0,0,0,0,84,0,
84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,85,
85,85,85,85,85,85,85,85,85,0,0,0,0,0,0,0,85,85,85,85,85,85,85,85,85,85,85,85,85,
85,85,85,85,85,85,85,85,85,85,85,85,85,0,0,0,0,85,0,85,85,85,85,85,85,85,85,85,
85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,94,0,95,95,95,95,95,95,95,95,
95,95,100,100,100,100,100,100,100,100,100,100,0,96,0,0,0,0,0,0,97,0,0,0,16,0,0,0,
0,98,16,0,101,101,101,101,101,101,101,101,101,101,0,0,0,96,0,0,0,0,0,0,97,102,19,
0,16,0,0,0,19,98,16,0,0,104,104,104,104,104,104,104,104,104,104,0,0,0,0,0,0,0,0,
0,0,102,19,19,0,0,0,0,19,19,94,0,105,105,105,105,105,105,105,105,106,106,0,0,0,0,
0,0,0,0,0,0,0,96,0,19,0,0,0,0,115,19,0,0,0,0,0,0,94,116,106,106,106,106,106,106,
106,106,106,106,0,0,0,0,0,96,0,0,0,0,0,96,115,0,0,0,0,0,113,0,0,116,0,0,0,0,0,
114,0,0,124,124,124,124,124,124,124,124,124,124,0,0,0,96,0,0,0,0,0,0,113,102,19,
0,0,0,0,0,19,114,110,110,110,110,110,110,110,110,110,110,0,0,0,0,0,0,0,110,110,
110,110,110,110,102,19,0,0,0,0,0,19,0,0,0,0,0,0,0,110,110,110,110,110,110,110,
110,110,110,0,110,110,110,110,110,110,110,110,110,110,110,110,0,0,0,133,0,111,0,
0,134,0,0,0,0,0,112,0,0,135,135,135,135,135,135,135,135,0,110,110,110,110,110,
110,136,0,0,0,0,111,0,0,0,0,0,0,0,0,112,0,0,0,0,0,0,0,0,0,0,0,0,0,0,137,0,0,0,0,
138,139,0,0,0,140,0,0,0,0,0,0,0,141,0,0,0,142,0,143,0,144,0,145,146,146,146,146,
146,146,146,146,146,146,0,0,0,0,0,0,0,146,146,146,146,146,146,146,146,146,146,
146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,0,0,0,0,0,0,146,
146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,
146,146,146,146,146,147,0,0,0,0,0,0,0,0,146,146,146,146,146,146,146,146,146,146,
0,0,0,0,0,0,0,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,
146,146,146,146,146,146,146,146,146,146,0,0,0,0,0,0,146,146,146,146,146,146,146,
146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0};const int Cyc_Lex_lex_check[2027]=(const int[2027]){- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,11,15,122,35,35,35,35,35,168,- 1,- 1,- 1,- 1,- 1,38,- 1,
- 1,38,0,0,0,0,- 1,0,0,0,35,131,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,4,2,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,39,42,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,3,1,12,12,13,3,12,13,18,18,18,18,18,18,18,
18,49,50,50,50,51,54,1,82,41,12,57,12,13,40,52,52,86,13,30,30,30,30,30,30,30,30,
13,13,13,13,13,13,13,13,32,32,40,41,32,44,43,13,45,87,45,45,45,45,45,45,45,45,45,
45,64,44,44,32,43,32,46,65,89,68,1,46,123,133,119,134,13,119,136,57,137,13,13,69,
72,46,13,73,74,138,139,140,64,141,13,142,41,143,13,65,13,68,13,144,13,28,28,28,
28,28,28,28,28,28,28,69,72,159,- 1,73,74,0,28,28,28,28,28,28,29,29,29,29,29,29,29,
29,29,29,2,38,- 1,75,76,77,97,29,29,29,29,29,29,98,- 1,107,28,28,28,28,28,28,108,
47,111,47,47,47,47,47,47,47,47,47,47,75,76,77,97,29,29,29,29,29,29,98,47,107,112,
113,114,115,116,47,108,- 1,111,- 1,- 1,- 1,- 1,- 1,47,- 1,- 1,47,3,99,99,99,99,99,99,99,
99,99,99,135,47,- 1,112,113,114,115,116,47,135,135,135,135,135,135,135,135,47,- 1,
1,47,48,- 1,48,48,48,48,48,48,48,48,48,48,103,103,103,103,103,103,103,103,103,103,
166,48,41,166,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,96,- 1,96,- 1,48,96,96,96,96,96,96,96,96,
96,96,- 1,- 1,- 1,- 1,158,48,- 1,- 1,- 1,- 1,- 1,- 1,48,158,158,158,158,158,158,158,158,48,
53,53,53,53,53,53,53,53,53,53,53,- 1,119,- 1,- 1,- 1,- 1,53,53,53,53,53,53,53,53,53,
53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,- 1,- 1,- 1,- 1,53,- 1,53,53,53,53,
53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,55,55,55,55,55,
55,55,55,55,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,
55,55,55,55,55,55,55,55,55,55,55,- 1,- 1,- 1,- 1,55,- 1,55,55,55,55,55,55,55,55,55,55,
55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,56,56,56,56,56,56,56,56,56,56,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,
56,56,56,56,56,- 1,- 1,- 1,166,56,- 1,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,
56,56,56,56,56,56,56,56,56,56,56,61,61,61,61,61,61,61,61,61,61,62,62,62,62,62,62,
62,62,62,62,- 1,170,170,170,170,170,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,61,
- 1,- 1,102,170,102,- 1,62,102,102,102,102,102,102,102,102,102,102,- 1,- 1,61,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,62,63,63,63,63,63,63,63,63,63,63,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,- 1,- 1,- 1,- 1,63,- 1,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,66,66,66,66,66,66,66,66,66,66,67,67,67,67,67,67,
67,67,67,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,67,- 1,- 1,94,94,94,94,94,94,94,94,94,94,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,
67,94,94,- 1,- 1,- 1,- 1,- 1,94,67,70,70,70,70,70,70,70,70,70,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
70,70,70,70,70,70,94,94,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,71,71,71,71,71,
71,71,71,71,- 1,70,70,70,70,70,70,71,71,71,71,71,71,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,71,71,71,71,71,- 1,- 1,- 1,- 1,- 1,
71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,79,79,79,79,79,79,79,79,79,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,- 1,
- 1,- 1,- 1,79,- 1,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,81,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,- 1,- 1,81,81,81,81,81,81,
81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,81,- 1,81,
81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,83,83,
83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,- 1,- 1,- 1,
- 1,83,- 1,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,
83,83,84,84,84,84,84,84,84,84,84,84,84,- 1,- 1,- 1,- 1,- 1,- 1,84,84,84,84,84,84,84,84,
84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,- 1,- 1,- 1,- 1,84,- 1,84,84,84,
84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,85,85,85,85,
85,85,85,85,85,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
85,85,85,85,85,85,85,85,85,85,85,85,- 1,- 1,- 1,- 1,85,- 1,85,85,85,85,85,85,85,85,85,
85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,95,- 1,95,95,95,95,95,95,95,95,
95,95,100,100,100,100,100,100,100,100,100,100,- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,-
1,100,- 1,- 1,- 1,- 1,95,100,- 1,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,95,
- 1,- 1,- 1,- 1,- 1,- 1,95,101,101,- 1,100,- 1,- 1,- 1,101,95,100,- 1,- 1,104,104,104,104,
104,104,104,104,104,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,104,- 1,- 1,- 1,- 1,
101,104,105,- 1,105,105,105,105,105,105,105,105,105,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,105,- 1,104,- 1,- 1,- 1,- 1,105,104,- 1,- 1,- 1,- 1,- 1,- 1,106,105,106,106,106,106,
106,106,106,106,106,106,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,106,105,- 1,- 1,- 1,- 1,- 1,
106,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,124,124,124,124,124,124,124,124,124,124,-
1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,106,124,124,- 1,- 1,- 1,- 1,- 1,124,106,109,109,109,109,
109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,124,124,- 1,
- 1,- 1,- 1,- 1,124,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,110,110,110,110,110,110,110,110,110,- 1,
109,109,109,109,109,109,110,110,110,110,110,110,- 1,- 1,- 1,132,- 1,110,- 1,- 1,132,- 1,
- 1,- 1,- 1,- 1,110,- 1,- 1,132,132,132,132,132,132,132,132,- 1,110,110,110,110,110,110,
132,- 1,- 1,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,132,- 1,- 1,- 1,- 1,132,132,- 1,- 1,- 1,132,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,- 1,- 1,- 1,
132,- 1,132,- 1,132,- 1,132,145,145,145,145,145,145,145,145,145,145,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,
145,145,145,145,145,145,145,145,- 1,- 1,- 1,- 1,- 1,- 1,145,145,145,145,145,145,145,
145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,146,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,146,146,146,146,146,146,146,146,146,146,- 1,- 1,- 1,- 1,- 1,-
1,- 1,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,
146,146,146,146,146,146,146,- 1,- 1,- 1,- 1,- 1,- 1,146,146,146,146,146,146,146,146,
146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};int Cyc_Lex_lex_engine(
int start_state,struct Cyc_Lexing_lexbuf*lbuf){int state;int base;int backtrk;int c;
state=start_state;if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);
lbuf->lex_last_action=- 1;}else{state=(- state)- 1;}while(1){base=Cyc_Lex_lex_base[
_check_known_subscript_notnull(171,state)];if(base < 0)return(- base)- 1;backtrk=
Cyc_Lex_lex_backtrk[_check_known_subscript_notnull(171,state)];if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}if(lbuf->lex_curr_pos
>= lbuf->lex_buffer_len){if(!lbuf->lex_eof_reached)return(- state)- 1;else{c=256;}}
else{c=(int)*((char*)_check_dynforward_subscript(lbuf->lex_buffer,sizeof(char),
lbuf->lex_curr_pos ++));if(c == - 1)c=256;}if(Cyc_Lex_lex_check[
_check_known_subscript_notnull(2027,base + c)]== state)state=Cyc_Lex_lex_trans[
_check_known_subscript_notnull(2027,base + c)];else{state=Cyc_Lex_lex_default[
_check_known_subscript_notnull(171,state)];}if(state < 0){lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC[0]=({struct Cyc_Lexing_Error_struct
_tmpAD;_tmpAD.tag=Cyc_Lexing_Error;_tmpAD.f1=({const char*_tmpAE="empty token";
_tag_dynforward(_tmpAE,sizeof(char),_get_zero_arr_size(_tmpAE,12));});_tmpAD;});
_tmpAC;}));else{return lbuf->lex_last_action;}}else{if(c == 256)lbuf->lex_eof_reached=
0;}}}int Cyc_Lex_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_Lex_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL2E: return(int)Cyc_Lex_process_id(
lexbuf);case 1: _LL2F: return(int)Cyc_Lex_process_id(lexbuf);case 2: _LL30: return(int)
Cyc_Lex_process_qual_id(lexbuf);case 3: _LL31: Cyc_Lex_token_int=Cyc_Lex_intconst(
lexbuf,3,0,16);return 356;case 4: _LL32: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,1,
0,8);return 356;case 5: _LL33: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,1,0,10);
return 356;case 6: _LL34: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,1,0,10);return
356;case 7: _LL35: Cyc_Lex_token_string=*Cyc_Lex_get_symbol(Cyc_Lex_str_index_lbuf(
lexbuf));return 352;case 8: _LL36: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,2,0,16);
return 348;case 9: _LL37: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,8);return 348;
case 10: _LL38: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);return 348;case 11:
_LL39: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);return 348;case 12: _LL3A:
Cyc_Lex_token_string=(struct _dynforward_ptr)Cyc_Lexing_lexeme(lexbuf);return 351;
case 13: _LL3B: Cyc_Lex_token_string=(struct _dynforward_ptr)Cyc_Lexing_lexeme(
lexbuf);return 351;case 14: _LL3C: return 330;case 15: _LL3D: return 331;case 16: _LL3E:
return 328;case 17: _LL3F: return 329;case 18: _LL40: return 324;case 19: _LL41: return 325;
case 20: _LL42: return 337;case 21: _LL43: return 338;case 22: _LL44: return 334;case 23:
_LL45: return 335;case 24: _LL46: return 336;case 25: _LL47: return 343;case 26: _LL48:
return 342;case 27: _LL49: return 341;case 28: _LL4A: return 339;case 29: _LL4B: return 340;
case 30: _LL4C: return 332;case 31: _LL4D: return 333;case 32: _LL4E: return 326;case 33:
_LL4F: return 327;case 34: _LL50: return 345;case 35: _LL51: return 323;case 36: _LL52:
return 344;case 37: _LL53: return 346;case 38: _LL54: return Cyc_Lex_token(lexbuf);case 39:
_LL55: return Cyc_Lex_token(lexbuf);case 40: _LL56: return Cyc_Lex_token(lexbuf);case
41: _LL57: Cyc_Lex_comment_depth=1;Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(
lexbuf);Cyc_Lex_comment(lexbuf);return Cyc_Lex_token(lexbuf);case 42: _LL58: Cyc_Lex_string_pos=
0;Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);while(Cyc_Lex_strng(
lexbuf)){;}Cyc_Lex_token_string=(struct _dynforward_ptr)Cyc_Lex_get_stored_string();
return 349;case 43: _LL59: Cyc_Lex_token_char='\a';return 350;case 44: _LL5A: Cyc_Lex_token_char='\b';
return 350;case 45: _LL5B: Cyc_Lex_token_char='\f';return 350;case 46: _LL5C: Cyc_Lex_token_char='\n';
return 350;case 47: _LL5D: Cyc_Lex_token_char='\r';return 350;case 48: _LL5E: Cyc_Lex_token_char='\t';
return 350;case 49: _LL5F: Cyc_Lex_token_char='\v';return 350;case 50: _LL60: Cyc_Lex_token_char='\\';
return 350;case 51: _LL61: Cyc_Lex_token_char='\'';return 350;case 52: _LL62: Cyc_Lex_token_char='"';
return 350;case 53: _LL63: Cyc_Lex_token_char='?';return 350;case 54: _LL64: Cyc_Lex_token_char=(
char)(*Cyc_Lex_intconst(lexbuf,2,1,8)).f2;return 350;case 55: _LL65: Cyc_Lex_token_char=(
char)(*Cyc_Lex_intconst(lexbuf,3,1,16)).f2;return 350;case 56: _LL66: Cyc_Lex_token_char=
Cyc_Lexing_lexeme_char(lexbuf,1);return 350;case 57: _LL67: return - 1;case 58: _LL68:
return(int)Cyc_Lexing_lexeme_char(lexbuf,0);default: _LL69:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_token_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({struct Cyc_Lexing_Error_struct
_tmpB0;_tmpB0.tag=Cyc_Lexing_Error;_tmpB0.f1=({const char*_tmpB1="some action didn't return!";
_tag_dynforward(_tmpB1,sizeof(char),_get_zero_arr_size(_tmpB1,27));});_tmpB0;});
_tmpAF;}));}int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_token_rec(
lexbuf,0);}int Cyc_Lex_strng_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL6B: return
1;case 1: _LL6C: return 0;case 2: _LL6D: return 1;case 3: _LL6E: Cyc_Lex_store_string_char('\a');
return 1;case 4: _LL6F: Cyc_Lex_store_string_char('\b');return 1;case 5: _LL70: Cyc_Lex_store_string_char('\f');
return 1;case 6: _LL71: Cyc_Lex_store_string_char('\n');return 1;case 7: _LL72: Cyc_Lex_store_string_char('\r');
return 1;case 8: _LL73: Cyc_Lex_store_string_char('\t');return 1;case 9: _LL74: Cyc_Lex_store_string_char('\v');
return 1;case 10: _LL75: Cyc_Lex_store_string_char('\\');return 1;case 11: _LL76: Cyc_Lex_store_string_char('\'');
return 1;case 12: _LL77: Cyc_Lex_store_string_char('"');return 1;case 13: _LL78: Cyc_Lex_store_string_char('?');
return 1;case 14: _LL79: Cyc_Lex_store_string_char((char)(*Cyc_Lex_intconst(lexbuf,1,
0,8)).f2);return 1;case 15: _LL7A: Cyc_Lex_store_string_char((char)(*Cyc_Lex_intconst(
lexbuf,2,0,16)).f2);return 1;case 16: _LL7B: Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(
lexbuf,0));return 1;case 17: _LL7C: Cyc_Lex_runaway_err(({const char*_tmpB2="string ends in newline";
_tag_dynforward(_tmpB2,sizeof(char),_get_zero_arr_size(_tmpB2,23));}),lexbuf);
return 0;case 18: _LL7D: Cyc_Lex_runaway_err(({const char*_tmpB3="unterminated string";
_tag_dynforward(_tmpB3,sizeof(char),_get_zero_arr_size(_tmpB3,20));}),lexbuf);
return 0;case 19: _LL7E: Cyc_Lex_err(({const char*_tmpB4="bad character following backslash in string";
_tag_dynforward(_tmpB4,sizeof(char),_get_zero_arr_size(_tmpB4,44));}),lexbuf);
return 1;default: _LL7F:(lexbuf->refill_buff)(lexbuf);return Cyc_Lex_strng_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpB5=
_cycalloc(sizeof(*_tmpB5));_tmpB5[0]=({struct Cyc_Lexing_Error_struct _tmpB6;
_tmpB6.tag=Cyc_Lexing_Error;_tmpB6.f1=({const char*_tmpB7="some action didn't return!";
_tag_dynforward(_tmpB7,sizeof(char),_get_zero_arr_size(_tmpB7,27));});_tmpB6;});
_tmpB5;}));}int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_rec(
lexbuf,1);}int Cyc_Lex_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL81: ++ Cyc_Lex_comment_depth;
return Cyc_Lex_comment(lexbuf);case 1: _LL82: -- Cyc_Lex_comment_depth;if(Cyc_Lex_comment_depth
> 0)return Cyc_Lex_comment(lexbuf);return 0;case 2: _LL83: Cyc_Lex_runaway_err(({
const char*_tmpB8="unterminated comment";_tag_dynforward(_tmpB8,sizeof(char),
_get_zero_arr_size(_tmpB8,21));}),lexbuf);return 0;case 3: _LL84: return Cyc_Lex_comment(
lexbuf);case 4: _LL85: return Cyc_Lex_comment(lexbuf);case 5: _LL86: return Cyc_Lex_comment(
lexbuf);default: _LL87:(lexbuf->refill_buff)(lexbuf);return Cyc_Lex_comment_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpB9=
_cycalloc(sizeof(*_tmpB9));_tmpB9[0]=({struct Cyc_Lexing_Error_struct _tmpBA;
_tmpBA.tag=Cyc_Lexing_Error;_tmpBA.f1=({const char*_tmpBB="some action didn't return!";
_tag_dynforward(_tmpBB,sizeof(char),_get_zero_arr_size(_tmpBB,27));});_tmpBA;});
_tmpB9;}));}int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_comment_rec(
lexbuf,2);}void Cyc_Lex_lex_init(int include_cyclone_keywords){Cyc_Lex_ids_trie=({
struct Cyc_Lex_Trie*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC->children=(union Cyc_Lex_TrieChildren_union)({
union Cyc_Lex_TrieChildren_union _tmpBD;(_tmpBD.Zero).tag=0;_tmpBD;});_tmpBC->shared_str=
0;_tmpBC;});Cyc_Lex_typedefs_trie=({struct Cyc_Lex_Trie*_tmpBE=_cycalloc(sizeof(*
_tmpBE));_tmpBE->children=(union Cyc_Lex_TrieChildren_union)({union Cyc_Lex_TrieChildren_union
_tmpBF;(_tmpBF.Zero).tag=0;_tmpBF;});_tmpBE->shared_str=0;_tmpBE;});Cyc_Lex_symbols=(
struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(int,struct _dynforward_ptr*))
Cyc_Xarray_create)(101,({struct _dynforward_ptr*_tmpC0=_cycalloc(sizeof(*_tmpC0));
_tmpC0[0]=(struct _dynforward_ptr)({const char*_tmpC1="";_tag_dynforward(_tmpC1,
sizeof(char),_get_zero_arr_size(_tmpC1,1));});_tmpC0;}));((void(*)(struct Cyc_Xarray_Xarray*,
struct _dynforward_ptr*))Cyc_Xarray_add)((struct Cyc_Xarray_Xarray*)_check_null(
Cyc_Lex_symbols),& Cyc_Lex_bogus_string);Cyc_Lex_num_kws=Cyc_Lex_num_keywords(
include_cyclone_keywords);Cyc_Lex_kw_nums=({unsigned int _tmpC2=(unsigned int)Cyc_Lex_num_kws;
int*_tmpC3=(int*)_cycalloc_atomic(_check_times(sizeof(int),_tmpC2));struct
_dynforward_ptr _tmpC5=_tag_dynforward(_tmpC3,sizeof(int),_tmpC2);{unsigned int
_tmpC4=_tmpC2;unsigned int i;for(i=0;i < _tmpC4;i ++){_tmpC3[i]=0;}}_tmpC5;});{
unsigned int i=0;unsigned int rwsze=67;{unsigned int j=0;for(0;j < rwsze;j ++){if(
include_cyclone_keywords  || (Cyc_Lex_rw_array[(int)j]).f3){struct _dynforward_ptr
_tmpC6=(Cyc_Lex_rw_array[(int)j]).f1;Cyc_Lex_str_index(_tmpC6,0,(int)Cyc_strlen((
struct _dynforward_ptr)_tmpC6));*((int*)_check_dynforward_subscript(Cyc_Lex_kw_nums,
sizeof(int),(int)i))=(int)(Cyc_Lex_rw_array[(int)j]).f2;i ++;}}}Cyc_Lex_typedef_init();
Cyc_Lex_comment_depth=0;}}
