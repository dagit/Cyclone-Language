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
struct Cyc_List_List*fields);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct
Cyc_Absyn_Exp*);struct _tuple6{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
struct Cyc_Tcpat_TcPatResult{struct _tuple6*tvars_and_bounds_opt;struct Cyc_List_List*
patvars;};struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Pat*p,void**topt);void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_List_List*);int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*,
struct Cyc_List_List*);struct _tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p){void*_tmp0=(void*)p->r;struct _tuple1*_tmp1;struct _tuple1*_tmp2;struct Cyc_List_List*
_tmp3;int _tmp4;struct Cyc_Absyn_AggrInfo _tmp5;void*_tmp6;void*_tmp7;struct _tuple1*
_tmp8;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmpA;struct Cyc_List_List*
_tmpB;int _tmpC;struct Cyc_Absyn_Exp*_tmpD;_LL1: if(_tmp0 <= (void*)2)goto _LL9;if(*((
int*)_tmp0)!= 12)goto _LL3;_tmp1=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp0)->f1;
_LL2:{struct _handler_cons _tmpE;_push_handler(& _tmpE);{int _tmp10=0;if(setjmp(
_tmpE.handler))_tmp10=1;if(!_tmp10){{struct _RegionHandle _tmp11=_new_region("r");
struct _RegionHandle*r=& _tmp11;_push_region(r);{void*_tmp12=Cyc_Tcenv_lookup_ordinary(
r,te,p->loc,_tmp1);struct Cyc_Absyn_Tuniondecl*_tmp13;struct Cyc_Absyn_Tunionfield*
_tmp14;struct Cyc_Absyn_Enumdecl*_tmp15;struct Cyc_Absyn_Enumfield*_tmp16;void*
_tmp17;struct Cyc_Absyn_Enumfield*_tmp18;_LLC: if(*((int*)_tmp12)!= 1)goto _LLE;
_LLD:(void*)(p->r=(void*)((void*)0));({void*_tmp19[0]={};Cyc_Tcutil_terr(p->loc,({
const char*_tmp1A="struct tag used without arguments in pattern";_tag_dynforward(
_tmp1A,sizeof(char),_get_zero_arr_size(_tmp1A,45));}),_tag_dynforward(_tmp19,
sizeof(void*),0));});_npop_handler(1);return;_LLE: if(*((int*)_tmp12)!= 2)goto
_LL10;_tmp13=((struct Cyc_Tcenv_TunionRes_struct*)_tmp12)->f1;_tmp14=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp12)->f2;_LLF:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Tunion_p_struct*
_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B[0]=({struct Cyc_Absyn_Tunion_p_struct
_tmp1C;_tmp1C.tag=6;_tmp1C.f1=_tmp13;_tmp1C.f2=_tmp14;_tmp1C.f3=0;_tmp1C.f4=0;
_tmp1C;});_tmp1B;})));_npop_handler(1);return;_LL10: if(*((int*)_tmp12)!= 3)goto
_LL12;_tmp15=((struct Cyc_Tcenv_EnumRes_struct*)_tmp12)->f1;_tmp16=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp12)->f2;_LL11:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Enum_p_struct*
_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D[0]=({struct Cyc_Absyn_Enum_p_struct
_tmp1E;_tmp1E.tag=10;_tmp1E.f1=_tmp15;_tmp1E.f2=_tmp16;_tmp1E;});_tmp1D;})));
_npop_handler(1);return;_LL12: if(*((int*)_tmp12)!= 4)goto _LL14;_tmp17=(void*)((
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp12)->f1;_tmp18=((struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp12)->f2;_LL13:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_AnonEnum_p_struct*
_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F[0]=({struct Cyc_Absyn_AnonEnum_p_struct
_tmp20;_tmp20.tag=11;_tmp20.f1=(void*)_tmp17;_tmp20.f2=_tmp18;_tmp20;});_tmp1F;})));
_npop_handler(1);return;_LL14: if(*((int*)_tmp12)!= 0)goto _LLB;_LL15: goto _LLB;
_LLB:;};_pop_region(r);};_pop_handler();}else{void*_tmpF=(void*)_exn_thrown;void*
_tmp22=_tmpF;_LL17: if(_tmp22 != Cyc_Dict_Absent)goto _LL19;_LL18: goto _LL16;_LL19:;
_LL1A:(void)_throw(_tmp22);_LL16:;}}}{void*_tmp23=(*_tmp1).f1;struct Cyc_List_List*
_tmp24;_LL1C: if((int)_tmp23 != 0)goto _LL1E;_LL1D: goto _LL1F;_LL1E: if(_tmp23 <= (
void*)1)goto _LL20;if(*((int*)_tmp23)!= 0)goto _LL20;_tmp24=((struct Cyc_Absyn_Rel_n_struct*)
_tmp23)->f1;if(_tmp24 != 0)goto _LL20;_LL1F:(*_tmp1).f1=(void*)0;(void*)(p->r=(
void*)((void*)({struct Cyc_Absyn_Var_p_struct*_tmp25=_cycalloc(sizeof(*_tmp25));
_tmp25[0]=({struct Cyc_Absyn_Var_p_struct _tmp26;_tmp26.tag=0;_tmp26.f1=Cyc_Absyn_new_vardecl(
_tmp1,(void*)0,0);_tmp26.f2=Cyc_Absyn_new_pat((void*)0,0);_tmp26;});_tmp25;})));
return;_LL20:;_LL21:({void*_tmp27[0]={};Cyc_Tcutil_terr(p->loc,({const char*
_tmp28="qualified variable in pattern";_tag_dynforward(_tmp28,sizeof(char),
_get_zero_arr_size(_tmp28,30));}),_tag_dynforward(_tmp27,sizeof(void*),0));});
return;_LL1B:;}_LL3: if(*((int*)_tmp0)!= 13)goto _LL5;_tmp2=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp0)->f1;_tmp3=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f2;_tmp4=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f3;_LL4:{struct _handler_cons _tmp29;
_push_handler(& _tmp29);{int _tmp2B=0;if(setjmp(_tmp29.handler))_tmp2B=1;if(!
_tmp2B){{struct _RegionHandle _tmp2C=_new_region("r");struct _RegionHandle*r=&
_tmp2C;_push_region(r);{void*_tmp2D=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp2);
struct Cyc_Absyn_Aggrdecl*_tmp2E;struct Cyc_Absyn_Tuniondecl*_tmp2F;struct Cyc_Absyn_Tunionfield*
_tmp30;_LL23: if(*((int*)_tmp2D)!= 1)goto _LL25;_tmp2E=((struct Cyc_Tcenv_AggrRes_struct*)
_tmp2D)->f1;_LL24: if((void*)_tmp2E->kind == (void*)1){(void*)(p->r=(void*)((void*)
0));({void*_tmp31[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp32="cannot pattern-match a union";
_tag_dynforward(_tmp32,sizeof(char),_get_zero_arr_size(_tmp32,29));}),
_tag_dynforward(_tmp31,sizeof(void*),0));});_npop_handler(1);return;}{struct Cyc_List_List*
_tmp33=0;for(0;_tmp3 != 0;_tmp3=_tmp3->tl){_tmp33=({struct Cyc_List_List*_tmp34=
_cycalloc(sizeof(*_tmp34));_tmp34->hd=({struct _tuple8*_tmp35=_cycalloc(sizeof(*
_tmp35));_tmp35->f1=0;_tmp35->f2=(struct Cyc_Absyn_Pat*)_tmp3->hd;_tmp35;});
_tmp34->tl=_tmp33;_tmp34;});}(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Aggr_p_struct*
_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36[0]=({struct Cyc_Absyn_Aggr_p_struct
_tmp37;_tmp37.tag=5;_tmp37.f1=({struct Cyc_Absyn_AggrInfo _tmp38;_tmp38.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp39=_cycalloc(sizeof(*_tmp39));
_tmp39[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp3A;_tmp3A.tag=1;_tmp3A.f1=({
struct Cyc_Absyn_Aggrdecl**_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B[0]=_tmp2E;
_tmp3B;});_tmp3A;});_tmp39;}));_tmp38.targs=0;_tmp38;});_tmp37.f2=0;_tmp37.f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp33);_tmp37.f4=
_tmp4;_tmp37;});_tmp36;})));_npop_handler(1);return;}_LL25: if(*((int*)_tmp2D)!= 
2)goto _LL27;_tmp2F=((struct Cyc_Tcenv_TunionRes_struct*)_tmp2D)->f1;_tmp30=((
struct Cyc_Tcenv_TunionRes_struct*)_tmp2D)->f2;_LL26:(void*)(p->r=(void*)((void*)({
struct Cyc_Absyn_Tunion_p_struct*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=({
struct Cyc_Absyn_Tunion_p_struct _tmp3D;_tmp3D.tag=6;_tmp3D.f1=_tmp2F;_tmp3D.f2=
_tmp30;_tmp3D.f3=_tmp3;_tmp3D.f4=_tmp4;_tmp3D;});_tmp3C;})));_npop_handler(1);
return;_LL27: if(*((int*)_tmp2D)!= 3)goto _LL29;_LL28: goto _LL2A;_LL29: if(*((int*)
_tmp2D)!= 4)goto _LL2B;_LL2A:({void*_tmp3E[0]={};Cyc_Tcutil_terr(p->loc,({const
char*_tmp3F="enum tag used with arguments in pattern";_tag_dynforward(_tmp3F,
sizeof(char),_get_zero_arr_size(_tmp3F,40));}),_tag_dynforward(_tmp3E,sizeof(
void*),0));});(void*)(p->r=(void*)((void*)0));_npop_handler(1);return;_LL2B: if(*((
int*)_tmp2D)!= 0)goto _LL22;_LL2C: goto _LL22;_LL22:;};_pop_region(r);};
_pop_handler();}else{void*_tmp2A=(void*)_exn_thrown;void*_tmp41=_tmp2A;_LL2E: if(
_tmp41 != Cyc_Dict_Absent)goto _LL30;_LL2F: goto _LL2D;_LL30:;_LL31:(void)_throw(
_tmp41);_LL2D:;}}}({struct Cyc_String_pa_struct _tmp44;_tmp44.tag=0;_tmp44.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp2));{
void*_tmp42[1]={& _tmp44};Cyc_Tcutil_terr(p->loc,({const char*_tmp43="%s is not a constructor in pattern";
_tag_dynforward(_tmp43,sizeof(char),_get_zero_arr_size(_tmp43,35));}),
_tag_dynforward(_tmp42,sizeof(void*),1));}});(void*)(p->r=(void*)((void*)0));
return;_LL5: if(*((int*)_tmp0)!= 5)goto _LL7;_tmp5=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f1;_tmp6=(void*)_tmp5.aggr_info;if(*((int*)_tmp6)!= 0)goto _LL7;_tmp7=(
void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp6)->f1;_tmp8=((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp6)->f2;_tmp9=_tmp5.targs;_tmpA=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;
_tmpB=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f3;_tmpC=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f4;_LL6:{struct _handler_cons _tmp45;_push_handler(& _tmp45);{int _tmp47=0;
if(setjmp(_tmp45.handler))_tmp47=1;if(!_tmp47){{struct Cyc_Absyn_Aggrdecl**_tmp48=
Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmp8);struct Cyc_Absyn_Aggrdecl*_tmp49=*
_tmp48;if((void*)_tmp49->kind == (void*)1){({void*_tmp4A[0]={};Cyc_Tcutil_terr(p->loc,({
const char*_tmp4B="cannot pattern-match a union";_tag_dynforward(_tmp4B,sizeof(
char),_get_zero_arr_size(_tmp4B,29));}),_tag_dynforward(_tmp4A,sizeof(void*),0));});(
void*)(p->r=(void*)((void*)0));_npop_handler(0);return;}if(_tmp49->impl == 0){({
void*_tmp4C[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp4D="can't destructure an abstract struct";
_tag_dynforward(_tmp4D,sizeof(char),_get_zero_arr_size(_tmp4D,37));}),
_tag_dynforward(_tmp4C,sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));
_npop_handler(0);return;}{int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp49->impl))->exist_vars)- ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpA);if(more_exists != 0)({void*_tmp4E[
0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp4F="wrong number of existentially bound type variables";
_tag_dynforward(_tmp4F,sizeof(char),_get_zero_arr_size(_tmp4F,51));}),
_tag_dynforward(_tmp4E,sizeof(void*),0));});(void*)(p->r=(void*)((void*)({struct
Cyc_Absyn_Aggr_p_struct*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50[0]=({struct Cyc_Absyn_Aggr_p_struct
_tmp51;_tmp51.tag=5;_tmp51.f1=({struct Cyc_Absyn_AggrInfo _tmp52;_tmp52.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp53=_cycalloc(sizeof(*_tmp53));
_tmp53[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp54;_tmp54.tag=1;_tmp54.f1=_tmp48;
_tmp54;});_tmp53;}));_tmp52.targs=_tmp9;_tmp52;});_tmp51.f2=_tmpA;_tmp51.f3=
_tmpB;_tmp51.f4=_tmpC;_tmp51;});_tmp50;})));}};_pop_handler();}else{void*_tmp46=(
void*)_exn_thrown;void*_tmp56=_tmp46;_LL33: if(_tmp56 != Cyc_Dict_Absent)goto _LL35;
_LL34:({void*_tmp57[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp58="Non-struct name has designator patterns";
_tag_dynforward(_tmp58,sizeof(char),_get_zero_arr_size(_tmp58,40));}),
_tag_dynforward(_tmp57,sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));goto
_LL32;_LL35:;_LL36:(void)_throw(_tmp56);_LL32:;}}}return;_LL7: if(*((int*)_tmp0)
!= 14)goto _LL9;_tmpD=((struct Cyc_Absyn_Exp_p_struct*)_tmp0)->f1;_LL8: Cyc_Tcexp_tcExp(
te,0,_tmpD);if(!Cyc_Tcutil_is_const_exp(te,_tmpD)){({void*_tmp59[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp5A="non-constant expression in case pattern";
_tag_dynforward(_tmp5A,sizeof(char),_get_zero_arr_size(_tmp5A,40));}),
_tag_dynforward(_tmp59,sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));}{
unsigned int _tmp5C;int _tmp5D;struct _tuple7 _tmp5B=Cyc_Evexp_eval_const_uint_exp(
_tmpD);_tmp5C=_tmp5B.f1;_tmp5D=_tmp5B.f2;(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E[0]=({struct Cyc_Absyn_Int_p_struct _tmp5F;
_tmp5F.tag=7;_tmp5F.f1=(void*)((void*)2);_tmp5F.f2=(int)_tmp5C;_tmp5F;});_tmp5E;})));
return;}_LL9:;_LLA: return;_LL0:;}static struct _dynforward_ptr*Cyc_Tcpat_get_name(
struct Cyc_Absyn_Vardecl*vd){return(*vd->name).f2;}static void*Cyc_Tcpat_any_type(
struct Cyc_List_List*s,void**topt){if(topt != 0)return*topt;return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp60=_cycalloc(sizeof(*
_tmp60));_tmp60->v=s;_tmp60;}));}static void*Cyc_Tcpat_num_type(void**topt,void*
numt){if(topt != 0  && Cyc_Tcutil_coerceable(*topt))return*topt;{void*_tmp61=Cyc_Tcutil_compress(
numt);_LL38: if(_tmp61 <= (void*)4)goto _LL3A;if(*((int*)_tmp61)!= 12)goto _LL3A;
_LL39: if(topt != 0)return*topt;goto _LL37;_LL3A:;_LL3B: goto _LL37;_LL37:;}return
numt;}static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**
v_result_ptr,void*t){(void*)(vd->type=(void*)t);vd->tq=Cyc_Absyn_empty_tqual(0);*
v_result_ptr=(struct Cyc_List_List*)({struct Cyc_List_List*_tmp62=_cycalloc(
sizeof(*_tmp62));_tmp62->hd=vd;_tmp62->tl=*v_result_ptr;_tmp62;});}static struct
Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,
struct Cyc_Tcpat_TcPatResult res2){struct _tuple6*_tmp64;struct Cyc_List_List*_tmp65;
struct Cyc_Tcpat_TcPatResult _tmp63=res1;_tmp64=_tmp63.tvars_and_bounds_opt;_tmp65=
_tmp63.patvars;{struct _tuple6*_tmp67;struct Cyc_List_List*_tmp68;struct Cyc_Tcpat_TcPatResult
_tmp66=res2;_tmp67=_tmp66.tvars_and_bounds_opt;_tmp68=_tmp66.patvars;if(_tmp64 != 
0  || _tmp67 != 0){if(_tmp64 == 0)_tmp64=({struct _tuple6*_tmp69=_cycalloc(sizeof(*
_tmp69));_tmp69->f1=0;_tmp69->f2=0;_tmp69;});if(_tmp67 == 0)_tmp67=({struct
_tuple6*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->f1=0;_tmp6A->f2=0;_tmp6A;});
return({struct Cyc_Tcpat_TcPatResult _tmp6B;_tmp6B.tvars_and_bounds_opt=({struct
_tuple6*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((struct _tuple6*)
_check_null(_tmp64))).f1,(*((struct _tuple6*)_check_null(_tmp67))).f1);_tmp6C->f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((
struct _tuple6*)_check_null(_tmp64))).f2,(*((struct _tuple6*)_check_null(_tmp67))).f2);
_tmp6C;});_tmp6B.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp65,_tmp68);_tmp6B;});}return({struct Cyc_Tcpat_TcPatResult
_tmp6D;_tmp6D.tvars_and_bounds_opt=0;_tmp6D.patvars=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp65,_tmp68);
_tmp6D;});}}static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Pat*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->loc=
loc;_tmp6E->topt=0;_tmp6E->r=(void*)((void*)0);_tmp6E;});}struct _tuple9{struct
Cyc_Absyn_Tqual f1;void*f2;};struct _tuple10{struct Cyc_Absyn_Aggrfield*f1;struct
Cyc_Absyn_Pat*f2;};static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p,void**topt,void**rgn_opt){Cyc_Tcpat_resolve_pat(te,p);{
void*t;struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp141;
_tmp141.tvars_and_bounds_opt=0;_tmp141.patvars=0;_tmp141;});{void*_tmp6F=(void*)
p->r;struct Cyc_Absyn_Vardecl*_tmp70;struct Cyc_Absyn_Pat*_tmp71;struct Cyc_Absyn_Vardecl*
_tmp72;struct Cyc_Absyn_Pat*_tmp73;struct Cyc_Absyn_Tvar*_tmp74;struct Cyc_Absyn_Vardecl*
_tmp75;void*_tmp76;void*_tmp77;void*_tmp78;struct Cyc_Absyn_Enumdecl*_tmp79;void*
_tmp7A;struct Cyc_Absyn_Pat*_tmp7B;struct Cyc_List_List*_tmp7C;struct Cyc_List_List**
_tmp7D;int _tmp7E;struct Cyc_Absyn_AggrInfo _tmp7F;void*_tmp80;struct Cyc_Absyn_Aggrdecl**
_tmp81;struct Cyc_Absyn_Aggrdecl*_tmp82;struct Cyc_List_List*_tmp83;struct Cyc_List_List**
_tmp84;struct Cyc_List_List*_tmp85;struct Cyc_List_List*_tmp86;struct Cyc_List_List**
_tmp87;int _tmp88;struct Cyc_Absyn_Tuniondecl*_tmp89;struct Cyc_Absyn_Tunionfield*
_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_List_List**_tmp8C;int _tmp8D;struct
Cyc_Absyn_AggrInfo _tmp8E;void*_tmp8F;_LL3D: if((int)_tmp6F != 0)goto _LL3F;_LL3E: t=
Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);goto _LL3C;_LL3F: if(
_tmp6F <= (void*)2)goto _LL53;if(*((int*)_tmp6F)!= 0)goto _LL41;_tmp70=((struct Cyc_Absyn_Var_p_struct*)
_tmp6F)->f1;_tmp71=((struct Cyc_Absyn_Var_p_struct*)_tmp6F)->f2;_LL40: res=Cyc_Tcpat_tcPatRec(
te,_tmp71,topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp71->topt))->v;
Cyc_Tcpat_set_vd(_tmp70,& res.patvars,t);goto _LL3C;_LL41: if(*((int*)_tmp6F)!= 1)
goto _LL43;_tmp72=((struct Cyc_Absyn_Reference_p_struct*)_tmp6F)->f1;_tmp73=((
struct Cyc_Absyn_Reference_p_struct*)_tmp6F)->f2;_LL42: res=Cyc_Tcpat_tcPatRec(te,
_tmp73,topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp73->topt))->v;
if(rgn_opt == 0){({void*_tmp90[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp91="* pattern would point into an unknown/unallowed region";
_tag_dynforward(_tmp91,sizeof(char),_get_zero_arr_size(_tmp91,55));}),
_tag_dynforward(_tmp90,sizeof(void*),0));});goto _LL3C;}Cyc_Tcpat_set_vd(_tmp72,&
res.patvars,(void*)({struct Cyc_Absyn_PointerType_struct*_tmp92=_cycalloc(sizeof(*
_tmp92));_tmp92[0]=({struct Cyc_Absyn_PointerType_struct _tmp93;_tmp93.tag=4;
_tmp93.f1=({struct Cyc_Absyn_PtrInfo _tmp94;_tmp94.elt_typ=(void*)t;_tmp94.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp94.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp95;_tmp95.rgn=(
void*)*rgn_opt;_tmp95.nullable=Cyc_Absyn_false_conref;_tmp95.bounds=Cyc_Absyn_empty_conref();
_tmp95.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmp95.ptrloc=
0;_tmp95;});_tmp94;});_tmp93;});_tmp92;}));goto _LL3C;_LL43: if(*((int*)_tmp6F)!= 
2)goto _LL45;_tmp74=((struct Cyc_Absyn_TagInt_p_struct*)_tmp6F)->f1;_tmp75=((
struct Cyc_Absyn_TagInt_p_struct*)_tmp6F)->f2;_LL44: Cyc_Tcpat_set_vd(_tmp75,& res.patvars,(
void*)_tmp75->type);{struct _RegionHandle _tmp96=_new_region("r2");struct
_RegionHandle*r2=& _tmp96;_push_region(r2);Cyc_Tcenv_add_type_vars(r2,p->loc,te,({
struct Cyc_Absyn_Tvar*_tmp97[1];_tmp97[0]=_tmp74;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp97,sizeof(struct Cyc_Absyn_Tvar*),
1));}));;_pop_region(r2);}if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({
struct _tuple6*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->f1=0;_tmp98->f2=0;_tmp98;});(*
res.tvars_and_bounds_opt).f1=({struct Cyc_List_List*_tmp99=_cycalloc(sizeof(*
_tmp99));_tmp99->hd=_tmp74;_tmp99->tl=(*res.tvars_and_bounds_opt).f1;_tmp99;});t=
Cyc_Absyn_uint_typ;goto _LL3C;_LL45: if(*((int*)_tmp6F)!= 7)goto _LL47;_tmp76=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp6F)->f1;if((int)_tmp76 != 1)goto _LL47;_LL46: t=
Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL3C;_LL47: if(*((int*)_tmp6F)!= 
7)goto _LL49;_tmp77=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp6F)->f1;if((int)
_tmp77 != 2)goto _LL49;_LL48: goto _LL4A;_LL49: if(*((int*)_tmp6F)!= 7)goto _LL4B;
_tmp78=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp6F)->f1;if((int)_tmp78 != 0)
goto _LL4B;_LL4A: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL3C;_LL4B:
if(*((int*)_tmp6F)!= 8)goto _LL4D;_LL4C: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);
goto _LL3C;_LL4D: if(*((int*)_tmp6F)!= 9)goto _LL4F;_LL4E: t=Cyc_Tcpat_num_type(topt,
Cyc_Absyn_float_typ);goto _LL3C;_LL4F: if(*((int*)_tmp6F)!= 10)goto _LL51;_tmp79=((
struct Cyc_Absyn_Enum_p_struct*)_tmp6F)->f1;_LL50: t=Cyc_Tcpat_num_type(topt,(void*)({
struct Cyc_Absyn_EnumType_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=({
struct Cyc_Absyn_EnumType_struct _tmp9B;_tmp9B.tag=12;_tmp9B.f1=_tmp79->name;
_tmp9B.f2=(struct Cyc_Absyn_Enumdecl*)_tmp79;_tmp9B;});_tmp9A;}));goto _LL3C;_LL51:
if(*((int*)_tmp6F)!= 11)goto _LL53;_tmp7A=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp6F)->f1;_LL52: t=Cyc_Tcpat_num_type(topt,_tmp7A);goto _LL3C;_LL53: if((int)
_tmp6F != 1)goto _LL55;_LL54: if(topt != 0){void*_tmp9C=Cyc_Tcutil_compress(*topt);
_LL66: if(_tmp9C <= (void*)4)goto _LL68;if(*((int*)_tmp9C)!= 4)goto _LL68;_LL67: t=*
topt;goto tcpat_end;_LL68:;_LL69: goto _LL65;_LL65:;}{struct Cyc_List_List*_tmp9D=
Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E[0]=({struct Cyc_Absyn_PointerType_struct
_tmp9F;_tmp9F.tag=4;_tmp9F.f1=({struct Cyc_Absyn_PtrInfo _tmpA0;_tmpA0.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->v=_tmp9D;_tmpA3;}));_tmpA0.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmpA0.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpA1;_tmpA1.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmpA2=_cycalloc(sizeof(*
_tmpA2));_tmpA2->v=_tmp9D;_tmpA2;}));_tmpA1.nullable=Cyc_Absyn_true_conref;
_tmpA1.bounds=Cyc_Absyn_empty_conref();_tmpA1.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpA1.ptrloc=0;_tmpA1;});_tmpA0;});_tmp9F;});_tmp9E;});
goto _LL3C;}_LL55: if(_tmp6F <= (void*)2)goto _LL57;if(*((int*)_tmp6F)!= 4)goto _LL57;
_tmp7B=((struct Cyc_Absyn_Pointer_p_struct*)_tmp6F)->f1;_LL56: {void*inner_typ=(
void*)0;void**_tmpA4=0;if(topt != 0){void*_tmpA5=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmpA6;void*_tmpA7;_LL6B: if(_tmpA5 <= (void*)4)goto _LL6D;
if(*((int*)_tmpA5)!= 4)goto _LL6D;_tmpA6=((struct Cyc_Absyn_PointerType_struct*)
_tmpA5)->f1;_tmpA7=(void*)_tmpA6.elt_typ;_LL6C: inner_typ=_tmpA7;_tmpA4=(void**)&
inner_typ;goto _LL6A;_LL6D:;_LL6E: goto _LL6A;_LL6A:;}{void*ptr_rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmpBC=_cycalloc(sizeof(*
_tmpBC));_tmpBC->v=Cyc_Tcenv_lookup_type_vars(te);_tmpBC;}));res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,_tmp7B,_tmpA4,(void**)& ptr_rgn));if(_tmpA4 == 0){void*
_tmpA8=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp7B->topt))->v);
struct Cyc_Absyn_TunionFieldInfo _tmpA9;void*_tmpAA;struct Cyc_Absyn_Tuniondecl*
_tmpAB;struct Cyc_Absyn_Tunionfield*_tmpAC;struct Cyc_List_List*_tmpAD;_LL70: if(
_tmpA8 <= (void*)4)goto _LL72;if(*((int*)_tmpA8)!= 3)goto _LL72;_tmpA9=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpA8)->f1;_tmpAA=(void*)_tmpA9.field_info;if(*((int*)_tmpAA)!= 1)goto _LL72;
_tmpAB=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpAA)->f1;_tmpAC=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpAA)->f2;_tmpAD=_tmpA9.targs;_LL71: t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE[0]=({struct Cyc_Absyn_TunionType_struct
_tmpAF;_tmpAF.tag=2;_tmpAF.f1=({struct Cyc_Absyn_TunionInfo _tmpB0;_tmpB0.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmpB1=_cycalloc(sizeof(*
_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_KnownTunion_struct _tmpB2;_tmpB2.tag=1;
_tmpB2.f1=({struct Cyc_Absyn_Tuniondecl**_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[
0]=_tmpAB;_tmpB3;});_tmpB2;});_tmpB1;}));_tmpB0.targs=_tmpAD;_tmpB0.rgn=(void*)
ptr_rgn;_tmpB0;});_tmpAF;});_tmpAE;});goto _LL6F;_LL72:;_LL73: t=(void*)({struct
Cyc_Absyn_PointerType_struct*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({
struct Cyc_Absyn_PointerType_struct _tmpB5;_tmpB5.tag=4;_tmpB5.f1=({struct Cyc_Absyn_PtrInfo
_tmpB6;_tmpB6.elt_typ=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp7B->topt))->v);
_tmpB6.elt_tq=Cyc_Absyn_empty_tqual(0);_tmpB6.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmpB7;_tmpB7.rgn=(void*)ptr_rgn;_tmpB7.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpB7.bounds=Cyc_Absyn_empty_conref();_tmpB7.zero_term=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmpB7.ptrloc=0;_tmpB7;});
_tmpB6;});_tmpB5;});_tmpB4;});goto _LL6F;_LL6F:;}else{t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=({struct Cyc_Absyn_PointerType_struct
_tmpB9;_tmpB9.tag=4;_tmpB9.f1=({struct Cyc_Absyn_PtrInfo _tmpBA;_tmpBA.elt_typ=(
void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp7B->topt))->v);_tmpBA.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmpBA.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpBB;_tmpBB.rgn=(
void*)ptr_rgn;_tmpBB.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmpBB.bounds=Cyc_Absyn_empty_conref();_tmpBB.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpBB.ptrloc=0;_tmpBB;});_tmpBA;});_tmpB9;});_tmpB8;});}
goto _LL3C;}}_LL57: if(_tmp6F <= (void*)2)goto _LL59;if(*((int*)_tmp6F)!= 3)goto
_LL59;_tmp7C=((struct Cyc_Absyn_Tuple_p_struct*)_tmp6F)->f1;_tmp7D=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Tuple_p_struct*)_tmp6F)->f1;_tmp7E=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp6F)->f2;_LL58: {struct Cyc_List_List*_tmpBD=*_tmp7D;struct Cyc_List_List*
pat_ts=0;struct Cyc_List_List*topt_ts=0;if(topt != 0){void*_tmpBE=Cyc_Tcutil_compress(*
topt);struct Cyc_List_List*_tmpBF;_LL75: if(_tmpBE <= (void*)4)goto _LL77;if(*((int*)
_tmpBE)!= 9)goto _LL77;_tmpBF=((struct Cyc_Absyn_TupleType_struct*)_tmpBE)->f1;
_LL76: topt_ts=_tmpBF;if(_tmp7E){int _tmpC0=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpBD);int _tmpC1=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBF);if(
_tmpC0 < _tmpC1){struct Cyc_List_List*wild_ps=0;{int i=0;for(0;i < _tmpC1 - _tmpC0;i
++){wild_ps=({struct Cyc_List_List*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->hd=
Cyc_Tcpat_wild_pat(p->loc);_tmpC2->tl=wild_ps;_tmpC2;});}}*_tmp7D=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpBD,wild_ps);
_tmpBD=*_tmp7D;}else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBD)
== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBF))({void*_tmpC3[0]={};
Cyc_Tcutil_warn(p->loc,({const char*_tmpC4="unnecessary ... in tuple pattern";
_tag_dynforward(_tmpC4,sizeof(char),_get_zero_arr_size(_tmpC4,33));}),
_tag_dynforward(_tmpC3,sizeof(void*),0));});}}goto _LL74;_LL77:;_LL78: goto _LL74;
_LL74:;}else{if(_tmp7E)({void*_tmpC5[0]={};Cyc_Tcutil_terr(p->loc,({const char*
_tmpC6="cannot determine missing fields for ... in tuple pattern";
_tag_dynforward(_tmpC6,sizeof(char),_get_zero_arr_size(_tmpC6,57));}),
_tag_dynforward(_tmpC5,sizeof(void*),0));});}for(0;_tmpBD != 0;_tmpBD=_tmpBD->tl){
void**_tmpC7=0;if(topt_ts != 0){_tmpC7=(void**)&(*((struct _tuple9*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(
struct Cyc_Absyn_Pat*)_tmpBD->hd,_tmpC7,rgn_opt));pat_ts=({struct Cyc_List_List*
_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8->hd=({struct _tuple9*_tmpC9=_cycalloc(
sizeof(*_tmpC9));_tmpC9->f1=Cyc_Absyn_empty_tqual(0);_tmpC9->f2=(void*)((struct
Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Pat*)_tmpBD->hd)->topt))->v;_tmpC9;});
_tmpC8->tl=pat_ts;_tmpC8;});}t=(void*)({struct Cyc_Absyn_TupleType_struct*_tmpCA=
_cycalloc(sizeof(*_tmpCA));_tmpCA[0]=({struct Cyc_Absyn_TupleType_struct _tmpCB;
_tmpCB.tag=9;_tmpCB.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
pat_ts);_tmpCB;});_tmpCA;});goto _LL3C;}_LL59: if(_tmp6F <= (void*)2)goto _LL5B;if(*((
int*)_tmp6F)!= 5)goto _LL5B;_tmp7F=((struct Cyc_Absyn_Aggr_p_struct*)_tmp6F)->f1;
_tmp80=(void*)_tmp7F.aggr_info;if(*((int*)_tmp80)!= 1)goto _LL5B;_tmp81=((struct
Cyc_Absyn_KnownAggr_struct*)_tmp80)->f1;_tmp82=*_tmp81;_tmp83=_tmp7F.targs;
_tmp84=(struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_struct*)_tmp6F)->f1).targs;
_tmp85=((struct Cyc_Absyn_Aggr_p_struct*)_tmp6F)->f2;_tmp86=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp6F)->f3;_tmp87=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_struct*)
_tmp6F)->f3;_tmp88=((struct Cyc_Absyn_Aggr_p_struct*)_tmp6F)->f4;_LL5A: {struct
Cyc_List_List*_tmpCC=*_tmp87;if(_tmp82->impl == 0){({void*_tmpCD[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmpCE="can't destructure an abstract struct";_tag_dynforward(
_tmpCE,sizeof(char),_get_zero_arr_size(_tmpCE,37));}),_tag_dynforward(_tmpCD,
sizeof(void*),0));});t=Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpCF=_cycalloc(
sizeof(*_tmpCF));_tmpCF->v=Cyc_Tcenv_lookup_type_vars(te);_tmpCF;}));goto _LL3C;}{
struct _RegionHandle _tmpD0=_new_region("rgn");struct _RegionHandle*rgn=& _tmpD0;
_push_region(rgn);{struct Cyc_List_List*_tmpD1=0;struct Cyc_List_List*
outlives_constraints=0;struct Cyc_List_List*_tmpD2=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp82->impl))->exist_vars;{struct Cyc_List_List*t=_tmp85;for(0;t != 0;
t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;struct Cyc_Absyn_Tvar*
uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmpD2))->hd;_tmpD2=
_tmpD2->tl;{void*_tmpD3=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmpD4=Cyc_Absyn_compress_kb((
void*)uv->kind);int error=0;void*k2;{void*_tmpD5=_tmpD4;void*_tmpD6;void*_tmpD7;
_LL7A: if(*((int*)_tmpD5)!= 2)goto _LL7C;_tmpD6=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpD5)->f2;_LL7B: _tmpD7=_tmpD6;goto _LL7D;_LL7C: if(*((int*)_tmpD5)!= 0)goto _LL7E;
_tmpD7=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpD5)->f1;_LL7D: k2=_tmpD7;goto
_LL79;_LL7E:;_LL7F:({void*_tmpD8[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpD9="unconstrained existential type variable in struct";
_tag_dynforward(_tmpD9,sizeof(char),_get_zero_arr_size(_tmpD9,50));}),
_tag_dynforward(_tmpD8,sizeof(void*),0));});_LL79:;}{void*_tmpDA=_tmpD3;void*
_tmpDB;struct Cyc_Core_Opt*_tmpDC;struct Cyc_Core_Opt**_tmpDD;void*_tmpDE;struct
Cyc_Core_Opt*_tmpDF;struct Cyc_Core_Opt**_tmpE0;_LL81: if(*((int*)_tmpDA)!= 0)goto
_LL83;_tmpDB=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpDA)->f1;_LL82: if(!Cyc_Tcutil_kind_leq(
k2,_tmpDB))error=1;goto _LL80;_LL83: if(*((int*)_tmpDA)!= 2)goto _LL85;_tmpDC=((
struct Cyc_Absyn_Less_kb_struct*)_tmpDA)->f1;_tmpDD=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmpDA)->f1;_tmpDE=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpDA)->f2;_LL84: _tmpE0=_tmpDD;goto _LL86;_LL85: if(*((int*)_tmpDA)!= 1)goto _LL80;
_tmpDF=((struct Cyc_Absyn_Unknown_kb_struct*)_tmpDA)->f1;_tmpE0=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmpDA)->f1;_LL86:*_tmpE0=({struct Cyc_Core_Opt*
_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->v=(void*)_tmpD4;_tmpE1;});goto _LL80;
_LL80:;}if(error)({struct Cyc_String_pa_struct _tmpE6;_tmpE6.tag=0;_tmpE6.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(k2));{
struct Cyc_String_pa_struct _tmpE5;_tmpE5.tag=0;_tmpE5.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_kindbound2string(_tmpD3));{struct Cyc_String_pa_struct
_tmpE4;_tmpE4.tag=0;_tmpE4.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*tv->name);{
void*_tmpE2[3]={& _tmpE4,& _tmpE5,& _tmpE6};Cyc_Tcutil_terr(p->loc,({const char*
_tmpE3="type variable %s has kind %s but must have at least kind %s";
_tag_dynforward(_tmpE3,sizeof(char),_get_zero_arr_size(_tmpE3,60));}),
_tag_dynforward(_tmpE2,sizeof(void*),3));}}}});{void*vartype=(void*)({struct Cyc_Absyn_VarType_struct*
_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA[0]=({struct Cyc_Absyn_VarType_struct
_tmpEB;_tmpEB.tag=1;_tmpEB.f1=tv;_tmpEB;});_tmpEA;});_tmpD1=({struct Cyc_List_List*
_tmpE7=_region_malloc(rgn,sizeof(*_tmpE7));_tmpE7->hd=(void*)((void*)vartype);
_tmpE7->tl=_tmpD1;_tmpE7;});if(k2 == (void*)3)outlives_constraints=({struct Cyc_List_List*
_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->hd=({struct _tuple0*_tmpE9=_cycalloc(
sizeof(*_tmpE9));_tmpE9->f1=Cyc_Absyn_empty_effect;_tmpE9->f2=vartype;_tmpE9;});
_tmpE8->tl=outlives_constraints;_tmpE8;});}}}}_tmpD1=Cyc_List_imp_rev(_tmpD1);{
struct _RegionHandle _tmpEC=_new_region("r2");struct _RegionHandle*r2=& _tmpEC;
_push_region(r2);{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,
_tmp85);struct Cyc_List_List*_tmpED=Cyc_Tcenv_lookup_type_vars(te2);struct _tuple4
_tmpEE=({struct _tuple4 _tmp10F;_tmp10F.f1=_tmpED;_tmp10F.f2=rgn;_tmp10F;});struct
Cyc_List_List*_tmpEF=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmpEE,_tmp82->tvs);struct
Cyc_List_List*_tmpF0=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,
rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp82->impl))->exist_vars,
_tmpD1);struct Cyc_List_List*_tmpF1=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmpEF);struct Cyc_List_List*_tmpF2=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmpF0);struct Cyc_List_List*_tmpF3=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmpEF,_tmpF0);
if(_tmp85 != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp82->impl))->rgn_po
!= 0){if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({struct _tuple6*
_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->f1=0;_tmpF4->f2=0;_tmpF4;});(*res.tvars_and_bounds_opt).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f1,_tmp85);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
outlives_constraints);{struct Cyc_List_List*_tmpF5=0;{struct Cyc_List_List*_tmpF6=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp82->impl))->rgn_po;for(0;_tmpF6 != 
0;_tmpF6=_tmpF6->tl){_tmpF5=({struct Cyc_List_List*_tmpF7=_cycalloc(sizeof(*
_tmpF7));_tmpF7->hd=({struct _tuple0*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->f1=
Cyc_Tcutil_rsubstitute(rgn,_tmpF3,(*((struct _tuple0*)_tmpF6->hd)).f1);_tmpF8->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmpF3,(*((struct _tuple0*)_tmpF6->hd)).f2);_tmpF8;});
_tmpF7->tl=_tmpF5;_tmpF7;});}}_tmpF5=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmpF5);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
_tmpF5);}}*_tmp84=_tmpF1;t=(void*)({struct Cyc_Absyn_AggrType_struct*_tmpF9=
_cycalloc(sizeof(*_tmpF9));_tmpF9[0]=({struct Cyc_Absyn_AggrType_struct _tmpFA;
_tmpFA.tag=10;_tmpFA.f1=({struct Cyc_Absyn_AggrInfo _tmpFB;_tmpFB.aggr_info=(void*)((
void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));
_tmpFC[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpFD;_tmpFD.tag=1;_tmpFD.f1=({
struct Cyc_Absyn_Aggrdecl**_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=_tmp82;
_tmpFE;});_tmpFD;});_tmpFC;}));_tmpFB.targs=*_tmp84;_tmpFB;});_tmpFA;});_tmpF9;});
if(_tmp88){int _tmpFF=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpCC);
int _tmp100=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp82->impl))->fields);if(_tmpFF < _tmp100){struct Cyc_List_List*
wild_dps=0;{int i=0;for(0;i < _tmp100 - _tmpFF;i ++){wild_dps=({struct Cyc_List_List*
_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->hd=({struct _tuple8*_tmp102=
_cycalloc(sizeof(*_tmp102));_tmp102->f1=0;_tmp102->f2=Cyc_Tcpat_wild_pat(p->loc);
_tmp102;});_tmp101->tl=wild_dps;_tmp101;});}}*_tmp87=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpCC,
wild_dps);_tmpCC=*_tmp87;}else{if(_tmpFF == _tmp100)({void*_tmp103[0]={};Cyc_Tcutil_warn(
p->loc,({const char*_tmp104="unnecessary ... in struct pattern";_tag_dynforward(
_tmp104,sizeof(char),_get_zero_arr_size(_tmp104,34));}),_tag_dynforward(_tmp103,
sizeof(void*),0));});}}{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,p->loc,
_tmpCC,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp82->impl))->fields);for(0;
fields != 0;fields=fields->tl){struct _tuple10 _tmp106;struct Cyc_Absyn_Aggrfield*
_tmp107;struct Cyc_Absyn_Pat*_tmp108;struct _tuple10*_tmp105=(struct _tuple10*)
fields->hd;_tmp106=*_tmp105;_tmp107=_tmp106.f1;_tmp108=_tmp106.f2;{void*_tmp109=
Cyc_Tcutil_rsubstitute(rgn,_tmpF3,(void*)_tmp107->type);res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te2,_tmp108,(void**)& _tmp109,rgn_opt));if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp108->topt))->v,_tmp109))({struct Cyc_String_pa_struct
_tmp10E;_tmp10E.tag=0;_tmp10E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp108->topt))->v));{
struct Cyc_String_pa_struct _tmp10D;_tmp10D.tag=0;_tmp10D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp109));{struct
Cyc_String_pa_struct _tmp10C;_tmp10C.tag=0;_tmp10C.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*_tmp107->name);{void*_tmp10A[3]={& _tmp10C,& _tmp10D,&
_tmp10E};Cyc_Tcutil_terr(p->loc,({const char*_tmp10B="field %s of struct pattern expects type %s != %s";
_tag_dynforward(_tmp10B,sizeof(char),_get_zero_arr_size(_tmp10B,49));}),
_tag_dynforward(_tmp10A,sizeof(void*),3));}}}});}}}};_pop_region(r2);}};
_pop_region(rgn);}goto _LL3C;}_LL5B: if(_tmp6F <= (void*)2)goto _LL5D;if(*((int*)
_tmp6F)!= 6)goto _LL5D;_tmp89=((struct Cyc_Absyn_Tunion_p_struct*)_tmp6F)->f1;
_tmp8A=((struct Cyc_Absyn_Tunion_p_struct*)_tmp6F)->f2;_tmp8B=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp6F)->f3;_tmp8C=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tunion_p_struct*)
_tmp6F)->f3;_tmp8D=((struct Cyc_Absyn_Tunion_p_struct*)_tmp6F)->f4;_LL5C: {struct
Cyc_List_List*_tmp110=*_tmp8C;{struct _RegionHandle _tmp111=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp111;_push_region(rgn);{struct Cyc_List_List*tqts=
_tmp8A->typs;struct Cyc_List_List*_tmp112=Cyc_Tcenv_lookup_type_vars(te);struct
_tuple4 _tmp113=({struct _tuple4 _tmp13E;_tmp13E.f1=_tmp112;_tmp13E.f2=rgn;_tmp13E;});
struct Cyc_List_List*_tmp114=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp113,
_tmp89->tvs);struct Cyc_List_List*_tmp115=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp114);if(tqts == 0)t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116[0]=({struct Cyc_Absyn_TunionType_struct
_tmp117;_tmp117.tag=2;_tmp117.f1=({struct Cyc_Absyn_TunionInfo _tmp118;_tmp118.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmp11A=_cycalloc(sizeof(*
_tmp11A));_tmp11A[0]=({struct Cyc_Absyn_KnownTunion_struct _tmp11B;_tmp11B.tag=1;
_tmp11B.f1=({struct Cyc_Absyn_Tuniondecl**_tmp11C=_cycalloc(sizeof(*_tmp11C));
_tmp11C[0]=_tmp89;_tmp11C;});_tmp11B;});_tmp11A;}));_tmp118.targs=_tmp115;
_tmp118.rgn=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->v=_tmp112;
_tmp119;}));_tmp118;});_tmp117;});_tmp116;});else{t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp11E;_tmp11E.tag=3;_tmp11E.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp11F;
_tmp11F.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp121;_tmp121.tag=1;_tmp121.f1=_tmp89;_tmp121.f2=_tmp8A;_tmp121;});_tmp120;}));
_tmp11F.targs=_tmp115;_tmp11F;});_tmp11E;});_tmp11D;});}if(topt != 0  && tqts == 0){
void*_tmp122=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo _tmp123;struct
Cyc_List_List*_tmp124;_LL88: if(_tmp122 <= (void*)4)goto _LL8C;if(*((int*)_tmp122)
!= 3)goto _LL8A;_LL89: t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp125=
_cycalloc(sizeof(*_tmp125));_tmp125[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp126;_tmp126.tag=3;_tmp126.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp127;
_tmp127.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp129;_tmp129.tag=1;_tmp129.f1=_tmp89;_tmp129.f2=_tmp8A;_tmp129;});_tmp128;}));
_tmp127.targs=_tmp115;_tmp127;});_tmp126;});_tmp125;});goto _LL87;_LL8A: if(*((int*)
_tmp122)!= 2)goto _LL8C;_tmp123=((struct Cyc_Absyn_TunionType_struct*)_tmp122)->f1;
_tmp124=_tmp123.targs;_LL8B: {struct Cyc_List_List*_tmp12A=_tmp115;for(0;_tmp12A
!= 0  && _tmp124 != 0;(_tmp12A=_tmp12A->tl,_tmp124=_tmp124->tl)){Cyc_Tcutil_unify((
void*)_tmp12A->hd,(void*)_tmp124->hd);}goto _LL87;}_LL8C:;_LL8D: goto _LL87;_LL87:;}
if(_tmp8D){int _tmp12B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp110);
int _tmp12C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);if(_tmp12B < 
_tmp12C){struct Cyc_List_List*wild_ps=0;{int i=0;for(0;i < _tmp12C - _tmp12B;i ++){
wild_ps=({struct Cyc_List_List*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->hd=
Cyc_Tcpat_wild_pat(p->loc);_tmp12D->tl=wild_ps;_tmp12D;});}}*_tmp8C=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp110,
wild_ps);_tmp110=*_tmp8C;}else{if(_tmp12B == _tmp12C)({struct Cyc_String_pa_struct
_tmp130;_tmp130.tag=0;_tmp130.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp89->name));{void*_tmp12E[1]={& _tmp130};Cyc_Tcutil_warn(
p->loc,({const char*_tmp12F="unnecessary ... in tunion field %s";_tag_dynforward(
_tmp12F,sizeof(char),_get_zero_arr_size(_tmp12F,35));}),_tag_dynforward(_tmp12E,
sizeof(void*),1));}});}}for(0;_tmp110 != 0  && tqts != 0;(_tmp110=_tmp110->tl,tqts=
tqts->tl)){struct Cyc_Absyn_Pat*_tmp131=(struct Cyc_Absyn_Pat*)_tmp110->hd;void*
_tmp132=Cyc_Tcutil_rsubstitute(rgn,_tmp114,(*((struct _tuple9*)tqts->hd)).f2);res=
Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp131,(void**)& _tmp132,
rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmp131->topt))->v,
_tmp132))({struct Cyc_String_pa_struct _tmp137;_tmp137.tag=0;_tmp137.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp131->topt))->v));{struct Cyc_String_pa_struct
_tmp136;_tmp136.tag=0;_tmp136.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(_tmp132));{struct Cyc_String_pa_struct _tmp135;_tmp135.tag=
0;_tmp135.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp8A->name));{void*_tmp133[3]={& _tmp135,& _tmp136,& _tmp137};Cyc_Tcutil_terr(
_tmp131->loc,({const char*_tmp134="%s expects argument type %s, not %s";
_tag_dynforward(_tmp134,sizeof(char),_get_zero_arr_size(_tmp134,36));}),
_tag_dynforward(_tmp133,sizeof(void*),3));}}}});}if(_tmp110 != 0)({struct Cyc_String_pa_struct
_tmp13A;_tmp13A.tag=0;_tmp13A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp8A->name));{void*_tmp138[1]={& _tmp13A};Cyc_Tcutil_terr(
p->loc,({const char*_tmp139="too many arguments for tunion constructor %s";
_tag_dynforward(_tmp139,sizeof(char),_get_zero_arr_size(_tmp139,45));}),
_tag_dynforward(_tmp138,sizeof(void*),1));}});if(tqts != 0)({struct Cyc_String_pa_struct
_tmp13D;_tmp13D.tag=0;_tmp13D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp8A->name));{void*_tmp13B[1]={& _tmp13D};Cyc_Tcutil_terr(
p->loc,({const char*_tmp13C="too few arguments for tunion constructor %s";
_tag_dynforward(_tmp13C,sizeof(char),_get_zero_arr_size(_tmp13C,44));}),
_tag_dynforward(_tmp13B,sizeof(void*),1));}});};_pop_region(rgn);}goto _LL3C;}
_LL5D: if(_tmp6F <= (void*)2)goto _LL5F;if(*((int*)_tmp6F)!= 5)goto _LL5F;_tmp8E=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp6F)->f1;_tmp8F=(void*)_tmp8E.aggr_info;if(*((
int*)_tmp8F)!= 0)goto _LL5F;_LL5E: goto _LL60;_LL5F: if(_tmp6F <= (void*)2)goto _LL61;
if(*((int*)_tmp6F)!= 12)goto _LL61;_LL60: goto _LL62;_LL61: if(_tmp6F <= (void*)2)
goto _LL63;if(*((int*)_tmp6F)!= 14)goto _LL63;_LL62: goto _LL64;_LL63: if(_tmp6F <= (
void*)2)goto _LL3C;if(*((int*)_tmp6F)!= 13)goto _LL3C;_LL64: t=Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp13F;}));goto _LL3C;_LL3C:;}tcpat_end: p->topt=({struct Cyc_Core_Opt*_tmp140=
_cycalloc(sizeof(*_tmp140));_tmp140->v=(void*)t;_tmp140;});return res;}}struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){struct
Cyc_Tcpat_TcPatResult _tmp142=Cyc_Tcpat_tcPatRec(te,p,topt,0);{struct
_RegionHandle _tmp143=_new_region("r");struct _RegionHandle*r=& _tmp143;
_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dynforward_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*
x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp142.patvars),p->loc,({const char*
_tmp144="pattern contains a repeated variable";_tag_dynforward(_tmp144,sizeof(
char),_get_zero_arr_size(_tmp144,37));}));;_pop_region(r);}return _tmp142;}void
Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){void*
_tmp145=(void*)p->r;struct Cyc_Absyn_Pat*_tmp146;struct Cyc_List_List*_tmp147;
struct Cyc_List_List*_tmp148;struct Cyc_List_List*_tmp149;_LL8F: if(_tmp145 <= (void*)
2)goto _LL97;if(*((int*)_tmp145)!= 4)goto _LL91;_tmp146=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp145)->f1;_LL90: Cyc_Tcpat_check_pat_regions(te,_tmp146);{void*_tmp14A=(void*)((
struct Cyc_Core_Opt*)_check_null(p->topt))->v;struct Cyc_Absyn_PtrInfo _tmp14B;
struct Cyc_Absyn_PtrAtts _tmp14C;void*_tmp14D;struct Cyc_Absyn_TunionInfo _tmp14E;
void*_tmp14F;_LL9A: if(_tmp14A <= (void*)4)goto _LL9E;if(*((int*)_tmp14A)!= 4)goto
_LL9C;_tmp14B=((struct Cyc_Absyn_PointerType_struct*)_tmp14A)->f1;_tmp14C=_tmp14B.ptr_atts;
_tmp14D=(void*)_tmp14C.rgn;_LL9B: _tmp14F=_tmp14D;goto _LL9D;_LL9C: if(*((int*)
_tmp14A)!= 2)goto _LL9E;_tmp14E=((struct Cyc_Absyn_TunionType_struct*)_tmp14A)->f1;
_tmp14F=(void*)_tmp14E.rgn;_LL9D: Cyc_Tcenv_check_rgn_accessible(te,p->loc,
_tmp14F);return;_LL9E:;_LL9F:({void*_tmp150[0]={};((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp151="check_pat_regions: bad pointer type";
_tag_dynforward(_tmp151,sizeof(char),_get_zero_arr_size(_tmp151,36));}),
_tag_dynforward(_tmp150,sizeof(void*),0));});_LL99:;}_LL91: if(*((int*)_tmp145)!= 
6)goto _LL93;_tmp147=((struct Cyc_Absyn_Tunion_p_struct*)_tmp145)->f3;_LL92: for(0;
_tmp147 != 0;_tmp147=_tmp147->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp147->hd);}{void*_tmp152=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_TunionInfo _tmp153;void*_tmp154;_LLA1: if(_tmp152 <= (void*)4)goto
_LLA5;if(*((int*)_tmp152)!= 2)goto _LLA3;_tmp153=((struct Cyc_Absyn_TunionType_struct*)
_tmp152)->f1;_tmp154=(void*)_tmp153.rgn;_LLA2: Cyc_Tcenv_check_rgn_accessible(te,
p->loc,_tmp154);return;_LLA3: if(*((int*)_tmp152)!= 3)goto _LLA5;_LLA4: return;
_LLA5:;_LLA6:({void*_tmp155[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp156="check_pat_regions: bad tunion type";
_tag_dynforward(_tmp156,sizeof(char),_get_zero_arr_size(_tmp156,35));}),
_tag_dynforward(_tmp155,sizeof(void*),0));});_LLA0:;}_LL93: if(*((int*)_tmp145)!= 
5)goto _LL95;_tmp148=((struct Cyc_Absyn_Aggr_p_struct*)_tmp145)->f3;_LL94: for(0;
_tmp148 != 0;_tmp148=_tmp148->tl){Cyc_Tcpat_check_pat_regions(te,(*((struct
_tuple8*)_tmp148->hd)).f2);}return;_LL95: if(*((int*)_tmp145)!= 3)goto _LL97;
_tmp149=((struct Cyc_Absyn_Tuple_p_struct*)_tmp145)->f1;_LL96: for(0;_tmp149 != 0;
_tmp149=_tmp149->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp149->hd);}return;_LL97:;_LL98: return;_LL8E:;}struct Cyc_Tcpat_Name_v_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Tcpat_Int_v_struct{int tag;int f1;};
struct Cyc_Tcpat_Con_s{void*name;int arity;struct Cyc_Core_Opt*span;struct Cyc_Absyn_Pat*
orig_pat;};struct Cyc_Tcpat_Con_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*
f2;};static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*
c2){void*_tmp157=(void*)c1->name;struct _dynforward_ptr _tmp158;int _tmp159;_LLA8:
if(*((int*)_tmp157)!= 0)goto _LLAA;_tmp158=((struct Cyc_Tcpat_Name_v_struct*)
_tmp157)->f1;_LLA9: {void*_tmp15A=(void*)c2->name;struct _dynforward_ptr _tmp15B;
_LLAD: if(*((int*)_tmp15A)!= 0)goto _LLAF;_tmp15B=((struct Cyc_Tcpat_Name_v_struct*)
_tmp15A)->f1;_LLAE: return Cyc_strcmp((struct _dynforward_ptr)_tmp158,(struct
_dynforward_ptr)_tmp15B);_LLAF: if(*((int*)_tmp15A)!= 1)goto _LLAC;_LLB0: return - 1;
_LLAC:;}_LLAA: if(*((int*)_tmp157)!= 1)goto _LLA7;_tmp159=((struct Cyc_Tcpat_Int_v_struct*)
_tmp157)->f1;_LLAB: {void*_tmp15C=(void*)c2->name;int _tmp15D;_LLB2: if(*((int*)
_tmp15C)!= 0)goto _LLB4;_LLB3: return 1;_LLB4: if(*((int*)_tmp15C)!= 1)goto _LLB1;
_tmp15D=((struct Cyc_Tcpat_Int_v_struct*)_tmp15C)->f1;_LLB5: return _tmp159 - 
_tmp15D;_LLB1:;}_LLA7:;}static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct
_RegionHandle*r){return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(
struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={(void*)1};static struct Cyc_Core_Opt Cyc_Tcpat_two_opt={(
void*)2};static struct Cyc_Core_Opt*Cyc_Tcpat_one_opt_ptr=(struct Cyc_Core_Opt*)&
Cyc_Tcpat_one_opt;static char _tmp15F[5]="NULL";static struct Cyc_Tcpat_Name_v_struct
Cyc_Tcpat_null_name_value={0,{_tmp15F,_tmp15F + 5}};static char _tmp161[2]="&";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={0,{_tmp161,_tmp161 + 
2}};static char _tmp163[2]="$";static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={
0,{_tmp163,_tmp163 + 2}};static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct Cyc_Absyn_Pat*
p){return({struct Cyc_Tcpat_Con_s*_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164->name=(
void*)((void*)& Cyc_Tcpat_null_name_value);_tmp164->arity=0;_tmp164->span=(struct
Cyc_Core_Opt*)& Cyc_Tcpat_two_opt;_tmp164->orig_pat=(struct Cyc_Absyn_Pat*)p;
_tmp164;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct Cyc_Absyn_Pat*
p){return({struct Cyc_Tcpat_Con_s*_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165->name=(
void*)((void*)& Cyc_Tcpat_ptr_name_value);_tmp165->arity=1;_tmp165->span=(struct
Cyc_Core_Opt*)& Cyc_Tcpat_two_opt;_tmp165->orig_pat=(struct Cyc_Absyn_Pat*)p;
_tmp165;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166->name=(
void*)((void*)& Cyc_Tcpat_ptr_name_value);_tmp166->arity=1;_tmp166->span=(struct
Cyc_Core_Opt*)& Cyc_Tcpat_one_opt;_tmp166->orig_pat=(struct Cyc_Absyn_Pat*)p;
_tmp166;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i,struct Cyc_Absyn_Pat*
p){return({struct Cyc_Tcpat_Con_s*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167->name=(
void*)((void*)({struct Cyc_Tcpat_Int_v_struct*_tmp168=_cycalloc_atomic(sizeof(*
_tmp168));_tmp168[0]=({struct Cyc_Tcpat_Int_v_struct _tmp169;_tmp169.tag=1;_tmp169.f1=
i;_tmp169;});_tmp168;}));_tmp167->arity=0;_tmp167->span=0;_tmp167->orig_pat=p;
_tmp167;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct
_dynforward_ptr f,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*_tmp16A=
_cycalloc(sizeof(*_tmp16A));_tmp16A->name=(void*)((void*)({struct Cyc_Tcpat_Name_v_struct*
_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B[0]=({struct Cyc_Tcpat_Name_v_struct
_tmp16C;_tmp16C.tag=0;_tmp16C.f1=f;_tmp16C;});_tmp16B;}));_tmp16A->arity=0;
_tmp16A->span=0;_tmp16A->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp16A;});}static
struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){return({
struct Cyc_Tcpat_Con_s*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D->name=(void*)((
void*)({struct Cyc_Tcpat_Int_v_struct*_tmp16F=_cycalloc_atomic(sizeof(*_tmp16F));
_tmp16F[0]=({struct Cyc_Tcpat_Int_v_struct _tmp170;_tmp170.tag=1;_tmp170.f1=(int)c;
_tmp170;});_tmp16F;}));_tmp16D->arity=0;_tmp16D->span=({struct Cyc_Core_Opt*
_tmp16E=_cycalloc_atomic(sizeof(*_tmp16E));_tmp16E->v=(void*)256;_tmp16E;});
_tmp16D->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp16D;});}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_tuple_con(int i,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*
_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->name=(void*)((void*)& Cyc_Tcpat_tuple_name_value);
_tmp171->arity=i;_tmp171->span=Cyc_Tcpat_one_opt_ptr;_tmp171->orig_pat=p;_tmp171;});}
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172[0]=({struct Cyc_Tcpat_Con_struct
_tmp173;_tmp173.tag=0;_tmp173.f1=Cyc_Tcpat_null_con(p);_tmp173.f2=0;_tmp173;});
_tmp172;});}static void*Cyc_Tcpat_int_pat(int i,struct Cyc_Absyn_Pat*p){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174[0]=({
struct Cyc_Tcpat_Con_struct _tmp175;_tmp175.tag=0;_tmp175.f1=Cyc_Tcpat_int_con(i,p);
_tmp175.f2=0;_tmp175;});_tmp174;});}static void*Cyc_Tcpat_char_pat(char c,struct
Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp176=_cycalloc(
sizeof(*_tmp176));_tmp176[0]=({struct Cyc_Tcpat_Con_struct _tmp177;_tmp177.tag=0;
_tmp177.f1=Cyc_Tcpat_char_con(c,p);_tmp177.f2=0;_tmp177;});_tmp176;});}static
void*Cyc_Tcpat_float_pat(struct _dynforward_ptr f,struct Cyc_Absyn_Pat*p){return(
void*)({struct Cyc_Tcpat_Con_struct*_tmp178=_cycalloc(sizeof(*_tmp178));_tmp178[0]=({
struct Cyc_Tcpat_Con_struct _tmp179;_tmp179.tag=0;_tmp179.f1=Cyc_Tcpat_float_con(f,
p);_tmp179.f2=0;_tmp179;});_tmp178;});}static void*Cyc_Tcpat_null_ptr_pat(void*p,
struct Cyc_Absyn_Pat*p0){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp17A=
_cycalloc(sizeof(*_tmp17A));_tmp17A[0]=({struct Cyc_Tcpat_Con_struct _tmp17B;
_tmp17B.tag=0;_tmp17B.f1=Cyc_Tcpat_null_ptr_con(p0);_tmp17B.f2=({struct Cyc_List_List*
_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->hd=(void*)p;_tmp17C->tl=0;_tmp17C;});
_tmp17B;});_tmp17A;});}static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*
p0){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp17D=_cycalloc(sizeof(*_tmp17D));
_tmp17D[0]=({struct Cyc_Tcpat_Con_struct _tmp17E;_tmp17E.tag=0;_tmp17E.f1=Cyc_Tcpat_ptr_con(
p0);_tmp17E.f2=({struct Cyc_List_List*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F->hd=(
void*)p;_tmp17F->tl=0;_tmp17F;});_tmp17E;});_tmp17D;});}static void*Cyc_Tcpat_tuple_pat(
struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180[0]=({struct Cyc_Tcpat_Con_struct
_tmp181;_tmp181.tag=0;_tmp181.f1=Cyc_Tcpat_tuple_con(Cyc_List_length(ss),p);
_tmp181.f2=ss;_tmp181;});_tmp180;});}static void*Cyc_Tcpat_con_pat(struct
_dynforward_ptr con_name,struct Cyc_Core_Opt*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*
p){struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*_tmp184=_cycalloc(sizeof(*
_tmp184));_tmp184->name=(void*)((void*)({struct Cyc_Tcpat_Name_v_struct*_tmp185=
_cycalloc(sizeof(*_tmp185));_tmp185[0]=({struct Cyc_Tcpat_Name_v_struct _tmp186;
_tmp186.tag=0;_tmp186.f1=con_name;_tmp186;});_tmp185;}));_tmp184->arity=Cyc_List_length(
ps);_tmp184->span=span;_tmp184->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp184;});
return(void*)({struct Cyc_Tcpat_Con_struct*_tmp182=_cycalloc(sizeof(*_tmp182));
_tmp182[0]=({struct Cyc_Tcpat_Con_struct _tmp183;_tmp183.tag=0;_tmp183.f1=c;
_tmp183.f2=ps;_tmp183;});_tmp182;});}static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*
p){void*s;{void*_tmp187=(void*)p->r;void*_tmp188;int _tmp189;char _tmp18A;struct
_dynforward_ptr _tmp18B;struct Cyc_Absyn_Pat*_tmp18C;struct Cyc_Absyn_Pat*_tmp18D;
struct Cyc_Absyn_Pat*_tmp18E;struct Cyc_Absyn_Tuniondecl*_tmp18F;struct Cyc_Absyn_Tunionfield*
_tmp190;struct Cyc_List_List*_tmp191;struct Cyc_List_List*_tmp192;struct Cyc_List_List*
_tmp193;struct Cyc_Absyn_AggrInfo _tmp194;void*_tmp195;struct Cyc_Absyn_Aggrdecl**
_tmp196;struct Cyc_Absyn_Aggrdecl*_tmp197;struct Cyc_List_List*_tmp198;struct Cyc_Absyn_Enumdecl*
_tmp199;struct Cyc_Absyn_Enumfield*_tmp19A;void*_tmp19B;struct Cyc_Absyn_Enumfield*
_tmp19C;_LLB7: if((int)_tmp187 != 0)goto _LLB9;_LLB8: goto _LLBA;_LLB9: if(_tmp187 <= (
void*)2)goto _LLBB;if(*((int*)_tmp187)!= 2)goto _LLBB;_LLBA: s=(void*)0;goto _LLB6;
_LLBB: if((int)_tmp187 != 1)goto _LLBD;_LLBC: s=Cyc_Tcpat_null_pat(p);goto _LLB6;
_LLBD: if(_tmp187 <= (void*)2)goto _LLBF;if(*((int*)_tmp187)!= 7)goto _LLBF;_tmp188=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp187)->f1;_tmp189=((struct Cyc_Absyn_Int_p_struct*)
_tmp187)->f2;_LLBE: s=Cyc_Tcpat_int_pat(_tmp189,(struct Cyc_Absyn_Pat*)p);goto
_LLB6;_LLBF: if(_tmp187 <= (void*)2)goto _LLC1;if(*((int*)_tmp187)!= 8)goto _LLC1;
_tmp18A=((struct Cyc_Absyn_Char_p_struct*)_tmp187)->f1;_LLC0: s=Cyc_Tcpat_char_pat(
_tmp18A,p);goto _LLB6;_LLC1: if(_tmp187 <= (void*)2)goto _LLC3;if(*((int*)_tmp187)!= 
9)goto _LLC3;_tmp18B=((struct Cyc_Absyn_Float_p_struct*)_tmp187)->f1;_LLC2: s=Cyc_Tcpat_float_pat(
_tmp18B,p);goto _LLB6;_LLC3: if(_tmp187 <= (void*)2)goto _LLC5;if(*((int*)_tmp187)!= 
0)goto _LLC5;_tmp18C=((struct Cyc_Absyn_Var_p_struct*)_tmp187)->f2;_LLC4: s=Cyc_Tcpat_compile_pat(
_tmp18C);goto _LLB6;_LLC5: if(_tmp187 <= (void*)2)goto _LLC7;if(*((int*)_tmp187)!= 1)
goto _LLC7;_tmp18D=((struct Cyc_Absyn_Reference_p_struct*)_tmp187)->f2;_LLC6: s=Cyc_Tcpat_compile_pat(
_tmp18D);goto _LLB6;_LLC7: if(_tmp187 <= (void*)2)goto _LLC9;if(*((int*)_tmp187)!= 4)
goto _LLC9;_tmp18E=((struct Cyc_Absyn_Pointer_p_struct*)_tmp187)->f1;_LLC8:{void*
_tmp19D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp19E;struct Cyc_Absyn_PtrAtts _tmp19F;struct Cyc_Absyn_Conref*
_tmp1A0;_LLDE: if(_tmp19D <= (void*)4)goto _LLE2;if(*((int*)_tmp19D)!= 4)goto _LLE0;
_tmp19E=((struct Cyc_Absyn_PointerType_struct*)_tmp19D)->f1;_tmp19F=_tmp19E.ptr_atts;
_tmp1A0=_tmp19F.nullable;_LLDF: {int is_nullable=0;int still_working=1;while(
still_working){void*_tmp1A1=(void*)_tmp1A0->v;struct Cyc_Absyn_Conref*_tmp1A2;int
_tmp1A3;_LLE5: if(_tmp1A1 <= (void*)1)goto _LLE7;if(*((int*)_tmp1A1)!= 1)goto _LLE7;
_tmp1A2=((struct Cyc_Absyn_Forward_constr_struct*)_tmp1A1)->f1;_LLE6:(void*)(
_tmp1A0->v=(void*)((void*)_tmp1A2->v));continue;_LLE7: if((int)_tmp1A1 != 0)goto
_LLE9;_LLE8:(void*)(_tmp1A0->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp1A4=_cycalloc(sizeof(*_tmp1A4));_tmp1A4[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp1A5;_tmp1A5.tag=0;_tmp1A5.f1=(void*)0;_tmp1A5;});_tmp1A4;})));is_nullable=0;
still_working=0;goto _LLE4;_LLE9: if(_tmp1A1 <= (void*)1)goto _LLE4;if(*((int*)
_tmp1A1)!= 0)goto _LLE4;_tmp1A3=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp1A1)->f1;
_LLEA: is_nullable=(int)_tmp1A3;still_working=0;goto _LLE4;_LLE4:;}{void*ss=Cyc_Tcpat_compile_pat(
_tmp18E);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(ss,p);else{s=Cyc_Tcpat_ptr_pat(
ss,p);}goto _LLDD;}}_LLE0: if(*((int*)_tmp19D)!= 2)goto _LLE2;_LLE1:{void*_tmp1A6=(
void*)_tmp18E->r;struct Cyc_Absyn_Tuniondecl*_tmp1A7;struct Cyc_Absyn_Tunionfield*
_tmp1A8;struct Cyc_List_List*_tmp1A9;_LLEC: if(_tmp1A6 <= (void*)2)goto _LLEE;if(*((
int*)_tmp1A6)!= 6)goto _LLEE;_tmp1A7=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1A6)->f1;
_tmp1A8=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1A6)->f2;_tmp1A9=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp1A6)->f3;_LLED: {struct Cyc_Core_Opt*span;if(_tmp1A7->is_xtunion)span=0;else{
span=({struct Cyc_Core_Opt*_tmp1AA=_cycalloc_atomic(sizeof(*_tmp1AA));_tmp1AA->v=(
void*)((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1A7->fields))->v);_tmp1AA;});}s=Cyc_Tcpat_con_pat(*(*
_tmp1A8->name).f2,span,((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp1A9),p);goto _LLEB;}
_LLEE:;_LLEF:({void*_tmp1AB[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1AC="non-[x]tunion pattern has tunion type";
_tag_dynforward(_tmp1AC,sizeof(char),_get_zero_arr_size(_tmp1AC,38));}),
_tag_dynforward(_tmp1AB,sizeof(void*),0));});_LLEB:;}goto _LLDD;_LLE2:;_LLE3:({
void*_tmp1AD[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1AE="pointer pattern does not have pointer type";
_tag_dynforward(_tmp1AE,sizeof(char),_get_zero_arr_size(_tmp1AE,43));}),
_tag_dynforward(_tmp1AD,sizeof(void*),0));});_LLDD:;}goto _LLB6;_LLC9: if(_tmp187
<= (void*)2)goto _LLCB;if(*((int*)_tmp187)!= 6)goto _LLCB;_tmp18F=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp187)->f1;_tmp190=((struct Cyc_Absyn_Tunion_p_struct*)_tmp187)->f2;_tmp191=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp187)->f3;if(_tmp191 != 0)goto _LLCB;_LLCA: {
struct Cyc_Core_Opt*span;{void*_tmp1AF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v);_LLF1: if(_tmp1AF <= (void*)4)goto _LLF5;if(*((int*)
_tmp1AF)!= 2)goto _LLF3;_LLF2: if(_tmp18F->is_xtunion)span=0;else{span=({struct Cyc_Core_Opt*
_tmp1B0=_cycalloc_atomic(sizeof(*_tmp1B0));_tmp1B0->v=(void*)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp18F->fields))->v);_tmp1B0;});}goto _LLF0;_LLF3: if(*((int*)_tmp1AF)!= 3)goto
_LLF5;_LLF4: span=({struct Cyc_Core_Opt*_tmp1B1=_cycalloc_atomic(sizeof(*_tmp1B1));
_tmp1B1->v=(void*)1;_tmp1B1;});goto _LLF0;_LLF5:;_LLF6: span=({void*_tmp1B2[0]={};((
struct Cyc_Core_Opt*(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1B3="void tunion pattern has bad type";_tag_dynforward(_tmp1B3,
sizeof(char),_get_zero_arr_size(_tmp1B3,33));}),_tag_dynforward(_tmp1B2,sizeof(
void*),0));});goto _LLF0;_LLF0:;}s=Cyc_Tcpat_con_pat(*(*_tmp190->name).f2,span,0,
p);goto _LLB6;}_LLCB: if(_tmp187 <= (void*)2)goto _LLCD;if(*((int*)_tmp187)!= 6)goto
_LLCD;_tmp192=((struct Cyc_Absyn_Tunion_p_struct*)_tmp187)->f3;_LLCC: _tmp193=
_tmp192;goto _LLCE;_LLCD: if(_tmp187 <= (void*)2)goto _LLCF;if(*((int*)_tmp187)!= 3)
goto _LLCF;_tmp193=((struct Cyc_Absyn_Tuple_p_struct*)_tmp187)->f1;_LLCE: s=Cyc_Tcpat_tuple_pat(((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp193),(struct Cyc_Absyn_Pat*)p);goto _LLB6;
_LLCF: if(_tmp187 <= (void*)2)goto _LLD1;if(*((int*)_tmp187)!= 5)goto _LLD1;_tmp194=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp187)->f1;_tmp195=(void*)_tmp194.aggr_info;if(*((
int*)_tmp195)!= 1)goto _LLD1;_tmp196=((struct Cyc_Absyn_KnownAggr_struct*)_tmp195)->f1;
_tmp197=*_tmp196;_tmp198=((struct Cyc_Absyn_Aggr_p_struct*)_tmp187)->f3;_LLD0: {
struct Cyc_List_List*ps=0;{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp197->impl))->fields;for(0;fields != 0;fields=fields->tl){int found=
Cyc_strcmp((struct _dynforward_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,({
const char*_tmp1C2="";_tag_dynforward(_tmp1C2,sizeof(char),_get_zero_arr_size(
_tmp1C2,1));}))== 0;{struct Cyc_List_List*dlps0=_tmp198;for(0;!found  && dlps0 != 0;
dlps0=dlps0->tl){struct _tuple8 _tmp1B5;struct Cyc_List_List*_tmp1B6;struct Cyc_Absyn_Pat*
_tmp1B7;struct _tuple8*_tmp1B4=(struct _tuple8*)dlps0->hd;_tmp1B5=*_tmp1B4;_tmp1B6=
_tmp1B5.f1;_tmp1B7=_tmp1B5.f2;{struct Cyc_List_List*_tmp1B8=_tmp1B6;struct Cyc_List_List
_tmp1B9;void*_tmp1BA;struct _dynforward_ptr*_tmp1BB;struct Cyc_List_List*_tmp1BC;
_LLF8: if(_tmp1B8 == 0)goto _LLFA;_tmp1B9=*_tmp1B8;_tmp1BA=(void*)_tmp1B9.hd;if(*((
int*)_tmp1BA)!= 1)goto _LLFA;_tmp1BB=((struct Cyc_Absyn_FieldName_struct*)_tmp1BA)->f1;
_tmp1BC=_tmp1B9.tl;if(_tmp1BC != 0)goto _LLFA;_LLF9: if(Cyc_strptrcmp(_tmp1BB,((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){ps=({struct Cyc_List_List*
_tmp1BD=_cycalloc(sizeof(*_tmp1BD));_tmp1BD->hd=(void*)Cyc_Tcpat_compile_pat(
_tmp1B7);_tmp1BD->tl=ps;_tmp1BD;});found=1;}goto _LLF7;_LLFA:;_LLFB:({void*
_tmp1BE[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1BF="bad designator(s)";_tag_dynforward(_tmp1BF,sizeof(char),
_get_zero_arr_size(_tmp1BF,18));}),_tag_dynforward(_tmp1BE,sizeof(void*),0));});
_LLF7:;}}}if(!found)({void*_tmp1C0[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1C1="bad designator";
_tag_dynforward(_tmp1C1,sizeof(char),_get_zero_arr_size(_tmp1C1,15));}),
_tag_dynforward(_tmp1C0,sizeof(void*),0));});}}s=Cyc_Tcpat_tuple_pat(ps,(struct
Cyc_Absyn_Pat*)p);goto _LLB6;}_LLD1: if(_tmp187 <= (void*)2)goto _LLD3;if(*((int*)
_tmp187)!= 10)goto _LLD3;_tmp199=((struct Cyc_Absyn_Enum_p_struct*)_tmp187)->f1;
_tmp19A=((struct Cyc_Absyn_Enum_p_struct*)_tmp187)->f2;_LLD2: {int span=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp199->fields))->v);s=Cyc_Tcpat_con_pat(*(*_tmp19A->name).f2,({
struct Cyc_Core_Opt*_tmp1C3=_cycalloc_atomic(sizeof(*_tmp1C3));_tmp1C3->v=(void*)
span;_tmp1C3;}),0,p);goto _LLB6;}_LLD3: if(_tmp187 <= (void*)2)goto _LLD5;if(*((int*)
_tmp187)!= 11)goto _LLD5;_tmp19B=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp187)->f1;_tmp19C=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp187)->f2;_LLD4: {
struct Cyc_List_List*fields;{void*_tmp1C4=Cyc_Tcutil_compress(_tmp19B);struct Cyc_List_List*
_tmp1C5;_LLFD: if(_tmp1C4 <= (void*)4)goto _LLFF;if(*((int*)_tmp1C4)!= 13)goto _LLFF;
_tmp1C5=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp1C4)->f1;_LLFE: fields=_tmp1C5;
goto _LLFC;_LLFF:;_LL100:({void*_tmp1C6[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1C7="bad type in AnonEnum_p";
_tag_dynforward(_tmp1C7,sizeof(char),_get_zero_arr_size(_tmp1C7,23));}),
_tag_dynforward(_tmp1C6,sizeof(void*),0));});_LLFC:;}{int span=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(fields);s=Cyc_Tcpat_con_pat(*(*_tmp19C->name).f2,({struct Cyc_Core_Opt*
_tmp1C8=_cycalloc_atomic(sizeof(*_tmp1C8));_tmp1C8->v=(void*)span;_tmp1C8;}),0,p);
goto _LLB6;}}_LLD5: if(_tmp187 <= (void*)2)goto _LLD7;if(*((int*)_tmp187)!= 12)goto
_LLD7;_LLD6: goto _LLD8;_LLD7: if(_tmp187 <= (void*)2)goto _LLD9;if(*((int*)_tmp187)
!= 13)goto _LLD9;_LLD8: goto _LLDA;_LLD9: if(_tmp187 <= (void*)2)goto _LLDB;if(*((int*)
_tmp187)!= 5)goto _LLDB;_LLDA: goto _LLDC;_LLDB: if(_tmp187 <= (void*)2)goto _LLB6;if(*((
int*)_tmp187)!= 14)goto _LLB6;_LLDC: s=(void*)0;_LLB6:;}return s;}struct Cyc_Tcpat_Pos_struct{
int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{
int tag;struct Cyc_Set_Set*f1;};struct Cyc_Tcpat_Failure_struct{int tag;void*f1;};
struct Cyc_Tcpat_Success_struct{int tag;void*f1;};struct Cyc_Tcpat_IfEq_struct{int
tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};struct
_tuple11{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*
c){void*_tmp1C9=td;struct Cyc_Set_Set*_tmp1CA;_LL102: if(*((int*)_tmp1C9)!= 1)goto
_LL104;_tmp1CA=((struct Cyc_Tcpat_Neg_struct*)_tmp1C9)->f1;_LL103: if(((int(*)(
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp1CA,c))({void*
_tmp1CB[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1CC="add_neg called when constructor already in set";
_tag_dynforward(_tmp1CC,sizeof(char),_get_zero_arr_size(_tmp1CC,47));}),
_tag_dynforward(_tmp1CB,sizeof(void*),0));});if(c->span != 0  && ((int(*)(struct
Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp1CA)+ 1 >= (int)((struct Cyc_Core_Opt*)
_check_null(c->span))->v)({void*_tmp1CD[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1CE="add_neg called when |cs U {c}| >= span(c)";
_tag_dynforward(_tmp1CE,sizeof(char),_get_zero_arr_size(_tmp1CE,42));}),
_tag_dynforward(_tmp1CD,sizeof(void*),0));});return(void*)({struct Cyc_Tcpat_Neg_struct*
_tmp1CF=_region_malloc(r,sizeof(*_tmp1CF));_tmp1CF[0]=({struct Cyc_Tcpat_Neg_struct
_tmp1D0;_tmp1D0.tag=1;_tmp1D0.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp1CA,c);
_tmp1D0;});_tmp1CF;});_LL104: if(*((int*)_tmp1C9)!= 0)goto _LL101;_LL105:({void*
_tmp1D1[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1D2="add_neg called when td is Positive";_tag_dynforward(_tmp1D2,
sizeof(char),_get_zero_arr_size(_tmp1D2,35));}),_tag_dynforward(_tmp1D1,sizeof(
void*),0));});_LL101:;}static void*Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,
void*td){void*_tmp1D3=td;struct Cyc_Tcpat_Con_s*_tmp1D4;struct Cyc_Set_Set*_tmp1D5;
_LL107: if(*((int*)_tmp1D3)!= 0)goto _LL109;_tmp1D4=((struct Cyc_Tcpat_Pos_struct*)
_tmp1D3)->f1;_LL108: if(Cyc_Tcpat_compare_con(c,_tmp1D4)== 0)return(void*)0;else{
return(void*)1;}_LL109: if(*((int*)_tmp1D3)!= 1)goto _LL106;_tmp1D5=((struct Cyc_Tcpat_Neg_struct*)
_tmp1D3)->f1;_LL10A: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))
Cyc_Set_member)(_tmp1D5,c))return(void*)1;else{if(c->span != 0  && (int)((struct
Cyc_Core_Opt*)_check_null(c->span))->v == ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp1D5)+ 1)return(void*)0;else{return(void*)2;}}_LL106:;}struct _tuple12{struct
Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*Cyc_Tcpat_augment(
struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){struct Cyc_List_List*
_tmp1D6=ctxt;struct Cyc_List_List _tmp1D7;struct _tuple12*_tmp1D8;struct _tuple12
_tmp1D9;struct Cyc_Tcpat_Con_s*_tmp1DA;struct Cyc_List_List*_tmp1DB;struct Cyc_List_List*
_tmp1DC;_LL10C: if(_tmp1D6 != 0)goto _LL10E;_LL10D: return 0;_LL10E: if(_tmp1D6 == 0)
goto _LL10B;_tmp1D7=*_tmp1D6;_tmp1D8=(struct _tuple12*)_tmp1D7.hd;_tmp1D9=*_tmp1D8;
_tmp1DA=_tmp1D9.f1;_tmp1DB=_tmp1D9.f2;_tmp1DC=_tmp1D7.tl;_LL10F: return({struct
Cyc_List_List*_tmp1DD=_region_malloc(r,sizeof(*_tmp1DD));_tmp1DD->hd=({struct
_tuple12*_tmp1DE=_region_malloc(r,sizeof(*_tmp1DE));_tmp1DE->f1=_tmp1DA;_tmp1DE->f2=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp1DF=_region_malloc(r,sizeof(*
_tmp1DF));_tmp1DF->hd=(void*)dsc;_tmp1DF->tl=_tmp1DB;_tmp1DF;});_tmp1DE;});
_tmp1DD->tl=_tmp1DC;_tmp1DD;});_LL10B:;}static struct Cyc_List_List*Cyc_Tcpat_norm_context(
struct _RegionHandle*r,struct Cyc_List_List*ctxt){struct Cyc_List_List*_tmp1E0=ctxt;
struct Cyc_List_List _tmp1E1;struct _tuple12*_tmp1E2;struct _tuple12 _tmp1E3;struct
Cyc_Tcpat_Con_s*_tmp1E4;struct Cyc_List_List*_tmp1E5;struct Cyc_List_List*_tmp1E6;
_LL111: if(_tmp1E0 != 0)goto _LL113;_LL112:({void*_tmp1E7[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp1E8="norm_context: empty context";_tag_dynforward(_tmp1E8,sizeof(char),
_get_zero_arr_size(_tmp1E8,28));}),_tag_dynforward(_tmp1E7,sizeof(void*),0));});
_LL113: if(_tmp1E0 == 0)goto _LL110;_tmp1E1=*_tmp1E0;_tmp1E2=(struct _tuple12*)
_tmp1E1.hd;_tmp1E3=*_tmp1E2;_tmp1E4=_tmp1E3.f1;_tmp1E5=_tmp1E3.f2;_tmp1E6=
_tmp1E1.tl;_LL114: return Cyc_Tcpat_augment(r,_tmp1E6,(void*)({struct Cyc_Tcpat_Pos_struct*
_tmp1E9=_region_malloc(r,sizeof(*_tmp1E9));_tmp1E9[0]=({struct Cyc_Tcpat_Pos_struct
_tmp1EA;_tmp1EA.tag=0;_tmp1EA.f1=_tmp1E4;_tmp1EA.f2=Cyc_List_rrev(r,_tmp1E5);
_tmp1EA;});_tmp1E9;}));_LL110:;}static void*Cyc_Tcpat_build_desc(struct
_RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
struct _tuple6 _tmp1EC=({struct _tuple6 _tmp1EB;_tmp1EB.f1=ctxt;_tmp1EB.f2=work;
_tmp1EB;});struct Cyc_List_List*_tmp1ED;struct Cyc_List_List*_tmp1EE;struct Cyc_List_List*
_tmp1EF;struct Cyc_List_List*_tmp1F0;struct Cyc_List_List*_tmp1F1;struct Cyc_List_List
_tmp1F2;struct _tuple12*_tmp1F3;struct _tuple12 _tmp1F4;struct Cyc_Tcpat_Con_s*
_tmp1F5;struct Cyc_List_List*_tmp1F6;struct Cyc_List_List*_tmp1F7;struct Cyc_List_List*
_tmp1F8;struct Cyc_List_List _tmp1F9;struct _tuple11*_tmp1FA;struct _tuple11 _tmp1FB;
struct Cyc_List_List*_tmp1FC;struct Cyc_List_List*_tmp1FD;_LL116: _tmp1ED=_tmp1EC.f1;
if(_tmp1ED != 0)goto _LL118;_tmp1EE=_tmp1EC.f2;if(_tmp1EE != 0)goto _LL118;_LL117:
return dsc;_LL118: _tmp1EF=_tmp1EC.f1;if(_tmp1EF != 0)goto _LL11A;_LL119: goto _LL11B;
_LL11A: _tmp1F0=_tmp1EC.f2;if(_tmp1F0 != 0)goto _LL11C;_LL11B:({void*_tmp1FE[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1FF="build_desc: ctxt and work don't match";_tag_dynforward(
_tmp1FF,sizeof(char),_get_zero_arr_size(_tmp1FF,38));}),_tag_dynforward(_tmp1FE,
sizeof(void*),0));});_LL11C: _tmp1F1=_tmp1EC.f1;if(_tmp1F1 == 0)goto _LL115;_tmp1F2=*
_tmp1F1;_tmp1F3=(struct _tuple12*)_tmp1F2.hd;_tmp1F4=*_tmp1F3;_tmp1F5=_tmp1F4.f1;
_tmp1F6=_tmp1F4.f2;_tmp1F7=_tmp1F2.tl;_tmp1F8=_tmp1EC.f2;if(_tmp1F8 == 0)goto
_LL115;_tmp1F9=*_tmp1F8;_tmp1FA=(struct _tuple11*)_tmp1F9.hd;_tmp1FB=*_tmp1FA;
_tmp1FC=_tmp1FB.f3;_tmp1FD=_tmp1F9.tl;_LL11D: {struct Cyc_Tcpat_Pos_struct*
_tmp200=({struct Cyc_Tcpat_Pos_struct*_tmp201=_region_malloc(r,sizeof(*_tmp201));
_tmp201[0]=({struct Cyc_Tcpat_Pos_struct _tmp202;_tmp202.tag=0;_tmp202.f1=_tmp1F5;
_tmp202.f2=Cyc_List_rappend(r,Cyc_List_rrev(r,_tmp1F6),({struct Cyc_List_List*
_tmp203=_region_malloc(r,sizeof(*_tmp203));_tmp203->hd=(void*)dsc;_tmp203->tl=
_tmp1FC;_tmp203;}));_tmp202;});_tmp201;});return Cyc_Tcpat_build_desc(r,_tmp1F7,(
void*)_tmp200,_tmp1FD);}_LL115:;}static void*Cyc_Tcpat_match(struct _RegionHandle*,
void*,struct Cyc_List_List*,void*,struct Cyc_List_List*,struct Cyc_List_List*,void*,
struct Cyc_List_List*);static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*
dsc,struct Cyc_List_List*allmrules){struct Cyc_List_List*_tmp204=allmrules;struct
Cyc_List_List _tmp205;struct _tuple0*_tmp206;struct _tuple0 _tmp207;void*_tmp208;
void*_tmp209;struct Cyc_List_List*_tmp20A;_LL11F: if(_tmp204 != 0)goto _LL121;_LL120:
return(void*)({struct Cyc_Tcpat_Failure_struct*_tmp20B=_cycalloc(sizeof(*_tmp20B));
_tmp20B[0]=({struct Cyc_Tcpat_Failure_struct _tmp20C;_tmp20C.tag=0;_tmp20C.f1=(
void*)dsc;_tmp20C;});_tmp20B;});_LL121: if(_tmp204 == 0)goto _LL11E;_tmp205=*
_tmp204;_tmp206=(struct _tuple0*)_tmp205.hd;_tmp207=*_tmp206;_tmp208=_tmp207.f1;
_tmp209=_tmp207.f2;_tmp20A=_tmp205.tl;_LL122: return Cyc_Tcpat_match(r,_tmp208,0,
dsc,0,0,_tmp209,_tmp20A);_LL11E:;}static void*Cyc_Tcpat_match_compile(struct
_RegionHandle*r,struct Cyc_List_List*allmrules){return Cyc_Tcpat_or_match(r,(void*)({
struct Cyc_Tcpat_Neg_struct*_tmp20D=_region_malloc(r,sizeof(*_tmp20D));_tmp20D[0]=({
struct Cyc_Tcpat_Neg_struct _tmp20E;_tmp20E.tag=1;_tmp20E.f1=Cyc_Tcpat_empty_con_set(
r);_tmp20E;});_tmp20D;}),allmrules);}static void*Cyc_Tcpat_and_match(struct
_RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp20F=work;
struct Cyc_List_List _tmp210;struct _tuple11*_tmp211;struct _tuple11 _tmp212;struct
Cyc_List_List*_tmp213;struct Cyc_List_List*_tmp214;struct Cyc_List_List*_tmp215;
struct Cyc_List_List*_tmp216;struct Cyc_List_List _tmp217;struct _tuple11*_tmp218;
struct _tuple11 _tmp219;struct Cyc_List_List*_tmp21A;struct Cyc_List_List*_tmp21B;
struct Cyc_List_List*_tmp21C;struct Cyc_List_List*_tmp21D;_LL124: if(_tmp20F != 0)
goto _LL126;_LL125: return(void*)({struct Cyc_Tcpat_Success_struct*_tmp21E=
_region_malloc(r,sizeof(*_tmp21E));_tmp21E[0]=({struct Cyc_Tcpat_Success_struct
_tmp21F;_tmp21F.tag=1;_tmp21F.f1=(void*)right_hand_side;_tmp21F;});_tmp21E;});
_LL126: if(_tmp20F == 0)goto _LL128;_tmp210=*_tmp20F;_tmp211=(struct _tuple11*)
_tmp210.hd;_tmp212=*_tmp211;_tmp213=_tmp212.f1;if(_tmp213 != 0)goto _LL128;_tmp214=
_tmp212.f2;if(_tmp214 != 0)goto _LL128;_tmp215=_tmp212.f3;if(_tmp215 != 0)goto
_LL128;_tmp216=_tmp210.tl;_LL127: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp216,right_hand_side,rules);_LL128: if(_tmp20F == 0)goto _LL123;_tmp217=*
_tmp20F;_tmp218=(struct _tuple11*)_tmp217.hd;_tmp219=*_tmp218;_tmp21A=_tmp219.f1;
_tmp21B=_tmp219.f2;_tmp21C=_tmp219.f3;_tmp21D=_tmp217.tl;_LL129: if((_tmp21A == 0
 || _tmp21B == 0) || _tmp21C == 0)({void*_tmp220[0]={};Cyc_Tcutil_impos(({const
char*_tmp221="tcpat:and_match: malformed work frame";_tag_dynforward(_tmp221,
sizeof(char),_get_zero_arr_size(_tmp221,38));}),_tag_dynforward(_tmp220,sizeof(
void*),0));});{struct Cyc_List_List _tmp223;void*_tmp224;struct Cyc_List_List*
_tmp225;struct Cyc_List_List*_tmp222=(struct Cyc_List_List*)_tmp21A;_tmp223=*
_tmp222;_tmp224=(void*)_tmp223.hd;_tmp225=_tmp223.tl;{struct Cyc_List_List _tmp227;
struct Cyc_List_List*_tmp228;struct Cyc_List_List*_tmp229;struct Cyc_List_List*
_tmp226=(struct Cyc_List_List*)_tmp21B;_tmp227=*_tmp226;_tmp228=(struct Cyc_List_List*)
_tmp227.hd;_tmp229=_tmp227.tl;{struct Cyc_List_List _tmp22B;void*_tmp22C;struct Cyc_List_List*
_tmp22D;struct Cyc_List_List*_tmp22A=(struct Cyc_List_List*)_tmp21C;_tmp22B=*
_tmp22A;_tmp22C=(void*)_tmp22B.hd;_tmp22D=_tmp22B.tl;{struct _tuple11*_tmp22E=({
struct _tuple11*_tmp230=_region_malloc(r,sizeof(*_tmp230));_tmp230->f1=_tmp225;
_tmp230->f2=_tmp229;_tmp230->f3=_tmp22D;_tmp230;});return Cyc_Tcpat_match(r,
_tmp224,_tmp228,_tmp22C,ctx,({struct Cyc_List_List*_tmp22F=_region_malloc(r,
sizeof(*_tmp22F));_tmp22F->hd=_tmp22E;_tmp22F->tl=_tmp21D;_tmp22F;}),
right_hand_side,rules);}}}}_LL123:;}static struct Cyc_List_List*Cyc_Tcpat_getdargs(
struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){void*_tmp231=dsc;
struct Cyc_Set_Set*_tmp232;struct Cyc_List_List*_tmp233;_LL12B: if(*((int*)_tmp231)
!= 1)goto _LL12D;_tmp232=((struct Cyc_Tcpat_Neg_struct*)_tmp231)->f1;_LL12C: {void*
any=(void*)({struct Cyc_Tcpat_Neg_struct*_tmp236=_region_malloc(r,sizeof(*_tmp236));
_tmp236[0]=({struct Cyc_Tcpat_Neg_struct _tmp237;_tmp237.tag=1;_tmp237.f1=Cyc_Tcpat_empty_con_set(
r);_tmp237;});_tmp236;});struct Cyc_List_List*_tmp234=0;{int i=0;for(0;i < pcon->arity;
++ i){_tmp234=({struct Cyc_List_List*_tmp235=_region_malloc(r,sizeof(*_tmp235));
_tmp235->hd=(void*)any;_tmp235->tl=_tmp234;_tmp235;});}}return _tmp234;}_LL12D:
if(*((int*)_tmp231)!= 0)goto _LL12A;_tmp233=((struct Cyc_Tcpat_Pos_struct*)_tmp231)->f2;
_LL12E: return _tmp233;_LL12A:;}struct _tuple13{struct _RegionHandle*f1;struct Cyc_List_List*
f2;};static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple13*env,int i){struct
_tuple13 _tmp239;struct _RegionHandle*_tmp23A;struct Cyc_List_List*_tmp23B;struct
_tuple13*_tmp238=env;_tmp239=*_tmp238;_tmp23A=_tmp239.f1;_tmp23B=_tmp239.f2;
return({struct Cyc_List_List*_tmp23C=_region_malloc(_tmp23A,sizeof(*_tmp23C));
_tmp23C->hd=(void*)(i + 1);_tmp23C->tl=_tmp23B;_tmp23C;});}static struct Cyc_List_List*
Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*
obj){struct _tuple13 _tmp23D=({struct _tuple13 _tmp23E;_tmp23E.f1=r;_tmp23E.f2=obj;
_tmp23E;});return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*
f)(struct _tuple13*,int),struct _tuple13*env))Cyc_List_rtabulate_c)(r,pcon->arity,
Cyc_Tcpat_getoarg,& _tmp23D);}static void*Cyc_Tcpat_match(struct _RegionHandle*r,
void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp23F=p;struct Cyc_Tcpat_Con_s*
_tmp240;struct Cyc_List_List*_tmp241;_LL130: if((int)_tmp23F != 0)goto _LL132;_LL131:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL132: if(_tmp23F <= (void*)1)goto _LL12F;if(*((int*)_tmp23F)!= 0)goto
_LL12F;_tmp240=((struct Cyc_Tcpat_Con_struct*)_tmp23F)->f1;_tmp241=((struct Cyc_Tcpat_Con_struct*)
_tmp23F)->f2;_LL133: {void*_tmp242=Cyc_Tcpat_static_match(_tmp240,dsc);_LL135:
if((int)_tmp242 != 0)goto _LL137;_LL136: {struct Cyc_List_List*_tmp243=({struct Cyc_List_List*
_tmp248=_region_malloc(r,sizeof(*_tmp248));_tmp248->hd=({struct _tuple12*_tmp249=
_region_malloc(r,sizeof(*_tmp249));_tmp249->f1=_tmp240;_tmp249->f2=0;_tmp249;});
_tmp248->tl=ctx;_tmp248;});struct _tuple11*_tmp244=({struct _tuple11*_tmp247=
_region_malloc(r,sizeof(*_tmp247));_tmp247->f1=_tmp241;_tmp247->f2=Cyc_Tcpat_getoargs(
r,_tmp240,obj);_tmp247->f3=Cyc_Tcpat_getdargs(r,_tmp240,dsc);_tmp247;});struct
Cyc_List_List*_tmp245=({struct Cyc_List_List*_tmp246=_region_malloc(r,sizeof(*
_tmp246));_tmp246->hd=_tmp244;_tmp246->tl=work;_tmp246;});return Cyc_Tcpat_and_match(
r,_tmp243,_tmp245,right_hand_side,rules);}_LL137: if((int)_tmp242 != 1)goto _LL139;
_LL138: return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);
_LL139: if((int)_tmp242 != 2)goto _LL134;_LL13A: {struct Cyc_List_List*_tmp24A=({
struct Cyc_List_List*_tmp253=_region_malloc(r,sizeof(*_tmp253));_tmp253->hd=({
struct _tuple12*_tmp254=_region_malloc(r,sizeof(*_tmp254));_tmp254->f1=_tmp240;
_tmp254->f2=0;_tmp254;});_tmp253->tl=ctx;_tmp253;});struct _tuple11*_tmp24B=({
struct _tuple11*_tmp252=_region_malloc(r,sizeof(*_tmp252));_tmp252->f1=_tmp241;
_tmp252->f2=Cyc_Tcpat_getoargs(r,_tmp240,obj);_tmp252->f3=Cyc_Tcpat_getdargs(r,
_tmp240,dsc);_tmp252;});struct Cyc_List_List*_tmp24C=({struct Cyc_List_List*
_tmp251=_region_malloc(r,sizeof(*_tmp251));_tmp251->hd=_tmp24B;_tmp251->tl=work;
_tmp251;});void*_tmp24D=Cyc_Tcpat_and_match(r,_tmp24A,_tmp24C,right_hand_side,
rules);void*_tmp24E=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(
r,dsc,_tmp240),work),rules);return(void*)({struct Cyc_Tcpat_IfEq_struct*_tmp24F=
_region_malloc(r,sizeof(*_tmp24F));_tmp24F[0]=({struct Cyc_Tcpat_IfEq_struct
_tmp250;_tmp250.tag=2;_tmp250.f1=obj;_tmp250.f2=_tmp240;_tmp250.f3=(void*)
_tmp24D;_tmp250.f4=(void*)_tmp24E;_tmp250;});_tmp24F;});}_LL134:;}_LL12F:;}
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),
void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){void*
_tmp255=d;void*_tmp256;void*_tmp257;void*_tmp258;void*_tmp259;_LL13C: if(*((int*)
_tmp255)!= 0)goto _LL13E;_tmp256=(void*)((struct Cyc_Tcpat_Failure_struct*)_tmp255)->f1;
_LL13D: if(!(*exhaust_done)){not_exhaust(env1,_tmp256);*exhaust_done=1;}goto
_LL13B;_LL13E: if(*((int*)_tmp255)!= 1)goto _LL140;_tmp257=(void*)((struct Cyc_Tcpat_Success_struct*)
_tmp255)->f1;_LL13F: rhs_appears(env2,_tmp257);goto _LL13B;_LL140: if(*((int*)
_tmp255)!= 2)goto _LL13B;_tmp258=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp255)->f3;
_tmp259=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp255)->f4;_LL141: Cyc_Tcpat_check_exhaust_overlap(
_tmp258,not_exhaust,env1,rhs_appears,env2,exhaust_done);Cyc_Tcpat_check_exhaust_overlap(
_tmp259,not_exhaust,env1,rhs_appears,env2,exhaust_done);goto _LL13B;_LL13B:;}
struct _tuple14{int f1;struct Cyc_Position_Segment*f2;};struct _tuple15{void*f1;
struct _tuple14*f2;};struct _tuple16{struct _RegionHandle*f1;int*f2;};static struct
_tuple15*Cyc_Tcpat_get_match(struct _tuple16*env,struct Cyc_Absyn_Switch_clause*
swc){struct _tuple16 _tmp25B;struct _RegionHandle*_tmp25C;int*_tmp25D;struct
_tuple16*_tmp25A=env;_tmp25B=*_tmp25A;_tmp25C=_tmp25B.f1;_tmp25D=_tmp25B.f2;{
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);struct _tuple14*rhs=({struct _tuple14*
_tmp263=_region_malloc(_tmp25C,sizeof(*_tmp263));_tmp263->f1=0;_tmp263->f2=(swc->pattern)->loc;
_tmp263;});void*sp;if(swc->where_clause != 0){sp=Cyc_Tcpat_tuple_pat(({struct Cyc_List_List*
_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E->hd=(void*)sp0;_tmp25E->tl=({struct
Cyc_List_List*_tmp25F=_cycalloc(sizeof(*_tmp25F));_tmp25F->hd=(void*)Cyc_Tcpat_int_pat(*
_tmp25D,0);_tmp25F->tl=0;_tmp25F;});_tmp25E;}),0);*_tmp25D=*_tmp25D + 1;}else{sp=
Cyc_Tcpat_tuple_pat(({struct Cyc_List_List*_tmp260=_cycalloc(sizeof(*_tmp260));
_tmp260->hd=(void*)sp0;_tmp260->tl=({struct Cyc_List_List*_tmp261=_cycalloc(
sizeof(*_tmp261));_tmp261->hd=(void*)((void*)0);_tmp261->tl=0;_tmp261;});_tmp260;}),
0);}return({struct _tuple15*_tmp262=_region_malloc(_tmp25C,sizeof(*_tmp262));
_tmp262->f1=sp;_tmp262->f2=rhs;_tmp262;});}}void Cyc_Tcpat_add_con_to_list(struct
Cyc_Tcpat_Con_s*c,struct Cyc_List_List**cs){*cs=({struct Cyc_List_List*_tmp264=
_cycalloc(sizeof(*_tmp264));_tmp264->hd=c;_tmp264->tl=*cs;_tmp264;});}char Cyc_Tcpat_Desc2string[
16]="\000\000\000\000Desc2string\000";static struct _dynforward_ptr Cyc_Tcpat_descs2string(
struct Cyc_List_List*);static struct _dynforward_ptr Cyc_Tcpat_desc2string(void*d){
void*_tmp265=d;struct Cyc_Tcpat_Con_s*_tmp266;struct Cyc_List_List*_tmp267;struct
Cyc_Set_Set*_tmp268;_LL143: if(*((int*)_tmp265)!= 0)goto _LL145;_tmp266=((struct
Cyc_Tcpat_Pos_struct*)_tmp265)->f1;_tmp267=((struct Cyc_Tcpat_Pos_struct*)_tmp265)->f2;
_LL144: {void*_tmp269=(void*)_tmp266->name;struct Cyc_Absyn_Pat*_tmp26A=_tmp266->orig_pat;
if(_tmp26A == 0)return Cyc_Tcpat_desc2string((void*)((struct Cyc_List_List*)
_check_null(_tmp267))->hd);{void*_tmp26B=(void*)_tmp26A->r;struct Cyc_Absyn_Vardecl*
_tmp26C;struct Cyc_Absyn_Vardecl*_tmp26D;struct Cyc_Absyn_Tvar*_tmp26E;struct Cyc_Absyn_Vardecl*
_tmp26F;struct Cyc_Absyn_Pat*_tmp270;struct Cyc_Absyn_Pat _tmp271;void*_tmp272;
struct Cyc_Absyn_Tunionfield*_tmp273;struct Cyc_Absyn_AggrInfo _tmp274;void*_tmp275;
struct Cyc_Absyn_Aggrdecl**_tmp276;struct Cyc_Absyn_Aggrdecl*_tmp277;struct Cyc_Absyn_Tunionfield*
_tmp278;int _tmp279;char _tmp27A;struct _dynforward_ptr _tmp27B;struct Cyc_Absyn_Enumfield*
_tmp27C;struct Cyc_Absyn_Enumfield*_tmp27D;struct Cyc_Absyn_Exp*_tmp27E;_LL148: if((
int)_tmp26B != 0)goto _LL14A;_LL149: return({const char*_tmp27F="_";_tag_dynforward(
_tmp27F,sizeof(char),_get_zero_arr_size(_tmp27F,2));});_LL14A: if(_tmp26B <= (void*)
2)goto _LL15A;if(*((int*)_tmp26B)!= 0)goto _LL14C;_tmp26C=((struct Cyc_Absyn_Var_p_struct*)
_tmp26B)->f1;_LL14B: return Cyc_Absynpp_qvar2string(_tmp26C->name);_LL14C: if(*((
int*)_tmp26B)!= 1)goto _LL14E;_tmp26D=((struct Cyc_Absyn_Reference_p_struct*)
_tmp26B)->f1;_LL14D: return(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp282;_tmp282.tag=0;_tmp282.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp26D->name));{void*_tmp280[1]={& _tmp282};Cyc_aprintf(({
const char*_tmp281="*%s";_tag_dynforward(_tmp281,sizeof(char),_get_zero_arr_size(
_tmp281,4));}),_tag_dynforward(_tmp280,sizeof(void*),1));}});_LL14E: if(*((int*)
_tmp26B)!= 2)goto _LL150;_tmp26E=((struct Cyc_Absyn_TagInt_p_struct*)_tmp26B)->f1;
_tmp26F=((struct Cyc_Absyn_TagInt_p_struct*)_tmp26B)->f2;_LL14F: return(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp286;_tmp286.tag=0;_tmp286.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp26E->name);{struct Cyc_String_pa_struct
_tmp285;_tmp285.tag=0;_tmp285.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp26F->name));{void*_tmp283[2]={& _tmp285,& _tmp286};Cyc_aprintf(({
const char*_tmp284="%s<`%s>";_tag_dynforward(_tmp284,sizeof(char),
_get_zero_arr_size(_tmp284,8));}),_tag_dynforward(_tmp283,sizeof(void*),2));}}});
_LL150: if(*((int*)_tmp26B)!= 3)goto _LL152;_LL151: return(struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp289;_tmp289.tag=0;_tmp289.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcpat_descs2string(_tmp267));{void*
_tmp287[1]={& _tmp289};Cyc_aprintf(({const char*_tmp288="$(%s)";_tag_dynforward(
_tmp288,sizeof(char),_get_zero_arr_size(_tmp288,6));}),_tag_dynforward(_tmp287,
sizeof(void*),1));}});_LL152: if(*((int*)_tmp26B)!= 4)goto _LL154;_tmp270=((struct
Cyc_Absyn_Pointer_p_struct*)_tmp26B)->f1;_tmp271=*_tmp270;_tmp272=(void*)_tmp271.r;
if(_tmp272 <= (void*)2)goto _LL154;if(*((int*)_tmp272)!= 6)goto _LL154;_tmp273=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp272)->f2;_LL153: return(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp28D;_tmp28D.tag=0;_tmp28D.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcpat_descs2string(_tmp267));{
struct Cyc_String_pa_struct _tmp28C;_tmp28C.tag=0;_tmp28C.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp273->name));{
void*_tmp28A[2]={& _tmp28C,& _tmp28D};Cyc_aprintf(({const char*_tmp28B="&%s(%s)";
_tag_dynforward(_tmp28B,sizeof(char),_get_zero_arr_size(_tmp28B,8));}),
_tag_dynforward(_tmp28A,sizeof(void*),2));}}});_LL154: if(*((int*)_tmp26B)!= 4)
goto _LL156;_LL155: return(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp290;_tmp290.tag=0;_tmp290.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Tcpat_descs2string(_tmp267));{void*_tmp28E[1]={& _tmp290};Cyc_aprintf(({const
char*_tmp28F="&%s";_tag_dynforward(_tmp28F,sizeof(char),_get_zero_arr_size(
_tmp28F,4));}),_tag_dynforward(_tmp28E,sizeof(void*),1));}});_LL156: if(*((int*)
_tmp26B)!= 5)goto _LL158;_tmp274=((struct Cyc_Absyn_Aggr_p_struct*)_tmp26B)->f1;
_tmp275=(void*)_tmp274.aggr_info;if(*((int*)_tmp275)!= 1)goto _LL158;_tmp276=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp275)->f1;_tmp277=*_tmp276;_LL157: return(
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp294;_tmp294.tag=0;_tmp294.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcpat_descs2string(_tmp267));{
struct Cyc_String_pa_struct _tmp293;_tmp293.tag=0;_tmp293.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp277->name));{
void*_tmp291[2]={& _tmp293,& _tmp294};Cyc_aprintf(({const char*_tmp292="%s{%s}";
_tag_dynforward(_tmp292,sizeof(char),_get_zero_arr_size(_tmp292,7));}),
_tag_dynforward(_tmp291,sizeof(void*),2));}}});_LL158: if(*((int*)_tmp26B)!= 6)
goto _LL15A;_tmp278=((struct Cyc_Absyn_Tunion_p_struct*)_tmp26B)->f2;_LL159: return(
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp297;_tmp297.tag=0;_tmp297.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp278->name));{
void*_tmp295[1]={& _tmp297};Cyc_aprintf(({const char*_tmp296="%s";_tag_dynforward(
_tmp296,sizeof(char),_get_zero_arr_size(_tmp296,3));}),_tag_dynforward(_tmp295,
sizeof(void*),1));}});_LL15A: if((int)_tmp26B != 1)goto _LL15C;_LL15B: return({const
char*_tmp298="NULL";_tag_dynforward(_tmp298,sizeof(char),_get_zero_arr_size(
_tmp298,5));});_LL15C: if(_tmp26B <= (void*)2)goto _LL168;if(*((int*)_tmp26B)!= 7)
goto _LL15E;_tmp279=((struct Cyc_Absyn_Int_p_struct*)_tmp26B)->f2;_LL15D: return(
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp29B;_tmp29B.tag=1;_tmp29B.f1=(
unsigned long)_tmp279;{void*_tmp299[1]={& _tmp29B};Cyc_aprintf(({const char*
_tmp29A="%d";_tag_dynforward(_tmp29A,sizeof(char),_get_zero_arr_size(_tmp29A,3));}),
_tag_dynforward(_tmp299,sizeof(void*),1));}});_LL15E: if(*((int*)_tmp26B)!= 8)
goto _LL160;_tmp27A=((struct Cyc_Absyn_Char_p_struct*)_tmp26B)->f1;_LL15F: return(
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp29E;_tmp29E.tag=1;_tmp29E.f1=(
unsigned long)((int)_tmp27A);{void*_tmp29C[1]={& _tmp29E};Cyc_aprintf(({const char*
_tmp29D="%d";_tag_dynforward(_tmp29D,sizeof(char),_get_zero_arr_size(_tmp29D,3));}),
_tag_dynforward(_tmp29C,sizeof(void*),1));}});_LL160: if(*((int*)_tmp26B)!= 9)
goto _LL162;_tmp27B=((struct Cyc_Absyn_Float_p_struct*)_tmp26B)->f1;_LL161: return
_tmp27B;_LL162: if(*((int*)_tmp26B)!= 10)goto _LL164;_tmp27C=((struct Cyc_Absyn_Enum_p_struct*)
_tmp26B)->f2;_LL163: _tmp27D=_tmp27C;goto _LL165;_LL164: if(*((int*)_tmp26B)!= 11)
goto _LL166;_tmp27D=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp26B)->f2;_LL165:
return Cyc_Absynpp_qvar2string(_tmp27D->name);_LL166: if(*((int*)_tmp26B)!= 14)
goto _LL168;_tmp27E=((struct Cyc_Absyn_Exp_p_struct*)_tmp26B)->f1;_LL167: return Cyc_Absynpp_exp2string(
_tmp27E);_LL168:;_LL169:(int)_throw((void*)Cyc_Tcpat_Desc2string);_LL147:;}}
_LL145: if(*((int*)_tmp265)!= 1)goto _LL142;_tmp268=((struct Cyc_Tcpat_Neg_struct*)
_tmp265)->f1;_LL146: if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp268))
return({const char*_tmp29F="_";_tag_dynforward(_tmp29F,sizeof(char),
_get_zero_arr_size(_tmp29F,2));});{struct Cyc_Tcpat_Con_s*_tmp2A0=((struct Cyc_Tcpat_Con_s*(*)(
struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp268);struct Cyc_Absyn_Pat*_tmp2A1=
_tmp2A0->orig_pat;if(_tmp2A1 == 0)(int)_throw((void*)Cyc_Tcpat_Desc2string);{void*
_tmp2A2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2A1->topt))->v);
void*_tmp2A3;struct Cyc_Absyn_PtrInfo _tmp2A4;struct Cyc_Absyn_PtrAtts _tmp2A5;
struct Cyc_Absyn_TunionInfo _tmp2A6;void*_tmp2A7;struct Cyc_Absyn_Tuniondecl**
_tmp2A8;struct Cyc_Absyn_Tuniondecl*_tmp2A9;_LL16B: if(_tmp2A2 <= (void*)4)goto
_LL173;if(*((int*)_tmp2A2)!= 5)goto _LL16D;_tmp2A3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2A2)->f2;if((int)_tmp2A3 != 0)goto _LL16D;_LL16C:{int i=0;for(0;i < 256;i ++){
struct Cyc_Tcpat_Con_s*_tmp2AA=Cyc_Tcpat_char_con((char)i,(struct Cyc_Absyn_Pat*)
_tmp2A1);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp268,_tmp2AA))return(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp2AD;
_tmp2AD.tag=1;_tmp2AD.f1=(unsigned long)i;{void*_tmp2AB[1]={& _tmp2AD};Cyc_aprintf(({
const char*_tmp2AC="%d";_tag_dynforward(_tmp2AC,sizeof(char),_get_zero_arr_size(
_tmp2AC,3));}),_tag_dynforward(_tmp2AB,sizeof(void*),1));}});}}(int)_throw((void*)
Cyc_Tcpat_Desc2string);_LL16D: if(*((int*)_tmp2A2)!= 5)goto _LL16F;_LL16E:{
unsigned int i=0;for(0;i < 0 - 1;i ++){struct Cyc_Tcpat_Con_s*_tmp2AE=Cyc_Tcpat_int_con((
int)i,_tmp2A1);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp268,_tmp2AE))return(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp2B1;
_tmp2B1.tag=1;_tmp2B1.f1=(unsigned long)((int)i);{void*_tmp2AF[1]={& _tmp2B1};Cyc_aprintf(({
const char*_tmp2B0="%d";_tag_dynforward(_tmp2B0,sizeof(char),_get_zero_arr_size(
_tmp2B0,3));}),_tag_dynforward(_tmp2AF,sizeof(void*),1));}});}}(int)_throw((void*)
Cyc_Tcpat_Desc2string);_LL16F: if(*((int*)_tmp2A2)!= 4)goto _LL171;_tmp2A4=((
struct Cyc_Absyn_PointerType_struct*)_tmp2A2)->f1;_tmp2A5=_tmp2A4.ptr_atts;_LL170: {
struct Cyc_Absyn_Conref*_tmp2B2=_tmp2A5.nullable;int is_nullable=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2B2);if(is_nullable){if(!((
int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp268,
Cyc_Tcpat_null_con((struct Cyc_Absyn_Pat*)_tmp2A1)))return({const char*_tmp2B3="NULL";
_tag_dynforward(_tmp2B3,sizeof(char),_get_zero_arr_size(_tmp2B3,5));});}return({
const char*_tmp2B4="&_";_tag_dynforward(_tmp2B4,sizeof(char),_get_zero_arr_size(
_tmp2B4,3));});}_LL171: if(*((int*)_tmp2A2)!= 2)goto _LL173;_tmp2A6=((struct Cyc_Absyn_TunionType_struct*)
_tmp2A2)->f1;_tmp2A7=(void*)_tmp2A6.tunion_info;if(*((int*)_tmp2A7)!= 1)goto
_LL173;_tmp2A8=((struct Cyc_Absyn_KnownTunion_struct*)_tmp2A7)->f1;_tmp2A9=*
_tmp2A8;_LL172: if(_tmp2A9->is_xtunion)(int)_throw((void*)Cyc_Tcpat_Desc2string);{
struct Cyc_List_List*_tmp2B5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2A9->fields))->v;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2B5);for(0;(unsigned int)_tmp2B5;_tmp2B5=_tmp2B5->tl){struct _dynforward_ptr n=*(*((
struct Cyc_Absyn_Tunionfield*)_tmp2B5->hd)->name).f2;struct Cyc_List_List*_tmp2B6=((
struct Cyc_Absyn_Tunionfield*)_tmp2B5->hd)->typs;if(!((int(*)(struct Cyc_Set_Set*s,
struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp268,({struct Cyc_Tcpat_Con_s*
_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7->name=(void*)((void*)({struct Cyc_Tcpat_Name_v_struct*
_tmp2B8=_cycalloc(sizeof(*_tmp2B8));_tmp2B8[0]=({struct Cyc_Tcpat_Name_v_struct
_tmp2B9;_tmp2B9.tag=0;_tmp2B9.f1=n;_tmp2B9;});_tmp2B8;}));_tmp2B7->arity=0;
_tmp2B7->span=0;_tmp2B7->orig_pat=_tmp2A1;_tmp2B7;}))){if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp2B6)== 0)return n;else{return(struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp2BC;_tmp2BC.tag=0;_tmp2BC.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)n);{void*_tmp2BA[1]={& _tmp2BC};Cyc_aprintf(({
const char*_tmp2BB="&%s(...)";_tag_dynforward(_tmp2BB,sizeof(char),
_get_zero_arr_size(_tmp2BB,9));}),_tag_dynforward(_tmp2BA,sizeof(void*),1));}});}}}(
int)_throw((void*)Cyc_Tcpat_Desc2string);}_LL173:;_LL174:(int)_throw((void*)Cyc_Tcpat_Desc2string);
_LL16A:;}}_LL142:;}static struct _dynforward_ptr*Cyc_Tcpat_desc2stringptr(void*d){
return({struct _dynforward_ptr*_tmp2BD=_cycalloc(sizeof(*_tmp2BD));_tmp2BD[0]=Cyc_Tcpat_desc2string(
d);_tmp2BD;});}static struct _dynforward_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*
ds){struct Cyc_List_List*_tmp2BE=((struct Cyc_List_List*(*)(struct _dynforward_ptr*(*
f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_desc2stringptr,ds);
struct _dynforward_ptr*comma=_init_dynforward_ptr(_cycalloc(sizeof(struct
_dynforward_ptr)),",",sizeof(char),2);{struct Cyc_List_List*_tmp2BF=_tmp2BE;for(0;
_tmp2BF != 0;_tmp2BF=((struct Cyc_List_List*)_check_null(_tmp2BF))->tl){if(_tmp2BF->tl
!= 0){_tmp2BF->tl=({struct Cyc_List_List*_tmp2C0=_cycalloc(sizeof(*_tmp2C0));
_tmp2C0->hd=comma;_tmp2C0->tl=_tmp2BF->tl;_tmp2C0;});_tmp2BF=_tmp2BF->tl;}}}
return(struct _dynforward_ptr)Cyc_strconcat_l(_tmp2BE);}static void Cyc_Tcpat_not_exhaust_err(
struct Cyc_Position_Segment*loc,void*desc){struct _handler_cons _tmp2C2;
_push_handler(& _tmp2C2);{int _tmp2C4=0;if(setjmp(_tmp2C2.handler))_tmp2C4=1;if(!
_tmp2C4){{struct _dynforward_ptr _tmp2C5=Cyc_Tcpat_desc2string(desc);({struct Cyc_String_pa_struct
_tmp2C8;_tmp2C8.tag=0;_tmp2C8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp2C5);{void*_tmp2C6[1]={& _tmp2C8};Cyc_Tcutil_terr(loc,({const char*_tmp2C7="patterns may not be exhaustive.\n\tmissing case for %s";
_tag_dynforward(_tmp2C7,sizeof(char),_get_zero_arr_size(_tmp2C7,53));}),
_tag_dynforward(_tmp2C6,sizeof(void*),1));}});};_pop_handler();}else{void*
_tmp2C3=(void*)_exn_thrown;void*_tmp2CA=_tmp2C3;_LL176: if(_tmp2CA != Cyc_Tcpat_Desc2string)
goto _LL178;_LL177:({void*_tmp2CB[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp2CC="patterns may not be exhaustive.";
_tag_dynforward(_tmp2CC,sizeof(char),_get_zero_arr_size(_tmp2CC,32));}),
_tag_dynforward(_tmp2CB,sizeof(void*),0));});goto _LL175;_LL178:;_LL179:(void)
_throw(_tmp2CA);_LL175:;}}}static void Cyc_Tcpat_rule_occurs(int dummy,struct
_tuple14*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*
loc,struct Cyc_List_List*swcs){struct _RegionHandle _tmp2CD=_new_region("r");struct
_RegionHandle*r=& _tmp2CD;_push_region(r);{int _tmp2CE=0;struct _tuple16 _tmp2CF=({
struct _tuple16 _tmp2DB;_tmp2DB.f1=r;_tmp2DB.f2=& _tmp2CE;_tmp2DB;});struct Cyc_List_List*
_tmp2D0=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple15*(*f)(
struct _tuple16*,struct Cyc_Absyn_Switch_clause*),struct _tuple16*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp2CF,swcs);void*_tmp2D1=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp2D0);int _tmp2D2=0;((void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple14*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp2D1,Cyc_Tcpat_not_exhaust_err,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp2D2);for(0;_tmp2D0 != 0;_tmp2D0=_tmp2D0->tl){
struct _tuple15 _tmp2D4;struct _tuple14*_tmp2D5;struct _tuple14 _tmp2D6;int _tmp2D7;
struct Cyc_Position_Segment*_tmp2D8;struct _tuple15*_tmp2D3=(struct _tuple15*)
_tmp2D0->hd;_tmp2D4=*_tmp2D3;_tmp2D5=_tmp2D4.f2;_tmp2D6=*_tmp2D5;_tmp2D7=_tmp2D6.f1;
_tmp2D8=_tmp2D6.f2;if(!_tmp2D7)({void*_tmp2D9[0]={};Cyc_Tcutil_terr(_tmp2D8,({
const char*_tmp2DA="redundant pattern";_tag_dynforward(_tmp2DA,sizeof(char),
_get_zero_arr_size(_tmp2DA,18));}),_tag_dynforward(_tmp2D9,sizeof(void*),0));});}};
_pop_region(r);}struct _tuple17{struct Cyc_Position_Segment*f1;int f2;};static void
Cyc_Tcpat_not_exhaust_warn(struct _tuple17*pr,void*desc){(*pr).f2=0;{struct
_handler_cons _tmp2DC;_push_handler(& _tmp2DC);{int _tmp2DE=0;if(setjmp(_tmp2DC.handler))
_tmp2DE=1;if(!_tmp2DE){{struct _dynforward_ptr _tmp2DF=Cyc_Tcpat_desc2string(desc);({
struct Cyc_String_pa_struct _tmp2E2;_tmp2E2.tag=0;_tmp2E2.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp2DF);{void*_tmp2E0[1]={& _tmp2E2};Cyc_Tcutil_warn((*
pr).f1,({const char*_tmp2E1="pattern not exhaustive.\n\tmissing case for %s";
_tag_dynforward(_tmp2E1,sizeof(char),_get_zero_arr_size(_tmp2E1,45));}),
_tag_dynforward(_tmp2E0,sizeof(void*),1));}});};_pop_handler();}else{void*
_tmp2DD=(void*)_exn_thrown;void*_tmp2E4=_tmp2DD;_LL17B: if(_tmp2E4 != Cyc_Tcpat_Desc2string)
goto _LL17D;_LL17C:({void*_tmp2E5[0]={};Cyc_Tcutil_warn((*pr).f1,({const char*
_tmp2E6="pattern not exhaustive.";_tag_dynforward(_tmp2E6,sizeof(char),
_get_zero_arr_size(_tmp2E6,24));}),_tag_dynforward(_tmp2E5,sizeof(void*),0));});
goto _LL17A;_LL17D:;_LL17E:(void)_throw(_tmp2E4);_LL17A:;}}}}static void Cyc_Tcpat_dummy_fn(
int i,int j){return;}struct _tuple18{void*f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Pat*p){struct _RegionHandle _tmp2E7=
_new_region("r");struct _RegionHandle*r=& _tmp2E7;_push_region(r);{struct Cyc_List_List*
_tmp2E8=({struct Cyc_List_List*_tmp2EE=_region_malloc(r,sizeof(*_tmp2EE));_tmp2EE->hd=({
struct _tuple18*_tmp2EF=_region_malloc(r,sizeof(*_tmp2EF));_tmp2EF->f1=Cyc_Tcpat_compile_pat(
p);_tmp2EF->f2=0;_tmp2EF;});_tmp2EE->tl=0;_tmp2EE;});void*_tmp2E9=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp2E8);struct _tuple17 _tmp2EA=({struct _tuple17 _tmp2ED;_tmp2ED.f1=loc;_tmp2ED.f2=
1;_tmp2ED;});int _tmp2EB=0;((void(*)(void*d,void(*not_exhaust)(struct _tuple17*,
void*),struct _tuple17*env1,void(*rhs_appears)(int,int),int env2,int*exhaust_done))
Cyc_Tcpat_check_exhaust_overlap)(_tmp2E9,Cyc_Tcpat_not_exhaust_warn,& _tmp2EA,Cyc_Tcpat_dummy_fn,
0,& _tmp2EB);{int _tmp2EC=_tmp2EA.f2;_npop_handler(0);return _tmp2EC;}};_pop_region(
r);}static struct _tuple15*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);struct _tuple14*rhs=({struct _tuple14*
_tmp2F1=_cycalloc(sizeof(*_tmp2F1));_tmp2F1->f1=0;_tmp2F1->f2=(swc->pattern)->loc;
_tmp2F1;});return({struct _tuple15*_tmp2F0=_cycalloc(sizeof(*_tmp2F0));_tmp2F0->f1=
sp0;_tmp2F0->f2=rhs;_tmp2F0;});}static void Cyc_Tcpat_not_exhaust_err2(struct Cyc_Position_Segment*
loc,void*d){;}void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*loc,
struct Cyc_List_List*swcs){struct Cyc_List_List*_tmp2F2=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcpat_get_match2,swcs);void*_tmp2F3=((void*(*)(struct _RegionHandle*r,struct
Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(Cyc_Core_heap_region,_tmp2F2);
int _tmp2F4=0;((void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple14*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp2F3,Cyc_Tcpat_not_exhaust_err2,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp2F4);for(0;_tmp2F2 != 0;_tmp2F2=_tmp2F2->tl){
struct _tuple15 _tmp2F6;struct _tuple14*_tmp2F7;struct _tuple14 _tmp2F8;int _tmp2F9;
struct Cyc_Position_Segment*_tmp2FA;struct _tuple15*_tmp2F5=(struct _tuple15*)
_tmp2F2->hd;_tmp2F6=*_tmp2F5;_tmp2F7=_tmp2F6.f2;_tmp2F8=*_tmp2F7;_tmp2F9=_tmp2F8.f1;
_tmp2FA=_tmp2F8.f2;if(!_tmp2F9)({void*_tmp2FB[0]={};Cyc_Tcutil_terr(_tmp2FA,({
const char*_tmp2FC="redundant pattern";_tag_dynforward(_tmp2FC,sizeof(char),
_get_zero_arr_size(_tmp2FC,18));}),_tag_dynforward(_tmp2FB,sizeof(void*),0));});}}
