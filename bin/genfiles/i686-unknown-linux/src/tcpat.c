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
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_dynforward_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(
struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rrev(struct
_RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];struct Cyc_List_List*
Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rtabulate_c(struct
_RegionHandle*r,int n,void*(*f)(void*,int),void*env);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
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
char*tag;struct _dynforward_ptr f1;};struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(
struct _RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(
struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(
struct Cyc_Set_Set*s);int Cyc_Set_is_empty(struct Cyc_Set_Set*s);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];void*Cyc_Set_choose(
struct Cyc_Set_Set*s);int Cyc_strcmp(struct _dynforward_ptr s1,struct _dynforward_ptr
s2);int Cyc_strptrcmp(struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct
_dynforward_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_Lineno_Pos{struct
_dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;void*
Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct
Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_char_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_float_typ;
extern void*Cyc_Absyn_empty_effect;struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,
struct Cyc_Position_Segment*s);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct
_dynforward_ptr Cyc_Absynpp_kind2string(void*);struct _dynforward_ptr Cyc_Absynpp_kindbound2string(
void*);struct _dynforward_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,
struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*
rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*
po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*rgn);void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);int Cyc_Tcutil_kind_leq(void*k1,void*
k2);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_coerceable(void*);extern
struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern
struct Cyc_Core_Opt Cyc_Tcutil_mk;int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple4{struct Cyc_List_List*
f1;struct _RegionHandle*f2;};struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct _tuple5*Cyc_Tcutil_r_make_inst_var(struct _tuple4*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr err_msg);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int
Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct _tuple6{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple6*
tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt);void
Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);void
Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,struct Cyc_List_List*);
int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,struct Cyc_Absyn_Pat*
p);void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*,struct Cyc_List_List*);
struct _tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p){void*_tmp0=(void*)p->r;struct _tuple1*_tmp1;struct _tuple1*_tmp2;struct Cyc_List_List*
_tmp3;int _tmp4;struct Cyc_Absyn_AggrInfo _tmp5;void*_tmp6;void*_tmp7;struct _tuple1*
_tmp8;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmpA;struct Cyc_List_List**
_tmpB;struct Cyc_List_List*_tmpC;int _tmpD;struct Cyc_Absyn_Exp*_tmpE;_LL1: if(_tmp0
<= (void*)2)goto _LL9;if(*((int*)_tmp0)!= 12)goto _LL3;_tmp1=((struct Cyc_Absyn_UnknownId_p_struct*)
_tmp0)->f1;_LL2:{struct _handler_cons _tmpF;_push_handler(& _tmpF);{int _tmp11=0;if(
setjmp(_tmpF.handler))_tmp11=1;if(!_tmp11){{struct _RegionHandle _tmp12=
_new_region("r");struct _RegionHandle*r=& _tmp12;_push_region(r);{void*_tmp13=Cyc_Tcenv_lookup_ordinary(
r,te,p->loc,_tmp1);struct Cyc_Absyn_Tuniondecl*_tmp14;struct Cyc_Absyn_Tunionfield*
_tmp15;struct Cyc_Absyn_Enumdecl*_tmp16;struct Cyc_Absyn_Enumfield*_tmp17;void*
_tmp18;struct Cyc_Absyn_Enumfield*_tmp19;_LLC: if(*((int*)_tmp13)!= 1)goto _LLE;
_LLD:(void*)(p->r=(void*)((void*)0));({void*_tmp1A[0]={};Cyc_Tcutil_terr(p->loc,({
const char*_tmp1B="struct tag used without arguments in pattern";_tag_dynforward(
_tmp1B,sizeof(char),_get_zero_arr_size(_tmp1B,45));}),_tag_dynforward(_tmp1A,
sizeof(void*),0));});_npop_handler(1);return;_LLE: if(*((int*)_tmp13)!= 2)goto
_LL10;_tmp14=((struct Cyc_Tcenv_TunionRes_struct*)_tmp13)->f1;_tmp15=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp13)->f2;_LLF:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Tunion_p_struct*
_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C[0]=({struct Cyc_Absyn_Tunion_p_struct
_tmp1D;_tmp1D.tag=6;_tmp1D.f1=_tmp14;_tmp1D.f2=_tmp15;_tmp1D.f3=0;_tmp1D.f4=0;
_tmp1D;});_tmp1C;})));_npop_handler(1);return;_LL10: if(*((int*)_tmp13)!= 3)goto
_LL12;_tmp16=((struct Cyc_Tcenv_EnumRes_struct*)_tmp13)->f1;_tmp17=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp13)->f2;_LL11:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Enum_p_struct*
_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E[0]=({struct Cyc_Absyn_Enum_p_struct
_tmp1F;_tmp1F.tag=10;_tmp1F.f1=_tmp16;_tmp1F.f2=_tmp17;_tmp1F;});_tmp1E;})));
_npop_handler(1);return;_LL12: if(*((int*)_tmp13)!= 4)goto _LL14;_tmp18=(void*)((
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp13)->f1;_tmp19=((struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp13)->f2;_LL13:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_AnonEnum_p_struct*
_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20[0]=({struct Cyc_Absyn_AnonEnum_p_struct
_tmp21;_tmp21.tag=11;_tmp21.f1=(void*)_tmp18;_tmp21.f2=_tmp19;_tmp21;});_tmp20;})));
_npop_handler(1);return;_LL14: if(*((int*)_tmp13)!= 0)goto _LLB;_LL15: goto _LLB;
_LLB:;};_pop_region(r);};_pop_handler();}else{void*_tmp10=(void*)_exn_thrown;
void*_tmp23=_tmp10;_LL17: if(_tmp23 != Cyc_Dict_Absent)goto _LL19;_LL18: goto _LL16;
_LL19:;_LL1A:(void)_throw(_tmp23);_LL16:;}}}{void*_tmp24=(*_tmp1).f1;struct Cyc_List_List*
_tmp25;_LL1C: if((int)_tmp24 != 0)goto _LL1E;_LL1D: goto _LL1F;_LL1E: if(_tmp24 <= (
void*)1)goto _LL20;if(*((int*)_tmp24)!= 0)goto _LL20;_tmp25=((struct Cyc_Absyn_Rel_n_struct*)
_tmp24)->f1;if(_tmp25 != 0)goto _LL20;_LL1F:(*_tmp1).f1=(void*)0;(void*)(p->r=(
void*)((void*)({struct Cyc_Absyn_Var_p_struct*_tmp26=_cycalloc(sizeof(*_tmp26));
_tmp26[0]=({struct Cyc_Absyn_Var_p_struct _tmp27;_tmp27.tag=0;_tmp27.f1=Cyc_Absyn_new_vardecl(
_tmp1,(void*)0,0);_tmp27.f2=Cyc_Absyn_new_pat((void*)0,0);_tmp27;});_tmp26;})));
return;_LL20:;_LL21:({void*_tmp28[0]={};Cyc_Tcutil_terr(p->loc,({const char*
_tmp29="qualified variable in pattern";_tag_dynforward(_tmp29,sizeof(char),
_get_zero_arr_size(_tmp29,30));}),_tag_dynforward(_tmp28,sizeof(void*),0));});
return;_LL1B:;}_LL3: if(*((int*)_tmp0)!= 13)goto _LL5;_tmp2=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp0)->f1;_tmp3=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f2;_tmp4=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f3;_LL4:{struct _handler_cons _tmp2A;
_push_handler(& _tmp2A);{int _tmp2C=0;if(setjmp(_tmp2A.handler))_tmp2C=1;if(!
_tmp2C){{struct _RegionHandle _tmp2D=_new_region("r");struct _RegionHandle*r=&
_tmp2D;_push_region(r);{void*_tmp2E=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp2);
struct Cyc_Absyn_Aggrdecl*_tmp2F;struct Cyc_Absyn_Tuniondecl*_tmp30;struct Cyc_Absyn_Tunionfield*
_tmp31;_LL23: if(*((int*)_tmp2E)!= 1)goto _LL25;_tmp2F=((struct Cyc_Tcenv_AggrRes_struct*)
_tmp2E)->f1;_LL24: if((void*)_tmp2F->kind == (void*)1){(void*)(p->r=(void*)((void*)
0));({void*_tmp32[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp33="cannot pattern-match a union";
_tag_dynforward(_tmp33,sizeof(char),_get_zero_arr_size(_tmp33,29));}),
_tag_dynforward(_tmp32,sizeof(void*),0));});_npop_handler(1);return;}{struct Cyc_List_List*
_tmp34=0;for(0;_tmp3 != 0;_tmp3=_tmp3->tl){_tmp34=({struct Cyc_List_List*_tmp35=
_cycalloc(sizeof(*_tmp35));_tmp35->hd=({struct _tuple8*_tmp36=_cycalloc(sizeof(*
_tmp36));_tmp36->f1=0;_tmp36->f2=(struct Cyc_Absyn_Pat*)_tmp3->hd;_tmp36;});
_tmp35->tl=_tmp34;_tmp35;});}(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Aggr_p_struct*
_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37[0]=({struct Cyc_Absyn_Aggr_p_struct
_tmp38;_tmp38.tag=5;_tmp38.f1=({struct Cyc_Absyn_AggrInfo _tmp39;_tmp39.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));
_tmp3A[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp3B;_tmp3B.tag=1;_tmp3B.f1=({
struct Cyc_Absyn_Aggrdecl**_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=_tmp2F;
_tmp3C;});_tmp3B;});_tmp3A;}));_tmp39.targs=0;_tmp39;});_tmp38.f2=0;_tmp38.f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp34);_tmp38.f4=
_tmp4;_tmp38;});_tmp37;})));_npop_handler(1);return;}_LL25: if(*((int*)_tmp2E)!= 
2)goto _LL27;_tmp30=((struct Cyc_Tcenv_TunionRes_struct*)_tmp2E)->f1;_tmp31=((
struct Cyc_Tcenv_TunionRes_struct*)_tmp2E)->f2;_LL26:(void*)(p->r=(void*)((void*)({
struct Cyc_Absyn_Tunion_p_struct*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({
struct Cyc_Absyn_Tunion_p_struct _tmp3E;_tmp3E.tag=6;_tmp3E.f1=_tmp30;_tmp3E.f2=
_tmp31;_tmp3E.f3=_tmp3;_tmp3E.f4=_tmp4;_tmp3E;});_tmp3D;})));_npop_handler(1);
return;_LL27: if(*((int*)_tmp2E)!= 3)goto _LL29;_LL28: goto _LL2A;_LL29: if(*((int*)
_tmp2E)!= 4)goto _LL2B;_LL2A:({void*_tmp3F[0]={};Cyc_Tcutil_terr(p->loc,({const
char*_tmp40="enum tag used with arguments in pattern";_tag_dynforward(_tmp40,
sizeof(char),_get_zero_arr_size(_tmp40,40));}),_tag_dynforward(_tmp3F,sizeof(
void*),0));});(void*)(p->r=(void*)((void*)0));_npop_handler(1);return;_LL2B: if(*((
int*)_tmp2E)!= 0)goto _LL22;_LL2C: goto _LL22;_LL22:;};_pop_region(r);};
_pop_handler();}else{void*_tmp2B=(void*)_exn_thrown;void*_tmp42=_tmp2B;_LL2E: if(
_tmp42 != Cyc_Dict_Absent)goto _LL30;_LL2F: goto _LL2D;_LL30:;_LL31:(void)_throw(
_tmp42);_LL2D:;}}}({struct Cyc_String_pa_struct _tmp45;_tmp45.tag=0;_tmp45.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp2));{
void*_tmp43[1]={& _tmp45};Cyc_Tcutil_terr(p->loc,({const char*_tmp44="%s is not a constructor in pattern";
_tag_dynforward(_tmp44,sizeof(char),_get_zero_arr_size(_tmp44,35));}),
_tag_dynforward(_tmp43,sizeof(void*),1));}});(void*)(p->r=(void*)((void*)0));
return;_LL5: if(*((int*)_tmp0)!= 5)goto _LL7;_tmp5=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f1;_tmp6=(void*)_tmp5.aggr_info;if(*((int*)_tmp6)!= 0)goto _LL7;_tmp7=(
void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp6)->f1;_tmp8=((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp6)->f2;_tmp9=_tmp5.targs;_tmpA=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;
_tmpB=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;_tmpC=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f3;_tmpD=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f4;_LL6:{struct _handler_cons _tmp46;_push_handler(& _tmp46);{int _tmp48=0;
if(setjmp(_tmp46.handler))_tmp48=1;if(!_tmp48){{struct Cyc_Absyn_Aggrdecl**_tmp49=
Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmp8);struct Cyc_Absyn_Aggrdecl*_tmp4A=*
_tmp49;if((void*)_tmp4A->kind == (void*)1){({void*_tmp4B[0]={};Cyc_Tcutil_terr(p->loc,({
const char*_tmp4C="cannot pattern-match a union";_tag_dynforward(_tmp4C,sizeof(
char),_get_zero_arr_size(_tmp4C,29));}),_tag_dynforward(_tmp4B,sizeof(void*),0));});(
void*)(p->r=(void*)((void*)0));_npop_handler(0);return;}if(_tmp4A->impl == 0){({
void*_tmp4D[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp4E="can't destructure an abstract struct";
_tag_dynforward(_tmp4E,sizeof(char),_get_zero_arr_size(_tmp4E,37));}),
_tag_dynforward(_tmp4D,sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));
_npop_handler(0);return;}{int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4A->impl))->exist_vars)- ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(*_tmpB);if(more_exists < 0){({void*_tmp4F[
0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp50="too many existentially bound type variables in pattern";
_tag_dynforward(_tmp50,sizeof(char),_get_zero_arr_size(_tmp50,55));}),
_tag_dynforward(_tmp4F,sizeof(void*),0));});{struct Cyc_List_List**_tmp51=_tmpB;{
int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4A->impl))->exist_vars);for(0;n != 0;n --){_tmp51=&((struct Cyc_List_List*)
_check_null(*_tmp51))->tl;}}*_tmp51=0;}}else{if(more_exists > 0){struct Cyc_List_List*
_tmp52=0;for(0;more_exists != 0;more_exists --){_tmp52=({struct Cyc_List_List*
_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->hd=Cyc_Tcutil_new_tvar((void*)({struct
Cyc_Absyn_Unknown_kb_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54[0]=({struct
Cyc_Absyn_Unknown_kb_struct _tmp55;_tmp55.tag=1;_tmp55.f1=0;_tmp55;});_tmp54;}));
_tmp53->tl=_tmp52;_tmp53;});}*_tmpB=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB,_tmp52);}}(void*)(p->r=(
void*)((void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp56=_cycalloc(sizeof(*_tmp56));
_tmp56[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp57;_tmp57.tag=5;_tmp57.f1=({struct
Cyc_Absyn_AggrInfo _tmp58;_tmp58.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*
_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59[0]=({struct Cyc_Absyn_KnownAggr_struct
_tmp5A;_tmp5A.tag=1;_tmp5A.f1=_tmp49;_tmp5A;});_tmp59;}));_tmp58.targs=_tmp9;
_tmp58;});_tmp57.f2=*_tmpB;_tmp57.f3=_tmpC;_tmp57.f4=_tmpD;_tmp57;});_tmp56;})));}};
_pop_handler();}else{void*_tmp47=(void*)_exn_thrown;void*_tmp5C=_tmp47;_LL33: if(
_tmp5C != Cyc_Dict_Absent)goto _LL35;_LL34:({void*_tmp5D[0]={};Cyc_Tcutil_terr(p->loc,({
const char*_tmp5E="Non-struct name has designator patterns";_tag_dynforward(
_tmp5E,sizeof(char),_get_zero_arr_size(_tmp5E,40));}),_tag_dynforward(_tmp5D,
sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));goto _LL32;_LL35:;_LL36:(
void)_throw(_tmp5C);_LL32:;}}}return;_LL7: if(*((int*)_tmp0)!= 14)goto _LL9;_tmpE=((
struct Cyc_Absyn_Exp_p_struct*)_tmp0)->f1;_LL8: Cyc_Tcexp_tcExp(te,0,_tmpE);if(!
Cyc_Tcutil_is_const_exp(te,_tmpE)){({void*_tmp5F[0]={};Cyc_Tcutil_terr(p->loc,({
const char*_tmp60="non-constant expression in case pattern";_tag_dynforward(
_tmp60,sizeof(char),_get_zero_arr_size(_tmp60,40));}),_tag_dynforward(_tmp5F,
sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));}{unsigned int _tmp62;int
_tmp63;struct _tuple7 _tmp61=Cyc_Evexp_eval_const_uint_exp(_tmpE);_tmp62=_tmp61.f1;
_tmp63=_tmp61.f2;(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64[0]=({struct Cyc_Absyn_Int_p_struct _tmp65;
_tmp65.tag=7;_tmp65.f1=(void*)((void*)2);_tmp65.f2=(int)_tmp62;_tmp65;});_tmp64;})));
return;}_LL9:;_LLA: return;_LL0:;}static struct _dynforward_ptr*Cyc_Tcpat_get_name(
struct Cyc_Absyn_Vardecl*vd){return(*vd->name).f2;}static void*Cyc_Tcpat_any_type(
struct Cyc_List_List*s,void**topt){if(topt != 0)return*topt;return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp66=_cycalloc(sizeof(*
_tmp66));_tmp66->v=s;_tmp66;}));}static void*Cyc_Tcpat_num_type(void**topt,void*
numt){if(topt != 0  && Cyc_Tcutil_coerceable(*topt))return*topt;{void*_tmp67=Cyc_Tcutil_compress(
numt);_LL38: if(_tmp67 <= (void*)4)goto _LL3A;if(*((int*)_tmp67)!= 12)goto _LL3A;
_LL39: if(topt != 0)return*topt;goto _LL37;_LL3A:;_LL3B: goto _LL37;_LL37:;}return
numt;}static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**
v_result_ptr,void*t){(void*)(vd->type=(void*)t);vd->tq=Cyc_Absyn_empty_tqual(0);*
v_result_ptr=(struct Cyc_List_List*)({struct Cyc_List_List*_tmp68=_cycalloc(
sizeof(*_tmp68));_tmp68->hd=vd;_tmp68->tl=*v_result_ptr;_tmp68;});}static struct
Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,
struct Cyc_Tcpat_TcPatResult res2){struct _tuple6*_tmp6A;struct Cyc_List_List*_tmp6B;
struct Cyc_Tcpat_TcPatResult _tmp69=res1;_tmp6A=_tmp69.tvars_and_bounds_opt;_tmp6B=
_tmp69.patvars;{struct _tuple6*_tmp6D;struct Cyc_List_List*_tmp6E;struct Cyc_Tcpat_TcPatResult
_tmp6C=res2;_tmp6D=_tmp6C.tvars_and_bounds_opt;_tmp6E=_tmp6C.patvars;if(_tmp6A != 
0  || _tmp6D != 0){if(_tmp6A == 0)_tmp6A=({struct _tuple6*_tmp6F=_cycalloc(sizeof(*
_tmp6F));_tmp6F->f1=0;_tmp6F->f2=0;_tmp6F;});if(_tmp6D == 0)_tmp6D=({struct
_tuple6*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->f1=0;_tmp70->f2=0;_tmp70;});
return({struct Cyc_Tcpat_TcPatResult _tmp71;_tmp71.tvars_and_bounds_opt=({struct
_tuple6*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((struct _tuple6*)
_check_null(_tmp6A))).f1,(*((struct _tuple6*)_check_null(_tmp6D))).f1);_tmp72->f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((
struct _tuple6*)_check_null(_tmp6A))).f2,(*((struct _tuple6*)_check_null(_tmp6D))).f2);
_tmp72;});_tmp71.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp6B,_tmp6E);_tmp71;});}return({struct Cyc_Tcpat_TcPatResult
_tmp73;_tmp73.tvars_and_bounds_opt=0;_tmp73.patvars=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp6B,_tmp6E);
_tmp73;});}}static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Pat*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->loc=
loc;_tmp74->topt=0;_tmp74->r=(void*)((void*)0);_tmp74;});}struct _tuple9{struct
Cyc_Absyn_Tqual f1;void*f2;};struct _tuple10{struct Cyc_Absyn_Aggrfield*f1;struct
Cyc_Absyn_Pat*f2;};static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p,void**topt,void**rgn_opt){Cyc_Tcpat_resolve_pat(te,p);{
void*t;struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp149;
_tmp149.tvars_and_bounds_opt=0;_tmp149.patvars=0;_tmp149;});{void*_tmp75=(void*)
p->r;struct Cyc_Absyn_Vardecl*_tmp76;struct Cyc_Absyn_Pat*_tmp77;struct Cyc_Absyn_Vardecl*
_tmp78;struct Cyc_Absyn_Pat*_tmp79;struct Cyc_Absyn_Tvar*_tmp7A;struct Cyc_Absyn_Vardecl*
_tmp7B;void*_tmp7C;void*_tmp7D;void*_tmp7E;struct Cyc_Absyn_Enumdecl*_tmp7F;void*
_tmp80;struct Cyc_Absyn_Pat*_tmp81;struct Cyc_List_List*_tmp82;struct Cyc_List_List**
_tmp83;int _tmp84;struct Cyc_Absyn_AggrInfo _tmp85;void*_tmp86;struct Cyc_Absyn_Aggrdecl**
_tmp87;struct Cyc_Absyn_Aggrdecl*_tmp88;struct Cyc_List_List*_tmp89;struct Cyc_List_List**
_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_List_List*_tmp8C;struct Cyc_List_List**
_tmp8D;int _tmp8E;struct Cyc_Absyn_Tuniondecl*_tmp8F;struct Cyc_Absyn_Tunionfield*
_tmp90;struct Cyc_List_List*_tmp91;struct Cyc_List_List**_tmp92;int _tmp93;struct
Cyc_Absyn_AggrInfo _tmp94;void*_tmp95;_LL3D: if((int)_tmp75 != 0)goto _LL3F;_LL3E: t=
Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);goto _LL3C;_LL3F: if(
_tmp75 <= (void*)2)goto _LL53;if(*((int*)_tmp75)!= 0)goto _LL41;_tmp76=((struct Cyc_Absyn_Var_p_struct*)
_tmp75)->f1;_tmp77=((struct Cyc_Absyn_Var_p_struct*)_tmp75)->f2;_LL40: res=Cyc_Tcpat_tcPatRec(
te,_tmp77,topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp77->topt))->v;
Cyc_Tcpat_set_vd(_tmp76,& res.patvars,t);goto _LL3C;_LL41: if(*((int*)_tmp75)!= 1)
goto _LL43;_tmp78=((struct Cyc_Absyn_Reference_p_struct*)_tmp75)->f1;_tmp79=((
struct Cyc_Absyn_Reference_p_struct*)_tmp75)->f2;_LL42: res=Cyc_Tcpat_tcPatRec(te,
_tmp79,topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp79->topt))->v;
if(rgn_opt == 0){({void*_tmp96[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp97="* pattern would point into an unknown/unallowed region";
_tag_dynforward(_tmp97,sizeof(char),_get_zero_arr_size(_tmp97,55));}),
_tag_dynforward(_tmp96,sizeof(void*),0));});goto _LL3C;}Cyc_Tcpat_set_vd(_tmp78,&
res.patvars,(void*)({struct Cyc_Absyn_PointerType_struct*_tmp98=_cycalloc(sizeof(*
_tmp98));_tmp98[0]=({struct Cyc_Absyn_PointerType_struct _tmp99;_tmp99.tag=4;
_tmp99.f1=({struct Cyc_Absyn_PtrInfo _tmp9A;_tmp9A.elt_typ=(void*)t;_tmp9A.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp9A.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp9B;_tmp9B.rgn=(
void*)*rgn_opt;_tmp9B.nullable=Cyc_Absyn_false_conref;_tmp9B.bounds=Cyc_Absyn_empty_conref();
_tmp9B.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmp9B.ptrloc=
0;_tmp9B;});_tmp9A;});_tmp99;});_tmp98;}));goto _LL3C;_LL43: if(*((int*)_tmp75)!= 
2)goto _LL45;_tmp7A=((struct Cyc_Absyn_TagInt_p_struct*)_tmp75)->f1;_tmp7B=((
struct Cyc_Absyn_TagInt_p_struct*)_tmp75)->f2;_LL44: Cyc_Tcpat_set_vd(_tmp7B,& res.patvars,(
void*)_tmp7B->type);{struct _RegionHandle _tmp9C=_new_region("r2");struct
_RegionHandle*r2=& _tmp9C;_push_region(r2);Cyc_Tcenv_add_type_vars(r2,p->loc,te,({
struct Cyc_Absyn_Tvar*_tmp9D[1];_tmp9D[0]=_tmp7A;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp9D,sizeof(struct Cyc_Absyn_Tvar*),
1));}));;_pop_region(r2);}if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({
struct _tuple6*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->f1=0;_tmp9E->f2=0;_tmp9E;});(*
res.tvars_and_bounds_opt).f1=({struct Cyc_List_List*_tmp9F=_cycalloc(sizeof(*
_tmp9F));_tmp9F->hd=_tmp7A;_tmp9F->tl=(*res.tvars_and_bounds_opt).f1;_tmp9F;});t=
Cyc_Absyn_uint_typ;goto _LL3C;_LL45: if(*((int*)_tmp75)!= 7)goto _LL47;_tmp7C=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp75)->f1;if((int)_tmp7C != 1)goto _LL47;_LL46: t=
Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL3C;_LL47: if(*((int*)_tmp75)!= 
7)goto _LL49;_tmp7D=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp75)->f1;if((int)
_tmp7D != 2)goto _LL49;_LL48: goto _LL4A;_LL49: if(*((int*)_tmp75)!= 7)goto _LL4B;
_tmp7E=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp75)->f1;if((int)_tmp7E != 0)
goto _LL4B;_LL4A: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL3C;_LL4B:
if(*((int*)_tmp75)!= 8)goto _LL4D;_LL4C: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);
goto _LL3C;_LL4D: if(*((int*)_tmp75)!= 9)goto _LL4F;_LL4E: t=Cyc_Tcpat_num_type(topt,
Cyc_Absyn_float_typ);goto _LL3C;_LL4F: if(*((int*)_tmp75)!= 10)goto _LL51;_tmp7F=((
struct Cyc_Absyn_Enum_p_struct*)_tmp75)->f1;_LL50: t=Cyc_Tcpat_num_type(topt,(void*)({
struct Cyc_Absyn_EnumType_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=({
struct Cyc_Absyn_EnumType_struct _tmpA1;_tmpA1.tag=12;_tmpA1.f1=_tmp7F->name;
_tmpA1.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7F;_tmpA1;});_tmpA0;}));goto _LL3C;_LL51:
if(*((int*)_tmp75)!= 11)goto _LL53;_tmp80=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp75)->f1;_LL52: t=Cyc_Tcpat_num_type(topt,_tmp80);goto _LL3C;_LL53: if((int)
_tmp75 != 1)goto _LL55;_LL54: if(topt != 0){void*_tmpA2=Cyc_Tcutil_compress(*topt);
_LL66: if(_tmpA2 <= (void*)4)goto _LL68;if(*((int*)_tmpA2)!= 4)goto _LL68;_LL67: t=*
topt;goto tcpat_end;_LL68:;_LL69: goto _LL65;_LL65:;}{struct Cyc_List_List*_tmpA3=
Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=({struct Cyc_Absyn_PointerType_struct
_tmpA5;_tmpA5.tag=4;_tmpA5.f1=({struct Cyc_Absyn_PtrInfo _tmpA6;_tmpA6.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->v=_tmpA3;_tmpA9;}));_tmpA6.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmpA6.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpA7;_tmpA7.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmpA8=_cycalloc(sizeof(*
_tmpA8));_tmpA8->v=_tmpA3;_tmpA8;}));_tmpA7.nullable=Cyc_Absyn_true_conref;
_tmpA7.bounds=Cyc_Absyn_empty_conref();_tmpA7.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpA7.ptrloc=0;_tmpA7;});_tmpA6;});_tmpA5;});_tmpA4;});
goto _LL3C;}_LL55: if(_tmp75 <= (void*)2)goto _LL57;if(*((int*)_tmp75)!= 4)goto _LL57;
_tmp81=((struct Cyc_Absyn_Pointer_p_struct*)_tmp75)->f1;_LL56: {void*inner_typ=(
void*)0;void**_tmpAA=0;if(topt != 0){void*_tmpAB=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmpAC;void*_tmpAD;_LL6B: if(_tmpAB <= (void*)4)goto _LL6D;
if(*((int*)_tmpAB)!= 4)goto _LL6D;_tmpAC=((struct Cyc_Absyn_PointerType_struct*)
_tmpAB)->f1;_tmpAD=(void*)_tmpAC.elt_typ;_LL6C: inner_typ=_tmpAD;_tmpAA=(void**)&
inner_typ;goto _LL6A;_LL6D:;_LL6E: goto _LL6A;_LL6A:;}{void*ptr_rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmpC3=_cycalloc(sizeof(*
_tmpC3));_tmpC3->v=Cyc_Tcenv_lookup_type_vars(te);_tmpC3;}));res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,_tmp81,_tmpAA,(void**)& ptr_rgn));if(_tmpAA == 0){void*
_tmpAE=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp81->topt))->v);
struct Cyc_Absyn_TunionFieldInfo _tmpAF;void*_tmpB0;struct Cyc_Absyn_Tuniondecl*
_tmpB1;struct Cyc_Absyn_Tunionfield*_tmpB2;struct Cyc_List_List*_tmpB3;_LL70: if(
_tmpAE <= (void*)4)goto _LL72;if(*((int*)_tmpAE)!= 3)goto _LL72;_tmpAF=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpAE)->f1;_tmpB0=(void*)_tmpAF.field_info;if(*((int*)_tmpB0)!= 1)goto _LL72;
_tmpB1=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpB0)->f1;_tmpB2=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpB0)->f2;_tmpB3=_tmpAF.targs;if(!(!_tmpB1->is_flat))goto _LL72;_LL71: t=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({
struct Cyc_Absyn_TunionType_struct _tmpB5;_tmpB5.tag=2;_tmpB5.f1=({struct Cyc_Absyn_TunionInfo
_tmpB6;_tmpB6.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmpB9;_tmpB9.tag=1;_tmpB9.f1=({struct Cyc_Absyn_Tuniondecl**_tmpBA=_cycalloc(
sizeof(*_tmpBA));_tmpBA[0]=_tmpB1;_tmpBA;});_tmpB9;});_tmpB8;}));_tmpB6.targs=
_tmpB3;_tmpB6.rgn=({struct Cyc_Core_Opt*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->v=(
void*)ptr_rgn;_tmpB7;});_tmpB6;});_tmpB5;});_tmpB4;});goto _LL6F;_LL72:;_LL73: t=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmpBB=_cycalloc(sizeof(*_tmpBB));
_tmpBB[0]=({struct Cyc_Absyn_PointerType_struct _tmpBC;_tmpBC.tag=4;_tmpBC.f1=({
struct Cyc_Absyn_PtrInfo _tmpBD;_tmpBD.elt_typ=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp81->topt))->v);_tmpBD.elt_tq=Cyc_Absyn_empty_tqual(0);_tmpBD.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmpBE;_tmpBE.rgn=(void*)ptr_rgn;_tmpBE.nullable=((struct
Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmpBE.bounds=Cyc_Absyn_empty_conref();
_tmpBE.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmpBE.ptrloc=
0;_tmpBE;});_tmpBD;});_tmpBC;});_tmpBB;});goto _LL6F;_LL6F:;}else{t=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({
struct Cyc_Absyn_PointerType_struct _tmpC0;_tmpC0.tag=4;_tmpC0.f1=({struct Cyc_Absyn_PtrInfo
_tmpC1;_tmpC1.elt_typ=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp81->topt))->v);
_tmpC1.elt_tq=Cyc_Absyn_empty_tqual(0);_tmpC1.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmpC2;_tmpC2.rgn=(void*)ptr_rgn;_tmpC2.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpC2.bounds=Cyc_Absyn_empty_conref();_tmpC2.zero_term=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmpC2.ptrloc=0;_tmpC2;});
_tmpC1;});_tmpC0;});_tmpBF;});}goto _LL3C;}}_LL57: if(_tmp75 <= (void*)2)goto _LL59;
if(*((int*)_tmp75)!= 3)goto _LL59;_tmp82=((struct Cyc_Absyn_Tuple_p_struct*)_tmp75)->f1;
_tmp83=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_struct*)_tmp75)->f1;
_tmp84=((struct Cyc_Absyn_Tuple_p_struct*)_tmp75)->f2;_LL58: {struct Cyc_List_List*
_tmpC4=*_tmp83;struct Cyc_List_List*pat_ts=0;struct Cyc_List_List*topt_ts=0;if(
topt != 0){void*_tmpC5=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmpC6;
_LL75: if(_tmpC5 <= (void*)4)goto _LL77;if(*((int*)_tmpC5)!= 9)goto _LL77;_tmpC6=((
struct Cyc_Absyn_TupleType_struct*)_tmpC5)->f1;_LL76: topt_ts=_tmpC6;if(_tmp84){
int _tmpC7=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC4);int _tmpC8=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC6);if(_tmpC7 < _tmpC8){struct
Cyc_List_List*wild_ps=0;{int i=0;for(0;i < _tmpC8 - _tmpC7;i ++){wild_ps=({struct Cyc_List_List*
_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->hd=Cyc_Tcpat_wild_pat(p->loc);_tmpC9->tl=
wild_ps;_tmpC9;});}}*_tmp83=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC4,wild_ps);_tmpC4=*_tmp83;}
else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC4)== ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpC6))({void*_tmpCA[0]={};Cyc_Tcutil_warn(
p->loc,({const char*_tmpCB="unnecessary ... in tuple pattern";_tag_dynforward(
_tmpCB,sizeof(char),_get_zero_arr_size(_tmpCB,33));}),_tag_dynforward(_tmpCA,
sizeof(void*),0));});}}goto _LL74;_LL77:;_LL78: goto _LL74;_LL74:;}else{if(_tmp84)({
void*_tmpCC[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmpCD="cannot determine missing fields for ... in tuple pattern";
_tag_dynforward(_tmpCD,sizeof(char),_get_zero_arr_size(_tmpCD,57));}),
_tag_dynforward(_tmpCC,sizeof(void*),0));});}for(0;_tmpC4 != 0;_tmpC4=_tmpC4->tl){
void**_tmpCE=0;if(topt_ts != 0){_tmpCE=(void**)&(*((struct _tuple9*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(
struct Cyc_Absyn_Pat*)_tmpC4->hd,_tmpCE,rgn_opt));pat_ts=({struct Cyc_List_List*
_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->hd=({struct _tuple9*_tmpD0=_cycalloc(
sizeof(*_tmpD0));_tmpD0->f1=Cyc_Absyn_empty_tqual(0);_tmpD0->f2=(void*)((struct
Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Pat*)_tmpC4->hd)->topt))->v;_tmpD0;});
_tmpCF->tl=pat_ts;_tmpCF;});}t=(void*)({struct Cyc_Absyn_TupleType_struct*_tmpD1=
_cycalloc(sizeof(*_tmpD1));_tmpD1[0]=({struct Cyc_Absyn_TupleType_struct _tmpD2;
_tmpD2.tag=9;_tmpD2.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
pat_ts);_tmpD2;});_tmpD1;});goto _LL3C;}_LL59: if(_tmp75 <= (void*)2)goto _LL5B;if(*((
int*)_tmp75)!= 5)goto _LL5B;_tmp85=((struct Cyc_Absyn_Aggr_p_struct*)_tmp75)->f1;
_tmp86=(void*)_tmp85.aggr_info;if(*((int*)_tmp86)!= 1)goto _LL5B;_tmp87=((struct
Cyc_Absyn_KnownAggr_struct*)_tmp86)->f1;_tmp88=*_tmp87;_tmp89=_tmp85.targs;
_tmp8A=(struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_struct*)_tmp75)->f1).targs;
_tmp8B=((struct Cyc_Absyn_Aggr_p_struct*)_tmp75)->f2;_tmp8C=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp75)->f3;_tmp8D=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_struct*)
_tmp75)->f3;_tmp8E=((struct Cyc_Absyn_Aggr_p_struct*)_tmp75)->f4;_LL5A: {struct
Cyc_List_List*_tmpD3=*_tmp8D;if(_tmp88->impl == 0){({void*_tmpD4[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmpD5="can't destructure an abstract struct";_tag_dynforward(
_tmpD5,sizeof(char),_get_zero_arr_size(_tmpD5,37));}),_tag_dynforward(_tmpD4,
sizeof(void*),0));});t=Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpD6=_cycalloc(
sizeof(*_tmpD6));_tmpD6->v=Cyc_Tcenv_lookup_type_vars(te);_tmpD6;}));goto _LL3C;}
if(_tmp8B != 0)rgn_opt=0;{struct _RegionHandle _tmpD7=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpD7;_push_region(rgn);{struct Cyc_List_List*_tmpD8=0;struct
Cyc_List_List*outlives_constraints=0;struct Cyc_List_List*_tmpD9=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp88->impl))->exist_vars;{struct Cyc_List_List*t=_tmp8B;for(0;t != 0;
t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;struct Cyc_Absyn_Tvar*
uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmpD9))->hd;_tmpD9=
_tmpD9->tl;{void*_tmpDA=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmpDB=Cyc_Absyn_compress_kb((
void*)uv->kind);int error=0;void*k2;{void*_tmpDC=_tmpDB;void*_tmpDD;void*_tmpDE;
_LL7A: if(*((int*)_tmpDC)!= 2)goto _LL7C;_tmpDD=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpDC)->f2;_LL7B: _tmpDE=_tmpDD;goto _LL7D;_LL7C: if(*((int*)_tmpDC)!= 0)goto _LL7E;
_tmpDE=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpDC)->f1;_LL7D: k2=_tmpDE;goto
_LL79;_LL7E:;_LL7F:({void*_tmpDF[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpE0="unconstrained existential type variable in struct";
_tag_dynforward(_tmpE0,sizeof(char),_get_zero_arr_size(_tmpE0,50));}),
_tag_dynforward(_tmpDF,sizeof(void*),0));});_LL79:;}{void*_tmpE1=_tmpDA;void*
_tmpE2;struct Cyc_Core_Opt*_tmpE3;struct Cyc_Core_Opt**_tmpE4;void*_tmpE5;struct
Cyc_Core_Opt*_tmpE6;struct Cyc_Core_Opt**_tmpE7;_LL81: if(*((int*)_tmpE1)!= 0)goto
_LL83;_tmpE2=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpE1)->f1;_LL82: if(!Cyc_Tcutil_kind_leq(
k2,_tmpE2))error=1;goto _LL80;_LL83: if(*((int*)_tmpE1)!= 2)goto _LL85;_tmpE3=((
struct Cyc_Absyn_Less_kb_struct*)_tmpE1)->f1;_tmpE4=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmpE1)->f1;_tmpE5=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpE1)->f2;_LL84: _tmpE7=_tmpE4;goto _LL86;_LL85: if(*((int*)_tmpE1)!= 1)goto _LL80;
_tmpE6=((struct Cyc_Absyn_Unknown_kb_struct*)_tmpE1)->f1;_tmpE7=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmpE1)->f1;_LL86:*_tmpE7=({struct Cyc_Core_Opt*
_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->v=(void*)_tmpDB;_tmpE8;});goto _LL80;
_LL80:;}if(error)({struct Cyc_String_pa_struct _tmpED;_tmpED.tag=0;_tmpED.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(k2));{
struct Cyc_String_pa_struct _tmpEC;_tmpEC.tag=0;_tmpEC.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_kindbound2string(_tmpDA));{struct Cyc_String_pa_struct
_tmpEB;_tmpEB.tag=0;_tmpEB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*tv->name);{
void*_tmpE9[3]={& _tmpEB,& _tmpEC,& _tmpED};Cyc_Tcutil_terr(p->loc,({const char*
_tmpEA="type variable %s has kind %s but must have at least kind %s";
_tag_dynforward(_tmpEA,sizeof(char),_get_zero_arr_size(_tmpEA,60));}),
_tag_dynforward(_tmpE9,sizeof(void*),3));}}}});{void*vartype=(void*)({struct Cyc_Absyn_VarType_struct*
_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1[0]=({struct Cyc_Absyn_VarType_struct
_tmpF2;_tmpF2.tag=1;_tmpF2.f1=tv;_tmpF2;});_tmpF1;});_tmpD8=({struct Cyc_List_List*
_tmpEE=_region_malloc(rgn,sizeof(*_tmpEE));_tmpEE->hd=(void*)((void*)vartype);
_tmpEE->tl=_tmpD8;_tmpEE;});if(k2 == (void*)3)outlives_constraints=({struct Cyc_List_List*
_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF->hd=({struct _tuple0*_tmpF0=_cycalloc(
sizeof(*_tmpF0));_tmpF0->f1=Cyc_Absyn_empty_effect;_tmpF0->f2=vartype;_tmpF0;});
_tmpEF->tl=outlives_constraints;_tmpEF;});}}}}_tmpD8=Cyc_List_imp_rev(_tmpD8);{
struct _RegionHandle _tmpF3=_new_region("r2");struct _RegionHandle*r2=& _tmpF3;
_push_region(r2);{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,
_tmp8B);struct Cyc_List_List*_tmpF4=Cyc_Tcenv_lookup_type_vars(te2);struct _tuple4
_tmpF5=({struct _tuple4 _tmp116;_tmp116.f1=_tmpF4;_tmp116.f2=rgn;_tmp116;});struct
Cyc_List_List*_tmpF6=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmpF5,_tmp88->tvs);struct
Cyc_List_List*_tmpF7=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,
rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp88->impl))->exist_vars,
_tmpD8);struct Cyc_List_List*_tmpF8=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmpF6);struct Cyc_List_List*_tmpF9=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmpF7);struct Cyc_List_List*_tmpFA=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmpF6,_tmpF7);
if(_tmp8B != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp88->impl))->rgn_po
!= 0){if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({struct _tuple6*
_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->f1=0;_tmpFB->f2=0;_tmpFB;});(*res.tvars_and_bounds_opt).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f1,_tmp8B);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
outlives_constraints);{struct Cyc_List_List*_tmpFC=0;{struct Cyc_List_List*_tmpFD=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp88->impl))->rgn_po;for(0;_tmpFD != 
0;_tmpFD=_tmpFD->tl){_tmpFC=({struct Cyc_List_List*_tmpFE=_cycalloc(sizeof(*
_tmpFE));_tmpFE->hd=({struct _tuple0*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF->f1=
Cyc_Tcutil_rsubstitute(rgn,_tmpFA,(*((struct _tuple0*)_tmpFD->hd)).f1);_tmpFF->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmpFA,(*((struct _tuple0*)_tmpFD->hd)).f2);_tmpFF;});
_tmpFE->tl=_tmpFC;_tmpFE;});}}_tmpFC=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmpFC);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
_tmpFC);}}*_tmp8A=_tmpF8;t=(void*)({struct Cyc_Absyn_AggrType_struct*_tmp100=
_cycalloc(sizeof(*_tmp100));_tmp100[0]=({struct Cyc_Absyn_AggrType_struct _tmp101;
_tmp101.tag=10;_tmp101.f1=({struct Cyc_Absyn_AggrInfo _tmp102;_tmp102.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp103=_cycalloc(sizeof(*
_tmp103));_tmp103[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp104;_tmp104.tag=1;
_tmp104.f1=({struct Cyc_Absyn_Aggrdecl**_tmp105=_cycalloc(sizeof(*_tmp105));
_tmp105[0]=_tmp88;_tmp105;});_tmp104;});_tmp103;}));_tmp102.targs=*_tmp8A;
_tmp102;});_tmp101;});_tmp100;});if(_tmp8E){int _tmp106=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmpD3);int _tmp107=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp88->impl))->fields);if(_tmp106 < 
_tmp107){struct Cyc_List_List*wild_dps=0;{int i=0;for(0;i < _tmp107 - _tmp106;i ++){
wild_dps=({struct Cyc_List_List*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->hd=({
struct _tuple8*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109->f1=0;_tmp109->f2=Cyc_Tcpat_wild_pat(
p->loc);_tmp109;});_tmp108->tl=wild_dps;_tmp108;});}}*_tmp8D=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpD3,
wild_dps);_tmpD3=*_tmp8D;}else{if(_tmp106 == _tmp107)({void*_tmp10A[0]={};Cyc_Tcutil_warn(
p->loc,({const char*_tmp10B="unnecessary ... in struct pattern";_tag_dynforward(
_tmp10B,sizeof(char),_get_zero_arr_size(_tmp10B,34));}),_tag_dynforward(_tmp10A,
sizeof(void*),0));});}}{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,p->loc,
_tmpD3,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp88->impl))->fields);for(0;
fields != 0;fields=fields->tl){struct _tuple10 _tmp10D;struct Cyc_Absyn_Aggrfield*
_tmp10E;struct Cyc_Absyn_Pat*_tmp10F;struct _tuple10*_tmp10C=(struct _tuple10*)
fields->hd;_tmp10D=*_tmp10C;_tmp10E=_tmp10D.f1;_tmp10F=_tmp10D.f2;{void*_tmp110=
Cyc_Tcutil_rsubstitute(rgn,_tmpFA,(void*)_tmp10E->type);res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te2,_tmp10F,(void**)& _tmp110,rgn_opt));if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp10F->topt))->v,_tmp110))({struct Cyc_String_pa_struct
_tmp115;_tmp115.tag=0;_tmp115.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp10F->topt))->v));{
struct Cyc_String_pa_struct _tmp114;_tmp114.tag=0;_tmp114.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp110));{struct
Cyc_String_pa_struct _tmp113;_tmp113.tag=0;_tmp113.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*_tmp10E->name);{void*_tmp111[3]={& _tmp113,& _tmp114,&
_tmp115};Cyc_Tcutil_terr(p->loc,({const char*_tmp112="field %s of struct pattern expects type %s != %s";
_tag_dynforward(_tmp112,sizeof(char),_get_zero_arr_size(_tmp112,49));}),
_tag_dynforward(_tmp111,sizeof(void*),3));}}}});}}}};_pop_region(r2);}};
_pop_region(rgn);}goto _LL3C;}_LL5B: if(_tmp75 <= (void*)2)goto _LL5D;if(*((int*)
_tmp75)!= 6)goto _LL5D;_tmp8F=((struct Cyc_Absyn_Tunion_p_struct*)_tmp75)->f1;
_tmp90=((struct Cyc_Absyn_Tunion_p_struct*)_tmp75)->f2;_tmp91=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp75)->f3;_tmp92=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tunion_p_struct*)
_tmp75)->f3;_tmp93=((struct Cyc_Absyn_Tunion_p_struct*)_tmp75)->f4;_LL5C: {struct
Cyc_List_List*_tmp117=*_tmp92;if(_tmp8F->is_flat)rgn_opt=0;{struct _RegionHandle
_tmp118=_new_region("rgn");struct _RegionHandle*rgn=& _tmp118;_push_region(rgn);{
struct Cyc_List_List*tqts=_tmp90->typs;struct Cyc_List_List*_tmp119=Cyc_Tcenv_lookup_type_vars(
te);struct _tuple4 _tmp11A=({struct _tuple4 _tmp146;_tmp146.f1=_tmp119;_tmp146.f2=
rgn;_tmp146;});struct Cyc_List_List*_tmp11B=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp11A,_tmp8F->tvs);struct Cyc_List_List*_tmp11C=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp11B);if(tqts == 0  || _tmp8F->is_flat){struct Cyc_Core_Opt*ropt=0;
if(!_tmp8F->is_flat)ropt=({struct Cyc_Core_Opt*_tmp11D=_cycalloc(sizeof(*_tmp11D));
_tmp11D->v=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt*_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E->v=_tmp119;
_tmp11E;}));_tmp11D;});t=(void*)({struct Cyc_Absyn_TunionType_struct*_tmp11F=
_cycalloc(sizeof(*_tmp11F));_tmp11F[0]=({struct Cyc_Absyn_TunionType_struct
_tmp120;_tmp120.tag=2;_tmp120.f1=({struct Cyc_Absyn_TunionInfo _tmp121;_tmp121.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmp122=_cycalloc(sizeof(*
_tmp122));_tmp122[0]=({struct Cyc_Absyn_KnownTunion_struct _tmp123;_tmp123.tag=1;
_tmp123.f1=({struct Cyc_Absyn_Tuniondecl**_tmp124=_cycalloc(sizeof(*_tmp124));
_tmp124[0]=_tmp8F;_tmp124;});_tmp123;});_tmp122;}));_tmp121.targs=_tmp11C;
_tmp121.rgn=ropt;_tmp121;});_tmp120;});_tmp11F;});}else{t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp126;_tmp126.tag=3;_tmp126.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp127;
_tmp127.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp129;_tmp129.tag=1;_tmp129.f1=_tmp8F;_tmp129.f2=_tmp90;_tmp129;});_tmp128;}));
_tmp127.targs=_tmp11C;_tmp127;});_tmp126;});_tmp125;});}if(topt != 0  && (tqts == 0
 || _tmp8F->is_flat)){void*_tmp12A=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo
_tmp12B;struct Cyc_List_List*_tmp12C;_LL88: if(_tmp12A <= (void*)4)goto _LL8C;if(*((
int*)_tmp12A)!= 3)goto _LL8A;_LL89: t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp12E;_tmp12E.tag=3;_tmp12E.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp12F;
_tmp12F.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp131;_tmp131.tag=1;_tmp131.f1=_tmp8F;_tmp131.f2=_tmp90;_tmp131;});_tmp130;}));
_tmp12F.targs=_tmp11C;_tmp12F;});_tmp12E;});_tmp12D;});goto _LL87;_LL8A: if(*((int*)
_tmp12A)!= 2)goto _LL8C;_tmp12B=((struct Cyc_Absyn_TunionType_struct*)_tmp12A)->f1;
_tmp12C=_tmp12B.targs;_LL8B: {struct Cyc_List_List*_tmp132=_tmp11C;for(0;_tmp132
!= 0  && _tmp12C != 0;(_tmp132=_tmp132->tl,_tmp12C=_tmp12C->tl)){Cyc_Tcutil_unify((
void*)_tmp132->hd,(void*)_tmp12C->hd);}goto _LL87;}_LL8C:;_LL8D: goto _LL87;_LL87:;}
if(_tmp93){int _tmp133=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp117);
int _tmp134=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);if(_tmp133 < 
_tmp134){struct Cyc_List_List*wild_ps=0;{int i=0;for(0;i < _tmp134 - _tmp133;i ++){
wild_ps=({struct Cyc_List_List*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->hd=
Cyc_Tcpat_wild_pat(p->loc);_tmp135->tl=wild_ps;_tmp135;});}}*_tmp92=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp117,
wild_ps);_tmp117=*_tmp92;}else{if(_tmp133 == _tmp134)({struct Cyc_String_pa_struct
_tmp138;_tmp138.tag=0;_tmp138.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp8F->name));{void*_tmp136[1]={& _tmp138};Cyc_Tcutil_warn(
p->loc,({const char*_tmp137="unnecessary ... in tunion field %s";_tag_dynforward(
_tmp137,sizeof(char),_get_zero_arr_size(_tmp137,35));}),_tag_dynforward(_tmp136,
sizeof(void*),1));}});}}for(0;_tmp117 != 0  && tqts != 0;(_tmp117=_tmp117->tl,tqts=
tqts->tl)){struct Cyc_Absyn_Pat*_tmp139=(struct Cyc_Absyn_Pat*)_tmp117->hd;void*
_tmp13A=Cyc_Tcutil_rsubstitute(rgn,_tmp11B,(*((struct _tuple9*)tqts->hd)).f2);res=
Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp139,(void**)& _tmp13A,
rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmp139->topt))->v,
_tmp13A))({struct Cyc_String_pa_struct _tmp13F;_tmp13F.tag=0;_tmp13F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp139->topt))->v));{struct Cyc_String_pa_struct
_tmp13E;_tmp13E.tag=0;_tmp13E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(_tmp13A));{struct Cyc_String_pa_struct _tmp13D;_tmp13D.tag=
0;_tmp13D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp90->name));{void*_tmp13B[3]={& _tmp13D,& _tmp13E,& _tmp13F};Cyc_Tcutil_terr(
_tmp139->loc,({const char*_tmp13C="%s expects argument type %s, not %s";
_tag_dynforward(_tmp13C,sizeof(char),_get_zero_arr_size(_tmp13C,36));}),
_tag_dynforward(_tmp13B,sizeof(void*),3));}}}});}if(_tmp117 != 0)({struct Cyc_String_pa_struct
_tmp142;_tmp142.tag=0;_tmp142.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp90->name));{void*_tmp140[1]={& _tmp142};Cyc_Tcutil_terr(
p->loc,({const char*_tmp141="too many arguments for tunion constructor %s";
_tag_dynforward(_tmp141,sizeof(char),_get_zero_arr_size(_tmp141,45));}),
_tag_dynforward(_tmp140,sizeof(void*),1));}});if(tqts != 0)({struct Cyc_String_pa_struct
_tmp145;_tmp145.tag=0;_tmp145.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp90->name));{void*_tmp143[1]={& _tmp145};Cyc_Tcutil_terr(
p->loc,({const char*_tmp144="too few arguments for tunion constructor %s";
_tag_dynforward(_tmp144,sizeof(char),_get_zero_arr_size(_tmp144,44));}),
_tag_dynforward(_tmp143,sizeof(void*),1));}});};_pop_region(rgn);}goto _LL3C;}
_LL5D: if(_tmp75 <= (void*)2)goto _LL5F;if(*((int*)_tmp75)!= 5)goto _LL5F;_tmp94=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp75)->f1;_tmp95=(void*)_tmp94.aggr_info;if(*((
int*)_tmp95)!= 0)goto _LL5F;_LL5E: goto _LL60;_LL5F: if(_tmp75 <= (void*)2)goto _LL61;
if(*((int*)_tmp75)!= 12)goto _LL61;_LL60: goto _LL62;_LL61: if(_tmp75 <= (void*)2)
goto _LL63;if(*((int*)_tmp75)!= 14)goto _LL63;_LL62: goto _LL64;_LL63: if(_tmp75 <= (
void*)2)goto _LL3C;if(*((int*)_tmp75)!= 13)goto _LL3C;_LL64: t=Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp147;}));goto _LL3C;_LL3C:;}tcpat_end: p->topt=({struct Cyc_Core_Opt*_tmp148=
_cycalloc(sizeof(*_tmp148));_tmp148->v=(void*)t;_tmp148;});return res;}}struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){struct
Cyc_Tcpat_TcPatResult _tmp14A=Cyc_Tcpat_tcPatRec(te,p,topt,0);{struct
_RegionHandle _tmp14B=_new_region("r");struct _RegionHandle*r=& _tmp14B;
_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dynforward_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*
x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp14A.patvars),p->loc,({const char*
_tmp14C="pattern contains a repeated variable";_tag_dynforward(_tmp14C,sizeof(
char),_get_zero_arr_size(_tmp14C,37));}));;_pop_region(r);}return _tmp14A;}void
Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){void*
_tmp14D=(void*)p->r;struct Cyc_Absyn_Pat*_tmp14E;struct Cyc_List_List*_tmp14F;
struct Cyc_List_List*_tmp150;struct Cyc_List_List*_tmp151;_LL8F: if(_tmp14D <= (void*)
2)goto _LL97;if(*((int*)_tmp14D)!= 4)goto _LL91;_tmp14E=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp14D)->f1;_LL90: Cyc_Tcpat_check_pat_regions(te,_tmp14E);{void*_tmp152=(void*)((
struct Cyc_Core_Opt*)_check_null(p->topt))->v;struct Cyc_Absyn_PtrInfo _tmp153;
struct Cyc_Absyn_PtrAtts _tmp154;void*_tmp155;struct Cyc_Absyn_TunionInfo _tmp156;
struct Cyc_Core_Opt*_tmp157;struct Cyc_Core_Opt _tmp158;void*_tmp159;_LL9A: if(
_tmp152 <= (void*)4)goto _LL9E;if(*((int*)_tmp152)!= 4)goto _LL9C;_tmp153=((struct
Cyc_Absyn_PointerType_struct*)_tmp152)->f1;_tmp154=_tmp153.ptr_atts;_tmp155=(
void*)_tmp154.rgn;_LL9B: _tmp159=_tmp155;goto _LL9D;_LL9C: if(*((int*)_tmp152)!= 2)
goto _LL9E;_tmp156=((struct Cyc_Absyn_TunionType_struct*)_tmp152)->f1;_tmp157=
_tmp156.rgn;if(_tmp157 == 0)goto _LL9E;_tmp158=*_tmp157;_tmp159=(void*)_tmp158.v;
_LL9D: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp159);return;_LL9E:;_LL9F:({
void*_tmp15A[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp15B="check_pat_regions: bad pointer type";
_tag_dynforward(_tmp15B,sizeof(char),_get_zero_arr_size(_tmp15B,36));}),
_tag_dynforward(_tmp15A,sizeof(void*),0));});_LL99:;}_LL91: if(*((int*)_tmp14D)!= 
6)goto _LL93;_tmp14F=((struct Cyc_Absyn_Tunion_p_struct*)_tmp14D)->f3;_LL92: for(0;
_tmp14F != 0;_tmp14F=_tmp14F->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp14F->hd);}{void*_tmp15C=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_TunionInfo _tmp15D;struct Cyc_Core_Opt*_tmp15E;struct Cyc_Absyn_TunionInfo
_tmp15F;struct Cyc_Core_Opt*_tmp160;struct Cyc_Core_Opt _tmp161;void*_tmp162;_LLA1:
if(_tmp15C <= (void*)4)goto _LLA7;if(*((int*)_tmp15C)!= 2)goto _LLA3;_tmp15D=((
struct Cyc_Absyn_TunionType_struct*)_tmp15C)->f1;_tmp15E=_tmp15D.rgn;if(_tmp15E != 
0)goto _LLA3;_LLA2: return;_LLA3: if(*((int*)_tmp15C)!= 2)goto _LLA5;_tmp15F=((
struct Cyc_Absyn_TunionType_struct*)_tmp15C)->f1;_tmp160=_tmp15F.rgn;if(_tmp160 == 
0)goto _LLA5;_tmp161=*_tmp160;_tmp162=(void*)_tmp161.v;_LLA4: Cyc_Tcenv_check_rgn_accessible(
te,p->loc,_tmp162);return;_LLA5: if(*((int*)_tmp15C)!= 3)goto _LLA7;_LLA6: return;
_LLA7:;_LLA8:({void*_tmp163[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp164="check_pat_regions: bad tunion type";
_tag_dynforward(_tmp164,sizeof(char),_get_zero_arr_size(_tmp164,35));}),
_tag_dynforward(_tmp163,sizeof(void*),0));});_LLA0:;}_LL93: if(*((int*)_tmp14D)!= 
5)goto _LL95;_tmp150=((struct Cyc_Absyn_Aggr_p_struct*)_tmp14D)->f3;_LL94: for(0;
_tmp150 != 0;_tmp150=_tmp150->tl){Cyc_Tcpat_check_pat_regions(te,(*((struct
_tuple8*)_tmp150->hd)).f2);}return;_LL95: if(*((int*)_tmp14D)!= 3)goto _LL97;
_tmp151=((struct Cyc_Absyn_Tuple_p_struct*)_tmp14D)->f1;_LL96: for(0;_tmp151 != 0;
_tmp151=_tmp151->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp151->hd);}return;_LL97:;_LL98: return;_LL8E:;}struct Cyc_Tcpat_Name_v_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Tcpat_Int_v_struct{int tag;int f1;};
struct Cyc_Tcpat_Con_s{void*name;int arity;struct Cyc_Core_Opt*span;struct Cyc_Absyn_Pat*
orig_pat;};struct Cyc_Tcpat_Con_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*
f2;};static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*
c2){void*_tmp165=(void*)c1->name;struct _dynforward_ptr _tmp166;int _tmp167;_LLAA:
if(*((int*)_tmp165)!= 0)goto _LLAC;_tmp166=((struct Cyc_Tcpat_Name_v_struct*)
_tmp165)->f1;_LLAB: {void*_tmp168=(void*)c2->name;struct _dynforward_ptr _tmp169;
_LLAF: if(*((int*)_tmp168)!= 0)goto _LLB1;_tmp169=((struct Cyc_Tcpat_Name_v_struct*)
_tmp168)->f1;_LLB0: return Cyc_strcmp((struct _dynforward_ptr)_tmp166,(struct
_dynforward_ptr)_tmp169);_LLB1: if(*((int*)_tmp168)!= 1)goto _LLAE;_LLB2: return - 1;
_LLAE:;}_LLAC: if(*((int*)_tmp165)!= 1)goto _LLA9;_tmp167=((struct Cyc_Tcpat_Int_v_struct*)
_tmp165)->f1;_LLAD: {void*_tmp16A=(void*)c2->name;int _tmp16B;_LLB4: if(*((int*)
_tmp16A)!= 0)goto _LLB6;_LLB5: return 1;_LLB6: if(*((int*)_tmp16A)!= 1)goto _LLB3;
_tmp16B=((struct Cyc_Tcpat_Int_v_struct*)_tmp16A)->f1;_LLB7: return _tmp167 - 
_tmp16B;_LLB3:;}_LLA9:;}static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct
_RegionHandle*r){return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(
struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={(void*)1};static struct Cyc_Core_Opt Cyc_Tcpat_two_opt={(
void*)2};static struct Cyc_Core_Opt*Cyc_Tcpat_one_opt_ptr=(struct Cyc_Core_Opt*)&
Cyc_Tcpat_one_opt;static char _tmp16D[5]="NULL";static struct Cyc_Tcpat_Name_v_struct
Cyc_Tcpat_null_name_value={0,{_tmp16D,_tmp16D + 5}};static char _tmp16F[2]="&";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={0,{_tmp16F,_tmp16F + 
2}};static char _tmp171[2]="$";static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={
0,{_tmp171,_tmp171 + 2}};static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct Cyc_Absyn_Pat*
p){return({struct Cyc_Tcpat_Con_s*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172->name=(
void*)((void*)& Cyc_Tcpat_null_name_value);_tmp172->arity=0;_tmp172->span=(struct
Cyc_Core_Opt*)& Cyc_Tcpat_two_opt;_tmp172->orig_pat=(struct Cyc_Absyn_Pat*)p;
_tmp172;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct Cyc_Absyn_Pat*
p){return({struct Cyc_Tcpat_Con_s*_tmp173=_cycalloc(sizeof(*_tmp173));_tmp173->name=(
void*)((void*)& Cyc_Tcpat_ptr_name_value);_tmp173->arity=1;_tmp173->span=(struct
Cyc_Core_Opt*)& Cyc_Tcpat_two_opt;_tmp173->orig_pat=(struct Cyc_Absyn_Pat*)p;
_tmp173;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->name=(
void*)((void*)& Cyc_Tcpat_ptr_name_value);_tmp174->arity=1;_tmp174->span=(struct
Cyc_Core_Opt*)& Cyc_Tcpat_one_opt;_tmp174->orig_pat=(struct Cyc_Absyn_Pat*)p;
_tmp174;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i,struct Cyc_Absyn_Pat*
p){return({struct Cyc_Tcpat_Con_s*_tmp175=_cycalloc(sizeof(*_tmp175));_tmp175->name=(
void*)((void*)({struct Cyc_Tcpat_Int_v_struct*_tmp176=_cycalloc_atomic(sizeof(*
_tmp176));_tmp176[0]=({struct Cyc_Tcpat_Int_v_struct _tmp177;_tmp177.tag=1;_tmp177.f1=
i;_tmp177;});_tmp176;}));_tmp175->arity=0;_tmp175->span=0;_tmp175->orig_pat=p;
_tmp175;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct
_dynforward_ptr f,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*_tmp178=
_cycalloc(sizeof(*_tmp178));_tmp178->name=(void*)((void*)({struct Cyc_Tcpat_Name_v_struct*
_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179[0]=({struct Cyc_Tcpat_Name_v_struct
_tmp17A;_tmp17A.tag=0;_tmp17A.f1=f;_tmp17A;});_tmp179;}));_tmp178->arity=0;
_tmp178->span=0;_tmp178->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp178;});}static
struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){return({
struct Cyc_Tcpat_Con_s*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->name=(void*)((
void*)({struct Cyc_Tcpat_Int_v_struct*_tmp17D=_cycalloc_atomic(sizeof(*_tmp17D));
_tmp17D[0]=({struct Cyc_Tcpat_Int_v_struct _tmp17E;_tmp17E.tag=1;_tmp17E.f1=(int)c;
_tmp17E;});_tmp17D;}));_tmp17B->arity=0;_tmp17B->span=({struct Cyc_Core_Opt*
_tmp17C=_cycalloc_atomic(sizeof(*_tmp17C));_tmp17C->v=(void*)256;_tmp17C;});
_tmp17B->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp17B;});}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_tuple_con(int i,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*
_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F->name=(void*)((void*)& Cyc_Tcpat_tuple_name_value);
_tmp17F->arity=i;_tmp17F->span=Cyc_Tcpat_one_opt_ptr;_tmp17F->orig_pat=p;_tmp17F;});}
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180[0]=({struct Cyc_Tcpat_Con_struct
_tmp181;_tmp181.tag=0;_tmp181.f1=Cyc_Tcpat_null_con(p);_tmp181.f2=0;_tmp181;});
_tmp180;});}static void*Cyc_Tcpat_int_pat(int i,struct Cyc_Absyn_Pat*p){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp182=_cycalloc(sizeof(*_tmp182));_tmp182[0]=({
struct Cyc_Tcpat_Con_struct _tmp183;_tmp183.tag=0;_tmp183.f1=Cyc_Tcpat_int_con(i,p);
_tmp183.f2=0;_tmp183;});_tmp182;});}static void*Cyc_Tcpat_char_pat(char c,struct
Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp184=_cycalloc(
sizeof(*_tmp184));_tmp184[0]=({struct Cyc_Tcpat_Con_struct _tmp185;_tmp185.tag=0;
_tmp185.f1=Cyc_Tcpat_char_con(c,p);_tmp185.f2=0;_tmp185;});_tmp184;});}static
void*Cyc_Tcpat_float_pat(struct _dynforward_ptr f,struct Cyc_Absyn_Pat*p){return(
void*)({struct Cyc_Tcpat_Con_struct*_tmp186=_cycalloc(sizeof(*_tmp186));_tmp186[0]=({
struct Cyc_Tcpat_Con_struct _tmp187;_tmp187.tag=0;_tmp187.f1=Cyc_Tcpat_float_con(f,
p);_tmp187.f2=0;_tmp187;});_tmp186;});}static void*Cyc_Tcpat_null_ptr_pat(void*p,
struct Cyc_Absyn_Pat*p0){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp188=
_cycalloc(sizeof(*_tmp188));_tmp188[0]=({struct Cyc_Tcpat_Con_struct _tmp189;
_tmp189.tag=0;_tmp189.f1=Cyc_Tcpat_null_ptr_con(p0);_tmp189.f2=({struct Cyc_List_List*
_tmp18A=_cycalloc(sizeof(*_tmp18A));_tmp18A->hd=(void*)p;_tmp18A->tl=0;_tmp18A;});
_tmp189;});_tmp188;});}static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*
p0){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp18B=_cycalloc(sizeof(*_tmp18B));
_tmp18B[0]=({struct Cyc_Tcpat_Con_struct _tmp18C;_tmp18C.tag=0;_tmp18C.f1=Cyc_Tcpat_ptr_con(
p0);_tmp18C.f2=({struct Cyc_List_List*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->hd=(
void*)p;_tmp18D->tl=0;_tmp18D;});_tmp18C;});_tmp18B;});}static void*Cyc_Tcpat_tuple_pat(
struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E[0]=({struct Cyc_Tcpat_Con_struct
_tmp18F;_tmp18F.tag=0;_tmp18F.f1=Cyc_Tcpat_tuple_con(Cyc_List_length(ss),p);
_tmp18F.f2=ss;_tmp18F;});_tmp18E;});}static void*Cyc_Tcpat_con_pat(struct
_dynforward_ptr con_name,struct Cyc_Core_Opt*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*
p){struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*_tmp192=_cycalloc(sizeof(*
_tmp192));_tmp192->name=(void*)((void*)({struct Cyc_Tcpat_Name_v_struct*_tmp193=
_cycalloc(sizeof(*_tmp193));_tmp193[0]=({struct Cyc_Tcpat_Name_v_struct _tmp194;
_tmp194.tag=0;_tmp194.f1=con_name;_tmp194;});_tmp193;}));_tmp192->arity=Cyc_List_length(
ps);_tmp192->span=span;_tmp192->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp192;});
return(void*)({struct Cyc_Tcpat_Con_struct*_tmp190=_cycalloc(sizeof(*_tmp190));
_tmp190[0]=({struct Cyc_Tcpat_Con_struct _tmp191;_tmp191.tag=0;_tmp191.f1=c;
_tmp191.f2=ps;_tmp191;});_tmp190;});}static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*
p){void*s;{void*_tmp195=(void*)p->r;void*_tmp196;int _tmp197;char _tmp198;struct
_dynforward_ptr _tmp199;struct Cyc_Absyn_Pat*_tmp19A;struct Cyc_Absyn_Pat*_tmp19B;
struct Cyc_Absyn_Pat*_tmp19C;struct Cyc_Absyn_Tuniondecl*_tmp19D;struct Cyc_Absyn_Tunionfield*
_tmp19E;struct Cyc_List_List*_tmp19F;struct Cyc_List_List*_tmp1A0;struct Cyc_Absyn_AggrInfo
_tmp1A1;void*_tmp1A2;struct Cyc_Absyn_Aggrdecl**_tmp1A3;struct Cyc_Absyn_Aggrdecl*
_tmp1A4;struct Cyc_List_List*_tmp1A5;struct Cyc_Absyn_Enumdecl*_tmp1A6;struct Cyc_Absyn_Enumfield*
_tmp1A7;void*_tmp1A8;struct Cyc_Absyn_Enumfield*_tmp1A9;_LLB9: if((int)_tmp195 != 0)
goto _LLBB;_LLBA: goto _LLBC;_LLBB: if(_tmp195 <= (void*)2)goto _LLBD;if(*((int*)
_tmp195)!= 2)goto _LLBD;_LLBC: s=(void*)0;goto _LLB8;_LLBD: if((int)_tmp195 != 1)goto
_LLBF;_LLBE: s=Cyc_Tcpat_null_pat(p);goto _LLB8;_LLBF: if(_tmp195 <= (void*)2)goto
_LLC1;if(*((int*)_tmp195)!= 7)goto _LLC1;_tmp196=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp195)->f1;_tmp197=((struct Cyc_Absyn_Int_p_struct*)_tmp195)->f2;_LLC0: s=Cyc_Tcpat_int_pat(
_tmp197,(struct Cyc_Absyn_Pat*)p);goto _LLB8;_LLC1: if(_tmp195 <= (void*)2)goto _LLC3;
if(*((int*)_tmp195)!= 8)goto _LLC3;_tmp198=((struct Cyc_Absyn_Char_p_struct*)
_tmp195)->f1;_LLC2: s=Cyc_Tcpat_char_pat(_tmp198,p);goto _LLB8;_LLC3: if(_tmp195 <= (
void*)2)goto _LLC5;if(*((int*)_tmp195)!= 9)goto _LLC5;_tmp199=((struct Cyc_Absyn_Float_p_struct*)
_tmp195)->f1;_LLC4: s=Cyc_Tcpat_float_pat(_tmp199,p);goto _LLB8;_LLC5: if(_tmp195 <= (
void*)2)goto _LLC7;if(*((int*)_tmp195)!= 0)goto _LLC7;_tmp19A=((struct Cyc_Absyn_Var_p_struct*)
_tmp195)->f2;_LLC6: s=Cyc_Tcpat_compile_pat(_tmp19A);goto _LLB8;_LLC7: if(_tmp195 <= (
void*)2)goto _LLC9;if(*((int*)_tmp195)!= 1)goto _LLC9;_tmp19B=((struct Cyc_Absyn_Reference_p_struct*)
_tmp195)->f2;_LLC8: s=Cyc_Tcpat_compile_pat(_tmp19B);goto _LLB8;_LLC9: if(_tmp195 <= (
void*)2)goto _LLCB;if(*((int*)_tmp195)!= 4)goto _LLCB;_tmp19C=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp195)->f1;_LLCA:{void*_tmp1AA=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v);struct Cyc_Absyn_PtrInfo _tmp1AB;struct Cyc_Absyn_PtrAtts
_tmp1AC;struct Cyc_Absyn_Conref*_tmp1AD;struct Cyc_Absyn_TunionInfo _tmp1AE;struct
Cyc_Core_Opt*_tmp1AF;struct Cyc_Core_Opt _tmp1B0;_LLDE: if(_tmp1AA <= (void*)4)goto
_LLE2;if(*((int*)_tmp1AA)!= 4)goto _LLE0;_tmp1AB=((struct Cyc_Absyn_PointerType_struct*)
_tmp1AA)->f1;_tmp1AC=_tmp1AB.ptr_atts;_tmp1AD=_tmp1AC.nullable;_LLDF: {int
is_nullable=0;int still_working=1;while(still_working){void*_tmp1B1=(void*)
_tmp1AD->v;struct Cyc_Absyn_Conref*_tmp1B2;int _tmp1B3;_LLE5: if(_tmp1B1 <= (void*)1)
goto _LLE7;if(*((int*)_tmp1B1)!= 1)goto _LLE7;_tmp1B2=((struct Cyc_Absyn_Forward_constr_struct*)
_tmp1B1)->f1;_LLE6:(void*)(_tmp1AD->v=(void*)((void*)_tmp1B2->v));continue;_LLE7:
if((int)_tmp1B1 != 0)goto _LLE9;_LLE8:(void*)(_tmp1AD->v=(void*)((void*)({struct
Cyc_Absyn_Eq_constr_struct*_tmp1B4=_cycalloc(sizeof(*_tmp1B4));_tmp1B4[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp1B5;_tmp1B5.tag=0;_tmp1B5.f1=(void*)0;
_tmp1B5;});_tmp1B4;})));is_nullable=0;still_working=0;goto _LLE4;_LLE9: if(_tmp1B1
<= (void*)1)goto _LLE4;if(*((int*)_tmp1B1)!= 0)goto _LLE4;_tmp1B3=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp1B1)->f1;_LLEA: is_nullable=(int)_tmp1B3;still_working=0;goto _LLE4;_LLE4:;}{
void*ss=Cyc_Tcpat_compile_pat(_tmp19C);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(
ss,p);else{s=Cyc_Tcpat_ptr_pat(ss,p);}goto _LLDD;}}_LLE0: if(*((int*)_tmp1AA)!= 2)
goto _LLE2;_tmp1AE=((struct Cyc_Absyn_TunionType_struct*)_tmp1AA)->f1;_tmp1AF=
_tmp1AE.rgn;if(_tmp1AF == 0)goto _LLE2;_tmp1B0=*_tmp1AF;_LLE1:{void*_tmp1B6=(void*)
_tmp19C->r;struct Cyc_Absyn_Tuniondecl*_tmp1B7;struct Cyc_Absyn_Tunionfield*
_tmp1B8;struct Cyc_List_List*_tmp1B9;_LLEC: if(_tmp1B6 <= (void*)2)goto _LLEE;if(*((
int*)_tmp1B6)!= 6)goto _LLEE;_tmp1B7=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1B6)->f1;
_tmp1B8=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1B6)->f2;_tmp1B9=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp1B6)->f3;_LLED: {struct Cyc_Core_Opt*span;if(_tmp1B7->is_xtunion)span=0;else{
span=({struct Cyc_Core_Opt*_tmp1BA=_cycalloc_atomic(sizeof(*_tmp1BA));_tmp1BA->v=(
void*)((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1B7->fields))->v);_tmp1BA;});}s=Cyc_Tcpat_con_pat(*(*
_tmp1B8->name).f2,span,((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp1B9),p);goto _LLEB;}
_LLEE:;_LLEF:({void*_tmp1BB[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1BC="non-[x]tunion pattern has tunion type";
_tag_dynforward(_tmp1BC,sizeof(char),_get_zero_arr_size(_tmp1BC,38));}),
_tag_dynforward(_tmp1BB,sizeof(void*),0));});_LLEB:;}goto _LLDD;_LLE2:;_LLE3:({
void*_tmp1BD[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1BE="pointer pattern does not have pointer type";
_tag_dynforward(_tmp1BE,sizeof(char),_get_zero_arr_size(_tmp1BE,43));}),
_tag_dynforward(_tmp1BD,sizeof(void*),0));});_LLDD:;}goto _LLB8;_LLCB: if(_tmp195
<= (void*)2)goto _LLCD;if(*((int*)_tmp195)!= 6)goto _LLCD;_tmp19D=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp195)->f1;_tmp19E=((struct Cyc_Absyn_Tunion_p_struct*)_tmp195)->f2;_tmp19F=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp195)->f3;_LLCC: {struct Cyc_Core_Opt*span;{
void*_tmp1BF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);
_LLF1: if(_tmp1BF <= (void*)4)goto _LLF5;if(*((int*)_tmp1BF)!= 2)goto _LLF3;_LLF2:
if(_tmp19D->is_xtunion)span=0;else{span=({struct Cyc_Core_Opt*_tmp1C0=
_cycalloc_atomic(sizeof(*_tmp1C0));_tmp1C0->v=(void*)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp19D->fields))->v);_tmp1C0;});}goto _LLF0;_LLF3: if(*((int*)_tmp1BF)!= 3)goto
_LLF5;_LLF4: span=({struct Cyc_Core_Opt*_tmp1C1=_cycalloc_atomic(sizeof(*_tmp1C1));
_tmp1C1->v=(void*)1;_tmp1C1;});goto _LLF0;_LLF5:;_LLF6: span=({void*_tmp1C2[0]={};((
struct Cyc_Core_Opt*(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1C3="void tunion pattern has bad type";_tag_dynforward(_tmp1C3,
sizeof(char),_get_zero_arr_size(_tmp1C3,33));}),_tag_dynforward(_tmp1C2,sizeof(
void*),0));});goto _LLF0;_LLF0:;}s=Cyc_Tcpat_con_pat(*(*_tmp19E->name).f2,span,((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp19F),p);goto _LLB8;}_LLCD: if(_tmp195 <= (
void*)2)goto _LLCF;if(*((int*)_tmp195)!= 3)goto _LLCF;_tmp1A0=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp195)->f1;_LLCE: s=Cyc_Tcpat_tuple_pat(((struct Cyc_List_List*(*)(void*(*f)(
struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,
_tmp1A0),(struct Cyc_Absyn_Pat*)p);goto _LLB8;_LLCF: if(_tmp195 <= (void*)2)goto
_LLD1;if(*((int*)_tmp195)!= 5)goto _LLD1;_tmp1A1=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp195)->f1;_tmp1A2=(void*)_tmp1A1.aggr_info;if(*((int*)_tmp1A2)!= 1)goto _LLD1;
_tmp1A3=((struct Cyc_Absyn_KnownAggr_struct*)_tmp1A2)->f1;_tmp1A4=*_tmp1A3;
_tmp1A5=((struct Cyc_Absyn_Aggr_p_struct*)_tmp195)->f3;_LLD0: {struct Cyc_List_List*
ps=0;{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp1A4->impl))->fields;for(0;fields != 0;fields=fields->tl){int found=Cyc_strcmp((
struct _dynforward_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,({const
char*_tmp1D2="";_tag_dynforward(_tmp1D2,sizeof(char),_get_zero_arr_size(_tmp1D2,
1));}))== 0;{struct Cyc_List_List*dlps0=_tmp1A5;for(0;!found  && dlps0 != 0;dlps0=
dlps0->tl){struct _tuple8 _tmp1C5;struct Cyc_List_List*_tmp1C6;struct Cyc_Absyn_Pat*
_tmp1C7;struct _tuple8*_tmp1C4=(struct _tuple8*)dlps0->hd;_tmp1C5=*_tmp1C4;_tmp1C6=
_tmp1C5.f1;_tmp1C7=_tmp1C5.f2;{struct Cyc_List_List*_tmp1C8=_tmp1C6;struct Cyc_List_List
_tmp1C9;void*_tmp1CA;struct _dynforward_ptr*_tmp1CB;struct Cyc_List_List*_tmp1CC;
_LLF8: if(_tmp1C8 == 0)goto _LLFA;_tmp1C9=*_tmp1C8;_tmp1CA=(void*)_tmp1C9.hd;if(*((
int*)_tmp1CA)!= 1)goto _LLFA;_tmp1CB=((struct Cyc_Absyn_FieldName_struct*)_tmp1CA)->f1;
_tmp1CC=_tmp1C9.tl;if(_tmp1CC != 0)goto _LLFA;_LLF9: if(Cyc_strptrcmp(_tmp1CB,((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){ps=({struct Cyc_List_List*
_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->hd=(void*)Cyc_Tcpat_compile_pat(
_tmp1C7);_tmp1CD->tl=ps;_tmp1CD;});found=1;}goto _LLF7;_LLFA:;_LLFB:({void*
_tmp1CE[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1CF="bad designator(s)";_tag_dynforward(_tmp1CF,sizeof(char),
_get_zero_arr_size(_tmp1CF,18));}),_tag_dynforward(_tmp1CE,sizeof(void*),0));});
_LLF7:;}}}if(!found)({void*_tmp1D0[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1D1="bad designator";
_tag_dynforward(_tmp1D1,sizeof(char),_get_zero_arr_size(_tmp1D1,15));}),
_tag_dynforward(_tmp1D0,sizeof(void*),0));});}}s=Cyc_Tcpat_tuple_pat(ps,(struct
Cyc_Absyn_Pat*)p);goto _LLB8;}_LLD1: if(_tmp195 <= (void*)2)goto _LLD3;if(*((int*)
_tmp195)!= 10)goto _LLD3;_tmp1A6=((struct Cyc_Absyn_Enum_p_struct*)_tmp195)->f1;
_tmp1A7=((struct Cyc_Absyn_Enum_p_struct*)_tmp195)->f2;_LLD2: {int span=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1A6->fields))->v);s=Cyc_Tcpat_con_pat(*(*_tmp1A7->name).f2,({
struct Cyc_Core_Opt*_tmp1D3=_cycalloc_atomic(sizeof(*_tmp1D3));_tmp1D3->v=(void*)
span;_tmp1D3;}),0,p);goto _LLB8;}_LLD3: if(_tmp195 <= (void*)2)goto _LLD5;if(*((int*)
_tmp195)!= 11)goto _LLD5;_tmp1A8=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp195)->f1;_tmp1A9=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp195)->f2;_LLD4: {
struct Cyc_List_List*fields;{void*_tmp1D4=Cyc_Tcutil_compress(_tmp1A8);struct Cyc_List_List*
_tmp1D5;_LLFD: if(_tmp1D4 <= (void*)4)goto _LLFF;if(*((int*)_tmp1D4)!= 13)goto _LLFF;
_tmp1D5=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp1D4)->f1;_LLFE: fields=_tmp1D5;
goto _LLFC;_LLFF:;_LL100:({void*_tmp1D6[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1D7="bad type in AnonEnum_p";
_tag_dynforward(_tmp1D7,sizeof(char),_get_zero_arr_size(_tmp1D7,23));}),
_tag_dynforward(_tmp1D6,sizeof(void*),0));});_LLFC:;}{int span=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(fields);s=Cyc_Tcpat_con_pat(*(*_tmp1A9->name).f2,({struct Cyc_Core_Opt*
_tmp1D8=_cycalloc_atomic(sizeof(*_tmp1D8));_tmp1D8->v=(void*)span;_tmp1D8;}),0,p);
goto _LLB8;}}_LLD5: if(_tmp195 <= (void*)2)goto _LLD7;if(*((int*)_tmp195)!= 12)goto
_LLD7;_LLD6: goto _LLD8;_LLD7: if(_tmp195 <= (void*)2)goto _LLD9;if(*((int*)_tmp195)
!= 13)goto _LLD9;_LLD8: goto _LLDA;_LLD9: if(_tmp195 <= (void*)2)goto _LLDB;if(*((int*)
_tmp195)!= 5)goto _LLDB;_LLDA: goto _LLDC;_LLDB: if(_tmp195 <= (void*)2)goto _LLB8;if(*((
int*)_tmp195)!= 14)goto _LLB8;_LLDC: s=(void*)0;_LLB8:;}return s;}struct Cyc_Tcpat_Pos_struct{
int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{
int tag;struct Cyc_Set_Set*f1;};struct Cyc_Tcpat_Failure_struct{int tag;void*f1;};
struct Cyc_Tcpat_Success_struct{int tag;void*f1;};struct Cyc_Tcpat_IfEq_struct{int
tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};struct
_tuple11{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*
c){void*_tmp1D9=td;struct Cyc_Set_Set*_tmp1DA;_LL102: if(*((int*)_tmp1D9)!= 1)goto
_LL104;_tmp1DA=((struct Cyc_Tcpat_Neg_struct*)_tmp1D9)->f1;_LL103: if(((int(*)(
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp1DA,c))({void*
_tmp1DB[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1DC="add_neg called when constructor already in set";
_tag_dynforward(_tmp1DC,sizeof(char),_get_zero_arr_size(_tmp1DC,47));}),
_tag_dynforward(_tmp1DB,sizeof(void*),0));});if(c->span != 0  && ((int(*)(struct
Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp1DA)+ 1 >= (int)((struct Cyc_Core_Opt*)
_check_null(c->span))->v)({void*_tmp1DD[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1DE="add_neg called when |cs U {c}| >= span(c)";
_tag_dynforward(_tmp1DE,sizeof(char),_get_zero_arr_size(_tmp1DE,42));}),
_tag_dynforward(_tmp1DD,sizeof(void*),0));});return(void*)({struct Cyc_Tcpat_Neg_struct*
_tmp1DF=_region_malloc(r,sizeof(*_tmp1DF));_tmp1DF[0]=({struct Cyc_Tcpat_Neg_struct
_tmp1E0;_tmp1E0.tag=1;_tmp1E0.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp1DA,c);
_tmp1E0;});_tmp1DF;});_LL104: if(*((int*)_tmp1D9)!= 0)goto _LL101;_LL105:({void*
_tmp1E1[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1E2="add_neg called when td is Positive";_tag_dynforward(_tmp1E2,
sizeof(char),_get_zero_arr_size(_tmp1E2,35));}),_tag_dynforward(_tmp1E1,sizeof(
void*),0));});_LL101:;}static void*Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,
void*td){void*_tmp1E3=td;struct Cyc_Tcpat_Con_s*_tmp1E4;struct Cyc_Set_Set*_tmp1E5;
_LL107: if(*((int*)_tmp1E3)!= 0)goto _LL109;_tmp1E4=((struct Cyc_Tcpat_Pos_struct*)
_tmp1E3)->f1;_LL108: if(Cyc_Tcpat_compare_con(c,_tmp1E4)== 0)return(void*)0;else{
return(void*)1;}_LL109: if(*((int*)_tmp1E3)!= 1)goto _LL106;_tmp1E5=((struct Cyc_Tcpat_Neg_struct*)
_tmp1E3)->f1;_LL10A: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))
Cyc_Set_member)(_tmp1E5,c))return(void*)1;else{if(c->span != 0  && (int)((struct
Cyc_Core_Opt*)_check_null(c->span))->v == ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp1E5)+ 1)return(void*)0;else{return(void*)2;}}_LL106:;}struct _tuple12{struct
Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*Cyc_Tcpat_augment(
struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){struct Cyc_List_List*
_tmp1E6=ctxt;struct Cyc_List_List _tmp1E7;struct _tuple12*_tmp1E8;struct _tuple12
_tmp1E9;struct Cyc_Tcpat_Con_s*_tmp1EA;struct Cyc_List_List*_tmp1EB;struct Cyc_List_List*
_tmp1EC;_LL10C: if(_tmp1E6 != 0)goto _LL10E;_LL10D: return 0;_LL10E: if(_tmp1E6 == 0)
goto _LL10B;_tmp1E7=*_tmp1E6;_tmp1E8=(struct _tuple12*)_tmp1E7.hd;_tmp1E9=*_tmp1E8;
_tmp1EA=_tmp1E9.f1;_tmp1EB=_tmp1E9.f2;_tmp1EC=_tmp1E7.tl;_LL10F: return({struct
Cyc_List_List*_tmp1ED=_region_malloc(r,sizeof(*_tmp1ED));_tmp1ED->hd=({struct
_tuple12*_tmp1EE=_region_malloc(r,sizeof(*_tmp1EE));_tmp1EE->f1=_tmp1EA;_tmp1EE->f2=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp1EF=_region_malloc(r,sizeof(*
_tmp1EF));_tmp1EF->hd=(void*)dsc;_tmp1EF->tl=_tmp1EB;_tmp1EF;});_tmp1EE;});
_tmp1ED->tl=_tmp1EC;_tmp1ED;});_LL10B:;}static struct Cyc_List_List*Cyc_Tcpat_norm_context(
struct _RegionHandle*r,struct Cyc_List_List*ctxt){struct Cyc_List_List*_tmp1F0=ctxt;
struct Cyc_List_List _tmp1F1;struct _tuple12*_tmp1F2;struct _tuple12 _tmp1F3;struct
Cyc_Tcpat_Con_s*_tmp1F4;struct Cyc_List_List*_tmp1F5;struct Cyc_List_List*_tmp1F6;
_LL111: if(_tmp1F0 != 0)goto _LL113;_LL112:({void*_tmp1F7[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp1F8="norm_context: empty context";_tag_dynforward(_tmp1F8,sizeof(char),
_get_zero_arr_size(_tmp1F8,28));}),_tag_dynforward(_tmp1F7,sizeof(void*),0));});
_LL113: if(_tmp1F0 == 0)goto _LL110;_tmp1F1=*_tmp1F0;_tmp1F2=(struct _tuple12*)
_tmp1F1.hd;_tmp1F3=*_tmp1F2;_tmp1F4=_tmp1F3.f1;_tmp1F5=_tmp1F3.f2;_tmp1F6=
_tmp1F1.tl;_LL114: return Cyc_Tcpat_augment(r,_tmp1F6,(void*)({struct Cyc_Tcpat_Pos_struct*
_tmp1F9=_region_malloc(r,sizeof(*_tmp1F9));_tmp1F9[0]=({struct Cyc_Tcpat_Pos_struct
_tmp1FA;_tmp1FA.tag=0;_tmp1FA.f1=_tmp1F4;_tmp1FA.f2=Cyc_List_rrev(r,_tmp1F5);
_tmp1FA;});_tmp1F9;}));_LL110:;}static void*Cyc_Tcpat_build_desc(struct
_RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
struct _tuple6 _tmp1FC=({struct _tuple6 _tmp1FB;_tmp1FB.f1=ctxt;_tmp1FB.f2=work;
_tmp1FB;});struct Cyc_List_List*_tmp1FD;struct Cyc_List_List*_tmp1FE;struct Cyc_List_List*
_tmp1FF;struct Cyc_List_List*_tmp200;struct Cyc_List_List*_tmp201;struct Cyc_List_List
_tmp202;struct _tuple12*_tmp203;struct _tuple12 _tmp204;struct Cyc_Tcpat_Con_s*
_tmp205;struct Cyc_List_List*_tmp206;struct Cyc_List_List*_tmp207;struct Cyc_List_List*
_tmp208;struct Cyc_List_List _tmp209;struct _tuple11*_tmp20A;struct _tuple11 _tmp20B;
struct Cyc_List_List*_tmp20C;struct Cyc_List_List*_tmp20D;_LL116: _tmp1FD=_tmp1FC.f1;
if(_tmp1FD != 0)goto _LL118;_tmp1FE=_tmp1FC.f2;if(_tmp1FE != 0)goto _LL118;_LL117:
return dsc;_LL118: _tmp1FF=_tmp1FC.f1;if(_tmp1FF != 0)goto _LL11A;_LL119: goto _LL11B;
_LL11A: _tmp200=_tmp1FC.f2;if(_tmp200 != 0)goto _LL11C;_LL11B:({void*_tmp20E[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp20F="build_desc: ctxt and work don't match";_tag_dynforward(
_tmp20F,sizeof(char),_get_zero_arr_size(_tmp20F,38));}),_tag_dynforward(_tmp20E,
sizeof(void*),0));});_LL11C: _tmp201=_tmp1FC.f1;if(_tmp201 == 0)goto _LL115;_tmp202=*
_tmp201;_tmp203=(struct _tuple12*)_tmp202.hd;_tmp204=*_tmp203;_tmp205=_tmp204.f1;
_tmp206=_tmp204.f2;_tmp207=_tmp202.tl;_tmp208=_tmp1FC.f2;if(_tmp208 == 0)goto
_LL115;_tmp209=*_tmp208;_tmp20A=(struct _tuple11*)_tmp209.hd;_tmp20B=*_tmp20A;
_tmp20C=_tmp20B.f3;_tmp20D=_tmp209.tl;_LL11D: {struct Cyc_Tcpat_Pos_struct*
_tmp210=({struct Cyc_Tcpat_Pos_struct*_tmp211=_region_malloc(r,sizeof(*_tmp211));
_tmp211[0]=({struct Cyc_Tcpat_Pos_struct _tmp212;_tmp212.tag=0;_tmp212.f1=_tmp205;
_tmp212.f2=Cyc_List_rappend(r,Cyc_List_rrev(r,_tmp206),({struct Cyc_List_List*
_tmp213=_region_malloc(r,sizeof(*_tmp213));_tmp213->hd=(void*)dsc;_tmp213->tl=
_tmp20C;_tmp213;}));_tmp212;});_tmp211;});return Cyc_Tcpat_build_desc(r,_tmp207,(
void*)_tmp210,_tmp20D);}_LL115:;}static void*Cyc_Tcpat_match(struct _RegionHandle*,
void*,struct Cyc_List_List*,void*,struct Cyc_List_List*,struct Cyc_List_List*,void*,
struct Cyc_List_List*);static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*
dsc,struct Cyc_List_List*allmrules){struct Cyc_List_List*_tmp214=allmrules;struct
Cyc_List_List _tmp215;struct _tuple0*_tmp216;struct _tuple0 _tmp217;void*_tmp218;
void*_tmp219;struct Cyc_List_List*_tmp21A;_LL11F: if(_tmp214 != 0)goto _LL121;_LL120:
return(void*)({struct Cyc_Tcpat_Failure_struct*_tmp21B=_cycalloc(sizeof(*_tmp21B));
_tmp21B[0]=({struct Cyc_Tcpat_Failure_struct _tmp21C;_tmp21C.tag=0;_tmp21C.f1=(
void*)dsc;_tmp21C;});_tmp21B;});_LL121: if(_tmp214 == 0)goto _LL11E;_tmp215=*
_tmp214;_tmp216=(struct _tuple0*)_tmp215.hd;_tmp217=*_tmp216;_tmp218=_tmp217.f1;
_tmp219=_tmp217.f2;_tmp21A=_tmp215.tl;_LL122: return Cyc_Tcpat_match(r,_tmp218,0,
dsc,0,0,_tmp219,_tmp21A);_LL11E:;}static void*Cyc_Tcpat_match_compile(struct
_RegionHandle*r,struct Cyc_List_List*allmrules){return Cyc_Tcpat_or_match(r,(void*)({
struct Cyc_Tcpat_Neg_struct*_tmp21D=_region_malloc(r,sizeof(*_tmp21D));_tmp21D[0]=({
struct Cyc_Tcpat_Neg_struct _tmp21E;_tmp21E.tag=1;_tmp21E.f1=Cyc_Tcpat_empty_con_set(
r);_tmp21E;});_tmp21D;}),allmrules);}static void*Cyc_Tcpat_and_match(struct
_RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp21F=work;
struct Cyc_List_List _tmp220;struct _tuple11*_tmp221;struct _tuple11 _tmp222;struct
Cyc_List_List*_tmp223;struct Cyc_List_List*_tmp224;struct Cyc_List_List*_tmp225;
struct Cyc_List_List*_tmp226;struct Cyc_List_List _tmp227;struct _tuple11*_tmp228;
struct _tuple11 _tmp229;struct Cyc_List_List*_tmp22A;struct Cyc_List_List*_tmp22B;
struct Cyc_List_List*_tmp22C;struct Cyc_List_List*_tmp22D;_LL124: if(_tmp21F != 0)
goto _LL126;_LL125: return(void*)({struct Cyc_Tcpat_Success_struct*_tmp22E=
_region_malloc(r,sizeof(*_tmp22E));_tmp22E[0]=({struct Cyc_Tcpat_Success_struct
_tmp22F;_tmp22F.tag=1;_tmp22F.f1=(void*)right_hand_side;_tmp22F;});_tmp22E;});
_LL126: if(_tmp21F == 0)goto _LL128;_tmp220=*_tmp21F;_tmp221=(struct _tuple11*)
_tmp220.hd;_tmp222=*_tmp221;_tmp223=_tmp222.f1;if(_tmp223 != 0)goto _LL128;_tmp224=
_tmp222.f2;if(_tmp224 != 0)goto _LL128;_tmp225=_tmp222.f3;if(_tmp225 != 0)goto
_LL128;_tmp226=_tmp220.tl;_LL127: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp226,right_hand_side,rules);_LL128: if(_tmp21F == 0)goto _LL123;_tmp227=*
_tmp21F;_tmp228=(struct _tuple11*)_tmp227.hd;_tmp229=*_tmp228;_tmp22A=_tmp229.f1;
_tmp22B=_tmp229.f2;_tmp22C=_tmp229.f3;_tmp22D=_tmp227.tl;_LL129: if((_tmp22A == 0
 || _tmp22B == 0) || _tmp22C == 0)({void*_tmp230[0]={};Cyc_Tcutil_impos(({const
char*_tmp231="tcpat:and_match: malformed work frame";_tag_dynforward(_tmp231,
sizeof(char),_get_zero_arr_size(_tmp231,38));}),_tag_dynforward(_tmp230,sizeof(
void*),0));});{struct Cyc_List_List _tmp233;void*_tmp234;struct Cyc_List_List*
_tmp235;struct Cyc_List_List*_tmp232=(struct Cyc_List_List*)_tmp22A;_tmp233=*
_tmp232;_tmp234=(void*)_tmp233.hd;_tmp235=_tmp233.tl;{struct Cyc_List_List _tmp237;
struct Cyc_List_List*_tmp238;struct Cyc_List_List*_tmp239;struct Cyc_List_List*
_tmp236=(struct Cyc_List_List*)_tmp22B;_tmp237=*_tmp236;_tmp238=(struct Cyc_List_List*)
_tmp237.hd;_tmp239=_tmp237.tl;{struct Cyc_List_List _tmp23B;void*_tmp23C;struct Cyc_List_List*
_tmp23D;struct Cyc_List_List*_tmp23A=(struct Cyc_List_List*)_tmp22C;_tmp23B=*
_tmp23A;_tmp23C=(void*)_tmp23B.hd;_tmp23D=_tmp23B.tl;{struct _tuple11*_tmp23E=({
struct _tuple11*_tmp240=_region_malloc(r,sizeof(*_tmp240));_tmp240->f1=_tmp235;
_tmp240->f2=_tmp239;_tmp240->f3=_tmp23D;_tmp240;});return Cyc_Tcpat_match(r,
_tmp234,_tmp238,_tmp23C,ctx,({struct Cyc_List_List*_tmp23F=_region_malloc(r,
sizeof(*_tmp23F));_tmp23F->hd=_tmp23E;_tmp23F->tl=_tmp22D;_tmp23F;}),
right_hand_side,rules);}}}}_LL123:;}static struct Cyc_List_List*Cyc_Tcpat_getdargs(
struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){void*_tmp241=dsc;
struct Cyc_Set_Set*_tmp242;struct Cyc_List_List*_tmp243;_LL12B: if(*((int*)_tmp241)
!= 1)goto _LL12D;_tmp242=((struct Cyc_Tcpat_Neg_struct*)_tmp241)->f1;_LL12C: {void*
any=(void*)({struct Cyc_Tcpat_Neg_struct*_tmp246=_region_malloc(r,sizeof(*_tmp246));
_tmp246[0]=({struct Cyc_Tcpat_Neg_struct _tmp247;_tmp247.tag=1;_tmp247.f1=Cyc_Tcpat_empty_con_set(
r);_tmp247;});_tmp246;});struct Cyc_List_List*_tmp244=0;{int i=0;for(0;i < pcon->arity;
++ i){_tmp244=({struct Cyc_List_List*_tmp245=_region_malloc(r,sizeof(*_tmp245));
_tmp245->hd=(void*)any;_tmp245->tl=_tmp244;_tmp245;});}}return _tmp244;}_LL12D:
if(*((int*)_tmp241)!= 0)goto _LL12A;_tmp243=((struct Cyc_Tcpat_Pos_struct*)_tmp241)->f2;
_LL12E: return _tmp243;_LL12A:;}struct _tuple13{struct _RegionHandle*f1;struct Cyc_List_List*
f2;};static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple13*env,int i){struct
_tuple13 _tmp249;struct _RegionHandle*_tmp24A;struct Cyc_List_List*_tmp24B;struct
_tuple13*_tmp248=env;_tmp249=*_tmp248;_tmp24A=_tmp249.f1;_tmp24B=_tmp249.f2;
return({struct Cyc_List_List*_tmp24C=_region_malloc(_tmp24A,sizeof(*_tmp24C));
_tmp24C->hd=(void*)(i + 1);_tmp24C->tl=_tmp24B;_tmp24C;});}static struct Cyc_List_List*
Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*
obj){struct _tuple13 _tmp24D=({struct _tuple13 _tmp24E;_tmp24E.f1=r;_tmp24E.f2=obj;
_tmp24E;});return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*
f)(struct _tuple13*,int),struct _tuple13*env))Cyc_List_rtabulate_c)(r,pcon->arity,
Cyc_Tcpat_getoarg,& _tmp24D);}static void*Cyc_Tcpat_match(struct _RegionHandle*r,
void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp24F=p;struct Cyc_Tcpat_Con_s*
_tmp250;struct Cyc_List_List*_tmp251;_LL130: if((int)_tmp24F != 0)goto _LL132;_LL131:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL132: if(_tmp24F <= (void*)1)goto _LL12F;if(*((int*)_tmp24F)!= 0)goto
_LL12F;_tmp250=((struct Cyc_Tcpat_Con_struct*)_tmp24F)->f1;_tmp251=((struct Cyc_Tcpat_Con_struct*)
_tmp24F)->f2;_LL133: {void*_tmp252=Cyc_Tcpat_static_match(_tmp250,dsc);_LL135:
if((int)_tmp252 != 0)goto _LL137;_LL136: {struct Cyc_List_List*_tmp253=({struct Cyc_List_List*
_tmp258=_region_malloc(r,sizeof(*_tmp258));_tmp258->hd=({struct _tuple12*_tmp259=
_region_malloc(r,sizeof(*_tmp259));_tmp259->f1=_tmp250;_tmp259->f2=0;_tmp259;});
_tmp258->tl=ctx;_tmp258;});struct _tuple11*_tmp254=({struct _tuple11*_tmp257=
_region_malloc(r,sizeof(*_tmp257));_tmp257->f1=_tmp251;_tmp257->f2=Cyc_Tcpat_getoargs(
r,_tmp250,obj);_tmp257->f3=Cyc_Tcpat_getdargs(r,_tmp250,dsc);_tmp257;});struct
Cyc_List_List*_tmp255=({struct Cyc_List_List*_tmp256=_region_malloc(r,sizeof(*
_tmp256));_tmp256->hd=_tmp254;_tmp256->tl=work;_tmp256;});return Cyc_Tcpat_and_match(
r,_tmp253,_tmp255,right_hand_side,rules);}_LL137: if((int)_tmp252 != 1)goto _LL139;
_LL138: return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);
_LL139: if((int)_tmp252 != 2)goto _LL134;_LL13A: {struct Cyc_List_List*_tmp25A=({
struct Cyc_List_List*_tmp263=_region_malloc(r,sizeof(*_tmp263));_tmp263->hd=({
struct _tuple12*_tmp264=_region_malloc(r,sizeof(*_tmp264));_tmp264->f1=_tmp250;
_tmp264->f2=0;_tmp264;});_tmp263->tl=ctx;_tmp263;});struct _tuple11*_tmp25B=({
struct _tuple11*_tmp262=_region_malloc(r,sizeof(*_tmp262));_tmp262->f1=_tmp251;
_tmp262->f2=Cyc_Tcpat_getoargs(r,_tmp250,obj);_tmp262->f3=Cyc_Tcpat_getdargs(r,
_tmp250,dsc);_tmp262;});struct Cyc_List_List*_tmp25C=({struct Cyc_List_List*
_tmp261=_region_malloc(r,sizeof(*_tmp261));_tmp261->hd=_tmp25B;_tmp261->tl=work;
_tmp261;});void*_tmp25D=Cyc_Tcpat_and_match(r,_tmp25A,_tmp25C,right_hand_side,
rules);void*_tmp25E=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(
r,dsc,_tmp250),work),rules);return(void*)({struct Cyc_Tcpat_IfEq_struct*_tmp25F=
_region_malloc(r,sizeof(*_tmp25F));_tmp25F[0]=({struct Cyc_Tcpat_IfEq_struct
_tmp260;_tmp260.tag=2;_tmp260.f1=obj;_tmp260.f2=_tmp250;_tmp260.f3=(void*)
_tmp25D;_tmp260.f4=(void*)_tmp25E;_tmp260;});_tmp25F;});}_LL134:;}_LL12F:;}
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),
void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){void*
_tmp265=d;void*_tmp266;void*_tmp267;void*_tmp268;void*_tmp269;_LL13C: if(*((int*)
_tmp265)!= 0)goto _LL13E;_tmp266=(void*)((struct Cyc_Tcpat_Failure_struct*)_tmp265)->f1;
_LL13D: if(!(*exhaust_done)){not_exhaust(env1,_tmp266);*exhaust_done=1;}goto
_LL13B;_LL13E: if(*((int*)_tmp265)!= 1)goto _LL140;_tmp267=(void*)((struct Cyc_Tcpat_Success_struct*)
_tmp265)->f1;_LL13F: rhs_appears(env2,_tmp267);goto _LL13B;_LL140: if(*((int*)
_tmp265)!= 2)goto _LL13B;_tmp268=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp265)->f3;
_tmp269=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp265)->f4;_LL141: Cyc_Tcpat_check_exhaust_overlap(
_tmp268,not_exhaust,env1,rhs_appears,env2,exhaust_done);Cyc_Tcpat_check_exhaust_overlap(
_tmp269,not_exhaust,env1,rhs_appears,env2,exhaust_done);goto _LL13B;_LL13B:;}
struct _tuple14{int f1;struct Cyc_Position_Segment*f2;};struct _tuple15{void*f1;
struct _tuple14*f2;};struct _tuple16{struct _RegionHandle*f1;int*f2;};static struct
_tuple15*Cyc_Tcpat_get_match(struct _tuple16*env,struct Cyc_Absyn_Switch_clause*
swc){struct _tuple16 _tmp26B;struct _RegionHandle*_tmp26C;int*_tmp26D;struct
_tuple16*_tmp26A=env;_tmp26B=*_tmp26A;_tmp26C=_tmp26B.f1;_tmp26D=_tmp26B.f2;{
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);struct _tuple14*rhs=({struct _tuple14*
_tmp273=_region_malloc(_tmp26C,sizeof(*_tmp273));_tmp273->f1=0;_tmp273->f2=(swc->pattern)->loc;
_tmp273;});void*sp;if(swc->where_clause != 0){sp=Cyc_Tcpat_tuple_pat(({struct Cyc_List_List*
_tmp26E=_cycalloc(sizeof(*_tmp26E));_tmp26E->hd=(void*)sp0;_tmp26E->tl=({struct
Cyc_List_List*_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F->hd=(void*)Cyc_Tcpat_int_pat(*
_tmp26D,0);_tmp26F->tl=0;_tmp26F;});_tmp26E;}),0);*_tmp26D=*_tmp26D + 1;}else{sp=
Cyc_Tcpat_tuple_pat(({struct Cyc_List_List*_tmp270=_cycalloc(sizeof(*_tmp270));
_tmp270->hd=(void*)sp0;_tmp270->tl=({struct Cyc_List_List*_tmp271=_cycalloc(
sizeof(*_tmp271));_tmp271->hd=(void*)((void*)0);_tmp271->tl=0;_tmp271;});_tmp270;}),
0);}return({struct _tuple15*_tmp272=_region_malloc(_tmp26C,sizeof(*_tmp272));
_tmp272->f1=sp;_tmp272->f2=rhs;_tmp272;});}}void Cyc_Tcpat_add_con_to_list(struct
Cyc_Tcpat_Con_s*c,struct Cyc_List_List**cs){*cs=({struct Cyc_List_List*_tmp274=
_cycalloc(sizeof(*_tmp274));_tmp274->hd=c;_tmp274->tl=*cs;_tmp274;});}char Cyc_Tcpat_Desc2string[
16]="\000\000\000\000Desc2string\000";static struct _dynforward_ptr Cyc_Tcpat_descs2string(
struct Cyc_List_List*);static struct _dynforward_ptr Cyc_Tcpat_desc2string(void*d){
void*_tmp275=d;struct Cyc_Tcpat_Con_s*_tmp276;struct Cyc_List_List*_tmp277;struct
Cyc_Set_Set*_tmp278;_LL143: if(*((int*)_tmp275)!= 0)goto _LL145;_tmp276=((struct
Cyc_Tcpat_Pos_struct*)_tmp275)->f1;_tmp277=((struct Cyc_Tcpat_Pos_struct*)_tmp275)->f2;
_LL144: {void*_tmp279=(void*)_tmp276->name;struct Cyc_Absyn_Pat*_tmp27A=_tmp276->orig_pat;
if(_tmp27A == 0)return Cyc_Tcpat_desc2string((void*)((struct Cyc_List_List*)
_check_null(_tmp277))->hd);{void*_tmp27B=(void*)_tmp27A->r;struct Cyc_Absyn_Vardecl*
_tmp27C;struct Cyc_Absyn_Vardecl*_tmp27D;struct Cyc_Absyn_Tvar*_tmp27E;struct Cyc_Absyn_Vardecl*
_tmp27F;struct Cyc_Absyn_Pat*_tmp280;struct Cyc_Absyn_Pat _tmp281;void*_tmp282;
struct Cyc_Absyn_Tunionfield*_tmp283;struct Cyc_Absyn_AggrInfo _tmp284;void*_tmp285;
struct Cyc_Absyn_Aggrdecl**_tmp286;struct Cyc_Absyn_Aggrdecl*_tmp287;struct Cyc_Absyn_Tunionfield*
_tmp288;int _tmp289;char _tmp28A;struct _dynforward_ptr _tmp28B;struct Cyc_Absyn_Enumfield*
_tmp28C;struct Cyc_Absyn_Enumfield*_tmp28D;struct Cyc_Absyn_Exp*_tmp28E;_LL148: if((
int)_tmp27B != 0)goto _LL14A;_LL149: return({const char*_tmp28F="_";_tag_dynforward(
_tmp28F,sizeof(char),_get_zero_arr_size(_tmp28F,2));});_LL14A: if(_tmp27B <= (void*)
2)goto _LL15A;if(*((int*)_tmp27B)!= 0)goto _LL14C;_tmp27C=((struct Cyc_Absyn_Var_p_struct*)
_tmp27B)->f1;_LL14B: return Cyc_Absynpp_qvar2string(_tmp27C->name);_LL14C: if(*((
int*)_tmp27B)!= 1)goto _LL14E;_tmp27D=((struct Cyc_Absyn_Reference_p_struct*)
_tmp27B)->f1;_LL14D: return(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp292;_tmp292.tag=0;_tmp292.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp27D->name));{void*_tmp290[1]={& _tmp292};Cyc_aprintf(({
const char*_tmp291="*%s";_tag_dynforward(_tmp291,sizeof(char),_get_zero_arr_size(
_tmp291,4));}),_tag_dynforward(_tmp290,sizeof(void*),1));}});_LL14E: if(*((int*)
_tmp27B)!= 2)goto _LL150;_tmp27E=((struct Cyc_Absyn_TagInt_p_struct*)_tmp27B)->f1;
_tmp27F=((struct Cyc_Absyn_TagInt_p_struct*)_tmp27B)->f2;_LL14F: return(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp296;_tmp296.tag=0;_tmp296.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp27E->name);{struct Cyc_String_pa_struct
_tmp295;_tmp295.tag=0;_tmp295.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp27F->name));{void*_tmp293[2]={& _tmp295,& _tmp296};Cyc_aprintf(({
const char*_tmp294="%s<`%s>";_tag_dynforward(_tmp294,sizeof(char),
_get_zero_arr_size(_tmp294,8));}),_tag_dynforward(_tmp293,sizeof(void*),2));}}});
_LL150: if(*((int*)_tmp27B)!= 3)goto _LL152;_LL151: return(struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp299;_tmp299.tag=0;_tmp299.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcpat_descs2string(_tmp277));{void*
_tmp297[1]={& _tmp299};Cyc_aprintf(({const char*_tmp298="$(%s)";_tag_dynforward(
_tmp298,sizeof(char),_get_zero_arr_size(_tmp298,6));}),_tag_dynforward(_tmp297,
sizeof(void*),1));}});_LL152: if(*((int*)_tmp27B)!= 4)goto _LL154;_tmp280=((struct
Cyc_Absyn_Pointer_p_struct*)_tmp27B)->f1;_tmp281=*_tmp280;_tmp282=(void*)_tmp281.r;
if(_tmp282 <= (void*)2)goto _LL154;if(*((int*)_tmp282)!= 6)goto _LL154;_tmp283=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp282)->f2;_LL153: return(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp29D;_tmp29D.tag=0;_tmp29D.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcpat_descs2string(_tmp277));{
struct Cyc_String_pa_struct _tmp29C;_tmp29C.tag=0;_tmp29C.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp283->name));{
void*_tmp29A[2]={& _tmp29C,& _tmp29D};Cyc_aprintf(({const char*_tmp29B="&%s(%s)";
_tag_dynforward(_tmp29B,sizeof(char),_get_zero_arr_size(_tmp29B,8));}),
_tag_dynforward(_tmp29A,sizeof(void*),2));}}});_LL154: if(*((int*)_tmp27B)!= 4)
goto _LL156;_LL155: return(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp2A0;_tmp2A0.tag=0;_tmp2A0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Tcpat_descs2string(_tmp277));{void*_tmp29E[1]={& _tmp2A0};Cyc_aprintf(({const
char*_tmp29F="&%s";_tag_dynforward(_tmp29F,sizeof(char),_get_zero_arr_size(
_tmp29F,4));}),_tag_dynforward(_tmp29E,sizeof(void*),1));}});_LL156: if(*((int*)
_tmp27B)!= 5)goto _LL158;_tmp284=((struct Cyc_Absyn_Aggr_p_struct*)_tmp27B)->f1;
_tmp285=(void*)_tmp284.aggr_info;if(*((int*)_tmp285)!= 1)goto _LL158;_tmp286=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp285)->f1;_tmp287=*_tmp286;_LL157: return(
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp2A4;_tmp2A4.tag=0;_tmp2A4.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcpat_descs2string(_tmp277));{
struct Cyc_String_pa_struct _tmp2A3;_tmp2A3.tag=0;_tmp2A3.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp287->name));{
void*_tmp2A1[2]={& _tmp2A3,& _tmp2A4};Cyc_aprintf(({const char*_tmp2A2="%s{%s}";
_tag_dynforward(_tmp2A2,sizeof(char),_get_zero_arr_size(_tmp2A2,7));}),
_tag_dynforward(_tmp2A1,sizeof(void*),2));}}});_LL158: if(*((int*)_tmp27B)!= 6)
goto _LL15A;_tmp288=((struct Cyc_Absyn_Tunion_p_struct*)_tmp27B)->f2;_LL159: return(
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp2A7;_tmp2A7.tag=0;_tmp2A7.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp288->name));{
void*_tmp2A5[1]={& _tmp2A7};Cyc_aprintf(({const char*_tmp2A6="%s";_tag_dynforward(
_tmp2A6,sizeof(char),_get_zero_arr_size(_tmp2A6,3));}),_tag_dynforward(_tmp2A5,
sizeof(void*),1));}});_LL15A: if((int)_tmp27B != 1)goto _LL15C;_LL15B: return({const
char*_tmp2A8="NULL";_tag_dynforward(_tmp2A8,sizeof(char),_get_zero_arr_size(
_tmp2A8,5));});_LL15C: if(_tmp27B <= (void*)2)goto _LL168;if(*((int*)_tmp27B)!= 7)
goto _LL15E;_tmp289=((struct Cyc_Absyn_Int_p_struct*)_tmp27B)->f2;_LL15D: return(
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp2AB;_tmp2AB.tag=1;_tmp2AB.f1=(
unsigned long)_tmp289;{void*_tmp2A9[1]={& _tmp2AB};Cyc_aprintf(({const char*
_tmp2AA="%d";_tag_dynforward(_tmp2AA,sizeof(char),_get_zero_arr_size(_tmp2AA,3));}),
_tag_dynforward(_tmp2A9,sizeof(void*),1));}});_LL15E: if(*((int*)_tmp27B)!= 8)
goto _LL160;_tmp28A=((struct Cyc_Absyn_Char_p_struct*)_tmp27B)->f1;_LL15F: return(
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp2AE;_tmp2AE.tag=1;_tmp2AE.f1=(
unsigned long)((int)_tmp28A);{void*_tmp2AC[1]={& _tmp2AE};Cyc_aprintf(({const char*
_tmp2AD="%d";_tag_dynforward(_tmp2AD,sizeof(char),_get_zero_arr_size(_tmp2AD,3));}),
_tag_dynforward(_tmp2AC,sizeof(void*),1));}});_LL160: if(*((int*)_tmp27B)!= 9)
goto _LL162;_tmp28B=((struct Cyc_Absyn_Float_p_struct*)_tmp27B)->f1;_LL161: return
_tmp28B;_LL162: if(*((int*)_tmp27B)!= 10)goto _LL164;_tmp28C=((struct Cyc_Absyn_Enum_p_struct*)
_tmp27B)->f2;_LL163: _tmp28D=_tmp28C;goto _LL165;_LL164: if(*((int*)_tmp27B)!= 11)
goto _LL166;_tmp28D=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp27B)->f2;_LL165:
return Cyc_Absynpp_qvar2string(_tmp28D->name);_LL166: if(*((int*)_tmp27B)!= 14)
goto _LL168;_tmp28E=((struct Cyc_Absyn_Exp_p_struct*)_tmp27B)->f1;_LL167: return Cyc_Absynpp_exp2string(
_tmp28E);_LL168:;_LL169:(int)_throw((void*)Cyc_Tcpat_Desc2string);_LL147:;}}
_LL145: if(*((int*)_tmp275)!= 1)goto _LL142;_tmp278=((struct Cyc_Tcpat_Neg_struct*)
_tmp275)->f1;_LL146: if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp278))
return({const char*_tmp2AF="_";_tag_dynforward(_tmp2AF,sizeof(char),
_get_zero_arr_size(_tmp2AF,2));});{struct Cyc_Tcpat_Con_s*_tmp2B0=((struct Cyc_Tcpat_Con_s*(*)(
struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp278);struct Cyc_Absyn_Pat*_tmp2B1=
_tmp2B0->orig_pat;if(_tmp2B1 == 0)(int)_throw((void*)Cyc_Tcpat_Desc2string);{void*
_tmp2B2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2B1->topt))->v);
void*_tmp2B3;struct Cyc_Absyn_PtrInfo _tmp2B4;struct Cyc_Absyn_PtrAtts _tmp2B5;
struct Cyc_Absyn_TunionInfo _tmp2B6;void*_tmp2B7;struct Cyc_Absyn_Tuniondecl**
_tmp2B8;struct Cyc_Absyn_Tuniondecl*_tmp2B9;_LL16B: if(_tmp2B2 <= (void*)4)goto
_LL173;if(*((int*)_tmp2B2)!= 5)goto _LL16D;_tmp2B3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2B2)->f2;if((int)_tmp2B3 != 0)goto _LL16D;_LL16C:{int i=0;for(0;i < 256;i ++){
struct Cyc_Tcpat_Con_s*_tmp2BA=Cyc_Tcpat_char_con((char)i,(struct Cyc_Absyn_Pat*)
_tmp2B1);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp278,_tmp2BA))return(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp2BD;
_tmp2BD.tag=1;_tmp2BD.f1=(unsigned long)i;{void*_tmp2BB[1]={& _tmp2BD};Cyc_aprintf(({
const char*_tmp2BC="%d";_tag_dynforward(_tmp2BC,sizeof(char),_get_zero_arr_size(
_tmp2BC,3));}),_tag_dynforward(_tmp2BB,sizeof(void*),1));}});}}(int)_throw((void*)
Cyc_Tcpat_Desc2string);_LL16D: if(*((int*)_tmp2B2)!= 5)goto _LL16F;_LL16E:{
unsigned int i=0;for(0;i < 0 - 1;i ++){struct Cyc_Tcpat_Con_s*_tmp2BE=Cyc_Tcpat_int_con((
int)i,_tmp2B1);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp278,_tmp2BE))return(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp2C1;
_tmp2C1.tag=1;_tmp2C1.f1=(unsigned long)((int)i);{void*_tmp2BF[1]={& _tmp2C1};Cyc_aprintf(({
const char*_tmp2C0="%d";_tag_dynforward(_tmp2C0,sizeof(char),_get_zero_arr_size(
_tmp2C0,3));}),_tag_dynforward(_tmp2BF,sizeof(void*),1));}});}}(int)_throw((void*)
Cyc_Tcpat_Desc2string);_LL16F: if(*((int*)_tmp2B2)!= 4)goto _LL171;_tmp2B4=((
struct Cyc_Absyn_PointerType_struct*)_tmp2B2)->f1;_tmp2B5=_tmp2B4.ptr_atts;_LL170: {
struct Cyc_Absyn_Conref*_tmp2C2=_tmp2B5.nullable;int is_nullable=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2C2);if(is_nullable){if(!((
int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp278,
Cyc_Tcpat_null_con((struct Cyc_Absyn_Pat*)_tmp2B1)))return({const char*_tmp2C3="NULL";
_tag_dynforward(_tmp2C3,sizeof(char),_get_zero_arr_size(_tmp2C3,5));});}return({
const char*_tmp2C4="&_";_tag_dynforward(_tmp2C4,sizeof(char),_get_zero_arr_size(
_tmp2C4,3));});}_LL171: if(*((int*)_tmp2B2)!= 2)goto _LL173;_tmp2B6=((struct Cyc_Absyn_TunionType_struct*)
_tmp2B2)->f1;_tmp2B7=(void*)_tmp2B6.tunion_info;if(*((int*)_tmp2B7)!= 1)goto
_LL173;_tmp2B8=((struct Cyc_Absyn_KnownTunion_struct*)_tmp2B7)->f1;_tmp2B9=*
_tmp2B8;_LL172: if(_tmp2B9->is_xtunion)(int)_throw((void*)Cyc_Tcpat_Desc2string);{
struct Cyc_List_List*_tmp2C5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2B9->fields))->v;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2C5);for(0;(unsigned int)_tmp2C5;_tmp2C5=_tmp2C5->tl){struct _dynforward_ptr n=*(*((
struct Cyc_Absyn_Tunionfield*)_tmp2C5->hd)->name).f2;struct Cyc_List_List*_tmp2C6=((
struct Cyc_Absyn_Tunionfield*)_tmp2C5->hd)->typs;if(!((int(*)(struct Cyc_Set_Set*s,
struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp278,({struct Cyc_Tcpat_Con_s*
_tmp2C7=_cycalloc(sizeof(*_tmp2C7));_tmp2C7->name=(void*)((void*)({struct Cyc_Tcpat_Name_v_struct*
_tmp2C8=_cycalloc(sizeof(*_tmp2C8));_tmp2C8[0]=({struct Cyc_Tcpat_Name_v_struct
_tmp2C9;_tmp2C9.tag=0;_tmp2C9.f1=n;_tmp2C9;});_tmp2C8;}));_tmp2C7->arity=0;
_tmp2C7->span=0;_tmp2C7->orig_pat=_tmp2B1;_tmp2C7;}))){if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp2C6)== 0)return n;else{if(_tmp2B9->is_flat)return(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp2CC;_tmp2CC.tag=0;_tmp2CC.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)n);{void*_tmp2CA[1]={& _tmp2CC};Cyc_aprintf(({
const char*_tmp2CB="%s(...)";_tag_dynforward(_tmp2CB,sizeof(char),
_get_zero_arr_size(_tmp2CB,8));}),_tag_dynforward(_tmp2CA,sizeof(void*),1));}});
else{return(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp2CF;_tmp2CF.tag=
0;_tmp2CF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)n);{void*_tmp2CD[1]={&
_tmp2CF};Cyc_aprintf(({const char*_tmp2CE="&%s(...)";_tag_dynforward(_tmp2CE,
sizeof(char),_get_zero_arr_size(_tmp2CE,9));}),_tag_dynforward(_tmp2CD,sizeof(
void*),1));}});}}}}(int)_throw((void*)Cyc_Tcpat_Desc2string);}_LL173:;_LL174:(
int)_throw((void*)Cyc_Tcpat_Desc2string);_LL16A:;}}_LL142:;}static struct
_dynforward_ptr*Cyc_Tcpat_desc2stringptr(void*d){return({struct _dynforward_ptr*
_tmp2D0=_cycalloc(sizeof(*_tmp2D0));_tmp2D0[0]=Cyc_Tcpat_desc2string(d);_tmp2D0;});}
static struct _dynforward_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*ds){struct
Cyc_List_List*_tmp2D1=((struct Cyc_List_List*(*)(struct _dynforward_ptr*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_desc2stringptr,ds);struct
_dynforward_ptr*comma=_init_dynforward_ptr(_cycalloc(sizeof(struct
_dynforward_ptr)),",",sizeof(char),2);{struct Cyc_List_List*_tmp2D2=_tmp2D1;for(0;
_tmp2D2 != 0;_tmp2D2=((struct Cyc_List_List*)_check_null(_tmp2D2))->tl){if(_tmp2D2->tl
!= 0){_tmp2D2->tl=({struct Cyc_List_List*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));
_tmp2D3->hd=comma;_tmp2D3->tl=_tmp2D2->tl;_tmp2D3;});_tmp2D2=_tmp2D2->tl;}}}
return(struct _dynforward_ptr)Cyc_strconcat_l(_tmp2D1);}static void Cyc_Tcpat_not_exhaust_err(
struct Cyc_Position_Segment*loc,void*desc){struct _handler_cons _tmp2D5;
_push_handler(& _tmp2D5);{int _tmp2D7=0;if(setjmp(_tmp2D5.handler))_tmp2D7=1;if(!
_tmp2D7){{struct _dynforward_ptr _tmp2D8=Cyc_Tcpat_desc2string(desc);({struct Cyc_String_pa_struct
_tmp2DB;_tmp2DB.tag=0;_tmp2DB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp2D8);{void*_tmp2D9[1]={& _tmp2DB};Cyc_Tcutil_terr(loc,({const char*_tmp2DA="patterns may not be exhaustive.\n\tmissing case for %s";
_tag_dynforward(_tmp2DA,sizeof(char),_get_zero_arr_size(_tmp2DA,53));}),
_tag_dynforward(_tmp2D9,sizeof(void*),1));}});};_pop_handler();}else{void*
_tmp2D6=(void*)_exn_thrown;void*_tmp2DD=_tmp2D6;_LL176: if(_tmp2DD != Cyc_Tcpat_Desc2string)
goto _LL178;_LL177:({void*_tmp2DE[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp2DF="patterns may not be exhaustive.";
_tag_dynforward(_tmp2DF,sizeof(char),_get_zero_arr_size(_tmp2DF,32));}),
_tag_dynforward(_tmp2DE,sizeof(void*),0));});goto _LL175;_LL178:;_LL179:(void)
_throw(_tmp2DD);_LL175:;}}}static void Cyc_Tcpat_rule_occurs(int dummy,struct
_tuple14*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*
loc,struct Cyc_List_List*swcs){struct _RegionHandle _tmp2E0=_new_region("r");struct
_RegionHandle*r=& _tmp2E0;_push_region(r);{int _tmp2E1=0;struct _tuple16 _tmp2E2=({
struct _tuple16 _tmp2EE;_tmp2EE.f1=r;_tmp2EE.f2=& _tmp2E1;_tmp2EE;});struct Cyc_List_List*
_tmp2E3=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple15*(*f)(
struct _tuple16*,struct Cyc_Absyn_Switch_clause*),struct _tuple16*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp2E2,swcs);void*_tmp2E4=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp2E3);int _tmp2E5=0;((void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple14*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp2E4,Cyc_Tcpat_not_exhaust_err,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp2E5);for(0;_tmp2E3 != 0;_tmp2E3=_tmp2E3->tl){
struct _tuple15 _tmp2E7;struct _tuple14*_tmp2E8;struct _tuple14 _tmp2E9;int _tmp2EA;
struct Cyc_Position_Segment*_tmp2EB;struct _tuple15*_tmp2E6=(struct _tuple15*)
_tmp2E3->hd;_tmp2E7=*_tmp2E6;_tmp2E8=_tmp2E7.f2;_tmp2E9=*_tmp2E8;_tmp2EA=_tmp2E9.f1;
_tmp2EB=_tmp2E9.f2;if(!_tmp2EA)({void*_tmp2EC[0]={};Cyc_Tcutil_terr(_tmp2EB,({
const char*_tmp2ED="redundant pattern";_tag_dynforward(_tmp2ED,sizeof(char),
_get_zero_arr_size(_tmp2ED,18));}),_tag_dynforward(_tmp2EC,sizeof(void*),0));});}};
_pop_region(r);}struct _tuple17{struct Cyc_Position_Segment*f1;int f2;};static void
Cyc_Tcpat_not_exhaust_warn(struct _tuple17*pr,void*desc){(*pr).f2=0;{struct
_handler_cons _tmp2EF;_push_handler(& _tmp2EF);{int _tmp2F1=0;if(setjmp(_tmp2EF.handler))
_tmp2F1=1;if(!_tmp2F1){{struct _dynforward_ptr _tmp2F2=Cyc_Tcpat_desc2string(desc);({
struct Cyc_String_pa_struct _tmp2F5;_tmp2F5.tag=0;_tmp2F5.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp2F2);{void*_tmp2F3[1]={& _tmp2F5};Cyc_Tcutil_warn((*
pr).f1,({const char*_tmp2F4="pattern not exhaustive.\n\tmissing case for %s";
_tag_dynforward(_tmp2F4,sizeof(char),_get_zero_arr_size(_tmp2F4,45));}),
_tag_dynforward(_tmp2F3,sizeof(void*),1));}});};_pop_handler();}else{void*
_tmp2F0=(void*)_exn_thrown;void*_tmp2F7=_tmp2F0;_LL17B: if(_tmp2F7 != Cyc_Tcpat_Desc2string)
goto _LL17D;_LL17C:({void*_tmp2F8[0]={};Cyc_Tcutil_warn((*pr).f1,({const char*
_tmp2F9="pattern not exhaustive.";_tag_dynforward(_tmp2F9,sizeof(char),
_get_zero_arr_size(_tmp2F9,24));}),_tag_dynforward(_tmp2F8,sizeof(void*),0));});
goto _LL17A;_LL17D:;_LL17E:(void)_throw(_tmp2F7);_LL17A:;}}}}static void Cyc_Tcpat_dummy_fn(
int i,int j){return;}struct _tuple18{void*f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Pat*p){struct _RegionHandle _tmp2FA=
_new_region("r");struct _RegionHandle*r=& _tmp2FA;_push_region(r);{struct Cyc_List_List*
_tmp2FB=({struct Cyc_List_List*_tmp301=_region_malloc(r,sizeof(*_tmp301));_tmp301->hd=({
struct _tuple18*_tmp302=_region_malloc(r,sizeof(*_tmp302));_tmp302->f1=Cyc_Tcpat_compile_pat(
p);_tmp302->f2=0;_tmp302;});_tmp301->tl=0;_tmp301;});void*_tmp2FC=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp2FB);struct _tuple17 _tmp2FD=({struct _tuple17 _tmp300;_tmp300.f1=loc;_tmp300.f2=
1;_tmp300;});int _tmp2FE=0;((void(*)(void*d,void(*not_exhaust)(struct _tuple17*,
void*),struct _tuple17*env1,void(*rhs_appears)(int,int),int env2,int*exhaust_done))
Cyc_Tcpat_check_exhaust_overlap)(_tmp2FC,Cyc_Tcpat_not_exhaust_warn,& _tmp2FD,Cyc_Tcpat_dummy_fn,
0,& _tmp2FE);{int _tmp2FF=_tmp2FD.f2;_npop_handler(0);return _tmp2FF;}};_pop_region(
r);}static struct _tuple15*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);struct _tuple14*rhs=({struct _tuple14*
_tmp304=_cycalloc(sizeof(*_tmp304));_tmp304->f1=0;_tmp304->f2=(swc->pattern)->loc;
_tmp304;});return({struct _tuple15*_tmp303=_cycalloc(sizeof(*_tmp303));_tmp303->f1=
sp0;_tmp303->f2=rhs;_tmp303;});}static void Cyc_Tcpat_not_exhaust_err2(struct Cyc_Position_Segment*
loc,void*d){;}void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*loc,
struct Cyc_List_List*swcs){struct Cyc_List_List*_tmp305=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcpat_get_match2,swcs);void*_tmp306=((void*(*)(struct _RegionHandle*r,struct
Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(Cyc_Core_heap_region,_tmp305);
int _tmp307=0;((void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple14*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp306,Cyc_Tcpat_not_exhaust_err2,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp307);for(0;_tmp305 != 0;_tmp305=_tmp305->tl){
struct _tuple15 _tmp309;struct _tuple14*_tmp30A;struct _tuple14 _tmp30B;int _tmp30C;
struct Cyc_Position_Segment*_tmp30D;struct _tuple15*_tmp308=(struct _tuple15*)
_tmp305->hd;_tmp309=*_tmp308;_tmp30A=_tmp309.f2;_tmp30B=*_tmp30A;_tmp30C=_tmp30B.f1;
_tmp30D=_tmp30B.f2;if(!_tmp30C)({void*_tmp30E[0]={};Cyc_Tcutil_terr(_tmp30D,({
const char*_tmp30F="redundant pattern";_tag_dynforward(_tmp30F,sizeof(char),
_get_zero_arr_size(_tmp30F,18));}),_tag_dynforward(_tmp30E,sizeof(void*),0));});}}
