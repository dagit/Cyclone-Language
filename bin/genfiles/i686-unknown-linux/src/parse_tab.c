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
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;typedef struct{
int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t
Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,
struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
struct _dynforward_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dynforward_ptr,
struct _dynforward_ptr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dynforward_ptr f1;};extern char
Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{char*tag;struct
_dynforward_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);
void*refill_state;struct _dynforward_ptr lex_buffer;int lex_buffer_len;int
lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;
int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(
struct _dynforward_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dynforward_ptr lex_base;struct _dynforward_ptr lex_backtrk;struct
_dynforward_ptr lex_default;struct _dynforward_ptr lex_trans;struct _dynforward_ptr
lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct
Cyc_List_List*x);void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*
x,void*accum);struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_flatten(
struct Cyc_List_List*x);extern char Cyc_List_Nth[8];struct Cyc_List_List*Cyc_List_zip(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);
unsigned int Cyc_strlen(struct _dynforward_ptr s);int Cyc_strcmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2);int Cyc_zstrcmp(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_zstrptrcmp(struct _dynforward_ptr*,struct
_dynforward_ptr*);struct _dynforward_ptr Cyc_strcat(struct _dynforward_ptr dest,
struct _dynforward_ptr src);struct _dynforward_ptr Cyc_strcpy(struct _dynforward_ptr
dest,struct _dynforward_ptr src);struct _dynforward_ptr Cyc_substring(struct
_dynforward_ptr,int ofs,unsigned int n);struct Cyc_Lineno_Pos{struct _dynforward_ptr
logical_file;struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct Cyc_Position_Segment*Cyc_Position_segment_join(struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*);struct _dynforward_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dynforward_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};
struct Cyc_Position_Error*Cyc_Position_mk_err_parse(struct Cyc_Position_Segment*,
struct _dynforward_ptr);extern char Cyc_Position_Nocontext[14];void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];extern void*Cyc_Absyn_rel_ns_null;
int Cyc_Absyn_is_qvar_qualified(struct _tuple1*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);
struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_one_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dynforward_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_char_typ;extern
void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_float_typ;
void*Cyc_Absyn_double_typ(int);void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct
Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_array_typ(
void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*
zero_term,struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct
Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct
_dynforward_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dynforward_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dynforward_ptr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*
Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct
Cyc_List_List*fs);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct
_tuple1*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(
void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int
is_xtunion,int is_flat,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(void*a);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern
char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct _tuple4{void*f1;
void*f2;};struct _tuple4*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*
d);struct _tuple4*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
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
le;};void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,
struct _dynforward_ptr ap);void*Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_compress(
void*t);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Lex_register_typedef(
struct _tuple1*s);void Cyc_Lex_enter_namespace(struct _dynforward_ptr*);void Cyc_Lex_leave_namespace();
void Cyc_Lex_enter_using(struct _tuple1*);void Cyc_Lex_leave_using();int Cyc_Parse_no_register=
0;struct Cyc_Parse_Signed_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
struct Cyc_Parse_Unsigned_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
struct Cyc_Parse_Short_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct
Cyc_Parse_Long_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Type_spec_struct{
int tag;void*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Parse_Decl_spec_struct{
int tag;struct Cyc_Absyn_Decl*f1;};struct Cyc_Parse_Declaration_spec{struct Cyc_Core_Opt*
sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int is_inline;struct Cyc_List_List*
attributes;};struct Cyc_Parse_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;
};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};struct _tuple5{
void*f1;struct Cyc_Core_Opt*f2;};static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);struct _tuple6{struct Cyc_Absyn_Tqual
f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};static struct _tuple6
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
struct Cyc_Core_Opt*Cyc_Parse_lbuf=0;static struct Cyc_List_List*Cyc_Parse_parse_result=
0;static void Cyc_Parse_err(struct _dynforward_ptr msg,struct Cyc_Position_Segment*sg){
Cyc_Position_post_error(Cyc_Position_mk_err_parse(sg,msg));}static void*Cyc_Parse_abort(
struct Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap){
Cyc_Parse_err((struct _dynforward_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),
sg);(int)_throw((void*)Cyc_Position_Exit);}static void Cyc_Parse_unimp(struct
_dynforward_ptr msg,struct Cyc_Position_Segment*sg){({struct Cyc_String_pa_struct
_tmp3;_tmp3.tag=0;_tmp3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)msg);{
struct Cyc_String_pa_struct _tmp2;_tmp2.tag=0;_tmp2.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Position_string_of_segment(sg));{void*_tmp0[2]={& _tmp2,&
_tmp3};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="%s: Warning: Cyclone does not yet support %s\n";
_tag_dynforward(_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,46));}),
_tag_dynforward(_tmp0,sizeof(void*),2));}}});return;}static int Cyc_Parse_enum_counter=
0;struct _tuple1*Cyc_Parse_gensym_enum(){return({struct _tuple1*_tmp4=_cycalloc(
sizeof(*_tmp4));_tmp4->f1=(void*)({struct Cyc_Absyn_Rel_n_struct*_tmp9=_cycalloc(
sizeof(*_tmp9));_tmp9[0]=({struct Cyc_Absyn_Rel_n_struct _tmpA;_tmpA.tag=0;_tmpA.f1=
0;_tmpA;});_tmp9;});_tmp4->f2=({struct _dynforward_ptr*_tmp5=_cycalloc(sizeof(*
_tmp5));_tmp5[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp8;_tmp8.tag=
1;_tmp8.f1=(unsigned long)Cyc_Parse_enum_counter ++;{void*_tmp6[1]={& _tmp8};Cyc_aprintf(({
const char*_tmp7="__anonymous_enum_%d__";_tag_dynforward(_tmp7,sizeof(char),
_get_zero_arr_size(_tmp7,22));}),_tag_dynforward(_tmp6,sizeof(void*),1));}});
_tmp5;});_tmp4;});}struct _tuple7{struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;void*
f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple8{struct _tuple7*
f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(
struct Cyc_Position_Segment*loc,struct _tuple8*field_info){struct _tuple8 _tmpC;
struct _tuple7*_tmpD;struct _tuple7 _tmpE;struct _tuple1*_tmpF;struct Cyc_Absyn_Tqual
_tmp10;void*_tmp11;struct Cyc_List_List*_tmp12;struct Cyc_List_List*_tmp13;struct
Cyc_Absyn_Exp*_tmp14;struct _tuple8*_tmpB=field_info;_tmpC=*_tmpB;_tmpD=_tmpC.f1;
_tmpE=*_tmpD;_tmpF=_tmpE.f1;_tmp10=_tmpE.f2;_tmp11=_tmpE.f3;_tmp12=_tmpE.f4;
_tmp13=_tmpE.f5;_tmp14=_tmpC.f2;if(_tmp12 != 0)Cyc_Parse_err(({const char*_tmp15="bad type params in struct field";
_tag_dynforward(_tmp15,sizeof(char),_get_zero_arr_size(_tmp15,32));}),loc);if(
Cyc_Absyn_is_qvar_qualified(_tmpF))Cyc_Parse_err(({const char*_tmp16="struct or union field cannot be qualified with a namespace";
_tag_dynforward(_tmp16,sizeof(char),_get_zero_arr_size(_tmp16,59));}),loc);
return({struct Cyc_Absyn_Aggrfield*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->name=(*
_tmpF).f2;_tmp17->tq=_tmp10;_tmp17->type=(void*)_tmp11;_tmp17->width=_tmp14;
_tmp17->attributes=_tmp13;_tmp17;});}static void*Cyc_Parse_type_spec(void*t,
struct Cyc_Position_Segment*loc){return(void*)({struct Cyc_Parse_Type_spec_struct*
_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18[0]=({struct Cyc_Parse_Type_spec_struct
_tmp19;_tmp19.tag=4;_tmp19.f1=(void*)t;_tmp19.f2=loc;_tmp19;});_tmp18;});}static
void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp1A=t;struct Cyc_Absyn_ArrayInfo
_tmp1B;void*_tmp1C;struct Cyc_Absyn_Tqual _tmp1D;struct Cyc_Absyn_Exp*_tmp1E;struct
Cyc_Absyn_Conref*_tmp1F;struct Cyc_Position_Segment*_tmp20;_LL1: if(_tmp1A <= (void*)
4)goto _LL3;if(*((int*)_tmp1A)!= 7)goto _LL3;_tmp1B=((struct Cyc_Absyn_ArrayType_struct*)
_tmp1A)->f1;_tmp1C=(void*)_tmp1B.elt_type;_tmp1D=_tmp1B.tq;_tmp1E=_tmp1B.num_elts;
_tmp1F=_tmp1B.zero_term;_tmp20=_tmp1B.zt_loc;_LL2: return Cyc_Absyn_starb_typ(
_tmp1C,argposn?Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp21=_cycalloc(sizeof(*
_tmp21));_tmp21->v=(void*)((void*)3);_tmp21;}),0):(void*)2,_tmp1D,_tmp1E == 0?(
void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp22=_cycalloc(sizeof(*_tmp22));
_tmp22[0]=({struct Cyc_Absyn_Upper_b_struct _tmp23;_tmp23.tag=0;_tmp23.f1=(struct
Cyc_Absyn_Exp*)_tmp1E;_tmp23;});_tmp22;}),_tmp1F);_LL3:;_LL4: return t;_LL0:;}
static void Cyc_Parse_arg_array2ptr(struct _tuple2*x){(*x).f3=Cyc_Parse_array2ptr((*
x).f3,1);}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static struct _tuple9*
Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,struct _tuple2*t){return({
struct _tuple9*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->f1=(*t).f2;_tmp24->f2=(*
t).f3;_tmp24;});}static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*
d){void*_tmp25=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp26;_LL6: if(_tmp25 <= (void*)
2)goto _LL8;if(*((int*)_tmp25)!= 0)goto _LL8;_tmp26=((struct Cyc_Absyn_Var_d_struct*)
_tmp25)->f1;_LL7: return _tmp26;_LL8:;_LL9:({void*_tmp27[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(d->loc,({
const char*_tmp28="bad declaration in `forarray' statement";_tag_dynforward(
_tmp28,sizeof(char),_get_zero_arr_size(_tmp28,40));}),_tag_dynforward(_tmp27,
sizeof(void*),0));});_LL5:;}static int Cyc_Parse_is_typeparam(void*tm){void*_tmp29=
tm;_LLB: if(*((int*)_tmp29)!= 4)goto _LLD;_LLC: return 1;_LLD:;_LLE: return 0;_LLA:;}
static void*Cyc_Parse_id2type(struct _dynforward_ptr s,void*k){if(Cyc_zstrcmp((
struct _dynforward_ptr)s,({const char*_tmp2A="`H";_tag_dynforward(_tmp2A,sizeof(
char),_get_zero_arr_size(_tmp2A,3));}))== 0)return(void*)2;else{if(Cyc_zstrcmp((
struct _dynforward_ptr)s,({const char*_tmp2B="`U";_tag_dynforward(_tmp2B,sizeof(
char),_get_zero_arr_size(_tmp2B,3));}))== 0)return(void*)3;else{return(void*)({
struct Cyc_Absyn_VarType_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C[0]=({
struct Cyc_Absyn_VarType_struct _tmp2D;_tmp2D.tag=1;_tmp2D.f1=({struct Cyc_Absyn_Tvar*
_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->name=({struct _dynforward_ptr*_tmp2F=
_cycalloc(sizeof(*_tmp2F));_tmp2F[0]=s;_tmp2F;});_tmp2E->identity=0;_tmp2E->kind=(
void*)k;_tmp2E;});_tmp2D;});_tmp2C;});}}}static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(
struct Cyc_Absyn_Tvar*t){void*k;{void*_tmp30=Cyc_Absyn_compress_kb((void*)t->kind);
void*_tmp31;void*_tmp32;_LL10: if(*((int*)_tmp30)!= 0)goto _LL12;_tmp31=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp30)->f1;_LL11: k=(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp34;
_tmp34.tag=0;_tmp34.f1=(void*)_tmp31;_tmp34;});_tmp33;});goto _LLF;_LL12: if(*((
int*)_tmp30)!= 1)goto _LL14;_LL13: k=(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp36;_tmp36.tag=1;_tmp36.f1=0;_tmp36;});_tmp35;});goto _LLF;_LL14: if(*((int*)
_tmp30)!= 2)goto _LLF;_tmp32=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp30)->f2;
_LL15: k=(void*)({struct Cyc_Absyn_Less_kb_struct*_tmp37=_cycalloc(sizeof(*_tmp37));
_tmp37[0]=({struct Cyc_Absyn_Less_kb_struct _tmp38;_tmp38.tag=2;_tmp38.f1=0;_tmp38.f2=(
void*)_tmp32;_tmp38;});_tmp37;});goto _LLF;_LLF:;}return({struct Cyc_Absyn_Tvar*
_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->name=t->name;_tmp39->identity=0;_tmp39->kind=(
void*)k;_tmp39;});}static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*
loc,void*t){void*_tmp3A=t;struct Cyc_Absyn_Tvar*_tmp3B;_LL17: if(_tmp3A <= (void*)4)
goto _LL19;if(*((int*)_tmp3A)!= 1)goto _LL19;_tmp3B=((struct Cyc_Absyn_VarType_struct*)
_tmp3A)->f1;_LL18: return _tmp3B;_LL19:;_LL1A:({void*_tmp3C[0]={};((int(*)(struct
Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(
loc,({const char*_tmp3D="expecting a list of type variables, not types";
_tag_dynforward(_tmp3D,sizeof(char),_get_zero_arr_size(_tmp3D,46));}),
_tag_dynforward(_tmp3C,sizeof(void*),0));});_LL16:;}static void*Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar*pr){return(void*)({struct Cyc_Absyn_VarType_struct*_tmp3E=
_cycalloc(sizeof(*_tmp3E));_tmp3E[0]=({struct Cyc_Absyn_VarType_struct _tmp3F;
_tmp3F.tag=1;_tmp3F.f1=pr;_tmp3F;});_tmp3E;});}static void Cyc_Parse_set_vartyp_kind(
void*t,void*k){void*_tmp40=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp41;
struct Cyc_Absyn_Tvar _tmp42;void*_tmp43;void**_tmp44;_LL1C: if(_tmp40 <= (void*)4)
goto _LL1E;if(*((int*)_tmp40)!= 1)goto _LL1E;_tmp41=((struct Cyc_Absyn_VarType_struct*)
_tmp40)->f1;_tmp42=*_tmp41;_tmp43=(void*)_tmp42.kind;_tmp44=(void**)&(*((struct
Cyc_Absyn_VarType_struct*)_tmp40)->f1).kind;_LL1D: {void*_tmp45=Cyc_Absyn_compress_kb(*
_tmp44);_LL21: if(*((int*)_tmp45)!= 1)goto _LL23;_LL22:*_tmp44=(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp47;
_tmp47.tag=0;_tmp47.f1=(void*)k;_tmp47;});_tmp46;});return;_LL23:;_LL24: return;
_LL20:;}_LL1E:;_LL1F: return;_LL1B:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp48=(void*)tms->hd;void*_tmp49;
_LL26: if(*((int*)_tmp48)!= 3)goto _LL28;_tmp49=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp48)->f1;_LL27: if(tms->tl == 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 
0){void*_tmp4A=_tmp49;struct Cyc_List_List*_tmp4B;_LL2B: if(*((int*)_tmp4A)!= 1)
goto _LL2D;_LL2C:({void*_tmp4C[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp4D="function declaration with both new- and old-style parameter declarations; ignoring old-style";
_tag_dynforward(_tmp4D,sizeof(char),_get_zero_arr_size(_tmp4D,93));}),
_tag_dynforward(_tmp4C,sizeof(void*),0));});return tms;_LL2D: if(*((int*)_tmp4A)!= 
0)goto _LL2A;_tmp4B=((struct Cyc_Absyn_NoTypes_struct*)_tmp4A)->f1;_LL2E: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp4B)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(tds))({void*_tmp4E[0]={};((int(*)(struct Cyc_Position_Segment*sg,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({const
char*_tmp4F="wrong number of parameter declarations in old-style function declaration";
_tag_dynforward(_tmp4F,sizeof(char),_get_zero_arr_size(_tmp4F,73));}),
_tag_dynforward(_tmp4E,sizeof(void*),0));});{struct Cyc_List_List*_tmp50=0;for(0;
_tmp4B != 0;_tmp4B=_tmp4B->tl){struct Cyc_List_List*_tmp51=tds;for(0;_tmp51 != 0;
_tmp51=_tmp51->tl){struct Cyc_Absyn_Decl*_tmp52=(struct Cyc_Absyn_Decl*)_tmp51->hd;
void*_tmp53=(void*)_tmp52->r;struct Cyc_Absyn_Vardecl*_tmp54;_LL30: if(_tmp53 <= (
void*)2)goto _LL32;if(*((int*)_tmp53)!= 0)goto _LL32;_tmp54=((struct Cyc_Absyn_Var_d_struct*)
_tmp53)->f1;_LL31: if(Cyc_zstrptrcmp((*_tmp54->name).f2,(struct _dynforward_ptr*)
_tmp4B->hd)!= 0)continue;if(_tmp54->initializer != 0)({void*_tmp55[0]={};((int(*)(
struct Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Parse_abort)(_tmp52->loc,({const char*_tmp56="initializer found in parameter declaration";
_tag_dynforward(_tmp56,sizeof(char),_get_zero_arr_size(_tmp56,43));}),
_tag_dynforward(_tmp55,sizeof(void*),0));});if(Cyc_Absyn_is_qvar_qualified(
_tmp54->name))({void*_tmp57[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmp52->loc,({const
char*_tmp58="namespaces forbidden in parameter declarations";_tag_dynforward(
_tmp58,sizeof(char),_get_zero_arr_size(_tmp58,47));}),_tag_dynforward(_tmp57,
sizeof(void*),0));});_tmp50=({struct Cyc_List_List*_tmp59=_cycalloc(sizeof(*
_tmp59));_tmp59->hd=({struct _tuple2*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->f1=({
struct Cyc_Core_Opt*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B->v=(*_tmp54->name).f2;
_tmp5B;});_tmp5A->f2=_tmp54->tq;_tmp5A->f3=(void*)_tmp54->type;_tmp5A;});_tmp59->tl=
_tmp50;_tmp59;});goto L;_LL32:;_LL33:({void*_tmp5C[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmp52->loc,({
const char*_tmp5D="nonvariable declaration in parameter type";_tag_dynforward(
_tmp5D,sizeof(char),_get_zero_arr_size(_tmp5D,42));}),_tag_dynforward(_tmp5C,
sizeof(void*),0));});_LL2F:;}L: if(_tmp51 == 0)({struct Cyc_String_pa_struct _tmp60;
_tmp60.tag=0;_tmp60.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct
_dynforward_ptr*)_tmp4B->hd));{void*_tmp5E[1]={& _tmp60};((int(*)(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({
const char*_tmp5F="%s is not given a type";_tag_dynforward(_tmp5F,sizeof(char),
_get_zero_arr_size(_tmp5F,23));}),_tag_dynforward(_tmp5E,sizeof(void*),1));}});}
return({struct Cyc_List_List*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp62=_cycalloc(sizeof(*_tmp62));
_tmp62[0]=({struct Cyc_Absyn_Function_mod_struct _tmp63;_tmp63.tag=3;_tmp63.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp64=_cycalloc(sizeof(*_tmp64));
_tmp64[0]=({struct Cyc_Absyn_WithTypes_struct _tmp65;_tmp65.tag=1;_tmp65.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp50);_tmp65.f2=
0;_tmp65.f3=0;_tmp65.f4=0;_tmp65.f5=0;_tmp65;});_tmp64;}));_tmp63;});_tmp62;}));
_tmp61->tl=0;_tmp61;});}_LL2A:;}goto _LL29;_LL28:;_LL29: return({struct Cyc_List_List*
_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->hd=(void*)((void*)tms->hd);_tmp66->tl=
Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);_tmp66;});_LL25:;}}struct _tuple10{
struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,
struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc){
if(tds != 0)d=({struct Cyc_Parse_Declarator*_tmp67=_cycalloc(sizeof(*_tmp67));
_tmp67->id=d->id;_tmp67->tms=Cyc_Parse_oldstyle2newstyle(d->tms,tds,loc);_tmp67;});{
void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(
0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){tss=((struct Cyc_Parse_Declaration_spec*)
dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)dso->v)->tq;is_inline=((
struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;atts=((struct Cyc_Parse_Declaration_spec*)
dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc != 0){void*
_tmp68=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc))->v;_LL35: if((int)_tmp68 != 1)goto _LL37;_LL36: sc=(void*)3;goto _LL34;
_LL37: if((int)_tmp68 != 3)goto _LL39;_LL38: sc=(void*)0;goto _LL34;_LL39:;_LL3A: Cyc_Parse_err(({
const char*_tmp69="bad storage class on function";_tag_dynforward(_tmp69,sizeof(
char),_get_zero_arr_size(_tmp69,30));}),loc);goto _LL34;_LL34:;}}{void*_tmp6B;
struct Cyc_Core_Opt*_tmp6C;struct _tuple5 _tmp6A=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmp6B=_tmp6A.f1;_tmp6C=_tmp6A.f2;{struct Cyc_Absyn_Tqual _tmp6E;void*
_tmp6F;struct Cyc_List_List*_tmp70;struct Cyc_List_List*_tmp71;struct _tuple6 _tmp6D=
Cyc_Parse_apply_tms(tq,_tmp6B,atts,d->tms);_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;
_tmp70=_tmp6D.f3;_tmp71=_tmp6D.f4;if(_tmp6C != 0)({void*_tmp72[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp73="nested type declaration within function prototype";
_tag_dynforward(_tmp73,sizeof(char),_get_zero_arr_size(_tmp73,50));}),
_tag_dynforward(_tmp72,sizeof(void*),0));});if(_tmp70 != 0)({void*_tmp74[0]={};
Cyc_Tcutil_warn(loc,({const char*_tmp75="bad type params, ignoring";
_tag_dynforward(_tmp75,sizeof(char),_get_zero_arr_size(_tmp75,26));}),
_tag_dynforward(_tmp74,sizeof(void*),0));});{void*_tmp76=_tmp6F;struct Cyc_Absyn_FnInfo
_tmp77;struct Cyc_List_List*_tmp78;struct Cyc_Core_Opt*_tmp79;void*_tmp7A;struct
Cyc_List_List*_tmp7B;int _tmp7C;struct Cyc_Absyn_VarargInfo*_tmp7D;struct Cyc_List_List*
_tmp7E;struct Cyc_List_List*_tmp7F;_LL3C: if(_tmp76 <= (void*)4)goto _LL3E;if(*((int*)
_tmp76)!= 8)goto _LL3E;_tmp77=((struct Cyc_Absyn_FnType_struct*)_tmp76)->f1;_tmp78=
_tmp77.tvars;_tmp79=_tmp77.effect;_tmp7A=(void*)_tmp77.ret_typ;_tmp7B=_tmp77.args;
_tmp7C=_tmp77.c_varargs;_tmp7D=_tmp77.cyc_varargs;_tmp7E=_tmp77.rgn_po;_tmp7F=
_tmp77.attributes;_LL3D: {struct Cyc_List_List*_tmp80=0;{struct Cyc_List_List*
_tmp81=_tmp7B;for(0;_tmp81 != 0;_tmp81=_tmp81->tl){struct _tuple2 _tmp83;struct Cyc_Core_Opt*
_tmp84;struct Cyc_Absyn_Tqual _tmp85;void*_tmp86;struct _tuple2*_tmp82=(struct
_tuple2*)_tmp81->hd;_tmp83=*_tmp82;_tmp84=_tmp83.f1;_tmp85=_tmp83.f2;_tmp86=
_tmp83.f3;if(_tmp84 == 0){Cyc_Parse_err(({const char*_tmp87="missing argument variable in function prototype";
_tag_dynforward(_tmp87,sizeof(char),_get_zero_arr_size(_tmp87,48));}),loc);
_tmp80=({struct Cyc_List_List*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->hd=({
struct _tuple10*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->f1=_init_dynforward_ptr(
_cycalloc(sizeof(struct _dynforward_ptr)),"?",sizeof(char),2);_tmp89->f2=_tmp85;
_tmp89->f3=_tmp86;_tmp89;});_tmp88->tl=_tmp80;_tmp88;});}else{_tmp80=({struct Cyc_List_List*
_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->hd=({struct _tuple10*_tmp8C=_cycalloc(
sizeof(*_tmp8C));_tmp8C->f1=(struct _dynforward_ptr*)_tmp84->v;_tmp8C->f2=_tmp85;
_tmp8C->f3=_tmp86;_tmp8C;});_tmp8B->tl=_tmp80;_tmp8B;});}}}return({struct Cyc_Absyn_Fndecl*
_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->sc=(void*)sc;_tmp8D->name=d->id;_tmp8D->tvs=
_tmp78;_tmp8D->is_inline=is_inline;_tmp8D->effect=_tmp79;_tmp8D->ret_type=(void*)
_tmp7A;_tmp8D->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp80);_tmp8D->c_varargs=_tmp7C;_tmp8D->cyc_varargs=_tmp7D;_tmp8D->rgn_po=
_tmp7E;_tmp8D->body=body;_tmp8D->cached_typ=0;_tmp8D->param_vardecls=0;_tmp8D->fn_vardecl=
0;_tmp8D->attributes=Cyc_List_append(_tmp7F,_tmp71);_tmp8D;});}_LL3E:;_LL3F:({
void*_tmp8E[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({const char*_tmp8F="declarator is not a function prototype";
_tag_dynforward(_tmp8F,sizeof(char),_get_zero_arr_size(_tmp8F,39));}),
_tag_dynforward(_tmp8E,sizeof(void*),0));});_LL3B:;}}}}}static char _tmp90[52]="at most one type may appear within a type specifier";
static struct _dynforward_ptr Cyc_Parse_msg1={_tmp90,_tmp90 + 52};static char _tmp91[
63]="const or volatile may appear only once within a type specifier";static
struct _dynforward_ptr Cyc_Parse_msg2={_tmp91,_tmp91 + 63};static char _tmp92[50]="type specifier includes more than one declaration";
static struct _dynforward_ptr Cyc_Parse_msg3={_tmp92,_tmp92 + 50};static char _tmp93[
60]="sign specifier may appear only once within a type specifier";static struct
_dynforward_ptr Cyc_Parse_msg4={_tmp93,_tmp93 + 60};static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc){struct Cyc_Core_Opt*
declopt=0;int seen_type=0;int seen_sign=0;int seen_size=0;void*t=(void*)0;void*sz=(
void*)2;void*sgn=(void*)0;struct Cyc_Position_Segment*last_loc=loc;for(0;ts != 0;
ts=ts->tl){void*_tmp94=(void*)ts->hd;void*_tmp95;struct Cyc_Position_Segment*
_tmp96;struct Cyc_Position_Segment*_tmp97;struct Cyc_Position_Segment*_tmp98;
struct Cyc_Position_Segment*_tmp99;struct Cyc_Position_Segment*_tmp9A;struct Cyc_Absyn_Decl*
_tmp9B;_LL41: if(*((int*)_tmp94)!= 4)goto _LL43;_tmp95=(void*)((struct Cyc_Parse_Type_spec_struct*)
_tmp94)->f1;_tmp96=((struct Cyc_Parse_Type_spec_struct*)_tmp94)->f2;_LL42: if(
seen_type)Cyc_Parse_err(Cyc_Parse_msg1,_tmp96);last_loc=_tmp96;seen_type=1;t=
_tmp95;goto _LL40;_LL43: if(*((int*)_tmp94)!= 0)goto _LL45;_tmp97=((struct Cyc_Parse_Signed_spec_struct*)
_tmp94)->f1;_LL44: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp97);if(seen_type)
Cyc_Parse_err(({const char*_tmp9C="signed qualifier must come before type";
_tag_dynforward(_tmp9C,sizeof(char),_get_zero_arr_size(_tmp9C,39));}),_tmp97);
last_loc=_tmp97;seen_sign=1;sgn=(void*)0;goto _LL40;_LL45: if(*((int*)_tmp94)!= 1)
goto _LL47;_tmp98=((struct Cyc_Parse_Unsigned_spec_struct*)_tmp94)->f1;_LL46: if(
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp98);if(seen_type)Cyc_Parse_err(({
const char*_tmp9D="signed qualifier must come before type";_tag_dynforward(_tmp9D,
sizeof(char),_get_zero_arr_size(_tmp9D,39));}),_tmp98);last_loc=_tmp98;seen_sign=
1;sgn=(void*)1;goto _LL40;_LL47: if(*((int*)_tmp94)!= 2)goto _LL49;_tmp99=((struct
Cyc_Parse_Short_spec_struct*)_tmp94)->f1;_LL48: if(seen_size)Cyc_Parse_err(({
const char*_tmp9E="integral size may appear only once within a type specifier";
_tag_dynforward(_tmp9E,sizeof(char),_get_zero_arr_size(_tmp9E,59));}),_tmp99);
if(seen_type)Cyc_Parse_err(({const char*_tmp9F="short modifier must come before base type";
_tag_dynforward(_tmp9F,sizeof(char),_get_zero_arr_size(_tmp9F,42));}),_tmp99);
last_loc=_tmp99;seen_size=1;sz=(void*)1;goto _LL40;_LL49: if(*((int*)_tmp94)!= 3)
goto _LL4B;_tmp9A=((struct Cyc_Parse_Long_spec_struct*)_tmp94)->f1;_LL4A: if(
seen_type)Cyc_Parse_err(({const char*_tmpA0="long modifier must come before base type";
_tag_dynforward(_tmpA0,sizeof(char),_get_zero_arr_size(_tmpA0,41));}),_tmp9A);
if(seen_size){void*_tmpA1=sz;_LL4E: if((int)_tmpA1 != 3)goto _LL50;_LL4F: sz=(void*)
4;goto _LL4D;_LL50:;_LL51: Cyc_Parse_err(({const char*_tmpA2="extra long in type specifier";
_tag_dynforward(_tmpA2,sizeof(char),_get_zero_arr_size(_tmpA2,29));}),_tmp9A);
goto _LL4D;_LL4D:;}else{sz=(void*)3;}last_loc=_tmp9A;seen_size=1;goto _LL40;_LL4B:
if(*((int*)_tmp94)!= 5)goto _LL40;_tmp9B=((struct Cyc_Parse_Decl_spec_struct*)
_tmp94)->f1;_LL4C: last_loc=_tmp9B->loc;if(declopt == 0  && !seen_type){seen_type=1;{
void*_tmpA3=(void*)_tmp9B->r;struct Cyc_Absyn_Aggrdecl*_tmpA4;struct Cyc_Absyn_Tuniondecl*
_tmpA5;struct Cyc_Absyn_Enumdecl*_tmpA6;_LL53: if(_tmpA3 <= (void*)2)goto _LL59;if(*((
int*)_tmpA3)!= 4)goto _LL55;_tmpA4=((struct Cyc_Absyn_Aggr_d_struct*)_tmpA3)->f1;
_LL54: {struct Cyc_List_List*_tmpA7=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Parse_copy_tvar,_tmpA4->tvs));t=(void*)({struct Cyc_Absyn_AggrType_struct*
_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8[0]=({struct Cyc_Absyn_AggrType_struct
_tmpA9;_tmpA9.tag=10;_tmpA9.f1=({struct Cyc_Absyn_AggrInfo _tmpAA;_tmpAA.aggr_info=(
void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmpAB=_cycalloc(sizeof(*
_tmpAB));_tmpAB[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmpAC;_tmpAC.tag=0;
_tmpAC.f1=(void*)((void*)_tmpA4->kind);_tmpAC.f2=_tmpA4->name;_tmpAC;});_tmpAB;}));
_tmpAA.targs=_tmpA7;_tmpAA;});_tmpA9;});_tmpA8;});if(_tmpA4->impl != 0)declopt=({
struct Cyc_Core_Opt*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->v=_tmp9B;_tmpAD;});
goto _LL52;}_LL55: if(*((int*)_tmpA3)!= 5)goto _LL57;_tmpA5=((struct Cyc_Absyn_Tunion_d_struct*)
_tmpA3)->f1;_LL56: {struct Cyc_List_List*_tmpAE=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmpA5->tvs));struct Cyc_Core_Opt*
ropt=_tmpA5->is_flat?0:({struct Cyc_Core_Opt*_tmpB6=_cycalloc(sizeof(*_tmpB6));
_tmpB6->v=(void*)((void*)2);_tmpB6;});t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({struct Cyc_Absyn_TunionType_struct
_tmpB0;_tmpB0.tag=2;_tmpB0.f1=({struct Cyc_Absyn_TunionInfo _tmpB1;_tmpB1.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmpB2=_cycalloc(sizeof(*
_tmpB2));_tmpB2[0]=({struct Cyc_Absyn_KnownTunion_struct _tmpB3;_tmpB3.tag=1;
_tmpB3.f1=({struct Cyc_Absyn_Tuniondecl**_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[
0]=_tmpA5;_tmpB4;});_tmpB3;});_tmpB2;}));_tmpB1.targs=_tmpAE;_tmpB1.rgn=ropt;
_tmpB1;});_tmpB0;});_tmpAF;});if(_tmpA5->fields != 0)declopt=({struct Cyc_Core_Opt*
_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->v=_tmp9B;_tmpB5;});goto _LL52;}_LL57:
if(*((int*)_tmpA3)!= 6)goto _LL59;_tmpA6=((struct Cyc_Absyn_Enum_d_struct*)_tmpA3)->f1;
_LL58: t=(void*)({struct Cyc_Absyn_EnumType_struct*_tmpB7=_cycalloc(sizeof(*_tmpB7));
_tmpB7[0]=({struct Cyc_Absyn_EnumType_struct _tmpB8;_tmpB8.tag=12;_tmpB8.f1=_tmpA6->name;
_tmpB8.f2=0;_tmpB8;});_tmpB7;});declopt=({struct Cyc_Core_Opt*_tmpB9=_cycalloc(
sizeof(*_tmpB9));_tmpB9->v=_tmp9B;_tmpB9;});goto _LL52;_LL59:;_LL5A:({void*_tmpBA[
0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Parse_abort)(_tmp9B->loc,({const char*_tmpBB="bad declaration within type specifier";
_tag_dynforward(_tmpBB,sizeof(char),_get_zero_arr_size(_tmpBB,38));}),
_tag_dynforward(_tmpBA,sizeof(void*),0));});_LL52:;}}else{Cyc_Parse_err(Cyc_Parse_msg3,
_tmp9B->loc);}goto _LL40;_LL40:;}if(!seen_type){if(!seen_sign  && !seen_size)({
void*_tmpBC[0]={};Cyc_Tcutil_warn(last_loc,({const char*_tmpBD="missing type within specifier";
_tag_dynforward(_tmpBD,sizeof(char),_get_zero_arr_size(_tmpBD,30));}),
_tag_dynforward(_tmpBC,sizeof(void*),0));});t=(void*)({struct Cyc_Absyn_IntType_struct*
_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE[0]=({struct Cyc_Absyn_IntType_struct
_tmpBF;_tmpBF.tag=5;_tmpBF.f1=(void*)sgn;_tmpBF.f2=(void*)sz;_tmpBF;});_tmpBE;});}
else{if(seen_sign){void*_tmpC0=t;void*_tmpC1;_LL5C: if(_tmpC0 <= (void*)4)goto
_LL5E;if(*((int*)_tmpC0)!= 5)goto _LL5E;_tmpC1=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpC0)->f2;_LL5D: t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpC2=_cycalloc(
sizeof(*_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_IntType_struct _tmpC3;_tmpC3.tag=5;
_tmpC3.f1=(void*)sgn;_tmpC3.f2=(void*)_tmpC1;_tmpC3;});_tmpC2;});goto _LL5B;_LL5E:;
_LL5F: Cyc_Parse_err(({const char*_tmpC4="sign specification on non-integral type";
_tag_dynforward(_tmpC4,sizeof(char),_get_zero_arr_size(_tmpC4,40));}),last_loc);
goto _LL5B;_LL5B:;}if(seen_size){void*_tmpC5=t;void*_tmpC6;_LL61: if(_tmpC5 <= (
void*)4)goto _LL65;if(*((int*)_tmpC5)!= 5)goto _LL63;_tmpC6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpC5)->f1;_LL62: t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpC7=_cycalloc(
sizeof(*_tmpC7));_tmpC7[0]=({struct Cyc_Absyn_IntType_struct _tmpC8;_tmpC8.tag=5;
_tmpC8.f1=(void*)_tmpC6;_tmpC8.f2=(void*)sz;_tmpC8;});_tmpC7;});goto _LL60;_LL63:
if(*((int*)_tmpC5)!= 6)goto _LL65;_LL64: t=(void*)({struct Cyc_Absyn_DoubleType_struct*
_tmpC9=_cycalloc_atomic(sizeof(*_tmpC9));_tmpC9[0]=({struct Cyc_Absyn_DoubleType_struct
_tmpCA;_tmpCA.tag=6;_tmpCA.f1=1;_tmpCA;});_tmpC9;});goto _LL60;_LL65:;_LL66: Cyc_Parse_err(({
const char*_tmpCB="size qualifier on non-integral type";_tag_dynforward(_tmpCB,
sizeof(char),_get_zero_arr_size(_tmpCB,36));}),last_loc);goto _LL60;_LL60:;}}
return({struct _tuple5 _tmpCC;_tmpCC.f1=t;_tmpCC.f2=declopt;_tmpCC;});}static
struct Cyc_List_List*Cyc_Parse_apply_tmss(struct Cyc_Absyn_Tqual tq,void*t,struct
Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{struct Cyc_Parse_Declarator*
_tmpCD=(struct Cyc_Parse_Declarator*)ds->hd;struct _tuple1*_tmpCE=_tmpCD->id;
struct Cyc_Absyn_Tqual _tmpD0;void*_tmpD1;struct Cyc_List_List*_tmpD2;struct Cyc_List_List*
_tmpD3;struct _tuple6 _tmpCF=Cyc_Parse_apply_tms(tq,t,shared_atts,_tmpCD->tms);
_tmpD0=_tmpCF.f1;_tmpD1=_tmpCF.f2;_tmpD2=_tmpCF.f3;_tmpD3=_tmpCF.f4;return({
struct Cyc_List_List*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->hd=({struct _tuple7*
_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5->f1=_tmpCE;_tmpD5->f2=_tmpD0;_tmpD5->f3=
_tmpD1;_tmpD5->f4=_tmpD2;_tmpD5->f5=_tmpD3;_tmpD5;});_tmpD4->tl=Cyc_Parse_apply_tmss(
_tmpD0,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts);_tmpD4;});}}static struct
_tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0)return({struct _tuple6 _tmpD6;_tmpD6.f1=
tq;_tmpD6.f2=t;_tmpD6.f3=0;_tmpD6.f4=atts;_tmpD6;});{void*_tmpD7=(void*)tms->hd;
struct Cyc_Absyn_Conref*_tmpD8;struct Cyc_Position_Segment*_tmpD9;struct Cyc_Absyn_Exp*
_tmpDA;struct Cyc_Absyn_Conref*_tmpDB;struct Cyc_Position_Segment*_tmpDC;void*
_tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_Position_Segment*_tmpDF;struct Cyc_Absyn_PtrAtts
_tmpE0;struct Cyc_Absyn_Tqual _tmpE1;struct Cyc_Position_Segment*_tmpE2;struct Cyc_List_List*
_tmpE3;_LL68: if(*((int*)_tmpD7)!= 0)goto _LL6A;_tmpD8=((struct Cyc_Absyn_Carray_mod_struct*)
_tmpD7)->f1;_tmpD9=((struct Cyc_Absyn_Carray_mod_struct*)_tmpD7)->f2;_LL69: return
Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,0,_tmpD8,
_tmpD9),atts,tms->tl);_LL6A: if(*((int*)_tmpD7)!= 1)goto _LL6C;_tmpDA=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpD7)->f1;_tmpDB=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpD7)->f2;_tmpDC=((
struct Cyc_Absyn_ConstArray_mod_struct*)_tmpD7)->f3;_LL6B: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)_tmpDA,
_tmpDB,_tmpDC),atts,tms->tl);_LL6C: if(*((int*)_tmpD7)!= 3)goto _LL6E;_tmpDD=(void*)((
struct Cyc_Absyn_Function_mod_struct*)_tmpD7)->f1;_LL6D: {void*_tmpE4=_tmpDD;
struct Cyc_List_List*_tmpE5;int _tmpE6;struct Cyc_Absyn_VarargInfo*_tmpE7;struct Cyc_Core_Opt*
_tmpE8;struct Cyc_List_List*_tmpE9;struct Cyc_Position_Segment*_tmpEA;_LL75: if(*((
int*)_tmpE4)!= 1)goto _LL77;_tmpE5=((struct Cyc_Absyn_WithTypes_struct*)_tmpE4)->f1;
_tmpE6=((struct Cyc_Absyn_WithTypes_struct*)_tmpE4)->f2;_tmpE7=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE4)->f3;_tmpE8=((struct Cyc_Absyn_WithTypes_struct*)_tmpE4)->f4;_tmpE9=((
struct Cyc_Absyn_WithTypes_struct*)_tmpE4)->f5;_LL76: {struct Cyc_List_List*
typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{struct
Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->hd=(void*)((
void*)as->hd);_tmpEB->tl=fn_atts;_tmpEB;});else{new_atts=({struct Cyc_List_List*
_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->hd=(void*)((void*)as->hd);_tmpEC->tl=
new_atts;_tmpEC;});}}}if(tms->tl != 0){void*_tmpED=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;struct Cyc_List_List*_tmpEE;_LL7A: if(*((int*)_tmpED)!= 
4)goto _LL7C;_tmpEE=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpED)->f1;_LL7B:
typvars=_tmpEE;tms=tms->tl;goto _LL79;_LL7C:;_LL7D: goto _LL79;_LL79:;}if(((((!
_tmpE6  && _tmpE7 == 0) && _tmpE5 != 0) && _tmpE5->tl == 0) && (*((struct _tuple2*)
_tmpE5->hd)).f1 == 0) && (*((struct _tuple2*)_tmpE5->hd)).f3 == (void*)0)_tmpE5=0;t=
Cyc_Parse_array2ptr(t,0);((void(*)(void(*f)(struct _tuple2*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,_tmpE5);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(
tq.loc),Cyc_Absyn_function_typ(typvars,_tmpE8,t,_tmpE5,_tmpE6,_tmpE7,_tmpE9,
fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);}_LL77: if(*((int*)
_tmpE4)!= 0)goto _LL74;_tmpEA=((struct Cyc_Absyn_NoTypes_struct*)_tmpE4)->f2;_LL78:({
void*_tmpEF[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmpEA,({const char*_tmpF0="function declaration without parameter types";
_tag_dynforward(_tmpF0,sizeof(char),_get_zero_arr_size(_tmpF0,45));}),
_tag_dynforward(_tmpEF,sizeof(void*),0));});_LL74:;}_LL6E: if(*((int*)_tmpD7)!= 4)
goto _LL70;_tmpDE=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpD7)->f1;_tmpDF=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpD7)->f2;_LL6F: if(tms->tl == 0)return({
struct _tuple6 _tmpF1;_tmpF1.f1=tq;_tmpF1.f2=t;_tmpF1.f3=_tmpDE;_tmpF1.f4=atts;
_tmpF1;});({void*_tmpF2[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmpDF,({const char*
_tmpF3="type parameters must appear before function arguments in declarator";
_tag_dynforward(_tmpF3,sizeof(char),_get_zero_arr_size(_tmpF3,68));}),
_tag_dynforward(_tmpF2,sizeof(void*),0));});_LL70: if(*((int*)_tmpD7)!= 2)goto
_LL72;_tmpE0=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpD7)->f1;_tmpE1=((struct
Cyc_Absyn_Pointer_mod_struct*)_tmpD7)->f2;_LL71: return Cyc_Parse_apply_tms(_tmpE1,(
void*)({struct Cyc_Absyn_PointerType_struct*_tmpF4=_cycalloc(sizeof(*_tmpF4));
_tmpF4[0]=({struct Cyc_Absyn_PointerType_struct _tmpF5;_tmpF5.tag=4;_tmpF5.f1=({
struct Cyc_Absyn_PtrInfo _tmpF6;_tmpF6.elt_typ=(void*)t;_tmpF6.elt_tq=tq;_tmpF6.ptr_atts=
_tmpE0;_tmpF6;});_tmpF5;});_tmpF4;}),atts,tms->tl);_LL72: if(*((int*)_tmpD7)!= 5)
goto _LL67;_tmpE2=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpD7)->f1;_tmpE3=((
struct Cyc_Absyn_Attributes_mod_struct*)_tmpD7)->f2;_LL73: return Cyc_Parse_apply_tms(
tq,t,Cyc_List_append(atts,_tmpE3),tms->tl);_LL67:;}}void*Cyc_Parse_speclist2typ(
struct Cyc_List_List*tss,struct Cyc_Position_Segment*loc){void*_tmpF8;struct Cyc_Core_Opt*
_tmpF9;struct _tuple5 _tmpF7=Cyc_Parse_collapse_type_specifiers(tss,loc);_tmpF8=
_tmpF7.f1;_tmpF9=_tmpF7.f2;if(_tmpF9 != 0)({void*_tmpFA[0]={};Cyc_Tcutil_warn(loc,({
const char*_tmpFB="ignoring nested type declaration(s) in specifier list";
_tag_dynforward(_tmpFB,sizeof(char),_get_zero_arr_size(_tmpFB,54));}),
_tag_dynforward(_tmpFA,sizeof(void*),0));});return _tmpF8;}static struct Cyc_Absyn_Decl*
Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*loc,struct _tuple7*t){
struct _tuple7 _tmpFD;struct _tuple1*_tmpFE;struct Cyc_Absyn_Tqual _tmpFF;void*
_tmp100;struct Cyc_List_List*_tmp101;struct Cyc_List_List*_tmp102;struct _tuple7*
_tmpFC=t;_tmpFD=*_tmpFC;_tmpFE=_tmpFD.f1;_tmpFF=_tmpFD.f2;_tmp100=_tmpFD.f3;
_tmp101=_tmpFD.f4;_tmp102=_tmpFD.f5;Cyc_Lex_register_typedef(_tmpFE);{struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*type;{void*_tmp103=_tmp100;struct Cyc_Core_Opt*_tmp104;
_LL7F: if(_tmp103 <= (void*)4)goto _LL81;if(*((int*)_tmp103)!= 0)goto _LL81;_tmp104=((
struct Cyc_Absyn_Evar_struct*)_tmp103)->f1;_LL80: type=0;if(_tmp104 == 0)kind=({
struct Cyc_Core_Opt*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->v=(void*)((void*)
2);_tmp105;});else{kind=_tmp104;}goto _LL7E;_LL81:;_LL82: kind=0;type=({struct Cyc_Core_Opt*
_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->v=(void*)_tmp100;_tmp106;});goto
_LL7E;_LL7E:;}return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_struct*
_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107[0]=({struct Cyc_Absyn_Typedef_d_struct
_tmp108;_tmp108.tag=7;_tmp108.f1=({struct Cyc_Absyn_Typedefdecl*_tmp109=_cycalloc(
sizeof(*_tmp109));_tmp109->name=_tmpFE;_tmp109->tvs=_tmp101;_tmp109->kind=kind;
_tmp109->defn=type;_tmp109->atts=_tmp102;_tmp109->tq=_tmpFF;_tmp109;});_tmp108;});
_tmp107;}),loc);}}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*
d,struct Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp10B;_tmp10B.tag=11;_tmp10B.f1=d;_tmp10B.f2=s;_tmp10B;});_tmp10A;}),Cyc_Position_segment_join(
d->loc,s->loc));}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*
x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*
loc){struct Cyc_Parse_Declaration_spec _tmp10D;struct Cyc_Absyn_Tqual _tmp10E;struct
Cyc_List_List*_tmp10F;struct Cyc_List_List*_tmp110;struct Cyc_Parse_Declaration_spec*
_tmp10C=ds;_tmp10D=*_tmp10C;_tmp10E=_tmp10D.tq;_tmp10F=_tmp10D.type_specs;
_tmp110=_tmp10D.attributes;if(_tmp10E.loc == 0)_tmp10E.loc=tqual_loc;if(ds->is_inline)
Cyc_Parse_err(({const char*_tmp111="inline is allowed only on function definitions";
_tag_dynforward(_tmp111,sizeof(char),_get_zero_arr_size(_tmp111,47));}),loc);if(
_tmp10F == 0){Cyc_Parse_err(({const char*_tmp112="missing type specifiers in declaration";
_tag_dynforward(_tmp112,sizeof(char),_get_zero_arr_size(_tmp112,39));}),loc);
return 0;}{void*s=(void*)2;int istypedef=0;if(ds->sc != 0){void*_tmp113=(void*)((
struct Cyc_Core_Opt*)_check_null(ds->sc))->v;_LL84: if((int)_tmp113 != 0)goto _LL86;
_LL85: istypedef=1;goto _LL83;_LL86: if((int)_tmp113 != 1)goto _LL88;_LL87: s=(void*)3;
goto _LL83;_LL88: if((int)_tmp113 != 2)goto _LL8A;_LL89: s=(void*)4;goto _LL83;_LL8A:
if((int)_tmp113 != 3)goto _LL8C;_LL8B: s=(void*)0;goto _LL83;_LL8C: if((int)_tmp113 != 
4)goto _LL8E;_LL8D: s=(void*)2;goto _LL83;_LL8E: if((int)_tmp113 != 5)goto _LL90;_LL8F:
if(Cyc_Parse_no_register)s=(void*)2;else{s=(void*)5;}goto _LL83;_LL90: if((int)
_tmp113 != 6)goto _LL83;_LL91: s=(void*)1;goto _LL83;_LL83:;}{struct Cyc_List_List*
_tmp115;struct Cyc_List_List*_tmp116;struct _tuple0 _tmp114=((struct _tuple0(*)(
struct Cyc_List_List*x))Cyc_List_split)(ids);_tmp115=_tmp114.f1;_tmp116=_tmp114.f2;{
int exps_empty=1;{struct Cyc_List_List*es=_tmp116;for(0;es != 0;es=es->tl){if((
struct Cyc_Absyn_Exp*)es->hd != 0){exps_empty=0;break;}}}{struct _tuple5 _tmp117=Cyc_Parse_collapse_type_specifiers(
_tmp10F,loc);if(_tmp115 == 0){void*_tmp119;struct Cyc_Core_Opt*_tmp11A;struct
_tuple5 _tmp118=_tmp117;_tmp119=_tmp118.f1;_tmp11A=_tmp118.f2;if(_tmp11A != 0){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp11A->v;{void*_tmp11B=(void*)d->r;
struct Cyc_Absyn_Enumdecl*_tmp11C;struct Cyc_Absyn_Aggrdecl*_tmp11D;struct Cyc_Absyn_Tuniondecl*
_tmp11E;_LL93: if(_tmp11B <= (void*)2)goto _LL99;if(*((int*)_tmp11B)!= 6)goto _LL95;
_tmp11C=((struct Cyc_Absyn_Enum_d_struct*)_tmp11B)->f1;_LL94:(void*)(_tmp11C->sc=(
void*)s);if(_tmp110 != 0)Cyc_Parse_err(({const char*_tmp11F="bad attributes on enum";
_tag_dynforward(_tmp11F,sizeof(char),_get_zero_arr_size(_tmp11F,23));}),loc);
goto _LL92;_LL95: if(*((int*)_tmp11B)!= 4)goto _LL97;_tmp11D=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp11B)->f1;_LL96:(void*)(_tmp11D->sc=(void*)s);_tmp11D->attributes=_tmp110;
goto _LL92;_LL97: if(*((int*)_tmp11B)!= 5)goto _LL99;_tmp11E=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp11B)->f1;_LL98:(void*)(_tmp11E->sc=(void*)s);if(_tmp110 != 0)Cyc_Parse_err(({
const char*_tmp120="bad attributes on tunion";_tag_dynforward(_tmp120,sizeof(char),
_get_zero_arr_size(_tmp120,25));}),loc);goto _LL92;_LL99:;_LL9A: Cyc_Parse_err(({
const char*_tmp121="bad declaration";_tag_dynforward(_tmp121,sizeof(char),
_get_zero_arr_size(_tmp121,16));}),loc);return 0;_LL92:;}return({struct Cyc_List_List*
_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->hd=d;_tmp122->tl=0;_tmp122;});}
else{void*_tmp123=_tmp119;struct Cyc_Absyn_AggrInfo _tmp124;void*_tmp125;void*
_tmp126;struct _tuple1*_tmp127;struct Cyc_List_List*_tmp128;struct Cyc_Absyn_TunionInfo
_tmp129;void*_tmp12A;struct Cyc_Absyn_Tuniondecl**_tmp12B;struct Cyc_Absyn_TunionInfo
_tmp12C;void*_tmp12D;struct Cyc_Absyn_UnknownTunionInfo _tmp12E;struct _tuple1*
_tmp12F;int _tmp130;int _tmp131;struct Cyc_List_List*_tmp132;struct _tuple1*_tmp133;
struct Cyc_List_List*_tmp134;_LL9C: if(_tmp123 <= (void*)4)goto _LLA6;if(*((int*)
_tmp123)!= 10)goto _LL9E;_tmp124=((struct Cyc_Absyn_AggrType_struct*)_tmp123)->f1;
_tmp125=(void*)_tmp124.aggr_info;if(*((int*)_tmp125)!= 0)goto _LL9E;_tmp126=(void*)((
struct Cyc_Absyn_UnknownAggr_struct*)_tmp125)->f1;_tmp127=((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp125)->f2;_tmp128=_tmp124.targs;_LL9D: {struct Cyc_List_List*_tmp135=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp128);struct Cyc_Absyn_Aggrdecl*_tmp136=({struct Cyc_Absyn_Aggrdecl*_tmp13B=
_cycalloc(sizeof(*_tmp13B));_tmp13B->kind=(void*)_tmp126;_tmp13B->sc=(void*)s;
_tmp13B->name=_tmp127;_tmp13B->tvs=_tmp135;_tmp13B->impl=0;_tmp13B->attributes=0;
_tmp13B;});if(_tmp110 != 0)Cyc_Parse_err(({const char*_tmp137="bad attributes on type declaration";
_tag_dynforward(_tmp137,sizeof(char),_get_zero_arr_size(_tmp137,35));}),loc);
return({struct Cyc_List_List*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp139=_cycalloc(sizeof(*_tmp139));
_tmp139[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp13A;_tmp13A.tag=4;_tmp13A.f1=
_tmp136;_tmp13A;});_tmp139;}),loc);_tmp138->tl=0;_tmp138;});}_LL9E: if(*((int*)
_tmp123)!= 2)goto _LLA0;_tmp129=((struct Cyc_Absyn_TunionType_struct*)_tmp123)->f1;
_tmp12A=(void*)_tmp129.tunion_info;if(*((int*)_tmp12A)!= 1)goto _LLA0;_tmp12B=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp12A)->f1;_LL9F: if(_tmp110 != 0)Cyc_Parse_err(({
const char*_tmp13C="bad attributes on tunion";_tag_dynforward(_tmp13C,sizeof(char),
_get_zero_arr_size(_tmp13C,25));}),loc);return({struct Cyc_List_List*_tmp13D=
_cycalloc(sizeof(*_tmp13D));_tmp13D->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*
_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E[0]=({struct Cyc_Absyn_Tunion_d_struct
_tmp13F;_tmp13F.tag=5;_tmp13F.f1=*_tmp12B;_tmp13F;});_tmp13E;}),loc);_tmp13D->tl=
0;_tmp13D;});_LLA0: if(*((int*)_tmp123)!= 2)goto _LLA2;_tmp12C=((struct Cyc_Absyn_TunionType_struct*)
_tmp123)->f1;_tmp12D=(void*)_tmp12C.tunion_info;if(*((int*)_tmp12D)!= 0)goto
_LLA2;_tmp12E=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp12D)->f1;_tmp12F=
_tmp12E.name;_tmp130=_tmp12E.is_xtunion;_tmp131=_tmp12E.is_flat;_tmp132=_tmp12C.targs;
_LLA1: {struct Cyc_List_List*_tmp140=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp132);struct Cyc_Absyn_Decl*_tmp141=
Cyc_Absyn_tunion_decl(s,_tmp12F,_tmp140,0,_tmp130,_tmp131,loc);if(_tmp110 != 0)
Cyc_Parse_err(({const char*_tmp142="bad attributes on tunion";_tag_dynforward(
_tmp142,sizeof(char),_get_zero_arr_size(_tmp142,25));}),loc);return({struct Cyc_List_List*
_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->hd=_tmp141;_tmp143->tl=0;_tmp143;});}
_LLA2: if(*((int*)_tmp123)!= 12)goto _LLA4;_tmp133=((struct Cyc_Absyn_EnumType_struct*)
_tmp123)->f1;_LLA3: {struct Cyc_Absyn_Enumdecl*_tmp144=({struct Cyc_Absyn_Enumdecl*
_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->sc=(void*)s;_tmp14A->name=_tmp133;
_tmp14A->fields=0;_tmp14A;});if(_tmp110 != 0)Cyc_Parse_err(({const char*_tmp145="bad attributes on enum";
_tag_dynforward(_tmp145,sizeof(char),_get_zero_arr_size(_tmp145,23));}),loc);
return({struct Cyc_List_List*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->hd=({
struct Cyc_Absyn_Decl*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147->r=(void*)((
void*)({struct Cyc_Absyn_Enum_d_struct*_tmp148=_cycalloc(sizeof(*_tmp148));
_tmp148[0]=({struct Cyc_Absyn_Enum_d_struct _tmp149;_tmp149.tag=6;_tmp149.f1=
_tmp144;_tmp149;});_tmp148;}));_tmp147->loc=loc;_tmp147;});_tmp146->tl=0;_tmp146;});}
_LLA4: if(*((int*)_tmp123)!= 13)goto _LLA6;_tmp134=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp123)->f1;_LLA5: {struct Cyc_Absyn_Enumdecl*_tmp14B=({struct Cyc_Absyn_Enumdecl*
_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151->sc=(void*)s;_tmp151->name=Cyc_Parse_gensym_enum();
_tmp151->fields=({struct Cyc_Core_Opt*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152->v=
_tmp134;_tmp152;});_tmp151;});if(_tmp110 != 0)Cyc_Parse_err(({const char*_tmp14C="bad attributes on enum";
_tag_dynforward(_tmp14C,sizeof(char),_get_zero_arr_size(_tmp14C,23));}),loc);
return({struct Cyc_List_List*_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D->hd=({
struct Cyc_Absyn_Decl*_tmp14E=_cycalloc(sizeof(*_tmp14E));_tmp14E->r=(void*)((
void*)({struct Cyc_Absyn_Enum_d_struct*_tmp14F=_cycalloc(sizeof(*_tmp14F));
_tmp14F[0]=({struct Cyc_Absyn_Enum_d_struct _tmp150;_tmp150.tag=6;_tmp150.f1=
_tmp14B;_tmp150;});_tmp14F;}));_tmp14E->loc=loc;_tmp14E;});_tmp14D->tl=0;_tmp14D;});}
_LLA6:;_LLA7: Cyc_Parse_err(({const char*_tmp153="missing declarator";
_tag_dynforward(_tmp153,sizeof(char),_get_zero_arr_size(_tmp153,19));}),loc);
return 0;_LL9B:;}}else{void*t=_tmp117.f1;struct Cyc_List_List*_tmp154=Cyc_Parse_apply_tmss(
_tmp10E,t,_tmp115,_tmp110);if(istypedef){if(!exps_empty)Cyc_Parse_err(({const
char*_tmp155="initializer in typedef declaration";_tag_dynforward(_tmp155,
sizeof(char),_get_zero_arr_size(_tmp155,35));}),loc);{struct Cyc_List_List*decls=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,
struct _tuple7*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_v_typ_to_typedef,loc,_tmp154);if(_tmp117.f2 != 0){struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)(_tmp117.f2)->v;{void*_tmp156=(void*)d->r;struct Cyc_Absyn_Aggrdecl*
_tmp157;struct Cyc_Absyn_Tuniondecl*_tmp158;struct Cyc_Absyn_Enumdecl*_tmp159;
_LLA9: if(_tmp156 <= (void*)2)goto _LLAF;if(*((int*)_tmp156)!= 4)goto _LLAB;_tmp157=((
struct Cyc_Absyn_Aggr_d_struct*)_tmp156)->f1;_LLAA:(void*)(_tmp157->sc=(void*)s);
_tmp157->attributes=_tmp110;_tmp110=0;goto _LLA8;_LLAB: if(*((int*)_tmp156)!= 5)
goto _LLAD;_tmp158=((struct Cyc_Absyn_Tunion_d_struct*)_tmp156)->f1;_LLAC:(void*)(
_tmp158->sc=(void*)s);goto _LLA8;_LLAD: if(*((int*)_tmp156)!= 6)goto _LLAF;_tmp159=((
struct Cyc_Absyn_Enum_d_struct*)_tmp156)->f1;_LLAE:(void*)(_tmp159->sc=(void*)s);
goto _LLA8;_LLAF:;_LLB0: Cyc_Parse_err(({const char*_tmp15A="declaration within typedef is not a struct, union, tunion, or xtunion";
_tag_dynforward(_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,70));}),loc);
goto _LLA8;_LLA8:;}decls=({struct Cyc_List_List*_tmp15B=_cycalloc(sizeof(*_tmp15B));
_tmp15B->hd=d;_tmp15B->tl=decls;_tmp15B;});}return decls;}}else{if(_tmp117.f2 != 0)
Cyc_Parse_unimp(({const char*_tmp15C="nested type declaration within declarator";
_tag_dynforward(_tmp15C,sizeof(char),_get_zero_arr_size(_tmp15C,42));}),loc);{
struct Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp15D=_tmp154;for(0;_tmp15D != 
0;(_tmp15D=_tmp15D->tl,_tmp116=_tmp116->tl)){struct _tuple7 _tmp15F;struct _tuple1*
_tmp160;struct Cyc_Absyn_Tqual _tmp161;void*_tmp162;struct Cyc_List_List*_tmp163;
struct Cyc_List_List*_tmp164;struct _tuple7*_tmp15E=(struct _tuple7*)_tmp15D->hd;
_tmp15F=*_tmp15E;_tmp160=_tmp15F.f1;_tmp161=_tmp15F.f2;_tmp162=_tmp15F.f3;
_tmp163=_tmp15F.f4;_tmp164=_tmp15F.f5;if(_tmp163 != 0)({void*_tmp165[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp166="bad type params, ignoring";_tag_dynforward(_tmp166,
sizeof(char),_get_zero_arr_size(_tmp166,26));}),_tag_dynforward(_tmp165,sizeof(
void*),0));});if(_tmp116 == 0)({void*_tmp167[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({
const char*_tmp168="unexpected NULL in parse!";_tag_dynforward(_tmp168,sizeof(
char),_get_zero_arr_size(_tmp168,26));}),_tag_dynforward(_tmp167,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*_tmp169=(struct Cyc_Absyn_Exp*)_tmp116->hd;struct Cyc_Absyn_Vardecl*
_tmp16A=Cyc_Absyn_new_vardecl(_tmp160,_tmp162,_tmp169);_tmp16A->tq=_tmp161;(void*)(
_tmp16A->sc=(void*)s);_tmp16A->attributes=_tmp164;{struct Cyc_Absyn_Decl*_tmp16B=({
struct Cyc_Absyn_Decl*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E[
0]=({struct Cyc_Absyn_Var_d_struct _tmp16F;_tmp16F.tag=0;_tmp16F.f1=_tmp16A;
_tmp16F;});_tmp16E;}));_tmp16D->loc=loc;_tmp16D;});decls=({struct Cyc_List_List*
_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C->hd=_tmp16B;_tmp16C->tl=decls;
_tmp16C;});}}}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
decls);}}}}}}}}static void*Cyc_Parse_id_to_kind(struct _dynforward_ptr s,struct Cyc_Position_Segment*
loc){if(Cyc_strlen((struct _dynforward_ptr)s)== 1)switch(*((const char*)
_check_dynforward_subscript(s,sizeof(char),0))){case 'A': _LLB1: return(void*)0;
case 'M': _LLB2: return(void*)1;case 'B': _LLB3: return(void*)2;case 'R': _LLB4: return(
void*)3;case 'E': _LLB5: return(void*)4;case 'I': _LLB6: return(void*)5;default: _LLB7:
break;}Cyc_Parse_err((struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp172;
_tmp172.tag=0;_tmp172.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)s);{void*
_tmp170[1]={& _tmp172};Cyc_aprintf(({const char*_tmp171="bad kind: %s";
_tag_dynforward(_tmp171,sizeof(char),_get_zero_arr_size(_tmp171,13));}),
_tag_dynforward(_tmp170,sizeof(void*),1));}}),loc);return(void*)2;}static struct
Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){void*_tmp173=(void*)p->r;
struct _tuple1*_tmp174;struct Cyc_Absyn_Vardecl*_tmp175;struct Cyc_Absyn_Pat*
_tmp176;struct Cyc_Absyn_Pat _tmp177;void*_tmp178;struct Cyc_Absyn_Pat*_tmp179;void*
_tmp17A;int _tmp17B;char _tmp17C;struct _dynforward_ptr _tmp17D;struct _tuple1*
_tmp17E;struct Cyc_List_List*_tmp17F;int _tmp180;struct Cyc_Absyn_Exp*_tmp181;_LLBA:
if(_tmp173 <= (void*)2)goto _LLC0;if(*((int*)_tmp173)!= 12)goto _LLBC;_tmp174=((
struct Cyc_Absyn_UnknownId_p_struct*)_tmp173)->f1;_LLBB: return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp182=_cycalloc(sizeof(*_tmp182));
_tmp182[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp183;_tmp183.tag=2;_tmp183.f1=
_tmp174;_tmp183;});_tmp182;}),p->loc);_LLBC: if(*((int*)_tmp173)!= 1)goto _LLBE;
_tmp175=((struct Cyc_Absyn_Reference_p_struct*)_tmp173)->f1;_tmp176=((struct Cyc_Absyn_Reference_p_struct*)
_tmp173)->f2;_tmp177=*_tmp176;_tmp178=(void*)_tmp177.r;if((int)_tmp178 != 0)goto
_LLBE;_LLBD: return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp185;_tmp185.tag=2;_tmp185.f1=_tmp175->name;_tmp185;});_tmp184;}),p->loc),p->loc);
_LLBE: if(*((int*)_tmp173)!= 4)goto _LLC0;_tmp179=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp173)->f1;_LLBF: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp179),p->loc);
_LLC0: if((int)_tmp173 != 1)goto _LLC2;_LLC1: return Cyc_Absyn_null_exp(p->loc);_LLC2:
if(_tmp173 <= (void*)2)goto _LLCC;if(*((int*)_tmp173)!= 7)goto _LLC4;_tmp17A=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp173)->f1;_tmp17B=((struct Cyc_Absyn_Int_p_struct*)
_tmp173)->f2;_LLC3: return Cyc_Absyn_int_exp(_tmp17A,_tmp17B,p->loc);_LLC4: if(*((
int*)_tmp173)!= 8)goto _LLC6;_tmp17C=((struct Cyc_Absyn_Char_p_struct*)_tmp173)->f1;
_LLC5: return Cyc_Absyn_char_exp(_tmp17C,p->loc);_LLC6: if(*((int*)_tmp173)!= 9)
goto _LLC8;_tmp17D=((struct Cyc_Absyn_Float_p_struct*)_tmp173)->f1;_LLC7: return Cyc_Absyn_float_exp(
_tmp17D,p->loc);_LLC8: if(*((int*)_tmp173)!= 13)goto _LLCA;_tmp17E=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp173)->f1;_tmp17F=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp173)->f2;
_tmp180=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp173)->f3;if(_tmp180 != 0)goto
_LLCA;_LLC9: {struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp186=_cycalloc(sizeof(*_tmp186));_tmp186[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp187;_tmp187.tag=2;_tmp187.f1=_tmp17E;_tmp187;});_tmp186;}),p->loc);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp17F);return Cyc_Absyn_unknowncall_exp(
e1,es,p->loc);}_LLCA: if(*((int*)_tmp173)!= 14)goto _LLCC;_tmp181=((struct Cyc_Absyn_Exp_p_struct*)
_tmp173)->f1;_LLCB: return _tmp181;_LLCC:;_LLCD: Cyc_Parse_err(({const char*_tmp188="cannot mix patterns and expressions in case";
_tag_dynforward(_tmp188,sizeof(char),_get_zero_arr_size(_tmp188,44));}),p->loc);
return Cyc_Absyn_null_exp(p->loc);_LLB9:;}struct _tuple11{void*f1;int f2;};struct
Cyc_Int_tok_struct{int tag;struct _tuple11*f1;};struct Cyc_Char_tok_struct{int tag;
char f1;};struct Cyc_String_tok_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Stringopt_tok_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_QualId_tok_struct{int tag;struct _tuple1*
f1;};struct _tuple12{struct Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*f2;
struct Cyc_Absyn_Conref*f3;};struct Cyc_YY1_struct{int tag;struct _tuple12*f1;};
struct Cyc_YY2_struct{int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_YY3_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_YY4_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY5_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY6_struct{int
tag;void*f1;};struct Cyc_YY7_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY8_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_YY9_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY10_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct _tuple13{struct
Cyc_List_List*f1;int f2;};struct Cyc_YY11_struct{int tag;struct _tuple13*f1;};struct
Cyc_YY12_struct{int tag;struct Cyc_List_List*f1;};struct _tuple14{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_YY13_struct{int tag;struct _tuple14*f1;};
struct Cyc_YY14_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY15_struct{int
tag;struct _tuple13*f1;};struct Cyc_YY16_struct{int tag;struct Cyc_Absyn_Fndecl*f1;}
;struct Cyc_YY17_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY18_struct{int
tag;struct Cyc_Parse_Declaration_spec*f1;};struct _tuple15{struct Cyc_Parse_Declarator*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_YY19_struct{int tag;struct _tuple15*f1;};
struct Cyc_YY20_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY21_struct{int
tag;void*f1;};struct Cyc_YY22_struct{int tag;void*f1;};struct Cyc_YY23_struct{int
tag;void*f1;};struct Cyc_YY24_struct{int tag;struct Cyc_Absyn_Tqual f1;};struct Cyc_YY25_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY26_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY27_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY28_struct{
int tag;struct Cyc_Parse_Declarator*f1;};struct Cyc_YY29_struct{int tag;struct Cyc_Parse_Abstractdeclarator*
f1;};struct Cyc_YY30_struct{int tag;int f1;};struct Cyc_YY31_struct{int tag;void*f1;}
;struct Cyc_YY32_struct{int tag;struct Cyc_Absyn_Tunionfield*f1;};struct Cyc_YY33_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple16{struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;};struct Cyc_YY34_struct{int tag;struct _tuple16*f1;};
struct Cyc_YY35_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY36_struct{int
tag;struct _tuple2*f1;};struct Cyc_YY37_struct{int tag;struct Cyc_List_List*f1;};
struct _tuple17{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct
Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_YY38_struct{int tag;struct
_tuple17*f1;};struct Cyc_YY39_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY40_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY41_struct{int tag;void*f1;};struct Cyc_YY42_struct{
int tag;void*f1;};struct Cyc_YY43_struct{int tag;void*f1;};struct Cyc_YY44_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY45_struct{int tag;void*f1;};struct Cyc_YY46_struct{
int tag;struct Cyc_Absyn_Enumfield*f1;};struct Cyc_YY47_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY48_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY49_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY50_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_YY51_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YYINITIALSVAL_struct{
int tag;};union Cyc_YYSTYPE_union{struct Cyc_Int_tok_struct Int_tok;struct Cyc_Char_tok_struct
Char_tok;struct Cyc_String_tok_struct String_tok;struct Cyc_Stringopt_tok_struct
Stringopt_tok;struct Cyc_QualId_tok_struct QualId_tok;struct Cyc_YY1_struct YY1;
struct Cyc_YY2_struct YY2;struct Cyc_YY3_struct YY3;struct Cyc_YY4_struct YY4;struct
Cyc_YY5_struct YY5;struct Cyc_YY6_struct YY6;struct Cyc_YY7_struct YY7;struct Cyc_YY8_struct
YY8;struct Cyc_YY9_struct YY9;struct Cyc_YY10_struct YY10;struct Cyc_YY11_struct YY11;
struct Cyc_YY12_struct YY12;struct Cyc_YY13_struct YY13;struct Cyc_YY14_struct YY14;
struct Cyc_YY15_struct YY15;struct Cyc_YY16_struct YY16;struct Cyc_YY17_struct YY17;
struct Cyc_YY18_struct YY18;struct Cyc_YY19_struct YY19;struct Cyc_YY20_struct YY20;
struct Cyc_YY21_struct YY21;struct Cyc_YY22_struct YY22;struct Cyc_YY23_struct YY23;
struct Cyc_YY24_struct YY24;struct Cyc_YY25_struct YY25;struct Cyc_YY26_struct YY26;
struct Cyc_YY27_struct YY27;struct Cyc_YY28_struct YY28;struct Cyc_YY29_struct YY29;
struct Cyc_YY30_struct YY30;struct Cyc_YY31_struct YY31;struct Cyc_YY32_struct YY32;
struct Cyc_YY33_struct YY33;struct Cyc_YY34_struct YY34;struct Cyc_YY35_struct YY35;
struct Cyc_YY36_struct YY36;struct Cyc_YY37_struct YY37;struct Cyc_YY38_struct YY38;
struct Cyc_YY39_struct YY39;struct Cyc_YY40_struct YY40;struct Cyc_YY41_struct YY41;
struct Cyc_YY42_struct YY42;struct Cyc_YY43_struct YY43;struct Cyc_YY44_struct YY44;
struct Cyc_YY45_struct YY45;struct Cyc_YY46_struct YY46;struct Cyc_YY47_struct YY47;
struct Cyc_YY48_struct YY48;struct Cyc_YY49_struct YY49;struct Cyc_YY50_struct YY50;
struct Cyc_YY51_struct YY51;struct Cyc_YYINITIALSVAL_struct YYINITIALSVAL;};static
char _tmp18A[15]="$(sign_t,int)@";static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={
Cyc_Core_Failure,{_tmp18A,_tmp18A + 15}};struct _tuple11*Cyc_yyget_Int_tok(union
Cyc_YYSTYPE_union yy1){struct _tuple11*yyzzz;{union Cyc_YYSTYPE_union _tmp18B=yy1;
struct _tuple11*_tmp18C;_LLCF: if((_tmp18B.Int_tok).tag != 0)goto _LLD1;_tmp18C=(
_tmp18B.Int_tok).f1;_LLD0: yyzzz=_tmp18C;goto _LLCE;_LLD1:;_LLD2:(int)_throw((void*)&
Cyc_yyfail_Int_tok);_LLCE:;}return yyzzz;}static char _tmp18E[5]="char";static
struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp18E,
_tmp18E + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE_union yy1){char yyzzz;{union
Cyc_YYSTYPE_union _tmp18F=yy1;char _tmp190;_LLD4: if((_tmp18F.Char_tok).tag != 1)
goto _LLD6;_tmp190=(_tmp18F.Char_tok).f1;_LLD5: yyzzz=_tmp190;goto _LLD3;_LLD6:;
_LLD7:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLD3:;}return yyzzz;}static char
_tmp192[9]="string_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp192,_tmp192 + 9}};struct _dynforward_ptr Cyc_yyget_String_tok(
union Cyc_YYSTYPE_union yy1){struct _dynforward_ptr yyzzz;{union Cyc_YYSTYPE_union
_tmp193=yy1;struct _dynforward_ptr _tmp194;_LLD9: if((_tmp193.String_tok).tag != 2)
goto _LLDB;_tmp194=(_tmp193.String_tok).f1;_LLDA: yyzzz=_tmp194;goto _LLD8;_LLDB:;
_LLDC:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLD8:;}return yyzzz;}static char
_tmp196[54]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp196,_tmp196 + 54}};
struct _tuple12*Cyc_yyget_YY1(union Cyc_YYSTYPE_union yy1){struct _tuple12*yyzzz;{
union Cyc_YYSTYPE_union _tmp197=yy1;struct _tuple12*_tmp198;_LLDE: if((_tmp197.YY1).tag
!= 5)goto _LLE0;_tmp198=(_tmp197.YY1).f1;_LLDF: yyzzz=_tmp198;goto _LLDD;_LLE0:;
_LLE1:(int)_throw((void*)& Cyc_yyfail_YY1);_LLDD:;}return yyzzz;}static char _tmp19A[
19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{
_tmp19A,_tmp19A + 19}};struct Cyc_Absyn_Conref*Cyc_yyget_YY2(union Cyc_YYSTYPE_union
yy1){struct Cyc_Absyn_Conref*yyzzz;{union Cyc_YYSTYPE_union _tmp19B=yy1;struct Cyc_Absyn_Conref*
_tmp19C;_LLE3: if((_tmp19B.YY2).tag != 6)goto _LLE5;_tmp19C=(_tmp19B.YY2).f1;_LLE4:
yyzzz=_tmp19C;goto _LLE2;_LLE5:;_LLE6:(int)_throw((void*)& Cyc_yyfail_YY2);_LLE2:;}
return yyzzz;}static char _tmp19E[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={
Cyc_Core_Failure,{_tmp19E,_tmp19E + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union
Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE_union _tmp19F=
yy1;struct Cyc_Absyn_Exp*_tmp1A0;_LLE8: if((_tmp19F.YY3).tag != 7)goto _LLEA;_tmp1A0=(
_tmp19F.YY3).f1;_LLE9: yyzzz=_tmp1A0;goto _LLE7;_LLEA:;_LLEB:(int)_throw((void*)&
Cyc_yyfail_YY3);_LLE7:;}return yyzzz;}static char _tmp1A2[14]="list_t<exp_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1A2,
_tmp1A2 + 14}};struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1A3=yy1;struct Cyc_List_List*
_tmp1A4;_LLED: if((_tmp1A3.YY4).tag != 8)goto _LLEF;_tmp1A4=(_tmp1A3.YY4).f1;_LLEE:
yyzzz=_tmp1A4;goto _LLEC;_LLEF:;_LLF0:(int)_throw((void*)& Cyc_yyfail_YY4);_LLEC:;}
return yyzzz;}static char _tmp1A6[39]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1A6,
_tmp1A6 + 39}};struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1A7=yy1;struct Cyc_List_List*
_tmp1A8;_LLF2: if((_tmp1A7.YY5).tag != 9)goto _LLF4;_tmp1A8=(_tmp1A7.YY5).f1;_LLF3:
yyzzz=_tmp1A8;goto _LLF1;_LLF4:;_LLF5:(int)_throw((void*)& Cyc_yyfail_YY5);_LLF1:;}
return yyzzz;}static char _tmp1AA[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1AA,_tmp1AA + 9}};void*Cyc_yyget_YY6(union
Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1AB=yy1;void*_tmp1AC;
_LLF7: if((_tmp1AB.YY6).tag != 10)goto _LLF9;_tmp1AC=(_tmp1AB.YY6).f1;_LLF8: yyzzz=
_tmp1AC;goto _LLF6;_LLF9:;_LLFA:(int)_throw((void*)& Cyc_yyfail_YY6);_LLF6:;}
return yyzzz;}static char _tmp1AE[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1AE,_tmp1AE + 16}};struct Cyc_Core_Opt*Cyc_yyget_YY7(
union Cyc_YYSTYPE_union yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE_union
_tmp1AF=yy1;struct Cyc_Core_Opt*_tmp1B0;_LLFC: if((_tmp1AF.YY7).tag != 11)goto _LLFE;
_tmp1B0=(_tmp1AF.YY7).f1;_LLFD: yyzzz=_tmp1B0;goto _LLFB;_LLFE:;_LLFF:(int)_throw((
void*)& Cyc_yyfail_YY7);_LLFB:;}return yyzzz;}static char _tmp1B2[7]="qvar_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp1B2,
_tmp1B2 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE_union yy1){
struct _tuple1*yyzzz;{union Cyc_YYSTYPE_union _tmp1B3=yy1;struct _tuple1*_tmp1B4;
_LL101: if((_tmp1B3.QualId_tok).tag != 4)goto _LL103;_tmp1B4=(_tmp1B3.QualId_tok).f1;
_LL102: yyzzz=_tmp1B4;goto _LL100;_LL103:;_LL104:(int)_throw((void*)& Cyc_yyfail_QualId_tok);
_LL100:;}return yyzzz;}static char _tmp1B6[7]="stmt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp1B6,_tmp1B6 + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_YY8(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Stmt*yyzzz;{union Cyc_YYSTYPE_union
_tmp1B7=yy1;struct Cyc_Absyn_Stmt*_tmp1B8;_LL106: if((_tmp1B7.YY8).tag != 12)goto
_LL108;_tmp1B8=(_tmp1B7.YY8).f1;_LL107: yyzzz=_tmp1B8;goto _LL105;_LL108:;_LL109:(
int)_throw((void*)& Cyc_yyfail_YY8);_LL105:;}return yyzzz;}static char _tmp1BA[24]="list_t<switch_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp1BA,
_tmp1BA + 24}};struct Cyc_List_List*Cyc_yyget_YY9(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1BB=yy1;struct Cyc_List_List*
_tmp1BC;_LL10B: if((_tmp1BB.YY9).tag != 13)goto _LL10D;_tmp1BC=(_tmp1BB.YY9).f1;
_LL10C: yyzzz=_tmp1BC;goto _LL10A;_LL10D:;_LL10E:(int)_throw((void*)& Cyc_yyfail_YY9);
_LL10A:;}return yyzzz;}static char _tmp1BE[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp1BE,_tmp1BE + 6}};struct Cyc_Absyn_Pat*Cyc_yyget_YY10(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Pat*yyzzz;{union Cyc_YYSTYPE_union
_tmp1BF=yy1;struct Cyc_Absyn_Pat*_tmp1C0;_LL110: if((_tmp1BF.YY10).tag != 14)goto
_LL112;_tmp1C0=(_tmp1BF.YY10).f1;_LL111: yyzzz=_tmp1C0;goto _LL10F;_LL112:;_LL113:(
int)_throw((void*)& Cyc_yyfail_YY10);_LL10F:;}return yyzzz;}static char _tmp1C2[23]="$(list_t<pat_t>,bool)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp1C2,
_tmp1C2 + 23}};struct _tuple13*Cyc_yyget_YY11(union Cyc_YYSTYPE_union yy1){struct
_tuple13*yyzzz;{union Cyc_YYSTYPE_union _tmp1C3=yy1;struct _tuple13*_tmp1C4;_LL115:
if((_tmp1C3.YY11).tag != 15)goto _LL117;_tmp1C4=(_tmp1C3.YY11).f1;_LL116: yyzzz=
_tmp1C4;goto _LL114;_LL117:;_LL118:(int)_throw((void*)& Cyc_yyfail_YY11);_LL114:;}
return yyzzz;}static char _tmp1C6[14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp1C6,_tmp1C6 + 14}};struct Cyc_List_List*Cyc_yyget_YY12(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp1C7=yy1;struct Cyc_List_List*_tmp1C8;_LL11A: if((_tmp1C7.YY12).tag != 16)goto
_LL11C;_tmp1C8=(_tmp1C7.YY12).f1;_LL11B: yyzzz=_tmp1C8;goto _LL119;_LL11C:;_LL11D:(
int)_throw((void*)& Cyc_yyfail_YY12);_LL119:;}return yyzzz;}static char _tmp1CA[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp1CA,
_tmp1CA + 31}};struct _tuple14*Cyc_yyget_YY13(union Cyc_YYSTYPE_union yy1){struct
_tuple14*yyzzz;{union Cyc_YYSTYPE_union _tmp1CB=yy1;struct _tuple14*_tmp1CC;_LL11F:
if((_tmp1CB.YY13).tag != 17)goto _LL121;_tmp1CC=(_tmp1CB.YY13).f1;_LL120: yyzzz=
_tmp1CC;goto _LL11E;_LL121:;_LL122:(int)_throw((void*)& Cyc_yyfail_YY13);_LL11E:;}
return yyzzz;}static char _tmp1CE[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp1CE,
_tmp1CE + 39}};struct Cyc_List_List*Cyc_yyget_YY14(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1CF=yy1;struct Cyc_List_List*
_tmp1D0;_LL124: if((_tmp1CF.YY14).tag != 18)goto _LL126;_tmp1D0=(_tmp1CF.YY14).f1;
_LL125: yyzzz=_tmp1D0;goto _LL123;_LL126:;_LL127:(int)_throw((void*)& Cyc_yyfail_YY14);
_LL123:;}return yyzzz;}static char _tmp1D2[48]="$(list_t<$(list_t<designator_t>,pat_t)@>,bool)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp1D2,
_tmp1D2 + 48}};struct _tuple13*Cyc_yyget_YY15(union Cyc_YYSTYPE_union yy1){struct
_tuple13*yyzzz;{union Cyc_YYSTYPE_union _tmp1D3=yy1;struct _tuple13*_tmp1D4;_LL129:
if((_tmp1D3.YY15).tag != 19)goto _LL12B;_tmp1D4=(_tmp1D3.YY15).f1;_LL12A: yyzzz=
_tmp1D4;goto _LL128;_LL12B:;_LL12C:(int)_throw((void*)& Cyc_yyfail_YY15);_LL128:;}
return yyzzz;}static char _tmp1D6[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp1D6,_tmp1D6 + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_YY16(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE_union
_tmp1D7=yy1;struct Cyc_Absyn_Fndecl*_tmp1D8;_LL12E: if((_tmp1D7.YY16).tag != 20)
goto _LL130;_tmp1D8=(_tmp1D7.YY16).f1;_LL12F: yyzzz=_tmp1D8;goto _LL12D;_LL130:;
_LL131:(int)_throw((void*)& Cyc_yyfail_YY16);_LL12D:;}return yyzzz;}static char
_tmp1DA[15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY17={
Cyc_Core_Failure,{_tmp1DA,_tmp1DA + 15}};struct Cyc_List_List*Cyc_yyget_YY17(union
Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1DB=
yy1;struct Cyc_List_List*_tmp1DC;_LL133: if((_tmp1DB.YY17).tag != 21)goto _LL135;
_tmp1DC=(_tmp1DB.YY17).f1;_LL134: yyzzz=_tmp1DC;goto _LL132;_LL135:;_LL136:(int)
_throw((void*)& Cyc_yyfail_YY17);_LL132:;}return yyzzz;}static char _tmp1DE[12]="decl_spec_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp1DE,
_tmp1DE + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY18(union Cyc_YYSTYPE_union
yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE_union _tmp1DF=yy1;
struct Cyc_Parse_Declaration_spec*_tmp1E0;_LL138: if((_tmp1DF.YY18).tag != 22)goto
_LL13A;_tmp1E0=(_tmp1DF.YY18).f1;_LL139: yyzzz=_tmp1E0;goto _LL137;_LL13A:;_LL13B:(
int)_throw((void*)& Cyc_yyfail_YY18);_LL137:;}return yyzzz;}static char _tmp1E2[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp1E2,
_tmp1E2 + 27}};struct _tuple15*Cyc_yyget_YY19(union Cyc_YYSTYPE_union yy1){struct
_tuple15*yyzzz;{union Cyc_YYSTYPE_union _tmp1E3=yy1;struct _tuple15*_tmp1E4;_LL13D:
if((_tmp1E3.YY19).tag != 23)goto _LL13F;_tmp1E4=(_tmp1E3.YY19).f1;_LL13E: yyzzz=
_tmp1E4;goto _LL13C;_LL13F:;_LL140:(int)_throw((void*)& Cyc_yyfail_YY19);_LL13C:;}
return yyzzz;}static char _tmp1E6[35]="list_t<$(declarator_t,exp_opt_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp1E6,_tmp1E6 + 
35}};struct Cyc_List_List*Cyc_yyget_YY20(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp1E7=yy1;struct Cyc_List_List*_tmp1E8;_LL142: if((
_tmp1E7.YY20).tag != 24)goto _LL144;_tmp1E8=(_tmp1E7.YY20).f1;_LL143: yyzzz=_tmp1E8;
goto _LL141;_LL144:;_LL145:(int)_throw((void*)& Cyc_yyfail_YY20);_LL141:;}return
yyzzz;}static char _tmp1EA[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp1EA,_tmp1EA + 16}};void*Cyc_yyget_YY21(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1EB=yy1;void*
_tmp1EC;_LL147: if((_tmp1EB.YY21).tag != 25)goto _LL149;_tmp1EC=(_tmp1EB.YY21).f1;
_LL148: yyzzz=_tmp1EC;goto _LL146;_LL149:;_LL14A:(int)_throw((void*)& Cyc_yyfail_YY21);
_LL146:;}return yyzzz;}static char _tmp1EE[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp1EE,_tmp1EE + 17}};void*Cyc_yyget_YY22(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1EF=yy1;void*
_tmp1F0;_LL14C: if((_tmp1EF.YY22).tag != 26)goto _LL14E;_tmp1F0=(_tmp1EF.YY22).f1;
_LL14D: yyzzz=_tmp1F0;goto _LL14B;_LL14E:;_LL14F:(int)_throw((void*)& Cyc_yyfail_YY22);
_LL14B:;}return yyzzz;}static char _tmp1F2[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp1F2,_tmp1F2 + 12}};void*Cyc_yyget_YY23(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1F3=yy1;void*
_tmp1F4;_LL151: if((_tmp1F3.YY23).tag != 27)goto _LL153;_tmp1F4=(_tmp1F3.YY23).f1;
_LL152: yyzzz=_tmp1F4;goto _LL150;_LL153:;_LL154:(int)_throw((void*)& Cyc_yyfail_YY23);
_LL150:;}return yyzzz;}static char _tmp1F6[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp1F6,_tmp1F6 + 8}};struct Cyc_Absyn_Tqual Cyc_yyget_YY24(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Tqual yyzzz;{union Cyc_YYSTYPE_union
_tmp1F7=yy1;struct Cyc_Absyn_Tqual _tmp1F8;_LL156: if((_tmp1F7.YY24).tag != 28)goto
_LL158;_tmp1F8=(_tmp1F7.YY24).f1;_LL157: yyzzz=_tmp1F8;goto _LL155;_LL158:;_LL159:(
int)_throw((void*)& Cyc_yyfail_YY24);_LL155:;}return yyzzz;}static char _tmp1FA[20]="list_t<aggrfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp1FA,
_tmp1FA + 20}};struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1FB=yy1;struct Cyc_List_List*
_tmp1FC;_LL15B: if((_tmp1FB.YY25).tag != 29)goto _LL15D;_tmp1FC=(_tmp1FB.YY25).f1;
_LL15C: yyzzz=_tmp1FC;goto _LL15A;_LL15D:;_LL15E:(int)_throw((void*)& Cyc_yyfail_YY25);
_LL15A:;}return yyzzz;}static char _tmp1FE[28]="list_t<list_t<aggrfield_t>>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp1FE,_tmp1FE + 
28}};struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp1FF=yy1;struct Cyc_List_List*_tmp200;_LL160: if((
_tmp1FF.YY26).tag != 30)goto _LL162;_tmp200=(_tmp1FF.YY26).f1;_LL161: yyzzz=_tmp200;
goto _LL15F;_LL162:;_LL163:(int)_throw((void*)& Cyc_yyfail_YY26);_LL15F:;}return
yyzzz;}static char _tmp202[24]="list_t<type_modifier_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp202,_tmp202 + 24}};struct Cyc_List_List*Cyc_yyget_YY27(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp203=yy1;struct Cyc_List_List*_tmp204;_LL165: if((_tmp203.YY27).tag != 31)goto
_LL167;_tmp204=(_tmp203.YY27).f1;_LL166: yyzzz=_tmp204;goto _LL164;_LL167:;_LL168:(
int)_throw((void*)& Cyc_yyfail_YY27);_LL164:;}return yyzzz;}static char _tmp206[13]="declarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp206,
_tmp206 + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY28(union Cyc_YYSTYPE_union
yy1){struct Cyc_Parse_Declarator*yyzzz;{union Cyc_YYSTYPE_union _tmp207=yy1;struct
Cyc_Parse_Declarator*_tmp208;_LL16A: if((_tmp207.YY28).tag != 32)goto _LL16C;
_tmp208=(_tmp207.YY28).f1;_LL16B: yyzzz=_tmp208;goto _LL169;_LL16C:;_LL16D:(int)
_throw((void*)& Cyc_yyfail_YY28);_LL169:;}return yyzzz;}static char _tmp20A[21]="abstractdeclarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp20A,
_tmp20A + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_YY29(union Cyc_YYSTYPE_union
yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{union Cyc_YYSTYPE_union _tmp20B=yy1;
struct Cyc_Parse_Abstractdeclarator*_tmp20C;_LL16F: if((_tmp20B.YY29).tag != 33)
goto _LL171;_tmp20C=(_tmp20B.YY29).f1;_LL170: yyzzz=_tmp20C;goto _LL16E;_LL171:;
_LL172:(int)_throw((void*)& Cyc_yyfail_YY29);_LL16E:;}return yyzzz;}static char
_tmp20E[5]="bool";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{
_tmp20E,_tmp20E + 5}};int Cyc_yyget_YY30(union Cyc_YYSTYPE_union yy1){int yyzzz;{
union Cyc_YYSTYPE_union _tmp20F=yy1;int _tmp210;_LL174: if((_tmp20F.YY30).tag != 34)
goto _LL176;_tmp210=(_tmp20F.YY30).f1;_LL175: yyzzz=_tmp210;goto _LL173;_LL176:;
_LL177:(int)_throw((void*)& Cyc_yyfail_YY30);_LL173:;}return yyzzz;}static char
_tmp212[8]="scope_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{
_tmp212,_tmp212 + 8}};void*Cyc_yyget_YY31(union Cyc_YYSTYPE_union yy1){void*yyzzz;{
union Cyc_YYSTYPE_union _tmp213=yy1;void*_tmp214;_LL179: if((_tmp213.YY31).tag != 35)
goto _LL17B;_tmp214=(_tmp213.YY31).f1;_LL17A: yyzzz=_tmp214;goto _LL178;_LL17B:;
_LL17C:(int)_throw((void*)& Cyc_yyfail_YY31);_LL178:;}return yyzzz;}static char
_tmp216[14]="tunionfield_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY32={
Cyc_Core_Failure,{_tmp216,_tmp216 + 14}};struct Cyc_Absyn_Tunionfield*Cyc_yyget_YY32(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Tunionfield*yyzzz;{union Cyc_YYSTYPE_union
_tmp217=yy1;struct Cyc_Absyn_Tunionfield*_tmp218;_LL17E: if((_tmp217.YY32).tag != 
36)goto _LL180;_tmp218=(_tmp217.YY32).f1;_LL17F: yyzzz=_tmp218;goto _LL17D;_LL180:;
_LL181:(int)_throw((void*)& Cyc_yyfail_YY32);_LL17D:;}return yyzzz;}static char
_tmp21A[22]="list_t<tunionfield_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY33={
Cyc_Core_Failure,{_tmp21A,_tmp21A + 22}};struct Cyc_List_List*Cyc_yyget_YY33(union
Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp21B=
yy1;struct Cyc_List_List*_tmp21C;_LL183: if((_tmp21B.YY33).tag != 37)goto _LL185;
_tmp21C=(_tmp21B.YY33).f1;_LL184: yyzzz=_tmp21C;goto _LL182;_LL185:;_LL186:(int)
_throw((void*)& Cyc_yyfail_YY33);_LL182:;}return yyzzz;}static char _tmp21E[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp21E,
_tmp21E + 50}};struct _tuple16*Cyc_yyget_YY34(union Cyc_YYSTYPE_union yy1){struct
_tuple16*yyzzz;{union Cyc_YYSTYPE_union _tmp21F=yy1;struct _tuple16*_tmp220;_LL188:
if((_tmp21F.YY34).tag != 38)goto _LL18A;_tmp220=(_tmp21F.YY34).f1;_LL189: yyzzz=
_tmp220;goto _LL187;_LL18A:;_LL18B:(int)_throw((void*)& Cyc_yyfail_YY34);_LL187:;}
return yyzzz;}static char _tmp222[14]="list_t<var_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp222,_tmp222 + 14}};struct Cyc_List_List*Cyc_yyget_YY35(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp223=yy1;struct Cyc_List_List*_tmp224;_LL18D: if((_tmp223.YY35).tag != 39)goto
_LL18F;_tmp224=(_tmp223.YY35).f1;_LL18E: yyzzz=_tmp224;goto _LL18C;_LL18F:;_LL190:(
int)_throw((void*)& Cyc_yyfail_YY35);_LL18C:;}return yyzzz;}static char _tmp226[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp226,
_tmp226 + 32}};struct _tuple2*Cyc_yyget_YY36(union Cyc_YYSTYPE_union yy1){struct
_tuple2*yyzzz;{union Cyc_YYSTYPE_union _tmp227=yy1;struct _tuple2*_tmp228;_LL192:
if((_tmp227.YY36).tag != 40)goto _LL194;_tmp228=(_tmp227.YY36).f1;_LL193: yyzzz=
_tmp228;goto _LL191;_LL194:;_LL195:(int)_throw((void*)& Cyc_yyfail_YY36);_LL191:;}
return yyzzz;}static char _tmp22A[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp22A,
_tmp22A + 40}};struct Cyc_List_List*Cyc_yyget_YY37(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp22B=yy1;struct Cyc_List_List*
_tmp22C;_LL197: if((_tmp22B.YY37).tag != 41)goto _LL199;_tmp22C=(_tmp22B.YY37).f1;
_LL198: yyzzz=_tmp22C;goto _LL196;_LL199:;_LL19A:(int)_throw((void*)& Cyc_yyfail_YY37);
_LL196:;}return yyzzz;}static char _tmp22E[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp22E,
_tmp22E + 107}};struct _tuple17*Cyc_yyget_YY38(union Cyc_YYSTYPE_union yy1){struct
_tuple17*yyzzz;{union Cyc_YYSTYPE_union _tmp22F=yy1;struct _tuple17*_tmp230;_LL19C:
if((_tmp22F.YY38).tag != 42)goto _LL19E;_tmp230=(_tmp22F.YY38).f1;_LL19D: yyzzz=
_tmp230;goto _LL19B;_LL19E:;_LL19F:(int)_throw((void*)& Cyc_yyfail_YY38);_LL19B:;}
return yyzzz;}static char _tmp232[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp232,_tmp232 + 15}};struct Cyc_List_List*Cyc_yyget_YY39(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp233=yy1;struct Cyc_List_List*_tmp234;_LL1A1: if((_tmp233.YY39).tag != 43)goto
_LL1A3;_tmp234=(_tmp233.YY39).f1;_LL1A2: yyzzz=_tmp234;goto _LL1A0;_LL1A3:;_LL1A4:(
int)_throw((void*)& Cyc_yyfail_YY39);_LL1A0:;}return yyzzz;}static char _tmp236[21]="list_t<designator_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp236,
_tmp236 + 21}};struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp237=yy1;struct Cyc_List_List*
_tmp238;_LL1A6: if((_tmp237.YY40).tag != 44)goto _LL1A8;_tmp238=(_tmp237.YY40).f1;
_LL1A7: yyzzz=_tmp238;goto _LL1A5;_LL1A8:;_LL1A9:(int)_throw((void*)& Cyc_yyfail_YY40);
_LL1A5:;}return yyzzz;}static char _tmp23A[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp23A,_tmp23A + 13}};void*Cyc_yyget_YY41(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp23B=yy1;void*
_tmp23C;_LL1AB: if((_tmp23B.YY41).tag != 45)goto _LL1AD;_tmp23C=(_tmp23B.YY41).f1;
_LL1AC: yyzzz=_tmp23C;goto _LL1AA;_LL1AD:;_LL1AE:(int)_throw((void*)& Cyc_yyfail_YY41);
_LL1AA:;}return yyzzz;}static char _tmp23E[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp23E,_tmp23E + 7}};void*Cyc_yyget_YY42(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp23F=yy1;void*
_tmp240;_LL1B0: if((_tmp23F.YY42).tag != 46)goto _LL1B2;_tmp240=(_tmp23F.YY42).f1;
_LL1B1: yyzzz=_tmp240;goto _LL1AF;_LL1B2:;_LL1B3:(int)_throw((void*)& Cyc_yyfail_YY42);
_LL1AF:;}return yyzzz;}static char _tmp242[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp242,_tmp242 + 7}};void*Cyc_yyget_YY43(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp243=yy1;void*
_tmp244;_LL1B5: if((_tmp243.YY43).tag != 47)goto _LL1B7;_tmp244=(_tmp243.YY43).f1;
_LL1B6: yyzzz=_tmp244;goto _LL1B4;_LL1B7:;_LL1B8:(int)_throw((void*)& Cyc_yyfail_YY43);
_LL1B4:;}return yyzzz;}static char _tmp246[20]="list_t<attribute_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp246,_tmp246 + 20}};
struct Cyc_List_List*Cyc_yyget_YY44(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp247=yy1;struct Cyc_List_List*_tmp248;_LL1BA: if((
_tmp247.YY44).tag != 48)goto _LL1BC;_tmp248=(_tmp247.YY44).f1;_LL1BB: yyzzz=_tmp248;
goto _LL1B9;_LL1BC:;_LL1BD:(int)_throw((void*)& Cyc_yyfail_YY44);_LL1B9:;}return
yyzzz;}static char _tmp24A[12]="attribute_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp24A,_tmp24A + 12}};void*Cyc_yyget_YY45(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp24B=yy1;void*
_tmp24C;_LL1BF: if((_tmp24B.YY45).tag != 49)goto _LL1C1;_tmp24C=(_tmp24B.YY45).f1;
_LL1C0: yyzzz=_tmp24C;goto _LL1BE;_LL1C1:;_LL1C2:(int)_throw((void*)& Cyc_yyfail_YY45);
_LL1BE:;}return yyzzz;}static char _tmp24E[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp24E,_tmp24E + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_YY46(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union
Cyc_YYSTYPE_union _tmp24F=yy1;struct Cyc_Absyn_Enumfield*_tmp250;_LL1C4: if((
_tmp24F.YY46).tag != 50)goto _LL1C6;_tmp250=(_tmp24F.YY46).f1;_LL1C5: yyzzz=_tmp250;
goto _LL1C3;_LL1C6:;_LL1C7:(int)_throw((void*)& Cyc_yyfail_YY46);_LL1C3:;}return
yyzzz;}static char _tmp252[20]="list_t<enumfield_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp252,_tmp252 + 20}};struct Cyc_List_List*Cyc_yyget_YY47(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp253=yy1;struct Cyc_List_List*_tmp254;_LL1C9: if((_tmp253.YY47).tag != 51)goto
_LL1CB;_tmp254=(_tmp253.YY47).f1;_LL1CA: yyzzz=_tmp254;goto _LL1C8;_LL1CB:;_LL1CC:(
int)_throw((void*)& Cyc_yyfail_YY47);_LL1C8:;}return yyzzz;}static char _tmp256[14]="opt_t<type_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp256,
_tmp256 + 14}};struct Cyc_Core_Opt*Cyc_yyget_YY48(union Cyc_YYSTYPE_union yy1){
struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE_union _tmp257=yy1;struct Cyc_Core_Opt*
_tmp258;_LL1CE: if((_tmp257.YY48).tag != 52)goto _LL1D0;_tmp258=(_tmp257.YY48).f1;
_LL1CF: yyzzz=_tmp258;goto _LL1CD;_LL1D0:;_LL1D1:(int)_throw((void*)& Cyc_yyfail_YY48);
_LL1CD:;}return yyzzz;}static char _tmp25A[26]="list_t<$(type_t,type_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp25A,_tmp25A + 
26}};struct Cyc_List_List*Cyc_yyget_YY49(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp25B=yy1;struct Cyc_List_List*_tmp25C;_LL1D3: if((
_tmp25B.YY49).tag != 53)goto _LL1D5;_tmp25C=(_tmp25B.YY49).f1;_LL1D4: yyzzz=_tmp25C;
goto _LL1D2;_LL1D5:;_LL1D6:(int)_throw((void*)& Cyc_yyfail_YY49);_LL1D2:;}return
yyzzz;}static char _tmp25E[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp25E,_tmp25E + 15}};struct Cyc_Absyn_Conref*
Cyc_yyget_YY50(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Conref*yyzzz;{union Cyc_YYSTYPE_union
_tmp25F=yy1;struct Cyc_Absyn_Conref*_tmp260;_LL1D8: if((_tmp25F.YY50).tag != 54)
goto _LL1DA;_tmp260=(_tmp25F.YY50).f1;_LL1D9: yyzzz=_tmp260;goto _LL1D7;_LL1DA:;
_LL1DB:(int)_throw((void*)& Cyc_yyfail_YY50);_LL1D7:;}return yyzzz;}static char
_tmp262[40]="list_t<$(Position::seg_t,qvar_t,bool)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp262,_tmp262 + 40}};struct Cyc_List_List*Cyc_yyget_YY51(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp263=yy1;struct Cyc_List_List*_tmp264;_LL1DD: if((_tmp263.YY51).tag != 55)goto
_LL1DF;_tmp264=(_tmp263.YY51).f1;_LL1DE: yyzzz=_tmp264;goto _LL1DC;_LL1DF:;_LL1E0:(
int)_throw((void*)& Cyc_yyfail_YY51);_LL1DC:;}return yyzzz;}struct Cyc_Yyltype{int
timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_Yyltype
Cyc_yynewloc(){return({struct Cyc_Yyltype _tmp265;_tmp265.timestamp=0;_tmp265.first_line=
0;_tmp265.first_column=0;_tmp265.last_line=0;_tmp265.last_column=0;_tmp265;});}
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[358]={0,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,129,2,2,111,127,125,2,
108,109,117,122,106,120,114,126,2,2,2,2,2,2,2,2,2,2,113,103,112,107,121,119,118,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,115,2,116,124,110,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,104,123,105,128,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,
58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,
85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102};static char _tmp266[2]="$";
static char _tmp267[6]="error";static char _tmp268[12]="$undefined.";static char
_tmp269[5]="AUTO";static char _tmp26A[9]="REGISTER";static char _tmp26B[7]="STATIC";
static char _tmp26C[7]="EXTERN";static char _tmp26D[8]="TYPEDEF";static char _tmp26E[5]="VOID";
static char _tmp26F[5]="CHAR";static char _tmp270[6]="SHORT";static char _tmp271[4]="INT";
static char _tmp272[5]="LONG";static char _tmp273[6]="FLOAT";static char _tmp274[7]="DOUBLE";
static char _tmp275[7]="SIGNED";static char _tmp276[9]="UNSIGNED";static char _tmp277[
6]="CONST";static char _tmp278[9]="VOLATILE";static char _tmp279[9]="RESTRICT";
static char _tmp27A[7]="STRUCT";static char _tmp27B[6]="UNION";static char _tmp27C[5]="CASE";
static char _tmp27D[8]="DEFAULT";static char _tmp27E[7]="INLINE";static char _tmp27F[7]="SIZEOF";
static char _tmp280[9]="OFFSETOF";static char _tmp281[3]="IF";static char _tmp282[5]="ELSE";
static char _tmp283[7]="SWITCH";static char _tmp284[6]="WHILE";static char _tmp285[3]="DO";
static char _tmp286[4]="FOR";static char _tmp287[5]="GOTO";static char _tmp288[9]="CONTINUE";
static char _tmp289[6]="BREAK";static char _tmp28A[7]="RETURN";static char _tmp28B[5]="ENUM";
static char _tmp28C[8]="NULL_kw";static char _tmp28D[4]="LET";static char _tmp28E[6]="THROW";
static char _tmp28F[4]="TRY";static char _tmp290[6]="CATCH";static char _tmp291[7]="EXPORT";
static char _tmp292[4]="NEW";static char _tmp293[9]="ABSTRACT";static char _tmp294[9]="FALLTHRU";
static char _tmp295[6]="USING";static char _tmp296[10]="NAMESPACE";static char _tmp297[
7]="TUNION";static char _tmp298[8]="XTUNION";static char _tmp299[7]="MALLOC";static
char _tmp29A[8]="RMALLOC";static char _tmp29B[7]="CALLOC";static char _tmp29C[8]="RCALLOC";
static char _tmp29D[9]="REGION_T";static char _tmp29E[9]="SIZEOF_T";static char
_tmp29F[6]="TAG_T";static char _tmp2A0[7]="REGION";static char _tmp2A1[5]="RNEW";
static char _tmp2A2[8]="REGIONS";static char _tmp2A3[13]="RESET_REGION";static char
_tmp2A4[4]="GEN";static char _tmp2A5[14]="NOZEROTERM_kw";static char _tmp2A6[12]="ZEROTERM_kw";
static char _tmp2A7[7]="PORTON";static char _tmp2A8[8]="PORTOFF";static char _tmp2A9[8]="FLAT_kw";
static char _tmp2AA[7]="PTR_OP";static char _tmp2AB[7]="INC_OP";static char _tmp2AC[7]="DEC_OP";
static char _tmp2AD[8]="LEFT_OP";static char _tmp2AE[9]="RIGHT_OP";static char _tmp2AF[
6]="LE_OP";static char _tmp2B0[6]="GE_OP";static char _tmp2B1[6]="EQ_OP";static char
_tmp2B2[6]="NE_OP";static char _tmp2B3[7]="AND_OP";static char _tmp2B4[6]="OR_OP";
static char _tmp2B5[11]="MUL_ASSIGN";static char _tmp2B6[11]="DIV_ASSIGN";static char
_tmp2B7[11]="MOD_ASSIGN";static char _tmp2B8[11]="ADD_ASSIGN";static char _tmp2B9[11]="SUB_ASSIGN";
static char _tmp2BA[12]="LEFT_ASSIGN";static char _tmp2BB[13]="RIGHT_ASSIGN";static
char _tmp2BC[11]="AND_ASSIGN";static char _tmp2BD[11]="XOR_ASSIGN";static char
_tmp2BE[10]="OR_ASSIGN";static char _tmp2BF[9]="ELLIPSIS";static char _tmp2C0[11]="LEFT_RIGHT";
static char _tmp2C1[12]="COLON_COLON";static char _tmp2C2[11]="IDENTIFIER";static
char _tmp2C3[17]="INTEGER_CONSTANT";static char _tmp2C4[7]="STRING";static char
_tmp2C5[19]="CHARACTER_CONSTANT";static char _tmp2C6[18]="FLOATING_CONSTANT";
static char _tmp2C7[9]="TYPE_VAR";static char _tmp2C8[13]="TYPEDEF_NAME";static char
_tmp2C9[16]="QUAL_IDENTIFIER";static char _tmp2CA[18]="QUAL_TYPEDEF_NAME";static
char _tmp2CB[13]="TYPE_INTEGER";static char _tmp2CC[10]="ATTRIBUTE";static char
_tmp2CD[4]="';'";static char _tmp2CE[4]="'{'";static char _tmp2CF[4]="'}'";static
char _tmp2D0[4]="','";static char _tmp2D1[4]="'='";static char _tmp2D2[4]="'('";
static char _tmp2D3[4]="')'";static char _tmp2D4[4]="'_'";static char _tmp2D5[4]="'$'";
static char _tmp2D6[4]="'<'";static char _tmp2D7[4]="':'";static char _tmp2D8[4]="'.'";
static char _tmp2D9[4]="'['";static char _tmp2DA[4]="']'";static char _tmp2DB[4]="'*'";
static char _tmp2DC[4]="'@'";static char _tmp2DD[4]="'?'";static char _tmp2DE[4]="'-'";
static char _tmp2DF[4]="'>'";static char _tmp2E0[4]="'+'";static char _tmp2E1[4]="'|'";
static char _tmp2E2[4]="'^'";static char _tmp2E3[4]="'&'";static char _tmp2E4[4]="'/'";
static char _tmp2E5[4]="'%'";static char _tmp2E6[4]="'~'";static char _tmp2E7[4]="'!'";
static char _tmp2E8[5]="prog";static char _tmp2E9[17]="translation_unit";static char
_tmp2EA[12]="export_list";static char _tmp2EB[19]="export_list_values";static char
_tmp2EC[21]="external_declaration";static char _tmp2ED[20]="function_definition";
static char _tmp2EE[21]="function_definition2";static char _tmp2EF[13]="using_action";
static char _tmp2F0[15]="unusing_action";static char _tmp2F1[17]="namespace_action";
static char _tmp2F2[19]="unnamespace_action";static char _tmp2F3[12]="declaration";
static char _tmp2F4[17]="declaration_list";static char _tmp2F5[23]="declaration_specifiers";
static char _tmp2F6[24]="storage_class_specifier";static char _tmp2F7[15]="attributes_opt";
static char _tmp2F8[11]="attributes";static char _tmp2F9[15]="attribute_list";static
char _tmp2FA[10]="attribute";static char _tmp2FB[15]="type_specifier";static char
_tmp2FC[25]="type_specifier_notypedef";static char _tmp2FD[5]="kind";static char
_tmp2FE[15]="type_qualifier";static char _tmp2FF[15]="enum_specifier";static char
_tmp300[11]="enum_field";static char _tmp301[22]="enum_declaration_list";static
char _tmp302[26]="struct_or_union_specifier";static char _tmp303[16]="type_params_opt";
static char _tmp304[16]="struct_or_union";static char _tmp305[24]="struct_declaration_list";
static char _tmp306[25]="struct_declaration_list0";static char _tmp307[21]="init_declarator_list";
static char _tmp308[22]="init_declarator_list0";static char _tmp309[16]="init_declarator";
static char _tmp30A[19]="struct_declaration";static char _tmp30B[25]="specifier_qualifier_list";
static char _tmp30C[35]="notypedef_specifier_qualifier_list";static char _tmp30D[23]="struct_declarator_list";
static char _tmp30E[24]="struct_declarator_list0";static char _tmp30F[18]="struct_declarator";
static char _tmp310[17]="tunion_specifier";static char _tmp311[9]="flat_opt";static
char _tmp312[18]="tunion_or_xtunion";static char _tmp313[17]="tunionfield_list";
static char _tmp314[18]="tunionfield_scope";static char _tmp315[12]="tunionfield";
static char _tmp316[11]="declarator";static char _tmp317[23]="declarator_withtypedef";
static char _tmp318[18]="direct_declarator";static char _tmp319[30]="direct_declarator_withtypedef";
static char _tmp31A[8]="pointer";static char _tmp31B[12]="one_pointer";static char
_tmp31C[23]="pointer_null_and_bound";static char _tmp31D[14]="pointer_bound";
static char _tmp31E[18]="zeroterm_qual_opt";static char _tmp31F[12]="opt_rgn_opt";
static char _tmp320[8]="rgn_opt";static char _tmp321[11]="tqual_list";static char
_tmp322[20]="parameter_type_list";static char _tmp323[9]="type_var";static char
_tmp324[16]="optional_effect";static char _tmp325[19]="optional_rgn_order";static
char _tmp326[10]="rgn_order";static char _tmp327[16]="optional_inject";static char
_tmp328[11]="effect_set";static char _tmp329[14]="atomic_effect";static char _tmp32A[
11]="region_set";static char _tmp32B[15]="parameter_list";static char _tmp32C[22]="parameter_declaration";
static char _tmp32D[16]="identifier_list";static char _tmp32E[17]="identifier_list0";
static char _tmp32F[12]="initializer";static char _tmp330[18]="array_initializer";
static char _tmp331[17]="initializer_list";static char _tmp332[12]="designation";
static char _tmp333[16]="designator_list";static char _tmp334[11]="designator";
static char _tmp335[10]="type_name";static char _tmp336[14]="any_type_name";static
char _tmp337[15]="type_name_list";static char _tmp338[20]="abstract_declarator";
static char _tmp339[27]="direct_abstract_declarator";static char _tmp33A[10]="statement";
static char _tmp33B[18]="labeled_statement";static char _tmp33C[21]="expression_statement";
static char _tmp33D[19]="compound_statement";static char _tmp33E[16]="block_item_list";
static char _tmp33F[20]="selection_statement";static char _tmp340[15]="switch_clauses";
static char _tmp341[20]="iteration_statement";static char _tmp342[15]="jump_statement";
static char _tmp343[12]="exp_pattern";static char _tmp344[20]="conditional_pattern";
static char _tmp345[19]="logical_or_pattern";static char _tmp346[20]="logical_and_pattern";
static char _tmp347[21]="inclusive_or_pattern";static char _tmp348[21]="exclusive_or_pattern";
static char _tmp349[12]="and_pattern";static char _tmp34A[17]="equality_pattern";
static char _tmp34B[19]="relational_pattern";static char _tmp34C[14]="shift_pattern";
static char _tmp34D[17]="additive_pattern";static char _tmp34E[23]="multiplicative_pattern";
static char _tmp34F[13]="cast_pattern";static char _tmp350[14]="unary_pattern";
static char _tmp351[16]="postfix_pattern";static char _tmp352[16]="primary_pattern";
static char _tmp353[8]="pattern";static char _tmp354[19]="tuple_pattern_list";static
char _tmp355[20]="tuple_pattern_list0";static char _tmp356[14]="field_pattern";
static char _tmp357[19]="field_pattern_list";static char _tmp358[20]="field_pattern_list0";
static char _tmp359[11]="expression";static char _tmp35A[22]="assignment_expression";
static char _tmp35B[20]="assignment_operator";static char _tmp35C[23]="conditional_expression";
static char _tmp35D[20]="constant_expression";static char _tmp35E[22]="logical_or_expression";
static char _tmp35F[23]="logical_and_expression";static char _tmp360[24]="inclusive_or_expression";
static char _tmp361[24]="exclusive_or_expression";static char _tmp362[15]="and_expression";
static char _tmp363[20]="equality_expression";static char _tmp364[22]="relational_expression";
static char _tmp365[17]="shift_expression";static char _tmp366[20]="additive_expression";
static char _tmp367[26]="multiplicative_expression";static char _tmp368[16]="cast_expression";
static char _tmp369[17]="unary_expression";static char _tmp36A[15]="unary_operator";
static char _tmp36B[19]="postfix_expression";static char _tmp36C[19]="primary_expression";
static char _tmp36D[25]="argument_expression_list";static char _tmp36E[26]="argument_expression_list0";
static char _tmp36F[9]="constant";static char _tmp370[20]="qual_opt_identifier";
static char _tmp371[17]="qual_opt_typedef";static char _tmp372[18]="struct_union_name";
static char _tmp373[11]="field_name";static char _tmp374[12]="right_angle";static
struct _dynforward_ptr Cyc_yytname[271]={{_tmp266,_tmp266 + 2},{_tmp267,_tmp267 + 6},{
_tmp268,_tmp268 + 12},{_tmp269,_tmp269 + 5},{_tmp26A,_tmp26A + 9},{_tmp26B,_tmp26B + 
7},{_tmp26C,_tmp26C + 7},{_tmp26D,_tmp26D + 8},{_tmp26E,_tmp26E + 5},{_tmp26F,
_tmp26F + 5},{_tmp270,_tmp270 + 6},{_tmp271,_tmp271 + 4},{_tmp272,_tmp272 + 5},{
_tmp273,_tmp273 + 6},{_tmp274,_tmp274 + 7},{_tmp275,_tmp275 + 7},{_tmp276,_tmp276 + 
9},{_tmp277,_tmp277 + 6},{_tmp278,_tmp278 + 9},{_tmp279,_tmp279 + 9},{_tmp27A,
_tmp27A + 7},{_tmp27B,_tmp27B + 6},{_tmp27C,_tmp27C + 5},{_tmp27D,_tmp27D + 8},{
_tmp27E,_tmp27E + 7},{_tmp27F,_tmp27F + 7},{_tmp280,_tmp280 + 9},{_tmp281,_tmp281 + 
3},{_tmp282,_tmp282 + 5},{_tmp283,_tmp283 + 7},{_tmp284,_tmp284 + 6},{_tmp285,
_tmp285 + 3},{_tmp286,_tmp286 + 4},{_tmp287,_tmp287 + 5},{_tmp288,_tmp288 + 9},{
_tmp289,_tmp289 + 6},{_tmp28A,_tmp28A + 7},{_tmp28B,_tmp28B + 5},{_tmp28C,_tmp28C + 
8},{_tmp28D,_tmp28D + 4},{_tmp28E,_tmp28E + 6},{_tmp28F,_tmp28F + 4},{_tmp290,
_tmp290 + 6},{_tmp291,_tmp291 + 7},{_tmp292,_tmp292 + 4},{_tmp293,_tmp293 + 9},{
_tmp294,_tmp294 + 9},{_tmp295,_tmp295 + 6},{_tmp296,_tmp296 + 10},{_tmp297,_tmp297 + 
7},{_tmp298,_tmp298 + 8},{_tmp299,_tmp299 + 7},{_tmp29A,_tmp29A + 8},{_tmp29B,
_tmp29B + 7},{_tmp29C,_tmp29C + 8},{_tmp29D,_tmp29D + 9},{_tmp29E,_tmp29E + 9},{
_tmp29F,_tmp29F + 6},{_tmp2A0,_tmp2A0 + 7},{_tmp2A1,_tmp2A1 + 5},{_tmp2A2,_tmp2A2 + 
8},{_tmp2A3,_tmp2A3 + 13},{_tmp2A4,_tmp2A4 + 4},{_tmp2A5,_tmp2A5 + 14},{_tmp2A6,
_tmp2A6 + 12},{_tmp2A7,_tmp2A7 + 7},{_tmp2A8,_tmp2A8 + 8},{_tmp2A9,_tmp2A9 + 8},{
_tmp2AA,_tmp2AA + 7},{_tmp2AB,_tmp2AB + 7},{_tmp2AC,_tmp2AC + 7},{_tmp2AD,_tmp2AD + 
8},{_tmp2AE,_tmp2AE + 9},{_tmp2AF,_tmp2AF + 6},{_tmp2B0,_tmp2B0 + 6},{_tmp2B1,
_tmp2B1 + 6},{_tmp2B2,_tmp2B2 + 6},{_tmp2B3,_tmp2B3 + 7},{_tmp2B4,_tmp2B4 + 6},{
_tmp2B5,_tmp2B5 + 11},{_tmp2B6,_tmp2B6 + 11},{_tmp2B7,_tmp2B7 + 11},{_tmp2B8,
_tmp2B8 + 11},{_tmp2B9,_tmp2B9 + 11},{_tmp2BA,_tmp2BA + 12},{_tmp2BB,_tmp2BB + 13},{
_tmp2BC,_tmp2BC + 11},{_tmp2BD,_tmp2BD + 11},{_tmp2BE,_tmp2BE + 10},{_tmp2BF,
_tmp2BF + 9},{_tmp2C0,_tmp2C0 + 11},{_tmp2C1,_tmp2C1 + 12},{_tmp2C2,_tmp2C2 + 11},{
_tmp2C3,_tmp2C3 + 17},{_tmp2C4,_tmp2C4 + 7},{_tmp2C5,_tmp2C5 + 19},{_tmp2C6,_tmp2C6
+ 18},{_tmp2C7,_tmp2C7 + 9},{_tmp2C8,_tmp2C8 + 13},{_tmp2C9,_tmp2C9 + 16},{_tmp2CA,
_tmp2CA + 18},{_tmp2CB,_tmp2CB + 13},{_tmp2CC,_tmp2CC + 10},{_tmp2CD,_tmp2CD + 4},{
_tmp2CE,_tmp2CE + 4},{_tmp2CF,_tmp2CF + 4},{_tmp2D0,_tmp2D0 + 4},{_tmp2D1,_tmp2D1 + 
4},{_tmp2D2,_tmp2D2 + 4},{_tmp2D3,_tmp2D3 + 4},{_tmp2D4,_tmp2D4 + 4},{_tmp2D5,
_tmp2D5 + 4},{_tmp2D6,_tmp2D6 + 4},{_tmp2D7,_tmp2D7 + 4},{_tmp2D8,_tmp2D8 + 4},{
_tmp2D9,_tmp2D9 + 4},{_tmp2DA,_tmp2DA + 4},{_tmp2DB,_tmp2DB + 4},{_tmp2DC,_tmp2DC + 
4},{_tmp2DD,_tmp2DD + 4},{_tmp2DE,_tmp2DE + 4},{_tmp2DF,_tmp2DF + 4},{_tmp2E0,
_tmp2E0 + 4},{_tmp2E1,_tmp2E1 + 4},{_tmp2E2,_tmp2E2 + 4},{_tmp2E3,_tmp2E3 + 4},{
_tmp2E4,_tmp2E4 + 4},{_tmp2E5,_tmp2E5 + 4},{_tmp2E6,_tmp2E6 + 4},{_tmp2E7,_tmp2E7 + 
4},{_tmp2E8,_tmp2E8 + 5},{_tmp2E9,_tmp2E9 + 17},{_tmp2EA,_tmp2EA + 12},{_tmp2EB,
_tmp2EB + 19},{_tmp2EC,_tmp2EC + 21},{_tmp2ED,_tmp2ED + 20},{_tmp2EE,_tmp2EE + 21},{
_tmp2EF,_tmp2EF + 13},{_tmp2F0,_tmp2F0 + 15},{_tmp2F1,_tmp2F1 + 17},{_tmp2F2,
_tmp2F2 + 19},{_tmp2F3,_tmp2F3 + 12},{_tmp2F4,_tmp2F4 + 17},{_tmp2F5,_tmp2F5 + 23},{
_tmp2F6,_tmp2F6 + 24},{_tmp2F7,_tmp2F7 + 15},{_tmp2F8,_tmp2F8 + 11},{_tmp2F9,
_tmp2F9 + 15},{_tmp2FA,_tmp2FA + 10},{_tmp2FB,_tmp2FB + 15},{_tmp2FC,_tmp2FC + 25},{
_tmp2FD,_tmp2FD + 5},{_tmp2FE,_tmp2FE + 15},{_tmp2FF,_tmp2FF + 15},{_tmp300,_tmp300
+ 11},{_tmp301,_tmp301 + 22},{_tmp302,_tmp302 + 26},{_tmp303,_tmp303 + 16},{_tmp304,
_tmp304 + 16},{_tmp305,_tmp305 + 24},{_tmp306,_tmp306 + 25},{_tmp307,_tmp307 + 21},{
_tmp308,_tmp308 + 22},{_tmp309,_tmp309 + 16},{_tmp30A,_tmp30A + 19},{_tmp30B,
_tmp30B + 25},{_tmp30C,_tmp30C + 35},{_tmp30D,_tmp30D + 23},{_tmp30E,_tmp30E + 24},{
_tmp30F,_tmp30F + 18},{_tmp310,_tmp310 + 17},{_tmp311,_tmp311 + 9},{_tmp312,_tmp312
+ 18},{_tmp313,_tmp313 + 17},{_tmp314,_tmp314 + 18},{_tmp315,_tmp315 + 12},{_tmp316,
_tmp316 + 11},{_tmp317,_tmp317 + 23},{_tmp318,_tmp318 + 18},{_tmp319,_tmp319 + 30},{
_tmp31A,_tmp31A + 8},{_tmp31B,_tmp31B + 12},{_tmp31C,_tmp31C + 23},{_tmp31D,_tmp31D
+ 14},{_tmp31E,_tmp31E + 18},{_tmp31F,_tmp31F + 12},{_tmp320,_tmp320 + 8},{_tmp321,
_tmp321 + 11},{_tmp322,_tmp322 + 20},{_tmp323,_tmp323 + 9},{_tmp324,_tmp324 + 16},{
_tmp325,_tmp325 + 19},{_tmp326,_tmp326 + 10},{_tmp327,_tmp327 + 16},{_tmp328,
_tmp328 + 11},{_tmp329,_tmp329 + 14},{_tmp32A,_tmp32A + 11},{_tmp32B,_tmp32B + 15},{
_tmp32C,_tmp32C + 22},{_tmp32D,_tmp32D + 16},{_tmp32E,_tmp32E + 17},{_tmp32F,
_tmp32F + 12},{_tmp330,_tmp330 + 18},{_tmp331,_tmp331 + 17},{_tmp332,_tmp332 + 12},{
_tmp333,_tmp333 + 16},{_tmp334,_tmp334 + 11},{_tmp335,_tmp335 + 10},{_tmp336,
_tmp336 + 14},{_tmp337,_tmp337 + 15},{_tmp338,_tmp338 + 20},{_tmp339,_tmp339 + 27},{
_tmp33A,_tmp33A + 10},{_tmp33B,_tmp33B + 18},{_tmp33C,_tmp33C + 21},{_tmp33D,
_tmp33D + 19},{_tmp33E,_tmp33E + 16},{_tmp33F,_tmp33F + 20},{_tmp340,_tmp340 + 15},{
_tmp341,_tmp341 + 20},{_tmp342,_tmp342 + 15},{_tmp343,_tmp343 + 12},{_tmp344,
_tmp344 + 20},{_tmp345,_tmp345 + 19},{_tmp346,_tmp346 + 20},{_tmp347,_tmp347 + 21},{
_tmp348,_tmp348 + 21},{_tmp349,_tmp349 + 12},{_tmp34A,_tmp34A + 17},{_tmp34B,
_tmp34B + 19},{_tmp34C,_tmp34C + 14},{_tmp34D,_tmp34D + 17},{_tmp34E,_tmp34E + 23},{
_tmp34F,_tmp34F + 13},{_tmp350,_tmp350 + 14},{_tmp351,_tmp351 + 16},{_tmp352,
_tmp352 + 16},{_tmp353,_tmp353 + 8},{_tmp354,_tmp354 + 19},{_tmp355,_tmp355 + 20},{
_tmp356,_tmp356 + 14},{_tmp357,_tmp357 + 19},{_tmp358,_tmp358 + 20},{_tmp359,
_tmp359 + 11},{_tmp35A,_tmp35A + 22},{_tmp35B,_tmp35B + 20},{_tmp35C,_tmp35C + 23},{
_tmp35D,_tmp35D + 20},{_tmp35E,_tmp35E + 22},{_tmp35F,_tmp35F + 23},{_tmp360,
_tmp360 + 24},{_tmp361,_tmp361 + 24},{_tmp362,_tmp362 + 15},{_tmp363,_tmp363 + 20},{
_tmp364,_tmp364 + 22},{_tmp365,_tmp365 + 17},{_tmp366,_tmp366 + 20},{_tmp367,
_tmp367 + 26},{_tmp368,_tmp368 + 16},{_tmp369,_tmp369 + 17},{_tmp36A,_tmp36A + 15},{
_tmp36B,_tmp36B + 19},{_tmp36C,_tmp36C + 19},{_tmp36D,_tmp36D + 25},{_tmp36E,
_tmp36E + 26},{_tmp36F,_tmp36F + 9},{_tmp370,_tmp370 + 20},{_tmp371,_tmp371 + 17},{
_tmp372,_tmp372 + 18},{_tmp373,_tmp373 + 11},{_tmp374,_tmp374 + 12}};static short Cyc_yyr1[
495]={0,130,131,131,131,131,131,131,131,131,131,131,132,132,133,133,134,134,134,
135,135,135,135,136,136,137,138,139,140,141,141,141,141,142,142,143,143,143,143,
143,143,143,143,143,143,144,144,144,144,144,144,144,145,145,146,147,147,148,148,
148,148,148,148,149,149,150,150,150,150,150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,151,152,152,152,153,153,153,154,154,155,155,155,156,
156,156,157,157,158,158,159,159,160,160,161,162,162,163,163,164,165,165,165,165,
165,165,166,166,166,166,166,166,167,168,168,169,169,169,170,170,170,171,171,172,
172,173,173,173,173,174,174,174,175,175,176,176,177,177,178,178,178,178,178,178,
178,178,178,178,179,179,179,179,179,179,179,179,179,179,179,180,180,181,182,182,
182,182,183,183,183,184,184,184,185,185,185,186,186,186,187,187,188,188,188,188,
189,189,190,190,191,191,192,192,193,193,194,194,195,195,195,195,196,196,197,197,
198,198,198,199,200,200,201,201,202,202,202,202,203,203,203,203,204,205,205,206,
206,207,207,208,208,208,208,208,209,209,210,210,210,211,211,211,211,211,211,211,
211,211,211,211,212,212,212,212,212,212,212,212,212,212,212,213,214,214,215,215,
216,216,216,216,216,216,217,217,217,217,217,217,218,218,218,218,218,218,219,219,
219,219,219,219,219,219,219,219,219,219,219,219,220,220,220,220,220,220,220,220,
221,222,222,223,223,224,224,225,225,226,226,227,227,228,228,228,229,229,229,229,
229,230,230,230,231,231,231,232,232,232,232,233,233,234,234,234,234,234,234,234,
235,236,237,237,237,237,237,237,237,237,237,237,237,237,237,237,238,238,238,239,
239,240,240,241,241,241,242,242,243,243,244,244,245,245,245,245,245,245,245,245,
245,245,245,246,246,246,246,246,246,246,247,248,248,249,249,250,250,251,251,252,
252,253,253,253,254,254,254,254,254,255,255,255,256,256,256,257,257,257,257,258,
258,259,259,259,259,259,259,259,259,259,259,259,259,259,259,259,259,260,260,260,
261,261,261,261,261,261,261,261,261,261,262,262,262,262,262,262,262,262,262,263,
264,264,265,265,265,265,266,266,267,267,268,268,269,269,270,270};static short Cyc_yyr2[
495]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,1,2,1,2,
1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,
1,1,1,4,4,4,4,1,1,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,
1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,3,8,5,7,1,0,1,1,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,
5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,2,2,1,2,0,3,3,0,1,1,0,1,1,0,1,1,0,2,3,5,
5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,
2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,6,3,9,6,5,3,1,2,
2,3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,
4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,
6,6,1,1,1,3,1,1,3,4,4,5,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,1,1,1,1,1,1,
1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,
3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,9,11,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,1,3,2,5,4,5,
5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[979]={0,18,45,46,47,
48,50,65,68,69,70,71,72,73,74,75,87,88,89,103,104,41,0,0,51,0,0,0,0,84,0,0,136,
485,199,487,486,488,85,0,0,66,0,181,181,179,1,0,16,0,0,17,0,35,43,37,63,39,76,77,
0,78,0,137,149,0,174,184,79,153,101,49,48,42,0,91,484,0,485,481,482,483,0,361,0,
0,0,0,222,0,363,364,25,27,137,137,137,0,0,0,0,0,0,0,137,137,177,178,180,2,0,0,0,
0,29,0,109,110,112,36,44,38,40,137,489,490,101,138,139,187,137,33,137,0,19,137,
137,0,162,150,175,186,185,190,137,64,0,49,95,0,93,0,485,370,0,0,0,0,0,0,0,0,0,0,
0,101,0,0,471,137,0,0,458,0,0,456,457,0,387,389,402,410,412,414,416,418,420,423,
428,431,434,438,0,440,459,470,469,0,371,369,32,0,0,101,0,0,0,119,115,117,240,242,
0,0,0,9,10,491,492,200,86,0,0,154,67,220,0,217,0,0,0,3,0,5,0,30,0,0,137,20,0,137,
107,0,100,189,0,188,0,0,0,0,0,0,0,0,0,0,0,0,0,485,275,277,283,279,0,281,263,264,
265,0,266,267,268,0,34,21,112,207,223,0,0,203,201,0,247,0,184,0,192,52,191,0,0,
96,92,0,0,365,0,0,137,448,137,404,438,0,405,406,0,0,0,0,0,0,137,441,442,137,0,0,
0,444,445,443,0,362,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,392,393,394,395,396,
397,398,399,400,401,391,0,446,0,465,466,0,0,0,473,0,101,377,378,0,375,0,224,0,0,
0,137,243,215,0,120,125,121,123,116,118,137,0,249,241,250,494,493,0,81,82,83,58,
57,0,55,155,137,219,151,249,221,163,164,137,80,137,183,182,26,0,28,0,111,113,226,
225,22,98,108,0,0,0,127,128,130,0,101,101,0,0,0,0,0,0,137,0,312,313,314,0,0,316,
0,0,0,0,0,0,284,280,112,282,278,276,208,137,0,0,214,202,209,158,0,0,137,203,160,
137,161,156,184,193,53,102,0,97,409,94,90,373,374,0,0,0,227,0,0,231,0,0,0,236,0,
0,0,0,0,137,0,0,0,472,479,0,478,388,411,0,413,415,417,419,421,422,426,427,424,
425,429,430,433,432,435,436,437,390,464,461,0,463,0,137,0,366,0,372,31,384,0,380,
385,0,382,367,0,0,244,126,122,124,0,203,0,184,0,251,137,137,0,262,246,0,0,0,119,
0,101,137,137,0,173,152,218,4,6,0,131,114,0,0,203,0,134,0,0,0,291,0,0,0,0,0,311,
315,0,0,0,270,0,0,0,274,137,23,201,137,211,0,0,204,0,159,207,195,248,157,193,176,
0,7,0,447,0,0,239,0,228,0,232,235,237,452,0,0,0,0,0,0,0,0,439,475,0,0,462,460,0,
0,376,379,381,368,0,245,216,258,0,252,253,184,0,203,0,184,0,0,0,0,54,56,0,165,0,
203,0,0,184,0,129,132,137,101,144,0,0,0,0,0,0,0,0,0,0,0,0,0,291,317,0,0,0,0,24,
203,0,212,210,0,203,137,194,0,8,0,0,0,238,229,233,0,0,0,0,407,408,451,477,0,480,
403,474,476,0,383,386,257,255,260,0,261,254,184,0,61,59,60,166,169,0,171,172,167,
184,0,135,146,145,0,0,140,285,291,0,0,0,137,0,0,319,320,322,324,326,328,330,332,
335,340,343,346,350,352,359,360,0,137,288,297,0,0,0,0,0,0,0,0,0,0,318,0,0,0,273,
197,213,205,196,201,13,0,0,450,449,0,234,453,137,0,467,0,259,256,0,170,168,99,
133,147,144,144,0,0,291,137,356,137,0,353,137,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,354,292,0,299,0,0,0,307,0,0,0,0,0,0,290,269,272,0,0,203,12,14,0,0,0,0,468,
0,137,143,142,286,287,0,0,0,0,291,293,323,0,325,327,329,331,333,334,338,339,336,
337,341,342,345,344,347,348,349,0,298,300,301,0,309,308,0,303,0,0,0,0,206,198,15,
0,0,137,0,0,289,355,0,351,294,0,137,302,310,304,305,0,0,230,454,0,62,148,0,0,321,
291,295,306,271,0,358,357,296,455,0,0,0};static short Cyc_yydefgoto[141]={976,46,
628,817,47,48,264,49,417,50,419,51,132,52,53,479,207,401,402,208,56,219,209,58,
148,149,59,145,60,240,241,115,116,117,242,210,386,429,430,431,61,62,129,765,766,
767,63,432,64,406,65,66,67,106,143,246,291,625,558,68,559,471,617,463,467,468,
381,284,227,87,88,495,422,496,497,498,499,211,286,287,560,392,267,268,269,270,
271,272,696,273,274,775,776,777,778,779,780,781,782,783,784,785,786,787,788,789,
790,370,371,372,548,549,550,275,178,358,179,485,180,181,182,183,184,185,186,187,
188,189,190,191,192,193,194,511,512,195,196,70,818,220,396};static short Cyc_yypact[
979]={2594,- -32768,- -32768,- -32768,- -32768,- 36,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,3031,159,868,- -32768,67,- 18,4,13,45,- 13,73,- -32768,- -32768,111,- -32768,-
-32768,- -32768,- -32768,171,613,115,176,202,202,190,- -32768,2474,- -32768,322,541,
- -32768,239,3031,3031,3031,- -32768,3031,- -32768,- -32768,568,- -32768,280,3098,550,
256,103,593,- -32768,- -32768,247,249,220,- -32768,67,275,- -32768,1153,68,- -32768,-
-32768,- -32768,4959,- -32768,287,321,1153,330,346,352,- -32768,145,- -32768,- -32768,
5453,5453,5453,2474,2474,210,366,169,374,210,5706,1980,- -32768,- -32768,- -32768,-
-32768,2474,2713,2474,2713,- -32768,388,394,- -32768,2963,- -32768,- -32768,- -32768,
- -32768,5572,- -32768,- -32768,247,- -32768,- -32768,130,1604,- -32768,3098,239,-
-32768,3160,5453,3781,- -32768,550,- -32768,- -32768,- -32768,163,5453,- -32768,2713,
- -32768,398,411,404,67,- 10,- -32768,1153,183,5145,413,4959,5021,418,439,450,454,
458,247,5207,5207,- -32768,2102,464,5269,- -32768,5269,5269,- -32768,- -32768,285,-
-32768,- -32768,- 40,504,462,469,472,685,105,755,241,218,- -32768,962,5269,167,- 22,
- -32768,500,1077,520,- -32768,- -32768,533,4959,247,1077,528,95,5468,5810,5468,293,
- -32768,- 29,- 29,- 29,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,27,542,-
-32768,- -32768,488,326,- -32768,561,- 2,544,- -32768,575,- -32768,580,- -32768,169,
3874,3098,- -32768,584,5587,- -32768,768,604,- -32768,67,- -32768,588,225,606,3311,
617,626,631,639,3936,3311,- 66,35,628,607,- -32768,- -32768,1477,1477,239,1477,-
-32768,- -32768,- -32768,651,- -32768,- -32768,- -32768,350,- -32768,- -32768,644,654,-
-32768,- 24,658,693,373,712,701,- 6,593,727,- -32768,754,- -32768,- 6,764,67,- -32768,
4959,770,- -32768,752,759,2102,- -32768,5706,- -32768,- -32768,3409,- -32768,804,4959,
4959,4959,4959,4959,776,2102,- -32768,- -32768,1731,779,390,4959,- -32768,- -32768,-
-32768,4959,- -32768,5269,4959,5269,5269,5269,5269,5269,5269,5269,5269,5269,5269,
5269,5269,5269,5269,5269,5269,5269,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,4959,- -32768,210,- -32768,- -32768,
3998,210,4959,- -32768,786,247,- -32768,- -32768,798,802,1153,- -32768,402,3474,800,
5453,- -32768,810,805,- -32768,5810,5810,5810,- -32768,- -32768,2846,4060,144,-
-32768,595,- -32768,- -32768,- 24,- -32768,- -32768,- -32768,- -32768,809,817,812,-
-32768,2779,- -32768,633,372,- -32768,- -32768,- -32768,5706,- -32768,2224,- -32768,-
-32768,- -32768,2474,- -32768,2474,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,733,4959,824,816,- -32768,815,540,247,327,4959,4959,821,829,4959,905,1858,
834,- -32768,- -32768,- -32768,431,898,- -32768,4153,3311,844,850,4959,3311,- -32768,
- -32768,2963,- -32768,- -32768,- -32768,- -32768,5706,835,127,- -32768,- -32768,822,-
-32768,- 24,837,5691,693,- -32768,5453,- -32768,- -32768,593,449,- -32768,- -32768,
2354,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,839,843,859,- -32768,210,
4959,- -32768,724,3874,141,- -32768,446,847,848,849,451,5706,856,851,5083,- -32768,
- -32768,857,852,- -32768,504,177,462,469,472,685,105,105,755,755,755,755,241,241,
218,218,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,860,- -32768,18,5453,3688,
- -32768,3301,- -32768,- -32768,- -32768,1153,- -32768,- -32768,854,862,- -32768,185,
865,- -32768,- -32768,- -32768,- -32768,863,693,864,593,855,595,3222,5453,4215,-
-32768,- -32768,745,872,27,1125,873,247,3160,5453,4308,- -32768,633,- -32768,-
-32768,- -32768,813,- -32768,- -32768,768,4959,693,67,870,461,468,4959,825,493,869,
4401,4463,515,- -32768,- -32768,882,884,879,- -32768,874,875,517,- -32768,3098,-
-32768,887,5453,- -32768,889,- 24,- -32768,876,- -32768,92,- -32768,701,- -32768,449,-
-32768,892,- -32768,2474,894,98,890,- -32768,888,- -32768,3502,- -32768,- -32768,-
-32768,- -32768,4959,980,4959,5021,897,894,899,3688,- -32768,- -32768,4959,4959,-
-32768,- -32768,- 6,748,- -32768,- -32768,- -32768,- -32768,3567,- -32768,- -32768,-
-32768,900,- -32768,- -32768,593,902,693,- 6,593,891,592,903,904,- -32768,- -32768,
908,- -32768,910,693,913,- 6,593,907,- -32768,- -32768,5572,247,853,3311,922,919,
3188,918,927,3311,4959,4525,603,4587,621,4649,825,- -32768,930,942,81,932,- -32768,
693,253,- -32768,- -32768,939,693,5706,- -32768,298,- -32768,928,929,4959,- -32768,-
-32768,- -32768,3874,610,931,945,- -32768,804,- -32768,- -32768,756,- -32768,- -32768,
- -32768,- -32768,3688,- -32768,- -32768,- -32768,- -32768,- -32768,943,- -32768,- -32768,
593,961,- -32768,- -32768,- -32768,- -32768,- -32768,946,- -32768,- -32768,- -32768,593,
951,- -32768,- -32768,- -32768,952,67,637,1030,825,955,5331,953,2224,5269,947,-
-32768,17,- -32768,988,948,739,803,113,819,325,250,- -32768,- -32768,- -32768,-
-32768,995,5269,1731,- -32768,- -32768,641,3311,646,4711,3311,690,4773,4835,697,
968,- -32768,3311,3311,977,- -32768,- -32768,- -32768,969,- -32768,887,- -32768,971,
698,- -32768,- -32768,198,- -32768,- -32768,5706,1052,- -32768,3595,- -32768,- -32768,
972,- -32768,- -32768,- -32768,- -32768,973,28,853,3311,974,825,2102,- -32768,5706,
975,- -32768,1350,5269,4959,5269,5269,5269,5269,5269,5269,5269,5269,5269,5269,
5269,5269,5269,5269,5269,5269,5269,4959,- -32768,- -32768,979,- -32768,3311,3311,
699,- -32768,3311,3311,707,3311,709,4897,- -32768,- -32768,- -32768,959,- 24,693,-
-32768,- -32768,579,4959,978,981,- -32768,990,5706,- -32768,- -32768,- -32768,- -32768,
983,982,986,5269,825,- -32768,504,294,462,469,469,685,105,105,755,755,755,755,241,
241,218,218,- -32768,- -32768,- -32768,315,- -32768,- -32768,- -32768,3311,- -32768,-
-32768,3311,- -32768,3311,3311,711,994,- -32768,- -32768,- -32768,787,985,5706,987,
713,- -32768,894,331,- -32768,- -32768,4959,1350,- -32768,- -32768,- -32768,- -32768,
3311,3311,- -32768,- -32768,991,- -32768,- -32768,992,993,- -32768,825,- -32768,-
-32768,- -32768,996,- -32768,- -32768,- -32768,- -32768,1095,1097,- -32768};static
short Cyc_yypgoto[141]={- -32768,276,- -32768,214,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- 49,- 89,51,- -32768,- -32768,0,535,- -32768,101,- 63,1001,31,
- -32768,- -32768,- 123,- -32768,- 94,- -32768,419,- -32768,- -32768,- -32768,877,858,119,
406,- -32768,- -32768,522,- -32768,- -32768,- -32768,59,- -32768,- -32768,636,- 126,1045,
- 356,226,- -32768,- -32768,1067,- 47,- -32768,- -32768,490,- 110,- 119,- 87,- 384,227,497,
502,- 379,- 430,- 100,- 346,- 113,- -32768,- 198,- 148,- 510,- 335,- -32768,624,- 151,- 35,-
121,- 58,- 269,192,- -32768,- -32768,- 56,- 252,- -32768,- 229,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,180,921,- -32768,463,- -32768,- -32768,317,
612,- -32768,- 157,- 419,- 142,- 325,- 322,- 275,796,- 314,- 308,- 93,- 295,- 293,36,442,435,
- -32768,- -32768,- 343,- -32768,108,438,- 55,1070,- 358,16};static short Cyc_yytable[
5922]={54,305,534,514,226,125,537,134,516,584,247,308,456,457,131,459,568,309,
320,519,536,54,285,293,292,282,520,521,298,238,655,57,244,763,764,615,464,449,
328,421,101,546,450,393,399,526,527,54,283,528,529,579,57,54,54,54,517,54,71,212,
213,214,239,54,138,580,393,507,366,131,229,315,73,34,93,633,277,579,57,329,465,
265,154,276,57,57,57,380,57,621,97,618,394,395,57,847,367,54,54,405,475,55,155,
414,119,120,121,604,122,376,54,54,54,54,133,394,94,612,54,400,395,563,55,662,326,
95,580,451,57,57,54,90,54,- 141,653,632,848,735,563,138,484,57,57,57,57,384,54,
452,55,57,453,489,391,490,55,55,55,96,55,33,154,57,466,57,55,506,36,408,687,133,
410,- 223,483,808,- 223,664,98,57,336,337,155,266,424,133,462,90,855,856,410,276,
217,721,34,809,90,281,218,473,55,55,379,- 201,99,89,688,- 201,103,323,383,324,325,
55,55,55,55,265,265,338,265,55,43,44,45,225,34,857,339,34,359,397,398,55,614,55,
858,360,361,362,54,385,245,477,243,522,523,524,525,55,637,203,693,33,388,204,225,
493,494,153,36,389,34,33,90,74,54,54,199,54,36,57,265,484,722,290,540,363,466,40,
573,100,300,364,365,326,104,746,43,44,45,133,651,480,140,301,661,57,57,756,57,
636,727,573,217,476,326,90,105,395,218,481,108,890,90,226,147,266,266,33,266,54,
384,384,384,109,36,546,382,811,387,127,128,33,814,437,299,344,438,484,36,55,588,
590,114,552,345,346,380,40,33,574,57,466,410,146,644,36,43,44,45,144,243,342,812,
343,40,55,55,863,55,610,266,815,410,215,216,395,864,865,410,151,530,531,532,383,
383,383,231,232,233,234,33,326,567,598,327,197,35,36,37,177,326,388,611,816,572,
727,578,951,389,131,43,44,45,198,385,385,385,54,654,54,55,326,294,217,964,110,
111,583,952,218,484,623,411,200,380,412,390,726,69,144,622,589,54,441,670,861,
678,862,57,448,57,407,201,461,668,683,326,678,54,202,75,91,682,92,33,680,16,17,
18,433,35,36,37,57,221,805,281,669,69,472,404,90,54,223,90,69,321,389,681,57,69,
235,727,133,737,731,326,466,124,509,236,732,939,69,295,544,618,225,326,133,624,
297,150,57,666,91,296,574,55,375,55,304,906,225,91,711,310,908,410,822,225,410,
225,716,601,69,69,326,911,69,839,868,726,55,648,912,913,311,69,69,69,69,326,543,
710,639,547,326,312,55,643,276,313,567,918,919,314,326,920,921,691,69,322,447,
326,909,910,692,712,578,33,330,225,55,69,331,35,36,37,150,965,225,91,332,904,762,
404,333,303,326,306,306,697,389,368,43,44,45,317,318,54,900,373,306,390,306,306,
905,703,321,744,326,844,326,748,374,709,500,54,726,407,504,33,321,306,91,378,759,
35,36,37,57,91,605,112,113,515,609,427,415,90,403,39,433,90,624,141,142,135,57,
33,133,136,409,125,137,35,36,37,413,738,33,123,891,69,950,102,35,36,37,416,409,
538,225,435,418,747,439,118,425,901,382,902,581,225,582,436,39,750,758,967,751,
829,564,69,33,799,565,434,326,566,55,36,832,440,39,326,230,443,823,455,40,657,
968,802,442,658,326,886,55,43,44,45,150,444,39,454,222,974,306,836,575,445,837,
265,576,462,326,577,289,869,237,326,591,592,871,460,595,627,599,334,335,914,915,
916,917,466,469,90,278,306,608,306,306,306,306,306,306,306,306,306,306,306,306,
306,306,306,306,306,555,556,557,961,54,966,945,326,265,433,875,880,888,90,326,
889,326,470,243,929,433,845,91,433,326,91,326,932,326,934,411,957,474,963,395,57,
33,340,341,867,634,635,35,36,37,125,39,225,673,674,675,547,427,409,478,266,409,
54,694,695,423,43,44,45,739,740,69,39,69,763,764,33,826,827,851,852,409,35,36,37,
482,306,409,278,487,791,486,427,57,853,854,488,428,328,768,505,43,44,45,508,795,
859,860,959,326,55,896,897,266,539,922,923,924,458,265,720,33,76,541,542,551,554,
35,36,37,700,702,553,569,571,423,69,427,586,501,502,503,570,585,587,593,43,44,45,
594,510,596,306,600,513,949,602,606,607,613,616,77,619,55,629,630,306,631,54,640,
641,642,646,728,650,659,78,79,34,80,81,645,649,36,660,652,533,667,663,665,690,
510,82,698,83,84,91,676,679,57,91,85,704,705,706,870,281,708,874,86,713,707,719,
715,647,882,883,562,723,266,724,729,733,749,734,743,409,745,752,753,225,796,798,
754,801,755,804,409,757,760,409,510,769,689,770,306,898,793,794,806,807,810,813,
819,820,824,821,347,348,349,350,351,352,353,354,355,356,825,828,55,830,831,833,
834,838,840,846,843,510,927,928,849,69,930,931,357,933,850,866,881,884,885,887,
892,894,899,937,895,926,944,903,306,958,942,946,943,177,947,948,306,960,977,962,
978,91,426,971,972,973,940,224,975,677,761,686,423,139,107,938,420,718,76,873,
717,714,877,879,953,638,742,954,377,955,956,518,792,126,0,91,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,969,970,0,423,0,77,0,0,124,321,0,0,0,22,0,0,907,369,0,0,
152,79,0,80,81,- 137,- 137,36,0,672,0,27,28,29,925,0,82,0,83,84,685,0,76,32,0,85,0,
0,936,0,0,0,0,86,0,835,510,0,941,0,0,0,0,0,842,0,0,306,33,0,0,0,0,34,35,36,37,38,
39,0,0,77,0,0,427,306,41,42,0,0,0,0,0,43,44,45,152,79,423,80,81,0,0,36,0,730,0,0,
0,0,423,0,82,736,83,84,0,0,0,0,0,85,0,0,0,0,0,0,0,86,0,0,0,0,0,0,0,0,0,0,306,0,
306,306,306,306,306,306,306,306,306,306,306,306,306,306,306,306,306,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,124,0,0,0,0,0,0,0,0,0,0,0,423,0,0,0,0,0,306,0,0,0,0,0,
0,423,2,3,4,72,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,694,695,21,156,157,248,0,
249,250,251,252,253,254,255,256,22,76,23,158,257,0,306,159,24,258,0,0,0,0,160,
161,162,163,27,28,29,259,164,0,260,165,0,0,0,0,32,0,166,167,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,423,0,0,261,79,168,80,81,34,35,36,37,38,39,262,130,- 291,0,0,169,
0,41,228,0,0,0,0,0,171,0,0,172,0,173,0,0,174,0,0,175,176,2,3,4,72,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,0,21,156,157,248,0,249,250,251,252,253,254,255,256,
22,76,23,158,257,0,0,159,24,258,0,0,- 137,- 137,160,161,162,163,27,28,29,259,164,0,
260,165,0,0,0,0,32,0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,261,79,
168,80,81,34,35,36,37,38,39,262,130,0,0,0,169,0,41,228,0,0,0,0,0,171,0,0,172,0,
173,0,0,174,0,0,175,176,2,3,4,72,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,
156,157,248,0,249,250,251,252,253,254,255,256,22,76,23,158,257,0,0,159,24,258,0,
0,0,0,160,161,162,163,27,28,29,259,164,0,260,165,0,0,0,0,32,0,166,167,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,261,79,168,80,81,34,35,36,37,38,39,262,130,263,0,
0,169,0,41,228,0,0,0,0,0,171,0,0,172,0,173,0,0,174,0,0,175,176,2,3,4,72,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,156,157,248,0,249,250,251,252,253,254,
255,256,22,76,23,158,257,0,0,159,24,258,0,0,0,0,160,161,162,163,27,28,29,259,164,
0,260,165,0,0,0,0,32,0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,261,79,
168,80,81,34,35,36,37,38,39,262,130,0,0,0,169,0,41,228,0,0,0,0,0,171,0,0,172,0,
173,0,0,174,0,0,175,176,2,3,4,72,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,
156,157,0,0,0,0,0,0,0,0,0,0,22,76,23,158,0,0,0,159,24,0,0,0,0,0,160,161,162,163,
27,28,29,0,164,0,0,165,0,0,0,0,32,0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,33,79,168,80,81,34,35,36,37,38,39,597,0,0,0,0,169,0,41,228,0,0,0,0,0,171,0,
0,172,0,173,0,0,174,0,0,175,176,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,156,
157,0,0,0,0,0,0,0,0,0,0,22,76,0,158,0,0,0,159,0,0,0,0,0,0,160,161,162,163,27,28,
29,0,164,205,0,165,0,0,0,0,32,0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,33,79,168,80,81,34,35,36,37,38,39,0,206,0,0,0,169,0,41,228,0,0,0,0,0,171,0,0,
172,0,173,0,0,174,0,0,175,176,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,156,
157,0,0,0,0,0,0,0,0,0,0,22,76,0,158,0,0,0,159,0,0,0,0,0,0,160,161,162,163,27,28,
29,0,164,0,0,165,0,0,0,0,32,0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
33,79,168,80,81,34,35,36,37,38,39,0,319,0,0,0,169,0,41,228,0,0,0,0,0,171,0,0,172,
0,173,0,0,174,0,0,175,176,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,156,157,0,
0,0,0,0,0,0,0,0,0,22,76,0,158,0,0,0,159,0,0,0,0,0,0,160,161,162,163,27,28,29,0,
164,0,0,165,0,0,0,0,32,0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,33,79,
168,80,81,34,35,36,37,38,39,0,0,0,0,0,169,0,41,228,0,0,0,0,0,171,0,0,172,0,173,0,
0,174,0,0,175,176,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,626,0,24,0,25,26,- 137,- 137,0,0,0,0,27,
28,29,0,0,0,0,0,0,0,30,31,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,33,
0,0,0,0,34,35,36,37,38,39,0,0,- 11,0,0,40,0,41,42,0,0,0,0,0,43,44,45,- 11,1,0,2,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,
23,0,0,0,0,0,24,0,25,26,- 137,- 137,0,0,0,0,27,28,29,0,0,0,0,0,0,0,30,31,32,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,33,0,0,0,0,34,35,36,37,38,39,0,0,- 11,0,
0,40,0,41,42,0,0,0,0,0,43,44,45,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,- 137,- 137,0,
0,0,0,27,28,29,0,0,0,0,0,0,0,30,31,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,33,0,0,0,0,34,35,36,37,38,39,0,0,0,0,0,40,0,41,42,0,0,0,0,0,43,44,45,1,0,2,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,
0,23,0,0,0,0,0,24,0,25,26,- 137,- 137,0,0,0,0,27,28,29,0,0,0,0,0,0,0,30,31,32,0,0,
0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,33,0,0,0,0,34,35,36,37,38,
39,22,0,- 11,0,0,40,0,41,42,0,0,0,0,0,43,44,45,0,27,28,29,0,0,0,0,0,0,0,0,0,32,0,
0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,279,0,0,33,0,0,0,0,34,35,36,
37,38,39,281,22,0,0,0,404,- 201,41,42,0,- 201,0,389,0,43,44,45,0,0,27,28,29,0,0,0,
0,0,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,279,0,0,0,0,0,0,0,34,35,
0,37,38,39,281,0,0,0,0,388,- 201,41,42,0,- 201,0,389,0,43,44,45,2,3,4,72,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,
0,24,0,0,0,- 137,- 137,0,0,0,0,27,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,2,3,4,72,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,34,35,0,37,38,39,0,130,22,0,
237,0,0,41,42,0,24,0,0,0,- 137,- 137,0,0,0,0,27,28,29,0,0,0,0,0,0,0,0,0,32,0,0,2,3,
4,72,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,34,35,0,37,38,39,
0,22,0,23,0,0,0,41,42,24,0,0,0,0,0,0,0,0,0,27,28,29,0,0,0,0,0,0,0,0,0,32,0,0,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,34,35,22,37,38,39,0,
130,0,0,0,0,0,41,42,0,0,0,771,772,27,28,29,0,0,0,0,0,0,0,0,76,32,0,0,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,0,0,0,0,279,0,0,280,0,0,0,0,34,35,22,37,38,39,281,0,
77,0,0,0,- 201,41,42,0,- 201,0,0,0,27,28,29,152,79,0,80,81,0,0,36,0,32,0,0,0,0,0,0,
773,0,83,84,0,0,0,0,0,85,0,0,172,0,774,279,0,86,0,0,175,176,0,34,35,0,37,38,39,
281,0,0,0,0,0,- 201,41,42,0,- 201,156,157,248,76,249,250,251,252,253,254,255,256,0,
76,0,158,257,0,0,159,0,258,0,0,0,0,160,161,162,163,0,0,0,259,164,0,260,165,0,0,0,
0,77,0,166,167,0,0,0,0,0,0,0,0,656,0,0,152,79,0,80,81,0,0,36,0,0,261,79,168,80,
81,0,82,36,83,84,0,262,130,0,0,85,169,0,0,170,0,0,0,86,0,171,0,0,172,0,173,156,
157,174,0,0,175,176,491,0,0,0,0,0,76,0,158,0,0,0,159,0,0,0,0,0,0,160,161,162,163,
0,0,0,0,164,0,0,165,0,0,0,0,0,0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,33,79,168,80,81,0,0,36,0,0,0,76,307,492,0,0,169,0,0,170,0,0,493,494,0,171,156,
157,172,0,173,0,0,174,0,0,175,176,0,76,0,158,0,0,0,159,0,0,0,0,77,0,160,161,162,
163,0,0,0,0,164,0,545,165,0,152,79,0,80,81,166,167,36,0,0,0,0,0,0,0,0,82,0,83,84,
0,0,493,494,0,85,0,0,33,79,168,80,81,86,0,36,0,0,0,76,307,725,0,0,169,0,0,170,0,
0,493,494,0,171,156,157,172,0,173,0,0,174,0,0,175,176,0,76,0,158,0,0,0,159,0,0,0,
0,77,0,160,161,162,163,0,0,0,0,164,0,741,165,0,152,79,0,80,81,166,167,36,0,0,0,0,
0,0,0,0,82,0,83,84,0,0,493,494,0,85,0,0,33,79,168,80,81,86,0,36,0,0,0,0,307,893,
0,0,169,0,0,170,0,0,493,494,0,171,156,157,172,0,173,0,0,174,0,0,175,176,0,76,0,
158,0,0,0,159,0,0,0,0,0,0,160,161,162,163,0,0,0,0,164,0,0,165,0,0,0,0,0,0,166,
167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,33,79,168,80,81,0,0,36,0,0,0,0,307,
0,0,0,169,0,0,170,0,0,493,494,0,171,156,157,172,0,173,0,0,174,0,0,175,176,0,76,0,
158,0,0,0,159,0,0,0,0,0,0,160,161,162,163,0,0,0,0,164,0,0,165,0,0,0,0,0,0,166,
167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,33,79,168,80,81,0,0,36,0,0,0,0,0,0,
0,0,169,0,0,170,0,0,0,0,288,171,156,157,172,0,173,0,0,174,0,0,175,176,0,76,0,158,
0,0,0,159,0,0,0,0,0,0,160,161,162,163,0,0,0,0,164,0,0,165,0,0,0,0,0,0,166,167,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,0,0,33,79,168,80,81,0,0,36,76,0,158,0,
307,0,159,0,169,0,0,170,0,160,161,162,163,171,0,0,172,164,173,0,165,174,0,0,175,
176,0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,0,0,33,79,168,80,81,0,0,
36,76,0,158,446,0,0,159,0,169,0,0,170,0,160,161,162,163,171,0,0,172,164,173,0,
165,174,0,0,175,176,0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,0,0,33,
79,168,80,81,0,0,36,76,0,158,0,0,0,159,0,169,535,0,170,0,160,161,162,163,171,0,0,
172,164,173,0,165,174,0,0,175,176,0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,33,79,168,80,81,0,0,36,0,0,0,0,0,0,0,0,169,0,0,170,0,0,0,0,561,171,156,157,
172,0,173,0,0,174,0,0,175,176,0,76,0,158,0,0,0,159,0,0,0,0,0,0,160,161,162,163,0,
0,0,0,164,0,0,165,0,0,0,0,0,0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,
0,0,33,79,168,80,81,0,0,36,76,0,158,0,0,0,159,0,169,603,0,170,0,160,161,162,163,
171,0,0,172,164,173,0,165,174,0,0,175,176,0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,33,79,168,80,81,0,0,36,0,0,0,0,0,0,0,0,169,0,0,170,0,0,0,0,671,171,
156,157,172,0,173,0,0,174,0,0,175,176,0,76,0,158,0,0,0,159,0,0,0,0,0,0,160,161,
162,163,0,0,0,0,164,0,0,165,0,0,0,0,0,0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,33,79,168,80,81,0,0,36,0,0,0,0,0,0,0,0,169,0,0,170,0,0,0,0,684,171,156,
157,172,0,173,0,0,174,0,0,175,176,0,76,0,158,0,0,0,159,0,0,0,0,0,0,160,161,162,
163,0,0,0,0,164,0,0,165,0,0,0,0,0,0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,
157,0,0,0,33,79,168,80,81,0,0,36,76,0,158,699,0,0,159,0,169,0,0,170,0,160,161,
162,163,171,0,0,172,164,173,0,165,174,0,0,175,176,0,166,167,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,156,157,0,0,0,33,79,168,80,81,0,0,36,76,0,158,701,0,0,159,0,169,0,0,
170,0,160,161,162,163,171,0,0,172,164,173,0,165,174,0,0,175,176,0,166,167,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,0,0,33,79,168,80,81,0,0,36,76,0,158,0,0,0,
159,0,169,797,0,170,0,160,161,162,163,171,0,0,172,164,173,0,165,174,0,0,175,176,
0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,0,0,33,79,168,80,81,0,0,36,
76,0,158,0,0,0,159,0,169,800,0,170,0,160,161,162,163,171,0,0,172,164,173,0,165,
174,0,0,175,176,0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,0,0,33,79,
168,80,81,0,0,36,76,0,158,803,0,0,159,0,169,0,0,170,0,160,161,162,163,171,0,0,
172,164,173,0,165,174,0,0,175,176,0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,
157,0,0,0,33,79,168,80,81,0,0,36,76,0,158,0,0,0,159,0,169,872,0,170,0,160,161,
162,163,171,0,0,172,164,173,0,165,174,0,0,175,176,0,166,167,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,156,157,0,0,0,33,79,168,80,81,0,0,36,76,0,158,0,0,0,159,0,169,876,0,
170,0,160,161,162,163,171,0,0,172,164,173,0,165,174,0,0,175,176,0,166,167,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,0,0,33,79,168,80,81,0,0,36,76,0,158,0,0,0,
159,0,169,878,0,170,0,160,161,162,163,171,0,0,172,164,173,0,165,174,0,0,175,176,
0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,0,0,33,79,168,80,81,0,0,36,
76,0,158,0,0,0,159,0,169,935,0,170,0,160,161,162,163,171,0,0,172,164,173,0,165,
174,0,0,175,176,0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,0,0,33,79,
168,80,81,0,0,36,76,0,0,0,0,0,0,0,169,0,0,170,0,160,161,162,163,171,0,0,172,0,
173,0,165,174,0,0,175,176,0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,0,
0,33,79,168,80,81,0,0,36,76,0,0,0,307,0,0,0,169,0,0,170,0,160,161,162,163,171,0,
0,172,0,173,0,165,174,0,0,175,176,0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,
157,0,0,0,33,79,168,80,81,0,0,36,76,0,0,0,647,0,0,0,169,0,0,170,0,160,161,162,
163,171,0,0,172,0,173,0,165,174,0,0,175,176,0,166,167,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,156,157,0,0,0,33,79,168,80,81,0,0,36,76,0,0,0,0,0,0,0,302,0,0,170,0,160,
161,162,163,171,0,0,172,0,173,0,165,174,0,0,175,176,0,166,167,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,156,157,0,0,0,33,79,168,80,81,0,0,36,76,0,0,0,0,0,0,0,316,0,0,170,
0,160,161,162,163,171,0,0,172,0,173,0,165,174,0,0,175,176,0,166,167,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,156,157,0,0,0,33,79,168,80,81,0,0,36,76,0,0,0,0,0,0,0,169,0,
0,170,0,160,161,162,163,171,0,0,172,0,173,0,165,174,0,0,175,176,0,166,167,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,33,79,168,80,81,0,0,36,0,0,0,0,0,0,0,0,841,0,
0,170,0,0,0,0,0,171,0,0,172,0,173,0,0,174,0,0,175,176,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,22,0,0,27,28,29,0,0,205,0,0,0,- 137,- 137,0,32,0,0,27,28,29,0,0,0,0,0,0,0,0,0,
32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,35,0,37,38,39,0,206,0,0,0,0,0,41,42,34,35,0,37,
38,39,0,0,0,0,0,0,0,41,42,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,27,28,29,0,0,0,
0,0,0,0,0,0,32,0,0,27,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,
35,0,37,38,39,0,0,- 105,0,0,0,0,41,42,34,35,0,37,38,39,0,0,- 106,0,0,0,0,41,42,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,27,28,29,0,0,0,0,0,0,0,0,0,32,0,0,27,28,29,0,0,
0,0,0,0,0,0,0,32,0,0,0,0,0,0,620,0,0,0,0,0,0,0,34,35,0,37,38,39,0,0,0,0,0,0,0,41,
42,34,35,0,37,38,39,0,0,0,0,0,0,0,41,42,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,0,- 137,- 137,0,0,0,0,27,28,29,
0,0,0,0,0,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
34,0,0,0,38,39,0,0,0,0,0,0,0,41,42};static short Cyc_yycheck[5922]={0,158,360,328,
104,60,364,63,330,428,129,159,264,265,63,267,395,159,169,333,363,21,135,144,143,
135,334,335,151,118,540,0,126,5,6,465,60,103,78,237,40,376,108,72,17,340,341,47,
135,342,343,407,21,53,54,55,331,57,94,94,95,96,118,63,64,411,72,319,90,118,105,
165,21,97,92,494,132,433,47,119,104,130,92,132,53,54,55,206,57,473,103,470,121,
122,63,78,118,97,98,225,106,0,112,105,53,54,55,450,57,203,110,111,112,113,63,121,
112,463,118,92,122,390,21,553,106,112,472,92,97,98,130,23,132,105,116,493,119,
647,407,139,297,110,111,112,113,208,146,112,47,118,115,302,210,304,53,54,55,112,
57,92,92,130,281,132,63,316,99,225,587,118,225,103,295,92,106,559,103,146,73,74,
112,130,238,132,92,77,73,74,243,238,92,93,97,112,86,103,98,284,97,98,105,109,91,
23,588,113,91,171,208,173,174,110,111,112,113,264,265,112,267,118,117,118,119,
104,97,112,121,97,192,213,214,130,105,132,121,68,69,70,238,208,110,288,123,336,
337,338,339,146,107,104,593,92,108,108,135,114,115,77,99,115,97,92,154,104,264,
265,86,267,99,238,319,428,630,110,368,108,395,108,404,108,97,114,115,106,108,669,
117,118,119,238,113,291,66,110,109,264,265,681,267,497,635,427,92,287,106,197,
104,122,98,293,120,113,204,413,94,264,265,92,267,319,383,384,385,47,99,660,207,
711,209,49,50,92,716,108,154,117,111,494,99,238,434,435,103,378,126,127,465,108,
92,404,319,470,407,104,505,99,117,118,119,112,241,120,109,122,108,264,265,117,
267,458,319,717,427,97,98,122,126,127,433,104,344,345,346,383,384,385,110,111,
112,113,92,106,392,442,109,108,98,99,100,82,106,108,458,105,404,740,406,113,115,
458,117,118,119,92,383,384,385,417,539,419,319,106,146,92,93,103,104,427,113,98,
587,478,106,103,553,109,210,635,0,112,475,114,442,251,565,120,572,122,417,257,
419,225,106,103,564,576,106,583,458,107,22,23,575,25,92,575,17,18,19,243,98,99,
100,442,108,704,103,564,40,106,108,373,482,109,376,47,169,115,575,458,52,103,827,
442,651,643,106,616,60,109,106,643,886,65,106,103,885,388,106,458,479,107,74,482,
561,77,105,572,417,202,419,108,847,404,86,612,108,849,583,727,411,586,413,620,
103,97,98,106,852,101,769,793,740,442,508,853,854,108,110,111,112,113,106,373,
610,109,376,106,108,458,109,610,108,563,859,860,108,106,861,862,109,133,108,256,
106,850,851,109,613,579,92,77,463,482,146,123,98,99,100,151,948,472,154,124,846,
689,108,125,156,106,158,159,109,115,104,117,118,119,166,167,610,840,92,171,388,
173,174,846,103,302,667,106,773,106,671,92,109,310,628,827,404,314,92,316,192,
197,108,684,98,99,100,610,204,451,103,104,329,455,108,105,542,109,102,427,546,
624,63,64,108,628,92,610,112,225,719,115,98,99,100,108,654,92,104,824,236,904,40,
98,99,100,105,243,365,564,246,105,670,249,52,105,841,572,843,417,575,419,108,102,
106,683,952,109,749,108,266,92,103,112,104,106,115,610,99,760,108,102,106,105,92,
109,113,108,542,952,103,108,546,106,815,628,117,118,119,295,103,102,108,101,967,
297,103,108,103,106,793,112,92,106,115,137,109,107,106,436,437,109,105,440,482,
442,75,76,855,856,857,858,885,109,660,133,328,454,330,331,332,333,334,335,336,
337,338,339,340,341,342,343,344,345,346,383,384,385,943,793,951,895,106,846,572,
109,103,103,694,106,106,106,113,688,109,583,774,373,586,106,376,106,109,106,109,
106,109,109,109,122,793,92,71,72,792,105,106,98,99,100,889,102,717,92,93,94,660,
108,404,116,793,407,846,22,23,237,117,118,119,105,106,417,102,419,5,6,92,105,106,
124,125,427,98,99,100,105,428,433,236,121,694,105,108,846,75,76,121,113,78,691,
108,117,118,119,109,697,71,72,105,106,793,836,837,846,112,863,864,865,266,952,
628,92,38,109,106,109,105,98,99,100,597,598,106,108,106,307,482,108,106,311,312,
313,109,103,113,108,117,118,119,104,322,30,494,103,326,903,42,97,92,108,122,77,
109,846,109,106,508,92,952,106,106,106,105,640,106,105,92,93,97,95,96,109,109,99,
106,109,358,116,109,109,104,363,108,108,110,111,542,109,109,952,546,117,104,103,
109,797,103,116,800,125,105,121,104,121,104,807,808,389,112,952,116,25,109,116,
109,109,572,109,109,109,895,698,699,109,701,109,703,583,109,116,586,413,104,589,
109,587,838,113,105,103,92,103,97,109,109,108,723,79,80,81,82,83,84,85,86,87,88,
106,109,952,93,109,105,105,28,104,113,108,450,871,872,77,628,875,876,107,878,123,
77,105,97,106,105,25,106,105,121,108,103,93,109,643,92,109,105,108,773,109,106,
651,109,0,109,0,660,241,109,109,109,889,103,109,571,688,586,497,65,44,885,236,
624,38,799,620,616,802,803,929,498,660,932,204,934,935,332,694,60,- 1,694,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,957,958,- 1,540,- 1,77,- 1,- 1,719,841,- 1,- 1,
- 1,37,- 1,- 1,848,89,- 1,- 1,92,93,- 1,95,96,49,50,99,- 1,566,- 1,55,56,57,866,- 1,108,-
1,110,111,577,- 1,38,67,- 1,117,- 1,- 1,880,- 1,- 1,- 1,- 1,125,- 1,766,593,- 1,890,- 1,- 1,
- 1,- 1,- 1,771,- 1,- 1,774,92,- 1,- 1,- 1,- 1,97,98,99,100,101,102,- 1,- 1,77,- 1,- 1,108,
792,110,111,- 1,- 1,- 1,- 1,- 1,117,118,119,92,93,635,95,96,- 1,- 1,99,- 1,642,- 1,- 1,- 1,
- 1,647,- 1,108,650,110,111,- 1,- 1,- 1,- 1,- 1,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,847,- 1,849,850,851,852,853,854,855,856,857,858,859,860,861,
862,863,864,865,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,889,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,727,- 1,- 1,- 1,- 1,- 1,903,- 1,- 1,- 1,- 1,- 1,- 1,740,3,4,5,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,
34,35,36,37,38,39,40,41,- 1,951,44,45,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,59,-
1,61,62,- 1,- 1,- 1,- 1,67,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,827,- 1,- 1,92,93,94,95,96,97,98,99,100,101,102,103,104,105,- 1,- 1,108,- 1,110,
111,- 1,- 1,- 1,- 1,- 1,117,- 1,- 1,120,- 1,122,- 1,- 1,125,- 1,- 1,128,129,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,
38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,54,55,56,57,58,59,- 1,61,62,- 1,- 1,
- 1,- 1,67,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
92,93,94,95,96,97,98,99,100,101,102,103,104,- 1,- 1,- 1,108,- 1,110,111,- 1,- 1,- 1,- 1,
- 1,117,- 1,- 1,120,- 1,122,- 1,- 1,125,- 1,- 1,128,129,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,
- 1,44,45,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,- 1,67,- 1,69,
70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,93,94,95,96,
97,98,99,100,101,102,103,104,105,- 1,- 1,108,- 1,110,111,- 1,- 1,- 1,- 1,- 1,117,- 1,- 1,
120,- 1,122,- 1,- 1,125,- 1,- 1,128,129,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,
- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,- 1,67,- 1,69,70,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,93,94,95,96,97,98,99,
100,101,102,103,104,- 1,- 1,- 1,108,- 1,110,111,- 1,- 1,- 1,- 1,- 1,117,- 1,- 1,120,- 1,122,
- 1,- 1,125,- 1,- 1,128,129,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,- 1,- 1,
51,52,53,54,55,56,57,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,67,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,93,94,95,96,97,98,99,100,101,102,
103,- 1,- 1,- 1,- 1,108,- 1,110,111,- 1,- 1,- 1,- 1,- 1,117,- 1,- 1,120,- 1,122,- 1,- 1,125,- 1,
- 1,128,129,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,- 1,
59,60,- 1,62,- 1,- 1,- 1,- 1,67,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,92,93,94,95,96,97,98,99,100,101,102,- 1,104,- 1,- 1,- 1,108,- 1,110,
111,- 1,- 1,- 1,- 1,- 1,117,- 1,- 1,120,- 1,122,- 1,- 1,125,- 1,- 1,128,129,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,
- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,67,
- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,93,94,
95,96,97,98,99,100,101,102,- 1,104,- 1,- 1,- 1,108,- 1,110,111,- 1,- 1,- 1,- 1,- 1,117,- 1,
- 1,120,- 1,122,- 1,- 1,125,- 1,- 1,128,129,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,53,54,55,56,57,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,67,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,93,94,95,96,97,98,99,100,101,102,
- 1,- 1,- 1,- 1,- 1,108,- 1,110,111,- 1,- 1,- 1,- 1,- 1,117,- 1,- 1,120,- 1,122,- 1,- 1,125,- 1,-
1,128,129,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,49,50,- 1,- 1,
- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,- 1,- 1,- 1,97,98,99,100,101,102,- 1,- 1,
105,- 1,- 1,108,- 1,110,111,- 1,- 1,- 1,- 1,- 1,117,118,119,0,1,- 1,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,
39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,
66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,
- 1,- 1,- 1,- 1,97,98,99,100,101,102,- 1,- 1,105,- 1,- 1,108,- 1,110,111,- 1,- 1,- 1,- 1,- 1,
117,118,119,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,
- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,- 1,- 1,- 1,97,98,99,100,101,102,- 1,- 1,- 1,
- 1,- 1,108,- 1,110,111,- 1,- 1,- 1,- 1,- 1,117,118,119,1,- 1,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,
- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,66,67,
- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,92,- 1,- 1,-
1,- 1,97,98,99,100,101,102,37,- 1,105,- 1,- 1,108,- 1,110,111,- 1,- 1,- 1,- 1,- 1,117,118,
119,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,89,- 1,- 1,92,- 1,- 1,- 1,- 1,97,98,99,100,101,102,103,37,-
1,- 1,- 1,108,109,110,111,- 1,113,- 1,115,- 1,117,118,119,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,100,101,102,103,- 1,- 1,- 1,- 1,108,109,110,111,- 1,
113,- 1,115,- 1,117,118,119,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,-
1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,- 1,- 1,49,
50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,- 1,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,97,98,- 1,100,101,102,- 1,
104,37,- 1,107,- 1,- 1,110,111,- 1,45,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,-
1,24,- 1,- 1,- 1,- 1,- 1,97,98,- 1,100,101,102,- 1,37,- 1,39,- 1,- 1,- 1,110,111,45,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,37,100,101,
102,- 1,104,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,25,26,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,38,67,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,89,- 1,- 1,92,
- 1,- 1,- 1,- 1,97,98,37,100,101,102,103,- 1,77,- 1,- 1,- 1,109,110,111,- 1,113,- 1,- 1,- 1,
55,56,57,92,93,- 1,95,96,- 1,- 1,99,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,110,111,- 1,- 1,- 1,
- 1,- 1,117,- 1,- 1,120,- 1,122,89,- 1,125,- 1,- 1,128,129,- 1,97,98,- 1,100,101,102,103,-
1,- 1,- 1,- 1,- 1,109,110,111,- 1,113,25,26,27,38,29,30,31,32,33,34,35,36,- 1,38,- 1,40,
41,- 1,- 1,44,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,58,59,- 1,61,62,- 1,- 1,- 1,- 1,77,
- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,- 1,- 1,92,93,- 1,95,96,- 1,- 1,99,- 1,- 1,92,93,94,
95,96,- 1,108,99,110,111,- 1,103,104,- 1,- 1,117,108,- 1,- 1,111,- 1,- 1,- 1,125,- 1,117,-
1,- 1,120,- 1,122,25,26,125,- 1,- 1,128,129,32,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,- 1,- 1,44,-
1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,93,94,95,96,- 1,- 1,
99,- 1,- 1,- 1,38,104,105,- 1,- 1,108,- 1,- 1,111,- 1,- 1,114,115,- 1,117,25,26,120,- 1,122,
- 1,- 1,125,- 1,- 1,128,129,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,77,- 1,51,52,53,54,- 1,
- 1,- 1,- 1,59,- 1,89,62,- 1,92,93,- 1,95,96,69,70,99,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,
110,111,- 1,- 1,114,115,- 1,117,- 1,- 1,92,93,94,95,96,125,- 1,99,- 1,- 1,- 1,38,104,105,
- 1,- 1,108,- 1,- 1,111,- 1,- 1,114,115,- 1,117,25,26,120,- 1,122,- 1,- 1,125,- 1,- 1,128,
129,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,77,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,89,
62,- 1,92,93,- 1,95,96,69,70,99,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,110,111,- 1,- 1,114,
115,- 1,117,- 1,- 1,92,93,94,95,96,125,- 1,99,- 1,- 1,- 1,- 1,104,105,- 1,- 1,108,- 1,- 1,
111,- 1,- 1,114,115,- 1,117,25,26,120,- 1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,38,- 1,40,- 1,
- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,
69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,93,94,95,
96,- 1,- 1,99,- 1,- 1,- 1,- 1,104,- 1,- 1,- 1,108,- 1,- 1,111,- 1,- 1,114,115,- 1,117,25,26,
120,- 1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,
52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,93,94,95,96,- 1,- 1,99,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,108,- 1,- 1,111,- 1,- 1,- 1,- 1,116,117,25,26,120,- 1,122,- 1,- 1,125,- 1,- 1,128,129,
- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,
- 1,- 1,- 1,- 1,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,
- 1,92,93,94,95,96,- 1,- 1,99,38,- 1,40,- 1,104,- 1,44,- 1,108,- 1,- 1,111,- 1,51,52,53,54,
117,- 1,- 1,120,59,122,- 1,62,125,- 1,- 1,128,129,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,92,93,94,95,96,- 1,- 1,99,38,- 1,40,103,- 1,- 1,
44,- 1,108,- 1,- 1,111,- 1,51,52,53,54,117,- 1,- 1,120,59,122,- 1,62,125,- 1,- 1,128,129,
- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,92,93,94,
95,96,- 1,- 1,99,38,- 1,40,- 1,- 1,- 1,44,- 1,108,109,- 1,111,- 1,51,52,53,54,117,- 1,- 1,
120,59,122,- 1,62,125,- 1,- 1,128,129,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,93,94,95,96,- 1,- 1,99,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,-
1,- 1,111,- 1,- 1,- 1,- 1,116,117,25,26,120,- 1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,38,- 1,
40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,
- 1,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,92,93,
94,95,96,- 1,- 1,99,38,- 1,40,- 1,- 1,- 1,44,- 1,108,109,- 1,111,- 1,51,52,53,54,117,- 1,-
1,120,59,122,- 1,62,125,- 1,- 1,128,129,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,93,94,95,96,- 1,- 1,99,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,
- 1,- 1,111,- 1,- 1,- 1,- 1,116,117,25,26,120,- 1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,38,- 1,
40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,
- 1,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,93,
94,95,96,- 1,- 1,99,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,111,- 1,- 1,- 1,- 1,116,117,25,
26,120,- 1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,
51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,92,93,94,95,96,- 1,- 1,99,38,- 1,40,103,-
1,- 1,44,- 1,108,- 1,- 1,111,- 1,51,52,53,54,117,- 1,- 1,120,59,122,- 1,62,125,- 1,- 1,128,
129,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,92,
93,94,95,96,- 1,- 1,99,38,- 1,40,103,- 1,- 1,44,- 1,108,- 1,- 1,111,- 1,51,52,53,54,117,-
1,- 1,120,59,122,- 1,62,125,- 1,- 1,128,129,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,92,93,94,95,96,- 1,- 1,99,38,- 1,40,- 1,- 1,- 1,44,- 1,
108,109,- 1,111,- 1,51,52,53,54,117,- 1,- 1,120,59,122,- 1,62,125,- 1,- 1,128,129,- 1,69,
70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,92,93,94,95,96,
- 1,- 1,99,38,- 1,40,- 1,- 1,- 1,44,- 1,108,109,- 1,111,- 1,51,52,53,54,117,- 1,- 1,120,59,
122,- 1,62,125,- 1,- 1,128,129,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,25,26,- 1,- 1,- 1,92,93,94,95,96,- 1,- 1,99,38,- 1,40,103,- 1,- 1,44,- 1,108,- 1,- 1,
111,- 1,51,52,53,54,117,- 1,- 1,120,59,122,- 1,62,125,- 1,- 1,128,129,- 1,69,70,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,92,93,94,95,96,- 1,- 1,99,
38,- 1,40,- 1,- 1,- 1,44,- 1,108,109,- 1,111,- 1,51,52,53,54,117,- 1,- 1,120,59,122,- 1,62,
125,- 1,- 1,128,129,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,
- 1,- 1,- 1,92,93,94,95,96,- 1,- 1,99,38,- 1,40,- 1,- 1,- 1,44,- 1,108,109,- 1,111,- 1,51,52,
53,54,117,- 1,- 1,120,59,122,- 1,62,125,- 1,- 1,128,129,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,92,93,94,95,96,- 1,- 1,99,38,- 1,40,- 1,- 1,
- 1,44,- 1,108,109,- 1,111,- 1,51,52,53,54,117,- 1,- 1,120,59,122,- 1,62,125,- 1,- 1,128,
129,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,92,
93,94,95,96,- 1,- 1,99,38,- 1,40,- 1,- 1,- 1,44,- 1,108,109,- 1,111,- 1,51,52,53,54,117,-
1,- 1,120,59,122,- 1,62,125,- 1,- 1,128,129,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,92,93,94,95,96,- 1,- 1,99,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
108,- 1,- 1,111,- 1,51,52,53,54,117,- 1,- 1,120,- 1,122,- 1,62,125,- 1,- 1,128,129,- 1,69,
70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,92,93,94,95,96,
- 1,- 1,99,38,- 1,- 1,- 1,104,- 1,- 1,- 1,108,- 1,- 1,111,- 1,51,52,53,54,117,- 1,- 1,120,- 1,
122,- 1,62,125,- 1,- 1,128,129,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,25,26,- 1,- 1,- 1,92,93,94,95,96,- 1,- 1,99,38,- 1,- 1,- 1,104,- 1,- 1,- 1,108,- 1,- 1,
111,- 1,51,52,53,54,117,- 1,- 1,120,- 1,122,- 1,62,125,- 1,- 1,128,129,- 1,69,70,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,92,93,94,95,96,- 1,- 1,99,
38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,111,- 1,51,52,53,54,117,- 1,- 1,120,- 1,122,- 1,62,
125,- 1,- 1,128,129,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,
- 1,- 1,- 1,92,93,94,95,96,- 1,- 1,99,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,111,- 1,51,52,
53,54,117,- 1,- 1,120,- 1,122,- 1,62,125,- 1,- 1,128,129,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,92,93,94,95,96,- 1,- 1,99,38,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,108,- 1,- 1,111,- 1,51,52,53,54,117,- 1,- 1,120,- 1,122,- 1,62,125,- 1,- 1,128,
129,- 1,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,
93,94,95,96,- 1,- 1,99,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,111,- 1,- 1,- 1,- 1,- 1,117,- 1,
- 1,120,- 1,122,- 1,- 1,125,- 1,- 1,128,129,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,37,- 1,- 1,55,56,57,- 1,- 1,60,- 1,- 1,- 1,49,50,- 1,67,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,100,101,102,
- 1,104,- 1,- 1,- 1,- 1,- 1,110,111,97,98,- 1,100,101,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,67,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,100,101,102,- 1,- 1,105,- 1,- 1,- 1,- 1,110,111,97,98,- 1,
100,101,102,- 1,- 1,105,- 1,- 1,- 1,- 1,110,111,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,37,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,55,56,57,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,100,
101,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,97,98,- 1,100,101,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
110,111,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111};
char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible\000";char Cyc_Yystack_overflow[
21]="\000\000\000\000Yystack_overflow\000";void Cyc_yyerror(struct _dynforward_ptr);
int Cyc_yylex();static int Cyc_yychar=(int)'\000';union Cyc_YYSTYPE_union Cyc_yylval=(
union Cyc_YYSTYPE_union)((struct Cyc_YYINITIALSVAL_struct){56});static int Cyc_yynerrs=
0;struct _tuple18{struct Cyc_Position_Segment*f1;struct _tuple1*f2;int f3;};struct
_tuple19{struct _dynforward_ptr f1;void*f2;};static char _tmp414[8]="stdcall";static
char _tmp415[6]="cdecl";static char _tmp416[9]="fastcall";static char _tmp417[9]="noreturn";
static char _tmp418[6]="const";static char _tmp419[8]="aligned";static char _tmp41A[7]="packed";
static char _tmp41B[7]="shared";static char _tmp41C[7]="unused";static char _tmp41D[5]="weak";
static char _tmp41E[10]="dllimport";static char _tmp41F[10]="dllexport";static char
_tmp420[23]="no_instrument_function";static char _tmp421[12]="constructor";static
char _tmp422[11]="destructor";static char _tmp423[22]="no_check_memory_usage";
static char _tmp424[5]="pure";struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(){int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmp8B1=10000;short*_tmp8B2=(short*)
_cycalloc_atomic(_check_times(sizeof(short),_tmp8B1));{unsigned int _tmp8B3=
_tmp8B1;unsigned int i;for(i=0;i < _tmp8B3;i ++){_tmp8B2[i]=(short)0;}}_tmp8B2;});
int yyvsp_offset;union Cyc_YYSTYPE_union*yyvs=(union Cyc_YYSTYPE_union*)({
unsigned int _tmp8AE=10000;union Cyc_YYSTYPE_union*_tmp8AF=(union Cyc_YYSTYPE_union*)
_cycalloc(_check_times(sizeof(union Cyc_YYSTYPE_union),_tmp8AE));{unsigned int
_tmp8B0=_tmp8AE;unsigned int i;for(i=0;i < _tmp8B0;i ++){_tmp8AF[i]=Cyc_yylval;}}
_tmp8AF;});int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({
unsigned int _tmp8AB=10000;struct Cyc_Yyltype*_tmp8AC=(struct Cyc_Yyltype*)
_cycalloc_atomic(_check_times(sizeof(struct Cyc_Yyltype),_tmp8AB));{unsigned int
_tmp8AD=_tmp8AB;unsigned int i;for(i=0;i < _tmp8AD;i ++){_tmp8AC[i]=Cyc_yynewloc();}}
_tmp8AC;});int yystacksize=10000;union Cyc_YYSTYPE_union yyval=Cyc_yylval;int yylen;
yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=
0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(10000,++
yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 
10000){Cyc_yyerror(({const char*_tmp376="parser stack overflow";_tag_dynforward(
_tmp376,sizeof(char),_get_zero_arr_size(_tmp376,22));}));(int)_throw((void*)Cyc_Yystack_overflow);}}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(979,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex();
if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0  && Cyc_yychar
<= 357?(int)Cyc_yytranslate[_check_known_subscript_notnull(358,Cyc_yychar)]: 271;}
yyn +=yychar1;if((yyn < 0  || yyn > 5921) || Cyc_yycheck[
_check_known_subscript_notnull(5922,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(5922,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 978)
return 0;if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;if(yyerrstatus != 0)yyerrstatus --;yystate=yyn;goto
yynewstate;yydefault: yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(979,
yystate)];if(yyn == 0)goto yyerrlab;yyreduce: yylen=(int)Cyc_yyr2[
_check_known_subscript_notnull(495,yyn)];if(yylen > 0)yyval=yyvs[
_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){case
1: _LL1E1: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);break;
case 2: _LL1E2: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons
_tmp377;_push_handler(& _tmp377);{int _tmp379=0;if(setjmp(_tmp377.handler))_tmp379=
1;if(!_tmp379){x=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);;_pop_handler();}else{void*_tmp378=(void*)_exn_thrown;void*
_tmp37B=_tmp378;_LL1E5: if(*((void**)_tmp37B)!= Cyc_Core_Failure)goto _LL1E7;
_LL1E6: x=0;goto _LL1E4;_LL1E7:;_LL1E8:(void)_throw(_tmp37B);_LL1E4:;}}}{struct
_handler_cons _tmp37C;_push_handler(& _tmp37C);{int _tmp37E=0;if(setjmp(_tmp37C.handler))
_tmp37E=1;if(!_tmp37E){y=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);;_pop_handler();}else{void*_tmp37D=(void*)_exn_thrown;void*
_tmp380=_tmp37D;_LL1EA: if(*((void**)_tmp380)!= Cyc_Core_Failure)goto _LL1EC;
_LL1EB: y=0;goto _LL1E9;_LL1EC:;_LL1ED:(void)_throw(_tmp380);_LL1E9:;}}}yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp381;(_tmp381.YY17).tag=21;(
_tmp381.YY17).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(x,y);_tmp381;});break;}case 3: _LL1E3: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp382;(_tmp382.YY17).tag=21;(_tmp382.YY17).f1=({struct
Cyc_List_List*_tmp383=_cycalloc(sizeof(*_tmp383));_tmp383->hd=({struct Cyc_Absyn_Decl*
_tmp384=_cycalloc(sizeof(*_tmp384));_tmp384->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp385=_cycalloc(sizeof(*_tmp385));_tmp385[0]=({struct Cyc_Absyn_Using_d_struct
_tmp386;_tmp386.tag=9;_tmp386.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp386.f2=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp386;});_tmp385;}));
_tmp384->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp384;});_tmp383->tl=0;_tmp383;});_tmp382;});Cyc_Lex_leave_using();
break;case 4: _LL1EE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp387;(
_tmp387.YY17).tag=21;(_tmp387.YY17).f1=({struct Cyc_List_List*_tmp388=_cycalloc(
sizeof(*_tmp388));_tmp388->hd=({struct Cyc_Absyn_Decl*_tmp389=_cycalloc(sizeof(*
_tmp389));_tmp389->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*_tmp38A=
_cycalloc(sizeof(*_tmp38A));_tmp38A[0]=({struct Cyc_Absyn_Using_d_struct _tmp38B;
_tmp38B.tag=9;_tmp38B.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp38B.f2=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp38B;});_tmp38A;}));
_tmp389->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp389;});_tmp388->tl=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp388;});_tmp387;});
break;case 5: _LL1EF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp38C;(
_tmp38C.YY17).tag=21;(_tmp38C.YY17).f1=({struct Cyc_List_List*_tmp38D=_cycalloc(
sizeof(*_tmp38D));_tmp38D->hd=({struct Cyc_Absyn_Decl*_tmp38E=_cycalloc(sizeof(*
_tmp38E));_tmp38E->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp38F=
_cycalloc(sizeof(*_tmp38F));_tmp38F[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp390;_tmp390.tag=8;_tmp390.f1=({struct _dynforward_ptr*_tmp391=_cycalloc(
sizeof(*_tmp391));_tmp391[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp391;});_tmp390.f2=
Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp390;});
_tmp38F;}));_tmp38E->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp38E;});
_tmp38D->tl=0;_tmp38D;});_tmp38C;});Cyc_Lex_leave_namespace();break;case 6: _LL1F0: {
struct _dynforward_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp392;_push_handler(& _tmp392);{int _tmp394=0;if(setjmp(_tmp392.handler))
_tmp394=1;if(!_tmp394){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp393=(void*)_exn_thrown;void*_tmp396=_tmp393;_LL1F3: if(*((void**)_tmp396)
!= Cyc_Core_Failure)goto _LL1F5;_LL1F4: nspace=({const char*_tmp397="";
_tag_dynforward(_tmp397,sizeof(char),_get_zero_arr_size(_tmp397,1));});goto
_LL1F2;_LL1F5:;_LL1F6:(void)_throw(_tmp396);_LL1F2:;}}}{struct _handler_cons
_tmp398;_push_handler(& _tmp398);{int _tmp39A=0;if(setjmp(_tmp398.handler))_tmp39A=
1;if(!_tmp39A){x=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);;_pop_handler();}else{void*_tmp399=(void*)_exn_thrown;void*
_tmp39C=_tmp399;_LL1F8: if(*((void**)_tmp39C)!= Cyc_Core_Failure)goto _LL1FA;
_LL1F9: x=0;goto _LL1F7;_LL1FA:;_LL1FB:(void)_throw(_tmp39C);_LL1F7:;}}}{struct
_handler_cons _tmp39D;_push_handler(& _tmp39D);{int _tmp39F=0;if(setjmp(_tmp39D.handler))
_tmp39F=1;if(!_tmp39F){y=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);;_pop_handler();}else{void*_tmp39E=(void*)_exn_thrown;void*
_tmp3A1=_tmp39E;_LL1FD: if(*((void**)_tmp3A1)!= Cyc_Core_Failure)goto _LL1FF;
_LL1FE: y=0;goto _LL1FC;_LL1FF:;_LL200:(void)_throw(_tmp3A1);_LL1FC:;}}}yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3A2;(_tmp3A2.YY17).tag=21;(
_tmp3A2.YY17).f1=({struct Cyc_List_List*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));
_tmp3A3->hd=({struct Cyc_Absyn_Decl*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));_tmp3A4->r=(
void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp3A5=_cycalloc(sizeof(*
_tmp3A5));_tmp3A5[0]=({struct Cyc_Absyn_Namespace_d_struct _tmp3A6;_tmp3A6.tag=8;
_tmp3A6.f1=({struct _dynforward_ptr*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));_tmp3A7[0]=
nspace;_tmp3A7;});_tmp3A6.f2=x;_tmp3A6;});_tmp3A5;}));_tmp3A4->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3A4;});
_tmp3A3->tl=y;_tmp3A3;});_tmp3A2;});break;}case 7: _LL1F1: if(Cyc_strcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),({const char*_tmp3A8="C";_tag_dynforward(_tmp3A8,sizeof(char),
_get_zero_arr_size(_tmp3A8,2));}))== 0)yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3A9;(_tmp3A9.YY17).tag=21;(_tmp3A9.YY17).f1=({struct Cyc_List_List*_tmp3AA=
_cycalloc(sizeof(*_tmp3AA));_tmp3AA->hd=({struct Cyc_Absyn_Decl*_tmp3AB=_cycalloc(
sizeof(*_tmp3AB));_tmp3AB->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp3AC=_cycalloc(sizeof(*_tmp3AC));_tmp3AC[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp3AD;_tmp3AD.tag=10;_tmp3AD.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3AD;});_tmp3AC;}));
_tmp3AB->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3AB;});_tmp3AA->tl=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3AA;});_tmp3A9;});else{
if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp3AE="C include";
_tag_dynforward(_tmp3AE,sizeof(char),_get_zero_arr_size(_tmp3AE,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp3AF="expecting \"C\" or \"C include\"";_tag_dynforward(_tmp3AF,
sizeof(char),_get_zero_arr_size(_tmp3AF,29));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3B0;(_tmp3B0.YY17).tag=21;(_tmp3B0.YY17).f1=({
struct Cyc_List_List*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1->hd=({struct Cyc_Absyn_Decl*
_tmp3B2=_cycalloc(sizeof(*_tmp3B2));_tmp3B2->r=(void*)((void*)({struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3[0]=({struct Cyc_Absyn_ExternCinclude_d_struct
_tmp3B4;_tmp3B4.tag=11;_tmp3B4.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3B4.f2=0;_tmp3B4;});
_tmp3B3;}));_tmp3B2->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3B2;});
_tmp3B1->tl=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3B1;});_tmp3B0;});}break;case 8: _LL201: if(Cyc_strcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),({const char*_tmp3B5="C include";_tag_dynforward(_tmp3B5,
sizeof(char),_get_zero_arr_size(_tmp3B5,10));}))!= 0)Cyc_Parse_err(({const char*
_tmp3B6="expecting \"C include\"";_tag_dynforward(_tmp3B6,sizeof(char),
_get_zero_arr_size(_tmp3B6,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));{struct Cyc_List_List*
exs=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3B7;(_tmp3B7.YY17).tag=
21;(_tmp3B7.YY17).f1=({struct Cyc_List_List*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));
_tmp3B8->hd=({struct Cyc_Absyn_Decl*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9->r=(
void*)((void*)({struct Cyc_Absyn_ExternCinclude_d_struct*_tmp3BA=_cycalloc(
sizeof(*_tmp3BA));_tmp3BA[0]=({struct Cyc_Absyn_ExternCinclude_d_struct _tmp3BB;
_tmp3BB.tag=11;_tmp3BB.f1=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp3BB.f2=exs;_tmp3BB;});_tmp3BA;}));_tmp3B9->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3B9;});
_tmp3B8->tl=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3B8;});_tmp3B7;});break;}case 9: _LL202: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3BC;(_tmp3BC.YY17).tag=21;(_tmp3BC.YY17).f1=({struct
Cyc_List_List*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->hd=({struct Cyc_Absyn_Decl*
_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->r=(void*)((void*)0);_tmp3BE->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3BE;});_tmp3BD->tl=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3BD;});_tmp3BC;});
break;case 10: _LL203: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3BF;(_tmp3BF.YY17).tag=21;(_tmp3BF.YY17).f1=({struct Cyc_List_List*_tmp3C0=
_cycalloc(sizeof(*_tmp3C0));_tmp3C0->hd=({struct Cyc_Absyn_Decl*_tmp3C1=_cycalloc(
sizeof(*_tmp3C1));_tmp3C1->r=(void*)((void*)1);_tmp3C1->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3C1;});
_tmp3C0->tl=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3C0;});_tmp3BF;});break;case 11: _LL204: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3C2;(_tmp3C2.YY17).tag=21;(_tmp3C2.YY17).f1=0;_tmp3C2;});
break;case 12: _LL205: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)];break;case 13: _LL206: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3C3;(_tmp3C3.YY51).tag=55;(_tmp3C3.YY51).f1=0;_tmp3C3;});break;case 14: _LL207:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3C4;(_tmp3C4.YY51).tag=
55;(_tmp3C4.YY51).f1=({struct Cyc_List_List*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));
_tmp3C5->hd=({struct _tuple18*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3C6->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp3C6->f3=0;_tmp3C6;});_tmp3C5->tl=0;_tmp3C5;});_tmp3C4;});break;case 15: _LL208:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3C7;(_tmp3C7.YY51).tag=
55;(_tmp3C7.YY51).f1=({struct Cyc_List_List*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));
_tmp3C8->hd=({struct _tuple18*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));_tmp3C9->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3C9->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp3C9->f3=0;_tmp3C9;});_tmp3C8->tl=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3C8;});_tmp3C7;});
break;case 16: _LL209: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3CA;(_tmp3CA.YY17).tag=21;(_tmp3CA.YY17).f1=({struct Cyc_List_List*_tmp3CB=
_cycalloc(sizeof(*_tmp3CB));_tmp3CB->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp3CC=_cycalloc(sizeof(*_tmp3CC));_tmp3CC[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp3CD;_tmp3CD.tag=1;_tmp3CD.f1=Cyc_yyget_YY16(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3CD;});_tmp3CC;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3CB->tl=0;
_tmp3CB;});_tmp3CA;});break;case 17: _LL20A: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 18: _LL20B: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3CE;(_tmp3CE.YY17).tag=21;(
_tmp3CE.YY17).f1=0;_tmp3CE;});break;case 19: _LL20C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3CF;(_tmp3CF.YY16).tag=20;(_tmp3CF.YY16).f1=Cyc_Parse_make_function(
0,Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,
Cyc_yyget_YY8(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3CF;});
break;case 20: _LL20D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3D0;(_tmp3D0.YY16).tag=20;(_tmp3D0.YY16).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1->v=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3D1;}),Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY8(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3D0;});
break;case 21: _LL20E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3D2;(_tmp3D2.YY16).tag=20;(_tmp3D2.YY16).f1=Cyc_Parse_make_function(0,Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3D2;});
break;case 22: _LL20F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3D3;(_tmp3D3.YY16).tag=20;(_tmp3D3.YY16).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));_tmp3D4->v=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3D4;}),Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3D3;});
break;case 23: _LL210: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3D5;(_tmp3D5.YY16).tag=20;(_tmp3D5.YY16).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6->v=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3D6;}),Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY8(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3D5;});
break;case 24: _LL211: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3D7;(_tmp3D7.YY16).tag=20;(_tmp3D7.YY16).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));_tmp3D8->v=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3D8;}),Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3D7;});
break;case 25: _LL212: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL213: Cyc_Lex_leave_using();
break;case 27: _LL214: Cyc_Lex_enter_namespace(({struct _dynforward_ptr*_tmp3D9=
_cycalloc(sizeof(*_tmp3D9));_tmp3D9[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3D9;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL215: Cyc_Lex_leave_namespace();
break;case 29: _LL216: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3DA;(_tmp3DA.YY17).tag=21;(_tmp3DA.YY17).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp3DA;});
break;case 30: _LL217: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3DB;(_tmp3DB.YY17).tag=21;(_tmp3DB.YY17).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3DB;});
break;case 31: _LL218: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3DC;(_tmp3DC.YY17).tag=21;(_tmp3DC.YY17).f1=({struct Cyc_List_List*_tmp3DD=
_cycalloc(sizeof(*_tmp3DD));_tmp3DD->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3DD->tl=0;
_tmp3DD;});_tmp3DC;});break;case 32: _LL219: {struct Cyc_List_List*_tmp3DE=0;{
struct Cyc_List_List*_tmp3DF=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);for(0;_tmp3DF != 0;_tmp3DF=_tmp3DF->tl){struct
_dynforward_ptr*_tmp3E0=(struct _dynforward_ptr*)_tmp3DF->hd;struct _tuple1*
_tmp3E1=({struct _tuple1*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));_tmp3E4->f1=Cyc_Absyn_rel_ns_null;
_tmp3E4->f2=_tmp3E0;_tmp3E4;});struct Cyc_Absyn_Vardecl*_tmp3E2=Cyc_Absyn_new_vardecl(
_tmp3E1,Cyc_Absyn_wildtyp(0),0);_tmp3DE=({struct Cyc_List_List*_tmp3E3=_cycalloc(
sizeof(*_tmp3E3));_tmp3E3->hd=_tmp3E2;_tmp3E3->tl=_tmp3DE;_tmp3E3;});}}_tmp3DE=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3DE);yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3E5;(_tmp3E5.YY17).tag=21;(
_tmp3E5.YY17).f1=({struct Cyc_List_List*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));
_tmp3E6->hd=Cyc_Absyn_letv_decl(_tmp3DE,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E6->tl=0;
_tmp3E6;});_tmp3E5;});break;}case 33: _LL21A: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 34: _LL21B: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3E7;(_tmp3E7.YY17).tag=21;(
_tmp3E7.YY17).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp3E7;});break;case 35: _LL21C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3E8;(_tmp3E8.YY18).tag=22;(_tmp3E8.YY18).f1=({struct
Cyc_Parse_Declaration_spec*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));_tmp3E9->sc=({
struct Cyc_Core_Opt*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));_tmp3EA->v=(void*)Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3EA;});_tmp3E9->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E9->type_specs=
0;_tmp3E9->is_inline=0;_tmp3E9->attributes=0;_tmp3E9;});_tmp3E8;});break;case 36:
_LL21D: if((Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc
!= 0)({void*_tmp3EB[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp3EC="Only one storage class is allowed in a declaration";_tag_dynforward(
_tmp3EC,sizeof(char),_get_zero_arr_size(_tmp3EC,51));}),_tag_dynforward(_tmp3EB,
sizeof(void*),0));});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3ED;(_tmp3ED.YY18).tag=22;(_tmp3ED.YY18).f1=({struct Cyc_Parse_Declaration_spec*
_tmp3EE=_cycalloc(sizeof(*_tmp3EE));_tmp3EE->sc=({struct Cyc_Core_Opt*_tmp3EF=
_cycalloc(sizeof(*_tmp3EF));_tmp3EF->v=(void*)Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3EF;});_tmp3EE->tq=(
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;
_tmp3EE->type_specs=(Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs;_tmp3EE->is_inline=(Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp3EE->attributes=(
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp3EE;});_tmp3ED;});break;case 37: _LL21E: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp3F0;(_tmp3F0.YY18).tag=22;(_tmp3F0.YY18).f1=({struct Cyc_Parse_Declaration_spec*
_tmp3F1=_cycalloc(sizeof(*_tmp3F1));_tmp3F1->sc=0;_tmp3F1->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp3F1->type_specs=({struct Cyc_List_List*_tmp3F2=
_cycalloc(sizeof(*_tmp3F2));_tmp3F2->hd=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3F2->tl=0;_tmp3F2;});
_tmp3F1->is_inline=0;_tmp3F1->attributes=0;_tmp3F1;});_tmp3F0;});break;case 38:
_LL21F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3F3;(_tmp3F3.YY18).tag=
22;(_tmp3F3.YY18).f1=({struct Cyc_Parse_Declaration_spec*_tmp3F4=_cycalloc(
sizeof(*_tmp3F4));_tmp3F4->sc=(Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3F4->tq=(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3F4->type_specs=({
struct Cyc_List_List*_tmp3F5=_cycalloc(sizeof(*_tmp3F5));_tmp3F5->hd=(void*)Cyc_yyget_YY22(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3F5->tl=(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp3F5;});
_tmp3F4->is_inline=(Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp3F4->attributes=(Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp3F4;});
_tmp3F3;});break;case 39: _LL220: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3F6;(_tmp3F6.YY18).tag=22;(_tmp3F6.YY18).f1=({struct Cyc_Parse_Declaration_spec*
_tmp3F7=_cycalloc(sizeof(*_tmp3F7));_tmp3F7->sc=0;_tmp3F7->tq=Cyc_yyget_YY24(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3F7->type_specs=0;
_tmp3F7->is_inline=0;_tmp3F7->attributes=0;_tmp3F7;});_tmp3F6;});break;case 40:
_LL221: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3F8;(_tmp3F8.YY18).tag=
22;(_tmp3F8.YY18).f1=({struct Cyc_Parse_Declaration_spec*_tmp3F9=_cycalloc(
sizeof(*_tmp3F9));_tmp3F9->sc=(Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3F9->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);_tmp3F9->type_specs=(
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp3F9->is_inline=(Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp3F9->attributes=(Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp3F9;});
_tmp3F8;});break;case 41: _LL222: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3FA;(_tmp3FA.YY18).tag=22;(_tmp3FA.YY18).f1=({struct Cyc_Parse_Declaration_spec*
_tmp3FB=_cycalloc(sizeof(*_tmp3FB));_tmp3FB->sc=0;_tmp3FB->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp3FB->type_specs=0;_tmp3FB->is_inline=1;_tmp3FB->attributes=
0;_tmp3FB;});_tmp3FA;});break;case 42: _LL223: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3FC;(_tmp3FC.YY18).tag=22;(_tmp3FC.YY18).f1=({struct
Cyc_Parse_Declaration_spec*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));_tmp3FD->sc=(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3FD->tq=(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3FD->type_specs=(
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp3FD->is_inline=1;_tmp3FD->attributes=(Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp3FD;});
_tmp3FC;});break;case 43: _LL224: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3FE;(_tmp3FE.YY18).tag=22;(_tmp3FE.YY18).f1=({struct Cyc_Parse_Declaration_spec*
_tmp3FF=_cycalloc(sizeof(*_tmp3FF));_tmp3FF->sc=0;_tmp3FF->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp3FF->type_specs=0;_tmp3FF->is_inline=0;_tmp3FF->attributes=
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3FF;});
_tmp3FE;});break;case 44: _LL225: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp400;(_tmp400.YY18).tag=22;(_tmp400.YY18).f1=({struct Cyc_Parse_Declaration_spec*
_tmp401=_cycalloc(sizeof(*_tmp401));_tmp401->sc=(Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp401->tq=(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp401->type_specs=(
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp401->is_inline=(Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp401->attributes=Cyc_List_imp_append(Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp401;});
_tmp400;});break;case 45: _LL226: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp402;(_tmp402.YY21).tag=25;(_tmp402.YY21).f1=(void*)((void*)4);_tmp402;});
break;case 46: _LL227: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp403;(_tmp403.YY21).tag=25;(_tmp403.YY21).f1=(void*)((void*)5);_tmp403;});
break;case 47: _LL228: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp404;(_tmp404.YY21).tag=25;(_tmp404.YY21).f1=(void*)((void*)3);_tmp404;});
break;case 48: _LL229: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp405;(_tmp405.YY21).tag=25;(_tmp405.YY21).f1=(void*)((void*)1);_tmp405;});
break;case 49: _LL22A: if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp406="C";
_tag_dynforward(_tmp406,sizeof(char),_get_zero_arr_size(_tmp406,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp407="only extern or extern \"C\" is allowed";_tag_dynforward(
_tmp407,sizeof(char),_get_zero_arr_size(_tmp407,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp408;(_tmp408.YY21).tag=25;(_tmp408.YY21).f1=(void*)((
void*)2);_tmp408;});break;case 50: _LL22B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp409;(_tmp409.YY21).tag=25;(_tmp409.YY21).f1=(void*)((void*)0);_tmp409;});
break;case 51: _LL22C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp40A;(_tmp40A.YY21).tag=25;(_tmp40A.YY21).f1=(void*)((void*)6);_tmp40A;});
break;case 52: _LL22D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp40B;(_tmp40B.YY44).tag=48;(_tmp40B.YY44).f1=0;_tmp40B;});break;case 53: _LL22E:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 54:
_LL22F: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;
case 55: _LL230: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp40C;(
_tmp40C.YY44).tag=48;(_tmp40C.YY44).f1=({struct Cyc_List_List*_tmp40D=_cycalloc(
sizeof(*_tmp40D));_tmp40D->hd=(void*)Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp40D->tl=0;_tmp40D;});
_tmp40C;});break;case 56: _LL231: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp40E;(_tmp40E.YY44).tag=48;(_tmp40E.YY44).f1=({struct Cyc_List_List*_tmp40F=
_cycalloc(sizeof(*_tmp40F));_tmp40F->hd=(void*)Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp40F->tl=Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp40F;});_tmp40E;});
break;case 57: _LL232: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={1,- 1};
static struct _tuple19 att_map[17]={{{_tmp414,_tmp414 + 8},(void*)0},{{_tmp415,
_tmp415 + 6},(void*)1},{{_tmp416,_tmp416 + 9},(void*)2},{{_tmp417,_tmp417 + 9},(
void*)3},{{_tmp418,_tmp418 + 6},(void*)4},{{_tmp419,_tmp419 + 8},(void*)&
att_aligned},{{_tmp41A,_tmp41A + 7},(void*)5},{{_tmp41B,_tmp41B + 7},(void*)7},{{
_tmp41C,_tmp41C + 7},(void*)8},{{_tmp41D,_tmp41D + 5},(void*)9},{{_tmp41E,_tmp41E + 
10},(void*)10},{{_tmp41F,_tmp41F + 10},(void*)11},{{_tmp420,_tmp420 + 23},(void*)
12},{{_tmp421,_tmp421 + 12},(void*)13},{{_tmp422,_tmp422 + 11},(void*)14},{{
_tmp423,_tmp423 + 22},(void*)15},{{_tmp424,_tmp424 + 5},(void*)16}};struct
_dynforward_ptr _tmp410=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);if((((_get_dynforward_size(_tmp410,sizeof(char))> 4  && *((
const char*)_check_dynforward_subscript(_tmp410,sizeof(char),0))== '_') && *((
const char*)_check_dynforward_subscript(_tmp410,sizeof(char),1))== '_') && *((
const char*)_check_dynforward_subscript(_tmp410,sizeof(char),(int)(
_get_dynforward_size(_tmp410,sizeof(char))- 2)))== '_') && *((const char*)
_check_dynforward_subscript(_tmp410,sizeof(char),(int)(_get_dynforward_size(
_tmp410,sizeof(char))- 3)))== '_')_tmp410=(struct _dynforward_ptr)Cyc_substring((
struct _dynforward_ptr)_tmp410,2,_get_dynforward_size(_tmp410,sizeof(char))- 5);{
int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((struct _dynforward_ptr)_tmp410,(struct
_dynforward_ptr)(att_map[i]).f1)== 0){yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp411;(_tmp411.YY45).tag=49;(_tmp411.YY45).f1=(void*)(att_map[i]).f2;_tmp411;});
break;}}if(i == 17){Cyc_Parse_err(({const char*_tmp412="unrecognized attribute";
_tag_dynforward(_tmp412,sizeof(char),_get_zero_arr_size(_tmp412,23));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp413;(_tmp413.YY45).tag=49;(_tmp413.YY45).f1=(void*)((
void*)1);_tmp413;});}break;}}case 58: _LL233: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp426;(_tmp426.YY45).tag=49;(_tmp426.YY45).f1=(void*)((void*)
4);_tmp426;});break;case 59: _LL234: {struct _dynforward_ptr _tmp427=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct _tuple11
_tmp429;int _tmp42A;struct _tuple11*_tmp428=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp429=*_tmp428;_tmp42A=
_tmp429.f2;{void*a;if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp427,({const char*
_tmp42B="regparm";_tag_dynforward(_tmp42B,sizeof(char),_get_zero_arr_size(
_tmp42B,8));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)_tmp427,({const char*
_tmp42C="__regparm__";_tag_dynforward(_tmp42C,sizeof(char),_get_zero_arr_size(
_tmp42C,12));}))== 0){if(_tmp42A < 0  || _tmp42A > 3)Cyc_Parse_err(({const char*
_tmp42D="regparm requires value between 0 and 3";_tag_dynforward(_tmp42D,sizeof(
char),_get_zero_arr_size(_tmp42D,39));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp42E=_cycalloc_atomic(sizeof(*_tmp42E));
_tmp42E[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp42F;_tmp42F.tag=0;_tmp42F.f1=
_tmp42A;_tmp42F;});_tmp42E;});}else{if(Cyc_zstrcmp((struct _dynforward_ptr)
_tmp427,({const char*_tmp430="aligned";_tag_dynforward(_tmp430,sizeof(char),
_get_zero_arr_size(_tmp430,8));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)
_tmp427,({const char*_tmp431="__aligned__";_tag_dynforward(_tmp431,sizeof(char),
_get_zero_arr_size(_tmp431,12));}))== 0){if(_tmp42A < 0)Cyc_Parse_err(({const char*
_tmp432="aligned requires positive power of two";_tag_dynforward(_tmp432,sizeof(
char),_get_zero_arr_size(_tmp432,39));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp42A;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp433="aligned requires positive power of two";
_tag_dynforward(_tmp433,sizeof(char),_get_zero_arr_size(_tmp433,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp434=_cycalloc_atomic(sizeof(*_tmp434));
_tmp434[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp435;_tmp435.tag=1;_tmp435.f1=
_tmp42A;_tmp435;});_tmp434;});}}else{if(Cyc_zstrcmp((struct _dynforward_ptr)
_tmp427,({const char*_tmp436="initializes";_tag_dynforward(_tmp436,sizeof(char),
_get_zero_arr_size(_tmp436,12));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)
_tmp427,({const char*_tmp437="__initializes__";_tag_dynforward(_tmp437,sizeof(
char),_get_zero_arr_size(_tmp437,16));}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*
_tmp438=_cycalloc_atomic(sizeof(*_tmp438));_tmp438[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp439;_tmp439.tag=4;_tmp439.f1=_tmp42A;_tmp439;});_tmp438;});else{Cyc_Parse_err(({
const char*_tmp43A="unrecognized attribute";_tag_dynforward(_tmp43A,sizeof(char),
_get_zero_arr_size(_tmp43A,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}}
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp43B;(_tmp43B.YY45).tag=
49;(_tmp43B.YY45).f1=(void*)a;_tmp43B;});break;}}case 60: _LL235: {struct
_dynforward_ptr _tmp43C=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dynforward_ptr _tmp43D=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _dynforward_ptr)_tmp43C,({const char*_tmp43E="section";_tag_dynforward(
_tmp43E,sizeof(char),_get_zero_arr_size(_tmp43E,8));}))== 0  || Cyc_zstrcmp((
struct _dynforward_ptr)_tmp43C,({const char*_tmp43F="__section__";_tag_dynforward(
_tmp43F,sizeof(char),_get_zero_arr_size(_tmp43F,12));}))== 0)a=(void*)({struct
Cyc_Absyn_Section_att_struct*_tmp440=_cycalloc(sizeof(*_tmp440));_tmp440[0]=({
struct Cyc_Absyn_Section_att_struct _tmp441;_tmp441.tag=2;_tmp441.f1=_tmp43D;
_tmp441;});_tmp440;});else{Cyc_Parse_err(({const char*_tmp442="unrecognized attribute";
_tag_dynforward(_tmp442,sizeof(char),_get_zero_arr_size(_tmp442,23));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp443;(_tmp443.YY45).tag=
49;(_tmp443.YY45).f1=(void*)a;_tmp443;});break;}case 61: _LL236: {struct
_dynforward_ptr _tmp444=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dynforward_ptr _tmp445=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _dynforward_ptr)_tmp444,({const char*_tmp446="__mode__";_tag_dynforward(
_tmp446,sizeof(char),_get_zero_arr_size(_tmp446,9));}))== 0)a=(void*)({struct Cyc_Absyn_Mode_att_struct*
_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447[0]=({struct Cyc_Absyn_Mode_att_struct
_tmp448;_tmp448.tag=5;_tmp448.f1=_tmp445;_tmp448;});_tmp447;});else{Cyc_Parse_err(({
const char*_tmp449="unrecognized attribute";_tag_dynforward(_tmp449,sizeof(char),
_get_zero_arr_size(_tmp449,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp44A;(_tmp44A.YY45).tag=
49;(_tmp44A.YY45).f1=(void*)a;_tmp44A;});break;}case 62: _LL237: {struct
_dynforward_ptr _tmp44B=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dynforward_ptr _tmp44C=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp44E;struct
_tuple11 _tmp44D=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp44E=_tmp44D.f2;{int _tmp450;struct _tuple11 _tmp44F=*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp450=_tmp44F.f2;{
void*a=(void*)1;if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp44B,({const char*
_tmp451="format";_tag_dynforward(_tmp451,sizeof(char),_get_zero_arr_size(_tmp451,
7));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)_tmp44B,({const char*_tmp452="__format__";
_tag_dynforward(_tmp452,sizeof(char),_get_zero_arr_size(_tmp452,11));}))== 0){
if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp44C,({const char*_tmp453="printf";
_tag_dynforward(_tmp453,sizeof(char),_get_zero_arr_size(_tmp453,7));}))== 0)a=(
void*)({struct Cyc_Absyn_Format_att_struct*_tmp454=_cycalloc(sizeof(*_tmp454));
_tmp454[0]=({struct Cyc_Absyn_Format_att_struct _tmp455;_tmp455.tag=3;_tmp455.f1=(
void*)((void*)0);_tmp455.f2=_tmp44E;_tmp455.f3=_tmp450;_tmp455;});_tmp454;});
else{if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp44C,({const char*_tmp456="scanf";
_tag_dynforward(_tmp456,sizeof(char),_get_zero_arr_size(_tmp456,6));}))== 0)a=(
void*)({struct Cyc_Absyn_Format_att_struct*_tmp457=_cycalloc(sizeof(*_tmp457));
_tmp457[0]=({struct Cyc_Absyn_Format_att_struct _tmp458;_tmp458.tag=3;_tmp458.f1=(
void*)((void*)1);_tmp458.f2=_tmp44E;_tmp458.f3=_tmp450;_tmp458;});_tmp457;});
else{Cyc_Parse_err(({const char*_tmp459="unrecognized format type";
_tag_dynforward(_tmp459,sizeof(char),_get_zero_arr_size(_tmp459,25));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp45A="unrecognized attribute";_tag_dynforward(_tmp45A,sizeof(char),
_get_zero_arr_size(_tmp45A,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp45B;(_tmp45B.YY45).tag=49;(_tmp45B.YY45).f1=(
void*)a;_tmp45B;});break;}}}case 63: _LL238: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 64: _LL239: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp45C;(_tmp45C.YY22).tag=26;(
_tmp45C.YY22).f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp45E;_tmp45E.tag=16;_tmp45E.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp45E.f2=Cyc_yyget_YY39(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp45E.f3=0;_tmp45E.f4=
0;_tmp45E;});_tmp45D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp45C;});
break;case 65: _LL23A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp45F;(_tmp45F.YY22).tag=26;(_tmp45F.YY22).f1=(void*)Cyc_Parse_type_spec((void*)
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp45F;});break;case 66: _LL23B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp460;(_tmp460.YY22).tag=26;(_tmp460.YY22).f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp460;});
break;case 67: _LL23C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp461;(_tmp461.YY22).tag=26;(_tmp461.YY22).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp462=_cycalloc(sizeof(*_tmp462));_tmp462->v=(void*)Cyc_yyget_YY42(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp462;}),0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp461;});
break;case 68: _LL23D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp463;(_tmp463.YY22).tag=26;(_tmp463.YY22).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_char_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp463;});break;case 69: _LL23E: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp464;(_tmp464.YY22).tag=26;(_tmp464.YY22).f1=(void*)((
void*)({struct Cyc_Parse_Short_spec_struct*_tmp465=_cycalloc(sizeof(*_tmp465));
_tmp465[0]=({struct Cyc_Parse_Short_spec_struct _tmp466;_tmp466.tag=2;_tmp466.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp466;});_tmp465;}));_tmp464;});break;case 70: _LL23F:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp467;(_tmp467.YY22).tag=
26;(_tmp467.YY22).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp467;});
break;case 71: _LL240: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp468;(_tmp468.YY22).tag=26;(_tmp468.YY22).f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*
_tmp469=_cycalloc(sizeof(*_tmp469));_tmp469[0]=({struct Cyc_Parse_Long_spec_struct
_tmp46A;_tmp46A.tag=3;_tmp46A.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp46A;});
_tmp469;}));_tmp468;});break;case 72: _LL241: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp46B;(_tmp46B.YY22).tag=26;(_tmp46B.YY22).f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp46B;});
break;case 73: _LL242: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp46C;(_tmp46C.YY22).tag=26;(_tmp46C.YY22).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_double_typ(
0),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp46C;});break;case 74: _LL243: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp46D;(_tmp46D.YY22).tag=26;(_tmp46D.YY22).f1=(void*)((
void*)({struct Cyc_Parse_Signed_spec_struct*_tmp46E=_cycalloc(sizeof(*_tmp46E));
_tmp46E[0]=({struct Cyc_Parse_Signed_spec_struct _tmp46F;_tmp46F.tag=0;_tmp46F.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp46F;});_tmp46E;}));_tmp46D;});break;case 75: _LL244:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp470;(_tmp470.YY22).tag=
26;(_tmp470.YY22).f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp472;_tmp472.tag=1;_tmp472.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp472;});
_tmp471;}));_tmp470;});break;case 76: _LL245: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 77: _LL246: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 78: _LL247:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 79:
_LL248: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp473;(_tmp473.YY22).tag=
26;(_tmp473.YY22).f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_YY43(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp473;});
break;case 80: _LL249: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp474;(_tmp474.YY22).tag=26;(_tmp474.YY22).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TupleType_struct*_tmp475=_cycalloc(sizeof(*_tmp475));_tmp475[0]=({
struct Cyc_Absyn_TupleType_struct _tmp476;_tmp476.tag=9;_tmp476.f1=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp476;});_tmp475;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp474;});break;case 81: _LL24A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp477;(_tmp477.YY22).tag=26;(_tmp477.YY22).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp478=_cycalloc(sizeof(*_tmp478));
_tmp478[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp479;_tmp479.tag=15;_tmp479.f1=(
void*)Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp479;});_tmp478;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp477;});
break;case 82: _LL24B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp47A;(_tmp47A.YY22).tag=26;(_tmp47A.YY22).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_SizeofType_struct*_tmp47B=_cycalloc(sizeof(*_tmp47B));_tmp47B[0]=({
struct Cyc_Absyn_SizeofType_struct _tmp47C;_tmp47C.tag=14;_tmp47C.f1=(void*)Cyc_yyget_YY43(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp47C;});_tmp47B;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp47A;});break;case 83: _LL24C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp47D;(_tmp47D.YY22).tag=26;(_tmp47D.YY22).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TagType_struct*_tmp47E=_cycalloc(sizeof(*_tmp47E));
_tmp47E[0]=({struct Cyc_Absyn_TagType_struct _tmp47F;_tmp47F.tag=17;_tmp47F.f1=(
void*)Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp47F;});_tmp47E;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp47D;});
break;case 84: _LL24D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp480;(_tmp480.YY22).tag=26;(_tmp480.YY22).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TagType_struct*_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481[0]=({
struct Cyc_Absyn_TagType_struct _tmp482;_tmp482.tag=17;_tmp482.f1=(void*)Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp483=_cycalloc(sizeof(*_tmp483));_tmp483->v=(void*)((void*)
5);_tmp483;}),0);_tmp482;});_tmp481;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp480;});
break;case 85: _LL24E: {struct _tuple11 _tmp485;int _tmp486;struct _tuple11*_tmp484=
Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp485=*_tmp484;_tmp486=_tmp485.f2;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp487;(_tmp487.YY22).tag=26;(_tmp487.YY22).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TypeInt_struct*_tmp488=_cycalloc_atomic(sizeof(*_tmp488));
_tmp488[0]=({struct Cyc_Absyn_TypeInt_struct _tmp489;_tmp489.tag=18;_tmp489.f1=
_tmp486;_tmp489;});_tmp488;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp487;});
break;}case 86: _LL24F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp48A;(_tmp48A.YY42).tag=46;(_tmp48A.YY42).f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp48A;});
break;case 87: _LL250: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp48B;(_tmp48B.YY24).tag=28;(_tmp48B.YY24).f1=({struct Cyc_Absyn_Tqual _tmp48C;
_tmp48C.print_const=1;_tmp48C.q_volatile=0;_tmp48C.q_restrict=0;_tmp48C.real_const=
1;_tmp48C.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp48C;});_tmp48B;});break;case 88: _LL251: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp48D;(_tmp48D.YY24).tag=28;(
_tmp48D.YY24).f1=({struct Cyc_Absyn_Tqual _tmp48E;_tmp48E.print_const=0;_tmp48E.q_volatile=
1;_tmp48E.q_restrict=0;_tmp48E.real_const=0;_tmp48E.loc=0;_tmp48E;});_tmp48D;});
break;case 89: _LL252: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp48F;(_tmp48F.YY24).tag=28;(_tmp48F.YY24).f1=({struct Cyc_Absyn_Tqual _tmp490;
_tmp490.print_const=0;_tmp490.q_volatile=0;_tmp490.q_restrict=1;_tmp490.real_const=
0;_tmp490.loc=0;_tmp490;});_tmp48F;});break;case 90: _LL253: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp491;(_tmp491.YY22).tag=26;(_tmp491.YY22).f1=(void*)((
void*)({struct Cyc_Parse_Decl_spec_struct*_tmp492=_cycalloc(sizeof(*_tmp492));
_tmp492[0]=({struct Cyc_Parse_Decl_spec_struct _tmp493;_tmp493.tag=5;_tmp493.f1=({
struct Cyc_Absyn_Decl*_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494->r=(void*)((
void*)({struct Cyc_Absyn_Enum_d_struct*_tmp495=_cycalloc(sizeof(*_tmp495));
_tmp495[0]=({struct Cyc_Absyn_Enum_d_struct _tmp496;_tmp496.tag=6;_tmp496.f1=({
struct Cyc_Absyn_Enumdecl*_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497->sc=(void*)((
void*)2);_tmp497->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp497->fields=({struct Cyc_Core_Opt*_tmp498=_cycalloc(
sizeof(*_tmp498));_tmp498->v=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp498;});_tmp497;});_tmp496;});_tmp495;}));_tmp494->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp494;});_tmp493;});_tmp492;}));_tmp491;});break;
case 91: _LL254: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp499;(
_tmp499.YY22).tag=26;(_tmp499.YY22).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_EnumType_struct*_tmp49A=_cycalloc(sizeof(*_tmp49A));_tmp49A[0]=({
struct Cyc_Absyn_EnumType_struct _tmp49B;_tmp49B.tag=12;_tmp49B.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp49B.f2=0;_tmp49B;});
_tmp49A;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp499;});break;case 92: _LL255: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp49C;(_tmp49C.YY22).tag=26;(_tmp49C.YY22).f1=(void*)((
void*)({struct Cyc_Parse_Type_spec_struct*_tmp49D=_cycalloc(sizeof(*_tmp49D));
_tmp49D[0]=({struct Cyc_Parse_Type_spec_struct _tmp49E;_tmp49E.tag=4;_tmp49E.f1=(
void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmp49F=_cycalloc(sizeof(*
_tmp49F));_tmp49F[0]=({struct Cyc_Absyn_AnonEnumType_struct _tmp4A0;_tmp4A0.tag=13;
_tmp4A0.f1=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]);_tmp4A0;});_tmp49F;}));_tmp49E.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp49E;});
_tmp49D;}));_tmp49C;});break;case 93: _LL256: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4A1;(_tmp4A1.YY46).tag=50;(_tmp4A1.YY46).f1=({struct Cyc_Absyn_Enumfield*
_tmp4A2=_cycalloc(sizeof(*_tmp4A2));_tmp4A2->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4A2->tag=0;_tmp4A2->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4A2;});_tmp4A1;});break;case 94: _LL257: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4A3;(_tmp4A3.YY46).tag=50;(
_tmp4A3.YY46).f1=({struct Cyc_Absyn_Enumfield*_tmp4A4=_cycalloc(sizeof(*_tmp4A4));
_tmp4A4->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp4A4->tag=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4A4->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4A4;});
_tmp4A3;});break;case 95: _LL258: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4A5;(_tmp4A5.YY47).tag=51;(_tmp4A5.YY47).f1=({struct Cyc_List_List*_tmp4A6=
_cycalloc(sizeof(*_tmp4A6));_tmp4A6->hd=Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4A6->tl=0;_tmp4A6;});
_tmp4A5;});break;case 96: _LL259: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4A7;(_tmp4A7.YY47).tag=51;(_tmp4A7.YY47).f1=({struct Cyc_List_List*_tmp4A8=
_cycalloc(sizeof(*_tmp4A8));_tmp4A8->hd=Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4A8->tl=0;_tmp4A8;});
_tmp4A7;});break;case 97: _LL25A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4A9;(_tmp4A9.YY47).tag=51;(_tmp4A9.YY47).f1=({struct Cyc_List_List*_tmp4AA=
_cycalloc(sizeof(*_tmp4AA));_tmp4AA->hd=Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4AA->tl=Cyc_yyget_YY47(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4AA;});_tmp4A9;});
break;case 98: _LL25B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4AB;(_tmp4AB.YY22).tag=26;(_tmp4AB.YY22).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_AnonAggrType_struct*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC[
0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp4AD;_tmp4AD.tag=11;_tmp4AD.f1=(void*)
Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp4AD.f2=Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]);_tmp4AD;});_tmp4AC;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4AB;});
break;case 99: _LL25C: {struct Cyc_List_List*_tmp4AE=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_YY39(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp4AF=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4B0;(_tmp4B0.YY22).tag=26;(_tmp4B0.YY22).f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp4B1=_cycalloc(sizeof(*_tmp4B1));_tmp4B1[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp4B2;_tmp4B2.tag=5;_tmp4B2.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4AE,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4AF,Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4B2;});
_tmp4B1;}));_tmp4B0;});break;}case 100: _LL25D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4B3;(_tmp4B3.YY22).tag=26;(_tmp4B3.YY22).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp4B4=_cycalloc(sizeof(*_tmp4B4));
_tmp4B4[0]=({struct Cyc_Absyn_AggrType_struct _tmp4B5;_tmp4B5.tag=10;_tmp4B5.f1=({
struct Cyc_Absyn_AggrInfo _tmp4B6;_tmp4B6.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp4B7=_cycalloc(sizeof(*_tmp4B7));_tmp4B7[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp4B8;_tmp4B8.tag=0;_tmp4B8.f1=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4B8.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4B8;});_tmp4B7;}));
_tmp4B6.targs=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4B6;});_tmp4B5;});_tmp4B4;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4B3;});
break;case 101: _LL25E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4B9;(_tmp4B9.YY39).tag=43;(_tmp4B9.YY39).f1=0;_tmp4B9;});break;case 102:
_LL25F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4BA;(_tmp4BA.YY39).tag=
43;(_tmp4BA.YY39).f1=Cyc_List_imp_rev(Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4BA;});break;case
103: _LL260: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4BB;(
_tmp4BB.YY23).tag=27;(_tmp4BB.YY23).f1=(void*)((void*)0);_tmp4BB;});break;case
104: _LL261: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4BC;(
_tmp4BC.YY23).tag=27;(_tmp4BC.YY23).f1=(void*)((void*)1);_tmp4BC;});break;case
105: _LL262: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4BD;(
_tmp4BD.YY25).tag=29;(_tmp4BD.YY25).f1=0;_tmp4BD;});break;case 106: _LL263: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4BE;(_tmp4BE.YY25).tag=29;(
_tmp4BE.YY25).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));_tmp4BE;});break;
case 107: _LL264: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4BF;(
_tmp4BF.YY26).tag=30;(_tmp4BF.YY26).f1=({struct Cyc_List_List*_tmp4C0=_cycalloc(
sizeof(*_tmp4C0));_tmp4C0->hd=Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4C0->tl=0;_tmp4C0;});_tmp4BF;});break;case 108: _LL265:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4C1;(_tmp4C1.YY26).tag=
30;(_tmp4C1.YY26).f1=({struct Cyc_List_List*_tmp4C2=_cycalloc(sizeof(*_tmp4C2));
_tmp4C2->hd=Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4C2->tl=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp4C2;});_tmp4C1;});break;case 109: _LL266: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4C3;(_tmp4C3.YY20).tag=24;(_tmp4C3.YY20).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp4C3;});break;case
110: _LL267: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4C4;(
_tmp4C4.YY20).tag=24;(_tmp4C4.YY20).f1=({struct Cyc_List_List*_tmp4C5=_cycalloc(
sizeof(*_tmp4C5));_tmp4C5->hd=Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4C5->tl=0;_tmp4C5;});_tmp4C4;});break;case 111: _LL268:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4C6;(_tmp4C6.YY20).tag=
24;(_tmp4C6.YY20).f1=({struct Cyc_List_List*_tmp4C7=_cycalloc(sizeof(*_tmp4C7));
_tmp4C7->hd=Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4C7->tl=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp4C7;});_tmp4C6;});break;case 112: _LL269: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4C8;(_tmp4C8.YY19).tag=23;(_tmp4C8.YY19).f1=({
struct _tuple15*_tmp4C9=_cycalloc(sizeof(*_tmp4C9));_tmp4C9->f1=Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4C9->f2=0;_tmp4C9;});
_tmp4C8;});break;case 113: _LL26A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4CA;(_tmp4CA.YY19).tag=23;(_tmp4CA.YY19).f1=({struct _tuple15*_tmp4CB=
_cycalloc(sizeof(*_tmp4CB));_tmp4CB->f1=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4CB->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4CB;});
_tmp4CA;});break;case 114: _LL26B: {struct _tuple16 _tmp4CD;struct Cyc_Absyn_Tqual
_tmp4CE;struct Cyc_List_List*_tmp4CF;struct Cyc_List_List*_tmp4D0;struct _tuple16*
_tmp4CC=Cyc_yyget_YY34(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp4CD=*_tmp4CC;_tmp4CE=_tmp4CD.f1;_tmp4CF=_tmp4CD.f2;_tmp4D0=_tmp4CD.f3;if(
_tmp4CE.loc == 0)_tmp4CE.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);{struct Cyc_List_List*
_tmp4D2;struct Cyc_List_List*_tmp4D3;struct _tuple0 _tmp4D1=((struct _tuple0(*)(
struct Cyc_List_List*x))Cyc_List_split)(Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4D2=_tmp4D1.f1;
_tmp4D3=_tmp4D1.f2;{void*_tmp4D4=Cyc_Parse_speclist2typ(_tmp4CF,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp4D5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(Cyc_Parse_apply_tmss(_tmp4CE,_tmp4D4,_tmp4D2,_tmp4D0),_tmp4D3);
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4D6;(_tmp4D6.YY25).tag=
29;(_tmp4D6.YY25).f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(
struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp4D5);
_tmp4D6;});break;}}}case 115: _LL26C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4D7;(_tmp4D7.YY34).tag=38;(_tmp4D7.YY34).f1=({struct _tuple16*_tmp4D8=
_cycalloc(sizeof(*_tmp4D8));_tmp4D8->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4D8->f2=({
struct Cyc_List_List*_tmp4D9=_cycalloc(sizeof(*_tmp4D9));_tmp4D9->hd=(void*)Cyc_yyget_YY22(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4D9->tl=0;_tmp4D9;});
_tmp4D8->f3=0;_tmp4D8;});_tmp4D7;});break;case 116: _LL26D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4DA;(_tmp4DA.YY34).tag=38;(_tmp4DA.YY34).f1=({struct
_tuple16*_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB->f1=(*Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp4DB->f2=({struct Cyc_List_List*
_tmp4DC=_cycalloc(sizeof(*_tmp4DC));_tmp4DC->hd=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4DC->tl=(*Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4DC;});_tmp4DB->f3=(*
Cyc_yyget_YY34(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp4DB;});_tmp4DA;});break;case 117: _LL26E: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4DD;(_tmp4DD.YY34).tag=38;(_tmp4DD.YY34).f1=({struct
_tuple16*_tmp4DE=_cycalloc(sizeof(*_tmp4DE));_tmp4DE->f1=Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4DE->f2=0;_tmp4DE->f3=0;
_tmp4DE;});_tmp4DD;});break;case 118: _LL26F: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4DF;(_tmp4DF.YY34).tag=38;(_tmp4DF.YY34).f1=({struct
_tuple16*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0->f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY34(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);
_tmp4E0->f2=(*Cyc_yyget_YY34(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f2;_tmp4E0->f3=(*Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp4E0;});_tmp4DF;});
break;case 119: _LL270: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4E1;(_tmp4E1.YY34).tag=38;(_tmp4E1.YY34).f1=({struct _tuple16*_tmp4E2=
_cycalloc(sizeof(*_tmp4E2));_tmp4E2->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4E2->f2=0;
_tmp4E2->f3=Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4E2;});_tmp4E1;});break;case 120: _LL271: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4E3;(_tmp4E3.YY34).tag=38;(_tmp4E3.YY34).f1=({struct
_tuple16*_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4->f1=(*Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp4E4->f2=(*Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4E4->f3=Cyc_List_append(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY34(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp4E4;});_tmp4E3;});break;case 121: _LL272: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4E5;(_tmp4E5.YY34).tag=38;(_tmp4E5.YY34).f1=({struct
_tuple16*_tmp4E6=_cycalloc(sizeof(*_tmp4E6));_tmp4E6->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4E6->f2=({struct Cyc_List_List*_tmp4E7=_cycalloc(
sizeof(*_tmp4E7));_tmp4E7->hd=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E7->tl=0;_tmp4E7;});
_tmp4E6->f3=0;_tmp4E6;});_tmp4E5;});break;case 122: _LL273: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4E8;(_tmp4E8.YY34).tag=38;(_tmp4E8.YY34).f1=({struct
_tuple16*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));_tmp4E9->f1=(*Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp4E9->f2=({struct Cyc_List_List*
_tmp4EA=_cycalloc(sizeof(*_tmp4EA));_tmp4EA->hd=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4EA->tl=(*Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4EA;});_tmp4E9->f3=(*
Cyc_yyget_YY34(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp4E9;});_tmp4E8;});break;case 123: _LL274: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4EB;(_tmp4EB.YY34).tag=38;(_tmp4EB.YY34).f1=({struct
_tuple16*_tmp4EC=_cycalloc(sizeof(*_tmp4EC));_tmp4EC->f1=Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4EC->f2=0;_tmp4EC->f3=0;
_tmp4EC;});_tmp4EB;});break;case 124: _LL275: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4ED;(_tmp4ED.YY34).tag=38;(_tmp4ED.YY34).f1=({struct
_tuple16*_tmp4EE=_cycalloc(sizeof(*_tmp4EE));_tmp4EE->f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY34(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);
_tmp4EE->f2=(*Cyc_yyget_YY34(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f2;_tmp4EE->f3=(*Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp4EE;});_tmp4ED;});
break;case 125: _LL276: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4EF;(_tmp4EF.YY34).tag=38;(_tmp4EF.YY34).f1=({struct _tuple16*_tmp4F0=
_cycalloc(sizeof(*_tmp4F0));_tmp4F0->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4F0->f2=0;
_tmp4F0->f3=Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4F0;});_tmp4EF;});break;case 126: _LL277: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4F1;(_tmp4F1.YY34).tag=38;(_tmp4F1.YY34).f1=({struct
_tuple16*_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2->f1=(*Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp4F2->f2=(*Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4F2->f3=Cyc_List_append(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY34(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp4F2;});_tmp4F1;});break;case 127: _LL278: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4F3;(_tmp4F3.YY20).tag=24;(_tmp4F3.YY20).f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp4F3;});break;case 128:
_LL279: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4F4;(_tmp4F4.YY20).tag=
24;(_tmp4F4.YY20).f1=({struct Cyc_List_List*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));
_tmp4F5->hd=Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4F5->tl=0;_tmp4F5;});_tmp4F4;});break;case 129: _LL27A: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4F6;(_tmp4F6.YY20).tag=24;(
_tmp4F6.YY20).f1=({struct Cyc_List_List*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));
_tmp4F7->hd=Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4F7->tl=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp4F7;});_tmp4F6;});break;case 130: _LL27B: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4F8;(_tmp4F8.YY19).tag=23;(_tmp4F8.YY19).f1=({
struct _tuple15*_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9->f1=Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F9->f2=0;_tmp4F9;});
_tmp4F8;});break;case 131: _LL27C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4FA;(_tmp4FA.YY19).tag=23;(_tmp4FA.YY19).f1=({struct _tuple15*_tmp4FB=
_cycalloc(sizeof(*_tmp4FB));_tmp4FB->f1=({struct Cyc_Parse_Declarator*_tmp4FC=
_cycalloc(sizeof(*_tmp4FC));_tmp4FC->id=({struct _tuple1*_tmp4FD=_cycalloc(
sizeof(*_tmp4FD));_tmp4FD->f1=Cyc_Absyn_rel_ns_null;_tmp4FD->f2=
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"",sizeof(char),1);
_tmp4FD;});_tmp4FC->tms=0;_tmp4FC;});_tmp4FB->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4FB;});_tmp4FA;});
break;case 132: _LL27D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4FF;(_tmp4FF.YY19).tag=23;(_tmp4FF.YY19).f1=({struct _tuple15*_tmp500=
_cycalloc(sizeof(*_tmp500));_tmp500->f1=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp500->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp500;});
_tmp4FF;});break;case 133: _LL27E: {struct Cyc_List_List*_tmp501=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY39(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp502;(_tmp502.YY22).tag=26;(_tmp502.YY22).f1=(void*)((
void*)({struct Cyc_Parse_Decl_spec_struct*_tmp503=_cycalloc(sizeof(*_tmp503));
_tmp503[0]=({struct Cyc_Parse_Decl_spec_struct _tmp504;_tmp504.tag=5;_tmp504.f1=
Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp501,({struct Cyc_Core_Opt*
_tmp505=_cycalloc(sizeof(*_tmp505));_tmp505->v=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp505;}),Cyc_yyget_YY30(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp504;});
_tmp503;}));_tmp502;});break;}case 134: _LL27F: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp506;(_tmp506.YY22).tag=26;(_tmp506.YY22).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp507=_cycalloc(sizeof(*_tmp507));
_tmp507[0]=({struct Cyc_Absyn_TunionType_struct _tmp508;_tmp508.tag=2;_tmp508.f1=({
struct Cyc_Absyn_TunionInfo _tmp509;_tmp509.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp50A=_cycalloc(sizeof(*_tmp50A));_tmp50A[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp50B;_tmp50B.tag=0;_tmp50B.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp50C;
_tmp50C.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp50C.is_xtunion=Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp50C.is_flat=Cyc_yyget_YY30(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp50C;});_tmp50B;});
_tmp50A;}));_tmp509.targs=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp509.rgn=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp509;});_tmp508;});
_tmp507;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp506;});break;case 135: _LL280: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp50D;(_tmp50D.YY22).tag=26;(_tmp50D.YY22).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp50E=_cycalloc(sizeof(*_tmp50E));
_tmp50E[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp50F;_tmp50F.tag=3;_tmp50F.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp510;_tmp510.field_info=(void*)((void*)({
struct Cyc_Absyn_UnknownTunionfield_struct*_tmp511=_cycalloc(sizeof(*_tmp511));
_tmp511[0]=({struct Cyc_Absyn_UnknownTunionfield_struct _tmp512;_tmp512.tag=0;
_tmp512.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp513;_tmp513.tunion_name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp513.field_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp513.is_xtunion=Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp513;});_tmp512;});
_tmp511;}));_tmp510.targs=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp510;});_tmp50F;});_tmp50E;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp50D;});
break;case 136: _LL281: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp514;(_tmp514.YY30).tag=34;(_tmp514.YY30).f1=1;_tmp514;});break;case 137:
_LL282: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp515;(_tmp515.YY30).tag=
34;(_tmp515.YY30).f1=0;_tmp515;});break;case 138: _LL283: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp516;(_tmp516.YY30).tag=34;(_tmp516.YY30).f1=0;_tmp516;});
break;case 139: _LL284: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp517;(_tmp517.YY30).tag=34;(_tmp517.YY30).f1=1;_tmp517;});break;case 140:
_LL285: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp518;(_tmp518.YY33).tag=
37;(_tmp518.YY33).f1=({struct Cyc_List_List*_tmp519=_cycalloc(sizeof(*_tmp519));
_tmp519->hd=Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp519->tl=0;_tmp519;});_tmp518;});break;case 141: _LL286: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp51A;(_tmp51A.YY33).tag=37;(
_tmp51A.YY33).f1=({struct Cyc_List_List*_tmp51B=_cycalloc(sizeof(*_tmp51B));
_tmp51B->hd=Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp51B->tl=0;_tmp51B;});_tmp51A;});break;case 142: _LL287:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp51C;(_tmp51C.YY33).tag=
37;(_tmp51C.YY33).f1=({struct Cyc_List_List*_tmp51D=_cycalloc(sizeof(*_tmp51D));
_tmp51D->hd=Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp51D->tl=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp51D;});_tmp51C;});
break;case 143: _LL288: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp51E;(_tmp51E.YY33).tag=37;(_tmp51E.YY33).f1=({struct Cyc_List_List*_tmp51F=
_cycalloc(sizeof(*_tmp51F));_tmp51F->hd=Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp51F->tl=Cyc_yyget_YY33(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp51F;});_tmp51E;});
break;case 144: _LL289: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp520;(_tmp520.YY31).tag=35;(_tmp520.YY31).f1=(void*)((void*)2);_tmp520;});
break;case 145: _LL28A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp521;(_tmp521.YY31).tag=35;(_tmp521.YY31).f1=(void*)((void*)3);_tmp521;});
break;case 146: _LL28B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp522;(_tmp522.YY31).tag=35;(_tmp522.YY31).f1=(void*)((void*)0);_tmp522;});
break;case 147: _LL28C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp523;(_tmp523.YY32).tag=36;(_tmp523.YY32).f1=({struct Cyc_Absyn_Tunionfield*
_tmp524=_cycalloc(sizeof(*_tmp524));_tmp524->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp524->typs=0;_tmp524->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp524->sc=(void*)Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp524;});_tmp523;});
break;case 148: _LL28D: {struct Cyc_List_List*_tmp525=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp526;(_tmp526.YY32).tag=36;(_tmp526.YY32).f1=({struct
Cyc_Absyn_Tunionfield*_tmp527=_cycalloc(sizeof(*_tmp527));_tmp527->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp527->typs=
_tmp525;_tmp527->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp527->sc=(
void*)Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp527;});_tmp526;});break;}case 149: _LL28E: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 150: _LL28F: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp528;(_tmp528.YY28).tag=32;(
_tmp528.YY28).f1=({struct Cyc_Parse_Declarator*_tmp529=_cycalloc(sizeof(*_tmp529));
_tmp529->id=(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp529->tms=Cyc_List_imp_append(Cyc_yyget_YY27(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp529;});_tmp528;});
break;case 151: _LL290: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 152: _LL291: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp52A;(_tmp52A.YY28).tag=32;(_tmp52A.YY28).f1=({struct Cyc_Parse_Declarator*
_tmp52B=_cycalloc(sizeof(*_tmp52B));_tmp52B->id=(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->id;_tmp52B->tms=Cyc_List_imp_append(
Cyc_yyget_YY27(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp52B;});
_tmp52A;});break;case 153: _LL292: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp52C;(_tmp52C.YY28).tag=32;(_tmp52C.YY28).f1=({struct Cyc_Parse_Declarator*
_tmp52D=_cycalloc(sizeof(*_tmp52D));_tmp52D->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp52D->tms=0;_tmp52D;});
_tmp52C;});break;case 154: _LL293: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 155: _LL294: {struct Cyc_Parse_Declarator*_tmp52E=Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp52E->tms=({
struct Cyc_List_List*_tmp52F=_cycalloc(sizeof(*_tmp52F));_tmp52F->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp530=_cycalloc(sizeof(*_tmp530));
_tmp530[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp531;_tmp531.tag=5;_tmp531.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp531.f2=Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp531;});_tmp530;}));
_tmp52F->tl=_tmp52E->tms;_tmp52F;});yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;}case 156: _LL295: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp532;(_tmp532.YY28).tag=32;(_tmp532.YY28).f1=({struct
Cyc_Parse_Declarator*_tmp533=_cycalloc(sizeof(*_tmp533));_tmp533->id=(Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp533->tms=({
struct Cyc_List_List*_tmp534=_cycalloc(sizeof(*_tmp534));_tmp534->hd=(void*)((
void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp535=_cycalloc(sizeof(*_tmp535));
_tmp535[0]=({struct Cyc_Absyn_Carray_mod_struct _tmp536;_tmp536.tag=0;_tmp536.f1=
Cyc_yyget_YY50(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp536.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp536;});_tmp535;}));_tmp534->tl=(Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp534;});
_tmp533;});_tmp532;});break;case 157: _LL296: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp537;(_tmp537.YY28).tag=32;(_tmp537.YY28).f1=({struct Cyc_Parse_Declarator*
_tmp538=_cycalloc(sizeof(*_tmp538));_tmp538->id=(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp538->tms=({
struct Cyc_List_List*_tmp539=_cycalloc(sizeof(*_tmp539));_tmp539->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp53A=_cycalloc(sizeof(*_tmp53A));
_tmp53A[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp53B;_tmp53B.tag=1;_tmp53B.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp53B.f2=Cyc_yyget_YY50(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp53B.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp53B;});_tmp53A;}));_tmp539->tl=(Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp539;});
_tmp538;});_tmp537;});break;case 158: _LL297: {struct _tuple17 _tmp53D;struct Cyc_List_List*
_tmp53E;int _tmp53F;struct Cyc_Absyn_VarargInfo*_tmp540;struct Cyc_Core_Opt*_tmp541;
struct Cyc_List_List*_tmp542;struct _tuple17*_tmp53C=Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp53D=*_tmp53C;_tmp53E=
_tmp53D.f1;_tmp53F=_tmp53D.f2;_tmp540=_tmp53D.f3;_tmp541=_tmp53D.f4;_tmp542=
_tmp53D.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp543;(
_tmp543.YY28).tag=32;(_tmp543.YY28).f1=({struct Cyc_Parse_Declarator*_tmp544=
_cycalloc(sizeof(*_tmp544));_tmp544->id=(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp544->tms=({
struct Cyc_List_List*_tmp545=_cycalloc(sizeof(*_tmp545));_tmp545->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp546=_cycalloc(sizeof(*_tmp546));
_tmp546[0]=({struct Cyc_Absyn_Function_mod_struct _tmp547;_tmp547.tag=3;_tmp547.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp548=_cycalloc(sizeof(*
_tmp548));_tmp548[0]=({struct Cyc_Absyn_WithTypes_struct _tmp549;_tmp549.tag=1;
_tmp549.f1=_tmp53E;_tmp549.f2=_tmp53F;_tmp549.f3=_tmp540;_tmp549.f4=_tmp541;
_tmp549.f5=_tmp542;_tmp549;});_tmp548;}));_tmp547;});_tmp546;}));_tmp545->tl=(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;
_tmp545;});_tmp544;});_tmp543;});break;}case 159: _LL298: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp54A;(_tmp54A.YY28).tag=32;(_tmp54A.YY28).f1=({struct
Cyc_Parse_Declarator*_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B->id=(Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp54B->tms=({
struct Cyc_List_List*_tmp54C=_cycalloc(sizeof(*_tmp54C));_tmp54C->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp54D=_cycalloc(sizeof(*_tmp54D));
_tmp54D[0]=({struct Cyc_Absyn_Function_mod_struct _tmp54E;_tmp54E.tag=3;_tmp54E.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp54F=_cycalloc(sizeof(*
_tmp54F));_tmp54F[0]=({struct Cyc_Absyn_WithTypes_struct _tmp550;_tmp550.tag=1;
_tmp550.f1=0;_tmp550.f2=0;_tmp550.f3=0;_tmp550.f4=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp550.f5=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp550;});_tmp54F;}));
_tmp54E;});_tmp54D;}));_tmp54C->tl=(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp54C;});_tmp54B;});
_tmp54A;});break;case 160: _LL299: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp551;(_tmp551.YY28).tag=32;(_tmp551.YY28).f1=({struct Cyc_Parse_Declarator*
_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552->id=(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp552->tms=({
struct Cyc_List_List*_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp554=_cycalloc(sizeof(*_tmp554));
_tmp554[0]=({struct Cyc_Absyn_Function_mod_struct _tmp555;_tmp555.tag=3;_tmp555.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp556=_cycalloc(sizeof(*_tmp556));
_tmp556[0]=({struct Cyc_Absyn_NoTypes_struct _tmp557;_tmp557.tag=0;_tmp557.f1=Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp557.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp557;});
_tmp556;}));_tmp555;});_tmp554;}));_tmp553->tl=(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp553;});_tmp552;});
_tmp551;});break;case 161: _LL29A: {struct Cyc_List_List*_tmp558=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp559;(_tmp559.YY28).tag=
32;(_tmp559.YY28).f1=({struct Cyc_Parse_Declarator*_tmp55A=_cycalloc(sizeof(*
_tmp55A));_tmp55A->id=(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp55A->tms=({struct Cyc_List_List*_tmp55B=_cycalloc(
sizeof(*_tmp55B));_tmp55B->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp55D;_tmp55D.tag=4;_tmp55D.f1=_tmp558;_tmp55D.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp55D.f3=0;
_tmp55D;});_tmp55C;}));_tmp55B->tl=(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp55B;});_tmp55A;});
_tmp559;});break;}case 162: _LL29B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp55E;(_tmp55E.YY28).tag=32;(_tmp55E.YY28).f1=({struct Cyc_Parse_Declarator*
_tmp55F=_cycalloc(sizeof(*_tmp55F));_tmp55F->id=(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp55F->tms=({
struct Cyc_List_List*_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp561=_cycalloc(sizeof(*_tmp561));
_tmp561[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp562;_tmp562.tag=5;_tmp562.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp562.f2=Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp562;});_tmp561;}));
_tmp560->tl=(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp560;});_tmp55F;});_tmp55E;});break;case 163: _LL29C:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp563;(_tmp563.YY28).tag=
32;(_tmp563.YY28).f1=({struct Cyc_Parse_Declarator*_tmp564=_cycalloc(sizeof(*
_tmp564));_tmp564->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp564->tms=0;_tmp564;});_tmp563;});break;case 164: _LL29D:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp565;(_tmp565.YY28).tag=
32;(_tmp565.YY28).f1=({struct Cyc_Parse_Declarator*_tmp566=_cycalloc(sizeof(*
_tmp566));_tmp566->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp566->tms=0;_tmp566;});_tmp565;});break;case 165: _LL29E:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 166:
_LL29F: {struct Cyc_Parse_Declarator*_tmp567=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp567->tms=({struct Cyc_List_List*
_tmp568=_cycalloc(sizeof(*_tmp568));_tmp568->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp56A;_tmp56A.tag=5;_tmp56A.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp56A.f2=
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp56A;});_tmp569;}));_tmp568->tl=_tmp567->tms;_tmp568;});yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 167: _LL2A0:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp56B;(_tmp56B.YY28).tag=
32;(_tmp56B.YY28).f1=({struct Cyc_Parse_Declarator*_tmp56C=_cycalloc(sizeof(*
_tmp56C));_tmp56C->id=(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp56C->tms=({struct Cyc_List_List*_tmp56D=_cycalloc(
sizeof(*_tmp56D));_tmp56D->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp56E=_cycalloc(sizeof(*_tmp56E));_tmp56E[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp56F;_tmp56F.tag=0;_tmp56F.f1=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp56F.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp56F;});
_tmp56E;}));_tmp56D->tl=(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms;_tmp56D;});_tmp56C;});_tmp56B;});break;case 168:
_LL2A1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp570;(_tmp570.YY28).tag=
32;(_tmp570.YY28).f1=({struct Cyc_Parse_Declarator*_tmp571=_cycalloc(sizeof(*
_tmp571));_tmp571->id=(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp571->tms=({struct Cyc_List_List*_tmp572=_cycalloc(
sizeof(*_tmp572));_tmp572->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp574;_tmp574.tag=1;_tmp574.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp574.f2=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp574.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp574;});
_tmp573;}));_tmp572->tl=(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms;_tmp572;});_tmp571;});_tmp570;});break;case 169:
_LL2A2: {struct _tuple17 _tmp576;struct Cyc_List_List*_tmp577;int _tmp578;struct Cyc_Absyn_VarargInfo*
_tmp579;struct Cyc_Core_Opt*_tmp57A;struct Cyc_List_List*_tmp57B;struct _tuple17*
_tmp575=Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp576=*_tmp575;_tmp577=_tmp576.f1;_tmp578=_tmp576.f2;_tmp579=_tmp576.f3;
_tmp57A=_tmp576.f4;_tmp57B=_tmp576.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp57C;(_tmp57C.YY28).tag=32;(_tmp57C.YY28).f1=({struct Cyc_Parse_Declarator*
_tmp57D=_cycalloc(sizeof(*_tmp57D));_tmp57D->id=(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp57D->tms=({
struct Cyc_List_List*_tmp57E=_cycalloc(sizeof(*_tmp57E));_tmp57E->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp57F=_cycalloc(sizeof(*_tmp57F));
_tmp57F[0]=({struct Cyc_Absyn_Function_mod_struct _tmp580;_tmp580.tag=3;_tmp580.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp581=_cycalloc(sizeof(*
_tmp581));_tmp581[0]=({struct Cyc_Absyn_WithTypes_struct _tmp582;_tmp582.tag=1;
_tmp582.f1=_tmp577;_tmp582.f2=_tmp578;_tmp582.f3=_tmp579;_tmp582.f4=_tmp57A;
_tmp582.f5=_tmp57B;_tmp582;});_tmp581;}));_tmp580;});_tmp57F;}));_tmp57E->tl=(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;
_tmp57E;});_tmp57D;});_tmp57C;});break;}case 170: _LL2A3: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp583;(_tmp583.YY28).tag=32;(_tmp583.YY28).f1=({struct
Cyc_Parse_Declarator*_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584->id=(Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp584->tms=({
struct Cyc_List_List*_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp586=_cycalloc(sizeof(*_tmp586));
_tmp586[0]=({struct Cyc_Absyn_Function_mod_struct _tmp587;_tmp587.tag=3;_tmp587.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp588=_cycalloc(sizeof(*
_tmp588));_tmp588[0]=({struct Cyc_Absyn_WithTypes_struct _tmp589;_tmp589.tag=1;
_tmp589.f1=0;_tmp589.f2=0;_tmp589.f3=0;_tmp589.f4=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp589.f5=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp589;});_tmp588;}));
_tmp587;});_tmp586;}));_tmp585->tl=(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp585;});_tmp584;});
_tmp583;});break;case 171: _LL2A4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp58A;(_tmp58A.YY28).tag=32;(_tmp58A.YY28).f1=({struct Cyc_Parse_Declarator*
_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B->id=(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp58B->tms=({
struct Cyc_List_List*_tmp58C=_cycalloc(sizeof(*_tmp58C));_tmp58C->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp58D=_cycalloc(sizeof(*_tmp58D));
_tmp58D[0]=({struct Cyc_Absyn_Function_mod_struct _tmp58E;_tmp58E.tag=3;_tmp58E.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp58F=_cycalloc(sizeof(*_tmp58F));
_tmp58F[0]=({struct Cyc_Absyn_NoTypes_struct _tmp590;_tmp590.tag=0;_tmp590.f1=Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp590.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp590;});
_tmp58F;}));_tmp58E;});_tmp58D;}));_tmp58C->tl=(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp58C;});_tmp58B;});
_tmp58A;});break;case 172: _LL2A5: {struct Cyc_List_List*_tmp591=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp592;(_tmp592.YY28).tag=
32;(_tmp592.YY28).f1=({struct Cyc_Parse_Declarator*_tmp593=_cycalloc(sizeof(*
_tmp593));_tmp593->id=(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp593->tms=({struct Cyc_List_List*_tmp594=_cycalloc(
sizeof(*_tmp594));_tmp594->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp595=_cycalloc(sizeof(*_tmp595));_tmp595[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp596;_tmp596.tag=4;_tmp596.f1=_tmp591;_tmp596.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp596.f3=0;
_tmp596;});_tmp595;}));_tmp594->tl=(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp594;});_tmp593;});
_tmp592;});break;}case 173: _LL2A6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp597;(_tmp597.YY28).tag=32;(_tmp597.YY28).f1=({struct Cyc_Parse_Declarator*
_tmp598=_cycalloc(sizeof(*_tmp598));_tmp598->id=(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp598->tms=({
struct Cyc_List_List*_tmp599=_cycalloc(sizeof(*_tmp599));_tmp599->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp59A=_cycalloc(sizeof(*_tmp59A));
_tmp59A[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp59B;_tmp59B.tag=5;_tmp59B.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp59B.f2=Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp59B;});_tmp59A;}));
_tmp599->tl=(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp599;});_tmp598;});_tmp597;});break;case 174: _LL2A7:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 175:
_LL2A8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp59C;(_tmp59C.YY27).tag=
31;(_tmp59C.YY27).f1=Cyc_List_imp_append(Cyc_yyget_YY27(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY27(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp59C;});break;case 176:
_LL2A9: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)ans=({struct Cyc_List_List*
_tmp59D=_cycalloc(sizeof(*_tmp59D));_tmp59D->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp59E=_cycalloc(sizeof(*_tmp59E));_tmp59E[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp59F;_tmp59F.tag=5;_tmp59F.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp59F.f2=
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp59F;});_tmp59E;}));_tmp59D->tl=ans;_tmp59D;});{struct Cyc_Absyn_PtrLoc*
_tmp5A0=({struct Cyc_Absyn_PtrLoc*_tmp5A6=_cycalloc(sizeof(*_tmp5A6));_tmp5A6->ptr_loc=(*
Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f1;
_tmp5A6->rgn_loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp5A6->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line);_tmp5A6;});ans=({struct Cyc_List_List*_tmp5A1=
_cycalloc(sizeof(*_tmp5A1));_tmp5A1->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp5A3;_tmp5A3.tag=2;_tmp5A3.f1=({struct Cyc_Absyn_PtrAtts _tmp5A4;_tmp5A4.rgn=(
void*)Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5A4.nullable=(*Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])).f2;_tmp5A4.bounds=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f3;_tmp5A4.zero_term=
Cyc_yyget_YY50(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp5A4.ptrloc=_tmp5A0;_tmp5A4;});_tmp5A3.f2=Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5A3;});_tmp5A2;}));
_tmp5A1->tl=ans;_tmp5A1;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5A5;(_tmp5A5.YY27).tag=31;(_tmp5A5.YY27).f1=ans;_tmp5A5;});break;}}case 177:
_LL2AA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5A7;(_tmp5A7.YY1).tag=
5;(_tmp5A7.YY1).f1=({struct _tuple12*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));_tmp5A8->f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp5A8->f2=Cyc_Absyn_true_conref;_tmp5A8->f3=Cyc_yyget_YY2(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5A8;});_tmp5A7;});
break;case 178: _LL2AB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5A9;(_tmp5A9.YY1).tag=5;(_tmp5A9.YY1).f1=({struct _tuple12*_tmp5AA=_cycalloc(
sizeof(*_tmp5AA));_tmp5AA->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp5AA->f2=
Cyc_Absyn_false_conref;_tmp5AA->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5AA;});_tmp5A9;});
break;case 179: _LL2AC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5AB;(_tmp5AB.YY1).tag=5;(_tmp5AB.YY1).f1=({struct _tuple12*_tmp5AC=_cycalloc(
sizeof(*_tmp5AC));_tmp5AC->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5AC->f2=Cyc_Absyn_true_conref;
_tmp5AC->f3=Cyc_Absyn_bounds_dynforward_conref;_tmp5AC;});_tmp5AB;});break;case
180: _LL2AD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5AD;(
_tmp5AD.YY1).tag=5;(_tmp5AD.YY1).f1=({struct _tuple12*_tmp5AE=_cycalloc(sizeof(*
_tmp5AE));_tmp5AE->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp5AE->f2=
Cyc_Absyn_true_conref;_tmp5AE->f3=Cyc_Absyn_bounds_dyneither_conref;_tmp5AE;});
_tmp5AD;});break;case 181: _LL2AE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5AF;(_tmp5AF.YY2).tag=6;(_tmp5AF.YY2).f1=Cyc_Absyn_bounds_one_conref;_tmp5AF;});
break;case 182: _LL2AF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5B0;(_tmp5B0.YY2).tag=6;(_tmp5B0.YY2).f1=Cyc_Absyn_new_conref((void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp5B1=_cycalloc(sizeof(*_tmp5B1));_tmp5B1[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp5B2;_tmp5B2.tag=0;_tmp5B2.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5B2;});_tmp5B1;}));
_tmp5B0;});break;case 183: _LL2B0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5B3;(_tmp5B3.YY2).tag=6;(_tmp5B3.YY2).f1=Cyc_Absyn_new_conref((void*)({
struct Cyc_Absyn_AbsUpper_b_struct*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));_tmp5B4[0]=({
struct Cyc_Absyn_AbsUpper_b_struct _tmp5B5;_tmp5B5.tag=1;_tmp5B5.f1=(void*)Cyc_yyget_YY43(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5B5;});_tmp5B4;}));
_tmp5B3;});break;case 184: _LL2B1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5B6;(_tmp5B6.YY50).tag=54;(_tmp5B6.YY50).f1=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp5B6;});break;case 185: _LL2B2: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5B7;(_tmp5B7.YY50).tag=54;(_tmp5B7.YY50).f1=Cyc_Absyn_true_conref;
_tmp5B7;});break;case 186: _LL2B3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5B8;(_tmp5B8.YY50).tag=54;(_tmp5B8.YY50).f1=Cyc_Absyn_false_conref;_tmp5B8;});
break;case 187: _LL2B4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5B9;(_tmp5B9.YY48).tag=52;(_tmp5B9.YY48).f1=0;_tmp5B9;});break;case 188:
_LL2B5: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY43(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5BA;(_tmp5BA.YY48).tag=52;(_tmp5BA.YY48).f1=({struct
Cyc_Core_Opt*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB->v=(void*)Cyc_yyget_YY43(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5BB;});_tmp5BA;});
break;case 189: _LL2B6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5BC;(_tmp5BC.YY48).tag=52;(_tmp5BC.YY48).f1=({struct Cyc_Core_Opt*_tmp5BD=
_cycalloc(sizeof(*_tmp5BD));_tmp5BD->v=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE->v=(void*)((void*)3);_tmp5BE;}),0);
_tmp5BD;});_tmp5BC;});break;case 190: _LL2B7: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp5BF;(_tmp5BF.YY43).tag=47;(_tmp5BF.YY43).f1=(void*)Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));_tmp5C0->v=(void*)((void*)
3);_tmp5C0;}),0);_tmp5BF;});break;case 191: _LL2B8: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY43(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3);yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 192: _LL2B9: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5C1;(_tmp5C1.YY43).tag=47;(
_tmp5C1.YY43).f1=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp5C2=
_cycalloc(sizeof(*_tmp5C2));_tmp5C2->v=(void*)((void*)3);_tmp5C2;}),0);_tmp5C1;});
break;case 193: _LL2BA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5C3;(_tmp5C3.YY24).tag=28;(_tmp5C3.YY24).f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset + 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line));_tmp5C3;});
break;case 194: _LL2BB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5C4;(_tmp5C4.YY24).tag=28;(_tmp5C4.YY24).f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY24(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5C4;});break;case 195:
_LL2BC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5C5;(_tmp5C5.YY38).tag=
42;(_tmp5C5.YY38).f1=({struct _tuple17*_tmp5C6=_cycalloc(sizeof(*_tmp5C6));
_tmp5C6->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp5C6->f2=0;_tmp5C6->f3=0;_tmp5C6->f4=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5C6->f5=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C6;});_tmp5C5;});
break;case 196: _LL2BD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5C7;(_tmp5C7.YY38).tag=42;(_tmp5C7.YY38).f1=({struct _tuple17*_tmp5C8=
_cycalloc(sizeof(*_tmp5C8));_tmp5C8->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]));_tmp5C8->f2=1;_tmp5C8->f3=0;_tmp5C8->f4=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5C8->f5=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C8;});_tmp5C7;});
break;case 197: _LL2BE: {struct _tuple2 _tmp5CA;struct Cyc_Core_Opt*_tmp5CB;struct Cyc_Absyn_Tqual
_tmp5CC;void*_tmp5CD;struct _tuple2*_tmp5C9=Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5CA=*_tmp5C9;_tmp5CB=
_tmp5CA.f1;_tmp5CC=_tmp5CA.f2;_tmp5CD=_tmp5CA.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5CE=({struct Cyc_Absyn_VarargInfo*_tmp5D1=_cycalloc(sizeof(*_tmp5D1));_tmp5D1->name=
_tmp5CB;_tmp5D1->tq=_tmp5CC;_tmp5D1->type=(void*)_tmp5CD;_tmp5D1->inject=Cyc_yyget_YY30(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5D1;});yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5CF;(_tmp5CF.YY38).tag=42;(
_tmp5CF.YY38).f1=({struct _tuple17*_tmp5D0=_cycalloc(sizeof(*_tmp5D0));_tmp5D0->f1=
0;_tmp5D0->f2=0;_tmp5D0->f3=_tmp5CE;_tmp5D0->f4=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D0->f5=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D0;});_tmp5CF;});
break;}}case 198: _LL2BF: {struct _tuple2 _tmp5D3;struct Cyc_Core_Opt*_tmp5D4;struct
Cyc_Absyn_Tqual _tmp5D5;void*_tmp5D6;struct _tuple2*_tmp5D2=Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5D3=*_tmp5D2;_tmp5D4=
_tmp5D3.f1;_tmp5D5=_tmp5D3.f2;_tmp5D6=_tmp5D3.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5D7=({struct Cyc_Absyn_VarargInfo*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA->name=
_tmp5D4;_tmp5DA->tq=_tmp5D5;_tmp5DA->type=(void*)_tmp5D6;_tmp5DA->inject=Cyc_yyget_YY30(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5DA;});yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5D8;(_tmp5D8.YY38).tag=42;(
_tmp5D8.YY38).f1=({struct _tuple17*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp5D9->f2=0;
_tmp5D9->f3=_tmp5D7;_tmp5D9->f4=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D9->f5=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D9;});_tmp5D8;});
break;}}case 199: _LL2C0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5DB;(_tmp5DB.YY43).tag=47;(_tmp5DB.YY43).f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp5DC=_cycalloc(sizeof(*_tmp5DC));_tmp5DC[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp5DD;_tmp5DD.tag=1;_tmp5DD.f1=0;_tmp5DD;});_tmp5DC;}));_tmp5DB;});break;case
200: _LL2C1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5DE;(
_tmp5DE.YY43).tag=47;(_tmp5DE.YY43).f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5DF=_cycalloc(sizeof(*_tmp5DF));_tmp5DF[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5E0;_tmp5E0.tag=0;_tmp5E0.f1=(void*)Cyc_yyget_YY42(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E0;});_tmp5DF;}));
_tmp5DE;});break;case 201: _LL2C2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5E1;(_tmp5E1.YY48).tag=52;(_tmp5E1.YY48).f1=0;_tmp5E1;});break;case 202:
_LL2C3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5E2;(_tmp5E2.YY48).tag=
52;(_tmp5E2.YY48).f1=({struct Cyc_Core_Opt*_tmp5E3=_cycalloc(sizeof(*_tmp5E3));
_tmp5E3->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp5E4=_cycalloc(
sizeof(*_tmp5E4));_tmp5E4[0]=({struct Cyc_Absyn_JoinEff_struct _tmp5E5;_tmp5E5.tag=
20;_tmp5E5.f1=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp5E5;});_tmp5E4;}));_tmp5E3;});_tmp5E2;});break;case 203:
_LL2C4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5E6;(_tmp5E6.YY49).tag=
53;(_tmp5E6.YY49).f1=0;_tmp5E6;});break;case 204: _LL2C5: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 205: _LL2C6: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5E7;(_tmp5E7.YY49).tag=53;(
_tmp5E7.YY49).f1=({struct Cyc_List_List*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));
_tmp5E8->hd=({struct _tuple4*_tmp5E9=_cycalloc(sizeof(*_tmp5E9));_tmp5E9->f1=(
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));
_tmp5EC[0]=({struct Cyc_Absyn_JoinEff_struct _tmp5ED;_tmp5ED.tag=20;_tmp5ED.f1=Cyc_yyget_YY39(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5ED;});_tmp5EC;});
_tmp5E9->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5EA=_cycalloc(sizeof(*_tmp5EA));_tmp5EA[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5EB;_tmp5EB.tag=0;_tmp5EB.f1=(void*)((void*)3);_tmp5EB;});_tmp5EA;}));
_tmp5E9;});_tmp5E8->tl=0;_tmp5E8;});_tmp5E7;});break;case 206: _LL2C7: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5EE;(_tmp5EE.YY49).tag=53;(_tmp5EE.YY49).f1=({
struct Cyc_List_List*_tmp5EF=_cycalloc(sizeof(*_tmp5EF));_tmp5EF->hd=({struct
_tuple4*_tmp5F0=_cycalloc(sizeof(*_tmp5F0));_tmp5F0->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp5F3=_cycalloc(sizeof(*_tmp5F3));_tmp5F3[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp5F4;_tmp5F4.tag=20;_tmp5F4.f1=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp5F4;});_tmp5F3;});
_tmp5F0->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5F1=_cycalloc(sizeof(*_tmp5F1));_tmp5F1[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5F2;_tmp5F2.tag=0;_tmp5F2.f1=(void*)((void*)3);_tmp5F2;});_tmp5F1;}));
_tmp5F0;});_tmp5EF->tl=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp5EF;});_tmp5EE;});break;case 207: _LL2C8: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5F5;(_tmp5F5.YY30).tag=34;(_tmp5F5.YY30).f1=0;_tmp5F5;});
break;case 208: _LL2C9: if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp5F6="inject";
_tag_dynforward(_tmp5F6,sizeof(char),_get_zero_arr_size(_tmp5F6,7));}))!= 0)Cyc_Parse_err(({
const char*_tmp5F7="missing type in function declaration";_tag_dynforward(_tmp5F7,
sizeof(char),_get_zero_arr_size(_tmp5F7,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5F8;(_tmp5F8.YY30).tag=34;(_tmp5F8.YY30).f1=1;_tmp5F8;});
break;case 209: _LL2CA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 210: _LL2CB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5F9;(_tmp5F9.YY39).tag=43;(_tmp5F9.YY39).f1=Cyc_List_imp_append(Cyc_yyget_YY39(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5F9;});break;case 211:
_LL2CC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5FA;(_tmp5FA.YY39).tag=
43;(_tmp5FA.YY39).f1=0;_tmp5FA;});break;case 212: _LL2CD: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 213: _LL2CE:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5FB;(_tmp5FB.YY39).tag=
43;(_tmp5FB.YY39).f1=({struct Cyc_List_List*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));
_tmp5FC->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp5FD=_cycalloc(
sizeof(*_tmp5FD));_tmp5FD[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp5FE;_tmp5FE.tag=
21;_tmp5FE.f1=(void*)Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp5FE;});_tmp5FD;}));_tmp5FC->tl=0;_tmp5FC;});_tmp5FB;});
break;case 214: _LL2CF: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY43(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)4);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5FF;(_tmp5FF.YY39).tag=43;(_tmp5FF.YY39).f1=({struct
Cyc_List_List*_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->hd=(void*)Cyc_yyget_YY43(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp600->tl=0;_tmp600;});
_tmp5FF;});break;case 215: _LL2D0: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY43(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp601;(_tmp601.YY39).tag=43;(_tmp601.YY39).f1=({struct
Cyc_List_List*_tmp602=_cycalloc(sizeof(*_tmp602));_tmp602->hd=(void*)((void*)({
struct Cyc_Absyn_AccessEff_struct*_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603[0]=({
struct Cyc_Absyn_AccessEff_struct _tmp604;_tmp604.tag=19;_tmp604.f1=(void*)Cyc_yyget_YY43(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp604;});_tmp603;}));
_tmp602->tl=0;_tmp602;});_tmp601;});break;case 216: _LL2D1: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(
void*)3);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp605;(_tmp605.YY39).tag=
43;(_tmp605.YY39).f1=({struct Cyc_List_List*_tmp606=_cycalloc(sizeof(*_tmp606));
_tmp606->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp607=_cycalloc(
sizeof(*_tmp607));_tmp607[0]=({struct Cyc_Absyn_AccessEff_struct _tmp608;_tmp608.tag=
19;_tmp608.f1=(void*)Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp608;});_tmp607;}));_tmp606->tl=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp606;});_tmp605;});
break;case 217: _LL2D2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp609;(_tmp609.YY37).tag=41;(_tmp609.YY37).f1=({struct Cyc_List_List*_tmp60A=
_cycalloc(sizeof(*_tmp60A));_tmp60A->hd=Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp60A->tl=0;_tmp60A;});
_tmp609;});break;case 218: _LL2D3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp60B;(_tmp60B.YY37).tag=41;(_tmp60B.YY37).f1=({struct Cyc_List_List*_tmp60C=
_cycalloc(sizeof(*_tmp60C));_tmp60C->hd=Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp60C->tl=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp60C;});_tmp60B;});
break;case 219: _LL2D4: {struct _tuple16 _tmp60E;struct Cyc_Absyn_Tqual _tmp60F;struct
Cyc_List_List*_tmp610;struct Cyc_List_List*_tmp611;struct _tuple16*_tmp60D=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp60E=*_tmp60D;
_tmp60F=_tmp60E.f1;_tmp610=_tmp60E.f2;_tmp611=_tmp60E.f3;if(_tmp60F.loc == 0)
_tmp60F.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator _tmp613;struct _tuple1*
_tmp614;struct Cyc_List_List*_tmp615;struct Cyc_Parse_Declarator*_tmp612=Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp613=*_tmp612;
_tmp614=_tmp613.id;_tmp615=_tmp613.tms;{void*_tmp616=Cyc_Parse_speclist2typ(
_tmp610,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp618;void*_tmp619;struct
Cyc_List_List*_tmp61A;struct Cyc_List_List*_tmp61B;struct _tuple6 _tmp617=Cyc_Parse_apply_tms(
_tmp60F,_tmp616,_tmp611,_tmp615);_tmp618=_tmp617.f1;_tmp619=_tmp617.f2;_tmp61A=
_tmp617.f3;_tmp61B=_tmp617.f4;if(_tmp61A != 0)Cyc_Parse_err(({const char*_tmp61C="parameter with bad type params";
_tag_dynforward(_tmp61C,sizeof(char),_get_zero_arr_size(_tmp61C,31));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp614))Cyc_Parse_err(({const char*_tmp61D="parameter cannot be qualified with a namespace";
_tag_dynforward(_tmp61D,sizeof(char),_get_zero_arr_size(_tmp61D,47));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp61E=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp623=_cycalloc(sizeof(*
_tmp623));_tmp623->v=(*_tmp614).f2;_tmp623;});if(_tmp61B != 0)({void*_tmp61F[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp620="extra attributes on parameter, ignoring";
_tag_dynforward(_tmp620,sizeof(char),_get_zero_arr_size(_tmp620,40));}),
_tag_dynforward(_tmp61F,sizeof(void*),0));});yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp621;(_tmp621.YY36).tag=40;(_tmp621.YY36).f1=({struct
_tuple2*_tmp622=_cycalloc(sizeof(*_tmp622));_tmp622->f1=_tmp61E;_tmp622->f2=
_tmp618;_tmp622->f3=_tmp619;_tmp622;});_tmp621;});break;}}}}case 220: _LL2D5: {
struct _tuple16 _tmp625;struct Cyc_Absyn_Tqual _tmp626;struct Cyc_List_List*_tmp627;
struct Cyc_List_List*_tmp628;struct _tuple16*_tmp624=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp625=*_tmp624;_tmp626=
_tmp625.f1;_tmp627=_tmp625.f2;_tmp628=_tmp625.f3;if(_tmp626.loc == 0)_tmp626.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);{void*_tmp629=Cyc_Parse_speclist2typ(_tmp627,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp628 != 0)({
void*_tmp62A[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp62B="bad attributes on parameter, ignoring";_tag_dynforward(_tmp62B,sizeof(
char),_get_zero_arr_size(_tmp62B,38));}),_tag_dynforward(_tmp62A,sizeof(void*),0));});
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp62C;(_tmp62C.YY36).tag=
40;(_tmp62C.YY36).f1=({struct _tuple2*_tmp62D=_cycalloc(sizeof(*_tmp62D));_tmp62D->f1=
0;_tmp62D->f2=_tmp626;_tmp62D->f3=_tmp629;_tmp62D;});_tmp62C;});break;}}case 221:
_LL2D6: {struct _tuple16 _tmp62F;struct Cyc_Absyn_Tqual _tmp630;struct Cyc_List_List*
_tmp631;struct Cyc_List_List*_tmp632;struct _tuple16*_tmp62E=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp62F=*_tmp62E;_tmp630=
_tmp62F.f1;_tmp631=_tmp62F.f2;_tmp632=_tmp62F.f3;if(_tmp630.loc == 0)_tmp630.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{void*_tmp633=Cyc_Parse_speclist2typ(_tmp631,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp634=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp636;void*_tmp637;struct Cyc_List_List*_tmp638;struct Cyc_List_List*
_tmp639;struct _tuple6 _tmp635=Cyc_Parse_apply_tms(_tmp630,_tmp633,_tmp632,_tmp634);
_tmp636=_tmp635.f1;_tmp637=_tmp635.f2;_tmp638=_tmp635.f3;_tmp639=_tmp635.f4;if(
_tmp638 != 0)({void*_tmp63A[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp63B="bad type parameters on formal argument, ignoring";_tag_dynforward(
_tmp63B,sizeof(char),_get_zero_arr_size(_tmp63B,49));}),_tag_dynforward(_tmp63A,
sizeof(void*),0));});if(_tmp639 != 0)({void*_tmp63C[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp63D="bad attributes on parameter, ignoring";_tag_dynforward(_tmp63D,sizeof(
char),_get_zero_arr_size(_tmp63D,38));}),_tag_dynforward(_tmp63C,sizeof(void*),0));});
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp63E;(_tmp63E.YY36).tag=
40;(_tmp63E.YY36).f1=({struct _tuple2*_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F->f1=
0;_tmp63F->f2=_tmp636;_tmp63F->f3=_tmp637;_tmp63F;});_tmp63E;});break;}}case 222:
_LL2D7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp640;(_tmp640.YY35).tag=
39;(_tmp640.YY35).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));
_tmp640;});break;case 223: _LL2D8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp641;(_tmp641.YY35).tag=39;(_tmp641.YY35).f1=({struct Cyc_List_List*_tmp642=
_cycalloc(sizeof(*_tmp642));_tmp642->hd=({struct _dynforward_ptr*_tmp643=
_cycalloc(sizeof(*_tmp643));_tmp643[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp643;});_tmp642->tl=0;
_tmp642;});_tmp641;});break;case 224: _LL2D9: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp644;(_tmp644.YY35).tag=39;(_tmp644.YY35).f1=({struct Cyc_List_List*
_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645->hd=({struct _dynforward_ptr*_tmp646=
_cycalloc(sizeof(*_tmp646));_tmp646[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp646;});_tmp645->tl=Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp645;});_tmp644;});
break;case 225: _LL2DA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 226: _LL2DB: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 227: _LL2DC: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp647;(_tmp647.YY3).tag=7;(_tmp647.YY3).f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp648=_cycalloc(sizeof(*_tmp648));
_tmp648[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp649;_tmp649.tag=36;
_tmp649.f1=0;_tmp649.f2=0;_tmp649;});_tmp648;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp647;});
break;case 228: _LL2DD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp64A;(_tmp64A.YY3).tag=7;(_tmp64A.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnresolvedMem_e_struct*_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B[0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp64C;_tmp64C.tag=36;_tmp64C.f1=0;
_tmp64C.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));
_tmp64C;});_tmp64B;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp64A;});
break;case 229: _LL2DE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp64D;(_tmp64D.YY3).tag=7;(_tmp64D.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnresolvedMem_e_struct*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E[0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp64F;_tmp64F.tag=36;_tmp64F.f1=0;
_tmp64F.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp64F;});_tmp64E;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp64D;});
break;case 230: _LL2DF: {struct Cyc_Absyn_Vardecl*_tmp650=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654->f1=(void*)0;_tmp654->f2=({
struct _dynforward_ptr*_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp655;});_tmp654;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp650->tq).real_const=
1;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp651;(_tmp651.YY3).tag=
7;(_tmp651.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp652=_cycalloc(sizeof(*_tmp652));_tmp652[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp653;_tmp653.tag=29;_tmp653.f1=_tmp650;_tmp653.f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp653.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp653.f4=0;
_tmp653;});_tmp652;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp651;});
break;}case 231: _LL2E0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp656;(_tmp656.YY5).tag=9;(_tmp656.YY5).f1=({struct Cyc_List_List*_tmp657=
_cycalloc(sizeof(*_tmp657));_tmp657->hd=({struct _tuple20*_tmp658=_cycalloc(
sizeof(*_tmp658));_tmp658->f1=0;_tmp658->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp658;});_tmp657->tl=0;
_tmp657;});_tmp656;});break;case 232: _LL2E1: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp659;(_tmp659.YY5).tag=9;(_tmp659.YY5).f1=({struct Cyc_List_List*
_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A->hd=({struct _tuple20*_tmp65B=
_cycalloc(sizeof(*_tmp65B));_tmp65B->f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp65B->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp65B;});_tmp65A->tl=
0;_tmp65A;});_tmp659;});break;case 233: _LL2E2: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp65C;(_tmp65C.YY5).tag=9;(_tmp65C.YY5).f1=({struct Cyc_List_List*
_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D->hd=({struct _tuple20*_tmp65E=
_cycalloc(sizeof(*_tmp65E));_tmp65E->f1=0;_tmp65E->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp65E;});_tmp65D->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp65D;});_tmp65C;});
break;case 234: _LL2E3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp65F;(_tmp65F.YY5).tag=9;(_tmp65F.YY5).f1=({struct Cyc_List_List*_tmp660=
_cycalloc(sizeof(*_tmp660));_tmp660->hd=({struct _tuple20*_tmp661=_cycalloc(
sizeof(*_tmp661));_tmp661->f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp661->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp661;});_tmp660->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp660;});_tmp65F;});
break;case 235: _LL2E4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp662;(_tmp662.YY40).tag=44;(_tmp662.YY40).f1=Cyc_List_imp_rev(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp662;});break;
case 236: _LL2E5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp663;(
_tmp663.YY40).tag=44;(_tmp663.YY40).f1=({struct Cyc_List_List*_tmp664=_cycalloc(
sizeof(*_tmp664));_tmp664->hd=(void*)Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp664->tl=0;_tmp664;});
_tmp663;});break;case 237: _LL2E6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp665;(_tmp665.YY40).tag=44;(_tmp665.YY40).f1=({struct Cyc_List_List*_tmp666=
_cycalloc(sizeof(*_tmp666));_tmp666->hd=(void*)Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp666->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp666;});_tmp665;});
break;case 238: _LL2E7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp667;(_tmp667.YY41).tag=45;(_tmp667.YY41).f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp669;_tmp669.tag=0;_tmp669.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp669;});_tmp668;}));
_tmp667;});break;case 239: _LL2E8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp66A;(_tmp66A.YY41).tag=45;(_tmp66A.YY41).f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp66B=_cycalloc(sizeof(*_tmp66B));_tmp66B[0]=({struct Cyc_Absyn_FieldName_struct
_tmp66C;_tmp66C.tag=1;_tmp66C.f1=({struct _dynforward_ptr*_tmp66D=_cycalloc(
sizeof(*_tmp66D));_tmp66D[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp66D;});_tmp66C;});
_tmp66B;}));_tmp66A;});break;case 240: _LL2E9: {struct _tuple16 _tmp66F;struct Cyc_Absyn_Tqual
_tmp670;struct Cyc_List_List*_tmp671;struct Cyc_List_List*_tmp672;struct _tuple16*
_tmp66E=Cyc_yyget_YY34(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp66F=*_tmp66E;_tmp670=_tmp66F.f1;_tmp671=_tmp66F.f2;_tmp672=_tmp66F.f3;{void*
_tmp673=Cyc_Parse_speclist2typ(_tmp671,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp672 != 0)({
void*_tmp674[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp675="ignoring attributes in type";_tag_dynforward(_tmp675,sizeof(char),
_get_zero_arr_size(_tmp675,28));}),_tag_dynforward(_tmp674,sizeof(void*),0));});
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp676;(_tmp676.YY36).tag=
40;(_tmp676.YY36).f1=({struct _tuple2*_tmp677=_cycalloc(sizeof(*_tmp677));_tmp677->f1=
0;_tmp677->f2=_tmp670;_tmp677->f3=_tmp673;_tmp677;});_tmp676;});break;}}case 241:
_LL2EA: {struct _tuple16 _tmp679;struct Cyc_Absyn_Tqual _tmp67A;struct Cyc_List_List*
_tmp67B;struct Cyc_List_List*_tmp67C;struct _tuple16*_tmp678=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp679=*_tmp678;_tmp67A=
_tmp679.f1;_tmp67B=_tmp679.f2;_tmp67C=_tmp679.f3;{void*_tmp67D=Cyc_Parse_speclist2typ(
_tmp67B,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp67E=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct _tuple6 _tmp67F=
Cyc_Parse_apply_tms(_tmp67A,_tmp67D,_tmp67C,_tmp67E);if(_tmp67F.f3 != 0)({void*
_tmp680[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp681="bad type params, ignoring";_tag_dynforward(_tmp681,sizeof(char),
_get_zero_arr_size(_tmp681,26));}),_tag_dynforward(_tmp680,sizeof(void*),0));});
if(_tmp67F.f4 != 0)({void*_tmp682[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp683="bad specifiers, ignoring";_tag_dynforward(_tmp683,sizeof(char),
_get_zero_arr_size(_tmp683,25));}),_tag_dynforward(_tmp682,sizeof(void*),0));});
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp684;(_tmp684.YY36).tag=
40;(_tmp684.YY36).f1=({struct _tuple2*_tmp685=_cycalloc(sizeof(*_tmp685));_tmp685->f1=
0;_tmp685->f2=_tmp67F.f1;_tmp685->f3=_tmp67F.f2;_tmp685;});_tmp684;});break;}}
case 242: _LL2EB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp686;(
_tmp686.YY43).tag=47;(_tmp686.YY43).f1=(void*)(*Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp686;});break;case
243: _LL2EC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp687;(
_tmp687.YY43).tag=47;(_tmp687.YY43).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp688=_cycalloc(sizeof(*_tmp688));_tmp688[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp689;_tmp689.tag=20;_tmp689.f1=0;_tmp689;});_tmp688;}));_tmp687;});break;case
244: _LL2ED: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp68A;(
_tmp68A.YY43).tag=47;(_tmp68A.YY43).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp68B=_cycalloc(sizeof(*_tmp68B));_tmp68B[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp68C;_tmp68C.tag=20;_tmp68C.f1=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp68C;});_tmp68B;}));
_tmp68A;});break;case 245: _LL2EE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp68D;(_tmp68D.YY43).tag=47;(_tmp68D.YY43).f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp68E=_cycalloc(sizeof(*_tmp68E));_tmp68E[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp68F;_tmp68F.tag=21;_tmp68F.f1=(void*)Cyc_yyget_YY43(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp68F;});_tmp68E;}));
_tmp68D;});break;case 246: _LL2EF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp690;(_tmp690.YY43).tag=47;(_tmp690.YY43).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp692;_tmp692.tag=20;_tmp692.f1=({struct Cyc_List_List*_tmp693=_cycalloc(
sizeof(*_tmp693));_tmp693->hd=(void*)Cyc_yyget_YY43(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp693->tl=Cyc_yyget_YY39(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp693;});_tmp692;});
_tmp691;}));_tmp690;});break;case 247: _LL2F0: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp694;(_tmp694.YY39).tag=43;(_tmp694.YY39).f1=({struct
Cyc_List_List*_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695->hd=(void*)Cyc_yyget_YY43(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp695->tl=0;_tmp695;});
_tmp694;});break;case 248: _LL2F1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp696;(_tmp696.YY39).tag=43;(_tmp696.YY39).f1=({struct Cyc_List_List*_tmp697=
_cycalloc(sizeof(*_tmp697));_tmp697->hd=(void*)Cyc_yyget_YY43(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp697->tl=Cyc_yyget_YY39(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp697;});_tmp696;});
break;case 249: _LL2F2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp698;(_tmp698.YY29).tag=33;(_tmp698.YY29).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp699=_cycalloc(sizeof(*_tmp699));_tmp699->tms=Cyc_yyget_YY27(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp699;});_tmp698;});
break;case 250: _LL2F3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 251: _LL2F4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp69A;(_tmp69A.YY29).tag=33;(_tmp69A.YY29).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp69B=_cycalloc(sizeof(*_tmp69B));_tmp69B->tms=Cyc_List_imp_append(Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp69B;});
_tmp69A;});break;case 252: _LL2F5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 253: _LL2F6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp69C;(_tmp69C.YY29).tag=33;(_tmp69C.YY29).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp69D=_cycalloc(sizeof(*_tmp69D));_tmp69D->tms=({struct Cyc_List_List*_tmp69E=
_cycalloc(sizeof(*_tmp69E));_tmp69E->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp69F=_cycalloc(sizeof(*_tmp69F));_tmp69F[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6A0;_tmp6A0.tag=0;_tmp6A0.f1=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A0.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6A0;});
_tmp69F;}));_tmp69E->tl=0;_tmp69E;});_tmp69D;});_tmp69C;});break;case 254: _LL2F7:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6A1;(_tmp6A1.YY29).tag=
33;(_tmp6A1.YY29).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6A2=_cycalloc(
sizeof(*_tmp6A2));_tmp6A2->tms=({struct Cyc_List_List*_tmp6A3=_cycalloc(sizeof(*
_tmp6A3));_tmp6A3->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp6A4=
_cycalloc(sizeof(*_tmp6A4));_tmp6A4[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6A5;_tmp6A5.tag=0;_tmp6A5.f1=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A5.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6A5;});
_tmp6A4;}));_tmp6A3->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms;_tmp6A3;});_tmp6A2;});_tmp6A1;});break;case 255:
_LL2F8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6A6;(_tmp6A6.YY29).tag=
33;(_tmp6A6.YY29).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6A7=_cycalloc(
sizeof(*_tmp6A7));_tmp6A7->tms=({struct Cyc_List_List*_tmp6A8=_cycalloc(sizeof(*
_tmp6A8));_tmp6A8->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6A9=_cycalloc(sizeof(*_tmp6A9));_tmp6A9[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6AA;_tmp6AA.tag=1;_tmp6AA.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6AA.f2=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6AA.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6AA;});
_tmp6A9;}));_tmp6A8->tl=0;_tmp6A8;});_tmp6A7;});_tmp6A6;});break;case 256: _LL2F9:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6AB;(_tmp6AB.YY29).tag=
33;(_tmp6AB.YY29).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6AC=_cycalloc(
sizeof(*_tmp6AC));_tmp6AC->tms=({struct Cyc_List_List*_tmp6AD=_cycalloc(sizeof(*
_tmp6AD));_tmp6AD->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6AE=_cycalloc(sizeof(*_tmp6AE));_tmp6AE[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6AF;_tmp6AF.tag=1;_tmp6AF.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6AF.f2=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6AF.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6AF;});
_tmp6AE;}));_tmp6AD->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms;_tmp6AD;});_tmp6AC;});_tmp6AB;});break;case 257:
_LL2FA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6B0;(_tmp6B0.YY29).tag=
33;(_tmp6B0.YY29).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6B1=_cycalloc(
sizeof(*_tmp6B1));_tmp6B1->tms=({struct Cyc_List_List*_tmp6B2=_cycalloc(sizeof(*
_tmp6B2));_tmp6B2->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6B3=_cycalloc(sizeof(*_tmp6B3));_tmp6B3[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6B4;_tmp6B4.tag=3;_tmp6B4.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6B5=_cycalloc(sizeof(*_tmp6B5));_tmp6B5[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6B6;_tmp6B6.tag=1;_tmp6B6.f1=0;_tmp6B6.f2=0;_tmp6B6.f3=0;_tmp6B6.f4=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6B6.f5=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6B6;});_tmp6B5;}));
_tmp6B4;});_tmp6B3;}));_tmp6B2->tl=0;_tmp6B2;});_tmp6B1;});_tmp6B0;});break;case
258: _LL2FB: {struct _tuple17 _tmp6B8;struct Cyc_List_List*_tmp6B9;int _tmp6BA;struct
Cyc_Absyn_VarargInfo*_tmp6BB;struct Cyc_Core_Opt*_tmp6BC;struct Cyc_List_List*
_tmp6BD;struct _tuple17*_tmp6B7=Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6B8=*_tmp6B7;_tmp6B9=
_tmp6B8.f1;_tmp6BA=_tmp6B8.f2;_tmp6BB=_tmp6B8.f3;_tmp6BC=_tmp6B8.f4;_tmp6BD=
_tmp6B8.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6BE;(
_tmp6BE.YY29).tag=33;(_tmp6BE.YY29).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF->tms=({struct Cyc_List_List*_tmp6C0=
_cycalloc(sizeof(*_tmp6C0));_tmp6C0->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6C1=_cycalloc(sizeof(*_tmp6C1));_tmp6C1[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6C2;_tmp6C2.tag=3;_tmp6C2.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6C3=_cycalloc(sizeof(*_tmp6C3));_tmp6C3[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6C4;_tmp6C4.tag=1;_tmp6C4.f1=_tmp6B9;_tmp6C4.f2=_tmp6BA;_tmp6C4.f3=_tmp6BB;
_tmp6C4.f4=_tmp6BC;_tmp6C4.f5=_tmp6BD;_tmp6C4;});_tmp6C3;}));_tmp6C2;});_tmp6C1;}));
_tmp6C0->tl=0;_tmp6C0;});_tmp6BF;});_tmp6BE;});break;}case 259: _LL2FC: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6C5;(_tmp6C5.YY29).tag=33;(
_tmp6C5.YY29).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6C6=_cycalloc(sizeof(*
_tmp6C6));_tmp6C6->tms=({struct Cyc_List_List*_tmp6C7=_cycalloc(sizeof(*_tmp6C7));
_tmp6C7->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp6C8=
_cycalloc(sizeof(*_tmp6C8));_tmp6C8[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6C9;_tmp6C9.tag=3;_tmp6C9.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6CB;_tmp6CB.tag=1;_tmp6CB.f1=0;_tmp6CB.f2=0;_tmp6CB.f3=0;_tmp6CB.f4=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6CB.f5=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6CB;});_tmp6CA;}));
_tmp6C9;});_tmp6C8;}));_tmp6C7->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp6C7;});_tmp6C6;});
_tmp6C5;});break;case 260: _LL2FD: {struct _tuple17 _tmp6CD;struct Cyc_List_List*
_tmp6CE;int _tmp6CF;struct Cyc_Absyn_VarargInfo*_tmp6D0;struct Cyc_Core_Opt*_tmp6D1;
struct Cyc_List_List*_tmp6D2;struct _tuple17*_tmp6CC=Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6CD=*_tmp6CC;_tmp6CE=
_tmp6CD.f1;_tmp6CF=_tmp6CD.f2;_tmp6D0=_tmp6CD.f3;_tmp6D1=_tmp6CD.f4;_tmp6D2=
_tmp6CD.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6D3;(
_tmp6D3.YY29).tag=33;(_tmp6D3.YY29).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4->tms=({struct Cyc_List_List*_tmp6D5=
_cycalloc(sizeof(*_tmp6D5));_tmp6D5->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6D6=_cycalloc(sizeof(*_tmp6D6));_tmp6D6[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6D7;_tmp6D7.tag=3;_tmp6D7.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6D8=_cycalloc(sizeof(*_tmp6D8));_tmp6D8[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6D9;_tmp6D9.tag=1;_tmp6D9.f1=_tmp6CE;_tmp6D9.f2=_tmp6CF;_tmp6D9.f3=_tmp6D0;
_tmp6D9.f4=_tmp6D1;_tmp6D9.f5=_tmp6D2;_tmp6D9;});_tmp6D8;}));_tmp6D7;});_tmp6D6;}));
_tmp6D5->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp6D5;});_tmp6D4;});_tmp6D3;});break;}case 261: _LL2FE: {
struct Cyc_List_List*_tmp6DA=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6DB;(_tmp6DB.YY29).tag=
33;(_tmp6DB.YY29).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6DC=_cycalloc(
sizeof(*_tmp6DC));_tmp6DC->tms=({struct Cyc_List_List*_tmp6DD=_cycalloc(sizeof(*
_tmp6DD));_tmp6DD->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp6DE=_cycalloc(sizeof(*_tmp6DE));_tmp6DE[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp6DF;_tmp6DF.tag=4;_tmp6DF.f1=_tmp6DA;_tmp6DF.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6DF.f3=0;
_tmp6DF;});_tmp6DE;}));_tmp6DD->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp6DD;});_tmp6DC;});
_tmp6DB;});break;}case 262: _LL2FF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6E0;(_tmp6E0.YY29).tag=33;(_tmp6E0.YY29).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6E1=_cycalloc(sizeof(*_tmp6E1));_tmp6E1->tms=({struct Cyc_List_List*_tmp6E2=
_cycalloc(sizeof(*_tmp6E2));_tmp6E2->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp6E3=_cycalloc(sizeof(*_tmp6E3));_tmp6E3[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp6E4;_tmp6E4.tag=5;_tmp6E4.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6E4.f2=Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E4;});_tmp6E3;}));
_tmp6E2->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp6E2;});_tmp6E1;});_tmp6E0;});break;case 263: _LL300:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 264:
_LL301: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
265: _LL302: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 266: _LL303: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 267: _LL304: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 268: _LL305: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 269: _LL306: if(Cyc_zstrcmp((struct _dynforward_ptr)
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({
const char*_tmp6E5="`H";_tag_dynforward(_tmp6E5,sizeof(char),_get_zero_arr_size(
_tmp6E5,3));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp6E6="`U";
_tag_dynforward(_tmp6E6,sizeof(char),_get_zero_arr_size(_tmp6E6,3));}))== 0)Cyc_Parse_err((
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp6E9;_tmp6E9.tag=0;_tmp6E9.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{void*_tmp6E7[1]={&
_tmp6E9};Cyc_aprintf(({const char*_tmp6E8="bad occurrence of heap region %s";
_tag_dynforward(_tmp6E8,sizeof(char),_get_zero_arr_size(_tmp6E8,33));}),
_tag_dynforward(_tmp6E7,sizeof(void*),1));}}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp6F3=_cycalloc(sizeof(*_tmp6F3));_tmp6F3->name=({
struct _dynforward_ptr*_tmp6F6=_cycalloc(sizeof(*_tmp6F6));_tmp6F6[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6F6;});_tmp6F3->identity=
0;_tmp6F3->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6F4=_cycalloc(
sizeof(*_tmp6F4));_tmp6F4[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6F5;_tmp6F5.tag=0;
_tmp6F5.f1=(void*)((void*)3);_tmp6F5;});_tmp6F4;}));_tmp6F3;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp6F1=_cycalloc(sizeof(*_tmp6F1));_tmp6F1[0]=({
struct Cyc_Absyn_VarType_struct _tmp6F2;_tmp6F2.tag=1;_tmp6F2.f1=tv;_tmp6F2;});
_tmp6F1;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6EA;(
_tmp6EA.YY8).tag=12;(_tmp6EA.YY8).f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*
_tmp6EB=_cycalloc(sizeof(*_tmp6EB));_tmp6EB[0]=({struct Cyc_Absyn_Region_s_struct
_tmp6EC;_tmp6EC.tag=15;_tmp6EC.f1=tv;_tmp6EC.f2=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp6ED=_cycalloc(sizeof(*_tmp6ED));_tmp6ED->f1=(void*)0;_tmp6ED->f2=({
struct _dynforward_ptr*_tmp6EE=_cycalloc(sizeof(*_tmp6EE));_tmp6EE[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6EE;});_tmp6ED;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp6EF=_cycalloc(sizeof(*_tmp6EF));
_tmp6EF[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp6F0;_tmp6F0.tag=15;_tmp6F0.f1=(
void*)t;_tmp6F0;});_tmp6EF;}),0);_tmp6EC.f3=0;_tmp6EC.f4=Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6EC;});_tmp6EB;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp6EA;});
break;}case 270: _LL307: if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp6F7="H";
_tag_dynforward(_tmp6F7,sizeof(char),_get_zero_arr_size(_tmp6F7,2));}))== 0)Cyc_Parse_err(({
const char*_tmp6F8="bad occurrence of heap region `H";_tag_dynforward(_tmp6F8,
sizeof(char),_get_zero_arr_size(_tmp6F8,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp702=_cycalloc(sizeof(*_tmp702));_tmp702->name=({
struct _dynforward_ptr*_tmp705=_cycalloc(sizeof(*_tmp705));_tmp705[0]=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp708;_tmp708.tag=0;_tmp708.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp706[1]={&
_tmp708};Cyc_aprintf(({const char*_tmp707="`%s";_tag_dynforward(_tmp707,sizeof(
char),_get_zero_arr_size(_tmp707,4));}),_tag_dynforward(_tmp706,sizeof(void*),1));}});
_tmp705;});_tmp702->identity=0;_tmp702->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp703=_cycalloc(sizeof(*_tmp703));_tmp703[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp704;_tmp704.tag=0;_tmp704.f1=(void*)((void*)3);_tmp704;});_tmp703;}));
_tmp702;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp700=_cycalloc(
sizeof(*_tmp700));_tmp700[0]=({struct Cyc_Absyn_VarType_struct _tmp701;_tmp701.tag=
1;_tmp701.f1=tv;_tmp701;});_tmp700;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6F9;(_tmp6F9.YY8).tag=12;(_tmp6F9.YY8).f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp6FA=_cycalloc(sizeof(*_tmp6FA));_tmp6FA[0]=({
struct Cyc_Absyn_Region_s_struct _tmp6FB;_tmp6FB.tag=15;_tmp6FB.f1=tv;_tmp6FB.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp6FC=_cycalloc(sizeof(*_tmp6FC));
_tmp6FC->f1=(void*)0;_tmp6FC->f2=({struct _dynforward_ptr*_tmp6FD=_cycalloc(
sizeof(*_tmp6FD));_tmp6FD[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6FD;});_tmp6FC;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp6FE=_cycalloc(sizeof(*_tmp6FE));
_tmp6FE[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp6FF;_tmp6FF.tag=15;_tmp6FF.f1=(
void*)t;_tmp6FF;});_tmp6FE;}),0);_tmp6FB.f3=0;_tmp6FB.f4=Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6FB;});_tmp6FA;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp6F9;});
break;}case 271: _LL308: if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),({const char*_tmp709="resetable";
_tag_dynforward(_tmp709,sizeof(char),_get_zero_arr_size(_tmp709,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp70A="expecting [resetable]";_tag_dynforward(_tmp70A,sizeof(char),
_get_zero_arr_size(_tmp70A,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line));if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),({const char*_tmp70B="`H";_tag_dynforward(_tmp70B,
sizeof(char),_get_zero_arr_size(_tmp70B,3));}))== 0  || Cyc_zstrcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),({const char*_tmp70C="`U";_tag_dynforward(_tmp70C,sizeof(char),
_get_zero_arr_size(_tmp70C,3));})))Cyc_Parse_err((struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp70F;_tmp70F.tag=0;_tmp70F.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));{void*_tmp70D[1]={& _tmp70F};Cyc_aprintf(({const char*
_tmp70E="bad occurrence of heap region %s";_tag_dynforward(_tmp70E,sizeof(char),
_get_zero_arr_size(_tmp70E,33));}),_tag_dynforward(_tmp70D,sizeof(void*),1));}}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp719=_cycalloc(sizeof(*_tmp719));_tmp719->name=({struct _dynforward_ptr*
_tmp71C=_cycalloc(sizeof(*_tmp71C));_tmp71C[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp71C;});_tmp719->identity=
0;_tmp719->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp71A=_cycalloc(
sizeof(*_tmp71A));_tmp71A[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp71B;_tmp71B.tag=0;
_tmp71B.f1=(void*)((void*)3);_tmp71B;});_tmp71A;}));_tmp719;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp717=_cycalloc(sizeof(*_tmp717));_tmp717[0]=({
struct Cyc_Absyn_VarType_struct _tmp718;_tmp718.tag=1;_tmp718.f1=tv;_tmp718;});
_tmp717;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp710;(
_tmp710.YY8).tag=12;(_tmp710.YY8).f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*
_tmp711=_cycalloc(sizeof(*_tmp711));_tmp711[0]=({struct Cyc_Absyn_Region_s_struct
_tmp712;_tmp712.tag=15;_tmp712.f1=tv;_tmp712.f2=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp713=_cycalloc(sizeof(*_tmp713));_tmp713->f1=(void*)0;_tmp713->f2=({
struct _dynforward_ptr*_tmp714=_cycalloc(sizeof(*_tmp714));_tmp714[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp714;});_tmp713;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp715=_cycalloc(sizeof(*_tmp715));
_tmp715[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp716;_tmp716.tag=15;_tmp716.f1=(
void*)t;_tmp716;});_tmp715;}),0);_tmp712.f3=1;_tmp712.f4=Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp712;});_tmp711;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp710;});
break;}case 272: _LL309: if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp71D="resetable";
_tag_dynforward(_tmp71D,sizeof(char),_get_zero_arr_size(_tmp71D,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp71E="expecting `resetable'";_tag_dynforward(_tmp71E,sizeof(char),
_get_zero_arr_size(_tmp71E,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),({const char*_tmp71F="H";_tag_dynforward(_tmp71F,sizeof(
char),_get_zero_arr_size(_tmp71F,2));}))== 0)Cyc_Parse_err(({const char*_tmp720="bad occurrence of heap region `H";
_tag_dynforward(_tmp720,sizeof(char),_get_zero_arr_size(_tmp720,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp72A=_cycalloc(sizeof(*_tmp72A));_tmp72A->name=({
struct _dynforward_ptr*_tmp72D=_cycalloc(sizeof(*_tmp72D));_tmp72D[0]=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp730;_tmp730.tag=0;_tmp730.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp72E[1]={&
_tmp730};Cyc_aprintf(({const char*_tmp72F="`%s";_tag_dynforward(_tmp72F,sizeof(
char),_get_zero_arr_size(_tmp72F,4));}),_tag_dynforward(_tmp72E,sizeof(void*),1));}});
_tmp72D;});_tmp72A->identity=0;_tmp72A->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp72B=_cycalloc(sizeof(*_tmp72B));_tmp72B[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp72C;_tmp72C.tag=0;_tmp72C.f1=(void*)((void*)3);_tmp72C;});_tmp72B;}));
_tmp72A;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp728=_cycalloc(
sizeof(*_tmp728));_tmp728[0]=({struct Cyc_Absyn_VarType_struct _tmp729;_tmp729.tag=
1;_tmp729.f1=tv;_tmp729;});_tmp728;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp721;(_tmp721.YY8).tag=12;(_tmp721.YY8).f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp722=_cycalloc(sizeof(*_tmp722));_tmp722[0]=({
struct Cyc_Absyn_Region_s_struct _tmp723;_tmp723.tag=15;_tmp723.f1=tv;_tmp723.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp724=_cycalloc(sizeof(*_tmp724));
_tmp724->f1=(void*)0;_tmp724->f2=({struct _dynforward_ptr*_tmp725=_cycalloc(
sizeof(*_tmp725));_tmp725[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp725;});_tmp724;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp726=_cycalloc(sizeof(*_tmp726));
_tmp726[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp727;_tmp727.tag=15;_tmp727.f1=(
void*)t;_tmp727;});_tmp726;}),0);_tmp723.f3=1;_tmp723.f4=Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp723;});_tmp722;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp721;});
break;}case 273: _LL30A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp731;(_tmp731.YY8).tag=12;(_tmp731.YY8).f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_ResetRegion_s_struct*_tmp732=_cycalloc(sizeof(*_tmp732));_tmp732[0]=({
struct Cyc_Absyn_ResetRegion_s_struct _tmp733;_tmp733.tag=16;_tmp733.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp733;});_tmp732;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp731;});break;case 274: _LL30B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp734;(_tmp734.YY8).tag=12;(_tmp734.YY8).f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmp735=_cycalloc(sizeof(*_tmp735));
_tmp735[0]=({struct Cyc_Absyn_Label_s_struct _tmp736;_tmp736.tag=12;_tmp736.f1=({
struct _dynforward_ptr*_tmp737=_cycalloc(sizeof(*_tmp737));_tmp737[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp737;});_tmp736.f2=
Cyc_yyget_YY8(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp736;});
_tmp735;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp734;});break;case 275: _LL30C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp738;(_tmp738.YY8).tag=12;(_tmp738.YY8).f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp738;});break;case 276: _LL30D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp739;(_tmp739.YY8).tag=12;(_tmp739.YY8).f1=Cyc_Absyn_exp_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp739;});
break;case 277: _LL30E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp73A;(_tmp73A.YY8).tag=12;(_tmp73A.YY8).f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp73A;});
break;case 278: _LL30F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 279: _LL310: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp73B;(_tmp73B.YY8).tag=12;(_tmp73B.YY8).f1=Cyc_Parse_flatten_declarations(Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0));
_tmp73B;});break;case 280: _LL311: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp73C;(_tmp73C.YY8).tag=12;(_tmp73C.YY8).f1=Cyc_Parse_flatten_declarations(Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp73C;});break;case 281:
_LL312: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
282: _LL313: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp73D;(
_tmp73D.YY8).tag=12;(_tmp73D.YY8).f1=Cyc_Absyn_seq_stmt(Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp73D;});
break;case 283: _LL314: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp73E;(_tmp73E.YY8).tag=12;(_tmp73E.YY8).f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp73F=_cycalloc(sizeof(*_tmp73F));_tmp73F[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp740;_tmp740.tag=1;_tmp740.f1=Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp740;});_tmp73F;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0));_tmp73E;});break;case 284:
_LL315: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp741;(_tmp741.YY8).tag=
12;(_tmp741.YY8).f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp742=_cycalloc(sizeof(*_tmp742));_tmp742[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp743;_tmp743.tag=1;_tmp743.f1=Cyc_yyget_YY16(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp743;});_tmp742;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_YY8(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]));_tmp741;});break;case 285: _LL316: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp744;(_tmp744.YY8).tag=12;(_tmp744.YY8).f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp744;});break;case 286: _LL317: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp745;(_tmp745.YY8).tag=12;(_tmp745.YY8).f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY8(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp745;});
break;case 287: _LL318: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp746;(_tmp746.YY8).tag=12;(_tmp746.YY8).f1=Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp746;});
break;case 288: _LL319: {struct Cyc_Absyn_Exp*_tmp747=Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnknownId_e_struct*_tmp749=_cycalloc(sizeof(*_tmp749));_tmp749[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp74A;_tmp74A.tag=2;_tmp74A.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp74A;});_tmp749;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp748;(_tmp748.YY8).tag=12;(_tmp748.YY8).f1=Cyc_Absyn_switch_stmt(_tmp747,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp748;});
break;}case 289: _LL31A: {struct Cyc_Absyn_Exp*_tmp74B=Cyc_Absyn_tuple_exp(Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp74C;(_tmp74C.YY8).tag=12;(_tmp74C.YY8).f1=
Cyc_Absyn_switch_stmt(_tmp74B,Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp74C;});
break;}case 290: _LL31B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp74D;(_tmp74D.YY8).tag=12;(_tmp74D.YY8).f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY8(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp74D;});
break;case 291: _LL31C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp74E;(_tmp74E.YY9).tag=13;(_tmp74E.YY9).f1=0;_tmp74E;});break;case 292: _LL31D:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp74F;(_tmp74F.YY9).tag=
13;(_tmp74F.YY9).f1=({struct Cyc_List_List*_tmp750=_cycalloc(sizeof(*_tmp750));
_tmp750->hd=({struct Cyc_Absyn_Switch_clause*_tmp751=_cycalloc(sizeof(*_tmp751));
_tmp751->pattern=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp751->pat_vars=
0;_tmp751->where_clause=0;_tmp751->body=Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp751->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp751;});
_tmp750->tl=0;_tmp750;});_tmp74F;});break;case 293: _LL31E: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp752;(_tmp752.YY9).tag=13;(_tmp752.YY9).f1=({struct Cyc_List_List*
_tmp753=_cycalloc(sizeof(*_tmp753));_tmp753->hd=({struct Cyc_Absyn_Switch_clause*
_tmp754=_cycalloc(sizeof(*_tmp754));_tmp754->pattern=Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp754->pat_vars=0;
_tmp754->where_clause=0;_tmp754->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp754->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp754;});_tmp753->tl=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp753;});_tmp752;});
break;case 294: _LL31F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp755;(_tmp755.YY9).tag=13;(_tmp755.YY9).f1=({struct Cyc_List_List*_tmp756=
_cycalloc(sizeof(*_tmp756));_tmp756->hd=({struct Cyc_Absyn_Switch_clause*_tmp757=
_cycalloc(sizeof(*_tmp757));_tmp757->pattern=Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp757->pat_vars=0;
_tmp757->where_clause=0;_tmp757->body=Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp757->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp757;});
_tmp756->tl=Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp756;});_tmp755;});break;case 295: _LL320: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp758;(_tmp758.YY9).tag=13;(_tmp758.YY9).f1=({struct Cyc_List_List*
_tmp759=_cycalloc(sizeof(*_tmp759));_tmp759->hd=({struct Cyc_Absyn_Switch_clause*
_tmp75A=_cycalloc(sizeof(*_tmp75A));_tmp75A->pattern=Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp75A->pat_vars=0;
_tmp75A->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp75A->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp75A->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp75A;});
_tmp759->tl=Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp759;});_tmp758;});break;case 296: _LL321: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp75B;(_tmp75B.YY9).tag=13;(_tmp75B.YY9).f1=({struct Cyc_List_List*
_tmp75C=_cycalloc(sizeof(*_tmp75C));_tmp75C->hd=({struct Cyc_Absyn_Switch_clause*
_tmp75D=_cycalloc(sizeof(*_tmp75D));_tmp75D->pattern=Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp75D->pat_vars=0;
_tmp75D->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp75D->body=Cyc_yyget_YY8(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp75D->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp75D;});
_tmp75C->tl=Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp75C;});_tmp75B;});break;case 297: _LL322: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp75E;(_tmp75E.YY8).tag=12;(_tmp75E.YY8).f1=Cyc_Absyn_while_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp75E;});
break;case 298: _LL323: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp75F;(_tmp75F.YY8).tag=12;(_tmp75F.YY8).f1=Cyc_Absyn_do_stmt(Cyc_yyget_YY8(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp75F;});
break;case 299: _LL324: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp760;(_tmp760.YY8).tag=12;(_tmp760.YY8).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp760;});
break;case 300: _LL325: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp761;(_tmp761.YY8).tag=12;(_tmp761.YY8).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp761;});break;case 301: _LL326: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp762;(_tmp762.YY8).tag=12;(_tmp762.YY8).f1=Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp762;});
break;case 302: _LL327: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp763;(_tmp763.YY8).tag=12;(_tmp763.YY8).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp763;});
break;case 303: _LL328: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp764;(_tmp764.YY8).tag=12;(_tmp764.YY8).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(
0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY8(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp764;});
break;case 304: _LL329: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp765;(_tmp765.YY8).tag=12;(_tmp765.YY8).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY8(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp765;});
break;case 305: _LL32A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp766;(_tmp766.YY8).tag=12;(_tmp766.YY8).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY8(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp766;});
break;case 306: _LL32B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp767;(_tmp767.YY8).tag=12;(_tmp767.YY8).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp767;});
break;case 307: _LL32C: {struct Cyc_List_List*_tmp768=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp769=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY8(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp76A;(_tmp76A.YY8).tag=12;(_tmp76A.YY8).f1=Cyc_Parse_flatten_declarations(
_tmp768,_tmp769);_tmp76A;});break;}case 308: _LL32D: {struct Cyc_List_List*_tmp76B=
Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
struct Cyc_Absyn_Stmt*_tmp76C=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY8(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp76D;(_tmp76D.YY8).tag=12;(_tmp76D.YY8).f1=Cyc_Parse_flatten_declarations(
_tmp76B,_tmp76C);_tmp76D;});break;}case 309: _LL32E: {struct Cyc_List_List*_tmp76E=
Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
struct Cyc_Absyn_Stmt*_tmp76F=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY8(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp770;(_tmp770.YY8).tag=12;(_tmp770.YY8).f1=Cyc_Parse_flatten_declarations(
_tmp76E,_tmp76F);_tmp770;});break;}case 310: _LL32F: {struct Cyc_List_List*_tmp771=
Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
struct Cyc_Absyn_Stmt*_tmp772=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp773;(_tmp773.YY8).tag=12;(_tmp773.YY8).f1=Cyc_Parse_flatten_declarations(
_tmp771,_tmp772);_tmp773;});break;}case 311: _LL330: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp774;(_tmp774.YY8).tag=12;(_tmp774.YY8).f1=Cyc_Absyn_goto_stmt(({
struct _dynforward_ptr*_tmp775=_cycalloc(sizeof(*_tmp775));_tmp775[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp775;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp774;});
break;case 312: _LL331: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp776;(_tmp776.YY8).tag=12;(_tmp776.YY8).f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp776;});
break;case 313: _LL332: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp777;(_tmp777.YY8).tag=12;(_tmp777.YY8).f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp777;});
break;case 314: _LL333: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp778;(_tmp778.YY8).tag=12;(_tmp778.YY8).f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp778;});
break;case 315: _LL334: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp779;(_tmp779.YY8).tag=12;(_tmp779.YY8).f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp779;});
break;case 316: _LL335: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77A;(_tmp77A.YY8).tag=12;(_tmp77A.YY8).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp77A;});
break;case 317: _LL336: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77B;(_tmp77B.YY8).tag=12;(_tmp77B.YY8).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));_tmp77B;});
break;case 318: _LL337: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77C;(_tmp77C.YY8).tag=12;(_tmp77C.YY8).f1=Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));_tmp77C;});
break;case 319: _LL338: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 320: _LL339: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 321: _LL33A: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp77D;(_tmp77D.YY10).tag=14;(_tmp77D.YY10).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp77D;});
break;case 322: _LL33B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 323: _LL33C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77E;(_tmp77E.YY10).tag=14;(_tmp77E.YY10).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp77E;});break;case 324: _LL33D: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 325: _LL33E: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp77F;(_tmp77F.YY10).tag=14;(
_tmp77F.YY10).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp77F;});
break;case 326: _LL33F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 327: _LL340: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp780;(_tmp780.YY10).tag=14;(_tmp780.YY10).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)14,Cyc_Parse_pat2exp(Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp780;});
break;case 328: _LL341: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 329: _LL342: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp781;(_tmp781.YY10).tag=14;(_tmp781.YY10).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)15,Cyc_Parse_pat2exp(Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp781;});
break;case 330: _LL343: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 331: _LL344: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp782;(_tmp782.YY10).tag=14;(_tmp782.YY10).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)13,Cyc_Parse_pat2exp(Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp782;});
break;case 332: _LL345: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 333: _LL346: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp783;(_tmp783.YY10).tag=14;(_tmp783.YY10).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp783;});break;case 334: _LL347: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp784;(_tmp784.YY10).tag=14;(_tmp784.YY10).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp784;});
break;case 335: _LL348: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 336: _LL349: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp785;(_tmp785.YY10).tag=14;(_tmp785.YY10).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp785;});break;case 337: _LL34A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp786;(_tmp786.YY10).tag=14;(_tmp786.YY10).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp786;});
break;case 338: _LL34B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp787;(_tmp787.YY10).tag=14;(_tmp787.YY10).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp787;});break;case 339: _LL34C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp788;(_tmp788.YY10).tag=14;(_tmp788.YY10).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp788;});
break;case 340: _LL34D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 341: _LL34E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp789;(_tmp789.YY10).tag=14;(_tmp789.YY10).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)16,Cyc_Parse_pat2exp(Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp789;});
break;case 342: _LL34F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78A;(_tmp78A.YY10).tag=14;(_tmp78A.YY10).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)17,Cyc_Parse_pat2exp(Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp78A;});
break;case 343: _LL350: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 344: _LL351: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78B;(_tmp78B.YY10).tag=14;(_tmp78B.YY10).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)0,Cyc_Parse_pat2exp(Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp78B;});
break;case 345: _LL352: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78C;(_tmp78C.YY10).tag=14;(_tmp78C.YY10).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)2,Cyc_Parse_pat2exp(Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp78C;});
break;case 346: _LL353: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 347: _LL354: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78D;(_tmp78D.YY10).tag=14;(_tmp78D.YY10).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)1,Cyc_Parse_pat2exp(Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp78D;});
break;case 348: _LL355: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78E;(_tmp78E.YY10).tag=14;(_tmp78E.YY10).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)3,Cyc_Parse_pat2exp(Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp78E;});
break;case 349: _LL356: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78F;(_tmp78F.YY10).tag=14;(_tmp78F.YY10).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)4,Cyc_Parse_pat2exp(Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp78F;});
break;case 350: _LL357: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 351: _LL358: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp790;(_tmp790.YY10).tag=14;(_tmp790.YY10).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp790;});break;case 352: _LL359: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 353: _LL35A: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp791;(_tmp791.YY10).tag=14;(
_tmp791.YY10).f1=Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp791;});break;case 354:
_LL35B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp792;(_tmp792.YY10).tag=
14;(_tmp792.YY10).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp792;});
break;case 355: _LL35C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp793;(_tmp793.YY10).tag=14;(_tmp793.YY10).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp793;});break;case 356: _LL35D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp794;(_tmp794.YY10).tag=14;(_tmp794.YY10).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp794;});break;case 357: _LL35E: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp795;(_tmp795.YY10).tag=14;(_tmp795.YY10).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*_tmp796=
_cycalloc(sizeof(*_tmp796));_tmp796[0]=({struct Cyc_Absyn_StructField_struct
_tmp797;_tmp797.tag=0;_tmp797.f1=({struct _dynforward_ptr*_tmp798=_cycalloc(
sizeof(*_tmp798));_tmp798[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp798;});_tmp797;});
_tmp796;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp795;});break;case 358: _LL35F: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp799;(_tmp799.YY10).tag=14;(_tmp799.YY10).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp79A=
_cycalloc_atomic(sizeof(*_tmp79A));_tmp79A[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp79B;_tmp79B.tag=1;_tmp79B.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp79B;});_tmp79A;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp799;});break;case 359: _LL360: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 360: _LL361: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 361: _LL362:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp79C;(_tmp79C.YY10).tag=
14;(_tmp79C.YY10).f1=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp79C;});
break;case 362: _LL363: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp79D;(_tmp79D.YY10).tag=14;(_tmp79D.YY10).f1=Cyc_Absyn_exp_pat(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp79D;});break;
case 363: _LL364: {struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);{void*_tmp79E=(void*)e->r;
void*_tmp79F;void*_tmp7A0;char _tmp7A1;void*_tmp7A2;void*_tmp7A3;short _tmp7A4;
void*_tmp7A5;void*_tmp7A6;int _tmp7A7;void*_tmp7A8;struct _dynforward_ptr _tmp7A9;
void*_tmp7AA;void*_tmp7AB;void*_tmp7AC;_LL367: if(*((int*)_tmp79E)!= 0)goto _LL369;
_tmp79F=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp79E)->f1;if(_tmp79F <= (void*)
1)goto _LL369;if(*((int*)_tmp79F)!= 0)goto _LL369;_tmp7A0=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp79F)->f1;_tmp7A1=((struct Cyc_Absyn_Char_c_struct*)_tmp79F)->f2;_LL368: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7AD;(_tmp7AD.YY10).tag=14;(
_tmp7AD.YY10).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Char_p_struct*
_tmp7AE=_cycalloc_atomic(sizeof(*_tmp7AE));_tmp7AE[0]=({struct Cyc_Absyn_Char_p_struct
_tmp7AF;_tmp7AF.tag=8;_tmp7AF.f1=_tmp7A1;_tmp7AF;});_tmp7AE;}),e->loc);_tmp7AD;});
goto _LL366;_LL369: if(*((int*)_tmp79E)!= 0)goto _LL36B;_tmp7A2=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp79E)->f1;if(_tmp7A2 <= (void*)1)goto _LL36B;if(*((int*)_tmp7A2)!= 1)goto _LL36B;
_tmp7A3=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmp7A2)->f1;_tmp7A4=((struct
Cyc_Absyn_Short_c_struct*)_tmp7A2)->f2;_LL36A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7B0;(_tmp7B0.YY10).tag=14;(_tmp7B0.YY10).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Int_p_struct*_tmp7B1=_cycalloc(sizeof(*_tmp7B1));_tmp7B1[
0]=({struct Cyc_Absyn_Int_p_struct _tmp7B2;_tmp7B2.tag=7;_tmp7B2.f1=(void*)_tmp7A3;
_tmp7B2.f2=(int)_tmp7A4;_tmp7B2;});_tmp7B1;}),e->loc);_tmp7B0;});goto _LL366;
_LL36B: if(*((int*)_tmp79E)!= 0)goto _LL36D;_tmp7A5=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp79E)->f1;if(_tmp7A5 <= (void*)1)goto _LL36D;if(*((int*)_tmp7A5)!= 2)goto _LL36D;
_tmp7A6=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp7A5)->f1;_tmp7A7=((struct Cyc_Absyn_Int_c_struct*)
_tmp7A5)->f2;_LL36C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B3;(_tmp7B3.YY10).tag=14;(_tmp7B3.YY10).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Int_p_struct*_tmp7B4=_cycalloc(sizeof(*_tmp7B4));_tmp7B4[0]=({
struct Cyc_Absyn_Int_p_struct _tmp7B5;_tmp7B5.tag=7;_tmp7B5.f1=(void*)_tmp7A6;
_tmp7B5.f2=_tmp7A7;_tmp7B5;});_tmp7B4;}),e->loc);_tmp7B3;});goto _LL366;_LL36D:
if(*((int*)_tmp79E)!= 0)goto _LL36F;_tmp7A8=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp79E)->f1;if(_tmp7A8 <= (void*)1)goto _LL36F;if(*((int*)_tmp7A8)!= 4)goto _LL36F;
_tmp7A9=((struct Cyc_Absyn_Float_c_struct*)_tmp7A8)->f1;_LL36E: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7B6;(_tmp7B6.YY10).tag=14;(_tmp7B6.YY10).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Float_p_struct*_tmp7B7=_cycalloc(sizeof(*_tmp7B7));
_tmp7B7[0]=({struct Cyc_Absyn_Float_p_struct _tmp7B8;_tmp7B8.tag=9;_tmp7B8.f1=
_tmp7A9;_tmp7B8;});_tmp7B7;}),e->loc);_tmp7B6;});goto _LL366;_LL36F: if(*((int*)
_tmp79E)!= 0)goto _LL371;_tmp7AA=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp79E)->f1;
if((int)_tmp7AA != 0)goto _LL371;_LL370: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B9;(_tmp7B9.YY10).tag=14;(_tmp7B9.YY10).f1=Cyc_Absyn_new_pat((void*)1,e->loc);
_tmp7B9;});goto _LL366;_LL371: if(*((int*)_tmp79E)!= 0)goto _LL373;_tmp7AB=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp79E)->f1;if(_tmp7AB <= (void*)1)goto _LL373;
if(*((int*)_tmp7AB)!= 5)goto _LL373;_LL372: Cyc_Parse_err(({const char*_tmp7BA="strings cannot occur within patterns";
_tag_dynforward(_tmp7BA,sizeof(char),_get_zero_arr_size(_tmp7BA,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL366;
_LL373: if(*((int*)_tmp79E)!= 0)goto _LL375;_tmp7AC=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp79E)->f1;if(_tmp7AC <= (void*)1)goto _LL375;if(*((int*)_tmp7AC)!= 3)goto _LL375;
_LL374: Cyc_Parse_unimp(({const char*_tmp7BB="long long's in patterns";
_tag_dynforward(_tmp7BB,sizeof(char),_get_zero_arr_size(_tmp7BB,24));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL366;
_LL375:;_LL376: Cyc_Parse_err(({const char*_tmp7BC="bad constant in case";
_tag_dynforward(_tmp7BC,sizeof(char),_get_zero_arr_size(_tmp7BC,21));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_LL366:;}break;}
case 364: _LL365: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7BD;(
_tmp7BD.YY10).tag=14;(_tmp7BD.YY10).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownId_p_struct*
_tmp7BE=_cycalloc(sizeof(*_tmp7BE));_tmp7BE[0]=({struct Cyc_Absyn_UnknownId_p_struct
_tmp7BF;_tmp7BF.tag=12;_tmp7BF.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7BF;});_tmp7BE;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7BD;});
break;case 365: _LL377: if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp7C0="as";
_tag_dynforward(_tmp7C0,sizeof(char),_get_zero_arr_size(_tmp7C0,3));}))!= 0)Cyc_Parse_err(({
const char*_tmp7C1="expecting `as'";_tag_dynforward(_tmp7C1,sizeof(char),
_get_zero_arr_size(_tmp7C1,15));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7C2;(_tmp7C2.YY10).tag=14;(_tmp7C2.YY10).f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_struct*_tmp7C3=_cycalloc(
sizeof(*_tmp7C3));_tmp7C3[0]=({struct Cyc_Absyn_Var_p_struct _tmp7C4;_tmp7C4.tag=0;
_tmp7C4.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7C5=_cycalloc(sizeof(*
_tmp7C5));_tmp7C5->f1=(void*)0;_tmp7C5->f2=({struct _dynforward_ptr*_tmp7C6=
_cycalloc(sizeof(*_tmp7C6));_tmp7C6[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7C6;});_tmp7C5;}),(
void*)0,0);_tmp7C4.f2=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7C4;});_tmp7C3;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp7C2;});
break;case 366: _LL378: {struct Cyc_List_List*_tmp7C8;int _tmp7C9;struct _tuple13
_tmp7C7=*Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7C8=_tmp7C7.f1;_tmp7C9=_tmp7C7.f2;yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp7CA;(_tmp7CA.YY10).tag=14;(_tmp7CA.YY10).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Tuple_p_struct*_tmp7CB=_cycalloc(sizeof(*_tmp7CB));
_tmp7CB[0]=({struct Cyc_Absyn_Tuple_p_struct _tmp7CC;_tmp7CC.tag=3;_tmp7CC.f1=
_tmp7C8;_tmp7CC.f2=_tmp7C9;_tmp7CC;});_tmp7CB;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7CA;});
break;}case 367: _LL379: {struct Cyc_List_List*_tmp7CE;int _tmp7CF;struct _tuple13
_tmp7CD=*Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7CE=_tmp7CD.f1;_tmp7CF=_tmp7CD.f2;yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp7D0;(_tmp7D0.YY10).tag=14;(_tmp7D0.YY10).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_UnknownCall_p_struct*_tmp7D1=_cycalloc(sizeof(*_tmp7D1));
_tmp7D1[0]=({struct Cyc_Absyn_UnknownCall_p_struct _tmp7D2;_tmp7D2.tag=13;_tmp7D2.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp7D2.f2=_tmp7CE;_tmp7D2.f3=_tmp7CF;_tmp7D2;});_tmp7D1;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7D0;});
break;}case 368: _LL37A: {struct Cyc_List_List*_tmp7D4;int _tmp7D5;struct _tuple13
_tmp7D3=*Cyc_yyget_YY15(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7D4=_tmp7D3.f1;_tmp7D5=_tmp7D3.f2;{struct Cyc_List_List*_tmp7D6=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7D7;(_tmp7D7.YY10).tag=14;(_tmp7D7.YY10).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Aggr_p_struct*_tmp7D8=_cycalloc(sizeof(*_tmp7D8));_tmp7D8[0]=({
struct Cyc_Absyn_Aggr_p_struct _tmp7D9;_tmp7D9.tag=5;_tmp7D9.f1=({struct Cyc_Absyn_AggrInfo
_tmp7DA;_tmp7DA.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp7DB=_cycalloc(sizeof(*_tmp7DB));_tmp7DB[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp7DC;_tmp7DC.tag=0;_tmp7DC.f1=(void*)((void*)0);_tmp7DC.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp7DC;});_tmp7DB;}));
_tmp7DA.targs=0;_tmp7DA;});_tmp7D9.f2=_tmp7D6;_tmp7D9.f3=_tmp7D4;_tmp7D9.f4=
_tmp7D5;_tmp7D9;});_tmp7D8;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7D7;});
break;}}case 369: _LL37B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7DD;(_tmp7DD.YY10).tag=14;(_tmp7DD.YY10).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Pointer_p_struct*_tmp7DE=_cycalloc(sizeof(*_tmp7DE));_tmp7DE[0]=({
struct Cyc_Absyn_Pointer_p_struct _tmp7DF;_tmp7DF.tag=4;_tmp7DF.f1=Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7DF;});_tmp7DE;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7DD;});break;case 370: _LL37C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7E0;(_tmp7E0.YY10).tag=14;(_tmp7E0.YY10).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp7E1=_cycalloc(sizeof(*_tmp7E1));
_tmp7E1[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp7E2;_tmp7E2.tag=4;_tmp7E2.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp7E3=_cycalloc(
sizeof(*_tmp7E3));_tmp7E3[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp7E4;_tmp7E4.tag=
4;_tmp7E4.f1=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7E4;});_tmp7E3;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7E2;});
_tmp7E1;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7E0;});break;case 371: _LL37D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7E5;(_tmp7E5.YY10).tag=14;(_tmp7E5.YY10).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Reference_p_struct*_tmp7E6=_cycalloc(sizeof(*_tmp7E6));
_tmp7E6[0]=({struct Cyc_Absyn_Reference_p_struct _tmp7E7;_tmp7E7.tag=1;_tmp7E7.f1=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7E8=_cycalloc(sizeof(*_tmp7E8));
_tmp7E8->f1=(void*)0;_tmp7E8->f2=({struct _dynforward_ptr*_tmp7E9=_cycalloc(
sizeof(*_tmp7E9));_tmp7E9[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7E9;});_tmp7E8;}),(void*)
0,0);_tmp7E7.f2=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7E7;});
_tmp7E6;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7E5;});break;case 372: _LL37E: if(Cyc_strcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),({const char*_tmp7EA="as";_tag_dynforward(_tmp7EA,
sizeof(char),_get_zero_arr_size(_tmp7EA,3));}))!= 0)Cyc_Parse_err(({const char*
_tmp7EB="expecting `as'";_tag_dynforward(_tmp7EB,sizeof(char),_get_zero_arr_size(
_tmp7EB,15));}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7EC;(_tmp7EC.YY10).tag=14;(_tmp7EC.YY10).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Reference_p_struct*_tmp7ED=_cycalloc(sizeof(*_tmp7ED));_tmp7ED[0]=({
struct Cyc_Absyn_Reference_p_struct _tmp7EE;_tmp7EE.tag=1;_tmp7EE.f1=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp7EF=_cycalloc(sizeof(*_tmp7EF));_tmp7EF->f1=(void*)0;_tmp7EF->f2=({
struct _dynforward_ptr*_tmp7F0=_cycalloc(sizeof(*_tmp7F0));_tmp7F0[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7F0;});_tmp7EF;}),(
void*)0,0);_tmp7EE.f2=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7EE;});_tmp7ED;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp7EC;});
break;case 373: _LL37F: {void*_tmp7F1=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp7F9=_cycalloc(sizeof(*_tmp7F9));_tmp7F9[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp7FA;_tmp7FA.tag=0;_tmp7FA.f1=(void*)((void*)5);_tmp7FA;});_tmp7F9;}));yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7F2;(_tmp7F2.YY10).tag=14;(
_tmp7F2.YY10).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp7F3=_cycalloc(sizeof(*_tmp7F3));_tmp7F3[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp7F4;_tmp7F4.tag=2;_tmp7F4.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp7F1);
_tmp7F4.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7F5=_cycalloc(sizeof(*
_tmp7F5));_tmp7F5->f1=(void*)0;_tmp7F5->f2=({struct _dynforward_ptr*_tmp7F6=
_cycalloc(sizeof(*_tmp7F6));_tmp7F6[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7F6;});_tmp7F5;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp7F7=_cycalloc(sizeof(*_tmp7F7));
_tmp7F7[0]=({struct Cyc_Absyn_TagType_struct _tmp7F8;_tmp7F8.tag=17;_tmp7F8.f1=(
void*)_tmp7F1;_tmp7F8;});_tmp7F7;}),0);_tmp7F4;});_tmp7F3;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7F2;});
break;}case 374: _LL380: {struct Cyc_Absyn_Tvar*_tmp7FB=Cyc_Tcutil_new_tvar((void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp805=_cycalloc(sizeof(*_tmp805));_tmp805[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp806;_tmp806.tag=0;_tmp806.f1=(void*)((void*)5);
_tmp806;});_tmp805;}));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7FC;(_tmp7FC.YY10).tag=14;(_tmp7FC.YY10).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_TagInt_p_struct*_tmp7FD=_cycalloc(sizeof(*_tmp7FD));_tmp7FD[0]=({
struct Cyc_Absyn_TagInt_p_struct _tmp7FE;_tmp7FE.tag=2;_tmp7FE.f1=_tmp7FB;_tmp7FE.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7FF=_cycalloc(sizeof(*_tmp7FF));
_tmp7FF->f1=(void*)0;_tmp7FF->f2=({struct _dynforward_ptr*_tmp800=_cycalloc(
sizeof(*_tmp800));_tmp800[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp800;});_tmp7FF;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp801=_cycalloc(sizeof(*_tmp801));
_tmp801[0]=({struct Cyc_Absyn_TagType_struct _tmp802;_tmp802.tag=17;_tmp802.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp803=_cycalloc(sizeof(*_tmp803));
_tmp803[0]=({struct Cyc_Absyn_VarType_struct _tmp804;_tmp804.tag=1;_tmp804.f1=
_tmp7FB;_tmp804;});_tmp803;}));_tmp802;});_tmp801;}),0);_tmp7FE;});_tmp7FD;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7FC;});break;}case 375: _LL381: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp807;(_tmp807.YY11).tag=15;(_tmp807.YY11).f1=({struct
_tuple13*_tmp808=_cycalloc(sizeof(*_tmp808));_tmp808->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY12(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp808->f2=0;_tmp808;});
_tmp807;});break;case 376: _LL382: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp809;(_tmp809.YY11).tag=15;(_tmp809.YY11).f1=({struct _tuple13*_tmp80A=
_cycalloc(sizeof(*_tmp80A));_tmp80A->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_yyget_YY12(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp80A->f2=1;_tmp80A;});_tmp809;});break;case 377: _LL383:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp80B;(_tmp80B.YY11).tag=
15;(_tmp80B.YY11).f1=({struct _tuple13*_tmp80C=_cycalloc(sizeof(*_tmp80C));
_tmp80C->f1=0;_tmp80C->f2=1;_tmp80C;});_tmp80B;});break;case 378: _LL384: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp80D;(_tmp80D.YY12).tag=16;(
_tmp80D.YY12).f1=({struct Cyc_List_List*_tmp80E=_cycalloc(sizeof(*_tmp80E));
_tmp80E->hd=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp80E->tl=0;_tmp80E;});_tmp80D;});break;case 379: _LL385: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp80F;(_tmp80F.YY12).tag=16;(
_tmp80F.YY12).f1=({struct Cyc_List_List*_tmp810=_cycalloc(sizeof(*_tmp810));
_tmp810->hd=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp810->tl=Cyc_yyget_YY12(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp810;});_tmp80F;});break;case 380: _LL386: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp811;(_tmp811.YY13).tag=17;(_tmp811.YY13).f1=({
struct _tuple14*_tmp812=_cycalloc(sizeof(*_tmp812));_tmp812->f1=0;_tmp812->f2=Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp812;});_tmp811;});
break;case 381: _LL387: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp813;(_tmp813.YY13).tag=17;(_tmp813.YY13).f1=({struct _tuple14*_tmp814=
_cycalloc(sizeof(*_tmp814));_tmp814->f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp814->f2=Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp814;});_tmp813;});
break;case 382: _LL388: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp815;(_tmp815.YY15).tag=19;(_tmp815.YY15).f1=({struct _tuple13*_tmp816=
_cycalloc(sizeof(*_tmp816));_tmp816->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_yyget_YY14(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp816->f2=0;_tmp816;});_tmp815;});break;case 383: _LL389: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp817;(_tmp817.YY15).tag=19;(
_tmp817.YY15).f1=({struct _tuple13*_tmp818=_cycalloc(sizeof(*_tmp818));_tmp818->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp818->f2=1;_tmp818;});
_tmp817;});break;case 384: _LL38A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp819;(_tmp819.YY15).tag=19;(_tmp819.YY15).f1=({struct _tuple13*_tmp81A=
_cycalloc(sizeof(*_tmp81A));_tmp81A->f1=0;_tmp81A->f2=1;_tmp81A;});_tmp819;});
break;case 385: _LL38B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp81B;(_tmp81B.YY14).tag=18;(_tmp81B.YY14).f1=({struct Cyc_List_List*_tmp81C=
_cycalloc(sizeof(*_tmp81C));_tmp81C->hd=Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp81C->tl=0;_tmp81C;});
_tmp81B;});break;case 386: _LL38C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp81D;(_tmp81D.YY14).tag=18;(_tmp81D.YY14).f1=({struct Cyc_List_List*_tmp81E=
_cycalloc(sizeof(*_tmp81E));_tmp81E->hd=Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp81E->tl=Cyc_yyget_YY14(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp81E;});_tmp81D;});
break;case 387: _LL38D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 388: _LL38E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp81F;(_tmp81F.YY3).tag=7;(_tmp81F.YY3).f1=Cyc_Absyn_seq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp81F;});
break;case 389: _LL38F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 390: _LL390: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp820;(_tmp820.YY3).tag=7;(_tmp820.YY3).f1=Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp820;});
break;case 391: _LL391: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp821;(_tmp821.YY7).tag=11;(_tmp821.YY7).f1=0;_tmp821;});break;case 392: _LL392:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp822;(_tmp822.YY7).tag=
11;(_tmp822.YY7).f1=({struct Cyc_Core_Opt*_tmp823=_cycalloc(sizeof(*_tmp823));
_tmp823->v=(void*)((void*)1);_tmp823;});_tmp822;});break;case 393: _LL393: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp824;(_tmp824.YY7).tag=11;(
_tmp824.YY7).f1=({struct Cyc_Core_Opt*_tmp825=_cycalloc(sizeof(*_tmp825));_tmp825->v=(
void*)((void*)3);_tmp825;});_tmp824;});break;case 394: _LL394: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp826;(_tmp826.YY7).tag=11;(_tmp826.YY7).f1=({struct Cyc_Core_Opt*
_tmp827=_cycalloc(sizeof(*_tmp827));_tmp827->v=(void*)((void*)4);_tmp827;});
_tmp826;});break;case 395: _LL395: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp828;(_tmp828.YY7).tag=11;(_tmp828.YY7).f1=({struct Cyc_Core_Opt*_tmp829=
_cycalloc(sizeof(*_tmp829));_tmp829->v=(void*)((void*)0);_tmp829;});_tmp828;});
break;case 396: _LL396: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp82A;(_tmp82A.YY7).tag=11;(_tmp82A.YY7).f1=({struct Cyc_Core_Opt*_tmp82B=
_cycalloc(sizeof(*_tmp82B));_tmp82B->v=(void*)((void*)2);_tmp82B;});_tmp82A;});
break;case 397: _LL397: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp82C;(_tmp82C.YY7).tag=11;(_tmp82C.YY7).f1=({struct Cyc_Core_Opt*_tmp82D=
_cycalloc(sizeof(*_tmp82D));_tmp82D->v=(void*)((void*)16);_tmp82D;});_tmp82C;});
break;case 398: _LL398: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp82E;(_tmp82E.YY7).tag=11;(_tmp82E.YY7).f1=({struct Cyc_Core_Opt*_tmp82F=
_cycalloc(sizeof(*_tmp82F));_tmp82F->v=(void*)((void*)17);_tmp82F;});_tmp82E;});
break;case 399: _LL399: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp830;(_tmp830.YY7).tag=11;(_tmp830.YY7).f1=({struct Cyc_Core_Opt*_tmp831=
_cycalloc(sizeof(*_tmp831));_tmp831->v=(void*)((void*)13);_tmp831;});_tmp830;});
break;case 400: _LL39A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp832;(_tmp832.YY7).tag=11;(_tmp832.YY7).f1=({struct Cyc_Core_Opt*_tmp833=
_cycalloc(sizeof(*_tmp833));_tmp833->v=(void*)((void*)15);_tmp833;});_tmp832;});
break;case 401: _LL39B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp834;(_tmp834.YY7).tag=11;(_tmp834.YY7).f1=({struct Cyc_Core_Opt*_tmp835=
_cycalloc(sizeof(*_tmp835));_tmp835->v=(void*)((void*)14);_tmp835;});_tmp834;});
break;case 402: _LL39C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 403: _LL39D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp836;(_tmp836.YY3).tag=7;(_tmp836.YY3).f1=Cyc_Absyn_conditional_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp836;});
break;case 404: _LL39E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp837;(_tmp837.YY3).tag=7;(_tmp837.YY3).f1=Cyc_Absyn_throw_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp837;});
break;case 405: _LL39F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp838;(_tmp838.YY3).tag=7;(_tmp838.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp838;});
break;case 406: _LL3A0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp839;(_tmp839.YY3).tag=7;(_tmp839.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp839;});
break;case 407: _LL3A1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83A;(_tmp83A.YY3).tag=7;(_tmp83A.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83A;});
break;case 408: _LL3A2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83B;(_tmp83B.YY3).tag=7;(_tmp83B.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83B;});
break;case 409: _LL3A3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 410: _LL3A4: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 411: _LL3A5: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp83C;(_tmp83C.YY3).tag=7;(_tmp83C.YY3).f1=Cyc_Absyn_or_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83C;});
break;case 412: _LL3A6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 413: _LL3A7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83D;(_tmp83D.YY3).tag=7;(_tmp83D.YY3).f1=Cyc_Absyn_and_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83D;});
break;case 414: _LL3A8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 415: _LL3A9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83E;(_tmp83E.YY3).tag=7;(_tmp83E.YY3).f1=Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83E;});
break;case 416: _LL3AA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 417: _LL3AB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83F;(_tmp83F.YY3).tag=7;(_tmp83F.YY3).f1=Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83F;});
break;case 418: _LL3AC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 419: _LL3AD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp840;(_tmp840.YY3).tag=7;(_tmp840.YY3).f1=Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp840;});
break;case 420: _LL3AE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 421: _LL3AF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp841;(_tmp841.YY3).tag=7;(_tmp841.YY3).f1=Cyc_Absyn_eq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp841;});
break;case 422: _LL3B0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp842;(_tmp842.YY3).tag=7;(_tmp842.YY3).f1=Cyc_Absyn_neq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp842;});
break;case 423: _LL3B1: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 424: _LL3B2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp843;(_tmp843.YY3).tag=7;(_tmp843.YY3).f1=Cyc_Absyn_lt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp843;});
break;case 425: _LL3B3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp844;(_tmp844.YY3).tag=7;(_tmp844.YY3).f1=Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp844;});
break;case 426: _LL3B4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp845;(_tmp845.YY3).tag=7;(_tmp845.YY3).f1=Cyc_Absyn_lte_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp845;});
break;case 427: _LL3B5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp846;(_tmp846.YY3).tag=7;(_tmp846.YY3).f1=Cyc_Absyn_gte_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp846;});
break;case 428: _LL3B6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 429: _LL3B7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp847;(_tmp847.YY3).tag=7;(_tmp847.YY3).f1=Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp847;});
break;case 430: _LL3B8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp848;(_tmp848.YY3).tag=7;(_tmp848.YY3).f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp848;});
break;case 431: _LL3B9: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 432: _LL3BA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp849;(_tmp849.YY3).tag=7;(_tmp849.YY3).f1=Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp849;});
break;case 433: _LL3BB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84A;(_tmp84A.YY3).tag=7;(_tmp84A.YY3).f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84A;});
break;case 434: _LL3BC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 435: _LL3BD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84B;(_tmp84B.YY3).tag=7;(_tmp84B.YY3).f1=Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84B;});
break;case 436: _LL3BE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84C;(_tmp84C.YY3).tag=7;(_tmp84C.YY3).f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84C;});
break;case 437: _LL3BF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84D;(_tmp84D.YY3).tag=7;(_tmp84D.YY3).f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84D;});
break;case 438: _LL3C0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 439: _LL3C1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84E;(_tmp84E.YY3).tag=7;(_tmp84E.YY3).f1=Cyc_Absyn_cast_exp((*Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84E;});
break;case 440: _LL3C2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 441: _LL3C3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84F;(_tmp84F.YY3).tag=7;(_tmp84F.YY3).f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84F;});
break;case 442: _LL3C4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp850;(_tmp850.YY3).tag=7;(_tmp850.YY3).f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp850;});
break;case 443: _LL3C5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp851;(_tmp851.YY3).tag=7;(_tmp851.YY3).f1=Cyc_Absyn_address_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp851;});
break;case 444: _LL3C6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp852;(_tmp852.YY3).tag=7;(_tmp852.YY3).f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp852;});
break;case 445: _LL3C7: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 446: _LL3C8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp853;(_tmp853.YY3).tag=7;(_tmp853.YY3).f1=Cyc_Absyn_prim1_exp(Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp853;});
break;case 447: _LL3C9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp854;(_tmp854.YY3).tag=7;(_tmp854.YY3).f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp854;});
break;case 448: _LL3CA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp855;(_tmp855.YY3).tag=7;(_tmp855.YY3).f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp855;});
break;case 449: _LL3CB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp856;(_tmp856.YY3).tag=7;(_tmp856.YY3).f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct
Cyc_Absyn_StructField_struct*_tmp857=_cycalloc(sizeof(*_tmp857));_tmp857[0]=({
struct Cyc_Absyn_StructField_struct _tmp858;_tmp858.tag=0;_tmp858.f1=({struct
_dynforward_ptr*_tmp859=_cycalloc(sizeof(*_tmp859));_tmp859[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp859;});_tmp858;});
_tmp857;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp856;});break;case 450: _LL3CC: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp85A;(_tmp85A.YY3).tag=7;(_tmp85A.YY3).f1=Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp85B=_cycalloc_atomic(sizeof(*
_tmp85B));_tmp85B[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp85C;_tmp85C.tag=1;
_tmp85C.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2;_tmp85C;});_tmp85B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85A;});
break;case 451: _LL3CD: {struct Cyc_Position_Segment*_tmp85D=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp85E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp85D,Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp85F;(_tmp85F.YY3).tag=
7;(_tmp85F.YY3).f1=Cyc_Absyn_gentyp_exp(_tmp85E,(*Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85F;});
break;}case 452: _LL3CE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp860;(_tmp860.YY3).tag=7;(_tmp860.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Malloc_e_struct*_tmp861=_cycalloc(sizeof(*_tmp861));_tmp861[0]=({
struct Cyc_Absyn_Malloc_e_struct _tmp862;_tmp862.tag=35;_tmp862.f1=({struct Cyc_Absyn_MallocInfo
_tmp863;_tmp863.is_calloc=0;_tmp863.rgn=0;_tmp863.elt_type=0;_tmp863.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp863.fat_result=0;_tmp863;});_tmp862;});_tmp861;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp860;});
break;case 453: _LL3CF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp864;(_tmp864.YY3).tag=7;(_tmp864.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Malloc_e_struct*_tmp865=_cycalloc(sizeof(*_tmp865));_tmp865[0]=({
struct Cyc_Absyn_Malloc_e_struct _tmp866;_tmp866.tag=35;_tmp866.f1=({struct Cyc_Absyn_MallocInfo
_tmp867;_tmp867.is_calloc=0;_tmp867.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp867.elt_type=0;
_tmp867.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp867.fat_result=0;_tmp867;});_tmp866;});_tmp865;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp864;});
break;case 454: _LL3D0: {void*_tmp869;struct _tuple2 _tmp868=*Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp869=_tmp868.f3;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp86A;(_tmp86A.YY3).tag=7;(
_tmp86A.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp86B=_cycalloc(sizeof(*_tmp86B));_tmp86B[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp86C;_tmp86C.tag=35;_tmp86C.f1=({struct Cyc_Absyn_MallocInfo _tmp86D;_tmp86D.is_calloc=
1;_tmp86D.rgn=0;_tmp86D.elt_type=({void**_tmp86E=_cycalloc(sizeof(*_tmp86E));
_tmp86E[0]=_tmp869;_tmp86E;});_tmp86D.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp86D.fat_result=0;
_tmp86D;});_tmp86C;});_tmp86B;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86A;});
break;}case 455: _LL3D1: {void*_tmp870;struct _tuple2 _tmp86F=*Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp870=_tmp86F.f3;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp871;(_tmp871.YY3).tag=7;(
_tmp871.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp872=_cycalloc(sizeof(*_tmp872));_tmp872[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp873;_tmp873.tag=35;_tmp873.f1=({struct Cyc_Absyn_MallocInfo _tmp874;_tmp874.is_calloc=
1;_tmp874.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);_tmp874.elt_type=({void**
_tmp875=_cycalloc(sizeof(*_tmp875));_tmp875[0]=_tmp870;_tmp875;});_tmp874.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp874.fat_result=0;_tmp874;});_tmp873;});_tmp872;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp871;});
break;}case 456: _LL3D2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp876;(_tmp876.YY6).tag=10;(_tmp876.YY6).f1=(void*)((void*)12);_tmp876;});
break;case 457: _LL3D3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp877;(_tmp877.YY6).tag=10;(_tmp877.YY6).f1=(void*)((void*)11);_tmp877;});
break;case 458: _LL3D4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp878;(_tmp878.YY6).tag=10;(_tmp878.YY6).f1=(void*)((void*)2);_tmp878;});
break;case 459: _LL3D5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 460: _LL3D6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp879;(_tmp879.YY3).tag=7;(_tmp879.YY3).f1=Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp879;});
break;case 461: _LL3D7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87A;(_tmp87A.YY3).tag=7;(_tmp87A.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87A;});
break;case 462: _LL3D8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87B;(_tmp87B.YY3).tag=7;(_tmp87B.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87B;});
break;case 463: _LL3D9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87C;(_tmp87C.YY3).tag=7;(_tmp87C.YY3).f1=Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct
_dynforward_ptr*_tmp87D=_cycalloc(sizeof(*_tmp87D));_tmp87D[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp87D;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87C;});
break;case 464: _LL3DA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87E;(_tmp87E.YY3).tag=7;(_tmp87E.YY3).f1=Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct
_dynforward_ptr*_tmp87F=_cycalloc(sizeof(*_tmp87F));_tmp87F[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp87F;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87E;});
break;case 465: _LL3DB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp880;(_tmp880.YY3).tag=7;(_tmp880.YY3).f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp880;});
break;case 466: _LL3DC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp881;(_tmp881.YY3).tag=7;(_tmp881.YY3).f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp881;});
break;case 467: _LL3DD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp882;(_tmp882.YY3).tag=7;(_tmp882.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_CompoundLit_e_struct*_tmp883=_cycalloc(sizeof(*_tmp883));_tmp883[0]=({
struct Cyc_Absyn_CompoundLit_e_struct _tmp884;_tmp884.tag=27;_tmp884.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp884.f2=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp884;});_tmp883;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp882;});break;case 468: _LL3DE: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp885;(_tmp885.YY3).tag=7;(_tmp885.YY3).f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp886=_cycalloc(sizeof(*_tmp886));
_tmp886[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp887;_tmp887.tag=27;_tmp887.f1=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
_tmp887.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp887;});_tmp886;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp885;});
break;case 469: _LL3DF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp888;(_tmp888.YY3).tag=7;(_tmp888.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnknownId_e_struct*_tmp889=_cycalloc(sizeof(*_tmp889));_tmp889[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp88A;_tmp88A.tag=2;_tmp88A.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp88A;});_tmp889;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp888;});break;case 470: _LL3E0: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 471: _LL3E1: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp88B;(_tmp88B.YY3).tag=7;(
_tmp88B.YY3).f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88B;});
break;case 472: _LL3E2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 473: _LL3E3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88C;(_tmp88C.YY3).tag=7;(_tmp88C.YY3).f1=Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88C;});
break;case 474: _LL3E4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88D;(_tmp88D.YY3).tag=7;(_tmp88D.YY3).f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(
Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88D;});
break;case 475: _LL3E5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88E;(_tmp88E.YY3).tag=7;(_tmp88E.YY3).f1=Cyc_Absyn_tuple_exp(Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88E;});
break;case 476: _LL3E6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88F;(_tmp88F.YY3).tag=7;(_tmp88F.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Struct_e_struct*_tmp890=_cycalloc(sizeof(*_tmp890));_tmp890[0]=({
struct Cyc_Absyn_Struct_e_struct _tmp891;_tmp891.tag=30;_tmp891.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp891.f2=Cyc_yyget_YY39(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp891.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp891.f4=0;_tmp891;});
_tmp890;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp88F;});break;case 477: _LL3E7: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp892;(_tmp892.YY3).tag=7;(_tmp892.YY3).f1=Cyc_Absyn_stmt_exp(
Cyc_yyget_YY8(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp892;});
break;case 478: _LL3E8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp893;(_tmp893.YY4).tag=8;(_tmp893.YY4).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp893;});break;case 479: _LL3E9: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp894;(_tmp894.YY4).tag=8;(_tmp894.YY4).f1=({struct Cyc_List_List*
_tmp895=_cycalloc(sizeof(*_tmp895));_tmp895->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp895->tl=0;_tmp895;});
_tmp894;});break;case 480: _LL3EA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp896;(_tmp896.YY4).tag=8;(_tmp896.YY4).f1=({struct Cyc_List_List*_tmp897=
_cycalloc(sizeof(*_tmp897));_tmp897->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp897->tl=Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp897;});_tmp896;});
break;case 481: _LL3EB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp898;(_tmp898.YY3).tag=7;(_tmp898.YY3).f1=Cyc_Absyn_int_exp((*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp898;});
break;case 482: _LL3EC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp899;(_tmp899.YY3).tag=7;(_tmp899.YY3).f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp899;});
break;case 483: _LL3ED: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp89A;(_tmp89A.YY3).tag=7;(_tmp89A.YY3).f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89A;});
break;case 484: _LL3EE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp89B;(_tmp89B.YY3).tag=7;(_tmp89B.YY3).f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89B;});
break;case 485: _LL3EF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp89C;(_tmp89C.QualId_tok).tag=4;(_tmp89C.QualId_tok).f1=({struct _tuple1*
_tmp89D=_cycalloc(sizeof(*_tmp89D));_tmp89D->f1=Cyc_Absyn_rel_ns_null;_tmp89D->f2=({
struct _dynforward_ptr*_tmp89E=_cycalloc(sizeof(*_tmp89E));_tmp89E[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp89E;});_tmp89D;});
_tmp89C;});break;case 486: _LL3F0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 487: _LL3F1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp89F;(_tmp89F.QualId_tok).tag=4;(_tmp89F.QualId_tok).f1=({struct _tuple1*
_tmp8A0=_cycalloc(sizeof(*_tmp8A0));_tmp8A0->f1=Cyc_Absyn_rel_ns_null;_tmp8A0->f2=({
struct _dynforward_ptr*_tmp8A1=_cycalloc(sizeof(*_tmp8A1));_tmp8A1[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8A1;});_tmp8A0;});
_tmp89F;});break;case 488: _LL3F2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 489: _LL3F3: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 490: _LL3F4: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 491: _LL3F5: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 492: _LL3F6:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 493:
_LL3F7: break;case 494: _LL3F8:((struct Cyc_Lexing_lexbuf*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;break;default: _LL3F9: break;}
yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=yyval;yylsp_offset ++;if(
yylen == 0){(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line=
Cyc_yylloc.first_line;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_column=
Cyc_yylloc.first_column;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_column;}else{(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_column;}
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(495,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(141,yyn - 130)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
5921) && Cyc_yycheck[_check_known_subscript_notnull(5922,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(5922,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(141,yyn - 130)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(979,yystate)];if(yyn > - 32768  && yyn < 5921){int sze=
0;struct _dynforward_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 271 / 
sizeof(char*);x ++){if(Cyc_yycheck[_check_known_subscript_notnull(5922,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dynforward_ptr)Cyc_yytname[
_check_known_subscript_notnull(271,x)])+ 15,count ++);}msg=({unsigned int _tmp8A2=(
unsigned int)(sze + 15);char*_tmp8A3=(char*)_cycalloc_atomic(_check_times(sizeof(
char),_tmp8A2 + 1));struct _dynforward_ptr _tmp8A5=_tag_dynforward(_tmp8A3,sizeof(
char),_tmp8A2 + 1);{unsigned int _tmp8A4=_tmp8A2;unsigned int i;for(i=0;i < _tmp8A4;
i ++){_tmp8A3[i]='\000';}_tmp8A3[_tmp8A4]=(char)0;}_tmp8A5;});Cyc_strcpy(msg,({
const char*_tmp8A6="parse error";_tag_dynforward(_tmp8A6,sizeof(char),
_get_zero_arr_size(_tmp8A6,12));}));if(count < 5){count=0;for(x=yyn < 0?- yyn: 0;x < 
271 / sizeof(char*);x ++){if(Cyc_yycheck[_check_known_subscript_notnull(5922,x + 
yyn)]== x){Cyc_strcat(msg,(struct _dynforward_ptr)(count == 0?(struct
_dynforward_ptr)({const char*_tmp8A7=", expecting `";_tag_dynforward(_tmp8A7,
sizeof(char),_get_zero_arr_size(_tmp8A7,14));}):(struct _dynforward_ptr)({const
char*_tmp8A8=" or `";_tag_dynforward(_tmp8A8,sizeof(char),_get_zero_arr_size(
_tmp8A8,6));})));Cyc_strcat(msg,(struct _dynforward_ptr)Cyc_yytname[
_check_known_subscript_notnull(271,x)]);Cyc_strcat(msg,({const char*_tmp8A9="'";
_tag_dynforward(_tmp8A9,sizeof(char),_get_zero_arr_size(_tmp8A9,2));}));count ++;}}}
Cyc_yyerror((struct _dynforward_ptr)msg);}else{Cyc_yyerror(({const char*_tmp8AA="parse error";
_tag_dynforward(_tmp8AA,sizeof(char),_get_zero_arr_size(_tmp8AA,12));}));}}goto
yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0)return 1;Cyc_yychar=- 2;}
yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0)return 1;
yyvsp_offset --;yystate=(int)yyss[_check_known_subscript_notnull(10000,--
yyssp_offset)];yylsp_offset --;yyerrhandle: yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(979,yystate)];if(yyn == - 32768)goto yyerrdefault;
yyn +=1;if((yyn < 0  || yyn > 5921) || Cyc_yycheck[_check_known_subscript_notnull(
5922,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(5922,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 978)
return 0;yyvs[_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;
yyls[_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto
yynewstate;}void Cyc_yyprint(int i,union Cyc_YYSTYPE_union v){union Cyc_YYSTYPE_union
_tmp8B4=v;struct _tuple11*_tmp8B5;struct _tuple11 _tmp8B6;int _tmp8B7;char _tmp8B8;
struct _dynforward_ptr _tmp8B9;struct _tuple1*_tmp8BA;struct _tuple1 _tmp8BB;void*
_tmp8BC;struct _dynforward_ptr*_tmp8BD;_LL3FC: if((_tmp8B4.Int_tok).tag != 0)goto
_LL3FE;_tmp8B5=(_tmp8B4.Int_tok).f1;_tmp8B6=*_tmp8B5;_tmp8B7=_tmp8B6.f2;_LL3FD:({
struct Cyc_Int_pa_struct _tmp8C0;_tmp8C0.tag=1;_tmp8C0.f1=(unsigned long)_tmp8B7;{
void*_tmp8BE[1]={& _tmp8C0};Cyc_fprintf(Cyc_stderr,({const char*_tmp8BF="%d";
_tag_dynforward(_tmp8BF,sizeof(char),_get_zero_arr_size(_tmp8BF,3));}),
_tag_dynforward(_tmp8BE,sizeof(void*),1));}});goto _LL3FB;_LL3FE: if((_tmp8B4.Char_tok).tag
!= 1)goto _LL400;_tmp8B8=(_tmp8B4.Char_tok).f1;_LL3FF:({struct Cyc_Int_pa_struct
_tmp8C3;_tmp8C3.tag=1;_tmp8C3.f1=(unsigned long)((int)_tmp8B8);{void*_tmp8C1[1]={&
_tmp8C3};Cyc_fprintf(Cyc_stderr,({const char*_tmp8C2="%c";_tag_dynforward(_tmp8C2,
sizeof(char),_get_zero_arr_size(_tmp8C2,3));}),_tag_dynforward(_tmp8C1,sizeof(
void*),1));}});goto _LL3FB;_LL400: if((_tmp8B4.String_tok).tag != 2)goto _LL402;
_tmp8B9=(_tmp8B4.String_tok).f1;_LL401:({struct Cyc_String_pa_struct _tmp8C6;
_tmp8C6.tag=0;_tmp8C6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp8B9);{
void*_tmp8C4[1]={& _tmp8C6};Cyc_fprintf(Cyc_stderr,({const char*_tmp8C5="\"%s\"";
_tag_dynforward(_tmp8C5,sizeof(char),_get_zero_arr_size(_tmp8C5,5));}),
_tag_dynforward(_tmp8C4,sizeof(void*),1));}});goto _LL3FB;_LL402: if((_tmp8B4.QualId_tok).tag
!= 4)goto _LL404;_tmp8BA=(_tmp8B4.QualId_tok).f1;_tmp8BB=*_tmp8BA;_tmp8BC=_tmp8BB.f1;
_tmp8BD=_tmp8BB.f2;_LL403: {struct Cyc_List_List*_tmp8C7=0;{void*_tmp8C8=_tmp8BC;
struct Cyc_List_List*_tmp8C9;struct Cyc_List_List*_tmp8CA;_LL407: if(_tmp8C8 <= (
void*)1)goto _LL40B;if(*((int*)_tmp8C8)!= 0)goto _LL409;_tmp8C9=((struct Cyc_Absyn_Rel_n_struct*)
_tmp8C8)->f1;_LL408: _tmp8C7=_tmp8C9;goto _LL406;_LL409: if(*((int*)_tmp8C8)!= 1)
goto _LL40B;_tmp8CA=((struct Cyc_Absyn_Abs_n_struct*)_tmp8C8)->f1;_LL40A: _tmp8C7=
_tmp8CA;goto _LL406;_LL40B: if((int)_tmp8C8 != 0)goto _LL406;_LL40C: goto _LL406;
_LL406:;}for(0;_tmp8C7 != 0;_tmp8C7=_tmp8C7->tl){({struct Cyc_String_pa_struct
_tmp8CD;_tmp8CD.tag=0;_tmp8CD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct _dynforward_ptr*)_tmp8C7->hd));{void*_tmp8CB[1]={& _tmp8CD};Cyc_fprintf(Cyc_stderr,({
const char*_tmp8CC="%s::";_tag_dynforward(_tmp8CC,sizeof(char),_get_zero_arr_size(
_tmp8CC,5));}),_tag_dynforward(_tmp8CB,sizeof(void*),1));}});}({struct Cyc_String_pa_struct
_tmp8D0;_tmp8D0.tag=0;_tmp8D0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp8BD);{void*_tmp8CE[1]={& _tmp8D0};Cyc_fprintf(Cyc_stderr,({const char*_tmp8CF="%s::";
_tag_dynforward(_tmp8CF,sizeof(char),_get_zero_arr_size(_tmp8CF,5));}),
_tag_dynforward(_tmp8CE,sizeof(void*),1));}});goto _LL3FB;}_LL404:;_LL405:({void*
_tmp8D1[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp8D2="?";_tag_dynforward(
_tmp8D2,sizeof(char),_get_zero_arr_size(_tmp8D2,2));}),_tag_dynforward(_tmp8D1,
sizeof(void*),0));});goto _LL3FB;_LL3FB:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_Parse_lbuf=({struct Cyc_Core_Opt*
_tmp8D3=_cycalloc(sizeof(*_tmp8D3));_tmp8D3->v=Cyc_Lexing_from_file(f);_tmp8D3;});
Cyc_yyparse();return Cyc_Parse_parse_result;}
