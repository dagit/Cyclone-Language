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
 struct Cyc_Core_Opt{void*v;};int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
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
struct _dynforward_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct
Cyc___cycFILE*,struct _dynforward_ptr,struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dynforward_ptr f1;}
;struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
int Cyc_printf(struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr
Cyc_vrprintf(struct _RegionHandle*,struct _dynforward_ptr,struct _dynforward_ptr);
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,
void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_map2(void*(*f)(
void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);void Cyc_List_iter(void(*
f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_revappend(struct
Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*
env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*
r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct
_tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_rsplit(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);void*Cyc_List_assoc_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{
struct _dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_List_List*
Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Error{
struct _dynforward_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dynforward_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_elab(struct
Cyc_Position_Segment*,struct _dynforward_ptr);extern char Cyc_Position_Nocontext[14];
extern int Cyc_Position_num_errors;extern int Cyc_Position_max_errors;void Cyc_Position_post_error(
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple1*,struct _tuple1*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);struct
Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*
Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_one_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_empty_effect;
extern struct _tuple1*Cyc_Absyn_tunion_print_arg_qvar;extern struct _tuple1*Cyc_Absyn_tunion_scanf_arg_qvar;
extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct
Cyc_List_List*,struct _dynforward_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dynforward_ptr*);struct _tuple4{struct Cyc_Absyn_Tqual
f1;void*f2;};struct _tuple4*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
struct _dynforward_ptr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(
void*a);struct _tuple5{void*f1;struct _tuple1*f2;};struct _tuple5 Cyc_Absyn_aggr_kinded_name(
void*);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);struct
Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int
expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dynforward_ptr
Cyc_Absynpp_typ2string(void*);struct _dynforward_ptr Cyc_Absynpp_kind2string(void*);
struct _dynforward_ptr Cyc_Absynpp_kindbound2string(void*);struct _dynforward_ptr
Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);struct
_tuple6{void*f1;void*f2;};struct _tuple6*Cyc_Dict_rchoose(struct _RegionHandle*r,
struct Cyc_Dict_Dict*d);struct _tuple6*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict*d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
le;};void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*,void*r1,void*r2);unsigned int Cyc_strlen(struct
_dynforward_ptr s);int Cyc_strcmp(struct _dynforward_ptr s1,struct _dynforward_ptr s2);
int Cyc_strptrcmp(struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct
_dynforward_ptr Cyc_strconcat(struct _dynforward_ptr,struct _dynforward_ptr);struct
_tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(struct
Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);void*
Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void Cyc_Tcutil_flush_warnings();extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
void*Cyc_Tcutil_copy_type(void*t);int Cyc_Tcutil_kind_leq(void*k1,void*k2);void*
Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(void*t);
void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*,void*);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,
struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*,void*);void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*,void*);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_function_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*
e);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;
extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int
Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);void*
Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();int
Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,
void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*
Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple8{struct Cyc_Absyn_Tvar*
f1;void*f2;};struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct
Cyc_Absyn_Tvar*);struct _tuple9{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dynforward_ptr*fn);void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*,int allow_unique);void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void
Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct
Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dynforward_ptr
err_msg);void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,struct Cyc_List_List*);
void Cyc_Tcutil_check_no_unique_region(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*t);void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*,struct
Cyc_Absyn_Conref*);void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*,
unsigned int i,struct Cyc_Absyn_Conref*);int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*
b);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*
rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*,int*is_forward_only);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest,int*is_forward_only);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*,
void*t,struct Cyc_Absyn_Exp*e);struct _tuple10{int f1;void*f2;};struct _tuple10 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_normalize_effect(
void*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int*Cyc_Tcutil_new_tvar_id();
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*);int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_snd_tqt(struct _tuple4*);int Cyc_Tcutil_supports_default(
void*);int Cyc_Tcutil_admits_zero(void*t);int Cyc_Tcutil_is_noreturn(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,
struct Cyc_Absyn_Exp*);void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,
struct Cyc_Position_Segment*,struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcTuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Tuniondecl*);void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,
struct Cyc_Position_Segment*,struct Cyc_Absyn_Enumdecl*);char Cyc_Tcutil_Unify[10]="\000\000\000\000Unify\000";
void Cyc_Tcutil_unify_it(void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(void*)0;int
Cyc_Tcutil_tq1_const=0;void*Cyc_Tcutil_t2_failure=(void*)0;int Cyc_Tcutil_tq2_const=
0;struct _dynforward_ptr Cyc_Tcutil_failure_reason=(struct _dynforward_ptr){(void*)
0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure(){if(Cyc_Position_num_errors >= 
Cyc_Position_max_errors)return;Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
struct _dynforward_ptr s1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp22;_tmp22.tag=0;_tmp22.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure));{struct Cyc_String_pa_struct _tmp21;_tmp21.tag=0;_tmp21.f1=(
struct _dynforward_ptr)(Cyc_Tcutil_tq1_const?({const char*_tmp23="const ";
_tag_dynforward(_tmp23,sizeof(char),_get_zero_arr_size(_tmp23,7));}):({const char*
_tmp24="";_tag_dynforward(_tmp24,sizeof(char),_get_zero_arr_size(_tmp24,1));}));{
void*_tmp1F[2]={& _tmp21,& _tmp22};Cyc_aprintf(({const char*_tmp20="%s%s";
_tag_dynforward(_tmp20,sizeof(char),_get_zero_arr_size(_tmp20,5));}),
_tag_dynforward(_tmp1F,sizeof(void*),2));}}});struct _dynforward_ptr s2=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp1C;_tmp1C.tag=0;_tmp1C.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure));{
struct Cyc_String_pa_struct _tmp1B;_tmp1B.tag=0;_tmp1B.f1=(struct _dynforward_ptr)(
Cyc_Tcutil_tq2_const?({const char*_tmp1D="const ";_tag_dynforward(_tmp1D,sizeof(
char),_get_zero_arr_size(_tmp1D,7));}):({const char*_tmp1E="";_tag_dynforward(
_tmp1E,sizeof(char),_get_zero_arr_size(_tmp1E,1));}));{void*_tmp19[2]={& _tmp1B,&
_tmp1C};Cyc_aprintf(({const char*_tmp1A="%s%s";_tag_dynforward(_tmp1A,sizeof(char),
_get_zero_arr_size(_tmp1A,5));}),_tag_dynforward(_tmp19,sizeof(void*),2));}}});
int pos=2;({struct Cyc_String_pa_struct _tmp2;_tmp2.tag=0;_tmp2.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)s1);{void*_tmp0[1]={& _tmp2};Cyc_fprintf(
Cyc_stderr,({const char*_tmp1="  %s";_tag_dynforward(_tmp1,sizeof(char),
_get_zero_arr_size(_tmp1,5));}),_tag_dynforward(_tmp0,sizeof(void*),1));}});pos +=
_get_dynforward_size(s1,sizeof(char));if(pos + 5 >= 80){({void*_tmp3[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp4="\n\t";_tag_dynforward(_tmp4,sizeof(char),
_get_zero_arr_size(_tmp4,3));}),_tag_dynforward(_tmp3,sizeof(void*),0));});pos=8;}
else{({void*_tmp5[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp6=" ";
_tag_dynforward(_tmp6,sizeof(char),_get_zero_arr_size(_tmp6,2));}),
_tag_dynforward(_tmp5,sizeof(void*),0));});pos ++;}({void*_tmp7[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp8="and ";_tag_dynforward(_tmp8,sizeof(char),
_get_zero_arr_size(_tmp8,5));}),_tag_dynforward(_tmp7,sizeof(void*),0));});pos +=
4;if(pos + _get_dynforward_size(s2,sizeof(char))>= 80){({void*_tmp9[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpA="\n\t";_tag_dynforward(_tmpA,sizeof(char),
_get_zero_arr_size(_tmpA,3));}),_tag_dynforward(_tmp9,sizeof(void*),0));});pos=8;}({
struct Cyc_String_pa_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)s2);{void*_tmpB[1]={& _tmpD};Cyc_fprintf(Cyc_stderr,({const
char*_tmpC="%s ";_tag_dynforward(_tmpC,sizeof(char),_get_zero_arr_size(_tmpC,4));}),
_tag_dynforward(_tmpB,sizeof(void*),1));}});pos +=_get_dynforward_size(s2,sizeof(
char))+ 1;if(pos + 17 >= 80){({void*_tmpE[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmpF="\n\t";_tag_dynforward(_tmpF,sizeof(char),_get_zero_arr_size(_tmpF,3));}),
_tag_dynforward(_tmpE,sizeof(void*),0));});pos=8;}({void*_tmp10[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp11="are not compatible. ";_tag_dynforward(_tmp11,
sizeof(char),_get_zero_arr_size(_tmp11,21));}),_tag_dynforward(_tmp10,sizeof(
void*),0));});pos +=17;if(Cyc_Tcutil_failure_reason.curr != ((struct
_dynforward_ptr)_tag_dynforward(0,0,0)).curr){if(pos + Cyc_strlen((struct
_dynforward_ptr)Cyc_Tcutil_failure_reason)>= 80)({void*_tmp12[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp13="\n\t";_tag_dynforward(_tmp13,sizeof(char),
_get_zero_arr_size(_tmp13,3));}),_tag_dynforward(_tmp12,sizeof(void*),0));});({
struct Cyc_String_pa_struct _tmp16;_tmp16.tag=0;_tmp16.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Tcutil_failure_reason);{void*_tmp14[1]={& _tmp16};Cyc_fprintf(
Cyc_stderr,({const char*_tmp15="%s";_tag_dynforward(_tmp15,sizeof(char),
_get_zero_arr_size(_tmp15,3));}),_tag_dynforward(_tmp14,sizeof(void*),1));}});}({
void*_tmp17[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp18="\n";_tag_dynforward(
_tmp18,sizeof(char),_get_zero_arr_size(_tmp18,2));}),_tag_dynforward(_tmp17,
sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*loc,struct _dynforward_ptr fmt,struct _dynforward_ptr ap){
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dynforward_ptr)Cyc_vrprintf(
Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap){struct _dynforward_ptr msg=(struct _dynforward_ptr)Cyc_vrprintf(
Cyc_Core_heap_region,fmt,ap);({struct Cyc_String_pa_struct _tmp27;_tmp27.tag=0;
_tmp27.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)msg);{void*_tmp25[1]={&
_tmp27};Cyc_fprintf(Cyc_stderr,({const char*_tmp26="Compiler Error (Tcutil::impos): %s\n";
_tag_dynforward(_tmp26,sizeof(char),_get_zero_arr_size(_tmp26,36));}),
_tag_dynforward(_tmp25,sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp28=
_cycalloc(sizeof(*_tmp28));_tmp28[0]=({struct Cyc_Core_Impossible_struct _tmp29;
_tmp29.tag=Cyc_Core_Impossible;_tmp29.f1=msg;_tmp29;});_tmp28;}));}static struct
_dynforward_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){return*tv->name;}
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){for(0;tvs != 0;tvs=tvs->tl){({
struct Cyc_String_pa_struct _tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_kindbound2string((void*)((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind));{struct Cyc_String_pa_struct _tmp2C;_tmp2C.tag=0;_tmp2C.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
tvs->hd));{void*_tmp2A[2]={& _tmp2C,& _tmp2D};Cyc_fprintf(Cyc_stderr,({const char*
_tmp2B="%s::%s ";_tag_dynforward(_tmp2B,sizeof(char),_get_zero_arr_size(_tmp2B,8));}),
_tag_dynforward(_tmp2A,sizeof(void*),2));}}});}({void*_tmp2E[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp2F="\n";_tag_dynforward(_tmp2F,sizeof(char),
_get_zero_arr_size(_tmp2F,2));}),_tag_dynforward(_tmp2E,sizeof(void*),0));});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=
0;static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap){struct
_dynforward_ptr msg=(struct _dynforward_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,
ap);Cyc_Tcutil_warning_segs=({struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*
_tmp30));_tmp30->hd=sg;_tmp30->tl=Cyc_Tcutil_warning_segs;_tmp30;});Cyc_Tcutil_warning_msgs=({
struct Cyc_List_List*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->hd=({struct
_dynforward_ptr*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32[0]=msg;_tmp32;});_tmp31->tl=
Cyc_Tcutil_warning_msgs;_tmp31;});}void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs
== 0)return;({void*_tmp33[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp34="***Warnings***\n";
_tag_dynforward(_tmp34,sizeof(char),_get_zero_arr_size(_tmp34,16));}),
_tag_dynforward(_tmp33,sizeof(void*),0));});{struct Cyc_List_List*_tmp35=Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){({struct Cyc_String_pa_struct _tmp39;_tmp39.tag=
0;_tmp39.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct
_dynforward_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd));{
struct Cyc_String_pa_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*((struct _dynforward_ptr*)((struct Cyc_List_List*)
_check_null(_tmp35))->hd));{void*_tmp36[2]={& _tmp38,& _tmp39};Cyc_fprintf(Cyc_stderr,({
const char*_tmp37="%s: %s\n";_tag_dynforward(_tmp37,sizeof(char),
_get_zero_arr_size(_tmp37,8));}),_tag_dynforward(_tmp36,sizeof(void*),2));}}});
_tmp35=_tmp35->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}({
void*_tmp3A[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp3B="**************\n";
_tag_dynforward(_tmp3B,sizeof(char),_get_zero_arr_size(_tmp3B,16));}),
_tag_dynforward(_tmp3A,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;static int Cyc_Tcutil_fast_tvar_cmp(
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){return*((int*)_check_null(tv1->identity))
- *((int*)_check_null(tv2->identity));}void*Cyc_Tcutil_compress(void*t){void*
_tmp3C=t;struct Cyc_Core_Opt*_tmp3D;void**_tmp3E;void**_tmp3F;void***_tmp40;
struct Cyc_Core_Opt*_tmp41;struct Cyc_Core_Opt**_tmp42;_LL1: if(_tmp3C <= (void*)4)
goto _LL9;if(*((int*)_tmp3C)!= 0)goto _LL3;_tmp3D=((struct Cyc_Absyn_Evar_struct*)
_tmp3C)->f2;if(_tmp3D != 0)goto _LL3;_LL2: goto _LL4;_LL3: if(*((int*)_tmp3C)!= 16)
goto _LL5;_tmp3E=((struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;if(_tmp3E != 0)
goto _LL5;_LL4: return t;_LL5: if(*((int*)_tmp3C)!= 16)goto _LL7;_tmp3F=((struct Cyc_Absyn_TypedefType_struct*)
_tmp3C)->f4;_tmp40=(void***)&((struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;
_LL6: {void*t2=Cyc_Tcutil_compress(*((void**)_check_null(*_tmp40)));if(t2 != *((
void**)_check_null(*_tmp40)))*_tmp40=({void**_tmp43=_cycalloc(sizeof(*_tmp43));
_tmp43[0]=t2;_tmp43;});return t2;}_LL7: if(*((int*)_tmp3C)!= 0)goto _LL9;_tmp41=((
struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;_tmp42=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Evar_struct*)_tmp3C)->f2;_LL8: {void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(*_tmp42))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp42))->v)*_tmp42=({struct Cyc_Core_Opt*_tmp44=_cycalloc(sizeof(*
_tmp44));_tmp44->v=(void*)t2;_tmp44;});return t2;}_LL9:;_LLA: return t;_LL0:;}void*
Cyc_Tcutil_copy_type(void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(
struct Cyc_List_List*ts){return Cyc_List_map(Cyc_Tcutil_copy_type,ts);}static
struct Cyc_Absyn_Conref*Cyc_Tcutil_copy_conref(struct Cyc_Absyn_Conref*c){void*
_tmp45=(void*)c->v;void*_tmp46;struct Cyc_Absyn_Conref*_tmp47;_LLC: if((int)_tmp45
!= 0)goto _LLE;_LLD: return Cyc_Absyn_empty_conref();_LLE: if(_tmp45 <= (void*)1)goto
_LL10;if(*((int*)_tmp45)!= 0)goto _LL10;_tmp46=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp45)->f1;_LLF: return Cyc_Absyn_new_conref(_tmp46);_LL10: if(_tmp45 <= (void*)1)
goto _LLB;if(*((int*)_tmp45)!= 1)goto _LLB;_tmp47=((struct Cyc_Absyn_Forward_constr_struct*)
_tmp45)->f1;_LL11: return Cyc_Tcutil_copy_conref(_tmp47);_LLB:;}static void*Cyc_Tcutil_copy_kindbound(
void*kb){void*_tmp48=Cyc_Absyn_compress_kb(kb);void*_tmp49;void*_tmp4A;_LL13: if(*((
int*)_tmp48)!= 0)goto _LL15;_tmp49=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp48)->f1;
_LL14: return(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp4B=_cycalloc(sizeof(*
_tmp4B));_tmp4B[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp4C;_tmp4C.tag=0;_tmp4C.f1=(
void*)_tmp49;_tmp4C;});_tmp4B;});_LL15: if(*((int*)_tmp48)!= 1)goto _LL17;_LL16:
return(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));
_tmp4D[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp4E;_tmp4E.tag=1;_tmp4E.f1=0;
_tmp4E;});_tmp4D;});_LL17: if(*((int*)_tmp48)!= 2)goto _LL12;_tmp4A=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp48)->f2;_LL18: return(void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp50;_tmp50.tag=2;_tmp50.f1=0;_tmp50.f2=(void*)_tmp4A;_tmp50;});_tmp4F;});
_LL12:;}static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
return({struct Cyc_Absyn_Tvar*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->name=tv->name;
_tmp51->identity=0;_tmp51->kind=(void*)Cyc_Tcutil_copy_kindbound((void*)tv->kind);
_tmp51;});}static struct _tuple2*Cyc_Tcutil_copy_arg(struct _tuple2*arg){struct
_tuple2 _tmp53;struct Cyc_Core_Opt*_tmp54;struct Cyc_Absyn_Tqual _tmp55;void*_tmp56;
struct _tuple2*_tmp52=arg;_tmp53=*_tmp52;_tmp54=_tmp53.f1;_tmp55=_tmp53.f2;_tmp56=
_tmp53.f3;return({struct _tuple2*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->f1=
_tmp54;_tmp57->f2=_tmp55;_tmp57->f3=Cyc_Tcutil_copy_type(_tmp56);_tmp57;});}
static struct _tuple4*Cyc_Tcutil_copy_tqt(struct _tuple4*arg){struct _tuple4 _tmp59;
struct Cyc_Absyn_Tqual _tmp5A;void*_tmp5B;struct _tuple4*_tmp58=arg;_tmp59=*_tmp58;
_tmp5A=_tmp59.f1;_tmp5B=_tmp59.f2;return({struct _tuple4*_tmp5C=_cycalloc(sizeof(*
_tmp5C));_tmp5C->f1=_tmp5A;_tmp5C->f2=Cyc_Tcutil_copy_type(_tmp5B);_tmp5C;});}
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*
f){return({struct Cyc_Absyn_Aggrfield*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->name=
f->name;_tmp5D->tq=f->tq;_tmp5D->type=(void*)Cyc_Tcutil_copy_type((void*)f->type);
_tmp5D->width=f->width;_tmp5D->attributes=f->attributes;_tmp5D;});}static struct
_tuple6*Cyc_Tcutil_copy_rgncmp(struct _tuple6*x){struct _tuple6 _tmp5F;void*_tmp60;
void*_tmp61;struct _tuple6*_tmp5E=x;_tmp5F=*_tmp5E;_tmp60=_tmp5F.f1;_tmp61=_tmp5F.f2;
return({struct _tuple6*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->f1=Cyc_Tcutil_copy_type(
_tmp60);_tmp62->f2=Cyc_Tcutil_copy_type(_tmp61);_tmp62;});}static struct Cyc_Absyn_Enumfield*
Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){return({struct Cyc_Absyn_Enumfield*
_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->name=f->name;_tmp63->tag=f->tag;_tmp63->loc=
f->loc;_tmp63;});}void*Cyc_Tcutil_copy_type(void*t){void*_tmp64=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp65;struct Cyc_Absyn_TunionInfo _tmp66;void*_tmp67;
struct Cyc_List_List*_tmp68;void*_tmp69;struct Cyc_Absyn_TunionFieldInfo _tmp6A;
void*_tmp6B;struct Cyc_List_List*_tmp6C;struct Cyc_Absyn_PtrInfo _tmp6D;void*_tmp6E;
struct Cyc_Absyn_Tqual _tmp6F;struct Cyc_Absyn_PtrAtts _tmp70;void*_tmp71;struct Cyc_Absyn_Conref*
_tmp72;struct Cyc_Absyn_Conref*_tmp73;struct Cyc_Absyn_Conref*_tmp74;struct Cyc_Absyn_PtrLoc*
_tmp75;void*_tmp76;void*_tmp77;int _tmp78;struct Cyc_Absyn_ArrayInfo _tmp79;void*
_tmp7A;struct Cyc_Absyn_Tqual _tmp7B;struct Cyc_Absyn_Exp*_tmp7C;struct Cyc_Absyn_Conref*
_tmp7D;struct Cyc_Position_Segment*_tmp7E;struct Cyc_Absyn_FnInfo _tmp7F;struct Cyc_List_List*
_tmp80;struct Cyc_Core_Opt*_tmp81;void*_tmp82;struct Cyc_List_List*_tmp83;int
_tmp84;struct Cyc_Absyn_VarargInfo*_tmp85;struct Cyc_List_List*_tmp86;struct Cyc_List_List*
_tmp87;struct Cyc_List_List*_tmp88;struct Cyc_Absyn_AggrInfo _tmp89;void*_tmp8A;
void*_tmp8B;struct _tuple1*_tmp8C;struct Cyc_List_List*_tmp8D;struct Cyc_Absyn_AggrInfo
_tmp8E;void*_tmp8F;struct Cyc_Absyn_Aggrdecl**_tmp90;struct Cyc_List_List*_tmp91;
void*_tmp92;struct Cyc_List_List*_tmp93;struct _tuple1*_tmp94;struct Cyc_Absyn_Enumdecl*
_tmp95;struct Cyc_List_List*_tmp96;void*_tmp97;int _tmp98;void*_tmp99;void*_tmp9A;
struct _tuple1*_tmp9B;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_Typedefdecl*
_tmp9D;void*_tmp9E;struct Cyc_List_List*_tmp9F;void*_tmpA0;_LL1A: if((int)_tmp64 != 
0)goto _LL1C;_LL1B: goto _LL1D;_LL1C: if(_tmp64 <= (void*)4)goto _LL28;if(*((int*)
_tmp64)!= 0)goto _LL1E;_LL1D: return t;_LL1E: if(*((int*)_tmp64)!= 1)goto _LL20;
_tmp65=((struct Cyc_Absyn_VarType_struct*)_tmp64)->f1;_LL1F: return(void*)({struct
Cyc_Absyn_VarType_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1[0]=({struct Cyc_Absyn_VarType_struct
_tmpA2;_tmpA2.tag=1;_tmpA2.f1=Cyc_Tcutil_copy_tvar(_tmp65);_tmpA2;});_tmpA1;});
_LL20: if(*((int*)_tmp64)!= 2)goto _LL22;_tmp66=((struct Cyc_Absyn_TunionType_struct*)
_tmp64)->f1;_tmp67=(void*)_tmp66.tunion_info;_tmp68=_tmp66.targs;_tmp69=(void*)
_tmp66.rgn;_LL21: return(void*)({struct Cyc_Absyn_TunionType_struct*_tmpA3=
_cycalloc(sizeof(*_tmpA3));_tmpA3[0]=({struct Cyc_Absyn_TunionType_struct _tmpA4;
_tmpA4.tag=2;_tmpA4.f1=({struct Cyc_Absyn_TunionInfo _tmpA5;_tmpA5.tunion_info=(
void*)_tmp67;_tmpA5.targs=Cyc_Tcutil_copy_types(_tmp68);_tmpA5.rgn=(void*)Cyc_Tcutil_copy_type(
_tmp69);_tmpA5;});_tmpA4;});_tmpA3;});_LL22: if(*((int*)_tmp64)!= 3)goto _LL24;
_tmp6A=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp64)->f1;_tmp6B=(void*)
_tmp6A.field_info;_tmp6C=_tmp6A.targs;_LL23: return(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmpA7;_tmpA7.tag=3;_tmpA7.f1=({struct Cyc_Absyn_TunionFieldInfo _tmpA8;_tmpA8.field_info=(
void*)_tmp6B;_tmpA8.targs=Cyc_Tcutil_copy_types(_tmp6C);_tmpA8;});_tmpA7;});
_tmpA6;});_LL24: if(*((int*)_tmp64)!= 4)goto _LL26;_tmp6D=((struct Cyc_Absyn_PointerType_struct*)
_tmp64)->f1;_tmp6E=(void*)_tmp6D.elt_typ;_tmp6F=_tmp6D.elt_tq;_tmp70=_tmp6D.ptr_atts;
_tmp71=(void*)_tmp70.rgn;_tmp72=_tmp70.nullable;_tmp73=_tmp70.bounds;_tmp74=
_tmp70.zero_term;_tmp75=_tmp70.ptrloc;_LL25: {void*_tmpA9=Cyc_Tcutil_copy_type(
_tmp6E);void*_tmpAA=Cyc_Tcutil_copy_type(_tmp71);struct Cyc_Absyn_Conref*_tmpAB=((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(
_tmp72);struct Cyc_Absyn_Tqual _tmpAC=_tmp6F;struct Cyc_Absyn_Conref*_tmpAD=Cyc_Tcutil_copy_conref(
_tmp73);struct Cyc_Absyn_Conref*_tmpAE=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp74);return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({struct Cyc_Absyn_PointerType_struct
_tmpB0;_tmpB0.tag=4;_tmpB0.f1=({struct Cyc_Absyn_PtrInfo _tmpB1;_tmpB1.elt_typ=(
void*)_tmpA9;_tmpB1.elt_tq=_tmpAC;_tmpB1.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmpB2;_tmpB2.rgn=(void*)_tmpAA;_tmpB2.nullable=_tmpAB;_tmpB2.bounds=_tmpAD;
_tmpB2.zero_term=_tmpAE;_tmpB2.ptrloc=_tmp75;_tmpB2;});_tmpB1;});_tmpB0;});
_tmpAF;});}_LL26: if(*((int*)_tmp64)!= 5)goto _LL28;_tmp76=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp64)->f1;_tmp77=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp64)->f2;_LL27:
return(void*)({struct Cyc_Absyn_IntType_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));
_tmpB3[0]=({struct Cyc_Absyn_IntType_struct _tmpB4;_tmpB4.tag=5;_tmpB4.f1=(void*)
_tmp76;_tmpB4.f2=(void*)_tmp77;_tmpB4;});_tmpB3;});_LL28: if((int)_tmp64 != 1)goto
_LL2A;_LL29: return t;_LL2A: if(_tmp64 <= (void*)4)goto _LL46;if(*((int*)_tmp64)!= 6)
goto _LL2C;_tmp78=((struct Cyc_Absyn_DoubleType_struct*)_tmp64)->f1;_LL2B: return(
void*)({struct Cyc_Absyn_DoubleType_struct*_tmpB5=_cycalloc_atomic(sizeof(*_tmpB5));
_tmpB5[0]=({struct Cyc_Absyn_DoubleType_struct _tmpB6;_tmpB6.tag=6;_tmpB6.f1=
_tmp78;_tmpB6;});_tmpB5;});_LL2C: if(*((int*)_tmp64)!= 7)goto _LL2E;_tmp79=((
struct Cyc_Absyn_ArrayType_struct*)_tmp64)->f1;_tmp7A=(void*)_tmp79.elt_type;
_tmp7B=_tmp79.tq;_tmp7C=_tmp79.num_elts;_tmp7D=_tmp79.zero_term;_tmp7E=_tmp79.zt_loc;
_LL2D: return(void*)({struct Cyc_Absyn_ArrayType_struct*_tmpB7=_cycalloc(sizeof(*
_tmpB7));_tmpB7[0]=({struct Cyc_Absyn_ArrayType_struct _tmpB8;_tmpB8.tag=7;_tmpB8.f1=({
struct Cyc_Absyn_ArrayInfo _tmpB9;_tmpB9.elt_type=(void*)Cyc_Tcutil_copy_type(
_tmp7A);_tmpB9.tq=_tmp7B;_tmpB9.num_elts=_tmp7C;_tmpB9.zero_term=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp7D);_tmpB9.zt_loc=_tmp7E;
_tmpB9;});_tmpB8;});_tmpB7;});_LL2E: if(*((int*)_tmp64)!= 8)goto _LL30;_tmp7F=((
struct Cyc_Absyn_FnType_struct*)_tmp64)->f1;_tmp80=_tmp7F.tvars;_tmp81=_tmp7F.effect;
_tmp82=(void*)_tmp7F.ret_typ;_tmp83=_tmp7F.args;_tmp84=_tmp7F.c_varargs;_tmp85=
_tmp7F.cyc_varargs;_tmp86=_tmp7F.rgn_po;_tmp87=_tmp7F.attributes;_LL2F: {struct
Cyc_List_List*_tmpBA=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,
_tmp80);struct Cyc_Core_Opt*_tmpBB=_tmp81 == 0?0:({struct Cyc_Core_Opt*_tmpC5=
_cycalloc(sizeof(*_tmpC5));_tmpC5->v=(void*)Cyc_Tcutil_copy_type((void*)_tmp81->v);
_tmpC5;});void*_tmpBC=Cyc_Tcutil_copy_type(_tmp82);struct Cyc_List_List*_tmpBD=((
struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp83);int _tmpBE=_tmp84;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp85 != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp85;cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpBF=_cycalloc(sizeof(*_tmpBF));
_tmpBF->name=cv->name;_tmpBF->tq=cv->tq;_tmpBF->type=(void*)Cyc_Tcutil_copy_type((
void*)cv->type);_tmpBF->inject=cv->inject;_tmpBF;});}{struct Cyc_List_List*_tmpC0=((
struct Cyc_List_List*(*)(struct _tuple6*(*f)(struct _tuple6*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp86);struct Cyc_List_List*_tmpC1=_tmp87;
return(void*)({struct Cyc_Absyn_FnType_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));
_tmpC2[0]=({struct Cyc_Absyn_FnType_struct _tmpC3;_tmpC3.tag=8;_tmpC3.f1=({struct
Cyc_Absyn_FnInfo _tmpC4;_tmpC4.tvars=_tmpBA;_tmpC4.effect=_tmpBB;_tmpC4.ret_typ=(
void*)_tmpBC;_tmpC4.args=_tmpBD;_tmpC4.c_varargs=_tmpBE;_tmpC4.cyc_varargs=
cyc_varargs2;_tmpC4.rgn_po=_tmpC0;_tmpC4.attributes=_tmpC1;_tmpC4;});_tmpC3;});
_tmpC2;});}}_LL30: if(*((int*)_tmp64)!= 9)goto _LL32;_tmp88=((struct Cyc_Absyn_TupleType_struct*)
_tmp64)->f1;_LL31: return(void*)({struct Cyc_Absyn_TupleType_struct*_tmpC6=
_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_TupleType_struct _tmpC7;
_tmpC7.tag=9;_tmpC7.f1=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp88);
_tmpC7;});_tmpC6;});_LL32: if(*((int*)_tmp64)!= 10)goto _LL34;_tmp89=((struct Cyc_Absyn_AggrType_struct*)
_tmp64)->f1;_tmp8A=(void*)_tmp89.aggr_info;if(*((int*)_tmp8A)!= 0)goto _LL34;
_tmp8B=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp8A)->f1;_tmp8C=((struct
Cyc_Absyn_UnknownAggr_struct*)_tmp8A)->f2;_tmp8D=_tmp89.targs;_LL33: return(void*)({
struct Cyc_Absyn_AggrType_struct*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=({
struct Cyc_Absyn_AggrType_struct _tmpC9;_tmpC9.tag=10;_tmpC9.f1=({struct Cyc_Absyn_AggrInfo
_tmpCA;_tmpCA.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmpCC;_tmpCC.tag=0;_tmpCC.f1=(void*)_tmp8B;_tmpCC.f2=_tmp8C;_tmpCC;});_tmpCB;}));
_tmpCA.targs=Cyc_Tcutil_copy_types(_tmp8D);_tmpCA;});_tmpC9;});_tmpC8;});_LL34:
if(*((int*)_tmp64)!= 10)goto _LL36;_tmp8E=((struct Cyc_Absyn_AggrType_struct*)
_tmp64)->f1;_tmp8F=(void*)_tmp8E.aggr_info;if(*((int*)_tmp8F)!= 1)goto _LL36;
_tmp90=((struct Cyc_Absyn_KnownAggr_struct*)_tmp8F)->f1;_tmp91=_tmp8E.targs;_LL35:
return(void*)({struct Cyc_Absyn_AggrType_struct*_tmpCD=_cycalloc(sizeof(*_tmpCD));
_tmpCD[0]=({struct Cyc_Absyn_AggrType_struct _tmpCE;_tmpCE.tag=10;_tmpCE.f1=({
struct Cyc_Absyn_AggrInfo _tmpCF;_tmpCF.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*
_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0[0]=({struct Cyc_Absyn_KnownAggr_struct
_tmpD1;_tmpD1.tag=1;_tmpD1.f1=_tmp90;_tmpD1;});_tmpD0;}));_tmpCF.targs=Cyc_Tcutil_copy_types(
_tmp91);_tmpCF;});_tmpCE;});_tmpCD;});_LL36: if(*((int*)_tmp64)!= 11)goto _LL38;
_tmp92=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp64)->f1;_tmp93=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp64)->f2;_LL37: return(void*)({struct Cyc_Absyn_AnonAggrType_struct*
_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmpD3;_tmpD3.tag=11;_tmpD3.f1=(void*)_tmp92;_tmpD3.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp93);_tmpD3;});_tmpD2;});_LL38: if(*((int*)
_tmp64)!= 12)goto _LL3A;_tmp94=((struct Cyc_Absyn_EnumType_struct*)_tmp64)->f1;
_tmp95=((struct Cyc_Absyn_EnumType_struct*)_tmp64)->f2;_LL39: return(void*)({
struct Cyc_Absyn_EnumType_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({
struct Cyc_Absyn_EnumType_struct _tmpD5;_tmpD5.tag=12;_tmpD5.f1=_tmp94;_tmpD5.f2=
_tmp95;_tmpD5;});_tmpD4;});_LL3A: if(*((int*)_tmp64)!= 13)goto _LL3C;_tmp96=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp64)->f1;_LL3B: return(void*)({struct Cyc_Absyn_AnonEnumType_struct*
_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmpD7;_tmpD7.tag=13;_tmpD7.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*
f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,
_tmp96);_tmpD7;});_tmpD6;});_LL3C: if(*((int*)_tmp64)!= 14)goto _LL3E;_tmp97=(void*)((
struct Cyc_Absyn_SizeofType_struct*)_tmp64)->f1;_LL3D: return(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8[0]=({struct Cyc_Absyn_SizeofType_struct
_tmpD9;_tmpD9.tag=14;_tmpD9.f1=(void*)Cyc_Tcutil_copy_type(_tmp97);_tmpD9;});
_tmpD8;});_LL3E: if(*((int*)_tmp64)!= 18)goto _LL40;_tmp98=((struct Cyc_Absyn_TypeInt_struct*)
_tmp64)->f1;_LL3F: return(void*)({struct Cyc_Absyn_TypeInt_struct*_tmpDA=
_cycalloc_atomic(sizeof(*_tmpDA));_tmpDA[0]=({struct Cyc_Absyn_TypeInt_struct
_tmpDB;_tmpDB.tag=18;_tmpDB.f1=_tmp98;_tmpDB;});_tmpDA;});_LL40: if(*((int*)
_tmp64)!= 17)goto _LL42;_tmp99=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp64)->f1;
_LL41: return(void*)({struct Cyc_Absyn_TagType_struct*_tmpDC=_cycalloc(sizeof(*
_tmpDC));_tmpDC[0]=({struct Cyc_Absyn_TagType_struct _tmpDD;_tmpDD.tag=17;_tmpDD.f1=(
void*)Cyc_Tcutil_copy_type(_tmp99);_tmpDD;});_tmpDC;});_LL42: if(*((int*)_tmp64)
!= 15)goto _LL44;_tmp9A=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp64)->f1;
_LL43: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpDE=_cycalloc(
sizeof(*_tmpDE));_tmpDE[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmpDF;_tmpDF.tag=
15;_tmpDF.f1=(void*)Cyc_Tcutil_copy_type(_tmp9A);_tmpDF;});_tmpDE;});_LL44: if(*((
int*)_tmp64)!= 16)goto _LL46;_tmp9B=((struct Cyc_Absyn_TypedefType_struct*)_tmp64)->f1;
_tmp9C=((struct Cyc_Absyn_TypedefType_struct*)_tmp64)->f2;_tmp9D=((struct Cyc_Absyn_TypedefType_struct*)
_tmp64)->f3;_LL45: return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmpE0=
_cycalloc(sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_TypedefType_struct _tmpE1;
_tmpE1.tag=16;_tmpE1.f1=_tmp9B;_tmpE1.f2=Cyc_Tcutil_copy_types(_tmp9C);_tmpE1.f3=
_tmp9D;_tmpE1.f4=0;_tmpE1;});_tmpE0;});_LL46: if((int)_tmp64 != 3)goto _LL48;_LL47:
goto _LL49;_LL48: if((int)_tmp64 != 2)goto _LL4A;_LL49: return t;_LL4A: if(_tmp64 <= (
void*)4)goto _LL4C;if(*((int*)_tmp64)!= 19)goto _LL4C;_tmp9E=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp64)->f1;_LL4B: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmpE2=
_cycalloc(sizeof(*_tmpE2));_tmpE2[0]=({struct Cyc_Absyn_AccessEff_struct _tmpE3;
_tmpE3.tag=19;_tmpE3.f1=(void*)Cyc_Tcutil_copy_type(_tmp9E);_tmpE3;});_tmpE2;});
_LL4C: if(_tmp64 <= (void*)4)goto _LL4E;if(*((int*)_tmp64)!= 20)goto _LL4E;_tmp9F=((
struct Cyc_Absyn_JoinEff_struct*)_tmp64)->f1;_LL4D: return(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4[0]=({struct Cyc_Absyn_JoinEff_struct
_tmpE5;_tmpE5.tag=20;_tmpE5.f1=Cyc_Tcutil_copy_types(_tmp9F);_tmpE5;});_tmpE4;});
_LL4E: if(_tmp64 <= (void*)4)goto _LL19;if(*((int*)_tmp64)!= 21)goto _LL19;_tmpA0=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp64)->f1;_LL4F: return(void*)({struct
Cyc_Absyn_RgnsEff_struct*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmpE7;_tmpE7.tag=21;_tmpE7.f1=(void*)Cyc_Tcutil_copy_type(_tmpA0);_tmpE7;});
_tmpE6;});_LL19:;}int Cyc_Tcutil_kind_leq(void*k1,void*k2){if(k1 == k2)return 1;{
struct _tuple6 _tmpE9=({struct _tuple6 _tmpE8;_tmpE8.f1=k1;_tmpE8.f2=k2;_tmpE8;});
void*_tmpEA;void*_tmpEB;void*_tmpEC;void*_tmpED;void*_tmpEE;void*_tmpEF;_LL51:
_tmpEA=_tmpE9.f1;if((int)_tmpEA != 2)goto _LL53;_tmpEB=_tmpE9.f2;if((int)_tmpEB != 
1)goto _LL53;_LL52: goto _LL54;_LL53: _tmpEC=_tmpE9.f1;if((int)_tmpEC != 2)goto _LL55;
_tmpED=_tmpE9.f2;if((int)_tmpED != 0)goto _LL55;_LL54: goto _LL56;_LL55: _tmpEE=
_tmpE9.f1;if((int)_tmpEE != 1)goto _LL57;_tmpEF=_tmpE9.f2;if((int)_tmpEF != 0)goto
_LL57;_LL56: return 1;_LL57:;_LL58: return 0;_LL50:;}}void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*tv){void*_tmpF0=Cyc_Absyn_compress_kb((void*)tv->kind);void*
_tmpF1;void*_tmpF2;_LL5A: if(*((int*)_tmpF0)!= 0)goto _LL5C;_tmpF1=(void*)((struct
Cyc_Absyn_Eq_kb_struct*)_tmpF0)->f1;_LL5B: return _tmpF1;_LL5C: if(*((int*)_tmpF0)
!= 2)goto _LL5E;_tmpF2=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpF0)->f2;_LL5D:
return _tmpF2;_LL5E:;_LL5F:({void*_tmpF3[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpF4="kind not suitably constrained!";
_tag_dynforward(_tmpF4,sizeof(char),_get_zero_arr_size(_tmpF4,31));}),
_tag_dynforward(_tmpF3,sizeof(void*),0));});_LL59:;}void*Cyc_Tcutil_typ_kind(
void*t){void*_tmpF5=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpF6;struct Cyc_Core_Opt*
_tmpF7;struct Cyc_Absyn_Tvar*_tmpF8;void*_tmpF9;struct Cyc_Absyn_TunionFieldInfo
_tmpFA;void*_tmpFB;struct Cyc_Absyn_Tunionfield*_tmpFC;struct Cyc_Absyn_TunionFieldInfo
_tmpFD;void*_tmpFE;struct Cyc_Absyn_Enumdecl*_tmpFF;struct Cyc_Absyn_AggrInfo
_tmp100;void*_tmp101;struct Cyc_Absyn_AggrInfo _tmp102;void*_tmp103;struct Cyc_Absyn_Aggrdecl**
_tmp104;struct Cyc_Absyn_Aggrdecl*_tmp105;struct Cyc_Absyn_Aggrdecl _tmp106;struct
Cyc_Absyn_AggrdeclImpl*_tmp107;struct Cyc_Absyn_Enumdecl*_tmp108;struct Cyc_Absyn_PtrInfo
_tmp109;struct Cyc_Absyn_Typedefdecl*_tmp10A;_LL61: if(_tmpF5 <= (void*)4)goto _LL65;
if(*((int*)_tmpF5)!= 0)goto _LL63;_tmpF6=((struct Cyc_Absyn_Evar_struct*)_tmpF5)->f1;
_tmpF7=((struct Cyc_Absyn_Evar_struct*)_tmpF5)->f2;_LL62: return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF6))->v;_LL63: if(*((int*)_tmpF5)!= 1)goto _LL65;_tmpF8=((struct
Cyc_Absyn_VarType_struct*)_tmpF5)->f1;_LL64: return Cyc_Tcutil_tvar_kind(_tmpF8);
_LL65: if((int)_tmpF5 != 0)goto _LL67;_LL66: return(void*)1;_LL67: if(_tmpF5 <= (void*)
4)goto _LL69;if(*((int*)_tmpF5)!= 5)goto _LL69;_tmpF9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpF5)->f2;_LL68: return(_tmpF9 == (void*)((void*)2) || _tmpF9 == (void*)((void*)3))?(
void*)2:(void*)1;_LL69: if((int)_tmpF5 != 1)goto _LL6B;_LL6A: goto _LL6C;_LL6B: if(
_tmpF5 <= (void*)4)goto _LL71;if(*((int*)_tmpF5)!= 6)goto _LL6D;_LL6C: goto _LL6E;
_LL6D: if(*((int*)_tmpF5)!= 8)goto _LL6F;_LL6E: return(void*)1;_LL6F: if(*((int*)
_tmpF5)!= 15)goto _LL71;_LL70: return(void*)2;_LL71: if((int)_tmpF5 != 3)goto _LL73;
_LL72: goto _LL74;_LL73: if((int)_tmpF5 != 2)goto _LL75;_LL74: return(void*)3;_LL75:
if(_tmpF5 <= (void*)4)goto _LL77;if(*((int*)_tmpF5)!= 2)goto _LL77;_LL76: return(
void*)2;_LL77: if(_tmpF5 <= (void*)4)goto _LL79;if(*((int*)_tmpF5)!= 3)goto _LL79;
_tmpFA=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpF5)->f1;_tmpFB=(void*)
_tmpFA.field_info;if(*((int*)_tmpFB)!= 1)goto _LL79;_tmpFC=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpFB)->f2;_LL78: if(_tmpFC->typs == 0)return(void*)2;else{return(void*)1;}_LL79:
if(_tmpF5 <= (void*)4)goto _LL7B;if(*((int*)_tmpF5)!= 3)goto _LL7B;_tmpFD=((struct
Cyc_Absyn_TunionFieldType_struct*)_tmpF5)->f1;_tmpFE=(void*)_tmpFD.field_info;
if(*((int*)_tmpFE)!= 0)goto _LL7B;_LL7A:({void*_tmp10B[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp10C="typ_kind: Unresolved TunionFieldType";_tag_dynforward(_tmp10C,sizeof(
char),_get_zero_arr_size(_tmp10C,37));}),_tag_dynforward(_tmp10B,sizeof(void*),0));});
_LL7B: if(_tmpF5 <= (void*)4)goto _LL7D;if(*((int*)_tmpF5)!= 12)goto _LL7D;_tmpFF=((
struct Cyc_Absyn_EnumType_struct*)_tmpF5)->f2;if(_tmpFF != 0)goto _LL7D;_LL7C: goto
_LL7E;_LL7D: if(_tmpF5 <= (void*)4)goto _LL7F;if(*((int*)_tmpF5)!= 10)goto _LL7F;
_tmp100=((struct Cyc_Absyn_AggrType_struct*)_tmpF5)->f1;_tmp101=(void*)_tmp100.aggr_info;
if(*((int*)_tmp101)!= 0)goto _LL7F;_LL7E: return(void*)0;_LL7F: if(_tmpF5 <= (void*)
4)goto _LL81;if(*((int*)_tmpF5)!= 10)goto _LL81;_tmp102=((struct Cyc_Absyn_AggrType_struct*)
_tmpF5)->f1;_tmp103=(void*)_tmp102.aggr_info;if(*((int*)_tmp103)!= 1)goto _LL81;
_tmp104=((struct Cyc_Absyn_KnownAggr_struct*)_tmp103)->f1;_tmp105=*_tmp104;
_tmp106=*_tmp105;_tmp107=_tmp106.impl;_LL80: return _tmp107 == 0?(void*)0:(void*)1;
_LL81: if(_tmpF5 <= (void*)4)goto _LL83;if(*((int*)_tmpF5)!= 11)goto _LL83;_LL82:
goto _LL84;_LL83: if(_tmpF5 <= (void*)4)goto _LL85;if(*((int*)_tmpF5)!= 13)goto _LL85;
_LL84: return(void*)1;_LL85: if(_tmpF5 <= (void*)4)goto _LL87;if(*((int*)_tmpF5)!= 
12)goto _LL87;_tmp108=((struct Cyc_Absyn_EnumType_struct*)_tmpF5)->f2;_LL86: if(
_tmp108->fields == 0)return(void*)0;else{return(void*)1;}_LL87: if(_tmpF5 <= (void*)
4)goto _LL89;if(*((int*)_tmpF5)!= 4)goto _LL89;_tmp109=((struct Cyc_Absyn_PointerType_struct*)
_tmpF5)->f1;_LL88: {void*_tmp10D=(void*)(Cyc_Absyn_compress_conref((_tmp109.ptr_atts).bounds))->v;
void*_tmp10E;void*_tmp10F;void*_tmp110;void*_tmp111;_LL9C: if((int)_tmp10D != 0)
goto _LL9E;_LL9D: goto _LL9F;_LL9E: if(_tmp10D <= (void*)1)goto _LLA0;if(*((int*)
_tmp10D)!= 0)goto _LLA0;_tmp10E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp10D)->f1;if((int)_tmp10E != 0)goto _LLA0;_LL9F: return(void*)1;_LLA0: if(_tmp10D
<= (void*)1)goto _LLA2;if(*((int*)_tmp10D)!= 0)goto _LLA2;_tmp10F=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmp10D)->f1;if((int)_tmp10F != 1)goto _LLA2;_LLA1:
return(void*)1;_LLA2: if(_tmp10D <= (void*)1)goto _LLA4;if(*((int*)_tmp10D)!= 0)
goto _LLA4;_tmp110=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp10D)->f1;if(
_tmp110 <= (void*)2)goto _LLA4;if(*((int*)_tmp110)!= 0)goto _LLA4;_LLA3: goto _LLA5;
_LLA4: if(_tmp10D <= (void*)1)goto _LLA6;if(*((int*)_tmp10D)!= 0)goto _LLA6;_tmp111=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp10D)->f1;if(_tmp111 <= (void*)2)
goto _LLA6;if(*((int*)_tmp111)!= 1)goto _LLA6;_LLA5: return(void*)2;_LLA6: if(
_tmp10D <= (void*)1)goto _LL9B;if(*((int*)_tmp10D)!= 1)goto _LL9B;_LLA7:({void*
_tmp112[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp113="typ_kind: forward constr in ptr bounds";_tag_dynforward(
_tmp113,sizeof(char),_get_zero_arr_size(_tmp113,39));}),_tag_dynforward(_tmp112,
sizeof(void*),0));});_LL9B:;}_LL89: if(_tmpF5 <= (void*)4)goto _LL8B;if(*((int*)
_tmpF5)!= 14)goto _LL8B;_LL8A: return(void*)2;_LL8B: if(_tmpF5 <= (void*)4)goto _LL8D;
if(*((int*)_tmpF5)!= 18)goto _LL8D;_LL8C: return(void*)5;_LL8D: if(_tmpF5 <= (void*)
4)goto _LL8F;if(*((int*)_tmpF5)!= 17)goto _LL8F;_LL8E: return(void*)2;_LL8F: if(
_tmpF5 <= (void*)4)goto _LL91;if(*((int*)_tmpF5)!= 7)goto _LL91;_LL90: goto _LL92;
_LL91: if(_tmpF5 <= (void*)4)goto _LL93;if(*((int*)_tmpF5)!= 9)goto _LL93;_LL92:
return(void*)1;_LL93: if(_tmpF5 <= (void*)4)goto _LL95;if(*((int*)_tmpF5)!= 16)goto
_LL95;_tmp10A=((struct Cyc_Absyn_TypedefType_struct*)_tmpF5)->f3;_LL94: if(_tmp10A
== 0  || _tmp10A->kind == 0)({struct Cyc_String_pa_struct _tmp116;_tmp116.tag=0;
_tmp116.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp114[1]={& _tmp116};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp115="typ_kind: typedef found: %s";
_tag_dynforward(_tmp115,sizeof(char),_get_zero_arr_size(_tmp115,28));}),
_tag_dynforward(_tmp114,sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp10A->kind))->v;_LL95: if(_tmpF5 <= (void*)4)goto _LL97;if(*((int*)
_tmpF5)!= 19)goto _LL97;_LL96: goto _LL98;_LL97: if(_tmpF5 <= (void*)4)goto _LL99;if(*((
int*)_tmpF5)!= 20)goto _LL99;_LL98: goto _LL9A;_LL99: if(_tmpF5 <= (void*)4)goto _LL60;
if(*((int*)_tmpF5)!= 21)goto _LL60;_LL9A: return(void*)4;_LL60:;}int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp117;_push_handler(& _tmp117);{int _tmp119=
0;if(setjmp(_tmp117.handler))_tmp119=1;if(!_tmp119){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp11A=1;_npop_handler(0);return _tmp11A;};_pop_handler();}else{void*_tmp118=(
void*)_exn_thrown;void*_tmp11C=_tmp118;_LLA9: if(_tmp11C != Cyc_Tcutil_Unify)goto
_LLAB;_LLAA: return 0;_LLAB:;_LLAC:(void)_throw(_tmp11C);_LLA8:;}}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,void*t){t=Cyc_Tcutil_compress(t);{void*_tmp11D=t;struct Cyc_Absyn_Tvar*
_tmp11E;struct Cyc_Core_Opt*_tmp11F;struct Cyc_Core_Opt*_tmp120;struct Cyc_Core_Opt**
_tmp121;struct Cyc_Absyn_PtrInfo _tmp122;struct Cyc_Absyn_ArrayInfo _tmp123;void*
_tmp124;struct Cyc_Absyn_FnInfo _tmp125;struct Cyc_List_List*_tmp126;struct Cyc_Core_Opt*
_tmp127;void*_tmp128;struct Cyc_List_List*_tmp129;int _tmp12A;struct Cyc_Absyn_VarargInfo*
_tmp12B;struct Cyc_List_List*_tmp12C;struct Cyc_List_List*_tmp12D;struct Cyc_List_List*
_tmp12E;struct Cyc_Absyn_TunionInfo _tmp12F;struct Cyc_List_List*_tmp130;void*
_tmp131;struct Cyc_List_List*_tmp132;struct Cyc_Absyn_TunionFieldInfo _tmp133;
struct Cyc_List_List*_tmp134;struct Cyc_Absyn_AggrInfo _tmp135;struct Cyc_List_List*
_tmp136;struct Cyc_List_List*_tmp137;void*_tmp138;void*_tmp139;void*_tmp13A;void*
_tmp13B;void*_tmp13C;struct Cyc_List_List*_tmp13D;_LLAE: if(_tmp11D <= (void*)4)
goto _LLD0;if(*((int*)_tmp11D)!= 1)goto _LLB0;_tmp11E=((struct Cyc_Absyn_VarType_struct*)
_tmp11D)->f1;_LLAF: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,_tmp11E)){Cyc_Tcutil_failure_reason=({const char*_tmp13E="(type variable would escape scope)";
_tag_dynforward(_tmp13E,sizeof(char),_get_zero_arr_size(_tmp13E,35));});(int)
_throw((void*)Cyc_Tcutil_Unify);}goto _LLAD;_LLB0: if(*((int*)_tmp11D)!= 0)goto
_LLB2;_tmp11F=((struct Cyc_Absyn_Evar_struct*)_tmp11D)->f2;_tmp120=((struct Cyc_Absyn_Evar_struct*)
_tmp11D)->f4;_tmp121=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp11D)->f4;_LLB1: if(t == evar){Cyc_Tcutil_failure_reason=({const char*_tmp13F="(occurs check)";
_tag_dynforward(_tmp13F,sizeof(char),_get_zero_arr_size(_tmp13F,15));});(int)
_throw((void*)Cyc_Tcutil_Unify);}else{if(_tmp11F != 0)Cyc_Tcutil_occurs(evar,r,
env,(void*)_tmp11F->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp121))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp140=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp121))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd))_tmp140=({struct Cyc_List_List*_tmp141=
_cycalloc(sizeof(*_tmp141));_tmp141->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp141->tl=
_tmp140;_tmp141;});}}*_tmp121=({struct Cyc_Core_Opt*_tmp142=_cycalloc(sizeof(*
_tmp142));_tmp142->v=_tmp140;_tmp142;});}}}goto _LLAD;_LLB2: if(*((int*)_tmp11D)!= 
4)goto _LLB4;_tmp122=((struct Cyc_Absyn_PointerType_struct*)_tmp11D)->f1;_LLB3: Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp122.elt_typ);Cyc_Tcutil_occurs(evar,r,env,(void*)(_tmp122.ptr_atts).rgn);{
void*_tmp143=(void*)(Cyc_Absyn_compress_conref((_tmp122.ptr_atts).bounds))->v;
void*_tmp144;void*_tmp145;_LLD3: if(_tmp143 <= (void*)1)goto _LLD5;if(*((int*)
_tmp143)!= 0)goto _LLD5;_tmp144=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp143)->f1;if(_tmp144 <= (void*)2)goto _LLD5;if(*((int*)_tmp144)!= 1)goto _LLD5;
_tmp145=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp144)->f1;_LLD4: Cyc_Tcutil_occurs(
evar,r,env,_tmp145);goto _LLD2;_LLD5:;_LLD6: goto _LLD2;_LLD2:;}goto _LLAD;_LLB4: if(*((
int*)_tmp11D)!= 7)goto _LLB6;_tmp123=((struct Cyc_Absyn_ArrayType_struct*)_tmp11D)->f1;
_tmp124=(void*)_tmp123.elt_type;_LLB5: Cyc_Tcutil_occurs(evar,r,env,_tmp124);goto
_LLAD;_LLB6: if(*((int*)_tmp11D)!= 8)goto _LLB8;_tmp125=((struct Cyc_Absyn_FnType_struct*)
_tmp11D)->f1;_tmp126=_tmp125.tvars;_tmp127=_tmp125.effect;_tmp128=(void*)_tmp125.ret_typ;
_tmp129=_tmp125.args;_tmp12A=_tmp125.c_varargs;_tmp12B=_tmp125.cyc_varargs;
_tmp12C=_tmp125.rgn_po;_tmp12D=_tmp125.attributes;_LLB7: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp126,env);if(_tmp127 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp127->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp128);for(0;_tmp129 != 0;_tmp129=_tmp129->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple2*)_tmp129->hd)).f3);}if(_tmp12B != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp12B->type);for(0;_tmp12C != 0;_tmp12C=_tmp12C->tl){struct
_tuple6 _tmp147;void*_tmp148;void*_tmp149;struct _tuple6*_tmp146=(struct _tuple6*)
_tmp12C->hd;_tmp147=*_tmp146;_tmp148=_tmp147.f1;_tmp149=_tmp147.f2;Cyc_Tcutil_occurs(
evar,r,env,_tmp148);Cyc_Tcutil_occurs(evar,r,env,_tmp149);}goto _LLAD;_LLB8: if(*((
int*)_tmp11D)!= 9)goto _LLBA;_tmp12E=((struct Cyc_Absyn_TupleType_struct*)_tmp11D)->f1;
_LLB9: for(0;_tmp12E != 0;_tmp12E=_tmp12E->tl){Cyc_Tcutil_occurs(evar,r,env,(*((
struct _tuple4*)_tmp12E->hd)).f2);}goto _LLAD;_LLBA: if(*((int*)_tmp11D)!= 2)goto
_LLBC;_tmp12F=((struct Cyc_Absyn_TunionType_struct*)_tmp11D)->f1;_tmp130=_tmp12F.targs;
_tmp131=(void*)_tmp12F.rgn;_LLBB: Cyc_Tcutil_occurs(evar,r,env,_tmp131);Cyc_Tcutil_occurslist(
evar,r,env,_tmp130);goto _LLAD;_LLBC: if(*((int*)_tmp11D)!= 16)goto _LLBE;_tmp132=((
struct Cyc_Absyn_TypedefType_struct*)_tmp11D)->f2;_LLBD: _tmp134=_tmp132;goto _LLBF;
_LLBE: if(*((int*)_tmp11D)!= 3)goto _LLC0;_tmp133=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp11D)->f1;_tmp134=_tmp133.targs;_LLBF: _tmp136=_tmp134;goto _LLC1;_LLC0: if(*((
int*)_tmp11D)!= 10)goto _LLC2;_tmp135=((struct Cyc_Absyn_AggrType_struct*)_tmp11D)->f1;
_tmp136=_tmp135.targs;_LLC1: Cyc_Tcutil_occurslist(evar,r,env,_tmp136);goto _LLAD;
_LLC2: if(*((int*)_tmp11D)!= 11)goto _LLC4;_tmp137=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp11D)->f2;_LLC3: for(0;_tmp137 != 0;_tmp137=_tmp137->tl){Cyc_Tcutil_occurs(evar,
r,env,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp137->hd)->type);}goto _LLAD;_LLC4:
if(*((int*)_tmp11D)!= 17)goto _LLC6;_tmp138=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp11D)->f1;_LLC5: _tmp139=_tmp138;goto _LLC7;_LLC6: if(*((int*)_tmp11D)!= 14)goto
_LLC8;_tmp139=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp11D)->f1;_LLC7:
_tmp13A=_tmp139;goto _LLC9;_LLC8: if(*((int*)_tmp11D)!= 19)goto _LLCA;_tmp13A=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp11D)->f1;_LLC9: _tmp13B=_tmp13A;goto _LLCB;
_LLCA: if(*((int*)_tmp11D)!= 15)goto _LLCC;_tmp13B=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp11D)->f1;_LLCB: _tmp13C=_tmp13B;goto _LLCD;_LLCC: if(*((int*)_tmp11D)!= 21)goto
_LLCE;_tmp13C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp11D)->f1;_LLCD: Cyc_Tcutil_occurs(
evar,r,env,_tmp13C);goto _LLAD;_LLCE: if(*((int*)_tmp11D)!= 20)goto _LLD0;_tmp13D=((
struct Cyc_Absyn_JoinEff_struct*)_tmp11D)->f1;_LLCF: Cyc_Tcutil_occurslist(evar,r,
env,_tmp13D);goto _LLAD;_LLD0:;_LLD1: goto _LLAD;_LLAD:;}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}static void
Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(0;t1 != 
0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)(int)_throw((void*)Cyc_Tcutil_Unify);}static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const
 && !tq1.real_const)({void*_tmp14A[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp14B="tq1 real_const not set.";
_tag_dynforward(_tmp14B,sizeof(char),_get_zero_arr_size(_tmp14B,24));}),
_tag_dynforward(_tmp14A,sizeof(void*),0));});if(tq2.print_const  && !tq2.real_const)({
void*_tmp14C[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp14D="tq2 real_const not set.";_tag_dynforward(
_tmp14D,sizeof(char),_get_zero_arr_size(_tmp14D,24));}),_tag_dynforward(_tmp14C,
sizeof(void*),0));});if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile)
 || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=tq2.real_const;Cyc_Tcutil_failure_reason=({
const char*_tmp14E="(qualifiers don't match)";_tag_dynforward(_tmp14E,sizeof(char),
_get_zero_arr_size(_tmp14E,25));});(int)_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_tq1_const=
0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_dynforward_ptr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(
y);if(x == y)return;{void*_tmp14F=(void*)x->v;void*_tmp150;_LLD8: if((int)_tmp14F
!= 0)goto _LLDA;_LLD9:(void*)(x->v=(void*)((void*)({struct Cyc_Absyn_Forward_constr_struct*
_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151[0]=({struct Cyc_Absyn_Forward_constr_struct
_tmp152;_tmp152.tag=1;_tmp152.f1=y;_tmp152;});_tmp151;})));return;_LLDA: if(
_tmp14F <= (void*)1)goto _LLDC;if(*((int*)_tmp14F)!= 0)goto _LLDC;_tmp150=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp14F)->f1;_LLDB: {void*_tmp153=(void*)y->v;
void*_tmp154;_LLDF: if((int)_tmp153 != 0)goto _LLE1;_LLE0:(void*)(y->v=(void*)((
void*)x->v));return;_LLE1: if(_tmp153 <= (void*)1)goto _LLE3;if(*((int*)_tmp153)!= 
0)goto _LLE3;_tmp154=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp153)->f1;
_LLE2: if(cmp(_tmp150,_tmp154)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((
void*)Cyc_Tcutil_Unify);}return;_LLE3: if(_tmp153 <= (void*)1)goto _LLDE;if(*((int*)
_tmp153)!= 1)goto _LLDE;_LLE4:({void*_tmp155[0]={};Cyc_Tcutil_impos(({const char*
_tmp156="unify_conref: forward after compress(2)";_tag_dynforward(_tmp156,
sizeof(char),_get_zero_arr_size(_tmp156,40));}),_tag_dynforward(_tmp155,sizeof(
void*),0));});_LLDE:;}_LLDC: if(_tmp14F <= (void*)1)goto _LLD7;if(*((int*)_tmp14F)
!= 1)goto _LLD7;_LLDD:({void*_tmp157[0]={};Cyc_Tcutil_impos(({const char*_tmp158="unify_conref: forward after compress";
_tag_dynforward(_tmp158,sizeof(char),_get_zero_arr_size(_tmp158,37));}),
_tag_dynforward(_tmp157,sizeof(void*),0));});_LLD7:;}}static int Cyc_Tcutil_unify_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct
_handler_cons _tmp159;_push_handler(& _tmp159);{int _tmp15B=0;if(setjmp(_tmp159.handler))
_tmp15B=1;if(!_tmp15B){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct
_dynforward_ptr)_tag_dynforward(0,0,0));{int _tmp15C=1;_npop_handler(0);return
_tmp15C;};_pop_handler();}else{void*_tmp15A=(void*)_exn_thrown;void*_tmp15E=
_tmp15A;_LLE6: if(_tmp15E != Cyc_Tcutil_Unify)goto _LLE8;_LLE7: return 0;_LLE8:;_LLE9:(
void)_throw(_tmp15E);_LLE5:;}}}static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple6 _tmp160=({struct _tuple6 _tmp15F;_tmp15F.f1=b1;_tmp15F.f2=b2;_tmp15F;});
void*_tmp161;void*_tmp162;void*_tmp163;void*_tmp164;void*_tmp165;void*_tmp166;
void*_tmp167;void*_tmp168;void*_tmp169;struct Cyc_Absyn_Exp*_tmp16A;void*_tmp16B;
struct Cyc_Absyn_Exp*_tmp16C;void*_tmp16D;void*_tmp16E;void*_tmp16F;void*_tmp170;
void*_tmp171;void*_tmp172;void*_tmp173;void*_tmp174;_LLEB: _tmp161=_tmp160.f1;if((
int)_tmp161 != 0)goto _LLED;_tmp162=_tmp160.f2;if((int)_tmp162 != 0)goto _LLED;_LLEC:
return 0;_LLED: _tmp163=_tmp160.f1;if((int)_tmp163 != 0)goto _LLEF;_LLEE: return - 1;
_LLEF: _tmp164=_tmp160.f2;if((int)_tmp164 != 0)goto _LLF1;_LLF0: return 1;_LLF1:
_tmp165=_tmp160.f1;if((int)_tmp165 != 1)goto _LLF3;_tmp166=_tmp160.f2;if((int)
_tmp166 != 1)goto _LLF3;_LLF2: return 0;_LLF3: _tmp167=_tmp160.f1;if((int)_tmp167 != 1)
goto _LLF5;_LLF4: return - 1;_LLF5: _tmp168=_tmp160.f2;if((int)_tmp168 != 1)goto _LLF7;
_LLF6: return 1;_LLF7: _tmp169=_tmp160.f1;if(_tmp169 <= (void*)2)goto _LLF9;if(*((int*)
_tmp169)!= 0)goto _LLF9;_tmp16A=((struct Cyc_Absyn_Upper_b_struct*)_tmp169)->f1;
_tmp16B=_tmp160.f2;if(_tmp16B <= (void*)2)goto _LLF9;if(*((int*)_tmp16B)!= 0)goto
_LLF9;_tmp16C=((struct Cyc_Absyn_Upper_b_struct*)_tmp16B)->f1;_LLF8: return Cyc_Evexp_const_exp_cmp(
_tmp16A,_tmp16C);_LLF9: _tmp16D=_tmp160.f1;if(_tmp16D <= (void*)2)goto _LLFB;if(*((
int*)_tmp16D)!= 0)goto _LLFB;_tmp16E=_tmp160.f2;if(_tmp16E <= (void*)2)goto _LLFB;
if(*((int*)_tmp16E)!= 1)goto _LLFB;_LLFA: return - 1;_LLFB: _tmp16F=_tmp160.f1;if(
_tmp16F <= (void*)2)goto _LLFD;if(*((int*)_tmp16F)!= 1)goto _LLFD;_tmp170=_tmp160.f2;
if(_tmp170 <= (void*)2)goto _LLFD;if(*((int*)_tmp170)!= 0)goto _LLFD;_LLFC: return 1;
_LLFD: _tmp171=_tmp160.f1;if(_tmp171 <= (void*)2)goto _LLEA;if(*((int*)_tmp171)!= 1)
goto _LLEA;_tmp172=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp171)->f1;
_tmp173=_tmp160.f2;if(_tmp173 <= (void*)2)goto _LLEA;if(*((int*)_tmp173)!= 1)goto
_LLEA;_tmp174=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp173)->f1;_LLFE:
return Cyc_Tcutil_typecmp(_tmp172,_tmp174);_LLEA:;}static int Cyc_Tcutil_unify_it_bounds(
void*b1,void*b2){struct _tuple6 _tmp176=({struct _tuple6 _tmp175;_tmp175.f1=b1;
_tmp175.f2=b2;_tmp175;});void*_tmp177;void*_tmp178;void*_tmp179;void*_tmp17A;
void*_tmp17B;void*_tmp17C;void*_tmp17D;void*_tmp17E;void*_tmp17F;struct Cyc_Absyn_Exp*
_tmp180;void*_tmp181;struct Cyc_Absyn_Exp*_tmp182;void*_tmp183;struct Cyc_Absyn_Exp*
_tmp184;void*_tmp185;void*_tmp186;void*_tmp187;void*_tmp188;void*_tmp189;struct
Cyc_Absyn_Exp*_tmp18A;void*_tmp18B;void*_tmp18C;void*_tmp18D;void*_tmp18E;_LL100:
_tmp177=_tmp176.f1;if((int)_tmp177 != 0)goto _LL102;_tmp178=_tmp176.f2;if((int)
_tmp178 != 0)goto _LL102;_LL101: return 0;_LL102: _tmp179=_tmp176.f1;if((int)_tmp179
!= 0)goto _LL104;_LL103: return - 1;_LL104: _tmp17A=_tmp176.f2;if((int)_tmp17A != 0)
goto _LL106;_LL105: return 1;_LL106: _tmp17B=_tmp176.f1;if((int)_tmp17B != 1)goto
_LL108;_tmp17C=_tmp176.f2;if((int)_tmp17C != 1)goto _LL108;_LL107: return 0;_LL108:
_tmp17D=_tmp176.f1;if((int)_tmp17D != 1)goto _LL10A;_LL109: return - 1;_LL10A: _tmp17E=
_tmp176.f2;if((int)_tmp17E != 1)goto _LL10C;_LL10B: return 1;_LL10C: _tmp17F=_tmp176.f1;
if(_tmp17F <= (void*)2)goto _LL10E;if(*((int*)_tmp17F)!= 0)goto _LL10E;_tmp180=((
struct Cyc_Absyn_Upper_b_struct*)_tmp17F)->f1;_tmp181=_tmp176.f2;if(_tmp181 <= (
void*)2)goto _LL10E;if(*((int*)_tmp181)!= 0)goto _LL10E;_tmp182=((struct Cyc_Absyn_Upper_b_struct*)
_tmp181)->f1;_LL10D: return Cyc_Evexp_const_exp_cmp(_tmp180,_tmp182);_LL10E:
_tmp183=_tmp176.f1;if(_tmp183 <= (void*)2)goto _LL110;if(*((int*)_tmp183)!= 0)goto
_LL110;_tmp184=((struct Cyc_Absyn_Upper_b_struct*)_tmp183)->f1;_tmp185=_tmp176.f2;
if(_tmp185 <= (void*)2)goto _LL110;if(*((int*)_tmp185)!= 1)goto _LL110;_tmp186=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp185)->f1;_LL10F: _tmp188=_tmp186;
_tmp18A=_tmp184;goto _LL111;_LL110: _tmp187=_tmp176.f1;if(_tmp187 <= (void*)2)goto
_LL112;if(*((int*)_tmp187)!= 1)goto _LL112;_tmp188=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp187)->f1;_tmp189=_tmp176.f2;if(_tmp189 <= (void*)2)goto _LL112;if(*((int*)
_tmp189)!= 0)goto _LL112;_tmp18A=((struct Cyc_Absyn_Upper_b_struct*)_tmp189)->f1;
_LL111: {unsigned int _tmp190;int _tmp191;struct _tuple7 _tmp18F=Cyc_Evexp_eval_const_uint_exp(
_tmp18A);_tmp190=_tmp18F.f1;_tmp191=_tmp18F.f2;if(!_tmp191)return 1;_tmp18C=
_tmp188;_tmp18E=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp192=_cycalloc_atomic(
sizeof(*_tmp192));_tmp192[0]=({struct Cyc_Absyn_TypeInt_struct _tmp193;_tmp193.tag=
18;_tmp193.f1=(int)_tmp190;_tmp193;});_tmp192;});goto _LL113;}_LL112: _tmp18B=
_tmp176.f1;if(_tmp18B <= (void*)2)goto _LLFF;if(*((int*)_tmp18B)!= 1)goto _LLFF;
_tmp18C=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp18B)->f1;_tmp18D=_tmp176.f2;
if(_tmp18D <= (void*)2)goto _LLFF;if(*((int*)_tmp18D)!= 1)goto _LLFF;_tmp18E=(void*)((
struct Cyc_Absyn_AbsUpper_b_struct*)_tmp18D)->f1;_LL113: Cyc_Tcutil_unify_it(
_tmp18C,_tmp18E);return 0;_LLFF:;}static int Cyc_Tcutil_attribute_case_number(void*
att){void*_tmp194=att;_LL115: if(_tmp194 <= (void*)17)goto _LL117;if(*((int*)
_tmp194)!= 0)goto _LL117;_LL116: return 0;_LL117: if((int)_tmp194 != 0)goto _LL119;
_LL118: return 1;_LL119: if((int)_tmp194 != 1)goto _LL11B;_LL11A: return 2;_LL11B: if((
int)_tmp194 != 2)goto _LL11D;_LL11C: return 3;_LL11D: if((int)_tmp194 != 3)goto _LL11F;
_LL11E: return 4;_LL11F: if((int)_tmp194 != 4)goto _LL121;_LL120: return 5;_LL121: if(
_tmp194 <= (void*)17)goto _LL123;if(*((int*)_tmp194)!= 1)goto _LL123;_LL122: return 6;
_LL123: if((int)_tmp194 != 5)goto _LL125;_LL124: return 7;_LL125: if(_tmp194 <= (void*)
17)goto _LL127;if(*((int*)_tmp194)!= 2)goto _LL127;_LL126: return 8;_LL127: if((int)
_tmp194 != 6)goto _LL129;_LL128: return 9;_LL129: if((int)_tmp194 != 7)goto _LL12B;
_LL12A: return 10;_LL12B: if((int)_tmp194 != 8)goto _LL12D;_LL12C: return 11;_LL12D: if((
int)_tmp194 != 9)goto _LL12F;_LL12E: return 12;_LL12F: if((int)_tmp194 != 10)goto
_LL131;_LL130: return 13;_LL131: if((int)_tmp194 != 11)goto _LL133;_LL132: return 14;
_LL133: if((int)_tmp194 != 12)goto _LL135;_LL134: return 15;_LL135: if((int)_tmp194 != 
13)goto _LL137;_LL136: return 16;_LL137: if((int)_tmp194 != 14)goto _LL139;_LL138:
return 17;_LL139: if((int)_tmp194 != 15)goto _LL13B;_LL13A: return 18;_LL13B: if(
_tmp194 <= (void*)17)goto _LL13F;if(*((int*)_tmp194)!= 3)goto _LL13D;_LL13C: return
19;_LL13D: if(*((int*)_tmp194)!= 4)goto _LL13F;_LL13E: return 20;_LL13F:;_LL140:
return 21;_LL114:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){struct
_tuple6 _tmp196=({struct _tuple6 _tmp195;_tmp195.f1=att1;_tmp195.f2=att2;_tmp195;});
void*_tmp197;int _tmp198;void*_tmp199;int _tmp19A;void*_tmp19B;int _tmp19C;void*
_tmp19D;int _tmp19E;void*_tmp19F;int _tmp1A0;void*_tmp1A1;int _tmp1A2;void*_tmp1A3;
struct _dynforward_ptr _tmp1A4;void*_tmp1A5;struct _dynforward_ptr _tmp1A6;void*
_tmp1A7;void*_tmp1A8;int _tmp1A9;int _tmp1AA;void*_tmp1AB;void*_tmp1AC;int _tmp1AD;
int _tmp1AE;_LL142: _tmp197=_tmp196.f1;if(_tmp197 <= (void*)17)goto _LL144;if(*((int*)
_tmp197)!= 0)goto _LL144;_tmp198=((struct Cyc_Absyn_Regparm_att_struct*)_tmp197)->f1;
_tmp199=_tmp196.f2;if(_tmp199 <= (void*)17)goto _LL144;if(*((int*)_tmp199)!= 0)
goto _LL144;_tmp19A=((struct Cyc_Absyn_Regparm_att_struct*)_tmp199)->f1;_LL143:
_tmp19C=_tmp198;_tmp19E=_tmp19A;goto _LL145;_LL144: _tmp19B=_tmp196.f1;if(_tmp19B
<= (void*)17)goto _LL146;if(*((int*)_tmp19B)!= 4)goto _LL146;_tmp19C=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp19B)->f1;_tmp19D=_tmp196.f2;if(_tmp19D <= (void*)17)goto _LL146;if(*((int*)
_tmp19D)!= 4)goto _LL146;_tmp19E=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp19D)->f1;_LL145: _tmp1A0=_tmp19C;_tmp1A2=_tmp19E;goto _LL147;_LL146: _tmp19F=
_tmp196.f1;if(_tmp19F <= (void*)17)goto _LL148;if(*((int*)_tmp19F)!= 1)goto _LL148;
_tmp1A0=((struct Cyc_Absyn_Aligned_att_struct*)_tmp19F)->f1;_tmp1A1=_tmp196.f2;
if(_tmp1A1 <= (void*)17)goto _LL148;if(*((int*)_tmp1A1)!= 1)goto _LL148;_tmp1A2=((
struct Cyc_Absyn_Aligned_att_struct*)_tmp1A1)->f1;_LL147: return Cyc_Core_intcmp(
_tmp1A0,_tmp1A2);_LL148: _tmp1A3=_tmp196.f1;if(_tmp1A3 <= (void*)17)goto _LL14A;if(*((
int*)_tmp1A3)!= 2)goto _LL14A;_tmp1A4=((struct Cyc_Absyn_Section_att_struct*)
_tmp1A3)->f1;_tmp1A5=_tmp196.f2;if(_tmp1A5 <= (void*)17)goto _LL14A;if(*((int*)
_tmp1A5)!= 2)goto _LL14A;_tmp1A6=((struct Cyc_Absyn_Section_att_struct*)_tmp1A5)->f1;
_LL149: return Cyc_strcmp((struct _dynforward_ptr)_tmp1A4,(struct _dynforward_ptr)
_tmp1A6);_LL14A: _tmp1A7=_tmp196.f1;if(_tmp1A7 <= (void*)17)goto _LL14C;if(*((int*)
_tmp1A7)!= 3)goto _LL14C;_tmp1A8=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp1A7)->f1;_tmp1A9=((struct Cyc_Absyn_Format_att_struct*)_tmp1A7)->f2;_tmp1AA=((
struct Cyc_Absyn_Format_att_struct*)_tmp1A7)->f3;_tmp1AB=_tmp196.f2;if(_tmp1AB <= (
void*)17)goto _LL14C;if(*((int*)_tmp1AB)!= 3)goto _LL14C;_tmp1AC=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp1AB)->f1;_tmp1AD=((struct Cyc_Absyn_Format_att_struct*)
_tmp1AB)->f2;_tmp1AE=((struct Cyc_Absyn_Format_att_struct*)_tmp1AB)->f3;_LL14B: {
int _tmp1AF=Cyc_Core_intcmp((int)((unsigned int)_tmp1A8),(int)((unsigned int)
_tmp1AC));if(_tmp1AF != 0)return _tmp1AF;{int _tmp1B0=Cyc_Core_intcmp(_tmp1A9,
_tmp1AD);if(_tmp1B0 != 0)return _tmp1B0;return Cyc_Core_intcmp(_tmp1AA,_tmp1AE);}}
_LL14C:;_LL14D: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(
att2));_LL141:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(
a1,a2)== 0;}int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*
a2){{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a2))return 0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(
Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(
void*t);static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return
Cyc_Tcutil_rgns_of((void*)af->type);}static struct _tuple8*Cyc_Tcutil_region_free_subst(
struct Cyc_Absyn_Tvar*tv){void*t;{void*_tmp1B1=Cyc_Tcutil_tvar_kind(tv);_LL14F:
if((int)_tmp1B1 != 3)goto _LL151;_LL150: t=(void*)2;goto _LL14E;_LL151: if((int)
_tmp1B1 != 4)goto _LL153;_LL152: t=Cyc_Absyn_empty_effect;goto _LL14E;_LL153: if((int)
_tmp1B1 != 5)goto _LL155;_LL154: t=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp1B2=
_cycalloc_atomic(sizeof(*_tmp1B2));_tmp1B2[0]=({struct Cyc_Absyn_TypeInt_struct
_tmp1B3;_tmp1B3.tag=18;_tmp1B3.f1=0;_tmp1B3;});_tmp1B2;});goto _LL14E;_LL155:;
_LL156: t=Cyc_Absyn_sint_typ;goto _LL14E;_LL14E:;}return({struct _tuple8*_tmp1B4=
_cycalloc(sizeof(*_tmp1B4));_tmp1B4->f1=tv;_tmp1B4->f2=t;_tmp1B4;});}static void*
Cyc_Tcutil_rgns_of(void*t){void*_tmp1B5=Cyc_Tcutil_compress(t);void*_tmp1B6;
struct Cyc_Absyn_TunionInfo _tmp1B7;struct Cyc_List_List*_tmp1B8;void*_tmp1B9;
struct Cyc_Absyn_PtrInfo _tmp1BA;void*_tmp1BB;struct Cyc_Absyn_PtrAtts _tmp1BC;void*
_tmp1BD;struct Cyc_Absyn_ArrayInfo _tmp1BE;void*_tmp1BF;struct Cyc_List_List*
_tmp1C0;struct Cyc_Absyn_TunionFieldInfo _tmp1C1;struct Cyc_List_List*_tmp1C2;
struct Cyc_Absyn_AggrInfo _tmp1C3;struct Cyc_List_List*_tmp1C4;struct Cyc_List_List*
_tmp1C5;void*_tmp1C6;struct Cyc_Absyn_FnInfo _tmp1C7;struct Cyc_List_List*_tmp1C8;
struct Cyc_Core_Opt*_tmp1C9;void*_tmp1CA;struct Cyc_List_List*_tmp1CB;struct Cyc_Absyn_VarargInfo*
_tmp1CC;struct Cyc_List_List*_tmp1CD;void*_tmp1CE;struct Cyc_List_List*_tmp1CF;
_LL158: if((int)_tmp1B5 != 0)goto _LL15A;_LL159: goto _LL15B;_LL15A: if((int)_tmp1B5 != 
1)goto _LL15C;_LL15B: goto _LL15D;_LL15C: if(_tmp1B5 <= (void*)4)goto _LL180;if(*((int*)
_tmp1B5)!= 6)goto _LL15E;_LL15D: goto _LL15F;_LL15E: if(*((int*)_tmp1B5)!= 12)goto
_LL160;_LL15F: goto _LL161;_LL160: if(*((int*)_tmp1B5)!= 13)goto _LL162;_LL161: goto
_LL163;_LL162: if(*((int*)_tmp1B5)!= 18)goto _LL164;_LL163: goto _LL165;_LL164: if(*((
int*)_tmp1B5)!= 5)goto _LL166;_LL165: return Cyc_Absyn_empty_effect;_LL166: if(*((
int*)_tmp1B5)!= 0)goto _LL168;_LL167: goto _LL169;_LL168: if(*((int*)_tmp1B5)!= 1)
goto _LL16A;_LL169: {void*_tmp1D0=Cyc_Tcutil_typ_kind(t);_LL18D: if((int)_tmp1D0 != 
3)goto _LL18F;_LL18E: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1D1=
_cycalloc(sizeof(*_tmp1D1));_tmp1D1[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1D2;
_tmp1D2.tag=19;_tmp1D2.f1=(void*)t;_tmp1D2;});_tmp1D1;});_LL18F: if((int)_tmp1D0
!= 4)goto _LL191;_LL190: return t;_LL191: if((int)_tmp1D0 != 5)goto _LL193;_LL192:
return Cyc_Absyn_empty_effect;_LL193:;_LL194: return(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp1D4;_tmp1D4.tag=21;_tmp1D4.f1=(void*)t;_tmp1D4;});_tmp1D3;});_LL18C:;}_LL16A:
if(*((int*)_tmp1B5)!= 15)goto _LL16C;_tmp1B6=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp1B5)->f1;_LL16B: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1D5=
_cycalloc(sizeof(*_tmp1D5));_tmp1D5[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1D6;
_tmp1D6.tag=19;_tmp1D6.f1=(void*)_tmp1B6;_tmp1D6;});_tmp1D5;});_LL16C: if(*((int*)
_tmp1B5)!= 2)goto _LL16E;_tmp1B7=((struct Cyc_Absyn_TunionType_struct*)_tmp1B5)->f1;
_tmp1B8=_tmp1B7.targs;_tmp1B9=(void*)_tmp1B7.rgn;_LL16D: {struct Cyc_List_List*ts=({
struct Cyc_List_List*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));
_tmp1DA[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1DB;_tmp1DB.tag=19;_tmp1DB.f1=(
void*)_tmp1B9;_tmp1DB;});_tmp1DA;}));_tmp1D9->tl=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1B8);_tmp1D9;});return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1D8;_tmp1D8.tag=20;_tmp1D8.f1=ts;_tmp1D8;});_tmp1D7;}));}_LL16E: if(*((int*)
_tmp1B5)!= 4)goto _LL170;_tmp1BA=((struct Cyc_Absyn_PointerType_struct*)_tmp1B5)->f1;
_tmp1BB=(void*)_tmp1BA.elt_typ;_tmp1BC=_tmp1BA.ptr_atts;_tmp1BD=(void*)_tmp1BC.rgn;
_LL16F: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1DD;_tmp1DD.tag=20;_tmp1DD.f1=({void*_tmp1DE[2];_tmp1DE[1]=Cyc_Tcutil_rgns_of(
_tmp1BB);_tmp1DE[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1DF=_cycalloc(
sizeof(*_tmp1DF));_tmp1DF[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1E0;_tmp1E0.tag=
19;_tmp1E0.f1=(void*)_tmp1BD;_tmp1E0;});_tmp1DF;});Cyc_List_list(_tag_dynforward(
_tmp1DE,sizeof(void*),2));});_tmp1DD;});_tmp1DC;}));_LL170: if(*((int*)_tmp1B5)!= 
7)goto _LL172;_tmp1BE=((struct Cyc_Absyn_ArrayType_struct*)_tmp1B5)->f1;_tmp1BF=(
void*)_tmp1BE.elt_type;_LL171: return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(
_tmp1BF));_LL172: if(*((int*)_tmp1B5)!= 9)goto _LL174;_tmp1C0=((struct Cyc_Absyn_TupleType_struct*)
_tmp1B5)->f1;_LL173: {struct Cyc_List_List*_tmp1E1=0;for(0;_tmp1C0 != 0;_tmp1C0=
_tmp1C0->tl){_tmp1E1=({struct Cyc_List_List*_tmp1E2=_cycalloc(sizeof(*_tmp1E2));
_tmp1E2->hd=(void*)(*((struct _tuple4*)_tmp1C0->hd)).f2;_tmp1E2->tl=_tmp1E1;
_tmp1E2;});}_tmp1C2=_tmp1E1;goto _LL175;}_LL174: if(*((int*)_tmp1B5)!= 3)goto
_LL176;_tmp1C1=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1B5)->f1;_tmp1C2=
_tmp1C1.targs;_LL175: _tmp1C4=_tmp1C2;goto _LL177;_LL176: if(*((int*)_tmp1B5)!= 10)
goto _LL178;_tmp1C3=((struct Cyc_Absyn_AggrType_struct*)_tmp1B5)->f1;_tmp1C4=
_tmp1C3.targs;_LL177: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1E4;_tmp1E4.tag=20;_tmp1E4.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1C4);
_tmp1E4;});_tmp1E3;}));_LL178: if(*((int*)_tmp1B5)!= 11)goto _LL17A;_tmp1C5=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp1B5)->f2;_LL179: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1E5=_cycalloc(sizeof(*_tmp1E5));
_tmp1E5[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1E6;_tmp1E6.tag=20;_tmp1E6.f1=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1C5);_tmp1E6;});_tmp1E5;}));_LL17A:
if(*((int*)_tmp1B5)!= 14)goto _LL17C;_tmp1C6=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp1B5)->f1;_LL17B: return Cyc_Tcutil_rgns_of(_tmp1C6);_LL17C: if(*((int*)_tmp1B5)
!= 17)goto _LL17E;_LL17D: return Cyc_Tcutil_rgns_of(t);_LL17E: if(*((int*)_tmp1B5)!= 
8)goto _LL180;_tmp1C7=((struct Cyc_Absyn_FnType_struct*)_tmp1B5)->f1;_tmp1C8=
_tmp1C7.tvars;_tmp1C9=_tmp1C7.effect;_tmp1CA=(void*)_tmp1C7.ret_typ;_tmp1CB=
_tmp1C7.args;_tmp1CC=_tmp1C7.cyc_varargs;_tmp1CD=_tmp1C7.rgn_po;_LL17F: {void*
_tmp1E7=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp1C8),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1C9))->v);return Cyc_Tcutil_normalize_effect(
_tmp1E7);}_LL180: if((int)_tmp1B5 != 3)goto _LL182;_LL181: goto _LL183;_LL182: if((int)
_tmp1B5 != 2)goto _LL184;_LL183: return Cyc_Absyn_empty_effect;_LL184: if(_tmp1B5 <= (
void*)4)goto _LL186;if(*((int*)_tmp1B5)!= 19)goto _LL186;_LL185: goto _LL187;_LL186:
if(_tmp1B5 <= (void*)4)goto _LL188;if(*((int*)_tmp1B5)!= 20)goto _LL188;_LL187:
return t;_LL188: if(_tmp1B5 <= (void*)4)goto _LL18A;if(*((int*)_tmp1B5)!= 21)goto
_LL18A;_tmp1CE=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1B5)->f1;_LL189:
return Cyc_Tcutil_rgns_of(_tmp1CE);_LL18A: if(_tmp1B5 <= (void*)4)goto _LL157;if(*((
int*)_tmp1B5)!= 16)goto _LL157;_tmp1CF=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1B5)->f2;_LL18B: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1E9;_tmp1E9.tag=20;_tmp1E9.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1CF);
_tmp1E9;});_tmp1E8;}));_LL157:;}void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp1EA=e;struct Cyc_List_List*_tmp1EB;struct Cyc_List_List**_tmp1EC;void*
_tmp1ED;_LL196: if(_tmp1EA <= (void*)4)goto _LL19A;if(*((int*)_tmp1EA)!= 20)goto
_LL198;_tmp1EB=((struct Cyc_Absyn_JoinEff_struct*)_tmp1EA)->f1;_tmp1EC=(struct Cyc_List_List**)&((
struct Cyc_Absyn_JoinEff_struct*)_tmp1EA)->f1;_LL197: {int redo_join=0;{struct Cyc_List_List*
effs=*_tmp1EC;for(0;effs != 0;effs=effs->tl){void*_tmp1EE=(void*)effs->hd;(void*)(
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp1EE)));{void*
_tmp1EF=(void*)effs->hd;void*_tmp1F0;_LL19D: if(_tmp1EF <= (void*)4)goto _LL1A1;if(*((
int*)_tmp1EF)!= 20)goto _LL19F;_LL19E: goto _LL1A0;_LL19F: if(*((int*)_tmp1EF)!= 19)
goto _LL1A1;_tmp1F0=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1EF)->f1;if((
int)_tmp1F0 != 2)goto _LL1A1;_LL1A0: redo_join=1;goto _LL19C;_LL1A1:;_LL1A2: goto
_LL19C;_LL19C:;}}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct
Cyc_List_List*effs=*_tmp1EC;for(0;effs != 0;effs=effs->tl){void*_tmp1F1=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp1F2;void*_tmp1F3;_LL1A4: if(_tmp1F1 <= (
void*)4)goto _LL1A8;if(*((int*)_tmp1F1)!= 20)goto _LL1A6;_tmp1F2=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1F1)->f1;_LL1A5: effects=Cyc_List_revappend(_tmp1F2,effects);goto _LL1A3;
_LL1A6: if(*((int*)_tmp1F1)!= 19)goto _LL1A8;_tmp1F3=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1F1)->f1;if((int)_tmp1F3 != 2)goto _LL1A8;_LL1A7: goto _LL1A3;_LL1A8:;_LL1A9:
effects=({struct Cyc_List_List*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));_tmp1F4->hd=(
void*)_tmp1F1;_tmp1F4->tl=effects;_tmp1F4;});goto _LL1A3;_LL1A3:;}}*_tmp1EC=Cyc_List_imp_rev(
effects);return e;}}_LL198: if(*((int*)_tmp1EA)!= 21)goto _LL19A;_tmp1ED=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1EA)->f1;_LL199: {void*_tmp1F5=Cyc_Tcutil_compress(
_tmp1ED);_LL1AB: if(_tmp1F5 <= (void*)4)goto _LL1AF;if(*((int*)_tmp1F5)!= 0)goto
_LL1AD;_LL1AC: goto _LL1AE;_LL1AD: if(*((int*)_tmp1F5)!= 1)goto _LL1AF;_LL1AE: return
e;_LL1AF:;_LL1B0: return Cyc_Tcutil_rgns_of(_tmp1ED);_LL1AA:;}_LL19A:;_LL19B:
return e;_LL195:;}}static struct Cyc_Core_Opt Cyc_Tcutil_ek={(void*)((void*)4)};
static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct*
_tmp1F6=({struct Cyc_Absyn_FnType_struct*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));
_tmp1F7[0]=({struct Cyc_Absyn_FnType_struct _tmp1F8;_tmp1F8.tag=8;_tmp1F8.f1=({
struct Cyc_Absyn_FnInfo _tmp1F9;_tmp1F9.tvars=0;_tmp1F9.effect=({struct Cyc_Core_Opt*
_tmp1FA=_cycalloc(sizeof(*_tmp1FA));_tmp1FA->v=(void*)eff;_tmp1FA;});_tmp1F9.ret_typ=(
void*)((void*)0);_tmp1F9.args=0;_tmp1F9.c_varargs=0;_tmp1F9.cyc_varargs=0;
_tmp1F9.rgn_po=0;_tmp1F9.attributes=0;_tmp1F9;});_tmp1F8;});_tmp1F7;});return Cyc_Absyn_atb_typ((
void*)_tmp1F6,(void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){r=Cyc_Tcutil_compress(
r);if(r == (void*)2)return 1;{void*_tmp1FB=Cyc_Tcutil_compress(e);void*_tmp1FC;
struct Cyc_List_List*_tmp1FD;void*_tmp1FE;struct Cyc_Core_Opt*_tmp1FF;struct Cyc_Core_Opt*
_tmp200;struct Cyc_Core_Opt**_tmp201;struct Cyc_Core_Opt*_tmp202;_LL1B2: if(_tmp1FB
<= (void*)4)goto _LL1BA;if(*((int*)_tmp1FB)!= 19)goto _LL1B4;_tmp1FC=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp1FB)->f1;_LL1B3: if(constrain)return Cyc_Tcutil_unify(
r,_tmp1FC);_tmp1FC=Cyc_Tcutil_compress(_tmp1FC);if(r == _tmp1FC)return 1;{struct
_tuple6 _tmp204=({struct _tuple6 _tmp203;_tmp203.f1=r;_tmp203.f2=_tmp1FC;_tmp203;});
void*_tmp205;struct Cyc_Absyn_Tvar*_tmp206;void*_tmp207;struct Cyc_Absyn_Tvar*
_tmp208;_LL1BD: _tmp205=_tmp204.f1;if(_tmp205 <= (void*)4)goto _LL1BF;if(*((int*)
_tmp205)!= 1)goto _LL1BF;_tmp206=((struct Cyc_Absyn_VarType_struct*)_tmp205)->f1;
_tmp207=_tmp204.f2;if(_tmp207 <= (void*)4)goto _LL1BF;if(*((int*)_tmp207)!= 1)goto
_LL1BF;_tmp208=((struct Cyc_Absyn_VarType_struct*)_tmp207)->f1;_LL1BE: return Cyc_Absyn_tvar_cmp(
_tmp206,_tmp208)== 0;_LL1BF:;_LL1C0: return 0;_LL1BC:;}_LL1B4: if(*((int*)_tmp1FB)
!= 20)goto _LL1B6;_tmp1FD=((struct Cyc_Absyn_JoinEff_struct*)_tmp1FB)->f1;_LL1B5:
for(0;_tmp1FD != 0;_tmp1FD=_tmp1FD->tl){if(Cyc_Tcutil_region_in_effect(constrain,
r,(void*)_tmp1FD->hd))return 1;}return 0;_LL1B6: if(*((int*)_tmp1FB)!= 21)goto
_LL1B8;_tmp1FE=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1FB)->f1;_LL1B7: {
void*_tmp209=Cyc_Tcutil_rgns_of(_tmp1FE);void*_tmp20A;_LL1C2: if(_tmp209 <= (void*)
4)goto _LL1C4;if(*((int*)_tmp209)!= 21)goto _LL1C4;_tmp20A=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp209)->f1;_LL1C3: if(!constrain)return 0;{void*_tmp20B=Cyc_Tcutil_compress(
_tmp20A);struct Cyc_Core_Opt*_tmp20C;struct Cyc_Core_Opt*_tmp20D;struct Cyc_Core_Opt**
_tmp20E;struct Cyc_Core_Opt*_tmp20F;_LL1C7: if(_tmp20B <= (void*)4)goto _LL1C9;if(*((
int*)_tmp20B)!= 0)goto _LL1C9;_tmp20C=((struct Cyc_Absyn_Evar_struct*)_tmp20B)->f1;
_tmp20D=((struct Cyc_Absyn_Evar_struct*)_tmp20B)->f2;_tmp20E=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp20B)->f2;_tmp20F=((struct Cyc_Absyn_Evar_struct*)
_tmp20B)->f4;_LL1C8: {void*_tmp210=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp20F);Cyc_Tcutil_occurs(_tmp210,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp20F))->v,r);{void*_tmp211=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp213=_cycalloc(sizeof(*_tmp213));
_tmp213[0]=({struct Cyc_Absyn_JoinEff_struct _tmp214;_tmp214.tag=20;_tmp214.f1=({
void*_tmp215[2];_tmp215[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp216=
_cycalloc(sizeof(*_tmp216));_tmp216[0]=({struct Cyc_Absyn_AccessEff_struct _tmp217;
_tmp217.tag=19;_tmp217.f1=(void*)r;_tmp217;});_tmp216;});_tmp215[0]=_tmp210;Cyc_List_list(
_tag_dynforward(_tmp215,sizeof(void*),2));});_tmp214;});_tmp213;}));*_tmp20E=({
struct Cyc_Core_Opt*_tmp212=_cycalloc(sizeof(*_tmp212));_tmp212->v=(void*)_tmp211;
_tmp212;});return 1;}}_LL1C9:;_LL1CA: return 0;_LL1C6:;}_LL1C4:;_LL1C5: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp209);_LL1C1:;}_LL1B8: if(*((int*)_tmp1FB)!= 0)goto _LL1BA;_tmp1FF=((
struct Cyc_Absyn_Evar_struct*)_tmp1FB)->f1;_tmp200=((struct Cyc_Absyn_Evar_struct*)
_tmp1FB)->f2;_tmp201=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1FB)->f2;_tmp202=((struct Cyc_Absyn_Evar_struct*)_tmp1FB)->f4;_LL1B9: if(
_tmp1FF == 0  || (void*)_tmp1FF->v != (void*)4)({void*_tmp218[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp219="effect evar has wrong kind";_tag_dynforward(_tmp219,sizeof(char),
_get_zero_arr_size(_tmp219,27));}),_tag_dynforward(_tmp218,sizeof(void*),0));});
if(!constrain)return 0;{void*_tmp21A=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp202);Cyc_Tcutil_occurs(_tmp21A,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp202))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp21B=({struct Cyc_Absyn_JoinEff_struct*_tmp21D=_cycalloc(sizeof(*_tmp21D));
_tmp21D[0]=({struct Cyc_Absyn_JoinEff_struct _tmp21E;_tmp21E.tag=20;_tmp21E.f1=({
struct Cyc_List_List*_tmp21F=_cycalloc(sizeof(*_tmp21F));_tmp21F->hd=(void*)
_tmp21A;_tmp21F->tl=({struct Cyc_List_List*_tmp220=_cycalloc(sizeof(*_tmp220));
_tmp220->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp221=_cycalloc(
sizeof(*_tmp221));_tmp221[0]=({struct Cyc_Absyn_AccessEff_struct _tmp222;_tmp222.tag=
19;_tmp222.f1=(void*)r;_tmp222;});_tmp221;}));_tmp220->tl=0;_tmp220;});_tmp21F;});
_tmp21E;});_tmp21D;});*_tmp201=({struct Cyc_Core_Opt*_tmp21C=_cycalloc(sizeof(*
_tmp21C));_tmp21C->v=(void*)((void*)_tmp21B);_tmp21C;});return 1;}}_LL1BA:;_LL1BB:
return 0;_LL1B1:;}}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*
t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp223=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(
e));struct Cyc_List_List*_tmp224;void*_tmp225;struct Cyc_Core_Opt*_tmp226;struct
Cyc_Core_Opt*_tmp227;struct Cyc_Core_Opt**_tmp228;struct Cyc_Core_Opt*_tmp229;
_LL1CC: if(_tmp223 <= (void*)4)goto _LL1D4;if(*((int*)_tmp223)!= 19)goto _LL1CE;
_LL1CD: return 0;_LL1CE: if(*((int*)_tmp223)!= 20)goto _LL1D0;_tmp224=((struct Cyc_Absyn_JoinEff_struct*)
_tmp223)->f1;_LL1CF: for(0;_tmp224 != 0;_tmp224=_tmp224->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp224->hd))return 1;}return 0;_LL1D0: if(*((int*)
_tmp223)!= 21)goto _LL1D2;_tmp225=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp223)->f1;_LL1D1: _tmp225=Cyc_Tcutil_compress(_tmp225);if(t == _tmp225)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp225);{void*_tmp22A=Cyc_Tcutil_rgns_of(
t);void*_tmp22B;_LL1D7: if(_tmp22A <= (void*)4)goto _LL1D9;if(*((int*)_tmp22A)!= 21)
goto _LL1D9;_tmp22B=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp22A)->f1;_LL1D8: {
struct _tuple6 _tmp22D=({struct _tuple6 _tmp22C;_tmp22C.f1=Cyc_Tcutil_compress(
_tmp22B);_tmp22C.f2=_tmp225;_tmp22C;});void*_tmp22E;struct Cyc_Absyn_Tvar*_tmp22F;
void*_tmp230;struct Cyc_Absyn_Tvar*_tmp231;_LL1DC: _tmp22E=_tmp22D.f1;if(_tmp22E <= (
void*)4)goto _LL1DE;if(*((int*)_tmp22E)!= 1)goto _LL1DE;_tmp22F=((struct Cyc_Absyn_VarType_struct*)
_tmp22E)->f1;_tmp230=_tmp22D.f2;if(_tmp230 <= (void*)4)goto _LL1DE;if(*((int*)
_tmp230)!= 1)goto _LL1DE;_tmp231=((struct Cyc_Absyn_VarType_struct*)_tmp230)->f1;
_LL1DD: return Cyc_Tcutil_unify(t,_tmp225);_LL1DE:;_LL1DF: return _tmp22B == _tmp225;
_LL1DB:;}_LL1D9:;_LL1DA: return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,
_tmp22A);_LL1D6:;}_LL1D2: if(*((int*)_tmp223)!= 0)goto _LL1D4;_tmp226=((struct Cyc_Absyn_Evar_struct*)
_tmp223)->f1;_tmp227=((struct Cyc_Absyn_Evar_struct*)_tmp223)->f2;_tmp228=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp223)->f2;_tmp229=((struct Cyc_Absyn_Evar_struct*)
_tmp223)->f4;_LL1D3: if(_tmp226 == 0  || (void*)_tmp226->v != (void*)4)({void*
_tmp232[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp233="effect evar has wrong kind";_tag_dynforward(_tmp233,sizeof(
char),_get_zero_arr_size(_tmp233,27));}),_tag_dynforward(_tmp232,sizeof(void*),0));});
if(!may_constrain_evars)return 0;{void*_tmp234=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp229);Cyc_Tcutil_occurs(_tmp234,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp229))->v,t);{struct Cyc_Absyn_JoinEff_struct*
_tmp235=({struct Cyc_Absyn_JoinEff_struct*_tmp237=_cycalloc(sizeof(*_tmp237));
_tmp237[0]=({struct Cyc_Absyn_JoinEff_struct _tmp238;_tmp238.tag=20;_tmp238.f1=({
struct Cyc_List_List*_tmp239=_cycalloc(sizeof(*_tmp239));_tmp239->hd=(void*)
_tmp234;_tmp239->tl=({struct Cyc_List_List*_tmp23A=_cycalloc(sizeof(*_tmp23A));
_tmp23A->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp23B=_cycalloc(
sizeof(*_tmp23B));_tmp23B[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp23C;_tmp23C.tag=
21;_tmp23C.f1=(void*)t;_tmp23C;});_tmp23B;}));_tmp23A->tl=0;_tmp23A;});_tmp239;});
_tmp238;});_tmp237;});*_tmp228=({struct Cyc_Core_Opt*_tmp236=_cycalloc(sizeof(*
_tmp236));_tmp236->v=(void*)((void*)_tmp235);_tmp236;});return 1;}}_LL1D4:;_LL1D5:
return 0;_LL1CB:;}}static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,
struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp23D=e;struct
Cyc_Absyn_Tvar*_tmp23E;struct Cyc_List_List*_tmp23F;void*_tmp240;struct Cyc_Core_Opt*
_tmp241;struct Cyc_Core_Opt*_tmp242;struct Cyc_Core_Opt**_tmp243;struct Cyc_Core_Opt*
_tmp244;_LL1E1: if(_tmp23D <= (void*)4)goto _LL1E9;if(*((int*)_tmp23D)!= 1)goto
_LL1E3;_tmp23E=((struct Cyc_Absyn_VarType_struct*)_tmp23D)->f1;_LL1E2: return Cyc_Absyn_tvar_cmp(
v,_tmp23E)== 0;_LL1E3: if(*((int*)_tmp23D)!= 20)goto _LL1E5;_tmp23F=((struct Cyc_Absyn_JoinEff_struct*)
_tmp23D)->f1;_LL1E4: for(0;_tmp23F != 0;_tmp23F=_tmp23F->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp23F->hd))return 1;}return 0;_LL1E5: if(*((int*)
_tmp23D)!= 21)goto _LL1E7;_tmp240=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp23D)->f1;_LL1E6: {void*_tmp245=Cyc_Tcutil_rgns_of(_tmp240);void*_tmp246;
_LL1EC: if(_tmp245 <= (void*)4)goto _LL1EE;if(*((int*)_tmp245)!= 21)goto _LL1EE;
_tmp246=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp245)->f1;_LL1ED: if(!
may_constrain_evars)return 0;{void*_tmp247=Cyc_Tcutil_compress(_tmp246);struct Cyc_Core_Opt*
_tmp248;struct Cyc_Core_Opt*_tmp249;struct Cyc_Core_Opt**_tmp24A;struct Cyc_Core_Opt*
_tmp24B;_LL1F1: if(_tmp247 <= (void*)4)goto _LL1F3;if(*((int*)_tmp247)!= 0)goto
_LL1F3;_tmp248=((struct Cyc_Absyn_Evar_struct*)_tmp247)->f1;_tmp249=((struct Cyc_Absyn_Evar_struct*)
_tmp247)->f2;_tmp24A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp247)->f2;_tmp24B=((struct Cyc_Absyn_Evar_struct*)_tmp247)->f4;_LL1F2: {void*
_tmp24C=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp24B);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp24B))->v,v))return 0;{void*_tmp24D=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp24F=_cycalloc(sizeof(*_tmp24F));
_tmp24F[0]=({struct Cyc_Absyn_JoinEff_struct _tmp250;_tmp250.tag=20;_tmp250.f1=({
void*_tmp251[2];_tmp251[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp252=
_cycalloc(sizeof(*_tmp252));_tmp252[0]=({struct Cyc_Absyn_VarType_struct _tmp253;
_tmp253.tag=1;_tmp253.f1=v;_tmp253;});_tmp252;});_tmp251[0]=_tmp24C;Cyc_List_list(
_tag_dynforward(_tmp251,sizeof(void*),2));});_tmp250;});_tmp24F;}));*_tmp24A=({
struct Cyc_Core_Opt*_tmp24E=_cycalloc(sizeof(*_tmp24E));_tmp24E->v=(void*)_tmp24D;
_tmp24E;});return 1;}}_LL1F3:;_LL1F4: return 0;_LL1F0:;}_LL1EE:;_LL1EF: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp245);_LL1EB:;}_LL1E7: if(*((int*)_tmp23D)!= 0)goto
_LL1E9;_tmp241=((struct Cyc_Absyn_Evar_struct*)_tmp23D)->f1;_tmp242=((struct Cyc_Absyn_Evar_struct*)
_tmp23D)->f2;_tmp243=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp23D)->f2;_tmp244=((struct Cyc_Absyn_Evar_struct*)_tmp23D)->f4;_LL1E8: if(
_tmp241 == 0  || (void*)_tmp241->v != (void*)4)({void*_tmp254[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp255="effect evar has wrong kind";_tag_dynforward(_tmp255,sizeof(char),
_get_zero_arr_size(_tmp255,27));}),_tag_dynforward(_tmp254,sizeof(void*),0));});{
void*_tmp256=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp244);if(
!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp244))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct*
_tmp257=({struct Cyc_Absyn_JoinEff_struct*_tmp259=_cycalloc(sizeof(*_tmp259));
_tmp259[0]=({struct Cyc_Absyn_JoinEff_struct _tmp25A;_tmp25A.tag=20;_tmp25A.f1=({
struct Cyc_List_List*_tmp25B=_cycalloc(sizeof(*_tmp25B));_tmp25B->hd=(void*)
_tmp256;_tmp25B->tl=({struct Cyc_List_List*_tmp25C=_cycalloc(sizeof(*_tmp25C));
_tmp25C->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp25D=_cycalloc(
sizeof(*_tmp25D));_tmp25D[0]=({struct Cyc_Absyn_VarType_struct _tmp25E;_tmp25E.tag=
1;_tmp25E.f1=v;_tmp25E;});_tmp25D;}));_tmp25C->tl=0;_tmp25C;});_tmp25B;});
_tmp25A;});_tmp259;});*_tmp243=({struct Cyc_Core_Opt*_tmp258=_cycalloc(sizeof(*
_tmp258));_tmp258->v=(void*)((void*)_tmp257);_tmp258;});return 1;}}_LL1E9:;_LL1EA:
return 0;_LL1E0:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp25F=e;struct Cyc_List_List*_tmp260;void*_tmp261;_LL1F6: if(_tmp25F <= (
void*)4)goto _LL1FC;if(*((int*)_tmp25F)!= 20)goto _LL1F8;_tmp260=((struct Cyc_Absyn_JoinEff_struct*)
_tmp25F)->f1;_LL1F7: for(0;_tmp260 != 0;_tmp260=_tmp260->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp260->hd))return 1;}return 0;_LL1F8: if(*((int*)_tmp25F)!= 21)goto
_LL1FA;_tmp261=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp25F)->f1;_LL1F9: {
void*_tmp262=Cyc_Tcutil_rgns_of(_tmp261);void*_tmp263;_LL1FF: if(_tmp262 <= (void*)
4)goto _LL201;if(*((int*)_tmp262)!= 21)goto _LL201;_tmp263=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp262)->f1;_LL200: return 0;_LL201:;_LL202: return Cyc_Tcutil_evar_in_effect(evar,
_tmp262);_LL1FE:;}_LL1FA: if(*((int*)_tmp25F)!= 0)goto _LL1FC;_LL1FB: return evar == 
e;_LL1FC:;_LL1FD: return 0;_LL1F5:;}}int Cyc_Tcutil_subset_effect(int
may_constrain_evars,void*e1,void*e2){void*_tmp264=Cyc_Tcutil_compress(e1);struct
Cyc_List_List*_tmp265;void*_tmp266;struct Cyc_Absyn_Tvar*_tmp267;void*_tmp268;
struct Cyc_Core_Opt*_tmp269;struct Cyc_Core_Opt**_tmp26A;struct Cyc_Core_Opt*
_tmp26B;_LL204: if(_tmp264 <= (void*)4)goto _LL20E;if(*((int*)_tmp264)!= 20)goto
_LL206;_tmp265=((struct Cyc_Absyn_JoinEff_struct*)_tmp264)->f1;_LL205: for(0;
_tmp265 != 0;_tmp265=_tmp265->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp265->hd,e2))return 0;}return 1;_LL206: if(*((int*)
_tmp264)!= 19)goto _LL208;_tmp266=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp264)->f1;_LL207: return Cyc_Tcutil_region_in_effect(0,_tmp266,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp266,(void*)2);_LL208: if(*((int*)
_tmp264)!= 1)goto _LL20A;_tmp267=((struct Cyc_Absyn_VarType_struct*)_tmp264)->f1;
_LL209: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp267,e2);
_LL20A: if(*((int*)_tmp264)!= 21)goto _LL20C;_tmp268=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp264)->f1;_LL20B: {void*_tmp26C=Cyc_Tcutil_rgns_of(_tmp268);void*_tmp26D;
_LL211: if(_tmp26C <= (void*)4)goto _LL213;if(*((int*)_tmp26C)!= 21)goto _LL213;
_tmp26D=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp26C)->f1;_LL212: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,_tmp26D,e2) || may_constrain_evars  && Cyc_Tcutil_unify(
_tmp26D,Cyc_Absyn_sint_typ);_LL213:;_LL214: return Cyc_Tcutil_subset_effect(
may_constrain_evars,_tmp26C,e2);_LL210:;}_LL20C: if(*((int*)_tmp264)!= 0)goto
_LL20E;_tmp269=((struct Cyc_Absyn_Evar_struct*)_tmp264)->f2;_tmp26A=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp264)->f2;_tmp26B=((struct Cyc_Absyn_Evar_struct*)
_tmp264)->f4;_LL20D: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*_tmp26A=({struct Cyc_Core_Opt*
_tmp26E=_cycalloc(sizeof(*_tmp26E));_tmp26E->v=(void*)Cyc_Absyn_empty_effect;
_tmp26E;});return 1;_LL20E:;_LL20F:({struct Cyc_String_pa_struct _tmp271;_tmp271.tag=
0;_tmp271.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
e1));{void*_tmp26F[1]={& _tmp271};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp270="subset_effect: bad effect: %s";
_tag_dynforward(_tmp270,sizeof(char),_get_zero_arr_size(_tmp270,30));}),
_tag_dynforward(_tmp26F,sizeof(void*),1));}});_LL203:;}static int Cyc_Tcutil_unify_effect(
void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(e1);e2=Cyc_Tcutil_normalize_effect(
e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct
Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){struct
_tuple6 _tmp273;void*_tmp274;void*_tmp275;struct _tuple6*_tmp272=(struct _tuple6*)
r1->hd;_tmp273=*_tmp272;_tmp274=_tmp273.f1;_tmp275=_tmp273.f2;{int found=_tmp274
== (void*)2;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple6 _tmp277;void*_tmp278;void*_tmp279;struct _tuple6*_tmp276=(struct
_tuple6*)r2->hd;_tmp277=*_tmp276;_tmp278=_tmp277.f1;_tmp279=_tmp277.f2;if(Cyc_Tcutil_unify(
_tmp274,_tmp278) && Cyc_Tcutil_unify(_tmp275,_tmp279)){found=1;break;}}}if(!
found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*
rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(
rpo2,rpo1);}struct _tuple11{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*
f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _dynforward_ptr)_tag_dynforward(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp27A=t1;struct Cyc_Core_Opt*_tmp27B;struct Cyc_Core_Opt*
_tmp27C;struct Cyc_Core_Opt**_tmp27D;struct Cyc_Core_Opt*_tmp27E;_LL216: if(_tmp27A
<= (void*)4)goto _LL218;if(*((int*)_tmp27A)!= 0)goto _LL218;_tmp27B=((struct Cyc_Absyn_Evar_struct*)
_tmp27A)->f1;_tmp27C=((struct Cyc_Absyn_Evar_struct*)_tmp27A)->f2;_tmp27D=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp27A)->f2;_tmp27E=((struct Cyc_Absyn_Evar_struct*)
_tmp27A)->f4;_LL217: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp27E))->v,t2);{void*_tmp27F=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp27F,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp27B))->v)){*_tmp27D=({struct Cyc_Core_Opt*_tmp280=_cycalloc(sizeof(*_tmp280));
_tmp280->v=(void*)t2;_tmp280;});return;}else{{void*_tmp281=t2;struct Cyc_Core_Opt*
_tmp282;struct Cyc_Core_Opt**_tmp283;struct Cyc_Core_Opt*_tmp284;struct Cyc_Absyn_PtrInfo
_tmp285;_LL21B: if(_tmp281 <= (void*)4)goto _LL21F;if(*((int*)_tmp281)!= 0)goto
_LL21D;_tmp282=((struct Cyc_Absyn_Evar_struct*)_tmp281)->f2;_tmp283=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp281)->f2;_tmp284=((struct Cyc_Absyn_Evar_struct*)
_tmp281)->f4;_LL21C: {struct Cyc_List_List*_tmp286=(struct Cyc_List_List*)_tmp27E->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp284))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp286,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=({
const char*_tmp287="(type variable would escape scope)";_tag_dynforward(_tmp287,
sizeof(char),_get_zero_arr_size(_tmp287,35));});(int)_throw((void*)Cyc_Tcutil_Unify);}}}
if(Cyc_Tcutil_kind_leq((void*)_tmp27B->v,_tmp27F)){*_tmp283=({struct Cyc_Core_Opt*
_tmp288=_cycalloc(sizeof(*_tmp288));_tmp288->v=(void*)t1;_tmp288;});return;}Cyc_Tcutil_failure_reason=({
const char*_tmp289="(kinds are incompatible)";_tag_dynforward(_tmp289,sizeof(char),
_get_zero_arr_size(_tmp289,25));});goto _LL21A;}_LL21D: if(*((int*)_tmp281)!= 4)
goto _LL21F;_tmp285=((struct Cyc_Absyn_PointerType_struct*)_tmp281)->f1;if(!((void*)
_tmp27B->v == (void*)2))goto _LL21F;_LL21E: {struct Cyc_Absyn_Conref*_tmp28A=Cyc_Absyn_compress_conref((
_tmp285.ptr_atts).bounds);{void*_tmp28B=(void*)_tmp28A->v;_LL222: if((int)_tmp28B
!= 0)goto _LL224;_LL223:(void*)(_tmp28A->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp28C=_cycalloc(sizeof(*_tmp28C));_tmp28C[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp28D;_tmp28D.tag=0;_tmp28D.f1=(void*)Cyc_Absyn_bounds_one;_tmp28D;});_tmp28C;})));*
_tmp27D=({struct Cyc_Core_Opt*_tmp28E=_cycalloc(sizeof(*_tmp28E));_tmp28E->v=(
void*)t2;_tmp28E;});return;_LL224:;_LL225: goto _LL221;_LL221:;}goto _LL21A;}_LL21F:;
_LL220: goto _LL21A;_LL21A:;}Cyc_Tcutil_failure_reason=({const char*_tmp28F="(kinds are incompatible)";
_tag_dynforward(_tmp28F,sizeof(char),_get_zero_arr_size(_tmp28F,25));});(int)
_throw((void*)Cyc_Tcutil_Unify);}}_LL218:;_LL219: goto _LL215;_LL215:;}{struct
_tuple6 _tmp291=({struct _tuple6 _tmp290;_tmp290.f1=t2;_tmp290.f2=t1;_tmp290;});
void*_tmp292;void*_tmp293;void*_tmp294;void*_tmp295;struct Cyc_Absyn_Tvar*_tmp296;
void*_tmp297;struct Cyc_Absyn_Tvar*_tmp298;void*_tmp299;struct Cyc_Absyn_AggrInfo
_tmp29A;void*_tmp29B;struct Cyc_List_List*_tmp29C;void*_tmp29D;struct Cyc_Absyn_AggrInfo
_tmp29E;void*_tmp29F;struct Cyc_List_List*_tmp2A0;void*_tmp2A1;struct _tuple1*
_tmp2A2;void*_tmp2A3;struct _tuple1*_tmp2A4;void*_tmp2A5;struct Cyc_List_List*
_tmp2A6;void*_tmp2A7;struct Cyc_List_List*_tmp2A8;void*_tmp2A9;struct Cyc_Absyn_TunionInfo
_tmp2AA;void*_tmp2AB;struct Cyc_Absyn_Tuniondecl**_tmp2AC;struct Cyc_Absyn_Tuniondecl*
_tmp2AD;struct Cyc_List_List*_tmp2AE;void*_tmp2AF;void*_tmp2B0;struct Cyc_Absyn_TunionInfo
_tmp2B1;void*_tmp2B2;struct Cyc_Absyn_Tuniondecl**_tmp2B3;struct Cyc_Absyn_Tuniondecl*
_tmp2B4;struct Cyc_List_List*_tmp2B5;void*_tmp2B6;void*_tmp2B7;struct Cyc_Absyn_TunionFieldInfo
_tmp2B8;void*_tmp2B9;struct Cyc_Absyn_Tuniondecl*_tmp2BA;struct Cyc_Absyn_Tunionfield*
_tmp2BB;struct Cyc_List_List*_tmp2BC;void*_tmp2BD;struct Cyc_Absyn_TunionFieldInfo
_tmp2BE;void*_tmp2BF;struct Cyc_Absyn_Tuniondecl*_tmp2C0;struct Cyc_Absyn_Tunionfield*
_tmp2C1;struct Cyc_List_List*_tmp2C2;void*_tmp2C3;struct Cyc_Absyn_PtrInfo _tmp2C4;
void*_tmp2C5;struct Cyc_Absyn_Tqual _tmp2C6;struct Cyc_Absyn_PtrAtts _tmp2C7;void*
_tmp2C8;struct Cyc_Absyn_Conref*_tmp2C9;struct Cyc_Absyn_Conref*_tmp2CA;struct Cyc_Absyn_Conref*
_tmp2CB;void*_tmp2CC;struct Cyc_Absyn_PtrInfo _tmp2CD;void*_tmp2CE;struct Cyc_Absyn_Tqual
_tmp2CF;struct Cyc_Absyn_PtrAtts _tmp2D0;void*_tmp2D1;struct Cyc_Absyn_Conref*
_tmp2D2;struct Cyc_Absyn_Conref*_tmp2D3;struct Cyc_Absyn_Conref*_tmp2D4;void*
_tmp2D5;void*_tmp2D6;void*_tmp2D7;void*_tmp2D8;void*_tmp2D9;void*_tmp2DA;void*
_tmp2DB;void*_tmp2DC;void*_tmp2DD;int _tmp2DE;void*_tmp2DF;int _tmp2E0;void*
_tmp2E1;void*_tmp2E2;void*_tmp2E3;void*_tmp2E4;void*_tmp2E5;int _tmp2E6;void*
_tmp2E7;int _tmp2E8;void*_tmp2E9;void*_tmp2EA;void*_tmp2EB;void*_tmp2EC;void*
_tmp2ED;struct Cyc_Absyn_ArrayInfo _tmp2EE;void*_tmp2EF;struct Cyc_Absyn_Tqual
_tmp2F0;struct Cyc_Absyn_Exp*_tmp2F1;struct Cyc_Absyn_Conref*_tmp2F2;void*_tmp2F3;
struct Cyc_Absyn_ArrayInfo _tmp2F4;void*_tmp2F5;struct Cyc_Absyn_Tqual _tmp2F6;
struct Cyc_Absyn_Exp*_tmp2F7;struct Cyc_Absyn_Conref*_tmp2F8;void*_tmp2F9;struct
Cyc_Absyn_FnInfo _tmp2FA;struct Cyc_List_List*_tmp2FB;struct Cyc_Core_Opt*_tmp2FC;
void*_tmp2FD;struct Cyc_List_List*_tmp2FE;int _tmp2FF;struct Cyc_Absyn_VarargInfo*
_tmp300;struct Cyc_List_List*_tmp301;struct Cyc_List_List*_tmp302;void*_tmp303;
struct Cyc_Absyn_FnInfo _tmp304;struct Cyc_List_List*_tmp305;struct Cyc_Core_Opt*
_tmp306;void*_tmp307;struct Cyc_List_List*_tmp308;int _tmp309;struct Cyc_Absyn_VarargInfo*
_tmp30A;struct Cyc_List_List*_tmp30B;struct Cyc_List_List*_tmp30C;void*_tmp30D;
struct Cyc_List_List*_tmp30E;void*_tmp30F;struct Cyc_List_List*_tmp310;void*
_tmp311;void*_tmp312;struct Cyc_List_List*_tmp313;void*_tmp314;void*_tmp315;
struct Cyc_List_List*_tmp316;void*_tmp317;void*_tmp318;void*_tmp319;void*_tmp31A;
void*_tmp31B;void*_tmp31C;void*_tmp31D;void*_tmp31E;void*_tmp31F;void*_tmp320;
void*_tmp321;void*_tmp322;void*_tmp323;void*_tmp324;_LL227: _tmp292=_tmp291.f1;
if(_tmp292 <= (void*)4)goto _LL229;if(*((int*)_tmp292)!= 0)goto _LL229;_LL228: Cyc_Tcutil_unify_it(
t2,t1);return;_LL229: _tmp293=_tmp291.f1;if((int)_tmp293 != 0)goto _LL22B;_tmp294=
_tmp291.f2;if((int)_tmp294 != 0)goto _LL22B;_LL22A: return;_LL22B: _tmp295=_tmp291.f1;
if(_tmp295 <= (void*)4)goto _LL22D;if(*((int*)_tmp295)!= 1)goto _LL22D;_tmp296=((
struct Cyc_Absyn_VarType_struct*)_tmp295)->f1;_tmp297=_tmp291.f2;if(_tmp297 <= (
void*)4)goto _LL22D;if(*((int*)_tmp297)!= 1)goto _LL22D;_tmp298=((struct Cyc_Absyn_VarType_struct*)
_tmp297)->f1;_LL22C: {struct _dynforward_ptr*_tmp325=_tmp296->name;struct
_dynforward_ptr*_tmp326=_tmp298->name;int _tmp327=*((int*)_check_null(_tmp296->identity));
int _tmp328=*((int*)_check_null(_tmp298->identity));void*_tmp329=Cyc_Tcutil_tvar_kind(
_tmp296);void*_tmp32A=Cyc_Tcutil_tvar_kind(_tmp298);if(_tmp328 == _tmp327  && Cyc_strptrcmp(
_tmp325,_tmp326)== 0){if(_tmp329 != _tmp32A)({struct Cyc_String_pa_struct _tmp32F;
_tmp32F.tag=0;_tmp32F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(
_tmp32A));{struct Cyc_String_pa_struct _tmp32E;_tmp32E.tag=0;_tmp32E.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(_tmp329));{
struct Cyc_String_pa_struct _tmp32D;_tmp32D.tag=0;_tmp32D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp325);{void*_tmp32B[3]={& _tmp32D,&
_tmp32E,& _tmp32F};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp32C="same type variable %s has kinds %s and %s";_tag_dynforward(
_tmp32C,sizeof(char),_get_zero_arr_size(_tmp32C,42));}),_tag_dynforward(_tmp32B,
sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=({const char*_tmp330="(variable types are not the same)";
_tag_dynforward(_tmp330,sizeof(char),_get_zero_arr_size(_tmp330,34));});goto
_LL226;}_LL22D: _tmp299=_tmp291.f1;if(_tmp299 <= (void*)4)goto _LL22F;if(*((int*)
_tmp299)!= 10)goto _LL22F;_tmp29A=((struct Cyc_Absyn_AggrType_struct*)_tmp299)->f1;
_tmp29B=(void*)_tmp29A.aggr_info;_tmp29C=_tmp29A.targs;_tmp29D=_tmp291.f2;if(
_tmp29D <= (void*)4)goto _LL22F;if(*((int*)_tmp29D)!= 10)goto _LL22F;_tmp29E=((
struct Cyc_Absyn_AggrType_struct*)_tmp29D)->f1;_tmp29F=(void*)_tmp29E.aggr_info;
_tmp2A0=_tmp29E.targs;_LL22E: {void*_tmp332;struct _tuple1*_tmp333;struct _tuple5
_tmp331=Cyc_Absyn_aggr_kinded_name(_tmp29F);_tmp332=_tmp331.f1;_tmp333=_tmp331.f2;{
void*_tmp335;struct _tuple1*_tmp336;struct _tuple5 _tmp334=Cyc_Absyn_aggr_kinded_name(
_tmp29B);_tmp335=_tmp334.f1;_tmp336=_tmp334.f2;if(_tmp332 != _tmp335){Cyc_Tcutil_failure_reason=({
const char*_tmp337="(struct and union type)";_tag_dynforward(_tmp337,sizeof(char),
_get_zero_arr_size(_tmp337,24));});goto _LL226;}if(Cyc_Absyn_qvar_cmp(_tmp333,
_tmp336)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp338="(different type name)";
_tag_dynforward(_tmp338,sizeof(char),_get_zero_arr_size(_tmp338,22));});goto
_LL226;}Cyc_Tcutil_unify_list(_tmp2A0,_tmp29C);return;}}_LL22F: _tmp2A1=_tmp291.f1;
if(_tmp2A1 <= (void*)4)goto _LL231;if(*((int*)_tmp2A1)!= 12)goto _LL231;_tmp2A2=((
struct Cyc_Absyn_EnumType_struct*)_tmp2A1)->f1;_tmp2A3=_tmp291.f2;if(_tmp2A3 <= (
void*)4)goto _LL231;if(*((int*)_tmp2A3)!= 12)goto _LL231;_tmp2A4=((struct Cyc_Absyn_EnumType_struct*)
_tmp2A3)->f1;_LL230: if(Cyc_Absyn_qvar_cmp(_tmp2A2,_tmp2A4)== 0)return;Cyc_Tcutil_failure_reason=({
const char*_tmp339="(different enum types)";_tag_dynforward(_tmp339,sizeof(char),
_get_zero_arr_size(_tmp339,23));});goto _LL226;_LL231: _tmp2A5=_tmp291.f1;if(
_tmp2A5 <= (void*)4)goto _LL233;if(*((int*)_tmp2A5)!= 13)goto _LL233;_tmp2A6=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp2A5)->f1;_tmp2A7=_tmp291.f2;if(_tmp2A7
<= (void*)4)goto _LL233;if(*((int*)_tmp2A7)!= 13)goto _LL233;_tmp2A8=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp2A7)->f1;_LL232: {int bad=0;for(0;_tmp2A6 != 0  && _tmp2A8 != 0;(_tmp2A6=_tmp2A6->tl,
_tmp2A8=_tmp2A8->tl)){struct Cyc_Absyn_Enumfield*_tmp33A=(struct Cyc_Absyn_Enumfield*)
_tmp2A6->hd;struct Cyc_Absyn_Enumfield*_tmp33B=(struct Cyc_Absyn_Enumfield*)
_tmp2A8->hd;if(Cyc_Absyn_qvar_cmp(_tmp33A->name,_tmp33B->name)!= 0){Cyc_Tcutil_failure_reason=({
const char*_tmp33C="(different names for enum fields)";_tag_dynforward(_tmp33C,
sizeof(char),_get_zero_arr_size(_tmp33C,34));});bad=1;break;}if(_tmp33A->tag == 
_tmp33B->tag)continue;if(_tmp33A->tag == 0  || _tmp33B->tag == 0){Cyc_Tcutil_failure_reason=({
const char*_tmp33D="(different tag values for enum fields)";_tag_dynforward(
_tmp33D,sizeof(char),_get_zero_arr_size(_tmp33D,39));});bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp33A->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp33B->tag))){Cyc_Tcutil_failure_reason=({const char*_tmp33E="(different tag values for enum fields)";
_tag_dynforward(_tmp33E,sizeof(char),_get_zero_arr_size(_tmp33E,39));});bad=1;
break;}}if(bad)goto _LL226;if(_tmp2A6 == 0  && _tmp2A8 == 0)return;Cyc_Tcutil_failure_reason=({
const char*_tmp33F="(different number of fields for enums)";_tag_dynforward(
_tmp33F,sizeof(char),_get_zero_arr_size(_tmp33F,39));});goto _LL226;}_LL233:
_tmp2A9=_tmp291.f1;if(_tmp2A9 <= (void*)4)goto _LL235;if(*((int*)_tmp2A9)!= 2)goto
_LL235;_tmp2AA=((struct Cyc_Absyn_TunionType_struct*)_tmp2A9)->f1;_tmp2AB=(void*)
_tmp2AA.tunion_info;if(*((int*)_tmp2AB)!= 1)goto _LL235;_tmp2AC=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp2AB)->f1;_tmp2AD=*_tmp2AC;_tmp2AE=_tmp2AA.targs;_tmp2AF=(void*)_tmp2AA.rgn;
_tmp2B0=_tmp291.f2;if(_tmp2B0 <= (void*)4)goto _LL235;if(*((int*)_tmp2B0)!= 2)goto
_LL235;_tmp2B1=((struct Cyc_Absyn_TunionType_struct*)_tmp2B0)->f1;_tmp2B2=(void*)
_tmp2B1.tunion_info;if(*((int*)_tmp2B2)!= 1)goto _LL235;_tmp2B3=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp2B2)->f1;_tmp2B4=*_tmp2B3;_tmp2B5=_tmp2B1.targs;_tmp2B6=(void*)_tmp2B1.rgn;
_LL234: if(_tmp2AD == _tmp2B4  || Cyc_Absyn_qvar_cmp(_tmp2AD->name,_tmp2B4->name)== 
0){Cyc_Tcutil_unify_it(_tmp2B6,_tmp2AF);Cyc_Tcutil_unify_list(_tmp2B5,_tmp2AE);
return;}Cyc_Tcutil_failure_reason=({const char*_tmp340="(different tunion types)";
_tag_dynforward(_tmp340,sizeof(char),_get_zero_arr_size(_tmp340,25));});goto
_LL226;_LL235: _tmp2B7=_tmp291.f1;if(_tmp2B7 <= (void*)4)goto _LL237;if(*((int*)
_tmp2B7)!= 3)goto _LL237;_tmp2B8=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp2B7)->f1;_tmp2B9=(void*)_tmp2B8.field_info;if(*((int*)_tmp2B9)!= 1)goto
_LL237;_tmp2BA=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2B9)->f1;_tmp2BB=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2B9)->f2;_tmp2BC=_tmp2B8.targs;
_tmp2BD=_tmp291.f2;if(_tmp2BD <= (void*)4)goto _LL237;if(*((int*)_tmp2BD)!= 3)goto
_LL237;_tmp2BE=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp2BD)->f1;_tmp2BF=(
void*)_tmp2BE.field_info;if(*((int*)_tmp2BF)!= 1)goto _LL237;_tmp2C0=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp2BF)->f1;_tmp2C1=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2BF)->f2;
_tmp2C2=_tmp2BE.targs;_LL236: if((_tmp2BA == _tmp2C0  || Cyc_Absyn_qvar_cmp(_tmp2BA->name,
_tmp2C0->name)== 0) && (_tmp2BB == _tmp2C1  || Cyc_Absyn_qvar_cmp(_tmp2BB->name,
_tmp2C1->name)== 0)){Cyc_Tcutil_unify_list(_tmp2C2,_tmp2BC);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp341="(different tunion field types)";_tag_dynforward(_tmp341,
sizeof(char),_get_zero_arr_size(_tmp341,31));});goto _LL226;_LL237: _tmp2C3=
_tmp291.f1;if(_tmp2C3 <= (void*)4)goto _LL239;if(*((int*)_tmp2C3)!= 4)goto _LL239;
_tmp2C4=((struct Cyc_Absyn_PointerType_struct*)_tmp2C3)->f1;_tmp2C5=(void*)
_tmp2C4.elt_typ;_tmp2C6=_tmp2C4.elt_tq;_tmp2C7=_tmp2C4.ptr_atts;_tmp2C8=(void*)
_tmp2C7.rgn;_tmp2C9=_tmp2C7.nullable;_tmp2CA=_tmp2C7.bounds;_tmp2CB=_tmp2C7.zero_term;
_tmp2CC=_tmp291.f2;if(_tmp2CC <= (void*)4)goto _LL239;if(*((int*)_tmp2CC)!= 4)goto
_LL239;_tmp2CD=((struct Cyc_Absyn_PointerType_struct*)_tmp2CC)->f1;_tmp2CE=(void*)
_tmp2CD.elt_typ;_tmp2CF=_tmp2CD.elt_tq;_tmp2D0=_tmp2CD.ptr_atts;_tmp2D1=(void*)
_tmp2D0.rgn;_tmp2D2=_tmp2D0.nullable;_tmp2D3=_tmp2D0.bounds;_tmp2D4=_tmp2D0.zero_term;
_LL238: Cyc_Tcutil_unify_it(_tmp2CE,_tmp2C5);Cyc_Tcutil_unify_it(_tmp2C8,_tmp2D1);
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;((void(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2D4,_tmp2CB,({const char*_tmp342="(not both zero terminated)";
_tag_dynforward(_tmp342,sizeof(char),_get_zero_arr_size(_tmp342,27));}));Cyc_Tcutil_unify_tqual(
_tmp2CF,_tmp2CE,_tmp2C6,_tmp2C5);Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp2D3,_tmp2CA,({const char*_tmp343="(different pointer bounds)";_tag_dynforward(
_tmp343,sizeof(char),_get_zero_arr_size(_tmp343,27));}));{void*_tmp344=(void*)(
Cyc_Absyn_compress_conref(_tmp2D3))->v;void*_tmp345;void*_tmp346;_LL262: if(
_tmp344 <= (void*)1)goto _LL266;if(*((int*)_tmp344)!= 0)goto _LL264;_tmp345=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp344)->f1;if((int)_tmp345 != 0)goto _LL264;
_LL263: return;_LL264: if(*((int*)_tmp344)!= 0)goto _LL266;_tmp346=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmp344)->f1;if((int)_tmp346 != 1)goto _LL266;_LL265:
return;_LL266:;_LL267: goto _LL261;_LL261:;}((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2D2,_tmp2C9,({const char*_tmp347="(incompatible pointer types)";
_tag_dynforward(_tmp347,sizeof(char),_get_zero_arr_size(_tmp347,29));}));return;
_LL239: _tmp2D5=_tmp291.f1;if(_tmp2D5 <= (void*)4)goto _LL23B;if(*((int*)_tmp2D5)!= 
5)goto _LL23B;_tmp2D6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2D5)->f1;
_tmp2D7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2D5)->f2;_tmp2D8=_tmp291.f2;
if(_tmp2D8 <= (void*)4)goto _LL23B;if(*((int*)_tmp2D8)!= 5)goto _LL23B;_tmp2D9=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2D8)->f1;_tmp2DA=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2D8)->f2;_LL23A: if(_tmp2D9 == _tmp2D6  && ((_tmp2DA == _tmp2D7  || _tmp2DA == (
void*)2  && _tmp2D7 == (void*)3) || _tmp2DA == (void*)3  && _tmp2D7 == (void*)2))
return;Cyc_Tcutil_failure_reason=({const char*_tmp348="(different integral types)";
_tag_dynforward(_tmp348,sizeof(char),_get_zero_arr_size(_tmp348,27));});goto
_LL226;_LL23B: _tmp2DB=_tmp291.f1;if((int)_tmp2DB != 1)goto _LL23D;_tmp2DC=_tmp291.f2;
if((int)_tmp2DC != 1)goto _LL23D;_LL23C: return;_LL23D: _tmp2DD=_tmp291.f1;if(
_tmp2DD <= (void*)4)goto _LL23F;if(*((int*)_tmp2DD)!= 6)goto _LL23F;_tmp2DE=((
struct Cyc_Absyn_DoubleType_struct*)_tmp2DD)->f1;_tmp2DF=_tmp291.f2;if(_tmp2DF <= (
void*)4)goto _LL23F;if(*((int*)_tmp2DF)!= 6)goto _LL23F;_tmp2E0=((struct Cyc_Absyn_DoubleType_struct*)
_tmp2DF)->f1;_LL23E: if(_tmp2DE == _tmp2E0)return;goto _LL226;_LL23F: _tmp2E1=
_tmp291.f1;if(_tmp2E1 <= (void*)4)goto _LL241;if(*((int*)_tmp2E1)!= 14)goto _LL241;
_tmp2E2=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2E1)->f1;_tmp2E3=_tmp291.f2;
if(_tmp2E3 <= (void*)4)goto _LL241;if(*((int*)_tmp2E3)!= 14)goto _LL241;_tmp2E4=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2E3)->f1;_LL240: Cyc_Tcutil_unify_it(
_tmp2E2,_tmp2E4);return;_LL241: _tmp2E5=_tmp291.f1;if(_tmp2E5 <= (void*)4)goto
_LL243;if(*((int*)_tmp2E5)!= 18)goto _LL243;_tmp2E6=((struct Cyc_Absyn_TypeInt_struct*)
_tmp2E5)->f1;_tmp2E7=_tmp291.f2;if(_tmp2E7 <= (void*)4)goto _LL243;if(*((int*)
_tmp2E7)!= 18)goto _LL243;_tmp2E8=((struct Cyc_Absyn_TypeInt_struct*)_tmp2E7)->f1;
_LL242: if(_tmp2E6 == _tmp2E8)return;Cyc_Tcutil_failure_reason=({const char*_tmp349="(different type integers)";
_tag_dynforward(_tmp349,sizeof(char),_get_zero_arr_size(_tmp349,26));});goto
_LL226;_LL243: _tmp2E9=_tmp291.f1;if(_tmp2E9 <= (void*)4)goto _LL245;if(*((int*)
_tmp2E9)!= 17)goto _LL245;_tmp2EA=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp2E9)->f1;_tmp2EB=_tmp291.f2;if(_tmp2EB <= (void*)4)goto _LL245;if(*((int*)
_tmp2EB)!= 17)goto _LL245;_tmp2EC=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp2EB)->f1;_LL244: Cyc_Tcutil_unify_it(_tmp2EA,_tmp2EC);return;_LL245: _tmp2ED=
_tmp291.f1;if(_tmp2ED <= (void*)4)goto _LL247;if(*((int*)_tmp2ED)!= 7)goto _LL247;
_tmp2EE=((struct Cyc_Absyn_ArrayType_struct*)_tmp2ED)->f1;_tmp2EF=(void*)_tmp2EE.elt_type;
_tmp2F0=_tmp2EE.tq;_tmp2F1=_tmp2EE.num_elts;_tmp2F2=_tmp2EE.zero_term;_tmp2F3=
_tmp291.f2;if(_tmp2F3 <= (void*)4)goto _LL247;if(*((int*)_tmp2F3)!= 7)goto _LL247;
_tmp2F4=((struct Cyc_Absyn_ArrayType_struct*)_tmp2F3)->f1;_tmp2F5=(void*)_tmp2F4.elt_type;
_tmp2F6=_tmp2F4.tq;_tmp2F7=_tmp2F4.num_elts;_tmp2F8=_tmp2F4.zero_term;_LL246: Cyc_Tcutil_unify_it(
_tmp2F5,_tmp2EF);Cyc_Tcutil_unify_tqual(_tmp2F6,_tmp2F5,_tmp2F0,_tmp2EF);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2F2,_tmp2F8,({const char*_tmp34A="(not both zero terminated)";
_tag_dynforward(_tmp34A,sizeof(char),_get_zero_arr_size(_tmp34A,27));}));if(
_tmp2F1 == _tmp2F7)return;if(_tmp2F1 == 0  || _tmp2F7 == 0)goto _LL226;if(Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_tmp2F1,(struct Cyc_Absyn_Exp*)_tmp2F7))return;Cyc_Tcutil_failure_reason=({
const char*_tmp34B="(different array sizes)";_tag_dynforward(_tmp34B,sizeof(char),
_get_zero_arr_size(_tmp34B,24));});goto _LL226;_LL247: _tmp2F9=_tmp291.f1;if(
_tmp2F9 <= (void*)4)goto _LL249;if(*((int*)_tmp2F9)!= 8)goto _LL249;_tmp2FA=((
struct Cyc_Absyn_FnType_struct*)_tmp2F9)->f1;_tmp2FB=_tmp2FA.tvars;_tmp2FC=
_tmp2FA.effect;_tmp2FD=(void*)_tmp2FA.ret_typ;_tmp2FE=_tmp2FA.args;_tmp2FF=
_tmp2FA.c_varargs;_tmp300=_tmp2FA.cyc_varargs;_tmp301=_tmp2FA.rgn_po;_tmp302=
_tmp2FA.attributes;_tmp303=_tmp291.f2;if(_tmp303 <= (void*)4)goto _LL249;if(*((int*)
_tmp303)!= 8)goto _LL249;_tmp304=((struct Cyc_Absyn_FnType_struct*)_tmp303)->f1;
_tmp305=_tmp304.tvars;_tmp306=_tmp304.effect;_tmp307=(void*)_tmp304.ret_typ;
_tmp308=_tmp304.args;_tmp309=_tmp304.c_varargs;_tmp30A=_tmp304.cyc_varargs;
_tmp30B=_tmp304.rgn_po;_tmp30C=_tmp304.attributes;_LL248: {int done=0;{struct
_RegionHandle _tmp34C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp34C;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp305 != 0){if(_tmp2FB == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp34D="(second function type has too few type variables)";
_tag_dynforward(_tmp34D,sizeof(char),_get_zero_arr_size(_tmp34D,50));});(int)
_throw((void*)Cyc_Tcutil_Unify);}{void*_tmp34E=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp305->hd);void*_tmp34F=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2FB->hd);
if(_tmp34E != _tmp34F){Cyc_Tcutil_failure_reason=(struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp354;_tmp354.tag=0;_tmp354.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_kind2string(_tmp34F));{struct Cyc_String_pa_struct
_tmp353;_tmp353.tag=0;_tmp353.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_kind2string(_tmp34E));{struct Cyc_String_pa_struct _tmp352;_tmp352.tag=
0;_tmp352.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp305->hd));{void*_tmp350[3]={& _tmp352,& _tmp353,& _tmp354};
Cyc_aprintf(({const char*_tmp351="(type var %s has different kinds %s and %s)";
_tag_dynforward(_tmp351,sizeof(char),_get_zero_arr_size(_tmp351,44));}),
_tag_dynforward(_tmp350,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}
inst=({struct Cyc_List_List*_tmp355=_region_malloc(rgn,sizeof(*_tmp355));_tmp355->hd=({
struct _tuple8*_tmp356=_region_malloc(rgn,sizeof(*_tmp356));_tmp356->f1=(struct
Cyc_Absyn_Tvar*)_tmp2FB->hd;_tmp356->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp357=_cycalloc(sizeof(*_tmp357));_tmp357[0]=({struct Cyc_Absyn_VarType_struct
_tmp358;_tmp358.tag=1;_tmp358.f1=(struct Cyc_Absyn_Tvar*)_tmp305->hd;_tmp358;});
_tmp357;});_tmp356;});_tmp355->tl=inst;_tmp355;});_tmp305=_tmp305->tl;_tmp2FB=
_tmp2FB->tl;}}if(_tmp2FB != 0){Cyc_Tcutil_failure_reason=({const char*_tmp359="(second function type has too many type variables)";
_tag_dynforward(_tmp359,sizeof(char),_get_zero_arr_size(_tmp359,51));});
_npop_handler(0);goto _LL226;}if(inst != 0){Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_struct*
_tmp35A=_cycalloc(sizeof(*_tmp35A));_tmp35A[0]=({struct Cyc_Absyn_FnType_struct
_tmp35B;_tmp35B.tag=8;_tmp35B.f1=({struct Cyc_Absyn_FnInfo _tmp35C;_tmp35C.tvars=0;
_tmp35C.effect=_tmp306;_tmp35C.ret_typ=(void*)_tmp307;_tmp35C.args=_tmp308;
_tmp35C.c_varargs=_tmp309;_tmp35C.cyc_varargs=_tmp30A;_tmp35C.rgn_po=_tmp30B;
_tmp35C.attributes=_tmp30C;_tmp35C;});_tmp35B;});_tmp35A;}),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*_tmp35D=_cycalloc(sizeof(*
_tmp35D));_tmp35D[0]=({struct Cyc_Absyn_FnType_struct _tmp35E;_tmp35E.tag=8;
_tmp35E.f1=({struct Cyc_Absyn_FnInfo _tmp35F;_tmp35F.tvars=0;_tmp35F.effect=
_tmp2FC;_tmp35F.ret_typ=(void*)_tmp2FD;_tmp35F.args=_tmp2FE;_tmp35F.c_varargs=
_tmp2FF;_tmp35F.cyc_varargs=_tmp300;_tmp35F.rgn_po=_tmp301;_tmp35F.attributes=
_tmp302;_tmp35F;});_tmp35E;});_tmp35D;})));done=1;}};_pop_region(rgn);}if(done)
return;Cyc_Tcutil_unify_it(_tmp307,_tmp2FD);for(0;_tmp308 != 0  && _tmp2FE != 0;(
_tmp308=_tmp308->tl,_tmp2FE=_tmp2FE->tl)){struct Cyc_Absyn_Tqual _tmp361;void*
_tmp362;struct _tuple2 _tmp360=*((struct _tuple2*)_tmp308->hd);_tmp361=_tmp360.f2;
_tmp362=_tmp360.f3;{struct Cyc_Absyn_Tqual _tmp364;void*_tmp365;struct _tuple2
_tmp363=*((struct _tuple2*)_tmp2FE->hd);_tmp364=_tmp363.f2;_tmp365=_tmp363.f3;Cyc_Tcutil_unify_it(
_tmp362,_tmp365);Cyc_Tcutil_unify_tqual(_tmp361,_tmp362,_tmp364,_tmp365);}}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp308 != 0  || _tmp2FE != 0){Cyc_Tcutil_failure_reason=({
const char*_tmp366="(function types have different number of arguments)";
_tag_dynforward(_tmp366,sizeof(char),_get_zero_arr_size(_tmp366,52));});goto
_LL226;}if(_tmp309 != _tmp2FF){Cyc_Tcutil_failure_reason=({const char*_tmp367="(only one function type takes C varargs)";
_tag_dynforward(_tmp367,sizeof(char),_get_zero_arr_size(_tmp367,41));});goto
_LL226;}{int bad_cyc_vararg=0;{struct _tuple11 _tmp369=({struct _tuple11 _tmp368;
_tmp368.f1=_tmp30A;_tmp368.f2=_tmp300;_tmp368;});struct Cyc_Absyn_VarargInfo*
_tmp36A;struct Cyc_Absyn_VarargInfo*_tmp36B;struct Cyc_Absyn_VarargInfo*_tmp36C;
struct Cyc_Absyn_VarargInfo*_tmp36D;struct Cyc_Absyn_VarargInfo*_tmp36E;struct Cyc_Absyn_VarargInfo
_tmp36F;struct Cyc_Core_Opt*_tmp370;struct Cyc_Absyn_Tqual _tmp371;void*_tmp372;int
_tmp373;struct Cyc_Absyn_VarargInfo*_tmp374;struct Cyc_Absyn_VarargInfo _tmp375;
struct Cyc_Core_Opt*_tmp376;struct Cyc_Absyn_Tqual _tmp377;void*_tmp378;int _tmp379;
_LL269: _tmp36A=_tmp369.f1;if(_tmp36A != 0)goto _LL26B;_tmp36B=_tmp369.f2;if(
_tmp36B != 0)goto _LL26B;_LL26A: goto _LL268;_LL26B: _tmp36C=_tmp369.f1;if(_tmp36C != 
0)goto _LL26D;_LL26C: goto _LL26E;_LL26D: _tmp36D=_tmp369.f2;if(_tmp36D != 0)goto
_LL26F;_LL26E: bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp37A="(only one function type takes varargs)";
_tag_dynforward(_tmp37A,sizeof(char),_get_zero_arr_size(_tmp37A,39));});goto
_LL268;_LL26F: _tmp36E=_tmp369.f1;if(_tmp36E == 0)goto _LL268;_tmp36F=*_tmp36E;
_tmp370=_tmp36F.name;_tmp371=_tmp36F.tq;_tmp372=(void*)_tmp36F.type;_tmp373=
_tmp36F.inject;_tmp374=_tmp369.f2;if(_tmp374 == 0)goto _LL268;_tmp375=*_tmp374;
_tmp376=_tmp375.name;_tmp377=_tmp375.tq;_tmp378=(void*)_tmp375.type;_tmp379=
_tmp375.inject;_LL270: Cyc_Tcutil_unify_it(_tmp372,_tmp378);Cyc_Tcutil_unify_tqual(
_tmp371,_tmp372,_tmp377,_tmp378);if(_tmp373 != _tmp379){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({
const char*_tmp37B="(only one function type injects varargs)";_tag_dynforward(
_tmp37B,sizeof(char),_get_zero_arr_size(_tmp37B,41));});}goto _LL268;_LL268:;}if(
bad_cyc_vararg)goto _LL226;{int bad_effect=0;{struct _tuple12 _tmp37D=({struct
_tuple12 _tmp37C;_tmp37C.f1=_tmp306;_tmp37C.f2=_tmp2FC;_tmp37C;});struct Cyc_Core_Opt*
_tmp37E;struct Cyc_Core_Opt*_tmp37F;struct Cyc_Core_Opt*_tmp380;struct Cyc_Core_Opt*
_tmp381;_LL272: _tmp37E=_tmp37D.f1;if(_tmp37E != 0)goto _LL274;_tmp37F=_tmp37D.f2;
if(_tmp37F != 0)goto _LL274;_LL273: goto _LL271;_LL274: _tmp380=_tmp37D.f1;if(_tmp380
!= 0)goto _LL276;_LL275: goto _LL277;_LL276: _tmp381=_tmp37D.f2;if(_tmp381 != 0)goto
_LL278;_LL277: bad_effect=1;goto _LL271;_LL278:;_LL279: bad_effect=!Cyc_Tcutil_unify_effect((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp306))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2FC))->v);goto _LL271;_LL271:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=({const char*_tmp382="(function type effects do not unify)";
_tag_dynforward(_tmp382,sizeof(char),_get_zero_arr_size(_tmp382,37));});goto
_LL226;}if(!Cyc_Tcutil_same_atts(_tmp302,_tmp30C)){Cyc_Tcutil_failure_reason=({
const char*_tmp383="(function types have different attributes)";_tag_dynforward(
_tmp383,sizeof(char),_get_zero_arr_size(_tmp383,43));});goto _LL226;}if(!Cyc_Tcutil_same_rgn_po(
_tmp301,_tmp30B)){Cyc_Tcutil_failure_reason=({const char*_tmp384="(function types have different region lifetime orderings)";
_tag_dynforward(_tmp384,sizeof(char),_get_zero_arr_size(_tmp384,58));});goto
_LL226;}return;}}}_LL249: _tmp30D=_tmp291.f1;if(_tmp30D <= (void*)4)goto _LL24B;if(*((
int*)_tmp30D)!= 9)goto _LL24B;_tmp30E=((struct Cyc_Absyn_TupleType_struct*)_tmp30D)->f1;
_tmp30F=_tmp291.f2;if(_tmp30F <= (void*)4)goto _LL24B;if(*((int*)_tmp30F)!= 9)goto
_LL24B;_tmp310=((struct Cyc_Absyn_TupleType_struct*)_tmp30F)->f1;_LL24A: for(0;
_tmp310 != 0  && _tmp30E != 0;(_tmp310=_tmp310->tl,_tmp30E=_tmp30E->tl)){struct Cyc_Absyn_Tqual
_tmp386;void*_tmp387;struct _tuple4 _tmp385=*((struct _tuple4*)_tmp310->hd);_tmp386=
_tmp385.f1;_tmp387=_tmp385.f2;{struct Cyc_Absyn_Tqual _tmp389;void*_tmp38A;struct
_tuple4 _tmp388=*((struct _tuple4*)_tmp30E->hd);_tmp389=_tmp388.f1;_tmp38A=_tmp388.f2;
Cyc_Tcutil_unify_it(_tmp387,_tmp38A);Cyc_Tcutil_unify_tqual(_tmp386,_tmp387,
_tmp389,_tmp38A);}}if(_tmp310 == 0  && _tmp30E == 0)return;Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp38B="(tuple types have different numbers of components)";
_tag_dynforward(_tmp38B,sizeof(char),_get_zero_arr_size(_tmp38B,51));});goto
_LL226;_LL24B: _tmp311=_tmp291.f1;if(_tmp311 <= (void*)4)goto _LL24D;if(*((int*)
_tmp311)!= 11)goto _LL24D;_tmp312=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp311)->f1;_tmp313=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp311)->f2;_tmp314=
_tmp291.f2;if(_tmp314 <= (void*)4)goto _LL24D;if(*((int*)_tmp314)!= 11)goto _LL24D;
_tmp315=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp314)->f1;_tmp316=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp314)->f2;_LL24C: if(_tmp315 != _tmp312){
Cyc_Tcutil_failure_reason=({const char*_tmp38C="(struct and union type)";
_tag_dynforward(_tmp38C,sizeof(char),_get_zero_arr_size(_tmp38C,24));});goto
_LL226;}for(0;_tmp316 != 0  && _tmp313 != 0;(_tmp316=_tmp316->tl,_tmp313=_tmp313->tl)){
struct Cyc_Absyn_Aggrfield*_tmp38D=(struct Cyc_Absyn_Aggrfield*)_tmp316->hd;struct
Cyc_Absyn_Aggrfield*_tmp38E=(struct Cyc_Absyn_Aggrfield*)_tmp313->hd;if(Cyc_strptrcmp(
_tmp38D->name,_tmp38E->name)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp38F="(different member names)";
_tag_dynforward(_tmp38F,sizeof(char),_get_zero_arr_size(_tmp38F,25));});(int)
_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_unify_it((void*)_tmp38D->type,(void*)
_tmp38E->type);Cyc_Tcutil_unify_tqual(_tmp38D->tq,(void*)_tmp38D->type,_tmp38E->tq,(
void*)_tmp38E->type);if(!Cyc_Tcutil_same_atts(_tmp38D->attributes,_tmp38E->attributes)){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({
const char*_tmp390="(different attributes on member)";_tag_dynforward(_tmp390,
sizeof(char),_get_zero_arr_size(_tmp390,33));});(int)_throw((void*)Cyc_Tcutil_Unify);}
if((_tmp38D->width != 0  && _tmp38E->width == 0  || _tmp38E->width != 0  && _tmp38D->width
== 0) || (_tmp38D->width != 0  && _tmp38E->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp38D->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp38E->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp391="(different bitfield widths on member)";
_tag_dynforward(_tmp391,sizeof(char),_get_zero_arr_size(_tmp391,38));});(int)
_throw((void*)Cyc_Tcutil_Unify);}}if(_tmp316 == 0  && _tmp313 == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp392="(different number of members)";
_tag_dynforward(_tmp392,sizeof(char),_get_zero_arr_size(_tmp392,30));});goto
_LL226;_LL24D: _tmp317=_tmp291.f1;if((int)_tmp317 != 2)goto _LL24F;_tmp318=_tmp291.f2;
if((int)_tmp318 != 2)goto _LL24F;_LL24E: return;_LL24F: _tmp319=_tmp291.f1;if((int)
_tmp319 != 3)goto _LL251;_tmp31A=_tmp291.f2;if((int)_tmp31A != 3)goto _LL251;_LL250:
return;_LL251: _tmp31B=_tmp291.f1;if(_tmp31B <= (void*)4)goto _LL253;if(*((int*)
_tmp31B)!= 15)goto _LL253;_tmp31C=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp31B)->f1;_tmp31D=_tmp291.f2;if(_tmp31D <= (void*)4)goto _LL253;if(*((int*)
_tmp31D)!= 15)goto _LL253;_tmp31E=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp31D)->f1;_LL252: Cyc_Tcutil_unify_it(_tmp31C,_tmp31E);return;_LL253: _tmp31F=
_tmp291.f1;if(_tmp31F <= (void*)4)goto _LL255;if(*((int*)_tmp31F)!= 20)goto _LL255;
_LL254: goto _LL256;_LL255: _tmp320=_tmp291.f2;if(_tmp320 <= (void*)4)goto _LL257;if(*((
int*)_tmp320)!= 20)goto _LL257;_LL256: goto _LL258;_LL257: _tmp321=_tmp291.f1;if(
_tmp321 <= (void*)4)goto _LL259;if(*((int*)_tmp321)!= 19)goto _LL259;_LL258: goto
_LL25A;_LL259: _tmp322=_tmp291.f1;if(_tmp322 <= (void*)4)goto _LL25B;if(*((int*)
_tmp322)!= 21)goto _LL25B;_LL25A: goto _LL25C;_LL25B: _tmp323=_tmp291.f2;if(_tmp323
<= (void*)4)goto _LL25D;if(*((int*)_tmp323)!= 21)goto _LL25D;_LL25C: goto _LL25E;
_LL25D: _tmp324=_tmp291.f2;if(_tmp324 <= (void*)4)goto _LL25F;if(*((int*)_tmp324)!= 
19)goto _LL25F;_LL25E: if(Cyc_Tcutil_unify_effect(t1,t2))return;Cyc_Tcutil_failure_reason=({
const char*_tmp393="(effects don't unify)";_tag_dynforward(_tmp393,sizeof(char),
_get_zero_arr_size(_tmp393,22));});goto _LL226;_LL25F:;_LL260: goto _LL226;_LL226:;}(
int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),
void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2
== 0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int
Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int
_tmp394=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int _tmp395=(
tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp394,_tmp395);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(x);y=
Cyc_Absyn_compress_conref(y);if(x == y)return 0;{void*_tmp396=(void*)x->v;void*
_tmp397;_LL27B: if((int)_tmp396 != 0)goto _LL27D;_LL27C: return - 1;_LL27D: if(_tmp396
<= (void*)1)goto _LL27F;if(*((int*)_tmp396)!= 0)goto _LL27F;_tmp397=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp396)->f1;_LL27E: {void*_tmp398=(void*)y->v;
void*_tmp399;_LL282: if((int)_tmp398 != 0)goto _LL284;_LL283: return 1;_LL284: if(
_tmp398 <= (void*)1)goto _LL286;if(*((int*)_tmp398)!= 0)goto _LL286;_tmp399=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp398)->f1;_LL285: return cmp(_tmp397,_tmp399);
_LL286: if(_tmp398 <= (void*)1)goto _LL281;if(*((int*)_tmp398)!= 1)goto _LL281;
_LL287:({void*_tmp39A[0]={};Cyc_Tcutil_impos(({const char*_tmp39B="unify_conref: forward after compress(2)";
_tag_dynforward(_tmp39B,sizeof(char),_get_zero_arr_size(_tmp39B,40));}),
_tag_dynforward(_tmp39A,sizeof(void*),0));});_LL281:;}_LL27F: if(_tmp396 <= (void*)
1)goto _LL27A;if(*((int*)_tmp396)!= 1)goto _LL27A;_LL280:({void*_tmp39C[0]={};Cyc_Tcutil_impos(({
const char*_tmp39D="unify_conref: forward after compress";_tag_dynforward(_tmp39D,
sizeof(char),_get_zero_arr_size(_tmp39D,37));}),_tag_dynforward(_tmp39C,sizeof(
void*),0));});_LL27A:;}}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple4*tqt1,
struct _tuple4*tqt2){struct _tuple4 _tmp39F;struct Cyc_Absyn_Tqual _tmp3A0;void*
_tmp3A1;struct _tuple4*_tmp39E=tqt1;_tmp39F=*_tmp39E;_tmp3A0=_tmp39F.f1;_tmp3A1=
_tmp39F.f2;{struct _tuple4 _tmp3A3;struct Cyc_Absyn_Tqual _tmp3A4;void*_tmp3A5;
struct _tuple4*_tmp3A2=tqt2;_tmp3A3=*_tmp3A2;_tmp3A4=_tmp3A3.f1;_tmp3A5=_tmp3A3.f2;{
int _tmp3A6=Cyc_Tcutil_tqual_cmp(_tmp3A0,_tmp3A4);if(_tmp3A6 != 0)return _tmp3A6;
return Cyc_Tcutil_typecmp(_tmp3A1,_tmp3A5);}}}static int Cyc_Tcutil_aggrfield_cmp(
struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp3A7=Cyc_strptrcmp(
f1->name,f2->name);if(_tmp3A7 != 0)return _tmp3A7;{int _tmp3A8=Cyc_Tcutil_tqual_cmp(
f1->tq,f2->tq);if(_tmp3A8 != 0)return _tmp3A8;{int _tmp3A9=Cyc_Tcutil_typecmp((void*)
f1->type,(void*)f2->type);if(_tmp3A9 != 0)return _tmp3A9;{int _tmp3AA=Cyc_List_list_cmp(
Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);if(_tmp3AA != 0)return
_tmp3AA;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
f1->width,f2->width);}}}}static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*
e1,struct Cyc_Absyn_Enumfield*e2){int _tmp3AB=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp3AB != 0)return _tmp3AB;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp3AC=t;_LL289: if((int)_tmp3AC != 0)goto _LL28B;_LL28A: return 0;
_LL28B: if(_tmp3AC <= (void*)4)goto _LL297;if(*((int*)_tmp3AC)!= 0)goto _LL28D;
_LL28C: return 1;_LL28D: if(*((int*)_tmp3AC)!= 1)goto _LL28F;_LL28E: return 2;_LL28F:
if(*((int*)_tmp3AC)!= 2)goto _LL291;_LL290: return 3;_LL291: if(*((int*)_tmp3AC)!= 3)
goto _LL293;_LL292: return 4;_LL293: if(*((int*)_tmp3AC)!= 4)goto _LL295;_LL294:
return 5;_LL295: if(*((int*)_tmp3AC)!= 5)goto _LL297;_LL296: return 6;_LL297: if((int)
_tmp3AC != 1)goto _LL299;_LL298: return 7;_LL299: if(_tmp3AC <= (void*)4)goto _LL2AD;
if(*((int*)_tmp3AC)!= 6)goto _LL29B;_LL29A: return 8;_LL29B: if(*((int*)_tmp3AC)!= 7)
goto _LL29D;_LL29C: return 9;_LL29D: if(*((int*)_tmp3AC)!= 8)goto _LL29F;_LL29E:
return 10;_LL29F: if(*((int*)_tmp3AC)!= 9)goto _LL2A1;_LL2A0: return 11;_LL2A1: if(*((
int*)_tmp3AC)!= 10)goto _LL2A3;_LL2A2: return 12;_LL2A3: if(*((int*)_tmp3AC)!= 11)
goto _LL2A5;_LL2A4: return 14;_LL2A5: if(*((int*)_tmp3AC)!= 12)goto _LL2A7;_LL2A6:
return 16;_LL2A7: if(*((int*)_tmp3AC)!= 13)goto _LL2A9;_LL2A8: return 17;_LL2A9: if(*((
int*)_tmp3AC)!= 15)goto _LL2AB;_LL2AA: return 18;_LL2AB: if(*((int*)_tmp3AC)!= 16)
goto _LL2AD;_LL2AC: return 19;_LL2AD: if((int)_tmp3AC != 3)goto _LL2AF;_LL2AE: return 20;
_LL2AF: if((int)_tmp3AC != 2)goto _LL2B1;_LL2B0: return 21;_LL2B1: if(_tmp3AC <= (void*)
4)goto _LL2B3;if(*((int*)_tmp3AC)!= 19)goto _LL2B3;_LL2B2: return 22;_LL2B3: if(
_tmp3AC <= (void*)4)goto _LL2B5;if(*((int*)_tmp3AC)!= 20)goto _LL2B5;_LL2B4: return
23;_LL2B5: if(_tmp3AC <= (void*)4)goto _LL2B7;if(*((int*)_tmp3AC)!= 21)goto _LL2B7;
_LL2B6: return 24;_LL2B7: if(_tmp3AC <= (void*)4)goto _LL2B9;if(*((int*)_tmp3AC)!= 14)
goto _LL2B9;_LL2B8: return 25;_LL2B9: if(_tmp3AC <= (void*)4)goto _LL2BB;if(*((int*)
_tmp3AC)!= 18)goto _LL2BB;_LL2BA: return 26;_LL2BB: if(_tmp3AC <= (void*)4)goto _LL288;
if(*((int*)_tmp3AC)!= 17)goto _LL288;_LL2BC: return 27;_LL288:;}int Cyc_Tcutil_typecmp(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == 
t2)return 0;{int _tmp3AD=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp3AD != 0)return _tmp3AD;{struct _tuple6 _tmp3AF=({struct _tuple6 _tmp3AE;
_tmp3AE.f1=t2;_tmp3AE.f2=t1;_tmp3AE;});void*_tmp3B0;void*_tmp3B1;void*_tmp3B2;
struct Cyc_Absyn_Tvar*_tmp3B3;void*_tmp3B4;struct Cyc_Absyn_Tvar*_tmp3B5;void*
_tmp3B6;struct Cyc_Absyn_AggrInfo _tmp3B7;void*_tmp3B8;struct Cyc_List_List*_tmp3B9;
void*_tmp3BA;struct Cyc_Absyn_AggrInfo _tmp3BB;void*_tmp3BC;struct Cyc_List_List*
_tmp3BD;void*_tmp3BE;struct _tuple1*_tmp3BF;void*_tmp3C0;struct _tuple1*_tmp3C1;
void*_tmp3C2;struct Cyc_List_List*_tmp3C3;void*_tmp3C4;struct Cyc_List_List*
_tmp3C5;void*_tmp3C6;struct Cyc_Absyn_TunionInfo _tmp3C7;void*_tmp3C8;struct Cyc_Absyn_Tuniondecl**
_tmp3C9;struct Cyc_Absyn_Tuniondecl*_tmp3CA;struct Cyc_List_List*_tmp3CB;void*
_tmp3CC;void*_tmp3CD;struct Cyc_Absyn_TunionInfo _tmp3CE;void*_tmp3CF;struct Cyc_Absyn_Tuniondecl**
_tmp3D0;struct Cyc_Absyn_Tuniondecl*_tmp3D1;struct Cyc_List_List*_tmp3D2;void*
_tmp3D3;void*_tmp3D4;struct Cyc_Absyn_TunionFieldInfo _tmp3D5;void*_tmp3D6;struct
Cyc_Absyn_Tuniondecl*_tmp3D7;struct Cyc_Absyn_Tunionfield*_tmp3D8;struct Cyc_List_List*
_tmp3D9;void*_tmp3DA;struct Cyc_Absyn_TunionFieldInfo _tmp3DB;void*_tmp3DC;struct
Cyc_Absyn_Tuniondecl*_tmp3DD;struct Cyc_Absyn_Tunionfield*_tmp3DE;struct Cyc_List_List*
_tmp3DF;void*_tmp3E0;struct Cyc_Absyn_PtrInfo _tmp3E1;void*_tmp3E2;struct Cyc_Absyn_Tqual
_tmp3E3;struct Cyc_Absyn_PtrAtts _tmp3E4;void*_tmp3E5;struct Cyc_Absyn_Conref*
_tmp3E6;struct Cyc_Absyn_Conref*_tmp3E7;struct Cyc_Absyn_Conref*_tmp3E8;void*
_tmp3E9;struct Cyc_Absyn_PtrInfo _tmp3EA;void*_tmp3EB;struct Cyc_Absyn_Tqual _tmp3EC;
struct Cyc_Absyn_PtrAtts _tmp3ED;void*_tmp3EE;struct Cyc_Absyn_Conref*_tmp3EF;
struct Cyc_Absyn_Conref*_tmp3F0;struct Cyc_Absyn_Conref*_tmp3F1;void*_tmp3F2;void*
_tmp3F3;void*_tmp3F4;void*_tmp3F5;void*_tmp3F6;void*_tmp3F7;void*_tmp3F8;void*
_tmp3F9;void*_tmp3FA;int _tmp3FB;void*_tmp3FC;int _tmp3FD;void*_tmp3FE;struct Cyc_Absyn_ArrayInfo
_tmp3FF;void*_tmp400;struct Cyc_Absyn_Tqual _tmp401;struct Cyc_Absyn_Exp*_tmp402;
struct Cyc_Absyn_Conref*_tmp403;void*_tmp404;struct Cyc_Absyn_ArrayInfo _tmp405;
void*_tmp406;struct Cyc_Absyn_Tqual _tmp407;struct Cyc_Absyn_Exp*_tmp408;struct Cyc_Absyn_Conref*
_tmp409;void*_tmp40A;struct Cyc_Absyn_FnInfo _tmp40B;struct Cyc_List_List*_tmp40C;
struct Cyc_Core_Opt*_tmp40D;void*_tmp40E;struct Cyc_List_List*_tmp40F;int _tmp410;
struct Cyc_Absyn_VarargInfo*_tmp411;struct Cyc_List_List*_tmp412;struct Cyc_List_List*
_tmp413;void*_tmp414;struct Cyc_Absyn_FnInfo _tmp415;struct Cyc_List_List*_tmp416;
struct Cyc_Core_Opt*_tmp417;void*_tmp418;struct Cyc_List_List*_tmp419;int _tmp41A;
struct Cyc_Absyn_VarargInfo*_tmp41B;struct Cyc_List_List*_tmp41C;struct Cyc_List_List*
_tmp41D;void*_tmp41E;struct Cyc_List_List*_tmp41F;void*_tmp420;struct Cyc_List_List*
_tmp421;void*_tmp422;void*_tmp423;struct Cyc_List_List*_tmp424;void*_tmp425;void*
_tmp426;struct Cyc_List_List*_tmp427;void*_tmp428;void*_tmp429;void*_tmp42A;void*
_tmp42B;void*_tmp42C;void*_tmp42D;void*_tmp42E;void*_tmp42F;void*_tmp430;void*
_tmp431;void*_tmp432;void*_tmp433;void*_tmp434;int _tmp435;void*_tmp436;int
_tmp437;void*_tmp438;void*_tmp439;void*_tmp43A;void*_tmp43B;void*_tmp43C;void*
_tmp43D;_LL2BE: _tmp3B0=_tmp3AF.f1;if(_tmp3B0 <= (void*)4)goto _LL2C0;if(*((int*)
_tmp3B0)!= 0)goto _LL2C0;_tmp3B1=_tmp3AF.f2;if(_tmp3B1 <= (void*)4)goto _LL2C0;if(*((
int*)_tmp3B1)!= 0)goto _LL2C0;_LL2BF:({void*_tmp43E[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp43F="typecmp: can only compare closed types";_tag_dynforward(_tmp43F,sizeof(
char),_get_zero_arr_size(_tmp43F,39));}),_tag_dynforward(_tmp43E,sizeof(void*),0));});
_LL2C0: _tmp3B2=_tmp3AF.f1;if(_tmp3B2 <= (void*)4)goto _LL2C2;if(*((int*)_tmp3B2)!= 
1)goto _LL2C2;_tmp3B3=((struct Cyc_Absyn_VarType_struct*)_tmp3B2)->f1;_tmp3B4=
_tmp3AF.f2;if(_tmp3B4 <= (void*)4)goto _LL2C2;if(*((int*)_tmp3B4)!= 1)goto _LL2C2;
_tmp3B5=((struct Cyc_Absyn_VarType_struct*)_tmp3B4)->f1;_LL2C1: return Cyc_Core_intcmp(*((
int*)_check_null(_tmp3B5->identity)),*((int*)_check_null(_tmp3B3->identity)));
_LL2C2: _tmp3B6=_tmp3AF.f1;if(_tmp3B6 <= (void*)4)goto _LL2C4;if(*((int*)_tmp3B6)!= 
10)goto _LL2C4;_tmp3B7=((struct Cyc_Absyn_AggrType_struct*)_tmp3B6)->f1;_tmp3B8=(
void*)_tmp3B7.aggr_info;_tmp3B9=_tmp3B7.targs;_tmp3BA=_tmp3AF.f2;if(_tmp3BA <= (
void*)4)goto _LL2C4;if(*((int*)_tmp3BA)!= 10)goto _LL2C4;_tmp3BB=((struct Cyc_Absyn_AggrType_struct*)
_tmp3BA)->f1;_tmp3BC=(void*)_tmp3BB.aggr_info;_tmp3BD=_tmp3BB.targs;_LL2C3: {
struct _tuple1*_tmp441;struct _tuple5 _tmp440=Cyc_Absyn_aggr_kinded_name(_tmp3B8);
_tmp441=_tmp440.f2;{struct _tuple1*_tmp443;struct _tuple5 _tmp442=Cyc_Absyn_aggr_kinded_name(
_tmp3BC);_tmp443=_tmp442.f2;{int _tmp444=Cyc_Absyn_qvar_cmp(_tmp441,_tmp443);if(
_tmp444 != 0)return _tmp444;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3B9,_tmp3BD);}}}}_LL2C4: _tmp3BE=_tmp3AF.f1;if(_tmp3BE <= (void*)4)goto _LL2C6;
if(*((int*)_tmp3BE)!= 12)goto _LL2C6;_tmp3BF=((struct Cyc_Absyn_EnumType_struct*)
_tmp3BE)->f1;_tmp3C0=_tmp3AF.f2;if(_tmp3C0 <= (void*)4)goto _LL2C6;if(*((int*)
_tmp3C0)!= 12)goto _LL2C6;_tmp3C1=((struct Cyc_Absyn_EnumType_struct*)_tmp3C0)->f1;
_LL2C5: return Cyc_Absyn_qvar_cmp(_tmp3BF,_tmp3C1);_LL2C6: _tmp3C2=_tmp3AF.f1;if(
_tmp3C2 <= (void*)4)goto _LL2C8;if(*((int*)_tmp3C2)!= 13)goto _LL2C8;_tmp3C3=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp3C2)->f1;_tmp3C4=_tmp3AF.f2;if(_tmp3C4
<= (void*)4)goto _LL2C8;if(*((int*)_tmp3C4)!= 13)goto _LL2C8;_tmp3C5=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3C4)->f1;_LL2C7: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct
Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp3C3,_tmp3C5);_LL2C8: _tmp3C6=_tmp3AF.f1;if(_tmp3C6 <= (
void*)4)goto _LL2CA;if(*((int*)_tmp3C6)!= 2)goto _LL2CA;_tmp3C7=((struct Cyc_Absyn_TunionType_struct*)
_tmp3C6)->f1;_tmp3C8=(void*)_tmp3C7.tunion_info;if(*((int*)_tmp3C8)!= 1)goto
_LL2CA;_tmp3C9=((struct Cyc_Absyn_KnownTunion_struct*)_tmp3C8)->f1;_tmp3CA=*
_tmp3C9;_tmp3CB=_tmp3C7.targs;_tmp3CC=(void*)_tmp3C7.rgn;_tmp3CD=_tmp3AF.f2;if(
_tmp3CD <= (void*)4)goto _LL2CA;if(*((int*)_tmp3CD)!= 2)goto _LL2CA;_tmp3CE=((
struct Cyc_Absyn_TunionType_struct*)_tmp3CD)->f1;_tmp3CF=(void*)_tmp3CE.tunion_info;
if(*((int*)_tmp3CF)!= 1)goto _LL2CA;_tmp3D0=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp3CF)->f1;_tmp3D1=*_tmp3D0;_tmp3D2=_tmp3CE.targs;_tmp3D3=(void*)_tmp3CE.rgn;
_LL2C9: if(_tmp3D1 == _tmp3CA)return 0;{int _tmp445=Cyc_Absyn_qvar_cmp(_tmp3D1->name,
_tmp3CA->name);if(_tmp445 != 0)return _tmp445;{int _tmp446=Cyc_Tcutil_typecmp(
_tmp3D3,_tmp3CC);if(_tmp446 != 0)return _tmp446;return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3D2,_tmp3CB);}}_LL2CA: _tmp3D4=_tmp3AF.f1;if(_tmp3D4 <= (void*)4)goto _LL2CC;
if(*((int*)_tmp3D4)!= 3)goto _LL2CC;_tmp3D5=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3D4)->f1;_tmp3D6=(void*)_tmp3D5.field_info;if(*((int*)_tmp3D6)!= 1)goto
_LL2CC;_tmp3D7=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3D6)->f1;_tmp3D8=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3D6)->f2;_tmp3D9=_tmp3D5.targs;
_tmp3DA=_tmp3AF.f2;if(_tmp3DA <= (void*)4)goto _LL2CC;if(*((int*)_tmp3DA)!= 3)goto
_LL2CC;_tmp3DB=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp3DA)->f1;_tmp3DC=(
void*)_tmp3DB.field_info;if(*((int*)_tmp3DC)!= 1)goto _LL2CC;_tmp3DD=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp3DC)->f1;_tmp3DE=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3DC)->f2;
_tmp3DF=_tmp3DB.targs;_LL2CB: if(_tmp3DD == _tmp3D7)return 0;{int _tmp447=Cyc_Absyn_qvar_cmp(
_tmp3D7->name,_tmp3DD->name);if(_tmp447 != 0)return _tmp447;{int _tmp448=Cyc_Absyn_qvar_cmp(
_tmp3D8->name,_tmp3DE->name);if(_tmp448 != 0)return _tmp448;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp3DF,_tmp3D9);}}_LL2CC: _tmp3E0=_tmp3AF.f1;if(_tmp3E0 <= (
void*)4)goto _LL2CE;if(*((int*)_tmp3E0)!= 4)goto _LL2CE;_tmp3E1=((struct Cyc_Absyn_PointerType_struct*)
_tmp3E0)->f1;_tmp3E2=(void*)_tmp3E1.elt_typ;_tmp3E3=_tmp3E1.elt_tq;_tmp3E4=
_tmp3E1.ptr_atts;_tmp3E5=(void*)_tmp3E4.rgn;_tmp3E6=_tmp3E4.nullable;_tmp3E7=
_tmp3E4.bounds;_tmp3E8=_tmp3E4.zero_term;_tmp3E9=_tmp3AF.f2;if(_tmp3E9 <= (void*)
4)goto _LL2CE;if(*((int*)_tmp3E9)!= 4)goto _LL2CE;_tmp3EA=((struct Cyc_Absyn_PointerType_struct*)
_tmp3E9)->f1;_tmp3EB=(void*)_tmp3EA.elt_typ;_tmp3EC=_tmp3EA.elt_tq;_tmp3ED=
_tmp3EA.ptr_atts;_tmp3EE=(void*)_tmp3ED.rgn;_tmp3EF=_tmp3ED.nullable;_tmp3F0=
_tmp3ED.bounds;_tmp3F1=_tmp3ED.zero_term;_LL2CD: {int _tmp449=Cyc_Tcutil_typecmp(
_tmp3EB,_tmp3E2);if(_tmp449 != 0)return _tmp449;{int _tmp44A=Cyc_Tcutil_typecmp(
_tmp3EE,_tmp3E5);if(_tmp44A != 0)return _tmp44A;{int _tmp44B=Cyc_Tcutil_tqual_cmp(
_tmp3EC,_tmp3E3);if(_tmp44B != 0)return _tmp44B;{int _tmp44C=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp3F0,_tmp3E7);if(_tmp44C != 0)return _tmp44C;{int _tmp44D=((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3F1,_tmp3E8);if(_tmp44D != 0)return _tmp44D;{void*_tmp44E=(
void*)(Cyc_Absyn_compress_conref(_tmp3F0))->v;void*_tmp44F;void*_tmp450;_LL2F3:
if(_tmp44E <= (void*)1)goto _LL2F7;if(*((int*)_tmp44E)!= 0)goto _LL2F5;_tmp44F=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp44E)->f1;if((int)_tmp44F != 0)goto
_LL2F5;_LL2F4: return 0;_LL2F5: if(*((int*)_tmp44E)!= 0)goto _LL2F7;_tmp450=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp44E)->f1;if((int)_tmp450 != 1)goto _LL2F7;
_LL2F6: return 0;_LL2F7:;_LL2F8: goto _LL2F2;_LL2F2:;}return((int(*)(int(*cmp)(int,
int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3EF,_tmp3E6);}}}}}_LL2CE: _tmp3F2=_tmp3AF.f1;if(_tmp3F2 <= (
void*)4)goto _LL2D0;if(*((int*)_tmp3F2)!= 5)goto _LL2D0;_tmp3F3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3F2)->f1;_tmp3F4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3F2)->f2;
_tmp3F5=_tmp3AF.f2;if(_tmp3F5 <= (void*)4)goto _LL2D0;if(*((int*)_tmp3F5)!= 5)goto
_LL2D0;_tmp3F6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3F5)->f1;_tmp3F7=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp3F5)->f2;_LL2CF: if(_tmp3F6 != _tmp3F3)
return Cyc_Core_intcmp((int)((unsigned int)_tmp3F6),(int)((unsigned int)_tmp3F3));
if(_tmp3F7 != _tmp3F4)return Cyc_Core_intcmp((int)((unsigned int)_tmp3F7),(int)((
unsigned int)_tmp3F4));return 0;_LL2D0: _tmp3F8=_tmp3AF.f1;if((int)_tmp3F8 != 1)
goto _LL2D2;_tmp3F9=_tmp3AF.f2;if((int)_tmp3F9 != 1)goto _LL2D2;_LL2D1: return 0;
_LL2D2: _tmp3FA=_tmp3AF.f1;if(_tmp3FA <= (void*)4)goto _LL2D4;if(*((int*)_tmp3FA)!= 
6)goto _LL2D4;_tmp3FB=((struct Cyc_Absyn_DoubleType_struct*)_tmp3FA)->f1;_tmp3FC=
_tmp3AF.f2;if(_tmp3FC <= (void*)4)goto _LL2D4;if(*((int*)_tmp3FC)!= 6)goto _LL2D4;
_tmp3FD=((struct Cyc_Absyn_DoubleType_struct*)_tmp3FC)->f1;_LL2D3: if(_tmp3FB == 
_tmp3FD)return 0;else{if(_tmp3FB)return - 1;else{return 1;}}_LL2D4: _tmp3FE=_tmp3AF.f1;
if(_tmp3FE <= (void*)4)goto _LL2D6;if(*((int*)_tmp3FE)!= 7)goto _LL2D6;_tmp3FF=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3FE)->f1;_tmp400=(void*)_tmp3FF.elt_type;
_tmp401=_tmp3FF.tq;_tmp402=_tmp3FF.num_elts;_tmp403=_tmp3FF.zero_term;_tmp404=
_tmp3AF.f2;if(_tmp404 <= (void*)4)goto _LL2D6;if(*((int*)_tmp404)!= 7)goto _LL2D6;
_tmp405=((struct Cyc_Absyn_ArrayType_struct*)_tmp404)->f1;_tmp406=(void*)_tmp405.elt_type;
_tmp407=_tmp405.tq;_tmp408=_tmp405.num_elts;_tmp409=_tmp405.zero_term;_LL2D5: {
int _tmp451=Cyc_Tcutil_tqual_cmp(_tmp407,_tmp401);if(_tmp451 != 0)return _tmp451;{
int _tmp452=Cyc_Tcutil_typecmp(_tmp406,_tmp400);if(_tmp452 != 0)return _tmp452;{int
_tmp453=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp403,_tmp409);if(_tmp453 != 0)
return _tmp453;if(_tmp402 == _tmp408)return 0;if(_tmp402 == 0  || _tmp408 == 0)({void*
_tmp454[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp455="missing expression in array index";_tag_dynforward(_tmp455,
sizeof(char),_get_zero_arr_size(_tmp455,34));}),_tag_dynforward(_tmp454,sizeof(
void*),0));});return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp402,_tmp408);}}}_LL2D6: _tmp40A=_tmp3AF.f1;if(_tmp40A <= (void*)4)goto _LL2D8;
if(*((int*)_tmp40A)!= 8)goto _LL2D8;_tmp40B=((struct Cyc_Absyn_FnType_struct*)
_tmp40A)->f1;_tmp40C=_tmp40B.tvars;_tmp40D=_tmp40B.effect;_tmp40E=(void*)_tmp40B.ret_typ;
_tmp40F=_tmp40B.args;_tmp410=_tmp40B.c_varargs;_tmp411=_tmp40B.cyc_varargs;
_tmp412=_tmp40B.rgn_po;_tmp413=_tmp40B.attributes;_tmp414=_tmp3AF.f2;if(_tmp414
<= (void*)4)goto _LL2D8;if(*((int*)_tmp414)!= 8)goto _LL2D8;_tmp415=((struct Cyc_Absyn_FnType_struct*)
_tmp414)->f1;_tmp416=_tmp415.tvars;_tmp417=_tmp415.effect;_tmp418=(void*)_tmp415.ret_typ;
_tmp419=_tmp415.args;_tmp41A=_tmp415.c_varargs;_tmp41B=_tmp415.cyc_varargs;
_tmp41C=_tmp415.rgn_po;_tmp41D=_tmp415.attributes;_LL2D7:({void*_tmp456[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp457="typecmp: function types not handled";_tag_dynforward(_tmp457,
sizeof(char),_get_zero_arr_size(_tmp457,36));}),_tag_dynforward(_tmp456,sizeof(
void*),0));});_LL2D8: _tmp41E=_tmp3AF.f1;if(_tmp41E <= (void*)4)goto _LL2DA;if(*((
int*)_tmp41E)!= 9)goto _LL2DA;_tmp41F=((struct Cyc_Absyn_TupleType_struct*)_tmp41E)->f1;
_tmp420=_tmp3AF.f2;if(_tmp420 <= (void*)4)goto _LL2DA;if(*((int*)_tmp420)!= 9)goto
_LL2DA;_tmp421=((struct Cyc_Absyn_TupleType_struct*)_tmp420)->f1;_LL2D9: return((
int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp421,_tmp41F);_LL2DA: _tmp422=
_tmp3AF.f1;if(_tmp422 <= (void*)4)goto _LL2DC;if(*((int*)_tmp422)!= 11)goto _LL2DC;
_tmp423=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp422)->f1;_tmp424=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp422)->f2;_tmp425=_tmp3AF.f2;if(_tmp425
<= (void*)4)goto _LL2DC;if(*((int*)_tmp425)!= 11)goto _LL2DC;_tmp426=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp425)->f1;_tmp427=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp425)->f2;_LL2DB: if(_tmp426 != _tmp423){if(_tmp426 == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp427,_tmp424);_LL2DC: _tmp428=_tmp3AF.f1;if(_tmp428 <= (void*)4)goto _LL2DE;if(*((
int*)_tmp428)!= 15)goto _LL2DE;_tmp429=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp428)->f1;_tmp42A=_tmp3AF.f2;if(_tmp42A <= (void*)4)goto _LL2DE;if(*((int*)
_tmp42A)!= 15)goto _LL2DE;_tmp42B=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp42A)->f1;_LL2DD: return Cyc_Tcutil_typecmp(_tmp429,_tmp42B);_LL2DE: _tmp42C=
_tmp3AF.f1;if(_tmp42C <= (void*)4)goto _LL2E0;if(*((int*)_tmp42C)!= 14)goto _LL2E0;
_tmp42D=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp42C)->f1;_tmp42E=_tmp3AF.f2;
if(_tmp42E <= (void*)4)goto _LL2E0;if(*((int*)_tmp42E)!= 14)goto _LL2E0;_tmp42F=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp42E)->f1;_LL2DF: return Cyc_Tcutil_typecmp(
_tmp42D,_tmp42F);_LL2E0: _tmp430=_tmp3AF.f1;if(_tmp430 <= (void*)4)goto _LL2E2;if(*((
int*)_tmp430)!= 17)goto _LL2E2;_tmp431=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp430)->f1;_tmp432=_tmp3AF.f2;if(_tmp432 <= (void*)4)goto _LL2E2;if(*((int*)
_tmp432)!= 17)goto _LL2E2;_tmp433=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp432)->f1;_LL2E1: return Cyc_Tcutil_typecmp(_tmp431,_tmp433);_LL2E2: _tmp434=
_tmp3AF.f1;if(_tmp434 <= (void*)4)goto _LL2E4;if(*((int*)_tmp434)!= 18)goto _LL2E4;
_tmp435=((struct Cyc_Absyn_TypeInt_struct*)_tmp434)->f1;_tmp436=_tmp3AF.f2;if(
_tmp436 <= (void*)4)goto _LL2E4;if(*((int*)_tmp436)!= 18)goto _LL2E4;_tmp437=((
struct Cyc_Absyn_TypeInt_struct*)_tmp436)->f1;_LL2E3: return Cyc_Core_intcmp(
_tmp435,_tmp437);_LL2E4: _tmp438=_tmp3AF.f1;if(_tmp438 <= (void*)4)goto _LL2E6;if(*((
int*)_tmp438)!= 20)goto _LL2E6;_LL2E5: goto _LL2E7;_LL2E6: _tmp439=_tmp3AF.f2;if(
_tmp439 <= (void*)4)goto _LL2E8;if(*((int*)_tmp439)!= 20)goto _LL2E8;_LL2E7: goto
_LL2E9;_LL2E8: _tmp43A=_tmp3AF.f1;if(_tmp43A <= (void*)4)goto _LL2EA;if(*((int*)
_tmp43A)!= 19)goto _LL2EA;_LL2E9: goto _LL2EB;_LL2EA: _tmp43B=_tmp3AF.f1;if(_tmp43B
<= (void*)4)goto _LL2EC;if(*((int*)_tmp43B)!= 21)goto _LL2EC;_LL2EB: goto _LL2ED;
_LL2EC: _tmp43C=_tmp3AF.f2;if(_tmp43C <= (void*)4)goto _LL2EE;if(*((int*)_tmp43C)!= 
21)goto _LL2EE;_LL2ED: goto _LL2EF;_LL2EE: _tmp43D=_tmp3AF.f2;if(_tmp43D <= (void*)4)
goto _LL2F0;if(*((int*)_tmp43D)!= 19)goto _LL2F0;_LL2EF:({void*_tmp458[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp459="typecmp: effects not handled";_tag_dynforward(_tmp459,sizeof(char),
_get_zero_arr_size(_tmp459,29));}),_tag_dynforward(_tmp458,sizeof(void*),0));});
_LL2F0:;_LL2F1:({void*_tmp45A[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp45B="Unmatched case in typecmp";
_tag_dynforward(_tmp45B,sizeof(char),_get_zero_arr_size(_tmp45B,26));}),
_tag_dynforward(_tmp45A,sizeof(void*),0));});_LL2BD:;}}}int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp45C=Cyc_Tcutil_compress(t);_LL2FA: if(_tmp45C <= (void*)4)goto
_LL2FC;if(*((int*)_tmp45C)!= 5)goto _LL2FC;_LL2FB: goto _LL2FD;_LL2FC: if((int)
_tmp45C != 1)goto _LL2FE;_LL2FD: goto _LL2FF;_LL2FE: if(_tmp45C <= (void*)4)goto _LL304;
if(*((int*)_tmp45C)!= 6)goto _LL300;_LL2FF: goto _LL301;_LL300: if(*((int*)_tmp45C)
!= 12)goto _LL302;_LL301: goto _LL303;_LL302: if(*((int*)_tmp45C)!= 13)goto _LL304;
_LL303: return 1;_LL304:;_LL305: return 0;_LL2F9:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple6 _tmp45E=({struct _tuple6 _tmp45D;_tmp45D.f1=t1;_tmp45D.f2=t2;_tmp45D;});
void*_tmp45F;int _tmp460;void*_tmp461;int _tmp462;void*_tmp463;void*_tmp464;void*
_tmp465;void*_tmp466;void*_tmp467;void*_tmp468;void*_tmp469;void*_tmp46A;void*
_tmp46B;void*_tmp46C;void*_tmp46D;void*_tmp46E;void*_tmp46F;void*_tmp470;void*
_tmp471;void*_tmp472;void*_tmp473;void*_tmp474;void*_tmp475;void*_tmp476;void*
_tmp477;void*_tmp478;void*_tmp479;void*_tmp47A;void*_tmp47B;void*_tmp47C;void*
_tmp47D;void*_tmp47E;void*_tmp47F;void*_tmp480;void*_tmp481;void*_tmp482;void*
_tmp483;void*_tmp484;void*_tmp485;void*_tmp486;void*_tmp487;void*_tmp488;void*
_tmp489;void*_tmp48A;void*_tmp48B;void*_tmp48C;void*_tmp48D;void*_tmp48E;void*
_tmp48F;void*_tmp490;void*_tmp491;void*_tmp492;void*_tmp493;void*_tmp494;void*
_tmp495;void*_tmp496;void*_tmp497;void*_tmp498;void*_tmp499;void*_tmp49A;void*
_tmp49B;void*_tmp49C;void*_tmp49D;void*_tmp49E;void*_tmp49F;void*_tmp4A0;void*
_tmp4A1;void*_tmp4A2;void*_tmp4A3;void*_tmp4A4;_LL307: _tmp45F=_tmp45E.f1;if(
_tmp45F <= (void*)4)goto _LL309;if(*((int*)_tmp45F)!= 6)goto _LL309;_tmp460=((
struct Cyc_Absyn_DoubleType_struct*)_tmp45F)->f1;_tmp461=_tmp45E.f2;if(_tmp461 <= (
void*)4)goto _LL309;if(*((int*)_tmp461)!= 6)goto _LL309;_tmp462=((struct Cyc_Absyn_DoubleType_struct*)
_tmp461)->f1;_LL308: return !_tmp462  && _tmp460;_LL309: _tmp463=_tmp45E.f1;if(
_tmp463 <= (void*)4)goto _LL30B;if(*((int*)_tmp463)!= 6)goto _LL30B;_tmp464=_tmp45E.f2;
if((int)_tmp464 != 1)goto _LL30B;_LL30A: goto _LL30C;_LL30B: _tmp465=_tmp45E.f1;if(
_tmp465 <= (void*)4)goto _LL30D;if(*((int*)_tmp465)!= 6)goto _LL30D;_tmp466=_tmp45E.f2;
if(_tmp466 <= (void*)4)goto _LL30D;if(*((int*)_tmp466)!= 5)goto _LL30D;_LL30C: goto
_LL30E;_LL30D: _tmp467=_tmp45E.f1;if(_tmp467 <= (void*)4)goto _LL30F;if(*((int*)
_tmp467)!= 6)goto _LL30F;_tmp468=_tmp45E.f2;if(_tmp468 <= (void*)4)goto _LL30F;if(*((
int*)_tmp468)!= 14)goto _LL30F;_LL30E: goto _LL310;_LL30F: _tmp469=_tmp45E.f1;if((
int)_tmp469 != 1)goto _LL311;_tmp46A=_tmp45E.f2;if(_tmp46A <= (void*)4)goto _LL311;
if(*((int*)_tmp46A)!= 14)goto _LL311;_LL310: goto _LL312;_LL311: _tmp46B=_tmp45E.f1;
if(_tmp46B <= (void*)4)goto _LL313;if(*((int*)_tmp46B)!= 6)goto _LL313;_tmp46C=
_tmp45E.f2;if(_tmp46C <= (void*)4)goto _LL313;if(*((int*)_tmp46C)!= 17)goto _LL313;
_LL312: goto _LL314;_LL313: _tmp46D=_tmp45E.f1;if((int)_tmp46D != 1)goto _LL315;
_tmp46E=_tmp45E.f2;if(_tmp46E <= (void*)4)goto _LL315;if(*((int*)_tmp46E)!= 17)
goto _LL315;_LL314: goto _LL316;_LL315: _tmp46F=_tmp45E.f1;if((int)_tmp46F != 1)goto
_LL317;_tmp470=_tmp45E.f2;if(_tmp470 <= (void*)4)goto _LL317;if(*((int*)_tmp470)!= 
5)goto _LL317;_LL316: return 1;_LL317: _tmp471=_tmp45E.f1;if(_tmp471 <= (void*)4)goto
_LL319;if(*((int*)_tmp471)!= 5)goto _LL319;_tmp472=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp471)->f2;if((int)_tmp472 != 4)goto _LL319;_tmp473=_tmp45E.f2;if(_tmp473 <= (
void*)4)goto _LL319;if(*((int*)_tmp473)!= 5)goto _LL319;_tmp474=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp473)->f2;if((int)_tmp474 != 4)goto _LL319;_LL318: return 0;_LL319: _tmp475=
_tmp45E.f1;if(_tmp475 <= (void*)4)goto _LL31B;if(*((int*)_tmp475)!= 5)goto _LL31B;
_tmp476=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp475)->f2;if((int)_tmp476 != 
4)goto _LL31B;_LL31A: return 1;_LL31B: _tmp477=_tmp45E.f1;if(_tmp477 <= (void*)4)goto
_LL31D;if(*((int*)_tmp477)!= 5)goto _LL31D;_tmp478=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp477)->f2;if((int)_tmp478 != 3)goto _LL31D;_tmp479=_tmp45E.f2;if(_tmp479 <= (
void*)4)goto _LL31D;if(*((int*)_tmp479)!= 5)goto _LL31D;_tmp47A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp479)->f2;if((int)_tmp47A != 2)goto _LL31D;_LL31C: goto _LL31E;_LL31D: _tmp47B=
_tmp45E.f1;if(_tmp47B <= (void*)4)goto _LL31F;if(*((int*)_tmp47B)!= 5)goto _LL31F;
_tmp47C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp47B)->f2;if((int)_tmp47C != 
2)goto _LL31F;_tmp47D=_tmp45E.f2;if(_tmp47D <= (void*)4)goto _LL31F;if(*((int*)
_tmp47D)!= 5)goto _LL31F;_tmp47E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp47D)->f2;
if((int)_tmp47E != 3)goto _LL31F;_LL31E: return 0;_LL31F: _tmp47F=_tmp45E.f1;if(
_tmp47F <= (void*)4)goto _LL321;if(*((int*)_tmp47F)!= 5)goto _LL321;_tmp480=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp47F)->f2;if((int)_tmp480 != 3)goto _LL321;
_tmp481=_tmp45E.f2;if((int)_tmp481 != 1)goto _LL321;_LL320: goto _LL322;_LL321:
_tmp482=_tmp45E.f1;if(_tmp482 <= (void*)4)goto _LL323;if(*((int*)_tmp482)!= 5)goto
_LL323;_tmp483=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp482)->f2;if((int)
_tmp483 != 2)goto _LL323;_tmp484=_tmp45E.f2;if((int)_tmp484 != 1)goto _LL323;_LL322:
goto _LL324;_LL323: _tmp485=_tmp45E.f1;if(_tmp485 <= (void*)4)goto _LL325;if(*((int*)
_tmp485)!= 5)goto _LL325;_tmp486=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp485)->f2;
if((int)_tmp486 != 3)goto _LL325;_tmp487=_tmp45E.f2;if(_tmp487 <= (void*)4)goto
_LL325;if(*((int*)_tmp487)!= 5)goto _LL325;_tmp488=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp487)->f2;if((int)_tmp488 != 1)goto _LL325;_LL324: goto _LL326;_LL325: _tmp489=
_tmp45E.f1;if(_tmp489 <= (void*)4)goto _LL327;if(*((int*)_tmp489)!= 5)goto _LL327;
_tmp48A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp489)->f2;if((int)_tmp48A != 
2)goto _LL327;_tmp48B=_tmp45E.f2;if(_tmp48B <= (void*)4)goto _LL327;if(*((int*)
_tmp48B)!= 5)goto _LL327;_tmp48C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48B)->f2;
if((int)_tmp48C != 1)goto _LL327;_LL326: goto _LL328;_LL327: _tmp48D=_tmp45E.f1;if(
_tmp48D <= (void*)4)goto _LL329;if(*((int*)_tmp48D)!= 17)goto _LL329;_tmp48E=
_tmp45E.f2;if(_tmp48E <= (void*)4)goto _LL329;if(*((int*)_tmp48E)!= 5)goto _LL329;
_tmp48F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48E)->f2;if((int)_tmp48F != 
1)goto _LL329;_LL328: goto _LL32A;_LL329: _tmp490=_tmp45E.f1;if(_tmp490 <= (void*)4)
goto _LL32B;if(*((int*)_tmp490)!= 14)goto _LL32B;_tmp491=_tmp45E.f2;if(_tmp491 <= (
void*)4)goto _LL32B;if(*((int*)_tmp491)!= 5)goto _LL32B;_tmp492=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp491)->f2;if((int)_tmp492 != 1)goto _LL32B;_LL32A: goto _LL32C;_LL32B: _tmp493=
_tmp45E.f1;if(_tmp493 <= (void*)4)goto _LL32D;if(*((int*)_tmp493)!= 5)goto _LL32D;
_tmp494=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp493)->f2;if((int)_tmp494 != 
3)goto _LL32D;_tmp495=_tmp45E.f2;if(_tmp495 <= (void*)4)goto _LL32D;if(*((int*)
_tmp495)!= 5)goto _LL32D;_tmp496=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp495)->f2;
if((int)_tmp496 != 0)goto _LL32D;_LL32C: goto _LL32E;_LL32D: _tmp497=_tmp45E.f1;if(
_tmp497 <= (void*)4)goto _LL32F;if(*((int*)_tmp497)!= 5)goto _LL32F;_tmp498=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp497)->f2;if((int)_tmp498 != 2)goto _LL32F;
_tmp499=_tmp45E.f2;if(_tmp499 <= (void*)4)goto _LL32F;if(*((int*)_tmp499)!= 5)goto
_LL32F;_tmp49A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp499)->f2;if((int)
_tmp49A != 0)goto _LL32F;_LL32E: goto _LL330;_LL32F: _tmp49B=_tmp45E.f1;if(_tmp49B <= (
void*)4)goto _LL331;if(*((int*)_tmp49B)!= 5)goto _LL331;_tmp49C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49B)->f2;if((int)_tmp49C != 1)goto _LL331;_tmp49D=_tmp45E.f2;if(_tmp49D <= (
void*)4)goto _LL331;if(*((int*)_tmp49D)!= 5)goto _LL331;_tmp49E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49D)->f2;if((int)_tmp49E != 0)goto _LL331;_LL330: goto _LL332;_LL331: _tmp49F=
_tmp45E.f1;if(_tmp49F <= (void*)4)goto _LL333;if(*((int*)_tmp49F)!= 17)goto _LL333;
_tmp4A0=_tmp45E.f2;if(_tmp4A0 <= (void*)4)goto _LL333;if(*((int*)_tmp4A0)!= 5)goto
_LL333;_tmp4A1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A0)->f2;if((int)
_tmp4A1 != 0)goto _LL333;_LL332: goto _LL334;_LL333: _tmp4A2=_tmp45E.f1;if(_tmp4A2 <= (
void*)4)goto _LL335;if(*((int*)_tmp4A2)!= 14)goto _LL335;_tmp4A3=_tmp45E.f2;if(
_tmp4A3 <= (void*)4)goto _LL335;if(*((int*)_tmp4A3)!= 5)goto _LL335;_tmp4A4=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp4A3)->f2;if((int)_tmp4A4 != 0)goto _LL335;
_LL334: return 1;_LL335:;_LL336: return 0;_LL306:;}}int Cyc_Tcutil_coerce_list(struct
Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct Cyc_Core_Opt*
max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);
if(Cyc_Tcutil_is_arithmetic_type(t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(
t1,(void*)max_arith_type->v))max_arith_type=({struct Cyc_Core_Opt*_tmp4A5=
_cycalloc(sizeof(*_tmp4A5));_tmp4A5->v=(void*)t1;_tmp4A5;});}}}if(max_arith_type
!= 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)
el->hd,t)){({struct Cyc_String_pa_struct _tmp4A9;_tmp4A9.tag=0;_tmp4A9.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v));{struct Cyc_String_pa_struct
_tmp4A8;_tmp4A8.tag=0;_tmp4A8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp4A6[2]={& _tmp4A8,& _tmp4A9};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)el->hd)->loc,({const char*_tmp4A7="type mismatch: expecting %s but found %s";
_tag_dynforward(_tmp4A7,sizeof(char),_get_zero_arr_size(_tmp4A7,41));}),
_tag_dynforward(_tmp4A6,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp4AA=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL338: if(_tmp4AA <= (void*)4)goto _LL33A;if(*((int*)_tmp4AA)!= 4)goto _LL33A;
_LL339: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(void*)3);goto _LL337;
_LL33A:;_LL33B: return 0;_LL337:;}return 1;}int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp4AB=Cyc_Tcutil_compress(t);_LL33D: if(_tmp4AB <= (void*)4)goto _LL347;if(*((
int*)_tmp4AB)!= 5)goto _LL33F;_LL33E: goto _LL340;_LL33F: if(*((int*)_tmp4AB)!= 14)
goto _LL341;_LL340: goto _LL342;_LL341: if(*((int*)_tmp4AB)!= 17)goto _LL343;_LL342:
goto _LL344;_LL343: if(*((int*)_tmp4AB)!= 12)goto _LL345;_LL344: goto _LL346;_LL345:
if(*((int*)_tmp4AB)!= 13)goto _LL347;_LL346: return 1;_LL347:;_LL348: return 0;_LL33C:;}
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))
return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*_tmp4AC[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp4AD="integral size mismatch; conversion supplied";
_tag_dynforward(_tmp4AD,sizeof(char),_get_zero_arr_size(_tmp4AD,44));}),
_tag_dynforward(_tmp4AC,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*
_tmp4AE[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp4AF="integral size mismatch; conversion supplied";
_tag_dynforward(_tmp4AF,sizeof(char),_get_zero_arr_size(_tmp4AF,44));}),
_tag_dynforward(_tmp4AE,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple6 _tmp4B1=({struct _tuple6 _tmp4B0;_tmp4B0.f1=
t1;_tmp4B0.f2=t2;_tmp4B0;});void*_tmp4B2;struct Cyc_Absyn_PtrInfo _tmp4B3;void*
_tmp4B4;struct Cyc_Absyn_PtrInfo _tmp4B5;void*_tmp4B6;struct Cyc_Absyn_TunionInfo
_tmp4B7;void*_tmp4B8;struct Cyc_Absyn_Tuniondecl**_tmp4B9;struct Cyc_Absyn_Tuniondecl*
_tmp4BA;struct Cyc_List_List*_tmp4BB;void*_tmp4BC;void*_tmp4BD;struct Cyc_Absyn_TunionInfo
_tmp4BE;void*_tmp4BF;struct Cyc_Absyn_Tuniondecl**_tmp4C0;struct Cyc_Absyn_Tuniondecl*
_tmp4C1;struct Cyc_List_List*_tmp4C2;void*_tmp4C3;void*_tmp4C4;struct Cyc_Absyn_ArrayInfo
_tmp4C5;void*_tmp4C6;struct Cyc_Absyn_Tqual _tmp4C7;struct Cyc_Absyn_Exp*_tmp4C8;
struct Cyc_Absyn_Conref*_tmp4C9;void*_tmp4CA;struct Cyc_Absyn_ArrayInfo _tmp4CB;
void*_tmp4CC;struct Cyc_Absyn_Tqual _tmp4CD;struct Cyc_Absyn_Exp*_tmp4CE;struct Cyc_Absyn_Conref*
_tmp4CF;void*_tmp4D0;struct Cyc_Absyn_TunionFieldInfo _tmp4D1;void*_tmp4D2;struct
Cyc_Absyn_Tuniondecl*_tmp4D3;struct Cyc_Absyn_Tunionfield*_tmp4D4;struct Cyc_List_List*
_tmp4D5;void*_tmp4D6;struct Cyc_Absyn_TunionInfo _tmp4D7;void*_tmp4D8;struct Cyc_Absyn_Tuniondecl**
_tmp4D9;struct Cyc_Absyn_Tuniondecl*_tmp4DA;struct Cyc_List_List*_tmp4DB;void*
_tmp4DC;struct Cyc_Absyn_PtrInfo _tmp4DD;void*_tmp4DE;struct Cyc_Absyn_Tqual _tmp4DF;
struct Cyc_Absyn_PtrAtts _tmp4E0;void*_tmp4E1;struct Cyc_Absyn_Conref*_tmp4E2;
struct Cyc_Absyn_Conref*_tmp4E3;struct Cyc_Absyn_Conref*_tmp4E4;void*_tmp4E5;
struct Cyc_Absyn_TunionInfo _tmp4E6;void*_tmp4E7;struct Cyc_Absyn_Tuniondecl**
_tmp4E8;struct Cyc_Absyn_Tuniondecl*_tmp4E9;struct Cyc_List_List*_tmp4EA;void*
_tmp4EB;void*_tmp4EC;void*_tmp4ED;void*_tmp4EE;void*_tmp4EF;void*_tmp4F0;void*
_tmp4F1;void*_tmp4F2;void*_tmp4F3;_LL34A: _tmp4B2=_tmp4B1.f1;if(_tmp4B2 <= (void*)
4)goto _LL34C;if(*((int*)_tmp4B2)!= 4)goto _LL34C;_tmp4B3=((struct Cyc_Absyn_PointerType_struct*)
_tmp4B2)->f1;_tmp4B4=_tmp4B1.f2;if(_tmp4B4 <= (void*)4)goto _LL34C;if(*((int*)
_tmp4B4)!= 4)goto _LL34C;_tmp4B5=((struct Cyc_Absyn_PointerType_struct*)_tmp4B4)->f1;
_LL34B: {int okay=1;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp4B3.ptr_atts).nullable,(
_tmp4B5.ptr_atts).nullable)){void*_tmp4F4=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)((_tmp4B3.ptr_atts).nullable))->v;
int _tmp4F5;_LL35D: if(_tmp4F4 <= (void*)1)goto _LL35F;if(*((int*)_tmp4F4)!= 0)goto
_LL35F;_tmp4F5=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp4F4)->f1;_LL35E: okay=
!_tmp4F5;goto _LL35C;_LL35F:;_LL360:({void*_tmp4F6[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp4F7="silent_castable conref not eq";_tag_dynforward(_tmp4F7,sizeof(char),
_get_zero_arr_size(_tmp4F7,30));}),_tag_dynforward(_tmp4F6,sizeof(void*),0));});
_LL35C:;}if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,(_tmp4B3.ptr_atts).bounds,(
_tmp4B5.ptr_atts).bounds)){struct _tuple6 _tmp4F9=({struct _tuple6 _tmp4F8;_tmp4F8.f1=(
void*)(Cyc_Absyn_compress_conref((_tmp4B3.ptr_atts).bounds))->v;_tmp4F8.f2=(void*)(
Cyc_Absyn_compress_conref((_tmp4B5.ptr_atts).bounds))->v;_tmp4F8;});void*_tmp4FA;
void*_tmp4FB;void*_tmp4FC;void*_tmp4FD;void*_tmp4FE;_LL362: _tmp4FA=_tmp4F9.f1;
if(_tmp4FA <= (void*)1)goto _LL364;if(*((int*)_tmp4FA)!= 0)goto _LL364;_tmp4FB=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp4FA)->f1;_tmp4FC=_tmp4F9.f2;if(
_tmp4FC <= (void*)1)goto _LL364;if(*((int*)_tmp4FC)!= 0)goto _LL364;_tmp4FD=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp4FC)->f1;_LL363:{struct _tuple6 _tmp500=({
struct _tuple6 _tmp4FF;_tmp4FF.f1=_tmp4FB;_tmp4FF.f2=_tmp4FD;_tmp4FF;});void*
_tmp501;void*_tmp502;void*_tmp503;void*_tmp504;void*_tmp505;void*_tmp506;void*
_tmp507;void*_tmp508;void*_tmp509;void*_tmp50A;void*_tmp50B;void*_tmp50C;void*
_tmp50D;struct Cyc_Absyn_Exp*_tmp50E;void*_tmp50F;struct Cyc_Absyn_Exp*_tmp510;
void*_tmp511;void*_tmp512;struct Cyc_Absyn_Exp*_tmp513;void*_tmp514;void*_tmp515;
struct Cyc_Absyn_Exp*_tmp516;void*_tmp517;void*_tmp518;void*_tmp519;void*_tmp51A;
void*_tmp51B;struct Cyc_Absyn_Exp*_tmp51C;void*_tmp51D;void*_tmp51E;void*_tmp51F;
void*_tmp520;_LL369: _tmp501=_tmp500.f1;if(_tmp501 <= (void*)2)goto _LL36B;if(*((
int*)_tmp501)!= 0)goto _LL36B;_tmp502=_tmp500.f2;if((int)_tmp502 != 0)goto _LL36B;
_LL36A: goto _LL36C;_LL36B: _tmp503=_tmp500.f1;if(_tmp503 <= (void*)2)goto _LL36D;if(*((
int*)_tmp503)!= 0)goto _LL36D;_tmp504=_tmp500.f2;if((int)_tmp504 != 1)goto _LL36D;
_LL36C: goto _LL36E;_LL36D: _tmp505=_tmp500.f1;if((int)_tmp505 != 1)goto _LL36F;
_tmp506=_tmp500.f2;if((int)_tmp506 != 1)goto _LL36F;_LL36E: goto _LL370;_LL36F:
_tmp507=_tmp500.f1;if((int)_tmp507 != 0)goto _LL371;_tmp508=_tmp500.f2;if((int)
_tmp508 != 1)goto _LL371;_LL370: goto _LL372;_LL371: _tmp509=_tmp500.f1;if((int)
_tmp509 != 0)goto _LL373;_tmp50A=_tmp500.f2;if((int)_tmp50A != 0)goto _LL373;_LL372:
okay=1;goto _LL368;_LL373: _tmp50B=_tmp500.f1;if((int)_tmp50B != 1)goto _LL375;
_tmp50C=_tmp500.f2;if((int)_tmp50C != 0)goto _LL375;_LL374: okay=0;goto _LL368;
_LL375: _tmp50D=_tmp500.f1;if(_tmp50D <= (void*)2)goto _LL377;if(*((int*)_tmp50D)!= 
0)goto _LL377;_tmp50E=((struct Cyc_Absyn_Upper_b_struct*)_tmp50D)->f1;_tmp50F=
_tmp500.f2;if(_tmp50F <= (void*)2)goto _LL377;if(*((int*)_tmp50F)!= 0)goto _LL377;
_tmp510=((struct Cyc_Absyn_Upper_b_struct*)_tmp50F)->f1;_LL376: okay=okay  && Cyc_Evexp_lte_const_exp(
_tmp510,_tmp50E);if(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp4B5.ptr_atts).zero_term))({void*_tmp521[0]={};Cyc_Tcutil_warn(loc,({const
char*_tmp522="implicit cast to shorter array";_tag_dynforward(_tmp522,sizeof(
char),_get_zero_arr_size(_tmp522,31));}),_tag_dynforward(_tmp521,sizeof(void*),0));});
goto _LL368;_LL377: _tmp511=_tmp500.f1;if((int)_tmp511 != 0)goto _LL379;_tmp512=
_tmp500.f2;if(_tmp512 <= (void*)2)goto _LL379;if(*((int*)_tmp512)!= 0)goto _LL379;
_tmp513=((struct Cyc_Absyn_Upper_b_struct*)_tmp512)->f1;_LL378: _tmp516=_tmp513;
goto _LL37A;_LL379: _tmp514=_tmp500.f1;if((int)_tmp514 != 1)goto _LL37B;_tmp515=
_tmp500.f2;if(_tmp515 <= (void*)2)goto _LL37B;if(*((int*)_tmp515)!= 0)goto _LL37B;
_tmp516=((struct Cyc_Absyn_Upper_b_struct*)_tmp515)->f1;_LL37A: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp4B3.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp4B5.ptr_atts).bounds))goto _LL368;okay=0;goto
_LL368;_LL37B: _tmp517=_tmp500.f1;if(_tmp517 <= (void*)2)goto _LL37D;if(*((int*)
_tmp517)!= 1)goto _LL37D;_tmp518=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp517)->f1;_tmp519=_tmp500.f2;if(_tmp519 <= (void*)2)goto _LL37D;if(*((int*)
_tmp519)!= 1)goto _LL37D;_tmp51A=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp519)->f1;_LL37C: if(!Cyc_Tcutil_unify(_tmp518,_tmp51A)){struct _tuple6 _tmp524=({
struct _tuple6 _tmp523;_tmp523.f1=Cyc_Tcutil_compress(_tmp518);_tmp523.f2=Cyc_Tcutil_compress(
_tmp51A);_tmp523;});void*_tmp525;int _tmp526;void*_tmp527;int _tmp528;_LL384:
_tmp525=_tmp524.f1;if(_tmp525 <= (void*)4)goto _LL386;if(*((int*)_tmp525)!= 18)
goto _LL386;_tmp526=((struct Cyc_Absyn_TypeInt_struct*)_tmp525)->f1;_tmp527=
_tmp524.f2;if(_tmp527 <= (void*)4)goto _LL386;if(*((int*)_tmp527)!= 18)goto _LL386;
_tmp528=((struct Cyc_Absyn_TypeInt_struct*)_tmp527)->f1;_LL385: if(_tmp526 < 
_tmp528)okay=0;goto _LL383;_LL386:;_LL387: okay=0;goto _LL383;_LL383:;}goto _LL368;
_LL37D: _tmp51B=_tmp500.f1;if(_tmp51B <= (void*)2)goto _LL37F;if(*((int*)_tmp51B)!= 
0)goto _LL37F;_tmp51C=((struct Cyc_Absyn_Upper_b_struct*)_tmp51B)->f1;_tmp51D=
_tmp500.f2;if(_tmp51D <= (void*)2)goto _LL37F;if(*((int*)_tmp51D)!= 1)goto _LL37F;
_tmp51E=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp51D)->f1;_LL37E: {
unsigned int _tmp52A;int _tmp52B;struct _tuple7 _tmp529=Cyc_Evexp_eval_const_uint_exp(
_tmp51C);_tmp52A=_tmp529.f1;_tmp52B=_tmp529.f2;if(!_tmp52B){okay=0;goto _LL368;}{
void*_tmp52C=Cyc_Tcutil_compress(_tmp51E);int _tmp52D;_LL389: if(_tmp52C <= (void*)
4)goto _LL38B;if(*((int*)_tmp52C)!= 18)goto _LL38B;_tmp52D=((struct Cyc_Absyn_TypeInt_struct*)
_tmp52C)->f1;_LL38A: if(_tmp52A < _tmp52D)okay=0;goto _LL388;_LL38B:;_LL38C: okay=0;
goto _LL388;_LL388:;}goto _LL368;}_LL37F: _tmp51F=_tmp500.f1;if(_tmp51F <= (void*)2)
goto _LL381;if(*((int*)_tmp51F)!= 1)goto _LL381;_LL380: goto _LL382;_LL381: _tmp520=
_tmp500.f2;if(_tmp520 <= (void*)2)goto _LL368;if(*((int*)_tmp520)!= 1)goto _LL368;
_LL382: okay=0;goto _LL368;_LL368:;}goto _LL361;_LL364: _tmp4FE=_tmp4F9.f1;if((int)
_tmp4FE != 0)goto _LL366;_LL365: okay=0;goto _LL361;_LL366:;_LL367:({void*_tmp52E[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp52F="silent_castable conrefs didn't unify";_tag_dynforward(_tmp52F,
sizeof(char),_get_zero_arr_size(_tmp52F,37));}),_tag_dynforward(_tmp52E,sizeof(
void*),0));});_LL361:;}okay=okay  && Cyc_Tcutil_unify((void*)_tmp4B3.elt_typ,(
void*)_tmp4B5.elt_typ);okay=okay  && (Cyc_Tcutil_unify((void*)(_tmp4B3.ptr_atts).rgn,(
void*)(_tmp4B5.ptr_atts).rgn) || Cyc_Tcenv_region_outlives(te,(void*)(_tmp4B3.ptr_atts).rgn,(
void*)(_tmp4B5.ptr_atts).rgn));okay=okay  && (!(_tmp4B3.elt_tq).real_const  || (
_tmp4B5.elt_tq).real_const);okay=okay  && (((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp4B3.ptr_atts).zero_term,(
_tmp4B5.ptr_atts).zero_term) || ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,(_tmp4B3.ptr_atts).zero_term) && (_tmp4B5.elt_tq).real_const);return okay;}
_LL34C: _tmp4B6=_tmp4B1.f1;if(_tmp4B6 <= (void*)4)goto _LL34E;if(*((int*)_tmp4B6)!= 
2)goto _LL34E;_tmp4B7=((struct Cyc_Absyn_TunionType_struct*)_tmp4B6)->f1;_tmp4B8=(
void*)_tmp4B7.tunion_info;if(*((int*)_tmp4B8)!= 1)goto _LL34E;_tmp4B9=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp4B8)->f1;_tmp4BA=*_tmp4B9;_tmp4BB=_tmp4B7.targs;_tmp4BC=(void*)_tmp4B7.rgn;
_tmp4BD=_tmp4B1.f2;if(_tmp4BD <= (void*)4)goto _LL34E;if(*((int*)_tmp4BD)!= 2)goto
_LL34E;_tmp4BE=((struct Cyc_Absyn_TunionType_struct*)_tmp4BD)->f1;_tmp4BF=(void*)
_tmp4BE.tunion_info;if(*((int*)_tmp4BF)!= 1)goto _LL34E;_tmp4C0=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp4BF)->f1;_tmp4C1=*_tmp4C0;_tmp4C2=_tmp4BE.targs;_tmp4C3=(void*)_tmp4BE.rgn;
_LL34D: if(_tmp4BA != _tmp4C1  || !Cyc_Tcenv_region_outlives(te,_tmp4BC,_tmp4C3))
return 0;for(0;_tmp4BB != 0  && _tmp4C2 != 0;(_tmp4BB=_tmp4BB->tl,_tmp4C2=_tmp4C2->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp4BB->hd,(void*)_tmp4C2->hd))return 0;}if(_tmp4BB
!= 0  || _tmp4C2 != 0)return 0;return 1;_LL34E: _tmp4C4=_tmp4B1.f1;if(_tmp4C4 <= (void*)
4)goto _LL350;if(*((int*)_tmp4C4)!= 7)goto _LL350;_tmp4C5=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4C4)->f1;_tmp4C6=(void*)_tmp4C5.elt_type;_tmp4C7=_tmp4C5.tq;_tmp4C8=_tmp4C5.num_elts;
_tmp4C9=_tmp4C5.zero_term;_tmp4CA=_tmp4B1.f2;if(_tmp4CA <= (void*)4)goto _LL350;
if(*((int*)_tmp4CA)!= 7)goto _LL350;_tmp4CB=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4CA)->f1;_tmp4CC=(void*)_tmp4CB.elt_type;_tmp4CD=_tmp4CB.tq;_tmp4CE=_tmp4CB.num_elts;
_tmp4CF=_tmp4CB.zero_term;_LL34F: {int okay;okay=((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp4C9,_tmp4CF) && ((_tmp4C8 != 0  && _tmp4CE != 0) && Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_tmp4C8,(struct Cyc_Absyn_Exp*)_tmp4CE));return(okay  && Cyc_Tcutil_unify(
_tmp4C6,_tmp4CC)) && (!_tmp4C7.real_const  || _tmp4CD.real_const);}_LL350: _tmp4D0=
_tmp4B1.f1;if(_tmp4D0 <= (void*)4)goto _LL352;if(*((int*)_tmp4D0)!= 3)goto _LL352;
_tmp4D1=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp4D0)->f1;_tmp4D2=(void*)
_tmp4D1.field_info;if(*((int*)_tmp4D2)!= 1)goto _LL352;_tmp4D3=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp4D2)->f1;_tmp4D4=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4D2)->f2;
_tmp4D5=_tmp4D1.targs;_tmp4D6=_tmp4B1.f2;if(_tmp4D6 <= (void*)4)goto _LL352;if(*((
int*)_tmp4D6)!= 2)goto _LL352;_tmp4D7=((struct Cyc_Absyn_TunionType_struct*)
_tmp4D6)->f1;_tmp4D8=(void*)_tmp4D7.tunion_info;if(*((int*)_tmp4D8)!= 1)goto
_LL352;_tmp4D9=((struct Cyc_Absyn_KnownTunion_struct*)_tmp4D8)->f1;_tmp4DA=*
_tmp4D9;_tmp4DB=_tmp4D7.targs;_LL351: if((_tmp4D3 == _tmp4DA  || Cyc_Absyn_qvar_cmp(
_tmp4D3->name,_tmp4DA->name)== 0) && _tmp4D4->typs == 0){for(0;_tmp4D5 != 0  && 
_tmp4DB != 0;(_tmp4D5=_tmp4D5->tl,_tmp4DB=_tmp4DB->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp4D5->hd,(void*)_tmp4DB->hd))break;}if(_tmp4D5 == 0  && _tmp4DB == 0)
return 1;}return 0;_LL352: _tmp4DC=_tmp4B1.f1;if(_tmp4DC <= (void*)4)goto _LL354;if(*((
int*)_tmp4DC)!= 4)goto _LL354;_tmp4DD=((struct Cyc_Absyn_PointerType_struct*)
_tmp4DC)->f1;_tmp4DE=(void*)_tmp4DD.elt_typ;_tmp4DF=_tmp4DD.elt_tq;_tmp4E0=
_tmp4DD.ptr_atts;_tmp4E1=(void*)_tmp4E0.rgn;_tmp4E2=_tmp4E0.nullable;_tmp4E3=
_tmp4E0.bounds;_tmp4E4=_tmp4E0.zero_term;_tmp4E5=_tmp4B1.f2;if(_tmp4E5 <= (void*)
4)goto _LL354;if(*((int*)_tmp4E5)!= 2)goto _LL354;_tmp4E6=((struct Cyc_Absyn_TunionType_struct*)
_tmp4E5)->f1;_tmp4E7=(void*)_tmp4E6.tunion_info;if(*((int*)_tmp4E7)!= 1)goto
_LL354;_tmp4E8=((struct Cyc_Absyn_KnownTunion_struct*)_tmp4E7)->f1;_tmp4E9=*
_tmp4E8;_tmp4EA=_tmp4E6.targs;_tmp4EB=(void*)_tmp4E6.rgn;_LL353:{void*_tmp530=
Cyc_Tcutil_compress(_tmp4DE);struct Cyc_Absyn_TunionFieldInfo _tmp531;void*_tmp532;
struct Cyc_Absyn_Tuniondecl*_tmp533;struct Cyc_Absyn_Tunionfield*_tmp534;struct Cyc_List_List*
_tmp535;_LL38E: if(_tmp530 <= (void*)4)goto _LL390;if(*((int*)_tmp530)!= 3)goto
_LL390;_tmp531=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp530)->f1;_tmp532=(
void*)_tmp531.field_info;if(*((int*)_tmp532)!= 1)goto _LL390;_tmp533=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp532)->f1;_tmp534=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp532)->f2;
_tmp535=_tmp531.targs;_LL38F: if(!Cyc_Tcutil_unify(_tmp4E1,_tmp4EB) && !Cyc_Tcenv_region_outlives(
te,_tmp4E1,_tmp4EB))return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4E2,
Cyc_Absyn_false_conref))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp4E3,Cyc_Absyn_bounds_one_conref))return 0;if(!((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp4E4,Cyc_Absyn_false_conref))return 0;if(Cyc_Absyn_qvar_cmp(_tmp4E9->name,
_tmp533->name)== 0  && _tmp534->typs != 0){int okay=1;for(0;_tmp535 != 0  && _tmp4EA
!= 0;(_tmp535=_tmp535->tl,_tmp4EA=_tmp4EA->tl)){if(!Cyc_Tcutil_unify((void*)
_tmp535->hd,(void*)_tmp4EA->hd)){okay=0;break;}}if((!okay  || _tmp535 != 0) || 
_tmp4EA != 0)return 0;return 1;}goto _LL38D;_LL390:;_LL391: goto _LL38D;_LL38D:;}
return 0;_LL354: _tmp4EC=_tmp4B1.f1;if(_tmp4EC <= (void*)4)goto _LL356;if(*((int*)
_tmp4EC)!= 14)goto _LL356;_tmp4ED=_tmp4B1.f2;if(_tmp4ED <= (void*)4)goto _LL356;if(*((
int*)_tmp4ED)!= 5)goto _LL356;_tmp4EE=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4ED)->f2;if((int)_tmp4EE != 2)goto _LL356;_LL355: goto _LL357;_LL356: _tmp4EF=
_tmp4B1.f1;if(_tmp4EF <= (void*)4)goto _LL358;if(*((int*)_tmp4EF)!= 14)goto _LL358;
_tmp4F0=_tmp4B1.f2;if(_tmp4F0 <= (void*)4)goto _LL358;if(*((int*)_tmp4F0)!= 5)goto
_LL358;_tmp4F1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4F0)->f2;if((int)
_tmp4F1 != 3)goto _LL358;_LL357: return 1;_LL358: _tmp4F2=_tmp4B1.f1;if(_tmp4F2 <= (
void*)4)goto _LL35A;if(*((int*)_tmp4F2)!= 17)goto _LL35A;_tmp4F3=_tmp4B1.f2;if(
_tmp4F3 <= (void*)4)goto _LL35A;if(*((int*)_tmp4F3)!= 5)goto _LL35A;_LL359: return 0;
_LL35A:;_LL35B: return Cyc_Tcutil_unify(t1,t2);_LL349:;}}int Cyc_Tcutil_is_pointer_type(
void*t){void*_tmp536=Cyc_Tcutil_compress(t);_LL393: if(_tmp536 <= (void*)4)goto
_LL395;if(*((int*)_tmp536)!= 4)goto _LL395;_LL394: return 1;_LL395:;_LL396: return 0;
_LL392:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){void*_tmp537=(void*)e->r;
void*_tmp538;int _tmp539;void*_tmp53A;char _tmp53B;void*_tmp53C;short _tmp53D;void*
_tmp53E;long long _tmp53F;void*_tmp540;struct Cyc_Absyn_Exp*_tmp541;_LL398: if(*((
int*)_tmp537)!= 0)goto _LL39A;_tmp538=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp537)->f1;if(_tmp538 <= (void*)1)goto _LL39A;if(*((int*)_tmp538)!= 2)goto _LL39A;
_tmp539=((struct Cyc_Absyn_Int_c_struct*)_tmp538)->f2;if(_tmp539 != 0)goto _LL39A;
_LL399: goto _LL39B;_LL39A: if(*((int*)_tmp537)!= 0)goto _LL39C;_tmp53A=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp537)->f1;if(_tmp53A <= (void*)1)goto _LL39C;
if(*((int*)_tmp53A)!= 0)goto _LL39C;_tmp53B=((struct Cyc_Absyn_Char_c_struct*)
_tmp53A)->f2;if(_tmp53B != 0)goto _LL39C;_LL39B: goto _LL39D;_LL39C: if(*((int*)
_tmp537)!= 0)goto _LL39E;_tmp53C=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp537)->f1;
if(_tmp53C <= (void*)1)goto _LL39E;if(*((int*)_tmp53C)!= 1)goto _LL39E;_tmp53D=((
struct Cyc_Absyn_Short_c_struct*)_tmp53C)->f2;if(_tmp53D != 0)goto _LL39E;_LL39D:
goto _LL39F;_LL39E: if(*((int*)_tmp537)!= 0)goto _LL3A0;_tmp53E=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp537)->f1;if(_tmp53E <= (void*)1)goto _LL3A0;if(*((int*)_tmp53E)!= 3)goto _LL3A0;
_tmp53F=((struct Cyc_Absyn_LongLong_c_struct*)_tmp53E)->f2;if(_tmp53F != 0)goto
_LL3A0;_LL39F: return 1;_LL3A0: if(*((int*)_tmp537)!= 15)goto _LL3A2;_tmp540=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp537)->f1;_tmp541=((struct Cyc_Absyn_Cast_e_struct*)
_tmp537)->f2;_LL3A1: return Cyc_Tcutil_is_zero(_tmp541) && Cyc_Tcutil_admits_zero(
_tmp540);_LL3A2:;_LL3A3: return 0;_LL397:;}struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((
void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(void*)((void*)0)};struct Cyc_Core_Opt
Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)((void*)
1)};int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*
e1){if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){(void*)(e1->r=(
void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp542=_cycalloc(sizeof(*_tmp542));
_tmp542[0]=({struct Cyc_Absyn_Const_e_struct _tmp543;_tmp543.tag=0;_tmp543.f1=(
void*)((void*)0);_tmp543;});_tmp542;})));{struct Cyc_List_List*_tmp544=Cyc_Tcenv_lookup_type_vars(
te);struct Cyc_Absyn_PointerType_struct*_tmp545=({struct Cyc_Absyn_PointerType_struct*
_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546[0]=({struct Cyc_Absyn_PointerType_struct
_tmp547;_tmp547.tag=4;_tmp547.f1=({struct Cyc_Absyn_PtrInfo _tmp548;_tmp548.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B->v=_tmp544;_tmp54B;}));_tmp548.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp548.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp549;
_tmp549.rgn=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt*_tmp54A=_cycalloc(sizeof(*_tmp54A));_tmp54A->v=_tmp544;
_tmp54A;}));_tmp549.nullable=Cyc_Absyn_true_conref;_tmp549.bounds=Cyc_Absyn_empty_conref();
_tmp549.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp549.ptrloc=0;_tmp549;});_tmp548;});_tmp547;});_tmp546;});(void*)(((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v=(void*)((void*)_tmp545));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}struct _dynforward_ptr Cyc_Tcutil_coercion2string(void*c){
void*_tmp54C=c;_LL3A5: if((int)_tmp54C != 0)goto _LL3A7;_LL3A6: return({const char*
_tmp54D="unknown";_tag_dynforward(_tmp54D,sizeof(char),_get_zero_arr_size(
_tmp54D,8));});_LL3A7: if((int)_tmp54C != 1)goto _LL3A9;_LL3A8: return({const char*
_tmp54E="no coercion";_tag_dynforward(_tmp54E,sizeof(char),_get_zero_arr_size(
_tmp54E,12));});_LL3A9: if((int)_tmp54C != 2)goto _LL3AB;_LL3AA: return({const char*
_tmp54F="null check";_tag_dynforward(_tmp54F,sizeof(char),_get_zero_arr_size(
_tmp54F,11));});_LL3AB: if((int)_tmp54C != 3)goto _LL3A4;_LL3AC: return({const char*
_tmp550="other coercion";_tag_dynforward(_tmp550,sizeof(char),_get_zero_arr_size(
_tmp550,15));});_LL3A4:;}int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*c;if(Cyc_Tcutil_unify(t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(
t2) && Cyc_Tcutil_is_arithmetic_type(t1)){if(Cyc_Tcutil_will_lose_precision(t1,
t2))({struct Cyc_String_pa_struct _tmp554;_tmp554.tag=0;_tmp554.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmp553;_tmp553.tag=0;_tmp553.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp551[2]={& _tmp553,& _tmp554};Cyc_Tcutil_warn(
e->loc,({const char*_tmp552="integral size mismatch; %s -> %s conversion supplied";
_tag_dynforward(_tmp552,sizeof(char),_get_zero_arr_size(_tmp552,53));}),
_tag_dynforward(_tmp551,sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(te,e,t2,(
void*)1);return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2,(void*)3);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;
else{if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= (void*)0){Cyc_Tcutil_unchecked_cast(
te,e,t2,c);if(c != (void*)2)({struct Cyc_String_pa_struct _tmp558;_tmp558.tag=0;
_tmp558.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp557;_tmp557.tag=0;_tmp557.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp555[2]={& _tmp557,& _tmp558};Cyc_Tcutil_warn(e->loc,({const char*_tmp556="implicit cast from %s to %s";
_tag_dynforward(_tmp556,sizeof(char),_get_zero_arr_size(_tmp556,28));}),
_tag_dynforward(_tmp555,sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp559=Cyc_Tcutil_compress(t);
_LL3AE: if(_tmp559 <= (void*)4)goto _LL3B0;if(*((int*)_tmp559)!= 5)goto _LL3B0;
_LL3AF: goto _LL3B1;_LL3B0: if((int)_tmp559 != 1)goto _LL3B2;_LL3B1: goto _LL3B3;_LL3B2:
if(_tmp559 <= (void*)4)goto _LL3B4;if(*((int*)_tmp559)!= 6)goto _LL3B4;_LL3B3:
return 1;_LL3B4:;_LL3B5: return 0;_LL3AD:;}static struct _tuple4*Cyc_Tcutil_flatten_typ_f(
struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*x){return({struct _tuple4*
_tmp55A=_cycalloc(sizeof(*_tmp55A));_tmp55A->f1=x->tq;_tmp55A->f2=Cyc_Tcutil_substitute(
inst,(void*)x->type);_tmp55A;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv*te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp55B=t1;
struct Cyc_List_List*_tmp55C;struct Cyc_Absyn_AggrInfo _tmp55D;void*_tmp55E;struct
Cyc_Absyn_Aggrdecl**_tmp55F;struct Cyc_Absyn_Aggrdecl*_tmp560;struct Cyc_List_List*
_tmp561;void*_tmp562;struct Cyc_List_List*_tmp563;struct Cyc_Absyn_FnInfo _tmp564;
_LL3B7: if((int)_tmp55B != 0)goto _LL3B9;_LL3B8: return 0;_LL3B9: if(_tmp55B <= (void*)
4)goto _LL3C1;if(*((int*)_tmp55B)!= 9)goto _LL3BB;_tmp55C=((struct Cyc_Absyn_TupleType_struct*)
_tmp55B)->f1;_LL3BA: return _tmp55C;_LL3BB: if(*((int*)_tmp55B)!= 10)goto _LL3BD;
_tmp55D=((struct Cyc_Absyn_AggrType_struct*)_tmp55B)->f1;_tmp55E=(void*)_tmp55D.aggr_info;
if(*((int*)_tmp55E)!= 1)goto _LL3BD;_tmp55F=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp55E)->f1;_tmp560=*_tmp55F;_tmp561=_tmp55D.targs;_LL3BC: if((((void*)_tmp560->kind
== (void*)1  || _tmp560->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp560->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp560->impl))->rgn_po != 0)return({struct Cyc_List_List*_tmp565=_cycalloc(
sizeof(*_tmp565));_tmp565->hd=({struct _tuple4*_tmp566=_cycalloc(sizeof(*_tmp566));
_tmp566->f1=Cyc_Absyn_empty_tqual(0);_tmp566->f2=t1;_tmp566;});_tmp565->tl=0;
_tmp565;});{struct Cyc_List_List*_tmp567=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp560->tvs,_tmp561);return((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmp567,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp560->impl))->fields);}_LL3BD: if(*((
int*)_tmp55B)!= 11)goto _LL3BF;_tmp562=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp55B)->f1;if((int)_tmp562 != 0)goto _LL3BF;_tmp563=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp55B)->f2;_LL3BE: return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_flatten_typ_f,0,_tmp563);_LL3BF: if(*((int*)_tmp55B)!= 8)goto _LL3C1;
_tmp564=((struct Cyc_Absyn_FnType_struct*)_tmp55B)->f1;_LL3C0: return({struct Cyc_List_List*
_tmp568=_cycalloc(sizeof(*_tmp568));_tmp568->hd=({struct _tuple4*_tmp569=
_cycalloc(sizeof(*_tmp569));_tmp569->f1=Cyc_Absyn_const_tqual(0);_tmp569->f2=t1;
_tmp569;});_tmp568->tl=0;_tmp568;});_LL3C1:;_LL3C2: return({struct Cyc_List_List*
_tmp56A=_cycalloc(sizeof(*_tmp56A));_tmp56A->hd=({struct _tuple4*_tmp56B=
_cycalloc(sizeof(*_tmp56B));_tmp56B->f1=Cyc_Absyn_empty_tqual(0);_tmp56B->f2=t1;
_tmp56B;});_tmp56A->tl=0;_tmp56A;});_LL3B6:;}}static int Cyc_Tcutil_sub_attributes(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t
!= 0;t=t->tl){void*_tmp56C=(void*)t->hd;_LL3C4: if((int)_tmp56C != 16)goto _LL3C6;
_LL3C5: goto _LL3C7;_LL3C6: if((int)_tmp56C != 3)goto _LL3C8;_LL3C7: goto _LL3C9;_LL3C8:
if(_tmp56C <= (void*)17)goto _LL3CA;if(*((int*)_tmp56C)!= 4)goto _LL3CA;_LL3C9:
continue;_LL3CA:;_LL3CB: if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)t->hd,
a2))return 0;_LL3C3:;}}for(0;a2 != 0;a2=a2->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple6*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple6*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6
_tmp56E=({struct _tuple6 _tmp56D;_tmp56D.f1=t1;_tmp56D.f2=t2;_tmp56D;});void*
_tmp56F;struct Cyc_Absyn_PtrInfo _tmp570;void*_tmp571;struct Cyc_Absyn_Tqual _tmp572;
struct Cyc_Absyn_PtrAtts _tmp573;void*_tmp574;struct Cyc_Absyn_Conref*_tmp575;
struct Cyc_Absyn_Conref*_tmp576;struct Cyc_Absyn_Conref*_tmp577;void*_tmp578;
struct Cyc_Absyn_PtrInfo _tmp579;void*_tmp57A;struct Cyc_Absyn_Tqual _tmp57B;struct
Cyc_Absyn_PtrAtts _tmp57C;void*_tmp57D;struct Cyc_Absyn_Conref*_tmp57E;struct Cyc_Absyn_Conref*
_tmp57F;struct Cyc_Absyn_Conref*_tmp580;void*_tmp581;struct Cyc_Absyn_TunionInfo
_tmp582;void*_tmp583;struct Cyc_Absyn_Tuniondecl**_tmp584;struct Cyc_Absyn_Tuniondecl*
_tmp585;struct Cyc_List_List*_tmp586;void*_tmp587;void*_tmp588;struct Cyc_Absyn_TunionInfo
_tmp589;void*_tmp58A;struct Cyc_Absyn_Tuniondecl**_tmp58B;struct Cyc_Absyn_Tuniondecl*
_tmp58C;struct Cyc_List_List*_tmp58D;void*_tmp58E;void*_tmp58F;struct Cyc_Absyn_FnInfo
_tmp590;void*_tmp591;struct Cyc_Absyn_FnInfo _tmp592;_LL3CD: _tmp56F=_tmp56E.f1;if(
_tmp56F <= (void*)4)goto _LL3CF;if(*((int*)_tmp56F)!= 4)goto _LL3CF;_tmp570=((
struct Cyc_Absyn_PointerType_struct*)_tmp56F)->f1;_tmp571=(void*)_tmp570.elt_typ;
_tmp572=_tmp570.elt_tq;_tmp573=_tmp570.ptr_atts;_tmp574=(void*)_tmp573.rgn;
_tmp575=_tmp573.nullable;_tmp576=_tmp573.bounds;_tmp577=_tmp573.zero_term;
_tmp578=_tmp56E.f2;if(_tmp578 <= (void*)4)goto _LL3CF;if(*((int*)_tmp578)!= 4)goto
_LL3CF;_tmp579=((struct Cyc_Absyn_PointerType_struct*)_tmp578)->f1;_tmp57A=(void*)
_tmp579.elt_typ;_tmp57B=_tmp579.elt_tq;_tmp57C=_tmp579.ptr_atts;_tmp57D=(void*)
_tmp57C.rgn;_tmp57E=_tmp57C.nullable;_tmp57F=_tmp57C.bounds;_tmp580=_tmp57C.zero_term;
_LL3CE: if(_tmp572.real_const  && !_tmp57B.real_const)return 0;if((!((int(*)(int(*
cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp575,_tmp57E) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp575)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp57E))return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp577,
_tmp580) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp577)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp580))return 0;if(!Cyc_Tcutil_unify(_tmp574,_tmp57D) && !Cyc_Tcenv_region_outlives(
te,_tmp574,_tmp57D))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp576,_tmp57F)){struct _tuple6 _tmp594=({struct _tuple6 _tmp593;_tmp593.f1=Cyc_Absyn_conref_val(
_tmp576);_tmp593.f2=Cyc_Absyn_conref_val(_tmp57F);_tmp593;});void*_tmp595;void*
_tmp596;void*_tmp597;void*_tmp598;void*_tmp599;struct Cyc_Absyn_Exp*_tmp59A;void*
_tmp59B;struct Cyc_Absyn_Exp*_tmp59C;_LL3D6: _tmp595=_tmp594.f1;if(_tmp595 <= (void*)
2)goto _LL3D8;if(*((int*)_tmp595)!= 0)goto _LL3D8;_tmp596=_tmp594.f2;if((int)
_tmp596 != 0)goto _LL3D8;_LL3D7: goto _LL3D5;_LL3D8: _tmp597=_tmp594.f1;if(_tmp597 <= (
void*)2)goto _LL3DA;if(*((int*)_tmp597)!= 0)goto _LL3DA;_tmp598=_tmp594.f2;if((int)
_tmp598 != 1)goto _LL3DA;_LL3D9: goto _LL3D5;_LL3DA: _tmp599=_tmp594.f1;if(_tmp599 <= (
void*)2)goto _LL3DC;if(*((int*)_tmp599)!= 0)goto _LL3DC;_tmp59A=((struct Cyc_Absyn_Upper_b_struct*)
_tmp599)->f1;_tmp59B=_tmp594.f2;if(_tmp59B <= (void*)2)goto _LL3DC;if(*((int*)
_tmp59B)!= 0)goto _LL3DC;_tmp59C=((struct Cyc_Absyn_Upper_b_struct*)_tmp59B)->f1;
_LL3DB: if(!Cyc_Evexp_lte_const_exp(_tmp59C,_tmp59A))return 0;goto _LL3D5;_LL3DC:;
_LL3DD: return 0;_LL3D5:;}return Cyc_Tcutil_ptrsubtype(te,({struct Cyc_List_List*
_tmp59D=_cycalloc(sizeof(*_tmp59D));_tmp59D->hd=({struct _tuple6*_tmp59E=
_cycalloc(sizeof(*_tmp59E));_tmp59E->f1=t1;_tmp59E->f2=t2;_tmp59E;});_tmp59D->tl=
assume;_tmp59D;}),_tmp571,_tmp57A);_LL3CF: _tmp581=_tmp56E.f1;if(_tmp581 <= (void*)
4)goto _LL3D1;if(*((int*)_tmp581)!= 2)goto _LL3D1;_tmp582=((struct Cyc_Absyn_TunionType_struct*)
_tmp581)->f1;_tmp583=(void*)_tmp582.tunion_info;if(*((int*)_tmp583)!= 1)goto
_LL3D1;_tmp584=((struct Cyc_Absyn_KnownTunion_struct*)_tmp583)->f1;_tmp585=*
_tmp584;_tmp586=_tmp582.targs;_tmp587=(void*)_tmp582.rgn;_tmp588=_tmp56E.f2;if(
_tmp588 <= (void*)4)goto _LL3D1;if(*((int*)_tmp588)!= 2)goto _LL3D1;_tmp589=((
struct Cyc_Absyn_TunionType_struct*)_tmp588)->f1;_tmp58A=(void*)_tmp589.tunion_info;
if(*((int*)_tmp58A)!= 1)goto _LL3D1;_tmp58B=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp58A)->f1;_tmp58C=*_tmp58B;_tmp58D=_tmp589.targs;_tmp58E=(void*)_tmp589.rgn;
_LL3D0: if(_tmp585 != _tmp58C  || !Cyc_Tcenv_region_outlives(te,_tmp587,_tmp58E))
return 0;for(0;_tmp586 != 0  && _tmp58D != 0;(_tmp586=_tmp586->tl,_tmp58D=_tmp58D->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp586->hd,(void*)_tmp58D->hd))return 0;}if(_tmp586
!= 0  || _tmp58D != 0)return 0;return 1;_LL3D1: _tmp58F=_tmp56E.f1;if(_tmp58F <= (void*)
4)goto _LL3D3;if(*((int*)_tmp58F)!= 8)goto _LL3D3;_tmp590=((struct Cyc_Absyn_FnType_struct*)
_tmp58F)->f1;_tmp591=_tmp56E.f2;if(_tmp591 <= (void*)4)goto _LL3D3;if(*((int*)
_tmp591)!= 8)goto _LL3D3;_tmp592=((struct Cyc_Absyn_FnType_struct*)_tmp591)->f1;
_LL3D2: if(_tmp590.tvars != 0  || _tmp592.tvars != 0){struct Cyc_List_List*_tmp59F=
_tmp590.tvars;struct Cyc_List_List*_tmp5A0=_tmp592.tvars;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp59F)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp5A0))return 0;{struct _RegionHandle _tmp5A1=_new_region("r");struct
_RegionHandle*r=& _tmp5A1;_push_region(r);{struct Cyc_List_List*inst=0;while(
_tmp59F != 0){if(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp59F->hd)!= Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp5A0))->hd)){int
_tmp5A2=0;_npop_handler(0);return _tmp5A2;}inst=({struct Cyc_List_List*_tmp5A3=
_region_malloc(r,sizeof(*_tmp5A3));_tmp5A3->hd=({struct _tuple8*_tmp5A4=
_region_malloc(r,sizeof(*_tmp5A4));_tmp5A4->f1=(struct Cyc_Absyn_Tvar*)_tmp5A0->hd;
_tmp5A4->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp5A5=_cycalloc(sizeof(*
_tmp5A5));_tmp5A5[0]=({struct Cyc_Absyn_VarType_struct _tmp5A6;_tmp5A6.tag=1;
_tmp5A6.f1=(struct Cyc_Absyn_Tvar*)_tmp59F->hd;_tmp5A6;});_tmp5A5;});_tmp5A4;});
_tmp5A3->tl=inst;_tmp5A3;});_tmp59F=_tmp59F->tl;_tmp5A0=_tmp5A0->tl;}if(inst != 0){
_tmp590.tvars=0;_tmp592.tvars=0;{int _tmp5AB=Cyc_Tcutil_subtype(te,assume,(void*)({
struct Cyc_Absyn_FnType_struct*_tmp5A7=_cycalloc(sizeof(*_tmp5A7));_tmp5A7[0]=({
struct Cyc_Absyn_FnType_struct _tmp5A8;_tmp5A8.tag=8;_tmp5A8.f1=_tmp590;_tmp5A8;});
_tmp5A7;}),(void*)({struct Cyc_Absyn_FnType_struct*_tmp5A9=_cycalloc(sizeof(*
_tmp5A9));_tmp5A9[0]=({struct Cyc_Absyn_FnType_struct _tmp5AA;_tmp5AA.tag=8;
_tmp5AA.f1=_tmp592;_tmp5AA;});_tmp5A9;}));_npop_handler(0);return _tmp5AB;}}};
_pop_region(r);}}if(!Cyc_Tcutil_subtype(te,assume,(void*)_tmp590.ret_typ,(void*)
_tmp592.ret_typ))return 0;{struct Cyc_List_List*_tmp5AC=_tmp590.args;struct Cyc_List_List*
_tmp5AD=_tmp592.args;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5AC)
!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5AD))return 0;for(0;
_tmp5AC != 0;(_tmp5AC=_tmp5AC->tl,_tmp5AD=_tmp5AD->tl)){struct Cyc_Absyn_Tqual
_tmp5AF;void*_tmp5B0;struct _tuple2 _tmp5AE=*((struct _tuple2*)_tmp5AC->hd);_tmp5AF=
_tmp5AE.f2;_tmp5B0=_tmp5AE.f3;{struct Cyc_Absyn_Tqual _tmp5B2;void*_tmp5B3;struct
_tuple2 _tmp5B1=*((struct _tuple2*)((struct Cyc_List_List*)_check_null(_tmp5AD))->hd);
_tmp5B2=_tmp5B1.f2;_tmp5B3=_tmp5B1.f3;if(_tmp5B2.real_const  && !_tmp5AF.real_const
 || !Cyc_Tcutil_subtype(te,assume,_tmp5B3,_tmp5B0))return 0;}}if(_tmp590.c_varargs
!= _tmp592.c_varargs)return 0;if(_tmp590.cyc_varargs != 0  && _tmp592.cyc_varargs != 
0){struct Cyc_Absyn_VarargInfo _tmp5B4=*_tmp590.cyc_varargs;struct Cyc_Absyn_VarargInfo
_tmp5B5=*_tmp592.cyc_varargs;if((_tmp5B5.tq).real_const  && !(_tmp5B4.tq).real_const
 || !Cyc_Tcutil_subtype(te,assume,(void*)_tmp5B5.type,(void*)_tmp5B4.type))
return 0;}else{if(_tmp590.cyc_varargs != 0  || _tmp592.cyc_varargs != 0)return 0;}if(
!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp590.effect))->v,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp592.effect))->v))return 0;if(!Cyc_Tcutil_sub_rgnpo(
_tmp590.rgn_po,_tmp592.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(_tmp590.attributes,
_tmp592.attributes))return 0;return 1;}_LL3D3:;_LL3D4: return 0;_LL3CC:;}}static int
Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple6 _tmp5B7=({struct _tuple6
_tmp5B6;_tmp5B6.f1=Cyc_Tcutil_compress(t1);_tmp5B6.f2=Cyc_Tcutil_compress(t2);
_tmp5B6;});void*_tmp5B8;void*_tmp5B9;void*_tmp5BA;void*_tmp5BB;_LL3DF: _tmp5B8=
_tmp5B7.f1;if(_tmp5B8 <= (void*)4)goto _LL3E1;if(*((int*)_tmp5B8)!= 5)goto _LL3E1;
_tmp5B9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5B8)->f2;_tmp5BA=_tmp5B7.f2;
if(_tmp5BA <= (void*)4)goto _LL3E1;if(*((int*)_tmp5BA)!= 5)goto _LL3E1;_tmp5BB=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp5BA)->f2;_LL3E0: return(_tmp5B9 == 
_tmp5BB  || _tmp5B9 == (void*)2  && _tmp5BB == (void*)3) || _tmp5B9 == (void*)3  && 
_tmp5BB == (void*)2;_LL3E1:;_LL3E2: return 0;_LL3DE:;}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*
tqs1=Cyc_Tcutil_flatten_typ(te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(
te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct
_tuple4 _tmp5BD;struct Cyc_Absyn_Tqual _tmp5BE;void*_tmp5BF;struct _tuple4*_tmp5BC=(
struct _tuple4*)tqs1->hd;_tmp5BD=*_tmp5BC;_tmp5BE=_tmp5BD.f1;_tmp5BF=_tmp5BD.f2;{
struct _tuple4 _tmp5C1;struct Cyc_Absyn_Tqual _tmp5C2;void*_tmp5C3;struct _tuple4*
_tmp5C0=(struct _tuple4*)tqs2->hd;_tmp5C1=*_tmp5C0;_tmp5C2=_tmp5C1.f1;_tmp5C3=
_tmp5C1.f2;if(_tmp5C2.real_const  && Cyc_Tcutil_subtype(te,assume,_tmp5BF,_tmp5C3))
continue;else{if(Cyc_Tcutil_unify(_tmp5BF,_tmp5C3))continue;else{if(Cyc_Tcutil_isomorphic(
_tmp5BF,_tmp5C3))continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(
void*t){void*_tmp5C4=Cyc_Tcutil_compress(t);void*_tmp5C5;_LL3E4: if(_tmp5C4 <= (
void*)4)goto _LL3E6;if(*((int*)_tmp5C4)!= 5)goto _LL3E6;_tmp5C5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5C4)->f2;if((int)_tmp5C5 != 0)goto _LL3E6;_LL3E5: return 1;_LL3E6:;_LL3E7: return
0;_LL3E3:;}void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return(void*)1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t2 == (void*)0)return(void*)1;{void*_tmp5C6=t2;
void*_tmp5C7;void*_tmp5C8;_LL3E9: if(_tmp5C6 <= (void*)4)goto _LL3ED;if(*((int*)
_tmp5C6)!= 5)goto _LL3EB;_tmp5C7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5C6)->f2;
if((int)_tmp5C7 != 2)goto _LL3EB;_LL3EA: goto _LL3EC;_LL3EB: if(*((int*)_tmp5C6)!= 5)
goto _LL3ED;_tmp5C8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5C6)->f2;if((int)
_tmp5C8 != 3)goto _LL3ED;_LL3EC: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return(void*)
1;goto _LL3E8;_LL3ED:;_LL3EE: goto _LL3E8;_LL3E8:;}{void*_tmp5C9=t1;struct Cyc_Absyn_PtrInfo
_tmp5CA;void*_tmp5CB;struct Cyc_Absyn_Tqual _tmp5CC;struct Cyc_Absyn_PtrAtts _tmp5CD;
void*_tmp5CE;struct Cyc_Absyn_Conref*_tmp5CF;struct Cyc_Absyn_Conref*_tmp5D0;
struct Cyc_Absyn_Conref*_tmp5D1;struct Cyc_Absyn_ArrayInfo _tmp5D2;void*_tmp5D3;
struct Cyc_Absyn_Tqual _tmp5D4;struct Cyc_Absyn_Exp*_tmp5D5;struct Cyc_Absyn_Conref*
_tmp5D6;struct Cyc_Absyn_Enumdecl*_tmp5D7;_LL3F0: if(_tmp5C9 <= (void*)4)goto _LL3F8;
if(*((int*)_tmp5C9)!= 4)goto _LL3F2;_tmp5CA=((struct Cyc_Absyn_PointerType_struct*)
_tmp5C9)->f1;_tmp5CB=(void*)_tmp5CA.elt_typ;_tmp5CC=_tmp5CA.elt_tq;_tmp5CD=
_tmp5CA.ptr_atts;_tmp5CE=(void*)_tmp5CD.rgn;_tmp5CF=_tmp5CD.nullable;_tmp5D0=
_tmp5CD.bounds;_tmp5D1=_tmp5CD.zero_term;_LL3F1:{void*_tmp5D8=t2;struct Cyc_Absyn_PtrInfo
_tmp5D9;void*_tmp5DA;struct Cyc_Absyn_Tqual _tmp5DB;struct Cyc_Absyn_PtrAtts _tmp5DC;
void*_tmp5DD;struct Cyc_Absyn_Conref*_tmp5DE;struct Cyc_Absyn_Conref*_tmp5DF;
struct Cyc_Absyn_Conref*_tmp5E0;_LL3FF: if(_tmp5D8 <= (void*)4)goto _LL401;if(*((int*)
_tmp5D8)!= 4)goto _LL401;_tmp5D9=((struct Cyc_Absyn_PointerType_struct*)_tmp5D8)->f1;
_tmp5DA=(void*)_tmp5D9.elt_typ;_tmp5DB=_tmp5D9.elt_tq;_tmp5DC=_tmp5D9.ptr_atts;
_tmp5DD=(void*)_tmp5DC.rgn;_tmp5DE=_tmp5DC.nullable;_tmp5DF=_tmp5DC.bounds;
_tmp5E0=_tmp5DC.zero_term;_LL400: {void*coercion=(void*)3;struct Cyc_List_List*
_tmp5E1=({struct Cyc_List_List*_tmp5F0=_cycalloc(sizeof(*_tmp5F0));_tmp5F0->hd=({
struct _tuple6*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));_tmp5F1->f1=t1;_tmp5F1->f2=t2;
_tmp5F1;});_tmp5F0->tl=0;_tmp5F0;});int _tmp5E2=Cyc_Tcutil_ptrsubtype(te,_tmp5E1,
_tmp5CB,_tmp5DA) && (!_tmp5CC.real_const  || _tmp5DB.real_const);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp5D1,_tmp5E0) || !((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp5E0);int _tmp5E3=_tmp5E2?0:((Cyc_Tcutil_bits_only(_tmp5CB) && Cyc_Tcutil_is_char_type(
_tmp5DA)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5E0)) && (_tmp5DB.real_const  || !_tmp5CC.real_const);int bounds_ok=Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,_tmp5D0,_tmp5DF);if(!bounds_ok  && !_tmp5E3){struct
_tuple6 _tmp5E5=({struct _tuple6 _tmp5E4;_tmp5E4.f1=Cyc_Absyn_conref_val(_tmp5D0);
_tmp5E4.f2=Cyc_Absyn_conref_val(_tmp5DF);_tmp5E4;});void*_tmp5E6;struct Cyc_Absyn_Exp*
_tmp5E7;void*_tmp5E8;struct Cyc_Absyn_Exp*_tmp5E9;void*_tmp5EA;void*_tmp5EB;void*
_tmp5EC;void*_tmp5ED;void*_tmp5EE;void*_tmp5EF;_LL404: _tmp5E6=_tmp5E5.f1;if(
_tmp5E6 <= (void*)2)goto _LL406;if(*((int*)_tmp5E6)!= 0)goto _LL406;_tmp5E7=((
struct Cyc_Absyn_Upper_b_struct*)_tmp5E6)->f1;_tmp5E8=_tmp5E5.f2;if(_tmp5E8 <= (
void*)2)goto _LL406;if(*((int*)_tmp5E8)!= 0)goto _LL406;_tmp5E9=((struct Cyc_Absyn_Upper_b_struct*)
_tmp5E8)->f1;_LL405: if(Cyc_Evexp_lte_const_exp(_tmp5E9,_tmp5E7))bounds_ok=1;goto
_LL403;_LL406: _tmp5EA=_tmp5E5.f1;if(_tmp5EA <= (void*)2)goto _LL408;if(*((int*)
_tmp5EA)!= 1)goto _LL408;_tmp5EB=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp5EA)->f1;_tmp5EC=_tmp5E5.f2;if(_tmp5EC <= (void*)2)goto _LL408;if(*((int*)
_tmp5EC)!= 1)goto _LL408;_tmp5ED=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp5EC)->f1;_LL407: bounds_ok=Cyc_Tcutil_unify(_tmp5EB,_tmp5ED);goto _LL403;
_LL408: _tmp5EE=_tmp5E5.f1;if(_tmp5EE <= (void*)2)goto _LL40A;if(*((int*)_tmp5EE)!= 
1)goto _LL40A;_LL409: goto _LL40B;_LL40A: _tmp5EF=_tmp5E5.f2;if(_tmp5EF <= (void*)2)
goto _LL40C;if(*((int*)_tmp5EF)!= 1)goto _LL40C;_LL40B: bounds_ok=0;goto _LL403;
_LL40C:;_LL40D: bounds_ok=1;goto _LL403;_LL403:;}if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp5CF) && !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp5DE))coercion=(void*)2;if(((bounds_ok  && zeroterm_ok)
 && (_tmp5E2  || _tmp5E3)) && (Cyc_Tcutil_unify(_tmp5CE,_tmp5DD) || Cyc_Tcenv_region_outlives(
te,_tmp5CE,_tmp5DD)))return coercion;else{return(void*)0;}}}_LL401:;_LL402: goto
_LL3FE;_LL3FE:;}return(void*)0;_LL3F2: if(*((int*)_tmp5C9)!= 7)goto _LL3F4;_tmp5D2=((
struct Cyc_Absyn_ArrayType_struct*)_tmp5C9)->f1;_tmp5D3=(void*)_tmp5D2.elt_type;
_tmp5D4=_tmp5D2.tq;_tmp5D5=_tmp5D2.num_elts;_tmp5D6=_tmp5D2.zero_term;_LL3F3:{
void*_tmp5F2=t2;struct Cyc_Absyn_ArrayInfo _tmp5F3;void*_tmp5F4;struct Cyc_Absyn_Tqual
_tmp5F5;struct Cyc_Absyn_Exp*_tmp5F6;struct Cyc_Absyn_Conref*_tmp5F7;_LL40F: if(
_tmp5F2 <= (void*)4)goto _LL411;if(*((int*)_tmp5F2)!= 7)goto _LL411;_tmp5F3=((
struct Cyc_Absyn_ArrayType_struct*)_tmp5F2)->f1;_tmp5F4=(void*)_tmp5F3.elt_type;
_tmp5F5=_tmp5F3.tq;_tmp5F6=_tmp5F3.num_elts;_tmp5F7=_tmp5F3.zero_term;_LL410: {
int okay;okay=((_tmp5D5 != 0  && _tmp5F6 != 0) && ((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp5D6,_tmp5F7)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp5F6,(
struct Cyc_Absyn_Exp*)_tmp5D5);return(okay  && Cyc_Tcutil_unify(_tmp5D3,_tmp5F4))
 && (!_tmp5D4.real_const  || _tmp5F5.real_const)?(void*)3:(void*)0;}_LL411:;
_LL412: return(void*)0;_LL40E:;}return(void*)0;_LL3F4: if(*((int*)_tmp5C9)!= 12)
goto _LL3F6;_tmp5D7=((struct Cyc_Absyn_EnumType_struct*)_tmp5C9)->f2;_LL3F5:{void*
_tmp5F8=t2;struct Cyc_Absyn_Enumdecl*_tmp5F9;_LL414: if(_tmp5F8 <= (void*)4)goto
_LL416;if(*((int*)_tmp5F8)!= 12)goto _LL416;_tmp5F9=((struct Cyc_Absyn_EnumType_struct*)
_tmp5F8)->f2;_LL415: if((_tmp5D7->fields != 0  && _tmp5F9->fields != 0) && ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp5D7->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5F9->fields))->v))
return(void*)1;goto _LL413;_LL416:;_LL417: goto _LL413;_LL413:;}goto _LL3F7;_LL3F6:
if(*((int*)_tmp5C9)!= 5)goto _LL3F8;_LL3F7: goto _LL3F9;_LL3F8: if((int)_tmp5C9 != 1)
goto _LL3FA;_LL3F9: goto _LL3FB;_LL3FA: if(_tmp5C9 <= (void*)4)goto _LL3FC;if(*((int*)
_tmp5C9)!= 6)goto _LL3FC;_LL3FB: return Cyc_Tcutil_coerceable(t2)?(void*)1:(void*)0;
_LL3FC:;_LL3FD: return(void*)0;_LL3EF:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,void*c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp5FA=Cyc_Absyn_copy_exp(e);(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*_tmp5FB=_cycalloc(
sizeof(*_tmp5FB));_tmp5FB[0]=({struct Cyc_Absyn_Cast_e_struct _tmp5FC;_tmp5FC.tag=
15;_tmp5FC.f1=(void*)t;_tmp5FC.f2=_tmp5FA;_tmp5FC.f3=0;_tmp5FC.f4=(void*)c;
_tmp5FC;});_tmp5FB;})));e->topt=({struct Cyc_Core_Opt*_tmp5FD=_cycalloc(sizeof(*
_tmp5FD));_tmp5FD->v=(void*)t;_tmp5FD;});}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*
e){void*_tmp5FE=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL419: if(_tmp5FE <= (void*)4)goto _LL425;if(*((int*)_tmp5FE)!= 5)goto _LL41B;
_LL41A: goto _LL41C;_LL41B: if(*((int*)_tmp5FE)!= 12)goto _LL41D;_LL41C: goto _LL41E;
_LL41D: if(*((int*)_tmp5FE)!= 13)goto _LL41F;_LL41E: goto _LL420;_LL41F: if(*((int*)
_tmp5FE)!= 17)goto _LL421;_LL420: goto _LL422;_LL421: if(*((int*)_tmp5FE)!= 14)goto
_LL423;_LL422: return 1;_LL423: if(*((int*)_tmp5FE)!= 0)goto _LL425;_LL424: return Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL425:;
_LL426: return 0;_LL418:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp5FF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL428: if((int)_tmp5FF != 1)goto _LL42A;_LL429: goto
_LL42B;_LL42A: if(_tmp5FF <= (void*)4)goto _LL42C;if(*((int*)_tmp5FF)!= 6)goto
_LL42C;_LL42B: return 1;_LL42C:;_LL42D: return 0;_LL427:;}}int Cyc_Tcutil_is_function_type(
void*t){void*_tmp600=Cyc_Tcutil_compress(t);_LL42F: if(_tmp600 <= (void*)4)goto
_LL431;if(*((int*)_tmp600)!= 8)goto _LL431;_LL430: return 1;_LL431:;_LL432: return 0;
_LL42E:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct _tuple6
_tmp602=({struct _tuple6 _tmp601;_tmp601.f1=t1;_tmp601.f2=t2;_tmp601;});void*
_tmp603;int _tmp604;void*_tmp605;int _tmp606;void*_tmp607;void*_tmp608;void*
_tmp609;void*_tmp60A;void*_tmp60B;void*_tmp60C;void*_tmp60D;void*_tmp60E;void*
_tmp60F;void*_tmp610;void*_tmp611;void*_tmp612;void*_tmp613;void*_tmp614;void*
_tmp615;void*_tmp616;void*_tmp617;void*_tmp618;void*_tmp619;void*_tmp61A;void*
_tmp61B;void*_tmp61C;void*_tmp61D;void*_tmp61E;void*_tmp61F;void*_tmp620;void*
_tmp621;void*_tmp622;void*_tmp623;void*_tmp624;_LL434: _tmp603=_tmp602.f1;if(
_tmp603 <= (void*)4)goto _LL436;if(*((int*)_tmp603)!= 6)goto _LL436;_tmp604=((
struct Cyc_Absyn_DoubleType_struct*)_tmp603)->f1;_tmp605=_tmp602.f2;if(_tmp605 <= (
void*)4)goto _LL436;if(*((int*)_tmp605)!= 6)goto _LL436;_tmp606=((struct Cyc_Absyn_DoubleType_struct*)
_tmp605)->f1;_LL435: if(_tmp604)return t1;else{return t2;}_LL436: _tmp607=_tmp602.f1;
if(_tmp607 <= (void*)4)goto _LL438;if(*((int*)_tmp607)!= 6)goto _LL438;_LL437:
return t1;_LL438: _tmp608=_tmp602.f2;if(_tmp608 <= (void*)4)goto _LL43A;if(*((int*)
_tmp608)!= 6)goto _LL43A;_LL439: return t2;_LL43A: _tmp609=_tmp602.f1;if((int)
_tmp609 != 1)goto _LL43C;_LL43B: goto _LL43D;_LL43C: _tmp60A=_tmp602.f2;if((int)
_tmp60A != 1)goto _LL43E;_LL43D: return(void*)1;_LL43E: _tmp60B=_tmp602.f1;if(
_tmp60B <= (void*)4)goto _LL440;if(*((int*)_tmp60B)!= 5)goto _LL440;_tmp60C=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp60B)->f1;if((int)_tmp60C != 1)goto _LL440;
_tmp60D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp60B)->f2;if((int)_tmp60D != 
4)goto _LL440;_LL43F: goto _LL441;_LL440: _tmp60E=_tmp602.f2;if(_tmp60E <= (void*)4)
goto _LL442;if(*((int*)_tmp60E)!= 5)goto _LL442;_tmp60F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp60E)->f1;if((int)_tmp60F != 1)goto _LL442;_tmp610=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp60E)->f2;if((int)_tmp610 != 4)goto _LL442;_LL441: return Cyc_Absyn_ulonglong_typ;
_LL442: _tmp611=_tmp602.f1;if(_tmp611 <= (void*)4)goto _LL444;if(*((int*)_tmp611)!= 
5)goto _LL444;_tmp612=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp611)->f2;if((
int)_tmp612 != 4)goto _LL444;_LL443: goto _LL445;_LL444: _tmp613=_tmp602.f2;if(
_tmp613 <= (void*)4)goto _LL446;if(*((int*)_tmp613)!= 5)goto _LL446;_tmp614=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp613)->f2;if((int)_tmp614 != 4)goto _LL446;
_LL445: return Cyc_Absyn_slonglong_typ;_LL446: _tmp615=_tmp602.f1;if(_tmp615 <= (
void*)4)goto _LL448;if(*((int*)_tmp615)!= 5)goto _LL448;_tmp616=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp615)->f1;if((int)_tmp616 != 1)goto _LL448;_tmp617=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp615)->f2;if((int)_tmp617 != 3)goto _LL448;_LL447: goto _LL449;_LL448: _tmp618=
_tmp602.f2;if(_tmp618 <= (void*)4)goto _LL44A;if(*((int*)_tmp618)!= 5)goto _LL44A;
_tmp619=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp618)->f1;if((int)_tmp619 != 
1)goto _LL44A;_tmp61A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp618)->f2;if((
int)_tmp61A != 3)goto _LL44A;_LL449: return Cyc_Absyn_ulong_typ;_LL44A: _tmp61B=
_tmp602.f1;if(_tmp61B <= (void*)4)goto _LL44C;if(*((int*)_tmp61B)!= 5)goto _LL44C;
_tmp61C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp61B)->f1;if((int)_tmp61C != 
1)goto _LL44C;_tmp61D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp61B)->f2;if((
int)_tmp61D != 2)goto _LL44C;_LL44B: goto _LL44D;_LL44C: _tmp61E=_tmp602.f2;if(
_tmp61E <= (void*)4)goto _LL44E;if(*((int*)_tmp61E)!= 5)goto _LL44E;_tmp61F=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp61E)->f1;if((int)_tmp61F != 1)goto _LL44E;
_tmp620=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp61E)->f2;if((int)_tmp620 != 
2)goto _LL44E;_LL44D: return Cyc_Absyn_uint_typ;_LL44E: _tmp621=_tmp602.f1;if(
_tmp621 <= (void*)4)goto _LL450;if(*((int*)_tmp621)!= 5)goto _LL450;_tmp622=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp621)->f2;if((int)_tmp622 != 3)goto _LL450;
_LL44F: goto _LL451;_LL450: _tmp623=_tmp602.f2;if(_tmp623 <= (void*)4)goto _LL452;if(*((
int*)_tmp623)!= 5)goto _LL452;_tmp624=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp623)->f2;if((int)_tmp624 != 3)goto _LL452;_LL451: return Cyc_Absyn_slong_typ;
_LL452:;_LL453: return Cyc_Absyn_sint_typ;_LL433:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp625=(void*)e->r;struct Cyc_Core_Opt*_tmp626;
_LL455: if(*((int*)_tmp625)!= 4)goto _LL457;_tmp626=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp625)->f2;if(_tmp626 != 0)goto _LL457;_LL456:({void*_tmp627[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp628="assignment in test";_tag_dynforward(_tmp628,sizeof(
char),_get_zero_arr_size(_tmp628,19));}),_tag_dynforward(_tmp627,sizeof(void*),0));});
goto _LL454;_LL457:;_LL458: goto _LL454;_LL454:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple6 _tmp62A=({struct _tuple6 _tmp629;_tmp629.f1=c1;_tmp629.f2=c2;_tmp629;});
void*_tmp62B;void*_tmp62C;void*_tmp62D;void*_tmp62E;void*_tmp62F;struct Cyc_Core_Opt*
_tmp630;struct Cyc_Core_Opt**_tmp631;void*_tmp632;struct Cyc_Core_Opt*_tmp633;
struct Cyc_Core_Opt**_tmp634;void*_tmp635;struct Cyc_Core_Opt*_tmp636;struct Cyc_Core_Opt**
_tmp637;void*_tmp638;void*_tmp639;void*_tmp63A;void*_tmp63B;void*_tmp63C;void*
_tmp63D;struct Cyc_Core_Opt*_tmp63E;struct Cyc_Core_Opt**_tmp63F;void*_tmp640;void*
_tmp641;struct Cyc_Core_Opt*_tmp642;struct Cyc_Core_Opt**_tmp643;void*_tmp644;void*
_tmp645;struct Cyc_Core_Opt*_tmp646;struct Cyc_Core_Opt**_tmp647;void*_tmp648;
_LL45A: _tmp62B=_tmp62A.f1;if(*((int*)_tmp62B)!= 0)goto _LL45C;_tmp62C=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp62B)->f1;_tmp62D=_tmp62A.f2;if(*((int*)_tmp62D)
!= 0)goto _LL45C;_tmp62E=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp62D)->f1;
_LL45B: return _tmp62C == _tmp62E;_LL45C: _tmp62F=_tmp62A.f2;if(*((int*)_tmp62F)!= 1)
goto _LL45E;_tmp630=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp62F)->f1;_tmp631=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp62F)->f1;_LL45D:*
_tmp631=({struct Cyc_Core_Opt*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->v=(
void*)c1;_tmp649;});return 1;_LL45E: _tmp632=_tmp62A.f1;if(*((int*)_tmp632)!= 1)
goto _LL460;_tmp633=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp632)->f1;_tmp634=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp632)->f1;_LL45F:*
_tmp634=({struct Cyc_Core_Opt*_tmp64A=_cycalloc(sizeof(*_tmp64A));_tmp64A->v=(
void*)c2;_tmp64A;});return 1;_LL460: _tmp635=_tmp62A.f1;if(*((int*)_tmp635)!= 2)
goto _LL462;_tmp636=((struct Cyc_Absyn_Less_kb_struct*)_tmp635)->f1;_tmp637=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp635)->f1;_tmp638=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp635)->f2;_tmp639=_tmp62A.f2;if(*((
int*)_tmp639)!= 0)goto _LL462;_tmp63A=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp639)->f1;_LL461: if(Cyc_Tcutil_kind_leq(_tmp63A,_tmp638)){*_tmp637=({struct
Cyc_Core_Opt*_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B->v=(void*)c2;_tmp64B;});
return 1;}else{return 0;}_LL462: _tmp63B=_tmp62A.f1;if(*((int*)_tmp63B)!= 0)goto
_LL464;_tmp63C=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp63B)->f1;_tmp63D=
_tmp62A.f2;if(*((int*)_tmp63D)!= 2)goto _LL464;_tmp63E=((struct Cyc_Absyn_Less_kb_struct*)
_tmp63D)->f1;_tmp63F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp63D)->f1;_tmp640=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp63D)->f2;
_LL463: if(Cyc_Tcutil_kind_leq(_tmp63C,_tmp640)){*_tmp63F=({struct Cyc_Core_Opt*
_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C->v=(void*)c1;_tmp64C;});return 1;}
else{return 0;}_LL464: _tmp641=_tmp62A.f1;if(*((int*)_tmp641)!= 2)goto _LL459;
_tmp642=((struct Cyc_Absyn_Less_kb_struct*)_tmp641)->f1;_tmp643=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp641)->f1;_tmp644=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp641)->f2;_tmp645=_tmp62A.f2;if(*((int*)_tmp645)!= 2)goto _LL459;_tmp646=((
struct Cyc_Absyn_Less_kb_struct*)_tmp645)->f1;_tmp647=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp645)->f1;_tmp648=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp645)->f2;_LL465: if(Cyc_Tcutil_kind_leq(_tmp644,_tmp648)){*_tmp647=({struct
Cyc_Core_Opt*_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D->v=(void*)c1;_tmp64D;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp648,_tmp644)){*_tmp643=({struct Cyc_Core_Opt*
_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E->v=(void*)c2;_tmp64E;});return 1;}
else{return 0;}}_LL459:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp64F=_cycalloc_atomic(sizeof(*_tmp64F));_tmp64F[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp64F;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _dynforward_ptr s=(struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp654;_tmp654.tag=1;_tmp654.f1=(
unsigned long)i;{void*_tmp652[1]={& _tmp654};Cyc_aprintf(({const char*_tmp653="#%d";
_tag_dynforward(_tmp653,sizeof(char),_get_zero_arr_size(_tmp653,4));}),
_tag_dynforward(_tmp652,sizeof(void*),1));}});return({struct Cyc_Absyn_Tvar*
_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650->name=({struct _dynforward_ptr*
_tmp651=_cycalloc(sizeof(struct _dynforward_ptr)* 1);_tmp651[0]=s;_tmp651;});
_tmp650->identity=0;_tmp650->kind=(void*)k;_tmp650;});}int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*t){struct _dynforward_ptr _tmp655=*t->name;return*((const char*)
_check_dynforward_subscript(_tmp655,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_String_pa_struct _tmp658;_tmp658.tag=0;
_tmp658.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*t->name);{void*
_tmp656[1]={& _tmp658};Cyc_printf(({const char*_tmp657="%s";_tag_dynforward(
_tmp657,sizeof(char),_get_zero_arr_size(_tmp657,3));}),_tag_dynforward(_tmp656,
sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(t))return;{struct
_dynforward_ptr _tmp659=Cyc_strconcat(({const char*_tmp65E="`";_tag_dynforward(
_tmp65E,sizeof(char),_get_zero_arr_size(_tmp65E,2));}),(struct _dynforward_ptr)*t->name);({
struct _dynforward_ptr _tmp65A=_dynforward_ptr_plus(_tmp659,sizeof(char),1);char
_tmp65B=*((char*)_check_dynforward_subscript(_tmp65A,sizeof(char),0));char
_tmp65C='t';if(_get_dynforward_size(_tmp65A,sizeof(char))== 1  && (_tmp65B == '\000'
 && _tmp65C != '\000'))_throw_arraybounds();*((char*)_tmp65A.curr)=_tmp65C;});t->name=({
struct _dynforward_ptr*_tmp65D=_cycalloc(sizeof(struct _dynforward_ptr)* 1);
_tmp65D[0]=(struct _dynforward_ptr)_tmp659;_tmp65D;});}}struct _tuple13{struct
_dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple2*Cyc_Tcutil_fndecl2typ_f(
struct _tuple13*x){return({struct _tuple2*_tmp65F=_cycalloc(sizeof(*_tmp65F));
_tmp65F->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp660=_cycalloc(sizeof(*
_tmp660));_tmp660->v=(*x).f1;_tmp660;});_tmp65F->f2=(*x).f2;_tmp65F->f3=(*x).f3;
_tmp65F;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp661=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))_tmp661=({
struct Cyc_List_List*_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662->hd=(void*)((
void*)atts->hd);_tmp662->tl=_tmp661;_tmp662;});}}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp663=_cycalloc(sizeof(*_tmp663));_tmp663[0]=({struct Cyc_Absyn_FnType_struct
_tmp664;_tmp664.tag=8;_tmp664.f1=({struct Cyc_Absyn_FnInfo _tmp665;_tmp665.tvars=
fd->tvs;_tmp665.effect=fd->effect;_tmp665.ret_typ=(void*)((void*)fd->ret_type);
_tmp665.args=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple13*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp665.c_varargs=
fd->c_varargs;_tmp665.cyc_varargs=fd->cyc_varargs;_tmp665.rgn_po=fd->rgn_po;
_tmp665.attributes=_tmp661;_tmp665;});_tmp664;});_tmp663;});}return(void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}struct _tuple14{void*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple14*
t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*t).f2;}static
struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({struct _tuple4*
_tmp666=_cycalloc(sizeof(*_tmp666));_tmp666->f1=(*pr).f1;_tmp666->f2=t;_tmp666;});}
struct _tuple15{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple16{
struct _tuple15*f1;void*f2;};static struct _tuple16*Cyc_Tcutil_substitute_f1(struct
_RegionHandle*rgn,struct _tuple2*y){return({struct _tuple16*_tmp667=_region_malloc(
rgn,sizeof(*_tmp667));_tmp667->f1=({struct _tuple15*_tmp668=_region_malloc(rgn,
sizeof(*_tmp668));_tmp668->f1=(*y).f1;_tmp668->f2=(*y).f2;_tmp668;});_tmp667->f2=(*
y).f3;_tmp667;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(struct _tuple16*w){
struct _tuple15*_tmp66A;void*_tmp66B;struct _tuple16 _tmp669=*w;_tmp66A=_tmp669.f1;
_tmp66B=_tmp669.f2;{struct Cyc_Core_Opt*_tmp66D;struct Cyc_Absyn_Tqual _tmp66E;
struct _tuple15 _tmp66C=*_tmp66A;_tmp66D=_tmp66C.f1;_tmp66E=_tmp66C.f2;return({
struct _tuple2*_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F->f1=_tmp66D;_tmp66F->f2=
_tmp66E;_tmp66F->f3=_tmp66B;_tmp66F;});}}static void*Cyc_Tcutil_field_type(struct
Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*_tmp670=
_cycalloc(sizeof(*_tmp670));_tmp670->name=f->name;_tmp670->tq=f->tq;_tmp670->type=(
void*)t;_tmp670->width=f->width;_tmp670->attributes=f->attributes;_tmp670;});}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,void*t){void*_tmp671=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp672;struct Cyc_Absyn_AggrInfo _tmp673;void*_tmp674;struct Cyc_List_List*_tmp675;
struct Cyc_Absyn_TunionInfo _tmp676;void*_tmp677;struct Cyc_List_List*_tmp678;void*
_tmp679;struct Cyc_Absyn_TunionFieldInfo _tmp67A;void*_tmp67B;struct Cyc_List_List*
_tmp67C;struct _tuple1*_tmp67D;struct Cyc_List_List*_tmp67E;struct Cyc_Absyn_Typedefdecl*
_tmp67F;void**_tmp680;struct Cyc_Absyn_ArrayInfo _tmp681;void*_tmp682;struct Cyc_Absyn_Tqual
_tmp683;struct Cyc_Absyn_Exp*_tmp684;struct Cyc_Absyn_Conref*_tmp685;struct Cyc_Position_Segment*
_tmp686;struct Cyc_Absyn_PtrInfo _tmp687;void*_tmp688;struct Cyc_Absyn_Tqual _tmp689;
struct Cyc_Absyn_PtrAtts _tmp68A;void*_tmp68B;struct Cyc_Absyn_Conref*_tmp68C;
struct Cyc_Absyn_Conref*_tmp68D;struct Cyc_Absyn_Conref*_tmp68E;struct Cyc_Absyn_FnInfo
_tmp68F;struct Cyc_List_List*_tmp690;struct Cyc_Core_Opt*_tmp691;void*_tmp692;
struct Cyc_List_List*_tmp693;int _tmp694;struct Cyc_Absyn_VarargInfo*_tmp695;struct
Cyc_List_List*_tmp696;struct Cyc_List_List*_tmp697;struct Cyc_List_List*_tmp698;
void*_tmp699;struct Cyc_List_List*_tmp69A;struct Cyc_Core_Opt*_tmp69B;void*_tmp69C;
void*_tmp69D;void*_tmp69E;void*_tmp69F;void*_tmp6A0;struct Cyc_List_List*_tmp6A1;
_LL467: if(_tmp671 <= (void*)4)goto _LL489;if(*((int*)_tmp671)!= 1)goto _LL469;
_tmp672=((struct Cyc_Absyn_VarType_struct*)_tmp671)->f1;_LL468: {struct
_handler_cons _tmp6A2;_push_handler(& _tmp6A2);{int _tmp6A4=0;if(setjmp(_tmp6A2.handler))
_tmp6A4=1;if(!_tmp6A4){{void*_tmp6A5=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(
Cyc_Absyn_tvar_cmp,inst,_tmp672);_npop_handler(0);return _tmp6A5;};_pop_handler();}
else{void*_tmp6A3=(void*)_exn_thrown;void*_tmp6A7=_tmp6A3;_LL49C: if(_tmp6A7 != 
Cyc_Core_Not_found)goto _LL49E;_LL49D: return t;_LL49E:;_LL49F:(void)_throw(_tmp6A7);
_LL49B:;}}}_LL469: if(*((int*)_tmp671)!= 10)goto _LL46B;_tmp673=((struct Cyc_Absyn_AggrType_struct*)
_tmp671)->f1;_tmp674=(void*)_tmp673.aggr_info;_tmp675=_tmp673.targs;_LL46A: {
struct Cyc_List_List*_tmp6A8=Cyc_Tcutil_substs(rgn,inst,_tmp675);return _tmp6A8 == 
_tmp675?t:(void*)({struct Cyc_Absyn_AggrType_struct*_tmp6A9=_cycalloc(sizeof(*
_tmp6A9));_tmp6A9[0]=({struct Cyc_Absyn_AggrType_struct _tmp6AA;_tmp6AA.tag=10;
_tmp6AA.f1=({struct Cyc_Absyn_AggrInfo _tmp6AB;_tmp6AB.aggr_info=(void*)_tmp674;
_tmp6AB.targs=_tmp6A8;_tmp6AB;});_tmp6AA;});_tmp6A9;});}_LL46B: if(*((int*)
_tmp671)!= 2)goto _LL46D;_tmp676=((struct Cyc_Absyn_TunionType_struct*)_tmp671)->f1;
_tmp677=(void*)_tmp676.tunion_info;_tmp678=_tmp676.targs;_tmp679=(void*)_tmp676.rgn;
_LL46C: {struct Cyc_List_List*_tmp6AC=Cyc_Tcutil_substs(rgn,inst,_tmp678);void*
_tmp6AD=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp679);return _tmp6AC == _tmp678  && 
_tmp6AD == _tmp679?t:(void*)({struct Cyc_Absyn_TunionType_struct*_tmp6AE=_cycalloc(
sizeof(*_tmp6AE));_tmp6AE[0]=({struct Cyc_Absyn_TunionType_struct _tmp6AF;_tmp6AF.tag=
2;_tmp6AF.f1=({struct Cyc_Absyn_TunionInfo _tmp6B0;_tmp6B0.tunion_info=(void*)
_tmp677;_tmp6B0.targs=_tmp6AC;_tmp6B0.rgn=(void*)_tmp6AD;_tmp6B0;});_tmp6AF;});
_tmp6AE;});}_LL46D: if(*((int*)_tmp671)!= 3)goto _LL46F;_tmp67A=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp671)->f1;_tmp67B=(void*)_tmp67A.field_info;_tmp67C=_tmp67A.targs;_LL46E: {
struct Cyc_List_List*_tmp6B1=Cyc_Tcutil_substs(rgn,inst,_tmp67C);return _tmp6B1 == 
_tmp67C?t:(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp6B2=_cycalloc(
sizeof(*_tmp6B2));_tmp6B2[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp6B3;
_tmp6B3.tag=3;_tmp6B3.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp6B4;_tmp6B4.field_info=(
void*)_tmp67B;_tmp6B4.targs=_tmp6B1;_tmp6B4;});_tmp6B3;});_tmp6B2;});}_LL46F: if(*((
int*)_tmp671)!= 16)goto _LL471;_tmp67D=((struct Cyc_Absyn_TypedefType_struct*)
_tmp671)->f1;_tmp67E=((struct Cyc_Absyn_TypedefType_struct*)_tmp671)->f2;_tmp67F=((
struct Cyc_Absyn_TypedefType_struct*)_tmp671)->f3;_tmp680=((struct Cyc_Absyn_TypedefType_struct*)
_tmp671)->f4;_LL470: {struct Cyc_List_List*_tmp6B5=Cyc_Tcutil_substs(rgn,inst,
_tmp67E);return _tmp6B5 == _tmp67E?t:(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp6B6=_cycalloc(sizeof(*_tmp6B6));_tmp6B6[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp6B7;_tmp6B7.tag=16;_tmp6B7.f1=_tmp67D;_tmp6B7.f2=_tmp6B5;_tmp6B7.f3=_tmp67F;
_tmp6B7.f4=_tmp680;_tmp6B7;});_tmp6B6;});}_LL471: if(*((int*)_tmp671)!= 7)goto
_LL473;_tmp681=((struct Cyc_Absyn_ArrayType_struct*)_tmp671)->f1;_tmp682=(void*)
_tmp681.elt_type;_tmp683=_tmp681.tq;_tmp684=_tmp681.num_elts;_tmp685=_tmp681.zero_term;
_tmp686=_tmp681.zt_loc;_LL472: {void*_tmp6B8=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp682);return _tmp6B8 == _tmp682?t:(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp6BA;_tmp6BA.tag=7;_tmp6BA.f1=({struct Cyc_Absyn_ArrayInfo _tmp6BB;_tmp6BB.elt_type=(
void*)_tmp6B8;_tmp6BB.tq=_tmp683;_tmp6BB.num_elts=_tmp684;_tmp6BB.zero_term=
_tmp685;_tmp6BB.zt_loc=_tmp686;_tmp6BB;});_tmp6BA;});_tmp6B9;});}_LL473: if(*((
int*)_tmp671)!= 4)goto _LL475;_tmp687=((struct Cyc_Absyn_PointerType_struct*)
_tmp671)->f1;_tmp688=(void*)_tmp687.elt_typ;_tmp689=_tmp687.elt_tq;_tmp68A=
_tmp687.ptr_atts;_tmp68B=(void*)_tmp68A.rgn;_tmp68C=_tmp68A.nullable;_tmp68D=
_tmp68A.bounds;_tmp68E=_tmp68A.zero_term;_LL474: {void*_tmp6BC=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp688);void*_tmp6BD=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp68B);struct
Cyc_Absyn_Conref*_tmp6BE=_tmp68D;{void*_tmp6BF=(void*)(Cyc_Absyn_compress_conref(
_tmp68D))->v;void*_tmp6C0;void*_tmp6C1;_LL4A1: if(_tmp6BF <= (void*)1)goto _LL4A3;
if(*((int*)_tmp6BF)!= 0)goto _LL4A3;_tmp6C0=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp6BF)->f1;if(_tmp6C0 <= (void*)2)goto _LL4A3;if(*((int*)_tmp6C0)!= 1)goto _LL4A3;
_tmp6C1=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp6C0)->f1;_LL4A2: {void*
_tmp6C2=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6C1);if(_tmp6C1 != _tmp6C2)_tmp6BE=
Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp6C3=
_cycalloc(sizeof(*_tmp6C3));_tmp6C3[0]=({struct Cyc_Absyn_AbsUpper_b_struct
_tmp6C4;_tmp6C4.tag=1;_tmp6C4.f1=(void*)_tmp6C2;_tmp6C4;});_tmp6C3;}));goto
_LL4A0;}_LL4A3:;_LL4A4: goto _LL4A0;_LL4A0:;}if((_tmp6BC == _tmp688  && _tmp6BD == 
_tmp68B) && _tmp6BE == _tmp68D)return t;return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5[0]=({struct Cyc_Absyn_PointerType_struct
_tmp6C6;_tmp6C6.tag=4;_tmp6C6.f1=({struct Cyc_Absyn_PtrInfo _tmp6C7;_tmp6C7.elt_typ=(
void*)_tmp6BC;_tmp6C7.elt_tq=_tmp689;_tmp6C7.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp6C8;_tmp6C8.rgn=(void*)_tmp6BD;_tmp6C8.nullable=_tmp68C;_tmp6C8.bounds=
_tmp6BE;_tmp6C8.zero_term=_tmp68E;_tmp6C8.ptrloc=0;_tmp6C8;});_tmp6C7;});_tmp6C6;});
_tmp6C5;});}_LL475: if(*((int*)_tmp671)!= 8)goto _LL477;_tmp68F=((struct Cyc_Absyn_FnType_struct*)
_tmp671)->f1;_tmp690=_tmp68F.tvars;_tmp691=_tmp68F.effect;_tmp692=(void*)_tmp68F.ret_typ;
_tmp693=_tmp68F.args;_tmp694=_tmp68F.c_varargs;_tmp695=_tmp68F.cyc_varargs;
_tmp696=_tmp68F.rgn_po;_tmp697=_tmp68F.attributes;_LL476:{struct Cyc_List_List*
_tmp6C9=_tmp690;for(0;_tmp6C9 != 0;_tmp6C9=_tmp6C9->tl){inst=({struct Cyc_List_List*
_tmp6CA=_region_malloc(rgn,sizeof(*_tmp6CA));_tmp6CA->hd=({struct _tuple8*_tmp6CB=
_region_malloc(rgn,sizeof(*_tmp6CB));_tmp6CB->f1=(struct Cyc_Absyn_Tvar*)_tmp6C9->hd;
_tmp6CB->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp6CC=_cycalloc(sizeof(*
_tmp6CC));_tmp6CC[0]=({struct Cyc_Absyn_VarType_struct _tmp6CD;_tmp6CD.tag=1;
_tmp6CD.f1=(struct Cyc_Absyn_Tvar*)_tmp6C9->hd;_tmp6CD;});_tmp6CC;});_tmp6CB;});
_tmp6CA->tl=inst;_tmp6CA;});}}{struct Cyc_List_List*_tmp6CF;struct Cyc_List_List*
_tmp6D0;struct _tuple0 _tmp6CE=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple16*(*f)(struct _RegionHandle*,struct _tuple2*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmp693));_tmp6CF=_tmp6CE.f1;_tmp6D0=_tmp6CE.f2;{struct Cyc_List_List*_tmp6D1=
Cyc_Tcutil_substs(rgn,inst,_tmp6D0);struct Cyc_List_List*_tmp6D2=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp6CF,_tmp6D1));struct Cyc_Core_Opt*
eff2;if(_tmp691 == 0)eff2=0;else{void*_tmp6D3=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp691->v);if(_tmp6D3 == (void*)_tmp691->v)eff2=_tmp691;else{eff2=({struct
Cyc_Core_Opt*_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4->v=(void*)_tmp6D3;
_tmp6D4;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp695 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp6D6;struct Cyc_Absyn_Tqual _tmp6D7;void*
_tmp6D8;int _tmp6D9;struct Cyc_Absyn_VarargInfo _tmp6D5=*_tmp695;_tmp6D6=_tmp6D5.name;
_tmp6D7=_tmp6D5.tq;_tmp6D8=(void*)_tmp6D5.type;_tmp6D9=_tmp6D5.inject;{void*
_tmp6DA=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D8);if(_tmp6DA == _tmp6D8)
cyc_varargs2=_tmp695;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp6DB=
_cycalloc(sizeof(*_tmp6DB));_tmp6DB->name=_tmp6D6;_tmp6DB->tq=_tmp6D7;_tmp6DB->type=(
void*)_tmp6DA;_tmp6DB->inject=_tmp6D9;_tmp6DB;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp6DD;struct Cyc_List_List*_tmp6DE;struct _tuple0 _tmp6DC=Cyc_List_rsplit(
rgn,rgn,_tmp696);_tmp6DD=_tmp6DC.f1;_tmp6DE=_tmp6DC.f2;{struct Cyc_List_List*
_tmp6DF=Cyc_Tcutil_substs(rgn,inst,_tmp6DD);struct Cyc_List_List*_tmp6E0=Cyc_Tcutil_substs(
rgn,inst,_tmp6DE);if(_tmp6DF == _tmp6DD  && _tmp6E0 == _tmp6DE)rgn_po2=_tmp696;
else{rgn_po2=Cyc_List_zip(_tmp6DF,_tmp6E0);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp6E1=_cycalloc(sizeof(*_tmp6E1));_tmp6E1[0]=({struct Cyc_Absyn_FnType_struct
_tmp6E2;_tmp6E2.tag=8;_tmp6E2.f1=({struct Cyc_Absyn_FnInfo _tmp6E3;_tmp6E3.tvars=
_tmp690;_tmp6E3.effect=eff2;_tmp6E3.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp692);_tmp6E3.args=_tmp6D2;_tmp6E3.c_varargs=_tmp694;_tmp6E3.cyc_varargs=
cyc_varargs2;_tmp6E3.rgn_po=rgn_po2;_tmp6E3.attributes=_tmp697;_tmp6E3;});
_tmp6E2;});_tmp6E1;});}}}}}_LL477: if(*((int*)_tmp671)!= 9)goto _LL479;_tmp698=((
struct Cyc_Absyn_TupleType_struct*)_tmp671)->f1;_LL478: {struct Cyc_List_List*
_tmp6E4=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple4*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmp698);struct Cyc_List_List*
_tmp6E5=Cyc_Tcutil_substs(rgn,inst,_tmp6E4);if(_tmp6E5 == _tmp6E4)return t;{struct
Cyc_List_List*_tmp6E6=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct _tuple4*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmp698,_tmp6E5);return(void*)({struct Cyc_Absyn_TupleType_struct*_tmp6E7=
_cycalloc(sizeof(*_tmp6E7));_tmp6E7[0]=({struct Cyc_Absyn_TupleType_struct _tmp6E8;
_tmp6E8.tag=9;_tmp6E8.f1=_tmp6E6;_tmp6E8;});_tmp6E7;});}}_LL479: if(*((int*)
_tmp671)!= 11)goto _LL47B;_tmp699=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp671)->f1;_tmp69A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp671)->f2;_LL47A: {
struct Cyc_List_List*_tmp6E9=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp69A);struct Cyc_List_List*_tmp6EA=Cyc_Tcutil_substs(rgn,inst,_tmp6E9);if(
_tmp6EA == _tmp6E9)return t;{struct Cyc_List_List*_tmp6EB=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp69A,
_tmp6EA);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp6EC=_cycalloc(
sizeof(*_tmp6EC));_tmp6EC[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp6ED;
_tmp6ED.tag=11;_tmp6ED.f1=(void*)_tmp699;_tmp6ED.f2=_tmp6EB;_tmp6ED;});_tmp6EC;});}}
_LL47B: if(*((int*)_tmp671)!= 0)goto _LL47D;_tmp69B=((struct Cyc_Absyn_Evar_struct*)
_tmp671)->f2;_LL47C: if(_tmp69B != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)
_tmp69B->v);else{return t;}_LL47D: if(*((int*)_tmp671)!= 15)goto _LL47F;_tmp69C=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp671)->f1;_LL47E: {void*_tmp6EE=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp69C);return _tmp6EE == _tmp69C?t:(void*)({
struct Cyc_Absyn_RgnHandleType_struct*_tmp6EF=_cycalloc(sizeof(*_tmp6EF));_tmp6EF[
0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp6F0;_tmp6F0.tag=15;_tmp6F0.f1=(void*)
_tmp6EE;_tmp6F0;});_tmp6EF;});}_LL47F: if(*((int*)_tmp671)!= 14)goto _LL481;
_tmp69D=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp671)->f1;_LL480: {void*
_tmp6F1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp69D);return _tmp6F1 == _tmp69D?t:(void*)({
struct Cyc_Absyn_SizeofType_struct*_tmp6F2=_cycalloc(sizeof(*_tmp6F2));_tmp6F2[0]=({
struct Cyc_Absyn_SizeofType_struct _tmp6F3;_tmp6F3.tag=14;_tmp6F3.f1=(void*)
_tmp6F1;_tmp6F3;});_tmp6F2;});}_LL481: if(*((int*)_tmp671)!= 17)goto _LL483;
_tmp69E=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp671)->f1;_LL482: {void*
_tmp6F4=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp69E);return _tmp6F4 == _tmp69E?t:(void*)({
struct Cyc_Absyn_TagType_struct*_tmp6F5=_cycalloc(sizeof(*_tmp6F5));_tmp6F5[0]=({
struct Cyc_Absyn_TagType_struct _tmp6F6;_tmp6F6.tag=17;_tmp6F6.f1=(void*)_tmp6F4;
_tmp6F6;});_tmp6F5;});}_LL483: if(*((int*)_tmp671)!= 18)goto _LL485;_LL484: goto
_LL486;_LL485: if(*((int*)_tmp671)!= 12)goto _LL487;_LL486: goto _LL488;_LL487: if(*((
int*)_tmp671)!= 13)goto _LL489;_LL488: goto _LL48A;_LL489: if((int)_tmp671 != 0)goto
_LL48B;_LL48A: goto _LL48C;_LL48B: if(_tmp671 <= (void*)4)goto _LL48D;if(*((int*)
_tmp671)!= 5)goto _LL48D;_LL48C: goto _LL48E;_LL48D: if((int)_tmp671 != 1)goto _LL48F;
_LL48E: goto _LL490;_LL48F: if(_tmp671 <= (void*)4)goto _LL491;if(*((int*)_tmp671)!= 
6)goto _LL491;_LL490: goto _LL492;_LL491: if((int)_tmp671 != 3)goto _LL493;_LL492: goto
_LL494;_LL493: if((int)_tmp671 != 2)goto _LL495;_LL494: return t;_LL495: if(_tmp671 <= (
void*)4)goto _LL497;if(*((int*)_tmp671)!= 21)goto _LL497;_tmp69F=(void*)((struct
Cyc_Absyn_RgnsEff_struct*)_tmp671)->f1;_LL496: {void*_tmp6F7=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp69F);return _tmp6F7 == _tmp69F?t:(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp6F8=_cycalloc(sizeof(*_tmp6F8));_tmp6F8[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp6F9;_tmp6F9.tag=21;_tmp6F9.f1=(void*)_tmp6F7;_tmp6F9;});_tmp6F8;});}_LL497:
if(_tmp671 <= (void*)4)goto _LL499;if(*((int*)_tmp671)!= 19)goto _LL499;_tmp6A0=(
void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp671)->f1;_LL498: {void*_tmp6FA=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6A0);return _tmp6FA == _tmp6A0?t:(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp6FB=_cycalloc(sizeof(*_tmp6FB));_tmp6FB[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp6FC;_tmp6FC.tag=19;_tmp6FC.f1=(void*)_tmp6FA;_tmp6FC;});_tmp6FB;});}_LL499:
if(_tmp671 <= (void*)4)goto _LL466;if(*((int*)_tmp671)!= 20)goto _LL466;_tmp6A1=((
struct Cyc_Absyn_JoinEff_struct*)_tmp671)->f1;_LL49A: {struct Cyc_List_List*
_tmp6FD=Cyc_Tcutil_substs(rgn,inst,_tmp6A1);return _tmp6FD == _tmp6A1?t:(void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp6FE=_cycalloc(sizeof(*_tmp6FE));_tmp6FE[0]=({
struct Cyc_Absyn_JoinEff_struct _tmp6FF;_tmp6FF.tag=20;_tmp6FF.f1=_tmp6FD;_tmp6FF;});
_tmp6FE;});}_LL466:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 0)
return 0;{void*_tmp700=(void*)ts->hd;struct Cyc_List_List*_tmp701=ts->tl;void*
_tmp702=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp700);struct Cyc_List_List*_tmp703=Cyc_Tcutil_substs(
rgn,inst,_tmp701);if(_tmp700 == _tmp702  && _tmp701 == _tmp703)return ts;return(
struct Cyc_List_List*)((struct Cyc_List_List*)({struct Cyc_List_List*_tmp704=
_cycalloc(sizeof(*_tmp704));_tmp704->hd=(void*)_tmp702;_tmp704->tl=_tmp703;
_tmp704;}));}}extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){void*k=Cyc_Tcutil_tvar_kind(tv);
return({struct _tuple8*_tmp705=_cycalloc(sizeof(*_tmp705));_tmp705->f1=tv;_tmp705->f2=
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp706=_cycalloc(sizeof(*_tmp706));
_tmp706->v=(void*)k;_tmp706;}),({struct Cyc_Core_Opt*_tmp707=_cycalloc(sizeof(*
_tmp707));_tmp707->v=s;_tmp707;}));_tmp705;});}struct _tuple8*Cyc_Tcutil_r_make_inst_var(
struct _tuple9*env,struct Cyc_Absyn_Tvar*tv){struct _tuple9 _tmp709;struct Cyc_List_List*
_tmp70A;struct _RegionHandle*_tmp70B;struct _tuple9*_tmp708=env;_tmp709=*_tmp708;
_tmp70A=_tmp709.f1;_tmp70B=_tmp709.f2;{void*k=Cyc_Tcutil_tvar_kind(tv);return({
struct _tuple8*_tmp70C=_region_malloc(_tmp70B,sizeof(*_tmp70C));_tmp70C->f1=tv;
_tmp70C->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp70D=_cycalloc(sizeof(*
_tmp70D));_tmp70D->v=(void*)k;_tmp70D;}),({struct Cyc_Core_Opt*_tmp70E=_cycalloc(
sizeof(*_tmp70E));_tmp70E->v=_tmp70A;_tmp70E;}));_tmp70C;});}}static struct Cyc_List_List*
Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(
void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(
k1,k2))({struct Cyc_String_pa_struct _tmp713;_tmp713.tag=0;_tmp713.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kindbound2string(k2));{
struct Cyc_String_pa_struct _tmp712;_tmp712.tag=0;_tmp712.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kindbound2string(k1));{
struct Cyc_String_pa_struct _tmp711;_tmp711.tag=0;_tmp711.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*tv->name);{void*_tmp70F[3]={& _tmp711,&
_tmp712,& _tmp713};Cyc_Tcutil_terr(loc,({const char*_tmp710="type variable %s is used with inconsistent kinds %s and %s";
_tag_dynforward(_tmp710,sizeof(char),_get_zero_arr_size(_tmp710,59));}),
_tag_dynforward(_tmp70F,sizeof(void*),3));}}}});if(tv->identity == 0)tv->identity=((
struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{if(*((int*)_check_null(tv->identity))
!= *((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)))({void*
_tmp714[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp715="same type variable has different identity!";_tag_dynforward(
_tmp715,sizeof(char),_get_zero_arr_size(_tmp715,43));}),_tag_dynforward(_tmp714,
sizeof(void*),0));});}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();return({
struct Cyc_List_List*_tmp716=_cycalloc(sizeof(*_tmp716));_tmp716->hd=tv;_tmp716->tl=
tvs;_tmp716;});}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({void*_tmp717[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp718="fast_add_free_tvar: bad identity in tv";_tag_dynforward(
_tmp718,sizeof(char),_get_zero_arr_size(_tmp718,39));}),_tag_dynforward(_tmp717,
sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity == 0)({void*_tmp719[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp71A="fast_add_free_tvar: bad identity in tvs2";_tag_dynforward(_tmp71A,
sizeof(char),_get_zero_arr_size(_tmp71A,41));}),_tag_dynforward(_tmp719,sizeof(
void*),0));});if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity))
== *((int*)_check_null(tv->identity)))return tvs;}}return({struct Cyc_List_List*
_tmp71B=_cycalloc(sizeof(*_tmp71B));_tmp71B->hd=tv;_tmp71B->tl=tvs;_tmp71B;});}
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({struct Cyc_String_pa_struct _tmp71E;
_tmp71E.tag=0;_tmp71E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcutil_tvar2string(
tv));{void*_tmp71C[1]={& _tmp71E};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp71D="bound tvar id for %s is NULL";
_tag_dynforward(_tmp71D,sizeof(char),_get_zero_arr_size(_tmp71D,29));}),
_tag_dynforward(_tmp71C,sizeof(void*),1));}});return({struct Cyc_List_List*
_tmp71F=_cycalloc(sizeof(*_tmp71F));_tmp71F->hd=tv;_tmp71F->tl=tvs;_tmp71F;});}
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct Cyc_List_List*es,void*e){
void*_tmp720=Cyc_Tcutil_compress(e);int _tmp721;_LL4A6: if(_tmp720 <= (void*)4)goto
_LL4A8;if(*((int*)_tmp720)!= 0)goto _LL4A8;_tmp721=((struct Cyc_Absyn_Evar_struct*)
_tmp720)->f3;_LL4A7:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){void*
_tmp722=Cyc_Tcutil_compress((void*)es2->hd);int _tmp723;_LL4AB: if(_tmp722 <= (void*)
4)goto _LL4AD;if(*((int*)_tmp722)!= 0)goto _LL4AD;_tmp723=((struct Cyc_Absyn_Evar_struct*)
_tmp722)->f3;_LL4AC: if(_tmp721 == _tmp723)return es;goto _LL4AA;_LL4AD:;_LL4AE: goto
_LL4AA;_LL4AA:;}}return({struct Cyc_List_List*_tmp724=_cycalloc(sizeof(*_tmp724));
_tmp724->hd=(void*)e;_tmp724->tl=es;_tmp724;});_LL4A8:;_LL4A9: return es;_LL4A5:;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs){struct Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int
present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(*((int*)
_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))== *((int*)_check_null(((
struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;break;}}}if(!present)r=({
struct Cyc_List_List*_tmp725=_cycalloc(sizeof(*_tmp725));_tmp725->hd=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp725->tl=r;_tmp725;});}r=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dynforward_ptr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)width))({struct Cyc_String_pa_struct _tmp728;_tmp728.tag=0;
_tmp728.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*fn);{void*_tmp726[1]={&
_tmp728};Cyc_Tcutil_terr(loc,({const char*_tmp727="bitfield %s does not have constant width";
_tag_dynforward(_tmp727,sizeof(char),_get_zero_arr_size(_tmp727,41));}),
_tag_dynforward(_tmp726,sizeof(void*),1));}});else{unsigned int _tmp72A;int
_tmp72B;struct _tuple7 _tmp729=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
width);_tmp72A=_tmp729.f1;_tmp72B=_tmp729.f2;if(!_tmp72B)({void*_tmp72C[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp72D="bitfield width cannot use sizeof or offsetof";
_tag_dynforward(_tmp72D,sizeof(char),_get_zero_arr_size(_tmp72D,45));}),
_tag_dynforward(_tmp72C,sizeof(void*),0));});w=_tmp72A;}{void*_tmp72E=Cyc_Tcutil_compress(
field_typ);void*_tmp72F;_LL4B0: if(_tmp72E <= (void*)4)goto _LL4B2;if(*((int*)
_tmp72E)!= 5)goto _LL4B2;_tmp72F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp72E)->f2;
_LL4B1:{void*_tmp730=_tmp72F;_LL4B5: if((int)_tmp730 != 0)goto _LL4B7;_LL4B6: if(w > 
8)({void*_tmp731[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp732="bitfield larger than type";
_tag_dynforward(_tmp732,sizeof(char),_get_zero_arr_size(_tmp732,26));}),
_tag_dynforward(_tmp731,sizeof(void*),0));});goto _LL4B4;_LL4B7: if((int)_tmp730 != 
1)goto _LL4B9;_LL4B8: if(w > 16)({void*_tmp733[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp734="bitfield larger than type";_tag_dynforward(_tmp734,sizeof(char),
_get_zero_arr_size(_tmp734,26));}),_tag_dynforward(_tmp733,sizeof(void*),0));});
goto _LL4B4;_LL4B9: if((int)_tmp730 != 3)goto _LL4BB;_LL4BA: goto _LL4BC;_LL4BB: if((
int)_tmp730 != 2)goto _LL4BD;_LL4BC: if(w > 32)({void*_tmp735[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp736="bitfield larger than type";_tag_dynforward(_tmp736,
sizeof(char),_get_zero_arr_size(_tmp736,26));}),_tag_dynforward(_tmp735,sizeof(
void*),0));});goto _LL4B4;_LL4BD: if((int)_tmp730 != 4)goto _LL4B4;_LL4BE: if(w > 64)({
void*_tmp737[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp738="bitfield larger than type";
_tag_dynforward(_tmp738,sizeof(char),_get_zero_arr_size(_tmp738,26));}),
_tag_dynforward(_tmp737,sizeof(void*),0));});goto _LL4B4;_LL4B4:;}goto _LL4AF;
_LL4B2:;_LL4B3:({struct Cyc_String_pa_struct _tmp73C;_tmp73C.tag=0;_tmp73C.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(field_typ));{
struct Cyc_String_pa_struct _tmp73B;_tmp73B.tag=0;_tmp73B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*fn);{void*_tmp739[2]={& _tmp73B,& _tmp73C};
Cyc_Tcutil_terr(loc,({const char*_tmp73A="bitfield %s must have integral type but has type %s";
_tag_dynforward(_tmp73A,sizeof(char),_get_zero_arr_size(_tmp73A,52));}),
_tag_dynforward(_tmp739,sizeof(void*),2));}}});goto _LL4AF;_LL4AF:;}}}static void
Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*loc,struct _dynforward_ptr*
fn,struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*_tmp73D=(void*)
atts->hd;_LL4C0: if((int)_tmp73D != 5)goto _LL4C2;_LL4C1: continue;_LL4C2: if(_tmp73D
<= (void*)17)goto _LL4C4;if(*((int*)_tmp73D)!= 1)goto _LL4C4;_LL4C3: continue;
_LL4C4:;_LL4C5:({struct Cyc_String_pa_struct _tmp741;_tmp741.tag=0;_tmp741.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*fn);{struct Cyc_String_pa_struct
_tmp740;_tmp740.tag=0;_tmp740.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));{void*_tmp73E[2]={& _tmp740,& _tmp741};
Cyc_Tcutil_terr(loc,({const char*_tmp73F="bad attribute %s on member %s";
_tag_dynforward(_tmp73F,sizeof(char),_get_zero_arr_size(_tmp73F,30));}),
_tag_dynforward(_tmp73E,sizeof(void*),2));}}});_LL4BF:;}}struct Cyc_Tcutil_CVTEnv{
struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*
free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp742=t;struct
Cyc_Absyn_Typedefdecl*_tmp743;void**_tmp744;_LL4C7: if(_tmp742 <= (void*)4)goto
_LL4C9;if(*((int*)_tmp742)!= 16)goto _LL4C9;_tmp743=((struct Cyc_Absyn_TypedefType_struct*)
_tmp742)->f3;_tmp744=((struct Cyc_Absyn_TypedefType_struct*)_tmp742)->f4;_LL4C8:
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp743))->tq).real_const  || (
_tmp743->tq).print_const){if(declared_const)({void*_tmp745[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp746="extra const";_tag_dynforward(_tmp746,sizeof(char),
_get_zero_arr_size(_tmp746,12));}),_tag_dynforward(_tmp745,sizeof(void*),0));});
return 1;}if((unsigned int)_tmp744)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmp744);else{return declared_const;}_LL4C9:;_LL4CA: return
declared_const;_LL4C6:;}struct _tuple17{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,void*t,int allow_unique){{
void*_tmp747=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp748;struct Cyc_Core_Opt**
_tmp749;struct Cyc_Core_Opt*_tmp74A;struct Cyc_Core_Opt**_tmp74B;struct Cyc_Absyn_Tvar*
_tmp74C;struct Cyc_List_List*_tmp74D;struct _tuple1*_tmp74E;struct Cyc_Absyn_Enumdecl*
_tmp74F;struct Cyc_Absyn_Enumdecl**_tmp750;struct Cyc_Absyn_TunionInfo _tmp751;void*
_tmp752;void**_tmp753;struct Cyc_List_List*_tmp754;struct Cyc_List_List**_tmp755;
void*_tmp756;struct Cyc_Absyn_TunionFieldInfo _tmp757;void*_tmp758;void**_tmp759;
struct Cyc_List_List*_tmp75A;struct Cyc_Absyn_PtrInfo _tmp75B;void*_tmp75C;struct
Cyc_Absyn_Tqual _tmp75D;struct Cyc_Absyn_Tqual*_tmp75E;struct Cyc_Absyn_PtrAtts
_tmp75F;void*_tmp760;struct Cyc_Absyn_Conref*_tmp761;struct Cyc_Absyn_Conref*
_tmp762;struct Cyc_Absyn_Conref*_tmp763;void*_tmp764;void*_tmp765;struct Cyc_Absyn_ArrayInfo
_tmp766;void*_tmp767;struct Cyc_Absyn_Tqual _tmp768;struct Cyc_Absyn_Tqual*_tmp769;
struct Cyc_Absyn_Exp*_tmp76A;struct Cyc_Absyn_Exp**_tmp76B;struct Cyc_Absyn_Conref*
_tmp76C;struct Cyc_Position_Segment*_tmp76D;struct Cyc_Absyn_FnInfo _tmp76E;struct
Cyc_List_List*_tmp76F;struct Cyc_List_List**_tmp770;struct Cyc_Core_Opt*_tmp771;
struct Cyc_Core_Opt**_tmp772;void*_tmp773;struct Cyc_List_List*_tmp774;int _tmp775;
struct Cyc_Absyn_VarargInfo*_tmp776;struct Cyc_List_List*_tmp777;struct Cyc_List_List*
_tmp778;struct Cyc_List_List*_tmp779;void*_tmp77A;struct Cyc_List_List*_tmp77B;
struct Cyc_Absyn_AggrInfo _tmp77C;void*_tmp77D;void**_tmp77E;struct Cyc_List_List*
_tmp77F;struct Cyc_List_List**_tmp780;struct _tuple1*_tmp781;struct Cyc_List_List*
_tmp782;struct Cyc_List_List**_tmp783;struct Cyc_Absyn_Typedefdecl*_tmp784;struct
Cyc_Absyn_Typedefdecl**_tmp785;void**_tmp786;void***_tmp787;void*_tmp788;void*
_tmp789;void*_tmp78A;struct Cyc_List_List*_tmp78B;_LL4CC: if((int)_tmp747 != 0)goto
_LL4CE;_LL4CD: goto _LL4CB;_LL4CE: if(_tmp747 <= (void*)4)goto _LL4E2;if(*((int*)
_tmp747)!= 0)goto _LL4D0;_tmp748=((struct Cyc_Absyn_Evar_struct*)_tmp747)->f1;
_tmp749=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp747)->f1;
_tmp74A=((struct Cyc_Absyn_Evar_struct*)_tmp747)->f2;_tmp74B=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp747)->f2;_LL4CF: if(*_tmp749 == 0)*_tmp749=({
struct Cyc_Core_Opt*_tmp78C=_cycalloc(sizeof(*_tmp78C));_tmp78C->v=(void*)
expected_kind;_tmp78C;});if((cvtenv.fn_result  && cvtenv.generalize_evars) && 
expected_kind == (void*)3)*_tmp74B=({struct Cyc_Core_Opt*_tmp78D=_cycalloc(sizeof(*
_tmp78D));_tmp78D->v=(void*)((void*)2);_tmp78D;});else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp78E=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp792=_cycalloc(sizeof(*_tmp792));_tmp792[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp793;_tmp793.tag=2;_tmp793.f1=0;_tmp793.f2=(void*)expected_kind;_tmp793;});
_tmp792;}));*_tmp74B=({struct Cyc_Core_Opt*_tmp78F=_cycalloc(sizeof(*_tmp78F));
_tmp78F->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp790=_cycalloc(
sizeof(*_tmp790));_tmp790[0]=({struct Cyc_Absyn_VarType_struct _tmp791;_tmp791.tag=
1;_tmp791.f1=_tmp78E;_tmp791;});_tmp790;}));_tmp78F;});_tmp74C=_tmp78E;goto
_LL4D1;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,t);}}
goto _LL4CB;_LL4D0: if(*((int*)_tmp747)!= 1)goto _LL4D2;_tmp74C=((struct Cyc_Absyn_VarType_struct*)
_tmp747)->f1;_LL4D1:{void*_tmp794=Cyc_Absyn_compress_kb((void*)_tmp74C->kind);
struct Cyc_Core_Opt*_tmp795;struct Cyc_Core_Opt**_tmp796;_LL501: if(*((int*)_tmp794)
!= 1)goto _LL503;_tmp795=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp794)->f1;
_tmp796=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp794)->f1;
_LL502:*_tmp796=({struct Cyc_Core_Opt*_tmp797=_cycalloc(sizeof(*_tmp797));_tmp797->v=(
void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp798=_cycalloc(sizeof(*_tmp798));
_tmp798[0]=({struct Cyc_Absyn_Less_kb_struct _tmp799;_tmp799.tag=2;_tmp799.f1=0;
_tmp799.f2=(void*)expected_kind;_tmp799;});_tmp798;}));_tmp797;});goto _LL500;
_LL503:;_LL504: goto _LL500;_LL500:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmp74C);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,
_tmp74C);goto _LL4CB;_LL4D2: if(*((int*)_tmp747)!= 13)goto _LL4D4;_tmp74D=((struct
Cyc_Absyn_AnonEnumType_struct*)_tmp747)->f1;_LL4D3: {struct Cyc_Tcenv_Genv*ge=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);{struct _RegionHandle _tmp79A=_new_region("uprev_rgn");struct
_RegionHandle*uprev_rgn=& _tmp79A;_push_region(uprev_rgn);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmp74D != 0;_tmp74D=_tmp74D->tl){
struct Cyc_Absyn_Enumfield*_tmp79B=(struct Cyc_Absyn_Enumfield*)_tmp74D->hd;if(((
int(*)(int(*compare)(struct _dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*
l,struct _dynforward_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp79B->name).f2))({
struct Cyc_String_pa_struct _tmp79E;_tmp79E.tag=0;_tmp79E.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*(*_tmp79B->name).f2);{void*_tmp79C[1]={&
_tmp79E};Cyc_Tcutil_terr(_tmp79B->loc,({const char*_tmp79D="duplicate enum field name %s";
_tag_dynforward(_tmp79D,sizeof(char),_get_zero_arr_size(_tmp79D,29));}),
_tag_dynforward(_tmp79C,sizeof(void*),1));}});else{prev_fields=({struct Cyc_List_List*
_tmp79F=_region_malloc(uprev_rgn,sizeof(*_tmp79F));_tmp79F->hd=(*_tmp79B->name).f2;
_tmp79F->tl=prev_fields;_tmp79F;});}if(_tmp79B->tag == 0)_tmp79B->tag=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(tag_count,_tmp79B->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)_check_null(_tmp79B->tag)))({struct Cyc_String_pa_struct
_tmp7A2;_tmp7A2.tag=0;_tmp7A2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*(*
_tmp79B->name).f2);{void*_tmp7A0[1]={& _tmp7A2};Cyc_Tcutil_terr(loc,({const char*
_tmp7A1="enum field %s: expression is not constant";_tag_dynforward(_tmp7A1,
sizeof(char),_get_zero_arr_size(_tmp7A1,42));}),_tag_dynforward(_tmp7A0,sizeof(
void*),1));}});}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp79B->tag))).f1;tag_count=t1 + 1;(*_tmp79B->name).f1=(void*)({
struct Cyc_Absyn_Abs_n_struct*_tmp7A3=_cycalloc(sizeof(*_tmp7A3));_tmp7A3[0]=({
struct Cyc_Absyn_Abs_n_struct _tmp7A4;_tmp7A4.tag=1;_tmp7A4.f1=te->ns;_tmp7A4;});
_tmp7A3;});ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k,struct _tuple17*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp79B->name).f2,(
struct _tuple17*)({struct _tuple17*_tmp7A5=_cycalloc(sizeof(*_tmp7A5));_tmp7A5->f1=(
void*)({struct Cyc_Tcenv_AnonEnumRes_struct*_tmp7A6=_cycalloc(sizeof(*_tmp7A6));
_tmp7A6[0]=({struct Cyc_Tcenv_AnonEnumRes_struct _tmp7A7;_tmp7A7.tag=4;_tmp7A7.f1=(
void*)t;_tmp7A7.f2=_tmp79B;_tmp7A7;});_tmp7A6;});_tmp7A5->f2=1;_tmp7A5;}));}}};
_pop_region(uprev_rgn);}goto _LL4CB;}_LL4D4: if(*((int*)_tmp747)!= 12)goto _LL4D6;
_tmp74E=((struct Cyc_Absyn_EnumType_struct*)_tmp747)->f1;_tmp74F=((struct Cyc_Absyn_EnumType_struct*)
_tmp747)->f2;_tmp750=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_struct*)
_tmp747)->f2;_LL4D5: if(*_tmp750 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*
_tmp750))->fields == 0){struct _handler_cons _tmp7A8;_push_handler(& _tmp7A8);{int
_tmp7AA=0;if(setjmp(_tmp7A8.handler))_tmp7AA=1;if(!_tmp7AA){{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp74E);*_tmp750=(struct Cyc_Absyn_Enumdecl*)*
ed;};_pop_handler();}else{void*_tmp7A9=(void*)_exn_thrown;void*_tmp7AC=_tmp7A9;
_LL506: if(_tmp7AC != Cyc_Dict_Absent)goto _LL508;_LL507: {struct Cyc_Tcenv_Genv*
_tmp7AD=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Enumdecl*_tmp7AE=({struct Cyc_Absyn_Enumdecl*
_tmp7AF=_cycalloc(sizeof(*_tmp7AF));_tmp7AF->sc=(void*)((void*)3);_tmp7AF->name=
_tmp74E;_tmp7AF->fields=0;_tmp7AF;});Cyc_Tc_tcEnumdecl(te,_tmp7AD,loc,_tmp7AE);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp74E);*_tmp750=(
struct Cyc_Absyn_Enumdecl*)*ed;goto _LL505;}}_LL508:;_LL509:(void)_throw(_tmp7AC);
_LL505:;}}}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*
_tmp750);*_tmp74E=(ed->name)[_check_known_subscript_notnull(1,0)];goto _LL4CB;}
_LL4D6: if(*((int*)_tmp747)!= 2)goto _LL4D8;_tmp751=((struct Cyc_Absyn_TunionType_struct*)
_tmp747)->f1;_tmp752=(void*)_tmp751.tunion_info;_tmp753=(void**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp747)->f1).tunion_info;_tmp754=_tmp751.targs;_tmp755=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp747)->f1).targs;_tmp756=(void*)_tmp751.rgn;
_LL4D7: {struct Cyc_List_List*_tmp7B0=*_tmp755;{void*_tmp7B1=*_tmp753;struct Cyc_Absyn_UnknownTunionInfo
_tmp7B2;struct _tuple1*_tmp7B3;int _tmp7B4;struct Cyc_Absyn_Tuniondecl**_tmp7B5;
struct Cyc_Absyn_Tuniondecl*_tmp7B6;_LL50B: if(*((int*)_tmp7B1)!= 0)goto _LL50D;
_tmp7B2=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp7B1)->f1;_tmp7B3=_tmp7B2.name;
_tmp7B4=_tmp7B2.is_xtunion;_LL50C: {struct Cyc_Absyn_Tuniondecl**tudp;{struct
_handler_cons _tmp7B7;_push_handler(& _tmp7B7);{int _tmp7B9=0;if(setjmp(_tmp7B7.handler))
_tmp7B9=1;if(!_tmp7B9){tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp7B3);;
_pop_handler();}else{void*_tmp7B8=(void*)_exn_thrown;void*_tmp7BB=_tmp7B8;_LL510:
if(_tmp7BB != Cyc_Dict_Absent)goto _LL512;_LL511: {struct Cyc_Tcenv_Genv*_tmp7BC=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*_tmp7BD=({struct Cyc_Absyn_Tuniondecl*
_tmp7C4=_cycalloc(sizeof(*_tmp7C4));_tmp7C4->sc=(void*)((void*)3);_tmp7C4->name=
_tmp7B3;_tmp7C4->tvs=0;_tmp7C4->fields=0;_tmp7C4->is_xtunion=_tmp7B4;_tmp7C4;});
Cyc_Tc_tcTuniondecl(te,_tmp7BC,loc,_tmp7BD);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp7B3);if(_tmp7B0 != 0){({struct Cyc_String_pa_struct _tmp7C1;_tmp7C1.tag=0;
_tmp7C1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp7B3));{struct Cyc_String_pa_struct _tmp7C0;_tmp7C0.tag=0;_tmp7C0.f1=(struct
_dynforward_ptr)(_tmp7B4?(struct _dynforward_ptr)((struct _dynforward_ptr)({const
char*_tmp7C2="xtunion";_tag_dynforward(_tmp7C2,sizeof(char),_get_zero_arr_size(
_tmp7C2,8));})):(struct _dynforward_ptr)({const char*_tmp7C3="tunion";
_tag_dynforward(_tmp7C3,sizeof(char),_get_zero_arr_size(_tmp7C3,7));}));{void*
_tmp7BE[2]={& _tmp7C0,& _tmp7C1};Cyc_Tcutil_terr(loc,({const char*_tmp7BF="declare parameterized %s %s before using";
_tag_dynforward(_tmp7BF,sizeof(char),_get_zero_arr_size(_tmp7BF,41));}),
_tag_dynforward(_tmp7BE,sizeof(void*),2));}}});return cvtenv;}goto _LL50F;}_LL512:;
_LL513:(void)_throw(_tmp7BB);_LL50F:;}}}if((*tudp)->is_xtunion != _tmp7B4)({
struct Cyc_String_pa_struct _tmp7C7;_tmp7C7.tag=0;_tmp7C7.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7B3));{void*
_tmp7C5[1]={& _tmp7C7};Cyc_Tcutil_terr(loc,({const char*_tmp7C6="[x]tunion is different from type declaration %s";
_tag_dynforward(_tmp7C6,sizeof(char),_get_zero_arr_size(_tmp7C6,48));}),
_tag_dynforward(_tmp7C5,sizeof(void*),1));}});*_tmp753=(void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp7C8=_cycalloc(sizeof(*_tmp7C8));_tmp7C8[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp7C9;_tmp7C9.tag=1;_tmp7C9.f1=tudp;_tmp7C9;});_tmp7C8;});_tmp7B6=*tudp;goto
_LL50E;}_LL50D: if(*((int*)_tmp7B1)!= 1)goto _LL50A;_tmp7B5=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp7B1)->f1;_tmp7B6=*_tmp7B5;_LL50E: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp756,allow_unique);{struct Cyc_List_List*tvs=_tmp7B6->tvs;for(
0;_tmp7B0 != 0  && tvs != 0;(_tmp7B0=_tmp7B0->tl,tvs=tvs->tl)){void*t1=(void*)
_tmp7B0->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1,0);}if(_tmp7B0 != 0)({struct Cyc_String_pa_struct
_tmp7CC;_tmp7CC.tag=0;_tmp7CC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp7B6->name));{void*_tmp7CA[1]={& _tmp7CC};Cyc_Tcutil_terr(
loc,({const char*_tmp7CB="too many type arguments for tunion %s";_tag_dynforward(
_tmp7CB,sizeof(char),_get_zero_arr_size(_tmp7CB,38));}),_tag_dynforward(_tmp7CA,
sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;
tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=
Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp7CD=_cycalloc(
sizeof(*_tmp7CD));_tmp7CD->hd=(void*)e;_tmp7CD->tl=hidden_ts;_tmp7CD;});cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,0);}*_tmp755=Cyc_List_imp_append(*
_tmp755,Cyc_List_imp_rev(hidden_ts));}goto _LL50A;}_LL50A:;}goto _LL4CB;}_LL4D8:
if(*((int*)_tmp747)!= 3)goto _LL4DA;_tmp757=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp747)->f1;_tmp758=(void*)_tmp757.field_info;_tmp759=(void**)&(((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp747)->f1).field_info;_tmp75A=_tmp757.targs;_LL4D9:{void*_tmp7CE=*_tmp759;
struct Cyc_Absyn_UnknownTunionFieldInfo _tmp7CF;struct _tuple1*_tmp7D0;struct
_tuple1*_tmp7D1;int _tmp7D2;struct Cyc_Absyn_Tuniondecl*_tmp7D3;struct Cyc_Absyn_Tunionfield*
_tmp7D4;_LL515: if(*((int*)_tmp7CE)!= 0)goto _LL517;_tmp7CF=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp7CE)->f1;_tmp7D0=_tmp7CF.tunion_name;_tmp7D1=_tmp7CF.field_name;_tmp7D2=
_tmp7CF.is_xtunion;_LL516: {struct Cyc_Absyn_Tuniondecl*tud;struct Cyc_Absyn_Tunionfield*
tuf;{struct _handler_cons _tmp7D5;_push_handler(& _tmp7D5);{int _tmp7D7=0;if(setjmp(
_tmp7D5.handler))_tmp7D7=1;if(!_tmp7D7){*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp7D0);;_pop_handler();}else{void*_tmp7D6=(void*)_exn_thrown;void*_tmp7D9=
_tmp7D6;_LL51A: if(_tmp7D9 != Cyc_Dict_Absent)goto _LL51C;_LL51B:({struct Cyc_String_pa_struct
_tmp7DC;_tmp7DC.tag=0;_tmp7DC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp7D0));{void*_tmp7DA[1]={& _tmp7DC};Cyc_Tcutil_terr(
loc,({const char*_tmp7DB="unbound type tunion %s";_tag_dynforward(_tmp7DB,sizeof(
char),_get_zero_arr_size(_tmp7DB,23));}),_tag_dynforward(_tmp7DA,sizeof(void*),1));}});
return cvtenv;_LL51C:;_LL51D:(void)_throw(_tmp7D9);_LL519:;}}}{struct
_handler_cons _tmp7DD;_push_handler(& _tmp7DD);{int _tmp7DF=0;if(setjmp(_tmp7DD.handler))
_tmp7DF=1;if(!_tmp7DF){{struct _RegionHandle _tmp7E0=_new_region("r");struct
_RegionHandle*r=& _tmp7E0;_push_region(r);{void*_tmp7E1=Cyc_Tcenv_lookup_ordinary(
r,te,loc,_tmp7D1);struct Cyc_Absyn_Tuniondecl*_tmp7E2;struct Cyc_Absyn_Tunionfield*
_tmp7E3;_LL51F: if(*((int*)_tmp7E1)!= 2)goto _LL521;_tmp7E2=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp7E1)->f1;_tmp7E3=((struct Cyc_Tcenv_TunionRes_struct*)_tmp7E1)->f2;_LL520: tuf=
_tmp7E3;tud=_tmp7E2;if(tud->is_xtunion != _tmp7D2)({struct Cyc_String_pa_struct
_tmp7E6;_tmp7E6.tag=0;_tmp7E6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp7D0));{void*_tmp7E4[1]={& _tmp7E6};Cyc_Tcutil_terr(
loc,({const char*_tmp7E5="[x]tunion is different from type declaration %s";
_tag_dynforward(_tmp7E5,sizeof(char),_get_zero_arr_size(_tmp7E5,48));}),
_tag_dynforward(_tmp7E4,sizeof(void*),1));}});goto _LL51E;_LL521:;_LL522:({struct
Cyc_String_pa_struct _tmp7EA;_tmp7EA.tag=0;_tmp7EA.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7D0));{struct Cyc_String_pa_struct
_tmp7E9;_tmp7E9.tag=0;_tmp7E9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp7D1));{void*_tmp7E7[2]={& _tmp7E9,& _tmp7EA};Cyc_Tcutil_terr(
loc,({const char*_tmp7E8="unbound field %s in type tunion %s";_tag_dynforward(
_tmp7E8,sizeof(char),_get_zero_arr_size(_tmp7E8,35));}),_tag_dynforward(_tmp7E7,
sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv _tmp7EB=cvtenv;_npop_handler(1);
return _tmp7EB;}_LL51E:;};_pop_region(r);};_pop_handler();}else{void*_tmp7DE=(
void*)_exn_thrown;void*_tmp7ED=_tmp7DE;_LL524: if(_tmp7ED != Cyc_Dict_Absent)goto
_LL526;_LL525:({struct Cyc_String_pa_struct _tmp7F1;_tmp7F1.tag=0;_tmp7F1.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7D0));{
struct Cyc_String_pa_struct _tmp7F0;_tmp7F0.tag=0;_tmp7F0.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7D1));{void*
_tmp7EE[2]={& _tmp7F0,& _tmp7F1};Cyc_Tcutil_terr(loc,({const char*_tmp7EF="unbound field %s in type tunion %s";
_tag_dynforward(_tmp7EF,sizeof(char),_get_zero_arr_size(_tmp7EF,35));}),
_tag_dynforward(_tmp7EE,sizeof(void*),2));}}});return cvtenv;_LL526:;_LL527:(void)
_throw(_tmp7ED);_LL523:;}}}*_tmp759=(void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp7F2=_cycalloc(sizeof(*_tmp7F2));_tmp7F2[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp7F3;_tmp7F3.tag=1;_tmp7F3.f1=tud;_tmp7F3.f2=tuf;_tmp7F3;});_tmp7F2;});
_tmp7D3=tud;_tmp7D4=tuf;goto _LL518;}_LL517: if(*((int*)_tmp7CE)!= 1)goto _LL514;
_tmp7D3=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp7CE)->f1;_tmp7D4=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp7CE)->f2;_LL518: {struct Cyc_List_List*tvs=
_tmp7D3->tvs;for(0;_tmp75A != 0  && tvs != 0;(_tmp75A=_tmp75A->tl,tvs=tvs->tl)){
void*t1=(void*)_tmp75A->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1,0);}if(_tmp75A
!= 0)({struct Cyc_String_pa_struct _tmp7F7;_tmp7F7.tag=0;_tmp7F7.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7D4->name));{
struct Cyc_String_pa_struct _tmp7F6;_tmp7F6.tag=0;_tmp7F6.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7D3->name));{
void*_tmp7F4[2]={& _tmp7F6,& _tmp7F7};Cyc_Tcutil_terr(loc,({const char*_tmp7F5="too many type arguments for tunion %s.%s";
_tag_dynforward(_tmp7F5,sizeof(char),_get_zero_arr_size(_tmp7F5,41));}),
_tag_dynforward(_tmp7F4,sizeof(void*),2));}}});if(tvs != 0)({struct Cyc_String_pa_struct
_tmp7FB;_tmp7FB.tag=0;_tmp7FB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp7D4->name));{struct Cyc_String_pa_struct _tmp7FA;
_tmp7FA.tag=0;_tmp7FA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp7D3->name));{void*_tmp7F8[2]={& _tmp7FA,& _tmp7FB};Cyc_Tcutil_terr(loc,({const
char*_tmp7F9="too few type arguments for tunion %s.%s";_tag_dynforward(_tmp7F9,
sizeof(char),_get_zero_arr_size(_tmp7F9,40));}),_tag_dynforward(_tmp7F8,sizeof(
void*),2));}}});goto _LL514;}_LL514:;}goto _LL4CB;_LL4DA: if(*((int*)_tmp747)!= 4)
goto _LL4DC;_tmp75B=((struct Cyc_Absyn_PointerType_struct*)_tmp747)->f1;_tmp75C=(
void*)_tmp75B.elt_typ;_tmp75D=_tmp75B.elt_tq;_tmp75E=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_PointerType_struct*)_tmp747)->f1).elt_tq;_tmp75F=_tmp75B.ptr_atts;
_tmp760=(void*)_tmp75F.rgn;_tmp761=_tmp75F.nullable;_tmp762=_tmp75F.bounds;
_tmp763=_tmp75F.zero_term;_LL4DB: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp75C,0);_tmp75E->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp75E->print_const,_tmp75C);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp760,allow_unique);{void*_tmp7FC=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp763))->v;int _tmp7FD;
_LL529: if((int)_tmp7FC != 0)goto _LL52B;_LL52A:{void*_tmp7FE=Cyc_Tcutil_compress(
_tmp75C);void*_tmp7FF;_LL530: if(_tmp7FE <= (void*)4)goto _LL532;if(*((int*)_tmp7FE)
!= 5)goto _LL532;_tmp7FF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp7FE)->f2;
if((int)_tmp7FF != 0)goto _LL532;_LL531:((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp763,
Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL52F;_LL532:;_LL533:((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp763,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL52F;
_LL52F:;}goto _LL528;_LL52B: if(_tmp7FC <= (void*)1)goto _LL52D;if(*((int*)_tmp7FC)
!= 0)goto _LL52D;_tmp7FD=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp7FC)->f1;
if(_tmp7FD != 1)goto _LL52D;_LL52C: if(!Cyc_Tcutil_admits_zero(_tmp75C))({struct Cyc_String_pa_struct
_tmp802;_tmp802.tag=0;_tmp802.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(_tmp75C));{void*_tmp800[1]={& _tmp802};Cyc_Tcutil_terr(loc,({
const char*_tmp801="cannot have a pointer to zero-terminated %s elements";
_tag_dynforward(_tmp801,sizeof(char),_get_zero_arr_size(_tmp801,53));}),
_tag_dynforward(_tmp800,sizeof(void*),1));}});is_zero_terminated=1;goto _LL528;
_LL52D:;_LL52E: is_zero_terminated=0;goto _LL528;_LL528:;}{void*_tmp803=(void*)(
Cyc_Absyn_compress_conref(_tmp762))->v;void*_tmp804;void*_tmp805;void*_tmp806;
struct Cyc_Absyn_Exp*_tmp807;void*_tmp808;void*_tmp809;_LL535: if((int)_tmp803 != 0)
goto _LL537;_LL536: goto _LL538;_LL537: if(_tmp803 <= (void*)1)goto _LL539;if(*((int*)
_tmp803)!= 0)goto _LL539;_tmp804=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp803)->f1;if((int)_tmp804 != 0)goto _LL539;_LL538: goto _LL534;_LL539: if(_tmp803
<= (void*)1)goto _LL53B;if(*((int*)_tmp803)!= 0)goto _LL53B;_tmp805=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp803)->f1;if((int)_tmp805 != 1)goto _LL53B;
_LL53A: goto _LL534;_LL53B: if(_tmp803 <= (void*)1)goto _LL53D;if(*((int*)_tmp803)!= 
0)goto _LL53D;_tmp806=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp803)->f1;if(
_tmp806 <= (void*)2)goto _LL53D;if(*((int*)_tmp806)!= 0)goto _LL53D;_tmp807=((
struct Cyc_Absyn_Upper_b_struct*)_tmp806)->f1;_LL53C: Cyc_Tcexp_tcExp(te,0,_tmp807);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp807))({void*_tmp80A[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp80B="pointer bounds expression is not an unsigned int";
_tag_dynforward(_tmp80B,sizeof(char),_get_zero_arr_size(_tmp80B,49));}),
_tag_dynforward(_tmp80A,sizeof(void*),0));});{unsigned int _tmp80D;int _tmp80E;
struct _tuple7 _tmp80C=Cyc_Evexp_eval_const_uint_exp(_tmp807);_tmp80D=_tmp80C.f1;
_tmp80E=_tmp80C.f2;if(is_zero_terminated  && (!_tmp80E  || _tmp80D < 1))({void*
_tmp80F[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp810="zero-terminated pointer cannot point to empty sequence";
_tag_dynforward(_tmp810,sizeof(char),_get_zero_arr_size(_tmp810,55));}),
_tag_dynforward(_tmp80F,sizeof(void*),0));});goto _LL534;}_LL53D: if(_tmp803 <= (
void*)1)goto _LL53F;if(*((int*)_tmp803)!= 0)goto _LL53F;_tmp808=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp803)->f1;if(_tmp808 <= (void*)2)goto _LL53F;if(*((int*)_tmp808)!= 1)goto _LL53F;
_tmp809=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp808)->f1;_LL53E: cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,_tmp809,0);goto _LL534;
_LL53F: if(_tmp803 <= (void*)1)goto _LL534;if(*((int*)_tmp803)!= 1)goto _LL534;
_LL540:({void*_tmp811[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp812="forward constraint";
_tag_dynforward(_tmp812,sizeof(char),_get_zero_arr_size(_tmp812,19));}),
_tag_dynforward(_tmp811,sizeof(void*),0));});_LL534:;}goto _LL4CB;}_LL4DC: if(*((
int*)_tmp747)!= 17)goto _LL4DE;_tmp764=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp747)->f1;_LL4DD: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp764,allow_unique);goto _LL4CB;_LL4DE: if(*((int*)_tmp747)!= 14)goto _LL4E0;
_tmp765=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp747)->f1;_LL4DF: cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp765,allow_unique);goto
_LL4CB;_LL4E0: if(*((int*)_tmp747)!= 5)goto _LL4E2;_LL4E1: goto _LL4E3;_LL4E2: if((
int)_tmp747 != 1)goto _LL4E4;_LL4E3: goto _LL4E5;_LL4E4: if(_tmp747 <= (void*)4)goto
_LL4F2;if(*((int*)_tmp747)!= 6)goto _LL4E6;_LL4E5: goto _LL4CB;_LL4E6: if(*((int*)
_tmp747)!= 7)goto _LL4E8;_tmp766=((struct Cyc_Absyn_ArrayType_struct*)_tmp747)->f1;
_tmp767=(void*)_tmp766.elt_type;_tmp768=_tmp766.tq;_tmp769=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp747)->f1).tq;_tmp76A=_tmp766.num_elts;
_tmp76B=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp747)->f1).num_elts;
_tmp76C=_tmp766.zero_term;_tmp76D=_tmp766.zt_loc;_LL4E7: {struct Cyc_Absyn_Exp*
_tmp813=*_tmp76B;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,
_tmp767,0);_tmp769->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp769->print_const,
_tmp767);{int is_zero_terminated;{void*_tmp814=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp76C))->v;int _tmp815;
_LL542: if((int)_tmp814 != 0)goto _LL544;_LL543:((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp76C,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL541;_LL544: if(
_tmp814 <= (void*)1)goto _LL546;if(*((int*)_tmp814)!= 0)goto _LL546;_tmp815=(int)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp814)->f1;if(_tmp815 != 1)goto _LL546;_LL545:
if(!Cyc_Tcutil_admits_zero(_tmp767))({struct Cyc_String_pa_struct _tmp818;_tmp818.tag=
0;_tmp818.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
_tmp767));{void*_tmp816[1]={& _tmp818};Cyc_Tcutil_terr(loc,({const char*_tmp817="cannot have a zero-terminated array of %s elements";
_tag_dynforward(_tmp817,sizeof(char),_get_zero_arr_size(_tmp817,51));}),
_tag_dynforward(_tmp816,sizeof(void*),1));}});is_zero_terminated=1;goto _LL541;
_LL546:;_LL547: is_zero_terminated=0;goto _LL541;_LL541:;}if(_tmp813 == 0){if(
is_zero_terminated)*_tmp76B=(_tmp813=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,
0));else{({void*_tmp819[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp81A="array bound defaults to 0 here";
_tag_dynforward(_tmp81A,sizeof(char),_get_zero_arr_size(_tmp81A,31));}),
_tag_dynforward(_tmp819,sizeof(void*),0));});*_tmp76B=(_tmp813=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(0,0));}}Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp813);
if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_tmp813))({void*_tmp81B[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp81C="array bounds expression is not constant";
_tag_dynforward(_tmp81C,sizeof(char),_get_zero_arr_size(_tmp81C,40));}),
_tag_dynforward(_tmp81B,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_uint_typ(te,(
struct Cyc_Absyn_Exp*)_tmp813))({void*_tmp81D[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp81E="array bounds expression is not an unsigned int";_tag_dynforward(
_tmp81E,sizeof(char),_get_zero_arr_size(_tmp81E,47));}),_tag_dynforward(_tmp81D,
sizeof(void*),0));});{unsigned int _tmp820;int _tmp821;struct _tuple7 _tmp81F=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp813);_tmp820=_tmp81F.f1;_tmp821=_tmp81F.f2;if((
is_zero_terminated  && _tmp821) && _tmp820 < 1)({void*_tmp822[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp823="zero terminated array cannot have zero elements";
_tag_dynforward(_tmp823,sizeof(char),_get_zero_arr_size(_tmp823,48));}),
_tag_dynforward(_tmp822,sizeof(void*),0));});goto _LL4CB;}}}_LL4E8: if(*((int*)
_tmp747)!= 8)goto _LL4EA;_tmp76E=((struct Cyc_Absyn_FnType_struct*)_tmp747)->f1;
_tmp76F=_tmp76E.tvars;_tmp770=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp747)->f1).tvars;_tmp771=_tmp76E.effect;_tmp772=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp747)->f1).effect;_tmp773=(void*)_tmp76E.ret_typ;
_tmp774=_tmp76E.args;_tmp775=_tmp76E.c_varargs;_tmp776=_tmp76E.cyc_varargs;
_tmp777=_tmp76E.rgn_po;_tmp778=_tmp76E.attributes;_LL4E9: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp778 != 0;_tmp778=_tmp778->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp778->hd))({struct Cyc_String_pa_struct _tmp826;
_tmp826.tag=0;_tmp826.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absyn_attribute2string((
void*)_tmp778->hd));{void*_tmp824[1]={& _tmp826};Cyc_Tcutil_terr(loc,({const char*
_tmp825="bad function type attribute %s";_tag_dynforward(_tmp825,sizeof(char),
_get_zero_arr_size(_tmp825,31));}),_tag_dynforward(_tmp824,sizeof(void*),1));}});{
void*_tmp827=(void*)_tmp778->hd;void*_tmp828;int _tmp829;int _tmp82A;_LL549: if((
int)_tmp827 != 0)goto _LL54B;_LL54A: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}
goto _LL548;_LL54B: if((int)_tmp827 != 1)goto _LL54D;_LL54C: if(!seen_cdecl){
seen_cdecl=1;++ num_convs;}goto _LL548;_LL54D: if((int)_tmp827 != 2)goto _LL54F;
_LL54E: if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL548;_LL54F: if(
_tmp827 <= (void*)17)goto _LL551;if(*((int*)_tmp827)!= 3)goto _LL551;_tmp828=(void*)((
struct Cyc_Absyn_Format_att_struct*)_tmp827)->f1;_tmp829=((struct Cyc_Absyn_Format_att_struct*)
_tmp827)->f2;_tmp82A=((struct Cyc_Absyn_Format_att_struct*)_tmp827)->f3;_LL550:
if(!seen_format){seen_format=1;ft=_tmp828;fmt_desc_arg=_tmp829;fmt_arg_start=
_tmp82A;}else{({void*_tmp82B[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp82C="function can't have multiple format attributes";
_tag_dynforward(_tmp82C,sizeof(char),_get_zero_arr_size(_tmp82C,47));}),
_tag_dynforward(_tmp82B,sizeof(void*),0));});}goto _LL548;_LL551:;_LL552: goto
_LL548;_LL548:;}}if(num_convs > 1)({void*_tmp82D[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp82E="function can't have multiple calling conventions";
_tag_dynforward(_tmp82E,sizeof(char),_get_zero_arr_size(_tmp82E,49));}),
_tag_dynforward(_tmp82D,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*
_tmp770);{struct Cyc_List_List*b=*_tmp770;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)
b->hd)->identity=Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(
cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp82F=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp830;_LL554: if(*((int*)
_tmp82F)!= 0)goto _LL556;_tmp830=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp82F)->f1;
if((int)_tmp830 != 1)goto _LL556;_LL555:({struct Cyc_String_pa_struct _tmp833;
_tmp833.tag=0;_tmp833.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)b->hd)->name);{void*_tmp831[1]={& _tmp833};Cyc_Tcutil_terr(
loc,({const char*_tmp832="function attempts to abstract Mem type variable %s";
_tag_dynforward(_tmp832,sizeof(char),_get_zero_arr_size(_tmp832,51));}),
_tag_dynforward(_tmp831,sizeof(void*),1));}});goto _LL553;_LL556:;_LL557: goto
_LL553;_LL553:;}}}{struct Cyc_Tcutil_CVTEnv _tmp834=({struct Cyc_Tcutil_CVTEnv
_tmp8B0;_tmp8B0.kind_env=cvtenv.kind_env;_tmp8B0.free_vars=0;_tmp8B0.free_evars=
0;_tmp8B0.generalize_evars=cvtenv.generalize_evars;_tmp8B0.fn_result=1;_tmp8B0;});
_tmp834=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp834,(void*)1,_tmp773,1);_tmp834.fn_result=
0;{struct Cyc_List_List*a=_tmp774;for(0;a != 0;a=a->tl){struct _tuple2*_tmp835=(
struct _tuple2*)a->hd;void*_tmp836=(*_tmp835).f3;_tmp834=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp834,(void*)1,_tmp836,1);((*_tmp835).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp835).f2).print_const,_tmp836);}}if(_tmp776 != 0){if(_tmp775)({void*
_tmp837[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp838="both c_vararg and cyc_vararg";_tag_dynforward(_tmp838,sizeof(
char),_get_zero_arr_size(_tmp838,29));}),_tag_dynforward(_tmp837,sizeof(void*),0));});{
void*_tmp83A;int _tmp83B;struct Cyc_Absyn_VarargInfo _tmp839=*_tmp776;_tmp83A=(void*)
_tmp839.type;_tmp83B=_tmp839.inject;_tmp834=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp834,(void*)1,_tmp83A,1);(_tmp776->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp776->tq).print_const,_tmp83A);if(_tmp83B){void*_tmp83C=Cyc_Tcutil_compress(
_tmp83A);struct Cyc_Absyn_TunionInfo _tmp83D;void*_tmp83E;_LL559: if(_tmp83C <= (
void*)4)goto _LL55B;if(*((int*)_tmp83C)!= 2)goto _LL55B;_tmp83D=((struct Cyc_Absyn_TunionType_struct*)
_tmp83C)->f1;_tmp83E=(void*)_tmp83D.tunion_info;if(*((int*)_tmp83E)!= 1)goto
_LL55B;_LL55A: goto _LL558;_LL55B:;_LL55C:({void*_tmp83F[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp840="can't inject a non-[x]tunion type";_tag_dynforward(_tmp840,
sizeof(char),_get_zero_arr_size(_tmp840,34));}),_tag_dynforward(_tmp83F,sizeof(
void*),0));});goto _LL558;_LL558:;}}}if(seen_format){int _tmp841=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp774);if((((fmt_desc_arg < 0  || fmt_desc_arg
> _tmp841) || fmt_arg_start < 0) || _tmp776 == 0  && fmt_arg_start != 0) || _tmp776
!= 0  && fmt_arg_start != _tmp841 + 1)({void*_tmp842[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp843="bad format descriptor";_tag_dynforward(_tmp843,sizeof(char),
_get_zero_arr_size(_tmp843,22));}),_tag_dynforward(_tmp842,sizeof(void*),0));});
else{void*_tmp845;struct _tuple2 _tmp844=*((struct _tuple2*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp774,fmt_desc_arg - 1);_tmp845=_tmp844.f3;{void*_tmp846=
Cyc_Tcutil_compress(_tmp845);struct Cyc_Absyn_PtrInfo _tmp847;void*_tmp848;struct
Cyc_Absyn_PtrAtts _tmp849;struct Cyc_Absyn_Conref*_tmp84A;struct Cyc_Absyn_Conref*
_tmp84B;_LL55E: if(_tmp846 <= (void*)4)goto _LL560;if(*((int*)_tmp846)!= 4)goto
_LL560;_tmp847=((struct Cyc_Absyn_PointerType_struct*)_tmp846)->f1;_tmp848=(void*)
_tmp847.elt_typ;_tmp849=_tmp847.ptr_atts;_tmp84A=_tmp849.bounds;_tmp84B=_tmp849.zero_term;
_LL55F:{struct _tuple6 _tmp84D=({struct _tuple6 _tmp84C;_tmp84C.f1=Cyc_Tcutil_compress(
_tmp848);_tmp84C.f2=Cyc_Absyn_conref_def((void*)((void*)0),_tmp84A);_tmp84C;});
void*_tmp84E;void*_tmp84F;void*_tmp850;void*_tmp851;_LL563: _tmp84E=_tmp84D.f1;
if(_tmp84E <= (void*)4)goto _LL565;if(*((int*)_tmp84E)!= 5)goto _LL565;_tmp84F=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp84E)->f1;if((int)_tmp84F != 2)goto
_LL565;_tmp850=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp84E)->f2;if((int)
_tmp850 != 0)goto _LL565;_tmp851=_tmp84D.f2;if((int)_tmp851 != 0)goto _LL565;_LL564:
goto _LL562;_LL565:;_LL566:({void*_tmp852[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp853="format descriptor is not a char ? type";_tag_dynforward(_tmp853,sizeof(
char),_get_zero_arr_size(_tmp853,39));}),_tag_dynforward(_tmp852,sizeof(void*),0));});
goto _LL562;_LL562:;}goto _LL55D;_LL560:;_LL561:({void*_tmp854[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp855="format descriptor is not a char ? type";_tag_dynforward(
_tmp855,sizeof(char),_get_zero_arr_size(_tmp855,39));}),_tag_dynforward(_tmp854,
sizeof(void*),0));});goto _LL55D;_LL55D:;}if(fmt_arg_start != 0){int problem;{
struct _tuple6 _tmp857=({struct _tuple6 _tmp856;_tmp856.f1=ft;_tmp856.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp776))->type);_tmp856;});void*
_tmp858;void*_tmp859;struct Cyc_Absyn_TunionInfo _tmp85A;void*_tmp85B;struct Cyc_Absyn_Tuniondecl**
_tmp85C;struct Cyc_Absyn_Tuniondecl*_tmp85D;void*_tmp85E;void*_tmp85F;struct Cyc_Absyn_TunionInfo
_tmp860;void*_tmp861;struct Cyc_Absyn_Tuniondecl**_tmp862;struct Cyc_Absyn_Tuniondecl*
_tmp863;_LL568: _tmp858=_tmp857.f1;if((int)_tmp858 != 0)goto _LL56A;_tmp859=_tmp857.f2;
if(_tmp859 <= (void*)4)goto _LL56A;if(*((int*)_tmp859)!= 2)goto _LL56A;_tmp85A=((
struct Cyc_Absyn_TunionType_struct*)_tmp859)->f1;_tmp85B=(void*)_tmp85A.tunion_info;
if(*((int*)_tmp85B)!= 1)goto _LL56A;_tmp85C=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp85B)->f1;_tmp85D=*_tmp85C;_LL569: problem=Cyc_Absyn_qvar_cmp(_tmp85D->name,
Cyc_Absyn_tunion_print_arg_qvar)!= 0;goto _LL567;_LL56A: _tmp85E=_tmp857.f1;if((
int)_tmp85E != 1)goto _LL56C;_tmp85F=_tmp857.f2;if(_tmp85F <= (void*)4)goto _LL56C;
if(*((int*)_tmp85F)!= 2)goto _LL56C;_tmp860=((struct Cyc_Absyn_TunionType_struct*)
_tmp85F)->f1;_tmp861=(void*)_tmp860.tunion_info;if(*((int*)_tmp861)!= 1)goto
_LL56C;_tmp862=((struct Cyc_Absyn_KnownTunion_struct*)_tmp861)->f1;_tmp863=*
_tmp862;_LL56B: problem=Cyc_Absyn_qvar_cmp(_tmp863->name,Cyc_Absyn_tunion_scanf_arg_qvar)
!= 0;goto _LL567;_LL56C:;_LL56D: problem=1;goto _LL567;_LL567:;}if(problem)({void*
_tmp864[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp865="format attribute and vararg types don't match";
_tag_dynforward(_tmp865,sizeof(char),_get_zero_arr_size(_tmp865,46));}),
_tag_dynforward(_tmp864,sizeof(void*),0));});}}}{struct Cyc_List_List*rpo=_tmp777;
for(0;rpo != 0;rpo=rpo->tl){struct _tuple6 _tmp867;void*_tmp868;void*_tmp869;struct
_tuple6*_tmp866=(struct _tuple6*)rpo->hd;_tmp867=*_tmp866;_tmp868=_tmp867.f1;
_tmp869=_tmp867.f2;_tmp834=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp834,(void*)4,
_tmp868,0);_tmp834=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp834,(void*)3,_tmp869,
0);}}if(*_tmp772 != 0)_tmp834=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp834,(void*)
4,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp772))->v,0);else{struct Cyc_List_List*
effect=0;{struct Cyc_List_List*tvs=_tmp834.free_vars;for(0;tvs != 0;tvs=tvs->tl){
void*_tmp86A=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);
struct Cyc_Core_Opt*_tmp86B;struct Cyc_Core_Opt**_tmp86C;void*_tmp86D;void*_tmp86E;
void*_tmp86F;void*_tmp870;struct Cyc_Core_Opt*_tmp871;struct Cyc_Core_Opt**_tmp872;
void*_tmp873;void*_tmp874;struct Cyc_Core_Opt*_tmp875;struct Cyc_Core_Opt**_tmp876;
_LL56F: if(*((int*)_tmp86A)!= 2)goto _LL571;_tmp86B=((struct Cyc_Absyn_Less_kb_struct*)
_tmp86A)->f1;_tmp86C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp86A)->f1;_tmp86D=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp86A)->f2;if((
int)_tmp86D != 3)goto _LL571;_LL570:*_tmp86C=({struct Cyc_Core_Opt*_tmp877=
_cycalloc(sizeof(*_tmp877));_tmp877->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp878=_cycalloc(sizeof(*_tmp878));_tmp878[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp879;_tmp879.tag=0;_tmp879.f1=(void*)((void*)3);_tmp879;});_tmp878;}));
_tmp877;});goto _LL572;_LL571: if(*((int*)_tmp86A)!= 0)goto _LL573;_tmp86E=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp86A)->f1;if((int)_tmp86E != 3)goto _LL573;_LL572:
effect=({struct Cyc_List_List*_tmp87A=_cycalloc(sizeof(*_tmp87A));_tmp87A->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp87B=_cycalloc(sizeof(*
_tmp87B));_tmp87B[0]=({struct Cyc_Absyn_AccessEff_struct _tmp87C;_tmp87C.tag=19;
_tmp87C.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp87D=_cycalloc(
sizeof(*_tmp87D));_tmp87D[0]=({struct Cyc_Absyn_VarType_struct _tmp87E;_tmp87E.tag=
1;_tmp87E.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp87E;});_tmp87D;}));_tmp87C;});
_tmp87B;}));_tmp87A->tl=effect;_tmp87A;});goto _LL56E;_LL573: if(*((int*)_tmp86A)
!= 2)goto _LL575;_tmp86F=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp86A)->f2;
if((int)_tmp86F != 5)goto _LL575;_LL574: goto _LL576;_LL575: if(*((int*)_tmp86A)!= 0)
goto _LL577;_tmp870=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp86A)->f1;if((int)
_tmp870 != 5)goto _LL577;_LL576: goto _LL56E;_LL577: if(*((int*)_tmp86A)!= 2)goto
_LL579;_tmp871=((struct Cyc_Absyn_Less_kb_struct*)_tmp86A)->f1;_tmp872=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp86A)->f1;_tmp873=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp86A)->f2;if((int)_tmp873 != 4)goto _LL579;_LL578:*_tmp872=({struct Cyc_Core_Opt*
_tmp87F=_cycalloc(sizeof(*_tmp87F));_tmp87F->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp880=_cycalloc(sizeof(*_tmp880));_tmp880[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp881;_tmp881.tag=0;_tmp881.f1=(void*)((void*)4);_tmp881;});_tmp880;}));
_tmp87F;});goto _LL57A;_LL579: if(*((int*)_tmp86A)!= 0)goto _LL57B;_tmp874=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp86A)->f1;if((int)_tmp874 != 4)goto _LL57B;_LL57A:
effect=({struct Cyc_List_List*_tmp882=_cycalloc(sizeof(*_tmp882));_tmp882->hd=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp883=_cycalloc(sizeof(*_tmp883));
_tmp883[0]=({struct Cyc_Absyn_VarType_struct _tmp884;_tmp884.tag=1;_tmp884.f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp884;});_tmp883;}));_tmp882->tl=effect;_tmp882;});
goto _LL56E;_LL57B: if(*((int*)_tmp86A)!= 1)goto _LL57D;_tmp875=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp86A)->f1;_tmp876=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp86A)->f1;_LL57C:*_tmp876=({struct Cyc_Core_Opt*_tmp885=_cycalloc(sizeof(*
_tmp885));_tmp885->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp886=
_cycalloc(sizeof(*_tmp886));_tmp886[0]=({struct Cyc_Absyn_Less_kb_struct _tmp887;
_tmp887.tag=2;_tmp887.f1=0;_tmp887.f2=(void*)((void*)0);_tmp887;});_tmp886;}));
_tmp885;});goto _LL57E;_LL57D:;_LL57E: effect=({struct Cyc_List_List*_tmp888=
_cycalloc(sizeof(*_tmp888));_tmp888->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp889=_cycalloc(sizeof(*_tmp889));_tmp889[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp88A;_tmp88A.tag=21;_tmp88A.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp88B=_cycalloc(sizeof(*_tmp88B));_tmp88B[0]=({struct Cyc_Absyn_VarType_struct
_tmp88C;_tmp88C.tag=1;_tmp88C.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp88C;});
_tmp88B;}));_tmp88A;});_tmp889;}));_tmp888->tl=effect;_tmp888;});goto _LL56E;
_LL56E:;}}{struct Cyc_List_List*ts=_tmp834.free_evars;for(0;ts != 0;ts=ts->tl){
void*_tmp88D=Cyc_Tcutil_typ_kind((void*)ts->hd);_LL580: if((int)_tmp88D != 3)goto
_LL582;_LL581: effect=({struct Cyc_List_List*_tmp88E=_cycalloc(sizeof(*_tmp88E));
_tmp88E->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp88F=_cycalloc(
sizeof(*_tmp88F));_tmp88F[0]=({struct Cyc_Absyn_AccessEff_struct _tmp890;_tmp890.tag=
19;_tmp890.f1=(void*)((void*)ts->hd);_tmp890;});_tmp88F;}));_tmp88E->tl=effect;
_tmp88E;});goto _LL57F;_LL582: if((int)_tmp88D != 4)goto _LL584;_LL583: effect=({
struct Cyc_List_List*_tmp891=_cycalloc(sizeof(*_tmp891));_tmp891->hd=(void*)((
void*)ts->hd);_tmp891->tl=effect;_tmp891;});goto _LL57F;_LL584: if((int)_tmp88D != 
5)goto _LL586;_LL585: goto _LL57F;_LL586:;_LL587: effect=({struct Cyc_List_List*
_tmp892=_cycalloc(sizeof(*_tmp892));_tmp892->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp893=_cycalloc(sizeof(*_tmp893));_tmp893[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp894;_tmp894.tag=21;_tmp894.f1=(void*)((void*)ts->hd);_tmp894;});_tmp893;}));
_tmp892->tl=effect;_tmp892;});goto _LL57F;_LL57F:;}}*_tmp772=({struct Cyc_Core_Opt*
_tmp895=_cycalloc(sizeof(*_tmp895));_tmp895->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp896=_cycalloc(sizeof(*_tmp896));_tmp896[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp897;_tmp897.tag=20;_tmp897.f1=effect;_tmp897;});_tmp896;}));_tmp895;});}if(*
_tmp770 != 0){struct Cyc_List_List*bs=*_tmp770;for(0;bs != 0;bs=bs->tl){void*
_tmp898=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp899;struct Cyc_Core_Opt**_tmp89A;struct Cyc_Core_Opt*
_tmp89B;struct Cyc_Core_Opt**_tmp89C;void*_tmp89D;struct Cyc_Core_Opt*_tmp89E;
struct Cyc_Core_Opt**_tmp89F;void*_tmp8A0;struct Cyc_Core_Opt*_tmp8A1;struct Cyc_Core_Opt**
_tmp8A2;void*_tmp8A3;void*_tmp8A4;_LL589: if(*((int*)_tmp898)!= 1)goto _LL58B;
_tmp899=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp898)->f1;_tmp89A=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp898)->f1;_LL58A:({struct Cyc_String_pa_struct
_tmp8A7;_tmp8A7.tag=0;_tmp8A7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)bs->hd)->name);{void*_tmp8A5[1]={& _tmp8A7};Cyc_Tcutil_warn(
loc,({const char*_tmp8A6="Type variable %s unconstrained, assuming boxed";
_tag_dynforward(_tmp8A6,sizeof(char),_get_zero_arr_size(_tmp8A6,47));}),
_tag_dynforward(_tmp8A5,sizeof(void*),1));}});_tmp89C=_tmp89A;goto _LL58C;_LL58B:
if(*((int*)_tmp898)!= 2)goto _LL58D;_tmp89B=((struct Cyc_Absyn_Less_kb_struct*)
_tmp898)->f1;_tmp89C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp898)->f1;_tmp89D=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp898)->f2;if((
int)_tmp89D != 1)goto _LL58D;_LL58C: _tmp89F=_tmp89C;goto _LL58E;_LL58D: if(*((int*)
_tmp898)!= 2)goto _LL58F;_tmp89E=((struct Cyc_Absyn_Less_kb_struct*)_tmp898)->f1;
_tmp89F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp898)->f1;
_tmp8A0=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp898)->f2;if((int)_tmp8A0 != 
0)goto _LL58F;_LL58E:*_tmp89F=({struct Cyc_Core_Opt*_tmp8A8=_cycalloc(sizeof(*
_tmp8A8));_tmp8A8->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8A9=
_cycalloc(sizeof(*_tmp8A9));_tmp8A9[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp8AA;
_tmp8AA.tag=0;_tmp8AA.f1=(void*)((void*)2);_tmp8AA;});_tmp8A9;}));_tmp8A8;});
goto _LL588;_LL58F: if(*((int*)_tmp898)!= 2)goto _LL591;_tmp8A1=((struct Cyc_Absyn_Less_kb_struct*)
_tmp898)->f1;_tmp8A2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp898)->f1;_tmp8A3=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp898)->f2;
_LL590:*_tmp8A2=({struct Cyc_Core_Opt*_tmp8AB=_cycalloc(sizeof(*_tmp8AB));_tmp8AB->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8AC=_cycalloc(sizeof(*_tmp8AC));
_tmp8AC[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp8AD;_tmp8AD.tag=0;_tmp8AD.f1=(void*)
_tmp8A3;_tmp8AD;});_tmp8AC;}));_tmp8AB;});goto _LL588;_LL591: if(*((int*)_tmp898)
!= 0)goto _LL593;_tmp8A4=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp898)->f1;if((
int)_tmp8A4 != 1)goto _LL593;_LL592:({void*_tmp8AE[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp8AF="functions can't abstract M types";_tag_dynforward(_tmp8AF,
sizeof(char),_get_zero_arr_size(_tmp8AF,33));}),_tag_dynforward(_tmp8AE,sizeof(
void*),0));});goto _LL588;_LL593:;_LL594: goto _LL588;_LL588:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
_tmp834.kind_env,*_tmp770);_tmp834.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmp834.free_vars,*_tmp770);{struct Cyc_List_List*tvs=_tmp834.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,(
struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=_tmp834.free_evars;
for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,(
void*)evs->hd);}}goto _LL4CB;}}_LL4EA: if(*((int*)_tmp747)!= 9)goto _LL4EC;_tmp779=((
struct Cyc_Absyn_TupleType_struct*)_tmp747)->f1;_LL4EB: for(0;_tmp779 != 0;_tmp779=
_tmp779->tl){struct _tuple4*_tmp8B1=(struct _tuple4*)_tmp779->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,(*_tmp8B1).f2,0);((*_tmp8B1).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp8B1).f1).print_const,(*_tmp8B1).f2);}goto _LL4CB;_LL4EC: if(*((int*)
_tmp747)!= 11)goto _LL4EE;_tmp77A=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp747)->f1;_tmp77B=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp747)->f2;_LL4ED:{
struct _RegionHandle _tmp8B2=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp8B2;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp77B != 0;_tmp77B=_tmp77B->tl){struct Cyc_Absyn_Aggrfield _tmp8B4;struct
_dynforward_ptr*_tmp8B5;struct Cyc_Absyn_Tqual _tmp8B6;struct Cyc_Absyn_Tqual*
_tmp8B7;void*_tmp8B8;struct Cyc_Absyn_Exp*_tmp8B9;struct Cyc_List_List*_tmp8BA;
struct Cyc_Absyn_Aggrfield*_tmp8B3=(struct Cyc_Absyn_Aggrfield*)_tmp77B->hd;
_tmp8B4=*_tmp8B3;_tmp8B5=_tmp8B4.name;_tmp8B6=_tmp8B4.tq;_tmp8B7=(struct Cyc_Absyn_Tqual*)&(*
_tmp8B3).tq;_tmp8B8=(void*)_tmp8B4.type;_tmp8B9=_tmp8B4.width;_tmp8BA=_tmp8B4.attributes;
if(((int(*)(int(*compare)(struct _dynforward_ptr*,struct _dynforward_ptr*),struct
Cyc_List_List*l,struct _dynforward_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,
_tmp8B5))({struct Cyc_String_pa_struct _tmp8BD;_tmp8BD.tag=0;_tmp8BD.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp8B5);{void*_tmp8BB[1]={& _tmp8BD};
Cyc_Tcutil_terr(loc,({const char*_tmp8BC="duplicate field %s";_tag_dynforward(
_tmp8BC,sizeof(char),_get_zero_arr_size(_tmp8BC,19));}),_tag_dynforward(_tmp8BB,
sizeof(void*),1));}});if(Cyc_strcmp((struct _dynforward_ptr)*_tmp8B5,({const char*
_tmp8BE="";_tag_dynforward(_tmp8BE,sizeof(char),_get_zero_arr_size(_tmp8BE,1));}))
!= 0)prev_fields=({struct Cyc_List_List*_tmp8BF=_region_malloc(aprev_rgn,sizeof(*
_tmp8BF));_tmp8BF->hd=_tmp8B5;_tmp8BF->tl=prev_fields;_tmp8BF;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,_tmp8B8,0);_tmp8B7->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp8B7->print_const,_tmp8B8);if(_tmp77A == (void*)1  && !Cyc_Tcutil_bits_only(
_tmp8B8))({struct Cyc_String_pa_struct _tmp8C2;_tmp8C2.tag=0;_tmp8C2.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp8B5);{void*_tmp8C0[1]={& _tmp8C2};
Cyc_Tcutil_warn(loc,({const char*_tmp8C1="union member %s is not `bits-only' so it can only be written and not read";
_tag_dynforward(_tmp8C1,sizeof(char),_get_zero_arr_size(_tmp8C1,74));}),
_tag_dynforward(_tmp8C0,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(loc,te,
_tmp8B8,_tmp8B9,_tmp8B5);Cyc_Tcutil_check_field_atts(loc,_tmp8B5,_tmp8BA);}};
_pop_region(aprev_rgn);}goto _LL4CB;_LL4EE: if(*((int*)_tmp747)!= 10)goto _LL4F0;
_tmp77C=((struct Cyc_Absyn_AggrType_struct*)_tmp747)->f1;_tmp77D=(void*)_tmp77C.aggr_info;
_tmp77E=(void**)&(((struct Cyc_Absyn_AggrType_struct*)_tmp747)->f1).aggr_info;
_tmp77F=_tmp77C.targs;_tmp780=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp747)->f1).targs;_LL4EF:{void*_tmp8C3=*_tmp77E;void*_tmp8C4;struct _tuple1*
_tmp8C5;struct Cyc_Absyn_Aggrdecl**_tmp8C6;struct Cyc_Absyn_Aggrdecl*_tmp8C7;
_LL596: if(*((int*)_tmp8C3)!= 0)goto _LL598;_tmp8C4=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp8C3)->f1;_tmp8C5=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp8C3)->f2;_LL597: {
struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp8C8;_push_handler(& _tmp8C8);{
int _tmp8CA=0;if(setjmp(_tmp8C8.handler))_tmp8CA=1;if(!_tmp8CA){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp8C5);*_tmp77E=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp8CB=
_cycalloc(sizeof(*_tmp8CB));_tmp8CB[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp8CC;
_tmp8CC.tag=1;_tmp8CC.f1=adp;_tmp8CC;});_tmp8CB;});;_pop_handler();}else{void*
_tmp8C9=(void*)_exn_thrown;void*_tmp8CE=_tmp8C9;_LL59B: if(_tmp8CE != Cyc_Dict_Absent)
goto _LL59D;_LL59C: {struct Cyc_Tcenv_Genv*_tmp8CF=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp8D0=({struct Cyc_Absyn_Aggrdecl*_tmp8D6=_cycalloc(
sizeof(*_tmp8D6));_tmp8D6->kind=(void*)_tmp8C4;_tmp8D6->sc=(void*)((void*)3);
_tmp8D6->name=_tmp8C5;_tmp8D6->tvs=0;_tmp8D6->impl=0;_tmp8D6->attributes=0;
_tmp8D6;});Cyc_Tc_tcAggrdecl(te,_tmp8CF,loc,_tmp8D0);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp8C5);*_tmp77E=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp8D1=
_cycalloc(sizeof(*_tmp8D1));_tmp8D1[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp8D2;
_tmp8D2.tag=1;_tmp8D2.f1=adp;_tmp8D2;});_tmp8D1;});if(*_tmp780 != 0){({struct Cyc_String_pa_struct
_tmp8D5;_tmp8D5.tag=0;_tmp8D5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp8C5));{void*_tmp8D3[1]={& _tmp8D5};Cyc_Tcutil_terr(
loc,({const char*_tmp8D4="declare parameterized type %s before using";
_tag_dynforward(_tmp8D4,sizeof(char),_get_zero_arr_size(_tmp8D4,43));}),
_tag_dynforward(_tmp8D3,sizeof(void*),1));}});return cvtenv;}goto _LL59A;}_LL59D:;
_LL59E:(void)_throw(_tmp8CE);_LL59A:;}}}_tmp8C7=*adp;goto _LL599;}_LL598: if(*((
int*)_tmp8C3)!= 1)goto _LL595;_tmp8C6=((struct Cyc_Absyn_KnownAggr_struct*)_tmp8C3)->f1;
_tmp8C7=*_tmp8C6;_LL599: {struct Cyc_List_List*tvs=_tmp8C7->tvs;struct Cyc_List_List*
ts=*_tmp780;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd,0);}if(ts != 0)({struct Cyc_String_pa_struct _tmp8D9;_tmp8D9.tag=
0;_tmp8D9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp8C7->name));{void*_tmp8D7[1]={& _tmp8D9};Cyc_Tcutil_terr(loc,({const char*
_tmp8D8="too many parameters for type %s";_tag_dynforward(_tmp8D8,sizeof(char),
_get_zero_arr_size(_tmp8D8,32));}),_tag_dynforward(_tmp8D7,sizeof(void*),1));}});
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);hidden_ts=({struct Cyc_List_List*_tmp8DA=_cycalloc(sizeof(*_tmp8DA));_tmp8DA->hd=(
void*)e;_tmp8DA->tl=hidden_ts;_tmp8DA;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,0);}*_tmp780=Cyc_List_imp_append(*_tmp780,Cyc_List_imp_rev(
hidden_ts));}}_LL595:;}goto _LL4CB;_LL4F0: if(*((int*)_tmp747)!= 16)goto _LL4F2;
_tmp781=((struct Cyc_Absyn_TypedefType_struct*)_tmp747)->f1;_tmp782=((struct Cyc_Absyn_TypedefType_struct*)
_tmp747)->f2;_tmp783=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp747)->f2;_tmp784=((struct Cyc_Absyn_TypedefType_struct*)_tmp747)->f3;_tmp785=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp747)->f3;
_tmp786=((struct Cyc_Absyn_TypedefType_struct*)_tmp747)->f4;_tmp787=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp747)->f4;_LL4F1: {struct Cyc_List_List*
targs=*_tmp783;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp8DB;
_push_handler(& _tmp8DB);{int _tmp8DD=0;if(setjmp(_tmp8DB.handler))_tmp8DD=1;if(!
_tmp8DD){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp781);;_pop_handler();}else{
void*_tmp8DC=(void*)_exn_thrown;void*_tmp8DF=_tmp8DC;_LL5A0: if(_tmp8DF != Cyc_Dict_Absent)
goto _LL5A2;_LL5A1:({struct Cyc_String_pa_struct _tmp8E2;_tmp8E2.tag=0;_tmp8E2.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp781));{
void*_tmp8E0[1]={& _tmp8E2};Cyc_Tcutil_terr(loc,({const char*_tmp8E1="unbound typedef name %s";
_tag_dynforward(_tmp8E1,sizeof(char),_get_zero_arr_size(_tmp8E1,24));}),
_tag_dynforward(_tmp8E0,sizeof(void*),1));}});return cvtenv;_LL5A2:;_LL5A3:(void)
_throw(_tmp8DF);_LL59F:;}}}*_tmp785=(struct Cyc_Absyn_Typedefdecl*)td;_tmp781[0]=(
td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(
ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,0);inst=({
struct Cyc_List_List*_tmp8E3=_cycalloc(sizeof(*_tmp8E3));_tmp8E3->hd=({struct
_tuple8*_tmp8E4=_cycalloc(sizeof(*_tmp8E4));_tmp8E4->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp8E4->f2=(void*)ts->hd;_tmp8E4;});_tmp8E3->tl=inst;_tmp8E3;});}if(ts
!= 0)({struct Cyc_String_pa_struct _tmp8E7;_tmp8E7.tag=0;_tmp8E7.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp781));{void*
_tmp8E5[1]={& _tmp8E7};Cyc_Tcutil_terr(loc,({const char*_tmp8E6="too many parameters for typedef %s";
_tag_dynforward(_tmp8E6,sizeof(char),_get_zero_arr_size(_tmp8E6,35));}),
_tag_dynforward(_tmp8E5,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp8E8=
_cycalloc(sizeof(*_tmp8E8));_tmp8E8->hd=(void*)e;_tmp8E8->tl=hidden_ts;_tmp8E8;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,0);inst=({struct Cyc_List_List*
_tmp8E9=_cycalloc(sizeof(*_tmp8E9));_tmp8E9->hd=({struct _tuple8*_tmp8EA=
_cycalloc(sizeof(*_tmp8EA));_tmp8EA->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp8EA->f2=
e;_tmp8EA;});_tmp8E9->tl=inst;_tmp8E9;});}*_tmp783=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_substitute(inst,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp787=({void**_tmp8EB=
_cycalloc(sizeof(*_tmp8EB));_tmp8EB[0]=new_typ;_tmp8EB;});}goto _LL4CB;}}_LL4F2:
if((int)_tmp747 != 3)goto _LL4F4;_LL4F3: if(!allow_unique)({void*_tmp8EC[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp8ED="`U only allowed in outermost pointers";_tag_dynforward(
_tmp8ED,sizeof(char),_get_zero_arr_size(_tmp8ED,38));}),_tag_dynforward(_tmp8EC,
sizeof(void*),0));});goto _LL4CB;_LL4F4: if(_tmp747 <= (void*)4)goto _LL4F6;if(*((
int*)_tmp747)!= 18)goto _LL4F6;_LL4F5: goto _LL4F7;_LL4F6: if((int)_tmp747 != 2)goto
_LL4F8;_LL4F7: goto _LL4CB;_LL4F8: if(_tmp747 <= (void*)4)goto _LL4FA;if(*((int*)
_tmp747)!= 15)goto _LL4FA;_tmp788=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp747)->f1;_LL4F9: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp788,1);goto _LL4CB;_LL4FA: if(_tmp747 <= (void*)4)goto _LL4FC;if(*((int*)_tmp747)
!= 19)goto _LL4FC;_tmp789=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp747)->f1;
_LL4FB: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp789,0);
goto _LL4CB;_LL4FC: if(_tmp747 <= (void*)4)goto _LL4FE;if(*((int*)_tmp747)!= 21)goto
_LL4FE;_tmp78A=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp747)->f1;_LL4FD:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp78A,0);goto
_LL4CB;_LL4FE: if(_tmp747 <= (void*)4)goto _LL4CB;if(*((int*)_tmp747)!= 20)goto
_LL4CB;_tmp78B=((struct Cyc_Absyn_JoinEff_struct*)_tmp747)->f1;_LL4FF: for(0;
_tmp78B != 0;_tmp78B=_tmp78B->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)4,(void*)_tmp78B->hd,0);}goto _LL4CB;_LL4CB:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind))({struct Cyc_String_pa_struct _tmp8F2;
_tmp8F2.tag=0;_tmp8F2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(
expected_kind));{struct Cyc_String_pa_struct _tmp8F1;_tmp8F1.tag=0;_tmp8F1.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(
t)));{struct Cyc_String_pa_struct _tmp8F0;_tmp8F0.tag=0;_tmp8F0.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp8EE[
3]={& _tmp8F0,& _tmp8F1,& _tmp8F2};Cyc_Tcutil_terr(loc,({const char*_tmp8EF="type %s has kind %s but as used here needs kind %s";
_tag_dynforward(_tmp8EF,sizeof(char),_get_zero_arr_size(_tmp8EF,51));}),
_tag_dynforward(_tmp8EE,sizeof(void*),3));}}}});return cvtenv;}static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*kind_env,void*expected_kind,int generalize_evars,void*t,
int allow_unique){struct Cyc_Tcutil_CVTEnv _tmp8F3=Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp8FC;_tmp8FC.kind_env=kind_env;_tmp8FC.free_vars=
0;_tmp8FC.free_evars=0;_tmp8FC.generalize_evars=generalize_evars;_tmp8FC.fn_result=
0;_tmp8FC;}),expected_kind,t,allow_unique);{struct Cyc_List_List*vs=_tmp8F3.free_vars;
for(0;vs != 0;vs=vs->tl){_tmp8F3.kind_env=Cyc_Tcutil_fast_add_free_tvar(kind_env,(
struct Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp8F3.free_evars;for(
0;evs != 0;evs=evs->tl){void*_tmp8F4=Cyc_Tcutil_compress((void*)evs->hd);struct
Cyc_Core_Opt*_tmp8F5;struct Cyc_Core_Opt**_tmp8F6;_LL5A5: if(_tmp8F4 <= (void*)4)
goto _LL5A7;if(*((int*)_tmp8F4)!= 0)goto _LL5A7;_tmp8F5=((struct Cyc_Absyn_Evar_struct*)
_tmp8F4)->f4;_tmp8F6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp8F4)->f4;_LL5A6: if(*_tmp8F6 == 0)*_tmp8F6=({struct Cyc_Core_Opt*_tmp8F7=
_cycalloc(sizeof(*_tmp8F7));_tmp8F7->v=kind_env;_tmp8F7;});else{struct Cyc_List_List*
_tmp8F8=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp8F6))->v;
struct Cyc_List_List*_tmp8F9=0;for(0;_tmp8F8 != 0;_tmp8F8=_tmp8F8->tl){if(((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct
Cyc_Absyn_Tvar*)_tmp8F8->hd))_tmp8F9=({struct Cyc_List_List*_tmp8FA=_cycalloc(
sizeof(*_tmp8FA));_tmp8FA->hd=(struct Cyc_Absyn_Tvar*)_tmp8F8->hd;_tmp8FA->tl=
_tmp8F9;_tmp8FA;});}*_tmp8F6=({struct Cyc_Core_Opt*_tmp8FB=_cycalloc(sizeof(*
_tmp8FB));_tmp8FB->v=_tmp8F9;_tmp8FB;});}goto _LL5A4;_LL5A7:;_LL5A8: goto _LL5A4;
_LL5A4:;}}return _tmp8F3;}void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*t,int allow_unique){int generalize_evars=Cyc_Tcutil_is_function_type(
t);struct Cyc_List_List*_tmp8FD=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Tcutil_CVTEnv
_tmp8FE=Cyc_Tcutil_check_valid_type(loc,te,_tmp8FD,(void*)1,generalize_evars,t,
allow_unique);struct Cyc_List_List*_tmp8FF=_tmp8FE.free_vars;struct Cyc_List_List*
_tmp900=_tmp8FE.free_evars;if(_tmp8FD != 0){struct Cyc_List_List*_tmp901=0;{struct
Cyc_List_List*_tmp902=_tmp8FF;for(0;(unsigned int)_tmp902;_tmp902=_tmp902->tl){
struct Cyc_Absyn_Tvar*_tmp903=(struct Cyc_Absyn_Tvar*)_tmp902->hd;int found=0;{
struct Cyc_List_List*_tmp904=_tmp8FD;for(0;(unsigned int)_tmp904;_tmp904=_tmp904->tl){
if(Cyc_Absyn_tvar_cmp(_tmp903,(struct Cyc_Absyn_Tvar*)_tmp904->hd)== 0){found=1;
break;}}}if(!found)_tmp901=({struct Cyc_List_List*_tmp905=_cycalloc(sizeof(*
_tmp905));_tmp905->hd=(struct Cyc_Absyn_Tvar*)_tmp902->hd;_tmp905->tl=_tmp901;
_tmp905;});}}_tmp8FF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp901);}{struct Cyc_List_List*x=_tmp8FF;for(0;x != 0;x=x->tl){void*_tmp906=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmp907;struct Cyc_Core_Opt**
_tmp908;struct Cyc_Core_Opt*_tmp909;struct Cyc_Core_Opt**_tmp90A;void*_tmp90B;
struct Cyc_Core_Opt*_tmp90C;struct Cyc_Core_Opt**_tmp90D;void*_tmp90E;struct Cyc_Core_Opt*
_tmp90F;struct Cyc_Core_Opt**_tmp910;void*_tmp911;void*_tmp912;_LL5AA: if(*((int*)
_tmp906)!= 1)goto _LL5AC;_tmp907=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp906)->f1;
_tmp908=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp906)->f1;
_LL5AB: _tmp90A=_tmp908;goto _LL5AD;_LL5AC: if(*((int*)_tmp906)!= 2)goto _LL5AE;
_tmp909=((struct Cyc_Absyn_Less_kb_struct*)_tmp906)->f1;_tmp90A=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp906)->f1;_tmp90B=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp906)->f2;if((int)_tmp90B != 1)goto _LL5AE;_LL5AD: _tmp90D=_tmp90A;goto _LL5AF;
_LL5AE: if(*((int*)_tmp906)!= 2)goto _LL5B0;_tmp90C=((struct Cyc_Absyn_Less_kb_struct*)
_tmp906)->f1;_tmp90D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp906)->f1;_tmp90E=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp906)->f2;if((
int)_tmp90E != 0)goto _LL5B0;_LL5AF:*_tmp90D=({struct Cyc_Core_Opt*_tmp913=
_cycalloc(sizeof(*_tmp913));_tmp913->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp914=_cycalloc(sizeof(*_tmp914));_tmp914[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp915;_tmp915.tag=0;_tmp915.f1=(void*)((void*)2);_tmp915;});_tmp914;}));
_tmp913;});goto _LL5A9;_LL5B0: if(*((int*)_tmp906)!= 2)goto _LL5B2;_tmp90F=((struct
Cyc_Absyn_Less_kb_struct*)_tmp906)->f1;_tmp910=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp906)->f1;_tmp911=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp906)->f2;_LL5B1:*_tmp910=({struct Cyc_Core_Opt*_tmp916=_cycalloc(sizeof(*
_tmp916));_tmp916->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp917=
_cycalloc(sizeof(*_tmp917));_tmp917[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp918;
_tmp918.tag=0;_tmp918.f1=(void*)_tmp911;_tmp918;});_tmp917;}));_tmp916;});goto
_LL5A9;_LL5B2: if(*((int*)_tmp906)!= 0)goto _LL5B4;_tmp912=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp906)->f1;if((int)_tmp912 != 1)goto _LL5B4;_LL5B3:({struct Cyc_String_pa_struct
_tmp91B;_tmp91B.tag=0;_tmp91B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));{void*_tmp919[1]={& _tmp91B};
Cyc_Tcutil_terr(loc,({const char*_tmp91A="type variable %s cannot have kind M";
_tag_dynforward(_tmp91A,sizeof(char),_get_zero_arr_size(_tmp91A,36));}),
_tag_dynforward(_tmp919,sizeof(void*),1));}});goto _LL5A9;_LL5B4:;_LL5B5: goto
_LL5A9;_LL5A9:;}}if(_tmp8FF != 0  || _tmp900 != 0){{void*_tmp91C=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp91D;struct Cyc_List_List*_tmp91E;struct Cyc_List_List**
_tmp91F;struct Cyc_Core_Opt*_tmp920;void*_tmp921;struct Cyc_List_List*_tmp922;int
_tmp923;struct Cyc_Absyn_VarargInfo*_tmp924;struct Cyc_List_List*_tmp925;struct Cyc_List_List*
_tmp926;_LL5B7: if(_tmp91C <= (void*)4)goto _LL5B9;if(*((int*)_tmp91C)!= 8)goto
_LL5B9;_tmp91D=((struct Cyc_Absyn_FnType_struct*)_tmp91C)->f1;_tmp91E=_tmp91D.tvars;
_tmp91F=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmp91C)->f1).tvars;
_tmp920=_tmp91D.effect;_tmp921=(void*)_tmp91D.ret_typ;_tmp922=_tmp91D.args;
_tmp923=_tmp91D.c_varargs;_tmp924=_tmp91D.cyc_varargs;_tmp925=_tmp91D.rgn_po;
_tmp926=_tmp91D.attributes;_LL5B8: if(*_tmp91F == 0){*_tmp91F=_tmp8FF;_tmp8FF=0;}
goto _LL5B6;_LL5B9:;_LL5BA: goto _LL5B6;_LL5B6:;}if(_tmp8FF != 0)({struct Cyc_String_pa_struct
_tmp929;_tmp929.tag=0;_tmp929.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)_tmp8FF->hd)->name);{void*_tmp927[1]={& _tmp929};Cyc_Tcutil_terr(
loc,({const char*_tmp928="unbound type variable %s";_tag_dynforward(_tmp928,
sizeof(char),_get_zero_arr_size(_tmp928,25));}),_tag_dynforward(_tmp927,sizeof(
void*),1));}});if(_tmp900 != 0)for(0;_tmp900 != 0;_tmp900=_tmp900->tl){void*e=(
void*)_tmp900->hd;void*_tmp92A=Cyc_Tcutil_typ_kind(e);_LL5BC: if((int)_tmp92A != 3)
goto _LL5BE;_LL5BD: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp92B[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp92C="can't unify evar with heap!";_tag_dynforward(_tmp92C,sizeof(char),
_get_zero_arr_size(_tmp92C,28));}),_tag_dynforward(_tmp92B,sizeof(void*),0));});
goto _LL5BB;_LL5BE: if((int)_tmp92A != 4)goto _LL5C0;_LL5BF: if(!Cyc_Tcutil_unify(e,
Cyc_Absyn_empty_effect))({void*_tmp92D[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp92E="can't unify evar with {}!";
_tag_dynforward(_tmp92E,sizeof(char),_get_zero_arr_size(_tmp92E,26));}),
_tag_dynforward(_tmp92D,sizeof(void*),0));});goto _LL5BB;_LL5C0:;_LL5C1:({struct
Cyc_String_pa_struct _tmp932;_tmp932.tag=0;_tmp932.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp931;_tmp931.tag=0;_tmp931.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(e));{void*_tmp92F[2]={& _tmp931,& _tmp932};Cyc_Tcutil_terr(
loc,({const char*_tmp930="hidden type variable %s isn't abstracted in type %s";
_tag_dynforward(_tmp930,sizeof(char),_get_zero_arr_size(_tmp930,52));}),
_tag_dynforward(_tmp92F,sizeof(void*),2));}}});goto _LL5BB;_LL5BB:;}}}void Cyc_Tcutil_check_no_unique_region(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp933;_tmp933.kind_env=0;_tmp933.free_vars=0;
_tmp933.free_evars=0;_tmp933.generalize_evars=0;_tmp933.fn_result=0;_tmp933;}),(
void*)1,t,0);}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t,1);{void*_tmp934=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp935;struct Cyc_List_List*_tmp936;struct Cyc_Core_Opt*
_tmp937;void*_tmp938;_LL5C3: if(_tmp934 <= (void*)4)goto _LL5C5;if(*((int*)_tmp934)
!= 8)goto _LL5C5;_tmp935=((struct Cyc_Absyn_FnType_struct*)_tmp934)->f1;_tmp936=
_tmp935.tvars;_tmp937=_tmp935.effect;_tmp938=(void*)_tmp935.ret_typ;_LL5C4: fd->tvs=
_tmp936;fd->effect=_tmp937;goto _LL5C2;_LL5C5:;_LL5C6:({void*_tmp939[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp93A="check_fndecl_valid_type: not a FnType";_tag_dynforward(_tmp93A,sizeof(
char),_get_zero_arr_size(_tmp93A,38));}),_tag_dynforward(_tmp939,sizeof(void*),0));});
_LL5C2:;}{struct _RegionHandle _tmp93B=_new_region("r");struct _RegionHandle*r=&
_tmp93B;_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr*(*f)(struct _tuple13*),struct Cyc_List_List*
x))Cyc_List_rmap)(r,(struct _dynforward_ptr*(*)(struct _tuple13*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,({const char*_tmp93C="function declaration has repeated parameter";
_tag_dynforward(_tmp93C,sizeof(char),_get_zero_arr_size(_tmp93C,44));}));;
_pop_region(r);}fd->cached_typ=({struct Cyc_Core_Opt*_tmp93D=_cycalloc(sizeof(*
_tmp93D));_tmp93D->v=(void*)t;_tmp93D;});}void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,void*expected_kind,
int allow_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp93E=Cyc_Tcutil_check_valid_type(
loc,te,bound_tvars,expected_kind,0,t,1);struct Cyc_List_List*_tmp93F=Cyc_Tcutil_remove_bound_tvars(
_tmp93E.free_vars,bound_tvars);struct Cyc_List_List*_tmp940=_tmp93E.free_evars;{
struct Cyc_List_List*fs=_tmp93F;for(0;fs != 0;fs=fs->tl){struct _dynforward_ptr*
_tmp941=((struct Cyc_Absyn_Tvar*)fs->hd)->name;({struct Cyc_String_pa_struct
_tmp945;_tmp945.tag=0;_tmp945.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct _tmp944;_tmp944.tag=0;
_tmp944.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp941);{void*
_tmp942[2]={& _tmp944,& _tmp945};Cyc_Tcutil_terr(loc,({const char*_tmp943="unbound type variable %s in type %s";
_tag_dynforward(_tmp943,sizeof(char),_get_zero_arr_size(_tmp943,36));}),
_tag_dynforward(_tmp942,sizeof(void*),2));}}});}}if(!allow_evars  && _tmp940 != 0)
for(0;_tmp940 != 0;_tmp940=_tmp940->tl){void*e=(void*)_tmp940->hd;void*_tmp946=
Cyc_Tcutil_typ_kind(e);_LL5C8: if((int)_tmp946 != 3)goto _LL5CA;_LL5C9: if(!Cyc_Tcutil_unify(
e,(void*)2))({void*_tmp947[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp948="can't unify evar with heap!";
_tag_dynforward(_tmp948,sizeof(char),_get_zero_arr_size(_tmp948,28));}),
_tag_dynforward(_tmp947,sizeof(void*),0));});goto _LL5C7;_LL5CA: if((int)_tmp946 != 
4)goto _LL5CC;_LL5CB: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*
_tmp949[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp94A="can't unify evar with {}!";_tag_dynforward(_tmp94A,sizeof(
char),_get_zero_arr_size(_tmp94A,26));}),_tag_dynforward(_tmp949,sizeof(void*),0));});
goto _LL5C7;_LL5CC:;_LL5CD:({struct Cyc_String_pa_struct _tmp94E;_tmp94E.tag=0;
_tmp94E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp94D;_tmp94D.tag=0;_tmp94D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(e));{void*_tmp94B[
2]={& _tmp94D,& _tmp94E};Cyc_Tcutil_terr(loc,({const char*_tmp94C="hidden type variable %s isn't abstracted in type %s";
_tag_dynforward(_tmp94C,sizeof(char),_get_zero_arr_size(_tmp94C,52));}),
_tag_dynforward(_tmp94B,sizeof(void*),2));}}});goto _LL5C7;_LL5C7:;}}void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)tv->identity=Cyc_Tcutil_new_tvar_id();}
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,
tvs);}static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct
Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dynforward_ptr(*a2string)(
void*),struct _dynforward_ptr msg){for(0;vs != 0;vs=vs->tl){struct Cyc_List_List*vs2=
vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(void*)vs2->hd)== 0)({
struct Cyc_String_pa_struct _tmp952;_tmp952.tag=0;_tmp952.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)a2string((void*)vs->hd));{struct Cyc_String_pa_struct
_tmp951;_tmp951.tag=0;_tmp951.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
msg);{void*_tmp94F[2]={& _tmp951,& _tmp952};Cyc_Tcutil_terr(loc,({const char*
_tmp950="%s: %s";_tag_dynforward(_tmp950,sizeof(char),_get_zero_arr_size(_tmp950,
7));}),_tag_dynforward(_tmp94F,sizeof(void*),2));}}});}}}static struct
_dynforward_ptr Cyc_Tcutil_strptr2string(struct _dynforward_ptr*s){return*s;}void
Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr msg){((void(*)(int(*cmp)(struct _dynforward_ptr*,struct
_dynforward_ptr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct
_dynforward_ptr(*a2string)(struct _dynforward_ptr*),struct _dynforward_ptr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){((void(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct
_dynforward_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,
Cyc_Tcutil_tvar2string,({const char*_tmp953="duplicate type variable";
_tag_dynforward(_tmp953,sizeof(char),_get_zero_arr_size(_tmp953,24));}));}struct
_tuple18{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple19{struct Cyc_List_List*
f1;void*f2;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){if(Cyc_strcmp((struct _dynforward_ptr)*((struct Cyc_Absyn_Aggrfield*)
sd_fields->hd)->name,({const char*_tmp954="";_tag_dynforward(_tmp954,sizeof(char),
_get_zero_arr_size(_tmp954,1));}))!= 0)fields=({struct Cyc_List_List*_tmp955=
_cycalloc(sizeof(*_tmp955));_tmp955->hd=({struct _tuple18*_tmp956=_cycalloc(
sizeof(*_tmp956));_tmp956->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd;_tmp956->f2=
0;_tmp956;});_tmp955->tl=fields;_tmp955;});}}fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct Cyc_List_List*ans=0;for(
0;des != 0;des=des->tl){struct _tuple19 _tmp958;struct Cyc_List_List*_tmp959;void*
_tmp95A;struct _tuple19*_tmp957=(struct _tuple19*)des->hd;_tmp958=*_tmp957;_tmp959=
_tmp958.f1;_tmp95A=_tmp958.f2;if(_tmp959 == 0){struct Cyc_List_List*_tmp95B=fields;
for(0;_tmp95B != 0;_tmp95B=_tmp95B->tl){if(!(*((struct _tuple18*)_tmp95B->hd)).f2){(*((
struct _tuple18*)_tmp95B->hd)).f2=1;(*((struct _tuple19*)des->hd)).f1=(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp95C=_cycalloc(sizeof(*_tmp95C));_tmp95C->hd=(void*)((
void*)({struct Cyc_Absyn_FieldName_struct*_tmp95D=_cycalloc(sizeof(*_tmp95D));
_tmp95D[0]=({struct Cyc_Absyn_FieldName_struct _tmp95E;_tmp95E.tag=1;_tmp95E.f1=((*((
struct _tuple18*)_tmp95B->hd)).f1)->name;_tmp95E;});_tmp95D;}));_tmp95C->tl=0;
_tmp95C;});ans=({struct Cyc_List_List*_tmp95F=_region_malloc(rgn,sizeof(*_tmp95F));
_tmp95F->hd=({struct _tuple20*_tmp960=_region_malloc(rgn,sizeof(*_tmp960));
_tmp960->f1=(*((struct _tuple18*)_tmp95B->hd)).f1;_tmp960->f2=_tmp95A;_tmp960;});
_tmp95F->tl=ans;_tmp95F;});break;}}if(_tmp95B == 0)({void*_tmp961[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp962="too many arguments to struct";_tag_dynforward(_tmp962,
sizeof(char),_get_zero_arr_size(_tmp962,29));}),_tag_dynforward(_tmp961,sizeof(
void*),0));});}else{if(_tmp959->tl != 0)({void*_tmp963[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp964="multiple designators are not supported";_tag_dynforward(
_tmp964,sizeof(char),_get_zero_arr_size(_tmp964,39));}),_tag_dynforward(_tmp963,
sizeof(void*),0));});else{void*_tmp965=(void*)_tmp959->hd;struct _dynforward_ptr*
_tmp966;_LL5CF: if(*((int*)_tmp965)!= 0)goto _LL5D1;_LL5D0:({void*_tmp967[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp968="array designator used in argument to struct";
_tag_dynforward(_tmp968,sizeof(char),_get_zero_arr_size(_tmp968,44));}),
_tag_dynforward(_tmp967,sizeof(void*),0));});goto _LL5CE;_LL5D1: if(*((int*)
_tmp965)!= 1)goto _LL5CE;_tmp966=((struct Cyc_Absyn_FieldName_struct*)_tmp965)->f1;
_LL5D2: {struct Cyc_List_List*_tmp969=fields;for(0;_tmp969 != 0;_tmp969=_tmp969->tl){
if(Cyc_strptrcmp(_tmp966,((*((struct _tuple18*)_tmp969->hd)).f1)->name)== 0){if((*((
struct _tuple18*)_tmp969->hd)).f2)({struct Cyc_String_pa_struct _tmp96C;_tmp96C.tag=
0;_tmp96C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp966);{void*
_tmp96A[1]={& _tmp96C};Cyc_Tcutil_terr(loc,({const char*_tmp96B="field %s has already been used as an argument";
_tag_dynforward(_tmp96B,sizeof(char),_get_zero_arr_size(_tmp96B,46));}),
_tag_dynforward(_tmp96A,sizeof(void*),1));}});(*((struct _tuple18*)_tmp969->hd)).f2=
1;ans=({struct Cyc_List_List*_tmp96D=_region_malloc(rgn,sizeof(*_tmp96D));_tmp96D->hd=({
struct _tuple20*_tmp96E=_region_malloc(rgn,sizeof(*_tmp96E));_tmp96E->f1=(*((
struct _tuple18*)_tmp969->hd)).f1;_tmp96E->f2=_tmp95A;_tmp96E;});_tmp96D->tl=ans;
_tmp96D;});break;}}if(_tmp969 == 0)({struct Cyc_String_pa_struct _tmp971;_tmp971.tag=
0;_tmp971.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp966);{void*
_tmp96F[1]={& _tmp971};Cyc_Tcutil_terr(loc,({const char*_tmp970="bad field designator %s";
_tag_dynforward(_tmp970,sizeof(char),_get_zero_arr_size(_tmp970,24));}),
_tag_dynforward(_tmp96F,sizeof(void*),1));}});goto _LL5CE;}_LL5CE:;}}}for(0;
fields != 0;fields=fields->tl){if(!(*((struct _tuple18*)fields->hd)).f2){({void*
_tmp972[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp973="too few arguments to struct";
_tag_dynforward(_tmp973,sizeof(char),_get_zero_arr_size(_tmp973,28));}),
_tag_dynforward(_tmp972,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest,int*forward_only){*forward_only=0;{void*_tmp974=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp975;void*_tmp976;struct Cyc_Absyn_PtrAtts _tmp977;
struct Cyc_Absyn_Conref*_tmp978;_LL5D4: if(_tmp974 <= (void*)4)goto _LL5D6;if(*((int*)
_tmp974)!= 4)goto _LL5D6;_tmp975=((struct Cyc_Absyn_PointerType_struct*)_tmp974)->f1;
_tmp976=(void*)_tmp975.elt_typ;_tmp977=_tmp975.ptr_atts;_tmp978=_tmp977.bounds;
_LL5D5: {struct Cyc_Absyn_Conref*_tmp979=Cyc_Absyn_compress_conref(_tmp978);void*
_tmp97A=(void*)(Cyc_Absyn_compress_conref(_tmp979))->v;void*_tmp97B;void*_tmp97C;
_LL5D9: if(_tmp97A <= (void*)1)goto _LL5DD;if(*((int*)_tmp97A)!= 0)goto _LL5DB;
_tmp97B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp97A)->f1;if((int)_tmp97B
!= 0)goto _LL5DB;_LL5DA:*forward_only=1;*elt_typ_dest=_tmp976;return 1;_LL5DB: if(*((
int*)_tmp97A)!= 0)goto _LL5DD;_tmp97C=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp97A)->f1;if((int)_tmp97C != 1)goto _LL5DD;_LL5DC:*elt_typ_dest=_tmp976;return 1;
_LL5DD: if((int)_tmp97A != 0)goto _LL5DF;_LL5DE:(void*)(_tmp979->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp97D=_cycalloc(sizeof(*_tmp97D));_tmp97D[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp97E;_tmp97E.tag=0;_tmp97E.f1=(void*)((void*)
0);_tmp97E;});_tmp97D;})));*forward_only=1;*elt_typ_dest=_tmp976;return 1;_LL5DF:;
_LL5E0: return 0;_LL5D8:;}_LL5D6:;_LL5D7: return 0;_LL5D3:;}}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmp97F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp980;void*_tmp981;struct Cyc_Absyn_PtrAtts _tmp982;struct Cyc_Absyn_Conref*
_tmp983;_LL5E2: if(_tmp97F <= (void*)4)goto _LL5E4;if(*((int*)_tmp97F)!= 4)goto
_LL5E4;_tmp980=((struct Cyc_Absyn_PointerType_struct*)_tmp97F)->f1;_tmp981=(void*)
_tmp980.elt_typ;_tmp982=_tmp980.ptr_atts;_tmp983=_tmp982.zero_term;_LL5E3:*
elt_typ_dest=_tmp981;return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp983);_LL5E4:;_LL5E5: return 0;_LL5E1:;}int Cyc_Tcutil_is_tagged_pointer_typ(
void*t,int*is_forward_only){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& ignore,is_forward_only);}struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){struct _tuple10 bogus_ans=({struct _tuple10 _tmp9C7;
_tmp9C7.f1=0;_tmp9C7.f2=(void*)2;_tmp9C7;});void*_tmp984=(void*)e->r;struct
_tuple1*_tmp985;void*_tmp986;struct Cyc_Absyn_Exp*_tmp987;struct _dynforward_ptr*
_tmp988;struct Cyc_Absyn_Exp*_tmp989;struct _dynforward_ptr*_tmp98A;struct Cyc_Absyn_Exp*
_tmp98B;struct Cyc_Absyn_Exp*_tmp98C;struct Cyc_Absyn_Exp*_tmp98D;_LL5E7: if(*((int*)
_tmp984)!= 1)goto _LL5E9;_tmp985=((struct Cyc_Absyn_Var_e_struct*)_tmp984)->f1;
_tmp986=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp984)->f2;_LL5E8: {void*
_tmp98E=_tmp986;struct Cyc_Absyn_Vardecl*_tmp98F;struct Cyc_Absyn_Vardecl*_tmp990;
struct Cyc_Absyn_Vardecl*_tmp991;struct Cyc_Absyn_Vardecl*_tmp992;_LL5F4: if((int)
_tmp98E != 0)goto _LL5F6;_LL5F5: goto _LL5F7;_LL5F6: if(_tmp98E <= (void*)1)goto _LL5F8;
if(*((int*)_tmp98E)!= 1)goto _LL5F8;_LL5F7: return bogus_ans;_LL5F8: if(_tmp98E <= (
void*)1)goto _LL5FA;if(*((int*)_tmp98E)!= 0)goto _LL5FA;_tmp98F=((struct Cyc_Absyn_Global_b_struct*)
_tmp98E)->f1;_LL5F9: {void*_tmp993=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL601: if(_tmp993 <= (void*)4)goto _LL603;if(*((int*)
_tmp993)!= 7)goto _LL603;_LL602: return({struct _tuple10 _tmp994;_tmp994.f1=1;
_tmp994.f2=(void*)2;_tmp994;});_LL603:;_LL604: return({struct _tuple10 _tmp995;
_tmp995.f1=(_tmp98F->tq).real_const;_tmp995.f2=(void*)2;_tmp995;});_LL600:;}
_LL5FA: if(_tmp98E <= (void*)1)goto _LL5FC;if(*((int*)_tmp98E)!= 3)goto _LL5FC;
_tmp990=((struct Cyc_Absyn_Local_b_struct*)_tmp98E)->f1;_LL5FB: {void*_tmp996=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL606: if(_tmp996 <= (void*)
4)goto _LL608;if(*((int*)_tmp996)!= 7)goto _LL608;_LL607: return({struct _tuple10
_tmp997;_tmp997.f1=1;_tmp997.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp990->rgn))->v;
_tmp997;});_LL608:;_LL609: _tmp990->escapes=1;return({struct _tuple10 _tmp998;
_tmp998.f1=(_tmp990->tq).real_const;_tmp998.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp990->rgn))->v;_tmp998;});_LL605:;}_LL5FC: if(_tmp98E <= (void*)1)
goto _LL5FE;if(*((int*)_tmp98E)!= 4)goto _LL5FE;_tmp991=((struct Cyc_Absyn_Pat_b_struct*)
_tmp98E)->f1;_LL5FD: _tmp992=_tmp991;goto _LL5FF;_LL5FE: if(_tmp98E <= (void*)1)goto
_LL5F3;if(*((int*)_tmp98E)!= 2)goto _LL5F3;_tmp992=((struct Cyc_Absyn_Param_b_struct*)
_tmp98E)->f1;_LL5FF: _tmp992->escapes=1;return({struct _tuple10 _tmp999;_tmp999.f1=(
_tmp992->tq).real_const;_tmp999.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp992->rgn))->v;_tmp999;});_LL5F3:;}_LL5E9: if(*((int*)_tmp984)!= 23)goto _LL5EB;
_tmp987=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp984)->f1;_tmp988=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp984)->f2;_LL5EA: {void*_tmp99A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp987->topt))->v);struct Cyc_List_List*_tmp99B;struct Cyc_Absyn_AggrInfo
_tmp99C;void*_tmp99D;struct Cyc_Absyn_Aggrdecl**_tmp99E;struct Cyc_Absyn_Aggrdecl*
_tmp99F;_LL60B: if(_tmp99A <= (void*)4)goto _LL60F;if(*((int*)_tmp99A)!= 11)goto
_LL60D;_tmp99B=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp99A)->f2;_LL60C: {
struct Cyc_Absyn_Aggrfield*_tmp9A0=Cyc_Absyn_lookup_field(_tmp99B,_tmp988);if(
_tmp9A0 != 0  && _tmp9A0->width != 0)return({struct _tuple10 _tmp9A1;_tmp9A1.f1=(
_tmp9A0->tq).real_const;_tmp9A1.f2=(Cyc_Tcutil_addressof_props(te,_tmp987)).f2;
_tmp9A1;});return bogus_ans;}_LL60D: if(*((int*)_tmp99A)!= 10)goto _LL60F;_tmp99C=((
struct Cyc_Absyn_AggrType_struct*)_tmp99A)->f1;_tmp99D=(void*)_tmp99C.aggr_info;
if(*((int*)_tmp99D)!= 1)goto _LL60F;_tmp99E=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp99D)->f1;_tmp99F=*_tmp99E;_LL60E: {struct Cyc_Absyn_Aggrfield*_tmp9A2=Cyc_Absyn_lookup_decl_field(
_tmp99F,_tmp988);if(_tmp9A2 != 0  && _tmp9A2->width != 0)return({struct _tuple10
_tmp9A3;_tmp9A3.f1=(_tmp9A2->tq).real_const;_tmp9A3.f2=(Cyc_Tcutil_addressof_props(
te,_tmp987)).f2;_tmp9A3;});return bogus_ans;}_LL60F:;_LL610: return bogus_ans;
_LL60A:;}_LL5EB: if(*((int*)_tmp984)!= 24)goto _LL5ED;_tmp989=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp984)->f1;_tmp98A=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp984)->f2;_LL5EC: {
void*_tmp9A4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp989->topt))->v);struct Cyc_Absyn_PtrInfo _tmp9A5;void*_tmp9A6;struct Cyc_Absyn_PtrAtts
_tmp9A7;void*_tmp9A8;_LL612: if(_tmp9A4 <= (void*)4)goto _LL614;if(*((int*)_tmp9A4)
!= 4)goto _LL614;_tmp9A5=((struct Cyc_Absyn_PointerType_struct*)_tmp9A4)->f1;
_tmp9A6=(void*)_tmp9A5.elt_typ;_tmp9A7=_tmp9A5.ptr_atts;_tmp9A8=(void*)_tmp9A7.rgn;
_LL613: {struct Cyc_Absyn_Aggrfield*finfo;{void*_tmp9A9=Cyc_Tcutil_compress(
_tmp9A6);struct Cyc_List_List*_tmp9AA;struct Cyc_Absyn_AggrInfo _tmp9AB;void*
_tmp9AC;struct Cyc_Absyn_Aggrdecl**_tmp9AD;struct Cyc_Absyn_Aggrdecl*_tmp9AE;
_LL617: if(_tmp9A9 <= (void*)4)goto _LL61B;if(*((int*)_tmp9A9)!= 11)goto _LL619;
_tmp9AA=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp9A9)->f2;_LL618: finfo=Cyc_Absyn_lookup_field(
_tmp9AA,_tmp98A);goto _LL616;_LL619: if(*((int*)_tmp9A9)!= 10)goto _LL61B;_tmp9AB=((
struct Cyc_Absyn_AggrType_struct*)_tmp9A9)->f1;_tmp9AC=(void*)_tmp9AB.aggr_info;
if(*((int*)_tmp9AC)!= 1)goto _LL61B;_tmp9AD=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp9AC)->f1;_tmp9AE=*_tmp9AD;_LL61A: finfo=Cyc_Absyn_lookup_decl_field(_tmp9AE,
_tmp98A);goto _LL616;_LL61B:;_LL61C: return bogus_ans;_LL616:;}if(finfo != 0  && 
finfo->width != 0)return({struct _tuple10 _tmp9AF;_tmp9AF.f1=(finfo->tq).real_const;
_tmp9AF.f2=_tmp9A8;_tmp9AF;});return bogus_ans;}_LL614:;_LL615: return bogus_ans;
_LL611:;}_LL5ED: if(*((int*)_tmp984)!= 22)goto _LL5EF;_tmp98B=((struct Cyc_Absyn_Deref_e_struct*)
_tmp984)->f1;_LL5EE: {void*_tmp9B0=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp98B->topt))->v);struct Cyc_Absyn_PtrInfo _tmp9B1;struct Cyc_Absyn_Tqual
_tmp9B2;struct Cyc_Absyn_PtrAtts _tmp9B3;void*_tmp9B4;_LL61E: if(_tmp9B0 <= (void*)4)
goto _LL620;if(*((int*)_tmp9B0)!= 4)goto _LL620;_tmp9B1=((struct Cyc_Absyn_PointerType_struct*)
_tmp9B0)->f1;_tmp9B2=_tmp9B1.elt_tq;_tmp9B3=_tmp9B1.ptr_atts;_tmp9B4=(void*)
_tmp9B3.rgn;_LL61F: return({struct _tuple10 _tmp9B5;_tmp9B5.f1=_tmp9B2.real_const;
_tmp9B5.f2=_tmp9B4;_tmp9B5;});_LL620:;_LL621: return bogus_ans;_LL61D:;}_LL5EF: if(*((
int*)_tmp984)!= 25)goto _LL5F1;_tmp98C=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp984)->f1;_tmp98D=((struct Cyc_Absyn_Subscript_e_struct*)_tmp984)->f2;_LL5F0: {
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp98C->topt))->v);
void*_tmp9B6=t;struct Cyc_List_List*_tmp9B7;struct Cyc_Absyn_PtrInfo _tmp9B8;struct
Cyc_Absyn_Tqual _tmp9B9;struct Cyc_Absyn_PtrAtts _tmp9BA;void*_tmp9BB;struct Cyc_Absyn_ArrayInfo
_tmp9BC;struct Cyc_Absyn_Tqual _tmp9BD;_LL623: if(_tmp9B6 <= (void*)4)goto _LL629;if(*((
int*)_tmp9B6)!= 9)goto _LL625;_tmp9B7=((struct Cyc_Absyn_TupleType_struct*)_tmp9B6)->f1;
_LL624: {unsigned int _tmp9BF;int _tmp9C0;struct _tuple7 _tmp9BE=Cyc_Evexp_eval_const_uint_exp(
_tmp98D);_tmp9BF=_tmp9BE.f1;_tmp9C0=_tmp9BE.f2;if(!_tmp9C0)return bogus_ans;{
struct _tuple4*_tmp9C1=Cyc_Absyn_lookup_tuple_field(_tmp9B7,(int)_tmp9BF);if(
_tmp9C1 != 0)return({struct _tuple10 _tmp9C2;_tmp9C2.f1=((*_tmp9C1).f1).real_const;
_tmp9C2.f2=(Cyc_Tcutil_addressof_props(te,_tmp98C)).f2;_tmp9C2;});return
bogus_ans;}}_LL625: if(*((int*)_tmp9B6)!= 4)goto _LL627;_tmp9B8=((struct Cyc_Absyn_PointerType_struct*)
_tmp9B6)->f1;_tmp9B9=_tmp9B8.elt_tq;_tmp9BA=_tmp9B8.ptr_atts;_tmp9BB=(void*)
_tmp9BA.rgn;_LL626: return({struct _tuple10 _tmp9C3;_tmp9C3.f1=_tmp9B9.real_const;
_tmp9C3.f2=_tmp9BB;_tmp9C3;});_LL627: if(*((int*)_tmp9B6)!= 7)goto _LL629;_tmp9BC=((
struct Cyc_Absyn_ArrayType_struct*)_tmp9B6)->f1;_tmp9BD=_tmp9BC.tq;_LL628: return({
struct _tuple10 _tmp9C4;_tmp9C4.f1=_tmp9BD.real_const;_tmp9C4.f2=(Cyc_Tcutil_addressof_props(
te,_tmp98C)).f2;_tmp9C4;});_LL629:;_LL62A: return bogus_ans;_LL622:;}_LL5F1:;
_LL5F2:({void*_tmp9C5[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp9C6="unary & applied to non-lvalue";
_tag_dynforward(_tmp9C6,sizeof(char),_get_zero_arr_size(_tmp9C6,30));}),
_tag_dynforward(_tmp9C5,sizeof(void*),0));});return bogus_ans;_LL5E6:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmp9C8=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmp9C9;void*_tmp9CA;struct Cyc_Absyn_Tqual
_tmp9CB;struct Cyc_Absyn_Conref*_tmp9CC;_LL62C: if(_tmp9C8 <= (void*)4)goto _LL62E;
if(*((int*)_tmp9C8)!= 7)goto _LL62E;_tmp9C9=((struct Cyc_Absyn_ArrayType_struct*)
_tmp9C8)->f1;_tmp9CA=(void*)_tmp9C9.elt_type;_tmp9CB=_tmp9C9.tq;_tmp9CC=_tmp9C9.zero_term;
_LL62D: {void*_tmp9CE;struct _tuple10 _tmp9CD=Cyc_Tcutil_addressof_props(te,e);
_tmp9CE=_tmp9CD.f2;return Cyc_Absyn_atb_typ(_tmp9CA,_tmp9CE,_tmp9CB,(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp9CF=_cycalloc(sizeof(*_tmp9CF));_tmp9CF[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp9D0;_tmp9D0.tag=0;_tmp9D0.f1=e;_tmp9D0;});
_tmp9CF;}),_tmp9CC);}_LL62E:;_LL62F: return e_typ;_LL62B:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(
b);{void*_tmp9D1=(void*)b->v;void*_tmp9D2;void*_tmp9D3;void*_tmp9D4;struct Cyc_Absyn_Exp*
_tmp9D5;void*_tmp9D6;_LL631: if(_tmp9D1 <= (void*)1)goto _LL639;if(*((int*)_tmp9D1)
!= 0)goto _LL633;_tmp9D2=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9D1)->f1;
if((int)_tmp9D2 != 1)goto _LL633;_LL632: return;_LL633: if(*((int*)_tmp9D1)!= 0)goto
_LL635;_tmp9D3=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9D1)->f1;if((int)
_tmp9D3 != 0)goto _LL635;_LL634: if(i < 0)({void*_tmp9D7[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp9D8="dereference is out of bounds";_tag_dynforward(_tmp9D8,sizeof(
char),_get_zero_arr_size(_tmp9D8,29));}),_tag_dynforward(_tmp9D7,sizeof(void*),0));});
return;_LL635: if(*((int*)_tmp9D1)!= 0)goto _LL637;_tmp9D4=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp9D1)->f1;if(_tmp9D4 <= (void*)2)goto _LL637;if(*((int*)_tmp9D4)!= 0)goto _LL637;
_tmp9D5=((struct Cyc_Absyn_Upper_b_struct*)_tmp9D4)->f1;_LL636: {unsigned int
_tmp9DA;int _tmp9DB;struct _tuple7 _tmp9D9=Cyc_Evexp_eval_const_uint_exp(_tmp9D5);
_tmp9DA=_tmp9D9.f1;_tmp9DB=_tmp9D9.f2;if(_tmp9DB  && _tmp9DA <= i)({struct Cyc_Int_pa_struct
_tmp9DF;_tmp9DF.tag=1;_tmp9DF.f1=(unsigned long)((int)i);{struct Cyc_Int_pa_struct
_tmp9DE;_tmp9DE.tag=1;_tmp9DE.f1=(unsigned long)((int)_tmp9DA);{void*_tmp9DC[2]={&
_tmp9DE,& _tmp9DF};Cyc_Tcutil_terr(loc,({const char*_tmp9DD="dereference is out of bounds: %d <= %d";
_tag_dynforward(_tmp9DD,sizeof(char),_get_zero_arr_size(_tmp9DD,39));}),
_tag_dynforward(_tmp9DC,sizeof(void*),2));}}});return;}_LL637: if(*((int*)_tmp9D1)
!= 0)goto _LL639;_tmp9D6=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9D1)->f1;
if(_tmp9D6 <= (void*)2)goto _LL639;if(*((int*)_tmp9D6)!= 1)goto _LL639;_LL638:
return;_LL639: if((int)_tmp9D1 != 0)goto _LL63B;_LL63A:(void*)(b->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp9E0=_cycalloc(sizeof(*_tmp9E0));_tmp9E0[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp9E1;_tmp9E1.tag=0;_tmp9E1.f1=(void*)((void*)((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp9E2=_cycalloc(sizeof(*_tmp9E2));
_tmp9E2[0]=({struct Cyc_Absyn_Upper_b_struct _tmp9E3;_tmp9E3.tag=0;_tmp9E3.f1=Cyc_Absyn_uint_exp(
i + 1,0);_tmp9E3;});_tmp9E2;})));_tmp9E1;});_tmp9E0;})));return;_LL63B:;_LL63C:({
void*_tmp9E4[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp9E5="check_bound -- bad compressed conref";
_tag_dynforward(_tmp9E5,sizeof(char),_get_zero_arr_size(_tmp9E5,37));}),
_tag_dynforward(_tmp9E4,sizeof(void*),0));});_LL630:;}}void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(
loc,0,b);}int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b){void*_tmp9E6=(
void*)(Cyc_Absyn_compress_conref(b))->v;void*_tmp9E7;struct Cyc_Absyn_Exp*_tmp9E8;
_LL63E: if(_tmp9E6 <= (void*)1)goto _LL640;if(*((int*)_tmp9E6)!= 0)goto _LL640;
_tmp9E7=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9E6)->f1;if(_tmp9E7 <= (
void*)2)goto _LL640;if(*((int*)_tmp9E7)!= 0)goto _LL640;_tmp9E8=((struct Cyc_Absyn_Upper_b_struct*)
_tmp9E7)->f1;_LL63F: {unsigned int _tmp9EA;int _tmp9EB;struct _tuple7 _tmp9E9=Cyc_Evexp_eval_const_uint_exp(
_tmp9E8);_tmp9EA=_tmp9E9.f1;_tmp9EB=_tmp9E9.f2;return _tmp9EB  && _tmp9EA == 1;}
_LL640:;_LL641: return 0;_LL63D:;}int Cyc_Tcutil_bits_only(void*t){void*_tmp9EC=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmp9ED;void*_tmp9EE;struct Cyc_Absyn_Conref*_tmp9EF;
struct Cyc_List_List*_tmp9F0;struct Cyc_Absyn_AggrInfo _tmp9F1;void*_tmp9F2;struct
Cyc_Absyn_AggrInfo _tmp9F3;void*_tmp9F4;struct Cyc_Absyn_Aggrdecl**_tmp9F5;struct
Cyc_Absyn_Aggrdecl*_tmp9F6;struct Cyc_List_List*_tmp9F7;struct Cyc_List_List*
_tmp9F8;_LL643: if((int)_tmp9EC != 0)goto _LL645;_LL644: goto _LL646;_LL645: if(
_tmp9EC <= (void*)4)goto _LL647;if(*((int*)_tmp9EC)!= 5)goto _LL647;_LL646: goto
_LL648;_LL647: if((int)_tmp9EC != 1)goto _LL649;_LL648: goto _LL64A;_LL649: if(_tmp9EC
<= (void*)4)goto _LL659;if(*((int*)_tmp9EC)!= 6)goto _LL64B;_LL64A: return 1;_LL64B:
if(*((int*)_tmp9EC)!= 12)goto _LL64D;_LL64C: goto _LL64E;_LL64D: if(*((int*)_tmp9EC)
!= 13)goto _LL64F;_LL64E: return 0;_LL64F: if(*((int*)_tmp9EC)!= 7)goto _LL651;
_tmp9ED=((struct Cyc_Absyn_ArrayType_struct*)_tmp9EC)->f1;_tmp9EE=(void*)_tmp9ED.elt_type;
_tmp9EF=_tmp9ED.zero_term;_LL650: return !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp9EF) && Cyc_Tcutil_bits_only(_tmp9EE);_LL651: if(*((
int*)_tmp9EC)!= 9)goto _LL653;_tmp9F0=((struct Cyc_Absyn_TupleType_struct*)_tmp9EC)->f1;
_LL652: for(0;_tmp9F0 != 0;_tmp9F0=_tmp9F0->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple4*)_tmp9F0->hd)).f2))return 0;}return 1;_LL653: if(*((int*)_tmp9EC)!= 
10)goto _LL655;_tmp9F1=((struct Cyc_Absyn_AggrType_struct*)_tmp9EC)->f1;_tmp9F2=(
void*)_tmp9F1.aggr_info;if(*((int*)_tmp9F2)!= 0)goto _LL655;_LL654: return 0;_LL655:
if(*((int*)_tmp9EC)!= 10)goto _LL657;_tmp9F3=((struct Cyc_Absyn_AggrType_struct*)
_tmp9EC)->f1;_tmp9F4=(void*)_tmp9F3.aggr_info;if(*((int*)_tmp9F4)!= 1)goto _LL657;
_tmp9F5=((struct Cyc_Absyn_KnownAggr_struct*)_tmp9F4)->f1;_tmp9F6=*_tmp9F5;
_tmp9F7=_tmp9F3.targs;_LL656: if(_tmp9F6->impl == 0)return 0;{struct _RegionHandle
_tmp9F9=_new_region("rgn");struct _RegionHandle*rgn=& _tmp9F9;_push_region(rgn);{
struct Cyc_List_List*_tmp9FA=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
rgn,rgn,_tmp9F6->tvs,_tmp9F7);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp9F6->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(
Cyc_Tcutil_rsubstitute(rgn,_tmp9FA,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp9FB=0;_npop_handler(0);return _tmp9FB;}}}{int _tmp9FC=1;_npop_handler(0);
return _tmp9FC;}};_pop_region(rgn);}_LL657: if(*((int*)_tmp9EC)!= 11)goto _LL659;
_tmp9F8=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp9EC)->f2;_LL658: for(0;_tmp9F8
!= 0;_tmp9F8=_tmp9F8->tl){if(!Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp9F8->hd)->type))return 0;}return 1;_LL659:;_LL65A: return 0;_LL642:;}struct
_tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmp9FD=(void*)e->r;
struct _tuple1*_tmp9FE;void*_tmp9FF;struct Cyc_Absyn_Exp*_tmpA00;struct Cyc_Absyn_Exp*
_tmpA01;struct Cyc_Absyn_Exp*_tmpA02;struct Cyc_Absyn_Exp*_tmpA03;struct Cyc_Absyn_Exp*
_tmpA04;struct Cyc_Absyn_Exp*_tmpA05;struct Cyc_Absyn_Exp*_tmpA06;void*_tmpA07;
struct Cyc_Absyn_Exp*_tmpA08;void*_tmpA09;void*_tmpA0A;struct Cyc_Absyn_Exp*
_tmpA0B;struct Cyc_Absyn_Exp*_tmpA0C;struct Cyc_Absyn_Exp*_tmpA0D;struct Cyc_Absyn_Exp*
_tmpA0E;struct Cyc_List_List*_tmpA0F;struct Cyc_List_List*_tmpA10;struct Cyc_List_List*
_tmpA11;void*_tmpA12;struct Cyc_List_List*_tmpA13;struct Cyc_List_List*_tmpA14;
struct Cyc_List_List*_tmpA15;_LL65C: if(*((int*)_tmp9FD)!= 0)goto _LL65E;_LL65D:
goto _LL65F;_LL65E: if(*((int*)_tmp9FD)!= 18)goto _LL660;_LL65F: goto _LL661;_LL660:
if(*((int*)_tmp9FD)!= 19)goto _LL662;_LL661: goto _LL663;_LL662: if(*((int*)_tmp9FD)
!= 20)goto _LL664;_LL663: goto _LL665;_LL664: if(*((int*)_tmp9FD)!= 21)goto _LL666;
_LL665: goto _LL667;_LL666: if(*((int*)_tmp9FD)!= 33)goto _LL668;_LL667: goto _LL669;
_LL668: if(*((int*)_tmp9FD)!= 34)goto _LL66A;_LL669: return 1;_LL66A: if(*((int*)
_tmp9FD)!= 1)goto _LL66C;_tmp9FE=((struct Cyc_Absyn_Var_e_struct*)_tmp9FD)->f1;
_tmp9FF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9FD)->f2;_LL66B: {void*
_tmpA16=_tmp9FF;struct Cyc_Absyn_Vardecl*_tmpA17;_LL68B: if(_tmpA16 <= (void*)1)
goto _LL68F;if(*((int*)_tmpA16)!= 1)goto _LL68D;_LL68C: return 1;_LL68D: if(*((int*)
_tmpA16)!= 0)goto _LL68F;_tmpA17=((struct Cyc_Absyn_Global_b_struct*)_tmpA16)->f1;
_LL68E: {void*_tmpA18=Cyc_Tcutil_compress((void*)_tmpA17->type);_LL694: if(
_tmpA18 <= (void*)4)goto _LL698;if(*((int*)_tmpA18)!= 7)goto _LL696;_LL695: goto
_LL697;_LL696: if(*((int*)_tmpA18)!= 8)goto _LL698;_LL697: return 1;_LL698:;_LL699:
return var_okay;_LL693:;}_LL68F: if((int)_tmpA16 != 0)goto _LL691;_LL690: return 0;
_LL691:;_LL692: return var_okay;_LL68A:;}_LL66C: if(*((int*)_tmp9FD)!= 6)goto _LL66E;
_tmpA00=((struct Cyc_Absyn_Conditional_e_struct*)_tmp9FD)->f1;_tmpA01=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp9FD)->f2;_tmpA02=((struct Cyc_Absyn_Conditional_e_struct*)_tmp9FD)->f3;_LL66D:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpA00) && Cyc_Tcutil_cnst_exp(te,0,_tmpA01))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpA02);_LL66E: if(*((int*)_tmp9FD)!= 9)goto _LL670;
_tmpA03=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp9FD)->f1;_tmpA04=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp9FD)->f2;_LL66F: return Cyc_Tcutil_cnst_exp(te,0,_tmpA03) && Cyc_Tcutil_cnst_exp(
te,0,_tmpA04);_LL670: if(*((int*)_tmp9FD)!= 13)goto _LL672;_tmpA05=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp9FD)->f1;_LL671: _tmpA06=_tmpA05;goto _LL673;_LL672: if(*((int*)_tmp9FD)!= 14)
goto _LL674;_tmpA06=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp9FD)->f1;_LL673:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpA06);_LL674: if(*((int*)_tmp9FD)!= 15)
goto _LL676;_tmpA07=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp9FD)->f1;_tmpA08=((
struct Cyc_Absyn_Cast_e_struct*)_tmp9FD)->f2;_tmpA09=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp9FD)->f4;if((int)_tmpA09 != 1)goto _LL676;_LL675: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpA08);_LL676: if(*((int*)_tmp9FD)!= 15)goto _LL678;_tmpA0A=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp9FD)->f1;_tmpA0B=((struct Cyc_Absyn_Cast_e_struct*)
_tmp9FD)->f2;_LL677: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpA0B);_LL678: if(*((
int*)_tmp9FD)!= 16)goto _LL67A;_tmpA0C=((struct Cyc_Absyn_Address_e_struct*)
_tmp9FD)->f1;_LL679: return Cyc_Tcutil_cnst_exp(te,1,_tmpA0C);_LL67A: if(*((int*)
_tmp9FD)!= 29)goto _LL67C;_tmpA0D=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp9FD)->f2;_tmpA0E=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp9FD)->f3;
_LL67B: return Cyc_Tcutil_cnst_exp(te,0,_tmpA0D) && Cyc_Tcutil_cnst_exp(te,0,
_tmpA0E);_LL67C: if(*((int*)_tmp9FD)!= 28)goto _LL67E;_tmpA0F=((struct Cyc_Absyn_Array_e_struct*)
_tmp9FD)->f1;_LL67D: _tmpA10=_tmpA0F;goto _LL67F;_LL67E: if(*((int*)_tmp9FD)!= 31)
goto _LL680;_tmpA10=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp9FD)->f2;_LL67F:
_tmpA11=_tmpA10;goto _LL681;_LL680: if(*((int*)_tmp9FD)!= 30)goto _LL682;_tmpA11=((
struct Cyc_Absyn_Struct_e_struct*)_tmp9FD)->f3;_LL681: for(0;_tmpA11 != 0;_tmpA11=
_tmpA11->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple21*)_tmpA11->hd)).f2))
return 0;}return 1;_LL682: if(*((int*)_tmp9FD)!= 3)goto _LL684;_tmpA12=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp9FD)->f1;_tmpA13=((struct Cyc_Absyn_Primop_e_struct*)
_tmp9FD)->f2;_LL683: _tmpA14=_tmpA13;goto _LL685;_LL684: if(*((int*)_tmp9FD)!= 26)
goto _LL686;_tmpA14=((struct Cyc_Absyn_Tuple_e_struct*)_tmp9FD)->f1;_LL685: _tmpA15=
_tmpA14;goto _LL687;_LL686: if(*((int*)_tmp9FD)!= 32)goto _LL688;_tmpA15=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp9FD)->f1;_LL687: for(0;_tmpA15 != 0;_tmpA15=_tmpA15->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmpA15->hd))return 0;}return 1;
_LL688:;_LL689: return 0;_LL65B:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmpA19=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpA1A;
struct Cyc_Absyn_PtrAtts _tmpA1B;struct Cyc_Absyn_Conref*_tmpA1C;struct Cyc_Absyn_Conref*
_tmpA1D;struct Cyc_Absyn_ArrayInfo _tmpA1E;void*_tmpA1F;struct Cyc_List_List*
_tmpA20;struct Cyc_Absyn_AggrInfo _tmpA21;void*_tmpA22;struct Cyc_List_List*_tmpA23;
struct Cyc_List_List*_tmpA24;_LL69B: if((int)_tmpA19 != 0)goto _LL69D;_LL69C: goto
_LL69E;_LL69D: if(_tmpA19 <= (void*)4)goto _LL69F;if(*((int*)_tmpA19)!= 5)goto
_LL69F;_LL69E: goto _LL6A0;_LL69F: if((int)_tmpA19 != 1)goto _LL6A1;_LL6A0: goto _LL6A2;
_LL6A1: if(_tmpA19 <= (void*)4)goto _LL6B1;if(*((int*)_tmpA19)!= 6)goto _LL6A3;
_LL6A2: return 1;_LL6A3: if(*((int*)_tmpA19)!= 4)goto _LL6A5;_tmpA1A=((struct Cyc_Absyn_PointerType_struct*)
_tmpA19)->f1;_tmpA1B=_tmpA1A.ptr_atts;_tmpA1C=_tmpA1B.nullable;_tmpA1D=_tmpA1B.bounds;
_LL6A4: {void*_tmpA25=Cyc_Absyn_conref_def((void*)((void*)0),_tmpA1D);_LL6B4: if((
int)_tmpA25 != 0)goto _LL6B6;_LL6B5: return 1;_LL6B6: if((int)_tmpA25 != 1)goto _LL6B8;
_LL6B7: return 1;_LL6B8:;_LL6B9: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmpA1C);_LL6B3:;}_LL6A5: if(*((int*)_tmpA19)!= 7)goto _LL6A7;_tmpA1E=((struct
Cyc_Absyn_ArrayType_struct*)_tmpA19)->f1;_tmpA1F=(void*)_tmpA1E.elt_type;_LL6A6:
return Cyc_Tcutil_supports_default(_tmpA1F);_LL6A7: if(*((int*)_tmpA19)!= 9)goto
_LL6A9;_tmpA20=((struct Cyc_Absyn_TupleType_struct*)_tmpA19)->f1;_LL6A8: for(0;
_tmpA20 != 0;_tmpA20=_tmpA20->tl){if(!Cyc_Tcutil_supports_default((*((struct
_tuple4*)_tmpA20->hd)).f2))return 0;}return 1;_LL6A9: if(*((int*)_tmpA19)!= 10)goto
_LL6AB;_tmpA21=((struct Cyc_Absyn_AggrType_struct*)_tmpA19)->f1;_tmpA22=(void*)
_tmpA21.aggr_info;_tmpA23=_tmpA21.targs;_LL6AA: {struct Cyc_Absyn_Aggrdecl*
_tmpA26=Cyc_Absyn_get_known_aggrdecl(_tmpA22);if(_tmpA26->impl == 0)return 0;if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA26->impl))->exist_vars != 0)return
0;return Cyc_Tcutil_fields_support_default(_tmpA26->tvs,_tmpA23,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpA26->impl))->fields);}_LL6AB: if(*((int*)_tmpA19)!= 11)goto _LL6AD;
_tmpA24=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpA19)->f2;_LL6AC: return Cyc_Tcutil_fields_support_default(
0,0,_tmpA24);_LL6AD: if(*((int*)_tmpA19)!= 13)goto _LL6AF;_LL6AE: goto _LL6B0;_LL6AF:
if(*((int*)_tmpA19)!= 12)goto _LL6B1;_LL6B0: return 1;_LL6B1:;_LL6B2: return 0;_LL69A:;}
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*
ts,struct Cyc_List_List*fs){{struct _RegionHandle _tmpA27=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpA27;_push_region(rgn);{struct Cyc_List_List*_tmpA28=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpA28,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpA29=0;_npop_handler(0);return _tmpA29;}}};
_pop_region(rgn);}return 1;}int Cyc_Tcutil_admits_zero(void*t){void*_tmpA2A=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpA2B;struct Cyc_Absyn_PtrAtts _tmpA2C;struct Cyc_Absyn_Conref*
_tmpA2D;struct Cyc_Absyn_Conref*_tmpA2E;_LL6BB: if(_tmpA2A <= (void*)4)goto _LL6BD;
if(*((int*)_tmpA2A)!= 5)goto _LL6BD;_LL6BC: goto _LL6BE;_LL6BD: if((int)_tmpA2A != 1)
goto _LL6BF;_LL6BE: goto _LL6C0;_LL6BF: if(_tmpA2A <= (void*)4)goto _LL6C3;if(*((int*)
_tmpA2A)!= 6)goto _LL6C1;_LL6C0: return 1;_LL6C1: if(*((int*)_tmpA2A)!= 4)goto _LL6C3;
_tmpA2B=((struct Cyc_Absyn_PointerType_struct*)_tmpA2A)->f1;_tmpA2C=_tmpA2B.ptr_atts;
_tmpA2D=_tmpA2C.nullable;_tmpA2E=_tmpA2C.bounds;_LL6C2: {void*_tmpA2F=(void*)(
Cyc_Absyn_compress_conref(_tmpA2E))->v;void*_tmpA30;void*_tmpA31;_LL6C6: if(
_tmpA2F <= (void*)1)goto _LL6CC;if(*((int*)_tmpA2F)!= 0)goto _LL6C8;_tmpA30=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpA2F)->f1;if((int)_tmpA30 != 0)goto _LL6C8;
_LL6C7: return 0;_LL6C8: if(*((int*)_tmpA2F)!= 0)goto _LL6CA;_tmpA31=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmpA2F)->f1;if((int)_tmpA31 != 1)goto _LL6CA;_LL6C9:
return 0;_LL6CA: if(*((int*)_tmpA2F)!= 0)goto _LL6CC;_LL6CB: {void*_tmpA32=(void*)(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmpA2D))->v;int _tmpA33;_LL6CF: if(_tmpA32 <= (void*)1)goto _LL6D1;if(*((int*)
_tmpA32)!= 0)goto _LL6D1;_tmpA33=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmpA32)->f1;
_LL6D0: return _tmpA33;_LL6D1:;_LL6D2: return 0;_LL6CE:;}_LL6CC:;_LL6CD: return 0;
_LL6C5:;}_LL6C3:;_LL6C4: return 0;_LL6BA:;}int Cyc_Tcutil_is_noreturn(void*t){{void*
_tmpA34=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpA35;void*_tmpA36;
struct Cyc_Absyn_FnInfo _tmpA37;struct Cyc_List_List*_tmpA38;_LL6D4: if(_tmpA34 <= (
void*)4)goto _LL6D8;if(*((int*)_tmpA34)!= 4)goto _LL6D6;_tmpA35=((struct Cyc_Absyn_PointerType_struct*)
_tmpA34)->f1;_tmpA36=(void*)_tmpA35.elt_typ;_LL6D5: return Cyc_Tcutil_is_noreturn(
_tmpA36);_LL6D6: if(*((int*)_tmpA34)!= 8)goto _LL6D8;_tmpA37=((struct Cyc_Absyn_FnType_struct*)
_tmpA34)->f1;_tmpA38=_tmpA37.attributes;_LL6D7: for(0;_tmpA38 != 0;_tmpA38=_tmpA38->tl){
void*_tmpA39=(void*)_tmpA38->hd;_LL6DB: if((int)_tmpA39 != 3)goto _LL6DD;_LL6DC:
return 1;_LL6DD:;_LL6DE: continue;_LL6DA:;}goto _LL6D3;_LL6D8:;_LL6D9: goto _LL6D3;
_LL6D3:;}return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,
struct Cyc_List_List*atts){void*_tmpA3A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmpA3B;struct Cyc_List_List*_tmpA3C;struct Cyc_List_List**_tmpA3D;_LL6E0: if(
_tmpA3A <= (void*)4)goto _LL6E2;if(*((int*)_tmpA3A)!= 8)goto _LL6E2;_tmpA3B=((
struct Cyc_Absyn_FnType_struct*)_tmpA3A)->f1;_tmpA3C=_tmpA3B.attributes;_tmpA3D=(
struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmpA3A)->f1).attributes;
_LL6E1: {struct Cyc_List_List*_tmpA3E=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((
void*)atts->hd))*_tmpA3D=({struct Cyc_List_List*_tmpA3F=_cycalloc(sizeof(*_tmpA3F));
_tmpA3F->hd=(void*)((void*)atts->hd);_tmpA3F->tl=*_tmpA3D;_tmpA3F;});else{
_tmpA3E=({struct Cyc_List_List*_tmpA40=_cycalloc(sizeof(*_tmpA40));_tmpA40->hd=(
void*)((void*)atts->hd);_tmpA40->tl=_tmpA3E;_tmpA40;});}}return _tmpA3E;}_LL6E2:;
_LL6E3:({void*_tmpA41[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA42="transfer_fn_type_atts";
_tag_dynforward(_tmpA42,sizeof(char),_get_zero_arr_size(_tmpA42,22));}),
_tag_dynforward(_tmpA41,sizeof(void*),0));});_LL6DF:;}
