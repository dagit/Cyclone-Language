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
 struct Cyc_Core_Opt{void*v;};void*Cyc_Core_identity(void*);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};int isalnum(int);struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*
env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];void*Cyc_List_fold_left(
void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x);void*Cyc_List_fold_right_c(
void*(*f)(void*,void*,void*),void*,struct Cyc_List_List*x,void*accum);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_flatten(struct Cyc_List_List*x);extern char Cyc_List_Nth[8];int Cyc_List_forall(
int(*pred)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_zip(struct
Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_zip3(struct
Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z);struct _tuple0{
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_split(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_tabulate(int n,void*(*f)(int));
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);struct
Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct
Cyc_Iter_Iter,void*);typedef struct{int __count;union{unsigned int __wch;char __wchb[
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
int Cyc_printf(struct _dynforward_ptr,struct _dynforward_ptr);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dynforward_ptr f1;};struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,
void*));struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict*d,void*k);void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict*
d);struct _tuple1{void*f1;void*f2;};struct _tuple1*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple1*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*Cyc_Dict_delete(struct
Cyc_Dict_Dict*,void*);unsigned int Cyc_strlen(struct _dynforward_ptr s);int Cyc_strcmp(
struct _dynforward_ptr s1,struct _dynforward_ptr s2);struct _dynforward_ptr Cyc_strconcat(
struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr Cyc_strdup(
struct _dynforward_ptr src);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*
Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table*t,void*key,void*val);void*Cyc_Hashtable_lookup(struct
Cyc_Hashtable_Table*t,void*key);struct Cyc_Lineno_Pos{struct _dynforward_ptr
logical_file;struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct Cyc_Position_Error{struct _dynforward_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];
struct Cyc_Typerep_Int_struct{int tag;int f1;unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{
int tag;unsigned int f1;void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;}
;struct _tuple2{unsigned int f1;struct _dynforward_ptr f2;void*f3;};struct Cyc_Typerep_Struct_struct{
int tag;struct _dynforward_ptr*f1;unsigned int f2;struct _dynforward_ptr f3;};struct
_tuple3{unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;
unsigned int f1;struct _dynforward_ptr f2;};struct _tuple4{unsigned int f1;struct
_dynforward_ptr f2;};struct Cyc_Typerep_TUnion_struct{int tag;struct _dynforward_ptr
f1;struct _dynforward_ptr f2;struct _dynforward_ptr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _dynforward_ptr f1;struct _dynforward_ptr f2;unsigned int f3;struct
_dynforward_ptr f4;};struct _tuple5{struct _dynforward_ptr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _dynforward_ptr f1;struct _dynforward_ptr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _dynforward_ptr*f1;int f2;struct _dynforward_ptr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _dynforward_ptr*f1;int f2;struct _dynforward_ptr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);struct Cyc_timespec{long tv_sec;long tv_nsec;};struct Cyc_tm{int tm_sec;int
tm_min;int tm_hour;int tm_mday;int tm_mon;int tm_year;int tm_wday;int tm_yday;int
tm_isdst;long tm_gmtoff;const char*tm_zone;};struct Cyc_itimerspec{struct Cyc_timespec
it_interval;struct Cyc_timespec it_value;};struct Cyc_Absyn_Rel_n_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple6{void*f1;struct _dynforward_ptr*f2;};struct Cyc_Absyn_Conref;
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
_tuple6*name;int is_xtunion;int is_flat;};struct Cyc_Absyn_UnknownTunion_struct{int
tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int
tag;struct Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;
struct Cyc_List_List*targs;struct Cyc_Core_Opt*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple6*tunion_name;struct _tuple6*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
int tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Absyn_TunionFieldInfo{
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{
int tag;void*f1;struct _tuple6*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct
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
;struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple6*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_SizeofType_struct{int tag;void*f1;};struct Cyc_Absyn_RgnHandleType_struct{
int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple6*f1;
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
int tag;void*f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple6*f1;void*f2;};
struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple6*f1;};struct Cyc_Absyn_Primop_e_struct{
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
struct Cyc_List_List*f1;};struct _tuple7{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple7*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple6*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple6*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple6*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;
void*r;struct Cyc_Position_Segment*loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple8{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple8 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dynforward_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple8 f2;struct _tuple8 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dynforward_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple8 f2;
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple6*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple6*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple6*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple6*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dynforward_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{void*
kind;void*sc;struct _tuple6*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple6*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple6*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;int is_flat;};struct Cyc_Absyn_Enumfield{struct _tuple6*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple6*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple6*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _dynforward_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple6*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple6*,struct _tuple6*);void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*
x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
void*Cyc_Absyn_const_string_typ(void*rgn);void*Cyc_Absyn_star_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_dynforward_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknownid_exp(struct _tuple6*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(
void*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(
struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple6*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(void*info);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct
_dynforward_ptr Cyc_Absynpp_typ2cstring(void*);struct _tuple9{unsigned int f1;int f2;
};struct _tuple9 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[11];struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_typecmp(void*,
void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,void*,int allow_unique);struct
Cyc_Tcgenrep_RepInfo{struct Cyc_List_List*decls;struct Cyc_Absyn_Exp*exp;struct Cyc_List_List*
dependencies;struct Cyc_Core_Opt*fwd_decl;int emitted;int is_extern;};struct Cyc_Dict_Dict*
Cyc_Tcgenrep_empty_typerep_dict(){return((struct Cyc_Dict_Dict*(*)(int(*cmp)(void*,
void*)))Cyc_Dict_empty)(Cyc_Tcutil_typecmp);}void Cyc_Tcgenrep_print_dict_entry(
void*type,struct Cyc_Tcgenrep_RepInfo*info){({struct Cyc_Int_pa_struct _tmp4;_tmp4.tag=
1;_tmp4.f1=(unsigned long)info->emitted;{struct Cyc_Int_pa_struct _tmp3;_tmp3.tag=
1;_tmp3.f1=(unsigned int)info;{struct Cyc_String_pa_struct _tmp2;_tmp2.tag=0;_tmp2.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(type));{
void*_tmp0[3]={& _tmp2,& _tmp3,& _tmp4};Cyc_printf(({const char*_tmp1="(%s,%x:%d,";
_tag_dynforward(_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,11));}),
_tag_dynforward(_tmp0,sizeof(void*),3));}}}});{struct Cyc_List_List*p=info->dependencies;
for(0;p != 0;p=p->tl){({struct Cyc_Int_pa_struct _tmp7;_tmp7.tag=1;_tmp7.f1=(
unsigned int)((struct Cyc_Tcgenrep_RepInfo*)p->hd);{void*_tmp5[1]={& _tmp7};Cyc_printf(({
const char*_tmp6="%x";_tag_dynforward(_tmp6,sizeof(char),_get_zero_arr_size(_tmp6,
3));}),_tag_dynforward(_tmp5,sizeof(void*),1));}});if(p->tl != 0)({void*_tmp8[0]={};
Cyc_printf(({const char*_tmp9=",";_tag_dynforward(_tmp9,sizeof(char),
_get_zero_arr_size(_tmp9,2));}),_tag_dynforward(_tmp8,sizeof(void*),0));});}}({
void*_tmpA[0]={};Cyc_printf(({const char*_tmpB=")\n";_tag_dynforward(_tmpB,
sizeof(char),_get_zero_arr_size(_tmpB,3));}),_tag_dynforward(_tmpA,sizeof(void*),
0));});}void Cyc_Tcgenrep_print_typerep_dict(struct Cyc_Dict_Dict*dict){((void(*)(
void(*f)(void*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Dict_Dict*d))Cyc_Dict_iter)(
Cyc_Tcgenrep_print_dict_entry,dict);}static int Cyc_Tcgenrep_rephash(struct Cyc_Tcgenrep_RepInfo*
ri){return(int)ri;}static int Cyc_Tcgenrep_repcmp(struct Cyc_Tcgenrep_RepInfo*r1,
struct Cyc_Tcgenrep_RepInfo*r2){unsigned int r1p;unsigned int r2p;r1p=(unsigned int)
r1;r2p=(unsigned int)r2;if(r1 < r2)return - 1;else{if(r1 == r2)return 0;else{return 1;}}}
static struct Cyc_Absyn_Tunionfield*Cyc_Tcgenrep_getField(struct Cyc_Absyn_Tuniondecl*
td,struct _tuple6*fieldname){if(td->fields == 0)({void*_tmpC[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpD="Could not find field in tuniondecl";
_tag_dynforward(_tmpD,sizeof(char),_get_zero_arr_size(_tmpD,35));}),
_tag_dynforward(_tmpC,sizeof(void*),0));});else{struct Cyc_List_List*l=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;for(0;l != 0;l=l->tl){if(!Cyc_Absyn_qvar_cmp(((
struct Cyc_Absyn_Tunionfield*)l->hd)->name,fieldname))return(struct Cyc_Absyn_Tunionfield*)
l->hd;}}({void*_tmpE[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpF="Could not find field in tuniondecl";
_tag_dynforward(_tmpF,sizeof(char),_get_zero_arr_size(_tmpF,35));}),
_tag_dynforward(_tmpE,sizeof(void*),0));});}static char _tmp10[9]="Tcgenrep";
static struct _dynforward_ptr Cyc_Tcgenrep_tcgenrep_nm={_tmp10,_tmp10 + 9};static
struct Cyc_List_List Cyc_Tcgenrep_l={(void*)& Cyc_Tcgenrep_tcgenrep_nm,0};static
struct Cyc_Absyn_Abs_n_struct Cyc_Tcgenrep_tcgenrep_nmspace={1,(struct Cyc_List_List*)&
Cyc_Tcgenrep_l};static void*Cyc_Tcgenrep_tcgenrep_nmspace2=(void*)& Cyc_Tcgenrep_tcgenrep_nmspace;
static char _tmp12[8]="Typerep";static struct _dynforward_ptr Cyc_Tcgenrep_typerep_nm={
_tmp12,_tmp12 + 8};static struct Cyc_List_List Cyc_Tcgenrep_l2={(void*)& Cyc_Tcgenrep_typerep_nm,
0};static struct Cyc_Absyn_Abs_n_struct Cyc_Tcgenrep_typerep_nmspace={1,(struct Cyc_List_List*)&
Cyc_Tcgenrep_l2};static struct Cyc_List_List*Cyc_Tcgenrep_dfsvisit(struct Cyc_Tcgenrep_RepInfo*
ri,struct Cyc_Hashtable_Table*visited){struct _handler_cons _tmp14;_push_handler(&
_tmp14);{int _tmp16=0;if(setjmp(_tmp14.handler))_tmp16=1;if(!_tmp16){((int(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Tcgenrep_RepInfo*key))Cyc_Hashtable_lookup)(
visited,ri);{struct Cyc_List_List*_tmp17=0;_npop_handler(0);return _tmp17;};
_pop_handler();}else{void*_tmp15=(void*)_exn_thrown;void*_tmp19=_tmp15;_LL1: if(
_tmp19 != Cyc_Core_Not_found)goto _LL3;_LL2:((void(*)(struct Cyc_Hashtable_Table*t,
struct Cyc_Tcgenrep_RepInfo*key,int val))Cyc_Hashtable_insert)(visited,ri,1);{
struct Cyc_List_List*ds=0;{struct Cyc_List_List*l=ri->dependencies;for(0;l != 0;l=l->tl){
ds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
ds,Cyc_Tcgenrep_dfsvisit((struct Cyc_Tcgenrep_RepInfo*)l->hd,visited));}}if(ri->emitted
== 0){if(ri->fwd_decl != 0)ds=({struct Cyc_List_List*_tmp1A=_cycalloc(sizeof(*
_tmp1A));_tmp1A->hd=({struct Cyc_Absyn_Decl*_tmp1B[1];_tmp1B[0]=(struct Cyc_Absyn_Decl*)((
struct Cyc_Core_Opt*)_check_null(ri->fwd_decl))->v;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1B,sizeof(struct Cyc_Absyn_Decl*),
1));});_tmp1A->tl=ds;_tmp1A;});{struct Cyc_List_List*_tmp1C=({struct Cyc_List_List*
_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D->hd=ri->decls;_tmp1D->tl=0;_tmp1D;});ri->emitted=
1;return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(ds,_tmp1C);}}else{return ds;}}_LL3:;_LL4:(void)_throw(_tmp19);
_LL0:;}}}static struct Cyc_List_List*Cyc_Tcgenrep_dfs(struct Cyc_Tcgenrep_RepInfo*
ri){struct Cyc_Hashtable_Table*tab=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(
struct Cyc_Tcgenrep_RepInfo*,struct Cyc_Tcgenrep_RepInfo*),int(*hash)(struct Cyc_Tcgenrep_RepInfo*)))
Cyc_Hashtable_create)(53,Cyc_Tcgenrep_repcmp,Cyc_Tcgenrep_rephash);struct Cyc_List_List*
ds=Cyc_Tcgenrep_dfsvisit(ri,tab);return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_flatten)(ds);}static char _tmp1E[11]="Typestruct";static struct
_dynforward_ptr Cyc_Tcgenrep_typestruct_str={_tmp1E,_tmp1E + 11};static char _tmp1F[
4]="Var";static struct _dynforward_ptr Cyc_Tcgenrep_var_str={_tmp1F,_tmp1F + 4};
static char _tmp20[4]="Int";static struct _dynforward_ptr Cyc_Tcgenrep_int_str={
_tmp20,_tmp20 + 4};static char _tmp21[6]="Float";static struct _dynforward_ptr Cyc_Tcgenrep_float_str={
_tmp21,_tmp21 + 6};static char _tmp22[7]="Double";static struct _dynforward_ptr Cyc_Tcgenrep_double_str={
_tmp22,_tmp22 + 7};static char _tmp23[8]="ThinPtr";static struct _dynforward_ptr Cyc_Tcgenrep_thinptr_str={
_tmp23,_tmp23 + 8};static char _tmp24[7]="FatPtr";static struct _dynforward_ptr Cyc_Tcgenrep_fatptr_str={
_tmp24,_tmp24 + 7};static char _tmp25[6]="Tuple";static struct _dynforward_ptr Cyc_Tcgenrep_tuple_str={
_tmp25,_tmp25 + 6};static char _tmp26[12]="TUnionField";static struct _dynforward_ptr
Cyc_Tcgenrep_tunionfield_str={_tmp26,_tmp26 + 12};static char _tmp27[7]="Struct";
static struct _dynforward_ptr Cyc_Tcgenrep_struct_str={_tmp27,_tmp27 + 7};static char
_tmp28[7]="TUnion";static struct _dynforward_ptr Cyc_Tcgenrep_tunion_str={_tmp28,
_tmp28 + 7};static char _tmp29[8]="XTUnion";static struct _dynforward_ptr Cyc_Tcgenrep_xtunion_str={
_tmp29,_tmp29 + 8};static char _tmp2A[6]="Union";static struct _dynforward_ptr Cyc_Tcgenrep_union_str={
_tmp2A,_tmp2A + 6};static char _tmp2B[5]="Enum";static struct _dynforward_ptr Cyc_Tcgenrep_enum_str={
_tmp2B,_tmp2B + 5};static char _tmp2C[7]="name_t";static struct _dynforward_ptr Cyc_Tcgenrep_name_t_str={
_tmp2C,_tmp2C + 7};static struct _tuple6*Cyc_Tcgenrep_typerep_name(struct
_dynforward_ptr*name){return({struct _tuple6*_tmp2D=_cycalloc(sizeof(*_tmp2D));
_tmp2D->f1=(void*)& Cyc_Tcgenrep_typerep_nmspace;_tmp2D->f2=name;_tmp2D;});}
static int Cyc_Tcgenrep_gen_id_counter=0;static struct _dynforward_ptr*Cyc_Tcgenrep_new_gen_id(
struct _dynforward_ptr name){return({struct _dynforward_ptr*_tmp2E=_cycalloc(
sizeof(*_tmp2E));_tmp2E[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp32;_tmp32.tag=1;_tmp32.f1=(unsigned long)Cyc_Tcgenrep_gen_id_counter ++;{
struct Cyc_String_pa_struct _tmp31;_tmp31.tag=0;_tmp31.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)name);{void*_tmp2F[2]={& _tmp31,& _tmp32};Cyc_aprintf(({
const char*_tmp30="_gen%s_%d";_tag_dynforward(_tmp30,sizeof(char),
_get_zero_arr_size(_tmp30,10));}),_tag_dynforward(_tmp2F,sizeof(void*),2));}}});
_tmp2E;});}static void Cyc_Tcgenrep_print_params(struct Cyc_List_List*l){({void*
_tmp33[0]={};Cyc_printf(({const char*_tmp34="<";_tag_dynforward(_tmp34,sizeof(
char),_get_zero_arr_size(_tmp34,2));}),_tag_dynforward(_tmp33,sizeof(void*),0));});{
struct Cyc_List_List*p=l;for(0;p != 0;p=p->tl){({struct Cyc_Int_pa_struct _tmp38;
_tmp38.tag=1;_tmp38.f1=(unsigned long)((int)(p->tl != 0?',':' '));{struct Cyc_String_pa_struct
_tmp37;_tmp37.tag=0;_tmp37.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)p->hd));{void*_tmp35[2]={& _tmp37,& _tmp38};Cyc_printf(({const char*_tmp36="%s%c";
_tag_dynforward(_tmp36,sizeof(char),_get_zero_arr_size(_tmp36,5));}),
_tag_dynforward(_tmp35,sizeof(void*),2));}}});}}({void*_tmp39[0]={};Cyc_printf(({
const char*_tmp3A=">\n";_tag_dynforward(_tmp3A,sizeof(char),_get_zero_arr_size(
_tmp3A,3));}),_tag_dynforward(_tmp39,sizeof(void*),0));});}static void Cyc_Tcgenrep_print_tvars(
struct Cyc_List_List*l){({void*_tmp3B[0]={};Cyc_printf(({const char*_tmp3C="<";
_tag_dynforward(_tmp3C,sizeof(char),_get_zero_arr_size(_tmp3C,2));}),
_tag_dynforward(_tmp3B,sizeof(void*),0));});{struct Cyc_List_List*p=l;for(0;p != 0;
p=p->tl){({struct Cyc_Int_pa_struct _tmp40;_tmp40.tag=1;_tmp40.f1=(unsigned long)((
int)(p->tl != 0?',':' '));{struct Cyc_String_pa_struct _tmp3F;_tmp3F.tag=0;_tmp3F.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*((struct Cyc_Absyn_Tvar*)p->hd)->name);{
void*_tmp3D[2]={& _tmp3F,& _tmp40};Cyc_printf(({const char*_tmp3E="%s%c";
_tag_dynforward(_tmp3E,sizeof(char),_get_zero_arr_size(_tmp3E,5));}),
_tag_dynforward(_tmp3D,sizeof(void*),2));}}});}}({void*_tmp41[0]={};Cyc_printf(({
const char*_tmp42=">\n";_tag_dynforward(_tmp42,sizeof(char),_get_zero_arr_size(
_tmp42,3));}),_tag_dynforward(_tmp41,sizeof(void*),0));});}static struct Cyc_Absyn_Rel_n_struct
Cyc_Tcgenrep_top_nm_0={0,0};static void*Cyc_Tcgenrep_top_nm=(void*)& Cyc_Tcgenrep_top_nm_0;
static struct _tuple6*Cyc_Tcgenrep_toplevel_name(struct _dynforward_ptr*name){
return({struct _tuple6*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->f1=Cyc_Tcgenrep_top_nm;
_tmp44->f2=(struct _dynforward_ptr*)name;_tmp44;});}static struct _tuple6*Cyc_Tcgenrep_relative_name(
struct Cyc_List_List*pathl,struct _dynforward_ptr*name){struct Cyc_Absyn_Rel_n_struct*
path=({struct Cyc_Absyn_Rel_n_struct*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46[0]=({
struct Cyc_Absyn_Rel_n_struct _tmp47;_tmp47.tag=0;_tmp47.f1=pathl;_tmp47;});_tmp46;});
return({struct _tuple6*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->f1=(void*)path;
_tmp45->f2=(struct _dynforward_ptr*)name;_tmp45;});}static struct Cyc_Absyn_Decl*
Cyc_Tcgenrep_gen_decl(struct _dynforward_ptr*name,void*type,struct Cyc_Absyn_Exp*
init,struct Cyc_Position_Segment*seg){struct _tuple6*qvar=Cyc_Tcgenrep_toplevel_name(
name);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(qvar,type,init);(void*)(
vd->sc=(void*)((void*)0));{struct Cyc_Absyn_Var_d_struct*r1=({struct Cyc_Absyn_Var_d_struct*
_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48[0]=({struct Cyc_Absyn_Var_d_struct _tmp49;
_tmp49.tag=0;_tmp49.f1=vd;_tmp49;});_tmp48;});void*r2=(void*)r1;return Cyc_Absyn_new_decl(
r2,seg);}}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_vardecl(struct _tuple6*name,
void*type,struct Cyc_Absyn_Exp*init,void*sc,struct Cyc_Position_Segment*seg){
struct _tuple6*topname=Cyc_Tcgenrep_toplevel_name((*name).f2);struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(topname,type,init);(void*)(vd->sc=(void*)sc);{struct Cyc_Absyn_Var_d_struct*
r1=({struct Cyc_Absyn_Var_d_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A[0]=({
struct Cyc_Absyn_Var_d_struct _tmp4B;_tmp4B.tag=0;_tmp4B.f1=vd;_tmp4B;});_tmp4A;});
void*r2=(void*)r1;return Cyc_Absyn_new_decl(r2,seg);}}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_cnst_string(struct _dynforward_ptr s,struct Cyc_Position_Segment*seg){
return Cyc_Absyn_const_exp((void*)((void*)({struct Cyc_Absyn_String_c_struct*
_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C[0]=({struct Cyc_Absyn_String_c_struct
_tmp4D;_tmp4D.tag=5;_tmp4D.f1=s;_tmp4D;});_tmp4C;})),seg);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_cnst_string_cls(struct Cyc_Position_Segment*seg,struct
_dynforward_ptr*s){return Cyc_Absyn_const_exp((void*)((void*)({struct Cyc_Absyn_String_c_struct*
_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E[0]=({struct Cyc_Absyn_String_c_struct
_tmp4F;_tmp4F.tag=5;_tmp4F.f1=*s;_tmp4F;});_tmp4E;})),seg);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_cnst_qvar_string_cls(struct Cyc_Position_Segment*seg,struct _tuple6*s){
return Cyc_Absyn_const_exp((void*)((void*)({struct Cyc_Absyn_String_c_struct*
_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50[0]=({struct Cyc_Absyn_String_c_struct
_tmp51;_tmp51.tag=5;_tmp51.f1=*(*s).f2;_tmp51;});_tmp50;})),seg);}static struct
Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_int(int i,struct Cyc_Position_Segment*seg){return
Cyc_Absyn_const_exp((void*)((void*)({struct Cyc_Absyn_Int_c_struct*_tmp52=
_cycalloc(sizeof(*_tmp52));_tmp52[0]=({struct Cyc_Absyn_Int_c_struct _tmp53;_tmp53.tag=
2;_tmp53.f1=(void*)((void*)0);_tmp53.f2=i;_tmp53;});_tmp52;})),seg);}static
struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_int_cls(struct Cyc_Position_Segment*seg,int
i){return Cyc_Absyn_const_exp((void*)((void*)({struct Cyc_Absyn_Int_c_struct*
_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54[0]=({struct Cyc_Absyn_Int_c_struct _tmp55;
_tmp55.tag=2;_tmp55.f1=(void*)((void*)0);_tmp55.f2=i;_tmp55;});_tmp54;})),seg);}
static int Cyc_Tcgenrep_size_of2int(void*sz){void*_tmp56=sz;_LL6: if((int)_tmp56 != 
0)goto _LL8;_LL7: return 8;_LL8: if((int)_tmp56 != 1)goto _LLA;_LL9: return 16;_LLA: if((
int)_tmp56 != 2)goto _LLC;_LLB: return 32;_LLC: if((int)_tmp56 != 3)goto _LLE;_LLD:
return 32;_LLE: if((int)_tmp56 != 4)goto _LL5;_LLF: return 64;_LL5:;}static void*Cyc_Tcgenrep_tunion_typ(
struct _tuple6*name){return(void*)({struct Cyc_Absyn_TunionType_struct*_tmp57=
_cycalloc(sizeof(*_tmp57));_tmp57[0]=({struct Cyc_Absyn_TunionType_struct _tmp58;
_tmp58.tag=2;_tmp58.f1=({struct Cyc_Absyn_TunionInfo _tmp59;_tmp59.tunion_info=(
void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*_tmp5B=_cycalloc(sizeof(*
_tmp5B));_tmp5B[0]=({struct Cyc_Absyn_UnknownTunion_struct _tmp5C;_tmp5C.tag=0;
_tmp5C.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp5D;_tmp5D.name=name;_tmp5D.is_xtunion=
0;_tmp5D.is_flat=0;_tmp5D;});_tmp5C;});_tmp5B;}));_tmp59.targs=0;_tmp59.rgn=({
struct Cyc_Core_Opt*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->v=(void*)((void*)2);
_tmp5A;});_tmp59;});_tmp58;});_tmp57;});}static void*Cyc_Tcgenrep_tunionfield_typ(
struct _tuple6*name,struct _tuple6*fieldname){return(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp5F;_tmp5F.tag=3;_tmp5F.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp60;_tmp60.field_info=(
void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*_tmp61=_cycalloc(
sizeof(*_tmp61));_tmp61[0]=({struct Cyc_Absyn_UnknownTunionfield_struct _tmp62;
_tmp62.tag=0;_tmp62.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp63;_tmp63.tunion_name=
name;_tmp63.field_name=fieldname;_tmp63.is_xtunion=0;_tmp63;});_tmp62;});_tmp61;}));
_tmp60.targs=0;_tmp60;});_tmp5F;});_tmp5E;});}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_call_exp(
struct _tuple6*name,struct Cyc_List_List*args,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_unknowncall_exp(Cyc_Absyn_unknownid_exp(name,loc),args,loc);}
static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_tunion_constructor_decl(struct _tuple6*
tunionname,struct _tuple6*fieldname,struct _tuple6*varname,struct Cyc_List_List*
args,void*sc,struct Cyc_Position_Segment*loc){void*_tmp64=Cyc_Tcgenrep_tunionfield_typ(
tunionname,fieldname);struct Cyc_Absyn_Exp*_tmp65=Cyc_Tcgenrep_call_exp(fieldname,
args,loc);struct Cyc_Absyn_Decl*_tmp66=Cyc_Tcgenrep_gen_vardecl(varname,_tmp64,(
struct Cyc_Absyn_Exp*)_tmp65,sc,loc);return _tmp66;}struct _tuple10{struct Cyc_Absyn_Tqual
f1;void*f2;};static void*Cyc_Tcgenrep_get_second(struct _tuple10*pair){return(*
pair).f2;}struct _tuple11{struct _dynforward_ptr*f1;struct Cyc_Absyn_Exp*f2;};
static struct _dynforward_ptr*Cyc_Tcgenrep_get_first(struct _tuple11*pair){return(*
pair).f1;}static char _tmp67[5]="list";static struct _dynforward_ptr Cyc_Tcgenrep_list_str={
_tmp67,_tmp67 + 5};static char _tmp68[5]="List";static struct _dynforward_ptr Cyc_Tcgenrep_List_str={
_tmp68,_tmp68 + 5};struct _tuple12{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;}
;static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_tuple2_exp_cls(struct Cyc_Position_Segment*
loc,struct _tuple12*es){struct _tuple12 _tmp6A;struct Cyc_Absyn_Exp*_tmp6B;struct Cyc_Absyn_Exp*
_tmp6C;struct _tuple12*_tmp69=es;_tmp6A=*_tmp69;_tmp6B=_tmp6A.f1;_tmp6C=_tmp6A.f2;
return Cyc_Absyn_tuple_exp(({struct Cyc_Absyn_Exp*_tmp6D[2];_tmp6D[1]=_tmp6C;
_tmp6D[0]=_tmp6B;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp6D,sizeof(struct Cyc_Absyn_Exp*),2));}),loc);}struct _tuple13{
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};static
struct Cyc_Absyn_Exp*Cyc_Tcgenrep_tuple3_exp_cls(struct Cyc_Position_Segment*loc,
struct _tuple13*es){struct _tuple13 _tmp6F;struct Cyc_Absyn_Exp*_tmp70;struct Cyc_Absyn_Exp*
_tmp71;struct Cyc_Absyn_Exp*_tmp72;struct _tuple13*_tmp6E=es;_tmp6F=*_tmp6E;_tmp70=
_tmp6F.f1;_tmp71=_tmp6F.f2;_tmp72=_tmp6F.f3;return Cyc_Absyn_tuple_exp(({struct
Cyc_Absyn_Exp*_tmp73[3];_tmp73[2]=_tmp72;_tmp73[1]=_tmp71;_tmp73[0]=_tmp70;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp73,sizeof(struct Cyc_Absyn_Exp*),3));}),loc);}static char _tmp74[5]="NULL";
static struct _dynforward_ptr Cyc_Tcgenrep_null_str={_tmp74,_tmp74 + 5};static struct
Cyc_Absyn_Exp*Cyc_Tcgenrep_list_exp(struct Cyc_List_List*l,struct Cyc_Position_Segment*
loc){if(l == 0)return Cyc_Absyn_null_exp(loc);return Cyc_Tcgenrep_call_exp(Cyc_Tcgenrep_toplevel_name(&
Cyc_Tcgenrep_list_str),l,loc);}struct _tuple14{void*f1;struct Cyc_Position_Segment*
f2;};static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_make_offsetof_exp(struct _tuple14*
typeloc,int index){struct _tuple14 _tmp76;void*_tmp77;struct Cyc_Position_Segment*
_tmp78;struct _tuple14*_tmp75=typeloc;_tmp76=*_tmp75;_tmp77=_tmp76.f1;_tmp78=
_tmp76.f2;return Cyc_Absyn_offsetof_exp(_tmp77,(void*)({struct Cyc_Absyn_TupleIndex_struct*
_tmp79=_cycalloc_atomic(sizeof(*_tmp79));_tmp79[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp7A;_tmp7A.tag=1;_tmp7A.f1=(unsigned int)index;_tmp7A;});_tmp79;}),_tmp78);}
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_get_and_cast_ri_exp(struct Cyc_Position_Segment*
loc,struct Cyc_Tcgenrep_RepInfo*info){return Cyc_Absyn_cast_exp(Cyc_Tcgenrep_tunion_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str)),(struct Cyc_Absyn_Exp*)
_check_null(info->exp),1,(void*)0,loc);}static struct _tuple11*Cyc_Tcgenrep_gen_id_for_exp(
struct Cyc_Absyn_Exp*e){return({struct _tuple11*_tmp7B=_cycalloc(sizeof(*_tmp7B));
_tmp7B->f1=Cyc_Tcgenrep_new_gen_id(({const char*_tmp7C="tuple";_tag_dynforward(
_tmp7C,sizeof(char),_get_zero_arr_size(_tmp7C,6));}));_tmp7B->f2=e;_tmp7B;});}
static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_decl_cls(struct _tuple14*env,struct
_tuple11*name_exp){struct _tuple14 _tmp7E;void*_tmp7F;struct Cyc_Position_Segment*
_tmp80;struct _tuple14*_tmp7D=env;_tmp7E=*_tmp7D;_tmp7F=_tmp7E.f1;_tmp80=_tmp7E.f2;{
struct _tuple11 _tmp82;struct _dynforward_ptr*_tmp83;struct Cyc_Absyn_Exp*_tmp84;
struct _tuple11*_tmp81=name_exp;_tmp82=*_tmp81;_tmp83=_tmp82.f1;_tmp84=_tmp82.f2;
return Cyc_Tcgenrep_gen_decl(_tmp83,_tmp7F,(struct Cyc_Absyn_Exp*)_tmp84,_tmp80);}}
struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct
_tuple15*Cyc_Tcgenrep_null_designator_exp(struct Cyc_Absyn_Exp*e){return({struct
_tuple15*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->f1=0;_tmp85->f2=e;_tmp85;});}
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_arr_init_exp(struct Cyc_List_List*l,struct
Cyc_Position_Segment*loc){struct Cyc_List_List*_tmp86=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_null_designator_exp,l);return Cyc_Absyn_unresolvedmem_exp(0,_tmp86,
loc);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_address_exp_cls(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_address_exp(e,loc);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_unknownid_exp_cls(struct Cyc_Position_Segment*loc,struct _tuple6*e){
return Cyc_Absyn_unknownid_exp(e,loc);}static int Cyc_Tcgenrep_has_bitfield(struct
Cyc_Absyn_Aggrfield*sf){return sf->width != 0;}static int Cyc_Tcgenrep_add_bitfield_sizes(
int total,struct Cyc_Absyn_Aggrfield*sf){unsigned int _tmp88;int _tmp89;struct
_tuple9 _tmp87=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(sf->width));
_tmp88=_tmp87.f1;_tmp89=_tmp87.f2;if(!_tmp89)({void*_tmp8A[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp8B="add_bitfield_sizes: sizeof or offsetof in bitfield size";
_tag_dynforward(_tmp8B,sizeof(char),_get_zero_arr_size(_tmp8B,56));}),
_tag_dynforward(_tmp8A,sizeof(void*),0));});return(int)(_tmp88 + total);}static
void*Cyc_Tcgenrep_select_structfield_type(struct Cyc_Absyn_Aggrfield*sf){if(Cyc_strcmp((
struct _dynforward_ptr)*sf->name,({const char*_tmp8C="";_tag_dynforward(_tmp8C,
sizeof(char),_get_zero_arr_size(_tmp8C,1));}))== 0)({void*_tmp8D[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp8E="gen(): anonymous (padding) structfield not handled yet";_tag_dynforward(
_tmp8E,sizeof(char),_get_zero_arr_size(_tmp8E,55));}),_tag_dynforward(_tmp8D,
sizeof(void*),0));});if(Cyc_Tcgenrep_has_bitfield(sf))({void*_tmp8F[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp90="gen(): mixed struct bitfields and union bitfields not handled";
_tag_dynforward(_tmp90,sizeof(char),_get_zero_arr_size(_tmp90,62));}),
_tag_dynforward(_tmp8F,sizeof(void*),0));});return(void*)sf->type;}struct
_tuple16{struct _dynforward_ptr*f1;void*f2;};static struct _tuple16*Cyc_Tcgenrep_select_structfield_nmtype(
struct Cyc_Absyn_Aggrfield*sf){if(Cyc_strcmp((struct _dynforward_ptr)*sf->name,({
const char*_tmp91="";_tag_dynforward(_tmp91,sizeof(char),_get_zero_arr_size(
_tmp91,1));}))== 0)({void*_tmp92[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp93="gen(): anonymous (padding) structfield not handled yet";
_tag_dynforward(_tmp93,sizeof(char),_get_zero_arr_size(_tmp93,55));}),
_tag_dynforward(_tmp92,sizeof(void*),0));});if(Cyc_Tcgenrep_has_bitfield(sf))({
void*_tmp94[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp95="gen(): mixed struct bitfields and union bitfields not handled";
_tag_dynforward(_tmp95,sizeof(char),_get_zero_arr_size(_tmp95,62));}),
_tag_dynforward(_tmp94,sizeof(void*),0));});return({struct _tuple16*_tmp96=
_cycalloc(sizeof(*_tmp96));_tmp96->f1=sf->name;_tmp96->f2=(void*)sf->type;_tmp96;});}
struct _tuple17{int f1;struct _tuple6*f2;};struct _tuple17*Cyc_Tcgenrep_select_enumfield_tagnm(
struct Cyc_Absyn_Enumfield*ef){if(ef->tag == 0)({void*_tmp97[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp98="Enum tag exp should be filled in by now";
_tag_dynforward(_tmp98,sizeof(char),_get_zero_arr_size(_tmp98,40));}),
_tag_dynforward(_tmp97,sizeof(void*),0));});return({struct _tuple17*_tmp99=
_cycalloc(sizeof(*_tmp99));_tmp99->f1=(int)(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(ef->tag))).f1;_tmp99->f2=ef->name;_tmp99;});}
static struct Cyc_Dict_Dict*Cyc_Tcgenrep_update_info(struct Cyc_Dict_Dict*dict,void*
type,struct Cyc_List_List*decls,struct Cyc_Absyn_Exp*exp,struct Cyc_List_List*
dependencies,struct Cyc_Core_Opt*fwd_decl){struct Cyc_Core_Opt*_tmp9A=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,void*k))Cyc_Dict_lookup_opt)(dict,type);if(_tmp9A != 0){if(((
struct Cyc_Tcgenrep_RepInfo*)_tmp9A->v)->decls != 0){Cyc_Tcgenrep_print_typerep_dict(
dict);({void*_tmp9B[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9C="Updating non-forward declaration";
_tag_dynforward(_tmp9C,sizeof(char),_get_zero_arr_size(_tmp9C,33));}),
_tag_dynforward(_tmp9B,sizeof(void*),0));});}((struct Cyc_Tcgenrep_RepInfo*)
_tmp9A->v)->decls=decls;((struct Cyc_Tcgenrep_RepInfo*)_tmp9A->v)->exp=exp;((
struct Cyc_Tcgenrep_RepInfo*)_tmp9A->v)->dependencies=dependencies;return dict;}
else{return((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*
v))Cyc_Dict_insert)(dict,type,({struct Cyc_Tcgenrep_RepInfo*_tmp9D=_cycalloc(
sizeof(*_tmp9D));_tmp9D->decls=decls;_tmp9D->exp=exp;_tmp9D->dependencies=
dependencies;_tmp9D->fwd_decl=fwd_decl;_tmp9D->emitted=0;_tmp9D->is_extern=0;
_tmp9D;}));}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_make_fwd_decl_info(struct Cyc_Dict_Dict*
dict,void*type,struct Cyc_Absyn_Exp*exp,struct Cyc_Core_Opt*fwd_decl,int is_extern){
struct Cyc_Core_Opt*_tmp9E=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,void*k))
Cyc_Dict_lookup_opt)(dict,type);if(_tmp9E != 0){({struct Cyc_String_pa_struct
_tmpA1;_tmpA1.tag=0;_tmpA1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
type));{void*_tmp9F[1]={& _tmpA1};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA0="Repinfo for fwd declaration of %s already exists";
_tag_dynforward(_tmpA0,sizeof(char),_get_zero_arr_size(_tmpA0,49));}),
_tag_dynforward(_tmp9F,sizeof(void*),1));}});return dict;}else{return((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(dict,
type,({struct Cyc_Tcgenrep_RepInfo*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->decls=
0;_tmpA2->exp=exp;_tmpA2->dependencies=0;_tmpA2->fwd_decl=fwd_decl;_tmpA2->emitted=
0;_tmpA2->is_extern=is_extern;_tmpA2;}));}}static struct Cyc_Absyn_Tqual Cyc_Tcgenrep_tq_none={
0,0,0,0,0};static struct _tuple10*Cyc_Tcgenrep_tqual_type(struct Cyc_Absyn_Tqual*tq,
void*type){return({struct _tuple10*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->f1=*((
struct Cyc_Absyn_Tqual*)_check_null(tq));_tmpA3->f2=type;_tmpA3;});}static void*
Cyc_Tcgenrep_tuple_typ(struct Cyc_List_List*types){struct Cyc_List_List*_tmpA4=((
struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_Absyn_Tqual*,void*),struct
Cyc_Absyn_Tqual*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tqual_type,(
struct Cyc_Absyn_Tqual*)& Cyc_Tcgenrep_tq_none,types);struct Cyc_Absyn_TupleType_struct*
tuple_type_base=({struct Cyc_Absyn_TupleType_struct*_tmpA5=_cycalloc(sizeof(*
_tmpA5));_tmpA5[0]=({struct Cyc_Absyn_TupleType_struct _tmpA6;_tmpA6.tag=9;_tmpA6.f1=
_tmpA4;_tmpA6;});_tmpA5;});void*tuple_type=(void*)tuple_type_base;return
tuple_type;}static struct _tuple15*Cyc_Tcgenrep_array_decls(void*type,struct Cyc_List_List*
exps,struct Cyc_Position_Segment*loc){struct Cyc_List_List*_tmpA7=((struct Cyc_List_List*(*)(
struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_gen_id_for_exp,exps);struct Cyc_List_List*_tmpA8=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct _tuple14*,struct _tuple11*),struct _tuple14*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_gen_decl_cls,({struct
_tuple14*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->f1=type;_tmpB6->f2=loc;_tmpB6;}),
_tmpA7);struct Cyc_List_List*_tmpA9=((struct Cyc_List_List*(*)(struct
_dynforward_ptr*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_first,
_tmpA7);struct Cyc_List_List*_tmpAA=((struct Cyc_List_List*(*)(struct _tuple6*(*f)(
struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_toplevel_name,
_tmpA9);struct Cyc_List_List*_tmpAB=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_unknownid_exp_cls,loc,_tmpAA);
struct Cyc_List_List*_tmpAC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct Cyc_Absyn_Exp*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_address_exp_cls,loc,_tmpAB);
struct Cyc_Absyn_Exp*_tmpAD=Cyc_Tcgenrep_arr_init_exp(_tmpAC,loc);struct
_dynforward_ptr*_tmpAE=Cyc_Tcgenrep_new_gen_id(({const char*_tmpB5="arr";
_tag_dynforward(_tmpB5,sizeof(char),_get_zero_arr_size(_tmpB5,4));}));void*
_tmpAF=Cyc_Absyn_at_typ(type,(void*)2,Cyc_Tcgenrep_tq_none,Cyc_Absyn_false_conref);
void*_tmpB0=Cyc_Absyn_array_typ(_tmpAF,Cyc_Tcgenrep_tq_none,0,Cyc_Absyn_false_conref,
0);struct Cyc_Absyn_Decl*_tmpB1=Cyc_Tcgenrep_gen_decl(_tmpAE,_tmpB0,(struct Cyc_Absyn_Exp*)
_tmpAD,loc);struct Cyc_Absyn_Exp*_tmpB2=Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(
_tmpAE),loc);return({struct _tuple15*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmpA8,({struct Cyc_Absyn_Decl*_tmpB4[1];_tmpB4[0]=_tmpB1;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpB4,sizeof(struct Cyc_Absyn_Decl*),
1));}));_tmpB3->f2=_tmpB2;_tmpB3;});}static void*Cyc_Tcgenrep_check_tunionfield_and_get_type(
struct Cyc_Absyn_Tunionfield*tuf){return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Absyn_TupleType_struct
_tmpB8;_tmpB8.tag=9;_tmpB8.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(({struct _tuple10*_tmpB9[1];_tmpB9[0]=({
struct _tuple10*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->f1=Cyc_Tcgenrep_tq_none;
_tmpBA->f2=Cyc_Absyn_uint_typ;_tmpBA;});((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpB9,sizeof(struct _tuple10*),1));}),
tuf->typs);_tmpB8;});_tmpB7;});}static struct _tuple6*Cyc_Tcgenrep_check_tunionfield_and_get_name(
struct Cyc_Absyn_Tunionfield*tuf){return tuf->name;}struct _tuple18{struct _tuple6*
f1;void*f2;};static struct _tuple18*Cyc_Tcgenrep_check_tunionfield_and_get_nmtype(
struct Cyc_Absyn_Tunionfield*tuf){return({struct _tuple18*_tmpBB=_cycalloc(sizeof(*
_tmpBB));_tmpBB->f1=tuf->name;_tmpBB->f2=(void*)({struct Cyc_Absyn_TupleType_struct*
_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Absyn_TupleType_struct
_tmpBD;_tmpBD.tag=9;_tmpBD.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(({struct _tuple10*_tmpBE[1];_tmpBE[0]=({
struct _tuple10*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->f1=Cyc_Tcgenrep_tq_none;
_tmpBF->f2=Cyc_Absyn_uint_typ;_tmpBF;});((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpBE,sizeof(struct _tuple10*),1));}),
tuf->typs);_tmpBD;});_tmpBC;});_tmpBB;});}static struct _tuple16*Cyc_Tcgenrep_check_xtunionfield_and_get_name_type(
struct Cyc_Absyn_Tunionfield*tuf){return({struct _tuple16*_tmpC0=_cycalloc(sizeof(*
_tmpC0));_tmpC0->f1=(*tuf->name).f2;_tmpC0->f2=(void*)({struct Cyc_Absyn_TupleType_struct*
_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1[0]=({struct Cyc_Absyn_TupleType_struct
_tmpC2;_tmpC2.tag=9;_tmpC2.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(({struct _tuple10*_tmpC3[1];_tmpC3[0]=({
struct _tuple10*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4->f1=Cyc_Tcgenrep_tq_none;
_tmpC4->f2=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Tcgenrep_tq_none,
Cyc_Absyn_true_conref);_tmpC4;});((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmpC3,sizeof(struct _tuple10*),1));}),tuf->typs);
_tmpC2;});_tmpC1;});_tmpC0;});}static int Cyc_Tcgenrep_filter_empty_tunionfield(
struct Cyc_Absyn_Tunionfield*tuf){return tuf->typs != 0;}static int Cyc_Tcgenrep_filter_nonempty_tunionfield(
struct Cyc_Absyn_Tunionfield*tuf){return tuf->typs == 0;}static struct
_dynforward_ptr*Cyc_Tcgenrep_get_tunionfield_name(struct Cyc_Absyn_Tunionfield*
tuf){return(*tuf->name).f2;}struct Cyc_Absyn_Aggrfield*Cyc_Tcgenrep_substitute_structfield_type(
struct Cyc_List_List*subst,struct Cyc_Absyn_Aggrfield*sf){return({struct Cyc_Absyn_Aggrfield*
_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->name=sf->name;_tmpC5->tq=sf->tq;_tmpC5->type=(
void*)Cyc_Tcutil_substitute(subst,(void*)sf->type);_tmpC5->width=sf->width;
_tmpC5->attributes=sf->attributes;_tmpC5;});}struct _tuple10*Cyc_Tcgenrep_substitute_tqual_type(
struct Cyc_List_List*subst,struct _tuple10*pair){struct _tuple10 _tmpC7;struct Cyc_Absyn_Tqual
_tmpC8;void*_tmpC9;struct _tuple10*_tmpC6=pair;_tmpC7=*_tmpC6;_tmpC8=_tmpC7.f1;
_tmpC9=_tmpC7.f2;return({struct _tuple10*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->f1=
_tmpC8;_tmpCA->f2=Cyc_Tcutil_substitute(subst,_tmpC9);_tmpCA;});}struct Cyc_Absyn_Tunionfield*
Cyc_Tcgenrep_substitute_tunionfield_type(struct Cyc_List_List*subst,struct Cyc_Absyn_Tunionfield*
tf){return({struct Cyc_Absyn_Tunionfield*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->name=
tf->name;_tmpCB->typs=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_List_List*,
struct _tuple10*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_substitute_tqual_type,subst,tf->typs);_tmpCB->loc=tf->loc;_tmpCB->sc=(
void*)((void*)tf->sc);_tmpCB;});}void*Cyc_Tcgenrep_monomorphize_type(void*type){
void*_tmpCC=Cyc_Tcutil_compress(type);struct Cyc_Absyn_AggrInfo _tmpCD;void*_tmpCE;
struct Cyc_List_List*_tmpCF;struct Cyc_Absyn_TunionInfo _tmpD0;void*_tmpD1;struct
Cyc_Absyn_Tuniondecl**_tmpD2;struct Cyc_Absyn_Tuniondecl*_tmpD3;struct Cyc_List_List*
_tmpD4;struct Cyc_Core_Opt*_tmpD5;_LL11: if(_tmpCC <= (void*)4)goto _LL15;if(*((int*)
_tmpCC)!= 10)goto _LL13;_tmpCD=((struct Cyc_Absyn_AggrType_struct*)_tmpCC)->f1;
_tmpCE=(void*)_tmpCD.aggr_info;_tmpCF=_tmpCD.targs;_LL12: {struct Cyc_Absyn_Aggrdecl*
_tmpD6=Cyc_Absyn_get_known_aggrdecl(_tmpCE);struct Cyc_List_List*_tmpD7=_tmpD6->tvs;
if(Cyc_List_length(_tmpCF)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpD7))({struct Cyc_Int_pa_struct _tmpDB;_tmpDB.tag=1;_tmpDB.f1=(unsigned long)((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpD7);{struct Cyc_Int_pa_struct
_tmpDA;_tmpDA.tag=1;_tmpDA.f1=(unsigned long)Cyc_List_length(_tmpCF);{void*
_tmpD8[2]={& _tmpDA,& _tmpDB};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpD9="gen(): number of params %d differs from number of tyvars %d";
_tag_dynforward(_tmpD9,sizeof(char),_get_zero_arr_size(_tmpD9,60));}),
_tag_dynforward(_tmpD8,sizeof(void*),2));}}});{struct Cyc_List_List*_tmpDC=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmpD7,_tmpCF);struct Cyc_List_List*fields=0;if(_tmpD6->impl != 0)fields=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_substitute_structfield_type,
_tmpDC,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD6->impl))->fields);{
struct Cyc_Absyn_AggrdeclImpl*_tmpDD=_tmpD6->impl == 0?0:({struct Cyc_Absyn_AggrdeclImpl*
_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->exist_vars=0;_tmpE5->rgn_po=0;_tmpE5->fields=
fields;_tmpE5;});struct Cyc_Absyn_Aggrdecl*ad2=({struct Cyc_Absyn_Aggrdecl*_tmpE4=
_cycalloc(sizeof(*_tmpE4));_tmpE4->kind=(void*)((void*)_tmpD6->kind);_tmpE4->sc=(
void*)((void*)_tmpD6->sc);_tmpE4->name=_tmpD6->name;_tmpE4->tvs=0;_tmpE4->impl=
_tmpDD;_tmpE4->attributes=_tmpD6->attributes;_tmpE4;});return(void*)({struct Cyc_Absyn_AggrType_struct*
_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE[0]=({struct Cyc_Absyn_AggrType_struct
_tmpDF;_tmpDF.tag=10;_tmpDF.f1=({struct Cyc_Absyn_AggrInfo _tmpE0;_tmpE0.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpE1=_cycalloc(sizeof(*_tmpE1));
_tmpE1[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpE2;_tmpE2.tag=1;_tmpE2.f1=({
struct Cyc_Absyn_Aggrdecl**_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3[0]=ad2;_tmpE3;});
_tmpE2;});_tmpE1;}));_tmpE0.targs=0;_tmpE0;});_tmpDF;});_tmpDE;});}}}_LL13: if(*((
int*)_tmpCC)!= 2)goto _LL15;_tmpD0=((struct Cyc_Absyn_TunionType_struct*)_tmpCC)->f1;
_tmpD1=(void*)_tmpD0.tunion_info;if(*((int*)_tmpD1)!= 1)goto _LL15;_tmpD2=((
struct Cyc_Absyn_KnownTunion_struct*)_tmpD1)->f1;_tmpD3=*_tmpD2;_tmpD4=_tmpD0.targs;
_tmpD5=_tmpD0.rgn;_LL14: {struct Cyc_List_List*_tmpE6=_tmpD3->tvs;if(Cyc_List_length(
_tmpD4)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpE6))({struct Cyc_Int_pa_struct
_tmpEA;_tmpEA.tag=1;_tmpEA.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpE6);{struct Cyc_Int_pa_struct _tmpE9;_tmpE9.tag=1;_tmpE9.f1=(unsigned long)Cyc_List_length(
_tmpD4);{void*_tmpE7[2]={& _tmpE9,& _tmpEA};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpE8="gen(): number of params %d differs from number of tyvars %d";
_tag_dynforward(_tmpE8,sizeof(char),_get_zero_arr_size(_tmpE8,60));}),
_tag_dynforward(_tmpE7,sizeof(void*),2));}}});{struct Cyc_List_List*_tmpEB=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmpE6,_tmpD4);struct Cyc_Core_Opt*fields=0;if(_tmpD3->fields != 0)fields=({struct
Cyc_Core_Opt*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->v=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tunionfield*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_substitute_tunionfield_type,
_tmpEB,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpD3->fields))->v);
_tmpEC;});{struct Cyc_Absyn_Tuniondecl*_tmpED=({struct Cyc_Absyn_Tuniondecl*_tmpF6=
_cycalloc(sizeof(*_tmpF6));_tmpF6->sc=(void*)((void*)_tmpD3->sc);_tmpF6->name=
_tmpD3->name;_tmpF6->tvs=0;_tmpF6->fields=fields;_tmpF6->is_xtunion=_tmpD3->is_xtunion;
_tmpF6->is_flat=_tmpD3->is_flat;_tmpF6;});struct Cyc_Absyn_TunionType_struct*
_tmpEE=({struct Cyc_Absyn_TunionType_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));
_tmpF0[0]=({struct Cyc_Absyn_TunionType_struct _tmpF1;_tmpF1.tag=2;_tmpF1.f1=({
struct Cyc_Absyn_TunionInfo _tmpF2;_tmpF2.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmpF4;_tmpF4.tag=1;_tmpF4.f1=({struct Cyc_Absyn_Tuniondecl**_tmpF5=_cycalloc(
sizeof(*_tmpF5));_tmpF5[0]=_tmpED;_tmpF5;});_tmpF4;});_tmpF3;}));_tmpF2.targs=
_tmpD4;_tmpF2.rgn=_tmpD5;_tmpF2;});_tmpF1;});_tmpF0;});struct Cyc_Position_Segment*
_tmpEF=Cyc_Position_segment_of_abs(0,0);return(void*)_tmpEE;}}}_LL15:;_LL16:
return type;_LL10:;}struct _dynforward_ptr Cyc_Tcgenrep_make_type_cstring(void*t){
struct _dynforward_ptr s=Cyc_strdup((struct _dynforward_ptr)Cyc_Absynpp_typ2cstring(
t));{int i=0;for(0;i < Cyc_strlen((struct _dynforward_ptr)s);i ++){if(*((char*)
_check_dynforward_subscript(s,sizeof(char),i))== ' ')({struct _dynforward_ptr
_tmpF7=_dynforward_ptr_plus(s,sizeof(char),i);char _tmpF8=*((char*)
_check_dynforward_subscript(_tmpF7,sizeof(char),0));char _tmpF9='_';if(
_get_dynforward_size(_tmpF7,sizeof(char))== 1  && (_tmpF8 == '\000'  && _tmpF9 != '\000'))
_throw_arraybounds();*((char*)_tmpF7.curr)=_tmpF9;});else{if(!isalnum((int)*((
char*)_check_dynforward_subscript(s,sizeof(char),i))) && *((char*)
_check_dynforward_subscript(s,sizeof(char),i))!= '_')({struct _dynforward_ptr
_tmpFA=_dynforward_ptr_plus(s,sizeof(char),i);char _tmpFB=*((char*)
_check_dynforward_subscript(_tmpFA,sizeof(char),0));char _tmpFC=(char)('0' + *((
char*)_check_dynforward_subscript(s,sizeof(char),i))% 10);if(
_get_dynforward_size(_tmpFA,sizeof(char))== 1  && (_tmpFB == '\000'  && _tmpFC != '\000'))
_throw_arraybounds();*((char*)_tmpFA.curr)=_tmpFC;});}}}return(struct
_dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)s,({const char*_tmpFD="_rep";
_tag_dynforward(_tmpFD,sizeof(char),_get_zero_arr_size(_tmpFD,5));}));}struct
_tuple19{struct Cyc_Dict_Dict*f1;struct Cyc_Tcgenrep_RepInfo*f2;};static struct
_tuple19*Cyc_Tcgenrep_lookupRep(struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,
struct Cyc_Position_Segment*loc,void*type);struct _tuple20{struct Cyc_Dict_Dict*f1;
struct Cyc_List_List*f2;};struct _tuple21{struct Cyc_Tcenv_Tenv*f1;struct Cyc_Position_Segment*
f2;};static struct _tuple20*Cyc_Tcgenrep_lookupRepsCls(struct _tuple21*env,void*
type,struct _tuple20*carry){struct _tuple19 _tmpFF;struct Cyc_Dict_Dict*_tmp100;
struct Cyc_Tcgenrep_RepInfo*_tmp101;struct _tuple19*_tmpFE=Cyc_Tcgenrep_lookupRep((*
env).f1,(*carry).f1,(*env).f2,type);_tmpFF=*_tmpFE;_tmp100=_tmpFF.f1;_tmp101=
_tmpFF.f2;return({struct _tuple20*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102->f1=
_tmp100;_tmp102->f2=({struct Cyc_List_List*_tmp103=_cycalloc(sizeof(*_tmp103));
_tmp103->hd=_tmp101;_tmp103->tl=(*carry).f2;_tmp103;});_tmp102;});}static struct
Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*
dict,struct Cyc_Position_Segment*loc,struct _tuple6*varname,void*sc,void*type,
struct Cyc_List_List*types){struct Cyc_Absyn_Exp*_tmp104=Cyc_Absyn_sizeoftyp_exp(
type,loc);struct _tuple21*_tmp105=({struct _tuple21*_tmp11A=_cycalloc(sizeof(*
_tmp11A));_tmp11A->f1=te;_tmp11A->f2=loc;_tmp11A;});struct _tuple20*_tmp106=({
struct _tuple20*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->f1=dict;_tmp119->f2=
0;_tmp119;});struct _tuple20 _tmp108;struct Cyc_Dict_Dict*_tmp109;struct Cyc_List_List*
_tmp10A;struct _tuple20*_tmp107=((struct _tuple20*(*)(struct _tuple20*(*f)(struct
_tuple21*,void*,struct _tuple20*),struct _tuple21*,struct Cyc_List_List*x,struct
_tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,_tmp105,types,
_tmp106);_tmp108=*_tmp107;_tmp109=_tmp108.f1;_tmp10A=_tmp108.f2;dict=_tmp109;{
struct Cyc_List_List*_tmp10B=((struct Cyc_List_List*(*)(int n,int(*f)(int)))Cyc_List_tabulate)(
Cyc_List_length(types),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*
_tmp10C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple14*,int),
struct _tuple14*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_make_offsetof_exp,({
struct _tuple14*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->f1=type;_tmp118->f2=
loc;_tmp118;}),_tmp10B);struct Cyc_List_List*_tmp10D=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,
loc,_tmp10A);struct Cyc_List_List*_tmp10E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple12*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp10C,_tmp10D));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*_tmp117[2];
_tmp117[1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp117[0]=Cyc_Absyn_uint_typ;Cyc_List_list(_tag_dynforward(_tmp117,sizeof(void*),
2));}));struct _tuple15 _tmp110;struct Cyc_List_List*_tmp111;struct Cyc_Absyn_Exp*
_tmp112;struct _tuple15*_tmp10F=Cyc_Tcgenrep_array_decls(tuple_type,_tmp10E,loc);
_tmp110=*_tmp10F;_tmp111=_tmp110.f1;_tmp112=_tmp110.f2;{struct Cyc_Absyn_Decl*
_tmp113=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tuple_str),varname,({struct Cyc_Absyn_Exp*
_tmp116[2];_tmp116[1]=_tmp112;_tmp116[0]=_tmp104;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp116,sizeof(struct Cyc_Absyn_Exp*),
2));}),sc,loc);struct Cyc_List_List*_tmp114=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp111,({struct Cyc_Absyn_Decl*
_tmp115[1];_tmp115[0]=_tmp113;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp115,sizeof(struct Cyc_Absyn_Decl*),1));}));
return Cyc_Tcgenrep_update_info(dict,type,_tmp114,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp10A,0);}}}static struct Cyc_Dict_Dict*
Cyc_Tcgenrep_buildRepTunionfield(struct _tuple6*tname,struct _tuple6*fname,struct
Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,struct
_tuple6*varname,void*sc,void*type,struct Cyc_List_List*types){struct Cyc_Absyn_Exp*
_tmp11B=Cyc_Absyn_sizeoftyp_exp(type,loc);struct _tuple21*_tmp11C=({struct
_tuple21*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->f1=te;_tmp133->f2=loc;
_tmp133;});struct _tuple20*_tmp11D=({struct _tuple20*_tmp132=_cycalloc(sizeof(*
_tmp132));_tmp132->f1=dict;_tmp132->f2=0;_tmp132;});struct _tuple20 _tmp11F;struct
Cyc_Dict_Dict*_tmp120;struct Cyc_List_List*_tmp121;struct _tuple20*_tmp11E=((
struct _tuple20*(*)(struct _tuple20*(*f)(struct _tuple21*,void*,struct _tuple20*),
struct _tuple21*,struct Cyc_List_List*x,struct _tuple20*accum))Cyc_List_fold_right_c)(
Cyc_Tcgenrep_lookupRepsCls,_tmp11C,types,_tmp11D);_tmp11F=*_tmp11E;_tmp120=
_tmp11F.f1;_tmp121=_tmp11F.f2;dict=_tmp120;{struct Cyc_List_List*_tmp122=((struct
Cyc_List_List*(*)(int n,int(*f)(int)))Cyc_List_tabulate)(Cyc_List_length(types),(
int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmp123=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple14*,int),struct _tuple14*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_make_offsetof_exp,({struct _tuple14*_tmp131=
_cycalloc(sizeof(*_tmp131));_tmp131->f1=type;_tmp131->f2=loc;_tmp131;}),_tmp122);
struct Cyc_List_List*_tmp124=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp121);struct Cyc_List_List*_tmp125=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple12*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp123,_tmp124));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*_tmp130[2];
_tmp130[1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp130[0]=Cyc_Absyn_uint_typ;Cyc_List_list(_tag_dynforward(_tmp130,sizeof(void*),
2));}));struct Cyc_Absyn_Exp*_tmp126=Cyc_Tcgenrep_cnst_string(*(*tname).f2,loc);
struct Cyc_Absyn_Exp*_tmp127=Cyc_Tcgenrep_cnst_string(*(*fname).f2,loc);struct
_tuple15 _tmp129;struct Cyc_List_List*_tmp12A;struct Cyc_Absyn_Exp*_tmp12B;struct
_tuple15*_tmp128=Cyc_Tcgenrep_array_decls(tuple_type,_tmp125,loc);_tmp129=*
_tmp128;_tmp12A=_tmp129.f1;_tmp12B=_tmp129.f2;{struct Cyc_Absyn_Decl*_tmp12C=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tunionfield_str),varname,({struct Cyc_Absyn_Exp*_tmp12F[4];_tmp12F[3]=
_tmp12B;_tmp12F[2]=_tmp11B;_tmp12F[1]=_tmp127;_tmp12F[0]=_tmp126;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp12F,sizeof(struct Cyc_Absyn_Exp*),
4));}),sc,loc);struct Cyc_List_List*_tmp12D=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp12A,({struct Cyc_Absyn_Decl*
_tmp12E[1];_tmp12E[0]=_tmp12C;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp12E,sizeof(struct Cyc_Absyn_Decl*),1));}));
return Cyc_Tcgenrep_update_info(dict,type,_tmp12D,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp121,0);}}}static struct Cyc_Dict_Dict*
Cyc_Tcgenrep_buildRepStruct(struct Cyc_Core_Opt*sname,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,struct _tuple6*varname,
void*sc,void*type,struct Cyc_List_List*nmtypes){struct Cyc_Absyn_Exp*_tmp134=Cyc_Absyn_sizeoftyp_exp(
type,loc);struct _tuple21*_tmp135=({struct _tuple21*_tmp152=_cycalloc(sizeof(*
_tmp152));_tmp152->f1=te;_tmp152->f2=loc;_tmp152;});struct _tuple20*_tmp136=({
struct _tuple20*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151->f1=dict;_tmp151->f2=
0;_tmp151;});struct Cyc_List_List*_tmp138;struct Cyc_List_List*_tmp139;struct
_tuple0 _tmp137=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(nmtypes);
_tmp138=_tmp137.f1;_tmp139=_tmp137.f2;{struct _tuple20 _tmp13B;struct Cyc_Dict_Dict*
_tmp13C;struct Cyc_List_List*_tmp13D;struct _tuple20*_tmp13A=((struct _tuple20*(*)(
struct _tuple20*(*f)(struct _tuple21*,void*,struct _tuple20*),struct _tuple21*,
struct Cyc_List_List*x,struct _tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp135,_tmp139,_tmp136);_tmp13B=*_tmp13A;_tmp13C=_tmp13B.f1;_tmp13D=_tmp13B.f2;
dict=_tmp13C;{struct Cyc_List_List*_tmp13E=((struct Cyc_List_List*(*)(int n,int(*f)(
int)))Cyc_List_tabulate)(Cyc_List_length(_tmp139),(int(*)(int))Cyc_Core_identity);
struct Cyc_List_List*_tmp13F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct _tuple14*,int),struct _tuple14*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_make_offsetof_exp,({struct _tuple14*_tmp150=_cycalloc(sizeof(*
_tmp150));_tmp150->f1=type;_tmp150->f2=loc;_tmp150;}),_tmp13E);struct Cyc_List_List*
_tmp140=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _dynforward_ptr*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))
Cyc_List_map_c)(Cyc_Tcgenrep_cnst_string_cls,loc,_tmp138);struct Cyc_List_List*
_tmp141=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,_tmp13D);struct Cyc_List_List*
_tmp142=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple13*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple3_exp_cls,loc,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y,struct Cyc_List_List*z))Cyc_List_zip3)(_tmp13F,_tmp140,
_tmp141));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*_tmp14F[3];_tmp14F[2]=
Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp14F[1]=Cyc_Absyn_const_string_typ((void*)2);_tmp14F[0]=Cyc_Absyn_uint_typ;
Cyc_List_list(_tag_dynforward(_tmp14F,sizeof(void*),3));}));struct _tuple15
_tmp144;struct Cyc_List_List*_tmp145;struct Cyc_Absyn_Exp*_tmp146;struct _tuple15*
_tmp143=Cyc_Tcgenrep_array_decls(tuple_type,_tmp142,loc);_tmp144=*_tmp143;
_tmp145=_tmp144.f1;_tmp146=_tmp144.f2;{struct Cyc_Absyn_Exp*name;if(sname == 0)
name=Cyc_Absyn_null_exp(loc);else{struct _dynforward_ptr*_tmp147=Cyc_Tcgenrep_new_gen_id(({
const char*_tmp14A="name";_tag_dynforward(_tmp14A,sizeof(char),_get_zero_arr_size(
_tmp14A,5));}));struct Cyc_Absyn_Decl*_tmp148=Cyc_Tcgenrep_gen_decl(_tmp147,Cyc_Absyn_const_string_typ((
void*)2),(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*(*((struct _tuple6*)
sname->v)).f2,loc),loc);_tmp145=({struct Cyc_List_List*_tmp149=_cycalloc(sizeof(*
_tmp149));_tmp149->hd=_tmp148;_tmp149->tl=_tmp145;_tmp149;});name=Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(_tmp147),loc),loc);}{struct
Cyc_Absyn_Decl*_tmp14B=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_struct_str),
varname,({struct Cyc_Absyn_Exp*_tmp14E[3];_tmp14E[2]=_tmp146;_tmp14E[1]=_tmp134;
_tmp14E[0]=name;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp14E,sizeof(struct Cyc_Absyn_Exp*),3));}),sc,loc);struct Cyc_List_List*
_tmp14C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp145,({struct Cyc_Absyn_Decl*_tmp14D[1];_tmp14D[0]=
_tmp14B;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp14D,sizeof(struct Cyc_Absyn_Decl*),1));}));return Cyc_Tcgenrep_update_info(
dict,type,_tmp14C,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp13D,0);}}}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepUnion(
struct Cyc_Core_Opt*uname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct
Cyc_Position_Segment*loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*
nmtypes){struct Cyc_Absyn_Exp*_tmp153=Cyc_Absyn_sizeoftyp_exp(type,loc);struct
_tuple21*_tmp154=({struct _tuple21*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E->f1=
te;_tmp16E->f2=loc;_tmp16E;});struct _tuple20*_tmp155=({struct _tuple20*_tmp16D=
_cycalloc(sizeof(*_tmp16D));_tmp16D->f1=dict;_tmp16D->f2=0;_tmp16D;});struct Cyc_List_List*
_tmp157;struct Cyc_List_List*_tmp158;struct _tuple0 _tmp156=((struct _tuple0(*)(
struct Cyc_List_List*x))Cyc_List_split)(nmtypes);_tmp157=_tmp156.f1;_tmp158=
_tmp156.f2;{struct _tuple20 _tmp15A;struct Cyc_Dict_Dict*_tmp15B;struct Cyc_List_List*
_tmp15C;struct _tuple20*_tmp159=((struct _tuple20*(*)(struct _tuple20*(*f)(struct
_tuple21*,void*,struct _tuple20*),struct _tuple21*,struct Cyc_List_List*x,struct
_tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,_tmp154,
_tmp158,_tmp155);_tmp15A=*_tmp159;_tmp15B=_tmp15A.f1;_tmp15C=_tmp15A.f2;dict=
_tmp15B;{struct Cyc_List_List*_tmp15D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _dynforward_ptr*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_string_cls,loc,
_tmp157);struct Cyc_List_List*_tmp15E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp15C);struct Cyc_List_List*_tmp15F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple12*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp15D,_tmp15E));void*_tmp160=Cyc_Tcgenrep_tuple_typ(({void*_tmp16C[2];_tmp16C[
1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp16C[0]=Cyc_Absyn_const_string_typ((void*)2);Cyc_List_list(_tag_dynforward(
_tmp16C,sizeof(void*),2));}));struct _tuple15 _tmp162;struct Cyc_List_List*_tmp163;
struct Cyc_Absyn_Exp*_tmp164;struct _tuple15*_tmp161=Cyc_Tcgenrep_array_decls(
_tmp160,_tmp15F,loc);_tmp162=*_tmp161;_tmp163=_tmp162.f1;_tmp164=_tmp162.f2;{
struct Cyc_Absyn_Exp*name;if(uname == 0)name=Cyc_Absyn_null_exp(loc);else{struct
_dynforward_ptr*_tmp165=Cyc_Tcgenrep_new_gen_id(({const char*_tmp168="name";
_tag_dynforward(_tmp168,sizeof(char),_get_zero_arr_size(_tmp168,5));}));struct
Cyc_Absyn_Decl*_tmp166=Cyc_Tcgenrep_gen_decl(_tmp165,Cyc_Absyn_const_string_typ((
void*)2),(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*(*((struct _tuple6*)
uname->v)).f2,loc),loc);_tmp163=({struct Cyc_List_List*_tmp167=_cycalloc(sizeof(*
_tmp167));_tmp167->hd=_tmp166;_tmp167->tl=_tmp163;_tmp167;});name=Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(_tmp165),loc),loc);}{struct
Cyc_Absyn_Decl*_tmp169=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_union_str),
varname,({struct Cyc_Absyn_Exp*_tmp16B[3];_tmp16B[2]=_tmp164;_tmp16B[1]=_tmp153;
_tmp16B[0]=name;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp16B,sizeof(struct Cyc_Absyn_Exp*),3));}),sc,loc);return Cyc_Tcgenrep_update_info(
dict,type,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp163,({struct Cyc_Absyn_Decl*_tmp16A[1];_tmp16A[0]=
_tmp169;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp16A,sizeof(struct Cyc_Absyn_Decl*),1));})),(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp15C,0);}}}}}
static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepEnum(struct _tuple6**ename,struct
Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,struct
_tuple6*varname,void*sc,void*type,struct Cyc_List_List*tagnms){struct Cyc_Absyn_Exp*
_tmp16F=Cyc_Absyn_sizeoftyp_exp(type,loc);struct Cyc_List_List*_tmp171;struct Cyc_List_List*
_tmp172;struct _tuple0 _tmp170=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
tagnms);_tmp171=_tmp170.f1;_tmp172=_tmp170.f2;{struct Cyc_List_List*_tmp173=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple6*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_cnst_qvar_string_cls,loc,_tmp172);struct Cyc_List_List*_tmp174=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,int),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,
loc,_tmp171);struct Cyc_List_List*_tmp175=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple12*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp174,_tmp173));void*_tmp176=Cyc_Tcgenrep_tuple_typ(({void*_tmp182[2];_tmp182[
1]=Cyc_Absyn_const_string_typ((void*)2);_tmp182[0]=Cyc_Absyn_uint_typ;Cyc_List_list(
_tag_dynforward(_tmp182,sizeof(void*),2));}));struct _tuple15 _tmp178;struct Cyc_List_List*
_tmp179;struct Cyc_Absyn_Exp*_tmp17A;struct _tuple15*_tmp177=Cyc_Tcgenrep_array_decls(
_tmp176,_tmp175,loc);_tmp178=*_tmp177;_tmp179=_tmp178.f1;_tmp17A=_tmp178.f2;{
struct Cyc_Absyn_Exp*name;if(ename == 0)name=Cyc_Absyn_null_exp(loc);else{struct
_dynforward_ptr*_tmp17B=Cyc_Tcgenrep_new_gen_id(({const char*_tmp17E="name";
_tag_dynforward(_tmp17E,sizeof(char),_get_zero_arr_size(_tmp17E,5));}));struct
Cyc_Absyn_Decl*_tmp17C=Cyc_Tcgenrep_gen_decl(_tmp17B,Cyc_Absyn_const_string_typ((
void*)2),(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*(*(*ename)).f2,loc),loc);
_tmp179=({struct Cyc_List_List*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D->hd=
_tmp17C;_tmp17D->tl=_tmp179;_tmp17D;});name=Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
Cyc_Tcgenrep_toplevel_name(_tmp17B),loc),loc);}{struct Cyc_Absyn_Decl*_tmp17F=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_enum_str),varname,({struct Cyc_Absyn_Exp*_tmp181[3];_tmp181[2]=
_tmp17A;_tmp181[1]=_tmp16F;_tmp181[0]=name;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp181,sizeof(struct Cyc_Absyn_Exp*),
3));}),sc,loc);return Cyc_Tcgenrep_update_info(dict,type,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp179,({
struct Cyc_Absyn_Decl*_tmp180[1];_tmp180[0]=_tmp17F;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp180,sizeof(struct Cyc_Absyn_Decl*),
1));})),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),0,0);}}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepTunion(
struct _tuple6*tname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*
loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*tonames,struct
Cyc_List_List*nmtypes){struct Cyc_List_List*_tmp183=((struct Cyc_List_List*(*)(int
n,int(*f)(int)))Cyc_List_tabulate)(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
tonames),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmp184=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp183);
struct Cyc_List_List*_tmp185=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_qvar_string_cls,loc,
tonames);struct Cyc_List_List*_tmp186=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp184,_tmp185);struct Cyc_List_List*
_tmp187=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple12*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,_tmp186);void*_tmp188=Cyc_Tcgenrep_tuple_typ(({
void*_tmp1A7[2];_tmp1A7[1]=Cyc_Absyn_const_string_typ((void*)2);_tmp1A7[0]=Cyc_Absyn_uint_typ;
Cyc_List_list(_tag_dynforward(_tmp1A7,sizeof(void*),2));}));struct _tuple15
_tmp18A;struct Cyc_List_List*_tmp18B;struct Cyc_Absyn_Exp*_tmp18C;struct _tuple15*
_tmp189=Cyc_Tcgenrep_array_decls(_tmp188,_tmp187,loc);_tmp18A=*_tmp189;_tmp18B=
_tmp18A.f1;_tmp18C=_tmp18A.f2;{struct _tuple21*_tmp18D=({struct _tuple21*_tmp1A6=
_cycalloc(sizeof(*_tmp1A6));_tmp1A6->f1=te;_tmp1A6->f2=loc;_tmp1A6;});struct
_tuple20*_tmp18E=({struct _tuple20*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));_tmp1A5->f1=
dict;_tmp1A5->f2=0;_tmp1A5;});struct Cyc_List_List*_tmp190;struct Cyc_List_List*
_tmp191;struct _tuple0 _tmp18F=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
nmtypes);_tmp190=_tmp18F.f1;_tmp191=_tmp18F.f2;{struct _tuple20 _tmp193;struct Cyc_Dict_Dict*
_tmp194;struct Cyc_List_List*_tmp195;struct _tuple20*_tmp192=((struct _tuple20*(*)(
struct _tuple20*(*f)(struct _tuple21*,void*,struct _tuple20*),struct _tuple21*,
struct Cyc_List_List*x,struct _tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp18D,_tmp191,_tmp18E);_tmp193=*_tmp192;_tmp194=_tmp193.f1;_tmp195=_tmp193.f2;
dict=_tmp194;{struct Cyc_List_List*_tmp196=((struct Cyc_List_List*(*)(int n,int(*f)(
int)))Cyc_List_tabulate)(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp195),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmp197=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp196);
struct Cyc_List_List*_tmp198=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_qvar_string_cls,loc,
_tmp190);struct Cyc_List_List*_tmp199=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp195);struct Cyc_List_List*_tmp19A=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y,struct Cyc_List_List*z))Cyc_List_zip3)(_tmp197,_tmp198,
_tmp199);struct Cyc_List_List*_tmp19B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple13*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple3_exp_cls,loc,_tmp19A);
void*tuple_type2=Cyc_Tcgenrep_tuple_typ(({void*_tmp1A4[3];_tmp1A4[2]=Cyc_Tcgenrep_tunion_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));_tmp1A4[1]=Cyc_Absyn_const_string_typ((
void*)2);_tmp1A4[0]=Cyc_Absyn_uint_typ;Cyc_List_list(_tag_dynforward(_tmp1A4,
sizeof(void*),3));}));struct _tuple15 _tmp19D;struct Cyc_List_List*_tmp19E;struct
Cyc_Absyn_Exp*_tmp19F;struct _tuple15*_tmp19C=Cyc_Tcgenrep_array_decls(
tuple_type2,_tmp19B,loc);_tmp19D=*_tmp19C;_tmp19E=_tmp19D.f1;_tmp19F=_tmp19D.f2;{
struct Cyc_Absyn_Exp*_tmp1A0=Cyc_Tcgenrep_cnst_string(*(*tname).f2,loc);struct Cyc_Absyn_Decl*
_tmp1A1=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tunion_str),varname,({struct Cyc_Absyn_Exp*
_tmp1A3[3];_tmp1A3[2]=_tmp19F;_tmp1A3[1]=_tmp18C;_tmp1A3[0]=_tmp1A0;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1A3,sizeof(struct Cyc_Absyn_Exp*),
3));}),sc,loc);return Cyc_Tcgenrep_update_info(dict,type,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp18B,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp19E,({struct Cyc_Absyn_Decl*_tmp1A2[1];_tmp1A2[0]=_tmp1A1;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1A2,sizeof(struct Cyc_Absyn_Decl*),
1));}))),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp195,0);return dict;}}}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepXTunion(
struct _tuple6*xname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*
loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*nametypes){
struct Cyc_List_List*_tmp1A9;struct Cyc_List_List*_tmp1AA;struct _tuple0 _tmp1A8=((
struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(nametypes);_tmp1A9=
_tmp1A8.f1;_tmp1AA=_tmp1A8.f2;{struct _tuple21*_tmp1AB=({struct _tuple21*_tmp1C0=
_cycalloc(sizeof(*_tmp1C0));_tmp1C0->f1=te;_tmp1C0->f2=loc;_tmp1C0;});struct
_tuple20*_tmp1AC=({struct _tuple20*_tmp1BF=_cycalloc(sizeof(*_tmp1BF));_tmp1BF->f1=
dict;_tmp1BF->f2=0;_tmp1BF;});struct _tuple20 _tmp1AE;struct Cyc_Dict_Dict*_tmp1AF;
struct Cyc_List_List*_tmp1B0;struct _tuple20*_tmp1AD=((struct _tuple20*(*)(struct
_tuple20*(*f)(struct _tuple21*,void*,struct _tuple20*),struct _tuple21*,struct Cyc_List_List*
x,struct _tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp1AB,_tmp1AA,_tmp1AC);_tmp1AE=*_tmp1AD;_tmp1AF=_tmp1AE.f1;_tmp1B0=_tmp1AE.f2;
dict=_tmp1AF;{struct Cyc_List_List*_tmp1B1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _dynforward_ptr*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_string_cls,loc,
_tmp1A9);struct Cyc_List_List*_tmp1B2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp1B0);struct Cyc_List_List*_tmp1B3=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp1B1,_tmp1B2);struct Cyc_List_List*
_tmp1B4=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple12*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,_tmp1B3);void*name_type=Cyc_Absyn_dynforward_typ(
Cyc_Absyn_char_typ,(void*)2,({struct Cyc_Absyn_Tqual _tmp1BE;_tmp1BE.print_const=1;
_tmp1BE.q_volatile=0;_tmp1BE.q_restrict=0;_tmp1BE.real_const=1;_tmp1BE.loc=0;
_tmp1BE;}),Cyc_Absyn_true_conref);void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*
_tmp1BD[2];_tmp1BD[1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp1BD[0]=name_type;Cyc_List_list(_tag_dynforward(_tmp1BD,sizeof(void*),2));}));
struct _tuple15 _tmp1B6;struct Cyc_List_List*_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B8;
struct _tuple15*_tmp1B5=Cyc_Tcgenrep_array_decls(tuple_type,_tmp1B4,loc);_tmp1B6=*
_tmp1B5;_tmp1B7=_tmp1B6.f1;_tmp1B8=_tmp1B6.f2;{struct Cyc_Absyn_Exp*_tmp1B9=Cyc_Tcgenrep_cnst_string(*(*
xname).f2,loc);struct Cyc_Absyn_Decl*_tmp1BA=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_xtunion_str),varname,({struct Cyc_Absyn_Exp*_tmp1BC[2];_tmp1BC[1]=
_tmp1B8;_tmp1BC[0]=_tmp1B9;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp1BC,sizeof(struct Cyc_Absyn_Exp*),2));}),sc,loc);return Cyc_Tcgenrep_update_info(
dict,type,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp1B7,({struct Cyc_Absyn_Decl*_tmp1BB[1];_tmp1BB[0]=
_tmp1BA;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp1BB,sizeof(struct Cyc_Absyn_Decl*),1));})),(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp1B0,0);
return dict;}}}}static struct _tuple19*Cyc_Tcgenrep_lookupRep(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,void*type){struct Cyc_Core_Opt*
info=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,void*k))Cyc_Dict_lookup_opt)(
dict,type);if(info != 0){if(((struct Cyc_Tcgenrep_RepInfo*)info->v)->is_extern)
dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,void*))Cyc_Dict_delete)(dict,
type);else{struct _tuple19*_tmp1C1=({struct _tuple19*_tmp1C2=_cycalloc(sizeof(*
_tmp1C2));_tmp1C2->f1=dict;_tmp1C2->f2=(struct Cyc_Tcgenrep_RepInfo*)info->v;
_tmp1C2;});return _tmp1C1;}}{void*_tmp1C3=Cyc_Tcutil_compress(Cyc_Tcgenrep_monomorphize_type(
type));void*_tmp1C4;void*_tmp1C5;int _tmp1C6;struct Cyc_Absyn_PtrInfo _tmp1C7;
struct Cyc_Absyn_ArrayInfo _tmp1C8;void*_tmp1C9;struct Cyc_Absyn_Tqual _tmp1CA;
struct Cyc_Absyn_Exp*_tmp1CB;struct Cyc_Absyn_Conref*_tmp1CC;struct Cyc_List_List*
_tmp1CD;struct _tuple6*_tmp1CE;struct Cyc_List_List*_tmp1CF;struct Cyc_Absyn_Typedefdecl*
_tmp1D0;void**_tmp1D1;struct Cyc_Absyn_Tvar*_tmp1D2;struct Cyc_Absyn_FnInfo _tmp1D3;
struct Cyc_Absyn_TunionInfo _tmp1D4;void*_tmp1D5;struct Cyc_Absyn_Tuniondecl**
_tmp1D6;struct Cyc_Absyn_Tuniondecl*_tmp1D7;struct Cyc_List_List*_tmp1D8;struct Cyc_Core_Opt*
_tmp1D9;struct Cyc_Absyn_TunionFieldInfo _tmp1DA;struct Cyc_Absyn_AggrInfo _tmp1DB;
void*_tmp1DC;struct Cyc_List_List*_tmp1DD;void*_tmp1DE;struct Cyc_List_List*
_tmp1DF;void*_tmp1E0;struct Cyc_List_List*_tmp1E1;struct Cyc_Absyn_Enumdecl*
_tmp1E2;struct Cyc_List_List*_tmp1E3;_LL18: if(_tmp1C3 <= (void*)4)goto _LL1A;if(*((
int*)_tmp1C3)!= 5)goto _LL1A;_tmp1C4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1C3)->f1;_tmp1C5=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1C3)->f2;_LL19: {
struct _tuple6*_tmp1E4=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({const
char*_tmp1E8="rep";_tag_dynforward(_tmp1E8,sizeof(char),_get_zero_arr_size(
_tmp1E8,4));})));struct Cyc_Absyn_Decl*_tmp1E5=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_int_str),_tmp1E4,({struct Cyc_Absyn_Exp*_tmp1E7[2];_tmp1E7[1]=Cyc_Tcgenrep_cnst_int(
Cyc_Tcgenrep_size_of2int(_tmp1C5),loc);_tmp1E7[0]=Cyc_Tcgenrep_cnst_int(_tmp1C4
== (void*)0?1: 0,loc);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp1E7,sizeof(struct Cyc_Absyn_Exp*),2));}),(void*)0,loc);dict=
Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*_tmp1E6[1];_tmp1E6[0]=
_tmp1E5;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp1E6,sizeof(struct Cyc_Absyn_Decl*),1));}),(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp1E4,loc),loc),0,0);goto _LL17;}
_LL1A: if((int)_tmp1C3 != 1)goto _LL1C;_LL1B: dict=Cyc_Tcgenrep_update_info(dict,
type,0,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_float_str),loc),0,0);goto _LL17;_LL1C: if(_tmp1C3 <= (void*)4)goto
_LL26;if(*((int*)_tmp1C3)!= 6)goto _LL1E;_tmp1C6=((struct Cyc_Absyn_DoubleType_struct*)
_tmp1C3)->f1;_LL1D: dict=Cyc_Tcgenrep_update_info(dict,type,0,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_double_str),loc),
0,0);goto _LL17;_LL1E: if(*((int*)_tmp1C3)!= 4)goto _LL20;_tmp1C7=((struct Cyc_Absyn_PointerType_struct*)
_tmp1C3)->f1;_LL1F:{void*_tmp1E9=Cyc_Absyn_conref_val((_tmp1C7.ptr_atts).bounds);
struct Cyc_Absyn_Exp*_tmp1EA;_LL51: if(_tmp1E9 <= (void*)2)goto _LL53;if(*((int*)
_tmp1E9)!= 0)goto _LL53;_tmp1EA=((struct Cyc_Absyn_Upper_b_struct*)_tmp1E9)->f1;
_LL52: {struct _tuple6*_tmp1EB=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({
const char*_tmp1F6="rep";_tag_dynforward(_tmp1F6,sizeof(char),_get_zero_arr_size(
_tmp1F6,4));})));struct Cyc_Core_Opt*_tmp1EC=({struct Cyc_Core_Opt*_tmp1F5=
_cycalloc(sizeof(*_tmp1F5));_tmp1F5->v=Cyc_Tcgenrep_gen_vardecl(_tmp1EB,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_thinptr_str)),0,(void*)3,loc);_tmp1F5;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1EB,loc),loc),_tmp1EC,0);{struct _tuple19 _tmp1EE;struct Cyc_Dict_Dict*_tmp1EF;
struct Cyc_Tcgenrep_RepInfo*_tmp1F0;struct _tuple19*_tmp1ED=Cyc_Tcgenrep_lookupRep(
te,dict,loc,(void*)_tmp1C7.elt_typ);_tmp1EE=*_tmp1ED;_tmp1EF=_tmp1EE.f1;_tmp1F0=
_tmp1EE.f2;dict=_tmp1EF;{struct Cyc_Absyn_Decl*_tmp1F1=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_thinptr_str),_tmp1EB,({struct Cyc_Absyn_Exp*_tmp1F4[2];_tmp1F4[1]=(
struct Cyc_Absyn_Exp*)_check_null(_tmp1F0->exp);_tmp1F4[0]=_tmp1EA;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1F4,sizeof(struct Cyc_Absyn_Exp*),
2));}),(void*)0,loc);dict=Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*
_tmp1F2[1];_tmp1F2[0]=_tmp1F1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp1F2,sizeof(struct Cyc_Absyn_Decl*),1));}),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp1EB,loc),
loc),({struct Cyc_Tcgenrep_RepInfo*_tmp1F3[1];_tmp1F3[0]=_tmp1F0;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1F3,sizeof(struct Cyc_Tcgenrep_RepInfo*),
1));}),0);goto _LL50;}}}_LL53: if((int)_tmp1E9 != 0)goto _LL55;_LL54: goto _LL56;_LL55:
if((int)_tmp1E9 != 1)goto _LL57;_LL56: {struct _tuple6*_tmp1F7=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(({const char*_tmp202="rep";_tag_dynforward(_tmp202,
sizeof(char),_get_zero_arr_size(_tmp202,4));})));struct Cyc_Core_Opt*_tmp1F8=({
struct Cyc_Core_Opt*_tmp201=_cycalloc(sizeof(*_tmp201));_tmp201->v=Cyc_Tcgenrep_gen_vardecl(
_tmp1F7,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_fatptr_str)),0,(void*)3,loc);_tmp201;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1F7,loc),loc),_tmp1F8,0);{struct _tuple19 _tmp1FA;
struct Cyc_Dict_Dict*_tmp1FB;struct Cyc_Tcgenrep_RepInfo*_tmp1FC;struct _tuple19*
_tmp1F9=Cyc_Tcgenrep_lookupRep(te,dict,loc,(void*)_tmp1C7.elt_typ);_tmp1FA=*
_tmp1F9;_tmp1FB=_tmp1FA.f1;_tmp1FC=_tmp1FA.f2;dict=_tmp1FB;{struct Cyc_Absyn_Decl*
_tmp1FD=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_fatptr_str),_tmp1F7,({struct Cyc_Absyn_Exp*
_tmp200[1];_tmp200[0]=(struct Cyc_Absyn_Exp*)_check_null(_tmp1FC->exp);((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp200,
sizeof(struct Cyc_Absyn_Exp*),1));}),(void*)0,loc);dict=Cyc_Tcgenrep_update_info(
dict,type,({struct Cyc_Absyn_Decl*_tmp1FE[1];_tmp1FE[0]=_tmp1FD;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1FE,sizeof(struct Cyc_Absyn_Decl*),
1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1F7,loc),loc),({struct Cyc_Tcgenrep_RepInfo*_tmp1FF[1];_tmp1FF[0]=_tmp1FC;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp1FF,sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),0);goto _LL50;}}}_LL57: if(
_tmp1E9 <= (void*)2)goto _LL50;if(*((int*)_tmp1E9)!= 1)goto _LL50;_LL58:({void*
_tmp203[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp204="tcgenregp: no support for abstract array bounds";
_tag_dynforward(_tmp204,sizeof(char),_get_zero_arr_size(_tmp204,48));}),
_tag_dynforward(_tmp203,sizeof(void*),0));});_LL50:;}goto _LL17;_LL20: if(*((int*)
_tmp1C3)!= 7)goto _LL22;_tmp1C8=((struct Cyc_Absyn_ArrayType_struct*)_tmp1C3)->f1;
_tmp1C9=(void*)_tmp1C8.elt_type;_tmp1CA=_tmp1C8.tq;_tmp1CB=_tmp1C8.num_elts;
_tmp1CC=_tmp1C8.zero_term;_LL21: if(_tmp1CB == 0)({void*_tmp205[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp206="tcgenrep:At this point, array bounds must be constants";_tag_dynforward(
_tmp206,sizeof(char),_get_zero_arr_size(_tmp206,55));}),_tag_dynforward(_tmp205,
sizeof(void*),0));});{struct _tuple19 _tmp208;struct Cyc_Dict_Dict*_tmp209;struct
Cyc_Tcgenrep_RepInfo*_tmp20A;struct _tuple19*_tmp207=Cyc_Tcgenrep_lookupRep(te,
dict,loc,_tmp1C9);_tmp208=*_tmp207;_tmp209=_tmp208.f1;_tmp20A=_tmp208.f2;dict=
_tmp209;{struct _tuple6*_tmp20B=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({
const char*_tmp210="rep";_tag_dynforward(_tmp210,sizeof(char),_get_zero_arr_size(
_tmp210,4));})));struct Cyc_Absyn_Decl*_tmp20C=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_thinptr_str),_tmp20B,({struct Cyc_Absyn_Exp*_tmp20F[2];_tmp20F[1]=(
struct Cyc_Absyn_Exp*)_check_null(_tmp20A->exp);_tmp20F[0]=(struct Cyc_Absyn_Exp*)
_tmp1CB;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp20F,sizeof(struct Cyc_Absyn_Exp*),2));}),(void*)0,loc);dict=
Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*_tmp20D[1];_tmp20D[0]=
_tmp20C;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp20D,sizeof(struct Cyc_Absyn_Decl*),1));}),(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp20B,loc),loc),({struct Cyc_Tcgenrep_RepInfo*
_tmp20E[1];_tmp20E[0]=_tmp20A;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp20E,sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),
0);goto _LL17;}}_LL22: if(*((int*)_tmp1C3)!= 9)goto _LL24;_tmp1CD=((struct Cyc_Absyn_TupleType_struct*)
_tmp1C3)->f1;_LL23: {struct _tuple6*_tmp211=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({
const char*_tmp215="rep";_tag_dynforward(_tmp215,sizeof(char),_get_zero_arr_size(
_tmp215,4));})));struct Cyc_Core_Opt*_tmp212=({struct Cyc_Core_Opt*_tmp214=
_cycalloc(sizeof(*_tmp214));_tmp214->v=Cyc_Tcgenrep_gen_vardecl(_tmp211,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tuple_str)),0,(void*)3,loc);_tmp214;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp211,loc),loc),_tmp212,0);{struct Cyc_List_List*_tmp213=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_second,
_tmp1CD);dict=Cyc_Tcgenrep_buildRepTuple(te,dict,loc,_tmp211,(void*)0,type,
_tmp213);goto _LL17;}}_LL24: if(*((int*)_tmp1C3)!= 16)goto _LL26;_tmp1CE=((struct
Cyc_Absyn_TypedefType_struct*)_tmp1C3)->f1;_tmp1CF=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1C3)->f2;_tmp1D0=((struct Cyc_Absyn_TypedefType_struct*)_tmp1C3)->f3;_tmp1D1=((
struct Cyc_Absyn_TypedefType_struct*)_tmp1C3)->f4;_LL25: if(_tmp1D1 == 0)({void*
_tmp216[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp217="gen(): can't gen for abstract typedef";_tag_dynforward(
_tmp217,sizeof(char),_get_zero_arr_size(_tmp217,38));}),_tag_dynforward(_tmp216,
sizeof(void*),0));});{struct _tuple19 _tmp219;struct Cyc_Dict_Dict*_tmp21A;struct
Cyc_Tcgenrep_RepInfo*_tmp21B;struct _tuple19*_tmp218=Cyc_Tcgenrep_lookupRep(te,
dict,loc,*_tmp1D1);_tmp219=*_tmp218;_tmp21A=_tmp219.f1;_tmp21B=_tmp219.f2;dict=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*
v))Cyc_Dict_insert)(_tmp21A,type,_tmp21B);goto _LL17;}_LL26: if((int)_tmp1C3 != 0)
goto _LL28;_LL27:({void*_tmp21C[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp21D="found void in gen() expression";
_tag_dynforward(_tmp21D,sizeof(char),_get_zero_arr_size(_tmp21D,31));}),
_tag_dynforward(_tmp21C,sizeof(void*),0));});goto _LL17;_LL28: if(_tmp1C3 <= (void*)
4)goto _LL30;if(*((int*)_tmp1C3)!= 0)goto _LL2A;_LL29:({void*_tmp21E[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp21F="found evar in gen() expression";_tag_dynforward(_tmp21F,
sizeof(char),_get_zero_arr_size(_tmp21F,31));}),_tag_dynforward(_tmp21E,sizeof(
void*),0));});goto _LL17;_LL2A: if(*((int*)_tmp1C3)!= 1)goto _LL2C;_tmp1D2=((struct
Cyc_Absyn_VarType_struct*)_tmp1C3)->f1;_LL2B:({void*_tmp220[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp221="found tyvar in gen() expression";_tag_dynforward(
_tmp221,sizeof(char),_get_zero_arr_size(_tmp221,32));}),_tag_dynforward(_tmp220,
sizeof(void*),0));});goto _LL17;_LL2C: if(*((int*)_tmp1C3)!= 8)goto _LL2E;_tmp1D3=((
struct Cyc_Absyn_FnType_struct*)_tmp1C3)->f1;_LL2D:({void*_tmp222[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp223="found function type in gen() expression";
_tag_dynforward(_tmp223,sizeof(char),_get_zero_arr_size(_tmp223,40));}),
_tag_dynforward(_tmp222,sizeof(void*),0));});goto _LL17;_LL2E: if(*((int*)_tmp1C3)
!= 15)goto _LL30;_LL2F: goto _LL31;_LL30: if((int)_tmp1C3 != 3)goto _LL32;_LL31: goto
_LL33;_LL32: if((int)_tmp1C3 != 2)goto _LL34;_LL33: goto _LL35;_LL34: if(_tmp1C3 <= (
void*)4)goto _LL36;if(*((int*)_tmp1C3)!= 19)goto _LL36;_LL35: goto _LL37;_LL36: if(
_tmp1C3 <= (void*)4)goto _LL38;if(*((int*)_tmp1C3)!= 20)goto _LL38;_LL37: goto _LL39;
_LL38: if(_tmp1C3 <= (void*)4)goto _LL3A;if(*((int*)_tmp1C3)!= 21)goto _LL3A;_LL39:({
void*_tmp224[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp225="gen(): unhandled region, handle or effect type";
_tag_dynforward(_tmp225,sizeof(char),_get_zero_arr_size(_tmp225,47));}),
_tag_dynforward(_tmp224,sizeof(void*),0));});goto _LL17;_LL3A: if(_tmp1C3 <= (void*)
4)goto _LL3C;if(*((int*)_tmp1C3)!= 2)goto _LL3C;_tmp1D4=((struct Cyc_Absyn_TunionType_struct*)
_tmp1C3)->f1;_tmp1D5=(void*)_tmp1D4.tunion_info;if(*((int*)_tmp1D5)!= 1)goto
_LL3C;_tmp1D6=((struct Cyc_Absyn_KnownTunion_struct*)_tmp1D5)->f1;_tmp1D7=*
_tmp1D6;_tmp1D8=_tmp1D4.targs;_tmp1D9=_tmp1D4.rgn;_LL3B: if(_tmp1D7->tvs != 0)({
void*_tmp226[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp227="gen(): tunion type variables not handled yet";
_tag_dynforward(_tmp227,sizeof(char),_get_zero_arr_size(_tmp227,45));}),
_tag_dynforward(_tmp226,sizeof(void*),0));});if(_tmp1D7->fields == 0){struct
_tuple6*_tmp228=Cyc_Tcgenrep_toplevel_name(({struct _dynforward_ptr*_tmp22B=
_cycalloc(sizeof(*_tmp22B));_tmp22B[0]=Cyc_Tcgenrep_make_type_cstring(type);
_tmp22B;}));struct Cyc_Core_Opt*_tmp229=({struct Cyc_Core_Opt*_tmp22A=_cycalloc(
sizeof(*_tmp22A));_tmp22A->v=Cyc_Tcgenrep_gen_vardecl(_tmp228,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tunion_str)),0,(void*)3,loc);_tmp22A;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp228,loc),loc),_tmp229,1);goto _LL17;}if(!_tmp1D7->is_xtunion){struct _tuple6*
_tmp22C=Cyc_Tcgenrep_toplevel_name(({struct _dynforward_ptr*_tmp234=_cycalloc(
sizeof(*_tmp234));_tmp234[0]=Cyc_Tcgenrep_make_type_cstring(type);_tmp234;}));
struct Cyc_Core_Opt*_tmp22D=({struct Cyc_Core_Opt*_tmp233=_cycalloc(sizeof(*
_tmp233));_tmp233->v=Cyc_Tcgenrep_gen_vardecl(_tmp22C,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tunion_str)),0,(void*)3,loc);_tmp233;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp22C,loc),loc),_tmp22D,0);{struct Cyc_List_List*_tmp22E=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1D7->fields))->v;struct Cyc_List_List*_tmp22F=((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*
x))Cyc_List_filter)(Cyc_Tcgenrep_filter_empty_tunionfield,_tmp22E);struct Cyc_List_List*
_tmp230=((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Tunionfield*),struct
Cyc_List_List*x))Cyc_List_filter)(Cyc_Tcgenrep_filter_nonempty_tunionfield,
_tmp22E);struct Cyc_List_List*_tmp231=((struct Cyc_List_List*(*)(struct _tuple18*(*
f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_tunionfield_and_get_nmtype,
_tmp22F);struct Cyc_List_List*_tmp232=((struct Cyc_List_List*(*)(struct _tuple6*(*f)(
struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_tunionfield_and_get_name,
_tmp230);dict=Cyc_Tcgenrep_buildRepTunion(_tmp1D7->name,te,dict,loc,_tmp22C,(
void*)2,type,_tmp232,_tmp231);}}else{struct _tuple6*_tmp235=Cyc_Tcgenrep_toplevel_name(({
struct _dynforward_ptr*_tmp23B=_cycalloc(sizeof(*_tmp23B));_tmp23B[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp23B;}));struct Cyc_Core_Opt*_tmp236=({struct Cyc_Core_Opt*_tmp23A=
_cycalloc(sizeof(*_tmp23A));_tmp23A->v=Cyc_Tcgenrep_gen_vardecl(_tmp235,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_xtunion_str)),0,(void*)3,loc);_tmp23A;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp235,loc),loc),_tmp236,0);{struct Cyc_List_List*_tmp237=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1D7->fields))->v;struct Cyc_List_List*_tmp238=((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*
x))Cyc_List_filter)(Cyc_Tcgenrep_filter_empty_tunionfield,_tmp237);struct Cyc_List_List*
_tmp239=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_xtunionfield_and_get_name_type,
_tmp238);dict=Cyc_Tcgenrep_buildRepXTunion(_tmp1D7->name,te,dict,loc,_tmp235,(
void*)2,type,_tmp239);}}goto _LL17;_LL3C: if(_tmp1C3 <= (void*)4)goto _LL3E;if(*((
int*)_tmp1C3)!= 2)goto _LL3E;_LL3D:({void*_tmp23C[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp23D="gen(): tunion must be resolved by now";_tag_dynforward(_tmp23D,sizeof(
char),_get_zero_arr_size(_tmp23D,38));}),_tag_dynforward(_tmp23C,sizeof(void*),0));});
_LL3E: if(_tmp1C3 <= (void*)4)goto _LL40;if(*((int*)_tmp1C3)!= 3)goto _LL40;_tmp1DA=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp1C3)->f1;_LL3F: if(_tmp1DA.targs != 0)({
void*_tmp23E[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp23F="gen(): tunionfield type parameters not handled yet";
_tag_dynforward(_tmp23F,sizeof(char),_get_zero_arr_size(_tmp23F,51));}),
_tag_dynforward(_tmp23E,sizeof(void*),0));});{void*_tmp240=(void*)_tmp1DA.field_info;
struct Cyc_Absyn_Tuniondecl*_tmp241;struct Cyc_Absyn_Tunionfield*_tmp242;_LL5A: if(*((
int*)_tmp240)!= 1)goto _LL5C;_tmp241=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp240)->f1;_tmp242=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp240)->f2;
_LL5B: {struct Cyc_List_List*_tmp243=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_second,_tmp242->typs);
struct Cyc_List_List*_tmp244=({struct Cyc_List_List*_tmp247=_cycalloc(sizeof(*
_tmp247));_tmp247->hd=(void*)Cyc_Absyn_uint_typ;_tmp247->tl=_tmp243;_tmp247;});
struct _tuple6*_tmp245=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({const
char*_tmp246="rep";_tag_dynforward(_tmp246,sizeof(char),_get_zero_arr_size(
_tmp246,4));})));dict=Cyc_Tcgenrep_buildRepTunionfield(_tmp241->name,_tmp242->name,
te,dict,loc,_tmp245,(void*)0,type,_tmp244);goto _LL59;}_LL5C:;_LL5D:({void*
_tmp248[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp249="gen(): tunionfield must be known at this point";
_tag_dynforward(_tmp249,sizeof(char),_get_zero_arr_size(_tmp249,47));}),
_tag_dynforward(_tmp248,sizeof(void*),0));});_LL59:;}goto _LL17;_LL40: if(_tmp1C3
<= (void*)4)goto _LL42;if(*((int*)_tmp1C3)!= 10)goto _LL42;_tmp1DB=((struct Cyc_Absyn_AggrType_struct*)
_tmp1C3)->f1;_tmp1DC=(void*)_tmp1DB.aggr_info;_tmp1DD=_tmp1DB.targs;_LL41: {
struct Cyc_Absyn_Aggrdecl*_tmp24A=Cyc_Absyn_get_known_aggrdecl(_tmp1DC);if(
_tmp24A->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp24A->impl))->exist_vars
!= 0)({void*_tmp24B[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp24C="gen(): existential type variables not yet supported";
_tag_dynforward(_tmp24C,sizeof(char),_get_zero_arr_size(_tmp24C,52));}),
_tag_dynforward(_tmp24B,sizeof(void*),0));});if((void*)_tmp24A->kind == (void*)0){
struct Cyc_Absyn_Aggrdecl*_tmp24D=_tmp24A;if(_tmp24D->impl == 0){struct _tuple6*
_tmp24E=Cyc_Tcgenrep_toplevel_name(({struct _dynforward_ptr*_tmp251=_cycalloc(
sizeof(*_tmp251));_tmp251[0]=Cyc_Tcgenrep_make_type_cstring(type);_tmp251;}));
struct Cyc_Core_Opt*_tmp24F=({struct Cyc_Core_Opt*_tmp250=_cycalloc(sizeof(*
_tmp250));_tmp250->v=Cyc_Tcgenrep_gen_vardecl(_tmp24E,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_struct_str)),0,(void*)3,loc);_tmp250;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp24E,loc),loc),_tmp24F,1);goto _LL17;}if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp24D->impl))->fields != 0  && ((int(*)(int(*pred)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcgenrep_has_bitfield,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp24D->impl))->fields)){int _tmp252=((int(*)(int(*f)(int,struct Cyc_Absyn_Aggrfield*),
int accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Tcgenrep_add_bitfield_sizes,
0,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp24D->impl))->fields);int
_tmp253=_tmp252 / 8 + (_tmp252 % 8 == 0?0: 1);struct Cyc_List_List*chars=0;{int i=0;
for(0;i < _tmp253;i ++){chars=({struct Cyc_List_List*_tmp254=_cycalloc(sizeof(*
_tmp254));_tmp254->hd=({struct _tuple10*_tmp255=_cycalloc(sizeof(*_tmp255));
_tmp255->f1=Cyc_Tcgenrep_tq_none;_tmp255->f2=Cyc_Absyn_char_typ;_tmp255;});
_tmp254->tl=chars;_tmp254;});}}{void*base_type=(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp25A=_cycalloc(sizeof(*_tmp25A));_tmp25A[0]=({struct Cyc_Absyn_TupleType_struct
_tmp25B;_tmp25B.tag=9;_tmp25B.f1=chars;_tmp25B;});_tmp25A;});struct _tuple19
_tmp257;struct Cyc_Dict_Dict*_tmp258;struct Cyc_Tcgenrep_RepInfo*_tmp259;struct
_tuple19*_tmp256=Cyc_Tcgenrep_lookupRep(te,dict,loc,base_type);_tmp257=*_tmp256;
_tmp258=_tmp257.f1;_tmp259=_tmp257.f2;dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(_tmp258,type,_tmp259);}}
else{struct _tuple6*_tmp25C=Cyc_Tcgenrep_toplevel_name(({struct _dynforward_ptr*
_tmp261=_cycalloc(sizeof(*_tmp261));_tmp261[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp261;}));struct Cyc_Core_Opt*_tmp25D=({struct Cyc_Core_Opt*_tmp260=
_cycalloc(sizeof(*_tmp260));_tmp260->v=Cyc_Tcgenrep_gen_vardecl(_tmp25C,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_struct_str)),0,(void*)3,loc);_tmp260;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp25C,loc),loc),_tmp25D,0);{struct Cyc_List_List*_tmp25E=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp24D->impl))->fields);dict=Cyc_Tcgenrep_buildRepStruct(({struct
Cyc_Core_Opt*_tmp25F=_cycalloc(sizeof(*_tmp25F));_tmp25F->v=_tmp24D->name;
_tmp25F;}),te,dict,loc,_tmp25C,(void*)2,type,_tmp25E);}}}else{struct Cyc_Absyn_Aggrdecl*
_tmp262=_tmp24A;if(_tmp262->tvs != 0)({void*_tmp263[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp264="gen: unions with parameters not yet supported";_tag_dynforward(_tmp264,
sizeof(char),_get_zero_arr_size(_tmp264,46));}),_tag_dynforward(_tmp263,sizeof(
void*),0));});if(_tmp262->impl == 0){struct _tuple6*_tmp265=Cyc_Tcgenrep_toplevel_name(({
struct _dynforward_ptr*_tmp268=_cycalloc(sizeof(*_tmp268));_tmp268[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp268;}));struct Cyc_Core_Opt*_tmp266=({struct Cyc_Core_Opt*_tmp267=
_cycalloc(sizeof(*_tmp267));_tmp267->v=Cyc_Tcgenrep_gen_vardecl(_tmp265,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_union_str)),0,(void*)3,loc);_tmp267;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp265,loc),loc),_tmp266,1);goto _LL17;}{struct Cyc_List_List*_tmp269=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp262->impl))->fields);struct _tuple6*_tmp26A=Cyc_Tcgenrep_toplevel_name(({
struct _dynforward_ptr*_tmp26C=_cycalloc(sizeof(*_tmp26C));_tmp26C[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp26C;}));dict=Cyc_Tcgenrep_buildRepUnion(({struct Cyc_Core_Opt*_tmp26B=
_cycalloc(sizeof(*_tmp26B));_tmp26B->v=_tmp262->name;_tmp26B;}),te,dict,loc,
_tmp26A,(void*)2,type,_tmp269);}}goto _LL17;}_LL42: if(_tmp1C3 <= (void*)4)goto
_LL44;if(*((int*)_tmp1C3)!= 11)goto _LL44;_tmp1DE=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1C3)->f1;if((int)_tmp1DE != 0)goto _LL44;_tmp1DF=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1C3)->f2;_LL43: if(_tmp1DF != 0  && ((int(*)(int(*pred)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcgenrep_has_bitfield,_tmp1DF)){int
_tmp26D=((int(*)(int(*f)(int,struct Cyc_Absyn_Aggrfield*),int accum,struct Cyc_List_List*
x))Cyc_List_fold_left)(Cyc_Tcgenrep_add_bitfield_sizes,0,_tmp1DF);int _tmp26E=
_tmp26D / 8 + (_tmp26D % 8 == 0?0: 1);void*base_type=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,
Cyc_Tcgenrep_tq_none,(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_int(_tmp26E,loc),
Cyc_Absyn_true_conref,0);struct _tuple19 _tmp270;struct Cyc_Dict_Dict*_tmp271;
struct Cyc_Tcgenrep_RepInfo*_tmp272;struct _tuple19*_tmp26F=Cyc_Tcgenrep_lookupRep(
te,dict,loc,base_type);_tmp270=*_tmp26F;_tmp271=_tmp270.f1;_tmp272=_tmp270.f2;
dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*
v))Cyc_Dict_insert)(_tmp271,type,_tmp272);}else{struct Cyc_List_List*_tmp273=((
struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Aggrfield*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_select_structfield_nmtype,_tmp1DF);
struct _tuple6*_tmp274=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({const
char*_tmp275="rep";_tag_dynforward(_tmp275,sizeof(char),_get_zero_arr_size(
_tmp275,4));})));dict=Cyc_Tcgenrep_buildRepStruct(0,te,dict,loc,_tmp274,(void*)0,
type,_tmp273);}goto _LL17;_LL44: if(_tmp1C3 <= (void*)4)goto _LL46;if(*((int*)
_tmp1C3)!= 11)goto _LL46;_tmp1E0=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1C3)->f1;if((int)_tmp1E0 != 1)goto _LL46;_tmp1E1=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1C3)->f2;_LL45: {struct Cyc_List_List*_tmp276=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,_tmp1E1);struct _tuple6*_tmp277=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(({const char*_tmp278="rep";_tag_dynforward(_tmp278,
sizeof(char),_get_zero_arr_size(_tmp278,4));})));dict=Cyc_Tcgenrep_buildRepUnion(
0,te,dict,loc,_tmp277,(void*)0,type,_tmp276);goto _LL17;}_LL46: if(_tmp1C3 <= (void*)
4)goto _LL48;if(*((int*)_tmp1C3)!= 12)goto _LL48;_tmp1E2=((struct Cyc_Absyn_EnumType_struct*)
_tmp1C3)->f2;_LL47: if(_tmp1E2 == 0)({void*_tmp279[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp27A="gen(): enum declaration must be present by now";_tag_dynforward(_tmp27A,
sizeof(char),_get_zero_arr_size(_tmp27A,47));}),_tag_dynforward(_tmp279,sizeof(
void*),0));});{struct Cyc_Absyn_Enumdecl _tmp27B=*_tmp1E2;if(_tmp27B.fields == 0){
struct _tuple6*_tmp27C=Cyc_Tcgenrep_toplevel_name(({struct _dynforward_ptr*_tmp27F=
_cycalloc(sizeof(*_tmp27F));_tmp27F[0]=Cyc_Tcgenrep_make_type_cstring(type);
_tmp27F;}));struct Cyc_Core_Opt*_tmp27D=({struct Cyc_Core_Opt*_tmp27E=_cycalloc(
sizeof(*_tmp27E));_tmp27E->v=Cyc_Tcgenrep_gen_vardecl(_tmp27C,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_enum_str)),0,(void*)3,loc);_tmp27E;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp27C,loc),loc),_tmp27D,1);goto _LL17;}{struct Cyc_List_List*_tmp280=((struct Cyc_List_List*(*)(
struct _tuple17*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_enumfield_tagnm,(struct Cyc_List_List*)(_tmp27B.fields)->v);
struct _tuple6*_tmp281=Cyc_Tcgenrep_toplevel_name(({struct _dynforward_ptr*_tmp282=
_cycalloc(sizeof(*_tmp282));_tmp282[0]=Cyc_Tcgenrep_make_type_cstring(type);
_tmp282;}));dict=Cyc_Tcgenrep_buildRepEnum((struct _tuple6**)& _tmp27B.name,te,
dict,loc,_tmp281,(void*)2,type,_tmp280);goto _LL17;}}_LL48: if(_tmp1C3 <= (void*)4)
goto _LL4A;if(*((int*)_tmp1C3)!= 13)goto _LL4A;_tmp1E3=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp1C3)->f1;_LL49: {struct Cyc_List_List*_tmp283=((struct Cyc_List_List*(*)(
struct _tuple17*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_enumfield_tagnm,_tmp1E3);struct _tuple6*_tmp284=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(({const char*_tmp285="rep";_tag_dynforward(_tmp285,
sizeof(char),_get_zero_arr_size(_tmp285,4));})));dict=Cyc_Tcgenrep_buildRepEnum(
0,te,dict,loc,_tmp284,(void*)0,type,_tmp283);goto _LL17;}_LL4A: if(_tmp1C3 <= (void*)
4)goto _LL4C;if(*((int*)_tmp1C3)!= 14)goto _LL4C;_LL4B:({void*_tmp286[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp287="gen() for sizeof_t<-> not yet supported";_tag_dynforward(_tmp287,
sizeof(char),_get_zero_arr_size(_tmp287,40));}),_tag_dynforward(_tmp286,sizeof(
void*),0));});_LL4C: if(_tmp1C3 <= (void*)4)goto _LL4E;if(*((int*)_tmp1C3)!= 18)
goto _LL4E;_LL4D:({void*_tmp288[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp289="gen() for `i not yet supported";
_tag_dynforward(_tmp289,sizeof(char),_get_zero_arr_size(_tmp289,31));}),
_tag_dynforward(_tmp288,sizeof(void*),0));});_LL4E: if(_tmp1C3 <= (void*)4)goto
_LL17;if(*((int*)_tmp1C3)!= 17)goto _LL17;_LL4F:({void*_tmp28A[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp28B="gen() for tag_t<-> not yet supported";_tag_dynforward(_tmp28B,sizeof(
char),_get_zero_arr_size(_tmp28B,37));}),_tag_dynforward(_tmp28A,sizeof(void*),0));});
_LL17:;}return({struct _tuple19*_tmp28C=_cycalloc(sizeof(*_tmp28C));_tmp28C->f1=
dict;_tmp28C->f2=((struct Cyc_Tcgenrep_RepInfo*(*)(struct Cyc_Dict_Dict*d,void*k))
Cyc_Dict_lookup)(dict,type);_tmp28C;});}static int Cyc_Tcgenrep_not_emitted_filter(
struct Cyc_Tcgenrep_RepInfo*ri){return ri->emitted == 0;}static void Cyc_Tcgenrep_mark_emitted(
struct Cyc_Tcgenrep_RepInfo*ri){ri->emitted=1;}struct _tuple22{struct Cyc_Dict_Dict*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple22 Cyc_Tcgenrep_tcGenrep(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
void*type,struct Cyc_Dict_Dict*dict){Cyc_Tcutil_check_valid_toplevel_type(loc,te,
type,0);Cyc_Tcutil_check_valid_toplevel_type(loc,te,type,0);{struct _tuple19
_tmp28E;struct Cyc_Dict_Dict*_tmp28F;struct Cyc_Tcgenrep_RepInfo*_tmp290;struct
_tuple19*_tmp28D=Cyc_Tcgenrep_lookupRep(te,dict,loc,type);_tmp28E=*_tmp28D;
_tmp28F=_tmp28E.f1;_tmp290=_tmp28E.f2;{struct Cyc_List_List*_tmp291=Cyc_Tcgenrep_dfs(
_tmp290);return({struct _tuple22 _tmp292;_tmp292.f1=_tmp28F;_tmp292.f2=_tmp291;
_tmp292.f3=(struct Cyc_Absyn_Exp*)_check_null(_tmp290->exp);_tmp292;});}}}
