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
_dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple1{void*f1;struct _dynforward_ptr*f2;};struct Cyc_Absyn_Conref;
struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{
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
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
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
};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);static void*Cyc_Tcexp_expr_err(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct
_dynforward_ptr msg,struct _dynforward_ptr ap){({void*_tmp0[0]={};Cyc_Tcutil_terr(
loc,(struct _dynforward_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),
_tag_dynforward(_tmp0,sizeof(void*),0));});if(topt == 0)return Cyc_Absyn_wildtyp(({
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
struct Cyc_Position_Segment*loc,void**topt,void*c,struct Cyc_Absyn_Exp*e){void*t;{
void*_tmp57=c;void*_tmp58;void*_tmp59;void*_tmp5A;void*_tmp5B;void*_tmp5C;void*
_tmp5D;int _tmp5E;struct _dynforward_ptr _tmp5F;_LL34: if(_tmp57 <= (void*)1)goto
_LL44;if(*((int*)_tmp57)!= 0)goto _LL36;_tmp58=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp57)->f1;if((int)_tmp58 != 0)goto _LL36;_LL35: t=Cyc_Absyn_schar_typ;goto _LL33;
_LL36: if(*((int*)_tmp57)!= 0)goto _LL38;_tmp59=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp57)->f1;if((int)_tmp59 != 1)goto _LL38;_LL37: t=Cyc_Absyn_uchar_typ;goto _LL33;
_LL38: if(*((int*)_tmp57)!= 0)goto _LL3A;_tmp5A=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp57)->f1;if((int)_tmp5A != 2)goto _LL3A;_LL39: t=Cyc_Absyn_char_typ;goto _LL33;
_LL3A: if(*((int*)_tmp57)!= 1)goto _LL3C;_tmp5B=(void*)((struct Cyc_Absyn_Short_c_struct*)
_tmp57)->f1;_LL3B: t=_tmp5B == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
goto _LL33;_LL3C: if(*((int*)_tmp57)!= 3)goto _LL3E;_tmp5C=(void*)((struct Cyc_Absyn_LongLong_c_struct*)
_tmp57)->f1;_LL3D: t=_tmp5C == (void*)1?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;
goto _LL33;_LL3E: if(*((int*)_tmp57)!= 4)goto _LL40;_LL3F: t=Cyc_Absyn_float_typ;
goto _LL33;_LL40: if(*((int*)_tmp57)!= 2)goto _LL42;_tmp5D=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp57)->f1;_tmp5E=((struct Cyc_Absyn_Int_c_struct*)_tmp57)->f2;_LL41: if(topt == 0)
t=_tmp5D == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{void*_tmp60=Cyc_Tcutil_compress(*
topt);void*_tmp61;void*_tmp62;void*_tmp63;void*_tmp64;void*_tmp65;void*_tmp66;
void*_tmp67;void*_tmp68;void*_tmp69;_LL47: if(_tmp60 <= (void*)4)goto _LL53;if(*((
int*)_tmp60)!= 5)goto _LL49;_tmp61=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp60)->f1;_tmp62=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp60)->f2;if((int)
_tmp62 != 0)goto _LL49;_LL48:{void*_tmp6A=_tmp61;_LL56: if((int)_tmp6A != 1)goto
_LL58;_LL57: t=Cyc_Absyn_uchar_typ;goto _LL55;_LL58: if((int)_tmp6A != 0)goto _LL5A;
_LL59: t=Cyc_Absyn_schar_typ;goto _LL55;_LL5A: if((int)_tmp6A != 2)goto _LL55;_LL5B: t=
Cyc_Absyn_char_typ;goto _LL55;_LL55:;}(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B[0]=({struct Cyc_Absyn_Const_e_struct
_tmp6C;_tmp6C.tag=0;_tmp6C.f1=(void*)((void*)({struct Cyc_Absyn_Char_c_struct*
_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D[0]=({struct Cyc_Absyn_Char_c_struct
_tmp6E;_tmp6E.tag=0;_tmp6E.f1=(void*)_tmp61;_tmp6E.f2=(char)_tmp5E;_tmp6E;});
_tmp6D;}));_tmp6C;});_tmp6B;})));goto _LL46;_LL49: if(*((int*)_tmp60)!= 5)goto
_LL4B;_tmp63=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp60)->f1;_tmp64=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp60)->f2;if((int)_tmp64 != 1)goto _LL4B;_LL4A: t=
_tmp63 == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Const_e_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F[
0]=({struct Cyc_Absyn_Const_e_struct _tmp70;_tmp70.tag=0;_tmp70.f1=(void*)((void*)({
struct Cyc_Absyn_Short_c_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71[0]=({
struct Cyc_Absyn_Short_c_struct _tmp72;_tmp72.tag=1;_tmp72.f1=(void*)_tmp63;_tmp72.f2=(
short)_tmp5E;_tmp72;});_tmp71;}));_tmp70;});_tmp6F;})));goto _LL46;_LL4B: if(*((
int*)_tmp60)!= 5)goto _LL4D;_tmp65=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp60)->f1;_tmp66=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp60)->f2;if((int)
_tmp66 != 2)goto _LL4D;_LL4C: t=_tmp65 == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL4D: if(*((int*)_tmp60)!= 5)goto _LL4F;_tmp67=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp60)->f1;_tmp68=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp60)->f2;if((int)
_tmp68 != 3)goto _LL4F;_LL4E: t=_tmp67 == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL4F: if(*((int*)_tmp60)!= 4)goto _LL51;if(!(_tmp5E == 0))goto _LL51;
_LL50:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp73=
_cycalloc(sizeof(*_tmp73));_tmp73[0]=({struct Cyc_Absyn_Const_e_struct _tmp74;
_tmp74.tag=0;_tmp74.f1=(void*)((void*)0);_tmp74;});_tmp73;})));{struct Cyc_List_List*
_tmp75=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76[0]=({struct Cyc_Absyn_PointerType_struct
_tmp77;_tmp77.tag=4;_tmp77.f1=({struct Cyc_Absyn_PtrInfo _tmp78;_tmp78.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->v=_tmp75;_tmp7B;}));_tmp78.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmp78.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp79;_tmp79.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp7A=_cycalloc(sizeof(*
_tmp7A));_tmp7A->v=_tmp75;_tmp7A;}));_tmp79.nullable=Cyc_Absyn_true_conref;
_tmp79.bounds=Cyc_Absyn_empty_conref();_tmp79.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp79.ptrloc=0;_tmp79;});_tmp78;});_tmp77;});_tmp76;});
goto _LL46;}_LL51: if(*((int*)_tmp60)!= 17)goto _LL53;_tmp69=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp60)->f1;_LL52: {struct Cyc_Absyn_TypeInt_struct*_tmp7C=({struct Cyc_Absyn_TypeInt_struct*
_tmp83=_cycalloc_atomic(sizeof(*_tmp83));_tmp83[0]=({struct Cyc_Absyn_TypeInt_struct
_tmp84;_tmp84.tag=18;_tmp84.f1=_tmp5E;_tmp84;});_tmp83;});if(!Cyc_Tcutil_unify(
_tmp69,(void*)_tmp7C)){({struct Cyc_String_pa_struct _tmp80;_tmp80.tag=0;_tmp80.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)
_tmp7C));{struct Cyc_String_pa_struct _tmp7F;_tmp7F.tag=0;_tmp7F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp69));{void*
_tmp7D[2]={& _tmp7F,& _tmp80};Cyc_Tcutil_terr(loc,({const char*_tmp7E="expecting %s but found %s";
_tag_dynforward(_tmp7E,sizeof(char),_get_zero_arr_size(_tmp7E,26));}),
_tag_dynforward(_tmp7D,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}t=(
void*)({struct Cyc_Absyn_TagType_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81[
0]=({struct Cyc_Absyn_TagType_struct _tmp82;_tmp82.tag=17;_tmp82.f1=(void*)((void*)
_tmp7C);_tmp82;});_tmp81;});goto _LL46;}_LL53:;_LL54: t=_tmp5D == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL46:;}goto _LL33;_LL42: if(*((int*)_tmp57)!= 5)goto
_LL44;_tmp5F=((struct Cyc_Absyn_String_c_struct*)_tmp57)->f1;_LL43: {int len=(int)
_get_dynforward_size(_tmp5F,sizeof(char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_Int_c_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90[0]=({
struct Cyc_Absyn_Int_c_struct _tmp91;_tmp91.tag=2;_tmp91.f1=(void*)((void*)1);
_tmp91.f2=len;_tmp91;});_tmp90;}),loc);elen->topt=({struct Cyc_Core_Opt*_tmp85=
_cycalloc(sizeof(*_tmp85));_tmp85->v=(void*)Cyc_Absyn_uint_typ;_tmp85;});t=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp87;_tmp87.tag=0;_tmp87.f1=elen;_tmp87;});_tmp86;}),Cyc_Absyn_true_conref);
if(topt != 0){void*_tmp88=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo
_tmp89;struct Cyc_Absyn_Tqual _tmp8A;_LL5D: if(_tmp88 <= (void*)4)goto _LL61;if(*((
int*)_tmp88)!= 7)goto _LL5F;_tmp89=((struct Cyc_Absyn_ArrayType_struct*)_tmp88)->f1;
_tmp8A=_tmp89.tq;_LL5E: return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,_tmp8A,(
struct Cyc_Absyn_Exp*)elen,((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),
0);_LL5F: if(*((int*)_tmp88)!= 4)goto _LL61;_LL60: if(!Cyc_Tcutil_unify(*topt,t)
 && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*
_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->v=(void*)t;_tmp8B;});Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);t=*topt;}else{t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp8C=_cycalloc(sizeof(*
_tmp8C));_tmp8C->v=Cyc_Tcenv_lookup_type_vars(te);_tmp8C;})),Cyc_Absyn_const_tqual(
0),(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));
_tmp8D[0]=({struct Cyc_Absyn_Upper_b_struct _tmp8E;_tmp8E.tag=0;_tmp8E.f1=elen;
_tmp8E;});_tmp8D;}),Cyc_Absyn_true_conref);if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*_tmp8F=_cycalloc(sizeof(*_tmp8F));
_tmp8F->v=(void*)t;_tmp8F;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);t=*
topt;}}goto _LL5C;_LL61:;_LL62: goto _LL5C;_LL5C:;}return t;}_LL44: if((int)_tmp57 != 
0)goto _LL33;_LL45: {struct Cyc_List_List*_tmp92=Cyc_Tcenv_lookup_type_vars(te);t=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp93=_cycalloc(sizeof(*_tmp93));
_tmp93[0]=({struct Cyc_Absyn_PointerType_struct _tmp94;_tmp94.tag=4;_tmp94.f1=({
struct Cyc_Absyn_PtrInfo _tmp95;_tmp95.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp98=_cycalloc(sizeof(*
_tmp98));_tmp98->v=_tmp92;_tmp98;}));_tmp95.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp95.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp96;_tmp96.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp97=_cycalloc(sizeof(*
_tmp97));_tmp97->v=_tmp92;_tmp97;}));_tmp96.nullable=Cyc_Absyn_true_conref;
_tmp96.bounds=Cyc_Absyn_empty_conref();_tmp96.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp96.ptrloc=0;_tmp96;});_tmp95;});_tmp94;});_tmp93;});
goto _LL33;}_LL33:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q,void*b){void*_tmp99=b;struct Cyc_Absyn_Vardecl*
_tmp9A;struct Cyc_Absyn_Fndecl*_tmp9B;struct Cyc_Absyn_Vardecl*_tmp9C;struct Cyc_Absyn_Vardecl*
_tmp9D;struct Cyc_Absyn_Vardecl*_tmp9E;_LL64: if((int)_tmp99 != 0)goto _LL66;_LL65:
return({struct Cyc_String_pa_struct _tmpA1;_tmpA1.tag=0;_tmpA1.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(q));{void*_tmp9F[
1]={& _tmpA1};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmpA0="undeclared identifier: %s";
_tag_dynforward(_tmpA0,sizeof(char),_get_zero_arr_size(_tmpA0,26));}),
_tag_dynforward(_tmp9F,sizeof(void*),1));}});_LL66: if(_tmp99 <= (void*)1)goto
_LL68;if(*((int*)_tmp99)!= 0)goto _LL68;_tmp9A=((struct Cyc_Absyn_Global_b_struct*)
_tmp99)->f1;_LL67:*q=*_tmp9A->name;return(void*)_tmp9A->type;_LL68: if(_tmp99 <= (
void*)1)goto _LL6A;if(*((int*)_tmp99)!= 1)goto _LL6A;_tmp9B=((struct Cyc_Absyn_Funname_b_struct*)
_tmp99)->f1;_LL69:*q=*_tmp9B->name;return Cyc_Tcutil_fndecl2typ(_tmp9B);_LL6A: if(
_tmp99 <= (void*)1)goto _LL6C;if(*((int*)_tmp99)!= 4)goto _LL6C;_tmp9C=((struct Cyc_Absyn_Pat_b_struct*)
_tmp99)->f1;_LL6B: _tmp9D=_tmp9C;goto _LL6D;_LL6C: if(_tmp99 <= (void*)1)goto _LL6E;
if(*((int*)_tmp99)!= 3)goto _LL6E;_tmp9D=((struct Cyc_Absyn_Local_b_struct*)_tmp99)->f1;
_LL6D: _tmp9E=_tmp9D;goto _LL6F;_LL6E: if(_tmp99 <= (void*)1)goto _LL63;if(*((int*)
_tmp99)!= 2)goto _LL63;_tmp9E=((struct Cyc_Absyn_Param_b_struct*)_tmp99)->f1;_LL6F:(*
q).f1=(void*)0;return(void*)_tmp9E->type;_LL63:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,
struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _dynforward_ptr,
struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{void*_tmpA2=(void*)
fmt->r;void*_tmpA3;struct _dynforward_ptr _tmpA4;struct Cyc_Absyn_Exp*_tmpA5;struct
Cyc_Absyn_Exp _tmpA6;void*_tmpA7;void*_tmpA8;struct _dynforward_ptr _tmpA9;_LL71:
if(*((int*)_tmpA2)!= 0)goto _LL73;_tmpA3=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmpA2)->f1;if(_tmpA3 <= (void*)1)goto _LL73;if(*((int*)_tmpA3)!= 5)goto _LL73;
_tmpA4=((struct Cyc_Absyn_String_c_struct*)_tmpA3)->f1;_LL72: _tmpA9=_tmpA4;goto
_LL74;_LL73: if(*((int*)_tmpA2)!= 15)goto _LL75;_tmpA5=((struct Cyc_Absyn_Cast_e_struct*)
_tmpA2)->f2;_tmpA6=*_tmpA5;_tmpA7=(void*)_tmpA6.r;if(*((int*)_tmpA7)!= 0)goto
_LL75;_tmpA8=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmpA7)->f1;if(_tmpA8 <= (
void*)1)goto _LL75;if(*((int*)_tmpA8)!= 5)goto _LL75;_tmpA9=((struct Cyc_Absyn_String_c_struct*)
_tmpA8)->f1;_LL74: desc_types=type_getter(te,(struct _dynforward_ptr)_tmpA9,fmt->loc);
goto _LL70;_LL75:;_LL76: if(opt_args != 0){struct Cyc_List_List*_tmpAA=(struct Cyc_List_List*)
opt_args->v;for(0;_tmpAA != 0;_tmpAA=_tmpAA->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmpAA->hd);}}return;_LL70:;}if(opt_args != 0){struct Cyc_List_List*_tmpAB=(struct
Cyc_List_List*)opt_args->v;for(0;desc_types != 0  && _tmpAB != 0;(desc_types=
desc_types->tl,_tmpAB=_tmpAB->tl)){void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*
e=(struct Cyc_Absyn_Exp*)_tmpAB->hd;Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(
te,e,t)){({struct Cyc_String_pa_struct _tmpAF;_tmpAF.tag=0;_tmpAF.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct _tmpAE;
_tmpAE.tag=0;_tmpAE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpAC[2]={& _tmpAE,& _tmpAF};Cyc_Tcutil_terr(e->loc,({const char*_tmpAD="descriptor has type \n%s\n but argument has type \n%s";
_tag_dynforward(_tmpAD,sizeof(char),_get_zero_arr_size(_tmpAD,51));}),
_tag_dynforward(_tmpAC,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}if(
desc_types != 0)({void*_tmpB0[0]={};Cyc_Tcutil_terr(fmt->loc,({const char*_tmpB1="too few arguments";
_tag_dynforward(_tmpB1,sizeof(char),_get_zero_arr_size(_tmpB1,18));}),
_tag_dynforward(_tmpB0,sizeof(void*),0));});if(_tmpAB != 0)({void*_tmpB2[0]={};
Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpAB->hd)->loc,({const char*_tmpB3="too many arguments";
_tag_dynforward(_tmpB3,sizeof(char),_get_zero_arr_size(_tmpB3,19));}),
_tag_dynforward(_tmpB2,sizeof(void*),0));});}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmpB4=p;_LL78: if((int)_tmpB4 != 0)goto _LL7A;_LL79:
goto _LL7B;_LL7A: if((int)_tmpB4 != 2)goto _LL7C;_LL7B: if(!Cyc_Tcutil_is_numeric(e))({
struct Cyc_String_pa_struct _tmpB7;_tmpB7.tag=0;_tmpB7.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpB5[1]={& _tmpB7};Cyc_Tcutil_terr(
loc,({const char*_tmpB6="expecting arithmetic type but found %s";_tag_dynforward(
_tmpB6,sizeof(char),_get_zero_arr_size(_tmpB6,39));}),_tag_dynforward(_tmpB5,
sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
_LL7C: if((int)_tmpB4 != 11)goto _LL7E;_LL7D: Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_String_pa_struct _tmpBA;_tmpBA.tag=
0;_tmpBA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpB8[1]={& _tmpBA};Cyc_Tcutil_terr(loc,({const char*_tmpB9="expecting integral or * type but found %s";
_tag_dynforward(_tmpB9,sizeof(char),_get_zero_arr_size(_tmpB9,42));}),
_tag_dynforward(_tmpB8,sizeof(void*),1));}});return Cyc_Absyn_sint_typ;_LL7E: if((
int)_tmpB4 != 12)goto _LL80;_LL7F: if(!Cyc_Tcutil_is_integral(e))({struct Cyc_String_pa_struct
_tmpBD;_tmpBD.tag=0;_tmpBD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpBB[1]={& _tmpBD};Cyc_Tcutil_terr(loc,({const char*_tmpBC="expecting integral type but found %s";
_tag_dynforward(_tmpBC,sizeof(char),_get_zero_arr_size(_tmpBC,37));}),
_tag_dynforward(_tmpBB,sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;_LL80: if((int)_tmpB4 != 19)goto _LL82;_LL81:{void*_tmpBE=
t;struct Cyc_Absyn_PtrInfo _tmpBF;struct Cyc_Absyn_PtrAtts _tmpC0;struct Cyc_Absyn_Conref*
_tmpC1;_LL85: if(_tmpBE <= (void*)4)goto _LL89;if(*((int*)_tmpBE)!= 7)goto _LL87;
_LL86: goto _LL84;_LL87: if(*((int*)_tmpBE)!= 4)goto _LL89;_tmpBF=((struct Cyc_Absyn_PointerType_struct*)
_tmpBE)->f1;_tmpC0=_tmpBF.ptr_atts;_tmpC1=_tmpC0.bounds;_LL88:{void*_tmpC2=(void*)(
Cyc_Absyn_compress_conref(_tmpC1))->v;void*_tmpC3;void*_tmpC4;void*_tmpC5;_LL8C:
if(_tmpC2 <= (void*)1)goto _LL92;if(*((int*)_tmpC2)!= 0)goto _LL8E;_tmpC3=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpC2)->f1;if((int)_tmpC3 != 0)goto _LL8E;_LL8D:
goto _LL8F;_LL8E: if(*((int*)_tmpC2)!= 0)goto _LL90;_tmpC4=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpC2)->f1;if((int)_tmpC4 != 1)goto _LL90;_LL8F: goto _LL91;_LL90: if(*((int*)_tmpC2)
!= 0)goto _LL92;_tmpC5=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpC2)->f1;if(
_tmpC5 <= (void*)2)goto _LL92;if(*((int*)_tmpC5)!= 0)goto _LL92;_LL91: goto _LL8B;
_LL92:;_LL93:({void*_tmpC6[0]={};Cyc_Tcutil_terr(loc,({const char*_tmpC7="can't apply size to pointer/array of abstract length";
_tag_dynforward(_tmpC7,sizeof(char),_get_zero_arr_size(_tmpC7,53));}),
_tag_dynforward(_tmpC6,sizeof(void*),0));});_LL8B:;}goto _LL84;_LL89:;_LL8A:({
struct Cyc_String_pa_struct _tmpCA;_tmpCA.tag=0;_tmpCA.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpC8[1]={& _tmpCA};Cyc_Tcutil_terr(
loc,({const char*_tmpC9="size requires pointer or array type, not %s";
_tag_dynforward(_tmpC9,sizeof(char),_get_zero_arr_size(_tmpC9,44));}),
_tag_dynforward(_tmpC8,sizeof(void*),1));}});_LL84:;}return Cyc_Absyn_uint_typ;
_LL82:;_LL83:({void*_tmpCB[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpCC="Non-unary primop";
_tag_dynforward(_tmpCC,sizeof(char),_get_zero_arr_size(_tmpCC,17));}),
_tag_dynforward(_tmpCB,sizeof(void*),0));});_LL77:;}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){({struct Cyc_String_pa_struct
_tmpCF;_tmpCF.tag=0;_tmpCF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmpCD[1]={& _tmpCF};
Cyc_Tcutil_terr(e1->loc,({const char*_tmpCE="type %s cannot be used here";
_tag_dynforward(_tmpCE,sizeof(char),_get_zero_arr_size(_tmpCE,28));}),
_tag_dynforward(_tmpCD,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->v=Cyc_Tcenv_lookup_type_vars(te);
_tmpD0;}));}if(!checker(e2)){({struct Cyc_String_pa_struct _tmpD3;_tmpD3.tag=0;
_tmpD3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmpD1[1]={& _tmpD3};
Cyc_Tcutil_terr(e2->loc,({const char*_tmpD2="type %s cannot be used here";
_tag_dynforward(_tmpD2,sizeof(char),_get_zero_arr_size(_tmpD2,28));}),
_tag_dynforward(_tmpD1,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->v=Cyc_Tcenv_lookup_type_vars(te);
_tmpD4;}));}{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);return Cyc_Tcutil_max_arithmetic_type(t1,t2);}}static
void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
void*_tmpD5=t1;struct Cyc_Absyn_PtrInfo _tmpD6;void*_tmpD7;struct Cyc_Absyn_Tqual
_tmpD8;struct Cyc_Absyn_PtrAtts _tmpD9;void*_tmpDA;struct Cyc_Absyn_Conref*_tmpDB;
struct Cyc_Absyn_Conref*_tmpDC;struct Cyc_Absyn_Conref*_tmpDD;_LL95: if(_tmpD5 <= (
void*)4)goto _LL97;if(*((int*)_tmpD5)!= 4)goto _LL97;_tmpD6=((struct Cyc_Absyn_PointerType_struct*)
_tmpD5)->f1;_tmpD7=(void*)_tmpD6.elt_typ;_tmpD8=_tmpD6.elt_tq;_tmpD9=_tmpD6.ptr_atts;
_tmpDA=(void*)_tmpD9.rgn;_tmpDB=_tmpD9.nullable;_tmpDC=_tmpD9.bounds;_tmpDD=
_tmpD9.zero_term;_LL96: if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpD7),(void*)
1))({void*_tmpDE[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpDF="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmpDF,sizeof(char),_get_zero_arr_size(_tmpDF,50));}),
_tag_dynforward(_tmpDE,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2))({
struct Cyc_String_pa_struct _tmpE2;_tmpE2.tag=0;_tmpE2.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{void*_tmpE0[1]={& _tmpE2};Cyc_Tcutil_terr(
e2->loc,({const char*_tmpE1="expecting int but found %s";_tag_dynforward(_tmpE1,
sizeof(char),_get_zero_arr_size(_tmpE1,27));}),_tag_dynforward(_tmpE0,sizeof(
void*),1));}});_tmpDC=Cyc_Absyn_compress_conref(_tmpDC);{void*_tmpE3=(void*)
_tmpDC->v;void*_tmpE4;void*_tmpE5;void*_tmpE6;struct Cyc_Absyn_Exp*_tmpE7;void*
_tmpE8;_LL9A: if(_tmpE3 <= (void*)1)goto _LLA2;if(*((int*)_tmpE3)!= 0)goto _LL9C;
_tmpE4=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpE3)->f1;if((int)_tmpE4 != 0)
goto _LL9C;_LL9B: return t1;_LL9C: if(*((int*)_tmpE3)!= 0)goto _LL9E;_tmpE5=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpE3)->f1;if((int)_tmpE5 != 1)goto _LL9E;_LL9D:
return t1;_LL9E: if(*((int*)_tmpE3)!= 0)goto _LLA0;_tmpE6=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpE3)->f1;if(_tmpE6 <= (void*)2)goto _LLA0;if(*((int*)_tmpE6)!= 0)goto _LLA0;
_tmpE7=((struct Cyc_Absyn_Upper_b_struct*)_tmpE6)->f1;_LL9F: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpDD))({void*_tmpE9[0]={};
Cyc_Tcutil_warn(e1->loc,({const char*_tmpEA="pointer arithmetic on thin, zero-terminated pointer may be expensive.";
_tag_dynforward(_tmpEA,sizeof(char),_get_zero_arr_size(_tmpEA,70));}),
_tag_dynforward(_tmpE9,sizeof(void*),0));});{struct Cyc_Absyn_PointerType_struct*
_tmpEB=({struct Cyc_Absyn_PointerType_struct*_tmpEC=_cycalloc(sizeof(*_tmpEC));
_tmpEC[0]=({struct Cyc_Absyn_PointerType_struct _tmpED;_tmpED.tag=4;_tmpED.f1=({
struct Cyc_Absyn_PtrInfo _tmpEE;_tmpEE.elt_typ=(void*)_tmpD7;_tmpEE.elt_tq=_tmpD8;
_tmpEE.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpEF;_tmpEF.rgn=(void*)_tmpDA;_tmpEF.nullable=
Cyc_Absyn_true_conref;_tmpEF.bounds=Cyc_Absyn_bounds_dyneither_conref;_tmpEF.zero_term=
_tmpDD;_tmpEF.ptrloc=0;_tmpEF;});_tmpEE;});_tmpED;});_tmpEC;});Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmpEB,(void*)3);return(void*)_tmpEB;}_LLA0: if(*((int*)_tmpE3)!= 0)
goto _LLA2;_tmpE8=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpE3)->f1;if(
_tmpE8 <= (void*)2)goto _LLA2;if(*((int*)_tmpE8)!= 1)goto _LLA2;_LLA1:({void*_tmpF0[
0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpF1="pointer arithmetic not allowed on pointers with abstract bounds";
_tag_dynforward(_tmpF1,sizeof(char),_get_zero_arr_size(_tmpF1,64));}),
_tag_dynforward(_tmpF0,sizeof(void*),0));});return t1;_LLA2:;_LLA3:(void*)(_tmpDC->v=(
void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmpF2=_cycalloc(sizeof(*_tmpF2));
_tmpF2[0]=({struct Cyc_Absyn_Eq_constr_struct _tmpF3;_tmpF3.tag=0;_tmpF3.f1=(void*)((
void*)0);_tmpF3;});_tmpF2;})));return t1;_LL99:;}_LL97:;_LL98: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);_LL94:;}static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;
void*t1_elt=(void*)0;int forward_only1=0;int forward_only2=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& t1_elt,& forward_only1)){if(Cyc_Tcutil_is_tagged_pointer_typ(t2,&
forward_only2)){if(!Cyc_Tcutil_unify(t1,t2)){({struct Cyc_String_pa_struct _tmpF7;
_tmpF7.tag=0;_tmpF7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{struct Cyc_String_pa_struct
_tmpF6;_tmpF6.tag=0;_tmpF6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmpF4[2]={& _tmpF6,&
_tmpF7};Cyc_Tcutil_terr(e1->loc,({const char*_tmpF5="pointer arithmetic on values of different types (%s != %s)";
_tag_dynforward(_tmpF5,sizeof(char),_get_zero_arr_size(_tmpF5,59));}),
_tag_dynforward(_tmpF4,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return Cyc_Absyn_sint_typ;}else{if(forward_only1)({void*_tmpF8[0]={};Cyc_Tcutil_terr(
e1->loc,({const char*_tmpF9="can't subtract from forward-only ? pointer";
_tag_dynforward(_tmpF9,sizeof(char),_get_zero_arr_size(_tmpF9,43));}),
_tag_dynforward(_tmpF8,sizeof(void*),0));});if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1_elt),(void*)1))({void*_tmpFA[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpFB="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmpFB,sizeof(char),_get_zero_arr_size(_tmpFB,50));}),
_tag_dynforward(_tmpFA,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){({
struct Cyc_String_pa_struct _tmpFF;_tmpFF.tag=0;_tmpFF.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmpFE;_tmpFE.tag=0;_tmpFE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmpFC[2]={& _tmpFE,& _tmpFF};Cyc_Tcutil_terr(e2->loc,({const char*
_tmpFD="expecting either %s or int but found %s";_tag_dynforward(_tmpFD,sizeof(
char),_get_zero_arr_size(_tmpFD,40));}),_tag_dynforward(_tmpFC,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}return t1;}}if(Cyc_Tcutil_is_pointer_type(t1))Cyc_Tcutil_unchecked_cast(
te,e1,Cyc_Absyn_sint_typ,(void*)3);if(Cyc_Tcutil_is_pointer_type(t2))Cyc_Tcutil_unchecked_cast(
te,e2,Cyc_Absyn_sint_typ,(void*)3);return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int e1_is_num=Cyc_Tcutil_is_numeric(
e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(e1_is_num  && e2_is_num)return
Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_unify(t1,t2) && Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,({struct Cyc_Core_Opt*_tmp100=_cycalloc(
sizeof(*_tmp100));_tmp100->v=Cyc_Tcenv_lookup_type_vars(te);_tmp100;}))))return
Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(
te,e2,t1,(void*)3);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_silent_castable(
te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e1,t2,(void*)3);return Cyc_Absyn_sint_typ;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{{struct _tuple0 _tmp102=({struct _tuple0 _tmp101;
_tmp101.f1=Cyc_Tcutil_compress(t1);_tmp101.f2=Cyc_Tcutil_compress(t2);_tmp101;});
void*_tmp103;struct Cyc_Absyn_PtrInfo _tmp104;void*_tmp105;void*_tmp106;struct Cyc_Absyn_PtrInfo
_tmp107;void*_tmp108;_LLA5: _tmp103=_tmp102.f1;if(_tmp103 <= (void*)4)goto _LLA7;
if(*((int*)_tmp103)!= 4)goto _LLA7;_tmp104=((struct Cyc_Absyn_PointerType_struct*)
_tmp103)->f1;_tmp105=(void*)_tmp104.elt_typ;_tmp106=_tmp102.f2;if(_tmp106 <= (
void*)4)goto _LLA7;if(*((int*)_tmp106)!= 4)goto _LLA7;_tmp107=((struct Cyc_Absyn_PointerType_struct*)
_tmp106)->f1;_tmp108=(void*)_tmp107.elt_typ;_LLA6: if(Cyc_Tcutil_unify(_tmp105,
_tmp108))return Cyc_Absyn_sint_typ;goto _LLA4;_LLA7:;_LLA8: goto _LLA4;_LLA4:;}({
struct Cyc_String_pa_struct _tmp10C;_tmp10C.tag=0;_tmp10C.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmp10B;_tmp10B.tag=0;_tmp10B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp109[2]={& _tmp10B,& _tmp10C};Cyc_Tcutil_terr(
loc,({const char*_tmp10A="comparison not allowed between %s and %s";
_tag_dynforward(_tmp10A,sizeof(char),_get_zero_arr_size(_tmp10A,41));}),
_tag_dynforward(_tmp109,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp10D=_cycalloc(sizeof(*_tmp10D));
_tmp10D->v=Cyc_Tcenv_lookup_type_vars(te);_tmp10D;}));}}}}}}static void*Cyc_Tcexp_tcBinPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp10E=p;_LLAA: if((int)_tmp10E != 
0)goto _LLAC;_LLAB: return Cyc_Tcexp_tcPlus(te,e1,e2);_LLAC: if((int)_tmp10E != 2)
goto _LLAE;_LLAD: return Cyc_Tcexp_tcMinus(te,e1,e2);_LLAE: if((int)_tmp10E != 1)goto
_LLB0;_LLAF: goto _LLB1;_LLB0: if((int)_tmp10E != 3)goto _LLB2;_LLB1: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);_LLB2: if((int)_tmp10E != 4)goto _LLB4;_LLB3: goto
_LLB5;_LLB4: if((int)_tmp10E != 13)goto _LLB6;_LLB5: goto _LLB7;_LLB6: if((int)_tmp10E
!= 14)goto _LLB8;_LLB7: goto _LLB9;_LLB8: if((int)_tmp10E != 15)goto _LLBA;_LLB9: goto
_LLBB;_LLBA: if((int)_tmp10E != 16)goto _LLBC;_LLBB: goto _LLBD;_LLBC: if((int)_tmp10E
!= 17)goto _LLBE;_LLBD: goto _LLBF;_LLBE: if((int)_tmp10E != 18)goto _LLC0;_LLBF:
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);_LLC0: if((int)
_tmp10E != 5)goto _LLC2;_LLC1: goto _LLC3;_LLC2: if((int)_tmp10E != 6)goto _LLC4;_LLC3:
goto _LLC5;_LLC4: if((int)_tmp10E != 7)goto _LLC6;_LLC5: goto _LLC7;_LLC6: if((int)
_tmp10E != 8)goto _LLC8;_LLC7: goto _LLC9;_LLC8: if((int)_tmp10E != 9)goto _LLCA;_LLC9:
goto _LLCB;_LLCA: if((int)_tmp10E != 10)goto _LLCC;_LLCB: return Cyc_Tcexp_tcAnyBinop(
te,loc,e1,e2);_LLCC:;_LLCD:({void*_tmp10F[0]={};((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp110="bad binary primop";
_tag_dynforward(_tmp110,sizeof(char),_get_zero_arr_size(_tmp110,18));}),
_tag_dynforward(_tmp10F,sizeof(void*),0));});_LLA9:;}static void*Cyc_Tcexp_tcPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_List_List*es){if(p == (void*)2  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
es)== 1)return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd);Cyc_Tcexp_tcExpList(te,es);{void*t;switch(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLCE: return({void*_tmp111[0]={};
Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp112="primitive operator has 0 arguments";
_tag_dynforward(_tmp112,sizeof(char),_get_zero_arr_size(_tmp112,35));}),
_tag_dynforward(_tmp111,sizeof(void*),0));});case 1: _LLCF: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLD0: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLD1: return({void*_tmp113[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp114="primitive operator has > 2 arguments";_tag_dynforward(_tmp114,sizeof(
char),_get_zero_arr_size(_tmp114,37));}),_tag_dynforward(_tmp113,sizeof(void*),0));});}
return t;}}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){{void*_tmp115=(void*)e->r;void*
_tmp116;struct Cyc_Absyn_Vardecl*_tmp117;void*_tmp118;struct Cyc_Absyn_Vardecl*
_tmp119;void*_tmp11A;struct Cyc_Absyn_Vardecl*_tmp11B;void*_tmp11C;struct Cyc_Absyn_Vardecl*
_tmp11D;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*
_tmp120;struct _dynforward_ptr*_tmp121;struct Cyc_Absyn_Exp*_tmp122;struct
_dynforward_ptr*_tmp123;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp125;
struct Cyc_Absyn_Exp*_tmp126;_LLD4: if(*((int*)_tmp115)!= 1)goto _LLD6;_tmp116=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp115)->f2;if(_tmp116 <= (void*)1)goto
_LLD6;if(*((int*)_tmp116)!= 2)goto _LLD6;_tmp117=((struct Cyc_Absyn_Param_b_struct*)
_tmp116)->f1;_LLD5: _tmp119=_tmp117;goto _LLD7;_LLD6: if(*((int*)_tmp115)!= 1)goto
_LLD8;_tmp118=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp115)->f2;if(_tmp118 <= (
void*)1)goto _LLD8;if(*((int*)_tmp118)!= 3)goto _LLD8;_tmp119=((struct Cyc_Absyn_Local_b_struct*)
_tmp118)->f1;_LLD7: _tmp11B=_tmp119;goto _LLD9;_LLD8: if(*((int*)_tmp115)!= 1)goto
_LLDA;_tmp11A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp115)->f2;if(_tmp11A <= (
void*)1)goto _LLDA;if(*((int*)_tmp11A)!= 4)goto _LLDA;_tmp11B=((struct Cyc_Absyn_Pat_b_struct*)
_tmp11A)->f1;_LLD9: _tmp11D=_tmp11B;goto _LLDB;_LLDA: if(*((int*)_tmp115)!= 1)goto
_LLDC;_tmp11C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp115)->f2;if(_tmp11C <= (
void*)1)goto _LLDC;if(*((int*)_tmp11C)!= 0)goto _LLDC;_tmp11D=((struct Cyc_Absyn_Global_b_struct*)
_tmp11C)->f1;_LLDB: if(!(_tmp11D->tq).real_const)return;goto _LLD3;_LLDC: if(*((int*)
_tmp115)!= 25)goto _LLDE;_tmp11E=((struct Cyc_Absyn_Subscript_e_struct*)_tmp115)->f1;
_tmp11F=((struct Cyc_Absyn_Subscript_e_struct*)_tmp115)->f2;_LLDD:{void*_tmp127=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp11E->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp128;struct Cyc_Absyn_Tqual _tmp129;struct Cyc_Absyn_ArrayInfo
_tmp12A;struct Cyc_Absyn_Tqual _tmp12B;struct Cyc_List_List*_tmp12C;_LLEB: if(
_tmp127 <= (void*)4)goto _LLF1;if(*((int*)_tmp127)!= 4)goto _LLED;_tmp128=((struct
Cyc_Absyn_PointerType_struct*)_tmp127)->f1;_tmp129=_tmp128.elt_tq;_LLEC: _tmp12B=
_tmp129;goto _LLEE;_LLED: if(*((int*)_tmp127)!= 7)goto _LLEF;_tmp12A=((struct Cyc_Absyn_ArrayType_struct*)
_tmp127)->f1;_tmp12B=_tmp12A.tq;_LLEE: if(!_tmp12B.real_const)return;goto _LLEA;
_LLEF: if(*((int*)_tmp127)!= 9)goto _LLF1;_tmp12C=((struct Cyc_Absyn_TupleType_struct*)
_tmp127)->f1;_LLF0: {unsigned int _tmp12E;int _tmp12F;struct _tuple7 _tmp12D=Cyc_Evexp_eval_const_uint_exp(
_tmp11F);_tmp12E=_tmp12D.f1;_tmp12F=_tmp12D.f2;if(!_tmp12F){({void*_tmp130[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp131="tuple projection cannot use sizeof or offsetof";
_tag_dynforward(_tmp131,sizeof(char),_get_zero_arr_size(_tmp131,47));}),
_tag_dynforward(_tmp130,sizeof(void*),0));});return;}{struct _handler_cons _tmp132;
_push_handler(& _tmp132);{int _tmp134=0;if(setjmp(_tmp132.handler))_tmp134=1;if(!
_tmp134){{struct _tuple9 _tmp136;struct Cyc_Absyn_Tqual _tmp137;struct _tuple9*
_tmp135=((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp12C,(
int)_tmp12E);_tmp136=*_tmp135;_tmp137=_tmp136.f1;if(!_tmp137.real_const){
_npop_handler(0);return;}};_pop_handler();}else{void*_tmp133=(void*)_exn_thrown;
void*_tmp139=_tmp133;_LLF4: if(_tmp139 != Cyc_List_Nth)goto _LLF6;_LLF5: return;
_LLF6:;_LLF7:(void)_throw(_tmp139);_LLF3:;}}}goto _LLEA;}_LLF1:;_LLF2: goto _LLEA;
_LLEA:;}goto _LLD3;_LLDE: if(*((int*)_tmp115)!= 23)goto _LLE0;_tmp120=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp115)->f1;_tmp121=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp115)->f2;_LLDF:{
void*_tmp13A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp120->topt))->v);struct Cyc_Absyn_AggrInfo _tmp13B;void*_tmp13C;struct Cyc_Absyn_Aggrdecl**
_tmp13D;struct Cyc_List_List*_tmp13E;_LLF9: if(_tmp13A <= (void*)4)goto _LLFD;if(*((
int*)_tmp13A)!= 10)goto _LLFB;_tmp13B=((struct Cyc_Absyn_AggrType_struct*)_tmp13A)->f1;
_tmp13C=(void*)_tmp13B.aggr_info;if(*((int*)_tmp13C)!= 1)goto _LLFB;_tmp13D=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp13C)->f1;_LLFA: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp13D == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp13D,_tmp121);if(sf == 0  || !(sf->tq).real_const)return;goto _LLF8;}_LLFB: if(*((
int*)_tmp13A)!= 11)goto _LLFD;_tmp13E=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp13A)->f2;_LLFC: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp13E,
_tmp121);if(sf == 0  || !(sf->tq).real_const)return;goto _LLF8;}_LLFD:;_LLFE: goto
_LLF8;_LLF8:;}goto _LLD3;_LLE0: if(*((int*)_tmp115)!= 24)goto _LLE2;_tmp122=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp115)->f1;_tmp123=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp115)->f2;_LLE1:{void*_tmp13F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp122->topt))->v);struct Cyc_Absyn_PtrInfo _tmp140;void*_tmp141;
_LL100: if(_tmp13F <= (void*)4)goto _LL102;if(*((int*)_tmp13F)!= 4)goto _LL102;
_tmp140=((struct Cyc_Absyn_PointerType_struct*)_tmp13F)->f1;_tmp141=(void*)
_tmp140.elt_typ;_LL101:{void*_tmp142=Cyc_Tcutil_compress(_tmp141);struct Cyc_Absyn_AggrInfo
_tmp143;void*_tmp144;struct Cyc_Absyn_Aggrdecl**_tmp145;struct Cyc_List_List*
_tmp146;_LL105: if(_tmp142 <= (void*)4)goto _LL109;if(*((int*)_tmp142)!= 10)goto
_LL107;_tmp143=((struct Cyc_Absyn_AggrType_struct*)_tmp142)->f1;_tmp144=(void*)
_tmp143.aggr_info;if(*((int*)_tmp144)!= 1)goto _LL107;_tmp145=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp144)->f1;_LL106: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp145 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp145,_tmp123);if(sf == 0  || !(sf->tq).real_const)
return;goto _LL104;}_LL107: if(*((int*)_tmp142)!= 11)goto _LL109;_tmp146=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp142)->f2;_LL108: {struct Cyc_Absyn_Aggrfield*
sf=Cyc_Absyn_lookup_field(_tmp146,_tmp123);if(sf == 0  || !(sf->tq).real_const)
return;goto _LL104;}_LL109:;_LL10A: goto _LL104;_LL104:;}goto _LLFF;_LL102:;_LL103:
goto _LLFF;_LLFF:;}goto _LLD3;_LLE2: if(*((int*)_tmp115)!= 22)goto _LLE4;_tmp124=((
struct Cyc_Absyn_Deref_e_struct*)_tmp115)->f1;_LLE3:{void*_tmp147=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp124->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp148;struct Cyc_Absyn_Tqual _tmp149;struct Cyc_Absyn_ArrayInfo _tmp14A;struct Cyc_Absyn_Tqual
_tmp14B;_LL10C: if(_tmp147 <= (void*)4)goto _LL110;if(*((int*)_tmp147)!= 4)goto
_LL10E;_tmp148=((struct Cyc_Absyn_PointerType_struct*)_tmp147)->f1;_tmp149=
_tmp148.elt_tq;_LL10D: _tmp14B=_tmp149;goto _LL10F;_LL10E: if(*((int*)_tmp147)!= 7)
goto _LL110;_tmp14A=((struct Cyc_Absyn_ArrayType_struct*)_tmp147)->f1;_tmp14B=
_tmp14A.tq;_LL10F: if(!_tmp14B.real_const)return;goto _LL10B;_LL110:;_LL111: goto
_LL10B;_LL10B:;}goto _LLD3;_LLE4: if(*((int*)_tmp115)!= 13)goto _LLE6;_tmp125=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp115)->f1;_LLE5: _tmp126=_tmp125;goto
_LLE7;_LLE6: if(*((int*)_tmp115)!= 14)goto _LLE8;_tmp126=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp115)->f1;_LLE7: Cyc_Tcexp_check_writable(te,_tmp126);return;_LLE8:;_LLE9: goto
_LLD3;_LLD3:;}({struct Cyc_String_pa_struct _tmp14E;_tmp14E.tag=0;_tmp14E.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{void*
_tmp14C[1]={& _tmp14E};Cyc_Tcutil_terr(e->loc,({const char*_tmp14D="attempt to write a const location: %s";
_tag_dynforward(_tmp14D,sizeof(char),_get_zero_arr_size(_tmp14D,38));}),
_tag_dynforward(_tmp14C,sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,void*i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(!Cyc_Absyn_is_lvalue(e))return({
void*_tmp14F[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp150="increment/decrement of non-lvalue";
_tag_dynforward(_tmp150,sizeof(char),_get_zero_arr_size(_tmp150,34));}),
_tag_dynforward(_tmp14F,sizeof(void*),0));});Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt,& forward_only) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == (
void*)0  || i == (void*)1)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),(
void*)1))({void*_tmp151[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp152="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmp152,sizeof(char),_get_zero_arr_size(_tmp152,50));}),
_tag_dynforward(_tmp151,sizeof(void*),0));});if(forward_only  && (i == (void*)2
 || i == (void*)3))({void*_tmp153[0]={};Cyc_Tcutil_terr(e->loc,({const char*
_tmp154="can't subtract from forward-only ? pointer";_tag_dynforward(_tmp154,
sizeof(char),_get_zero_arr_size(_tmp154,43));}),_tag_dynforward(_tmp153,sizeof(
void*),0));});}else{({struct Cyc_String_pa_struct _tmp157;_tmp157.tag=0;_tmp157.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*
_tmp155[1]={& _tmp157};Cyc_Tcutil_terr(e->loc,({const char*_tmp156="expecting arithmetic or ? type but found %s";
_tag_dynforward(_tmp156,sizeof(char),_get_zero_arr_size(_tmp156,44));}),
_tag_dynforward(_tmp155,sizeof(void*),1));}});}}return t;}}static void*Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){Cyc_Tcexp_tcTest(te,e1,({const
char*_tmp158="conditional expression";_tag_dynforward(_tmp158,sizeof(char),
_get_zero_arr_size(_tmp158,23));}));Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(
te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({
struct Cyc_Core_Opt*_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp161;}));struct Cyc_List_List _tmp159=({struct Cyc_List_List _tmp160;_tmp160.hd=
e3;_tmp160.tl=0;_tmp160;});struct Cyc_List_List _tmp15A=({struct Cyc_List_List
_tmp15F;_tmp15F.hd=e2;_tmp15F.tl=(struct Cyc_List_List*)& _tmp159;_tmp15F;});if(!
Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)& _tmp15A)){({struct Cyc_String_pa_struct
_tmp15E;_tmp15E.tag=0;_tmp15E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v));{
struct Cyc_String_pa_struct _tmp15D;_tmp15D.tag=0;_tmp15D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmp15B[2]={& _tmp15D,& _tmp15E};
Cyc_Tcutil_terr(loc,({const char*_tmp15C="conditional clause types do not match: %s != %s";
_tag_dynforward(_tmp15C,sizeof(char),_get_zero_arr_size(_tmp15C,48));}),
_tag_dynforward(_tmp15B,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t;}}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcTest(te,e1,({
const char*_tmp162="logical-and expression";_tag_dynforward(_tmp162,sizeof(char),
_get_zero_arr_size(_tmp162,23));}));Cyc_Tcexp_tcTest(te,e2,({const char*_tmp163="logical-and expression";
_tag_dynforward(_tmp163,sizeof(char),_get_zero_arr_size(_tmp163,23));}));return
Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcTest(te,e1,({
const char*_tmp164="logical-or expression";_tag_dynforward(_tmp164,sizeof(char),
_get_zero_arr_size(_tmp164,22));}));Cyc_Tcexp_tcTest(te,e2,({const char*_tmp165="logical-or expression";
_tag_dynforward(_tmp165,sizeof(char),_get_zero_arr_size(_tmp165,22));}));return
Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){{struct _RegionHandle _tmp166=_new_region("r");struct
_RegionHandle*r=& _tmp166;_push_region(r);Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(
r,te),0,e1);;_pop_region(r);}Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp167=Cyc_Tcutil_compress(t1);_LL113: if(_tmp167 <= (void*)4)goto _LL115;if(*((
int*)_tmp167)!= 7)goto _LL115;_LL114:({void*_tmp168[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp169="cannot assign to an array";_tag_dynforward(_tmp169,sizeof(
char),_get_zero_arr_size(_tmp169,26));}),_tag_dynforward(_tmp168,sizeof(void*),0));});
goto _LL112;_LL115:;_LL116: goto _LL112;_LL112:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),(void*)1))({void*_tmp16A[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp16B="type is abstract (can't determine size).";
_tag_dynforward(_tmp16B,sizeof(char),_get_zero_arr_size(_tmp16B,41));}),
_tag_dynforward(_tmp16A,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(e1))return({
void*_tmp16C[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp16D="assignment to non-lvalue";
_tag_dynforward(_tmp16D,sizeof(char),_get_zero_arr_size(_tmp16D,25));}),
_tag_dynforward(_tmp16C,sizeof(void*),0));});Cyc_Tcexp_check_writable(te,e1);if(
po == 0){if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){void*_tmp16E=({struct Cyc_String_pa_struct
_tmp172;_tmp172.tag=0;_tmp172.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp171;_tmp171.tag=0;
_tmp171.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp16F[2]={& _tmp171,& _tmp172};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp170="type mismatch: %s != %s";_tag_dynforward(_tmp170,sizeof(char),
_get_zero_arr_size(_tmp170,24));}),_tag_dynforward(_tmp16F,sizeof(void*),2));}}});
Cyc_Tcutil_unify(t1,t2);Cyc_Tcutil_explain_failure();return _tmp16E;}}else{void*
_tmp173=(void*)po->v;void*_tmp174=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp173,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp174,t1) || Cyc_Tcutil_coerceable(_tmp174) && Cyc_Tcutil_coerceable(
t1))){void*_tmp175=({struct Cyc_String_pa_struct _tmp179;_tmp179.tag=0;_tmp179.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp178;_tmp178.tag=0;_tmp178.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp176[2]={& _tmp178,& _tmp179};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp177="Cannot use this operator in an assignment when the arguments have types %s and %s";
_tag_dynforward(_tmp177,sizeof(char),_get_zero_arr_size(_tmp177,82));}),
_tag_dynforward(_tmp176,sizeof(void*),2));}}});Cyc_Tcutil_unify(_tmp174,t1);Cyc_Tcutil_explain_failure();
return _tmp175;}return _tmp174;}return t1;}}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={6,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp17A=Cyc_Tcutil_compress(
t1);void*_tmp17B;void*_tmp17C;_LL118: if((int)_tmp17A != 1)goto _LL11A;_LL119: Cyc_Tcutil_unchecked_cast(
te,e,dbl_typ,(void*)1);t1=dbl_typ;goto _LL117;_LL11A: if(_tmp17A <= (void*)4)goto
_LL11E;if(*((int*)_tmp17A)!= 5)goto _LL11C;_tmp17B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp17A)->f2;if((int)_tmp17B != 0)goto _LL11C;_LL11B: goto _LL11D;_LL11C: if(*((int*)
_tmp17A)!= 5)goto _LL11E;_tmp17C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp17A)->f2;
if((int)_tmp17C != 1)goto _LL11E;_LL11D: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);t1=Cyc_Absyn_sint_typ;goto _LL117;_LL11E:;_LL11F: goto _LL117;_LL117:;}
for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp17E;struct Cyc_List_List*
_tmp17F;struct Cyc_Position_Segment*_tmp180;void*_tmp181;struct Cyc_Absyn_Tunionfield
_tmp17D=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_tmp17E=_tmp17D.name;_tmp17F=
_tmp17D.typs;_tmp180=_tmp17D.loc;_tmp181=(void*)_tmp17D.sc;if(_tmp17F == 0  || 
_tmp17F->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple9*)_tmp17F->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)fields->hd);}}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp183;struct Cyc_List_List*_tmp184;struct Cyc_Position_Segment*
_tmp185;void*_tmp186;struct Cyc_Absyn_Tunionfield _tmp182=*((struct Cyc_Absyn_Tunionfield*)
fields->hd);_tmp183=_tmp182.name;_tmp184=_tmp182.typs;_tmp185=_tmp182.loc;
_tmp186=(void*)_tmp182.sc;if(_tmp184 == 0  || _tmp184->tl != 0)continue;{void*t2=
Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple9*)_tmp184->hd)).f2);if(Cyc_Tcutil_coerce_arg(
te,e,t2))return(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)
fields->hd);}}({struct Cyc_String_pa_struct _tmp18A;_tmp18A.tag=0;_tmp18A.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{
struct Cyc_String_pa_struct _tmp189;_tmp189.tag=0;_tmp189.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(tu));{void*
_tmp187[2]={& _tmp189,& _tmp18A};Cyc_Tcutil_terr(e->loc,({const char*_tmp188="can't find a field in %s to inject a value of type %s";
_tag_dynforward(_tmp188,sizeof(char),_get_zero_arr_size(_tmp188,54));}),
_tag_dynforward(_tmp187,sizeof(void*),2));}}});return 0;}static void*Cyc_Tcexp_tcFnCall(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){
struct Cyc_List_List*_tmp18C=args;struct _RegionHandle _tmp18D=_new_region("ter");
struct _RegionHandle*ter=& _tmp18D;_push_region(ter);{struct Cyc_Tcenv_Tenv*_tmp18E=
Cyc_Tcenv_new_block(ter,loc,te_orig);Cyc_Tcexp_tcExp(_tmp18E,0,e);{void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp18F=t;struct Cyc_Absyn_PtrInfo
_tmp190;void*_tmp191;struct Cyc_Absyn_Tqual _tmp192;struct Cyc_Absyn_PtrAtts _tmp193;
void*_tmp194;struct Cyc_Absyn_Conref*_tmp195;struct Cyc_Absyn_Conref*_tmp196;
struct Cyc_Absyn_Conref*_tmp197;_LL121: if(_tmp18F <= (void*)4)goto _LL123;if(*((int*)
_tmp18F)!= 4)goto _LL123;_tmp190=((struct Cyc_Absyn_PointerType_struct*)_tmp18F)->f1;
_tmp191=(void*)_tmp190.elt_typ;_tmp192=_tmp190.elt_tq;_tmp193=_tmp190.ptr_atts;
_tmp194=(void*)_tmp193.rgn;_tmp195=_tmp193.nullable;_tmp196=_tmp193.bounds;
_tmp197=_tmp193.zero_term;_LL122: Cyc_Tcenv_check_rgn_accessible(_tmp18E,loc,
_tmp194);Cyc_Tcutil_check_nonzero_bound(loc,_tmp196);{void*_tmp198=Cyc_Tcutil_compress(
_tmp191);struct Cyc_Absyn_FnInfo _tmp199;struct Cyc_List_List*_tmp19A;struct Cyc_Core_Opt*
_tmp19B;void*_tmp19C;struct Cyc_List_List*_tmp19D;int _tmp19E;struct Cyc_Absyn_VarargInfo*
_tmp19F;struct Cyc_List_List*_tmp1A0;struct Cyc_List_List*_tmp1A1;_LL126: if(
_tmp198 <= (void*)4)goto _LL128;if(*((int*)_tmp198)!= 8)goto _LL128;_tmp199=((
struct Cyc_Absyn_FnType_struct*)_tmp198)->f1;_tmp19A=_tmp199.tvars;_tmp19B=
_tmp199.effect;_tmp19C=(void*)_tmp199.ret_typ;_tmp19D=_tmp199.args;_tmp19E=
_tmp199.c_varargs;_tmp19F=_tmp199.cyc_varargs;_tmp1A0=_tmp199.rgn_po;_tmp1A1=
_tmp199.attributes;_LL127: if(topt != 0)Cyc_Tcutil_unify(_tmp19C,*topt);while(
_tmp18C != 0  && _tmp19D != 0){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp18C->hd;
void*t2=(*((struct _tuple2*)_tmp19D->hd)).f3;Cyc_Tcexp_tcExp(_tmp18E,(void**)& t2,
e1);if(!Cyc_Tcutil_coerce_arg(_tmp18E,e1,t2)){({struct Cyc_String_pa_struct
_tmp1A5;_tmp1A5.tag=0;_tmp1A5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp1A4;_tmp1A4.tag=0;
_tmp1A4.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmp1A2[2]={&
_tmp1A4,& _tmp1A5};Cyc_Tcutil_terr(e1->loc,({const char*_tmp1A3="actual argument has type \n\t%s\n but formal has type \n\t%s.";
_tag_dynforward(_tmp1A3,sizeof(char),_get_zero_arr_size(_tmp1A3,57));}),
_tag_dynforward(_tmp1A2,sizeof(void*),2));}}});Cyc_Tcutil_unify((void*)((struct
Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}
_tmp18C=_tmp18C->tl;_tmp19D=_tmp19D->tl;}{int args_already_checked=0;{struct Cyc_List_List*
a=_tmp1A1;for(0;a != 0;a=a->tl){void*_tmp1A6=(void*)a->hd;void*_tmp1A7;int _tmp1A8;
int _tmp1A9;_LL12B: if(_tmp1A6 <= (void*)17)goto _LL12D;if(*((int*)_tmp1A6)!= 3)goto
_LL12D;_tmp1A7=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp1A6)->f1;_tmp1A8=((
struct Cyc_Absyn_Format_att_struct*)_tmp1A6)->f2;_tmp1A9=((struct Cyc_Absyn_Format_att_struct*)
_tmp1A6)->f3;_LL12C:{struct _handler_cons _tmp1AA;_push_handler(& _tmp1AA);{int
_tmp1AC=0;if(setjmp(_tmp1AA.handler))_tmp1AC=1;if(!_tmp1AC){{struct Cyc_Absyn_Exp*
_tmp1AD=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,
_tmp1A8 - 1);struct Cyc_Core_Opt*fmt_args;if(_tmp1A9 == 0)fmt_args=0;else{fmt_args=({
struct Cyc_Core_Opt*_tmp1AE=_cycalloc(sizeof(*_tmp1AE));_tmp1AE->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp1A9 - 1);_tmp1AE;});}
args_already_checked=1;{void*_tmp1AF=_tmp1A7;_LL130: if((int)_tmp1AF != 0)goto
_LL132;_LL131: Cyc_Tcexp_check_format_args(_tmp18E,_tmp1AD,fmt_args,Cyc_Formatstr_get_format_typs);
goto _LL12F;_LL132: if((int)_tmp1AF != 1)goto _LL12F;_LL133: Cyc_Tcexp_check_format_args(
_tmp18E,_tmp1AD,fmt_args,Cyc_Formatstr_get_scanf_typs);goto _LL12F;_LL12F:;}};
_pop_handler();}else{void*_tmp1AB=(void*)_exn_thrown;void*_tmp1B1=_tmp1AB;_LL135:
if(_tmp1B1 != Cyc_List_Nth)goto _LL137;_LL136:({void*_tmp1B2[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp1B3="bad format arguments";_tag_dynforward(_tmp1B3,sizeof(
char),_get_zero_arr_size(_tmp1B3,21));}),_tag_dynforward(_tmp1B2,sizeof(void*),0));});
goto _LL134;_LL137:;_LL138:(void)_throw(_tmp1B1);_LL134:;}}}goto _LL12A;_LL12D:;
_LL12E: goto _LL12A;_LL12A:;}}if(_tmp19D != 0)({void*_tmp1B4[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp1B5="too few arguments for function";_tag_dynforward(_tmp1B5,
sizeof(char),_get_zero_arr_size(_tmp1B5,31));}),_tag_dynforward(_tmp1B4,sizeof(
void*),0));});else{if((_tmp18C != 0  || _tmp19E) || _tmp19F != 0){if(_tmp19E)for(0;
_tmp18C != 0;_tmp18C=_tmp18C->tl){Cyc_Tcexp_tcExp(_tmp18E,0,(struct Cyc_Absyn_Exp*)
_tmp18C->hd);}else{if(_tmp19F == 0)({void*_tmp1B6[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp1B7="too many arguments for function";_tag_dynforward(_tmp1B7,
sizeof(char),_get_zero_arr_size(_tmp1B7,32));}),_tag_dynforward(_tmp1B6,sizeof(
void*),0));});else{void*_tmp1B9;int _tmp1BA;struct Cyc_Absyn_VarargInfo _tmp1B8=*
_tmp19F;_tmp1B9=(void*)_tmp1B8.type;_tmp1BA=_tmp1B8.inject;{struct Cyc_Absyn_VarargCallInfo*
_tmp1BB=({struct Cyc_Absyn_VarargCallInfo*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));
_tmp1D3->num_varargs=0;_tmp1D3->injectors=0;_tmp1D3->vai=(struct Cyc_Absyn_VarargInfo*)
_tmp19F;_tmp1D3;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)_tmp1BB;
if(!_tmp1BA)for(0;_tmp18C != 0;_tmp18C=_tmp18C->tl){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp18C->hd;_tmp1BB->num_varargs ++;Cyc_Tcexp_tcExp(_tmp18E,(
void**)& _tmp1B9,e1);if(!Cyc_Tcutil_coerce_arg(_tmp18E,e1,_tmp1B9)){({struct Cyc_String_pa_struct
_tmp1BF;_tmp1BF.tag=0;_tmp1BF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{
struct Cyc_String_pa_struct _tmp1BE;_tmp1BE.tag=0;_tmp1BE.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp1B9));{void*
_tmp1BC[2]={& _tmp1BE,& _tmp1BF};Cyc_Tcutil_terr(loc,({const char*_tmp1BD="vararg requires type %s but argument has type %s";
_tag_dynforward(_tmp1BD,sizeof(char),_get_zero_arr_size(_tmp1BD,49));}),
_tag_dynforward(_tmp1BC,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
else{void*_tmp1C0=Cyc_Tcutil_compress(_tmp1B9);struct Cyc_Absyn_TunionInfo _tmp1C1;
void*_tmp1C2;struct Cyc_Absyn_Tuniondecl**_tmp1C3;struct Cyc_Absyn_Tuniondecl*
_tmp1C4;struct Cyc_List_List*_tmp1C5;struct Cyc_Core_Opt*_tmp1C6;_LL13A: if(_tmp1C0
<= (void*)4)goto _LL13C;if(*((int*)_tmp1C0)!= 2)goto _LL13C;_tmp1C1=((struct Cyc_Absyn_TunionType_struct*)
_tmp1C0)->f1;_tmp1C2=(void*)_tmp1C1.tunion_info;if(*((int*)_tmp1C2)!= 1)goto
_LL13C;_tmp1C3=((struct Cyc_Absyn_KnownTunion_struct*)_tmp1C2)->f1;_tmp1C4=*
_tmp1C3;_tmp1C5=_tmp1C1.targs;_tmp1C6=_tmp1C1.rgn;_LL13B: {struct Cyc_Absyn_Tuniondecl*
_tmp1C7=*Cyc_Tcenv_lookup_tuniondecl(_tmp18E,loc,_tmp1C4->name);struct Cyc_List_List*
fields=0;if(_tmp1C7->fields == 0)({struct Cyc_String_pa_struct _tmp1CA;_tmp1CA.tag=
0;_tmp1CA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
_tmp1B9));{void*_tmp1C8[1]={& _tmp1CA};Cyc_Tcutil_terr(loc,({const char*_tmp1C9="can't inject into %s";
_tag_dynforward(_tmp1C9,sizeof(char),_get_zero_arr_size(_tmp1C9,21));}),
_tag_dynforward(_tmp1C8,sizeof(void*),1));}});else{fields=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1C7->fields))->v;}if(!Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1C6))->v,Cyc_Tcenv_curr_rgn(_tmp18E)))({void*
_tmp1CB[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1CC="bad region for injected varargs";
_tag_dynforward(_tmp1CC,sizeof(char),_get_zero_arr_size(_tmp1CC,32));}),
_tag_dynforward(_tmp1CB,sizeof(void*),0));});{struct _RegionHandle _tmp1CD=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp1CD;_push_region(rgn);{struct Cyc_List_List*
_tmp1CE=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp1C7->tvs,
_tmp1C5);for(0;_tmp18C != 0;_tmp18C=_tmp18C->tl){_tmp1BB->num_varargs ++;{struct
Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp18C->hd;if(!args_already_checked)Cyc_Tcexp_tcExp(
_tmp18E,0,e1);{struct Cyc_Absyn_Tunionfield*_tmp1CF=Cyc_Tcexp_tcInjection(_tmp18E,
e1,_tmp1B9,rgn,_tmp1CE,fields);if(_tmp1CF != 0)_tmp1BB->injectors=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1BB->injectors,({
struct Cyc_List_List*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->hd=(struct Cyc_Absyn_Tunionfield*)
_tmp1CF;_tmp1D0->tl=0;_tmp1D0;}));}}}};_pop_region(rgn);}goto _LL139;}_LL13C:;
_LL13D:({void*_tmp1D1[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1D2="bad inject vararg type";
_tag_dynforward(_tmp1D2,sizeof(char),_get_zero_arr_size(_tmp1D2,23));}),
_tag_dynforward(_tmp1D1,sizeof(void*),0));});goto _LL139;_LL139:;}}}}}}Cyc_Tcenv_check_effect_accessible(
_tmp18E,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp19B))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp18E,loc,_tmp1A0);{void*_tmp1D4=_tmp19C;_npop_handler(0);return _tmp1D4;}}
_LL128:;_LL129: {void*_tmp1D7=({void*_tmp1D5[0]={};Cyc_Tcexp_expr_err(_tmp18E,
loc,topt,({const char*_tmp1D6="expected pointer to function";_tag_dynforward(
_tmp1D6,sizeof(char),_get_zero_arr_size(_tmp1D6,29));}),_tag_dynforward(_tmp1D5,
sizeof(void*),0));});_npop_handler(0);return _tmp1D7;}_LL125:;}_LL123:;_LL124: {
void*_tmp1DA=({void*_tmp1D8[0]={};Cyc_Tcexp_expr_err(_tmp18E,loc,topt,({const
char*_tmp1D9="expected pointer to function";_tag_dynforward(_tmp1D9,sizeof(char),
_get_zero_arr_size(_tmp1D9,29));}),_tag_dynforward(_tmp1D8,sizeof(void*),0));});
_npop_handler(0);return _tmp1DA;}_LL120:;}};_pop_region(ter);}static void*Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e){Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_exn_typ,e);if(!Cyc_Tcutil_coerce_arg(te,
e,Cyc_Absyn_exn_typ))({struct Cyc_String_pa_struct _tmp1DD;_tmp1DD.tag=0;_tmp1DD.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp1DB[1]={& _tmp1DD};Cyc_Tcutil_terr(
loc,({const char*_tmp1DC="expected xtunion exn but found %s";_tag_dynforward(
_tmp1DC,sizeof(char),_get_zero_arr_size(_tmp1DC,34));}),_tag_dynforward(_tmp1DB,
sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp1DE=
_cycalloc(sizeof(*_tmp1DE));_tmp1DE->v=Cyc_Tcenv_lookup_type_vars(te);_tmp1DE;}));}
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(
te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0));{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1DF=t1;struct Cyc_Absyn_PtrInfo
_tmp1E0;void*_tmp1E1;struct Cyc_Absyn_Tqual _tmp1E2;struct Cyc_Absyn_PtrAtts _tmp1E3;
void*_tmp1E4;struct Cyc_Absyn_Conref*_tmp1E5;struct Cyc_Absyn_Conref*_tmp1E6;
struct Cyc_Absyn_Conref*_tmp1E7;_LL13F: if(_tmp1DF <= (void*)4)goto _LL141;if(*((int*)
_tmp1DF)!= 4)goto _LL141;_tmp1E0=((struct Cyc_Absyn_PointerType_struct*)_tmp1DF)->f1;
_tmp1E1=(void*)_tmp1E0.elt_typ;_tmp1E2=_tmp1E0.elt_tq;_tmp1E3=_tmp1E0.ptr_atts;
_tmp1E4=(void*)_tmp1E3.rgn;_tmp1E5=_tmp1E3.nullable;_tmp1E6=_tmp1E3.bounds;
_tmp1E7=_tmp1E3.zero_term;_LL140:{void*_tmp1E8=Cyc_Tcutil_compress(_tmp1E1);
struct Cyc_Absyn_FnInfo _tmp1E9;struct Cyc_List_List*_tmp1EA;struct Cyc_Core_Opt*
_tmp1EB;void*_tmp1EC;struct Cyc_List_List*_tmp1ED;int _tmp1EE;struct Cyc_Absyn_VarargInfo*
_tmp1EF;struct Cyc_List_List*_tmp1F0;struct Cyc_List_List*_tmp1F1;_LL144: if(
_tmp1E8 <= (void*)4)goto _LL146;if(*((int*)_tmp1E8)!= 8)goto _LL146;_tmp1E9=((
struct Cyc_Absyn_FnType_struct*)_tmp1E8)->f1;_tmp1EA=_tmp1E9.tvars;_tmp1EB=
_tmp1E9.effect;_tmp1EC=(void*)_tmp1E9.ret_typ;_tmp1ED=_tmp1E9.args;_tmp1EE=
_tmp1E9.c_varargs;_tmp1EF=_tmp1E9.cyc_varargs;_tmp1F0=_tmp1E9.rgn_po;_tmp1F1=
_tmp1E9.attributes;_LL145: {struct Cyc_List_List*instantiation=0;for(0;ts != 0  && 
_tmp1EA != 0;(ts=ts->tl,_tmp1EA=_tmp1EA->tl)){void*k=Cyc_Tcutil_tvar_kind((struct
Cyc_Absyn_Tvar*)_tmp1EA->hd);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),k,1,(void*)ts->hd);instantiation=({struct Cyc_List_List*_tmp1F2=_cycalloc(
sizeof(*_tmp1F2));_tmp1F2->hd=({struct _tuple5*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));
_tmp1F3->f1=(struct Cyc_Absyn_Tvar*)_tmp1EA->hd;_tmp1F3->f2=(void*)ts->hd;_tmp1F3;});
_tmp1F2->tl=instantiation;_tmp1F2;});}if(ts != 0)return({void*_tmp1F4[0]={};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp1F5="too many type variables in instantiation";
_tag_dynforward(_tmp1F5,sizeof(char),_get_zero_arr_size(_tmp1F5,41));}),
_tag_dynforward(_tmp1F4,sizeof(void*),0));});{void*new_fn_typ=Cyc_Tcutil_substitute(
instantiation,(void*)({struct Cyc_Absyn_FnType_struct*_tmp1FA=_cycalloc(sizeof(*
_tmp1FA));_tmp1FA[0]=({struct Cyc_Absyn_FnType_struct _tmp1FB;_tmp1FB.tag=8;
_tmp1FB.f1=({struct Cyc_Absyn_FnInfo _tmp1FC;_tmp1FC.tvars=_tmp1EA;_tmp1FC.effect=
_tmp1EB;_tmp1FC.ret_typ=(void*)_tmp1EC;_tmp1FC.args=_tmp1ED;_tmp1FC.c_varargs=
_tmp1EE;_tmp1FC.cyc_varargs=_tmp1EF;_tmp1FC.rgn_po=_tmp1F0;_tmp1FC.attributes=
_tmp1F1;_tmp1FC;});_tmp1FB;});_tmp1FA;}));return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp1F6=_cycalloc(sizeof(*_tmp1F6));_tmp1F6[0]=({struct Cyc_Absyn_PointerType_struct
_tmp1F7;_tmp1F7.tag=4;_tmp1F7.f1=({struct Cyc_Absyn_PtrInfo _tmp1F8;_tmp1F8.elt_typ=(
void*)new_fn_typ;_tmp1F8.elt_tq=_tmp1E2;_tmp1F8.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp1F9;_tmp1F9.rgn=(void*)_tmp1E4;_tmp1F9.nullable=_tmp1E5;_tmp1F9.bounds=
_tmp1E6;_tmp1F9.zero_term=_tmp1E7;_tmp1F9.ptrloc=0;_tmp1F9;});_tmp1F8;});_tmp1F7;});
_tmp1F6;});}}_LL146:;_LL147: goto _LL143;_LL143:;}goto _LL13E;_LL141:;_LL142: goto
_LL13E;_LL13E:;}return({struct Cyc_String_pa_struct _tmp1FF;_tmp1FF.tag=0;_tmp1FF.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp1FD[1]={& _tmp1FF};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp1FE="expecting polymorphic type but found %s";
_tag_dynforward(_tmp1FE,sizeof(char),_get_zero_arr_size(_tmp1FE,40));}),
_tag_dynforward(_tmp1FD,sizeof(void*),1));}});}}static void*Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct
Cyc_Absyn_Exp*e,void**c){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(te,loc,t2,t))*((void**)
_check_null(c))=(void*)1;else{void*crc=Cyc_Tcutil_castable(te,loc,t2,t);if(crc != (
void*)0)*((void**)_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((
void**)_check_null(c))=(void*)1;else{Cyc_Tcutil_unify(t2,t);{void*_tmp200=({
struct Cyc_String_pa_struct _tmp204;_tmp204.tag=0;_tmp204.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp203;_tmp203.tag=0;_tmp203.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{void*_tmp201[2]={& _tmp203,& _tmp204};Cyc_Tcexp_expr_err(
te,loc,(void**)& t,({const char*_tmp202="cannot cast %s to %s";_tag_dynforward(
_tmp202,sizeof(char),_get_zero_arr_size(_tmp202,21));}),_tag_dynforward(_tmp201,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();return _tmp200;}}}}{struct
_tuple0 _tmp206=({struct _tuple0 _tmp205;_tmp205.f1=(void*)e->r;_tmp205.f2=Cyc_Tcutil_compress(
t);_tmp205;});void*_tmp207;struct Cyc_Absyn_MallocInfo _tmp208;int _tmp209;void*
_tmp20A;struct Cyc_Absyn_PtrInfo _tmp20B;struct Cyc_Absyn_PtrAtts _tmp20C;struct Cyc_Absyn_Conref*
_tmp20D;struct Cyc_Absyn_Conref*_tmp20E;struct Cyc_Absyn_Conref*_tmp20F;_LL149:
_tmp207=_tmp206.f1;if(*((int*)_tmp207)!= 35)goto _LL14B;_tmp208=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp207)->f1;_tmp209=_tmp208.fat_result;_tmp20A=_tmp206.f2;if(_tmp20A <= (void*)4)
goto _LL14B;if(*((int*)_tmp20A)!= 4)goto _LL14B;_tmp20B=((struct Cyc_Absyn_PointerType_struct*)
_tmp20A)->f1;_tmp20C=_tmp20B.ptr_atts;_tmp20D=_tmp20C.nullable;_tmp20E=_tmp20C.bounds;
_tmp20F=_tmp20C.zero_term;_LL14A: if((_tmp209  && !((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp20F)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp20D)){void*_tmp210=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp20E);struct Cyc_Absyn_Exp*_tmp211;_LL14E: if(_tmp210 <= (void*)2)goto _LL150;if(*((
int*)_tmp210)!= 0)goto _LL150;_tmp211=((struct Cyc_Absyn_Upper_b_struct*)_tmp210)->f1;
_LL14F: if((Cyc_Evexp_eval_const_uint_exp(_tmp211)).f1 == 1)({void*_tmp212[0]={};
Cyc_Tcutil_warn(loc,({const char*_tmp213="cast from ? pointer to * pointer will lose size information";
_tag_dynforward(_tmp213,sizeof(char),_get_zero_arr_size(_tmp213,60));}),
_tag_dynforward(_tmp212,sizeof(void*),0));});goto _LL14D;_LL150:;_LL151: goto
_LL14D;_LL14D:;}goto _LL148;_LL14B:;_LL14C: goto _LL148;_LL148:;}return t;}}static
void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){void**_tmp214=0;struct
Cyc_Absyn_Tqual _tmp215=Cyc_Absyn_empty_tqual(0);if(topt != 0){void*_tmp216=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp217;void*_tmp218;struct Cyc_Absyn_Tqual _tmp219;
struct Cyc_Absyn_PtrAtts _tmp21A;struct Cyc_Absyn_Conref*_tmp21B;_LL153: if(_tmp216
<= (void*)4)goto _LL155;if(*((int*)_tmp216)!= 4)goto _LL155;_tmp217=((struct Cyc_Absyn_PointerType_struct*)
_tmp216)->f1;_tmp218=(void*)_tmp217.elt_typ;_tmp219=_tmp217.elt_tq;_tmp21A=
_tmp217.ptr_atts;_tmp21B=_tmp21A.zero_term;_LL154: _tmp214=({void**_tmp21C=
_cycalloc(sizeof(*_tmp21C));_tmp21C[0]=_tmp218;_tmp21C;});_tmp215=_tmp219;goto
_LL152;_LL155:;_LL156: goto _LL152;_LL152:;}{struct _RegionHandle _tmp21D=
_new_region("r");struct _RegionHandle*r=& _tmp21D;_push_region(r);Cyc_Tcexp_tcExpNoInst(
Cyc_Tcenv_clear_notreadctxt(r,te),_tmp214,e);;_pop_region(r);}{void*_tmp21E=(
void*)e->r;struct Cyc_Absyn_Exp*_tmp21F;struct Cyc_Absyn_Exp*_tmp220;_LL158: if(*((
int*)_tmp21E)!= 25)goto _LL15A;_tmp21F=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp21E)->f1;_tmp220=((struct Cyc_Absyn_Subscript_e_struct*)_tmp21E)->f2;_LL159:{
void*_tmp221=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp21F->topt))->v);_LL15D: if(_tmp221 <= (void*)4)goto _LL15F;if(*((int*)_tmp221)
!= 9)goto _LL15F;_LL15E: goto _LL15C;_LL15F:;_LL160:(void*)(e0->r=(void*)((void*)(
Cyc_Absyn_add_exp(_tmp21F,_tmp220,0))->r));return Cyc_Tcexp_tcPlus(te,_tmp21F,
_tmp220);_LL15C:;}goto _LL157;_LL15A:;_LL15B: goto _LL157;_LL157:;}{int _tmp223;void*
_tmp224;struct _tuple6 _tmp222=Cyc_Tcutil_addressof_props(te,e);_tmp223=_tmp222.f1;
_tmp224=_tmp222.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp223){
tq.print_const=1;tq.real_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp224,tq,Cyc_Absyn_false_conref);return t;}}}}static
void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*t){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,t);if(!Cyc_Evexp_okay_szofarg(t))({struct Cyc_String_pa_struct _tmp227;
_tmp227.tag=0;_tmp227.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp225[1]={& _tmp227};Cyc_Tcutil_terr(loc,({const char*_tmp226="sizeof applied to type %s, which has unknown size here";
_tag_dynforward(_tmp226,sizeof(char),_get_zero_arr_size(_tmp226,55));}),
_tag_dynforward(_tmp225,sizeof(void*),1));}});if(topt != 0){void*_tmp228=Cyc_Tcutil_compress(*
topt);_LL162: if(_tmp228 <= (void*)4)goto _LL164;if(*((int*)_tmp228)!= 14)goto
_LL164;_LL163: return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmp229=_cycalloc(
sizeof(*_tmp229));_tmp229[0]=({struct Cyc_Absyn_SizeofType_struct _tmp22A;_tmp22A.tag=
14;_tmp22A.f1=(void*)t;_tmp22A;});_tmp229;});_LL164:;_LL165: goto _LL161;_LL161:;}
return Cyc_Absyn_uint_typ;}int Cyc_Tcexp_structfield_has_name(struct
_dynforward_ptr*n,struct Cyc_Absyn_Aggrfield*sf){return Cyc_strcmp((struct
_dynforward_ptr)*n,(struct _dynforward_ptr)*sf->name)== 0;}static void*Cyc_Tcexp_tcOffsetof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,void*n){
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);{void*
_tmp22B=n;struct _dynforward_ptr*_tmp22C;unsigned int _tmp22D;_LL167: if(*((int*)
_tmp22B)!= 0)goto _LL169;_tmp22C=((struct Cyc_Absyn_StructField_struct*)_tmp22B)->f1;
_LL168: {int bad_type=1;{void*_tmp22E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp22F;void*_tmp230;struct Cyc_Absyn_Aggrdecl**_tmp231;struct Cyc_List_List*
_tmp232;_LL16C: if(_tmp22E <= (void*)4)goto _LL170;if(*((int*)_tmp22E)!= 10)goto
_LL16E;_tmp22F=((struct Cyc_Absyn_AggrType_struct*)_tmp22E)->f1;_tmp230=(void*)
_tmp22F.aggr_info;if(*((int*)_tmp230)!= 1)goto _LL16E;_tmp231=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp230)->f1;_LL16D: if((*_tmp231)->impl == 0)goto _LL16B;if(!((int(*)(int(*pred)(
struct _dynforward_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dynforward_ptr*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp22C,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp231)->impl))->fields))({struct
Cyc_String_pa_struct _tmp235;_tmp235.tag=0;_tmp235.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*_tmp22C);{void*_tmp233[1]={& _tmp235};Cyc_Tcutil_terr(loc,({
const char*_tmp234="no field of struct/union has name %s";_tag_dynforward(_tmp234,
sizeof(char),_get_zero_arr_size(_tmp234,37));}),_tag_dynforward(_tmp233,sizeof(
void*),1));}});bad_type=0;goto _LL16B;_LL16E: if(*((int*)_tmp22E)!= 11)goto _LL170;
_tmp232=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp22E)->f2;_LL16F: if(!((int(*)(
int(*pred)(struct _dynforward_ptr*,struct Cyc_Absyn_Aggrfield*),struct
_dynforward_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp22C,_tmp232))({struct Cyc_String_pa_struct _tmp238;_tmp238.tag=0;_tmp238.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp22C);{void*_tmp236[1]={&
_tmp238};Cyc_Tcutil_terr(loc,({const char*_tmp237="no field of struct/union has name %s";
_tag_dynforward(_tmp237,sizeof(char),_get_zero_arr_size(_tmp237,37));}),
_tag_dynforward(_tmp236,sizeof(void*),1));}});bad_type=0;goto _LL16B;_LL170:;
_LL171: goto _LL16B;_LL16B:;}if(bad_type)({struct Cyc_String_pa_struct _tmp23B;
_tmp23B.tag=0;_tmp23B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp239[1]={& _tmp23B};Cyc_Tcutil_terr(loc,({const char*_tmp23A="%s is not a known struct/union type";
_tag_dynforward(_tmp23A,sizeof(char),_get_zero_arr_size(_tmp23A,36));}),
_tag_dynforward(_tmp239,sizeof(void*),1));}});goto _LL166;}_LL169: if(*((int*)
_tmp22B)!= 1)goto _LL166;_tmp22D=((struct Cyc_Absyn_TupleIndex_struct*)_tmp22B)->f1;
_LL16A: {int bad_type=1;{void*_tmp23C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp23D;void*_tmp23E;struct Cyc_Absyn_Aggrdecl**_tmp23F;struct Cyc_List_List*
_tmp240;struct Cyc_List_List*_tmp241;struct Cyc_Absyn_TunionFieldInfo _tmp242;void*
_tmp243;struct Cyc_Absyn_Tunionfield*_tmp244;_LL173: if(_tmp23C <= (void*)4)goto
_LL17B;if(*((int*)_tmp23C)!= 10)goto _LL175;_tmp23D=((struct Cyc_Absyn_AggrType_struct*)
_tmp23C)->f1;_tmp23E=(void*)_tmp23D.aggr_info;if(*((int*)_tmp23E)!= 1)goto _LL175;
_tmp23F=((struct Cyc_Absyn_KnownAggr_struct*)_tmp23E)->f1;_LL174: if((*_tmp23F)->impl
== 0)goto _LL172;_tmp240=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp23F)->impl))->fields;
goto _LL176;_LL175: if(*((int*)_tmp23C)!= 11)goto _LL177;_tmp240=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp23C)->f2;_LL176: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp240)
<= _tmp22D)({struct Cyc_Int_pa_struct _tmp248;_tmp248.tag=1;_tmp248.f1=(
unsigned long)((int)_tmp22D);{struct Cyc_Int_pa_struct _tmp247;_tmp247.tag=1;
_tmp247.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp240);{void*_tmp245[2]={& _tmp247,& _tmp248};Cyc_Tcutil_terr(loc,({const char*
_tmp246="struct/union has too few components: %d <= %d";_tag_dynforward(_tmp246,
sizeof(char),_get_zero_arr_size(_tmp246,46));}),_tag_dynforward(_tmp245,sizeof(
void*),2));}}});bad_type=0;goto _LL172;_LL177: if(*((int*)_tmp23C)!= 9)goto _LL179;
_tmp241=((struct Cyc_Absyn_TupleType_struct*)_tmp23C)->f1;_LL178: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp241)<= _tmp22D)({struct Cyc_Int_pa_struct
_tmp24C;_tmp24C.tag=1;_tmp24C.f1=(unsigned long)((int)_tmp22D);{struct Cyc_Int_pa_struct
_tmp24B;_tmp24B.tag=1;_tmp24B.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp241);{void*_tmp249[2]={& _tmp24B,& _tmp24C};Cyc_Tcutil_terr(
loc,({const char*_tmp24A="tuple has too few components: %d <= %d";_tag_dynforward(
_tmp24A,sizeof(char),_get_zero_arr_size(_tmp24A,39));}),_tag_dynforward(_tmp249,
sizeof(void*),2));}}});bad_type=0;goto _LL172;_LL179: if(*((int*)_tmp23C)!= 3)goto
_LL17B;_tmp242=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp23C)->f1;_tmp243=(
void*)_tmp242.field_info;if(*((int*)_tmp243)!= 1)goto _LL17B;_tmp244=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp243)->f2;_LL17A: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp244->typs)
< _tmp22D)({struct Cyc_Int_pa_struct _tmp250;_tmp250.tag=1;_tmp250.f1=(
unsigned long)((int)_tmp22D);{struct Cyc_Int_pa_struct _tmp24F;_tmp24F.tag=1;
_tmp24F.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp244->typs);{void*_tmp24D[2]={& _tmp24F,& _tmp250};Cyc_Tcutil_terr(loc,({const
char*_tmp24E="tunionfield has too few components: %d < %d";_tag_dynforward(
_tmp24E,sizeof(char),_get_zero_arr_size(_tmp24E,44));}),_tag_dynforward(_tmp24D,
sizeof(void*),2));}}});bad_type=0;goto _LL172;_LL17B:;_LL17C: goto _LL172;_LL172:;}
if(bad_type)({struct Cyc_String_pa_struct _tmp253;_tmp253.tag=0;_tmp253.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp251[
1]={& _tmp253};Cyc_Tcutil_terr(loc,({const char*_tmp252="%s is not a known type";
_tag_dynforward(_tmp252,sizeof(char),_get_zero_arr_size(_tmp252,23));}),
_tag_dynforward(_tmp251,sizeof(void*),1));}});goto _LL166;}_LL166:;}return Cyc_Absyn_uint_typ;}
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){struct _RegionHandle _tmp254=_new_region("r");
struct _RegionHandle*r=& _tmp254;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp255=Cyc_Tcenv_clear_notreadctxt(
r,te_orig);Cyc_Tcexp_tcExp(_tmp255,0,e);{void*t=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp256=t;struct Cyc_Absyn_PtrInfo
_tmp257;void*_tmp258;struct Cyc_Absyn_PtrAtts _tmp259;void*_tmp25A;struct Cyc_Absyn_Conref*
_tmp25B;struct Cyc_Absyn_Conref*_tmp25C;_LL17E: if(_tmp256 <= (void*)4)goto _LL180;
if(*((int*)_tmp256)!= 4)goto _LL180;_tmp257=((struct Cyc_Absyn_PointerType_struct*)
_tmp256)->f1;_tmp258=(void*)_tmp257.elt_typ;_tmp259=_tmp257.ptr_atts;_tmp25A=(
void*)_tmp259.rgn;_tmp25B=_tmp259.bounds;_tmp25C=_tmp259.zero_term;_LL17F: Cyc_Tcenv_check_rgn_accessible(
_tmp255,loc,_tmp25A);Cyc_Tcutil_check_nonzero_bound(loc,_tmp25B);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp258),(void*)1))({void*_tmp25D[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp25E="can't dereference abstract pointer type";_tag_dynforward(
_tmp25E,sizeof(char),_get_zero_arr_size(_tmp25E,40));}),_tag_dynforward(_tmp25D,
sizeof(void*),0));});{void*_tmp25F=_tmp258;_npop_handler(0);return _tmp25F;}
_LL180:;_LL181: {void*_tmp263=({struct Cyc_String_pa_struct _tmp262;_tmp262.tag=0;
_tmp262.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp260[1]={& _tmp262};Cyc_Tcexp_expr_err(_tmp255,loc,topt,({const char*
_tmp261="expecting * or @ type but found %s";_tag_dynforward(_tmp261,sizeof(char),
_get_zero_arr_size(_tmp261,35));}),_tag_dynforward(_tmp260,sizeof(void*),1));}});
_npop_handler(0);return _tmp263;}_LL17D:;}};_pop_region(r);}static void*Cyc_Tcexp_tcAggrMember(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
outer_e,struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*f){Cyc_Tcexp_tcExpNoPromote(
te,0,e);{void*_tmp264=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo _tmp265;void*_tmp266;struct Cyc_Absyn_Aggrdecl**
_tmp267;struct Cyc_Absyn_Aggrdecl*_tmp268;struct Cyc_List_List*_tmp269;void*
_tmp26A;struct Cyc_List_List*_tmp26B;_LL183: if(_tmp264 <= (void*)4)goto _LL18B;if(*((
int*)_tmp264)!= 10)goto _LL185;_tmp265=((struct Cyc_Absyn_AggrType_struct*)_tmp264)->f1;
_tmp266=(void*)_tmp265.aggr_info;if(*((int*)_tmp266)!= 1)goto _LL185;_tmp267=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp266)->f1;_tmp268=*_tmp267;_tmp269=_tmp265.targs;
_LL184: {struct Cyc_Absyn_Aggrfield*_tmp26E=Cyc_Absyn_lookup_decl_field(_tmp268,f);
if(_tmp26E == 0)return({struct Cyc_String_pa_struct _tmp272;_tmp272.tag=0;_tmp272.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*f);{struct Cyc_String_pa_struct
_tmp271;_tmp271.tag=0;_tmp271.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp268->name));{void*_tmp26F[2]={& _tmp271,& _tmp272};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp270="type %s has no %s field";_tag_dynforward(
_tmp270,sizeof(char),_get_zero_arr_size(_tmp270,24));}),_tag_dynforward(_tmp26F,
sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp268->impl))->exist_vars
!= 0)return({void*_tmp273[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp274="must use pattern-matching to access fields of existential types";
_tag_dynforward(_tmp274,sizeof(char),_get_zero_arr_size(_tmp274,64));}),
_tag_dynforward(_tmp273,sizeof(void*),0));});{void*t2;{struct _RegionHandle
_tmp275=_new_region("rgn");struct _RegionHandle*rgn=& _tmp275;_push_region(rgn);{
struct Cyc_List_List*_tmp276=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
rgn,rgn,_tmp268->tvs,_tmp269);t2=Cyc_Tcutil_rsubstitute(rgn,_tmp276,(void*)
_tmp26E->type);if(((void*)_tmp268->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only(t2)){void*_tmp27A=({struct Cyc_String_pa_struct
_tmp279;_tmp279.tag=0;_tmp279.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
f);{void*_tmp277[1]={& _tmp279};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp278="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp278,sizeof(char),_get_zero_arr_size(_tmp278,56));}),
_tag_dynforward(_tmp277,sizeof(void*),1));}});_npop_handler(0);return _tmp27A;}};
_pop_region(rgn);}return t2;}}_LL185: if(*((int*)_tmp264)!= 11)goto _LL187;_tmp26A=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp264)->f1;_tmp26B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp264)->f2;_LL186: {struct Cyc_Absyn_Aggrfield*_tmp27B=Cyc_Absyn_lookup_field(
_tmp26B,f);if(_tmp27B == 0)return({struct Cyc_String_pa_struct _tmp27E;_tmp27E.tag=
0;_tmp27E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp27C[1]={&
_tmp27E};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp27D="type has no %s field";
_tag_dynforward(_tmp27D,sizeof(char),_get_zero_arr_size(_tmp27D,21));}),
_tag_dynforward(_tmp27C,sizeof(void*),1));}});if((_tmp26A == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only((void*)_tmp27B->type))return({struct Cyc_String_pa_struct
_tmp281;_tmp281.tag=0;_tmp281.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
f);{void*_tmp27F[1]={& _tmp281};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp280="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp280,sizeof(char),_get_zero_arr_size(_tmp280,56));}),
_tag_dynforward(_tmp27F,sizeof(void*),1));}});return(void*)_tmp27B->type;}_LL187:
if(*((int*)_tmp264)!= 7)goto _LL189;if(!(Cyc_strcmp((struct _dynforward_ptr)*f,({
const char*_tmp26C="size";_tag_dynforward(_tmp26C,sizeof(char),_get_zero_arr_size(
_tmp26C,5));}))== 0))goto _LL189;_LL188: goto _LL18A;_LL189: if(*((int*)_tmp264)!= 4)
goto _LL18B;if(!(Cyc_strcmp((struct _dynforward_ptr)*f,({const char*_tmp26D="size";
_tag_dynforward(_tmp26D,sizeof(char),_get_zero_arr_size(_tmp26D,5));}))== 0))
goto _LL18B;_LL18A:(void*)(outer_e->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp282=_cycalloc(sizeof(*_tmp282));_tmp282[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp283;_tmp283.tag=3;_tmp283.f1=(void*)((void*)19);_tmp283.f2=({struct Cyc_List_List*
_tmp284=_cycalloc(sizeof(*_tmp284));_tmp284->hd=e;_tmp284->tl=0;_tmp284;});
_tmp283;});_tmp282;})));return Cyc_Absyn_uint_typ;_LL18B:;_LL18C: if(Cyc_strcmp((
struct _dynforward_ptr)*f,({const char*_tmp285="size";_tag_dynforward(_tmp285,
sizeof(char),_get_zero_arr_size(_tmp285,5));}))== 0)return({struct Cyc_String_pa_struct
_tmp288;_tmp288.tag=0;_tmp288.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*_tmp286[1]={& _tmp288};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp287="expecting struct, union, or array, found %s";
_tag_dynforward(_tmp287,sizeof(char),_get_zero_arr_size(_tmp287,44));}),
_tag_dynforward(_tmp286,sizeof(void*),1));}});else{return({struct Cyc_String_pa_struct
_tmp28B;_tmp28B.tag=0;_tmp28B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*_tmp289[1]={& _tmp28B};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp28A="expecting struct or union, found %s";
_tag_dynforward(_tmp28A,sizeof(char),_get_zero_arr_size(_tmp28A,36));}),
_tag_dynforward(_tmp289,sizeof(void*),1));}});}_LL182:;}}static void*Cyc_Tcexp_tcAggrArrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct _dynforward_ptr*f){{struct _RegionHandle _tmp28C=_new_region("r");struct
_RegionHandle*r=& _tmp28C;_push_region(r);Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_notreadctxt(
r,te),0,e);;_pop_region(r);}{void*_tmp28D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp28E;void*_tmp28F;struct Cyc_Absyn_PtrAtts
_tmp290;void*_tmp291;struct Cyc_Absyn_Conref*_tmp292;struct Cyc_Absyn_Conref*
_tmp293;_LL18E: if(_tmp28D <= (void*)4)goto _LL190;if(*((int*)_tmp28D)!= 4)goto
_LL190;_tmp28E=((struct Cyc_Absyn_PointerType_struct*)_tmp28D)->f1;_tmp28F=(void*)
_tmp28E.elt_typ;_tmp290=_tmp28E.ptr_atts;_tmp291=(void*)_tmp290.rgn;_tmp292=
_tmp290.bounds;_tmp293=_tmp290.zero_term;_LL18F: Cyc_Tcutil_check_nonzero_bound(
loc,_tmp292);{void*_tmp294=Cyc_Tcutil_compress(_tmp28F);struct Cyc_Absyn_AggrInfo
_tmp295;void*_tmp296;struct Cyc_Absyn_Aggrdecl**_tmp297;struct Cyc_Absyn_Aggrdecl*
_tmp298;struct Cyc_List_List*_tmp299;void*_tmp29A;struct Cyc_List_List*_tmp29B;
_LL193: if(_tmp294 <= (void*)4)goto _LL197;if(*((int*)_tmp294)!= 10)goto _LL195;
_tmp295=((struct Cyc_Absyn_AggrType_struct*)_tmp294)->f1;_tmp296=(void*)_tmp295.aggr_info;
if(*((int*)_tmp296)!= 1)goto _LL195;_tmp297=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp296)->f1;_tmp298=*_tmp297;_tmp299=_tmp295.targs;_LL194: {struct Cyc_Absyn_Aggrfield*
_tmp29C=Cyc_Absyn_lookup_decl_field(_tmp298,f);if(_tmp29C == 0)return({struct Cyc_String_pa_struct
_tmp2A0;_tmp2A0.tag=0;_tmp2A0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
f);{struct Cyc_String_pa_struct _tmp29F;_tmp29F.tag=0;_tmp29F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp298->name));{
void*_tmp29D[2]={& _tmp29F,& _tmp2A0};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp29E="type %s has no %s field";_tag_dynforward(_tmp29E,sizeof(char),
_get_zero_arr_size(_tmp29E,24));}),_tag_dynforward(_tmp29D,sizeof(void*),2));}}});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp298->impl))->exist_vars != 0)
return({void*_tmp2A1[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2A2="must use pattern-matching to access fields of existential types";
_tag_dynforward(_tmp2A2,sizeof(char),_get_zero_arr_size(_tmp2A2,64));}),
_tag_dynforward(_tmp2A1,sizeof(void*),0));});{void*t3;{struct _RegionHandle
_tmp2A3=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2A3;_push_region(rgn);{
struct Cyc_List_List*_tmp2A4=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
rgn,rgn,_tmp298->tvs,_tmp299);t3=Cyc_Tcutil_rsubstitute(rgn,_tmp2A4,(void*)
_tmp29C->type);};_pop_region(rgn);}if(((void*)_tmp298->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only(t3))return({struct Cyc_String_pa_struct _tmp2A7;
_tmp2A7.tag=0;_tmp2A7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{
void*_tmp2A5[1]={& _tmp2A7};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2A6="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp2A6,sizeof(char),_get_zero_arr_size(_tmp2A6,56));}),
_tag_dynforward(_tmp2A5,sizeof(void*),1));}});return t3;}}_LL195: if(*((int*)
_tmp294)!= 11)goto _LL197;_tmp29A=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp294)->f1;_tmp29B=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp294)->f2;_LL196: {
struct Cyc_Absyn_Aggrfield*_tmp2A8=Cyc_Absyn_lookup_field(_tmp29B,f);if(_tmp2A8 == 
0)return({struct Cyc_String_pa_struct _tmp2AB;_tmp2AB.tag=0;_tmp2AB.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp2A9[1]={& _tmp2AB};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2AA="type has no %s field";_tag_dynforward(_tmp2AA,
sizeof(char),_get_zero_arr_size(_tmp2AA,21));}),_tag_dynforward(_tmp2A9,sizeof(
void*),1));}});if((_tmp29A == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp2A8->type))return({struct Cyc_String_pa_struct _tmp2AE;_tmp2AE.tag=0;
_tmp2AE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp2AC[1]={&
_tmp2AE};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2AD="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp2AD,sizeof(char),_get_zero_arr_size(_tmp2AD,56));}),
_tag_dynforward(_tmp2AC,sizeof(void*),1));}});return(void*)_tmp2A8->type;}_LL197:;
_LL198: goto _LL192;_LL192:;}goto _LL18D;_LL190:;_LL191: goto _LL18D;_LL18D:;}return({
struct Cyc_String_pa_struct _tmp2B1;_tmp2B1.tag=0;_tmp2B1.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp2AF[1]={& _tmp2B1};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2B0="expecting struct or union pointer, found %s";
_tag_dynforward(_tmp2B0,sizeof(char),_get_zero_arr_size(_tmp2B0,44));}),
_tag_dynforward(_tmp2AF,sizeof(void*),1));}});}static void*Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,
struct Cyc_Absyn_Exp*index){unsigned int _tmp2B3;int _tmp2B4;struct _tuple7 _tmp2B2=
Cyc_Evexp_eval_const_uint_exp(index);_tmp2B3=_tmp2B2.f1;_tmp2B4=_tmp2B2.f2;if(!
_tmp2B4)return({void*_tmp2B5[0]={};Cyc_Tcexp_expr_err(te,loc,0,({const char*
_tmp2B6="tuple projection cannot use sizeof or offsetof";_tag_dynforward(_tmp2B6,
sizeof(char),_get_zero_arr_size(_tmp2B6,47));}),_tag_dynforward(_tmp2B5,sizeof(
void*),0));});{struct _handler_cons _tmp2B7;_push_handler(& _tmp2B7);{int _tmp2B9=0;
if(setjmp(_tmp2B7.handler))_tmp2B9=1;if(!_tmp2B9){{void*_tmp2BA=(*((struct
_tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2B3)).f2;
_npop_handler(0);return _tmp2BA;};_pop_handler();}else{void*_tmp2B8=(void*)
_exn_thrown;void*_tmp2BC=_tmp2B8;_LL19A: if(_tmp2BC != Cyc_List_Nth)goto _LL19C;
_LL19B: return({struct Cyc_Int_pa_struct _tmp2C0;_tmp2C0.tag=1;_tmp2C0.f1=(
unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);{struct Cyc_Int_pa_struct
_tmp2BF;_tmp2BF.tag=1;_tmp2BF.f1=(unsigned long)((int)_tmp2B3);{void*_tmp2BD[2]={&
_tmp2BF,& _tmp2C0};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp2BE="index is %d but tuple has only %d fields";
_tag_dynforward(_tmp2BE,sizeof(char),_get_zero_arr_size(_tmp2BE,41));}),
_tag_dynforward(_tmp2BD,sizeof(void*),2));}}});_LL19C:;_LL19D:(void)_throw(
_tmp2BC);_LL199:;}}}}static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*
te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){struct _RegionHandle _tmp2C1=_new_region("r");struct
_RegionHandle*r=& _tmp2C1;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp2C2=Cyc_Tcenv_clear_notreadctxt(
r,te_orig);Cyc_Tcexp_tcExp(_tmp2C2,0,e1);Cyc_Tcexp_tcExp(_tmp2C2,0,e2);{void*t1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*
t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!Cyc_Tcutil_coerce_sint_typ(_tmp2C2,e2)){void*_tmp2C6=({struct Cyc_String_pa_struct
_tmp2C5;_tmp2C5.tag=0;_tmp2C5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{void*_tmp2C3[1]={& _tmp2C5};Cyc_Tcexp_expr_err(
_tmp2C2,e2->loc,topt,({const char*_tmp2C4="expecting int subscript, found %s";
_tag_dynforward(_tmp2C4,sizeof(char),_get_zero_arr_size(_tmp2C4,34));}),
_tag_dynforward(_tmp2C3,sizeof(void*),1));}});_npop_handler(0);return _tmp2C6;}{
void*_tmp2C7=t1;struct Cyc_Absyn_PtrInfo _tmp2C8;void*_tmp2C9;struct Cyc_Absyn_Tqual
_tmp2CA;struct Cyc_Absyn_PtrAtts _tmp2CB;void*_tmp2CC;struct Cyc_Absyn_Conref*
_tmp2CD;struct Cyc_Absyn_Conref*_tmp2CE;struct Cyc_List_List*_tmp2CF;_LL19F: if(
_tmp2C7 <= (void*)4)goto _LL1A3;if(*((int*)_tmp2C7)!= 4)goto _LL1A1;_tmp2C8=((
struct Cyc_Absyn_PointerType_struct*)_tmp2C7)->f1;_tmp2C9=(void*)_tmp2C8.elt_typ;
_tmp2CA=_tmp2C8.elt_tq;_tmp2CB=_tmp2C8.ptr_atts;_tmp2CC=(void*)_tmp2CB.rgn;
_tmp2CD=_tmp2CB.bounds;_tmp2CE=_tmp2CB.zero_term;_LL1A0: if(((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2CE)){int emit_warning=1;{void*
_tmp2D0=(void*)(Cyc_Absyn_compress_conref(_tmp2CD))->v;void*_tmp2D1;struct Cyc_Absyn_Exp*
_tmp2D2;void*_tmp2D3;void*_tmp2D4;_LL1A6: if(_tmp2D0 <= (void*)1)goto _LL1AC;if(*((
int*)_tmp2D0)!= 0)goto _LL1A8;_tmp2D1=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2D0)->f1;if(_tmp2D1 <= (void*)2)goto _LL1A8;if(*((int*)_tmp2D1)!= 0)goto _LL1A8;
_tmp2D2=((struct Cyc_Absyn_Upper_b_struct*)_tmp2D1)->f1;_LL1A7: if(Cyc_Tcutil_is_const_exp(
_tmp2C2,e2)){unsigned int _tmp2D6;int _tmp2D7;struct _tuple7 _tmp2D5=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp2D6=_tmp2D5.f1;_tmp2D7=_tmp2D5.f2;if(_tmp2D7){unsigned int _tmp2D9;int
_tmp2DA;struct _tuple7 _tmp2D8=Cyc_Evexp_eval_const_uint_exp(_tmp2D2);_tmp2D9=
_tmp2D8.f1;_tmp2DA=_tmp2D8.f2;if(_tmp2DA  && _tmp2D9 > _tmp2D6)emit_warning=0;}}
goto _LL1A5;_LL1A8: if(*((int*)_tmp2D0)!= 0)goto _LL1AA;_tmp2D3=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2D0)->f1;if((int)_tmp2D3 != 0)goto _LL1AA;_LL1A9: emit_warning=0;goto _LL1A5;
_LL1AA: if(*((int*)_tmp2D0)!= 0)goto _LL1AC;_tmp2D4=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2D0)->f1;if((int)_tmp2D4 != 1)goto _LL1AC;_LL1AB: emit_warning=0;goto _LL1A5;
_LL1AC:;_LL1AD: goto _LL1A5;_LL1A5:;}if(emit_warning)({void*_tmp2DB[0]={};Cyc_Tcutil_warn(
e2->loc,({const char*_tmp2DC="subscript on thin, zero-terminated pointer could be expensive.";
_tag_dynforward(_tmp2DC,sizeof(char),_get_zero_arr_size(_tmp2DC,63));}),
_tag_dynforward(_tmp2DB,sizeof(void*),0));});}else{if(Cyc_Tcutil_is_const_exp(
_tmp2C2,e2)){unsigned int _tmp2DE;int _tmp2DF;struct _tuple7 _tmp2DD=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp2DE=_tmp2DD.f1;_tmp2DF=_tmp2DD.f2;if(_tmp2DF)Cyc_Tcutil_check_bound(loc,
_tmp2DE,_tmp2CD);}else{if(Cyc_Tcutil_is_bound_one(_tmp2CD) && !((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2CE))({void*_tmp2E0[0]={};
Cyc_Tcutil_warn(e1->loc,({const char*_tmp2E1="subscript applied to pointer to one object";
_tag_dynforward(_tmp2E1,sizeof(char),_get_zero_arr_size(_tmp2E1,43));}),
_tag_dynforward(_tmp2E0,sizeof(void*),0));});Cyc_Tcutil_check_nonzero_bound(loc,
_tmp2CD);}}Cyc_Tcenv_check_rgn_accessible(_tmp2C2,loc,_tmp2CC);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp2C9),(void*)1))({void*_tmp2E2[0]={};Cyc_Tcutil_terr(e1->loc,({
const char*_tmp2E3="can't subscript an abstract pointer";_tag_dynforward(_tmp2E3,
sizeof(char),_get_zero_arr_size(_tmp2E3,36));}),_tag_dynforward(_tmp2E2,sizeof(
void*),0));});{void*_tmp2E4=_tmp2C9;_npop_handler(0);return _tmp2E4;}_LL1A1: if(*((
int*)_tmp2C7)!= 9)goto _LL1A3;_tmp2CF=((struct Cyc_Absyn_TupleType_struct*)_tmp2C7)->f1;
_LL1A2: {void*_tmp2E5=Cyc_Tcexp_ithTupleType(_tmp2C2,loc,_tmp2CF,e2);
_npop_handler(0);return _tmp2E5;}_LL1A3:;_LL1A4: {void*_tmp2E9=({struct Cyc_String_pa_struct
_tmp2E8;_tmp2E8.tag=0;_tmp2E8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp2E6[1]={& _tmp2E8};Cyc_Tcexp_expr_err(
_tmp2C2,loc,topt,({const char*_tmp2E7="subscript applied to %s";_tag_dynforward(
_tmp2E7,sizeof(char),_get_zero_arr_size(_tmp2E7,24));}),_tag_dynforward(_tmp2E6,
sizeof(void*),1));}});_npop_handler(0);return _tmp2E9;}_LL19E:;}}};_pop_region(r);}
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp2EA=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp2EB;
_LL1AF: if(_tmp2EA <= (void*)4)goto _LL1B1;if(*((int*)_tmp2EA)!= 9)goto _LL1B1;
_tmp2EB=((struct Cyc_Absyn_TupleType_struct*)_tmp2EA)->f1;_LL1B0: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp2EB)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1AE;for(0;es != 0;(es=es->tl,_tmp2EB=_tmp2EB->tl)){
void*_tmp2EC=(*((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp2EB))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp2EC,(struct Cyc_Absyn_Exp*)es->hd);Cyc_Tcutil_coerce_arg(
te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple9*)_tmp2EB->hd)).f2);fields=({
struct Cyc_List_List*_tmp2ED=_cycalloc(sizeof(*_tmp2ED));_tmp2ED->hd=({struct
_tuple9*_tmp2EE=_cycalloc(sizeof(*_tmp2EE));_tmp2EE->f1=(*((struct _tuple9*)
_tmp2EB->hd)).f1;_tmp2EE->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp2EE;});_tmp2ED->tl=fields;_tmp2ED;});}done=
1;goto _LL1AE;_LL1B1:;_LL1B2: goto _LL1AE;_LL1AE:;}if(!done)for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*
_tmp2EF=_cycalloc(sizeof(*_tmp2EF));_tmp2EF->hd=({struct _tuple9*_tmp2F0=
_cycalloc(sizeof(*_tmp2F0));_tmp2F0->f1=Cyc_Absyn_empty_tqual(0);_tmp2F0->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp2F0;});_tmp2EF->tl=fields;_tmp2EF;});}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp2F1=_cycalloc(sizeof(*_tmp2F1));_tmp2F1[0]=({struct Cyc_Absyn_TupleType_struct
_tmp2F2;_tmp2F2.tag=9;_tmp2F2.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(fields);_tmp2F2;});_tmp2F1;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*
t,struct Cyc_List_List*des){return({void*_tmp2F3[0]={};Cyc_Tcexp_expr_err(te,loc,
topt,({const char*_tmp2F4="tcCompoundLit";_tag_dynforward(_tmp2F4,sizeof(char),
_get_zero_arr_size(_tmp2F4,14));}),_tag_dynforward(_tmp2F3,sizeof(void*),0));});}
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**elt_topt,int zero_term,struct Cyc_List_List*des){struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple8*),struct Cyc_List_List*
x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple8*))Cyc_Core_snd,des);void*
res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*
_tmp30F=_cycalloc(sizeof(*_tmp30F));_tmp30F->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp30F;}));int _tmp2F5=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);
struct Cyc_Absyn_Const_e_struct*_tmp2F6=({struct Cyc_Absyn_Const_e_struct*_tmp30B=
_cycalloc(sizeof(*_tmp30B));_tmp30B[0]=({struct Cyc_Absyn_Const_e_struct _tmp30C;
_tmp30C.tag=0;_tmp30C.f1=(void*)((void*)({struct Cyc_Absyn_Int_c_struct*_tmp30D=
_cycalloc(sizeof(*_tmp30D));_tmp30D[0]=({struct Cyc_Absyn_Int_c_struct _tmp30E;
_tmp30E.tag=2;_tmp30E.f1=(void*)((void*)1);_tmp30E.f2=_tmp2F5;_tmp30E;});_tmp30D;}));
_tmp30C;});_tmp30B;});struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)
_tmp2F6,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp2F7=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp2F5 - 1);if(!Cyc_Tcutil_is_zero(
_tmp2F7))({void*_tmp2F8[0]={};Cyc_Tcutil_terr(_tmp2F7->loc,({const char*_tmp2F9="zero-terminated array doesn't end with zero.";
_tag_dynforward(_tmp2F9,sizeof(char),_get_zero_arr_size(_tmp2F9,45));}),
_tag_dynforward(_tmp2F8,sizeof(void*),0));});}sz_exp->topt=({struct Cyc_Core_Opt*
_tmp2FA=_cycalloc(sizeof(*_tmp2FA));_tmp2FA->v=(void*)Cyc_Absyn_uint_typ;_tmp2FA;});{
void*res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(0),(struct Cyc_Absyn_Exp*)
sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);{struct Cyc_List_List*
es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct
Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,es))({struct Cyc_String_pa_struct
_tmp2FD;_tmp2FD.tag=0;_tmp2FD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(res));{void*_tmp2FB[1]={& _tmp2FD};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,({const
char*_tmp2FC="elements of array do not all have the same type (%s)";
_tag_dynforward(_tmp2FC,sizeof(char),_get_zero_arr_size(_tmp2FC,53));}),
_tag_dynforward(_tmp2FB,sizeof(void*),1));}});{int offset=0;for(0;des != 0;(offset
++,des=des->tl)){struct Cyc_List_List*ds=(*((struct _tuple8*)des->hd)).f1;if(ds != 
0){void*_tmp2FE=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp2FF;_LL1B4: if(*((int*)
_tmp2FE)!= 1)goto _LL1B6;_LL1B5:({void*_tmp300[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp301="only array index designators are supported";_tag_dynforward(
_tmp301,sizeof(char),_get_zero_arr_size(_tmp301,43));}),_tag_dynforward(_tmp300,
sizeof(void*),0));});goto _LL1B3;_LL1B6: if(*((int*)_tmp2FE)!= 0)goto _LL1B3;
_tmp2FF=((struct Cyc_Absyn_ArrayElement_struct*)_tmp2FE)->f1;_LL1B7: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp2FF);{unsigned int _tmp303;int _tmp304;struct _tuple7 _tmp302=Cyc_Evexp_eval_const_uint_exp(
_tmp2FF);_tmp303=_tmp302.f1;_tmp304=_tmp302.f2;if(!_tmp304)({void*_tmp305[0]={};
Cyc_Tcutil_terr(_tmp2FF->loc,({const char*_tmp306="index designator cannot use sizeof or offsetof";
_tag_dynforward(_tmp306,sizeof(char),_get_zero_arr_size(_tmp306,47));}),
_tag_dynforward(_tmp305,sizeof(void*),0));});else{if(_tmp303 != offset)({struct
Cyc_Int_pa_struct _tmp30A;_tmp30A.tag=1;_tmp30A.f1=(unsigned long)((int)_tmp303);{
struct Cyc_Int_pa_struct _tmp309;_tmp309.tag=1;_tmp309.f1=(unsigned long)offset;{
void*_tmp307[2]={& _tmp309,& _tmp30A};Cyc_Tcutil_terr(_tmp2FF->loc,({const char*
_tmp308="expecting index designator %d but found %d";_tag_dynforward(_tmp308,
sizeof(char),_get_zero_arr_size(_tmp308,43));}),_tag_dynforward(_tmp307,sizeof(
void*),2));}}});}goto _LL1B3;}_LL1B3:;}}}return res_t2;}}static void*Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(
te,0,bound);{void*_tmp310=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);_LL1B9: if(_tmp310 <= (void*)4)goto _LL1BB;if(*((int*)
_tmp310)!= 17)goto _LL1BB;_LL1BA: goto _LL1B8;_LL1BB:;_LL1BC: if(!Cyc_Tcutil_coerce_uint_typ(
te,bound))({struct Cyc_String_pa_struct _tmp313;_tmp313.tag=0;_tmp313.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(bound->topt))->v));{void*_tmp311[1]={& _tmp313};Cyc_Tcutil_terr(
bound->loc,({const char*_tmp312="expecting unsigned int, found %s";
_tag_dynforward(_tmp312,sizeof(char),_get_zero_arr_size(_tmp312,33));}),
_tag_dynforward(_tmp311,sizeof(void*),1));}});_LL1B8:;}if(!(vd->tq).real_const)({
void*_tmp314[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp315="comprehension index variable is not declared const!";
_tag_dynforward(_tmp315,sizeof(char),_get_zero_arr_size(_tmp315,52));}),
_tag_dynforward(_tmp314,sizeof(void*),0));});{struct _RegionHandle _tmp316=
_new_region("r");struct _RegionHandle*r=& _tmp316;_push_region(r);{struct Cyc_Tcenv_Tenv*
te2=Cyc_Tcenv_copy_tenv(r,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(r,loc,te2);
te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*_tmp317=te2;
struct Cyc_Absyn_PtrInfo pinfo;void**_tmp318=0;struct Cyc_Absyn_Tqual*_tmp319=0;
struct Cyc_Absyn_Conref**_tmp31A=0;if(topt != 0){void*_tmp31B=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp31C;struct Cyc_Absyn_ArrayInfo _tmp31D;void*
_tmp31E;void**_tmp31F;struct Cyc_Absyn_Tqual _tmp320;struct Cyc_Absyn_Tqual*_tmp321;
struct Cyc_Absyn_Exp*_tmp322;struct Cyc_Absyn_Conref*_tmp323;struct Cyc_Absyn_Conref**
_tmp324;_LL1BE: if(_tmp31B <= (void*)4)goto _LL1C2;if(*((int*)_tmp31B)!= 4)goto
_LL1C0;_tmp31C=((struct Cyc_Absyn_PointerType_struct*)_tmp31B)->f1;_LL1BF: pinfo=
_tmp31C;_tmp318=(void**)((void**)((void*)& pinfo.elt_typ));_tmp319=(struct Cyc_Absyn_Tqual*)&
pinfo.elt_tq;_tmp31A=(struct Cyc_Absyn_Conref**)&(pinfo.ptr_atts).zero_term;goto
_LL1BD;_LL1C0: if(*((int*)_tmp31B)!= 7)goto _LL1C2;_tmp31D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp31B)->f1;_tmp31E=(void*)_tmp31D.elt_type;_tmp31F=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp31B)->f1).elt_type;_tmp320=_tmp31D.tq;_tmp321=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp31B)->f1).tq;_tmp322=_tmp31D.num_elts;
_tmp323=_tmp31D.zero_term;_tmp324=(struct Cyc_Absyn_Conref**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp31B)->f1).zero_term;_LL1C1: _tmp318=(void**)_tmp31F;_tmp319=(struct Cyc_Absyn_Tqual*)
_tmp321;_tmp31A=(struct Cyc_Absyn_Conref**)_tmp324;goto _LL1BD;_LL1C2:;_LL1C3: goto
_LL1BD;_LL1BD:;}{void*t=Cyc_Tcexp_tcExp(_tmp317,_tmp318,body);if(_tmp317->le == 0){
if(!Cyc_Tcutil_is_const_exp(_tmp317,bound))({void*_tmp325[0]={};Cyc_Tcutil_terr(
bound->loc,({const char*_tmp326="bound is not constant";_tag_dynforward(_tmp326,
sizeof(char),_get_zero_arr_size(_tmp326,22));}),_tag_dynforward(_tmp325,sizeof(
void*),0));});if(!Cyc_Tcutil_is_const_exp(_tmp317,body))({void*_tmp327[0]={};Cyc_Tcutil_terr(
bound->loc,({const char*_tmp328="body is not constant";_tag_dynforward(_tmp328,
sizeof(char),_get_zero_arr_size(_tmp328,21));}),_tag_dynforward(_tmp327,sizeof(
void*),0));});}if(_tmp31A != 0  && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,*_tmp31A)){struct Cyc_Absyn_Exp*_tmp329=Cyc_Absyn_uint_exp(1,0);_tmp329->topt=({
struct Cyc_Core_Opt*_tmp32A=_cycalloc(sizeof(*_tmp32A));_tmp32A->v=(void*)Cyc_Absyn_uint_typ;
_tmp32A;});bound=Cyc_Absyn_add_exp(bound,_tmp329,0);bound->topt=({struct Cyc_Core_Opt*
_tmp32B=_cycalloc(sizeof(*_tmp32B));_tmp32B->v=(void*)Cyc_Absyn_uint_typ;_tmp32B;});*
is_zero_term=1;}{void*_tmp32C=Cyc_Absyn_array_typ(t,_tmp319 == 0?Cyc_Absyn_empty_tqual(
0):*_tmp319,(struct Cyc_Absyn_Exp*)bound,_tmp31A == 0?Cyc_Absyn_false_conref:*
_tmp31A,0);_npop_handler(0);return _tmp32C;}}}};_pop_region(r);}}struct _tuple10{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**
tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**
ad_opt){struct Cyc_Absyn_Aggrdecl*ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)
_check_null(*ad_opt);else{{struct _handler_cons _tmp32D;_push_handler(& _tmp32D);{
int _tmp32F=0;if(setjmp(_tmp32D.handler))_tmp32F=1;if(!_tmp32F){ad=*Cyc_Tcenv_lookup_aggrdecl(
te,loc,*tn);;_pop_handler();}else{void*_tmp32E=(void*)_exn_thrown;void*_tmp331=
_tmp32E;_LL1C5: if(_tmp331 != Cyc_Dict_Absent)goto _LL1C7;_LL1C6:({struct Cyc_String_pa_struct
_tmp334;_tmp334.tag=0;_tmp334.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(*tn));{void*_tmp332[1]={& _tmp334};Cyc_Tcutil_terr(loc,({
const char*_tmp333="unbound struct name %s";_tag_dynforward(_tmp333,sizeof(char),
_get_zero_arr_size(_tmp333,23));}),_tag_dynforward(_tmp332,sizeof(void*),1));}});
return topt != 0?*topt:(void*)0;_LL1C7:;_LL1C8:(void)_throw(_tmp331);_LL1C4:;}}}*
ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}if((void*)ad->kind == (void*)1)({
void*_tmp335[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp336="expecting struct but found union";
_tag_dynforward(_tmp336,sizeof(char),_get_zero_arr_size(_tmp336,33));}),
_tag_dynforward(_tmp335,sizeof(void*),0));});if(ad->impl == 0){({void*_tmp337[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp338="can't build abstract struct";
_tag_dynforward(_tmp338,sizeof(char),_get_zero_arr_size(_tmp338,28));}),
_tag_dynforward(_tmp337,sizeof(void*),0));});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp339=_cycalloc(sizeof(*_tmp339));_tmp339->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp339;}));}{struct _RegionHandle _tmp33A=_new_region("rgn");struct _RegionHandle*
rgn=& _tmp33A;_push_region(rgn);{struct _tuple4 _tmp33B=({struct _tuple4 _tmp35C;
_tmp35C.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp35C.f2=rgn;_tmp35C;});struct Cyc_List_List*
_tmp33C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct
_tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp33B,ad->tvs);struct Cyc_List_List*_tmp33D=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,
struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp33B,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars);struct Cyc_List_List*_tmp33E=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp33C);struct Cyc_List_List*_tmp33F=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp33D);struct Cyc_List_List*_tmp340=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp33C,_tmp33D);struct Cyc_Absyn_AggrType_struct*_tmp341=({struct Cyc_Absyn_AggrType_struct*
_tmp356=_cycalloc(sizeof(*_tmp356));_tmp356[0]=({struct Cyc_Absyn_AggrType_struct
_tmp357;_tmp357.tag=10;_tmp357.f1=({struct Cyc_Absyn_AggrInfo _tmp358;_tmp358.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp359=_cycalloc(sizeof(*
_tmp359));_tmp359[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp35A;_tmp35A.tag=1;
_tmp35A.f1=({struct Cyc_Absyn_Aggrdecl**_tmp35B=_cycalloc(sizeof(*_tmp35B));
_tmp35B[0]=ad;_tmp35B;});_tmp35A;});_tmp359;}));_tmp358.targs=_tmp33E;_tmp358;});
_tmp357;});_tmp356;});struct Cyc_List_List*_tmp342=*ts;struct Cyc_List_List*
_tmp343=_tmp33F;while(_tmp342 != 0  && _tmp343 != 0){Cyc_Tcutil_check_type(loc,te,
Cyc_Tcenv_lookup_type_vars(te),(void*)0,1,(void*)_tmp342->hd);Cyc_Tcutil_unify((
void*)_tmp342->hd,(void*)_tmp343->hd);_tmp342=_tmp342->tl;_tmp343=_tmp343->tl;}
if(_tmp342 != 0)({void*_tmp344[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp345="too many explicit witness types";
_tag_dynforward(_tmp345,sizeof(char),_get_zero_arr_size(_tmp345,32));}),
_tag_dynforward(_tmp344,sizeof(void*),0));});*ts=_tmp33F;if(topt != 0)Cyc_Tcutil_unify((
void*)_tmp341,*topt);{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);for(0;fields != 0;
fields=fields->tl){struct _tuple10 _tmp347;struct Cyc_Absyn_Aggrfield*_tmp348;
struct Cyc_Absyn_Exp*_tmp349;struct _tuple10*_tmp346=(struct _tuple10*)fields->hd;
_tmp347=*_tmp346;_tmp348=_tmp347.f1;_tmp349=_tmp347.f2;{void*_tmp34A=Cyc_Tcutil_rsubstitute(
rgn,_tmp340,(void*)_tmp348->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp34A,
_tmp349);if(!Cyc_Tcutil_coerce_arg(te,_tmp349,_tmp34A)){({struct Cyc_String_pa_struct
_tmp350;_tmp350.tag=0;_tmp350.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp349->topt))->v));{
struct Cyc_String_pa_struct _tmp34F;_tmp34F.tag=0;_tmp34F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp34A));{struct
Cyc_String_pa_struct _tmp34E;_tmp34E.tag=0;_tmp34E.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(*tn));{struct Cyc_String_pa_struct
_tmp34D;_tmp34D.tag=0;_tmp34D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp348->name);{void*_tmp34B[4]={& _tmp34D,& _tmp34E,& _tmp34F,& _tmp350};Cyc_Tcutil_terr(
_tmp349->loc,({const char*_tmp34C="field %s of struct %s expects type %s != %s";
_tag_dynforward(_tmp34C,sizeof(char),_get_zero_arr_size(_tmp34C,44));}),
_tag_dynforward(_tmp34B,sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();}}}{
struct Cyc_List_List*_tmp351=0;{struct Cyc_List_List*_tmp352=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->rgn_po;for(0;_tmp352 != 0;_tmp352=_tmp352->tl){_tmp351=({
struct Cyc_List_List*_tmp353=_cycalloc(sizeof(*_tmp353));_tmp353->hd=({struct
_tuple0*_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354->f1=Cyc_Tcutil_rsubstitute(
rgn,_tmp340,(*((struct _tuple0*)_tmp352->hd)).f1);_tmp354->f2=Cyc_Tcutil_rsubstitute(
rgn,_tmp340,(*((struct _tuple0*)_tmp352->hd)).f2);_tmp354;});_tmp353->tl=_tmp351;
_tmp353;});}}_tmp351=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp351);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp351);{void*_tmp355=(void*)
_tmp341;_npop_handler(0);return _tmp355;}}}};_pop_region(rgn);}}static void*Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*ts,struct Cyc_List_List*
args){{struct _RegionHandle _tmp35D=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp35D;_push_region(rgn);{void*_tmp35E=Cyc_Tcutil_compress(ts);void*_tmp35F;
struct Cyc_List_List*_tmp360;_LL1CA: if(_tmp35E <= (void*)4)goto _LL1CC;if(*((int*)
_tmp35E)!= 11)goto _LL1CC;_tmp35F=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp35E)->f1;_tmp360=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp35E)->f2;_LL1CB:
if(_tmp35F == (void*)1)({void*_tmp361[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp362="expecting struct but found union";_tag_dynforward(_tmp362,sizeof(char),
_get_zero_arr_size(_tmp362,33));}),_tag_dynforward(_tmp361,sizeof(void*),0));});{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,_tmp360);for(0;
fields != 0;fields=fields->tl){struct _tuple10 _tmp364;struct Cyc_Absyn_Aggrfield*
_tmp365;struct Cyc_Absyn_Exp*_tmp366;struct _tuple10*_tmp363=(struct _tuple10*)
fields->hd;_tmp364=*_tmp363;_tmp365=_tmp364.f1;_tmp366=_tmp364.f2;Cyc_Tcexp_tcExpInitializer(
te,(void**)((void**)((void*)& _tmp365->type)),_tmp366);if(!Cyc_Tcutil_coerce_arg(
te,_tmp366,(void*)_tmp365->type)){({struct Cyc_String_pa_struct _tmp36B;_tmp36B.tag=
0;_tmp36B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp366->topt))->v));{struct Cyc_String_pa_struct
_tmp36A;_tmp36A.tag=0;_tmp36A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)_tmp365->type));{struct Cyc_String_pa_struct _tmp369;
_tmp369.tag=0;_tmp369.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp365->name);{
void*_tmp367[3]={& _tmp369,& _tmp36A,& _tmp36B};Cyc_Tcutil_terr(_tmp366->loc,({
const char*_tmp368="field %s of struct expects type %s != %s";_tag_dynforward(
_tmp368,sizeof(char),_get_zero_arr_size(_tmp368,41));}),_tag_dynforward(_tmp367,
sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}goto _LL1C9;}_LL1CC:;
_LL1CD:({void*_tmp36C[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp36D="tcAnonStruct: wrong type";
_tag_dynforward(_tmp36D,sizeof(char),_get_zero_arr_size(_tmp36D,25));}),
_tag_dynforward(_tmp36C,sizeof(void*),0));});_LL1C9:;};_pop_region(rgn);}return
ts;}static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*
tud,struct Cyc_Absyn_Tunionfield*tuf){struct _RegionHandle _tmp36E=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp36E;_push_region(rgn);{struct _tuple4 _tmp36F=({
struct _tuple4 _tmp391;_tmp391.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp391.f2=rgn;
_tmp391;});struct Cyc_List_List*_tmp370=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp36F,tud->tvs);struct Cyc_List_List*_tmp371=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp370);void*res=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp38C=_cycalloc(sizeof(*_tmp38C));_tmp38C[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp38D;_tmp38D.tag=3;_tmp38D.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp38E;
_tmp38E.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp38F=_cycalloc(sizeof(*_tmp38F));_tmp38F[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp390;_tmp390.tag=1;_tmp390.f1=tud;_tmp390.f2=tuf;_tmp390;});_tmp38F;}));
_tmp38E.targs=_tmp371;_tmp38E;});_tmp38D;});_tmp38C;});if(topt != 0){void*_tmp372=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo _tmp373;struct Cyc_List_List*
_tmp374;struct Cyc_Core_Opt*_tmp375;_LL1CF: if(_tmp372 <= (void*)4)goto _LL1D3;if(*((
int*)_tmp372)!= 3)goto _LL1D1;_LL1D0: Cyc_Tcutil_unify(*topt,res);goto _LL1CE;
_LL1D1: if(*((int*)_tmp372)!= 2)goto _LL1D3;_tmp373=((struct Cyc_Absyn_TunionType_struct*)
_tmp372)->f1;_tmp374=_tmp373.targs;_tmp375=_tmp373.rgn;_LL1D2:{struct Cyc_List_List*
a=_tmp371;for(0;a != 0  && _tmp374 != 0;(a=a->tl,_tmp374=_tmp374->tl)){Cyc_Tcutil_unify((
void*)a->hd,(void*)_tmp374->hd);}}if(tud->is_flat  || tuf->typs == 0  && es == 0){e->topt=({
struct Cyc_Core_Opt*_tmp376=_cycalloc(sizeof(*_tmp376));_tmp376->v=(void*)res;
_tmp376;});res=(void*)({struct Cyc_Absyn_TunionType_struct*_tmp377=_cycalloc(
sizeof(*_tmp377));_tmp377[0]=({struct Cyc_Absyn_TunionType_struct _tmp378;_tmp378.tag=
2;_tmp378.f1=({struct Cyc_Absyn_TunionInfo _tmp379;_tmp379.tunion_info=(void*)((
void*)({struct Cyc_Absyn_KnownTunion_struct*_tmp37A=_cycalloc(sizeof(*_tmp37A));
_tmp37A[0]=({struct Cyc_Absyn_KnownTunion_struct _tmp37B;_tmp37B.tag=1;_tmp37B.f1=({
struct Cyc_Absyn_Tuniondecl**_tmp37C=_cycalloc(sizeof(*_tmp37C));_tmp37C[0]=tud;
_tmp37C;});_tmp37B;});_tmp37A;}));_tmp379.targs=_tmp371;_tmp379.rgn=_tmp375;
_tmp379;});_tmp378;});_tmp377;});Cyc_Tcutil_unchecked_cast(te,e,res,(void*)1);}
goto _LL1CE;_LL1D3:;_LL1D4: goto _LL1CE;_LL1CE:;}{struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
es->hd;void*t=Cyc_Tcutil_rsubstitute(rgn,_tmp370,(*((struct _tuple9*)ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({
struct Cyc_String_pa_struct _tmp381;_tmp381.tag=0;_tmp381.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)(e->topt == 0?(struct _dynforward_ptr)({
const char*_tmp382="?";_tag_dynforward(_tmp382,sizeof(char),_get_zero_arr_size(
_tmp382,2));}): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)));{
struct Cyc_String_pa_struct _tmp380;_tmp380.tag=0;_tmp380.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp37F;_tmp37F.tag=0;_tmp37F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(tuf->name));{void*_tmp37D[3]={& _tmp37F,& _tmp380,& _tmp381};
Cyc_Tcutil_terr(e->loc,({const char*_tmp37E="tunion constructor %s expects argument of type %s but this argument has type %s";
_tag_dynforward(_tmp37E,sizeof(char),_get_zero_arr_size(_tmp37E,80));}),
_tag_dynforward(_tmp37D,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
if(es != 0){void*_tmp386=({struct Cyc_String_pa_struct _tmp385;_tmp385.tag=0;
_tmp385.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp383[1]={& _tmp385};Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)
es->hd)->loc,topt,({const char*_tmp384="too many arguments for tunion constructor %s";
_tag_dynforward(_tmp384,sizeof(char),_get_zero_arr_size(_tmp384,45));}),
_tag_dynforward(_tmp383,sizeof(void*),1));}});_npop_handler(0);return _tmp386;}
if(ts != 0){void*_tmp38A=({struct Cyc_String_pa_struct _tmp389;_tmp389.tag=0;
_tmp389.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp387[1]={& _tmp389};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp388="too few arguments for tunion constructor %s";_tag_dynforward(
_tmp388,sizeof(char),_get_zero_arr_size(_tmp388,44));}),_tag_dynforward(_tmp387,
sizeof(void*),1));}});_npop_handler(0);return _tmp38A;}{void*_tmp38B=res;
_npop_handler(0);return _tmp38B;}}};_pop_region(rgn);}static int Cyc_Tcexp_zeroable_type(
void*t){void*_tmp392=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp393;
struct Cyc_Absyn_PtrAtts _tmp394;struct Cyc_Absyn_Conref*_tmp395;struct Cyc_Absyn_ArrayInfo
_tmp396;void*_tmp397;struct Cyc_List_List*_tmp398;struct Cyc_Absyn_AggrInfo _tmp399;
void*_tmp39A;struct Cyc_List_List*_tmp39B;struct Cyc_List_List*_tmp39C;_LL1D6: if((
int)_tmp392 != 0)goto _LL1D8;_LL1D7: return 1;_LL1D8: if(_tmp392 <= (void*)4)goto
_LL1E4;if(*((int*)_tmp392)!= 0)goto _LL1DA;_LL1D9: goto _LL1DB;_LL1DA: if(*((int*)
_tmp392)!= 1)goto _LL1DC;_LL1DB: goto _LL1DD;_LL1DC: if(*((int*)_tmp392)!= 2)goto
_LL1DE;_LL1DD: goto _LL1DF;_LL1DE: if(*((int*)_tmp392)!= 3)goto _LL1E0;_LL1DF: return
0;_LL1E0: if(*((int*)_tmp392)!= 4)goto _LL1E2;_tmp393=((struct Cyc_Absyn_PointerType_struct*)
_tmp392)->f1;_tmp394=_tmp393.ptr_atts;_tmp395=_tmp394.nullable;_LL1E1: return((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp395);_LL1E2: if(*((
int*)_tmp392)!= 5)goto _LL1E4;_LL1E3: goto _LL1E5;_LL1E4: if((int)_tmp392 != 1)goto
_LL1E6;_LL1E5: goto _LL1E7;_LL1E6: if(_tmp392 <= (void*)4)goto _LL200;if(*((int*)
_tmp392)!= 6)goto _LL1E8;_LL1E7: return 1;_LL1E8: if(*((int*)_tmp392)!= 7)goto _LL1EA;
_tmp396=((struct Cyc_Absyn_ArrayType_struct*)_tmp392)->f1;_tmp397=(void*)_tmp396.elt_type;
_LL1E9: return Cyc_Tcexp_zeroable_type(_tmp397);_LL1EA: if(*((int*)_tmp392)!= 8)
goto _LL1EC;_LL1EB: return 0;_LL1EC: if(*((int*)_tmp392)!= 9)goto _LL1EE;_tmp398=((
struct Cyc_Absyn_TupleType_struct*)_tmp392)->f1;_LL1ED: for(0;(unsigned int)
_tmp398;_tmp398=_tmp398->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple9*)
_tmp398->hd)).f2))return 0;}return 1;_LL1EE: if(*((int*)_tmp392)!= 10)goto _LL1F0;
_tmp399=((struct Cyc_Absyn_AggrType_struct*)_tmp392)->f1;_tmp39A=(void*)_tmp399.aggr_info;
_tmp39B=_tmp399.targs;_LL1EF: {struct Cyc_Absyn_Aggrdecl*_tmp39D=Cyc_Absyn_get_known_aggrdecl(
_tmp39A);if(_tmp39D->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp39D->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp39E=_new_region("r");
struct _RegionHandle*r=& _tmp39E;_push_region(r);{struct Cyc_List_List*_tmp39F=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp39D->tvs,_tmp39B);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp39D->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp39F,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp3A0=0;_npop_handler(0);
return _tmp3A0;}}}{int _tmp3A1=1;_npop_handler(0);return _tmp3A1;}};_pop_region(r);}}
_LL1F0: if(*((int*)_tmp392)!= 12)goto _LL1F2;_LL1F1: return 1;_LL1F2: if(*((int*)
_tmp392)!= 14)goto _LL1F4;_LL1F3: return 1;_LL1F4: if(*((int*)_tmp392)!= 17)goto
_LL1F6;_LL1F5: return 1;_LL1F6: if(*((int*)_tmp392)!= 11)goto _LL1F8;_tmp39C=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp392)->f2;_LL1F7: for(0;_tmp39C != 0;
_tmp39C=_tmp39C->tl){if(!Cyc_Tcexp_zeroable_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp39C->hd)->type))return 0;}return 1;_LL1F8: if(*((int*)_tmp392)!= 13)goto _LL1FA;
_LL1F9: return 1;_LL1FA: if(*((int*)_tmp392)!= 16)goto _LL1FC;_LL1FB: return 0;_LL1FC:
if(*((int*)_tmp392)!= 15)goto _LL1FE;_LL1FD: return 0;_LL1FE: if(*((int*)_tmp392)!= 
18)goto _LL200;_LL1FF: goto _LL201;_LL200: if((int)_tmp392 != 2)goto _LL202;_LL201:
goto _LL203;_LL202: if((int)_tmp392 != 3)goto _LL204;_LL203: goto _LL205;_LL204: if(
_tmp392 <= (void*)4)goto _LL206;if(*((int*)_tmp392)!= 19)goto _LL206;_LL205: goto
_LL207;_LL206: if(_tmp392 <= (void*)4)goto _LL208;if(*((int*)_tmp392)!= 20)goto
_LL208;_LL207: goto _LL209;_LL208: if(_tmp392 <= (void*)4)goto _LL1D5;if(*((int*)
_tmp392)!= 21)goto _LL1D5;_LL209:({struct Cyc_String_pa_struct _tmp3A4;_tmp3A4.tag=
0;_tmp3A4.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp3A2[1]={& _tmp3A4};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp3A3="bad type `%s' in zeroable type";
_tag_dynforward(_tmp3A3,sizeof(char),_get_zero_arr_size(_tmp3A3,31));}),
_tag_dynforward(_tmp3A2,sizeof(void*),1));}});_LL1D5:;}static void Cyc_Tcexp_check_malloc_type(
int allow_zero,struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(
t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;if(topt != 0){void*_tmp3A5=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp3A6;void*_tmp3A7;_LL20B:
if(_tmp3A5 <= (void*)4)goto _LL20D;if(*((int*)_tmp3A5)!= 4)goto _LL20D;_tmp3A6=((
struct Cyc_Absyn_PointerType_struct*)_tmp3A5)->f1;_tmp3A7=(void*)_tmp3A6.elt_typ;
_LL20C: Cyc_Tcutil_unify(_tmp3A7,t);if(Cyc_Tcutil_bits_only(t) || allow_zero  && 
Cyc_Tcexp_zeroable_type(t))return;goto _LL20A;_LL20D:;_LL20E: goto _LL20A;_LL20A:;}({
struct Cyc_String_pa_struct _tmp3AB;_tmp3AB.tag=0;_tmp3AB.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp3AA;_tmp3AA.tag=0;_tmp3AA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)(
allow_zero?(struct _dynforward_ptr)({const char*_tmp3AC="calloc";_tag_dynforward(
_tmp3AC,sizeof(char),_get_zero_arr_size(_tmp3AC,7));}):(struct _dynforward_ptr)({
const char*_tmp3AD="malloc";_tag_dynforward(_tmp3AD,sizeof(char),
_get_zero_arr_size(_tmp3AD,7));})));{void*_tmp3A8[2]={& _tmp3AA,& _tmp3AB};Cyc_Tcutil_terr(
loc,({const char*_tmp3A9="%s cannot be used with type %s\n\t(type needs initialization)";
_tag_dynforward(_tmp3A9,sizeof(char),_get_zero_arr_size(_tmp3A9,60));}),
_tag_dynforward(_tmp3A8,sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)2;
if(ropt != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp3B4;_tmp3B4.tag=15;_tmp3B4.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp3B5;}));_tmp3B4;});_tmp3B3;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)ropt);
void*_tmp3AE=Cyc_Tcutil_compress(handle_type);void*_tmp3AF;_LL210: if(_tmp3AE <= (
void*)4)goto _LL212;if(*((int*)_tmp3AE)!= 15)goto _LL212;_tmp3AF=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp3AE)->f1;_LL211: rgn=_tmp3AF;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL20F;_LL212:;_LL213:({struct Cyc_String_pa_struct _tmp3B2;
_tmp3B2.tag=0;_tmp3B2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
handle_type));{void*_tmp3B0[1]={& _tmp3B2};Cyc_Tcutil_terr(ropt->loc,({const char*
_tmp3B1="expecting region_t type but found %s";_tag_dynforward(_tmp3B1,sizeof(
char),_get_zero_arr_size(_tmp3B1,37));}),_tag_dynforward(_tmp3B0,sizeof(void*),1));}});
goto _LL20F;_LL20F:;}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*
elt_type;struct Cyc_Absyn_Exp*num_elts;int one_elt;if(*is_calloc){if(*t == 0)({void*
_tmp3B6[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp3B7="calloc with empty type";_tag_dynforward(_tmp3B7,sizeof(char),
_get_zero_arr_size(_tmp3B7,23));}),_tag_dynforward(_tmp3B6,sizeof(void*),0));});
elt_type=*((void**)_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;one_elt=0;}else{void*_tmp3B8=(void*)(*e)->r;void*_tmp3B9;void*
_tmp3BA;struct Cyc_List_List*_tmp3BB;struct Cyc_List_List _tmp3BC;struct Cyc_Absyn_Exp*
_tmp3BD;struct Cyc_List_List*_tmp3BE;struct Cyc_List_List _tmp3BF;struct Cyc_Absyn_Exp*
_tmp3C0;struct Cyc_List_List*_tmp3C1;_LL215: if(*((int*)_tmp3B8)!= 18)goto _LL217;
_tmp3B9=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3B8)->f1;_LL216:
elt_type=_tmp3B9;*t=(void**)({void**_tmp3C2=_cycalloc(sizeof(*_tmp3C2));_tmp3C2[
0]=elt_type;_tmp3C2;});num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(te,(void**)&
Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL214;_LL217: if(*((int*)_tmp3B8)!= 3)
goto _LL219;_tmp3BA=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp3B8)->f1;if((
int)_tmp3BA != 1)goto _LL219;_tmp3BB=((struct Cyc_Absyn_Primop_e_struct*)_tmp3B8)->f2;
if(_tmp3BB == 0)goto _LL219;_tmp3BC=*_tmp3BB;_tmp3BD=(struct Cyc_Absyn_Exp*)_tmp3BC.hd;
_tmp3BE=_tmp3BC.tl;if(_tmp3BE == 0)goto _LL219;_tmp3BF=*_tmp3BE;_tmp3C0=(struct Cyc_Absyn_Exp*)
_tmp3BF.hd;_tmp3C1=_tmp3BF.tl;if(_tmp3C1 != 0)goto _LL219;_LL218:{struct _tuple0
_tmp3C4=({struct _tuple0 _tmp3C3;_tmp3C3.f1=(void*)_tmp3BD->r;_tmp3C3.f2=(void*)
_tmp3C0->r;_tmp3C3;});void*_tmp3C5;void*_tmp3C6;void*_tmp3C7;void*_tmp3C8;_LL21C:
_tmp3C5=_tmp3C4.f1;if(*((int*)_tmp3C5)!= 18)goto _LL21E;_tmp3C6=(void*)((struct
Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3C5)->f1;_LL21D: Cyc_Tcexp_check_malloc_type(0,
loc,topt,_tmp3C6);elt_type=_tmp3C6;*t=(void**)({void**_tmp3C9=_cycalloc(sizeof(*
_tmp3C9));_tmp3C9[0]=elt_type;_tmp3C9;});num_elts=_tmp3C0;one_elt=0;goto _LL21B;
_LL21E: _tmp3C7=_tmp3C4.f2;if(*((int*)_tmp3C7)!= 18)goto _LL220;_tmp3C8=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3C7)->f1;_LL21F: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp3C8);elt_type=_tmp3C8;*t=(void**)({void**_tmp3CA=_cycalloc(
sizeof(*_tmp3CA));_tmp3CA[0]=elt_type;_tmp3CA;});num_elts=_tmp3BD;one_elt=0;goto
_LL21B;_LL220:;_LL221: goto No_sizeof;_LL21B:;}goto _LL214;_LL219:;_LL21A: No_sizeof:
elt_type=Cyc_Absyn_char_typ;*t=(void**)({void**_tmp3CB=_cycalloc(sizeof(*_tmp3CB));
_tmp3CB[0]=elt_type;_tmp3CB;});num_elts=*e;one_elt=0;goto _LL214;_LL214:;}*e=
num_elts;*is_fat=!one_elt;{void*_tmp3CC=elt_type;struct Cyc_Absyn_AggrInfo _tmp3CD;
void*_tmp3CE;struct Cyc_Absyn_Aggrdecl**_tmp3CF;struct Cyc_Absyn_Aggrdecl*_tmp3D0;
_LL223: if(_tmp3CC <= (void*)4)goto _LL225;if(*((int*)_tmp3CC)!= 10)goto _LL225;
_tmp3CD=((struct Cyc_Absyn_AggrType_struct*)_tmp3CC)->f1;_tmp3CE=(void*)_tmp3CD.aggr_info;
if(*((int*)_tmp3CE)!= 1)goto _LL225;_tmp3CF=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp3CE)->f1;_tmp3D0=*_tmp3CF;_LL224: if(_tmp3D0->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3D0->impl))->exist_vars != 0)({void*_tmp3D1[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp3D2="malloc with existential types not yet implemented";
_tag_dynforward(_tmp3D2,sizeof(char),_get_zero_arr_size(_tmp3D2,50));}),
_tag_dynforward(_tmp3D1,sizeof(void*),0));});goto _LL222;_LL225:;_LL226: goto
_LL222;_LL222:;}{void*(*_tmp3D3)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Conref*zero_term)=Cyc_Absyn_at_typ;struct Cyc_Absyn_Conref*_tmp3D4=Cyc_Absyn_false_conref;
if(topt != 0){void*_tmp3D5=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp3D6;struct Cyc_Absyn_PtrAtts _tmp3D7;struct Cyc_Absyn_Conref*_tmp3D8;struct Cyc_Absyn_Conref*
_tmp3D9;_LL228: if(_tmp3D5 <= (void*)4)goto _LL22A;if(*((int*)_tmp3D5)!= 4)goto
_LL22A;_tmp3D6=((struct Cyc_Absyn_PointerType_struct*)_tmp3D5)->f1;_tmp3D7=
_tmp3D6.ptr_atts;_tmp3D8=_tmp3D7.nullable;_tmp3D9=_tmp3D7.zero_term;_LL229:
_tmp3D4=_tmp3D9;if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3D8))_tmp3D3=Cyc_Absyn_star_typ;if(((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp3D9) && !(*is_calloc)){({void*_tmp3DA[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp3DB="converting malloc to calloc to ensure zero-termination";
_tag_dynforward(_tmp3DB,sizeof(char),_get_zero_arr_size(_tmp3DB,55));}),
_tag_dynforward(_tmp3DA,sizeof(void*),0));});*is_calloc=1;}goto _LL227;_LL22A:;
_LL22B: goto _LL227;_LL227:;}if(!one_elt)_tmp3D3=Cyc_Absyn_dynforward_typ;return
_tmp3D3(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp3D4);}}}static void*Cyc_Tcexp_tcStmtExp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Stmt*
s){{struct _RegionHandle _tmp3DC=_new_region("r");struct _RegionHandle*r=& _tmp3DC;
_push_region(r);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,te,s),s,1);;
_pop_region(r);}Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
while(1){void*_tmp3DD=(void*)s->r;struct Cyc_Absyn_Exp*_tmp3DE;struct Cyc_Absyn_Stmt*
_tmp3DF;struct Cyc_Absyn_Stmt*_tmp3E0;struct Cyc_Absyn_Decl*_tmp3E1;struct Cyc_Absyn_Stmt*
_tmp3E2;_LL22D: if(_tmp3DD <= (void*)1)goto _LL233;if(*((int*)_tmp3DD)!= 0)goto
_LL22F;_tmp3DE=((struct Cyc_Absyn_Exp_s_struct*)_tmp3DD)->f1;_LL22E: return(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3DE->topt))->v;_LL22F: if(*((int*)_tmp3DD)!= 
1)goto _LL231;_tmp3DF=((struct Cyc_Absyn_Seq_s_struct*)_tmp3DD)->f1;_tmp3E0=((
struct Cyc_Absyn_Seq_s_struct*)_tmp3DD)->f2;_LL230: s=_tmp3E0;continue;_LL231: if(*((
int*)_tmp3DD)!= 11)goto _LL233;_tmp3E1=((struct Cyc_Absyn_Decl_s_struct*)_tmp3DD)->f1;
_tmp3E2=((struct Cyc_Absyn_Decl_s_struct*)_tmp3DD)->f2;_LL232: s=_tmp3E2;continue;
_LL233:;_LL234: return({void*_tmp3E3[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp3E4="statement expression must end with expression";_tag_dynforward(
_tmp3E4,sizeof(char),_get_zero_arr_size(_tmp3E4,46));}),_tag_dynforward(_tmp3E3,
sizeof(void*),0));});_LL22C:;}}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)2;if(rgn_handle != 
0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp3EA=
_cycalloc(sizeof(*_tmp3EA));_tmp3EA[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp3EB;_tmp3EB.tag=15;_tmp3EB.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));_tmp3EC->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp3EC;}));_tmp3EB;});_tmp3EA;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
rgn_handle);void*_tmp3E5=Cyc_Tcutil_compress(handle_type);void*_tmp3E6;_LL236:
if(_tmp3E5 <= (void*)4)goto _LL238;if(*((int*)_tmp3E5)!= 15)goto _LL238;_tmp3E6=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp3E5)->f1;_LL237: rgn=_tmp3E6;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL235;_LL238:;_LL239:({struct Cyc_String_pa_struct
_tmp3E9;_tmp3E9.tag=0;_tmp3E9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(handle_type));{void*_tmp3E7[1]={& _tmp3E9};Cyc_Tcutil_terr(
rgn_handle->loc,({const char*_tmp3E8="expecting region_t type but found %s";
_tag_dynforward(_tmp3E8,sizeof(char),_get_zero_arr_size(_tmp3E8,37));}),
_tag_dynforward(_tmp3E7,sizeof(void*),1));}});goto _LL235;_LL235:;}{void*_tmp3ED=(
void*)e1->r;struct Cyc_Core_Opt*_tmp3EE;struct Cyc_List_List*_tmp3EF;struct Cyc_List_List*
_tmp3F0;void*_tmp3F1;struct _dynforward_ptr _tmp3F2;_LL23B: if(*((int*)_tmp3ED)!= 
29)goto _LL23D;_LL23C: {void*_tmp3F3=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);void*
_tmp3F4=Cyc_Tcutil_compress(_tmp3F3);struct Cyc_Absyn_ArrayInfo _tmp3F5;void*
_tmp3F6;struct Cyc_Absyn_Tqual _tmp3F7;struct Cyc_Absyn_Exp*_tmp3F8;struct Cyc_Absyn_Conref*
_tmp3F9;_LL246: if(_tmp3F4 <= (void*)4)goto _LL248;if(*((int*)_tmp3F4)!= 7)goto
_LL248;_tmp3F5=((struct Cyc_Absyn_ArrayType_struct*)_tmp3F4)->f1;_tmp3F6=(void*)
_tmp3F5.elt_type;_tmp3F7=_tmp3F5.tq;_tmp3F8=_tmp3F5.num_elts;_tmp3F9=_tmp3F5.zero_term;
_LL247: {struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp3F8);void*
b;{void*_tmp3FA=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(bnd->topt))->v);
void*_tmp3FB;_LL24B: if(_tmp3FA <= (void*)4)goto _LL24D;if(*((int*)_tmp3FA)!= 17)
goto _LL24D;_tmp3FB=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp3FA)->f1;_LL24C:
b=(void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp3FC=_cycalloc(sizeof(*_tmp3FC));
_tmp3FC[0]=({struct Cyc_Absyn_AbsUpper_b_struct _tmp3FD;_tmp3FD.tag=1;_tmp3FD.f1=(
void*)_tmp3FB;_tmp3FD;});_tmp3FC;});goto _LL24A;_LL24D:;_LL24E: if(Cyc_Tcutil_is_const_exp(
te,bnd))b=(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp3FE=_cycalloc(sizeof(*
_tmp3FE));_tmp3FE[0]=({struct Cyc_Absyn_Upper_b_struct _tmp3FF;_tmp3FF.tag=0;
_tmp3FF.f1=bnd;_tmp3FF;});_tmp3FE;});else{b=(void*)0;}_LL24A:;}{void*res_typ=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp401=_cycalloc(sizeof(*_tmp401));
_tmp401[0]=({struct Cyc_Absyn_PointerType_struct _tmp402;_tmp402.tag=4;_tmp402.f1=({
struct Cyc_Absyn_PtrInfo _tmp403;_tmp403.elt_typ=(void*)_tmp3F6;_tmp403.elt_tq=
_tmp3F7;_tmp403.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp404;_tmp404.rgn=(void*)rgn;
_tmp404.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp404.bounds=Cyc_Absyn_new_conref(b);_tmp404.zero_term=_tmp3F9;_tmp404.ptrloc=
0;_tmp404;});_tmp403;});_tmp402;});_tmp401;});if(topt != 0){if(!Cyc_Tcutil_unify(*
topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){e->topt=({
struct Cyc_Core_Opt*_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400->v=(void*)res_typ;
_tmp400;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);res_typ=*topt;}}return
res_typ;}}_LL248:;_LL249:({void*_tmp405[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp406="tcNew: comprehension returned non-array type";
_tag_dynforward(_tmp406,sizeof(char),_get_zero_arr_size(_tmp406,45));}),
_tag_dynforward(_tmp405,sizeof(void*),0));});_LL245:;}_LL23D: if(*((int*)_tmp3ED)
!= 36)goto _LL23F;_tmp3EE=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp3ED)->f1;
_tmp3EF=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp3ED)->f2;_LL23E:(void*)(e1->r=(
void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp407=_cycalloc(sizeof(*_tmp407));
_tmp407[0]=({struct Cyc_Absyn_Array_e_struct _tmp408;_tmp408.tag=28;_tmp408.f1=
_tmp3EF;_tmp408;});_tmp407;})));_tmp3F0=_tmp3EF;goto _LL240;_LL23F: if(*((int*)
_tmp3ED)!= 28)goto _LL241;_tmp3F0=((struct Cyc_Absyn_Array_e_struct*)_tmp3ED)->f1;
_LL240: {void**elt_typ_opt=0;int zero_term=0;if(topt != 0){void*_tmp409=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp40A;void*_tmp40B;void**_tmp40C;struct Cyc_Absyn_Tqual
_tmp40D;struct Cyc_Absyn_PtrAtts _tmp40E;struct Cyc_Absyn_Conref*_tmp40F;_LL250: if(
_tmp409 <= (void*)4)goto _LL252;if(*((int*)_tmp409)!= 4)goto _LL252;_tmp40A=((
struct Cyc_Absyn_PointerType_struct*)_tmp409)->f1;_tmp40B=(void*)_tmp40A.elt_typ;
_tmp40C=(void**)&(((struct Cyc_Absyn_PointerType_struct*)_tmp409)->f1).elt_typ;
_tmp40D=_tmp40A.elt_tq;_tmp40E=_tmp40A.ptr_atts;_tmp40F=_tmp40E.zero_term;_LL251:
elt_typ_opt=(void**)_tmp40C;zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp40F);goto _LL24F;_LL252:;_LL253: goto _LL24F;_LL24F:;}{void*_tmp410=Cyc_Tcexp_tcArray(
te,e1->loc,elt_typ_opt,zero_term,_tmp3F0);e1->topt=({struct Cyc_Core_Opt*_tmp411=
_cycalloc(sizeof(*_tmp411));_tmp411->v=(void*)_tmp410;_tmp411;});{void*res_typ;{
void*_tmp412=Cyc_Tcutil_compress(_tmp410);struct Cyc_Absyn_ArrayInfo _tmp413;void*
_tmp414;struct Cyc_Absyn_Tqual _tmp415;struct Cyc_Absyn_Exp*_tmp416;struct Cyc_Absyn_Conref*
_tmp417;_LL255: if(_tmp412 <= (void*)4)goto _LL257;if(*((int*)_tmp412)!= 7)goto
_LL257;_tmp413=((struct Cyc_Absyn_ArrayType_struct*)_tmp412)->f1;_tmp414=(void*)
_tmp413.elt_type;_tmp415=_tmp413.tq;_tmp416=_tmp413.num_elts;_tmp417=_tmp413.zero_term;
_LL256: res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp418=_cycalloc(
sizeof(*_tmp418));_tmp418[0]=({struct Cyc_Absyn_PointerType_struct _tmp419;_tmp419.tag=
4;_tmp419.f1=({struct Cyc_Absyn_PtrInfo _tmp41A;_tmp41A.elt_typ=(void*)_tmp414;
_tmp41A.elt_tq=_tmp415;_tmp41A.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp41B;_tmp41B.rgn=(
void*)rgn;_tmp41B.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp41B.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp41C=_cycalloc(sizeof(*_tmp41C));_tmp41C[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp41D;_tmp41D.tag=0;_tmp41D.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp416);
_tmp41D;});_tmp41C;}));_tmp41B.zero_term=_tmp417;_tmp41B.ptrloc=0;_tmp41B;});
_tmp41A;});_tmp419;});_tmp418;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ)
 && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){e->topt=({struct Cyc_Core_Opt*
_tmp41E=_cycalloc(sizeof(*_tmp41E));_tmp41E->v=(void*)res_typ;_tmp41E;});Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}goto _LL254;_LL257:;_LL258:({void*_tmp41F[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp420="tcExpNoPromote on Array_e returned non-array type";
_tag_dynforward(_tmp420,sizeof(char),_get_zero_arr_size(_tmp420,50));}),
_tag_dynforward(_tmp41F,sizeof(void*),0));});_LL254:;}return res_typ;}}}_LL241:
if(*((int*)_tmp3ED)!= 0)goto _LL243;_tmp3F1=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp3ED)->f1;if(_tmp3F1 <= (void*)1)goto _LL243;if(*((int*)_tmp3F1)!= 5)goto _LL243;
_tmp3F2=((struct Cyc_Absyn_String_c_struct*)_tmp3F1)->f1;_LL242: {void*_tmp421=
Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);
void*_tmp422=Cyc_Tcexp_tcExp(te,(void**)& _tmp421,e1);return Cyc_Absyn_atb_typ(
_tmp422,rgn,Cyc_Absyn_empty_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp424;_tmp424.tag=0;_tmp424.f1=Cyc_Absyn_uint_exp(1,0);_tmp424;});_tmp423;}),
Cyc_Absyn_false_conref);}_LL243:;_LL244: {void**topt2=0;if(topt != 0){void*
_tmp425=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp426;void*_tmp427;
void**_tmp428;struct Cyc_Absyn_Tqual _tmp429;_LL25A: if(_tmp425 <= (void*)4)goto
_LL25E;if(*((int*)_tmp425)!= 4)goto _LL25C;_tmp426=((struct Cyc_Absyn_PointerType_struct*)
_tmp425)->f1;_tmp427=(void*)_tmp426.elt_typ;_tmp428=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp425)->f1).elt_typ;_tmp429=_tmp426.elt_tq;_LL25B: topt2=(void**)_tmp428;goto
_LL259;_LL25C: if(*((int*)_tmp425)!= 2)goto _LL25E;_LL25D: topt2=({void**_tmp42A=
_cycalloc(sizeof(*_tmp42A));_tmp42A[0]=*topt;_tmp42A;});goto _LL259;_LL25E:;
_LL25F: goto _LL259;_LL259:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);void*res_typ=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp42C=_cycalloc(sizeof(*_tmp42C));
_tmp42C[0]=({struct Cyc_Absyn_PointerType_struct _tmp42D;_tmp42D.tag=4;_tmp42D.f1=({
struct Cyc_Absyn_PtrInfo _tmp42E;_tmp42E.elt_typ=(void*)telt;_tmp42E.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmp42E.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp42F;_tmp42F.rgn=(void*)rgn;
_tmp42F.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp42F.bounds=Cyc_Absyn_bounds_one_conref;_tmp42F.zero_term=Cyc_Absyn_false_conref;
_tmp42F.ptrloc=0;_tmp42F;});_tmp42E;});_tmp42D;});_tmp42C;});if(topt != 0){if(!
Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*
topt)){e->topt=({struct Cyc_Core_Opt*_tmp42B=_cycalloc(sizeof(*_tmp42B));_tmp42B->v=(
void*)res_typ;_tmp42B;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);res_typ=*
topt;}}return res_typ;}}_LL23A:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,
void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp430=t;struct Cyc_Absyn_ArrayInfo _tmp431;void*_tmp432;struct
Cyc_Absyn_Tqual _tmp433;struct Cyc_Absyn_Exp*_tmp434;struct Cyc_Absyn_Conref*
_tmp435;_LL261: if(_tmp430 <= (void*)4)goto _LL263;if(*((int*)_tmp430)!= 7)goto
_LL263;_tmp431=((struct Cyc_Absyn_ArrayType_struct*)_tmp430)->f1;_tmp432=(void*)
_tmp431.elt_type;_tmp433=_tmp431.tq;_tmp434=_tmp431.num_elts;_tmp435=_tmp431.zero_term;
_LL262: {void*_tmp437;struct _tuple6 _tmp436=Cyc_Tcutil_addressof_props(te,e);
_tmp437=_tmp436.f2;{void*_tmp438=_tmp434 == 0?(void*)((void*)0):(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp439=_cycalloc(sizeof(*_tmp439));_tmp439[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp43A;_tmp43A.tag=0;_tmp43A.f1=(struct Cyc_Absyn_Exp*)
_tmp434;_tmp43A;});_tmp439;});t=Cyc_Absyn_atb_typ(_tmp432,_tmp437,_tmp433,
_tmp438,_tmp435);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);
return t;}}_LL263:;_LL264: return t;_LL260:;}void*Cyc_Tcexp_tcExpInitializer(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(
te,topt,e);void*_tmp43B=(void*)e->r;void*_tmp43C;_LL266: if(*((int*)_tmp43B)!= 28)
goto _LL268;_LL267: goto _LL269;_LL268: if(*((int*)_tmp43B)!= 29)goto _LL26A;_LL269:
goto _LL26B;_LL26A: if(*((int*)_tmp43B)!= 0)goto _LL26C;_tmp43C=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp43B)->f1;if(_tmp43C <= (void*)1)goto _LL26C;if(*((int*)_tmp43C)!= 5)goto _LL26C;
_LL26B: return t;_LL26C:;_LL26D: t=Cyc_Tcutil_compress(t);{void*_tmp43D=t;struct Cyc_Absyn_ArrayInfo
_tmp43E;void*_tmp43F;struct Cyc_Absyn_Tqual _tmp440;struct Cyc_Absyn_Exp*_tmp441;
struct Cyc_Absyn_Conref*_tmp442;_LL26F: if(_tmp43D <= (void*)4)goto _LL271;if(*((int*)
_tmp43D)!= 7)goto _LL271;_tmp43E=((struct Cyc_Absyn_ArrayType_struct*)_tmp43D)->f1;
_tmp43F=(void*)_tmp43E.elt_type;_tmp440=_tmp43E.tq;_tmp441=_tmp43E.num_elts;
_tmp442=_tmp43E.zero_term;_LL270: {void*_tmp444;struct _tuple6 _tmp443=Cyc_Tcutil_addressof_props(
te,e);_tmp444=_tmp443.f2;{void*b=_tmp441 == 0?(void*)((void*)0):(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp446;_tmp446.tag=0;_tmp446.f1=(struct Cyc_Absyn_Exp*)
_tmp441;_tmp446;});_tmp445;});t=Cyc_Absyn_atb_typ(_tmp43F,_tmp444,_tmp440,b,
_tmp442);Cyc_Tcutil_unchecked_cast(te,e,t,(void*)3);return t;}}_LL271:;_LL272:
return t;_LL26E:;}_LL265:;}static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp447=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp448;_LL274: if(*((int*)_tmp447)!= 13)goto _LL276;_tmp448=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp447)->f1;_LL275: Cyc_Tcexp_tcExpNoInst(te,topt,_tmp448);(void*)(((struct Cyc_Core_Opt*)
_check_null(_tmp448->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp448->topt))->v,0));e->topt=_tmp448->topt;goto
_LL273;_LL276:;_LL277: Cyc_Tcexp_tcExpNoInst(te,topt,e);(void*)(((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0));{void*_tmp449=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp44A;void*
_tmp44B;struct Cyc_Absyn_Tqual _tmp44C;struct Cyc_Absyn_PtrAtts _tmp44D;void*_tmp44E;
struct Cyc_Absyn_Conref*_tmp44F;struct Cyc_Absyn_Conref*_tmp450;struct Cyc_Absyn_Conref*
_tmp451;_LL279: if(_tmp449 <= (void*)4)goto _LL27B;if(*((int*)_tmp449)!= 4)goto
_LL27B;_tmp44A=((struct Cyc_Absyn_PointerType_struct*)_tmp449)->f1;_tmp44B=(void*)
_tmp44A.elt_typ;_tmp44C=_tmp44A.elt_tq;_tmp44D=_tmp44A.ptr_atts;_tmp44E=(void*)
_tmp44D.rgn;_tmp44F=_tmp44D.nullable;_tmp450=_tmp44D.bounds;_tmp451=_tmp44D.zero_term;
_LL27A:{void*_tmp452=Cyc_Tcutil_compress(_tmp44B);struct Cyc_Absyn_FnInfo _tmp453;
struct Cyc_List_List*_tmp454;struct Cyc_Core_Opt*_tmp455;void*_tmp456;struct Cyc_List_List*
_tmp457;int _tmp458;struct Cyc_Absyn_VarargInfo*_tmp459;struct Cyc_List_List*
_tmp45A;struct Cyc_List_List*_tmp45B;_LL27E: if(_tmp452 <= (void*)4)goto _LL280;if(*((
int*)_tmp452)!= 8)goto _LL280;_tmp453=((struct Cyc_Absyn_FnType_struct*)_tmp452)->f1;
_tmp454=_tmp453.tvars;_tmp455=_tmp453.effect;_tmp456=(void*)_tmp453.ret_typ;
_tmp457=_tmp453.args;_tmp458=_tmp453.c_varargs;_tmp459=_tmp453.cyc_varargs;
_tmp45A=_tmp453.rgn_po;_tmp45B=_tmp453.attributes;_LL27F: if(_tmp454 != 0){struct
_RegionHandle _tmp45C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp45C;
_push_region(rgn);{struct _tuple4 _tmp45D=({struct _tuple4 _tmp46A;_tmp46A.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp46A.f2=rgn;_tmp46A;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp45D,_tmp454);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467[0]=({struct Cyc_Absyn_FnType_struct
_tmp468;_tmp468.tag=8;_tmp468.f1=({struct Cyc_Absyn_FnInfo _tmp469;_tmp469.tvars=0;
_tmp469.effect=_tmp455;_tmp469.ret_typ=(void*)_tmp456;_tmp469.args=_tmp457;
_tmp469.c_varargs=_tmp458;_tmp469.cyc_varargs=_tmp459;_tmp469.rgn_po=_tmp45A;
_tmp469.attributes=_tmp45B;_tmp469;});_tmp468;});_tmp467;}));struct Cyc_Absyn_PointerType_struct*
_tmp45E=({struct Cyc_Absyn_PointerType_struct*_tmp463=_cycalloc(sizeof(*_tmp463));
_tmp463[0]=({struct Cyc_Absyn_PointerType_struct _tmp464;_tmp464.tag=4;_tmp464.f1=({
struct Cyc_Absyn_PtrInfo _tmp465;_tmp465.elt_typ=(void*)ftyp;_tmp465.elt_tq=
_tmp44C;_tmp465.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp466;_tmp466.rgn=(void*)
_tmp44E;_tmp466.nullable=_tmp44F;_tmp466.bounds=_tmp450;_tmp466.zero_term=
_tmp451;_tmp466.ptrloc=0;_tmp466;});_tmp465;});_tmp464;});_tmp463;});struct Cyc_Absyn_Exp*
_tmp45F=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmp460=_cycalloc(sizeof(*_tmp460));_tmp460[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmp461;_tmp461.tag=14;_tmp461.f1=_tmp45F;_tmp461.f2=ts;_tmp461;});_tmp460;})));
e->topt=({struct Cyc_Core_Opt*_tmp462=_cycalloc(sizeof(*_tmp462));_tmp462->v=(
void*)((void*)_tmp45E);_tmp462;});};_pop_region(rgn);}goto _LL27D;_LL280:;_LL281:
goto _LL27D;_LL27D:;}goto _LL278;_LL27B:;_LL27C: goto _LL278;_LL278:;}goto _LL273;
_LL273:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static void
Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Position_Segment*loc=e->loc;void*t;{void*_tmp46B=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp46C;struct _tuple1*_tmp46D;struct Cyc_Absyn_Exp*_tmp46E;struct Cyc_List_List*
_tmp46F;struct Cyc_Core_Opt*_tmp470;struct Cyc_List_List*_tmp471;void*_tmp472;
struct _tuple1*_tmp473;void*_tmp474;void*_tmp475;struct Cyc_List_List*_tmp476;
struct Cyc_Absyn_Exp*_tmp477;void*_tmp478;struct Cyc_Absyn_Exp*_tmp479;struct Cyc_Core_Opt*
_tmp47A;struct Cyc_Absyn_Exp*_tmp47B;struct Cyc_Absyn_Exp*_tmp47C;struct Cyc_Absyn_Exp*
_tmp47D;struct Cyc_Absyn_Exp*_tmp47E;struct Cyc_Absyn_Exp*_tmp47F;struct Cyc_Absyn_Exp*
_tmp480;struct Cyc_Absyn_Exp*_tmp481;struct Cyc_Absyn_Exp*_tmp482;struct Cyc_Absyn_Exp*
_tmp483;struct Cyc_Absyn_Exp*_tmp484;struct Cyc_Absyn_Exp*_tmp485;struct Cyc_List_List*
_tmp486;struct Cyc_Absyn_VarargCallInfo*_tmp487;struct Cyc_Absyn_VarargCallInfo**
_tmp488;struct Cyc_Absyn_Exp*_tmp489;struct Cyc_Absyn_Exp*_tmp48A;struct Cyc_List_List*
_tmp48B;void*_tmp48C;struct Cyc_Absyn_Exp*_tmp48D;void*_tmp48E;void**_tmp48F;
struct Cyc_Absyn_Exp*_tmp490;struct Cyc_Absyn_Exp*_tmp491;struct Cyc_Absyn_Exp*
_tmp492;struct Cyc_Absyn_Exp*_tmp493;void*_tmp494;void*_tmp495;void*_tmp496;
struct Cyc_Absyn_Exp*_tmp497;struct Cyc_Absyn_Exp*_tmp498;struct _dynforward_ptr*
_tmp499;struct Cyc_Absyn_Exp*_tmp49A;struct _dynforward_ptr*_tmp49B;struct Cyc_Absyn_Exp*
_tmp49C;struct Cyc_Absyn_Exp*_tmp49D;struct Cyc_List_List*_tmp49E;struct _tuple2*
_tmp49F;struct Cyc_List_List*_tmp4A0;struct Cyc_List_List*_tmp4A1;struct Cyc_Absyn_Stmt*
_tmp4A2;struct Cyc_Absyn_Vardecl*_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A4;struct Cyc_Absyn_Exp*
_tmp4A5;int _tmp4A6;int*_tmp4A7;struct _tuple1*_tmp4A8;struct _tuple1**_tmp4A9;
struct Cyc_List_List*_tmp4AA;struct Cyc_List_List**_tmp4AB;struct Cyc_List_List*
_tmp4AC;struct Cyc_Absyn_Aggrdecl*_tmp4AD;struct Cyc_Absyn_Aggrdecl**_tmp4AE;void*
_tmp4AF;struct Cyc_List_List*_tmp4B0;struct Cyc_List_List*_tmp4B1;struct Cyc_Absyn_Tuniondecl*
_tmp4B2;struct Cyc_Absyn_Tunionfield*_tmp4B3;struct _tuple1*_tmp4B4;struct _tuple1**
_tmp4B5;struct Cyc_Absyn_Enumdecl*_tmp4B6;struct Cyc_Absyn_Enumfield*_tmp4B7;
struct _tuple1*_tmp4B8;struct _tuple1**_tmp4B9;void*_tmp4BA;struct Cyc_Absyn_Enumfield*
_tmp4BB;struct Cyc_Absyn_MallocInfo _tmp4BC;int _tmp4BD;int*_tmp4BE;struct Cyc_Absyn_Exp*
_tmp4BF;void**_tmp4C0;void***_tmp4C1;struct Cyc_Absyn_Exp*_tmp4C2;struct Cyc_Absyn_Exp**
_tmp4C3;int _tmp4C4;int*_tmp4C5;_LL283: if(*((int*)_tmp46B)!= 13)goto _LL285;
_tmp46C=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp46B)->f1;_LL284: Cyc_Tcexp_tcExpNoInst(
te,0,_tmp46C);return;_LL285: if(*((int*)_tmp46B)!= 2)goto _LL287;_tmp46D=((struct
Cyc_Absyn_UnknownId_e_struct*)_tmp46B)->f1;_LL286: Cyc_Tcexp_resolve_unknown_id(
te,e,_tmp46D);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL287: if(*((int*)_tmp46B)
!= 10)goto _LL289;_tmp46E=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp46B)->f1;
_tmp46F=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp46B)->f2;_LL288: Cyc_Tcexp_resolve_unknown_fn(
te,e,_tmp46E,_tmp46F);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL289: if(*((int*)
_tmp46B)!= 36)goto _LL28B;_tmp470=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp46B)->f1;_tmp471=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp46B)->f2;
_LL28A: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp471);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL28B: if(*((int*)_tmp46B)!= 0)goto _LL28D;_tmp472=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp46B)->f1;_LL28C: t=Cyc_Tcexp_tcConst(te,loc,
topt,_tmp472,e);goto _LL282;_LL28D: if(*((int*)_tmp46B)!= 1)goto _LL28F;_tmp473=((
struct Cyc_Absyn_Var_e_struct*)_tmp46B)->f1;_tmp474=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp46B)->f2;_LL28E: t=Cyc_Tcexp_tcVar(te,loc,_tmp473,_tmp474);goto _LL282;_LL28F:
if(*((int*)_tmp46B)!= 3)goto _LL291;_tmp475=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp46B)->f1;_tmp476=((struct Cyc_Absyn_Primop_e_struct*)_tmp46B)->f2;_LL290: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp475,_tmp476);goto _LL282;_LL291: if(*((int*)
_tmp46B)!= 5)goto _LL293;_tmp477=((struct Cyc_Absyn_Increment_e_struct*)_tmp46B)->f1;
_tmp478=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp46B)->f2;_LL292: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp477,_tmp478);goto _LL282;_LL293: if(*((int*)_tmp46B)!= 4)goto
_LL295;_tmp479=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp46B)->f1;_tmp47A=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp46B)->f2;_tmp47B=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp46B)->f3;_LL294: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp479,_tmp47A,_tmp47B);
goto _LL282;_LL295: if(*((int*)_tmp46B)!= 6)goto _LL297;_tmp47C=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp46B)->f1;_tmp47D=((struct Cyc_Absyn_Conditional_e_struct*)_tmp46B)->f2;
_tmp47E=((struct Cyc_Absyn_Conditional_e_struct*)_tmp46B)->f3;_LL296: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp47C,_tmp47D,_tmp47E);goto _LL282;_LL297: if(*((int*)_tmp46B)!= 7)
goto _LL299;_tmp47F=((struct Cyc_Absyn_And_e_struct*)_tmp46B)->f1;_tmp480=((struct
Cyc_Absyn_And_e_struct*)_tmp46B)->f2;_LL298: t=Cyc_Tcexp_tcAnd(te,loc,_tmp47F,
_tmp480);goto _LL282;_LL299: if(*((int*)_tmp46B)!= 8)goto _LL29B;_tmp481=((struct
Cyc_Absyn_Or_e_struct*)_tmp46B)->f1;_tmp482=((struct Cyc_Absyn_Or_e_struct*)
_tmp46B)->f2;_LL29A: t=Cyc_Tcexp_tcOr(te,loc,_tmp481,_tmp482);goto _LL282;_LL29B:
if(*((int*)_tmp46B)!= 9)goto _LL29D;_tmp483=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp46B)->f1;_tmp484=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp46B)->f2;_LL29C: t=
Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp483,_tmp484);goto _LL282;_LL29D: if(*((int*)
_tmp46B)!= 11)goto _LL29F;_tmp485=((struct Cyc_Absyn_FnCall_e_struct*)_tmp46B)->f1;
_tmp486=((struct Cyc_Absyn_FnCall_e_struct*)_tmp46B)->f2;_tmp487=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp46B)->f3;_tmp488=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)
_tmp46B)->f3;_LL29E: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp485,_tmp486,_tmp488);
goto _LL282;_LL29F: if(*((int*)_tmp46B)!= 12)goto _LL2A1;_tmp489=((struct Cyc_Absyn_Throw_e_struct*)
_tmp46B)->f1;_LL2A0: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp489);goto _LL282;_LL2A1:
if(*((int*)_tmp46B)!= 14)goto _LL2A3;_tmp48A=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp46B)->f1;_tmp48B=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp46B)->f2;_LL2A2:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp48A,_tmp48B);goto _LL282;_LL2A3: if(*((
int*)_tmp46B)!= 15)goto _LL2A5;_tmp48C=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp46B)->f1;_tmp48D=((struct Cyc_Absyn_Cast_e_struct*)_tmp46B)->f2;_tmp48E=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp46B)->f4;_tmp48F=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp46B)->f4);_LL2A4: t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp48C,_tmp48D,(void**)
_tmp48F);goto _LL282;_LL2A5: if(*((int*)_tmp46B)!= 16)goto _LL2A7;_tmp490=((struct
Cyc_Absyn_Address_e_struct*)_tmp46B)->f1;_LL2A6: t=Cyc_Tcexp_tcAddress(te,loc,e,
topt,_tmp490);goto _LL282;_LL2A7: if(*((int*)_tmp46B)!= 17)goto _LL2A9;_tmp491=((
struct Cyc_Absyn_New_e_struct*)_tmp46B)->f1;_tmp492=((struct Cyc_Absyn_New_e_struct*)
_tmp46B)->f2;_LL2A8: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp491,e,_tmp492);goto _LL282;
_LL2A9: if(*((int*)_tmp46B)!= 19)goto _LL2AB;_tmp493=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp46B)->f1;_LL2AA: {void*_tmp4C6=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp493);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4C6);goto _LL282;}_LL2AB: if(*((int*)_tmp46B)!= 18)goto _LL2AD;
_tmp494=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp46B)->f1;_LL2AC: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp494);goto _LL282;_LL2AD: if(*((int*)_tmp46B)!= 20)goto _LL2AF;
_tmp495=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp46B)->f1;_tmp496=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp46B)->f2;_LL2AE: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp495,_tmp496);goto _LL282;_LL2AF: if(*((int*)_tmp46B)!= 21)goto _LL2B1;
_LL2B0:({void*_tmp4C7[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp4C8="gen() not in top-level initializer";
_tag_dynforward(_tmp4C8,sizeof(char),_get_zero_arr_size(_tmp4C8,35));}),
_tag_dynforward(_tmp4C7,sizeof(void*),0));});return;_LL2B1: if(*((int*)_tmp46B)!= 
22)goto _LL2B3;_tmp497=((struct Cyc_Absyn_Deref_e_struct*)_tmp46B)->f1;_LL2B2: t=
Cyc_Tcexp_tcDeref(te,loc,topt,_tmp497);goto _LL282;_LL2B3: if(*((int*)_tmp46B)!= 
23)goto _LL2B5;_tmp498=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp46B)->f1;
_tmp499=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp46B)->f2;_LL2B4: t=Cyc_Tcexp_tcAggrMember(
te,loc,topt,e,_tmp498,_tmp499);goto _LL282;_LL2B5: if(*((int*)_tmp46B)!= 24)goto
_LL2B7;_tmp49A=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp46B)->f1;_tmp49B=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp46B)->f2;_LL2B6: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp49A,_tmp49B);goto _LL282;_LL2B7: if(*((int*)_tmp46B)!= 25)goto
_LL2B9;_tmp49C=((struct Cyc_Absyn_Subscript_e_struct*)_tmp46B)->f1;_tmp49D=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp46B)->f2;_LL2B8: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp49C,_tmp49D);goto _LL282;_LL2B9: if(*((int*)_tmp46B)!= 26)goto
_LL2BB;_tmp49E=((struct Cyc_Absyn_Tuple_e_struct*)_tmp46B)->f1;_LL2BA: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp49E);goto _LL282;_LL2BB: if(*((int*)_tmp46B)!= 27)goto _LL2BD;
_tmp49F=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp46B)->f1;_tmp4A0=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp46B)->f2;_LL2BC: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp49F,_tmp4A0);goto
_LL282;_LL2BD: if(*((int*)_tmp46B)!= 28)goto _LL2BF;_tmp4A1=((struct Cyc_Absyn_Array_e_struct*)
_tmp46B)->f1;_LL2BE: {void**elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp4C9=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp4CA;void*_tmp4CB;void**
_tmp4CC;struct Cyc_Absyn_Conref*_tmp4CD;_LL2D0: if(_tmp4C9 <= (void*)4)goto _LL2D2;
if(*((int*)_tmp4C9)!= 7)goto _LL2D2;_tmp4CA=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4C9)->f1;_tmp4CB=(void*)_tmp4CA.elt_type;_tmp4CC=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp4C9)->f1).elt_type;_tmp4CD=_tmp4CA.zero_term;_LL2D1: elt_topt=(void**)_tmp4CC;
zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp4CD);
goto _LL2CF;_LL2D2:;_LL2D3: goto _LL2CF;_LL2CF:;}t=Cyc_Tcexp_tcArray(te,loc,
elt_topt,zero_term,_tmp4A1);goto _LL282;}_LL2BF: if(*((int*)_tmp46B)!= 37)goto
_LL2C1;_tmp4A2=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp46B)->f1;_LL2C0: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp4A2);goto _LL282;_LL2C1: if(*((int*)_tmp46B)!= 29)goto _LL2C3;
_tmp4A3=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp46B)->f1;_tmp4A4=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp46B)->f2;_tmp4A5=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp46B)->f3;_tmp4A6=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp46B)->f4;
_tmp4A7=(int*)&((struct Cyc_Absyn_Comprehension_e_struct*)_tmp46B)->f4;_LL2C2: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4A3,_tmp4A4,_tmp4A5,_tmp4A7);goto
_LL282;_LL2C3: if(*((int*)_tmp46B)!= 30)goto _LL2C5;_tmp4A8=((struct Cyc_Absyn_Struct_e_struct*)
_tmp46B)->f1;_tmp4A9=(struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp46B)->f1;_tmp4AA=((struct Cyc_Absyn_Struct_e_struct*)_tmp46B)->f2;_tmp4AB=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp46B)->f2;_tmp4AC=((
struct Cyc_Absyn_Struct_e_struct*)_tmp46B)->f3;_tmp4AD=((struct Cyc_Absyn_Struct_e_struct*)
_tmp46B)->f4;_tmp4AE=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp46B)->f4;_LL2C4: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp4A9,_tmp4AB,_tmp4AC,
_tmp4AE);goto _LL282;_LL2C5: if(*((int*)_tmp46B)!= 31)goto _LL2C7;_tmp4AF=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp46B)->f1;_tmp4B0=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp46B)->f2;_LL2C6: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp4AF,_tmp4B0);goto _LL282;
_LL2C7: if(*((int*)_tmp46B)!= 32)goto _LL2C9;_tmp4B1=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp46B)->f1;_tmp4B2=((struct Cyc_Absyn_Tunion_e_struct*)_tmp46B)->f2;_tmp4B3=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp46B)->f3;_LL2C8: t=Cyc_Tcexp_tcTunion(te,loc,
topt,e,_tmp4B1,_tmp4B2,_tmp4B3);goto _LL282;_LL2C9: if(*((int*)_tmp46B)!= 33)goto
_LL2CB;_tmp4B4=((struct Cyc_Absyn_Enum_e_struct*)_tmp46B)->f1;_tmp4B5=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp46B)->f1;_tmp4B6=((struct Cyc_Absyn_Enum_e_struct*)
_tmp46B)->f2;_tmp4B7=((struct Cyc_Absyn_Enum_e_struct*)_tmp46B)->f3;_LL2CA:*
_tmp4B5=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp4B7))->name;t=(void*)({
struct Cyc_Absyn_EnumType_struct*_tmp4CE=_cycalloc(sizeof(*_tmp4CE));_tmp4CE[0]=({
struct Cyc_Absyn_EnumType_struct _tmp4CF;_tmp4CF.tag=12;_tmp4CF.f1=((struct Cyc_Absyn_Enumdecl*)
_check_null(_tmp4B6))->name;_tmp4CF.f2=_tmp4B6;_tmp4CF;});_tmp4CE;});goto _LL282;
_LL2CB: if(*((int*)_tmp46B)!= 34)goto _LL2CD;_tmp4B8=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp46B)->f1;_tmp4B9=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp46B)->f1;_tmp4BA=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp46B)->f2;
_tmp4BB=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp46B)->f3;_LL2CC:*_tmp4B9=((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp4BB))->name;t=_tmp4BA;goto _LL282;
_LL2CD: if(*((int*)_tmp46B)!= 35)goto _LL282;_tmp4BC=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp46B)->f1;_tmp4BD=_tmp4BC.is_calloc;_tmp4BE=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp46B)->f1).is_calloc;_tmp4BF=_tmp4BC.rgn;_tmp4C0=_tmp4BC.elt_type;_tmp4C1=(
void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp46B)->f1).elt_type;_tmp4C2=
_tmp4BC.num_elts;_tmp4C3=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp46B)->f1).num_elts;_tmp4C4=_tmp4BC.fat_result;_tmp4C5=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp46B)->f1).fat_result;_LL2CE: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp4BF,_tmp4C1,
_tmp4C3,_tmp4BE,_tmp4C5);goto _LL282;_LL282:;}e->topt=({struct Cyc_Core_Opt*
_tmp4D0=_cycalloc(sizeof(*_tmp4D0));_tmp4D0->v=(void*)t;_tmp4D0;});}
