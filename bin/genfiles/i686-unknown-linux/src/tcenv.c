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
_dynforward_ptr f1;};typedef struct{int __count;union{unsigned int __wch;char __wchb[
4];}__value;}Cyc___mbstate_t;typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;
typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,struct
_dynforward_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _dynforward_ptr,struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;
short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dynforward_ptr f1;};extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dynforward_ptr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);extern char Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct Cyc_Dict_Dict*Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict*d);int Cyc_Dict_member(struct Cyc_Dict_Dict*
d,void*k);struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*
v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);void**Cyc_Dict_rlookup_opt(
struct _RegionHandle*,struct Cyc_Dict_Dict*d,void*k);struct _tuple0{void*f1;void*f2;
};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*d);
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
struct Cyc_Dict_Dict*Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict*,
void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];
struct Cyc_SlowDict_Dict;extern char Cyc_SlowDict_Present[12];extern char Cyc_SlowDict_Absent[
11];struct Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct _dynforward_ptr
line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _dynforward_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];
int Cyc_strptrcmp(struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;
extern void*Cyc_Absyn_empty_effect;extern struct _tuple1*Cyc_Absyn_exn_name;extern
struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;void*Cyc_Absyn_dynforward_typ(void*
t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dynforward_ptr
Cyc_Absynpp_typ2string(void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
le;};void*Cyc_Tcenv_env_err(struct _dynforward_ptr msg);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(
struct _RegionHandle*);struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct
_RegionHandle*);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,
struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Fenv*old_fenv,
struct Cyc_Absyn_Fndecl*new_fn);struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _dynforward_ptr*,struct
Cyc_List_List*);void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_xtuniondecl(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);void*Cyc_Tcenv_return_typ(
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
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct
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
_RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable);void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_rgn_resetable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*,void*r1,void*r2);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(
struct Cyc_Tcenv_Tenv*te);void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_compress(void*
t);int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e);void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,
struct Cyc_List_List*);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int*Cyc_Tcutil_new_tvar_id();
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);char Cyc_Tcenv_Env_error[14]="\000\000\000\000Env_error\000";
void*Cyc_Tcenv_env_err(struct _dynforward_ptr msg){({struct Cyc_String_pa_struct
_tmp2;_tmp2.tag=0;_tmp2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)msg);{
void*_tmp0[1]={& _tmp2};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="Internal error in tcenv: %s\n";
_tag_dynforward(_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,29));}),
_tag_dynforward(_tmp0,sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(
int)_throw((void*)Cyc_Tcenv_Env_error);}struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;
struct Cyc_Tcenv_CtrlEnv{struct _RegionHandle*ctrl_rgn;void*continue_stmt;void*
break_stmt;struct _tuple4*fallthru_clause;void*next_stmt;int try_depth;};struct Cyc_Tcenv_SharedFenv{
struct _RegionHandle*frgn;void*return_typ;struct Cyc_Dict_Dict*seen_labels;struct
Cyc_Dict_Dict*needed_labels;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*
delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dynforward_ptr*v;
void*b;struct Cyc_Tcenv_Bindings*tl;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*
shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;
struct Cyc_Tcenv_Bindings*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*
ctrl_env;void*capability;void*curr_rgn;int in_notreadctxt;};char Cyc_Tcenv_NoBinding[
14]="\000\000\000\000NoBinding\000";void*Cyc_Tcenv_lookup_binding(struct Cyc_Tcenv_Bindings*
bs,struct _dynforward_ptr*v){for(0;(unsigned int)bs;bs=bs->tl){if(Cyc_strptrcmp(v,
bs->v)== 0)return(void*)bs->b;}(int)_throw((void*)Cyc_Tcenv_NoBinding);}struct
Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*r){return({struct Cyc_Tcenv_Genv*
_tmp3=_region_malloc(r,sizeof(*_tmp3));_tmp3->grgn=r;_tmp3->namespaces=((struct
Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dynforward_ptr*,struct
_dynforward_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp);_tmp3->aggrdecls=((struct Cyc_Dict_Dict*(*)(
struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*)))
Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp3->tuniondecls=((struct Cyc_Dict_Dict*(*)(
struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*)))
Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp3->enumdecls=((struct Cyc_Dict_Dict*(*)(
struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*)))
Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp3->typedefs=((struct Cyc_Dict_Dict*(*)(
struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*)))
Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp3->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*)))
Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp3->availables=0;_tmp3;});}struct _tuple5{
void*f1;int f2;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
Cyc_Tcutil_empty_var_set=({struct Cyc_Core_Opt*_tmp4=_cycalloc(sizeof(*_tmp4));
_tmp4->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dynforward_ptr*,struct
_dynforward_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp4;});{struct Cyc_Tcenv_Genv*
_tmp5=Cyc_Tcenv_empty_genv(r);_tmp5->tuniondecls=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k,struct Cyc_Absyn_Tuniondecl**v))Cyc_Dict_insert)(
_tmp5->tuniondecls,(*Cyc_Absyn_exn_name).f2,({struct Cyc_Absyn_Tuniondecl**_tmp6=
_cycalloc(sizeof(*_tmp6));_tmp6[0]=Cyc_Absyn_exn_tud;_tmp6;}));{struct Cyc_List_List*
_tmp7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;
for(0;_tmp7 != 0;_tmp7=_tmp7->tl){_tmp5->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k,struct _tuple5*v))Cyc_Dict_insert)(
_tmp5->ordinaries,(*((struct Cyc_Absyn_Tunionfield*)_tmp7->hd)->name).f2,({struct
_tuple5*_tmp8=_region_malloc(r,sizeof(*_tmp8));_tmp8->f1=(void*)({struct Cyc_Tcenv_TunionRes_struct*
_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9[0]=({struct Cyc_Tcenv_TunionRes_struct _tmpA;
_tmpA.tag=2;_tmpA.f1=Cyc_Absyn_exn_tud;_tmpA.f2=(struct Cyc_Absyn_Tunionfield*)
_tmp7->hd;_tmpA;});_tmp9;});_tmp8->f2=1;_tmp8;}));}}{struct Cyc_Dict_Dict*ae=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict*(*)(struct _RegionHandle*,int(*cmp)(
struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),
0,_tmp5);return({struct Cyc_Tcenv_Tenv*_tmpB=_region_malloc(r,sizeof(*_tmpB));
_tmpB->ns=0;_tmpB->ae=ae;_tmpB->le=0;_tmpB;});}}}static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*n,
struct Cyc_List_List*ns){return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,
loc,n,ns));}static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*
ns){if(ns == 0)return((struct Cyc_List_List*(*)(struct _dynforward_ptr msg))Cyc_Tcenv_env_err)(({
const char*_tmpC="outer_namespace";_tag_dynforward(_tmpC,sizeof(char),
_get_zero_arr_size(_tmpC,16));}));return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}static int Cyc_Tcenv_same_namespace(
struct Cyc_List_List*n1,struct Cyc_List_List*n2){if(n1 == n2)return 1;for(0;n1 != 0;
n1=n1->tl){if(n2 == 0)return 0;if(Cyc_strptrcmp((struct _dynforward_ptr*)n1->hd,(
struct _dynforward_ptr*)n2->hd)!= 0)return 0;n2=n2->tl;}if(n2 != 0)return 0;return 1;}
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dynforward_ptr*),struct
_dynforward_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*nss){for(0;nss != 0;
nss=nss->tl){if(!Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)nss->hd)){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)nss->hd);struct
_handler_cons _tmpD;_push_handler(& _tmpD);{int _tmpF=0;if(setjmp(_tmpD.handler))
_tmpF=1;if(!_tmpF){lookup(ge2,v);({struct Cyc_String_pa_struct _tmp12;_tmp12.tag=0;
_tmp12.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*v);{void*_tmp10[1]={&
_tmp12};Cyc_Tcutil_terr(loc,({const char*_tmp11="%s is ambiguous";_tag_dynforward(
_tmp11,sizeof(char),_get_zero_arr_size(_tmp11,16));}),_tag_dynforward(_tmp10,
sizeof(void*),1));}});;_pop_handler();}else{void*_tmpE=(void*)_exn_thrown;void*
_tmp14=_tmpE;_LL1: if(_tmp14 != Cyc_Dict_Absent)goto _LL3;_LL2: goto _LL0;_LL3:;_LL4:(
void)_throw(_tmp14);_LL0:;}}}}return;}static void*Cyc_Tcenv_scoped_lookup(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,
struct _dynforward_ptr*),struct _dynforward_ptr*v){struct Cyc_List_List*cns=te->ns;
while(1){struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);{struct _handler_cons _tmp15;
_push_handler(& _tmp15);{int _tmp17=0;if(setjmp(_tmp15.handler))_tmp17=1;if(!
_tmp17){{void*result=lookup(ge,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp18=result;_npop_handler(0);return _tmp18;}};_pop_handler();}else{void*
_tmp16=(void*)_exn_thrown;void*_tmp1A=_tmp16;_LL6: if(_tmp1A != Cyc_Dict_Absent)
goto _LL8;_LL7: goto _LL5;_LL8:;_LL9:(void)_throw(_tmp1A);_LL5:;}}}{struct Cyc_List_List*
_tmp1B=ge->availables;for(0;_tmp1B != 0;_tmp1B=_tmp1B->tl){struct Cyc_Tcenv_Genv*
ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,(struct Cyc_List_List*)_tmp1B->hd);struct _handler_cons _tmp1C;_push_handler(&
_tmp1C);{int _tmp1E=0;if(setjmp(_tmp1C.handler))_tmp1E=1;if(!_tmp1E){{void*result=
lookup(ge2,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)
_tmp1B->hd,_tmp1B->tl);{void*_tmp1F=result;_npop_handler(0);return _tmp1F;}};
_pop_handler();}else{void*_tmp1D=(void*)_exn_thrown;void*_tmp21=_tmp1D;_LLB: if(
_tmp21 != Cyc_Dict_Absent)goto _LLD;_LLC: goto _LLA;_LLD:;_LLE:(void)_throw(_tmp21);
_LLA:;}}}}if(cns == 0)(int)_throw((void*)Cyc_Dict_Absent);cns=Cyc_Tcenv_outer_namespace(
cns);}}static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,
struct _dynforward_ptr*v){struct Cyc_Dict_Dict*_tmp22=ge->ordinaries;struct _tuple5*
ans=((struct _tuple5*(*)(struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(
_tmp22,v);(*ans).f2=1;return(*ans).f1;}static void*Cyc_Tcenv_lookup_ordinary_global(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmp24;void*_tmp25;struct _dynforward_ptr*_tmp26;struct _tuple1*_tmp23=q;
_tmp24=*_tmp23;_tmp25=_tmp24.f1;_tmp26=_tmp24.f2;{void*_tmp27=_tmp25;struct Cyc_List_List*
_tmp28;struct Cyc_List_List*_tmp29;struct Cyc_List_List _tmp2A;struct
_dynforward_ptr*_tmp2B;struct Cyc_List_List*_tmp2C;struct Cyc_List_List*_tmp2D;
_LL10: if((int)_tmp27 != 0)goto _LL12;_LL11: goto _LL13;_LL12: if(_tmp27 <= (void*)1)
goto _LL14;if(*((int*)_tmp27)!= 0)goto _LL14;_tmp28=((struct Cyc_Absyn_Rel_n_struct*)
_tmp27)->f1;if(_tmp28 != 0)goto _LL14;_LL13: return Cyc_Tcenv_scoped_lookup(te,loc,
Cyc_Tcenv_lookup_ordinary_global_f,_tmp26);_LL14: if(_tmp27 <= (void*)1)goto _LL16;
if(*((int*)_tmp27)!= 0)goto _LL16;_tmp29=((struct Cyc_Absyn_Rel_n_struct*)_tmp27)->f1;
if(_tmp29 == 0)goto _LL16;_tmp2A=*_tmp29;_tmp2B=(struct _dynforward_ptr*)_tmp2A.hd;
_tmp2C=_tmp2A.tl;_LL15: {struct Cyc_Tcenv_Genv*_tmp2E=Cyc_Tcenv_lookup_namespace(
te,loc,_tmp2B,_tmp2C);return Cyc_Tcenv_lookup_ordinary_global_f(_tmp2E,_tmp26);}
_LL16: if(_tmp27 <= (void*)1)goto _LLF;if(*((int*)_tmp27)!= 1)goto _LLF;_tmp2D=((
struct Cyc_Absyn_Abs_n_struct*)_tmp27)->f1;_LL17: return Cyc_Tcenv_lookup_ordinary_global_f(((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,_tmp2D),_tmp26);_LLF:;}}struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*n,
struct Cyc_List_List*ns){struct Cyc_List_List*_tmp2F=te->ns;struct _RegionHandle
_tmp30=_new_region("temp");struct _RegionHandle*temp=& _tmp30;_push_region(temp);{
struct Cyc_List_List*_tmp31=0;while(1){struct Cyc_Tcenv_Genv*_tmp32=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp2F);
struct Cyc_List_List*_tmp33=_tmp32->availables;struct Cyc_Set_Set*_tmp34=_tmp32->namespaces;{
struct Cyc_List_List*_tmp35=_tmp33;for(0;_tmp35 != 0;_tmp35=_tmp35->tl){struct Cyc_Set_Set*
_tmp36=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp35->hd))->namespaces;if(((int(*)(
struct Cyc_Set_Set*s,struct _dynforward_ptr*elt))Cyc_Set_member)(_tmp36,n))_tmp31=({
struct Cyc_List_List*_tmp37=_region_malloc(temp,sizeof(*_tmp37));_tmp37->hd=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp2F,({struct Cyc_List_List*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->hd=n;
_tmp38->tl=ns;_tmp38;}));_tmp37->tl=_tmp31;_tmp37;});}}if(((int(*)(struct Cyc_Set_Set*
s,struct _dynforward_ptr*elt))Cyc_Set_member)(_tmp34,n))_tmp31=({struct Cyc_List_List*
_tmp39=_region_malloc(temp,sizeof(*_tmp39));_tmp39->hd=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2F,({struct Cyc_List_List*
_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->hd=n;_tmp3A->tl=ns;_tmp3A;}));_tmp39->tl=
_tmp31;_tmp39;});if(_tmp31 != 0){if(_tmp31->tl != 0)({struct Cyc_String_pa_struct
_tmp3D;_tmp3D.tag=0;_tmp3D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*n);{
void*_tmp3B[1]={& _tmp3D};Cyc_Tcutil_terr(loc,({const char*_tmp3C="%s is ambiguous";
_tag_dynforward(_tmp3C,sizeof(char),_get_zero_arr_size(_tmp3C,16));}),
_tag_dynforward(_tmp3B,sizeof(void*),1));}});{struct Cyc_List_List*_tmp3E=(struct
Cyc_List_List*)_tmp31->hd;_npop_handler(0);return _tmp3E;}}if(_tmp2F == 0)(int)
_throw((void*)Cyc_Dict_Absent);_tmp2F=Cyc_Tcenv_outer_namespace(_tmp2F);}};
_pop_region(temp);}static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dynforward_ptr*v){struct Cyc_Dict_Dict*_tmp3F=ge->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_lookup)(_tmp3F,v);}struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmp41;void*_tmp42;struct _dynforward_ptr*_tmp43;struct _tuple1*_tmp40=q;
_tmp41=*_tmp40;_tmp42=_tmp41.f1;_tmp43=_tmp41.f2;{void*_tmp44=_tmp42;struct Cyc_List_List*
_tmp45;struct Cyc_List_List*_tmp46;struct Cyc_List_List*_tmp47;struct Cyc_List_List
_tmp48;struct _dynforward_ptr*_tmp49;struct Cyc_List_List*_tmp4A;_LL19: if((int)
_tmp44 != 0)goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp44 <= (void*)1)goto _LL1D;if(*((
int*)_tmp44)!= 0)goto _LL1D;_tmp45=((struct Cyc_Absyn_Rel_n_struct*)_tmp44)->f1;
if(_tmp45 != 0)goto _LL1D;_LL1C: return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,
struct _dynforward_ptr*),struct _dynforward_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,
Cyc_Tcenv_lookup_aggrdecl_f,_tmp43);_LL1D: if(_tmp44 <= (void*)1)goto _LL1F;if(*((
int*)_tmp44)!= 1)goto _LL1F;_tmp46=((struct Cyc_Absyn_Abs_n_struct*)_tmp44)->f1;
_LL1E: {struct Cyc_Dict_Dict*_tmp4B=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp46))->aggrdecls;return((
struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(
_tmp4B,_tmp43);}_LL1F: if(_tmp44 <= (void*)1)goto _LL18;if(*((int*)_tmp44)!= 0)goto
_LL18;_tmp47=((struct Cyc_Absyn_Rel_n_struct*)_tmp44)->f1;if(_tmp47 == 0)goto _LL18;
_tmp48=*_tmp47;_tmp49=(struct _dynforward_ptr*)_tmp48.hd;_tmp4A=_tmp48.tl;_LL20: {
struct Cyc_Dict_Dict*_tmp4C=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp49,_tmp4A))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_lookup)(_tmp4C,_tmp43);}_LL18:;}}static struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl_f(struct Cyc_Tcenv_Genv*ge,struct _dynforward_ptr*v){
struct Cyc_Dict_Dict*_tmp4D=ge->tuniondecls;return((struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp4D,v);}
struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q){struct _tuple1 _tmp4F;void*_tmp50;
struct _dynforward_ptr*_tmp51;struct _tuple1*_tmp4E=q;_tmp4F=*_tmp4E;_tmp50=_tmp4F.f1;
_tmp51=_tmp4F.f2;{void*_tmp52=_tmp50;struct Cyc_List_List*_tmp53;struct Cyc_List_List*
_tmp54;struct Cyc_List_List _tmp55;struct _dynforward_ptr*_tmp56;struct Cyc_List_List*
_tmp57;struct Cyc_List_List*_tmp58;_LL22: if((int)_tmp52 != 0)goto _LL24;_LL23: goto
_LL25;_LL24: if(_tmp52 <= (void*)1)goto _LL26;if(*((int*)_tmp52)!= 0)goto _LL26;
_tmp53=((struct Cyc_Absyn_Rel_n_struct*)_tmp52)->f1;if(_tmp53 != 0)goto _LL26;_LL25:
return((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Tuniondecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dynforward_ptr*),struct _dynforward_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_tuniondecl_f,
_tmp51);_LL26: if(_tmp52 <= (void*)1)goto _LL28;if(*((int*)_tmp52)!= 0)goto _LL28;
_tmp54=((struct Cyc_Absyn_Rel_n_struct*)_tmp52)->f1;if(_tmp54 == 0)goto _LL28;
_tmp55=*_tmp54;_tmp56=(struct _dynforward_ptr*)_tmp55.hd;_tmp57=_tmp55.tl;_LL27: {
struct Cyc_Dict_Dict*_tmp59=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp56,_tmp57))->tuniondecls;
return((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_lookup)(_tmp59,_tmp51);}_LL28: if(_tmp52 <= (void*)1)
goto _LL21;if(*((int*)_tmp52)!= 1)goto _LL21;_tmp58=((struct Cyc_Absyn_Abs_n_struct*)
_tmp52)->f1;_LL29: {struct Cyc_Dict_Dict*_tmp5A=(((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp58))->tuniondecls;
return((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_lookup)(_tmp5A,_tmp51);}_LL21:;}}static struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_xtuniondecl_f(struct Cyc_Tcenv_Genv*ge,struct _dynforward_ptr*v){
return((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_lookup)(ge->tuniondecls,v);}struct Cyc_Core_Opt*Cyc_Tcenv_lookup_xtuniondecl(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct _tuple1*q){struct _tuple1 _tmp5C;void*_tmp5D;struct _dynforward_ptr*_tmp5E;
struct _tuple1*_tmp5B=q;_tmp5C=*_tmp5B;_tmp5D=_tmp5C.f1;_tmp5E=_tmp5C.f2;{void*
_tmp5F=_tmp5D;struct Cyc_List_List*_tmp60;struct Cyc_List_List*_tmp61;struct Cyc_List_List
_tmp62;struct _dynforward_ptr*_tmp63;struct Cyc_List_List*_tmp64;struct Cyc_List_List*
_tmp65;_LL2B: if(_tmp5F <= (void*)1)goto _LL2D;if(*((int*)_tmp5F)!= 0)goto _LL2D;
_tmp60=((struct Cyc_Absyn_Rel_n_struct*)_tmp5F)->f1;if(_tmp60 != 0)goto _LL2D;_LL2C: {
struct _handler_cons _tmp66;_push_handler(& _tmp66);{int _tmp68=0;if(setjmp(_tmp66.handler))
_tmp68=1;if(!_tmp68){{struct Cyc_Core_Opt*_tmp6A=({struct Cyc_Core_Opt*_tmp69=
_region_malloc(r,sizeof(*_tmp69));_tmp69->v=((struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Tuniondecl**(*
lookup)(struct Cyc_Tcenv_Genv*,struct _dynforward_ptr*),struct _dynforward_ptr*v))
Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_xtuniondecl_f,_tmp5E);_tmp69;});
_npop_handler(0);return _tmp6A;};_pop_handler();}else{void*_tmp67=(void*)
_exn_thrown;void*_tmp6C=_tmp67;_LL34: if(_tmp6C != Cyc_Dict_Absent)goto _LL36;_LL35:
return 0;_LL36:;_LL37:(void)_throw(_tmp6C);_LL33:;}}}_LL2D: if((int)_tmp5F != 0)
goto _LL2F;_LL2E:({void*_tmp6D[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp6E="lookup_xtuniondecl: impossible";
_tag_dynforward(_tmp6E,sizeof(char),_get_zero_arr_size(_tmp6E,31));}),
_tag_dynforward(_tmp6D,sizeof(void*),0));});return 0;_LL2F: if(_tmp5F <= (void*)1)
goto _LL31;if(*((int*)_tmp5F)!= 0)goto _LL31;_tmp61=((struct Cyc_Absyn_Rel_n_struct*)
_tmp5F)->f1;if(_tmp61 == 0)goto _LL31;_tmp62=*_tmp61;_tmp63=(struct _dynforward_ptr*)
_tmp62.hd;_tmp64=_tmp62.tl;_LL30: {struct Cyc_Tcenv_Genv*ge;{struct _handler_cons
_tmp6F;_push_handler(& _tmp6F);{int _tmp71=0;if(setjmp(_tmp6F.handler))_tmp71=1;
if(!_tmp71){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp63,_tmp64);;_pop_handler();}
else{void*_tmp70=(void*)_exn_thrown;void*_tmp73=_tmp70;_LL39: if(_tmp73 != Cyc_Dict_Absent)
goto _LL3B;_LL3A:({void*_tmp74[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp75="bad qualified name for xtunion";
_tag_dynforward(_tmp75,sizeof(char),_get_zero_arr_size(_tmp75,31));}),
_tag_dynforward(_tmp74,sizeof(void*),0));});(int)_throw((void*)Cyc_Dict_Absent);
_LL3B:;_LL3C:(void)_throw(_tmp73);_LL38:;}}}{struct Cyc_Dict_Dict*_tmp76=ge->tuniondecls;
return({struct Cyc_Core_Opt*_tmp77=_region_malloc(r,sizeof(*_tmp77));_tmp77->v=((
struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))
Cyc_Dict_lookup)(_tmp76,_tmp5E);_tmp77;});}}_LL31: if(_tmp5F <= (void*)1)goto _LL2A;
if(*((int*)_tmp5F)!= 1)goto _LL2A;_tmp65=((struct Cyc_Absyn_Abs_n_struct*)_tmp5F)->f1;
_LL32: {struct Cyc_Dict_Dict*_tmp78=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp65))->tuniondecls;return({
struct Cyc_Core_Opt*_tmp79=_region_malloc(r,sizeof(*_tmp79));_tmp79->v=((struct
Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(
_tmp78,_tmp5E);_tmp79;});}_LL2A:;}}static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dynforward_ptr*v){struct Cyc_Dict_Dict*_tmp7A=ge->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_lookup)(_tmp7A,v);}struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmp7C;void*_tmp7D;struct _dynforward_ptr*_tmp7E;struct _tuple1*_tmp7B=q;
_tmp7C=*_tmp7B;_tmp7D=_tmp7C.f1;_tmp7E=_tmp7C.f2;{void*_tmp7F=_tmp7D;struct Cyc_List_List*
_tmp80;struct Cyc_List_List*_tmp81;struct Cyc_List_List _tmp82;struct
_dynforward_ptr*_tmp83;struct Cyc_List_List*_tmp84;struct Cyc_List_List*_tmp85;
_LL3E: if((int)_tmp7F != 0)goto _LL40;_LL3F: goto _LL41;_LL40: if(_tmp7F <= (void*)1)
goto _LL42;if(*((int*)_tmp7F)!= 0)goto _LL42;_tmp80=((struct Cyc_Absyn_Rel_n_struct*)
_tmp7F)->f1;if(_tmp80 != 0)goto _LL42;_LL41: return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Enumdecl**(*
lookup)(struct Cyc_Tcenv_Genv*,struct _dynforward_ptr*),struct _dynforward_ptr*v))
Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,_tmp7E);_LL42: if(
_tmp7F <= (void*)1)goto _LL44;if(*((int*)_tmp7F)!= 0)goto _LL44;_tmp81=((struct Cyc_Absyn_Rel_n_struct*)
_tmp7F)->f1;if(_tmp81 == 0)goto _LL44;_tmp82=*_tmp81;_tmp83=(struct _dynforward_ptr*)
_tmp82.hd;_tmp84=_tmp82.tl;_LL43: {struct Cyc_Dict_Dict*_tmp86=(Cyc_Tcenv_lookup_namespace(
te,loc,_tmp83,_tmp84))->enumdecls;return((struct Cyc_Absyn_Enumdecl**(*)(struct
Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp86,_tmp7E);}_LL44:
if(_tmp7F <= (void*)1)goto _LL3D;if(*((int*)_tmp7F)!= 1)goto _LL3D;_tmp85=((struct
Cyc_Absyn_Abs_n_struct*)_tmp7F)->f1;_LL45: {struct Cyc_Dict_Dict*_tmp87=(((struct
Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,_tmp85))->enumdecls;return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict*
d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp87,_tmp7E);}_LL3D:;}}static
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*
ge,struct _dynforward_ptr*v){struct Cyc_Dict_Dict*_tmp88=ge->typedefs;return((
struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))
Cyc_Dict_lookup)(_tmp88,v);}struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmp8A;void*_tmp8B;struct _dynforward_ptr*_tmp8C;struct _tuple1*_tmp89=q;
_tmp8A=*_tmp89;_tmp8B=_tmp8A.f1;_tmp8C=_tmp8A.f2;{void*_tmp8D=_tmp8B;struct Cyc_List_List*
_tmp8E;struct Cyc_List_List*_tmp8F;struct Cyc_List_List _tmp90;struct
_dynforward_ptr*_tmp91;struct Cyc_List_List*_tmp92;struct Cyc_List_List*_tmp93;
_LL47: if((int)_tmp8D != 0)goto _LL49;_LL48: goto _LL4A;_LL49: if(_tmp8D <= (void*)1)
goto _LL4B;if(*((int*)_tmp8D)!= 0)goto _LL4B;_tmp8E=((struct Cyc_Absyn_Rel_n_struct*)
_tmp8D)->f1;if(_tmp8E != 0)goto _LL4B;_LL4A: return((struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Typedefdecl*(*
lookup)(struct Cyc_Tcenv_Genv*,struct _dynforward_ptr*),struct _dynforward_ptr*v))
Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,_tmp8C);_LL4B: if(
_tmp8D <= (void*)1)goto _LL4D;if(*((int*)_tmp8D)!= 0)goto _LL4D;_tmp8F=((struct Cyc_Absyn_Rel_n_struct*)
_tmp8D)->f1;if(_tmp8F == 0)goto _LL4D;_tmp90=*_tmp8F;_tmp91=(struct _dynforward_ptr*)
_tmp90.hd;_tmp92=_tmp90.tl;_LL4C: {struct Cyc_Dict_Dict*_tmp94=(Cyc_Tcenv_lookup_namespace(
te,loc,_tmp91,_tmp92))->typedefs;return((struct Cyc_Absyn_Typedefdecl*(*)(struct
Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp94,_tmp8C);}_LL4D:
if(_tmp8D <= (void*)1)goto _LL46;if(*((int*)_tmp8D)!= 1)goto _LL46;_tmp93=((struct
Cyc_Absyn_Abs_n_struct*)_tmp8D)->f1;_LL4E: {struct Cyc_Dict_Dict*_tmp95=(((struct
Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,_tmp93))->typedefs;return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict*
d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp95,_tmp8C);}_LL46:;}}static
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct
_dynforward_ptr err_msg){struct Cyc_Tcenv_Fenv*_tmp96=te->le;if((struct Cyc_Tcenv_Tenv*)
te == 0)((int(*)(struct _dynforward_ptr msg))Cyc_Tcenv_env_err)(err_msg);return(
struct Cyc_Tcenv_Fenv*)_check_null(_tmp96);}static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(
struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){if(te->le
== 0)((int(*)(struct _dynforward_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp97="put_fenv";
_tag_dynforward(_tmp97,sizeof(char),_get_zero_arr_size(_tmp97,9));}));return({
struct Cyc_Tcenv_Tenv*_tmp98=_region_malloc(l,sizeof(*_tmp98));_tmp98->ns=te->ns;
_tmp98->ae=te->ae;_tmp98->le=(struct Cyc_Tcenv_Fenv*)fe;_tmp98;});}static struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*
te){return({struct Cyc_Tcenv_Tenv*_tmp99=_region_malloc(l,sizeof(*_tmp99));_tmp99->ns=
te->ns;_tmp99->ae=te->ae;_tmp99->le=0;_tmp99;});}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmp9B;
struct Cyc_List_List*_tmp9C;struct Cyc_RgnOrder_RgnPO*_tmp9D;struct Cyc_Tcenv_Bindings*
_tmp9E;struct Cyc_Absyn_Stmt*_tmp9F;struct Cyc_Tcenv_CtrlEnv*_tmpA0;void*_tmpA1;
void*_tmpA2;int _tmpA3;struct Cyc_Tcenv_Fenv _tmp9A=*f;_tmp9B=_tmp9A.shared;_tmp9C=
_tmp9A.type_vars;_tmp9D=_tmp9A.region_order;_tmp9E=_tmp9A.locals;_tmp9F=_tmp9A.encloser;
_tmpA0=_tmp9A.ctrl_env;_tmpA1=(void*)_tmp9A.capability;_tmpA2=(void*)_tmp9A.curr_rgn;
_tmpA3=_tmp9A.in_notreadctxt;return({struct Cyc_Tcenv_Fenv*_tmpA4=_region_malloc(
l,sizeof(*_tmpA4));_tmpA4->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp9B;_tmpA4->type_vars=(
struct Cyc_List_List*)_tmp9C;_tmpA4->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp9D;_tmpA4->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp9E);_tmpA4->encloser=(struct Cyc_Absyn_Stmt*)_tmp9F;_tmpA4->ctrl_env=(struct
Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmpA0);_tmpA4->capability=(void*)((
void*)_tmpA1);_tmpA4->curr_rgn=(void*)((void*)_tmpA2);_tmpA4->in_notreadctxt=(
int)_tmpA3;_tmpA4;});}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmpA6;
struct Cyc_List_List*_tmpA7;struct Cyc_RgnOrder_RgnPO*_tmpA8;struct Cyc_Tcenv_Bindings*
_tmpA9;struct Cyc_Absyn_Stmt*_tmpAA;struct Cyc_Tcenv_CtrlEnv*_tmpAB;void*_tmpAC;
void*_tmpAD;int _tmpAE;struct Cyc_Tcenv_Fenv _tmpA5=*f;_tmpA6=_tmpA5.shared;_tmpA7=
_tmpA5.type_vars;_tmpA8=_tmpA5.region_order;_tmpA9=_tmpA5.locals;_tmpAA=_tmpA5.encloser;
_tmpAB=_tmpA5.ctrl_env;_tmpAC=(void*)_tmpA5.capability;_tmpAD=(void*)_tmpA5.curr_rgn;
_tmpAE=_tmpA5.in_notreadctxt;{struct _RegionHandle*_tmpB0;void*_tmpB1;void*_tmpB2;
struct _tuple4*_tmpB3;void*_tmpB4;int _tmpB5;struct Cyc_Tcenv_CtrlEnv _tmpAF=*_tmpAB;
_tmpB0=_tmpAF.ctrl_rgn;_tmpB1=(void*)_tmpAF.continue_stmt;_tmpB2=(void*)_tmpAF.break_stmt;
_tmpB3=_tmpAF.fallthru_clause;_tmpB4=(void*)_tmpAF.next_stmt;_tmpB5=_tmpAF.try_depth;{
struct Cyc_Tcenv_CtrlEnv*_tmpB6=({struct Cyc_Tcenv_CtrlEnv*_tmpB8=_region_malloc(l,
sizeof(*_tmpB8));_tmpB8->ctrl_rgn=_tmpB0;_tmpB8->continue_stmt=(void*)_tmpB1;
_tmpB8->break_stmt=(void*)_tmpB2;_tmpB8->fallthru_clause=_tmpB3;_tmpB8->next_stmt=(
void*)_tmpB4;_tmpB8->try_depth=_tmpB5;_tmpB8;});return({struct Cyc_Tcenv_Fenv*
_tmpB7=_region_malloc(l,sizeof(*_tmpB7));_tmpB7->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmpA6;_tmpB7->type_vars=(struct Cyc_List_List*)_tmpA7;_tmpB7->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmpA8;_tmpB7->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmpA9);_tmpB7->encloser=(struct Cyc_Absyn_Stmt*)_tmpAA;
_tmpB7->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpB6;_tmpB7->capability=(void*)((
void*)_tmpAC);_tmpB7->curr_rgn=(void*)((void*)_tmpAD);_tmpB7->in_notreadctxt=(
int)_tmpAE;_tmpB7;});}}}void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv _tmpBB;struct Cyc_Tcenv_SharedFenv*_tmpBC;struct Cyc_Tcenv_Fenv*
_tmpBA=Cyc_Tcenv_get_fenv(te,({const char*_tmpB9="return_typ";_tag_dynforward(
_tmpB9,sizeof(char),_get_zero_arr_size(_tmpB9,11));}));_tmpBB=*_tmpBA;_tmpBC=
_tmpBB.shared;{void*_tmpBE;struct Cyc_Tcenv_SharedFenv _tmpBD=*_tmpBC;_tmpBE=(void*)
_tmpBD.return_typ;return _tmpBE;}}struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpBF=te->le;if(te->le == 0)return
0;{struct Cyc_List_List*_tmpC1;struct Cyc_Tcenv_Fenv _tmpC0=*((struct Cyc_Tcenv_Fenv*)
_check_null(_tmpBF));_tmpC1=_tmpC0.type_vars;return _tmpC1;}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){struct Cyc_Tcenv_Fenv _tmpC4;
struct Cyc_Tcenv_SharedFenv*_tmpC5;struct Cyc_List_List*_tmpC6;struct Cyc_RgnOrder_RgnPO*
_tmpC7;struct Cyc_Tcenv_Bindings*_tmpC8;struct Cyc_Absyn_Stmt*_tmpC9;struct Cyc_Tcenv_CtrlEnv*
_tmpCA;void*_tmpCB;void*_tmpCC;int _tmpCD;struct Cyc_Tcenv_Fenv*_tmpC3=Cyc_Tcenv_get_fenv(
te,({const char*_tmpC2="add_type_vars";_tag_dynforward(_tmpC2,sizeof(char),
_get_zero_arr_size(_tmpC2,14));}));_tmpC4=*_tmpC3;_tmpC5=_tmpC4.shared;_tmpC6=
_tmpC4.type_vars;_tmpC7=_tmpC4.region_order;_tmpC8=_tmpC4.locals;_tmpC9=_tmpC4.encloser;
_tmpCA=_tmpC4.ctrl_env;_tmpCB=(void*)_tmpC4.capability;_tmpCC=(void*)_tmpC4.curr_rgn;
_tmpCD=_tmpC4.in_notreadctxt;Cyc_Tcutil_add_tvar_identities(tvs);{struct Cyc_List_List*
_tmpCE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(tvs,_tmpC6);Cyc_Tcutil_check_unique_tvars(loc,_tmpCE);{struct
Cyc_Tcenv_Fenv*_tmpCF=({struct Cyc_Tcenv_Fenv*_tmpD0=_region_malloc(r,sizeof(*
_tmpD0));_tmpD0->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpC5;_tmpD0->type_vars=(
struct Cyc_List_List*)_tmpCE;_tmpD0->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmpC7;_tmpD0->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmpC8);_tmpD0->encloser=(struct Cyc_Absyn_Stmt*)_tmpC9;_tmpD0->ctrl_env=(struct
Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmpCA);_tmpD0->capability=(void*)((
void*)_tmpCB);_tmpD0->curr_rgn=(void*)((void*)_tmpCC);_tmpD0->in_notreadctxt=(
int)_tmpCD;_tmpD0;});return Cyc_Tcenv_put_fenv(r,te,_tmpCF);}}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*
_tmpD1=te->le;if(_tmpD1 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);else{struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,(struct Cyc_Tcenv_Fenv*)_tmpD1);return Cyc_Tcenv_put_fenv(
r,te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*
vd){struct _dynforward_ptr*_tmpD2=(*vd->name).f2;{void*_tmpD3=(*vd->name).f1;
_LL50: if((int)_tmpD3 != 0)goto _LL52;_LL51: goto _LL4F;_LL52:;_LL53:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));
_tmpD4[0]=({struct Cyc_Core_Impossible_struct _tmpD5;_tmpD5.tag=Cyc_Core_Impossible;
_tmpD5.f1=({const char*_tmpD6="add_local_var: called with Rel_n";_tag_dynforward(
_tmpD6,sizeof(char),_get_zero_arr_size(_tmpD6,33));});_tmpD5;});_tmpD4;}));_LL4F:;}{
struct Cyc_Tcenv_Fenv _tmpD9;struct Cyc_Tcenv_SharedFenv*_tmpDA;struct Cyc_List_List*
_tmpDB;struct Cyc_RgnOrder_RgnPO*_tmpDC;struct Cyc_Tcenv_Bindings*_tmpDD;struct Cyc_Absyn_Stmt*
_tmpDE;struct Cyc_Tcenv_CtrlEnv*_tmpDF;void*_tmpE0;void*_tmpE1;int _tmpE2;struct
Cyc_Tcenv_Fenv*_tmpD8=Cyc_Tcenv_get_fenv(te,({const char*_tmpD7="add_local_var";
_tag_dynforward(_tmpD7,sizeof(char),_get_zero_arr_size(_tmpD7,14));}));_tmpD9=*
_tmpD8;_tmpDA=_tmpD9.shared;_tmpDB=_tmpD9.type_vars;_tmpDC=_tmpD9.region_order;
_tmpDD=_tmpD9.locals;_tmpDE=_tmpD9.encloser;_tmpDF=_tmpD9.ctrl_env;_tmpE0=(void*)
_tmpD9.capability;_tmpE1=(void*)_tmpD9.curr_rgn;_tmpE2=_tmpD9.in_notreadctxt;{
struct Cyc_Tcenv_Bindings*_tmpE3=({struct Cyc_Tcenv_Bindings*_tmpE6=_region_malloc(
r,sizeof(*_tmpE6));_tmpE6->v=_tmpD2;_tmpE6->b=(void*)((void*)({struct Cyc_Absyn_Local_b_struct*
_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7[0]=({struct Cyc_Absyn_Local_b_struct
_tmpE8;_tmpE8.tag=3;_tmpE8.f1=vd;_tmpE8;});_tmpE7;}));_tmpE6->tl=(struct Cyc_Tcenv_Bindings*)
_tmpDD;_tmpE6;});struct Cyc_Tcenv_Fenv*_tmpE4=({struct Cyc_Tcenv_Fenv*_tmpE5=
_region_malloc(r,sizeof(*_tmpE5));_tmpE5->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmpDA;_tmpE5->type_vars=(struct Cyc_List_List*)_tmpDB;_tmpE5->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmpDC;_tmpE5->locals=(struct Cyc_Tcenv_Bindings*)
_tmpE3;_tmpE5->encloser=(struct Cyc_Absyn_Stmt*)_tmpDE;_tmpE5->ctrl_env=(struct
Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmpDF);_tmpE5->capability=(void*)((
void*)_tmpE0);_tmpE5->curr_rgn=(void*)((void*)_tmpE1);_tmpE5->in_notreadctxt=(
int)_tmpE2;_tmpE5;});return Cyc_Tcenv_put_fenv(r,te,_tmpE4);}}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpE9=te->le;if(_tmpE9 == 0)return Cyc_Tcenv_put_emptyfenv(r,
te);{struct Cyc_Tcenv_SharedFenv*_tmpEB;struct Cyc_List_List*_tmpEC;struct Cyc_RgnOrder_RgnPO*
_tmpED;struct Cyc_Tcenv_Bindings*_tmpEE;struct Cyc_Absyn_Stmt*_tmpEF;struct Cyc_Tcenv_CtrlEnv*
_tmpF0;void*_tmpF1;void*_tmpF2;int _tmpF3;struct Cyc_Tcenv_Fenv _tmpEA=*_tmpE9;
_tmpEB=_tmpEA.shared;_tmpEC=_tmpEA.type_vars;_tmpED=_tmpEA.region_order;_tmpEE=
_tmpEA.locals;_tmpEF=_tmpEA.encloser;_tmpF0=_tmpEA.ctrl_env;_tmpF1=(void*)_tmpEA.capability;
_tmpF2=(void*)_tmpEA.curr_rgn;_tmpF3=_tmpEA.in_notreadctxt;{struct Cyc_Tcenv_Fenv*
_tmpF4=({struct Cyc_Tcenv_Fenv*_tmpF5=_region_malloc(r,sizeof(*_tmpF5));_tmpF5->shared=(
struct Cyc_Tcenv_SharedFenv*)_tmpEB;_tmpF5->type_vars=(struct Cyc_List_List*)
_tmpEC;_tmpF5->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpED;_tmpF5->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmpEE);_tmpF5->encloser=(
struct Cyc_Absyn_Stmt*)_tmpEF;_tmpF5->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct
Cyc_Tcenv_CtrlEnv*)_tmpF0);_tmpF5->capability=(void*)((void*)_tmpF1);_tmpF5->curr_rgn=(
void*)((void*)_tmpF2);_tmpF5->in_notreadctxt=(int)1;_tmpF5;});return Cyc_Tcenv_put_fenv(
r,te,_tmpF4);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpF6=te->le;if(
_tmpF6 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_SharedFenv*
_tmpF8;struct Cyc_List_List*_tmpF9;struct Cyc_RgnOrder_RgnPO*_tmpFA;struct Cyc_Tcenv_Bindings*
_tmpFB;struct Cyc_Absyn_Stmt*_tmpFC;struct Cyc_Tcenv_CtrlEnv*_tmpFD;void*_tmpFE;
void*_tmpFF;int _tmp100;struct Cyc_Tcenv_Fenv _tmpF7=*_tmpF6;_tmpF8=_tmpF7.shared;
_tmpF9=_tmpF7.type_vars;_tmpFA=_tmpF7.region_order;_tmpFB=_tmpF7.locals;_tmpFC=
_tmpF7.encloser;_tmpFD=_tmpF7.ctrl_env;_tmpFE=(void*)_tmpF7.capability;_tmpFF=(
void*)_tmpF7.curr_rgn;_tmp100=_tmpF7.in_notreadctxt;{struct Cyc_Tcenv_Fenv*
_tmp101=({struct Cyc_Tcenv_Fenv*_tmp102=_region_malloc(r,sizeof(*_tmp102));
_tmp102->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpF8;_tmp102->type_vars=(struct
Cyc_List_List*)_tmpF9;_tmp102->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpFA;
_tmp102->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmpFB);
_tmp102->encloser=(struct Cyc_Absyn_Stmt*)_tmpFC;_tmp102->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmpFD);_tmp102->capability=(void*)((void*)_tmpFE);
_tmp102->curr_rgn=(void*)((void*)_tmpFF);_tmp102->in_notreadctxt=(int)0;_tmp102;});
return Cyc_Tcenv_put_fenv(r,te,_tmp101);}}}int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv*_tmp103=te->le;if(_tmp103 == 0)return 0;{struct Cyc_Tcenv_Fenv
_tmp105;int _tmp106;struct Cyc_Tcenv_Fenv*_tmp104=(struct Cyc_Tcenv_Fenv*)_tmp103;
_tmp105=*_tmp104;_tmp106=_tmp105.in_notreadctxt;return _tmp106;}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){struct _dynforward_ptr*
_tmp107=(*vd->name).f2;struct Cyc_Tcenv_Fenv _tmp10A;struct Cyc_Tcenv_SharedFenv*
_tmp10B;struct Cyc_List_List*_tmp10C;struct Cyc_RgnOrder_RgnPO*_tmp10D;struct Cyc_Tcenv_Bindings*
_tmp10E;struct Cyc_Absyn_Stmt*_tmp10F;struct Cyc_Tcenv_CtrlEnv*_tmp110;void*
_tmp111;void*_tmp112;int _tmp113;struct Cyc_Tcenv_Fenv*_tmp109=Cyc_Tcenv_get_fenv(
te,({const char*_tmp108="add_pat_var";_tag_dynforward(_tmp108,sizeof(char),
_get_zero_arr_size(_tmp108,12));}));_tmp10A=*_tmp109;_tmp10B=_tmp10A.shared;
_tmp10C=_tmp10A.type_vars;_tmp10D=_tmp10A.region_order;_tmp10E=_tmp10A.locals;
_tmp10F=_tmp10A.encloser;_tmp110=_tmp10A.ctrl_env;_tmp111=(void*)_tmp10A.capability;
_tmp112=(void*)_tmp10A.curr_rgn;_tmp113=_tmp10A.in_notreadctxt;{struct Cyc_Tcenv_Bindings*
_tmp114=({struct Cyc_Tcenv_Bindings*_tmp117=_region_malloc(r,sizeof(*_tmp117));
_tmp117->v=_tmp107;_tmp117->b=(void*)((void*)({struct Cyc_Absyn_Pat_b_struct*
_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118[0]=({struct Cyc_Absyn_Pat_b_struct
_tmp119;_tmp119.tag=4;_tmp119.f1=vd;_tmp119;});_tmp118;}));_tmp117->tl=(struct
Cyc_Tcenv_Bindings*)_tmp10E;_tmp117;});struct Cyc_Tcenv_Fenv*_tmp115=({struct Cyc_Tcenv_Fenv*
_tmp116=_region_malloc(r,sizeof(*_tmp116));_tmp116->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp10B;_tmp116->type_vars=(struct Cyc_List_List*)_tmp10C;_tmp116->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp10D;_tmp116->locals=(struct Cyc_Tcenv_Bindings*)
_tmp114;_tmp116->encloser=(struct Cyc_Absyn_Stmt*)_tmp10F;_tmp116->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp110);_tmp116->capability=(
void*)((void*)_tmp111);_tmp116->curr_rgn=(void*)((void*)_tmp112);_tmp116->in_notreadctxt=(
int)_tmp113;_tmp116;});return Cyc_Tcenv_put_fenv(r,te,_tmp115);}}void*Cyc_Tcenv_lookup_ordinary(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct _tuple1*q){struct Cyc_Tcenv_Fenv*_tmp11A=te->le;struct _tuple1 _tmp11C;void*
_tmp11D;struct _dynforward_ptr*_tmp11E;struct _tuple1*_tmp11B=q;_tmp11C=*_tmp11B;
_tmp11D=_tmp11C.f1;_tmp11E=_tmp11C.f2;{void*_tmp11F=_tmp11D;struct Cyc_List_List*
_tmp120;_LL55: if((int)_tmp11F != 0)goto _LL57;_LL56: if(_tmp11A == 0)(int)_throw((
void*)Cyc_Dict_Absent);goto _LL58;_LL57: if(_tmp11F <= (void*)1)goto _LL59;if(*((int*)
_tmp11F)!= 0)goto _LL59;_tmp120=((struct Cyc_Absyn_Rel_n_struct*)_tmp11F)->f1;if(
_tmp120 != 0)goto _LL59;if(!(_tmp11A != 0))goto _LL59;_LL58: {struct Cyc_Tcenv_Fenv
_tmp122;struct Cyc_Tcenv_Bindings*_tmp123;struct Cyc_Tcenv_Fenv*_tmp121=(struct Cyc_Tcenv_Fenv*)
_tmp11A;_tmp122=*_tmp121;_tmp123=_tmp122.locals;{struct _handler_cons _tmp124;
_push_handler(& _tmp124);{int _tmp126=0;if(setjmp(_tmp124.handler))_tmp126=1;if(!
_tmp126){{void*_tmp129=(void*)({struct Cyc_Tcenv_VarRes_struct*_tmp127=
_region_malloc(r,sizeof(*_tmp127));_tmp127[0]=({struct Cyc_Tcenv_VarRes_struct
_tmp128;_tmp128.tag=0;_tmp128.f1=(void*)Cyc_Tcenv_lookup_binding(_tmp123,_tmp11E);
_tmp128;});_tmp127;});_npop_handler(0);return _tmp129;};_pop_handler();}else{void*
_tmp125=(void*)_exn_thrown;void*_tmp12B=_tmp125;_LL5C:;_LL5D: return(void*)Cyc_Tcenv_lookup_ordinary_global(
te,loc,q);_LL5E:;_LL5F:(void)_throw(_tmp12B);_LL5B:;}}}}_LL59:;_LL5A: {struct
_handler_cons _tmp12C;_push_handler(& _tmp12C);{int _tmp12E=0;if(setjmp(_tmp12C.handler))
_tmp12E=1;if(!_tmp12E){{void*_tmp12F=(void*)Cyc_Tcenv_lookup_ordinary_global(te,
loc,q);_npop_handler(0);return _tmp12F;};_pop_handler();}else{void*_tmp12D=(void*)
_exn_thrown;void*_tmp131=_tmp12D;_LL61: if(_tmp131 != Cyc_Dict_Absent)goto _LL63;
_LL62: return(void*)({struct Cyc_Tcenv_VarRes_struct*_tmp132=_region_malloc(r,
sizeof(*_tmp132));_tmp132[0]=({struct Cyc_Tcenv_VarRes_struct _tmp133;_tmp133.tag=
0;_tmp133.f1=(void*)((void*)0);_tmp133;});_tmp132;});_LL63:;_LL64:(void)_throw(
_tmp131);_LL60:;}}}_LL54:;}}void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv
_tmp136;struct Cyc_Tcenv_CtrlEnv*_tmp137;struct Cyc_Tcenv_Fenv*_tmp135=Cyc_Tcenv_get_fenv(
te,({const char*_tmp134="process_continue";_tag_dynforward(_tmp134,sizeof(char),
_get_zero_arr_size(_tmp134,17));}));_tmp136=*_tmp135;_tmp137=_tmp136.ctrl_env;{
void*_tmp138=(void*)_tmp137->continue_stmt;struct Cyc_Absyn_Stmt*_tmp139;_LL66:
if(_tmp138 <= (void*)3)goto _LL68;if(*((int*)_tmp138)!= 0)goto _LL68;_tmp139=((
struct Cyc_Tcenv_Stmt_j_struct*)_tmp138)->f1;_LL67: _tmp139->non_local_preds=({
struct Cyc_List_List*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A->hd=s;_tmp13A->tl=
_tmp139->non_local_preds;_tmp13A;});*sopt=(struct Cyc_Absyn_Stmt*)_tmp139;return;
_LL68: if((int)_tmp138 != 0)goto _LL6A;_LL69:({void*_tmp13B[0]={};Cyc_Tcutil_terr(s->loc,({
const char*_tmp13C="continue not in a loop";_tag_dynforward(_tmp13C,sizeof(char),
_get_zero_arr_size(_tmp13C,23));}),_tag_dynforward(_tmp13B,sizeof(void*),0));});
return;_LL6A: if((int)_tmp138 != 1)goto _LL6C;_LL6B: goto _LL6D;_LL6C: if((int)_tmp138
!= 2)goto _LL65;_LL6D:((int(*)(struct _dynforward_ptr msg))Cyc_Tcenv_env_err)(({
const char*_tmp13D="bad continue destination";_tag_dynforward(_tmp13D,sizeof(char),
_get_zero_arr_size(_tmp13D,25));}));_LL65:;}}void Cyc_Tcenv_process_break(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv
_tmp140;struct Cyc_Tcenv_CtrlEnv*_tmp141;struct Cyc_Tcenv_SharedFenv*_tmp142;
struct Cyc_Tcenv_Fenv*_tmp13F=Cyc_Tcenv_get_fenv(te,({const char*_tmp13E="process_break";
_tag_dynforward(_tmp13E,sizeof(char),_get_zero_arr_size(_tmp13E,14));}));_tmp140=*
_tmp13F;_tmp141=_tmp140.ctrl_env;_tmp142=_tmp140.shared;{void*_tmp143=(void*)
_tmp141->break_stmt;struct Cyc_Absyn_Stmt*_tmp144;_LL6F: if(_tmp143 <= (void*)3)
goto _LL71;if(*((int*)_tmp143)!= 0)goto _LL71;_tmp144=((struct Cyc_Tcenv_Stmt_j_struct*)
_tmp143)->f1;_LL70: _tmp144->non_local_preds=({struct Cyc_List_List*_tmp145=
_cycalloc(sizeof(*_tmp145));_tmp145->hd=s;_tmp145->tl=_tmp144->non_local_preds;
_tmp145;});*sopt=(struct Cyc_Absyn_Stmt*)_tmp144;return;_LL71: if((int)_tmp143 != 0)
goto _LL73;_LL72:({void*_tmp146[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp147="break not in a loop or switch";
_tag_dynforward(_tmp147,sizeof(char),_get_zero_arr_size(_tmp147,30));}),
_tag_dynforward(_tmp146,sizeof(void*),0));});return;_LL73: if((int)_tmp143 != 2)
goto _LL75;_LL74: if((void*)_tmp142->return_typ != (void*)((void*)0))({void*_tmp148[
0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp149="break causes function not to return a value";
_tag_dynforward(_tmp149,sizeof(char),_get_zero_arr_size(_tmp149,44));}),
_tag_dynforward(_tmp148,sizeof(void*),0));});return;_LL75: if((int)_tmp143 != 1)
goto _LL6E;_LL76:({void*_tmp14A[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp14B="break causes outer switch clause to implicitly fallthru";
_tag_dynforward(_tmp14B,sizeof(char),_get_zero_arr_size(_tmp14B,56));}),
_tag_dynforward(_tmp14A,sizeof(void*),0));});return;_LL6E:;}}void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dynforward_ptr*l,struct Cyc_Absyn_Stmt**
sopt){struct Cyc_Tcenv_Fenv _tmp14E;struct Cyc_Tcenv_SharedFenv*_tmp14F;struct Cyc_Tcenv_Fenv*
_tmp14D=Cyc_Tcenv_get_fenv(te,({const char*_tmp14C="process_goto";_tag_dynforward(
_tmp14C,sizeof(char),_get_zero_arr_size(_tmp14C,13));}));_tmp14E=*_tmp14D;
_tmp14F=_tmp14E.shared;{struct _RegionHandle _tmp150=_new_region("temp");struct
_RegionHandle*temp=& _tmp150;_push_region(temp);{struct Cyc_Absyn_Stmt**sopt2=((
struct Cyc_Absyn_Stmt**(*)(struct _RegionHandle*,struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_rlookup_opt)(temp,_tmp14F->seen_labels,l);if(sopt2 == 
0){struct Cyc_Dict_Dict*_tmp151=_tmp14F->needed_labels;struct Cyc_List_List**slopt=((
struct Cyc_List_List**(*)(struct _RegionHandle*,struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_rlookup_opt)(temp,_tmp151,l);if(slopt == 0)slopt=({
struct Cyc_List_List**_tmp152=_region_malloc(temp,sizeof(*_tmp152));_tmp152[0]=0;
_tmp152;});{struct _RegionHandle*frgn=_tmp14F->frgn;struct Cyc_List_List*
new_needed=({struct Cyc_List_List*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->hd=
s;_tmp153->tl=*((struct Cyc_List_List**)_check_null(slopt));_tmp153;});_tmp14F->needed_labels=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k,struct Cyc_List_List*
v))Cyc_Dict_insert)(_tmp151,l,new_needed);}}else{struct Cyc_Absyn_Stmt*s=*sopt2;s->non_local_preds=({
struct Cyc_List_List*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154->hd=s;_tmp154->tl=
s->non_local_preds;_tmp154;});*sopt=(struct Cyc_Absyn_Stmt*)s;}};_pop_region(temp);}}
struct _tuple4*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Switch_clause***clauseopt){struct Cyc_Tcenv_Fenv _tmp157;struct
Cyc_Tcenv_CtrlEnv*_tmp158;struct Cyc_Tcenv_Fenv*_tmp156=Cyc_Tcenv_get_fenv(te,({
const char*_tmp155="process_fallthru";_tag_dynforward(_tmp155,sizeof(char),
_get_zero_arr_size(_tmp155,17));}));_tmp157=*_tmp156;_tmp158=_tmp157.ctrl_env;{
struct _tuple4*_tmp159=(struct _tuple4*)_tmp158->fallthru_clause;if(_tmp159 != 0){(((*
_tmp159).f1)->body)->non_local_preds=({struct Cyc_List_List*_tmp15A=_cycalloc(
sizeof(*_tmp15A));_tmp15A->hd=s;_tmp15A->tl=(((*_tmp159).f1)->body)->non_local_preds;
_tmp15A;});*clauseopt=(struct Cyc_Absyn_Switch_clause**)({struct Cyc_Absyn_Switch_clause**
_tmp15B=_cycalloc(sizeof(*_tmp15B));_tmp15B[0]=(*_tmp159).f1;_tmp15B;});}return
_tmp159;}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,
struct Cyc_Absyn_Switch_clause*clause){struct Cyc_Tcenv_Fenv _tmp15E;struct Cyc_Tcenv_SharedFenv*
_tmp15F;struct Cyc_List_List*_tmp160;struct Cyc_RgnOrder_RgnPO*_tmp161;struct Cyc_Tcenv_Bindings*
_tmp162;struct Cyc_Absyn_Stmt*_tmp163;struct Cyc_Tcenv_CtrlEnv*_tmp164;void*
_tmp165;void*_tmp166;int _tmp167;struct Cyc_Tcenv_Fenv*_tmp15D=Cyc_Tcenv_get_fenv(
te,({const char*_tmp15C="set_fallthru";_tag_dynforward(_tmp15C,sizeof(char),
_get_zero_arr_size(_tmp15C,13));}));_tmp15E=*_tmp15D;_tmp15F=_tmp15E.shared;
_tmp160=_tmp15E.type_vars;_tmp161=_tmp15E.region_order;_tmp162=_tmp15E.locals;
_tmp163=_tmp15E.encloser;_tmp164=_tmp15E.ctrl_env;_tmp165=(void*)_tmp15E.capability;
_tmp166=(void*)_tmp15E.curr_rgn;_tmp167=_tmp15E.in_notreadctxt;{struct Cyc_Tcenv_CtrlEnv
_tmp169;struct _RegionHandle*_tmp16A;void*_tmp16B;void*_tmp16C;struct _tuple4*
_tmp16D;void*_tmp16E;int _tmp16F;struct Cyc_Tcenv_CtrlEnv*_tmp168=_tmp164;_tmp169=*
_tmp168;_tmp16A=_tmp169.ctrl_rgn;_tmp16B=(void*)_tmp169.continue_stmt;_tmp16C=(
void*)_tmp169.break_stmt;_tmp16D=_tmp169.fallthru_clause;_tmp16E=(void*)_tmp169.next_stmt;
_tmp16F=_tmp169.try_depth;{struct Cyc_List_List*ft_typ=0;for(0;vds != 0;vds=vds->tl){
ft_typ=({struct Cyc_List_List*_tmp170=_region_malloc(_tmp16A,sizeof(*_tmp170));
_tmp170->hd=(void*)((void*)((struct Cyc_Absyn_Vardecl*)vds->hd)->type);_tmp170->tl=
ft_typ;_tmp170;});}{struct Cyc_Tcenv_CList*_tmp171=(struct Cyc_Tcenv_CList*)Cyc_List_imp_rev(
ft_typ);struct Cyc_Tcenv_CtrlEnv*_tmp172=({struct Cyc_Tcenv_CtrlEnv*_tmp175=
_region_malloc(r,sizeof(*_tmp175));_tmp175->ctrl_rgn=_tmp16A;_tmp175->continue_stmt=(
void*)_tmp16B;_tmp175->break_stmt=(void*)_tmp16C;_tmp175->fallthru_clause=({
struct _tuple4*_tmp176=_region_malloc(_tmp16A,sizeof(*_tmp176));_tmp176->f1=
clause;_tmp176->f2=new_tvs;_tmp176->f3=_tmp171;_tmp176;});_tmp175->next_stmt=(
void*)_tmp16E;_tmp175->try_depth=_tmp16F;_tmp175;});struct Cyc_Tcenv_Fenv*_tmp173=({
struct Cyc_Tcenv_Fenv*_tmp174=_region_malloc(r,sizeof(*_tmp174));_tmp174->shared=(
struct Cyc_Tcenv_SharedFenv*)_tmp15F;_tmp174->type_vars=(struct Cyc_List_List*)
_tmp160;_tmp174->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp161;_tmp174->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmp162);_tmp174->encloser=(
struct Cyc_Absyn_Stmt*)_tmp163;_tmp174->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)
_tmp172;_tmp174->capability=(void*)((void*)_tmp165);_tmp174->curr_rgn=(void*)((
void*)_tmp166);_tmp174->in_notreadctxt=(int)_tmp167;_tmp174;});return Cyc_Tcenv_put_fenv(
r,te,_tmp173);}}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(
r,Cyc_Tcenv_get_fenv(te,({const char*_tmp17A="clear_fallthru";_tag_dynforward(
_tmp17A,sizeof(char),_get_zero_arr_size(_tmp17A,15));})));struct Cyc_Tcenv_Fenv
_tmp178;struct Cyc_Tcenv_CtrlEnv*_tmp179;struct Cyc_Tcenv_Fenv*_tmp177=fe;_tmp178=*
_tmp177;_tmp179=_tmp178.ctrl_env;_tmp179->fallthru_clause=0;return Cyc_Tcenv_put_fenv(
r,te,fe);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){struct Cyc_Tcenv_Fenv
_tmp17D;struct Cyc_Tcenv_SharedFenv*_tmp17E;struct Cyc_List_List*_tmp17F;struct Cyc_RgnOrder_RgnPO*
_tmp180;struct Cyc_Tcenv_Bindings*_tmp181;struct Cyc_Absyn_Stmt*_tmp182;struct Cyc_Tcenv_CtrlEnv*
_tmp183;void*_tmp184;void*_tmp185;int _tmp186;struct Cyc_Tcenv_Fenv*_tmp17C=Cyc_Tcenv_get_fenv(
te,({const char*_tmp17B="set_in_loop";_tag_dynforward(_tmp17B,sizeof(char),
_get_zero_arr_size(_tmp17B,12));}));_tmp17D=*_tmp17C;_tmp17E=_tmp17D.shared;
_tmp17F=_tmp17D.type_vars;_tmp180=_tmp17D.region_order;_tmp181=_tmp17D.locals;
_tmp182=_tmp17D.encloser;_tmp183=_tmp17D.ctrl_env;_tmp184=(void*)_tmp17D.capability;
_tmp185=(void*)_tmp17D.curr_rgn;_tmp186=_tmp17D.in_notreadctxt;{struct Cyc_Tcenv_CtrlEnv*
new_cenv=({struct Cyc_Tcenv_CtrlEnv*_tmp188=_region_malloc(r,sizeof(*_tmp188));
_tmp188->ctrl_rgn=r;_tmp188->continue_stmt=(void*)((void*)({struct Cyc_Tcenv_Stmt_j_struct*
_tmp18B=_region_malloc(r,sizeof(*_tmp18B));_tmp18B[0]=({struct Cyc_Tcenv_Stmt_j_struct
_tmp18C;_tmp18C.tag=0;_tmp18C.f1=continue_dest;_tmp18C;});_tmp18B;}));_tmp188->break_stmt=(
void*)((void*)((void*)_tmp183->next_stmt));_tmp188->next_stmt=(void*)((void*)({
struct Cyc_Tcenv_Stmt_j_struct*_tmp189=_region_malloc(r,sizeof(*_tmp189));_tmp189[
0]=({struct Cyc_Tcenv_Stmt_j_struct _tmp18A;_tmp18A.tag=0;_tmp18A.f1=continue_dest;
_tmp18A;});_tmp189;}));_tmp188->fallthru_clause=(struct _tuple4*)_tmp183->fallthru_clause;
_tmp188->try_depth=_tmp183->try_depth;_tmp188;});struct Cyc_Tcenv_Fenv*new_fenv=({
struct Cyc_Tcenv_Fenv*_tmp187=_region_malloc(r,sizeof(*_tmp187));_tmp187->shared=(
struct Cyc_Tcenv_SharedFenv*)_tmp17E;_tmp187->type_vars=(struct Cyc_List_List*)
_tmp17F;_tmp187->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp180;_tmp187->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmp181);_tmp187->encloser=(
struct Cyc_Absyn_Stmt*)_tmp182;_tmp187->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)
new_cenv;_tmp187->capability=(void*)((void*)_tmp184);_tmp187->curr_rgn=(void*)((
void*)_tmp185);_tmp187->in_notreadctxt=(int)_tmp186;_tmp187;});return Cyc_Tcenv_put_fenv(
r,te,new_fenv);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,
Cyc_Tcenv_get_fenv(te,({const char*_tmp190="enter_try";_tag_dynforward(_tmp190,
sizeof(char),_get_zero_arr_size(_tmp190,10));})));struct Cyc_Tcenv_Fenv _tmp18E;
struct Cyc_Tcenv_CtrlEnv*_tmp18F;struct Cyc_Tcenv_Fenv*_tmp18D=fe;_tmp18E=*_tmp18D;
_tmp18F=_tmp18E.ctrl_env;++ _tmp18F->try_depth;return Cyc_Tcenv_put_fenv(r,te,fe);}
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp193;
struct Cyc_Tcenv_CtrlEnv*_tmp194;struct Cyc_Tcenv_Fenv*_tmp192=Cyc_Tcenv_get_fenv(
te,({const char*_tmp191="get_try_depth";_tag_dynforward(_tmp191,sizeof(char),
_get_zero_arr_size(_tmp191,14));}));_tmp193=*_tmp192;_tmp194=_tmp193.ctrl_env;
return _tmp194->try_depth;}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(
r,Cyc_Tcenv_get_fenv(te,({const char*_tmp198="set_in_switch";_tag_dynforward(
_tmp198,sizeof(char),_get_zero_arr_size(_tmp198,14));})));struct Cyc_Tcenv_Fenv
_tmp196;struct Cyc_Tcenv_CtrlEnv*_tmp197;struct Cyc_Tcenv_Fenv*_tmp195=fe;_tmp196=*
_tmp195;_tmp197=_tmp196.ctrl_env;(void*)(_tmp197->break_stmt=(void*)((void*)
_tmp197->next_stmt));(void*)(_tmp197->next_stmt=(void*)((void*)1));return Cyc_Tcenv_put_fenv(
r,te,fe);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct
Cyc_Tcenv_Tenv*te,void*j){struct Cyc_Tcenv_SharedFenv*_tmp19B;struct Cyc_List_List*
_tmp19C;struct Cyc_RgnOrder_RgnPO*_tmp19D;struct Cyc_Tcenv_Bindings*_tmp19E;struct
Cyc_Absyn_Stmt*_tmp19F;struct Cyc_Tcenv_CtrlEnv*_tmp1A0;void*_tmp1A1;void*_tmp1A2;
int _tmp1A3;struct Cyc_Tcenv_Fenv _tmp19A=*Cyc_Tcenv_get_fenv(te,({const char*
_tmp199="set_next";_tag_dynforward(_tmp199,sizeof(char),_get_zero_arr_size(
_tmp199,9));}));_tmp19B=_tmp19A.shared;_tmp19C=_tmp19A.type_vars;_tmp19D=_tmp19A.region_order;
_tmp19E=_tmp19A.locals;_tmp19F=_tmp19A.encloser;_tmp1A0=_tmp19A.ctrl_env;_tmp1A1=(
void*)_tmp19A.capability;_tmp1A2=(void*)_tmp19A.curr_rgn;_tmp1A3=_tmp19A.in_notreadctxt;{
struct Cyc_Tcenv_CtrlEnv*new_cenv=({struct Cyc_Tcenv_CtrlEnv*_tmp1A5=
_region_malloc(r,sizeof(*_tmp1A5));_tmp1A5->ctrl_rgn=r;_tmp1A5->continue_stmt=(
void*)((void*)((void*)_tmp1A0->continue_stmt));_tmp1A5->break_stmt=(void*)((void*)((
void*)_tmp1A0->break_stmt));_tmp1A5->next_stmt=(void*)j;_tmp1A5->fallthru_clause=(
struct _tuple4*)_tmp1A0->fallthru_clause;_tmp1A5->try_depth=_tmp1A0->try_depth;
_tmp1A5;});struct Cyc_Tcenv_Fenv*new_fenv=({struct Cyc_Tcenv_Fenv*_tmp1A4=
_region_malloc(r,sizeof(*_tmp1A4));_tmp1A4->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp19B;_tmp1A4->type_vars=(struct Cyc_List_List*)_tmp19C;_tmp1A4->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp19D;_tmp1A4->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp19E);_tmp1A4->encloser=(struct Cyc_Absyn_Stmt*)
_tmp19F;_tmp1A4->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv;_tmp1A4->capability=(
void*)((void*)_tmp1A1);_tmp1A4->curr_rgn=(void*)((void*)_tmp1A2);_tmp1A4->in_notreadctxt=(
int)_tmp1A3;_tmp1A4;});return Cyc_Tcenv_put_fenv(r,te,new_fenv);}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*
s){struct Cyc_Tcenv_Fenv _tmp1A8;struct Cyc_Tcenv_SharedFenv*_tmp1A9;struct Cyc_Tcenv_Fenv*
_tmp1A7=Cyc_Tcenv_get_fenv(te,({const char*_tmp1A6="add_label";_tag_dynforward(
_tmp1A6,sizeof(char),_get_zero_arr_size(_tmp1A6,10));}));_tmp1A8=*_tmp1A7;
_tmp1A9=_tmp1A8.shared;{struct Cyc_Dict_Dict*needed=_tmp1A9->needed_labels;{
struct _RegionHandle _tmp1AA=_new_region("temp");struct _RegionHandle*temp=& _tmp1AA;
_push_region(temp);{struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_rlookup_opt)(
temp,needed,v);struct _RegionHandle*frgn=_tmp1A9->frgn;if(sl_opt != 0){_tmp1A9->needed_labels=((
struct Cyc_Dict_Dict*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict*,struct
_dynforward_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{struct Cyc_List_List*_tmp1AB=*
sl_opt;s->non_local_preds=_tmp1AB;for(0;_tmp1AB != 0;_tmp1AB=_tmp1AB->tl){void*
_tmp1AC=(void*)((struct Cyc_Absyn_Stmt*)_tmp1AB->hd)->r;struct Cyc_Absyn_Stmt*
_tmp1AD;struct Cyc_Absyn_Stmt**_tmp1AE;_LL78: if(_tmp1AC <= (void*)1)goto _LL7A;if(*((
int*)_tmp1AC)!= 7)goto _LL7A;_tmp1AD=((struct Cyc_Absyn_Goto_s_struct*)_tmp1AC)->f2;
_tmp1AE=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Goto_s_struct*)_tmp1AC)->f2;
_LL79:*_tmp1AE=(struct Cyc_Absyn_Stmt*)s;goto _LL77;_LL7A:;_LL7B:((int(*)(struct
_dynforward_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp1AF="Tcenv: add_label backpatching of non-goto";
_tag_dynforward(_tmp1AF,sizeof(char),_get_zero_arr_size(_tmp1AF,42));}));goto
_LL77;_LL77:;}}}};_pop_region(temp);}if(((int(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_member)(_tmp1A9->seen_labels,v))({struct Cyc_String_pa_struct
_tmp1B2;_tmp1B2.tag=0;_tmp1B2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
v);{void*_tmp1B0[1]={& _tmp1B2};Cyc_Tcutil_terr(s->loc,({const char*_tmp1B1="Repeated label: %s";
_tag_dynforward(_tmp1B1,sizeof(char),_get_zero_arr_size(_tmp1B1,19));}),
_tag_dynforward(_tmp1B0,sizeof(void*),1));}});_tmp1A9->seen_labels=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(
_tmp1A9->seen_labels,v,s);return te;}}int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv _tmp1B5;struct Cyc_Tcenv_SharedFenv*_tmp1B6;struct Cyc_Tcenv_Fenv*
_tmp1B4=Cyc_Tcenv_get_fenv(te,({const char*_tmp1B3="all_labels_resolved";
_tag_dynforward(_tmp1B3,sizeof(char),_get_zero_arr_size(_tmp1B3,20));}));_tmp1B5=*
_tmp1B4;_tmp1B6=_tmp1B5.shared;return((int(*)(struct Cyc_Dict_Dict*d))Cyc_Dict_is_empty)(
_tmp1B6->needed_labels);}struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv _tmp1B9;struct Cyc_Absyn_Stmt*_tmp1BA;struct Cyc_Tcenv_Fenv*
_tmp1B8=Cyc_Tcenv_get_fenv(te,({const char*_tmp1B7="get_encloser";_tag_dynforward(
_tmp1B7,sizeof(char),_get_zero_arr_size(_tmp1B7,13));}));_tmp1B9=*_tmp1B8;
_tmp1BA=_tmp1B9.encloser;return _tmp1BA;}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){struct Cyc_Tcenv_SharedFenv*
_tmp1BD;struct Cyc_List_List*_tmp1BE;struct Cyc_RgnOrder_RgnPO*_tmp1BF;struct Cyc_Tcenv_Bindings*
_tmp1C0;struct Cyc_Absyn_Stmt*_tmp1C1;struct Cyc_Tcenv_CtrlEnv*_tmp1C2;void*
_tmp1C3;void*_tmp1C4;int _tmp1C5;struct Cyc_Tcenv_Fenv _tmp1BC=*Cyc_Tcenv_get_fenv(
te,({const char*_tmp1BB="set_encloser";_tag_dynforward(_tmp1BB,sizeof(char),
_get_zero_arr_size(_tmp1BB,13));}));_tmp1BD=_tmp1BC.shared;_tmp1BE=_tmp1BC.type_vars;
_tmp1BF=_tmp1BC.region_order;_tmp1C0=_tmp1BC.locals;_tmp1C1=_tmp1BC.encloser;
_tmp1C2=_tmp1BC.ctrl_env;_tmp1C3=(void*)_tmp1BC.capability;_tmp1C4=(void*)
_tmp1BC.curr_rgn;_tmp1C5=_tmp1BC.in_notreadctxt;{struct Cyc_Tcenv_Fenv*_tmp1C6=({
struct Cyc_Tcenv_Fenv*_tmp1C7=_region_malloc(r,sizeof(*_tmp1C7));_tmp1C7->shared=(
struct Cyc_Tcenv_SharedFenv*)_tmp1BD;_tmp1C7->type_vars=(struct Cyc_List_List*)
_tmp1BE;_tmp1C7->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1BF;_tmp1C7->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmp1C0);_tmp1C7->encloser=(
struct Cyc_Absyn_Stmt*)s;_tmp1C7->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp1C2);_tmp1C7->capability=(void*)((void*)_tmp1C3);_tmp1C7->curr_rgn=(void*)((
void*)_tmp1C4);_tmp1C7->in_notreadctxt=(int)_tmp1C5;_tmp1C7;});return Cyc_Tcenv_put_fenv(
r,te,_tmp1C6);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable){struct Cyc_Absyn_Tvar*tv;{void*
_tmp1C8=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp1C9;_LL7D: if(_tmp1C8 <= (
void*)4)goto _LL7F;if(*((int*)_tmp1C8)!= 1)goto _LL7F;_tmp1C9=((struct Cyc_Absyn_VarType_struct*)
_tmp1C8)->f1;_LL7E: tv=_tmp1C9;goto _LL7C;_LL7F:;_LL80: tv=((struct Cyc_Absyn_Tvar*(*)(
struct _dynforward_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp1CA="bad add region";
_tag_dynforward(_tmp1CA,sizeof(char),_get_zero_arr_size(_tmp1CA,15));}));goto
_LL7C;_LL7C:;}{struct Cyc_Tcenv_SharedFenv*_tmp1CD;struct Cyc_List_List*_tmp1CE;
struct Cyc_RgnOrder_RgnPO*_tmp1CF;struct Cyc_Tcenv_Bindings*_tmp1D0;struct Cyc_Absyn_Stmt*
_tmp1D1;struct Cyc_Tcenv_CtrlEnv*_tmp1D2;void*_tmp1D3;void*_tmp1D4;int _tmp1D5;
struct Cyc_Tcenv_Fenv _tmp1CC=*Cyc_Tcenv_get_fenv(te,({const char*_tmp1CB="add_region";
_tag_dynforward(_tmp1CB,sizeof(char),_get_zero_arr_size(_tmp1CB,11));}));_tmp1CD=
_tmp1CC.shared;_tmp1CE=_tmp1CC.type_vars;_tmp1CF=_tmp1CC.region_order;_tmp1D0=
_tmp1CC.locals;_tmp1D1=_tmp1CC.encloser;_tmp1D2=_tmp1CC.ctrl_env;_tmp1D3=(void*)
_tmp1CC.capability;_tmp1D4=(void*)_tmp1CC.curr_rgn;_tmp1D5=_tmp1CC.in_notreadctxt;
_tmp1CF=Cyc_RgnOrder_add_youngest(_tmp1CF,tv,resetable);_tmp1D3=(void*)({struct
Cyc_Absyn_JoinEff_struct*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6[0]=({struct
Cyc_Absyn_JoinEff_struct _tmp1D7;_tmp1D7.tag=20;_tmp1D7.f1=({struct Cyc_List_List*
_tmp1D8=_cycalloc(sizeof(*_tmp1D8));_tmp1D8->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1DB;_tmp1DB.tag=19;_tmp1DB.f1=(void*)rgn;_tmp1DB;});_tmp1DA;}));_tmp1D8->tl=({
struct Cyc_List_List*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9->hd=(void*)
_tmp1D3;_tmp1D9->tl=0;_tmp1D9;});_tmp1D8;});_tmp1D7;});_tmp1D6;});{struct Cyc_Tcenv_Fenv*
_tmp1DC=({struct Cyc_Tcenv_Fenv*_tmp1DD=_region_malloc(r,sizeof(*_tmp1DD));
_tmp1DD->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1CD;_tmp1DD->type_vars=(struct
Cyc_List_List*)_tmp1CE;_tmp1DD->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1CF;
_tmp1DD->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmp1D0);
_tmp1DD->encloser=(struct Cyc_Absyn_Stmt*)_tmp1D1;_tmp1DD->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmp1D2);_tmp1DD->capability=(void*)((void*)_tmp1D3);
_tmp1DD->curr_rgn=(void*)((void*)_tmp1D4);_tmp1DD->in_notreadctxt=(int)_tmp1D5;
_tmp1DD;});return Cyc_Tcenv_put_fenv(r,te,_tmp1DC);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Tvar*block_rgn){struct Cyc_Tcenv_SharedFenv*_tmp1E0;struct Cyc_List_List*
_tmp1E1;struct Cyc_RgnOrder_RgnPO*_tmp1E2;struct Cyc_Tcenv_Bindings*_tmp1E3;struct
Cyc_Absyn_Stmt*_tmp1E4;struct Cyc_Tcenv_CtrlEnv*_tmp1E5;void*_tmp1E6;void*_tmp1E7;
int _tmp1E8;struct Cyc_Tcenv_Fenv _tmp1DF=*Cyc_Tcenv_get_fenv(te,({const char*
_tmp1DE="new_named_block";_tag_dynforward(_tmp1DE,sizeof(char),
_get_zero_arr_size(_tmp1DE,16));}));_tmp1E0=_tmp1DF.shared;_tmp1E1=_tmp1DF.type_vars;
_tmp1E2=_tmp1DF.region_order;_tmp1E3=_tmp1DF.locals;_tmp1E4=_tmp1DF.encloser;
_tmp1E5=_tmp1DF.ctrl_env;_tmp1E6=(void*)_tmp1DF.capability;_tmp1E7=(void*)
_tmp1DF.curr_rgn;_tmp1E8=_tmp1DF.in_notreadctxt;{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(
r,Cyc_Tcenv_get_fenv(te,({const char*_tmp1F4="new_block";_tag_dynforward(_tmp1F4,
sizeof(char),_get_zero_arr_size(_tmp1F4,10));})));void*block_typ=(void*)({struct
Cyc_Absyn_VarType_struct*_tmp1F2=_cycalloc(sizeof(*_tmp1F2));_tmp1F2[0]=({struct
Cyc_Absyn_VarType_struct _tmp1F3;_tmp1F3.tag=1;_tmp1F3.f1=block_rgn;_tmp1F3;});
_tmp1F2;});_tmp1E1=({struct Cyc_List_List*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));
_tmp1E9->hd=block_rgn;_tmp1E9->tl=_tmp1E1;_tmp1E9;});Cyc_Tcutil_check_unique_tvars(
loc,_tmp1E1);_tmp1E2=Cyc_RgnOrder_add_youngest(_tmp1E2,block_rgn,0);_tmp1E6=(
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));
_tmp1EA[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1EB;_tmp1EB.tag=20;_tmp1EB.f1=({
struct Cyc_List_List*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1EE=_cycalloc(sizeof(*_tmp1EE));
_tmp1EE[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1EF;_tmp1EF.tag=19;_tmp1EF.f1=(
void*)block_typ;_tmp1EF;});_tmp1EE;}));_tmp1EC->tl=({struct Cyc_List_List*_tmp1ED=
_cycalloc(sizeof(*_tmp1ED));_tmp1ED->hd=(void*)_tmp1E6;_tmp1ED->tl=0;_tmp1ED;});
_tmp1EC;});_tmp1EB;});_tmp1EA;});_tmp1E7=block_typ;{struct Cyc_Tcenv_Fenv*_tmp1F0=({
struct Cyc_Tcenv_Fenv*_tmp1F1=_region_malloc(r,sizeof(*_tmp1F1));_tmp1F1->shared=(
struct Cyc_Tcenv_SharedFenv*)_tmp1E0;_tmp1F1->type_vars=(struct Cyc_List_List*)
_tmp1E1;_tmp1F1->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1E2;_tmp1F1->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmp1E3);_tmp1F1->encloser=(
struct Cyc_Absyn_Stmt*)_tmp1E4;_tmp1F1->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmp1E5);_tmp1F1->capability=(void*)((void*)_tmp1E6);
_tmp1F1->curr_rgn=(void*)((void*)_tmp1E7);_tmp1F1->in_notreadctxt=(int)_tmp1E8;
_tmp1F1;});return Cyc_Tcenv_put_fenv(r,te,_tmp1F0);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp1F6;_tmp1F6.tag=0;_tmp1F6.f1=(void*)((void*)3);_tmp1F6;});_tmp1F5;}));Cyc_Tcutil_add_tvar_identity(
t);return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs){struct Cyc_Tcenv_SharedFenv*
_tmp1F9;struct Cyc_List_List*_tmp1FA;struct Cyc_RgnOrder_RgnPO*_tmp1FB;struct Cyc_Tcenv_Bindings*
_tmp1FC;struct Cyc_Absyn_Stmt*_tmp1FD;struct Cyc_Tcenv_CtrlEnv*_tmp1FE;void*
_tmp1FF;void*_tmp200;int _tmp201;struct Cyc_Tcenv_Fenv _tmp1F8=*Cyc_Tcenv_get_fenv(
te,({const char*_tmp1F7="new_outlives_constraints";_tag_dynforward(_tmp1F7,
sizeof(char),_get_zero_arr_size(_tmp1F7,25));}));_tmp1F9=_tmp1F8.shared;_tmp1FA=
_tmp1F8.type_vars;_tmp1FB=_tmp1F8.region_order;_tmp1FC=_tmp1F8.locals;_tmp1FD=
_tmp1F8.encloser;_tmp1FE=_tmp1F8.ctrl_env;_tmp1FF=(void*)_tmp1F8.capability;
_tmp200=(void*)_tmp1F8.curr_rgn;_tmp201=_tmp1F8.in_notreadctxt;for(0;cs != 0;cs=
cs->tl){_tmp1FB=Cyc_RgnOrder_add_outlives_constraint(_tmp1FB,(*((struct _tuple0*)
cs->hd)).f1,(*((struct _tuple0*)cs->hd)).f2);}{struct Cyc_Tcenv_Fenv*_tmp202=({
struct Cyc_Tcenv_Fenv*_tmp203=_region_malloc(r,sizeof(*_tmp203));_tmp203->shared=(
struct Cyc_Tcenv_SharedFenv*)_tmp1F9;_tmp203->type_vars=(struct Cyc_List_List*)
_tmp1FA;_tmp203->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1FB;_tmp203->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmp1FC);_tmp203->encloser=(
struct Cyc_Absyn_Stmt*)_tmp1FD;_tmp203->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmp1FE);_tmp203->capability=(void*)((void*)_tmp1FF);
_tmp203->curr_rgn=(void*)((void*)_tmp200);_tmp203->in_notreadctxt=(int)_tmp201;
_tmp203;});return Cyc_Tcenv_put_fenv(r,te,_tmp202);}}void*Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmp204=te->le;if(_tmp204 == 0)
return(void*)2;{struct Cyc_Tcenv_Fenv*fe=(struct Cyc_Tcenv_Fenv*)_tmp204;struct Cyc_Tcenv_Fenv
_tmp206;void*_tmp207;struct Cyc_Tcenv_Fenv*_tmp205=fe;_tmp206=*_tmp205;_tmp207=(
void*)_tmp206.curr_rgn;return _tmp207;}}void Cyc_Tcenv_check_rgn_accessible(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*rgn){struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_get_fenv(te,({const char*_tmp211="check_rgn_accessible";
_tag_dynforward(_tmp211,sizeof(char),_get_zero_arr_size(_tmp211,21));}));struct
Cyc_Tcenv_Fenv _tmp209;void*_tmp20A;struct Cyc_RgnOrder_RgnPO*_tmp20B;struct Cyc_Tcenv_Fenv*
_tmp208=fe;_tmp209=*_tmp208;_tmp20A=(void*)_tmp209.capability;_tmp20B=_tmp209.region_order;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp20A) || Cyc_Tcutil_region_in_effect(1,
rgn,_tmp20A))return;if(Cyc_RgnOrder_eff_outlives_eff(_tmp20B,(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp20D;_tmp20D.tag=19;_tmp20D.f1=(void*)rgn;_tmp20D;});_tmp20C;}),_tmp20A))
return;({struct Cyc_String_pa_struct _tmp210;_tmp210.tag=0;_tmp210.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(rgn));{void*
_tmp20E[1]={& _tmp210};Cyc_Tcutil_terr(loc,({const char*_tmp20F="Expression accesses unavailable region %s";
_tag_dynforward(_tmp20F,sizeof(char),_get_zero_arr_size(_tmp20F,42));}),
_tag_dynforward(_tmp20E,sizeof(void*),1));}});}void Cyc_Tcenv_check_rgn_resetable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*rgn){Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);{struct Cyc_Tcenv_Fenv _tmp214;struct Cyc_RgnOrder_RgnPO*_tmp215;struct
Cyc_Tcenv_Fenv*_tmp213=Cyc_Tcenv_get_fenv(te,({const char*_tmp212="check_rgn_resetable";
_tag_dynforward(_tmp212,sizeof(char),_get_zero_arr_size(_tmp212,20));}));_tmp214=*
_tmp213;_tmp215=_tmp214.region_order;{void*_tmp216=Cyc_Tcutil_compress(rgn);
struct Cyc_Absyn_Tvar*_tmp217;_LL82: if(_tmp216 <= (void*)4)goto _LL84;if(*((int*)
_tmp216)!= 1)goto _LL84;_tmp217=((struct Cyc_Absyn_VarType_struct*)_tmp216)->f1;
_LL83: if(!Cyc_RgnOrder_is_region_resetable(_tmp215,_tmp217))({struct Cyc_String_pa_struct
_tmp21A;_tmp21A.tag=0;_tmp21A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(rgn));{void*_tmp218[1]={& _tmp21A};Cyc_Tcutil_terr(loc,({
const char*_tmp219="Region %s is not resetable";_tag_dynforward(_tmp219,sizeof(
char),_get_zero_arr_size(_tmp219,27));}),_tag_dynforward(_tmp218,sizeof(void*),1));}});
return;_LL84:;_LL85:({void*_tmp21B[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp21C="check_rgn_resetable";
_tag_dynforward(_tmp21C,sizeof(char),_get_zero_arr_size(_tmp21C,20));}),
_tag_dynforward(_tmp21B,sizeof(void*),0));});_LL81:;}}}int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){struct Cyc_Tcenv_Fenv*_tmp21D=te->le;
if(_tmp21D == 0)return rt_a == (void*)2;{struct Cyc_Tcenv_Fenv*fe=(struct Cyc_Tcenv_Fenv*)
_tmp21D;struct Cyc_Tcenv_Fenv _tmp21F;struct Cyc_RgnOrder_RgnPO*_tmp220;struct Cyc_Tcenv_Fenv*
_tmp21E=fe;_tmp21F=*_tmp21E;_tmp220=_tmp21F.region_order;return Cyc_RgnOrder_effect_outlives(
_tmp220,(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp221=_cycalloc(sizeof(*
_tmp221));_tmp221[0]=({struct Cyc_Absyn_AccessEff_struct _tmp222;_tmp222.tag=19;
_tmp222.f1=(void*)rt_a;_tmp222;});_tmp221;}),rt_b);}}struct _tuple6{void*f1;void*
f2;struct Cyc_RgnOrder_RgnPO*f3;struct Cyc_Position_Segment*f4;};void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff){struct Cyc_Tcenv_Fenv
_tmp225;void*_tmp226;struct Cyc_RgnOrder_RgnPO*_tmp227;struct Cyc_Tcenv_SharedFenv*
_tmp228;struct Cyc_Tcenv_Fenv*_tmp224=Cyc_Tcenv_get_fenv(te,({const char*_tmp223="check_effect_accessible";
_tag_dynforward(_tmp223,sizeof(char),_get_zero_arr_size(_tmp223,24));}));_tmp225=*
_tmp224;_tmp226=(void*)_tmp225.capability;_tmp227=_tmp225.region_order;_tmp228=
_tmp225.shared;if(Cyc_Tcutil_subset_effect(0,eff,_tmp226))return;if(Cyc_RgnOrder_eff_outlives_eff(
_tmp227,eff,_tmp226))return;{struct _RegionHandle*frgn=_tmp228->frgn;_tmp228->delayed_effect_checks=({
struct Cyc_List_List*_tmp229=_region_malloc(frgn,sizeof(*_tmp229));_tmp229->hd=({
struct _tuple6*_tmp22A=_region_malloc(frgn,sizeof(*_tmp22A));_tmp22A->f1=_tmp226;
_tmp22A->f2=eff;_tmp22A->f3=_tmp227;_tmp22A->f4=loc;_tmp22A;});_tmp229->tl=
_tmp228->delayed_effect_checks;_tmp229;});}}void Cyc_Tcenv_check_delayed_effects(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp22D;struct Cyc_Tcenv_SharedFenv*
_tmp22E;struct Cyc_Tcenv_Fenv*_tmp22C=Cyc_Tcenv_get_fenv(te,({const char*_tmp22B="check_delayed_effects";
_tag_dynforward(_tmp22B,sizeof(char),_get_zero_arr_size(_tmp22B,22));}));_tmp22D=*
_tmp22C;_tmp22E=_tmp22D.shared;{struct Cyc_List_List*_tmp22F=_tmp22E->delayed_effect_checks;
for(0;_tmp22F != 0;_tmp22F=_tmp22F->tl){struct _tuple6 _tmp231;void*_tmp232;void*
_tmp233;struct Cyc_RgnOrder_RgnPO*_tmp234;struct Cyc_Position_Segment*_tmp235;
struct _tuple6*_tmp230=(struct _tuple6*)_tmp22F->hd;_tmp231=*_tmp230;_tmp232=
_tmp231.f1;_tmp233=_tmp231.f2;_tmp234=_tmp231.f3;_tmp235=_tmp231.f4;if(Cyc_Tcutil_subset_effect(
1,_tmp233,_tmp232))continue;if(Cyc_RgnOrder_eff_outlives_eff(_tmp234,_tmp233,
_tmp232))continue;({struct Cyc_String_pa_struct _tmp239;_tmp239.tag=0;_tmp239.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp233));{
struct Cyc_String_pa_struct _tmp238;_tmp238.tag=0;_tmp238.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp232));{void*
_tmp236[2]={& _tmp238,& _tmp239};Cyc_Tcutil_terr(_tmp235,({const char*_tmp237="Capability \n%s\ndoes not cover function's effect\n%s";
_tag_dynforward(_tmp237,sizeof(char),_get_zero_arr_size(_tmp237,51));}),
_tag_dynforward(_tmp236,sizeof(void*),2));}}});}}}struct _tuple7{struct Cyc_RgnOrder_RgnPO*
f1;struct Cyc_List_List*f2;struct Cyc_Position_Segment*f3;};void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po){
struct Cyc_Tcenv_Fenv*_tmp23A=te->le;if(_tmp23A == 0){for(0;po != 0;po=po->tl){if(!
Cyc_Tcutil_subset_effect(1,(*((struct _tuple0*)po->hd)).f1,Cyc_Absyn_empty_effect)
 || !Cyc_Tcutil_subset_effect(1,(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp23B=_cycalloc(sizeof(*_tmp23B));_tmp23B[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp23C;_tmp23C.tag=19;_tmp23C.f1=(void*)(*((struct _tuple0*)po->hd)).f2;_tmp23C;});
_tmp23B;}),Cyc_Absyn_empty_effect))({void*_tmp23D[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp23E="the required region ordering is not satisfied here";
_tag_dynforward(_tmp23E,sizeof(char),_get_zero_arr_size(_tmp23E,51));}),
_tag_dynforward(_tmp23D,sizeof(void*),0));});}return;}{struct Cyc_Tcenv_Fenv
_tmp240;struct Cyc_RgnOrder_RgnPO*_tmp241;struct Cyc_Tcenv_SharedFenv*_tmp242;
struct Cyc_Tcenv_Fenv*_tmp23F=(struct Cyc_Tcenv_Fenv*)_tmp23A;_tmp240=*_tmp23F;
_tmp241=_tmp240.region_order;_tmp242=_tmp240.shared;if(!Cyc_RgnOrder_satisfies_constraints(
_tmp241,po,(void*)2,0))_tmp242->delayed_constraint_checks=({struct Cyc_List_List*
_tmp243=_region_malloc(_tmp242->frgn,sizeof(*_tmp243));_tmp243->hd=({struct
_tuple7*_tmp244=_region_malloc(_tmp242->frgn,sizeof(*_tmp244));_tmp244->f1=
_tmp241;_tmp244->f2=po;_tmp244->f3=loc;_tmp244;});_tmp243->tl=_tmp242->delayed_constraint_checks;
_tmp243;});}}void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv _tmp247;struct Cyc_Tcenv_SharedFenv*_tmp248;struct Cyc_Tcenv_Fenv*
_tmp246=Cyc_Tcenv_get_fenv(te,({const char*_tmp245="check_delayed_constraints";
_tag_dynforward(_tmp245,sizeof(char),_get_zero_arr_size(_tmp245,26));}));_tmp247=*
_tmp246;_tmp248=_tmp247.shared;{struct Cyc_List_List*_tmp249=_tmp248->delayed_constraint_checks;
for(0;_tmp249 != 0;_tmp249=_tmp249->tl){struct _tuple7 _tmp24B;struct Cyc_RgnOrder_RgnPO*
_tmp24C;struct Cyc_List_List*_tmp24D;struct Cyc_Position_Segment*_tmp24E;struct
_tuple7*_tmp24A=(struct _tuple7*)_tmp249->hd;_tmp24B=*_tmp24A;_tmp24C=_tmp24B.f1;
_tmp24D=_tmp24B.f2;_tmp24E=_tmp24B.f3;if(!Cyc_RgnOrder_satisfies_constraints(
_tmp24C,_tmp24D,(void*)2,1))({void*_tmp24F[0]={};Cyc_Tcutil_terr(_tmp24E,({const
char*_tmp250="the required region ordering is not satisfied here";
_tag_dynforward(_tmp250,sizeof(char),_get_zero_arr_size(_tmp250,51));}),
_tag_dynforward(_tmp24F,sizeof(void*),0));});}}}struct _tuple8{struct
_dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Tcenv_Bindings*locals=0;struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*
_tmp27E=_cycalloc(sizeof(*_tmp27E));_tmp27E->name=({struct _dynforward_ptr*
_tmp281=_cycalloc(sizeof(*_tmp281));_tmp281[0]=(struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp284;_tmp284.tag=0;_tmp284.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*(*fd->name).f2);{void*_tmp282[1]={& _tmp284};Cyc_aprintf(({
const char*_tmp283="`%s";_tag_dynforward(_tmp283,sizeof(char),_get_zero_arr_size(
_tmp283,4));}),_tag_dynforward(_tmp282,sizeof(void*),1));}});_tmp281;});_tmp27E->identity=
Cyc_Tcutil_new_tvar_id();_tmp27E->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp27F=_cycalloc(sizeof(*_tmp27F));_tmp27F[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp280;_tmp280.tag=0;_tmp280.f1=(void*)((void*)3);_tmp280;});_tmp27F;}));
_tmp27E;});struct Cyc_List_List*_tmp251=({struct Cyc_List_List*_tmp27D=_cycalloc(
sizeof(*_tmp27D));_tmp27D->hd=rgn0;_tmp27D->tl=fd->tvs;_tmp27D;});Cyc_Tcutil_check_unique_tvars(
loc,_tmp251);{struct Cyc_RgnOrder_RgnPO*_tmp252=Cyc_RgnOrder_initial_fn_po(fd->tvs,
fd->rgn_po,(void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v,rgn0);void*
param_rgn=(void*)({struct Cyc_Absyn_VarType_struct*_tmp27B=_cycalloc(sizeof(*
_tmp27B));_tmp27B[0]=({struct Cyc_Absyn_VarType_struct _tmp27C;_tmp27C.tag=1;
_tmp27C.f1=rgn0;_tmp27C;});_tmp27B;});struct Cyc_List_List*_tmp253=0;{struct Cyc_List_List*
_tmp254=fd->args;for(0;_tmp254 != 0;_tmp254=_tmp254->tl){struct Cyc_Absyn_Vardecl*
_tmp255=({struct Cyc_Absyn_Vardecl*_tmp25B=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)
* 1);_tmp25B[0]=({struct Cyc_Absyn_Vardecl _tmp25C;_tmp25C.sc=(void*)((void*)2);
_tmp25C.name=({struct _tuple1*_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E->f1=(
void*)((void*)0);_tmp25E->f2=(*((struct _tuple8*)_tmp254->hd)).f1;_tmp25E;});
_tmp25C.tq=(*((struct _tuple8*)_tmp254->hd)).f2;_tmp25C.type=(void*)(*((struct
_tuple8*)_tmp254->hd)).f3;_tmp25C.initializer=0;_tmp25C.rgn=({struct Cyc_Core_Opt*
_tmp25D=_cycalloc(sizeof(*_tmp25D));_tmp25D->v=(void*)param_rgn;_tmp25D;});
_tmp25C.attributes=0;_tmp25C.escapes=0;_tmp25C;});_tmp25B;});_tmp253=({struct Cyc_List_List*
_tmp256=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp256[0]=({struct Cyc_List_List
_tmp257;_tmp257.hd=_tmp255;_tmp257.tl=_tmp253;_tmp257;});_tmp256;});locals=({
struct Cyc_Tcenv_Bindings*_tmp258=_region_malloc(r,sizeof(*_tmp258));_tmp258->v=(*((
struct _tuple8*)_tmp254->hd)).f1;_tmp258->b=(void*)((void*)({struct Cyc_Absyn_Param_b_struct*
_tmp259=_cycalloc(sizeof(*_tmp259));_tmp259[0]=({struct Cyc_Absyn_Param_b_struct
_tmp25A;_tmp25A.tag=2;_tmp25A.f1=_tmp255;_tmp25A;});_tmp259;}));_tmp258->tl=
locals;_tmp258;});}}if(fd->cyc_varargs != 0){struct Cyc_Core_Opt*_tmp260;struct Cyc_Absyn_Tqual
_tmp261;void*_tmp262;int _tmp263;struct Cyc_Absyn_VarargInfo _tmp25F=*((struct Cyc_Absyn_VarargInfo*)
_check_null(fd->cyc_varargs));_tmp260=_tmp25F.name;_tmp261=_tmp25F.tq;_tmp262=(
void*)_tmp25F.type;_tmp263=_tmp25F.inject;if(_tmp260 != 0){void*_tmp264=Cyc_Absyn_dynforward_typ(
_tmp262,param_rgn,_tmp261,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl*
_tmp265=({struct Cyc_Absyn_Vardecl*_tmp26A=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)
* 1);_tmp26A[0]=({struct Cyc_Absyn_Vardecl _tmp26B;_tmp26B.sc=(void*)((void*)2);
_tmp26B.name=({struct _tuple1*_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D->f1=(
void*)((void*)0);_tmp26D->f2=(struct _dynforward_ptr*)_tmp260->v;_tmp26D;});
_tmp26B.tq=Cyc_Absyn_empty_tqual(0);_tmp26B.type=(void*)_tmp264;_tmp26B.initializer=
0;_tmp26B.rgn=({struct Cyc_Core_Opt*_tmp26C=_cycalloc(sizeof(*_tmp26C));_tmp26C->v=(
void*)param_rgn;_tmp26C;});_tmp26B.attributes=0;_tmp26B.escapes=0;_tmp26B;});
_tmp26A;});_tmp253=({struct Cyc_List_List*_tmp266=_cycalloc(sizeof(*_tmp266));
_tmp266->hd=_tmp265;_tmp266->tl=_tmp253;_tmp266;});locals=({struct Cyc_Tcenv_Bindings*
_tmp267=_region_malloc(r,sizeof(*_tmp267));_tmp267->v=(struct _dynforward_ptr*)
_tmp260->v;_tmp267->b=(void*)((void*)({struct Cyc_Absyn_Param_b_struct*_tmp268=
_cycalloc(sizeof(*_tmp268));_tmp268[0]=({struct Cyc_Absyn_Param_b_struct _tmp269;
_tmp269.tag=2;_tmp269.f1=_tmp265;_tmp269;});_tmp268;}));_tmp267->tl=locals;
_tmp267;});}else{({void*_tmp26E[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp26F="missing name for varargs";
_tag_dynforward(_tmp26F,sizeof(char),_get_zero_arr_size(_tmp26F,25));}),
_tag_dynforward(_tmp26E,sizeof(void*),0));});}}fd->param_vardecls=({struct Cyc_Core_Opt*
_tmp270=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1);_tmp270[0]=({struct Cyc_Core_Opt
_tmp271;_tmp271.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp253);_tmp271;});_tmp270;});return({struct Cyc_Tcenv_Fenv*_tmp272=
_region_malloc(r,sizeof(*_tmp272));_tmp272->shared=(struct Cyc_Tcenv_SharedFenv*)({
struct Cyc_Tcenv_SharedFenv*_tmp27A=_region_malloc(r,sizeof(*_tmp27A));_tmp27A->frgn=
r;_tmp27A->return_typ=(void*)((void*)fd->ret_type);_tmp27A->seen_labels=((struct
Cyc_Dict_Dict*(*)(struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,struct
_dynforward_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp27A->needed_labels=((
struct Cyc_Dict_Dict*(*)(struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,
struct _dynforward_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp27A->delayed_effect_checks=
0;_tmp27A->delayed_constraint_checks=0;_tmp27A;});_tmp272->type_vars=(struct Cyc_List_List*)
_tmp251;_tmp272->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp252;_tmp272->locals=(
struct Cyc_Tcenv_Bindings*)locals;_tmp272->encloser=(struct Cyc_Absyn_Stmt*)fd->body;
_tmp272->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp279=
_region_malloc(r,sizeof(*_tmp279));_tmp279->ctrl_rgn=r;_tmp279->continue_stmt=(
void*)((void*)0);_tmp279->break_stmt=(void*)((void*)0);_tmp279->fallthru_clause=
0;_tmp279->next_stmt=(void*)((void*)2);_tmp279->try_depth=0;_tmp279;});_tmp272->capability=(
void*)((void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp273=_cycalloc(sizeof(*
_tmp273));_tmp273[0]=({struct Cyc_Absyn_JoinEff_struct _tmp274;_tmp274.tag=20;
_tmp274.f1=({struct Cyc_List_List*_tmp275=_cycalloc(sizeof(*_tmp275));_tmp275->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp277=_cycalloc(sizeof(*
_tmp277));_tmp277[0]=({struct Cyc_Absyn_AccessEff_struct _tmp278;_tmp278.tag=19;
_tmp278.f1=(void*)param_rgn;_tmp278;});_tmp277;}));_tmp275->tl=({struct Cyc_List_List*
_tmp276=_cycalloc(sizeof(*_tmp276));_tmp276->hd=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(fd->effect))->v);_tmp276->tl=0;_tmp276;});_tmp275;});_tmp274;});
_tmp273;})));_tmp272->curr_rgn=(void*)((void*)param_rgn);_tmp272->in_notreadctxt=(
int)0;_tmp272;});}}struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(struct
_RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Fenv*old_fenv,
struct Cyc_Absyn_Fndecl*fd){struct Cyc_Tcenv_Fenv _tmp286;struct Cyc_Tcenv_Bindings*
_tmp287;struct Cyc_RgnOrder_RgnPO*_tmp288;struct Cyc_List_List*_tmp289;struct Cyc_Tcenv_Fenv*
_tmp285=old_fenv;_tmp286=*_tmp285;_tmp287=_tmp286.locals;_tmp288=_tmp286.region_order;
_tmp289=_tmp286.type_vars;{struct Cyc_Tcenv_Bindings*_tmp28A=(struct Cyc_Tcenv_Bindings*)
_tmp287;struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp2B8=_cycalloc(
sizeof(*_tmp2B8));_tmp2B8->name=({struct _dynforward_ptr*_tmp2BB=_cycalloc(
sizeof(*_tmp2BB));_tmp2BB[0]=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp2BE;_tmp2BE.tag=0;_tmp2BE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*(*
fd->name).f2);{void*_tmp2BC[1]={& _tmp2BE};Cyc_aprintf(({const char*_tmp2BD="`%s";
_tag_dynforward(_tmp2BD,sizeof(char),_get_zero_arr_size(_tmp2BD,4));}),
_tag_dynforward(_tmp2BC,sizeof(void*),1));}});_tmp2BB;});_tmp2B8->identity=Cyc_Tcutil_new_tvar_id();
_tmp2B8->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp2B9=_cycalloc(
sizeof(*_tmp2B9));_tmp2B9[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp2BA;_tmp2BA.tag=0;
_tmp2BA.f1=(void*)((void*)3);_tmp2BA;});_tmp2B9;}));_tmp2B8;});{struct Cyc_List_List*
_tmp28B=fd->tvs;for(0;_tmp28B != 0;_tmp28B=_tmp28B->tl){if(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp28B->hd)== (void*)3)_tmp288=Cyc_RgnOrder_add_youngest(
_tmp288,(struct Cyc_Absyn_Tvar*)_tmp28B->hd,0);}}_tmp288=Cyc_RgnOrder_add_youngest(
_tmp288,rgn0,0);{struct Cyc_List_List*_tmp28C=({struct Cyc_List_List*_tmp2B7=
_cycalloc(sizeof(*_tmp2B7));_tmp2B7->hd=rgn0;_tmp2B7->tl=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp289);
_tmp2B7;});Cyc_Tcutil_check_unique_tvars(loc,_tmp28C);{void*param_rgn=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));_tmp2B5[0]=({
struct Cyc_Absyn_VarType_struct _tmp2B6;_tmp2B6.tag=1;_tmp2B6.f1=rgn0;_tmp2B6;});
_tmp2B5;});struct Cyc_List_List*_tmp28D=0;{struct Cyc_List_List*_tmp28E=fd->args;
for(0;_tmp28E != 0;_tmp28E=_tmp28E->tl){struct Cyc_Absyn_Vardecl*_tmp28F=({struct
Cyc_Absyn_Vardecl*_tmp295=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp295[
0]=({struct Cyc_Absyn_Vardecl _tmp296;_tmp296.sc=(void*)((void*)2);_tmp296.name=({
struct _tuple1*_tmp298=_cycalloc(sizeof(*_tmp298));_tmp298->f1=(void*)((void*)0);
_tmp298->f2=(*((struct _tuple8*)_tmp28E->hd)).f1;_tmp298;});_tmp296.tq=(*((struct
_tuple8*)_tmp28E->hd)).f2;_tmp296.type=(void*)(*((struct _tuple8*)_tmp28E->hd)).f3;
_tmp296.initializer=0;_tmp296.rgn=({struct Cyc_Core_Opt*_tmp297=_cycalloc(sizeof(*
_tmp297));_tmp297->v=(void*)param_rgn;_tmp297;});_tmp296.attributes=0;_tmp296.escapes=
0;_tmp296;});_tmp295;});_tmp28D=({struct Cyc_List_List*_tmp290=_cycalloc(sizeof(
struct Cyc_List_List)* 1);_tmp290[0]=({struct Cyc_List_List _tmp291;_tmp291.hd=
_tmp28F;_tmp291.tl=_tmp28D;_tmp291;});_tmp290;});_tmp28A=({struct Cyc_Tcenv_Bindings*
_tmp292=_region_malloc(r,sizeof(*_tmp292));_tmp292->v=(*((struct _tuple8*)_tmp28E->hd)).f1;
_tmp292->b=(void*)((void*)({struct Cyc_Absyn_Param_b_struct*_tmp293=_cycalloc(
sizeof(*_tmp293));_tmp293[0]=({struct Cyc_Absyn_Param_b_struct _tmp294;_tmp294.tag=
2;_tmp294.f1=_tmp28F;_tmp294;});_tmp293;}));_tmp292->tl=_tmp28A;_tmp292;});}}if(
fd->cyc_varargs != 0){struct Cyc_Core_Opt*_tmp29A;struct Cyc_Absyn_Tqual _tmp29B;
void*_tmp29C;int _tmp29D;struct Cyc_Absyn_VarargInfo _tmp299=*((struct Cyc_Absyn_VarargInfo*)
_check_null(fd->cyc_varargs));_tmp29A=_tmp299.name;_tmp29B=_tmp299.tq;_tmp29C=(
void*)_tmp299.type;_tmp29D=_tmp299.inject;if(_tmp29A != 0){void*_tmp29E=Cyc_Absyn_dynforward_typ(
_tmp29C,param_rgn,_tmp29B,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl*
_tmp29F=({struct Cyc_Absyn_Vardecl*_tmp2A4=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)
* 1);_tmp2A4[0]=({struct Cyc_Absyn_Vardecl _tmp2A5;_tmp2A5.sc=(void*)((void*)2);
_tmp2A5.name=({struct _tuple1*_tmp2A7=_cycalloc(sizeof(*_tmp2A7));_tmp2A7->f1=(
void*)((void*)0);_tmp2A7->f2=(struct _dynforward_ptr*)_tmp29A->v;_tmp2A7;});
_tmp2A5.tq=Cyc_Absyn_empty_tqual(0);_tmp2A5.type=(void*)_tmp29E;_tmp2A5.initializer=
0;_tmp2A5.rgn=({struct Cyc_Core_Opt*_tmp2A6=_cycalloc(sizeof(*_tmp2A6));_tmp2A6->v=(
void*)param_rgn;_tmp2A6;});_tmp2A5.attributes=0;_tmp2A5.escapes=0;_tmp2A5;});
_tmp2A4;});_tmp28D=({struct Cyc_List_List*_tmp2A0=_cycalloc(sizeof(*_tmp2A0));
_tmp2A0->hd=_tmp29F;_tmp2A0->tl=_tmp28D;_tmp2A0;});_tmp28A=({struct Cyc_Tcenv_Bindings*
_tmp2A1=_region_malloc(r,sizeof(*_tmp2A1));_tmp2A1->v=(struct _dynforward_ptr*)
_tmp29A->v;_tmp2A1->b=(void*)((void*)({struct Cyc_Absyn_Param_b_struct*_tmp2A2=
_cycalloc(sizeof(*_tmp2A2));_tmp2A2[0]=({struct Cyc_Absyn_Param_b_struct _tmp2A3;
_tmp2A3.tag=2;_tmp2A3.f1=_tmp29F;_tmp2A3;});_tmp2A2;}));_tmp2A1->tl=_tmp28A;
_tmp2A1;});}else{({void*_tmp2A8[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp2A9="missing name for varargs";
_tag_dynforward(_tmp2A9,sizeof(char),_get_zero_arr_size(_tmp2A9,25));}),
_tag_dynforward(_tmp2A8,sizeof(void*),0));});}}fd->param_vardecls=({struct Cyc_Core_Opt*
_tmp2AA=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1);_tmp2AA[0]=({struct Cyc_Core_Opt
_tmp2AB;_tmp2AB.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp28D);_tmp2AB;});_tmp2AA;});return({struct Cyc_Tcenv_Fenv*_tmp2AC=
_region_malloc(r,sizeof(*_tmp2AC));_tmp2AC->shared=(struct Cyc_Tcenv_SharedFenv*)({
struct Cyc_Tcenv_SharedFenv*_tmp2B4=_region_malloc(r,sizeof(*_tmp2B4));_tmp2B4->frgn=
r;_tmp2B4->return_typ=(void*)((void*)fd->ret_type);_tmp2B4->seen_labels=((struct
Cyc_Dict_Dict*(*)(struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,struct
_dynforward_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2B4->needed_labels=((
struct Cyc_Dict_Dict*(*)(struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,
struct _dynforward_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2B4->delayed_effect_checks=
0;_tmp2B4->delayed_constraint_checks=0;_tmp2B4;});_tmp2AC->type_vars=(struct Cyc_List_List*)
_tmp28C;_tmp2AC->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp288;_tmp2AC->locals=(
struct Cyc_Tcenv_Bindings*)_tmp28A;_tmp2AC->encloser=(struct Cyc_Absyn_Stmt*)fd->body;
_tmp2AC->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp2B3=
_region_malloc(r,sizeof(*_tmp2B3));_tmp2B3->ctrl_rgn=r;_tmp2B3->continue_stmt=(
void*)((void*)0);_tmp2B3->break_stmt=(void*)((void*)0);_tmp2B3->fallthru_clause=
0;_tmp2B3->next_stmt=(void*)((void*)2);_tmp2B3->try_depth=0;_tmp2B3;});_tmp2AC->capability=(
void*)((void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp2AD=_cycalloc(sizeof(*
_tmp2AD));_tmp2AD[0]=({struct Cyc_Absyn_JoinEff_struct _tmp2AE;_tmp2AE.tag=20;
_tmp2AE.f1=({struct Cyc_List_List*_tmp2AF=_cycalloc(sizeof(*_tmp2AF));_tmp2AF->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp2B1=_cycalloc(sizeof(*
_tmp2B1));_tmp2B1[0]=({struct Cyc_Absyn_AccessEff_struct _tmp2B2;_tmp2B2.tag=19;
_tmp2B2.f1=(void*)param_rgn;_tmp2B2;});_tmp2B1;}));_tmp2AF->tl=({struct Cyc_List_List*
_tmp2B0=_cycalloc(sizeof(*_tmp2B0));_tmp2B0->hd=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(fd->effect))->v);_tmp2B0->tl=0;_tmp2B0;});_tmp2AF;});_tmp2AE;});
_tmp2AD;})));_tmp2AC->curr_rgn=(void*)((void*)param_rgn);_tmp2AC->in_notreadctxt=(
int)0;_tmp2AC;});}}}}
