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
Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dynforward_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,
void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_merge(int(*cmp)(void*,void*),
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_memq(struct Cyc_List_List*l,void*x);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];
typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
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
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict d,void*k);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*
r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict,
void*);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict
d);int Cyc_strptrcmp(struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct Cyc_Lineno_Pos{
struct _dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dynforward_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];int Cyc_Position_error_p();
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple1{union Cyc_Absyn_Nmspace_union
f1;struct _dynforward_ptr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_No_constr_struct{int tag;};
union Cyc_Absyn_Constraint_union{struct Cyc_Absyn_Eq_constr_struct Eq_constr;struct
Cyc_Absyn_Forward_constr_struct Forward_constr;struct Cyc_Absyn_No_constr_struct
No_constr;};struct Cyc_Absyn_Conref{union Cyc_Absyn_Constraint_union v;};struct Cyc_Absyn_Eq_kb_struct{
int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;
};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;};struct
Cyc_Absyn_Tvar{struct _dynforward_ptr*name;int*identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AbsUpper_b_struct{int tag;void*f1;
};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
info);int Cyc_Absyn_is_union_type(void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
_dynforward_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_List_List*,void*);
struct Cyc_Dict_Dict Cyc_CfFlowInfo_empty_flowdict();struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_Dict_Dict Cyc_CfFlowInfo_mt_place_set();int Cyc_CfFlowInfo_update_place_set(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,
struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict
consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(
struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};extern void*Cyc_CfFlowInfo_unknown_none;extern void*Cyc_CfFlowInfo_unknown_all;
extern void*Cyc_CfFlowInfo_esc_none;extern void*Cyc_CfFlowInfo_esc_all;int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);void*Cyc_CfFlowInfo_typ_to_absrval(
void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo_union
f1,union Cyc_CfFlowInfo_FlowInfo_union f2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct
Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_Dict_Dict
d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_join_flow(struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo_union,
union Cyc_CfFlowInfo_FlowInfo_union,int);struct _tuple5{union Cyc_CfFlowInfo_FlowInfo_union
f1;void*f2;};struct _tuple5 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_Dict_Dict*
all_changed,struct _tuple5 pr1,struct _tuple5 pr2,int);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_after_flow(struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo_union,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_kill_flow_region(union Cyc_CfFlowInfo_FlowInfo_union
f,void*rgn);struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;
};int Cyc_CfFlowInfo_contains_region(void*rgn,void*t);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_readthrough_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);struct _tuple6{struct Cyc_CfFlowInfo_ConsumeInfo f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};struct _tuple6 Cyc_CfFlowInfo_save_consume_info(union Cyc_CfFlowInfo_FlowInfo_union
f,int clear);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_restore_consume_info(
union Cyc_CfFlowInfo_FlowInfo_union f,struct Cyc_CfFlowInfo_ConsumeInfo c);struct
_dynforward_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place);
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dynforward_ptr
Cyc_Absynpp_typ2string(void*);struct _dynforward_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct Cyc_NewControlFlow_CFStmtAnnot{struct Cyc_Absyn_Stmt*encloser;int visited;
union Cyc_CfFlowInfo_FlowInfo_union flow;};static char Cyc_NewControlFlow_CFAnnot[12]="\000\000\000\000CFAnnot\000";
struct Cyc_NewControlFlow_CFAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot
f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser){(void*)(enclosee->annot=(void*)((void*)({struct Cyc_NewControlFlow_CFAnnot_struct*
_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0[0]=({struct Cyc_NewControlFlow_CFAnnot_struct
_tmp1;_tmp1.tag=Cyc_NewControlFlow_CFAnnot;_tmp1.f1=({struct Cyc_NewControlFlow_CFStmtAnnot
_tmp2;_tmp2.encloser=encloser;_tmp2.visited=0;_tmp2.flow=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3;(_tmp3.BottomFL).tag=0;_tmp3;});_tmp2;});
_tmp1;});_tmp0;})));}static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt*s){void*_tmp4=(void*)s->annot;struct Cyc_NewControlFlow_CFStmtAnnot
_tmp5;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp6;_LL1: if(*((void**)_tmp4)!= Cyc_NewControlFlow_CFAnnot)
goto _LL3;_tmp5=((struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp4)->f1;_tmp6=(
struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_struct*)
_tmp4)->f1;_LL2: return _tmp6;_LL3:;_LL4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7[0]=({struct Cyc_Core_Impossible_struct _tmp8;
_tmp8.tag=Cyc_Core_Impossible;_tmp8.f1=({const char*_tmp9="ControlFlow -- wrong stmt annotation";
_tag_dynforward(_tmp9,sizeof(char),_get_zero_arr_size(_tmp9,37));});_tmp8;});
_tmp7;}));_LL0:;}struct Cyc_NewControlFlow_AnalEnv{int iterate_again;int
iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo_union tryflow;struct Cyc_Dict_Dict*
all_changed;int noreturn;struct Cyc_List_List*param_roots;};static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,
struct Cyc_Absyn_Stmt*);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Decl*);
static struct _tuple5 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);
struct _tuple7{union Cyc_CfFlowInfo_FlowInfo_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};static struct _tuple7 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpA=Cyc_NewControlFlow_get_stmt_annot(
s);_tmpA->flow=Cyc_CfFlowInfo_join_flow(env->all_changed,inflow,_tmpA->flow,1);
_tmpA->visited=env->iteration_num;return _tmpA;}static int Cyc_NewControlFlow_get_field_index_fs(
struct Cyc_List_List*fs,struct _dynforward_ptr*f){int n=0;for(0;(unsigned int)fs;fs=
fs->tl){struct _dynforward_ptr*_tmpB=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(_tmpB,f)== 0)return n;n=n + 1;}(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC[0]=({struct Cyc_Core_Impossible_struct _tmpD;
_tmpD.tag=Cyc_Core_Impossible;_tmpD.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp10;_tmp10.tag=0;_tmp10.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{
void*_tmpE[1]={& _tmp10};Cyc_aprintf(({const char*_tmpF="get_field_index_fs failed: %s";
_tag_dynforward(_tmpF,sizeof(char),_get_zero_arr_size(_tmpF,30));}),
_tag_dynforward(_tmpE,sizeof(void*),1));}});_tmpD;});_tmpC;}));}static int Cyc_NewControlFlow_get_field_index(
void*t,struct _dynforward_ptr*f){void*_tmp11=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp12;union Cyc_Absyn_AggrInfoU_union _tmp13;struct Cyc_List_List*_tmp14;_LL6: if(
_tmp11 <= (void*)4)goto _LLA;if(*((int*)_tmp11)!= 10)goto _LL8;_tmp12=((struct Cyc_Absyn_AggrType_struct*)
_tmp11)->f1;_tmp13=_tmp12.aggr_info;_LL7: {struct Cyc_List_List*_tmp15=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp13))->impl))->fields;
_tmp14=_tmp15;goto _LL9;}_LL8: if(*((int*)_tmp11)!= 11)goto _LLA;_tmp14=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp11)->f2;_LL9: return Cyc_NewControlFlow_get_field_index_fs(_tmp14,f);_LLA:;
_LLB:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp16=_cycalloc(
sizeof(*_tmp16));_tmp16[0]=({struct Cyc_Core_Impossible_struct _tmp17;_tmp17.tag=
Cyc_Core_Impossible;_tmp17.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp1A;_tmp1A.tag=0;_tmp1A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp18[1]={& _tmp1A};Cyc_aprintf(({const char*_tmp19="get_field_index failed: %s";
_tag_dynforward(_tmp19,sizeof(char),_get_zero_arr_size(_tmp19,27));}),
_tag_dynforward(_tmp18,sizeof(void*),1));}});_tmp17;});_tmp16;}));_LL5:;}static
void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union new_flow){if(env->in_try)env->tryflow=Cyc_CfFlowInfo_join_flow(
env->all_changed,new_flow,env->tryflow,1);}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo_union
flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1B=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo_union _tmp1C=Cyc_CfFlowInfo_join_flow(env->all_changed,
flow,_tmp1B->flow,1);if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp1C,_tmp1B->flow)){
_tmp1B->flow=_tmp1C;if(_tmp1B->visited == env->iteration_num)env->iterate_again=1;}}
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_add_vars(union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*vds,void*leafval,struct Cyc_Position_Segment*loc){
union Cyc_CfFlowInfo_FlowInfo_union _tmp1D=inflow;struct Cyc_Dict_Dict _tmp1E;struct
Cyc_List_List*_tmp1F;struct Cyc_CfFlowInfo_ConsumeInfo _tmp20;_LLD: if((_tmp1D.BottomFL).tag
!= 0)goto _LLF;_LLE: return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp21;(_tmp21.BottomFL).tag=0;_tmp21;});_LLF: if((_tmp1D.ReachableFL).tag != 1)
goto _LLC;_tmp1E=(_tmp1D.ReachableFL).f1;_tmp1F=(_tmp1D.ReachableFL).f2;_tmp20=(
_tmp1D.ReachableFL).f3;_LL10: for(0;vds != 0;vds=vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp22=({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp23=_cycalloc(sizeof(*_tmp23));
_tmp23[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp24;_tmp24.tag=0;_tmp24.f1=(
struct Cyc_Absyn_Vardecl*)vds->hd;_tmp24;});_tmp23;});_tmp1E=Cyc_Dict_insert(
_tmp1E,(void*)_tmp22,Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Absyn_Vardecl*)
vds->hd)->type,leafval));}return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp25;(_tmp25.ReachableFL).tag=1;(_tmp25.ReachableFL).f1=_tmp1E;(_tmp25.ReachableFL).f2=
_tmp1F;(_tmp25.ReachableFL).f3=_tmp20;_tmp25;});_LLC:;}struct _tuple8{struct Cyc_CfFlowInfo_Place*
f1;struct Cyc_Position_Segment*f2;};static void Cyc_NewControlFlow_remove_vars(
union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Dict_Dict*out_unique_places,
struct Cyc_Dict_Dict old_unique_places){struct Cyc_CfFlowInfo_ConsumeInfo _tmp27;
struct Cyc_Dict_Dict _tmp28;struct _tuple6 _tmp26=Cyc_CfFlowInfo_save_consume_info(
outflow,0);_tmp27=_tmp26.f1;_tmp28=_tmp27.consumed;{struct _RegionHandle _tmp29=
_new_region("r");struct _RegionHandle*r=& _tmp29;_push_region(r);{struct Cyc_Iter_Iter
_tmp2A=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))
Cyc_Dict_make_iter)(r,*((struct Cyc_Dict_Dict*)_check_null(out_unique_places)));
struct _tuple8 _tmp2B=*((struct _tuple8*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict
d))Cyc_Dict_rchoose)(r,*out_unique_places);while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple8*))Cyc_Iter_next)(_tmp2A,& _tmp2B)){struct Cyc_CfFlowInfo_Place*
_tmp2C=_tmp2B.f1;if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))
Cyc_Dict_member)(_tmp28,_tmp2C))({struct Cyc_String_pa_struct _tmp2F;_tmp2F.tag=0;
_tmp2F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_CfFlowInfo_place_err_string(
_tmp2C));{void*_tmp2D[1]={& _tmp2F};Cyc_Tcutil_terr(_tmp2B.f2,({const char*_tmp2E="Failed to consume unique variable %s";
_tag_dynforward(_tmp2E,sizeof(char),_get_zero_arr_size(_tmp2E,37));}),
_tag_dynforward(_tmp2D,sizeof(void*),1));}});}};_pop_region(r);}*
out_unique_places=old_unique_places;}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_use_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
inflow,void*r){union Cyc_CfFlowInfo_FlowInfo_union _tmp30=inflow;struct Cyc_Dict_Dict
_tmp31;struct Cyc_List_List*_tmp32;struct Cyc_CfFlowInfo_ConsumeInfo _tmp33;_LL12:
if((_tmp30.BottomFL).tag != 0)goto _LL14;_LL13: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp34;(_tmp34.BottomFL).tag=0;_tmp34;});_LL14:
if((_tmp30.ReachableFL).tag != 1)goto _LL11;_tmp31=(_tmp30.ReachableFL).f1;_tmp32=(
_tmp30.ReachableFL).f2;_tmp33=(_tmp30.ReachableFL).f3;_LL15: if(Cyc_CfFlowInfo_initlevel(
_tmp31,r)!= (void*)2)({void*_tmp35[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp36="expression may not be fully initialized";
_tag_dynforward(_tmp36,sizeof(char),_get_zero_arr_size(_tmp36,40));}),
_tag_dynforward(_tmp35,sizeof(void*),0));});{struct Cyc_Dict_Dict _tmp37=Cyc_CfFlowInfo_escape_deref(
_tmp31,env->all_changed,r);if(_tmp31.t == _tmp37.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp38=({union Cyc_CfFlowInfo_FlowInfo_union _tmp39;(_tmp39.ReachableFL).tag=1;(
_tmp39.ReachableFL).f1=_tmp37;(_tmp39.ReachableFL).f2=_tmp32;(_tmp39.ReachableFL).f3=
_tmp33;_tmp39;});Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp38);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp38;}}_LL11:;}struct _tuple9{
union Cyc_CfFlowInfo_FlowInfo_union f1;struct Cyc_List_List*f2;};static struct
_tuple9 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_List_List*es,int
arg1_unconsumed){if(es == 0)return({struct _tuple9 _tmp3A;_tmp3A.f1=inflow;_tmp3A.f2=
0;_tmp3A;});if(es->tl == 0){union Cyc_CfFlowInfo_FlowInfo_union _tmp3C;void*_tmp3D;
struct _tuple5 _tmp3B=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_tmp3C=_tmp3B.f1;_tmp3D=_tmp3B.f2;return({struct _tuple9 _tmp3E;_tmp3E.f1=
_tmp3C;_tmp3E.f2=({struct Cyc_List_List*_tmp3F=_region_malloc(rgn,sizeof(*_tmp3F));
_tmp3F->hd=(void*)_tmp3D;_tmp3F->tl=0;_tmp3F;});_tmp3E;});}{struct Cyc_Dict_Dict*
outer_all_changed=env->all_changed;struct Cyc_Dict_Dict this_all_changed;union Cyc_CfFlowInfo_FlowInfo_union
old_inflow;union Cyc_CfFlowInfo_FlowInfo_union outflow;struct Cyc_List_List*rvals;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp41;union Cyc_CfFlowInfo_FlowInfo_union _tmp42;
struct _tuple6 _tmp40=Cyc_CfFlowInfo_save_consume_info(inflow,1);_tmp41=_tmp40.f1;
_tmp42=_tmp40.f2;{struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp57;_tmp57.consumed=Cyc_CfFlowInfo_mt_place_set();_tmp57.may_consume=0;_tmp57;});
int init_consume=0;do{this_all_changed=Cyc_CfFlowInfo_mt_place_set();_tmp42=Cyc_CfFlowInfo_restore_consume_info(
_tmp42,_tmp41);env->all_changed=({struct Cyc_Dict_Dict*_tmp43=_cycalloc(sizeof(*
_tmp43));_tmp43[0]=Cyc_CfFlowInfo_mt_place_set();_tmp43;});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp45;void*_tmp46;struct _tuple5 _tmp44=Cyc_NewControlFlow_anal_Rexp(env,_tmp42,(
struct Cyc_Absyn_Exp*)es->hd);_tmp45=_tmp44.f1;_tmp46=_tmp44.f2;outflow=_tmp45;
rvals=({struct Cyc_List_List*_tmp47=_region_malloc(rgn,sizeof(*_tmp47));_tmp47->hd=(
void*)_tmp46;_tmp47->tl=0;_tmp47;});this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);if(
arg1_unconsumed){union Cyc_CfFlowInfo_FlowInfo_union _tmp48=outflow;struct Cyc_Dict_Dict
_tmp49;struct Cyc_List_List*_tmp4A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4B;_LL17:
if((_tmp48.BottomFL).tag != 0)goto _LL19;_LL18: goto _LL16;_LL19: if((_tmp48.ReachableFL).tag
!= 1)goto _LL16;_tmp49=(_tmp48.ReachableFL).f1;_tmp4A=(_tmp48.ReachableFL).f2;
_tmp4B=(_tmp48.ReachableFL).f3;_LL1A: outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4C;(_tmp4C.ReachableFL).tag=1;(_tmp4C.ReachableFL).f1=
_tmp49;(_tmp4C.ReachableFL).f2=_tmp4A;(_tmp4C.ReachableFL).f3=({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp4D;_tmp4D.consumed=_tmp4B.consumed;_tmp4D.may_consume=_tmp41.may_consume;
_tmp4D;});_tmp4C;});_LL16:;}{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=
es2->tl){env->all_changed=({struct Cyc_Dict_Dict*_tmp4E=_cycalloc(sizeof(*_tmp4E));
_tmp4E[0]=Cyc_CfFlowInfo_mt_place_set();_tmp4E;});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp50;void*_tmp51;struct _tuple5 _tmp4F=Cyc_NewControlFlow_anal_Rexp(env,_tmp42,(
struct Cyc_Absyn_Exp*)es2->hd);_tmp50=_tmp4F.f1;_tmp51=_tmp4F.f2;rvals=({struct
Cyc_List_List*_tmp52=_region_malloc(rgn,sizeof(*_tmp52));_tmp52->hd=(void*)
_tmp51;_tmp52->tl=rvals;_tmp52;});outflow=Cyc_CfFlowInfo_after_flow((struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp50,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);}}}{
struct Cyc_CfFlowInfo_ConsumeInfo _tmp54;struct _tuple6 _tmp53=Cyc_CfFlowInfo_save_consume_info(
outflow,0);_tmp54=_tmp53.f1;if(init_consume)old_inflow=Cyc_CfFlowInfo_restore_consume_info(
_tmp42,outflow_consume);else{old_inflow=_tmp42;}init_consume=1;outflow_consume=
_tmp54;_tmp42=Cyc_CfFlowInfo_join_flow(outer_all_changed,_tmp42,outflow,1);}}}while(
!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp42,old_inflow));if(outer_all_changed == 
0)env->all_changed=0;else{env->all_changed=({struct Cyc_Dict_Dict*_tmp55=
_cycalloc(sizeof(*_tmp55));_tmp55[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0);_tmp55;});}Cyc_NewControlFlow_update_tryflow(
env,outflow);return({struct _tuple9 _tmp56;_tmp56.f1=outflow;_tmp56.f2=Cyc_List_imp_rev(
rvals);_tmp56;});}}}static struct _tuple5 Cyc_NewControlFlow_anal_use_ints(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct
Cyc_List_List*es,int arg1_unconsumed){struct _RegionHandle _tmp58=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp58;_push_region(rgn);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5A;struct Cyc_List_List*_tmp5B;struct _tuple9 _tmp59=Cyc_NewControlFlow_anal_unordered_Rexps(
rgn,env,inflow,es,arg1_unconsumed);_tmp5A=_tmp59.f1;_tmp5B=_tmp59.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5C=_tmp5A;struct Cyc_Dict_Dict _tmp5D;_LL1C: if((_tmp5C.ReachableFL).tag != 1)
goto _LL1E;_tmp5D=(_tmp5C.ReachableFL).f1;_LL1D: for(0;_tmp5B != 0;(_tmp5B=_tmp5B->tl,
es=((struct Cyc_List_List*)_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(
_tmp5D,(void*)_tmp5B->hd)== (void*)0)({void*_tmp5E[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,({const
char*_tmp5F="expression may not be initialized";_tag_dynforward(_tmp5F,sizeof(
char),_get_zero_arr_size(_tmp5F,34));}),_tag_dynforward(_tmp5E,sizeof(void*),0));});}
goto _LL1B;_LL1E: if((_tmp5C.BottomFL).tag != 0)goto _LL1B;_LL1F: goto _LL1B;_LL1B:;}{
struct _tuple5 _tmp61=({struct _tuple5 _tmp60;_tmp60.f1=_tmp5A;_tmp60.f2=Cyc_CfFlowInfo_unknown_all;
_tmp60;});_npop_handler(0);return _tmp61;}};_pop_region(rgn);}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Absyn_Exp*e,void*il){
union Cyc_CfFlowInfo_FlowInfo_union _tmp62=outflow;struct Cyc_Dict_Dict _tmp63;
struct Cyc_List_List*_tmp64;struct Cyc_CfFlowInfo_ConsumeInfo _tmp65;_LL21: if((
_tmp62.BottomFL).tag != 0)goto _LL23;_LL22: return outflow;_LL23: if((_tmp62.ReachableFL).tag
!= 1)goto _LL20;_tmp63=(_tmp62.ReachableFL).f1;_tmp64=(_tmp62.ReachableFL).f2;
_tmp65=(_tmp62.ReachableFL).f3;_LL24: {void*_tmp66=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp67;_LL26: if((int)_tmp66 != 0)
goto _LL28;_LL27: return outflow;_LL28: if(_tmp66 <= (void*)1)goto _LL25;if(*((int*)
_tmp66)!= 0)goto _LL25;_tmp67=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp66)->f1;
_LL29: {void*nzval=il == (void*)2?(void*)1:(void*)2;return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp68;(_tmp68.ReachableFL).tag=1;(_tmp68.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(e->loc,_tmp63,env->all_changed,_tmp67,nzval);(_tmp68.ReachableFL).f2=
_tmp64;(_tmp68.ReachableFL).f3=_tmp65;_tmp68;});}_LL25:;}_LL20:;}static struct
_tuple7 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union outflow,
struct Cyc_Absyn_Exp*e,void*il){union Cyc_CfFlowInfo_FlowInfo_union _tmp69=outflow;
struct Cyc_Dict_Dict _tmp6A;struct Cyc_List_List*_tmp6B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp6C;_LL2B: if((_tmp69.BottomFL).tag != 0)goto _LL2D;_LL2C: return({struct _tuple7
_tmp6D;_tmp6D.f1=outflow;_tmp6D.f2=outflow;_tmp6D;});_LL2D: if((_tmp69.ReachableFL).tag
!= 1)goto _LL2A;_tmp6A=(_tmp69.ReachableFL).f1;_tmp6B=(_tmp69.ReachableFL).f2;
_tmp6C=(_tmp69.ReachableFL).f3;_LL2E: {void*_tmp6E=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp6F;_LL30: if((int)_tmp6E != 0)
goto _LL32;_LL31: return({struct _tuple7 _tmp70;_tmp70.f1=outflow;_tmp70.f2=outflow;
_tmp70;});_LL32: if(_tmp6E <= (void*)1)goto _LL2F;if(*((int*)_tmp6E)!= 0)goto _LL2F;
_tmp6F=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp6E)->f1;_LL33: {void*nzval=il == (
void*)2?(void*)1:(void*)2;return({struct _tuple7 _tmp71;_tmp71.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp73;(_tmp73.ReachableFL).tag=1;(_tmp73.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(e->loc,_tmp6A,env->all_changed,_tmp6F,nzval);(_tmp73.ReachableFL).f2=
_tmp6B;(_tmp73.ReachableFL).f3=_tmp6C;_tmp73;});_tmp71.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp72;(_tmp72.ReachableFL).tag=1;(_tmp72.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(e->loc,_tmp6A,env->all_changed,_tmp6F,(void*)0);(
_tmp72.ReachableFL).f2=_tmp6B;(_tmp72.ReachableFL).f3=_tmp6C;_tmp72;});_tmp71;});}
_LL2F:;}_LL2A:;}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_if_tagcmp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union flow,
struct Cyc_Absyn_Exp*e1,void*r1,void*p,void*r2){union Cyc_CfFlowInfo_FlowInfo_union
_tmp74=flow;struct Cyc_Dict_Dict _tmp75;struct Cyc_List_List*_tmp76;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp77;_LL35: if((_tmp74.BottomFL).tag != 0)goto _LL37;_LL36: return flow;_LL37: if((
_tmp74.ReachableFL).tag != 1)goto _LL34;_tmp75=(_tmp74.ReachableFL).f1;_tmp76=(
_tmp74.ReachableFL).f2;_tmp77=(_tmp74.ReachableFL).f3;_LL38: {void*_tmp78=r2;
struct Cyc_List_List*_tmp79;_LL3A: if(_tmp78 <= (void*)3)goto _LL3C;if(*((int*)
_tmp78)!= 3)goto _LL3C;_tmp79=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp78)->f1;
_LL3B: {void*_tmp7A=(Cyc_NewControlFlow_anal_Lexp(env,flow,e1)).f2;struct Cyc_CfFlowInfo_Place*
_tmp7B;_LL3F: if((int)_tmp7A != 0)goto _LL41;_LL40: return flow;_LL41: if(_tmp7A <= (
void*)1)goto _LL3E;if(*((int*)_tmp7A)!= 0)goto _LL3E;_tmp7B=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp7A)->f1;_LL42: {struct Cyc_List_List*new_cl;{void*_tmp7C=r1;struct Cyc_List_List*
_tmp7D;void*_tmp7E;_LL44: if(_tmp7C <= (void*)3)goto _LL48;if(*((int*)_tmp7C)!= 3)
goto _LL46;_tmp7D=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp7C)->f1;_LL45: new_cl=
_tmp7D;goto _LL43;_LL46: if(*((int*)_tmp7C)!= 0)goto _LL48;_tmp7E=(void*)((struct
Cyc_CfFlowInfo_UnknownR_struct*)_tmp7C)->f1;if((int)_tmp7E != 2)goto _LL48;_LL47:
goto _LL49;_LL48: if((int)_tmp7C != 0)goto _LL4A;_LL49: goto _LL4B;_LL4A: if((int)
_tmp7C != 1)goto _LL4C;_LL4B: new_cl=0;goto _LL43;_LL4C:;_LL4D: return flow;_LL43:;}
for(0;_tmp79 != 0;_tmp79=_tmp79->tl){void*new_cmp;{struct _tuple0 _tmp80=({struct
_tuple0 _tmp7F;_tmp7F.f1=p;_tmp7F.f2=(void*)((struct Cyc_CfFlowInfo_TagCmp*)_tmp79->hd)->cmp;
_tmp7F;});void*_tmp81;void*_tmp82;void*_tmp83;void*_tmp84;_LL4F: _tmp81=_tmp80.f1;
if((int)_tmp81 != 8)goto _LL51;_LL50: goto _LL52;_LL51: _tmp82=_tmp80.f2;if((int)
_tmp82 != 8)goto _LL53;_LL52: new_cmp=(void*)8;goto _LL4E;_LL53: _tmp83=_tmp80.f1;if((
int)_tmp83 != 5)goto _LL55;_tmp84=_tmp80.f2;if((int)_tmp84 != 5)goto _LL55;_LL54:
new_cmp=(void*)5;goto _LL4E;_LL55:;_LL56: new_cmp=(void*)10;goto _LL4E;_LL4E:;}
new_cl=({struct Cyc_List_List*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->hd=({
struct Cyc_CfFlowInfo_TagCmp*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->cmp=(void*)
new_cmp;_tmp86->bd=(void*)((void*)((struct Cyc_CfFlowInfo_TagCmp*)_tmp79->hd)->bd);
_tmp86;});_tmp85->tl=new_cl;_tmp85;});}return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp87;(_tmp87.ReachableFL).tag=1;(_tmp87.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(e1->loc,_tmp75,env->all_changed,_tmp7B,(void*)({
struct Cyc_CfFlowInfo_TagCmps_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88[0]=({
struct Cyc_CfFlowInfo_TagCmps_struct _tmp89;_tmp89.tag=3;_tmp89.f1=new_cl;_tmp89;});
_tmp88;}));(_tmp87.ReachableFL).f2=_tmp76;(_tmp87.ReachableFL).f3=_tmp77;_tmp87;});}
_LL3E:;}_LL3C:;_LL3D: return flow;_LL39:;}_LL34:;}static struct _tuple5 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
union Cyc_CfFlowInfo_FlowInfo_union f,struct Cyc_Absyn_Exp*e,void*r){void*_tmp8A=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp8B;void*_tmp8C;struct Cyc_Absyn_PtrAtts _tmp8D;struct
Cyc_Absyn_Conref*_tmp8E;struct Cyc_Absyn_Conref*_tmp8F;_LL58: if(_tmp8A <= (void*)4)
goto _LL5A;if(*((int*)_tmp8A)!= 4)goto _LL5A;_tmp8B=((struct Cyc_Absyn_PointerType_struct*)
_tmp8A)->f1;_tmp8C=(void*)_tmp8B.elt_typ;_tmp8D=_tmp8B.ptr_atts;_tmp8E=_tmp8D.bounds;
_tmp8F=_tmp8D.zero_term;_LL59: {union Cyc_CfFlowInfo_FlowInfo_union _tmp90=f;
struct Cyc_Dict_Dict _tmp91;struct Cyc_List_List*_tmp92;_LL5D: if((_tmp90.BottomFL).tag
!= 0)goto _LL5F;_LL5E: return({struct _tuple5 _tmp93;_tmp93.f1=f;_tmp93.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp8C,Cyc_CfFlowInfo_unknown_all);_tmp93;});_LL5F: if((_tmp90.ReachableFL).tag != 
1)goto _LL5C;_tmp91=(_tmp90.ReachableFL).f1;_tmp92=(_tmp90.ReachableFL).f2;_LL60:
if(Cyc_Tcutil_is_bound_one(_tmp8E)){void*_tmp94=r;struct Cyc_CfFlowInfo_Place*
_tmp95;void*_tmp96;_LL62: if((int)_tmp94 != 1)goto _LL64;_LL63: goto _LL65;_LL64: if((
int)_tmp94 != 2)goto _LL66;_LL65:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp98;_tmp98.tag=Cyc_CfFlowInfo_NotZero;_tmp98.f1=_tmp92;_tmp98;});_tmp97;})));
goto _LL61;_LL66: if(_tmp94 <= (void*)3)goto _LL68;if(*((int*)_tmp94)!= 2)goto _LL68;
_tmp95=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp94)->f1;_LL67:(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp99=_cycalloc(sizeof(*
_tmp99));_tmp99[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp9A;_tmp9A.tag=Cyc_CfFlowInfo_NotZero;
_tmp9A.f1=_tmp92;_tmp9A;});_tmp99;})));return({struct _tuple5 _tmp9B;_tmp9B.f1=f;
_tmp9B.f2=Cyc_CfFlowInfo_lookup_place(_tmp91,_tmp95);_tmp9B;});_LL68: if((int)
_tmp94 != 0)goto _LL6A;_LL69:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));
return({struct _tuple5 _tmp9C;_tmp9C.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp9D;(_tmp9D.BottomFL).tag=0;_tmp9D;});
_tmp9C.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp8C,Cyc_CfFlowInfo_unknown_all);
_tmp9C;});_LL6A: if(_tmp94 <= (void*)3)goto _LL6C;if(*((int*)_tmp94)!= 0)goto _LL6C;
_tmp96=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp94)->f1;_LL6B: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmp96);goto _LL6D;_LL6C:;_LL6D:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E[0]=({
struct Cyc_CfFlowInfo_UnknownZ_struct _tmp9F;_tmp9F.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp9F.f1=_tmp92;_tmp9F;});_tmp9E;})));_LL61:;}else{(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));
_tmpA0[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmpA1;_tmpA1.tag=Cyc_CfFlowInfo_UnknownZ;
_tmpA1.f1=_tmp92;_tmpA1;});_tmpA0;})));}{void*_tmpA2=Cyc_CfFlowInfo_initlevel(
_tmp91,r);_LL6F: if((int)_tmpA2 != 0)goto _LL71;_LL70:({void*_tmpA3[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmpA4="dereference of possibly uninitialized pointer";
_tag_dynforward(_tmpA4,sizeof(char),_get_zero_arr_size(_tmpA4,46));}),
_tag_dynforward(_tmpA3,sizeof(void*),0));});goto _LL72;_LL71: if((int)_tmpA2 != 2)
goto _LL73;_LL72: return({struct _tuple5 _tmpA5;_tmpA5.f1=f;_tmpA5.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp8C,Cyc_CfFlowInfo_unknown_all);_tmpA5;});_LL73: if((int)_tmpA2 != 1)goto _LL6E;
_LL74: return({struct _tuple5 _tmpA6;_tmpA6.f1=f;_tmpA6.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp8C,Cyc_CfFlowInfo_unknown_none);_tmpA6;});_LL6E:;}_LL5C:;}_LL5A:;_LL5B:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpA7=_cycalloc(sizeof(*_tmpA7));
_tmpA7[0]=({struct Cyc_Core_Impossible_struct _tmpA8;_tmpA8.tag=Cyc_Core_Impossible;
_tmpA8.f1=({const char*_tmpA9="right deref of non-pointer-type";_tag_dynforward(
_tmpA9,sizeof(char),_get_zero_arr_size(_tmpA9,32));});_tmpA8;});_tmpA7;}));_LL57:;}
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmpAA=(void*)e1->r;
void*_tmpAB;struct Cyc_Absyn_Vardecl*_tmpAC;void*_tmpAD;struct Cyc_Absyn_Vardecl*
_tmpAE;void*_tmpAF;struct Cyc_Absyn_Vardecl*_tmpB0;void*_tmpB1;struct Cyc_Absyn_Vardecl*
_tmpB2;_LL76: if(*((int*)_tmpAA)!= 1)goto _LL78;_tmpAB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpAA)->f2;if(_tmpAB <= (void*)1)goto _LL78;if(*((int*)_tmpAB)!= 4)goto _LL78;
_tmpAC=((struct Cyc_Absyn_Pat_b_struct*)_tmpAB)->f1;_LL77: _tmpAE=_tmpAC;goto _LL79;
_LL78: if(*((int*)_tmpAA)!= 1)goto _LL7A;_tmpAD=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpAA)->f2;if(_tmpAD <= (void*)1)goto _LL7A;if(*((int*)_tmpAD)!= 3)goto _LL7A;
_tmpAE=((struct Cyc_Absyn_Local_b_struct*)_tmpAD)->f1;_LL79: _tmpB0=_tmpAE;goto
_LL7B;_LL7A: if(*((int*)_tmpAA)!= 1)goto _LL7C;_tmpAF=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpAA)->f2;if(_tmpAF <= (void*)1)goto _LL7C;if(*((int*)_tmpAF)!= 2)goto _LL7C;
_tmpB0=((struct Cyc_Absyn_Param_b_struct*)_tmpAF)->f1;_LL7B: _tmpB2=_tmpB0;goto
_LL7D;_LL7C: if(*((int*)_tmpAA)!= 1)goto _LL7E;_tmpB1=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpAA)->f2;if(_tmpB1 <= (void*)1)goto _LL7E;if(*((int*)_tmpB1)!= 0)goto _LL7E;
_tmpB2=((struct Cyc_Absyn_Global_b_struct*)_tmpB1)->f1;_LL7D: if(!_tmpB2->escapes){
void*_tmpB3=(void*)e2->r;void*_tmpB4;struct Cyc_Absyn_Vardecl*_tmpB5;void*_tmpB6;
struct Cyc_Absyn_Vardecl*_tmpB7;void*_tmpB8;struct Cyc_Absyn_Vardecl*_tmpB9;void*
_tmpBA;struct Cyc_Absyn_Vardecl*_tmpBB;_LL81: if(*((int*)_tmpB3)!= 1)goto _LL83;
_tmpB4=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpB3)->f2;if(_tmpB4 <= (void*)1)
goto _LL83;if(*((int*)_tmpB4)!= 4)goto _LL83;_tmpB5=((struct Cyc_Absyn_Pat_b_struct*)
_tmpB4)->f1;_LL82: _tmpB7=_tmpB5;goto _LL84;_LL83: if(*((int*)_tmpB3)!= 1)goto _LL85;
_tmpB6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpB3)->f2;if(_tmpB6 <= (void*)1)
goto _LL85;if(*((int*)_tmpB6)!= 3)goto _LL85;_tmpB7=((struct Cyc_Absyn_Local_b_struct*)
_tmpB6)->f1;_LL84: _tmpB9=_tmpB7;goto _LL86;_LL85: if(*((int*)_tmpB3)!= 1)goto _LL87;
_tmpB8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpB3)->f2;if(_tmpB8 <= (void*)1)
goto _LL87;if(*((int*)_tmpB8)!= 2)goto _LL87;_tmpB9=((struct Cyc_Absyn_Param_b_struct*)
_tmpB8)->f1;_LL86: _tmpBB=_tmpB9;goto _LL88;_LL87: if(*((int*)_tmpB3)!= 1)goto _LL89;
_tmpBA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpB3)->f2;if(_tmpBA <= (void*)1)
goto _LL89;if(*((int*)_tmpBA)!= 0)goto _LL89;_tmpBB=((struct Cyc_Absyn_Global_b_struct*)
_tmpBA)->f1;_LL88: if(!_tmpBB->escapes){int found=0;{struct Cyc_List_List*_tmpBC=
relns;for(0;_tmpBC != 0;_tmpBC=_tmpBC->tl){struct Cyc_CfFlowInfo_Reln*_tmpBD=(
struct Cyc_CfFlowInfo_Reln*)_tmpBC->hd;if(_tmpBD->vd == _tmpBB){union Cyc_CfFlowInfo_RelnOp_union
_tmpBE=_tmpBD->rop;struct Cyc_Absyn_Vardecl*_tmpBF;_LL8C: if((_tmpBE.LessSize).tag
!= 2)goto _LL8E;_tmpBF=(_tmpBE.LessSize).f1;if(!(_tmpBF == _tmpB2))goto _LL8E;_LL8D:
return relns;_LL8E:;_LL8F: continue;_LL8B:;}}}if(!found)return({struct Cyc_List_List*
_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->hd=({struct Cyc_CfFlowInfo_Reln*_tmpC1=
_cycalloc(sizeof(*_tmpC1));_tmpC1->vd=_tmpBB;_tmpC1->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmpC2;(_tmpC2.LessSize).tag=2;(_tmpC2.LessSize).f1=
_tmpB2;_tmpC2;});_tmpC1;});_tmpC0->tl=relns;_tmpC0;});}return relns;_LL89:;_LL8A:
return relns;_LL80:;}else{return relns;}_LL7E:;_LL7F: return relns;_LL75:;}struct
_tuple10{void*f1;union Cyc_CfFlowInfo_FlowInfo_union f2;};struct _tuple11{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_Dict_Dict d;struct Cyc_List_List*relns;struct Cyc_CfFlowInfo_ConsumeInfo
cinfo;{union Cyc_CfFlowInfo_FlowInfo_union _tmpC3=inflow;struct Cyc_Dict_Dict _tmpC4;
struct Cyc_List_List*_tmpC5;struct Cyc_CfFlowInfo_ConsumeInfo _tmpC6;_LL91: if((
_tmpC3.BottomFL).tag != 0)goto _LL93;_LL92: return({struct _tuple5 _tmpC7;_tmpC7.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmpC8;(
_tmpC8.BottomFL).tag=0;_tmpC8;});_tmpC7.f2=Cyc_CfFlowInfo_unknown_all;_tmpC7;});
_LL93: if((_tmpC3.ReachableFL).tag != 1)goto _LL90;_tmpC4=(_tmpC3.ReachableFL).f1;
_tmpC5=(_tmpC3.ReachableFL).f2;_tmpC6=(_tmpC3.ReachableFL).f3;_LL94: d=_tmpC4;
relns=_tmpC5;cinfo=_tmpC6;_LL90:;}{void*_tmpC9=(void*)e->r;struct Cyc_Absyn_Exp*
_tmpCA;void*_tmpCB;struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Absyn_Exp*_tmpCD;struct
Cyc_Absyn_Exp*_tmpCE;union Cyc_Absyn_Cnst_union _tmpCF;union Cyc_Absyn_Cnst_union
_tmpD0;int _tmpD1;union Cyc_Absyn_Cnst_union _tmpD2;void*_tmpD3;struct Cyc_List_List*
_tmpD4;void*_tmpD5;void*_tmpD6;struct Cyc_Absyn_Vardecl*_tmpD7;void*_tmpD8;struct
Cyc_Absyn_Vardecl*_tmpD9;void*_tmpDA;struct Cyc_Absyn_Vardecl*_tmpDB;void*_tmpDC;
struct Cyc_List_List*_tmpDD;struct Cyc_Absyn_Exp*_tmpDE;struct Cyc_Absyn_Exp*_tmpDF;
struct Cyc_Core_Opt*_tmpE0;struct Cyc_Core_Opt _tmpE1;struct Cyc_Absyn_Exp*_tmpE2;
struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Core_Opt*_tmpE4;struct Cyc_Absyn_Exp*_tmpE5;
struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_Absyn_Exp*_tmpE8;
struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_List_List*_tmpEA;struct Cyc_Absyn_MallocInfo
_tmpEB;int _tmpEC;struct Cyc_Absyn_Exp*_tmpED;void**_tmpEE;struct Cyc_Absyn_Exp*
_tmpEF;int _tmpF0;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_Absyn_Exp*
_tmpF3;struct Cyc_Absyn_Exp*_tmpF4;struct Cyc_Absyn_Exp*_tmpF5;struct
_dynforward_ptr*_tmpF6;struct Cyc_Absyn_Exp*_tmpF7;struct _dynforward_ptr*_tmpF8;
struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;struct Cyc_Absyn_Exp*_tmpFB;
struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_Absyn_Exp*_tmpFE;
struct Cyc_Absyn_Exp*_tmpFF;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*
_tmp101;struct Cyc_List_List*_tmp102;struct Cyc_Absyn_Tuniondecl*_tmp103;struct Cyc_List_List*
_tmp104;struct Cyc_List_List*_tmp105;struct Cyc_List_List*_tmp106;struct Cyc_Absyn_Aggrdecl*
_tmp107;struct Cyc_Absyn_Aggrdecl _tmp108;struct Cyc_Absyn_AggrdeclImpl*_tmp109;
struct Cyc_Absyn_AggrdeclImpl _tmp10A;struct Cyc_List_List*_tmp10B;struct Cyc_List_List*
_tmp10C;struct Cyc_Absyn_Vardecl*_tmp10D;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*
_tmp10F;int _tmp110;struct Cyc_Absyn_Stmt*_tmp111;void*_tmp112;_LL96: if(*((int*)
_tmpC9)!= 15)goto _LL98;_tmpCA=((struct Cyc_Absyn_Cast_e_struct*)_tmpC9)->f2;
_tmpCB=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpC9)->f4;if((int)_tmpCB != 2)
goto _LL98;_LL97: {union Cyc_CfFlowInfo_FlowInfo_union _tmp114;void*_tmp115;struct
_tuple5 _tmp113=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpCA);_tmp114=_tmp113.f1;
_tmp115=_tmp113.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp117;void*_tmp118;
struct _tuple5 _tmp116=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp114,_tmpCA,
_tmp115);_tmp117=_tmp116.f1;_tmp118=_tmp116.f2;return({struct _tuple5 _tmp119;
_tmp119.f1=_tmp117;_tmp119.f2=_tmp115;_tmp119;});}}_LL98: if(*((int*)_tmpC9)!= 15)
goto _LL9A;_tmpCC=((struct Cyc_Absyn_Cast_e_struct*)_tmpC9)->f2;_LL99: _tmpCD=
_tmpCC;goto _LL9B;_LL9A: if(*((int*)_tmpC9)!= 13)goto _LL9C;_tmpCD=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpC9)->f1;_LL9B: _tmpCE=_tmpCD;goto _LL9D;_LL9C: if(*((int*)_tmpC9)!= 14)goto
_LL9E;_tmpCE=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpC9)->f1;_LL9D: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpCE);_LL9E: if(*((int*)_tmpC9)!= 0)goto _LLA0;_tmpCF=((struct Cyc_Absyn_Const_e_struct*)
_tmpC9)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpC9)->f1).Null_c).tag != 6)
goto _LLA0;_LL9F: goto _LLA1;_LLA0: if(*((int*)_tmpC9)!= 0)goto _LLA2;_tmpD0=((struct
Cyc_Absyn_Const_e_struct*)_tmpC9)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpC9)->f1).Int_c).tag != 2)goto _LLA2;_tmpD1=(_tmpD0.Int_c).f2;if(_tmpD1 != 0)
goto _LLA2;_LLA1: return({struct _tuple5 _tmp11A;_tmp11A.f1=inflow;_tmp11A.f2=(void*)
0;_tmp11A;});_LLA2: if(*((int*)_tmpC9)!= 0)goto _LLA4;_tmpD2=((struct Cyc_Absyn_Const_e_struct*)
_tmpC9)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpC9)->f1).Int_c).tag != 2)
goto _LLA4;_LLA3: goto _LLA5;_LLA4: if(*((int*)_tmpC9)!= 1)goto _LLA6;_tmpD3=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpC9)->f2;if(_tmpD3 <= (void*)1)goto _LLA6;if(*((
int*)_tmpD3)!= 1)goto _LLA6;_LLA5: return({struct _tuple5 _tmp11B;_tmp11B.f1=inflow;
_tmp11B.f2=(void*)1;_tmp11B;});_LLA6: if(*((int*)_tmpC9)!= 32)goto _LLA8;_tmpD4=((
struct Cyc_Absyn_Tunion_e_struct*)_tmpC9)->f1;if(_tmpD4 != 0)goto _LLA8;_LLA7: goto
_LLA9;_LLA8: if(*((int*)_tmpC9)!= 0)goto _LLAA;_LLA9: goto _LLAB;_LLAA: if(*((int*)
_tmpC9)!= 19)goto _LLAC;_LLAB: goto _LLAD;_LLAC: if(*((int*)_tmpC9)!= 18)goto _LLAE;
_LLAD: goto _LLAF;_LLAE: if(*((int*)_tmpC9)!= 20)goto _LLB0;_LLAF: goto _LLB1;_LLB0:
if(*((int*)_tmpC9)!= 21)goto _LLB2;_LLB1: goto _LLB3;_LLB2: if(*((int*)_tmpC9)!= 34)
goto _LLB4;_LLB3: goto _LLB5;_LLB4: if(*((int*)_tmpC9)!= 33)goto _LLB6;_LLB5: return({
struct _tuple5 _tmp11C;_tmp11C.f1=inflow;_tmp11C.f2=Cyc_CfFlowInfo_unknown_all;
_tmp11C;});_LLB6: if(*((int*)_tmpC9)!= 1)goto _LLB8;_tmpD5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC9)->f2;if(_tmpD5 <= (void*)1)goto _LLB8;if(*((int*)_tmpD5)!= 0)goto _LLB8;
_LLB7: return({struct _tuple5 _tmp11D;_tmp11D.f1=inflow;_tmp11D.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp11D;});_LLB8: if(*((int*)_tmpC9)!= 1)goto _LLBA;_tmpD6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC9)->f2;if(_tmpD6 <= (void*)1)goto _LLBA;if(*((int*)_tmpD6)!= 2)goto _LLBA;
_tmpD7=((struct Cyc_Absyn_Param_b_struct*)_tmpD6)->f1;_LLB9: _tmpD9=_tmpD7;goto
_LLBB;_LLBA: if(*((int*)_tmpC9)!= 1)goto _LLBC;_tmpD8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC9)->f2;if(_tmpD8 <= (void*)1)goto _LLBC;if(*((int*)_tmpD8)!= 3)goto _LLBC;
_tmpD9=((struct Cyc_Absyn_Local_b_struct*)_tmpD8)->f1;_LLBB: _tmpDB=_tmpD9;goto
_LLBD;_LLBC: if(*((int*)_tmpC9)!= 1)goto _LLBE;_tmpDA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC9)->f2;if(_tmpDA <= (void*)1)goto _LLBE;if(*((int*)_tmpDA)!= 4)goto _LLBE;
_tmpDB=((struct Cyc_Absyn_Pat_b_struct*)_tmpDA)->f1;_LLBD: if(Cyc_CfFlowInfo_contains_region((
void*)0,(void*)_tmpDB->type)){struct Cyc_CfFlowInfo_VarRoot_struct*_tmp11E=({
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123[
0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp124;_tmp124.tag=0;_tmp124.f1=_tmpDB;
_tmp124;});_tmp123;});struct Cyc_CfFlowInfo_Place*_tmp11F=({struct Cyc_CfFlowInfo_Place*
_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->root=(void*)((void*)_tmp11E);
_tmp122->fields=0;_tmp122;});cinfo.may_consume=((struct Cyc_List_List*(*)(int(*
cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_merge)(Cyc_CfFlowInfo_place_cmp,({struct Cyc_List_List*
_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->hd=_tmp11F;_tmp120->tl=0;_tmp120;}),
cinfo.may_consume);inflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp121;(_tmp121.ReachableFL).tag=1;(_tmp121.ReachableFL).f1=d;(_tmp121.ReachableFL).f2=
relns;(_tmp121.ReachableFL).f3=cinfo;_tmp121;});}return({struct _tuple5 _tmp125;
_tmp125.f1=inflow;_tmp125.f2=Cyc_Dict_lookup(d,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp127;_tmp127.tag=0;_tmp127.f1=_tmpDB;_tmp127;});_tmp126;}));_tmp125;});_LLBE:
if(*((int*)_tmpC9)!= 3)goto _LLC0;_tmpDC=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmpC9)->f1;_tmpDD=((struct Cyc_Absyn_Primop_e_struct*)_tmpC9)->f2;_LLBF: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp129;void*_tmp12A;struct _tuple5 _tmp128=Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmpDD,0);_tmp129=_tmp128.f1;_tmp12A=_tmp128.f2;{void*_tmp12B=_tmpDC;
_LLFB: if((int)_tmp12B != 0)goto _LLFD;_LLFC: goto _LLFE;_LLFD: if((int)_tmp12B != 2)
goto _LLFF;_LLFE: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpDD))->hd)->loc,_tmp129);goto _LLFA;_LLFF:;
_LL100: _tmp129=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpDD))->hd)->loc,_tmp129);goto _LLFA;_LLFA:;}
return({struct _tuple5 _tmp12C;_tmp12C.f1=_tmp129;_tmp12C.f2=_tmp12A;_tmp12C;});}
_LLC0: if(*((int*)_tmpC9)!= 5)goto _LLC2;_tmpDE=((struct Cyc_Absyn_Increment_e_struct*)
_tmpC9)->f1;_LLC1: {struct Cyc_List_List _tmp12D=({struct Cyc_List_List _tmp13C;
_tmp13C.hd=_tmpDE;_tmp13C.tl=0;_tmp13C;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp12F;struct _tuple5 _tmp12E=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct
Cyc_List_List*)& _tmp12D,0);_tmp12F=_tmp12E.f1;Cyc_CfFlowInfo_check_unique_rvals(
_tmpDE->loc,_tmp12F);{void*_tmp131;struct _tuple5 _tmp130=Cyc_NewControlFlow_anal_Lexp(
env,_tmp12F,_tmpDE);_tmp131=_tmp130.f2;{struct _tuple10 _tmp133=({struct _tuple10
_tmp132;_tmp132.f1=_tmp131;_tmp132.f2=_tmp12F;_tmp132;});void*_tmp134;struct Cyc_CfFlowInfo_Place*
_tmp135;union Cyc_CfFlowInfo_FlowInfo_union _tmp136;struct Cyc_Dict_Dict _tmp137;
struct Cyc_List_List*_tmp138;struct Cyc_CfFlowInfo_ConsumeInfo _tmp139;_LL102:
_tmp134=_tmp133.f1;if(_tmp134 <= (void*)1)goto _LL104;if(*((int*)_tmp134)!= 0)goto
_LL104;_tmp135=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp134)->f1;_tmp136=
_tmp133.f2;if(((_tmp133.f2).ReachableFL).tag != 1)goto _LL104;_tmp137=(_tmp136.ReachableFL).f1;
_tmp138=(_tmp136.ReachableFL).f2;_tmp139=(_tmp136.ReachableFL).f3;_LL103: _tmp138=
Cyc_CfFlowInfo_reln_kill_exp(_tmp138,_tmpDE);_tmp12F=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp13A;(_tmp13A.ReachableFL).tag=1;(_tmp13A.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(_tmpDE->loc,_tmp137,env->all_changed,_tmp135,Cyc_CfFlowInfo_unknown_all);(
_tmp13A.ReachableFL).f2=_tmp138;(_tmp13A.ReachableFL).f3=_tmp139;_tmp13A;});goto
_LL101;_LL104:;_LL105: goto _LL101;_LL101:;}return({struct _tuple5 _tmp13B;_tmp13B.f1=
_tmp12F;_tmp13B.f2=Cyc_CfFlowInfo_unknown_all;_tmp13B;});}}_LLC2: if(*((int*)
_tmpC9)!= 4)goto _LLC4;_tmpDF=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpC9)->f1;
_tmpE0=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpC9)->f2;if(_tmpE0 == 0)goto _LLC4;
_tmpE1=*_tmpE0;_tmpE2=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpC9)->f3;_LLC3: {
struct Cyc_List_List _tmp13D=({struct Cyc_List_List _tmp15B;_tmp15B.hd=_tmpE2;
_tmp15B.tl=0;_tmp15B;});struct Cyc_List_List _tmp13E=({struct Cyc_List_List _tmp15A;
_tmp15A.hd=_tmpDF;_tmp15A.tl=(struct Cyc_List_List*)& _tmp13D;_tmp15A;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp140;struct _tuple5 _tmp13F=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct
Cyc_List_List*)& _tmp13E,1);_tmp140=_tmp13F.f1;{void*_tmp142;struct _tuple5 _tmp141=
Cyc_NewControlFlow_anal_Lexp(env,_tmp140,e);_tmp142=_tmp141.f2;_tmp140=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp140);{struct _tuple10 _tmp144=({struct _tuple10 _tmp143;_tmp143.f1=
_tmp142;_tmp143.f2=_tmp140;_tmp143;});void*_tmp145;struct Cyc_CfFlowInfo_Place*
_tmp146;union Cyc_CfFlowInfo_FlowInfo_union _tmp147;struct Cyc_Dict_Dict _tmp148;
struct Cyc_List_List*_tmp149;struct Cyc_CfFlowInfo_ConsumeInfo _tmp14A;_LL107:
_tmp145=_tmp144.f1;if(_tmp145 <= (void*)1)goto _LL109;if(*((int*)_tmp145)!= 0)goto
_LL109;_tmp146=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp145)->f1;_tmp147=
_tmp144.f2;if(((_tmp144.f2).ReachableFL).tag != 1)goto _LL109;_tmp148=(_tmp147.ReachableFL).f1;
_tmp149=(_tmp147.ReachableFL).f2;_tmp14A=(_tmp147.ReachableFL).f3;_LL108: _tmp149=
Cyc_CfFlowInfo_reln_kill_exp(_tmp149,_tmpDF);_tmp148=Cyc_CfFlowInfo_assign_place(
_tmpDF->loc,_tmp148,env->all_changed,_tmp146,Cyc_CfFlowInfo_unknown_all);if(Cyc_CfFlowInfo_contains_region((
void*)0,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpDF->topt))->v)){_tmp14A.may_consume=((
struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_merge)(Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B->hd=_tmp146;
_tmp14B->tl=0;_tmp14B;}),_tmp14A.may_consume);if(!((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp14A.consumed,_tmp146)){struct
Cyc_CfFlowInfo_Place _tmp14D;void*_tmp14E;struct Cyc_CfFlowInfo_Place*_tmp14C=
_tmp146;_tmp14D=*_tmp14C;_tmp14E=(void*)_tmp14D.root;{void*_tmp14F=_tmp14E;
struct Cyc_Absyn_Vardecl*_tmp150;_LL10C: if(*((int*)_tmp14F)!= 0)goto _LL10E;
_tmp150=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp14F)->f1;_LL10D: {struct
_dynforward_ptr _tmp151=Cyc_Absynpp_qvar2string(_tmp150->name);({struct Cyc_String_pa_struct
_tmp154;_tmp154.tag=0;_tmp154.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp151);{void*_tmp152[1]={& _tmp154};Cyc_Tcutil_warn(_tmpDF->loc,({const char*
_tmp153="may clobber unique pointer %s";_tag_dynforward(_tmp153,sizeof(char),
_get_zero_arr_size(_tmp153,30));}),_tag_dynforward(_tmp152,sizeof(void*),1));}});
goto _LL10B;}_LL10E:;_LL10F:({void*_tmp155[0]={};((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp156="warning locations not for VarRoots";
_tag_dynforward(_tmp156,sizeof(char),_get_zero_arr_size(_tmp156,35));}),
_tag_dynforward(_tmp155,sizeof(void*),0));});_LL10B:;}}if(env->all_changed == 0){
struct Cyc_Dict_Dict _tmp157=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct
Cyc_CfFlowInfo_Place*))Cyc_Dict_delete)(_tmp14A.consumed,_tmp146);if((_tmp14A.consumed).t
!= _tmp157.t);_tmp14A.consumed=_tmp157;}}_tmp140=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp158;(_tmp158.ReachableFL).tag=1;(_tmp158.ReachableFL).f1=
_tmp148;(_tmp158.ReachableFL).f2=_tmp149;(_tmp158.ReachableFL).f3=_tmp14A;
_tmp158;});goto _LL106;_LL109:;_LL10A: goto _LL106;_LL106:;}return({struct _tuple5
_tmp159;_tmp159.f1=_tmp140;_tmp159.f2=Cyc_CfFlowInfo_unknown_all;_tmp159;});}}
_LLC4: if(*((int*)_tmpC9)!= 4)goto _LLC6;_tmpE3=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpC9)->f1;_tmpE4=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpC9)->f2;if(_tmpE4 != 
0)goto _LLC6;_tmpE5=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpC9)->f3;_LLC5: {
struct Cyc_Dict_Dict*_tmp15C=env->all_changed;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp15E;union Cyc_CfFlowInfo_FlowInfo_union _tmp15F;struct _tuple6 _tmp15D=Cyc_CfFlowInfo_save_consume_info(
inflow,1);_tmp15E=_tmp15D.f1;_tmp15F=_tmp15D.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
empty_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp188;_tmp188.consumed=Cyc_CfFlowInfo_mt_place_set();
_tmp188.may_consume=0;_tmp188;});struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=
empty_consume;int init_consume=0;while(1){env->all_changed=({struct Cyc_Dict_Dict*
_tmp160=_cycalloc(sizeof(*_tmp160));_tmp160[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp160;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp162;void*_tmp163;struct _tuple5
_tmp161=Cyc_NewControlFlow_anal_Lexp(env,_tmp15F,_tmpE3);_tmp162=_tmp161.f1;
_tmp163=_tmp161.f2;{struct Cyc_Dict_Dict _tmp164=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));env->all_changed=({struct Cyc_Dict_Dict*_tmp165=
_cycalloc(sizeof(*_tmp165));_tmp165[0]=Cyc_CfFlowInfo_mt_place_set();_tmp165;});{
union Cyc_CfFlowInfo_FlowInfo_union _tmp167;void*_tmp168;struct _tuple5 _tmp166=Cyc_NewControlFlow_anal_Rexp(
env,_tmp15F,_tmpE5);_tmp167=_tmp166.f1;_tmp168=_tmp166.f2;{struct Cyc_Dict_Dict
_tmp169=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo_union
_tmp16A=Cyc_CfFlowInfo_after_flow((struct Cyc_Dict_Dict*)& _tmp164,_tmp162,_tmp167,
_tmp164,_tmp169);union Cyc_CfFlowInfo_FlowInfo_union _tmp16B=Cyc_CfFlowInfo_join_flow(
_tmp15C,_tmp15F,_tmp16A,1);struct Cyc_CfFlowInfo_ConsumeInfo _tmp16D;struct _tuple6
_tmp16C=Cyc_CfFlowInfo_save_consume_info(_tmp16B,0);_tmp16D=_tmp16C.f1;if(
init_consume)_tmp15F=Cyc_CfFlowInfo_restore_consume_info(_tmp15F,outflow_consume);
outflow_consume=_tmp16D;init_consume=1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp16B,_tmp15F)){if(_tmp15C == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp15C,Cyc_CfFlowInfo_union_place_set(
_tmp164,_tmp169,0),0);}_tmp16A=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp16A);{union Cyc_CfFlowInfo_FlowInfo_union _tmp16E=_tmp16A;struct Cyc_Dict_Dict
_tmp16F;struct Cyc_List_List*_tmp170;struct Cyc_CfFlowInfo_ConsumeInfo _tmp171;
_LL111: if((_tmp16E.BottomFL).tag != 0)goto _LL113;_LL112: return({struct _tuple5
_tmp172;_tmp172.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp173;(_tmp173.BottomFL).tag=0;_tmp173;});_tmp172.f2=_tmp168;_tmp172;});_LL113:
if((_tmp16E.ReachableFL).tag != 1)goto _LL110;_tmp16F=(_tmp16E.ReachableFL).f1;
_tmp170=(_tmp16E.ReachableFL).f2;_tmp171=(_tmp16E.ReachableFL).f3;_LL114: _tmp171=
Cyc_CfFlowInfo_and_consume(_tmp15E,_tmp171);{void*_tmp174=_tmp163;struct Cyc_CfFlowInfo_Place*
_tmp175;_LL116: if(_tmp174 <= (void*)1)goto _LL118;if(*((int*)_tmp174)!= 0)goto
_LL118;_tmp175=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp174)->f1;_LL117: _tmp16F=
Cyc_CfFlowInfo_assign_place(e->loc,_tmp16F,env->all_changed,_tmp175,_tmp168);if(
Cyc_CfFlowInfo_contains_region((void*)0,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmpE3->topt))->v)){_tmp171.may_consume=((struct Cyc_List_List*(*)(int(*cmp)(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_merge)(Cyc_CfFlowInfo_place_cmp,({struct Cyc_List_List*
_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->hd=_tmp175;_tmp176->tl=0;_tmp176;}),
_tmp171.may_consume);if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
k))Cyc_Dict_member)(_tmp171.consumed,_tmp175)){struct Cyc_CfFlowInfo_Place _tmp178;
void*_tmp179;struct Cyc_CfFlowInfo_Place*_tmp177=_tmp175;_tmp178=*_tmp177;_tmp179=(
void*)_tmp178.root;{void*_tmp17A=_tmp179;struct Cyc_Absyn_Vardecl*_tmp17B;_LL11B:
if(*((int*)_tmp17A)!= 0)goto _LL11D;_tmp17B=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp17A)->f1;_LL11C: {struct _dynforward_ptr _tmp17C=Cyc_Absynpp_qvar2string(
_tmp17B->name);({struct Cyc_String_pa_struct _tmp17F;_tmp17F.tag=0;_tmp17F.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp17C);{void*_tmp17D[1]={&
_tmp17F};Cyc_Tcutil_warn(_tmpE3->loc,({const char*_tmp17E="may clobber unique pointer %s";
_tag_dynforward(_tmp17E,sizeof(char),_get_zero_arr_size(_tmp17E,30));}),
_tag_dynforward(_tmp17D,sizeof(void*),1));}});goto _LL11A;}_LL11D:;_LL11E:({void*
_tmp180[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp181="warning locations not for VarRoots";_tag_dynforward(_tmp181,
sizeof(char),_get_zero_arr_size(_tmp181,35));}),_tag_dynforward(_tmp180,sizeof(
void*),0));});_LL11A:;}}if(env->all_changed == 0){struct Cyc_Dict_Dict _tmp182=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))Cyc_Dict_delete)(
_tmp171.consumed,_tmp175);if((_tmp171.consumed).t != _tmp182.t);_tmp171.consumed=
_tmp182;}}_tmp170=Cyc_CfFlowInfo_reln_assign_exp(_tmp170,_tmpE3,_tmpE5);_tmp16A=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp183;(
_tmp183.ReachableFL).tag=1;(_tmp183.ReachableFL).f1=_tmp16F;(_tmp183.ReachableFL).f2=
_tmp170;(_tmp183.ReachableFL).f3=_tmp171;_tmp183;});Cyc_NewControlFlow_update_tryflow(
env,_tmp16A);return({struct _tuple5 _tmp184;_tmp184.f1=_tmp16A;_tmp184.f2=_tmp168;
_tmp184;});_LL118: if((int)_tmp174 != 0)goto _LL115;_LL119: if(Cyc_CfFlowInfo_contains_region((
void*)0,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpE3->topt))->v))({void*
_tmp185[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp186="dest of unique assignment is unknown!";_tag_dynforward(
_tmp186,sizeof(char),_get_zero_arr_size(_tmp186,38));}),_tag_dynforward(_tmp185,
sizeof(void*),0));});return({struct _tuple5 _tmp187;_tmp187.f1=Cyc_NewControlFlow_use_Rval(
env,_tmpE5->loc,_tmp16A,_tmp168);_tmp187.f2=_tmp168;_tmp187;});_LL115:;}_LL110:;}}
_tmp15F=Cyc_CfFlowInfo_restore_consume_info(_tmp16B,empty_consume);}}}}}}}_LLC6:
if(*((int*)_tmpC9)!= 9)goto _LLC8;_tmpE6=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpC9)->f1;_tmpE7=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpC9)->f2;_LLC7: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp18A;void*_tmp18B;struct _tuple5 _tmp189=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpE6);_tmp18A=_tmp189.f1;_tmp18B=_tmp189.f2;_tmp18A=Cyc_CfFlowInfo_drop_unique_rvals(
_tmpE6->loc,_tmp18A);return Cyc_NewControlFlow_anal_Rexp(env,_tmp18A,_tmpE7);}
_LLC8: if(*((int*)_tmpC9)!= 12)goto _LLCA;_tmpE8=((struct Cyc_Absyn_Throw_e_struct*)
_tmpC9)->f1;_LLC9: {union Cyc_CfFlowInfo_FlowInfo_union _tmp18D;void*_tmp18E;
struct _tuple5 _tmp18C=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpE8);_tmp18D=
_tmp18C.f1;_tmp18E=_tmp18C.f2;_tmp18D=Cyc_CfFlowInfo_consume_unique_rvals(_tmpE8->loc,
_tmp18D);Cyc_NewControlFlow_use_Rval(env,_tmpE8->loc,_tmp18D,_tmp18E);return({
struct _tuple5 _tmp18F;_tmp18F.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp190;(_tmp190.BottomFL).tag=0;_tmp190;});_tmp18F.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp18F;});}_LLCA: if(*((int*)_tmpC9)!= 11)goto _LLCC;_tmpE9=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpC9)->f1;_tmpEA=((struct Cyc_Absyn_FnCall_e_struct*)_tmpC9)->f2;_LLCB: {struct
_RegionHandle _tmp191=_new_region("temp");struct _RegionHandle*temp=& _tmp191;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp194;struct Cyc_List_List*
_tmp195;struct _tuple9 _tmp193=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_List_List*_tmp192=_region_malloc(temp,sizeof(*_tmp192));
_tmp192->hd=_tmpE9;_tmp192->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmpEA);_tmp192;}),1);_tmp194=
_tmp193.f1;_tmp195=_tmp193.f2;_tmp194=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp194);{union Cyc_CfFlowInfo_FlowInfo_union _tmp196=Cyc_NewControlFlow_use_Rval(
env,_tmpE9->loc,_tmp194,(void*)((struct Cyc_List_List*)_check_null(_tmp195))->hd);
_tmp195=_tmp195->tl;{struct Cyc_List_List*init_params=0;{void*_tmp197=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpE9->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp198;void*_tmp199;_LL120: if(_tmp197 <= (void*)4)goto _LL122;if(*((int*)_tmp197)
!= 4)goto _LL122;_tmp198=((struct Cyc_Absyn_PointerType_struct*)_tmp197)->f1;
_tmp199=(void*)_tmp198.elt_typ;_LL121:{void*_tmp19A=Cyc_Tcutil_compress(_tmp199);
struct Cyc_Absyn_FnInfo _tmp19B;struct Cyc_List_List*_tmp19C;_LL125: if(_tmp19A <= (
void*)4)goto _LL127;if(*((int*)_tmp19A)!= 8)goto _LL127;_tmp19B=((struct Cyc_Absyn_FnType_struct*)
_tmp19A)->f1;_tmp19C=_tmp19B.attributes;_LL126: for(0;_tmp19C != 0;_tmp19C=_tmp19C->tl){
void*_tmp19D=(void*)_tmp19C->hd;int _tmp19E;_LL12A: if(_tmp19D <= (void*)17)goto
_LL12C;if(*((int*)_tmp19D)!= 4)goto _LL12C;_tmp19E=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp19D)->f1;_LL12B: init_params=({struct Cyc_List_List*_tmp19F=_region_malloc(
temp,sizeof(*_tmp19F));_tmp19F->hd=(void*)_tmp19E;_tmp19F->tl=init_params;
_tmp19F;});goto _LL129;_LL12C:;_LL12D: goto _LL129;_LL129:;}goto _LL124;_LL127:;
_LL128:({void*_tmp1A0[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1A1="anal_Rexp: bad function type";
_tag_dynforward(_tmp1A1,sizeof(char),_get_zero_arr_size(_tmp1A1,29));}),
_tag_dynforward(_tmp1A0,sizeof(void*),0));});_LL124:;}goto _LL11F;_LL122:;_LL123:({
void*_tmp1A2[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1A3="anal_Rexp: bad function type";
_tag_dynforward(_tmp1A3,sizeof(char),_get_zero_arr_size(_tmp1A3,29));}),
_tag_dynforward(_tmp1A2,sizeof(void*),0));});_LL11F:;}{int i=1;for(0;_tmp195 != 0;(((
_tmp195=_tmp195->tl,_tmpEA=((struct Cyc_List_List*)_check_null(_tmpEA))->tl)),++ i)){
if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){_tmp196=
Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpEA))->hd)->loc,_tmp196,(void*)_tmp195->hd);continue;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1A4=_tmp194;struct Cyc_Dict_Dict _tmp1A5;_LL12F: if((_tmp1A4.BottomFL).tag != 0)
goto _LL131;_LL130: goto _LL12E;_LL131: if((_tmp1A4.ReachableFL).tag != 1)goto _LL12E;
_tmp1A5=(_tmp1A4.ReachableFL).f1;_LL132: if(Cyc_CfFlowInfo_initlevel(_tmp1A5,(
void*)_tmp195->hd)== (void*)0)({void*_tmp1A6[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpEA))->hd)->loc,({const char*_tmp1A7="expression may not be initialized";
_tag_dynforward(_tmp1A7,sizeof(char),_get_zero_arr_size(_tmp1A7,34));}),
_tag_dynforward(_tmp1A6,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1A8=_tmp196;struct Cyc_Dict_Dict _tmp1A9;struct Cyc_List_List*_tmp1AA;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1AB;_LL134: if((_tmp1A8.BottomFL).tag != 0)goto _LL136;_LL135: goto _LL133;_LL136:
if((_tmp1A8.ReachableFL).tag != 1)goto _LL133;_tmp1A9=(_tmp1A8.ReachableFL).f1;
_tmp1AA=(_tmp1A8.ReachableFL).f2;_tmp1AB=(_tmp1A8.ReachableFL).f3;_LL137: {
struct Cyc_Dict_Dict _tmp1AC=Cyc_CfFlowInfo_escape_deref(_tmp1A9,env->all_changed,(
void*)_tmp195->hd);{void*_tmp1AD=(void*)_tmp195->hd;struct Cyc_CfFlowInfo_Place*
_tmp1AE;_LL139: if(_tmp1AD <= (void*)3)goto _LL13B;if(*((int*)_tmp1AD)!= 2)goto
_LL13B;_tmp1AE=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1AD)->f1;_LL13A:{
void*_tmp1AF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpEA))->hd)->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1B0;void*_tmp1B1;_LL13E: if(_tmp1AF <= (void*)4)goto
_LL140;if(*((int*)_tmp1AF)!= 4)goto _LL140;_tmp1B0=((struct Cyc_Absyn_PointerType_struct*)
_tmp1AF)->f1;_tmp1B1=(void*)_tmp1B0.elt_typ;_LL13F: _tmp1AC=Cyc_CfFlowInfo_assign_place(((
struct Cyc_Absyn_Exp*)_tmpEA->hd)->loc,_tmp1AC,env->all_changed,_tmp1AE,Cyc_CfFlowInfo_typ_to_absrval(
_tmp1B1,Cyc_CfFlowInfo_esc_all));goto _LL13D;_LL140:;_LL141:({void*_tmp1B2[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1B3="anal_Rexp:bad type for initialized arg";_tag_dynforward(
_tmp1B3,sizeof(char),_get_zero_arr_size(_tmp1B3,39));}),_tag_dynforward(_tmp1B2,
sizeof(void*),0));});_LL13D:;}goto _LL138;_LL13B:;_LL13C: goto _LL138;_LL138:;}
_tmp196=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1B4;(_tmp1B4.ReachableFL).tag=1;(_tmp1B4.ReachableFL).f1=_tmp1AC;(_tmp1B4.ReachableFL).f2=
_tmp1AA;(_tmp1B4.ReachableFL).f3=_tmp1AB;_tmp1B4;});goto _LL133;}_LL133:;}goto
_LL12E;_LL12E:;}}}if(Cyc_Tcutil_is_noreturn((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpE9->topt))->v)){struct _tuple5 _tmp1B7=({struct _tuple5 _tmp1B5;
_tmp1B5.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1B6;(_tmp1B6.BottomFL).tag=0;_tmp1B6;});_tmp1B5.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp1B5;});_npop_handler(0);return _tmp1B7;}else{struct _tuple5 _tmp1B9=({struct
_tuple5 _tmp1B8;_tmp1B8.f1=_tmp196;_tmp1B8.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp1B8;});
_npop_handler(0);return _tmp1B9;}}}};_pop_region(temp);}_LLCC: if(*((int*)_tmpC9)
!= 35)goto _LLCE;_tmpEB=((struct Cyc_Absyn_Malloc_e_struct*)_tmpC9)->f1;_tmpEC=
_tmpEB.is_calloc;_tmpED=_tmpEB.rgn;_tmpEE=_tmpEB.elt_type;_tmpEF=_tmpEB.num_elts;
_tmpF0=_tmpEB.fat_result;_LLCD: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp1CC;_tmp1CC.tag=1;_tmp1CC.f1=_tmpEF;_tmp1CC.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp1CC;});_tmp1CB;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->root=(
void*)root;_tmp1CA->fields=0;_tmp1CA;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1C8=_cycalloc(sizeof(*_tmp1C8));_tmp1C8[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp1C9;_tmp1C9.tag=2;_tmp1C9.f1=place;_tmp1C9;});_tmp1C8;});void*place_val=
_tmpF0?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(*((void**)_check_null(_tmpEE)),Cyc_CfFlowInfo_unknown_none);
union Cyc_CfFlowInfo_FlowInfo_union outflow;Cyc_CfFlowInfo_update_place_set(env->all_changed,
place,0);if(_tmpED != 0){struct _RegionHandle _tmp1BA=_new_region("temp");struct
_RegionHandle*temp=& _tmp1BA;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1BD;struct Cyc_List_List*_tmp1BE;struct _tuple9 _tmp1BC=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp1BB[2];_tmp1BB[1]=_tmpEF;_tmp1BB[0]=(
struct Cyc_Absyn_Exp*)_tmpED;((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(_tmp1BB,sizeof(
struct Cyc_Absyn_Exp*),2));}),0);_tmp1BD=_tmp1BC.f1;_tmp1BE=_tmp1BC.f2;outflow=
_tmp1BD;};_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo_union _tmp1C0;
struct _tuple5 _tmp1BF=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpEF);_tmp1C0=
_tmp1BF.f1;outflow=_tmp1C0;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpEF->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1C1=outflow;struct
Cyc_Dict_Dict _tmp1C2;struct Cyc_List_List*_tmp1C3;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1C4;_LL143: if((_tmp1C1.BottomFL).tag != 0)goto _LL145;_LL144: return({struct
_tuple5 _tmp1C5;_tmp1C5.f1=outflow;_tmp1C5.f2=rval;_tmp1C5;});_LL145: if((_tmp1C1.ReachableFL).tag
!= 1)goto _LL142;_tmp1C2=(_tmp1C1.ReachableFL).f1;_tmp1C3=(_tmp1C1.ReachableFL).f2;
_tmp1C4=(_tmp1C1.ReachableFL).f3;_LL146: return({struct _tuple5 _tmp1C6;_tmp1C6.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp1C7;(
_tmp1C7.ReachableFL).tag=1;(_tmp1C7.ReachableFL).f1=Cyc_Dict_insert(_tmp1C2,root,
place_val);(_tmp1C7.ReachableFL).f2=_tmp1C3;(_tmp1C7.ReachableFL).f3=_tmp1C4;
_tmp1C7;});_tmp1C6.f2=rval;_tmp1C6;});_LL142:;}}_LLCE: if(*((int*)_tmpC9)!= 17)
goto _LLD0;_tmpF1=((struct Cyc_Absyn_New_e_struct*)_tmpC9)->f1;_tmpF2=((struct Cyc_Absyn_New_e_struct*)
_tmpC9)->f2;_LLCF: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp1DF=_cycalloc(sizeof(*_tmp1DF));_tmp1DF[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp1E0;_tmp1E0.tag=1;_tmp1E0.f1=_tmpF2;_tmp1E0.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp1E0;});_tmp1DF;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE->root=(
void*)root;_tmp1DE->fields=0;_tmp1DE;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp1DD;_tmp1DD.tag=2;_tmp1DD.f1=place;_tmp1DD;});_tmp1DC;});Cyc_CfFlowInfo_update_place_set(
env->all_changed,place,0);{union Cyc_CfFlowInfo_FlowInfo_union outflow;void*
place_val;if(_tmpF1 != 0){struct _RegionHandle _tmp1CD=_new_region("temp");struct
_RegionHandle*temp=& _tmp1CD;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1D0;struct Cyc_List_List*_tmp1D1;struct _tuple9 _tmp1CF=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp1CE[2];_tmp1CE[1]=_tmpF2;_tmp1CE[0]=(
struct Cyc_Absyn_Exp*)_tmpF1;((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(_tmp1CE,sizeof(
struct Cyc_Absyn_Exp*),2));}),0);_tmp1D0=_tmp1CF.f1;_tmp1D1=_tmp1CF.f2;outflow=
_tmp1D0;place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp1D1))->tl))->hd;};_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1D3;void*_tmp1D4;struct _tuple5 _tmp1D2=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpF2);_tmp1D3=_tmp1D2.f1;_tmp1D4=_tmp1D2.f2;outflow=_tmp1D3;place_val=
_tmp1D4;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpF2->loc,outflow);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp1D5=outflow;struct Cyc_Dict_Dict _tmp1D6;
struct Cyc_List_List*_tmp1D7;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1D8;_LL148: if((
_tmp1D5.BottomFL).tag != 0)goto _LL14A;_LL149: return({struct _tuple5 _tmp1D9;_tmp1D9.f1=
outflow;_tmp1D9.f2=rval;_tmp1D9;});_LL14A: if((_tmp1D5.ReachableFL).tag != 1)goto
_LL147;_tmp1D6=(_tmp1D5.ReachableFL).f1;_tmp1D7=(_tmp1D5.ReachableFL).f2;_tmp1D8=(
_tmp1D5.ReachableFL).f3;_LL14B: return({struct _tuple5 _tmp1DA;_tmp1DA.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp1DB;(_tmp1DB.ReachableFL).tag=1;(_tmp1DB.ReachableFL).f1=
Cyc_Dict_insert(_tmp1D6,root,place_val);(_tmp1DB.ReachableFL).f2=_tmp1D7;(
_tmp1DB.ReachableFL).f3=_tmp1D8;_tmp1DB;});_tmp1DA.f2=rval;_tmp1DA;});_LL147:;}}}
_LLD0: if(*((int*)_tmpC9)!= 16)goto _LLD2;_tmpF3=((struct Cyc_Absyn_Address_e_struct*)
_tmpC9)->f1;_LLD1: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1E2;void*_tmp1E3;
struct _tuple5 _tmp1E1=Cyc_NewControlFlow_anal_Lexp(env,inflow,_tmpF3);_tmp1E2=
_tmp1E1.f1;_tmp1E3=_tmp1E1.f2;{void*_tmp1E4=_tmp1E3;struct Cyc_CfFlowInfo_Place*
_tmp1E5;_LL14D: if((int)_tmp1E4 != 0)goto _LL14F;_LL14E: return({struct _tuple5
_tmp1E6;_tmp1E6.f1=_tmp1E2;_tmp1E6.f2=(void*)1;_tmp1E6;});_LL14F: if(_tmp1E4 <= (
void*)1)goto _LL14C;if(*((int*)_tmp1E4)!= 0)goto _LL14C;_tmp1E5=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp1E4)->f1;_LL150: return({struct _tuple5 _tmp1E7;_tmp1E7.f1=_tmp1E2;_tmp1E7.f2=(
void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));
_tmp1E8[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp1E9;_tmp1E9.tag=2;_tmp1E9.f1=
_tmp1E5;_tmp1E9;});_tmp1E8;});_tmp1E7;});_LL14C:;}}_LLD2: if(*((int*)_tmpC9)!= 22)
goto _LLD4;_tmpF4=((struct Cyc_Absyn_Deref_e_struct*)_tmpC9)->f1;_LLD3: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp1EB;void*_tmp1EC;struct _tuple5 _tmp1EA=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpF4);_tmp1EB=_tmp1EA.f1;_tmp1EC=_tmp1EA.f2;_tmp1EB=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp1EB);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1EB,_tmpF4,
_tmp1EC);}_LLD4: if(*((int*)_tmpC9)!= 23)goto _LLD6;_tmpF5=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpC9)->f1;_tmpF6=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpC9)->f2;_LLD5: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp1EE;void*_tmp1EF;struct _tuple5 _tmp1ED=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpF5);_tmp1EE=_tmp1ED.f1;_tmp1EF=_tmp1ED.f2;if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF5->topt))->v))return({struct _tuple5
_tmp1F0;_tmp1F0.f1=_tmp1EE;_tmp1F0.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp1F0;});{
void*_tmp1F1=_tmp1EF;struct _dynforward_ptr _tmp1F2;_LL152: if(_tmp1F1 <= (void*)3)
goto _LL154;if(*((int*)_tmp1F1)!= 4)goto _LL154;_tmp1F2=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1F1)->f1;_LL153: {int _tmp1F3=Cyc_NewControlFlow_get_field_index((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpF5->topt))->v,_tmpF6);return({struct _tuple5
_tmp1F4;_tmp1F4.f1=_tmp1EE;_tmp1F4.f2=*((void**)_check_dynforward_subscript(
_tmp1F2,sizeof(void*),_tmp1F3));_tmp1F4;});}_LL154:;_LL155:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5[0]=({
struct Cyc_Core_Impossible_struct _tmp1F6;_tmp1F6.tag=Cyc_Core_Impossible;_tmp1F6.f1=(
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp1F9;_tmp1F9.tag=0;_tmp1F9.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{void*
_tmp1F7[1]={& _tmp1F9};Cyc_aprintf(({const char*_tmp1F8="anal_Rexp: AggrMember: %s";
_tag_dynforward(_tmp1F8,sizeof(char),_get_zero_arr_size(_tmp1F8,26));}),
_tag_dynforward(_tmp1F7,sizeof(void*),1));}});_tmp1F6;});_tmp1F5;}));_LL151:;}}
_LLD6: if(*((int*)_tmpC9)!= 24)goto _LLD8;_tmpF7=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpC9)->f1;_tmpF8=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpC9)->f2;_LLD7: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp1FB;void*_tmp1FC;struct _tuple5 _tmp1FA=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpF7);_tmp1FB=_tmp1FA.f1;_tmp1FC=_tmp1FA.f2;_tmp1FB=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp1FB);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1FE;void*_tmp1FF;struct
_tuple5 _tmp1FD=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1FB,_tmpF7,_tmp1FC);
_tmp1FE=_tmp1FD.f1;_tmp1FF=_tmp1FD.f2;{void*_tmp200=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpF7->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp201;void*_tmp202;_LL157: if(_tmp200 <= (void*)4)goto _LL159;if(*((int*)_tmp200)
!= 4)goto _LL159;_tmp201=((struct Cyc_Absyn_PointerType_struct*)_tmp200)->f1;
_tmp202=(void*)_tmp201.elt_typ;_LL158: if(Cyc_Absyn_is_union_type(_tmp202))return({
struct _tuple5 _tmp203;_tmp203.f1=_tmp1FE;_tmp203.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp203;});{void*_tmp204=_tmp1FF;struct _dynforward_ptr _tmp205;_LL15C: if(_tmp204
<= (void*)3)goto _LL15E;if(*((int*)_tmp204)!= 4)goto _LL15E;_tmp205=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp204)->f1;_LL15D: {int _tmp206=Cyc_NewControlFlow_get_field_index(_tmp202,
_tmpF8);return({struct _tuple5 _tmp207;_tmp207.f1=_tmp1FE;_tmp207.f2=*((void**)
_check_dynforward_subscript(_tmp205,sizeof(void*),_tmp206));_tmp207;});}_LL15E:;
_LL15F:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp208=_cycalloc(
sizeof(*_tmp208));_tmp208[0]=({struct Cyc_Core_Impossible_struct _tmp209;_tmp209.tag=
Cyc_Core_Impossible;_tmp209.f1=({const char*_tmp20A="anal_Rexp: AggrArrow";
_tag_dynforward(_tmp20A,sizeof(char),_get_zero_arr_size(_tmp20A,21));});_tmp209;});
_tmp208;}));_LL15B:;}_LL159:;_LL15A:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp20B=_cycalloc(sizeof(*_tmp20B));_tmp20B[0]=({struct Cyc_Core_Impossible_struct
_tmp20C;_tmp20C.tag=Cyc_Core_Impossible;_tmp20C.f1=({const char*_tmp20D="anal_Rexp: AggrArrow ptr";
_tag_dynforward(_tmp20D,sizeof(char),_get_zero_arr_size(_tmp20D,25));});_tmp20C;});
_tmp20B;}));_LL156:;}}}_LLD8: if(*((int*)_tmpC9)!= 6)goto _LLDA;_tmpF9=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpC9)->f1;_tmpFA=((struct Cyc_Absyn_Conditional_e_struct*)_tmpC9)->f2;_tmpFB=((
struct Cyc_Absyn_Conditional_e_struct*)_tmpC9)->f3;_LLD9: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp20F;union Cyc_CfFlowInfo_FlowInfo_union _tmp210;struct _tuple7 _tmp20E=Cyc_NewControlFlow_anal_test(
env,inflow,_tmpF9);_tmp20F=_tmp20E.f1;_tmp210=_tmp20E.f2;_tmp20F=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpF9->loc,_tmp20F);_tmp210=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpF9->loc,
_tmp210);{struct _tuple5 _tmp211=Cyc_NewControlFlow_anal_Rexp(env,_tmp20F,_tmpFA);
struct _tuple5 _tmp212=Cyc_NewControlFlow_anal_Rexp(env,_tmp210,_tmpFB);return Cyc_CfFlowInfo_join_flow_and_rval(
env->all_changed,_tmp211,_tmp212,1);}}_LLDA: if(*((int*)_tmpC9)!= 7)goto _LLDC;
_tmpFC=((struct Cyc_Absyn_And_e_struct*)_tmpC9)->f1;_tmpFD=((struct Cyc_Absyn_And_e_struct*)
_tmpC9)->f2;_LLDB: {union Cyc_CfFlowInfo_FlowInfo_union _tmp214;union Cyc_CfFlowInfo_FlowInfo_union
_tmp215;struct _tuple7 _tmp213=Cyc_NewControlFlow_anal_test(env,inflow,_tmpFC);
_tmp214=_tmp213.f1;_tmp215=_tmp213.f2;_tmp214=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpFC->loc,_tmp214);_tmp215=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpFC->loc,
_tmp215);{union Cyc_CfFlowInfo_FlowInfo_union _tmp217;void*_tmp218;struct _tuple5
_tmp216=Cyc_NewControlFlow_anal_Rexp(env,_tmp214,_tmpFD);_tmp217=_tmp216.f1;
_tmp218=_tmp216.f2;_tmp217=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpFD->loc,
_tmp217);{struct _tuple5 _tmp219=({struct _tuple5 _tmp21C;_tmp21C.f1=_tmp217;_tmp21C.f2=
_tmp218;_tmp21C;});struct _tuple5 _tmp21A=({struct _tuple5 _tmp21B;_tmp21B.f1=
_tmp215;_tmp21B.f2=(void*)((void*)0);_tmp21B;});return Cyc_CfFlowInfo_join_flow_and_rval(
env->all_changed,_tmp219,_tmp21A,0);}}}_LLDC: if(*((int*)_tmpC9)!= 8)goto _LLDE;
_tmpFE=((struct Cyc_Absyn_Or_e_struct*)_tmpC9)->f1;_tmpFF=((struct Cyc_Absyn_Or_e_struct*)
_tmpC9)->f2;_LLDD: {union Cyc_CfFlowInfo_FlowInfo_union _tmp21E;union Cyc_CfFlowInfo_FlowInfo_union
_tmp21F;struct _tuple7 _tmp21D=Cyc_NewControlFlow_anal_test(env,inflow,_tmpFE);
_tmp21E=_tmp21D.f1;_tmp21F=_tmp21D.f2;_tmp21E=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpFE->loc,_tmp21E);_tmp21F=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpFE->loc,
_tmp21F);{union Cyc_CfFlowInfo_FlowInfo_union _tmp221;void*_tmp222;struct _tuple5
_tmp220=Cyc_NewControlFlow_anal_Rexp(env,_tmp21F,_tmpFF);_tmp221=_tmp220.f1;
_tmp222=_tmp220.f2;_tmp221=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpFF->loc,
_tmp221);{struct _tuple5 _tmp223=({struct _tuple5 _tmp226;_tmp226.f1=_tmp221;_tmp226.f2=
_tmp222;_tmp226;});struct _tuple5 _tmp224=({struct _tuple5 _tmp225;_tmp225.f1=
_tmp21E;_tmp225.f2=(void*)((void*)1);_tmp225;});return Cyc_CfFlowInfo_join_flow_and_rval(
env->all_changed,_tmp223,_tmp224,0);}}}_LLDE: if(*((int*)_tmpC9)!= 25)goto _LLE0;
_tmp100=((struct Cyc_Absyn_Subscript_e_struct*)_tmpC9)->f1;_tmp101=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpC9)->f2;_LLDF: {struct _RegionHandle _tmp227=_new_region("temp");struct
_RegionHandle*temp=& _tmp227;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp22A;struct Cyc_List_List*_tmp22B;struct _tuple9 _tmp229=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp228[2];_tmp228[1]=_tmp101;_tmp228[0]=
_tmp100;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))
Cyc_List_rlist)(temp,_tag_dynforward(_tmp228,sizeof(struct Cyc_Absyn_Exp*),2));}),
1);_tmp22A=_tmp229.f1;_tmp22B=_tmp229.f2;_tmp22A=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp101->loc,_tmp22A);{union Cyc_CfFlowInfo_FlowInfo_union _tmp22C=_tmp22A;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp22D=_tmp22A;struct Cyc_Dict_Dict _tmp22E;struct
Cyc_List_List*_tmp22F;struct Cyc_CfFlowInfo_ConsumeInfo _tmp230;_LL161: if((_tmp22D.ReachableFL).tag
!= 1)goto _LL163;_tmp22E=(_tmp22D.ReachableFL).f1;_tmp22F=(_tmp22D.ReachableFL).f2;
_tmp230=(_tmp22D.ReachableFL).f3;_LL162: if(Cyc_CfFlowInfo_initlevel(_tmp22E,(
void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(
_tmp22B))->tl))->hd)== (void*)0)({void*_tmp231[0]={};Cyc_Tcutil_terr(_tmp101->loc,({
const char*_tmp232="expression may not be initialized";_tag_dynforward(_tmp232,
sizeof(char),_get_zero_arr_size(_tmp232,34));}),_tag_dynforward(_tmp231,sizeof(
void*),0));});{struct Cyc_List_List*_tmp233=Cyc_NewControlFlow_add_subscript_reln(
_tmp22F,_tmp100,_tmp101);if(_tmp22F != _tmp233)_tmp22C=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp234;(_tmp234.ReachableFL).tag=1;(_tmp234.ReachableFL).f1=
_tmp22E;(_tmp234.ReachableFL).f2=_tmp233;(_tmp234.ReachableFL).f3=_tmp230;
_tmp234;});goto _LL160;}_LL163:;_LL164: goto _LL160;_LL160:;}{void*_tmp235=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp100->topt))->v);struct Cyc_List_List*
_tmp236;struct Cyc_Absyn_PtrInfo _tmp237;struct Cyc_Absyn_PtrAtts _tmp238;struct Cyc_Absyn_Conref*
_tmp239;_LL166: if(_tmp235 <= (void*)4)goto _LL16A;if(*((int*)_tmp235)!= 9)goto
_LL168;_tmp236=((struct Cyc_Absyn_TupleType_struct*)_tmp235)->f1;_LL167: {void*
_tmp23A=(void*)((struct Cyc_List_List*)_check_null(_tmp22B))->hd;struct
_dynforward_ptr _tmp23B;_LL16D: if(_tmp23A <= (void*)3)goto _LL16F;if(*((int*)
_tmp23A)!= 4)goto _LL16F;_tmp23B=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp23A)->f1;
_LL16E: {unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp101)).f1;struct _tuple5
_tmp23D=({struct _tuple5 _tmp23C;_tmp23C.f1=_tmp22C;_tmp23C.f2=*((void**)
_check_dynforward_subscript(_tmp23B,sizeof(void*),(int)i));_tmp23C;});
_npop_handler(0);return _tmp23D;}_LL16F:;_LL170:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp23E=_cycalloc(sizeof(*_tmp23E));_tmp23E[0]=({struct Cyc_Core_Impossible_struct
_tmp23F;_tmp23F.tag=Cyc_Core_Impossible;_tmp23F.f1=({const char*_tmp240="anal_Rexp: Subscript";
_tag_dynforward(_tmp240,sizeof(char),_get_zero_arr_size(_tmp240,21));});_tmp23F;});
_tmp23E;}));_LL16C:;}_LL168: if(*((int*)_tmp235)!= 4)goto _LL16A;_tmp237=((struct
Cyc_Absyn_PointerType_struct*)_tmp235)->f1;_tmp238=_tmp237.ptr_atts;_tmp239=
_tmp238.bounds;_LL169:{union Cyc_Absyn_Constraint_union _tmp241=(Cyc_Absyn_compress_conref(
_tmp239))->v;void*_tmp242;_LL172: if((_tmp241.Eq_constr).tag != 0)goto _LL174;
_tmp242=(_tmp241.Eq_constr).f1;if(_tmp242 <= (void*)2)goto _LL174;if(*((int*)
_tmp242)!= 1)goto _LL174;_LL173:{void*_tmp243=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp22B))->tl))->hd;struct Cyc_List_List*
_tmp244;_LL177: if(_tmp243 <= (void*)3)goto _LL179;if(*((int*)_tmp243)!= 3)goto
_LL179;_tmp244=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp243)->f1;_LL178:(void*)(
_tmp101->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp245=
_cycalloc(sizeof(*_tmp245));_tmp245[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp246;_tmp246.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp246.f1=_tmp244;_tmp246;});
_tmp245;})));goto _LL176;_LL179:;_LL17A: goto _LL176;_LL176:;}goto _LL171;_LL174:;
_LL175: goto _LL171;_LL171:;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp248;void*
_tmp249;struct _tuple5 _tmp247=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp22A,
_tmp100,(void*)((struct Cyc_List_List*)_check_null(_tmp22B))->hd);_tmp248=_tmp247.f1;
_tmp249=_tmp247.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp24A=_tmp248;_LL17C: if((
_tmp24A.BottomFL).tag != 0)goto _LL17E;_LL17D: {struct _tuple5 _tmp24C=({struct
_tuple5 _tmp24B;_tmp24B.f1=_tmp248;_tmp24B.f2=_tmp249;_tmp24B;});_npop_handler(0);
return _tmp24C;}_LL17E:;_LL17F: {struct _tuple5 _tmp24E=({struct _tuple5 _tmp24D;
_tmp24D.f1=_tmp22C;_tmp24D.f2=_tmp249;_tmp24D;});_npop_handler(0);return _tmp24E;}
_LL17B:;}}_LL16A:;_LL16B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp24F=_cycalloc(sizeof(*_tmp24F));_tmp24F[0]=({struct Cyc_Core_Impossible_struct
_tmp250;_tmp250.tag=Cyc_Core_Impossible;_tmp250.f1=({const char*_tmp251="anal_Rexp: Subscript -- bad type";
_tag_dynforward(_tmp251,sizeof(char),_get_zero_arr_size(_tmp251,33));});_tmp250;});
_tmp24F;}));_LL165:;}}};_pop_region(temp);}_LLE0: if(*((int*)_tmpC9)!= 32)goto
_LLE2;_tmp102=((struct Cyc_Absyn_Tunion_e_struct*)_tmpC9)->f1;_tmp103=((struct Cyc_Absyn_Tunion_e_struct*)
_tmpC9)->f2;_LLE1: if(_tmp103->is_flat){struct _RegionHandle _tmp252=_new_region("temp");
struct _RegionHandle*temp=& _tmp252;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp254;struct Cyc_List_List*_tmp255;struct _tuple9 _tmp253=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp102,0);_tmp254=_tmp253.f1;_tmp255=_tmp253.f2;_tmp254=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp254);for(0;(unsigned int)_tmp102;(_tmp102=_tmp102->tl,_tmp255=_tmp255->tl)){
_tmp254=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)_tmp102->hd)->loc,
_tmp254,(void*)((struct Cyc_List_List*)_check_null(_tmp255))->hd);}{struct _tuple5
_tmp257=({struct _tuple5 _tmp256;_tmp256.f1=_tmp254;_tmp256.f2=Cyc_CfFlowInfo_unknown_all;
_tmp256;});_npop_handler(0);return _tmp257;}};_pop_region(temp);}_tmp104=_tmp102;
goto _LLE3;_LLE2: if(*((int*)_tmpC9)!= 26)goto _LLE4;_tmp104=((struct Cyc_Absyn_Tuple_e_struct*)
_tmpC9)->f1;_LLE3: {struct _RegionHandle _tmp258=_new_region("temp");struct
_RegionHandle*temp=& _tmp258;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp25A;struct Cyc_List_List*_tmp25B;struct _tuple9 _tmp259=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp104,0);_tmp25A=_tmp259.f1;_tmp25B=_tmp259.f2;_tmp25A=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp25A);{struct _dynforward_ptr aggrdict=({unsigned int _tmp260=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp104);void**
_tmp261=(void**)_cycalloc(_check_times(sizeof(void*),_tmp260));struct
_dynforward_ptr _tmp264=_tag_dynforward(_tmp261,sizeof(void*),_tmp260);{
unsigned int _tmp262=_tmp260;unsigned int i;for(i=0;i < _tmp262;i ++){_tmp261[i]=({
void*_tmp263=(void*)((struct Cyc_List_List*)_check_null(_tmp25B))->hd;_tmp25B=
_tmp25B->tl;_tmp263;});}}_tmp264;});struct _tuple5 _tmp25F=({struct _tuple5 _tmp25C;
_tmp25C.f1=_tmp25A;_tmp25C.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp25D=_cycalloc(sizeof(*_tmp25D));_tmp25D[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp25E;_tmp25E.tag=4;_tmp25E.f1=aggrdict;_tmp25E;});_tmp25D;});_tmp25C;});
_npop_handler(0);return _tmp25F;}};_pop_region(temp);}_LLE4: if(*((int*)_tmpC9)!= 
31)goto _LLE6;_tmp105=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpC9)->f2;_LLE5: {
struct Cyc_List_List*fs;{void*_tmp265=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_List_List*_tmp266;_LL181: if(_tmp265 <= (void*)
4)goto _LL183;if(*((int*)_tmp265)!= 11)goto _LL183;_tmp266=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp265)->f2;_LL182: fs=_tmp266;goto _LL180;_LL183:;_LL184:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267[0]=({
struct Cyc_Core_Impossible_struct _tmp268;_tmp268.tag=Cyc_Core_Impossible;_tmp268.f1=({
const char*_tmp269="anal_Rexp:anon struct has bad type";_tag_dynforward(_tmp269,
sizeof(char),_get_zero_arr_size(_tmp269,35));});_tmp268;});_tmp267;}));_LL180:;}
_tmp106=_tmp105;_tmp10B=fs;goto _LLE7;}_LLE6: if(*((int*)_tmpC9)!= 30)goto _LLE8;
_tmp106=((struct Cyc_Absyn_Struct_e_struct*)_tmpC9)->f3;_tmp107=((struct Cyc_Absyn_Struct_e_struct*)
_tmpC9)->f4;if(_tmp107 == 0)goto _LLE8;_tmp108=*_tmp107;_tmp109=_tmp108.impl;if(
_tmp109 == 0)goto _LLE8;_tmp10A=*_tmp109;_tmp10B=_tmp10A.fields;_LLE7: {struct
_RegionHandle _tmp26A=_new_region("temp");struct _RegionHandle*temp=& _tmp26A;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp26C;struct Cyc_List_List*
_tmp26D;struct _tuple9 _tmp26B=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple11*))Cyc_Core_snd,_tmp106),0);_tmp26C=_tmp26B.f1;_tmp26D=_tmp26B.f2;
_tmp26C=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp26C);{struct
_dynforward_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp10B,Cyc_CfFlowInfo_unknown_all);{
int i=0;for(0;_tmp26D != 0;(((_tmp26D=_tmp26D->tl,_tmp106=_tmp106->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(
_tmp106))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*_tmp26E=(void*)ds->hd;struct
_dynforward_ptr*_tmp26F;_LL186: if(*((int*)_tmp26E)!= 0)goto _LL188;_LL187:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp270=_cycalloc(sizeof(*
_tmp270));_tmp270[0]=({struct Cyc_Core_Impossible_struct _tmp271;_tmp271.tag=Cyc_Core_Impossible;
_tmp271.f1=({const char*_tmp272="anal_Rexp:Struct_e";_tag_dynforward(_tmp272,
sizeof(char),_get_zero_arr_size(_tmp272,19));});_tmp271;});_tmp270;}));_LL188:
if(*((int*)_tmp26E)!= 1)goto _LL185;_tmp26F=((struct Cyc_Absyn_FieldName_struct*)
_tmp26E)->f1;_LL189: {int _tmp273=Cyc_NewControlFlow_get_field_index_fs(_tmp10B,
_tmp26F);*((void**)_check_dynforward_subscript(aggrdict,sizeof(void*),_tmp273))=(
void*)_tmp26D->hd;}_LL185:;}}}{struct _tuple5 _tmp277=({struct _tuple5 _tmp274;
_tmp274.f1=_tmp26C;_tmp274.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp275=_cycalloc(sizeof(*_tmp275));_tmp275[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp276;_tmp276.tag=4;_tmp276.f1=aggrdict;_tmp276;});_tmp275;});_tmp274;});
_npop_handler(0);return _tmp277;}}};_pop_region(temp);}_LLE8: if(*((int*)_tmpC9)!= 
30)goto _LLEA;_LLE9:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp278=
_cycalloc(sizeof(*_tmp278));_tmp278[0]=({struct Cyc_Core_Impossible_struct _tmp279;
_tmp279.tag=Cyc_Core_Impossible;_tmp279.f1=({const char*_tmp27A="anal_Rexp:missing aggrdeclimpl";
_tag_dynforward(_tmp27A,sizeof(char),_get_zero_arr_size(_tmp27A,31));});_tmp279;});
_tmp278;}));_LLEA: if(*((int*)_tmpC9)!= 28)goto _LLEC;_tmp10C=((struct Cyc_Absyn_Array_e_struct*)
_tmpC9)->f1;_LLEB: {struct _RegionHandle _tmp27B=_new_region("temp");struct
_RegionHandle*temp=& _tmp27B;_push_region(temp);{struct Cyc_List_List*_tmp27C=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct
_tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple11*))Cyc_Core_snd,_tmp10C);union Cyc_CfFlowInfo_FlowInfo_union _tmp27E;
struct Cyc_List_List*_tmp27F;struct _tuple9 _tmp27D=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp27C,0);_tmp27E=_tmp27D.f1;_tmp27F=_tmp27D.f2;_tmp27E=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp27E);for(0;_tmp27F != 0;(_tmp27F=_tmp27F->tl,_tmp27C=_tmp27C->tl)){
_tmp27E=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp27C))->hd)->loc,_tmp27E,(void*)_tmp27F->hd);}{struct _tuple5
_tmp281=({struct _tuple5 _tmp280;_tmp280.f1=_tmp27E;_tmp280.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp280;});_npop_handler(0);return _tmp281;}};_pop_region(temp);}_LLEC: if(*((int*)
_tmpC9)!= 29)goto _LLEE;_tmp10D=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpC9)->f1;
_tmp10E=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpC9)->f2;_tmp10F=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmpC9)->f3;_tmp110=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpC9)->f4;_LLED: {union Cyc_CfFlowInfo_FlowInfo_union _tmp283;void*_tmp284;
struct _tuple5 _tmp282=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp10E);_tmp283=
_tmp282.f1;_tmp284=_tmp282.f2;_tmp283=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp10E->loc,_tmp283);{union Cyc_CfFlowInfo_FlowInfo_union _tmp285=_tmp283;struct
Cyc_Dict_Dict _tmp286;struct Cyc_List_List*_tmp287;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp288;_LL18B: if((_tmp285.BottomFL).tag != 0)goto _LL18D;_LL18C: return({struct
_tuple5 _tmp289;_tmp289.f1=_tmp283;_tmp289.f2=Cyc_CfFlowInfo_unknown_all;_tmp289;});
_LL18D: if((_tmp285.ReachableFL).tag != 1)goto _LL18A;_tmp286=(_tmp285.ReachableFL).f1;
_tmp287=(_tmp285.ReachableFL).f2;_tmp288=(_tmp285.ReachableFL).f3;_LL18E: if(Cyc_CfFlowInfo_initlevel(
_tmp286,_tmp284)== (void*)0)({void*_tmp28A[0]={};Cyc_Tcutil_terr(_tmp10E->loc,({
const char*_tmp28B="expression may not be initialized";_tag_dynforward(_tmp28B,
sizeof(char),_get_zero_arr_size(_tmp28B,34));}),_tag_dynforward(_tmp28A,sizeof(
void*),0));});{struct Cyc_List_List*new_relns=_tmp287;comp_loop: {void*_tmp28C=(
void*)_tmp10E->r;struct Cyc_Absyn_Exp*_tmp28D;void*_tmp28E;struct Cyc_Absyn_Vardecl*
_tmp28F;void*_tmp290;struct Cyc_Absyn_Vardecl*_tmp291;void*_tmp292;struct Cyc_Absyn_Vardecl*
_tmp293;void*_tmp294;struct Cyc_Absyn_Vardecl*_tmp295;union Cyc_Absyn_Cnst_union
_tmp296;int _tmp297;void*_tmp298;struct Cyc_List_List*_tmp299;struct Cyc_List_List
_tmp29A;struct Cyc_Absyn_Exp*_tmp29B;_LL190: if(*((int*)_tmp28C)!= 15)goto _LL192;
_tmp28D=((struct Cyc_Absyn_Cast_e_struct*)_tmp28C)->f2;_LL191: _tmp10E=_tmp28D;
goto comp_loop;_LL192: if(*((int*)_tmp28C)!= 1)goto _LL194;_tmp28E=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp28C)->f2;if(_tmp28E <= (void*)1)goto _LL194;if(*((int*)
_tmp28E)!= 0)goto _LL194;_tmp28F=((struct Cyc_Absyn_Global_b_struct*)_tmp28E)->f1;
if(!(!_tmp28F->escapes))goto _LL194;_LL193: _tmp291=_tmp28F;goto _LL195;_LL194: if(*((
int*)_tmp28C)!= 1)goto _LL196;_tmp290=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp28C)->f2;if(_tmp290 <= (void*)1)goto _LL196;if(*((int*)_tmp290)!= 3)goto _LL196;
_tmp291=((struct Cyc_Absyn_Local_b_struct*)_tmp290)->f1;if(!(!_tmp291->escapes))
goto _LL196;_LL195: _tmp293=_tmp291;goto _LL197;_LL196: if(*((int*)_tmp28C)!= 1)goto
_LL198;_tmp292=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp28C)->f2;if(_tmp292 <= (
void*)1)goto _LL198;if(*((int*)_tmp292)!= 4)goto _LL198;_tmp293=((struct Cyc_Absyn_Pat_b_struct*)
_tmp292)->f1;if(!(!_tmp293->escapes))goto _LL198;_LL197: _tmp295=_tmp293;goto
_LL199;_LL198: if(*((int*)_tmp28C)!= 1)goto _LL19A;_tmp294=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp28C)->f2;if(_tmp294 <= (void*)1)goto _LL19A;if(*((int*)_tmp294)!= 2)goto _LL19A;
_tmp295=((struct Cyc_Absyn_Param_b_struct*)_tmp294)->f1;if(!(!_tmp295->escapes))
goto _LL19A;_LL199: new_relns=({struct Cyc_List_List*_tmp29C=_cycalloc(sizeof(*
_tmp29C));_tmp29C->hd=({struct Cyc_CfFlowInfo_Reln*_tmp29D=_cycalloc(sizeof(*
_tmp29D));_tmp29D->vd=_tmp10D;_tmp29D->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp29E;(_tmp29E.LessVar).tag=1;(_tmp29E.LessVar).f1=
_tmp295;_tmp29E;});_tmp29D;});_tmp29C->tl=_tmp287;_tmp29C;});goto _LL18F;_LL19A:
if(*((int*)_tmp28C)!= 0)goto _LL19C;_tmp296=((struct Cyc_Absyn_Const_e_struct*)
_tmp28C)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp28C)->f1).Int_c).tag != 2)
goto _LL19C;_tmp297=(_tmp296.Int_c).f2;_LL19B: new_relns=({struct Cyc_List_List*
_tmp29F=_cycalloc(sizeof(*_tmp29F));_tmp29F->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp2A0=_cycalloc(sizeof(*_tmp2A0));_tmp2A0->vd=_tmp10D;_tmp2A0->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp2A1;(_tmp2A1.LessConst).tag=3;(_tmp2A1.LessConst).f1=(
unsigned int)_tmp297;_tmp2A1;});_tmp2A0;});_tmp29F->tl=_tmp287;_tmp29F;});goto
_LL18F;_LL19C: if(*((int*)_tmp28C)!= 3)goto _LL19E;_tmp298=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp28C)->f1;if((int)_tmp298 != 19)goto _LL19E;_tmp299=((struct Cyc_Absyn_Primop_e_struct*)
_tmp28C)->f2;if(_tmp299 == 0)goto _LL19E;_tmp29A=*_tmp299;_tmp29B=(struct Cyc_Absyn_Exp*)
_tmp29A.hd;_LL19D:{void*_tmp2A2=(void*)_tmp29B->r;void*_tmp2A3;struct Cyc_Absyn_Vardecl*
_tmp2A4;void*_tmp2A5;struct Cyc_Absyn_Vardecl*_tmp2A6;void*_tmp2A7;struct Cyc_Absyn_Vardecl*
_tmp2A8;void*_tmp2A9;struct Cyc_Absyn_Vardecl*_tmp2AA;_LL1A1: if(*((int*)_tmp2A2)
!= 1)goto _LL1A3;_tmp2A3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2A2)->f2;if(
_tmp2A3 <= (void*)1)goto _LL1A3;if(*((int*)_tmp2A3)!= 0)goto _LL1A3;_tmp2A4=((
struct Cyc_Absyn_Global_b_struct*)_tmp2A3)->f1;if(!(!_tmp2A4->escapes))goto _LL1A3;
_LL1A2: _tmp2A6=_tmp2A4;goto _LL1A4;_LL1A3: if(*((int*)_tmp2A2)!= 1)goto _LL1A5;
_tmp2A5=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2A2)->f2;if(_tmp2A5 <= (void*)
1)goto _LL1A5;if(*((int*)_tmp2A5)!= 3)goto _LL1A5;_tmp2A6=((struct Cyc_Absyn_Local_b_struct*)
_tmp2A5)->f1;if(!(!_tmp2A6->escapes))goto _LL1A5;_LL1A4: _tmp2A8=_tmp2A6;goto
_LL1A6;_LL1A5: if(*((int*)_tmp2A2)!= 1)goto _LL1A7;_tmp2A7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2A2)->f2;if(_tmp2A7 <= (void*)1)goto _LL1A7;if(*((int*)_tmp2A7)!= 4)goto _LL1A7;
_tmp2A8=((struct Cyc_Absyn_Pat_b_struct*)_tmp2A7)->f1;if(!(!_tmp2A8->escapes))
goto _LL1A7;_LL1A6: _tmp2AA=_tmp2A8;goto _LL1A8;_LL1A7: if(*((int*)_tmp2A2)!= 1)goto
_LL1A9;_tmp2A9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2A2)->f2;if(_tmp2A9 <= (
void*)1)goto _LL1A9;if(*((int*)_tmp2A9)!= 2)goto _LL1A9;_tmp2AA=((struct Cyc_Absyn_Param_b_struct*)
_tmp2A9)->f1;if(!(!_tmp2AA->escapes))goto _LL1A9;_LL1A8: new_relns=({struct Cyc_List_List*
_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp2AC=_cycalloc(sizeof(*_tmp2AC));_tmp2AC->vd=_tmp10D;_tmp2AC->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp2AD;(_tmp2AD.LessSize).tag=2;(_tmp2AD.LessSize).f1=
_tmp2AA;_tmp2AD;});_tmp2AC;});_tmp2AB->tl=_tmp287;_tmp2AB;});goto _LL1A0;_LL1A9:;
_LL1AA: goto _LL1A0;_LL1A0:;}goto _LL18F;_LL19E:;_LL19F: goto _LL18F;_LL18F:;}if(
_tmp287 != new_relns)_tmp283=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp2AE;(_tmp2AE.ReachableFL).tag=1;(_tmp2AE.ReachableFL).f1=_tmp286;(_tmp2AE.ReachableFL).f2=
new_relns;(_tmp2AE.ReachableFL).f3=_tmp288;_tmp2AE;});{void*_tmp2AF=_tmp284;
_LL1AC: if((int)_tmp2AF != 0)goto _LL1AE;_LL1AD: return({struct _tuple5 _tmp2B0;
_tmp2B0.f1=_tmp283;_tmp2B0.f2=Cyc_CfFlowInfo_unknown_all;_tmp2B0;});_LL1AE: if((
int)_tmp2AF != 2)goto _LL1B0;_LL1AF: goto _LL1B1;_LL1B0: if((int)_tmp2AF != 1)goto
_LL1B2;_LL1B1: goto _LL1B3;_LL1B2: if(_tmp2AF <= (void*)3)goto _LL1B4;if(*((int*)
_tmp2AF)!= 2)goto _LL1B4;_LL1B3: {struct Cyc_List_List _tmp2B1=({struct Cyc_List_List
_tmp2BD;_tmp2BD.hd=_tmp10D;_tmp2BD.tl=0;_tmp2BD;});_tmp283=Cyc_NewControlFlow_add_vars(
_tmp283,(struct Cyc_List_List*)& _tmp2B1,Cyc_CfFlowInfo_unknown_all,e->loc);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp2B3;void*_tmp2B4;struct _tuple5 _tmp2B2=Cyc_NewControlFlow_anal_Rexp(
env,_tmp283,_tmp10F);_tmp2B3=_tmp2B2.f1;_tmp2B4=_tmp2B2.f2;_tmp2B3=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp10F->loc,_tmp2B3);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2B5=_tmp2B3;struct
Cyc_Dict_Dict _tmp2B6;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2B7;_LL1B7: if((_tmp2B5.BottomFL).tag
!= 0)goto _LL1B9;_LL1B8: return({struct _tuple5 _tmp2B8;_tmp2B8.f1=_tmp2B3;_tmp2B8.f2=
Cyc_CfFlowInfo_unknown_all;_tmp2B8;});_LL1B9: if((_tmp2B5.ReachableFL).tag != 1)
goto _LL1B6;_tmp2B6=(_tmp2B5.ReachableFL).f1;_tmp2B7=(_tmp2B5.ReachableFL).f3;
_LL1BA: if(Cyc_CfFlowInfo_initlevel(_tmp2B6,_tmp2B4)!= (void*)2){({void*_tmp2B9[0]={};
Cyc_Tcutil_terr(_tmp10E->loc,({const char*_tmp2BA="expression may not be initialized";
_tag_dynforward(_tmp2BA,sizeof(char),_get_zero_arr_size(_tmp2BA,34));}),
_tag_dynforward(_tmp2B9,sizeof(void*),0));});return({struct _tuple5 _tmp2BB;
_tmp2BB.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp2BC;(_tmp2BC.BottomFL).tag=0;_tmp2BC;});_tmp2BB.f2=Cyc_CfFlowInfo_unknown_all;
_tmp2BB;});}_LL1B6:;}_tmp283=_tmp2B3;goto _LL1B5;}}_LL1B4:;_LL1B5: while(1){struct
Cyc_List_List _tmp2BE=({struct Cyc_List_List _tmp2CA;_tmp2CA.hd=_tmp10D;_tmp2CA.tl=
0;_tmp2CA;});_tmp283=Cyc_NewControlFlow_add_vars(_tmp283,(struct Cyc_List_List*)&
_tmp2BE,Cyc_CfFlowInfo_unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2C0;void*_tmp2C1;struct _tuple5 _tmp2BF=Cyc_NewControlFlow_anal_Rexp(env,
_tmp283,_tmp10F);_tmp2C0=_tmp2BF.f1;_tmp2C1=_tmp2BF.f2;_tmp2C0=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp10F->loc,_tmp2C0);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2C2=_tmp2C0;struct
Cyc_Dict_Dict _tmp2C3;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2C4;_LL1BC: if((_tmp2C2.BottomFL).tag
!= 0)goto _LL1BE;_LL1BD: goto _LL1BB;_LL1BE: if((_tmp2C2.ReachableFL).tag != 1)goto
_LL1BB;_tmp2C3=(_tmp2C2.ReachableFL).f1;_tmp2C4=(_tmp2C2.ReachableFL).f3;_LL1BF:
if(Cyc_CfFlowInfo_initlevel(_tmp2C3,_tmp2C1)!= (void*)2){({void*_tmp2C5[0]={};
Cyc_Tcutil_terr(_tmp10E->loc,({const char*_tmp2C6="expression may not be initialized";
_tag_dynforward(_tmp2C6,sizeof(char),_get_zero_arr_size(_tmp2C6,34));}),
_tag_dynforward(_tmp2C5,sizeof(void*),0));});return({struct _tuple5 _tmp2C7;
_tmp2C7.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp2C8;(_tmp2C8.BottomFL).tag=0;_tmp2C8;});_tmp2C7.f2=Cyc_CfFlowInfo_unknown_all;
_tmp2C7;});}_LL1BB:;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp2C9=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp283,_tmp2C0,1);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp2C9,_tmp283))break;_tmp283=_tmp2C9;}}}return({struct _tuple5 _tmp2CB;_tmp2CB.f1=
_tmp283;_tmp2CB.f2=Cyc_CfFlowInfo_unknown_all;_tmp2CB;});_LL1AB:;}}_LL18A:;}}
_LLEE: if(*((int*)_tmpC9)!= 37)goto _LLF0;_tmp111=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmpC9)->f1;_LLEF: while(1){inflow=(Cyc_NewControlFlow_pre_stmt_check(env,inflow,
_tmp111))->flow;{void*_tmp2CC=(void*)_tmp111->r;struct Cyc_Absyn_Stmt*_tmp2CD;
struct Cyc_Absyn_Stmt*_tmp2CE;struct Cyc_Absyn_Decl*_tmp2CF;struct Cyc_Absyn_Stmt*
_tmp2D0;struct Cyc_Absyn_Exp*_tmp2D1;_LL1C1: if(_tmp2CC <= (void*)1)goto _LL1C7;if(*((
int*)_tmp2CC)!= 1)goto _LL1C3;_tmp2CD=((struct Cyc_Absyn_Seq_s_struct*)_tmp2CC)->f1;
_tmp2CE=((struct Cyc_Absyn_Seq_s_struct*)_tmp2CC)->f2;_LL1C2: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp2CD);_tmp111=_tmp2CE;goto _LL1C0;_LL1C3: if(*((int*)_tmp2CC)!= 11)
goto _LL1C5;_tmp2CF=((struct Cyc_Absyn_Decl_s_struct*)_tmp2CC)->f1;_tmp2D0=((
struct Cyc_Absyn_Decl_s_struct*)_tmp2CC)->f2;_LL1C4: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp2CF);_tmp111=_tmp2D0;goto _LL1C0;_LL1C5: if(*((int*)_tmp2CC)!= 0)
goto _LL1C7;_tmp2D1=((struct Cyc_Absyn_Exp_s_struct*)_tmp2CC)->f1;_LL1C6: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp2D1);_LL1C7:;_LL1C8:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2D2=_cycalloc(sizeof(*_tmp2D2));_tmp2D2[0]=({struct Cyc_Core_Impossible_struct
_tmp2D3;_tmp2D3.tag=Cyc_Core_Impossible;_tmp2D3.f1=({const char*_tmp2D4="analyze_Rexp: ill-formed StmtExp";
_tag_dynforward(_tmp2D4,sizeof(char),_get_zero_arr_size(_tmp2D4,33));});_tmp2D3;});
_tmp2D2;}));_LL1C0:;}}_LLF0: if(*((int*)_tmpC9)!= 1)goto _LLF2;_tmp112=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpC9)->f2;if((int)_tmp112 != 0)goto _LLF2;_LLF1:
goto _LLF3;_LLF2: if(*((int*)_tmpC9)!= 2)goto _LLF4;_LLF3: goto _LLF5;_LLF4: if(*((int*)
_tmpC9)!= 10)goto _LLF6;_LLF5: goto _LLF7;_LLF6: if(*((int*)_tmpC9)!= 36)goto _LLF8;
_LLF7: goto _LLF9;_LLF8: if(*((int*)_tmpC9)!= 27)goto _LL95;_LLF9:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp2D5=_cycalloc(sizeof(*_tmp2D5));_tmp2D5[0]=({
struct Cyc_Core_Impossible_struct _tmp2D6;_tmp2D6.tag=Cyc_Core_Impossible;_tmp2D6.f1=({
const char*_tmp2D7="anal_Rexp, unexpected exp form";_tag_dynforward(_tmp2D7,
sizeof(char),_get_zero_arr_size(_tmp2D7,31));});_tmp2D6;});_tmp2D5;}));_LL95:;}}
static struct _tuple5 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union f,
struct Cyc_Absyn_Exp*e,void*r,struct Cyc_List_List*flds){void*_tmp2D8=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp2D9;void*_tmp2DA;struct Cyc_Absyn_PtrAtts _tmp2DB;struct Cyc_Absyn_Conref*
_tmp2DC;struct Cyc_Absyn_Conref*_tmp2DD;_LL1CA: if(_tmp2D8 <= (void*)4)goto _LL1CC;
if(*((int*)_tmp2D8)!= 4)goto _LL1CC;_tmp2D9=((struct Cyc_Absyn_PointerType_struct*)
_tmp2D8)->f1;_tmp2DA=(void*)_tmp2D9.elt_typ;_tmp2DB=_tmp2D9.ptr_atts;_tmp2DC=
_tmp2DB.bounds;_tmp2DD=_tmp2DB.zero_term;_LL1CB: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp2DE=f;struct Cyc_Dict_Dict _tmp2DF;struct Cyc_List_List*_tmp2E0;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2E1;_LL1CF: if((_tmp2DE.BottomFL).tag != 0)goto _LL1D1;_LL1D0: return({struct
_tuple5 _tmp2E2;_tmp2E2.f1=f;_tmp2E2.f2=(void*)0;_tmp2E2;});_LL1D1: if((_tmp2DE.ReachableFL).tag
!= 1)goto _LL1CE;_tmp2DF=(_tmp2DE.ReachableFL).f1;_tmp2E0=(_tmp2DE.ReachableFL).f2;
_tmp2E1=(_tmp2DE.ReachableFL).f3;_LL1D2: if(Cyc_Tcutil_is_bound_one(_tmp2DC)){
void*_tmp2E3=r;struct Cyc_CfFlowInfo_Place*_tmp2E4;struct Cyc_CfFlowInfo_Place
_tmp2E5;void*_tmp2E6;struct Cyc_List_List*_tmp2E7;void*_tmp2E8;_LL1D4: if((int)
_tmp2E3 != 1)goto _LL1D6;_LL1D5: goto _LL1D7;_LL1D6: if((int)_tmp2E3 != 2)goto _LL1D8;
_LL1D7:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp2E9=_cycalloc(sizeof(*_tmp2E9));_tmp2E9[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp2EA;_tmp2EA.tag=Cyc_CfFlowInfo_NotZero;_tmp2EA.f1=_tmp2E0;_tmp2EA;});_tmp2E9;})));
goto _LL1D3;_LL1D8: if(_tmp2E3 <= (void*)3)goto _LL1DA;if(*((int*)_tmp2E3)!= 2)goto
_LL1DA;_tmp2E4=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2E3)->f1;_tmp2E5=*
_tmp2E4;_tmp2E6=(void*)_tmp2E5.root;_tmp2E7=_tmp2E5.fields;_LL1D9:(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp2EB=_cycalloc(sizeof(*
_tmp2EB));_tmp2EB[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp2EC;_tmp2EC.tag=
Cyc_CfFlowInfo_NotZero;_tmp2EC.f1=_tmp2E0;_tmp2EC;});_tmp2EB;})));return({struct
_tuple5 _tmp2ED;_tmp2ED.f1=f;_tmp2ED.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*
_tmp2EE=_cycalloc(sizeof(*_tmp2EE));_tmp2EE[0]=({struct Cyc_CfFlowInfo_PlaceL_struct
_tmp2EF;_tmp2EF.tag=0;_tmp2EF.f1=({struct Cyc_CfFlowInfo_Place*_tmp2F0=_cycalloc(
sizeof(*_tmp2F0));_tmp2F0->root=(void*)_tmp2E6;_tmp2F0->fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2E7,flds);
_tmp2F0;});_tmp2EF;});_tmp2EE;});_tmp2ED;});_LL1DA: if((int)_tmp2E3 != 0)goto
_LL1DC;_LL1DB:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));return({
struct _tuple5 _tmp2F1;_tmp2F1.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp2F2;(_tmp2F2.BottomFL).tag=0;_tmp2F2;});_tmp2F1.f2=(void*)0;_tmp2F1;});
_LL1DC: if(_tmp2E3 <= (void*)3)goto _LL1DE;if(*((int*)_tmp2E3)!= 0)goto _LL1DE;
_tmp2E8=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp2E3)->f1;_LL1DD: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmp2E8);goto _LL1DF;_LL1DE:;_LL1DF:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2F3=_cycalloc(sizeof(*_tmp2F3));_tmp2F3[
0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp2F4;_tmp2F4.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp2F4.f1=_tmp2E0;_tmp2F4;});_tmp2F3;})));_LL1D3:;}else{(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2F5=_cycalloc(sizeof(*_tmp2F5));
_tmp2F5[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp2F6;_tmp2F6.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp2F6.f1=_tmp2E0;_tmp2F6;});_tmp2F5;})));}if(Cyc_CfFlowInfo_initlevel(_tmp2DF,
r)== (void*)0)({void*_tmp2F7[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp2F8="dereference of possibly uninitialized pointer";
_tag_dynforward(_tmp2F8,sizeof(char),_get_zero_arr_size(_tmp2F8,46));}),
_tag_dynforward(_tmp2F7,sizeof(void*),0));});return({struct _tuple5 _tmp2F9;
_tmp2F9.f1=f;_tmp2F9.f2=(void*)0;_tmp2F9;});_LL1CE:;}_LL1CC:;_LL1CD:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp2FA=_cycalloc(sizeof(*_tmp2FA));
_tmp2FA[0]=({struct Cyc_Core_Impossible_struct _tmp2FB;_tmp2FB.tag=Cyc_Core_Impossible;
_tmp2FB.f1=({const char*_tmp2FC="left deref of non-pointer-type";_tag_dynforward(
_tmp2FC,sizeof(char),_get_zero_arr_size(_tmp2FC,31));});_tmp2FB;});_tmp2FA;}));
_LL1C9:;}static struct _tuple5 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict d;{union Cyc_CfFlowInfo_FlowInfo_union _tmp2FD=inflow;
struct Cyc_Dict_Dict _tmp2FE;struct Cyc_List_List*_tmp2FF;_LL1E1: if((_tmp2FD.BottomFL).tag
!= 0)goto _LL1E3;_LL1E2: return({struct _tuple5 _tmp300;_tmp300.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp301;(_tmp301.BottomFL).tag=0;_tmp301;});
_tmp300.f2=(void*)0;_tmp300;});_LL1E3: if((_tmp2FD.ReachableFL).tag != 1)goto
_LL1E0;_tmp2FE=(_tmp2FD.ReachableFL).f1;_tmp2FF=(_tmp2FD.ReachableFL).f2;_LL1E4:
d=_tmp2FE;_LL1E0:;}{void*_tmp302=(void*)e->r;struct Cyc_Absyn_Exp*_tmp303;struct
Cyc_Absyn_Exp*_tmp304;struct Cyc_Absyn_Exp*_tmp305;void*_tmp306;struct Cyc_Absyn_Vardecl*
_tmp307;void*_tmp308;struct Cyc_Absyn_Vardecl*_tmp309;void*_tmp30A;struct Cyc_Absyn_Vardecl*
_tmp30B;void*_tmp30C;struct Cyc_Absyn_Vardecl*_tmp30D;struct Cyc_Absyn_Exp*_tmp30E;
struct _dynforward_ptr*_tmp30F;struct Cyc_Absyn_Exp*_tmp310;struct Cyc_Absyn_Exp*
_tmp311;struct Cyc_Absyn_Exp*_tmp312;struct Cyc_Absyn_Exp*_tmp313;struct
_dynforward_ptr*_tmp314;_LL1E6: if(*((int*)_tmp302)!= 15)goto _LL1E8;_tmp303=((
struct Cyc_Absyn_Cast_e_struct*)_tmp302)->f2;_LL1E7: _tmp304=_tmp303;goto _LL1E9;
_LL1E8: if(*((int*)_tmp302)!= 13)goto _LL1EA;_tmp304=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp302)->f1;_LL1E9: _tmp305=_tmp304;goto _LL1EB;_LL1EA: if(*((int*)_tmp302)!= 14)
goto _LL1EC;_tmp305=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp302)->f1;_LL1EB:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp305,flds);_LL1EC: if(*((int*)
_tmp302)!= 1)goto _LL1EE;_tmp306=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp302)->f2;
if(_tmp306 <= (void*)1)goto _LL1EE;if(*((int*)_tmp306)!= 2)goto _LL1EE;_tmp307=((
struct Cyc_Absyn_Param_b_struct*)_tmp306)->f1;_LL1ED: _tmp309=_tmp307;goto _LL1EF;
_LL1EE: if(*((int*)_tmp302)!= 1)goto _LL1F0;_tmp308=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp302)->f2;if(_tmp308 <= (void*)1)goto _LL1F0;if(*((int*)_tmp308)!= 3)goto _LL1F0;
_tmp309=((struct Cyc_Absyn_Local_b_struct*)_tmp308)->f1;_LL1EF: _tmp30B=_tmp309;
goto _LL1F1;_LL1F0: if(*((int*)_tmp302)!= 1)goto _LL1F2;_tmp30A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp302)->f2;if(_tmp30A <= (void*)1)goto _LL1F2;if(*((int*)_tmp30A)!= 4)goto _LL1F2;
_tmp30B=((struct Cyc_Absyn_Pat_b_struct*)_tmp30A)->f1;_LL1F1: return({struct
_tuple5 _tmp315;_tmp315.f1=inflow;_tmp315.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*
_tmp316=_cycalloc(sizeof(*_tmp316));_tmp316[0]=({struct Cyc_CfFlowInfo_PlaceL_struct
_tmp317;_tmp317.tag=0;_tmp317.f1=({struct Cyc_CfFlowInfo_Place*_tmp318=_cycalloc(
sizeof(*_tmp318));_tmp318->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp319=_cycalloc(sizeof(*_tmp319));_tmp319[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp31A;_tmp31A.tag=0;_tmp31A.f1=_tmp30B;_tmp31A;});_tmp319;}));_tmp318->fields=
flds;_tmp318;});_tmp317;});_tmp316;});_tmp315;});_LL1F2: if(*((int*)_tmp302)!= 1)
goto _LL1F4;_tmp30C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp302)->f2;if(
_tmp30C <= (void*)1)goto _LL1F4;if(*((int*)_tmp30C)!= 0)goto _LL1F4;_tmp30D=((
struct Cyc_Absyn_Global_b_struct*)_tmp30C)->f1;_LL1F3: return({struct _tuple5
_tmp31B;_tmp31B.f1=inflow;_tmp31B.f2=(void*)0;_tmp31B;});_LL1F4: if(*((int*)
_tmp302)!= 24)goto _LL1F6;_tmp30E=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp302)->f1;
_tmp30F=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp302)->f2;_LL1F5:{void*_tmp31C=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp30E->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp31D;void*_tmp31E;_LL1FF: if(_tmp31C <= (void*)4)goto
_LL201;if(*((int*)_tmp31C)!= 4)goto _LL201;_tmp31D=((struct Cyc_Absyn_PointerType_struct*)
_tmp31C)->f1;_tmp31E=(void*)_tmp31D.elt_typ;_LL200: if(!Cyc_Absyn_is_union_type(
_tmp31E))flds=({struct Cyc_List_List*_tmp31F=_cycalloc(sizeof(*_tmp31F));_tmp31F->hd=(
void*)Cyc_NewControlFlow_get_field_index(_tmp31E,_tmp30F);_tmp31F->tl=flds;
_tmp31F;});goto _LL1FE;_LL201:;_LL202:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp320=_cycalloc(sizeof(*_tmp320));_tmp320[0]=({struct Cyc_Core_Impossible_struct
_tmp321;_tmp321.tag=Cyc_Core_Impossible;_tmp321.f1=({const char*_tmp322="anal_Rexp: AggrArrow ptr";
_tag_dynforward(_tmp322,sizeof(char),_get_zero_arr_size(_tmp322,25));});_tmp321;});
_tmp320;}));_LL1FE:;}_tmp310=_tmp30E;goto _LL1F7;_LL1F6: if(*((int*)_tmp302)!= 22)
goto _LL1F8;_tmp310=((struct Cyc_Absyn_Deref_e_struct*)_tmp302)->f1;_LL1F7: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp324;void*_tmp325;struct _tuple5 _tmp323=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp310);_tmp324=_tmp323.f1;_tmp325=_tmp323.f2;_tmp324=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp324);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp324,_tmp310,
_tmp325,flds);}_LL1F8: if(*((int*)_tmp302)!= 25)goto _LL1FA;_tmp311=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp302)->f1;_tmp312=((struct Cyc_Absyn_Subscript_e_struct*)_tmp302)->f2;_LL1F9: {
void*_tmp326=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp311->topt))->v);struct Cyc_Absyn_PtrInfo _tmp327;struct Cyc_Absyn_PtrAtts
_tmp328;struct Cyc_Absyn_Conref*_tmp329;_LL204: if(_tmp326 <= (void*)4)goto _LL208;
if(*((int*)_tmp326)!= 9)goto _LL206;_LL205: {unsigned int _tmp32A=(Cyc_Evexp_eval_const_uint_exp(
_tmp312)).f1;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp311,({struct
Cyc_List_List*_tmp32B=_cycalloc(sizeof(*_tmp32B));_tmp32B->hd=(void*)((int)
_tmp32A);_tmp32B->tl=flds;_tmp32B;}));}_LL206: if(*((int*)_tmp326)!= 4)goto _LL208;
_tmp327=((struct Cyc_Absyn_PointerType_struct*)_tmp326)->f1;_tmp328=_tmp327.ptr_atts;
_tmp329=_tmp328.bounds;_LL207: {struct _RegionHandle _tmp32C=_new_region("temp");
struct _RegionHandle*temp=& _tmp32C;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp32F;struct Cyc_List_List*_tmp330;struct _tuple9 _tmp32E=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp32D[2];_tmp32D[1]=_tmp312;_tmp32D[0]=
_tmp311;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))
Cyc_List_rlist)(temp,_tag_dynforward(_tmp32D,sizeof(struct Cyc_Absyn_Exp*),2));}),
1);_tmp32F=_tmp32E.f1;_tmp330=_tmp32E.f2;_tmp32F=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp312->loc,_tmp32F);{union Cyc_CfFlowInfo_FlowInfo_union _tmp331=_tmp32F;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp332=_tmp32F;struct Cyc_Dict_Dict _tmp333;struct
Cyc_List_List*_tmp334;struct Cyc_CfFlowInfo_ConsumeInfo _tmp335;_LL20B: if((_tmp332.ReachableFL).tag
!= 1)goto _LL20D;_tmp333=(_tmp332.ReachableFL).f1;_tmp334=(_tmp332.ReachableFL).f2;
_tmp335=(_tmp332.ReachableFL).f3;_LL20C: if(Cyc_CfFlowInfo_initlevel(_tmp333,(
void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(
_tmp330))->tl))->hd)== (void*)0)({void*_tmp336[0]={};Cyc_Tcutil_terr(_tmp312->loc,({
const char*_tmp337="expression may not be initialized";_tag_dynforward(_tmp337,
sizeof(char),_get_zero_arr_size(_tmp337,34));}),_tag_dynforward(_tmp336,sizeof(
void*),0));});{struct Cyc_List_List*_tmp338=Cyc_NewControlFlow_add_subscript_reln(
_tmp334,_tmp311,_tmp312);if(_tmp334 != _tmp338)_tmp331=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp339;(_tmp339.ReachableFL).tag=1;(_tmp339.ReachableFL).f1=
_tmp333;(_tmp339.ReachableFL).f2=_tmp338;(_tmp339.ReachableFL).f3=_tmp335;
_tmp339;});goto _LL20A;}_LL20D:;_LL20E: goto _LL20A;_LL20A:;}{union Cyc_Absyn_Constraint_union
_tmp33A=(Cyc_Absyn_compress_conref(_tmp329))->v;void*_tmp33B;_LL210: if((_tmp33A.Eq_constr).tag
!= 0)goto _LL212;_tmp33B=(_tmp33A.Eq_constr).f1;if(_tmp33B <= (void*)2)goto _LL212;
if(*((int*)_tmp33B)!= 1)goto _LL212;_LL211:{void*_tmp33C=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp330))->tl))->hd;struct Cyc_List_List*
_tmp33D;_LL215: if(_tmp33C <= (void*)3)goto _LL217;if(*((int*)_tmp33C)!= 3)goto
_LL217;_tmp33D=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp33C)->f1;_LL216:(void*)(
_tmp312->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp33E=
_cycalloc(sizeof(*_tmp33E));_tmp33E[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp33F;_tmp33F.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp33F.f1=_tmp33D;_tmp33F;});
_tmp33E;})));goto _LL214;_LL217:;_LL218: goto _LL214;_LL214:;}goto _LL20F;_LL212:;
_LL213: goto _LL20F;_LL20F:;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp341;void*
_tmp342;struct _tuple5 _tmp340=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp32F,
_tmp311,(void*)((struct Cyc_List_List*)_check_null(_tmp330))->hd,flds);_tmp341=
_tmp340.f1;_tmp342=_tmp340.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp343=_tmp341;
_LL21A: if((_tmp343.BottomFL).tag != 0)goto _LL21C;_LL21B: {struct _tuple5 _tmp345=({
struct _tuple5 _tmp344;_tmp344.f1=_tmp341;_tmp344.f2=_tmp342;_tmp344;});
_npop_handler(0);return _tmp345;}_LL21C:;_LL21D: {struct _tuple5 _tmp347=({struct
_tuple5 _tmp346;_tmp346.f1=_tmp331;_tmp346.f2=_tmp342;_tmp346;});_npop_handler(0);
return _tmp347;}_LL219:;}}}};_pop_region(temp);}_LL208:;_LL209:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp348=_cycalloc(sizeof(*_tmp348));_tmp348[0]=({
struct Cyc_Core_Impossible_struct _tmp349;_tmp349.tag=Cyc_Core_Impossible;_tmp349.f1=({
const char*_tmp34A="anal_Lexp: Subscript -- bad type";_tag_dynforward(_tmp34A,
sizeof(char),_get_zero_arr_size(_tmp34A,33));});_tmp349;});_tmp348;}));_LL203:;}
_LL1FA: if(*((int*)_tmp302)!= 23)goto _LL1FC;_tmp313=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp302)->f1;_tmp314=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp302)->f2;_LL1FB:
if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp313->topt))->v))
return({struct _tuple5 _tmp34B;_tmp34B.f1=inflow;_tmp34B.f2=(void*)0;_tmp34B;});
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp313,({struct Cyc_List_List*
_tmp34C=_cycalloc(sizeof(*_tmp34C));_tmp34C->hd=(void*)Cyc_NewControlFlow_get_field_index((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp313->topt))->v,_tmp314);_tmp34C->tl=
flds;_tmp34C;}));_LL1FC:;_LL1FD: return({struct _tuple5 _tmp34D;_tmp34D.f1=(union
Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp34E;(
_tmp34E.BottomFL).tag=0;_tmp34E;});_tmp34D.f2=(void*)0;_tmp34D;});_LL1E5:;}}
static struct _tuple5 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){union Cyc_CfFlowInfo_FlowInfo_union
_tmp350;void*_tmp351;struct _tuple5 _tmp34F=Cyc_NewControlFlow_anal_Lexp_rec(env,
inflow,e,0);_tmp350=_tmp34F.f1;_tmp351=_tmp34F.f2;return({struct _tuple5 _tmp352;
_tmp352.f1=_tmp350;_tmp352.f2=_tmp351;_tmp352;});}static struct _tuple7 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){{void*_tmp353=(void*)e->r;struct Cyc_Absyn_Exp*_tmp354;
struct Cyc_Absyn_Exp*_tmp355;struct Cyc_Absyn_Exp*_tmp356;struct Cyc_Absyn_Exp*
_tmp357;struct Cyc_Absyn_Exp*_tmp358;struct Cyc_Absyn_Exp*_tmp359;struct Cyc_Absyn_Exp*
_tmp35A;struct Cyc_Absyn_Exp*_tmp35B;struct Cyc_Absyn_Exp*_tmp35C;void*_tmp35D;
struct Cyc_List_List*_tmp35E;struct Cyc_List_List _tmp35F;struct Cyc_Absyn_Exp*
_tmp360;struct Cyc_List_List*_tmp361;void*_tmp362;struct Cyc_List_List*_tmp363;
_LL21F: if(*((int*)_tmp353)!= 6)goto _LL221;_tmp354=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp353)->f1;_tmp355=((struct Cyc_Absyn_Conditional_e_struct*)_tmp353)->f2;
_tmp356=((struct Cyc_Absyn_Conditional_e_struct*)_tmp353)->f3;_LL220: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp365;union Cyc_CfFlowInfo_FlowInfo_union _tmp366;struct _tuple7 _tmp364=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp354);_tmp365=_tmp364.f1;_tmp366=_tmp364.f2;_tmp365=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp354->loc,_tmp365);_tmp366=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp354->loc,
_tmp366);{union Cyc_CfFlowInfo_FlowInfo_union _tmp368;union Cyc_CfFlowInfo_FlowInfo_union
_tmp369;struct _tuple7 _tmp367=Cyc_NewControlFlow_anal_test(env,_tmp365,_tmp355);
_tmp368=_tmp367.f1;_tmp369=_tmp367.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp36B;
union Cyc_CfFlowInfo_FlowInfo_union _tmp36C;struct _tuple7 _tmp36A=Cyc_NewControlFlow_anal_test(
env,_tmp366,_tmp356);_tmp36B=_tmp36A.f1;_tmp36C=_tmp36A.f2;return({struct _tuple7
_tmp36D;_tmp36D.f1=Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp368,_tmp36B,1);
_tmp36D.f2=Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp369,_tmp36C,1);_tmp36D;});}}}
_LL221: if(*((int*)_tmp353)!= 7)goto _LL223;_tmp357=((struct Cyc_Absyn_And_e_struct*)
_tmp353)->f1;_tmp358=((struct Cyc_Absyn_And_e_struct*)_tmp353)->f2;_LL222: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp36F;union Cyc_CfFlowInfo_FlowInfo_union _tmp370;
struct _tuple7 _tmp36E=Cyc_NewControlFlow_anal_test(env,inflow,_tmp357);_tmp36F=
_tmp36E.f1;_tmp370=_tmp36E.f2;_tmp36F=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp357->loc,_tmp36F);_tmp370=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp357->loc,
_tmp370);{union Cyc_CfFlowInfo_FlowInfo_union _tmp372;union Cyc_CfFlowInfo_FlowInfo_union
_tmp373;struct _tuple7 _tmp371=Cyc_NewControlFlow_anal_test(env,_tmp36F,_tmp358);
_tmp372=_tmp371.f1;_tmp373=_tmp371.f2;_tmp372=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp358->loc,_tmp372);_tmp373=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp358->loc,
_tmp373);return({struct _tuple7 _tmp374;_tmp374.f1=_tmp372;_tmp374.f2=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp370,_tmp373,0);_tmp374;});}}_LL223: if(*((int*)_tmp353)!= 8)
goto _LL225;_tmp359=((struct Cyc_Absyn_Or_e_struct*)_tmp353)->f1;_tmp35A=((struct
Cyc_Absyn_Or_e_struct*)_tmp353)->f2;_LL224: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp376;union Cyc_CfFlowInfo_FlowInfo_union _tmp377;struct _tuple7 _tmp375=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp359);_tmp376=_tmp375.f1;_tmp377=_tmp375.f2;_tmp376=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp359->loc,_tmp376);_tmp377=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp359->loc,
_tmp377);{union Cyc_CfFlowInfo_FlowInfo_union _tmp379;union Cyc_CfFlowInfo_FlowInfo_union
_tmp37A;struct _tuple7 _tmp378=Cyc_NewControlFlow_anal_test(env,_tmp377,_tmp35A);
_tmp379=_tmp378.f1;_tmp37A=_tmp378.f2;_tmp379=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp35A->loc,_tmp379);_tmp37A=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp35A->loc,
_tmp37A);return({struct _tuple7 _tmp37B;_tmp37B.f1=Cyc_CfFlowInfo_join_flow(env->all_changed,
_tmp376,_tmp379,0);_tmp37B.f2=_tmp37A;_tmp37B;});}}_LL225: if(*((int*)_tmp353)!= 
9)goto _LL227;_tmp35B=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp353)->f1;_tmp35C=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp353)->f2;_LL226: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp37D;void*_tmp37E;struct _tuple5 _tmp37C=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp35B);_tmp37D=_tmp37C.f1;_tmp37E=_tmp37C.f2;_tmp37D=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp35B->loc,_tmp37D);return Cyc_NewControlFlow_anal_test(env,_tmp37D,_tmp35C);}
_LL227: if(*((int*)_tmp353)!= 3)goto _LL229;_tmp35D=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp353)->f1;if((int)_tmp35D != 11)goto _LL229;_tmp35E=((struct Cyc_Absyn_Primop_e_struct*)
_tmp353)->f2;if(_tmp35E == 0)goto _LL229;_tmp35F=*_tmp35E;_tmp360=(struct Cyc_Absyn_Exp*)
_tmp35F.hd;_tmp361=_tmp35F.tl;if(_tmp361 != 0)goto _LL229;_LL228: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp380;union Cyc_CfFlowInfo_FlowInfo_union _tmp381;struct _tuple7 _tmp37F=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp360);_tmp380=_tmp37F.f1;_tmp381=_tmp37F.f2;return({struct _tuple7
_tmp382;_tmp382.f1=_tmp381;_tmp382.f2=_tmp380;_tmp382;});}_LL229: if(*((int*)
_tmp353)!= 3)goto _LL22B;_tmp362=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp353)->f1;_tmp363=((struct Cyc_Absyn_Primop_e_struct*)_tmp353)->f2;_LL22A: {
void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo_union f;{struct _RegionHandle _tmp383=
_new_region("temp");struct _RegionHandle*temp=& _tmp383;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp385;struct Cyc_List_List*_tmp386;struct _tuple9
_tmp384=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp363,0);
_tmp385=_tmp384.f1;_tmp386=_tmp384.f2;r1=(void*)((struct Cyc_List_List*)
_check_null(_tmp386))->hd;r2=(void*)((struct Cyc_List_List*)_check_null(_tmp386->tl))->hd;
f=_tmp385;};_pop_region(temp);}{void*_tmp387=_tmp362;_LL22E: if((int)_tmp387 != 0)
goto _LL230;_LL22F: goto _LL231;_LL230: if((int)_tmp387 != 2)goto _LL232;_LL231: Cyc_CfFlowInfo_check_unique_rvals(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp363))->hd)->loc,f);
goto _LL22D;_LL232:;_LL233: f=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp363))->hd)->loc,f);goto _LL22D;_LL22D:;}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp388=f;struct Cyc_Dict_Dict _tmp389;struct Cyc_List_List*
_tmp38A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp38B;_LL235: if((_tmp388.BottomFL).tag
!= 0)goto _LL237;_LL236: return({struct _tuple7 _tmp38C;_tmp38C.f1=f;_tmp38C.f2=f;
_tmp38C;});_LL237: if((_tmp388.ReachableFL).tag != 1)goto _LL234;_tmp389=(_tmp388.ReachableFL).f1;
_tmp38A=(_tmp388.ReachableFL).f2;_tmp38B=(_tmp388.ReachableFL).f3;_LL238: {
struct Cyc_Absyn_Exp*_tmp38D=(struct Cyc_Absyn_Exp*)_tmp363->hd;struct Cyc_Absyn_Exp*
_tmp38E=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp363->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(_tmp389,r1)== (void*)0)({void*_tmp38F[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp363->hd)->loc,({const char*_tmp390="expression may not be initialized";
_tag_dynforward(_tmp390,sizeof(char),_get_zero_arr_size(_tmp390,34));}),
_tag_dynforward(_tmp38F,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(_tmp389,
r2)== (void*)0)({void*_tmp391[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp363->tl))->hd)->loc,({const char*_tmp392="expression may not be initialized";
_tag_dynforward(_tmp392,sizeof(char),_get_zero_arr_size(_tmp392,34));}),
_tag_dynforward(_tmp391,sizeof(void*),0));});if(_tmp362 == (void*)5  || _tmp362 == (
void*)6){struct _tuple0 _tmp394=({struct _tuple0 _tmp393;_tmp393.f1=r1;_tmp393.f2=r2;
_tmp393;});void*_tmp395;void*_tmp396;void*_tmp397;void*_tmp398;void*_tmp399;void*
_tmp39A;void*_tmp39B;void*_tmp39C;void*_tmp39D;void*_tmp39E;void*_tmp39F;void*
_tmp3A0;void*_tmp3A1;void*_tmp3A2;void*_tmp3A3;void*_tmp3A4;void*_tmp3A5;void*
_tmp3A6;void*_tmp3A7;void*_tmp3A8;_LL23A: _tmp395=_tmp394.f1;if(_tmp395 <= (void*)
3)goto _LL23C;if(*((int*)_tmp395)!= 0)goto _LL23C;_tmp396=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp395)->f1;_tmp397=_tmp394.f2;if((int)_tmp397 != 0)goto _LL23C;_LL23B: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3AA;union Cyc_CfFlowInfo_FlowInfo_union _tmp3AB;struct _tuple7 _tmp3A9=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp38D,_tmp396);_tmp3AA=_tmp3A9.f1;_tmp3AB=_tmp3A9.f2;{void*_tmp3AC=
_tmp362;_LL24F: if((int)_tmp3AC != 5)goto _LL251;_LL250: return({struct _tuple7
_tmp3AD;_tmp3AD.f1=_tmp3AB;_tmp3AD.f2=_tmp3AA;_tmp3AD;});_LL251: if((int)_tmp3AC
!= 6)goto _LL253;_LL252: return({struct _tuple7 _tmp3AE;_tmp3AE.f1=_tmp3AA;_tmp3AE.f2=
_tmp3AB;_tmp3AE;});_LL253:;_LL254:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp3AF=_cycalloc(sizeof(*_tmp3AF));_tmp3AF[0]=({struct Cyc_Core_Impossible_struct
_tmp3B0;_tmp3B0.tag=Cyc_Core_Impossible;_tmp3B0.f1=({const char*_tmp3B1="anal_test, zero-split";
_tag_dynforward(_tmp3B1,sizeof(char),_get_zero_arr_size(_tmp3B1,22));});_tmp3B0;});
_tmp3AF;}));_LL24E:;}}_LL23C: _tmp398=_tmp394.f1;if((int)_tmp398 != 0)goto _LL23E;
_tmp399=_tmp394.f2;if(_tmp399 <= (void*)3)goto _LL23E;if(*((int*)_tmp399)!= 0)goto
_LL23E;_tmp39A=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp399)->f1;
_LL23D: {union Cyc_CfFlowInfo_FlowInfo_union _tmp3B3;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3B4;struct _tuple7 _tmp3B2=Cyc_NewControlFlow_splitzero(env,f,f,_tmp38E,
_tmp39A);_tmp3B3=_tmp3B2.f1;_tmp3B4=_tmp3B2.f2;{void*_tmp3B5=_tmp362;_LL256: if((
int)_tmp3B5 != 5)goto _LL258;_LL257: return({struct _tuple7 _tmp3B6;_tmp3B6.f1=
_tmp3B4;_tmp3B6.f2=_tmp3B3;_tmp3B6;});_LL258: if((int)_tmp3B5 != 6)goto _LL25A;
_LL259: return({struct _tuple7 _tmp3B7;_tmp3B7.f1=_tmp3B3;_tmp3B7.f2=_tmp3B4;
_tmp3B7;});_LL25A:;_LL25B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8[0]=({struct Cyc_Core_Impossible_struct
_tmp3B9;_tmp3B9.tag=Cyc_Core_Impossible;_tmp3B9.f1=({const char*_tmp3BA="anal_test, zero-split";
_tag_dynforward(_tmp3BA,sizeof(char),_get_zero_arr_size(_tmp3BA,22));});_tmp3B9;});
_tmp3B8;}));_LL255:;}}_LL23E: _tmp39B=_tmp394.f1;if((int)_tmp39B != 0)goto _LL240;
_tmp39C=_tmp394.f2;if((int)_tmp39C != 0)goto _LL240;_LL23F: if(_tmp362 == (void*)5)
return({struct _tuple7 _tmp3BB;_tmp3BB.f1=f;_tmp3BB.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3BC;(_tmp3BC.BottomFL).tag=0;_tmp3BC;});
_tmp3BB;});else{return({struct _tuple7 _tmp3BD;_tmp3BD.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3BE;(_tmp3BE.BottomFL).tag=0;_tmp3BE;});
_tmp3BD.f2=f;_tmp3BD;});}_LL240: _tmp39D=_tmp394.f1;if((int)_tmp39D != 0)goto
_LL242;_tmp39E=_tmp394.f2;if((int)_tmp39E != 1)goto _LL242;_LL241: goto _LL243;
_LL242: _tmp39F=_tmp394.f1;if((int)_tmp39F != 0)goto _LL244;_tmp3A0=_tmp394.f2;if((
int)_tmp3A0 != 2)goto _LL244;_LL243: goto _LL245;_LL244: _tmp3A1=_tmp394.f1;if((int)
_tmp3A1 != 0)goto _LL246;_tmp3A2=_tmp394.f2;if(_tmp3A2 <= (void*)3)goto _LL246;if(*((
int*)_tmp3A2)!= 2)goto _LL246;_LL245: goto _LL247;_LL246: _tmp3A3=_tmp394.f1;if((int)
_tmp3A3 != 1)goto _LL248;_tmp3A4=_tmp394.f2;if((int)_tmp3A4 != 0)goto _LL248;_LL247:
goto _LL249;_LL248: _tmp3A5=_tmp394.f1;if((int)_tmp3A5 != 2)goto _LL24A;_tmp3A6=
_tmp394.f2;if((int)_tmp3A6 != 0)goto _LL24A;_LL249: goto _LL24B;_LL24A: _tmp3A7=
_tmp394.f1;if(_tmp3A7 <= (void*)3)goto _LL24C;if(*((int*)_tmp3A7)!= 2)goto _LL24C;
_tmp3A8=_tmp394.f2;if((int)_tmp3A8 != 0)goto _LL24C;_LL24B: if(_tmp362 == (void*)6)
return({struct _tuple7 _tmp3BF;_tmp3BF.f1=f;_tmp3BF.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3C0;(_tmp3C0.BottomFL).tag=0;_tmp3C0;});
_tmp3BF;});else{return({struct _tuple7 _tmp3C1;_tmp3C1.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3C2;(_tmp3C2.BottomFL).tag=0;_tmp3C2;});
_tmp3C1.f2=f;_tmp3C1;});}_LL24C:;_LL24D: goto _LL239;_LL239:;}{struct _tuple0
_tmp3C4=({struct _tuple0 _tmp3C3;_tmp3C3.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp38D->topt))->v);_tmp3C3.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp38E->topt))->v);_tmp3C3;});void*_tmp3C5;void*
_tmp3C6;void*_tmp3C7;void*_tmp3C8;void*_tmp3C9;void*_tmp3CA;_LL25D: _tmp3C5=
_tmp3C4.f1;if(_tmp3C5 <= (void*)4)goto _LL25F;if(*((int*)_tmp3C5)!= 5)goto _LL25F;
_tmp3C6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3C5)->f1;if((int)_tmp3C6 != 
1)goto _LL25F;_LL25E: goto _LL260;_LL25F: _tmp3C7=_tmp3C4.f2;if(_tmp3C7 <= (void*)4)
goto _LL261;if(*((int*)_tmp3C7)!= 5)goto _LL261;_tmp3C8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3C7)->f1;if((int)_tmp3C8 != 1)goto _LL261;_LL260: goto _LL262;_LL261: _tmp3C9=
_tmp3C4.f1;if(_tmp3C9 <= (void*)4)goto _LL263;if(*((int*)_tmp3C9)!= 17)goto _LL263;
_LL262: goto _LL264;_LL263: _tmp3CA=_tmp3C4.f2;if(_tmp3CA <= (void*)4)goto _LL265;if(*((
int*)_tmp3CA)!= 17)goto _LL265;_LL264: goto _LL25C;_LL265:;_LL266: return({struct
_tuple7 _tmp3CB;_tmp3CB.f1=f;_tmp3CB.f2=f;_tmp3CB;});_LL25C:;}{void*_tmp3CC=
_tmp362;_LL268: if((int)_tmp3CC != 5)goto _LL26A;_LL269: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3CD=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp38D,r1,(void*)5,r2);_tmp3CD=Cyc_NewControlFlow_if_tagcmp(
env,_tmp3CD,_tmp38E,r2,(void*)5,r1);return({struct _tuple7 _tmp3CE;_tmp3CE.f1=
_tmp3CD;_tmp3CE.f2=f;_tmp3CE;});}_LL26A: if((int)_tmp3CC != 6)goto _LL26C;_LL26B: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp3CF=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp38D,r1,(void*)5,r2);_tmp3CF=Cyc_NewControlFlow_if_tagcmp(env,_tmp3CF,_tmp38E,
r2,(void*)5,r1);return({struct _tuple7 _tmp3D0;_tmp3D0.f1=f;_tmp3D0.f2=_tmp3CF;
_tmp3D0;});}_LL26C: if((int)_tmp3CC != 7)goto _LL26E;_LL26D: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D1=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp38E,r2,(void*)8,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D2=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp38D,r1,(void*)10,r2);return({
struct _tuple7 _tmp3D3;_tmp3D3.f1=_tmp3D1;_tmp3D3.f2=_tmp3D2;_tmp3D3;});}_LL26E:
if((int)_tmp3CC != 9)goto _LL270;_LL26F: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D4=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp38E,r2,(void*)10,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D5=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp38D,r1,(void*)8,r2);return({
struct _tuple7 _tmp3D6;_tmp3D6.f1=_tmp3D4;_tmp3D6.f2=_tmp3D5;_tmp3D6;});}_LL270:
if((int)_tmp3CC != 8)goto _LL272;_LL271: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D7=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp38D,r1,(void*)8,r2);union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D8=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp38E,r2,(void*)10,r1);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D9=_tmp3D7;struct Cyc_Dict_Dict _tmp3DA;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3DB;_LL277: if((_tmp3D9.BottomFL).tag != 0)goto _LL279;_LL278:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));_tmp3DC[0]=({
struct Cyc_Core_Impossible_struct _tmp3DD;_tmp3DD.tag=Cyc_Core_Impossible;_tmp3DD.f1=({
const char*_tmp3DE="anal_test, Lt";_tag_dynforward(_tmp3DE,sizeof(char),
_get_zero_arr_size(_tmp3DE,14));});_tmp3DD;});_tmp3DC;}));_LL279: if((_tmp3D9.ReachableFL).tag
!= 1)goto _LL276;_tmp3DA=(_tmp3D9.ReachableFL).f1;_tmp3DB=(_tmp3D9.ReachableFL).f3;
_LL27A: _tmp389=_tmp3DA;_tmp38B=_tmp3DB;_LL276:;}{void*_tmp3DF=(void*)_tmp38D->r;
void*_tmp3E0;struct Cyc_Absyn_Vardecl*_tmp3E1;void*_tmp3E2;struct Cyc_Absyn_Vardecl*
_tmp3E3;void*_tmp3E4;struct Cyc_Absyn_Vardecl*_tmp3E5;void*_tmp3E6;struct Cyc_Absyn_Vardecl*
_tmp3E7;_LL27C: if(*((int*)_tmp3DF)!= 1)goto _LL27E;_tmp3E0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3DF)->f2;if(_tmp3E0 <= (void*)1)goto _LL27E;if(*((int*)_tmp3E0)!= 0)goto _LL27E;
_tmp3E1=((struct Cyc_Absyn_Global_b_struct*)_tmp3E0)->f1;if(!(!_tmp3E1->escapes))
goto _LL27E;_LL27D: _tmp3E3=_tmp3E1;goto _LL27F;_LL27E: if(*((int*)_tmp3DF)!= 1)goto
_LL280;_tmp3E2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3DF)->f2;if(_tmp3E2 <= (
void*)1)goto _LL280;if(*((int*)_tmp3E2)!= 3)goto _LL280;_tmp3E3=((struct Cyc_Absyn_Local_b_struct*)
_tmp3E2)->f1;if(!(!_tmp3E3->escapes))goto _LL280;_LL27F: _tmp3E5=_tmp3E3;goto
_LL281;_LL280: if(*((int*)_tmp3DF)!= 1)goto _LL282;_tmp3E4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3DF)->f2;if(_tmp3E4 <= (void*)1)goto _LL282;if(*((int*)_tmp3E4)!= 4)goto _LL282;
_tmp3E5=((struct Cyc_Absyn_Pat_b_struct*)_tmp3E4)->f1;if(!(!_tmp3E5->escapes))
goto _LL282;_LL281: _tmp3E7=_tmp3E5;goto _LL283;_LL282: if(*((int*)_tmp3DF)!= 1)goto
_LL284;_tmp3E6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3DF)->f2;if(_tmp3E6 <= (
void*)1)goto _LL284;if(*((int*)_tmp3E6)!= 2)goto _LL284;_tmp3E7=((struct Cyc_Absyn_Param_b_struct*)
_tmp3E6)->f1;if(!(!_tmp3E7->escapes))goto _LL284;_LL283: {void*_tmp3E8=(void*)
_tmp38E->r;void*_tmp3E9;struct Cyc_Absyn_Vardecl*_tmp3EA;void*_tmp3EB;struct Cyc_Absyn_Vardecl*
_tmp3EC;void*_tmp3ED;struct Cyc_Absyn_Vardecl*_tmp3EE;void*_tmp3EF;struct Cyc_Absyn_Vardecl*
_tmp3F0;union Cyc_Absyn_Cnst_union _tmp3F1;int _tmp3F2;void*_tmp3F3;struct Cyc_List_List*
_tmp3F4;struct Cyc_List_List _tmp3F5;struct Cyc_Absyn_Exp*_tmp3F6;_LL287: if(*((int*)
_tmp3E8)!= 1)goto _LL289;_tmp3E9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3E8)->f2;
if(_tmp3E9 <= (void*)1)goto _LL289;if(*((int*)_tmp3E9)!= 0)goto _LL289;_tmp3EA=((
struct Cyc_Absyn_Global_b_struct*)_tmp3E9)->f1;if(!(!_tmp3EA->escapes))goto _LL289;
_LL288: _tmp3EC=_tmp3EA;goto _LL28A;_LL289: if(*((int*)_tmp3E8)!= 1)goto _LL28B;
_tmp3EB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3E8)->f2;if(_tmp3EB <= (void*)
1)goto _LL28B;if(*((int*)_tmp3EB)!= 3)goto _LL28B;_tmp3EC=((struct Cyc_Absyn_Local_b_struct*)
_tmp3EB)->f1;if(!(!_tmp3EC->escapes))goto _LL28B;_LL28A: _tmp3EE=_tmp3EC;goto
_LL28C;_LL28B: if(*((int*)_tmp3E8)!= 1)goto _LL28D;_tmp3ED=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3E8)->f2;if(_tmp3ED <= (void*)1)goto _LL28D;if(*((int*)_tmp3ED)!= 4)goto _LL28D;
_tmp3EE=((struct Cyc_Absyn_Pat_b_struct*)_tmp3ED)->f1;if(!(!_tmp3EE->escapes))
goto _LL28D;_LL28C: _tmp3F0=_tmp3EE;goto _LL28E;_LL28D: if(*((int*)_tmp3E8)!= 1)goto
_LL28F;_tmp3EF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3E8)->f2;if(_tmp3EF <= (
void*)1)goto _LL28F;if(*((int*)_tmp3EF)!= 2)goto _LL28F;_tmp3F0=((struct Cyc_Absyn_Param_b_struct*)
_tmp3EF)->f1;if(!(!_tmp3F0->escapes))goto _LL28F;_LL28E: {struct Cyc_List_List*
_tmp3F7=({struct Cyc_List_List*_tmp3FA=_cycalloc(sizeof(*_tmp3FA));_tmp3FA->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));_tmp3FB->vd=
_tmp3E7;_tmp3FB->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp3FC;(_tmp3FC.LessVar).tag=1;(_tmp3FC.LessVar).f1=_tmp3F0;_tmp3FC;});_tmp3FB;});
_tmp3FA->tl=_tmp38A;_tmp3FA;});return({struct _tuple7 _tmp3F8;_tmp3F8.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3F9;(_tmp3F9.ReachableFL).tag=1;(_tmp3F9.ReachableFL).f1=
_tmp389;(_tmp3F9.ReachableFL).f2=_tmp3F7;(_tmp3F9.ReachableFL).f3=_tmp38B;
_tmp3F9;});_tmp3F8.f2=_tmp3D8;_tmp3F8;});}_LL28F: if(*((int*)_tmp3E8)!= 0)goto
_LL291;_tmp3F1=((struct Cyc_Absyn_Const_e_struct*)_tmp3E8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp3E8)->f1).Int_c).tag != 2)goto _LL291;_tmp3F2=(_tmp3F1.Int_c).f2;_LL290: {
struct Cyc_List_List*_tmp3FD=({struct Cyc_List_List*_tmp400=_cycalloc(sizeof(*
_tmp400));_tmp400->hd=({struct Cyc_CfFlowInfo_Reln*_tmp401=_cycalloc(sizeof(*
_tmp401));_tmp401->vd=_tmp3E7;_tmp401->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp402;(_tmp402.LessConst).tag=3;(_tmp402.LessConst).f1=(
unsigned int)_tmp3F2;_tmp402;});_tmp401;});_tmp400->tl=_tmp38A;_tmp400;});return({
struct _tuple7 _tmp3FE;_tmp3FE.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp3FF;(_tmp3FF.ReachableFL).tag=1;(_tmp3FF.ReachableFL).f1=_tmp389;(_tmp3FF.ReachableFL).f2=
_tmp3FD;(_tmp3FF.ReachableFL).f3=_tmp38B;_tmp3FF;});_tmp3FE.f2=_tmp3D8;_tmp3FE;});}
_LL291: if(*((int*)_tmp3E8)!= 3)goto _LL293;_tmp3F3=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3E8)->f1;if((int)_tmp3F3 != 19)goto _LL293;_tmp3F4=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3E8)->f2;if(_tmp3F4 == 0)goto _LL293;_tmp3F5=*_tmp3F4;_tmp3F6=(struct Cyc_Absyn_Exp*)
_tmp3F5.hd;_LL292: {void*_tmp403=(void*)_tmp3F6->r;void*_tmp404;struct Cyc_Absyn_Vardecl*
_tmp405;void*_tmp406;struct Cyc_Absyn_Vardecl*_tmp407;void*_tmp408;struct Cyc_Absyn_Vardecl*
_tmp409;void*_tmp40A;struct Cyc_Absyn_Vardecl*_tmp40B;_LL296: if(*((int*)_tmp403)
!= 1)goto _LL298;_tmp404=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp403)->f2;if(
_tmp404 <= (void*)1)goto _LL298;if(*((int*)_tmp404)!= 0)goto _LL298;_tmp405=((
struct Cyc_Absyn_Global_b_struct*)_tmp404)->f1;if(!(!_tmp405->escapes))goto _LL298;
_LL297: _tmp407=_tmp405;goto _LL299;_LL298: if(*((int*)_tmp403)!= 1)goto _LL29A;
_tmp406=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp403)->f2;if(_tmp406 <= (void*)
1)goto _LL29A;if(*((int*)_tmp406)!= 3)goto _LL29A;_tmp407=((struct Cyc_Absyn_Local_b_struct*)
_tmp406)->f1;if(!(!_tmp407->escapes))goto _LL29A;_LL299: _tmp409=_tmp407;goto
_LL29B;_LL29A: if(*((int*)_tmp403)!= 1)goto _LL29C;_tmp408=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp403)->f2;if(_tmp408 <= (void*)1)goto _LL29C;if(*((int*)_tmp408)!= 4)goto _LL29C;
_tmp409=((struct Cyc_Absyn_Pat_b_struct*)_tmp408)->f1;if(!(!_tmp409->escapes))
goto _LL29C;_LL29B: _tmp40B=_tmp409;goto _LL29D;_LL29C: if(*((int*)_tmp403)!= 1)goto
_LL29E;_tmp40A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp403)->f2;if(_tmp40A <= (
void*)1)goto _LL29E;if(*((int*)_tmp40A)!= 2)goto _LL29E;_tmp40B=((struct Cyc_Absyn_Param_b_struct*)
_tmp40A)->f1;if(!(!_tmp40B->escapes))goto _LL29E;_LL29D: {struct Cyc_List_List*
_tmp40C=({struct Cyc_List_List*_tmp40F=_cycalloc(sizeof(*_tmp40F));_tmp40F->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp410=_cycalloc(sizeof(*_tmp410));_tmp410->vd=
_tmp3E7;_tmp410->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp411;(_tmp411.LessSize).tag=2;(_tmp411.LessSize).f1=_tmp40B;_tmp411;});
_tmp410;});_tmp40F->tl=_tmp38A;_tmp40F;});return({struct _tuple7 _tmp40D;_tmp40D.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp40E;(
_tmp40E.ReachableFL).tag=1;(_tmp40E.ReachableFL).f1=_tmp389;(_tmp40E.ReachableFL).f2=
_tmp40C;(_tmp40E.ReachableFL).f3=_tmp38B;_tmp40E;});_tmp40D.f2=_tmp3D8;_tmp40D;});}
_LL29E:;_LL29F: return({struct _tuple7 _tmp412;_tmp412.f1=_tmp3D7;_tmp412.f2=
_tmp3D8;_tmp412;});_LL295:;}_LL293:;_LL294: return({struct _tuple7 _tmp413;_tmp413.f1=
_tmp3D7;_tmp413.f2=_tmp3D8;_tmp413;});_LL286:;}_LL284:;_LL285: return({struct
_tuple7 _tmp414;_tmp414.f1=_tmp3D7;_tmp414.f2=_tmp3D8;_tmp414;});_LL27B:;}}_LL272:
if((int)_tmp3CC != 10)goto _LL274;_LL273: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp415=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp38D,r1,(void*)10,r2);union Cyc_CfFlowInfo_FlowInfo_union
_tmp416=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp38E,r1,(void*)8,r1);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp417=_tmp415;struct Cyc_Dict_Dict _tmp418;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp419;_LL2A1: if((_tmp417.BottomFL).tag != 0)goto _LL2A3;_LL2A2:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp41A=_cycalloc(sizeof(*_tmp41A));_tmp41A[0]=({
struct Cyc_Core_Impossible_struct _tmp41B;_tmp41B.tag=Cyc_Core_Impossible;_tmp41B.f1=({
const char*_tmp41C="anal_test, Lte";_tag_dynforward(_tmp41C,sizeof(char),
_get_zero_arr_size(_tmp41C,15));});_tmp41B;});_tmp41A;}));_LL2A3: if((_tmp417.ReachableFL).tag
!= 1)goto _LL2A0;_tmp418=(_tmp417.ReachableFL).f1;_tmp419=(_tmp417.ReachableFL).f3;
_LL2A4: _tmp389=_tmp418;_tmp38B=_tmp419;_LL2A0:;}{void*_tmp41D=(void*)_tmp38D->r;
void*_tmp41E;struct Cyc_Absyn_Vardecl*_tmp41F;void*_tmp420;struct Cyc_Absyn_Vardecl*
_tmp421;void*_tmp422;struct Cyc_Absyn_Vardecl*_tmp423;void*_tmp424;struct Cyc_Absyn_Vardecl*
_tmp425;_LL2A6: if(*((int*)_tmp41D)!= 1)goto _LL2A8;_tmp41E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp41D)->f2;if(_tmp41E <= (void*)1)goto _LL2A8;if(*((int*)_tmp41E)!= 0)goto _LL2A8;
_tmp41F=((struct Cyc_Absyn_Global_b_struct*)_tmp41E)->f1;if(!(!_tmp41F->escapes))
goto _LL2A8;_LL2A7: _tmp421=_tmp41F;goto _LL2A9;_LL2A8: if(*((int*)_tmp41D)!= 1)goto
_LL2AA;_tmp420=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp41D)->f2;if(_tmp420 <= (
void*)1)goto _LL2AA;if(*((int*)_tmp420)!= 3)goto _LL2AA;_tmp421=((struct Cyc_Absyn_Local_b_struct*)
_tmp420)->f1;if(!(!_tmp421->escapes))goto _LL2AA;_LL2A9: _tmp423=_tmp421;goto
_LL2AB;_LL2AA: if(*((int*)_tmp41D)!= 1)goto _LL2AC;_tmp422=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp41D)->f2;if(_tmp422 <= (void*)1)goto _LL2AC;if(*((int*)_tmp422)!= 4)goto _LL2AC;
_tmp423=((struct Cyc_Absyn_Pat_b_struct*)_tmp422)->f1;if(!(!_tmp423->escapes))
goto _LL2AC;_LL2AB: _tmp425=_tmp423;goto _LL2AD;_LL2AC: if(*((int*)_tmp41D)!= 1)goto
_LL2AE;_tmp424=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp41D)->f2;if(_tmp424 <= (
void*)1)goto _LL2AE;if(*((int*)_tmp424)!= 2)goto _LL2AE;_tmp425=((struct Cyc_Absyn_Param_b_struct*)
_tmp424)->f1;if(!(!_tmp425->escapes))goto _LL2AE;_LL2AD: {void*_tmp426=(void*)
_tmp38E->r;void*_tmp427;struct Cyc_List_List*_tmp428;struct Cyc_List_List _tmp429;
struct Cyc_Absyn_Exp*_tmp42A;_LL2B1: if(*((int*)_tmp426)!= 3)goto _LL2B3;_tmp427=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp426)->f1;if((int)_tmp427 != 19)goto
_LL2B3;_tmp428=((struct Cyc_Absyn_Primop_e_struct*)_tmp426)->f2;if(_tmp428 == 0)
goto _LL2B3;_tmp429=*_tmp428;_tmp42A=(struct Cyc_Absyn_Exp*)_tmp429.hd;_LL2B2: {
void*_tmp42B=(void*)_tmp42A->r;void*_tmp42C;struct Cyc_Absyn_Vardecl*_tmp42D;void*
_tmp42E;struct Cyc_Absyn_Vardecl*_tmp42F;void*_tmp430;struct Cyc_Absyn_Vardecl*
_tmp431;void*_tmp432;struct Cyc_Absyn_Vardecl*_tmp433;_LL2B6: if(*((int*)_tmp42B)
!= 1)goto _LL2B8;_tmp42C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp42B)->f2;if(
_tmp42C <= (void*)1)goto _LL2B8;if(*((int*)_tmp42C)!= 0)goto _LL2B8;_tmp42D=((
struct Cyc_Absyn_Global_b_struct*)_tmp42C)->f1;if(!(!_tmp42D->escapes))goto _LL2B8;
_LL2B7: _tmp42F=_tmp42D;goto _LL2B9;_LL2B8: if(*((int*)_tmp42B)!= 1)goto _LL2BA;
_tmp42E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp42B)->f2;if(_tmp42E <= (void*)
1)goto _LL2BA;if(*((int*)_tmp42E)!= 3)goto _LL2BA;_tmp42F=((struct Cyc_Absyn_Local_b_struct*)
_tmp42E)->f1;if(!(!_tmp42F->escapes))goto _LL2BA;_LL2B9: _tmp431=_tmp42F;goto
_LL2BB;_LL2BA: if(*((int*)_tmp42B)!= 1)goto _LL2BC;_tmp430=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp42B)->f2;if(_tmp430 <= (void*)1)goto _LL2BC;if(*((int*)_tmp430)!= 4)goto _LL2BC;
_tmp431=((struct Cyc_Absyn_Pat_b_struct*)_tmp430)->f1;if(!(!_tmp431->escapes))
goto _LL2BC;_LL2BB: _tmp433=_tmp431;goto _LL2BD;_LL2BC: if(*((int*)_tmp42B)!= 1)goto
_LL2BE;_tmp432=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp42B)->f2;if(_tmp432 <= (
void*)1)goto _LL2BE;if(*((int*)_tmp432)!= 2)goto _LL2BE;_tmp433=((struct Cyc_Absyn_Param_b_struct*)
_tmp432)->f1;if(!(!_tmp433->escapes))goto _LL2BE;_LL2BD: {struct Cyc_List_List*
_tmp434=({struct Cyc_List_List*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->vd=
_tmp425;_tmp438->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp439;(_tmp439.LessEqSize).tag=4;(_tmp439.LessEqSize).f1=_tmp433;_tmp439;});
_tmp438;});_tmp437->tl=_tmp38A;_tmp437;});return({struct _tuple7 _tmp435;_tmp435.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp436;(
_tmp436.ReachableFL).tag=1;(_tmp436.ReachableFL).f1=_tmp389;(_tmp436.ReachableFL).f2=
_tmp434;(_tmp436.ReachableFL).f3=_tmp38B;_tmp436;});_tmp435.f2=_tmp416;_tmp435;});}
_LL2BE:;_LL2BF: return({struct _tuple7 _tmp43A;_tmp43A.f1=_tmp415;_tmp43A.f2=
_tmp416;_tmp43A;});_LL2B5:;}_LL2B3:;_LL2B4: return({struct _tuple7 _tmp43B;_tmp43B.f1=
_tmp415;_tmp43B.f2=_tmp416;_tmp43B;});_LL2B0:;}_LL2AE:;_LL2AF: return({struct
_tuple7 _tmp43C;_tmp43C.f1=_tmp415;_tmp43C.f2=_tmp416;_tmp43C;});_LL2A5:;}}_LL274:;
_LL275: return({struct _tuple7 _tmp43D;_tmp43D.f1=f;_tmp43D.f2=f;_tmp43D;});_LL267:;}}
_LL234:;}}_LL22B:;_LL22C: goto _LL21E;_LL21E:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp43F;void*_tmp440;struct _tuple5 _tmp43E=Cyc_NewControlFlow_anal_Rexp(env,
inflow,e);_tmp43F=_tmp43E.f1;_tmp440=_tmp43E.f2;_tmp43F=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp43F);{union Cyc_CfFlowInfo_FlowInfo_union _tmp441=_tmp43F;struct Cyc_Dict_Dict
_tmp442;_LL2C1: if((_tmp441.BottomFL).tag != 0)goto _LL2C3;_LL2C2: return({struct
_tuple7 _tmp443;_tmp443.f1=_tmp43F;_tmp443.f2=_tmp43F;_tmp443;});_LL2C3: if((
_tmp441.ReachableFL).tag != 1)goto _LL2C0;_tmp442=(_tmp441.ReachableFL).f1;_LL2C4: {
void*_tmp444=_tmp440;void*_tmp445;void*_tmp446;void*_tmp447;_LL2C6: if((int)
_tmp444 != 0)goto _LL2C8;_LL2C7: return({struct _tuple7 _tmp448;_tmp448.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp449;(_tmp449.BottomFL).tag=0;_tmp449;});
_tmp448.f2=_tmp43F;_tmp448;});_LL2C8: if((int)_tmp444 != 2)goto _LL2CA;_LL2C9: goto
_LL2CB;_LL2CA: if((int)_tmp444 != 1)goto _LL2CC;_LL2CB: goto _LL2CD;_LL2CC: if(_tmp444
<= (void*)3)goto _LL2CE;if(*((int*)_tmp444)!= 2)goto _LL2CE;_LL2CD: return({struct
_tuple7 _tmp44A;_tmp44A.f1=_tmp43F;_tmp44A.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp44B;(_tmp44B.BottomFL).tag=0;_tmp44B;});
_tmp44A;});_LL2CE: if(_tmp444 <= (void*)3)goto _LL2D0;if(*((int*)_tmp444)!= 0)goto
_LL2D0;_tmp445=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp444)->f1;if((
int)_tmp445 != 0)goto _LL2D0;_LL2CF: goto _LL2D1;_LL2D0: if(_tmp444 <= (void*)3)goto
_LL2D2;if(*((int*)_tmp444)!= 1)goto _LL2D2;_tmp446=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp444)->f1;if((int)_tmp446 != 0)goto _LL2D2;_LL2D1:({void*_tmp44C[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp44D="expression may not be initialized";_tag_dynforward(
_tmp44D,sizeof(char),_get_zero_arr_size(_tmp44D,34));}),_tag_dynforward(_tmp44C,
sizeof(void*),0));});return({struct _tuple7 _tmp44E;_tmp44E.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp450;(_tmp450.BottomFL).tag=0;_tmp450;});
_tmp44E.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp44F;(_tmp44F.BottomFL).tag=0;_tmp44F;});_tmp44E;});_LL2D2: if(_tmp444 <= (void*)
3)goto _LL2D4;if(*((int*)_tmp444)!= 0)goto _LL2D4;_tmp447=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp444)->f1;_LL2D3: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp43F,e,
_tmp447);_LL2D4: if(_tmp444 <= (void*)3)goto _LL2D6;if(*((int*)_tmp444)!= 1)goto
_LL2D6;_LL2D5: return({struct _tuple7 _tmp451;_tmp451.f1=_tmp43F;_tmp451.f2=_tmp43F;
_tmp451;});_LL2D6: if(_tmp444 <= (void*)3)goto _LL2D8;if(*((int*)_tmp444)!= 3)goto
_LL2D8;_LL2D7: return({struct _tuple7 _tmp452;_tmp452.f1=_tmp43F;_tmp452.f2=_tmp43F;
_tmp452;});_LL2D8: if(_tmp444 <= (void*)3)goto _LL2C5;if(*((int*)_tmp444)!= 4)goto
_LL2C5;_LL2D9:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp453=
_cycalloc(sizeof(*_tmp453));_tmp453[0]=({struct Cyc_Core_Impossible_struct _tmp454;
_tmp454.tag=Cyc_Core_Impossible;_tmp454.f1=({const char*_tmp455="anal_test";
_tag_dynforward(_tmp455,sizeof(char),_get_zero_arr_size(_tmp455,10));});_tmp454;});
_tmp453;}));_LL2C5:;}_LL2C0:;}}}static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
flow){union Cyc_CfFlowInfo_FlowInfo_union _tmp456=flow;struct Cyc_Dict_Dict _tmp457;
_LL2DB: if((_tmp456.BottomFL).tag != 0)goto _LL2DD;_LL2DC: return;_LL2DD: if((_tmp456.ReachableFL).tag
!= 1)goto _LL2DA;_tmp457=(_tmp456.ReachableFL).f1;_LL2DE:{struct Cyc_List_List*
_tmp458=env->param_roots;for(0;_tmp458 != 0;_tmp458=_tmp458->tl){if(Cyc_CfFlowInfo_initlevel(
_tmp457,Cyc_CfFlowInfo_lookup_place(_tmp457,(struct Cyc_CfFlowInfo_Place*)_tmp458->hd))
!= (void*)2)({void*_tmp459[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp45A="function may not initialize all the parameters with attribute 'initializes'";
_tag_dynforward(_tmp45A,sizeof(char),_get_zero_arr_size(_tmp45A,76));}),
_tag_dynforward(_tmp459,sizeof(void*),0));});}}return;_LL2DA:;}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause
_tmp45C;struct Cyc_Core_Opt*_tmp45D;struct Cyc_Absyn_Exp*_tmp45E;struct Cyc_Absyn_Stmt*
_tmp45F;struct Cyc_Position_Segment*_tmp460;struct Cyc_Absyn_Switch_clause*_tmp45B=(
struct Cyc_Absyn_Switch_clause*)scs->hd;_tmp45C=*_tmp45B;_tmp45D=_tmp45C.pat_vars;
_tmp45E=_tmp45C.where_clause;_tmp45F=_tmp45C.body;_tmp460=_tmp45C.loc;{union Cyc_CfFlowInfo_FlowInfo_union
clause_inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp45D))->v,Cyc_CfFlowInfo_unknown_all,_tmp460);union
Cyc_CfFlowInfo_FlowInfo_union clause_outflow;if(_tmp45E != 0){union Cyc_CfFlowInfo_FlowInfo_union
_tmp462;union Cyc_CfFlowInfo_FlowInfo_union _tmp463;struct _tuple7 _tmp461=Cyc_NewControlFlow_anal_test(
env,clause_inflow,(struct Cyc_Absyn_Exp*)_tmp45E);_tmp462=_tmp461.f1;_tmp463=
_tmp461.f2;inflow=_tmp463;clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
_tmp462,_tmp45F);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp45F);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp464=
clause_outflow;_LL2E0: if((_tmp464.BottomFL).tag != 0)goto _LL2E2;_LL2E1: goto _LL2DF;
_LL2E2:;_LL2E3: if(scs->tl == 0)return clause_outflow;else{if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->pat_vars))->v != 0)({void*_tmp465[0]={};Cyc_Tcutil_terr(
_tmp45F->loc,({const char*_tmp466="switch clause may implicitly fallthru";
_tag_dynforward(_tmp466,sizeof(char),_get_zero_arr_size(_tmp466,38));}),
_tag_dynforward(_tmp465,sizeof(void*),0));});else{({void*_tmp467[0]={};Cyc_Tcutil_warn(
_tmp45F->loc,({const char*_tmp468="switch clause may implicitly fallthru";
_tag_dynforward(_tmp468,sizeof(char),_get_zero_arr_size(_tmp468,38));}),
_tag_dynforward(_tmp467,sizeof(void*),0));});}Cyc_NewControlFlow_update_flow(env,((
struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL2DF;_LL2DF:;}}}return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp469;(_tmp469.BottomFL).tag=0;_tmp469;});}
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union
outflow;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp46A=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);inflow=_tmp46A->flow;{void*_tmp46B=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp46C;struct Cyc_Absyn_Exp*_tmp46D;struct Cyc_Absyn_Exp*_tmp46E;struct Cyc_Absyn_Stmt*
_tmp46F;struct Cyc_Absyn_Stmt*_tmp470;struct Cyc_Absyn_Exp*_tmp471;struct Cyc_Absyn_Stmt*
_tmp472;struct Cyc_Absyn_Stmt*_tmp473;struct _tuple3 _tmp474;struct Cyc_Absyn_Exp*
_tmp475;struct Cyc_Absyn_Stmt*_tmp476;struct Cyc_Absyn_Stmt*_tmp477;struct Cyc_Absyn_Stmt*
_tmp478;struct _tuple3 _tmp479;struct Cyc_Absyn_Exp*_tmp47A;struct Cyc_Absyn_Stmt*
_tmp47B;struct Cyc_Absyn_Exp*_tmp47C;struct _tuple3 _tmp47D;struct Cyc_Absyn_Exp*
_tmp47E;struct Cyc_Absyn_Stmt*_tmp47F;struct _tuple3 _tmp480;struct Cyc_Absyn_Exp*
_tmp481;struct Cyc_Absyn_Stmt*_tmp482;struct Cyc_Absyn_Stmt*_tmp483;struct Cyc_Absyn_Stmt*
_tmp484;struct Cyc_List_List*_tmp485;struct Cyc_Absyn_Switch_clause**_tmp486;
struct Cyc_Absyn_Switch_clause*_tmp487;struct Cyc_Absyn_Stmt*_tmp488;struct Cyc_Absyn_Stmt*
_tmp489;struct Cyc_Absyn_Stmt*_tmp48A;struct Cyc_Absyn_Exp*_tmp48B;struct Cyc_List_List*
_tmp48C;struct Cyc_Absyn_Stmt*_tmp48D;struct Cyc_List_List*_tmp48E;struct Cyc_Absyn_Decl*
_tmp48F;struct Cyc_Absyn_Stmt*_tmp490;struct Cyc_Absyn_Stmt*_tmp491;struct Cyc_Absyn_Tvar*
_tmp492;struct Cyc_Absyn_Vardecl*_tmp493;int _tmp494;struct Cyc_Absyn_Stmt*_tmp495;
struct Cyc_Absyn_Exp*_tmp496;_LL2E5: if((int)_tmp46B != 0)goto _LL2E7;_LL2E6: return
inflow;_LL2E7: if(_tmp46B <= (void*)1)goto _LL30D;if(*((int*)_tmp46B)!= 2)goto
_LL2E9;_tmp46C=((struct Cyc_Absyn_Return_s_struct*)_tmp46B)->f1;if(_tmp46C != 0)
goto _LL2E9;_LL2E8: if(env->noreturn)({void*_tmp497[0]={};Cyc_Tcutil_terr(s->loc,({
const char*_tmp498="`noreturn' function might return";_tag_dynforward(_tmp498,
sizeof(char),_get_zero_arr_size(_tmp498,33));}),_tag_dynforward(_tmp497,sizeof(
void*),0));});Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp499;(
_tmp499.BottomFL).tag=0;_tmp499;});_LL2E9: if(*((int*)_tmp46B)!= 2)goto _LL2EB;
_tmp46D=((struct Cyc_Absyn_Return_s_struct*)_tmp46B)->f1;_LL2EA: if(env->noreturn)({
void*_tmp49A[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp49B="`noreturn' function might return";
_tag_dynforward(_tmp49B,sizeof(char),_get_zero_arr_size(_tmp49B,33));}),
_tag_dynforward(_tmp49A,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp49D;void*_tmp49E;struct _tuple5 _tmp49C=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp46D));_tmp49D=_tmp49C.f1;_tmp49E=
_tmp49C.f2;_tmp49D=Cyc_CfFlowInfo_consume_unique_rvals(_tmp46D->loc,_tmp49D);
_tmp49D=Cyc_NewControlFlow_use_Rval(env,_tmp46D->loc,_tmp49D,_tmp49E);Cyc_NewControlFlow_check_init_params(
s->loc,env,_tmp49D);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp49F;(_tmp49F.BottomFL).tag=0;_tmp49F;});}_LL2EB: if(*((int*)_tmp46B)!= 0)goto
_LL2ED;_tmp46E=((struct Cyc_Absyn_Exp_s_struct*)_tmp46B)->f1;_LL2EC: outflow=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp46E)).f1;goto _LL2E4;_LL2ED: if(*((int*)_tmp46B)!= 1)goto _LL2EF;
_tmp46F=((struct Cyc_Absyn_Seq_s_struct*)_tmp46B)->f1;_tmp470=((struct Cyc_Absyn_Seq_s_struct*)
_tmp46B)->f2;_LL2EE: outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp46F),_tmp470);goto _LL2E4;_LL2EF: if(*((int*)_tmp46B)!= 3)goto
_LL2F1;_tmp471=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp46B)->f1;_tmp472=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp46B)->f2;_tmp473=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp46B)->f3;_LL2F0: {union Cyc_CfFlowInfo_FlowInfo_union _tmp4A1;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4A2;struct _tuple7 _tmp4A0=Cyc_NewControlFlow_anal_test(env,inflow,_tmp471);
_tmp4A1=_tmp4A0.f1;_tmp4A2=_tmp4A0.f2;_tmp4A1=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp471->loc,_tmp4A1);_tmp4A2=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp471->loc,
_tmp4A2);outflow=Cyc_CfFlowInfo_join_flow(env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp4A1,_tmp472),Cyc_NewControlFlow_anal_stmt(env,_tmp4A2,_tmp473),1);goto
_LL2E4;}_LL2F1: if(*((int*)_tmp46B)!= 4)goto _LL2F3;_tmp474=((struct Cyc_Absyn_While_s_struct*)
_tmp46B)->f1;_tmp475=_tmp474.f1;_tmp476=_tmp474.f2;_tmp477=((struct Cyc_Absyn_While_s_struct*)
_tmp46B)->f2;_LL2F2: {union Cyc_CfFlowInfo_FlowInfo_union _tmp4A3=(Cyc_NewControlFlow_pre_stmt_check(
env,inflow,_tmp476))->flow;union Cyc_CfFlowInfo_FlowInfo_union _tmp4A5;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4A6;struct _tuple7 _tmp4A4=Cyc_NewControlFlow_anal_test(env,_tmp4A3,_tmp475);
_tmp4A5=_tmp4A4.f1;_tmp4A6=_tmp4A4.f2;_tmp4A5=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp475->loc,_tmp4A5);_tmp4A6=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp475->loc,
_tmp4A6);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4A7=Cyc_NewControlFlow_anal_stmt(
env,_tmp4A5,_tmp477);Cyc_NewControlFlow_update_flow(env,_tmp476,_tmp4A7);outflow=
_tmp4A6;goto _LL2E4;}}_LL2F3: if(*((int*)_tmp46B)!= 13)goto _LL2F5;_tmp478=((struct
Cyc_Absyn_Do_s_struct*)_tmp46B)->f1;_tmp479=((struct Cyc_Absyn_Do_s_struct*)
_tmp46B)->f2;_tmp47A=_tmp479.f1;_tmp47B=_tmp479.f2;_LL2F4: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp4A8=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp478);union Cyc_CfFlowInfo_FlowInfo_union
_tmp4A9=(Cyc_NewControlFlow_pre_stmt_check(env,_tmp4A8,_tmp47B))->flow;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4AB;union Cyc_CfFlowInfo_FlowInfo_union _tmp4AC;struct _tuple7 _tmp4AA=Cyc_NewControlFlow_anal_test(
env,_tmp4A9,_tmp47A);_tmp4AB=_tmp4AA.f1;_tmp4AC=_tmp4AA.f2;Cyc_NewControlFlow_update_flow(
env,_tmp478,_tmp4AB);outflow=_tmp4AC;goto _LL2E4;}_LL2F5: if(*((int*)_tmp46B)!= 8)
goto _LL2F7;_tmp47C=((struct Cyc_Absyn_For_s_struct*)_tmp46B)->f1;_tmp47D=((struct
Cyc_Absyn_For_s_struct*)_tmp46B)->f2;_tmp47E=_tmp47D.f1;_tmp47F=_tmp47D.f2;
_tmp480=((struct Cyc_Absyn_For_s_struct*)_tmp46B)->f3;_tmp481=_tmp480.f1;_tmp482=
_tmp480.f2;_tmp483=((struct Cyc_Absyn_For_s_struct*)_tmp46B)->f4;_LL2F6: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp4AD=(Cyc_NewControlFlow_anal_Rexp(env,inflow,
_tmp47C)).f1;_tmp4AD=Cyc_CfFlowInfo_drop_unique_rvals(_tmp47C->loc,_tmp4AD);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp4AE=(Cyc_NewControlFlow_pre_stmt_check(env,
_tmp4AD,_tmp47F))->flow;union Cyc_CfFlowInfo_FlowInfo_union _tmp4B0;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4B1;struct _tuple7 _tmp4AF=Cyc_NewControlFlow_anal_test(env,_tmp4AE,_tmp47E);
_tmp4B0=_tmp4AF.f1;_tmp4B1=_tmp4AF.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp4B2=
Cyc_NewControlFlow_anal_stmt(env,_tmp4B0,_tmp483);union Cyc_CfFlowInfo_FlowInfo_union
_tmp4B3=(Cyc_NewControlFlow_pre_stmt_check(env,_tmp4B2,_tmp482))->flow;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4B4=(Cyc_NewControlFlow_anal_Rexp(env,_tmp4B3,_tmp481)).f1;_tmp4B4=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp481->loc,_tmp4B4);Cyc_NewControlFlow_update_flow(env,_tmp47F,_tmp4B4);
outflow=_tmp4B1;goto _LL2E4;}}}_LL2F7: if(*((int*)_tmp46B)!= 5)goto _LL2F9;_tmp484=((
struct Cyc_Absyn_Break_s_struct*)_tmp46B)->f1;if(_tmp484 != 0)goto _LL2F9;_LL2F8:
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4B5;(_tmp4B5.BottomFL).tag=0;_tmp4B5;});_LL2F9: if(*((int*)_tmp46B)!= 10)goto
_LL2FB;_tmp485=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp46B)->f1;_tmp486=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp46B)->f2;if(_tmp486 == 0)goto _LL2FB;
_tmp487=*_tmp486;_LL2FA: {struct _RegionHandle _tmp4B6=_new_region("temp");struct
_RegionHandle*temp=& _tmp4B6;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp4B8;struct Cyc_List_List*_tmp4B9;struct _tuple9 _tmp4B7=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp485,0);_tmp4B8=_tmp4B7.f1;_tmp4B9=_tmp4B7.f2;for(0;_tmp4B9 != 
0;(_tmp4B9=_tmp4B9->tl,_tmp485=_tmp485->tl)){_tmp4B8=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp485))->hd)->loc,
_tmp4B8,(void*)_tmp4B9->hd);}_tmp4B8=Cyc_CfFlowInfo_consume_unique_rvals(s->loc,
_tmp4B8);_tmp4B8=Cyc_NewControlFlow_add_vars(_tmp4B8,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp487->pat_vars))->v,Cyc_CfFlowInfo_unknown_all,
s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp487->body,
_tmp4B8);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4BB=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4BA;(_tmp4BA.BottomFL).tag=0;_tmp4BA;});
_npop_handler(0);return _tmp4BB;}};_pop_region(temp);}_LL2FB: if(*((int*)_tmp46B)
!= 5)goto _LL2FD;_tmp488=((struct Cyc_Absyn_Break_s_struct*)_tmp46B)->f1;_LL2FC:
_tmp489=_tmp488;goto _LL2FE;_LL2FD: if(*((int*)_tmp46B)!= 6)goto _LL2FF;_tmp489=((
struct Cyc_Absyn_Continue_s_struct*)_tmp46B)->f1;_LL2FE: _tmp48A=_tmp489;goto
_LL300;_LL2FF: if(*((int*)_tmp46B)!= 7)goto _LL301;_tmp48A=((struct Cyc_Absyn_Goto_s_struct*)
_tmp46B)->f2;_LL300: if(env->iteration_num == 1){struct Cyc_Absyn_Stmt*_tmp4BC=
_tmp46A->encloser;struct Cyc_Absyn_Stmt*_tmp4BD=(Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)_check_null(_tmp48A)))->encloser;while(_tmp4BD != _tmp4BC){
struct Cyc_Absyn_Stmt*_tmp4BE=(Cyc_NewControlFlow_get_stmt_annot(_tmp4BC))->encloser;
if(_tmp4BE == _tmp4BC){({void*_tmp4BF[0]={};Cyc_Tcutil_terr(s->loc,({const char*
_tmp4C0="goto enters local scope or exception handler";_tag_dynforward(_tmp4C0,
sizeof(char),_get_zero_arr_size(_tmp4C0,45));}),_tag_dynforward(_tmp4BF,sizeof(
void*),0));});break;}_tmp4BC=_tmp4BE;}}Cyc_NewControlFlow_update_flow(env,(
struct Cyc_Absyn_Stmt*)_check_null(_tmp48A),inflow);return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4C1;(_tmp4C1.BottomFL).tag=0;_tmp4C1;});
_LL301: if(*((int*)_tmp46B)!= 9)goto _LL303;_tmp48B=((struct Cyc_Absyn_Switch_s_struct*)
_tmp46B)->f1;_tmp48C=((struct Cyc_Absyn_Switch_s_struct*)_tmp46B)->f2;_LL302: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp4C3;void*_tmp4C4;struct _tuple5 _tmp4C2=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp48B);_tmp4C3=_tmp4C2.f1;_tmp4C4=_tmp4C2.f2;_tmp4C3=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp48B->loc,_tmp4C3);_tmp4C3=Cyc_NewControlFlow_use_Rval(env,_tmp48B->loc,
_tmp4C3,_tmp4C4);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp4C3,_tmp48C);goto
_LL2E4;}_LL303: if(*((int*)_tmp46B)!= 14)goto _LL305;_tmp48D=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp46B)->f1;_tmp48E=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp46B)->f2;_LL304: {
int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo_union old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo_union s1_outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp48D);union Cyc_CfFlowInfo_FlowInfo_union
scs_inflow=env->tryflow;env->in_try=old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(
env,scs_inflow);{union Cyc_CfFlowInfo_FlowInfo_union scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp48E);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4C5=scs_outflow;
_LL310: if((_tmp4C5.BottomFL).tag != 0)goto _LL312;_LL311: goto _LL30F;_LL312:;_LL313:({
void*_tmp4C6[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp4C7="last catch clause may implicitly fallthru";
_tag_dynforward(_tmp4C7,sizeof(char),_get_zero_arr_size(_tmp4C7,42));}),
_tag_dynforward(_tmp4C6,sizeof(void*),0));});_LL30F:;}outflow=s1_outflow;goto
_LL2E4;}}}_LL305: if(*((int*)_tmp46B)!= 11)goto _LL307;_tmp48F=((struct Cyc_Absyn_Decl_s_struct*)
_tmp46B)->f1;_tmp490=((struct Cyc_Absyn_Decl_s_struct*)_tmp46B)->f2;_LL306:
outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,
_tmp48F),_tmp490);goto _LL2E4;_LL307: if(*((int*)_tmp46B)!= 12)goto _LL309;_tmp491=((
struct Cyc_Absyn_Label_s_struct*)_tmp46B)->f2;_LL308: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp491);goto _LL2E4;_LL309: if(*((int*)_tmp46B)!= 15)goto _LL30B;
_tmp492=((struct Cyc_Absyn_Region_s_struct*)_tmp46B)->f1;_tmp493=((struct Cyc_Absyn_Region_s_struct*)
_tmp46B)->f2;_tmp494=((struct Cyc_Absyn_Region_s_struct*)_tmp46B)->f3;_tmp495=((
struct Cyc_Absyn_Region_s_struct*)_tmp46B)->f4;_LL30A: {struct Cyc_List_List
_tmp4C8=({struct Cyc_List_List _tmp4C9;_tmp4C9.hd=_tmp493;_tmp4C9.tl=0;_tmp4C9;});
inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)& _tmp4C8,Cyc_CfFlowInfo_unknown_all,
_tmp495->loc);outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp495);goto
_LL2E4;}_LL30B: if(*((int*)_tmp46B)!= 16)goto _LL30D;_tmp496=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp46B)->f1;_LL30C: {union Cyc_CfFlowInfo_FlowInfo_union _tmp4CB;void*_tmp4CC;
struct _tuple5 _tmp4CA=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp496);_tmp4CB=
_tmp4CA.f1;_tmp4CC=_tmp4CA.f2;_tmp4CB=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp496->loc,_tmp4CB);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4CD=Cyc_NewControlFlow_use_Rval(
env,_tmp496->loc,_tmp4CB,_tmp4CC);{void*_tmp4CE=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp496->topt))->v);void*_tmp4CF;_LL315: if(
_tmp4CE <= (void*)4)goto _LL317;if(*((int*)_tmp4CE)!= 15)goto _LL317;_tmp4CF=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp4CE)->f1;_LL316: outflow=Cyc_CfFlowInfo_kill_flow_region(
_tmp4CB,_tmp4CF);goto _LL314;_LL317:;_LL318:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp4D0=_cycalloc(sizeof(*_tmp4D0));_tmp4D0[0]=({struct Cyc_Core_Impossible_struct
_tmp4D1;_tmp4D1.tag=Cyc_Core_Impossible;_tmp4D1.f1=({const char*_tmp4D2="anal_stmt -- reset_region";
_tag_dynforward(_tmp4D2,sizeof(char),_get_zero_arr_size(_tmp4D2,26));});_tmp4D1;});
_tmp4D0;}));_LL314:;}goto _LL2E4;}}_LL30D:;_LL30E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp4D3=_cycalloc(sizeof(*_tmp4D3));_tmp4D3[0]=({struct Cyc_Core_Impossible_struct
_tmp4D4;_tmp4D4.tag=Cyc_Core_Impossible;_tmp4D4.f1=({const char*_tmp4D5="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_dynforward(_tmp4D5,sizeof(char),_get_zero_arr_size(_tmp4D5,56));});_tmp4D4;});
_tmp4D3;}));_LL2E4:;}outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,outflow);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp4D6=outflow;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp4D7;_LL31A: if((_tmp4D6.ReachableFL).tag != 1)goto _LL31C;_tmp4D7=(_tmp4D6.ReachableFL).f3;
_LL31B: goto _LL319;_LL31C:;_LL31D: goto _LL319;_LL319:;}return outflow;}static void
Cyc_NewControlFlow_check_nested_fun(union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Fndecl*fd);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Decl*decl){void*_tmp4D8=(void*)decl->r;struct Cyc_Absyn_Vardecl*
_tmp4D9;struct Cyc_Core_Opt*_tmp4DA;struct Cyc_Core_Opt _tmp4DB;struct Cyc_List_List*
_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DD;struct Cyc_List_List*_tmp4DE;struct Cyc_Absyn_Fndecl*
_tmp4DF;_LL31F: if(_tmp4D8 <= (void*)2)goto _LL327;if(*((int*)_tmp4D8)!= 0)goto
_LL321;_tmp4D9=((struct Cyc_Absyn_Var_d_struct*)_tmp4D8)->f1;_LL320: {struct Cyc_List_List
_tmp4E0=({struct Cyc_List_List _tmp4EF;_tmp4EF.hd=_tmp4D9;_tmp4EF.tl=0;_tmp4EF;});
inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)& _tmp4E0,Cyc_CfFlowInfo_unknown_none,
decl->loc);{struct Cyc_Absyn_Exp*_tmp4E1=_tmp4D9->initializer;if(_tmp4E1 == 0)
return inflow;{union Cyc_CfFlowInfo_FlowInfo_union _tmp4E3;void*_tmp4E4;struct
_tuple5 _tmp4E2=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
_tmp4E1);_tmp4E3=_tmp4E2.f1;_tmp4E4=_tmp4E2.f2;_tmp4E3=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4E1->loc,_tmp4E3);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4E5=_tmp4E3;struct
Cyc_Dict_Dict _tmp4E6;struct Cyc_List_List*_tmp4E7;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp4E8;_LL32A: if((_tmp4E5.BottomFL).tag != 0)goto _LL32C;_LL32B: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4E9;(_tmp4E9.BottomFL).tag=0;_tmp4E9;});
_LL32C: if((_tmp4E5.ReachableFL).tag != 1)goto _LL329;_tmp4E6=(_tmp4E5.ReachableFL).f1;
_tmp4E7=(_tmp4E5.ReachableFL).f2;_tmp4E8=(_tmp4E5.ReachableFL).f3;_LL32D: _tmp4E6=
Cyc_CfFlowInfo_assign_place(decl->loc,_tmp4E6,env->all_changed,({struct Cyc_CfFlowInfo_Place*
_tmp4EA=_cycalloc(sizeof(*_tmp4EA));_tmp4EA->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp4EB=_cycalloc(sizeof(*_tmp4EB));_tmp4EB[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp4EC;_tmp4EC.tag=0;_tmp4EC.f1=_tmp4D9;_tmp4EC;});_tmp4EB;}));_tmp4EA->fields=
0;_tmp4EA;}),_tmp4E4);_tmp4E7=Cyc_CfFlowInfo_reln_assign_var(_tmp4E7,_tmp4D9,(
struct Cyc_Absyn_Exp*)_check_null(_tmp4D9->initializer));{union Cyc_CfFlowInfo_FlowInfo_union
_tmp4ED=({union Cyc_CfFlowInfo_FlowInfo_union _tmp4EE;(_tmp4EE.ReachableFL).tag=1;(
_tmp4EE.ReachableFL).f1=_tmp4E6;(_tmp4EE.ReachableFL).f2=_tmp4E7;(_tmp4EE.ReachableFL).f3=
_tmp4E8;_tmp4EE;});Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp4ED);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp4ED;}_LL329:;}}}}_LL321:
if(*((int*)_tmp4D8)!= 2)goto _LL323;_tmp4DA=((struct Cyc_Absyn_Let_d_struct*)
_tmp4D8)->f2;if(_tmp4DA == 0)goto _LL323;_tmp4DB=*_tmp4DA;_tmp4DC=(struct Cyc_List_List*)
_tmp4DB.v;_tmp4DD=((struct Cyc_Absyn_Let_d_struct*)_tmp4D8)->f3;_LL322: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp4F1;void*_tmp4F2;struct _tuple5 _tmp4F0=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp4DD);_tmp4F1=_tmp4F0.f1;_tmp4F2=_tmp4F0.f2;_tmp4F1=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4DD->loc,_tmp4F1);_tmp4F1=Cyc_NewControlFlow_use_Rval(env,_tmp4DD->loc,
_tmp4F1,_tmp4F2);return Cyc_NewControlFlow_add_vars(_tmp4F1,_tmp4DC,Cyc_CfFlowInfo_unknown_all,
decl->loc);}_LL323: if(*((int*)_tmp4D8)!= 3)goto _LL325;_tmp4DE=((struct Cyc_Absyn_Letv_d_struct*)
_tmp4D8)->f1;_LL324: return Cyc_NewControlFlow_add_vars(inflow,_tmp4DE,Cyc_CfFlowInfo_unknown_none,
decl->loc);_LL325: if(*((int*)_tmp4D8)!= 1)goto _LL327;_tmp4DF=((struct Cyc_Absyn_Fn_d_struct*)
_tmp4D8)->f1;_LL326: Cyc_NewControlFlow_check_nested_fun(inflow,_tmp4DF);{void*
_tmp4F3=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4DF->cached_typ))->v;struct
Cyc_Absyn_Vardecl*_tmp4F4=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp4DF->fn_vardecl);
return Cyc_NewControlFlow_add_vars(inflow,({struct Cyc_List_List*_tmp4F5=_cycalloc(
sizeof(*_tmp4F5));_tmp4F5->hd=_tmp4F4;_tmp4F5->tl=0;_tmp4F5;}),Cyc_CfFlowInfo_unknown_all,
decl->loc);}_LL327:;_LL328:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6[0]=({struct Cyc_Core_Impossible_struct
_tmp4F7;_tmp4F7.tag=Cyc_Core_Impossible;_tmp4F7.f1=({const char*_tmp4F8="anal_decl: unexpected decl variant";
_tag_dynforward(_tmp4F8,sizeof(char),_get_zero_arr_size(_tmp4F8,35));});_tmp4F7;});
_tmp4F6;}));_LL31E:;}static void Cyc_NewControlFlow_postpass(struct Cyc_Absyn_Stmt*
s){(void*)(s->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot));{void*_tmp4F9=(void*)s->r;
struct _tuple3 _tmp4FA;struct Cyc_Absyn_Stmt*_tmp4FB;struct _tuple3 _tmp4FC;struct Cyc_Absyn_Stmt*
_tmp4FD;struct Cyc_Absyn_Stmt*_tmp4FE;struct Cyc_Absyn_Stmt*_tmp4FF;struct Cyc_Absyn_Stmt*
_tmp500;struct _tuple3 _tmp501;struct Cyc_Absyn_Stmt*_tmp502;struct Cyc_Absyn_Stmt*
_tmp503;struct Cyc_Absyn_Stmt*_tmp504;struct _tuple3 _tmp505;struct Cyc_Absyn_Stmt*
_tmp506;struct Cyc_Absyn_Stmt*_tmp507;struct Cyc_Absyn_Stmt*_tmp508;struct Cyc_Absyn_Stmt*
_tmp509;struct Cyc_Absyn_Stmt*_tmp50A;struct Cyc_Absyn_Stmt*_tmp50B;struct Cyc_Absyn_Stmt*
_tmp50C;struct Cyc_List_List*_tmp50D;struct Cyc_List_List*_tmp50E;_LL32F: if(
_tmp4F9 <= (void*)1)goto _LL343;if(*((int*)_tmp4F9)!= 8)goto _LL331;_tmp4FA=((
struct Cyc_Absyn_For_s_struct*)_tmp4F9)->f2;_tmp4FB=_tmp4FA.f2;_tmp4FC=((struct
Cyc_Absyn_For_s_struct*)_tmp4F9)->f3;_tmp4FD=_tmp4FC.f2;_tmp4FE=((struct Cyc_Absyn_For_s_struct*)
_tmp4F9)->f4;_LL330: Cyc_NewControlFlow_postpass(_tmp4FD);_tmp4FF=_tmp4FE;_tmp500=
_tmp4FB;goto _LL332;_LL331: if(*((int*)_tmp4F9)!= 1)goto _LL333;_tmp4FF=((struct Cyc_Absyn_Seq_s_struct*)
_tmp4F9)->f1;_tmp500=((struct Cyc_Absyn_Seq_s_struct*)_tmp4F9)->f2;_LL332: _tmp502=
_tmp4FF;_tmp503=_tmp500;goto _LL334;_LL333: if(*((int*)_tmp4F9)!= 4)goto _LL335;
_tmp501=((struct Cyc_Absyn_While_s_struct*)_tmp4F9)->f1;_tmp502=_tmp501.f2;
_tmp503=((struct Cyc_Absyn_While_s_struct*)_tmp4F9)->f2;_LL334: _tmp504=_tmp503;
_tmp506=_tmp502;goto _LL336;_LL335: if(*((int*)_tmp4F9)!= 13)goto _LL337;_tmp504=((
struct Cyc_Absyn_Do_s_struct*)_tmp4F9)->f1;_tmp505=((struct Cyc_Absyn_Do_s_struct*)
_tmp4F9)->f2;_tmp506=_tmp505.f2;_LL336: _tmp507=_tmp504;_tmp508=_tmp506;goto
_LL338;_LL337: if(*((int*)_tmp4F9)!= 3)goto _LL339;_tmp507=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp4F9)->f2;_tmp508=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4F9)->f3;_LL338:
Cyc_NewControlFlow_postpass(_tmp507);_tmp509=_tmp508;goto _LL33A;_LL339: if(*((int*)
_tmp4F9)!= 15)goto _LL33B;_tmp509=((struct Cyc_Absyn_Region_s_struct*)_tmp4F9)->f4;
_LL33A: _tmp50A=_tmp509;goto _LL33C;_LL33B: if(*((int*)_tmp4F9)!= 12)goto _LL33D;
_tmp50A=((struct Cyc_Absyn_Label_s_struct*)_tmp4F9)->f2;_LL33C: _tmp50B=_tmp50A;
goto _LL33E;_LL33D: if(*((int*)_tmp4F9)!= 11)goto _LL33F;_tmp50B=((struct Cyc_Absyn_Decl_s_struct*)
_tmp4F9)->f2;_LL33E: Cyc_NewControlFlow_postpass(_tmp50B);return;_LL33F: if(*((int*)
_tmp4F9)!= 14)goto _LL341;_tmp50C=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp4F9)->f1;
_tmp50D=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp4F9)->f2;_LL340: Cyc_NewControlFlow_postpass(
_tmp50C);_tmp50E=_tmp50D;goto _LL342;_LL341: if(*((int*)_tmp4F9)!= 9)goto _LL343;
_tmp50E=((struct Cyc_Absyn_Switch_s_struct*)_tmp4F9)->f2;_LL342: for(0;_tmp50E != 0;
_tmp50E=_tmp50E->tl){Cyc_NewControlFlow_postpass(((struct Cyc_Absyn_Switch_clause*)
_tmp50E->hd)->body);}return;_LL343:;_LL344: return;_LL32E:;}}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd){Cyc_NewControlFlow_check_nested_fun((union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp50F;(_tmp50F.ReachableFL).tag=1;(_tmp50F.ReachableFL).f1=
Cyc_CfFlowInfo_empty_flowdict();(_tmp50F.ReachableFL).f2=0;(_tmp50F.ReachableFL).f3=({
struct Cyc_CfFlowInfo_ConsumeInfo _tmp510;_tmp510.consumed=Cyc_CfFlowInfo_mt_place_set();
_tmp510.may_consume=0;_tmp510;});_tmp50F;}),fd);}static void Cyc_NewControlFlow_check_nested_fun(
union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*fd){{struct
_RegionHandle _tmp511=_new_region("frgn");struct _RegionHandle*frgn=& _tmp511;
_push_region(frgn);{struct Cyc_Position_Segment*_tmp512=(fd->body)->loc;inflow=
Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,Cyc_CfFlowInfo_unknown_all,_tmp512);{struct
Cyc_List_List*param_roots=0;{union Cyc_CfFlowInfo_FlowInfo_union _tmp513=inflow;
struct Cyc_Dict_Dict _tmp514;struct Cyc_List_List*_tmp515;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp516;_LL346: if((_tmp513.BottomFL).tag != 0)goto _LL348;_LL347:({void*_tmp517[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp518="check_fun";_tag_dynforward(_tmp518,sizeof(char),
_get_zero_arr_size(_tmp518,10));}),_tag_dynforward(_tmp517,sizeof(void*),0));});
_LL348: if((_tmp513.ReachableFL).tag != 1)goto _LL345;_tmp514=(_tmp513.ReachableFL).f1;
_tmp515=(_tmp513.ReachableFL).f2;_tmp516=(_tmp513.ReachableFL).f3;_LL349: {
struct Cyc_List_List*atts;{void*_tmp519=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp51A;struct Cyc_List_List*
_tmp51B;_LL34B: if(_tmp519 <= (void*)4)goto _LL34D;if(*((int*)_tmp519)!= 8)goto
_LL34D;_tmp51A=((struct Cyc_Absyn_FnType_struct*)_tmp519)->f1;_tmp51B=_tmp51A.attributes;
_LL34C: atts=_tmp51B;goto _LL34A;_LL34D:;_LL34E:({void*_tmp51C[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp51D="check_fun: non-function type cached with fndecl_t";_tag_dynforward(
_tmp51D,sizeof(char),_get_zero_arr_size(_tmp51D,50));}),_tag_dynforward(_tmp51C,
sizeof(void*),0));});_LL34A:;}for(0;atts != 0;atts=atts->tl){void*_tmp51E=(void*)
atts->hd;int _tmp51F;_LL350: if(_tmp51E <= (void*)17)goto _LL352;if(*((int*)_tmp51E)
!= 4)goto _LL352;_tmp51F=((struct Cyc_Absyn_Initializes_att_struct*)_tmp51E)->f1;
_LL351: {unsigned int j=(unsigned int)_tmp51F;if(j > ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp520[0]={};Cyc_Tcutil_terr(_tmp512,({const char*_tmp521="initializes attribute exceeds number of parameters";
_tag_dynforward(_tmp521,sizeof(char),_get_zero_arr_size(_tmp521,51));}),
_tag_dynforward(_tmp520,sizeof(void*),0));});continue;}{struct Cyc_Absyn_Vardecl*
_tmp522=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(
int)(j - 1));{void*_tmp523=Cyc_Tcutil_compress((void*)_tmp522->type);struct Cyc_Absyn_PtrInfo
_tmp524;void*_tmp525;struct Cyc_Absyn_PtrAtts _tmp526;struct Cyc_Absyn_Conref*
_tmp527;struct Cyc_Absyn_Conref*_tmp528;struct Cyc_Absyn_Conref*_tmp529;_LL355: if(
_tmp523 <= (void*)4)goto _LL357;if(*((int*)_tmp523)!= 4)goto _LL357;_tmp524=((
struct Cyc_Absyn_PointerType_struct*)_tmp523)->f1;_tmp525=(void*)_tmp524.elt_typ;
_tmp526=_tmp524.ptr_atts;_tmp527=_tmp526.nullable;_tmp528=_tmp526.bounds;_tmp529=
_tmp526.zero_term;_LL356: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp527))({void*_tmp52A[0]={};Cyc_Tcutil_terr(_tmp512,({const char*_tmp52B="initializes attribute not allowed on nullable pointers";
_tag_dynforward(_tmp52B,sizeof(char),_get_zero_arr_size(_tmp52B,55));}),
_tag_dynforward(_tmp52A,sizeof(void*),0));});if(!Cyc_Tcutil_is_bound_one(_tmp528))({
void*_tmp52C[0]={};Cyc_Tcutil_terr(_tmp512,({const char*_tmp52D="initializes attribute allowed only on pointers of size 1";
_tag_dynforward(_tmp52D,sizeof(char),_get_zero_arr_size(_tmp52D,57));}),
_tag_dynforward(_tmp52C,sizeof(void*),0));});if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp529))({void*_tmp52E[0]={};Cyc_Tcutil_terr(_tmp512,({
const char*_tmp52F="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_dynforward(_tmp52F,sizeof(char),_get_zero_arr_size(_tmp52F,77));}),
_tag_dynforward(_tmp52E,sizeof(void*),0));});{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp530=({struct Cyc_CfFlowInfo_InitParam_struct*_tmp538=_cycalloc(sizeof(*
_tmp538));_tmp538[0]=({struct Cyc_CfFlowInfo_InitParam_struct _tmp539;_tmp539.tag=
2;_tmp539.f1=(int)j;_tmp539.f2=(void*)_tmp525;_tmp539;});_tmp538;});struct Cyc_CfFlowInfo_Place*
_tmp531=({struct Cyc_CfFlowInfo_Place*_tmp537=_cycalloc(sizeof(*_tmp537));_tmp537->root=(
void*)((void*)_tmp530);_tmp537->fields=0;_tmp537;});_tmp514=Cyc_Dict_insert(
_tmp514,(void*)_tmp530,Cyc_CfFlowInfo_typ_to_absrval(_tmp525,Cyc_CfFlowInfo_esc_none));
_tmp514=Cyc_Dict_insert(_tmp514,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp532=_cycalloc(sizeof(*_tmp532));_tmp532[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp533;_tmp533.tag=0;_tmp533.f1=_tmp522;_tmp533;});_tmp532;}),(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp534=_cycalloc(sizeof(*_tmp534));_tmp534[0]=({
struct Cyc_CfFlowInfo_AddressOf_struct _tmp535;_tmp535.tag=2;_tmp535.f1=_tmp531;
_tmp535;});_tmp534;}));param_roots=({struct Cyc_List_List*_tmp536=_region_malloc(
frgn,sizeof(*_tmp536));_tmp536->hd=_tmp531;_tmp536->tl=param_roots;_tmp536;});
goto _LL354;}_LL357:;_LL358:({void*_tmp53A[0]={};Cyc_Tcutil_terr(_tmp512,({const
char*_tmp53B="initializes attribute on non-pointer";_tag_dynforward(_tmp53B,
sizeof(char),_get_zero_arr_size(_tmp53B,37));}),_tag_dynforward(_tmp53A,sizeof(
void*),0));});_LL354:;}goto _LL34F;}}_LL352:;_LL353: goto _LL34F;_LL34F:;}inflow=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp53C;(
_tmp53C.ReachableFL).tag=1;(_tmp53C.ReachableFL).f1=_tmp514;(_tmp53C.ReachableFL).f2=
_tmp515;(_tmp53C.ReachableFL).f3=_tmp516;_tmp53C;});}_LL345:;}{int noreturn=Cyc_Tcutil_is_noreturn(
Cyc_Tcutil_fndecl2typ(fd));struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp545=_region_malloc(frgn,sizeof(*_tmp545));_tmp545->iterate_again=1;_tmp545->iteration_num=
0;_tmp545->in_try=0;_tmp545->tryflow=inflow;_tmp545->all_changed=0;_tmp545->noreturn=
noreturn;_tmp545->param_roots=param_roots;_tmp545;});union Cyc_CfFlowInfo_FlowInfo_union
outflow=inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp53D=outflow;_LL35A: if((_tmp53D.BottomFL).tag != 0)goto _LL35C;_LL35B: goto
_LL359;_LL35C:;_LL35D: Cyc_NewControlFlow_check_init_params(_tmp512,env,outflow);
if(noreturn)({void*_tmp53E[0]={};Cyc_Tcutil_terr(_tmp512,({const char*_tmp53F="`noreturn' function might (implicitly) return";
_tag_dynforward(_tmp53F,sizeof(char),_get_zero_arr_size(_tmp53F,46));}),
_tag_dynforward(_tmp53E,sizeof(void*),0));});else{void*_tmp540=Cyc_Tcutil_compress((
void*)fd->ret_type);_LL35F: if((int)_tmp540 != 0)goto _LL361;_LL360: goto _LL35E;
_LL361: if(_tmp540 <= (void*)4)goto _LL363;if(*((int*)_tmp540)!= 6)goto _LL363;
_LL362: goto _LL364;_LL363: if((int)_tmp540 != 1)goto _LL365;_LL364: goto _LL366;_LL365:
if(_tmp540 <= (void*)4)goto _LL367;if(*((int*)_tmp540)!= 5)goto _LL367;_LL366:({
void*_tmp541[0]={};Cyc_Tcutil_warn(_tmp512,({const char*_tmp542="function may not return a value";
_tag_dynforward(_tmp542,sizeof(char),_get_zero_arr_size(_tmp542,32));}),
_tag_dynforward(_tmp541,sizeof(void*),0));});goto _LL35E;_LL367:;_LL368:({void*
_tmp543[0]={};Cyc_Tcutil_terr(_tmp512,({const char*_tmp544="function may not return a value";
_tag_dynforward(_tmp544,sizeof(char),_get_zero_arr_size(_tmp544,32));}),
_tag_dynforward(_tmp543,sizeof(void*),0));});goto _LL35E;_LL35E:;}goto _LL359;
_LL359:;}}}};_pop_region(frgn);}Cyc_NewControlFlow_postpass(fd->body);}void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){void*_tmp546=(void*)((struct Cyc_Absyn_Decl*)
ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp547;struct Cyc_List_List*_tmp548;struct Cyc_List_List*
_tmp549;struct Cyc_List_List*_tmp54A;_LL36A: if(_tmp546 <= (void*)2)goto _LL374;if(*((
int*)_tmp546)!= 1)goto _LL36C;_tmp547=((struct Cyc_Absyn_Fn_d_struct*)_tmp546)->f1;
_LL36B: Cyc_NewControlFlow_check_fun(_tmp547);goto _LL369;_LL36C: if(*((int*)
_tmp546)!= 10)goto _LL36E;_tmp548=((struct Cyc_Absyn_ExternC_d_struct*)_tmp546)->f1;
_LL36D: _tmp549=_tmp548;goto _LL36F;_LL36E: if(*((int*)_tmp546)!= 9)goto _LL370;
_tmp549=((struct Cyc_Absyn_Using_d_struct*)_tmp546)->f2;_LL36F: _tmp54A=_tmp549;
goto _LL371;_LL370: if(*((int*)_tmp546)!= 8)goto _LL372;_tmp54A=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp546)->f2;_LL371: Cyc_NewControlFlow_cf_check(_tmp54A);goto _LL369;_LL372: if(*((
int*)_tmp546)!= 11)goto _LL374;_LL373: goto _LL369;_LL374:;_LL375: goto _LL369;_LL369:;}}
