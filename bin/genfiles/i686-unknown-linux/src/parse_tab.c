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
_tuple1*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct
Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct
Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*
targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple1*
tunion_name;struct _tuple1*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
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
is_xtunion;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*
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
is_xtunion,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(struct Cyc_List_List*
tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,
struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(void*a);struct Cyc_Iter_Iter{
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
eff1,void*eff2);struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
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
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmpA5->tvs));t=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({
struct Cyc_Absyn_TunionType_struct _tmpB0;_tmpB0.tag=2;_tmpB0.f1=({struct Cyc_Absyn_TunionInfo
_tmpB1;_tmpB1.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmpB3;_tmpB3.tag=1;_tmpB3.f1=({struct Cyc_Absyn_Tuniondecl**_tmpB4=_cycalloc(
sizeof(*_tmpB4));_tmpB4[0]=_tmpA5;_tmpB4;});_tmpB3;});_tmpB2;}));_tmpB1.targs=
_tmpAE;_tmpB1.rgn=(void*)((void*)2);_tmpB1;});_tmpB0;});_tmpAF;});if(_tmpA5->fields
!= 0)declopt=({struct Cyc_Core_Opt*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->v=
_tmp9B;_tmpB5;});goto _LL52;}_LL57: if(*((int*)_tmpA3)!= 6)goto _LL59;_tmpA6=((
struct Cyc_Absyn_Enum_d_struct*)_tmpA3)->f1;_LL58: t=(void*)({struct Cyc_Absyn_EnumType_struct*
_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_EnumType_struct
_tmpB7;_tmpB7.tag=12;_tmpB7.f1=_tmpA6->name;_tmpB7.f2=0;_tmpB7;});_tmpB6;});
declopt=({struct Cyc_Core_Opt*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->v=_tmp9B;
_tmpB8;});goto _LL52;_LL59:;_LL5A:({void*_tmpB9[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmp9B->loc,({
const char*_tmpBA="bad declaration within type specifier";_tag_dynforward(_tmpBA,
sizeof(char),_get_zero_arr_size(_tmpBA,38));}),_tag_dynforward(_tmpB9,sizeof(
void*),0));});_LL52:;}}else{Cyc_Parse_err(Cyc_Parse_msg3,_tmp9B->loc);}goto _LL40;
_LL40:;}if(!seen_type){if(!seen_sign  && !seen_size)({void*_tmpBB[0]={};Cyc_Tcutil_warn(
last_loc,({const char*_tmpBC="missing type within specifier";_tag_dynforward(
_tmpBC,sizeof(char),_get_zero_arr_size(_tmpBC,30));}),_tag_dynforward(_tmpBB,
sizeof(void*),0));});t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpBD=_cycalloc(
sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Absyn_IntType_struct _tmpBE;_tmpBE.tag=5;
_tmpBE.f1=(void*)sgn;_tmpBE.f2=(void*)sz;_tmpBE;});_tmpBD;});}else{if(seen_sign){
void*_tmpBF=t;void*_tmpC0;_LL5C: if(_tmpBF <= (void*)4)goto _LL5E;if(*((int*)_tmpBF)
!= 5)goto _LL5E;_tmpC0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBF)->f2;_LL5D:
t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpC1=_cycalloc(sizeof(*_tmpC1));
_tmpC1[0]=({struct Cyc_Absyn_IntType_struct _tmpC2;_tmpC2.tag=5;_tmpC2.f1=(void*)
sgn;_tmpC2.f2=(void*)_tmpC0;_tmpC2;});_tmpC1;});goto _LL5B;_LL5E:;_LL5F: Cyc_Parse_err(({
const char*_tmpC3="sign specification on non-integral type";_tag_dynforward(
_tmpC3,sizeof(char),_get_zero_arr_size(_tmpC3,40));}),last_loc);goto _LL5B;_LL5B:;}
if(seen_size){void*_tmpC4=t;void*_tmpC5;_LL61: if(_tmpC4 <= (void*)4)goto _LL65;if(*((
int*)_tmpC4)!= 5)goto _LL63;_tmpC5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpC4)->f1;_LL62: t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpC6=_cycalloc(
sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_IntType_struct _tmpC7;_tmpC7.tag=5;
_tmpC7.f1=(void*)_tmpC5;_tmpC7.f2=(void*)sz;_tmpC7;});_tmpC6;});goto _LL60;_LL63:
if(*((int*)_tmpC4)!= 6)goto _LL65;_LL64: t=(void*)({struct Cyc_Absyn_DoubleType_struct*
_tmpC8=_cycalloc_atomic(sizeof(*_tmpC8));_tmpC8[0]=({struct Cyc_Absyn_DoubleType_struct
_tmpC9;_tmpC9.tag=6;_tmpC9.f1=1;_tmpC9;});_tmpC8;});goto _LL60;_LL65:;_LL66: Cyc_Parse_err(({
const char*_tmpCA="size qualifier on non-integral type";_tag_dynforward(_tmpCA,
sizeof(char),_get_zero_arr_size(_tmpCA,36));}),last_loc);goto _LL60;_LL60:;}}
return({struct _tuple5 _tmpCB;_tmpCB.f1=t;_tmpCB.f2=declopt;_tmpCB;});}static
struct Cyc_List_List*Cyc_Parse_apply_tmss(struct Cyc_Absyn_Tqual tq,void*t,struct
Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{struct Cyc_Parse_Declarator*
_tmpCC=(struct Cyc_Parse_Declarator*)ds->hd;struct _tuple1*_tmpCD=_tmpCC->id;
struct Cyc_Absyn_Tqual _tmpCF;void*_tmpD0;struct Cyc_List_List*_tmpD1;struct Cyc_List_List*
_tmpD2;struct _tuple6 _tmpCE=Cyc_Parse_apply_tms(tq,t,shared_atts,_tmpCC->tms);
_tmpCF=_tmpCE.f1;_tmpD0=_tmpCE.f2;_tmpD1=_tmpCE.f3;_tmpD2=_tmpCE.f4;return({
struct Cyc_List_List*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->hd=({struct _tuple7*
_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->f1=_tmpCD;_tmpD4->f2=_tmpCF;_tmpD4->f3=
_tmpD0;_tmpD4->f4=_tmpD1;_tmpD4->f5=_tmpD2;_tmpD4;});_tmpD3->tl=Cyc_Parse_apply_tmss(
_tmpCF,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts);_tmpD3;});}}static struct
_tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0)return({struct _tuple6 _tmpD5;_tmpD5.f1=
tq;_tmpD5.f2=t;_tmpD5.f3=0;_tmpD5.f4=atts;_tmpD5;});{void*_tmpD6=(void*)tms->hd;
struct Cyc_Absyn_Conref*_tmpD7;struct Cyc_Position_Segment*_tmpD8;struct Cyc_Absyn_Exp*
_tmpD9;struct Cyc_Absyn_Conref*_tmpDA;struct Cyc_Position_Segment*_tmpDB;void*
_tmpDC;struct Cyc_List_List*_tmpDD;struct Cyc_Position_Segment*_tmpDE;struct Cyc_Absyn_PtrAtts
_tmpDF;struct Cyc_Absyn_Tqual _tmpE0;struct Cyc_Position_Segment*_tmpE1;struct Cyc_List_List*
_tmpE2;_LL68: if(*((int*)_tmpD6)!= 0)goto _LL6A;_tmpD7=((struct Cyc_Absyn_Carray_mod_struct*)
_tmpD6)->f1;_tmpD8=((struct Cyc_Absyn_Carray_mod_struct*)_tmpD6)->f2;_LL69: return
Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,0,_tmpD7,
_tmpD8),atts,tms->tl);_LL6A: if(*((int*)_tmpD6)!= 1)goto _LL6C;_tmpD9=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpD6)->f1;_tmpDA=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpD6)->f2;_tmpDB=((
struct Cyc_Absyn_ConstArray_mod_struct*)_tmpD6)->f3;_LL6B: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)_tmpD9,
_tmpDA,_tmpDB),atts,tms->tl);_LL6C: if(*((int*)_tmpD6)!= 3)goto _LL6E;_tmpDC=(void*)((
struct Cyc_Absyn_Function_mod_struct*)_tmpD6)->f1;_LL6D: {void*_tmpE3=_tmpDC;
struct Cyc_List_List*_tmpE4;int _tmpE5;struct Cyc_Absyn_VarargInfo*_tmpE6;struct Cyc_Core_Opt*
_tmpE7;struct Cyc_List_List*_tmpE8;struct Cyc_Position_Segment*_tmpE9;_LL75: if(*((
int*)_tmpE3)!= 1)goto _LL77;_tmpE4=((struct Cyc_Absyn_WithTypes_struct*)_tmpE3)->f1;
_tmpE5=((struct Cyc_Absyn_WithTypes_struct*)_tmpE3)->f2;_tmpE6=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE3)->f3;_tmpE7=((struct Cyc_Absyn_WithTypes_struct*)_tmpE3)->f4;_tmpE8=((
struct Cyc_Absyn_WithTypes_struct*)_tmpE3)->f5;_LL76: {struct Cyc_List_List*
typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{struct
Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->hd=(void*)((
void*)as->hd);_tmpEA->tl=fn_atts;_tmpEA;});else{new_atts=({struct Cyc_List_List*
_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->hd=(void*)((void*)as->hd);_tmpEB->tl=
new_atts;_tmpEB;});}}}if(tms->tl != 0){void*_tmpEC=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;struct Cyc_List_List*_tmpED;_LL7A: if(*((int*)_tmpEC)!= 
4)goto _LL7C;_tmpED=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpEC)->f1;_LL7B:
typvars=_tmpED;tms=tms->tl;goto _LL79;_LL7C:;_LL7D: goto _LL79;_LL79:;}if(((((!
_tmpE5  && _tmpE6 == 0) && _tmpE4 != 0) && _tmpE4->tl == 0) && (*((struct _tuple2*)
_tmpE4->hd)).f1 == 0) && (*((struct _tuple2*)_tmpE4->hd)).f3 == (void*)0)_tmpE4=0;t=
Cyc_Parse_array2ptr(t,0);((void(*)(void(*f)(struct _tuple2*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,_tmpE4);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(
tq.loc),Cyc_Absyn_function_typ(typvars,_tmpE7,t,_tmpE4,_tmpE5,_tmpE6,_tmpE8,
fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);}_LL77: if(*((int*)
_tmpE3)!= 0)goto _LL74;_tmpE9=((struct Cyc_Absyn_NoTypes_struct*)_tmpE3)->f2;_LL78:({
void*_tmpEE[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmpE9,({const char*_tmpEF="function declaration without parameter types";
_tag_dynforward(_tmpEF,sizeof(char),_get_zero_arr_size(_tmpEF,45));}),
_tag_dynforward(_tmpEE,sizeof(void*),0));});_LL74:;}_LL6E: if(*((int*)_tmpD6)!= 4)
goto _LL70;_tmpDD=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpD6)->f1;_tmpDE=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpD6)->f2;_LL6F: if(tms->tl == 0)return({
struct _tuple6 _tmpF0;_tmpF0.f1=tq;_tmpF0.f2=t;_tmpF0.f3=_tmpDD;_tmpF0.f4=atts;
_tmpF0;});({void*_tmpF1[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmpDE,({const char*
_tmpF2="type parameters must appear before function arguments in declarator";
_tag_dynforward(_tmpF2,sizeof(char),_get_zero_arr_size(_tmpF2,68));}),
_tag_dynforward(_tmpF1,sizeof(void*),0));});_LL70: if(*((int*)_tmpD6)!= 2)goto
_LL72;_tmpDF=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpD6)->f1;_tmpE0=((struct
Cyc_Absyn_Pointer_mod_struct*)_tmpD6)->f2;_LL71: return Cyc_Parse_apply_tms(_tmpE0,(
void*)({struct Cyc_Absyn_PointerType_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));
_tmpF3[0]=({struct Cyc_Absyn_PointerType_struct _tmpF4;_tmpF4.tag=4;_tmpF4.f1=({
struct Cyc_Absyn_PtrInfo _tmpF5;_tmpF5.elt_typ=(void*)t;_tmpF5.elt_tq=tq;_tmpF5.ptr_atts=
_tmpDF;_tmpF5;});_tmpF4;});_tmpF3;}),atts,tms->tl);_LL72: if(*((int*)_tmpD6)!= 5)
goto _LL67;_tmpE1=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpD6)->f1;_tmpE2=((
struct Cyc_Absyn_Attributes_mod_struct*)_tmpD6)->f2;_LL73: return Cyc_Parse_apply_tms(
tq,t,Cyc_List_append(atts,_tmpE2),tms->tl);_LL67:;}}void*Cyc_Parse_speclist2typ(
struct Cyc_List_List*tss,struct Cyc_Position_Segment*loc){void*_tmpF7;struct Cyc_Core_Opt*
_tmpF8;struct _tuple5 _tmpF6=Cyc_Parse_collapse_type_specifiers(tss,loc);_tmpF7=
_tmpF6.f1;_tmpF8=_tmpF6.f2;if(_tmpF8 != 0)({void*_tmpF9[0]={};Cyc_Tcutil_warn(loc,({
const char*_tmpFA="ignoring nested type declaration(s) in specifier list";
_tag_dynforward(_tmpFA,sizeof(char),_get_zero_arr_size(_tmpFA,54));}),
_tag_dynforward(_tmpF9,sizeof(void*),0));});return _tmpF7;}static struct Cyc_Absyn_Decl*
Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*loc,struct _tuple7*t){
struct _tuple7 _tmpFC;struct _tuple1*_tmpFD;struct Cyc_Absyn_Tqual _tmpFE;void*_tmpFF;
struct Cyc_List_List*_tmp100;struct Cyc_List_List*_tmp101;struct _tuple7*_tmpFB=t;
_tmpFC=*_tmpFB;_tmpFD=_tmpFC.f1;_tmpFE=_tmpFC.f2;_tmpFF=_tmpFC.f3;_tmp100=_tmpFC.f4;
_tmp101=_tmpFC.f5;Cyc_Lex_register_typedef(_tmpFD);{struct Cyc_Core_Opt*kind;
struct Cyc_Core_Opt*type;{void*_tmp102=_tmpFF;struct Cyc_Core_Opt*_tmp103;_LL7F:
if(_tmp102 <= (void*)4)goto _LL81;if(*((int*)_tmp102)!= 0)goto _LL81;_tmp103=((
struct Cyc_Absyn_Evar_struct*)_tmp102)->f1;_LL80: type=0;if(_tmp103 == 0)kind=({
struct Cyc_Core_Opt*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->v=(void*)((void*)
2);_tmp104;});else{kind=_tmp103;}goto _LL7E;_LL81:;_LL82: kind=0;type=({struct Cyc_Core_Opt*
_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->v=(void*)_tmpFF;_tmp105;});goto
_LL7E;_LL7E:;}return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_struct*
_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106[0]=({struct Cyc_Absyn_Typedef_d_struct
_tmp107;_tmp107.tag=7;_tmp107.f1=({struct Cyc_Absyn_Typedefdecl*_tmp108=_cycalloc(
sizeof(*_tmp108));_tmp108->name=_tmpFD;_tmp108->tvs=_tmp100;_tmp108->kind=kind;
_tmp108->defn=type;_tmp108->atts=_tmp101;_tmp108->tq=_tmpFE;_tmp108;});_tmp107;});
_tmp106;}),loc);}}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*
d,struct Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp10A;_tmp10A.tag=11;_tmp10A.f1=d;_tmp10A.f2=s;_tmp10A;});_tmp109;}),Cyc_Position_segment_join(
d->loc,s->loc));}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*
x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*
loc){struct Cyc_Parse_Declaration_spec _tmp10C;struct Cyc_Absyn_Tqual _tmp10D;struct
Cyc_List_List*_tmp10E;struct Cyc_List_List*_tmp10F;struct Cyc_Parse_Declaration_spec*
_tmp10B=ds;_tmp10C=*_tmp10B;_tmp10D=_tmp10C.tq;_tmp10E=_tmp10C.type_specs;
_tmp10F=_tmp10C.attributes;if(_tmp10D.loc == 0)_tmp10D.loc=tqual_loc;if(ds->is_inline)
Cyc_Parse_err(({const char*_tmp110="inline is allowed only on function definitions";
_tag_dynforward(_tmp110,sizeof(char),_get_zero_arr_size(_tmp110,47));}),loc);if(
_tmp10E == 0){Cyc_Parse_err(({const char*_tmp111="missing type specifiers in declaration";
_tag_dynforward(_tmp111,sizeof(char),_get_zero_arr_size(_tmp111,39));}),loc);
return 0;}{void*s=(void*)2;int istypedef=0;if(ds->sc != 0){void*_tmp112=(void*)((
struct Cyc_Core_Opt*)_check_null(ds->sc))->v;_LL84: if((int)_tmp112 != 0)goto _LL86;
_LL85: istypedef=1;goto _LL83;_LL86: if((int)_tmp112 != 1)goto _LL88;_LL87: s=(void*)3;
goto _LL83;_LL88: if((int)_tmp112 != 2)goto _LL8A;_LL89: s=(void*)4;goto _LL83;_LL8A:
if((int)_tmp112 != 3)goto _LL8C;_LL8B: s=(void*)0;goto _LL83;_LL8C: if((int)_tmp112 != 
4)goto _LL8E;_LL8D: s=(void*)2;goto _LL83;_LL8E: if((int)_tmp112 != 5)goto _LL90;_LL8F:
if(Cyc_Parse_no_register)s=(void*)2;else{s=(void*)5;}goto _LL83;_LL90: if((int)
_tmp112 != 6)goto _LL83;_LL91: s=(void*)1;goto _LL83;_LL83:;}{struct Cyc_List_List*
_tmp114;struct Cyc_List_List*_tmp115;struct _tuple0 _tmp113=((struct _tuple0(*)(
struct Cyc_List_List*x))Cyc_List_split)(ids);_tmp114=_tmp113.f1;_tmp115=_tmp113.f2;{
int exps_empty=1;{struct Cyc_List_List*es=_tmp115;for(0;es != 0;es=es->tl){if((
struct Cyc_Absyn_Exp*)es->hd != 0){exps_empty=0;break;}}}{struct _tuple5 _tmp116=Cyc_Parse_collapse_type_specifiers(
_tmp10E,loc);if(_tmp114 == 0){void*_tmp118;struct Cyc_Core_Opt*_tmp119;struct
_tuple5 _tmp117=_tmp116;_tmp118=_tmp117.f1;_tmp119=_tmp117.f2;if(_tmp119 != 0){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp119->v;{void*_tmp11A=(void*)d->r;
struct Cyc_Absyn_Enumdecl*_tmp11B;struct Cyc_Absyn_Aggrdecl*_tmp11C;struct Cyc_Absyn_Tuniondecl*
_tmp11D;_LL93: if(_tmp11A <= (void*)2)goto _LL99;if(*((int*)_tmp11A)!= 6)goto _LL95;
_tmp11B=((struct Cyc_Absyn_Enum_d_struct*)_tmp11A)->f1;_LL94:(void*)(_tmp11B->sc=(
void*)s);if(_tmp10F != 0)Cyc_Parse_err(({const char*_tmp11E="bad attributes on enum";
_tag_dynforward(_tmp11E,sizeof(char),_get_zero_arr_size(_tmp11E,23));}),loc);
goto _LL92;_LL95: if(*((int*)_tmp11A)!= 4)goto _LL97;_tmp11C=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp11A)->f1;_LL96:(void*)(_tmp11C->sc=(void*)s);_tmp11C->attributes=_tmp10F;
goto _LL92;_LL97: if(*((int*)_tmp11A)!= 5)goto _LL99;_tmp11D=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp11A)->f1;_LL98:(void*)(_tmp11D->sc=(void*)s);if(_tmp10F != 0)Cyc_Parse_err(({
const char*_tmp11F="bad attributes on tunion";_tag_dynforward(_tmp11F,sizeof(char),
_get_zero_arr_size(_tmp11F,25));}),loc);goto _LL92;_LL99:;_LL9A: Cyc_Parse_err(({
const char*_tmp120="bad declaration";_tag_dynforward(_tmp120,sizeof(char),
_get_zero_arr_size(_tmp120,16));}),loc);return 0;_LL92:;}return({struct Cyc_List_List*
_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->hd=d;_tmp121->tl=0;_tmp121;});}
else{void*_tmp122=_tmp118;struct Cyc_Absyn_AggrInfo _tmp123;void*_tmp124;void*
_tmp125;struct _tuple1*_tmp126;struct Cyc_List_List*_tmp127;struct Cyc_Absyn_TunionInfo
_tmp128;void*_tmp129;struct Cyc_Absyn_Tuniondecl**_tmp12A;struct Cyc_Absyn_TunionInfo
_tmp12B;void*_tmp12C;struct Cyc_Absyn_UnknownTunionInfo _tmp12D;struct _tuple1*
_tmp12E;int _tmp12F;struct Cyc_List_List*_tmp130;struct _tuple1*_tmp131;struct Cyc_List_List*
_tmp132;_LL9C: if(_tmp122 <= (void*)4)goto _LLA6;if(*((int*)_tmp122)!= 10)goto _LL9E;
_tmp123=((struct Cyc_Absyn_AggrType_struct*)_tmp122)->f1;_tmp124=(void*)_tmp123.aggr_info;
if(*((int*)_tmp124)!= 0)goto _LL9E;_tmp125=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp124)->f1;_tmp126=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp124)->f2;_tmp127=
_tmp123.targs;_LL9D: {struct Cyc_List_List*_tmp133=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp127);
struct Cyc_Absyn_Aggrdecl*_tmp134=({struct Cyc_Absyn_Aggrdecl*_tmp139=_cycalloc(
sizeof(*_tmp139));_tmp139->kind=(void*)_tmp125;_tmp139->sc=(void*)s;_tmp139->name=
_tmp126;_tmp139->tvs=_tmp133;_tmp139->impl=0;_tmp139->attributes=0;_tmp139;});
if(_tmp10F != 0)Cyc_Parse_err(({const char*_tmp135="bad attributes on type declaration";
_tag_dynforward(_tmp135,sizeof(char),_get_zero_arr_size(_tmp135,35));}),loc);
return({struct Cyc_List_List*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp137=_cycalloc(sizeof(*_tmp137));
_tmp137[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp138;_tmp138.tag=4;_tmp138.f1=
_tmp134;_tmp138;});_tmp137;}),loc);_tmp136->tl=0;_tmp136;});}_LL9E: if(*((int*)
_tmp122)!= 2)goto _LLA0;_tmp128=((struct Cyc_Absyn_TunionType_struct*)_tmp122)->f1;
_tmp129=(void*)_tmp128.tunion_info;if(*((int*)_tmp129)!= 1)goto _LLA0;_tmp12A=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp129)->f1;_LL9F: if(_tmp10F != 0)Cyc_Parse_err(({
const char*_tmp13A="bad attributes on tunion";_tag_dynforward(_tmp13A,sizeof(char),
_get_zero_arr_size(_tmp13A,25));}),loc);return({struct Cyc_List_List*_tmp13B=
_cycalloc(sizeof(*_tmp13B));_tmp13B->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*
_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C[0]=({struct Cyc_Absyn_Tunion_d_struct
_tmp13D;_tmp13D.tag=5;_tmp13D.f1=*_tmp12A;_tmp13D;});_tmp13C;}),loc);_tmp13B->tl=
0;_tmp13B;});_LLA0: if(*((int*)_tmp122)!= 2)goto _LLA2;_tmp12B=((struct Cyc_Absyn_TunionType_struct*)
_tmp122)->f1;_tmp12C=(void*)_tmp12B.tunion_info;if(*((int*)_tmp12C)!= 0)goto
_LLA2;_tmp12D=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp12C)->f1;_tmp12E=
_tmp12D.name;_tmp12F=_tmp12D.is_xtunion;_tmp130=_tmp12B.targs;_LLA1: {struct Cyc_List_List*
_tmp13E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp130);struct Cyc_Absyn_Decl*_tmp13F=Cyc_Absyn_tunion_decl(s,_tmp12E,
_tmp13E,0,_tmp12F,loc);if(_tmp10F != 0)Cyc_Parse_err(({const char*_tmp140="bad attributes on tunion";
_tag_dynforward(_tmp140,sizeof(char),_get_zero_arr_size(_tmp140,25));}),loc);
return({struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->hd=
_tmp13F;_tmp141->tl=0;_tmp141;});}_LLA2: if(*((int*)_tmp122)!= 12)goto _LLA4;
_tmp131=((struct Cyc_Absyn_EnumType_struct*)_tmp122)->f1;_LLA3: {struct Cyc_Absyn_Enumdecl*
_tmp142=({struct Cyc_Absyn_Enumdecl*_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148->sc=(
void*)s;_tmp148->name=_tmp131;_tmp148->fields=0;_tmp148;});if(_tmp10F != 0)Cyc_Parse_err(({
const char*_tmp143="bad attributes on enum";_tag_dynforward(_tmp143,sizeof(char),
_get_zero_arr_size(_tmp143,23));}),loc);return({struct Cyc_List_List*_tmp144=
_cycalloc(sizeof(*_tmp144));_tmp144->hd=({struct Cyc_Absyn_Decl*_tmp145=_cycalloc(
sizeof(*_tmp145));_tmp145->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp147;_tmp147.tag=6;_tmp147.f1=_tmp142;_tmp147;});_tmp146;}));_tmp145->loc=loc;
_tmp145;});_tmp144->tl=0;_tmp144;});}_LLA4: if(*((int*)_tmp122)!= 13)goto _LLA6;
_tmp132=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp122)->f1;_LLA5: {struct Cyc_Absyn_Enumdecl*
_tmp149=({struct Cyc_Absyn_Enumdecl*_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F->sc=(
void*)s;_tmp14F->name=Cyc_Parse_gensym_enum();_tmp14F->fields=({struct Cyc_Core_Opt*
_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150->v=_tmp132;_tmp150;});_tmp14F;});if(
_tmp10F != 0)Cyc_Parse_err(({const char*_tmp14A="bad attributes on enum";
_tag_dynforward(_tmp14A,sizeof(char),_get_zero_arr_size(_tmp14A,23));}),loc);
return({struct Cyc_List_List*_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B->hd=({
struct Cyc_Absyn_Decl*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->r=(void*)((
void*)({struct Cyc_Absyn_Enum_d_struct*_tmp14D=_cycalloc(sizeof(*_tmp14D));
_tmp14D[0]=({struct Cyc_Absyn_Enum_d_struct _tmp14E;_tmp14E.tag=6;_tmp14E.f1=
_tmp149;_tmp14E;});_tmp14D;}));_tmp14C->loc=loc;_tmp14C;});_tmp14B->tl=0;_tmp14B;});}
_LLA6:;_LLA7: Cyc_Parse_err(({const char*_tmp151="missing declarator";
_tag_dynforward(_tmp151,sizeof(char),_get_zero_arr_size(_tmp151,19));}),loc);
return 0;_LL9B:;}}else{void*t=_tmp116.f1;struct Cyc_List_List*_tmp152=Cyc_Parse_apply_tmss(
_tmp10D,t,_tmp114,_tmp10F);if(istypedef){if(!exps_empty)Cyc_Parse_err(({const
char*_tmp153="initializer in typedef declaration";_tag_dynforward(_tmp153,
sizeof(char),_get_zero_arr_size(_tmp153,35));}),loc);{struct Cyc_List_List*decls=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,
struct _tuple7*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_v_typ_to_typedef,loc,_tmp152);if(_tmp116.f2 != 0){struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)(_tmp116.f2)->v;{void*_tmp154=(void*)d->r;struct Cyc_Absyn_Aggrdecl*
_tmp155;struct Cyc_Absyn_Tuniondecl*_tmp156;struct Cyc_Absyn_Enumdecl*_tmp157;
_LLA9: if(_tmp154 <= (void*)2)goto _LLAF;if(*((int*)_tmp154)!= 4)goto _LLAB;_tmp155=((
struct Cyc_Absyn_Aggr_d_struct*)_tmp154)->f1;_LLAA:(void*)(_tmp155->sc=(void*)s);
_tmp155->attributes=_tmp10F;_tmp10F=0;goto _LLA8;_LLAB: if(*((int*)_tmp154)!= 5)
goto _LLAD;_tmp156=((struct Cyc_Absyn_Tunion_d_struct*)_tmp154)->f1;_LLAC:(void*)(
_tmp156->sc=(void*)s);goto _LLA8;_LLAD: if(*((int*)_tmp154)!= 6)goto _LLAF;_tmp157=((
struct Cyc_Absyn_Enum_d_struct*)_tmp154)->f1;_LLAE:(void*)(_tmp157->sc=(void*)s);
goto _LLA8;_LLAF:;_LLB0: Cyc_Parse_err(({const char*_tmp158="declaration within typedef is not a struct, union, tunion, or xtunion";
_tag_dynforward(_tmp158,sizeof(char),_get_zero_arr_size(_tmp158,70));}),loc);
goto _LLA8;_LLA8:;}decls=({struct Cyc_List_List*_tmp159=_cycalloc(sizeof(*_tmp159));
_tmp159->hd=d;_tmp159->tl=decls;_tmp159;});}return decls;}}else{if(_tmp116.f2 != 0)
Cyc_Parse_unimp(({const char*_tmp15A="nested type declaration within declarator";
_tag_dynforward(_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,42));}),loc);{
struct Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp15B=_tmp152;for(0;_tmp15B != 
0;(_tmp15B=_tmp15B->tl,_tmp115=_tmp115->tl)){struct _tuple7 _tmp15D;struct _tuple1*
_tmp15E;struct Cyc_Absyn_Tqual _tmp15F;void*_tmp160;struct Cyc_List_List*_tmp161;
struct Cyc_List_List*_tmp162;struct _tuple7*_tmp15C=(struct _tuple7*)_tmp15B->hd;
_tmp15D=*_tmp15C;_tmp15E=_tmp15D.f1;_tmp15F=_tmp15D.f2;_tmp160=_tmp15D.f3;
_tmp161=_tmp15D.f4;_tmp162=_tmp15D.f5;if(_tmp161 != 0)({void*_tmp163[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp164="bad type params, ignoring";_tag_dynforward(_tmp164,
sizeof(char),_get_zero_arr_size(_tmp164,26));}),_tag_dynforward(_tmp163,sizeof(
void*),0));});if(_tmp115 == 0)({void*_tmp165[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({
const char*_tmp166="unexpected NULL in parse!";_tag_dynforward(_tmp166,sizeof(
char),_get_zero_arr_size(_tmp166,26));}),_tag_dynforward(_tmp165,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*_tmp167=(struct Cyc_Absyn_Exp*)_tmp115->hd;struct Cyc_Absyn_Vardecl*
_tmp168=Cyc_Absyn_new_vardecl(_tmp15E,_tmp160,_tmp167);_tmp168->tq=_tmp15F;(void*)(
_tmp168->sc=(void*)s);_tmp168->attributes=_tmp162;{struct Cyc_Absyn_Decl*_tmp169=({
struct Cyc_Absyn_Decl*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C[
0]=({struct Cyc_Absyn_Var_d_struct _tmp16D;_tmp16D.tag=0;_tmp16D.f1=_tmp168;
_tmp16D;});_tmp16C;}));_tmp16B->loc=loc;_tmp16B;});decls=({struct Cyc_List_List*
_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A->hd=_tmp169;_tmp16A->tl=decls;
_tmp16A;});}}}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
decls);}}}}}}}}static void*Cyc_Parse_id_to_kind(struct _dynforward_ptr s,struct Cyc_Position_Segment*
loc){if(Cyc_strlen((struct _dynforward_ptr)s)== 1)switch(*((const char*)
_check_dynforward_subscript(s,sizeof(char),0))){case 'A': _LLB1: return(void*)0;
case 'M': _LLB2: return(void*)1;case 'B': _LLB3: return(void*)2;case 'R': _LLB4: return(
void*)3;case 'E': _LLB5: return(void*)4;case 'I': _LLB6: return(void*)5;default: _LLB7:
break;}Cyc_Parse_err((struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp170;
_tmp170.tag=0;_tmp170.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)s);{void*
_tmp16E[1]={& _tmp170};Cyc_aprintf(({const char*_tmp16F="bad kind: %s";
_tag_dynforward(_tmp16F,sizeof(char),_get_zero_arr_size(_tmp16F,13));}),
_tag_dynforward(_tmp16E,sizeof(void*),1));}}),loc);return(void*)2;}static struct
Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){void*_tmp171=(void*)p->r;
struct _tuple1*_tmp172;struct Cyc_Absyn_Vardecl*_tmp173;struct Cyc_Absyn_Pat*
_tmp174;struct Cyc_Absyn_Pat _tmp175;void*_tmp176;struct Cyc_Absyn_Pat*_tmp177;void*
_tmp178;int _tmp179;char _tmp17A;struct _dynforward_ptr _tmp17B;struct _tuple1*
_tmp17C;struct Cyc_List_List*_tmp17D;int _tmp17E;struct Cyc_Absyn_Exp*_tmp17F;_LLBA:
if(_tmp171 <= (void*)2)goto _LLC0;if(*((int*)_tmp171)!= 12)goto _LLBC;_tmp172=((
struct Cyc_Absyn_UnknownId_p_struct*)_tmp171)->f1;_LLBB: return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp180=_cycalloc(sizeof(*_tmp180));
_tmp180[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp181;_tmp181.tag=2;_tmp181.f1=
_tmp172;_tmp181;});_tmp180;}),p->loc);_LLBC: if(*((int*)_tmp171)!= 1)goto _LLBE;
_tmp173=((struct Cyc_Absyn_Reference_p_struct*)_tmp171)->f1;_tmp174=((struct Cyc_Absyn_Reference_p_struct*)
_tmp171)->f2;_tmp175=*_tmp174;_tmp176=(void*)_tmp175.r;if((int)_tmp176 != 0)goto
_LLBE;_LLBD: return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp182=_cycalloc(sizeof(*_tmp182));_tmp182[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp183;_tmp183.tag=2;_tmp183.f1=_tmp173->name;_tmp183;});_tmp182;}),p->loc),p->loc);
_LLBE: if(*((int*)_tmp171)!= 4)goto _LLC0;_tmp177=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp171)->f1;_LLBF: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp177),p->loc);
_LLC0: if((int)_tmp171 != 1)goto _LLC2;_LLC1: return Cyc_Absyn_null_exp(p->loc);_LLC2:
if(_tmp171 <= (void*)2)goto _LLCC;if(*((int*)_tmp171)!= 7)goto _LLC4;_tmp178=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp171)->f1;_tmp179=((struct Cyc_Absyn_Int_p_struct*)
_tmp171)->f2;_LLC3: return Cyc_Absyn_int_exp(_tmp178,_tmp179,p->loc);_LLC4: if(*((
int*)_tmp171)!= 8)goto _LLC6;_tmp17A=((struct Cyc_Absyn_Char_p_struct*)_tmp171)->f1;
_LLC5: return Cyc_Absyn_char_exp(_tmp17A,p->loc);_LLC6: if(*((int*)_tmp171)!= 9)
goto _LLC8;_tmp17B=((struct Cyc_Absyn_Float_p_struct*)_tmp171)->f1;_LLC7: return Cyc_Absyn_float_exp(
_tmp17B,p->loc);_LLC8: if(*((int*)_tmp171)!= 13)goto _LLCA;_tmp17C=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp171)->f1;_tmp17D=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp171)->f2;
_tmp17E=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp171)->f3;if(_tmp17E != 0)goto
_LLCA;_LLC9: {struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp185;_tmp185.tag=2;_tmp185.f1=_tmp17C;_tmp185;});_tmp184;}),p->loc);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp17D);return Cyc_Absyn_unknowncall_exp(
e1,es,p->loc);}_LLCA: if(*((int*)_tmp171)!= 14)goto _LLCC;_tmp17F=((struct Cyc_Absyn_Exp_p_struct*)
_tmp171)->f1;_LLCB: return _tmp17F;_LLCC:;_LLCD: Cyc_Parse_err(({const char*_tmp186="cannot mix patterns and expressions in case";
_tag_dynforward(_tmp186,sizeof(char),_get_zero_arr_size(_tmp186,44));}),p->loc);
return Cyc_Absyn_null_exp(p->loc);_LLB9:;}char Cyc_AbstractDeclarator_tok[27]="\000\000\000\000AbstractDeclarator_tok\000";
struct Cyc_AbstractDeclarator_tok_struct{char*tag;struct Cyc_Parse_Abstractdeclarator*
f1;};char Cyc_AggrFieldDeclListList_tok[30]="\000\000\000\000AggrFieldDeclListList_tok\000";
struct Cyc_AggrFieldDeclListList_tok_struct{char*tag;struct Cyc_List_List*f1;};
char Cyc_AggrFieldDeclList_tok[26]="\000\000\000\000AggrFieldDeclList_tok\000";
struct Cyc_AggrFieldDeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_AttributeList_tok[
22]="\000\000\000\000AttributeList_tok\000";struct Cyc_AttributeList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_Attribute_tok[18]="\000\000\000\000Attribute_tok\000";
struct Cyc_Attribute_tok_struct{char*tag;void*f1;};char Cyc_Bool_tok[13]="\000\000\000\000Bool_tok\000";
struct Cyc_Bool_tok_struct{char*tag;int f1;};char Cyc_Char_tok[13]="\000\000\000\000Char_tok\000";
struct Cyc_Char_tok_struct{char*tag;char f1;};char Cyc_DeclList_tok[17]="\000\000\000\000DeclList_tok\000";
struct Cyc_DeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_DeclSpec_tok[
17]="\000\000\000\000DeclSpec_tok\000";struct Cyc_DeclSpec_tok_struct{char*tag;
struct Cyc_Parse_Declaration_spec*f1;};char Cyc_Declarator_tok[19]="\000\000\000\000Declarator_tok\000";
struct Cyc_Declarator_tok_struct{char*tag;struct Cyc_Parse_Declarator*f1;};char Cyc_DesignatorList_tok[
23]="\000\000\000\000DesignatorList_tok\000";struct Cyc_DesignatorList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_Designator_tok[19]="\000\000\000\000Designator_tok\000";
struct Cyc_Designator_tok_struct{char*tag;void*f1;};char Cyc_EnumfieldList_tok[22]="\000\000\000\000EnumfieldList_tok\000";
struct Cyc_EnumfieldList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_Enumfield_tok[
18]="\000\000\000\000Enumfield_tok\000";struct Cyc_Enumfield_tok_struct{char*tag;
struct Cyc_Absyn_Enumfield*f1;};char Cyc_ExpList_tok[16]="\000\000\000\000ExpList_tok\000";
struct Cyc_ExpList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_Exp_tok[
12]="\000\000\000\000Exp_tok\000";struct Cyc_Exp_tok_struct{char*tag;struct Cyc_Absyn_Exp*
f1;};char Cyc_FieldPatternList_tok[25]="\000\000\000\000FieldPatternList_tok\000";
struct Cyc_FieldPatternList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_FieldPattern_tok[
21]="\000\000\000\000FieldPattern_tok\000";struct _tuple11{struct Cyc_List_List*f1;
struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{char*tag;struct
_tuple11*f1;};char Cyc_FnDecl_tok[15]="\000\000\000\000FnDecl_tok\000";struct Cyc_FnDecl_tok_struct{
char*tag;struct Cyc_Absyn_Fndecl*f1;};char Cyc_IdList_tok[15]="\000\000\000\000IdList_tok\000";
struct Cyc_IdList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_InitDeclList_tok[
21]="\000\000\000\000InitDeclList_tok\000";struct Cyc_InitDeclList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_InitDecl_tok[17]="\000\000\000\000InitDecl_tok\000";
struct _tuple12{struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
char*tag;struct _tuple12*f1;};char Cyc_InitializerList_tok[24]="\000\000\000\000InitializerList_tok\000";
struct Cyc_InitializerList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_Int_tok[
12]="\000\000\000\000Int_tok\000";struct _tuple13{void*f1;int f2;};struct Cyc_Int_tok_struct{
char*tag;struct _tuple13*f1;};char Cyc_Kind_tok[13]="\000\000\000\000Kind_tok\000";
struct Cyc_Kind_tok_struct{char*tag;void*f1;};char Cyc_ParamDeclListBool_tok[26]="\000\000\000\000ParamDeclListBool_tok\000";
struct _tuple14{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct
Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_ParamDeclListBool_tok_struct{
char*tag;struct _tuple14*f1;};char Cyc_ParamDeclList_tok[22]="\000\000\000\000ParamDeclList_tok\000";
struct Cyc_ParamDeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_ParamDecl_tok[
18]="\000\000\000\000ParamDecl_tok\000";struct Cyc_ParamDecl_tok_struct{char*tag;
struct _tuple2*f1;};char Cyc_PatternList_tok[20]="\000\000\000\000PatternList_tok\000";
struct Cyc_PatternList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_Pattern_tok[
16]="\000\000\000\000Pattern_tok\000";struct Cyc_Pattern_tok_struct{char*tag;
struct Cyc_Absyn_Pat*f1;};char Cyc_Pointer_Sort_tok[21]="\000\000\000\000Pointer_Sort_tok\000";
struct Cyc_Pointer_Sort_tok_struct{char*tag;void*f1;};char Cyc_Primop_tok[15]="\000\000\000\000Primop_tok\000";
struct Cyc_Primop_tok_struct{char*tag;void*f1;};char Cyc_Primopopt_tok[18]="\000\000\000\000Primopopt_tok\000";
struct Cyc_Primopopt_tok_struct{char*tag;struct Cyc_Core_Opt*f1;};char Cyc_QualId_tok[
15]="\000\000\000\000QualId_tok\000";struct Cyc_QualId_tok_struct{char*tag;struct
_tuple1*f1;};char Cyc_QualSpecList_tok[21]="\000\000\000\000QualSpecList_tok\000";
struct _tuple15{struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};struct Cyc_QualSpecList_tok_struct{char*tag;struct _tuple15*f1;};char Cyc_Rgnorder_tok[
17]="\000\000\000\000Rgnorder_tok\000";struct Cyc_Rgnorder_tok_struct{char*tag;
struct Cyc_List_List*f1;};char Cyc_Scope_tok[14]="\000\000\000\000Scope_tok\000";
struct Cyc_Scope_tok_struct{char*tag;void*f1;};char Cyc_Short_tok[14]="\000\000\000\000Short_tok\000";
struct Cyc_Short_tok_struct{char*tag;short f1;};char Cyc_Stmt_tok[13]="\000\000\000\000Stmt_tok\000";
struct Cyc_Stmt_tok_struct{char*tag;struct Cyc_Absyn_Stmt*f1;};char Cyc_StorageClass_tok[
21]="\000\000\000\000StorageClass_tok\000";struct Cyc_StorageClass_tok_struct{
char*tag;void*f1;};char Cyc_String_tok[15]="\000\000\000\000String_tok\000";
struct Cyc_String_tok_struct{char*tag;struct _dynforward_ptr f1;};char Cyc_Stringopt_tok[
18]="\000\000\000\000Stringopt_tok\000";struct Cyc_Stringopt_tok_struct{char*tag;
struct Cyc_Core_Opt*f1;};char Cyc_StructOrUnion_tok[22]="\000\000\000\000StructOrUnion_tok\000";
struct Cyc_StructOrUnion_tok_struct{char*tag;void*f1;};char Cyc_SwitchClauseList_tok[
25]="\000\000\000\000SwitchClauseList_tok\000";struct Cyc_SwitchClauseList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_TunionFieldList_tok[24]="\000\000\000\000TunionFieldList_tok\000";
struct Cyc_TunionFieldList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_TunionField_tok[
20]="\000\000\000\000TunionField_tok\000";struct Cyc_TunionField_tok_struct{char*
tag;struct Cyc_Absyn_Tunionfield*f1;};char Cyc_TypeList_tok[17]="\000\000\000\000TypeList_tok\000";
struct Cyc_TypeList_tok_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_TypeModifierList_tok[
25]="\000\000\000\000TypeModifierList_tok\000";struct Cyc_TypeModifierList_tok_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_TypeOpt_tok[16]="\000\000\000\000TypeOpt_tok\000";
struct Cyc_TypeOpt_tok_struct{char*tag;struct Cyc_Core_Opt*f1;};char Cyc_TypeQual_tok[
17]="\000\000\000\000TypeQual_tok\000";struct Cyc_TypeQual_tok_struct{char*tag;
struct Cyc_Absyn_Tqual f1;};char Cyc_TypeSpecifier_tok[22]="\000\000\000\000TypeSpecifier_tok\000";
struct Cyc_TypeSpecifier_tok_struct{char*tag;void*f1;};char Cyc_Type_tok[13]="\000\000\000\000Type_tok\000";
struct Cyc_Type_tok_struct{char*tag;void*f1;};char Cyc_YY1[8]="\000\000\000\000YY1\000";
struct _tuple16{struct Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Absyn_Conref*
f3;};struct Cyc_YY1_struct{char*tag;struct _tuple16*f1;};char Cyc_YY2[8]="\000\000\000\000YY2\000";
struct Cyc_YY2_struct{char*tag;struct Cyc_Absyn_Conref*f1;};char Cyc_YY3[8]="\000\000\000\000YY3\000";
struct _tuple17{struct Cyc_List_List*f1;int f2;};struct Cyc_YY3_struct{char*tag;
struct _tuple17*f1;};char Cyc_YY4[8]="\000\000\000\000YY4\000";struct Cyc_YY4_struct{
char*tag;struct _tuple17*f1;};char Cyc_YY5[8]="\000\000\000\000YY5\000";struct Cyc_YY5_struct{
char*tag;struct _tuple14*f1;};char Cyc_YY6[8]="\000\000\000\000YY6\000";struct Cyc_YY6_struct{
char*tag;struct Cyc_Absyn_Conref*f1;};char Cyc_YY7[8]="\000\000\000\000YY7\000";
struct Cyc_YY7_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_YYINITIALSVAL[18]="\000\000\000\000YYINITIALSVAL\000";
char Cyc_Okay_tok[13]="\000\000\000\000Okay_tok\000";static char _tmp188[15]="$(sign_t,int)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp188,
_tmp188 + 15}};struct _tuple13*Cyc_yyget_Int_tok(void*yy1){struct _tuple13*yyzzz;{
void*_tmp189=yy1;struct _tuple13*_tmp18A;_LLCF: if(*((void**)_tmp189)!= Cyc_Int_tok)
goto _LLD1;_tmp18A=((struct Cyc_Int_tok_struct*)_tmp189)->f1;_LLD0: yyzzz=_tmp18A;
goto _LLCE;_LLD1:;_LLD2:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLCE:;}return
yyzzz;}static char _tmp18C[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{_tmp18C,_tmp18C + 5}};char Cyc_yyget_Char_tok(void*yy1){char
yyzzz;{void*_tmp18D=yy1;char _tmp18E;_LLD4: if(*((void**)_tmp18D)!= Cyc_Char_tok)
goto _LLD6;_tmp18E=((struct Cyc_Char_tok_struct*)_tmp18D)->f1;_LLD5: yyzzz=_tmp18E;
goto _LLD3;_LLD6:;_LLD7:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLD3:;}return
yyzzz;}static char _tmp190[9]="string_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp190,_tmp190 + 9}};struct _dynforward_ptr Cyc_yyget_String_tok(
void*yy1){struct _dynforward_ptr yyzzz;{void*_tmp191=yy1;struct _dynforward_ptr
_tmp192;_LLD9: if(*((void**)_tmp191)!= Cyc_String_tok)goto _LLDB;_tmp192=((struct
Cyc_String_tok_struct*)_tmp191)->f1;_LLDA: yyzzz=_tmp192;goto _LLD8;_LLDB:;_LLDC:(
int)_throw((void*)& Cyc_yyfail_String_tok);_LLD8:;}return yyzzz;}static char _tmp194[
54]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp194,_tmp194 + 54}};struct _tuple16*Cyc_yyget_YY1(
void*yy1){struct _tuple16*yyzzz;{void*_tmp195=yy1;struct _tuple16*_tmp196;_LLDE:
if(*((void**)_tmp195)!= Cyc_YY1)goto _LLE0;_tmp196=((struct Cyc_YY1_struct*)
_tmp195)->f1;_LLDF: yyzzz=_tmp196;goto _LLDD;_LLE0:;_LLE1:(int)_throw((void*)& Cyc_yyfail_YY1);
_LLDD:;}return yyzzz;}static char _tmp198[19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp198,_tmp198 + 19}};struct Cyc_Absyn_Conref*
Cyc_yyget_YY2(void*yy1){struct Cyc_Absyn_Conref*yyzzz;{void*_tmp199=yy1;struct Cyc_Absyn_Conref*
_tmp19A;_LLE3: if(*((void**)_tmp199)!= Cyc_YY2)goto _LLE5;_tmp19A=((struct Cyc_YY2_struct*)
_tmp199)->f1;_LLE4: yyzzz=_tmp19A;goto _LLE2;_LLE5:;_LLE6:(int)_throw((void*)& Cyc_yyfail_YY2);
_LLE2:;}return yyzzz;}static char _tmp19C[6]="exp_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp19C,_tmp19C + 6}};struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok(void*yy1){struct Cyc_Absyn_Exp*yyzzz;{void*_tmp19D=yy1;struct
Cyc_Absyn_Exp*_tmp19E;_LLE8: if(*((void**)_tmp19D)!= Cyc_Exp_tok)goto _LLEA;
_tmp19E=((struct Cyc_Exp_tok_struct*)_tmp19D)->f1;_LLE9: yyzzz=_tmp19E;goto _LLE7;
_LLEA:;_LLEB:(int)_throw((void*)& Cyc_yyfail_Exp_tok);_LLE7:;}return yyzzz;}static
char _tmp1A0[14]="list_t<exp_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure,{_tmp1A0,_tmp1A0 + 14}};struct Cyc_List_List*Cyc_yyget_ExpList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1A1=yy1;struct Cyc_List_List*
_tmp1A2;_LLED: if(*((void**)_tmp1A1)!= Cyc_ExpList_tok)goto _LLEF;_tmp1A2=((struct
Cyc_ExpList_tok_struct*)_tmp1A1)->f1;_LLEE: yyzzz=_tmp1A2;goto _LLEC;_LLEF:;_LLF0:(
int)_throw((void*)& Cyc_yyfail_ExpList_tok);_LLEC:;}return yyzzz;}static char
_tmp1A4[39]="list_t<$(list_t<designator_t>,exp_t)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitializerList_tok={Cyc_Core_Failure,{_tmp1A4,_tmp1A4 + 39}};struct
Cyc_List_List*Cyc_yyget_InitializerList_tok(void*yy1){struct Cyc_List_List*yyzzz;{
void*_tmp1A5=yy1;struct Cyc_List_List*_tmp1A6;_LLF2: if(*((void**)_tmp1A5)!= Cyc_InitializerList_tok)
goto _LLF4;_tmp1A6=((struct Cyc_InitializerList_tok_struct*)_tmp1A5)->f1;_LLF3:
yyzzz=_tmp1A6;goto _LLF1;_LLF4:;_LLF5:(int)_throw((void*)& Cyc_yyfail_InitializerList_tok);
_LLF1:;}return yyzzz;}static char _tmp1A8[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={Cyc_Core_Failure,{_tmp1A8,_tmp1A8 + 9}};void*Cyc_yyget_Primop_tok(
void*yy1){void*yyzzz;{void*_tmp1A9=yy1;void*_tmp1AA;_LLF7: if(*((void**)_tmp1A9)
!= Cyc_Primop_tok)goto _LLF9;_tmp1AA=(void*)((struct Cyc_Primop_tok_struct*)
_tmp1A9)->f1;_LLF8: yyzzz=_tmp1AA;goto _LLF6;_LLF9:;_LLFA:(int)_throw((void*)& Cyc_yyfail_Primop_tok);
_LLF6:;}return yyzzz;}static char _tmp1AC[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={Cyc_Core_Failure,{_tmp1AC,_tmp1AC + 16}};struct Cyc_Core_Opt*
Cyc_yyget_Primopopt_tok(void*yy1){struct Cyc_Core_Opt*yyzzz;{void*_tmp1AD=yy1;
struct Cyc_Core_Opt*_tmp1AE;_LLFC: if(*((void**)_tmp1AD)!= Cyc_Primopopt_tok)goto
_LLFE;_tmp1AE=((struct Cyc_Primopopt_tok_struct*)_tmp1AD)->f1;_LLFD: yyzzz=_tmp1AE;
goto _LLFB;_LLFE:;_LLFF:(int)_throw((void*)& Cyc_yyfail_Primopopt_tok);_LLFB:;}
return yyzzz;}static char _tmp1B0[7]="qvar_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp1B0,_tmp1B0 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(
void*yy1){struct _tuple1*yyzzz;{void*_tmp1B1=yy1;struct _tuple1*_tmp1B2;_LL101: if(*((
void**)_tmp1B1)!= Cyc_QualId_tok)goto _LL103;_tmp1B2=((struct Cyc_QualId_tok_struct*)
_tmp1B1)->f1;_LL102: yyzzz=_tmp1B2;goto _LL100;_LL103:;_LL104:(int)_throw((void*)&
Cyc_yyfail_QualId_tok);_LL100:;}return yyzzz;}static char _tmp1B4[7]="stmt_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={Cyc_Core_Failure,{
_tmp1B4,_tmp1B4 + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(void*yy1){struct Cyc_Absyn_Stmt*
yyzzz;{void*_tmp1B5=yy1;struct Cyc_Absyn_Stmt*_tmp1B6;_LL106: if(*((void**)_tmp1B5)
!= Cyc_Stmt_tok)goto _LL108;_tmp1B6=((struct Cyc_Stmt_tok_struct*)_tmp1B5)->f1;
_LL107: yyzzz=_tmp1B6;goto _LL105;_LL108:;_LL109:(int)_throw((void*)& Cyc_yyfail_Stmt_tok);
_LL105:;}return yyzzz;}static char _tmp1B8[24]="list_t<switch_clause_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={Cyc_Core_Failure,{
_tmp1B8,_tmp1B8 + 24}};struct Cyc_List_List*Cyc_yyget_SwitchClauseList_tok(void*
yy1){struct Cyc_List_List*yyzzz;{void*_tmp1B9=yy1;struct Cyc_List_List*_tmp1BA;
_LL10B: if(*((void**)_tmp1B9)!= Cyc_SwitchClauseList_tok)goto _LL10D;_tmp1BA=((
struct Cyc_SwitchClauseList_tok_struct*)_tmp1B9)->f1;_LL10C: yyzzz=_tmp1BA;goto
_LL10A;_LL10D:;_LL10E:(int)_throw((void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL10A:;}return yyzzz;}static char _tmp1BC[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={Cyc_Core_Failure,{_tmp1BC,_tmp1BC + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_Pattern_tok(void*yy1){struct Cyc_Absyn_Pat*yyzzz;{void*_tmp1BD=yy1;
struct Cyc_Absyn_Pat*_tmp1BE;_LL110: if(*((void**)_tmp1BD)!= Cyc_Pattern_tok)goto
_LL112;_tmp1BE=((struct Cyc_Pattern_tok_struct*)_tmp1BD)->f1;_LL111: yyzzz=_tmp1BE;
goto _LL10F;_LL112:;_LL113:(int)_throw((void*)& Cyc_yyfail_Pattern_tok);_LL10F:;}
return yyzzz;}static char _tmp1C0[23]="$(list_t<pat_t>,bool)@";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp1C0,_tmp1C0 + 23}};struct _tuple17*Cyc_yyget_YY3(
void*yy1){struct _tuple17*yyzzz;{void*_tmp1C1=yy1;struct _tuple17*_tmp1C2;_LL115:
if(*((void**)_tmp1C1)!= Cyc_YY3)goto _LL117;_tmp1C2=((struct Cyc_YY3_struct*)
_tmp1C1)->f1;_LL116: yyzzz=_tmp1C2;goto _LL114;_LL117:;_LL118:(int)_throw((void*)&
Cyc_yyfail_YY3);_LL114:;}return yyzzz;}static char _tmp1C4[14]="list_t<pat_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_PatternList_tok={Cyc_Core_Failure,{
_tmp1C4,_tmp1C4 + 14}};struct Cyc_List_List*Cyc_yyget_PatternList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp1C5=yy1;struct Cyc_List_List*_tmp1C6;_LL11A:
if(*((void**)_tmp1C5)!= Cyc_PatternList_tok)goto _LL11C;_tmp1C6=((struct Cyc_PatternList_tok_struct*)
_tmp1C5)->f1;_LL11B: yyzzz=_tmp1C6;goto _LL119;_LL11C:;_LL11D:(int)_throw((void*)&
Cyc_yyfail_PatternList_tok);_LL119:;}return yyzzz;}static char _tmp1C8[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={Cyc_Core_Failure,{
_tmp1C8,_tmp1C8 + 31}};struct _tuple11*Cyc_yyget_FieldPattern_tok(void*yy1){struct
_tuple11*yyzzz;{void*_tmp1C9=yy1;struct _tuple11*_tmp1CA;_LL11F: if(*((void**)
_tmp1C9)!= Cyc_FieldPattern_tok)goto _LL121;_tmp1CA=((struct Cyc_FieldPattern_tok_struct*)
_tmp1C9)->f1;_LL120: yyzzz=_tmp1CA;goto _LL11E;_LL121:;_LL122:(int)_throw((void*)&
Cyc_yyfail_FieldPattern_tok);_LL11E:;}return yyzzz;}static char _tmp1CC[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={Cyc_Core_Failure,{
_tmp1CC,_tmp1CC + 39}};struct Cyc_List_List*Cyc_yyget_FieldPatternList_tok(void*
yy1){struct Cyc_List_List*yyzzz;{void*_tmp1CD=yy1;struct Cyc_List_List*_tmp1CE;
_LL124: if(*((void**)_tmp1CD)!= Cyc_FieldPatternList_tok)goto _LL126;_tmp1CE=((
struct Cyc_FieldPatternList_tok_struct*)_tmp1CD)->f1;_LL125: yyzzz=_tmp1CE;goto
_LL123;_LL126:;_LL127:(int)_throw((void*)& Cyc_yyfail_FieldPatternList_tok);
_LL123:;}return yyzzz;}static char _tmp1D0[48]="$(list_t<$(list_t<designator_t>,pat_t)@>,bool)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1D0,
_tmp1D0 + 48}};struct _tuple17*Cyc_yyget_YY4(void*yy1){struct _tuple17*yyzzz;{void*
_tmp1D1=yy1;struct _tuple17*_tmp1D2;_LL129: if(*((void**)_tmp1D1)!= Cyc_YY4)goto
_LL12B;_tmp1D2=((struct Cyc_YY4_struct*)_tmp1D1)->f1;_LL12A: yyzzz=_tmp1D2;goto
_LL128;_LL12B:;_LL12C:(int)_throw((void*)& Cyc_yyfail_YY4);_LL128:;}return yyzzz;}
static char _tmp1D4[9]="fndecl_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_FnDecl_tok={
Cyc_Core_Failure,{_tmp1D4,_tmp1D4 + 9}};struct Cyc_Absyn_Fndecl*Cyc_yyget_FnDecl_tok(
void*yy1){struct Cyc_Absyn_Fndecl*yyzzz;{void*_tmp1D5=yy1;struct Cyc_Absyn_Fndecl*
_tmp1D6;_LL12E: if(*((void**)_tmp1D5)!= Cyc_FnDecl_tok)goto _LL130;_tmp1D6=((
struct Cyc_FnDecl_tok_struct*)_tmp1D5)->f1;_LL12F: yyzzz=_tmp1D6;goto _LL12D;_LL130:;
_LL131:(int)_throw((void*)& Cyc_yyfail_FnDecl_tok);_LL12D:;}return yyzzz;}static
char _tmp1D8[15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={
Cyc_Core_Failure,{_tmp1D8,_tmp1D8 + 15}};struct Cyc_List_List*Cyc_yyget_DeclList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1D9=yy1;struct Cyc_List_List*
_tmp1DA;_LL133: if(*((void**)_tmp1D9)!= Cyc_DeclList_tok)goto _LL135;_tmp1DA=((
struct Cyc_DeclList_tok_struct*)_tmp1D9)->f1;_LL134: yyzzz=_tmp1DA;goto _LL132;
_LL135:;_LL136:(int)_throw((void*)& Cyc_yyfail_DeclList_tok);_LL132:;}return yyzzz;}
static char _tmp1DC[12]="decl_spec_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclSpec_tok={
Cyc_Core_Failure,{_tmp1DC,_tmp1DC + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_DeclSpec_tok(
void*yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{void*_tmp1DD=yy1;struct Cyc_Parse_Declaration_spec*
_tmp1DE;_LL138: if(*((void**)_tmp1DD)!= Cyc_DeclSpec_tok)goto _LL13A;_tmp1DE=((
struct Cyc_DeclSpec_tok_struct*)_tmp1DD)->f1;_LL139: yyzzz=_tmp1DE;goto _LL137;
_LL13A:;_LL13B:(int)_throw((void*)& Cyc_yyfail_DeclSpec_tok);_LL137:;}return yyzzz;}
static char _tmp1E0[27]="$(declarator_t,exp_opt_t)@";static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={Cyc_Core_Failure,{_tmp1E0,_tmp1E0 + 27}};struct _tuple12*
Cyc_yyget_InitDecl_tok(void*yy1){struct _tuple12*yyzzz;{void*_tmp1E1=yy1;struct
_tuple12*_tmp1E2;_LL13D: if(*((void**)_tmp1E1)!= Cyc_InitDecl_tok)goto _LL13F;
_tmp1E2=((struct Cyc_InitDecl_tok_struct*)_tmp1E1)->f1;_LL13E: yyzzz=_tmp1E2;goto
_LL13C;_LL13F:;_LL140:(int)_throw((void*)& Cyc_yyfail_InitDecl_tok);_LL13C:;}
return yyzzz;}static char _tmp1E4[35]="list_t<$(declarator_t,exp_opt_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={Cyc_Core_Failure,{
_tmp1E4,_tmp1E4 + 35}};struct Cyc_List_List*Cyc_yyget_InitDeclList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp1E5=yy1;struct Cyc_List_List*_tmp1E6;_LL142:
if(*((void**)_tmp1E5)!= Cyc_InitDeclList_tok)goto _LL144;_tmp1E6=((struct Cyc_InitDeclList_tok_struct*)
_tmp1E5)->f1;_LL143: yyzzz=_tmp1E6;goto _LL141;_LL144:;_LL145:(int)_throw((void*)&
Cyc_yyfail_InitDeclList_tok);_LL141:;}return yyzzz;}static char _tmp1E8[16]="storage_class_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StorageClass_tok={Cyc_Core_Failure,{
_tmp1E8,_tmp1E8 + 16}};void*Cyc_yyget_StorageClass_tok(void*yy1){void*yyzzz;{void*
_tmp1E9=yy1;void*_tmp1EA;_LL147: if(*((void**)_tmp1E9)!= Cyc_StorageClass_tok)
goto _LL149;_tmp1EA=(void*)((struct Cyc_StorageClass_tok_struct*)_tmp1E9)->f1;
_LL148: yyzzz=_tmp1EA;goto _LL146;_LL149:;_LL14A:(int)_throw((void*)& Cyc_yyfail_StorageClass_tok);
_LL146:;}return yyzzz;}static char _tmp1EC[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={Cyc_Core_Failure,{_tmp1EC,_tmp1EC + 17}};void*Cyc_yyget_TypeSpecifier_tok(
void*yy1){void*yyzzz;{void*_tmp1ED=yy1;void*_tmp1EE;_LL14C: if(*((void**)_tmp1ED)
!= Cyc_TypeSpecifier_tok)goto _LL14E;_tmp1EE=(void*)((struct Cyc_TypeSpecifier_tok_struct*)
_tmp1ED)->f1;_LL14D: yyzzz=_tmp1EE;goto _LL14B;_LL14E:;_LL14F:(int)_throw((void*)&
Cyc_yyfail_TypeSpecifier_tok);_LL14B:;}return yyzzz;}static char _tmp1F0[12]="aggr_kind_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={Cyc_Core_Failure,{
_tmp1F0,_tmp1F0 + 12}};void*Cyc_yyget_StructOrUnion_tok(void*yy1){void*yyzzz;{
void*_tmp1F1=yy1;void*_tmp1F2;_LL151: if(*((void**)_tmp1F1)!= Cyc_StructOrUnion_tok)
goto _LL153;_tmp1F2=(void*)((struct Cyc_StructOrUnion_tok_struct*)_tmp1F1)->f1;
_LL152: yyzzz=_tmp1F2;goto _LL150;_LL153:;_LL154:(int)_throw((void*)& Cyc_yyfail_StructOrUnion_tok);
_LL150:;}return yyzzz;}static char _tmp1F4[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={Cyc_Core_Failure,{_tmp1F4,_tmp1F4 + 8}};struct Cyc_Absyn_Tqual
Cyc_yyget_TypeQual_tok(void*yy1){struct Cyc_Absyn_Tqual yyzzz;{void*_tmp1F5=yy1;
struct Cyc_Absyn_Tqual _tmp1F6;_LL156: if(*((void**)_tmp1F5)!= Cyc_TypeQual_tok)
goto _LL158;_tmp1F6=((struct Cyc_TypeQual_tok_struct*)_tmp1F5)->f1;_LL157: yyzzz=
_tmp1F6;goto _LL155;_LL158:;_LL159:(int)_throw((void*)& Cyc_yyfail_TypeQual_tok);
_LL155:;}return yyzzz;}static char _tmp1F8[20]="list_t<aggrfield_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclList_tok={Cyc_Core_Failure,{
_tmp1F8,_tmp1F8 + 20}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclList_tok(void*
yy1){struct Cyc_List_List*yyzzz;{void*_tmp1F9=yy1;struct Cyc_List_List*_tmp1FA;
_LL15B: if(*((void**)_tmp1F9)!= Cyc_AggrFieldDeclList_tok)goto _LL15D;_tmp1FA=((
struct Cyc_AggrFieldDeclList_tok_struct*)_tmp1F9)->f1;_LL15C: yyzzz=_tmp1FA;goto
_LL15A;_LL15D:;_LL15E:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclList_tok);
_LL15A:;}return yyzzz;}static char _tmp1FC[28]="list_t<list_t<aggrfield_t>>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_AggrFieldDeclListList_tok={Cyc_Core_Failure,{
_tmp1FC,_tmp1FC + 28}};struct Cyc_List_List*Cyc_yyget_AggrFieldDeclListList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp1FD=yy1;struct Cyc_List_List*
_tmp1FE;_LL160: if(*((void**)_tmp1FD)!= Cyc_AggrFieldDeclListList_tok)goto _LL162;
_tmp1FE=((struct Cyc_AggrFieldDeclListList_tok_struct*)_tmp1FD)->f1;_LL161: yyzzz=
_tmp1FE;goto _LL15F;_LL162:;_LL163:(int)_throw((void*)& Cyc_yyfail_AggrFieldDeclListList_tok);
_LL15F:;}return yyzzz;}static char _tmp200[24]="list_t<type_modifier_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={Cyc_Core_Failure,{
_tmp200,_tmp200 + 24}};struct Cyc_List_List*Cyc_yyget_TypeModifierList_tok(void*
yy1){struct Cyc_List_List*yyzzz;{void*_tmp201=yy1;struct Cyc_List_List*_tmp202;
_LL165: if(*((void**)_tmp201)!= Cyc_TypeModifierList_tok)goto _LL167;_tmp202=((
struct Cyc_TypeModifierList_tok_struct*)_tmp201)->f1;_LL166: yyzzz=_tmp202;goto
_LL164;_LL167:;_LL168:(int)_throw((void*)& Cyc_yyfail_TypeModifierList_tok);
_LL164:;}return yyzzz;}static char _tmp204[13]="declarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Declarator_tok={Cyc_Core_Failure,{_tmp204,_tmp204 + 13}};struct Cyc_Parse_Declarator*
Cyc_yyget_Declarator_tok(void*yy1){struct Cyc_Parse_Declarator*yyzzz;{void*
_tmp205=yy1;struct Cyc_Parse_Declarator*_tmp206;_LL16A: if(*((void**)_tmp205)!= 
Cyc_Declarator_tok)goto _LL16C;_tmp206=((struct Cyc_Declarator_tok_struct*)_tmp205)->f1;
_LL16B: yyzzz=_tmp206;goto _LL169;_LL16C:;_LL16D:(int)_throw((void*)& Cyc_yyfail_Declarator_tok);
_LL169:;}return yyzzz;}static char _tmp208[21]="abstractdeclarator_t";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={Cyc_Core_Failure,{
_tmp208,_tmp208 + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_AbstractDeclarator_tok(
void*yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{void*_tmp209=yy1;struct Cyc_Parse_Abstractdeclarator*
_tmp20A;_LL16F: if(*((void**)_tmp209)!= Cyc_AbstractDeclarator_tok)goto _LL171;
_tmp20A=((struct Cyc_AbstractDeclarator_tok_struct*)_tmp209)->f1;_LL170: yyzzz=
_tmp20A;goto _LL16E;_LL171:;_LL172:(int)_throw((void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL16E:;}return yyzzz;}static char _tmp20C[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={Cyc_Core_Failure,{_tmp20C,_tmp20C + 5}};int Cyc_yyget_Bool_tok(
void*yy1){int yyzzz;{void*_tmp20D=yy1;int _tmp20E;_LL174: if(*((void**)_tmp20D)!= 
Cyc_Bool_tok)goto _LL176;_tmp20E=((struct Cyc_Bool_tok_struct*)_tmp20D)->f1;_LL175:
yyzzz=_tmp20E;goto _LL173;_LL176:;_LL177:(int)_throw((void*)& Cyc_yyfail_Bool_tok);
_LL173:;}return yyzzz;}static char _tmp210[8]="scope_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={Cyc_Core_Failure,{_tmp210,_tmp210 + 8}};void*Cyc_yyget_Scope_tok(
void*yy1){void*yyzzz;{void*_tmp211=yy1;void*_tmp212;_LL179: if(*((void**)_tmp211)
!= Cyc_Scope_tok)goto _LL17B;_tmp212=(void*)((struct Cyc_Scope_tok_struct*)_tmp211)->f1;
_LL17A: yyzzz=_tmp212;goto _LL178;_LL17B:;_LL17C:(int)_throw((void*)& Cyc_yyfail_Scope_tok);
_LL178:;}return yyzzz;}static char _tmp214[14]="tunionfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionField_tok={Cyc_Core_Failure,{_tmp214,_tmp214 + 14}};struct Cyc_Absyn_Tunionfield*
Cyc_yyget_TunionField_tok(void*yy1){struct Cyc_Absyn_Tunionfield*yyzzz;{void*
_tmp215=yy1;struct Cyc_Absyn_Tunionfield*_tmp216;_LL17E: if(*((void**)_tmp215)!= 
Cyc_TunionField_tok)goto _LL180;_tmp216=((struct Cyc_TunionField_tok_struct*)
_tmp215)->f1;_LL17F: yyzzz=_tmp216;goto _LL17D;_LL180:;_LL181:(int)_throw((void*)&
Cyc_yyfail_TunionField_tok);_LL17D:;}return yyzzz;}static char _tmp218[22]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={Cyc_Core_Failure,{
_tmp218,_tmp218 + 22}};struct Cyc_List_List*Cyc_yyget_TunionFieldList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp219=yy1;struct Cyc_List_List*_tmp21A;_LL183:
if(*((void**)_tmp219)!= Cyc_TunionFieldList_tok)goto _LL185;_tmp21A=((struct Cyc_TunionFieldList_tok_struct*)
_tmp219)->f1;_LL184: yyzzz=_tmp21A;goto _LL182;_LL185:;_LL186:(int)_throw((void*)&
Cyc_yyfail_TunionFieldList_tok);_LL182:;}return yyzzz;}static char _tmp21C[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={Cyc_Core_Failure,{
_tmp21C,_tmp21C + 50}};struct _tuple15*Cyc_yyget_QualSpecList_tok(void*yy1){struct
_tuple15*yyzzz;{void*_tmp21D=yy1;struct _tuple15*_tmp21E;_LL188: if(*((void**)
_tmp21D)!= Cyc_QualSpecList_tok)goto _LL18A;_tmp21E=((struct Cyc_QualSpecList_tok_struct*)
_tmp21D)->f1;_LL189: yyzzz=_tmp21E;goto _LL187;_LL18A:;_LL18B:(int)_throw((void*)&
Cyc_yyfail_QualSpecList_tok);_LL187:;}return yyzzz;}static char _tmp220[14]="list_t<var_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={Cyc_Core_Failure,{
_tmp220,_tmp220 + 14}};struct Cyc_List_List*Cyc_yyget_IdList_tok(void*yy1){struct
Cyc_List_List*yyzzz;{void*_tmp221=yy1;struct Cyc_List_List*_tmp222;_LL18D: if(*((
void**)_tmp221)!= Cyc_IdList_tok)goto _LL18F;_tmp222=((struct Cyc_IdList_tok_struct*)
_tmp221)->f1;_LL18E: yyzzz=_tmp222;goto _LL18C;_LL18F:;_LL190:(int)_throw((void*)&
Cyc_yyfail_IdList_tok);_LL18C:;}return yyzzz;}static char _tmp224[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={Cyc_Core_Failure,{
_tmp224,_tmp224 + 32}};struct _tuple2*Cyc_yyget_ParamDecl_tok(void*yy1){struct
_tuple2*yyzzz;{void*_tmp225=yy1;struct _tuple2*_tmp226;_LL192: if(*((void**)
_tmp225)!= Cyc_ParamDecl_tok)goto _LL194;_tmp226=((struct Cyc_ParamDecl_tok_struct*)
_tmp225)->f1;_LL193: yyzzz=_tmp226;goto _LL191;_LL194:;_LL195:(int)_throw((void*)&
Cyc_yyfail_ParamDecl_tok);_LL191:;}return yyzzz;}static char _tmp228[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={Cyc_Core_Failure,{
_tmp228,_tmp228 + 40}};struct Cyc_List_List*Cyc_yyget_ParamDeclList_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp229=yy1;struct Cyc_List_List*_tmp22A;_LL197:
if(*((void**)_tmp229)!= Cyc_ParamDeclList_tok)goto _LL199;_tmp22A=((struct Cyc_ParamDeclList_tok_struct*)
_tmp229)->f1;_LL198: yyzzz=_tmp22A;goto _LL196;_LL199:;_LL19A:(int)_throw((void*)&
Cyc_yyfail_ParamDeclList_tok);_LL196:;}return yyzzz;}static char _tmp22C[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp22C,
_tmp22C + 107}};struct _tuple14*Cyc_yyget_YY5(void*yy1){struct _tuple14*yyzzz;{void*
_tmp22D=yy1;struct _tuple14*_tmp22E;_LL19C: if(*((void**)_tmp22D)!= Cyc_YY5)goto
_LL19E;_tmp22E=((struct Cyc_YY5_struct*)_tmp22D)->f1;_LL19D: yyzzz=_tmp22E;goto
_LL19B;_LL19E:;_LL19F:(int)_throw((void*)& Cyc_yyfail_YY5);_LL19B:;}return yyzzz;}
static char _tmp230[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeList_tok={
Cyc_Core_Failure,{_tmp230,_tmp230 + 15}};struct Cyc_List_List*Cyc_yyget_TypeList_tok(
void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp231=yy1;struct Cyc_List_List*
_tmp232;_LL1A1: if(*((void**)_tmp231)!= Cyc_TypeList_tok)goto _LL1A3;_tmp232=((
struct Cyc_TypeList_tok_struct*)_tmp231)->f1;_LL1A2: yyzzz=_tmp232;goto _LL1A0;
_LL1A3:;_LL1A4:(int)_throw((void*)& Cyc_yyfail_TypeList_tok);_LL1A0:;}return yyzzz;}
static char _tmp234[21]="list_t<designator_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_DesignatorList_tok={Cyc_Core_Failure,{_tmp234,_tmp234 + 21}};struct Cyc_List_List*
Cyc_yyget_DesignatorList_tok(void*yy1){struct Cyc_List_List*yyzzz;{void*_tmp235=
yy1;struct Cyc_List_List*_tmp236;_LL1A6: if(*((void**)_tmp235)!= Cyc_DesignatorList_tok)
goto _LL1A8;_tmp236=((struct Cyc_DesignatorList_tok_struct*)_tmp235)->f1;_LL1A7:
yyzzz=_tmp236;goto _LL1A5;_LL1A8:;_LL1A9:(int)_throw((void*)& Cyc_yyfail_DesignatorList_tok);
_LL1A5:;}return yyzzz;}static char _tmp238[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Designator_tok={Cyc_Core_Failure,{_tmp238,_tmp238 + 13}};void*Cyc_yyget_Designator_tok(
void*yy1){void*yyzzz;{void*_tmp239=yy1;void*_tmp23A;_LL1AB: if(*((void**)_tmp239)
!= Cyc_Designator_tok)goto _LL1AD;_tmp23A=(void*)((struct Cyc_Designator_tok_struct*)
_tmp239)->f1;_LL1AC: yyzzz=_tmp23A;goto _LL1AA;_LL1AD:;_LL1AE:(int)_throw((void*)&
Cyc_yyfail_Designator_tok);_LL1AA:;}return yyzzz;}static char _tmp23C[7]="kind_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Kind_tok={Cyc_Core_Failure,{
_tmp23C,_tmp23C + 7}};void*Cyc_yyget_Kind_tok(void*yy1){void*yyzzz;{void*_tmp23D=
yy1;void*_tmp23E;_LL1B0: if(*((void**)_tmp23D)!= Cyc_Kind_tok)goto _LL1B2;_tmp23E=(
void*)((struct Cyc_Kind_tok_struct*)_tmp23D)->f1;_LL1B1: yyzzz=_tmp23E;goto _LL1AF;
_LL1B2:;_LL1B3:(int)_throw((void*)& Cyc_yyfail_Kind_tok);_LL1AF:;}return yyzzz;}
static char _tmp240[7]="type_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={
Cyc_Core_Failure,{_tmp240,_tmp240 + 7}};void*Cyc_yyget_Type_tok(void*yy1){void*
yyzzz;{void*_tmp241=yy1;void*_tmp242;_LL1B5: if(*((void**)_tmp241)!= Cyc_Type_tok)
goto _LL1B7;_tmp242=(void*)((struct Cyc_Type_tok_struct*)_tmp241)->f1;_LL1B6: yyzzz=
_tmp242;goto _LL1B4;_LL1B7:;_LL1B8:(int)_throw((void*)& Cyc_yyfail_Type_tok);
_LL1B4:;}return yyzzz;}static char _tmp244[20]="list_t<attribute_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={Cyc_Core_Failure,{_tmp244,
_tmp244 + 20}};struct Cyc_List_List*Cyc_yyget_AttributeList_tok(void*yy1){struct
Cyc_List_List*yyzzz;{void*_tmp245=yy1;struct Cyc_List_List*_tmp246;_LL1BA: if(*((
void**)_tmp245)!= Cyc_AttributeList_tok)goto _LL1BC;_tmp246=((struct Cyc_AttributeList_tok_struct*)
_tmp245)->f1;_LL1BB: yyzzz=_tmp246;goto _LL1B9;_LL1BC:;_LL1BD:(int)_throw((void*)&
Cyc_yyfail_AttributeList_tok);_LL1B9:;}return yyzzz;}static char _tmp248[12]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={Cyc_Core_Failure,{
_tmp248,_tmp248 + 12}};void*Cyc_yyget_Attribute_tok(void*yy1){void*yyzzz;{void*
_tmp249=yy1;void*_tmp24A;_LL1BF: if(*((void**)_tmp249)!= Cyc_Attribute_tok)goto
_LL1C1;_tmp24A=(void*)((struct Cyc_Attribute_tok_struct*)_tmp249)->f1;_LL1C0:
yyzzz=_tmp24A;goto _LL1BE;_LL1C1:;_LL1C2:(int)_throw((void*)& Cyc_yyfail_Attribute_tok);
_LL1BE:;}return yyzzz;}static char _tmp24C[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={Cyc_Core_Failure,{_tmp24C,_tmp24C + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_Enumfield_tok(void*yy1){struct Cyc_Absyn_Enumfield*yyzzz;{void*_tmp24D=
yy1;struct Cyc_Absyn_Enumfield*_tmp24E;_LL1C4: if(*((void**)_tmp24D)!= Cyc_Enumfield_tok)
goto _LL1C6;_tmp24E=((struct Cyc_Enumfield_tok_struct*)_tmp24D)->f1;_LL1C5: yyzzz=
_tmp24E;goto _LL1C3;_LL1C6:;_LL1C7:(int)_throw((void*)& Cyc_yyfail_Enumfield_tok);
_LL1C3:;}return yyzzz;}static char _tmp250[20]="list_t<enumfield_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={Cyc_Core_Failure,{_tmp250,
_tmp250 + 20}};struct Cyc_List_List*Cyc_yyget_EnumfieldList_tok(void*yy1){struct
Cyc_List_List*yyzzz;{void*_tmp251=yy1;struct Cyc_List_List*_tmp252;_LL1C9: if(*((
void**)_tmp251)!= Cyc_EnumfieldList_tok)goto _LL1CB;_tmp252=((struct Cyc_EnumfieldList_tok_struct*)
_tmp251)->f1;_LL1CA: yyzzz=_tmp252;goto _LL1C8;_LL1CB:;_LL1CC:(int)_throw((void*)&
Cyc_yyfail_EnumfieldList_tok);_LL1C8:;}return yyzzz;}static char _tmp254[14]="opt_t<type_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeOpt_tok={Cyc_Core_Failure,{
_tmp254,_tmp254 + 14}};struct Cyc_Core_Opt*Cyc_yyget_TypeOpt_tok(void*yy1){struct
Cyc_Core_Opt*yyzzz;{void*_tmp255=yy1;struct Cyc_Core_Opt*_tmp256;_LL1CE: if(*((
void**)_tmp255)!= Cyc_TypeOpt_tok)goto _LL1D0;_tmp256=((struct Cyc_TypeOpt_tok_struct*)
_tmp255)->f1;_LL1CF: yyzzz=_tmp256;goto _LL1CD;_LL1D0:;_LL1D1:(int)_throw((void*)&
Cyc_yyfail_TypeOpt_tok);_LL1CD:;}return yyzzz;}static char _tmp258[26]="list_t<$(type_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={Cyc_Core_Failure,{
_tmp258,_tmp258 + 26}};struct Cyc_List_List*Cyc_yyget_Rgnorder_tok(void*yy1){
struct Cyc_List_List*yyzzz;{void*_tmp259=yy1;struct Cyc_List_List*_tmp25A;_LL1D3:
if(*((void**)_tmp259)!= Cyc_Rgnorder_tok)goto _LL1D5;_tmp25A=((struct Cyc_Rgnorder_tok_struct*)
_tmp259)->f1;_LL1D4: yyzzz=_tmp25A;goto _LL1D2;_LL1D5:;_LL1D6:(int)_throw((void*)&
Cyc_yyfail_Rgnorder_tok);_LL1D2:;}return yyzzz;}static char _tmp25C[15]="conref_t<bool>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp25C,
_tmp25C + 15}};struct Cyc_Absyn_Conref*Cyc_yyget_YY6(void*yy1){struct Cyc_Absyn_Conref*
yyzzz;{void*_tmp25D=yy1;struct Cyc_Absyn_Conref*_tmp25E;_LL1D8: if(*((void**)
_tmp25D)!= Cyc_YY6)goto _LL1DA;_tmp25E=((struct Cyc_YY6_struct*)_tmp25D)->f1;
_LL1D9: yyzzz=_tmp25E;goto _LL1D7;_LL1DA:;_LL1DB:(int)_throw((void*)& Cyc_yyfail_YY6);
_LL1D7:;}return yyzzz;}static char _tmp260[40]="list_t<$(Position::seg_t,qvar_t,bool)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp260,
_tmp260 + 40}};struct Cyc_List_List*Cyc_yyget_YY7(void*yy1){struct Cyc_List_List*
yyzzz;{void*_tmp261=yy1;struct Cyc_List_List*_tmp262;_LL1DD: if(*((void**)_tmp261)
!= Cyc_YY7)goto _LL1DF;_tmp262=((struct Cyc_YY7_struct*)_tmp261)->f1;_LL1DE: yyzzz=
_tmp262;goto _LL1DC;_LL1DF:;_LL1E0:(int)_throw((void*)& Cyc_yyfail_YY7);_LL1DC:;}
return yyzzz;}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int
last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){return({struct Cyc_Yyltype
_tmp263;_tmp263.timestamp=0;_tmp263.first_line=0;_tmp263.first_column=0;_tmp263.last_line=
0;_tmp263.last_column=0;_tmp263;});}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
static short Cyc_yytranslate[357]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,128,2,2,110,126,124,2,107,108,116,121,105,119,113,125,2,2,2,2,
2,2,2,2,2,2,112,102,111,106,120,118,117,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,114,2,115,123,109,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,103,122,104,127,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,
45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,
72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,
99,100,101};static char _tmp264[2]="$";static char _tmp265[6]="error";static char
_tmp266[12]="$undefined.";static char _tmp267[5]="AUTO";static char _tmp268[9]="REGISTER";
static char _tmp269[7]="STATIC";static char _tmp26A[7]="EXTERN";static char _tmp26B[8]="TYPEDEF";
static char _tmp26C[5]="VOID";static char _tmp26D[5]="CHAR";static char _tmp26E[6]="SHORT";
static char _tmp26F[4]="INT";static char _tmp270[5]="LONG";static char _tmp271[6]="FLOAT";
static char _tmp272[7]="DOUBLE";static char _tmp273[7]="SIGNED";static char _tmp274[9]="UNSIGNED";
static char _tmp275[6]="CONST";static char _tmp276[9]="VOLATILE";static char _tmp277[9]="RESTRICT";
static char _tmp278[7]="STRUCT";static char _tmp279[6]="UNION";static char _tmp27A[5]="CASE";
static char _tmp27B[8]="DEFAULT";static char _tmp27C[7]="INLINE";static char _tmp27D[7]="SIZEOF";
static char _tmp27E[9]="OFFSETOF";static char _tmp27F[3]="IF";static char _tmp280[5]="ELSE";
static char _tmp281[7]="SWITCH";static char _tmp282[6]="WHILE";static char _tmp283[3]="DO";
static char _tmp284[4]="FOR";static char _tmp285[5]="GOTO";static char _tmp286[9]="CONTINUE";
static char _tmp287[6]="BREAK";static char _tmp288[7]="RETURN";static char _tmp289[5]="ENUM";
static char _tmp28A[8]="NULL_kw";static char _tmp28B[4]="LET";static char _tmp28C[6]="THROW";
static char _tmp28D[4]="TRY";static char _tmp28E[6]="CATCH";static char _tmp28F[7]="EXPORT";
static char _tmp290[4]="NEW";static char _tmp291[9]="ABSTRACT";static char _tmp292[9]="FALLTHRU";
static char _tmp293[6]="USING";static char _tmp294[10]="NAMESPACE";static char _tmp295[
7]="TUNION";static char _tmp296[8]="XTUNION";static char _tmp297[7]="MALLOC";static
char _tmp298[8]="RMALLOC";static char _tmp299[7]="CALLOC";static char _tmp29A[8]="RCALLOC";
static char _tmp29B[9]="REGION_T";static char _tmp29C[9]="SIZEOF_T";static char
_tmp29D[6]="TAG_T";static char _tmp29E[7]="REGION";static char _tmp29F[5]="RNEW";
static char _tmp2A0[8]="REGIONS";static char _tmp2A1[13]="RESET_REGION";static char
_tmp2A2[4]="GEN";static char _tmp2A3[14]="NOZEROTERM_kw";static char _tmp2A4[12]="ZEROTERM_kw";
static char _tmp2A5[7]="PORTON";static char _tmp2A6[8]="PORTOFF";static char _tmp2A7[7]="PTR_OP";
static char _tmp2A8[7]="INC_OP";static char _tmp2A9[7]="DEC_OP";static char _tmp2AA[8]="LEFT_OP";
static char _tmp2AB[9]="RIGHT_OP";static char _tmp2AC[6]="LE_OP";static char _tmp2AD[6]="GE_OP";
static char _tmp2AE[6]="EQ_OP";static char _tmp2AF[6]="NE_OP";static char _tmp2B0[7]="AND_OP";
static char _tmp2B1[6]="OR_OP";static char _tmp2B2[11]="MUL_ASSIGN";static char
_tmp2B3[11]="DIV_ASSIGN";static char _tmp2B4[11]="MOD_ASSIGN";static char _tmp2B5[11]="ADD_ASSIGN";
static char _tmp2B6[11]="SUB_ASSIGN";static char _tmp2B7[12]="LEFT_ASSIGN";static
char _tmp2B8[13]="RIGHT_ASSIGN";static char _tmp2B9[11]="AND_ASSIGN";static char
_tmp2BA[11]="XOR_ASSIGN";static char _tmp2BB[10]="OR_ASSIGN";static char _tmp2BC[9]="ELLIPSIS";
static char _tmp2BD[11]="LEFT_RIGHT";static char _tmp2BE[12]="COLON_COLON";static
char _tmp2BF[11]="IDENTIFIER";static char _tmp2C0[17]="INTEGER_CONSTANT";static char
_tmp2C1[7]="STRING";static char _tmp2C2[19]="CHARACTER_CONSTANT";static char _tmp2C3[
18]="FLOATING_CONSTANT";static char _tmp2C4[9]="TYPE_VAR";static char _tmp2C5[13]="TYPEDEF_NAME";
static char _tmp2C6[16]="QUAL_IDENTIFIER";static char _tmp2C7[18]="QUAL_TYPEDEF_NAME";
static char _tmp2C8[13]="TYPE_INTEGER";static char _tmp2C9[10]="ATTRIBUTE";static
char _tmp2CA[4]="';'";static char _tmp2CB[4]="'{'";static char _tmp2CC[4]="'}'";
static char _tmp2CD[4]="','";static char _tmp2CE[4]="'='";static char _tmp2CF[4]="'('";
static char _tmp2D0[4]="')'";static char _tmp2D1[4]="'_'";static char _tmp2D2[4]="'$'";
static char _tmp2D3[4]="'<'";static char _tmp2D4[4]="':'";static char _tmp2D5[4]="'.'";
static char _tmp2D6[4]="'['";static char _tmp2D7[4]="']'";static char _tmp2D8[4]="'*'";
static char _tmp2D9[4]="'@'";static char _tmp2DA[4]="'?'";static char _tmp2DB[4]="'-'";
static char _tmp2DC[4]="'>'";static char _tmp2DD[4]="'+'";static char _tmp2DE[4]="'|'";
static char _tmp2DF[4]="'^'";static char _tmp2E0[4]="'&'";static char _tmp2E1[4]="'/'";
static char _tmp2E2[4]="'%'";static char _tmp2E3[4]="'~'";static char _tmp2E4[4]="'!'";
static char _tmp2E5[5]="prog";static char _tmp2E6[17]="translation_unit";static char
_tmp2E7[12]="export_list";static char _tmp2E8[19]="export_list_values";static char
_tmp2E9[21]="external_declaration";static char _tmp2EA[20]="function_definition";
static char _tmp2EB[21]="function_definition2";static char _tmp2EC[13]="using_action";
static char _tmp2ED[15]="unusing_action";static char _tmp2EE[17]="namespace_action";
static char _tmp2EF[19]="unnamespace_action";static char _tmp2F0[12]="declaration";
static char _tmp2F1[17]="declaration_list";static char _tmp2F2[23]="declaration_specifiers";
static char _tmp2F3[24]="storage_class_specifier";static char _tmp2F4[15]="attributes_opt";
static char _tmp2F5[11]="attributes";static char _tmp2F6[15]="attribute_list";static
char _tmp2F7[10]="attribute";static char _tmp2F8[15]="type_specifier";static char
_tmp2F9[25]="type_specifier_notypedef";static char _tmp2FA[5]="kind";static char
_tmp2FB[15]="type_qualifier";static char _tmp2FC[15]="enum_specifier";static char
_tmp2FD[11]="enum_field";static char _tmp2FE[22]="enum_declaration_list";static
char _tmp2FF[26]="struct_or_union_specifier";static char _tmp300[16]="type_params_opt";
static char _tmp301[16]="struct_or_union";static char _tmp302[24]="struct_declaration_list";
static char _tmp303[25]="struct_declaration_list0";static char _tmp304[21]="init_declarator_list";
static char _tmp305[22]="init_declarator_list0";static char _tmp306[16]="init_declarator";
static char _tmp307[19]="struct_declaration";static char _tmp308[25]="specifier_qualifier_list";
static char _tmp309[35]="notypedef_specifier_qualifier_list";static char _tmp30A[23]="struct_declarator_list";
static char _tmp30B[24]="struct_declarator_list0";static char _tmp30C[18]="struct_declarator";
static char _tmp30D[17]="tunion_specifier";static char _tmp30E[18]="tunion_or_xtunion";
static char _tmp30F[17]="tunionfield_list";static char _tmp310[18]="tunionfield_scope";
static char _tmp311[12]="tunionfield";static char _tmp312[11]="declarator";static
char _tmp313[23]="declarator_withtypedef";static char _tmp314[18]="direct_declarator";
static char _tmp315[30]="direct_declarator_withtypedef";static char _tmp316[8]="pointer";
static char _tmp317[12]="one_pointer";static char _tmp318[23]="pointer_null_and_bound";
static char _tmp319[14]="pointer_bound";static char _tmp31A[18]="zeroterm_qual_opt";
static char _tmp31B[8]="rgn_opt";static char _tmp31C[11]="tqual_list";static char
_tmp31D[20]="parameter_type_list";static char _tmp31E[9]="type_var";static char
_tmp31F[16]="optional_effect";static char _tmp320[19]="optional_rgn_order";static
char _tmp321[10]="rgn_order";static char _tmp322[16]="optional_inject";static char
_tmp323[11]="effect_set";static char _tmp324[14]="atomic_effect";static char _tmp325[
11]="region_set";static char _tmp326[15]="parameter_list";static char _tmp327[22]="parameter_declaration";
static char _tmp328[16]="identifier_list";static char _tmp329[17]="identifier_list0";
static char _tmp32A[12]="initializer";static char _tmp32B[18]="array_initializer";
static char _tmp32C[17]="initializer_list";static char _tmp32D[12]="designation";
static char _tmp32E[16]="designator_list";static char _tmp32F[11]="designator";
static char _tmp330[10]="type_name";static char _tmp331[14]="any_type_name";static
char _tmp332[15]="type_name_list";static char _tmp333[20]="abstract_declarator";
static char _tmp334[27]="direct_abstract_declarator";static char _tmp335[10]="statement";
static char _tmp336[18]="labeled_statement";static char _tmp337[21]="expression_statement";
static char _tmp338[19]="compound_statement";static char _tmp339[16]="block_item_list";
static char _tmp33A[20]="selection_statement";static char _tmp33B[15]="switch_clauses";
static char _tmp33C[20]="iteration_statement";static char _tmp33D[15]="jump_statement";
static char _tmp33E[12]="exp_pattern";static char _tmp33F[20]="conditional_pattern";
static char _tmp340[19]="logical_or_pattern";static char _tmp341[20]="logical_and_pattern";
static char _tmp342[21]="inclusive_or_pattern";static char _tmp343[21]="exclusive_or_pattern";
static char _tmp344[12]="and_pattern";static char _tmp345[17]="equality_pattern";
static char _tmp346[19]="relational_pattern";static char _tmp347[14]="shift_pattern";
static char _tmp348[17]="additive_pattern";static char _tmp349[23]="multiplicative_pattern";
static char _tmp34A[13]="cast_pattern";static char _tmp34B[14]="unary_pattern";
static char _tmp34C[16]="postfix_pattern";static char _tmp34D[16]="primary_pattern";
static char _tmp34E[8]="pattern";static char _tmp34F[19]="tuple_pattern_list";static
char _tmp350[20]="tuple_pattern_list0";static char _tmp351[14]="field_pattern";
static char _tmp352[19]="field_pattern_list";static char _tmp353[20]="field_pattern_list0";
static char _tmp354[11]="expression";static char _tmp355[22]="assignment_expression";
static char _tmp356[20]="assignment_operator";static char _tmp357[23]="conditional_expression";
static char _tmp358[20]="constant_expression";static char _tmp359[22]="logical_or_expression";
static char _tmp35A[23]="logical_and_expression";static char _tmp35B[24]="inclusive_or_expression";
static char _tmp35C[24]="exclusive_or_expression";static char _tmp35D[15]="and_expression";
static char _tmp35E[20]="equality_expression";static char _tmp35F[22]="relational_expression";
static char _tmp360[17]="shift_expression";static char _tmp361[20]="additive_expression";
static char _tmp362[26]="multiplicative_expression";static char _tmp363[16]="cast_expression";
static char _tmp364[17]="unary_expression";static char _tmp365[15]="unary_operator";
static char _tmp366[19]="postfix_expression";static char _tmp367[19]="primary_expression";
static char _tmp368[25]="argument_expression_list";static char _tmp369[26]="argument_expression_list0";
static char _tmp36A[9]="constant";static char _tmp36B[20]="qual_opt_identifier";
static char _tmp36C[17]="qual_opt_typedef";static char _tmp36D[18]="struct_union_name";
static char _tmp36E[11]="field_name";static char _tmp36F[12]="right_angle";static
struct _dynforward_ptr Cyc_yytname[268]={{_tmp264,_tmp264 + 2},{_tmp265,_tmp265 + 6},{
_tmp266,_tmp266 + 12},{_tmp267,_tmp267 + 5},{_tmp268,_tmp268 + 9},{_tmp269,_tmp269 + 
7},{_tmp26A,_tmp26A + 7},{_tmp26B,_tmp26B + 8},{_tmp26C,_tmp26C + 5},{_tmp26D,
_tmp26D + 5},{_tmp26E,_tmp26E + 6},{_tmp26F,_tmp26F + 4},{_tmp270,_tmp270 + 5},{
_tmp271,_tmp271 + 6},{_tmp272,_tmp272 + 7},{_tmp273,_tmp273 + 7},{_tmp274,_tmp274 + 
9},{_tmp275,_tmp275 + 6},{_tmp276,_tmp276 + 9},{_tmp277,_tmp277 + 9},{_tmp278,
_tmp278 + 7},{_tmp279,_tmp279 + 6},{_tmp27A,_tmp27A + 5},{_tmp27B,_tmp27B + 8},{
_tmp27C,_tmp27C + 7},{_tmp27D,_tmp27D + 7},{_tmp27E,_tmp27E + 9},{_tmp27F,_tmp27F + 
3},{_tmp280,_tmp280 + 5},{_tmp281,_tmp281 + 7},{_tmp282,_tmp282 + 6},{_tmp283,
_tmp283 + 3},{_tmp284,_tmp284 + 4},{_tmp285,_tmp285 + 5},{_tmp286,_tmp286 + 9},{
_tmp287,_tmp287 + 6},{_tmp288,_tmp288 + 7},{_tmp289,_tmp289 + 5},{_tmp28A,_tmp28A + 
8},{_tmp28B,_tmp28B + 4},{_tmp28C,_tmp28C + 6},{_tmp28D,_tmp28D + 4},{_tmp28E,
_tmp28E + 6},{_tmp28F,_tmp28F + 7},{_tmp290,_tmp290 + 4},{_tmp291,_tmp291 + 9},{
_tmp292,_tmp292 + 9},{_tmp293,_tmp293 + 6},{_tmp294,_tmp294 + 10},{_tmp295,_tmp295 + 
7},{_tmp296,_tmp296 + 8},{_tmp297,_tmp297 + 7},{_tmp298,_tmp298 + 8},{_tmp299,
_tmp299 + 7},{_tmp29A,_tmp29A + 8},{_tmp29B,_tmp29B + 9},{_tmp29C,_tmp29C + 9},{
_tmp29D,_tmp29D + 6},{_tmp29E,_tmp29E + 7},{_tmp29F,_tmp29F + 5},{_tmp2A0,_tmp2A0 + 
8},{_tmp2A1,_tmp2A1 + 13},{_tmp2A2,_tmp2A2 + 4},{_tmp2A3,_tmp2A3 + 14},{_tmp2A4,
_tmp2A4 + 12},{_tmp2A5,_tmp2A5 + 7},{_tmp2A6,_tmp2A6 + 8},{_tmp2A7,_tmp2A7 + 7},{
_tmp2A8,_tmp2A8 + 7},{_tmp2A9,_tmp2A9 + 7},{_tmp2AA,_tmp2AA + 8},{_tmp2AB,_tmp2AB + 
9},{_tmp2AC,_tmp2AC + 6},{_tmp2AD,_tmp2AD + 6},{_tmp2AE,_tmp2AE + 6},{_tmp2AF,
_tmp2AF + 6},{_tmp2B0,_tmp2B0 + 7},{_tmp2B1,_tmp2B1 + 6},{_tmp2B2,_tmp2B2 + 11},{
_tmp2B3,_tmp2B3 + 11},{_tmp2B4,_tmp2B4 + 11},{_tmp2B5,_tmp2B5 + 11},{_tmp2B6,
_tmp2B6 + 11},{_tmp2B7,_tmp2B7 + 12},{_tmp2B8,_tmp2B8 + 13},{_tmp2B9,_tmp2B9 + 11},{
_tmp2BA,_tmp2BA + 11},{_tmp2BB,_tmp2BB + 10},{_tmp2BC,_tmp2BC + 9},{_tmp2BD,_tmp2BD
+ 11},{_tmp2BE,_tmp2BE + 12},{_tmp2BF,_tmp2BF + 11},{_tmp2C0,_tmp2C0 + 17},{_tmp2C1,
_tmp2C1 + 7},{_tmp2C2,_tmp2C2 + 19},{_tmp2C3,_tmp2C3 + 18},{_tmp2C4,_tmp2C4 + 9},{
_tmp2C5,_tmp2C5 + 13},{_tmp2C6,_tmp2C6 + 16},{_tmp2C7,_tmp2C7 + 18},{_tmp2C8,
_tmp2C8 + 13},{_tmp2C9,_tmp2C9 + 10},{_tmp2CA,_tmp2CA + 4},{_tmp2CB,_tmp2CB + 4},{
_tmp2CC,_tmp2CC + 4},{_tmp2CD,_tmp2CD + 4},{_tmp2CE,_tmp2CE + 4},{_tmp2CF,_tmp2CF + 
4},{_tmp2D0,_tmp2D0 + 4},{_tmp2D1,_tmp2D1 + 4},{_tmp2D2,_tmp2D2 + 4},{_tmp2D3,
_tmp2D3 + 4},{_tmp2D4,_tmp2D4 + 4},{_tmp2D5,_tmp2D5 + 4},{_tmp2D6,_tmp2D6 + 4},{
_tmp2D7,_tmp2D7 + 4},{_tmp2D8,_tmp2D8 + 4},{_tmp2D9,_tmp2D9 + 4},{_tmp2DA,_tmp2DA + 
4},{_tmp2DB,_tmp2DB + 4},{_tmp2DC,_tmp2DC + 4},{_tmp2DD,_tmp2DD + 4},{_tmp2DE,
_tmp2DE + 4},{_tmp2DF,_tmp2DF + 4},{_tmp2E0,_tmp2E0 + 4},{_tmp2E1,_tmp2E1 + 4},{
_tmp2E2,_tmp2E2 + 4},{_tmp2E3,_tmp2E3 + 4},{_tmp2E4,_tmp2E4 + 4},{_tmp2E5,_tmp2E5 + 
5},{_tmp2E6,_tmp2E6 + 17},{_tmp2E7,_tmp2E7 + 12},{_tmp2E8,_tmp2E8 + 19},{_tmp2E9,
_tmp2E9 + 21},{_tmp2EA,_tmp2EA + 20},{_tmp2EB,_tmp2EB + 21},{_tmp2EC,_tmp2EC + 13},{
_tmp2ED,_tmp2ED + 15},{_tmp2EE,_tmp2EE + 17},{_tmp2EF,_tmp2EF + 19},{_tmp2F0,
_tmp2F0 + 12},{_tmp2F1,_tmp2F1 + 17},{_tmp2F2,_tmp2F2 + 23},{_tmp2F3,_tmp2F3 + 24},{
_tmp2F4,_tmp2F4 + 15},{_tmp2F5,_tmp2F5 + 11},{_tmp2F6,_tmp2F6 + 15},{_tmp2F7,
_tmp2F7 + 10},{_tmp2F8,_tmp2F8 + 15},{_tmp2F9,_tmp2F9 + 25},{_tmp2FA,_tmp2FA + 5},{
_tmp2FB,_tmp2FB + 15},{_tmp2FC,_tmp2FC + 15},{_tmp2FD,_tmp2FD + 11},{_tmp2FE,
_tmp2FE + 22},{_tmp2FF,_tmp2FF + 26},{_tmp300,_tmp300 + 16},{_tmp301,_tmp301 + 16},{
_tmp302,_tmp302 + 24},{_tmp303,_tmp303 + 25},{_tmp304,_tmp304 + 21},{_tmp305,
_tmp305 + 22},{_tmp306,_tmp306 + 16},{_tmp307,_tmp307 + 19},{_tmp308,_tmp308 + 25},{
_tmp309,_tmp309 + 35},{_tmp30A,_tmp30A + 23},{_tmp30B,_tmp30B + 24},{_tmp30C,
_tmp30C + 18},{_tmp30D,_tmp30D + 17},{_tmp30E,_tmp30E + 18},{_tmp30F,_tmp30F + 17},{
_tmp310,_tmp310 + 18},{_tmp311,_tmp311 + 12},{_tmp312,_tmp312 + 11},{_tmp313,
_tmp313 + 23},{_tmp314,_tmp314 + 18},{_tmp315,_tmp315 + 30},{_tmp316,_tmp316 + 8},{
_tmp317,_tmp317 + 12},{_tmp318,_tmp318 + 23},{_tmp319,_tmp319 + 14},{_tmp31A,
_tmp31A + 18},{_tmp31B,_tmp31B + 8},{_tmp31C,_tmp31C + 11},{_tmp31D,_tmp31D + 20},{
_tmp31E,_tmp31E + 9},{_tmp31F,_tmp31F + 16},{_tmp320,_tmp320 + 19},{_tmp321,_tmp321
+ 10},{_tmp322,_tmp322 + 16},{_tmp323,_tmp323 + 11},{_tmp324,_tmp324 + 14},{_tmp325,
_tmp325 + 11},{_tmp326,_tmp326 + 15},{_tmp327,_tmp327 + 22},{_tmp328,_tmp328 + 16},{
_tmp329,_tmp329 + 17},{_tmp32A,_tmp32A + 12},{_tmp32B,_tmp32B + 18},{_tmp32C,
_tmp32C + 17},{_tmp32D,_tmp32D + 12},{_tmp32E,_tmp32E + 16},{_tmp32F,_tmp32F + 11},{
_tmp330,_tmp330 + 10},{_tmp331,_tmp331 + 14},{_tmp332,_tmp332 + 15},{_tmp333,
_tmp333 + 20},{_tmp334,_tmp334 + 27},{_tmp335,_tmp335 + 10},{_tmp336,_tmp336 + 18},{
_tmp337,_tmp337 + 21},{_tmp338,_tmp338 + 19},{_tmp339,_tmp339 + 16},{_tmp33A,
_tmp33A + 20},{_tmp33B,_tmp33B + 15},{_tmp33C,_tmp33C + 20},{_tmp33D,_tmp33D + 15},{
_tmp33E,_tmp33E + 12},{_tmp33F,_tmp33F + 20},{_tmp340,_tmp340 + 19},{_tmp341,
_tmp341 + 20},{_tmp342,_tmp342 + 21},{_tmp343,_tmp343 + 21},{_tmp344,_tmp344 + 12},{
_tmp345,_tmp345 + 17},{_tmp346,_tmp346 + 19},{_tmp347,_tmp347 + 14},{_tmp348,
_tmp348 + 17},{_tmp349,_tmp349 + 23},{_tmp34A,_tmp34A + 13},{_tmp34B,_tmp34B + 14},{
_tmp34C,_tmp34C + 16},{_tmp34D,_tmp34D + 16},{_tmp34E,_tmp34E + 8},{_tmp34F,_tmp34F
+ 19},{_tmp350,_tmp350 + 20},{_tmp351,_tmp351 + 14},{_tmp352,_tmp352 + 19},{_tmp353,
_tmp353 + 20},{_tmp354,_tmp354 + 11},{_tmp355,_tmp355 + 22},{_tmp356,_tmp356 + 20},{
_tmp357,_tmp357 + 23},{_tmp358,_tmp358 + 20},{_tmp359,_tmp359 + 22},{_tmp35A,
_tmp35A + 23},{_tmp35B,_tmp35B + 24},{_tmp35C,_tmp35C + 24},{_tmp35D,_tmp35D + 15},{
_tmp35E,_tmp35E + 20},{_tmp35F,_tmp35F + 22},{_tmp360,_tmp360 + 17},{_tmp361,
_tmp361 + 20},{_tmp362,_tmp362 + 26},{_tmp363,_tmp363 + 16},{_tmp364,_tmp364 + 17},{
_tmp365,_tmp365 + 15},{_tmp366,_tmp366 + 19},{_tmp367,_tmp367 + 19},{_tmp368,
_tmp368 + 25},{_tmp369,_tmp369 + 26},{_tmp36A,_tmp36A + 9},{_tmp36B,_tmp36B + 20},{
_tmp36C,_tmp36C + 17},{_tmp36D,_tmp36D + 18},{_tmp36E,_tmp36E + 11},{_tmp36F,
_tmp36F + 12}};static short Cyc_yyr1[490]={0,129,130,130,130,130,130,130,130,130,
130,130,131,131,132,132,133,133,133,134,134,134,134,135,135,136,137,138,139,140,
140,140,140,141,141,142,142,142,142,142,142,142,142,142,142,143,143,143,143,143,
143,143,144,144,145,146,146,147,147,147,147,147,147,148,148,149,149,149,149,149,
149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,150,151,151,151,
152,152,152,153,153,154,154,154,155,155,155,156,156,157,157,158,158,159,159,160,
161,161,162,162,163,164,164,164,164,164,164,165,165,165,165,165,165,166,167,167,
168,168,168,169,169,169,170,170,171,171,171,171,172,172,172,173,173,174,174,175,
175,176,176,176,176,176,176,176,176,176,176,177,177,177,177,177,177,177,177,177,
177,177,178,178,179,180,180,180,180,181,181,181,182,182,182,183,183,183,184,184,
185,185,185,185,186,186,187,187,188,188,189,189,190,190,191,191,192,192,192,192,
193,193,194,194,195,195,195,196,197,197,198,198,199,199,199,199,200,200,200,200,
201,202,202,203,203,204,204,205,205,205,205,205,206,206,207,207,207,208,208,208,
208,208,208,208,208,208,208,208,209,209,209,209,209,209,209,209,209,209,209,210,
211,211,212,212,213,213,213,213,213,213,214,214,214,214,214,214,215,215,215,215,
215,215,216,216,216,216,216,216,216,216,216,216,216,216,216,216,217,217,217,217,
217,217,217,217,218,219,219,220,220,221,221,222,222,223,223,224,224,225,225,225,
226,226,226,226,226,227,227,227,228,228,228,229,229,229,229,230,230,231,231,231,
231,231,231,231,232,233,234,234,234,234,234,234,234,234,234,234,234,234,234,234,
235,235,235,236,236,237,237,238,238,238,239,239,240,240,241,241,242,242,242,242,
242,242,242,242,242,242,242,243,243,243,243,243,243,243,244,245,245,246,246,247,
247,248,248,249,249,250,250,250,251,251,251,251,251,252,252,252,253,253,253,254,
254,254,254,255,255,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,
256,257,257,257,258,258,258,258,258,258,258,258,258,258,259,259,259,259,259,259,
259,259,259,260,261,261,262,262,262,262,263,263,264,264,265,265,266,266,267,267};
static short Cyc_yyr2[490]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,2,3,1,1,1,2,3,3,4,3,4,2,1,
2,1,2,3,5,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,
3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,4,4,1,1,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,0,3,1,1,0,1,
1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,3,7,4,6,1,1,1,2,3,3,0,1,1,2,5,
1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,2,2,1,2,0,3,3,0,1,1,0,1,
1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,
2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,6,3,9,
6,5,3,1,2,2,3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,
2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,
1,2,2,4,2,6,6,1,1,1,3,1,1,3,4,4,5,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,1,
1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,
3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,9,11,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,1,
3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[975]={0,
18,45,46,47,48,50,65,68,69,70,71,72,73,74,75,87,88,89,103,104,41,0,0,51,0,0,136,
137,0,0,84,0,0,480,194,482,481,483,85,0,0,66,0,179,179,177,1,0,16,0,0,17,0,35,43,
37,63,39,76,77,0,78,185,0,147,0,172,182,79,151,101,49,48,42,0,91,479,0,480,476,
477,478,0,356,0,0,0,0,217,0,358,359,25,27,0,0,0,0,0,0,0,0,0,0,0,0,175,176,178,2,
0,0,0,0,29,0,109,110,112,36,44,38,40,105,484,485,101,187,0,186,0,33,0,0,19,196,0,
0,160,148,173,184,183,185,0,64,0,49,95,0,93,0,480,365,0,0,0,0,0,0,0,0,0,0,0,101,
0,0,466,0,0,0,453,0,0,451,452,0,382,384,397,405,407,409,411,413,415,418,423,426,
429,433,0,435,454,465,464,0,366,364,32,0,0,101,0,0,0,119,115,117,235,237,0,0,0,9,
10,486,487,195,86,0,0,152,67,215,0,212,0,0,0,3,0,5,0,30,0,0,0,20,0,106,107,0,100,
101,0,0,0,0,0,0,0,0,0,0,0,0,0,480,270,272,278,274,0,276,258,259,260,0,261,262,
263,0,34,21,112,202,218,0,0,198,196,0,242,0,182,0,52,0,0,96,92,0,0,360,0,0,0,443,
0,399,433,0,400,401,0,0,0,0,0,0,0,436,437,0,0,0,0,439,440,438,0,357,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,387,388,389,390,391,392,393,394,395,396,386,0,441,0,
460,461,0,0,0,468,0,101,372,373,0,370,0,219,0,0,0,0,238,210,0,120,125,121,123,
116,118,196,0,244,236,245,489,488,0,81,82,83,58,57,0,55,153,196,214,149,244,216,
161,162,0,80,0,181,180,26,0,28,0,111,113,221,220,22,98,108,0,0,0,127,128,130,0,
101,0,134,0,0,0,0,0,0,0,0,307,308,309,0,0,311,0,0,0,0,0,0,279,275,112,277,273,
271,203,0,0,0,209,197,204,156,0,0,0,198,158,0,159,154,182,188,53,102,0,97,404,94,
90,368,369,0,0,0,222,0,0,226,0,0,0,231,0,0,0,0,0,0,0,0,0,467,474,0,473,383,406,0,
408,410,412,414,416,417,421,422,419,420,424,425,428,427,430,431,432,385,459,456,
0,458,0,0,0,361,0,367,31,379,0,375,380,0,377,362,0,0,239,126,122,124,0,198,0,182,
0,246,196,0,0,257,241,0,0,0,119,0,101,196,0,0,171,150,213,4,6,0,131,114,0,0,198,
101,142,0,0,0,286,0,0,0,0,0,306,310,0,0,0,265,0,0,0,269,0,23,196,0,206,0,0,199,0,
157,202,190,243,155,188,174,0,7,0,442,0,0,234,0,223,0,227,230,232,447,0,0,0,0,0,
0,0,0,434,470,0,0,457,455,0,0,371,374,376,363,0,240,211,253,0,247,248,182,0,198,
0,182,0,0,0,0,54,56,0,163,0,198,0,0,182,0,129,132,105,135,144,143,0,0,138,0,0,0,
0,0,0,0,0,0,0,0,0,0,286,312,0,0,0,0,24,198,0,207,205,0,198,0,189,0,8,0,0,0,233,
224,228,0,0,0,0,402,403,446,472,0,475,398,469,471,0,378,381,252,250,255,0,256,
249,182,0,61,59,60,164,167,0,169,170,165,182,0,133,145,142,142,280,286,0,0,0,0,0,
0,314,315,317,319,321,323,325,327,330,335,338,341,345,347,354,355,0,0,283,292,0,
0,0,0,0,0,0,0,0,0,313,0,0,0,268,192,208,200,191,196,13,0,0,445,444,0,229,448,0,0,
462,0,254,251,0,168,166,99,0,141,140,0,0,286,0,351,0,0,348,286,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,349,287,0,294,0,0,0,302,0,0,0,0,0,0,285,264,267,0,0,198,12,
14,0,0,0,0,463,0,0,281,282,0,0,0,0,286,288,318,0,320,322,324,326,328,329,333,334,
331,332,336,337,340,339,342,343,344,0,293,295,296,0,304,303,0,298,0,0,0,0,201,
193,15,0,0,0,0,146,284,350,0,346,289,0,286,297,305,299,300,0,0,225,449,0,62,0,0,
316,286,290,301,266,0,353,352,291,450,0,0,0};static short Cyc_yydefgoto[139]={972,
47,626,817,48,49,263,50,414,51,416,52,133,53,54,477,208,398,399,209,57,220,210,
59,149,150,60,146,61,241,242,116,117,118,243,211,383,426,427,428,62,63,690,691,
692,64,429,65,403,66,67,68,107,144,129,623,556,69,557,469,615,461,465,466,378,
283,228,88,89,493,419,494,495,496,497,212,285,286,558,389,266,267,268,269,270,
271,698,272,273,775,776,777,778,779,780,781,782,783,784,785,786,787,788,789,790,
367,368,369,546,547,548,274,179,355,180,483,181,182,183,184,185,186,187,188,189,
190,191,192,193,194,195,509,510,196,197,71,818,221,393};static short Cyc_yypact[
975]={2463,- -32768,- -32768,- -32768,- -32768,- 13,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,2941,210,629,- -32768,178,1,- -32768,- -32768,- 3,25,60,75,85,- -32768,121,-
-32768,- -32768,- -32768,- -32768,130,518,192,161,207,207,177,- -32768,2344,- -32768,
155,389,- -32768,762,2941,2941,2941,- -32768,2941,- -32768,- -32768,246,- -32768,106,
1130,218,- 14,691,583,- -32768,- -32768,203,221,248,- -32768,178,276,- -32768,3090,
259,- -32768,- -32768,- -32768,4653,- -32768,278,303,3090,297,305,401,- -32768,195,-
-32768,- -32768,5119,5119,5119,2344,2344,193,329,302,408,193,5340,1245,- -32768,-
-32768,- -32768,- -32768,2344,2581,2344,2581,- -32768,412,350,- -32768,2874,- -32768,
- -32768,- -32768,- -32768,5340,- -32768,- -32768,203,- -32768,178,- -32768,1602,-
-32768,1130,762,- -32768,5104,5119,3492,- -32768,218,- -32768,- -32768,- -32768,106,
5119,- -32768,2581,- -32768,418,421,438,178,- 15,- -32768,3090,137,4824,439,4653,
4702,450,459,461,469,478,203,4873,4873,- -32768,1975,484,4934,- -32768,4934,4934,-
-32768,- -32768,317,- -32768,- -32768,- 24,517,485,500,507,715,123,725,349,251,-
-32768,700,4934,186,- 43,- -32768,524,1125,549,- -32768,- -32768,580,4653,203,1125,
570,69,5340,5355,5340,545,- -32768,- 48,- 48,- 48,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,46,548,- -32768,- -32768,721,345,- -32768,576,17,589,- -32768,598,-
-32768,606,- -32768,302,3584,1130,- -32768,611,5340,- -32768,750,605,- 42,626,277,
630,3027,634,652,645,655,3645,3027,15,138,658,657,- -32768,- -32768,1728,1728,762,
1728,- -32768,- -32768,- -32768,659,- -32768,- -32768,- -32768,213,- -32768,- -32768,669,
707,- -32768,7,695,693,237,703,710,80,583,718,735,80,736,178,- -32768,4653,742,-
-32768,745,751,1975,- -32768,5340,- -32768,- -32768,3124,- -32768,773,4653,4653,4653,
4653,4653,766,1975,- -32768,- -32768,1728,767,443,4653,- -32768,- -32768,- -32768,
4653,- -32768,4934,4653,4934,4934,4934,4934,4934,4934,4934,4934,4934,4934,4934,
4934,4934,4934,4934,4934,4934,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,4653,- -32768,193,- -32768,- -32768,3706,193,
4653,- -32768,774,203,- -32768,- -32768,769,783,3090,- -32768,487,884,785,5119,-
-32768,794,799,- -32768,5355,5355,5355,- -32768,- -32768,2702,3767,124,- -32768,347,
- -32768,- -32768,7,- -32768,- -32768,- -32768,- -32768,803,798,811,- -32768,2646,-
-32768,503,514,- -32768,- -32768,- -32768,5340,- -32768,2096,- -32768,- -32768,- -32768,
2344,- -32768,2344,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,985,
4653,819,825,- -32768,820,376,203,178,830,4653,4653,829,835,4653,910,1854,839,-
-32768,- -32768,- -32768,495,900,- -32768,3859,3027,847,854,4653,3027,- -32768,-
-32768,2874,- -32768,- -32768,- -32768,- -32768,5340,841,71,- -32768,- -32768,831,-
-32768,7,838,5237,693,- -32768,5119,- -32768,- -32768,583,796,- -32768,- -32768,2225,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,843,845,863,- -32768,193,4653,-
-32768,712,3584,406,- -32768,533,850,852,853,543,5340,855,858,4763,- -32768,-
-32768,857,861,- -32768,517,223,485,500,507,715,123,123,725,725,725,725,349,349,
251,251,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,859,- -32768,- 37,5119,
3400,- -32768,3017,- -32768,- -32768,- -32768,3090,- -32768,- -32768,864,865,- -32768,
140,873,- -32768,- -32768,- -32768,- -32768,866,693,869,583,856,347,5222,5119,3920,-
-32768,- -32768,734,875,46,2758,876,203,5104,5119,4012,- -32768,503,- -32768,-
-32768,- -32768,810,- -32768,- -32768,750,4653,693,203,816,550,565,4653,832,577,874,
4104,4165,582,- -32768,- -32768,870,883,878,- -32768,867,877,586,- -32768,1130,-
-32768,893,5119,- -32768,895,7,- -32768,882,- -32768,169,- -32768,710,- -32768,796,-
-32768,902,- -32768,2344,903,116,892,- -32768,894,- -32768,3216,- -32768,- -32768,-
-32768,- -32768,4653,986,4653,4702,905,903,906,3400,- -32768,- -32768,4653,4653,-
-32768,- -32768,80,754,- -32768,- -32768,- -32768,- -32768,3188,- -32768,- -32768,-
-32768,907,- -32768,- -32768,583,908,693,80,583,912,587,909,914,- -32768,- -32768,
915,- -32768,917,693,920,80,583,916,- -32768,- -32768,5340,- -32768,- -32768,- -32768,
925,178,596,3027,927,924,1416,898,930,3027,4653,4226,604,4287,617,4348,832,-
-32768,918,944,- 10,936,- -32768,693,157,- -32768,- -32768,943,693,5340,- -32768,546,
- -32768,932,933,4653,- -32768,- -32768,- -32768,3584,620,935,938,- -32768,773,-
-32768,- -32768,777,- -32768,- -32768,- -32768,- -32768,3400,- -32768,- -32768,- -32768,
- -32768,- -32768,937,- -32768,- -32768,583,952,- -32768,- -32768,- -32768,- -32768,-
-32768,939,- -32768,- -32768,- -32768,583,942,- -32768,945,45,816,1021,832,947,4983,
946,2096,4934,963,- -32768,- 11,- -32768,978,934,760,821,125,842,381,353,- -32768,-
-32768,- -32768,- -32768,1002,4934,1728,- -32768,- -32768,621,3027,627,4409,3027,643,
4470,4531,647,975,- -32768,3027,3027,984,- -32768,- -32768,- -32768,982,- -32768,893,
- -32768,987,653,- -32768,- -32768,254,- -32768,- -32768,5340,1063,- -32768,3308,-
-32768,- -32768,988,- -32768,- -32768,- -32768,5340,- -32768,- -32768,3027,990,832,
1975,- -32768,5340,981,- -32768,1378,4934,4653,4934,4934,4934,4934,4934,4934,4934,
4934,4934,4934,4934,4934,4934,4934,4934,4934,4934,4653,- -32768,- -32768,993,-
-32768,3027,3027,651,- -32768,3027,3027,656,3027,662,4592,- -32768,- -32768,- -32768,
970,7,693,- -32768,- -32768,474,4653,989,997,- -32768,1004,666,- -32768,- -32768,996,
998,1000,4934,832,- -32768,517,318,485,500,500,715,123,123,725,725,725,725,349,
349,251,251,- -32768,- -32768,- -32768,383,- -32768,- -32768,- -32768,3027,- -32768,-
-32768,3027,- -32768,3027,3027,683,1016,- -32768,- -32768,- -32768,814,1001,5340,
1003,- -32768,- -32768,903,150,- -32768,- -32768,4653,1378,- -32768,- -32768,- -32768,-
-32768,3027,3027,- -32768,- -32768,1006,- -32768,1007,1009,- -32768,832,- -32768,-
-32768,- -32768,1010,- -32768,- -32768,- -32768,- -32768,1108,1112,- -32768};static
short Cyc_yypgoto[139]={- -32768,174,- -32768,232,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,41,- 102,99,- -32768,- -32768,0,552,- -32768,70,- 176,1018,31,
- -32768,- -32768,- 112,- -32768,- 25,- -32768,442,- -32768,- -32768,- -32768,887,888,391,
462,- -32768,- -32768,541,- -32768,- -32768,158,- -32768,- -32768,86,- 27,1065,- 355,48,
- -32768,- -32768,1084,- 164,1008,510,- 127,- 61,- 97,- 451,269,539,544,- 386,- 435,- 104,
- 361,- 122,- -32768,- 212,- 148,- 493,- 285,- -32768,663,- 143,129,- 111,- 50,- 160,319,-
-32768,- -32768,- 29,- 259,- -32768,- 379,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,136,955,- -32768,504,- -32768,- -32768,234,356,- -32768,- 146,
- 395,- 142,- 317,- 324,- 286,836,- 311,- 321,- 274,- 322,- 302,- 150,730,465,- -32768,-
-32768,- 316,- -32768,233,330,- 32,1103,- 325,36};static short Cyc_yytable[5466]={55,
227,130,514,454,455,566,457,512,281,518,519,305,302,284,524,525,239,306,517,619,
55,320,390,321,322,418,317,613,126,582,58,532,381,290,135,535,526,527,282,295,
102,515,356,534,653,363,578,55,577,688,689,58,325,55,55,55,505,55,520,521,522,
523,396,55,139,846,462,323,145,56,432,391,392,364,577,155,34,651,58,72,808,616,
130,37,58,58,58,544,58,240,56,94,41,326,58,156,631,55,55,610,809,245,35,276,132,
662,847,95,578,463,55,55,55,55,141,660,447,56,55,74,411,448,475,56,56,56,103,56,
58,58,55,602,55,56,686,96,397,392,119,139,312,58,58,58,58,377,55,482,- 139,58,390,
737,120,121,122,487,123,488,90,132,388,58,134,58,35,630,35,56,56,504,97,264,376,
275,612,405,98,58,373,481,56,56,56,56,473,471,99,223,56,685,528,529,530,407,333,
334,854,855,402,391,56,35,56,381,381,381,218,723,380,421,100,407,219,154,128,748,
56,134,464,277,433,110,200,213,214,215,561,758,449,265,385,134,297,335,230,856,
101,386,55,382,218,960,336,561,857,298,219,659,450,394,395,451,357,358,359,91,
111,112,387,460,392,811,55,55,812,55,814,105,34,58,280,216,217,404,695,37,- 196,
392,482,275,- 196,104,634,218,232,233,234,235,478,219,296,430,360,58,58,109,58,
204,361,362,34,205,724,264,264,227,264,37,56,106,91,621,75,145,459,55,178,323,40,
91,291,474,277,147,136,479,805,323,137,70,464,138,56,56,649,56,34,134,280,538,
148,470,36,37,38,482,58,729,124,155,456,76,92,608,93,646,264,815,323,642,- 218,
265,265,- 218,265,889,341,34,572,156,70,407,544,571,37,342,343,70,152,380,380,380,
70,435,198,56,436,91,565,838,125,407,34,199,664,70,571,407,201,37,570,377,576,
318,151,586,464,92,41,202,382,382,382,55,265,55,92,44,45,46,728,323,323,581,324,
652,609,70,70,948,91,70,387,936,666,222,372,91,482,55,70,70,70,70,58,678,58,40,
404,408,680,668,409,562,237,55,729,563,246,897,564,231,681,70,667,902,34,339,862,
340,58,430,36,37,38,679,70,863,864,55,596,151,424,56,92,56,58,323,445,377,113,
114,733,288,949,226,132,616,734,860,746,861,739,550,750,541,203,622,545,56,58,
635,713,236,244,224,822,761,491,492,718,947,292,905,293,56,226,92,903,728,909,
910,318,867,92,915,916,572,134,908,498,729,676,294,502,301,318,323,407,56,507,
407,464,676,134,406,307,917,918,513,565,687,906,907,34,308,70,309,439,964,36,37,
38,406,446,310,576,437,712,911,912,913,914,970,311,901,829,579,542,580,319,323,
327,420,70,536,599,832,379,323,384,620,91,40,34,91,328,55,34,573,36,37,38,574,
728,37,575,430,40,961,401,151,329,844,41,55,365,386,430,843,330,430,244,44,45,46,
34,323,58,370,637,866,36,37,38,142,143,323,275,816,641,385,622,625,323,400,58,
693,386,420,44,45,46,499,500,501,77,589,590,323,371,593,694,597,508,655,375,56,
511,656,890,323,410,705,699,606,323,740,126,963,323,752,412,711,753,56,898,766,
899,92,767,413,92,749,78,799,134,431,323,415,531,919,920,921,422,508,760,885,802,
79,80,323,81,82,323,323,37,823,868,894,406,323,434,406,870,83,438,84,85,714,440,
560,441,70,86,70,442,323,879,946,874,323,87,406,887,323,443,888,926,406,323,587,
458,929,452,508,323,603,453,931,408,607,91,942,238,226,91,344,345,346,347,348,
349,350,351,352,353,323,331,332,954,226,55,545,337,338,958,460,226,722,226,962,
467,508,468,354,44,45,46,70,472,34,16,17,18,632,633,36,37,38,688,689,464,58,671,
672,673,401,702,704,392,791,476,264,386,40,44,45,46,480,34,553,554,555,55,484,36,
37,38,325,420,226,34,696,697,126,424,741,742,37,226,425,56,115,485,44,45,46,41,
92,486,730,503,92,506,58,539,44,45,46,826,827,850,851,537,264,300,540,303,303,91,
265,549,420,852,853,314,315,551,406,34,303,552,303,303,568,36,37,38,567,406,858,
859,406,56,569,424,956,323,670,583,77,303,835,836,44,45,46,91,584,683,585,588,
796,798,591,801,592,804,594,598,600,604,265,605,617,508,611,55,628,627,614,226,
629,638,70,639,640,821,78,379,644,643,226,647,648,650,657,35,658,665,543,706,661,
153,80,663,81,82,58,700,37,674,677,707,708,709,92,420,264,83,710,84,85,280,732,
491,492,715,86,420,717,725,738,721,645,178,87,726,793,731,768,735,736,745,747,
754,795,56,806,765,755,756,303,757,92,751,759,764,769,762,770,872,794,807,876,
878,810,813,819,820,824,825,830,828,833,831,265,837,839,125,834,842,848,303,849,
303,303,303,303,303,303,303,303,303,303,303,303,303,303,303,303,303,318,845,34,
244,865,880,883,904,36,37,38,420,40,884,891,900,934,886,424,893,896,923,941,939,
420,922,943,44,45,46,940,945,944,955,973,957,226,959,974,933,967,968,869,969,971,
873,937,675,225,938,417,684,881,882,763,108,423,140,720,2,3,4,73,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,289,935,21,303,895,719,716,636,374,792,744,77,127,516,
0,22,0,23,0,0,0,0,0,24,0,0,0,27,28,0,0,420,0,29,30,31,0,924,925,0,0,927,928,0,
930,0,0,0,0,78,0,0,0,0,0,0,0,0,0,0,0,366,0,0,153,80,125,81,82,0,303,37,0,226,35,
36,0,38,39,40,83,131,84,85,303,0,0,42,43,86,0,0,0,950,0,0,951,87,952,953,0,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,0,157,158,0,965,966,0,0,0,0,0,0,0,22,77,0,
159,0,0,0,160,0,0,0,0,27,28,161,162,163,164,29,30,31,0,165,206,0,166,0,0,0,0,0,
167,168,303,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,169,81,82,35,36,37,38,
39,40,0,207,0,0,0,170,0,42,229,0,0,0,0,0,172,0,0,173,0,174,0,0,175,0,303,176,177,
0,0,0,0,0,303,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,696,697,21,157,
158,247,0,248,249,250,251,252,253,254,255,22,77,23,159,256,0,0,160,24,257,0,0,27,
28,161,162,163,164,29,30,31,258,165,0,259,166,771,772,0,0,0,167,168,0,0,0,0,0,0,
77,0,0,0,0,0,0,0,0,0,0,0,0,0,0,260,80,169,81,82,35,36,37,38,39,40,261,131,0,0,0,
170,0,42,229,0,0,0,78,0,172,0,0,173,0,174,0,841,175,0,303,176,177,153,80,0,81,82,
0,0,37,0,0,0,0,0,0,0,303,773,0,84,85,0,0,0,0,0,86,0,0,173,0,774,0,0,87,0,0,176,
177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,303,0,303,303,
303,303,303,303,303,303,303,303,303,303,303,303,303,303,303,0,0,0,0,0,0,0,0,0,0,
2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,157,158,247,303,248,249,
250,251,252,253,254,255,22,77,23,159,256,0,0,160,24,257,0,0,27,28,161,162,163,
164,29,30,31,258,165,0,259,166,0,0,0,0,0,167,168,0,0,0,0,0,0,303,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,260,80,169,81,82,35,36,37,38,39,40,261,131,262,0,0,170,0,42,229,0,0,
0,0,0,172,0,0,173,0,174,0,0,175,0,0,176,177,2,3,4,73,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,157,158,247,0,248,249,250,251,252,253,254,255,22,77,23,159,
256,0,0,160,24,257,0,0,27,28,161,162,163,164,29,30,31,258,165,0,259,166,0,0,0,0,
0,167,168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,260,80,169,81,82,35,36,37,38,
39,40,261,131,0,0,0,170,0,42,229,0,0,0,0,0,172,0,0,173,0,174,0,0,175,0,0,176,177,
2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,157,158,0,0,0,0,0,0,0,0,
0,0,22,77,23,159,0,0,0,160,24,0,0,0,27,28,161,162,163,164,29,30,31,0,165,0,0,166,
0,0,0,0,0,167,168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,169,81,82,35,
36,37,38,39,40,595,0,0,0,0,170,0,42,229,0,0,0,0,0,172,0,0,173,0,174,0,0,175,0,0,
176,177,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,157,158,0,0,0,0,0,0,0,0,0,0,
22,77,0,159,0,0,0,160,0,0,0,0,27,28,161,162,163,164,29,30,31,0,165,0,0,166,0,0,0,
0,0,167,168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,169,81,82,35,36,37,
38,39,40,0,316,0,0,0,170,0,42,229,0,0,0,0,0,172,0,0,173,0,174,0,0,175,0,0,176,
177,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,157,158,0,0,0,0,0,0,0,0,0,0,22,
77,0,159,0,0,0,160,0,0,0,0,27,28,161,162,163,164,29,30,31,0,165,0,0,166,0,0,0,0,
0,167,168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,169,81,82,35,36,37,38,
39,40,0,0,0,0,0,170,0,42,229,0,0,0,0,0,172,0,0,173,0,174,0,0,175,0,0,176,177,- 11,
1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,
0,0,22,0,23,0,0,0,624,0,24,0,25,26,27,28,0,0,0,0,29,30,31,0,0,0,0,0,0,0,32,33,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,-
11,0,0,41,0,42,43,0,0,0,0,0,44,45,46,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,28,
0,0,0,0,29,30,31,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,- 11,0,0,41,0,42,43,0,0,0,0,0,44,45,46,- 11,1,
0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,
0,22,0,23,0,0,0,0,0,24,0,25,26,27,28,0,0,0,0,29,30,31,0,0,0,0,0,0,0,32,33,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,0,0,0,
41,0,42,43,0,0,0,0,0,44,45,46,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,28,0,0,0,0,29,
30,31,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
0,0,34,0,0,0,0,35,36,37,38,39,40,22,0,- 11,0,0,41,0,42,43,0,0,0,27,28,44,45,46,0,
29,30,31,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,
278,0,0,34,0,22,0,0,35,36,37,38,39,40,280,0,0,27,28,401,0,42,43,29,30,31,386,0,
44,45,46,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,278,0,0,0,
0,22,0,0,35,36,0,38,39,40,280,0,0,27,28,385,0,42,43,29,30,31,386,0,44,45,46,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,
0,0,0,0,0,424,0,42,43,0,0,0,0,0,44,45,46,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,0,0,27,28,0,0,
0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,21,0,0,0,0,35,36,0,38,39,40,0,131,22,0,238,0,0,42,43,0,24,0,0,0,27,28,
0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,35,36,0,38,39,40,0,0,0,0,0,0,0,42,43,157,158,247,77,248,249,250,251,
252,253,254,255,0,77,0,159,256,0,0,160,0,257,0,0,0,0,161,162,163,164,0,0,0,258,
165,0,259,166,0,0,0,78,0,167,168,0,0,0,0,0,0,0,0,654,0,0,153,80,0,81,82,0,0,37,0,
0,260,80,169,81,82,0,83,37,84,85,77,261,131,0,0,86,170,0,0,171,0,0,0,87,0,172,0,
0,173,0,174,157,158,175,0,0,176,177,489,0,0,0,0,0,77,0,159,0,78,0,160,0,0,0,0,0,
0,161,162,163,164,0,0,153,80,165,81,82,166,0,37,0,0,0,167,168,0,0,0,83,0,84,85,0,
0,0,0,0,86,0,0,0,0,0,0,0,87,34,80,169,81,82,0,0,37,0,0,0,77,304,490,0,0,170,0,0,
171,0,0,491,492,0,172,157,158,173,0,174,0,0,175,0,0,176,177,0,77,0,159,0,0,0,160,
0,0,0,78,0,0,161,162,163,164,0,0,0,0,165,743,0,166,153,80,0,81,82,167,168,37,0,0,
0,0,0,0,0,0,83,0,84,85,0,0,491,492,0,86,0,0,34,80,169,81,82,87,0,37,0,0,0,0,304,
727,0,0,170,0,0,171,0,0,491,492,0,172,157,158,173,0,174,0,0,175,0,0,176,177,0,77,
0,159,0,0,0,160,0,0,0,0,0,0,161,162,163,164,0,0,0,0,165,0,0,166,0,0,0,0,0,167,
168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,169,81,82,0,0,37,0,0,0,0,304,
892,0,0,170,0,0,171,0,0,491,492,0,172,157,158,173,0,174,0,0,175,0,0,176,177,0,77,
0,159,0,0,0,160,0,0,0,0,0,0,161,162,163,164,0,0,0,0,165,0,0,166,0,0,0,0,0,167,
168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,169,81,82,0,0,37,0,0,0,0,304,
0,0,0,170,0,0,171,0,0,491,492,0,172,157,158,173,0,174,0,0,175,0,0,176,177,0,77,0,
159,0,0,0,160,0,0,0,0,0,0,161,162,163,164,0,0,0,0,165,0,0,166,0,0,0,0,0,167,168,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,169,81,82,0,0,37,0,0,0,0,0,0,0,0,
170,0,0,171,0,0,0,0,287,172,157,158,173,0,174,0,0,175,0,0,176,177,0,77,0,159,0,0,
0,160,0,0,0,0,0,0,161,162,163,164,0,0,0,0,165,0,0,166,0,0,0,0,0,167,168,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,157,158,0,0,0,34,80,169,81,82,0,0,37,77,0,159,0,304,0,
160,0,170,0,0,171,0,161,162,163,164,172,0,0,173,165,174,0,166,175,0,0,176,177,
167,168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,157,158,0,0,0,34,80,169,81,82,0,0,37,77,
0,159,444,0,0,160,0,170,0,0,171,0,161,162,163,164,172,0,0,173,165,174,0,166,175,
0,0,176,177,167,168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,157,158,0,0,0,34,80,169,81,
82,0,0,37,77,0,159,0,0,0,160,0,170,533,0,171,0,161,162,163,164,172,0,0,173,165,
174,0,166,175,0,0,176,177,167,168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,
80,169,81,82,0,0,37,0,0,0,0,0,0,0,0,170,0,0,171,0,0,0,0,559,172,157,158,173,0,
174,0,0,175,0,0,176,177,0,77,0,159,0,0,0,160,0,0,0,0,0,0,161,162,163,164,0,0,0,0,
165,0,0,166,0,0,0,0,0,167,168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,157,158,0,0,0,34,
80,169,81,82,0,0,37,77,0,159,0,0,0,160,0,170,601,0,171,0,161,162,163,164,172,0,0,
173,165,174,0,166,175,0,0,176,177,167,168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,34,80,169,81,82,0,0,37,0,0,0,0,0,0,0,0,170,0,0,171,0,0,0,0,669,172,157,158,
173,0,174,0,0,175,0,0,176,177,0,77,0,159,0,0,0,160,0,0,0,0,0,0,161,162,163,164,0,
0,0,0,165,0,0,166,0,0,0,0,0,167,168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,
80,169,81,82,0,0,37,0,0,0,0,0,0,0,0,170,0,0,171,0,0,0,0,682,172,157,158,173,0,
174,0,0,175,0,0,176,177,0,77,0,159,0,0,0,160,0,0,0,0,0,0,161,162,163,164,0,0,0,0,
165,0,0,166,0,0,0,0,0,167,168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,157,158,0,0,0,34,
80,169,81,82,0,0,37,77,0,159,701,0,0,160,0,170,0,0,171,0,161,162,163,164,172,0,0,
173,165,174,0,166,175,0,0,176,177,167,168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,157,
158,0,0,0,34,80,169,81,82,0,0,37,77,0,159,703,0,0,160,0,170,0,0,171,0,161,162,
163,164,172,0,0,173,165,174,0,166,175,0,0,176,177,167,168,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,157,158,0,0,0,34,80,169,81,82,0,0,37,77,0,159,0,0,0,160,0,170,797,0,
171,0,161,162,163,164,172,0,0,173,165,174,0,166,175,0,0,176,177,167,168,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,157,158,0,0,0,34,80,169,81,82,0,0,37,77,0,159,0,0,0,160,
0,170,800,0,171,0,161,162,163,164,172,0,0,173,165,174,0,166,175,0,0,176,177,167,
168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,157,158,0,0,0,34,80,169,81,82,0,0,37,77,0,
159,803,0,0,160,0,170,0,0,171,0,161,162,163,164,172,0,0,173,165,174,0,166,175,0,
0,176,177,167,168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,157,158,0,0,0,34,80,169,81,82,
0,0,37,77,0,159,0,0,0,160,0,170,871,0,171,0,161,162,163,164,172,0,0,173,165,174,
0,166,175,0,0,176,177,167,168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,157,158,0,0,0,34,
80,169,81,82,0,0,37,77,0,159,0,0,0,160,0,170,875,0,171,0,161,162,163,164,172,0,0,
173,165,174,0,166,175,0,0,176,177,167,168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,157,
158,0,0,0,34,80,169,81,82,0,0,37,77,0,159,0,0,0,160,0,170,877,0,171,0,161,162,
163,164,172,0,0,173,165,174,0,166,175,0,0,176,177,167,168,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,157,158,0,0,0,34,80,169,81,82,0,0,37,77,0,159,0,0,0,160,0,170,932,0,
171,0,161,162,163,164,172,0,0,173,165,174,0,166,175,0,0,176,177,167,168,0,0,0,0,
157,158,0,0,0,0,0,0,0,0,0,0,0,77,0,0,0,34,80,169,81,82,0,0,37,0,161,162,163,164,
0,0,0,170,0,0,171,166,0,0,0,0,172,167,168,173,0,174,0,0,175,0,0,176,177,0,0,0,0,
0,0,157,158,0,0,0,34,80,169,81,82,0,0,37,77,0,0,0,304,0,0,0,170,0,0,171,0,161,
162,163,164,172,0,0,173,0,174,0,166,175,0,0,176,177,167,168,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,157,158,0,0,0,34,80,169,81,82,0,0,37,77,0,0,0,645,0,0,0,170,0,0,171,
0,161,162,163,164,172,0,0,173,0,174,0,166,175,0,0,176,177,167,168,0,0,0,0,157,
158,0,0,0,0,0,0,0,0,0,0,0,77,0,0,0,34,80,169,81,82,0,0,37,0,161,162,163,164,0,0,
0,299,0,0,171,166,0,0,0,0,172,167,168,173,0,174,0,0,175,0,0,176,177,0,0,0,0,0,0,
157,158,0,0,0,34,80,169,81,82,0,0,37,77,0,0,0,0,0,0,0,313,0,0,171,0,161,162,163,
164,172,0,0,173,0,174,0,166,175,0,0,176,177,167,168,0,0,0,0,157,158,0,0,0,0,0,0,
0,0,0,0,0,77,0,0,0,34,80,169,81,82,0,0,37,0,161,162,163,164,0,0,0,170,0,0,171,
166,0,0,0,0,172,167,168,173,0,174,0,0,175,0,0,176,177,0,0,0,0,0,0,0,0,0,0,0,34,
80,169,81,82,0,0,37,0,0,0,0,0,0,0,0,840,0,0,171,0,0,0,0,0,172,0,0,173,0,174,0,0,
175,0,0,176,177,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,22,0,0,0,0,0,0,0,0,0,0,0,27,28,0,22,0,0,29,30,31,0,0,0,0,0,0,27,
28,0,0,0,0,29,30,31,0,0,206,0,0,0,0,0,0,0,0,0,0,0,0,278,0,0,279,0,0,0,0,35,36,0,
38,39,40,280,0,0,0,0,0,0,42,43,35,36,0,38,39,40,0,207,0,0,0,0,0,42,43,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,0,0,0,
0,0,0,0,0,0,0,0,27,28,0,22,0,0,29,30,31,0,0,0,0,0,0,27,28,0,0,0,0,29,30,31,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,278,0,0,0,0,0,0,0,35,36,0,38,39,40,280,618,0,0,0,0,0,42,
43,35,36,0,38,39,40,0,0,0,0,0,0,0,42,43,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,0,0,0,0,0,0,0,0,0,0,0,27,28,0,22,0,0,
29,30,31,0,0,0,0,0,0,27,28,0,0,0,0,29,30,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,35,36,0,38,39,40,0,0,0,0,0,0,0,42,43,35,0,0,0,39,40,0,0,0,0,0,0,0,42,
43};static short Cyc_yycheck[5466]={0,105,63,327,263,264,392,266,325,136,331,332,
160,159,136,337,338,119,160,330,471,21,172,71,174,175,238,170,463,61,425,0,357,
209,145,64,361,339,340,136,152,41,328,193,360,538,89,408,48,404,5,6,21,77,54,55,
56,316,58,333,334,335,336,17,64,65,77,60,105,111,0,113,120,121,117,430,91,91,115,
48,93,91,468,144,98,54,55,56,373,58,119,21,91,107,118,64,111,492,98,99,461,111,
127,96,133,64,557,118,111,470,103,111,112,113,114,67,551,102,48,119,21,104,107,
287,54,55,56,41,58,98,99,131,448,133,64,586,111,91,121,53,140,166,111,112,113,
114,207,147,294,104,119,71,645,54,55,56,299,58,301,23,119,211,131,64,133,96,491,
96,98,99,313,111,131,104,133,104,226,102,147,204,292,111,112,113,114,105,283,102,
102,119,585,341,342,343,226,72,73,72,73,226,120,131,96,133,380,381,382,91,92,209,
239,90,244,97,78,109,667,147,119,280,134,246,48,87,95,96,97,387,679,91,131,107,
133,96,111,106,111,107,114,239,209,91,92,120,404,120,109,97,108,111,214,215,114,
67,68,69,23,102,103,211,91,121,713,263,264,108,266,718,107,91,239,102,98,99,226,
591,98,108,121,425,239,112,90,495,91,111,112,113,114,289,97,155,244,107,263,264,
119,266,103,113,114,91,107,628,263,264,410,266,98,239,103,78,476,103,111,102,316,
83,105,101,87,147,286,237,103,107,290,706,105,111,0,392,114,263,264,112,266,91,
239,102,365,93,105,97,98,99,492,316,633,103,91,265,22,23,456,25,506,316,719,105,
503,102,263,264,105,266,112,116,91,401,111,41,404,658,401,98,125,126,48,103,380,
381,382,53,107,107,316,110,155,389,769,61,424,91,91,559,66,424,430,102,98,401,
463,403,170,75,431,468,78,107,105,380,381,382,414,316,416,87,116,117,118,633,105,
105,424,108,537,456,98,99,112,198,102,385,885,562,107,203,205,585,440,111,112,
113,114,414,573,416,101,401,105,573,563,108,107,105,456,742,111,129,839,114,106,
574,134,562,845,91,119,116,121,440,424,97,98,99,573,147,125,126,480,440,152,107,
414,155,416,456,105,255,551,102,103,641,138,112,105,456,884,641,119,665,121,649,
375,669,370,106,477,373,440,480,106,610,102,124,108,729,682,113,114,618,901,105,
848,104,456,136,198,846,742,852,853,299,793,205,858,859,570,440,851,307,827,570,
106,311,107,313,105,581,480,108,584,614,581,456,226,107,860,861,326,561,587,849,
850,91,107,237,107,250,949,97,98,99,244,256,107,577,248,608,854,855,856,857,963,
107,845,751,414,102,416,107,105,76,238,265,362,102,762,208,105,210,473,370,101,
91,373,122,608,91,107,97,98,99,111,827,98,114,570,101,945,107,292,123,774,107,
626,103,114,581,773,124,584,242,116,117,118,91,105,608,91,108,792,97,98,99,63,64,
105,608,104,108,107,622,480,105,108,626,108,114,304,116,117,118,308,309,310,38,
434,435,105,91,438,108,440,319,540,107,608,323,544,824,105,107,102,108,452,105,
652,721,949,105,105,104,108,108,626,840,102,842,370,105,104,373,668,76,102,608,
103,105,104,355,862,863,864,104,360,681,815,102,91,92,105,94,95,105,105,98,108,
108,834,401,105,107,404,108,107,107,109,110,611,107,386,91,414,116,416,102,105,
102,900,108,105,124,424,102,105,102,105,108,430,105,432,104,108,107,410,105,449,
112,108,105,453,540,108,106,385,544,78,79,80,81,82,83,84,85,86,87,105,74,75,108,
401,793,658,70,71,940,91,408,626,410,948,108,448,112,106,116,117,118,480,108,91,
17,18,19,104,105,97,98,99,5,6,884,793,91,92,93,107,595,596,121,696,115,793,114,
101,116,117,118,104,91,380,381,382,845,104,97,98,99,77,495,461,91,22,23,888,107,
104,105,98,470,112,793,102,120,116,117,118,107,540,120,638,107,544,108,845,108,
116,117,118,104,105,123,124,111,845,157,105,159,160,658,793,108,538,74,75,167,
168,105,570,91,172,104,174,175,108,97,98,99,107,581,70,71,584,845,105,107,104,
105,564,102,38,193,766,767,116,117,118,696,105,575,112,103,700,701,107,703,103,
705,30,102,42,96,845,91,108,591,107,949,105,108,121,562,91,105,626,105,105,725,
76,570,104,108,573,108,105,108,104,96,105,115,88,103,108,91,92,108,94,95,949,107,
98,108,108,102,108,120,658,633,949,107,115,109,110,102,640,113,114,104,116,645,
120,111,648,103,103,773,124,115,112,25,693,108,108,108,108,108,699,949,102,691,
108,108,294,108,696,115,108,104,103,115,108,799,104,91,802,803,102,96,108,108,
107,105,92,108,104,108,949,28,103,721,107,107,76,325,122,327,328,329,330,331,332,
333,334,335,336,337,338,339,340,341,342,343,840,112,91,686,76,104,96,847,97,98,
99,729,101,105,25,108,120,104,107,105,104,102,92,108,742,865,104,116,117,118,107,
105,108,91,0,108,719,108,0,879,108,108,797,108,108,800,888,569,104,889,237,584,
807,808,686,45,242,66,622,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,144,
884,24,425,837,618,614,496,205,696,658,38,61,329,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,-
1,- 1,- 1,49,50,- 1,- 1,827,- 1,55,56,57,- 1,870,871,- 1,- 1,874,875,- 1,877,- 1,- 1,- 1,- 1,
76,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,- 1,- 1,91,92,888,94,95,- 1,492,98,- 1,834,96,
97,- 1,99,100,101,107,103,109,110,506,- 1,- 1,109,110,116,- 1,- 1,- 1,926,- 1,- 1,929,
124,931,932,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,954,955,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,50,51,52,53,54,55,56,
57,- 1,59,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,585,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,96,97,98,99,100,101,- 1,103,- 1,- 1,- 1,107,- 1,
109,110,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,119,- 1,121,- 1,- 1,124,- 1,641,127,128,- 1,- 1,- 1,- 1,
- 1,649,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,
29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,54,55,
56,57,58,59,- 1,61,62,25,26,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,96,97,98,99,100,101,102,103,- 1,- 1,- 1,107,
- 1,109,110,- 1,- 1,- 1,76,- 1,116,- 1,- 1,119,- 1,121,- 1,771,124,- 1,774,127,128,91,92,-
1,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,792,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,- 1,-
1,119,- 1,121,- 1,- 1,124,- 1,- 1,127,128,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,846,- 1,848,849,850,851,852,853,
854,855,856,857,858,859,860,861,862,863,864,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,900,29,30,31,32,33,
34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,54,55,56,57,58,59,- 1,
61,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,948,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,91,92,93,94,95,96,97,98,99,100,101,102,103,104,- 1,- 1,107,- 1,109,110,-
1,- 1,- 1,- 1,- 1,116,- 1,- 1,119,- 1,121,- 1,- 1,124,- 1,- 1,127,128,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,
39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,
- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,
93,94,95,96,97,98,99,100,101,102,103,- 1,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,
- 1,- 1,119,- 1,121,- 1,- 1,124,- 1,- 1,127,128,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,
45,- 1,- 1,- 1,49,50,51,52,53,54,55,56,57,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,96,97,98,
99,100,101,102,- 1,- 1,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,119,- 1,121,- 1,
- 1,124,- 1,- 1,127,128,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,50,51,52,53,54,
55,56,57,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,96,97,98,99,100,101,- 1,103,- 1,- 1,- 1,
107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,119,- 1,121,- 1,- 1,124,- 1,- 1,127,128,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,50,51,52,53,54,55,56,57,- 1,59,- 1,- 1,62,- 1,
- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
91,92,93,94,95,96,97,98,99,100,101,- 1,- 1,- 1,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,
116,- 1,- 1,119,- 1,121,- 1,- 1,124,- 1,- 1,127,128,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,
- 1,43,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,66,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,- 1,- 1,- 1,- 1,
96,97,98,99,100,101,- 1,- 1,104,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,117,118,0,
1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,55,56,
57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,- 1,- 1,- 1,- 1,96,97,98,99,100,101,- 1,- 1,104,- 1,- 1,107,- 1,
109,110,- 1,- 1,- 1,- 1,- 1,116,117,118,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,
45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,66,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,- 1,- 1,- 1,- 1,96,97,98,
99,100,101,- 1,- 1,- 1,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,117,118,1,- 1,3,4,5,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,65,66,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,-
1,- 1,91,- 1,- 1,- 1,- 1,96,97,98,99,100,101,37,- 1,104,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,
49,50,116,117,118,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,- 1,- 1,91,- 1,37,- 1,- 1,96,97,98,99,100,
101,102,- 1,- 1,49,50,107,- 1,109,110,55,56,57,114,- 1,116,117,118,- 1,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,- 1,- 1,- 1,- 1,37,- 1,- 1,
96,97,- 1,99,100,101,102,- 1,- 1,49,50,107,- 1,109,110,55,56,57,114,- 1,116,117,118,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,91,- 1,- 1,- 1,- 1,96,97,98,99,100,101,- 1,- 1,- 1,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,
- 1,116,117,118,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,- 1,- 1,49,50,- 1,- 1,- 1,
- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,96,97,- 1,99,100,101,- 1,103,37,- 1,106,- 1,-
1,109,110,- 1,45,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,96,97,- 1,99,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,25,26,27,38,29,30,31,32,
33,34,35,36,- 1,38,- 1,40,41,- 1,- 1,44,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,58,59,
- 1,61,62,- 1,- 1,- 1,76,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,- 1,- 1,91,92,- 1,94,95,- 1,
- 1,98,- 1,- 1,91,92,93,94,95,- 1,107,98,109,110,38,102,103,- 1,- 1,116,107,- 1,- 1,110,
- 1,- 1,- 1,124,- 1,116,- 1,- 1,119,- 1,121,25,26,124,- 1,- 1,127,128,32,- 1,- 1,- 1,- 1,- 1,
38,- 1,40,- 1,76,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,91,92,59,94,95,62,- 1,98,
- 1,- 1,- 1,68,69,- 1,- 1,- 1,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
124,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,38,103,104,- 1,- 1,107,- 1,- 1,110,- 1,- 1,113,
114,- 1,116,25,26,119,- 1,121,- 1,- 1,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,-
1,- 1,76,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,88,- 1,62,91,92,- 1,94,95,68,69,98,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,109,110,- 1,- 1,113,114,- 1,116,- 1,- 1,91,92,93,94,95,124,-
1,98,- 1,- 1,- 1,- 1,103,104,- 1,- 1,107,- 1,- 1,110,- 1,- 1,113,114,- 1,116,25,26,119,- 1,
121,- 1,- 1,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,
54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,103,104,- 1,- 1,
107,- 1,- 1,110,- 1,- 1,113,114,- 1,116,25,26,119,- 1,121,- 1,- 1,124,- 1,- 1,127,128,- 1,
38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,
- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,
92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,113,114,- 1,116,
25,26,119,- 1,121,- 1,- 1,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,115,116,25,26,119,- 1,121,- 1,- 1,124,- 1,- 1,127,
128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,
62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,
- 1,- 1,91,92,93,94,95,- 1,- 1,98,38,- 1,40,- 1,103,- 1,44,- 1,107,- 1,- 1,110,- 1,51,52,53,
54,116,- 1,- 1,119,59,121,- 1,62,124,- 1,- 1,127,128,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,38,- 1,40,102,- 1,- 1,
44,- 1,107,- 1,- 1,110,- 1,51,52,53,54,116,- 1,- 1,119,59,121,- 1,62,124,- 1,- 1,127,128,
68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,91,92,93,94,
95,- 1,- 1,98,38,- 1,40,- 1,- 1,- 1,44,- 1,107,108,- 1,110,- 1,51,52,53,54,116,- 1,- 1,119,
59,121,- 1,62,124,- 1,- 1,127,128,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,
110,- 1,- 1,- 1,- 1,115,116,25,26,119,- 1,121,- 1,- 1,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,
- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,
69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,91,92,93,94,95,
- 1,- 1,98,38,- 1,40,- 1,- 1,- 1,44,- 1,107,108,- 1,110,- 1,51,52,53,54,116,- 1,- 1,119,59,
121,- 1,62,124,- 1,- 1,127,128,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,110,-
1,- 1,- 1,- 1,115,116,25,26,119,- 1,121,- 1,- 1,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,
44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,
98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,115,116,25,26,119,- 1,121,- 1,
- 1,124,- 1,- 1,127,128,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,
- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,38,- 1,40,102,- 1,- 1,44,- 1,107,- 1,- 1,
110,- 1,51,52,53,54,116,- 1,- 1,119,59,121,- 1,62,124,- 1,- 1,127,128,68,69,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,38,
- 1,40,102,- 1,- 1,44,- 1,107,- 1,- 1,110,- 1,51,52,53,54,116,- 1,- 1,119,59,121,- 1,62,
124,- 1,- 1,127,128,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,
- 1,- 1,91,92,93,94,95,- 1,- 1,98,38,- 1,40,- 1,- 1,- 1,44,- 1,107,108,- 1,110,- 1,51,52,53,
54,116,- 1,- 1,119,59,121,- 1,62,124,- 1,- 1,127,128,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,38,- 1,40,- 1,- 1,- 1,44,
- 1,107,108,- 1,110,- 1,51,52,53,54,116,- 1,- 1,119,59,121,- 1,62,124,- 1,- 1,127,128,68,
69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,91,92,93,94,95,
- 1,- 1,98,38,- 1,40,102,- 1,- 1,44,- 1,107,- 1,- 1,110,- 1,51,52,53,54,116,- 1,- 1,119,59,
121,- 1,62,124,- 1,- 1,127,128,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,25,26,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,38,- 1,40,- 1,- 1,- 1,44,- 1,107,108,- 1,110,
- 1,51,52,53,54,116,- 1,- 1,119,59,121,- 1,62,124,- 1,- 1,127,128,68,69,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,38,- 1,40,
- 1,- 1,- 1,44,- 1,107,108,- 1,110,- 1,51,52,53,54,116,- 1,- 1,119,59,121,- 1,62,124,- 1,-
1,127,128,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,
91,92,93,94,95,- 1,- 1,98,38,- 1,40,- 1,- 1,- 1,44,- 1,107,108,- 1,110,- 1,51,52,53,54,
116,- 1,- 1,119,59,121,- 1,62,124,- 1,- 1,127,128,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,38,- 1,40,- 1,- 1,- 1,44,- 1,
107,108,- 1,110,- 1,51,52,53,54,116,- 1,- 1,119,59,121,- 1,62,124,- 1,- 1,127,128,68,69,
- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,91,92,93,94,95,- 1,
- 1,98,- 1,51,52,53,54,- 1,- 1,- 1,107,- 1,- 1,110,62,- 1,- 1,- 1,- 1,116,68,69,119,- 1,121,
- 1,- 1,124,- 1,- 1,127,128,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,
38,- 1,- 1,- 1,103,- 1,- 1,- 1,107,- 1,- 1,110,- 1,51,52,53,54,116,- 1,- 1,119,- 1,121,- 1,62,
124,- 1,- 1,127,128,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,
- 1,- 1,91,92,93,94,95,- 1,- 1,98,38,- 1,- 1,- 1,103,- 1,- 1,- 1,107,- 1,- 1,110,- 1,51,52,53,
54,116,- 1,- 1,119,- 1,121,- 1,62,124,- 1,- 1,127,128,68,69,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,- 1,51,52,53,54,- 1,- 1,
- 1,107,- 1,- 1,110,62,- 1,- 1,- 1,- 1,116,68,69,119,- 1,121,- 1,- 1,124,- 1,- 1,127,128,- 1,
- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
107,- 1,- 1,110,- 1,51,52,53,54,116,- 1,- 1,119,- 1,121,- 1,62,124,- 1,- 1,127,128,68,69,
- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,91,92,93,94,95,- 1,
- 1,98,- 1,51,52,53,54,- 1,- 1,- 1,107,- 1,- 1,110,62,- 1,- 1,- 1,- 1,116,68,69,119,- 1,121,
- 1,- 1,124,- 1,- 1,127,128,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,92,93,94,95,- 1,- 1,98,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,119,- 1,121,- 1,- 1,
124,- 1,- 1,127,128,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,37,- 1,- 1,55,56,
57,- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,88,- 1,- 1,91,- 1,- 1,- 1,- 1,96,97,- 1,99,100,101,102,- 1,- 1,- 1,- 1,- 1,- 1,
109,110,96,97,- 1,99,100,101,- 1,103,- 1,- 1,- 1,- 1,- 1,109,110,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,37,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,37,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,
55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,
97,- 1,99,100,101,102,88,- 1,- 1,- 1,- 1,- 1,109,110,96,97,- 1,99,100,101,- 1,- 1,- 1,- 1,-
1,- 1,- 1,109,110,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,37,- 1,- 1,55,56,57,
- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,97,- 1,99,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,
110,96,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110};char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible\000";
char Cyc_Yystack_overflow[21]="\000\000\000\000Yystack_overflow\000";void Cyc_yyerror(
struct _dynforward_ptr);int Cyc_yylex();static int Cyc_yychar=(int)'\000';void*Cyc_yylval=(
void*)Cyc_YYINITIALSVAL;static int Cyc_yynerrs=0;struct _tuple18{struct Cyc_Position_Segment*
f1;struct _tuple1*f2;int f3;};struct _tuple19{struct _dynforward_ptr f1;void*f2;};
static char _tmp43F[8]="stdcall";static char _tmp440[6]="cdecl";static char _tmp441[9]="fastcall";
static char _tmp442[9]="noreturn";static char _tmp443[6]="const";static char _tmp444[8]="aligned";
static char _tmp445[7]="packed";static char _tmp446[7]="shared";static char _tmp447[7]="unused";
static char _tmp448[5]="weak";static char _tmp449[10]="dllimport";static char _tmp44A[
10]="dllexport";static char _tmp44B[23]="no_instrument_function";static char _tmp44C[
12]="constructor";static char _tmp44D[11]="destructor";static char _tmp44E[22]="no_check_memory_usage";
static char _tmp44F[5]="pure";struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(){int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmpA41=10000;short*_tmpA42=(short*)
_cycalloc_atomic(_check_times(sizeof(short),_tmpA41));{unsigned int _tmpA43=
_tmpA41;unsigned int i;for(i=0;i < _tmpA43;i ++){_tmpA42[i]=(short)0;}}_tmpA42;});
int yyvsp_offset;void**yyvs=(void**)({unsigned int _tmpA3E=10000;void**_tmpA3F=(
void**)_cycalloc(_check_times(sizeof(void*),_tmpA3E));{unsigned int _tmpA40=
_tmpA3E;unsigned int i;for(i=0;i < _tmpA40;i ++){_tmpA3F[i]=Cyc_yylval;}}_tmpA3F;});
int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({unsigned int _tmpA3B=
10000;struct Cyc_Yyltype*_tmpA3C=(struct Cyc_Yyltype*)_cycalloc_atomic(
_check_times(sizeof(struct Cyc_Yyltype),_tmpA3B));{unsigned int _tmpA3D=_tmpA3B;
unsigned int i;for(i=0;i < _tmpA3D;i ++){_tmpA3C[i]=Cyc_yynewloc();}}_tmpA3C;});int
yystacksize=10000;void*yyval=Cyc_yylval;int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=
0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[
_check_known_subscript_notnull(10000,++ yyssp_offset)]=(short)yystate;if(
yyssp_offset >= yystacksize - 1){if(yystacksize >= 10000){Cyc_yyerror(({const char*
_tmp370="parser stack overflow";_tag_dynforward(_tmp370,sizeof(char),
_get_zero_arr_size(_tmp370,22));}));(int)_throw((void*)Cyc_Yystack_overflow);}}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(975,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex();
if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0  && Cyc_yychar
<= 356?(int)Cyc_yytranslate[_check_known_subscript_notnull(357,Cyc_yychar)]: 268;}
yyn +=yychar1;if((yyn < 0  || yyn > 5465) || Cyc_yycheck[
_check_known_subscript_notnull(5466,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(5466,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 974)
return 0;if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;if(yyerrstatus != 0)yyerrstatus --;yystate=yyn;goto
yynewstate;yydefault: yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(975,
yystate)];if(yyn == 0)goto yyerrlab;yyreduce: yylen=(int)Cyc_yyr2[
_check_known_subscript_notnull(490,yyn)];if(yylen > 0)yyval=yyvs[
_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){case
1: _LL1E1: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
break;case 2: _LL1E2: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp371;_push_handler(& _tmp371);{int _tmp373=0;if(setjmp(_tmp371.handler))
_tmp373=1;if(!_tmp373){x=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);;_pop_handler();}else{
void*_tmp372=(void*)_exn_thrown;void*_tmp375=_tmp372;_LL1E5: if(*((void**)_tmp375)
!= Cyc_Core_Failure)goto _LL1E7;_LL1E6: x=0;goto _LL1E4;_LL1E7:;_LL1E8:(void)_throw(
_tmp375);_LL1E4:;}}}{struct _handler_cons _tmp376;_push_handler(& _tmp376);{int
_tmp378=0;if(setjmp(_tmp376.handler))_tmp378=1;if(!_tmp378){y=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp377=(void*)_exn_thrown;void*_tmp37A=_tmp377;_LL1EA: if(*((void**)_tmp37A)
!= Cyc_Core_Failure)goto _LL1EC;_LL1EB: y=0;goto _LL1E9;_LL1EC:;_LL1ED:(void)_throw(
_tmp37A);_LL1E9:;}}}yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp37B=
_cycalloc(sizeof(*_tmp37B));_tmp37B[0]=({struct Cyc_DeclList_tok_struct _tmp37C;
_tmp37C.tag=Cyc_DeclList_tok;_tmp37C.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y);_tmp37C;});_tmp37B;});break;}
case 3: _LL1E3: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp37D=_cycalloc(
sizeof(*_tmp37D));_tmp37D[0]=({struct Cyc_DeclList_tok_struct _tmp37E;_tmp37E.tag=
Cyc_DeclList_tok;_tmp37E.f1=({struct Cyc_List_List*_tmp37F=_cycalloc(sizeof(*
_tmp37F));_tmp37F->hd=({struct Cyc_Absyn_Decl*_tmp380=_cycalloc(sizeof(*_tmp380));
_tmp380->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*_tmp381=_cycalloc(
sizeof(*_tmp381));_tmp381[0]=({struct Cyc_Absyn_Using_d_struct _tmp382;_tmp382.tag=
9;_tmp382.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp382.f2=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp382;});_tmp381;}));
_tmp380->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp380;});_tmp37F->tl=0;_tmp37F;});_tmp37E;});
_tmp37D;});Cyc_Lex_leave_using();break;case 4: _LL1EE: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp383=_cycalloc(sizeof(*_tmp383));_tmp383[0]=({struct Cyc_DeclList_tok_struct
_tmp384;_tmp384.tag=Cyc_DeclList_tok;_tmp384.f1=({struct Cyc_List_List*_tmp385=
_cycalloc(sizeof(*_tmp385));_tmp385->hd=({struct Cyc_Absyn_Decl*_tmp386=_cycalloc(
sizeof(*_tmp386));_tmp386->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp387=_cycalloc(sizeof(*_tmp387));_tmp387[0]=({struct Cyc_Absyn_Using_d_struct
_tmp388;_tmp388.tag=9;_tmp388.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp388.f2=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp388;});_tmp387;}));
_tmp386->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp386;});_tmp385->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp385;});_tmp384;});
_tmp383;});break;case 5: _LL1EF: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp389=_cycalloc(sizeof(*_tmp389));_tmp389[0]=({struct Cyc_DeclList_tok_struct
_tmp38A;_tmp38A.tag=Cyc_DeclList_tok;_tmp38A.f1=({struct Cyc_List_List*_tmp38B=
_cycalloc(sizeof(*_tmp38B));_tmp38B->hd=({struct Cyc_Absyn_Decl*_tmp38C=_cycalloc(
sizeof(*_tmp38C));_tmp38C->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp38D=_cycalloc(sizeof(*_tmp38D));_tmp38D[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp38E;_tmp38E.tag=8;_tmp38E.f1=({struct _dynforward_ptr*_tmp38F=_cycalloc(
sizeof(*_tmp38F));_tmp38F[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp38F;});_tmp38E.f2=
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp38E;});_tmp38D;}));_tmp38C->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp38C;});
_tmp38B->tl=0;_tmp38B;});_tmp38A;});_tmp389;});Cyc_Lex_leave_namespace();break;
case 6: _LL1F0: {struct _dynforward_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*
y;{struct _handler_cons _tmp390;_push_handler(& _tmp390);{int _tmp392=0;if(setjmp(
_tmp390.handler))_tmp392=1;if(!_tmp392){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp391=(void*)_exn_thrown;void*_tmp394=_tmp391;_LL1F3: if(*((void**)_tmp394)
!= Cyc_Core_Failure)goto _LL1F5;_LL1F4: nspace=({const char*_tmp395="";
_tag_dynforward(_tmp395,sizeof(char),_get_zero_arr_size(_tmp395,1));});goto
_LL1F2;_LL1F5:;_LL1F6:(void)_throw(_tmp394);_LL1F2:;}}}{struct _handler_cons
_tmp396;_push_handler(& _tmp396);{int _tmp398=0;if(setjmp(_tmp396.handler))_tmp398=
1;if(!_tmp398){x=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);;_pop_handler();}else{void*_tmp397=(void*)_exn_thrown;
void*_tmp39A=_tmp397;_LL1F8: if(*((void**)_tmp39A)!= Cyc_Core_Failure)goto _LL1FA;
_LL1F9: x=0;goto _LL1F7;_LL1FA:;_LL1FB:(void)_throw(_tmp39A);_LL1F7:;}}}{struct
_handler_cons _tmp39B;_push_handler(& _tmp39B);{int _tmp39D=0;if(setjmp(_tmp39B.handler))
_tmp39D=1;if(!_tmp39D){y=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{void*
_tmp39C=(void*)_exn_thrown;void*_tmp39F=_tmp39C;_LL1FD: if(*((void**)_tmp39F)!= 
Cyc_Core_Failure)goto _LL1FF;_LL1FE: y=0;goto _LL1FC;_LL1FF:;_LL200:(void)_throw(
_tmp39F);_LL1FC:;}}}yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp3A0=
_cycalloc(sizeof(*_tmp3A0));_tmp3A0[0]=({struct Cyc_DeclList_tok_struct _tmp3A1;
_tmp3A1.tag=Cyc_DeclList_tok;_tmp3A1.f1=({struct Cyc_List_List*_tmp3A2=_cycalloc(
sizeof(*_tmp3A2));_tmp3A2->hd=({struct Cyc_Absyn_Decl*_tmp3A3=_cycalloc(sizeof(*
_tmp3A3));_tmp3A3->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp3A4=
_cycalloc(sizeof(*_tmp3A4));_tmp3A4[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp3A5;_tmp3A5.tag=8;_tmp3A5.f1=({struct _dynforward_ptr*_tmp3A6=_cycalloc(
sizeof(*_tmp3A6));_tmp3A6[0]=nspace;_tmp3A6;});_tmp3A5.f2=x;_tmp3A5;});_tmp3A4;}));
_tmp3A3->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp3A3;});_tmp3A2->tl=y;_tmp3A2;});_tmp3A1;});
_tmp3A0;});break;}case 7: _LL1F1: if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp3A7="C";
_tag_dynforward(_tmp3A7,sizeof(char),_get_zero_arr_size(_tmp3A7,2));}))== 0)
yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));
_tmp3A8[0]=({struct Cyc_DeclList_tok_struct _tmp3A9;_tmp3A9.tag=Cyc_DeclList_tok;
_tmp3A9.f1=({struct Cyc_List_List*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));_tmp3AA->hd=({
struct Cyc_Absyn_Decl*_tmp3AB=_cycalloc(sizeof(*_tmp3AB));_tmp3AB->r=(void*)((
void*)({struct Cyc_Absyn_ExternC_d_struct*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));
_tmp3AC[0]=({struct Cyc_Absyn_ExternC_d_struct _tmp3AD;_tmp3AD.tag=10;_tmp3AD.f1=
Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp3AD;});_tmp3AC;}));_tmp3AB->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3AB;});
_tmp3AA->tl=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3AA;});_tmp3A9;});_tmp3A8;});else{if(Cyc_strcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),({const char*_tmp3AE="C include";_tag_dynforward(_tmp3AE,
sizeof(char),_get_zero_arr_size(_tmp3AE,10));}))!= 0)Cyc_Parse_err(({const char*
_tmp3AF="expecting \"C\" or \"C include\"";_tag_dynforward(_tmp3AF,sizeof(char),
_get_zero_arr_size(_tmp3AF,29));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp3B0=_cycalloc(sizeof(*_tmp3B0));_tmp3B0[0]=({
struct Cyc_DeclList_tok_struct _tmp3B1;_tmp3B1.tag=Cyc_DeclList_tok;_tmp3B1.f1=({
struct Cyc_List_List*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));_tmp3B2->hd=({struct Cyc_Absyn_Decl*
_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3->r=(void*)((void*)({struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp3B4=_cycalloc(sizeof(*_tmp3B4));_tmp3B4[0]=({struct Cyc_Absyn_ExternCinclude_d_struct
_tmp3B5;_tmp3B5.tag=11;_tmp3B5.f1=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3B5.f2=0;_tmp3B5;});
_tmp3B4;}));_tmp3B3->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3B3;});
_tmp3B2->tl=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3B2;});_tmp3B1;});_tmp3B0;});}break;case 8: _LL201: if(Cyc_strcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]),({const char*_tmp3B6="C include";_tag_dynforward(_tmp3B6,
sizeof(char),_get_zero_arr_size(_tmp3B6,10));}))!= 0)Cyc_Parse_err(({const char*
_tmp3B7="expecting \"C include\"";_tag_dynforward(_tmp3B7,sizeof(char),
_get_zero_arr_size(_tmp3B7,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));{struct Cyc_List_List*
exs=Cyc_yyget_YY7(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));
_tmp3B8[0]=({struct Cyc_DeclList_tok_struct _tmp3B9;_tmp3B9.tag=Cyc_DeclList_tok;
_tmp3B9.f1=({struct Cyc_List_List*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));_tmp3BA->hd=({
struct Cyc_Absyn_Decl*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));_tmp3BB->r=(void*)((
void*)({struct Cyc_Absyn_ExternCinclude_d_struct*_tmp3BC=_cycalloc(sizeof(*
_tmp3BC));_tmp3BC[0]=({struct Cyc_Absyn_ExternCinclude_d_struct _tmp3BD;_tmp3BD.tag=
11;_tmp3BD.f1=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp3BD.f2=exs;_tmp3BD;});_tmp3BC;}));_tmp3BB->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3BB;});
_tmp3BA->tl=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3BA;});_tmp3B9;});_tmp3B8;});break;}case 9: _LL202: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE[0]=({
struct Cyc_DeclList_tok_struct _tmp3BF;_tmp3BF.tag=Cyc_DeclList_tok;_tmp3BF.f1=({
struct Cyc_List_List*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));_tmp3C0->hd=({struct Cyc_Absyn_Decl*
_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1->r=(void*)((void*)0);_tmp3C1->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3C1;});_tmp3C0->tl=Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3C0;});_tmp3BF;});
_tmp3BE;});break;case 10: _LL203: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp3C2=_cycalloc(sizeof(*_tmp3C2));_tmp3C2[0]=({struct Cyc_DeclList_tok_struct
_tmp3C3;_tmp3C3.tag=Cyc_DeclList_tok;_tmp3C3.f1=({struct Cyc_List_List*_tmp3C4=
_cycalloc(sizeof(*_tmp3C4));_tmp3C4->hd=({struct Cyc_Absyn_Decl*_tmp3C5=_cycalloc(
sizeof(*_tmp3C5));_tmp3C5->r=(void*)((void*)1);_tmp3C5->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3C5;});
_tmp3C4->tl=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3C4;});_tmp3C3;});_tmp3C2;});break;case 11: _LL204: yyval=(void*)({
struct Cyc_DeclList_tok_struct*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6[0]=({
struct Cyc_DeclList_tok_struct _tmp3C7;_tmp3C7.tag=Cyc_DeclList_tok;_tmp3C7.f1=0;
_tmp3C7;});_tmp3C6;});break;case 12: _LL205: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 13: _LL206: yyval=(
void*)({struct Cyc_YY7_struct*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8[0]=({
struct Cyc_YY7_struct _tmp3C9;_tmp3C9.tag=Cyc_YY7;_tmp3C9.f1=0;_tmp3C9;});_tmp3C8;});
break;case 14: _LL207: yyval=(void*)({struct Cyc_YY7_struct*_tmp3CA=_cycalloc(
sizeof(*_tmp3CA));_tmp3CA[0]=({struct Cyc_YY7_struct _tmp3CB;_tmp3CB.tag=Cyc_YY7;
_tmp3CB.f1=({struct Cyc_List_List*_tmp3CC=_cycalloc(sizeof(*_tmp3CC));_tmp3CC->hd=({
struct _tuple18*_tmp3CD=_cycalloc(sizeof(*_tmp3CD));_tmp3CD->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3CD->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp3CD->f3=0;_tmp3CD;});_tmp3CC->tl=0;_tmp3CC;});_tmp3CB;});_tmp3CA;});break;
case 15: _LL208: yyval=(void*)({struct Cyc_YY7_struct*_tmp3CE=_cycalloc(sizeof(*
_tmp3CE));_tmp3CE[0]=({struct Cyc_YY7_struct _tmp3CF;_tmp3CF.tag=Cyc_YY7;_tmp3CF.f1=({
struct Cyc_List_List*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));_tmp3D0->hd=({struct
_tuple18*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3D1->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp3D1->f3=0;_tmp3D1;});_tmp3D0->tl=Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3D0;});_tmp3CF;});
_tmp3CE;});break;case 16: _LL209: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2[0]=({struct Cyc_DeclList_tok_struct
_tmp3D3;_tmp3D3.tag=Cyc_DeclList_tok;_tmp3D3.f1=({struct Cyc_List_List*_tmp3D4=
_cycalloc(sizeof(*_tmp3D4));_tmp3D4->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp3D5=_cycalloc(sizeof(*_tmp3D5));_tmp3D5[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp3D6;_tmp3D6.tag=1;_tmp3D6.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3D6;});_tmp3D5;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3D4->tl=0;
_tmp3D4;});_tmp3D3;});_tmp3D2;});break;case 17: _LL20A: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 18: _LL20B: yyval=(
void*)({struct Cyc_DeclList_tok_struct*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));
_tmp3D7[0]=({struct Cyc_DeclList_tok_struct _tmp3D8;_tmp3D8.tag=Cyc_DeclList_tok;
_tmp3D8.f1=0;_tmp3D8;});_tmp3D7;});break;case 19: _LL20C: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3D9=_cycalloc(sizeof(*_tmp3D9));_tmp3D9[0]=({struct Cyc_FnDecl_tok_struct
_tmp3DA;_tmp3DA.tag=Cyc_FnDecl_tok;_tmp3DA.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3DA;});
_tmp3D9;});break;case 20: _LL20D: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3DB=_cycalloc(sizeof(*_tmp3DB));_tmp3DB[0]=({struct Cyc_FnDecl_tok_struct
_tmp3DC;_tmp3DC.tag=Cyc_FnDecl_tok;_tmp3DC.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));_tmp3DD->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3DD;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3DC;});
_tmp3DB;});break;case 21: _LL20E: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3DE=_cycalloc(sizeof(*_tmp3DE));_tmp3DE[0]=({struct Cyc_FnDecl_tok_struct
_tmp3DF;_tmp3DF.tag=Cyc_FnDecl_tok;_tmp3DF.f1=Cyc_Parse_make_function(0,Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3DF;});
_tmp3DE;});break;case 22: _LL20F: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3E0=_cycalloc(sizeof(*_tmp3E0));_tmp3E0[0]=({struct Cyc_FnDecl_tok_struct
_tmp3E1;_tmp3E1.tag=Cyc_FnDecl_tok;_tmp3E1.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3E2=_cycalloc(sizeof(*_tmp3E2));_tmp3E2->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3E2;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E1;});
_tmp3E0;});break;case 23: _LL210: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3E3=_cycalloc(sizeof(*_tmp3E3));_tmp3E3[0]=({struct Cyc_FnDecl_tok_struct
_tmp3E4;_tmp3E4.tag=Cyc_FnDecl_tok;_tmp3E4.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));_tmp3E5->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3E5;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E4;});
_tmp3E3;});break;case 24: _LL211: yyval=(void*)({struct Cyc_FnDecl_tok_struct*
_tmp3E6=_cycalloc(sizeof(*_tmp3E6));_tmp3E6[0]=({struct Cyc_FnDecl_tok_struct
_tmp3E7;_tmp3E7.tag=Cyc_FnDecl_tok;_tmp3E7.f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3E8=_cycalloc(sizeof(*_tmp3E8));_tmp3E8->v=Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3E8;}),Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E7;});
_tmp3E6;});break;case 25: _LL212: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL213: Cyc_Lex_leave_using();
break;case 27: _LL214: Cyc_Lex_enter_namespace(({struct _dynforward_ptr*_tmp3E9=
_cycalloc(sizeof(*_tmp3E9));_tmp3E9[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3E9;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL215: Cyc_Lex_leave_namespace();
break;case 29: _LL216: yyval=(void*)({struct Cyc_DeclList_tok_struct*_tmp3EA=
_cycalloc(sizeof(*_tmp3EA));_tmp3EA[0]=({struct Cyc_DeclList_tok_struct _tmp3EB;
_tmp3EB.tag=Cyc_DeclList_tok;_tmp3EB.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp3EB;});
_tmp3EA;});break;case 30: _LL217: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp3EC=_cycalloc(sizeof(*_tmp3EC));_tmp3EC[0]=({struct Cyc_DeclList_tok_struct
_tmp3ED;_tmp3ED.tag=Cyc_DeclList_tok;_tmp3ED.f1=Cyc_Parse_make_declarations(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3ED;});
_tmp3EC;});break;case 31: _LL218: yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp3EE=_cycalloc(sizeof(*_tmp3EE));_tmp3EE[0]=({struct Cyc_DeclList_tok_struct
_tmp3EF;_tmp3EF.tag=Cyc_DeclList_tok;_tmp3EF.f1=({struct Cyc_List_List*_tmp3F0=
_cycalloc(sizeof(*_tmp3F0));_tmp3F0->hd=Cyc_Absyn_let_decl(Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3F0->tl=0;
_tmp3F0;});_tmp3EF;});_tmp3EE;});break;case 32: _LL219: {struct Cyc_List_List*
_tmp3F1=0;{struct Cyc_List_List*_tmp3F2=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);for(0;_tmp3F2 != 0;
_tmp3F2=_tmp3F2->tl){struct _dynforward_ptr*_tmp3F3=(struct _dynforward_ptr*)
_tmp3F2->hd;struct _tuple1*_tmp3F4=({struct _tuple1*_tmp3F7=_cycalloc(sizeof(*
_tmp3F7));_tmp3F7->f1=Cyc_Absyn_rel_ns_null;_tmp3F7->f2=_tmp3F3;_tmp3F7;});
struct Cyc_Absyn_Vardecl*_tmp3F5=Cyc_Absyn_new_vardecl(_tmp3F4,Cyc_Absyn_wildtyp(
0),0);_tmp3F1=({struct Cyc_List_List*_tmp3F6=_cycalloc(sizeof(*_tmp3F6));_tmp3F6->hd=
_tmp3F5;_tmp3F6->tl=_tmp3F1;_tmp3F6;});}}_tmp3F1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3F1);yyval=(void*)({struct Cyc_DeclList_tok_struct*
_tmp3F8=_cycalloc(sizeof(*_tmp3F8));_tmp3F8[0]=({struct Cyc_DeclList_tok_struct
_tmp3F9;_tmp3F9.tag=Cyc_DeclList_tok;_tmp3F9.f1=({struct Cyc_List_List*_tmp3FA=
_cycalloc(sizeof(*_tmp3FA));_tmp3FA->hd=Cyc_Absyn_letv_decl(_tmp3F1,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3FA->tl=0;
_tmp3FA;});_tmp3F9;});_tmp3F8;});break;}case 33: _LL21A: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 34: _LL21B: yyval=(
void*)({struct Cyc_DeclList_tok_struct*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));
_tmp3FB[0]=({struct Cyc_DeclList_tok_struct _tmp3FC;_tmp3FC.tag=Cyc_DeclList_tok;
_tmp3FC.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_DeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp3FC;});_tmp3FB;});
break;case 35: _LL21C: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp3FD=
_cycalloc(sizeof(*_tmp3FD));_tmp3FD[0]=({struct Cyc_DeclSpec_tok_struct _tmp3FE;
_tmp3FE.tag=Cyc_DeclSpec_tok;_tmp3FE.f1=({struct Cyc_Parse_Declaration_spec*
_tmp3FF=_cycalloc(sizeof(*_tmp3FF));_tmp3FF->sc=({struct Cyc_Core_Opt*_tmp400=
_cycalloc(sizeof(*_tmp400));_tmp400->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp400;});_tmp3FF->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp3FF->type_specs=0;_tmp3FF->is_inline=0;_tmp3FF->attributes=
0;_tmp3FF;});_tmp3FE;});_tmp3FD;});break;case 36: _LL21D: if((Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc != 0)({void*
_tmp401[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp402="Only one storage class is allowed in a declaration";_tag_dynforward(
_tmp402,sizeof(char),_get_zero_arr_size(_tmp402,51));}),_tag_dynforward(_tmp401,
sizeof(void*),0));});yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp403=
_cycalloc(sizeof(*_tmp403));_tmp403[0]=({struct Cyc_DeclSpec_tok_struct _tmp404;
_tmp404.tag=Cyc_DeclSpec_tok;_tmp404.f1=({struct Cyc_Parse_Declaration_spec*
_tmp405=_cycalloc(sizeof(*_tmp405));_tmp405->sc=({struct Cyc_Core_Opt*_tmp406=
_cycalloc(sizeof(*_tmp406));_tmp406->v=(void*)Cyc_yyget_StorageClass_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp406;});_tmp405->tq=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;
_tmp405->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->type_specs;_tmp405->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp405->attributes=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp405;});_tmp404;});_tmp403;});break;case 37: _LL21E: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407[0]=({struct Cyc_DeclSpec_tok_struct
_tmp408;_tmp408.tag=Cyc_DeclSpec_tok;_tmp408.f1=({struct Cyc_Parse_Declaration_spec*
_tmp409=_cycalloc(sizeof(*_tmp409));_tmp409->sc=0;_tmp409->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp409->type_specs=({struct Cyc_List_List*_tmp40A=
_cycalloc(sizeof(*_tmp40A));_tmp40A->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp40A->tl=0;_tmp40A;});
_tmp409->is_inline=0;_tmp409->attributes=0;_tmp409;});_tmp408;});_tmp407;});
break;case 38: _LL21F: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp40B=
_cycalloc(sizeof(*_tmp40B));_tmp40B[0]=({struct Cyc_DeclSpec_tok_struct _tmp40C;
_tmp40C.tag=Cyc_DeclSpec_tok;_tmp40C.f1=({struct Cyc_Parse_Declaration_spec*
_tmp40D=_cycalloc(sizeof(*_tmp40D));_tmp40D->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp40D->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp40D->type_specs=({
struct Cyc_List_List*_tmp40E=_cycalloc(sizeof(*_tmp40E));_tmp40E->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp40E->tl=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp40E;});
_tmp40D->is_inline=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->is_inline;_tmp40D->attributes=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp40D;});
_tmp40C;});_tmp40B;});break;case 39: _LL220: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp40F=_cycalloc(sizeof(*_tmp40F));_tmp40F[0]=({struct Cyc_DeclSpec_tok_struct
_tmp410;_tmp410.tag=Cyc_DeclSpec_tok;_tmp410.f1=({struct Cyc_Parse_Declaration_spec*
_tmp411=_cycalloc(sizeof(*_tmp411));_tmp411->sc=0;_tmp411->tq=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp411->type_specs=0;
_tmp411->is_inline=0;_tmp411->attributes=0;_tmp411;});_tmp410;});_tmp40F;});
break;case 40: _LL221: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*_tmp412=
_cycalloc(sizeof(*_tmp412));_tmp412[0]=({struct Cyc_DeclSpec_tok_struct _tmp413;
_tmp413.tag=Cyc_DeclSpec_tok;_tmp413.f1=({struct Cyc_Parse_Declaration_spec*
_tmp414=_cycalloc(sizeof(*_tmp414));_tmp414->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp414->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);
_tmp414->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->type_specs;_tmp414->is_inline=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp414->attributes=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp414;});_tmp413;});_tmp412;});break;case 41: _LL222: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp415=_cycalloc(sizeof(*_tmp415));_tmp415[0]=({struct Cyc_DeclSpec_tok_struct
_tmp416;_tmp416.tag=Cyc_DeclSpec_tok;_tmp416.f1=({struct Cyc_Parse_Declaration_spec*
_tmp417=_cycalloc(sizeof(*_tmp417));_tmp417->sc=0;_tmp417->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp417->type_specs=0;_tmp417->is_inline=1;_tmp417->attributes=
0;_tmp417;});_tmp416;});_tmp415;});break;case 42: _LL223: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp418=_cycalloc(sizeof(*_tmp418));_tmp418[0]=({struct Cyc_DeclSpec_tok_struct
_tmp419;_tmp419.tag=Cyc_DeclSpec_tok;_tmp419.f1=({struct Cyc_Parse_Declaration_spec*
_tmp41A=_cycalloc(sizeof(*_tmp41A));_tmp41A->sc=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp41A->tq=(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp41A->type_specs=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp41A->is_inline=1;_tmp41A->attributes=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp41A;});
_tmp419;});_tmp418;});break;case 43: _LL224: yyval=(void*)({struct Cyc_DeclSpec_tok_struct*
_tmp41B=_cycalloc(sizeof(*_tmp41B));_tmp41B[0]=({struct Cyc_DeclSpec_tok_struct
_tmp41C;_tmp41C.tag=Cyc_DeclSpec_tok;_tmp41C.f1=({struct Cyc_Parse_Declaration_spec*
_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D->sc=0;_tmp41D->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp41D->type_specs=0;_tmp41D->is_inline=0;_tmp41D->attributes=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp41D;});_tmp41C;});_tmp41B;});break;case 44: _LL225: yyval=(void*)({
struct Cyc_DeclSpec_tok_struct*_tmp41E=_cycalloc(sizeof(*_tmp41E));_tmp41E[0]=({
struct Cyc_DeclSpec_tok_struct _tmp41F;_tmp41F.tag=Cyc_DeclSpec_tok;_tmp41F.f1=({
struct Cyc_Parse_Declaration_spec*_tmp420=_cycalloc(sizeof(*_tmp420));_tmp420->sc=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;
_tmp420->tq=(Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->tq;_tmp420->type_specs=(Cyc_yyget_DeclSpec_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp420->is_inline=(
Cyc_yyget_DeclSpec_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;
_tmp420->attributes=Cyc_List_imp_append(Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_DeclSpec_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp420;});
_tmp41F;});_tmp41E;});break;case 45: _LL226: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421[0]=({struct Cyc_StorageClass_tok_struct
_tmp422;_tmp422.tag=Cyc_StorageClass_tok;_tmp422.f1=(void*)((void*)4);_tmp422;});
_tmp421;});break;case 46: _LL227: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423[0]=({struct Cyc_StorageClass_tok_struct
_tmp424;_tmp424.tag=Cyc_StorageClass_tok;_tmp424.f1=(void*)((void*)5);_tmp424;});
_tmp423;});break;case 47: _LL228: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425[0]=({struct Cyc_StorageClass_tok_struct
_tmp426;_tmp426.tag=Cyc_StorageClass_tok;_tmp426.f1=(void*)((void*)3);_tmp426;});
_tmp425;});break;case 48: _LL229: yyval=(void*)({struct Cyc_StorageClass_tok_struct*
_tmp427=_cycalloc(sizeof(*_tmp427));_tmp427[0]=({struct Cyc_StorageClass_tok_struct
_tmp428;_tmp428.tag=Cyc_StorageClass_tok;_tmp428.f1=(void*)((void*)1);_tmp428;});
_tmp427;});break;case 49: _LL22A: if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp429="C";
_tag_dynforward(_tmp429,sizeof(char),_get_zero_arr_size(_tmp429,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp42A="only extern or extern \"C\" is allowed";_tag_dynforward(
_tmp42A,sizeof(char),_get_zero_arr_size(_tmp42A,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_StorageClass_tok_struct*_tmp42B=_cycalloc(sizeof(*_tmp42B));_tmp42B[0]=({
struct Cyc_StorageClass_tok_struct _tmp42C;_tmp42C.tag=Cyc_StorageClass_tok;
_tmp42C.f1=(void*)((void*)2);_tmp42C;});_tmp42B;});break;case 50: _LL22B: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp42D=_cycalloc(sizeof(*_tmp42D));
_tmp42D[0]=({struct Cyc_StorageClass_tok_struct _tmp42E;_tmp42E.tag=Cyc_StorageClass_tok;
_tmp42E.f1=(void*)((void*)0);_tmp42E;});_tmp42D;});break;case 51: _LL22C: yyval=(
void*)({struct Cyc_StorageClass_tok_struct*_tmp42F=_cycalloc(sizeof(*_tmp42F));
_tmp42F[0]=({struct Cyc_StorageClass_tok_struct _tmp430;_tmp430.tag=Cyc_StorageClass_tok;
_tmp430.f1=(void*)((void*)6);_tmp430;});_tmp42F;});break;case 52: _LL22D: yyval=(
void*)({struct Cyc_AttributeList_tok_struct*_tmp431=_cycalloc(sizeof(*_tmp431));
_tmp431[0]=({struct Cyc_AttributeList_tok_struct _tmp432;_tmp432.tag=Cyc_AttributeList_tok;
_tmp432.f1=0;_tmp432;});_tmp431;});break;case 53: _LL22E: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 54: _LL22F: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;case 55: _LL230:
yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp433=_cycalloc(sizeof(*
_tmp433));_tmp433[0]=({struct Cyc_AttributeList_tok_struct _tmp434;_tmp434.tag=Cyc_AttributeList_tok;
_tmp434.f1=({struct Cyc_List_List*_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435->hd=(
void*)Cyc_yyget_Attribute_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp435->tl=0;_tmp435;});_tmp434;});_tmp433;});break;case 56:
_LL231: yyval=(void*)({struct Cyc_AttributeList_tok_struct*_tmp436=_cycalloc(
sizeof(*_tmp436));_tmp436[0]=({struct Cyc_AttributeList_tok_struct _tmp437;_tmp437.tag=
Cyc_AttributeList_tok;_tmp437.f1=({struct Cyc_List_List*_tmp438=_cycalloc(sizeof(*
_tmp438));_tmp438->hd=(void*)Cyc_yyget_Attribute_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp438->tl=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp438;});_tmp437;});
_tmp436;});break;case 57: _LL232: {static struct Cyc_Absyn_Aligned_att_struct
att_aligned={1,- 1};static struct _tuple19 att_map[17]={{{_tmp43F,_tmp43F + 8},(void*)
0},{{_tmp440,_tmp440 + 6},(void*)1},{{_tmp441,_tmp441 + 9},(void*)2},{{_tmp442,
_tmp442 + 9},(void*)3},{{_tmp443,_tmp443 + 6},(void*)4},{{_tmp444,_tmp444 + 8},(
void*)& att_aligned},{{_tmp445,_tmp445 + 7},(void*)5},{{_tmp446,_tmp446 + 7},(void*)
7},{{_tmp447,_tmp447 + 7},(void*)8},{{_tmp448,_tmp448 + 5},(void*)9},{{_tmp449,
_tmp449 + 10},(void*)10},{{_tmp44A,_tmp44A + 10},(void*)11},{{_tmp44B,_tmp44B + 23},(
void*)12},{{_tmp44C,_tmp44C + 12},(void*)13},{{_tmp44D,_tmp44D + 11},(void*)14},{{
_tmp44E,_tmp44E + 22},(void*)15},{{_tmp44F,_tmp44F + 5},(void*)16}};struct
_dynforward_ptr _tmp439=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);if((((_get_dynforward_size(_tmp439,sizeof(char))> 4  && *((
const char*)_check_dynforward_subscript(_tmp439,sizeof(char),0))== '_') && *((
const char*)_check_dynforward_subscript(_tmp439,sizeof(char),1))== '_') && *((
const char*)_check_dynforward_subscript(_tmp439,sizeof(char),(int)(
_get_dynforward_size(_tmp439,sizeof(char))- 2)))== '_') && *((const char*)
_check_dynforward_subscript(_tmp439,sizeof(char),(int)(_get_dynforward_size(
_tmp439,sizeof(char))- 3)))== '_')_tmp439=(struct _dynforward_ptr)Cyc_substring((
struct _dynforward_ptr)_tmp439,2,_get_dynforward_size(_tmp439,sizeof(char))- 5);{
int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((struct _dynforward_ptr)_tmp439,(struct
_dynforward_ptr)(att_map[i]).f1)== 0){yyval=(void*)({struct Cyc_Attribute_tok_struct*
_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A[0]=({struct Cyc_Attribute_tok_struct
_tmp43B;_tmp43B.tag=Cyc_Attribute_tok;_tmp43B.f1=(void*)(att_map[i]).f2;_tmp43B;});
_tmp43A;});break;}}if(i == 17){Cyc_Parse_err(({const char*_tmp43C="unrecognized attribute";
_tag_dynforward(_tmp43C,sizeof(char),_get_zero_arr_size(_tmp43C,23));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D[0]=({
struct Cyc_Attribute_tok_struct _tmp43E;_tmp43E.tag=Cyc_Attribute_tok;_tmp43E.f1=(
void*)((void*)1);_tmp43E;});_tmp43D;});}break;}}case 58: _LL233: yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp451=_cycalloc(sizeof(*_tmp451));_tmp451[0]=({
struct Cyc_Attribute_tok_struct _tmp452;_tmp452.tag=Cyc_Attribute_tok;_tmp452.f1=(
void*)((void*)4);_tmp452;});_tmp451;});break;case 59: _LL234: {struct
_dynforward_ptr _tmp453=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _tuple13 _tmp455;int _tmp456;struct _tuple13*_tmp454=
Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp455=*_tmp454;_tmp456=_tmp455.f2;{void*a;if(Cyc_zstrcmp((struct
_dynforward_ptr)_tmp453,({const char*_tmp457="regparm";_tag_dynforward(_tmp457,
sizeof(char),_get_zero_arr_size(_tmp457,8));}))== 0  || Cyc_zstrcmp((struct
_dynforward_ptr)_tmp453,({const char*_tmp458="__regparm__";_tag_dynforward(
_tmp458,sizeof(char),_get_zero_arr_size(_tmp458,12));}))== 0){if(_tmp456 < 0  || 
_tmp456 > 3)Cyc_Parse_err(({const char*_tmp459="regparm requires value between 0 and 3";
_tag_dynforward(_tmp459,sizeof(char),_get_zero_arr_size(_tmp459,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp45A=_cycalloc_atomic(sizeof(*_tmp45A));
_tmp45A[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp45B;_tmp45B.tag=0;_tmp45B.f1=
_tmp456;_tmp45B;});_tmp45A;});}else{if(Cyc_zstrcmp((struct _dynforward_ptr)
_tmp453,({const char*_tmp45C="aligned";_tag_dynforward(_tmp45C,sizeof(char),
_get_zero_arr_size(_tmp45C,8));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)
_tmp453,({const char*_tmp45D="__aligned__";_tag_dynforward(_tmp45D,sizeof(char),
_get_zero_arr_size(_tmp45D,12));}))== 0){if(_tmp456 < 0)Cyc_Parse_err(({const char*
_tmp45E="aligned requires positive power of two";_tag_dynforward(_tmp45E,sizeof(
char),_get_zero_arr_size(_tmp45E,39));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp456;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp45F="aligned requires positive power of two";
_tag_dynforward(_tmp45F,sizeof(char),_get_zero_arr_size(_tmp45F,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp460=_cycalloc_atomic(sizeof(*_tmp460));
_tmp460[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp461;_tmp461.tag=1;_tmp461.f1=
_tmp456;_tmp461;});_tmp460;});}}else{if(Cyc_zstrcmp((struct _dynforward_ptr)
_tmp453,({const char*_tmp462="initializes";_tag_dynforward(_tmp462,sizeof(char),
_get_zero_arr_size(_tmp462,12));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)
_tmp453,({const char*_tmp463="__initializes__";_tag_dynforward(_tmp463,sizeof(
char),_get_zero_arr_size(_tmp463,16));}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*
_tmp464=_cycalloc_atomic(sizeof(*_tmp464));_tmp464[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp465;_tmp465.tag=4;_tmp465.f1=_tmp456;_tmp465;});_tmp464;});else{Cyc_Parse_err(({
const char*_tmp466="unrecognized attribute";_tag_dynforward(_tmp466,sizeof(char),
_get_zero_arr_size(_tmp466,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp467=_cycalloc(sizeof(*_tmp467));
_tmp467[0]=({struct Cyc_Attribute_tok_struct _tmp468;_tmp468.tag=Cyc_Attribute_tok;
_tmp468.f1=(void*)a;_tmp468;});_tmp467;});break;}}case 60: _LL235: {struct
_dynforward_ptr _tmp469=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dynforward_ptr _tmp46A=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _dynforward_ptr)_tmp469,({const char*_tmp46B="section";_tag_dynforward(
_tmp46B,sizeof(char),_get_zero_arr_size(_tmp46B,8));}))== 0  || Cyc_zstrcmp((
struct _dynforward_ptr)_tmp469,({const char*_tmp46C="__section__";_tag_dynforward(
_tmp46C,sizeof(char),_get_zero_arr_size(_tmp46C,12));}))== 0)a=(void*)({struct
Cyc_Absyn_Section_att_struct*_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D[0]=({
struct Cyc_Absyn_Section_att_struct _tmp46E;_tmp46E.tag=2;_tmp46E.f1=_tmp46A;
_tmp46E;});_tmp46D;});else{Cyc_Parse_err(({const char*_tmp46F="unrecognized attribute";
_tag_dynforward(_tmp46F,sizeof(char),_get_zero_arr_size(_tmp46F,23));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp470=_cycalloc(sizeof(*_tmp470));
_tmp470[0]=({struct Cyc_Attribute_tok_struct _tmp471;_tmp471.tag=Cyc_Attribute_tok;
_tmp471.f1=(void*)a;_tmp471;});_tmp470;});break;}case 61: _LL236: {struct
_dynforward_ptr _tmp472=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dynforward_ptr _tmp473=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _dynforward_ptr)_tmp472,({const char*_tmp474="__mode__";_tag_dynforward(
_tmp474,sizeof(char),_get_zero_arr_size(_tmp474,9));}))== 0)a=(void*)({struct Cyc_Absyn_Mode_att_struct*
_tmp475=_cycalloc(sizeof(*_tmp475));_tmp475[0]=({struct Cyc_Absyn_Mode_att_struct
_tmp476;_tmp476.tag=5;_tmp476.f1=_tmp473;_tmp476;});_tmp475;});else{Cyc_Parse_err(({
const char*_tmp477="unrecognized attribute";_tag_dynforward(_tmp477,sizeof(char),
_get_zero_arr_size(_tmp477,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(void*)({struct Cyc_Attribute_tok_struct*_tmp478=_cycalloc(sizeof(*_tmp478));
_tmp478[0]=({struct Cyc_Attribute_tok_struct _tmp479;_tmp479.tag=Cyc_Attribute_tok;
_tmp479.f1=(void*)a;_tmp479;});_tmp478;});break;}case 62: _LL237: {struct
_dynforward_ptr _tmp47A=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dynforward_ptr _tmp47B=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp47D;struct
_tuple13 _tmp47C=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp47D=_tmp47C.f2;{int _tmp47F;struct _tuple13 _tmp47E=*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp47F=_tmp47E.f2;{
void*a=(void*)1;if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp47A,({const char*
_tmp480="format";_tag_dynforward(_tmp480,sizeof(char),_get_zero_arr_size(_tmp480,
7));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)_tmp47A,({const char*_tmp481="__format__";
_tag_dynforward(_tmp481,sizeof(char),_get_zero_arr_size(_tmp481,11));}))== 0){
if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp47B,({const char*_tmp482="printf";
_tag_dynforward(_tmp482,sizeof(char),_get_zero_arr_size(_tmp482,7));}))== 0)a=(
void*)({struct Cyc_Absyn_Format_att_struct*_tmp483=_cycalloc(sizeof(*_tmp483));
_tmp483[0]=({struct Cyc_Absyn_Format_att_struct _tmp484;_tmp484.tag=3;_tmp484.f1=(
void*)((void*)0);_tmp484.f2=_tmp47D;_tmp484.f3=_tmp47F;_tmp484;});_tmp483;});
else{if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp47B,({const char*_tmp485="scanf";
_tag_dynforward(_tmp485,sizeof(char),_get_zero_arr_size(_tmp485,6));}))== 0)a=(
void*)({struct Cyc_Absyn_Format_att_struct*_tmp486=_cycalloc(sizeof(*_tmp486));
_tmp486[0]=({struct Cyc_Absyn_Format_att_struct _tmp487;_tmp487.tag=3;_tmp487.f1=(
void*)((void*)1);_tmp487.f2=_tmp47D;_tmp487.f3=_tmp47F;_tmp487;});_tmp486;});
else{Cyc_Parse_err(({const char*_tmp488="unrecognized format type";
_tag_dynforward(_tmp488,sizeof(char),_get_zero_arr_size(_tmp488,25));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp489="unrecognized attribute";_tag_dynforward(_tmp489,sizeof(char),
_get_zero_arr_size(_tmp489,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=(void*)({
struct Cyc_Attribute_tok_struct*_tmp48A=_cycalloc(sizeof(*_tmp48A));_tmp48A[0]=({
struct Cyc_Attribute_tok_struct _tmp48B;_tmp48B.tag=Cyc_Attribute_tok;_tmp48B.f1=(
void*)a;_tmp48B;});_tmp48A;});break;}}}case 63: _LL238: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 64: _LL239: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp48C=_cycalloc(sizeof(*_tmp48C));
_tmp48C[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp48D;_tmp48D.tag=Cyc_TypeSpecifier_tok;
_tmp48D.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp48E=_cycalloc(sizeof(*_tmp48E));_tmp48E[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp48F;_tmp48F.tag=16;_tmp48F.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp48F.f2=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp48F.f3=0;_tmp48F.f4=
0;_tmp48F;});_tmp48E;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp48D;});
_tmp48C;});break;case 65: _LL23A: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp490=_cycalloc(sizeof(*_tmp490));_tmp490[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp491;_tmp491.tag=Cyc_TypeSpecifier_tok;_tmp491.f1=(void*)Cyc_Parse_type_spec((
void*)0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp491;});_tmp490;});break;case 66: _LL23B: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp492=_cycalloc(sizeof(*_tmp492));
_tmp492[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp493;_tmp493.tag=Cyc_TypeSpecifier_tok;
_tmp493.f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp493;});
_tmp492;});break;case 67: _LL23C: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp495;_tmp495.tag=Cyc_TypeSpecifier_tok;_tmp495.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp496=_cycalloc(sizeof(*_tmp496));
_tmp496->v=(void*)Cyc_yyget_Kind_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp496;}),0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp495;});
_tmp494;});break;case 68: _LL23D: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp498;_tmp498.tag=Cyc_TypeSpecifier_tok;_tmp498.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_char_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp498;});
_tmp497;});break;case 69: _LL23E: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp499=_cycalloc(sizeof(*_tmp499));_tmp499[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp49A;_tmp49A.tag=Cyc_TypeSpecifier_tok;_tmp49A.f1=(void*)((void*)({struct Cyc_Parse_Short_spec_struct*
_tmp49B=_cycalloc(sizeof(*_tmp49B));_tmp49B[0]=({struct Cyc_Parse_Short_spec_struct
_tmp49C;_tmp49C.tag=2;_tmp49C.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp49C;});
_tmp49B;}));_tmp49A;});_tmp499;});break;case 70: _LL23F: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp49D=_cycalloc(sizeof(*_tmp49D));_tmp49D[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp49E;_tmp49E.tag=Cyc_TypeSpecifier_tok;_tmp49E.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp49E;});
_tmp49D;});break;case 71: _LL240: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp49F=_cycalloc(sizeof(*_tmp49F));_tmp49F[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4A0;_tmp4A0.tag=Cyc_TypeSpecifier_tok;_tmp4A0.f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*
_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1[0]=({struct Cyc_Parse_Long_spec_struct
_tmp4A2;_tmp4A2.tag=3;_tmp4A2.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4A2;});
_tmp4A1;}));_tmp4A0;});_tmp49F;});break;case 72: _LL241: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4A3=_cycalloc(sizeof(*_tmp4A3));_tmp4A3[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4A4;_tmp4A4.tag=Cyc_TypeSpecifier_tok;_tmp4A4.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A4;});
_tmp4A3;});break;case 73: _LL242: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4A5=_cycalloc(sizeof(*_tmp4A5));_tmp4A5[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4A6;_tmp4A6.tag=Cyc_TypeSpecifier_tok;_tmp4A6.f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A6;});
_tmp4A5;});break;case 74: _LL243: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4A8;_tmp4A8.tag=Cyc_TypeSpecifier_tok;_tmp4A8.f1=(void*)((void*)({struct Cyc_Parse_Signed_spec_struct*
_tmp4A9=_cycalloc(sizeof(*_tmp4A9));_tmp4A9[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp4AA;_tmp4AA.tag=0;_tmp4AA.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4AA;});
_tmp4A9;}));_tmp4A8;});_tmp4A7;});break;case 75: _LL244: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4AB=_cycalloc(sizeof(*_tmp4AB));_tmp4AB[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4AC;_tmp4AC.tag=Cyc_TypeSpecifier_tok;_tmp4AC.f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp4AE;_tmp4AE.tag=1;_tmp4AE.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4AE;});
_tmp4AD;}));_tmp4AC;});_tmp4AB;});break;case 76: _LL245: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 77: _LL246: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 78: _LL247:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 79:
_LL248: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4AF=_cycalloc(
sizeof(*_tmp4AF));_tmp4AF[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4B0;_tmp4B0.tag=
Cyc_TypeSpecifier_tok;_tmp4B0.f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4B0;});
_tmp4AF;});break;case 80: _LL249: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4B1=_cycalloc(sizeof(*_tmp4B1));_tmp4B1[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4B2;_tmp4B2.tag=Cyc_TypeSpecifier_tok;_tmp4B2.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TupleType_struct*_tmp4B3=_cycalloc(sizeof(*_tmp4B3));
_tmp4B3[0]=({struct Cyc_Absyn_TupleType_struct _tmp4B4;_tmp4B4.tag=9;_tmp4B4.f1=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct
_tuple2*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp4B4;});_tmp4B3;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4B2;});_tmp4B1;});break;case 81: _LL24A: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));
_tmp4B5[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4B6;_tmp4B6.tag=Cyc_TypeSpecifier_tok;
_tmp4B6.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp4B7=_cycalloc(sizeof(*_tmp4B7));_tmp4B7[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp4B8;_tmp4B8.tag=15;_tmp4B8.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4B8;});_tmp4B7;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4B6;});_tmp4B5;});break;case 82: _LL24B: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4B9=_cycalloc(sizeof(*_tmp4B9));
_tmp4B9[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4BA;_tmp4BA.tag=Cyc_TypeSpecifier_tok;
_tmp4BA.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp4BB=_cycalloc(sizeof(*_tmp4BB));_tmp4BB[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp4BC;_tmp4BC.tag=14;_tmp4BC.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4BC;});_tmp4BB;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4BA;});_tmp4B9;});break;case 83: _LL24C: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4BD=_cycalloc(sizeof(*_tmp4BD));
_tmp4BD[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4BE;_tmp4BE.tag=Cyc_TypeSpecifier_tok;
_tmp4BE.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_struct*
_tmp4BF=_cycalloc(sizeof(*_tmp4BF));_tmp4BF[0]=({struct Cyc_Absyn_TagType_struct
_tmp4C0;_tmp4C0.tag=17;_tmp4C0.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4C0;});_tmp4BF;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4BE;});_tmp4BD;});break;case 84: _LL24D: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4C1=_cycalloc(sizeof(*_tmp4C1));
_tmp4C1[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4C2;_tmp4C2.tag=Cyc_TypeSpecifier_tok;
_tmp4C2.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_struct*
_tmp4C3=_cycalloc(sizeof(*_tmp4C3));_tmp4C3[0]=({struct Cyc_Absyn_TagType_struct
_tmp4C4;_tmp4C4.tag=17;_tmp4C4.f1=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp4C5=_cycalloc(sizeof(*_tmp4C5));_tmp4C5->v=(void*)((void*)5);_tmp4C5;}),0);
_tmp4C4;});_tmp4C3;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4C2;});
_tmp4C1;});break;case 85: _LL24E: {struct _tuple13 _tmp4C7;int _tmp4C8;struct _tuple13*
_tmp4C6=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4C7=*_tmp4C6;_tmp4C8=_tmp4C7.f2;yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4C9=_cycalloc(sizeof(*_tmp4C9));_tmp4C9[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4CA;_tmp4CA.tag=Cyc_TypeSpecifier_tok;_tmp4CA.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TypeInt_struct*_tmp4CB=_cycalloc_atomic(sizeof(*_tmp4CB));
_tmp4CB[0]=({struct Cyc_Absyn_TypeInt_struct _tmp4CC;_tmp4CC.tag=18;_tmp4CC.f1=
_tmp4C8;_tmp4CC;});_tmp4CB;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4CA;});
_tmp4C9;});break;}case 86: _LL24F: yyval=(void*)({struct Cyc_Kind_tok_struct*_tmp4CD=
_cycalloc(sizeof(*_tmp4CD));_tmp4CD[0]=({struct Cyc_Kind_tok_struct _tmp4CE;
_tmp4CE.tag=Cyc_Kind_tok;_tmp4CE.f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4CE;});
_tmp4CD;});break;case 87: _LL250: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp4CF=_cycalloc(sizeof(*_tmp4CF));_tmp4CF[0]=({struct Cyc_TypeQual_tok_struct
_tmp4D0;_tmp4D0.tag=Cyc_TypeQual_tok;_tmp4D0.f1=({struct Cyc_Absyn_Tqual _tmp4D1;
_tmp4D1.print_const=1;_tmp4D1.q_volatile=0;_tmp4D1.q_restrict=0;_tmp4D1.real_const=
1;_tmp4D1.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4D1;});_tmp4D0;});_tmp4CF;});break;case 88: _LL251:
yyval=(void*)({struct Cyc_TypeQual_tok_struct*_tmp4D2=_cycalloc(sizeof(*_tmp4D2));
_tmp4D2[0]=({struct Cyc_TypeQual_tok_struct _tmp4D3;_tmp4D3.tag=Cyc_TypeQual_tok;
_tmp4D3.f1=({struct Cyc_Absyn_Tqual _tmp4D4;_tmp4D4.print_const=0;_tmp4D4.q_volatile=
1;_tmp4D4.q_restrict=0;_tmp4D4.real_const=0;_tmp4D4.loc=0;_tmp4D4;});_tmp4D3;});
_tmp4D2;});break;case 89: _LL252: yyval=(void*)({struct Cyc_TypeQual_tok_struct*
_tmp4D5=_cycalloc(sizeof(*_tmp4D5));_tmp4D5[0]=({struct Cyc_TypeQual_tok_struct
_tmp4D6;_tmp4D6.tag=Cyc_TypeQual_tok;_tmp4D6.f1=({struct Cyc_Absyn_Tqual _tmp4D7;
_tmp4D7.print_const=0;_tmp4D7.q_volatile=0;_tmp4D7.q_restrict=1;_tmp4D7.real_const=
0;_tmp4D7.loc=0;_tmp4D7;});_tmp4D6;});_tmp4D5;});break;case 90: _LL253: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp4D8=_cycalloc(sizeof(*_tmp4D8));_tmp4D8[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp4D9;_tmp4D9.tag=Cyc_TypeSpecifier_tok;
_tmp4D9.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp4DA=_cycalloc(
sizeof(*_tmp4DA));_tmp4DA[0]=({struct Cyc_Parse_Decl_spec_struct _tmp4DB;_tmp4DB.tag=
5;_tmp4DB.f1=({struct Cyc_Absyn_Decl*_tmp4DC=_cycalloc(sizeof(*_tmp4DC));_tmp4DC->r=(
void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp4DD=_cycalloc(sizeof(*_tmp4DD));
_tmp4DD[0]=({struct Cyc_Absyn_Enum_d_struct _tmp4DE;_tmp4DE.tag=6;_tmp4DE.f1=({
struct Cyc_Absyn_Enumdecl*_tmp4DF=_cycalloc(sizeof(*_tmp4DF));_tmp4DF->sc=(void*)((
void*)2);_tmp4DF->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp4DF->fields=({struct Cyc_Core_Opt*_tmp4E0=_cycalloc(
sizeof(*_tmp4E0));_tmp4E0->v=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4E0;});_tmp4DF;});
_tmp4DE;});_tmp4DD;}));_tmp4DC->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4DC;});
_tmp4DB;});_tmp4DA;}));_tmp4D9;});_tmp4D8;});break;case 91: _LL254: yyval=(void*)({
struct Cyc_TypeSpecifier_tok_struct*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));_tmp4E1[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp4E2;_tmp4E2.tag=Cyc_TypeSpecifier_tok;
_tmp4E2.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_EnumType_struct*
_tmp4E3=_cycalloc(sizeof(*_tmp4E3));_tmp4E3[0]=({struct Cyc_Absyn_EnumType_struct
_tmp4E4;_tmp4E4.tag=12;_tmp4E4.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E4.f2=0;_tmp4E4;});
_tmp4E3;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4E2;});_tmp4E1;});break;case 92: _LL255: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp4E5=_cycalloc(sizeof(*_tmp4E5));
_tmp4E5[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp4E6;_tmp4E6.tag=Cyc_TypeSpecifier_tok;
_tmp4E6.f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*_tmp4E7=_cycalloc(
sizeof(*_tmp4E7));_tmp4E7[0]=({struct Cyc_Parse_Type_spec_struct _tmp4E8;_tmp4E8.tag=
4;_tmp4E8.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmp4E9=
_cycalloc(sizeof(*_tmp4E9));_tmp4E9[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp4EA;_tmp4EA.tag=13;_tmp4EA.f1=Cyc_yyget_EnumfieldList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4EA;});_tmp4E9;}));
_tmp4E8.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4E8;});_tmp4E7;}));_tmp4E6;});_tmp4E5;});break;
case 93: _LL256: yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp4EB=_cycalloc(
sizeof(*_tmp4EB));_tmp4EB[0]=({struct Cyc_Enumfield_tok_struct _tmp4EC;_tmp4EC.tag=
Cyc_Enumfield_tok;_tmp4EC.f1=({struct Cyc_Absyn_Enumfield*_tmp4ED=_cycalloc(
sizeof(*_tmp4ED));_tmp4ED->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4ED->tag=0;_tmp4ED->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4ED;});_tmp4EC;});_tmp4EB;});break;case 94: _LL257:
yyval=(void*)({struct Cyc_Enumfield_tok_struct*_tmp4EE=_cycalloc(sizeof(*_tmp4EE));
_tmp4EE[0]=({struct Cyc_Enumfield_tok_struct _tmp4EF;_tmp4EF.tag=Cyc_Enumfield_tok;
_tmp4EF.f1=({struct Cyc_Absyn_Enumfield*_tmp4F0=_cycalloc(sizeof(*_tmp4F0));
_tmp4F0->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp4F0->tag=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F0->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4F0;});
_tmp4EF;});_tmp4EE;});break;case 95: _LL258: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp4F1=_cycalloc(sizeof(*_tmp4F1));_tmp4F1[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp4F2;_tmp4F2.tag=Cyc_EnumfieldList_tok;_tmp4F2.f1=({struct Cyc_List_List*
_tmp4F3=_cycalloc(sizeof(*_tmp4F3));_tmp4F3->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F3->tl=0;_tmp4F3;});
_tmp4F2;});_tmp4F1;});break;case 96: _LL259: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp4F4=_cycalloc(sizeof(*_tmp4F4));_tmp4F4[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp4F5;_tmp4F5.tag=Cyc_EnumfieldList_tok;_tmp4F5.f1=({struct Cyc_List_List*
_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4F6->tl=0;_tmp4F6;});
_tmp4F5;});_tmp4F4;});break;case 97: _LL25A: yyval=(void*)({struct Cyc_EnumfieldList_tok_struct*
_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7[0]=({struct Cyc_EnumfieldList_tok_struct
_tmp4F8;_tmp4F8.tag=Cyc_EnumfieldList_tok;_tmp4F8.f1=({struct Cyc_List_List*
_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9->hd=Cyc_yyget_Enumfield_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4F9->tl=Cyc_yyget_EnumfieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F9;});_tmp4F8;});
_tmp4F7;});break;case 98: _LL25B: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp4FB;_tmp4FB.tag=Cyc_TypeSpecifier_tok;_tmp4FB.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp4FC=_cycalloc(sizeof(*_tmp4FC));
_tmp4FC[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp4FD;_tmp4FD.tag=11;_tmp4FD.f1=(
void*)Cyc_yyget_StructOrUnion_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp4FD.f2=Cyc_yyget_AggrFieldDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4FD;});_tmp4FC;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4FB;});_tmp4FA;});break;case 99: _LL25C: {struct
Cyc_List_List*_tmp4FE=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp4FF=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp500=_cycalloc(sizeof(*_tmp500));_tmp500[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp501;_tmp501.tag=Cyc_TypeSpecifier_tok;
_tmp501.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp502=_cycalloc(
sizeof(*_tmp502));_tmp502[0]=({struct Cyc_Parse_Decl_spec_struct _tmp503;_tmp503.tag=
5;_tmp503.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4FE,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4FF,Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp503;});
_tmp502;}));_tmp501;});_tmp500;});break;}case 100: _LL25D: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp504=_cycalloc(sizeof(*_tmp504));_tmp504[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp505;_tmp505.tag=Cyc_TypeSpecifier_tok;_tmp505.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp506=_cycalloc(sizeof(*_tmp506));
_tmp506[0]=({struct Cyc_Absyn_AggrType_struct _tmp507;_tmp507.tag=10;_tmp507.f1=({
struct Cyc_Absyn_AggrInfo _tmp508;_tmp508.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp509=_cycalloc(sizeof(*_tmp509));_tmp509[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp50A;_tmp50A.tag=0;_tmp50A.f1=(void*)Cyc_yyget_StructOrUnion_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp50A.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp50A;});_tmp509;}));
_tmp508.targs=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp508;});_tmp507;});_tmp506;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp505;});
_tmp504;});break;case 101: _LL25E: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp50B=_cycalloc(sizeof(*_tmp50B));_tmp50B[0]=({struct Cyc_TypeList_tok_struct
_tmp50C;_tmp50C.tag=Cyc_TypeList_tok;_tmp50C.f1=0;_tmp50C;});_tmp50B;});break;
case 102: _LL25F: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp50D=_cycalloc(
sizeof(*_tmp50D));_tmp50D[0]=({struct Cyc_TypeList_tok_struct _tmp50E;_tmp50E.tag=
Cyc_TypeList_tok;_tmp50E.f1=Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp50E;});_tmp50D;});
break;case 103: _LL260: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*_tmp50F=
_cycalloc(sizeof(*_tmp50F));_tmp50F[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp510;_tmp510.tag=Cyc_StructOrUnion_tok;_tmp510.f1=(void*)((void*)0);_tmp510;});
_tmp50F;});break;case 104: _LL261: yyval=(void*)({struct Cyc_StructOrUnion_tok_struct*
_tmp511=_cycalloc(sizeof(*_tmp511));_tmp511[0]=({struct Cyc_StructOrUnion_tok_struct
_tmp512;_tmp512.tag=Cyc_StructOrUnion_tok;_tmp512.f1=(void*)((void*)1);_tmp512;});
_tmp511;});break;case 105: _LL262: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp513=_cycalloc(sizeof(*_tmp513));_tmp513[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp514;_tmp514.tag=Cyc_AggrFieldDeclList_tok;_tmp514.f1=0;_tmp514;});_tmp513;});
break;case 106: _LL263: yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp515=_cycalloc(sizeof(*_tmp515));_tmp515[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp516;_tmp516.tag=Cyc_AggrFieldDeclList_tok;_tmp516.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_AggrFieldDeclListList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));_tmp516;});_tmp515;});
break;case 107: _LL264: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp517=_cycalloc(sizeof(*_tmp517));_tmp517[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp518;_tmp518.tag=Cyc_AggrFieldDeclListList_tok;_tmp518.f1=({struct Cyc_List_List*
_tmp519=_cycalloc(sizeof(*_tmp519));_tmp519->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp519->tl=0;_tmp519;});
_tmp518;});_tmp517;});break;case 108: _LL265: yyval=(void*)({struct Cyc_AggrFieldDeclListList_tok_struct*
_tmp51A=_cycalloc(sizeof(*_tmp51A));_tmp51A[0]=({struct Cyc_AggrFieldDeclListList_tok_struct
_tmp51B;_tmp51B.tag=Cyc_AggrFieldDeclListList_tok;_tmp51B.f1=({struct Cyc_List_List*
_tmp51C=_cycalloc(sizeof(*_tmp51C));_tmp51C->hd=Cyc_yyget_AggrFieldDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp51C->tl=Cyc_yyget_AggrFieldDeclListList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp51C;});_tmp51B;});
_tmp51A;});break;case 109: _LL266: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp51D=_cycalloc(sizeof(*_tmp51D));_tmp51D[0]=({struct Cyc_InitDeclList_tok_struct
_tmp51E;_tmp51E.tag=Cyc_InitDeclList_tok;_tmp51E.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp51E;});_tmp51D;});
break;case 110: _LL267: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp51F=
_cycalloc(sizeof(*_tmp51F));_tmp51F[0]=({struct Cyc_InitDeclList_tok_struct
_tmp520;_tmp520.tag=Cyc_InitDeclList_tok;_tmp520.f1=({struct Cyc_List_List*
_tmp521=_cycalloc(sizeof(*_tmp521));_tmp521->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp521->tl=0;_tmp521;});
_tmp520;});_tmp51F;});break;case 111: _LL268: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp522=_cycalloc(sizeof(*_tmp522));_tmp522[0]=({struct Cyc_InitDeclList_tok_struct
_tmp523;_tmp523.tag=Cyc_InitDeclList_tok;_tmp523.f1=({struct Cyc_List_List*
_tmp524=_cycalloc(sizeof(*_tmp524));_tmp524->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp524->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp524;});_tmp523;});
_tmp522;});break;case 112: _LL269: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp525=_cycalloc(sizeof(*_tmp525));_tmp525[0]=({struct Cyc_InitDecl_tok_struct
_tmp526;_tmp526.tag=Cyc_InitDecl_tok;_tmp526.f1=({struct _tuple12*_tmp527=
_cycalloc(sizeof(*_tmp527));_tmp527->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp527->f2=0;_tmp527;});
_tmp526;});_tmp525;});break;case 113: _LL26A: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp528=_cycalloc(sizeof(*_tmp528));_tmp528[0]=({struct Cyc_InitDecl_tok_struct
_tmp529;_tmp529.tag=Cyc_InitDecl_tok;_tmp529.f1=({struct _tuple12*_tmp52A=
_cycalloc(sizeof(*_tmp52A));_tmp52A->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp52A->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp52A;});_tmp529;});_tmp528;});break;case 114: _LL26B: {struct _tuple15 _tmp52C;
struct Cyc_Absyn_Tqual _tmp52D;struct Cyc_List_List*_tmp52E;struct Cyc_List_List*
_tmp52F;struct _tuple15*_tmp52B=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp52C=*_tmp52B;_tmp52D=
_tmp52C.f1;_tmp52E=_tmp52C.f2;_tmp52F=_tmp52C.f3;if(_tmp52D.loc == 0)_tmp52D.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);{struct Cyc_List_List*_tmp531;struct Cyc_List_List*
_tmp532;struct _tuple0 _tmp530=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
Cyc_yyget_InitDeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));_tmp531=_tmp530.f1;_tmp532=_tmp530.f2;{void*_tmp533=Cyc_Parse_speclist2typ(
_tmp52E,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));struct Cyc_List_List*_tmp534=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(Cyc_Parse_apply_tmss(
_tmp52D,_tmp533,_tmp531,_tmp52F),_tmp532);yyval=(void*)({struct Cyc_AggrFieldDeclList_tok_struct*
_tmp535=_cycalloc(sizeof(*_tmp535));_tmp535[0]=({struct Cyc_AggrFieldDeclList_tok_struct
_tmp536;_tmp536.tag=Cyc_AggrFieldDeclList_tok;_tmp536.f1=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmp534);_tmp536;});_tmp535;});break;}}}case 115:
_LL26C: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp537=_cycalloc(
sizeof(*_tmp537));_tmp537[0]=({struct Cyc_QualSpecList_tok_struct _tmp538;_tmp538.tag=
Cyc_QualSpecList_tok;_tmp538.f1=({struct _tuple15*_tmp539=_cycalloc(sizeof(*
_tmp539));_tmp539->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp539->f2=({
struct Cyc_List_List*_tmp53A=_cycalloc(sizeof(*_tmp53A));_tmp53A->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp53A->tl=0;_tmp53A;});
_tmp539->f3=0;_tmp539;});_tmp538;});_tmp537;});break;case 116: _LL26D: yyval=(void*)({
struct Cyc_QualSpecList_tok_struct*_tmp53B=_cycalloc(sizeof(*_tmp53B));_tmp53B[0]=({
struct Cyc_QualSpecList_tok_struct _tmp53C;_tmp53C.tag=Cyc_QualSpecList_tok;
_tmp53C.f1=({struct _tuple15*_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp53D->f2=({struct Cyc_List_List*_tmp53E=_cycalloc(sizeof(*
_tmp53E));_tmp53E->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp53E->tl=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp53E;});_tmp53D->f3=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3;_tmp53D;});_tmp53C;});_tmp53B;});break;case 117: _LL26E: yyval=(
void*)({struct Cyc_QualSpecList_tok_struct*_tmp53F=_cycalloc(sizeof(*_tmp53F));
_tmp53F[0]=({struct Cyc_QualSpecList_tok_struct _tmp540;_tmp540.tag=Cyc_QualSpecList_tok;
_tmp540.f1=({struct _tuple15*_tmp541=_cycalloc(sizeof(*_tmp541));_tmp541->f1=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp541->f2=0;_tmp541->f3=
0;_tmp541;});_tmp540;});_tmp53F;});break;case 118: _LL26F: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp542=_cycalloc(sizeof(*_tmp542));_tmp542[0]=({struct Cyc_QualSpecList_tok_struct
_tmp543;_tmp543.tag=Cyc_QualSpecList_tok;_tmp543.f1=({struct _tuple15*_tmp544=
_cycalloc(sizeof(*_tmp544));_tmp544->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp544->f2=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp544->f3=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp544;});_tmp543;});
_tmp542;});break;case 119: _LL270: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp545=_cycalloc(sizeof(*_tmp545));_tmp545[0]=({struct Cyc_QualSpecList_tok_struct
_tmp546;_tmp546.tag=Cyc_QualSpecList_tok;_tmp546.f1=({struct _tuple15*_tmp547=
_cycalloc(sizeof(*_tmp547));_tmp547->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp547->f2=0;
_tmp547->f3=Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp547;});_tmp546;});_tmp545;});break;case 120: _LL271:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp548=_cycalloc(sizeof(*
_tmp548));_tmp548[0]=({struct Cyc_QualSpecList_tok_struct _tmp549;_tmp549.tag=Cyc_QualSpecList_tok;
_tmp549.f1=({struct _tuple15*_tmp54A=_cycalloc(sizeof(*_tmp54A));_tmp54A->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp54A->f2=(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp54A->f3=Cyc_List_append(
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);_tmp54A;});_tmp549;});
_tmp548;});break;case 121: _LL272: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B[0]=({struct Cyc_QualSpecList_tok_struct
_tmp54C;_tmp54C.tag=Cyc_QualSpecList_tok;_tmp54C.f1=({struct _tuple15*_tmp54D=
_cycalloc(sizeof(*_tmp54D));_tmp54D->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp54D->f2=({
struct Cyc_List_List*_tmp54E=_cycalloc(sizeof(*_tmp54E));_tmp54E->hd=(void*)Cyc_yyget_TypeSpecifier_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp54E->tl=0;_tmp54E;});
_tmp54D->f3=0;_tmp54D;});_tmp54C;});_tmp54B;});break;case 122: _LL273: yyval=(void*)({
struct Cyc_QualSpecList_tok_struct*_tmp54F=_cycalloc(sizeof(*_tmp54F));_tmp54F[0]=({
struct Cyc_QualSpecList_tok_struct _tmp550;_tmp550.tag=Cyc_QualSpecList_tok;
_tmp550.f1=({struct _tuple15*_tmp551=_cycalloc(sizeof(*_tmp551));_tmp551->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp551->f2=({struct Cyc_List_List*_tmp552=_cycalloc(sizeof(*
_tmp552));_tmp552->hd=(void*)Cyc_yyget_TypeSpecifier_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp552->tl=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp552;});_tmp551->f3=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3;_tmp551;});_tmp550;});_tmp54F;});break;case 123: _LL274: yyval=(
void*)({struct Cyc_QualSpecList_tok_struct*_tmp553=_cycalloc(sizeof(*_tmp553));
_tmp553[0]=({struct Cyc_QualSpecList_tok_struct _tmp554;_tmp554.tag=Cyc_QualSpecList_tok;
_tmp554.f1=({struct _tuple15*_tmp555=_cycalloc(sizeof(*_tmp555));_tmp555->f1=Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp555->f2=0;_tmp555->f3=
0;_tmp555;});_tmp554;});_tmp553;});break;case 124: _LL275: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp556=_cycalloc(sizeof(*_tmp556));_tmp556[0]=({struct Cyc_QualSpecList_tok_struct
_tmp557;_tmp557.tag=Cyc_QualSpecList_tok;_tmp557.f1=({struct _tuple15*_tmp558=
_cycalloc(sizeof(*_tmp558));_tmp558->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp558->f2=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp558->f3=(*Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp558;});_tmp557;});
_tmp556;});break;case 125: _LL276: yyval=(void*)({struct Cyc_QualSpecList_tok_struct*
_tmp559=_cycalloc(sizeof(*_tmp559));_tmp559[0]=({struct Cyc_QualSpecList_tok_struct
_tmp55A;_tmp55A.tag=Cyc_QualSpecList_tok;_tmp55A.f1=({struct _tuple15*_tmp55B=
_cycalloc(sizeof(*_tmp55B));_tmp55B->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp55B->f2=0;
_tmp55B->f3=Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp55B;});_tmp55A;});_tmp559;});break;case 126: _LL277:
yyval=(void*)({struct Cyc_QualSpecList_tok_struct*_tmp55C=_cycalloc(sizeof(*
_tmp55C));_tmp55C[0]=({struct Cyc_QualSpecList_tok_struct _tmp55D;_tmp55D.tag=Cyc_QualSpecList_tok;
_tmp55D.f1=({struct _tuple15*_tmp55E=_cycalloc(sizeof(*_tmp55E));_tmp55E->f1=(*
Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1;_tmp55E->f2=(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp55E->f3=Cyc_List_append(
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(*Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);_tmp55E;});_tmp55D;});
_tmp55C;});break;case 127: _LL278: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp55F=_cycalloc(sizeof(*_tmp55F));_tmp55F[0]=({struct Cyc_InitDeclList_tok_struct
_tmp560;_tmp560.tag=Cyc_InitDeclList_tok;_tmp560.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp560;});_tmp55F;});
break;case 128: _LL279: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*_tmp561=
_cycalloc(sizeof(*_tmp561));_tmp561[0]=({struct Cyc_InitDeclList_tok_struct
_tmp562;_tmp562.tag=Cyc_InitDeclList_tok;_tmp562.f1=({struct Cyc_List_List*
_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp563->tl=0;_tmp563;});
_tmp562;});_tmp561;});break;case 129: _LL27A: yyval=(void*)({struct Cyc_InitDeclList_tok_struct*
_tmp564=_cycalloc(sizeof(*_tmp564));_tmp564[0]=({struct Cyc_InitDeclList_tok_struct
_tmp565;_tmp565.tag=Cyc_InitDeclList_tok;_tmp565.f1=({struct Cyc_List_List*
_tmp566=_cycalloc(sizeof(*_tmp566));_tmp566->hd=Cyc_yyget_InitDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp566->tl=Cyc_yyget_InitDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp566;});_tmp565;});
_tmp564;});break;case 130: _LL27B: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp567=_cycalloc(sizeof(*_tmp567));_tmp567[0]=({struct Cyc_InitDecl_tok_struct
_tmp568;_tmp568.tag=Cyc_InitDecl_tok;_tmp568.f1=({struct _tuple12*_tmp569=
_cycalloc(sizeof(*_tmp569));_tmp569->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp569->f2=0;_tmp569;});
_tmp568;});_tmp567;});break;case 131: _LL27C: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp56A=_cycalloc(sizeof(*_tmp56A));_tmp56A[0]=({struct Cyc_InitDecl_tok_struct
_tmp56B;_tmp56B.tag=Cyc_InitDecl_tok;_tmp56B.f1=({struct _tuple12*_tmp56C=
_cycalloc(sizeof(*_tmp56C));_tmp56C->f1=({struct Cyc_Parse_Declarator*_tmp56D=
_cycalloc(sizeof(*_tmp56D));_tmp56D->id=({struct _tuple1*_tmp56E=_cycalloc(
sizeof(*_tmp56E));_tmp56E->f1=Cyc_Absyn_rel_ns_null;_tmp56E->f2=
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"",sizeof(char),1);
_tmp56E;});_tmp56D->tms=0;_tmp56D;});_tmp56C->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp56C;});_tmp56B;});
_tmp56A;});break;case 132: _LL27D: yyval=(void*)({struct Cyc_InitDecl_tok_struct*
_tmp570=_cycalloc(sizeof(*_tmp570));_tmp570[0]=({struct Cyc_InitDecl_tok_struct
_tmp571;_tmp571.tag=Cyc_InitDecl_tok;_tmp571.f1=({struct _tuple12*_tmp572=
_cycalloc(sizeof(*_tmp572));_tmp572->f1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp572->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp572;});_tmp571;});_tmp570;});break;case 133: _LL27E: {struct Cyc_List_List*
_tmp573=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(void*)({struct
Cyc_TypeSpecifier_tok_struct*_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574[0]=({
struct Cyc_TypeSpecifier_tok_struct _tmp575;_tmp575.tag=Cyc_TypeSpecifier_tok;
_tmp575.f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*_tmp576=_cycalloc(
sizeof(*_tmp576));_tmp576[0]=({struct Cyc_Parse_Decl_spec_struct _tmp577;_tmp577.tag=
5;_tmp577.f1=Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp573,({struct Cyc_Core_Opt*
_tmp578=_cycalloc(sizeof(*_tmp578));_tmp578->v=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp578;}),Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp577;});
_tmp576;}));_tmp575;});_tmp574;});break;}case 134: _LL27F: yyval=(void*)({struct Cyc_TypeSpecifier_tok_struct*
_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579[0]=({struct Cyc_TypeSpecifier_tok_struct
_tmp57A;_tmp57A.tag=Cyc_TypeSpecifier_tok;_tmp57A.f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp57B=_cycalloc(sizeof(*_tmp57B));
_tmp57B[0]=({struct Cyc_Absyn_TunionType_struct _tmp57C;_tmp57C.tag=2;_tmp57C.f1=({
struct Cyc_Absyn_TunionInfo _tmp57D;_tmp57D.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp57E=_cycalloc(sizeof(*_tmp57E));_tmp57E[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp57F;_tmp57F.tag=0;_tmp57F.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp580;
_tmp580.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp580.is_xtunion=Cyc_yyget_Bool_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp580;});_tmp57F;});
_tmp57E;}));_tmp57D.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp57D.rgn=(void*)Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp57D;});_tmp57C;});
_tmp57B;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp57A;});_tmp579;});break;case 135: _LL280: yyval=(
void*)({struct Cyc_TypeSpecifier_tok_struct*_tmp581=_cycalloc(sizeof(*_tmp581));
_tmp581[0]=({struct Cyc_TypeSpecifier_tok_struct _tmp582;_tmp582.tag=Cyc_TypeSpecifier_tok;
_tmp582.f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp584;_tmp584.tag=3;_tmp584.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp585;
_tmp585.field_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*
_tmp586=_cycalloc(sizeof(*_tmp586));_tmp586[0]=({struct Cyc_Absyn_UnknownTunionfield_struct
_tmp587;_tmp587.tag=0;_tmp587.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp588;
_tmp588.tunion_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp588.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp588.is_xtunion=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp588;});_tmp587;});
_tmp586;}));_tmp585.targs=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp585;});_tmp584;});
_tmp583;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp582;});_tmp581;});break;case 136: _LL281: yyval=(
void*)({struct Cyc_Bool_tok_struct*_tmp589=_cycalloc_atomic(sizeof(*_tmp589));
_tmp589[0]=({struct Cyc_Bool_tok_struct _tmp58A;_tmp58A.tag=Cyc_Bool_tok;_tmp58A.f1=
0;_tmp58A;});_tmp589;});break;case 137: _LL282: yyval=(void*)({struct Cyc_Bool_tok_struct*
_tmp58B=_cycalloc_atomic(sizeof(*_tmp58B));_tmp58B[0]=({struct Cyc_Bool_tok_struct
_tmp58C;_tmp58C.tag=Cyc_Bool_tok;_tmp58C.f1=1;_tmp58C;});_tmp58B;});break;case
138: _LL283: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*_tmp58D=_cycalloc(
sizeof(*_tmp58D));_tmp58D[0]=({struct Cyc_TunionFieldList_tok_struct _tmp58E;
_tmp58E.tag=Cyc_TunionFieldList_tok;_tmp58E.f1=({struct Cyc_List_List*_tmp58F=
_cycalloc(sizeof(*_tmp58F));_tmp58F->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp58F->tl=0;_tmp58F;});
_tmp58E;});_tmp58D;});break;case 139: _LL284: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp590=_cycalloc(sizeof(*_tmp590));_tmp590[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp591;_tmp591.tag=Cyc_TunionFieldList_tok;_tmp591.f1=({struct Cyc_List_List*
_tmp592=_cycalloc(sizeof(*_tmp592));_tmp592->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp592->tl=0;_tmp592;});
_tmp591;});_tmp590;});break;case 140: _LL285: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp593=_cycalloc(sizeof(*_tmp593));_tmp593[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp594;_tmp594.tag=Cyc_TunionFieldList_tok;_tmp594.f1=({struct Cyc_List_List*
_tmp595=_cycalloc(sizeof(*_tmp595));_tmp595->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp595->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp595;});_tmp594;});
_tmp593;});break;case 141: _LL286: yyval=(void*)({struct Cyc_TunionFieldList_tok_struct*
_tmp596=_cycalloc(sizeof(*_tmp596));_tmp596[0]=({struct Cyc_TunionFieldList_tok_struct
_tmp597;_tmp597.tag=Cyc_TunionFieldList_tok;_tmp597.f1=({struct Cyc_List_List*
_tmp598=_cycalloc(sizeof(*_tmp598));_tmp598->hd=Cyc_yyget_TunionField_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp598->tl=Cyc_yyget_TunionFieldList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp598;});_tmp597;});
_tmp596;});break;case 142: _LL287: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp599=_cycalloc(sizeof(*_tmp599));_tmp599[0]=({struct Cyc_Scope_tok_struct
_tmp59A;_tmp59A.tag=Cyc_Scope_tok;_tmp59A.f1=(void*)((void*)2);_tmp59A;});
_tmp599;});break;case 143: _LL288: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B[0]=({struct Cyc_Scope_tok_struct
_tmp59C;_tmp59C.tag=Cyc_Scope_tok;_tmp59C.f1=(void*)((void*)3);_tmp59C;});
_tmp59B;});break;case 144: _LL289: yyval=(void*)({struct Cyc_Scope_tok_struct*
_tmp59D=_cycalloc(sizeof(*_tmp59D));_tmp59D[0]=({struct Cyc_Scope_tok_struct
_tmp59E;_tmp59E.tag=Cyc_Scope_tok;_tmp59E.f1=(void*)((void*)0);_tmp59E;});
_tmp59D;});break;case 145: _LL28A: yyval=(void*)({struct Cyc_TunionField_tok_struct*
_tmp59F=_cycalloc(sizeof(*_tmp59F));_tmp59F[0]=({struct Cyc_TunionField_tok_struct
_tmp5A0;_tmp5A0.tag=Cyc_TunionField_tok;_tmp5A0.f1=({struct Cyc_Absyn_Tunionfield*
_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5A1->typs=0;_tmp5A1->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5A1->sc=(void*)Cyc_yyget_Scope_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5A1;});_tmp5A0;});
_tmp59F;});break;case 146: _LL28B: {struct Cyc_List_List*_tmp5A2=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_TunionField_tok_struct*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));_tmp5A3[0]=({
struct Cyc_TunionField_tok_struct _tmp5A4;_tmp5A4.tag=Cyc_TunionField_tok;_tmp5A4.f1=({
struct Cyc_Absyn_Tunionfield*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));_tmp5A5->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp5A5->typs=_tmp5A2;_tmp5A5->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5A5->sc=(
void*)Cyc_yyget_Scope_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]);_tmp5A5;});_tmp5A4;});_tmp5A3;});break;}case 147: _LL28C: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 148: _LL28D: yyval=(
void*)({struct Cyc_Declarator_tok_struct*_tmp5A6=_cycalloc(sizeof(*_tmp5A6));
_tmp5A6[0]=({struct Cyc_Declarator_tok_struct _tmp5A7;_tmp5A7.tag=Cyc_Declarator_tok;
_tmp5A7.f1=({struct Cyc_Parse_Declarator*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));
_tmp5A8->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp5A8->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp5A8;});
_tmp5A7;});_tmp5A6;});break;case 149: _LL28E: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 150: _LL28F: yyval=(
void*)({struct Cyc_Declarator_tok_struct*_tmp5A9=_cycalloc(sizeof(*_tmp5A9));
_tmp5A9[0]=({struct Cyc_Declarator_tok_struct _tmp5AA;_tmp5AA.tag=Cyc_Declarator_tok;
_tmp5AA.f1=({struct Cyc_Parse_Declarator*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));
_tmp5AB->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp5AB->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp5AB;});
_tmp5AA;});_tmp5A9;});break;case 151: _LL290: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5AC=_cycalloc(sizeof(*_tmp5AC));_tmp5AC[0]=({struct Cyc_Declarator_tok_struct
_tmp5AD;_tmp5AD.tag=Cyc_Declarator_tok;_tmp5AD.f1=({struct Cyc_Parse_Declarator*
_tmp5AE=_cycalloc(sizeof(*_tmp5AE));_tmp5AE->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5AE->tms=0;_tmp5AE;});
_tmp5AD;});_tmp5AC;});break;case 152: _LL291: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 153: _LL292: {
struct Cyc_Parse_Declarator*_tmp5AF=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5AF->tms=({struct Cyc_List_List*
_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp5B1=_cycalloc(sizeof(*_tmp5B1));_tmp5B1[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp5B2;_tmp5B2.tag=5;_tmp5B2.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp5B2.f2=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp5B2;});_tmp5B1;}));_tmp5B0->tl=_tmp5AF->tms;_tmp5B0;});
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 154:
_LL293: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5B3=_cycalloc(sizeof(*
_tmp5B3));_tmp5B3[0]=({struct Cyc_Declarator_tok_struct _tmp5B4;_tmp5B4.tag=Cyc_Declarator_tok;
_tmp5B4.f1=({struct Cyc_Parse_Declarator*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));
_tmp5B5->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5B5->tms=({struct Cyc_List_List*_tmp5B6=_cycalloc(
sizeof(*_tmp5B6));_tmp5B6->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp5B8;_tmp5B8.tag=0;_tmp5B8.f1=Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B8.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5B8;});
_tmp5B7;}));_tmp5B6->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5B6;});_tmp5B5;});
_tmp5B4;});_tmp5B3;});break;case 155: _LL294: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9[0]=({struct Cyc_Declarator_tok_struct
_tmp5BA;_tmp5BA.tag=Cyc_Declarator_tok;_tmp5BA.f1=({struct Cyc_Parse_Declarator*
_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5BB->tms=({
struct Cyc_List_List*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));_tmp5BC->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));
_tmp5BD[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp5BE;_tmp5BE.tag=1;_tmp5BE.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5BE.f2=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp5BE.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5BE;});_tmp5BD;}));_tmp5BC->tl=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5BC;});
_tmp5BB;});_tmp5BA;});_tmp5B9;});break;case 156: _LL295: {struct _tuple14 _tmp5C0;
struct Cyc_List_List*_tmp5C1;int _tmp5C2;struct Cyc_Absyn_VarargInfo*_tmp5C3;struct
Cyc_Core_Opt*_tmp5C4;struct Cyc_List_List*_tmp5C5;struct _tuple14*_tmp5BF=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5C0=*_tmp5BF;
_tmp5C1=_tmp5C0.f1;_tmp5C2=_tmp5C0.f2;_tmp5C3=_tmp5C0.f3;_tmp5C4=_tmp5C0.f4;
_tmp5C5=_tmp5C0.f5;yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5C6=
_cycalloc(sizeof(*_tmp5C6));_tmp5C6[0]=({struct Cyc_Declarator_tok_struct _tmp5C7;
_tmp5C7.tag=Cyc_Declarator_tok;_tmp5C7.f1=({struct Cyc_Parse_Declarator*_tmp5C8=
_cycalloc(sizeof(*_tmp5C8));_tmp5C8->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5C8->tms=({
struct Cyc_List_List*_tmp5C9=_cycalloc(sizeof(*_tmp5C9));_tmp5C9->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5CA=_cycalloc(sizeof(*_tmp5CA));
_tmp5CA[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5CB;_tmp5CB.tag=3;_tmp5CB.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp5CC=_cycalloc(sizeof(*
_tmp5CC));_tmp5CC[0]=({struct Cyc_Absyn_WithTypes_struct _tmp5CD;_tmp5CD.tag=1;
_tmp5CD.f1=_tmp5C1;_tmp5CD.f2=_tmp5C2;_tmp5CD.f3=_tmp5C3;_tmp5CD.f4=_tmp5C4;
_tmp5CD.f5=_tmp5C5;_tmp5CD;});_tmp5CC;}));_tmp5CB;});_tmp5CA;}));_tmp5C9->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp5C9;});_tmp5C8;});_tmp5C7;});_tmp5C6;});break;}case 157: _LL296:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));
_tmp5CE[0]=({struct Cyc_Declarator_tok_struct _tmp5CF;_tmp5CF.tag=Cyc_Declarator_tok;
_tmp5CF.f1=({struct Cyc_Parse_Declarator*_tmp5D0=_cycalloc(sizeof(*_tmp5D0));
_tmp5D0->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp5D0->tms=({struct Cyc_List_List*_tmp5D1=_cycalloc(
sizeof(*_tmp5D1));_tmp5D1->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp5D2=_cycalloc(sizeof(*_tmp5D2));_tmp5D2[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp5D3;_tmp5D3.tag=3;_tmp5D3.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp5D5;_tmp5D5.tag=1;_tmp5D5.f1=0;_tmp5D5.f2=0;_tmp5D5.f3=0;_tmp5D5.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5D5.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D5;});_tmp5D4;}));
_tmp5D3;});_tmp5D2;}));_tmp5D1->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5D1;});_tmp5D0;});
_tmp5CF;});_tmp5CE;});break;case 158: _LL297: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5D6=_cycalloc(sizeof(*_tmp5D6));_tmp5D6[0]=({struct Cyc_Declarator_tok_struct
_tmp5D7;_tmp5D7.tag=Cyc_Declarator_tok;_tmp5D7.f1=({struct Cyc_Parse_Declarator*
_tmp5D8=_cycalloc(sizeof(*_tmp5D8));_tmp5D8->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5D8->tms=({
struct Cyc_List_List*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));
_tmp5DA[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5DB;_tmp5DB.tag=3;_tmp5DB.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));
_tmp5DC[0]=({struct Cyc_Absyn_NoTypes_struct _tmp5DD;_tmp5DD.tag=0;_tmp5DD.f1=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5DD.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5DD;});
_tmp5DC;}));_tmp5DB;});_tmp5DA;}));_tmp5D9->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5D9;});_tmp5D8;});
_tmp5D7;});_tmp5D6;});break;case 159: _LL298: {struct Cyc_List_List*_tmp5DE=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp5DF=_cycalloc(sizeof(*_tmp5DF));_tmp5DF[0]=({
struct Cyc_Declarator_tok_struct _tmp5E0;_tmp5E0.tag=Cyc_Declarator_tok;_tmp5E0.f1=({
struct Cyc_Parse_Declarator*_tmp5E1=_cycalloc(sizeof(*_tmp5E1));_tmp5E1->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5E1->tms=({
struct Cyc_List_List*_tmp5E2=_cycalloc(sizeof(*_tmp5E2));_tmp5E2->hd=(void*)((
void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp5E3=_cycalloc(sizeof(*_tmp5E3));
_tmp5E3[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp5E4;_tmp5E4.tag=4;_tmp5E4.f1=
_tmp5DE;_tmp5E4.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5E4.f3=0;
_tmp5E4;});_tmp5E3;}));_tmp5E2->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5E2;});_tmp5E1;});
_tmp5E0;});_tmp5DF;});break;}case 160: _LL299: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5E5=_cycalloc(sizeof(*_tmp5E5));_tmp5E5[0]=({struct Cyc_Declarator_tok_struct
_tmp5E6;_tmp5E6.tag=Cyc_Declarator_tok;_tmp5E6.f1=({struct Cyc_Parse_Declarator*
_tmp5E7=_cycalloc(sizeof(*_tmp5E7));_tmp5E7->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp5E7->tms=({
struct Cyc_List_List*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));_tmp5E8->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp5E9=_cycalloc(sizeof(*_tmp5E9));
_tmp5E9[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp5EA;_tmp5EA.tag=5;_tmp5EA.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5EA.f2=Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5EA;});_tmp5E9;}));
_tmp5E8->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp5E8;});_tmp5E7;});_tmp5E6;});_tmp5E5;});break;case
161: _LL29A: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5EB=_cycalloc(
sizeof(*_tmp5EB));_tmp5EB[0]=({struct Cyc_Declarator_tok_struct _tmp5EC;_tmp5EC.tag=
Cyc_Declarator_tok;_tmp5EC.f1=({struct Cyc_Parse_Declarator*_tmp5ED=_cycalloc(
sizeof(*_tmp5ED));_tmp5ED->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5ED->tms=0;_tmp5ED;});
_tmp5EC;});_tmp5EB;});break;case 162: _LL29B: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE[0]=({struct Cyc_Declarator_tok_struct
_tmp5EF;_tmp5EF.tag=Cyc_Declarator_tok;_tmp5EF.f1=({struct Cyc_Parse_Declarator*
_tmp5F0=_cycalloc(sizeof(*_tmp5F0));_tmp5F0->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F0->tms=0;_tmp5F0;});
_tmp5EF;});_tmp5EE;});break;case 163: _LL29C: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 164: _LL29D: {
struct Cyc_Parse_Declarator*_tmp5F1=Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5F1->tms=({struct Cyc_List_List*
_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp5F3=_cycalloc(sizeof(*_tmp5F3));_tmp5F3[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp5F4;_tmp5F4.tag=5;_tmp5F4.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp5F4.f2=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp5F4;});_tmp5F3;}));_tmp5F2->tl=_tmp5F1->tms;_tmp5F2;});
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 165:
_LL29E: yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp5F5=_cycalloc(sizeof(*
_tmp5F5));_tmp5F5[0]=({struct Cyc_Declarator_tok_struct _tmp5F6;_tmp5F6.tag=Cyc_Declarator_tok;
_tmp5F6.f1=({struct Cyc_Parse_Declarator*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));
_tmp5F7->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5F7->tms=({struct Cyc_List_List*_tmp5F8=_cycalloc(
sizeof(*_tmp5F8));_tmp5F8->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp5FA;_tmp5FA.tag=0;_tmp5FA.f1=Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5FA.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5FA;});
_tmp5F9;}));_tmp5F8->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5F8;});_tmp5F7;});
_tmp5F6;});_tmp5F5;});break;case 166: _LL29F: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB[0]=({struct Cyc_Declarator_tok_struct
_tmp5FC;_tmp5FC.tag=Cyc_Declarator_tok;_tmp5FC.f1=({struct Cyc_Parse_Declarator*
_tmp5FD=_cycalloc(sizeof(*_tmp5FD));_tmp5FD->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5FD->tms=({
struct Cyc_List_List*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));_tmp5FE->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp5FF=_cycalloc(sizeof(*_tmp5FF));
_tmp5FF[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp600;_tmp600.tag=1;_tmp600.f1=
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp600.f2=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp600.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp600;});_tmp5FF;}));_tmp5FE->tl=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5FE;});
_tmp5FD;});_tmp5FC;});_tmp5FB;});break;case 167: _LL2A0: {struct _tuple14 _tmp602;
struct Cyc_List_List*_tmp603;int _tmp604;struct Cyc_Absyn_VarargInfo*_tmp605;struct
Cyc_Core_Opt*_tmp606;struct Cyc_List_List*_tmp607;struct _tuple14*_tmp601=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp602=*_tmp601;
_tmp603=_tmp602.f1;_tmp604=_tmp602.f2;_tmp605=_tmp602.f3;_tmp606=_tmp602.f4;
_tmp607=_tmp602.f5;yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp608=
_cycalloc(sizeof(*_tmp608));_tmp608[0]=({struct Cyc_Declarator_tok_struct _tmp609;
_tmp609.tag=Cyc_Declarator_tok;_tmp609.f1=({struct Cyc_Parse_Declarator*_tmp60A=
_cycalloc(sizeof(*_tmp60A));_tmp60A->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp60A->tms=({
struct Cyc_List_List*_tmp60B=_cycalloc(sizeof(*_tmp60B));_tmp60B->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp60C=_cycalloc(sizeof(*_tmp60C));
_tmp60C[0]=({struct Cyc_Absyn_Function_mod_struct _tmp60D;_tmp60D.tag=3;_tmp60D.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp60E=_cycalloc(sizeof(*
_tmp60E));_tmp60E[0]=({struct Cyc_Absyn_WithTypes_struct _tmp60F;_tmp60F.tag=1;
_tmp60F.f1=_tmp603;_tmp60F.f2=_tmp604;_tmp60F.f3=_tmp605;_tmp60F.f4=_tmp606;
_tmp60F.f5=_tmp607;_tmp60F;});_tmp60E;}));_tmp60D;});_tmp60C;}));_tmp60B->tl=(
Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)]))->tms;_tmp60B;});_tmp60A;});_tmp609;});_tmp608;});break;}case 168: _LL2A1:
yyval=(void*)({struct Cyc_Declarator_tok_struct*_tmp610=_cycalloc(sizeof(*_tmp610));
_tmp610[0]=({struct Cyc_Declarator_tok_struct _tmp611;_tmp611.tag=Cyc_Declarator_tok;
_tmp611.f1=({struct Cyc_Parse_Declarator*_tmp612=_cycalloc(sizeof(*_tmp612));
_tmp612->id=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp612->tms=({struct Cyc_List_List*_tmp613=_cycalloc(
sizeof(*_tmp613));_tmp613->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp614=_cycalloc(sizeof(*_tmp614));_tmp614[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp615;_tmp615.tag=3;_tmp615.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp617;_tmp617.tag=1;_tmp617.f1=0;_tmp617.f2=0;_tmp617.f3=0;_tmp617.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp617.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp617;});_tmp616;}));
_tmp615;});_tmp614;}));_tmp613->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp613;});_tmp612;});
_tmp611;});_tmp610;});break;case 169: _LL2A2: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp618=_cycalloc(sizeof(*_tmp618));_tmp618[0]=({struct Cyc_Declarator_tok_struct
_tmp619;_tmp619.tag=Cyc_Declarator_tok;_tmp619.f1=({struct Cyc_Parse_Declarator*
_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp61A->tms=({
struct Cyc_List_List*_tmp61B=_cycalloc(sizeof(*_tmp61B));_tmp61B->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp61C=_cycalloc(sizeof(*_tmp61C));
_tmp61C[0]=({struct Cyc_Absyn_Function_mod_struct _tmp61D;_tmp61D.tag=3;_tmp61D.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp61E=_cycalloc(sizeof(*_tmp61E));
_tmp61E[0]=({struct Cyc_Absyn_NoTypes_struct _tmp61F;_tmp61F.tag=0;_tmp61F.f1=Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp61F.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp61F;});
_tmp61E;}));_tmp61D;});_tmp61C;}));_tmp61B->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp61B;});_tmp61A;});
_tmp619;});_tmp618;});break;case 170: _LL2A3: {struct Cyc_List_List*_tmp620=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_Declarator_tok_struct*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621[0]=({
struct Cyc_Declarator_tok_struct _tmp622;_tmp622.tag=Cyc_Declarator_tok;_tmp622.f1=({
struct Cyc_Parse_Declarator*_tmp623=_cycalloc(sizeof(*_tmp623));_tmp623->id=(Cyc_yyget_Declarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp623->tms=({
struct Cyc_List_List*_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624->hd=(void*)((
void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp625=_cycalloc(sizeof(*_tmp625));
_tmp625[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp626;_tmp626.tag=4;_tmp626.f1=
_tmp620;_tmp626.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp626.f3=0;
_tmp626;});_tmp625;}));_tmp624->tl=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp624;});_tmp623;});
_tmp622;});_tmp621;});break;}case 171: _LL2A4: yyval=(void*)({struct Cyc_Declarator_tok_struct*
_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627[0]=({struct Cyc_Declarator_tok_struct
_tmp628;_tmp628.tag=Cyc_Declarator_tok;_tmp628.f1=({struct Cyc_Parse_Declarator*
_tmp629=_cycalloc(sizeof(*_tmp629));_tmp629->id=(Cyc_yyget_Declarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp629->tms=({
struct Cyc_List_List*_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp62B=_cycalloc(sizeof(*_tmp62B));
_tmp62B[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp62C;_tmp62C.tag=5;_tmp62C.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp62C.f2=Cyc_yyget_AttributeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp62C;});_tmp62B;}));
_tmp62A->tl=(Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp62A;});_tmp629;});_tmp628;});_tmp627;});break;case
172: _LL2A5: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 173: _LL2A6: yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*_tmp62D=
_cycalloc(sizeof(*_tmp62D));_tmp62D[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp62E;_tmp62E.tag=Cyc_TypeModifierList_tok;_tmp62E.f1=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp62E;});_tmp62D;});
break;case 174: _LL2A7: {struct Cyc_List_List*ans=0;if(Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)ans=({struct Cyc_List_List*
_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp630=_cycalloc(sizeof(*_tmp630));_tmp630[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp631;_tmp631.tag=5;_tmp631.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp631.f2=
Cyc_yyget_AttributeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp631;});_tmp630;}));_tmp62F->tl=ans;_tmp62F;});{struct Cyc_Absyn_PtrLoc*
_tmp632=({struct Cyc_Absyn_PtrLoc*_tmp639=_cycalloc(sizeof(*_tmp639));_tmp639->ptr_loc=(*
Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f1;
_tmp639->rgn_loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp639->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line);_tmp639;});ans=({struct Cyc_List_List*_tmp633=
_cycalloc(sizeof(*_tmp633));_tmp633->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp635;_tmp635.tag=2;_tmp635.f1=({struct Cyc_Absyn_PtrAtts _tmp636;_tmp636.rgn=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp636.nullable=(*Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])).f2;_tmp636.bounds=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f3;_tmp636.zero_term=
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp636.ptrloc=_tmp632;_tmp636;});_tmp635.f2=Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp635;});_tmp634;}));
_tmp633->tl=ans;_tmp633;});yyval=(void*)({struct Cyc_TypeModifierList_tok_struct*
_tmp637=_cycalloc(sizeof(*_tmp637));_tmp637[0]=({struct Cyc_TypeModifierList_tok_struct
_tmp638;_tmp638.tag=Cyc_TypeModifierList_tok;_tmp638.f1=ans;_tmp638;});_tmp637;});
break;}}case 175: _LL2A8: yyval=(void*)({struct Cyc_YY1_struct*_tmp63A=_cycalloc(
sizeof(*_tmp63A));_tmp63A[0]=({struct Cyc_YY1_struct _tmp63B;_tmp63B.tag=Cyc_YY1;
_tmp63B.f1=({struct _tuple16*_tmp63C=_cycalloc(sizeof(*_tmp63C));_tmp63C->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp63C->f2=
Cyc_Absyn_true_conref;_tmp63C->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp63C;});_tmp63B;});
_tmp63A;});break;case 176: _LL2A9: yyval=(void*)({struct Cyc_YY1_struct*_tmp63D=
_cycalloc(sizeof(*_tmp63D));_tmp63D[0]=({struct Cyc_YY1_struct _tmp63E;_tmp63E.tag=
Cyc_YY1;_tmp63E.f1=({struct _tuple16*_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F->f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp63F->f2=Cyc_Absyn_false_conref;_tmp63F->f3=Cyc_yyget_YY2(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp63F;});_tmp63E;});
_tmp63D;});break;case 177: _LL2AA: yyval=(void*)({struct Cyc_YY1_struct*_tmp640=
_cycalloc(sizeof(*_tmp640));_tmp640[0]=({struct Cyc_YY1_struct _tmp641;_tmp641.tag=
Cyc_YY1;_tmp641.f1=({struct _tuple16*_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642->f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp642->f2=Cyc_Absyn_true_conref;_tmp642->f3=Cyc_Absyn_bounds_dynforward_conref;
_tmp642;});_tmp641;});_tmp640;});break;case 178: _LL2AB: yyval=(void*)({struct Cyc_YY1_struct*
_tmp643=_cycalloc(sizeof(*_tmp643));_tmp643[0]=({struct Cyc_YY1_struct _tmp644;
_tmp644.tag=Cyc_YY1;_tmp644.f1=({struct _tuple16*_tmp645=_cycalloc(sizeof(*
_tmp645));_tmp645->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp645->f2=
Cyc_Absyn_true_conref;_tmp645->f3=Cyc_Absyn_bounds_dyneither_conref;_tmp645;});
_tmp644;});_tmp643;});break;case 179: _LL2AC: yyval=(void*)({struct Cyc_YY2_struct*
_tmp646=_cycalloc(sizeof(*_tmp646));_tmp646[0]=({struct Cyc_YY2_struct _tmp647;
_tmp647.tag=Cyc_YY2;_tmp647.f1=Cyc_Absyn_bounds_one_conref;_tmp647;});_tmp646;});
break;case 180: _LL2AD: yyval=(void*)({struct Cyc_YY2_struct*_tmp648=_cycalloc(
sizeof(*_tmp648));_tmp648[0]=({struct Cyc_YY2_struct _tmp649;_tmp649.tag=Cyc_YY2;
_tmp649.f1=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp64A=
_cycalloc(sizeof(*_tmp64A));_tmp64A[0]=({struct Cyc_Absyn_Upper_b_struct _tmp64B;
_tmp64B.tag=0;_tmp64B.f1=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp64B;});_tmp64A;}));_tmp649;});_tmp648;});break;case
181: _LL2AE: yyval=(void*)({struct Cyc_YY2_struct*_tmp64C=_cycalloc(sizeof(*_tmp64C));
_tmp64C[0]=({struct Cyc_YY2_struct _tmp64D;_tmp64D.tag=Cyc_YY2;_tmp64D.f1=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp64E=_cycalloc(sizeof(*_tmp64E));
_tmp64E[0]=({struct Cyc_Absyn_AbsUpper_b_struct _tmp64F;_tmp64F.tag=1;_tmp64F.f1=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp64F;});_tmp64E;}));_tmp64D;});_tmp64C;});break;case 182: _LL2AF: yyval=(
void*)({struct Cyc_YY6_struct*_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650[0]=({
struct Cyc_YY6_struct _tmp651;_tmp651.tag=Cyc_YY6;_tmp651.f1=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp651;});_tmp650;});break;case 183: _LL2B0: yyval=(void*)({
struct Cyc_YY6_struct*_tmp652=_cycalloc(sizeof(*_tmp652));_tmp652[0]=({struct Cyc_YY6_struct
_tmp653;_tmp653.tag=Cyc_YY6;_tmp653.f1=Cyc_Absyn_true_conref;_tmp653;});_tmp652;});
break;case 184: _LL2B1: yyval=(void*)({struct Cyc_YY6_struct*_tmp654=_cycalloc(
sizeof(*_tmp654));_tmp654[0]=({struct Cyc_YY6_struct _tmp655;_tmp655.tag=Cyc_YY6;
_tmp655.f1=Cyc_Absyn_false_conref;_tmp655;});_tmp654;});break;case 185: _LL2B2:
yyval=(void*)({struct Cyc_Type_tok_struct*_tmp656=_cycalloc(sizeof(*_tmp656));
_tmp656[0]=({struct Cyc_Type_tok_struct _tmp657;_tmp657.tag=Cyc_Type_tok;_tmp657.f1=(
void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp658=_cycalloc(sizeof(*_tmp658));
_tmp658->v=(void*)((void*)3);_tmp658;}),0);_tmp657;});_tmp656;});break;case 186:
_LL2B3: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3);yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 187: _LL2B4: yyval=(
void*)({struct Cyc_Type_tok_struct*_tmp659=_cycalloc(sizeof(*_tmp659));_tmp659[0]=({
struct Cyc_Type_tok_struct _tmp65A;_tmp65A.tag=Cyc_Type_tok;_tmp65A.f1=(void*)Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->v=(void*)((void*)
3);_tmp65B;}),0);_tmp65A;});_tmp659;});break;case 188: _LL2B5: yyval=(void*)({
struct Cyc_TypeQual_tok_struct*_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C[0]=({
struct Cyc_TypeQual_tok_struct _tmp65D;_tmp65D.tag=Cyc_TypeQual_tok;_tmp65D.f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).last_line));_tmp65D;});_tmp65C;});break;case 189: _LL2B6: yyval=(
void*)({struct Cyc_TypeQual_tok_struct*_tmp65E=_cycalloc(sizeof(*_tmp65E));
_tmp65E[0]=({struct Cyc_TypeQual_tok_struct _tmp65F;_tmp65F.tag=Cyc_TypeQual_tok;
_tmp65F.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_TypeQual_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_TypeQual_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp65F;});_tmp65E;});
break;case 190: _LL2B7: yyval=(void*)({struct Cyc_YY5_struct*_tmp660=_cycalloc(
sizeof(*_tmp660));_tmp660[0]=({struct Cyc_YY5_struct _tmp661;_tmp661.tag=Cyc_YY5;
_tmp661.f1=({struct _tuple14*_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp662->f2=0;
_tmp662->f3=0;_tmp662->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp662->f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp662;});_tmp661;});
_tmp660;});break;case 191: _LL2B8: yyval=(void*)({struct Cyc_YY5_struct*_tmp663=
_cycalloc(sizeof(*_tmp663));_tmp663[0]=({struct Cyc_YY5_struct _tmp664;_tmp664.tag=
Cyc_YY5;_tmp664.f1=({struct _tuple14*_tmp665=_cycalloc(sizeof(*_tmp665));_tmp665->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));_tmp665->f2=1;
_tmp665->f3=0;_tmp665->f4=Cyc_yyget_TypeOpt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp665->f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp665;});_tmp664;});
_tmp663;});break;case 192: _LL2B9: {struct _tuple2 _tmp667;struct Cyc_Core_Opt*
_tmp668;struct Cyc_Absyn_Tqual _tmp669;void*_tmp66A;struct _tuple2*_tmp666=Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp667=*_tmp666;
_tmp668=_tmp667.f1;_tmp669=_tmp667.f2;_tmp66A=_tmp667.f3;{struct Cyc_Absyn_VarargInfo*
_tmp66B=({struct Cyc_Absyn_VarargInfo*_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F->name=
_tmp668;_tmp66F->tq=_tmp669;_tmp66F->type=(void*)_tmp66A;_tmp66F->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp66F;});yyval=(
void*)({struct Cyc_YY5_struct*_tmp66C=_cycalloc(sizeof(*_tmp66C));_tmp66C[0]=({
struct Cyc_YY5_struct _tmp66D;_tmp66D.tag=Cyc_YY5;_tmp66D.f1=({struct _tuple14*
_tmp66E=_cycalloc(sizeof(*_tmp66E));_tmp66E->f1=0;_tmp66E->f2=0;_tmp66E->f3=
_tmp66B;_tmp66E->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp66E->f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp66E;});_tmp66D;});
_tmp66C;});break;}}case 193: _LL2BA: {struct _tuple2 _tmp671;struct Cyc_Core_Opt*
_tmp672;struct Cyc_Absyn_Tqual _tmp673;void*_tmp674;struct _tuple2*_tmp670=Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp671=*_tmp670;
_tmp672=_tmp671.f1;_tmp673=_tmp671.f2;_tmp674=_tmp671.f3;{struct Cyc_Absyn_VarargInfo*
_tmp675=({struct Cyc_Absyn_VarargInfo*_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679->name=
_tmp672;_tmp679->tq=_tmp673;_tmp679->type=(void*)_tmp674;_tmp679->inject=Cyc_yyget_Bool_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp679;});yyval=(
void*)({struct Cyc_YY5_struct*_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676[0]=({
struct Cyc_YY5_struct _tmp677;_tmp677.tag=Cyc_YY5;_tmp677.f1=({struct _tuple14*
_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678->f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ParamDeclList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp678->f2=0;_tmp678->f3=
_tmp675;_tmp678->f4=Cyc_yyget_TypeOpt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp678->f5=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp678;});_tmp677;});
_tmp676;});break;}}case 194: _LL2BB: yyval=(void*)({struct Cyc_Type_tok_struct*
_tmp67A=_cycalloc(sizeof(*_tmp67A));_tmp67A[0]=({struct Cyc_Type_tok_struct
_tmp67B;_tmp67B.tag=Cyc_Type_tok;_tmp67B.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp67D;_tmp67D.tag=1;_tmp67D.f1=0;_tmp67D;});_tmp67C;}));_tmp67B;});_tmp67A;});
break;case 195: _LL2BC: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp67E=_cycalloc(
sizeof(*_tmp67E));_tmp67E[0]=({struct Cyc_Type_tok_struct _tmp67F;_tmp67F.tag=Cyc_Type_tok;
_tmp67F.f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp680=_cycalloc(sizeof(*_tmp680));_tmp680[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp681;_tmp681.tag=0;_tmp681.f1=(void*)Cyc_yyget_Kind_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp681;});_tmp680;}));
_tmp67F;});_tmp67E;});break;case 196: _LL2BD: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*
_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682[0]=({struct Cyc_TypeOpt_tok_struct
_tmp683;_tmp683.tag=Cyc_TypeOpt_tok;_tmp683.f1=0;_tmp683;});_tmp682;});break;
case 197: _LL2BE: yyval=(void*)({struct Cyc_TypeOpt_tok_struct*_tmp684=_cycalloc(
sizeof(*_tmp684));_tmp684[0]=({struct Cyc_TypeOpt_tok_struct _tmp685;_tmp685.tag=
Cyc_TypeOpt_tok;_tmp685.f1=({struct Cyc_Core_Opt*_tmp686=_cycalloc(sizeof(*
_tmp686));_tmp686->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp687=
_cycalloc(sizeof(*_tmp687));_tmp687[0]=({struct Cyc_Absyn_JoinEff_struct _tmp688;
_tmp688.tag=20;_tmp688.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp688;});_tmp687;}));
_tmp686;});_tmp685;});_tmp684;});break;case 198: _LL2BF: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*
_tmp689=_cycalloc(sizeof(*_tmp689));_tmp689[0]=({struct Cyc_Rgnorder_tok_struct
_tmp68A;_tmp68A.tag=Cyc_Rgnorder_tok;_tmp68A.f1=0;_tmp68A;});_tmp689;});break;
case 199: _LL2C0: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 200: _LL2C1: yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp68B=
_cycalloc(sizeof(*_tmp68B));_tmp68B[0]=({struct Cyc_Rgnorder_tok_struct _tmp68C;
_tmp68C.tag=Cyc_Rgnorder_tok;_tmp68C.f1=({struct Cyc_List_List*_tmp68D=_cycalloc(
sizeof(*_tmp68D));_tmp68D->hd=({struct _tuple4*_tmp68E=_cycalloc(sizeof(*_tmp68E));
_tmp68E->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp691=_cycalloc(sizeof(*
_tmp691));_tmp691[0]=({struct Cyc_Absyn_JoinEff_struct _tmp692;_tmp692.tag=20;
_tmp692.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp692;});_tmp691;});_tmp68E->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp690;_tmp690.tag=0;_tmp690.f1=(void*)((void*)3);_tmp690;});_tmp68F;}));
_tmp68E;});_tmp68D->tl=0;_tmp68D;});_tmp68C;});_tmp68B;});break;case 201: _LL2C2:
yyval=(void*)({struct Cyc_Rgnorder_tok_struct*_tmp693=_cycalloc(sizeof(*_tmp693));
_tmp693[0]=({struct Cyc_Rgnorder_tok_struct _tmp694;_tmp694.tag=Cyc_Rgnorder_tok;
_tmp694.f1=({struct Cyc_List_List*_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695->hd=({
struct _tuple4*_tmp696=_cycalloc(sizeof(*_tmp696));_tmp696->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp699=_cycalloc(sizeof(*_tmp699));_tmp699[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp69A;_tmp69A.tag=20;_tmp69A.f1=Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp69A;});_tmp699;});
_tmp696->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp697=_cycalloc(sizeof(*_tmp697));_tmp697[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp698;_tmp698.tag=0;_tmp698.f1=(void*)((void*)3);_tmp698;});_tmp697;}));
_tmp696;});_tmp695->tl=Cyc_yyget_Rgnorder_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp695;});_tmp694;});
_tmp693;});break;case 202: _LL2C3: yyval=(void*)({struct Cyc_Bool_tok_struct*_tmp69B=
_cycalloc_atomic(sizeof(*_tmp69B));_tmp69B[0]=({struct Cyc_Bool_tok_struct _tmp69C;
_tmp69C.tag=Cyc_Bool_tok;_tmp69C.f1=0;_tmp69C;});_tmp69B;});break;case 203: _LL2C4:
if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp69D="inject";
_tag_dynforward(_tmp69D,sizeof(char),_get_zero_arr_size(_tmp69D,7));}))!= 0)Cyc_Parse_err(({
const char*_tmp69E="missing type in function declaration";_tag_dynforward(_tmp69E,
sizeof(char),_get_zero_arr_size(_tmp69E,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Bool_tok_struct*_tmp69F=_cycalloc_atomic(sizeof(*_tmp69F));_tmp69F[0]=({
struct Cyc_Bool_tok_struct _tmp6A0;_tmp6A0.tag=Cyc_Bool_tok;_tmp6A0.f1=1;_tmp6A0;});
_tmp69F;});break;case 204: _LL2C5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 205: _LL2C6: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp6A1=_cycalloc(sizeof(*_tmp6A1));_tmp6A1[0]=({struct Cyc_TypeList_tok_struct
_tmp6A2;_tmp6A2.tag=Cyc_TypeList_tok;_tmp6A2.f1=Cyc_List_imp_append(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp6A2;});_tmp6A1;});
break;case 206: _LL2C7: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp6A3=
_cycalloc(sizeof(*_tmp6A3));_tmp6A3[0]=({struct Cyc_TypeList_tok_struct _tmp6A4;
_tmp6A4.tag=Cyc_TypeList_tok;_tmp6A4.f1=0;_tmp6A4;});_tmp6A3;});break;case 207:
_LL2C8: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 208: _LL2C9: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp6A5=_cycalloc(
sizeof(*_tmp6A5));_tmp6A5[0]=({struct Cyc_TypeList_tok_struct _tmp6A6;_tmp6A6.tag=
Cyc_TypeList_tok;_tmp6A6.f1=({struct Cyc_List_List*_tmp6A7=_cycalloc(sizeof(*
_tmp6A7));_tmp6A7->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6A8=
_cycalloc(sizeof(*_tmp6A8));_tmp6A8[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6A9;
_tmp6A9.tag=21;_tmp6A9.f1=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6A9;});_tmp6A8;}));
_tmp6A7->tl=0;_tmp6A7;});_tmp6A6;});_tmp6A5;});break;case 209: _LL2CA: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)4);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp6AA=_cycalloc(sizeof(*
_tmp6AA));_tmp6AA[0]=({struct Cyc_TypeList_tok_struct _tmp6AB;_tmp6AB.tag=Cyc_TypeList_tok;
_tmp6AB.f1=({struct Cyc_List_List*_tmp6AC=_cycalloc(sizeof(*_tmp6AC));_tmp6AC->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp6AC->tl=0;_tmp6AC;});_tmp6AB;});_tmp6AA;});break;case 210: _LL2CB: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)3);yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp6AD=_cycalloc(sizeof(*
_tmp6AD));_tmp6AD[0]=({struct Cyc_TypeList_tok_struct _tmp6AE;_tmp6AE.tag=Cyc_TypeList_tok;
_tmp6AE.f1=({struct Cyc_List_List*_tmp6AF=_cycalloc(sizeof(*_tmp6AF));_tmp6AF->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp6B0=_cycalloc(sizeof(*
_tmp6B0));_tmp6B0[0]=({struct Cyc_Absyn_AccessEff_struct _tmp6B1;_tmp6B1.tag=19;
_tmp6B1.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp6B1;});_tmp6B0;}));_tmp6AF->tl=0;_tmp6AF;});_tmp6AE;});
_tmp6AD;});break;case 211: _LL2CC: Cyc_Parse_set_vartyp_kind(Cyc_yyget_Type_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)3);yyval=(
void*)({struct Cyc_TypeList_tok_struct*_tmp6B2=_cycalloc(sizeof(*_tmp6B2));
_tmp6B2[0]=({struct Cyc_TypeList_tok_struct _tmp6B3;_tmp6B3.tag=Cyc_TypeList_tok;
_tmp6B3.f1=({struct Cyc_List_List*_tmp6B4=_cycalloc(sizeof(*_tmp6B4));_tmp6B4->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp6B5=_cycalloc(sizeof(*
_tmp6B5));_tmp6B5[0]=({struct Cyc_Absyn_AccessEff_struct _tmp6B6;_tmp6B6.tag=19;
_tmp6B6.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp6B6;});_tmp6B5;}));_tmp6B4->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B4;});_tmp6B3;});
_tmp6B2;});break;case 212: _LL2CD: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp6B7=_cycalloc(sizeof(*_tmp6B7));_tmp6B7[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp6B8;_tmp6B8.tag=Cyc_ParamDeclList_tok;_tmp6B8.f1=({struct Cyc_List_List*
_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9->hd=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B9->tl=0;_tmp6B9;});
_tmp6B8;});_tmp6B7;});break;case 213: _LL2CE: yyval=(void*)({struct Cyc_ParamDeclList_tok_struct*
_tmp6BA=_cycalloc(sizeof(*_tmp6BA));_tmp6BA[0]=({struct Cyc_ParamDeclList_tok_struct
_tmp6BB;_tmp6BB.tag=Cyc_ParamDeclList_tok;_tmp6BB.f1=({struct Cyc_List_List*
_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC->hd=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6BC->tl=Cyc_yyget_ParamDeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6BC;});_tmp6BB;});
_tmp6BA;});break;case 214: _LL2CF: {struct _tuple15 _tmp6BE;struct Cyc_Absyn_Tqual
_tmp6BF;struct Cyc_List_List*_tmp6C0;struct Cyc_List_List*_tmp6C1;struct _tuple15*
_tmp6BD=Cyc_yyget_QualSpecList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp6BE=*_tmp6BD;_tmp6BF=_tmp6BE.f1;_tmp6C0=_tmp6BE.f2;
_tmp6C1=_tmp6BE.f3;if(_tmp6BF.loc == 0)_tmp6BF.loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator
_tmp6C3;struct _tuple1*_tmp6C4;struct Cyc_List_List*_tmp6C5;struct Cyc_Parse_Declarator*
_tmp6C2=Cyc_yyget_Declarator_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp6C3=*_tmp6C2;_tmp6C4=_tmp6C3.id;_tmp6C5=_tmp6C3.tms;{void*
_tmp6C6=Cyc_Parse_speclist2typ(_tmp6C0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual
_tmp6C8;void*_tmp6C9;struct Cyc_List_List*_tmp6CA;struct Cyc_List_List*_tmp6CB;
struct _tuple6 _tmp6C7=Cyc_Parse_apply_tms(_tmp6BF,_tmp6C6,_tmp6C1,_tmp6C5);
_tmp6C8=_tmp6C7.f1;_tmp6C9=_tmp6C7.f2;_tmp6CA=_tmp6C7.f3;_tmp6CB=_tmp6C7.f4;if(
_tmp6CA != 0)Cyc_Parse_err(({const char*_tmp6CC="parameter with bad type params";
_tag_dynforward(_tmp6CC,sizeof(char),_get_zero_arr_size(_tmp6CC,31));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp6C4))Cyc_Parse_err(({const char*_tmp6CD="parameter cannot be qualified with a namespace";
_tag_dynforward(_tmp6CD,sizeof(char),_get_zero_arr_size(_tmp6CD,47));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp6CE=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp6D4=_cycalloc(sizeof(*
_tmp6D4));_tmp6D4->v=(*_tmp6C4).f2;_tmp6D4;});if(_tmp6CB != 0)({void*_tmp6CF[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp6D0="extra attributes on parameter, ignoring";
_tag_dynforward(_tmp6D0,sizeof(char),_get_zero_arr_size(_tmp6D0,40));}),
_tag_dynforward(_tmp6CF,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1[0]=({struct Cyc_ParamDecl_tok_struct
_tmp6D2;_tmp6D2.tag=Cyc_ParamDecl_tok;_tmp6D2.f1=({struct _tuple2*_tmp6D3=
_cycalloc(sizeof(*_tmp6D3));_tmp6D3->f1=_tmp6CE;_tmp6D3->f2=_tmp6C8;_tmp6D3->f3=
_tmp6C9;_tmp6D3;});_tmp6D2;});_tmp6D1;});break;}}}}case 215: _LL2D0: {struct
_tuple15 _tmp6D6;struct Cyc_Absyn_Tqual _tmp6D7;struct Cyc_List_List*_tmp6D8;struct
Cyc_List_List*_tmp6D9;struct _tuple15*_tmp6D5=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6D6=*_tmp6D5;_tmp6D7=
_tmp6D6.f1;_tmp6D8=_tmp6D6.f2;_tmp6D9=_tmp6D6.f3;if(_tmp6D7.loc == 0)_tmp6D7.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);{void*_tmp6DA=Cyc_Parse_speclist2typ(_tmp6D8,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp6D9 != 0)({
void*_tmp6DB[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6DC="bad attributes on parameter, ignoring";_tag_dynforward(_tmp6DC,sizeof(
char),_get_zero_arr_size(_tmp6DC,38));}),_tag_dynforward(_tmp6DB,sizeof(void*),0));});
yyval=(void*)({struct Cyc_ParamDecl_tok_struct*_tmp6DD=_cycalloc(sizeof(*_tmp6DD));
_tmp6DD[0]=({struct Cyc_ParamDecl_tok_struct _tmp6DE;_tmp6DE.tag=Cyc_ParamDecl_tok;
_tmp6DE.f1=({struct _tuple2*_tmp6DF=_cycalloc(sizeof(*_tmp6DF));_tmp6DF->f1=0;
_tmp6DF->f2=_tmp6D7;_tmp6DF->f3=_tmp6DA;_tmp6DF;});_tmp6DE;});_tmp6DD;});break;}}
case 216: _LL2D1: {struct _tuple15 _tmp6E1;struct Cyc_Absyn_Tqual _tmp6E2;struct Cyc_List_List*
_tmp6E3;struct Cyc_List_List*_tmp6E4;struct _tuple15*_tmp6E0=Cyc_yyget_QualSpecList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6E1=*_tmp6E0;
_tmp6E2=_tmp6E1.f1;_tmp6E3=_tmp6E1.f2;_tmp6E4=_tmp6E1.f3;if(_tmp6E2.loc == 0)
_tmp6E2.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{void*_tmp6E5=Cyc_Parse_speclist2typ(_tmp6E3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6E6=(Cyc_yyget_AbstractDeclarator_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->tms;struct Cyc_Absyn_Tqual _tmp6E8;void*_tmp6E9;struct Cyc_List_List*
_tmp6EA;struct Cyc_List_List*_tmp6EB;struct _tuple6 _tmp6E7=Cyc_Parse_apply_tms(
_tmp6E2,_tmp6E5,_tmp6E4,_tmp6E6);_tmp6E8=_tmp6E7.f1;_tmp6E9=_tmp6E7.f2;_tmp6EA=
_tmp6E7.f3;_tmp6EB=_tmp6E7.f4;if(_tmp6EA != 0)({void*_tmp6EC[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp6ED="bad type parameters on formal argument, ignoring";
_tag_dynforward(_tmp6ED,sizeof(char),_get_zero_arr_size(_tmp6ED,49));}),
_tag_dynforward(_tmp6EC,sizeof(void*),0));});if(_tmp6EB != 0)({void*_tmp6EE[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp6EF="bad attributes on parameter, ignoring";
_tag_dynforward(_tmp6EF,sizeof(char),_get_zero_arr_size(_tmp6EF,38));}),
_tag_dynforward(_tmp6EE,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp6F0=_cycalloc(sizeof(*_tmp6F0));_tmp6F0[0]=({struct Cyc_ParamDecl_tok_struct
_tmp6F1;_tmp6F1.tag=Cyc_ParamDecl_tok;_tmp6F1.f1=({struct _tuple2*_tmp6F2=
_cycalloc(sizeof(*_tmp6F2));_tmp6F2->f1=0;_tmp6F2->f2=_tmp6E8;_tmp6F2->f3=
_tmp6E9;_tmp6F2;});_tmp6F1;});_tmp6F0;});break;}}case 217: _LL2D2: yyval=(void*)({
struct Cyc_IdList_tok_struct*_tmp6F3=_cycalloc(sizeof(*_tmp6F3));_tmp6F3[0]=({
struct Cyc_IdList_tok_struct _tmp6F4;_tmp6F4.tag=Cyc_IdList_tok;_tmp6F4.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_IdList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp6F4;});_tmp6F3;});
break;case 218: _LL2D3: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp6F5=
_cycalloc(sizeof(*_tmp6F5));_tmp6F5[0]=({struct Cyc_IdList_tok_struct _tmp6F6;
_tmp6F6.tag=Cyc_IdList_tok;_tmp6F6.f1=({struct Cyc_List_List*_tmp6F7=_cycalloc(
sizeof(*_tmp6F7));_tmp6F7->hd=({struct _dynforward_ptr*_tmp6F8=_cycalloc(sizeof(*
_tmp6F8));_tmp6F8[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp6F8;});_tmp6F7->tl=0;_tmp6F7;});_tmp6F6;});_tmp6F5;});
break;case 219: _LL2D4: yyval=(void*)({struct Cyc_IdList_tok_struct*_tmp6F9=
_cycalloc(sizeof(*_tmp6F9));_tmp6F9[0]=({struct Cyc_IdList_tok_struct _tmp6FA;
_tmp6FA.tag=Cyc_IdList_tok;_tmp6FA.f1=({struct Cyc_List_List*_tmp6FB=_cycalloc(
sizeof(*_tmp6FB));_tmp6FB->hd=({struct _dynforward_ptr*_tmp6FC=_cycalloc(sizeof(*
_tmp6FC));_tmp6FC[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp6FC;});_tmp6FB->tl=Cyc_yyget_IdList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6FB;});_tmp6FA;});
_tmp6F9;});break;case 220: _LL2D5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 221: _LL2D6: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 222: _LL2D7: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp6FD=_cycalloc(sizeof(*_tmp6FD));_tmp6FD[0]=({struct Cyc_Exp_tok_struct _tmp6FE;
_tmp6FE.tag=Cyc_Exp_tok;_tmp6FE.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp6FF=_cycalloc(sizeof(*_tmp6FF));_tmp6FF[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp700;_tmp700.tag=36;_tmp700.f1=0;_tmp700.f2=0;_tmp700;});_tmp6FF;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp6FE;});
_tmp6FD;});break;case 223: _LL2D8: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp701=
_cycalloc(sizeof(*_tmp701));_tmp701[0]=({struct Cyc_Exp_tok_struct _tmp702;_tmp702.tag=
Cyc_Exp_tok;_tmp702.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp703=_cycalloc(sizeof(*_tmp703));_tmp703[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp704;_tmp704.tag=36;_tmp704.f1=0;_tmp704.f2=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp704;});_tmp703;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp702;});_tmp701;});break;case 224: _LL2D9: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp705=_cycalloc(sizeof(*_tmp705));_tmp705[0]=({
struct Cyc_Exp_tok_struct _tmp706;_tmp706.tag=Cyc_Exp_tok;_tmp706.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp707=_cycalloc(sizeof(*_tmp707));
_tmp707[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp708;_tmp708.tag=36;
_tmp708.f1=0;_tmp708.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp708;});_tmp707;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp706;});
_tmp705;});break;case 225: _LL2DA: {struct Cyc_Absyn_Vardecl*_tmp709=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp70E=_cycalloc(sizeof(*_tmp70E));_tmp70E->f1=(void*)0;_tmp70E->f2=({
struct _dynforward_ptr*_tmp70F=_cycalloc(sizeof(*_tmp70F));_tmp70F[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp70F;});_tmp70E;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp709->tq).real_const=
1;yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp70A=_cycalloc(sizeof(*_tmp70A));
_tmp70A[0]=({struct Cyc_Exp_tok_struct _tmp70B;_tmp70B.tag=Cyc_Exp_tok;_tmp70B.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*_tmp70C=
_cycalloc(sizeof(*_tmp70C));_tmp70C[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp70D;_tmp70D.tag=29;_tmp70D.f1=_tmp709;_tmp70D.f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp70D.f3=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp70D.f4=0;
_tmp70D;});_tmp70C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp70B;});
_tmp70A;});break;}case 226: _LL2DB: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp710=_cycalloc(sizeof(*_tmp710));_tmp710[0]=({struct Cyc_InitializerList_tok_struct
_tmp711;_tmp711.tag=Cyc_InitializerList_tok;_tmp711.f1=({struct Cyc_List_List*
_tmp712=_cycalloc(sizeof(*_tmp712));_tmp712->hd=({struct _tuple20*_tmp713=
_cycalloc(sizeof(*_tmp713));_tmp713->f1=0;_tmp713->f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp713;});_tmp712->tl=0;
_tmp712;});_tmp711;});_tmp710;});break;case 227: _LL2DC: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp714=_cycalloc(sizeof(*_tmp714));_tmp714[0]=({struct Cyc_InitializerList_tok_struct
_tmp715;_tmp715.tag=Cyc_InitializerList_tok;_tmp715.f1=({struct Cyc_List_List*
_tmp716=_cycalloc(sizeof(*_tmp716));_tmp716->hd=({struct _tuple20*_tmp717=
_cycalloc(sizeof(*_tmp717));_tmp717->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp717->f2=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp717;});_tmp716->tl=
0;_tmp716;});_tmp715;});_tmp714;});break;case 228: _LL2DD: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp718=_cycalloc(sizeof(*_tmp718));_tmp718[0]=({struct Cyc_InitializerList_tok_struct
_tmp719;_tmp719.tag=Cyc_InitializerList_tok;_tmp719.f1=({struct Cyc_List_List*
_tmp71A=_cycalloc(sizeof(*_tmp71A));_tmp71A->hd=({struct _tuple20*_tmp71B=
_cycalloc(sizeof(*_tmp71B));_tmp71B->f1=0;_tmp71B->f2=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp71B;});_tmp71A->tl=Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp71A;});_tmp719;});
_tmp718;});break;case 229: _LL2DE: yyval=(void*)({struct Cyc_InitializerList_tok_struct*
_tmp71C=_cycalloc(sizeof(*_tmp71C));_tmp71C[0]=({struct Cyc_InitializerList_tok_struct
_tmp71D;_tmp71D.tag=Cyc_InitializerList_tok;_tmp71D.f1=({struct Cyc_List_List*
_tmp71E=_cycalloc(sizeof(*_tmp71E));_tmp71E->hd=({struct _tuple20*_tmp71F=
_cycalloc(sizeof(*_tmp71F));_tmp71F->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp71F->f2=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp71F;});_tmp71E->tl=
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp71E;});_tmp71D;});_tmp71C;});break;case 230: _LL2DF: yyval=(
void*)({struct Cyc_DesignatorList_tok_struct*_tmp720=_cycalloc(sizeof(*_tmp720));
_tmp720[0]=({struct Cyc_DesignatorList_tok_struct _tmp721;_tmp721.tag=Cyc_DesignatorList_tok;
_tmp721.f1=Cyc_List_imp_rev(Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp721;});_tmp720;});
break;case 231: _LL2E0: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*_tmp722=
_cycalloc(sizeof(*_tmp722));_tmp722[0]=({struct Cyc_DesignatorList_tok_struct
_tmp723;_tmp723.tag=Cyc_DesignatorList_tok;_tmp723.f1=({struct Cyc_List_List*
_tmp724=_cycalloc(sizeof(*_tmp724));_tmp724->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp724->tl=0;_tmp724;});
_tmp723;});_tmp722;});break;case 232: _LL2E1: yyval=(void*)({struct Cyc_DesignatorList_tok_struct*
_tmp725=_cycalloc(sizeof(*_tmp725));_tmp725[0]=({struct Cyc_DesignatorList_tok_struct
_tmp726;_tmp726.tag=Cyc_DesignatorList_tok;_tmp726.f1=({struct Cyc_List_List*
_tmp727=_cycalloc(sizeof(*_tmp727));_tmp727->hd=(void*)Cyc_yyget_Designator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp727->tl=Cyc_yyget_DesignatorList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp727;});_tmp726;});
_tmp725;});break;case 233: _LL2E2: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp728=_cycalloc(sizeof(*_tmp728));_tmp728[0]=({struct Cyc_Designator_tok_struct
_tmp729;_tmp729.tag=Cyc_Designator_tok;_tmp729.f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp72A=_cycalloc(sizeof(*_tmp72A));_tmp72A[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp72B;_tmp72B.tag=0;_tmp72B.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp72B;});_tmp72A;}));
_tmp729;});_tmp728;});break;case 234: _LL2E3: yyval=(void*)({struct Cyc_Designator_tok_struct*
_tmp72C=_cycalloc(sizeof(*_tmp72C));_tmp72C[0]=({struct Cyc_Designator_tok_struct
_tmp72D;_tmp72D.tag=Cyc_Designator_tok;_tmp72D.f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp72E=_cycalloc(sizeof(*_tmp72E));_tmp72E[0]=({struct Cyc_Absyn_FieldName_struct
_tmp72F;_tmp72F.tag=1;_tmp72F.f1=({struct _dynforward_ptr*_tmp730=_cycalloc(
sizeof(*_tmp730));_tmp730[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp730;});_tmp72F;});
_tmp72E;}));_tmp72D;});_tmp72C;});break;case 235: _LL2E4: {struct _tuple15 _tmp732;
struct Cyc_Absyn_Tqual _tmp733;struct Cyc_List_List*_tmp734;struct Cyc_List_List*
_tmp735;struct _tuple15*_tmp731=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp732=*_tmp731;_tmp733=
_tmp732.f1;_tmp734=_tmp732.f2;_tmp735=_tmp732.f3;{void*_tmp736=Cyc_Parse_speclist2typ(
_tmp734,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp735 != 0)({void*_tmp737[0]={};Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp738="ignoring attributes in type";
_tag_dynforward(_tmp738,sizeof(char),_get_zero_arr_size(_tmp738,28));}),
_tag_dynforward(_tmp737,sizeof(void*),0));});yyval=(void*)({struct Cyc_ParamDecl_tok_struct*
_tmp739=_cycalloc(sizeof(*_tmp739));_tmp739[0]=({struct Cyc_ParamDecl_tok_struct
_tmp73A;_tmp73A.tag=Cyc_ParamDecl_tok;_tmp73A.f1=({struct _tuple2*_tmp73B=
_cycalloc(sizeof(*_tmp73B));_tmp73B->f1=0;_tmp73B->f2=_tmp733;_tmp73B->f3=
_tmp736;_tmp73B;});_tmp73A;});_tmp739;});break;}}case 236: _LL2E5: {struct _tuple15
_tmp73D;struct Cyc_Absyn_Tqual _tmp73E;struct Cyc_List_List*_tmp73F;struct Cyc_List_List*
_tmp740;struct _tuple15*_tmp73C=Cyc_yyget_QualSpecList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp73D=*_tmp73C;_tmp73E=
_tmp73D.f1;_tmp73F=_tmp73D.f2;_tmp740=_tmp73D.f3;{void*_tmp741=Cyc_Parse_speclist2typ(
_tmp73F,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp742=(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct _tuple6
_tmp743=Cyc_Parse_apply_tms(_tmp73E,_tmp741,_tmp740,_tmp742);if(_tmp743.f3 != 0)({
void*_tmp744[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp745="bad type params, ignoring";_tag_dynforward(_tmp745,sizeof(char),
_get_zero_arr_size(_tmp745,26));}),_tag_dynforward(_tmp744,sizeof(void*),0));});
if(_tmp743.f4 != 0)({void*_tmp746[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp747="bad specifiers, ignoring";_tag_dynforward(_tmp747,sizeof(char),
_get_zero_arr_size(_tmp747,25));}),_tag_dynforward(_tmp746,sizeof(void*),0));});
yyval=(void*)({struct Cyc_ParamDecl_tok_struct*_tmp748=_cycalloc(sizeof(*_tmp748));
_tmp748[0]=({struct Cyc_ParamDecl_tok_struct _tmp749;_tmp749.tag=Cyc_ParamDecl_tok;
_tmp749.f1=({struct _tuple2*_tmp74A=_cycalloc(sizeof(*_tmp74A));_tmp74A->f1=0;
_tmp74A->f2=_tmp743.f1;_tmp74A->f3=_tmp743.f2;_tmp74A;});_tmp749;});_tmp748;});
break;}}case 237: _LL2E6: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp74B=
_cycalloc(sizeof(*_tmp74B));_tmp74B[0]=({struct Cyc_Type_tok_struct _tmp74C;
_tmp74C.tag=Cyc_Type_tok;_tmp74C.f1=(void*)(*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp74C;});_tmp74B;});
break;case 238: _LL2E7: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp74D=_cycalloc(
sizeof(*_tmp74D));_tmp74D[0]=({struct Cyc_Type_tok_struct _tmp74E;_tmp74E.tag=Cyc_Type_tok;
_tmp74E.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp74F=_cycalloc(
sizeof(*_tmp74F));_tmp74F[0]=({struct Cyc_Absyn_JoinEff_struct _tmp750;_tmp750.tag=
20;_tmp750.f1=0;_tmp750;});_tmp74F;}));_tmp74E;});_tmp74D;});break;case 239:
_LL2E8: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp751=_cycalloc(sizeof(*
_tmp751));_tmp751[0]=({struct Cyc_Type_tok_struct _tmp752;_tmp752.tag=Cyc_Type_tok;
_tmp752.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp753=_cycalloc(
sizeof(*_tmp753));_tmp753[0]=({struct Cyc_Absyn_JoinEff_struct _tmp754;_tmp754.tag=
20;_tmp754.f1=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp754;});_tmp753;}));_tmp752;});_tmp751;});break;case 240:
_LL2E9: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp755=_cycalloc(sizeof(*
_tmp755));_tmp755[0]=({struct Cyc_Type_tok_struct _tmp756;_tmp756.tag=Cyc_Type_tok;
_tmp756.f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp757=_cycalloc(
sizeof(*_tmp757));_tmp757[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp758;_tmp758.tag=
21;_tmp758.f1=(void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp758;});_tmp757;}));_tmp756;});_tmp755;});break;case
241: _LL2EA: yyval=(void*)({struct Cyc_Type_tok_struct*_tmp759=_cycalloc(sizeof(*
_tmp759));_tmp759[0]=({struct Cyc_Type_tok_struct _tmp75A;_tmp75A.tag=Cyc_Type_tok;
_tmp75A.f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp75B=_cycalloc(
sizeof(*_tmp75B));_tmp75B[0]=({struct Cyc_Absyn_JoinEff_struct _tmp75C;_tmp75C.tag=
20;_tmp75C.f1=({struct Cyc_List_List*_tmp75D=_cycalloc(sizeof(*_tmp75D));_tmp75D->hd=(
void*)Cyc_yyget_Type_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp75D->tl=Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp75D;});_tmp75C;});_tmp75B;}));_tmp75A;});_tmp759;});
break;case 242: _LL2EB: yyval=(void*)({struct Cyc_TypeList_tok_struct*_tmp75E=
_cycalloc(sizeof(*_tmp75E));_tmp75E[0]=({struct Cyc_TypeList_tok_struct _tmp75F;
_tmp75F.tag=Cyc_TypeList_tok;_tmp75F.f1=({struct Cyc_List_List*_tmp760=_cycalloc(
sizeof(*_tmp760));_tmp760->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp760->tl=0;_tmp760;});
_tmp75F;});_tmp75E;});break;case 243: _LL2EC: yyval=(void*)({struct Cyc_TypeList_tok_struct*
_tmp761=_cycalloc(sizeof(*_tmp761));_tmp761[0]=({struct Cyc_TypeList_tok_struct
_tmp762;_tmp762.tag=Cyc_TypeList_tok;_tmp762.f1=({struct Cyc_List_List*_tmp763=
_cycalloc(sizeof(*_tmp763));_tmp763->hd=(void*)Cyc_yyget_Type_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp763->tl=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp763;});_tmp762;});
_tmp761;});break;case 244: _LL2ED: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp764=_cycalloc(sizeof(*_tmp764));_tmp764[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp765;_tmp765.tag=Cyc_AbstractDeclarator_tok;_tmp765.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp766=_cycalloc(sizeof(*_tmp766));_tmp766->tms=Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp766;});_tmp765;});
_tmp764;});break;case 245: _LL2EE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 246: _LL2EF: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp767=_cycalloc(sizeof(*_tmp767));_tmp767[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp768;_tmp768.tag=Cyc_AbstractDeclarator_tok;_tmp768.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp769=_cycalloc(sizeof(*_tmp769));_tmp769->tms=Cyc_List_imp_append(Cyc_yyget_TypeModifierList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_AbstractDeclarator_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp769;});
_tmp768;});_tmp767;});break;case 247: _LL2F0: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 248: _LL2F1:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp76A=_cycalloc(sizeof(*
_tmp76A));_tmp76A[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp76B;_tmp76B.tag=
Cyc_AbstractDeclarator_tok;_tmp76B.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp76C=_cycalloc(sizeof(*_tmp76C));_tmp76C->tms=({struct Cyc_List_List*_tmp76D=
_cycalloc(sizeof(*_tmp76D));_tmp76D->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp76E=_cycalloc(sizeof(*_tmp76E));_tmp76E[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp76F;_tmp76F.tag=0;_tmp76F.f1=Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp76F.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp76F;});
_tmp76E;}));_tmp76D->tl=0;_tmp76D;});_tmp76C;});_tmp76B;});_tmp76A;});break;case
249: _LL2F2: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp770=
_cycalloc(sizeof(*_tmp770));_tmp770[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp771;_tmp771.tag=Cyc_AbstractDeclarator_tok;_tmp771.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp772=_cycalloc(sizeof(*_tmp772));_tmp772->tms=({struct Cyc_List_List*_tmp773=
_cycalloc(sizeof(*_tmp773));_tmp773->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp774=_cycalloc(sizeof(*_tmp774));_tmp774[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp775;_tmp775.tag=0;_tmp775.f1=Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp775.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp775;});
_tmp774;}));_tmp773->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp773;});_tmp772;});
_tmp771;});_tmp770;});break;case 250: _LL2F3: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp776=_cycalloc(sizeof(*_tmp776));_tmp776[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp777;_tmp777.tag=Cyc_AbstractDeclarator_tok;_tmp777.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp778=_cycalloc(sizeof(*_tmp778));_tmp778->tms=({struct Cyc_List_List*_tmp779=
_cycalloc(sizeof(*_tmp779));_tmp779->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp77A=_cycalloc(sizeof(*_tmp77A));_tmp77A[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp77B;_tmp77B.tag=1;_tmp77B.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp77B.f2=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp77B.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp77B;});
_tmp77A;}));_tmp779->tl=0;_tmp779;});_tmp778;});_tmp777;});_tmp776;});break;case
251: _LL2F4: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp77C=
_cycalloc(sizeof(*_tmp77C));_tmp77C[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp77D;_tmp77D.tag=Cyc_AbstractDeclarator_tok;_tmp77D.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp77E=_cycalloc(sizeof(*_tmp77E));_tmp77E->tms=({struct Cyc_List_List*_tmp77F=
_cycalloc(sizeof(*_tmp77F));_tmp77F->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp780=_cycalloc(sizeof(*_tmp780));_tmp780[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp781;_tmp781.tag=1;_tmp781.f1=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp781.f2=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp781.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp781;});
_tmp780;}));_tmp77F->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp77F;});_tmp77E;});
_tmp77D;});_tmp77C;});break;case 252: _LL2F5: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp782=_cycalloc(sizeof(*_tmp782));_tmp782[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp783;_tmp783.tag=Cyc_AbstractDeclarator_tok;_tmp783.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp784=_cycalloc(sizeof(*_tmp784));_tmp784->tms=({struct Cyc_List_List*_tmp785=
_cycalloc(sizeof(*_tmp785));_tmp785->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp786=_cycalloc(sizeof(*_tmp786));_tmp786[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp787;_tmp787.tag=3;_tmp787.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp788=_cycalloc(sizeof(*_tmp788));_tmp788[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp789;_tmp789.tag=1;_tmp789.f1=0;_tmp789.f2=0;_tmp789.f3=0;_tmp789.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp789.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp789;});_tmp788;}));
_tmp787;});_tmp786;}));_tmp785->tl=0;_tmp785;});_tmp784;});_tmp783;});_tmp782;});
break;case 253: _LL2F6: {struct _tuple14 _tmp78B;struct Cyc_List_List*_tmp78C;int
_tmp78D;struct Cyc_Absyn_VarargInfo*_tmp78E;struct Cyc_Core_Opt*_tmp78F;struct Cyc_List_List*
_tmp790;struct _tuple14*_tmp78A=Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp78B=*_tmp78A;_tmp78C=_tmp78B.f1;_tmp78D=_tmp78B.f2;
_tmp78E=_tmp78B.f3;_tmp78F=_tmp78B.f4;_tmp790=_tmp78B.f5;yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp791=_cycalloc(sizeof(*_tmp791));_tmp791[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp792;_tmp792.tag=Cyc_AbstractDeclarator_tok;
_tmp792.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp793=_cycalloc(sizeof(*
_tmp793));_tmp793->tms=({struct Cyc_List_List*_tmp794=_cycalloc(sizeof(*_tmp794));
_tmp794->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp795=
_cycalloc(sizeof(*_tmp795));_tmp795[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp796;_tmp796.tag=3;_tmp796.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp797=_cycalloc(sizeof(*_tmp797));_tmp797[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp798;_tmp798.tag=1;_tmp798.f1=_tmp78C;_tmp798.f2=_tmp78D;_tmp798.f3=_tmp78E;
_tmp798.f4=_tmp78F;_tmp798.f5=_tmp790;_tmp798;});_tmp797;}));_tmp796;});_tmp795;}));
_tmp794->tl=0;_tmp794;});_tmp793;});_tmp792;});_tmp791;});break;}case 254: _LL2F7:
yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp799=_cycalloc(sizeof(*
_tmp799));_tmp799[0]=({struct Cyc_AbstractDeclarator_tok_struct _tmp79A;_tmp79A.tag=
Cyc_AbstractDeclarator_tok;_tmp79A.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp79B=_cycalloc(sizeof(*_tmp79B));_tmp79B->tms=({struct Cyc_List_List*_tmp79C=
_cycalloc(sizeof(*_tmp79C));_tmp79C->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp79D=_cycalloc(sizeof(*_tmp79D));_tmp79D[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp79E;_tmp79E.tag=3;_tmp79E.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp79F=_cycalloc(sizeof(*_tmp79F));_tmp79F[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp7A0;_tmp7A0.tag=1;_tmp7A0.f1=0;_tmp7A0.f2=0;_tmp7A0.f3=0;_tmp7A0.f4=Cyc_yyget_TypeOpt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7A0.f5=Cyc_yyget_Rgnorder_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7A0;});_tmp79F;}));
_tmp79E;});_tmp79D;}));_tmp79C->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp79C;});_tmp79B;});
_tmp79A;});_tmp799;});break;case 255: _LL2F8: {struct _tuple14 _tmp7A2;struct Cyc_List_List*
_tmp7A3;int _tmp7A4;struct Cyc_Absyn_VarargInfo*_tmp7A5;struct Cyc_Core_Opt*_tmp7A6;
struct Cyc_List_List*_tmp7A7;struct _tuple14*_tmp7A1=Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7A2=*_tmp7A1;_tmp7A3=
_tmp7A2.f1;_tmp7A4=_tmp7A2.f2;_tmp7A5=_tmp7A2.f3;_tmp7A6=_tmp7A2.f4;_tmp7A7=
_tmp7A2.f5;yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*_tmp7A8=
_cycalloc(sizeof(*_tmp7A8));_tmp7A8[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp7A9;_tmp7A9.tag=Cyc_AbstractDeclarator_tok;_tmp7A9.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp7AA=_cycalloc(sizeof(*_tmp7AA));_tmp7AA->tms=({struct Cyc_List_List*_tmp7AB=
_cycalloc(sizeof(*_tmp7AB));_tmp7AB->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp7AC=_cycalloc(sizeof(*_tmp7AC));_tmp7AC[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp7AD;_tmp7AD.tag=3;_tmp7AD.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp7AE=_cycalloc(sizeof(*_tmp7AE));_tmp7AE[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp7AF;_tmp7AF.tag=1;_tmp7AF.f1=_tmp7A3;_tmp7AF.f2=_tmp7A4;_tmp7AF.f3=_tmp7A5;
_tmp7AF.f4=_tmp7A6;_tmp7AF.f5=_tmp7A7;_tmp7AF;});_tmp7AE;}));_tmp7AD;});_tmp7AC;}));
_tmp7AB->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp7AB;});_tmp7AA;});
_tmp7A9;});_tmp7A8;});break;}case 256: _LL2F9: {struct Cyc_List_List*_tmp7B0=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(void*)({struct
Cyc_AbstractDeclarator_tok_struct*_tmp7B1=_cycalloc(sizeof(*_tmp7B1));_tmp7B1[0]=({
struct Cyc_AbstractDeclarator_tok_struct _tmp7B2;_tmp7B2.tag=Cyc_AbstractDeclarator_tok;
_tmp7B2.f1=({struct Cyc_Parse_Abstractdeclarator*_tmp7B3=_cycalloc(sizeof(*
_tmp7B3));_tmp7B3->tms=({struct Cyc_List_List*_tmp7B4=_cycalloc(sizeof(*_tmp7B4));
_tmp7B4->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp7B5=
_cycalloc(sizeof(*_tmp7B5));_tmp7B5[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp7B6;_tmp7B6.tag=4;_tmp7B6.f1=_tmp7B0;_tmp7B6.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7B6.f3=0;
_tmp7B6;});_tmp7B5;}));_tmp7B4->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp7B4;});_tmp7B3;});
_tmp7B2;});_tmp7B1;});break;}case 257: _LL2FA: yyval=(void*)({struct Cyc_AbstractDeclarator_tok_struct*
_tmp7B7=_cycalloc(sizeof(*_tmp7B7));_tmp7B7[0]=({struct Cyc_AbstractDeclarator_tok_struct
_tmp7B8;_tmp7B8.tag=Cyc_AbstractDeclarator_tok;_tmp7B8.f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp7B9=_cycalloc(sizeof(*_tmp7B9));_tmp7B9->tms=({struct Cyc_List_List*_tmp7BA=
_cycalloc(sizeof(*_tmp7BA));_tmp7BA->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp7BB=_cycalloc(sizeof(*_tmp7BB));_tmp7BB[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp7BC;_tmp7BC.tag=5;_tmp7BC.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp7BC.f2=Cyc_yyget_AttributeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7BC;});_tmp7BB;}));
_tmp7BA->tl=(Cyc_yyget_AbstractDeclarator_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms;_tmp7BA;});_tmp7B9;});
_tmp7B8;});_tmp7B7;});break;case 258: _LL2FB: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 259: _LL2FC: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 260: _LL2FD:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 261:
_LL2FE: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
262: _LL2FF: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 263: _LL300: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 264: _LL301: if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp7BD="`H";
_tag_dynforward(_tmp7BD,sizeof(char),_get_zero_arr_size(_tmp7BD,3));}))== 0  || 
Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp7BE="`U";
_tag_dynforward(_tmp7BE,sizeof(char),_get_zero_arr_size(_tmp7BE,3));}))== 0)Cyc_Parse_err((
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp7C1;_tmp7C1.tag=0;_tmp7C1.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{void*_tmp7BF[1]={&
_tmp7C1};Cyc_aprintf(({const char*_tmp7C0="bad occurrence of heap region %s";
_tag_dynforward(_tmp7C0,sizeof(char),_get_zero_arr_size(_tmp7C0,33));}),
_tag_dynforward(_tmp7BF,sizeof(void*),1));}}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp7CC=_cycalloc(sizeof(*_tmp7CC));_tmp7CC->name=({
struct _dynforward_ptr*_tmp7CF=_cycalloc(sizeof(*_tmp7CF));_tmp7CF[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7CF;});_tmp7CC->identity=
0;_tmp7CC->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp7CD=_cycalloc(
sizeof(*_tmp7CD));_tmp7CD[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp7CE;_tmp7CE.tag=0;
_tmp7CE.f1=(void*)((void*)3);_tmp7CE;});_tmp7CD;}));_tmp7CC;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp7CA=_cycalloc(sizeof(*_tmp7CA));_tmp7CA[0]=({
struct Cyc_Absyn_VarType_struct _tmp7CB;_tmp7CB.tag=1;_tmp7CB.f1=tv;_tmp7CB;});
_tmp7CA;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7C2=_cycalloc(sizeof(*
_tmp7C2));_tmp7C2[0]=({struct Cyc_Stmt_tok_struct _tmp7C3;_tmp7C3.tag=Cyc_Stmt_tok;
_tmp7C3.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp7C4=
_cycalloc(sizeof(*_tmp7C4));_tmp7C4[0]=({struct Cyc_Absyn_Region_s_struct _tmp7C5;
_tmp7C5.tag=15;_tmp7C5.f1=tv;_tmp7C5.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp7C6=_cycalloc(sizeof(*_tmp7C6));_tmp7C6->f1=(void*)0;_tmp7C6->f2=({struct
_dynforward_ptr*_tmp7C7=_cycalloc(sizeof(*_tmp7C7));_tmp7C7[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7C7;});_tmp7C6;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp7C8=_cycalloc(sizeof(*_tmp7C8));
_tmp7C8[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp7C9;_tmp7C9.tag=15;_tmp7C9.f1=(
void*)t;_tmp7C9;});_tmp7C8;}),0);_tmp7C5.f3=0;_tmp7C5.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7C5;});_tmp7C4;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C3;});
_tmp7C2;});break;}case 265: _LL302: if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp7D0="H";
_tag_dynforward(_tmp7D0,sizeof(char),_get_zero_arr_size(_tmp7D0,2));}))== 0)Cyc_Parse_err(({
const char*_tmp7D1="bad occurrence of heap region `H";_tag_dynforward(_tmp7D1,
sizeof(char),_get_zero_arr_size(_tmp7D1,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp7DC=_cycalloc(sizeof(*_tmp7DC));_tmp7DC->name=({
struct _dynforward_ptr*_tmp7DF=_cycalloc(sizeof(*_tmp7DF));_tmp7DF[0]=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp7E2;_tmp7E2.tag=0;_tmp7E2.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp7E0[1]={&
_tmp7E2};Cyc_aprintf(({const char*_tmp7E1="`%s";_tag_dynforward(_tmp7E1,sizeof(
char),_get_zero_arr_size(_tmp7E1,4));}),_tag_dynforward(_tmp7E0,sizeof(void*),1));}});
_tmp7DF;});_tmp7DC->identity=0;_tmp7DC->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp7DD=_cycalloc(sizeof(*_tmp7DD));_tmp7DD[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp7DE;_tmp7DE.tag=0;_tmp7DE.f1=(void*)((void*)3);_tmp7DE;});_tmp7DD;}));
_tmp7DC;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp7DA=_cycalloc(
sizeof(*_tmp7DA));_tmp7DA[0]=({struct Cyc_Absyn_VarType_struct _tmp7DB;_tmp7DB.tag=
1;_tmp7DB.f1=tv;_tmp7DB;});_tmp7DA;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp7D2=_cycalloc(sizeof(*_tmp7D2));_tmp7D2[0]=({struct Cyc_Stmt_tok_struct
_tmp7D3;_tmp7D3.tag=Cyc_Stmt_tok;_tmp7D3.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp7D4=_cycalloc(sizeof(*_tmp7D4));_tmp7D4[0]=({
struct Cyc_Absyn_Region_s_struct _tmp7D5;_tmp7D5.tag=15;_tmp7D5.f1=tv;_tmp7D5.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7D6=_cycalloc(sizeof(*_tmp7D6));
_tmp7D6->f1=(void*)0;_tmp7D6->f2=({struct _dynforward_ptr*_tmp7D7=_cycalloc(
sizeof(*_tmp7D7));_tmp7D7[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7D7;});_tmp7D6;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp7D8=_cycalloc(sizeof(*_tmp7D8));
_tmp7D8[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp7D9;_tmp7D9.tag=15;_tmp7D9.f1=(
void*)t;_tmp7D9;});_tmp7D8;}),0);_tmp7D5.f3=0;_tmp7D5.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7D5;});_tmp7D4;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7D3;});
_tmp7D2;});break;}case 266: _LL303: if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),({const char*_tmp7E3="resetable";
_tag_dynforward(_tmp7E3,sizeof(char),_get_zero_arr_size(_tmp7E3,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp7E4="expecting [resetable]";_tag_dynforward(_tmp7E4,sizeof(char),
_get_zero_arr_size(_tmp7E4,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line));if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),({const char*_tmp7E5="`H";_tag_dynforward(_tmp7E5,
sizeof(char),_get_zero_arr_size(_tmp7E5,3));}))== 0  || Cyc_zstrcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),({const char*_tmp7E6="`U";_tag_dynforward(_tmp7E6,sizeof(char),
_get_zero_arr_size(_tmp7E6,3));})))Cyc_Parse_err((struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp7E9;_tmp7E9.tag=0;_tmp7E9.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));{void*_tmp7E7[1]={& _tmp7E9};Cyc_aprintf(({const char*
_tmp7E8="bad occurrence of heap region %s";_tag_dynforward(_tmp7E8,sizeof(char),
_get_zero_arr_size(_tmp7E8,33));}),_tag_dynforward(_tmp7E7,sizeof(void*),1));}}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp7F4=_cycalloc(sizeof(*_tmp7F4));_tmp7F4->name=({struct _dynforward_ptr*
_tmp7F7=_cycalloc(sizeof(*_tmp7F7));_tmp7F7[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7F7;});_tmp7F4->identity=
0;_tmp7F4->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp7F5=_cycalloc(
sizeof(*_tmp7F5));_tmp7F5[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp7F6;_tmp7F6.tag=0;
_tmp7F6.f1=(void*)((void*)3);_tmp7F6;});_tmp7F5;}));_tmp7F4;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp7F2=_cycalloc(sizeof(*_tmp7F2));_tmp7F2[0]=({
struct Cyc_Absyn_VarType_struct _tmp7F3;_tmp7F3.tag=1;_tmp7F3.f1=tv;_tmp7F3;});
_tmp7F2;});yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp7EA=_cycalloc(sizeof(*
_tmp7EA));_tmp7EA[0]=({struct Cyc_Stmt_tok_struct _tmp7EB;_tmp7EB.tag=Cyc_Stmt_tok;
_tmp7EB.f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*_tmp7EC=
_cycalloc(sizeof(*_tmp7EC));_tmp7EC[0]=({struct Cyc_Absyn_Region_s_struct _tmp7ED;
_tmp7ED.tag=15;_tmp7ED.f1=tv;_tmp7ED.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp7EE=_cycalloc(sizeof(*_tmp7EE));_tmp7EE->f1=(void*)0;_tmp7EE->f2=({struct
_dynforward_ptr*_tmp7EF=_cycalloc(sizeof(*_tmp7EF));_tmp7EF[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7EF;});_tmp7EE;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp7F0=_cycalloc(sizeof(*_tmp7F0));
_tmp7F0[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp7F1;_tmp7F1.tag=15;_tmp7F1.f1=(
void*)t;_tmp7F1;});_tmp7F0;}),0);_tmp7ED.f3=1;_tmp7ED.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7ED;});_tmp7EC;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7EB;});
_tmp7EA;});break;}case 267: _LL304: if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp7F8="resetable";
_tag_dynforward(_tmp7F8,sizeof(char),_get_zero_arr_size(_tmp7F8,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp7F9="expecting `resetable'";_tag_dynforward(_tmp7F9,sizeof(char),
_get_zero_arr_size(_tmp7F9,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),({const char*_tmp7FA="H";_tag_dynforward(_tmp7FA,sizeof(
char),_get_zero_arr_size(_tmp7FA,2));}))== 0)Cyc_Parse_err(({const char*_tmp7FB="bad occurrence of heap region `H";
_tag_dynforward(_tmp7FB,sizeof(char),_get_zero_arr_size(_tmp7FB,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp806=_cycalloc(sizeof(*_tmp806));_tmp806->name=({
struct _dynforward_ptr*_tmp809=_cycalloc(sizeof(*_tmp809));_tmp809[0]=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp80C;_tmp80C.tag=0;_tmp80C.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp80A[1]={&
_tmp80C};Cyc_aprintf(({const char*_tmp80B="`%s";_tag_dynforward(_tmp80B,sizeof(
char),_get_zero_arr_size(_tmp80B,4));}),_tag_dynforward(_tmp80A,sizeof(void*),1));}});
_tmp809;});_tmp806->identity=0;_tmp806->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp807=_cycalloc(sizeof(*_tmp807));_tmp807[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp808;_tmp808.tag=0;_tmp808.f1=(void*)((void*)3);_tmp808;});_tmp807;}));
_tmp806;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp804=_cycalloc(
sizeof(*_tmp804));_tmp804[0]=({struct Cyc_Absyn_VarType_struct _tmp805;_tmp805.tag=
1;_tmp805.f1=tv;_tmp805;});_tmp804;});yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp7FC=_cycalloc(sizeof(*_tmp7FC));_tmp7FC[0]=({struct Cyc_Stmt_tok_struct
_tmp7FD;_tmp7FD.tag=Cyc_Stmt_tok;_tmp7FD.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp7FE=_cycalloc(sizeof(*_tmp7FE));_tmp7FE[0]=({
struct Cyc_Absyn_Region_s_struct _tmp7FF;_tmp7FF.tag=15;_tmp7FF.f1=tv;_tmp7FF.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp800=_cycalloc(sizeof(*_tmp800));
_tmp800->f1=(void*)0;_tmp800->f2=({struct _dynforward_ptr*_tmp801=_cycalloc(
sizeof(*_tmp801));_tmp801[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp801;});_tmp800;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp802=_cycalloc(sizeof(*_tmp802));
_tmp802[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp803;_tmp803.tag=15;_tmp803.f1=(
void*)t;_tmp803;});_tmp802;}),0);_tmp7FF.f3=1;_tmp7FF.f4=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7FF;});_tmp7FE;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7FD;});
_tmp7FC;});break;}case 268: _LL305: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp80D=_cycalloc(sizeof(*_tmp80D));_tmp80D[0]=({struct Cyc_Stmt_tok_struct
_tmp80E;_tmp80E.tag=Cyc_Stmt_tok;_tmp80E.f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_ResetRegion_s_struct*_tmp80F=_cycalloc(sizeof(*_tmp80F));_tmp80F[0]=({
struct Cyc_Absyn_ResetRegion_s_struct _tmp810;_tmp810.tag=16;_tmp810.f1=Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp810;});_tmp80F;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp80E;});_tmp80D;});break;case 269: _LL306: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp811=_cycalloc(sizeof(*_tmp811));_tmp811[0]=({
struct Cyc_Stmt_tok_struct _tmp812;_tmp812.tag=Cyc_Stmt_tok;_tmp812.f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmp813=_cycalloc(sizeof(*_tmp813));
_tmp813[0]=({struct Cyc_Absyn_Label_s_struct _tmp814;_tmp814.tag=12;_tmp814.f1=({
struct _dynforward_ptr*_tmp815=_cycalloc(sizeof(*_tmp815));_tmp815[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp815;});_tmp814.f2=
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp814;});_tmp813;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp812;});
_tmp811;});break;case 270: _LL307: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp816=
_cycalloc(sizeof(*_tmp816));_tmp816[0]=({struct Cyc_Stmt_tok_struct _tmp817;
_tmp817.tag=Cyc_Stmt_tok;_tmp817.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp817;});
_tmp816;});break;case 271: _LL308: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp818=
_cycalloc(sizeof(*_tmp818));_tmp818[0]=({struct Cyc_Stmt_tok_struct _tmp819;
_tmp819.tag=Cyc_Stmt_tok;_tmp819.f1=Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp819;});
_tmp818;});break;case 272: _LL309: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp81A=
_cycalloc(sizeof(*_tmp81A));_tmp81A[0]=({struct Cyc_Stmt_tok_struct _tmp81B;
_tmp81B.tag=Cyc_Stmt_tok;_tmp81B.f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp81B;});
_tmp81A;});break;case 273: _LL30A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 274: _LL30B: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp81C=_cycalloc(sizeof(*_tmp81C));_tmp81C[0]=({struct Cyc_Stmt_tok_struct
_tmp81D;_tmp81D.tag=Cyc_Stmt_tok;_tmp81D.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0));
_tmp81D;});_tmp81C;});break;case 275: _LL30C: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp81E=_cycalloc(sizeof(*_tmp81E));_tmp81E[0]=({struct Cyc_Stmt_tok_struct
_tmp81F;_tmp81F.tag=Cyc_Stmt_tok;_tmp81F.f1=Cyc_Parse_flatten_declarations(Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp81F;});_tmp81E;});
break;case 276: _LL30D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 277: _LL30E: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp820=_cycalloc(sizeof(*_tmp820));_tmp820[0]=({struct Cyc_Stmt_tok_struct
_tmp821;_tmp821.tag=Cyc_Stmt_tok;_tmp821.f1=Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp821;});
_tmp820;});break;case 278: _LL30F: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp822=
_cycalloc(sizeof(*_tmp822));_tmp822[0]=({struct Cyc_Stmt_tok_struct _tmp823;
_tmp823.tag=Cyc_Stmt_tok;_tmp823.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp824=_cycalloc(sizeof(*_tmp824));_tmp824[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp825;_tmp825.tag=1;_tmp825.f1=Cyc_yyget_FnDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp825;});_tmp824;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0));_tmp823;});_tmp822;});break;
case 279: _LL310: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp826=_cycalloc(
sizeof(*_tmp826));_tmp826[0]=({struct Cyc_Stmt_tok_struct _tmp827;_tmp827.tag=Cyc_Stmt_tok;
_tmp827.f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp828=_cycalloc(sizeof(*_tmp828));_tmp828[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp829;_tmp829.tag=1;_tmp829.f1=Cyc_yyget_FnDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp829;});_tmp828;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp827;});_tmp826;});
break;case 280: _LL311: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp82A=_cycalloc(
sizeof(*_tmp82A));_tmp82A[0]=({struct Cyc_Stmt_tok_struct _tmp82B;_tmp82B.tag=Cyc_Stmt_tok;
_tmp82B.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp82B;});
_tmp82A;});break;case 281: _LL312: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp82C=
_cycalloc(sizeof(*_tmp82C));_tmp82C[0]=({struct Cyc_Stmt_tok_struct _tmp82D;
_tmp82D.tag=Cyc_Stmt_tok;_tmp82D.f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp82D;});
_tmp82C;});break;case 282: _LL313: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp82E=
_cycalloc(sizeof(*_tmp82E));_tmp82E[0]=({struct Cyc_Stmt_tok_struct _tmp82F;
_tmp82F.tag=Cyc_Stmt_tok;_tmp82F.f1=Cyc_Absyn_switch_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp82F;});
_tmp82E;});break;case 283: _LL314: {struct Cyc_Absyn_Exp*_tmp830=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp833=_cycalloc(sizeof(*_tmp833));
_tmp833[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp834;_tmp834.tag=2;_tmp834.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp834;});_tmp833;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp831=_cycalloc(sizeof(*_tmp831));_tmp831[0]=({
struct Cyc_Stmt_tok_struct _tmp832;_tmp832.tag=Cyc_Stmt_tok;_tmp832.f1=Cyc_Absyn_switch_stmt(
_tmp830,Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp832;});
_tmp831;});break;}case 284: _LL315: {struct Cyc_Absyn_Exp*_tmp835=Cyc_Absyn_tuple_exp(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp836=
_cycalloc(sizeof(*_tmp836));_tmp836[0]=({struct Cyc_Stmt_tok_struct _tmp837;
_tmp837.tag=Cyc_Stmt_tok;_tmp837.f1=Cyc_Absyn_switch_stmt(_tmp835,Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp837;});
_tmp836;});break;}case 285: _LL316: yyval=(void*)({struct Cyc_Stmt_tok_struct*
_tmp838=_cycalloc(sizeof(*_tmp838));_tmp838[0]=({struct Cyc_Stmt_tok_struct
_tmp839;_tmp839.tag=Cyc_Stmt_tok;_tmp839.f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_SwitchClauseList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp839;});
_tmp838;});break;case 286: _LL317: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp83A=_cycalloc(sizeof(*_tmp83A));_tmp83A[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp83B;_tmp83B.tag=Cyc_SwitchClauseList_tok;_tmp83B.f1=0;_tmp83B;});_tmp83A;});
break;case 287: _LL318: yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*
_tmp83C=_cycalloc(sizeof(*_tmp83C));_tmp83C[0]=({struct Cyc_SwitchClauseList_tok_struct
_tmp83D;_tmp83D.tag=Cyc_SwitchClauseList_tok;_tmp83D.f1=({struct Cyc_List_List*
_tmp83E=_cycalloc(sizeof(*_tmp83E));_tmp83E->hd=({struct Cyc_Absyn_Switch_clause*
_tmp83F=_cycalloc(sizeof(*_tmp83F));_tmp83F->pattern=Cyc_Absyn_new_pat((void*)0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));_tmp83F->pat_vars=0;_tmp83F->where_clause=0;
_tmp83F->body=Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp83F->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp83F;});
_tmp83E->tl=0;_tmp83E;});_tmp83D;});_tmp83C;});break;case 288: _LL319: yyval=(void*)({
struct Cyc_SwitchClauseList_tok_struct*_tmp840=_cycalloc(sizeof(*_tmp840));
_tmp840[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp841;_tmp841.tag=Cyc_SwitchClauseList_tok;
_tmp841.f1=({struct Cyc_List_List*_tmp842=_cycalloc(sizeof(*_tmp842));_tmp842->hd=({
struct Cyc_Absyn_Switch_clause*_tmp843=_cycalloc(sizeof(*_tmp843));_tmp843->pattern=
Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp843->pat_vars=0;_tmp843->where_clause=0;_tmp843->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp843->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp843;});
_tmp842->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp842;});_tmp841;});_tmp840;});break;case 289: _LL31A:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp844=_cycalloc(sizeof(*
_tmp844));_tmp844[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp845;_tmp845.tag=
Cyc_SwitchClauseList_tok;_tmp845.f1=({struct Cyc_List_List*_tmp846=_cycalloc(
sizeof(*_tmp846));_tmp846->hd=({struct Cyc_Absyn_Switch_clause*_tmp847=_cycalloc(
sizeof(*_tmp847));_tmp847->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp847->pat_vars=0;
_tmp847->where_clause=0;_tmp847->body=Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp847->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp847;});
_tmp846->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp846;});_tmp845;});_tmp844;});break;case 290: _LL31B:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp848=_cycalloc(sizeof(*
_tmp848));_tmp848[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp849;_tmp849.tag=
Cyc_SwitchClauseList_tok;_tmp849.f1=({struct Cyc_List_List*_tmp84A=_cycalloc(
sizeof(*_tmp84A));_tmp84A->hd=({struct Cyc_Absyn_Switch_clause*_tmp84B=_cycalloc(
sizeof(*_tmp84B));_tmp84B->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp84B->pat_vars=0;
_tmp84B->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp84B->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp84B->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp84B;});
_tmp84A->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp84A;});_tmp849;});_tmp848;});break;case 291: _LL31C:
yyval=(void*)({struct Cyc_SwitchClauseList_tok_struct*_tmp84C=_cycalloc(sizeof(*
_tmp84C));_tmp84C[0]=({struct Cyc_SwitchClauseList_tok_struct _tmp84D;_tmp84D.tag=
Cyc_SwitchClauseList_tok;_tmp84D.f1=({struct Cyc_List_List*_tmp84E=_cycalloc(
sizeof(*_tmp84E));_tmp84E->hd=({struct Cyc_Absyn_Switch_clause*_tmp84F=_cycalloc(
sizeof(*_tmp84F));_tmp84F->pattern=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp84F->pat_vars=0;
_tmp84F->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp84F->body=Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp84F->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp84F;});
_tmp84E->tl=Cyc_yyget_SwitchClauseList_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp84E;});_tmp84D;});_tmp84C;});break;case 292: _LL31D:
yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp850=_cycalloc(sizeof(*_tmp850));
_tmp850[0]=({struct Cyc_Stmt_tok_struct _tmp851;_tmp851.tag=Cyc_Stmt_tok;_tmp851.f1=
Cyc_Absyn_while_stmt(Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp851;});_tmp850;});break;case 293: _LL31E: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp852=_cycalloc(sizeof(*_tmp852));_tmp852[0]=({
struct Cyc_Stmt_tok_struct _tmp853;_tmp853.tag=Cyc_Stmt_tok;_tmp853.f1=Cyc_Absyn_do_stmt(
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp853;});_tmp852;});break;case 294: _LL31F: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp854=_cycalloc(sizeof(*_tmp854));_tmp854[0]=({
struct Cyc_Stmt_tok_struct _tmp855;_tmp855.tag=Cyc_Stmt_tok;_tmp855.f1=Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp855;});
_tmp854;});break;case 295: _LL320: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp856=
_cycalloc(sizeof(*_tmp856));_tmp856[0]=({struct Cyc_Stmt_tok_struct _tmp857;
_tmp857.tag=Cyc_Stmt_tok;_tmp857.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp857;});
_tmp856;});break;case 296: _LL321: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp858=
_cycalloc(sizeof(*_tmp858));_tmp858[0]=({struct Cyc_Stmt_tok_struct _tmp859;
_tmp859.tag=Cyc_Stmt_tok;_tmp859.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp859;});
_tmp858;});break;case 297: _LL322: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp85A=
_cycalloc(sizeof(*_tmp85A));_tmp85A[0]=({struct Cyc_Stmt_tok_struct _tmp85B;
_tmp85B.tag=Cyc_Stmt_tok;_tmp85B.f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp85B;});
_tmp85A;});break;case 298: _LL323: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp85C=
_cycalloc(sizeof(*_tmp85C));_tmp85C[0]=({struct Cyc_Stmt_tok_struct _tmp85D;
_tmp85D.tag=Cyc_Stmt_tok;_tmp85D.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85D;});
_tmp85C;});break;case 299: _LL324: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp85E=
_cycalloc(sizeof(*_tmp85E));_tmp85E[0]=({struct Cyc_Stmt_tok_struct _tmp85F;
_tmp85F.tag=Cyc_Stmt_tok;_tmp85F.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(0),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85F;});
_tmp85E;});break;case 300: _LL325: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp860=
_cycalloc(sizeof(*_tmp860));_tmp860[0]=({struct Cyc_Stmt_tok_struct _tmp861;
_tmp861.tag=Cyc_Stmt_tok;_tmp861.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp861;});
_tmp860;});break;case 301: _LL326: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp862=
_cycalloc(sizeof(*_tmp862));_tmp862[0]=({struct Cyc_Stmt_tok_struct _tmp863;
_tmp863.tag=Cyc_Stmt_tok;_tmp863.f1=Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp863;});
_tmp862;});break;case 302: _LL327: {struct Cyc_List_List*_tmp864=Cyc_yyget_DeclList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp865=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp866=
_cycalloc(sizeof(*_tmp866));_tmp866[0]=({struct Cyc_Stmt_tok_struct _tmp867;
_tmp867.tag=Cyc_Stmt_tok;_tmp867.f1=Cyc_Parse_flatten_declarations(_tmp864,
_tmp865);_tmp867;});_tmp866;});break;}case 303: _LL328: {struct Cyc_List_List*
_tmp868=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp869=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp86A=_cycalloc(sizeof(*_tmp86A));_tmp86A[0]=({
struct Cyc_Stmt_tok_struct _tmp86B;_tmp86B.tag=Cyc_Stmt_tok;_tmp86B.f1=Cyc_Parse_flatten_declarations(
_tmp868,_tmp869);_tmp86B;});_tmp86A;});break;}case 304: _LL329: {struct Cyc_List_List*
_tmp86C=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp86D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp86E=_cycalloc(sizeof(*_tmp86E));_tmp86E[0]=({
struct Cyc_Stmt_tok_struct _tmp86F;_tmp86F.tag=Cyc_Stmt_tok;_tmp86F.f1=Cyc_Parse_flatten_declarations(
_tmp86C,_tmp86D);_tmp86F;});_tmp86E;});break;}case 305: _LL32A: {struct Cyc_List_List*
_tmp870=Cyc_yyget_DeclList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp871=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_Stmt_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp872=_cycalloc(sizeof(*_tmp872));_tmp872[0]=({
struct Cyc_Stmt_tok_struct _tmp873;_tmp873.tag=Cyc_Stmt_tok;_tmp873.f1=Cyc_Parse_flatten_declarations(
_tmp870,_tmp871);_tmp873;});_tmp872;});break;}case 306: _LL32B: yyval=(void*)({
struct Cyc_Stmt_tok_struct*_tmp874=_cycalloc(sizeof(*_tmp874));_tmp874[0]=({
struct Cyc_Stmt_tok_struct _tmp875;_tmp875.tag=Cyc_Stmt_tok;_tmp875.f1=Cyc_Absyn_goto_stmt(({
struct _dynforward_ptr*_tmp876=_cycalloc(sizeof(*_tmp876));_tmp876[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp876;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp875;});
_tmp874;});break;case 307: _LL32C: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp877=
_cycalloc(sizeof(*_tmp877));_tmp877[0]=({struct Cyc_Stmt_tok_struct _tmp878;
_tmp878.tag=Cyc_Stmt_tok;_tmp878.f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp878;});
_tmp877;});break;case 308: _LL32D: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp879=
_cycalloc(sizeof(*_tmp879));_tmp879[0]=({struct Cyc_Stmt_tok_struct _tmp87A;
_tmp87A.tag=Cyc_Stmt_tok;_tmp87A.f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp87A;});
_tmp879;});break;case 309: _LL32E: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp87B=
_cycalloc(sizeof(*_tmp87B));_tmp87B[0]=({struct Cyc_Stmt_tok_struct _tmp87C;
_tmp87C.tag=Cyc_Stmt_tok;_tmp87C.f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp87C;});
_tmp87B;});break;case 310: _LL32F: yyval=(void*)({struct Cyc_Stmt_tok_struct*_tmp87D=
_cycalloc(sizeof(*_tmp87D));_tmp87D[0]=({struct Cyc_Stmt_tok_struct _tmp87E;
_tmp87E.tag=Cyc_Stmt_tok;_tmp87E.f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp87E;});_tmp87D;});break;case 311: _LL330: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp87F=_cycalloc(sizeof(*_tmp87F));_tmp87F[0]=({
struct Cyc_Stmt_tok_struct _tmp880;_tmp880.tag=Cyc_Stmt_tok;_tmp880.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp880;});_tmp87F;});break;case 312: _LL331: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp881=_cycalloc(sizeof(*_tmp881));_tmp881[0]=({
struct Cyc_Stmt_tok_struct _tmp882;_tmp882.tag=Cyc_Stmt_tok;_tmp882.f1=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));_tmp882;});_tmp881;});break;case 313: _LL332: yyval=(
void*)({struct Cyc_Stmt_tok_struct*_tmp883=_cycalloc(sizeof(*_tmp883));_tmp883[0]=({
struct Cyc_Stmt_tok_struct _tmp884;_tmp884.tag=Cyc_Stmt_tok;_tmp884.f1=Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));_tmp884;});_tmp883;});break;case 314: _LL333: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 315: _LL334:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 316:
_LL335: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp885=_cycalloc(sizeof(*
_tmp885));_tmp885[0]=({struct Cyc_Pattern_tok_struct _tmp886;_tmp886.tag=Cyc_Pattern_tok;
_tmp886.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp886;});
_tmp885;});break;case 317: _LL336: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 318: _LL337: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp887=_cycalloc(sizeof(*_tmp887));_tmp887[0]=({struct Cyc_Pattern_tok_struct
_tmp888;_tmp888.tag=Cyc_Pattern_tok;_tmp888.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp888;});
_tmp887;});break;case 319: _LL338: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 320: _LL339: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp889=_cycalloc(sizeof(*_tmp889));_tmp889[0]=({struct Cyc_Pattern_tok_struct
_tmp88A;_tmp88A.tag=Cyc_Pattern_tok;_tmp88A.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp88A;});
_tmp889;});break;case 321: _LL33A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 322: _LL33B: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp88B=_cycalloc(sizeof(*_tmp88B));_tmp88B[0]=({struct Cyc_Pattern_tok_struct
_tmp88C;_tmp88C.tag=Cyc_Pattern_tok;_tmp88C.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)14,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp88C;});
_tmp88B;});break;case 323: _LL33C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 324: _LL33D: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp88D=_cycalloc(sizeof(*_tmp88D));_tmp88D[0]=({struct Cyc_Pattern_tok_struct
_tmp88E;_tmp88E.tag=Cyc_Pattern_tok;_tmp88E.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)15,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp88E;});
_tmp88D;});break;case 325: _LL33E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 326: _LL33F: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp88F=_cycalloc(sizeof(*_tmp88F));_tmp88F[0]=({struct Cyc_Pattern_tok_struct
_tmp890;_tmp890.tag=Cyc_Pattern_tok;_tmp890.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)13,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp890;});
_tmp88F;});break;case 327: _LL340: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 328: _LL341: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp891=_cycalloc(sizeof(*_tmp891));_tmp891[0]=({struct Cyc_Pattern_tok_struct
_tmp892;_tmp892.tag=Cyc_Pattern_tok;_tmp892.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp892;});
_tmp891;});break;case 329: _LL342: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp893=_cycalloc(sizeof(*_tmp893));_tmp893[0]=({struct Cyc_Pattern_tok_struct
_tmp894;_tmp894.tag=Cyc_Pattern_tok;_tmp894.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp894;});
_tmp893;});break;case 330: _LL343: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 331: _LL344: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp895=_cycalloc(sizeof(*_tmp895));_tmp895[0]=({struct Cyc_Pattern_tok_struct
_tmp896;_tmp896.tag=Cyc_Pattern_tok;_tmp896.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp896;});
_tmp895;});break;case 332: _LL345: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp897=_cycalloc(sizeof(*_tmp897));_tmp897[0]=({struct Cyc_Pattern_tok_struct
_tmp898;_tmp898.tag=Cyc_Pattern_tok;_tmp898.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp898;});
_tmp897;});break;case 333: _LL346: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp899=_cycalloc(sizeof(*_tmp899));_tmp899[0]=({struct Cyc_Pattern_tok_struct
_tmp89A;_tmp89A.tag=Cyc_Pattern_tok;_tmp89A.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp89A;});
_tmp899;});break;case 334: _LL347: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp89B=_cycalloc(sizeof(*_tmp89B));_tmp89B[0]=({struct Cyc_Pattern_tok_struct
_tmp89C;_tmp89C.tag=Cyc_Pattern_tok;_tmp89C.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp89C;});
_tmp89B;});break;case 335: _LL348: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 336: _LL349: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp89D=_cycalloc(sizeof(*_tmp89D));_tmp89D[0]=({struct Cyc_Pattern_tok_struct
_tmp89E;_tmp89E.tag=Cyc_Pattern_tok;_tmp89E.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)16,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp89E;});
_tmp89D;});break;case 337: _LL34A: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp89F=_cycalloc(sizeof(*_tmp89F));_tmp89F[0]=({struct Cyc_Pattern_tok_struct
_tmp8A0;_tmp8A0.tag=Cyc_Pattern_tok;_tmp8A0.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)17,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp8A0;});
_tmp89F;});break;case 338: _LL34B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 339: _LL34C: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8A1=_cycalloc(sizeof(*_tmp8A1));_tmp8A1[0]=({struct Cyc_Pattern_tok_struct
_tmp8A2;_tmp8A2.tag=Cyc_Pattern_tok;_tmp8A2.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)0,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp8A2;});
_tmp8A1;});break;case 340: _LL34D: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8A3=_cycalloc(sizeof(*_tmp8A3));_tmp8A3[0]=({struct Cyc_Pattern_tok_struct
_tmp8A4;_tmp8A4.tag=Cyc_Pattern_tok;_tmp8A4.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)2,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp8A4;});
_tmp8A3;});break;case 341: _LL34E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 342: _LL34F: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8A5=_cycalloc(sizeof(*_tmp8A5));_tmp8A5[0]=({struct Cyc_Pattern_tok_struct
_tmp8A6;_tmp8A6.tag=Cyc_Pattern_tok;_tmp8A6.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)1,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp8A6;});
_tmp8A5;});break;case 343: _LL350: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8A7=_cycalloc(sizeof(*_tmp8A7));_tmp8A7[0]=({struct Cyc_Pattern_tok_struct
_tmp8A8;_tmp8A8.tag=Cyc_Pattern_tok;_tmp8A8.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)3,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp8A8;});
_tmp8A7;});break;case 344: _LL351: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8A9=_cycalloc(sizeof(*_tmp8A9));_tmp8A9[0]=({struct Cyc_Pattern_tok_struct
_tmp8AA;_tmp8AA.tag=Cyc_Pattern_tok;_tmp8AA.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)4,Cyc_Parse_pat2exp(Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp8AA;});
_tmp8A9;});break;case 345: _LL352: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 346: _LL353: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8AB=_cycalloc(sizeof(*_tmp8AB));_tmp8AB[0]=({struct Cyc_Pattern_tok_struct
_tmp8AC;_tmp8AC.tag=Cyc_Pattern_tok;_tmp8AC.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)])).f3,Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp8AC;});
_tmp8AB;});break;case 347: _LL354: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 348: _LL355: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8AD=_cycalloc(sizeof(*_tmp8AD));_tmp8AD[0]=({struct Cyc_Pattern_tok_struct
_tmp8AE;_tmp8AE.tag=Cyc_Pattern_tok;_tmp8AE.f1=Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp8AE;});_tmp8AD;});
break;case 349: _LL356: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8AF=
_cycalloc(sizeof(*_tmp8AF));_tmp8AF[0]=({struct Cyc_Pattern_tok_struct _tmp8B0;
_tmp8B0.tag=Cyc_Pattern_tok;_tmp8B0.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp8B0;});
_tmp8AF;});break;case 350: _LL357: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8B1=_cycalloc(sizeof(*_tmp8B1));_tmp8B1[0]=({struct Cyc_Pattern_tok_struct
_tmp8B2;_tmp8B2.tag=Cyc_Pattern_tok;_tmp8B2.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)])).f3,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp8B2;});_tmp8B1;});break;case 351: _LL358: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp8B3=_cycalloc(sizeof(*_tmp8B3));_tmp8B3[
0]=({struct Cyc_Pattern_tok_struct _tmp8B4;_tmp8B4.tag=Cyc_Pattern_tok;_tmp8B4.f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp8B4;});
_tmp8B3;});break;case 352: _LL359: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8B5=_cycalloc(sizeof(*_tmp8B5));_tmp8B5[0]=({struct Cyc_Pattern_tok_struct
_tmp8B6;_tmp8B6.tag=Cyc_Pattern_tok;_tmp8B6.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*_tmp8B7=_cycalloc(sizeof(*
_tmp8B7));_tmp8B7[0]=({struct Cyc_Absyn_StructField_struct _tmp8B8;_tmp8B8.tag=0;
_tmp8B8.f1=({struct _dynforward_ptr*_tmp8B9=_cycalloc(sizeof(*_tmp8B9));_tmp8B9[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp8B9;});_tmp8B8;});_tmp8B7;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp8B6;});
_tmp8B5;});break;case 353: _LL35A: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8BA=_cycalloc(sizeof(*_tmp8BA));_tmp8BA[0]=({struct Cyc_Pattern_tok_struct
_tmp8BB;_tmp8BB.tag=Cyc_Pattern_tok;_tmp8BB.f1=Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp8BC=_cycalloc_atomic(
sizeof(*_tmp8BC));_tmp8BC[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp8BD;_tmp8BD.tag=
1;_tmp8BD.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp8BD;});_tmp8BC;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp8BB;});_tmp8BA;});break;case 354: _LL35B: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 355: _LL35C:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 356:
_LL35D: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8BE=_cycalloc(sizeof(*
_tmp8BE));_tmp8BE[0]=({struct Cyc_Pattern_tok_struct _tmp8BF;_tmp8BF.tag=Cyc_Pattern_tok;
_tmp8BF.f1=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8BF;});
_tmp8BE;});break;case 357: _LL35E: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp8C0=_cycalloc(sizeof(*_tmp8C0));_tmp8C0[0]=({struct Cyc_Pattern_tok_struct
_tmp8C1;_tmp8C1.tag=Cyc_Pattern_tok;_tmp8C1.f1=Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8C1;});_tmp8C0;});
break;case 358: _LL35F: {struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);{void*_tmp8C2=(void*)e->r;
void*_tmp8C3;void*_tmp8C4;char _tmp8C5;void*_tmp8C6;void*_tmp8C7;short _tmp8C8;
void*_tmp8C9;void*_tmp8CA;int _tmp8CB;void*_tmp8CC;struct _dynforward_ptr _tmp8CD;
void*_tmp8CE;void*_tmp8CF;void*_tmp8D0;_LL362: if(*((int*)_tmp8C2)!= 0)goto _LL364;
_tmp8C3=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp8C2)->f1;if(_tmp8C3 <= (void*)
1)goto _LL364;if(*((int*)_tmp8C3)!= 0)goto _LL364;_tmp8C4=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp8C3)->f1;_tmp8C5=((struct Cyc_Absyn_Char_c_struct*)_tmp8C3)->f2;_LL363: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp8D1=_cycalloc(sizeof(*_tmp8D1));_tmp8D1[
0]=({struct Cyc_Pattern_tok_struct _tmp8D2;_tmp8D2.tag=Cyc_Pattern_tok;_tmp8D2.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Char_p_struct*_tmp8D3=
_cycalloc_atomic(sizeof(*_tmp8D3));_tmp8D3[0]=({struct Cyc_Absyn_Char_p_struct
_tmp8D4;_tmp8D4.tag=8;_tmp8D4.f1=_tmp8C5;_tmp8D4;});_tmp8D3;}),e->loc);_tmp8D2;});
_tmp8D1;});goto _LL361;_LL364: if(*((int*)_tmp8C2)!= 0)goto _LL366;_tmp8C6=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp8C2)->f1;if(_tmp8C6 <= (void*)1)goto _LL366;
if(*((int*)_tmp8C6)!= 1)goto _LL366;_tmp8C7=(void*)((struct Cyc_Absyn_Short_c_struct*)
_tmp8C6)->f1;_tmp8C8=((struct Cyc_Absyn_Short_c_struct*)_tmp8C6)->f2;_LL365: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp8D5=_cycalloc(sizeof(*_tmp8D5));_tmp8D5[
0]=({struct Cyc_Pattern_tok_struct _tmp8D6;_tmp8D6.tag=Cyc_Pattern_tok;_tmp8D6.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*_tmp8D7=_cycalloc(
sizeof(*_tmp8D7));_tmp8D7[0]=({struct Cyc_Absyn_Int_p_struct _tmp8D8;_tmp8D8.tag=7;
_tmp8D8.f1=(void*)_tmp8C7;_tmp8D8.f2=(int)_tmp8C8;_tmp8D8;});_tmp8D7;}),e->loc);
_tmp8D6;});_tmp8D5;});goto _LL361;_LL366: if(*((int*)_tmp8C2)!= 0)goto _LL368;
_tmp8C9=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp8C2)->f1;if(_tmp8C9 <= (void*)
1)goto _LL368;if(*((int*)_tmp8C9)!= 2)goto _LL368;_tmp8CA=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp8C9)->f1;_tmp8CB=((struct Cyc_Absyn_Int_c_struct*)_tmp8C9)->f2;_LL367: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp8D9=_cycalloc(sizeof(*_tmp8D9));_tmp8D9[
0]=({struct Cyc_Pattern_tok_struct _tmp8DA;_tmp8DA.tag=Cyc_Pattern_tok;_tmp8DA.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*_tmp8DB=_cycalloc(
sizeof(*_tmp8DB));_tmp8DB[0]=({struct Cyc_Absyn_Int_p_struct _tmp8DC;_tmp8DC.tag=7;
_tmp8DC.f1=(void*)_tmp8CA;_tmp8DC.f2=_tmp8CB;_tmp8DC;});_tmp8DB;}),e->loc);
_tmp8DA;});_tmp8D9;});goto _LL361;_LL368: if(*((int*)_tmp8C2)!= 0)goto _LL36A;
_tmp8CC=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp8C2)->f1;if(_tmp8CC <= (void*)
1)goto _LL36A;if(*((int*)_tmp8CC)!= 4)goto _LL36A;_tmp8CD=((struct Cyc_Absyn_Float_c_struct*)
_tmp8CC)->f1;_LL369: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8DD=
_cycalloc(sizeof(*_tmp8DD));_tmp8DD[0]=({struct Cyc_Pattern_tok_struct _tmp8DE;
_tmp8DE.tag=Cyc_Pattern_tok;_tmp8DE.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_struct*
_tmp8DF=_cycalloc(sizeof(*_tmp8DF));_tmp8DF[0]=({struct Cyc_Absyn_Float_p_struct
_tmp8E0;_tmp8E0.tag=9;_tmp8E0.f1=_tmp8CD;_tmp8E0;});_tmp8DF;}),e->loc);_tmp8DE;});
_tmp8DD;});goto _LL361;_LL36A: if(*((int*)_tmp8C2)!= 0)goto _LL36C;_tmp8CE=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp8C2)->f1;if((int)_tmp8CE != 0)goto _LL36C;
_LL36B: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8E1=_cycalloc(sizeof(*
_tmp8E1));_tmp8E1[0]=({struct Cyc_Pattern_tok_struct _tmp8E2;_tmp8E2.tag=Cyc_Pattern_tok;
_tmp8E2.f1=Cyc_Absyn_new_pat((void*)1,e->loc);_tmp8E2;});_tmp8E1;});goto _LL361;
_LL36C: if(*((int*)_tmp8C2)!= 0)goto _LL36E;_tmp8CF=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8C2)->f1;if(_tmp8CF <= (void*)1)goto _LL36E;if(*((int*)_tmp8CF)!= 5)goto _LL36E;
_LL36D: Cyc_Parse_err(({const char*_tmp8E3="strings cannot occur within patterns";
_tag_dynforward(_tmp8E3,sizeof(char),_get_zero_arr_size(_tmp8E3,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL361;
_LL36E: if(*((int*)_tmp8C2)!= 0)goto _LL370;_tmp8D0=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8C2)->f1;if(_tmp8D0 <= (void*)1)goto _LL370;if(*((int*)_tmp8D0)!= 3)goto _LL370;
_LL36F: Cyc_Parse_unimp(({const char*_tmp8E4="long long's in patterns";
_tag_dynforward(_tmp8E4,sizeof(char),_get_zero_arr_size(_tmp8E4,24));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL361;
_LL370:;_LL371: Cyc_Parse_err(({const char*_tmp8E5="bad constant in case";
_tag_dynforward(_tmp8E5,sizeof(char),_get_zero_arr_size(_tmp8E5,21));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_LL361:;}break;}
case 359: _LL360: yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp8E6=_cycalloc(
sizeof(*_tmp8E6));_tmp8E6[0]=({struct Cyc_Pattern_tok_struct _tmp8E7;_tmp8E7.tag=
Cyc_Pattern_tok;_tmp8E7.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownId_p_struct*
_tmp8E8=_cycalloc(sizeof(*_tmp8E8));_tmp8E8[0]=({struct Cyc_Absyn_UnknownId_p_struct
_tmp8E9;_tmp8E9.tag=12;_tmp8E9.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8E9;});_tmp8E8;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8E7;});
_tmp8E6;});break;case 360: _LL372: if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp8EA="as";
_tag_dynforward(_tmp8EA,sizeof(char),_get_zero_arr_size(_tmp8EA,3));}))!= 0)Cyc_Parse_err(({
const char*_tmp8EB="expecting `as'";_tag_dynforward(_tmp8EB,sizeof(char),
_get_zero_arr_size(_tmp8EB,15));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(void*)({
struct Cyc_Pattern_tok_struct*_tmp8EC=_cycalloc(sizeof(*_tmp8EC));_tmp8EC[0]=({
struct Cyc_Pattern_tok_struct _tmp8ED;_tmp8ED.tag=Cyc_Pattern_tok;_tmp8ED.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Var_p_struct*_tmp8EE=_cycalloc(sizeof(*_tmp8EE));_tmp8EE[
0]=({struct Cyc_Absyn_Var_p_struct _tmp8EF;_tmp8EF.tag=0;_tmp8EF.f1=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp8F0=_cycalloc(sizeof(*_tmp8F0));_tmp8F0->f1=(void*)0;_tmp8F0->f2=({
struct _dynforward_ptr*_tmp8F1=_cycalloc(sizeof(*_tmp8F1));_tmp8F1[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8F1;});_tmp8F0;}),(
void*)0,0);_tmp8EF.f2=Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp8EF;});_tmp8EE;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp8ED;});
_tmp8EC;});break;case 361: _LL373: {struct Cyc_List_List*_tmp8F3;int _tmp8F4;struct
_tuple17 _tmp8F2=*Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp8F3=_tmp8F2.f1;_tmp8F4=_tmp8F2.f2;yyval=(void*)({struct
Cyc_Pattern_tok_struct*_tmp8F5=_cycalloc(sizeof(*_tmp8F5));_tmp8F5[0]=({struct
Cyc_Pattern_tok_struct _tmp8F6;_tmp8F6.tag=Cyc_Pattern_tok;_tmp8F6.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Tuple_p_struct*_tmp8F7=_cycalloc(sizeof(*_tmp8F7));
_tmp8F7[0]=({struct Cyc_Absyn_Tuple_p_struct _tmp8F8;_tmp8F8.tag=3;_tmp8F8.f1=
_tmp8F3;_tmp8F8.f2=_tmp8F4;_tmp8F8;});_tmp8F7;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8F6;});
_tmp8F5;});break;}case 362: _LL374: {struct Cyc_List_List*_tmp8FA;int _tmp8FB;struct
_tuple17 _tmp8F9=*Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp8FA=_tmp8F9.f1;_tmp8FB=_tmp8F9.f2;yyval=(void*)({struct
Cyc_Pattern_tok_struct*_tmp8FC=_cycalloc(sizeof(*_tmp8FC));_tmp8FC[0]=({struct
Cyc_Pattern_tok_struct _tmp8FD;_tmp8FD.tag=Cyc_Pattern_tok;_tmp8FD.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_UnknownCall_p_struct*_tmp8FE=_cycalloc(sizeof(*_tmp8FE));
_tmp8FE[0]=({struct Cyc_Absyn_UnknownCall_p_struct _tmp8FF;_tmp8FF.tag=13;_tmp8FF.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp8FF.f2=_tmp8FA;_tmp8FF.f3=_tmp8FB;_tmp8FF;});_tmp8FE;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8FD;});
_tmp8FC;});break;}case 363: _LL375: {struct Cyc_List_List*_tmp901;int _tmp902;struct
_tuple17 _tmp900=*Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp901=_tmp900.f1;_tmp902=_tmp900.f2;{struct Cyc_List_List*
_tmp903=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),Cyc_yyget_TypeList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=(void*)({struct
Cyc_Pattern_tok_struct*_tmp904=_cycalloc(sizeof(*_tmp904));_tmp904[0]=({struct
Cyc_Pattern_tok_struct _tmp905;_tmp905.tag=Cyc_Pattern_tok;_tmp905.f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp906=_cycalloc(sizeof(*_tmp906));
_tmp906[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp907;_tmp907.tag=5;_tmp907.f1=({
struct Cyc_Absyn_AggrInfo _tmp908;_tmp908.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp909=_cycalloc(sizeof(*_tmp909));_tmp909[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp90A;_tmp90A.tag=0;_tmp90A.f1=(void*)((void*)0);_tmp90A.f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp90A;});_tmp909;}));
_tmp908.targs=0;_tmp908;});_tmp907.f2=_tmp903;_tmp907.f3=_tmp901;_tmp907.f4=
_tmp902;_tmp907;});_tmp906;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp905;});
_tmp904;});break;}}case 364: _LL376: yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp90B=_cycalloc(sizeof(*_tmp90B));_tmp90B[0]=({struct Cyc_Pattern_tok_struct
_tmp90C;_tmp90C.tag=Cyc_Pattern_tok;_tmp90C.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Pointer_p_struct*_tmp90D=_cycalloc(sizeof(*_tmp90D));_tmp90D[0]=({
struct Cyc_Absyn_Pointer_p_struct _tmp90E;_tmp90E.tag=4;_tmp90E.f1=Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp90E;});_tmp90D;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp90C;});_tmp90B;});break;case 365: _LL377: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp90F=_cycalloc(sizeof(*_tmp90F));_tmp90F[
0]=({struct Cyc_Pattern_tok_struct _tmp910;_tmp910.tag=Cyc_Pattern_tok;_tmp910.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp911=_cycalloc(
sizeof(*_tmp911));_tmp911[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp912;_tmp912.tag=
4;_tmp912.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp913=
_cycalloc(sizeof(*_tmp913));_tmp913[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp914;
_tmp914.tag=4;_tmp914.f1=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp914;});_tmp913;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp912;});
_tmp911;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp910;});_tmp90F;});break;case 366: _LL378: yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp915=_cycalloc(sizeof(*_tmp915));_tmp915[
0]=({struct Cyc_Pattern_tok_struct _tmp916;_tmp916.tag=Cyc_Pattern_tok;_tmp916.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmp917=_cycalloc(
sizeof(*_tmp917));_tmp917[0]=({struct Cyc_Absyn_Reference_p_struct _tmp918;_tmp918.tag=
1;_tmp918.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp919=_cycalloc(sizeof(*
_tmp919));_tmp919->f1=(void*)0;_tmp919->f2=({struct _dynforward_ptr*_tmp91A=
_cycalloc(sizeof(*_tmp91A));_tmp91A[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp91A;});_tmp919;}),(void*)
0,0);_tmp918.f2=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp918;});
_tmp917;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp916;});_tmp915;});break;case 367: _LL379: if(Cyc_strcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),({const char*_tmp91B="as";_tag_dynforward(_tmp91B,
sizeof(char),_get_zero_arr_size(_tmp91B,3));}))!= 0)Cyc_Parse_err(({const char*
_tmp91C="expecting `as'";_tag_dynforward(_tmp91C,sizeof(char),_get_zero_arr_size(
_tmp91C,15));}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));yyval=(void*)({struct Cyc_Pattern_tok_struct*
_tmp91D=_cycalloc(sizeof(*_tmp91D));_tmp91D[0]=({struct Cyc_Pattern_tok_struct
_tmp91E;_tmp91E.tag=Cyc_Pattern_tok;_tmp91E.f1=Cyc_Absyn_new_pat((void*)({struct
Cyc_Absyn_Reference_p_struct*_tmp91F=_cycalloc(sizeof(*_tmp91F));_tmp91F[0]=({
struct Cyc_Absyn_Reference_p_struct _tmp920;_tmp920.tag=1;_tmp920.f1=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp921=_cycalloc(sizeof(*_tmp921));_tmp921->f1=(void*)0;_tmp921->f2=({
struct _dynforward_ptr*_tmp922=_cycalloc(sizeof(*_tmp922));_tmp922[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp922;});_tmp921;}),(
void*)0,0);_tmp920.f2=Cyc_yyget_Pattern_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp920;});_tmp91F;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp91E;});
_tmp91D;});break;case 368: _LL37A: {void*_tmp923=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp92C=_cycalloc(sizeof(*_tmp92C));_tmp92C[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp92D;_tmp92D.tag=0;_tmp92D.f1=(void*)((void*)5);_tmp92D;});_tmp92C;}));yyval=(
void*)({struct Cyc_Pattern_tok_struct*_tmp924=_cycalloc(sizeof(*_tmp924));_tmp924[
0]=({struct Cyc_Pattern_tok_struct _tmp925;_tmp925.tag=Cyc_Pattern_tok;_tmp925.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*_tmp926=_cycalloc(
sizeof(*_tmp926));_tmp926[0]=({struct Cyc_Absyn_TagInt_p_struct _tmp927;_tmp927.tag=
2;_tmp927.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp923);
_tmp927.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp928=_cycalloc(sizeof(*
_tmp928));_tmp928->f1=(void*)0;_tmp928->f2=({struct _dynforward_ptr*_tmp929=
_cycalloc(sizeof(*_tmp929));_tmp929[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp929;});_tmp928;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp92A=_cycalloc(sizeof(*_tmp92A));
_tmp92A[0]=({struct Cyc_Absyn_TagType_struct _tmp92B;_tmp92B.tag=17;_tmp92B.f1=(
void*)_tmp923;_tmp92B;});_tmp92A;}),0);_tmp927;});_tmp926;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp925;});
_tmp924;});break;}case 369: _LL37B: {struct Cyc_Absyn_Tvar*_tmp92E=Cyc_Tcutil_new_tvar((
void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp939=_cycalloc(sizeof(*_tmp939));_tmp939[
0]=({struct Cyc_Absyn_Eq_kb_struct _tmp93A;_tmp93A.tag=0;_tmp93A.f1=(void*)((void*)
5);_tmp93A;});_tmp939;}));yyval=(void*)({struct Cyc_Pattern_tok_struct*_tmp92F=
_cycalloc(sizeof(*_tmp92F));_tmp92F[0]=({struct Cyc_Pattern_tok_struct _tmp930;
_tmp930.tag=Cyc_Pattern_tok;_tmp930.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp931=_cycalloc(sizeof(*_tmp931));_tmp931[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp932;_tmp932.tag=2;_tmp932.f1=_tmp92E;_tmp932.f2=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp933=_cycalloc(sizeof(*_tmp933));_tmp933->f1=(void*)0;_tmp933->f2=({
struct _dynforward_ptr*_tmp934=_cycalloc(sizeof(*_tmp934));_tmp934[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp934;});_tmp933;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp935=_cycalloc(sizeof(*_tmp935));
_tmp935[0]=({struct Cyc_Absyn_TagType_struct _tmp936;_tmp936.tag=17;_tmp936.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp937=_cycalloc(sizeof(*_tmp937));
_tmp937[0]=({struct Cyc_Absyn_VarType_struct _tmp938;_tmp938.tag=1;_tmp938.f1=
_tmp92E;_tmp938;});_tmp937;}));_tmp936;});_tmp935;}),0);_tmp932;});_tmp931;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp930;});_tmp92F;});break;}case 370: _LL37C: yyval=(
void*)({struct Cyc_YY3_struct*_tmp93B=_cycalloc(sizeof(*_tmp93B));_tmp93B[0]=({
struct Cyc_YY3_struct _tmp93C;_tmp93C.tag=Cyc_YY3;_tmp93C.f1=({struct _tuple17*
_tmp93D=_cycalloc(sizeof(*_tmp93D));_tmp93D->f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_PatternList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp93D->f2=0;_tmp93D;});
_tmp93C;});_tmp93B;});break;case 371: _LL37D: yyval=(void*)({struct Cyc_YY3_struct*
_tmp93E=_cycalloc(sizeof(*_tmp93E));_tmp93E[0]=({struct Cyc_YY3_struct _tmp93F;
_tmp93F.tag=Cyc_YY3;_tmp93F.f1=({struct _tuple17*_tmp940=_cycalloc(sizeof(*
_tmp940));_tmp940->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_PatternList_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]));_tmp940->f2=1;_tmp940;});_tmp93F;});_tmp93E;});break;case 372: _LL37E:
yyval=(void*)({struct Cyc_YY3_struct*_tmp941=_cycalloc(sizeof(*_tmp941));_tmp941[
0]=({struct Cyc_YY3_struct _tmp942;_tmp942.tag=Cyc_YY3;_tmp942.f1=({struct _tuple17*
_tmp943=_cycalloc(sizeof(*_tmp943));_tmp943->f1=0;_tmp943->f2=1;_tmp943;});
_tmp942;});_tmp941;});break;case 373: _LL37F: yyval=(void*)({struct Cyc_PatternList_tok_struct*
_tmp944=_cycalloc(sizeof(*_tmp944));_tmp944[0]=({struct Cyc_PatternList_tok_struct
_tmp945;_tmp945.tag=Cyc_PatternList_tok;_tmp945.f1=({struct Cyc_List_List*_tmp946=
_cycalloc(sizeof(*_tmp946));_tmp946->hd=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp946->tl=0;_tmp946;});
_tmp945;});_tmp944;});break;case 374: _LL380: yyval=(void*)({struct Cyc_PatternList_tok_struct*
_tmp947=_cycalloc(sizeof(*_tmp947));_tmp947[0]=({struct Cyc_PatternList_tok_struct
_tmp948;_tmp948.tag=Cyc_PatternList_tok;_tmp948.f1=({struct Cyc_List_List*_tmp949=
_cycalloc(sizeof(*_tmp949));_tmp949->hd=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp949->tl=Cyc_yyget_PatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp949;});_tmp948;});
_tmp947;});break;case 375: _LL381: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp94A=_cycalloc(sizeof(*_tmp94A));_tmp94A[0]=({struct Cyc_FieldPattern_tok_struct
_tmp94B;_tmp94B.tag=Cyc_FieldPattern_tok;_tmp94B.f1=({struct _tuple11*_tmp94C=
_cycalloc(sizeof(*_tmp94C));_tmp94C->f1=0;_tmp94C->f2=Cyc_yyget_Pattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp94C;});_tmp94B;});
_tmp94A;});break;case 376: _LL382: yyval=(void*)({struct Cyc_FieldPattern_tok_struct*
_tmp94D=_cycalloc(sizeof(*_tmp94D));_tmp94D[0]=({struct Cyc_FieldPattern_tok_struct
_tmp94E;_tmp94E.tag=Cyc_FieldPattern_tok;_tmp94E.f1=({struct _tuple11*_tmp94F=
_cycalloc(sizeof(*_tmp94F));_tmp94F->f1=Cyc_yyget_DesignatorList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp94F->f2=Cyc_yyget_Pattern_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp94F;});_tmp94E;});
_tmp94D;});break;case 377: _LL383: yyval=(void*)({struct Cyc_YY4_struct*_tmp950=
_cycalloc(sizeof(*_tmp950));_tmp950[0]=({struct Cyc_YY4_struct _tmp951;_tmp951.tag=
Cyc_YY4;_tmp951.f1=({struct _tuple17*_tmp952=_cycalloc(sizeof(*_tmp952));_tmp952->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_FieldPatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp952->f2=0;_tmp952;});
_tmp951;});_tmp950;});break;case 378: _LL384: yyval=(void*)({struct Cyc_YY4_struct*
_tmp953=_cycalloc(sizeof(*_tmp953));_tmp953[0]=({struct Cyc_YY4_struct _tmp954;
_tmp954.tag=Cyc_YY4;_tmp954.f1=({struct _tuple17*_tmp955=_cycalloc(sizeof(*
_tmp955));_tmp955->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_FieldPatternList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp955->f2=1;_tmp955;});_tmp954;});_tmp953;});break;case 379:
_LL385: yyval=(void*)({struct Cyc_YY4_struct*_tmp956=_cycalloc(sizeof(*_tmp956));
_tmp956[0]=({struct Cyc_YY4_struct _tmp957;_tmp957.tag=Cyc_YY4;_tmp957.f1=({struct
_tuple17*_tmp958=_cycalloc(sizeof(*_tmp958));_tmp958->f1=0;_tmp958->f2=1;_tmp958;});
_tmp957;});_tmp956;});break;case 380: _LL386: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp959=_cycalloc(sizeof(*_tmp959));_tmp959[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp95A;_tmp95A.tag=Cyc_FieldPatternList_tok;_tmp95A.f1=({struct Cyc_List_List*
_tmp95B=_cycalloc(sizeof(*_tmp95B));_tmp95B->hd=Cyc_yyget_FieldPattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp95B->tl=0;_tmp95B;});
_tmp95A;});_tmp959;});break;case 381: _LL387: yyval=(void*)({struct Cyc_FieldPatternList_tok_struct*
_tmp95C=_cycalloc(sizeof(*_tmp95C));_tmp95C[0]=({struct Cyc_FieldPatternList_tok_struct
_tmp95D;_tmp95D.tag=Cyc_FieldPatternList_tok;_tmp95D.f1=({struct Cyc_List_List*
_tmp95E=_cycalloc(sizeof(*_tmp95E));_tmp95E->hd=Cyc_yyget_FieldPattern_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp95E->tl=Cyc_yyget_FieldPatternList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp95E;});_tmp95D;});
_tmp95C;});break;case 382: _LL388: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 383: _LL389: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp95F=_cycalloc(sizeof(*_tmp95F));_tmp95F[0]=({struct Cyc_Exp_tok_struct _tmp960;
_tmp960.tag=Cyc_Exp_tok;_tmp960.f1=Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp960;});
_tmp95F;});break;case 384: _LL38A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 385: _LL38B: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp961=_cycalloc(sizeof(*_tmp961));_tmp961[0]=({struct Cyc_Exp_tok_struct _tmp962;
_tmp962.tag=Cyc_Exp_tok;_tmp962.f1=Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Primopopt_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp962;});
_tmp961;});break;case 386: _LL38C: yyval=(void*)({struct Cyc_Primopopt_tok_struct*
_tmp963=_cycalloc(sizeof(*_tmp963));_tmp963[0]=({struct Cyc_Primopopt_tok_struct
_tmp964;_tmp964.tag=Cyc_Primopopt_tok;_tmp964.f1=0;_tmp964;});_tmp963;});break;
case 387: _LL38D: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp965=_cycalloc(
sizeof(*_tmp965));_tmp965[0]=({struct Cyc_Primopopt_tok_struct _tmp966;_tmp966.tag=
Cyc_Primopopt_tok;_tmp966.f1=({struct Cyc_Core_Opt*_tmp967=_cycalloc(sizeof(*
_tmp967));_tmp967->v=(void*)((void*)1);_tmp967;});_tmp966;});_tmp965;});break;
case 388: _LL38E: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp968=_cycalloc(
sizeof(*_tmp968));_tmp968[0]=({struct Cyc_Primopopt_tok_struct _tmp969;_tmp969.tag=
Cyc_Primopopt_tok;_tmp969.f1=({struct Cyc_Core_Opt*_tmp96A=_cycalloc(sizeof(*
_tmp96A));_tmp96A->v=(void*)((void*)3);_tmp96A;});_tmp969;});_tmp968;});break;
case 389: _LL38F: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp96B=_cycalloc(
sizeof(*_tmp96B));_tmp96B[0]=({struct Cyc_Primopopt_tok_struct _tmp96C;_tmp96C.tag=
Cyc_Primopopt_tok;_tmp96C.f1=({struct Cyc_Core_Opt*_tmp96D=_cycalloc(sizeof(*
_tmp96D));_tmp96D->v=(void*)((void*)4);_tmp96D;});_tmp96C;});_tmp96B;});break;
case 390: _LL390: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp96E=_cycalloc(
sizeof(*_tmp96E));_tmp96E[0]=({struct Cyc_Primopopt_tok_struct _tmp96F;_tmp96F.tag=
Cyc_Primopopt_tok;_tmp96F.f1=({struct Cyc_Core_Opt*_tmp970=_cycalloc(sizeof(*
_tmp970));_tmp970->v=(void*)((void*)0);_tmp970;});_tmp96F;});_tmp96E;});break;
case 391: _LL391: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp971=_cycalloc(
sizeof(*_tmp971));_tmp971[0]=({struct Cyc_Primopopt_tok_struct _tmp972;_tmp972.tag=
Cyc_Primopopt_tok;_tmp972.f1=({struct Cyc_Core_Opt*_tmp973=_cycalloc(sizeof(*
_tmp973));_tmp973->v=(void*)((void*)2);_tmp973;});_tmp972;});_tmp971;});break;
case 392: _LL392: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp974=_cycalloc(
sizeof(*_tmp974));_tmp974[0]=({struct Cyc_Primopopt_tok_struct _tmp975;_tmp975.tag=
Cyc_Primopopt_tok;_tmp975.f1=({struct Cyc_Core_Opt*_tmp976=_cycalloc(sizeof(*
_tmp976));_tmp976->v=(void*)((void*)16);_tmp976;});_tmp975;});_tmp974;});break;
case 393: _LL393: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp977=_cycalloc(
sizeof(*_tmp977));_tmp977[0]=({struct Cyc_Primopopt_tok_struct _tmp978;_tmp978.tag=
Cyc_Primopopt_tok;_tmp978.f1=({struct Cyc_Core_Opt*_tmp979=_cycalloc(sizeof(*
_tmp979));_tmp979->v=(void*)((void*)17);_tmp979;});_tmp978;});_tmp977;});break;
case 394: _LL394: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp97A=_cycalloc(
sizeof(*_tmp97A));_tmp97A[0]=({struct Cyc_Primopopt_tok_struct _tmp97B;_tmp97B.tag=
Cyc_Primopopt_tok;_tmp97B.f1=({struct Cyc_Core_Opt*_tmp97C=_cycalloc(sizeof(*
_tmp97C));_tmp97C->v=(void*)((void*)13);_tmp97C;});_tmp97B;});_tmp97A;});break;
case 395: _LL395: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp97D=_cycalloc(
sizeof(*_tmp97D));_tmp97D[0]=({struct Cyc_Primopopt_tok_struct _tmp97E;_tmp97E.tag=
Cyc_Primopopt_tok;_tmp97E.f1=({struct Cyc_Core_Opt*_tmp97F=_cycalloc(sizeof(*
_tmp97F));_tmp97F->v=(void*)((void*)15);_tmp97F;});_tmp97E;});_tmp97D;});break;
case 396: _LL396: yyval=(void*)({struct Cyc_Primopopt_tok_struct*_tmp980=_cycalloc(
sizeof(*_tmp980));_tmp980[0]=({struct Cyc_Primopopt_tok_struct _tmp981;_tmp981.tag=
Cyc_Primopopt_tok;_tmp981.f1=({struct Cyc_Core_Opt*_tmp982=_cycalloc(sizeof(*
_tmp982));_tmp982->v=(void*)((void*)14);_tmp982;});_tmp981;});_tmp980;});break;
case 397: _LL397: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 398: _LL398: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp983=_cycalloc(
sizeof(*_tmp983));_tmp983[0]=({struct Cyc_Exp_tok_struct _tmp984;_tmp984.tag=Cyc_Exp_tok;
_tmp984.f1=Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp984;});
_tmp983;});break;case 399: _LL399: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp985=
_cycalloc(sizeof(*_tmp985));_tmp985[0]=({struct Cyc_Exp_tok_struct _tmp986;_tmp986.tag=
Cyc_Exp_tok;_tmp986.f1=Cyc_Absyn_throw_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp986;});
_tmp985;});break;case 400: _LL39A: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp987=
_cycalloc(sizeof(*_tmp987));_tmp987[0]=({struct Cyc_Exp_tok_struct _tmp988;_tmp988.tag=
Cyc_Exp_tok;_tmp988.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp988;});
_tmp987;});break;case 401: _LL39B: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp989=
_cycalloc(sizeof(*_tmp989));_tmp989[0]=({struct Cyc_Exp_tok_struct _tmp98A;_tmp98A.tag=
Cyc_Exp_tok;_tmp98A.f1=Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp98A;});
_tmp989;});break;case 402: _LL39C: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp98B=
_cycalloc(sizeof(*_tmp98B));_tmp98B[0]=({struct Cyc_Exp_tok_struct _tmp98C;_tmp98C.tag=
Cyc_Exp_tok;_tmp98C.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp98C;});
_tmp98B;});break;case 403: _LL39D: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp98D=
_cycalloc(sizeof(*_tmp98D));_tmp98D[0]=({struct Cyc_Exp_tok_struct _tmp98E;_tmp98E.tag=
Cyc_Exp_tok;_tmp98E.f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp98E;});
_tmp98D;});break;case 404: _LL39E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 405: _LL39F: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 406: _LL3A0: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp98F=_cycalloc(sizeof(*_tmp98F));_tmp98F[0]=({struct Cyc_Exp_tok_struct _tmp990;
_tmp990.tag=Cyc_Exp_tok;_tmp990.f1=Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp990;});
_tmp98F;});break;case 407: _LL3A1: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 408: _LL3A2: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp991=_cycalloc(sizeof(*_tmp991));_tmp991[0]=({struct Cyc_Exp_tok_struct _tmp992;
_tmp992.tag=Cyc_Exp_tok;_tmp992.f1=Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp992;});
_tmp991;});break;case 409: _LL3A3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 410: _LL3A4: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp993=_cycalloc(sizeof(*_tmp993));_tmp993[0]=({struct Cyc_Exp_tok_struct _tmp994;
_tmp994.tag=Cyc_Exp_tok;_tmp994.f1=Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp994;});
_tmp993;});break;case 411: _LL3A5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 412: _LL3A6: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp995=_cycalloc(sizeof(*_tmp995));_tmp995[0]=({struct Cyc_Exp_tok_struct _tmp996;
_tmp996.tag=Cyc_Exp_tok;_tmp996.f1=Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp996;});
_tmp995;});break;case 413: _LL3A7: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 414: _LL3A8: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp997=_cycalloc(sizeof(*_tmp997));_tmp997[0]=({struct Cyc_Exp_tok_struct _tmp998;
_tmp998.tag=Cyc_Exp_tok;_tmp998.f1=Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp998;});
_tmp997;});break;case 415: _LL3A9: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 416: _LL3AA: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp999=_cycalloc(sizeof(*_tmp999));_tmp999[0]=({struct Cyc_Exp_tok_struct _tmp99A;
_tmp99A.tag=Cyc_Exp_tok;_tmp99A.f1=Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp99A;});
_tmp999;});break;case 417: _LL3AB: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp99B=
_cycalloc(sizeof(*_tmp99B));_tmp99B[0]=({struct Cyc_Exp_tok_struct _tmp99C;_tmp99C.tag=
Cyc_Exp_tok;_tmp99C.f1=Cyc_Absyn_neq_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp99C;});
_tmp99B;});break;case 418: _LL3AC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 419: _LL3AD: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp99D=_cycalloc(sizeof(*_tmp99D));_tmp99D[0]=({struct Cyc_Exp_tok_struct _tmp99E;
_tmp99E.tag=Cyc_Exp_tok;_tmp99E.f1=Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp99E;});
_tmp99D;});break;case 420: _LL3AE: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp99F=
_cycalloc(sizeof(*_tmp99F));_tmp99F[0]=({struct Cyc_Exp_tok_struct _tmp9A0;_tmp9A0.tag=
Cyc_Exp_tok;_tmp9A0.f1=Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9A0;});
_tmp99F;});break;case 421: _LL3AF: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9A1=
_cycalloc(sizeof(*_tmp9A1));_tmp9A1[0]=({struct Cyc_Exp_tok_struct _tmp9A2;_tmp9A2.tag=
Cyc_Exp_tok;_tmp9A2.f1=Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9A2;});
_tmp9A1;});break;case 422: _LL3B0: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9A3=
_cycalloc(sizeof(*_tmp9A3));_tmp9A3[0]=({struct Cyc_Exp_tok_struct _tmp9A4;_tmp9A4.tag=
Cyc_Exp_tok;_tmp9A4.f1=Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9A4;});
_tmp9A3;});break;case 423: _LL3B1: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 424: _LL3B2: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp9A5=_cycalloc(sizeof(*_tmp9A5));_tmp9A5[0]=({struct Cyc_Exp_tok_struct _tmp9A6;
_tmp9A6.tag=Cyc_Exp_tok;_tmp9A6.f1=Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9A6;});
_tmp9A5;});break;case 425: _LL3B3: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9A7=
_cycalloc(sizeof(*_tmp9A7));_tmp9A7[0]=({struct Cyc_Exp_tok_struct _tmp9A8;_tmp9A8.tag=
Cyc_Exp_tok;_tmp9A8.f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9A8;});
_tmp9A7;});break;case 426: _LL3B4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 427: _LL3B5: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp9A9=_cycalloc(sizeof(*_tmp9A9));_tmp9A9[0]=({struct Cyc_Exp_tok_struct _tmp9AA;
_tmp9AA.tag=Cyc_Exp_tok;_tmp9AA.f1=Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9AA;});
_tmp9A9;});break;case 428: _LL3B6: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9AB=
_cycalloc(sizeof(*_tmp9AB));_tmp9AB[0]=({struct Cyc_Exp_tok_struct _tmp9AC;_tmp9AC.tag=
Cyc_Exp_tok;_tmp9AC.f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9AC;});
_tmp9AB;});break;case 429: _LL3B7: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 430: _LL3B8: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp9AD=_cycalloc(sizeof(*_tmp9AD));_tmp9AD[0]=({struct Cyc_Exp_tok_struct _tmp9AE;
_tmp9AE.tag=Cyc_Exp_tok;_tmp9AE.f1=Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9AE;});
_tmp9AD;});break;case 431: _LL3B9: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9AF=
_cycalloc(sizeof(*_tmp9AF));_tmp9AF[0]=({struct Cyc_Exp_tok_struct _tmp9B0;_tmp9B0.tag=
Cyc_Exp_tok;_tmp9B0.f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9B0;});
_tmp9AF;});break;case 432: _LL3BA: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9B1=
_cycalloc(sizeof(*_tmp9B1));_tmp9B1[0]=({struct Cyc_Exp_tok_struct _tmp9B2;_tmp9B2.tag=
Cyc_Exp_tok;_tmp9B2.f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9B2;});
_tmp9B1;});break;case 433: _LL3BB: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 434: _LL3BC: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp9B3=_cycalloc(sizeof(*_tmp9B3));_tmp9B3[0]=({struct Cyc_Exp_tok_struct _tmp9B4;
_tmp9B4.tag=Cyc_Exp_tok;_tmp9B4.f1=Cyc_Absyn_cast_exp((*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9B4;});
_tmp9B3;});break;case 435: _LL3BD: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 436: _LL3BE: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp9B5=_cycalloc(sizeof(*_tmp9B5));_tmp9B5[0]=({struct Cyc_Exp_tok_struct _tmp9B6;
_tmp9B6.tag=Cyc_Exp_tok;_tmp9B6.f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9B6;});
_tmp9B5;});break;case 437: _LL3BF: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9B7=
_cycalloc(sizeof(*_tmp9B7));_tmp9B7[0]=({struct Cyc_Exp_tok_struct _tmp9B8;_tmp9B8.tag=
Cyc_Exp_tok;_tmp9B8.f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9B8;});
_tmp9B7;});break;case 438: _LL3C0: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9B9=
_cycalloc(sizeof(*_tmp9B9));_tmp9B9[0]=({struct Cyc_Exp_tok_struct _tmp9BA;_tmp9BA.tag=
Cyc_Exp_tok;_tmp9BA.f1=Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9BA;});
_tmp9B9;});break;case 439: _LL3C1: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9BB=
_cycalloc(sizeof(*_tmp9BB));_tmp9BB[0]=({struct Cyc_Exp_tok_struct _tmp9BC;_tmp9BC.tag=
Cyc_Exp_tok;_tmp9BC.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9BC;});
_tmp9BB;});break;case 440: _LL3C2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 441: _LL3C3: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp9BD=_cycalloc(sizeof(*_tmp9BD));_tmp9BD[0]=({struct Cyc_Exp_tok_struct _tmp9BE;
_tmp9BE.tag=Cyc_Exp_tok;_tmp9BE.f1=Cyc_Absyn_prim1_exp(Cyc_yyget_Primop_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9BE;});
_tmp9BD;});break;case 442: _LL3C4: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9BF=
_cycalloc(sizeof(*_tmp9BF));_tmp9BF[0]=({struct Cyc_Exp_tok_struct _tmp9C0;_tmp9C0.tag=
Cyc_Exp_tok;_tmp9C0.f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9C0;});
_tmp9BF;});break;case 443: _LL3C5: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9C1=
_cycalloc(sizeof(*_tmp9C1));_tmp9C1[0]=({struct Cyc_Exp_tok_struct _tmp9C2;_tmp9C2.tag=
Cyc_Exp_tok;_tmp9C2.f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9C2;});
_tmp9C1;});break;case 444: _LL3C6: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9C3=
_cycalloc(sizeof(*_tmp9C3));_tmp9C3[0]=({struct Cyc_Exp_tok_struct _tmp9C4;_tmp9C4.tag=
Cyc_Exp_tok;_tmp9C4.f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmp9C5=_cycalloc(sizeof(*_tmp9C5));_tmp9C5[0]=({struct Cyc_Absyn_StructField_struct
_tmp9C6;_tmp9C6.tag=0;_tmp9C6.f1=({struct _dynforward_ptr*_tmp9C7=_cycalloc(
sizeof(*_tmp9C7));_tmp9C7[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp9C7;});_tmp9C6;});
_tmp9C5;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp9C4;});_tmp9C3;});break;case 445: _LL3C7: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmp9C8=_cycalloc(sizeof(*_tmp9C8));_tmp9C8[0]=({
struct Cyc_Exp_tok_struct _tmp9C9;_tmp9C9.tag=Cyc_Exp_tok;_tmp9C9.f1=Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp9CA=_cycalloc_atomic(
sizeof(*_tmp9CA));_tmp9CA[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp9CB;_tmp9CB.tag=
1;_tmp9CB.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp9CB;});_tmp9CA;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp9C9;});_tmp9C8;});break;case 446: _LL3C8: {struct
Cyc_Position_Segment*_tmp9CC=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp9CD=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp9CC,Cyc_yyget_TypeList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]));yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9CE=_cycalloc(
sizeof(*_tmp9CE));_tmp9CE[0]=({struct Cyc_Exp_tok_struct _tmp9CF;_tmp9CF.tag=Cyc_Exp_tok;
_tmp9CF.f1=Cyc_Absyn_gentyp_exp(_tmp9CD,(*Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9CF;});
_tmp9CE;});break;}case 447: _LL3C9: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9D0=
_cycalloc(sizeof(*_tmp9D0));_tmp9D0[0]=({struct Cyc_Exp_tok_struct _tmp9D1;_tmp9D1.tag=
Cyc_Exp_tok;_tmp9D1.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp9D2=_cycalloc(sizeof(*_tmp9D2));_tmp9D2[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp9D3;_tmp9D3.tag=35;_tmp9D3.f1=({struct Cyc_Absyn_MallocInfo _tmp9D4;_tmp9D4.is_calloc=
0;_tmp9D4.rgn=0;_tmp9D4.elt_type=0;_tmp9D4.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp9D4.fat_result=0;
_tmp9D4;});_tmp9D3;});_tmp9D2;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9D1;});
_tmp9D0;});break;case 448: _LL3CA: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9D5=
_cycalloc(sizeof(*_tmp9D5));_tmp9D5[0]=({struct Cyc_Exp_tok_struct _tmp9D6;_tmp9D6.tag=
Cyc_Exp_tok;_tmp9D6.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp9D7=_cycalloc(sizeof(*_tmp9D7));_tmp9D7[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp9D8;_tmp9D8.tag=35;_tmp9D8.f1=({struct Cyc_Absyn_MallocInfo _tmp9D9;_tmp9D9.is_calloc=
0;_tmp9D9.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp9D9.elt_type=0;
_tmp9D9.num_elts=Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp9D9.fat_result=0;_tmp9D9;});_tmp9D8;});_tmp9D7;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9D6;});
_tmp9D5;});break;case 449: _LL3CB: {void*_tmp9DB;struct _tuple2 _tmp9DA=*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp9DB=_tmp9DA.f3;
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9DC=_cycalloc(sizeof(*_tmp9DC));
_tmp9DC[0]=({struct Cyc_Exp_tok_struct _tmp9DD;_tmp9DD.tag=Cyc_Exp_tok;_tmp9DD.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp9DE=_cycalloc(
sizeof(*_tmp9DE));_tmp9DE[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp9DF;_tmp9DF.tag=
35;_tmp9DF.f1=({struct Cyc_Absyn_MallocInfo _tmp9E0;_tmp9E0.is_calloc=1;_tmp9E0.rgn=
0;_tmp9E0.elt_type=({void**_tmp9E1=_cycalloc(sizeof(*_tmp9E1));_tmp9E1[0]=
_tmp9DB;_tmp9E1;});_tmp9E0.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp9E0.fat_result=0;
_tmp9E0;});_tmp9DF;});_tmp9DE;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9DD;});
_tmp9DC;});break;}case 450: _LL3CC: {void*_tmp9E3;struct _tuple2 _tmp9E2=*Cyc_yyget_ParamDecl_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp9E3=_tmp9E2.f3;
yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9E4=_cycalloc(sizeof(*_tmp9E4));
_tmp9E4[0]=({struct Cyc_Exp_tok_struct _tmp9E5;_tmp9E5.tag=Cyc_Exp_tok;_tmp9E5.f1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp9E6=_cycalloc(
sizeof(*_tmp9E6));_tmp9E6[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp9E7;_tmp9E7.tag=
35;_tmp9E7.f1=({struct Cyc_Absyn_MallocInfo _tmp9E8;_tmp9E8.is_calloc=1;_tmp9E8.rgn=(
struct Cyc_Absyn_Exp*)Cyc_yyget_Exp_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 8)]);_tmp9E8.elt_type=({void**_tmp9E9=_cycalloc(sizeof(*_tmp9E9));
_tmp9E9[0]=_tmp9E3;_tmp9E9;});_tmp9E8.num_elts=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp9E8.fat_result=0;
_tmp9E8;});_tmp9E7;});_tmp9E6;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9E5;});
_tmp9E4;});break;}case 451: _LL3CD: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp9EA=_cycalloc(sizeof(*_tmp9EA));_tmp9EA[0]=({struct Cyc_Primop_tok_struct
_tmp9EB;_tmp9EB.tag=Cyc_Primop_tok;_tmp9EB.f1=(void*)((void*)12);_tmp9EB;});
_tmp9EA;});break;case 452: _LL3CE: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp9EC=_cycalloc(sizeof(*_tmp9EC));_tmp9EC[0]=({struct Cyc_Primop_tok_struct
_tmp9ED;_tmp9ED.tag=Cyc_Primop_tok;_tmp9ED.f1=(void*)((void*)11);_tmp9ED;});
_tmp9EC;});break;case 453: _LL3CF: yyval=(void*)({struct Cyc_Primop_tok_struct*
_tmp9EE=_cycalloc(sizeof(*_tmp9EE));_tmp9EE[0]=({struct Cyc_Primop_tok_struct
_tmp9EF;_tmp9EF.tag=Cyc_Primop_tok;_tmp9EF.f1=(void*)((void*)2);_tmp9EF;});
_tmp9EE;});break;case 454: _LL3D0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 455: _LL3D1: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmp9F0=_cycalloc(sizeof(*_tmp9F0));_tmp9F0[0]=({struct Cyc_Exp_tok_struct _tmp9F1;
_tmp9F1.tag=Cyc_Exp_tok;_tmp9F1.f1=Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9F1;});
_tmp9F0;});break;case 456: _LL3D2: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9F2=
_cycalloc(sizeof(*_tmp9F2));_tmp9F2[0]=({struct Cyc_Exp_tok_struct _tmp9F3;_tmp9F3.tag=
Cyc_Exp_tok;_tmp9F3.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9F3;});
_tmp9F2;});break;case 457: _LL3D3: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9F4=
_cycalloc(sizeof(*_tmp9F4));_tmp9F4[0]=({struct Cyc_Exp_tok_struct _tmp9F5;_tmp9F5.tag=
Cyc_Exp_tok;_tmp9F5.f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9F5;});
_tmp9F4;});break;case 458: _LL3D4: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9F6=
_cycalloc(sizeof(*_tmp9F6));_tmp9F6[0]=({struct Cyc_Exp_tok_struct _tmp9F7;_tmp9F7.tag=
Cyc_Exp_tok;_tmp9F7.f1=Cyc_Absyn_aggrmember_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _dynforward_ptr*
_tmp9F8=_cycalloc(sizeof(*_tmp9F8));_tmp9F8[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp9F8;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9F7;});
_tmp9F6;});break;case 459: _LL3D5: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9F9=
_cycalloc(sizeof(*_tmp9F9));_tmp9F9[0]=({struct Cyc_Exp_tok_struct _tmp9FA;_tmp9FA.tag=
Cyc_Exp_tok;_tmp9FA.f1=Cyc_Absyn_aggrarrow_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _dynforward_ptr*
_tmp9FB=_cycalloc(sizeof(*_tmp9FB));_tmp9FB[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp9FB;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9FA;});
_tmp9F9;});break;case 460: _LL3D6: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9FC=
_cycalloc(sizeof(*_tmp9FC));_tmp9FC[0]=({struct Cyc_Exp_tok_struct _tmp9FD;_tmp9FD.tag=
Cyc_Exp_tok;_tmp9FD.f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9FD;});
_tmp9FC;});break;case 461: _LL3D7: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmp9FE=
_cycalloc(sizeof(*_tmp9FE));_tmp9FE[0]=({struct Cyc_Exp_tok_struct _tmp9FF;_tmp9FF.tag=
Cyc_Exp_tok;_tmp9FF.f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp9FF;});
_tmp9FE;});break;case 462: _LL3D8: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpA00=
_cycalloc(sizeof(*_tmpA00));_tmpA00[0]=({struct Cyc_Exp_tok_struct _tmpA01;_tmpA01.tag=
Cyc_Exp_tok;_tmpA01.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_struct*
_tmpA02=_cycalloc(sizeof(*_tmpA02));_tmpA02[0]=({struct Cyc_Absyn_CompoundLit_e_struct
_tmpA03;_tmpA03.tag=27;_tmpA03.f1=Cyc_yyget_ParamDecl_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmpA03.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmpA03;});_tmpA02;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmpA01;});_tmpA00;});break;case 463: _LL3D9: yyval=(
void*)({struct Cyc_Exp_tok_struct*_tmpA04=_cycalloc(sizeof(*_tmpA04));_tmpA04[0]=({
struct Cyc_Exp_tok_struct _tmpA05;_tmpA05.tag=Cyc_Exp_tok;_tmpA05.f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmpA06=_cycalloc(sizeof(*_tmpA06));
_tmpA06[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmpA07;_tmpA07.tag=27;_tmpA07.f1=
Cyc_yyget_ParamDecl_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
5)]);_tmpA07.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmpA07;});_tmpA06;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA05;});
_tmpA04;});break;case 464: _LL3DA: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpA08=
_cycalloc(sizeof(*_tmpA08));_tmpA08[0]=({struct Cyc_Exp_tok_struct _tmpA09;_tmpA09.tag=
Cyc_Exp_tok;_tmpA09.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmpA0A=_cycalloc(sizeof(*_tmpA0A));_tmpA0A[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmpA0B;_tmpA0B.tag=2;_tmpA0B.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmpA0B;});_tmpA0A;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA09;});
_tmpA08;});break;case 465: _LL3DB: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 466: _LL3DC: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmpA0C=_cycalloc(sizeof(*_tmpA0C));_tmpA0C[0]=({struct Cyc_Exp_tok_struct _tmpA0D;
_tmpA0D.tag=Cyc_Exp_tok;_tmpA0D.f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA0D;});
_tmpA0C;});break;case 467: _LL3DD: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 468: _LL3DE: yyval=(void*)({struct Cyc_Exp_tok_struct*
_tmpA0E=_cycalloc(sizeof(*_tmpA0E));_tmpA0E[0]=({struct Cyc_Exp_tok_struct _tmpA0F;
_tmpA0F.tag=Cyc_Exp_tok;_tmpA0F.f1=Cyc_Absyn_noinstantiate_exp(Cyc_yyget_Exp_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA0F;});
_tmpA0E;});break;case 469: _LL3DF: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpA10=
_cycalloc(sizeof(*_tmpA10));_tmpA10[0]=({struct Cyc_Exp_tok_struct _tmpA11;_tmpA11.tag=
Cyc_Exp_tok;_tmpA11.f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA11;});
_tmpA10;});break;case 470: _LL3E0: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpA12=
_cycalloc(sizeof(*_tmpA12));_tmpA12[0]=({struct Cyc_Exp_tok_struct _tmpA13;_tmpA13.tag=
Cyc_Exp_tok;_tmpA13.f1=Cyc_Absyn_tuple_exp(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA13;});
_tmpA12;});break;case 471: _LL3E1: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpA14=
_cycalloc(sizeof(*_tmpA14));_tmpA14[0]=({struct Cyc_Exp_tok_struct _tmpA15;_tmpA15.tag=
Cyc_Exp_tok;_tmpA15.f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmpA16=_cycalloc(sizeof(*_tmpA16));_tmpA16[0]=({struct Cyc_Absyn_Struct_e_struct
_tmpA17;_tmpA17.tag=30;_tmpA17.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmpA17.f2=Cyc_yyget_TypeList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmpA17.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_InitializerList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmpA17.f4=0;
_tmpA17;});_tmpA16;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA15;});
_tmpA14;});break;case 472: _LL3E2: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpA18=
_cycalloc(sizeof(*_tmpA18));_tmpA18[0]=({struct Cyc_Exp_tok_struct _tmpA19;_tmpA19.tag=
Cyc_Exp_tok;_tmpA19.f1=Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA19;});
_tmpA18;});break;case 473: _LL3E3: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmpA1A=_cycalloc(sizeof(*_tmpA1A));_tmpA1A[0]=({struct Cyc_ExpList_tok_struct
_tmpA1B;_tmpA1B.tag=Cyc_ExpList_tok;_tmpA1B.f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_ExpList_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmpA1B;});_tmpA1A;});
break;case 474: _LL3E4: yyval=(void*)({struct Cyc_ExpList_tok_struct*_tmpA1C=
_cycalloc(sizeof(*_tmpA1C));_tmpA1C[0]=({struct Cyc_ExpList_tok_struct _tmpA1D;
_tmpA1D.tag=Cyc_ExpList_tok;_tmpA1D.f1=({struct Cyc_List_List*_tmpA1E=_cycalloc(
sizeof(*_tmpA1E));_tmpA1E->hd=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmpA1E->tl=0;_tmpA1E;});
_tmpA1D;});_tmpA1C;});break;case 475: _LL3E5: yyval=(void*)({struct Cyc_ExpList_tok_struct*
_tmpA1F=_cycalloc(sizeof(*_tmpA1F));_tmpA1F[0]=({struct Cyc_ExpList_tok_struct
_tmpA20;_tmpA20.tag=Cyc_ExpList_tok;_tmpA20.f1=({struct Cyc_List_List*_tmpA21=
_cycalloc(sizeof(*_tmpA21));_tmpA21->hd=Cyc_yyget_Exp_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmpA21->tl=Cyc_yyget_ExpList_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmpA21;});_tmpA20;});
_tmpA1F;});break;case 476: _LL3E6: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpA22=
_cycalloc(sizeof(*_tmpA22));_tmpA22[0]=({struct Cyc_Exp_tok_struct _tmpA23;_tmpA23.tag=
Cyc_Exp_tok;_tmpA23.f1=Cyc_Absyn_int_exp((*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA23;});
_tmpA22;});break;case 477: _LL3E7: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpA24=
_cycalloc(sizeof(*_tmpA24));_tmpA24[0]=({struct Cyc_Exp_tok_struct _tmpA25;_tmpA25.tag=
Cyc_Exp_tok;_tmpA25.f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA25;});
_tmpA24;});break;case 478: _LL3E8: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpA26=
_cycalloc(sizeof(*_tmpA26));_tmpA26[0]=({struct Cyc_Exp_tok_struct _tmpA27;_tmpA27.tag=
Cyc_Exp_tok;_tmpA27.f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA27;});
_tmpA26;});break;case 479: _LL3E9: yyval=(void*)({struct Cyc_Exp_tok_struct*_tmpA28=
_cycalloc(sizeof(*_tmpA28));_tmpA28[0]=({struct Cyc_Exp_tok_struct _tmpA29;_tmpA29.tag=
Cyc_Exp_tok;_tmpA29.f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmpA29;});
_tmpA28;});break;case 480: _LL3EA: yyval=(void*)({struct Cyc_QualId_tok_struct*
_tmpA2A=_cycalloc(sizeof(*_tmpA2A));_tmpA2A[0]=({struct Cyc_QualId_tok_struct
_tmpA2B;_tmpA2B.tag=Cyc_QualId_tok;_tmpA2B.f1=({struct _tuple1*_tmpA2C=_cycalloc(
sizeof(*_tmpA2C));_tmpA2C->f1=Cyc_Absyn_rel_ns_null;_tmpA2C->f2=({struct
_dynforward_ptr*_tmpA2D=_cycalloc(sizeof(*_tmpA2D));_tmpA2D[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmpA2D;});_tmpA2C;});
_tmpA2B;});_tmpA2A;});break;case 481: _LL3EB: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 482: _LL3EC: yyval=(
void*)({struct Cyc_QualId_tok_struct*_tmpA2E=_cycalloc(sizeof(*_tmpA2E));_tmpA2E[
0]=({struct Cyc_QualId_tok_struct _tmpA2F;_tmpA2F.tag=Cyc_QualId_tok;_tmpA2F.f1=({
struct _tuple1*_tmpA30=_cycalloc(sizeof(*_tmpA30));_tmpA30->f1=Cyc_Absyn_rel_ns_null;
_tmpA30->f2=({struct _dynforward_ptr*_tmpA31=_cycalloc(sizeof(*_tmpA31));_tmpA31[
0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmpA31;});_tmpA30;});_tmpA2F;});_tmpA2E;});break;case 483: _LL3ED: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 484: _LL3EE: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 485: _LL3EF:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 486:
_LL3F0: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
487: _LL3F1: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 488: _LL3F2: break;case 489: _LL3F3:((struct Cyc_Lexing_lexbuf*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Parse_lbuf))->v)->lex_curr_pos -=1;break;default: _LL3F4: break;}
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(490,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(139,yyn - 129)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
5465) && Cyc_yycheck[_check_known_subscript_notnull(5466,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(5466,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(139,yyn - 129)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(975,yystate)];if(yyn > - 32768  && yyn < 5465){int sze=
0;struct _dynforward_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 268 / 
sizeof(char*);x ++){if(Cyc_yycheck[_check_known_subscript_notnull(5466,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dynforward_ptr)Cyc_yytname[
_check_known_subscript_notnull(268,x)])+ 15,count ++);}msg=({unsigned int _tmpA32=(
unsigned int)(sze + 15);char*_tmpA33=(char*)_cycalloc_atomic(_check_times(sizeof(
char),_tmpA32 + 1));struct _dynforward_ptr _tmpA35=_tag_dynforward(_tmpA33,sizeof(
char),_tmpA32 + 1);{unsigned int _tmpA34=_tmpA32;unsigned int i;for(i=0;i < _tmpA34;
i ++){_tmpA33[i]='\000';}_tmpA33[_tmpA34]=(char)0;}_tmpA35;});Cyc_strcpy(msg,({
const char*_tmpA36="parse error";_tag_dynforward(_tmpA36,sizeof(char),
_get_zero_arr_size(_tmpA36,12));}));if(count < 5){count=0;for(x=yyn < 0?- yyn: 0;x < 
268 / sizeof(char*);x ++){if(Cyc_yycheck[_check_known_subscript_notnull(5466,x + 
yyn)]== x){Cyc_strcat(msg,(struct _dynforward_ptr)(count == 0?(struct
_dynforward_ptr)({const char*_tmpA37=", expecting `";_tag_dynforward(_tmpA37,
sizeof(char),_get_zero_arr_size(_tmpA37,14));}):(struct _dynforward_ptr)({const
char*_tmpA38=" or `";_tag_dynforward(_tmpA38,sizeof(char),_get_zero_arr_size(
_tmpA38,6));})));Cyc_strcat(msg,(struct _dynforward_ptr)Cyc_yytname[
_check_known_subscript_notnull(268,x)]);Cyc_strcat(msg,({const char*_tmpA39="'";
_tag_dynforward(_tmpA39,sizeof(char),_get_zero_arr_size(_tmpA39,2));}));count ++;}}}
Cyc_yyerror((struct _dynforward_ptr)msg);}else{Cyc_yyerror(({const char*_tmpA3A="parse error";
_tag_dynforward(_tmpA3A,sizeof(char),_get_zero_arr_size(_tmpA3A,12));}));}}goto
yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0)return 1;Cyc_yychar=- 2;}
yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0)return 1;
yyvsp_offset --;yystate=(int)yyss[_check_known_subscript_notnull(10000,--
yyssp_offset)];yylsp_offset --;yyerrhandle: yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(975,yystate)];if(yyn == - 32768)goto yyerrdefault;
yyn +=1;if((yyn < 0  || yyn > 5465) || Cyc_yycheck[_check_known_subscript_notnull(
5466,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(5466,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 974)
return 0;yyvs[_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;
yyls[_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto
yynewstate;}void Cyc_yyprint(int i,void*v){void*_tmpA44=v;struct _tuple13*_tmpA45;
struct _tuple13 _tmpA46;int _tmpA47;char _tmpA48;short _tmpA49;struct _dynforward_ptr
_tmpA4A;struct Cyc_Core_Opt*_tmpA4B;struct Cyc_Core_Opt*_tmpA4C;struct Cyc_Core_Opt
_tmpA4D;struct _dynforward_ptr*_tmpA4E;struct _tuple1*_tmpA4F;struct _tuple1 _tmpA50;
void*_tmpA51;struct _dynforward_ptr*_tmpA52;_LL3F7: if(*((void**)_tmpA44)!= Cyc_Int_tok)
goto _LL3F9;_tmpA45=((struct Cyc_Int_tok_struct*)_tmpA44)->f1;_tmpA46=*_tmpA45;
_tmpA47=_tmpA46.f2;_LL3F8:({struct Cyc_Int_pa_struct _tmpA55;_tmpA55.tag=1;_tmpA55.f1=(
unsigned long)_tmpA47;{void*_tmpA53[1]={& _tmpA55};Cyc_fprintf(Cyc_stderr,({const
char*_tmpA54="%d";_tag_dynforward(_tmpA54,sizeof(char),_get_zero_arr_size(
_tmpA54,3));}),_tag_dynforward(_tmpA53,sizeof(void*),1));}});goto _LL3F6;_LL3F9:
if(*((void**)_tmpA44)!= Cyc_Char_tok)goto _LL3FB;_tmpA48=((struct Cyc_Char_tok_struct*)
_tmpA44)->f1;_LL3FA:({struct Cyc_Int_pa_struct _tmpA58;_tmpA58.tag=1;_tmpA58.f1=(
unsigned long)((int)_tmpA48);{void*_tmpA56[1]={& _tmpA58};Cyc_fprintf(Cyc_stderr,({
const char*_tmpA57="%c";_tag_dynforward(_tmpA57,sizeof(char),_get_zero_arr_size(
_tmpA57,3));}),_tag_dynforward(_tmpA56,sizeof(void*),1));}});goto _LL3F6;_LL3FB:
if(*((void**)_tmpA44)!= Cyc_Short_tok)goto _LL3FD;_tmpA49=((struct Cyc_Short_tok_struct*)
_tmpA44)->f1;_LL3FC:({struct Cyc_Int_pa_struct _tmpA5B;_tmpA5B.tag=1;_tmpA5B.f1=(
unsigned long)((int)_tmpA49);{void*_tmpA59[1]={& _tmpA5B};Cyc_fprintf(Cyc_stderr,({
const char*_tmpA5A="%ds";_tag_dynforward(_tmpA5A,sizeof(char),_get_zero_arr_size(
_tmpA5A,4));}),_tag_dynforward(_tmpA59,sizeof(void*),1));}});goto _LL3F6;_LL3FD:
if(*((void**)_tmpA44)!= Cyc_String_tok)goto _LL3FF;_tmpA4A=((struct Cyc_String_tok_struct*)
_tmpA44)->f1;_LL3FE:({struct Cyc_String_pa_struct _tmpA5E;_tmpA5E.tag=0;_tmpA5E.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmpA4A);{void*_tmpA5C[1]={&
_tmpA5E};Cyc_fprintf(Cyc_stderr,({const char*_tmpA5D="\"%s\"";_tag_dynforward(
_tmpA5D,sizeof(char),_get_zero_arr_size(_tmpA5D,5));}),_tag_dynforward(_tmpA5C,
sizeof(void*),1));}});goto _LL3F6;_LL3FF: if(*((void**)_tmpA44)!= Cyc_Stringopt_tok)
goto _LL401;_tmpA4B=((struct Cyc_Stringopt_tok_struct*)_tmpA44)->f1;if(_tmpA4B != 0)
goto _LL401;_LL400:({void*_tmpA5F[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmpA60="NULL";_tag_dynforward(_tmpA60,sizeof(char),_get_zero_arr_size(_tmpA60,5));}),
_tag_dynforward(_tmpA5F,sizeof(void*),0));});goto _LL3F6;_LL401: if(*((void**)
_tmpA44)!= Cyc_Stringopt_tok)goto _LL403;_tmpA4C=((struct Cyc_Stringopt_tok_struct*)
_tmpA44)->f1;if(_tmpA4C == 0)goto _LL403;_tmpA4D=*_tmpA4C;_tmpA4E=(struct
_dynforward_ptr*)_tmpA4D.v;_LL402:({struct Cyc_String_pa_struct _tmpA63;_tmpA63.tag=
0;_tmpA63.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmpA4E);{void*
_tmpA61[1]={& _tmpA63};Cyc_fprintf(Cyc_stderr,({const char*_tmpA62="\"%s\"";
_tag_dynforward(_tmpA62,sizeof(char),_get_zero_arr_size(_tmpA62,5));}),
_tag_dynforward(_tmpA61,sizeof(void*),1));}});goto _LL3F6;_LL403: if(*((void**)
_tmpA44)!= Cyc_QualId_tok)goto _LL405;_tmpA4F=((struct Cyc_QualId_tok_struct*)
_tmpA44)->f1;_tmpA50=*_tmpA4F;_tmpA51=_tmpA50.f1;_tmpA52=_tmpA50.f2;_LL404: {
struct Cyc_List_List*_tmpA64=0;{void*_tmpA65=_tmpA51;struct Cyc_List_List*_tmpA66;
struct Cyc_List_List*_tmpA67;_LL408: if(_tmpA65 <= (void*)1)goto _LL40C;if(*((int*)
_tmpA65)!= 0)goto _LL40A;_tmpA66=((struct Cyc_Absyn_Rel_n_struct*)_tmpA65)->f1;
_LL409: _tmpA64=_tmpA66;goto _LL407;_LL40A: if(*((int*)_tmpA65)!= 1)goto _LL40C;
_tmpA67=((struct Cyc_Absyn_Abs_n_struct*)_tmpA65)->f1;_LL40B: _tmpA64=_tmpA67;goto
_LL407;_LL40C: if((int)_tmpA65 != 0)goto _LL407;_LL40D: goto _LL407;_LL407:;}for(0;
_tmpA64 != 0;_tmpA64=_tmpA64->tl){({struct Cyc_String_pa_struct _tmpA6A;_tmpA6A.tag=
0;_tmpA6A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct
_dynforward_ptr*)_tmpA64->hd));{void*_tmpA68[1]={& _tmpA6A};Cyc_fprintf(Cyc_stderr,({
const char*_tmpA69="%s::";_tag_dynforward(_tmpA69,sizeof(char),_get_zero_arr_size(
_tmpA69,5));}),_tag_dynforward(_tmpA68,sizeof(void*),1));}});}({struct Cyc_String_pa_struct
_tmpA6D;_tmpA6D.tag=0;_tmpA6D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmpA52);{void*_tmpA6B[1]={& _tmpA6D};Cyc_fprintf(Cyc_stderr,({const char*_tmpA6C="%s::";
_tag_dynforward(_tmpA6C,sizeof(char),_get_zero_arr_size(_tmpA6C,5));}),
_tag_dynforward(_tmpA6B,sizeof(void*),1));}});goto _LL3F6;}_LL405:;_LL406:({void*
_tmpA6E[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpA6F="?";_tag_dynforward(
_tmpA6F,sizeof(char),_get_zero_arr_size(_tmpA6F,2));}),_tag_dynforward(_tmpA6E,
sizeof(void*),0));});goto _LL3F6;_LL3F6:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_Parse_lbuf=({struct Cyc_Core_Opt*
_tmpA70=_cycalloc(sizeof(*_tmpA70));_tmpA70->v=Cyc_Lexing_from_file(f);_tmpA70;});
Cyc_yyparse();return Cyc_Parse_parse_result;}
