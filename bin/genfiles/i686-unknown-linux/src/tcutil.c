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
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y);void Cyc_List_iter(void(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*
x,int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x);int Cyc_List_mem(int(*compare)(void*,
void*),struct Cyc_List_List*l,void*x);void*Cyc_List_assoc_cmp(int(*cmp)(void*,
void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*cmp)(void*,void*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{struct
_dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_List_List*Cyc_Position_strings_of_segments(
struct Cyc_List_List*);struct Cyc_Position_Error{struct _dynforward_ptr source;
struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};struct Cyc_Position_Error*
Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*,struct _dynforward_ptr);
extern char Cyc_Position_Nocontext[14];extern int Cyc_Position_num_errors;extern int
Cyc_Position_max_errors;void Cyc_Position_post_error(struct Cyc_Position_Error*);
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple1{union Cyc_Absyn_Nmspace_union
f1;struct _dynforward_ptr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_No_constr_struct{int tag;};
union Cyc_Absyn_Constraint_union{struct Cyc_Absyn_Eq_constr_struct Eq_constr;struct
Cyc_Absyn_Forward_constr_struct Forward_constr;struct Cyc_Absyn_No_constr_struct
No_constr;};struct Cyc_Absyn_Conref{union Cyc_Absyn_Constraint_union v;};struct Cyc_Absyn_Eq_kb_struct{
int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;
};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;};struct
Cyc_Absyn_Tvar{struct _dynforward_ptr*name;int*identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AbsUpper_b_struct{int tag;void*f1;
};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
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
tag;struct Cyc_Absyn_Tuniondecl**f1;};union Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct
UnknownTunion;struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{
union Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple1*tunion_name;struct
_tuple1*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int
tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};union Cyc_Absyn_TunionFieldInfoU_union{
struct Cyc_Absyn_UnknownTunionfield_struct UnknownTunionfield;struct Cyc_Absyn_KnownTunionfield_struct
KnownTunionfield;};struct Cyc_Absyn_TunionFieldInfo{union Cyc_Absyn_TunionFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple1*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
f1;};union Cyc_Absyn_AggrInfoU_union{struct Cyc_Absyn_UnknownAggr_struct
UnknownAggr;struct Cyc_Absyn_KnownAggr_struct KnownAggr;};struct Cyc_Absyn_AggrInfo{
union Cyc_Absyn_AggrInfoU_union aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{
void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;struct Cyc_Absyn_Conref*
zero_term;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_Evar_struct{int tag;
struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};struct
Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_TunionType_struct{
int tag;struct Cyc_Absyn_TunionInfo f1;};struct Cyc_Absyn_TunionFieldType_struct{int
tag;struct Cyc_Absyn_TunionFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{int
tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{int tag;struct
Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct
Cyc_Absyn_AnonAggrType_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{
int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;
};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_TypeInt_struct{
int tag;int f1;};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*f1;int f2;struct
Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Regparm_att_struct{
int tag;int f1;};struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Section_att_struct{
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
union Cyc_Absyn_AggrInfoU_union);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct
_dynforward_ptr Cyc_Absynpp_kind2string(void*);struct _dynforward_ptr Cyc_Absynpp_kindbound2string(
void*);struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct _tuple6{
void*f1;void*f2;};struct _tuple6*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict
d);struct _tuple6*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
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
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
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
struct Cyc_Absyn_Conref*Cyc_Tcutil_copy_conref(struct Cyc_Absyn_Conref*c){union Cyc_Absyn_Constraint_union
_tmp45=c->v;void*_tmp46;struct Cyc_Absyn_Conref*_tmp47;_LLC: if((_tmp45.No_constr).tag
!= 2)goto _LLE;_LLD: return Cyc_Absyn_empty_conref();_LLE: if((_tmp45.Eq_constr).tag
!= 0)goto _LL10;_tmp46=(_tmp45.Eq_constr).f1;_LLF: return Cyc_Absyn_new_conref(
_tmp46);_LL10: if((_tmp45.Forward_constr).tag != 1)goto _LLB;_tmp47=(_tmp45.Forward_constr).f1;
_LL11: return Cyc_Tcutil_copy_conref(_tmp47);_LLB:;}static void*Cyc_Tcutil_copy_kindbound(
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
t);struct Cyc_Absyn_Tvar*_tmp65;struct Cyc_Absyn_TunionInfo _tmp66;union Cyc_Absyn_TunionInfoU_union
_tmp67;struct Cyc_List_List*_tmp68;struct Cyc_Core_Opt*_tmp69;struct Cyc_Absyn_TunionFieldInfo
_tmp6A;union Cyc_Absyn_TunionFieldInfoU_union _tmp6B;struct Cyc_List_List*_tmp6C;
struct Cyc_Absyn_PtrInfo _tmp6D;void*_tmp6E;struct Cyc_Absyn_Tqual _tmp6F;struct Cyc_Absyn_PtrAtts
_tmp70;void*_tmp71;struct Cyc_Absyn_Conref*_tmp72;struct Cyc_Absyn_Conref*_tmp73;
struct Cyc_Absyn_Conref*_tmp74;struct Cyc_Absyn_PtrLoc*_tmp75;void*_tmp76;void*
_tmp77;int _tmp78;struct Cyc_Absyn_ArrayInfo _tmp79;void*_tmp7A;struct Cyc_Absyn_Tqual
_tmp7B;struct Cyc_Absyn_Exp*_tmp7C;struct Cyc_Absyn_Conref*_tmp7D;struct Cyc_Position_Segment*
_tmp7E;struct Cyc_Absyn_FnInfo _tmp7F;struct Cyc_List_List*_tmp80;struct Cyc_Core_Opt*
_tmp81;void*_tmp82;struct Cyc_List_List*_tmp83;int _tmp84;struct Cyc_Absyn_VarargInfo*
_tmp85;struct Cyc_List_List*_tmp86;struct Cyc_List_List*_tmp87;struct Cyc_List_List*
_tmp88;struct Cyc_Absyn_AggrInfo _tmp89;union Cyc_Absyn_AggrInfoU_union _tmp8A;void*
_tmp8B;struct _tuple1*_tmp8C;struct Cyc_List_List*_tmp8D;struct Cyc_Absyn_AggrInfo
_tmp8E;union Cyc_Absyn_AggrInfoU_union _tmp8F;struct Cyc_Absyn_Aggrdecl**_tmp90;
struct Cyc_List_List*_tmp91;void*_tmp92;struct Cyc_List_List*_tmp93;struct _tuple1*
_tmp94;struct Cyc_Absyn_Enumdecl*_tmp95;struct Cyc_List_List*_tmp96;void*_tmp97;
int _tmp98;void*_tmp99;void*_tmp9A;struct _tuple1*_tmp9B;struct Cyc_List_List*
_tmp9C;struct Cyc_Absyn_Typedefdecl*_tmp9D;void*_tmp9E;struct Cyc_List_List*_tmp9F;
void*_tmpA0;_LL1A: if((int)_tmp64 != 0)goto _LL1C;_LL1B: goto _LL1D;_LL1C: if(_tmp64 <= (
void*)4)goto _LL28;if(*((int*)_tmp64)!= 0)goto _LL1E;_LL1D: return t;_LL1E: if(*((int*)
_tmp64)!= 1)goto _LL20;_tmp65=((struct Cyc_Absyn_VarType_struct*)_tmp64)->f1;_LL1F:
return(void*)({struct Cyc_Absyn_VarType_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));
_tmpA1[0]=({struct Cyc_Absyn_VarType_struct _tmpA2;_tmpA2.tag=1;_tmpA2.f1=Cyc_Tcutil_copy_tvar(
_tmp65);_tmpA2;});_tmpA1;});_LL20: if(*((int*)_tmp64)!= 2)goto _LL22;_tmp66=((
struct Cyc_Absyn_TunionType_struct*)_tmp64)->f1;_tmp67=_tmp66.tunion_info;_tmp68=
_tmp66.targs;_tmp69=_tmp66.rgn;_LL21: {struct Cyc_Core_Opt*_tmpA3=(unsigned int)
_tmp69?({struct Cyc_Core_Opt*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7->v=(void*)
Cyc_Tcutil_copy_type((void*)_tmp69->v);_tmpA7;}): 0;return(void*)({struct Cyc_Absyn_TunionType_struct*
_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=({struct Cyc_Absyn_TunionType_struct
_tmpA5;_tmpA5.tag=2;_tmpA5.f1=({struct Cyc_Absyn_TunionInfo _tmpA6;_tmpA6.tunion_info=
_tmp67;_tmpA6.targs=Cyc_Tcutil_copy_types(_tmp68);_tmpA6.rgn=_tmpA3;_tmpA6;});
_tmpA5;});_tmpA4;});}_LL22: if(*((int*)_tmp64)!= 3)goto _LL24;_tmp6A=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp64)->f1;_tmp6B=_tmp6A.field_info;_tmp6C=_tmp6A.targs;_LL23: return(void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8[
0]=({struct Cyc_Absyn_TunionFieldType_struct _tmpA9;_tmpA9.tag=3;_tmpA9.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmpAA;_tmpAA.field_info=_tmp6B;_tmpAA.targs=Cyc_Tcutil_copy_types(
_tmp6C);_tmpAA;});_tmpA9;});_tmpA8;});_LL24: if(*((int*)_tmp64)!= 4)goto _LL26;
_tmp6D=((struct Cyc_Absyn_PointerType_struct*)_tmp64)->f1;_tmp6E=(void*)_tmp6D.elt_typ;
_tmp6F=_tmp6D.elt_tq;_tmp70=_tmp6D.ptr_atts;_tmp71=(void*)_tmp70.rgn;_tmp72=
_tmp70.nullable;_tmp73=_tmp70.bounds;_tmp74=_tmp70.zero_term;_tmp75=_tmp70.ptrloc;
_LL25: {void*_tmpAB=Cyc_Tcutil_copy_type(_tmp6E);void*_tmpAC=Cyc_Tcutil_copy_type(
_tmp71);struct Cyc_Absyn_Conref*_tmpAD=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp72);struct Cyc_Absyn_Tqual _tmpAE=_tmp6F;struct Cyc_Absyn_Conref*
_tmpAF=Cyc_Tcutil_copy_conref(_tmp73);struct Cyc_Absyn_Conref*_tmpB0=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp74);return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({
struct Cyc_Absyn_PointerType_struct _tmpB2;_tmpB2.tag=4;_tmpB2.f1=({struct Cyc_Absyn_PtrInfo
_tmpB3;_tmpB3.elt_typ=(void*)_tmpAB;_tmpB3.elt_tq=_tmpAE;_tmpB3.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmpB4;_tmpB4.rgn=(void*)_tmpAC;_tmpB4.nullable=_tmpAD;
_tmpB4.bounds=_tmpAF;_tmpB4.zero_term=_tmpB0;_tmpB4.ptrloc=_tmp75;_tmpB4;});
_tmpB3;});_tmpB2;});_tmpB1;});}_LL26: if(*((int*)_tmp64)!= 5)goto _LL28;_tmp76=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp64)->f1;_tmp77=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp64)->f2;_LL27: return(void*)({struct Cyc_Absyn_IntType_struct*_tmpB5=_cycalloc(
sizeof(*_tmpB5));_tmpB5[0]=({struct Cyc_Absyn_IntType_struct _tmpB6;_tmpB6.tag=5;
_tmpB6.f1=(void*)_tmp76;_tmpB6.f2=(void*)_tmp77;_tmpB6;});_tmpB5;});_LL28: if((
int)_tmp64 != 1)goto _LL2A;_LL29: return t;_LL2A: if(_tmp64 <= (void*)4)goto _LL46;if(*((
int*)_tmp64)!= 6)goto _LL2C;_tmp78=((struct Cyc_Absyn_DoubleType_struct*)_tmp64)->f1;
_LL2B: return(void*)({struct Cyc_Absyn_DoubleType_struct*_tmpB7=_cycalloc_atomic(
sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Absyn_DoubleType_struct _tmpB8;_tmpB8.tag=
6;_tmpB8.f1=_tmp78;_tmpB8;});_tmpB7;});_LL2C: if(*((int*)_tmp64)!= 7)goto _LL2E;
_tmp79=((struct Cyc_Absyn_ArrayType_struct*)_tmp64)->f1;_tmp7A=(void*)_tmp79.elt_type;
_tmp7B=_tmp79.tq;_tmp7C=_tmp79.num_elts;_tmp7D=_tmp79.zero_term;_tmp7E=_tmp79.zt_loc;
_LL2D: return(void*)({struct Cyc_Absyn_ArrayType_struct*_tmpB9=_cycalloc(sizeof(*
_tmpB9));_tmpB9[0]=({struct Cyc_Absyn_ArrayType_struct _tmpBA;_tmpBA.tag=7;_tmpBA.f1=({
struct Cyc_Absyn_ArrayInfo _tmpBB;_tmpBB.elt_type=(void*)Cyc_Tcutil_copy_type(
_tmp7A);_tmpBB.tq=_tmp7B;_tmpBB.num_elts=_tmp7C;_tmpBB.zero_term=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp7D);_tmpBB.zt_loc=_tmp7E;
_tmpBB;});_tmpBA;});_tmpB9;});_LL2E: if(*((int*)_tmp64)!= 8)goto _LL30;_tmp7F=((
struct Cyc_Absyn_FnType_struct*)_tmp64)->f1;_tmp80=_tmp7F.tvars;_tmp81=_tmp7F.effect;
_tmp82=(void*)_tmp7F.ret_typ;_tmp83=_tmp7F.args;_tmp84=_tmp7F.c_varargs;_tmp85=
_tmp7F.cyc_varargs;_tmp86=_tmp7F.rgn_po;_tmp87=_tmp7F.attributes;_LL2F: {struct
Cyc_List_List*_tmpBC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,
_tmp80);struct Cyc_Core_Opt*_tmpBD=_tmp81 == 0?0:({struct Cyc_Core_Opt*_tmpC7=
_cycalloc(sizeof(*_tmpC7));_tmpC7->v=(void*)Cyc_Tcutil_copy_type((void*)_tmp81->v);
_tmpC7;});void*_tmpBE=Cyc_Tcutil_copy_type(_tmp82);struct Cyc_List_List*_tmpBF=((
struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp83);int _tmpC0=_tmp84;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp85 != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp85;cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpC1=_cycalloc(sizeof(*_tmpC1));
_tmpC1->name=cv->name;_tmpC1->tq=cv->tq;_tmpC1->type=(void*)Cyc_Tcutil_copy_type((
void*)cv->type);_tmpC1->inject=cv->inject;_tmpC1;});}{struct Cyc_List_List*_tmpC2=((
struct Cyc_List_List*(*)(struct _tuple6*(*f)(struct _tuple6*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp86);struct Cyc_List_List*_tmpC3=_tmp87;
return(void*)({struct Cyc_Absyn_FnType_struct*_tmpC4=_cycalloc(sizeof(*_tmpC4));
_tmpC4[0]=({struct Cyc_Absyn_FnType_struct _tmpC5;_tmpC5.tag=8;_tmpC5.f1=({struct
Cyc_Absyn_FnInfo _tmpC6;_tmpC6.tvars=_tmpBC;_tmpC6.effect=_tmpBD;_tmpC6.ret_typ=(
void*)_tmpBE;_tmpC6.args=_tmpBF;_tmpC6.c_varargs=_tmpC0;_tmpC6.cyc_varargs=
cyc_varargs2;_tmpC6.rgn_po=_tmpC2;_tmpC6.attributes=_tmpC3;_tmpC6;});_tmpC5;});
_tmpC4;});}}_LL30: if(*((int*)_tmp64)!= 9)goto _LL32;_tmp88=((struct Cyc_Absyn_TupleType_struct*)
_tmp64)->f1;_LL31: return(void*)({struct Cyc_Absyn_TupleType_struct*_tmpC8=
_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=({struct Cyc_Absyn_TupleType_struct _tmpC9;
_tmpC9.tag=9;_tmpC9.f1=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp88);
_tmpC9;});_tmpC8;});_LL32: if(*((int*)_tmp64)!= 10)goto _LL34;_tmp89=((struct Cyc_Absyn_AggrType_struct*)
_tmp64)->f1;_tmp8A=_tmp89.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp64)->f1).aggr_info).UnknownAggr).tag != 0)goto _LL34;_tmp8B=(_tmp8A.UnknownAggr).f1;
_tmp8C=(_tmp8A.UnknownAggr).f2;_tmp8D=_tmp89.targs;_LL33: return(void*)({struct
Cyc_Absyn_AggrType_struct*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA[0]=({struct
Cyc_Absyn_AggrType_struct _tmpCB;_tmpCB.tag=10;_tmpCB.f1=({struct Cyc_Absyn_AggrInfo
_tmpCC;_tmpCC.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmpCD;(_tmpCD.UnknownAggr).tag=0;(_tmpCD.UnknownAggr).f1=(void*)_tmp8B;(_tmpCD.UnknownAggr).f2=
_tmp8C;_tmpCD;});_tmpCC.targs=Cyc_Tcutil_copy_types(_tmp8D);_tmpCC;});_tmpCB;});
_tmpCA;});_LL34: if(*((int*)_tmp64)!= 10)goto _LL36;_tmp8E=((struct Cyc_Absyn_AggrType_struct*)
_tmp64)->f1;_tmp8F=_tmp8E.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp64)->f1).aggr_info).KnownAggr).tag != 1)goto _LL36;_tmp90=(_tmp8F.KnownAggr).f1;
_tmp91=_tmp8E.targs;_LL35: return(void*)({struct Cyc_Absyn_AggrType_struct*_tmpCE=
_cycalloc(sizeof(*_tmpCE));_tmpCE[0]=({struct Cyc_Absyn_AggrType_struct _tmpCF;
_tmpCF.tag=10;_tmpCF.f1=({struct Cyc_Absyn_AggrInfo _tmpD0;_tmpD0.aggr_info=(union
Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmpD1;(_tmpD1.KnownAggr).tag=
1;(_tmpD1.KnownAggr).f1=_tmp90;_tmpD1;});_tmpD0.targs=Cyc_Tcutil_copy_types(
_tmp91);_tmpD0;});_tmpCF;});_tmpCE;});_LL36: if(*((int*)_tmp64)!= 11)goto _LL38;
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
_tmpF7;struct Cyc_Absyn_Tvar*_tmpF8;void*_tmpF9;struct Cyc_Absyn_TunionInfo _tmpFA;
union Cyc_Absyn_TunionInfoU_union _tmpFB;struct Cyc_Absyn_UnknownTunionInfo _tmpFC;
struct Cyc_Absyn_TunionInfo _tmpFD;union Cyc_Absyn_TunionInfoU_union _tmpFE;struct
Cyc_Absyn_Tuniondecl**_tmpFF;struct Cyc_Absyn_Tuniondecl*_tmp100;struct Cyc_Absyn_TunionFieldInfo
_tmp101;union Cyc_Absyn_TunionFieldInfoU_union _tmp102;struct Cyc_Absyn_Tuniondecl*
_tmp103;struct Cyc_Absyn_Tunionfield*_tmp104;struct Cyc_Absyn_TunionFieldInfo
_tmp105;union Cyc_Absyn_TunionFieldInfoU_union _tmp106;struct Cyc_Absyn_Enumdecl*
_tmp107;struct Cyc_Absyn_AggrInfo _tmp108;union Cyc_Absyn_AggrInfoU_union _tmp109;
struct Cyc_Absyn_AggrInfo _tmp10A;union Cyc_Absyn_AggrInfoU_union _tmp10B;struct Cyc_Absyn_Aggrdecl**
_tmp10C;struct Cyc_Absyn_Aggrdecl*_tmp10D;struct Cyc_Absyn_Aggrdecl _tmp10E;struct
Cyc_Absyn_AggrdeclImpl*_tmp10F;struct Cyc_Absyn_Enumdecl*_tmp110;struct Cyc_Absyn_PtrInfo
_tmp111;struct Cyc_Absyn_Typedefdecl*_tmp112;_LL61: if(_tmpF5 <= (void*)4)goto _LL65;
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
if(_tmpF5 <= (void*)4)goto _LL77;if(*((int*)_tmpF5)!= 2)goto _LL77;_tmpFA=((struct
Cyc_Absyn_TunionType_struct*)_tmpF5)->f1;_tmpFB=_tmpFA.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmpF5)->f1).tunion_info).UnknownTunion).tag
!= 0)goto _LL77;_tmpFC=(_tmpFB.UnknownTunion).f1;_LL76: if(_tmpFC.is_flat)return(
void*)1;else{return(void*)2;}_LL77: if(_tmpF5 <= (void*)4)goto _LL79;if(*((int*)
_tmpF5)!= 2)goto _LL79;_tmpFD=((struct Cyc_Absyn_TunionType_struct*)_tmpF5)->f1;
_tmpFE=_tmpFD.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)_tmpF5)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL79;_tmpFF=(_tmpFE.KnownTunion).f1;_tmp100=*_tmpFF;_LL78: if(_tmp100->is_flat)
return(void*)1;else{return(void*)2;}_LL79: if(_tmpF5 <= (void*)4)goto _LL7B;if(*((
int*)_tmpF5)!= 3)goto _LL7B;_tmp101=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF5)->f1;_tmp102=_tmp101.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF5)->f1).field_info).KnownTunionfield).tag != 1)goto _LL7B;_tmp103=(_tmp102.KnownTunionfield).f1;
_tmp104=(_tmp102.KnownTunionfield).f2;_LL7A: if(_tmp103->is_flat)return(void*)1;
else{if(_tmp104->typs == 0)return(void*)2;else{return(void*)1;}}_LL7B: if(_tmpF5 <= (
void*)4)goto _LL7D;if(*((int*)_tmpF5)!= 3)goto _LL7D;_tmp105=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF5)->f1;_tmp106=_tmp105.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF5)->f1).field_info).UnknownTunionfield).tag != 0)goto _LL7D;_LL7C:({void*
_tmp113[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp114="typ_kind: Unresolved TunionFieldType";_tag_dynforward(_tmp114,
sizeof(char),_get_zero_arr_size(_tmp114,37));}),_tag_dynforward(_tmp113,sizeof(
void*),0));});_LL7D: if(_tmpF5 <= (void*)4)goto _LL7F;if(*((int*)_tmpF5)!= 12)goto
_LL7F;_tmp107=((struct Cyc_Absyn_EnumType_struct*)_tmpF5)->f2;if(_tmp107 != 0)goto
_LL7F;_LL7E: goto _LL80;_LL7F: if(_tmpF5 <= (void*)4)goto _LL81;if(*((int*)_tmpF5)!= 
10)goto _LL81;_tmp108=((struct Cyc_Absyn_AggrType_struct*)_tmpF5)->f1;_tmp109=
_tmp108.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpF5)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL81;_LL80: return(void*)0;_LL81: if(_tmpF5 <= (void*)4)goto _LL83;if(*((
int*)_tmpF5)!= 10)goto _LL83;_tmp10A=((struct Cyc_Absyn_AggrType_struct*)_tmpF5)->f1;
_tmp10B=_tmp10A.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpF5)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL83;_tmp10C=(_tmp10B.KnownAggr).f1;_tmp10D=*_tmp10C;_tmp10E=*_tmp10D;
_tmp10F=_tmp10E.impl;_LL82: return _tmp10F == 0?(void*)0:(void*)1;_LL83: if(_tmpF5 <= (
void*)4)goto _LL85;if(*((int*)_tmpF5)!= 11)goto _LL85;_LL84: goto _LL86;_LL85: if(
_tmpF5 <= (void*)4)goto _LL87;if(*((int*)_tmpF5)!= 13)goto _LL87;_LL86: return(void*)
1;_LL87: if(_tmpF5 <= (void*)4)goto _LL89;if(*((int*)_tmpF5)!= 12)goto _LL89;_tmp110=((
struct Cyc_Absyn_EnumType_struct*)_tmpF5)->f2;_LL88: if(_tmp110->fields == 0)return(
void*)0;else{return(void*)1;}_LL89: if(_tmpF5 <= (void*)4)goto _LL8B;if(*((int*)
_tmpF5)!= 4)goto _LL8B;_tmp111=((struct Cyc_Absyn_PointerType_struct*)_tmpF5)->f1;
_LL8A: {union Cyc_Absyn_Constraint_union _tmp115=(Cyc_Absyn_compress_conref((
_tmp111.ptr_atts).bounds))->v;void*_tmp116;void*_tmp117;void*_tmp118;void*
_tmp119;_LL9E: if((_tmp115.No_constr).tag != 2)goto _LLA0;_LL9F: goto _LLA1;_LLA0: if((
_tmp115.Eq_constr).tag != 0)goto _LLA2;_tmp116=(_tmp115.Eq_constr).f1;if((int)
_tmp116 != 0)goto _LLA2;_LLA1: return(void*)1;_LLA2: if((_tmp115.Eq_constr).tag != 0)
goto _LLA4;_tmp117=(_tmp115.Eq_constr).f1;if((int)_tmp117 != 1)goto _LLA4;_LLA3:
return(void*)1;_LLA4: if((_tmp115.Eq_constr).tag != 0)goto _LLA6;_tmp118=(_tmp115.Eq_constr).f1;
if(_tmp118 <= (void*)2)goto _LLA6;if(*((int*)_tmp118)!= 0)goto _LLA6;_LLA5: goto
_LLA7;_LLA6: if((_tmp115.Eq_constr).tag != 0)goto _LLA8;_tmp119=(_tmp115.Eq_constr).f1;
if(_tmp119 <= (void*)2)goto _LLA8;if(*((int*)_tmp119)!= 1)goto _LLA8;_LLA7: return(
void*)2;_LLA8: if((_tmp115.Forward_constr).tag != 1)goto _LL9D;_LLA9:({void*_tmp11A[
0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp11B="typ_kind: forward constr in ptr bounds";_tag_dynforward(
_tmp11B,sizeof(char),_get_zero_arr_size(_tmp11B,39));}),_tag_dynforward(_tmp11A,
sizeof(void*),0));});_LL9D:;}_LL8B: if(_tmpF5 <= (void*)4)goto _LL8D;if(*((int*)
_tmpF5)!= 14)goto _LL8D;_LL8C: return(void*)2;_LL8D: if(_tmpF5 <= (void*)4)goto _LL8F;
if(*((int*)_tmpF5)!= 18)goto _LL8F;_LL8E: return(void*)5;_LL8F: if(_tmpF5 <= (void*)
4)goto _LL91;if(*((int*)_tmpF5)!= 17)goto _LL91;_LL90: return(void*)2;_LL91: if(
_tmpF5 <= (void*)4)goto _LL93;if(*((int*)_tmpF5)!= 7)goto _LL93;_LL92: goto _LL94;
_LL93: if(_tmpF5 <= (void*)4)goto _LL95;if(*((int*)_tmpF5)!= 9)goto _LL95;_LL94:
return(void*)1;_LL95: if(_tmpF5 <= (void*)4)goto _LL97;if(*((int*)_tmpF5)!= 16)goto
_LL97;_tmp112=((struct Cyc_Absyn_TypedefType_struct*)_tmpF5)->f3;_LL96: if(_tmp112
== 0  || _tmp112->kind == 0)({struct Cyc_String_pa_struct _tmp11E;_tmp11E.tag=0;
_tmp11E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp11C[1]={& _tmp11E};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp11D="typ_kind: typedef found: %s";
_tag_dynforward(_tmp11D,sizeof(char),_get_zero_arr_size(_tmp11D,28));}),
_tag_dynforward(_tmp11C,sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp112->kind))->v;_LL97: if(_tmpF5 <= (void*)4)goto _LL99;if(*((int*)
_tmpF5)!= 19)goto _LL99;_LL98: goto _LL9A;_LL99: if(_tmpF5 <= (void*)4)goto _LL9B;if(*((
int*)_tmpF5)!= 20)goto _LL9B;_LL9A: goto _LL9C;_LL9B: if(_tmpF5 <= (void*)4)goto _LL60;
if(*((int*)_tmpF5)!= 21)goto _LL60;_LL9C: return(void*)4;_LL60:;}int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp11F;_push_handler(& _tmp11F);{int _tmp121=
0;if(setjmp(_tmp11F.handler))_tmp121=1;if(!_tmp121){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp122=1;_npop_handler(0);return _tmp122;};_pop_handler();}else{void*_tmp120=(
void*)_exn_thrown;void*_tmp124=_tmp120;_LLAB: if(_tmp124 != Cyc_Tcutil_Unify)goto
_LLAD;_LLAC: return 0;_LLAD:;_LLAE:(void)_throw(_tmp124);_LLAA:;}}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,void*t){t=Cyc_Tcutil_compress(t);{void*_tmp125=t;struct Cyc_Absyn_Tvar*
_tmp126;struct Cyc_Core_Opt*_tmp127;struct Cyc_Core_Opt*_tmp128;struct Cyc_Core_Opt**
_tmp129;struct Cyc_Absyn_PtrInfo _tmp12A;struct Cyc_Absyn_ArrayInfo _tmp12B;void*
_tmp12C;struct Cyc_Absyn_FnInfo _tmp12D;struct Cyc_List_List*_tmp12E;struct Cyc_Core_Opt*
_tmp12F;void*_tmp130;struct Cyc_List_List*_tmp131;int _tmp132;struct Cyc_Absyn_VarargInfo*
_tmp133;struct Cyc_List_List*_tmp134;struct Cyc_List_List*_tmp135;struct Cyc_List_List*
_tmp136;struct Cyc_Absyn_TunionInfo _tmp137;struct Cyc_List_List*_tmp138;struct Cyc_Core_Opt*
_tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_Absyn_TunionFieldInfo _tmp13B;
struct Cyc_List_List*_tmp13C;struct Cyc_Absyn_AggrInfo _tmp13D;struct Cyc_List_List*
_tmp13E;struct Cyc_List_List*_tmp13F;void*_tmp140;void*_tmp141;void*_tmp142;void*
_tmp143;void*_tmp144;struct Cyc_List_List*_tmp145;_LLB0: if(_tmp125 <= (void*)4)
goto _LLD2;if(*((int*)_tmp125)!= 1)goto _LLB2;_tmp126=((struct Cyc_Absyn_VarType_struct*)
_tmp125)->f1;_LLB1: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,_tmp126)){Cyc_Tcutil_failure_reason=({const char*_tmp146="(type variable would escape scope)";
_tag_dynforward(_tmp146,sizeof(char),_get_zero_arr_size(_tmp146,35));});(int)
_throw((void*)Cyc_Tcutil_Unify);}goto _LLAF;_LLB2: if(*((int*)_tmp125)!= 0)goto
_LLB4;_tmp127=((struct Cyc_Absyn_Evar_struct*)_tmp125)->f2;_tmp128=((struct Cyc_Absyn_Evar_struct*)
_tmp125)->f4;_tmp129=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp125)->f4;_LLB3: if(t == evar){Cyc_Tcutil_failure_reason=({const char*_tmp147="(occurs check)";
_tag_dynforward(_tmp147,sizeof(char),_get_zero_arr_size(_tmp147,15));});(int)
_throw((void*)Cyc_Tcutil_Unify);}else{if(_tmp127 != 0)Cyc_Tcutil_occurs(evar,r,
env,(void*)_tmp127->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp129))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp148=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp129))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd))_tmp148=({struct Cyc_List_List*_tmp149=
_cycalloc(sizeof(*_tmp149));_tmp149->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp149->tl=
_tmp148;_tmp149;});}}*_tmp129=({struct Cyc_Core_Opt*_tmp14A=_cycalloc(sizeof(*
_tmp14A));_tmp14A->v=_tmp148;_tmp14A;});}}}goto _LLAF;_LLB4: if(*((int*)_tmp125)!= 
4)goto _LLB6;_tmp12A=((struct Cyc_Absyn_PointerType_struct*)_tmp125)->f1;_LLB5: Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp12A.elt_typ);Cyc_Tcutil_occurs(evar,r,env,(void*)(_tmp12A.ptr_atts).rgn);{
union Cyc_Absyn_Constraint_union _tmp14B=(Cyc_Absyn_compress_conref((_tmp12A.ptr_atts).bounds))->v;
void*_tmp14C;void*_tmp14D;_LLD5: if((_tmp14B.Eq_constr).tag != 0)goto _LLD7;_tmp14C=(
_tmp14B.Eq_constr).f1;if(_tmp14C <= (void*)2)goto _LLD7;if(*((int*)_tmp14C)!= 1)
goto _LLD7;_tmp14D=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp14C)->f1;_LLD6:
Cyc_Tcutil_occurs(evar,r,env,_tmp14D);goto _LLD4;_LLD7:;_LLD8: goto _LLD4;_LLD4:;}
goto _LLAF;_LLB6: if(*((int*)_tmp125)!= 7)goto _LLB8;_tmp12B=((struct Cyc_Absyn_ArrayType_struct*)
_tmp125)->f1;_tmp12C=(void*)_tmp12B.elt_type;_LLB7: Cyc_Tcutil_occurs(evar,r,env,
_tmp12C);goto _LLAF;_LLB8: if(*((int*)_tmp125)!= 8)goto _LLBA;_tmp12D=((struct Cyc_Absyn_FnType_struct*)
_tmp125)->f1;_tmp12E=_tmp12D.tvars;_tmp12F=_tmp12D.effect;_tmp130=(void*)_tmp12D.ret_typ;
_tmp131=_tmp12D.args;_tmp132=_tmp12D.c_varargs;_tmp133=_tmp12D.cyc_varargs;
_tmp134=_tmp12D.rgn_po;_tmp135=_tmp12D.attributes;_LLB9: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp12E,env);if(_tmp12F != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp12F->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp130);for(0;_tmp131 != 0;_tmp131=_tmp131->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple2*)_tmp131->hd)).f3);}if(_tmp133 != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp133->type);for(0;_tmp134 != 0;_tmp134=_tmp134->tl){struct
_tuple6 _tmp14F;void*_tmp150;void*_tmp151;struct _tuple6*_tmp14E=(struct _tuple6*)
_tmp134->hd;_tmp14F=*_tmp14E;_tmp150=_tmp14F.f1;_tmp151=_tmp14F.f2;Cyc_Tcutil_occurs(
evar,r,env,_tmp150);Cyc_Tcutil_occurs(evar,r,env,_tmp151);}goto _LLAF;_LLBA: if(*((
int*)_tmp125)!= 9)goto _LLBC;_tmp136=((struct Cyc_Absyn_TupleType_struct*)_tmp125)->f1;
_LLBB: for(0;_tmp136 != 0;_tmp136=_tmp136->tl){Cyc_Tcutil_occurs(evar,r,env,(*((
struct _tuple4*)_tmp136->hd)).f2);}goto _LLAF;_LLBC: if(*((int*)_tmp125)!= 2)goto
_LLBE;_tmp137=((struct Cyc_Absyn_TunionType_struct*)_tmp125)->f1;_tmp138=_tmp137.targs;
_tmp139=_tmp137.rgn;_LLBD: if((unsigned int)_tmp139)Cyc_Tcutil_occurs(evar,r,env,(
void*)_tmp139->v);Cyc_Tcutil_occurslist(evar,r,env,_tmp138);goto _LLAF;_LLBE: if(*((
int*)_tmp125)!= 16)goto _LLC0;_tmp13A=((struct Cyc_Absyn_TypedefType_struct*)
_tmp125)->f2;_LLBF: _tmp13C=_tmp13A;goto _LLC1;_LLC0: if(*((int*)_tmp125)!= 3)goto
_LLC2;_tmp13B=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp125)->f1;_tmp13C=
_tmp13B.targs;_LLC1: _tmp13E=_tmp13C;goto _LLC3;_LLC2: if(*((int*)_tmp125)!= 10)
goto _LLC4;_tmp13D=((struct Cyc_Absyn_AggrType_struct*)_tmp125)->f1;_tmp13E=
_tmp13D.targs;_LLC3: Cyc_Tcutil_occurslist(evar,r,env,_tmp13E);goto _LLAF;_LLC4:
if(*((int*)_tmp125)!= 11)goto _LLC6;_tmp13F=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp125)->f2;_LLC5: for(0;_tmp13F != 0;_tmp13F=_tmp13F->tl){Cyc_Tcutil_occurs(evar,
r,env,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp13F->hd)->type);}goto _LLAF;_LLC6:
if(*((int*)_tmp125)!= 17)goto _LLC8;_tmp140=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp125)->f1;_LLC7: _tmp141=_tmp140;goto _LLC9;_LLC8: if(*((int*)_tmp125)!= 14)goto
_LLCA;_tmp141=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp125)->f1;_LLC9:
_tmp142=_tmp141;goto _LLCB;_LLCA: if(*((int*)_tmp125)!= 19)goto _LLCC;_tmp142=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp125)->f1;_LLCB: _tmp143=_tmp142;goto _LLCD;
_LLCC: if(*((int*)_tmp125)!= 15)goto _LLCE;_tmp143=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp125)->f1;_LLCD: _tmp144=_tmp143;goto _LLCF;_LLCE: if(*((int*)_tmp125)!= 21)goto
_LLD0;_tmp144=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp125)->f1;_LLCF: Cyc_Tcutil_occurs(
evar,r,env,_tmp144);goto _LLAF;_LLD0: if(*((int*)_tmp125)!= 20)goto _LLD2;_tmp145=((
struct Cyc_Absyn_JoinEff_struct*)_tmp125)->f1;_LLD1: Cyc_Tcutil_occurslist(evar,r,
env,_tmp145);goto _LLAF;_LLD2:;_LLD3: goto _LLAF;_LLAF:;}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}static void
Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(0;t1 != 
0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)(int)_throw((void*)Cyc_Tcutil_Unify);}static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const
 && !tq1.real_const)({void*_tmp152[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp153="tq1 real_const not set.";
_tag_dynforward(_tmp153,sizeof(char),_get_zero_arr_size(_tmp153,24));}),
_tag_dynforward(_tmp152,sizeof(void*),0));});if(tq2.print_const  && !tq2.real_const)({
void*_tmp154[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp155="tq2 real_const not set.";_tag_dynforward(
_tmp155,sizeof(char),_get_zero_arr_size(_tmp155,24));}),_tag_dynforward(_tmp154,
sizeof(void*),0));});if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile)
 || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=tq2.real_const;Cyc_Tcutil_failure_reason=({
const char*_tmp156="(qualifiers don't match)";_tag_dynforward(_tmp156,sizeof(char),
_get_zero_arr_size(_tmp156,25));});(int)_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_tq1_const=
0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_dynforward_ptr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(
y);if(x == y)return;{union Cyc_Absyn_Constraint_union _tmp157=x->v;void*_tmp158;
_LLDA: if((_tmp157.No_constr).tag != 2)goto _LLDC;_LLDB: x->v=(union Cyc_Absyn_Constraint_union)({
union Cyc_Absyn_Constraint_union _tmp159;(_tmp159.Forward_constr).tag=1;(_tmp159.Forward_constr).f1=
y;_tmp159;});return;_LLDC: if((_tmp157.Eq_constr).tag != 0)goto _LLDE;_tmp158=(
_tmp157.Eq_constr).f1;_LLDD: {union Cyc_Absyn_Constraint_union _tmp15A=y->v;void*
_tmp15B;_LLE1: if((_tmp15A.No_constr).tag != 2)goto _LLE3;_LLE2: y->v=x->v;return;
_LLE3: if((_tmp15A.Eq_constr).tag != 0)goto _LLE5;_tmp15B=(_tmp15A.Eq_constr).f1;
_LLE4: if(cmp(_tmp158,_tmp15B)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((
void*)Cyc_Tcutil_Unify);}return;_LLE5: if((_tmp15A.Forward_constr).tag != 1)goto
_LLE0;_LLE6:({void*_tmp15C[0]={};Cyc_Tcutil_impos(({const char*_tmp15D="unify_conref: forward after compress(2)";
_tag_dynforward(_tmp15D,sizeof(char),_get_zero_arr_size(_tmp15D,40));}),
_tag_dynforward(_tmp15C,sizeof(void*),0));});_LLE0:;}_LLDE: if((_tmp157.Forward_constr).tag
!= 1)goto _LLD9;_LLDF:({void*_tmp15E[0]={};Cyc_Tcutil_impos(({const char*_tmp15F="unify_conref: forward after compress";
_tag_dynforward(_tmp15F,sizeof(char),_get_zero_arr_size(_tmp15F,37));}),
_tag_dynforward(_tmp15E,sizeof(void*),0));});_LLD9:;}}static int Cyc_Tcutil_unify_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct
_handler_cons _tmp160;_push_handler(& _tmp160);{int _tmp162=0;if(setjmp(_tmp160.handler))
_tmp162=1;if(!_tmp162){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct
_dynforward_ptr)_tag_dynforward(0,0,0));{int _tmp163=1;_npop_handler(0);return
_tmp163;};_pop_handler();}else{void*_tmp161=(void*)_exn_thrown;void*_tmp165=
_tmp161;_LLE8: if(_tmp165 != Cyc_Tcutil_Unify)goto _LLEA;_LLE9: return 0;_LLEA:;_LLEB:(
void)_throw(_tmp165);_LLE7:;}}}static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple6 _tmp167=({struct _tuple6 _tmp166;_tmp166.f1=b1;_tmp166.f2=b2;_tmp166;});
void*_tmp168;void*_tmp169;void*_tmp16A;void*_tmp16B;void*_tmp16C;void*_tmp16D;
void*_tmp16E;void*_tmp16F;void*_tmp170;struct Cyc_Absyn_Exp*_tmp171;void*_tmp172;
struct Cyc_Absyn_Exp*_tmp173;void*_tmp174;void*_tmp175;void*_tmp176;void*_tmp177;
void*_tmp178;void*_tmp179;void*_tmp17A;void*_tmp17B;_LLED: _tmp168=_tmp167.f1;if((
int)_tmp168 != 0)goto _LLEF;_tmp169=_tmp167.f2;if((int)_tmp169 != 0)goto _LLEF;_LLEE:
return 0;_LLEF: _tmp16A=_tmp167.f1;if((int)_tmp16A != 0)goto _LLF1;_LLF0: return - 1;
_LLF1: _tmp16B=_tmp167.f2;if((int)_tmp16B != 0)goto _LLF3;_LLF2: return 1;_LLF3:
_tmp16C=_tmp167.f1;if((int)_tmp16C != 1)goto _LLF5;_tmp16D=_tmp167.f2;if((int)
_tmp16D != 1)goto _LLF5;_LLF4: return 0;_LLF5: _tmp16E=_tmp167.f1;if((int)_tmp16E != 1)
goto _LLF7;_LLF6: return - 1;_LLF7: _tmp16F=_tmp167.f2;if((int)_tmp16F != 1)goto _LLF9;
_LLF8: return 1;_LLF9: _tmp170=_tmp167.f1;if(_tmp170 <= (void*)2)goto _LLFB;if(*((int*)
_tmp170)!= 0)goto _LLFB;_tmp171=((struct Cyc_Absyn_Upper_b_struct*)_tmp170)->f1;
_tmp172=_tmp167.f2;if(_tmp172 <= (void*)2)goto _LLFB;if(*((int*)_tmp172)!= 0)goto
_LLFB;_tmp173=((struct Cyc_Absyn_Upper_b_struct*)_tmp172)->f1;_LLFA: return Cyc_Evexp_const_exp_cmp(
_tmp171,_tmp173);_LLFB: _tmp174=_tmp167.f1;if(_tmp174 <= (void*)2)goto _LLFD;if(*((
int*)_tmp174)!= 0)goto _LLFD;_tmp175=_tmp167.f2;if(_tmp175 <= (void*)2)goto _LLFD;
if(*((int*)_tmp175)!= 1)goto _LLFD;_LLFC: return - 1;_LLFD: _tmp176=_tmp167.f1;if(
_tmp176 <= (void*)2)goto _LLFF;if(*((int*)_tmp176)!= 1)goto _LLFF;_tmp177=_tmp167.f2;
if(_tmp177 <= (void*)2)goto _LLFF;if(*((int*)_tmp177)!= 0)goto _LLFF;_LLFE: return 1;
_LLFF: _tmp178=_tmp167.f1;if(_tmp178 <= (void*)2)goto _LLEC;if(*((int*)_tmp178)!= 1)
goto _LLEC;_tmp179=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp178)->f1;
_tmp17A=_tmp167.f2;if(_tmp17A <= (void*)2)goto _LLEC;if(*((int*)_tmp17A)!= 1)goto
_LLEC;_tmp17B=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp17A)->f1;_LL100:
return Cyc_Tcutil_typecmp(_tmp179,_tmp17B);_LLEC:;}static int Cyc_Tcutil_unify_it_bounds(
void*b1,void*b2){struct _tuple6 _tmp17D=({struct _tuple6 _tmp17C;_tmp17C.f1=b1;
_tmp17C.f2=b2;_tmp17C;});void*_tmp17E;void*_tmp17F;void*_tmp180;void*_tmp181;
void*_tmp182;void*_tmp183;void*_tmp184;void*_tmp185;void*_tmp186;struct Cyc_Absyn_Exp*
_tmp187;void*_tmp188;struct Cyc_Absyn_Exp*_tmp189;void*_tmp18A;struct Cyc_Absyn_Exp*
_tmp18B;void*_tmp18C;void*_tmp18D;void*_tmp18E;void*_tmp18F;void*_tmp190;struct
Cyc_Absyn_Exp*_tmp191;void*_tmp192;void*_tmp193;void*_tmp194;void*_tmp195;_LL102:
_tmp17E=_tmp17D.f1;if((int)_tmp17E != 0)goto _LL104;_tmp17F=_tmp17D.f2;if((int)
_tmp17F != 0)goto _LL104;_LL103: return 0;_LL104: _tmp180=_tmp17D.f1;if((int)_tmp180
!= 0)goto _LL106;_LL105: return - 1;_LL106: _tmp181=_tmp17D.f2;if((int)_tmp181 != 0)
goto _LL108;_LL107: return 1;_LL108: _tmp182=_tmp17D.f1;if((int)_tmp182 != 1)goto
_LL10A;_tmp183=_tmp17D.f2;if((int)_tmp183 != 1)goto _LL10A;_LL109: return 0;_LL10A:
_tmp184=_tmp17D.f1;if((int)_tmp184 != 1)goto _LL10C;_LL10B: return - 1;_LL10C: _tmp185=
_tmp17D.f2;if((int)_tmp185 != 1)goto _LL10E;_LL10D: return 1;_LL10E: _tmp186=_tmp17D.f1;
if(_tmp186 <= (void*)2)goto _LL110;if(*((int*)_tmp186)!= 0)goto _LL110;_tmp187=((
struct Cyc_Absyn_Upper_b_struct*)_tmp186)->f1;_tmp188=_tmp17D.f2;if(_tmp188 <= (
void*)2)goto _LL110;if(*((int*)_tmp188)!= 0)goto _LL110;_tmp189=((struct Cyc_Absyn_Upper_b_struct*)
_tmp188)->f1;_LL10F: return Cyc_Evexp_const_exp_cmp(_tmp187,_tmp189);_LL110:
_tmp18A=_tmp17D.f1;if(_tmp18A <= (void*)2)goto _LL112;if(*((int*)_tmp18A)!= 0)goto
_LL112;_tmp18B=((struct Cyc_Absyn_Upper_b_struct*)_tmp18A)->f1;_tmp18C=_tmp17D.f2;
if(_tmp18C <= (void*)2)goto _LL112;if(*((int*)_tmp18C)!= 1)goto _LL112;_tmp18D=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp18C)->f1;_LL111: _tmp18F=_tmp18D;
_tmp191=_tmp18B;goto _LL113;_LL112: _tmp18E=_tmp17D.f1;if(_tmp18E <= (void*)2)goto
_LL114;if(*((int*)_tmp18E)!= 1)goto _LL114;_tmp18F=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp18E)->f1;_tmp190=_tmp17D.f2;if(_tmp190 <= (void*)2)goto _LL114;if(*((int*)
_tmp190)!= 0)goto _LL114;_tmp191=((struct Cyc_Absyn_Upper_b_struct*)_tmp190)->f1;
_LL113: {unsigned int _tmp197;int _tmp198;struct _tuple7 _tmp196=Cyc_Evexp_eval_const_uint_exp(
_tmp191);_tmp197=_tmp196.f1;_tmp198=_tmp196.f2;if(!_tmp198)return 1;_tmp193=
_tmp18F;_tmp195=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp199=_cycalloc_atomic(
sizeof(*_tmp199));_tmp199[0]=({struct Cyc_Absyn_TypeInt_struct _tmp19A;_tmp19A.tag=
18;_tmp19A.f1=(int)_tmp197;_tmp19A;});_tmp199;});goto _LL115;}_LL114: _tmp192=
_tmp17D.f1;if(_tmp192 <= (void*)2)goto _LL101;if(*((int*)_tmp192)!= 1)goto _LL101;
_tmp193=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp192)->f1;_tmp194=_tmp17D.f2;
if(_tmp194 <= (void*)2)goto _LL101;if(*((int*)_tmp194)!= 1)goto _LL101;_tmp195=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp194)->f1;_LL115: Cyc_Tcutil_unify_it(
_tmp193,_tmp195);return 0;_LL101:;}static int Cyc_Tcutil_attribute_case_number(void*
att){void*_tmp19B=att;_LL117: if(_tmp19B <= (void*)17)goto _LL119;if(*((int*)
_tmp19B)!= 0)goto _LL119;_LL118: return 0;_LL119: if((int)_tmp19B != 0)goto _LL11B;
_LL11A: return 1;_LL11B: if((int)_tmp19B != 1)goto _LL11D;_LL11C: return 2;_LL11D: if((
int)_tmp19B != 2)goto _LL11F;_LL11E: return 3;_LL11F: if((int)_tmp19B != 3)goto _LL121;
_LL120: return 4;_LL121: if((int)_tmp19B != 4)goto _LL123;_LL122: return 5;_LL123: if(
_tmp19B <= (void*)17)goto _LL125;if(*((int*)_tmp19B)!= 1)goto _LL125;_LL124: return 6;
_LL125: if((int)_tmp19B != 5)goto _LL127;_LL126: return 7;_LL127: if(_tmp19B <= (void*)
17)goto _LL129;if(*((int*)_tmp19B)!= 2)goto _LL129;_LL128: return 8;_LL129: if((int)
_tmp19B != 6)goto _LL12B;_LL12A: return 9;_LL12B: if((int)_tmp19B != 7)goto _LL12D;
_LL12C: return 10;_LL12D: if((int)_tmp19B != 8)goto _LL12F;_LL12E: return 11;_LL12F: if((
int)_tmp19B != 9)goto _LL131;_LL130: return 12;_LL131: if((int)_tmp19B != 10)goto
_LL133;_LL132: return 13;_LL133: if((int)_tmp19B != 11)goto _LL135;_LL134: return 14;
_LL135: if((int)_tmp19B != 12)goto _LL137;_LL136: return 15;_LL137: if((int)_tmp19B != 
13)goto _LL139;_LL138: return 16;_LL139: if((int)_tmp19B != 14)goto _LL13B;_LL13A:
return 17;_LL13B: if((int)_tmp19B != 15)goto _LL13D;_LL13C: return 18;_LL13D: if(
_tmp19B <= (void*)17)goto _LL141;if(*((int*)_tmp19B)!= 3)goto _LL13F;_LL13E: return
19;_LL13F: if(*((int*)_tmp19B)!= 4)goto _LL141;_LL140: return 20;_LL141:;_LL142:
return 21;_LL116:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){struct
_tuple6 _tmp19D=({struct _tuple6 _tmp19C;_tmp19C.f1=att1;_tmp19C.f2=att2;_tmp19C;});
void*_tmp19E;int _tmp19F;void*_tmp1A0;int _tmp1A1;void*_tmp1A2;int _tmp1A3;void*
_tmp1A4;int _tmp1A5;void*_tmp1A6;int _tmp1A7;void*_tmp1A8;int _tmp1A9;void*_tmp1AA;
struct _dynforward_ptr _tmp1AB;void*_tmp1AC;struct _dynforward_ptr _tmp1AD;void*
_tmp1AE;void*_tmp1AF;int _tmp1B0;int _tmp1B1;void*_tmp1B2;void*_tmp1B3;int _tmp1B4;
int _tmp1B5;_LL144: _tmp19E=_tmp19D.f1;if(_tmp19E <= (void*)17)goto _LL146;if(*((int*)
_tmp19E)!= 0)goto _LL146;_tmp19F=((struct Cyc_Absyn_Regparm_att_struct*)_tmp19E)->f1;
_tmp1A0=_tmp19D.f2;if(_tmp1A0 <= (void*)17)goto _LL146;if(*((int*)_tmp1A0)!= 0)
goto _LL146;_tmp1A1=((struct Cyc_Absyn_Regparm_att_struct*)_tmp1A0)->f1;_LL145:
_tmp1A3=_tmp19F;_tmp1A5=_tmp1A1;goto _LL147;_LL146: _tmp1A2=_tmp19D.f1;if(_tmp1A2
<= (void*)17)goto _LL148;if(*((int*)_tmp1A2)!= 4)goto _LL148;_tmp1A3=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1A2)->f1;_tmp1A4=_tmp19D.f2;if(_tmp1A4 <= (void*)17)goto _LL148;if(*((int*)
_tmp1A4)!= 4)goto _LL148;_tmp1A5=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1A4)->f1;_LL147: _tmp1A7=_tmp1A3;_tmp1A9=_tmp1A5;goto _LL149;_LL148: _tmp1A6=
_tmp19D.f1;if(_tmp1A6 <= (void*)17)goto _LL14A;if(*((int*)_tmp1A6)!= 1)goto _LL14A;
_tmp1A7=((struct Cyc_Absyn_Aligned_att_struct*)_tmp1A6)->f1;_tmp1A8=_tmp19D.f2;
if(_tmp1A8 <= (void*)17)goto _LL14A;if(*((int*)_tmp1A8)!= 1)goto _LL14A;_tmp1A9=((
struct Cyc_Absyn_Aligned_att_struct*)_tmp1A8)->f1;_LL149: return Cyc_Core_intcmp(
_tmp1A7,_tmp1A9);_LL14A: _tmp1AA=_tmp19D.f1;if(_tmp1AA <= (void*)17)goto _LL14C;if(*((
int*)_tmp1AA)!= 2)goto _LL14C;_tmp1AB=((struct Cyc_Absyn_Section_att_struct*)
_tmp1AA)->f1;_tmp1AC=_tmp19D.f2;if(_tmp1AC <= (void*)17)goto _LL14C;if(*((int*)
_tmp1AC)!= 2)goto _LL14C;_tmp1AD=((struct Cyc_Absyn_Section_att_struct*)_tmp1AC)->f1;
_LL14B: return Cyc_strcmp((struct _dynforward_ptr)_tmp1AB,(struct _dynforward_ptr)
_tmp1AD);_LL14C: _tmp1AE=_tmp19D.f1;if(_tmp1AE <= (void*)17)goto _LL14E;if(*((int*)
_tmp1AE)!= 3)goto _LL14E;_tmp1AF=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp1AE)->f1;_tmp1B0=((struct Cyc_Absyn_Format_att_struct*)_tmp1AE)->f2;_tmp1B1=((
struct Cyc_Absyn_Format_att_struct*)_tmp1AE)->f3;_tmp1B2=_tmp19D.f2;if(_tmp1B2 <= (
void*)17)goto _LL14E;if(*((int*)_tmp1B2)!= 3)goto _LL14E;_tmp1B3=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp1B2)->f1;_tmp1B4=((struct Cyc_Absyn_Format_att_struct*)
_tmp1B2)->f2;_tmp1B5=((struct Cyc_Absyn_Format_att_struct*)_tmp1B2)->f3;_LL14D: {
int _tmp1B6=Cyc_Core_intcmp((int)((unsigned int)_tmp1AF),(int)((unsigned int)
_tmp1B3));if(_tmp1B6 != 0)return _tmp1B6;{int _tmp1B7=Cyc_Core_intcmp(_tmp1B0,
_tmp1B4);if(_tmp1B7 != 0)return _tmp1B7;return Cyc_Core_intcmp(_tmp1B1,_tmp1B5);}}
_LL14E:;_LL14F: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(
att2));_LL143:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(
a1,a2)== 0;}int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*
a2){{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a2))return 0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(
Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(
void*t);static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return
Cyc_Tcutil_rgns_of((void*)af->type);}static struct Cyc_Absyn_TypeInt_struct Cyc_Tcutil_tizero={
18,0};static struct _tuple8*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;{void*_tmp1B9=Cyc_Tcutil_tvar_kind(tv);_LL151: if((int)_tmp1B9 != 3)goto
_LL153;_LL152: t=(void*)2;goto _LL150;_LL153: if((int)_tmp1B9 != 4)goto _LL155;_LL154:
t=Cyc_Absyn_empty_effect;goto _LL150;_LL155: if((int)_tmp1B9 != 5)goto _LL157;_LL156:
t=(void*)& Cyc_Tcutil_tizero;goto _LL150;_LL157:;_LL158: t=Cyc_Absyn_sint_typ;goto
_LL150;_LL150:;}return({struct _tuple8*_tmp1BA=_cycalloc(sizeof(*_tmp1BA));
_tmp1BA->f1=tv;_tmp1BA->f2=t;_tmp1BA;});}static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp1BB=Cyc_Tcutil_compress(t);void*_tmp1BC;struct Cyc_Absyn_TunionInfo
_tmp1BD;struct Cyc_List_List*_tmp1BE;struct Cyc_Core_Opt*_tmp1BF;struct Cyc_Absyn_PtrInfo
_tmp1C0;void*_tmp1C1;struct Cyc_Absyn_PtrAtts _tmp1C2;void*_tmp1C3;struct Cyc_Absyn_ArrayInfo
_tmp1C4;void*_tmp1C5;struct Cyc_List_List*_tmp1C6;struct Cyc_Absyn_TunionFieldInfo
_tmp1C7;struct Cyc_List_List*_tmp1C8;struct Cyc_Absyn_AggrInfo _tmp1C9;struct Cyc_List_List*
_tmp1CA;struct Cyc_List_List*_tmp1CB;void*_tmp1CC;struct Cyc_Absyn_FnInfo _tmp1CD;
struct Cyc_List_List*_tmp1CE;struct Cyc_Core_Opt*_tmp1CF;void*_tmp1D0;struct Cyc_List_List*
_tmp1D1;struct Cyc_Absyn_VarargInfo*_tmp1D2;struct Cyc_List_List*_tmp1D3;void*
_tmp1D4;struct Cyc_List_List*_tmp1D5;_LL15A: if((int)_tmp1BB != 0)goto _LL15C;_LL15B:
goto _LL15D;_LL15C: if((int)_tmp1BB != 1)goto _LL15E;_LL15D: goto _LL15F;_LL15E: if(
_tmp1BB <= (void*)4)goto _LL182;if(*((int*)_tmp1BB)!= 6)goto _LL160;_LL15F: goto
_LL161;_LL160: if(*((int*)_tmp1BB)!= 12)goto _LL162;_LL161: goto _LL163;_LL162: if(*((
int*)_tmp1BB)!= 13)goto _LL164;_LL163: goto _LL165;_LL164: if(*((int*)_tmp1BB)!= 18)
goto _LL166;_LL165: goto _LL167;_LL166: if(*((int*)_tmp1BB)!= 5)goto _LL168;_LL167:
return Cyc_Absyn_empty_effect;_LL168: if(*((int*)_tmp1BB)!= 0)goto _LL16A;_LL169:
goto _LL16B;_LL16A: if(*((int*)_tmp1BB)!= 1)goto _LL16C;_LL16B: {void*_tmp1D6=Cyc_Tcutil_typ_kind(
t);_LL18F: if((int)_tmp1D6 != 3)goto _LL191;_LL190: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1D8;_tmp1D8.tag=19;_tmp1D8.f1=(void*)t;_tmp1D8;});_tmp1D7;});_LL191: if((int)
_tmp1D6 != 4)goto _LL193;_LL192: return t;_LL193: if((int)_tmp1D6 != 5)goto _LL195;
_LL194: return Cyc_Absyn_empty_effect;_LL195:;_LL196: return(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp1DA;_tmp1DA.tag=21;_tmp1DA.f1=(void*)t;_tmp1DA;});_tmp1D9;});_LL18E:;}_LL16C:
if(*((int*)_tmp1BB)!= 15)goto _LL16E;_tmp1BC=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp1BB)->f1;_LL16D: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1DB=
_cycalloc(sizeof(*_tmp1DB));_tmp1DB[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1DC;
_tmp1DC.tag=19;_tmp1DC.f1=(void*)_tmp1BC;_tmp1DC;});_tmp1DB;});_LL16E: if(*((int*)
_tmp1BB)!= 2)goto _LL170;_tmp1BD=((struct Cyc_Absyn_TunionType_struct*)_tmp1BB)->f1;
_tmp1BE=_tmp1BD.targs;_tmp1BF=_tmp1BD.rgn;_LL16F: {struct Cyc_List_List*ts=Cyc_List_map(
Cyc_Tcutil_rgns_of,_tmp1BE);if((unsigned int)_tmp1BF)ts=({struct Cyc_List_List*
_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1DF;_tmp1DF.tag=19;_tmp1DF.f1=(void*)((void*)_tmp1BF->v);_tmp1DF;});_tmp1DE;}));
_tmp1DD->tl=ts;_tmp1DD;});return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1E1;_tmp1E1.tag=20;_tmp1E1.f1=ts;_tmp1E1;});_tmp1E0;}));}_LL170: if(*((int*)
_tmp1BB)!= 4)goto _LL172;_tmp1C0=((struct Cyc_Absyn_PointerType_struct*)_tmp1BB)->f1;
_tmp1C1=(void*)_tmp1C0.elt_typ;_tmp1C2=_tmp1C0.ptr_atts;_tmp1C3=(void*)_tmp1C2.rgn;
_LL171: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1E2=_cycalloc(sizeof(*_tmp1E2));_tmp1E2[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1E3;_tmp1E3.tag=20;_tmp1E3.f1=({void*_tmp1E4[2];_tmp1E4[1]=Cyc_Tcutil_rgns_of(
_tmp1C1);_tmp1E4[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1E5=_cycalloc(
sizeof(*_tmp1E5));_tmp1E5[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1E6;_tmp1E6.tag=
19;_tmp1E6.f1=(void*)_tmp1C3;_tmp1E6;});_tmp1E5;});Cyc_List_list(_tag_dynforward(
_tmp1E4,sizeof(void*),2));});_tmp1E3;});_tmp1E2;}));_LL172: if(*((int*)_tmp1BB)!= 
7)goto _LL174;_tmp1C4=((struct Cyc_Absyn_ArrayType_struct*)_tmp1BB)->f1;_tmp1C5=(
void*)_tmp1C4.elt_type;_LL173: return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(
_tmp1C5));_LL174: if(*((int*)_tmp1BB)!= 9)goto _LL176;_tmp1C6=((struct Cyc_Absyn_TupleType_struct*)
_tmp1BB)->f1;_LL175: {struct Cyc_List_List*_tmp1E7=0;for(0;_tmp1C6 != 0;_tmp1C6=
_tmp1C6->tl){_tmp1E7=({struct Cyc_List_List*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));
_tmp1E8->hd=(void*)(*((struct _tuple4*)_tmp1C6->hd)).f2;_tmp1E8->tl=_tmp1E7;
_tmp1E8;});}_tmp1C8=_tmp1E7;goto _LL177;}_LL176: if(*((int*)_tmp1BB)!= 3)goto
_LL178;_tmp1C7=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1BB)->f1;_tmp1C8=
_tmp1C7.targs;_LL177: _tmp1CA=_tmp1C8;goto _LL179;_LL178: if(*((int*)_tmp1BB)!= 10)
goto _LL17A;_tmp1C9=((struct Cyc_Absyn_AggrType_struct*)_tmp1BB)->f1;_tmp1CA=
_tmp1C9.targs;_LL179: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1EA;_tmp1EA.tag=20;_tmp1EA.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1CA);
_tmp1EA;});_tmp1E9;}));_LL17A: if(*((int*)_tmp1BB)!= 11)goto _LL17C;_tmp1CB=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp1BB)->f2;_LL17B: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));
_tmp1EB[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1EC;_tmp1EC.tag=20;_tmp1EC.f1=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1CB);_tmp1EC;});_tmp1EB;}));_LL17C:
if(*((int*)_tmp1BB)!= 14)goto _LL17E;_tmp1CC=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp1BB)->f1;_LL17D: return Cyc_Tcutil_rgns_of(_tmp1CC);_LL17E: if(*((int*)_tmp1BB)
!= 17)goto _LL180;_LL17F: return Cyc_Absyn_empty_effect;_LL180: if(*((int*)_tmp1BB)
!= 8)goto _LL182;_tmp1CD=((struct Cyc_Absyn_FnType_struct*)_tmp1BB)->f1;_tmp1CE=
_tmp1CD.tvars;_tmp1CF=_tmp1CD.effect;_tmp1D0=(void*)_tmp1CD.ret_typ;_tmp1D1=
_tmp1CD.args;_tmp1D2=_tmp1CD.cyc_varargs;_tmp1D3=_tmp1CD.rgn_po;_LL181: {void*
_tmp1ED=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp1CE),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1CF))->v);return Cyc_Tcutil_normalize_effect(
_tmp1ED);}_LL182: if((int)_tmp1BB != 3)goto _LL184;_LL183: goto _LL185;_LL184: if((int)
_tmp1BB != 2)goto _LL186;_LL185: return Cyc_Absyn_empty_effect;_LL186: if(_tmp1BB <= (
void*)4)goto _LL188;if(*((int*)_tmp1BB)!= 19)goto _LL188;_LL187: goto _LL189;_LL188:
if(_tmp1BB <= (void*)4)goto _LL18A;if(*((int*)_tmp1BB)!= 20)goto _LL18A;_LL189:
return t;_LL18A: if(_tmp1BB <= (void*)4)goto _LL18C;if(*((int*)_tmp1BB)!= 21)goto
_LL18C;_tmp1D4=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1BB)->f1;_LL18B:
return Cyc_Tcutil_rgns_of(_tmp1D4);_LL18C: if(_tmp1BB <= (void*)4)goto _LL159;if(*((
int*)_tmp1BB)!= 16)goto _LL159;_tmp1D5=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1BB)->f2;_LL18D: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1EF;_tmp1EF.tag=20;_tmp1EF.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1D5);
_tmp1EF;});_tmp1EE;}));_LL159:;}void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp1F0=e;struct Cyc_List_List*_tmp1F1;struct Cyc_List_List**_tmp1F2;void*
_tmp1F3;_LL198: if(_tmp1F0 <= (void*)4)goto _LL19C;if(*((int*)_tmp1F0)!= 20)goto
_LL19A;_tmp1F1=((struct Cyc_Absyn_JoinEff_struct*)_tmp1F0)->f1;_tmp1F2=(struct Cyc_List_List**)&((
struct Cyc_Absyn_JoinEff_struct*)_tmp1F0)->f1;_LL199: {int redo_join=0;{struct Cyc_List_List*
effs=*_tmp1F2;for(0;effs != 0;effs=effs->tl){void*_tmp1F4=(void*)effs->hd;(void*)(
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp1F4)));{void*
_tmp1F5=(void*)effs->hd;void*_tmp1F6;_LL19F: if(_tmp1F5 <= (void*)4)goto _LL1A3;if(*((
int*)_tmp1F5)!= 20)goto _LL1A1;_LL1A0: goto _LL1A2;_LL1A1: if(*((int*)_tmp1F5)!= 19)
goto _LL1A3;_tmp1F6=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1F5)->f1;if((
int)_tmp1F6 != 2)goto _LL1A3;_LL1A2: redo_join=1;goto _LL19E;_LL1A3:;_LL1A4: goto
_LL19E;_LL19E:;}}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct
Cyc_List_List*effs=*_tmp1F2;for(0;effs != 0;effs=effs->tl){void*_tmp1F7=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp1F8;void*_tmp1F9;_LL1A6: if(_tmp1F7 <= (
void*)4)goto _LL1AA;if(*((int*)_tmp1F7)!= 20)goto _LL1A8;_tmp1F8=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1F7)->f1;_LL1A7: effects=Cyc_List_revappend(_tmp1F8,effects);goto _LL1A5;
_LL1A8: if(*((int*)_tmp1F7)!= 19)goto _LL1AA;_tmp1F9=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1F7)->f1;if((int)_tmp1F9 != 2)goto _LL1AA;_LL1A9: goto _LL1A5;_LL1AA:;_LL1AB:
effects=({struct Cyc_List_List*_tmp1FA=_cycalloc(sizeof(*_tmp1FA));_tmp1FA->hd=(
void*)_tmp1F7;_tmp1FA->tl=effects;_tmp1FA;});goto _LL1A5;_LL1A5:;}}*_tmp1F2=Cyc_List_imp_rev(
effects);return e;}}_LL19A: if(*((int*)_tmp1F0)!= 21)goto _LL19C;_tmp1F3=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1F0)->f1;_LL19B: {void*_tmp1FB=Cyc_Tcutil_compress(
_tmp1F3);_LL1AD: if(_tmp1FB <= (void*)4)goto _LL1B1;if(*((int*)_tmp1FB)!= 0)goto
_LL1AF;_LL1AE: goto _LL1B0;_LL1AF: if(*((int*)_tmp1FB)!= 1)goto _LL1B1;_LL1B0: return
e;_LL1B1:;_LL1B2: return Cyc_Tcutil_rgns_of(_tmp1F3);_LL1AC:;}_LL19C:;_LL19D:
return e;_LL197:;}}static struct Cyc_Core_Opt Cyc_Tcutil_ek={(void*)((void*)4)};
static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct*
_tmp1FC=({struct Cyc_Absyn_FnType_struct*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));
_tmp1FD[0]=({struct Cyc_Absyn_FnType_struct _tmp1FE;_tmp1FE.tag=8;_tmp1FE.f1=({
struct Cyc_Absyn_FnInfo _tmp1FF;_tmp1FF.tvars=0;_tmp1FF.effect=({struct Cyc_Core_Opt*
_tmp200=_cycalloc(sizeof(*_tmp200));_tmp200->v=(void*)eff;_tmp200;});_tmp1FF.ret_typ=(
void*)((void*)0);_tmp1FF.args=0;_tmp1FF.c_varargs=0;_tmp1FF.cyc_varargs=0;
_tmp1FF.rgn_po=0;_tmp1FF.attributes=0;_tmp1FF;});_tmp1FE;});_tmp1FD;});return Cyc_Absyn_atb_typ((
void*)_tmp1FC,(void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){r=Cyc_Tcutil_compress(
r);if(r == (void*)2)return 1;{void*_tmp201=Cyc_Tcutil_compress(e);void*_tmp202;
struct Cyc_List_List*_tmp203;void*_tmp204;struct Cyc_Core_Opt*_tmp205;struct Cyc_Core_Opt*
_tmp206;struct Cyc_Core_Opt**_tmp207;struct Cyc_Core_Opt*_tmp208;_LL1B4: if(_tmp201
<= (void*)4)goto _LL1BC;if(*((int*)_tmp201)!= 19)goto _LL1B6;_tmp202=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp201)->f1;_LL1B5: if(constrain)return Cyc_Tcutil_unify(
r,_tmp202);_tmp202=Cyc_Tcutil_compress(_tmp202);if(r == _tmp202)return 1;{struct
_tuple6 _tmp20A=({struct _tuple6 _tmp209;_tmp209.f1=r;_tmp209.f2=_tmp202;_tmp209;});
void*_tmp20B;struct Cyc_Absyn_Tvar*_tmp20C;void*_tmp20D;struct Cyc_Absyn_Tvar*
_tmp20E;_LL1BF: _tmp20B=_tmp20A.f1;if(_tmp20B <= (void*)4)goto _LL1C1;if(*((int*)
_tmp20B)!= 1)goto _LL1C1;_tmp20C=((struct Cyc_Absyn_VarType_struct*)_tmp20B)->f1;
_tmp20D=_tmp20A.f2;if(_tmp20D <= (void*)4)goto _LL1C1;if(*((int*)_tmp20D)!= 1)goto
_LL1C1;_tmp20E=((struct Cyc_Absyn_VarType_struct*)_tmp20D)->f1;_LL1C0: return Cyc_Absyn_tvar_cmp(
_tmp20C,_tmp20E)== 0;_LL1C1:;_LL1C2: return 0;_LL1BE:;}_LL1B6: if(*((int*)_tmp201)
!= 20)goto _LL1B8;_tmp203=((struct Cyc_Absyn_JoinEff_struct*)_tmp201)->f1;_LL1B7:
for(0;_tmp203 != 0;_tmp203=_tmp203->tl){if(Cyc_Tcutil_region_in_effect(constrain,
r,(void*)_tmp203->hd))return 1;}return 0;_LL1B8: if(*((int*)_tmp201)!= 21)goto
_LL1BA;_tmp204=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp201)->f1;_LL1B9: {
void*_tmp20F=Cyc_Tcutil_rgns_of(_tmp204);void*_tmp210;_LL1C4: if(_tmp20F <= (void*)
4)goto _LL1C6;if(*((int*)_tmp20F)!= 21)goto _LL1C6;_tmp210=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp20F)->f1;_LL1C5: if(!constrain)return 0;{void*_tmp211=Cyc_Tcutil_compress(
_tmp210);struct Cyc_Core_Opt*_tmp212;struct Cyc_Core_Opt*_tmp213;struct Cyc_Core_Opt**
_tmp214;struct Cyc_Core_Opt*_tmp215;_LL1C9: if(_tmp211 <= (void*)4)goto _LL1CB;if(*((
int*)_tmp211)!= 0)goto _LL1CB;_tmp212=((struct Cyc_Absyn_Evar_struct*)_tmp211)->f1;
_tmp213=((struct Cyc_Absyn_Evar_struct*)_tmp211)->f2;_tmp214=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp211)->f2;_tmp215=((struct Cyc_Absyn_Evar_struct*)
_tmp211)->f4;_LL1CA: {void*_tmp216=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp215);Cyc_Tcutil_occurs(_tmp216,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp215))->v,r);{void*_tmp217=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp219=_cycalloc(sizeof(*_tmp219));
_tmp219[0]=({struct Cyc_Absyn_JoinEff_struct _tmp21A;_tmp21A.tag=20;_tmp21A.f1=({
void*_tmp21B[2];_tmp21B[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp21C=
_cycalloc(sizeof(*_tmp21C));_tmp21C[0]=({struct Cyc_Absyn_AccessEff_struct _tmp21D;
_tmp21D.tag=19;_tmp21D.f1=(void*)r;_tmp21D;});_tmp21C;});_tmp21B[0]=_tmp216;Cyc_List_list(
_tag_dynforward(_tmp21B,sizeof(void*),2));});_tmp21A;});_tmp219;}));*_tmp214=({
struct Cyc_Core_Opt*_tmp218=_cycalloc(sizeof(*_tmp218));_tmp218->v=(void*)_tmp217;
_tmp218;});return 1;}}_LL1CB:;_LL1CC: return 0;_LL1C8:;}_LL1C6:;_LL1C7: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp20F);_LL1C3:;}_LL1BA: if(*((int*)_tmp201)!= 0)goto _LL1BC;_tmp205=((
struct Cyc_Absyn_Evar_struct*)_tmp201)->f1;_tmp206=((struct Cyc_Absyn_Evar_struct*)
_tmp201)->f2;_tmp207=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp201)->f2;_tmp208=((struct Cyc_Absyn_Evar_struct*)_tmp201)->f4;_LL1BB: if(
_tmp205 == 0  || (void*)_tmp205->v != (void*)4)({void*_tmp21E[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp21F="effect evar has wrong kind";_tag_dynforward(_tmp21F,sizeof(char),
_get_zero_arr_size(_tmp21F,27));}),_tag_dynforward(_tmp21E,sizeof(void*),0));});
if(!constrain)return 0;{void*_tmp220=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp208);Cyc_Tcutil_occurs(_tmp220,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp208))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp221=({struct Cyc_Absyn_JoinEff_struct*_tmp223=_cycalloc(sizeof(*_tmp223));
_tmp223[0]=({struct Cyc_Absyn_JoinEff_struct _tmp224;_tmp224.tag=20;_tmp224.f1=({
struct Cyc_List_List*_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225->hd=(void*)
_tmp220;_tmp225->tl=({struct Cyc_List_List*_tmp226=_cycalloc(sizeof(*_tmp226));
_tmp226->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp227=_cycalloc(
sizeof(*_tmp227));_tmp227[0]=({struct Cyc_Absyn_AccessEff_struct _tmp228;_tmp228.tag=
19;_tmp228.f1=(void*)r;_tmp228;});_tmp227;}));_tmp226->tl=0;_tmp226;});_tmp225;});
_tmp224;});_tmp223;});*_tmp207=({struct Cyc_Core_Opt*_tmp222=_cycalloc(sizeof(*
_tmp222));_tmp222->v=(void*)((void*)_tmp221);_tmp222;});return 1;}}_LL1BC:;_LL1BD:
return 0;_LL1B3:;}}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*
t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp229=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(
e));struct Cyc_List_List*_tmp22A;void*_tmp22B;struct Cyc_Core_Opt*_tmp22C;struct
Cyc_Core_Opt*_tmp22D;struct Cyc_Core_Opt**_tmp22E;struct Cyc_Core_Opt*_tmp22F;
_LL1CE: if(_tmp229 <= (void*)4)goto _LL1D6;if(*((int*)_tmp229)!= 19)goto _LL1D0;
_LL1CF: return 0;_LL1D0: if(*((int*)_tmp229)!= 20)goto _LL1D2;_tmp22A=((struct Cyc_Absyn_JoinEff_struct*)
_tmp229)->f1;_LL1D1: for(0;_tmp22A != 0;_tmp22A=_tmp22A->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp22A->hd))return 1;}return 0;_LL1D2: if(*((int*)
_tmp229)!= 21)goto _LL1D4;_tmp22B=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp229)->f1;_LL1D3: _tmp22B=Cyc_Tcutil_compress(_tmp22B);if(t == _tmp22B)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp22B);{void*_tmp230=Cyc_Tcutil_rgns_of(
t);void*_tmp231;_LL1D9: if(_tmp230 <= (void*)4)goto _LL1DB;if(*((int*)_tmp230)!= 21)
goto _LL1DB;_tmp231=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp230)->f1;_LL1DA: {
struct _tuple6 _tmp233=({struct _tuple6 _tmp232;_tmp232.f1=Cyc_Tcutil_compress(
_tmp231);_tmp232.f2=_tmp22B;_tmp232;});void*_tmp234;struct Cyc_Absyn_Tvar*_tmp235;
void*_tmp236;struct Cyc_Absyn_Tvar*_tmp237;_LL1DE: _tmp234=_tmp233.f1;if(_tmp234 <= (
void*)4)goto _LL1E0;if(*((int*)_tmp234)!= 1)goto _LL1E0;_tmp235=((struct Cyc_Absyn_VarType_struct*)
_tmp234)->f1;_tmp236=_tmp233.f2;if(_tmp236 <= (void*)4)goto _LL1E0;if(*((int*)
_tmp236)!= 1)goto _LL1E0;_tmp237=((struct Cyc_Absyn_VarType_struct*)_tmp236)->f1;
_LL1DF: return Cyc_Tcutil_unify(t,_tmp22B);_LL1E0:;_LL1E1: return _tmp231 == _tmp22B;
_LL1DD:;}_LL1DB:;_LL1DC: return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,
_tmp230);_LL1D8:;}_LL1D4: if(*((int*)_tmp229)!= 0)goto _LL1D6;_tmp22C=((struct Cyc_Absyn_Evar_struct*)
_tmp229)->f1;_tmp22D=((struct Cyc_Absyn_Evar_struct*)_tmp229)->f2;_tmp22E=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp229)->f2;_tmp22F=((struct Cyc_Absyn_Evar_struct*)
_tmp229)->f4;_LL1D5: if(_tmp22C == 0  || (void*)_tmp22C->v != (void*)4)({void*
_tmp238[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp239="effect evar has wrong kind";_tag_dynforward(_tmp239,sizeof(
char),_get_zero_arr_size(_tmp239,27));}),_tag_dynforward(_tmp238,sizeof(void*),0));});
if(!may_constrain_evars)return 0;{void*_tmp23A=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp22F);Cyc_Tcutil_occurs(_tmp23A,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp22F))->v,t);{struct Cyc_Absyn_JoinEff_struct*
_tmp23B=({struct Cyc_Absyn_JoinEff_struct*_tmp23D=_cycalloc(sizeof(*_tmp23D));
_tmp23D[0]=({struct Cyc_Absyn_JoinEff_struct _tmp23E;_tmp23E.tag=20;_tmp23E.f1=({
struct Cyc_List_List*_tmp23F=_cycalloc(sizeof(*_tmp23F));_tmp23F->hd=(void*)
_tmp23A;_tmp23F->tl=({struct Cyc_List_List*_tmp240=_cycalloc(sizeof(*_tmp240));
_tmp240->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp241=_cycalloc(
sizeof(*_tmp241));_tmp241[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp242;_tmp242.tag=
21;_tmp242.f1=(void*)t;_tmp242;});_tmp241;}));_tmp240->tl=0;_tmp240;});_tmp23F;});
_tmp23E;});_tmp23D;});*_tmp22E=({struct Cyc_Core_Opt*_tmp23C=_cycalloc(sizeof(*
_tmp23C));_tmp23C->v=(void*)((void*)_tmp23B);_tmp23C;});return 1;}}_LL1D6:;_LL1D7:
return 0;_LL1CD:;}}static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,
struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp243=e;struct
Cyc_Absyn_Tvar*_tmp244;struct Cyc_List_List*_tmp245;void*_tmp246;struct Cyc_Core_Opt*
_tmp247;struct Cyc_Core_Opt*_tmp248;struct Cyc_Core_Opt**_tmp249;struct Cyc_Core_Opt*
_tmp24A;_LL1E3: if(_tmp243 <= (void*)4)goto _LL1EB;if(*((int*)_tmp243)!= 1)goto
_LL1E5;_tmp244=((struct Cyc_Absyn_VarType_struct*)_tmp243)->f1;_LL1E4: return Cyc_Absyn_tvar_cmp(
v,_tmp244)== 0;_LL1E5: if(*((int*)_tmp243)!= 20)goto _LL1E7;_tmp245=((struct Cyc_Absyn_JoinEff_struct*)
_tmp243)->f1;_LL1E6: for(0;_tmp245 != 0;_tmp245=_tmp245->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp245->hd))return 1;}return 0;_LL1E7: if(*((int*)
_tmp243)!= 21)goto _LL1E9;_tmp246=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp243)->f1;_LL1E8: {void*_tmp24B=Cyc_Tcutil_rgns_of(_tmp246);void*_tmp24C;
_LL1EE: if(_tmp24B <= (void*)4)goto _LL1F0;if(*((int*)_tmp24B)!= 21)goto _LL1F0;
_tmp24C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp24B)->f1;_LL1EF: if(!
may_constrain_evars)return 0;{void*_tmp24D=Cyc_Tcutil_compress(_tmp24C);struct Cyc_Core_Opt*
_tmp24E;struct Cyc_Core_Opt*_tmp24F;struct Cyc_Core_Opt**_tmp250;struct Cyc_Core_Opt*
_tmp251;_LL1F3: if(_tmp24D <= (void*)4)goto _LL1F5;if(*((int*)_tmp24D)!= 0)goto
_LL1F5;_tmp24E=((struct Cyc_Absyn_Evar_struct*)_tmp24D)->f1;_tmp24F=((struct Cyc_Absyn_Evar_struct*)
_tmp24D)->f2;_tmp250=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp24D)->f2;_tmp251=((struct Cyc_Absyn_Evar_struct*)_tmp24D)->f4;_LL1F4: {void*
_tmp252=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp251);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp251))->v,v))return 0;{void*_tmp253=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp255=_cycalloc(sizeof(*_tmp255));
_tmp255[0]=({struct Cyc_Absyn_JoinEff_struct _tmp256;_tmp256.tag=20;_tmp256.f1=({
void*_tmp257[2];_tmp257[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp258=
_cycalloc(sizeof(*_tmp258));_tmp258[0]=({struct Cyc_Absyn_VarType_struct _tmp259;
_tmp259.tag=1;_tmp259.f1=v;_tmp259;});_tmp258;});_tmp257[0]=_tmp252;Cyc_List_list(
_tag_dynforward(_tmp257,sizeof(void*),2));});_tmp256;});_tmp255;}));*_tmp250=({
struct Cyc_Core_Opt*_tmp254=_cycalloc(sizeof(*_tmp254));_tmp254->v=(void*)_tmp253;
_tmp254;});return 1;}}_LL1F5:;_LL1F6: return 0;_LL1F2:;}_LL1F0:;_LL1F1: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp24B);_LL1ED:;}_LL1E9: if(*((int*)_tmp243)!= 0)goto
_LL1EB;_tmp247=((struct Cyc_Absyn_Evar_struct*)_tmp243)->f1;_tmp248=((struct Cyc_Absyn_Evar_struct*)
_tmp243)->f2;_tmp249=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp243)->f2;_tmp24A=((struct Cyc_Absyn_Evar_struct*)_tmp243)->f4;_LL1EA: if(
_tmp247 == 0  || (void*)_tmp247->v != (void*)4)({void*_tmp25A[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp25B="effect evar has wrong kind";_tag_dynforward(_tmp25B,sizeof(char),
_get_zero_arr_size(_tmp25B,27));}),_tag_dynforward(_tmp25A,sizeof(void*),0));});{
void*_tmp25C=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp24A);if(
!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp24A))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct*
_tmp25D=({struct Cyc_Absyn_JoinEff_struct*_tmp25F=_cycalloc(sizeof(*_tmp25F));
_tmp25F[0]=({struct Cyc_Absyn_JoinEff_struct _tmp260;_tmp260.tag=20;_tmp260.f1=({
struct Cyc_List_List*_tmp261=_cycalloc(sizeof(*_tmp261));_tmp261->hd=(void*)
_tmp25C;_tmp261->tl=({struct Cyc_List_List*_tmp262=_cycalloc(sizeof(*_tmp262));
_tmp262->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp263=_cycalloc(
sizeof(*_tmp263));_tmp263[0]=({struct Cyc_Absyn_VarType_struct _tmp264;_tmp264.tag=
1;_tmp264.f1=v;_tmp264;});_tmp263;}));_tmp262->tl=0;_tmp262;});_tmp261;});
_tmp260;});_tmp25F;});*_tmp249=({struct Cyc_Core_Opt*_tmp25E=_cycalloc(sizeof(*
_tmp25E));_tmp25E->v=(void*)((void*)_tmp25D);_tmp25E;});return 1;}}_LL1EB:;_LL1EC:
return 0;_LL1E2:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp265=e;struct Cyc_List_List*_tmp266;void*_tmp267;_LL1F8: if(_tmp265 <= (
void*)4)goto _LL1FE;if(*((int*)_tmp265)!= 20)goto _LL1FA;_tmp266=((struct Cyc_Absyn_JoinEff_struct*)
_tmp265)->f1;_LL1F9: for(0;_tmp266 != 0;_tmp266=_tmp266->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp266->hd))return 1;}return 0;_LL1FA: if(*((int*)_tmp265)!= 21)goto
_LL1FC;_tmp267=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp265)->f1;_LL1FB: {
void*_tmp268=Cyc_Tcutil_rgns_of(_tmp267);void*_tmp269;_LL201: if(_tmp268 <= (void*)
4)goto _LL203;if(*((int*)_tmp268)!= 21)goto _LL203;_tmp269=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp268)->f1;_LL202: return 0;_LL203:;_LL204: return Cyc_Tcutil_evar_in_effect(evar,
_tmp268);_LL200:;}_LL1FC: if(*((int*)_tmp265)!= 0)goto _LL1FE;_LL1FD: return evar == 
e;_LL1FE:;_LL1FF: return 0;_LL1F7:;}}int Cyc_Tcutil_subset_effect(int
may_constrain_evars,void*e1,void*e2){void*_tmp26A=Cyc_Tcutil_compress(e1);struct
Cyc_List_List*_tmp26B;void*_tmp26C;struct Cyc_Absyn_Tvar*_tmp26D;void*_tmp26E;
struct Cyc_Core_Opt*_tmp26F;struct Cyc_Core_Opt**_tmp270;struct Cyc_Core_Opt*
_tmp271;_LL206: if(_tmp26A <= (void*)4)goto _LL210;if(*((int*)_tmp26A)!= 20)goto
_LL208;_tmp26B=((struct Cyc_Absyn_JoinEff_struct*)_tmp26A)->f1;_LL207: for(0;
_tmp26B != 0;_tmp26B=_tmp26B->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp26B->hd,e2))return 0;}return 1;_LL208: if(*((int*)
_tmp26A)!= 19)goto _LL20A;_tmp26C=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp26A)->f1;_LL209: return Cyc_Tcutil_region_in_effect(0,_tmp26C,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp26C,(void*)2);_LL20A: if(*((int*)
_tmp26A)!= 1)goto _LL20C;_tmp26D=((struct Cyc_Absyn_VarType_struct*)_tmp26A)->f1;
_LL20B: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp26D,e2);
_LL20C: if(*((int*)_tmp26A)!= 21)goto _LL20E;_tmp26E=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp26A)->f1;_LL20D: {void*_tmp272=Cyc_Tcutil_rgns_of(_tmp26E);void*_tmp273;
_LL213: if(_tmp272 <= (void*)4)goto _LL215;if(*((int*)_tmp272)!= 21)goto _LL215;
_tmp273=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp272)->f1;_LL214: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,_tmp273,e2) || may_constrain_evars  && Cyc_Tcutil_unify(
_tmp273,Cyc_Absyn_sint_typ);_LL215:;_LL216: return Cyc_Tcutil_subset_effect(
may_constrain_evars,_tmp272,e2);_LL212:;}_LL20E: if(*((int*)_tmp26A)!= 0)goto
_LL210;_tmp26F=((struct Cyc_Absyn_Evar_struct*)_tmp26A)->f2;_tmp270=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp26A)->f2;_tmp271=((struct Cyc_Absyn_Evar_struct*)
_tmp26A)->f4;_LL20F: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*_tmp270=({struct Cyc_Core_Opt*
_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274->v=(void*)Cyc_Absyn_empty_effect;
_tmp274;});return 1;_LL210:;_LL211:({struct Cyc_String_pa_struct _tmp277;_tmp277.tag=
0;_tmp277.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
e1));{void*_tmp275[1]={& _tmp277};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp276="subset_effect: bad effect: %s";
_tag_dynforward(_tmp276,sizeof(char),_get_zero_arr_size(_tmp276,30));}),
_tag_dynforward(_tmp275,sizeof(void*),1));}});_LL205:;}static int Cyc_Tcutil_unify_effect(
void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(e1);e2=Cyc_Tcutil_normalize_effect(
e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct
Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){struct
_tuple6 _tmp279;void*_tmp27A;void*_tmp27B;struct _tuple6*_tmp278=(struct _tuple6*)
r1->hd;_tmp279=*_tmp278;_tmp27A=_tmp279.f1;_tmp27B=_tmp279.f2;{int found=_tmp27A
== (void*)2;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple6 _tmp27D;void*_tmp27E;void*_tmp27F;struct _tuple6*_tmp27C=(struct
_tuple6*)r2->hd;_tmp27D=*_tmp27C;_tmp27E=_tmp27D.f1;_tmp27F=_tmp27D.f2;if(Cyc_Tcutil_unify(
_tmp27A,_tmp27E) && Cyc_Tcutil_unify(_tmp27B,_tmp27F)){found=1;break;}}}if(!
found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*
rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(
rpo2,rpo1);}struct _tuple11{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*
f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _dynforward_ptr)_tag_dynforward(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp280=t1;struct Cyc_Core_Opt*_tmp281;struct Cyc_Core_Opt*
_tmp282;struct Cyc_Core_Opt**_tmp283;struct Cyc_Core_Opt*_tmp284;_LL218: if(_tmp280
<= (void*)4)goto _LL21A;if(*((int*)_tmp280)!= 0)goto _LL21A;_tmp281=((struct Cyc_Absyn_Evar_struct*)
_tmp280)->f1;_tmp282=((struct Cyc_Absyn_Evar_struct*)_tmp280)->f2;_tmp283=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp280)->f2;_tmp284=((struct Cyc_Absyn_Evar_struct*)
_tmp280)->f4;_LL219: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp284))->v,t2);{void*_tmp285=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp285,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp281))->v)){*_tmp283=({struct Cyc_Core_Opt*_tmp286=_cycalloc(sizeof(*_tmp286));
_tmp286->v=(void*)t2;_tmp286;});return;}else{{void*_tmp287=t2;struct Cyc_Core_Opt*
_tmp288;struct Cyc_Core_Opt**_tmp289;struct Cyc_Core_Opt*_tmp28A;struct Cyc_Absyn_PtrInfo
_tmp28B;_LL21D: if(_tmp287 <= (void*)4)goto _LL221;if(*((int*)_tmp287)!= 0)goto
_LL21F;_tmp288=((struct Cyc_Absyn_Evar_struct*)_tmp287)->f2;_tmp289=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp287)->f2;_tmp28A=((struct Cyc_Absyn_Evar_struct*)
_tmp287)->f4;_LL21E: {struct Cyc_List_List*_tmp28C=(struct Cyc_List_List*)_tmp284->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp28A))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp28C,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=({
const char*_tmp28D="(type variable would escape scope)";_tag_dynforward(_tmp28D,
sizeof(char),_get_zero_arr_size(_tmp28D,35));});(int)_throw((void*)Cyc_Tcutil_Unify);}}}
if(Cyc_Tcutil_kind_leq((void*)_tmp281->v,_tmp285)){*_tmp289=({struct Cyc_Core_Opt*
_tmp28E=_cycalloc(sizeof(*_tmp28E));_tmp28E->v=(void*)t1;_tmp28E;});return;}Cyc_Tcutil_failure_reason=({
const char*_tmp28F="(kinds are incompatible)";_tag_dynforward(_tmp28F,sizeof(char),
_get_zero_arr_size(_tmp28F,25));});goto _LL21C;}_LL21F: if(*((int*)_tmp287)!= 4)
goto _LL221;_tmp28B=((struct Cyc_Absyn_PointerType_struct*)_tmp287)->f1;if(!((void*)
_tmp281->v == (void*)2))goto _LL221;_LL220: {struct Cyc_Absyn_Conref*_tmp290=Cyc_Absyn_compress_conref((
_tmp28B.ptr_atts).bounds);{union Cyc_Absyn_Constraint_union _tmp291=_tmp290->v;
_LL224: if((_tmp291.No_constr).tag != 2)goto _LL226;_LL225: _tmp290->v=(union Cyc_Absyn_Constraint_union)({
union Cyc_Absyn_Constraint_union _tmp292;(_tmp292.Eq_constr).tag=0;(_tmp292.Eq_constr).f1=(
void*)Cyc_Absyn_bounds_one;_tmp292;});*_tmp283=({struct Cyc_Core_Opt*_tmp293=
_cycalloc(sizeof(*_tmp293));_tmp293->v=(void*)t2;_tmp293;});return;_LL226:;
_LL227: goto _LL223;_LL223:;}goto _LL21C;}_LL221:;_LL222: goto _LL21C;_LL21C:;}Cyc_Tcutil_failure_reason=({
const char*_tmp294="(kinds are incompatible)";_tag_dynforward(_tmp294,sizeof(char),
_get_zero_arr_size(_tmp294,25));});(int)_throw((void*)Cyc_Tcutil_Unify);}}_LL21A:;
_LL21B: goto _LL217;_LL217:;}{struct _tuple6 _tmp296=({struct _tuple6 _tmp295;_tmp295.f1=
t2;_tmp295.f2=t1;_tmp295;});void*_tmp297;void*_tmp298;void*_tmp299;void*_tmp29A;
struct Cyc_Absyn_Tvar*_tmp29B;void*_tmp29C;struct Cyc_Absyn_Tvar*_tmp29D;void*
_tmp29E;struct Cyc_Absyn_AggrInfo _tmp29F;union Cyc_Absyn_AggrInfoU_union _tmp2A0;
struct Cyc_List_List*_tmp2A1;void*_tmp2A2;struct Cyc_Absyn_AggrInfo _tmp2A3;union
Cyc_Absyn_AggrInfoU_union _tmp2A4;struct Cyc_List_List*_tmp2A5;void*_tmp2A6;struct
_tuple1*_tmp2A7;void*_tmp2A8;struct _tuple1*_tmp2A9;void*_tmp2AA;struct Cyc_List_List*
_tmp2AB;void*_tmp2AC;struct Cyc_List_List*_tmp2AD;void*_tmp2AE;struct Cyc_Absyn_TunionInfo
_tmp2AF;union Cyc_Absyn_TunionInfoU_union _tmp2B0;struct Cyc_Absyn_Tuniondecl**
_tmp2B1;struct Cyc_Absyn_Tuniondecl*_tmp2B2;struct Cyc_List_List*_tmp2B3;struct Cyc_Core_Opt*
_tmp2B4;void*_tmp2B5;struct Cyc_Absyn_TunionInfo _tmp2B6;union Cyc_Absyn_TunionInfoU_union
_tmp2B7;struct Cyc_Absyn_Tuniondecl**_tmp2B8;struct Cyc_Absyn_Tuniondecl*_tmp2B9;
struct Cyc_List_List*_tmp2BA;struct Cyc_Core_Opt*_tmp2BB;void*_tmp2BC;struct Cyc_Absyn_TunionFieldInfo
_tmp2BD;union Cyc_Absyn_TunionFieldInfoU_union _tmp2BE;struct Cyc_Absyn_Tuniondecl*
_tmp2BF;struct Cyc_Absyn_Tunionfield*_tmp2C0;struct Cyc_List_List*_tmp2C1;void*
_tmp2C2;struct Cyc_Absyn_TunionFieldInfo _tmp2C3;union Cyc_Absyn_TunionFieldInfoU_union
_tmp2C4;struct Cyc_Absyn_Tuniondecl*_tmp2C5;struct Cyc_Absyn_Tunionfield*_tmp2C6;
struct Cyc_List_List*_tmp2C7;void*_tmp2C8;struct Cyc_Absyn_PtrInfo _tmp2C9;void*
_tmp2CA;struct Cyc_Absyn_Tqual _tmp2CB;struct Cyc_Absyn_PtrAtts _tmp2CC;void*_tmp2CD;
struct Cyc_Absyn_Conref*_tmp2CE;struct Cyc_Absyn_Conref*_tmp2CF;struct Cyc_Absyn_Conref*
_tmp2D0;void*_tmp2D1;struct Cyc_Absyn_PtrInfo _tmp2D2;void*_tmp2D3;struct Cyc_Absyn_Tqual
_tmp2D4;struct Cyc_Absyn_PtrAtts _tmp2D5;void*_tmp2D6;struct Cyc_Absyn_Conref*
_tmp2D7;struct Cyc_Absyn_Conref*_tmp2D8;struct Cyc_Absyn_Conref*_tmp2D9;void*
_tmp2DA;void*_tmp2DB;void*_tmp2DC;void*_tmp2DD;void*_tmp2DE;void*_tmp2DF;void*
_tmp2E0;void*_tmp2E1;void*_tmp2E2;int _tmp2E3;void*_tmp2E4;int _tmp2E5;void*
_tmp2E6;void*_tmp2E7;void*_tmp2E8;void*_tmp2E9;void*_tmp2EA;int _tmp2EB;void*
_tmp2EC;int _tmp2ED;void*_tmp2EE;void*_tmp2EF;void*_tmp2F0;void*_tmp2F1;void*
_tmp2F2;struct Cyc_Absyn_ArrayInfo _tmp2F3;void*_tmp2F4;struct Cyc_Absyn_Tqual
_tmp2F5;struct Cyc_Absyn_Exp*_tmp2F6;struct Cyc_Absyn_Conref*_tmp2F7;void*_tmp2F8;
struct Cyc_Absyn_ArrayInfo _tmp2F9;void*_tmp2FA;struct Cyc_Absyn_Tqual _tmp2FB;
struct Cyc_Absyn_Exp*_tmp2FC;struct Cyc_Absyn_Conref*_tmp2FD;void*_tmp2FE;struct
Cyc_Absyn_FnInfo _tmp2FF;struct Cyc_List_List*_tmp300;struct Cyc_Core_Opt*_tmp301;
void*_tmp302;struct Cyc_List_List*_tmp303;int _tmp304;struct Cyc_Absyn_VarargInfo*
_tmp305;struct Cyc_List_List*_tmp306;struct Cyc_List_List*_tmp307;void*_tmp308;
struct Cyc_Absyn_FnInfo _tmp309;struct Cyc_List_List*_tmp30A;struct Cyc_Core_Opt*
_tmp30B;void*_tmp30C;struct Cyc_List_List*_tmp30D;int _tmp30E;struct Cyc_Absyn_VarargInfo*
_tmp30F;struct Cyc_List_List*_tmp310;struct Cyc_List_List*_tmp311;void*_tmp312;
struct Cyc_List_List*_tmp313;void*_tmp314;struct Cyc_List_List*_tmp315;void*
_tmp316;void*_tmp317;struct Cyc_List_List*_tmp318;void*_tmp319;void*_tmp31A;
struct Cyc_List_List*_tmp31B;void*_tmp31C;void*_tmp31D;void*_tmp31E;void*_tmp31F;
void*_tmp320;void*_tmp321;void*_tmp322;void*_tmp323;void*_tmp324;void*_tmp325;
void*_tmp326;void*_tmp327;void*_tmp328;void*_tmp329;_LL229: _tmp297=_tmp296.f1;
if(_tmp297 <= (void*)4)goto _LL22B;if(*((int*)_tmp297)!= 0)goto _LL22B;_LL22A: Cyc_Tcutil_unify_it(
t2,t1);return;_LL22B: _tmp298=_tmp296.f1;if((int)_tmp298 != 0)goto _LL22D;_tmp299=
_tmp296.f2;if((int)_tmp299 != 0)goto _LL22D;_LL22C: return;_LL22D: _tmp29A=_tmp296.f1;
if(_tmp29A <= (void*)4)goto _LL22F;if(*((int*)_tmp29A)!= 1)goto _LL22F;_tmp29B=((
struct Cyc_Absyn_VarType_struct*)_tmp29A)->f1;_tmp29C=_tmp296.f2;if(_tmp29C <= (
void*)4)goto _LL22F;if(*((int*)_tmp29C)!= 1)goto _LL22F;_tmp29D=((struct Cyc_Absyn_VarType_struct*)
_tmp29C)->f1;_LL22E: {struct _dynforward_ptr*_tmp32A=_tmp29B->name;struct
_dynforward_ptr*_tmp32B=_tmp29D->name;int _tmp32C=*((int*)_check_null(_tmp29B->identity));
int _tmp32D=*((int*)_check_null(_tmp29D->identity));void*_tmp32E=Cyc_Tcutil_tvar_kind(
_tmp29B);void*_tmp32F=Cyc_Tcutil_tvar_kind(_tmp29D);if(_tmp32D == _tmp32C  && Cyc_strptrcmp(
_tmp32A,_tmp32B)== 0){if(_tmp32E != _tmp32F)({struct Cyc_String_pa_struct _tmp334;
_tmp334.tag=0;_tmp334.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(
_tmp32F));{struct Cyc_String_pa_struct _tmp333;_tmp333.tag=0;_tmp333.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(_tmp32E));{
struct Cyc_String_pa_struct _tmp332;_tmp332.tag=0;_tmp332.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp32A);{void*_tmp330[3]={& _tmp332,&
_tmp333,& _tmp334};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp331="same type variable %s has kinds %s and %s";_tag_dynforward(
_tmp331,sizeof(char),_get_zero_arr_size(_tmp331,42));}),_tag_dynforward(_tmp330,
sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=({const char*_tmp335="(variable types are not the same)";
_tag_dynforward(_tmp335,sizeof(char),_get_zero_arr_size(_tmp335,34));});goto
_LL228;}_LL22F: _tmp29E=_tmp296.f1;if(_tmp29E <= (void*)4)goto _LL231;if(*((int*)
_tmp29E)!= 10)goto _LL231;_tmp29F=((struct Cyc_Absyn_AggrType_struct*)_tmp29E)->f1;
_tmp2A0=_tmp29F.aggr_info;_tmp2A1=_tmp29F.targs;_tmp2A2=_tmp296.f2;if(_tmp2A2 <= (
void*)4)goto _LL231;if(*((int*)_tmp2A2)!= 10)goto _LL231;_tmp2A3=((struct Cyc_Absyn_AggrType_struct*)
_tmp2A2)->f1;_tmp2A4=_tmp2A3.aggr_info;_tmp2A5=_tmp2A3.targs;_LL230: {void*
_tmp337;struct _tuple1*_tmp338;struct _tuple5 _tmp336=Cyc_Absyn_aggr_kinded_name(
_tmp2A4);_tmp337=_tmp336.f1;_tmp338=_tmp336.f2;{void*_tmp33A;struct _tuple1*
_tmp33B;struct _tuple5 _tmp339=Cyc_Absyn_aggr_kinded_name(_tmp2A0);_tmp33A=_tmp339.f1;
_tmp33B=_tmp339.f2;if(_tmp337 != _tmp33A){Cyc_Tcutil_failure_reason=({const char*
_tmp33C="(struct and union type)";_tag_dynforward(_tmp33C,sizeof(char),
_get_zero_arr_size(_tmp33C,24));});goto _LL228;}if(Cyc_Absyn_qvar_cmp(_tmp338,
_tmp33B)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp33D="(different type name)";
_tag_dynforward(_tmp33D,sizeof(char),_get_zero_arr_size(_tmp33D,22));});goto
_LL228;}Cyc_Tcutil_unify_list(_tmp2A5,_tmp2A1);return;}}_LL231: _tmp2A6=_tmp296.f1;
if(_tmp2A6 <= (void*)4)goto _LL233;if(*((int*)_tmp2A6)!= 12)goto _LL233;_tmp2A7=((
struct Cyc_Absyn_EnumType_struct*)_tmp2A6)->f1;_tmp2A8=_tmp296.f2;if(_tmp2A8 <= (
void*)4)goto _LL233;if(*((int*)_tmp2A8)!= 12)goto _LL233;_tmp2A9=((struct Cyc_Absyn_EnumType_struct*)
_tmp2A8)->f1;_LL232: if(Cyc_Absyn_qvar_cmp(_tmp2A7,_tmp2A9)== 0)return;Cyc_Tcutil_failure_reason=({
const char*_tmp33E="(different enum types)";_tag_dynforward(_tmp33E,sizeof(char),
_get_zero_arr_size(_tmp33E,23));});goto _LL228;_LL233: _tmp2AA=_tmp296.f1;if(
_tmp2AA <= (void*)4)goto _LL235;if(*((int*)_tmp2AA)!= 13)goto _LL235;_tmp2AB=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp2AA)->f1;_tmp2AC=_tmp296.f2;if(_tmp2AC
<= (void*)4)goto _LL235;if(*((int*)_tmp2AC)!= 13)goto _LL235;_tmp2AD=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp2AC)->f1;_LL234: {int bad=0;for(0;_tmp2AB != 0  && _tmp2AD != 0;(_tmp2AB=_tmp2AB->tl,
_tmp2AD=_tmp2AD->tl)){struct Cyc_Absyn_Enumfield*_tmp33F=(struct Cyc_Absyn_Enumfield*)
_tmp2AB->hd;struct Cyc_Absyn_Enumfield*_tmp340=(struct Cyc_Absyn_Enumfield*)
_tmp2AD->hd;if(Cyc_Absyn_qvar_cmp(_tmp33F->name,_tmp340->name)!= 0){Cyc_Tcutil_failure_reason=({
const char*_tmp341="(different names for enum fields)";_tag_dynforward(_tmp341,
sizeof(char),_get_zero_arr_size(_tmp341,34));});bad=1;break;}if(_tmp33F->tag == 
_tmp340->tag)continue;if(_tmp33F->tag == 0  || _tmp340->tag == 0){Cyc_Tcutil_failure_reason=({
const char*_tmp342="(different tag values for enum fields)";_tag_dynforward(
_tmp342,sizeof(char),_get_zero_arr_size(_tmp342,39));});bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp33F->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp340->tag))){Cyc_Tcutil_failure_reason=({const char*_tmp343="(different tag values for enum fields)";
_tag_dynforward(_tmp343,sizeof(char),_get_zero_arr_size(_tmp343,39));});bad=1;
break;}}if(bad)goto _LL228;if(_tmp2AB == 0  && _tmp2AD == 0)return;Cyc_Tcutil_failure_reason=({
const char*_tmp344="(different number of fields for enums)";_tag_dynforward(
_tmp344,sizeof(char),_get_zero_arr_size(_tmp344,39));});goto _LL228;}_LL235:
_tmp2AE=_tmp296.f1;if(_tmp2AE <= (void*)4)goto _LL237;if(*((int*)_tmp2AE)!= 2)goto
_LL237;_tmp2AF=((struct Cyc_Absyn_TunionType_struct*)_tmp2AE)->f1;_tmp2B0=_tmp2AF.tunion_info;
if((((((struct Cyc_Absyn_TunionType_struct*)_tmp2AE)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL237;_tmp2B1=(_tmp2B0.KnownTunion).f1;_tmp2B2=*_tmp2B1;_tmp2B3=
_tmp2AF.targs;_tmp2B4=_tmp2AF.rgn;_tmp2B5=_tmp296.f2;if(_tmp2B5 <= (void*)4)goto
_LL237;if(*((int*)_tmp2B5)!= 2)goto _LL237;_tmp2B6=((struct Cyc_Absyn_TunionType_struct*)
_tmp2B5)->f1;_tmp2B7=_tmp2B6.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp2B5)->f1).tunion_info).KnownTunion).tag != 1)goto _LL237;_tmp2B8=(_tmp2B7.KnownTunion).f1;
_tmp2B9=*_tmp2B8;_tmp2BA=_tmp2B6.targs;_tmp2BB=_tmp2B6.rgn;_LL236: if(_tmp2B2 == 
_tmp2B9  || Cyc_Absyn_qvar_cmp(_tmp2B2->name,_tmp2B9->name)== 0){if(_tmp2BB != 0
 && _tmp2B4 != 0)Cyc_Tcutil_unify_it((void*)_tmp2BB->v,(void*)_tmp2B4->v);else{
if(_tmp2BB != 0  || _tmp2B4 != 0)goto Tunion_fail;}Cyc_Tcutil_unify_list(_tmp2BA,
_tmp2B3);return;}Tunion_fail: Cyc_Tcutil_failure_reason=({const char*_tmp345="(different tunion types)";
_tag_dynforward(_tmp345,sizeof(char),_get_zero_arr_size(_tmp345,25));});goto
_LL228;_LL237: _tmp2BC=_tmp296.f1;if(_tmp2BC <= (void*)4)goto _LL239;if(*((int*)
_tmp2BC)!= 3)goto _LL239;_tmp2BD=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp2BC)->f1;_tmp2BE=_tmp2BD.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp2BC)->f1).field_info).KnownTunionfield).tag != 1)goto _LL239;_tmp2BF=(_tmp2BE.KnownTunionfield).f1;
_tmp2C0=(_tmp2BE.KnownTunionfield).f2;_tmp2C1=_tmp2BD.targs;_tmp2C2=_tmp296.f2;
if(_tmp2C2 <= (void*)4)goto _LL239;if(*((int*)_tmp2C2)!= 3)goto _LL239;_tmp2C3=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp2C2)->f1;_tmp2C4=_tmp2C3.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp2C2)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL239;_tmp2C5=(_tmp2C4.KnownTunionfield).f1;_tmp2C6=(_tmp2C4.KnownTunionfield).f2;
_tmp2C7=_tmp2C3.targs;_LL238: if((_tmp2BF == _tmp2C5  || Cyc_Absyn_qvar_cmp(_tmp2BF->name,
_tmp2C5->name)== 0) && (_tmp2C0 == _tmp2C6  || Cyc_Absyn_qvar_cmp(_tmp2C0->name,
_tmp2C6->name)== 0)){Cyc_Tcutil_unify_list(_tmp2C7,_tmp2C1);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp346="(different tunion field types)";_tag_dynforward(_tmp346,
sizeof(char),_get_zero_arr_size(_tmp346,31));});goto _LL228;_LL239: _tmp2C8=
_tmp296.f1;if(_tmp2C8 <= (void*)4)goto _LL23B;if(*((int*)_tmp2C8)!= 4)goto _LL23B;
_tmp2C9=((struct Cyc_Absyn_PointerType_struct*)_tmp2C8)->f1;_tmp2CA=(void*)
_tmp2C9.elt_typ;_tmp2CB=_tmp2C9.elt_tq;_tmp2CC=_tmp2C9.ptr_atts;_tmp2CD=(void*)
_tmp2CC.rgn;_tmp2CE=_tmp2CC.nullable;_tmp2CF=_tmp2CC.bounds;_tmp2D0=_tmp2CC.zero_term;
_tmp2D1=_tmp296.f2;if(_tmp2D1 <= (void*)4)goto _LL23B;if(*((int*)_tmp2D1)!= 4)goto
_LL23B;_tmp2D2=((struct Cyc_Absyn_PointerType_struct*)_tmp2D1)->f1;_tmp2D3=(void*)
_tmp2D2.elt_typ;_tmp2D4=_tmp2D2.elt_tq;_tmp2D5=_tmp2D2.ptr_atts;_tmp2D6=(void*)
_tmp2D5.rgn;_tmp2D7=_tmp2D5.nullable;_tmp2D8=_tmp2D5.bounds;_tmp2D9=_tmp2D5.zero_term;
_LL23A: Cyc_Tcutil_unify_it(_tmp2D3,_tmp2CA);Cyc_Tcutil_unify_it(_tmp2CD,_tmp2D6);
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;((void(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2D9,_tmp2D0,({const char*_tmp347="(not both zero terminated)";
_tag_dynforward(_tmp347,sizeof(char),_get_zero_arr_size(_tmp347,27));}));Cyc_Tcutil_unify_tqual(
_tmp2D4,_tmp2D3,_tmp2CB,_tmp2CA);Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp2D8,_tmp2CF,({const char*_tmp348="(different pointer bounds)";_tag_dynforward(
_tmp348,sizeof(char),_get_zero_arr_size(_tmp348,27));}));{union Cyc_Absyn_Constraint_union
_tmp349=(Cyc_Absyn_compress_conref(_tmp2D8))->v;void*_tmp34A;void*_tmp34B;_LL264:
if((_tmp349.Eq_constr).tag != 0)goto _LL266;_tmp34A=(_tmp349.Eq_constr).f1;if((int)
_tmp34A != 0)goto _LL266;_LL265: return;_LL266: if((_tmp349.Eq_constr).tag != 0)goto
_LL268;_tmp34B=(_tmp349.Eq_constr).f1;if((int)_tmp34B != 1)goto _LL268;_LL267:
return;_LL268:;_LL269: goto _LL263;_LL263:;}((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2D7,_tmp2CE,({const char*_tmp34C="(incompatible pointer types)";
_tag_dynforward(_tmp34C,sizeof(char),_get_zero_arr_size(_tmp34C,29));}));return;
_LL23B: _tmp2DA=_tmp296.f1;if(_tmp2DA <= (void*)4)goto _LL23D;if(*((int*)_tmp2DA)!= 
5)goto _LL23D;_tmp2DB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2DA)->f1;
_tmp2DC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2DA)->f2;_tmp2DD=_tmp296.f2;
if(_tmp2DD <= (void*)4)goto _LL23D;if(*((int*)_tmp2DD)!= 5)goto _LL23D;_tmp2DE=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2DD)->f1;_tmp2DF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2DD)->f2;_LL23C: if(_tmp2DE == _tmp2DB  && ((_tmp2DF == _tmp2DC  || _tmp2DF == (
void*)2  && _tmp2DC == (void*)3) || _tmp2DF == (void*)3  && _tmp2DC == (void*)2))
return;Cyc_Tcutil_failure_reason=({const char*_tmp34D="(different integral types)";
_tag_dynforward(_tmp34D,sizeof(char),_get_zero_arr_size(_tmp34D,27));});goto
_LL228;_LL23D: _tmp2E0=_tmp296.f1;if((int)_tmp2E0 != 1)goto _LL23F;_tmp2E1=_tmp296.f2;
if((int)_tmp2E1 != 1)goto _LL23F;_LL23E: return;_LL23F: _tmp2E2=_tmp296.f1;if(
_tmp2E2 <= (void*)4)goto _LL241;if(*((int*)_tmp2E2)!= 6)goto _LL241;_tmp2E3=((
struct Cyc_Absyn_DoubleType_struct*)_tmp2E2)->f1;_tmp2E4=_tmp296.f2;if(_tmp2E4 <= (
void*)4)goto _LL241;if(*((int*)_tmp2E4)!= 6)goto _LL241;_tmp2E5=((struct Cyc_Absyn_DoubleType_struct*)
_tmp2E4)->f1;_LL240: if(_tmp2E3 == _tmp2E5)return;goto _LL228;_LL241: _tmp2E6=
_tmp296.f1;if(_tmp2E6 <= (void*)4)goto _LL243;if(*((int*)_tmp2E6)!= 14)goto _LL243;
_tmp2E7=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2E6)->f1;_tmp2E8=_tmp296.f2;
if(_tmp2E8 <= (void*)4)goto _LL243;if(*((int*)_tmp2E8)!= 14)goto _LL243;_tmp2E9=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2E8)->f1;_LL242: Cyc_Tcutil_unify_it(
_tmp2E7,_tmp2E9);return;_LL243: _tmp2EA=_tmp296.f1;if(_tmp2EA <= (void*)4)goto
_LL245;if(*((int*)_tmp2EA)!= 18)goto _LL245;_tmp2EB=((struct Cyc_Absyn_TypeInt_struct*)
_tmp2EA)->f1;_tmp2EC=_tmp296.f2;if(_tmp2EC <= (void*)4)goto _LL245;if(*((int*)
_tmp2EC)!= 18)goto _LL245;_tmp2ED=((struct Cyc_Absyn_TypeInt_struct*)_tmp2EC)->f1;
_LL244: if(_tmp2EB == _tmp2ED)return;Cyc_Tcutil_failure_reason=({const char*_tmp34E="(different type integers)";
_tag_dynforward(_tmp34E,sizeof(char),_get_zero_arr_size(_tmp34E,26));});goto
_LL228;_LL245: _tmp2EE=_tmp296.f1;if(_tmp2EE <= (void*)4)goto _LL247;if(*((int*)
_tmp2EE)!= 17)goto _LL247;_tmp2EF=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp2EE)->f1;_tmp2F0=_tmp296.f2;if(_tmp2F0 <= (void*)4)goto _LL247;if(*((int*)
_tmp2F0)!= 17)goto _LL247;_tmp2F1=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp2F0)->f1;_LL246: Cyc_Tcutil_unify_it(_tmp2EF,_tmp2F1);return;_LL247: _tmp2F2=
_tmp296.f1;if(_tmp2F2 <= (void*)4)goto _LL249;if(*((int*)_tmp2F2)!= 7)goto _LL249;
_tmp2F3=((struct Cyc_Absyn_ArrayType_struct*)_tmp2F2)->f1;_tmp2F4=(void*)_tmp2F3.elt_type;
_tmp2F5=_tmp2F3.tq;_tmp2F6=_tmp2F3.num_elts;_tmp2F7=_tmp2F3.zero_term;_tmp2F8=
_tmp296.f2;if(_tmp2F8 <= (void*)4)goto _LL249;if(*((int*)_tmp2F8)!= 7)goto _LL249;
_tmp2F9=((struct Cyc_Absyn_ArrayType_struct*)_tmp2F8)->f1;_tmp2FA=(void*)_tmp2F9.elt_type;
_tmp2FB=_tmp2F9.tq;_tmp2FC=_tmp2F9.num_elts;_tmp2FD=_tmp2F9.zero_term;_LL248: Cyc_Tcutil_unify_it(
_tmp2FA,_tmp2F4);Cyc_Tcutil_unify_tqual(_tmp2FB,_tmp2FA,_tmp2F5,_tmp2F4);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2F7,_tmp2FD,({const char*_tmp34F="(not both zero terminated)";
_tag_dynforward(_tmp34F,sizeof(char),_get_zero_arr_size(_tmp34F,27));}));if(
_tmp2F6 == _tmp2FC)return;if(_tmp2F6 == 0  || _tmp2FC == 0)goto _LL228;if(Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_tmp2F6,(struct Cyc_Absyn_Exp*)_tmp2FC))return;Cyc_Tcutil_failure_reason=({
const char*_tmp350="(different array sizes)";_tag_dynforward(_tmp350,sizeof(char),
_get_zero_arr_size(_tmp350,24));});goto _LL228;_LL249: _tmp2FE=_tmp296.f1;if(
_tmp2FE <= (void*)4)goto _LL24B;if(*((int*)_tmp2FE)!= 8)goto _LL24B;_tmp2FF=((
struct Cyc_Absyn_FnType_struct*)_tmp2FE)->f1;_tmp300=_tmp2FF.tvars;_tmp301=
_tmp2FF.effect;_tmp302=(void*)_tmp2FF.ret_typ;_tmp303=_tmp2FF.args;_tmp304=
_tmp2FF.c_varargs;_tmp305=_tmp2FF.cyc_varargs;_tmp306=_tmp2FF.rgn_po;_tmp307=
_tmp2FF.attributes;_tmp308=_tmp296.f2;if(_tmp308 <= (void*)4)goto _LL24B;if(*((int*)
_tmp308)!= 8)goto _LL24B;_tmp309=((struct Cyc_Absyn_FnType_struct*)_tmp308)->f1;
_tmp30A=_tmp309.tvars;_tmp30B=_tmp309.effect;_tmp30C=(void*)_tmp309.ret_typ;
_tmp30D=_tmp309.args;_tmp30E=_tmp309.c_varargs;_tmp30F=_tmp309.cyc_varargs;
_tmp310=_tmp309.rgn_po;_tmp311=_tmp309.attributes;_LL24A: {int done=0;{struct
_RegionHandle _tmp351=_new_region("rgn");struct _RegionHandle*rgn=& _tmp351;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp30A != 0){if(_tmp300 == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp352="(second function type has too few type variables)";
_tag_dynforward(_tmp352,sizeof(char),_get_zero_arr_size(_tmp352,50));});(int)
_throw((void*)Cyc_Tcutil_Unify);}{void*_tmp353=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp30A->hd);void*_tmp354=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp300->hd);
if(_tmp353 != _tmp354){Cyc_Tcutil_failure_reason=(struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp359;_tmp359.tag=0;_tmp359.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_kind2string(_tmp354));{struct Cyc_String_pa_struct
_tmp358;_tmp358.tag=0;_tmp358.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_kind2string(_tmp353));{struct Cyc_String_pa_struct _tmp357;_tmp357.tag=
0;_tmp357.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp30A->hd));{void*_tmp355[3]={& _tmp357,& _tmp358,& _tmp359};
Cyc_aprintf(({const char*_tmp356="(type var %s has different kinds %s and %s)";
_tag_dynforward(_tmp356,sizeof(char),_get_zero_arr_size(_tmp356,44));}),
_tag_dynforward(_tmp355,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}
inst=({struct Cyc_List_List*_tmp35A=_region_malloc(rgn,sizeof(*_tmp35A));_tmp35A->hd=({
struct _tuple8*_tmp35B=_region_malloc(rgn,sizeof(*_tmp35B));_tmp35B->f1=(struct
Cyc_Absyn_Tvar*)_tmp300->hd;_tmp35B->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp35C=_cycalloc(sizeof(*_tmp35C));_tmp35C[0]=({struct Cyc_Absyn_VarType_struct
_tmp35D;_tmp35D.tag=1;_tmp35D.f1=(struct Cyc_Absyn_Tvar*)_tmp30A->hd;_tmp35D;});
_tmp35C;});_tmp35B;});_tmp35A->tl=inst;_tmp35A;});_tmp30A=_tmp30A->tl;_tmp300=
_tmp300->tl;}}if(_tmp300 != 0){Cyc_Tcutil_failure_reason=({const char*_tmp35E="(second function type has too many type variables)";
_tag_dynforward(_tmp35E,sizeof(char),_get_zero_arr_size(_tmp35E,51));});
_npop_handler(0);goto _LL228;}if(inst != 0){Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_struct*
_tmp35F=_cycalloc(sizeof(*_tmp35F));_tmp35F[0]=({struct Cyc_Absyn_FnType_struct
_tmp360;_tmp360.tag=8;_tmp360.f1=({struct Cyc_Absyn_FnInfo _tmp361;_tmp361.tvars=0;
_tmp361.effect=_tmp30B;_tmp361.ret_typ=(void*)_tmp30C;_tmp361.args=_tmp30D;
_tmp361.c_varargs=_tmp30E;_tmp361.cyc_varargs=_tmp30F;_tmp361.rgn_po=_tmp310;
_tmp361.attributes=_tmp311;_tmp361;});_tmp360;});_tmp35F;}),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*_tmp362=_cycalloc(sizeof(*
_tmp362));_tmp362[0]=({struct Cyc_Absyn_FnType_struct _tmp363;_tmp363.tag=8;
_tmp363.f1=({struct Cyc_Absyn_FnInfo _tmp364;_tmp364.tvars=0;_tmp364.effect=
_tmp301;_tmp364.ret_typ=(void*)_tmp302;_tmp364.args=_tmp303;_tmp364.c_varargs=
_tmp304;_tmp364.cyc_varargs=_tmp305;_tmp364.rgn_po=_tmp306;_tmp364.attributes=
_tmp307;_tmp364;});_tmp363;});_tmp362;})));done=1;}};_pop_region(rgn);}if(done)
return;Cyc_Tcutil_unify_it(_tmp30C,_tmp302);for(0;_tmp30D != 0  && _tmp303 != 0;(
_tmp30D=_tmp30D->tl,_tmp303=_tmp303->tl)){struct Cyc_Absyn_Tqual _tmp366;void*
_tmp367;struct _tuple2 _tmp365=*((struct _tuple2*)_tmp30D->hd);_tmp366=_tmp365.f2;
_tmp367=_tmp365.f3;{struct Cyc_Absyn_Tqual _tmp369;void*_tmp36A;struct _tuple2
_tmp368=*((struct _tuple2*)_tmp303->hd);_tmp369=_tmp368.f2;_tmp36A=_tmp368.f3;Cyc_Tcutil_unify_it(
_tmp367,_tmp36A);Cyc_Tcutil_unify_tqual(_tmp366,_tmp367,_tmp369,_tmp36A);}}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp30D != 0  || _tmp303 != 0){Cyc_Tcutil_failure_reason=({
const char*_tmp36B="(function types have different number of arguments)";
_tag_dynforward(_tmp36B,sizeof(char),_get_zero_arr_size(_tmp36B,52));});goto
_LL228;}if(_tmp30E != _tmp304){Cyc_Tcutil_failure_reason=({const char*_tmp36C="(only one function type takes C varargs)";
_tag_dynforward(_tmp36C,sizeof(char),_get_zero_arr_size(_tmp36C,41));});goto
_LL228;}{int bad_cyc_vararg=0;{struct _tuple11 _tmp36E=({struct _tuple11 _tmp36D;
_tmp36D.f1=_tmp30F;_tmp36D.f2=_tmp305;_tmp36D;});struct Cyc_Absyn_VarargInfo*
_tmp36F;struct Cyc_Absyn_VarargInfo*_tmp370;struct Cyc_Absyn_VarargInfo*_tmp371;
struct Cyc_Absyn_VarargInfo*_tmp372;struct Cyc_Absyn_VarargInfo*_tmp373;struct Cyc_Absyn_VarargInfo
_tmp374;struct Cyc_Core_Opt*_tmp375;struct Cyc_Absyn_Tqual _tmp376;void*_tmp377;int
_tmp378;struct Cyc_Absyn_VarargInfo*_tmp379;struct Cyc_Absyn_VarargInfo _tmp37A;
struct Cyc_Core_Opt*_tmp37B;struct Cyc_Absyn_Tqual _tmp37C;void*_tmp37D;int _tmp37E;
_LL26B: _tmp36F=_tmp36E.f1;if(_tmp36F != 0)goto _LL26D;_tmp370=_tmp36E.f2;if(
_tmp370 != 0)goto _LL26D;_LL26C: goto _LL26A;_LL26D: _tmp371=_tmp36E.f1;if(_tmp371 != 
0)goto _LL26F;_LL26E: goto _LL270;_LL26F: _tmp372=_tmp36E.f2;if(_tmp372 != 0)goto
_LL271;_LL270: bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp37F="(only one function type takes varargs)";
_tag_dynforward(_tmp37F,sizeof(char),_get_zero_arr_size(_tmp37F,39));});goto
_LL26A;_LL271: _tmp373=_tmp36E.f1;if(_tmp373 == 0)goto _LL26A;_tmp374=*_tmp373;
_tmp375=_tmp374.name;_tmp376=_tmp374.tq;_tmp377=(void*)_tmp374.type;_tmp378=
_tmp374.inject;_tmp379=_tmp36E.f2;if(_tmp379 == 0)goto _LL26A;_tmp37A=*_tmp379;
_tmp37B=_tmp37A.name;_tmp37C=_tmp37A.tq;_tmp37D=(void*)_tmp37A.type;_tmp37E=
_tmp37A.inject;_LL272: Cyc_Tcutil_unify_it(_tmp377,_tmp37D);Cyc_Tcutil_unify_tqual(
_tmp376,_tmp377,_tmp37C,_tmp37D);if(_tmp378 != _tmp37E){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({
const char*_tmp380="(only one function type injects varargs)";_tag_dynforward(
_tmp380,sizeof(char),_get_zero_arr_size(_tmp380,41));});}goto _LL26A;_LL26A:;}if(
bad_cyc_vararg)goto _LL228;{int bad_effect=0;{struct _tuple12 _tmp382=({struct
_tuple12 _tmp381;_tmp381.f1=_tmp30B;_tmp381.f2=_tmp301;_tmp381;});struct Cyc_Core_Opt*
_tmp383;struct Cyc_Core_Opt*_tmp384;struct Cyc_Core_Opt*_tmp385;struct Cyc_Core_Opt*
_tmp386;_LL274: _tmp383=_tmp382.f1;if(_tmp383 != 0)goto _LL276;_tmp384=_tmp382.f2;
if(_tmp384 != 0)goto _LL276;_LL275: goto _LL273;_LL276: _tmp385=_tmp382.f1;if(_tmp385
!= 0)goto _LL278;_LL277: goto _LL279;_LL278: _tmp386=_tmp382.f2;if(_tmp386 != 0)goto
_LL27A;_LL279: bad_effect=1;goto _LL273;_LL27A:;_LL27B: bad_effect=!Cyc_Tcutil_unify_effect((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp30B))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp301))->v);goto _LL273;_LL273:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=({const char*_tmp387="(function type effects do not unify)";
_tag_dynforward(_tmp387,sizeof(char),_get_zero_arr_size(_tmp387,37));});goto
_LL228;}if(!Cyc_Tcutil_same_atts(_tmp307,_tmp311)){Cyc_Tcutil_failure_reason=({
const char*_tmp388="(function types have different attributes)";_tag_dynforward(
_tmp388,sizeof(char),_get_zero_arr_size(_tmp388,43));});goto _LL228;}if(!Cyc_Tcutil_same_rgn_po(
_tmp306,_tmp310)){Cyc_Tcutil_failure_reason=({const char*_tmp389="(function types have different region lifetime orderings)";
_tag_dynforward(_tmp389,sizeof(char),_get_zero_arr_size(_tmp389,58));});goto
_LL228;}return;}}}_LL24B: _tmp312=_tmp296.f1;if(_tmp312 <= (void*)4)goto _LL24D;if(*((
int*)_tmp312)!= 9)goto _LL24D;_tmp313=((struct Cyc_Absyn_TupleType_struct*)_tmp312)->f1;
_tmp314=_tmp296.f2;if(_tmp314 <= (void*)4)goto _LL24D;if(*((int*)_tmp314)!= 9)goto
_LL24D;_tmp315=((struct Cyc_Absyn_TupleType_struct*)_tmp314)->f1;_LL24C: for(0;
_tmp315 != 0  && _tmp313 != 0;(_tmp315=_tmp315->tl,_tmp313=_tmp313->tl)){struct Cyc_Absyn_Tqual
_tmp38B;void*_tmp38C;struct _tuple4 _tmp38A=*((struct _tuple4*)_tmp315->hd);_tmp38B=
_tmp38A.f1;_tmp38C=_tmp38A.f2;{struct Cyc_Absyn_Tqual _tmp38E;void*_tmp38F;struct
_tuple4 _tmp38D=*((struct _tuple4*)_tmp313->hd);_tmp38E=_tmp38D.f1;_tmp38F=_tmp38D.f2;
Cyc_Tcutil_unify_it(_tmp38C,_tmp38F);Cyc_Tcutil_unify_tqual(_tmp38B,_tmp38C,
_tmp38E,_tmp38F);}}if(_tmp315 == 0  && _tmp313 == 0)return;Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp390="(tuple types have different numbers of components)";
_tag_dynforward(_tmp390,sizeof(char),_get_zero_arr_size(_tmp390,51));});goto
_LL228;_LL24D: _tmp316=_tmp296.f1;if(_tmp316 <= (void*)4)goto _LL24F;if(*((int*)
_tmp316)!= 11)goto _LL24F;_tmp317=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp316)->f1;_tmp318=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp316)->f2;_tmp319=
_tmp296.f2;if(_tmp319 <= (void*)4)goto _LL24F;if(*((int*)_tmp319)!= 11)goto _LL24F;
_tmp31A=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp319)->f1;_tmp31B=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp319)->f2;_LL24E: if(_tmp31A != _tmp317){
Cyc_Tcutil_failure_reason=({const char*_tmp391="(struct and union type)";
_tag_dynforward(_tmp391,sizeof(char),_get_zero_arr_size(_tmp391,24));});goto
_LL228;}for(0;_tmp31B != 0  && _tmp318 != 0;(_tmp31B=_tmp31B->tl,_tmp318=_tmp318->tl)){
struct Cyc_Absyn_Aggrfield*_tmp392=(struct Cyc_Absyn_Aggrfield*)_tmp31B->hd;struct
Cyc_Absyn_Aggrfield*_tmp393=(struct Cyc_Absyn_Aggrfield*)_tmp318->hd;if(Cyc_strptrcmp(
_tmp392->name,_tmp393->name)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp394="(different member names)";
_tag_dynforward(_tmp394,sizeof(char),_get_zero_arr_size(_tmp394,25));});(int)
_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_unify_it((void*)_tmp392->type,(void*)
_tmp393->type);Cyc_Tcutil_unify_tqual(_tmp392->tq,(void*)_tmp392->type,_tmp393->tq,(
void*)_tmp393->type);if(!Cyc_Tcutil_same_atts(_tmp392->attributes,_tmp393->attributes)){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({
const char*_tmp395="(different attributes on member)";_tag_dynforward(_tmp395,
sizeof(char),_get_zero_arr_size(_tmp395,33));});(int)_throw((void*)Cyc_Tcutil_Unify);}
if((_tmp392->width != 0  && _tmp393->width == 0  || _tmp393->width != 0  && _tmp392->width
== 0) || (_tmp392->width != 0  && _tmp393->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp392->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp393->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp396="(different bitfield widths on member)";
_tag_dynforward(_tmp396,sizeof(char),_get_zero_arr_size(_tmp396,38));});(int)
_throw((void*)Cyc_Tcutil_Unify);}}if(_tmp31B == 0  && _tmp318 == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp397="(different number of members)";
_tag_dynforward(_tmp397,sizeof(char),_get_zero_arr_size(_tmp397,30));});goto
_LL228;_LL24F: _tmp31C=_tmp296.f1;if((int)_tmp31C != 2)goto _LL251;_tmp31D=_tmp296.f2;
if((int)_tmp31D != 2)goto _LL251;_LL250: return;_LL251: _tmp31E=_tmp296.f1;if((int)
_tmp31E != 3)goto _LL253;_tmp31F=_tmp296.f2;if((int)_tmp31F != 3)goto _LL253;_LL252:
return;_LL253: _tmp320=_tmp296.f1;if(_tmp320 <= (void*)4)goto _LL255;if(*((int*)
_tmp320)!= 15)goto _LL255;_tmp321=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp320)->f1;_tmp322=_tmp296.f2;if(_tmp322 <= (void*)4)goto _LL255;if(*((int*)
_tmp322)!= 15)goto _LL255;_tmp323=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp322)->f1;_LL254: Cyc_Tcutil_unify_it(_tmp321,_tmp323);return;_LL255: _tmp324=
_tmp296.f1;if(_tmp324 <= (void*)4)goto _LL257;if(*((int*)_tmp324)!= 20)goto _LL257;
_LL256: goto _LL258;_LL257: _tmp325=_tmp296.f2;if(_tmp325 <= (void*)4)goto _LL259;if(*((
int*)_tmp325)!= 20)goto _LL259;_LL258: goto _LL25A;_LL259: _tmp326=_tmp296.f1;if(
_tmp326 <= (void*)4)goto _LL25B;if(*((int*)_tmp326)!= 19)goto _LL25B;_LL25A: goto
_LL25C;_LL25B: _tmp327=_tmp296.f1;if(_tmp327 <= (void*)4)goto _LL25D;if(*((int*)
_tmp327)!= 21)goto _LL25D;_LL25C: goto _LL25E;_LL25D: _tmp328=_tmp296.f2;if(_tmp328
<= (void*)4)goto _LL25F;if(*((int*)_tmp328)!= 21)goto _LL25F;_LL25E: goto _LL260;
_LL25F: _tmp329=_tmp296.f2;if(_tmp329 <= (void*)4)goto _LL261;if(*((int*)_tmp329)!= 
19)goto _LL261;_LL260: if(Cyc_Tcutil_unify_effect(t1,t2))return;Cyc_Tcutil_failure_reason=({
const char*_tmp398="(effects don't unify)";_tag_dynforward(_tmp398,sizeof(char),
_get_zero_arr_size(_tmp398,22));});goto _LL228;_LL261:;_LL262: goto _LL228;_LL228:;}(
int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),
void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2
== 0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int
Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int
_tmp399=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int _tmp39A=(
tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp399,_tmp39A);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(x);y=
Cyc_Absyn_compress_conref(y);if(x == y)return 0;{union Cyc_Absyn_Constraint_union
_tmp39B=x->v;void*_tmp39C;_LL27D: if((_tmp39B.No_constr).tag != 2)goto _LL27F;
_LL27E: return - 1;_LL27F: if((_tmp39B.Eq_constr).tag != 0)goto _LL281;_tmp39C=(
_tmp39B.Eq_constr).f1;_LL280: {union Cyc_Absyn_Constraint_union _tmp39D=y->v;void*
_tmp39E;_LL284: if((_tmp39D.No_constr).tag != 2)goto _LL286;_LL285: return 1;_LL286:
if((_tmp39D.Eq_constr).tag != 0)goto _LL288;_tmp39E=(_tmp39D.Eq_constr).f1;_LL287:
return cmp(_tmp39C,_tmp39E);_LL288: if((_tmp39D.Forward_constr).tag != 1)goto _LL283;
_LL289:({void*_tmp39F[0]={};Cyc_Tcutil_impos(({const char*_tmp3A0="unify_conref: forward after compress(2)";
_tag_dynforward(_tmp3A0,sizeof(char),_get_zero_arr_size(_tmp3A0,40));}),
_tag_dynforward(_tmp39F,sizeof(void*),0));});_LL283:;}_LL281: if((_tmp39B.Forward_constr).tag
!= 1)goto _LL27C;_LL282:({void*_tmp3A1[0]={};Cyc_Tcutil_impos(({const char*_tmp3A2="unify_conref: forward after compress";
_tag_dynforward(_tmp3A2,sizeof(char),_get_zero_arr_size(_tmp3A2,37));}),
_tag_dynforward(_tmp3A1,sizeof(void*),0));});_LL27C:;}}static int Cyc_Tcutil_tqual_type_cmp(
struct _tuple4*tqt1,struct _tuple4*tqt2){struct _tuple4 _tmp3A4;struct Cyc_Absyn_Tqual
_tmp3A5;void*_tmp3A6;struct _tuple4*_tmp3A3=tqt1;_tmp3A4=*_tmp3A3;_tmp3A5=_tmp3A4.f1;
_tmp3A6=_tmp3A4.f2;{struct _tuple4 _tmp3A8;struct Cyc_Absyn_Tqual _tmp3A9;void*
_tmp3AA;struct _tuple4*_tmp3A7=tqt2;_tmp3A8=*_tmp3A7;_tmp3A9=_tmp3A8.f1;_tmp3AA=
_tmp3A8.f2;{int _tmp3AB=Cyc_Tcutil_tqual_cmp(_tmp3A5,_tmp3A9);if(_tmp3AB != 0)
return _tmp3AB;return Cyc_Tcutil_typecmp(_tmp3A6,_tmp3AA);}}}static int Cyc_Tcutil_aggrfield_cmp(
struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp3AC=Cyc_strptrcmp(
f1->name,f2->name);if(_tmp3AC != 0)return _tmp3AC;{int _tmp3AD=Cyc_Tcutil_tqual_cmp(
f1->tq,f2->tq);if(_tmp3AD != 0)return _tmp3AD;{int _tmp3AE=Cyc_Tcutil_typecmp((void*)
f1->type,(void*)f2->type);if(_tmp3AE != 0)return _tmp3AE;{int _tmp3AF=Cyc_List_list_cmp(
Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);if(_tmp3AF != 0)return
_tmp3AF;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
f1->width,f2->width);}}}}static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*
e1,struct Cyc_Absyn_Enumfield*e2){int _tmp3B0=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp3B0 != 0)return _tmp3B0;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp3B1=t;_LL28B: if((int)_tmp3B1 != 0)goto _LL28D;_LL28C: return 0;
_LL28D: if(_tmp3B1 <= (void*)4)goto _LL299;if(*((int*)_tmp3B1)!= 0)goto _LL28F;
_LL28E: return 1;_LL28F: if(*((int*)_tmp3B1)!= 1)goto _LL291;_LL290: return 2;_LL291:
if(*((int*)_tmp3B1)!= 2)goto _LL293;_LL292: return 3;_LL293: if(*((int*)_tmp3B1)!= 3)
goto _LL295;_LL294: return 4;_LL295: if(*((int*)_tmp3B1)!= 4)goto _LL297;_LL296:
return 5;_LL297: if(*((int*)_tmp3B1)!= 5)goto _LL299;_LL298: return 6;_LL299: if((int)
_tmp3B1 != 1)goto _LL29B;_LL29A: return 7;_LL29B: if(_tmp3B1 <= (void*)4)goto _LL2AF;
if(*((int*)_tmp3B1)!= 6)goto _LL29D;_LL29C: return 8;_LL29D: if(*((int*)_tmp3B1)!= 7)
goto _LL29F;_LL29E: return 9;_LL29F: if(*((int*)_tmp3B1)!= 8)goto _LL2A1;_LL2A0:
return 10;_LL2A1: if(*((int*)_tmp3B1)!= 9)goto _LL2A3;_LL2A2: return 11;_LL2A3: if(*((
int*)_tmp3B1)!= 10)goto _LL2A5;_LL2A4: return 12;_LL2A5: if(*((int*)_tmp3B1)!= 11)
goto _LL2A7;_LL2A6: return 14;_LL2A7: if(*((int*)_tmp3B1)!= 12)goto _LL2A9;_LL2A8:
return 16;_LL2A9: if(*((int*)_tmp3B1)!= 13)goto _LL2AB;_LL2AA: return 17;_LL2AB: if(*((
int*)_tmp3B1)!= 15)goto _LL2AD;_LL2AC: return 18;_LL2AD: if(*((int*)_tmp3B1)!= 16)
goto _LL2AF;_LL2AE: return 19;_LL2AF: if((int)_tmp3B1 != 3)goto _LL2B1;_LL2B0: return 20;
_LL2B1: if((int)_tmp3B1 != 2)goto _LL2B3;_LL2B2: return 21;_LL2B3: if(_tmp3B1 <= (void*)
4)goto _LL2B5;if(*((int*)_tmp3B1)!= 19)goto _LL2B5;_LL2B4: return 22;_LL2B5: if(
_tmp3B1 <= (void*)4)goto _LL2B7;if(*((int*)_tmp3B1)!= 20)goto _LL2B7;_LL2B6: return
23;_LL2B7: if(_tmp3B1 <= (void*)4)goto _LL2B9;if(*((int*)_tmp3B1)!= 21)goto _LL2B9;
_LL2B8: return 24;_LL2B9: if(_tmp3B1 <= (void*)4)goto _LL2BB;if(*((int*)_tmp3B1)!= 14)
goto _LL2BB;_LL2BA: return 25;_LL2BB: if(_tmp3B1 <= (void*)4)goto _LL2BD;if(*((int*)
_tmp3B1)!= 18)goto _LL2BD;_LL2BC: return 26;_LL2BD: if(_tmp3B1 <= (void*)4)goto _LL28A;
if(*((int*)_tmp3B1)!= 17)goto _LL28A;_LL2BE: return 27;_LL28A:;}int Cyc_Tcutil_typecmp(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == 
t2)return 0;{int _tmp3B2=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp3B2 != 0)return _tmp3B2;{struct _tuple6 _tmp3B4=({struct _tuple6 _tmp3B3;
_tmp3B3.f1=t2;_tmp3B3.f2=t1;_tmp3B3;});void*_tmp3B5;void*_tmp3B6;void*_tmp3B7;
struct Cyc_Absyn_Tvar*_tmp3B8;void*_tmp3B9;struct Cyc_Absyn_Tvar*_tmp3BA;void*
_tmp3BB;struct Cyc_Absyn_AggrInfo _tmp3BC;union Cyc_Absyn_AggrInfoU_union _tmp3BD;
struct Cyc_List_List*_tmp3BE;void*_tmp3BF;struct Cyc_Absyn_AggrInfo _tmp3C0;union
Cyc_Absyn_AggrInfoU_union _tmp3C1;struct Cyc_List_List*_tmp3C2;void*_tmp3C3;struct
_tuple1*_tmp3C4;void*_tmp3C5;struct _tuple1*_tmp3C6;void*_tmp3C7;struct Cyc_List_List*
_tmp3C8;void*_tmp3C9;struct Cyc_List_List*_tmp3CA;void*_tmp3CB;struct Cyc_Absyn_TunionInfo
_tmp3CC;union Cyc_Absyn_TunionInfoU_union _tmp3CD;struct Cyc_Absyn_Tuniondecl**
_tmp3CE;struct Cyc_Absyn_Tuniondecl*_tmp3CF;struct Cyc_List_List*_tmp3D0;struct Cyc_Core_Opt*
_tmp3D1;void*_tmp3D2;struct Cyc_Absyn_TunionInfo _tmp3D3;union Cyc_Absyn_TunionInfoU_union
_tmp3D4;struct Cyc_Absyn_Tuniondecl**_tmp3D5;struct Cyc_Absyn_Tuniondecl*_tmp3D6;
struct Cyc_List_List*_tmp3D7;struct Cyc_Core_Opt*_tmp3D8;void*_tmp3D9;struct Cyc_Absyn_TunionFieldInfo
_tmp3DA;union Cyc_Absyn_TunionFieldInfoU_union _tmp3DB;struct Cyc_Absyn_Tuniondecl*
_tmp3DC;struct Cyc_Absyn_Tunionfield*_tmp3DD;struct Cyc_List_List*_tmp3DE;void*
_tmp3DF;struct Cyc_Absyn_TunionFieldInfo _tmp3E0;union Cyc_Absyn_TunionFieldInfoU_union
_tmp3E1;struct Cyc_Absyn_Tuniondecl*_tmp3E2;struct Cyc_Absyn_Tunionfield*_tmp3E3;
struct Cyc_List_List*_tmp3E4;void*_tmp3E5;struct Cyc_Absyn_PtrInfo _tmp3E6;void*
_tmp3E7;struct Cyc_Absyn_Tqual _tmp3E8;struct Cyc_Absyn_PtrAtts _tmp3E9;void*_tmp3EA;
struct Cyc_Absyn_Conref*_tmp3EB;struct Cyc_Absyn_Conref*_tmp3EC;struct Cyc_Absyn_Conref*
_tmp3ED;void*_tmp3EE;struct Cyc_Absyn_PtrInfo _tmp3EF;void*_tmp3F0;struct Cyc_Absyn_Tqual
_tmp3F1;struct Cyc_Absyn_PtrAtts _tmp3F2;void*_tmp3F3;struct Cyc_Absyn_Conref*
_tmp3F4;struct Cyc_Absyn_Conref*_tmp3F5;struct Cyc_Absyn_Conref*_tmp3F6;void*
_tmp3F7;void*_tmp3F8;void*_tmp3F9;void*_tmp3FA;void*_tmp3FB;void*_tmp3FC;void*
_tmp3FD;void*_tmp3FE;void*_tmp3FF;int _tmp400;void*_tmp401;int _tmp402;void*
_tmp403;struct Cyc_Absyn_ArrayInfo _tmp404;void*_tmp405;struct Cyc_Absyn_Tqual
_tmp406;struct Cyc_Absyn_Exp*_tmp407;struct Cyc_Absyn_Conref*_tmp408;void*_tmp409;
struct Cyc_Absyn_ArrayInfo _tmp40A;void*_tmp40B;struct Cyc_Absyn_Tqual _tmp40C;
struct Cyc_Absyn_Exp*_tmp40D;struct Cyc_Absyn_Conref*_tmp40E;void*_tmp40F;struct
Cyc_Absyn_FnInfo _tmp410;struct Cyc_List_List*_tmp411;struct Cyc_Core_Opt*_tmp412;
void*_tmp413;struct Cyc_List_List*_tmp414;int _tmp415;struct Cyc_Absyn_VarargInfo*
_tmp416;struct Cyc_List_List*_tmp417;struct Cyc_List_List*_tmp418;void*_tmp419;
struct Cyc_Absyn_FnInfo _tmp41A;struct Cyc_List_List*_tmp41B;struct Cyc_Core_Opt*
_tmp41C;void*_tmp41D;struct Cyc_List_List*_tmp41E;int _tmp41F;struct Cyc_Absyn_VarargInfo*
_tmp420;struct Cyc_List_List*_tmp421;struct Cyc_List_List*_tmp422;void*_tmp423;
struct Cyc_List_List*_tmp424;void*_tmp425;struct Cyc_List_List*_tmp426;void*
_tmp427;void*_tmp428;struct Cyc_List_List*_tmp429;void*_tmp42A;void*_tmp42B;
struct Cyc_List_List*_tmp42C;void*_tmp42D;void*_tmp42E;void*_tmp42F;void*_tmp430;
void*_tmp431;void*_tmp432;void*_tmp433;void*_tmp434;void*_tmp435;void*_tmp436;
void*_tmp437;void*_tmp438;void*_tmp439;int _tmp43A;void*_tmp43B;int _tmp43C;void*
_tmp43D;void*_tmp43E;void*_tmp43F;void*_tmp440;void*_tmp441;void*_tmp442;_LL2C0:
_tmp3B5=_tmp3B4.f1;if(_tmp3B5 <= (void*)4)goto _LL2C2;if(*((int*)_tmp3B5)!= 0)goto
_LL2C2;_tmp3B6=_tmp3B4.f2;if(_tmp3B6 <= (void*)4)goto _LL2C2;if(*((int*)_tmp3B6)!= 
0)goto _LL2C2;_LL2C1:({void*_tmp443[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp444="typecmp: can only compare closed types";
_tag_dynforward(_tmp444,sizeof(char),_get_zero_arr_size(_tmp444,39));}),
_tag_dynforward(_tmp443,sizeof(void*),0));});_LL2C2: _tmp3B7=_tmp3B4.f1;if(
_tmp3B7 <= (void*)4)goto _LL2C4;if(*((int*)_tmp3B7)!= 1)goto _LL2C4;_tmp3B8=((
struct Cyc_Absyn_VarType_struct*)_tmp3B7)->f1;_tmp3B9=_tmp3B4.f2;if(_tmp3B9 <= (
void*)4)goto _LL2C4;if(*((int*)_tmp3B9)!= 1)goto _LL2C4;_tmp3BA=((struct Cyc_Absyn_VarType_struct*)
_tmp3B9)->f1;_LL2C3: return Cyc_Core_intcmp(*((int*)_check_null(_tmp3BA->identity)),*((
int*)_check_null(_tmp3B8->identity)));_LL2C4: _tmp3BB=_tmp3B4.f1;if(_tmp3BB <= (
void*)4)goto _LL2C6;if(*((int*)_tmp3BB)!= 10)goto _LL2C6;_tmp3BC=((struct Cyc_Absyn_AggrType_struct*)
_tmp3BB)->f1;_tmp3BD=_tmp3BC.aggr_info;_tmp3BE=_tmp3BC.targs;_tmp3BF=_tmp3B4.f2;
if(_tmp3BF <= (void*)4)goto _LL2C6;if(*((int*)_tmp3BF)!= 10)goto _LL2C6;_tmp3C0=((
struct Cyc_Absyn_AggrType_struct*)_tmp3BF)->f1;_tmp3C1=_tmp3C0.aggr_info;_tmp3C2=
_tmp3C0.targs;_LL2C5: {struct _tuple1*_tmp446;struct _tuple5 _tmp445=Cyc_Absyn_aggr_kinded_name(
_tmp3BD);_tmp446=_tmp445.f2;{struct _tuple1*_tmp448;struct _tuple5 _tmp447=Cyc_Absyn_aggr_kinded_name(
_tmp3C1);_tmp448=_tmp447.f2;{int _tmp449=Cyc_Absyn_qvar_cmp(_tmp446,_tmp448);if(
_tmp449 != 0)return _tmp449;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3BE,_tmp3C2);}}}}_LL2C6: _tmp3C3=_tmp3B4.f1;if(_tmp3C3 <= (void*)4)goto _LL2C8;
if(*((int*)_tmp3C3)!= 12)goto _LL2C8;_tmp3C4=((struct Cyc_Absyn_EnumType_struct*)
_tmp3C3)->f1;_tmp3C5=_tmp3B4.f2;if(_tmp3C5 <= (void*)4)goto _LL2C8;if(*((int*)
_tmp3C5)!= 12)goto _LL2C8;_tmp3C6=((struct Cyc_Absyn_EnumType_struct*)_tmp3C5)->f1;
_LL2C7: return Cyc_Absyn_qvar_cmp(_tmp3C4,_tmp3C6);_LL2C8: _tmp3C7=_tmp3B4.f1;if(
_tmp3C7 <= (void*)4)goto _LL2CA;if(*((int*)_tmp3C7)!= 13)goto _LL2CA;_tmp3C8=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp3C7)->f1;_tmp3C9=_tmp3B4.f2;if(_tmp3C9
<= (void*)4)goto _LL2CA;if(*((int*)_tmp3C9)!= 13)goto _LL2CA;_tmp3CA=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3C9)->f1;_LL2C9: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct
Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp3C8,_tmp3CA);_LL2CA: _tmp3CB=_tmp3B4.f1;if(_tmp3CB <= (
void*)4)goto _LL2CC;if(*((int*)_tmp3CB)!= 2)goto _LL2CC;_tmp3CC=((struct Cyc_Absyn_TunionType_struct*)
_tmp3CB)->f1;_tmp3CD=_tmp3CC.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp3CB)->f1).tunion_info).KnownTunion).tag != 1)goto _LL2CC;_tmp3CE=(_tmp3CD.KnownTunion).f1;
_tmp3CF=*_tmp3CE;_tmp3D0=_tmp3CC.targs;_tmp3D1=_tmp3CC.rgn;_tmp3D2=_tmp3B4.f2;
if(_tmp3D2 <= (void*)4)goto _LL2CC;if(*((int*)_tmp3D2)!= 2)goto _LL2CC;_tmp3D3=((
struct Cyc_Absyn_TunionType_struct*)_tmp3D2)->f1;_tmp3D4=_tmp3D3.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp3D2)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL2CC;_tmp3D5=(_tmp3D4.KnownTunion).f1;_tmp3D6=*_tmp3D5;_tmp3D7=_tmp3D3.targs;
_tmp3D8=_tmp3D3.rgn;_LL2CB: if(_tmp3D6 == _tmp3CF)return 0;{int _tmp44A=Cyc_Absyn_qvar_cmp(
_tmp3D6->name,_tmp3CF->name);if(_tmp44A != 0)return _tmp44A;if((unsigned int)
_tmp3D8  && (unsigned int)_tmp3D1){int _tmp44B=Cyc_Tcutil_typecmp((void*)_tmp3D8->v,(
void*)_tmp3D1->v);if(_tmp44B != 0)return _tmp44B;}else{if((unsigned int)_tmp3D8)
return - 1;else{return 1;}}return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,_tmp3D7,
_tmp3D0);}_LL2CC: _tmp3D9=_tmp3B4.f1;if(_tmp3D9 <= (void*)4)goto _LL2CE;if(*((int*)
_tmp3D9)!= 3)goto _LL2CE;_tmp3DA=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3D9)->f1;_tmp3DB=_tmp3DA.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3D9)->f1).field_info).KnownTunionfield).tag != 1)goto _LL2CE;_tmp3DC=(_tmp3DB.KnownTunionfield).f1;
_tmp3DD=(_tmp3DB.KnownTunionfield).f2;_tmp3DE=_tmp3DA.targs;_tmp3DF=_tmp3B4.f2;
if(_tmp3DF <= (void*)4)goto _LL2CE;if(*((int*)_tmp3DF)!= 3)goto _LL2CE;_tmp3E0=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp3DF)->f1;_tmp3E1=_tmp3E0.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp3DF)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL2CE;_tmp3E2=(_tmp3E1.KnownTunionfield).f1;_tmp3E3=(_tmp3E1.KnownTunionfield).f2;
_tmp3E4=_tmp3E0.targs;_LL2CD: if(_tmp3E2 == _tmp3DC)return 0;{int _tmp44C=Cyc_Absyn_qvar_cmp(
_tmp3DC->name,_tmp3E2->name);if(_tmp44C != 0)return _tmp44C;{int _tmp44D=Cyc_Absyn_qvar_cmp(
_tmp3DD->name,_tmp3E3->name);if(_tmp44D != 0)return _tmp44D;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp3E4,_tmp3DE);}}_LL2CE: _tmp3E5=_tmp3B4.f1;if(_tmp3E5 <= (
void*)4)goto _LL2D0;if(*((int*)_tmp3E5)!= 4)goto _LL2D0;_tmp3E6=((struct Cyc_Absyn_PointerType_struct*)
_tmp3E5)->f1;_tmp3E7=(void*)_tmp3E6.elt_typ;_tmp3E8=_tmp3E6.elt_tq;_tmp3E9=
_tmp3E6.ptr_atts;_tmp3EA=(void*)_tmp3E9.rgn;_tmp3EB=_tmp3E9.nullable;_tmp3EC=
_tmp3E9.bounds;_tmp3ED=_tmp3E9.zero_term;_tmp3EE=_tmp3B4.f2;if(_tmp3EE <= (void*)
4)goto _LL2D0;if(*((int*)_tmp3EE)!= 4)goto _LL2D0;_tmp3EF=((struct Cyc_Absyn_PointerType_struct*)
_tmp3EE)->f1;_tmp3F0=(void*)_tmp3EF.elt_typ;_tmp3F1=_tmp3EF.elt_tq;_tmp3F2=
_tmp3EF.ptr_atts;_tmp3F3=(void*)_tmp3F2.rgn;_tmp3F4=_tmp3F2.nullable;_tmp3F5=
_tmp3F2.bounds;_tmp3F6=_tmp3F2.zero_term;_LL2CF: {int _tmp44E=Cyc_Tcutil_typecmp(
_tmp3F0,_tmp3E7);if(_tmp44E != 0)return _tmp44E;{int _tmp44F=Cyc_Tcutil_typecmp(
_tmp3F3,_tmp3EA);if(_tmp44F != 0)return _tmp44F;{int _tmp450=Cyc_Tcutil_tqual_cmp(
_tmp3F1,_tmp3E8);if(_tmp450 != 0)return _tmp450;{int _tmp451=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp3F5,_tmp3EC);if(_tmp451 != 0)return _tmp451;{int _tmp452=((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3F6,_tmp3ED);if(_tmp452 != 0)return _tmp452;{union Cyc_Absyn_Constraint_union
_tmp453=(Cyc_Absyn_compress_conref(_tmp3F5))->v;void*_tmp454;void*_tmp455;_LL2F5:
if((_tmp453.Eq_constr).tag != 0)goto _LL2F7;_tmp454=(_tmp453.Eq_constr).f1;if((int)
_tmp454 != 0)goto _LL2F7;_LL2F6: return 0;_LL2F7: if((_tmp453.Eq_constr).tag != 0)goto
_LL2F9;_tmp455=(_tmp453.Eq_constr).f1;if((int)_tmp455 != 1)goto _LL2F9;_LL2F8:
return 0;_LL2F9:;_LL2FA: goto _LL2F4;_LL2F4:;}return((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,
_tmp3F4,_tmp3EB);}}}}}_LL2D0: _tmp3F7=_tmp3B4.f1;if(_tmp3F7 <= (void*)4)goto _LL2D2;
if(*((int*)_tmp3F7)!= 5)goto _LL2D2;_tmp3F8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3F7)->f1;_tmp3F9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3F7)->f2;
_tmp3FA=_tmp3B4.f2;if(_tmp3FA <= (void*)4)goto _LL2D2;if(*((int*)_tmp3FA)!= 5)goto
_LL2D2;_tmp3FB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3FA)->f1;_tmp3FC=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp3FA)->f2;_LL2D1: if(_tmp3FB != _tmp3F8)
return Cyc_Core_intcmp((int)((unsigned int)_tmp3FB),(int)((unsigned int)_tmp3F8));
if(_tmp3FC != _tmp3F9)return Cyc_Core_intcmp((int)((unsigned int)_tmp3FC),(int)((
unsigned int)_tmp3F9));return 0;_LL2D2: _tmp3FD=_tmp3B4.f1;if((int)_tmp3FD != 1)
goto _LL2D4;_tmp3FE=_tmp3B4.f2;if((int)_tmp3FE != 1)goto _LL2D4;_LL2D3: return 0;
_LL2D4: _tmp3FF=_tmp3B4.f1;if(_tmp3FF <= (void*)4)goto _LL2D6;if(*((int*)_tmp3FF)!= 
6)goto _LL2D6;_tmp400=((struct Cyc_Absyn_DoubleType_struct*)_tmp3FF)->f1;_tmp401=
_tmp3B4.f2;if(_tmp401 <= (void*)4)goto _LL2D6;if(*((int*)_tmp401)!= 6)goto _LL2D6;
_tmp402=((struct Cyc_Absyn_DoubleType_struct*)_tmp401)->f1;_LL2D5: if(_tmp400 == 
_tmp402)return 0;else{if(_tmp400)return - 1;else{return 1;}}_LL2D6: _tmp403=_tmp3B4.f1;
if(_tmp403 <= (void*)4)goto _LL2D8;if(*((int*)_tmp403)!= 7)goto _LL2D8;_tmp404=((
struct Cyc_Absyn_ArrayType_struct*)_tmp403)->f1;_tmp405=(void*)_tmp404.elt_type;
_tmp406=_tmp404.tq;_tmp407=_tmp404.num_elts;_tmp408=_tmp404.zero_term;_tmp409=
_tmp3B4.f2;if(_tmp409 <= (void*)4)goto _LL2D8;if(*((int*)_tmp409)!= 7)goto _LL2D8;
_tmp40A=((struct Cyc_Absyn_ArrayType_struct*)_tmp409)->f1;_tmp40B=(void*)_tmp40A.elt_type;
_tmp40C=_tmp40A.tq;_tmp40D=_tmp40A.num_elts;_tmp40E=_tmp40A.zero_term;_LL2D7: {
int _tmp456=Cyc_Tcutil_tqual_cmp(_tmp40C,_tmp406);if(_tmp456 != 0)return _tmp456;{
int _tmp457=Cyc_Tcutil_typecmp(_tmp40B,_tmp405);if(_tmp457 != 0)return _tmp457;{int
_tmp458=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp408,_tmp40E);if(_tmp458 != 0)
return _tmp458;if(_tmp407 == _tmp40D)return 0;if(_tmp407 == 0  || _tmp40D == 0)({void*
_tmp459[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp45A="missing expression in array index";_tag_dynforward(_tmp45A,
sizeof(char),_get_zero_arr_size(_tmp45A,34));}),_tag_dynforward(_tmp459,sizeof(
void*),0));});return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp407,_tmp40D);}}}_LL2D8: _tmp40F=_tmp3B4.f1;if(_tmp40F <= (void*)4)goto _LL2DA;
if(*((int*)_tmp40F)!= 8)goto _LL2DA;_tmp410=((struct Cyc_Absyn_FnType_struct*)
_tmp40F)->f1;_tmp411=_tmp410.tvars;_tmp412=_tmp410.effect;_tmp413=(void*)_tmp410.ret_typ;
_tmp414=_tmp410.args;_tmp415=_tmp410.c_varargs;_tmp416=_tmp410.cyc_varargs;
_tmp417=_tmp410.rgn_po;_tmp418=_tmp410.attributes;_tmp419=_tmp3B4.f2;if(_tmp419
<= (void*)4)goto _LL2DA;if(*((int*)_tmp419)!= 8)goto _LL2DA;_tmp41A=((struct Cyc_Absyn_FnType_struct*)
_tmp419)->f1;_tmp41B=_tmp41A.tvars;_tmp41C=_tmp41A.effect;_tmp41D=(void*)_tmp41A.ret_typ;
_tmp41E=_tmp41A.args;_tmp41F=_tmp41A.c_varargs;_tmp420=_tmp41A.cyc_varargs;
_tmp421=_tmp41A.rgn_po;_tmp422=_tmp41A.attributes;_LL2D9:({void*_tmp45B[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp45C="typecmp: function types not handled";_tag_dynforward(_tmp45C,
sizeof(char),_get_zero_arr_size(_tmp45C,36));}),_tag_dynforward(_tmp45B,sizeof(
void*),0));});_LL2DA: _tmp423=_tmp3B4.f1;if(_tmp423 <= (void*)4)goto _LL2DC;if(*((
int*)_tmp423)!= 9)goto _LL2DC;_tmp424=((struct Cyc_Absyn_TupleType_struct*)_tmp423)->f1;
_tmp425=_tmp3B4.f2;if(_tmp425 <= (void*)4)goto _LL2DC;if(*((int*)_tmp425)!= 9)goto
_LL2DC;_tmp426=((struct Cyc_Absyn_TupleType_struct*)_tmp425)->f1;_LL2DB: return((
int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp426,_tmp424);_LL2DC: _tmp427=
_tmp3B4.f1;if(_tmp427 <= (void*)4)goto _LL2DE;if(*((int*)_tmp427)!= 11)goto _LL2DE;
_tmp428=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp427)->f1;_tmp429=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp427)->f2;_tmp42A=_tmp3B4.f2;if(_tmp42A
<= (void*)4)goto _LL2DE;if(*((int*)_tmp42A)!= 11)goto _LL2DE;_tmp42B=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp42A)->f1;_tmp42C=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp42A)->f2;_LL2DD: if(_tmp42B != _tmp428){if(_tmp42B == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp42C,_tmp429);_LL2DE: _tmp42D=_tmp3B4.f1;if(_tmp42D <= (void*)4)goto _LL2E0;if(*((
int*)_tmp42D)!= 15)goto _LL2E0;_tmp42E=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp42D)->f1;_tmp42F=_tmp3B4.f2;if(_tmp42F <= (void*)4)goto _LL2E0;if(*((int*)
_tmp42F)!= 15)goto _LL2E0;_tmp430=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp42F)->f1;_LL2DF: return Cyc_Tcutil_typecmp(_tmp42E,_tmp430);_LL2E0: _tmp431=
_tmp3B4.f1;if(_tmp431 <= (void*)4)goto _LL2E2;if(*((int*)_tmp431)!= 14)goto _LL2E2;
_tmp432=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp431)->f1;_tmp433=_tmp3B4.f2;
if(_tmp433 <= (void*)4)goto _LL2E2;if(*((int*)_tmp433)!= 14)goto _LL2E2;_tmp434=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp433)->f1;_LL2E1: return Cyc_Tcutil_typecmp(
_tmp432,_tmp434);_LL2E2: _tmp435=_tmp3B4.f1;if(_tmp435 <= (void*)4)goto _LL2E4;if(*((
int*)_tmp435)!= 17)goto _LL2E4;_tmp436=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp435)->f1;_tmp437=_tmp3B4.f2;if(_tmp437 <= (void*)4)goto _LL2E4;if(*((int*)
_tmp437)!= 17)goto _LL2E4;_tmp438=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp437)->f1;_LL2E3: return Cyc_Tcutil_typecmp(_tmp436,_tmp438);_LL2E4: _tmp439=
_tmp3B4.f1;if(_tmp439 <= (void*)4)goto _LL2E6;if(*((int*)_tmp439)!= 18)goto _LL2E6;
_tmp43A=((struct Cyc_Absyn_TypeInt_struct*)_tmp439)->f1;_tmp43B=_tmp3B4.f2;if(
_tmp43B <= (void*)4)goto _LL2E6;if(*((int*)_tmp43B)!= 18)goto _LL2E6;_tmp43C=((
struct Cyc_Absyn_TypeInt_struct*)_tmp43B)->f1;_LL2E5: return Cyc_Core_intcmp(
_tmp43A,_tmp43C);_LL2E6: _tmp43D=_tmp3B4.f1;if(_tmp43D <= (void*)4)goto _LL2E8;if(*((
int*)_tmp43D)!= 20)goto _LL2E8;_LL2E7: goto _LL2E9;_LL2E8: _tmp43E=_tmp3B4.f2;if(
_tmp43E <= (void*)4)goto _LL2EA;if(*((int*)_tmp43E)!= 20)goto _LL2EA;_LL2E9: goto
_LL2EB;_LL2EA: _tmp43F=_tmp3B4.f1;if(_tmp43F <= (void*)4)goto _LL2EC;if(*((int*)
_tmp43F)!= 19)goto _LL2EC;_LL2EB: goto _LL2ED;_LL2EC: _tmp440=_tmp3B4.f1;if(_tmp440
<= (void*)4)goto _LL2EE;if(*((int*)_tmp440)!= 21)goto _LL2EE;_LL2ED: goto _LL2EF;
_LL2EE: _tmp441=_tmp3B4.f2;if(_tmp441 <= (void*)4)goto _LL2F0;if(*((int*)_tmp441)!= 
21)goto _LL2F0;_LL2EF: goto _LL2F1;_LL2F0: _tmp442=_tmp3B4.f2;if(_tmp442 <= (void*)4)
goto _LL2F2;if(*((int*)_tmp442)!= 19)goto _LL2F2;_LL2F1:({void*_tmp45D[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp45E="typecmp: effects not handled";_tag_dynforward(_tmp45E,sizeof(char),
_get_zero_arr_size(_tmp45E,29));}),_tag_dynforward(_tmp45D,sizeof(void*),0));});
_LL2F2:;_LL2F3:({void*_tmp45F[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp460="Unmatched case in typecmp";
_tag_dynforward(_tmp460,sizeof(char),_get_zero_arr_size(_tmp460,26));}),
_tag_dynforward(_tmp45F,sizeof(void*),0));});_LL2BF:;}}}int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp461=Cyc_Tcutil_compress(t);_LL2FC: if(_tmp461 <= (void*)4)goto
_LL2FE;if(*((int*)_tmp461)!= 5)goto _LL2FE;_LL2FD: goto _LL2FF;_LL2FE: if((int)
_tmp461 != 1)goto _LL300;_LL2FF: goto _LL301;_LL300: if(_tmp461 <= (void*)4)goto _LL306;
if(*((int*)_tmp461)!= 6)goto _LL302;_LL301: goto _LL303;_LL302: if(*((int*)_tmp461)
!= 12)goto _LL304;_LL303: goto _LL305;_LL304: if(*((int*)_tmp461)!= 13)goto _LL306;
_LL305: return 1;_LL306:;_LL307: return 0;_LL2FB:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple6 _tmp463=({struct _tuple6 _tmp462;_tmp462.f1=t1;_tmp462.f2=t2;_tmp462;});
void*_tmp464;int _tmp465;void*_tmp466;int _tmp467;void*_tmp468;void*_tmp469;void*
_tmp46A;void*_tmp46B;void*_tmp46C;void*_tmp46D;void*_tmp46E;void*_tmp46F;void*
_tmp470;void*_tmp471;void*_tmp472;void*_tmp473;void*_tmp474;void*_tmp475;void*
_tmp476;void*_tmp477;void*_tmp478;void*_tmp479;void*_tmp47A;void*_tmp47B;void*
_tmp47C;void*_tmp47D;void*_tmp47E;void*_tmp47F;void*_tmp480;void*_tmp481;void*
_tmp482;void*_tmp483;void*_tmp484;void*_tmp485;void*_tmp486;void*_tmp487;void*
_tmp488;void*_tmp489;void*_tmp48A;void*_tmp48B;void*_tmp48C;void*_tmp48D;void*
_tmp48E;void*_tmp48F;void*_tmp490;void*_tmp491;void*_tmp492;void*_tmp493;void*
_tmp494;void*_tmp495;void*_tmp496;void*_tmp497;void*_tmp498;void*_tmp499;void*
_tmp49A;void*_tmp49B;void*_tmp49C;void*_tmp49D;void*_tmp49E;void*_tmp49F;void*
_tmp4A0;void*_tmp4A1;void*_tmp4A2;void*_tmp4A3;void*_tmp4A4;void*_tmp4A5;void*
_tmp4A6;void*_tmp4A7;void*_tmp4A8;void*_tmp4A9;_LL309: _tmp464=_tmp463.f1;if(
_tmp464 <= (void*)4)goto _LL30B;if(*((int*)_tmp464)!= 6)goto _LL30B;_tmp465=((
struct Cyc_Absyn_DoubleType_struct*)_tmp464)->f1;_tmp466=_tmp463.f2;if(_tmp466 <= (
void*)4)goto _LL30B;if(*((int*)_tmp466)!= 6)goto _LL30B;_tmp467=((struct Cyc_Absyn_DoubleType_struct*)
_tmp466)->f1;_LL30A: return !_tmp467  && _tmp465;_LL30B: _tmp468=_tmp463.f1;if(
_tmp468 <= (void*)4)goto _LL30D;if(*((int*)_tmp468)!= 6)goto _LL30D;_tmp469=_tmp463.f2;
if((int)_tmp469 != 1)goto _LL30D;_LL30C: goto _LL30E;_LL30D: _tmp46A=_tmp463.f1;if(
_tmp46A <= (void*)4)goto _LL30F;if(*((int*)_tmp46A)!= 6)goto _LL30F;_tmp46B=_tmp463.f2;
if(_tmp46B <= (void*)4)goto _LL30F;if(*((int*)_tmp46B)!= 5)goto _LL30F;_LL30E: goto
_LL310;_LL30F: _tmp46C=_tmp463.f1;if(_tmp46C <= (void*)4)goto _LL311;if(*((int*)
_tmp46C)!= 6)goto _LL311;_tmp46D=_tmp463.f2;if(_tmp46D <= (void*)4)goto _LL311;if(*((
int*)_tmp46D)!= 14)goto _LL311;_LL310: goto _LL312;_LL311: _tmp46E=_tmp463.f1;if((
int)_tmp46E != 1)goto _LL313;_tmp46F=_tmp463.f2;if(_tmp46F <= (void*)4)goto _LL313;
if(*((int*)_tmp46F)!= 14)goto _LL313;_LL312: goto _LL314;_LL313: _tmp470=_tmp463.f1;
if(_tmp470 <= (void*)4)goto _LL315;if(*((int*)_tmp470)!= 6)goto _LL315;_tmp471=
_tmp463.f2;if(_tmp471 <= (void*)4)goto _LL315;if(*((int*)_tmp471)!= 17)goto _LL315;
_LL314: goto _LL316;_LL315: _tmp472=_tmp463.f1;if((int)_tmp472 != 1)goto _LL317;
_tmp473=_tmp463.f2;if(_tmp473 <= (void*)4)goto _LL317;if(*((int*)_tmp473)!= 17)
goto _LL317;_LL316: goto _LL318;_LL317: _tmp474=_tmp463.f1;if((int)_tmp474 != 1)goto
_LL319;_tmp475=_tmp463.f2;if(_tmp475 <= (void*)4)goto _LL319;if(*((int*)_tmp475)!= 
5)goto _LL319;_LL318: return 1;_LL319: _tmp476=_tmp463.f1;if(_tmp476 <= (void*)4)goto
_LL31B;if(*((int*)_tmp476)!= 5)goto _LL31B;_tmp477=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp476)->f2;if((int)_tmp477 != 4)goto _LL31B;_tmp478=_tmp463.f2;if(_tmp478 <= (
void*)4)goto _LL31B;if(*((int*)_tmp478)!= 5)goto _LL31B;_tmp479=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp478)->f2;if((int)_tmp479 != 4)goto _LL31B;_LL31A: return 0;_LL31B: _tmp47A=
_tmp463.f1;if(_tmp47A <= (void*)4)goto _LL31D;if(*((int*)_tmp47A)!= 5)goto _LL31D;
_tmp47B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp47A)->f2;if((int)_tmp47B != 
4)goto _LL31D;_LL31C: return 1;_LL31D: _tmp47C=_tmp463.f1;if(_tmp47C <= (void*)4)goto
_LL31F;if(*((int*)_tmp47C)!= 5)goto _LL31F;_tmp47D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp47C)->f2;if((int)_tmp47D != 3)goto _LL31F;_tmp47E=_tmp463.f2;if(_tmp47E <= (
void*)4)goto _LL31F;if(*((int*)_tmp47E)!= 5)goto _LL31F;_tmp47F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp47E)->f2;if((int)_tmp47F != 2)goto _LL31F;_LL31E: goto _LL320;_LL31F: _tmp480=
_tmp463.f1;if(_tmp480 <= (void*)4)goto _LL321;if(*((int*)_tmp480)!= 5)goto _LL321;
_tmp481=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp480)->f2;if((int)_tmp481 != 
2)goto _LL321;_tmp482=_tmp463.f2;if(_tmp482 <= (void*)4)goto _LL321;if(*((int*)
_tmp482)!= 5)goto _LL321;_tmp483=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp482)->f2;
if((int)_tmp483 != 3)goto _LL321;_LL320: return 0;_LL321: _tmp484=_tmp463.f1;if(
_tmp484 <= (void*)4)goto _LL323;if(*((int*)_tmp484)!= 5)goto _LL323;_tmp485=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp484)->f2;if((int)_tmp485 != 3)goto _LL323;
_tmp486=_tmp463.f2;if((int)_tmp486 != 1)goto _LL323;_LL322: goto _LL324;_LL323:
_tmp487=_tmp463.f1;if(_tmp487 <= (void*)4)goto _LL325;if(*((int*)_tmp487)!= 5)goto
_LL325;_tmp488=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp487)->f2;if((int)
_tmp488 != 2)goto _LL325;_tmp489=_tmp463.f2;if((int)_tmp489 != 1)goto _LL325;_LL324:
goto _LL326;_LL325: _tmp48A=_tmp463.f1;if(_tmp48A <= (void*)4)goto _LL327;if(*((int*)
_tmp48A)!= 5)goto _LL327;_tmp48B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48A)->f2;
if((int)_tmp48B != 3)goto _LL327;_tmp48C=_tmp463.f2;if(_tmp48C <= (void*)4)goto
_LL327;if(*((int*)_tmp48C)!= 5)goto _LL327;_tmp48D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48C)->f2;if((int)_tmp48D != 1)goto _LL327;_LL326: goto _LL328;_LL327: _tmp48E=
_tmp463.f1;if(_tmp48E <= (void*)4)goto _LL329;if(*((int*)_tmp48E)!= 5)goto _LL329;
_tmp48F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48E)->f2;if((int)_tmp48F != 
2)goto _LL329;_tmp490=_tmp463.f2;if(_tmp490 <= (void*)4)goto _LL329;if(*((int*)
_tmp490)!= 5)goto _LL329;_tmp491=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp490)->f2;
if((int)_tmp491 != 1)goto _LL329;_LL328: goto _LL32A;_LL329: _tmp492=_tmp463.f1;if(
_tmp492 <= (void*)4)goto _LL32B;if(*((int*)_tmp492)!= 17)goto _LL32B;_tmp493=
_tmp463.f2;if(_tmp493 <= (void*)4)goto _LL32B;if(*((int*)_tmp493)!= 5)goto _LL32B;
_tmp494=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp493)->f2;if((int)_tmp494 != 
1)goto _LL32B;_LL32A: goto _LL32C;_LL32B: _tmp495=_tmp463.f1;if(_tmp495 <= (void*)4)
goto _LL32D;if(*((int*)_tmp495)!= 14)goto _LL32D;_tmp496=_tmp463.f2;if(_tmp496 <= (
void*)4)goto _LL32D;if(*((int*)_tmp496)!= 5)goto _LL32D;_tmp497=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp496)->f2;if((int)_tmp497 != 1)goto _LL32D;_LL32C: goto _LL32E;_LL32D: _tmp498=
_tmp463.f1;if(_tmp498 <= (void*)4)goto _LL32F;if(*((int*)_tmp498)!= 5)goto _LL32F;
_tmp499=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp498)->f2;if((int)_tmp499 != 
3)goto _LL32F;_tmp49A=_tmp463.f2;if(_tmp49A <= (void*)4)goto _LL32F;if(*((int*)
_tmp49A)!= 5)goto _LL32F;_tmp49B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49A)->f2;
if((int)_tmp49B != 0)goto _LL32F;_LL32E: goto _LL330;_LL32F: _tmp49C=_tmp463.f1;if(
_tmp49C <= (void*)4)goto _LL331;if(*((int*)_tmp49C)!= 5)goto _LL331;_tmp49D=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp49C)->f2;if((int)_tmp49D != 2)goto _LL331;
_tmp49E=_tmp463.f2;if(_tmp49E <= (void*)4)goto _LL331;if(*((int*)_tmp49E)!= 5)goto
_LL331;_tmp49F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49E)->f2;if((int)
_tmp49F != 0)goto _LL331;_LL330: goto _LL332;_LL331: _tmp4A0=_tmp463.f1;if(_tmp4A0 <= (
void*)4)goto _LL333;if(*((int*)_tmp4A0)!= 5)goto _LL333;_tmp4A1=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4A0)->f2;if((int)_tmp4A1 != 1)goto _LL333;_tmp4A2=_tmp463.f2;if(_tmp4A2 <= (
void*)4)goto _LL333;if(*((int*)_tmp4A2)!= 5)goto _LL333;_tmp4A3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4A2)->f2;if((int)_tmp4A3 != 0)goto _LL333;_LL332: goto _LL334;_LL333: _tmp4A4=
_tmp463.f1;if(_tmp4A4 <= (void*)4)goto _LL335;if(*((int*)_tmp4A4)!= 17)goto _LL335;
_tmp4A5=_tmp463.f2;if(_tmp4A5 <= (void*)4)goto _LL335;if(*((int*)_tmp4A5)!= 5)goto
_LL335;_tmp4A6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A5)->f2;if((int)
_tmp4A6 != 0)goto _LL335;_LL334: goto _LL336;_LL335: _tmp4A7=_tmp463.f1;if(_tmp4A7 <= (
void*)4)goto _LL337;if(*((int*)_tmp4A7)!= 14)goto _LL337;_tmp4A8=_tmp463.f2;if(
_tmp4A8 <= (void*)4)goto _LL337;if(*((int*)_tmp4A8)!= 5)goto _LL337;_tmp4A9=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp4A8)->f2;if((int)_tmp4A9 != 0)goto _LL337;
_LL336: return 1;_LL337:;_LL338: return 0;_LL308:;}}int Cyc_Tcutil_coerce_list(struct
Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){{struct _RegionHandle _tmp4AA=
_new_region("r");struct _RegionHandle*r=& _tmp4AA;_push_region(r);{struct Cyc_Core_Opt*
max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);
if(Cyc_Tcutil_is_arithmetic_type(t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(
t1,(void*)max_arith_type->v))max_arith_type=({struct Cyc_Core_Opt*_tmp4AB=
_region_malloc(r,sizeof(*_tmp4AB));_tmp4AB->v=(void*)t1;_tmp4AB;});}}}if(
max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){int _tmp4AC=
0;_npop_handler(0);return _tmp4AC;}}};_pop_region(r);}{struct Cyc_List_List*el=es;
for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,
t)){({struct Cyc_String_pa_struct _tmp4B0;_tmp4B0.tag=0;_tmp4B0.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v));{struct Cyc_String_pa_struct
_tmp4AF;_tmp4AF.tag=0;_tmp4AF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp4AD[2]={& _tmp4AF,& _tmp4B0};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)el->hd)->loc,({const char*_tmp4AE="type mismatch: expecting %s but found %s";
_tag_dynforward(_tmp4AE,sizeof(char),_get_zero_arr_size(_tmp4AE,41));}),
_tag_dynforward(_tmp4AD,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp4B1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL33A: if(_tmp4B1 <= (void*)4)goto _LL33C;if(*((int*)_tmp4B1)!= 4)goto _LL33C;
_LL33B: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(void*)3);goto _LL339;
_LL33C:;_LL33D: return 0;_LL339:;}return 1;}int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp4B2=Cyc_Tcutil_compress(t);_LL33F: if(_tmp4B2 <= (void*)4)goto _LL349;if(*((
int*)_tmp4B2)!= 5)goto _LL341;_LL340: goto _LL342;_LL341: if(*((int*)_tmp4B2)!= 14)
goto _LL343;_LL342: goto _LL344;_LL343: if(*((int*)_tmp4B2)!= 17)goto _LL345;_LL344:
goto _LL346;_LL345: if(*((int*)_tmp4B2)!= 12)goto _LL347;_LL346: goto _LL348;_LL347:
if(*((int*)_tmp4B2)!= 13)goto _LL349;_LL348: return 1;_LL349:;_LL34A: return 0;_LL33E:;}
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))
return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*_tmp4B3[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp4B4="integral size mismatch; conversion supplied";
_tag_dynforward(_tmp4B4,sizeof(char),_get_zero_arr_size(_tmp4B4,44));}),
_tag_dynforward(_tmp4B3,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*
_tmp4B5[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp4B6="integral size mismatch; conversion supplied";
_tag_dynforward(_tmp4B6,sizeof(char),_get_zero_arr_size(_tmp4B6,44));}),
_tag_dynforward(_tmp4B5,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);return 1;}return 0;}struct _tuple13{union Cyc_Absyn_Constraint_union f1;
union Cyc_Absyn_Constraint_union f2;};int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple6 _tmp4B8=({struct _tuple6 _tmp4B7;_tmp4B7.f1=
t1;_tmp4B7.f2=t2;_tmp4B7;});void*_tmp4B9;struct Cyc_Absyn_PtrInfo _tmp4BA;void*
_tmp4BB;struct Cyc_Absyn_PtrInfo _tmp4BC;void*_tmp4BD;struct Cyc_Absyn_TunionInfo
_tmp4BE;union Cyc_Absyn_TunionInfoU_union _tmp4BF;struct Cyc_Absyn_Tuniondecl**
_tmp4C0;struct Cyc_Absyn_Tuniondecl*_tmp4C1;struct Cyc_List_List*_tmp4C2;struct Cyc_Core_Opt*
_tmp4C3;struct Cyc_Core_Opt _tmp4C4;void*_tmp4C5;void*_tmp4C6;struct Cyc_Absyn_TunionInfo
_tmp4C7;union Cyc_Absyn_TunionInfoU_union _tmp4C8;struct Cyc_Absyn_Tuniondecl**
_tmp4C9;struct Cyc_Absyn_Tuniondecl*_tmp4CA;struct Cyc_List_List*_tmp4CB;struct Cyc_Core_Opt*
_tmp4CC;struct Cyc_Core_Opt _tmp4CD;void*_tmp4CE;void*_tmp4CF;struct Cyc_Absyn_ArrayInfo
_tmp4D0;void*_tmp4D1;struct Cyc_Absyn_Tqual _tmp4D2;struct Cyc_Absyn_Exp*_tmp4D3;
struct Cyc_Absyn_Conref*_tmp4D4;void*_tmp4D5;struct Cyc_Absyn_ArrayInfo _tmp4D6;
void*_tmp4D7;struct Cyc_Absyn_Tqual _tmp4D8;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_Absyn_Conref*
_tmp4DA;void*_tmp4DB;struct Cyc_Absyn_TunionFieldInfo _tmp4DC;union Cyc_Absyn_TunionFieldInfoU_union
_tmp4DD;struct Cyc_Absyn_Tuniondecl*_tmp4DE;struct Cyc_Absyn_Tunionfield*_tmp4DF;
struct Cyc_List_List*_tmp4E0;void*_tmp4E1;struct Cyc_Absyn_TunionInfo _tmp4E2;union
Cyc_Absyn_TunionInfoU_union _tmp4E3;struct Cyc_Absyn_Tuniondecl**_tmp4E4;struct Cyc_Absyn_Tuniondecl*
_tmp4E5;struct Cyc_List_List*_tmp4E6;void*_tmp4E7;struct Cyc_Absyn_PtrInfo _tmp4E8;
void*_tmp4E9;struct Cyc_Absyn_Tqual _tmp4EA;struct Cyc_Absyn_PtrAtts _tmp4EB;void*
_tmp4EC;struct Cyc_Absyn_Conref*_tmp4ED;struct Cyc_Absyn_Conref*_tmp4EE;struct Cyc_Absyn_Conref*
_tmp4EF;void*_tmp4F0;struct Cyc_Absyn_TunionInfo _tmp4F1;union Cyc_Absyn_TunionInfoU_union
_tmp4F2;struct Cyc_Absyn_Tuniondecl**_tmp4F3;struct Cyc_Absyn_Tuniondecl*_tmp4F4;
struct Cyc_List_List*_tmp4F5;struct Cyc_Core_Opt*_tmp4F6;void*_tmp4F7;void*_tmp4F8;
void*_tmp4F9;void*_tmp4FA;void*_tmp4FB;void*_tmp4FC;void*_tmp4FD;void*_tmp4FE;
_LL34C: _tmp4B9=_tmp4B8.f1;if(_tmp4B9 <= (void*)4)goto _LL34E;if(*((int*)_tmp4B9)!= 
4)goto _LL34E;_tmp4BA=((struct Cyc_Absyn_PointerType_struct*)_tmp4B9)->f1;_tmp4BB=
_tmp4B8.f2;if(_tmp4BB <= (void*)4)goto _LL34E;if(*((int*)_tmp4BB)!= 4)goto _LL34E;
_tmp4BC=((struct Cyc_Absyn_PointerType_struct*)_tmp4BB)->f1;_LL34D: {int okay=1;
if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp4BA.ptr_atts).nullable,(
_tmp4BC.ptr_atts).nullable)){union Cyc_Absyn_Constraint_union _tmp4FF=(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)((_tmp4BA.ptr_atts).nullable))->v;
int _tmp500;_LL35F: if((_tmp4FF.Eq_constr).tag != 0)goto _LL361;_tmp500=(_tmp4FF.Eq_constr).f1;
_LL360: okay=!_tmp500;goto _LL35E;_LL361:;_LL362:({void*_tmp501[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp502="silent_castable conref not eq";_tag_dynforward(_tmp502,sizeof(char),
_get_zero_arr_size(_tmp502,30));}),_tag_dynforward(_tmp501,sizeof(void*),0));});
_LL35E:;}if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,(_tmp4BA.ptr_atts).bounds,(
_tmp4BC.ptr_atts).bounds)){struct _tuple13 _tmp504=({struct _tuple13 _tmp503;_tmp503.f1=(
Cyc_Absyn_compress_conref((_tmp4BA.ptr_atts).bounds))->v;_tmp503.f2=(Cyc_Absyn_compress_conref((
_tmp4BC.ptr_atts).bounds))->v;_tmp503;});union Cyc_Absyn_Constraint_union _tmp505;
void*_tmp506;union Cyc_Absyn_Constraint_union _tmp507;void*_tmp508;union Cyc_Absyn_Constraint_union
_tmp509;_LL364: _tmp505=_tmp504.f1;if(((_tmp504.f1).Eq_constr).tag != 0)goto _LL366;
_tmp506=(_tmp505.Eq_constr).f1;_tmp507=_tmp504.f2;if(((_tmp504.f2).Eq_constr).tag
!= 0)goto _LL366;_tmp508=(_tmp507.Eq_constr).f1;_LL365:{struct _tuple6 _tmp50B=({
struct _tuple6 _tmp50A;_tmp50A.f1=_tmp506;_tmp50A.f2=_tmp508;_tmp50A;});void*
_tmp50C;void*_tmp50D;void*_tmp50E;void*_tmp50F;void*_tmp510;void*_tmp511;void*
_tmp512;void*_tmp513;void*_tmp514;void*_tmp515;void*_tmp516;void*_tmp517;void*
_tmp518;struct Cyc_Absyn_Exp*_tmp519;void*_tmp51A;struct Cyc_Absyn_Exp*_tmp51B;
void*_tmp51C;void*_tmp51D;struct Cyc_Absyn_Exp*_tmp51E;void*_tmp51F;void*_tmp520;
struct Cyc_Absyn_Exp*_tmp521;void*_tmp522;void*_tmp523;void*_tmp524;void*_tmp525;
void*_tmp526;struct Cyc_Absyn_Exp*_tmp527;void*_tmp528;void*_tmp529;void*_tmp52A;
void*_tmp52B;_LL36B: _tmp50C=_tmp50B.f1;if(_tmp50C <= (void*)2)goto _LL36D;if(*((
int*)_tmp50C)!= 0)goto _LL36D;_tmp50D=_tmp50B.f2;if((int)_tmp50D != 0)goto _LL36D;
_LL36C: goto _LL36E;_LL36D: _tmp50E=_tmp50B.f1;if(_tmp50E <= (void*)2)goto _LL36F;if(*((
int*)_tmp50E)!= 0)goto _LL36F;_tmp50F=_tmp50B.f2;if((int)_tmp50F != 1)goto _LL36F;
_LL36E: goto _LL370;_LL36F: _tmp510=_tmp50B.f1;if((int)_tmp510 != 1)goto _LL371;
_tmp511=_tmp50B.f2;if((int)_tmp511 != 1)goto _LL371;_LL370: goto _LL372;_LL371:
_tmp512=_tmp50B.f1;if((int)_tmp512 != 0)goto _LL373;_tmp513=_tmp50B.f2;if((int)
_tmp513 != 1)goto _LL373;_LL372: goto _LL374;_LL373: _tmp514=_tmp50B.f1;if((int)
_tmp514 != 0)goto _LL375;_tmp515=_tmp50B.f2;if((int)_tmp515 != 0)goto _LL375;_LL374:
okay=1;goto _LL36A;_LL375: _tmp516=_tmp50B.f1;if((int)_tmp516 != 1)goto _LL377;
_tmp517=_tmp50B.f2;if((int)_tmp517 != 0)goto _LL377;_LL376: okay=0;goto _LL36A;
_LL377: _tmp518=_tmp50B.f1;if(_tmp518 <= (void*)2)goto _LL379;if(*((int*)_tmp518)!= 
0)goto _LL379;_tmp519=((struct Cyc_Absyn_Upper_b_struct*)_tmp518)->f1;_tmp51A=
_tmp50B.f2;if(_tmp51A <= (void*)2)goto _LL379;if(*((int*)_tmp51A)!= 0)goto _LL379;
_tmp51B=((struct Cyc_Absyn_Upper_b_struct*)_tmp51A)->f1;_LL378: okay=okay  && Cyc_Evexp_lte_const_exp(
_tmp51B,_tmp519);if(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp4BC.ptr_atts).zero_term))({void*_tmp52C[0]={};Cyc_Tcutil_warn(loc,({const
char*_tmp52D="implicit cast to shorter array";_tag_dynforward(_tmp52D,sizeof(
char),_get_zero_arr_size(_tmp52D,31));}),_tag_dynforward(_tmp52C,sizeof(void*),0));});
goto _LL36A;_LL379: _tmp51C=_tmp50B.f1;if((int)_tmp51C != 0)goto _LL37B;_tmp51D=
_tmp50B.f2;if(_tmp51D <= (void*)2)goto _LL37B;if(*((int*)_tmp51D)!= 0)goto _LL37B;
_tmp51E=((struct Cyc_Absyn_Upper_b_struct*)_tmp51D)->f1;_LL37A: _tmp521=_tmp51E;
goto _LL37C;_LL37B: _tmp51F=_tmp50B.f1;if((int)_tmp51F != 1)goto _LL37D;_tmp520=
_tmp50B.f2;if(_tmp520 <= (void*)2)goto _LL37D;if(*((int*)_tmp520)!= 0)goto _LL37D;
_tmp521=((struct Cyc_Absyn_Upper_b_struct*)_tmp520)->f1;_LL37C: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp4BA.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp4BC.ptr_atts).bounds))goto _LL36A;okay=0;goto
_LL36A;_LL37D: _tmp522=_tmp50B.f1;if(_tmp522 <= (void*)2)goto _LL37F;if(*((int*)
_tmp522)!= 1)goto _LL37F;_tmp523=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp522)->f1;_tmp524=_tmp50B.f2;if(_tmp524 <= (void*)2)goto _LL37F;if(*((int*)
_tmp524)!= 1)goto _LL37F;_tmp525=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp524)->f1;_LL37E: if(!Cyc_Tcutil_unify(_tmp523,_tmp525)){struct _tuple6 _tmp52F=({
struct _tuple6 _tmp52E;_tmp52E.f1=Cyc_Tcutil_compress(_tmp523);_tmp52E.f2=Cyc_Tcutil_compress(
_tmp525);_tmp52E;});void*_tmp530;int _tmp531;void*_tmp532;int _tmp533;_LL386:
_tmp530=_tmp52F.f1;if(_tmp530 <= (void*)4)goto _LL388;if(*((int*)_tmp530)!= 18)
goto _LL388;_tmp531=((struct Cyc_Absyn_TypeInt_struct*)_tmp530)->f1;_tmp532=
_tmp52F.f2;if(_tmp532 <= (void*)4)goto _LL388;if(*((int*)_tmp532)!= 18)goto _LL388;
_tmp533=((struct Cyc_Absyn_TypeInt_struct*)_tmp532)->f1;_LL387: if(_tmp531 < 
_tmp533)okay=0;goto _LL385;_LL388:;_LL389: okay=0;goto _LL385;_LL385:;}goto _LL36A;
_LL37F: _tmp526=_tmp50B.f1;if(_tmp526 <= (void*)2)goto _LL381;if(*((int*)_tmp526)!= 
0)goto _LL381;_tmp527=((struct Cyc_Absyn_Upper_b_struct*)_tmp526)->f1;_tmp528=
_tmp50B.f2;if(_tmp528 <= (void*)2)goto _LL381;if(*((int*)_tmp528)!= 1)goto _LL381;
_tmp529=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp528)->f1;_LL380: {
unsigned int _tmp535;int _tmp536;struct _tuple7 _tmp534=Cyc_Evexp_eval_const_uint_exp(
_tmp527);_tmp535=_tmp534.f1;_tmp536=_tmp534.f2;if(!_tmp536){okay=0;goto _LL36A;}{
void*_tmp537=Cyc_Tcutil_compress(_tmp529);int _tmp538;_LL38B: if(_tmp537 <= (void*)
4)goto _LL38D;if(*((int*)_tmp537)!= 18)goto _LL38D;_tmp538=((struct Cyc_Absyn_TypeInt_struct*)
_tmp537)->f1;_LL38C: if(_tmp535 < _tmp538)okay=0;goto _LL38A;_LL38D:;_LL38E: okay=0;
goto _LL38A;_LL38A:;}goto _LL36A;}_LL381: _tmp52A=_tmp50B.f1;if(_tmp52A <= (void*)2)
goto _LL383;if(*((int*)_tmp52A)!= 1)goto _LL383;_LL382: goto _LL384;_LL383: _tmp52B=
_tmp50B.f2;if(_tmp52B <= (void*)2)goto _LL36A;if(*((int*)_tmp52B)!= 1)goto _LL36A;
_LL384: okay=0;goto _LL36A;_LL36A:;}goto _LL363;_LL366: _tmp509=_tmp504.f1;if(((
_tmp504.f1).No_constr).tag != 2)goto _LL368;_LL367: okay=0;goto _LL363;_LL368:;
_LL369:({void*_tmp539[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp53A="silent_castable conrefs didn't unify";
_tag_dynforward(_tmp53A,sizeof(char),_get_zero_arr_size(_tmp53A,37));}),
_tag_dynforward(_tmp539,sizeof(void*),0));});_LL363:;}okay=okay  && Cyc_Tcutil_unify((
void*)_tmp4BA.elt_typ,(void*)_tmp4BC.elt_typ);okay=okay  && (Cyc_Tcutil_unify((
void*)(_tmp4BA.ptr_atts).rgn,(void*)(_tmp4BC.ptr_atts).rgn) || Cyc_Tcenv_region_outlives(
te,(void*)(_tmp4BA.ptr_atts).rgn,(void*)(_tmp4BC.ptr_atts).rgn));okay=okay  && (!(
_tmp4BA.elt_tq).real_const  || (_tmp4BC.elt_tq).real_const);okay=okay  && (((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp4BA.ptr_atts).zero_term,(_tmp4BC.ptr_atts).zero_term) || ((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,(_tmp4BA.ptr_atts).zero_term)
 && (_tmp4BC.elt_tq).real_const);return okay;}_LL34E: _tmp4BD=_tmp4B8.f1;if(
_tmp4BD <= (void*)4)goto _LL350;if(*((int*)_tmp4BD)!= 2)goto _LL350;_tmp4BE=((
struct Cyc_Absyn_TunionType_struct*)_tmp4BD)->f1;_tmp4BF=_tmp4BE.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp4BD)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL350;_tmp4C0=(_tmp4BF.KnownTunion).f1;_tmp4C1=*_tmp4C0;_tmp4C2=_tmp4BE.targs;
_tmp4C3=_tmp4BE.rgn;if(_tmp4C3 == 0)goto _LL350;_tmp4C4=*_tmp4C3;_tmp4C5=(void*)
_tmp4C4.v;_tmp4C6=_tmp4B8.f2;if(_tmp4C6 <= (void*)4)goto _LL350;if(*((int*)_tmp4C6)
!= 2)goto _LL350;_tmp4C7=((struct Cyc_Absyn_TunionType_struct*)_tmp4C6)->f1;
_tmp4C8=_tmp4C7.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)_tmp4C6)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL350;_tmp4C9=(_tmp4C8.KnownTunion).f1;_tmp4CA=*_tmp4C9;_tmp4CB=
_tmp4C7.targs;_tmp4CC=_tmp4C7.rgn;if(_tmp4CC == 0)goto _LL350;_tmp4CD=*_tmp4CC;
_tmp4CE=(void*)_tmp4CD.v;_LL34F: if(_tmp4C1 != _tmp4CA  || !Cyc_Tcenv_region_outlives(
te,_tmp4C5,_tmp4CE))return 0;for(0;_tmp4C2 != 0  && _tmp4CB != 0;(_tmp4C2=_tmp4C2->tl,
_tmp4CB=_tmp4CB->tl)){if(!Cyc_Tcutil_unify((void*)_tmp4C2->hd,(void*)_tmp4CB->hd))
return 0;}if(_tmp4C2 != 0  || _tmp4CB != 0)return 0;return 1;_LL350: _tmp4CF=_tmp4B8.f1;
if(_tmp4CF <= (void*)4)goto _LL352;if(*((int*)_tmp4CF)!= 7)goto _LL352;_tmp4D0=((
struct Cyc_Absyn_ArrayType_struct*)_tmp4CF)->f1;_tmp4D1=(void*)_tmp4D0.elt_type;
_tmp4D2=_tmp4D0.tq;_tmp4D3=_tmp4D0.num_elts;_tmp4D4=_tmp4D0.zero_term;_tmp4D5=
_tmp4B8.f2;if(_tmp4D5 <= (void*)4)goto _LL352;if(*((int*)_tmp4D5)!= 7)goto _LL352;
_tmp4D6=((struct Cyc_Absyn_ArrayType_struct*)_tmp4D5)->f1;_tmp4D7=(void*)_tmp4D6.elt_type;
_tmp4D8=_tmp4D6.tq;_tmp4D9=_tmp4D6.num_elts;_tmp4DA=_tmp4D6.zero_term;_LL351: {
int okay;okay=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4D4,_tmp4DA) && ((_tmp4D3 != 0
 && _tmp4D9 != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp4D3,(
struct Cyc_Absyn_Exp*)_tmp4D9));return(okay  && Cyc_Tcutil_unify(_tmp4D1,_tmp4D7))
 && (!_tmp4D2.real_const  || _tmp4D8.real_const);}_LL352: _tmp4DB=_tmp4B8.f1;if(
_tmp4DB <= (void*)4)goto _LL354;if(*((int*)_tmp4DB)!= 3)goto _LL354;_tmp4DC=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp4DB)->f1;_tmp4DD=_tmp4DC.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp4DB)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL354;_tmp4DE=(_tmp4DD.KnownTunionfield).f1;_tmp4DF=(_tmp4DD.KnownTunionfield).f2;
_tmp4E0=_tmp4DC.targs;_tmp4E1=_tmp4B8.f2;if(_tmp4E1 <= (void*)4)goto _LL354;if(*((
int*)_tmp4E1)!= 2)goto _LL354;_tmp4E2=((struct Cyc_Absyn_TunionType_struct*)
_tmp4E1)->f1;_tmp4E3=_tmp4E2.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp4E1)->f1).tunion_info).KnownTunion).tag != 1)goto _LL354;_tmp4E4=(_tmp4E3.KnownTunion).f1;
_tmp4E5=*_tmp4E4;_tmp4E6=_tmp4E2.targs;_LL353: if((_tmp4DE == _tmp4E5  || Cyc_Absyn_qvar_cmp(
_tmp4DE->name,_tmp4E5->name)== 0) && (_tmp4DF->typs == 0  || _tmp4DE->is_flat)){
for(0;_tmp4E0 != 0  && _tmp4E6 != 0;(_tmp4E0=_tmp4E0->tl,_tmp4E6=_tmp4E6->tl)){if(!
Cyc_Tcutil_unify((void*)_tmp4E0->hd,(void*)_tmp4E6->hd))break;}if(_tmp4E0 == 0
 && _tmp4E6 == 0)return 1;}return 0;_LL354: _tmp4E7=_tmp4B8.f1;if(_tmp4E7 <= (void*)4)
goto _LL356;if(*((int*)_tmp4E7)!= 4)goto _LL356;_tmp4E8=((struct Cyc_Absyn_PointerType_struct*)
_tmp4E7)->f1;_tmp4E9=(void*)_tmp4E8.elt_typ;_tmp4EA=_tmp4E8.elt_tq;_tmp4EB=
_tmp4E8.ptr_atts;_tmp4EC=(void*)_tmp4EB.rgn;_tmp4ED=_tmp4EB.nullable;_tmp4EE=
_tmp4EB.bounds;_tmp4EF=_tmp4EB.zero_term;_tmp4F0=_tmp4B8.f2;if(_tmp4F0 <= (void*)
4)goto _LL356;if(*((int*)_tmp4F0)!= 2)goto _LL356;_tmp4F1=((struct Cyc_Absyn_TunionType_struct*)
_tmp4F0)->f1;_tmp4F2=_tmp4F1.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp4F0)->f1).tunion_info).KnownTunion).tag != 1)goto _LL356;_tmp4F3=(_tmp4F2.KnownTunion).f1;
_tmp4F4=*_tmp4F3;_tmp4F5=_tmp4F1.targs;_tmp4F6=_tmp4F1.rgn;if(!(!_tmp4F4->is_flat))
goto _LL356;_LL355:{void*_tmp53B=Cyc_Tcutil_compress(_tmp4E9);struct Cyc_Absyn_TunionFieldInfo
_tmp53C;union Cyc_Absyn_TunionFieldInfoU_union _tmp53D;struct Cyc_Absyn_Tuniondecl*
_tmp53E;struct Cyc_Absyn_Tunionfield*_tmp53F;struct Cyc_List_List*_tmp540;_LL390:
if(_tmp53B <= (void*)4)goto _LL392;if(*((int*)_tmp53B)!= 3)goto _LL392;_tmp53C=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp53B)->f1;_tmp53D=_tmp53C.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp53B)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL392;_tmp53E=(_tmp53D.KnownTunionfield).f1;_tmp53F=(_tmp53D.KnownTunionfield).f2;
_tmp540=_tmp53C.targs;_LL391: if(!Cyc_Tcutil_unify(_tmp4EC,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4F6))->v) && !Cyc_Tcenv_region_outlives(te,_tmp4EC,(void*)
_tmp4F6->v))return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4ED,Cyc_Absyn_false_conref))
return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,_tmp4EE,Cyc_Absyn_bounds_one_conref))
return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4EF,Cyc_Absyn_false_conref))
return 0;if(Cyc_Absyn_qvar_cmp(_tmp4F4->name,_tmp53E->name)== 0  && _tmp53F->typs
!= 0){int okay=1;for(0;_tmp540 != 0  && _tmp4F5 != 0;(_tmp540=_tmp540->tl,_tmp4F5=
_tmp4F5->tl)){if(!Cyc_Tcutil_unify((void*)_tmp540->hd,(void*)_tmp4F5->hd)){okay=
0;break;}}if((!okay  || _tmp540 != 0) || _tmp4F5 != 0)return 0;return 1;}goto _LL38F;
_LL392:;_LL393: goto _LL38F;_LL38F:;}return 0;_LL356: _tmp4F7=_tmp4B8.f1;if(_tmp4F7
<= (void*)4)goto _LL358;if(*((int*)_tmp4F7)!= 14)goto _LL358;_tmp4F8=_tmp4B8.f2;
if(_tmp4F8 <= (void*)4)goto _LL358;if(*((int*)_tmp4F8)!= 5)goto _LL358;_tmp4F9=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp4F8)->f2;if((int)_tmp4F9 != 2)goto
_LL358;_LL357: goto _LL359;_LL358: _tmp4FA=_tmp4B8.f1;if(_tmp4FA <= (void*)4)goto
_LL35A;if(*((int*)_tmp4FA)!= 14)goto _LL35A;_tmp4FB=_tmp4B8.f2;if(_tmp4FB <= (void*)
4)goto _LL35A;if(*((int*)_tmp4FB)!= 5)goto _LL35A;_tmp4FC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4FB)->f2;if((int)_tmp4FC != 3)goto _LL35A;_LL359: return 1;_LL35A: _tmp4FD=
_tmp4B8.f1;if(_tmp4FD <= (void*)4)goto _LL35C;if(*((int*)_tmp4FD)!= 17)goto _LL35C;
_tmp4FE=_tmp4B8.f2;if(_tmp4FE <= (void*)4)goto _LL35C;if(*((int*)_tmp4FE)!= 5)goto
_LL35C;_LL35B: return 0;_LL35C:;_LL35D: return Cyc_Tcutil_unify(t1,t2);_LL34B:;}}int
Cyc_Tcutil_is_pointer_type(void*t){void*_tmp541=Cyc_Tcutil_compress(t);_LL395:
if(_tmp541 <= (void*)4)goto _LL397;if(*((int*)_tmp541)!= 4)goto _LL397;_LL396:
return 1;_LL397:;_LL398: return 0;_LL394:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*
e){void*_tmp542=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp543;int _tmp544;union Cyc_Absyn_Cnst_union
_tmp545;char _tmp546;union Cyc_Absyn_Cnst_union _tmp547;short _tmp548;union Cyc_Absyn_Cnst_union
_tmp549;long long _tmp54A;void*_tmp54B;struct Cyc_Absyn_Exp*_tmp54C;_LL39A: if(*((
int*)_tmp542)!= 0)goto _LL39C;_tmp543=((struct Cyc_Absyn_Const_e_struct*)_tmp542)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp542)->f1).Int_c).tag != 2)goto _LL39C;
_tmp544=(_tmp543.Int_c).f2;if(_tmp544 != 0)goto _LL39C;_LL39B: goto _LL39D;_LL39C:
if(*((int*)_tmp542)!= 0)goto _LL39E;_tmp545=((struct Cyc_Absyn_Const_e_struct*)
_tmp542)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp542)->f1).Char_c).tag != 
0)goto _LL39E;_tmp546=(_tmp545.Char_c).f2;if(_tmp546 != 0)goto _LL39E;_LL39D: goto
_LL39F;_LL39E: if(*((int*)_tmp542)!= 0)goto _LL3A0;_tmp547=((struct Cyc_Absyn_Const_e_struct*)
_tmp542)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp542)->f1).Short_c).tag != 
1)goto _LL3A0;_tmp548=(_tmp547.Short_c).f2;if(_tmp548 != 0)goto _LL3A0;_LL39F: goto
_LL3A1;_LL3A0: if(*((int*)_tmp542)!= 0)goto _LL3A2;_tmp549=((struct Cyc_Absyn_Const_e_struct*)
_tmp542)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp542)->f1).LongLong_c).tag
!= 3)goto _LL3A2;_tmp54A=(_tmp549.LongLong_c).f2;if(_tmp54A != 0)goto _LL3A2;_LL3A1:
return 1;_LL3A2: if(*((int*)_tmp542)!= 15)goto _LL3A4;_tmp54B=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp542)->f1;_tmp54C=((struct Cyc_Absyn_Cast_e_struct*)_tmp542)->f2;_LL3A3: return
Cyc_Tcutil_is_zero(_tmp54C) && Cyc_Tcutil_admits_zero(_tmp54B);_LL3A4:;_LL3A5:
return 0;_LL399:;}struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt
Cyc_Tcutil_ak={(void*)((void*)0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)
2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)((void*)1)};int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp54D=_cycalloc(sizeof(*_tmp54D));_tmp54D[0]=({struct Cyc_Absyn_Const_e_struct
_tmp54E;_tmp54E.tag=0;_tmp54E.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp54F;(_tmp54F.Null_c).tag=6;_tmp54F;});_tmp54E;});_tmp54D;})));{struct Cyc_List_List*
_tmp550=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_PointerType_struct*
_tmp551=({struct Cyc_Absyn_PointerType_struct*_tmp552=_cycalloc(sizeof(*_tmp552));
_tmp552[0]=({struct Cyc_Absyn_PointerType_struct _tmp553;_tmp553.tag=4;_tmp553.f1=({
struct Cyc_Absyn_PtrInfo _tmp554;_tmp554.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp557=_cycalloc(sizeof(*
_tmp557));_tmp557->v=_tmp550;_tmp557;}));_tmp554.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp554.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp555;_tmp555.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp556=_cycalloc(
sizeof(*_tmp556));_tmp556->v=_tmp550;_tmp556;}));_tmp555.nullable=Cyc_Absyn_true_conref;
_tmp555.bounds=Cyc_Absyn_empty_conref();_tmp555.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp555.ptrloc=0;_tmp555;});_tmp554;});_tmp553;});
_tmp552;});(void*)(((struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)
_tmp551));return Cyc_Tcutil_coerce_arg(te,e1,t2);}}return 0;}struct _dynforward_ptr
Cyc_Tcutil_coercion2string(void*c){void*_tmp558=c;_LL3A7: if((int)_tmp558 != 0)
goto _LL3A9;_LL3A8: return({const char*_tmp559="unknown";_tag_dynforward(_tmp559,
sizeof(char),_get_zero_arr_size(_tmp559,8));});_LL3A9: if((int)_tmp558 != 1)goto
_LL3AB;_LL3AA: return({const char*_tmp55A="no coercion";_tag_dynforward(_tmp55A,
sizeof(char),_get_zero_arr_size(_tmp55A,12));});_LL3AB: if((int)_tmp558 != 2)goto
_LL3AD;_LL3AC: return({const char*_tmp55B="null check";_tag_dynforward(_tmp55B,
sizeof(char),_get_zero_arr_size(_tmp55B,11));});_LL3AD: if((int)_tmp558 != 3)goto
_LL3A6;_LL3AE: return({const char*_tmp55C="other coercion";_tag_dynforward(_tmp55C,
sizeof(char),_get_zero_arr_size(_tmp55C,15));});_LL3A6:;}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*c;if(Cyc_Tcutil_unify(
t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(
t1)){if(Cyc_Tcutil_will_lose_precision(t1,t2))({struct Cyc_String_pa_struct
_tmp560;_tmp560.tag=0;_tmp560.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp55F;_tmp55F.tag=0;
_tmp55F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp55D[2]={& _tmp55F,& _tmp560};Cyc_Tcutil_warn(e->loc,({const char*
_tmp55E="integral size mismatch; %s -> %s conversion supplied";_tag_dynforward(
_tmp55E,sizeof(char),_get_zero_arr_size(_tmp55E,53));}),_tag_dynforward(_tmp55D,
sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)1);return 1;}
else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2,(void*)3);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;
else{if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= (void*)0){Cyc_Tcutil_unchecked_cast(
te,e,t2,c);if(c != (void*)2)({struct Cyc_String_pa_struct _tmp564;_tmp564.tag=0;
_tmp564.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp563;_tmp563.tag=0;_tmp563.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp561[2]={& _tmp563,& _tmp564};Cyc_Tcutil_warn(e->loc,({const char*_tmp562="implicit cast from %s to %s";
_tag_dynforward(_tmp562,sizeof(char),_get_zero_arr_size(_tmp562,28));}),
_tag_dynforward(_tmp561,sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp565=Cyc_Tcutil_compress(t);
_LL3B0: if(_tmp565 <= (void*)4)goto _LL3B2;if(*((int*)_tmp565)!= 5)goto _LL3B2;
_LL3B1: goto _LL3B3;_LL3B2: if((int)_tmp565 != 1)goto _LL3B4;_LL3B3: goto _LL3B5;_LL3B4:
if(_tmp565 <= (void*)4)goto _LL3B6;if(*((int*)_tmp565)!= 6)goto _LL3B6;_LL3B5:
return 1;_LL3B6:;_LL3B7: return 0;_LL3AF:;}static struct _tuple4*Cyc_Tcutil_flatten_typ_f(
struct _tuple9*env,struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp567;
struct _RegionHandle*_tmp568;struct _tuple9 _tmp566=*env;_tmp567=_tmp566.f1;_tmp568=
_tmp566.f2;return({struct _tuple4*_tmp569=_region_malloc(_tmp568,sizeof(*_tmp569));
_tmp569->f1=x->tq;_tmp569->f2=Cyc_Tcutil_rsubstitute(_tmp568,_tmp567,(void*)x->type);
_tmp569;});}static struct _tuple4*Cyc_Tcutil_rcopy_tqt(struct _RegionHandle*r,
struct _tuple4*x){struct Cyc_Absyn_Tqual _tmp56B;void*_tmp56C;struct _tuple4 _tmp56A=*
x;_tmp56B=_tmp56A.f1;_tmp56C=_tmp56A.f2;return({struct _tuple4*_tmp56D=
_region_malloc(r,sizeof(*_tmp56D));_tmp56D->f1=_tmp56B;_tmp56D->f2=_tmp56C;
_tmp56D;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*
r,struct Cyc_Tcenv_Tenv*te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp56E=t1;
struct Cyc_List_List*_tmp56F;struct Cyc_Absyn_AggrInfo _tmp570;union Cyc_Absyn_AggrInfoU_union
_tmp571;struct Cyc_Absyn_Aggrdecl**_tmp572;struct Cyc_Absyn_Aggrdecl*_tmp573;
struct Cyc_List_List*_tmp574;void*_tmp575;struct Cyc_List_List*_tmp576;struct Cyc_Absyn_FnInfo
_tmp577;_LL3B9: if((int)_tmp56E != 0)goto _LL3BB;_LL3BA: return 0;_LL3BB: if(_tmp56E <= (
void*)4)goto _LL3C3;if(*((int*)_tmp56E)!= 9)goto _LL3BD;_tmp56F=((struct Cyc_Absyn_TupleType_struct*)
_tmp56E)->f1;_LL3BC: return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple4*(*f)(struct _RegionHandle*,struct _tuple4*),struct _RegionHandle*env,struct
Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,r,_tmp56F);_LL3BD: if(*((
int*)_tmp56E)!= 10)goto _LL3BF;_tmp570=((struct Cyc_Absyn_AggrType_struct*)_tmp56E)->f1;
_tmp571=_tmp570.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp56E)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL3BF;_tmp572=(_tmp571.KnownAggr).f1;_tmp573=*_tmp572;_tmp574=_tmp570.targs;
_LL3BE: if((((void*)_tmp573->kind == (void*)1  || _tmp573->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp573->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp573->impl))->rgn_po != 0)return({struct Cyc_List_List*_tmp578=
_region_malloc(r,sizeof(*_tmp578));_tmp578->hd=({struct _tuple4*_tmp579=
_region_malloc(r,sizeof(*_tmp579));_tmp579->f1=Cyc_Absyn_empty_tqual(0);_tmp579->f2=
t1;_tmp579;});_tmp578->tl=0;_tmp578;});{struct Cyc_List_List*_tmp57A=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp573->tvs,_tmp574);struct _tuple9 env=({struct _tuple9
_tmp57B;_tmp57B.f1=_tmp57A;_tmp57B.f2=r;_tmp57B;});return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple4*(*f)(struct _tuple9*,struct Cyc_Absyn_Aggrfield*),
struct _tuple9*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp573->impl))->fields);}_LL3BF:
if(*((int*)_tmp56E)!= 11)goto _LL3C1;_tmp575=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp56E)->f1;if((int)_tmp575 != 0)goto _LL3C1;_tmp576=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp56E)->f2;_LL3C0: {struct _tuple9 env=({struct _tuple9 _tmp57C;_tmp57C.f1=0;
_tmp57C.f2=r;_tmp57C;});return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple4*(*f)(struct _tuple9*,struct Cyc_Absyn_Aggrfield*),struct _tuple9*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp576);}
_LL3C1: if(*((int*)_tmp56E)!= 8)goto _LL3C3;_tmp577=((struct Cyc_Absyn_FnType_struct*)
_tmp56E)->f1;_LL3C2: return({struct Cyc_List_List*_tmp57D=_region_malloc(r,sizeof(*
_tmp57D));_tmp57D->hd=({struct _tuple4*_tmp57E=_region_malloc(r,sizeof(*_tmp57E));
_tmp57E->f1=Cyc_Absyn_const_tqual(0);_tmp57E->f2=t1;_tmp57E;});_tmp57D->tl=0;
_tmp57D;});_LL3C3:;_LL3C4: return({struct Cyc_List_List*_tmp57F=_region_malloc(r,
sizeof(*_tmp57F));_tmp57F->hd=({struct _tuple4*_tmp580=_region_malloc(r,sizeof(*
_tmp580));_tmp580->f1=Cyc_Absyn_empty_tqual(0);_tmp580->f2=t1;_tmp580;});_tmp57F->tl=
0;_tmp57F;});_LL3B8:;}}static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,
struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){void*
_tmp581=(void*)t->hd;_LL3C6: if((int)_tmp581 != 16)goto _LL3C8;_LL3C7: goto _LL3C9;
_LL3C8: if((int)_tmp581 != 3)goto _LL3CA;_LL3C9: goto _LL3CB;_LL3CA: if(_tmp581 <= (
void*)17)goto _LL3CC;if(*((int*)_tmp581)!= 4)goto _LL3CC;_LL3CB: continue;_LL3CC:;
_LL3CD: if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;
_LL3C5:;}}for(0;a2 != 0;a2=a2->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple6*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple6*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6
_tmp583=({struct _tuple6 _tmp582;_tmp582.f1=t1;_tmp582.f2=t2;_tmp582;});void*
_tmp584;struct Cyc_Absyn_PtrInfo _tmp585;void*_tmp586;struct Cyc_Absyn_Tqual _tmp587;
struct Cyc_Absyn_PtrAtts _tmp588;void*_tmp589;struct Cyc_Absyn_Conref*_tmp58A;
struct Cyc_Absyn_Conref*_tmp58B;struct Cyc_Absyn_Conref*_tmp58C;void*_tmp58D;
struct Cyc_Absyn_PtrInfo _tmp58E;void*_tmp58F;struct Cyc_Absyn_Tqual _tmp590;struct
Cyc_Absyn_PtrAtts _tmp591;void*_tmp592;struct Cyc_Absyn_Conref*_tmp593;struct Cyc_Absyn_Conref*
_tmp594;struct Cyc_Absyn_Conref*_tmp595;void*_tmp596;struct Cyc_Absyn_TunionInfo
_tmp597;union Cyc_Absyn_TunionInfoU_union _tmp598;struct Cyc_Absyn_Tuniondecl**
_tmp599;struct Cyc_Absyn_Tuniondecl*_tmp59A;struct Cyc_List_List*_tmp59B;struct Cyc_Core_Opt*
_tmp59C;struct Cyc_Core_Opt _tmp59D;void*_tmp59E;void*_tmp59F;struct Cyc_Absyn_TunionInfo
_tmp5A0;union Cyc_Absyn_TunionInfoU_union _tmp5A1;struct Cyc_Absyn_Tuniondecl**
_tmp5A2;struct Cyc_Absyn_Tuniondecl*_tmp5A3;struct Cyc_List_List*_tmp5A4;struct Cyc_Core_Opt*
_tmp5A5;struct Cyc_Core_Opt _tmp5A6;void*_tmp5A7;void*_tmp5A8;struct Cyc_Absyn_FnInfo
_tmp5A9;void*_tmp5AA;struct Cyc_Absyn_FnInfo _tmp5AB;_LL3CF: _tmp584=_tmp583.f1;if(
_tmp584 <= (void*)4)goto _LL3D1;if(*((int*)_tmp584)!= 4)goto _LL3D1;_tmp585=((
struct Cyc_Absyn_PointerType_struct*)_tmp584)->f1;_tmp586=(void*)_tmp585.elt_typ;
_tmp587=_tmp585.elt_tq;_tmp588=_tmp585.ptr_atts;_tmp589=(void*)_tmp588.rgn;
_tmp58A=_tmp588.nullable;_tmp58B=_tmp588.bounds;_tmp58C=_tmp588.zero_term;
_tmp58D=_tmp583.f2;if(_tmp58D <= (void*)4)goto _LL3D1;if(*((int*)_tmp58D)!= 4)goto
_LL3D1;_tmp58E=((struct Cyc_Absyn_PointerType_struct*)_tmp58D)->f1;_tmp58F=(void*)
_tmp58E.elt_typ;_tmp590=_tmp58E.elt_tq;_tmp591=_tmp58E.ptr_atts;_tmp592=(void*)
_tmp591.rgn;_tmp593=_tmp591.nullable;_tmp594=_tmp591.bounds;_tmp595=_tmp591.zero_term;
_LL3D0: if(_tmp587.real_const  && !_tmp590.real_const)return 0;if((!((int(*)(int(*
cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp58A,_tmp593) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp58A)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp593))return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp58C,
_tmp595) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp58C)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp595))return 0;if(!Cyc_Tcutil_unify(_tmp589,_tmp592) && !Cyc_Tcenv_region_outlives(
te,_tmp589,_tmp592))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp58B,_tmp594)){struct _tuple6 _tmp5AD=({struct _tuple6 _tmp5AC;_tmp5AC.f1=Cyc_Absyn_conref_val(
_tmp58B);_tmp5AC.f2=Cyc_Absyn_conref_val(_tmp594);_tmp5AC;});void*_tmp5AE;void*
_tmp5AF;void*_tmp5B0;void*_tmp5B1;void*_tmp5B2;struct Cyc_Absyn_Exp*_tmp5B3;void*
_tmp5B4;struct Cyc_Absyn_Exp*_tmp5B5;_LL3D8: _tmp5AE=_tmp5AD.f1;if(_tmp5AE <= (void*)
2)goto _LL3DA;if(*((int*)_tmp5AE)!= 0)goto _LL3DA;_tmp5AF=_tmp5AD.f2;if((int)
_tmp5AF != 0)goto _LL3DA;_LL3D9: goto _LL3D7;_LL3DA: _tmp5B0=_tmp5AD.f1;if(_tmp5B0 <= (
void*)2)goto _LL3DC;if(*((int*)_tmp5B0)!= 0)goto _LL3DC;_tmp5B1=_tmp5AD.f2;if((int)
_tmp5B1 != 1)goto _LL3DC;_LL3DB: goto _LL3D7;_LL3DC: _tmp5B2=_tmp5AD.f1;if(_tmp5B2 <= (
void*)2)goto _LL3DE;if(*((int*)_tmp5B2)!= 0)goto _LL3DE;_tmp5B3=((struct Cyc_Absyn_Upper_b_struct*)
_tmp5B2)->f1;_tmp5B4=_tmp5AD.f2;if(_tmp5B4 <= (void*)2)goto _LL3DE;if(*((int*)
_tmp5B4)!= 0)goto _LL3DE;_tmp5B5=((struct Cyc_Absyn_Upper_b_struct*)_tmp5B4)->f1;
_LL3DD: if(!Cyc_Evexp_lte_const_exp(_tmp5B5,_tmp5B3))return 0;goto _LL3D7;_LL3DE:;
_LL3DF: return 0;_LL3D7:;}return Cyc_Tcutil_ptrsubtype(te,({struct Cyc_List_List*
_tmp5B6=_cycalloc(sizeof(*_tmp5B6));_tmp5B6->hd=({struct _tuple6*_tmp5B7=
_cycalloc(sizeof(*_tmp5B7));_tmp5B7->f1=t1;_tmp5B7->f2=t2;_tmp5B7;});_tmp5B6->tl=
assume;_tmp5B6;}),_tmp586,_tmp58F);_LL3D1: _tmp596=_tmp583.f1;if(_tmp596 <= (void*)
4)goto _LL3D3;if(*((int*)_tmp596)!= 2)goto _LL3D3;_tmp597=((struct Cyc_Absyn_TunionType_struct*)
_tmp596)->f1;_tmp598=_tmp597.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp596)->f1).tunion_info).KnownTunion).tag != 1)goto _LL3D3;_tmp599=(_tmp598.KnownTunion).f1;
_tmp59A=*_tmp599;_tmp59B=_tmp597.targs;_tmp59C=_tmp597.rgn;if(_tmp59C == 0)goto
_LL3D3;_tmp59D=*_tmp59C;_tmp59E=(void*)_tmp59D.v;_tmp59F=_tmp583.f2;if(_tmp59F <= (
void*)4)goto _LL3D3;if(*((int*)_tmp59F)!= 2)goto _LL3D3;_tmp5A0=((struct Cyc_Absyn_TunionType_struct*)
_tmp59F)->f1;_tmp5A1=_tmp5A0.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp59F)->f1).tunion_info).KnownTunion).tag != 1)goto _LL3D3;_tmp5A2=(_tmp5A1.KnownTunion).f1;
_tmp5A3=*_tmp5A2;_tmp5A4=_tmp5A0.targs;_tmp5A5=_tmp5A0.rgn;if(_tmp5A5 == 0)goto
_LL3D3;_tmp5A6=*_tmp5A5;_tmp5A7=(void*)_tmp5A6.v;_LL3D2: if(_tmp59A != _tmp5A3  || 
!Cyc_Tcenv_region_outlives(te,_tmp59E,_tmp5A7))return 0;for(0;_tmp59B != 0  && 
_tmp5A4 != 0;(_tmp59B=_tmp59B->tl,_tmp5A4=_tmp5A4->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp59B->hd,(void*)_tmp5A4->hd))return 0;}if(_tmp59B != 0  || _tmp5A4 != 0)
return 0;return 1;_LL3D3: _tmp5A8=_tmp583.f1;if(_tmp5A8 <= (void*)4)goto _LL3D5;if(*((
int*)_tmp5A8)!= 8)goto _LL3D5;_tmp5A9=((struct Cyc_Absyn_FnType_struct*)_tmp5A8)->f1;
_tmp5AA=_tmp583.f2;if(_tmp5AA <= (void*)4)goto _LL3D5;if(*((int*)_tmp5AA)!= 8)goto
_LL3D5;_tmp5AB=((struct Cyc_Absyn_FnType_struct*)_tmp5AA)->f1;_LL3D4: if(_tmp5A9.tvars
!= 0  || _tmp5AB.tvars != 0){struct Cyc_List_List*_tmp5B8=_tmp5A9.tvars;struct Cyc_List_List*
_tmp5B9=_tmp5AB.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp5B8)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5B9))return 0;{
struct _RegionHandle _tmp5BA=_new_region("r");struct _RegionHandle*r=& _tmp5BA;
_push_region(r);{struct Cyc_List_List*inst=0;while(_tmp5B8 != 0){if(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp5B8->hd)!= Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp5B9))->hd)){int _tmp5BB=0;_npop_handler(0);
return _tmp5BB;}inst=({struct Cyc_List_List*_tmp5BC=_region_malloc(r,sizeof(*
_tmp5BC));_tmp5BC->hd=({struct _tuple8*_tmp5BD=_region_malloc(r,sizeof(*_tmp5BD));
_tmp5BD->f1=(struct Cyc_Absyn_Tvar*)_tmp5B9->hd;_tmp5BD->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE[0]=({struct Cyc_Absyn_VarType_struct
_tmp5BF;_tmp5BF.tag=1;_tmp5BF.f1=(struct Cyc_Absyn_Tvar*)_tmp5B8->hd;_tmp5BF;});
_tmp5BE;});_tmp5BD;});_tmp5BC->tl=inst;_tmp5BC;});_tmp5B8=_tmp5B8->tl;_tmp5B9=
_tmp5B9->tl;}if(inst != 0){_tmp5A9.tvars=0;_tmp5AB.tvars=0;{int _tmp5C4=Cyc_Tcutil_subtype(
te,assume,(void*)({struct Cyc_Absyn_FnType_struct*_tmp5C0=_cycalloc(sizeof(*
_tmp5C0));_tmp5C0[0]=({struct Cyc_Absyn_FnType_struct _tmp5C1;_tmp5C1.tag=8;
_tmp5C1.f1=_tmp5A9;_tmp5C1;});_tmp5C0;}),(void*)({struct Cyc_Absyn_FnType_struct*
_tmp5C2=_cycalloc(sizeof(*_tmp5C2));_tmp5C2[0]=({struct Cyc_Absyn_FnType_struct
_tmp5C3;_tmp5C3.tag=8;_tmp5C3.f1=_tmp5AB;_tmp5C3;});_tmp5C2;}));_npop_handler(0);
return _tmp5C4;}}};_pop_region(r);}}if(!Cyc_Tcutil_subtype(te,assume,(void*)
_tmp5A9.ret_typ,(void*)_tmp5AB.ret_typ))return 0;{struct Cyc_List_List*_tmp5C5=
_tmp5A9.args;struct Cyc_List_List*_tmp5C6=_tmp5AB.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp5C5)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp5C6))return 0;for(0;_tmp5C5 != 0;(_tmp5C5=_tmp5C5->tl,_tmp5C6=_tmp5C6->tl)){
struct Cyc_Absyn_Tqual _tmp5C8;void*_tmp5C9;struct _tuple2 _tmp5C7=*((struct _tuple2*)
_tmp5C5->hd);_tmp5C8=_tmp5C7.f2;_tmp5C9=_tmp5C7.f3;{struct Cyc_Absyn_Tqual _tmp5CB;
void*_tmp5CC;struct _tuple2 _tmp5CA=*((struct _tuple2*)((struct Cyc_List_List*)
_check_null(_tmp5C6))->hd);_tmp5CB=_tmp5CA.f2;_tmp5CC=_tmp5CA.f3;if(_tmp5CB.real_const
 && !_tmp5C8.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp5CC,_tmp5C9))return
0;}}if(_tmp5A9.c_varargs != _tmp5AB.c_varargs)return 0;if(_tmp5A9.cyc_varargs != 0
 && _tmp5AB.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp5CD=*_tmp5A9.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp5CE=*_tmp5AB.cyc_varargs;if((_tmp5CE.tq).real_const
 && !(_tmp5CD.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,(void*)_tmp5CE.type,(
void*)_tmp5CD.type))return 0;}else{if(_tmp5A9.cyc_varargs != 0  || _tmp5AB.cyc_varargs
!= 0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp5A9.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5AB.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp5A9.rgn_po,_tmp5AB.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp5A9.attributes,_tmp5AB.attributes))return 0;return 1;}_LL3D5:;_LL3D6: return 0;
_LL3CE:;}}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple6 _tmp5D0=({
struct _tuple6 _tmp5CF;_tmp5CF.f1=Cyc_Tcutil_compress(t1);_tmp5CF.f2=Cyc_Tcutil_compress(
t2);_tmp5CF;});void*_tmp5D1;void*_tmp5D2;void*_tmp5D3;void*_tmp5D4;_LL3E1:
_tmp5D1=_tmp5D0.f1;if(_tmp5D1 <= (void*)4)goto _LL3E3;if(*((int*)_tmp5D1)!= 5)goto
_LL3E3;_tmp5D2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5D1)->f2;_tmp5D3=
_tmp5D0.f2;if(_tmp5D3 <= (void*)4)goto _LL3E3;if(*((int*)_tmp5D3)!= 5)goto _LL3E3;
_tmp5D4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5D3)->f2;_LL3E2: return(
_tmp5D2 == _tmp5D4  || _tmp5D2 == (void*)2  && _tmp5D4 == (void*)3) || _tmp5D2 == (
void*)3  && _tmp5D4 == (void*)2;_LL3E3:;_LL3E4: return 0;_LL3E0:;}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){struct
_RegionHandle _tmp5D5=_new_region("temp");struct _RegionHandle*temp=& _tmp5D5;
_push_region(temp);{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,te,t2);for(0;tqs2 != 0;(tqs2=
tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0){int _tmp5D6=0;_npop_handler(0);return
_tmp5D6;}{struct _tuple4 _tmp5D8;struct Cyc_Absyn_Tqual _tmp5D9;void*_tmp5DA;struct
_tuple4*_tmp5D7=(struct _tuple4*)tqs1->hd;_tmp5D8=*_tmp5D7;_tmp5D9=_tmp5D8.f1;
_tmp5DA=_tmp5D8.f2;{struct _tuple4 _tmp5DC;struct Cyc_Absyn_Tqual _tmp5DD;void*
_tmp5DE;struct _tuple4*_tmp5DB=(struct _tuple4*)tqs2->hd;_tmp5DC=*_tmp5DB;_tmp5DD=
_tmp5DC.f1;_tmp5DE=_tmp5DC.f2;if(_tmp5DD.real_const  && Cyc_Tcutil_subtype(te,
assume,_tmp5DA,_tmp5DE))continue;else{if(Cyc_Tcutil_unify(_tmp5DA,_tmp5DE))
continue;else{if(Cyc_Tcutil_isomorphic(_tmp5DA,_tmp5DE))continue;else{int _tmp5DF=
0;_npop_handler(0);return _tmp5DF;}}}}}}{int _tmp5E0=1;_npop_handler(0);return
_tmp5E0;}};_pop_region(temp);}static int Cyc_Tcutil_is_char_type(void*t){void*
_tmp5E1=Cyc_Tcutil_compress(t);void*_tmp5E2;_LL3E6: if(_tmp5E1 <= (void*)4)goto
_LL3E8;if(*((int*)_tmp5E1)!= 5)goto _LL3E8;_tmp5E2=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5E1)->f2;if((int)_tmp5E2 != 0)goto _LL3E8;_LL3E7: return 1;_LL3E8:;_LL3E9: return
0;_LL3E5:;}void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return(void*)1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t2 == (void*)0)return(void*)1;{void*_tmp5E3=t2;
void*_tmp5E4;void*_tmp5E5;_LL3EB: if(_tmp5E3 <= (void*)4)goto _LL3EF;if(*((int*)
_tmp5E3)!= 5)goto _LL3ED;_tmp5E4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E3)->f2;
if((int)_tmp5E4 != 2)goto _LL3ED;_LL3EC: goto _LL3EE;_LL3ED: if(*((int*)_tmp5E3)!= 5)
goto _LL3EF;_tmp5E5=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E3)->f2;if((int)
_tmp5E5 != 3)goto _LL3EF;_LL3EE: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return(void*)
1;goto _LL3EA;_LL3EF:;_LL3F0: goto _LL3EA;_LL3EA:;}{void*_tmp5E6=t1;struct Cyc_Absyn_PtrInfo
_tmp5E7;void*_tmp5E8;struct Cyc_Absyn_Tqual _tmp5E9;struct Cyc_Absyn_PtrAtts _tmp5EA;
void*_tmp5EB;struct Cyc_Absyn_Conref*_tmp5EC;struct Cyc_Absyn_Conref*_tmp5ED;
struct Cyc_Absyn_Conref*_tmp5EE;struct Cyc_Absyn_ArrayInfo _tmp5EF;void*_tmp5F0;
struct Cyc_Absyn_Tqual _tmp5F1;struct Cyc_Absyn_Exp*_tmp5F2;struct Cyc_Absyn_Conref*
_tmp5F3;struct Cyc_Absyn_Enumdecl*_tmp5F4;_LL3F2: if(_tmp5E6 <= (void*)4)goto _LL3FA;
if(*((int*)_tmp5E6)!= 4)goto _LL3F4;_tmp5E7=((struct Cyc_Absyn_PointerType_struct*)
_tmp5E6)->f1;_tmp5E8=(void*)_tmp5E7.elt_typ;_tmp5E9=_tmp5E7.elt_tq;_tmp5EA=
_tmp5E7.ptr_atts;_tmp5EB=(void*)_tmp5EA.rgn;_tmp5EC=_tmp5EA.nullable;_tmp5ED=
_tmp5EA.bounds;_tmp5EE=_tmp5EA.zero_term;_LL3F3:{void*_tmp5F5=t2;struct Cyc_Absyn_PtrInfo
_tmp5F6;void*_tmp5F7;struct Cyc_Absyn_Tqual _tmp5F8;struct Cyc_Absyn_PtrAtts _tmp5F9;
void*_tmp5FA;struct Cyc_Absyn_Conref*_tmp5FB;struct Cyc_Absyn_Conref*_tmp5FC;
struct Cyc_Absyn_Conref*_tmp5FD;_LL401: if(_tmp5F5 <= (void*)4)goto _LL403;if(*((int*)
_tmp5F5)!= 4)goto _LL403;_tmp5F6=((struct Cyc_Absyn_PointerType_struct*)_tmp5F5)->f1;
_tmp5F7=(void*)_tmp5F6.elt_typ;_tmp5F8=_tmp5F6.elt_tq;_tmp5F9=_tmp5F6.ptr_atts;
_tmp5FA=(void*)_tmp5F9.rgn;_tmp5FB=_tmp5F9.nullable;_tmp5FC=_tmp5F9.bounds;
_tmp5FD=_tmp5F9.zero_term;_LL402: {void*coercion=(void*)3;struct Cyc_List_List*
_tmp5FE=({struct Cyc_List_List*_tmp60D=_cycalloc(sizeof(*_tmp60D));_tmp60D->hd=({
struct _tuple6*_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E->f1=t1;_tmp60E->f2=t2;
_tmp60E;});_tmp60D->tl=0;_tmp60D;});int _tmp5FF=Cyc_Tcutil_ptrsubtype(te,_tmp5FE,
_tmp5E8,_tmp5F7) && (!_tmp5E9.real_const  || _tmp5F8.real_const);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp5EE,_tmp5FD) || !((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp5FD);int _tmp600=_tmp5FF?0:((Cyc_Tcutil_bits_only(_tmp5E8) && Cyc_Tcutil_is_char_type(
_tmp5F7)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5FD)) && (_tmp5F8.real_const  || !_tmp5E9.real_const);int bounds_ok=Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,_tmp5ED,_tmp5FC);if(!bounds_ok  && !_tmp600){struct
_tuple6 _tmp602=({struct _tuple6 _tmp601;_tmp601.f1=Cyc_Absyn_conref_val(_tmp5ED);
_tmp601.f2=Cyc_Absyn_conref_val(_tmp5FC);_tmp601;});void*_tmp603;struct Cyc_Absyn_Exp*
_tmp604;void*_tmp605;struct Cyc_Absyn_Exp*_tmp606;void*_tmp607;void*_tmp608;void*
_tmp609;void*_tmp60A;void*_tmp60B;void*_tmp60C;_LL406: _tmp603=_tmp602.f1;if(
_tmp603 <= (void*)2)goto _LL408;if(*((int*)_tmp603)!= 0)goto _LL408;_tmp604=((
struct Cyc_Absyn_Upper_b_struct*)_tmp603)->f1;_tmp605=_tmp602.f2;if(_tmp605 <= (
void*)2)goto _LL408;if(*((int*)_tmp605)!= 0)goto _LL408;_tmp606=((struct Cyc_Absyn_Upper_b_struct*)
_tmp605)->f1;_LL407: if(Cyc_Evexp_lte_const_exp(_tmp606,_tmp604))bounds_ok=1;goto
_LL405;_LL408: _tmp607=_tmp602.f1;if(_tmp607 <= (void*)2)goto _LL40A;if(*((int*)
_tmp607)!= 1)goto _LL40A;_tmp608=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp607)->f1;_tmp609=_tmp602.f2;if(_tmp609 <= (void*)2)goto _LL40A;if(*((int*)
_tmp609)!= 1)goto _LL40A;_tmp60A=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp609)->f1;_LL409: bounds_ok=Cyc_Tcutil_unify(_tmp608,_tmp60A);goto _LL405;
_LL40A: _tmp60B=_tmp602.f1;if(_tmp60B <= (void*)2)goto _LL40C;if(*((int*)_tmp60B)!= 
1)goto _LL40C;_LL40B: goto _LL40D;_LL40C: _tmp60C=_tmp602.f2;if(_tmp60C <= (void*)2)
goto _LL40E;if(*((int*)_tmp60C)!= 1)goto _LL40E;_LL40D: bounds_ok=0;goto _LL405;
_LL40E:;_LL40F: bounds_ok=1;goto _LL405;_LL405:;}if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp5EC) && !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp5FB))coercion=(void*)2;if(((bounds_ok  && zeroterm_ok)
 && (_tmp5FF  || _tmp600)) && (Cyc_Tcutil_unify(_tmp5EB,_tmp5FA) || Cyc_Tcenv_region_outlives(
te,_tmp5EB,_tmp5FA)))return coercion;else{return(void*)0;}}}_LL403:;_LL404: goto
_LL400;_LL400:;}return(void*)0;_LL3F4: if(*((int*)_tmp5E6)!= 7)goto _LL3F6;_tmp5EF=((
struct Cyc_Absyn_ArrayType_struct*)_tmp5E6)->f1;_tmp5F0=(void*)_tmp5EF.elt_type;
_tmp5F1=_tmp5EF.tq;_tmp5F2=_tmp5EF.num_elts;_tmp5F3=_tmp5EF.zero_term;_LL3F5:{
void*_tmp60F=t2;struct Cyc_Absyn_ArrayInfo _tmp610;void*_tmp611;struct Cyc_Absyn_Tqual
_tmp612;struct Cyc_Absyn_Exp*_tmp613;struct Cyc_Absyn_Conref*_tmp614;_LL411: if(
_tmp60F <= (void*)4)goto _LL413;if(*((int*)_tmp60F)!= 7)goto _LL413;_tmp610=((
struct Cyc_Absyn_ArrayType_struct*)_tmp60F)->f1;_tmp611=(void*)_tmp610.elt_type;
_tmp612=_tmp610.tq;_tmp613=_tmp610.num_elts;_tmp614=_tmp610.zero_term;_LL412: {
int okay;okay=((_tmp5F2 != 0  && _tmp613 != 0) && ((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp5F3,_tmp614)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp613,(
struct Cyc_Absyn_Exp*)_tmp5F2);return(okay  && Cyc_Tcutil_unify(_tmp5F0,_tmp611))
 && (!_tmp5F1.real_const  || _tmp612.real_const)?(void*)3:(void*)0;}_LL413:;
_LL414: return(void*)0;_LL410:;}return(void*)0;_LL3F6: if(*((int*)_tmp5E6)!= 12)
goto _LL3F8;_tmp5F4=((struct Cyc_Absyn_EnumType_struct*)_tmp5E6)->f2;_LL3F7:{void*
_tmp615=t2;struct Cyc_Absyn_Enumdecl*_tmp616;_LL416: if(_tmp615 <= (void*)4)goto
_LL418;if(*((int*)_tmp615)!= 12)goto _LL418;_tmp616=((struct Cyc_Absyn_EnumType_struct*)
_tmp615)->f2;_LL417: if((_tmp5F4->fields != 0  && _tmp616->fields != 0) && ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp5F4->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp616->fields))->v))
return(void*)1;goto _LL415;_LL418:;_LL419: goto _LL415;_LL415:;}goto _LL3F9;_LL3F8:
if(*((int*)_tmp5E6)!= 5)goto _LL3FA;_LL3F9: goto _LL3FB;_LL3FA: if((int)_tmp5E6 != 1)
goto _LL3FC;_LL3FB: goto _LL3FD;_LL3FC: if(_tmp5E6 <= (void*)4)goto _LL3FE;if(*((int*)
_tmp5E6)!= 6)goto _LL3FE;_LL3FD: return Cyc_Tcutil_coerceable(t2)?(void*)1:(void*)0;
_LL3FE:;_LL3FF: return(void*)0;_LL3F1:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,void*c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp617=Cyc_Absyn_copy_exp(e);(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*_tmp618=_cycalloc(
sizeof(*_tmp618));_tmp618[0]=({struct Cyc_Absyn_Cast_e_struct _tmp619;_tmp619.tag=
15;_tmp619.f1=(void*)t;_tmp619.f2=_tmp617;_tmp619.f3=0;_tmp619.f4=(void*)c;
_tmp619;});_tmp618;})));e->topt=({struct Cyc_Core_Opt*_tmp61A=_cycalloc(sizeof(*
_tmp61A));_tmp61A->v=(void*)t;_tmp61A;});}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*
e){void*_tmp61B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL41B: if(_tmp61B <= (void*)4)goto _LL427;if(*((int*)_tmp61B)!= 5)goto _LL41D;
_LL41C: goto _LL41E;_LL41D: if(*((int*)_tmp61B)!= 12)goto _LL41F;_LL41E: goto _LL420;
_LL41F: if(*((int*)_tmp61B)!= 13)goto _LL421;_LL420: goto _LL422;_LL421: if(*((int*)
_tmp61B)!= 17)goto _LL423;_LL422: goto _LL424;_LL423: if(*((int*)_tmp61B)!= 14)goto
_LL425;_LL424: return 1;_LL425: if(*((int*)_tmp61B)!= 0)goto _LL427;_LL426: return Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL427:;
_LL428: return 0;_LL41A:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp61C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL42A: if((int)_tmp61C != 1)goto _LL42C;_LL42B: goto
_LL42D;_LL42C: if(_tmp61C <= (void*)4)goto _LL42E;if(*((int*)_tmp61C)!= 6)goto
_LL42E;_LL42D: return 1;_LL42E:;_LL42F: return 0;_LL429:;}}int Cyc_Tcutil_is_function_type(
void*t){void*_tmp61D=Cyc_Tcutil_compress(t);_LL431: if(_tmp61D <= (void*)4)goto
_LL433;if(*((int*)_tmp61D)!= 8)goto _LL433;_LL432: return 1;_LL433:;_LL434: return 0;
_LL430:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct _tuple6
_tmp61F=({struct _tuple6 _tmp61E;_tmp61E.f1=t1;_tmp61E.f2=t2;_tmp61E;});void*
_tmp620;int _tmp621;void*_tmp622;int _tmp623;void*_tmp624;void*_tmp625;void*
_tmp626;void*_tmp627;void*_tmp628;void*_tmp629;void*_tmp62A;void*_tmp62B;void*
_tmp62C;void*_tmp62D;void*_tmp62E;void*_tmp62F;void*_tmp630;void*_tmp631;void*
_tmp632;void*_tmp633;void*_tmp634;void*_tmp635;void*_tmp636;void*_tmp637;void*
_tmp638;void*_tmp639;void*_tmp63A;void*_tmp63B;void*_tmp63C;void*_tmp63D;void*
_tmp63E;void*_tmp63F;void*_tmp640;void*_tmp641;_LL436: _tmp620=_tmp61F.f1;if(
_tmp620 <= (void*)4)goto _LL438;if(*((int*)_tmp620)!= 6)goto _LL438;_tmp621=((
struct Cyc_Absyn_DoubleType_struct*)_tmp620)->f1;_tmp622=_tmp61F.f2;if(_tmp622 <= (
void*)4)goto _LL438;if(*((int*)_tmp622)!= 6)goto _LL438;_tmp623=((struct Cyc_Absyn_DoubleType_struct*)
_tmp622)->f1;_LL437: if(_tmp621)return t1;else{return t2;}_LL438: _tmp624=_tmp61F.f1;
if(_tmp624 <= (void*)4)goto _LL43A;if(*((int*)_tmp624)!= 6)goto _LL43A;_LL439:
return t1;_LL43A: _tmp625=_tmp61F.f2;if(_tmp625 <= (void*)4)goto _LL43C;if(*((int*)
_tmp625)!= 6)goto _LL43C;_LL43B: return t2;_LL43C: _tmp626=_tmp61F.f1;if((int)
_tmp626 != 1)goto _LL43E;_LL43D: goto _LL43F;_LL43E: _tmp627=_tmp61F.f2;if((int)
_tmp627 != 1)goto _LL440;_LL43F: return(void*)1;_LL440: _tmp628=_tmp61F.f1;if(
_tmp628 <= (void*)4)goto _LL442;if(*((int*)_tmp628)!= 5)goto _LL442;_tmp629=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp628)->f1;if((int)_tmp629 != 1)goto _LL442;
_tmp62A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp628)->f2;if((int)_tmp62A != 
4)goto _LL442;_LL441: goto _LL443;_LL442: _tmp62B=_tmp61F.f2;if(_tmp62B <= (void*)4)
goto _LL444;if(*((int*)_tmp62B)!= 5)goto _LL444;_tmp62C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp62B)->f1;if((int)_tmp62C != 1)goto _LL444;_tmp62D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp62B)->f2;if((int)_tmp62D != 4)goto _LL444;_LL443: return Cyc_Absyn_ulonglong_typ;
_LL444: _tmp62E=_tmp61F.f1;if(_tmp62E <= (void*)4)goto _LL446;if(*((int*)_tmp62E)!= 
5)goto _LL446;_tmp62F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp62E)->f2;if((
int)_tmp62F != 4)goto _LL446;_LL445: goto _LL447;_LL446: _tmp630=_tmp61F.f2;if(
_tmp630 <= (void*)4)goto _LL448;if(*((int*)_tmp630)!= 5)goto _LL448;_tmp631=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp630)->f2;if((int)_tmp631 != 4)goto _LL448;
_LL447: return Cyc_Absyn_slonglong_typ;_LL448: _tmp632=_tmp61F.f1;if(_tmp632 <= (
void*)4)goto _LL44A;if(*((int*)_tmp632)!= 5)goto _LL44A;_tmp633=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp632)->f1;if((int)_tmp633 != 1)goto _LL44A;_tmp634=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp632)->f2;if((int)_tmp634 != 3)goto _LL44A;_LL449: goto _LL44B;_LL44A: _tmp635=
_tmp61F.f2;if(_tmp635 <= (void*)4)goto _LL44C;if(*((int*)_tmp635)!= 5)goto _LL44C;
_tmp636=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp635)->f1;if((int)_tmp636 != 
1)goto _LL44C;_tmp637=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp635)->f2;if((
int)_tmp637 != 3)goto _LL44C;_LL44B: return Cyc_Absyn_ulong_typ;_LL44C: _tmp638=
_tmp61F.f1;if(_tmp638 <= (void*)4)goto _LL44E;if(*((int*)_tmp638)!= 5)goto _LL44E;
_tmp639=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp638)->f1;if((int)_tmp639 != 
1)goto _LL44E;_tmp63A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp638)->f2;if((
int)_tmp63A != 2)goto _LL44E;_LL44D: goto _LL44F;_LL44E: _tmp63B=_tmp61F.f2;if(
_tmp63B <= (void*)4)goto _LL450;if(*((int*)_tmp63B)!= 5)goto _LL450;_tmp63C=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp63B)->f1;if((int)_tmp63C != 1)goto _LL450;
_tmp63D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp63B)->f2;if((int)_tmp63D != 
2)goto _LL450;_LL44F: return Cyc_Absyn_uint_typ;_LL450: _tmp63E=_tmp61F.f1;if(
_tmp63E <= (void*)4)goto _LL452;if(*((int*)_tmp63E)!= 5)goto _LL452;_tmp63F=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp63E)->f2;if((int)_tmp63F != 3)goto _LL452;
_LL451: goto _LL453;_LL452: _tmp640=_tmp61F.f2;if(_tmp640 <= (void*)4)goto _LL454;if(*((
int*)_tmp640)!= 5)goto _LL454;_tmp641=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp640)->f2;if((int)_tmp641 != 3)goto _LL454;_LL453: return Cyc_Absyn_slong_typ;
_LL454:;_LL455: return Cyc_Absyn_sint_typ;_LL435:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp642=(void*)e->r;struct Cyc_Core_Opt*_tmp643;
_LL457: if(*((int*)_tmp642)!= 4)goto _LL459;_tmp643=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp642)->f2;if(_tmp643 != 0)goto _LL459;_LL458:({void*_tmp644[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp645="assignment in test";_tag_dynforward(_tmp645,sizeof(
char),_get_zero_arr_size(_tmp645,19));}),_tag_dynforward(_tmp644,sizeof(void*),0));});
goto _LL456;_LL459:;_LL45A: goto _LL456;_LL456:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple6 _tmp647=({struct _tuple6 _tmp646;_tmp646.f1=c1;_tmp646.f2=c2;_tmp646;});
void*_tmp648;void*_tmp649;void*_tmp64A;void*_tmp64B;void*_tmp64C;struct Cyc_Core_Opt*
_tmp64D;struct Cyc_Core_Opt**_tmp64E;void*_tmp64F;struct Cyc_Core_Opt*_tmp650;
struct Cyc_Core_Opt**_tmp651;void*_tmp652;struct Cyc_Core_Opt*_tmp653;struct Cyc_Core_Opt**
_tmp654;void*_tmp655;void*_tmp656;void*_tmp657;void*_tmp658;void*_tmp659;void*
_tmp65A;struct Cyc_Core_Opt*_tmp65B;struct Cyc_Core_Opt**_tmp65C;void*_tmp65D;void*
_tmp65E;struct Cyc_Core_Opt*_tmp65F;struct Cyc_Core_Opt**_tmp660;void*_tmp661;void*
_tmp662;struct Cyc_Core_Opt*_tmp663;struct Cyc_Core_Opt**_tmp664;void*_tmp665;
_LL45C: _tmp648=_tmp647.f1;if(*((int*)_tmp648)!= 0)goto _LL45E;_tmp649=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp648)->f1;_tmp64A=_tmp647.f2;if(*((int*)_tmp64A)
!= 0)goto _LL45E;_tmp64B=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp64A)->f1;
_LL45D: return _tmp649 == _tmp64B;_LL45E: _tmp64C=_tmp647.f2;if(*((int*)_tmp64C)!= 1)
goto _LL460;_tmp64D=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp64C)->f1;_tmp64E=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp64C)->f1;_LL45F:*
_tmp64E=({struct Cyc_Core_Opt*_tmp666=_cycalloc(sizeof(*_tmp666));_tmp666->v=(
void*)c1;_tmp666;});return 1;_LL460: _tmp64F=_tmp647.f1;if(*((int*)_tmp64F)!= 1)
goto _LL462;_tmp650=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp64F)->f1;_tmp651=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp64F)->f1;_LL461:*
_tmp651=({struct Cyc_Core_Opt*_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667->v=(
void*)c2;_tmp667;});return 1;_LL462: _tmp652=_tmp647.f1;if(*((int*)_tmp652)!= 2)
goto _LL464;_tmp653=((struct Cyc_Absyn_Less_kb_struct*)_tmp652)->f1;_tmp654=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp652)->f1;_tmp655=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp652)->f2;_tmp656=_tmp647.f2;if(*((
int*)_tmp656)!= 0)goto _LL464;_tmp657=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp656)->f1;_LL463: if(Cyc_Tcutil_kind_leq(_tmp657,_tmp655)){*_tmp654=({struct
Cyc_Core_Opt*_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668->v=(void*)c2;_tmp668;});
return 1;}else{return 0;}_LL464: _tmp658=_tmp647.f1;if(*((int*)_tmp658)!= 0)goto
_LL466;_tmp659=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp658)->f1;_tmp65A=
_tmp647.f2;if(*((int*)_tmp65A)!= 2)goto _LL466;_tmp65B=((struct Cyc_Absyn_Less_kb_struct*)
_tmp65A)->f1;_tmp65C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp65A)->f1;_tmp65D=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp65A)->f2;
_LL465: if(Cyc_Tcutil_kind_leq(_tmp659,_tmp65D)){*_tmp65C=({struct Cyc_Core_Opt*
_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669->v=(void*)c1;_tmp669;});return 1;}
else{return 0;}_LL466: _tmp65E=_tmp647.f1;if(*((int*)_tmp65E)!= 2)goto _LL45B;
_tmp65F=((struct Cyc_Absyn_Less_kb_struct*)_tmp65E)->f1;_tmp660=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp65E)->f1;_tmp661=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp65E)->f2;_tmp662=_tmp647.f2;if(*((int*)_tmp662)!= 2)goto _LL45B;_tmp663=((
struct Cyc_Absyn_Less_kb_struct*)_tmp662)->f1;_tmp664=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp662)->f1;_tmp665=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp662)->f2;_LL467: if(Cyc_Tcutil_kind_leq(_tmp661,_tmp665)){*_tmp664=({struct
Cyc_Core_Opt*_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A->v=(void*)c1;_tmp66A;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp665,_tmp661)){*_tmp660=({struct Cyc_Core_Opt*
_tmp66B=_cycalloc(sizeof(*_tmp66B));_tmp66B->v=(void*)c2;_tmp66B;});return 1;}
else{return 0;}}_LL45B:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp66C=_cycalloc_atomic(sizeof(*_tmp66C));_tmp66C[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp66C;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _dynforward_ptr s=(struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp671;_tmp671.tag=1;_tmp671.f1=(
unsigned long)i;{void*_tmp66F[1]={& _tmp671};Cyc_aprintf(({const char*_tmp670="#%d";
_tag_dynforward(_tmp670,sizeof(char),_get_zero_arr_size(_tmp670,4));}),
_tag_dynforward(_tmp66F,sizeof(void*),1));}});return({struct Cyc_Absyn_Tvar*
_tmp66D=_cycalloc(sizeof(*_tmp66D));_tmp66D->name=({struct _dynforward_ptr*
_tmp66E=_cycalloc(sizeof(struct _dynforward_ptr)* 1);_tmp66E[0]=s;_tmp66E;});
_tmp66D->identity=0;_tmp66D->kind=(void*)k;_tmp66D;});}int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*t){struct _dynforward_ptr _tmp672=*t->name;return*((const char*)
_check_dynforward_subscript(_tmp672,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_String_pa_struct _tmp675;_tmp675.tag=0;
_tmp675.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*t->name);{void*
_tmp673[1]={& _tmp675};Cyc_printf(({const char*_tmp674="%s";_tag_dynforward(
_tmp674,sizeof(char),_get_zero_arr_size(_tmp674,3));}),_tag_dynforward(_tmp673,
sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(t))return;{struct
_dynforward_ptr _tmp676=Cyc_strconcat(({const char*_tmp67B="`";_tag_dynforward(
_tmp67B,sizeof(char),_get_zero_arr_size(_tmp67B,2));}),(struct _dynforward_ptr)*t->name);({
struct _dynforward_ptr _tmp677=_dynforward_ptr_plus(_tmp676,sizeof(char),1);char
_tmp678=*((char*)_check_dynforward_subscript(_tmp677,sizeof(char),0));char
_tmp679='t';if(_get_dynforward_size(_tmp677,sizeof(char))== 1  && (_tmp678 == '\000'
 && _tmp679 != '\000'))_throw_arraybounds();*((char*)_tmp677.curr)=_tmp679;});t->name=({
struct _dynforward_ptr*_tmp67A=_cycalloc(sizeof(struct _dynforward_ptr)* 1);
_tmp67A[0]=(struct _dynforward_ptr)_tmp676;_tmp67A;});}}struct _tuple14{struct
_dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple2*Cyc_Tcutil_fndecl2typ_f(
struct _tuple14*x){return({struct _tuple2*_tmp67C=_cycalloc(sizeof(*_tmp67C));
_tmp67C->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp67D=_cycalloc(sizeof(*
_tmp67D));_tmp67D->v=(*x).f1;_tmp67D;});_tmp67C->f2=(*x).f2;_tmp67C->f3=(*x).f3;
_tmp67C;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp67E=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))_tmp67E=({
struct Cyc_List_List*_tmp67F=_cycalloc(sizeof(*_tmp67F));_tmp67F->hd=(void*)((
void*)atts->hd);_tmp67F->tl=_tmp67E;_tmp67F;});}}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp680=_cycalloc(sizeof(*_tmp680));_tmp680[0]=({struct Cyc_Absyn_FnType_struct
_tmp681;_tmp681.tag=8;_tmp681.f1=({struct Cyc_Absyn_FnInfo _tmp682;_tmp682.tvars=
fd->tvs;_tmp682.effect=fd->effect;_tmp682.ret_typ=(void*)((void*)fd->ret_type);
_tmp682.args=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple14*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp682.c_varargs=
fd->c_varargs;_tmp682.cyc_varargs=fd->cyc_varargs;_tmp682.rgn_po=fd->rgn_po;
_tmp682.attributes=_tmp67E;_tmp682;});_tmp681;});_tmp680;});}return(void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}struct _tuple15{void*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple15*
t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*t).f2;}static
struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({struct _tuple4*
_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683->f1=(*pr).f1;_tmp683->f2=t;_tmp683;});}
struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple17{
struct _tuple16*f1;void*f2;};static struct _tuple17*Cyc_Tcutil_substitute_f1(struct
_RegionHandle*rgn,struct _tuple2*y){return({struct _tuple17*_tmp684=_region_malloc(
rgn,sizeof(*_tmp684));_tmp684->f1=({struct _tuple16*_tmp685=_region_malloc(rgn,
sizeof(*_tmp685));_tmp685->f1=(*y).f1;_tmp685->f2=(*y).f2;_tmp685;});_tmp684->f2=(*
y).f3;_tmp684;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(struct _tuple17*w){
struct _tuple16*_tmp687;void*_tmp688;struct _tuple17 _tmp686=*w;_tmp687=_tmp686.f1;
_tmp688=_tmp686.f2;{struct Cyc_Core_Opt*_tmp68A;struct Cyc_Absyn_Tqual _tmp68B;
struct _tuple16 _tmp689=*_tmp687;_tmp68A=_tmp689.f1;_tmp68B=_tmp689.f2;return({
struct _tuple2*_tmp68C=_cycalloc(sizeof(*_tmp68C));_tmp68C->f1=_tmp68A;_tmp68C->f2=
_tmp68B;_tmp68C->f3=_tmp688;_tmp68C;});}}static void*Cyc_Tcutil_field_type(struct
Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*_tmp68D=
_cycalloc(sizeof(*_tmp68D));_tmp68D->name=f->name;_tmp68D->tq=f->tq;_tmp68D->type=(
void*)t;_tmp68D->width=f->width;_tmp68D->attributes=f->attributes;_tmp68D;});}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,void*t){void*_tmp68E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp68F;struct Cyc_Absyn_AggrInfo _tmp690;union Cyc_Absyn_AggrInfoU_union _tmp691;
struct Cyc_List_List*_tmp692;struct Cyc_Absyn_TunionInfo _tmp693;union Cyc_Absyn_TunionInfoU_union
_tmp694;struct Cyc_List_List*_tmp695;struct Cyc_Core_Opt*_tmp696;struct Cyc_Absyn_TunionFieldInfo
_tmp697;union Cyc_Absyn_TunionFieldInfoU_union _tmp698;struct Cyc_List_List*_tmp699;
struct _tuple1*_tmp69A;struct Cyc_List_List*_tmp69B;struct Cyc_Absyn_Typedefdecl*
_tmp69C;void**_tmp69D;struct Cyc_Absyn_ArrayInfo _tmp69E;void*_tmp69F;struct Cyc_Absyn_Tqual
_tmp6A0;struct Cyc_Absyn_Exp*_tmp6A1;struct Cyc_Absyn_Conref*_tmp6A2;struct Cyc_Position_Segment*
_tmp6A3;struct Cyc_Absyn_PtrInfo _tmp6A4;void*_tmp6A5;struct Cyc_Absyn_Tqual _tmp6A6;
struct Cyc_Absyn_PtrAtts _tmp6A7;void*_tmp6A8;struct Cyc_Absyn_Conref*_tmp6A9;
struct Cyc_Absyn_Conref*_tmp6AA;struct Cyc_Absyn_Conref*_tmp6AB;struct Cyc_Absyn_FnInfo
_tmp6AC;struct Cyc_List_List*_tmp6AD;struct Cyc_Core_Opt*_tmp6AE;void*_tmp6AF;
struct Cyc_List_List*_tmp6B0;int _tmp6B1;struct Cyc_Absyn_VarargInfo*_tmp6B2;struct
Cyc_List_List*_tmp6B3;struct Cyc_List_List*_tmp6B4;struct Cyc_List_List*_tmp6B5;
void*_tmp6B6;struct Cyc_List_List*_tmp6B7;struct Cyc_Core_Opt*_tmp6B8;void*_tmp6B9;
void*_tmp6BA;void*_tmp6BB;void*_tmp6BC;void*_tmp6BD;struct Cyc_List_List*_tmp6BE;
_LL469: if(_tmp68E <= (void*)4)goto _LL48B;if(*((int*)_tmp68E)!= 1)goto _LL46B;
_tmp68F=((struct Cyc_Absyn_VarType_struct*)_tmp68E)->f1;_LL46A: {struct
_handler_cons _tmp6BF;_push_handler(& _tmp6BF);{int _tmp6C1=0;if(setjmp(_tmp6BF.handler))
_tmp6C1=1;if(!_tmp6C1){{void*_tmp6C2=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(
Cyc_Absyn_tvar_cmp,inst,_tmp68F);_npop_handler(0);return _tmp6C2;};_pop_handler();}
else{void*_tmp6C0=(void*)_exn_thrown;void*_tmp6C4=_tmp6C0;_LL49E: if(_tmp6C4 != 
Cyc_Core_Not_found)goto _LL4A0;_LL49F: return t;_LL4A0:;_LL4A1:(void)_throw(_tmp6C4);
_LL49D:;}}}_LL46B: if(*((int*)_tmp68E)!= 10)goto _LL46D;_tmp690=((struct Cyc_Absyn_AggrType_struct*)
_tmp68E)->f1;_tmp691=_tmp690.aggr_info;_tmp692=_tmp690.targs;_LL46C: {struct Cyc_List_List*
_tmp6C5=Cyc_Tcutil_substs(rgn,inst,_tmp692);return _tmp6C5 == _tmp692?t:(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp6C6=_cycalloc(sizeof(*_tmp6C6));_tmp6C6[0]=({
struct Cyc_Absyn_AggrType_struct _tmp6C7;_tmp6C7.tag=10;_tmp6C7.f1=({struct Cyc_Absyn_AggrInfo
_tmp6C8;_tmp6C8.aggr_info=_tmp691;_tmp6C8.targs=_tmp6C5;_tmp6C8;});_tmp6C7;});
_tmp6C6;});}_LL46D: if(*((int*)_tmp68E)!= 2)goto _LL46F;_tmp693=((struct Cyc_Absyn_TunionType_struct*)
_tmp68E)->f1;_tmp694=_tmp693.tunion_info;_tmp695=_tmp693.targs;_tmp696=_tmp693.rgn;
_LL46E: {struct Cyc_List_List*_tmp6C9=Cyc_Tcutil_substs(rgn,inst,_tmp695);struct
Cyc_Core_Opt*new_r;if((unsigned int)_tmp696){void*_tmp6CA=Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp696->v);if(_tmp6CA == (void*)_tmp696->v)new_r=_tmp696;else{
new_r=({struct Cyc_Core_Opt*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB->v=(void*)
_tmp6CA;_tmp6CB;});}}else{new_r=_tmp696;}return _tmp6C9 == _tmp695  && new_r == 
_tmp696?t:(void*)({struct Cyc_Absyn_TunionType_struct*_tmp6CC=_cycalloc(sizeof(*
_tmp6CC));_tmp6CC[0]=({struct Cyc_Absyn_TunionType_struct _tmp6CD;_tmp6CD.tag=2;
_tmp6CD.f1=({struct Cyc_Absyn_TunionInfo _tmp6CE;_tmp6CE.tunion_info=_tmp694;
_tmp6CE.targs=_tmp6C9;_tmp6CE.rgn=new_r;_tmp6CE;});_tmp6CD;});_tmp6CC;});}_LL46F:
if(*((int*)_tmp68E)!= 3)goto _LL471;_tmp697=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp68E)->f1;_tmp698=_tmp697.field_info;_tmp699=_tmp697.targs;_LL470: {struct Cyc_List_List*
_tmp6CF=Cyc_Tcutil_substs(rgn,inst,_tmp699);return _tmp6CF == _tmp699?t:(void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));
_tmp6D0[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp6D1;_tmp6D1.tag=3;_tmp6D1.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp6D2;_tmp6D2.field_info=_tmp698;_tmp6D2.targs=
_tmp6CF;_tmp6D2;});_tmp6D1;});_tmp6D0;});}_LL471: if(*((int*)_tmp68E)!= 16)goto
_LL473;_tmp69A=((struct Cyc_Absyn_TypedefType_struct*)_tmp68E)->f1;_tmp69B=((
struct Cyc_Absyn_TypedefType_struct*)_tmp68E)->f2;_tmp69C=((struct Cyc_Absyn_TypedefType_struct*)
_tmp68E)->f3;_tmp69D=((struct Cyc_Absyn_TypedefType_struct*)_tmp68E)->f4;_LL472: {
struct Cyc_List_List*_tmp6D3=Cyc_Tcutil_substs(rgn,inst,_tmp69B);return _tmp6D3 == 
_tmp69B?t:(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp6D4=_cycalloc(sizeof(*
_tmp6D4));_tmp6D4[0]=({struct Cyc_Absyn_TypedefType_struct _tmp6D5;_tmp6D5.tag=16;
_tmp6D5.f1=_tmp69A;_tmp6D5.f2=_tmp6D3;_tmp6D5.f3=_tmp69C;_tmp6D5.f4=_tmp69D;
_tmp6D5;});_tmp6D4;});}_LL473: if(*((int*)_tmp68E)!= 7)goto _LL475;_tmp69E=((
struct Cyc_Absyn_ArrayType_struct*)_tmp68E)->f1;_tmp69F=(void*)_tmp69E.elt_type;
_tmp6A0=_tmp69E.tq;_tmp6A1=_tmp69E.num_elts;_tmp6A2=_tmp69E.zero_term;_tmp6A3=
_tmp69E.zt_loc;_LL474: {void*_tmp6D6=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp69F);
return _tmp6D6 == _tmp69F?t:(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp6D7=
_cycalloc(sizeof(*_tmp6D7));_tmp6D7[0]=({struct Cyc_Absyn_ArrayType_struct _tmp6D8;
_tmp6D8.tag=7;_tmp6D8.f1=({struct Cyc_Absyn_ArrayInfo _tmp6D9;_tmp6D9.elt_type=(
void*)_tmp6D6;_tmp6D9.tq=_tmp6A0;_tmp6D9.num_elts=_tmp6A1;_tmp6D9.zero_term=
_tmp6A2;_tmp6D9.zt_loc=_tmp6A3;_tmp6D9;});_tmp6D8;});_tmp6D7;});}_LL475: if(*((
int*)_tmp68E)!= 4)goto _LL477;_tmp6A4=((struct Cyc_Absyn_PointerType_struct*)
_tmp68E)->f1;_tmp6A5=(void*)_tmp6A4.elt_typ;_tmp6A6=_tmp6A4.elt_tq;_tmp6A7=
_tmp6A4.ptr_atts;_tmp6A8=(void*)_tmp6A7.rgn;_tmp6A9=_tmp6A7.nullable;_tmp6AA=
_tmp6A7.bounds;_tmp6AB=_tmp6A7.zero_term;_LL476: {void*_tmp6DA=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6A5);void*_tmp6DB=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6A8);struct
Cyc_Absyn_Conref*_tmp6DC=_tmp6AA;{union Cyc_Absyn_Constraint_union _tmp6DD=(Cyc_Absyn_compress_conref(
_tmp6AA))->v;void*_tmp6DE;void*_tmp6DF;_LL4A3: if((_tmp6DD.Eq_constr).tag != 0)
goto _LL4A5;_tmp6DE=(_tmp6DD.Eq_constr).f1;if(_tmp6DE <= (void*)2)goto _LL4A5;if(*((
int*)_tmp6DE)!= 1)goto _LL4A5;_tmp6DF=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp6DE)->f1;_LL4A4: {void*_tmp6E0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6DF);if(
_tmp6DF != _tmp6E0)_tmp6DC=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_AbsUpper_b_struct*
_tmp6E1=_cycalloc(sizeof(*_tmp6E1));_tmp6E1[0]=({struct Cyc_Absyn_AbsUpper_b_struct
_tmp6E2;_tmp6E2.tag=1;_tmp6E2.f1=(void*)_tmp6E0;_tmp6E2;});_tmp6E1;}));goto
_LL4A2;}_LL4A5:;_LL4A6: goto _LL4A2;_LL4A2:;}if((_tmp6DA == _tmp6A5  && _tmp6DB == 
_tmp6A8) && _tmp6DC == _tmp6AA)return t;return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp6E3=_cycalloc(sizeof(*_tmp6E3));_tmp6E3[0]=({struct Cyc_Absyn_PointerType_struct
_tmp6E4;_tmp6E4.tag=4;_tmp6E4.f1=({struct Cyc_Absyn_PtrInfo _tmp6E5;_tmp6E5.elt_typ=(
void*)_tmp6DA;_tmp6E5.elt_tq=_tmp6A6;_tmp6E5.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp6E6;_tmp6E6.rgn=(void*)_tmp6DB;_tmp6E6.nullable=_tmp6A9;_tmp6E6.bounds=
_tmp6DC;_tmp6E6.zero_term=_tmp6AB;_tmp6E6.ptrloc=0;_tmp6E6;});_tmp6E5;});_tmp6E4;});
_tmp6E3;});}_LL477: if(*((int*)_tmp68E)!= 8)goto _LL479;_tmp6AC=((struct Cyc_Absyn_FnType_struct*)
_tmp68E)->f1;_tmp6AD=_tmp6AC.tvars;_tmp6AE=_tmp6AC.effect;_tmp6AF=(void*)_tmp6AC.ret_typ;
_tmp6B0=_tmp6AC.args;_tmp6B1=_tmp6AC.c_varargs;_tmp6B2=_tmp6AC.cyc_varargs;
_tmp6B3=_tmp6AC.rgn_po;_tmp6B4=_tmp6AC.attributes;_LL478:{struct Cyc_List_List*
_tmp6E7=_tmp6AD;for(0;_tmp6E7 != 0;_tmp6E7=_tmp6E7->tl){inst=({struct Cyc_List_List*
_tmp6E8=_region_malloc(rgn,sizeof(*_tmp6E8));_tmp6E8->hd=({struct _tuple8*_tmp6E9=
_region_malloc(rgn,sizeof(*_tmp6E9));_tmp6E9->f1=(struct Cyc_Absyn_Tvar*)_tmp6E7->hd;
_tmp6E9->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp6EA=_cycalloc(sizeof(*
_tmp6EA));_tmp6EA[0]=({struct Cyc_Absyn_VarType_struct _tmp6EB;_tmp6EB.tag=1;
_tmp6EB.f1=(struct Cyc_Absyn_Tvar*)_tmp6E7->hd;_tmp6EB;});_tmp6EA;});_tmp6E9;});
_tmp6E8->tl=inst;_tmp6E8;});}}{struct Cyc_List_List*_tmp6ED;struct Cyc_List_List*
_tmp6EE;struct _tuple0 _tmp6EC=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple17*(*f)(struct _RegionHandle*,struct _tuple2*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmp6B0));_tmp6ED=_tmp6EC.f1;_tmp6EE=_tmp6EC.f2;{struct Cyc_List_List*_tmp6EF=
Cyc_Tcutil_substs(rgn,inst,_tmp6EE);struct Cyc_List_List*_tmp6F0=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp6ED,_tmp6EF));struct Cyc_Core_Opt*
eff2;if(_tmp6AE == 0)eff2=0;else{void*_tmp6F1=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp6AE->v);if(_tmp6F1 == (void*)_tmp6AE->v)eff2=_tmp6AE;else{eff2=({struct
Cyc_Core_Opt*_tmp6F2=_cycalloc(sizeof(*_tmp6F2));_tmp6F2->v=(void*)_tmp6F1;
_tmp6F2;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp6B2 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp6F4;struct Cyc_Absyn_Tqual _tmp6F5;void*
_tmp6F6;int _tmp6F7;struct Cyc_Absyn_VarargInfo _tmp6F3=*_tmp6B2;_tmp6F4=_tmp6F3.name;
_tmp6F5=_tmp6F3.tq;_tmp6F6=(void*)_tmp6F3.type;_tmp6F7=_tmp6F3.inject;{void*
_tmp6F8=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F6);if(_tmp6F8 == _tmp6F6)
cyc_varargs2=_tmp6B2;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp6F9=
_cycalloc(sizeof(*_tmp6F9));_tmp6F9->name=_tmp6F4;_tmp6F9->tq=_tmp6F5;_tmp6F9->type=(
void*)_tmp6F8;_tmp6F9->inject=_tmp6F7;_tmp6F9;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp6FB;struct Cyc_List_List*_tmp6FC;struct _tuple0 _tmp6FA=Cyc_List_rsplit(
rgn,rgn,_tmp6B3);_tmp6FB=_tmp6FA.f1;_tmp6FC=_tmp6FA.f2;{struct Cyc_List_List*
_tmp6FD=Cyc_Tcutil_substs(rgn,inst,_tmp6FB);struct Cyc_List_List*_tmp6FE=Cyc_Tcutil_substs(
rgn,inst,_tmp6FC);if(_tmp6FD == _tmp6FB  && _tmp6FE == _tmp6FC)rgn_po2=_tmp6B3;
else{rgn_po2=Cyc_List_zip(_tmp6FD,_tmp6FE);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp6FF=_cycalloc(sizeof(*_tmp6FF));_tmp6FF[0]=({struct Cyc_Absyn_FnType_struct
_tmp700;_tmp700.tag=8;_tmp700.f1=({struct Cyc_Absyn_FnInfo _tmp701;_tmp701.tvars=
_tmp6AD;_tmp701.effect=eff2;_tmp701.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp6AF);_tmp701.args=_tmp6F0;_tmp701.c_varargs=_tmp6B1;_tmp701.cyc_varargs=
cyc_varargs2;_tmp701.rgn_po=rgn_po2;_tmp701.attributes=_tmp6B4;_tmp701;});
_tmp700;});_tmp6FF;});}}}}}_LL479: if(*((int*)_tmp68E)!= 9)goto _LL47B;_tmp6B5=((
struct Cyc_Absyn_TupleType_struct*)_tmp68E)->f1;_LL47A: {struct Cyc_List_List*
_tmp702=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple4*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmp6B5);struct Cyc_List_List*
_tmp703=Cyc_Tcutil_substs(rgn,inst,_tmp702);if(_tmp703 == _tmp702)return t;{struct
Cyc_List_List*_tmp704=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct _tuple4*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmp6B5,_tmp703);return(void*)({struct Cyc_Absyn_TupleType_struct*_tmp705=
_cycalloc(sizeof(*_tmp705));_tmp705[0]=({struct Cyc_Absyn_TupleType_struct _tmp706;
_tmp706.tag=9;_tmp706.f1=_tmp704;_tmp706;});_tmp705;});}}_LL47B: if(*((int*)
_tmp68E)!= 11)goto _LL47D;_tmp6B6=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp68E)->f1;_tmp6B7=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp68E)->f2;_LL47C: {
struct Cyc_List_List*_tmp707=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp6B7);struct Cyc_List_List*_tmp708=Cyc_Tcutil_substs(rgn,inst,_tmp707);if(
_tmp708 == _tmp707)return t;{struct Cyc_List_List*_tmp709=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp6B7,
_tmp708);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp70A=_cycalloc(
sizeof(*_tmp70A));_tmp70A[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp70B;
_tmp70B.tag=11;_tmp70B.f1=(void*)_tmp6B6;_tmp70B.f2=_tmp709;_tmp70B;});_tmp70A;});}}
_LL47D: if(*((int*)_tmp68E)!= 0)goto _LL47F;_tmp6B8=((struct Cyc_Absyn_Evar_struct*)
_tmp68E)->f2;_LL47E: if(_tmp6B8 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)
_tmp6B8->v);else{return t;}_LL47F: if(*((int*)_tmp68E)!= 15)goto _LL481;_tmp6B9=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp68E)->f1;_LL480: {void*_tmp70C=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B9);return _tmp70C == _tmp6B9?t:(void*)({
struct Cyc_Absyn_RgnHandleType_struct*_tmp70D=_cycalloc(sizeof(*_tmp70D));_tmp70D[
0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp70E;_tmp70E.tag=15;_tmp70E.f1=(void*)
_tmp70C;_tmp70E;});_tmp70D;});}_LL481: if(*((int*)_tmp68E)!= 14)goto _LL483;
_tmp6BA=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp68E)->f1;_LL482: {void*
_tmp70F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6BA);return _tmp70F == _tmp6BA?t:(void*)({
struct Cyc_Absyn_SizeofType_struct*_tmp710=_cycalloc(sizeof(*_tmp710));_tmp710[0]=({
struct Cyc_Absyn_SizeofType_struct _tmp711;_tmp711.tag=14;_tmp711.f1=(void*)
_tmp70F;_tmp711;});_tmp710;});}_LL483: if(*((int*)_tmp68E)!= 17)goto _LL485;
_tmp6BB=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp68E)->f1;_LL484: {void*
_tmp712=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6BB);return _tmp712 == _tmp6BB?t:(void*)({
struct Cyc_Absyn_TagType_struct*_tmp713=_cycalloc(sizeof(*_tmp713));_tmp713[0]=({
struct Cyc_Absyn_TagType_struct _tmp714;_tmp714.tag=17;_tmp714.f1=(void*)_tmp712;
_tmp714;});_tmp713;});}_LL485: if(*((int*)_tmp68E)!= 18)goto _LL487;_LL486: goto
_LL488;_LL487: if(*((int*)_tmp68E)!= 12)goto _LL489;_LL488: goto _LL48A;_LL489: if(*((
int*)_tmp68E)!= 13)goto _LL48B;_LL48A: goto _LL48C;_LL48B: if((int)_tmp68E != 0)goto
_LL48D;_LL48C: goto _LL48E;_LL48D: if(_tmp68E <= (void*)4)goto _LL48F;if(*((int*)
_tmp68E)!= 5)goto _LL48F;_LL48E: goto _LL490;_LL48F: if((int)_tmp68E != 1)goto _LL491;
_LL490: goto _LL492;_LL491: if(_tmp68E <= (void*)4)goto _LL493;if(*((int*)_tmp68E)!= 
6)goto _LL493;_LL492: goto _LL494;_LL493: if((int)_tmp68E != 3)goto _LL495;_LL494: goto
_LL496;_LL495: if((int)_tmp68E != 2)goto _LL497;_LL496: return t;_LL497: if(_tmp68E <= (
void*)4)goto _LL499;if(*((int*)_tmp68E)!= 21)goto _LL499;_tmp6BC=(void*)((struct
Cyc_Absyn_RgnsEff_struct*)_tmp68E)->f1;_LL498: {void*_tmp715=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6BC);return _tmp715 == _tmp6BC?t:(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp716=_cycalloc(sizeof(*_tmp716));_tmp716[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp717;_tmp717.tag=21;_tmp717.f1=(void*)_tmp715;_tmp717;});_tmp716;});}_LL499:
if(_tmp68E <= (void*)4)goto _LL49B;if(*((int*)_tmp68E)!= 19)goto _LL49B;_tmp6BD=(
void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp68E)->f1;_LL49A: {void*_tmp718=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6BD);return _tmp718 == _tmp6BD?t:(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp719=_cycalloc(sizeof(*_tmp719));_tmp719[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp71A;_tmp71A.tag=19;_tmp71A.f1=(void*)_tmp718;_tmp71A;});_tmp719;});}_LL49B:
if(_tmp68E <= (void*)4)goto _LL468;if(*((int*)_tmp68E)!= 20)goto _LL468;_tmp6BE=((
struct Cyc_Absyn_JoinEff_struct*)_tmp68E)->f1;_LL49C: {struct Cyc_List_List*
_tmp71B=Cyc_Tcutil_substs(rgn,inst,_tmp6BE);return _tmp71B == _tmp6BE?t:(void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp71C=_cycalloc(sizeof(*_tmp71C));_tmp71C[0]=({
struct Cyc_Absyn_JoinEff_struct _tmp71D;_tmp71D.tag=20;_tmp71D.f1=_tmp71B;_tmp71D;});
_tmp71C;});}_LL468:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 0)
return 0;{void*_tmp71E=(void*)ts->hd;struct Cyc_List_List*_tmp71F=ts->tl;void*
_tmp720=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp71E);struct Cyc_List_List*_tmp721=Cyc_Tcutil_substs(
rgn,inst,_tmp71F);if(_tmp71E == _tmp720  && _tmp71F == _tmp721)return ts;return(
struct Cyc_List_List*)((struct Cyc_List_List*)({struct Cyc_List_List*_tmp722=
_cycalloc(sizeof(*_tmp722));_tmp722->hd=(void*)_tmp720;_tmp722->tl=_tmp721;
_tmp722;}));}}extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){void*k=Cyc_Tcutil_tvar_kind(tv);
return({struct _tuple8*_tmp723=_cycalloc(sizeof(*_tmp723));_tmp723->f1=tv;_tmp723->f2=
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp724=_cycalloc(sizeof(*_tmp724));
_tmp724->v=(void*)k;_tmp724;}),({struct Cyc_Core_Opt*_tmp725=_cycalloc(sizeof(*
_tmp725));_tmp725->v=s;_tmp725;}));_tmp723;});}struct _tuple8*Cyc_Tcutil_r_make_inst_var(
struct _tuple9*env,struct Cyc_Absyn_Tvar*tv){struct _tuple9 _tmp727;struct Cyc_List_List*
_tmp728;struct _RegionHandle*_tmp729;struct _tuple9*_tmp726=env;_tmp727=*_tmp726;
_tmp728=_tmp727.f1;_tmp729=_tmp727.f2;{void*k=Cyc_Tcutil_tvar_kind(tv);return({
struct _tuple8*_tmp72A=_region_malloc(_tmp729,sizeof(*_tmp72A));_tmp72A->f1=tv;
_tmp72A->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp72B=_cycalloc(sizeof(*
_tmp72B));_tmp72B->v=(void*)k;_tmp72B;}),({struct Cyc_Core_Opt*_tmp72C=_cycalloc(
sizeof(*_tmp72C));_tmp72C->v=_tmp728;_tmp72C;}));_tmp72A;});}}static struct Cyc_List_List*
Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(
void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(
k1,k2))({struct Cyc_String_pa_struct _tmp731;_tmp731.tag=0;_tmp731.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kindbound2string(k2));{
struct Cyc_String_pa_struct _tmp730;_tmp730.tag=0;_tmp730.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kindbound2string(k1));{
struct Cyc_String_pa_struct _tmp72F;_tmp72F.tag=0;_tmp72F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*tv->name);{void*_tmp72D[3]={& _tmp72F,&
_tmp730,& _tmp731};Cyc_Tcutil_terr(loc,({const char*_tmp72E="type variable %s is used with inconsistent kinds %s and %s";
_tag_dynforward(_tmp72E,sizeof(char),_get_zero_arr_size(_tmp72E,59));}),
_tag_dynforward(_tmp72D,sizeof(void*),3));}}}});if(tv->identity == 0)tv->identity=((
struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{if(*((int*)_check_null(tv->identity))
!= *((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)))({void*
_tmp732[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp733="same type variable has different identity!";_tag_dynforward(
_tmp733,sizeof(char),_get_zero_arr_size(_tmp733,43));}),_tag_dynforward(_tmp732,
sizeof(void*),0));});}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();return({
struct Cyc_List_List*_tmp734=_cycalloc(sizeof(*_tmp734));_tmp734->hd=tv;_tmp734->tl=
tvs;_tmp734;});}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({void*_tmp735[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp736="fast_add_free_tvar: bad identity in tv";_tag_dynforward(
_tmp736,sizeof(char),_get_zero_arr_size(_tmp736,39));}),_tag_dynforward(_tmp735,
sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity == 0)({void*_tmp737[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp738="fast_add_free_tvar: bad identity in tvs2";_tag_dynforward(_tmp738,
sizeof(char),_get_zero_arr_size(_tmp738,41));}),_tag_dynforward(_tmp737,sizeof(
void*),0));});if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity))
== *((int*)_check_null(tv->identity)))return tvs;}}return({struct Cyc_List_List*
_tmp739=_cycalloc(sizeof(*_tmp739));_tmp739->hd=tv;_tmp739->tl=tvs;_tmp739;});}
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({struct Cyc_String_pa_struct _tmp73C;
_tmp73C.tag=0;_tmp73C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcutil_tvar2string(
tv));{void*_tmp73A[1]={& _tmp73C};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp73B="bound tvar id for %s is NULL";
_tag_dynforward(_tmp73B,sizeof(char),_get_zero_arr_size(_tmp73B,29));}),
_tag_dynforward(_tmp73A,sizeof(void*),1));}});return({struct Cyc_List_List*
_tmp73D=_cycalloc(sizeof(*_tmp73D));_tmp73D->hd=tv;_tmp73D->tl=tvs;_tmp73D;});}
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct Cyc_List_List*es,void*e){
void*_tmp73E=Cyc_Tcutil_compress(e);int _tmp73F;_LL4A8: if(_tmp73E <= (void*)4)goto
_LL4AA;if(*((int*)_tmp73E)!= 0)goto _LL4AA;_tmp73F=((struct Cyc_Absyn_Evar_struct*)
_tmp73E)->f3;_LL4A9:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){void*
_tmp740=Cyc_Tcutil_compress((void*)es2->hd);int _tmp741;_LL4AD: if(_tmp740 <= (void*)
4)goto _LL4AF;if(*((int*)_tmp740)!= 0)goto _LL4AF;_tmp741=((struct Cyc_Absyn_Evar_struct*)
_tmp740)->f3;_LL4AE: if(_tmp73F == _tmp741)return es;goto _LL4AC;_LL4AF:;_LL4B0: goto
_LL4AC;_LL4AC:;}}return({struct Cyc_List_List*_tmp742=_cycalloc(sizeof(*_tmp742));
_tmp742->hd=(void*)e;_tmp742->tl=es;_tmp742;});_LL4AA:;_LL4AB: return es;_LL4A7:;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs){struct Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int
present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(*((int*)
_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))== *((int*)_check_null(((
struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;break;}}}if(!present)r=({
struct Cyc_List_List*_tmp743=_cycalloc(sizeof(*_tmp743));_tmp743->hd=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp743->tl=r;_tmp743;});}r=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dynforward_ptr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)width))({struct Cyc_String_pa_struct _tmp746;_tmp746.tag=0;
_tmp746.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*fn);{void*_tmp744[1]={&
_tmp746};Cyc_Tcutil_terr(loc,({const char*_tmp745="bitfield %s does not have constant width";
_tag_dynforward(_tmp745,sizeof(char),_get_zero_arr_size(_tmp745,41));}),
_tag_dynforward(_tmp744,sizeof(void*),1));}});else{unsigned int _tmp748;int
_tmp749;struct _tuple7 _tmp747=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
width);_tmp748=_tmp747.f1;_tmp749=_tmp747.f2;if(!_tmp749)({void*_tmp74A[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp74B="bitfield width cannot use sizeof or offsetof";
_tag_dynforward(_tmp74B,sizeof(char),_get_zero_arr_size(_tmp74B,45));}),
_tag_dynforward(_tmp74A,sizeof(void*),0));});w=_tmp748;}{void*_tmp74C=Cyc_Tcutil_compress(
field_typ);void*_tmp74D;_LL4B2: if(_tmp74C <= (void*)4)goto _LL4B4;if(*((int*)
_tmp74C)!= 5)goto _LL4B4;_tmp74D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp74C)->f2;
_LL4B3:{void*_tmp74E=_tmp74D;_LL4B7: if((int)_tmp74E != 0)goto _LL4B9;_LL4B8: if(w > 
8)({void*_tmp74F[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp750="bitfield larger than type";
_tag_dynforward(_tmp750,sizeof(char),_get_zero_arr_size(_tmp750,26));}),
_tag_dynforward(_tmp74F,sizeof(void*),0));});goto _LL4B6;_LL4B9: if((int)_tmp74E != 
1)goto _LL4BB;_LL4BA: if(w > 16)({void*_tmp751[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp752="bitfield larger than type";_tag_dynforward(_tmp752,sizeof(char),
_get_zero_arr_size(_tmp752,26));}),_tag_dynforward(_tmp751,sizeof(void*),0));});
goto _LL4B6;_LL4BB: if((int)_tmp74E != 3)goto _LL4BD;_LL4BC: goto _LL4BE;_LL4BD: if((
int)_tmp74E != 2)goto _LL4BF;_LL4BE: if(w > 32)({void*_tmp753[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp754="bitfield larger than type";_tag_dynforward(_tmp754,
sizeof(char),_get_zero_arr_size(_tmp754,26));}),_tag_dynforward(_tmp753,sizeof(
void*),0));});goto _LL4B6;_LL4BF: if((int)_tmp74E != 4)goto _LL4B6;_LL4C0: if(w > 64)({
void*_tmp755[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp756="bitfield larger than type";
_tag_dynforward(_tmp756,sizeof(char),_get_zero_arr_size(_tmp756,26));}),
_tag_dynforward(_tmp755,sizeof(void*),0));});goto _LL4B6;_LL4B6:;}goto _LL4B1;
_LL4B4:;_LL4B5:({struct Cyc_String_pa_struct _tmp75A;_tmp75A.tag=0;_tmp75A.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(field_typ));{
struct Cyc_String_pa_struct _tmp759;_tmp759.tag=0;_tmp759.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*fn);{void*_tmp757[2]={& _tmp759,& _tmp75A};
Cyc_Tcutil_terr(loc,({const char*_tmp758="bitfield %s must have integral type but has type %s";
_tag_dynforward(_tmp758,sizeof(char),_get_zero_arr_size(_tmp758,52));}),
_tag_dynforward(_tmp757,sizeof(void*),2));}}});goto _LL4B1;_LL4B1:;}}}static void
Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*loc,struct _dynforward_ptr*
fn,struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*_tmp75B=(void*)
atts->hd;_LL4C2: if((int)_tmp75B != 5)goto _LL4C4;_LL4C3: continue;_LL4C4: if(_tmp75B
<= (void*)17)goto _LL4C6;if(*((int*)_tmp75B)!= 1)goto _LL4C6;_LL4C5: continue;
_LL4C6:;_LL4C7:({struct Cyc_String_pa_struct _tmp75F;_tmp75F.tag=0;_tmp75F.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*fn);{struct Cyc_String_pa_struct
_tmp75E;_tmp75E.tag=0;_tmp75E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));{void*_tmp75C[2]={& _tmp75E,& _tmp75F};
Cyc_Tcutil_terr(loc,({const char*_tmp75D="bad attribute %s on member %s";
_tag_dynforward(_tmp75D,sizeof(char),_get_zero_arr_size(_tmp75D,30));}),
_tag_dynforward(_tmp75C,sizeof(void*),2));}}});_LL4C1:;}}struct Cyc_Tcutil_CVTEnv{
struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*
free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp760=t;struct
Cyc_Absyn_Typedefdecl*_tmp761;void**_tmp762;_LL4C9: if(_tmp760 <= (void*)4)goto
_LL4CB;if(*((int*)_tmp760)!= 16)goto _LL4CB;_tmp761=((struct Cyc_Absyn_TypedefType_struct*)
_tmp760)->f3;_tmp762=((struct Cyc_Absyn_TypedefType_struct*)_tmp760)->f4;_LL4CA:
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp761))->tq).real_const  || (
_tmp761->tq).print_const){if(declared_const)({void*_tmp763[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp764="extra const";_tag_dynforward(_tmp764,sizeof(char),
_get_zero_arr_size(_tmp764,12));}),_tag_dynforward(_tmp763,sizeof(void*),0));});
return 1;}if((unsigned int)_tmp762)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmp762);else{return declared_const;}_LL4CB:;_LL4CC: return
declared_const;_LL4C8:;}struct _tuple18{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,void*t,int allow_unique){{
void*_tmp765=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp766;struct Cyc_Core_Opt**
_tmp767;struct Cyc_Core_Opt*_tmp768;struct Cyc_Core_Opt**_tmp769;struct Cyc_Absyn_Tvar*
_tmp76A;struct Cyc_List_List*_tmp76B;struct _tuple1*_tmp76C;struct Cyc_Absyn_Enumdecl*
_tmp76D;struct Cyc_Absyn_Enumdecl**_tmp76E;struct Cyc_Absyn_TunionInfo _tmp76F;
union Cyc_Absyn_TunionInfoU_union _tmp770;union Cyc_Absyn_TunionInfoU_union*_tmp771;
struct Cyc_List_List*_tmp772;struct Cyc_List_List**_tmp773;struct Cyc_Core_Opt*
_tmp774;struct Cyc_Core_Opt**_tmp775;struct Cyc_Absyn_TunionFieldInfo _tmp776;union
Cyc_Absyn_TunionFieldInfoU_union _tmp777;union Cyc_Absyn_TunionFieldInfoU_union*
_tmp778;struct Cyc_List_List*_tmp779;struct Cyc_Absyn_PtrInfo _tmp77A;void*_tmp77B;
struct Cyc_Absyn_Tqual _tmp77C;struct Cyc_Absyn_Tqual*_tmp77D;struct Cyc_Absyn_PtrAtts
_tmp77E;void*_tmp77F;struct Cyc_Absyn_Conref*_tmp780;struct Cyc_Absyn_Conref*
_tmp781;struct Cyc_Absyn_Conref*_tmp782;void*_tmp783;void*_tmp784;struct Cyc_Absyn_ArrayInfo
_tmp785;void*_tmp786;struct Cyc_Absyn_Tqual _tmp787;struct Cyc_Absyn_Tqual*_tmp788;
struct Cyc_Absyn_Exp*_tmp789;struct Cyc_Absyn_Exp**_tmp78A;struct Cyc_Absyn_Conref*
_tmp78B;struct Cyc_Position_Segment*_tmp78C;struct Cyc_Absyn_FnInfo _tmp78D;struct
Cyc_List_List*_tmp78E;struct Cyc_List_List**_tmp78F;struct Cyc_Core_Opt*_tmp790;
struct Cyc_Core_Opt**_tmp791;void*_tmp792;struct Cyc_List_List*_tmp793;int _tmp794;
struct Cyc_Absyn_VarargInfo*_tmp795;struct Cyc_List_List*_tmp796;struct Cyc_List_List*
_tmp797;struct Cyc_List_List*_tmp798;void*_tmp799;struct Cyc_List_List*_tmp79A;
struct Cyc_Absyn_AggrInfo _tmp79B;union Cyc_Absyn_AggrInfoU_union _tmp79C;union Cyc_Absyn_AggrInfoU_union*
_tmp79D;struct Cyc_List_List*_tmp79E;struct Cyc_List_List**_tmp79F;struct _tuple1*
_tmp7A0;struct Cyc_List_List*_tmp7A1;struct Cyc_List_List**_tmp7A2;struct Cyc_Absyn_Typedefdecl*
_tmp7A3;struct Cyc_Absyn_Typedefdecl**_tmp7A4;void**_tmp7A5;void***_tmp7A6;void*
_tmp7A7;void*_tmp7A8;void*_tmp7A9;struct Cyc_List_List*_tmp7AA;_LL4CE: if((int)
_tmp765 != 0)goto _LL4D0;_LL4CF: goto _LL4CD;_LL4D0: if(_tmp765 <= (void*)4)goto _LL4E4;
if(*((int*)_tmp765)!= 0)goto _LL4D2;_tmp766=((struct Cyc_Absyn_Evar_struct*)
_tmp765)->f1;_tmp767=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp765)->f1;_tmp768=((struct Cyc_Absyn_Evar_struct*)_tmp765)->f2;_tmp769=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp765)->f2;_LL4D1: if(*_tmp767 == 
0)*_tmp767=({struct Cyc_Core_Opt*_tmp7AB=_cycalloc(sizeof(*_tmp7AB));_tmp7AB->v=(
void*)expected_kind;_tmp7AB;});if((cvtenv.fn_result  && cvtenv.generalize_evars)
 && expected_kind == (void*)3)*_tmp769=({struct Cyc_Core_Opt*_tmp7AC=_cycalloc(
sizeof(*_tmp7AC));_tmp7AC->v=(void*)((void*)2);_tmp7AC;});else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp7AD=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp7B1=_cycalloc(sizeof(*_tmp7B1));_tmp7B1[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp7B2;_tmp7B2.tag=2;_tmp7B2.f1=0;_tmp7B2.f2=(void*)expected_kind;_tmp7B2;});
_tmp7B1;}));*_tmp769=({struct Cyc_Core_Opt*_tmp7AE=_cycalloc(sizeof(*_tmp7AE));
_tmp7AE->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp7AF=_cycalloc(
sizeof(*_tmp7AF));_tmp7AF[0]=({struct Cyc_Absyn_VarType_struct _tmp7B0;_tmp7B0.tag=
1;_tmp7B0.f1=_tmp7AD;_tmp7B0;});_tmp7AF;}));_tmp7AE;});_tmp76A=_tmp7AD;goto
_LL4D3;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,t);}}
goto _LL4CD;_LL4D2: if(*((int*)_tmp765)!= 1)goto _LL4D4;_tmp76A=((struct Cyc_Absyn_VarType_struct*)
_tmp765)->f1;_LL4D3:{void*_tmp7B3=Cyc_Absyn_compress_kb((void*)_tmp76A->kind);
struct Cyc_Core_Opt*_tmp7B4;struct Cyc_Core_Opt**_tmp7B5;_LL503: if(*((int*)_tmp7B3)
!= 1)goto _LL505;_tmp7B4=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp7B3)->f1;
_tmp7B5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp7B3)->f1;
_LL504:*_tmp7B5=({struct Cyc_Core_Opt*_tmp7B6=_cycalloc(sizeof(*_tmp7B6));_tmp7B6->v=(
void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp7B7=_cycalloc(sizeof(*_tmp7B7));
_tmp7B7[0]=({struct Cyc_Absyn_Less_kb_struct _tmp7B8;_tmp7B8.tag=2;_tmp7B8.f1=0;
_tmp7B8.f2=(void*)expected_kind;_tmp7B8;});_tmp7B7;}));_tmp7B6;});goto _LL502;
_LL505:;_LL506: goto _LL502;_LL502:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmp76A);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,
_tmp76A);goto _LL4CD;_LL4D4: if(*((int*)_tmp765)!= 13)goto _LL4D6;_tmp76B=((struct
Cyc_Absyn_AnonEnumType_struct*)_tmp765)->f1;_LL4D5: {struct Cyc_Tcenv_Genv*ge=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);{struct _RegionHandle _tmp7B9=_new_region("uprev_rgn");struct
_RegionHandle*uprev_rgn=& _tmp7B9;_push_region(uprev_rgn);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmp76B != 0;_tmp76B=_tmp76B->tl){
struct Cyc_Absyn_Enumfield*_tmp7BA=(struct Cyc_Absyn_Enumfield*)_tmp76B->hd;if(((
int(*)(int(*compare)(struct _dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*
l,struct _dynforward_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp7BA->name).f2))({
struct Cyc_String_pa_struct _tmp7BD;_tmp7BD.tag=0;_tmp7BD.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*(*_tmp7BA->name).f2);{void*_tmp7BB[1]={&
_tmp7BD};Cyc_Tcutil_terr(_tmp7BA->loc,({const char*_tmp7BC="duplicate enum field name %s";
_tag_dynforward(_tmp7BC,sizeof(char),_get_zero_arr_size(_tmp7BC,29));}),
_tag_dynforward(_tmp7BB,sizeof(void*),1));}});else{prev_fields=({struct Cyc_List_List*
_tmp7BE=_region_malloc(uprev_rgn,sizeof(*_tmp7BE));_tmp7BE->hd=(*_tmp7BA->name).f2;
_tmp7BE->tl=prev_fields;_tmp7BE;});}if(_tmp7BA->tag == 0)_tmp7BA->tag=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(tag_count,_tmp7BA->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)_check_null(_tmp7BA->tag)))({struct Cyc_String_pa_struct
_tmp7C1;_tmp7C1.tag=0;_tmp7C1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*(*
_tmp7BA->name).f2);{void*_tmp7BF[1]={& _tmp7C1};Cyc_Tcutil_terr(loc,({const char*
_tmp7C0="enum field %s: expression is not constant";_tag_dynforward(_tmp7C0,
sizeof(char),_get_zero_arr_size(_tmp7C0,42));}),_tag_dynforward(_tmp7BF,sizeof(
void*),1));}});}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp7BA->tag))).f1;tag_count=t1 + 1;(*_tmp7BA->name).f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp7C2;(_tmp7C2.Abs_n).tag=2;(_tmp7C2.Abs_n).f1=te->ns;
_tmp7C2;});ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dynforward_ptr*k,struct _tuple18*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp7BA->name).f2,(
struct _tuple18*)({struct _tuple18*_tmp7C3=_cycalloc(sizeof(*_tmp7C3));_tmp7C3->f1=(
void*)({struct Cyc_Tcenv_AnonEnumRes_struct*_tmp7C4=_cycalloc(sizeof(*_tmp7C4));
_tmp7C4[0]=({struct Cyc_Tcenv_AnonEnumRes_struct _tmp7C5;_tmp7C5.tag=4;_tmp7C5.f1=(
void*)t;_tmp7C5.f2=_tmp7BA;_tmp7C5;});_tmp7C4;});_tmp7C3->f2=1;_tmp7C3;}));}}};
_pop_region(uprev_rgn);}goto _LL4CD;}_LL4D6: if(*((int*)_tmp765)!= 12)goto _LL4D8;
_tmp76C=((struct Cyc_Absyn_EnumType_struct*)_tmp765)->f1;_tmp76D=((struct Cyc_Absyn_EnumType_struct*)
_tmp765)->f2;_tmp76E=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_struct*)
_tmp765)->f2;_LL4D7: if(*_tmp76E == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*
_tmp76E))->fields == 0){struct _handler_cons _tmp7C6;_push_handler(& _tmp7C6);{int
_tmp7C8=0;if(setjmp(_tmp7C6.handler))_tmp7C8=1;if(!_tmp7C8){{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp76C);*_tmp76E=(struct Cyc_Absyn_Enumdecl*)*
ed;};_pop_handler();}else{void*_tmp7C7=(void*)_exn_thrown;void*_tmp7CA=_tmp7C7;
_LL508: if(_tmp7CA != Cyc_Dict_Absent)goto _LL50A;_LL509: {struct Cyc_Tcenv_Genv*
_tmp7CB=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Enumdecl*_tmp7CC=({struct Cyc_Absyn_Enumdecl*
_tmp7CD=_cycalloc(sizeof(*_tmp7CD));_tmp7CD->sc=(void*)((void*)3);_tmp7CD->name=
_tmp76C;_tmp7CD->fields=0;_tmp7CD;});Cyc_Tc_tcEnumdecl(te,_tmp7CB,loc,_tmp7CC);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp76C);*_tmp76E=(
struct Cyc_Absyn_Enumdecl*)*ed;goto _LL507;}}_LL50A:;_LL50B:(void)_throw(_tmp7CA);
_LL507:;}}}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*
_tmp76E);*_tmp76C=(ed->name)[_check_known_subscript_notnull(1,0)];goto _LL4CD;}
_LL4D8: if(*((int*)_tmp765)!= 2)goto _LL4DA;_tmp76F=((struct Cyc_Absyn_TunionType_struct*)
_tmp765)->f1;_tmp770=_tmp76F.tunion_info;_tmp771=(union Cyc_Absyn_TunionInfoU_union*)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp765)->f1).tunion_info;_tmp772=_tmp76F.targs;
_tmp773=(struct Cyc_List_List**)&(((struct Cyc_Absyn_TunionType_struct*)_tmp765)->f1).targs;
_tmp774=_tmp76F.rgn;_tmp775=(struct Cyc_Core_Opt**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp765)->f1).rgn;_LL4D9: {struct Cyc_List_List*_tmp7CE=*_tmp773;{union Cyc_Absyn_TunionInfoU_union
_tmp7CF=*_tmp771;struct Cyc_Absyn_UnknownTunionInfo _tmp7D0;struct _tuple1*_tmp7D1;
int _tmp7D2;int _tmp7D3;struct Cyc_Absyn_Tuniondecl**_tmp7D4;struct Cyc_Absyn_Tuniondecl*
_tmp7D5;_LL50D: if((_tmp7CF.UnknownTunion).tag != 0)goto _LL50F;_tmp7D0=(_tmp7CF.UnknownTunion).f1;
_tmp7D1=_tmp7D0.name;_tmp7D2=_tmp7D0.is_xtunion;_tmp7D3=_tmp7D0.is_flat;_LL50E: {
struct Cyc_Absyn_Tuniondecl**tudp;{struct _handler_cons _tmp7D6;_push_handler(&
_tmp7D6);{int _tmp7D8=0;if(setjmp(_tmp7D6.handler))_tmp7D8=1;if(!_tmp7D8){tudp=
Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp7D1);;_pop_handler();}else{void*_tmp7D7=(
void*)_exn_thrown;void*_tmp7DA=_tmp7D7;_LL512: if(_tmp7DA != Cyc_Dict_Absent)goto
_LL514;_LL513: {struct Cyc_Tcenv_Genv*_tmp7DB=((struct Cyc_Tcenv_Genv*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*
_tmp7DC=({struct Cyc_Absyn_Tuniondecl*_tmp7E3=_cycalloc(sizeof(*_tmp7E3));_tmp7E3->sc=(
void*)((void*)3);_tmp7E3->name=_tmp7D1;_tmp7E3->tvs=0;_tmp7E3->fields=0;_tmp7E3->is_xtunion=
_tmp7D2;_tmp7E3->is_flat=_tmp7D3;_tmp7E3;});Cyc_Tc_tcTuniondecl(te,_tmp7DB,loc,
_tmp7DC);tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp7D1);if(_tmp7CE != 0){({
struct Cyc_String_pa_struct _tmp7E0;_tmp7E0.tag=0;_tmp7E0.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7D1));{
struct Cyc_String_pa_struct _tmp7DF;_tmp7DF.tag=0;_tmp7DF.f1=(struct
_dynforward_ptr)(_tmp7D2?(struct _dynforward_ptr)((struct _dynforward_ptr)({const
char*_tmp7E1="xtunion";_tag_dynforward(_tmp7E1,sizeof(char),_get_zero_arr_size(
_tmp7E1,8));})):(struct _dynforward_ptr)({const char*_tmp7E2="tunion";
_tag_dynforward(_tmp7E2,sizeof(char),_get_zero_arr_size(_tmp7E2,7));}));{void*
_tmp7DD[2]={& _tmp7DF,& _tmp7E0};Cyc_Tcutil_terr(loc,({const char*_tmp7DE="declare parameterized %s %s before using";
_tag_dynforward(_tmp7DE,sizeof(char),_get_zero_arr_size(_tmp7DE,41));}),
_tag_dynforward(_tmp7DD,sizeof(void*),2));}}});return cvtenv;}goto _LL511;}_LL514:;
_LL515:(void)_throw(_tmp7DA);_LL511:;}}}if((*tudp)->is_xtunion != _tmp7D2)({
struct Cyc_String_pa_struct _tmp7E6;_tmp7E6.tag=0;_tmp7E6.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7D1));{void*
_tmp7E4[1]={& _tmp7E6};Cyc_Tcutil_terr(loc,({const char*_tmp7E5="[x]tunion is different from type declaration %s";
_tag_dynforward(_tmp7E5,sizeof(char),_get_zero_arr_size(_tmp7E5,48));}),
_tag_dynforward(_tmp7E4,sizeof(void*),1));}});*_tmp771=(union Cyc_Absyn_TunionInfoU_union)({
union Cyc_Absyn_TunionInfoU_union _tmp7E7;(_tmp7E7.KnownTunion).tag=1;(_tmp7E7.KnownTunion).f1=
tudp;_tmp7E7;});_tmp7D5=*tudp;goto _LL510;}_LL50F: if((_tmp7CF.KnownTunion).tag != 
1)goto _LL50C;_tmp7D4=(_tmp7CF.KnownTunion).f1;_tmp7D5=*_tmp7D4;_LL510: if(_tmp7D5->is_flat
 && (unsigned int)*_tmp775)({struct Cyc_String_pa_struct _tmp7EA;_tmp7EA.tag=0;
_tmp7EA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(*_tmp775))->v));{void*_tmp7E8[1]={&
_tmp7EA};Cyc_Tcutil_terr(loc,({const char*_tmp7E9="flat tunion has region %s";
_tag_dynforward(_tmp7E9,sizeof(char),_get_zero_arr_size(_tmp7E9,26));}),
_tag_dynforward(_tmp7E8,sizeof(void*),1));}});if(!_tmp7D5->is_flat  && !((
unsigned int)*_tmp775))*_tmp775=({struct Cyc_Core_Opt*_tmp7EB=_cycalloc(sizeof(*
_tmp7EB));_tmp7EB->v=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp7EC=
_cycalloc(sizeof(*_tmp7EC));_tmp7EC->v=(void*)((void*)3);_tmp7EC;}),0);_tmp7EB;});
if((unsigned int)*_tmp775){void*_tmp7ED=(void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp775))->v;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp7ED,
allow_unique);}{struct Cyc_List_List*tvs=_tmp7D5->tvs;for(0;_tmp7CE != 0  && tvs != 
0;(_tmp7CE=_tmp7CE->tl,tvs=tvs->tl)){void*t1=(void*)_tmp7CE->hd;void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k1,t1,0);}if(_tmp7CE != 0)({struct Cyc_String_pa_struct _tmp7F0;_tmp7F0.tag=
0;_tmp7F0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp7D5->name));{void*_tmp7EE[1]={& _tmp7F0};Cyc_Tcutil_terr(loc,({const char*
_tmp7EF="too many type arguments for tunion %s";_tag_dynforward(_tmp7EF,sizeof(
char),_get_zero_arr_size(_tmp7EF,38));}),_tag_dynforward(_tmp7EE,sizeof(void*),1));}});
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);hidden_ts=({struct Cyc_List_List*_tmp7F1=_cycalloc(sizeof(*_tmp7F1));_tmp7F1->hd=(
void*)e;_tmp7F1->tl=hidden_ts;_tmp7F1;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,e,0);}*_tmp773=Cyc_List_imp_append(*_tmp773,Cyc_List_imp_rev(
hidden_ts));}goto _LL50C;}_LL50C:;}goto _LL4CD;}_LL4DA: if(*((int*)_tmp765)!= 3)
goto _LL4DC;_tmp776=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp765)->f1;
_tmp777=_tmp776.field_info;_tmp778=(union Cyc_Absyn_TunionFieldInfoU_union*)&(((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp765)->f1).field_info;_tmp779=_tmp776.targs;
_LL4DB:{union Cyc_Absyn_TunionFieldInfoU_union _tmp7F2=*_tmp778;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp7F3;struct _tuple1*_tmp7F4;struct _tuple1*_tmp7F5;int _tmp7F6;struct Cyc_Absyn_Tuniondecl*
_tmp7F7;struct Cyc_Absyn_Tunionfield*_tmp7F8;_LL517: if((_tmp7F2.UnknownTunionfield).tag
!= 0)goto _LL519;_tmp7F3=(_tmp7F2.UnknownTunionfield).f1;_tmp7F4=_tmp7F3.tunion_name;
_tmp7F5=_tmp7F3.field_name;_tmp7F6=_tmp7F3.is_xtunion;_LL518: {struct Cyc_Absyn_Tuniondecl*
tud;struct Cyc_Absyn_Tunionfield*tuf;{struct _handler_cons _tmp7F9;_push_handler(&
_tmp7F9);{int _tmp7FB=0;if(setjmp(_tmp7F9.handler))_tmp7FB=1;if(!_tmp7FB){*Cyc_Tcenv_lookup_tuniondecl(
te,loc,_tmp7F4);;_pop_handler();}else{void*_tmp7FA=(void*)_exn_thrown;void*
_tmp7FD=_tmp7FA;_LL51C: if(_tmp7FD != Cyc_Dict_Absent)goto _LL51E;_LL51D:({struct
Cyc_String_pa_struct _tmp800;_tmp800.tag=0;_tmp800.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7F4));{void*_tmp7FE[1]={&
_tmp800};Cyc_Tcutil_terr(loc,({const char*_tmp7FF="unbound type tunion %s";
_tag_dynforward(_tmp7FF,sizeof(char),_get_zero_arr_size(_tmp7FF,23));}),
_tag_dynforward(_tmp7FE,sizeof(void*),1));}});return cvtenv;_LL51E:;_LL51F:(void)
_throw(_tmp7FD);_LL51B:;}}}{struct _handler_cons _tmp801;_push_handler(& _tmp801);{
int _tmp803=0;if(setjmp(_tmp801.handler))_tmp803=1;if(!_tmp803){{struct
_RegionHandle _tmp804=_new_region("r");struct _RegionHandle*r=& _tmp804;
_push_region(r);{void*_tmp805=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmp7F5);struct
Cyc_Absyn_Tuniondecl*_tmp806;struct Cyc_Absyn_Tunionfield*_tmp807;_LL521: if(*((
int*)_tmp805)!= 2)goto _LL523;_tmp806=((struct Cyc_Tcenv_TunionRes_struct*)_tmp805)->f1;
_tmp807=((struct Cyc_Tcenv_TunionRes_struct*)_tmp805)->f2;_LL522: tuf=_tmp807;tud=
_tmp806;if(tud->is_xtunion != _tmp7F6)({struct Cyc_String_pa_struct _tmp80A;_tmp80A.tag=
0;_tmp80A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp7F4));{void*_tmp808[1]={& _tmp80A};Cyc_Tcutil_terr(loc,({const char*_tmp809="[x]tunion is different from type declaration %s";
_tag_dynforward(_tmp809,sizeof(char),_get_zero_arr_size(_tmp809,48));}),
_tag_dynforward(_tmp808,sizeof(void*),1));}});goto _LL520;_LL523:;_LL524:({struct
Cyc_String_pa_struct _tmp80E;_tmp80E.tag=0;_tmp80E.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7F4));{struct Cyc_String_pa_struct
_tmp80D;_tmp80D.tag=0;_tmp80D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp7F5));{void*_tmp80B[2]={& _tmp80D,& _tmp80E};Cyc_Tcutil_terr(
loc,({const char*_tmp80C="unbound field %s in type tunion %s";_tag_dynforward(
_tmp80C,sizeof(char),_get_zero_arr_size(_tmp80C,35));}),_tag_dynforward(_tmp80B,
sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv _tmp80F=cvtenv;_npop_handler(1);
return _tmp80F;}_LL520:;};_pop_region(r);};_pop_handler();}else{void*_tmp802=(
void*)_exn_thrown;void*_tmp811=_tmp802;_LL526: if(_tmp811 != Cyc_Dict_Absent)goto
_LL528;_LL527:({struct Cyc_String_pa_struct _tmp815;_tmp815.tag=0;_tmp815.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7F4));{
struct Cyc_String_pa_struct _tmp814;_tmp814.tag=0;_tmp814.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7F5));{void*
_tmp812[2]={& _tmp814,& _tmp815};Cyc_Tcutil_terr(loc,({const char*_tmp813="unbound field %s in type tunion %s";
_tag_dynforward(_tmp813,sizeof(char),_get_zero_arr_size(_tmp813,35));}),
_tag_dynforward(_tmp812,sizeof(void*),2));}}});return cvtenv;_LL528:;_LL529:(void)
_throw(_tmp811);_LL525:;}}}*_tmp778=(union Cyc_Absyn_TunionFieldInfoU_union)({
union Cyc_Absyn_TunionFieldInfoU_union _tmp816;(_tmp816.KnownTunionfield).tag=1;(
_tmp816.KnownTunionfield).f1=tud;(_tmp816.KnownTunionfield).f2=tuf;_tmp816;});
_tmp7F7=tud;_tmp7F8=tuf;goto _LL51A;}_LL519: if((_tmp7F2.KnownTunionfield).tag != 1)
goto _LL516;_tmp7F7=(_tmp7F2.KnownTunionfield).f1;_tmp7F8=(_tmp7F2.KnownTunionfield).f2;
_LL51A: {struct Cyc_List_List*tvs=_tmp7F7->tvs;for(0;_tmp779 != 0  && tvs != 0;(
_tmp779=_tmp779->tl,tvs=tvs->tl)){void*t1=(void*)_tmp779->hd;void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k1,t1,0);}if(_tmp779 != 0)({struct Cyc_String_pa_struct _tmp81A;_tmp81A.tag=
0;_tmp81A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp7F8->name));{struct Cyc_String_pa_struct _tmp819;_tmp819.tag=0;_tmp819.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7F7->name));{
void*_tmp817[2]={& _tmp819,& _tmp81A};Cyc_Tcutil_terr(loc,({const char*_tmp818="too many type arguments for tunion %s.%s";
_tag_dynforward(_tmp818,sizeof(char),_get_zero_arr_size(_tmp818,41));}),
_tag_dynforward(_tmp817,sizeof(void*),2));}}});if(tvs != 0)({struct Cyc_String_pa_struct
_tmp81E;_tmp81E.tag=0;_tmp81E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp7F8->name));{struct Cyc_String_pa_struct _tmp81D;
_tmp81D.tag=0;_tmp81D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp7F7->name));{void*_tmp81B[2]={& _tmp81D,& _tmp81E};Cyc_Tcutil_terr(loc,({const
char*_tmp81C="too few type arguments for tunion %s.%s";_tag_dynforward(_tmp81C,
sizeof(char),_get_zero_arr_size(_tmp81C,40));}),_tag_dynforward(_tmp81B,sizeof(
void*),2));}}});goto _LL516;}_LL516:;}goto _LL4CD;_LL4DC: if(*((int*)_tmp765)!= 4)
goto _LL4DE;_tmp77A=((struct Cyc_Absyn_PointerType_struct*)_tmp765)->f1;_tmp77B=(
void*)_tmp77A.elt_typ;_tmp77C=_tmp77A.elt_tq;_tmp77D=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_PointerType_struct*)_tmp765)->f1).elt_tq;_tmp77E=_tmp77A.ptr_atts;
_tmp77F=(void*)_tmp77E.rgn;_tmp780=_tmp77E.nullable;_tmp781=_tmp77E.bounds;
_tmp782=_tmp77E.zero_term;_LL4DD: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp77B,0);_tmp77D->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp77D->print_const,_tmp77B);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp77F,allow_unique);{union Cyc_Absyn_Constraint_union _tmp81F=(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp782))->v;int _tmp820;_LL52B: if((_tmp81F.No_constr).tag != 2)goto _LL52D;_LL52C:{
void*_tmp821=Cyc_Tcutil_compress(_tmp77B);void*_tmp822;_LL532: if(_tmp821 <= (void*)
4)goto _LL534;if(*((int*)_tmp821)!= 5)goto _LL534;_tmp822=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp821)->f2;if((int)_tmp822 != 0)goto _LL534;_LL533:((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp782,Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL531;_LL534:;_LL535:((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp782,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL531;
_LL531:;}goto _LL52A;_LL52D: if((_tmp81F.Eq_constr).tag != 0)goto _LL52F;_tmp820=(
_tmp81F.Eq_constr).f1;if(_tmp820 != 1)goto _LL52F;_LL52E: if(!Cyc_Tcutil_admits_zero(
_tmp77B))({struct Cyc_String_pa_struct _tmp825;_tmp825.tag=0;_tmp825.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp77B));{void*
_tmp823[1]={& _tmp825};Cyc_Tcutil_terr(loc,({const char*_tmp824="cannot have a pointer to zero-terminated %s elements";
_tag_dynforward(_tmp824,sizeof(char),_get_zero_arr_size(_tmp824,53));}),
_tag_dynforward(_tmp823,sizeof(void*),1));}});is_zero_terminated=1;goto _LL52A;
_LL52F:;_LL530: is_zero_terminated=0;goto _LL52A;_LL52A:;}{union Cyc_Absyn_Constraint_union
_tmp826=(Cyc_Absyn_compress_conref(_tmp781))->v;void*_tmp827;void*_tmp828;void*
_tmp829;struct Cyc_Absyn_Exp*_tmp82A;void*_tmp82B;void*_tmp82C;_LL537: if((_tmp826.No_constr).tag
!= 2)goto _LL539;_LL538: goto _LL53A;_LL539: if((_tmp826.Eq_constr).tag != 0)goto
_LL53B;_tmp827=(_tmp826.Eq_constr).f1;if((int)_tmp827 != 0)goto _LL53B;_LL53A: goto
_LL536;_LL53B: if((_tmp826.Eq_constr).tag != 0)goto _LL53D;_tmp828=(_tmp826.Eq_constr).f1;
if((int)_tmp828 != 1)goto _LL53D;_LL53C: goto _LL536;_LL53D: if((_tmp826.Eq_constr).tag
!= 0)goto _LL53F;_tmp829=(_tmp826.Eq_constr).f1;if(_tmp829 <= (void*)2)goto _LL53F;
if(*((int*)_tmp829)!= 0)goto _LL53F;_tmp82A=((struct Cyc_Absyn_Upper_b_struct*)
_tmp829)->f1;_LL53E: Cyc_Tcexp_tcExp(te,0,_tmp82A);if(!Cyc_Tcutil_coerce_uint_typ(
te,_tmp82A))({void*_tmp82D[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp82E="pointer bounds expression is not an unsigned int";
_tag_dynforward(_tmp82E,sizeof(char),_get_zero_arr_size(_tmp82E,49));}),
_tag_dynforward(_tmp82D,sizeof(void*),0));});{unsigned int _tmp830;int _tmp831;
struct _tuple7 _tmp82F=Cyc_Evexp_eval_const_uint_exp(_tmp82A);_tmp830=_tmp82F.f1;
_tmp831=_tmp82F.f2;if(is_zero_terminated  && (!_tmp831  || _tmp830 < 1))({void*
_tmp832[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp833="zero-terminated pointer cannot point to empty sequence";
_tag_dynforward(_tmp833,sizeof(char),_get_zero_arr_size(_tmp833,55));}),
_tag_dynforward(_tmp832,sizeof(void*),0));});goto _LL536;}_LL53F: if((_tmp826.Eq_constr).tag
!= 0)goto _LL541;_tmp82B=(_tmp826.Eq_constr).f1;if(_tmp82B <= (void*)2)goto _LL541;
if(*((int*)_tmp82B)!= 1)goto _LL541;_tmp82C=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp82B)->f1;_LL540: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp82C,0);goto _LL536;_LL541: if((_tmp826.Forward_constr).tag != 1)goto _LL536;
_LL542:({void*_tmp834[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp835="forward constraint";
_tag_dynforward(_tmp835,sizeof(char),_get_zero_arr_size(_tmp835,19));}),
_tag_dynforward(_tmp834,sizeof(void*),0));});_LL536:;}goto _LL4CD;}_LL4DE: if(*((
int*)_tmp765)!= 17)goto _LL4E0;_tmp783=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp765)->f1;_LL4DF: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp783,allow_unique);goto _LL4CD;_LL4E0: if(*((int*)_tmp765)!= 14)goto _LL4E2;
_tmp784=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp765)->f1;_LL4E1: cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp784,allow_unique);goto
_LL4CD;_LL4E2: if(*((int*)_tmp765)!= 5)goto _LL4E4;_LL4E3: goto _LL4E5;_LL4E4: if((
int)_tmp765 != 1)goto _LL4E6;_LL4E5: goto _LL4E7;_LL4E6: if(_tmp765 <= (void*)4)goto
_LL4F4;if(*((int*)_tmp765)!= 6)goto _LL4E8;_LL4E7: goto _LL4CD;_LL4E8: if(*((int*)
_tmp765)!= 7)goto _LL4EA;_tmp785=((struct Cyc_Absyn_ArrayType_struct*)_tmp765)->f1;
_tmp786=(void*)_tmp785.elt_type;_tmp787=_tmp785.tq;_tmp788=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp765)->f1).tq;_tmp789=_tmp785.num_elts;
_tmp78A=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp765)->f1).num_elts;
_tmp78B=_tmp785.zero_term;_tmp78C=_tmp785.zt_loc;_LL4E9: {struct Cyc_Absyn_Exp*
_tmp836=*_tmp78A;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,
_tmp786,0);_tmp788->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp788->print_const,
_tmp786);{int is_zero_terminated;{union Cyc_Absyn_Constraint_union _tmp837=(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp78B))->v;int _tmp838;_LL544: if((_tmp837.No_constr).tag != 2)goto _LL546;_LL545:((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp78B,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL543;
_LL546: if((_tmp837.Eq_constr).tag != 0)goto _LL548;_tmp838=(_tmp837.Eq_constr).f1;
if(_tmp838 != 1)goto _LL548;_LL547: if(!Cyc_Tcutil_admits_zero(_tmp786))({struct Cyc_String_pa_struct
_tmp83B;_tmp83B.tag=0;_tmp83B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(_tmp786));{void*_tmp839[1]={& _tmp83B};Cyc_Tcutil_terr(loc,({
const char*_tmp83A="cannot have a zero-terminated array of %s elements";
_tag_dynforward(_tmp83A,sizeof(char),_get_zero_arr_size(_tmp83A,51));}),
_tag_dynforward(_tmp839,sizeof(void*),1));}});is_zero_terminated=1;goto _LL543;
_LL548:;_LL549: is_zero_terminated=0;goto _LL543;_LL543:;}if(_tmp836 == 0){if(
is_zero_terminated)*_tmp78A=(_tmp836=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,
0));else{({void*_tmp83C[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp83D="array bound defaults to 0 here";
_tag_dynforward(_tmp83D,sizeof(char),_get_zero_arr_size(_tmp83D,31));}),
_tag_dynforward(_tmp83C,sizeof(void*),0));});*_tmp78A=(_tmp836=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(0,0));}}Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp836);
if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_tmp836))({void*_tmp83E[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp83F="array bounds expression is not constant";
_tag_dynforward(_tmp83F,sizeof(char),_get_zero_arr_size(_tmp83F,40));}),
_tag_dynforward(_tmp83E,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_uint_typ(te,(
struct Cyc_Absyn_Exp*)_tmp836))({void*_tmp840[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp841="array bounds expression is not an unsigned int";_tag_dynforward(
_tmp841,sizeof(char),_get_zero_arr_size(_tmp841,47));}),_tag_dynforward(_tmp840,
sizeof(void*),0));});{unsigned int _tmp843;int _tmp844;struct _tuple7 _tmp842=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp836);_tmp843=_tmp842.f1;_tmp844=_tmp842.f2;if((
is_zero_terminated  && _tmp844) && _tmp843 < 1)({void*_tmp845[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp846="zero terminated array cannot have zero elements";
_tag_dynforward(_tmp846,sizeof(char),_get_zero_arr_size(_tmp846,48));}),
_tag_dynforward(_tmp845,sizeof(void*),0));});goto _LL4CD;}}}_LL4EA: if(*((int*)
_tmp765)!= 8)goto _LL4EC;_tmp78D=((struct Cyc_Absyn_FnType_struct*)_tmp765)->f1;
_tmp78E=_tmp78D.tvars;_tmp78F=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp765)->f1).tvars;_tmp790=_tmp78D.effect;_tmp791=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp765)->f1).effect;_tmp792=(void*)_tmp78D.ret_typ;
_tmp793=_tmp78D.args;_tmp794=_tmp78D.c_varargs;_tmp795=_tmp78D.cyc_varargs;
_tmp796=_tmp78D.rgn_po;_tmp797=_tmp78D.attributes;_LL4EB: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp797 != 0;_tmp797=_tmp797->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp797->hd))({struct Cyc_String_pa_struct _tmp849;
_tmp849.tag=0;_tmp849.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absyn_attribute2string((
void*)_tmp797->hd));{void*_tmp847[1]={& _tmp849};Cyc_Tcutil_terr(loc,({const char*
_tmp848="bad function type attribute %s";_tag_dynforward(_tmp848,sizeof(char),
_get_zero_arr_size(_tmp848,31));}),_tag_dynforward(_tmp847,sizeof(void*),1));}});{
void*_tmp84A=(void*)_tmp797->hd;void*_tmp84B;int _tmp84C;int _tmp84D;_LL54B: if((
int)_tmp84A != 0)goto _LL54D;_LL54C: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}
goto _LL54A;_LL54D: if((int)_tmp84A != 1)goto _LL54F;_LL54E: if(!seen_cdecl){
seen_cdecl=1;++ num_convs;}goto _LL54A;_LL54F: if((int)_tmp84A != 2)goto _LL551;
_LL550: if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL54A;_LL551: if(
_tmp84A <= (void*)17)goto _LL553;if(*((int*)_tmp84A)!= 3)goto _LL553;_tmp84B=(void*)((
struct Cyc_Absyn_Format_att_struct*)_tmp84A)->f1;_tmp84C=((struct Cyc_Absyn_Format_att_struct*)
_tmp84A)->f2;_tmp84D=((struct Cyc_Absyn_Format_att_struct*)_tmp84A)->f3;_LL552:
if(!seen_format){seen_format=1;ft=_tmp84B;fmt_desc_arg=_tmp84C;fmt_arg_start=
_tmp84D;}else{({void*_tmp84E[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp84F="function can't have multiple format attributes";
_tag_dynforward(_tmp84F,sizeof(char),_get_zero_arr_size(_tmp84F,47));}),
_tag_dynforward(_tmp84E,sizeof(void*),0));});}goto _LL54A;_LL553:;_LL554: goto
_LL54A;_LL54A:;}}if(num_convs > 1)({void*_tmp850[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp851="function can't have multiple calling conventions";
_tag_dynforward(_tmp851,sizeof(char),_get_zero_arr_size(_tmp851,49));}),
_tag_dynforward(_tmp850,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*
_tmp78F);{struct Cyc_List_List*b=*_tmp78F;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)
b->hd)->identity=Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(
cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp852=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp853;_LL556: if(*((int*)
_tmp852)!= 0)goto _LL558;_tmp853=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp852)->f1;
if((int)_tmp853 != 1)goto _LL558;_LL557:({struct Cyc_String_pa_struct _tmp856;
_tmp856.tag=0;_tmp856.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)b->hd)->name);{void*_tmp854[1]={& _tmp856};Cyc_Tcutil_terr(
loc,({const char*_tmp855="function attempts to abstract Mem type variable %s";
_tag_dynforward(_tmp855,sizeof(char),_get_zero_arr_size(_tmp855,51));}),
_tag_dynforward(_tmp854,sizeof(void*),1));}});goto _LL555;_LL558:;_LL559: goto
_LL555;_LL555:;}}}{struct Cyc_Tcutil_CVTEnv _tmp857=({struct Cyc_Tcutil_CVTEnv
_tmp8D6;_tmp8D6.kind_env=cvtenv.kind_env;_tmp8D6.free_vars=0;_tmp8D6.free_evars=
0;_tmp8D6.generalize_evars=cvtenv.generalize_evars;_tmp8D6.fn_result=1;_tmp8D6;});
_tmp857=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp857,(void*)1,_tmp792,1);_tmp857.fn_result=
0;{struct Cyc_List_List*a=_tmp793;for(0;a != 0;a=a->tl){struct _tuple2*_tmp858=(
struct _tuple2*)a->hd;void*_tmp859=(*_tmp858).f3;_tmp857=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp857,(void*)1,_tmp859,1);((*_tmp858).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp858).f2).print_const,_tmp859);}}if(_tmp795 != 0){if(_tmp794)({void*
_tmp85A[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp85B="both c_vararg and cyc_vararg";_tag_dynforward(_tmp85B,sizeof(
char),_get_zero_arr_size(_tmp85B,29));}),_tag_dynforward(_tmp85A,sizeof(void*),0));});{
void*_tmp85D;int _tmp85E;struct Cyc_Absyn_VarargInfo _tmp85C=*_tmp795;_tmp85D=(void*)
_tmp85C.type;_tmp85E=_tmp85C.inject;_tmp857=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp857,(void*)1,_tmp85D,1);(_tmp795->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp795->tq).print_const,_tmp85D);if(_tmp85E){void*_tmp85F=Cyc_Tcutil_compress(
_tmp85D);struct Cyc_Absyn_TunionInfo _tmp860;union Cyc_Absyn_TunionInfoU_union
_tmp861;struct Cyc_Absyn_Tuniondecl**_tmp862;_LL55B: if(_tmp85F <= (void*)4)goto
_LL55D;if(*((int*)_tmp85F)!= 2)goto _LL55D;_tmp860=((struct Cyc_Absyn_TunionType_struct*)
_tmp85F)->f1;_tmp861=_tmp860.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp85F)->f1).tunion_info).KnownTunion).tag != 1)goto _LL55D;_tmp862=(_tmp861.KnownTunion).f1;
_LL55C: if((*_tmp862)->is_flat)({void*_tmp863[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp864="cant inject into a flat tunion";_tag_dynforward(_tmp864,sizeof(
char),_get_zero_arr_size(_tmp864,31));}),_tag_dynforward(_tmp863,sizeof(void*),0));});
goto _LL55A;_LL55D:;_LL55E:({void*_tmp865[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp866="can't inject a non-[x]tunion type";_tag_dynforward(_tmp866,sizeof(char),
_get_zero_arr_size(_tmp866,34));}),_tag_dynforward(_tmp865,sizeof(void*),0));});
goto _LL55A;_LL55A:;}}}if(seen_format){int _tmp867=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp793);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp867) || 
fmt_arg_start < 0) || _tmp795 == 0  && fmt_arg_start != 0) || _tmp795 != 0  && 
fmt_arg_start != _tmp867 + 1)({void*_tmp868[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp869="bad format descriptor";_tag_dynforward(_tmp869,sizeof(char),
_get_zero_arr_size(_tmp869,22));}),_tag_dynforward(_tmp868,sizeof(void*),0));});
else{void*_tmp86B;struct _tuple2 _tmp86A=*((struct _tuple2*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp793,fmt_desc_arg - 1);_tmp86B=_tmp86A.f3;{void*_tmp86C=
Cyc_Tcutil_compress(_tmp86B);struct Cyc_Absyn_PtrInfo _tmp86D;void*_tmp86E;struct
Cyc_Absyn_PtrAtts _tmp86F;struct Cyc_Absyn_Conref*_tmp870;struct Cyc_Absyn_Conref*
_tmp871;_LL560: if(_tmp86C <= (void*)4)goto _LL562;if(*((int*)_tmp86C)!= 4)goto
_LL562;_tmp86D=((struct Cyc_Absyn_PointerType_struct*)_tmp86C)->f1;_tmp86E=(void*)
_tmp86D.elt_typ;_tmp86F=_tmp86D.ptr_atts;_tmp870=_tmp86F.bounds;_tmp871=_tmp86F.zero_term;
_LL561:{struct _tuple6 _tmp873=({struct _tuple6 _tmp872;_tmp872.f1=Cyc_Tcutil_compress(
_tmp86E);_tmp872.f2=Cyc_Absyn_conref_def((void*)((void*)0),_tmp870);_tmp872;});
void*_tmp874;void*_tmp875;void*_tmp876;void*_tmp877;_LL565: _tmp874=_tmp873.f1;
if(_tmp874 <= (void*)4)goto _LL567;if(*((int*)_tmp874)!= 5)goto _LL567;_tmp875=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp874)->f1;if((int)_tmp875 != 2)goto
_LL567;_tmp876=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp874)->f2;if((int)
_tmp876 != 0)goto _LL567;_tmp877=_tmp873.f2;if((int)_tmp877 != 0)goto _LL567;_LL566:
goto _LL564;_LL567:;_LL568:({void*_tmp878[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp879="format descriptor is not a char ? type";_tag_dynforward(_tmp879,sizeof(
char),_get_zero_arr_size(_tmp879,39));}),_tag_dynforward(_tmp878,sizeof(void*),0));});
goto _LL564;_LL564:;}goto _LL55F;_LL562:;_LL563:({void*_tmp87A[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp87B="format descriptor is not a char ? type";_tag_dynforward(
_tmp87B,sizeof(char),_get_zero_arr_size(_tmp87B,39));}),_tag_dynforward(_tmp87A,
sizeof(void*),0));});goto _LL55F;_LL55F:;}if(fmt_arg_start != 0){int problem;{
struct _tuple6 _tmp87D=({struct _tuple6 _tmp87C;_tmp87C.f1=ft;_tmp87C.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp795))->type);_tmp87C;});void*
_tmp87E;void*_tmp87F;struct Cyc_Absyn_TunionInfo _tmp880;union Cyc_Absyn_TunionInfoU_union
_tmp881;struct Cyc_Absyn_Tuniondecl**_tmp882;struct Cyc_Absyn_Tuniondecl*_tmp883;
void*_tmp884;void*_tmp885;struct Cyc_Absyn_TunionInfo _tmp886;union Cyc_Absyn_TunionInfoU_union
_tmp887;struct Cyc_Absyn_Tuniondecl**_tmp888;struct Cyc_Absyn_Tuniondecl*_tmp889;
_LL56A: _tmp87E=_tmp87D.f1;if((int)_tmp87E != 0)goto _LL56C;_tmp87F=_tmp87D.f2;if(
_tmp87F <= (void*)4)goto _LL56C;if(*((int*)_tmp87F)!= 2)goto _LL56C;_tmp880=((
struct Cyc_Absyn_TunionType_struct*)_tmp87F)->f1;_tmp881=_tmp880.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp87F)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL56C;_tmp882=(_tmp881.KnownTunion).f1;_tmp883=*_tmp882;_LL56B: problem=
Cyc_Absyn_qvar_cmp(_tmp883->name,Cyc_Absyn_tunion_print_arg_qvar)!= 0;goto _LL569;
_LL56C: _tmp884=_tmp87D.f1;if((int)_tmp884 != 1)goto _LL56E;_tmp885=_tmp87D.f2;if(
_tmp885 <= (void*)4)goto _LL56E;if(*((int*)_tmp885)!= 2)goto _LL56E;_tmp886=((
struct Cyc_Absyn_TunionType_struct*)_tmp885)->f1;_tmp887=_tmp886.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp885)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL56E;_tmp888=(_tmp887.KnownTunion).f1;_tmp889=*_tmp888;_LL56D: problem=
Cyc_Absyn_qvar_cmp(_tmp889->name,Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL569;
_LL56E:;_LL56F: problem=1;goto _LL569;_LL569:;}if(problem)({void*_tmp88A[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp88B="format attribute and vararg types don't match";
_tag_dynforward(_tmp88B,sizeof(char),_get_zero_arr_size(_tmp88B,46));}),
_tag_dynforward(_tmp88A,sizeof(void*),0));});}}}{struct Cyc_List_List*rpo=_tmp796;
for(0;rpo != 0;rpo=rpo->tl){struct _tuple6 _tmp88D;void*_tmp88E;void*_tmp88F;struct
_tuple6*_tmp88C=(struct _tuple6*)rpo->hd;_tmp88D=*_tmp88C;_tmp88E=_tmp88D.f1;
_tmp88F=_tmp88D.f2;_tmp857=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp857,(void*)4,
_tmp88E,0);_tmp857=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp857,(void*)3,_tmp88F,
0);}}if(*_tmp791 != 0)_tmp857=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp857,(void*)
4,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp791))->v,0);else{struct Cyc_List_List*
effect=0;{struct Cyc_List_List*tvs=_tmp857.free_vars;for(0;tvs != 0;tvs=tvs->tl){
void*_tmp890=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);
struct Cyc_Core_Opt*_tmp891;struct Cyc_Core_Opt**_tmp892;void*_tmp893;void*_tmp894;
void*_tmp895;void*_tmp896;struct Cyc_Core_Opt*_tmp897;struct Cyc_Core_Opt**_tmp898;
void*_tmp899;void*_tmp89A;struct Cyc_Core_Opt*_tmp89B;struct Cyc_Core_Opt**_tmp89C;
_LL571: if(*((int*)_tmp890)!= 2)goto _LL573;_tmp891=((struct Cyc_Absyn_Less_kb_struct*)
_tmp890)->f1;_tmp892=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp890)->f1;_tmp893=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp890)->f2;if((
int)_tmp893 != 3)goto _LL573;_LL572:*_tmp892=({struct Cyc_Core_Opt*_tmp89D=
_cycalloc(sizeof(*_tmp89D));_tmp89D->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp89E=_cycalloc(sizeof(*_tmp89E));_tmp89E[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp89F;_tmp89F.tag=0;_tmp89F.f1=(void*)((void*)3);_tmp89F;});_tmp89E;}));
_tmp89D;});goto _LL574;_LL573: if(*((int*)_tmp890)!= 0)goto _LL575;_tmp894=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp890)->f1;if((int)_tmp894 != 3)goto _LL575;_LL574:
effect=({struct Cyc_List_List*_tmp8A0=_cycalloc(sizeof(*_tmp8A0));_tmp8A0->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp8A1=_cycalloc(sizeof(*
_tmp8A1));_tmp8A1[0]=({struct Cyc_Absyn_AccessEff_struct _tmp8A2;_tmp8A2.tag=19;
_tmp8A2.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp8A3=_cycalloc(
sizeof(*_tmp8A3));_tmp8A3[0]=({struct Cyc_Absyn_VarType_struct _tmp8A4;_tmp8A4.tag=
1;_tmp8A4.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp8A4;});_tmp8A3;}));_tmp8A2;});
_tmp8A1;}));_tmp8A0->tl=effect;_tmp8A0;});goto _LL570;_LL575: if(*((int*)_tmp890)
!= 2)goto _LL577;_tmp895=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp890)->f2;
if((int)_tmp895 != 5)goto _LL577;_LL576: goto _LL578;_LL577: if(*((int*)_tmp890)!= 0)
goto _LL579;_tmp896=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp890)->f1;if((int)
_tmp896 != 5)goto _LL579;_LL578: goto _LL570;_LL579: if(*((int*)_tmp890)!= 2)goto
_LL57B;_tmp897=((struct Cyc_Absyn_Less_kb_struct*)_tmp890)->f1;_tmp898=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp890)->f1;_tmp899=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp890)->f2;if((int)_tmp899 != 4)goto _LL57B;_LL57A:*_tmp898=({struct Cyc_Core_Opt*
_tmp8A5=_cycalloc(sizeof(*_tmp8A5));_tmp8A5->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp8A6=_cycalloc(sizeof(*_tmp8A6));_tmp8A6[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp8A7;_tmp8A7.tag=0;_tmp8A7.f1=(void*)((void*)4);_tmp8A7;});_tmp8A6;}));
_tmp8A5;});goto _LL57C;_LL57B: if(*((int*)_tmp890)!= 0)goto _LL57D;_tmp89A=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp890)->f1;if((int)_tmp89A != 4)goto _LL57D;_LL57C:
effect=({struct Cyc_List_List*_tmp8A8=_cycalloc(sizeof(*_tmp8A8));_tmp8A8->hd=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp8A9=_cycalloc(sizeof(*_tmp8A9));
_tmp8A9[0]=({struct Cyc_Absyn_VarType_struct _tmp8AA;_tmp8AA.tag=1;_tmp8AA.f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp8AA;});_tmp8A9;}));_tmp8A8->tl=effect;_tmp8A8;});
goto _LL570;_LL57D: if(*((int*)_tmp890)!= 1)goto _LL57F;_tmp89B=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp890)->f1;_tmp89C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp890)->f1;_LL57E:*_tmp89C=({struct Cyc_Core_Opt*_tmp8AB=_cycalloc(sizeof(*
_tmp8AB));_tmp8AB->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp8AC=
_cycalloc(sizeof(*_tmp8AC));_tmp8AC[0]=({struct Cyc_Absyn_Less_kb_struct _tmp8AD;
_tmp8AD.tag=2;_tmp8AD.f1=0;_tmp8AD.f2=(void*)((void*)0);_tmp8AD;});_tmp8AC;}));
_tmp8AB;});goto _LL580;_LL57F:;_LL580: effect=({struct Cyc_List_List*_tmp8AE=
_cycalloc(sizeof(*_tmp8AE));_tmp8AE->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp8AF=_cycalloc(sizeof(*_tmp8AF));_tmp8AF[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp8B0;_tmp8B0.tag=21;_tmp8B0.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp8B1=_cycalloc(sizeof(*_tmp8B1));_tmp8B1[0]=({struct Cyc_Absyn_VarType_struct
_tmp8B2;_tmp8B2.tag=1;_tmp8B2.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp8B2;});
_tmp8B1;}));_tmp8B0;});_tmp8AF;}));_tmp8AE->tl=effect;_tmp8AE;});goto _LL570;
_LL570:;}}{struct Cyc_List_List*ts=_tmp857.free_evars;for(0;ts != 0;ts=ts->tl){
void*_tmp8B3=Cyc_Tcutil_typ_kind((void*)ts->hd);_LL582: if((int)_tmp8B3 != 3)goto
_LL584;_LL583: effect=({struct Cyc_List_List*_tmp8B4=_cycalloc(sizeof(*_tmp8B4));
_tmp8B4->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp8B5=_cycalloc(
sizeof(*_tmp8B5));_tmp8B5[0]=({struct Cyc_Absyn_AccessEff_struct _tmp8B6;_tmp8B6.tag=
19;_tmp8B6.f1=(void*)((void*)ts->hd);_tmp8B6;});_tmp8B5;}));_tmp8B4->tl=effect;
_tmp8B4;});goto _LL581;_LL584: if((int)_tmp8B3 != 4)goto _LL586;_LL585: effect=({
struct Cyc_List_List*_tmp8B7=_cycalloc(sizeof(*_tmp8B7));_tmp8B7->hd=(void*)((
void*)ts->hd);_tmp8B7->tl=effect;_tmp8B7;});goto _LL581;_LL586: if((int)_tmp8B3 != 
5)goto _LL588;_LL587: goto _LL581;_LL588:;_LL589: effect=({struct Cyc_List_List*
_tmp8B8=_cycalloc(sizeof(*_tmp8B8));_tmp8B8->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp8B9=_cycalloc(sizeof(*_tmp8B9));_tmp8B9[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp8BA;_tmp8BA.tag=21;_tmp8BA.f1=(void*)((void*)ts->hd);_tmp8BA;});_tmp8B9;}));
_tmp8B8->tl=effect;_tmp8B8;});goto _LL581;_LL581:;}}*_tmp791=({struct Cyc_Core_Opt*
_tmp8BB=_cycalloc(sizeof(*_tmp8BB));_tmp8BB->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp8BC=_cycalloc(sizeof(*_tmp8BC));_tmp8BC[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp8BD;_tmp8BD.tag=20;_tmp8BD.f1=effect;_tmp8BD;});_tmp8BC;}));_tmp8BB;});}if(*
_tmp78F != 0){struct Cyc_List_List*bs=*_tmp78F;for(0;bs != 0;bs=bs->tl){void*
_tmp8BE=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp8BF;struct Cyc_Core_Opt**_tmp8C0;struct Cyc_Core_Opt*
_tmp8C1;struct Cyc_Core_Opt**_tmp8C2;void*_tmp8C3;struct Cyc_Core_Opt*_tmp8C4;
struct Cyc_Core_Opt**_tmp8C5;void*_tmp8C6;struct Cyc_Core_Opt*_tmp8C7;struct Cyc_Core_Opt**
_tmp8C8;void*_tmp8C9;void*_tmp8CA;_LL58B: if(*((int*)_tmp8BE)!= 1)goto _LL58D;
_tmp8BF=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp8BE)->f1;_tmp8C0=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8BE)->f1;_LL58C:({struct Cyc_String_pa_struct
_tmp8CD;_tmp8CD.tag=0;_tmp8CD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)bs->hd)->name);{void*_tmp8CB[1]={& _tmp8CD};Cyc_Tcutil_warn(
loc,({const char*_tmp8CC="Type variable %s unconstrained, assuming boxed";
_tag_dynforward(_tmp8CC,sizeof(char),_get_zero_arr_size(_tmp8CC,47));}),
_tag_dynforward(_tmp8CB,sizeof(void*),1));}});_tmp8C2=_tmp8C0;goto _LL58E;_LL58D:
if(*((int*)_tmp8BE)!= 2)goto _LL58F;_tmp8C1=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8BE)->f1;_tmp8C2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8BE)->f1;_tmp8C3=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8BE)->f2;if((
int)_tmp8C3 != 1)goto _LL58F;_LL58E: _tmp8C5=_tmp8C2;goto _LL590;_LL58F: if(*((int*)
_tmp8BE)!= 2)goto _LL591;_tmp8C4=((struct Cyc_Absyn_Less_kb_struct*)_tmp8BE)->f1;
_tmp8C5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8BE)->f1;
_tmp8C6=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8BE)->f2;if((int)_tmp8C6 != 
0)goto _LL591;_LL590:*_tmp8C5=({struct Cyc_Core_Opt*_tmp8CE=_cycalloc(sizeof(*
_tmp8CE));_tmp8CE->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8CF=
_cycalloc(sizeof(*_tmp8CF));_tmp8CF[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp8D0;
_tmp8D0.tag=0;_tmp8D0.f1=(void*)((void*)2);_tmp8D0;});_tmp8CF;}));_tmp8CE;});
goto _LL58A;_LL591: if(*((int*)_tmp8BE)!= 2)goto _LL593;_tmp8C7=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8BE)->f1;_tmp8C8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8BE)->f1;_tmp8C9=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8BE)->f2;
_LL592:*_tmp8C8=({struct Cyc_Core_Opt*_tmp8D1=_cycalloc(sizeof(*_tmp8D1));_tmp8D1->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8D2=_cycalloc(sizeof(*_tmp8D2));
_tmp8D2[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp8D3;_tmp8D3.tag=0;_tmp8D3.f1=(void*)
_tmp8C9;_tmp8D3;});_tmp8D2;}));_tmp8D1;});goto _LL58A;_LL593: if(*((int*)_tmp8BE)
!= 0)goto _LL595;_tmp8CA=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8BE)->f1;if((
int)_tmp8CA != 1)goto _LL595;_LL594:({void*_tmp8D4[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp8D5="functions can't abstract M types";_tag_dynforward(_tmp8D5,
sizeof(char),_get_zero_arr_size(_tmp8D5,33));}),_tag_dynforward(_tmp8D4,sizeof(
void*),0));});goto _LL58A;_LL595:;_LL596: goto _LL58A;_LL58A:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
_tmp857.kind_env,*_tmp78F);_tmp857.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmp857.free_vars,*_tmp78F);{struct Cyc_List_List*tvs=_tmp857.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,(
struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=_tmp857.free_evars;
for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,(
void*)evs->hd);}}goto _LL4CD;}}_LL4EC: if(*((int*)_tmp765)!= 9)goto _LL4EE;_tmp798=((
struct Cyc_Absyn_TupleType_struct*)_tmp765)->f1;_LL4ED: for(0;_tmp798 != 0;_tmp798=
_tmp798->tl){struct _tuple4*_tmp8D7=(struct _tuple4*)_tmp798->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,(*_tmp8D7).f2,0);((*_tmp8D7).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp8D7).f1).print_const,(*_tmp8D7).f2);}goto _LL4CD;_LL4EE: if(*((int*)
_tmp765)!= 11)goto _LL4F0;_tmp799=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp765)->f1;_tmp79A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp765)->f2;_LL4EF:{
struct _RegionHandle _tmp8D8=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp8D8;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp79A != 0;_tmp79A=_tmp79A->tl){struct Cyc_Absyn_Aggrfield _tmp8DA;struct
_dynforward_ptr*_tmp8DB;struct Cyc_Absyn_Tqual _tmp8DC;struct Cyc_Absyn_Tqual*
_tmp8DD;void*_tmp8DE;struct Cyc_Absyn_Exp*_tmp8DF;struct Cyc_List_List*_tmp8E0;
struct Cyc_Absyn_Aggrfield*_tmp8D9=(struct Cyc_Absyn_Aggrfield*)_tmp79A->hd;
_tmp8DA=*_tmp8D9;_tmp8DB=_tmp8DA.name;_tmp8DC=_tmp8DA.tq;_tmp8DD=(struct Cyc_Absyn_Tqual*)&(*
_tmp8D9).tq;_tmp8DE=(void*)_tmp8DA.type;_tmp8DF=_tmp8DA.width;_tmp8E0=_tmp8DA.attributes;
if(((int(*)(int(*compare)(struct _dynforward_ptr*,struct _dynforward_ptr*),struct
Cyc_List_List*l,struct _dynforward_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,
_tmp8DB))({struct Cyc_String_pa_struct _tmp8E3;_tmp8E3.tag=0;_tmp8E3.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp8DB);{void*_tmp8E1[1]={& _tmp8E3};
Cyc_Tcutil_terr(loc,({const char*_tmp8E2="duplicate field %s";_tag_dynforward(
_tmp8E2,sizeof(char),_get_zero_arr_size(_tmp8E2,19));}),_tag_dynforward(_tmp8E1,
sizeof(void*),1));}});if(Cyc_strcmp((struct _dynforward_ptr)*_tmp8DB,({const char*
_tmp8E4="";_tag_dynforward(_tmp8E4,sizeof(char),_get_zero_arr_size(_tmp8E4,1));}))
!= 0)prev_fields=({struct Cyc_List_List*_tmp8E5=_region_malloc(aprev_rgn,sizeof(*
_tmp8E5));_tmp8E5->hd=_tmp8DB;_tmp8E5->tl=prev_fields;_tmp8E5;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,_tmp8DE,0);_tmp8DD->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp8DD->print_const,_tmp8DE);if(_tmp799 == (void*)1  && !Cyc_Tcutil_bits_only(
_tmp8DE))({struct Cyc_String_pa_struct _tmp8E8;_tmp8E8.tag=0;_tmp8E8.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp8DB);{void*_tmp8E6[1]={& _tmp8E8};
Cyc_Tcutil_warn(loc,({const char*_tmp8E7="union member %s is not `bits-only' so it can only be written and not read";
_tag_dynforward(_tmp8E7,sizeof(char),_get_zero_arr_size(_tmp8E7,74));}),
_tag_dynforward(_tmp8E6,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(loc,te,
_tmp8DE,_tmp8DF,_tmp8DB);Cyc_Tcutil_check_field_atts(loc,_tmp8DB,_tmp8E0);}};
_pop_region(aprev_rgn);}goto _LL4CD;_LL4F0: if(*((int*)_tmp765)!= 10)goto _LL4F2;
_tmp79B=((struct Cyc_Absyn_AggrType_struct*)_tmp765)->f1;_tmp79C=_tmp79B.aggr_info;
_tmp79D=(union Cyc_Absyn_AggrInfoU_union*)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp765)->f1).aggr_info;_tmp79E=_tmp79B.targs;_tmp79F=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_AggrType_struct*)_tmp765)->f1).targs;_LL4F1:{union Cyc_Absyn_AggrInfoU_union
_tmp8E9=*_tmp79D;void*_tmp8EA;struct _tuple1*_tmp8EB;struct Cyc_Absyn_Aggrdecl**
_tmp8EC;struct Cyc_Absyn_Aggrdecl*_tmp8ED;_LL598: if((_tmp8E9.UnknownAggr).tag != 0)
goto _LL59A;_tmp8EA=(_tmp8E9.UnknownAggr).f1;_tmp8EB=(_tmp8E9.UnknownAggr).f2;
_LL599: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp8EE;_push_handler(&
_tmp8EE);{int _tmp8F0=0;if(setjmp(_tmp8EE.handler))_tmp8F0=1;if(!_tmp8F0){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp8EB);*_tmp79D=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmp8F1;(_tmp8F1.KnownAggr).tag=1;(_tmp8F1.KnownAggr).f1=adp;_tmp8F1;});;
_pop_handler();}else{void*_tmp8EF=(void*)_exn_thrown;void*_tmp8F3=_tmp8EF;_LL59D:
if(_tmp8F3 != Cyc_Dict_Absent)goto _LL59F;_LL59E: {struct Cyc_Tcenv_Genv*_tmp8F4=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Aggrdecl*_tmp8F5=({struct Cyc_Absyn_Aggrdecl*
_tmp8FA=_cycalloc(sizeof(*_tmp8FA));_tmp8FA->kind=(void*)_tmp8EA;_tmp8FA->sc=(
void*)((void*)3);_tmp8FA->name=_tmp8EB;_tmp8FA->tvs=0;_tmp8FA->impl=0;_tmp8FA->attributes=
0;_tmp8FA;});Cyc_Tc_tcAggrdecl(te,_tmp8F4,loc,_tmp8F5);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp8EB);*_tmp79D=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmp8F6;(_tmp8F6.KnownAggr).tag=1;(_tmp8F6.KnownAggr).f1=adp;_tmp8F6;});if(*
_tmp79F != 0){({struct Cyc_String_pa_struct _tmp8F9;_tmp8F9.tag=0;_tmp8F9.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp8EB));{
void*_tmp8F7[1]={& _tmp8F9};Cyc_Tcutil_terr(loc,({const char*_tmp8F8="declare parameterized type %s before using";
_tag_dynforward(_tmp8F8,sizeof(char),_get_zero_arr_size(_tmp8F8,43));}),
_tag_dynforward(_tmp8F7,sizeof(void*),1));}});return cvtenv;}goto _LL59C;}_LL59F:;
_LL5A0:(void)_throw(_tmp8F3);_LL59C:;}}}_tmp8ED=*adp;goto _LL59B;}_LL59A: if((
_tmp8E9.KnownAggr).tag != 1)goto _LL597;_tmp8EC=(_tmp8E9.KnownAggr).f1;_tmp8ED=*
_tmp8EC;_LL59B: {struct Cyc_List_List*tvs=_tmp8ED->tvs;struct Cyc_List_List*ts=*
_tmp79F;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd,0);}if(ts != 0)({struct Cyc_String_pa_struct _tmp8FD;_tmp8FD.tag=
0;_tmp8FD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp8ED->name));{void*_tmp8FB[1]={& _tmp8FD};Cyc_Tcutil_terr(loc,({const char*
_tmp8FC="too many parameters for type %s";_tag_dynforward(_tmp8FC,sizeof(char),
_get_zero_arr_size(_tmp8FC,32));}),_tag_dynforward(_tmp8FB,sizeof(void*),1));}});
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);hidden_ts=({struct Cyc_List_List*_tmp8FE=_cycalloc(sizeof(*_tmp8FE));_tmp8FE->hd=(
void*)e;_tmp8FE->tl=hidden_ts;_tmp8FE;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,0);}*_tmp79F=Cyc_List_imp_append(*_tmp79F,Cyc_List_imp_rev(
hidden_ts));}}_LL597:;}goto _LL4CD;_LL4F2: if(*((int*)_tmp765)!= 16)goto _LL4F4;
_tmp7A0=((struct Cyc_Absyn_TypedefType_struct*)_tmp765)->f1;_tmp7A1=((struct Cyc_Absyn_TypedefType_struct*)
_tmp765)->f2;_tmp7A2=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp765)->f2;_tmp7A3=((struct Cyc_Absyn_TypedefType_struct*)_tmp765)->f3;_tmp7A4=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp765)->f3;
_tmp7A5=((struct Cyc_Absyn_TypedefType_struct*)_tmp765)->f4;_tmp7A6=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp765)->f4;_LL4F3: {struct Cyc_List_List*
targs=*_tmp7A2;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp8FF;
_push_handler(& _tmp8FF);{int _tmp901=0;if(setjmp(_tmp8FF.handler))_tmp901=1;if(!
_tmp901){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp7A0);;_pop_handler();}else{
void*_tmp900=(void*)_exn_thrown;void*_tmp903=_tmp900;_LL5A2: if(_tmp903 != Cyc_Dict_Absent)
goto _LL5A4;_LL5A3:({struct Cyc_String_pa_struct _tmp906;_tmp906.tag=0;_tmp906.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7A0));{
void*_tmp904[1]={& _tmp906};Cyc_Tcutil_terr(loc,({const char*_tmp905="unbound typedef name %s";
_tag_dynforward(_tmp905,sizeof(char),_get_zero_arr_size(_tmp905,24));}),
_tag_dynforward(_tmp904,sizeof(void*),1));}});return cvtenv;_LL5A4:;_LL5A5:(void)
_throw(_tmp903);_LL5A1:;}}}*_tmp7A4=(struct Cyc_Absyn_Typedefdecl*)td;_tmp7A0[0]=(
td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(
ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,0);inst=({
struct Cyc_List_List*_tmp907=_cycalloc(sizeof(*_tmp907));_tmp907->hd=({struct
_tuple8*_tmp908=_cycalloc(sizeof(*_tmp908));_tmp908->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp908->f2=(void*)ts->hd;_tmp908;});_tmp907->tl=inst;_tmp907;});}if(ts
!= 0)({struct Cyc_String_pa_struct _tmp90B;_tmp90B.tag=0;_tmp90B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7A0));{void*
_tmp909[1]={& _tmp90B};Cyc_Tcutil_terr(loc,({const char*_tmp90A="too many parameters for typedef %s";
_tag_dynforward(_tmp90A,sizeof(char),_get_zero_arr_size(_tmp90A,35));}),
_tag_dynforward(_tmp909,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp90C=
_cycalloc(sizeof(*_tmp90C));_tmp90C->hd=(void*)e;_tmp90C->tl=hidden_ts;_tmp90C;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,0);inst=({struct Cyc_List_List*
_tmp90D=_cycalloc(sizeof(*_tmp90D));_tmp90D->hd=({struct _tuple8*_tmp90E=
_cycalloc(sizeof(*_tmp90E));_tmp90E->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp90E->f2=
e;_tmp90E;});_tmp90D->tl=inst;_tmp90D;});}*_tmp7A2=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_substitute(inst,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp7A6=({void**_tmp90F=
_cycalloc(sizeof(*_tmp90F));_tmp90F[0]=new_typ;_tmp90F;});}goto _LL4CD;}}_LL4F4:
if((int)_tmp765 != 3)goto _LL4F6;_LL4F5: if(!allow_unique)({void*_tmp910[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp911="`U only allowed in outermost pointers";_tag_dynforward(
_tmp911,sizeof(char),_get_zero_arr_size(_tmp911,38));}),_tag_dynforward(_tmp910,
sizeof(void*),0));});goto _LL4CD;_LL4F6: if(_tmp765 <= (void*)4)goto _LL4F8;if(*((
int*)_tmp765)!= 18)goto _LL4F8;_LL4F7: goto _LL4F9;_LL4F8: if((int)_tmp765 != 2)goto
_LL4FA;_LL4F9: goto _LL4CD;_LL4FA: if(_tmp765 <= (void*)4)goto _LL4FC;if(*((int*)
_tmp765)!= 15)goto _LL4FC;_tmp7A7=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp765)->f1;_LL4FB: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp7A7,1);goto _LL4CD;_LL4FC: if(_tmp765 <= (void*)4)goto _LL4FE;if(*((int*)_tmp765)
!= 19)goto _LL4FE;_tmp7A8=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp765)->f1;
_LL4FD: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp7A8,0);
goto _LL4CD;_LL4FE: if(_tmp765 <= (void*)4)goto _LL500;if(*((int*)_tmp765)!= 21)goto
_LL500;_tmp7A9=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp765)->f1;_LL4FF:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp7A9,0);goto
_LL4CD;_LL500: if(_tmp765 <= (void*)4)goto _LL4CD;if(*((int*)_tmp765)!= 20)goto
_LL4CD;_tmp7AA=((struct Cyc_Absyn_JoinEff_struct*)_tmp765)->f1;_LL501: for(0;
_tmp7AA != 0;_tmp7AA=_tmp7AA->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)4,(void*)_tmp7AA->hd,0);}goto _LL4CD;_LL4CD:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind))({struct Cyc_String_pa_struct _tmp916;
_tmp916.tag=0;_tmp916.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(
expected_kind));{struct Cyc_String_pa_struct _tmp915;_tmp915.tag=0;_tmp915.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(
t)));{struct Cyc_String_pa_struct _tmp914;_tmp914.tag=0;_tmp914.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp912[
3]={& _tmp914,& _tmp915,& _tmp916};Cyc_Tcutil_terr(loc,({const char*_tmp913="type %s has kind %s but as used here needs kind %s";
_tag_dynforward(_tmp913,sizeof(char),_get_zero_arr_size(_tmp913,51));}),
_tag_dynforward(_tmp912,sizeof(void*),3));}}}});return cvtenv;}static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*kind_env,void*expected_kind,int generalize_evars,void*t,
int allow_unique){struct Cyc_Tcutil_CVTEnv _tmp917=Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp920;_tmp920.kind_env=kind_env;_tmp920.free_vars=
0;_tmp920.free_evars=0;_tmp920.generalize_evars=generalize_evars;_tmp920.fn_result=
0;_tmp920;}),expected_kind,t,allow_unique);{struct Cyc_List_List*vs=_tmp917.free_vars;
for(0;vs != 0;vs=vs->tl){_tmp917.kind_env=Cyc_Tcutil_fast_add_free_tvar(kind_env,(
struct Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp917.free_evars;for(
0;evs != 0;evs=evs->tl){void*_tmp918=Cyc_Tcutil_compress((void*)evs->hd);struct
Cyc_Core_Opt*_tmp919;struct Cyc_Core_Opt**_tmp91A;_LL5A7: if(_tmp918 <= (void*)4)
goto _LL5A9;if(*((int*)_tmp918)!= 0)goto _LL5A9;_tmp919=((struct Cyc_Absyn_Evar_struct*)
_tmp918)->f4;_tmp91A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp918)->f4;_LL5A8: if(*_tmp91A == 0)*_tmp91A=({struct Cyc_Core_Opt*_tmp91B=
_cycalloc(sizeof(*_tmp91B));_tmp91B->v=kind_env;_tmp91B;});else{struct Cyc_List_List*
_tmp91C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp91A))->v;
struct Cyc_List_List*_tmp91D=0;for(0;_tmp91C != 0;_tmp91C=_tmp91C->tl){if(((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct
Cyc_Absyn_Tvar*)_tmp91C->hd))_tmp91D=({struct Cyc_List_List*_tmp91E=_cycalloc(
sizeof(*_tmp91E));_tmp91E->hd=(struct Cyc_Absyn_Tvar*)_tmp91C->hd;_tmp91E->tl=
_tmp91D;_tmp91E;});}*_tmp91A=({struct Cyc_Core_Opt*_tmp91F=_cycalloc(sizeof(*
_tmp91F));_tmp91F->v=_tmp91D;_tmp91F;});}goto _LL5A6;_LL5A9:;_LL5AA: goto _LL5A6;
_LL5A6:;}}return _tmp917;}void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*t,int allow_unique){int generalize_evars=Cyc_Tcutil_is_function_type(
t);struct Cyc_List_List*_tmp921=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Tcutil_CVTEnv
_tmp922=Cyc_Tcutil_check_valid_type(loc,te,_tmp921,(void*)1,generalize_evars,t,
allow_unique);struct Cyc_List_List*_tmp923=_tmp922.free_vars;struct Cyc_List_List*
_tmp924=_tmp922.free_evars;if(_tmp921 != 0){struct Cyc_List_List*_tmp925=0;{struct
Cyc_List_List*_tmp926=_tmp923;for(0;(unsigned int)_tmp926;_tmp926=_tmp926->tl){
struct Cyc_Absyn_Tvar*_tmp927=(struct Cyc_Absyn_Tvar*)_tmp926->hd;int found=0;{
struct Cyc_List_List*_tmp928=_tmp921;for(0;(unsigned int)_tmp928;_tmp928=_tmp928->tl){
if(Cyc_Absyn_tvar_cmp(_tmp927,(struct Cyc_Absyn_Tvar*)_tmp928->hd)== 0){found=1;
break;}}}if(!found)_tmp925=({struct Cyc_List_List*_tmp929=_cycalloc(sizeof(*
_tmp929));_tmp929->hd=(struct Cyc_Absyn_Tvar*)_tmp926->hd;_tmp929->tl=_tmp925;
_tmp929;});}}_tmp923=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp925);}{struct Cyc_List_List*x=_tmp923;for(0;x != 0;x=x->tl){void*_tmp92A=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmp92B;struct Cyc_Core_Opt**
_tmp92C;struct Cyc_Core_Opt*_tmp92D;struct Cyc_Core_Opt**_tmp92E;void*_tmp92F;
struct Cyc_Core_Opt*_tmp930;struct Cyc_Core_Opt**_tmp931;void*_tmp932;struct Cyc_Core_Opt*
_tmp933;struct Cyc_Core_Opt**_tmp934;void*_tmp935;void*_tmp936;_LL5AC: if(*((int*)
_tmp92A)!= 1)goto _LL5AE;_tmp92B=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp92A)->f1;
_tmp92C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp92A)->f1;
_LL5AD: _tmp92E=_tmp92C;goto _LL5AF;_LL5AE: if(*((int*)_tmp92A)!= 2)goto _LL5B0;
_tmp92D=((struct Cyc_Absyn_Less_kb_struct*)_tmp92A)->f1;_tmp92E=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp92A)->f1;_tmp92F=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp92A)->f2;if((int)_tmp92F != 1)goto _LL5B0;_LL5AF: _tmp931=_tmp92E;goto _LL5B1;
_LL5B0: if(*((int*)_tmp92A)!= 2)goto _LL5B2;_tmp930=((struct Cyc_Absyn_Less_kb_struct*)
_tmp92A)->f1;_tmp931=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp92A)->f1;_tmp932=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp92A)->f2;if((
int)_tmp932 != 0)goto _LL5B2;_LL5B1:*_tmp931=({struct Cyc_Core_Opt*_tmp937=
_cycalloc(sizeof(*_tmp937));_tmp937->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp938=_cycalloc(sizeof(*_tmp938));_tmp938[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp939;_tmp939.tag=0;_tmp939.f1=(void*)((void*)2);_tmp939;});_tmp938;}));
_tmp937;});goto _LL5AB;_LL5B2: if(*((int*)_tmp92A)!= 2)goto _LL5B4;_tmp933=((struct
Cyc_Absyn_Less_kb_struct*)_tmp92A)->f1;_tmp934=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp92A)->f1;_tmp935=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp92A)->f2;_LL5B3:*_tmp934=({struct Cyc_Core_Opt*_tmp93A=_cycalloc(sizeof(*
_tmp93A));_tmp93A->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp93B=
_cycalloc(sizeof(*_tmp93B));_tmp93B[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp93C;
_tmp93C.tag=0;_tmp93C.f1=(void*)_tmp935;_tmp93C;});_tmp93B;}));_tmp93A;});goto
_LL5AB;_LL5B4: if(*((int*)_tmp92A)!= 0)goto _LL5B6;_tmp936=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp92A)->f1;if((int)_tmp936 != 1)goto _LL5B6;_LL5B5:({struct Cyc_String_pa_struct
_tmp93F;_tmp93F.tag=0;_tmp93F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));{void*_tmp93D[1]={& _tmp93F};
Cyc_Tcutil_terr(loc,({const char*_tmp93E="type variable %s cannot have kind M";
_tag_dynforward(_tmp93E,sizeof(char),_get_zero_arr_size(_tmp93E,36));}),
_tag_dynforward(_tmp93D,sizeof(void*),1));}});goto _LL5AB;_LL5B6:;_LL5B7: goto
_LL5AB;_LL5AB:;}}if(_tmp923 != 0  || _tmp924 != 0){{void*_tmp940=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp941;struct Cyc_List_List*_tmp942;struct Cyc_List_List**
_tmp943;struct Cyc_Core_Opt*_tmp944;void*_tmp945;struct Cyc_List_List*_tmp946;int
_tmp947;struct Cyc_Absyn_VarargInfo*_tmp948;struct Cyc_List_List*_tmp949;struct Cyc_List_List*
_tmp94A;_LL5B9: if(_tmp940 <= (void*)4)goto _LL5BB;if(*((int*)_tmp940)!= 8)goto
_LL5BB;_tmp941=((struct Cyc_Absyn_FnType_struct*)_tmp940)->f1;_tmp942=_tmp941.tvars;
_tmp943=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmp940)->f1).tvars;
_tmp944=_tmp941.effect;_tmp945=(void*)_tmp941.ret_typ;_tmp946=_tmp941.args;
_tmp947=_tmp941.c_varargs;_tmp948=_tmp941.cyc_varargs;_tmp949=_tmp941.rgn_po;
_tmp94A=_tmp941.attributes;_LL5BA: if(*_tmp943 == 0){*_tmp943=_tmp923;_tmp923=0;}
goto _LL5B8;_LL5BB:;_LL5BC: goto _LL5B8;_LL5B8:;}if(_tmp923 != 0)({struct Cyc_String_pa_struct
_tmp94D;_tmp94D.tag=0;_tmp94D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)_tmp923->hd)->name);{void*_tmp94B[1]={& _tmp94D};Cyc_Tcutil_terr(
loc,({const char*_tmp94C="unbound type variable %s";_tag_dynforward(_tmp94C,
sizeof(char),_get_zero_arr_size(_tmp94C,25));}),_tag_dynforward(_tmp94B,sizeof(
void*),1));}});if(_tmp924 != 0)for(0;_tmp924 != 0;_tmp924=_tmp924->tl){void*e=(
void*)_tmp924->hd;void*_tmp94E=Cyc_Tcutil_typ_kind(e);_LL5BE: if((int)_tmp94E != 3)
goto _LL5C0;_LL5BF: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp94F[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp950="can't unify evar with heap!";_tag_dynforward(_tmp950,sizeof(char),
_get_zero_arr_size(_tmp950,28));}),_tag_dynforward(_tmp94F,sizeof(void*),0));});
goto _LL5BD;_LL5C0: if((int)_tmp94E != 4)goto _LL5C2;_LL5C1: if(!Cyc_Tcutil_unify(e,
Cyc_Absyn_empty_effect))({void*_tmp951[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp952="can't unify evar with {}!";
_tag_dynforward(_tmp952,sizeof(char),_get_zero_arr_size(_tmp952,26));}),
_tag_dynforward(_tmp951,sizeof(void*),0));});goto _LL5BD;_LL5C2:;_LL5C3:({struct
Cyc_String_pa_struct _tmp956;_tmp956.tag=0;_tmp956.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp955;_tmp955.tag=0;_tmp955.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(e));{void*_tmp953[2]={& _tmp955,& _tmp956};Cyc_Tcutil_terr(
loc,({const char*_tmp954="hidden type variable %s isn't abstracted in type %s";
_tag_dynforward(_tmp954,sizeof(char),_get_zero_arr_size(_tmp954,52));}),
_tag_dynforward(_tmp953,sizeof(void*),2));}}});goto _LL5BD;_LL5BD:;}}}void Cyc_Tcutil_check_no_unique_region(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp957;_tmp957.kind_env=0;_tmp957.free_vars=0;
_tmp957.free_evars=0;_tmp957.generalize_evars=0;_tmp957.fn_result=0;_tmp957;}),(
void*)1,t,0);}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t,1);{void*_tmp958=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp959;struct Cyc_List_List*_tmp95A;struct Cyc_Core_Opt*
_tmp95B;void*_tmp95C;_LL5C5: if(_tmp958 <= (void*)4)goto _LL5C7;if(*((int*)_tmp958)
!= 8)goto _LL5C7;_tmp959=((struct Cyc_Absyn_FnType_struct*)_tmp958)->f1;_tmp95A=
_tmp959.tvars;_tmp95B=_tmp959.effect;_tmp95C=(void*)_tmp959.ret_typ;_LL5C6: fd->tvs=
_tmp95A;fd->effect=_tmp95B;goto _LL5C4;_LL5C7:;_LL5C8:({void*_tmp95D[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp95E="check_fndecl_valid_type: not a FnType";_tag_dynforward(_tmp95E,sizeof(
char),_get_zero_arr_size(_tmp95E,38));}),_tag_dynforward(_tmp95D,sizeof(void*),0));});
_LL5C4:;}{struct _RegionHandle _tmp95F=_new_region("r");struct _RegionHandle*r=&
_tmp95F;_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr*(*f)(struct _tuple14*),struct Cyc_List_List*
x))Cyc_List_rmap)(r,(struct _dynforward_ptr*(*)(struct _tuple14*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,({const char*_tmp960="function declaration has repeated parameter";
_tag_dynforward(_tmp960,sizeof(char),_get_zero_arr_size(_tmp960,44));}));;
_pop_region(r);}fd->cached_typ=({struct Cyc_Core_Opt*_tmp961=_cycalloc(sizeof(*
_tmp961));_tmp961->v=(void*)t;_tmp961;});}void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,void*expected_kind,
int allow_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp962=Cyc_Tcutil_check_valid_type(
loc,te,bound_tvars,expected_kind,0,t,1);struct Cyc_List_List*_tmp963=Cyc_Tcutil_remove_bound_tvars(
_tmp962.free_vars,bound_tvars);struct Cyc_List_List*_tmp964=_tmp962.free_evars;{
struct Cyc_List_List*fs=_tmp963;for(0;fs != 0;fs=fs->tl){struct _dynforward_ptr*
_tmp965=((struct Cyc_Absyn_Tvar*)fs->hd)->name;({struct Cyc_String_pa_struct
_tmp969;_tmp969.tag=0;_tmp969.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct _tmp968;_tmp968.tag=0;
_tmp968.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp965);{void*
_tmp966[2]={& _tmp968,& _tmp969};Cyc_Tcutil_terr(loc,({const char*_tmp967="unbound type variable %s in type %s";
_tag_dynforward(_tmp967,sizeof(char),_get_zero_arr_size(_tmp967,36));}),
_tag_dynforward(_tmp966,sizeof(void*),2));}}});}}if(!allow_evars  && _tmp964 != 0)
for(0;_tmp964 != 0;_tmp964=_tmp964->tl){void*e=(void*)_tmp964->hd;void*_tmp96A=
Cyc_Tcutil_typ_kind(e);_LL5CA: if((int)_tmp96A != 3)goto _LL5CC;_LL5CB: if(!Cyc_Tcutil_unify(
e,(void*)2))({void*_tmp96B[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp96C="can't unify evar with heap!";
_tag_dynforward(_tmp96C,sizeof(char),_get_zero_arr_size(_tmp96C,28));}),
_tag_dynforward(_tmp96B,sizeof(void*),0));});goto _LL5C9;_LL5CC: if((int)_tmp96A != 
4)goto _LL5CE;_LL5CD: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*
_tmp96D[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp96E="can't unify evar with {}!";_tag_dynforward(_tmp96E,sizeof(
char),_get_zero_arr_size(_tmp96E,26));}),_tag_dynforward(_tmp96D,sizeof(void*),0));});
goto _LL5C9;_LL5CE:;_LL5CF:({struct Cyc_String_pa_struct _tmp972;_tmp972.tag=0;
_tmp972.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp971;_tmp971.tag=0;_tmp971.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(e));{void*_tmp96F[
2]={& _tmp971,& _tmp972};Cyc_Tcutil_terr(loc,({const char*_tmp970="hidden type variable %s isn't abstracted in type %s";
_tag_dynforward(_tmp970,sizeof(char),_get_zero_arr_size(_tmp970,52));}),
_tag_dynforward(_tmp96F,sizeof(void*),2));}}});goto _LL5C9;_LL5C9:;}}void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)tv->identity=Cyc_Tcutil_new_tvar_id();}
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,
tvs);}static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct
Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dynforward_ptr(*a2string)(
void*),struct _dynforward_ptr msg){for(0;vs != 0;vs=vs->tl){struct Cyc_List_List*vs2=
vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(void*)vs2->hd)== 0)({
struct Cyc_String_pa_struct _tmp976;_tmp976.tag=0;_tmp976.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)a2string((void*)vs->hd));{struct Cyc_String_pa_struct
_tmp975;_tmp975.tag=0;_tmp975.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
msg);{void*_tmp973[2]={& _tmp975,& _tmp976};Cyc_Tcutil_terr(loc,({const char*
_tmp974="%s: %s";_tag_dynforward(_tmp974,sizeof(char),_get_zero_arr_size(_tmp974,
7));}),_tag_dynforward(_tmp973,sizeof(void*),2));}}});}}}static struct
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
Cyc_Tcutil_tvar2string,({const char*_tmp977="duplicate type variable";
_tag_dynforward(_tmp977,sizeof(char),_get_zero_arr_size(_tmp977,24));}));}struct
_tuple19{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple20{struct Cyc_List_List*
f1;void*f2;};struct _tuple21{struct Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){if(Cyc_strcmp((struct _dynforward_ptr)*((struct Cyc_Absyn_Aggrfield*)
sd_fields->hd)->name,({const char*_tmp978="";_tag_dynforward(_tmp978,sizeof(char),
_get_zero_arr_size(_tmp978,1));}))!= 0)fields=({struct Cyc_List_List*_tmp979=
_cycalloc(sizeof(*_tmp979));_tmp979->hd=({struct _tuple19*_tmp97A=_cycalloc(
sizeof(*_tmp97A));_tmp97A->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd;_tmp97A->f2=
0;_tmp97A;});_tmp979->tl=fields;_tmp979;});}}fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct Cyc_List_List*ans=0;for(
0;des != 0;des=des->tl){struct _tuple20 _tmp97C;struct Cyc_List_List*_tmp97D;void*
_tmp97E;struct _tuple20*_tmp97B=(struct _tuple20*)des->hd;_tmp97C=*_tmp97B;_tmp97D=
_tmp97C.f1;_tmp97E=_tmp97C.f2;if(_tmp97D == 0){struct Cyc_List_List*_tmp97F=fields;
for(0;_tmp97F != 0;_tmp97F=_tmp97F->tl){if(!(*((struct _tuple19*)_tmp97F->hd)).f2){(*((
struct _tuple19*)_tmp97F->hd)).f2=1;(*((struct _tuple20*)des->hd)).f1=(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp980=_cycalloc(sizeof(*_tmp980));_tmp980->hd=(void*)((
void*)({struct Cyc_Absyn_FieldName_struct*_tmp981=_cycalloc(sizeof(*_tmp981));
_tmp981[0]=({struct Cyc_Absyn_FieldName_struct _tmp982;_tmp982.tag=1;_tmp982.f1=((*((
struct _tuple19*)_tmp97F->hd)).f1)->name;_tmp982;});_tmp981;}));_tmp980->tl=0;
_tmp980;});ans=({struct Cyc_List_List*_tmp983=_region_malloc(rgn,sizeof(*_tmp983));
_tmp983->hd=({struct _tuple21*_tmp984=_region_malloc(rgn,sizeof(*_tmp984));
_tmp984->f1=(*((struct _tuple19*)_tmp97F->hd)).f1;_tmp984->f2=_tmp97E;_tmp984;});
_tmp983->tl=ans;_tmp983;});break;}}if(_tmp97F == 0)({void*_tmp985[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp986="too many arguments to struct";_tag_dynforward(_tmp986,
sizeof(char),_get_zero_arr_size(_tmp986,29));}),_tag_dynforward(_tmp985,sizeof(
void*),0));});}else{if(_tmp97D->tl != 0)({void*_tmp987[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp988="multiple designators are not supported";_tag_dynforward(
_tmp988,sizeof(char),_get_zero_arr_size(_tmp988,39));}),_tag_dynforward(_tmp987,
sizeof(void*),0));});else{void*_tmp989=(void*)_tmp97D->hd;struct _dynforward_ptr*
_tmp98A;_LL5D1: if(*((int*)_tmp989)!= 0)goto _LL5D3;_LL5D2:({void*_tmp98B[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp98C="array designator used in argument to struct";
_tag_dynforward(_tmp98C,sizeof(char),_get_zero_arr_size(_tmp98C,44));}),
_tag_dynforward(_tmp98B,sizeof(void*),0));});goto _LL5D0;_LL5D3: if(*((int*)
_tmp989)!= 1)goto _LL5D0;_tmp98A=((struct Cyc_Absyn_FieldName_struct*)_tmp989)->f1;
_LL5D4: {struct Cyc_List_List*_tmp98D=fields;for(0;_tmp98D != 0;_tmp98D=_tmp98D->tl){
if(Cyc_strptrcmp(_tmp98A,((*((struct _tuple19*)_tmp98D->hd)).f1)->name)== 0){if((*((
struct _tuple19*)_tmp98D->hd)).f2)({struct Cyc_String_pa_struct _tmp990;_tmp990.tag=
0;_tmp990.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp98A);{void*
_tmp98E[1]={& _tmp990};Cyc_Tcutil_terr(loc,({const char*_tmp98F="field %s has already been used as an argument";
_tag_dynforward(_tmp98F,sizeof(char),_get_zero_arr_size(_tmp98F,46));}),
_tag_dynforward(_tmp98E,sizeof(void*),1));}});(*((struct _tuple19*)_tmp98D->hd)).f2=
1;ans=({struct Cyc_List_List*_tmp991=_region_malloc(rgn,sizeof(*_tmp991));_tmp991->hd=({
struct _tuple21*_tmp992=_region_malloc(rgn,sizeof(*_tmp992));_tmp992->f1=(*((
struct _tuple19*)_tmp98D->hd)).f1;_tmp992->f2=_tmp97E;_tmp992;});_tmp991->tl=ans;
_tmp991;});break;}}if(_tmp98D == 0)({struct Cyc_String_pa_struct _tmp995;_tmp995.tag=
0;_tmp995.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp98A);{void*
_tmp993[1]={& _tmp995};Cyc_Tcutil_terr(loc,({const char*_tmp994="bad field designator %s";
_tag_dynforward(_tmp994,sizeof(char),_get_zero_arr_size(_tmp994,24));}),
_tag_dynforward(_tmp993,sizeof(void*),1));}});goto _LL5D0;}_LL5D0:;}}}for(0;
fields != 0;fields=fields->tl){if(!(*((struct _tuple19*)fields->hd)).f2){({void*
_tmp996[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp997="too few arguments to struct";
_tag_dynforward(_tmp997,sizeof(char),_get_zero_arr_size(_tmp997,28));}),
_tag_dynforward(_tmp996,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest,int*forward_only){*forward_only=0;{void*_tmp998=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp999;void*_tmp99A;struct Cyc_Absyn_PtrAtts _tmp99B;
struct Cyc_Absyn_Conref*_tmp99C;_LL5D6: if(_tmp998 <= (void*)4)goto _LL5D8;if(*((int*)
_tmp998)!= 4)goto _LL5D8;_tmp999=((struct Cyc_Absyn_PointerType_struct*)_tmp998)->f1;
_tmp99A=(void*)_tmp999.elt_typ;_tmp99B=_tmp999.ptr_atts;_tmp99C=_tmp99B.bounds;
_LL5D7: {struct Cyc_Absyn_Conref*_tmp99D=Cyc_Absyn_compress_conref(_tmp99C);union
Cyc_Absyn_Constraint_union _tmp99E=(Cyc_Absyn_compress_conref(_tmp99D))->v;void*
_tmp99F;void*_tmp9A0;_LL5DB: if((_tmp99E.Eq_constr).tag != 0)goto _LL5DD;_tmp99F=(
_tmp99E.Eq_constr).f1;if((int)_tmp99F != 0)goto _LL5DD;_LL5DC:*forward_only=1;*
elt_typ_dest=_tmp99A;return 1;_LL5DD: if((_tmp99E.Eq_constr).tag != 0)goto _LL5DF;
_tmp9A0=(_tmp99E.Eq_constr).f1;if((int)_tmp9A0 != 1)goto _LL5DF;_LL5DE:*
elt_typ_dest=_tmp99A;return 1;_LL5DF: if((_tmp99E.No_constr).tag != 2)goto _LL5E1;
_LL5E0: _tmp99D->v=(union Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union
_tmp9A1;(_tmp9A1.Eq_constr).tag=0;(_tmp9A1.Eq_constr).f1=(void*)((void*)0);
_tmp9A1;});*forward_only=1;*elt_typ_dest=_tmp99A;return 1;_LL5E1:;_LL5E2: return 0;
_LL5DA:;}_LL5D8:;_LL5D9: return 0;_LL5D5:;}}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmp9A2=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp9A3;void*_tmp9A4;struct Cyc_Absyn_PtrAtts _tmp9A5;struct Cyc_Absyn_Conref*
_tmp9A6;_LL5E4: if(_tmp9A2 <= (void*)4)goto _LL5E6;if(*((int*)_tmp9A2)!= 4)goto
_LL5E6;_tmp9A3=((struct Cyc_Absyn_PointerType_struct*)_tmp9A2)->f1;_tmp9A4=(void*)
_tmp9A3.elt_typ;_tmp9A5=_tmp9A3.ptr_atts;_tmp9A6=_tmp9A5.zero_term;_LL5E5:*
elt_typ_dest=_tmp9A4;return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp9A6);_LL5E6:;_LL5E7: return 0;_LL5E3:;}int Cyc_Tcutil_is_tagged_pointer_typ(
void*t,int*is_forward_only){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& ignore,is_forward_only);}struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){struct _tuple10 bogus_ans=({struct _tuple10 _tmp9EA;
_tmp9EA.f1=0;_tmp9EA.f2=(void*)2;_tmp9EA;});void*_tmp9A7=(void*)e->r;struct
_tuple1*_tmp9A8;void*_tmp9A9;struct Cyc_Absyn_Exp*_tmp9AA;struct _dynforward_ptr*
_tmp9AB;struct Cyc_Absyn_Exp*_tmp9AC;struct _dynforward_ptr*_tmp9AD;struct Cyc_Absyn_Exp*
_tmp9AE;struct Cyc_Absyn_Exp*_tmp9AF;struct Cyc_Absyn_Exp*_tmp9B0;_LL5E9: if(*((int*)
_tmp9A7)!= 1)goto _LL5EB;_tmp9A8=((struct Cyc_Absyn_Var_e_struct*)_tmp9A7)->f1;
_tmp9A9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9A7)->f2;_LL5EA: {void*
_tmp9B1=_tmp9A9;struct Cyc_Absyn_Vardecl*_tmp9B2;struct Cyc_Absyn_Vardecl*_tmp9B3;
struct Cyc_Absyn_Vardecl*_tmp9B4;struct Cyc_Absyn_Vardecl*_tmp9B5;_LL5F6: if((int)
_tmp9B1 != 0)goto _LL5F8;_LL5F7: goto _LL5F9;_LL5F8: if(_tmp9B1 <= (void*)1)goto _LL5FA;
if(*((int*)_tmp9B1)!= 1)goto _LL5FA;_LL5F9: return bogus_ans;_LL5FA: if(_tmp9B1 <= (
void*)1)goto _LL5FC;if(*((int*)_tmp9B1)!= 0)goto _LL5FC;_tmp9B2=((struct Cyc_Absyn_Global_b_struct*)
_tmp9B1)->f1;_LL5FB: {void*_tmp9B6=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL603: if(_tmp9B6 <= (void*)4)goto _LL605;if(*((int*)
_tmp9B6)!= 7)goto _LL605;_LL604: return({struct _tuple10 _tmp9B7;_tmp9B7.f1=1;
_tmp9B7.f2=(void*)2;_tmp9B7;});_LL605:;_LL606: return({struct _tuple10 _tmp9B8;
_tmp9B8.f1=(_tmp9B2->tq).real_const;_tmp9B8.f2=(void*)2;_tmp9B8;});_LL602:;}
_LL5FC: if(_tmp9B1 <= (void*)1)goto _LL5FE;if(*((int*)_tmp9B1)!= 3)goto _LL5FE;
_tmp9B3=((struct Cyc_Absyn_Local_b_struct*)_tmp9B1)->f1;_LL5FD: {void*_tmp9B9=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL608: if(_tmp9B9 <= (void*)
4)goto _LL60A;if(*((int*)_tmp9B9)!= 7)goto _LL60A;_LL609: return({struct _tuple10
_tmp9BA;_tmp9BA.f1=1;_tmp9BA.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp9B3->rgn))->v;
_tmp9BA;});_LL60A:;_LL60B: _tmp9B3->escapes=1;return({struct _tuple10 _tmp9BB;
_tmp9BB.f1=(_tmp9B3->tq).real_const;_tmp9BB.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp9B3->rgn))->v;_tmp9BB;});_LL607:;}_LL5FE: if(_tmp9B1 <= (void*)1)
goto _LL600;if(*((int*)_tmp9B1)!= 4)goto _LL600;_tmp9B4=((struct Cyc_Absyn_Pat_b_struct*)
_tmp9B1)->f1;_LL5FF: _tmp9B5=_tmp9B4;goto _LL601;_LL600: if(_tmp9B1 <= (void*)1)goto
_LL5F5;if(*((int*)_tmp9B1)!= 2)goto _LL5F5;_tmp9B5=((struct Cyc_Absyn_Param_b_struct*)
_tmp9B1)->f1;_LL601: _tmp9B5->escapes=1;return({struct _tuple10 _tmp9BC;_tmp9BC.f1=(
_tmp9B5->tq).real_const;_tmp9BC.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp9B5->rgn))->v;_tmp9BC;});_LL5F5:;}_LL5EB: if(*((int*)_tmp9A7)!= 23)goto _LL5ED;
_tmp9AA=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp9A7)->f1;_tmp9AB=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp9A7)->f2;_LL5EC: {void*_tmp9BD=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp9AA->topt))->v);struct Cyc_List_List*_tmp9BE;struct Cyc_Absyn_AggrInfo
_tmp9BF;union Cyc_Absyn_AggrInfoU_union _tmp9C0;struct Cyc_Absyn_Aggrdecl**_tmp9C1;
struct Cyc_Absyn_Aggrdecl*_tmp9C2;_LL60D: if(_tmp9BD <= (void*)4)goto _LL611;if(*((
int*)_tmp9BD)!= 11)goto _LL60F;_tmp9BE=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp9BD)->f2;_LL60E: {struct Cyc_Absyn_Aggrfield*_tmp9C3=Cyc_Absyn_lookup_field(
_tmp9BE,_tmp9AB);if(_tmp9C3 != 0  && _tmp9C3->width != 0)return({struct _tuple10
_tmp9C4;_tmp9C4.f1=(_tmp9C3->tq).real_const;_tmp9C4.f2=(Cyc_Tcutil_addressof_props(
te,_tmp9AA)).f2;_tmp9C4;});return bogus_ans;}_LL60F: if(*((int*)_tmp9BD)!= 10)goto
_LL611;_tmp9BF=((struct Cyc_Absyn_AggrType_struct*)_tmp9BD)->f1;_tmp9C0=_tmp9BF.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp9BD)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL611;_tmp9C1=(_tmp9C0.KnownAggr).f1;_tmp9C2=*_tmp9C1;_LL610: {struct
Cyc_Absyn_Aggrfield*_tmp9C5=Cyc_Absyn_lookup_decl_field(_tmp9C2,_tmp9AB);if(
_tmp9C5 != 0  && _tmp9C5->width != 0)return({struct _tuple10 _tmp9C6;_tmp9C6.f1=(
_tmp9C5->tq).real_const;_tmp9C6.f2=(Cyc_Tcutil_addressof_props(te,_tmp9AA)).f2;
_tmp9C6;});return bogus_ans;}_LL611:;_LL612: return bogus_ans;_LL60C:;}_LL5ED: if(*((
int*)_tmp9A7)!= 24)goto _LL5EF;_tmp9AC=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp9A7)->f1;_tmp9AD=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp9A7)->f2;_LL5EE: {
void*_tmp9C7=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp9AC->topt))->v);struct Cyc_Absyn_PtrInfo _tmp9C8;void*_tmp9C9;struct Cyc_Absyn_PtrAtts
_tmp9CA;void*_tmp9CB;_LL614: if(_tmp9C7 <= (void*)4)goto _LL616;if(*((int*)_tmp9C7)
!= 4)goto _LL616;_tmp9C8=((struct Cyc_Absyn_PointerType_struct*)_tmp9C7)->f1;
_tmp9C9=(void*)_tmp9C8.elt_typ;_tmp9CA=_tmp9C8.ptr_atts;_tmp9CB=(void*)_tmp9CA.rgn;
_LL615: {struct Cyc_Absyn_Aggrfield*finfo;{void*_tmp9CC=Cyc_Tcutil_compress(
_tmp9C9);struct Cyc_List_List*_tmp9CD;struct Cyc_Absyn_AggrInfo _tmp9CE;union Cyc_Absyn_AggrInfoU_union
_tmp9CF;struct Cyc_Absyn_Aggrdecl**_tmp9D0;struct Cyc_Absyn_Aggrdecl*_tmp9D1;
_LL619: if(_tmp9CC <= (void*)4)goto _LL61D;if(*((int*)_tmp9CC)!= 11)goto _LL61B;
_tmp9CD=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp9CC)->f2;_LL61A: finfo=Cyc_Absyn_lookup_field(
_tmp9CD,_tmp9AD);goto _LL618;_LL61B: if(*((int*)_tmp9CC)!= 10)goto _LL61D;_tmp9CE=((
struct Cyc_Absyn_AggrType_struct*)_tmp9CC)->f1;_tmp9CF=_tmp9CE.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmp9CC)->f1).aggr_info).KnownAggr).tag != 1)
goto _LL61D;_tmp9D0=(_tmp9CF.KnownAggr).f1;_tmp9D1=*_tmp9D0;_LL61C: finfo=Cyc_Absyn_lookup_decl_field(
_tmp9D1,_tmp9AD);goto _LL618;_LL61D:;_LL61E: return bogus_ans;_LL618:;}if(finfo != 0
 && finfo->width != 0)return({struct _tuple10 _tmp9D2;_tmp9D2.f1=(finfo->tq).real_const;
_tmp9D2.f2=_tmp9CB;_tmp9D2;});return bogus_ans;}_LL616:;_LL617: return bogus_ans;
_LL613:;}_LL5EF: if(*((int*)_tmp9A7)!= 22)goto _LL5F1;_tmp9AE=((struct Cyc_Absyn_Deref_e_struct*)
_tmp9A7)->f1;_LL5F0: {void*_tmp9D3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp9AE->topt))->v);struct Cyc_Absyn_PtrInfo _tmp9D4;struct Cyc_Absyn_Tqual
_tmp9D5;struct Cyc_Absyn_PtrAtts _tmp9D6;void*_tmp9D7;_LL620: if(_tmp9D3 <= (void*)4)
goto _LL622;if(*((int*)_tmp9D3)!= 4)goto _LL622;_tmp9D4=((struct Cyc_Absyn_PointerType_struct*)
_tmp9D3)->f1;_tmp9D5=_tmp9D4.elt_tq;_tmp9D6=_tmp9D4.ptr_atts;_tmp9D7=(void*)
_tmp9D6.rgn;_LL621: return({struct _tuple10 _tmp9D8;_tmp9D8.f1=_tmp9D5.real_const;
_tmp9D8.f2=_tmp9D7;_tmp9D8;});_LL622:;_LL623: return bogus_ans;_LL61F:;}_LL5F1: if(*((
int*)_tmp9A7)!= 25)goto _LL5F3;_tmp9AF=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp9A7)->f1;_tmp9B0=((struct Cyc_Absyn_Subscript_e_struct*)_tmp9A7)->f2;_LL5F2: {
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp9AF->topt))->v);
void*_tmp9D9=t;struct Cyc_List_List*_tmp9DA;struct Cyc_Absyn_PtrInfo _tmp9DB;struct
Cyc_Absyn_Tqual _tmp9DC;struct Cyc_Absyn_PtrAtts _tmp9DD;void*_tmp9DE;struct Cyc_Absyn_ArrayInfo
_tmp9DF;struct Cyc_Absyn_Tqual _tmp9E0;_LL625: if(_tmp9D9 <= (void*)4)goto _LL62B;if(*((
int*)_tmp9D9)!= 9)goto _LL627;_tmp9DA=((struct Cyc_Absyn_TupleType_struct*)_tmp9D9)->f1;
_LL626: {unsigned int _tmp9E2;int _tmp9E3;struct _tuple7 _tmp9E1=Cyc_Evexp_eval_const_uint_exp(
_tmp9B0);_tmp9E2=_tmp9E1.f1;_tmp9E3=_tmp9E1.f2;if(!_tmp9E3)return bogus_ans;{
struct _tuple4*_tmp9E4=Cyc_Absyn_lookup_tuple_field(_tmp9DA,(int)_tmp9E2);if(
_tmp9E4 != 0)return({struct _tuple10 _tmp9E5;_tmp9E5.f1=((*_tmp9E4).f1).real_const;
_tmp9E5.f2=(Cyc_Tcutil_addressof_props(te,_tmp9AF)).f2;_tmp9E5;});return
bogus_ans;}}_LL627: if(*((int*)_tmp9D9)!= 4)goto _LL629;_tmp9DB=((struct Cyc_Absyn_PointerType_struct*)
_tmp9D9)->f1;_tmp9DC=_tmp9DB.elt_tq;_tmp9DD=_tmp9DB.ptr_atts;_tmp9DE=(void*)
_tmp9DD.rgn;_LL628: return({struct _tuple10 _tmp9E6;_tmp9E6.f1=_tmp9DC.real_const;
_tmp9E6.f2=_tmp9DE;_tmp9E6;});_LL629: if(*((int*)_tmp9D9)!= 7)goto _LL62B;_tmp9DF=((
struct Cyc_Absyn_ArrayType_struct*)_tmp9D9)->f1;_tmp9E0=_tmp9DF.tq;_LL62A: return({
struct _tuple10 _tmp9E7;_tmp9E7.f1=_tmp9E0.real_const;_tmp9E7.f2=(Cyc_Tcutil_addressof_props(
te,_tmp9AF)).f2;_tmp9E7;});_LL62B:;_LL62C: return bogus_ans;_LL624:;}_LL5F3:;
_LL5F4:({void*_tmp9E8[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp9E9="unary & applied to non-lvalue";
_tag_dynforward(_tmp9E9,sizeof(char),_get_zero_arr_size(_tmp9E9,30));}),
_tag_dynforward(_tmp9E8,sizeof(void*),0));});return bogus_ans;_LL5E8:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmp9EB=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmp9EC;void*_tmp9ED;struct Cyc_Absyn_Tqual
_tmp9EE;struct Cyc_Absyn_Conref*_tmp9EF;_LL62E: if(_tmp9EB <= (void*)4)goto _LL630;
if(*((int*)_tmp9EB)!= 7)goto _LL630;_tmp9EC=((struct Cyc_Absyn_ArrayType_struct*)
_tmp9EB)->f1;_tmp9ED=(void*)_tmp9EC.elt_type;_tmp9EE=_tmp9EC.tq;_tmp9EF=_tmp9EC.zero_term;
_LL62F: {void*_tmp9F1;struct _tuple10 _tmp9F0=Cyc_Tcutil_addressof_props(te,e);
_tmp9F1=_tmp9F0.f2;return Cyc_Absyn_atb_typ(_tmp9ED,_tmp9F1,_tmp9EE,(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp9F2=_cycalloc(sizeof(*_tmp9F2));_tmp9F2[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp9F3;_tmp9F3.tag=0;_tmp9F3.f1=e;_tmp9F3;});
_tmp9F2;}),_tmp9EF);}_LL630:;_LL631: return e_typ;_LL62D:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(
b);{union Cyc_Absyn_Constraint_union _tmp9F4=b->v;void*_tmp9F5;void*_tmp9F6;void*
_tmp9F7;struct Cyc_Absyn_Exp*_tmp9F8;void*_tmp9F9;_LL633: if((_tmp9F4.Eq_constr).tag
!= 0)goto _LL635;_tmp9F5=(_tmp9F4.Eq_constr).f1;if((int)_tmp9F5 != 1)goto _LL635;
_LL634: return;_LL635: if((_tmp9F4.Eq_constr).tag != 0)goto _LL637;_tmp9F6=(_tmp9F4.Eq_constr).f1;
if((int)_tmp9F6 != 0)goto _LL637;_LL636: if(i < 0)({void*_tmp9FA[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp9FB="dereference is out of bounds";_tag_dynforward(_tmp9FB,
sizeof(char),_get_zero_arr_size(_tmp9FB,29));}),_tag_dynforward(_tmp9FA,sizeof(
void*),0));});return;_LL637: if((_tmp9F4.Eq_constr).tag != 0)goto _LL639;_tmp9F7=(
_tmp9F4.Eq_constr).f1;if(_tmp9F7 <= (void*)2)goto _LL639;if(*((int*)_tmp9F7)!= 0)
goto _LL639;_tmp9F8=((struct Cyc_Absyn_Upper_b_struct*)_tmp9F7)->f1;_LL638: {
unsigned int _tmp9FD;int _tmp9FE;struct _tuple7 _tmp9FC=Cyc_Evexp_eval_const_uint_exp(
_tmp9F8);_tmp9FD=_tmp9FC.f1;_tmp9FE=_tmp9FC.f2;if(_tmp9FE  && _tmp9FD <= i)({
struct Cyc_Int_pa_struct _tmpA02;_tmpA02.tag=1;_tmpA02.f1=(unsigned long)((int)i);{
struct Cyc_Int_pa_struct _tmpA01;_tmpA01.tag=1;_tmpA01.f1=(unsigned long)((int)
_tmp9FD);{void*_tmp9FF[2]={& _tmpA01,& _tmpA02};Cyc_Tcutil_terr(loc,({const char*
_tmpA00="dereference is out of bounds: %d <= %d";_tag_dynforward(_tmpA00,sizeof(
char),_get_zero_arr_size(_tmpA00,39));}),_tag_dynforward(_tmp9FF,sizeof(void*),2));}}});
return;}_LL639: if((_tmp9F4.Eq_constr).tag != 0)goto _LL63B;_tmp9F9=(_tmp9F4.Eq_constr).f1;
if(_tmp9F9 <= (void*)2)goto _LL63B;if(*((int*)_tmp9F9)!= 1)goto _LL63B;_LL63A:
return;_LL63B: if((_tmp9F4.No_constr).tag != 2)goto _LL63D;_LL63C: b->v=(union Cyc_Absyn_Constraint_union)({
union Cyc_Absyn_Constraint_union _tmpA03;(_tmpA03.Eq_constr).tag=0;(_tmpA03.Eq_constr).f1=(
void*)((void*)((void*)({struct Cyc_Absyn_Upper_b_struct*_tmpA04=_cycalloc(sizeof(*
_tmpA04));_tmpA04[0]=({struct Cyc_Absyn_Upper_b_struct _tmpA05;_tmpA05.tag=0;
_tmpA05.f1=Cyc_Absyn_uint_exp(i + 1,0);_tmpA05;});_tmpA04;})));_tmpA03;});return;
_LL63D:;_LL63E:({void*_tmpA06[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA07="check_bound -- bad compressed conref";
_tag_dynforward(_tmpA07,sizeof(char),_get_zero_arr_size(_tmpA07,37));}),
_tag_dynforward(_tmpA06,sizeof(void*),0));});_LL632:;}}void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(
loc,0,b);}int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b){union Cyc_Absyn_Constraint_union
_tmpA08=(Cyc_Absyn_compress_conref(b))->v;void*_tmpA09;struct Cyc_Absyn_Exp*
_tmpA0A;_LL640: if((_tmpA08.Eq_constr).tag != 0)goto _LL642;_tmpA09=(_tmpA08.Eq_constr).f1;
if(_tmpA09 <= (void*)2)goto _LL642;if(*((int*)_tmpA09)!= 0)goto _LL642;_tmpA0A=((
struct Cyc_Absyn_Upper_b_struct*)_tmpA09)->f1;_LL641: {unsigned int _tmpA0C;int
_tmpA0D;struct _tuple7 _tmpA0B=Cyc_Evexp_eval_const_uint_exp(_tmpA0A);_tmpA0C=
_tmpA0B.f1;_tmpA0D=_tmpA0B.f2;return _tmpA0D  && _tmpA0C == 1;}_LL642:;_LL643:
return 0;_LL63F:;}int Cyc_Tcutil_bits_only(void*t){void*_tmpA0E=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmpA0F;void*_tmpA10;struct Cyc_Absyn_Conref*_tmpA11;
struct Cyc_List_List*_tmpA12;struct Cyc_Absyn_AggrInfo _tmpA13;union Cyc_Absyn_AggrInfoU_union
_tmpA14;struct Cyc_Absyn_AggrInfo _tmpA15;union Cyc_Absyn_AggrInfoU_union _tmpA16;
struct Cyc_Absyn_Aggrdecl**_tmpA17;struct Cyc_Absyn_Aggrdecl*_tmpA18;struct Cyc_List_List*
_tmpA19;struct Cyc_List_List*_tmpA1A;_LL645: if((int)_tmpA0E != 0)goto _LL647;_LL646:
goto _LL648;_LL647: if(_tmpA0E <= (void*)4)goto _LL649;if(*((int*)_tmpA0E)!= 5)goto
_LL649;_LL648: goto _LL64A;_LL649: if((int)_tmpA0E != 1)goto _LL64B;_LL64A: goto _LL64C;
_LL64B: if(_tmpA0E <= (void*)4)goto _LL65B;if(*((int*)_tmpA0E)!= 6)goto _LL64D;
_LL64C: return 1;_LL64D: if(*((int*)_tmpA0E)!= 12)goto _LL64F;_LL64E: goto _LL650;
_LL64F: if(*((int*)_tmpA0E)!= 13)goto _LL651;_LL650: return 0;_LL651: if(*((int*)
_tmpA0E)!= 7)goto _LL653;_tmpA0F=((struct Cyc_Absyn_ArrayType_struct*)_tmpA0E)->f1;
_tmpA10=(void*)_tmpA0F.elt_type;_tmpA11=_tmpA0F.zero_term;_LL652: return !((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpA11) && Cyc_Tcutil_bits_only(
_tmpA10);_LL653: if(*((int*)_tmpA0E)!= 9)goto _LL655;_tmpA12=((struct Cyc_Absyn_TupleType_struct*)
_tmpA0E)->f1;_LL654: for(0;_tmpA12 != 0;_tmpA12=_tmpA12->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple4*)_tmpA12->hd)).f2))return 0;}return 1;_LL655: if(*((int*)_tmpA0E)!= 
10)goto _LL657;_tmpA13=((struct Cyc_Absyn_AggrType_struct*)_tmpA0E)->f1;_tmpA14=
_tmpA13.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA0E)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL657;_LL656: return 0;_LL657: if(*((int*)_tmpA0E)!= 10)goto _LL659;
_tmpA15=((struct Cyc_Absyn_AggrType_struct*)_tmpA0E)->f1;_tmpA16=_tmpA15.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA0E)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL659;_tmpA17=(_tmpA16.KnownAggr).f1;_tmpA18=*_tmpA17;_tmpA19=_tmpA15.targs;
_LL658: if(_tmpA18->impl == 0)return 0;{struct _RegionHandle _tmpA1B=_new_region("rgn");
struct _RegionHandle*rgn=& _tmpA1B;_push_region(rgn);{struct Cyc_List_List*_tmpA1C=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA18->tvs,_tmpA19);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA18->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpA1C,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpA1D=0;_npop_handler(0);
return _tmpA1D;}}}{int _tmpA1E=1;_npop_handler(0);return _tmpA1E;}};_pop_region(rgn);}
_LL659: if(*((int*)_tmpA0E)!= 11)goto _LL65B;_tmpA1A=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpA0E)->f2;_LL65A: for(0;_tmpA1A != 0;_tmpA1A=_tmpA1A->tl){if(!Cyc_Tcutil_bits_only((
void*)((struct Cyc_Absyn_Aggrfield*)_tmpA1A->hd)->type))return 0;}return 1;_LL65B:;
_LL65C: return 0;_LL644:;}struct _tuple22{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e){void*_tmpA1F=(void*)e->r;struct _tuple1*_tmpA20;void*_tmpA21;struct Cyc_Absyn_Exp*
_tmpA22;struct Cyc_Absyn_Exp*_tmpA23;struct Cyc_Absyn_Exp*_tmpA24;struct Cyc_Absyn_Exp*
_tmpA25;struct Cyc_Absyn_Exp*_tmpA26;struct Cyc_Absyn_Exp*_tmpA27;struct Cyc_Absyn_Exp*
_tmpA28;void*_tmpA29;struct Cyc_Absyn_Exp*_tmpA2A;void*_tmpA2B;void*_tmpA2C;
struct Cyc_Absyn_Exp*_tmpA2D;struct Cyc_Absyn_Exp*_tmpA2E;struct Cyc_Absyn_Exp*
_tmpA2F;struct Cyc_Absyn_Exp*_tmpA30;struct Cyc_List_List*_tmpA31;struct Cyc_List_List*
_tmpA32;struct Cyc_List_List*_tmpA33;void*_tmpA34;struct Cyc_List_List*_tmpA35;
struct Cyc_List_List*_tmpA36;struct Cyc_List_List*_tmpA37;_LL65E: if(*((int*)
_tmpA1F)!= 0)goto _LL660;_LL65F: goto _LL661;_LL660: if(*((int*)_tmpA1F)!= 18)goto
_LL662;_LL661: goto _LL663;_LL662: if(*((int*)_tmpA1F)!= 19)goto _LL664;_LL663: goto
_LL665;_LL664: if(*((int*)_tmpA1F)!= 20)goto _LL666;_LL665: goto _LL667;_LL666: if(*((
int*)_tmpA1F)!= 21)goto _LL668;_LL667: goto _LL669;_LL668: if(*((int*)_tmpA1F)!= 33)
goto _LL66A;_LL669: goto _LL66B;_LL66A: if(*((int*)_tmpA1F)!= 34)goto _LL66C;_LL66B:
return 1;_LL66C: if(*((int*)_tmpA1F)!= 1)goto _LL66E;_tmpA20=((struct Cyc_Absyn_Var_e_struct*)
_tmpA1F)->f1;_tmpA21=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA1F)->f2;_LL66D: {
void*_tmpA38=_tmpA21;struct Cyc_Absyn_Vardecl*_tmpA39;_LL68D: if(_tmpA38 <= (void*)
1)goto _LL691;if(*((int*)_tmpA38)!= 1)goto _LL68F;_LL68E: return 1;_LL68F: if(*((int*)
_tmpA38)!= 0)goto _LL691;_tmpA39=((struct Cyc_Absyn_Global_b_struct*)_tmpA38)->f1;
_LL690: {void*_tmpA3A=Cyc_Tcutil_compress((void*)_tmpA39->type);_LL696: if(
_tmpA3A <= (void*)4)goto _LL69A;if(*((int*)_tmpA3A)!= 7)goto _LL698;_LL697: goto
_LL699;_LL698: if(*((int*)_tmpA3A)!= 8)goto _LL69A;_LL699: return 1;_LL69A:;_LL69B:
return var_okay;_LL695:;}_LL691: if((int)_tmpA38 != 0)goto _LL693;_LL692: return 0;
_LL693:;_LL694: return var_okay;_LL68C:;}_LL66E: if(*((int*)_tmpA1F)!= 6)goto _LL670;
_tmpA22=((struct Cyc_Absyn_Conditional_e_struct*)_tmpA1F)->f1;_tmpA23=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpA1F)->f2;_tmpA24=((struct Cyc_Absyn_Conditional_e_struct*)_tmpA1F)->f3;_LL66F:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpA22) && Cyc_Tcutil_cnst_exp(te,0,_tmpA23))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpA24);_LL670: if(*((int*)_tmpA1F)!= 9)goto _LL672;
_tmpA25=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpA1F)->f1;_tmpA26=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpA1F)->f2;_LL671: return Cyc_Tcutil_cnst_exp(te,0,_tmpA25) && Cyc_Tcutil_cnst_exp(
te,0,_tmpA26);_LL672: if(*((int*)_tmpA1F)!= 13)goto _LL674;_tmpA27=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpA1F)->f1;_LL673: _tmpA28=_tmpA27;goto _LL675;_LL674: if(*((int*)_tmpA1F)!= 14)
goto _LL676;_tmpA28=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpA1F)->f1;_LL675:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpA28);_LL676: if(*((int*)_tmpA1F)!= 15)
goto _LL678;_tmpA29=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpA1F)->f1;_tmpA2A=((
struct Cyc_Absyn_Cast_e_struct*)_tmpA1F)->f2;_tmpA2B=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmpA1F)->f4;if((int)_tmpA2B != 1)goto _LL678;_LL677: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpA2A);_LL678: if(*((int*)_tmpA1F)!= 15)goto _LL67A;_tmpA2C=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmpA1F)->f1;_tmpA2D=((struct Cyc_Absyn_Cast_e_struct*)
_tmpA1F)->f2;_LL679: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpA2D);_LL67A: if(*((
int*)_tmpA1F)!= 16)goto _LL67C;_tmpA2E=((struct Cyc_Absyn_Address_e_struct*)
_tmpA1F)->f1;_LL67B: return Cyc_Tcutil_cnst_exp(te,1,_tmpA2E);_LL67C: if(*((int*)
_tmpA1F)!= 29)goto _LL67E;_tmpA2F=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpA1F)->f2;_tmpA30=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpA1F)->f3;
_LL67D: return Cyc_Tcutil_cnst_exp(te,0,_tmpA2F) && Cyc_Tcutil_cnst_exp(te,0,
_tmpA30);_LL67E: if(*((int*)_tmpA1F)!= 28)goto _LL680;_tmpA31=((struct Cyc_Absyn_Array_e_struct*)
_tmpA1F)->f1;_LL67F: _tmpA32=_tmpA31;goto _LL681;_LL680: if(*((int*)_tmpA1F)!= 31)
goto _LL682;_tmpA32=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpA1F)->f2;_LL681:
_tmpA33=_tmpA32;goto _LL683;_LL682: if(*((int*)_tmpA1F)!= 30)goto _LL684;_tmpA33=((
struct Cyc_Absyn_Struct_e_struct*)_tmpA1F)->f3;_LL683: for(0;_tmpA33 != 0;_tmpA33=
_tmpA33->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple22*)_tmpA33->hd)).f2))
return 0;}return 1;_LL684: if(*((int*)_tmpA1F)!= 3)goto _LL686;_tmpA34=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpA1F)->f1;_tmpA35=((struct Cyc_Absyn_Primop_e_struct*)
_tmpA1F)->f2;_LL685: _tmpA36=_tmpA35;goto _LL687;_LL686: if(*((int*)_tmpA1F)!= 26)
goto _LL688;_tmpA36=((struct Cyc_Absyn_Tuple_e_struct*)_tmpA1F)->f1;_LL687: _tmpA37=
_tmpA36;goto _LL689;_LL688: if(*((int*)_tmpA1F)!= 32)goto _LL68A;_tmpA37=((struct
Cyc_Absyn_Tunion_e_struct*)_tmpA1F)->f1;_LL689: for(0;_tmpA37 != 0;_tmpA37=_tmpA37->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmpA37->hd))return 0;}return 1;
_LL68A:;_LL68B: return 0;_LL65D:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmpA3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpA3C;
struct Cyc_Absyn_PtrAtts _tmpA3D;struct Cyc_Absyn_Conref*_tmpA3E;struct Cyc_Absyn_Conref*
_tmpA3F;struct Cyc_Absyn_ArrayInfo _tmpA40;void*_tmpA41;struct Cyc_List_List*
_tmpA42;struct Cyc_Absyn_AggrInfo _tmpA43;union Cyc_Absyn_AggrInfoU_union _tmpA44;
struct Cyc_List_List*_tmpA45;struct Cyc_List_List*_tmpA46;_LL69D: if((int)_tmpA3B != 
0)goto _LL69F;_LL69E: goto _LL6A0;_LL69F: if(_tmpA3B <= (void*)4)goto _LL6A1;if(*((int*)
_tmpA3B)!= 5)goto _LL6A1;_LL6A0: goto _LL6A2;_LL6A1: if((int)_tmpA3B != 1)goto _LL6A3;
_LL6A2: goto _LL6A4;_LL6A3: if(_tmpA3B <= (void*)4)goto _LL6B3;if(*((int*)_tmpA3B)!= 
6)goto _LL6A5;_LL6A4: return 1;_LL6A5: if(*((int*)_tmpA3B)!= 4)goto _LL6A7;_tmpA3C=((
struct Cyc_Absyn_PointerType_struct*)_tmpA3B)->f1;_tmpA3D=_tmpA3C.ptr_atts;
_tmpA3E=_tmpA3D.nullable;_tmpA3F=_tmpA3D.bounds;_LL6A6: {void*_tmpA47=Cyc_Absyn_conref_def((
void*)((void*)0),_tmpA3F);_LL6B6: if((int)_tmpA47 != 0)goto _LL6B8;_LL6B7: return 1;
_LL6B8: if((int)_tmpA47 != 1)goto _LL6BA;_LL6B9: return 1;_LL6BA:;_LL6BB: return((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmpA3E);_LL6B5:;}_LL6A7:
if(*((int*)_tmpA3B)!= 7)goto _LL6A9;_tmpA40=((struct Cyc_Absyn_ArrayType_struct*)
_tmpA3B)->f1;_tmpA41=(void*)_tmpA40.elt_type;_LL6A8: return Cyc_Tcutil_supports_default(
_tmpA41);_LL6A9: if(*((int*)_tmpA3B)!= 9)goto _LL6AB;_tmpA42=((struct Cyc_Absyn_TupleType_struct*)
_tmpA3B)->f1;_LL6AA: for(0;_tmpA42 != 0;_tmpA42=_tmpA42->tl){if(!Cyc_Tcutil_supports_default((*((
struct _tuple4*)_tmpA42->hd)).f2))return 0;}return 1;_LL6AB: if(*((int*)_tmpA3B)!= 
10)goto _LL6AD;_tmpA43=((struct Cyc_Absyn_AggrType_struct*)_tmpA3B)->f1;_tmpA44=
_tmpA43.aggr_info;_tmpA45=_tmpA43.targs;_LL6AC: {struct Cyc_Absyn_Aggrdecl*
_tmpA48=Cyc_Absyn_get_known_aggrdecl(_tmpA44);if(_tmpA48->impl == 0)return 0;if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA48->impl))->exist_vars != 0)return
0;return Cyc_Tcutil_fields_support_default(_tmpA48->tvs,_tmpA45,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpA48->impl))->fields);}_LL6AD: if(*((int*)_tmpA3B)!= 11)goto _LL6AF;
_tmpA46=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpA3B)->f2;_LL6AE: return Cyc_Tcutil_fields_support_default(
0,0,_tmpA46);_LL6AF: if(*((int*)_tmpA3B)!= 13)goto _LL6B1;_LL6B0: goto _LL6B2;_LL6B1:
if(*((int*)_tmpA3B)!= 12)goto _LL6B3;_LL6B2: return 1;_LL6B3:;_LL6B4: return 0;_LL69C:;}
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*
ts,struct Cyc_List_List*fs){{struct _RegionHandle _tmpA49=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpA49;_push_region(rgn);{struct Cyc_List_List*_tmpA4A=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpA4A,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpA4B=0;_npop_handler(0);return _tmpA4B;}}};
_pop_region(rgn);}return 1;}int Cyc_Tcutil_admits_zero(void*t){void*_tmpA4C=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpA4D;struct Cyc_Absyn_PtrAtts _tmpA4E;struct Cyc_Absyn_Conref*
_tmpA4F;struct Cyc_Absyn_Conref*_tmpA50;_LL6BD: if(_tmpA4C <= (void*)4)goto _LL6BF;
if(*((int*)_tmpA4C)!= 5)goto _LL6BF;_LL6BE: goto _LL6C0;_LL6BF: if((int)_tmpA4C != 1)
goto _LL6C1;_LL6C0: goto _LL6C2;_LL6C1: if(_tmpA4C <= (void*)4)goto _LL6C5;if(*((int*)
_tmpA4C)!= 6)goto _LL6C3;_LL6C2: return 1;_LL6C3: if(*((int*)_tmpA4C)!= 4)goto _LL6C5;
_tmpA4D=((struct Cyc_Absyn_PointerType_struct*)_tmpA4C)->f1;_tmpA4E=_tmpA4D.ptr_atts;
_tmpA4F=_tmpA4E.nullable;_tmpA50=_tmpA4E.bounds;_LL6C4: {union Cyc_Absyn_Constraint_union
_tmpA51=(Cyc_Absyn_compress_conref(_tmpA50))->v;void*_tmpA52;void*_tmpA53;_LL6C8:
if((_tmpA51.Eq_constr).tag != 0)goto _LL6CA;_tmpA52=(_tmpA51.Eq_constr).f1;if((int)
_tmpA52 != 0)goto _LL6CA;_LL6C9: return 0;_LL6CA: if((_tmpA51.Eq_constr).tag != 0)goto
_LL6CC;_tmpA53=(_tmpA51.Eq_constr).f1;if((int)_tmpA53 != 1)goto _LL6CC;_LL6CB:
return 0;_LL6CC: if((_tmpA51.Eq_constr).tag != 0)goto _LL6CE;_LL6CD: {union Cyc_Absyn_Constraint_union
_tmpA54=(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmpA4F))->v;int _tmpA55;_LL6D1: if((_tmpA54.Eq_constr).tag != 0)goto _LL6D3;_tmpA55=(
_tmpA54.Eq_constr).f1;_LL6D2: return _tmpA55;_LL6D3:;_LL6D4: return 0;_LL6D0:;}
_LL6CE:;_LL6CF: return 0;_LL6C7:;}_LL6C5:;_LL6C6: return 0;_LL6BC:;}int Cyc_Tcutil_is_noreturn(
void*t){{void*_tmpA56=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpA57;void*
_tmpA58;struct Cyc_Absyn_FnInfo _tmpA59;struct Cyc_List_List*_tmpA5A;_LL6D6: if(
_tmpA56 <= (void*)4)goto _LL6DA;if(*((int*)_tmpA56)!= 4)goto _LL6D8;_tmpA57=((
struct Cyc_Absyn_PointerType_struct*)_tmpA56)->f1;_tmpA58=(void*)_tmpA57.elt_typ;
_LL6D7: return Cyc_Tcutil_is_noreturn(_tmpA58);_LL6D8: if(*((int*)_tmpA56)!= 8)goto
_LL6DA;_tmpA59=((struct Cyc_Absyn_FnType_struct*)_tmpA56)->f1;_tmpA5A=_tmpA59.attributes;
_LL6D9: for(0;_tmpA5A != 0;_tmpA5A=_tmpA5A->tl){void*_tmpA5B=(void*)_tmpA5A->hd;
_LL6DD: if((int)_tmpA5B != 3)goto _LL6DF;_LL6DE: return 1;_LL6DF:;_LL6E0: continue;
_LL6DC:;}goto _LL6D5;_LL6DA:;_LL6DB: goto _LL6D5;_LL6D5:;}return 0;}struct Cyc_List_List*
Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){void*_tmpA5C=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpA5D;struct Cyc_List_List*_tmpA5E;
struct Cyc_List_List**_tmpA5F;_LL6E2: if(_tmpA5C <= (void*)4)goto _LL6E4;if(*((int*)
_tmpA5C)!= 8)goto _LL6E4;_tmpA5D=((struct Cyc_Absyn_FnType_struct*)_tmpA5C)->f1;
_tmpA5E=_tmpA5D.attributes;_tmpA5F=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmpA5C)->f1).attributes;_LL6E3: {struct Cyc_List_List*_tmpA60=0;for(0;atts != 0;
atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))*_tmpA5F=({struct Cyc_List_List*
_tmpA61=_cycalloc(sizeof(*_tmpA61));_tmpA61->hd=(void*)((void*)atts->hd);_tmpA61->tl=*
_tmpA5F;_tmpA61;});else{_tmpA60=({struct Cyc_List_List*_tmpA62=_cycalloc(sizeof(*
_tmpA62));_tmpA62->hd=(void*)((void*)atts->hd);_tmpA62->tl=_tmpA60;_tmpA62;});}}
return _tmpA60;}_LL6E4:;_LL6E5:({void*_tmpA63[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpA64="transfer_fn_type_atts";_tag_dynforward(_tmpA64,sizeof(char),
_get_zero_arr_size(_tmpA64,22));}),_tag_dynforward(_tmpA63,sizeof(void*),0));});
_LL6E1:;}
