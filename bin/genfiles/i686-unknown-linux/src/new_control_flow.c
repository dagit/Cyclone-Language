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
Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);struct Cyc_List_List*Cyc_List_rlist(
struct _RegionHandle*,struct _dynforward_ptr);int Cyc_List_length(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct
Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_merge(int(*cmp)(void*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*
x,int n);int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*
env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];typedef struct{int __count;union{
unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef struct{long
__pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;
struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dynforward_ptr f1;};extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dynforward_ptr f1;};struct Cyc_Dict_Dict;
extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];int Cyc_Dict_member(
struct Cyc_Dict_Dict*d,void*k);struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);struct
_tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct
_tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*
Cyc_Dict_delete(struct Cyc_Dict_Dict*,void*);struct Cyc_Iter_Iter Cyc_Dict_make_iter(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict*d);struct Cyc_Lineno_Pos{struct
_dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dynforward_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];int Cyc_Position_error_p();
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple1{union Cyc_Absyn_Nmspace_union
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
Cyc_Dict_Dict*Cyc_CfFlowInfo_empty_aggrdict();struct Cyc_Dict_Dict*Cyc_CfFlowInfo_empty_flowdict();
struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct Cyc_Dict_Dict*f1;};struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_mt_place_set();int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict**
set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc);struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict*s1,struct Cyc_Dict_Dict*s2,int
disjoint);struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict*consumed;struct
Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(
struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict*f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};extern void*Cyc_CfFlowInfo_unknown_none;extern void*Cyc_CfFlowInfo_unknown_all;
extern void*Cyc_CfFlowInfo_esc_none;extern void*Cyc_CfFlowInfo_esc_all;int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);void*Cyc_CfFlowInfo_typ_to_absrval(
void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict*d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo_union
f1,union Cyc_CfFlowInfo_FlowInfo_union f2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct
Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(struct Cyc_Dict_Dict*
d,struct Cyc_Dict_Dict**all_changed,void*r);struct Cyc_Dict_Dict*Cyc_CfFlowInfo_assign_place(
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict*d,struct Cyc_Dict_Dict**
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_join_flow(struct Cyc_Dict_Dict**,union Cyc_CfFlowInfo_FlowInfo_union,
union Cyc_CfFlowInfo_FlowInfo_union,int);struct _tuple5{union Cyc_CfFlowInfo_FlowInfo_union
f1;void*f2;};struct _tuple5 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_Dict_Dict**
all_changed,struct _tuple5 pr1,struct _tuple5 pr2,int);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_after_flow(struct Cyc_Dict_Dict**,union Cyc_CfFlowInfo_FlowInfo_union,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict*);
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
Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt*encloser;int visited;union Cyc_CfFlowInfo_FlowInfo_union flow;
};static char Cyc_NewControlFlow_CFAnnot[12]="\000\000\000\000CFAnnot\000";struct
Cyc_NewControlFlow_CFAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot
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
iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo_union tryflow;struct Cyc_Dict_Dict**
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
_tmpA->visited=env->iteration_num;return _tmpA;}static void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union new_flow){
if(env->in_try)env->tryflow=Cyc_CfFlowInfo_join_flow(env->all_changed,new_flow,
env->tryflow,1);}static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo_union flow){struct Cyc_NewControlFlow_CFStmtAnnot*
_tmpB=Cyc_NewControlFlow_get_stmt_annot(s);union Cyc_CfFlowInfo_FlowInfo_union
_tmpC=Cyc_CfFlowInfo_join_flow(env->all_changed,flow,_tmpB->flow,1);if(!Cyc_CfFlowInfo_flow_lessthan_approx(
_tmpC,_tmpB->flow)){_tmpB->flow=_tmpC;if(_tmpB->visited == env->iteration_num)env->iterate_again=
1;}}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_add_vars(union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*vds,void*leafval,struct Cyc_Position_Segment*loc){
union Cyc_CfFlowInfo_FlowInfo_union _tmpD=inflow;struct Cyc_Dict_Dict*_tmpE;struct
Cyc_List_List*_tmpF;struct Cyc_CfFlowInfo_ConsumeInfo _tmp10;_LL6: if((_tmpD.BottomFL).tag
!= 0)goto _LL8;_LL7: return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp11;(_tmp11.BottomFL).tag=0;_tmp11;});_LL8: if((_tmpD.ReachableFL).tag != 1)
goto _LL5;_tmpE=(_tmpD.ReachableFL).f1;_tmpF=(_tmpD.ReachableFL).f2;_tmp10=(_tmpD.ReachableFL).f3;
_LL9: for(0;vds != 0;vds=vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct*_tmp12=({
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13[0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _tmp14;_tmp14.tag=0;_tmp14.f1=(struct Cyc_Absyn_Vardecl*)
vds->hd;_tmp14;});_tmp13;});_tmpE=Cyc_Dict_insert(_tmpE,(void*)_tmp12,Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Absyn_Vardecl*)vds->hd)->type,leafval));}return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp15;(_tmp15.ReachableFL).tag=1;(_tmp15.ReachableFL).f1=
_tmpE;(_tmp15.ReachableFL).f2=_tmpF;(_tmp15.ReachableFL).f3=_tmp10;_tmp15;});
_LL5:;}struct _tuple8{struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*f2;
};static void Cyc_NewControlFlow_remove_vars(union Cyc_CfFlowInfo_FlowInfo_union
outflow,struct Cyc_Dict_Dict**out_unique_places,struct Cyc_Dict_Dict*
old_unique_places){struct Cyc_CfFlowInfo_ConsumeInfo _tmp17;struct Cyc_Dict_Dict*
_tmp18;struct _tuple6 _tmp16=Cyc_CfFlowInfo_save_consume_info(outflow,0);_tmp17=
_tmp16.f1;_tmp18=_tmp17.consumed;{struct _RegionHandle _tmp19=_new_region("r");
struct _RegionHandle*r=& _tmp19;_push_region(r);{struct Cyc_Iter_Iter _tmp1A=((
struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict*d))Cyc_Dict_make_iter)(
r,*((struct Cyc_Dict_Dict**)_check_null(out_unique_places)));struct _tuple8 _tmp1B=*((
struct _tuple8*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict*d))Cyc_Dict_rchoose)(
r,*out_unique_places);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple8*))Cyc_Iter_next)(
_tmp1A,& _tmp1B)){struct Cyc_CfFlowInfo_Place*_tmp1C=_tmp1B.f1;if(!((int(*)(struct
Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp18,_tmp1C))({
struct Cyc_String_pa_struct _tmp1F;_tmp1F.tag=0;_tmp1F.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_CfFlowInfo_place_err_string(_tmp1C));{void*_tmp1D[1]={&
_tmp1F};Cyc_Tcutil_terr(_tmp1B.f2,({const char*_tmp1E="Failed to consume unique variable %s";
_tag_dynforward(_tmp1E,sizeof(char),_get_zero_arr_size(_tmp1E,37));}),
_tag_dynforward(_tmp1D,sizeof(void*),1));}});}};_pop_region(r);}*
out_unique_places=old_unique_places;}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_use_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
inflow,void*r){union Cyc_CfFlowInfo_FlowInfo_union _tmp20=inflow;struct Cyc_Dict_Dict*
_tmp21;struct Cyc_List_List*_tmp22;struct Cyc_CfFlowInfo_ConsumeInfo _tmp23;_LLB:
if((_tmp20.BottomFL).tag != 0)goto _LLD;_LLC: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp24;(_tmp24.BottomFL).tag=0;_tmp24;});_LLD:
if((_tmp20.ReachableFL).tag != 1)goto _LLA;_tmp21=(_tmp20.ReachableFL).f1;_tmp22=(
_tmp20.ReachableFL).f2;_tmp23=(_tmp20.ReachableFL).f3;_LLE: if(Cyc_CfFlowInfo_initlevel(
_tmp21,r)!= (void*)2)({void*_tmp25[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp26="expression may not be fully initialized";
_tag_dynforward(_tmp26,sizeof(char),_get_zero_arr_size(_tmp26,40));}),
_tag_dynforward(_tmp25,sizeof(void*),0));});{struct Cyc_Dict_Dict*_tmp27=Cyc_CfFlowInfo_escape_deref(
_tmp21,env->all_changed,r);if(_tmp21 == _tmp27)return inflow;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp28=({union Cyc_CfFlowInfo_FlowInfo_union _tmp29;(_tmp29.ReachableFL).tag=1;(
_tmp29.ReachableFL).f1=_tmp27;(_tmp29.ReachableFL).f2=_tmp22;(_tmp29.ReachableFL).f3=
_tmp23;_tmp29;});Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp28);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp28;}}_LLA:;}struct _tuple9{
union Cyc_CfFlowInfo_FlowInfo_union f1;struct Cyc_List_List*f2;};static struct
_tuple9 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_List_List*es,int
arg1_unconsumed){if(es == 0)return({struct _tuple9 _tmp2A;_tmp2A.f1=inflow;_tmp2A.f2=
0;_tmp2A;});if(es->tl == 0){union Cyc_CfFlowInfo_FlowInfo_union _tmp2C;void*_tmp2D;
struct _tuple5 _tmp2B=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_tmp2C=_tmp2B.f1;_tmp2D=_tmp2B.f2;return({struct _tuple9 _tmp2E;_tmp2E.f1=
_tmp2C;_tmp2E.f2=({struct Cyc_List_List*_tmp2F=_region_malloc(rgn,sizeof(*_tmp2F));
_tmp2F->hd=(void*)_tmp2D;_tmp2F->tl=0;_tmp2F;});_tmp2E;});}{struct Cyc_Dict_Dict**
outer_all_changed=env->all_changed;struct Cyc_Dict_Dict*this_all_changed;union Cyc_CfFlowInfo_FlowInfo_union
old_inflow;union Cyc_CfFlowInfo_FlowInfo_union outflow;struct Cyc_List_List*rvals;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp31;union Cyc_CfFlowInfo_FlowInfo_union _tmp32;
struct _tuple6 _tmp30=Cyc_CfFlowInfo_save_consume_info(inflow,1);_tmp31=_tmp30.f1;
_tmp32=_tmp30.f2;{struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp47;_tmp47.consumed=Cyc_CfFlowInfo_mt_place_set();_tmp47.may_consume=0;_tmp47;});
int init_consume=0;do{this_all_changed=Cyc_CfFlowInfo_mt_place_set();_tmp32=Cyc_CfFlowInfo_restore_consume_info(
_tmp32,_tmp31);env->all_changed=({struct Cyc_Dict_Dict**_tmp33=_cycalloc(sizeof(*
_tmp33));_tmp33[0]=Cyc_CfFlowInfo_mt_place_set();_tmp33;});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp35;void*_tmp36;struct _tuple5 _tmp34=Cyc_NewControlFlow_anal_Rexp(env,_tmp32,(
struct Cyc_Absyn_Exp*)es->hd);_tmp35=_tmp34.f1;_tmp36=_tmp34.f2;outflow=_tmp35;
rvals=({struct Cyc_List_List*_tmp37=_region_malloc(rgn,sizeof(*_tmp37));_tmp37->hd=(
void*)_tmp36;_tmp37->tl=0;_tmp37;});this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict**)_check_null(env->all_changed)),0);if(
arg1_unconsumed){union Cyc_CfFlowInfo_FlowInfo_union _tmp38=outflow;struct Cyc_Dict_Dict*
_tmp39;struct Cyc_List_List*_tmp3A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3B;_LL10:
if((_tmp38.BottomFL).tag != 0)goto _LL12;_LL11: goto _LLF;_LL12: if((_tmp38.ReachableFL).tag
!= 1)goto _LLF;_tmp39=(_tmp38.ReachableFL).f1;_tmp3A=(_tmp38.ReachableFL).f2;
_tmp3B=(_tmp38.ReachableFL).f3;_LL13: outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3C;(_tmp3C.ReachableFL).tag=1;(_tmp3C.ReachableFL).f1=
_tmp39;(_tmp3C.ReachableFL).f2=_tmp3A;(_tmp3C.ReachableFL).f3=({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3D;_tmp3D.consumed=_tmp3B.consumed;_tmp3D.may_consume=_tmp31.may_consume;
_tmp3D;});_tmp3C;});_LLF:;}{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
env->all_changed=({struct Cyc_Dict_Dict**_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E[
0]=Cyc_CfFlowInfo_mt_place_set();_tmp3E;});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp40;void*_tmp41;struct _tuple5 _tmp3F=Cyc_NewControlFlow_anal_Rexp(env,_tmp32,(
struct Cyc_Absyn_Exp*)es2->hd);_tmp40=_tmp3F.f1;_tmp41=_tmp3F.f2;rvals=({struct
Cyc_List_List*_tmp42=_region_malloc(rgn,sizeof(*_tmp42));_tmp42->hd=(void*)
_tmp41;_tmp42->tl=rvals;_tmp42;});outflow=Cyc_CfFlowInfo_after_flow((struct Cyc_Dict_Dict**)&
this_all_changed,outflow,_tmp40,this_all_changed,*((struct Cyc_Dict_Dict**)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict**)_check_null(env->all_changed)),0);}}}{
struct Cyc_CfFlowInfo_ConsumeInfo _tmp44;struct _tuple6 _tmp43=Cyc_CfFlowInfo_save_consume_info(
outflow,0);_tmp44=_tmp43.f1;if(init_consume)old_inflow=Cyc_CfFlowInfo_restore_consume_info(
_tmp32,outflow_consume);else{old_inflow=_tmp32;}init_consume=1;outflow_consume=
_tmp44;_tmp32=Cyc_CfFlowInfo_join_flow(outer_all_changed,_tmp32,outflow,1);}}}while(
!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp32,old_inflow));if(outer_all_changed == 
0)env->all_changed=0;else{env->all_changed=({struct Cyc_Dict_Dict**_tmp45=
_cycalloc(sizeof(*_tmp45));_tmp45[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0);_tmp45;});}Cyc_NewControlFlow_update_tryflow(
env,outflow);return({struct _tuple9 _tmp46;_tmp46.f1=outflow;_tmp46.f2=Cyc_List_imp_rev(
rvals);_tmp46;});}}}static struct _tuple5 Cyc_NewControlFlow_anal_use_ints(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct
Cyc_List_List*es,int arg1_unconsumed){struct _RegionHandle _tmp48=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp48;_push_region(rgn);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp4A;struct Cyc_List_List*_tmp4B;struct _tuple9 _tmp49=Cyc_NewControlFlow_anal_unordered_Rexps(
rgn,env,inflow,es,arg1_unconsumed);_tmp4A=_tmp49.f1;_tmp4B=_tmp49.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp4C=_tmp4A;struct Cyc_Dict_Dict*_tmp4D;_LL15: if((_tmp4C.ReachableFL).tag != 1)
goto _LL17;_tmp4D=(_tmp4C.ReachableFL).f1;_LL16: for(0;_tmp4B != 0;(_tmp4B=_tmp4B->tl,
es=((struct Cyc_List_List*)_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(
_tmp4D,(void*)_tmp4B->hd)== (void*)0)({void*_tmp4E[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,({const
char*_tmp4F="expression may not be initialized";_tag_dynforward(_tmp4F,sizeof(
char),_get_zero_arr_size(_tmp4F,34));}),_tag_dynforward(_tmp4E,sizeof(void*),0));});}
goto _LL14;_LL17: if((_tmp4C.BottomFL).tag != 0)goto _LL14;_LL18: goto _LL14;_LL14:;}{
struct _tuple5 _tmp51=({struct _tuple5 _tmp50;_tmp50.f1=_tmp4A;_tmp50.f2=Cyc_CfFlowInfo_unknown_all;
_tmp50;});_npop_handler(0);return _tmp51;}};_pop_region(rgn);}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Absyn_Exp*e,void*il){
union Cyc_CfFlowInfo_FlowInfo_union _tmp52=outflow;struct Cyc_Dict_Dict*_tmp53;
struct Cyc_List_List*_tmp54;struct Cyc_CfFlowInfo_ConsumeInfo _tmp55;_LL1A: if((
_tmp52.BottomFL).tag != 0)goto _LL1C;_LL1B: return outflow;_LL1C: if((_tmp52.ReachableFL).tag
!= 1)goto _LL19;_tmp53=(_tmp52.ReachableFL).f1;_tmp54=(_tmp52.ReachableFL).f2;
_tmp55=(_tmp52.ReachableFL).f3;_LL1D: {void*_tmp56=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp57;_LL1F: if((int)_tmp56 != 0)
goto _LL21;_LL20: return outflow;_LL21: if(_tmp56 <= (void*)1)goto _LL1E;if(*((int*)
_tmp56)!= 0)goto _LL1E;_tmp57=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp56)->f1;
_LL22: {void*nzval=il == (void*)2?(void*)1:(void*)2;return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp58;(_tmp58.ReachableFL).tag=1;(_tmp58.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(e->loc,_tmp53,env->all_changed,_tmp57,nzval);(_tmp58.ReachableFL).f2=
_tmp54;(_tmp58.ReachableFL).f3=_tmp55;_tmp58;});}_LL1E:;}_LL19:;}static struct
_tuple7 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union outflow,
struct Cyc_Absyn_Exp*e,void*il){union Cyc_CfFlowInfo_FlowInfo_union _tmp59=outflow;
struct Cyc_Dict_Dict*_tmp5A;struct Cyc_List_List*_tmp5B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp5C;_LL24: if((_tmp59.BottomFL).tag != 0)goto _LL26;_LL25: return({struct _tuple7
_tmp5D;_tmp5D.f1=outflow;_tmp5D.f2=outflow;_tmp5D;});_LL26: if((_tmp59.ReachableFL).tag
!= 1)goto _LL23;_tmp5A=(_tmp59.ReachableFL).f1;_tmp5B=(_tmp59.ReachableFL).f2;
_tmp5C=(_tmp59.ReachableFL).f3;_LL27: {void*_tmp5E=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp5F;_LL29: if((int)_tmp5E != 0)
goto _LL2B;_LL2A: return({struct _tuple7 _tmp60;_tmp60.f1=outflow;_tmp60.f2=outflow;
_tmp60;});_LL2B: if(_tmp5E <= (void*)1)goto _LL28;if(*((int*)_tmp5E)!= 0)goto _LL28;
_tmp5F=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp5E)->f1;_LL2C: {void*nzval=il == (
void*)2?(void*)1:(void*)2;return({struct _tuple7 _tmp61;_tmp61.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp63;(_tmp63.ReachableFL).tag=1;(_tmp63.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(e->loc,_tmp5A,env->all_changed,_tmp5F,nzval);(_tmp63.ReachableFL).f2=
_tmp5B;(_tmp63.ReachableFL).f3=_tmp5C;_tmp63;});_tmp61.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp62;(_tmp62.ReachableFL).tag=1;(_tmp62.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(e->loc,_tmp5A,env->all_changed,_tmp5F,(void*)0);(
_tmp62.ReachableFL).f2=_tmp5B;(_tmp62.ReachableFL).f3=_tmp5C;_tmp62;});_tmp61;});}
_LL28:;}_LL23:;}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_if_tagcmp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union flow,
struct Cyc_Absyn_Exp*e1,void*r1,void*p,void*r2){union Cyc_CfFlowInfo_FlowInfo_union
_tmp64=flow;struct Cyc_Dict_Dict*_tmp65;struct Cyc_List_List*_tmp66;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp67;_LL2E: if((_tmp64.BottomFL).tag != 0)goto _LL30;_LL2F: return flow;_LL30: if((
_tmp64.ReachableFL).tag != 1)goto _LL2D;_tmp65=(_tmp64.ReachableFL).f1;_tmp66=(
_tmp64.ReachableFL).f2;_tmp67=(_tmp64.ReachableFL).f3;_LL31: {void*_tmp68=r2;
struct Cyc_List_List*_tmp69;_LL33: if(_tmp68 <= (void*)3)goto _LL35;if(*((int*)
_tmp68)!= 3)goto _LL35;_tmp69=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp68)->f1;
_LL34: {void*_tmp6A=(Cyc_NewControlFlow_anal_Lexp(env,flow,e1)).f2;struct Cyc_CfFlowInfo_Place*
_tmp6B;_LL38: if((int)_tmp6A != 0)goto _LL3A;_LL39: return flow;_LL3A: if(_tmp6A <= (
void*)1)goto _LL37;if(*((int*)_tmp6A)!= 0)goto _LL37;_tmp6B=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp6A)->f1;_LL3B: {struct Cyc_List_List*new_cl;{void*_tmp6C=r1;struct Cyc_List_List*
_tmp6D;void*_tmp6E;_LL3D: if(_tmp6C <= (void*)3)goto _LL41;if(*((int*)_tmp6C)!= 3)
goto _LL3F;_tmp6D=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp6C)->f1;_LL3E: new_cl=
_tmp6D;goto _LL3C;_LL3F: if(*((int*)_tmp6C)!= 0)goto _LL41;_tmp6E=(void*)((struct
Cyc_CfFlowInfo_UnknownR_struct*)_tmp6C)->f1;if((int)_tmp6E != 2)goto _LL41;_LL40:
goto _LL42;_LL41: if((int)_tmp6C != 0)goto _LL43;_LL42: goto _LL44;_LL43: if((int)
_tmp6C != 1)goto _LL45;_LL44: new_cl=0;goto _LL3C;_LL45:;_LL46: return flow;_LL3C:;}
for(0;_tmp69 != 0;_tmp69=_tmp69->tl){void*new_cmp;{struct _tuple0 _tmp70=({struct
_tuple0 _tmp6F;_tmp6F.f1=p;_tmp6F.f2=(void*)((struct Cyc_CfFlowInfo_TagCmp*)_tmp69->hd)->cmp;
_tmp6F;});void*_tmp71;void*_tmp72;void*_tmp73;void*_tmp74;_LL48: _tmp71=_tmp70.f1;
if((int)_tmp71 != 8)goto _LL4A;_LL49: goto _LL4B;_LL4A: _tmp72=_tmp70.f2;if((int)
_tmp72 != 8)goto _LL4C;_LL4B: new_cmp=(void*)8;goto _LL47;_LL4C: _tmp73=_tmp70.f1;if((
int)_tmp73 != 5)goto _LL4E;_tmp74=_tmp70.f2;if((int)_tmp74 != 5)goto _LL4E;_LL4D:
new_cmp=(void*)5;goto _LL47;_LL4E:;_LL4F: new_cmp=(void*)10;goto _LL47;_LL47:;}
new_cl=({struct Cyc_List_List*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->hd=({
struct Cyc_CfFlowInfo_TagCmp*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->cmp=(void*)
new_cmp;_tmp76->bd=(void*)((void*)((struct Cyc_CfFlowInfo_TagCmp*)_tmp69->hd)->bd);
_tmp76;});_tmp75->tl=new_cl;_tmp75;});}return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp77;(_tmp77.ReachableFL).tag=1;(_tmp77.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(e1->loc,_tmp65,env->all_changed,_tmp6B,(void*)({
struct Cyc_CfFlowInfo_TagCmps_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78[0]=({
struct Cyc_CfFlowInfo_TagCmps_struct _tmp79;_tmp79.tag=3;_tmp79.f1=new_cl;_tmp79;});
_tmp78;}));(_tmp77.ReachableFL).f2=_tmp66;(_tmp77.ReachableFL).f3=_tmp67;_tmp77;});}
_LL37:;}_LL35:;_LL36: return flow;_LL32:;}_LL2D:;}static struct _tuple5 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
union Cyc_CfFlowInfo_FlowInfo_union f,struct Cyc_Absyn_Exp*e,void*r){void*_tmp7A=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp7B;void*_tmp7C;struct Cyc_Absyn_PtrAtts _tmp7D;struct
Cyc_Absyn_Conref*_tmp7E;struct Cyc_Absyn_Conref*_tmp7F;_LL51: if(_tmp7A <= (void*)4)
goto _LL53;if(*((int*)_tmp7A)!= 4)goto _LL53;_tmp7B=((struct Cyc_Absyn_PointerType_struct*)
_tmp7A)->f1;_tmp7C=(void*)_tmp7B.elt_typ;_tmp7D=_tmp7B.ptr_atts;_tmp7E=_tmp7D.bounds;
_tmp7F=_tmp7D.zero_term;_LL52: {union Cyc_CfFlowInfo_FlowInfo_union _tmp80=f;
struct Cyc_Dict_Dict*_tmp81;struct Cyc_List_List*_tmp82;_LL56: if((_tmp80.BottomFL).tag
!= 0)goto _LL58;_LL57: return({struct _tuple5 _tmp83;_tmp83.f1=f;_tmp83.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp7C,Cyc_CfFlowInfo_unknown_all);_tmp83;});_LL58: if((_tmp80.ReachableFL).tag != 
1)goto _LL55;_tmp81=(_tmp80.ReachableFL).f1;_tmp82=(_tmp80.ReachableFL).f2;_LL59:
if(Cyc_Tcutil_is_bound_one(_tmp7E)){void*_tmp84=r;struct Cyc_CfFlowInfo_Place*
_tmp85;void*_tmp86;_LL5B: if((int)_tmp84 != 1)goto _LL5D;_LL5C: goto _LL5E;_LL5D: if((
int)_tmp84 != 2)goto _LL5F;_LL5E:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp88;_tmp88.tag=Cyc_CfFlowInfo_NotZero;_tmp88.f1=_tmp82;_tmp88;});_tmp87;})));
goto _LL5A;_LL5F: if(_tmp84 <= (void*)3)goto _LL61;if(*((int*)_tmp84)!= 2)goto _LL61;
_tmp85=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp84)->f1;_LL60:(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp89=_cycalloc(sizeof(*
_tmp89));_tmp89[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp8A;_tmp8A.tag=Cyc_CfFlowInfo_NotZero;
_tmp8A.f1=_tmp82;_tmp8A;});_tmp89;})));return({struct _tuple5 _tmp8B;_tmp8B.f1=f;
_tmp8B.f2=Cyc_CfFlowInfo_lookup_place(_tmp81,_tmp85);_tmp8B;});_LL61: if((int)
_tmp84 != 0)goto _LL63;_LL62:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));
return({struct _tuple5 _tmp8C;_tmp8C.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp8D;(_tmp8D.BottomFL).tag=0;_tmp8D;});
_tmp8C.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp7C,Cyc_CfFlowInfo_unknown_all);
_tmp8C;});_LL63: if(_tmp84 <= (void*)3)goto _LL65;if(*((int*)_tmp84)!= 0)goto _LL65;
_tmp86=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp84)->f1;_LL64: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmp86);goto _LL66;_LL65:;_LL66:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E[0]=({
struct Cyc_CfFlowInfo_UnknownZ_struct _tmp8F;_tmp8F.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp8F.f1=_tmp82;_tmp8F;});_tmp8E;})));_LL5A:;}else{(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp90=_cycalloc(sizeof(*_tmp90));
_tmp90[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp91;_tmp91.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp91.f1=_tmp82;_tmp91;});_tmp90;})));}{void*_tmp92=Cyc_CfFlowInfo_initlevel(
_tmp81,r);_LL68: if((int)_tmp92 != 0)goto _LL6A;_LL69:({void*_tmp93[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp94="dereference of possibly uninitialized pointer";
_tag_dynforward(_tmp94,sizeof(char),_get_zero_arr_size(_tmp94,46));}),
_tag_dynforward(_tmp93,sizeof(void*),0));});goto _LL6B;_LL6A: if((int)_tmp92 != 2)
goto _LL6C;_LL6B: return({struct _tuple5 _tmp95;_tmp95.f1=f;_tmp95.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp7C,Cyc_CfFlowInfo_unknown_all);_tmp95;});_LL6C: if((int)_tmp92 != 1)goto _LL67;
_LL6D: return({struct _tuple5 _tmp96;_tmp96.f1=f;_tmp96.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp7C,Cyc_CfFlowInfo_unknown_none);_tmp96;});_LL67:;}_LL55:;}_LL53:;_LL54:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp97=_cycalloc(sizeof(*_tmp97));
_tmp97[0]=({struct Cyc_Core_Impossible_struct _tmp98;_tmp98.tag=Cyc_Core_Impossible;
_tmp98.f1=({const char*_tmp99="right deref of non-pointer-type";_tag_dynforward(
_tmp99,sizeof(char),_get_zero_arr_size(_tmp99,32));});_tmp98;});_tmp97;}));_LL50:;}
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp9A=(void*)e1->r;
void*_tmp9B;struct Cyc_Absyn_Vardecl*_tmp9C;void*_tmp9D;struct Cyc_Absyn_Vardecl*
_tmp9E;void*_tmp9F;struct Cyc_Absyn_Vardecl*_tmpA0;void*_tmpA1;struct Cyc_Absyn_Vardecl*
_tmpA2;_LL6F: if(*((int*)_tmp9A)!= 1)goto _LL71;_tmp9B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp9A)->f2;if(_tmp9B <= (void*)1)goto _LL71;if(*((int*)_tmp9B)!= 4)goto _LL71;
_tmp9C=((struct Cyc_Absyn_Pat_b_struct*)_tmp9B)->f1;_LL70: _tmp9E=_tmp9C;goto _LL72;
_LL71: if(*((int*)_tmp9A)!= 1)goto _LL73;_tmp9D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp9A)->f2;if(_tmp9D <= (void*)1)goto _LL73;if(*((int*)_tmp9D)!= 3)goto _LL73;
_tmp9E=((struct Cyc_Absyn_Local_b_struct*)_tmp9D)->f1;_LL72: _tmpA0=_tmp9E;goto
_LL74;_LL73: if(*((int*)_tmp9A)!= 1)goto _LL75;_tmp9F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp9A)->f2;if(_tmp9F <= (void*)1)goto _LL75;if(*((int*)_tmp9F)!= 2)goto _LL75;
_tmpA0=((struct Cyc_Absyn_Param_b_struct*)_tmp9F)->f1;_LL74: _tmpA2=_tmpA0;goto
_LL76;_LL75: if(*((int*)_tmp9A)!= 1)goto _LL77;_tmpA1=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp9A)->f2;if(_tmpA1 <= (void*)1)goto _LL77;if(*((int*)_tmpA1)!= 0)goto _LL77;
_tmpA2=((struct Cyc_Absyn_Global_b_struct*)_tmpA1)->f1;_LL76: if(!_tmpA2->escapes){
void*_tmpA3=(void*)e2->r;void*_tmpA4;struct Cyc_Absyn_Vardecl*_tmpA5;void*_tmpA6;
struct Cyc_Absyn_Vardecl*_tmpA7;void*_tmpA8;struct Cyc_Absyn_Vardecl*_tmpA9;void*
_tmpAA;struct Cyc_Absyn_Vardecl*_tmpAB;_LL7A: if(*((int*)_tmpA3)!= 1)goto _LL7C;
_tmpA4=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA3)->f2;if(_tmpA4 <= (void*)1)
goto _LL7C;if(*((int*)_tmpA4)!= 4)goto _LL7C;_tmpA5=((struct Cyc_Absyn_Pat_b_struct*)
_tmpA4)->f1;_LL7B: _tmpA7=_tmpA5;goto _LL7D;_LL7C: if(*((int*)_tmpA3)!= 1)goto _LL7E;
_tmpA6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA3)->f2;if(_tmpA6 <= (void*)1)
goto _LL7E;if(*((int*)_tmpA6)!= 3)goto _LL7E;_tmpA7=((struct Cyc_Absyn_Local_b_struct*)
_tmpA6)->f1;_LL7D: _tmpA9=_tmpA7;goto _LL7F;_LL7E: if(*((int*)_tmpA3)!= 1)goto _LL80;
_tmpA8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA3)->f2;if(_tmpA8 <= (void*)1)
goto _LL80;if(*((int*)_tmpA8)!= 2)goto _LL80;_tmpA9=((struct Cyc_Absyn_Param_b_struct*)
_tmpA8)->f1;_LL7F: _tmpAB=_tmpA9;goto _LL81;_LL80: if(*((int*)_tmpA3)!= 1)goto _LL82;
_tmpAA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA3)->f2;if(_tmpAA <= (void*)1)
goto _LL82;if(*((int*)_tmpAA)!= 0)goto _LL82;_tmpAB=((struct Cyc_Absyn_Global_b_struct*)
_tmpAA)->f1;_LL81: if(!_tmpAB->escapes){int found=0;{struct Cyc_List_List*_tmpAC=
relns;for(0;_tmpAC != 0;_tmpAC=_tmpAC->tl){struct Cyc_CfFlowInfo_Reln*_tmpAD=(
struct Cyc_CfFlowInfo_Reln*)_tmpAC->hd;if(_tmpAD->vd == _tmpAB){union Cyc_CfFlowInfo_RelnOp_union
_tmpAE=_tmpAD->rop;struct Cyc_Absyn_Vardecl*_tmpAF;_LL85: if((_tmpAE.LessSize).tag
!= 2)goto _LL87;_tmpAF=(_tmpAE.LessSize).f1;if(!(_tmpAF == _tmpA2))goto _LL87;_LL86:
return relns;_LL87:;_LL88: continue;_LL84:;}}}if(!found)return({struct Cyc_List_List*
_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->hd=({struct Cyc_CfFlowInfo_Reln*_tmpB1=
_cycalloc(sizeof(*_tmpB1));_tmpB1->vd=_tmpAB;_tmpB1->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmpB2;(_tmpB2.LessSize).tag=2;(_tmpB2.LessSize).f1=
_tmpA2;_tmpB2;});_tmpB1;});_tmpB0->tl=relns;_tmpB0;});}return relns;_LL82:;_LL83:
return relns;_LL79:;}else{return relns;}_LL77:;_LL78: return relns;_LL6E:;}struct
_tuple10{void*f1;union Cyc_CfFlowInfo_FlowInfo_union f2;};struct _tuple11{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_Dict_Dict*d;struct Cyc_List_List*relns;struct Cyc_CfFlowInfo_ConsumeInfo
cinfo;{union Cyc_CfFlowInfo_FlowInfo_union _tmpB3=inflow;struct Cyc_Dict_Dict*
_tmpB4;struct Cyc_List_List*_tmpB5;struct Cyc_CfFlowInfo_ConsumeInfo _tmpB6;_LL8A:
if((_tmpB3.BottomFL).tag != 0)goto _LL8C;_LL8B: return({struct _tuple5 _tmpB7;_tmpB7.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmpB8;(
_tmpB8.BottomFL).tag=0;_tmpB8;});_tmpB7.f2=Cyc_CfFlowInfo_unknown_all;_tmpB7;});
_LL8C: if((_tmpB3.ReachableFL).tag != 1)goto _LL89;_tmpB4=(_tmpB3.ReachableFL).f1;
_tmpB5=(_tmpB3.ReachableFL).f2;_tmpB6=(_tmpB3.ReachableFL).f3;_LL8D: d=_tmpB4;
relns=_tmpB5;cinfo=_tmpB6;_LL89:;}{void*_tmpB9=(void*)e->r;struct Cyc_Absyn_Exp*
_tmpBA;void*_tmpBB;struct Cyc_Absyn_Exp*_tmpBC;struct Cyc_Absyn_Exp*_tmpBD;struct
Cyc_Absyn_Exp*_tmpBE;union Cyc_Absyn_Cnst_union _tmpBF;union Cyc_Absyn_Cnst_union
_tmpC0;int _tmpC1;union Cyc_Absyn_Cnst_union _tmpC2;void*_tmpC3;struct Cyc_List_List*
_tmpC4;void*_tmpC5;void*_tmpC6;struct Cyc_Absyn_Vardecl*_tmpC7;void*_tmpC8;struct
Cyc_Absyn_Vardecl*_tmpC9;void*_tmpCA;struct Cyc_Absyn_Vardecl*_tmpCB;void*_tmpCC;
struct Cyc_List_List*_tmpCD;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpCF;
struct Cyc_Core_Opt*_tmpD0;struct Cyc_Core_Opt _tmpD1;struct Cyc_Absyn_Exp*_tmpD2;
struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Core_Opt*_tmpD4;struct Cyc_Absyn_Exp*_tmpD5;
struct Cyc_Absyn_Exp*_tmpD6;struct Cyc_Absyn_Exp*_tmpD7;struct Cyc_Absyn_Exp*_tmpD8;
struct Cyc_Absyn_Exp*_tmpD9;struct Cyc_List_List*_tmpDA;struct Cyc_Absyn_MallocInfo
_tmpDB;int _tmpDC;struct Cyc_Absyn_Exp*_tmpDD;void**_tmpDE;struct Cyc_Absyn_Exp*
_tmpDF;int _tmpE0;struct Cyc_Absyn_Exp*_tmpE1;struct Cyc_Absyn_Exp*_tmpE2;struct Cyc_Absyn_Exp*
_tmpE3;struct Cyc_Absyn_Exp*_tmpE4;struct Cyc_Absyn_Exp*_tmpE5;struct
_dynforward_ptr*_tmpE6;struct Cyc_Absyn_Exp*_tmpE7;struct _dynforward_ptr*_tmpE8;
struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_Absyn_Exp*_tmpEB;
struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_Absyn_Exp*_tmpEE;
struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_Absyn_Exp*_tmpF1;
struct Cyc_List_List*_tmpF2;struct Cyc_Absyn_Tuniondecl*_tmpF3;struct Cyc_List_List*
_tmpF4;struct Cyc_List_List*_tmpF5;struct Cyc_List_List*_tmpF6;struct Cyc_List_List*
_tmpF7;struct Cyc_Absyn_Vardecl*_tmpF8;struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Absyn_Exp*
_tmpFA;int _tmpFB;struct Cyc_Absyn_Stmt*_tmpFC;void*_tmpFD;_LL8F: if(*((int*)_tmpB9)
!= 15)goto _LL91;_tmpBA=((struct Cyc_Absyn_Cast_e_struct*)_tmpB9)->f2;_tmpBB=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmpB9)->f4;if((int)_tmpBB != 2)goto _LL91;_LL90: {
union Cyc_CfFlowInfo_FlowInfo_union _tmpFF;void*_tmp100;struct _tuple5 _tmpFE=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpBA);_tmpFF=_tmpFE.f1;_tmp100=_tmpFE.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp102;void*_tmp103;struct _tuple5 _tmp101=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmpFF,_tmpBA,_tmp100);_tmp102=_tmp101.f1;_tmp103=_tmp101.f2;return({
struct _tuple5 _tmp104;_tmp104.f1=_tmp102;_tmp104.f2=_tmp100;_tmp104;});}}_LL91:
if(*((int*)_tmpB9)!= 15)goto _LL93;_tmpBC=((struct Cyc_Absyn_Cast_e_struct*)_tmpB9)->f2;
_LL92: _tmpBD=_tmpBC;goto _LL94;_LL93: if(*((int*)_tmpB9)!= 13)goto _LL95;_tmpBD=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpB9)->f1;_LL94: _tmpBE=_tmpBD;goto
_LL96;_LL95: if(*((int*)_tmpB9)!= 14)goto _LL97;_tmpBE=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmpB9)->f1;_LL96: return Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpBE);_LL97:
if(*((int*)_tmpB9)!= 0)goto _LL99;_tmpBF=((struct Cyc_Absyn_Const_e_struct*)_tmpB9)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmpB9)->f1).Null_c).tag != 6)goto _LL99;
_LL98: goto _LL9A;_LL99: if(*((int*)_tmpB9)!= 0)goto _LL9B;_tmpC0=((struct Cyc_Absyn_Const_e_struct*)
_tmpB9)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpB9)->f1).Int_c).tag != 2)
goto _LL9B;_tmpC1=(_tmpC0.Int_c).f2;if(_tmpC1 != 0)goto _LL9B;_LL9A: return({struct
_tuple5 _tmp105;_tmp105.f1=inflow;_tmp105.f2=(void*)0;_tmp105;});_LL9B: if(*((int*)
_tmpB9)!= 0)goto _LL9D;_tmpC2=((struct Cyc_Absyn_Const_e_struct*)_tmpB9)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpB9)->f1).Int_c).tag != 2)goto _LL9D;_LL9C: goto
_LL9E;_LL9D: if(*((int*)_tmpB9)!= 1)goto _LL9F;_tmpC3=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpB9)->f2;if(_tmpC3 <= (void*)1)goto _LL9F;if(*((int*)_tmpC3)!= 1)goto _LL9F;
_LL9E: return({struct _tuple5 _tmp106;_tmp106.f1=inflow;_tmp106.f2=(void*)1;_tmp106;});
_LL9F: if(*((int*)_tmpB9)!= 32)goto _LLA1;_tmpC4=((struct Cyc_Absyn_Tunion_e_struct*)
_tmpB9)->f1;if(_tmpC4 != 0)goto _LLA1;_LLA0: goto _LLA2;_LLA1: if(*((int*)_tmpB9)!= 0)
goto _LLA3;_LLA2: goto _LLA4;_LLA3: if(*((int*)_tmpB9)!= 19)goto _LLA5;_LLA4: goto
_LLA6;_LLA5: if(*((int*)_tmpB9)!= 18)goto _LLA7;_LLA6: goto _LLA8;_LLA7: if(*((int*)
_tmpB9)!= 20)goto _LLA9;_LLA8: goto _LLAA;_LLA9: if(*((int*)_tmpB9)!= 21)goto _LLAB;
_LLAA: goto _LLAC;_LLAB: if(*((int*)_tmpB9)!= 34)goto _LLAD;_LLAC: goto _LLAE;_LLAD:
if(*((int*)_tmpB9)!= 33)goto _LLAF;_LLAE: return({struct _tuple5 _tmp107;_tmp107.f1=
inflow;_tmp107.f2=Cyc_CfFlowInfo_unknown_all;_tmp107;});_LLAF: if(*((int*)_tmpB9)
!= 1)goto _LLB1;_tmpC5=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpB9)->f2;if(
_tmpC5 <= (void*)1)goto _LLB1;if(*((int*)_tmpC5)!= 0)goto _LLB1;_LLB0: return({
struct _tuple5 _tmp108;_tmp108.f1=inflow;_tmp108.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp108;});_LLB1: if(*((int*)_tmpB9)!= 1)goto _LLB3;_tmpC6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpB9)->f2;if(_tmpC6 <= (void*)1)goto _LLB3;if(*((int*)_tmpC6)!= 2)goto _LLB3;
_tmpC7=((struct Cyc_Absyn_Param_b_struct*)_tmpC6)->f1;_LLB2: _tmpC9=_tmpC7;goto
_LLB4;_LLB3: if(*((int*)_tmpB9)!= 1)goto _LLB5;_tmpC8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpB9)->f2;if(_tmpC8 <= (void*)1)goto _LLB5;if(*((int*)_tmpC8)!= 3)goto _LLB5;
_tmpC9=((struct Cyc_Absyn_Local_b_struct*)_tmpC8)->f1;_LLB4: _tmpCB=_tmpC9;goto
_LLB6;_LLB5: if(*((int*)_tmpB9)!= 1)goto _LLB7;_tmpCA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpB9)->f2;if(_tmpCA <= (void*)1)goto _LLB7;if(*((int*)_tmpCA)!= 4)goto _LLB7;
_tmpCB=((struct Cyc_Absyn_Pat_b_struct*)_tmpCA)->f1;_LLB6: if(Cyc_CfFlowInfo_contains_region((
void*)0,(void*)_tmpCB->type)){struct Cyc_CfFlowInfo_VarRoot_struct*_tmp109=({
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E[
0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp10F;_tmp10F.tag=0;_tmp10F.f1=_tmpCB;
_tmp10F;});_tmp10E;});struct Cyc_CfFlowInfo_Place*_tmp10A=({struct Cyc_CfFlowInfo_Place*
_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D->root=(void*)((void*)_tmp109);
_tmp10D->fields=0;_tmp10D;});cinfo.may_consume=((struct Cyc_List_List*(*)(int(*
cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_merge)(Cyc_CfFlowInfo_place_cmp,({struct Cyc_List_List*
_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->hd=_tmp10A;_tmp10B->tl=0;_tmp10B;}),
cinfo.may_consume);inflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp10C;(_tmp10C.ReachableFL).tag=1;(_tmp10C.ReachableFL).f1=d;(_tmp10C.ReachableFL).f2=
relns;(_tmp10C.ReachableFL).f3=cinfo;_tmp10C;});}return({struct _tuple5 _tmp110;
_tmp110.f1=inflow;_tmp110.f2=Cyc_Dict_lookup(d,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp112;_tmp112.tag=0;_tmp112.f1=_tmpCB;_tmp112;});_tmp111;}));_tmp110;});_LLB7:
if(*((int*)_tmpB9)!= 3)goto _LLB9;_tmpCC=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmpB9)->f1;_tmpCD=((struct Cyc_Absyn_Primop_e_struct*)_tmpB9)->f2;_LLB8: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp114;void*_tmp115;struct _tuple5 _tmp113=Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmpCD,0);_tmp114=_tmp113.f1;_tmp115=_tmp113.f2;{void*_tmp116=_tmpCC;
_LLF2: if((int)_tmp116 != 0)goto _LLF4;_LLF3: goto _LLF5;_LLF4: if((int)_tmp116 != 2)
goto _LLF6;_LLF5: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpCD))->hd)->loc,_tmp114);goto _LLF1;_LLF6:;
_LLF7: _tmp114=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpCD))->hd)->loc,_tmp114);goto _LLF1;_LLF1:;}
return({struct _tuple5 _tmp117;_tmp117.f1=_tmp114;_tmp117.f2=_tmp115;_tmp117;});}
_LLB9: if(*((int*)_tmpB9)!= 5)goto _LLBB;_tmpCE=((struct Cyc_Absyn_Increment_e_struct*)
_tmpB9)->f1;_LLBA: {union Cyc_CfFlowInfo_FlowInfo_union _tmp11A;struct _tuple5
_tmp119=Cyc_NewControlFlow_anal_use_ints(env,inflow,({struct Cyc_Absyn_Exp*
_tmp118[1];_tmp118[0]=_tmpCE;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp118,sizeof(struct Cyc_Absyn_Exp*),1));}),0);
_tmp11A=_tmp119.f1;Cyc_CfFlowInfo_check_unique_rvals(_tmpCE->loc,_tmp11A);{void*
_tmp11C;struct _tuple5 _tmp11B=Cyc_NewControlFlow_anal_Lexp(env,_tmp11A,_tmpCE);
_tmp11C=_tmp11B.f2;{struct _tuple10 _tmp11E=({struct _tuple10 _tmp11D;_tmp11D.f1=
_tmp11C;_tmp11D.f2=_tmp11A;_tmp11D;});void*_tmp11F;struct Cyc_CfFlowInfo_Place*
_tmp120;union Cyc_CfFlowInfo_FlowInfo_union _tmp121;struct Cyc_Dict_Dict*_tmp122;
struct Cyc_List_List*_tmp123;struct Cyc_CfFlowInfo_ConsumeInfo _tmp124;_LLF9:
_tmp11F=_tmp11E.f1;if(_tmp11F <= (void*)1)goto _LLFB;if(*((int*)_tmp11F)!= 0)goto
_LLFB;_tmp120=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp11F)->f1;_tmp121=_tmp11E.f2;
if(((_tmp11E.f2).ReachableFL).tag != 1)goto _LLFB;_tmp122=(_tmp121.ReachableFL).f1;
_tmp123=(_tmp121.ReachableFL).f2;_tmp124=(_tmp121.ReachableFL).f3;_LLFA: _tmp123=
Cyc_CfFlowInfo_reln_kill_exp(_tmp123,_tmpCE);_tmp11A=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp125;(_tmp125.ReachableFL).tag=1;(_tmp125.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(_tmpCE->loc,_tmp122,env->all_changed,_tmp120,Cyc_CfFlowInfo_unknown_all);(
_tmp125.ReachableFL).f2=_tmp123;(_tmp125.ReachableFL).f3=_tmp124;_tmp125;});goto
_LLF8;_LLFB:;_LLFC: goto _LLF8;_LLF8:;}return({struct _tuple5 _tmp126;_tmp126.f1=
_tmp11A;_tmp126.f2=Cyc_CfFlowInfo_unknown_all;_tmp126;});}}_LLBB: if(*((int*)
_tmpB9)!= 4)goto _LLBD;_tmpCF=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpB9)->f1;
_tmpD0=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpB9)->f2;if(_tmpD0 == 0)goto _LLBD;
_tmpD1=*_tmpD0;_tmpD2=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpB9)->f3;_LLBC: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp129;struct _tuple5 _tmp128=Cyc_NewControlFlow_anal_use_ints(
env,inflow,({struct Cyc_Absyn_Exp*_tmp127[2];_tmp127[1]=_tmpD2;_tmp127[0]=_tmpCF;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp127,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp129=_tmp128.f1;{void*_tmp12B;
struct _tuple5 _tmp12A=Cyc_NewControlFlow_anal_Lexp(env,_tmp129,e);_tmp12B=_tmp12A.f2;
_tmp129=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp129);{struct _tuple10
_tmp12D=({struct _tuple10 _tmp12C;_tmp12C.f1=_tmp12B;_tmp12C.f2=_tmp129;_tmp12C;});
void*_tmp12E;struct Cyc_CfFlowInfo_Place*_tmp12F;union Cyc_CfFlowInfo_FlowInfo_union
_tmp130;struct Cyc_Dict_Dict*_tmp131;struct Cyc_List_List*_tmp132;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp133;_LLFE: _tmp12E=_tmp12D.f1;if(_tmp12E <= (void*)1)goto _LL100;if(*((int*)
_tmp12E)!= 0)goto _LL100;_tmp12F=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp12E)->f1;
_tmp130=_tmp12D.f2;if(((_tmp12D.f2).ReachableFL).tag != 1)goto _LL100;_tmp131=(
_tmp130.ReachableFL).f1;_tmp132=(_tmp130.ReachableFL).f2;_tmp133=(_tmp130.ReachableFL).f3;
_LLFF: _tmp132=Cyc_CfFlowInfo_reln_kill_exp(_tmp132,_tmpCF);_tmp131=Cyc_CfFlowInfo_assign_place(
_tmpCF->loc,_tmp131,env->all_changed,_tmp12F,Cyc_CfFlowInfo_unknown_all);if(Cyc_CfFlowInfo_contains_region((
void*)0,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpCF->topt))->v)){_tmp133.may_consume=((
struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_merge)(Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->hd=_tmp12F;
_tmp134->tl=0;_tmp134;}),_tmp133.may_consume);if(!((int(*)(struct Cyc_Dict_Dict*d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp133.consumed,_tmp12F)){struct
Cyc_CfFlowInfo_Place _tmp136;void*_tmp137;struct Cyc_CfFlowInfo_Place*_tmp135=
_tmp12F;_tmp136=*_tmp135;_tmp137=(void*)_tmp136.root;{void*_tmp138=_tmp137;
struct Cyc_Absyn_Vardecl*_tmp139;_LL103: if(*((int*)_tmp138)!= 0)goto _LL105;
_tmp139=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp138)->f1;_LL104: {struct
_dynforward_ptr _tmp13A=Cyc_Absynpp_qvar2string(_tmp139->name);({struct Cyc_String_pa_struct
_tmp13D;_tmp13D.tag=0;_tmp13D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp13A);{void*_tmp13B[1]={& _tmp13D};Cyc_Tcutil_warn(_tmpCF->loc,({const char*
_tmp13C="may clobber unique pointer %s";_tag_dynforward(_tmp13C,sizeof(char),
_get_zero_arr_size(_tmp13C,30));}),_tag_dynforward(_tmp13B,sizeof(void*),1));}});
goto _LL102;}_LL105:;_LL106:({void*_tmp13E[0]={};((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp13F="warning locations not for VarRoots";
_tag_dynforward(_tmp13F,sizeof(char),_get_zero_arr_size(_tmp13F,35));}),
_tag_dynforward(_tmp13E,sizeof(void*),0));});_LL102:;}}if(env->all_changed == 0){
struct Cyc_Dict_Dict*_tmp140=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,
struct Cyc_CfFlowInfo_Place*))Cyc_Dict_delete)(_tmp133.consumed,_tmp12F);if(
_tmp133.consumed != _tmp140);_tmp133.consumed=_tmp140;}}_tmp129=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp141;(_tmp141.ReachableFL).tag=1;(_tmp141.ReachableFL).f1=
_tmp131;(_tmp141.ReachableFL).f2=_tmp132;(_tmp141.ReachableFL).f3=_tmp133;
_tmp141;});goto _LLFD;_LL100:;_LL101: goto _LLFD;_LLFD:;}return({struct _tuple5
_tmp142;_tmp142.f1=_tmp129;_tmp142.f2=Cyc_CfFlowInfo_unknown_all;_tmp142;});}}
_LLBD: if(*((int*)_tmpB9)!= 4)goto _LLBF;_tmpD3=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpB9)->f1;_tmpD4=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpB9)->f2;if(_tmpD4 != 
0)goto _LLBF;_tmpD5=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpB9)->f3;_LLBE: {
struct Cyc_Dict_Dict**_tmp143=env->all_changed;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp145;union Cyc_CfFlowInfo_FlowInfo_union _tmp146;struct _tuple6 _tmp144=Cyc_CfFlowInfo_save_consume_info(
inflow,1);_tmp145=_tmp144.f1;_tmp146=_tmp144.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
empty_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp16F;_tmp16F.consumed=Cyc_CfFlowInfo_mt_place_set();
_tmp16F.may_consume=0;_tmp16F;});struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=
empty_consume;int init_consume=0;while(1){env->all_changed=({struct Cyc_Dict_Dict**
_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp147;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp149;void*_tmp14A;struct _tuple5
_tmp148=Cyc_NewControlFlow_anal_Lexp(env,_tmp146,_tmpD3);_tmp149=_tmp148.f1;
_tmp14A=_tmp148.f2;{struct Cyc_Dict_Dict*_tmp14B=*((struct Cyc_Dict_Dict**)
_check_null(env->all_changed));env->all_changed=({struct Cyc_Dict_Dict**_tmp14C=
_cycalloc(sizeof(*_tmp14C));_tmp14C[0]=Cyc_CfFlowInfo_mt_place_set();_tmp14C;});{
union Cyc_CfFlowInfo_FlowInfo_union _tmp14E;void*_tmp14F;struct _tuple5 _tmp14D=Cyc_NewControlFlow_anal_Rexp(
env,_tmp146,_tmpD5);_tmp14E=_tmp14D.f1;_tmp14F=_tmp14D.f2;{struct Cyc_Dict_Dict*
_tmp150=*((struct Cyc_Dict_Dict**)_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo_union
_tmp151=Cyc_CfFlowInfo_after_flow((struct Cyc_Dict_Dict**)& _tmp14B,_tmp149,
_tmp14E,_tmp14B,_tmp150);union Cyc_CfFlowInfo_FlowInfo_union _tmp152=Cyc_CfFlowInfo_join_flow(
_tmp143,_tmp146,_tmp151,1);struct Cyc_CfFlowInfo_ConsumeInfo _tmp154;struct _tuple6
_tmp153=Cyc_CfFlowInfo_save_consume_info(_tmp152,0);_tmp154=_tmp153.f1;if(
init_consume)_tmp146=Cyc_CfFlowInfo_restore_consume_info(_tmp146,outflow_consume);
outflow_consume=_tmp154;init_consume=1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp152,_tmp146)){if(_tmp143 == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict**)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp143,Cyc_CfFlowInfo_union_place_set(
_tmp14B,_tmp150,0),0);}_tmp151=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp151);{union Cyc_CfFlowInfo_FlowInfo_union _tmp155=_tmp151;struct Cyc_Dict_Dict*
_tmp156;struct Cyc_List_List*_tmp157;struct Cyc_CfFlowInfo_ConsumeInfo _tmp158;
_LL108: if((_tmp155.BottomFL).tag != 0)goto _LL10A;_LL109: return({struct _tuple5
_tmp159;_tmp159.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp15A;(_tmp15A.BottomFL).tag=0;_tmp15A;});_tmp159.f2=_tmp14F;_tmp159;});_LL10A:
if((_tmp155.ReachableFL).tag != 1)goto _LL107;_tmp156=(_tmp155.ReachableFL).f1;
_tmp157=(_tmp155.ReachableFL).f2;_tmp158=(_tmp155.ReachableFL).f3;_LL10B: _tmp158=
Cyc_CfFlowInfo_and_consume(_tmp145,_tmp158);{void*_tmp15B=_tmp14A;struct Cyc_CfFlowInfo_Place*
_tmp15C;_LL10D: if(_tmp15B <= (void*)1)goto _LL10F;if(*((int*)_tmp15B)!= 0)goto
_LL10F;_tmp15C=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp15B)->f1;_LL10E: _tmp156=
Cyc_CfFlowInfo_assign_place(e->loc,_tmp156,env->all_changed,_tmp15C,_tmp14F);if(
Cyc_CfFlowInfo_contains_region((void*)0,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmpD3->topt))->v)){_tmp158.may_consume=((struct Cyc_List_List*(*)(int(*cmp)(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_merge)(Cyc_CfFlowInfo_place_cmp,({struct Cyc_List_List*
_tmp15D=_cycalloc(sizeof(*_tmp15D));_tmp15D->hd=_tmp15C;_tmp15D->tl=0;_tmp15D;}),
_tmp158.may_consume);if(!((int(*)(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
k))Cyc_Dict_member)(_tmp158.consumed,_tmp15C)){struct Cyc_CfFlowInfo_Place _tmp15F;
void*_tmp160;struct Cyc_CfFlowInfo_Place*_tmp15E=_tmp15C;_tmp15F=*_tmp15E;_tmp160=(
void*)_tmp15F.root;{void*_tmp161=_tmp160;struct Cyc_Absyn_Vardecl*_tmp162;_LL112:
if(*((int*)_tmp161)!= 0)goto _LL114;_tmp162=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp161)->f1;_LL113: {struct _dynforward_ptr _tmp163=Cyc_Absynpp_qvar2string(
_tmp162->name);({struct Cyc_String_pa_struct _tmp166;_tmp166.tag=0;_tmp166.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp163);{void*_tmp164[1]={&
_tmp166};Cyc_Tcutil_warn(_tmpD3->loc,({const char*_tmp165="may clobber unique pointer %s";
_tag_dynforward(_tmp165,sizeof(char),_get_zero_arr_size(_tmp165,30));}),
_tag_dynforward(_tmp164,sizeof(void*),1));}});goto _LL111;}_LL114:;_LL115:({void*
_tmp167[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp168="warning locations not for VarRoots";_tag_dynforward(_tmp168,
sizeof(char),_get_zero_arr_size(_tmp168,35));}),_tag_dynforward(_tmp167,sizeof(
void*),0));});_LL111:;}}if(env->all_changed == 0){struct Cyc_Dict_Dict*_tmp169=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,struct Cyc_CfFlowInfo_Place*))Cyc_Dict_delete)(
_tmp158.consumed,_tmp15C);if(_tmp158.consumed != _tmp169);_tmp158.consumed=
_tmp169;}}_tmp157=Cyc_CfFlowInfo_reln_assign_exp(_tmp157,_tmpD3,_tmpD5);_tmp151=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp16A;(
_tmp16A.ReachableFL).tag=1;(_tmp16A.ReachableFL).f1=_tmp156;(_tmp16A.ReachableFL).f2=
_tmp157;(_tmp16A.ReachableFL).f3=_tmp158;_tmp16A;});Cyc_NewControlFlow_update_tryflow(
env,_tmp151);return({struct _tuple5 _tmp16B;_tmp16B.f1=_tmp151;_tmp16B.f2=_tmp14F;
_tmp16B;});_LL10F: if((int)_tmp15B != 0)goto _LL10C;_LL110: if(Cyc_CfFlowInfo_contains_region((
void*)0,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpD3->topt))->v))({void*
_tmp16C[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp16D="dest of unique assignment is unknown!";_tag_dynforward(
_tmp16D,sizeof(char),_get_zero_arr_size(_tmp16D,38));}),_tag_dynforward(_tmp16C,
sizeof(void*),0));});return({struct _tuple5 _tmp16E;_tmp16E.f1=Cyc_NewControlFlow_use_Rval(
env,_tmpD5->loc,_tmp151,_tmp14F);_tmp16E.f2=_tmp14F;_tmp16E;});_LL10C:;}_LL107:;}}
_tmp146=Cyc_CfFlowInfo_restore_consume_info(_tmp152,empty_consume);}}}}}}}_LLBF:
if(*((int*)_tmpB9)!= 9)goto _LLC1;_tmpD6=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpB9)->f1;_tmpD7=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpB9)->f2;_LLC0: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp171;void*_tmp172;struct _tuple5 _tmp170=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpD6);_tmp171=_tmp170.f1;_tmp172=_tmp170.f2;_tmp171=Cyc_CfFlowInfo_drop_unique_rvals(
_tmpD6->loc,_tmp171);return Cyc_NewControlFlow_anal_Rexp(env,_tmp171,_tmpD7);}
_LLC1: if(*((int*)_tmpB9)!= 12)goto _LLC3;_tmpD8=((struct Cyc_Absyn_Throw_e_struct*)
_tmpB9)->f1;_LLC2: {union Cyc_CfFlowInfo_FlowInfo_union _tmp174;void*_tmp175;
struct _tuple5 _tmp173=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpD8);_tmp174=
_tmp173.f1;_tmp175=_tmp173.f2;_tmp174=Cyc_CfFlowInfo_consume_unique_rvals(_tmpD8->loc,
_tmp174);Cyc_NewControlFlow_use_Rval(env,_tmpD8->loc,_tmp174,_tmp175);return({
struct _tuple5 _tmp176;_tmp176.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp177;(_tmp177.BottomFL).tag=0;_tmp177;});_tmp176.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp176;});}_LLC3: if(*((int*)_tmpB9)!= 11)goto _LLC5;_tmpD9=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpB9)->f1;_tmpDA=((struct Cyc_Absyn_FnCall_e_struct*)_tmpB9)->f2;_LLC4: {struct
_RegionHandle _tmp178=_new_region("temp");struct _RegionHandle*temp=& _tmp178;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp17B;struct Cyc_List_List*
_tmp17C;struct _tuple9 _tmp17A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_List_List*_tmp179=_region_malloc(temp,sizeof(*_tmp179));
_tmp179->hd=_tmpD9;_tmp179->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmpDA);_tmp179;}),1);_tmp17B=
_tmp17A.f1;_tmp17C=_tmp17A.f2;_tmp17B=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp17B);{union Cyc_CfFlowInfo_FlowInfo_union _tmp17D=Cyc_NewControlFlow_use_Rval(
env,_tmpD9->loc,_tmp17B,(void*)((struct Cyc_List_List*)_check_null(_tmp17C))->hd);
_tmp17C=_tmp17C->tl;{struct Cyc_List_List*init_params=0;{void*_tmp17E=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD9->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp17F;void*_tmp180;_LL117: if(_tmp17E <= (void*)4)goto _LL119;if(*((int*)_tmp17E)
!= 4)goto _LL119;_tmp17F=((struct Cyc_Absyn_PointerType_struct*)_tmp17E)->f1;
_tmp180=(void*)_tmp17F.elt_typ;_LL118:{void*_tmp181=Cyc_Tcutil_compress(_tmp180);
struct Cyc_Absyn_FnInfo _tmp182;struct Cyc_List_List*_tmp183;_LL11C: if(_tmp181 <= (
void*)4)goto _LL11E;if(*((int*)_tmp181)!= 8)goto _LL11E;_tmp182=((struct Cyc_Absyn_FnType_struct*)
_tmp181)->f1;_tmp183=_tmp182.attributes;_LL11D: for(0;_tmp183 != 0;_tmp183=_tmp183->tl){
void*_tmp184=(void*)_tmp183->hd;int _tmp185;_LL121: if(_tmp184 <= (void*)17)goto
_LL123;if(*((int*)_tmp184)!= 4)goto _LL123;_tmp185=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp184)->f1;_LL122: init_params=({struct Cyc_List_List*_tmp186=_region_malloc(
temp,sizeof(*_tmp186));_tmp186->hd=(void*)_tmp185;_tmp186->tl=init_params;
_tmp186;});goto _LL120;_LL123:;_LL124: goto _LL120;_LL120:;}goto _LL11B;_LL11E:;
_LL11F:({void*_tmp187[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp188="anal_Rexp: bad function type";
_tag_dynforward(_tmp188,sizeof(char),_get_zero_arr_size(_tmp188,29));}),
_tag_dynforward(_tmp187,sizeof(void*),0));});_LL11B:;}goto _LL116;_LL119:;_LL11A:({
void*_tmp189[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp18A="anal_Rexp: bad function type";
_tag_dynforward(_tmp18A,sizeof(char),_get_zero_arr_size(_tmp18A,29));}),
_tag_dynforward(_tmp189,sizeof(void*),0));});_LL116:;}{int i=1;for(0;_tmp17C != 0;(((
_tmp17C=_tmp17C->tl,_tmpDA=((struct Cyc_List_List*)_check_null(_tmpDA))->tl)),++ i)){
if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){_tmp17D=
Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpDA))->hd)->loc,_tmp17D,(void*)_tmp17C->hd);continue;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp18B=_tmp17B;struct Cyc_Dict_Dict*_tmp18C;_LL126: if((_tmp18B.BottomFL).tag != 0)
goto _LL128;_LL127: goto _LL125;_LL128: if((_tmp18B.ReachableFL).tag != 1)goto _LL125;
_tmp18C=(_tmp18B.ReachableFL).f1;_LL129: if(Cyc_CfFlowInfo_initlevel(_tmp18C,(
void*)_tmp17C->hd)== (void*)0)({void*_tmp18D[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpDA))->hd)->loc,({const char*_tmp18E="expression may not be initialized";
_tag_dynforward(_tmp18E,sizeof(char),_get_zero_arr_size(_tmp18E,34));}),
_tag_dynforward(_tmp18D,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp18F=_tmp17D;struct Cyc_Dict_Dict*_tmp190;struct Cyc_List_List*_tmp191;struct
Cyc_CfFlowInfo_ConsumeInfo _tmp192;_LL12B: if((_tmp18F.BottomFL).tag != 0)goto
_LL12D;_LL12C: goto _LL12A;_LL12D: if((_tmp18F.ReachableFL).tag != 1)goto _LL12A;
_tmp190=(_tmp18F.ReachableFL).f1;_tmp191=(_tmp18F.ReachableFL).f2;_tmp192=(
_tmp18F.ReachableFL).f3;_LL12E: {struct Cyc_Dict_Dict*_tmp193=Cyc_CfFlowInfo_escape_deref(
_tmp190,env->all_changed,(void*)_tmp17C->hd);{void*_tmp194=(void*)_tmp17C->hd;
struct Cyc_CfFlowInfo_Place*_tmp195;_LL130: if(_tmp194 <= (void*)3)goto _LL132;if(*((
int*)_tmp194)!= 2)goto _LL132;_tmp195=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp194)->f1;_LL131:{void*_tmp196=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpDA))->hd)->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp197;void*_tmp198;_LL135: if(_tmp196 <= (void*)4)goto
_LL137;if(*((int*)_tmp196)!= 4)goto _LL137;_tmp197=((struct Cyc_Absyn_PointerType_struct*)
_tmp196)->f1;_tmp198=(void*)_tmp197.elt_typ;_LL136: _tmp193=Cyc_CfFlowInfo_assign_place(((
struct Cyc_Absyn_Exp*)_tmpDA->hd)->loc,_tmp193,env->all_changed,_tmp195,Cyc_CfFlowInfo_typ_to_absrval(
_tmp198,Cyc_CfFlowInfo_esc_all));goto _LL134;_LL137:;_LL138:({void*_tmp199[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp19A="anal_Rexp:bad type for initialized arg";_tag_dynforward(
_tmp19A,sizeof(char),_get_zero_arr_size(_tmp19A,39));}),_tag_dynforward(_tmp199,
sizeof(void*),0));});_LL134:;}goto _LL12F;_LL132:;_LL133: goto _LL12F;_LL12F:;}
_tmp17D=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp19B;(_tmp19B.ReachableFL).tag=1;(_tmp19B.ReachableFL).f1=_tmp193;(_tmp19B.ReachableFL).f2=
_tmp191;(_tmp19B.ReachableFL).f3=_tmp192;_tmp19B;});goto _LL12A;}_LL12A:;}goto
_LL125;_LL125:;}}}if(Cyc_Tcutil_is_noreturn((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpD9->topt))->v)){struct _tuple5 _tmp19E=({struct _tuple5 _tmp19C;
_tmp19C.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp19D;(_tmp19D.BottomFL).tag=0;_tmp19D;});_tmp19C.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp19C;});_npop_handler(0);return _tmp19E;}else{struct _tuple5 _tmp1A0=({struct
_tuple5 _tmp19F;_tmp19F.f1=_tmp17D;_tmp19F.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp19F;});
_npop_handler(0);return _tmp1A0;}}}};_pop_region(temp);}_LLC5: if(*((int*)_tmpB9)
!= 35)goto _LLC7;_tmpDB=((struct Cyc_Absyn_Malloc_e_struct*)_tmpB9)->f1;_tmpDC=
_tmpDB.is_calloc;_tmpDD=_tmpDB.rgn;_tmpDE=_tmpDB.elt_type;_tmpDF=_tmpDB.num_elts;
_tmpE0=_tmpDB.fat_result;_LLC6: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp1B2=_cycalloc(sizeof(*_tmp1B2));_tmp1B2[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp1B3;_tmp1B3.tag=1;_tmp1B3.f1=_tmpDF;_tmp1B3.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp1B3;});_tmp1B2;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1->root=(
void*)root;_tmp1B1->fields=0;_tmp1B1;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp1B0;_tmp1B0.tag=2;_tmp1B0.f1=place;_tmp1B0;});_tmp1AF;});void*place_val=
_tmpE0?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(*((void**)_check_null(_tmpDE)),Cyc_CfFlowInfo_unknown_none);
union Cyc_CfFlowInfo_FlowInfo_union outflow;Cyc_CfFlowInfo_update_place_set(env->all_changed,
place,0);if(_tmpDD != 0){struct _RegionHandle _tmp1A1=_new_region("temp");struct
_RegionHandle*temp=& _tmp1A1;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1A4;struct Cyc_List_List*_tmp1A5;struct _tuple9 _tmp1A3=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp1A2[2];_tmp1A2[1]=_tmpDF;_tmp1A2[0]=(
struct Cyc_Absyn_Exp*)_tmpDD;((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(_tmp1A2,sizeof(
struct Cyc_Absyn_Exp*),2));}),0);_tmp1A4=_tmp1A3.f1;_tmp1A5=_tmp1A3.f2;outflow=
_tmp1A4;};_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo_union _tmp1A7;
struct _tuple5 _tmp1A6=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpDF);_tmp1A7=
_tmp1A6.f1;outflow=_tmp1A7;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpDF->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1A8=outflow;struct
Cyc_Dict_Dict*_tmp1A9;struct Cyc_List_List*_tmp1AA;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1AB;_LL13A: if((_tmp1A8.BottomFL).tag != 0)goto _LL13C;_LL13B: return({struct
_tuple5 _tmp1AC;_tmp1AC.f1=outflow;_tmp1AC.f2=rval;_tmp1AC;});_LL13C: if((_tmp1A8.ReachableFL).tag
!= 1)goto _LL139;_tmp1A9=(_tmp1A8.ReachableFL).f1;_tmp1AA=(_tmp1A8.ReachableFL).f2;
_tmp1AB=(_tmp1A8.ReachableFL).f3;_LL13D: return({struct _tuple5 _tmp1AD;_tmp1AD.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp1AE;(
_tmp1AE.ReachableFL).tag=1;(_tmp1AE.ReachableFL).f1=Cyc_Dict_insert(_tmp1A9,root,
place_val);(_tmp1AE.ReachableFL).f2=_tmp1AA;(_tmp1AE.ReachableFL).f3=_tmp1AB;
_tmp1AE;});_tmp1AD.f2=rval;_tmp1AD;});_LL139:;}}_LLC7: if(*((int*)_tmpB9)!= 17)
goto _LLC9;_tmpE1=((struct Cyc_Absyn_New_e_struct*)_tmpB9)->f1;_tmpE2=((struct Cyc_Absyn_New_e_struct*)
_tmpB9)->f2;_LLC8: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp1C7;_tmp1C7.tag=1;_tmp1C7.f1=_tmpE2;_tmp1C7.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp1C7;});_tmp1C6;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5->root=(
void*)root;_tmp1C5->fields=0;_tmp1C5;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp1C4;_tmp1C4.tag=2;_tmp1C4.f1=place;_tmp1C4;});_tmp1C3;});Cyc_CfFlowInfo_update_place_set(
env->all_changed,place,0);{union Cyc_CfFlowInfo_FlowInfo_union outflow;void*
place_val;if(_tmpE1 != 0){struct _RegionHandle _tmp1B4=_new_region("temp");struct
_RegionHandle*temp=& _tmp1B4;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1B7;struct Cyc_List_List*_tmp1B8;struct _tuple9 _tmp1B6=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp1B5[2];_tmp1B5[1]=_tmpE2;_tmp1B5[0]=(
struct Cyc_Absyn_Exp*)_tmpE1;((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(_tmp1B5,sizeof(
struct Cyc_Absyn_Exp*),2));}),0);_tmp1B7=_tmp1B6.f1;_tmp1B8=_tmp1B6.f2;outflow=
_tmp1B7;place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp1B8))->tl))->hd;};_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1BA;void*_tmp1BB;struct _tuple5 _tmp1B9=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpE2);_tmp1BA=_tmp1B9.f1;_tmp1BB=_tmp1B9.f2;outflow=_tmp1BA;place_val=
_tmp1BB;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpE2->loc,outflow);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp1BC=outflow;struct Cyc_Dict_Dict*_tmp1BD;
struct Cyc_List_List*_tmp1BE;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1BF;_LL13F: if((
_tmp1BC.BottomFL).tag != 0)goto _LL141;_LL140: return({struct _tuple5 _tmp1C0;_tmp1C0.f1=
outflow;_tmp1C0.f2=rval;_tmp1C0;});_LL141: if((_tmp1BC.ReachableFL).tag != 1)goto
_LL13E;_tmp1BD=(_tmp1BC.ReachableFL).f1;_tmp1BE=(_tmp1BC.ReachableFL).f2;_tmp1BF=(
_tmp1BC.ReachableFL).f3;_LL142: return({struct _tuple5 _tmp1C1;_tmp1C1.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp1C2;(_tmp1C2.ReachableFL).tag=1;(_tmp1C2.ReachableFL).f1=
Cyc_Dict_insert(_tmp1BD,root,place_val);(_tmp1C2.ReachableFL).f2=_tmp1BE;(
_tmp1C2.ReachableFL).f3=_tmp1BF;_tmp1C2;});_tmp1C1.f2=rval;_tmp1C1;});_LL13E:;}}}
_LLC9: if(*((int*)_tmpB9)!= 16)goto _LLCB;_tmpE3=((struct Cyc_Absyn_Address_e_struct*)
_tmpB9)->f1;_LLCA: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1C9;void*_tmp1CA;
struct _tuple5 _tmp1C8=Cyc_NewControlFlow_anal_Lexp(env,inflow,_tmpE3);_tmp1C9=
_tmp1C8.f1;_tmp1CA=_tmp1C8.f2;{void*_tmp1CB=_tmp1CA;struct Cyc_CfFlowInfo_Place*
_tmp1CC;_LL144: if((int)_tmp1CB != 0)goto _LL146;_LL145: return({struct _tuple5
_tmp1CD;_tmp1CD.f1=_tmp1C9;_tmp1CD.f2=(void*)1;_tmp1CD;});_LL146: if(_tmp1CB <= (
void*)1)goto _LL143;if(*((int*)_tmp1CB)!= 0)goto _LL143;_tmp1CC=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp1CB)->f1;_LL147: return({struct _tuple5 _tmp1CE;_tmp1CE.f1=_tmp1C9;_tmp1CE.f2=(
void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));
_tmp1CF[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp1D0;_tmp1D0.tag=2;_tmp1D0.f1=
_tmp1CC;_tmp1D0;});_tmp1CF;});_tmp1CE;});_LL143:;}}_LLCB: if(*((int*)_tmpB9)!= 22)
goto _LLCD;_tmpE4=((struct Cyc_Absyn_Deref_e_struct*)_tmpB9)->f1;_LLCC: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp1D2;void*_tmp1D3;struct _tuple5 _tmp1D1=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpE4);_tmp1D2=_tmp1D1.f1;_tmp1D3=_tmp1D1.f2;_tmp1D2=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp1D2);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1D2,_tmpE4,
_tmp1D3);}_LLCD: if(*((int*)_tmpB9)!= 23)goto _LLCF;_tmpE5=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpB9)->f1;_tmpE6=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpB9)->f2;_LLCE: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp1D5;void*_tmp1D6;struct _tuple5 _tmp1D4=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpE5);_tmp1D5=_tmp1D4.f1;_tmp1D6=_tmp1D4.f2;if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpE5->topt))->v))return({struct _tuple5
_tmp1D7;_tmp1D7.f1=_tmp1D5;_tmp1D7.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp1D7;});{
void*_tmp1D8=_tmp1D6;struct Cyc_Dict_Dict*_tmp1D9;_LL149: if(_tmp1D8 <= (void*)3)
goto _LL14B;if(*((int*)_tmp1D8)!= 4)goto _LL14B;_tmp1D9=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1D8)->f1;_LL14A: return({struct _tuple5 _tmp1DA;_tmp1DA.f1=_tmp1D5;_tmp1DA.f2=((
void*(*)(struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp1D9,
_tmpE6);_tmp1DA;});_LL14B:;_LL14C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1DB=_cycalloc(sizeof(*_tmp1DB));_tmp1DB[0]=({struct Cyc_Core_Impossible_struct
_tmp1DC;_tmp1DC.tag=Cyc_Core_Impossible;_tmp1DC.f1=({const char*_tmp1DD="anal_Rexp: AggrMember";
_tag_dynforward(_tmp1DD,sizeof(char),_get_zero_arr_size(_tmp1DD,22));});_tmp1DC;});
_tmp1DB;}));_LL148:;}}_LLCF: if(*((int*)_tmpB9)!= 24)goto _LLD1;_tmpE7=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpB9)->f1;_tmpE8=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpB9)->f2;_LLD0: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp1DF;void*_tmp1E0;struct _tuple5 _tmp1DE=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpE7);_tmp1DF=_tmp1DE.f1;_tmp1E0=_tmp1DE.f2;_tmp1DF=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp1DF);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1E2;void*_tmp1E3;struct
_tuple5 _tmp1E1=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1DF,_tmpE7,_tmp1E0);
_tmp1E2=_tmp1E1.f1;_tmp1E3=_tmp1E1.f2;{void*_tmp1E4=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpE7->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1E5;void*_tmp1E6;_LL14E: if(_tmp1E4 <= (void*)4)goto _LL150;if(*((int*)_tmp1E4)
!= 4)goto _LL150;_tmp1E5=((struct Cyc_Absyn_PointerType_struct*)_tmp1E4)->f1;
_tmp1E6=(void*)_tmp1E5.elt_typ;_LL14F: if(Cyc_Absyn_is_union_type(_tmp1E6))return({
struct _tuple5 _tmp1E7;_tmp1E7.f1=_tmp1E2;_tmp1E7.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp1E7;});goto _LL14D;_LL150:;_LL151:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8[0]=({struct Cyc_Core_Impossible_struct
_tmp1E9;_tmp1E9.tag=Cyc_Core_Impossible;_tmp1E9.f1=({const char*_tmp1EA="anal_Rexp: AggrArrow ptr";
_tag_dynforward(_tmp1EA,sizeof(char),_get_zero_arr_size(_tmp1EA,25));});_tmp1E9;});
_tmp1E8;}));_LL14D:;}{void*_tmp1EB=_tmp1E3;struct Cyc_Dict_Dict*_tmp1EC;_LL153:
if(_tmp1EB <= (void*)3)goto _LL155;if(*((int*)_tmp1EB)!= 4)goto _LL155;_tmp1EC=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1EB)->f1;_LL154: return({struct _tuple5
_tmp1ED;_tmp1ED.f1=_tmp1E2;_tmp1ED.f2=((void*(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_lookup)(_tmp1EC,_tmpE8);_tmp1ED;});_LL155:;_LL156:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1EE=_cycalloc(sizeof(*
_tmp1EE));_tmp1EE[0]=({struct Cyc_Core_Impossible_struct _tmp1EF;_tmp1EF.tag=Cyc_Core_Impossible;
_tmp1EF.f1=({const char*_tmp1F0="anal_Rexp: AggrArrow";_tag_dynforward(_tmp1F0,
sizeof(char),_get_zero_arr_size(_tmp1F0,21));});_tmp1EF;});_tmp1EE;}));_LL152:;}}}
_LLD1: if(*((int*)_tmpB9)!= 6)goto _LLD3;_tmpE9=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpB9)->f1;_tmpEA=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB9)->f2;_tmpEB=((
struct Cyc_Absyn_Conditional_e_struct*)_tmpB9)->f3;_LLD2: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp1F2;union Cyc_CfFlowInfo_FlowInfo_union _tmp1F3;struct _tuple7 _tmp1F1=Cyc_NewControlFlow_anal_test(
env,inflow,_tmpE9);_tmp1F2=_tmp1F1.f1;_tmp1F3=_tmp1F1.f2;_tmp1F2=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpE9->loc,_tmp1F2);_tmp1F3=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpE9->loc,
_tmp1F3);{struct _tuple5 _tmp1F4=Cyc_NewControlFlow_anal_Rexp(env,_tmp1F2,_tmpEA);
struct _tuple5 _tmp1F5=Cyc_NewControlFlow_anal_Rexp(env,_tmp1F3,_tmpEB);return Cyc_CfFlowInfo_join_flow_and_rval(
env->all_changed,_tmp1F4,_tmp1F5,1);}}_LLD3: if(*((int*)_tmpB9)!= 7)goto _LLD5;
_tmpEC=((struct Cyc_Absyn_And_e_struct*)_tmpB9)->f1;_tmpED=((struct Cyc_Absyn_And_e_struct*)
_tmpB9)->f2;_LLD4: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1F7;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1F8;struct _tuple7 _tmp1F6=Cyc_NewControlFlow_anal_test(env,inflow,_tmpEC);
_tmp1F7=_tmp1F6.f1;_tmp1F8=_tmp1F6.f2;_tmp1F7=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpEC->loc,_tmp1F7);_tmp1F8=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpEC->loc,
_tmp1F8);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1FA;void*_tmp1FB;struct _tuple5
_tmp1F9=Cyc_NewControlFlow_anal_Rexp(env,_tmp1F7,_tmpED);_tmp1FA=_tmp1F9.f1;
_tmp1FB=_tmp1F9.f2;_tmp1FA=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpED->loc,
_tmp1FA);{struct _tuple5 _tmp1FC=({struct _tuple5 _tmp1FF;_tmp1FF.f1=_tmp1FA;_tmp1FF.f2=
_tmp1FB;_tmp1FF;});struct _tuple5 _tmp1FD=({struct _tuple5 _tmp1FE;_tmp1FE.f1=
_tmp1F8;_tmp1FE.f2=(void*)((void*)0);_tmp1FE;});return Cyc_CfFlowInfo_join_flow_and_rval(
env->all_changed,_tmp1FC,_tmp1FD,0);}}}_LLD5: if(*((int*)_tmpB9)!= 8)goto _LLD7;
_tmpEE=((struct Cyc_Absyn_Or_e_struct*)_tmpB9)->f1;_tmpEF=((struct Cyc_Absyn_Or_e_struct*)
_tmpB9)->f2;_LLD6: {union Cyc_CfFlowInfo_FlowInfo_union _tmp201;union Cyc_CfFlowInfo_FlowInfo_union
_tmp202;struct _tuple7 _tmp200=Cyc_NewControlFlow_anal_test(env,inflow,_tmpEE);
_tmp201=_tmp200.f1;_tmp202=_tmp200.f2;_tmp201=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpEE->loc,_tmp201);_tmp202=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpEE->loc,
_tmp202);{union Cyc_CfFlowInfo_FlowInfo_union _tmp204;void*_tmp205;struct _tuple5
_tmp203=Cyc_NewControlFlow_anal_Rexp(env,_tmp202,_tmpEF);_tmp204=_tmp203.f1;
_tmp205=_tmp203.f2;_tmp204=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpEF->loc,
_tmp204);{struct _tuple5 _tmp206=({struct _tuple5 _tmp209;_tmp209.f1=_tmp204;_tmp209.f2=
_tmp205;_tmp209;});struct _tuple5 _tmp207=({struct _tuple5 _tmp208;_tmp208.f1=
_tmp201;_tmp208.f2=(void*)((void*)1);_tmp208;});return Cyc_CfFlowInfo_join_flow_and_rval(
env->all_changed,_tmp206,_tmp207,0);}}}_LLD7: if(*((int*)_tmpB9)!= 25)goto _LLD9;
_tmpF0=((struct Cyc_Absyn_Subscript_e_struct*)_tmpB9)->f1;_tmpF1=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpB9)->f2;_LLD8: {struct _RegionHandle _tmp20A=_new_region("temp");struct
_RegionHandle*temp=& _tmp20A;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp20D;struct Cyc_List_List*_tmp20E;struct _tuple9 _tmp20C=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp20B[2];_tmp20B[1]=_tmpF1;_tmp20B[0]=
_tmpF0;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(
temp,_tag_dynforward(_tmp20B,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp20D=
_tmp20C.f1;_tmp20E=_tmp20C.f2;_tmp20D=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpF1->loc,_tmp20D);{union Cyc_CfFlowInfo_FlowInfo_union _tmp20F=_tmp20D;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp210=_tmp20D;struct Cyc_Dict_Dict*_tmp211;struct
Cyc_List_List*_tmp212;struct Cyc_CfFlowInfo_ConsumeInfo _tmp213;_LL158: if((_tmp210.ReachableFL).tag
!= 1)goto _LL15A;_tmp211=(_tmp210.ReachableFL).f1;_tmp212=(_tmp210.ReachableFL).f2;
_tmp213=(_tmp210.ReachableFL).f3;_LL159: if(Cyc_CfFlowInfo_initlevel(_tmp211,(
void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(
_tmp20E))->tl))->hd)== (void*)0)({void*_tmp214[0]={};Cyc_Tcutil_terr(_tmpF1->loc,({
const char*_tmp215="expression may not be initialized";_tag_dynforward(_tmp215,
sizeof(char),_get_zero_arr_size(_tmp215,34));}),_tag_dynforward(_tmp214,sizeof(
void*),0));});{struct Cyc_List_List*_tmp216=Cyc_NewControlFlow_add_subscript_reln(
_tmp212,_tmpF0,_tmpF1);if(_tmp212 != _tmp216)_tmp20F=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp217;(_tmp217.ReachableFL).tag=1;(_tmp217.ReachableFL).f1=
_tmp211;(_tmp217.ReachableFL).f2=_tmp216;(_tmp217.ReachableFL).f3=_tmp213;
_tmp217;});goto _LL157;}_LL15A:;_LL15B: goto _LL157;_LL157:;}{void*_tmp218=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF0->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp219;struct Cyc_Absyn_PtrAtts _tmp21A;struct Cyc_Absyn_Conref*_tmp21B;_LL15D: if(
_tmp218 <= (void*)4)goto _LL161;if(*((int*)_tmp218)!= 9)goto _LL15F;_LL15E: {void*
_tmp21C=(void*)((struct Cyc_List_List*)_check_null(_tmp20E))->hd;struct Cyc_Dict_Dict*
_tmp21D;_LL164: if(_tmp21C <= (void*)3)goto _LL166;if(*((int*)_tmp21C)!= 4)goto
_LL166;_tmp21D=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp21C)->f1;_LL165: {
struct _tuple5 _tmp21F=({struct _tuple5 _tmp21E;_tmp21E.f1=_tmp20F;_tmp21E.f2=((void*(*)(
struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp21D,Cyc_Absyn_fieldname((
int)(Cyc_Evexp_eval_const_uint_exp(_tmpF1)).f1));_tmp21E;});_npop_handler(0);
return _tmp21F;}_LL166:;_LL167:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp220=_cycalloc(sizeof(*_tmp220));_tmp220[0]=({struct Cyc_Core_Impossible_struct
_tmp221;_tmp221.tag=Cyc_Core_Impossible;_tmp221.f1=({const char*_tmp222="anal_Rexp: Subscript";
_tag_dynforward(_tmp222,sizeof(char),_get_zero_arr_size(_tmp222,21));});_tmp221;});
_tmp220;}));_LL163:;}_LL15F: if(*((int*)_tmp218)!= 4)goto _LL161;_tmp219=((struct
Cyc_Absyn_PointerType_struct*)_tmp218)->f1;_tmp21A=_tmp219.ptr_atts;_tmp21B=
_tmp21A.bounds;_LL160:{void*_tmp223=(void*)(Cyc_Absyn_compress_conref(_tmp21B))->v;
void*_tmp224;_LL169: if(_tmp223 <= (void*)1)goto _LL16B;if(*((int*)_tmp223)!= 0)
goto _LL16B;_tmp224=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp223)->f1;if(
_tmp224 <= (void*)2)goto _LL16B;if(*((int*)_tmp224)!= 1)goto _LL16B;_LL16A:{void*
_tmp225=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp20E))->tl))->hd;struct Cyc_List_List*_tmp226;_LL16E: if(_tmp225 <= (
void*)3)goto _LL170;if(*((int*)_tmp225)!= 3)goto _LL170;_tmp226=((struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp225)->f1;_LL16F:(void*)(_tmpF1->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*
_tmp227=_cycalloc(sizeof(*_tmp227));_tmp227[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp228;_tmp228.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp228.f1=_tmp226;_tmp228;});
_tmp227;})));goto _LL16D;_LL170:;_LL171: goto _LL16D;_LL16D:;}goto _LL168;_LL16B:;
_LL16C: goto _LL168;_LL168:;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp22A;void*
_tmp22B;struct _tuple5 _tmp229=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp20D,
_tmpF0,(void*)((struct Cyc_List_List*)_check_null(_tmp20E))->hd);_tmp22A=_tmp229.f1;
_tmp22B=_tmp229.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp22C=_tmp22A;_LL173: if((
_tmp22C.BottomFL).tag != 0)goto _LL175;_LL174: {struct _tuple5 _tmp22E=({struct
_tuple5 _tmp22D;_tmp22D.f1=_tmp22A;_tmp22D.f2=_tmp22B;_tmp22D;});_npop_handler(0);
return _tmp22E;}_LL175:;_LL176: {struct _tuple5 _tmp230=({struct _tuple5 _tmp22F;
_tmp22F.f1=_tmp20F;_tmp22F.f2=_tmp22B;_tmp22F;});_npop_handler(0);return _tmp230;}
_LL172:;}}_LL161:;_LL162:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp231=_cycalloc(sizeof(*_tmp231));_tmp231[0]=({struct Cyc_Core_Impossible_struct
_tmp232;_tmp232.tag=Cyc_Core_Impossible;_tmp232.f1=({const char*_tmp233="anal_Rexp: Subscript -- bad type";
_tag_dynforward(_tmp233,sizeof(char),_get_zero_arr_size(_tmp233,33));});_tmp232;});
_tmp231;}));_LL15C:;}}};_pop_region(temp);}_LLD9: if(*((int*)_tmpB9)!= 32)goto
_LLDB;_tmpF2=((struct Cyc_Absyn_Tunion_e_struct*)_tmpB9)->f1;_tmpF3=((struct Cyc_Absyn_Tunion_e_struct*)
_tmpB9)->f2;_LLDA: if(_tmpF3->is_flat){struct _RegionHandle _tmp234=_new_region("temp");
struct _RegionHandle*temp=& _tmp234;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp236;struct Cyc_List_List*_tmp237;struct _tuple9 _tmp235=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmpF2,0);_tmp236=_tmp235.f1;_tmp237=_tmp235.f2;_tmp236=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp236);for(0;(unsigned int)_tmpF2;(_tmpF2=_tmpF2->tl,_tmp237=_tmp237->tl)){
_tmp236=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)_tmpF2->hd)->loc,
_tmp236,(void*)((struct Cyc_List_List*)_check_null(_tmp237))->hd);}{struct _tuple5
_tmp239=({struct _tuple5 _tmp238;_tmp238.f1=_tmp236;_tmp238.f2=Cyc_CfFlowInfo_unknown_all;
_tmp238;});_npop_handler(0);return _tmp239;}};_pop_region(temp);}_tmpF4=_tmpF2;
goto _LLDC;_LLDB: if(*((int*)_tmpB9)!= 26)goto _LLDD;_tmpF4=((struct Cyc_Absyn_Tuple_e_struct*)
_tmpB9)->f1;_LLDC: {struct _RegionHandle _tmp23A=_new_region("temp");struct
_RegionHandle*temp=& _tmp23A;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp23C;struct Cyc_List_List*_tmp23D;struct _tuple9 _tmp23B=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmpF4,0);_tmp23C=_tmp23B.f1;_tmp23D=_tmp23B.f2;_tmp23C=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp23C);{struct Cyc_Dict_Dict*aggrdict=Cyc_CfFlowInfo_empty_aggrdict();{
int i=0;for(0;_tmp23D != 0;(_tmp23D=_tmp23D->tl,++ i)){aggrdict=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k,void*v))Cyc_Dict_insert)(aggrdict,
Cyc_Absyn_fieldname(i),(void*)_tmp23D->hd);}}{struct _tuple5 _tmp241=({struct
_tuple5 _tmp23E;_tmp23E.f1=_tmp23C;_tmp23E.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp23F=_cycalloc(sizeof(*_tmp23F));_tmp23F[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp240;_tmp240.tag=4;_tmp240.f1=aggrdict;_tmp240;});_tmp23F;});_tmp23E;});
_npop_handler(0);return _tmp241;}}};_pop_region(temp);}_LLDD: if(*((int*)_tmpB9)!= 
31)goto _LLDF;_tmpF5=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpB9)->f2;_LLDE:
_tmpF6=_tmpF5;goto _LLE0;_LLDF: if(*((int*)_tmpB9)!= 30)goto _LLE1;_tmpF6=((struct
Cyc_Absyn_Struct_e_struct*)_tmpB9)->f3;_LLE0: {struct _RegionHandle _tmp242=
_new_region("temp");struct _RegionHandle*temp=& _tmp242;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp244;struct Cyc_List_List*_tmp245;struct _tuple9
_tmp243=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple11*),struct Cyc_List_List*
x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple11*))Cyc_Core_snd,
_tmpF6),0);_tmp244=_tmp243.f1;_tmp245=_tmp243.f2;_tmp244=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp244);{struct Cyc_Dict_Dict*aggrdict=Cyc_CfFlowInfo_empty_aggrdict();{
int i=0;for(0;_tmp245 != 0;(((_tmp245=_tmp245->tl,_tmpF6=_tmpF6->tl)),++ i)){struct
Cyc_List_List*ds=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmpF6))->hd)).f1;
for(0;ds != 0;ds=ds->tl){void*_tmp246=(void*)ds->hd;struct _dynforward_ptr*_tmp247;
_LL178: if(*((int*)_tmp246)!= 0)goto _LL17A;_LL179:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp248=_cycalloc(sizeof(*_tmp248));_tmp248[0]=({struct Cyc_Core_Impossible_struct
_tmp249;_tmp249.tag=Cyc_Core_Impossible;_tmp249.f1=({const char*_tmp24A="anal_Rexp:Struct_e";
_tag_dynforward(_tmp24A,sizeof(char),_get_zero_arr_size(_tmp24A,19));});_tmp249;});
_tmp248;}));_LL17A: if(*((int*)_tmp246)!= 1)goto _LL177;_tmp247=((struct Cyc_Absyn_FieldName_struct*)
_tmp246)->f1;_LL17B: aggrdict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _dynforward_ptr*k,void*v))Cyc_Dict_insert)(aggrdict,_tmp247,(void*)_tmp245->hd);
_LL177:;}}}{struct _tuple5 _tmp24E=({struct _tuple5 _tmp24B;_tmp24B.f1=_tmp244;
_tmp24B.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp24C=_cycalloc(
sizeof(*_tmp24C));_tmp24C[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp24D;
_tmp24D.tag=4;_tmp24D.f1=aggrdict;_tmp24D;});_tmp24C;});_tmp24B;});_npop_handler(
0);return _tmp24E;}}};_pop_region(temp);}_LLE1: if(*((int*)_tmpB9)!= 28)goto _LLE3;
_tmpF7=((struct Cyc_Absyn_Array_e_struct*)_tmpB9)->f1;_LLE2: {struct _RegionHandle
_tmp24F=_new_region("temp");struct _RegionHandle*temp=& _tmp24F;_push_region(temp);{
struct Cyc_List_List*_tmp250=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_Absyn_Exp*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(
temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple11*))Cyc_Core_snd,_tmpF7);union Cyc_CfFlowInfo_FlowInfo_union
_tmp252;struct Cyc_List_List*_tmp253;struct _tuple9 _tmp251=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp250,0);_tmp252=_tmp251.f1;_tmp253=_tmp251.f2;_tmp252=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp252);for(0;_tmp253 != 0;(_tmp253=_tmp253->tl,_tmp250=_tmp250->tl)){
_tmp252=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp250))->hd)->loc,_tmp252,(void*)_tmp253->hd);}{struct _tuple5
_tmp255=({struct _tuple5 _tmp254;_tmp254.f1=_tmp252;_tmp254.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp254;});_npop_handler(0);return _tmp255;}};_pop_region(temp);}_LLE3: if(*((int*)
_tmpB9)!= 29)goto _LLE5;_tmpF8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpB9)->f1;
_tmpF9=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpB9)->f2;_tmpFA=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpB9)->f3;_tmpFB=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpB9)->f4;_LLE4: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp257;void*_tmp258;struct _tuple5 _tmp256=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpF9);_tmp257=_tmp256.f1;_tmp258=_tmp256.f2;_tmp257=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpF9->loc,_tmp257);{union Cyc_CfFlowInfo_FlowInfo_union _tmp259=_tmp257;struct
Cyc_Dict_Dict*_tmp25A;struct Cyc_List_List*_tmp25B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp25C;_LL17D: if((_tmp259.BottomFL).tag != 0)goto _LL17F;_LL17E: return({struct
_tuple5 _tmp25D;_tmp25D.f1=_tmp257;_tmp25D.f2=Cyc_CfFlowInfo_unknown_all;_tmp25D;});
_LL17F: if((_tmp259.ReachableFL).tag != 1)goto _LL17C;_tmp25A=(_tmp259.ReachableFL).f1;
_tmp25B=(_tmp259.ReachableFL).f2;_tmp25C=(_tmp259.ReachableFL).f3;_LL180: if(Cyc_CfFlowInfo_initlevel(
_tmp25A,_tmp258)== (void*)0)({void*_tmp25E[0]={};Cyc_Tcutil_terr(_tmpF9->loc,({
const char*_tmp25F="expression may not be initialized";_tag_dynforward(_tmp25F,
sizeof(char),_get_zero_arr_size(_tmp25F,34));}),_tag_dynforward(_tmp25E,sizeof(
void*),0));});{struct Cyc_List_List*new_relns=_tmp25B;comp_loop: {void*_tmp260=(
void*)_tmpF9->r;struct Cyc_Absyn_Exp*_tmp261;void*_tmp262;struct Cyc_Absyn_Vardecl*
_tmp263;void*_tmp264;struct Cyc_Absyn_Vardecl*_tmp265;void*_tmp266;struct Cyc_Absyn_Vardecl*
_tmp267;void*_tmp268;struct Cyc_Absyn_Vardecl*_tmp269;union Cyc_Absyn_Cnst_union
_tmp26A;int _tmp26B;void*_tmp26C;struct Cyc_List_List*_tmp26D;struct Cyc_List_List
_tmp26E;struct Cyc_Absyn_Exp*_tmp26F;_LL182: if(*((int*)_tmp260)!= 15)goto _LL184;
_tmp261=((struct Cyc_Absyn_Cast_e_struct*)_tmp260)->f2;_LL183: _tmpF9=_tmp261;goto
comp_loop;_LL184: if(*((int*)_tmp260)!= 1)goto _LL186;_tmp262=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp260)->f2;if(_tmp262 <= (void*)1)goto _LL186;if(*((int*)_tmp262)!= 0)goto _LL186;
_tmp263=((struct Cyc_Absyn_Global_b_struct*)_tmp262)->f1;if(!(!_tmp263->escapes))
goto _LL186;_LL185: _tmp265=_tmp263;goto _LL187;_LL186: if(*((int*)_tmp260)!= 1)goto
_LL188;_tmp264=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp260)->f2;if(_tmp264 <= (
void*)1)goto _LL188;if(*((int*)_tmp264)!= 3)goto _LL188;_tmp265=((struct Cyc_Absyn_Local_b_struct*)
_tmp264)->f1;if(!(!_tmp265->escapes))goto _LL188;_LL187: _tmp267=_tmp265;goto
_LL189;_LL188: if(*((int*)_tmp260)!= 1)goto _LL18A;_tmp266=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp260)->f2;if(_tmp266 <= (void*)1)goto _LL18A;if(*((int*)_tmp266)!= 4)goto _LL18A;
_tmp267=((struct Cyc_Absyn_Pat_b_struct*)_tmp266)->f1;if(!(!_tmp267->escapes))
goto _LL18A;_LL189: _tmp269=_tmp267;goto _LL18B;_LL18A: if(*((int*)_tmp260)!= 1)goto
_LL18C;_tmp268=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp260)->f2;if(_tmp268 <= (
void*)1)goto _LL18C;if(*((int*)_tmp268)!= 2)goto _LL18C;_tmp269=((struct Cyc_Absyn_Param_b_struct*)
_tmp268)->f1;if(!(!_tmp269->escapes))goto _LL18C;_LL18B: new_relns=({struct Cyc_List_List*
_tmp270=_cycalloc(sizeof(*_tmp270));_tmp270->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp271=_cycalloc(sizeof(*_tmp271));_tmp271->vd=_tmpF8;_tmp271->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp272;(_tmp272.LessVar).tag=1;(_tmp272.LessVar).f1=
_tmp269;_tmp272;});_tmp271;});_tmp270->tl=_tmp25B;_tmp270;});goto _LL181;_LL18C:
if(*((int*)_tmp260)!= 0)goto _LL18E;_tmp26A=((struct Cyc_Absyn_Const_e_struct*)
_tmp260)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp260)->f1).Int_c).tag != 2)
goto _LL18E;_tmp26B=(_tmp26A.Int_c).f2;_LL18D: new_relns=({struct Cyc_List_List*
_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274->vd=_tmpF8;_tmp274->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp275;(_tmp275.LessConst).tag=3;(_tmp275.LessConst).f1=(
unsigned int)_tmp26B;_tmp275;});_tmp274;});_tmp273->tl=_tmp25B;_tmp273;});goto
_LL181;_LL18E: if(*((int*)_tmp260)!= 3)goto _LL190;_tmp26C=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp260)->f1;if((int)_tmp26C != 19)goto _LL190;_tmp26D=((struct Cyc_Absyn_Primop_e_struct*)
_tmp260)->f2;if(_tmp26D == 0)goto _LL190;_tmp26E=*_tmp26D;_tmp26F=(struct Cyc_Absyn_Exp*)
_tmp26E.hd;_LL18F:{void*_tmp276=(void*)_tmp26F->r;void*_tmp277;struct Cyc_Absyn_Vardecl*
_tmp278;void*_tmp279;struct Cyc_Absyn_Vardecl*_tmp27A;void*_tmp27B;struct Cyc_Absyn_Vardecl*
_tmp27C;void*_tmp27D;struct Cyc_Absyn_Vardecl*_tmp27E;_LL193: if(*((int*)_tmp276)
!= 1)goto _LL195;_tmp277=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp276)->f2;if(
_tmp277 <= (void*)1)goto _LL195;if(*((int*)_tmp277)!= 0)goto _LL195;_tmp278=((
struct Cyc_Absyn_Global_b_struct*)_tmp277)->f1;if(!(!_tmp278->escapes))goto _LL195;
_LL194: _tmp27A=_tmp278;goto _LL196;_LL195: if(*((int*)_tmp276)!= 1)goto _LL197;
_tmp279=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp276)->f2;if(_tmp279 <= (void*)
1)goto _LL197;if(*((int*)_tmp279)!= 3)goto _LL197;_tmp27A=((struct Cyc_Absyn_Local_b_struct*)
_tmp279)->f1;if(!(!_tmp27A->escapes))goto _LL197;_LL196: _tmp27C=_tmp27A;goto
_LL198;_LL197: if(*((int*)_tmp276)!= 1)goto _LL199;_tmp27B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp276)->f2;if(_tmp27B <= (void*)1)goto _LL199;if(*((int*)_tmp27B)!= 4)goto _LL199;
_tmp27C=((struct Cyc_Absyn_Pat_b_struct*)_tmp27B)->f1;if(!(!_tmp27C->escapes))
goto _LL199;_LL198: _tmp27E=_tmp27C;goto _LL19A;_LL199: if(*((int*)_tmp276)!= 1)goto
_LL19B;_tmp27D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp276)->f2;if(_tmp27D <= (
void*)1)goto _LL19B;if(*((int*)_tmp27D)!= 2)goto _LL19B;_tmp27E=((struct Cyc_Absyn_Param_b_struct*)
_tmp27D)->f1;if(!(!_tmp27E->escapes))goto _LL19B;_LL19A: new_relns=({struct Cyc_List_List*
_tmp27F=_cycalloc(sizeof(*_tmp27F));_tmp27F->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp280=_cycalloc(sizeof(*_tmp280));_tmp280->vd=_tmpF8;_tmp280->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp281;(_tmp281.LessSize).tag=2;(_tmp281.LessSize).f1=
_tmp27E;_tmp281;});_tmp280;});_tmp27F->tl=_tmp25B;_tmp27F;});goto _LL192;_LL19B:;
_LL19C: goto _LL192;_LL192:;}goto _LL181;_LL190:;_LL191: goto _LL181;_LL181:;}if(
_tmp25B != new_relns)_tmp257=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp282;(_tmp282.ReachableFL).tag=1;(_tmp282.ReachableFL).f1=_tmp25A;(_tmp282.ReachableFL).f2=
new_relns;(_tmp282.ReachableFL).f3=_tmp25C;_tmp282;});{void*_tmp283=_tmp258;
_LL19E: if((int)_tmp283 != 0)goto _LL1A0;_LL19F: return({struct _tuple5 _tmp284;
_tmp284.f1=_tmp257;_tmp284.f2=Cyc_CfFlowInfo_unknown_all;_tmp284;});_LL1A0: if((
int)_tmp283 != 2)goto _LL1A2;_LL1A1: goto _LL1A3;_LL1A2: if((int)_tmp283 != 1)goto
_LL1A4;_LL1A3: goto _LL1A5;_LL1A4: if(_tmp283 <= (void*)3)goto _LL1A6;if(*((int*)
_tmp283)!= 2)goto _LL1A6;_LL1A5: {struct Cyc_List_List _tmp285=({struct Cyc_List_List
_tmp291;_tmp291.hd=_tmpF8;_tmp291.tl=0;_tmp291;});_tmp257=Cyc_NewControlFlow_add_vars(
_tmp257,(struct Cyc_List_List*)& _tmp285,Cyc_CfFlowInfo_unknown_all,e->loc);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp287;void*_tmp288;struct _tuple5 _tmp286=Cyc_NewControlFlow_anal_Rexp(
env,_tmp257,_tmpFA);_tmp287=_tmp286.f1;_tmp288=_tmp286.f2;_tmp287=Cyc_CfFlowInfo_consume_unique_rvals(
_tmpFA->loc,_tmp287);{union Cyc_CfFlowInfo_FlowInfo_union _tmp289=_tmp287;struct
Cyc_Dict_Dict*_tmp28A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp28B;_LL1A9: if((_tmp289.BottomFL).tag
!= 0)goto _LL1AB;_LL1AA: return({struct _tuple5 _tmp28C;_tmp28C.f1=_tmp287;_tmp28C.f2=
Cyc_CfFlowInfo_unknown_all;_tmp28C;});_LL1AB: if((_tmp289.ReachableFL).tag != 1)
goto _LL1A8;_tmp28A=(_tmp289.ReachableFL).f1;_tmp28B=(_tmp289.ReachableFL).f3;
_LL1AC: if(Cyc_CfFlowInfo_initlevel(_tmp28A,_tmp288)!= (void*)2){({void*_tmp28D[0]={};
Cyc_Tcutil_terr(_tmpF9->loc,({const char*_tmp28E="expression may not be initialized";
_tag_dynforward(_tmp28E,sizeof(char),_get_zero_arr_size(_tmp28E,34));}),
_tag_dynforward(_tmp28D,sizeof(void*),0));});return({struct _tuple5 _tmp28F;
_tmp28F.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp290;(_tmp290.BottomFL).tag=0;_tmp290;});_tmp28F.f2=Cyc_CfFlowInfo_unknown_all;
_tmp28F;});}_LL1A8:;}_tmp257=_tmp287;goto _LL1A7;}}_LL1A6:;_LL1A7: while(1){struct
Cyc_List_List _tmp292=({struct Cyc_List_List _tmp29E;_tmp29E.hd=_tmpF8;_tmp29E.tl=0;
_tmp29E;});_tmp257=Cyc_NewControlFlow_add_vars(_tmp257,(struct Cyc_List_List*)&
_tmp292,Cyc_CfFlowInfo_unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp294;void*_tmp295;struct _tuple5 _tmp293=Cyc_NewControlFlow_anal_Rexp(env,
_tmp257,_tmpFA);_tmp294=_tmp293.f1;_tmp295=_tmp293.f2;_tmp294=Cyc_CfFlowInfo_consume_unique_rvals(
_tmpFA->loc,_tmp294);{union Cyc_CfFlowInfo_FlowInfo_union _tmp296=_tmp294;struct
Cyc_Dict_Dict*_tmp297;struct Cyc_CfFlowInfo_ConsumeInfo _tmp298;_LL1AE: if((_tmp296.BottomFL).tag
!= 0)goto _LL1B0;_LL1AF: goto _LL1AD;_LL1B0: if((_tmp296.ReachableFL).tag != 1)goto
_LL1AD;_tmp297=(_tmp296.ReachableFL).f1;_tmp298=(_tmp296.ReachableFL).f3;_LL1B1:
if(Cyc_CfFlowInfo_initlevel(_tmp297,_tmp295)!= (void*)2){({void*_tmp299[0]={};
Cyc_Tcutil_terr(_tmpF9->loc,({const char*_tmp29A="expression may not be initialized";
_tag_dynforward(_tmp29A,sizeof(char),_get_zero_arr_size(_tmp29A,34));}),
_tag_dynforward(_tmp299,sizeof(void*),0));});return({struct _tuple5 _tmp29B;
_tmp29B.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp29C;(_tmp29C.BottomFL).tag=0;_tmp29C;});_tmp29B.f2=Cyc_CfFlowInfo_unknown_all;
_tmp29B;});}_LL1AD:;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp29D=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp257,_tmp294,1);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp29D,_tmp257))break;_tmp257=_tmp29D;}}}return({struct _tuple5 _tmp29F;_tmp29F.f1=
_tmp257;_tmp29F.f2=Cyc_CfFlowInfo_unknown_all;_tmp29F;});_LL19D:;}}_LL17C:;}}
_LLE5: if(*((int*)_tmpB9)!= 37)goto _LLE7;_tmpFC=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmpB9)->f1;_LLE6: while(1){inflow=(Cyc_NewControlFlow_pre_stmt_check(env,inflow,
_tmpFC))->flow;{void*_tmp2A0=(void*)_tmpFC->r;struct Cyc_Absyn_Stmt*_tmp2A1;
struct Cyc_Absyn_Stmt*_tmp2A2;struct Cyc_Absyn_Decl*_tmp2A3;struct Cyc_Absyn_Stmt*
_tmp2A4;struct Cyc_Absyn_Exp*_tmp2A5;_LL1B3: if(_tmp2A0 <= (void*)1)goto _LL1B9;if(*((
int*)_tmp2A0)!= 1)goto _LL1B5;_tmp2A1=((struct Cyc_Absyn_Seq_s_struct*)_tmp2A0)->f1;
_tmp2A2=((struct Cyc_Absyn_Seq_s_struct*)_tmp2A0)->f2;_LL1B4: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp2A1);_tmpFC=_tmp2A2;goto _LL1B2;_LL1B5: if(*((int*)_tmp2A0)!= 11)
goto _LL1B7;_tmp2A3=((struct Cyc_Absyn_Decl_s_struct*)_tmp2A0)->f1;_tmp2A4=((
struct Cyc_Absyn_Decl_s_struct*)_tmp2A0)->f2;_LL1B6: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp2A3);_tmpFC=_tmp2A4;goto _LL1B2;_LL1B7: if(*((int*)_tmp2A0)!= 0)
goto _LL1B9;_tmp2A5=((struct Cyc_Absyn_Exp_s_struct*)_tmp2A0)->f1;_LL1B8: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp2A5);_LL1B9:;_LL1BA:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2A6=_cycalloc(sizeof(*_tmp2A6));_tmp2A6[0]=({struct Cyc_Core_Impossible_struct
_tmp2A7;_tmp2A7.tag=Cyc_Core_Impossible;_tmp2A7.f1=({const char*_tmp2A8="analyze_Rexp: ill-formed StmtExp";
_tag_dynforward(_tmp2A8,sizeof(char),_get_zero_arr_size(_tmp2A8,33));});_tmp2A7;});
_tmp2A6;}));_LL1B2:;}}_LLE7: if(*((int*)_tmpB9)!= 1)goto _LLE9;_tmpFD=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpB9)->f2;if((int)_tmpFD != 0)goto _LLE9;_LLE8:
goto _LLEA;_LLE9: if(*((int*)_tmpB9)!= 2)goto _LLEB;_LLEA: goto _LLEC;_LLEB: if(*((int*)
_tmpB9)!= 10)goto _LLED;_LLEC: goto _LLEE;_LLED: if(*((int*)_tmpB9)!= 36)goto _LLEF;
_LLEE: goto _LLF0;_LLEF: if(*((int*)_tmpB9)!= 27)goto _LL8E;_LLF0:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp2A9=_cycalloc(sizeof(*_tmp2A9));_tmp2A9[0]=({
struct Cyc_Core_Impossible_struct _tmp2AA;_tmp2AA.tag=Cyc_Core_Impossible;_tmp2AA.f1=({
const char*_tmp2AB="anal_Rexp, unexpected exp form";_tag_dynforward(_tmp2AB,
sizeof(char),_get_zero_arr_size(_tmp2AB,31));});_tmp2AA;});_tmp2A9;}));_LL8E:;}}
static struct _tuple5 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union f,
struct Cyc_Absyn_Exp*e,void*r,struct Cyc_List_List*flds){void*_tmp2AC=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp2AD;void*_tmp2AE;struct Cyc_Absyn_PtrAtts _tmp2AF;struct Cyc_Absyn_Conref*
_tmp2B0;struct Cyc_Absyn_Conref*_tmp2B1;_LL1BC: if(_tmp2AC <= (void*)4)goto _LL1BE;
if(*((int*)_tmp2AC)!= 4)goto _LL1BE;_tmp2AD=((struct Cyc_Absyn_PointerType_struct*)
_tmp2AC)->f1;_tmp2AE=(void*)_tmp2AD.elt_typ;_tmp2AF=_tmp2AD.ptr_atts;_tmp2B0=
_tmp2AF.bounds;_tmp2B1=_tmp2AF.zero_term;_LL1BD: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp2B2=f;struct Cyc_Dict_Dict*_tmp2B3;struct Cyc_List_List*_tmp2B4;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2B5;_LL1C1: if((_tmp2B2.BottomFL).tag != 0)goto _LL1C3;_LL1C2: return({struct
_tuple5 _tmp2B6;_tmp2B6.f1=f;_tmp2B6.f2=(void*)0;_tmp2B6;});_LL1C3: if((_tmp2B2.ReachableFL).tag
!= 1)goto _LL1C0;_tmp2B3=(_tmp2B2.ReachableFL).f1;_tmp2B4=(_tmp2B2.ReachableFL).f2;
_tmp2B5=(_tmp2B2.ReachableFL).f3;_LL1C4: if(Cyc_Tcutil_is_bound_one(_tmp2B0)){
void*_tmp2B7=r;struct Cyc_CfFlowInfo_Place*_tmp2B8;struct Cyc_CfFlowInfo_Place
_tmp2B9;void*_tmp2BA;struct Cyc_List_List*_tmp2BB;void*_tmp2BC;_LL1C6: if((int)
_tmp2B7 != 1)goto _LL1C8;_LL1C7: goto _LL1C9;_LL1C8: if((int)_tmp2B7 != 2)goto _LL1CA;
_LL1C9:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp2BD=_cycalloc(sizeof(*_tmp2BD));_tmp2BD[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp2BE;_tmp2BE.tag=Cyc_CfFlowInfo_NotZero;_tmp2BE.f1=_tmp2B4;_tmp2BE;});_tmp2BD;})));
goto _LL1C5;_LL1CA: if(_tmp2B7 <= (void*)3)goto _LL1CC;if(*((int*)_tmp2B7)!= 2)goto
_LL1CC;_tmp2B8=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2B7)->f1;_tmp2B9=*
_tmp2B8;_tmp2BA=(void*)_tmp2B9.root;_tmp2BB=_tmp2B9.fields;_LL1CB:(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp2BF=_cycalloc(sizeof(*
_tmp2BF));_tmp2BF[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp2C0;_tmp2C0.tag=
Cyc_CfFlowInfo_NotZero;_tmp2C0.f1=_tmp2B4;_tmp2C0;});_tmp2BF;})));return({struct
_tuple5 _tmp2C1;_tmp2C1.f1=f;_tmp2C1.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*
_tmp2C2=_cycalloc(sizeof(*_tmp2C2));_tmp2C2[0]=({struct Cyc_CfFlowInfo_PlaceL_struct
_tmp2C3;_tmp2C3.tag=0;_tmp2C3.f1=({struct Cyc_CfFlowInfo_Place*_tmp2C4=_cycalloc(
sizeof(*_tmp2C4));_tmp2C4->root=(void*)_tmp2BA;_tmp2C4->fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2BB,flds);
_tmp2C4;});_tmp2C3;});_tmp2C2;});_tmp2C1;});_LL1CC: if((int)_tmp2B7 != 0)goto
_LL1CE;_LL1CD:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));return({
struct _tuple5 _tmp2C5;_tmp2C5.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp2C6;(_tmp2C6.BottomFL).tag=0;_tmp2C6;});_tmp2C5.f2=(void*)0;_tmp2C5;});
_LL1CE: if(_tmp2B7 <= (void*)3)goto _LL1D0;if(*((int*)_tmp2B7)!= 0)goto _LL1D0;
_tmp2BC=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp2B7)->f1;_LL1CF: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmp2BC);goto _LL1D1;_LL1D0:;_LL1D1:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2C7=_cycalloc(sizeof(*_tmp2C7));_tmp2C7[
0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp2C8;_tmp2C8.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp2C8.f1=_tmp2B4;_tmp2C8;});_tmp2C7;})));_LL1C5:;}else{(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));
_tmp2C9[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp2CA;_tmp2CA.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp2CA.f1=_tmp2B4;_tmp2CA;});_tmp2C9;})));}if(Cyc_CfFlowInfo_initlevel(_tmp2B3,
r)== (void*)0)({void*_tmp2CB[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp2CC="dereference of possibly uninitialized pointer";
_tag_dynforward(_tmp2CC,sizeof(char),_get_zero_arr_size(_tmp2CC,46));}),
_tag_dynforward(_tmp2CB,sizeof(void*),0));});return({struct _tuple5 _tmp2CD;
_tmp2CD.f1=f;_tmp2CD.f2=(void*)0;_tmp2CD;});_LL1C0:;}_LL1BE:;_LL1BF:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));
_tmp2CE[0]=({struct Cyc_Core_Impossible_struct _tmp2CF;_tmp2CF.tag=Cyc_Core_Impossible;
_tmp2CF.f1=({const char*_tmp2D0="left deref of non-pointer-type";_tag_dynforward(
_tmp2D0,sizeof(char),_get_zero_arr_size(_tmp2D0,31));});_tmp2CF;});_tmp2CE;}));
_LL1BB:;}static struct _tuple5 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict*d;{union Cyc_CfFlowInfo_FlowInfo_union _tmp2D1=inflow;
struct Cyc_Dict_Dict*_tmp2D2;struct Cyc_List_List*_tmp2D3;_LL1D3: if((_tmp2D1.BottomFL).tag
!= 0)goto _LL1D5;_LL1D4: return({struct _tuple5 _tmp2D4;_tmp2D4.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp2D5;(_tmp2D5.BottomFL).tag=0;_tmp2D5;});
_tmp2D4.f2=(void*)0;_tmp2D4;});_LL1D5: if((_tmp2D1.ReachableFL).tag != 1)goto
_LL1D2;_tmp2D2=(_tmp2D1.ReachableFL).f1;_tmp2D3=(_tmp2D1.ReachableFL).f2;_LL1D6:
d=_tmp2D2;_LL1D2:;}{void*_tmp2D6=(void*)e->r;struct Cyc_Absyn_Exp*_tmp2D7;struct
Cyc_Absyn_Exp*_tmp2D8;struct Cyc_Absyn_Exp*_tmp2D9;void*_tmp2DA;struct Cyc_Absyn_Vardecl*
_tmp2DB;void*_tmp2DC;struct Cyc_Absyn_Vardecl*_tmp2DD;void*_tmp2DE;struct Cyc_Absyn_Vardecl*
_tmp2DF;void*_tmp2E0;struct Cyc_Absyn_Vardecl*_tmp2E1;struct Cyc_Absyn_Exp*_tmp2E2;
struct _dynforward_ptr*_tmp2E3;struct Cyc_Absyn_Exp*_tmp2E4;struct Cyc_Absyn_Exp*
_tmp2E5;struct Cyc_Absyn_Exp*_tmp2E6;struct Cyc_Absyn_Exp*_tmp2E7;struct
_dynforward_ptr*_tmp2E8;_LL1D8: if(*((int*)_tmp2D6)!= 15)goto _LL1DA;_tmp2D7=((
struct Cyc_Absyn_Cast_e_struct*)_tmp2D6)->f2;_LL1D9: _tmp2D8=_tmp2D7;goto _LL1DB;
_LL1DA: if(*((int*)_tmp2D6)!= 13)goto _LL1DC;_tmp2D8=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp2D6)->f1;_LL1DB: _tmp2D9=_tmp2D8;goto _LL1DD;_LL1DC: if(*((int*)_tmp2D6)!= 14)
goto _LL1DE;_tmp2D9=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp2D6)->f1;_LL1DD:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp2D9,flds);_LL1DE: if(*((int*)
_tmp2D6)!= 1)goto _LL1E0;_tmp2DA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2D6)->f2;
if(_tmp2DA <= (void*)1)goto _LL1E0;if(*((int*)_tmp2DA)!= 2)goto _LL1E0;_tmp2DB=((
struct Cyc_Absyn_Param_b_struct*)_tmp2DA)->f1;_LL1DF: _tmp2DD=_tmp2DB;goto _LL1E1;
_LL1E0: if(*((int*)_tmp2D6)!= 1)goto _LL1E2;_tmp2DC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D6)->f2;if(_tmp2DC <= (void*)1)goto _LL1E2;if(*((int*)_tmp2DC)!= 3)goto _LL1E2;
_tmp2DD=((struct Cyc_Absyn_Local_b_struct*)_tmp2DC)->f1;_LL1E1: _tmp2DF=_tmp2DD;
goto _LL1E3;_LL1E2: if(*((int*)_tmp2D6)!= 1)goto _LL1E4;_tmp2DE=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D6)->f2;if(_tmp2DE <= (void*)1)goto _LL1E4;if(*((int*)_tmp2DE)!= 4)goto _LL1E4;
_tmp2DF=((struct Cyc_Absyn_Pat_b_struct*)_tmp2DE)->f1;_LL1E3: return({struct
_tuple5 _tmp2E9;_tmp2E9.f1=inflow;_tmp2E9.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*
_tmp2EA=_cycalloc(sizeof(*_tmp2EA));_tmp2EA[0]=({struct Cyc_CfFlowInfo_PlaceL_struct
_tmp2EB;_tmp2EB.tag=0;_tmp2EB.f1=({struct Cyc_CfFlowInfo_Place*_tmp2EC=_cycalloc(
sizeof(*_tmp2EC));_tmp2EC->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp2ED=_cycalloc(sizeof(*_tmp2ED));_tmp2ED[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp2EE;_tmp2EE.tag=0;_tmp2EE.f1=_tmp2DF;_tmp2EE;});_tmp2ED;}));_tmp2EC->fields=
flds;_tmp2EC;});_tmp2EB;});_tmp2EA;});_tmp2E9;});_LL1E4: if(*((int*)_tmp2D6)!= 1)
goto _LL1E6;_tmp2E0=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2D6)->f2;if(
_tmp2E0 <= (void*)1)goto _LL1E6;if(*((int*)_tmp2E0)!= 0)goto _LL1E6;_tmp2E1=((
struct Cyc_Absyn_Global_b_struct*)_tmp2E0)->f1;_LL1E5: return({struct _tuple5
_tmp2EF;_tmp2EF.f1=inflow;_tmp2EF.f2=(void*)0;_tmp2EF;});_LL1E6: if(*((int*)
_tmp2D6)!= 24)goto _LL1E8;_tmp2E2=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2D6)->f1;
_tmp2E3=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2D6)->f2;_LL1E7:{void*_tmp2F0=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2E2->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp2F1;void*_tmp2F2;_LL1F1: if(_tmp2F0 <= (void*)4)goto
_LL1F3;if(*((int*)_tmp2F0)!= 4)goto _LL1F3;_tmp2F1=((struct Cyc_Absyn_PointerType_struct*)
_tmp2F0)->f1;_tmp2F2=(void*)_tmp2F1.elt_typ;_LL1F2: if(!Cyc_Absyn_is_union_type(
_tmp2F2))flds=({struct Cyc_List_List*_tmp2F3=_cycalloc(sizeof(*_tmp2F3));_tmp2F3->hd=
_tmp2E3;_tmp2F3->tl=flds;_tmp2F3;});goto _LL1F0;_LL1F3:;_LL1F4:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp2F4=_cycalloc(sizeof(*_tmp2F4));_tmp2F4[0]=({
struct Cyc_Core_Impossible_struct _tmp2F5;_tmp2F5.tag=Cyc_Core_Impossible;_tmp2F5.f1=({
const char*_tmp2F6="anal_Rexp: AggrArrow ptr";_tag_dynforward(_tmp2F6,sizeof(char),
_get_zero_arr_size(_tmp2F6,25));});_tmp2F5;});_tmp2F4;}));_LL1F0:;}_tmp2E4=
_tmp2E2;goto _LL1E9;_LL1E8: if(*((int*)_tmp2D6)!= 22)goto _LL1EA;_tmp2E4=((struct
Cyc_Absyn_Deref_e_struct*)_tmp2D6)->f1;_LL1E9: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp2F8;void*_tmp2F9;struct _tuple5 _tmp2F7=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp2E4);_tmp2F8=_tmp2F7.f1;_tmp2F9=_tmp2F7.f2;_tmp2F8=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp2F8);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp2F8,_tmp2E4,
_tmp2F9,flds);}_LL1EA: if(*((int*)_tmp2D6)!= 25)goto _LL1EC;_tmp2E5=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp2D6)->f1;_tmp2E6=((struct Cyc_Absyn_Subscript_e_struct*)_tmp2D6)->f2;_LL1EB: {
void*_tmp2FA=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2E5->topt))->v);struct Cyc_Absyn_PtrInfo _tmp2FB;struct Cyc_Absyn_PtrAtts
_tmp2FC;struct Cyc_Absyn_Conref*_tmp2FD;_LL1F6: if(_tmp2FA <= (void*)4)goto _LL1FA;
if(*((int*)_tmp2FA)!= 9)goto _LL1F8;_LL1F7: {struct _dynforward_ptr*_tmp2FE=Cyc_Absyn_fieldname((
int)(Cyc_Evexp_eval_const_uint_exp(_tmp2E6)).f1);return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp2E5,({struct Cyc_List_List*_tmp2FF=_cycalloc(sizeof(*_tmp2FF));
_tmp2FF->hd=_tmp2FE;_tmp2FF->tl=flds;_tmp2FF;}));}_LL1F8: if(*((int*)_tmp2FA)!= 4)
goto _LL1FA;_tmp2FB=((struct Cyc_Absyn_PointerType_struct*)_tmp2FA)->f1;_tmp2FC=
_tmp2FB.ptr_atts;_tmp2FD=_tmp2FC.bounds;_LL1F9: {struct _RegionHandle _tmp300=
_new_region("temp");struct _RegionHandle*temp=& _tmp300;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp303;struct Cyc_List_List*_tmp304;struct _tuple9
_tmp302=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*
_tmp301[2];_tmp301[1]=_tmp2E6;_tmp301[0]=_tmp2E5;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp301,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp303=_tmp302.f1;_tmp304=
_tmp302.f2;_tmp303=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp2E6->loc,_tmp303);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp305=_tmp303;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp306=_tmp303;struct Cyc_Dict_Dict*_tmp307;struct Cyc_List_List*_tmp308;struct
Cyc_CfFlowInfo_ConsumeInfo _tmp309;_LL1FD: if((_tmp306.ReachableFL).tag != 1)goto
_LL1FF;_tmp307=(_tmp306.ReachableFL).f1;_tmp308=(_tmp306.ReachableFL).f2;_tmp309=(
_tmp306.ReachableFL).f3;_LL1FE: if(Cyc_CfFlowInfo_initlevel(_tmp307,(void*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp304))->tl))->hd)
== (void*)0)({void*_tmp30A[0]={};Cyc_Tcutil_terr(_tmp2E6->loc,({const char*
_tmp30B="expression may not be initialized";_tag_dynforward(_tmp30B,sizeof(char),
_get_zero_arr_size(_tmp30B,34));}),_tag_dynforward(_tmp30A,sizeof(void*),0));});{
struct Cyc_List_List*_tmp30C=Cyc_NewControlFlow_add_subscript_reln(_tmp308,
_tmp2E5,_tmp2E6);if(_tmp308 != _tmp30C)_tmp305=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp30D;(_tmp30D.ReachableFL).tag=1;(_tmp30D.ReachableFL).f1=
_tmp307;(_tmp30D.ReachableFL).f2=_tmp30C;(_tmp30D.ReachableFL).f3=_tmp309;
_tmp30D;});goto _LL1FC;}_LL1FF:;_LL200: goto _LL1FC;_LL1FC:;}{void*_tmp30E=(void*)(
Cyc_Absyn_compress_conref(_tmp2FD))->v;void*_tmp30F;_LL202: if(_tmp30E <= (void*)1)
goto _LL204;if(*((int*)_tmp30E)!= 0)goto _LL204;_tmp30F=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp30E)->f1;if(_tmp30F <= (void*)2)goto _LL204;if(*((int*)_tmp30F)!= 1)goto _LL204;
_LL203:{void*_tmp310=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp304))->tl))->hd;struct Cyc_List_List*_tmp311;_LL207: if(_tmp310 <= (
void*)3)goto _LL209;if(*((int*)_tmp310)!= 3)goto _LL209;_tmp311=((struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp310)->f1;_LL208:(void*)(_tmp2E6->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*
_tmp312=_cycalloc(sizeof(*_tmp312));_tmp312[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp313;_tmp313.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp313.f1=_tmp311;_tmp313;});
_tmp312;})));goto _LL206;_LL209:;_LL20A: goto _LL206;_LL206:;}goto _LL201;_LL204:;
_LL205: goto _LL201;_LL201:;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp315;void*
_tmp316;struct _tuple5 _tmp314=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp303,
_tmp2E5,(void*)((struct Cyc_List_List*)_check_null(_tmp304))->hd,flds);_tmp315=
_tmp314.f1;_tmp316=_tmp314.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp317=_tmp315;
_LL20C: if((_tmp317.BottomFL).tag != 0)goto _LL20E;_LL20D: {struct _tuple5 _tmp319=({
struct _tuple5 _tmp318;_tmp318.f1=_tmp315;_tmp318.f2=_tmp316;_tmp318;});
_npop_handler(0);return _tmp319;}_LL20E:;_LL20F: {struct _tuple5 _tmp31B=({struct
_tuple5 _tmp31A;_tmp31A.f1=_tmp305;_tmp31A.f2=_tmp316;_tmp31A;});_npop_handler(0);
return _tmp31B;}_LL20B:;}}}};_pop_region(temp);}_LL1FA:;_LL1FB:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C[0]=({
struct Cyc_Core_Impossible_struct _tmp31D;_tmp31D.tag=Cyc_Core_Impossible;_tmp31D.f1=({
const char*_tmp31E="anal_Lexp: Subscript -- bad type";_tag_dynforward(_tmp31E,
sizeof(char),_get_zero_arr_size(_tmp31E,33));});_tmp31D;});_tmp31C;}));_LL1F5:;}
_LL1EC: if(*((int*)_tmp2D6)!= 23)goto _LL1EE;_tmp2E7=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp2D6)->f1;_tmp2E8=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp2D6)->f2;_LL1ED:
if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2E7->topt))->v))
return({struct _tuple5 _tmp31F;_tmp31F.f1=inflow;_tmp31F.f2=(void*)0;_tmp31F;});
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp2E7,({struct Cyc_List_List*
_tmp320=_cycalloc(sizeof(*_tmp320));_tmp320->hd=_tmp2E8;_tmp320->tl=flds;_tmp320;}));
_LL1EE:;_LL1EF: return({struct _tuple5 _tmp321;_tmp321.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp322;(_tmp322.BottomFL).tag=0;_tmp322;});
_tmp321.f2=(void*)0;_tmp321;});_LL1D7:;}}static struct _tuple5 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){union Cyc_CfFlowInfo_FlowInfo_union _tmp324;void*_tmp325;
struct _tuple5 _tmp323=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,e,0);_tmp324=
_tmp323.f1;_tmp325=_tmp323.f2;return({struct _tuple5 _tmp326;_tmp326.f1=_tmp324;
_tmp326.f2=_tmp325;_tmp326;});}static struct _tuple7 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){{void*_tmp327=(void*)e->r;struct Cyc_Absyn_Exp*_tmp328;
struct Cyc_Absyn_Exp*_tmp329;struct Cyc_Absyn_Exp*_tmp32A;struct Cyc_Absyn_Exp*
_tmp32B;struct Cyc_Absyn_Exp*_tmp32C;struct Cyc_Absyn_Exp*_tmp32D;struct Cyc_Absyn_Exp*
_tmp32E;struct Cyc_Absyn_Exp*_tmp32F;struct Cyc_Absyn_Exp*_tmp330;void*_tmp331;
struct Cyc_List_List*_tmp332;struct Cyc_List_List _tmp333;struct Cyc_Absyn_Exp*
_tmp334;struct Cyc_List_List*_tmp335;void*_tmp336;struct Cyc_List_List*_tmp337;
_LL211: if(*((int*)_tmp327)!= 6)goto _LL213;_tmp328=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp327)->f1;_tmp329=((struct Cyc_Absyn_Conditional_e_struct*)_tmp327)->f2;
_tmp32A=((struct Cyc_Absyn_Conditional_e_struct*)_tmp327)->f3;_LL212: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp339;union Cyc_CfFlowInfo_FlowInfo_union _tmp33A;struct _tuple7 _tmp338=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp328);_tmp339=_tmp338.f1;_tmp33A=_tmp338.f2;_tmp339=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp328->loc,_tmp339);_tmp33A=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp328->loc,
_tmp33A);{union Cyc_CfFlowInfo_FlowInfo_union _tmp33C;union Cyc_CfFlowInfo_FlowInfo_union
_tmp33D;struct _tuple7 _tmp33B=Cyc_NewControlFlow_anal_test(env,_tmp339,_tmp329);
_tmp33C=_tmp33B.f1;_tmp33D=_tmp33B.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp33F;
union Cyc_CfFlowInfo_FlowInfo_union _tmp340;struct _tuple7 _tmp33E=Cyc_NewControlFlow_anal_test(
env,_tmp33A,_tmp32A);_tmp33F=_tmp33E.f1;_tmp340=_tmp33E.f2;return({struct _tuple7
_tmp341;_tmp341.f1=Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp33C,_tmp33F,1);
_tmp341.f2=Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp33D,_tmp340,1);_tmp341;});}}}
_LL213: if(*((int*)_tmp327)!= 7)goto _LL215;_tmp32B=((struct Cyc_Absyn_And_e_struct*)
_tmp327)->f1;_tmp32C=((struct Cyc_Absyn_And_e_struct*)_tmp327)->f2;_LL214: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp343;union Cyc_CfFlowInfo_FlowInfo_union _tmp344;
struct _tuple7 _tmp342=Cyc_NewControlFlow_anal_test(env,inflow,_tmp32B);_tmp343=
_tmp342.f1;_tmp344=_tmp342.f2;_tmp343=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp32B->loc,_tmp343);_tmp344=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp32B->loc,
_tmp344);{union Cyc_CfFlowInfo_FlowInfo_union _tmp346;union Cyc_CfFlowInfo_FlowInfo_union
_tmp347;struct _tuple7 _tmp345=Cyc_NewControlFlow_anal_test(env,_tmp343,_tmp32C);
_tmp346=_tmp345.f1;_tmp347=_tmp345.f2;_tmp346=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp32C->loc,_tmp346);_tmp347=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp32C->loc,
_tmp347);return({struct _tuple7 _tmp348;_tmp348.f1=_tmp346;_tmp348.f2=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp344,_tmp347,0);_tmp348;});}}_LL215: if(*((int*)_tmp327)!= 8)
goto _LL217;_tmp32D=((struct Cyc_Absyn_Or_e_struct*)_tmp327)->f1;_tmp32E=((struct
Cyc_Absyn_Or_e_struct*)_tmp327)->f2;_LL216: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp34A;union Cyc_CfFlowInfo_FlowInfo_union _tmp34B;struct _tuple7 _tmp349=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp32D);_tmp34A=_tmp349.f1;_tmp34B=_tmp349.f2;_tmp34A=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp32D->loc,_tmp34A);_tmp34B=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp32D->loc,
_tmp34B);{union Cyc_CfFlowInfo_FlowInfo_union _tmp34D;union Cyc_CfFlowInfo_FlowInfo_union
_tmp34E;struct _tuple7 _tmp34C=Cyc_NewControlFlow_anal_test(env,_tmp34B,_tmp32E);
_tmp34D=_tmp34C.f1;_tmp34E=_tmp34C.f2;_tmp34D=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp32E->loc,_tmp34D);_tmp34E=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp32E->loc,
_tmp34E);return({struct _tuple7 _tmp34F;_tmp34F.f1=Cyc_CfFlowInfo_join_flow(env->all_changed,
_tmp34A,_tmp34D,0);_tmp34F.f2=_tmp34E;_tmp34F;});}}_LL217: if(*((int*)_tmp327)!= 
9)goto _LL219;_tmp32F=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp327)->f1;_tmp330=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp327)->f2;_LL218: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp351;void*_tmp352;struct _tuple5 _tmp350=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp32F);_tmp351=_tmp350.f1;_tmp352=_tmp350.f2;_tmp351=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp32F->loc,_tmp351);return Cyc_NewControlFlow_anal_test(env,_tmp351,_tmp330);}
_LL219: if(*((int*)_tmp327)!= 3)goto _LL21B;_tmp331=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp327)->f1;if((int)_tmp331 != 11)goto _LL21B;_tmp332=((struct Cyc_Absyn_Primop_e_struct*)
_tmp327)->f2;if(_tmp332 == 0)goto _LL21B;_tmp333=*_tmp332;_tmp334=(struct Cyc_Absyn_Exp*)
_tmp333.hd;_tmp335=_tmp333.tl;if(_tmp335 != 0)goto _LL21B;_LL21A: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp354;union Cyc_CfFlowInfo_FlowInfo_union _tmp355;struct _tuple7 _tmp353=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp334);_tmp354=_tmp353.f1;_tmp355=_tmp353.f2;return({struct _tuple7
_tmp356;_tmp356.f1=_tmp355;_tmp356.f2=_tmp354;_tmp356;});}_LL21B: if(*((int*)
_tmp327)!= 3)goto _LL21D;_tmp336=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp327)->f1;_tmp337=((struct Cyc_Absyn_Primop_e_struct*)_tmp327)->f2;_LL21C: {
void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo_union f;{struct _RegionHandle _tmp357=
_new_region("temp");struct _RegionHandle*temp=& _tmp357;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp359;struct Cyc_List_List*_tmp35A;struct _tuple9
_tmp358=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp337,0);
_tmp359=_tmp358.f1;_tmp35A=_tmp358.f2;r1=(void*)((struct Cyc_List_List*)
_check_null(_tmp35A))->hd;r2=(void*)((struct Cyc_List_List*)_check_null(_tmp35A->tl))->hd;
f=_tmp359;};_pop_region(temp);}{void*_tmp35B=_tmp336;_LL220: if((int)_tmp35B != 0)
goto _LL222;_LL221: goto _LL223;_LL222: if((int)_tmp35B != 2)goto _LL224;_LL223: Cyc_CfFlowInfo_check_unique_rvals(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp337))->hd)->loc,f);
goto _LL21F;_LL224:;_LL225: f=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp337))->hd)->loc,f);goto _LL21F;_LL21F:;}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp35C=f;struct Cyc_Dict_Dict*_tmp35D;struct
Cyc_List_List*_tmp35E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp35F;_LL227: if((_tmp35C.BottomFL).tag
!= 0)goto _LL229;_LL228: return({struct _tuple7 _tmp360;_tmp360.f1=f;_tmp360.f2=f;
_tmp360;});_LL229: if((_tmp35C.ReachableFL).tag != 1)goto _LL226;_tmp35D=(_tmp35C.ReachableFL).f1;
_tmp35E=(_tmp35C.ReachableFL).f2;_tmp35F=(_tmp35C.ReachableFL).f3;_LL22A: {
struct Cyc_Absyn_Exp*_tmp361=(struct Cyc_Absyn_Exp*)_tmp337->hd;struct Cyc_Absyn_Exp*
_tmp362=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp337->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(_tmp35D,r1)== (void*)0)({void*_tmp363[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp337->hd)->loc,({const char*_tmp364="expression may not be initialized";
_tag_dynforward(_tmp364,sizeof(char),_get_zero_arr_size(_tmp364,34));}),
_tag_dynforward(_tmp363,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(_tmp35D,
r2)== (void*)0)({void*_tmp365[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp337->tl))->hd)->loc,({const char*_tmp366="expression may not be initialized";
_tag_dynforward(_tmp366,sizeof(char),_get_zero_arr_size(_tmp366,34));}),
_tag_dynforward(_tmp365,sizeof(void*),0));});if(_tmp336 == (void*)5  || _tmp336 == (
void*)6){struct _tuple0 _tmp368=({struct _tuple0 _tmp367;_tmp367.f1=r1;_tmp367.f2=r2;
_tmp367;});void*_tmp369;void*_tmp36A;void*_tmp36B;void*_tmp36C;void*_tmp36D;void*
_tmp36E;void*_tmp36F;void*_tmp370;void*_tmp371;void*_tmp372;void*_tmp373;void*
_tmp374;void*_tmp375;void*_tmp376;void*_tmp377;void*_tmp378;void*_tmp379;void*
_tmp37A;void*_tmp37B;void*_tmp37C;_LL22C: _tmp369=_tmp368.f1;if(_tmp369 <= (void*)
3)goto _LL22E;if(*((int*)_tmp369)!= 0)goto _LL22E;_tmp36A=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp369)->f1;_tmp36B=_tmp368.f2;if((int)_tmp36B != 0)goto _LL22E;_LL22D: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp37E;union Cyc_CfFlowInfo_FlowInfo_union _tmp37F;struct _tuple7 _tmp37D=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp361,_tmp36A);_tmp37E=_tmp37D.f1;_tmp37F=_tmp37D.f2;{void*_tmp380=
_tmp336;_LL241: if((int)_tmp380 != 5)goto _LL243;_LL242: return({struct _tuple7
_tmp381;_tmp381.f1=_tmp37F;_tmp381.f2=_tmp37E;_tmp381;});_LL243: if((int)_tmp380
!= 6)goto _LL245;_LL244: return({struct _tuple7 _tmp382;_tmp382.f1=_tmp37E;_tmp382.f2=
_tmp37F;_tmp382;});_LL245:;_LL246:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp383=_cycalloc(sizeof(*_tmp383));_tmp383[0]=({struct Cyc_Core_Impossible_struct
_tmp384;_tmp384.tag=Cyc_Core_Impossible;_tmp384.f1=({const char*_tmp385="anal_test, zero-split";
_tag_dynforward(_tmp385,sizeof(char),_get_zero_arr_size(_tmp385,22));});_tmp384;});
_tmp383;}));_LL240:;}}_LL22E: _tmp36C=_tmp368.f1;if((int)_tmp36C != 0)goto _LL230;
_tmp36D=_tmp368.f2;if(_tmp36D <= (void*)3)goto _LL230;if(*((int*)_tmp36D)!= 0)goto
_LL230;_tmp36E=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp36D)->f1;
_LL22F: {union Cyc_CfFlowInfo_FlowInfo_union _tmp387;union Cyc_CfFlowInfo_FlowInfo_union
_tmp388;struct _tuple7 _tmp386=Cyc_NewControlFlow_splitzero(env,f,f,_tmp362,
_tmp36E);_tmp387=_tmp386.f1;_tmp388=_tmp386.f2;{void*_tmp389=_tmp336;_LL248: if((
int)_tmp389 != 5)goto _LL24A;_LL249: return({struct _tuple7 _tmp38A;_tmp38A.f1=
_tmp388;_tmp38A.f2=_tmp387;_tmp38A;});_LL24A: if((int)_tmp389 != 6)goto _LL24C;
_LL24B: return({struct _tuple7 _tmp38B;_tmp38B.f1=_tmp387;_tmp38B.f2=_tmp388;
_tmp38B;});_LL24C:;_LL24D:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp38C=_cycalloc(sizeof(*_tmp38C));_tmp38C[0]=({struct Cyc_Core_Impossible_struct
_tmp38D;_tmp38D.tag=Cyc_Core_Impossible;_tmp38D.f1=({const char*_tmp38E="anal_test, zero-split";
_tag_dynforward(_tmp38E,sizeof(char),_get_zero_arr_size(_tmp38E,22));});_tmp38D;});
_tmp38C;}));_LL247:;}}_LL230: _tmp36F=_tmp368.f1;if((int)_tmp36F != 0)goto _LL232;
_tmp370=_tmp368.f2;if((int)_tmp370 != 0)goto _LL232;_LL231: if(_tmp336 == (void*)5)
return({struct _tuple7 _tmp38F;_tmp38F.f1=f;_tmp38F.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp390;(_tmp390.BottomFL).tag=0;_tmp390;});
_tmp38F;});else{return({struct _tuple7 _tmp391;_tmp391.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp392;(_tmp392.BottomFL).tag=0;_tmp392;});
_tmp391.f2=f;_tmp391;});}_LL232: _tmp371=_tmp368.f1;if((int)_tmp371 != 0)goto
_LL234;_tmp372=_tmp368.f2;if((int)_tmp372 != 1)goto _LL234;_LL233: goto _LL235;
_LL234: _tmp373=_tmp368.f1;if((int)_tmp373 != 0)goto _LL236;_tmp374=_tmp368.f2;if((
int)_tmp374 != 2)goto _LL236;_LL235: goto _LL237;_LL236: _tmp375=_tmp368.f1;if((int)
_tmp375 != 0)goto _LL238;_tmp376=_tmp368.f2;if(_tmp376 <= (void*)3)goto _LL238;if(*((
int*)_tmp376)!= 2)goto _LL238;_LL237: goto _LL239;_LL238: _tmp377=_tmp368.f1;if((int)
_tmp377 != 1)goto _LL23A;_tmp378=_tmp368.f2;if((int)_tmp378 != 0)goto _LL23A;_LL239:
goto _LL23B;_LL23A: _tmp379=_tmp368.f1;if((int)_tmp379 != 2)goto _LL23C;_tmp37A=
_tmp368.f2;if((int)_tmp37A != 0)goto _LL23C;_LL23B: goto _LL23D;_LL23C: _tmp37B=
_tmp368.f1;if(_tmp37B <= (void*)3)goto _LL23E;if(*((int*)_tmp37B)!= 2)goto _LL23E;
_tmp37C=_tmp368.f2;if((int)_tmp37C != 0)goto _LL23E;_LL23D: if(_tmp336 == (void*)6)
return({struct _tuple7 _tmp393;_tmp393.f1=f;_tmp393.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp394;(_tmp394.BottomFL).tag=0;_tmp394;});
_tmp393;});else{return({struct _tuple7 _tmp395;_tmp395.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp396;(_tmp396.BottomFL).tag=0;_tmp396;});
_tmp395.f2=f;_tmp395;});}_LL23E:;_LL23F: goto _LL22B;_LL22B:;}{struct _tuple0
_tmp398=({struct _tuple0 _tmp397;_tmp397.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp361->topt))->v);_tmp397.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp362->topt))->v);_tmp397;});void*_tmp399;void*
_tmp39A;void*_tmp39B;void*_tmp39C;void*_tmp39D;void*_tmp39E;_LL24F: _tmp399=
_tmp398.f1;if(_tmp399 <= (void*)4)goto _LL251;if(*((int*)_tmp399)!= 5)goto _LL251;
_tmp39A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp399)->f1;if((int)_tmp39A != 
1)goto _LL251;_LL250: goto _LL252;_LL251: _tmp39B=_tmp398.f2;if(_tmp39B <= (void*)4)
goto _LL253;if(*((int*)_tmp39B)!= 5)goto _LL253;_tmp39C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp39B)->f1;if((int)_tmp39C != 1)goto _LL253;_LL252: goto _LL254;_LL253: _tmp39D=
_tmp398.f1;if(_tmp39D <= (void*)4)goto _LL255;if(*((int*)_tmp39D)!= 17)goto _LL255;
_LL254: goto _LL256;_LL255: _tmp39E=_tmp398.f2;if(_tmp39E <= (void*)4)goto _LL257;if(*((
int*)_tmp39E)!= 17)goto _LL257;_LL256: goto _LL24E;_LL257:;_LL258: return({struct
_tuple7 _tmp39F;_tmp39F.f1=f;_tmp39F.f2=f;_tmp39F;});_LL24E:;}{void*_tmp3A0=
_tmp336;_LL25A: if((int)_tmp3A0 != 5)goto _LL25C;_LL25B: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A1=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp361,r1,(void*)5,r2);_tmp3A1=Cyc_NewControlFlow_if_tagcmp(
env,_tmp3A1,_tmp362,r2,(void*)5,r1);return({struct _tuple7 _tmp3A2;_tmp3A2.f1=
_tmp3A1;_tmp3A2.f2=f;_tmp3A2;});}_LL25C: if((int)_tmp3A0 != 6)goto _LL25E;_LL25D: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp3A3=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp361,r1,(void*)5,r2);_tmp3A3=Cyc_NewControlFlow_if_tagcmp(env,_tmp3A3,_tmp362,
r2,(void*)5,r1);return({struct _tuple7 _tmp3A4;_tmp3A4.f1=f;_tmp3A4.f2=_tmp3A3;
_tmp3A4;});}_LL25E: if((int)_tmp3A0 != 7)goto _LL260;_LL25F: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A5=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp362,r2,(void*)8,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A6=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp361,r1,(void*)10,r2);return({
struct _tuple7 _tmp3A7;_tmp3A7.f1=_tmp3A5;_tmp3A7.f2=_tmp3A6;_tmp3A7;});}_LL260:
if((int)_tmp3A0 != 9)goto _LL262;_LL261: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A8=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp362,r2,(void*)10,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A9=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp361,r1,(void*)8,r2);return({
struct _tuple7 _tmp3AA;_tmp3AA.f1=_tmp3A8;_tmp3AA.f2=_tmp3A9;_tmp3AA;});}_LL262:
if((int)_tmp3A0 != 8)goto _LL264;_LL263: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3AB=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp361,r1,(void*)8,r2);union Cyc_CfFlowInfo_FlowInfo_union
_tmp3AC=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp362,r2,(void*)10,r1);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp3AD=_tmp3AB;struct Cyc_Dict_Dict*_tmp3AE;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3AF;_LL269: if((_tmp3AD.BottomFL).tag != 0)goto _LL26B;_LL26A:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp3B0=_cycalloc(sizeof(*_tmp3B0));_tmp3B0[0]=({
struct Cyc_Core_Impossible_struct _tmp3B1;_tmp3B1.tag=Cyc_Core_Impossible;_tmp3B1.f1=({
const char*_tmp3B2="anal_test, Lt";_tag_dynforward(_tmp3B2,sizeof(char),
_get_zero_arr_size(_tmp3B2,14));});_tmp3B1;});_tmp3B0;}));_LL26B: if((_tmp3AD.ReachableFL).tag
!= 1)goto _LL268;_tmp3AE=(_tmp3AD.ReachableFL).f1;_tmp3AF=(_tmp3AD.ReachableFL).f3;
_LL26C: _tmp35D=_tmp3AE;_tmp35F=_tmp3AF;_LL268:;}{void*_tmp3B3=(void*)_tmp361->r;
void*_tmp3B4;struct Cyc_Absyn_Vardecl*_tmp3B5;void*_tmp3B6;struct Cyc_Absyn_Vardecl*
_tmp3B7;void*_tmp3B8;struct Cyc_Absyn_Vardecl*_tmp3B9;void*_tmp3BA;struct Cyc_Absyn_Vardecl*
_tmp3BB;_LL26E: if(*((int*)_tmp3B3)!= 1)goto _LL270;_tmp3B4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3B3)->f2;if(_tmp3B4 <= (void*)1)goto _LL270;if(*((int*)_tmp3B4)!= 0)goto _LL270;
_tmp3B5=((struct Cyc_Absyn_Global_b_struct*)_tmp3B4)->f1;if(!(!_tmp3B5->escapes))
goto _LL270;_LL26F: _tmp3B7=_tmp3B5;goto _LL271;_LL270: if(*((int*)_tmp3B3)!= 1)goto
_LL272;_tmp3B6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3B3)->f2;if(_tmp3B6 <= (
void*)1)goto _LL272;if(*((int*)_tmp3B6)!= 3)goto _LL272;_tmp3B7=((struct Cyc_Absyn_Local_b_struct*)
_tmp3B6)->f1;if(!(!_tmp3B7->escapes))goto _LL272;_LL271: _tmp3B9=_tmp3B7;goto
_LL273;_LL272: if(*((int*)_tmp3B3)!= 1)goto _LL274;_tmp3B8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3B3)->f2;if(_tmp3B8 <= (void*)1)goto _LL274;if(*((int*)_tmp3B8)!= 4)goto _LL274;
_tmp3B9=((struct Cyc_Absyn_Pat_b_struct*)_tmp3B8)->f1;if(!(!_tmp3B9->escapes))
goto _LL274;_LL273: _tmp3BB=_tmp3B9;goto _LL275;_LL274: if(*((int*)_tmp3B3)!= 1)goto
_LL276;_tmp3BA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3B3)->f2;if(_tmp3BA <= (
void*)1)goto _LL276;if(*((int*)_tmp3BA)!= 2)goto _LL276;_tmp3BB=((struct Cyc_Absyn_Param_b_struct*)
_tmp3BA)->f1;if(!(!_tmp3BB->escapes))goto _LL276;_LL275: {void*_tmp3BC=(void*)
_tmp362->r;void*_tmp3BD;struct Cyc_Absyn_Vardecl*_tmp3BE;void*_tmp3BF;struct Cyc_Absyn_Vardecl*
_tmp3C0;void*_tmp3C1;struct Cyc_Absyn_Vardecl*_tmp3C2;void*_tmp3C3;struct Cyc_Absyn_Vardecl*
_tmp3C4;union Cyc_Absyn_Cnst_union _tmp3C5;int _tmp3C6;void*_tmp3C7;struct Cyc_List_List*
_tmp3C8;struct Cyc_List_List _tmp3C9;struct Cyc_Absyn_Exp*_tmp3CA;_LL279: if(*((int*)
_tmp3BC)!= 1)goto _LL27B;_tmp3BD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3BC)->f2;
if(_tmp3BD <= (void*)1)goto _LL27B;if(*((int*)_tmp3BD)!= 0)goto _LL27B;_tmp3BE=((
struct Cyc_Absyn_Global_b_struct*)_tmp3BD)->f1;if(!(!_tmp3BE->escapes))goto _LL27B;
_LL27A: _tmp3C0=_tmp3BE;goto _LL27C;_LL27B: if(*((int*)_tmp3BC)!= 1)goto _LL27D;
_tmp3BF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3BC)->f2;if(_tmp3BF <= (void*)
1)goto _LL27D;if(*((int*)_tmp3BF)!= 3)goto _LL27D;_tmp3C0=((struct Cyc_Absyn_Local_b_struct*)
_tmp3BF)->f1;if(!(!_tmp3C0->escapes))goto _LL27D;_LL27C: _tmp3C2=_tmp3C0;goto
_LL27E;_LL27D: if(*((int*)_tmp3BC)!= 1)goto _LL27F;_tmp3C1=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3BC)->f2;if(_tmp3C1 <= (void*)1)goto _LL27F;if(*((int*)_tmp3C1)!= 4)goto _LL27F;
_tmp3C2=((struct Cyc_Absyn_Pat_b_struct*)_tmp3C1)->f1;if(!(!_tmp3C2->escapes))
goto _LL27F;_LL27E: _tmp3C4=_tmp3C2;goto _LL280;_LL27F: if(*((int*)_tmp3BC)!= 1)goto
_LL281;_tmp3C3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3BC)->f2;if(_tmp3C3 <= (
void*)1)goto _LL281;if(*((int*)_tmp3C3)!= 2)goto _LL281;_tmp3C4=((struct Cyc_Absyn_Param_b_struct*)
_tmp3C3)->f1;if(!(!_tmp3C4->escapes))goto _LL281;_LL280: {struct Cyc_List_List*
_tmp3CB=({struct Cyc_List_List*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));_tmp3CE->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF->vd=
_tmp3BB;_tmp3CF->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp3D0;(_tmp3D0.LessVar).tag=1;(_tmp3D0.LessVar).f1=_tmp3C4;_tmp3D0;});_tmp3CF;});
_tmp3CE->tl=_tmp35E;_tmp3CE;});return({struct _tuple7 _tmp3CC;_tmp3CC.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3CD;(_tmp3CD.ReachableFL).tag=1;(_tmp3CD.ReachableFL).f1=
_tmp35D;(_tmp3CD.ReachableFL).f2=_tmp3CB;(_tmp3CD.ReachableFL).f3=_tmp35F;
_tmp3CD;});_tmp3CC.f2=_tmp3AC;_tmp3CC;});}_LL281: if(*((int*)_tmp3BC)!= 0)goto
_LL283;_tmp3C5=((struct Cyc_Absyn_Const_e_struct*)_tmp3BC)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp3BC)->f1).Int_c).tag != 2)goto _LL283;_tmp3C6=(_tmp3C5.Int_c).f2;_LL282: {
struct Cyc_List_List*_tmp3D1=({struct Cyc_List_List*_tmp3D4=_cycalloc(sizeof(*
_tmp3D4));_tmp3D4->hd=({struct Cyc_CfFlowInfo_Reln*_tmp3D5=_cycalloc(sizeof(*
_tmp3D5));_tmp3D5->vd=_tmp3BB;_tmp3D5->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp3D6;(_tmp3D6.LessConst).tag=3;(_tmp3D6.LessConst).f1=(
unsigned int)_tmp3C6;_tmp3D6;});_tmp3D5;});_tmp3D4->tl=_tmp35E;_tmp3D4;});return({
struct _tuple7 _tmp3D2;_tmp3D2.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D3;(_tmp3D3.ReachableFL).tag=1;(_tmp3D3.ReachableFL).f1=_tmp35D;(_tmp3D3.ReachableFL).f2=
_tmp3D1;(_tmp3D3.ReachableFL).f3=_tmp35F;_tmp3D3;});_tmp3D2.f2=_tmp3AC;_tmp3D2;});}
_LL283: if(*((int*)_tmp3BC)!= 3)goto _LL285;_tmp3C7=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3BC)->f1;if((int)_tmp3C7 != 19)goto _LL285;_tmp3C8=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3BC)->f2;if(_tmp3C8 == 0)goto _LL285;_tmp3C9=*_tmp3C8;_tmp3CA=(struct Cyc_Absyn_Exp*)
_tmp3C9.hd;_LL284: {void*_tmp3D7=(void*)_tmp3CA->r;void*_tmp3D8;struct Cyc_Absyn_Vardecl*
_tmp3D9;void*_tmp3DA;struct Cyc_Absyn_Vardecl*_tmp3DB;void*_tmp3DC;struct Cyc_Absyn_Vardecl*
_tmp3DD;void*_tmp3DE;struct Cyc_Absyn_Vardecl*_tmp3DF;_LL288: if(*((int*)_tmp3D7)
!= 1)goto _LL28A;_tmp3D8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3D7)->f2;if(
_tmp3D8 <= (void*)1)goto _LL28A;if(*((int*)_tmp3D8)!= 0)goto _LL28A;_tmp3D9=((
struct Cyc_Absyn_Global_b_struct*)_tmp3D8)->f1;if(!(!_tmp3D9->escapes))goto _LL28A;
_LL289: _tmp3DB=_tmp3D9;goto _LL28B;_LL28A: if(*((int*)_tmp3D7)!= 1)goto _LL28C;
_tmp3DA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3D7)->f2;if(_tmp3DA <= (void*)
1)goto _LL28C;if(*((int*)_tmp3DA)!= 3)goto _LL28C;_tmp3DB=((struct Cyc_Absyn_Local_b_struct*)
_tmp3DA)->f1;if(!(!_tmp3DB->escapes))goto _LL28C;_LL28B: _tmp3DD=_tmp3DB;goto
_LL28D;_LL28C: if(*((int*)_tmp3D7)!= 1)goto _LL28E;_tmp3DC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3D7)->f2;if(_tmp3DC <= (void*)1)goto _LL28E;if(*((int*)_tmp3DC)!= 4)goto _LL28E;
_tmp3DD=((struct Cyc_Absyn_Pat_b_struct*)_tmp3DC)->f1;if(!(!_tmp3DD->escapes))
goto _LL28E;_LL28D: _tmp3DF=_tmp3DD;goto _LL28F;_LL28E: if(*((int*)_tmp3D7)!= 1)goto
_LL290;_tmp3DE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3D7)->f2;if(_tmp3DE <= (
void*)1)goto _LL290;if(*((int*)_tmp3DE)!= 2)goto _LL290;_tmp3DF=((struct Cyc_Absyn_Param_b_struct*)
_tmp3DE)->f1;if(!(!_tmp3DF->escapes))goto _LL290;_LL28F: {struct Cyc_List_List*
_tmp3E0=({struct Cyc_List_List*_tmp3E3=_cycalloc(sizeof(*_tmp3E3));_tmp3E3->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));_tmp3E4->vd=
_tmp3BB;_tmp3E4->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp3E5;(_tmp3E5.LessSize).tag=2;(_tmp3E5.LessSize).f1=_tmp3DF;_tmp3E5;});
_tmp3E4;});_tmp3E3->tl=_tmp35E;_tmp3E3;});return({struct _tuple7 _tmp3E1;_tmp3E1.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp3E2;(
_tmp3E2.ReachableFL).tag=1;(_tmp3E2.ReachableFL).f1=_tmp35D;(_tmp3E2.ReachableFL).f2=
_tmp3E0;(_tmp3E2.ReachableFL).f3=_tmp35F;_tmp3E2;});_tmp3E1.f2=_tmp3AC;_tmp3E1;});}
_LL290:;_LL291: return({struct _tuple7 _tmp3E6;_tmp3E6.f1=_tmp3AB;_tmp3E6.f2=
_tmp3AC;_tmp3E6;});_LL287:;}_LL285:;_LL286: return({struct _tuple7 _tmp3E7;_tmp3E7.f1=
_tmp3AB;_tmp3E7.f2=_tmp3AC;_tmp3E7;});_LL278:;}_LL276:;_LL277: return({struct
_tuple7 _tmp3E8;_tmp3E8.f1=_tmp3AB;_tmp3E8.f2=_tmp3AC;_tmp3E8;});_LL26D:;}}_LL264:
if((int)_tmp3A0 != 10)goto _LL266;_LL265: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3E9=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp361,r1,(void*)10,r2);union Cyc_CfFlowInfo_FlowInfo_union
_tmp3EA=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp362,r1,(void*)8,r1);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp3EB=_tmp3E9;struct Cyc_Dict_Dict*_tmp3EC;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3ED;_LL293: if((_tmp3EB.BottomFL).tag != 0)goto _LL295;_LL294:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp3EE=_cycalloc(sizeof(*_tmp3EE));_tmp3EE[0]=({
struct Cyc_Core_Impossible_struct _tmp3EF;_tmp3EF.tag=Cyc_Core_Impossible;_tmp3EF.f1=({
const char*_tmp3F0="anal_test, Lte";_tag_dynforward(_tmp3F0,sizeof(char),
_get_zero_arr_size(_tmp3F0,15));});_tmp3EF;});_tmp3EE;}));_LL295: if((_tmp3EB.ReachableFL).tag
!= 1)goto _LL292;_tmp3EC=(_tmp3EB.ReachableFL).f1;_tmp3ED=(_tmp3EB.ReachableFL).f3;
_LL296: _tmp35D=_tmp3EC;_tmp35F=_tmp3ED;_LL292:;}{void*_tmp3F1=(void*)_tmp361->r;
void*_tmp3F2;struct Cyc_Absyn_Vardecl*_tmp3F3;void*_tmp3F4;struct Cyc_Absyn_Vardecl*
_tmp3F5;void*_tmp3F6;struct Cyc_Absyn_Vardecl*_tmp3F7;void*_tmp3F8;struct Cyc_Absyn_Vardecl*
_tmp3F9;_LL298: if(*((int*)_tmp3F1)!= 1)goto _LL29A;_tmp3F2=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3F1)->f2;if(_tmp3F2 <= (void*)1)goto _LL29A;if(*((int*)_tmp3F2)!= 0)goto _LL29A;
_tmp3F3=((struct Cyc_Absyn_Global_b_struct*)_tmp3F2)->f1;if(!(!_tmp3F3->escapes))
goto _LL29A;_LL299: _tmp3F5=_tmp3F3;goto _LL29B;_LL29A: if(*((int*)_tmp3F1)!= 1)goto
_LL29C;_tmp3F4=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3F1)->f2;if(_tmp3F4 <= (
void*)1)goto _LL29C;if(*((int*)_tmp3F4)!= 3)goto _LL29C;_tmp3F5=((struct Cyc_Absyn_Local_b_struct*)
_tmp3F4)->f1;if(!(!_tmp3F5->escapes))goto _LL29C;_LL29B: _tmp3F7=_tmp3F5;goto
_LL29D;_LL29C: if(*((int*)_tmp3F1)!= 1)goto _LL29E;_tmp3F6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3F1)->f2;if(_tmp3F6 <= (void*)1)goto _LL29E;if(*((int*)_tmp3F6)!= 4)goto _LL29E;
_tmp3F7=((struct Cyc_Absyn_Pat_b_struct*)_tmp3F6)->f1;if(!(!_tmp3F7->escapes))
goto _LL29E;_LL29D: _tmp3F9=_tmp3F7;goto _LL29F;_LL29E: if(*((int*)_tmp3F1)!= 1)goto
_LL2A0;_tmp3F8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3F1)->f2;if(_tmp3F8 <= (
void*)1)goto _LL2A0;if(*((int*)_tmp3F8)!= 2)goto _LL2A0;_tmp3F9=((struct Cyc_Absyn_Param_b_struct*)
_tmp3F8)->f1;if(!(!_tmp3F9->escapes))goto _LL2A0;_LL29F: {void*_tmp3FA=(void*)
_tmp362->r;void*_tmp3FB;struct Cyc_List_List*_tmp3FC;struct Cyc_List_List _tmp3FD;
struct Cyc_Absyn_Exp*_tmp3FE;_LL2A3: if(*((int*)_tmp3FA)!= 3)goto _LL2A5;_tmp3FB=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp3FA)->f1;if((int)_tmp3FB != 19)goto
_LL2A5;_tmp3FC=((struct Cyc_Absyn_Primop_e_struct*)_tmp3FA)->f2;if(_tmp3FC == 0)
goto _LL2A5;_tmp3FD=*_tmp3FC;_tmp3FE=(struct Cyc_Absyn_Exp*)_tmp3FD.hd;_LL2A4: {
void*_tmp3FF=(void*)_tmp3FE->r;void*_tmp400;struct Cyc_Absyn_Vardecl*_tmp401;void*
_tmp402;struct Cyc_Absyn_Vardecl*_tmp403;void*_tmp404;struct Cyc_Absyn_Vardecl*
_tmp405;void*_tmp406;struct Cyc_Absyn_Vardecl*_tmp407;_LL2A8: if(*((int*)_tmp3FF)
!= 1)goto _LL2AA;_tmp400=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3FF)->f2;if(
_tmp400 <= (void*)1)goto _LL2AA;if(*((int*)_tmp400)!= 0)goto _LL2AA;_tmp401=((
struct Cyc_Absyn_Global_b_struct*)_tmp400)->f1;if(!(!_tmp401->escapes))goto _LL2AA;
_LL2A9: _tmp403=_tmp401;goto _LL2AB;_LL2AA: if(*((int*)_tmp3FF)!= 1)goto _LL2AC;
_tmp402=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3FF)->f2;if(_tmp402 <= (void*)
1)goto _LL2AC;if(*((int*)_tmp402)!= 3)goto _LL2AC;_tmp403=((struct Cyc_Absyn_Local_b_struct*)
_tmp402)->f1;if(!(!_tmp403->escapes))goto _LL2AC;_LL2AB: _tmp405=_tmp403;goto
_LL2AD;_LL2AC: if(*((int*)_tmp3FF)!= 1)goto _LL2AE;_tmp404=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3FF)->f2;if(_tmp404 <= (void*)1)goto _LL2AE;if(*((int*)_tmp404)!= 4)goto _LL2AE;
_tmp405=((struct Cyc_Absyn_Pat_b_struct*)_tmp404)->f1;if(!(!_tmp405->escapes))
goto _LL2AE;_LL2AD: _tmp407=_tmp405;goto _LL2AF;_LL2AE: if(*((int*)_tmp3FF)!= 1)goto
_LL2B0;_tmp406=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3FF)->f2;if(_tmp406 <= (
void*)1)goto _LL2B0;if(*((int*)_tmp406)!= 2)goto _LL2B0;_tmp407=((struct Cyc_Absyn_Param_b_struct*)
_tmp406)->f1;if(!(!_tmp407->escapes))goto _LL2B0;_LL2AF: {struct Cyc_List_List*
_tmp408=({struct Cyc_List_List*_tmp40B=_cycalloc(sizeof(*_tmp40B));_tmp40B->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C->vd=
_tmp3F9;_tmp40C->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp40D;(_tmp40D.LessEqSize).tag=4;(_tmp40D.LessEqSize).f1=_tmp407;_tmp40D;});
_tmp40C;});_tmp40B->tl=_tmp35E;_tmp40B;});return({struct _tuple7 _tmp409;_tmp409.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp40A;(
_tmp40A.ReachableFL).tag=1;(_tmp40A.ReachableFL).f1=_tmp35D;(_tmp40A.ReachableFL).f2=
_tmp408;(_tmp40A.ReachableFL).f3=_tmp35F;_tmp40A;});_tmp409.f2=_tmp3EA;_tmp409;});}
_LL2B0:;_LL2B1: return({struct _tuple7 _tmp40E;_tmp40E.f1=_tmp3E9;_tmp40E.f2=
_tmp3EA;_tmp40E;});_LL2A7:;}_LL2A5:;_LL2A6: return({struct _tuple7 _tmp40F;_tmp40F.f1=
_tmp3E9;_tmp40F.f2=_tmp3EA;_tmp40F;});_LL2A2:;}_LL2A0:;_LL2A1: return({struct
_tuple7 _tmp410;_tmp410.f1=_tmp3E9;_tmp410.f2=_tmp3EA;_tmp410;});_LL297:;}}_LL266:;
_LL267: return({struct _tuple7 _tmp411;_tmp411.f1=f;_tmp411.f2=f;_tmp411;});_LL259:;}}
_LL226:;}}_LL21D:;_LL21E: goto _LL210;_LL210:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp413;void*_tmp414;struct _tuple5 _tmp412=Cyc_NewControlFlow_anal_Rexp(env,
inflow,e);_tmp413=_tmp412.f1;_tmp414=_tmp412.f2;_tmp413=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp413);{union Cyc_CfFlowInfo_FlowInfo_union _tmp415=_tmp413;struct Cyc_Dict_Dict*
_tmp416;_LL2B3: if((_tmp415.BottomFL).tag != 0)goto _LL2B5;_LL2B4: return({struct
_tuple7 _tmp417;_tmp417.f1=_tmp413;_tmp417.f2=_tmp413;_tmp417;});_LL2B5: if((
_tmp415.ReachableFL).tag != 1)goto _LL2B2;_tmp416=(_tmp415.ReachableFL).f1;_LL2B6: {
void*_tmp418=_tmp414;void*_tmp419;void*_tmp41A;void*_tmp41B;_LL2B8: if((int)
_tmp418 != 0)goto _LL2BA;_LL2B9: return({struct _tuple7 _tmp41C;_tmp41C.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp41D;(_tmp41D.BottomFL).tag=0;_tmp41D;});
_tmp41C.f2=_tmp413;_tmp41C;});_LL2BA: if((int)_tmp418 != 2)goto _LL2BC;_LL2BB: goto
_LL2BD;_LL2BC: if((int)_tmp418 != 1)goto _LL2BE;_LL2BD: goto _LL2BF;_LL2BE: if(_tmp418
<= (void*)3)goto _LL2C0;if(*((int*)_tmp418)!= 2)goto _LL2C0;_LL2BF: return({struct
_tuple7 _tmp41E;_tmp41E.f1=_tmp413;_tmp41E.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp41F;(_tmp41F.BottomFL).tag=0;_tmp41F;});
_tmp41E;});_LL2C0: if(_tmp418 <= (void*)3)goto _LL2C2;if(*((int*)_tmp418)!= 0)goto
_LL2C2;_tmp419=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp418)->f1;if((
int)_tmp419 != 0)goto _LL2C2;_LL2C1: goto _LL2C3;_LL2C2: if(_tmp418 <= (void*)3)goto
_LL2C4;if(*((int*)_tmp418)!= 1)goto _LL2C4;_tmp41A=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp418)->f1;if((int)_tmp41A != 0)goto _LL2C4;_LL2C3:({void*_tmp420[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp421="expression may not be initialized";_tag_dynforward(
_tmp421,sizeof(char),_get_zero_arr_size(_tmp421,34));}),_tag_dynforward(_tmp420,
sizeof(void*),0));});return({struct _tuple7 _tmp422;_tmp422.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp424;(_tmp424.BottomFL).tag=0;_tmp424;});
_tmp422.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp423;(_tmp423.BottomFL).tag=0;_tmp423;});_tmp422;});_LL2C4: if(_tmp418 <= (void*)
3)goto _LL2C6;if(*((int*)_tmp418)!= 0)goto _LL2C6;_tmp41B=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp418)->f1;_LL2C5: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp413,e,
_tmp41B);_LL2C6: if(_tmp418 <= (void*)3)goto _LL2C8;if(*((int*)_tmp418)!= 1)goto
_LL2C8;_LL2C7: return({struct _tuple7 _tmp425;_tmp425.f1=_tmp413;_tmp425.f2=_tmp413;
_tmp425;});_LL2C8: if(_tmp418 <= (void*)3)goto _LL2CA;if(*((int*)_tmp418)!= 3)goto
_LL2CA;_LL2C9: return({struct _tuple7 _tmp426;_tmp426.f1=_tmp413;_tmp426.f2=_tmp413;
_tmp426;});_LL2CA: if(_tmp418 <= (void*)3)goto _LL2B7;if(*((int*)_tmp418)!= 4)goto
_LL2B7;_LL2CB:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp427=
_cycalloc(sizeof(*_tmp427));_tmp427[0]=({struct Cyc_Core_Impossible_struct _tmp428;
_tmp428.tag=Cyc_Core_Impossible;_tmp428.f1=({const char*_tmp429="anal_test";
_tag_dynforward(_tmp429,sizeof(char),_get_zero_arr_size(_tmp429,10));});_tmp428;});
_tmp427;}));_LL2B7:;}_LL2B2:;}}}static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
flow){union Cyc_CfFlowInfo_FlowInfo_union _tmp42A=flow;struct Cyc_Dict_Dict*_tmp42B;
_LL2CD: if((_tmp42A.BottomFL).tag != 0)goto _LL2CF;_LL2CE: return;_LL2CF: if((_tmp42A.ReachableFL).tag
!= 1)goto _LL2CC;_tmp42B=(_tmp42A.ReachableFL).f1;_LL2D0:{struct Cyc_List_List*
_tmp42C=env->param_roots;for(0;_tmp42C != 0;_tmp42C=_tmp42C->tl){if(Cyc_CfFlowInfo_initlevel(
_tmp42B,Cyc_CfFlowInfo_lookup_place(_tmp42B,(struct Cyc_CfFlowInfo_Place*)_tmp42C->hd))
!= (void*)2)({void*_tmp42D[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp42E="function may not initialize all the parameters with attribute 'initializes'";
_tag_dynforward(_tmp42E,sizeof(char),_get_zero_arr_size(_tmp42E,76));}),
_tag_dynforward(_tmp42D,sizeof(void*),0));});}}return;_LL2CC:;}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause
_tmp430;struct Cyc_Core_Opt*_tmp431;struct Cyc_Absyn_Exp*_tmp432;struct Cyc_Absyn_Stmt*
_tmp433;struct Cyc_Position_Segment*_tmp434;struct Cyc_Absyn_Switch_clause*_tmp42F=(
struct Cyc_Absyn_Switch_clause*)scs->hd;_tmp430=*_tmp42F;_tmp431=_tmp430.pat_vars;
_tmp432=_tmp430.where_clause;_tmp433=_tmp430.body;_tmp434=_tmp430.loc;{union Cyc_CfFlowInfo_FlowInfo_union
clause_inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp431))->v,Cyc_CfFlowInfo_unknown_all,_tmp434);union
Cyc_CfFlowInfo_FlowInfo_union clause_outflow;if(_tmp432 != 0){union Cyc_CfFlowInfo_FlowInfo_union
_tmp436;union Cyc_CfFlowInfo_FlowInfo_union _tmp437;struct _tuple7 _tmp435=Cyc_NewControlFlow_anal_test(
env,clause_inflow,(struct Cyc_Absyn_Exp*)_tmp432);_tmp436=_tmp435.f1;_tmp437=
_tmp435.f2;inflow=_tmp437;clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
_tmp436,_tmp433);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp433);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp438=
clause_outflow;_LL2D2: if((_tmp438.BottomFL).tag != 0)goto _LL2D4;_LL2D3: goto _LL2D1;
_LL2D4:;_LL2D5: if(scs->tl == 0)return clause_outflow;else{if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->pat_vars))->v != 0)({void*_tmp439[0]={};Cyc_Tcutil_terr(
_tmp433->loc,({const char*_tmp43A="switch clause may implicitly fallthru";
_tag_dynforward(_tmp43A,sizeof(char),_get_zero_arr_size(_tmp43A,38));}),
_tag_dynforward(_tmp439,sizeof(void*),0));});else{({void*_tmp43B[0]={};Cyc_Tcutil_warn(
_tmp433->loc,({const char*_tmp43C="switch clause may implicitly fallthru";
_tag_dynforward(_tmp43C,sizeof(char),_get_zero_arr_size(_tmp43C,38));}),
_tag_dynforward(_tmp43B,sizeof(void*),0));});}Cyc_NewControlFlow_update_flow(env,((
struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL2D1;_LL2D1:;}}}return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp43D;(_tmp43D.BottomFL).tag=0;_tmp43D;});}
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union
outflow;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp43E=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);inflow=_tmp43E->flow;{void*_tmp43F=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp440;struct Cyc_Absyn_Exp*_tmp441;struct Cyc_Absyn_Exp*_tmp442;struct Cyc_Absyn_Stmt*
_tmp443;struct Cyc_Absyn_Stmt*_tmp444;struct Cyc_Absyn_Exp*_tmp445;struct Cyc_Absyn_Stmt*
_tmp446;struct Cyc_Absyn_Stmt*_tmp447;struct _tuple3 _tmp448;struct Cyc_Absyn_Exp*
_tmp449;struct Cyc_Absyn_Stmt*_tmp44A;struct Cyc_Absyn_Stmt*_tmp44B;struct Cyc_Absyn_Stmt*
_tmp44C;struct _tuple3 _tmp44D;struct Cyc_Absyn_Exp*_tmp44E;struct Cyc_Absyn_Stmt*
_tmp44F;struct Cyc_Absyn_Exp*_tmp450;struct _tuple3 _tmp451;struct Cyc_Absyn_Exp*
_tmp452;struct Cyc_Absyn_Stmt*_tmp453;struct _tuple3 _tmp454;struct Cyc_Absyn_Exp*
_tmp455;struct Cyc_Absyn_Stmt*_tmp456;struct Cyc_Absyn_Stmt*_tmp457;struct Cyc_Absyn_Stmt*
_tmp458;struct Cyc_List_List*_tmp459;struct Cyc_Absyn_Switch_clause**_tmp45A;
struct Cyc_Absyn_Switch_clause*_tmp45B;struct Cyc_Absyn_Stmt*_tmp45C;struct Cyc_Absyn_Stmt*
_tmp45D;struct Cyc_Absyn_Stmt*_tmp45E;struct Cyc_Absyn_Exp*_tmp45F;struct Cyc_List_List*
_tmp460;struct Cyc_Absyn_Stmt*_tmp461;struct Cyc_List_List*_tmp462;struct Cyc_Absyn_Decl*
_tmp463;struct Cyc_Absyn_Stmt*_tmp464;struct Cyc_Absyn_Stmt*_tmp465;struct Cyc_Absyn_Tvar*
_tmp466;struct Cyc_Absyn_Vardecl*_tmp467;int _tmp468;struct Cyc_Absyn_Stmt*_tmp469;
struct Cyc_Absyn_Exp*_tmp46A;_LL2D7: if((int)_tmp43F != 0)goto _LL2D9;_LL2D8: return
inflow;_LL2D9: if(_tmp43F <= (void*)1)goto _LL2FF;if(*((int*)_tmp43F)!= 2)goto
_LL2DB;_tmp440=((struct Cyc_Absyn_Return_s_struct*)_tmp43F)->f1;if(_tmp440 != 0)
goto _LL2DB;_LL2DA: if(env->noreturn)({void*_tmp46B[0]={};Cyc_Tcutil_terr(s->loc,({
const char*_tmp46C="`noreturn' function might return";_tag_dynforward(_tmp46C,
sizeof(char),_get_zero_arr_size(_tmp46C,33));}),_tag_dynforward(_tmp46B,sizeof(
void*),0));});Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp46D;(
_tmp46D.BottomFL).tag=0;_tmp46D;});_LL2DB: if(*((int*)_tmp43F)!= 2)goto _LL2DD;
_tmp441=((struct Cyc_Absyn_Return_s_struct*)_tmp43F)->f1;_LL2DC: if(env->noreturn)({
void*_tmp46E[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp46F="`noreturn' function might return";
_tag_dynforward(_tmp46F,sizeof(char),_get_zero_arr_size(_tmp46F,33));}),
_tag_dynforward(_tmp46E,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp471;void*_tmp472;struct _tuple5 _tmp470=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp441));_tmp471=_tmp470.f1;_tmp472=
_tmp470.f2;_tmp471=Cyc_CfFlowInfo_consume_unique_rvals(_tmp441->loc,_tmp471);
_tmp471=Cyc_NewControlFlow_use_Rval(env,_tmp441->loc,_tmp471,_tmp472);Cyc_NewControlFlow_check_init_params(
s->loc,env,_tmp471);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp473;(_tmp473.BottomFL).tag=0;_tmp473;});}_LL2DD: if(*((int*)_tmp43F)!= 0)goto
_LL2DF;_tmp442=((struct Cyc_Absyn_Exp_s_struct*)_tmp43F)->f1;_LL2DE: outflow=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp442)).f1;goto _LL2D6;_LL2DF: if(*((int*)_tmp43F)!= 1)goto _LL2E1;
_tmp443=((struct Cyc_Absyn_Seq_s_struct*)_tmp43F)->f1;_tmp444=((struct Cyc_Absyn_Seq_s_struct*)
_tmp43F)->f2;_LL2E0: outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp443),_tmp444);goto _LL2D6;_LL2E1: if(*((int*)_tmp43F)!= 3)goto
_LL2E3;_tmp445=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp43F)->f1;_tmp446=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp43F)->f2;_tmp447=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp43F)->f3;_LL2E2: {union Cyc_CfFlowInfo_FlowInfo_union _tmp475;union Cyc_CfFlowInfo_FlowInfo_union
_tmp476;struct _tuple7 _tmp474=Cyc_NewControlFlow_anal_test(env,inflow,_tmp445);
_tmp475=_tmp474.f1;_tmp476=_tmp474.f2;_tmp475=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp445->loc,_tmp475);_tmp476=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp445->loc,
_tmp476);outflow=Cyc_CfFlowInfo_join_flow(env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp475,_tmp446),Cyc_NewControlFlow_anal_stmt(env,_tmp476,_tmp447),1);goto
_LL2D6;}_LL2E3: if(*((int*)_tmp43F)!= 4)goto _LL2E5;_tmp448=((struct Cyc_Absyn_While_s_struct*)
_tmp43F)->f1;_tmp449=_tmp448.f1;_tmp44A=_tmp448.f2;_tmp44B=((struct Cyc_Absyn_While_s_struct*)
_tmp43F)->f2;_LL2E4: {union Cyc_CfFlowInfo_FlowInfo_union _tmp477=(Cyc_NewControlFlow_pre_stmt_check(
env,inflow,_tmp44A))->flow;union Cyc_CfFlowInfo_FlowInfo_union _tmp479;union Cyc_CfFlowInfo_FlowInfo_union
_tmp47A;struct _tuple7 _tmp478=Cyc_NewControlFlow_anal_test(env,_tmp477,_tmp449);
_tmp479=_tmp478.f1;_tmp47A=_tmp478.f2;_tmp479=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp449->loc,_tmp479);_tmp47A=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp449->loc,
_tmp47A);{union Cyc_CfFlowInfo_FlowInfo_union _tmp47B=Cyc_NewControlFlow_anal_stmt(
env,_tmp479,_tmp44B);Cyc_NewControlFlow_update_flow(env,_tmp44A,_tmp47B);outflow=
_tmp47A;goto _LL2D6;}}_LL2E5: if(*((int*)_tmp43F)!= 13)goto _LL2E7;_tmp44C=((struct
Cyc_Absyn_Do_s_struct*)_tmp43F)->f1;_tmp44D=((struct Cyc_Absyn_Do_s_struct*)
_tmp43F)->f2;_tmp44E=_tmp44D.f1;_tmp44F=_tmp44D.f2;_LL2E6: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp47C=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp44C);union Cyc_CfFlowInfo_FlowInfo_union
_tmp47D=(Cyc_NewControlFlow_pre_stmt_check(env,_tmp47C,_tmp44F))->flow;union Cyc_CfFlowInfo_FlowInfo_union
_tmp47F;union Cyc_CfFlowInfo_FlowInfo_union _tmp480;struct _tuple7 _tmp47E=Cyc_NewControlFlow_anal_test(
env,_tmp47D,_tmp44E);_tmp47F=_tmp47E.f1;_tmp480=_tmp47E.f2;Cyc_NewControlFlow_update_flow(
env,_tmp44C,_tmp47F);outflow=_tmp480;goto _LL2D6;}_LL2E7: if(*((int*)_tmp43F)!= 8)
goto _LL2E9;_tmp450=((struct Cyc_Absyn_For_s_struct*)_tmp43F)->f1;_tmp451=((struct
Cyc_Absyn_For_s_struct*)_tmp43F)->f2;_tmp452=_tmp451.f1;_tmp453=_tmp451.f2;
_tmp454=((struct Cyc_Absyn_For_s_struct*)_tmp43F)->f3;_tmp455=_tmp454.f1;_tmp456=
_tmp454.f2;_tmp457=((struct Cyc_Absyn_For_s_struct*)_tmp43F)->f4;_LL2E8: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp481=(Cyc_NewControlFlow_anal_Rexp(env,inflow,
_tmp450)).f1;_tmp481=Cyc_CfFlowInfo_drop_unique_rvals(_tmp450->loc,_tmp481);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp482=(Cyc_NewControlFlow_pre_stmt_check(env,
_tmp481,_tmp453))->flow;union Cyc_CfFlowInfo_FlowInfo_union _tmp484;union Cyc_CfFlowInfo_FlowInfo_union
_tmp485;struct _tuple7 _tmp483=Cyc_NewControlFlow_anal_test(env,_tmp482,_tmp452);
_tmp484=_tmp483.f1;_tmp485=_tmp483.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp486=
Cyc_NewControlFlow_anal_stmt(env,_tmp484,_tmp457);union Cyc_CfFlowInfo_FlowInfo_union
_tmp487=(Cyc_NewControlFlow_pre_stmt_check(env,_tmp486,_tmp456))->flow;union Cyc_CfFlowInfo_FlowInfo_union
_tmp488=(Cyc_NewControlFlow_anal_Rexp(env,_tmp487,_tmp455)).f1;_tmp488=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp455->loc,_tmp488);Cyc_NewControlFlow_update_flow(env,_tmp453,_tmp488);
outflow=_tmp485;goto _LL2D6;}}}_LL2E9: if(*((int*)_tmp43F)!= 5)goto _LL2EB;_tmp458=((
struct Cyc_Absyn_Break_s_struct*)_tmp43F)->f1;if(_tmp458 != 0)goto _LL2EB;_LL2EA:
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp489;(_tmp489.BottomFL).tag=0;_tmp489;});_LL2EB: if(*((int*)_tmp43F)!= 10)goto
_LL2ED;_tmp459=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp43F)->f1;_tmp45A=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp43F)->f2;if(_tmp45A == 0)goto _LL2ED;
_tmp45B=*_tmp45A;_LL2EC: {struct _RegionHandle _tmp48A=_new_region("temp");struct
_RegionHandle*temp=& _tmp48A;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp48C;struct Cyc_List_List*_tmp48D;struct _tuple9 _tmp48B=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp459,0);_tmp48C=_tmp48B.f1;_tmp48D=_tmp48B.f2;for(0;_tmp48D != 
0;(_tmp48D=_tmp48D->tl,_tmp459=_tmp459->tl)){_tmp48C=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp459))->hd)->loc,
_tmp48C,(void*)_tmp48D->hd);}_tmp48C=Cyc_CfFlowInfo_consume_unique_rvals(s->loc,
_tmp48C);_tmp48C=Cyc_NewControlFlow_add_vars(_tmp48C,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp45B->pat_vars))->v,Cyc_CfFlowInfo_unknown_all,
s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp45B->body,
_tmp48C);{union Cyc_CfFlowInfo_FlowInfo_union _tmp48F=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp48E;(_tmp48E.BottomFL).tag=0;_tmp48E;});
_npop_handler(0);return _tmp48F;}};_pop_region(temp);}_LL2ED: if(*((int*)_tmp43F)
!= 5)goto _LL2EF;_tmp45C=((struct Cyc_Absyn_Break_s_struct*)_tmp43F)->f1;_LL2EE:
_tmp45D=_tmp45C;goto _LL2F0;_LL2EF: if(*((int*)_tmp43F)!= 6)goto _LL2F1;_tmp45D=((
struct Cyc_Absyn_Continue_s_struct*)_tmp43F)->f1;_LL2F0: _tmp45E=_tmp45D;goto
_LL2F2;_LL2F1: if(*((int*)_tmp43F)!= 7)goto _LL2F3;_tmp45E=((struct Cyc_Absyn_Goto_s_struct*)
_tmp43F)->f2;_LL2F2: if(env->iteration_num == 1){struct Cyc_Absyn_Stmt*_tmp490=
_tmp43E->encloser;struct Cyc_Absyn_Stmt*_tmp491=(Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)_check_null(_tmp45E)))->encloser;while(_tmp491 != _tmp490){
struct Cyc_Absyn_Stmt*_tmp492=(Cyc_NewControlFlow_get_stmt_annot(_tmp490))->encloser;
if(_tmp492 == _tmp490){({void*_tmp493[0]={};Cyc_Tcutil_terr(s->loc,({const char*
_tmp494="goto enters local scope or exception handler";_tag_dynforward(_tmp494,
sizeof(char),_get_zero_arr_size(_tmp494,45));}),_tag_dynforward(_tmp493,sizeof(
void*),0));});break;}_tmp490=_tmp492;}}Cyc_NewControlFlow_update_flow(env,(
struct Cyc_Absyn_Stmt*)_check_null(_tmp45E),inflow);return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp495;(_tmp495.BottomFL).tag=0;_tmp495;});
_LL2F3: if(*((int*)_tmp43F)!= 9)goto _LL2F5;_tmp45F=((struct Cyc_Absyn_Switch_s_struct*)
_tmp43F)->f1;_tmp460=((struct Cyc_Absyn_Switch_s_struct*)_tmp43F)->f2;_LL2F4: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp497;void*_tmp498;struct _tuple5 _tmp496=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp45F);_tmp497=_tmp496.f1;_tmp498=_tmp496.f2;_tmp497=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp45F->loc,_tmp497);_tmp497=Cyc_NewControlFlow_use_Rval(env,_tmp45F->loc,
_tmp497,_tmp498);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp497,_tmp460);goto
_LL2D6;}_LL2F5: if(*((int*)_tmp43F)!= 14)goto _LL2F7;_tmp461=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp43F)->f1;_tmp462=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp43F)->f2;_LL2F6: {
int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo_union old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo_union s1_outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp461);union Cyc_CfFlowInfo_FlowInfo_union
scs_inflow=env->tryflow;env->in_try=old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(
env,scs_inflow);{union Cyc_CfFlowInfo_FlowInfo_union scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp462);{union Cyc_CfFlowInfo_FlowInfo_union _tmp499=scs_outflow;
_LL302: if((_tmp499.BottomFL).tag != 0)goto _LL304;_LL303: goto _LL301;_LL304:;_LL305:({
void*_tmp49A[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp49B="last catch clause may implicitly fallthru";
_tag_dynforward(_tmp49B,sizeof(char),_get_zero_arr_size(_tmp49B,42));}),
_tag_dynforward(_tmp49A,sizeof(void*),0));});_LL301:;}outflow=s1_outflow;goto
_LL2D6;}}}_LL2F7: if(*((int*)_tmp43F)!= 11)goto _LL2F9;_tmp463=((struct Cyc_Absyn_Decl_s_struct*)
_tmp43F)->f1;_tmp464=((struct Cyc_Absyn_Decl_s_struct*)_tmp43F)->f2;_LL2F8:
outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,
_tmp463),_tmp464);goto _LL2D6;_LL2F9: if(*((int*)_tmp43F)!= 12)goto _LL2FB;_tmp465=((
struct Cyc_Absyn_Label_s_struct*)_tmp43F)->f2;_LL2FA: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp465);goto _LL2D6;_LL2FB: if(*((int*)_tmp43F)!= 15)goto _LL2FD;
_tmp466=((struct Cyc_Absyn_Region_s_struct*)_tmp43F)->f1;_tmp467=((struct Cyc_Absyn_Region_s_struct*)
_tmp43F)->f2;_tmp468=((struct Cyc_Absyn_Region_s_struct*)_tmp43F)->f3;_tmp469=((
struct Cyc_Absyn_Region_s_struct*)_tmp43F)->f4;_LL2FC: {struct Cyc_List_List
_tmp49C=({struct Cyc_List_List _tmp49D;_tmp49D.hd=_tmp467;_tmp49D.tl=0;_tmp49D;});
inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)& _tmp49C,Cyc_CfFlowInfo_unknown_all,
_tmp469->loc);outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp469);goto
_LL2D6;}_LL2FD: if(*((int*)_tmp43F)!= 16)goto _LL2FF;_tmp46A=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp43F)->f1;_LL2FE: {union Cyc_CfFlowInfo_FlowInfo_union _tmp49F;void*_tmp4A0;
struct _tuple5 _tmp49E=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp46A);_tmp49F=
_tmp49E.f1;_tmp4A0=_tmp49E.f2;_tmp49F=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp46A->loc,_tmp49F);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4A1=Cyc_NewControlFlow_use_Rval(
env,_tmp46A->loc,_tmp49F,_tmp4A0);{void*_tmp4A2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp46A->topt))->v);void*_tmp4A3;_LL307: if(
_tmp4A2 <= (void*)4)goto _LL309;if(*((int*)_tmp4A2)!= 15)goto _LL309;_tmp4A3=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp4A2)->f1;_LL308: outflow=Cyc_CfFlowInfo_kill_flow_region(
_tmp49F,_tmp4A3);goto _LL306;_LL309:;_LL30A:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp4A4=_cycalloc(sizeof(*_tmp4A4));_tmp4A4[0]=({struct Cyc_Core_Impossible_struct
_tmp4A5;_tmp4A5.tag=Cyc_Core_Impossible;_tmp4A5.f1=({const char*_tmp4A6="anal_stmt -- reset_region";
_tag_dynforward(_tmp4A6,sizeof(char),_get_zero_arr_size(_tmp4A6,26));});_tmp4A5;});
_tmp4A4;}));_LL306:;}goto _LL2D6;}}_LL2FF:;_LL300:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7[0]=({struct Cyc_Core_Impossible_struct
_tmp4A8;_tmp4A8.tag=Cyc_Core_Impossible;_tmp4A8.f1=({const char*_tmp4A9="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_dynforward(_tmp4A9,sizeof(char),_get_zero_arr_size(_tmp4A9,56));});_tmp4A8;});
_tmp4A7;}));_LL2D6:;}outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,outflow);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp4AA=outflow;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp4AB;_LL30C: if((_tmp4AA.ReachableFL).tag != 1)goto _LL30E;_tmp4AB=(_tmp4AA.ReachableFL).f3;
_LL30D: goto _LL30B;_LL30E:;_LL30F: goto _LL30B;_LL30B:;}return outflow;}static void
Cyc_NewControlFlow_check_nested_fun(union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Fndecl*fd);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Decl*decl){void*_tmp4AC=(void*)decl->r;struct Cyc_Absyn_Vardecl*
_tmp4AD;struct Cyc_Core_Opt*_tmp4AE;struct Cyc_Core_Opt _tmp4AF;struct Cyc_List_List*
_tmp4B0;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_List_List*_tmp4B2;struct Cyc_Absyn_Fndecl*
_tmp4B3;_LL311: if(_tmp4AC <= (void*)2)goto _LL319;if(*((int*)_tmp4AC)!= 0)goto
_LL313;_tmp4AD=((struct Cyc_Absyn_Var_d_struct*)_tmp4AC)->f1;_LL312: {struct Cyc_List_List
_tmp4B4=({struct Cyc_List_List _tmp4C3;_tmp4C3.hd=_tmp4AD;_tmp4C3.tl=0;_tmp4C3;});
inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)& _tmp4B4,Cyc_CfFlowInfo_unknown_none,
decl->loc);{struct Cyc_Absyn_Exp*_tmp4B5=_tmp4AD->initializer;if(_tmp4B5 == 0)
return inflow;{union Cyc_CfFlowInfo_FlowInfo_union _tmp4B7;void*_tmp4B8;struct
_tuple5 _tmp4B6=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
_tmp4B5);_tmp4B7=_tmp4B6.f1;_tmp4B8=_tmp4B6.f2;_tmp4B7=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4B5->loc,_tmp4B7);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4B9=_tmp4B7;struct
Cyc_Dict_Dict*_tmp4BA;struct Cyc_List_List*_tmp4BB;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp4BC;_LL31C: if((_tmp4B9.BottomFL).tag != 0)goto _LL31E;_LL31D: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4BD;(_tmp4BD.BottomFL).tag=0;_tmp4BD;});
_LL31E: if((_tmp4B9.ReachableFL).tag != 1)goto _LL31B;_tmp4BA=(_tmp4B9.ReachableFL).f1;
_tmp4BB=(_tmp4B9.ReachableFL).f2;_tmp4BC=(_tmp4B9.ReachableFL).f3;_LL31F: _tmp4BA=
Cyc_CfFlowInfo_assign_place(decl->loc,_tmp4BA,env->all_changed,({struct Cyc_CfFlowInfo_Place*
_tmp4BE=_cycalloc(sizeof(*_tmp4BE));_tmp4BE->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp4BF=_cycalloc(sizeof(*_tmp4BF));_tmp4BF[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp4C0;_tmp4C0.tag=0;_tmp4C0.f1=_tmp4AD;_tmp4C0;});_tmp4BF;}));_tmp4BE->fields=
0;_tmp4BE;}),_tmp4B8);_tmp4BB=Cyc_CfFlowInfo_reln_assign_var(_tmp4BB,_tmp4AD,(
struct Cyc_Absyn_Exp*)_check_null(_tmp4AD->initializer));{union Cyc_CfFlowInfo_FlowInfo_union
_tmp4C1=({union Cyc_CfFlowInfo_FlowInfo_union _tmp4C2;(_tmp4C2.ReachableFL).tag=1;(
_tmp4C2.ReachableFL).f1=_tmp4BA;(_tmp4C2.ReachableFL).f2=_tmp4BB;(_tmp4C2.ReachableFL).f3=
_tmp4BC;_tmp4C2;});Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp4C1);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp4C1;}_LL31B:;}}}}_LL313:
if(*((int*)_tmp4AC)!= 2)goto _LL315;_tmp4AE=((struct Cyc_Absyn_Let_d_struct*)
_tmp4AC)->f2;if(_tmp4AE == 0)goto _LL315;_tmp4AF=*_tmp4AE;_tmp4B0=(struct Cyc_List_List*)
_tmp4AF.v;_tmp4B1=((struct Cyc_Absyn_Let_d_struct*)_tmp4AC)->f3;_LL314: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp4C5;void*_tmp4C6;struct _tuple5 _tmp4C4=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp4B1);_tmp4C5=_tmp4C4.f1;_tmp4C6=_tmp4C4.f2;_tmp4C5=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4B1->loc,_tmp4C5);_tmp4C5=Cyc_NewControlFlow_use_Rval(env,_tmp4B1->loc,
_tmp4C5,_tmp4C6);return Cyc_NewControlFlow_add_vars(_tmp4C5,_tmp4B0,Cyc_CfFlowInfo_unknown_all,
decl->loc);}_LL315: if(*((int*)_tmp4AC)!= 3)goto _LL317;_tmp4B2=((struct Cyc_Absyn_Letv_d_struct*)
_tmp4AC)->f1;_LL316: return Cyc_NewControlFlow_add_vars(inflow,_tmp4B2,Cyc_CfFlowInfo_unknown_none,
decl->loc);_LL317: if(*((int*)_tmp4AC)!= 1)goto _LL319;_tmp4B3=((struct Cyc_Absyn_Fn_d_struct*)
_tmp4AC)->f1;_LL318: Cyc_NewControlFlow_check_nested_fun(inflow,_tmp4B3);{void*
_tmp4C7=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4B3->cached_typ))->v;struct
Cyc_Absyn_Vardecl*_tmp4C8=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp4B3->fn_vardecl);
return Cyc_NewControlFlow_add_vars(inflow,({struct Cyc_List_List*_tmp4C9=_cycalloc(
sizeof(*_tmp4C9));_tmp4C9->hd=_tmp4C8;_tmp4C9->tl=0;_tmp4C9;}),Cyc_CfFlowInfo_unknown_all,
decl->loc);}_LL319:;_LL31A:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp4CA=_cycalloc(sizeof(*_tmp4CA));_tmp4CA[0]=({struct Cyc_Core_Impossible_struct
_tmp4CB;_tmp4CB.tag=Cyc_Core_Impossible;_tmp4CB.f1=({const char*_tmp4CC="anal_decl: unexpected decl variant";
_tag_dynforward(_tmp4CC,sizeof(char),_get_zero_arr_size(_tmp4CC,35));});_tmp4CB;});
_tmp4CA;}));_LL310:;}static void Cyc_NewControlFlow_postpass(struct Cyc_Absyn_Stmt*
s){(void*)(s->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot));{void*_tmp4CD=(void*)s->r;
struct _tuple3 _tmp4CE;struct Cyc_Absyn_Stmt*_tmp4CF;struct _tuple3 _tmp4D0;struct Cyc_Absyn_Stmt*
_tmp4D1;struct Cyc_Absyn_Stmt*_tmp4D2;struct Cyc_Absyn_Stmt*_tmp4D3;struct Cyc_Absyn_Stmt*
_tmp4D4;struct _tuple3 _tmp4D5;struct Cyc_Absyn_Stmt*_tmp4D6;struct Cyc_Absyn_Stmt*
_tmp4D7;struct Cyc_Absyn_Stmt*_tmp4D8;struct _tuple3 _tmp4D9;struct Cyc_Absyn_Stmt*
_tmp4DA;struct Cyc_Absyn_Stmt*_tmp4DB;struct Cyc_Absyn_Stmt*_tmp4DC;struct Cyc_Absyn_Stmt*
_tmp4DD;struct Cyc_Absyn_Stmt*_tmp4DE;struct Cyc_Absyn_Stmt*_tmp4DF;struct Cyc_Absyn_Stmt*
_tmp4E0;struct Cyc_List_List*_tmp4E1;struct Cyc_List_List*_tmp4E2;_LL321: if(
_tmp4CD <= (void*)1)goto _LL335;if(*((int*)_tmp4CD)!= 8)goto _LL323;_tmp4CE=((
struct Cyc_Absyn_For_s_struct*)_tmp4CD)->f2;_tmp4CF=_tmp4CE.f2;_tmp4D0=((struct
Cyc_Absyn_For_s_struct*)_tmp4CD)->f3;_tmp4D1=_tmp4D0.f2;_tmp4D2=((struct Cyc_Absyn_For_s_struct*)
_tmp4CD)->f4;_LL322: Cyc_NewControlFlow_postpass(_tmp4D1);_tmp4D3=_tmp4D2;_tmp4D4=
_tmp4CF;goto _LL324;_LL323: if(*((int*)_tmp4CD)!= 1)goto _LL325;_tmp4D3=((struct Cyc_Absyn_Seq_s_struct*)
_tmp4CD)->f1;_tmp4D4=((struct Cyc_Absyn_Seq_s_struct*)_tmp4CD)->f2;_LL324: _tmp4D6=
_tmp4D3;_tmp4D7=_tmp4D4;goto _LL326;_LL325: if(*((int*)_tmp4CD)!= 4)goto _LL327;
_tmp4D5=((struct Cyc_Absyn_While_s_struct*)_tmp4CD)->f1;_tmp4D6=_tmp4D5.f2;
_tmp4D7=((struct Cyc_Absyn_While_s_struct*)_tmp4CD)->f2;_LL326: _tmp4D8=_tmp4D7;
_tmp4DA=_tmp4D6;goto _LL328;_LL327: if(*((int*)_tmp4CD)!= 13)goto _LL329;_tmp4D8=((
struct Cyc_Absyn_Do_s_struct*)_tmp4CD)->f1;_tmp4D9=((struct Cyc_Absyn_Do_s_struct*)
_tmp4CD)->f2;_tmp4DA=_tmp4D9.f2;_LL328: _tmp4DB=_tmp4D8;_tmp4DC=_tmp4DA;goto
_LL32A;_LL329: if(*((int*)_tmp4CD)!= 3)goto _LL32B;_tmp4DB=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp4CD)->f2;_tmp4DC=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4CD)->f3;_LL32A:
Cyc_NewControlFlow_postpass(_tmp4DB);_tmp4DD=_tmp4DC;goto _LL32C;_LL32B: if(*((int*)
_tmp4CD)!= 15)goto _LL32D;_tmp4DD=((struct Cyc_Absyn_Region_s_struct*)_tmp4CD)->f4;
_LL32C: _tmp4DE=_tmp4DD;goto _LL32E;_LL32D: if(*((int*)_tmp4CD)!= 12)goto _LL32F;
_tmp4DE=((struct Cyc_Absyn_Label_s_struct*)_tmp4CD)->f2;_LL32E: _tmp4DF=_tmp4DE;
goto _LL330;_LL32F: if(*((int*)_tmp4CD)!= 11)goto _LL331;_tmp4DF=((struct Cyc_Absyn_Decl_s_struct*)
_tmp4CD)->f2;_LL330: Cyc_NewControlFlow_postpass(_tmp4DF);return;_LL331: if(*((int*)
_tmp4CD)!= 14)goto _LL333;_tmp4E0=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp4CD)->f1;
_tmp4E1=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp4CD)->f2;_LL332: Cyc_NewControlFlow_postpass(
_tmp4E0);_tmp4E2=_tmp4E1;goto _LL334;_LL333: if(*((int*)_tmp4CD)!= 9)goto _LL335;
_tmp4E2=((struct Cyc_Absyn_Switch_s_struct*)_tmp4CD)->f2;_LL334: for(0;_tmp4E2 != 0;
_tmp4E2=_tmp4E2->tl){Cyc_NewControlFlow_postpass(((struct Cyc_Absyn_Switch_clause*)
_tmp4E2->hd)->body);}return;_LL335:;_LL336: return;_LL320:;}}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd){Cyc_NewControlFlow_check_nested_fun((union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4E3;(_tmp4E3.ReachableFL).tag=1;(_tmp4E3.ReachableFL).f1=
Cyc_CfFlowInfo_empty_flowdict();(_tmp4E3.ReachableFL).f2=0;(_tmp4E3.ReachableFL).f3=({
struct Cyc_CfFlowInfo_ConsumeInfo _tmp4E4;_tmp4E4.consumed=Cyc_CfFlowInfo_mt_place_set();
_tmp4E4.may_consume=0;_tmp4E4;});_tmp4E3;}),fd);}static void Cyc_NewControlFlow_check_nested_fun(
union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*fd){struct Cyc_Position_Segment*
_tmp4E5=(fd->body)->loc;inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,Cyc_CfFlowInfo_unknown_all,
_tmp4E5);{struct Cyc_List_List*_tmp4E6=0;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp4E7=inflow;struct Cyc_Dict_Dict*_tmp4E8;struct Cyc_List_List*_tmp4E9;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp4EA;_LL338: if((_tmp4E7.BottomFL).tag != 0)goto _LL33A;_LL339:({void*_tmp4EB[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp4EC="check_fun";_tag_dynforward(_tmp4EC,sizeof(char),
_get_zero_arr_size(_tmp4EC,10));}),_tag_dynforward(_tmp4EB,sizeof(void*),0));});
_LL33A: if((_tmp4E7.ReachableFL).tag != 1)goto _LL337;_tmp4E8=(_tmp4E7.ReachableFL).f1;
_tmp4E9=(_tmp4E7.ReachableFL).f2;_tmp4EA=(_tmp4E7.ReachableFL).f3;_LL33B: {
struct Cyc_List_List*atts;{void*_tmp4ED=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp4EE;struct Cyc_List_List*
_tmp4EF;_LL33D: if(_tmp4ED <= (void*)4)goto _LL33F;if(*((int*)_tmp4ED)!= 8)goto
_LL33F;_tmp4EE=((struct Cyc_Absyn_FnType_struct*)_tmp4ED)->f1;_tmp4EF=_tmp4EE.attributes;
_LL33E: atts=_tmp4EF;goto _LL33C;_LL33F:;_LL340:({void*_tmp4F0[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp4F1="check_fun: non-function type cached with fndecl_t";_tag_dynforward(
_tmp4F1,sizeof(char),_get_zero_arr_size(_tmp4F1,50));}),_tag_dynforward(_tmp4F0,
sizeof(void*),0));});_LL33C:;}for(0;atts != 0;atts=atts->tl){void*_tmp4F2=(void*)
atts->hd;int _tmp4F3;_LL342: if(_tmp4F2 <= (void*)17)goto _LL344;if(*((int*)_tmp4F2)
!= 4)goto _LL344;_tmp4F3=((struct Cyc_Absyn_Initializes_att_struct*)_tmp4F2)->f1;
_LL343: {unsigned int j=(unsigned int)_tmp4F3;if(j > ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp4F4[0]={};Cyc_Tcutil_terr(_tmp4E5,({const char*_tmp4F5="initializes attribute exceeds number of parameters";
_tag_dynforward(_tmp4F5,sizeof(char),_get_zero_arr_size(_tmp4F5,51));}),
_tag_dynforward(_tmp4F4,sizeof(void*),0));});continue;}{struct Cyc_Absyn_Vardecl*
_tmp4F6=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(
int)(j - 1));{void*_tmp4F7=Cyc_Tcutil_compress((void*)_tmp4F6->type);struct Cyc_Absyn_PtrInfo
_tmp4F8;void*_tmp4F9;struct Cyc_Absyn_PtrAtts _tmp4FA;struct Cyc_Absyn_Conref*
_tmp4FB;struct Cyc_Absyn_Conref*_tmp4FC;struct Cyc_Absyn_Conref*_tmp4FD;_LL347: if(
_tmp4F7 <= (void*)4)goto _LL349;if(*((int*)_tmp4F7)!= 4)goto _LL349;_tmp4F8=((
struct Cyc_Absyn_PointerType_struct*)_tmp4F7)->f1;_tmp4F9=(void*)_tmp4F8.elt_typ;
_tmp4FA=_tmp4F8.ptr_atts;_tmp4FB=_tmp4FA.nullable;_tmp4FC=_tmp4FA.bounds;_tmp4FD=
_tmp4FA.zero_term;_LL348: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp4FB))({void*_tmp4FE[0]={};Cyc_Tcutil_terr(_tmp4E5,({const char*_tmp4FF="initializes attribute not allowed on nullable pointers";
_tag_dynforward(_tmp4FF,sizeof(char),_get_zero_arr_size(_tmp4FF,55));}),
_tag_dynforward(_tmp4FE,sizeof(void*),0));});if(!Cyc_Tcutil_is_bound_one(_tmp4FC))({
void*_tmp500[0]={};Cyc_Tcutil_terr(_tmp4E5,({const char*_tmp501="initializes attribute allowed only on pointers of size 1";
_tag_dynforward(_tmp501,sizeof(char),_get_zero_arr_size(_tmp501,57));}),
_tag_dynforward(_tmp500,sizeof(void*),0));});if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp4FD))({void*_tmp502[0]={};Cyc_Tcutil_terr(_tmp4E5,({
const char*_tmp503="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_dynforward(_tmp503,sizeof(char),_get_zero_arr_size(_tmp503,77));}),
_tag_dynforward(_tmp502,sizeof(void*),0));});{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp504=({struct Cyc_CfFlowInfo_InitParam_struct*_tmp50C=_cycalloc(sizeof(*
_tmp50C));_tmp50C[0]=({struct Cyc_CfFlowInfo_InitParam_struct _tmp50D;_tmp50D.tag=
2;_tmp50D.f1=(int)j;_tmp50D.f2=(void*)_tmp4F9;_tmp50D;});_tmp50C;});struct Cyc_CfFlowInfo_Place*
_tmp505=({struct Cyc_CfFlowInfo_Place*_tmp50B=_cycalloc(sizeof(*_tmp50B));_tmp50B->root=(
void*)((void*)_tmp504);_tmp50B->fields=0;_tmp50B;});_tmp4E8=Cyc_Dict_insert(
_tmp4E8,(void*)_tmp504,Cyc_CfFlowInfo_typ_to_absrval(_tmp4F9,Cyc_CfFlowInfo_esc_none));
_tmp4E8=Cyc_Dict_insert(_tmp4E8,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp506=_cycalloc(sizeof(*_tmp506));_tmp506[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp507;_tmp507.tag=0;_tmp507.f1=_tmp4F6;_tmp507;});_tmp506;}),(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp508=_cycalloc(sizeof(*_tmp508));_tmp508[0]=({
struct Cyc_CfFlowInfo_AddressOf_struct _tmp509;_tmp509.tag=2;_tmp509.f1=_tmp505;
_tmp509;});_tmp508;}));_tmp4E6=({struct Cyc_List_List*_tmp50A=_cycalloc(sizeof(*
_tmp50A));_tmp50A->hd=_tmp505;_tmp50A->tl=_tmp4E6;_tmp50A;});goto _LL346;}_LL349:;
_LL34A:({void*_tmp50E[0]={};Cyc_Tcutil_terr(_tmp4E5,({const char*_tmp50F="initializes attribute on non-pointer";
_tag_dynforward(_tmp50F,sizeof(char),_get_zero_arr_size(_tmp50F,37));}),
_tag_dynforward(_tmp50E,sizeof(void*),0));});_LL346:;}goto _LL341;}}_LL344:;
_LL345: goto _LL341;_LL341:;}inflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp510;(_tmp510.ReachableFL).tag=1;(_tmp510.ReachableFL).f1=_tmp4E8;(_tmp510.ReachableFL).f2=
_tmp4E9;(_tmp510.ReachableFL).f3=_tmp4EA;_tmp510;});}_LL337:;}{int noreturn=Cyc_Tcutil_is_noreturn(
Cyc_Tcutil_fndecl2typ(fd));struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp519=_cycalloc(sizeof(*_tmp519));_tmp519->iterate_again=1;_tmp519->iteration_num=
0;_tmp519->in_try=0;_tmp519->tryflow=inflow;_tmp519->all_changed=0;_tmp519->noreturn=
noreturn;_tmp519->param_roots=_tmp4E6;_tmp519;});union Cyc_CfFlowInfo_FlowInfo_union
outflow=inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp511=outflow;_LL34C: if((_tmp511.BottomFL).tag != 0)goto _LL34E;_LL34D: goto
_LL34B;_LL34E:;_LL34F: Cyc_NewControlFlow_check_init_params(_tmp4E5,env,outflow);
if(noreturn)({void*_tmp512[0]={};Cyc_Tcutil_terr(_tmp4E5,({const char*_tmp513="`noreturn' function might (implicitly) return";
_tag_dynforward(_tmp513,sizeof(char),_get_zero_arr_size(_tmp513,46));}),
_tag_dynforward(_tmp512,sizeof(void*),0));});else{void*_tmp514=Cyc_Tcutil_compress((
void*)fd->ret_type);_LL351: if((int)_tmp514 != 0)goto _LL353;_LL352: goto _LL350;
_LL353: if(_tmp514 <= (void*)4)goto _LL355;if(*((int*)_tmp514)!= 6)goto _LL355;
_LL354: goto _LL356;_LL355: if((int)_tmp514 != 1)goto _LL357;_LL356: goto _LL358;_LL357:
if(_tmp514 <= (void*)4)goto _LL359;if(*((int*)_tmp514)!= 5)goto _LL359;_LL358:({
void*_tmp515[0]={};Cyc_Tcutil_warn(_tmp4E5,({const char*_tmp516="function may not return a value";
_tag_dynforward(_tmp516,sizeof(char),_get_zero_arr_size(_tmp516,32));}),
_tag_dynforward(_tmp515,sizeof(void*),0));});goto _LL350;_LL359:;_LL35A:({void*
_tmp517[0]={};Cyc_Tcutil_terr(_tmp4E5,({const char*_tmp518="function may not return a value";
_tag_dynforward(_tmp518,sizeof(char),_get_zero_arr_size(_tmp518,32));}),
_tag_dynforward(_tmp517,sizeof(void*),0));});goto _LL350;_LL350:;}goto _LL34B;
_LL34B:;}Cyc_NewControlFlow_postpass(fd->body);}}}void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){void*_tmp51A=(void*)((struct Cyc_Absyn_Decl*)
ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp51B;struct Cyc_List_List*_tmp51C;struct Cyc_List_List*
_tmp51D;struct Cyc_List_List*_tmp51E;_LL35C: if(_tmp51A <= (void*)2)goto _LL366;if(*((
int*)_tmp51A)!= 1)goto _LL35E;_tmp51B=((struct Cyc_Absyn_Fn_d_struct*)_tmp51A)->f1;
_LL35D: Cyc_NewControlFlow_check_fun(_tmp51B);goto _LL35B;_LL35E: if(*((int*)
_tmp51A)!= 10)goto _LL360;_tmp51C=((struct Cyc_Absyn_ExternC_d_struct*)_tmp51A)->f1;
_LL35F: _tmp51D=_tmp51C;goto _LL361;_LL360: if(*((int*)_tmp51A)!= 9)goto _LL362;
_tmp51D=((struct Cyc_Absyn_Using_d_struct*)_tmp51A)->f2;_LL361: _tmp51E=_tmp51D;
goto _LL363;_LL362: if(*((int*)_tmp51A)!= 8)goto _LL364;_tmp51E=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp51A)->f2;_LL363: Cyc_NewControlFlow_cf_check(_tmp51E);goto _LL35B;_LL364: if(*((
int*)_tmp51A)!= 11)goto _LL366;_LL365: goto _LL35B;_LL366:;_LL367: goto _LL35B;_LL35B:;}}
