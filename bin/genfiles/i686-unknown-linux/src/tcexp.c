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
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*
env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];void*Cyc_List_nth(struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_nth_tail(
struct Cyc_List_List*x,int i);int Cyc_List_exists_c(int(*pred)(void*,void*),void*
env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*
r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{
struct _dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dynforward_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{
int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct
Loc_n;struct Cyc_Absyn_Rel_n_struct Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};
struct _tuple1{union Cyc_Absyn_Nmspace_union f1;struct _dynforward_ptr*f2;};struct
Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int
q_restrict;int real_const;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Conref{
void*v;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);
struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;
extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(void*t,void*rgn,struct
Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_dynforward_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);void*Cyc_Absyn_pointer_expand(
void*,int fresh_evar);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dynforward_ptr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dynforward_ptr*);
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);int Cyc_strcmp(
struct _dynforward_ptr s1,struct _dynforward_ptr s2);typedef struct{int __count;union{
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
tag;struct _dynforward_ptr f1;};struct _dynforward_ptr Cyc_vrprintf(struct
_RegionHandle*,struct _dynforward_ptr,struct _dynforward_ptr);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dynforward_ptr f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct
_dynforward_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*
env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
le;};void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te);int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);struct Cyc_Absyn_Stmt*
Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block(struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);int Cyc_Tcutil_kind_leq(void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(
void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*,void*);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);int
Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int
Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt
Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();
int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,
void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,
void*);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple4{struct
Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple5{struct Cyc_Absyn_Tvar*f1;
void*f2;};struct _tuple5*Cyc_Tcutil_r_make_inst_var(struct _tuple4*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*
bound_tvars,void*k,int allow_evars,void*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,struct Cyc_Absyn_Conref*);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*,int*
is_forward_only);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest,int*is_forward_only);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,
void**elt_typ_dest);struct _tuple6{int f1;void*f2;};struct _tuple6 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_bits_only(void*t);
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);struct
_tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(struct
Cyc_Absyn_Exp*e);int Cyc_Evexp_okay_szofarg(void*t);void Cyc_Tcstmt_tcStmt(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(
struct Cyc_Tcenv_Tenv*,struct _dynforward_ptr,struct Cyc_Position_Segment*);struct
Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct Cyc_Tcenv_Tenv*,struct
_dynforward_ptr,struct Cyc_Position_Segment*);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e,struct _dynforward_ptr msg_part);struct Cyc_CfFlowInfo_VarRoot_struct{
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
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct Cyc_Dict_Dict*f1;};struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict*consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict*f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _dynforward_ptr msg,struct _dynforward_ptr ap){({void*_tmp0[0]={};
Cyc_Tcutil_terr(loc,(struct _dynforward_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,
ap),_tag_dynforward(_tmp0,sizeof(void*),0));});if(topt == 0)return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp1;}));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct _handler_cons
_tmp2;_push_handler(& _tmp2);{int _tmp4=0;if(setjmp(_tmp2.handler))_tmp4=1;if(!
_tmp4){{struct _RegionHandle _tmp5=_new_region("r");struct _RegionHandle*r=& _tmp5;
_push_region(r);{void*_tmp6=Cyc_Tcenv_lookup_ordinary(r,te,e->loc,q);void*_tmp7;
struct Cyc_Absyn_Enumdecl*_tmp8;struct Cyc_Absyn_Enumfield*_tmp9;void*_tmpA;struct
Cyc_Absyn_Enumfield*_tmpB;struct Cyc_Absyn_Tuniondecl*_tmpC;struct Cyc_Absyn_Tunionfield*
_tmpD;_LL1: if(*((int*)_tmp6)!= 0)goto _LL3;_tmp7=(void*)((struct Cyc_Tcenv_VarRes_struct*)
_tmp6)->f1;_LL2:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmpE=
_cycalloc(sizeof(*_tmpE));_tmpE[0]=({struct Cyc_Absyn_Var_e_struct _tmpF;_tmpF.tag=
1;_tmpF.f1=q;_tmpF.f2=(void*)_tmp7;_tmpF;});_tmpE;})));goto _LL0;_LL3: if(*((int*)
_tmp6)!= 3)goto _LL5;_tmp8=((struct Cyc_Tcenv_EnumRes_struct*)_tmp6)->f1;_tmp9=((
struct Cyc_Tcenv_EnumRes_struct*)_tmp6)->f2;_LL4:(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Enum_e_struct*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10[0]=({
struct Cyc_Absyn_Enum_e_struct _tmp11;_tmp11.tag=33;_tmp11.f1=q;_tmp11.f2=(struct
Cyc_Absyn_Enumdecl*)_tmp8;_tmp11.f3=(struct Cyc_Absyn_Enumfield*)_tmp9;_tmp11;});
_tmp10;})));goto _LL0;_LL5: if(*((int*)_tmp6)!= 4)goto _LL7;_tmpA=(void*)((struct
Cyc_Tcenv_AnonEnumRes_struct*)_tmp6)->f1;_tmpB=((struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp6)->f2;_LL6:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_AnonEnum_e_struct*
_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12[0]=({struct Cyc_Absyn_AnonEnum_e_struct
_tmp13;_tmp13.tag=34;_tmp13.f1=q;_tmp13.f2=(void*)_tmpA;_tmp13.f3=(struct Cyc_Absyn_Enumfield*)
_tmpB;_tmp13;});_tmp12;})));goto _LL0;_LL7: if(*((int*)_tmp6)!= 2)goto _LL9;_tmpC=((
struct Cyc_Tcenv_TunionRes_struct*)_tmp6)->f1;_tmpD=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp6)->f2;_LL8:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Tunion_e_struct*
_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14[0]=({struct Cyc_Absyn_Tunion_e_struct
_tmp15;_tmp15.tag=32;_tmp15.f1=0;_tmp15.f2=_tmpC;_tmp15.f3=_tmpD;_tmp15;});
_tmp14;})));goto _LL0;_LL9: if(*((int*)_tmp6)!= 1)goto _LL0;_LLA:({struct Cyc_String_pa_struct
_tmp18;_tmp18.tag=0;_tmp18.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
q));{void*_tmp16[1]={& _tmp18};Cyc_Tcutil_terr(e->loc,({const char*_tmp17="bad occurrence of type name %s";
_tag_dynforward(_tmp17,sizeof(char),_get_zero_arr_size(_tmp17,31));}),
_tag_dynforward(_tmp16,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Var_e_struct*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19[0]=({
struct Cyc_Absyn_Var_e_struct _tmp1A;_tmp1A.tag=1;_tmp1A.f1=q;_tmp1A.f2=(void*)((
void*)0);_tmp1A;});_tmp19;})));goto _LL0;_LL0:;};_pop_region(r);};_pop_handler();}
else{void*_tmp3=(void*)_exn_thrown;void*_tmp1C=_tmp3;_LLC: if(_tmp1C != Cyc_Dict_Absent)
goto _LLE;_LLD:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmp1D=
_cycalloc(sizeof(*_tmp1D));_tmp1D[0]=({struct Cyc_Absyn_Var_e_struct _tmp1E;_tmp1E.tag=
1;_tmp1E.f1=q;_tmp1E.f2=(void*)((void*)0);_tmp1E;});_tmp1D;})));goto _LLB;_LLE:;
_LLF:(void)_throw(_tmp1C);_LLB:;}}}struct _tuple8{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple8*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*
e){return({struct _tuple8*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->f1=0;_tmp1F->f2=
e;_tmp1F;});}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*_tmp20=(
void*)e1->r;struct _tuple1*_tmp21;_LL11: if(*((int*)_tmp20)!= 2)goto _LL13;_tmp21=((
struct Cyc_Absyn_UnknownId_e_struct*)_tmp20)->f1;_LL12: {struct _handler_cons
_tmp22;_push_handler(& _tmp22);{int _tmp24=0;if(setjmp(_tmp22.handler))_tmp24=1;
if(!_tmp24){{struct _RegionHandle _tmp25=_new_region("r");struct _RegionHandle*r=&
_tmp25;_push_region(r);{void*_tmp26=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,
_tmp21);void*_tmp27;struct Cyc_Absyn_Tuniondecl*_tmp28;struct Cyc_Absyn_Tunionfield*
_tmp29;struct Cyc_Absyn_Aggrdecl*_tmp2A;_LL16: if(*((int*)_tmp26)!= 0)goto _LL18;
_tmp27=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp26)->f1;_LL17:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));
_tmp2B[0]=({struct Cyc_Absyn_FnCall_e_struct _tmp2C;_tmp2C.tag=11;_tmp2C.f1=e1;
_tmp2C.f2=es;_tmp2C.f3=0;_tmp2C;});_tmp2B;})));_npop_handler(1);return;_LL18: if(*((
int*)_tmp26)!= 2)goto _LL1A;_tmp28=((struct Cyc_Tcenv_TunionRes_struct*)_tmp26)->f1;
_tmp29=((struct Cyc_Tcenv_TunionRes_struct*)_tmp26)->f2;_LL19: if(_tmp29->typs == 0)({
struct Cyc_String_pa_struct _tmp2F;_tmp2F.tag=0;_tmp2F.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp29->name));{void*_tmp2D[1]={&
_tmp2F};Cyc_Tcutil_terr(e->loc,({const char*_tmp2E="%s is a constant, not a function";
_tag_dynforward(_tmp2E,sizeof(char),_get_zero_arr_size(_tmp2E,33));}),
_tag_dynforward(_tmp2D,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Tunion_e_struct*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30[0]=({
struct Cyc_Absyn_Tunion_e_struct _tmp31;_tmp31.tag=32;_tmp31.f1=es;_tmp31.f2=
_tmp28;_tmp31.f3=_tmp29;_tmp31;});_tmp30;})));_npop_handler(1);return;_LL1A: if(*((
int*)_tmp26)!= 1)goto _LL1C;_tmp2A=((struct Cyc_Tcenv_AggrRes_struct*)_tmp26)->f1;
_LL1B: {struct Cyc_List_List*_tmp32=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,
es);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Struct_e_struct*_tmp33=
_cycalloc(sizeof(*_tmp33));_tmp33[0]=({struct Cyc_Absyn_Struct_e_struct _tmp34;
_tmp34.tag=30;_tmp34.f1=_tmp2A->name;_tmp34.f2=0;_tmp34.f3=_tmp32;_tmp34.f4=(
struct Cyc_Absyn_Aggrdecl*)_tmp2A;_tmp34;});_tmp33;})));_npop_handler(1);return;}
_LL1C: if(*((int*)_tmp26)!= 4)goto _LL1E;_LL1D: goto _LL1F;_LL1E: if(*((int*)_tmp26)
!= 3)goto _LL15;_LL1F:({struct Cyc_String_pa_struct _tmp37;_tmp37.tag=0;_tmp37.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp21));{
void*_tmp35[1]={& _tmp37};Cyc_Tcutil_terr(e->loc,({const char*_tmp36="%s is an enum constructor, not a function";
_tag_dynforward(_tmp36,sizeof(char),_get_zero_arr_size(_tmp36,42));}),
_tag_dynforward(_tmp35,sizeof(void*),1));}});_npop_handler(1);return;_LL15:;};
_pop_region(r);};_pop_handler();}else{void*_tmp23=(void*)_exn_thrown;void*_tmp39=
_tmp23;_LL21: if(_tmp39 != Cyc_Dict_Absent)goto _LL23;_LL22:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A[
0]=({struct Cyc_Absyn_FnCall_e_struct _tmp3B;_tmp3B.tag=11;_tmp3B.f1=e1;_tmp3B.f2=
es;_tmp3B.f3=0;_tmp3B;});_tmp3A;})));return;_LL23:;_LL24:(void)_throw(_tmp39);
_LL20:;}}}_LL13:;_LL14:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp3D;_tmp3D.tag=11;_tmp3D.f1=e1;_tmp3D.f2=es;_tmp3D.f3=0;_tmp3D;});_tmp3C;})));
return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
des){if(topt == 0){(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E[0]=({struct Cyc_Absyn_Array_e_struct
_tmp3F;_tmp3F.tag=28;_tmp3F.f1=des;_tmp3F;});_tmp3E;})));return;}{void*t=*topt;
void*_tmp40=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp41;void*_tmp42;
struct Cyc_Absyn_ArrayInfo _tmp43;void*_tmp44;struct Cyc_Absyn_Tqual _tmp45;_LL26:
if(_tmp40 <= (void*)4)goto _LL2C;if(*((int*)_tmp40)!= 10)goto _LL28;_tmp41=((struct
Cyc_Absyn_AggrType_struct*)_tmp40)->f1;_tmp42=(void*)_tmp41.aggr_info;_LL27:{
void*_tmp46=_tmp42;struct Cyc_Absyn_Aggrdecl**_tmp47;struct Cyc_Absyn_Aggrdecl*
_tmp48;_LL2F: if(*((int*)_tmp46)!= 0)goto _LL31;_LL30:({void*_tmp49[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp4A="struct type not properly set";_tag_dynforward(_tmp4A,sizeof(char),
_get_zero_arr_size(_tmp4A,29));}),_tag_dynforward(_tmp49,sizeof(void*),0));});
_LL31: if(*((int*)_tmp46)!= 1)goto _LL2E;_tmp47=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp46)->f1;_tmp48=*_tmp47;_LL32:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp4C;_tmp4C.tag=30;_tmp4C.f1=_tmp48->name;_tmp4C.f2=0;_tmp4C.f3=des;_tmp4C.f4=(
struct Cyc_Absyn_Aggrdecl*)_tmp48;_tmp4C;});_tmp4B;})));_LL2E:;}goto _LL25;_LL28:
if(*((int*)_tmp40)!= 7)goto _LL2A;_tmp43=((struct Cyc_Absyn_ArrayType_struct*)
_tmp40)->f1;_tmp44=(void*)_tmp43.elt_type;_tmp45=_tmp43.tq;_LL29:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));
_tmp4D[0]=({struct Cyc_Absyn_Array_e_struct _tmp4E;_tmp4E.tag=28;_tmp4E.f1=des;
_tmp4E;});_tmp4D;})));goto _LL25;_LL2A: if(*((int*)_tmp40)!= 11)goto _LL2C;_LL2B:(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_AnonStruct_e_struct*_tmp4F=
_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_Absyn_AnonStruct_e_struct _tmp50;
_tmp50.tag=31;_tmp50.f1=(void*)t;_tmp50.f2=des;_tmp50;});_tmp4F;})));goto _LL25;
_LL2C:;_LL2D:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp51=
_cycalloc(sizeof(*_tmp51));_tmp51[0]=({struct Cyc_Absyn_Array_e_struct _tmp52;
_tmp52.tag=28;_tmp52.f1=des;_tmp52;});_tmp51;})));goto _LL25;_LL25:;}}static void
Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e);static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_dynforward_ptr msg_part){Cyc_Tcutil_check_contains_assign(e);Cyc_Tcexp_tcExp(te,(
void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_String_pa_struct
_tmp56;_tmp56.tag=0;_tmp56.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct
_tmp55;_tmp55.tag=0;_tmp55.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
msg_part);{void*_tmp53[2]={& _tmp55,& _tmp56};Cyc_Tcutil_terr(e->loc,({const char*
_tmp54="test of %s has type %s instead of integral or * type";_tag_dynforward(
_tmp54,sizeof(char),_get_zero_arr_size(_tmp54,53));}),_tag_dynforward(_tmp53,
sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,union Cyc_Absyn_Cnst_union c,struct Cyc_Absyn_Exp*
e){void*t;{union Cyc_Absyn_Cnst_union _tmp57=c;void*_tmp58;void*_tmp59;void*_tmp5A;
void*_tmp5B;void*_tmp5C;void*_tmp5D;int _tmp5E;struct _dynforward_ptr _tmp5F;_LL34:
if((_tmp57.Char_c).tag != 0)goto _LL36;_tmp58=(_tmp57.Char_c).f1;if((int)_tmp58 != 
0)goto _LL36;_LL35: t=Cyc_Absyn_schar_typ;goto _LL33;_LL36: if((_tmp57.Char_c).tag != 
0)goto _LL38;_tmp59=(_tmp57.Char_c).f1;if((int)_tmp59 != 1)goto _LL38;_LL37: t=Cyc_Absyn_uchar_typ;
goto _LL33;_LL38: if((_tmp57.Char_c).tag != 0)goto _LL3A;_tmp5A=(_tmp57.Char_c).f1;
if((int)_tmp5A != 2)goto _LL3A;_LL39: t=Cyc_Absyn_char_typ;goto _LL33;_LL3A: if((
_tmp57.Short_c).tag != 1)goto _LL3C;_tmp5B=(_tmp57.Short_c).f1;_LL3B: t=_tmp5B == (
void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL33;_LL3C: if((_tmp57.LongLong_c).tag
!= 3)goto _LL3E;_tmp5C=(_tmp57.LongLong_c).f1;_LL3D: t=_tmp5C == (void*)1?Cyc_Absyn_ulonglong_typ:
Cyc_Absyn_slonglong_typ;goto _LL33;_LL3E: if((_tmp57.Float_c).tag != 4)goto _LL40;
_LL3F: t=Cyc_Absyn_float_typ;goto _LL33;_LL40: if((_tmp57.Int_c).tag != 2)goto _LL42;
_tmp5D=(_tmp57.Int_c).f1;_tmp5E=(_tmp57.Int_c).f2;_LL41: if(topt == 0)t=_tmp5D == (
void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{void*_tmp60=Cyc_Tcutil_compress(*
topt);void*_tmp61;void*_tmp62;void*_tmp63;void*_tmp64;void*_tmp65;void*_tmp66;
void*_tmp67;void*_tmp68;void*_tmp69;_LL47: if(_tmp60 <= (void*)4)goto _LL53;if(*((
int*)_tmp60)!= 5)goto _LL49;_tmp61=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp60)->f1;_tmp62=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp60)->f2;if((int)
_tmp62 != 0)goto _LL49;_LL48:{void*_tmp6A=_tmp61;_LL56: if((int)_tmp6A != 1)goto
_LL58;_LL57: t=Cyc_Absyn_uchar_typ;goto _LL55;_LL58: if((int)_tmp6A != 0)goto _LL5A;
_LL59: t=Cyc_Absyn_schar_typ;goto _LL55;_LL5A: if((int)_tmp6A != 2)goto _LL55;_LL5B: t=
Cyc_Absyn_char_typ;goto _LL55;_LL55:;}(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B[0]=({struct Cyc_Absyn_Const_e_struct
_tmp6C;_tmp6C.tag=0;_tmp6C.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp6D;(_tmp6D.Char_c).tag=0;(_tmp6D.Char_c).f1=(void*)_tmp61;(_tmp6D.Char_c).f2=(
char)_tmp5E;_tmp6D;});_tmp6C;});_tmp6B;})));goto _LL46;_LL49: if(*((int*)_tmp60)!= 
5)goto _LL4B;_tmp63=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp60)->f1;_tmp64=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp60)->f2;if((int)_tmp64 != 1)goto _LL4B;
_LL4A: t=_tmp63 == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));
_tmp6E[0]=({struct Cyc_Absyn_Const_e_struct _tmp6F;_tmp6F.tag=0;_tmp6F.f1=(union
Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp70;(_tmp70.Short_c).tag=1;(
_tmp70.Short_c).f1=(void*)_tmp63;(_tmp70.Short_c).f2=(short)_tmp5E;_tmp70;});
_tmp6F;});_tmp6E;})));goto _LL46;_LL4B: if(*((int*)_tmp60)!= 5)goto _LL4D;_tmp65=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp60)->f1;_tmp66=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp60)->f2;if((int)_tmp66 != 2)goto _LL4D;_LL4C: t=_tmp65 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL4D: if(*((int*)_tmp60)!= 5)goto _LL4F;_tmp67=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp60)->f1;_tmp68=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp60)->f2;if((int)_tmp68 != 3)goto _LL4F;_LL4E: t=_tmp67 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL4F: if(*((int*)_tmp60)!= 4)goto _LL51;if(!(_tmp5E
== 0))goto _LL51;_LL50:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71[0]=({struct Cyc_Absyn_Const_e_struct
_tmp72;_tmp72.tag=0;_tmp72.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp73;(_tmp73.Null_c).tag=6;_tmp73;});_tmp72;});_tmp71;})));{struct Cyc_List_List*
_tmp74=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75[0]=({struct Cyc_Absyn_PointerType_struct
_tmp76;_tmp76.tag=4;_tmp76.f1=({struct Cyc_Absyn_PtrInfo _tmp77;_tmp77.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->v=_tmp74;_tmp7A;}));_tmp77.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmp77.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp78;_tmp78.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp79=_cycalloc(sizeof(*
_tmp79));_tmp79->v=_tmp74;_tmp79;}));_tmp78.nullable=Cyc_Absyn_true_conref;
_tmp78.bounds=Cyc_Absyn_empty_conref();_tmp78.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp78.ptrloc=0;_tmp78;});_tmp77;});_tmp76;});_tmp75;});
goto _LL46;}_LL51: if(*((int*)_tmp60)!= 17)goto _LL53;_tmp69=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp60)->f1;_LL52: {struct Cyc_Absyn_TypeInt_struct*_tmp7B=({struct Cyc_Absyn_TypeInt_struct*
_tmp82=_cycalloc_atomic(sizeof(*_tmp82));_tmp82[0]=({struct Cyc_Absyn_TypeInt_struct
_tmp83;_tmp83.tag=18;_tmp83.f1=_tmp5E;_tmp83;});_tmp82;});if(!Cyc_Tcutil_unify(
_tmp69,(void*)_tmp7B)){({struct Cyc_String_pa_struct _tmp7F;_tmp7F.tag=0;_tmp7F.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)
_tmp7B));{struct Cyc_String_pa_struct _tmp7E;_tmp7E.tag=0;_tmp7E.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp69));{void*
_tmp7C[2]={& _tmp7E,& _tmp7F};Cyc_Tcutil_terr(loc,({const char*_tmp7D="expecting %s but found %s";
_tag_dynforward(_tmp7D,sizeof(char),_get_zero_arr_size(_tmp7D,26));}),
_tag_dynforward(_tmp7C,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}t=(
void*)({struct Cyc_Absyn_TagType_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80[
0]=({struct Cyc_Absyn_TagType_struct _tmp81;_tmp81.tag=17;_tmp81.f1=(void*)((void*)
_tmp7B);_tmp81;});_tmp80;});goto _LL46;}_LL53:;_LL54: t=_tmp5D == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL46:;}goto _LL33;_LL42: if((_tmp57.String_c).tag != 
5)goto _LL44;_tmp5F=(_tmp57.String_c).f1;_LL43: {int len=(int)_get_dynforward_size(
_tmp5F,sizeof(char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmp8F;(_tmp8F.Int_c).tag=2;(_tmp8F.Int_c).f1=(void*)((
void*)1);(_tmp8F.Int_c).f2=len;_tmp8F;}),loc);elen->topt=({struct Cyc_Core_Opt*
_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->v=(void*)Cyc_Absyn_uint_typ;_tmp84;});
t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(0),(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp86;_tmp86.tag=0;_tmp86.f1=elen;_tmp86;});
_tmp85;}),Cyc_Absyn_true_conref);if(topt != 0){void*_tmp87=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_ArrayInfo _tmp88;struct Cyc_Absyn_Tqual _tmp89;_LL5D: if(
_tmp87 <= (void*)4)goto _LL61;if(*((int*)_tmp87)!= 7)goto _LL5F;_tmp88=((struct Cyc_Absyn_ArrayType_struct*)
_tmp87)->f1;_tmp89=_tmp88.tq;_LL5E: return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,
_tmp89,(struct Cyc_Absyn_Exp*)elen,((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),
0);_LL5F: if(*((int*)_tmp87)!= 4)goto _LL61;_LL60: if(!Cyc_Tcutil_unify(*topt,t)
 && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*
_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->v=(void*)t;_tmp8A;});Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);t=*topt;}else{t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp8B=_cycalloc(sizeof(*
_tmp8B));_tmp8B->v=Cyc_Tcenv_lookup_type_vars(te);_tmp8B;})),Cyc_Absyn_const_tqual(
0),(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));
_tmp8C[0]=({struct Cyc_Absyn_Upper_b_struct _tmp8D;_tmp8D.tag=0;_tmp8D.f1=elen;
_tmp8D;});_tmp8C;}),Cyc_Absyn_true_conref);if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*_tmp8E=_cycalloc(sizeof(*_tmp8E));
_tmp8E->v=(void*)t;_tmp8E;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);t=*
topt;}}goto _LL5C;_LL61:;_LL62: goto _LL5C;_LL5C:;}return t;}_LL44: if((_tmp57.Null_c).tag
!= 6)goto _LL33;_LL45: {struct Cyc_List_List*_tmp90=Cyc_Tcenv_lookup_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp91=_cycalloc(sizeof(*_tmp91));
_tmp91[0]=({struct Cyc_Absyn_PointerType_struct _tmp92;_tmp92.tag=4;_tmp92.f1=({
struct Cyc_Absyn_PtrInfo _tmp93;_tmp93.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp96=_cycalloc(sizeof(*
_tmp96));_tmp96->v=_tmp90;_tmp96;}));_tmp93.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp93.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp94;_tmp94.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp95=_cycalloc(sizeof(*
_tmp95));_tmp95->v=_tmp90;_tmp95;}));_tmp94.nullable=Cyc_Absyn_true_conref;
_tmp94.bounds=Cyc_Absyn_empty_conref();_tmp94.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp94.ptrloc=0;_tmp94;});_tmp93;});_tmp92;});_tmp91;});
goto _LL33;}_LL33:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q,void*b){void*_tmp97=b;struct Cyc_Absyn_Vardecl*
_tmp98;struct Cyc_Absyn_Fndecl*_tmp99;struct Cyc_Absyn_Vardecl*_tmp9A;struct Cyc_Absyn_Vardecl*
_tmp9B;struct Cyc_Absyn_Vardecl*_tmp9C;_LL64: if((int)_tmp97 != 0)goto _LL66;_LL65:
return({struct Cyc_String_pa_struct _tmp9F;_tmp9F.tag=0;_tmp9F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(q));{void*_tmp9D[
1]={& _tmp9F};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp9E="undeclared identifier: %s";
_tag_dynforward(_tmp9E,sizeof(char),_get_zero_arr_size(_tmp9E,26));}),
_tag_dynforward(_tmp9D,sizeof(void*),1));}});_LL66: if(_tmp97 <= (void*)1)goto
_LL68;if(*((int*)_tmp97)!= 0)goto _LL68;_tmp98=((struct Cyc_Absyn_Global_b_struct*)
_tmp97)->f1;_LL67:*q=*_tmp98->name;return(void*)_tmp98->type;_LL68: if(_tmp97 <= (
void*)1)goto _LL6A;if(*((int*)_tmp97)!= 1)goto _LL6A;_tmp99=((struct Cyc_Absyn_Funname_b_struct*)
_tmp97)->f1;_LL69:*q=*_tmp99->name;return Cyc_Tcutil_fndecl2typ(_tmp99);_LL6A: if(
_tmp97 <= (void*)1)goto _LL6C;if(*((int*)_tmp97)!= 4)goto _LL6C;_tmp9A=((struct Cyc_Absyn_Pat_b_struct*)
_tmp97)->f1;_LL6B: _tmp9B=_tmp9A;goto _LL6D;_LL6C: if(_tmp97 <= (void*)1)goto _LL6E;
if(*((int*)_tmp97)!= 3)goto _LL6E;_tmp9B=((struct Cyc_Absyn_Local_b_struct*)_tmp97)->f1;
_LL6D: _tmp9C=_tmp9B;goto _LL6F;_LL6E: if(_tmp97 <= (void*)1)goto _LL63;if(*((int*)
_tmp97)!= 2)goto _LL63;_tmp9C=((struct Cyc_Absyn_Param_b_struct*)_tmp97)->f1;_LL6F:(*
q).f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmpA0;(_tmpA0.Loc_n).tag=
0;_tmpA0;});return(void*)_tmp9C->type;_LL63:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,
struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _dynforward_ptr,
struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{void*_tmpA1=(void*)
fmt->r;union Cyc_Absyn_Cnst_union _tmpA2;struct _dynforward_ptr _tmpA3;struct Cyc_Absyn_Exp*
_tmpA4;struct Cyc_Absyn_Exp _tmpA5;void*_tmpA6;union Cyc_Absyn_Cnst_union _tmpA7;
struct _dynforward_ptr _tmpA8;_LL71: if(*((int*)_tmpA1)!= 0)goto _LL73;_tmpA2=((
struct Cyc_Absyn_Const_e_struct*)_tmpA1)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpA1)->f1).String_c).tag != 5)goto _LL73;_tmpA3=(_tmpA2.String_c).f1;_LL72:
_tmpA8=_tmpA3;goto _LL74;_LL73: if(*((int*)_tmpA1)!= 15)goto _LL75;_tmpA4=((struct
Cyc_Absyn_Cast_e_struct*)_tmpA1)->f2;_tmpA5=*_tmpA4;_tmpA6=(void*)_tmpA5.r;if(*((
int*)_tmpA6)!= 0)goto _LL75;_tmpA7=((struct Cyc_Absyn_Const_e_struct*)_tmpA6)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmpA6)->f1).String_c).tag != 5)goto _LL75;
_tmpA8=(_tmpA7.String_c).f1;_LL74: desc_types=type_getter(te,(struct
_dynforward_ptr)_tmpA8,fmt->loc);goto _LL70;_LL75:;_LL76: if(opt_args != 0){struct
Cyc_List_List*_tmpA9=(struct Cyc_List_List*)opt_args->v;for(0;_tmpA9 != 0;_tmpA9=
_tmpA9->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmpA9->hd);}}return;
_LL70:;}if(opt_args != 0){struct Cyc_List_List*_tmpAA=(struct Cyc_List_List*)
opt_args->v;for(0;desc_types != 0  && _tmpAA != 0;(desc_types=desc_types->tl,_tmpAA=
_tmpAA->tl)){void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
_tmpAA->hd;Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({
struct Cyc_String_pa_struct _tmpAE;_tmpAE.tag=0;_tmpAE.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v));{struct Cyc_String_pa_struct _tmpAD;_tmpAD.tag=0;_tmpAD.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*
_tmpAB[2]={& _tmpAD,& _tmpAE};Cyc_Tcutil_terr(e->loc,({const char*_tmpAC="descriptor has type \n%s\n but argument has type \n%s";
_tag_dynforward(_tmpAC,sizeof(char),_get_zero_arr_size(_tmpAC,51));}),
_tag_dynforward(_tmpAB,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}if(
desc_types != 0)({void*_tmpAF[0]={};Cyc_Tcutil_terr(fmt->loc,({const char*_tmpB0="too few arguments";
_tag_dynforward(_tmpB0,sizeof(char),_get_zero_arr_size(_tmpB0,18));}),
_tag_dynforward(_tmpAF,sizeof(void*),0));});if(_tmpAA != 0)({void*_tmpB1[0]={};
Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpAA->hd)->loc,({const char*_tmpB2="too many arguments";
_tag_dynforward(_tmpB2,sizeof(char),_get_zero_arr_size(_tmpB2,19));}),
_tag_dynforward(_tmpB1,sizeof(void*),0));});}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmpB3=p;_LL78: if((int)_tmpB3 != 0)goto _LL7A;_LL79:
goto _LL7B;_LL7A: if((int)_tmpB3 != 2)goto _LL7C;_LL7B: if(!Cyc_Tcutil_is_numeric(e))({
struct Cyc_String_pa_struct _tmpB6;_tmpB6.tag=0;_tmpB6.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpB4[1]={& _tmpB6};Cyc_Tcutil_terr(
loc,({const char*_tmpB5="expecting arithmetic type but found %s";_tag_dynforward(
_tmpB5,sizeof(char),_get_zero_arr_size(_tmpB5,39));}),_tag_dynforward(_tmpB4,
sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
_LL7C: if((int)_tmpB3 != 11)goto _LL7E;_LL7D: Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_String_pa_struct _tmpB9;_tmpB9.tag=
0;_tmpB9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpB7[1]={& _tmpB9};Cyc_Tcutil_terr(loc,({const char*_tmpB8="expecting integral or * type but found %s";
_tag_dynforward(_tmpB8,sizeof(char),_get_zero_arr_size(_tmpB8,42));}),
_tag_dynforward(_tmpB7,sizeof(void*),1));}});return Cyc_Absyn_sint_typ;_LL7E: if((
int)_tmpB3 != 12)goto _LL80;_LL7F: if(!Cyc_Tcutil_is_integral(e))({struct Cyc_String_pa_struct
_tmpBC;_tmpBC.tag=0;_tmpBC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpBA[1]={& _tmpBC};Cyc_Tcutil_terr(loc,({const char*_tmpBB="expecting integral type but found %s";
_tag_dynforward(_tmpBB,sizeof(char),_get_zero_arr_size(_tmpBB,37));}),
_tag_dynforward(_tmpBA,sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;_LL80: if((int)_tmpB3 != 19)goto _LL82;_LL81:{void*_tmpBD=
t;struct Cyc_Absyn_PtrInfo _tmpBE;struct Cyc_Absyn_PtrAtts _tmpBF;struct Cyc_Absyn_Conref*
_tmpC0;_LL85: if(_tmpBD <= (void*)4)goto _LL89;if(*((int*)_tmpBD)!= 7)goto _LL87;
_LL86: goto _LL84;_LL87: if(*((int*)_tmpBD)!= 4)goto _LL89;_tmpBE=((struct Cyc_Absyn_PointerType_struct*)
_tmpBD)->f1;_tmpBF=_tmpBE.ptr_atts;_tmpC0=_tmpBF.bounds;_LL88:{void*_tmpC1=(void*)(
Cyc_Absyn_compress_conref(_tmpC0))->v;void*_tmpC2;void*_tmpC3;void*_tmpC4;_LL8C:
if(_tmpC1 <= (void*)1)goto _LL92;if(*((int*)_tmpC1)!= 0)goto _LL8E;_tmpC2=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpC1)->f1;if((int)_tmpC2 != 0)goto _LL8E;_LL8D:
goto _LL8F;_LL8E: if(*((int*)_tmpC1)!= 0)goto _LL90;_tmpC3=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpC1)->f1;if((int)_tmpC3 != 1)goto _LL90;_LL8F: goto _LL91;_LL90: if(*((int*)_tmpC1)
!= 0)goto _LL92;_tmpC4=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpC1)->f1;if(
_tmpC4 <= (void*)2)goto _LL92;if(*((int*)_tmpC4)!= 0)goto _LL92;_LL91: goto _LL8B;
_LL92:;_LL93:({void*_tmpC5[0]={};Cyc_Tcutil_terr(loc,({const char*_tmpC6="can't apply size to pointer/array of abstract length";
_tag_dynforward(_tmpC6,sizeof(char),_get_zero_arr_size(_tmpC6,53));}),
_tag_dynforward(_tmpC5,sizeof(void*),0));});_LL8B:;}goto _LL84;_LL89:;_LL8A:({
struct Cyc_String_pa_struct _tmpC9;_tmpC9.tag=0;_tmpC9.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpC7[1]={& _tmpC9};Cyc_Tcutil_terr(
loc,({const char*_tmpC8="size requires pointer or array type, not %s";
_tag_dynforward(_tmpC8,sizeof(char),_get_zero_arr_size(_tmpC8,44));}),
_tag_dynforward(_tmpC7,sizeof(void*),1));}});_LL84:;}return Cyc_Absyn_uint_typ;
_LL82:;_LL83:({void*_tmpCA[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpCB="Non-unary primop";
_tag_dynforward(_tmpCB,sizeof(char),_get_zero_arr_size(_tmpCB,17));}),
_tag_dynforward(_tmpCA,sizeof(void*),0));});_LL77:;}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){({struct Cyc_String_pa_struct
_tmpCE;_tmpCE.tag=0;_tmpCE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmpCC[1]={& _tmpCE};
Cyc_Tcutil_terr(e1->loc,({const char*_tmpCD="type %s cannot be used here";
_tag_dynforward(_tmpCD,sizeof(char),_get_zero_arr_size(_tmpCD,28));}),
_tag_dynforward(_tmpCC,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->v=Cyc_Tcenv_lookup_type_vars(te);
_tmpCF;}));}if(!checker(e2)){({struct Cyc_String_pa_struct _tmpD2;_tmpD2.tag=0;
_tmpD2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmpD0[1]={& _tmpD2};
Cyc_Tcutil_terr(e2->loc,({const char*_tmpD1="type %s cannot be used here";
_tag_dynforward(_tmpD1,sizeof(char),_get_zero_arr_size(_tmpD1,28));}),
_tag_dynforward(_tmpD0,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->v=Cyc_Tcenv_lookup_type_vars(te);
_tmpD3;}));}{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);return Cyc_Tcutil_max_arithmetic_type(t1,t2);}}static
void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
void*_tmpD4=t1;struct Cyc_Absyn_PtrInfo _tmpD5;void*_tmpD6;struct Cyc_Absyn_Tqual
_tmpD7;struct Cyc_Absyn_PtrAtts _tmpD8;void*_tmpD9;struct Cyc_Absyn_Conref*_tmpDA;
struct Cyc_Absyn_Conref*_tmpDB;struct Cyc_Absyn_Conref*_tmpDC;_LL95: if(_tmpD4 <= (
void*)4)goto _LL97;if(*((int*)_tmpD4)!= 4)goto _LL97;_tmpD5=((struct Cyc_Absyn_PointerType_struct*)
_tmpD4)->f1;_tmpD6=(void*)_tmpD5.elt_typ;_tmpD7=_tmpD5.elt_tq;_tmpD8=_tmpD5.ptr_atts;
_tmpD9=(void*)_tmpD8.rgn;_tmpDA=_tmpD8.nullable;_tmpDB=_tmpD8.bounds;_tmpDC=
_tmpD8.zero_term;_LL96: if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpD6),(void*)
1))({void*_tmpDD[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpDE="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmpDE,sizeof(char),_get_zero_arr_size(_tmpDE,50));}),
_tag_dynforward(_tmpDD,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2))({
struct Cyc_String_pa_struct _tmpE1;_tmpE1.tag=0;_tmpE1.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{void*_tmpDF[1]={& _tmpE1};Cyc_Tcutil_terr(
e2->loc,({const char*_tmpE0="expecting int but found %s";_tag_dynforward(_tmpE0,
sizeof(char),_get_zero_arr_size(_tmpE0,27));}),_tag_dynforward(_tmpDF,sizeof(
void*),1));}});_tmpDB=Cyc_Absyn_compress_conref(_tmpDB);{void*_tmpE2=(void*)
_tmpDB->v;void*_tmpE3;void*_tmpE4;void*_tmpE5;struct Cyc_Absyn_Exp*_tmpE6;void*
_tmpE7;_LL9A: if(_tmpE2 <= (void*)1)goto _LLA2;if(*((int*)_tmpE2)!= 0)goto _LL9C;
_tmpE3=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpE2)->f1;if((int)_tmpE3 != 0)
goto _LL9C;_LL9B: return t1;_LL9C: if(*((int*)_tmpE2)!= 0)goto _LL9E;_tmpE4=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpE2)->f1;if((int)_tmpE4 != 1)goto _LL9E;_LL9D:
return t1;_LL9E: if(*((int*)_tmpE2)!= 0)goto _LLA0;_tmpE5=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpE2)->f1;if(_tmpE5 <= (void*)2)goto _LLA0;if(*((int*)_tmpE5)!= 0)goto _LLA0;
_tmpE6=((struct Cyc_Absyn_Upper_b_struct*)_tmpE5)->f1;_LL9F: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpDC))({void*_tmpE8[0]={};
Cyc_Tcutil_warn(e1->loc,({const char*_tmpE9="pointer arithmetic on thin, zero-terminated pointer may be expensive.";
_tag_dynforward(_tmpE9,sizeof(char),_get_zero_arr_size(_tmpE9,70));}),
_tag_dynforward(_tmpE8,sizeof(void*),0));});{struct Cyc_Absyn_PointerType_struct*
_tmpEA=({struct Cyc_Absyn_PointerType_struct*_tmpEB=_cycalloc(sizeof(*_tmpEB));
_tmpEB[0]=({struct Cyc_Absyn_PointerType_struct _tmpEC;_tmpEC.tag=4;_tmpEC.f1=({
struct Cyc_Absyn_PtrInfo _tmpED;_tmpED.elt_typ=(void*)_tmpD6;_tmpED.elt_tq=_tmpD7;
_tmpED.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpEE;_tmpEE.rgn=(void*)_tmpD9;_tmpEE.nullable=
Cyc_Absyn_true_conref;_tmpEE.bounds=Cyc_Absyn_bounds_dyneither_conref;_tmpEE.zero_term=
_tmpDC;_tmpEE.ptrloc=0;_tmpEE;});_tmpED;});_tmpEC;});_tmpEB;});Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmpEA,(void*)3);return(void*)_tmpEA;}_LLA0: if(*((int*)_tmpE2)!= 0)
goto _LLA2;_tmpE7=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpE2)->f1;if(
_tmpE7 <= (void*)2)goto _LLA2;if(*((int*)_tmpE7)!= 1)goto _LLA2;_LLA1:({void*_tmpEF[
0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpF0="pointer arithmetic not allowed on pointers with abstract bounds";
_tag_dynforward(_tmpF0,sizeof(char),_get_zero_arr_size(_tmpF0,64));}),
_tag_dynforward(_tmpEF,sizeof(void*),0));});return t1;_LLA2:;_LLA3:(void*)(_tmpDB->v=(
void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmpF1=_cycalloc(sizeof(*_tmpF1));
_tmpF1[0]=({struct Cyc_Absyn_Eq_constr_struct _tmpF2;_tmpF2.tag=0;_tmpF2.f1=(void*)((
void*)0);_tmpF2;});_tmpF1;})));return t1;_LL99:;}_LL97:;_LL98: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);_LL94:;}static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;
void*t1_elt=(void*)0;int forward_only1=0;int forward_only2=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& t1_elt,& forward_only1)){if(Cyc_Tcutil_is_tagged_pointer_typ(t2,&
forward_only2)){if(!Cyc_Tcutil_unify(t1,t2)){({struct Cyc_String_pa_struct _tmpF6;
_tmpF6.tag=0;_tmpF6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{struct Cyc_String_pa_struct
_tmpF5;_tmpF5.tag=0;_tmpF5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmpF3[2]={& _tmpF5,&
_tmpF6};Cyc_Tcutil_terr(e1->loc,({const char*_tmpF4="pointer arithmetic on values of different types (%s != %s)";
_tag_dynforward(_tmpF4,sizeof(char),_get_zero_arr_size(_tmpF4,59));}),
_tag_dynforward(_tmpF3,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return Cyc_Absyn_sint_typ;}else{if(forward_only1)({void*_tmpF7[0]={};Cyc_Tcutil_terr(
e1->loc,({const char*_tmpF8="can't subtract from forward-only ? pointer";
_tag_dynforward(_tmpF8,sizeof(char),_get_zero_arr_size(_tmpF8,43));}),
_tag_dynforward(_tmpF7,sizeof(void*),0));});if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1_elt),(void*)1))({void*_tmpF9[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpFA="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmpFA,sizeof(char),_get_zero_arr_size(_tmpFA,50));}),
_tag_dynforward(_tmpF9,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){({
struct Cyc_String_pa_struct _tmpFE;_tmpFE.tag=0;_tmpFE.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmpFD;_tmpFD.tag=0;_tmpFD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmpFB[2]={& _tmpFD,& _tmpFE};Cyc_Tcutil_terr(e2->loc,({const char*
_tmpFC="expecting either %s or int but found %s";_tag_dynforward(_tmpFC,sizeof(
char),_get_zero_arr_size(_tmpFC,40));}),_tag_dynforward(_tmpFB,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}return t1;}}if(Cyc_Tcutil_is_pointer_type(t1))Cyc_Tcutil_unchecked_cast(
te,e1,Cyc_Absyn_sint_typ,(void*)3);if(Cyc_Tcutil_is_pointer_type(t2))Cyc_Tcutil_unchecked_cast(
te,e2,Cyc_Absyn_sint_typ,(void*)3);return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int e1_is_num=Cyc_Tcutil_is_numeric(
e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(e1_is_num  && e2_is_num)return
Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_unify(t1,t2) && Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,({struct Cyc_Core_Opt*_tmpFF=_cycalloc(sizeof(*
_tmpFF));_tmpFF->v=Cyc_Tcenv_lookup_type_vars(te);_tmpFF;}))))return Cyc_Absyn_sint_typ;
else{if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(te,
e2,t1,(void*)3);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_silent_castable(te,
loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e1,t2,(void*)3);return Cyc_Absyn_sint_typ;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{{struct _tuple0 _tmp101=({struct _tuple0 _tmp100;
_tmp100.f1=Cyc_Tcutil_compress(t1);_tmp100.f2=Cyc_Tcutil_compress(t2);_tmp100;});
void*_tmp102;struct Cyc_Absyn_PtrInfo _tmp103;void*_tmp104;void*_tmp105;struct Cyc_Absyn_PtrInfo
_tmp106;void*_tmp107;_LLA5: _tmp102=_tmp101.f1;if(_tmp102 <= (void*)4)goto _LLA7;
if(*((int*)_tmp102)!= 4)goto _LLA7;_tmp103=((struct Cyc_Absyn_PointerType_struct*)
_tmp102)->f1;_tmp104=(void*)_tmp103.elt_typ;_tmp105=_tmp101.f2;if(_tmp105 <= (
void*)4)goto _LLA7;if(*((int*)_tmp105)!= 4)goto _LLA7;_tmp106=((struct Cyc_Absyn_PointerType_struct*)
_tmp105)->f1;_tmp107=(void*)_tmp106.elt_typ;_LLA6: if(Cyc_Tcutil_unify(_tmp104,
_tmp107))return Cyc_Absyn_sint_typ;goto _LLA4;_LLA7:;_LLA8: goto _LLA4;_LLA4:;}({
struct Cyc_String_pa_struct _tmp10B;_tmp10B.tag=0;_tmp10B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmp10A;_tmp10A.tag=0;_tmp10A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp108[2]={& _tmp10A,& _tmp10B};Cyc_Tcutil_terr(
loc,({const char*_tmp109="comparison not allowed between %s and %s";
_tag_dynforward(_tmp109,sizeof(char),_get_zero_arr_size(_tmp109,41));}),
_tag_dynforward(_tmp108,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp10C=_cycalloc(sizeof(*_tmp10C));
_tmp10C->v=Cyc_Tcenv_lookup_type_vars(te);_tmp10C;}));}}}}}}static void*Cyc_Tcexp_tcBinPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp10D=p;_LLAA: if((int)_tmp10D != 
0)goto _LLAC;_LLAB: return Cyc_Tcexp_tcPlus(te,e1,e2);_LLAC: if((int)_tmp10D != 2)
goto _LLAE;_LLAD: return Cyc_Tcexp_tcMinus(te,e1,e2);_LLAE: if((int)_tmp10D != 1)goto
_LLB0;_LLAF: goto _LLB1;_LLB0: if((int)_tmp10D != 3)goto _LLB2;_LLB1: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);_LLB2: if((int)_tmp10D != 4)goto _LLB4;_LLB3: goto
_LLB5;_LLB4: if((int)_tmp10D != 13)goto _LLB6;_LLB5: goto _LLB7;_LLB6: if((int)_tmp10D
!= 14)goto _LLB8;_LLB7: goto _LLB9;_LLB8: if((int)_tmp10D != 15)goto _LLBA;_LLB9: goto
_LLBB;_LLBA: if((int)_tmp10D != 16)goto _LLBC;_LLBB: goto _LLBD;_LLBC: if((int)_tmp10D
!= 17)goto _LLBE;_LLBD: goto _LLBF;_LLBE: if((int)_tmp10D != 18)goto _LLC0;_LLBF:
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);_LLC0: if((int)
_tmp10D != 5)goto _LLC2;_LLC1: goto _LLC3;_LLC2: if((int)_tmp10D != 6)goto _LLC4;_LLC3:
goto _LLC5;_LLC4: if((int)_tmp10D != 7)goto _LLC6;_LLC5: goto _LLC7;_LLC6: if((int)
_tmp10D != 8)goto _LLC8;_LLC7: goto _LLC9;_LLC8: if((int)_tmp10D != 9)goto _LLCA;_LLC9:
goto _LLCB;_LLCA: if((int)_tmp10D != 10)goto _LLCC;_LLCB: return Cyc_Tcexp_tcAnyBinop(
te,loc,e1,e2);_LLCC:;_LLCD:({void*_tmp10E[0]={};((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp10F="bad binary primop";
_tag_dynforward(_tmp10F,sizeof(char),_get_zero_arr_size(_tmp10F,18));}),
_tag_dynforward(_tmp10E,sizeof(void*),0));});_LLA9:;}static void*Cyc_Tcexp_tcPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_List_List*es){if(p == (void*)2  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
es)== 1)return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd);Cyc_Tcexp_tcExpList(te,es);{void*t;switch(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLCE: return({void*_tmp110[0]={};
Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp111="primitive operator has 0 arguments";
_tag_dynforward(_tmp111,sizeof(char),_get_zero_arr_size(_tmp111,35));}),
_tag_dynforward(_tmp110,sizeof(void*),0));});case 1: _LLCF: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLD0: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLD1: return({void*_tmp112[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp113="primitive operator has > 2 arguments";_tag_dynforward(_tmp113,sizeof(
char),_get_zero_arr_size(_tmp113,37));}),_tag_dynforward(_tmp112,sizeof(void*),0));});}
return t;}}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){{void*_tmp114=(void*)e->r;void*
_tmp115;struct Cyc_Absyn_Vardecl*_tmp116;void*_tmp117;struct Cyc_Absyn_Vardecl*
_tmp118;void*_tmp119;struct Cyc_Absyn_Vardecl*_tmp11A;void*_tmp11B;struct Cyc_Absyn_Vardecl*
_tmp11C;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*
_tmp11F;struct _dynforward_ptr*_tmp120;struct Cyc_Absyn_Exp*_tmp121;struct
_dynforward_ptr*_tmp122;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp124;
struct Cyc_Absyn_Exp*_tmp125;_LLD4: if(*((int*)_tmp114)!= 1)goto _LLD6;_tmp115=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp114)->f2;if(_tmp115 <= (void*)1)goto
_LLD6;if(*((int*)_tmp115)!= 2)goto _LLD6;_tmp116=((struct Cyc_Absyn_Param_b_struct*)
_tmp115)->f1;_LLD5: _tmp118=_tmp116;goto _LLD7;_LLD6: if(*((int*)_tmp114)!= 1)goto
_LLD8;_tmp117=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp114)->f2;if(_tmp117 <= (
void*)1)goto _LLD8;if(*((int*)_tmp117)!= 3)goto _LLD8;_tmp118=((struct Cyc_Absyn_Local_b_struct*)
_tmp117)->f1;_LLD7: _tmp11A=_tmp118;goto _LLD9;_LLD8: if(*((int*)_tmp114)!= 1)goto
_LLDA;_tmp119=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp114)->f2;if(_tmp119 <= (
void*)1)goto _LLDA;if(*((int*)_tmp119)!= 4)goto _LLDA;_tmp11A=((struct Cyc_Absyn_Pat_b_struct*)
_tmp119)->f1;_LLD9: _tmp11C=_tmp11A;goto _LLDB;_LLDA: if(*((int*)_tmp114)!= 1)goto
_LLDC;_tmp11B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp114)->f2;if(_tmp11B <= (
void*)1)goto _LLDC;if(*((int*)_tmp11B)!= 0)goto _LLDC;_tmp11C=((struct Cyc_Absyn_Global_b_struct*)
_tmp11B)->f1;_LLDB: if(!(_tmp11C->tq).real_const)return;goto _LLD3;_LLDC: if(*((int*)
_tmp114)!= 25)goto _LLDE;_tmp11D=((struct Cyc_Absyn_Subscript_e_struct*)_tmp114)->f1;
_tmp11E=((struct Cyc_Absyn_Subscript_e_struct*)_tmp114)->f2;_LLDD:{void*_tmp126=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp11D->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp127;struct Cyc_Absyn_Tqual _tmp128;struct Cyc_Absyn_ArrayInfo
_tmp129;struct Cyc_Absyn_Tqual _tmp12A;struct Cyc_List_List*_tmp12B;_LLEB: if(
_tmp126 <= (void*)4)goto _LLF1;if(*((int*)_tmp126)!= 4)goto _LLED;_tmp127=((struct
Cyc_Absyn_PointerType_struct*)_tmp126)->f1;_tmp128=_tmp127.elt_tq;_LLEC: _tmp12A=
_tmp128;goto _LLEE;_LLED: if(*((int*)_tmp126)!= 7)goto _LLEF;_tmp129=((struct Cyc_Absyn_ArrayType_struct*)
_tmp126)->f1;_tmp12A=_tmp129.tq;_LLEE: if(!_tmp12A.real_const)return;goto _LLEA;
_LLEF: if(*((int*)_tmp126)!= 9)goto _LLF1;_tmp12B=((struct Cyc_Absyn_TupleType_struct*)
_tmp126)->f1;_LLF0: {unsigned int _tmp12D;int _tmp12E;struct _tuple7 _tmp12C=Cyc_Evexp_eval_const_uint_exp(
_tmp11E);_tmp12D=_tmp12C.f1;_tmp12E=_tmp12C.f2;if(!_tmp12E){({void*_tmp12F[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp130="tuple projection cannot use sizeof or offsetof";
_tag_dynforward(_tmp130,sizeof(char),_get_zero_arr_size(_tmp130,47));}),
_tag_dynforward(_tmp12F,sizeof(void*),0));});return;}{struct _handler_cons _tmp131;
_push_handler(& _tmp131);{int _tmp133=0;if(setjmp(_tmp131.handler))_tmp133=1;if(!
_tmp133){{struct _tuple9 _tmp135;struct Cyc_Absyn_Tqual _tmp136;struct _tuple9*
_tmp134=((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp12B,(
int)_tmp12D);_tmp135=*_tmp134;_tmp136=_tmp135.f1;if(!_tmp136.real_const){
_npop_handler(0);return;}};_pop_handler();}else{void*_tmp132=(void*)_exn_thrown;
void*_tmp138=_tmp132;_LLF4: if(_tmp138 != Cyc_List_Nth)goto _LLF6;_LLF5: return;
_LLF6:;_LLF7:(void)_throw(_tmp138);_LLF3:;}}}goto _LLEA;}_LLF1:;_LLF2: goto _LLEA;
_LLEA:;}goto _LLD3;_LLDE: if(*((int*)_tmp114)!= 23)goto _LLE0;_tmp11F=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp114)->f1;_tmp120=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp114)->f2;_LLDF:{
void*_tmp139=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp11F->topt))->v);struct Cyc_Absyn_AggrInfo _tmp13A;void*_tmp13B;struct Cyc_Absyn_Aggrdecl**
_tmp13C;struct Cyc_List_List*_tmp13D;_LLF9: if(_tmp139 <= (void*)4)goto _LLFD;if(*((
int*)_tmp139)!= 10)goto _LLFB;_tmp13A=((struct Cyc_Absyn_AggrType_struct*)_tmp139)->f1;
_tmp13B=(void*)_tmp13A.aggr_info;if(*((int*)_tmp13B)!= 1)goto _LLFB;_tmp13C=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp13B)->f1;_LLFA: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp13C == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp13C,_tmp120);if(sf == 0  || !(sf->tq).real_const)return;goto _LLF8;}_LLFB: if(*((
int*)_tmp139)!= 11)goto _LLFD;_tmp13D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp139)->f2;_LLFC: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp13D,
_tmp120);if(sf == 0  || !(sf->tq).real_const)return;goto _LLF8;}_LLFD:;_LLFE: goto
_LLF8;_LLF8:;}goto _LLD3;_LLE0: if(*((int*)_tmp114)!= 24)goto _LLE2;_tmp121=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp114)->f1;_tmp122=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp114)->f2;_LLE1:{void*_tmp13E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp121->topt))->v);struct Cyc_Absyn_PtrInfo _tmp13F;void*_tmp140;
_LL100: if(_tmp13E <= (void*)4)goto _LL102;if(*((int*)_tmp13E)!= 4)goto _LL102;
_tmp13F=((struct Cyc_Absyn_PointerType_struct*)_tmp13E)->f1;_tmp140=(void*)
_tmp13F.elt_typ;_LL101:{void*_tmp141=Cyc_Tcutil_compress(_tmp140);struct Cyc_Absyn_AggrInfo
_tmp142;void*_tmp143;struct Cyc_Absyn_Aggrdecl**_tmp144;struct Cyc_List_List*
_tmp145;_LL105: if(_tmp141 <= (void*)4)goto _LL109;if(*((int*)_tmp141)!= 10)goto
_LL107;_tmp142=((struct Cyc_Absyn_AggrType_struct*)_tmp141)->f1;_tmp143=(void*)
_tmp142.aggr_info;if(*((int*)_tmp143)!= 1)goto _LL107;_tmp144=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp143)->f1;_LL106: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp144 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp144,_tmp122);if(sf == 0  || !(sf->tq).real_const)
return;goto _LL104;}_LL107: if(*((int*)_tmp141)!= 11)goto _LL109;_tmp145=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp141)->f2;_LL108: {struct Cyc_Absyn_Aggrfield*
sf=Cyc_Absyn_lookup_field(_tmp145,_tmp122);if(sf == 0  || !(sf->tq).real_const)
return;goto _LL104;}_LL109:;_LL10A: goto _LL104;_LL104:;}goto _LLFF;_LL102:;_LL103:
goto _LLFF;_LLFF:;}goto _LLD3;_LLE2: if(*((int*)_tmp114)!= 22)goto _LLE4;_tmp123=((
struct Cyc_Absyn_Deref_e_struct*)_tmp114)->f1;_LLE3:{void*_tmp146=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp123->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp147;struct Cyc_Absyn_Tqual _tmp148;struct Cyc_Absyn_ArrayInfo _tmp149;struct Cyc_Absyn_Tqual
_tmp14A;_LL10C: if(_tmp146 <= (void*)4)goto _LL110;if(*((int*)_tmp146)!= 4)goto
_LL10E;_tmp147=((struct Cyc_Absyn_PointerType_struct*)_tmp146)->f1;_tmp148=
_tmp147.elt_tq;_LL10D: _tmp14A=_tmp148;goto _LL10F;_LL10E: if(*((int*)_tmp146)!= 7)
goto _LL110;_tmp149=((struct Cyc_Absyn_ArrayType_struct*)_tmp146)->f1;_tmp14A=
_tmp149.tq;_LL10F: if(!_tmp14A.real_const)return;goto _LL10B;_LL110:;_LL111: goto
_LL10B;_LL10B:;}goto _LLD3;_LLE4: if(*((int*)_tmp114)!= 13)goto _LLE6;_tmp124=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp114)->f1;_LLE5: _tmp125=_tmp124;goto
_LLE7;_LLE6: if(*((int*)_tmp114)!= 14)goto _LLE8;_tmp125=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp114)->f1;_LLE7: Cyc_Tcexp_check_writable(te,_tmp125);return;_LLE8:;_LLE9: goto
_LLD3;_LLD3:;}({struct Cyc_String_pa_struct _tmp14D;_tmp14D.tag=0;_tmp14D.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{void*
_tmp14B[1]={& _tmp14D};Cyc_Tcutil_terr(e->loc,({const char*_tmp14C="attempt to write a const location: %s";
_tag_dynforward(_tmp14C,sizeof(char),_get_zero_arr_size(_tmp14C,38));}),
_tag_dynforward(_tmp14B,sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,void*i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(!Cyc_Absyn_is_lvalue(e))return({
void*_tmp14E[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp14F="increment/decrement of non-lvalue";
_tag_dynforward(_tmp14F,sizeof(char),_get_zero_arr_size(_tmp14F,34));}),
_tag_dynforward(_tmp14E,sizeof(void*),0));});Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt,& forward_only) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == (
void*)0  || i == (void*)1)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),(
void*)1))({void*_tmp150[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp151="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmp151,sizeof(char),_get_zero_arr_size(_tmp151,50));}),
_tag_dynforward(_tmp150,sizeof(void*),0));});if(forward_only  && (i == (void*)2
 || i == (void*)3))({void*_tmp152[0]={};Cyc_Tcutil_terr(e->loc,({const char*
_tmp153="can't subtract from forward-only ? pointer";_tag_dynforward(_tmp153,
sizeof(char),_get_zero_arr_size(_tmp153,43));}),_tag_dynforward(_tmp152,sizeof(
void*),0));});}else{({struct Cyc_String_pa_struct _tmp156;_tmp156.tag=0;_tmp156.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*
_tmp154[1]={& _tmp156};Cyc_Tcutil_terr(e->loc,({const char*_tmp155="expecting arithmetic or ? type but found %s";
_tag_dynforward(_tmp155,sizeof(char),_get_zero_arr_size(_tmp155,44));}),
_tag_dynforward(_tmp154,sizeof(void*),1));}});}}return t;}}static void*Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){Cyc_Tcexp_tcTest(te,e1,({const
char*_tmp157="conditional expression";_tag_dynforward(_tmp157,sizeof(char),
_get_zero_arr_size(_tmp157,23));}));Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(
te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({
struct Cyc_Core_Opt*_tmp160=_cycalloc(sizeof(*_tmp160));_tmp160->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp160;}));struct Cyc_List_List _tmp158=({struct Cyc_List_List _tmp15F;_tmp15F.hd=
e3;_tmp15F.tl=0;_tmp15F;});struct Cyc_List_List _tmp159=({struct Cyc_List_List
_tmp15E;_tmp15E.hd=e2;_tmp15E.tl=(struct Cyc_List_List*)& _tmp158;_tmp15E;});if(!
Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)& _tmp159)){({struct Cyc_String_pa_struct
_tmp15D;_tmp15D.tag=0;_tmp15D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v));{
struct Cyc_String_pa_struct _tmp15C;_tmp15C.tag=0;_tmp15C.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmp15A[2]={& _tmp15C,& _tmp15D};
Cyc_Tcutil_terr(loc,({const char*_tmp15B="conditional clause types do not match: %s != %s";
_tag_dynforward(_tmp15B,sizeof(char),_get_zero_arr_size(_tmp15B,48));}),
_tag_dynforward(_tmp15A,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t;}}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcTest(te,e1,({
const char*_tmp161="logical-and expression";_tag_dynforward(_tmp161,sizeof(char),
_get_zero_arr_size(_tmp161,23));}));Cyc_Tcexp_tcTest(te,e2,({const char*_tmp162="logical-and expression";
_tag_dynforward(_tmp162,sizeof(char),_get_zero_arr_size(_tmp162,23));}));return
Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcTest(te,e1,({
const char*_tmp163="logical-or expression";_tag_dynforward(_tmp163,sizeof(char),
_get_zero_arr_size(_tmp163,22));}));Cyc_Tcexp_tcTest(te,e2,({const char*_tmp164="logical-or expression";
_tag_dynforward(_tmp164,sizeof(char),_get_zero_arr_size(_tmp164,22));}));return
Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){{struct _RegionHandle _tmp165=_new_region("r");struct
_RegionHandle*r=& _tmp165;_push_region(r);Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(
r,te),0,e1);;_pop_region(r);}Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp166=Cyc_Tcutil_compress(t1);_LL113: if(_tmp166 <= (void*)4)goto _LL115;if(*((
int*)_tmp166)!= 7)goto _LL115;_LL114:({void*_tmp167[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp168="cannot assign to an array";_tag_dynforward(_tmp168,sizeof(
char),_get_zero_arr_size(_tmp168,26));}),_tag_dynforward(_tmp167,sizeof(void*),0));});
goto _LL112;_LL115:;_LL116: goto _LL112;_LL112:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),(void*)1))({void*_tmp169[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp16A="type is abstract (can't determine size).";
_tag_dynforward(_tmp16A,sizeof(char),_get_zero_arr_size(_tmp16A,41));}),
_tag_dynforward(_tmp169,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(e1))return({
void*_tmp16B[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp16C="assignment to non-lvalue";
_tag_dynforward(_tmp16C,sizeof(char),_get_zero_arr_size(_tmp16C,25));}),
_tag_dynforward(_tmp16B,sizeof(void*),0));});Cyc_Tcexp_check_writable(te,e1);if(
po == 0){if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){void*_tmp16D=({struct Cyc_String_pa_struct
_tmp171;_tmp171.tag=0;_tmp171.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp170;_tmp170.tag=0;
_tmp170.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp16E[2]={& _tmp170,& _tmp171};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp16F="type mismatch: %s != %s";_tag_dynforward(_tmp16F,sizeof(char),
_get_zero_arr_size(_tmp16F,24));}),_tag_dynforward(_tmp16E,sizeof(void*),2));}}});
Cyc_Tcutil_unify(t1,t2);Cyc_Tcutil_explain_failure();return _tmp16D;}}else{void*
_tmp172=(void*)po->v;void*_tmp173=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp172,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp173,t1) || Cyc_Tcutil_coerceable(_tmp173) && Cyc_Tcutil_coerceable(
t1))){void*_tmp174=({struct Cyc_String_pa_struct _tmp178;_tmp178.tag=0;_tmp178.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp177;_tmp177.tag=0;_tmp177.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp175[2]={& _tmp177,& _tmp178};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp176="Cannot use this operator in an assignment when the arguments have types %s and %s";
_tag_dynforward(_tmp176,sizeof(char),_get_zero_arr_size(_tmp176,82));}),
_tag_dynforward(_tmp175,sizeof(void*),2));}}});Cyc_Tcutil_unify(_tmp173,t1);Cyc_Tcutil_explain_failure();
return _tmp174;}return _tmp173;}return t1;}}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={6,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp179=Cyc_Tcutil_compress(
t1);void*_tmp17A;void*_tmp17B;_LL118: if((int)_tmp179 != 1)goto _LL11A;_LL119: Cyc_Tcutil_unchecked_cast(
te,e,dbl_typ,(void*)1);t1=dbl_typ;goto _LL117;_LL11A: if(_tmp179 <= (void*)4)goto
_LL11E;if(*((int*)_tmp179)!= 5)goto _LL11C;_tmp17A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp179)->f2;if((int)_tmp17A != 0)goto _LL11C;_LL11B: goto _LL11D;_LL11C: if(*((int*)
_tmp179)!= 5)goto _LL11E;_tmp17B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp179)->f2;
if((int)_tmp17B != 1)goto _LL11E;_LL11D: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);t1=Cyc_Absyn_sint_typ;goto _LL117;_LL11E:;_LL11F: goto _LL117;_LL117:;}
for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp17D;struct Cyc_List_List*
_tmp17E;struct Cyc_Position_Segment*_tmp17F;void*_tmp180;struct Cyc_Absyn_Tunionfield
_tmp17C=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_tmp17D=_tmp17C.name;_tmp17E=
_tmp17C.typs;_tmp17F=_tmp17C.loc;_tmp180=(void*)_tmp17C.sc;if(_tmp17E == 0  || 
_tmp17E->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple9*)_tmp17E->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)fields->hd);}}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp182;struct Cyc_List_List*_tmp183;struct Cyc_Position_Segment*
_tmp184;void*_tmp185;struct Cyc_Absyn_Tunionfield _tmp181=*((struct Cyc_Absyn_Tunionfield*)
fields->hd);_tmp182=_tmp181.name;_tmp183=_tmp181.typs;_tmp184=_tmp181.loc;
_tmp185=(void*)_tmp181.sc;if(_tmp183 == 0  || _tmp183->tl != 0)continue;{void*t2=
Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple9*)_tmp183->hd)).f2);if(Cyc_Tcutil_coerce_arg(
te,e,t2))return(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)
fields->hd);}}({struct Cyc_String_pa_struct _tmp189;_tmp189.tag=0;_tmp189.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{
struct Cyc_String_pa_struct _tmp188;_tmp188.tag=0;_tmp188.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(tu));{void*
_tmp186[2]={& _tmp188,& _tmp189};Cyc_Tcutil_terr(e->loc,({const char*_tmp187="can't find a field in %s to inject a value of type %s";
_tag_dynforward(_tmp187,sizeof(char),_get_zero_arr_size(_tmp187,54));}),
_tag_dynforward(_tmp186,sizeof(void*),2));}}});return 0;}static void*Cyc_Tcexp_tcFnCall(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){
struct Cyc_List_List*_tmp18B=args;struct _RegionHandle _tmp18C=_new_region("ter");
struct _RegionHandle*ter=& _tmp18C;_push_region(ter);{struct Cyc_Tcenv_Tenv*_tmp18D=
Cyc_Tcenv_new_block(ter,loc,te_orig);Cyc_Tcexp_tcExp(_tmp18D,0,e);{void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp18E=t;struct Cyc_Absyn_PtrInfo
_tmp18F;void*_tmp190;struct Cyc_Absyn_Tqual _tmp191;struct Cyc_Absyn_PtrAtts _tmp192;
void*_tmp193;struct Cyc_Absyn_Conref*_tmp194;struct Cyc_Absyn_Conref*_tmp195;
struct Cyc_Absyn_Conref*_tmp196;_LL121: if(_tmp18E <= (void*)4)goto _LL123;if(*((int*)
_tmp18E)!= 4)goto _LL123;_tmp18F=((struct Cyc_Absyn_PointerType_struct*)_tmp18E)->f1;
_tmp190=(void*)_tmp18F.elt_typ;_tmp191=_tmp18F.elt_tq;_tmp192=_tmp18F.ptr_atts;
_tmp193=(void*)_tmp192.rgn;_tmp194=_tmp192.nullable;_tmp195=_tmp192.bounds;
_tmp196=_tmp192.zero_term;_LL122: Cyc_Tcenv_check_rgn_accessible(_tmp18D,loc,
_tmp193);Cyc_Tcutil_check_nonzero_bound(loc,_tmp195);{void*_tmp197=Cyc_Tcutil_compress(
_tmp190);struct Cyc_Absyn_FnInfo _tmp198;struct Cyc_List_List*_tmp199;struct Cyc_Core_Opt*
_tmp19A;void*_tmp19B;struct Cyc_List_List*_tmp19C;int _tmp19D;struct Cyc_Absyn_VarargInfo*
_tmp19E;struct Cyc_List_List*_tmp19F;struct Cyc_List_List*_tmp1A0;_LL126: if(
_tmp197 <= (void*)4)goto _LL128;if(*((int*)_tmp197)!= 8)goto _LL128;_tmp198=((
struct Cyc_Absyn_FnType_struct*)_tmp197)->f1;_tmp199=_tmp198.tvars;_tmp19A=
_tmp198.effect;_tmp19B=(void*)_tmp198.ret_typ;_tmp19C=_tmp198.args;_tmp19D=
_tmp198.c_varargs;_tmp19E=_tmp198.cyc_varargs;_tmp19F=_tmp198.rgn_po;_tmp1A0=
_tmp198.attributes;_LL127: if(topt != 0)Cyc_Tcutil_unify(_tmp19B,*topt);while(
_tmp18B != 0  && _tmp19C != 0){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp18B->hd;
void*t2=(*((struct _tuple2*)_tmp19C->hd)).f3;Cyc_Tcexp_tcExp(_tmp18D,(void**)& t2,
e1);if(!Cyc_Tcutil_coerce_arg(_tmp18D,e1,t2)){({struct Cyc_String_pa_struct
_tmp1A4;_tmp1A4.tag=0;_tmp1A4.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp1A3;_tmp1A3.tag=0;
_tmp1A3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmp1A1[2]={&
_tmp1A3,& _tmp1A4};Cyc_Tcutil_terr(e1->loc,({const char*_tmp1A2="actual argument has type \n\t%s\n but formal has type \n\t%s.";
_tag_dynforward(_tmp1A2,sizeof(char),_get_zero_arr_size(_tmp1A2,57));}),
_tag_dynforward(_tmp1A1,sizeof(void*),2));}}});Cyc_Tcutil_unify((void*)((struct
Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}
_tmp18B=_tmp18B->tl;_tmp19C=_tmp19C->tl;}{int args_already_checked=0;{struct Cyc_List_List*
a=_tmp1A0;for(0;a != 0;a=a->tl){void*_tmp1A5=(void*)a->hd;void*_tmp1A6;int _tmp1A7;
int _tmp1A8;_LL12B: if(_tmp1A5 <= (void*)17)goto _LL12D;if(*((int*)_tmp1A5)!= 3)goto
_LL12D;_tmp1A6=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp1A5)->f1;_tmp1A7=((
struct Cyc_Absyn_Format_att_struct*)_tmp1A5)->f2;_tmp1A8=((struct Cyc_Absyn_Format_att_struct*)
_tmp1A5)->f3;_LL12C:{struct _handler_cons _tmp1A9;_push_handler(& _tmp1A9);{int
_tmp1AB=0;if(setjmp(_tmp1A9.handler))_tmp1AB=1;if(!_tmp1AB){{struct Cyc_Absyn_Exp*
_tmp1AC=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,
_tmp1A7 - 1);struct Cyc_Core_Opt*fmt_args;if(_tmp1A8 == 0)fmt_args=0;else{fmt_args=({
struct Cyc_Core_Opt*_tmp1AD=_cycalloc(sizeof(*_tmp1AD));_tmp1AD->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp1A8 - 1);_tmp1AD;});}
args_already_checked=1;{void*_tmp1AE=_tmp1A6;_LL130: if((int)_tmp1AE != 0)goto
_LL132;_LL131: Cyc_Tcexp_check_format_args(_tmp18D,_tmp1AC,fmt_args,Cyc_Formatstr_get_format_typs);
goto _LL12F;_LL132: if((int)_tmp1AE != 1)goto _LL12F;_LL133: Cyc_Tcexp_check_format_args(
_tmp18D,_tmp1AC,fmt_args,Cyc_Formatstr_get_scanf_typs);goto _LL12F;_LL12F:;}};
_pop_handler();}else{void*_tmp1AA=(void*)_exn_thrown;void*_tmp1B0=_tmp1AA;_LL135:
if(_tmp1B0 != Cyc_List_Nth)goto _LL137;_LL136:({void*_tmp1B1[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp1B2="bad format arguments";_tag_dynforward(_tmp1B2,sizeof(
char),_get_zero_arr_size(_tmp1B2,21));}),_tag_dynforward(_tmp1B1,sizeof(void*),0));});
goto _LL134;_LL137:;_LL138:(void)_throw(_tmp1B0);_LL134:;}}}goto _LL12A;_LL12D:;
_LL12E: goto _LL12A;_LL12A:;}}if(_tmp19C != 0)({void*_tmp1B3[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp1B4="too few arguments for function";_tag_dynforward(_tmp1B4,
sizeof(char),_get_zero_arr_size(_tmp1B4,31));}),_tag_dynforward(_tmp1B3,sizeof(
void*),0));});else{if((_tmp18B != 0  || _tmp19D) || _tmp19E != 0){if(_tmp19D)for(0;
_tmp18B != 0;_tmp18B=_tmp18B->tl){Cyc_Tcexp_tcExp(_tmp18D,0,(struct Cyc_Absyn_Exp*)
_tmp18B->hd);}else{if(_tmp19E == 0)({void*_tmp1B5[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp1B6="too many arguments for function";_tag_dynforward(_tmp1B6,
sizeof(char),_get_zero_arr_size(_tmp1B6,32));}),_tag_dynforward(_tmp1B5,sizeof(
void*),0));});else{void*_tmp1B8;int _tmp1B9;struct Cyc_Absyn_VarargInfo _tmp1B7=*
_tmp19E;_tmp1B8=(void*)_tmp1B7.type;_tmp1B9=_tmp1B7.inject;{struct Cyc_Absyn_VarargCallInfo*
_tmp1BA=({struct Cyc_Absyn_VarargCallInfo*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));
_tmp1D2->num_varargs=0;_tmp1D2->injectors=0;_tmp1D2->vai=(struct Cyc_Absyn_VarargInfo*)
_tmp19E;_tmp1D2;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)_tmp1BA;
if(!_tmp1B9)for(0;_tmp18B != 0;_tmp18B=_tmp18B->tl){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp18B->hd;_tmp1BA->num_varargs ++;Cyc_Tcexp_tcExp(_tmp18D,(
void**)& _tmp1B8,e1);if(!Cyc_Tcutil_coerce_arg(_tmp18D,e1,_tmp1B8)){({struct Cyc_String_pa_struct
_tmp1BE;_tmp1BE.tag=0;_tmp1BE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{
struct Cyc_String_pa_struct _tmp1BD;_tmp1BD.tag=0;_tmp1BD.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp1B8));{void*
_tmp1BB[2]={& _tmp1BD,& _tmp1BE};Cyc_Tcutil_terr(loc,({const char*_tmp1BC="vararg requires type %s but argument has type %s";
_tag_dynforward(_tmp1BC,sizeof(char),_get_zero_arr_size(_tmp1BC,49));}),
_tag_dynforward(_tmp1BB,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
else{void*_tmp1BF=Cyc_Tcutil_compress(_tmp1B8);struct Cyc_Absyn_TunionInfo _tmp1C0;
void*_tmp1C1;struct Cyc_Absyn_Tuniondecl**_tmp1C2;struct Cyc_Absyn_Tuniondecl*
_tmp1C3;struct Cyc_List_List*_tmp1C4;struct Cyc_Core_Opt*_tmp1C5;_LL13A: if(_tmp1BF
<= (void*)4)goto _LL13C;if(*((int*)_tmp1BF)!= 2)goto _LL13C;_tmp1C0=((struct Cyc_Absyn_TunionType_struct*)
_tmp1BF)->f1;_tmp1C1=(void*)_tmp1C0.tunion_info;if(*((int*)_tmp1C1)!= 1)goto
_LL13C;_tmp1C2=((struct Cyc_Absyn_KnownTunion_struct*)_tmp1C1)->f1;_tmp1C3=*
_tmp1C2;_tmp1C4=_tmp1C0.targs;_tmp1C5=_tmp1C0.rgn;_LL13B: {struct Cyc_Absyn_Tuniondecl*
_tmp1C6=*Cyc_Tcenv_lookup_tuniondecl(_tmp18D,loc,_tmp1C3->name);struct Cyc_List_List*
fields=0;if(_tmp1C6->fields == 0)({struct Cyc_String_pa_struct _tmp1C9;_tmp1C9.tag=
0;_tmp1C9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
_tmp1B8));{void*_tmp1C7[1]={& _tmp1C9};Cyc_Tcutil_terr(loc,({const char*_tmp1C8="can't inject into %s";
_tag_dynforward(_tmp1C8,sizeof(char),_get_zero_arr_size(_tmp1C8,21));}),
_tag_dynforward(_tmp1C7,sizeof(void*),1));}});else{fields=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1C6->fields))->v;}if(!Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1C5))->v,Cyc_Tcenv_curr_rgn(_tmp18D)))({void*
_tmp1CA[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1CB="bad region for injected varargs";
_tag_dynforward(_tmp1CB,sizeof(char),_get_zero_arr_size(_tmp1CB,32));}),
_tag_dynforward(_tmp1CA,sizeof(void*),0));});{struct _RegionHandle _tmp1CC=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp1CC;_push_region(rgn);{struct Cyc_List_List*
_tmp1CD=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp1C6->tvs,
_tmp1C4);for(0;_tmp18B != 0;_tmp18B=_tmp18B->tl){_tmp1BA->num_varargs ++;{struct
Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp18B->hd;if(!args_already_checked)Cyc_Tcexp_tcExp(
_tmp18D,0,e1);{struct Cyc_Absyn_Tunionfield*_tmp1CE=Cyc_Tcexp_tcInjection(_tmp18D,
e1,_tmp1B8,rgn,_tmp1CD,fields);if(_tmp1CE != 0)_tmp1BA->injectors=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1BA->injectors,({
struct Cyc_List_List*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF->hd=(struct Cyc_Absyn_Tunionfield*)
_tmp1CE;_tmp1CF->tl=0;_tmp1CF;}));}}}};_pop_region(rgn);}goto _LL139;}_LL13C:;
_LL13D:({void*_tmp1D0[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1D1="bad inject vararg type";
_tag_dynforward(_tmp1D1,sizeof(char),_get_zero_arr_size(_tmp1D1,23));}),
_tag_dynforward(_tmp1D0,sizeof(void*),0));});goto _LL139;_LL139:;}}}}}}Cyc_Tcenv_check_effect_accessible(
_tmp18D,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp19A))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp18D,loc,_tmp19F);{void*_tmp1D3=_tmp19B;_npop_handler(0);return _tmp1D3;}}
_LL128:;_LL129: {void*_tmp1D6=({void*_tmp1D4[0]={};Cyc_Tcexp_expr_err(_tmp18D,
loc,topt,({const char*_tmp1D5="expected pointer to function";_tag_dynforward(
_tmp1D5,sizeof(char),_get_zero_arr_size(_tmp1D5,29));}),_tag_dynforward(_tmp1D4,
sizeof(void*),0));});_npop_handler(0);return _tmp1D6;}_LL125:;}_LL123:;_LL124: {
void*_tmp1D9=({void*_tmp1D7[0]={};Cyc_Tcexp_expr_err(_tmp18D,loc,topt,({const
char*_tmp1D8="expected pointer to function";_tag_dynforward(_tmp1D8,sizeof(char),
_get_zero_arr_size(_tmp1D8,29));}),_tag_dynforward(_tmp1D7,sizeof(void*),0));});
_npop_handler(0);return _tmp1D9;}_LL120:;}};_pop_region(ter);}static void*Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e){Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_exn_typ,e);if(!Cyc_Tcutil_coerce_arg(te,
e,Cyc_Absyn_exn_typ))({struct Cyc_String_pa_struct _tmp1DC;_tmp1DC.tag=0;_tmp1DC.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp1DA[1]={& _tmp1DC};Cyc_Tcutil_terr(
loc,({const char*_tmp1DB="expected xtunion exn but found %s";_tag_dynforward(
_tmp1DB,sizeof(char),_get_zero_arr_size(_tmp1DB,34));}),_tag_dynforward(_tmp1DA,
sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp1DD=
_cycalloc(sizeof(*_tmp1DD));_tmp1DD->v=Cyc_Tcenv_lookup_type_vars(te);_tmp1DD;}));}
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(
te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0));{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1DE=t1;struct Cyc_Absyn_PtrInfo
_tmp1DF;void*_tmp1E0;struct Cyc_Absyn_Tqual _tmp1E1;struct Cyc_Absyn_PtrAtts _tmp1E2;
void*_tmp1E3;struct Cyc_Absyn_Conref*_tmp1E4;struct Cyc_Absyn_Conref*_tmp1E5;
struct Cyc_Absyn_Conref*_tmp1E6;_LL13F: if(_tmp1DE <= (void*)4)goto _LL141;if(*((int*)
_tmp1DE)!= 4)goto _LL141;_tmp1DF=((struct Cyc_Absyn_PointerType_struct*)_tmp1DE)->f1;
_tmp1E0=(void*)_tmp1DF.elt_typ;_tmp1E1=_tmp1DF.elt_tq;_tmp1E2=_tmp1DF.ptr_atts;
_tmp1E3=(void*)_tmp1E2.rgn;_tmp1E4=_tmp1E2.nullable;_tmp1E5=_tmp1E2.bounds;
_tmp1E6=_tmp1E2.zero_term;_LL140:{void*_tmp1E7=Cyc_Tcutil_compress(_tmp1E0);
struct Cyc_Absyn_FnInfo _tmp1E8;struct Cyc_List_List*_tmp1E9;struct Cyc_Core_Opt*
_tmp1EA;void*_tmp1EB;struct Cyc_List_List*_tmp1EC;int _tmp1ED;struct Cyc_Absyn_VarargInfo*
_tmp1EE;struct Cyc_List_List*_tmp1EF;struct Cyc_List_List*_tmp1F0;_LL144: if(
_tmp1E7 <= (void*)4)goto _LL146;if(*((int*)_tmp1E7)!= 8)goto _LL146;_tmp1E8=((
struct Cyc_Absyn_FnType_struct*)_tmp1E7)->f1;_tmp1E9=_tmp1E8.tvars;_tmp1EA=
_tmp1E8.effect;_tmp1EB=(void*)_tmp1E8.ret_typ;_tmp1EC=_tmp1E8.args;_tmp1ED=
_tmp1E8.c_varargs;_tmp1EE=_tmp1E8.cyc_varargs;_tmp1EF=_tmp1E8.rgn_po;_tmp1F0=
_tmp1E8.attributes;_LL145: {struct Cyc_List_List*instantiation=0;for(0;ts != 0  && 
_tmp1E9 != 0;(ts=ts->tl,_tmp1E9=_tmp1E9->tl)){void*k=Cyc_Tcutil_tvar_kind((struct
Cyc_Absyn_Tvar*)_tmp1E9->hd);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),k,1,(void*)ts->hd);instantiation=({struct Cyc_List_List*_tmp1F1=_cycalloc(
sizeof(*_tmp1F1));_tmp1F1->hd=({struct _tuple5*_tmp1F2=_cycalloc(sizeof(*_tmp1F2));
_tmp1F2->f1=(struct Cyc_Absyn_Tvar*)_tmp1E9->hd;_tmp1F2->f2=(void*)ts->hd;_tmp1F2;});
_tmp1F1->tl=instantiation;_tmp1F1;});}if(ts != 0)return({void*_tmp1F3[0]={};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp1F4="too many type variables in instantiation";
_tag_dynforward(_tmp1F4,sizeof(char),_get_zero_arr_size(_tmp1F4,41));}),
_tag_dynforward(_tmp1F3,sizeof(void*),0));});{void*new_fn_typ=Cyc_Tcutil_substitute(
instantiation,(void*)({struct Cyc_Absyn_FnType_struct*_tmp1F9=_cycalloc(sizeof(*
_tmp1F9));_tmp1F9[0]=({struct Cyc_Absyn_FnType_struct _tmp1FA;_tmp1FA.tag=8;
_tmp1FA.f1=({struct Cyc_Absyn_FnInfo _tmp1FB;_tmp1FB.tvars=_tmp1E9;_tmp1FB.effect=
_tmp1EA;_tmp1FB.ret_typ=(void*)_tmp1EB;_tmp1FB.args=_tmp1EC;_tmp1FB.c_varargs=
_tmp1ED;_tmp1FB.cyc_varargs=_tmp1EE;_tmp1FB.rgn_po=_tmp1EF;_tmp1FB.attributes=
_tmp1F0;_tmp1FB;});_tmp1FA;});_tmp1F9;}));return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5[0]=({struct Cyc_Absyn_PointerType_struct
_tmp1F6;_tmp1F6.tag=4;_tmp1F6.f1=({struct Cyc_Absyn_PtrInfo _tmp1F7;_tmp1F7.elt_typ=(
void*)new_fn_typ;_tmp1F7.elt_tq=_tmp1E1;_tmp1F7.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp1F8;_tmp1F8.rgn=(void*)_tmp1E3;_tmp1F8.nullable=_tmp1E4;_tmp1F8.bounds=
_tmp1E5;_tmp1F8.zero_term=_tmp1E6;_tmp1F8.ptrloc=0;_tmp1F8;});_tmp1F7;});_tmp1F6;});
_tmp1F5;});}}_LL146:;_LL147: goto _LL143;_LL143:;}goto _LL13E;_LL141:;_LL142: goto
_LL13E;_LL13E:;}return({struct Cyc_String_pa_struct _tmp1FE;_tmp1FE.tag=0;_tmp1FE.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp1FC[1]={& _tmp1FE};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp1FD="expecting polymorphic type but found %s";
_tag_dynforward(_tmp1FD,sizeof(char),_get_zero_arr_size(_tmp1FD,40));}),
_tag_dynforward(_tmp1FC,sizeof(void*),1));}});}}static void*Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct
Cyc_Absyn_Exp*e,void**c){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(te,loc,t2,t))*((void**)
_check_null(c))=(void*)1;else{void*crc=Cyc_Tcutil_castable(te,loc,t2,t);if(crc != (
void*)0)*((void**)_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((
void**)_check_null(c))=(void*)1;else{Cyc_Tcutil_unify(t2,t);{void*_tmp1FF=({
struct Cyc_String_pa_struct _tmp203;_tmp203.tag=0;_tmp203.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp202;_tmp202.tag=0;_tmp202.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{void*_tmp200[2]={& _tmp202,& _tmp203};Cyc_Tcexp_expr_err(
te,loc,(void**)& t,({const char*_tmp201="cannot cast %s to %s";_tag_dynforward(
_tmp201,sizeof(char),_get_zero_arr_size(_tmp201,21));}),_tag_dynforward(_tmp200,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();return _tmp1FF;}}}}{struct
_tuple0 _tmp205=({struct _tuple0 _tmp204;_tmp204.f1=(void*)e->r;_tmp204.f2=Cyc_Tcutil_compress(
t);_tmp204;});void*_tmp206;struct Cyc_Absyn_MallocInfo _tmp207;int _tmp208;void*
_tmp209;struct Cyc_Absyn_PtrInfo _tmp20A;struct Cyc_Absyn_PtrAtts _tmp20B;struct Cyc_Absyn_Conref*
_tmp20C;struct Cyc_Absyn_Conref*_tmp20D;struct Cyc_Absyn_Conref*_tmp20E;_LL149:
_tmp206=_tmp205.f1;if(*((int*)_tmp206)!= 35)goto _LL14B;_tmp207=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp206)->f1;_tmp208=_tmp207.fat_result;_tmp209=_tmp205.f2;if(_tmp209 <= (void*)4)
goto _LL14B;if(*((int*)_tmp209)!= 4)goto _LL14B;_tmp20A=((struct Cyc_Absyn_PointerType_struct*)
_tmp209)->f1;_tmp20B=_tmp20A.ptr_atts;_tmp20C=_tmp20B.nullable;_tmp20D=_tmp20B.bounds;
_tmp20E=_tmp20B.zero_term;_LL14A: if((_tmp208  && !((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp20E)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp20C)){void*_tmp20F=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp20D);struct Cyc_Absyn_Exp*_tmp210;_LL14E: if(_tmp20F <= (void*)2)goto _LL150;if(*((
int*)_tmp20F)!= 0)goto _LL150;_tmp210=((struct Cyc_Absyn_Upper_b_struct*)_tmp20F)->f1;
_LL14F: if((Cyc_Evexp_eval_const_uint_exp(_tmp210)).f1 == 1)({void*_tmp211[0]={};
Cyc_Tcutil_warn(loc,({const char*_tmp212="cast from ? pointer to * pointer will lose size information";
_tag_dynforward(_tmp212,sizeof(char),_get_zero_arr_size(_tmp212,60));}),
_tag_dynforward(_tmp211,sizeof(void*),0));});goto _LL14D;_LL150:;_LL151: goto
_LL14D;_LL14D:;}goto _LL148;_LL14B:;_LL14C: goto _LL148;_LL148:;}return t;}}static
void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){void**_tmp213=0;struct
Cyc_Absyn_Tqual _tmp214=Cyc_Absyn_empty_tqual(0);if(topt != 0){void*_tmp215=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp216;void*_tmp217;struct Cyc_Absyn_Tqual _tmp218;
struct Cyc_Absyn_PtrAtts _tmp219;struct Cyc_Absyn_Conref*_tmp21A;_LL153: if(_tmp215
<= (void*)4)goto _LL155;if(*((int*)_tmp215)!= 4)goto _LL155;_tmp216=((struct Cyc_Absyn_PointerType_struct*)
_tmp215)->f1;_tmp217=(void*)_tmp216.elt_typ;_tmp218=_tmp216.elt_tq;_tmp219=
_tmp216.ptr_atts;_tmp21A=_tmp219.zero_term;_LL154: _tmp213=({void**_tmp21B=
_cycalloc(sizeof(*_tmp21B));_tmp21B[0]=_tmp217;_tmp21B;});_tmp214=_tmp218;goto
_LL152;_LL155:;_LL156: goto _LL152;_LL152:;}{struct _RegionHandle _tmp21C=
_new_region("r");struct _RegionHandle*r=& _tmp21C;_push_region(r);Cyc_Tcexp_tcExpNoInst(
Cyc_Tcenv_clear_notreadctxt(r,te),_tmp213,e);;_pop_region(r);}{void*_tmp21D=(
void*)e->r;struct Cyc_Absyn_Exp*_tmp21E;struct Cyc_Absyn_Exp*_tmp21F;_LL158: if(*((
int*)_tmp21D)!= 25)goto _LL15A;_tmp21E=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp21D)->f1;_tmp21F=((struct Cyc_Absyn_Subscript_e_struct*)_tmp21D)->f2;_LL159:{
void*_tmp220=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp21E->topt))->v);_LL15D: if(_tmp220 <= (void*)4)goto _LL15F;if(*((int*)_tmp220)
!= 9)goto _LL15F;_LL15E: goto _LL15C;_LL15F:;_LL160:(void*)(e0->r=(void*)((void*)(
Cyc_Absyn_add_exp(_tmp21E,_tmp21F,0))->r));return Cyc_Tcexp_tcPlus(te,_tmp21E,
_tmp21F);_LL15C:;}goto _LL157;_LL15A:;_LL15B: goto _LL157;_LL157:;}{int _tmp222;void*
_tmp223;struct _tuple6 _tmp221=Cyc_Tcutil_addressof_props(te,e);_tmp222=_tmp221.f1;
_tmp223=_tmp221.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp222){
tq.print_const=1;tq.real_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp223,tq,Cyc_Absyn_false_conref);return t;}}}}static
void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*t){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,t);if(!Cyc_Evexp_okay_szofarg(t))({struct Cyc_String_pa_struct _tmp226;
_tmp226.tag=0;_tmp226.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp224[1]={& _tmp226};Cyc_Tcutil_terr(loc,({const char*_tmp225="sizeof applied to type %s, which has unknown size here";
_tag_dynforward(_tmp225,sizeof(char),_get_zero_arr_size(_tmp225,55));}),
_tag_dynforward(_tmp224,sizeof(void*),1));}});if(topt != 0){void*_tmp227=Cyc_Tcutil_compress(*
topt);_LL162: if(_tmp227 <= (void*)4)goto _LL164;if(*((int*)_tmp227)!= 14)goto
_LL164;_LL163: return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmp228=_cycalloc(
sizeof(*_tmp228));_tmp228[0]=({struct Cyc_Absyn_SizeofType_struct _tmp229;_tmp229.tag=
14;_tmp229.f1=(void*)t;_tmp229;});_tmp228;});_LL164:;_LL165: goto _LL161;_LL161:;}
return Cyc_Absyn_uint_typ;}int Cyc_Tcexp_structfield_has_name(struct
_dynforward_ptr*n,struct Cyc_Absyn_Aggrfield*sf){return Cyc_strcmp((struct
_dynforward_ptr)*n,(struct _dynforward_ptr)*sf->name)== 0;}static void*Cyc_Tcexp_tcOffsetof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,void*n){
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);{void*
_tmp22A=n;struct _dynforward_ptr*_tmp22B;unsigned int _tmp22C;_LL167: if(*((int*)
_tmp22A)!= 0)goto _LL169;_tmp22B=((struct Cyc_Absyn_StructField_struct*)_tmp22A)->f1;
_LL168: {int bad_type=1;{void*_tmp22D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp22E;void*_tmp22F;struct Cyc_Absyn_Aggrdecl**_tmp230;struct Cyc_List_List*
_tmp231;_LL16C: if(_tmp22D <= (void*)4)goto _LL170;if(*((int*)_tmp22D)!= 10)goto
_LL16E;_tmp22E=((struct Cyc_Absyn_AggrType_struct*)_tmp22D)->f1;_tmp22F=(void*)
_tmp22E.aggr_info;if(*((int*)_tmp22F)!= 1)goto _LL16E;_tmp230=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp22F)->f1;_LL16D: if((*_tmp230)->impl == 0)goto _LL16B;if(!((int(*)(int(*pred)(
struct _dynforward_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dynforward_ptr*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp22B,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp230)->impl))->fields))({struct
Cyc_String_pa_struct _tmp234;_tmp234.tag=0;_tmp234.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*_tmp22B);{void*_tmp232[1]={& _tmp234};Cyc_Tcutil_terr(loc,({
const char*_tmp233="no field of struct/union has name %s";_tag_dynforward(_tmp233,
sizeof(char),_get_zero_arr_size(_tmp233,37));}),_tag_dynforward(_tmp232,sizeof(
void*),1));}});bad_type=0;goto _LL16B;_LL16E: if(*((int*)_tmp22D)!= 11)goto _LL170;
_tmp231=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp22D)->f2;_LL16F: if(!((int(*)(
int(*pred)(struct _dynforward_ptr*,struct Cyc_Absyn_Aggrfield*),struct
_dynforward_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp22B,_tmp231))({struct Cyc_String_pa_struct _tmp237;_tmp237.tag=0;_tmp237.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp22B);{void*_tmp235[1]={&
_tmp237};Cyc_Tcutil_terr(loc,({const char*_tmp236="no field of struct/union has name %s";
_tag_dynforward(_tmp236,sizeof(char),_get_zero_arr_size(_tmp236,37));}),
_tag_dynforward(_tmp235,sizeof(void*),1));}});bad_type=0;goto _LL16B;_LL170:;
_LL171: goto _LL16B;_LL16B:;}if(bad_type)({struct Cyc_String_pa_struct _tmp23A;
_tmp23A.tag=0;_tmp23A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp238[1]={& _tmp23A};Cyc_Tcutil_terr(loc,({const char*_tmp239="%s is not a known struct/union type";
_tag_dynforward(_tmp239,sizeof(char),_get_zero_arr_size(_tmp239,36));}),
_tag_dynforward(_tmp238,sizeof(void*),1));}});goto _LL166;}_LL169: if(*((int*)
_tmp22A)!= 1)goto _LL166;_tmp22C=((struct Cyc_Absyn_TupleIndex_struct*)_tmp22A)->f1;
_LL16A: {int bad_type=1;{void*_tmp23B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp23C;void*_tmp23D;struct Cyc_Absyn_Aggrdecl**_tmp23E;struct Cyc_List_List*
_tmp23F;struct Cyc_List_List*_tmp240;struct Cyc_Absyn_TunionFieldInfo _tmp241;void*
_tmp242;struct Cyc_Absyn_Tunionfield*_tmp243;_LL173: if(_tmp23B <= (void*)4)goto
_LL17B;if(*((int*)_tmp23B)!= 10)goto _LL175;_tmp23C=((struct Cyc_Absyn_AggrType_struct*)
_tmp23B)->f1;_tmp23D=(void*)_tmp23C.aggr_info;if(*((int*)_tmp23D)!= 1)goto _LL175;
_tmp23E=((struct Cyc_Absyn_KnownAggr_struct*)_tmp23D)->f1;_LL174: if((*_tmp23E)->impl
== 0)goto _LL172;_tmp23F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp23E)->impl))->fields;
goto _LL176;_LL175: if(*((int*)_tmp23B)!= 11)goto _LL177;_tmp23F=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp23B)->f2;_LL176: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp23F)
<= _tmp22C)({struct Cyc_Int_pa_struct _tmp247;_tmp247.tag=1;_tmp247.f1=(
unsigned long)((int)_tmp22C);{struct Cyc_Int_pa_struct _tmp246;_tmp246.tag=1;
_tmp246.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp23F);{void*_tmp244[2]={& _tmp246,& _tmp247};Cyc_Tcutil_terr(loc,({const char*
_tmp245="struct/union has too few components: %d <= %d";_tag_dynforward(_tmp245,
sizeof(char),_get_zero_arr_size(_tmp245,46));}),_tag_dynforward(_tmp244,sizeof(
void*),2));}}});bad_type=0;goto _LL172;_LL177: if(*((int*)_tmp23B)!= 9)goto _LL179;
_tmp240=((struct Cyc_Absyn_TupleType_struct*)_tmp23B)->f1;_LL178: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp240)<= _tmp22C)({struct Cyc_Int_pa_struct
_tmp24B;_tmp24B.tag=1;_tmp24B.f1=(unsigned long)((int)_tmp22C);{struct Cyc_Int_pa_struct
_tmp24A;_tmp24A.tag=1;_tmp24A.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp240);{void*_tmp248[2]={& _tmp24A,& _tmp24B};Cyc_Tcutil_terr(
loc,({const char*_tmp249="tuple has too few components: %d <= %d";_tag_dynforward(
_tmp249,sizeof(char),_get_zero_arr_size(_tmp249,39));}),_tag_dynforward(_tmp248,
sizeof(void*),2));}}});bad_type=0;goto _LL172;_LL179: if(*((int*)_tmp23B)!= 3)goto
_LL17B;_tmp241=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp23B)->f1;_tmp242=(
void*)_tmp241.field_info;if(*((int*)_tmp242)!= 1)goto _LL17B;_tmp243=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp242)->f2;_LL17A: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp243->typs)
< _tmp22C)({struct Cyc_Int_pa_struct _tmp24F;_tmp24F.tag=1;_tmp24F.f1=(
unsigned long)((int)_tmp22C);{struct Cyc_Int_pa_struct _tmp24E;_tmp24E.tag=1;
_tmp24E.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp243->typs);{void*_tmp24C[2]={& _tmp24E,& _tmp24F};Cyc_Tcutil_terr(loc,({const
char*_tmp24D="tunionfield has too few components: %d < %d";_tag_dynforward(
_tmp24D,sizeof(char),_get_zero_arr_size(_tmp24D,44));}),_tag_dynforward(_tmp24C,
sizeof(void*),2));}}});bad_type=0;goto _LL172;_LL17B:;_LL17C: goto _LL172;_LL172:;}
if(bad_type)({struct Cyc_String_pa_struct _tmp252;_tmp252.tag=0;_tmp252.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp250[
1]={& _tmp252};Cyc_Tcutil_terr(loc,({const char*_tmp251="%s is not a known type";
_tag_dynforward(_tmp251,sizeof(char),_get_zero_arr_size(_tmp251,23));}),
_tag_dynforward(_tmp250,sizeof(void*),1));}});goto _LL166;}_LL166:;}return Cyc_Absyn_uint_typ;}
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){struct _RegionHandle _tmp253=_new_region("r");
struct _RegionHandle*r=& _tmp253;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp254=Cyc_Tcenv_clear_notreadctxt(
r,te_orig);Cyc_Tcexp_tcExp(_tmp254,0,e);{void*t=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp255=t;struct Cyc_Absyn_PtrInfo
_tmp256;void*_tmp257;struct Cyc_Absyn_PtrAtts _tmp258;void*_tmp259;struct Cyc_Absyn_Conref*
_tmp25A;struct Cyc_Absyn_Conref*_tmp25B;_LL17E: if(_tmp255 <= (void*)4)goto _LL180;
if(*((int*)_tmp255)!= 4)goto _LL180;_tmp256=((struct Cyc_Absyn_PointerType_struct*)
_tmp255)->f1;_tmp257=(void*)_tmp256.elt_typ;_tmp258=_tmp256.ptr_atts;_tmp259=(
void*)_tmp258.rgn;_tmp25A=_tmp258.bounds;_tmp25B=_tmp258.zero_term;_LL17F: Cyc_Tcenv_check_rgn_accessible(
_tmp254,loc,_tmp259);Cyc_Tcutil_check_nonzero_bound(loc,_tmp25A);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp257),(void*)1))({void*_tmp25C[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp25D="can't dereference abstract pointer type";_tag_dynforward(
_tmp25D,sizeof(char),_get_zero_arr_size(_tmp25D,40));}),_tag_dynforward(_tmp25C,
sizeof(void*),0));});{void*_tmp25E=_tmp257;_npop_handler(0);return _tmp25E;}
_LL180:;_LL181: {void*_tmp262=({struct Cyc_String_pa_struct _tmp261;_tmp261.tag=0;
_tmp261.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp25F[1]={& _tmp261};Cyc_Tcexp_expr_err(_tmp254,loc,topt,({const char*
_tmp260="expecting * or @ type but found %s";_tag_dynforward(_tmp260,sizeof(char),
_get_zero_arr_size(_tmp260,35));}),_tag_dynforward(_tmp25F,sizeof(void*),1));}});
_npop_handler(0);return _tmp262;}_LL17D:;}};_pop_region(r);}static void*Cyc_Tcexp_tcAggrMember(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
outer_e,struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*f){Cyc_Tcexp_tcExpNoPromote(
te,0,e);{void*_tmp263=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo _tmp264;void*_tmp265;struct Cyc_Absyn_Aggrdecl**
_tmp266;struct Cyc_Absyn_Aggrdecl*_tmp267;struct Cyc_List_List*_tmp268;void*
_tmp269;struct Cyc_List_List*_tmp26A;_LL183: if(_tmp263 <= (void*)4)goto _LL18B;if(*((
int*)_tmp263)!= 10)goto _LL185;_tmp264=((struct Cyc_Absyn_AggrType_struct*)_tmp263)->f1;
_tmp265=(void*)_tmp264.aggr_info;if(*((int*)_tmp265)!= 1)goto _LL185;_tmp266=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp265)->f1;_tmp267=*_tmp266;_tmp268=_tmp264.targs;
_LL184: {struct Cyc_Absyn_Aggrfield*_tmp26D=Cyc_Absyn_lookup_decl_field(_tmp267,f);
if(_tmp26D == 0)return({struct Cyc_String_pa_struct _tmp271;_tmp271.tag=0;_tmp271.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*f);{struct Cyc_String_pa_struct
_tmp270;_tmp270.tag=0;_tmp270.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp267->name));{void*_tmp26E[2]={& _tmp270,& _tmp271};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp26F="type %s has no %s field";_tag_dynforward(
_tmp26F,sizeof(char),_get_zero_arr_size(_tmp26F,24));}),_tag_dynforward(_tmp26E,
sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp267->impl))->exist_vars
!= 0)return({void*_tmp272[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp273="must use pattern-matching to access fields of existential types";
_tag_dynforward(_tmp273,sizeof(char),_get_zero_arr_size(_tmp273,64));}),
_tag_dynforward(_tmp272,sizeof(void*),0));});{void*t2;{struct _RegionHandle
_tmp274=_new_region("rgn");struct _RegionHandle*rgn=& _tmp274;_push_region(rgn);{
struct Cyc_List_List*_tmp275=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
rgn,rgn,_tmp267->tvs,_tmp268);t2=Cyc_Tcutil_rsubstitute(rgn,_tmp275,(void*)
_tmp26D->type);if(((void*)_tmp267->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only(t2)){void*_tmp279=({struct Cyc_String_pa_struct
_tmp278;_tmp278.tag=0;_tmp278.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
f);{void*_tmp276[1]={& _tmp278};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp277="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp277,sizeof(char),_get_zero_arr_size(_tmp277,56));}),
_tag_dynforward(_tmp276,sizeof(void*),1));}});_npop_handler(0);return _tmp279;}};
_pop_region(rgn);}return t2;}}_LL185: if(*((int*)_tmp263)!= 11)goto _LL187;_tmp269=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp263)->f1;_tmp26A=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp263)->f2;_LL186: {struct Cyc_Absyn_Aggrfield*_tmp27A=Cyc_Absyn_lookup_field(
_tmp26A,f);if(_tmp27A == 0)return({struct Cyc_String_pa_struct _tmp27D;_tmp27D.tag=
0;_tmp27D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp27B[1]={&
_tmp27D};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp27C="type has no %s field";
_tag_dynforward(_tmp27C,sizeof(char),_get_zero_arr_size(_tmp27C,21));}),
_tag_dynforward(_tmp27B,sizeof(void*),1));}});if((_tmp269 == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only((void*)_tmp27A->type))return({struct Cyc_String_pa_struct
_tmp280;_tmp280.tag=0;_tmp280.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
f);{void*_tmp27E[1]={& _tmp280};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp27F="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp27F,sizeof(char),_get_zero_arr_size(_tmp27F,56));}),
_tag_dynforward(_tmp27E,sizeof(void*),1));}});return(void*)_tmp27A->type;}_LL187:
if(*((int*)_tmp263)!= 7)goto _LL189;if(!(Cyc_strcmp((struct _dynforward_ptr)*f,({
const char*_tmp26B="size";_tag_dynforward(_tmp26B,sizeof(char),_get_zero_arr_size(
_tmp26B,5));}))== 0))goto _LL189;_LL188: goto _LL18A;_LL189: if(*((int*)_tmp263)!= 4)
goto _LL18B;if(!(Cyc_strcmp((struct _dynforward_ptr)*f,({const char*_tmp26C="size";
_tag_dynforward(_tmp26C,sizeof(char),_get_zero_arr_size(_tmp26C,5));}))== 0))
goto _LL18B;_LL18A:(void*)(outer_e->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp281=_cycalloc(sizeof(*_tmp281));_tmp281[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp282;_tmp282.tag=3;_tmp282.f1=(void*)((void*)19);_tmp282.f2=({struct Cyc_List_List*
_tmp283=_cycalloc(sizeof(*_tmp283));_tmp283->hd=e;_tmp283->tl=0;_tmp283;});
_tmp282;});_tmp281;})));return Cyc_Absyn_uint_typ;_LL18B:;_LL18C: if(Cyc_strcmp((
struct _dynforward_ptr)*f,({const char*_tmp284="size";_tag_dynforward(_tmp284,
sizeof(char),_get_zero_arr_size(_tmp284,5));}))== 0)return({struct Cyc_String_pa_struct
_tmp287;_tmp287.tag=0;_tmp287.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*_tmp285[1]={& _tmp287};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp286="expecting struct, union, or array, found %s";
_tag_dynforward(_tmp286,sizeof(char),_get_zero_arr_size(_tmp286,44));}),
_tag_dynforward(_tmp285,sizeof(void*),1));}});else{return({struct Cyc_String_pa_struct
_tmp28A;_tmp28A.tag=0;_tmp28A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*_tmp288[1]={& _tmp28A};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp289="expecting struct or union, found %s";
_tag_dynforward(_tmp289,sizeof(char),_get_zero_arr_size(_tmp289,36));}),
_tag_dynforward(_tmp288,sizeof(void*),1));}});}_LL182:;}}static void*Cyc_Tcexp_tcAggrArrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct _dynforward_ptr*f){{struct _RegionHandle _tmp28B=_new_region("r");struct
_RegionHandle*r=& _tmp28B;_push_region(r);Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_notreadctxt(
r,te),0,e);;_pop_region(r);}{void*_tmp28C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp28D;void*_tmp28E;struct Cyc_Absyn_PtrAtts
_tmp28F;void*_tmp290;struct Cyc_Absyn_Conref*_tmp291;struct Cyc_Absyn_Conref*
_tmp292;_LL18E: if(_tmp28C <= (void*)4)goto _LL190;if(*((int*)_tmp28C)!= 4)goto
_LL190;_tmp28D=((struct Cyc_Absyn_PointerType_struct*)_tmp28C)->f1;_tmp28E=(void*)
_tmp28D.elt_typ;_tmp28F=_tmp28D.ptr_atts;_tmp290=(void*)_tmp28F.rgn;_tmp291=
_tmp28F.bounds;_tmp292=_tmp28F.zero_term;_LL18F: Cyc_Tcutil_check_nonzero_bound(
loc,_tmp291);{void*_tmp293=Cyc_Tcutil_compress(_tmp28E);struct Cyc_Absyn_AggrInfo
_tmp294;void*_tmp295;struct Cyc_Absyn_Aggrdecl**_tmp296;struct Cyc_Absyn_Aggrdecl*
_tmp297;struct Cyc_List_List*_tmp298;void*_tmp299;struct Cyc_List_List*_tmp29A;
_LL193: if(_tmp293 <= (void*)4)goto _LL197;if(*((int*)_tmp293)!= 10)goto _LL195;
_tmp294=((struct Cyc_Absyn_AggrType_struct*)_tmp293)->f1;_tmp295=(void*)_tmp294.aggr_info;
if(*((int*)_tmp295)!= 1)goto _LL195;_tmp296=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp295)->f1;_tmp297=*_tmp296;_tmp298=_tmp294.targs;_LL194: {struct Cyc_Absyn_Aggrfield*
_tmp29B=Cyc_Absyn_lookup_decl_field(_tmp297,f);if(_tmp29B == 0)return({struct Cyc_String_pa_struct
_tmp29F;_tmp29F.tag=0;_tmp29F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
f);{struct Cyc_String_pa_struct _tmp29E;_tmp29E.tag=0;_tmp29E.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp297->name));{
void*_tmp29C[2]={& _tmp29E,& _tmp29F};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp29D="type %s has no %s field";_tag_dynforward(_tmp29D,sizeof(char),
_get_zero_arr_size(_tmp29D,24));}),_tag_dynforward(_tmp29C,sizeof(void*),2));}}});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp297->impl))->exist_vars != 0)
return({void*_tmp2A0[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2A1="must use pattern-matching to access fields of existential types";
_tag_dynforward(_tmp2A1,sizeof(char),_get_zero_arr_size(_tmp2A1,64));}),
_tag_dynforward(_tmp2A0,sizeof(void*),0));});{void*t3;{struct _RegionHandle
_tmp2A2=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2A2;_push_region(rgn);{
struct Cyc_List_List*_tmp2A3=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
rgn,rgn,_tmp297->tvs,_tmp298);t3=Cyc_Tcutil_rsubstitute(rgn,_tmp2A3,(void*)
_tmp29B->type);};_pop_region(rgn);}if(((void*)_tmp297->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only(t3))return({struct Cyc_String_pa_struct _tmp2A6;
_tmp2A6.tag=0;_tmp2A6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{
void*_tmp2A4[1]={& _tmp2A6};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2A5="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp2A5,sizeof(char),_get_zero_arr_size(_tmp2A5,56));}),
_tag_dynforward(_tmp2A4,sizeof(void*),1));}});return t3;}}_LL195: if(*((int*)
_tmp293)!= 11)goto _LL197;_tmp299=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp293)->f1;_tmp29A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp293)->f2;_LL196: {
struct Cyc_Absyn_Aggrfield*_tmp2A7=Cyc_Absyn_lookup_field(_tmp29A,f);if(_tmp2A7 == 
0)return({struct Cyc_String_pa_struct _tmp2AA;_tmp2AA.tag=0;_tmp2AA.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp2A8[1]={& _tmp2AA};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2A9="type has no %s field";_tag_dynforward(_tmp2A9,
sizeof(char),_get_zero_arr_size(_tmp2A9,21));}),_tag_dynforward(_tmp2A8,sizeof(
void*),1));}});if((_tmp299 == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp2A7->type))return({struct Cyc_String_pa_struct _tmp2AD;_tmp2AD.tag=0;
_tmp2AD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp2AB[1]={&
_tmp2AD};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2AC="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp2AC,sizeof(char),_get_zero_arr_size(_tmp2AC,56));}),
_tag_dynforward(_tmp2AB,sizeof(void*),1));}});return(void*)_tmp2A7->type;}_LL197:;
_LL198: goto _LL192;_LL192:;}goto _LL18D;_LL190:;_LL191: goto _LL18D;_LL18D:;}return({
struct Cyc_String_pa_struct _tmp2B0;_tmp2B0.tag=0;_tmp2B0.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp2AE[1]={& _tmp2B0};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2AF="expecting struct or union pointer, found %s";
_tag_dynforward(_tmp2AF,sizeof(char),_get_zero_arr_size(_tmp2AF,44));}),
_tag_dynforward(_tmp2AE,sizeof(void*),1));}});}static void*Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,
struct Cyc_Absyn_Exp*index){unsigned int _tmp2B2;int _tmp2B3;struct _tuple7 _tmp2B1=
Cyc_Evexp_eval_const_uint_exp(index);_tmp2B2=_tmp2B1.f1;_tmp2B3=_tmp2B1.f2;if(!
_tmp2B3)return({void*_tmp2B4[0]={};Cyc_Tcexp_expr_err(te,loc,0,({const char*
_tmp2B5="tuple projection cannot use sizeof or offsetof";_tag_dynforward(_tmp2B5,
sizeof(char),_get_zero_arr_size(_tmp2B5,47));}),_tag_dynforward(_tmp2B4,sizeof(
void*),0));});{struct _handler_cons _tmp2B6;_push_handler(& _tmp2B6);{int _tmp2B8=0;
if(setjmp(_tmp2B6.handler))_tmp2B8=1;if(!_tmp2B8){{void*_tmp2B9=(*((struct
_tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2B2)).f2;
_npop_handler(0);return _tmp2B9;};_pop_handler();}else{void*_tmp2B7=(void*)
_exn_thrown;void*_tmp2BB=_tmp2B7;_LL19A: if(_tmp2BB != Cyc_List_Nth)goto _LL19C;
_LL19B: return({struct Cyc_Int_pa_struct _tmp2BF;_tmp2BF.tag=1;_tmp2BF.f1=(
unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);{struct Cyc_Int_pa_struct
_tmp2BE;_tmp2BE.tag=1;_tmp2BE.f1=(unsigned long)((int)_tmp2B2);{void*_tmp2BC[2]={&
_tmp2BE,& _tmp2BF};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp2BD="index is %d but tuple has only %d fields";
_tag_dynforward(_tmp2BD,sizeof(char),_get_zero_arr_size(_tmp2BD,41));}),
_tag_dynforward(_tmp2BC,sizeof(void*),2));}}});_LL19C:;_LL19D:(void)_throw(
_tmp2BB);_LL199:;}}}}static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*
te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){struct _RegionHandle _tmp2C0=_new_region("r");struct
_RegionHandle*r=& _tmp2C0;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp2C1=Cyc_Tcenv_clear_notreadctxt(
r,te_orig);Cyc_Tcexp_tcExp(_tmp2C1,0,e1);Cyc_Tcexp_tcExp(_tmp2C1,0,e2);{void*t1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*
t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!Cyc_Tcutil_coerce_sint_typ(_tmp2C1,e2)){void*_tmp2C5=({struct Cyc_String_pa_struct
_tmp2C4;_tmp2C4.tag=0;_tmp2C4.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{void*_tmp2C2[1]={& _tmp2C4};Cyc_Tcexp_expr_err(
_tmp2C1,e2->loc,topt,({const char*_tmp2C3="expecting int subscript, found %s";
_tag_dynforward(_tmp2C3,sizeof(char),_get_zero_arr_size(_tmp2C3,34));}),
_tag_dynforward(_tmp2C2,sizeof(void*),1));}});_npop_handler(0);return _tmp2C5;}{
void*_tmp2C6=t1;struct Cyc_Absyn_PtrInfo _tmp2C7;void*_tmp2C8;struct Cyc_Absyn_Tqual
_tmp2C9;struct Cyc_Absyn_PtrAtts _tmp2CA;void*_tmp2CB;struct Cyc_Absyn_Conref*
_tmp2CC;struct Cyc_Absyn_Conref*_tmp2CD;struct Cyc_List_List*_tmp2CE;_LL19F: if(
_tmp2C6 <= (void*)4)goto _LL1A3;if(*((int*)_tmp2C6)!= 4)goto _LL1A1;_tmp2C7=((
struct Cyc_Absyn_PointerType_struct*)_tmp2C6)->f1;_tmp2C8=(void*)_tmp2C7.elt_typ;
_tmp2C9=_tmp2C7.elt_tq;_tmp2CA=_tmp2C7.ptr_atts;_tmp2CB=(void*)_tmp2CA.rgn;
_tmp2CC=_tmp2CA.bounds;_tmp2CD=_tmp2CA.zero_term;_LL1A0: if(((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2CD)){int emit_warning=1;{void*
_tmp2CF=(void*)(Cyc_Absyn_compress_conref(_tmp2CC))->v;void*_tmp2D0;struct Cyc_Absyn_Exp*
_tmp2D1;void*_tmp2D2;void*_tmp2D3;_LL1A6: if(_tmp2CF <= (void*)1)goto _LL1AC;if(*((
int*)_tmp2CF)!= 0)goto _LL1A8;_tmp2D0=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2CF)->f1;if(_tmp2D0 <= (void*)2)goto _LL1A8;if(*((int*)_tmp2D0)!= 0)goto _LL1A8;
_tmp2D1=((struct Cyc_Absyn_Upper_b_struct*)_tmp2D0)->f1;_LL1A7: if(Cyc_Tcutil_is_const_exp(
_tmp2C1,e2)){unsigned int _tmp2D5;int _tmp2D6;struct _tuple7 _tmp2D4=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp2D5=_tmp2D4.f1;_tmp2D6=_tmp2D4.f2;if(_tmp2D6){unsigned int _tmp2D8;int
_tmp2D9;struct _tuple7 _tmp2D7=Cyc_Evexp_eval_const_uint_exp(_tmp2D1);_tmp2D8=
_tmp2D7.f1;_tmp2D9=_tmp2D7.f2;if(_tmp2D9  && _tmp2D8 > _tmp2D5)emit_warning=0;}}
goto _LL1A5;_LL1A8: if(*((int*)_tmp2CF)!= 0)goto _LL1AA;_tmp2D2=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2CF)->f1;if((int)_tmp2D2 != 0)goto _LL1AA;_LL1A9: emit_warning=0;goto _LL1A5;
_LL1AA: if(*((int*)_tmp2CF)!= 0)goto _LL1AC;_tmp2D3=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2CF)->f1;if((int)_tmp2D3 != 1)goto _LL1AC;_LL1AB: emit_warning=0;goto _LL1A5;
_LL1AC:;_LL1AD: goto _LL1A5;_LL1A5:;}if(emit_warning)({void*_tmp2DA[0]={};Cyc_Tcutil_warn(
e2->loc,({const char*_tmp2DB="subscript on thin, zero-terminated pointer could be expensive.";
_tag_dynforward(_tmp2DB,sizeof(char),_get_zero_arr_size(_tmp2DB,63));}),
_tag_dynforward(_tmp2DA,sizeof(void*),0));});}else{if(Cyc_Tcutil_is_const_exp(
_tmp2C1,e2)){unsigned int _tmp2DD;int _tmp2DE;struct _tuple7 _tmp2DC=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp2DD=_tmp2DC.f1;_tmp2DE=_tmp2DC.f2;if(_tmp2DE)Cyc_Tcutil_check_bound(loc,
_tmp2DD,_tmp2CC);}else{if(Cyc_Tcutil_is_bound_one(_tmp2CC) && !((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2CD))({void*_tmp2DF[0]={};
Cyc_Tcutil_warn(e1->loc,({const char*_tmp2E0="subscript applied to pointer to one object";
_tag_dynforward(_tmp2E0,sizeof(char),_get_zero_arr_size(_tmp2E0,43));}),
_tag_dynforward(_tmp2DF,sizeof(void*),0));});Cyc_Tcutil_check_nonzero_bound(loc,
_tmp2CC);}}Cyc_Tcenv_check_rgn_accessible(_tmp2C1,loc,_tmp2CB);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp2C8),(void*)1))({void*_tmp2E1[0]={};Cyc_Tcutil_terr(e1->loc,({
const char*_tmp2E2="can't subscript an abstract pointer";_tag_dynforward(_tmp2E2,
sizeof(char),_get_zero_arr_size(_tmp2E2,36));}),_tag_dynforward(_tmp2E1,sizeof(
void*),0));});{void*_tmp2E3=_tmp2C8;_npop_handler(0);return _tmp2E3;}_LL1A1: if(*((
int*)_tmp2C6)!= 9)goto _LL1A3;_tmp2CE=((struct Cyc_Absyn_TupleType_struct*)_tmp2C6)->f1;
_LL1A2: {void*_tmp2E4=Cyc_Tcexp_ithTupleType(_tmp2C1,loc,_tmp2CE,e2);
_npop_handler(0);return _tmp2E4;}_LL1A3:;_LL1A4: {void*_tmp2E8=({struct Cyc_String_pa_struct
_tmp2E7;_tmp2E7.tag=0;_tmp2E7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp2E5[1]={& _tmp2E7};Cyc_Tcexp_expr_err(
_tmp2C1,loc,topt,({const char*_tmp2E6="subscript applied to %s";_tag_dynforward(
_tmp2E6,sizeof(char),_get_zero_arr_size(_tmp2E6,24));}),_tag_dynforward(_tmp2E5,
sizeof(void*),1));}});_npop_handler(0);return _tmp2E8;}_LL19E:;}}};_pop_region(r);}
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp2E9=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp2EA;
_LL1AF: if(_tmp2E9 <= (void*)4)goto _LL1B1;if(*((int*)_tmp2E9)!= 9)goto _LL1B1;
_tmp2EA=((struct Cyc_Absyn_TupleType_struct*)_tmp2E9)->f1;_LL1B0: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp2EA)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1AE;for(0;es != 0;(es=es->tl,_tmp2EA=_tmp2EA->tl)){
void*_tmp2EB=(*((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp2EA))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp2EB,(struct Cyc_Absyn_Exp*)es->hd);Cyc_Tcutil_coerce_arg(
te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple9*)_tmp2EA->hd)).f2);fields=({
struct Cyc_List_List*_tmp2EC=_cycalloc(sizeof(*_tmp2EC));_tmp2EC->hd=({struct
_tuple9*_tmp2ED=_cycalloc(sizeof(*_tmp2ED));_tmp2ED->f1=(*((struct _tuple9*)
_tmp2EA->hd)).f1;_tmp2ED->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp2ED;});_tmp2EC->tl=fields;_tmp2EC;});}done=
1;goto _LL1AE;_LL1B1:;_LL1B2: goto _LL1AE;_LL1AE:;}if(!done)for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*
_tmp2EE=_cycalloc(sizeof(*_tmp2EE));_tmp2EE->hd=({struct _tuple9*_tmp2EF=
_cycalloc(sizeof(*_tmp2EF));_tmp2EF->f1=Cyc_Absyn_empty_tqual(0);_tmp2EF->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp2EF;});_tmp2EE->tl=fields;_tmp2EE;});}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp2F0=_cycalloc(sizeof(*_tmp2F0));_tmp2F0[0]=({struct Cyc_Absyn_TupleType_struct
_tmp2F1;_tmp2F1.tag=9;_tmp2F1.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(fields);_tmp2F1;});_tmp2F0;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*
t,struct Cyc_List_List*des){return({void*_tmp2F2[0]={};Cyc_Tcexp_expr_err(te,loc,
topt,({const char*_tmp2F3="tcCompoundLit";_tag_dynforward(_tmp2F3,sizeof(char),
_get_zero_arr_size(_tmp2F3,14));}),_tag_dynforward(_tmp2F2,sizeof(void*),0));});}
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**elt_topt,int zero_term,struct Cyc_List_List*des){struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple8*),struct Cyc_List_List*
x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple8*))Cyc_Core_snd,des);void*
res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*
_tmp30D=_cycalloc(sizeof(*_tmp30D));_tmp30D->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp30D;}));int _tmp2F4=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);
struct Cyc_Absyn_Const_e_struct*_tmp2F5=({struct Cyc_Absyn_Const_e_struct*_tmp30A=
_cycalloc(sizeof(*_tmp30A));_tmp30A[0]=({struct Cyc_Absyn_Const_e_struct _tmp30B;
_tmp30B.tag=0;_tmp30B.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp30C;(_tmp30C.Int_c).tag=2;(_tmp30C.Int_c).f1=(void*)((void*)1);(_tmp30C.Int_c).f2=
_tmp2F4;_tmp30C;});_tmp30B;});_tmp30A;});struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((
void*)_tmp2F5,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp2F6=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp2F4 - 1);if(!Cyc_Tcutil_is_zero(
_tmp2F6))({void*_tmp2F7[0]={};Cyc_Tcutil_terr(_tmp2F6->loc,({const char*_tmp2F8="zero-terminated array doesn't end with zero.";
_tag_dynforward(_tmp2F8,sizeof(char),_get_zero_arr_size(_tmp2F8,45));}),
_tag_dynforward(_tmp2F7,sizeof(void*),0));});}sz_exp->topt=({struct Cyc_Core_Opt*
_tmp2F9=_cycalloc(sizeof(*_tmp2F9));_tmp2F9->v=(void*)Cyc_Absyn_uint_typ;_tmp2F9;});{
void*res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(0),(struct Cyc_Absyn_Exp*)
sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);{struct Cyc_List_List*
es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct
Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,es))({struct Cyc_String_pa_struct
_tmp2FC;_tmp2FC.tag=0;_tmp2FC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(res));{void*_tmp2FA[1]={& _tmp2FC};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,({const
char*_tmp2FB="elements of array do not all have the same type (%s)";
_tag_dynforward(_tmp2FB,sizeof(char),_get_zero_arr_size(_tmp2FB,53));}),
_tag_dynforward(_tmp2FA,sizeof(void*),1));}});{int offset=0;for(0;des != 0;(offset
++,des=des->tl)){struct Cyc_List_List*ds=(*((struct _tuple8*)des->hd)).f1;if(ds != 
0){void*_tmp2FD=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp2FE;_LL1B4: if(*((int*)
_tmp2FD)!= 1)goto _LL1B6;_LL1B5:({void*_tmp2FF[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp300="only array index designators are supported";_tag_dynforward(
_tmp300,sizeof(char),_get_zero_arr_size(_tmp300,43));}),_tag_dynforward(_tmp2FF,
sizeof(void*),0));});goto _LL1B3;_LL1B6: if(*((int*)_tmp2FD)!= 0)goto _LL1B3;
_tmp2FE=((struct Cyc_Absyn_ArrayElement_struct*)_tmp2FD)->f1;_LL1B7: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp2FE);{unsigned int _tmp302;int _tmp303;struct _tuple7 _tmp301=Cyc_Evexp_eval_const_uint_exp(
_tmp2FE);_tmp302=_tmp301.f1;_tmp303=_tmp301.f2;if(!_tmp303)({void*_tmp304[0]={};
Cyc_Tcutil_terr(_tmp2FE->loc,({const char*_tmp305="index designator cannot use sizeof or offsetof";
_tag_dynforward(_tmp305,sizeof(char),_get_zero_arr_size(_tmp305,47));}),
_tag_dynforward(_tmp304,sizeof(void*),0));});else{if(_tmp302 != offset)({struct
Cyc_Int_pa_struct _tmp309;_tmp309.tag=1;_tmp309.f1=(unsigned long)((int)_tmp302);{
struct Cyc_Int_pa_struct _tmp308;_tmp308.tag=1;_tmp308.f1=(unsigned long)offset;{
void*_tmp306[2]={& _tmp308,& _tmp309};Cyc_Tcutil_terr(_tmp2FE->loc,({const char*
_tmp307="expecting index designator %d but found %d";_tag_dynforward(_tmp307,
sizeof(char),_get_zero_arr_size(_tmp307,43));}),_tag_dynforward(_tmp306,sizeof(
void*),2));}}});}goto _LL1B3;}_LL1B3:;}}}return res_t2;}}static void*Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(
te,0,bound);{void*_tmp30E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);_LL1B9: if(_tmp30E <= (void*)4)goto _LL1BB;if(*((int*)
_tmp30E)!= 17)goto _LL1BB;_LL1BA: goto _LL1B8;_LL1BB:;_LL1BC: if(!Cyc_Tcutil_coerce_uint_typ(
te,bound))({struct Cyc_String_pa_struct _tmp311;_tmp311.tag=0;_tmp311.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(bound->topt))->v));{void*_tmp30F[1]={& _tmp311};Cyc_Tcutil_terr(
bound->loc,({const char*_tmp310="expecting unsigned int, found %s";
_tag_dynforward(_tmp310,sizeof(char),_get_zero_arr_size(_tmp310,33));}),
_tag_dynforward(_tmp30F,sizeof(void*),1));}});_LL1B8:;}if(!(vd->tq).real_const)({
void*_tmp312[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp313="comprehension index variable is not declared const!";
_tag_dynforward(_tmp313,sizeof(char),_get_zero_arr_size(_tmp313,52));}),
_tag_dynforward(_tmp312,sizeof(void*),0));});{struct _RegionHandle _tmp314=
_new_region("r");struct _RegionHandle*r=& _tmp314;_push_region(r);{struct Cyc_Tcenv_Tenv*
te2=Cyc_Tcenv_copy_tenv(r,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(r,loc,te2);
te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*_tmp315=te2;
struct Cyc_Absyn_PtrInfo pinfo;void**_tmp316=0;struct Cyc_Absyn_Tqual*_tmp317=0;
struct Cyc_Absyn_Conref**_tmp318=0;if(topt != 0){void*_tmp319=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp31A;struct Cyc_Absyn_ArrayInfo _tmp31B;void*
_tmp31C;void**_tmp31D;struct Cyc_Absyn_Tqual _tmp31E;struct Cyc_Absyn_Tqual*_tmp31F;
struct Cyc_Absyn_Exp*_tmp320;struct Cyc_Absyn_Conref*_tmp321;struct Cyc_Absyn_Conref**
_tmp322;_LL1BE: if(_tmp319 <= (void*)4)goto _LL1C2;if(*((int*)_tmp319)!= 4)goto
_LL1C0;_tmp31A=((struct Cyc_Absyn_PointerType_struct*)_tmp319)->f1;_LL1BF: pinfo=
_tmp31A;_tmp316=(void**)((void**)((void*)& pinfo.elt_typ));_tmp317=(struct Cyc_Absyn_Tqual*)&
pinfo.elt_tq;_tmp318=(struct Cyc_Absyn_Conref**)&(pinfo.ptr_atts).zero_term;goto
_LL1BD;_LL1C0: if(*((int*)_tmp319)!= 7)goto _LL1C2;_tmp31B=((struct Cyc_Absyn_ArrayType_struct*)
_tmp319)->f1;_tmp31C=(void*)_tmp31B.elt_type;_tmp31D=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp319)->f1).elt_type;_tmp31E=_tmp31B.tq;_tmp31F=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp319)->f1).tq;_tmp320=_tmp31B.num_elts;
_tmp321=_tmp31B.zero_term;_tmp322=(struct Cyc_Absyn_Conref**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp319)->f1).zero_term;_LL1C1: _tmp316=(void**)_tmp31D;_tmp317=(struct Cyc_Absyn_Tqual*)
_tmp31F;_tmp318=(struct Cyc_Absyn_Conref**)_tmp322;goto _LL1BD;_LL1C2:;_LL1C3: goto
_LL1BD;_LL1BD:;}{void*t=Cyc_Tcexp_tcExp(_tmp315,_tmp316,body);if(_tmp315->le == 0){
if(!Cyc_Tcutil_is_const_exp(_tmp315,bound))({void*_tmp323[0]={};Cyc_Tcutil_terr(
bound->loc,({const char*_tmp324="bound is not constant";_tag_dynforward(_tmp324,
sizeof(char),_get_zero_arr_size(_tmp324,22));}),_tag_dynforward(_tmp323,sizeof(
void*),0));});if(!Cyc_Tcutil_is_const_exp(_tmp315,body))({void*_tmp325[0]={};Cyc_Tcutil_terr(
bound->loc,({const char*_tmp326="body is not constant";_tag_dynforward(_tmp326,
sizeof(char),_get_zero_arr_size(_tmp326,21));}),_tag_dynforward(_tmp325,sizeof(
void*),0));});}if(_tmp318 != 0  && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,*_tmp318)){struct Cyc_Absyn_Exp*_tmp327=Cyc_Absyn_uint_exp(1,0);_tmp327->topt=({
struct Cyc_Core_Opt*_tmp328=_cycalloc(sizeof(*_tmp328));_tmp328->v=(void*)Cyc_Absyn_uint_typ;
_tmp328;});bound=Cyc_Absyn_add_exp(bound,_tmp327,0);bound->topt=({struct Cyc_Core_Opt*
_tmp329=_cycalloc(sizeof(*_tmp329));_tmp329->v=(void*)Cyc_Absyn_uint_typ;_tmp329;});*
is_zero_term=1;}{void*_tmp32A=Cyc_Absyn_array_typ(t,_tmp317 == 0?Cyc_Absyn_empty_tqual(
0):*_tmp317,(struct Cyc_Absyn_Exp*)bound,_tmp318 == 0?Cyc_Absyn_false_conref:*
_tmp318,0);_npop_handler(0);return _tmp32A;}}}};_pop_region(r);}}struct _tuple10{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**
tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**
ad_opt){struct Cyc_Absyn_Aggrdecl*ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)
_check_null(*ad_opt);else{{struct _handler_cons _tmp32B;_push_handler(& _tmp32B);{
int _tmp32D=0;if(setjmp(_tmp32B.handler))_tmp32D=1;if(!_tmp32D){ad=*Cyc_Tcenv_lookup_aggrdecl(
te,loc,*tn);;_pop_handler();}else{void*_tmp32C=(void*)_exn_thrown;void*_tmp32F=
_tmp32C;_LL1C5: if(_tmp32F != Cyc_Dict_Absent)goto _LL1C7;_LL1C6:({struct Cyc_String_pa_struct
_tmp332;_tmp332.tag=0;_tmp332.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(*tn));{void*_tmp330[1]={& _tmp332};Cyc_Tcutil_terr(loc,({
const char*_tmp331="unbound struct name %s";_tag_dynforward(_tmp331,sizeof(char),
_get_zero_arr_size(_tmp331,23));}),_tag_dynforward(_tmp330,sizeof(void*),1));}});
return topt != 0?*topt:(void*)0;_LL1C7:;_LL1C8:(void)_throw(_tmp32F);_LL1C4:;}}}*
ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}if((void*)ad->kind == (void*)1)({
void*_tmp333[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp334="expecting struct but found union";
_tag_dynforward(_tmp334,sizeof(char),_get_zero_arr_size(_tmp334,33));}),
_tag_dynforward(_tmp333,sizeof(void*),0));});if(ad->impl == 0){({void*_tmp335[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp336="can't build abstract struct";
_tag_dynforward(_tmp336,sizeof(char),_get_zero_arr_size(_tmp336,28));}),
_tag_dynforward(_tmp335,sizeof(void*),0));});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp337=_cycalloc(sizeof(*_tmp337));_tmp337->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp337;}));}{struct _RegionHandle _tmp338=_new_region("rgn");struct _RegionHandle*
rgn=& _tmp338;_push_region(rgn);{struct _tuple4 _tmp339=({struct _tuple4 _tmp35A;
_tmp35A.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp35A.f2=rgn;_tmp35A;});struct Cyc_List_List*
_tmp33A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct
_tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp339,ad->tvs);struct Cyc_List_List*_tmp33B=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,
struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp339,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars);struct Cyc_List_List*_tmp33C=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp33A);struct Cyc_List_List*_tmp33D=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp33B);struct Cyc_List_List*_tmp33E=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp33A,_tmp33B);struct Cyc_Absyn_AggrType_struct*_tmp33F=({struct Cyc_Absyn_AggrType_struct*
_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354[0]=({struct Cyc_Absyn_AggrType_struct
_tmp355;_tmp355.tag=10;_tmp355.f1=({struct Cyc_Absyn_AggrInfo _tmp356;_tmp356.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp357=_cycalloc(sizeof(*
_tmp357));_tmp357[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp358;_tmp358.tag=1;
_tmp358.f1=({struct Cyc_Absyn_Aggrdecl**_tmp359=_cycalloc(sizeof(*_tmp359));
_tmp359[0]=ad;_tmp359;});_tmp358;});_tmp357;}));_tmp356.targs=_tmp33C;_tmp356;});
_tmp355;});_tmp354;});struct Cyc_List_List*_tmp340=*ts;struct Cyc_List_List*
_tmp341=_tmp33D;while(_tmp340 != 0  && _tmp341 != 0){Cyc_Tcutil_check_type(loc,te,
Cyc_Tcenv_lookup_type_vars(te),(void*)0,1,(void*)_tmp340->hd);Cyc_Tcutil_unify((
void*)_tmp340->hd,(void*)_tmp341->hd);_tmp340=_tmp340->tl;_tmp341=_tmp341->tl;}
if(_tmp340 != 0)({void*_tmp342[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp343="too many explicit witness types";
_tag_dynforward(_tmp343,sizeof(char),_get_zero_arr_size(_tmp343,32));}),
_tag_dynforward(_tmp342,sizeof(void*),0));});*ts=_tmp33D;if(topt != 0)Cyc_Tcutil_unify((
void*)_tmp33F,*topt);{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);for(0;fields != 0;
fields=fields->tl){struct _tuple10 _tmp345;struct Cyc_Absyn_Aggrfield*_tmp346;
struct Cyc_Absyn_Exp*_tmp347;struct _tuple10*_tmp344=(struct _tuple10*)fields->hd;
_tmp345=*_tmp344;_tmp346=_tmp345.f1;_tmp347=_tmp345.f2;{void*_tmp348=Cyc_Tcutil_rsubstitute(
rgn,_tmp33E,(void*)_tmp346->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp348,
_tmp347);if(!Cyc_Tcutil_coerce_arg(te,_tmp347,_tmp348)){({struct Cyc_String_pa_struct
_tmp34E;_tmp34E.tag=0;_tmp34E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp347->topt))->v));{
struct Cyc_String_pa_struct _tmp34D;_tmp34D.tag=0;_tmp34D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp348));{struct
Cyc_String_pa_struct _tmp34C;_tmp34C.tag=0;_tmp34C.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(*tn));{struct Cyc_String_pa_struct
_tmp34B;_tmp34B.tag=0;_tmp34B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp346->name);{void*_tmp349[4]={& _tmp34B,& _tmp34C,& _tmp34D,& _tmp34E};Cyc_Tcutil_terr(
_tmp347->loc,({const char*_tmp34A="field %s of struct %s expects type %s != %s";
_tag_dynforward(_tmp34A,sizeof(char),_get_zero_arr_size(_tmp34A,44));}),
_tag_dynforward(_tmp349,sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();}}}{
struct Cyc_List_List*_tmp34F=0;{struct Cyc_List_List*_tmp350=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->rgn_po;for(0;_tmp350 != 0;_tmp350=_tmp350->tl){_tmp34F=({
struct Cyc_List_List*_tmp351=_cycalloc(sizeof(*_tmp351));_tmp351->hd=({struct
_tuple0*_tmp352=_cycalloc(sizeof(*_tmp352));_tmp352->f1=Cyc_Tcutil_rsubstitute(
rgn,_tmp33E,(*((struct _tuple0*)_tmp350->hd)).f1);_tmp352->f2=Cyc_Tcutil_rsubstitute(
rgn,_tmp33E,(*((struct _tuple0*)_tmp350->hd)).f2);_tmp352;});_tmp351->tl=_tmp34F;
_tmp351;});}}_tmp34F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp34F);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp34F);{void*_tmp353=(void*)
_tmp33F;_npop_handler(0);return _tmp353;}}}};_pop_region(rgn);}}static void*Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*ts,struct Cyc_List_List*
args){{struct _RegionHandle _tmp35B=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp35B;_push_region(rgn);{void*_tmp35C=Cyc_Tcutil_compress(ts);void*_tmp35D;
struct Cyc_List_List*_tmp35E;_LL1CA: if(_tmp35C <= (void*)4)goto _LL1CC;if(*((int*)
_tmp35C)!= 11)goto _LL1CC;_tmp35D=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp35C)->f1;_tmp35E=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp35C)->f2;_LL1CB:
if(_tmp35D == (void*)1)({void*_tmp35F[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp360="expecting struct but found union";_tag_dynforward(_tmp360,sizeof(char),
_get_zero_arr_size(_tmp360,33));}),_tag_dynforward(_tmp35F,sizeof(void*),0));});{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,_tmp35E);for(0;
fields != 0;fields=fields->tl){struct _tuple10 _tmp362;struct Cyc_Absyn_Aggrfield*
_tmp363;struct Cyc_Absyn_Exp*_tmp364;struct _tuple10*_tmp361=(struct _tuple10*)
fields->hd;_tmp362=*_tmp361;_tmp363=_tmp362.f1;_tmp364=_tmp362.f2;Cyc_Tcexp_tcExpInitializer(
te,(void**)((void**)((void*)& _tmp363->type)),_tmp364);if(!Cyc_Tcutil_coerce_arg(
te,_tmp364,(void*)_tmp363->type)){({struct Cyc_String_pa_struct _tmp369;_tmp369.tag=
0;_tmp369.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp364->topt))->v));{struct Cyc_String_pa_struct
_tmp368;_tmp368.tag=0;_tmp368.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)_tmp363->type));{struct Cyc_String_pa_struct _tmp367;
_tmp367.tag=0;_tmp367.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp363->name);{
void*_tmp365[3]={& _tmp367,& _tmp368,& _tmp369};Cyc_Tcutil_terr(_tmp364->loc,({
const char*_tmp366="field %s of struct expects type %s != %s";_tag_dynforward(
_tmp366,sizeof(char),_get_zero_arr_size(_tmp366,41));}),_tag_dynforward(_tmp365,
sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}goto _LL1C9;}_LL1CC:;
_LL1CD:({void*_tmp36A[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp36B="tcAnonStruct: wrong type";
_tag_dynforward(_tmp36B,sizeof(char),_get_zero_arr_size(_tmp36B,25));}),
_tag_dynforward(_tmp36A,sizeof(void*),0));});_LL1C9:;};_pop_region(rgn);}return
ts;}static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*
tud,struct Cyc_Absyn_Tunionfield*tuf){struct _RegionHandle _tmp36C=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp36C;_push_region(rgn);{struct _tuple4 _tmp36D=({
struct _tuple4 _tmp38F;_tmp38F.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp38F.f2=rgn;
_tmp38F;});struct Cyc_List_List*_tmp36E=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp36D,tud->tvs);struct Cyc_List_List*_tmp36F=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp36E);void*res=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp38A=_cycalloc(sizeof(*_tmp38A));_tmp38A[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp38B;_tmp38B.tag=3;_tmp38B.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp38C;
_tmp38C.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp38D=_cycalloc(sizeof(*_tmp38D));_tmp38D[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp38E;_tmp38E.tag=1;_tmp38E.f1=tud;_tmp38E.f2=tuf;_tmp38E;});_tmp38D;}));
_tmp38C.targs=_tmp36F;_tmp38C;});_tmp38B;});_tmp38A;});if(topt != 0){void*_tmp370=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo _tmp371;struct Cyc_List_List*
_tmp372;struct Cyc_Core_Opt*_tmp373;_LL1CF: if(_tmp370 <= (void*)4)goto _LL1D3;if(*((
int*)_tmp370)!= 3)goto _LL1D1;_LL1D0: Cyc_Tcutil_unify(*topt,res);goto _LL1CE;
_LL1D1: if(*((int*)_tmp370)!= 2)goto _LL1D3;_tmp371=((struct Cyc_Absyn_TunionType_struct*)
_tmp370)->f1;_tmp372=_tmp371.targs;_tmp373=_tmp371.rgn;_LL1D2:{struct Cyc_List_List*
a=_tmp36F;for(0;a != 0  && _tmp372 != 0;(a=a->tl,_tmp372=_tmp372->tl)){Cyc_Tcutil_unify((
void*)a->hd,(void*)_tmp372->hd);}}if(tud->is_flat  || tuf->typs == 0  && es == 0){e->topt=({
struct Cyc_Core_Opt*_tmp374=_cycalloc(sizeof(*_tmp374));_tmp374->v=(void*)res;
_tmp374;});res=(void*)({struct Cyc_Absyn_TunionType_struct*_tmp375=_cycalloc(
sizeof(*_tmp375));_tmp375[0]=({struct Cyc_Absyn_TunionType_struct _tmp376;_tmp376.tag=
2;_tmp376.f1=({struct Cyc_Absyn_TunionInfo _tmp377;_tmp377.tunion_info=(void*)((
void*)({struct Cyc_Absyn_KnownTunion_struct*_tmp378=_cycalloc(sizeof(*_tmp378));
_tmp378[0]=({struct Cyc_Absyn_KnownTunion_struct _tmp379;_tmp379.tag=1;_tmp379.f1=({
struct Cyc_Absyn_Tuniondecl**_tmp37A=_cycalloc(sizeof(*_tmp37A));_tmp37A[0]=tud;
_tmp37A;});_tmp379;});_tmp378;}));_tmp377.targs=_tmp36F;_tmp377.rgn=_tmp373;
_tmp377;});_tmp376;});_tmp375;});Cyc_Tcutil_unchecked_cast(te,e,res,(void*)1);}
goto _LL1CE;_LL1D3:;_LL1D4: goto _LL1CE;_LL1CE:;}{struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
es->hd;void*t=Cyc_Tcutil_rsubstitute(rgn,_tmp36E,(*((struct _tuple9*)ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({
struct Cyc_String_pa_struct _tmp37F;_tmp37F.tag=0;_tmp37F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)(e->topt == 0?(struct _dynforward_ptr)({
const char*_tmp380="?";_tag_dynforward(_tmp380,sizeof(char),_get_zero_arr_size(
_tmp380,2));}): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)));{
struct Cyc_String_pa_struct _tmp37E;_tmp37E.tag=0;_tmp37E.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp37D;_tmp37D.tag=0;_tmp37D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(tuf->name));{void*_tmp37B[3]={& _tmp37D,& _tmp37E,& _tmp37F};
Cyc_Tcutil_terr(e->loc,({const char*_tmp37C="tunion constructor %s expects argument of type %s but this argument has type %s";
_tag_dynforward(_tmp37C,sizeof(char),_get_zero_arr_size(_tmp37C,80));}),
_tag_dynforward(_tmp37B,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
if(es != 0){void*_tmp384=({struct Cyc_String_pa_struct _tmp383;_tmp383.tag=0;
_tmp383.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp381[1]={& _tmp383};Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)
es->hd)->loc,topt,({const char*_tmp382="too many arguments for tunion constructor %s";
_tag_dynforward(_tmp382,sizeof(char),_get_zero_arr_size(_tmp382,45));}),
_tag_dynforward(_tmp381,sizeof(void*),1));}});_npop_handler(0);return _tmp384;}
if(ts != 0){void*_tmp388=({struct Cyc_String_pa_struct _tmp387;_tmp387.tag=0;
_tmp387.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp385[1]={& _tmp387};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp386="too few arguments for tunion constructor %s";_tag_dynforward(
_tmp386,sizeof(char),_get_zero_arr_size(_tmp386,44));}),_tag_dynforward(_tmp385,
sizeof(void*),1));}});_npop_handler(0);return _tmp388;}{void*_tmp389=res;
_npop_handler(0);return _tmp389;}}};_pop_region(rgn);}static int Cyc_Tcexp_zeroable_type(
void*t){void*_tmp390=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp391;
struct Cyc_Absyn_PtrAtts _tmp392;struct Cyc_Absyn_Conref*_tmp393;struct Cyc_Absyn_ArrayInfo
_tmp394;void*_tmp395;struct Cyc_List_List*_tmp396;struct Cyc_Absyn_AggrInfo _tmp397;
void*_tmp398;struct Cyc_List_List*_tmp399;struct Cyc_List_List*_tmp39A;_LL1D6: if((
int)_tmp390 != 0)goto _LL1D8;_LL1D7: return 1;_LL1D8: if(_tmp390 <= (void*)4)goto
_LL1E4;if(*((int*)_tmp390)!= 0)goto _LL1DA;_LL1D9: goto _LL1DB;_LL1DA: if(*((int*)
_tmp390)!= 1)goto _LL1DC;_LL1DB: goto _LL1DD;_LL1DC: if(*((int*)_tmp390)!= 2)goto
_LL1DE;_LL1DD: goto _LL1DF;_LL1DE: if(*((int*)_tmp390)!= 3)goto _LL1E0;_LL1DF: return
0;_LL1E0: if(*((int*)_tmp390)!= 4)goto _LL1E2;_tmp391=((struct Cyc_Absyn_PointerType_struct*)
_tmp390)->f1;_tmp392=_tmp391.ptr_atts;_tmp393=_tmp392.nullable;_LL1E1: return((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp393);_LL1E2: if(*((
int*)_tmp390)!= 5)goto _LL1E4;_LL1E3: goto _LL1E5;_LL1E4: if((int)_tmp390 != 1)goto
_LL1E6;_LL1E5: goto _LL1E7;_LL1E6: if(_tmp390 <= (void*)4)goto _LL200;if(*((int*)
_tmp390)!= 6)goto _LL1E8;_LL1E7: return 1;_LL1E8: if(*((int*)_tmp390)!= 7)goto _LL1EA;
_tmp394=((struct Cyc_Absyn_ArrayType_struct*)_tmp390)->f1;_tmp395=(void*)_tmp394.elt_type;
_LL1E9: return Cyc_Tcexp_zeroable_type(_tmp395);_LL1EA: if(*((int*)_tmp390)!= 8)
goto _LL1EC;_LL1EB: return 0;_LL1EC: if(*((int*)_tmp390)!= 9)goto _LL1EE;_tmp396=((
struct Cyc_Absyn_TupleType_struct*)_tmp390)->f1;_LL1ED: for(0;(unsigned int)
_tmp396;_tmp396=_tmp396->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple9*)
_tmp396->hd)).f2))return 0;}return 1;_LL1EE: if(*((int*)_tmp390)!= 10)goto _LL1F0;
_tmp397=((struct Cyc_Absyn_AggrType_struct*)_tmp390)->f1;_tmp398=(void*)_tmp397.aggr_info;
_tmp399=_tmp397.targs;_LL1EF: {struct Cyc_Absyn_Aggrdecl*_tmp39B=Cyc_Absyn_get_known_aggrdecl(
_tmp398);if(_tmp39B->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp39B->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp39C=_new_region("r");
struct _RegionHandle*r=& _tmp39C;_push_region(r);{struct Cyc_List_List*_tmp39D=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp39B->tvs,_tmp399);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp39B->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp39D,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp39E=0;_npop_handler(0);
return _tmp39E;}}}{int _tmp39F=1;_npop_handler(0);return _tmp39F;}};_pop_region(r);}}
_LL1F0: if(*((int*)_tmp390)!= 12)goto _LL1F2;_LL1F1: return 1;_LL1F2: if(*((int*)
_tmp390)!= 14)goto _LL1F4;_LL1F3: return 1;_LL1F4: if(*((int*)_tmp390)!= 17)goto
_LL1F6;_LL1F5: return 1;_LL1F6: if(*((int*)_tmp390)!= 11)goto _LL1F8;_tmp39A=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp390)->f2;_LL1F7: for(0;_tmp39A != 0;
_tmp39A=_tmp39A->tl){if(!Cyc_Tcexp_zeroable_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp39A->hd)->type))return 0;}return 1;_LL1F8: if(*((int*)_tmp390)!= 13)goto _LL1FA;
_LL1F9: return 1;_LL1FA: if(*((int*)_tmp390)!= 16)goto _LL1FC;_LL1FB: return 0;_LL1FC:
if(*((int*)_tmp390)!= 15)goto _LL1FE;_LL1FD: return 0;_LL1FE: if(*((int*)_tmp390)!= 
18)goto _LL200;_LL1FF: goto _LL201;_LL200: if((int)_tmp390 != 2)goto _LL202;_LL201:
goto _LL203;_LL202: if((int)_tmp390 != 3)goto _LL204;_LL203: goto _LL205;_LL204: if(
_tmp390 <= (void*)4)goto _LL206;if(*((int*)_tmp390)!= 19)goto _LL206;_LL205: goto
_LL207;_LL206: if(_tmp390 <= (void*)4)goto _LL208;if(*((int*)_tmp390)!= 20)goto
_LL208;_LL207: goto _LL209;_LL208: if(_tmp390 <= (void*)4)goto _LL1D5;if(*((int*)
_tmp390)!= 21)goto _LL1D5;_LL209:({struct Cyc_String_pa_struct _tmp3A2;_tmp3A2.tag=
0;_tmp3A2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp3A0[1]={& _tmp3A2};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp3A1="bad type `%s' in zeroable type";
_tag_dynforward(_tmp3A1,sizeof(char),_get_zero_arr_size(_tmp3A1,31));}),
_tag_dynforward(_tmp3A0,sizeof(void*),1));}});_LL1D5:;}static void Cyc_Tcexp_check_malloc_type(
int allow_zero,struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(
t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;if(topt != 0){void*_tmp3A3=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp3A4;void*_tmp3A5;_LL20B:
if(_tmp3A3 <= (void*)4)goto _LL20D;if(*((int*)_tmp3A3)!= 4)goto _LL20D;_tmp3A4=((
struct Cyc_Absyn_PointerType_struct*)_tmp3A3)->f1;_tmp3A5=(void*)_tmp3A4.elt_typ;
_LL20C: Cyc_Tcutil_unify(_tmp3A5,t);if(Cyc_Tcutil_bits_only(t) || allow_zero  && 
Cyc_Tcexp_zeroable_type(t))return;goto _LL20A;_LL20D:;_LL20E: goto _LL20A;_LL20A:;}({
struct Cyc_String_pa_struct _tmp3A9;_tmp3A9.tag=0;_tmp3A9.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp3A8;_tmp3A8.tag=0;_tmp3A8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)(
allow_zero?(struct _dynforward_ptr)({const char*_tmp3AA="calloc";_tag_dynforward(
_tmp3AA,sizeof(char),_get_zero_arr_size(_tmp3AA,7));}):(struct _dynforward_ptr)({
const char*_tmp3AB="malloc";_tag_dynforward(_tmp3AB,sizeof(char),
_get_zero_arr_size(_tmp3AB,7));})));{void*_tmp3A6[2]={& _tmp3A8,& _tmp3A9};Cyc_Tcutil_terr(
loc,({const char*_tmp3A7="%s cannot be used with type %s\n\t(type needs initialization)";
_tag_dynforward(_tmp3A7,sizeof(char),_get_zero_arr_size(_tmp3A7,60));}),
_tag_dynforward(_tmp3A6,sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)2;
if(ropt != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp3B2;_tmp3B2.tag=15;_tmp3B2.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp3B3;}));_tmp3B2;});_tmp3B1;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)ropt);
void*_tmp3AC=Cyc_Tcutil_compress(handle_type);void*_tmp3AD;_LL210: if(_tmp3AC <= (
void*)4)goto _LL212;if(*((int*)_tmp3AC)!= 15)goto _LL212;_tmp3AD=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp3AC)->f1;_LL211: rgn=_tmp3AD;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL20F;_LL212:;_LL213:({struct Cyc_String_pa_struct _tmp3B0;
_tmp3B0.tag=0;_tmp3B0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
handle_type));{void*_tmp3AE[1]={& _tmp3B0};Cyc_Tcutil_terr(ropt->loc,({const char*
_tmp3AF="expecting region_t type but found %s";_tag_dynforward(_tmp3AF,sizeof(
char),_get_zero_arr_size(_tmp3AF,37));}),_tag_dynforward(_tmp3AE,sizeof(void*),1));}});
goto _LL20F;_LL20F:;}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*
elt_type;struct Cyc_Absyn_Exp*num_elts;int one_elt;if(*is_calloc){if(*t == 0)({void*
_tmp3B4[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp3B5="calloc with empty type";_tag_dynforward(_tmp3B5,sizeof(char),
_get_zero_arr_size(_tmp3B5,23));}),_tag_dynforward(_tmp3B4,sizeof(void*),0));});
elt_type=*((void**)_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;one_elt=0;}else{void*_tmp3B6=(void*)(*e)->r;void*_tmp3B7;void*
_tmp3B8;struct Cyc_List_List*_tmp3B9;struct Cyc_List_List _tmp3BA;struct Cyc_Absyn_Exp*
_tmp3BB;struct Cyc_List_List*_tmp3BC;struct Cyc_List_List _tmp3BD;struct Cyc_Absyn_Exp*
_tmp3BE;struct Cyc_List_List*_tmp3BF;_LL215: if(*((int*)_tmp3B6)!= 18)goto _LL217;
_tmp3B7=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3B6)->f1;_LL216:
elt_type=_tmp3B7;*t=(void**)({void**_tmp3C0=_cycalloc(sizeof(*_tmp3C0));_tmp3C0[
0]=elt_type;_tmp3C0;});num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(te,(void**)&
Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL214;_LL217: if(*((int*)_tmp3B6)!= 3)
goto _LL219;_tmp3B8=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp3B6)->f1;if((
int)_tmp3B8 != 1)goto _LL219;_tmp3B9=((struct Cyc_Absyn_Primop_e_struct*)_tmp3B6)->f2;
if(_tmp3B9 == 0)goto _LL219;_tmp3BA=*_tmp3B9;_tmp3BB=(struct Cyc_Absyn_Exp*)_tmp3BA.hd;
_tmp3BC=_tmp3BA.tl;if(_tmp3BC == 0)goto _LL219;_tmp3BD=*_tmp3BC;_tmp3BE=(struct Cyc_Absyn_Exp*)
_tmp3BD.hd;_tmp3BF=_tmp3BD.tl;if(_tmp3BF != 0)goto _LL219;_LL218:{struct _tuple0
_tmp3C2=({struct _tuple0 _tmp3C1;_tmp3C1.f1=(void*)_tmp3BB->r;_tmp3C1.f2=(void*)
_tmp3BE->r;_tmp3C1;});void*_tmp3C3;void*_tmp3C4;void*_tmp3C5;void*_tmp3C6;_LL21C:
_tmp3C3=_tmp3C2.f1;if(*((int*)_tmp3C3)!= 18)goto _LL21E;_tmp3C4=(void*)((struct
Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3C3)->f1;_LL21D: Cyc_Tcexp_check_malloc_type(0,
loc,topt,_tmp3C4);elt_type=_tmp3C4;*t=(void**)({void**_tmp3C7=_cycalloc(sizeof(*
_tmp3C7));_tmp3C7[0]=elt_type;_tmp3C7;});num_elts=_tmp3BE;one_elt=0;goto _LL21B;
_LL21E: _tmp3C5=_tmp3C2.f2;if(*((int*)_tmp3C5)!= 18)goto _LL220;_tmp3C6=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3C5)->f1;_LL21F: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp3C6);elt_type=_tmp3C6;*t=(void**)({void**_tmp3C8=_cycalloc(
sizeof(*_tmp3C8));_tmp3C8[0]=elt_type;_tmp3C8;});num_elts=_tmp3BB;one_elt=0;goto
_LL21B;_LL220:;_LL221: goto No_sizeof;_LL21B:;}goto _LL214;_LL219:;_LL21A: No_sizeof:
elt_type=Cyc_Absyn_char_typ;*t=(void**)({void**_tmp3C9=_cycalloc(sizeof(*_tmp3C9));
_tmp3C9[0]=elt_type;_tmp3C9;});num_elts=*e;one_elt=0;goto _LL214;_LL214:;}*e=
num_elts;*is_fat=!one_elt;{void*_tmp3CA=elt_type;struct Cyc_Absyn_AggrInfo _tmp3CB;
void*_tmp3CC;struct Cyc_Absyn_Aggrdecl**_tmp3CD;struct Cyc_Absyn_Aggrdecl*_tmp3CE;
_LL223: if(_tmp3CA <= (void*)4)goto _LL225;if(*((int*)_tmp3CA)!= 10)goto _LL225;
_tmp3CB=((struct Cyc_Absyn_AggrType_struct*)_tmp3CA)->f1;_tmp3CC=(void*)_tmp3CB.aggr_info;
if(*((int*)_tmp3CC)!= 1)goto _LL225;_tmp3CD=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp3CC)->f1;_tmp3CE=*_tmp3CD;_LL224: if(_tmp3CE->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3CE->impl))->exist_vars != 0)({void*_tmp3CF[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp3D0="malloc with existential types not yet implemented";
_tag_dynforward(_tmp3D0,sizeof(char),_get_zero_arr_size(_tmp3D0,50));}),
_tag_dynforward(_tmp3CF,sizeof(void*),0));});goto _LL222;_LL225:;_LL226: goto
_LL222;_LL222:;}{void*(*_tmp3D1)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Conref*zero_term)=Cyc_Absyn_at_typ;struct Cyc_Absyn_Conref*_tmp3D2=Cyc_Absyn_false_conref;
if(topt != 0){void*_tmp3D3=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp3D4;struct Cyc_Absyn_PtrAtts _tmp3D5;struct Cyc_Absyn_Conref*_tmp3D6;struct Cyc_Absyn_Conref*
_tmp3D7;_LL228: if(_tmp3D3 <= (void*)4)goto _LL22A;if(*((int*)_tmp3D3)!= 4)goto
_LL22A;_tmp3D4=((struct Cyc_Absyn_PointerType_struct*)_tmp3D3)->f1;_tmp3D5=
_tmp3D4.ptr_atts;_tmp3D6=_tmp3D5.nullable;_tmp3D7=_tmp3D5.zero_term;_LL229:
_tmp3D2=_tmp3D7;if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3D6))_tmp3D1=Cyc_Absyn_star_typ;if(((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp3D7) && !(*is_calloc)){({void*_tmp3D8[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp3D9="converting malloc to calloc to ensure zero-termination";
_tag_dynforward(_tmp3D9,sizeof(char),_get_zero_arr_size(_tmp3D9,55));}),
_tag_dynforward(_tmp3D8,sizeof(void*),0));});*is_calloc=1;}goto _LL227;_LL22A:;
_LL22B: goto _LL227;_LL227:;}if(!one_elt)_tmp3D1=Cyc_Absyn_dynforward_typ;return
_tmp3D1(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp3D2);}}}static void*Cyc_Tcexp_tcStmtExp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Stmt*
s){{struct _RegionHandle _tmp3DA=_new_region("r");struct _RegionHandle*r=& _tmp3DA;
_push_region(r);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,te,s),s,1);;
_pop_region(r);}Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
while(1){void*_tmp3DB=(void*)s->r;struct Cyc_Absyn_Exp*_tmp3DC;struct Cyc_Absyn_Stmt*
_tmp3DD;struct Cyc_Absyn_Stmt*_tmp3DE;struct Cyc_Absyn_Decl*_tmp3DF;struct Cyc_Absyn_Stmt*
_tmp3E0;_LL22D: if(_tmp3DB <= (void*)1)goto _LL233;if(*((int*)_tmp3DB)!= 0)goto
_LL22F;_tmp3DC=((struct Cyc_Absyn_Exp_s_struct*)_tmp3DB)->f1;_LL22E: return(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3DC->topt))->v;_LL22F: if(*((int*)_tmp3DB)!= 
1)goto _LL231;_tmp3DD=((struct Cyc_Absyn_Seq_s_struct*)_tmp3DB)->f1;_tmp3DE=((
struct Cyc_Absyn_Seq_s_struct*)_tmp3DB)->f2;_LL230: s=_tmp3DE;continue;_LL231: if(*((
int*)_tmp3DB)!= 11)goto _LL233;_tmp3DF=((struct Cyc_Absyn_Decl_s_struct*)_tmp3DB)->f1;
_tmp3E0=((struct Cyc_Absyn_Decl_s_struct*)_tmp3DB)->f2;_LL232: s=_tmp3E0;continue;
_LL233:;_LL234: return({void*_tmp3E1[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp3E2="statement expression must end with expression";_tag_dynforward(
_tmp3E2,sizeof(char),_get_zero_arr_size(_tmp3E2,46));}),_tag_dynforward(_tmp3E1,
sizeof(void*),0));});_LL22C:;}}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)2;if(rgn_handle != 
0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp3E8=
_cycalloc(sizeof(*_tmp3E8));_tmp3E8[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp3E9;_tmp3E9.tag=15;_tmp3E9.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));_tmp3EA->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp3EA;}));_tmp3E9;});_tmp3E8;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
rgn_handle);void*_tmp3E3=Cyc_Tcutil_compress(handle_type);void*_tmp3E4;_LL236:
if(_tmp3E3 <= (void*)4)goto _LL238;if(*((int*)_tmp3E3)!= 15)goto _LL238;_tmp3E4=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp3E3)->f1;_LL237: rgn=_tmp3E4;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL235;_LL238:;_LL239:({struct Cyc_String_pa_struct
_tmp3E7;_tmp3E7.tag=0;_tmp3E7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(handle_type));{void*_tmp3E5[1]={& _tmp3E7};Cyc_Tcutil_terr(
rgn_handle->loc,({const char*_tmp3E6="expecting region_t type but found %s";
_tag_dynforward(_tmp3E6,sizeof(char),_get_zero_arr_size(_tmp3E6,37));}),
_tag_dynforward(_tmp3E5,sizeof(void*),1));}});goto _LL235;_LL235:;}{void*_tmp3EB=(
void*)e1->r;struct Cyc_Core_Opt*_tmp3EC;struct Cyc_List_List*_tmp3ED;struct Cyc_List_List*
_tmp3EE;union Cyc_Absyn_Cnst_union _tmp3EF;struct _dynforward_ptr _tmp3F0;_LL23B: if(*((
int*)_tmp3EB)!= 29)goto _LL23D;_LL23C: {void*_tmp3F1=Cyc_Tcexp_tcExpNoPromote(te,
topt,e1);void*_tmp3F2=Cyc_Tcutil_compress(_tmp3F1);struct Cyc_Absyn_ArrayInfo
_tmp3F3;void*_tmp3F4;struct Cyc_Absyn_Tqual _tmp3F5;struct Cyc_Absyn_Exp*_tmp3F6;
struct Cyc_Absyn_Conref*_tmp3F7;_LL246: if(_tmp3F2 <= (void*)4)goto _LL248;if(*((int*)
_tmp3F2)!= 7)goto _LL248;_tmp3F3=((struct Cyc_Absyn_ArrayType_struct*)_tmp3F2)->f1;
_tmp3F4=(void*)_tmp3F3.elt_type;_tmp3F5=_tmp3F3.tq;_tmp3F6=_tmp3F3.num_elts;
_tmp3F7=_tmp3F3.zero_term;_LL247: {struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)
_check_null(_tmp3F6);void*b;{void*_tmp3F8=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bnd->topt))->v);void*_tmp3F9;_LL24B: if(_tmp3F8 <= (void*)4)goto _LL24D;
if(*((int*)_tmp3F8)!= 17)goto _LL24D;_tmp3F9=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp3F8)->f1;_LL24C: b=(void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp3FA=
_cycalloc(sizeof(*_tmp3FA));_tmp3FA[0]=({struct Cyc_Absyn_AbsUpper_b_struct
_tmp3FB;_tmp3FB.tag=1;_tmp3FB.f1=(void*)_tmp3F9;_tmp3FB;});_tmp3FA;});goto _LL24A;
_LL24D:;_LL24E: if(Cyc_Tcutil_is_const_exp(te,bnd))b=(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp3FC=_cycalloc(sizeof(*_tmp3FC));_tmp3FC[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp3FD;_tmp3FD.tag=0;_tmp3FD.f1=bnd;_tmp3FD;});_tmp3FC;});else{b=(void*)0;}
_LL24A:;}{void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp3FF=
_cycalloc(sizeof(*_tmp3FF));_tmp3FF[0]=({struct Cyc_Absyn_PointerType_struct
_tmp400;_tmp400.tag=4;_tmp400.f1=({struct Cyc_Absyn_PtrInfo _tmp401;_tmp401.elt_typ=(
void*)_tmp3F4;_tmp401.elt_tq=_tmp3F5;_tmp401.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp402;_tmp402.rgn=(void*)rgn;_tmp402.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp402.bounds=Cyc_Absyn_new_conref(b);_tmp402.zero_term=
_tmp3F7;_tmp402.ptrloc=0;_tmp402;});_tmp401;});_tmp400;});_tmp3FF;});if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*
topt)){e->topt=({struct Cyc_Core_Opt*_tmp3FE=_cycalloc(sizeof(*_tmp3FE));_tmp3FE->v=(
void*)res_typ;_tmp3FE;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);res_typ=*
topt;}}return res_typ;}}_LL248:;_LL249:({void*_tmp403[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp404="tcNew: comprehension returned non-array type";_tag_dynforward(_tmp404,
sizeof(char),_get_zero_arr_size(_tmp404,45));}),_tag_dynforward(_tmp403,sizeof(
void*),0));});_LL245:;}_LL23D: if(*((int*)_tmp3EB)!= 36)goto _LL23F;_tmp3EC=((
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp3EB)->f1;_tmp3ED=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp3EB)->f2;_LL23E:(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp405=_cycalloc(sizeof(*_tmp405));_tmp405[0]=({struct Cyc_Absyn_Array_e_struct
_tmp406;_tmp406.tag=28;_tmp406.f1=_tmp3ED;_tmp406;});_tmp405;})));_tmp3EE=
_tmp3ED;goto _LL240;_LL23F: if(*((int*)_tmp3EB)!= 28)goto _LL241;_tmp3EE=((struct
Cyc_Absyn_Array_e_struct*)_tmp3EB)->f1;_LL240: {void**elt_typ_opt=0;int zero_term=
0;if(topt != 0){void*_tmp407=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp408;void*_tmp409;void**_tmp40A;struct Cyc_Absyn_Tqual _tmp40B;struct Cyc_Absyn_PtrAtts
_tmp40C;struct Cyc_Absyn_Conref*_tmp40D;_LL250: if(_tmp407 <= (void*)4)goto _LL252;
if(*((int*)_tmp407)!= 4)goto _LL252;_tmp408=((struct Cyc_Absyn_PointerType_struct*)
_tmp407)->f1;_tmp409=(void*)_tmp408.elt_typ;_tmp40A=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp407)->f1).elt_typ;_tmp40B=_tmp408.elt_tq;_tmp40C=_tmp408.ptr_atts;_tmp40D=
_tmp40C.zero_term;_LL251: elt_typ_opt=(void**)_tmp40A;zero_term=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp40D);goto _LL24F;_LL252:;
_LL253: goto _LL24F;_LL24F:;}{void*_tmp40E=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp3EE);e1->topt=({struct Cyc_Core_Opt*_tmp40F=_cycalloc(
sizeof(*_tmp40F));_tmp40F->v=(void*)_tmp40E;_tmp40F;});{void*res_typ;{void*
_tmp410=Cyc_Tcutil_compress(_tmp40E);struct Cyc_Absyn_ArrayInfo _tmp411;void*
_tmp412;struct Cyc_Absyn_Tqual _tmp413;struct Cyc_Absyn_Exp*_tmp414;struct Cyc_Absyn_Conref*
_tmp415;_LL255: if(_tmp410 <= (void*)4)goto _LL257;if(*((int*)_tmp410)!= 7)goto
_LL257;_tmp411=((struct Cyc_Absyn_ArrayType_struct*)_tmp410)->f1;_tmp412=(void*)
_tmp411.elt_type;_tmp413=_tmp411.tq;_tmp414=_tmp411.num_elts;_tmp415=_tmp411.zero_term;
_LL256: res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp416=_cycalloc(
sizeof(*_tmp416));_tmp416[0]=({struct Cyc_Absyn_PointerType_struct _tmp417;_tmp417.tag=
4;_tmp417.f1=({struct Cyc_Absyn_PtrInfo _tmp418;_tmp418.elt_typ=(void*)_tmp412;
_tmp418.elt_tq=_tmp413;_tmp418.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp419;_tmp419.rgn=(
void*)rgn;_tmp419.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp419.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp41A=_cycalloc(sizeof(*_tmp41A));_tmp41A[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp41B;_tmp41B.tag=0;_tmp41B.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp414);
_tmp41B;});_tmp41A;}));_tmp419.zero_term=_tmp415;_tmp419.ptrloc=0;_tmp419;});
_tmp418;});_tmp417;});_tmp416;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ)
 && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){e->topt=({struct Cyc_Core_Opt*
_tmp41C=_cycalloc(sizeof(*_tmp41C));_tmp41C->v=(void*)res_typ;_tmp41C;});Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}goto _LL254;_LL257:;_LL258:({void*_tmp41D[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp41E="tcExpNoPromote on Array_e returned non-array type";
_tag_dynforward(_tmp41E,sizeof(char),_get_zero_arr_size(_tmp41E,50));}),
_tag_dynforward(_tmp41D,sizeof(void*),0));});_LL254:;}return res_typ;}}}_LL241:
if(*((int*)_tmp3EB)!= 0)goto _LL243;_tmp3EF=((struct Cyc_Absyn_Const_e_struct*)
_tmp3EB)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp3EB)->f1).String_c).tag
!= 5)goto _LL243;_tmp3F0=(_tmp3EF.String_c).f1;_LL242: {void*_tmp41F=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);
void*_tmp420=Cyc_Tcexp_tcExp(te,(void**)& _tmp41F,e1);return Cyc_Absyn_atb_typ(
_tmp420,rgn,Cyc_Absyn_empty_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp422;_tmp422.tag=0;_tmp422.f1=Cyc_Absyn_uint_exp(1,0);_tmp422;});_tmp421;}),
Cyc_Absyn_false_conref);}_LL243:;_LL244: {void**topt2=0;if(topt != 0){void*
_tmp423=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp424;void*_tmp425;
void**_tmp426;struct Cyc_Absyn_Tqual _tmp427;_LL25A: if(_tmp423 <= (void*)4)goto
_LL25E;if(*((int*)_tmp423)!= 4)goto _LL25C;_tmp424=((struct Cyc_Absyn_PointerType_struct*)
_tmp423)->f1;_tmp425=(void*)_tmp424.elt_typ;_tmp426=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp423)->f1).elt_typ;_tmp427=_tmp424.elt_tq;_LL25B: topt2=(void**)_tmp426;goto
_LL259;_LL25C: if(*((int*)_tmp423)!= 2)goto _LL25E;_LL25D: topt2=({void**_tmp428=
_cycalloc(sizeof(*_tmp428));_tmp428[0]=*topt;_tmp428;});goto _LL259;_LL25E:;
_LL25F: goto _LL259;_LL259:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);void*res_typ=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp42A=_cycalloc(sizeof(*_tmp42A));
_tmp42A[0]=({struct Cyc_Absyn_PointerType_struct _tmp42B;_tmp42B.tag=4;_tmp42B.f1=({
struct Cyc_Absyn_PtrInfo _tmp42C;_tmp42C.elt_typ=(void*)telt;_tmp42C.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmp42C.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp42D;_tmp42D.rgn=(void*)rgn;
_tmp42D.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp42D.bounds=Cyc_Absyn_bounds_one_conref;_tmp42D.zero_term=Cyc_Absyn_false_conref;
_tmp42D.ptrloc=0;_tmp42D;});_tmp42C;});_tmp42B;});_tmp42A;});if(topt != 0){if(!
Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*
topt)){e->topt=({struct Cyc_Core_Opt*_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429->v=(
void*)res_typ;_tmp429;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);res_typ=*
topt;}}return res_typ;}}_LL23A:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,
void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp42E=t;struct Cyc_Absyn_ArrayInfo _tmp42F;void*_tmp430;struct
Cyc_Absyn_Tqual _tmp431;struct Cyc_Absyn_Exp*_tmp432;struct Cyc_Absyn_Conref*
_tmp433;_LL261: if(_tmp42E <= (void*)4)goto _LL263;if(*((int*)_tmp42E)!= 7)goto
_LL263;_tmp42F=((struct Cyc_Absyn_ArrayType_struct*)_tmp42E)->f1;_tmp430=(void*)
_tmp42F.elt_type;_tmp431=_tmp42F.tq;_tmp432=_tmp42F.num_elts;_tmp433=_tmp42F.zero_term;
_LL262: {void*_tmp435;struct _tuple6 _tmp434=Cyc_Tcutil_addressof_props(te,e);
_tmp435=_tmp434.f2;{void*_tmp436=_tmp432 == 0?(void*)((void*)0):(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp438;_tmp438.tag=0;_tmp438.f1=(struct Cyc_Absyn_Exp*)
_tmp432;_tmp438;});_tmp437;});t=Cyc_Absyn_atb_typ(_tmp430,_tmp435,_tmp431,
_tmp436,_tmp433);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);
return t;}}_LL263:;_LL264: return t;_LL260:;}void*Cyc_Tcexp_tcExpInitializer(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(
te,topt,e);void*_tmp439=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp43A;_LL266: if(*((
int*)_tmp439)!= 28)goto _LL268;_LL267: goto _LL269;_LL268: if(*((int*)_tmp439)!= 29)
goto _LL26A;_LL269: goto _LL26B;_LL26A: if(*((int*)_tmp439)!= 0)goto _LL26C;_tmp43A=((
struct Cyc_Absyn_Const_e_struct*)_tmp439)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp439)->f1).String_c).tag != 5)goto _LL26C;_LL26B: return t;_LL26C:;_LL26D: t=Cyc_Tcutil_compress(
t);{void*_tmp43B=t;struct Cyc_Absyn_ArrayInfo _tmp43C;void*_tmp43D;struct Cyc_Absyn_Tqual
_tmp43E;struct Cyc_Absyn_Exp*_tmp43F;struct Cyc_Absyn_Conref*_tmp440;_LL26F: if(
_tmp43B <= (void*)4)goto _LL271;if(*((int*)_tmp43B)!= 7)goto _LL271;_tmp43C=((
struct Cyc_Absyn_ArrayType_struct*)_tmp43B)->f1;_tmp43D=(void*)_tmp43C.elt_type;
_tmp43E=_tmp43C.tq;_tmp43F=_tmp43C.num_elts;_tmp440=_tmp43C.zero_term;_LL270: {
void*_tmp442;struct _tuple6 _tmp441=Cyc_Tcutil_addressof_props(te,e);_tmp442=
_tmp441.f2;{void*b=_tmp43F == 0?(void*)((void*)0):(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp444;_tmp444.tag=0;_tmp444.f1=(struct Cyc_Absyn_Exp*)_tmp43F;_tmp444;});
_tmp443;});t=Cyc_Absyn_atb_typ(_tmp43D,_tmp442,_tmp43E,b,_tmp440);Cyc_Tcutil_unchecked_cast(
te,e,t,(void*)3);return t;}}_LL271:;_LL272: return t;_LL26E:;}_LL265:;}static void*
Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e){{void*_tmp445=(void*)e->r;struct Cyc_Absyn_Exp*_tmp446;_LL274: if(*((int*)
_tmp445)!= 13)goto _LL276;_tmp446=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp445)->f1;_LL275: Cyc_Tcexp_tcExpNoInst(te,topt,_tmp446);(void*)(((struct Cyc_Core_Opt*)
_check_null(_tmp446->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp446->topt))->v,0));e->topt=_tmp446->topt;goto
_LL273;_LL276:;_LL277: Cyc_Tcexp_tcExpNoInst(te,topt,e);(void*)(((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0));{void*_tmp447=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp448;void*
_tmp449;struct Cyc_Absyn_Tqual _tmp44A;struct Cyc_Absyn_PtrAtts _tmp44B;void*_tmp44C;
struct Cyc_Absyn_Conref*_tmp44D;struct Cyc_Absyn_Conref*_tmp44E;struct Cyc_Absyn_Conref*
_tmp44F;_LL279: if(_tmp447 <= (void*)4)goto _LL27B;if(*((int*)_tmp447)!= 4)goto
_LL27B;_tmp448=((struct Cyc_Absyn_PointerType_struct*)_tmp447)->f1;_tmp449=(void*)
_tmp448.elt_typ;_tmp44A=_tmp448.elt_tq;_tmp44B=_tmp448.ptr_atts;_tmp44C=(void*)
_tmp44B.rgn;_tmp44D=_tmp44B.nullable;_tmp44E=_tmp44B.bounds;_tmp44F=_tmp44B.zero_term;
_LL27A:{void*_tmp450=Cyc_Tcutil_compress(_tmp449);struct Cyc_Absyn_FnInfo _tmp451;
struct Cyc_List_List*_tmp452;struct Cyc_Core_Opt*_tmp453;void*_tmp454;struct Cyc_List_List*
_tmp455;int _tmp456;struct Cyc_Absyn_VarargInfo*_tmp457;struct Cyc_List_List*
_tmp458;struct Cyc_List_List*_tmp459;_LL27E: if(_tmp450 <= (void*)4)goto _LL280;if(*((
int*)_tmp450)!= 8)goto _LL280;_tmp451=((struct Cyc_Absyn_FnType_struct*)_tmp450)->f1;
_tmp452=_tmp451.tvars;_tmp453=_tmp451.effect;_tmp454=(void*)_tmp451.ret_typ;
_tmp455=_tmp451.args;_tmp456=_tmp451.c_varargs;_tmp457=_tmp451.cyc_varargs;
_tmp458=_tmp451.rgn_po;_tmp459=_tmp451.attributes;_LL27F: if(_tmp452 != 0){struct
_RegionHandle _tmp45A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp45A;
_push_region(rgn);{struct _tuple4 _tmp45B=({struct _tuple4 _tmp468;_tmp468.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp468.f2=rgn;_tmp468;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp45B,_tmp452);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465[0]=({struct Cyc_Absyn_FnType_struct
_tmp466;_tmp466.tag=8;_tmp466.f1=({struct Cyc_Absyn_FnInfo _tmp467;_tmp467.tvars=0;
_tmp467.effect=_tmp453;_tmp467.ret_typ=(void*)_tmp454;_tmp467.args=_tmp455;
_tmp467.c_varargs=_tmp456;_tmp467.cyc_varargs=_tmp457;_tmp467.rgn_po=_tmp458;
_tmp467.attributes=_tmp459;_tmp467;});_tmp466;});_tmp465;}));struct Cyc_Absyn_PointerType_struct*
_tmp45C=({struct Cyc_Absyn_PointerType_struct*_tmp461=_cycalloc(sizeof(*_tmp461));
_tmp461[0]=({struct Cyc_Absyn_PointerType_struct _tmp462;_tmp462.tag=4;_tmp462.f1=({
struct Cyc_Absyn_PtrInfo _tmp463;_tmp463.elt_typ=(void*)ftyp;_tmp463.elt_tq=
_tmp44A;_tmp463.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp464;_tmp464.rgn=(void*)
_tmp44C;_tmp464.nullable=_tmp44D;_tmp464.bounds=_tmp44E;_tmp464.zero_term=
_tmp44F;_tmp464.ptrloc=0;_tmp464;});_tmp463;});_tmp462;});_tmp461;});struct Cyc_Absyn_Exp*
_tmp45D=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmp45E=_cycalloc(sizeof(*_tmp45E));_tmp45E[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmp45F;_tmp45F.tag=14;_tmp45F.f1=_tmp45D;_tmp45F.f2=ts;_tmp45F;});_tmp45E;})));
e->topt=({struct Cyc_Core_Opt*_tmp460=_cycalloc(sizeof(*_tmp460));_tmp460->v=(
void*)((void*)_tmp45C);_tmp460;});};_pop_region(rgn);}goto _LL27D;_LL280:;_LL281:
goto _LL27D;_LL27D:;}goto _LL278;_LL27B:;_LL27C: goto _LL278;_LL278:;}goto _LL273;
_LL273:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static void
Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Position_Segment*loc=e->loc;void*t;{void*_tmp469=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp46A;struct _tuple1*_tmp46B;struct Cyc_Absyn_Exp*_tmp46C;struct Cyc_List_List*
_tmp46D;struct Cyc_Core_Opt*_tmp46E;struct Cyc_List_List*_tmp46F;union Cyc_Absyn_Cnst_union
_tmp470;struct _tuple1*_tmp471;void*_tmp472;void*_tmp473;struct Cyc_List_List*
_tmp474;struct Cyc_Absyn_Exp*_tmp475;void*_tmp476;struct Cyc_Absyn_Exp*_tmp477;
struct Cyc_Core_Opt*_tmp478;struct Cyc_Absyn_Exp*_tmp479;struct Cyc_Absyn_Exp*
_tmp47A;struct Cyc_Absyn_Exp*_tmp47B;struct Cyc_Absyn_Exp*_tmp47C;struct Cyc_Absyn_Exp*
_tmp47D;struct Cyc_Absyn_Exp*_tmp47E;struct Cyc_Absyn_Exp*_tmp47F;struct Cyc_Absyn_Exp*
_tmp480;struct Cyc_Absyn_Exp*_tmp481;struct Cyc_Absyn_Exp*_tmp482;struct Cyc_Absyn_Exp*
_tmp483;struct Cyc_List_List*_tmp484;struct Cyc_Absyn_VarargCallInfo*_tmp485;
struct Cyc_Absyn_VarargCallInfo**_tmp486;struct Cyc_Absyn_Exp*_tmp487;struct Cyc_Absyn_Exp*
_tmp488;struct Cyc_List_List*_tmp489;void*_tmp48A;struct Cyc_Absyn_Exp*_tmp48B;
void*_tmp48C;void**_tmp48D;struct Cyc_Absyn_Exp*_tmp48E;struct Cyc_Absyn_Exp*
_tmp48F;struct Cyc_Absyn_Exp*_tmp490;struct Cyc_Absyn_Exp*_tmp491;void*_tmp492;
void*_tmp493;void*_tmp494;struct Cyc_Absyn_Exp*_tmp495;struct Cyc_Absyn_Exp*
_tmp496;struct _dynforward_ptr*_tmp497;struct Cyc_Absyn_Exp*_tmp498;struct
_dynforward_ptr*_tmp499;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_Absyn_Exp*_tmp49B;
struct Cyc_List_List*_tmp49C;struct _tuple2*_tmp49D;struct Cyc_List_List*_tmp49E;
struct Cyc_List_List*_tmp49F;struct Cyc_Absyn_Stmt*_tmp4A0;struct Cyc_Absyn_Vardecl*
_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_Absyn_Exp*_tmp4A3;int _tmp4A4;int*
_tmp4A5;struct _tuple1*_tmp4A6;struct _tuple1**_tmp4A7;struct Cyc_List_List*_tmp4A8;
struct Cyc_List_List**_tmp4A9;struct Cyc_List_List*_tmp4AA;struct Cyc_Absyn_Aggrdecl*
_tmp4AB;struct Cyc_Absyn_Aggrdecl**_tmp4AC;void*_tmp4AD;struct Cyc_List_List*
_tmp4AE;struct Cyc_List_List*_tmp4AF;struct Cyc_Absyn_Tuniondecl*_tmp4B0;struct Cyc_Absyn_Tunionfield*
_tmp4B1;struct _tuple1*_tmp4B2;struct _tuple1**_tmp4B3;struct Cyc_Absyn_Enumdecl*
_tmp4B4;struct Cyc_Absyn_Enumfield*_tmp4B5;struct _tuple1*_tmp4B6;struct _tuple1**
_tmp4B7;void*_tmp4B8;struct Cyc_Absyn_Enumfield*_tmp4B9;struct Cyc_Absyn_MallocInfo
_tmp4BA;int _tmp4BB;int*_tmp4BC;struct Cyc_Absyn_Exp*_tmp4BD;void**_tmp4BE;void***
_tmp4BF;struct Cyc_Absyn_Exp*_tmp4C0;struct Cyc_Absyn_Exp**_tmp4C1;int _tmp4C2;int*
_tmp4C3;_LL283: if(*((int*)_tmp469)!= 13)goto _LL285;_tmp46A=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp469)->f1;_LL284: Cyc_Tcexp_tcExpNoInst(te,0,_tmp46A);return;_LL285: if(*((int*)
_tmp469)!= 2)goto _LL287;_tmp46B=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp469)->f1;
_LL286: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp46B);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL287: if(*((int*)_tmp469)!= 10)goto _LL289;_tmp46C=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp469)->f1;_tmp46D=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp469)->f2;_LL288:
Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp46C,_tmp46D);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL289: if(*((int*)_tmp469)!= 36)goto _LL28B;_tmp46E=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp469)->f1;_tmp46F=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp469)->f2;
_LL28A: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp46F);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL28B: if(*((int*)_tmp469)!= 0)goto _LL28D;_tmp470=((struct Cyc_Absyn_Const_e_struct*)
_tmp469)->f1;_LL28C: t=Cyc_Tcexp_tcConst(te,loc,topt,_tmp470,e);goto _LL282;_LL28D:
if(*((int*)_tmp469)!= 1)goto _LL28F;_tmp471=((struct Cyc_Absyn_Var_e_struct*)
_tmp469)->f1;_tmp472=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp469)->f2;_LL28E:
t=Cyc_Tcexp_tcVar(te,loc,_tmp471,_tmp472);goto _LL282;_LL28F: if(*((int*)_tmp469)
!= 3)goto _LL291;_tmp473=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp469)->f1;
_tmp474=((struct Cyc_Absyn_Primop_e_struct*)_tmp469)->f2;_LL290: t=Cyc_Tcexp_tcPrimop(
te,loc,topt,_tmp473,_tmp474);goto _LL282;_LL291: if(*((int*)_tmp469)!= 5)goto
_LL293;_tmp475=((struct Cyc_Absyn_Increment_e_struct*)_tmp469)->f1;_tmp476=(void*)((
struct Cyc_Absyn_Increment_e_struct*)_tmp469)->f2;_LL292: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp475,_tmp476);goto _LL282;_LL293: if(*((int*)_tmp469)!= 4)goto
_LL295;_tmp477=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp469)->f1;_tmp478=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp469)->f2;_tmp479=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp469)->f3;_LL294: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp477,_tmp478,_tmp479);
goto _LL282;_LL295: if(*((int*)_tmp469)!= 6)goto _LL297;_tmp47A=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp469)->f1;_tmp47B=((struct Cyc_Absyn_Conditional_e_struct*)_tmp469)->f2;
_tmp47C=((struct Cyc_Absyn_Conditional_e_struct*)_tmp469)->f3;_LL296: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp47A,_tmp47B,_tmp47C);goto _LL282;_LL297: if(*((int*)_tmp469)!= 7)
goto _LL299;_tmp47D=((struct Cyc_Absyn_And_e_struct*)_tmp469)->f1;_tmp47E=((struct
Cyc_Absyn_And_e_struct*)_tmp469)->f2;_LL298: t=Cyc_Tcexp_tcAnd(te,loc,_tmp47D,
_tmp47E);goto _LL282;_LL299: if(*((int*)_tmp469)!= 8)goto _LL29B;_tmp47F=((struct
Cyc_Absyn_Or_e_struct*)_tmp469)->f1;_tmp480=((struct Cyc_Absyn_Or_e_struct*)
_tmp469)->f2;_LL29A: t=Cyc_Tcexp_tcOr(te,loc,_tmp47F,_tmp480);goto _LL282;_LL29B:
if(*((int*)_tmp469)!= 9)goto _LL29D;_tmp481=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp469)->f1;_tmp482=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp469)->f2;_LL29C: t=
Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp481,_tmp482);goto _LL282;_LL29D: if(*((int*)
_tmp469)!= 11)goto _LL29F;_tmp483=((struct Cyc_Absyn_FnCall_e_struct*)_tmp469)->f1;
_tmp484=((struct Cyc_Absyn_FnCall_e_struct*)_tmp469)->f2;_tmp485=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp469)->f3;_tmp486=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)
_tmp469)->f3;_LL29E: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp483,_tmp484,_tmp486);
goto _LL282;_LL29F: if(*((int*)_tmp469)!= 12)goto _LL2A1;_tmp487=((struct Cyc_Absyn_Throw_e_struct*)
_tmp469)->f1;_LL2A0: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp487);goto _LL282;_LL2A1:
if(*((int*)_tmp469)!= 14)goto _LL2A3;_tmp488=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp469)->f1;_tmp489=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp469)->f2;_LL2A2:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp488,_tmp489);goto _LL282;_LL2A3: if(*((
int*)_tmp469)!= 15)goto _LL2A5;_tmp48A=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp469)->f1;_tmp48B=((struct Cyc_Absyn_Cast_e_struct*)_tmp469)->f2;_tmp48C=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp469)->f4;_tmp48D=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp469)->f4);_LL2A4: t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp48A,_tmp48B,(void**)
_tmp48D);goto _LL282;_LL2A5: if(*((int*)_tmp469)!= 16)goto _LL2A7;_tmp48E=((struct
Cyc_Absyn_Address_e_struct*)_tmp469)->f1;_LL2A6: t=Cyc_Tcexp_tcAddress(te,loc,e,
topt,_tmp48E);goto _LL282;_LL2A7: if(*((int*)_tmp469)!= 17)goto _LL2A9;_tmp48F=((
struct Cyc_Absyn_New_e_struct*)_tmp469)->f1;_tmp490=((struct Cyc_Absyn_New_e_struct*)
_tmp469)->f2;_LL2A8: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp48F,e,_tmp490);goto _LL282;
_LL2A9: if(*((int*)_tmp469)!= 19)goto _LL2AB;_tmp491=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp469)->f1;_LL2AA: {void*_tmp4C4=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp491);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4C4);goto _LL282;}_LL2AB: if(*((int*)_tmp469)!= 18)goto _LL2AD;
_tmp492=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp469)->f1;_LL2AC: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp492);goto _LL282;_LL2AD: if(*((int*)_tmp469)!= 20)goto _LL2AF;
_tmp493=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp469)->f1;_tmp494=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp469)->f2;_LL2AE: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp493,_tmp494);goto _LL282;_LL2AF: if(*((int*)_tmp469)!= 21)goto _LL2B1;
_LL2B0:({void*_tmp4C5[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp4C6="gen() not in top-level initializer";
_tag_dynforward(_tmp4C6,sizeof(char),_get_zero_arr_size(_tmp4C6,35));}),
_tag_dynforward(_tmp4C5,sizeof(void*),0));});return;_LL2B1: if(*((int*)_tmp469)!= 
22)goto _LL2B3;_tmp495=((struct Cyc_Absyn_Deref_e_struct*)_tmp469)->f1;_LL2B2: t=
Cyc_Tcexp_tcDeref(te,loc,topt,_tmp495);goto _LL282;_LL2B3: if(*((int*)_tmp469)!= 
23)goto _LL2B5;_tmp496=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp469)->f1;
_tmp497=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp469)->f2;_LL2B4: t=Cyc_Tcexp_tcAggrMember(
te,loc,topt,e,_tmp496,_tmp497);goto _LL282;_LL2B5: if(*((int*)_tmp469)!= 24)goto
_LL2B7;_tmp498=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp469)->f1;_tmp499=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp469)->f2;_LL2B6: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp498,_tmp499);goto _LL282;_LL2B7: if(*((int*)_tmp469)!= 25)goto
_LL2B9;_tmp49A=((struct Cyc_Absyn_Subscript_e_struct*)_tmp469)->f1;_tmp49B=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp469)->f2;_LL2B8: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp49A,_tmp49B);goto _LL282;_LL2B9: if(*((int*)_tmp469)!= 26)goto
_LL2BB;_tmp49C=((struct Cyc_Absyn_Tuple_e_struct*)_tmp469)->f1;_LL2BA: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp49C);goto _LL282;_LL2BB: if(*((int*)_tmp469)!= 27)goto _LL2BD;
_tmp49D=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp469)->f1;_tmp49E=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp469)->f2;_LL2BC: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp49D,_tmp49E);goto
_LL282;_LL2BD: if(*((int*)_tmp469)!= 28)goto _LL2BF;_tmp49F=((struct Cyc_Absyn_Array_e_struct*)
_tmp469)->f1;_LL2BE: {void**elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp4C7=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp4C8;void*_tmp4C9;void**
_tmp4CA;struct Cyc_Absyn_Conref*_tmp4CB;_LL2D0: if(_tmp4C7 <= (void*)4)goto _LL2D2;
if(*((int*)_tmp4C7)!= 7)goto _LL2D2;_tmp4C8=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4C7)->f1;_tmp4C9=(void*)_tmp4C8.elt_type;_tmp4CA=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp4C7)->f1).elt_type;_tmp4CB=_tmp4C8.zero_term;_LL2D1: elt_topt=(void**)_tmp4CA;
zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp4CB);
goto _LL2CF;_LL2D2:;_LL2D3: goto _LL2CF;_LL2CF:;}t=Cyc_Tcexp_tcArray(te,loc,
elt_topt,zero_term,_tmp49F);goto _LL282;}_LL2BF: if(*((int*)_tmp469)!= 37)goto
_LL2C1;_tmp4A0=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp469)->f1;_LL2C0: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp4A0);goto _LL282;_LL2C1: if(*((int*)_tmp469)!= 29)goto _LL2C3;
_tmp4A1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp469)->f1;_tmp4A2=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp469)->f2;_tmp4A3=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp469)->f3;_tmp4A4=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp469)->f4;
_tmp4A5=(int*)&((struct Cyc_Absyn_Comprehension_e_struct*)_tmp469)->f4;_LL2C2: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4A1,_tmp4A2,_tmp4A3,_tmp4A5);goto
_LL282;_LL2C3: if(*((int*)_tmp469)!= 30)goto _LL2C5;_tmp4A6=((struct Cyc_Absyn_Struct_e_struct*)
_tmp469)->f1;_tmp4A7=(struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp469)->f1;_tmp4A8=((struct Cyc_Absyn_Struct_e_struct*)_tmp469)->f2;_tmp4A9=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp469)->f2;_tmp4AA=((
struct Cyc_Absyn_Struct_e_struct*)_tmp469)->f3;_tmp4AB=((struct Cyc_Absyn_Struct_e_struct*)
_tmp469)->f4;_tmp4AC=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp469)->f4;_LL2C4: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp4A7,_tmp4A9,_tmp4AA,
_tmp4AC);goto _LL282;_LL2C5: if(*((int*)_tmp469)!= 31)goto _LL2C7;_tmp4AD=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp469)->f1;_tmp4AE=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp469)->f2;_LL2C6: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp4AD,_tmp4AE);goto _LL282;
_LL2C7: if(*((int*)_tmp469)!= 32)goto _LL2C9;_tmp4AF=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp469)->f1;_tmp4B0=((struct Cyc_Absyn_Tunion_e_struct*)_tmp469)->f2;_tmp4B1=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp469)->f3;_LL2C8: t=Cyc_Tcexp_tcTunion(te,loc,
topt,e,_tmp4AF,_tmp4B0,_tmp4B1);goto _LL282;_LL2C9: if(*((int*)_tmp469)!= 33)goto
_LL2CB;_tmp4B2=((struct Cyc_Absyn_Enum_e_struct*)_tmp469)->f1;_tmp4B3=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp469)->f1;_tmp4B4=((struct Cyc_Absyn_Enum_e_struct*)
_tmp469)->f2;_tmp4B5=((struct Cyc_Absyn_Enum_e_struct*)_tmp469)->f3;_LL2CA:*
_tmp4B3=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp4B5))->name;t=(void*)({
struct Cyc_Absyn_EnumType_struct*_tmp4CC=_cycalloc(sizeof(*_tmp4CC));_tmp4CC[0]=({
struct Cyc_Absyn_EnumType_struct _tmp4CD;_tmp4CD.tag=12;_tmp4CD.f1=((struct Cyc_Absyn_Enumdecl*)
_check_null(_tmp4B4))->name;_tmp4CD.f2=_tmp4B4;_tmp4CD;});_tmp4CC;});goto _LL282;
_LL2CB: if(*((int*)_tmp469)!= 34)goto _LL2CD;_tmp4B6=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp469)->f1;_tmp4B7=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp469)->f1;_tmp4B8=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp469)->f2;
_tmp4B9=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp469)->f3;_LL2CC:*_tmp4B7=((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp4B9))->name;t=_tmp4B8;goto _LL282;
_LL2CD: if(*((int*)_tmp469)!= 35)goto _LL282;_tmp4BA=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp469)->f1;_tmp4BB=_tmp4BA.is_calloc;_tmp4BC=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp469)->f1).is_calloc;_tmp4BD=_tmp4BA.rgn;_tmp4BE=_tmp4BA.elt_type;_tmp4BF=(
void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp469)->f1).elt_type;_tmp4C0=
_tmp4BA.num_elts;_tmp4C1=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp469)->f1).num_elts;_tmp4C2=_tmp4BA.fat_result;_tmp4C3=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp469)->f1).fat_result;_LL2CE: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp4BD,_tmp4BF,
_tmp4C1,_tmp4BC,_tmp4C3);goto _LL282;_LL282:;}e->topt=({struct Cyc_Core_Opt*
_tmp4CE=_cycalloc(sizeof(*_tmp4CE));_tmp4CE->v=(void*)t;_tmp4CE;});}
