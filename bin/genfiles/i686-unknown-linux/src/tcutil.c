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
struct Cyc_Position_Error*);struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple1{union Cyc_Absyn_Nmspace_union
f1;struct _dynforward_ptr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
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
struct Cyc_List_List*_tmp68;struct Cyc_Core_Opt*_tmp69;struct Cyc_Absyn_TunionFieldInfo
_tmp6A;void*_tmp6B;struct Cyc_List_List*_tmp6C;struct Cyc_Absyn_PtrInfo _tmp6D;void*
_tmp6E;struct Cyc_Absyn_Tqual _tmp6F;struct Cyc_Absyn_PtrAtts _tmp70;void*_tmp71;
struct Cyc_Absyn_Conref*_tmp72;struct Cyc_Absyn_Conref*_tmp73;struct Cyc_Absyn_Conref*
_tmp74;struct Cyc_Absyn_PtrLoc*_tmp75;void*_tmp76;void*_tmp77;int _tmp78;struct Cyc_Absyn_ArrayInfo
_tmp79;void*_tmp7A;struct Cyc_Absyn_Tqual _tmp7B;struct Cyc_Absyn_Exp*_tmp7C;struct
Cyc_Absyn_Conref*_tmp7D;struct Cyc_Position_Segment*_tmp7E;struct Cyc_Absyn_FnInfo
_tmp7F;struct Cyc_List_List*_tmp80;struct Cyc_Core_Opt*_tmp81;void*_tmp82;struct
Cyc_List_List*_tmp83;int _tmp84;struct Cyc_Absyn_VarargInfo*_tmp85;struct Cyc_List_List*
_tmp86;struct Cyc_List_List*_tmp87;struct Cyc_List_List*_tmp88;struct Cyc_Absyn_AggrInfo
_tmp89;void*_tmp8A;void*_tmp8B;struct _tuple1*_tmp8C;struct Cyc_List_List*_tmp8D;
struct Cyc_Absyn_AggrInfo _tmp8E;void*_tmp8F;struct Cyc_Absyn_Aggrdecl**_tmp90;
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
struct Cyc_Absyn_TunionType_struct*)_tmp64)->f1;_tmp67=(void*)_tmp66.tunion_info;
_tmp68=_tmp66.targs;_tmp69=_tmp66.rgn;_LL21: {struct Cyc_Core_Opt*_tmpA3=(
unsigned int)_tmp69?({struct Cyc_Core_Opt*_tmpA7=_cycalloc(sizeof(*_tmpA7));
_tmpA7->v=(void*)Cyc_Tcutil_copy_type((void*)_tmp69->v);_tmpA7;}): 0;return(void*)({
struct Cyc_Absyn_TunionType_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=({
struct Cyc_Absyn_TunionType_struct _tmpA5;_tmpA5.tag=2;_tmpA5.f1=({struct Cyc_Absyn_TunionInfo
_tmpA6;_tmpA6.tunion_info=(void*)_tmp67;_tmpA6.targs=Cyc_Tcutil_copy_types(
_tmp68);_tmpA6.rgn=_tmpA3;_tmpA6;});_tmpA5;});_tmpA4;});}_LL22: if(*((int*)_tmp64)
!= 3)goto _LL24;_tmp6A=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp64)->f1;
_tmp6B=(void*)_tmp6A.field_info;_tmp6C=_tmp6A.targs;_LL23: return(void*)({struct
Cyc_Absyn_TunionFieldType_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8[0]=({
struct Cyc_Absyn_TunionFieldType_struct _tmpA9;_tmpA9.tag=3;_tmpA9.f1=({struct Cyc_Absyn_TunionFieldInfo
_tmpAA;_tmpAA.field_info=(void*)_tmp6B;_tmpAA.targs=Cyc_Tcutil_copy_types(_tmp6C);
_tmpAA;});_tmpA9;});_tmpA8;});_LL24: if(*((int*)_tmp64)!= 4)goto _LL26;_tmp6D=((
struct Cyc_Absyn_PointerType_struct*)_tmp64)->f1;_tmp6E=(void*)_tmp6D.elt_typ;
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
_tmp64)->f1;_tmp8A=(void*)_tmp89.aggr_info;if(*((int*)_tmp8A)!= 0)goto _LL34;
_tmp8B=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp8A)->f1;_tmp8C=((struct
Cyc_Absyn_UnknownAggr_struct*)_tmp8A)->f2;_tmp8D=_tmp89.targs;_LL33: return(void*)({
struct Cyc_Absyn_AggrType_struct*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA[0]=({
struct Cyc_Absyn_AggrType_struct _tmpCB;_tmpCB.tag=10;_tmpCB.f1=({struct Cyc_Absyn_AggrInfo
_tmpCC;_tmpCC.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmpCE;_tmpCE.tag=0;_tmpCE.f1=(void*)_tmp8B;_tmpCE.f2=_tmp8C;_tmpCE;});_tmpCD;}));
_tmpCC.targs=Cyc_Tcutil_copy_types(_tmp8D);_tmpCC;});_tmpCB;});_tmpCA;});_LL34:
if(*((int*)_tmp64)!= 10)goto _LL36;_tmp8E=((struct Cyc_Absyn_AggrType_struct*)
_tmp64)->f1;_tmp8F=(void*)_tmp8E.aggr_info;if(*((int*)_tmp8F)!= 1)goto _LL36;
_tmp90=((struct Cyc_Absyn_KnownAggr_struct*)_tmp8F)->f1;_tmp91=_tmp8E.targs;_LL35:
return(void*)({struct Cyc_Absyn_AggrType_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));
_tmpCF[0]=({struct Cyc_Absyn_AggrType_struct _tmpD0;_tmpD0.tag=10;_tmpD0.f1=({
struct Cyc_Absyn_AggrInfo _tmpD1;_tmpD1.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*
_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2[0]=({struct Cyc_Absyn_KnownAggr_struct
_tmpD3;_tmpD3.tag=1;_tmpD3.f1=_tmp90;_tmpD3;});_tmpD2;}));_tmpD1.targs=Cyc_Tcutil_copy_types(
_tmp91);_tmpD1;});_tmpD0;});_tmpCF;});_LL36: if(*((int*)_tmp64)!= 11)goto _LL38;
_tmp92=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp64)->f1;_tmp93=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp64)->f2;_LL37: return(void*)({struct Cyc_Absyn_AnonAggrType_struct*
_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmpD5;_tmpD5.tag=11;_tmpD5.f1=(void*)_tmp92;_tmpD5.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp93);_tmpD5;});_tmpD4;});_LL38: if(*((int*)
_tmp64)!= 12)goto _LL3A;_tmp94=((struct Cyc_Absyn_EnumType_struct*)_tmp64)->f1;
_tmp95=((struct Cyc_Absyn_EnumType_struct*)_tmp64)->f2;_LL39: return(void*)({
struct Cyc_Absyn_EnumType_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6[0]=({
struct Cyc_Absyn_EnumType_struct _tmpD7;_tmpD7.tag=12;_tmpD7.f1=_tmp94;_tmpD7.f2=
_tmp95;_tmpD7;});_tmpD6;});_LL3A: if(*((int*)_tmp64)!= 13)goto _LL3C;_tmp96=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp64)->f1;_LL3B: return(void*)({struct Cyc_Absyn_AnonEnumType_struct*
_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmpD9;_tmpD9.tag=13;_tmpD9.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*
f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,
_tmp96);_tmpD9;});_tmpD8;});_LL3C: if(*((int*)_tmp64)!= 14)goto _LL3E;_tmp97=(void*)((
struct Cyc_Absyn_SizeofType_struct*)_tmp64)->f1;_LL3D: return(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA[0]=({struct Cyc_Absyn_SizeofType_struct
_tmpDB;_tmpDB.tag=14;_tmpDB.f1=(void*)Cyc_Tcutil_copy_type(_tmp97);_tmpDB;});
_tmpDA;});_LL3E: if(*((int*)_tmp64)!= 18)goto _LL40;_tmp98=((struct Cyc_Absyn_TypeInt_struct*)
_tmp64)->f1;_LL3F: return(void*)({struct Cyc_Absyn_TypeInt_struct*_tmpDC=
_cycalloc_atomic(sizeof(*_tmpDC));_tmpDC[0]=({struct Cyc_Absyn_TypeInt_struct
_tmpDD;_tmpDD.tag=18;_tmpDD.f1=_tmp98;_tmpDD;});_tmpDC;});_LL40: if(*((int*)
_tmp64)!= 17)goto _LL42;_tmp99=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp64)->f1;
_LL41: return(void*)({struct Cyc_Absyn_TagType_struct*_tmpDE=_cycalloc(sizeof(*
_tmpDE));_tmpDE[0]=({struct Cyc_Absyn_TagType_struct _tmpDF;_tmpDF.tag=17;_tmpDF.f1=(
void*)Cyc_Tcutil_copy_type(_tmp99);_tmpDF;});_tmpDE;});_LL42: if(*((int*)_tmp64)
!= 15)goto _LL44;_tmp9A=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp64)->f1;
_LL43: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpE0=_cycalloc(
sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmpE1;_tmpE1.tag=
15;_tmpE1.f1=(void*)Cyc_Tcutil_copy_type(_tmp9A);_tmpE1;});_tmpE0;});_LL44: if(*((
int*)_tmp64)!= 16)goto _LL46;_tmp9B=((struct Cyc_Absyn_TypedefType_struct*)_tmp64)->f1;
_tmp9C=((struct Cyc_Absyn_TypedefType_struct*)_tmp64)->f2;_tmp9D=((struct Cyc_Absyn_TypedefType_struct*)
_tmp64)->f3;_LL45: return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmpE2=
_cycalloc(sizeof(*_tmpE2));_tmpE2[0]=({struct Cyc_Absyn_TypedefType_struct _tmpE3;
_tmpE3.tag=16;_tmpE3.f1=_tmp9B;_tmpE3.f2=Cyc_Tcutil_copy_types(_tmp9C);_tmpE3.f3=
_tmp9D;_tmpE3.f4=0;_tmpE3;});_tmpE2;});_LL46: if((int)_tmp64 != 3)goto _LL48;_LL47:
goto _LL49;_LL48: if((int)_tmp64 != 2)goto _LL4A;_LL49: return t;_LL4A: if(_tmp64 <= (
void*)4)goto _LL4C;if(*((int*)_tmp64)!= 19)goto _LL4C;_tmp9E=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp64)->f1;_LL4B: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmpE4=
_cycalloc(sizeof(*_tmpE4));_tmpE4[0]=({struct Cyc_Absyn_AccessEff_struct _tmpE5;
_tmpE5.tag=19;_tmpE5.f1=(void*)Cyc_Tcutil_copy_type(_tmp9E);_tmpE5;});_tmpE4;});
_LL4C: if(_tmp64 <= (void*)4)goto _LL4E;if(*((int*)_tmp64)!= 20)goto _LL4E;_tmp9F=((
struct Cyc_Absyn_JoinEff_struct*)_tmp64)->f1;_LL4D: return(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6[0]=({struct Cyc_Absyn_JoinEff_struct
_tmpE7;_tmpE7.tag=20;_tmpE7.f1=Cyc_Tcutil_copy_types(_tmp9F);_tmpE7;});_tmpE6;});
_LL4E: if(_tmp64 <= (void*)4)goto _LL19;if(*((int*)_tmp64)!= 21)goto _LL19;_tmpA0=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp64)->f1;_LL4F: return(void*)({struct
Cyc_Absyn_RgnsEff_struct*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmpE9;_tmpE9.tag=21;_tmpE9.f1=(void*)Cyc_Tcutil_copy_type(_tmpA0);_tmpE9;});
_tmpE8;});_LL19:;}int Cyc_Tcutil_kind_leq(void*k1,void*k2){if(k1 == k2)return 1;{
struct _tuple6 _tmpEB=({struct _tuple6 _tmpEA;_tmpEA.f1=k1;_tmpEA.f2=k2;_tmpEA;});
void*_tmpEC;void*_tmpED;void*_tmpEE;void*_tmpEF;void*_tmpF0;void*_tmpF1;_LL51:
_tmpEC=_tmpEB.f1;if((int)_tmpEC != 2)goto _LL53;_tmpED=_tmpEB.f2;if((int)_tmpED != 
1)goto _LL53;_LL52: goto _LL54;_LL53: _tmpEE=_tmpEB.f1;if((int)_tmpEE != 2)goto _LL55;
_tmpEF=_tmpEB.f2;if((int)_tmpEF != 0)goto _LL55;_LL54: goto _LL56;_LL55: _tmpF0=
_tmpEB.f1;if((int)_tmpF0 != 1)goto _LL57;_tmpF1=_tmpEB.f2;if((int)_tmpF1 != 0)goto
_LL57;_LL56: return 1;_LL57:;_LL58: return 0;_LL50:;}}void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*tv){void*_tmpF2=Cyc_Absyn_compress_kb((void*)tv->kind);void*
_tmpF3;void*_tmpF4;_LL5A: if(*((int*)_tmpF2)!= 0)goto _LL5C;_tmpF3=(void*)((struct
Cyc_Absyn_Eq_kb_struct*)_tmpF2)->f1;_LL5B: return _tmpF3;_LL5C: if(*((int*)_tmpF2)
!= 2)goto _LL5E;_tmpF4=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpF2)->f2;_LL5D:
return _tmpF4;_LL5E:;_LL5F:({void*_tmpF5[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpF6="kind not suitably constrained!";
_tag_dynforward(_tmpF6,sizeof(char),_get_zero_arr_size(_tmpF6,31));}),
_tag_dynforward(_tmpF5,sizeof(void*),0));});_LL59:;}void*Cyc_Tcutil_typ_kind(
void*t){void*_tmpF7=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpF8;struct Cyc_Core_Opt*
_tmpF9;struct Cyc_Absyn_Tvar*_tmpFA;void*_tmpFB;struct Cyc_Absyn_TunionInfo _tmpFC;
void*_tmpFD;struct Cyc_Absyn_UnknownTunionInfo _tmpFE;struct Cyc_Absyn_TunionInfo
_tmpFF;void*_tmp100;struct Cyc_Absyn_Tuniondecl**_tmp101;struct Cyc_Absyn_Tuniondecl*
_tmp102;struct Cyc_Absyn_TunionFieldInfo _tmp103;void*_tmp104;struct Cyc_Absyn_Tuniondecl*
_tmp105;struct Cyc_Absyn_Tunionfield*_tmp106;struct Cyc_Absyn_TunionFieldInfo
_tmp107;void*_tmp108;struct Cyc_Absyn_Enumdecl*_tmp109;struct Cyc_Absyn_AggrInfo
_tmp10A;void*_tmp10B;struct Cyc_Absyn_AggrInfo _tmp10C;void*_tmp10D;struct Cyc_Absyn_Aggrdecl**
_tmp10E;struct Cyc_Absyn_Aggrdecl*_tmp10F;struct Cyc_Absyn_Aggrdecl _tmp110;struct
Cyc_Absyn_AggrdeclImpl*_tmp111;struct Cyc_Absyn_Enumdecl*_tmp112;struct Cyc_Absyn_PtrInfo
_tmp113;struct Cyc_Absyn_Typedefdecl*_tmp114;_LL61: if(_tmpF7 <= (void*)4)goto _LL65;
if(*((int*)_tmpF7)!= 0)goto _LL63;_tmpF8=((struct Cyc_Absyn_Evar_struct*)_tmpF7)->f1;
_tmpF9=((struct Cyc_Absyn_Evar_struct*)_tmpF7)->f2;_LL62: return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF8))->v;_LL63: if(*((int*)_tmpF7)!= 1)goto _LL65;_tmpFA=((struct
Cyc_Absyn_VarType_struct*)_tmpF7)->f1;_LL64: return Cyc_Tcutil_tvar_kind(_tmpFA);
_LL65: if((int)_tmpF7 != 0)goto _LL67;_LL66: return(void*)1;_LL67: if(_tmpF7 <= (void*)
4)goto _LL69;if(*((int*)_tmpF7)!= 5)goto _LL69;_tmpFB=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpF7)->f2;_LL68: return(_tmpFB == (void*)((void*)2) || _tmpFB == (void*)((void*)3))?(
void*)2:(void*)1;_LL69: if((int)_tmpF7 != 1)goto _LL6B;_LL6A: goto _LL6C;_LL6B: if(
_tmpF7 <= (void*)4)goto _LL71;if(*((int*)_tmpF7)!= 6)goto _LL6D;_LL6C: goto _LL6E;
_LL6D: if(*((int*)_tmpF7)!= 8)goto _LL6F;_LL6E: return(void*)1;_LL6F: if(*((int*)
_tmpF7)!= 15)goto _LL71;_LL70: return(void*)2;_LL71: if((int)_tmpF7 != 3)goto _LL73;
_LL72: goto _LL74;_LL73: if((int)_tmpF7 != 2)goto _LL75;_LL74: return(void*)3;_LL75:
if(_tmpF7 <= (void*)4)goto _LL77;if(*((int*)_tmpF7)!= 2)goto _LL77;_tmpFC=((struct
Cyc_Absyn_TunionType_struct*)_tmpF7)->f1;_tmpFD=(void*)_tmpFC.tunion_info;if(*((
int*)_tmpFD)!= 0)goto _LL77;_tmpFE=((struct Cyc_Absyn_UnknownTunion_struct*)_tmpFD)->f1;
_LL76: if(_tmpFE.is_flat)return(void*)1;else{return(void*)2;}_LL77: if(_tmpF7 <= (
void*)4)goto _LL79;if(*((int*)_tmpF7)!= 2)goto _LL79;_tmpFF=((struct Cyc_Absyn_TunionType_struct*)
_tmpF7)->f1;_tmp100=(void*)_tmpFF.tunion_info;if(*((int*)_tmp100)!= 1)goto _LL79;
_tmp101=((struct Cyc_Absyn_KnownTunion_struct*)_tmp100)->f1;_tmp102=*_tmp101;
_LL78: if(_tmp102->is_flat)return(void*)1;else{return(void*)2;}_LL79: if(_tmpF7 <= (
void*)4)goto _LL7B;if(*((int*)_tmpF7)!= 3)goto _LL7B;_tmp103=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF7)->f1;_tmp104=(void*)_tmp103.field_info;if(*((int*)_tmp104)!= 1)goto _LL7B;
_tmp105=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp104)->f1;_tmp106=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp104)->f2;_LL7A: if(_tmp105->is_flat)return(
void*)1;else{if(_tmp106->typs == 0)return(void*)2;else{return(void*)1;}}_LL7B: if(
_tmpF7 <= (void*)4)goto _LL7D;if(*((int*)_tmpF7)!= 3)goto _LL7D;_tmp107=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF7)->f1;_tmp108=(void*)_tmp107.field_info;if(*((int*)_tmp108)!= 0)goto _LL7D;
_LL7C:({void*_tmp115[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp116="typ_kind: Unresolved TunionFieldType";
_tag_dynforward(_tmp116,sizeof(char),_get_zero_arr_size(_tmp116,37));}),
_tag_dynforward(_tmp115,sizeof(void*),0));});_LL7D: if(_tmpF7 <= (void*)4)goto
_LL7F;if(*((int*)_tmpF7)!= 12)goto _LL7F;_tmp109=((struct Cyc_Absyn_EnumType_struct*)
_tmpF7)->f2;if(_tmp109 != 0)goto _LL7F;_LL7E: goto _LL80;_LL7F: if(_tmpF7 <= (void*)4)
goto _LL81;if(*((int*)_tmpF7)!= 10)goto _LL81;_tmp10A=((struct Cyc_Absyn_AggrType_struct*)
_tmpF7)->f1;_tmp10B=(void*)_tmp10A.aggr_info;if(*((int*)_tmp10B)!= 0)goto _LL81;
_LL80: return(void*)0;_LL81: if(_tmpF7 <= (void*)4)goto _LL83;if(*((int*)_tmpF7)!= 
10)goto _LL83;_tmp10C=((struct Cyc_Absyn_AggrType_struct*)_tmpF7)->f1;_tmp10D=(
void*)_tmp10C.aggr_info;if(*((int*)_tmp10D)!= 1)goto _LL83;_tmp10E=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp10D)->f1;_tmp10F=*_tmp10E;_tmp110=*_tmp10F;_tmp111=_tmp110.impl;_LL82: return
_tmp111 == 0?(void*)0:(void*)1;_LL83: if(_tmpF7 <= (void*)4)goto _LL85;if(*((int*)
_tmpF7)!= 11)goto _LL85;_LL84: goto _LL86;_LL85: if(_tmpF7 <= (void*)4)goto _LL87;if(*((
int*)_tmpF7)!= 13)goto _LL87;_LL86: return(void*)1;_LL87: if(_tmpF7 <= (void*)4)goto
_LL89;if(*((int*)_tmpF7)!= 12)goto _LL89;_tmp112=((struct Cyc_Absyn_EnumType_struct*)
_tmpF7)->f2;_LL88: if(_tmp112->fields == 0)return(void*)0;else{return(void*)1;}
_LL89: if(_tmpF7 <= (void*)4)goto _LL8B;if(*((int*)_tmpF7)!= 4)goto _LL8B;_tmp113=((
struct Cyc_Absyn_PointerType_struct*)_tmpF7)->f1;_LL8A: {void*_tmp117=(void*)(Cyc_Absyn_compress_conref((
_tmp113.ptr_atts).bounds))->v;void*_tmp118;void*_tmp119;void*_tmp11A;void*
_tmp11B;_LL9E: if((int)_tmp117 != 0)goto _LLA0;_LL9F: goto _LLA1;_LLA0: if(_tmp117 <= (
void*)1)goto _LLA2;if(*((int*)_tmp117)!= 0)goto _LLA2;_tmp118=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp117)->f1;if((int)_tmp118 != 0)goto _LLA2;_LLA1: return(void*)1;_LLA2: if(_tmp117
<= (void*)1)goto _LLA4;if(*((int*)_tmp117)!= 0)goto _LLA4;_tmp119=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmp117)->f1;if((int)_tmp119 != 1)goto _LLA4;_LLA3:
return(void*)1;_LLA4: if(_tmp117 <= (void*)1)goto _LLA6;if(*((int*)_tmp117)!= 0)
goto _LLA6;_tmp11A=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp117)->f1;if(
_tmp11A <= (void*)2)goto _LLA6;if(*((int*)_tmp11A)!= 0)goto _LLA6;_LLA5: goto _LLA7;
_LLA6: if(_tmp117 <= (void*)1)goto _LLA8;if(*((int*)_tmp117)!= 0)goto _LLA8;_tmp11B=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp117)->f1;if(_tmp11B <= (void*)2)
goto _LLA8;if(*((int*)_tmp11B)!= 1)goto _LLA8;_LLA7: return(void*)2;_LLA8: if(
_tmp117 <= (void*)1)goto _LL9D;if(*((int*)_tmp117)!= 1)goto _LL9D;_LLA9:({void*
_tmp11C[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp11D="typ_kind: forward constr in ptr bounds";_tag_dynforward(
_tmp11D,sizeof(char),_get_zero_arr_size(_tmp11D,39));}),_tag_dynforward(_tmp11C,
sizeof(void*),0));});_LL9D:;}_LL8B: if(_tmpF7 <= (void*)4)goto _LL8D;if(*((int*)
_tmpF7)!= 14)goto _LL8D;_LL8C: return(void*)2;_LL8D: if(_tmpF7 <= (void*)4)goto _LL8F;
if(*((int*)_tmpF7)!= 18)goto _LL8F;_LL8E: return(void*)5;_LL8F: if(_tmpF7 <= (void*)
4)goto _LL91;if(*((int*)_tmpF7)!= 17)goto _LL91;_LL90: return(void*)2;_LL91: if(
_tmpF7 <= (void*)4)goto _LL93;if(*((int*)_tmpF7)!= 7)goto _LL93;_LL92: goto _LL94;
_LL93: if(_tmpF7 <= (void*)4)goto _LL95;if(*((int*)_tmpF7)!= 9)goto _LL95;_LL94:
return(void*)1;_LL95: if(_tmpF7 <= (void*)4)goto _LL97;if(*((int*)_tmpF7)!= 16)goto
_LL97;_tmp114=((struct Cyc_Absyn_TypedefType_struct*)_tmpF7)->f3;_LL96: if(_tmp114
== 0  || _tmp114->kind == 0)({struct Cyc_String_pa_struct _tmp120;_tmp120.tag=0;
_tmp120.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp11E[1]={& _tmp120};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp11F="typ_kind: typedef found: %s";
_tag_dynforward(_tmp11F,sizeof(char),_get_zero_arr_size(_tmp11F,28));}),
_tag_dynforward(_tmp11E,sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp114->kind))->v;_LL97: if(_tmpF7 <= (void*)4)goto _LL99;if(*((int*)
_tmpF7)!= 19)goto _LL99;_LL98: goto _LL9A;_LL99: if(_tmpF7 <= (void*)4)goto _LL9B;if(*((
int*)_tmpF7)!= 20)goto _LL9B;_LL9A: goto _LL9C;_LL9B: if(_tmpF7 <= (void*)4)goto _LL60;
if(*((int*)_tmpF7)!= 21)goto _LL60;_LL9C: return(void*)4;_LL60:;}int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp121;_push_handler(& _tmp121);{int _tmp123=
0;if(setjmp(_tmp121.handler))_tmp123=1;if(!_tmp123){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp124=1;_npop_handler(0);return _tmp124;};_pop_handler();}else{void*_tmp122=(
void*)_exn_thrown;void*_tmp126=_tmp122;_LLAB: if(_tmp126 != Cyc_Tcutil_Unify)goto
_LLAD;_LLAC: return 0;_LLAD:;_LLAE:(void)_throw(_tmp126);_LLAA:;}}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,void*t){t=Cyc_Tcutil_compress(t);{void*_tmp127=t;struct Cyc_Absyn_Tvar*
_tmp128;struct Cyc_Core_Opt*_tmp129;struct Cyc_Core_Opt*_tmp12A;struct Cyc_Core_Opt**
_tmp12B;struct Cyc_Absyn_PtrInfo _tmp12C;struct Cyc_Absyn_ArrayInfo _tmp12D;void*
_tmp12E;struct Cyc_Absyn_FnInfo _tmp12F;struct Cyc_List_List*_tmp130;struct Cyc_Core_Opt*
_tmp131;void*_tmp132;struct Cyc_List_List*_tmp133;int _tmp134;struct Cyc_Absyn_VarargInfo*
_tmp135;struct Cyc_List_List*_tmp136;struct Cyc_List_List*_tmp137;struct Cyc_List_List*
_tmp138;struct Cyc_Absyn_TunionInfo _tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_Core_Opt*
_tmp13B;struct Cyc_List_List*_tmp13C;struct Cyc_Absyn_TunionFieldInfo _tmp13D;
struct Cyc_List_List*_tmp13E;struct Cyc_Absyn_AggrInfo _tmp13F;struct Cyc_List_List*
_tmp140;struct Cyc_List_List*_tmp141;void*_tmp142;void*_tmp143;void*_tmp144;void*
_tmp145;void*_tmp146;struct Cyc_List_List*_tmp147;_LLB0: if(_tmp127 <= (void*)4)
goto _LLD2;if(*((int*)_tmp127)!= 1)goto _LLB2;_tmp128=((struct Cyc_Absyn_VarType_struct*)
_tmp127)->f1;_LLB1: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,_tmp128)){Cyc_Tcutil_failure_reason=({const char*_tmp148="(type variable would escape scope)";
_tag_dynforward(_tmp148,sizeof(char),_get_zero_arr_size(_tmp148,35));});(int)
_throw((void*)Cyc_Tcutil_Unify);}goto _LLAF;_LLB2: if(*((int*)_tmp127)!= 0)goto
_LLB4;_tmp129=((struct Cyc_Absyn_Evar_struct*)_tmp127)->f2;_tmp12A=((struct Cyc_Absyn_Evar_struct*)
_tmp127)->f4;_tmp12B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp127)->f4;_LLB3: if(t == evar){Cyc_Tcutil_failure_reason=({const char*_tmp149="(occurs check)";
_tag_dynforward(_tmp149,sizeof(char),_get_zero_arr_size(_tmp149,15));});(int)
_throw((void*)Cyc_Tcutil_Unify);}else{if(_tmp129 != 0)Cyc_Tcutil_occurs(evar,r,
env,(void*)_tmp129->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp12B))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp14A=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp12B))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd))_tmp14A=({struct Cyc_List_List*_tmp14B=
_cycalloc(sizeof(*_tmp14B));_tmp14B->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp14B->tl=
_tmp14A;_tmp14B;});}}*_tmp12B=({struct Cyc_Core_Opt*_tmp14C=_cycalloc(sizeof(*
_tmp14C));_tmp14C->v=_tmp14A;_tmp14C;});}}}goto _LLAF;_LLB4: if(*((int*)_tmp127)!= 
4)goto _LLB6;_tmp12C=((struct Cyc_Absyn_PointerType_struct*)_tmp127)->f1;_LLB5: Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp12C.elt_typ);Cyc_Tcutil_occurs(evar,r,env,(void*)(_tmp12C.ptr_atts).rgn);{
void*_tmp14D=(void*)(Cyc_Absyn_compress_conref((_tmp12C.ptr_atts).bounds))->v;
void*_tmp14E;void*_tmp14F;_LLD5: if(_tmp14D <= (void*)1)goto _LLD7;if(*((int*)
_tmp14D)!= 0)goto _LLD7;_tmp14E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp14D)->f1;if(_tmp14E <= (void*)2)goto _LLD7;if(*((int*)_tmp14E)!= 1)goto _LLD7;
_tmp14F=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp14E)->f1;_LLD6: Cyc_Tcutil_occurs(
evar,r,env,_tmp14F);goto _LLD4;_LLD7:;_LLD8: goto _LLD4;_LLD4:;}goto _LLAF;_LLB6: if(*((
int*)_tmp127)!= 7)goto _LLB8;_tmp12D=((struct Cyc_Absyn_ArrayType_struct*)_tmp127)->f1;
_tmp12E=(void*)_tmp12D.elt_type;_LLB7: Cyc_Tcutil_occurs(evar,r,env,_tmp12E);goto
_LLAF;_LLB8: if(*((int*)_tmp127)!= 8)goto _LLBA;_tmp12F=((struct Cyc_Absyn_FnType_struct*)
_tmp127)->f1;_tmp130=_tmp12F.tvars;_tmp131=_tmp12F.effect;_tmp132=(void*)_tmp12F.ret_typ;
_tmp133=_tmp12F.args;_tmp134=_tmp12F.c_varargs;_tmp135=_tmp12F.cyc_varargs;
_tmp136=_tmp12F.rgn_po;_tmp137=_tmp12F.attributes;_LLB9: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp130,env);if(_tmp131 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp131->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp132);for(0;_tmp133 != 0;_tmp133=_tmp133->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple2*)_tmp133->hd)).f3);}if(_tmp135 != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp135->type);for(0;_tmp136 != 0;_tmp136=_tmp136->tl){struct
_tuple6 _tmp151;void*_tmp152;void*_tmp153;struct _tuple6*_tmp150=(struct _tuple6*)
_tmp136->hd;_tmp151=*_tmp150;_tmp152=_tmp151.f1;_tmp153=_tmp151.f2;Cyc_Tcutil_occurs(
evar,r,env,_tmp152);Cyc_Tcutil_occurs(evar,r,env,_tmp153);}goto _LLAF;_LLBA: if(*((
int*)_tmp127)!= 9)goto _LLBC;_tmp138=((struct Cyc_Absyn_TupleType_struct*)_tmp127)->f1;
_LLBB: for(0;_tmp138 != 0;_tmp138=_tmp138->tl){Cyc_Tcutil_occurs(evar,r,env,(*((
struct _tuple4*)_tmp138->hd)).f2);}goto _LLAF;_LLBC: if(*((int*)_tmp127)!= 2)goto
_LLBE;_tmp139=((struct Cyc_Absyn_TunionType_struct*)_tmp127)->f1;_tmp13A=_tmp139.targs;
_tmp13B=_tmp139.rgn;_LLBD: if((unsigned int)_tmp13B)Cyc_Tcutil_occurs(evar,r,env,(
void*)_tmp13B->v);Cyc_Tcutil_occurslist(evar,r,env,_tmp13A);goto _LLAF;_LLBE: if(*((
int*)_tmp127)!= 16)goto _LLC0;_tmp13C=((struct Cyc_Absyn_TypedefType_struct*)
_tmp127)->f2;_LLBF: _tmp13E=_tmp13C;goto _LLC1;_LLC0: if(*((int*)_tmp127)!= 3)goto
_LLC2;_tmp13D=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp127)->f1;_tmp13E=
_tmp13D.targs;_LLC1: _tmp140=_tmp13E;goto _LLC3;_LLC2: if(*((int*)_tmp127)!= 10)
goto _LLC4;_tmp13F=((struct Cyc_Absyn_AggrType_struct*)_tmp127)->f1;_tmp140=
_tmp13F.targs;_LLC3: Cyc_Tcutil_occurslist(evar,r,env,_tmp140);goto _LLAF;_LLC4:
if(*((int*)_tmp127)!= 11)goto _LLC6;_tmp141=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp127)->f2;_LLC5: for(0;_tmp141 != 0;_tmp141=_tmp141->tl){Cyc_Tcutil_occurs(evar,
r,env,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp141->hd)->type);}goto _LLAF;_LLC6:
if(*((int*)_tmp127)!= 17)goto _LLC8;_tmp142=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp127)->f1;_LLC7: _tmp143=_tmp142;goto _LLC9;_LLC8: if(*((int*)_tmp127)!= 14)goto
_LLCA;_tmp143=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp127)->f1;_LLC9:
_tmp144=_tmp143;goto _LLCB;_LLCA: if(*((int*)_tmp127)!= 19)goto _LLCC;_tmp144=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp127)->f1;_LLCB: _tmp145=_tmp144;goto _LLCD;
_LLCC: if(*((int*)_tmp127)!= 15)goto _LLCE;_tmp145=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp127)->f1;_LLCD: _tmp146=_tmp145;goto _LLCF;_LLCE: if(*((int*)_tmp127)!= 21)goto
_LLD0;_tmp146=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp127)->f1;_LLCF: Cyc_Tcutil_occurs(
evar,r,env,_tmp146);goto _LLAF;_LLD0: if(*((int*)_tmp127)!= 20)goto _LLD2;_tmp147=((
struct Cyc_Absyn_JoinEff_struct*)_tmp127)->f1;_LLD1: Cyc_Tcutil_occurslist(evar,r,
env,_tmp147);goto _LLAF;_LLD2:;_LLD3: goto _LLAF;_LLAF:;}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}static void
Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(0;t1 != 
0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)(int)_throw((void*)Cyc_Tcutil_Unify);}static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const
 && !tq1.real_const)({void*_tmp154[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp155="tq1 real_const not set.";
_tag_dynforward(_tmp155,sizeof(char),_get_zero_arr_size(_tmp155,24));}),
_tag_dynforward(_tmp154,sizeof(void*),0));});if(tq2.print_const  && !tq2.real_const)({
void*_tmp156[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp157="tq2 real_const not set.";_tag_dynforward(
_tmp157,sizeof(char),_get_zero_arr_size(_tmp157,24));}),_tag_dynforward(_tmp156,
sizeof(void*),0));});if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile)
 || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=tq2.real_const;Cyc_Tcutil_failure_reason=({
const char*_tmp158="(qualifiers don't match)";_tag_dynforward(_tmp158,sizeof(char),
_get_zero_arr_size(_tmp158,25));});(int)_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_tq1_const=
0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_dynforward_ptr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(
y);if(x == y)return;{void*_tmp159=(void*)x->v;void*_tmp15A;_LLDA: if((int)_tmp159
!= 0)goto _LLDC;_LLDB:(void*)(x->v=(void*)((void*)({struct Cyc_Absyn_Forward_constr_struct*
_tmp15B=_cycalloc(sizeof(*_tmp15B));_tmp15B[0]=({struct Cyc_Absyn_Forward_constr_struct
_tmp15C;_tmp15C.tag=1;_tmp15C.f1=y;_tmp15C;});_tmp15B;})));return;_LLDC: if(
_tmp159 <= (void*)1)goto _LLDE;if(*((int*)_tmp159)!= 0)goto _LLDE;_tmp15A=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp159)->f1;_LLDD: {void*_tmp15D=(void*)y->v;
void*_tmp15E;_LLE1: if((int)_tmp15D != 0)goto _LLE3;_LLE2:(void*)(y->v=(void*)((
void*)x->v));return;_LLE3: if(_tmp15D <= (void*)1)goto _LLE5;if(*((int*)_tmp15D)!= 
0)goto _LLE5;_tmp15E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp15D)->f1;
_LLE4: if(cmp(_tmp15A,_tmp15E)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((
void*)Cyc_Tcutil_Unify);}return;_LLE5: if(_tmp15D <= (void*)1)goto _LLE0;if(*((int*)
_tmp15D)!= 1)goto _LLE0;_LLE6:({void*_tmp15F[0]={};Cyc_Tcutil_impos(({const char*
_tmp160="unify_conref: forward after compress(2)";_tag_dynforward(_tmp160,
sizeof(char),_get_zero_arr_size(_tmp160,40));}),_tag_dynforward(_tmp15F,sizeof(
void*),0));});_LLE0:;}_LLDE: if(_tmp159 <= (void*)1)goto _LLD9;if(*((int*)_tmp159)
!= 1)goto _LLD9;_LLDF:({void*_tmp161[0]={};Cyc_Tcutil_impos(({const char*_tmp162="unify_conref: forward after compress";
_tag_dynforward(_tmp162,sizeof(char),_get_zero_arr_size(_tmp162,37));}),
_tag_dynforward(_tmp161,sizeof(void*),0));});_LLD9:;}}static int Cyc_Tcutil_unify_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct
_handler_cons _tmp163;_push_handler(& _tmp163);{int _tmp165=0;if(setjmp(_tmp163.handler))
_tmp165=1;if(!_tmp165){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct
_dynforward_ptr)_tag_dynforward(0,0,0));{int _tmp166=1;_npop_handler(0);return
_tmp166;};_pop_handler();}else{void*_tmp164=(void*)_exn_thrown;void*_tmp168=
_tmp164;_LLE8: if(_tmp168 != Cyc_Tcutil_Unify)goto _LLEA;_LLE9: return 0;_LLEA:;_LLEB:(
void)_throw(_tmp168);_LLE7:;}}}static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple6 _tmp16A=({struct _tuple6 _tmp169;_tmp169.f1=b1;_tmp169.f2=b2;_tmp169;});
void*_tmp16B;void*_tmp16C;void*_tmp16D;void*_tmp16E;void*_tmp16F;void*_tmp170;
void*_tmp171;void*_tmp172;void*_tmp173;struct Cyc_Absyn_Exp*_tmp174;void*_tmp175;
struct Cyc_Absyn_Exp*_tmp176;void*_tmp177;void*_tmp178;void*_tmp179;void*_tmp17A;
void*_tmp17B;void*_tmp17C;void*_tmp17D;void*_tmp17E;_LLED: _tmp16B=_tmp16A.f1;if((
int)_tmp16B != 0)goto _LLEF;_tmp16C=_tmp16A.f2;if((int)_tmp16C != 0)goto _LLEF;_LLEE:
return 0;_LLEF: _tmp16D=_tmp16A.f1;if((int)_tmp16D != 0)goto _LLF1;_LLF0: return - 1;
_LLF1: _tmp16E=_tmp16A.f2;if((int)_tmp16E != 0)goto _LLF3;_LLF2: return 1;_LLF3:
_tmp16F=_tmp16A.f1;if((int)_tmp16F != 1)goto _LLF5;_tmp170=_tmp16A.f2;if((int)
_tmp170 != 1)goto _LLF5;_LLF4: return 0;_LLF5: _tmp171=_tmp16A.f1;if((int)_tmp171 != 1)
goto _LLF7;_LLF6: return - 1;_LLF7: _tmp172=_tmp16A.f2;if((int)_tmp172 != 1)goto _LLF9;
_LLF8: return 1;_LLF9: _tmp173=_tmp16A.f1;if(_tmp173 <= (void*)2)goto _LLFB;if(*((int*)
_tmp173)!= 0)goto _LLFB;_tmp174=((struct Cyc_Absyn_Upper_b_struct*)_tmp173)->f1;
_tmp175=_tmp16A.f2;if(_tmp175 <= (void*)2)goto _LLFB;if(*((int*)_tmp175)!= 0)goto
_LLFB;_tmp176=((struct Cyc_Absyn_Upper_b_struct*)_tmp175)->f1;_LLFA: return Cyc_Evexp_const_exp_cmp(
_tmp174,_tmp176);_LLFB: _tmp177=_tmp16A.f1;if(_tmp177 <= (void*)2)goto _LLFD;if(*((
int*)_tmp177)!= 0)goto _LLFD;_tmp178=_tmp16A.f2;if(_tmp178 <= (void*)2)goto _LLFD;
if(*((int*)_tmp178)!= 1)goto _LLFD;_LLFC: return - 1;_LLFD: _tmp179=_tmp16A.f1;if(
_tmp179 <= (void*)2)goto _LLFF;if(*((int*)_tmp179)!= 1)goto _LLFF;_tmp17A=_tmp16A.f2;
if(_tmp17A <= (void*)2)goto _LLFF;if(*((int*)_tmp17A)!= 0)goto _LLFF;_LLFE: return 1;
_LLFF: _tmp17B=_tmp16A.f1;if(_tmp17B <= (void*)2)goto _LLEC;if(*((int*)_tmp17B)!= 1)
goto _LLEC;_tmp17C=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp17B)->f1;
_tmp17D=_tmp16A.f2;if(_tmp17D <= (void*)2)goto _LLEC;if(*((int*)_tmp17D)!= 1)goto
_LLEC;_tmp17E=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp17D)->f1;_LL100:
return Cyc_Tcutil_typecmp(_tmp17C,_tmp17E);_LLEC:;}static int Cyc_Tcutil_unify_it_bounds(
void*b1,void*b2){struct _tuple6 _tmp180=({struct _tuple6 _tmp17F;_tmp17F.f1=b1;
_tmp17F.f2=b2;_tmp17F;});void*_tmp181;void*_tmp182;void*_tmp183;void*_tmp184;
void*_tmp185;void*_tmp186;void*_tmp187;void*_tmp188;void*_tmp189;struct Cyc_Absyn_Exp*
_tmp18A;void*_tmp18B;struct Cyc_Absyn_Exp*_tmp18C;void*_tmp18D;struct Cyc_Absyn_Exp*
_tmp18E;void*_tmp18F;void*_tmp190;void*_tmp191;void*_tmp192;void*_tmp193;struct
Cyc_Absyn_Exp*_tmp194;void*_tmp195;void*_tmp196;void*_tmp197;void*_tmp198;_LL102:
_tmp181=_tmp180.f1;if((int)_tmp181 != 0)goto _LL104;_tmp182=_tmp180.f2;if((int)
_tmp182 != 0)goto _LL104;_LL103: return 0;_LL104: _tmp183=_tmp180.f1;if((int)_tmp183
!= 0)goto _LL106;_LL105: return - 1;_LL106: _tmp184=_tmp180.f2;if((int)_tmp184 != 0)
goto _LL108;_LL107: return 1;_LL108: _tmp185=_tmp180.f1;if((int)_tmp185 != 1)goto
_LL10A;_tmp186=_tmp180.f2;if((int)_tmp186 != 1)goto _LL10A;_LL109: return 0;_LL10A:
_tmp187=_tmp180.f1;if((int)_tmp187 != 1)goto _LL10C;_LL10B: return - 1;_LL10C: _tmp188=
_tmp180.f2;if((int)_tmp188 != 1)goto _LL10E;_LL10D: return 1;_LL10E: _tmp189=_tmp180.f1;
if(_tmp189 <= (void*)2)goto _LL110;if(*((int*)_tmp189)!= 0)goto _LL110;_tmp18A=((
struct Cyc_Absyn_Upper_b_struct*)_tmp189)->f1;_tmp18B=_tmp180.f2;if(_tmp18B <= (
void*)2)goto _LL110;if(*((int*)_tmp18B)!= 0)goto _LL110;_tmp18C=((struct Cyc_Absyn_Upper_b_struct*)
_tmp18B)->f1;_LL10F: return Cyc_Evexp_const_exp_cmp(_tmp18A,_tmp18C);_LL110:
_tmp18D=_tmp180.f1;if(_tmp18D <= (void*)2)goto _LL112;if(*((int*)_tmp18D)!= 0)goto
_LL112;_tmp18E=((struct Cyc_Absyn_Upper_b_struct*)_tmp18D)->f1;_tmp18F=_tmp180.f2;
if(_tmp18F <= (void*)2)goto _LL112;if(*((int*)_tmp18F)!= 1)goto _LL112;_tmp190=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp18F)->f1;_LL111: _tmp192=_tmp190;
_tmp194=_tmp18E;goto _LL113;_LL112: _tmp191=_tmp180.f1;if(_tmp191 <= (void*)2)goto
_LL114;if(*((int*)_tmp191)!= 1)goto _LL114;_tmp192=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp191)->f1;_tmp193=_tmp180.f2;if(_tmp193 <= (void*)2)goto _LL114;if(*((int*)
_tmp193)!= 0)goto _LL114;_tmp194=((struct Cyc_Absyn_Upper_b_struct*)_tmp193)->f1;
_LL113: {unsigned int _tmp19A;int _tmp19B;struct _tuple7 _tmp199=Cyc_Evexp_eval_const_uint_exp(
_tmp194);_tmp19A=_tmp199.f1;_tmp19B=_tmp199.f2;if(!_tmp19B)return 1;_tmp196=
_tmp192;_tmp198=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp19C=_cycalloc_atomic(
sizeof(*_tmp19C));_tmp19C[0]=({struct Cyc_Absyn_TypeInt_struct _tmp19D;_tmp19D.tag=
18;_tmp19D.f1=(int)_tmp19A;_tmp19D;});_tmp19C;});goto _LL115;}_LL114: _tmp195=
_tmp180.f1;if(_tmp195 <= (void*)2)goto _LL101;if(*((int*)_tmp195)!= 1)goto _LL101;
_tmp196=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp195)->f1;_tmp197=_tmp180.f2;
if(_tmp197 <= (void*)2)goto _LL101;if(*((int*)_tmp197)!= 1)goto _LL101;_tmp198=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp197)->f1;_LL115: Cyc_Tcutil_unify_it(
_tmp196,_tmp198);return 0;_LL101:;}static int Cyc_Tcutil_attribute_case_number(void*
att){void*_tmp19E=att;_LL117: if(_tmp19E <= (void*)17)goto _LL119;if(*((int*)
_tmp19E)!= 0)goto _LL119;_LL118: return 0;_LL119: if((int)_tmp19E != 0)goto _LL11B;
_LL11A: return 1;_LL11B: if((int)_tmp19E != 1)goto _LL11D;_LL11C: return 2;_LL11D: if((
int)_tmp19E != 2)goto _LL11F;_LL11E: return 3;_LL11F: if((int)_tmp19E != 3)goto _LL121;
_LL120: return 4;_LL121: if((int)_tmp19E != 4)goto _LL123;_LL122: return 5;_LL123: if(
_tmp19E <= (void*)17)goto _LL125;if(*((int*)_tmp19E)!= 1)goto _LL125;_LL124: return 6;
_LL125: if((int)_tmp19E != 5)goto _LL127;_LL126: return 7;_LL127: if(_tmp19E <= (void*)
17)goto _LL129;if(*((int*)_tmp19E)!= 2)goto _LL129;_LL128: return 8;_LL129: if((int)
_tmp19E != 6)goto _LL12B;_LL12A: return 9;_LL12B: if((int)_tmp19E != 7)goto _LL12D;
_LL12C: return 10;_LL12D: if((int)_tmp19E != 8)goto _LL12F;_LL12E: return 11;_LL12F: if((
int)_tmp19E != 9)goto _LL131;_LL130: return 12;_LL131: if((int)_tmp19E != 10)goto
_LL133;_LL132: return 13;_LL133: if((int)_tmp19E != 11)goto _LL135;_LL134: return 14;
_LL135: if((int)_tmp19E != 12)goto _LL137;_LL136: return 15;_LL137: if((int)_tmp19E != 
13)goto _LL139;_LL138: return 16;_LL139: if((int)_tmp19E != 14)goto _LL13B;_LL13A:
return 17;_LL13B: if((int)_tmp19E != 15)goto _LL13D;_LL13C: return 18;_LL13D: if(
_tmp19E <= (void*)17)goto _LL141;if(*((int*)_tmp19E)!= 3)goto _LL13F;_LL13E: return
19;_LL13F: if(*((int*)_tmp19E)!= 4)goto _LL141;_LL140: return 20;_LL141:;_LL142:
return 21;_LL116:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){struct
_tuple6 _tmp1A0=({struct _tuple6 _tmp19F;_tmp19F.f1=att1;_tmp19F.f2=att2;_tmp19F;});
void*_tmp1A1;int _tmp1A2;void*_tmp1A3;int _tmp1A4;void*_tmp1A5;int _tmp1A6;void*
_tmp1A7;int _tmp1A8;void*_tmp1A9;int _tmp1AA;void*_tmp1AB;int _tmp1AC;void*_tmp1AD;
struct _dynforward_ptr _tmp1AE;void*_tmp1AF;struct _dynforward_ptr _tmp1B0;void*
_tmp1B1;void*_tmp1B2;int _tmp1B3;int _tmp1B4;void*_tmp1B5;void*_tmp1B6;int _tmp1B7;
int _tmp1B8;_LL144: _tmp1A1=_tmp1A0.f1;if(_tmp1A1 <= (void*)17)goto _LL146;if(*((int*)
_tmp1A1)!= 0)goto _LL146;_tmp1A2=((struct Cyc_Absyn_Regparm_att_struct*)_tmp1A1)->f1;
_tmp1A3=_tmp1A0.f2;if(_tmp1A3 <= (void*)17)goto _LL146;if(*((int*)_tmp1A3)!= 0)
goto _LL146;_tmp1A4=((struct Cyc_Absyn_Regparm_att_struct*)_tmp1A3)->f1;_LL145:
_tmp1A6=_tmp1A2;_tmp1A8=_tmp1A4;goto _LL147;_LL146: _tmp1A5=_tmp1A0.f1;if(_tmp1A5
<= (void*)17)goto _LL148;if(*((int*)_tmp1A5)!= 4)goto _LL148;_tmp1A6=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1A5)->f1;_tmp1A7=_tmp1A0.f2;if(_tmp1A7 <= (void*)17)goto _LL148;if(*((int*)
_tmp1A7)!= 4)goto _LL148;_tmp1A8=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1A7)->f1;_LL147: _tmp1AA=_tmp1A6;_tmp1AC=_tmp1A8;goto _LL149;_LL148: _tmp1A9=
_tmp1A0.f1;if(_tmp1A9 <= (void*)17)goto _LL14A;if(*((int*)_tmp1A9)!= 1)goto _LL14A;
_tmp1AA=((struct Cyc_Absyn_Aligned_att_struct*)_tmp1A9)->f1;_tmp1AB=_tmp1A0.f2;
if(_tmp1AB <= (void*)17)goto _LL14A;if(*((int*)_tmp1AB)!= 1)goto _LL14A;_tmp1AC=((
struct Cyc_Absyn_Aligned_att_struct*)_tmp1AB)->f1;_LL149: return Cyc_Core_intcmp(
_tmp1AA,_tmp1AC);_LL14A: _tmp1AD=_tmp1A0.f1;if(_tmp1AD <= (void*)17)goto _LL14C;if(*((
int*)_tmp1AD)!= 2)goto _LL14C;_tmp1AE=((struct Cyc_Absyn_Section_att_struct*)
_tmp1AD)->f1;_tmp1AF=_tmp1A0.f2;if(_tmp1AF <= (void*)17)goto _LL14C;if(*((int*)
_tmp1AF)!= 2)goto _LL14C;_tmp1B0=((struct Cyc_Absyn_Section_att_struct*)_tmp1AF)->f1;
_LL14B: return Cyc_strcmp((struct _dynforward_ptr)_tmp1AE,(struct _dynforward_ptr)
_tmp1B0);_LL14C: _tmp1B1=_tmp1A0.f1;if(_tmp1B1 <= (void*)17)goto _LL14E;if(*((int*)
_tmp1B1)!= 3)goto _LL14E;_tmp1B2=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp1B1)->f1;_tmp1B3=((struct Cyc_Absyn_Format_att_struct*)_tmp1B1)->f2;_tmp1B4=((
struct Cyc_Absyn_Format_att_struct*)_tmp1B1)->f3;_tmp1B5=_tmp1A0.f2;if(_tmp1B5 <= (
void*)17)goto _LL14E;if(*((int*)_tmp1B5)!= 3)goto _LL14E;_tmp1B6=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp1B5)->f1;_tmp1B7=((struct Cyc_Absyn_Format_att_struct*)
_tmp1B5)->f2;_tmp1B8=((struct Cyc_Absyn_Format_att_struct*)_tmp1B5)->f3;_LL14D: {
int _tmp1B9=Cyc_Core_intcmp((int)((unsigned int)_tmp1B2),(int)((unsigned int)
_tmp1B6));if(_tmp1B9 != 0)return _tmp1B9;{int _tmp1BA=Cyc_Core_intcmp(_tmp1B3,
_tmp1B7);if(_tmp1BA != 0)return _tmp1BA;return Cyc_Core_intcmp(_tmp1B4,_tmp1B8);}}
_LL14E:;_LL14F: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(
att2));_LL143:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(
a1,a2)== 0;}int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*
a2){{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a2))return 0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(
Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(
void*t);static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return
Cyc_Tcutil_rgns_of((void*)af->type);}static struct _tuple8*Cyc_Tcutil_region_free_subst(
struct Cyc_Absyn_Tvar*tv){void*t;{void*_tmp1BB=Cyc_Tcutil_tvar_kind(tv);_LL151:
if((int)_tmp1BB != 3)goto _LL153;_LL152: t=(void*)2;goto _LL150;_LL153: if((int)
_tmp1BB != 4)goto _LL155;_LL154: t=Cyc_Absyn_empty_effect;goto _LL150;_LL155: if((int)
_tmp1BB != 5)goto _LL157;_LL156: t=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp1BC=
_cycalloc_atomic(sizeof(*_tmp1BC));_tmp1BC[0]=({struct Cyc_Absyn_TypeInt_struct
_tmp1BD;_tmp1BD.tag=18;_tmp1BD.f1=0;_tmp1BD;});_tmp1BC;});goto _LL150;_LL157:;
_LL158: t=Cyc_Absyn_sint_typ;goto _LL150;_LL150:;}return({struct _tuple8*_tmp1BE=
_cycalloc(sizeof(*_tmp1BE));_tmp1BE->f1=tv;_tmp1BE->f2=t;_tmp1BE;});}static void*
Cyc_Tcutil_rgns_of(void*t){void*_tmp1BF=Cyc_Tcutil_compress(t);void*_tmp1C0;
struct Cyc_Absyn_TunionInfo _tmp1C1;struct Cyc_List_List*_tmp1C2;struct Cyc_Core_Opt*
_tmp1C3;struct Cyc_Absyn_PtrInfo _tmp1C4;void*_tmp1C5;struct Cyc_Absyn_PtrAtts
_tmp1C6;void*_tmp1C7;struct Cyc_Absyn_ArrayInfo _tmp1C8;void*_tmp1C9;struct Cyc_List_List*
_tmp1CA;struct Cyc_Absyn_TunionFieldInfo _tmp1CB;struct Cyc_List_List*_tmp1CC;
struct Cyc_Absyn_AggrInfo _tmp1CD;struct Cyc_List_List*_tmp1CE;struct Cyc_List_List*
_tmp1CF;void*_tmp1D0;struct Cyc_Absyn_FnInfo _tmp1D1;struct Cyc_List_List*_tmp1D2;
struct Cyc_Core_Opt*_tmp1D3;void*_tmp1D4;struct Cyc_List_List*_tmp1D5;struct Cyc_Absyn_VarargInfo*
_tmp1D6;struct Cyc_List_List*_tmp1D7;void*_tmp1D8;struct Cyc_List_List*_tmp1D9;
_LL15A: if((int)_tmp1BF != 0)goto _LL15C;_LL15B: goto _LL15D;_LL15C: if((int)_tmp1BF != 
1)goto _LL15E;_LL15D: goto _LL15F;_LL15E: if(_tmp1BF <= (void*)4)goto _LL182;if(*((int*)
_tmp1BF)!= 6)goto _LL160;_LL15F: goto _LL161;_LL160: if(*((int*)_tmp1BF)!= 12)goto
_LL162;_LL161: goto _LL163;_LL162: if(*((int*)_tmp1BF)!= 13)goto _LL164;_LL163: goto
_LL165;_LL164: if(*((int*)_tmp1BF)!= 18)goto _LL166;_LL165: goto _LL167;_LL166: if(*((
int*)_tmp1BF)!= 5)goto _LL168;_LL167: return Cyc_Absyn_empty_effect;_LL168: if(*((
int*)_tmp1BF)!= 0)goto _LL16A;_LL169: goto _LL16B;_LL16A: if(*((int*)_tmp1BF)!= 1)
goto _LL16C;_LL16B: {void*_tmp1DA=Cyc_Tcutil_typ_kind(t);_LL18F: if((int)_tmp1DA != 
3)goto _LL191;_LL190: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1DB=
_cycalloc(sizeof(*_tmp1DB));_tmp1DB[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1DC;
_tmp1DC.tag=19;_tmp1DC.f1=(void*)t;_tmp1DC;});_tmp1DB;});_LL191: if((int)_tmp1DA
!= 4)goto _LL193;_LL192: return t;_LL193: if((int)_tmp1DA != 5)goto _LL195;_LL194:
return Cyc_Absyn_empty_effect;_LL195:;_LL196: return(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp1DE;_tmp1DE.tag=21;_tmp1DE.f1=(void*)t;_tmp1DE;});_tmp1DD;});_LL18E:;}_LL16C:
if(*((int*)_tmp1BF)!= 15)goto _LL16E;_tmp1C0=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp1BF)->f1;_LL16D: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1DF=
_cycalloc(sizeof(*_tmp1DF));_tmp1DF[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1E0;
_tmp1E0.tag=19;_tmp1E0.f1=(void*)_tmp1C0;_tmp1E0;});_tmp1DF;});_LL16E: if(*((int*)
_tmp1BF)!= 2)goto _LL170;_tmp1C1=((struct Cyc_Absyn_TunionType_struct*)_tmp1BF)->f1;
_tmp1C2=_tmp1C1.targs;_tmp1C3=_tmp1C1.rgn;_LL16F: {struct Cyc_List_List*ts=Cyc_List_map(
Cyc_Tcutil_rgns_of,_tmp1C2);if((unsigned int)_tmp1C3)ts=({struct Cyc_List_List*
_tmp1E1=_cycalloc(sizeof(*_tmp1E1));_tmp1E1->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1E2=_cycalloc(sizeof(*_tmp1E2));_tmp1E2[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1E3;_tmp1E3.tag=19;_tmp1E3.f1=(void*)((void*)_tmp1C3->v);_tmp1E3;});_tmp1E2;}));
_tmp1E1->tl=ts;_tmp1E1;});return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1E4=_cycalloc(sizeof(*_tmp1E4));_tmp1E4[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1E5;_tmp1E5.tag=20;_tmp1E5.f1=ts;_tmp1E5;});_tmp1E4;}));}_LL170: if(*((int*)
_tmp1BF)!= 4)goto _LL172;_tmp1C4=((struct Cyc_Absyn_PointerType_struct*)_tmp1BF)->f1;
_tmp1C5=(void*)_tmp1C4.elt_typ;_tmp1C6=_tmp1C4.ptr_atts;_tmp1C7=(void*)_tmp1C6.rgn;
_LL171: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1E6=_cycalloc(sizeof(*_tmp1E6));_tmp1E6[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1E7;_tmp1E7.tag=20;_tmp1E7.f1=({void*_tmp1E8[2];_tmp1E8[1]=Cyc_Tcutil_rgns_of(
_tmp1C5);_tmp1E8[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1E9=_cycalloc(
sizeof(*_tmp1E9));_tmp1E9[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1EA;_tmp1EA.tag=
19;_tmp1EA.f1=(void*)_tmp1C7;_tmp1EA;});_tmp1E9;});Cyc_List_list(_tag_dynforward(
_tmp1E8,sizeof(void*),2));});_tmp1E7;});_tmp1E6;}));_LL172: if(*((int*)_tmp1BF)!= 
7)goto _LL174;_tmp1C8=((struct Cyc_Absyn_ArrayType_struct*)_tmp1BF)->f1;_tmp1C9=(
void*)_tmp1C8.elt_type;_LL173: return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(
_tmp1C9));_LL174: if(*((int*)_tmp1BF)!= 9)goto _LL176;_tmp1CA=((struct Cyc_Absyn_TupleType_struct*)
_tmp1BF)->f1;_LL175: {struct Cyc_List_List*_tmp1EB=0;for(0;_tmp1CA != 0;_tmp1CA=
_tmp1CA->tl){_tmp1EB=({struct Cyc_List_List*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));
_tmp1EC->hd=(void*)(*((struct _tuple4*)_tmp1CA->hd)).f2;_tmp1EC->tl=_tmp1EB;
_tmp1EC;});}_tmp1CC=_tmp1EB;goto _LL177;}_LL176: if(*((int*)_tmp1BF)!= 3)goto
_LL178;_tmp1CB=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1BF)->f1;_tmp1CC=
_tmp1CB.targs;_LL177: _tmp1CE=_tmp1CC;goto _LL179;_LL178: if(*((int*)_tmp1BF)!= 10)
goto _LL17A;_tmp1CD=((struct Cyc_Absyn_AggrType_struct*)_tmp1BF)->f1;_tmp1CE=
_tmp1CD.targs;_LL179: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1EE;_tmp1EE.tag=20;_tmp1EE.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1CE);
_tmp1EE;});_tmp1ED;}));_LL17A: if(*((int*)_tmp1BF)!= 11)goto _LL17C;_tmp1CF=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp1BF)->f2;_LL17B: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));
_tmp1EF[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1F0;_tmp1F0.tag=20;_tmp1F0.f1=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1CF);_tmp1F0;});_tmp1EF;}));_LL17C:
if(*((int*)_tmp1BF)!= 14)goto _LL17E;_tmp1D0=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp1BF)->f1;_LL17D: return Cyc_Tcutil_rgns_of(_tmp1D0);_LL17E: if(*((int*)_tmp1BF)
!= 17)goto _LL180;_LL17F: return Cyc_Tcutil_rgns_of(t);_LL180: if(*((int*)_tmp1BF)!= 
8)goto _LL182;_tmp1D1=((struct Cyc_Absyn_FnType_struct*)_tmp1BF)->f1;_tmp1D2=
_tmp1D1.tvars;_tmp1D3=_tmp1D1.effect;_tmp1D4=(void*)_tmp1D1.ret_typ;_tmp1D5=
_tmp1D1.args;_tmp1D6=_tmp1D1.cyc_varargs;_tmp1D7=_tmp1D1.rgn_po;_LL181: {void*
_tmp1F1=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp1D2),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1D3))->v);return Cyc_Tcutil_normalize_effect(
_tmp1F1);}_LL182: if((int)_tmp1BF != 3)goto _LL184;_LL183: goto _LL185;_LL184: if((int)
_tmp1BF != 2)goto _LL186;_LL185: return Cyc_Absyn_empty_effect;_LL186: if(_tmp1BF <= (
void*)4)goto _LL188;if(*((int*)_tmp1BF)!= 19)goto _LL188;_LL187: goto _LL189;_LL188:
if(_tmp1BF <= (void*)4)goto _LL18A;if(*((int*)_tmp1BF)!= 20)goto _LL18A;_LL189:
return t;_LL18A: if(_tmp1BF <= (void*)4)goto _LL18C;if(*((int*)_tmp1BF)!= 21)goto
_LL18C;_tmp1D8=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1BF)->f1;_LL18B:
return Cyc_Tcutil_rgns_of(_tmp1D8);_LL18C: if(_tmp1BF <= (void*)4)goto _LL159;if(*((
int*)_tmp1BF)!= 16)goto _LL159;_tmp1D9=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1BF)->f2;_LL18D: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1F2=_cycalloc(sizeof(*_tmp1F2));_tmp1F2[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1F3;_tmp1F3.tag=20;_tmp1F3.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1D9);
_tmp1F3;});_tmp1F2;}));_LL159:;}void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp1F4=e;struct Cyc_List_List*_tmp1F5;struct Cyc_List_List**_tmp1F6;void*
_tmp1F7;_LL198: if(_tmp1F4 <= (void*)4)goto _LL19C;if(*((int*)_tmp1F4)!= 20)goto
_LL19A;_tmp1F5=((struct Cyc_Absyn_JoinEff_struct*)_tmp1F4)->f1;_tmp1F6=(struct Cyc_List_List**)&((
struct Cyc_Absyn_JoinEff_struct*)_tmp1F4)->f1;_LL199: {int redo_join=0;{struct Cyc_List_List*
effs=*_tmp1F6;for(0;effs != 0;effs=effs->tl){void*_tmp1F8=(void*)effs->hd;(void*)(
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp1F8)));{void*
_tmp1F9=(void*)effs->hd;void*_tmp1FA;_LL19F: if(_tmp1F9 <= (void*)4)goto _LL1A3;if(*((
int*)_tmp1F9)!= 20)goto _LL1A1;_LL1A0: goto _LL1A2;_LL1A1: if(*((int*)_tmp1F9)!= 19)
goto _LL1A3;_tmp1FA=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1F9)->f1;if((
int)_tmp1FA != 2)goto _LL1A3;_LL1A2: redo_join=1;goto _LL19E;_LL1A3:;_LL1A4: goto
_LL19E;_LL19E:;}}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct
Cyc_List_List*effs=*_tmp1F6;for(0;effs != 0;effs=effs->tl){void*_tmp1FB=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp1FC;void*_tmp1FD;_LL1A6: if(_tmp1FB <= (
void*)4)goto _LL1AA;if(*((int*)_tmp1FB)!= 20)goto _LL1A8;_tmp1FC=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1FB)->f1;_LL1A7: effects=Cyc_List_revappend(_tmp1FC,effects);goto _LL1A5;
_LL1A8: if(*((int*)_tmp1FB)!= 19)goto _LL1AA;_tmp1FD=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1FB)->f1;if((int)_tmp1FD != 2)goto _LL1AA;_LL1A9: goto _LL1A5;_LL1AA:;_LL1AB:
effects=({struct Cyc_List_List*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));_tmp1FE->hd=(
void*)_tmp1FB;_tmp1FE->tl=effects;_tmp1FE;});goto _LL1A5;_LL1A5:;}}*_tmp1F6=Cyc_List_imp_rev(
effects);return e;}}_LL19A: if(*((int*)_tmp1F4)!= 21)goto _LL19C;_tmp1F7=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1F4)->f1;_LL19B: {void*_tmp1FF=Cyc_Tcutil_compress(
_tmp1F7);_LL1AD: if(_tmp1FF <= (void*)4)goto _LL1B1;if(*((int*)_tmp1FF)!= 0)goto
_LL1AF;_LL1AE: goto _LL1B0;_LL1AF: if(*((int*)_tmp1FF)!= 1)goto _LL1B1;_LL1B0: return
e;_LL1B1:;_LL1B2: return Cyc_Tcutil_rgns_of(_tmp1F7);_LL1AC:;}_LL19C:;_LL19D:
return e;_LL197:;}}static struct Cyc_Core_Opt Cyc_Tcutil_ek={(void*)((void*)4)};
static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct*
_tmp200=({struct Cyc_Absyn_FnType_struct*_tmp201=_cycalloc(sizeof(*_tmp201));
_tmp201[0]=({struct Cyc_Absyn_FnType_struct _tmp202;_tmp202.tag=8;_tmp202.f1=({
struct Cyc_Absyn_FnInfo _tmp203;_tmp203.tvars=0;_tmp203.effect=({struct Cyc_Core_Opt*
_tmp204=_cycalloc(sizeof(*_tmp204));_tmp204->v=(void*)eff;_tmp204;});_tmp203.ret_typ=(
void*)((void*)0);_tmp203.args=0;_tmp203.c_varargs=0;_tmp203.cyc_varargs=0;
_tmp203.rgn_po=0;_tmp203.attributes=0;_tmp203;});_tmp202;});_tmp201;});return Cyc_Absyn_atb_typ((
void*)_tmp200,(void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){r=Cyc_Tcutil_compress(
r);if(r == (void*)2)return 1;{void*_tmp205=Cyc_Tcutil_compress(e);void*_tmp206;
struct Cyc_List_List*_tmp207;void*_tmp208;struct Cyc_Core_Opt*_tmp209;struct Cyc_Core_Opt*
_tmp20A;struct Cyc_Core_Opt**_tmp20B;struct Cyc_Core_Opt*_tmp20C;_LL1B4: if(_tmp205
<= (void*)4)goto _LL1BC;if(*((int*)_tmp205)!= 19)goto _LL1B6;_tmp206=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp205)->f1;_LL1B5: if(constrain)return Cyc_Tcutil_unify(
r,_tmp206);_tmp206=Cyc_Tcutil_compress(_tmp206);if(r == _tmp206)return 1;{struct
_tuple6 _tmp20E=({struct _tuple6 _tmp20D;_tmp20D.f1=r;_tmp20D.f2=_tmp206;_tmp20D;});
void*_tmp20F;struct Cyc_Absyn_Tvar*_tmp210;void*_tmp211;struct Cyc_Absyn_Tvar*
_tmp212;_LL1BF: _tmp20F=_tmp20E.f1;if(_tmp20F <= (void*)4)goto _LL1C1;if(*((int*)
_tmp20F)!= 1)goto _LL1C1;_tmp210=((struct Cyc_Absyn_VarType_struct*)_tmp20F)->f1;
_tmp211=_tmp20E.f2;if(_tmp211 <= (void*)4)goto _LL1C1;if(*((int*)_tmp211)!= 1)goto
_LL1C1;_tmp212=((struct Cyc_Absyn_VarType_struct*)_tmp211)->f1;_LL1C0: return Cyc_Absyn_tvar_cmp(
_tmp210,_tmp212)== 0;_LL1C1:;_LL1C2: return 0;_LL1BE:;}_LL1B6: if(*((int*)_tmp205)
!= 20)goto _LL1B8;_tmp207=((struct Cyc_Absyn_JoinEff_struct*)_tmp205)->f1;_LL1B7:
for(0;_tmp207 != 0;_tmp207=_tmp207->tl){if(Cyc_Tcutil_region_in_effect(constrain,
r,(void*)_tmp207->hd))return 1;}return 0;_LL1B8: if(*((int*)_tmp205)!= 21)goto
_LL1BA;_tmp208=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp205)->f1;_LL1B9: {
void*_tmp213=Cyc_Tcutil_rgns_of(_tmp208);void*_tmp214;_LL1C4: if(_tmp213 <= (void*)
4)goto _LL1C6;if(*((int*)_tmp213)!= 21)goto _LL1C6;_tmp214=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp213)->f1;_LL1C5: if(!constrain)return 0;{void*_tmp215=Cyc_Tcutil_compress(
_tmp214);struct Cyc_Core_Opt*_tmp216;struct Cyc_Core_Opt*_tmp217;struct Cyc_Core_Opt**
_tmp218;struct Cyc_Core_Opt*_tmp219;_LL1C9: if(_tmp215 <= (void*)4)goto _LL1CB;if(*((
int*)_tmp215)!= 0)goto _LL1CB;_tmp216=((struct Cyc_Absyn_Evar_struct*)_tmp215)->f1;
_tmp217=((struct Cyc_Absyn_Evar_struct*)_tmp215)->f2;_tmp218=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp215)->f2;_tmp219=((struct Cyc_Absyn_Evar_struct*)
_tmp215)->f4;_LL1CA: {void*_tmp21A=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp219);Cyc_Tcutil_occurs(_tmp21A,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp219))->v,r);{void*_tmp21B=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp21D=_cycalloc(sizeof(*_tmp21D));
_tmp21D[0]=({struct Cyc_Absyn_JoinEff_struct _tmp21E;_tmp21E.tag=20;_tmp21E.f1=({
void*_tmp21F[2];_tmp21F[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp220=
_cycalloc(sizeof(*_tmp220));_tmp220[0]=({struct Cyc_Absyn_AccessEff_struct _tmp221;
_tmp221.tag=19;_tmp221.f1=(void*)r;_tmp221;});_tmp220;});_tmp21F[0]=_tmp21A;Cyc_List_list(
_tag_dynforward(_tmp21F,sizeof(void*),2));});_tmp21E;});_tmp21D;}));*_tmp218=({
struct Cyc_Core_Opt*_tmp21C=_cycalloc(sizeof(*_tmp21C));_tmp21C->v=(void*)_tmp21B;
_tmp21C;});return 1;}}_LL1CB:;_LL1CC: return 0;_LL1C8:;}_LL1C6:;_LL1C7: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp213);_LL1C3:;}_LL1BA: if(*((int*)_tmp205)!= 0)goto _LL1BC;_tmp209=((
struct Cyc_Absyn_Evar_struct*)_tmp205)->f1;_tmp20A=((struct Cyc_Absyn_Evar_struct*)
_tmp205)->f2;_tmp20B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp205)->f2;_tmp20C=((struct Cyc_Absyn_Evar_struct*)_tmp205)->f4;_LL1BB: if(
_tmp209 == 0  || (void*)_tmp209->v != (void*)4)({void*_tmp222[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp223="effect evar has wrong kind";_tag_dynforward(_tmp223,sizeof(char),
_get_zero_arr_size(_tmp223,27));}),_tag_dynforward(_tmp222,sizeof(void*),0));});
if(!constrain)return 0;{void*_tmp224=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp20C);Cyc_Tcutil_occurs(_tmp224,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp20C))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp225=({struct Cyc_Absyn_JoinEff_struct*_tmp227=_cycalloc(sizeof(*_tmp227));
_tmp227[0]=({struct Cyc_Absyn_JoinEff_struct _tmp228;_tmp228.tag=20;_tmp228.f1=({
struct Cyc_List_List*_tmp229=_cycalloc(sizeof(*_tmp229));_tmp229->hd=(void*)
_tmp224;_tmp229->tl=({struct Cyc_List_List*_tmp22A=_cycalloc(sizeof(*_tmp22A));
_tmp22A->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp22B=_cycalloc(
sizeof(*_tmp22B));_tmp22B[0]=({struct Cyc_Absyn_AccessEff_struct _tmp22C;_tmp22C.tag=
19;_tmp22C.f1=(void*)r;_tmp22C;});_tmp22B;}));_tmp22A->tl=0;_tmp22A;});_tmp229;});
_tmp228;});_tmp227;});*_tmp20B=({struct Cyc_Core_Opt*_tmp226=_cycalloc(sizeof(*
_tmp226));_tmp226->v=(void*)((void*)_tmp225);_tmp226;});return 1;}}_LL1BC:;_LL1BD:
return 0;_LL1B3:;}}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*
t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp22D=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(
e));struct Cyc_List_List*_tmp22E;void*_tmp22F;struct Cyc_Core_Opt*_tmp230;struct
Cyc_Core_Opt*_tmp231;struct Cyc_Core_Opt**_tmp232;struct Cyc_Core_Opt*_tmp233;
_LL1CE: if(_tmp22D <= (void*)4)goto _LL1D6;if(*((int*)_tmp22D)!= 19)goto _LL1D0;
_LL1CF: return 0;_LL1D0: if(*((int*)_tmp22D)!= 20)goto _LL1D2;_tmp22E=((struct Cyc_Absyn_JoinEff_struct*)
_tmp22D)->f1;_LL1D1: for(0;_tmp22E != 0;_tmp22E=_tmp22E->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp22E->hd))return 1;}return 0;_LL1D2: if(*((int*)
_tmp22D)!= 21)goto _LL1D4;_tmp22F=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp22D)->f1;_LL1D3: _tmp22F=Cyc_Tcutil_compress(_tmp22F);if(t == _tmp22F)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp22F);{void*_tmp234=Cyc_Tcutil_rgns_of(
t);void*_tmp235;_LL1D9: if(_tmp234 <= (void*)4)goto _LL1DB;if(*((int*)_tmp234)!= 21)
goto _LL1DB;_tmp235=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp234)->f1;_LL1DA: {
struct _tuple6 _tmp237=({struct _tuple6 _tmp236;_tmp236.f1=Cyc_Tcutil_compress(
_tmp235);_tmp236.f2=_tmp22F;_tmp236;});void*_tmp238;struct Cyc_Absyn_Tvar*_tmp239;
void*_tmp23A;struct Cyc_Absyn_Tvar*_tmp23B;_LL1DE: _tmp238=_tmp237.f1;if(_tmp238 <= (
void*)4)goto _LL1E0;if(*((int*)_tmp238)!= 1)goto _LL1E0;_tmp239=((struct Cyc_Absyn_VarType_struct*)
_tmp238)->f1;_tmp23A=_tmp237.f2;if(_tmp23A <= (void*)4)goto _LL1E0;if(*((int*)
_tmp23A)!= 1)goto _LL1E0;_tmp23B=((struct Cyc_Absyn_VarType_struct*)_tmp23A)->f1;
_LL1DF: return Cyc_Tcutil_unify(t,_tmp22F);_LL1E0:;_LL1E1: return _tmp235 == _tmp22F;
_LL1DD:;}_LL1DB:;_LL1DC: return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,
_tmp234);_LL1D8:;}_LL1D4: if(*((int*)_tmp22D)!= 0)goto _LL1D6;_tmp230=((struct Cyc_Absyn_Evar_struct*)
_tmp22D)->f1;_tmp231=((struct Cyc_Absyn_Evar_struct*)_tmp22D)->f2;_tmp232=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp22D)->f2;_tmp233=((struct Cyc_Absyn_Evar_struct*)
_tmp22D)->f4;_LL1D5: if(_tmp230 == 0  || (void*)_tmp230->v != (void*)4)({void*
_tmp23C[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp23D="effect evar has wrong kind";_tag_dynforward(_tmp23D,sizeof(
char),_get_zero_arr_size(_tmp23D,27));}),_tag_dynforward(_tmp23C,sizeof(void*),0));});
if(!may_constrain_evars)return 0;{void*_tmp23E=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp233);Cyc_Tcutil_occurs(_tmp23E,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp233))->v,t);{struct Cyc_Absyn_JoinEff_struct*
_tmp23F=({struct Cyc_Absyn_JoinEff_struct*_tmp241=_cycalloc(sizeof(*_tmp241));
_tmp241[0]=({struct Cyc_Absyn_JoinEff_struct _tmp242;_tmp242.tag=20;_tmp242.f1=({
struct Cyc_List_List*_tmp243=_cycalloc(sizeof(*_tmp243));_tmp243->hd=(void*)
_tmp23E;_tmp243->tl=({struct Cyc_List_List*_tmp244=_cycalloc(sizeof(*_tmp244));
_tmp244->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp245=_cycalloc(
sizeof(*_tmp245));_tmp245[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp246;_tmp246.tag=
21;_tmp246.f1=(void*)t;_tmp246;});_tmp245;}));_tmp244->tl=0;_tmp244;});_tmp243;});
_tmp242;});_tmp241;});*_tmp232=({struct Cyc_Core_Opt*_tmp240=_cycalloc(sizeof(*
_tmp240));_tmp240->v=(void*)((void*)_tmp23F);_tmp240;});return 1;}}_LL1D6:;_LL1D7:
return 0;_LL1CD:;}}static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,
struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp247=e;struct
Cyc_Absyn_Tvar*_tmp248;struct Cyc_List_List*_tmp249;void*_tmp24A;struct Cyc_Core_Opt*
_tmp24B;struct Cyc_Core_Opt*_tmp24C;struct Cyc_Core_Opt**_tmp24D;struct Cyc_Core_Opt*
_tmp24E;_LL1E3: if(_tmp247 <= (void*)4)goto _LL1EB;if(*((int*)_tmp247)!= 1)goto
_LL1E5;_tmp248=((struct Cyc_Absyn_VarType_struct*)_tmp247)->f1;_LL1E4: return Cyc_Absyn_tvar_cmp(
v,_tmp248)== 0;_LL1E5: if(*((int*)_tmp247)!= 20)goto _LL1E7;_tmp249=((struct Cyc_Absyn_JoinEff_struct*)
_tmp247)->f1;_LL1E6: for(0;_tmp249 != 0;_tmp249=_tmp249->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp249->hd))return 1;}return 0;_LL1E7: if(*((int*)
_tmp247)!= 21)goto _LL1E9;_tmp24A=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp247)->f1;_LL1E8: {void*_tmp24F=Cyc_Tcutil_rgns_of(_tmp24A);void*_tmp250;
_LL1EE: if(_tmp24F <= (void*)4)goto _LL1F0;if(*((int*)_tmp24F)!= 21)goto _LL1F0;
_tmp250=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp24F)->f1;_LL1EF: if(!
may_constrain_evars)return 0;{void*_tmp251=Cyc_Tcutil_compress(_tmp250);struct Cyc_Core_Opt*
_tmp252;struct Cyc_Core_Opt*_tmp253;struct Cyc_Core_Opt**_tmp254;struct Cyc_Core_Opt*
_tmp255;_LL1F3: if(_tmp251 <= (void*)4)goto _LL1F5;if(*((int*)_tmp251)!= 0)goto
_LL1F5;_tmp252=((struct Cyc_Absyn_Evar_struct*)_tmp251)->f1;_tmp253=((struct Cyc_Absyn_Evar_struct*)
_tmp251)->f2;_tmp254=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp251)->f2;_tmp255=((struct Cyc_Absyn_Evar_struct*)_tmp251)->f4;_LL1F4: {void*
_tmp256=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp255);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp255))->v,v))return 0;{void*_tmp257=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp259=_cycalloc(sizeof(*_tmp259));
_tmp259[0]=({struct Cyc_Absyn_JoinEff_struct _tmp25A;_tmp25A.tag=20;_tmp25A.f1=({
void*_tmp25B[2];_tmp25B[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp25C=
_cycalloc(sizeof(*_tmp25C));_tmp25C[0]=({struct Cyc_Absyn_VarType_struct _tmp25D;
_tmp25D.tag=1;_tmp25D.f1=v;_tmp25D;});_tmp25C;});_tmp25B[0]=_tmp256;Cyc_List_list(
_tag_dynforward(_tmp25B,sizeof(void*),2));});_tmp25A;});_tmp259;}));*_tmp254=({
struct Cyc_Core_Opt*_tmp258=_cycalloc(sizeof(*_tmp258));_tmp258->v=(void*)_tmp257;
_tmp258;});return 1;}}_LL1F5:;_LL1F6: return 0;_LL1F2:;}_LL1F0:;_LL1F1: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp24F);_LL1ED:;}_LL1E9: if(*((int*)_tmp247)!= 0)goto
_LL1EB;_tmp24B=((struct Cyc_Absyn_Evar_struct*)_tmp247)->f1;_tmp24C=((struct Cyc_Absyn_Evar_struct*)
_tmp247)->f2;_tmp24D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp247)->f2;_tmp24E=((struct Cyc_Absyn_Evar_struct*)_tmp247)->f4;_LL1EA: if(
_tmp24B == 0  || (void*)_tmp24B->v != (void*)4)({void*_tmp25E[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp25F="effect evar has wrong kind";_tag_dynforward(_tmp25F,sizeof(char),
_get_zero_arr_size(_tmp25F,27));}),_tag_dynforward(_tmp25E,sizeof(void*),0));});{
void*_tmp260=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp24E);if(
!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp24E))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct*
_tmp261=({struct Cyc_Absyn_JoinEff_struct*_tmp263=_cycalloc(sizeof(*_tmp263));
_tmp263[0]=({struct Cyc_Absyn_JoinEff_struct _tmp264;_tmp264.tag=20;_tmp264.f1=({
struct Cyc_List_List*_tmp265=_cycalloc(sizeof(*_tmp265));_tmp265->hd=(void*)
_tmp260;_tmp265->tl=({struct Cyc_List_List*_tmp266=_cycalloc(sizeof(*_tmp266));
_tmp266->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp267=_cycalloc(
sizeof(*_tmp267));_tmp267[0]=({struct Cyc_Absyn_VarType_struct _tmp268;_tmp268.tag=
1;_tmp268.f1=v;_tmp268;});_tmp267;}));_tmp266->tl=0;_tmp266;});_tmp265;});
_tmp264;});_tmp263;});*_tmp24D=({struct Cyc_Core_Opt*_tmp262=_cycalloc(sizeof(*
_tmp262));_tmp262->v=(void*)((void*)_tmp261);_tmp262;});return 1;}}_LL1EB:;_LL1EC:
return 0;_LL1E2:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp269=e;struct Cyc_List_List*_tmp26A;void*_tmp26B;_LL1F8: if(_tmp269 <= (
void*)4)goto _LL1FE;if(*((int*)_tmp269)!= 20)goto _LL1FA;_tmp26A=((struct Cyc_Absyn_JoinEff_struct*)
_tmp269)->f1;_LL1F9: for(0;_tmp26A != 0;_tmp26A=_tmp26A->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp26A->hd))return 1;}return 0;_LL1FA: if(*((int*)_tmp269)!= 21)goto
_LL1FC;_tmp26B=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp269)->f1;_LL1FB: {
void*_tmp26C=Cyc_Tcutil_rgns_of(_tmp26B);void*_tmp26D;_LL201: if(_tmp26C <= (void*)
4)goto _LL203;if(*((int*)_tmp26C)!= 21)goto _LL203;_tmp26D=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp26C)->f1;_LL202: return 0;_LL203:;_LL204: return Cyc_Tcutil_evar_in_effect(evar,
_tmp26C);_LL200:;}_LL1FC: if(*((int*)_tmp269)!= 0)goto _LL1FE;_LL1FD: return evar == 
e;_LL1FE:;_LL1FF: return 0;_LL1F7:;}}int Cyc_Tcutil_subset_effect(int
may_constrain_evars,void*e1,void*e2){void*_tmp26E=Cyc_Tcutil_compress(e1);struct
Cyc_List_List*_tmp26F;void*_tmp270;struct Cyc_Absyn_Tvar*_tmp271;void*_tmp272;
struct Cyc_Core_Opt*_tmp273;struct Cyc_Core_Opt**_tmp274;struct Cyc_Core_Opt*
_tmp275;_LL206: if(_tmp26E <= (void*)4)goto _LL210;if(*((int*)_tmp26E)!= 20)goto
_LL208;_tmp26F=((struct Cyc_Absyn_JoinEff_struct*)_tmp26E)->f1;_LL207: for(0;
_tmp26F != 0;_tmp26F=_tmp26F->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp26F->hd,e2))return 0;}return 1;_LL208: if(*((int*)
_tmp26E)!= 19)goto _LL20A;_tmp270=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp26E)->f1;_LL209: return Cyc_Tcutil_region_in_effect(0,_tmp270,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp270,(void*)2);_LL20A: if(*((int*)
_tmp26E)!= 1)goto _LL20C;_tmp271=((struct Cyc_Absyn_VarType_struct*)_tmp26E)->f1;
_LL20B: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp271,e2);
_LL20C: if(*((int*)_tmp26E)!= 21)goto _LL20E;_tmp272=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp26E)->f1;_LL20D: {void*_tmp276=Cyc_Tcutil_rgns_of(_tmp272);void*_tmp277;
_LL213: if(_tmp276 <= (void*)4)goto _LL215;if(*((int*)_tmp276)!= 21)goto _LL215;
_tmp277=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp276)->f1;_LL214: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,_tmp277,e2) || may_constrain_evars  && Cyc_Tcutil_unify(
_tmp277,Cyc_Absyn_sint_typ);_LL215:;_LL216: return Cyc_Tcutil_subset_effect(
may_constrain_evars,_tmp276,e2);_LL212:;}_LL20E: if(*((int*)_tmp26E)!= 0)goto
_LL210;_tmp273=((struct Cyc_Absyn_Evar_struct*)_tmp26E)->f2;_tmp274=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp26E)->f2;_tmp275=((struct Cyc_Absyn_Evar_struct*)
_tmp26E)->f4;_LL20F: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*_tmp274=({struct Cyc_Core_Opt*
_tmp278=_cycalloc(sizeof(*_tmp278));_tmp278->v=(void*)Cyc_Absyn_empty_effect;
_tmp278;});return 1;_LL210:;_LL211:({struct Cyc_String_pa_struct _tmp27B;_tmp27B.tag=
0;_tmp27B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
e1));{void*_tmp279[1]={& _tmp27B};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp27A="subset_effect: bad effect: %s";
_tag_dynforward(_tmp27A,sizeof(char),_get_zero_arr_size(_tmp27A,30));}),
_tag_dynforward(_tmp279,sizeof(void*),1));}});_LL205:;}static int Cyc_Tcutil_unify_effect(
void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(e1);e2=Cyc_Tcutil_normalize_effect(
e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct
Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){struct
_tuple6 _tmp27D;void*_tmp27E;void*_tmp27F;struct _tuple6*_tmp27C=(struct _tuple6*)
r1->hd;_tmp27D=*_tmp27C;_tmp27E=_tmp27D.f1;_tmp27F=_tmp27D.f2;{int found=_tmp27E
== (void*)2;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple6 _tmp281;void*_tmp282;void*_tmp283;struct _tuple6*_tmp280=(struct
_tuple6*)r2->hd;_tmp281=*_tmp280;_tmp282=_tmp281.f1;_tmp283=_tmp281.f2;if(Cyc_Tcutil_unify(
_tmp27E,_tmp282) && Cyc_Tcutil_unify(_tmp27F,_tmp283)){found=1;break;}}}if(!
found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*
rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(
rpo2,rpo1);}struct _tuple11{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*
f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _dynforward_ptr)_tag_dynforward(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp284=t1;struct Cyc_Core_Opt*_tmp285;struct Cyc_Core_Opt*
_tmp286;struct Cyc_Core_Opt**_tmp287;struct Cyc_Core_Opt*_tmp288;_LL218: if(_tmp284
<= (void*)4)goto _LL21A;if(*((int*)_tmp284)!= 0)goto _LL21A;_tmp285=((struct Cyc_Absyn_Evar_struct*)
_tmp284)->f1;_tmp286=((struct Cyc_Absyn_Evar_struct*)_tmp284)->f2;_tmp287=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp284)->f2;_tmp288=((struct Cyc_Absyn_Evar_struct*)
_tmp284)->f4;_LL219: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp288))->v,t2);{void*_tmp289=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp289,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp285))->v)){*_tmp287=({struct Cyc_Core_Opt*_tmp28A=_cycalloc(sizeof(*_tmp28A));
_tmp28A->v=(void*)t2;_tmp28A;});return;}else{{void*_tmp28B=t2;struct Cyc_Core_Opt*
_tmp28C;struct Cyc_Core_Opt**_tmp28D;struct Cyc_Core_Opt*_tmp28E;struct Cyc_Absyn_PtrInfo
_tmp28F;_LL21D: if(_tmp28B <= (void*)4)goto _LL221;if(*((int*)_tmp28B)!= 0)goto
_LL21F;_tmp28C=((struct Cyc_Absyn_Evar_struct*)_tmp28B)->f2;_tmp28D=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp28B)->f2;_tmp28E=((struct Cyc_Absyn_Evar_struct*)
_tmp28B)->f4;_LL21E: {struct Cyc_List_List*_tmp290=(struct Cyc_List_List*)_tmp288->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp28E))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp290,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=({
const char*_tmp291="(type variable would escape scope)";_tag_dynforward(_tmp291,
sizeof(char),_get_zero_arr_size(_tmp291,35));});(int)_throw((void*)Cyc_Tcutil_Unify);}}}
if(Cyc_Tcutil_kind_leq((void*)_tmp285->v,_tmp289)){*_tmp28D=({struct Cyc_Core_Opt*
_tmp292=_cycalloc(sizeof(*_tmp292));_tmp292->v=(void*)t1;_tmp292;});return;}Cyc_Tcutil_failure_reason=({
const char*_tmp293="(kinds are incompatible)";_tag_dynforward(_tmp293,sizeof(char),
_get_zero_arr_size(_tmp293,25));});goto _LL21C;}_LL21F: if(*((int*)_tmp28B)!= 4)
goto _LL221;_tmp28F=((struct Cyc_Absyn_PointerType_struct*)_tmp28B)->f1;if(!((void*)
_tmp285->v == (void*)2))goto _LL221;_LL220: {struct Cyc_Absyn_Conref*_tmp294=Cyc_Absyn_compress_conref((
_tmp28F.ptr_atts).bounds);{void*_tmp295=(void*)_tmp294->v;_LL224: if((int)_tmp295
!= 0)goto _LL226;_LL225:(void*)(_tmp294->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp296=_cycalloc(sizeof(*_tmp296));_tmp296[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp297;_tmp297.tag=0;_tmp297.f1=(void*)Cyc_Absyn_bounds_one;_tmp297;});_tmp296;})));*
_tmp287=({struct Cyc_Core_Opt*_tmp298=_cycalloc(sizeof(*_tmp298));_tmp298->v=(
void*)t2;_tmp298;});return;_LL226:;_LL227: goto _LL223;_LL223:;}goto _LL21C;}_LL221:;
_LL222: goto _LL21C;_LL21C:;}Cyc_Tcutil_failure_reason=({const char*_tmp299="(kinds are incompatible)";
_tag_dynforward(_tmp299,sizeof(char),_get_zero_arr_size(_tmp299,25));});(int)
_throw((void*)Cyc_Tcutil_Unify);}}_LL21A:;_LL21B: goto _LL217;_LL217:;}{struct
_tuple6 _tmp29B=({struct _tuple6 _tmp29A;_tmp29A.f1=t2;_tmp29A.f2=t1;_tmp29A;});
void*_tmp29C;void*_tmp29D;void*_tmp29E;void*_tmp29F;struct Cyc_Absyn_Tvar*_tmp2A0;
void*_tmp2A1;struct Cyc_Absyn_Tvar*_tmp2A2;void*_tmp2A3;struct Cyc_Absyn_AggrInfo
_tmp2A4;void*_tmp2A5;struct Cyc_List_List*_tmp2A6;void*_tmp2A7;struct Cyc_Absyn_AggrInfo
_tmp2A8;void*_tmp2A9;struct Cyc_List_List*_tmp2AA;void*_tmp2AB;struct _tuple1*
_tmp2AC;void*_tmp2AD;struct _tuple1*_tmp2AE;void*_tmp2AF;struct Cyc_List_List*
_tmp2B0;void*_tmp2B1;struct Cyc_List_List*_tmp2B2;void*_tmp2B3;struct Cyc_Absyn_TunionInfo
_tmp2B4;void*_tmp2B5;struct Cyc_Absyn_Tuniondecl**_tmp2B6;struct Cyc_Absyn_Tuniondecl*
_tmp2B7;struct Cyc_List_List*_tmp2B8;struct Cyc_Core_Opt*_tmp2B9;void*_tmp2BA;
struct Cyc_Absyn_TunionInfo _tmp2BB;void*_tmp2BC;struct Cyc_Absyn_Tuniondecl**
_tmp2BD;struct Cyc_Absyn_Tuniondecl*_tmp2BE;struct Cyc_List_List*_tmp2BF;struct Cyc_Core_Opt*
_tmp2C0;void*_tmp2C1;struct Cyc_Absyn_TunionFieldInfo _tmp2C2;void*_tmp2C3;struct
Cyc_Absyn_Tuniondecl*_tmp2C4;struct Cyc_Absyn_Tunionfield*_tmp2C5;struct Cyc_List_List*
_tmp2C6;void*_tmp2C7;struct Cyc_Absyn_TunionFieldInfo _tmp2C8;void*_tmp2C9;struct
Cyc_Absyn_Tuniondecl*_tmp2CA;struct Cyc_Absyn_Tunionfield*_tmp2CB;struct Cyc_List_List*
_tmp2CC;void*_tmp2CD;struct Cyc_Absyn_PtrInfo _tmp2CE;void*_tmp2CF;struct Cyc_Absyn_Tqual
_tmp2D0;struct Cyc_Absyn_PtrAtts _tmp2D1;void*_tmp2D2;struct Cyc_Absyn_Conref*
_tmp2D3;struct Cyc_Absyn_Conref*_tmp2D4;struct Cyc_Absyn_Conref*_tmp2D5;void*
_tmp2D6;struct Cyc_Absyn_PtrInfo _tmp2D7;void*_tmp2D8;struct Cyc_Absyn_Tqual _tmp2D9;
struct Cyc_Absyn_PtrAtts _tmp2DA;void*_tmp2DB;struct Cyc_Absyn_Conref*_tmp2DC;
struct Cyc_Absyn_Conref*_tmp2DD;struct Cyc_Absyn_Conref*_tmp2DE;void*_tmp2DF;void*
_tmp2E0;void*_tmp2E1;void*_tmp2E2;void*_tmp2E3;void*_tmp2E4;void*_tmp2E5;void*
_tmp2E6;void*_tmp2E7;int _tmp2E8;void*_tmp2E9;int _tmp2EA;void*_tmp2EB;void*
_tmp2EC;void*_tmp2ED;void*_tmp2EE;void*_tmp2EF;int _tmp2F0;void*_tmp2F1;int
_tmp2F2;void*_tmp2F3;void*_tmp2F4;void*_tmp2F5;void*_tmp2F6;void*_tmp2F7;struct
Cyc_Absyn_ArrayInfo _tmp2F8;void*_tmp2F9;struct Cyc_Absyn_Tqual _tmp2FA;struct Cyc_Absyn_Exp*
_tmp2FB;struct Cyc_Absyn_Conref*_tmp2FC;void*_tmp2FD;struct Cyc_Absyn_ArrayInfo
_tmp2FE;void*_tmp2FF;struct Cyc_Absyn_Tqual _tmp300;struct Cyc_Absyn_Exp*_tmp301;
struct Cyc_Absyn_Conref*_tmp302;void*_tmp303;struct Cyc_Absyn_FnInfo _tmp304;struct
Cyc_List_List*_tmp305;struct Cyc_Core_Opt*_tmp306;void*_tmp307;struct Cyc_List_List*
_tmp308;int _tmp309;struct Cyc_Absyn_VarargInfo*_tmp30A;struct Cyc_List_List*
_tmp30B;struct Cyc_List_List*_tmp30C;void*_tmp30D;struct Cyc_Absyn_FnInfo _tmp30E;
struct Cyc_List_List*_tmp30F;struct Cyc_Core_Opt*_tmp310;void*_tmp311;struct Cyc_List_List*
_tmp312;int _tmp313;struct Cyc_Absyn_VarargInfo*_tmp314;struct Cyc_List_List*
_tmp315;struct Cyc_List_List*_tmp316;void*_tmp317;struct Cyc_List_List*_tmp318;
void*_tmp319;struct Cyc_List_List*_tmp31A;void*_tmp31B;void*_tmp31C;struct Cyc_List_List*
_tmp31D;void*_tmp31E;void*_tmp31F;struct Cyc_List_List*_tmp320;void*_tmp321;void*
_tmp322;void*_tmp323;void*_tmp324;void*_tmp325;void*_tmp326;void*_tmp327;void*
_tmp328;void*_tmp329;void*_tmp32A;void*_tmp32B;void*_tmp32C;void*_tmp32D;void*
_tmp32E;_LL229: _tmp29C=_tmp29B.f1;if(_tmp29C <= (void*)4)goto _LL22B;if(*((int*)
_tmp29C)!= 0)goto _LL22B;_LL22A: Cyc_Tcutil_unify_it(t2,t1);return;_LL22B: _tmp29D=
_tmp29B.f1;if((int)_tmp29D != 0)goto _LL22D;_tmp29E=_tmp29B.f2;if((int)_tmp29E != 0)
goto _LL22D;_LL22C: return;_LL22D: _tmp29F=_tmp29B.f1;if(_tmp29F <= (void*)4)goto
_LL22F;if(*((int*)_tmp29F)!= 1)goto _LL22F;_tmp2A0=((struct Cyc_Absyn_VarType_struct*)
_tmp29F)->f1;_tmp2A1=_tmp29B.f2;if(_tmp2A1 <= (void*)4)goto _LL22F;if(*((int*)
_tmp2A1)!= 1)goto _LL22F;_tmp2A2=((struct Cyc_Absyn_VarType_struct*)_tmp2A1)->f1;
_LL22E: {struct _dynforward_ptr*_tmp32F=_tmp2A0->name;struct _dynforward_ptr*
_tmp330=_tmp2A2->name;int _tmp331=*((int*)_check_null(_tmp2A0->identity));int
_tmp332=*((int*)_check_null(_tmp2A2->identity));void*_tmp333=Cyc_Tcutil_tvar_kind(
_tmp2A0);void*_tmp334=Cyc_Tcutil_tvar_kind(_tmp2A2);if(_tmp332 == _tmp331  && Cyc_strptrcmp(
_tmp32F,_tmp330)== 0){if(_tmp333 != _tmp334)({struct Cyc_String_pa_struct _tmp339;
_tmp339.tag=0;_tmp339.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(
_tmp334));{struct Cyc_String_pa_struct _tmp338;_tmp338.tag=0;_tmp338.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(_tmp333));{
struct Cyc_String_pa_struct _tmp337;_tmp337.tag=0;_tmp337.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp32F);{void*_tmp335[3]={& _tmp337,&
_tmp338,& _tmp339};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp336="same type variable %s has kinds %s and %s";_tag_dynforward(
_tmp336,sizeof(char),_get_zero_arr_size(_tmp336,42));}),_tag_dynforward(_tmp335,
sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=({const char*_tmp33A="(variable types are not the same)";
_tag_dynforward(_tmp33A,sizeof(char),_get_zero_arr_size(_tmp33A,34));});goto
_LL228;}_LL22F: _tmp2A3=_tmp29B.f1;if(_tmp2A3 <= (void*)4)goto _LL231;if(*((int*)
_tmp2A3)!= 10)goto _LL231;_tmp2A4=((struct Cyc_Absyn_AggrType_struct*)_tmp2A3)->f1;
_tmp2A5=(void*)_tmp2A4.aggr_info;_tmp2A6=_tmp2A4.targs;_tmp2A7=_tmp29B.f2;if(
_tmp2A7 <= (void*)4)goto _LL231;if(*((int*)_tmp2A7)!= 10)goto _LL231;_tmp2A8=((
struct Cyc_Absyn_AggrType_struct*)_tmp2A7)->f1;_tmp2A9=(void*)_tmp2A8.aggr_info;
_tmp2AA=_tmp2A8.targs;_LL230: {void*_tmp33C;struct _tuple1*_tmp33D;struct _tuple5
_tmp33B=Cyc_Absyn_aggr_kinded_name(_tmp2A9);_tmp33C=_tmp33B.f1;_tmp33D=_tmp33B.f2;{
void*_tmp33F;struct _tuple1*_tmp340;struct _tuple5 _tmp33E=Cyc_Absyn_aggr_kinded_name(
_tmp2A5);_tmp33F=_tmp33E.f1;_tmp340=_tmp33E.f2;if(_tmp33C != _tmp33F){Cyc_Tcutil_failure_reason=({
const char*_tmp341="(struct and union type)";_tag_dynforward(_tmp341,sizeof(char),
_get_zero_arr_size(_tmp341,24));});goto _LL228;}if(Cyc_Absyn_qvar_cmp(_tmp33D,
_tmp340)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp342="(different type name)";
_tag_dynforward(_tmp342,sizeof(char),_get_zero_arr_size(_tmp342,22));});goto
_LL228;}Cyc_Tcutil_unify_list(_tmp2AA,_tmp2A6);return;}}_LL231: _tmp2AB=_tmp29B.f1;
if(_tmp2AB <= (void*)4)goto _LL233;if(*((int*)_tmp2AB)!= 12)goto _LL233;_tmp2AC=((
struct Cyc_Absyn_EnumType_struct*)_tmp2AB)->f1;_tmp2AD=_tmp29B.f2;if(_tmp2AD <= (
void*)4)goto _LL233;if(*((int*)_tmp2AD)!= 12)goto _LL233;_tmp2AE=((struct Cyc_Absyn_EnumType_struct*)
_tmp2AD)->f1;_LL232: if(Cyc_Absyn_qvar_cmp(_tmp2AC,_tmp2AE)== 0)return;Cyc_Tcutil_failure_reason=({
const char*_tmp343="(different enum types)";_tag_dynforward(_tmp343,sizeof(char),
_get_zero_arr_size(_tmp343,23));});goto _LL228;_LL233: _tmp2AF=_tmp29B.f1;if(
_tmp2AF <= (void*)4)goto _LL235;if(*((int*)_tmp2AF)!= 13)goto _LL235;_tmp2B0=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp2AF)->f1;_tmp2B1=_tmp29B.f2;if(_tmp2B1
<= (void*)4)goto _LL235;if(*((int*)_tmp2B1)!= 13)goto _LL235;_tmp2B2=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp2B1)->f1;_LL234: {int bad=0;for(0;_tmp2B0 != 0  && _tmp2B2 != 0;(_tmp2B0=_tmp2B0->tl,
_tmp2B2=_tmp2B2->tl)){struct Cyc_Absyn_Enumfield*_tmp344=(struct Cyc_Absyn_Enumfield*)
_tmp2B0->hd;struct Cyc_Absyn_Enumfield*_tmp345=(struct Cyc_Absyn_Enumfield*)
_tmp2B2->hd;if(Cyc_Absyn_qvar_cmp(_tmp344->name,_tmp345->name)!= 0){Cyc_Tcutil_failure_reason=({
const char*_tmp346="(different names for enum fields)";_tag_dynforward(_tmp346,
sizeof(char),_get_zero_arr_size(_tmp346,34));});bad=1;break;}if(_tmp344->tag == 
_tmp345->tag)continue;if(_tmp344->tag == 0  || _tmp345->tag == 0){Cyc_Tcutil_failure_reason=({
const char*_tmp347="(different tag values for enum fields)";_tag_dynforward(
_tmp347,sizeof(char),_get_zero_arr_size(_tmp347,39));});bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp344->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp345->tag))){Cyc_Tcutil_failure_reason=({const char*_tmp348="(different tag values for enum fields)";
_tag_dynforward(_tmp348,sizeof(char),_get_zero_arr_size(_tmp348,39));});bad=1;
break;}}if(bad)goto _LL228;if(_tmp2B0 == 0  && _tmp2B2 == 0)return;Cyc_Tcutil_failure_reason=({
const char*_tmp349="(different number of fields for enums)";_tag_dynforward(
_tmp349,sizeof(char),_get_zero_arr_size(_tmp349,39));});goto _LL228;}_LL235:
_tmp2B3=_tmp29B.f1;if(_tmp2B3 <= (void*)4)goto _LL237;if(*((int*)_tmp2B3)!= 2)goto
_LL237;_tmp2B4=((struct Cyc_Absyn_TunionType_struct*)_tmp2B3)->f1;_tmp2B5=(void*)
_tmp2B4.tunion_info;if(*((int*)_tmp2B5)!= 1)goto _LL237;_tmp2B6=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp2B5)->f1;_tmp2B7=*_tmp2B6;_tmp2B8=_tmp2B4.targs;_tmp2B9=_tmp2B4.rgn;_tmp2BA=
_tmp29B.f2;if(_tmp2BA <= (void*)4)goto _LL237;if(*((int*)_tmp2BA)!= 2)goto _LL237;
_tmp2BB=((struct Cyc_Absyn_TunionType_struct*)_tmp2BA)->f1;_tmp2BC=(void*)_tmp2BB.tunion_info;
if(*((int*)_tmp2BC)!= 1)goto _LL237;_tmp2BD=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp2BC)->f1;_tmp2BE=*_tmp2BD;_tmp2BF=_tmp2BB.targs;_tmp2C0=_tmp2BB.rgn;_LL236:
if(_tmp2B7 == _tmp2BE  || Cyc_Absyn_qvar_cmp(_tmp2B7->name,_tmp2BE->name)== 0){if(
_tmp2C0 != 0  && _tmp2B9 != 0)Cyc_Tcutil_unify_it((void*)_tmp2C0->v,(void*)_tmp2B9->v);
else{if(_tmp2C0 != 0  || _tmp2B9 != 0)goto Tunion_fail;}Cyc_Tcutil_unify_list(
_tmp2BF,_tmp2B8);return;}Tunion_fail: Cyc_Tcutil_failure_reason=({const char*
_tmp34A="(different tunion types)";_tag_dynforward(_tmp34A,sizeof(char),
_get_zero_arr_size(_tmp34A,25));});goto _LL228;_LL237: _tmp2C1=_tmp29B.f1;if(
_tmp2C1 <= (void*)4)goto _LL239;if(*((int*)_tmp2C1)!= 3)goto _LL239;_tmp2C2=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp2C1)->f1;_tmp2C3=(void*)_tmp2C2.field_info;
if(*((int*)_tmp2C3)!= 1)goto _LL239;_tmp2C4=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp2C3)->f1;_tmp2C5=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2C3)->f2;
_tmp2C6=_tmp2C2.targs;_tmp2C7=_tmp29B.f2;if(_tmp2C7 <= (void*)4)goto _LL239;if(*((
int*)_tmp2C7)!= 3)goto _LL239;_tmp2C8=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp2C7)->f1;_tmp2C9=(void*)_tmp2C8.field_info;if(*((int*)_tmp2C9)!= 1)goto
_LL239;_tmp2CA=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2C9)->f1;_tmp2CB=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2C9)->f2;_tmp2CC=_tmp2C8.targs;
_LL238: if((_tmp2C4 == _tmp2CA  || Cyc_Absyn_qvar_cmp(_tmp2C4->name,_tmp2CA->name)
== 0) && (_tmp2C5 == _tmp2CB  || Cyc_Absyn_qvar_cmp(_tmp2C5->name,_tmp2CB->name)== 
0)){Cyc_Tcutil_unify_list(_tmp2CC,_tmp2C6);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp34B="(different tunion field types)";_tag_dynforward(_tmp34B,
sizeof(char),_get_zero_arr_size(_tmp34B,31));});goto _LL228;_LL239: _tmp2CD=
_tmp29B.f1;if(_tmp2CD <= (void*)4)goto _LL23B;if(*((int*)_tmp2CD)!= 4)goto _LL23B;
_tmp2CE=((struct Cyc_Absyn_PointerType_struct*)_tmp2CD)->f1;_tmp2CF=(void*)
_tmp2CE.elt_typ;_tmp2D0=_tmp2CE.elt_tq;_tmp2D1=_tmp2CE.ptr_atts;_tmp2D2=(void*)
_tmp2D1.rgn;_tmp2D3=_tmp2D1.nullable;_tmp2D4=_tmp2D1.bounds;_tmp2D5=_tmp2D1.zero_term;
_tmp2D6=_tmp29B.f2;if(_tmp2D6 <= (void*)4)goto _LL23B;if(*((int*)_tmp2D6)!= 4)goto
_LL23B;_tmp2D7=((struct Cyc_Absyn_PointerType_struct*)_tmp2D6)->f1;_tmp2D8=(void*)
_tmp2D7.elt_typ;_tmp2D9=_tmp2D7.elt_tq;_tmp2DA=_tmp2D7.ptr_atts;_tmp2DB=(void*)
_tmp2DA.rgn;_tmp2DC=_tmp2DA.nullable;_tmp2DD=_tmp2DA.bounds;_tmp2DE=_tmp2DA.zero_term;
_LL23A: Cyc_Tcutil_unify_it(_tmp2D8,_tmp2CF);Cyc_Tcutil_unify_it(_tmp2D2,_tmp2DB);
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;((void(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2DE,_tmp2D5,({const char*_tmp34C="(not both zero terminated)";
_tag_dynforward(_tmp34C,sizeof(char),_get_zero_arr_size(_tmp34C,27));}));Cyc_Tcutil_unify_tqual(
_tmp2D9,_tmp2D8,_tmp2D0,_tmp2CF);Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp2DD,_tmp2D4,({const char*_tmp34D="(different pointer bounds)";_tag_dynforward(
_tmp34D,sizeof(char),_get_zero_arr_size(_tmp34D,27));}));{void*_tmp34E=(void*)(
Cyc_Absyn_compress_conref(_tmp2DD))->v;void*_tmp34F;void*_tmp350;_LL264: if(
_tmp34E <= (void*)1)goto _LL268;if(*((int*)_tmp34E)!= 0)goto _LL266;_tmp34F=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp34E)->f1;if((int)_tmp34F != 0)goto _LL266;
_LL265: return;_LL266: if(*((int*)_tmp34E)!= 0)goto _LL268;_tmp350=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmp34E)->f1;if((int)_tmp350 != 1)goto _LL268;_LL267:
return;_LL268:;_LL269: goto _LL263;_LL263:;}((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2DC,_tmp2D3,({const char*_tmp351="(incompatible pointer types)";
_tag_dynforward(_tmp351,sizeof(char),_get_zero_arr_size(_tmp351,29));}));return;
_LL23B: _tmp2DF=_tmp29B.f1;if(_tmp2DF <= (void*)4)goto _LL23D;if(*((int*)_tmp2DF)!= 
5)goto _LL23D;_tmp2E0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2DF)->f1;
_tmp2E1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2DF)->f2;_tmp2E2=_tmp29B.f2;
if(_tmp2E2 <= (void*)4)goto _LL23D;if(*((int*)_tmp2E2)!= 5)goto _LL23D;_tmp2E3=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2E2)->f1;_tmp2E4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2E2)->f2;_LL23C: if(_tmp2E3 == _tmp2E0  && ((_tmp2E4 == _tmp2E1  || _tmp2E4 == (
void*)2  && _tmp2E1 == (void*)3) || _tmp2E4 == (void*)3  && _tmp2E1 == (void*)2))
return;Cyc_Tcutil_failure_reason=({const char*_tmp352="(different integral types)";
_tag_dynforward(_tmp352,sizeof(char),_get_zero_arr_size(_tmp352,27));});goto
_LL228;_LL23D: _tmp2E5=_tmp29B.f1;if((int)_tmp2E5 != 1)goto _LL23F;_tmp2E6=_tmp29B.f2;
if((int)_tmp2E6 != 1)goto _LL23F;_LL23E: return;_LL23F: _tmp2E7=_tmp29B.f1;if(
_tmp2E7 <= (void*)4)goto _LL241;if(*((int*)_tmp2E7)!= 6)goto _LL241;_tmp2E8=((
struct Cyc_Absyn_DoubleType_struct*)_tmp2E7)->f1;_tmp2E9=_tmp29B.f2;if(_tmp2E9 <= (
void*)4)goto _LL241;if(*((int*)_tmp2E9)!= 6)goto _LL241;_tmp2EA=((struct Cyc_Absyn_DoubleType_struct*)
_tmp2E9)->f1;_LL240: if(_tmp2E8 == _tmp2EA)return;goto _LL228;_LL241: _tmp2EB=
_tmp29B.f1;if(_tmp2EB <= (void*)4)goto _LL243;if(*((int*)_tmp2EB)!= 14)goto _LL243;
_tmp2EC=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2EB)->f1;_tmp2ED=_tmp29B.f2;
if(_tmp2ED <= (void*)4)goto _LL243;if(*((int*)_tmp2ED)!= 14)goto _LL243;_tmp2EE=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2ED)->f1;_LL242: Cyc_Tcutil_unify_it(
_tmp2EC,_tmp2EE);return;_LL243: _tmp2EF=_tmp29B.f1;if(_tmp2EF <= (void*)4)goto
_LL245;if(*((int*)_tmp2EF)!= 18)goto _LL245;_tmp2F0=((struct Cyc_Absyn_TypeInt_struct*)
_tmp2EF)->f1;_tmp2F1=_tmp29B.f2;if(_tmp2F1 <= (void*)4)goto _LL245;if(*((int*)
_tmp2F1)!= 18)goto _LL245;_tmp2F2=((struct Cyc_Absyn_TypeInt_struct*)_tmp2F1)->f1;
_LL244: if(_tmp2F0 == _tmp2F2)return;Cyc_Tcutil_failure_reason=({const char*_tmp353="(different type integers)";
_tag_dynforward(_tmp353,sizeof(char),_get_zero_arr_size(_tmp353,26));});goto
_LL228;_LL245: _tmp2F3=_tmp29B.f1;if(_tmp2F3 <= (void*)4)goto _LL247;if(*((int*)
_tmp2F3)!= 17)goto _LL247;_tmp2F4=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp2F3)->f1;_tmp2F5=_tmp29B.f2;if(_tmp2F5 <= (void*)4)goto _LL247;if(*((int*)
_tmp2F5)!= 17)goto _LL247;_tmp2F6=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp2F5)->f1;_LL246: Cyc_Tcutil_unify_it(_tmp2F4,_tmp2F6);return;_LL247: _tmp2F7=
_tmp29B.f1;if(_tmp2F7 <= (void*)4)goto _LL249;if(*((int*)_tmp2F7)!= 7)goto _LL249;
_tmp2F8=((struct Cyc_Absyn_ArrayType_struct*)_tmp2F7)->f1;_tmp2F9=(void*)_tmp2F8.elt_type;
_tmp2FA=_tmp2F8.tq;_tmp2FB=_tmp2F8.num_elts;_tmp2FC=_tmp2F8.zero_term;_tmp2FD=
_tmp29B.f2;if(_tmp2FD <= (void*)4)goto _LL249;if(*((int*)_tmp2FD)!= 7)goto _LL249;
_tmp2FE=((struct Cyc_Absyn_ArrayType_struct*)_tmp2FD)->f1;_tmp2FF=(void*)_tmp2FE.elt_type;
_tmp300=_tmp2FE.tq;_tmp301=_tmp2FE.num_elts;_tmp302=_tmp2FE.zero_term;_LL248: Cyc_Tcutil_unify_it(
_tmp2FF,_tmp2F9);Cyc_Tcutil_unify_tqual(_tmp300,_tmp2FF,_tmp2FA,_tmp2F9);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2FC,_tmp302,({const char*_tmp354="(not both zero terminated)";
_tag_dynforward(_tmp354,sizeof(char),_get_zero_arr_size(_tmp354,27));}));if(
_tmp2FB == _tmp301)return;if(_tmp2FB == 0  || _tmp301 == 0)goto _LL228;if(Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_tmp2FB,(struct Cyc_Absyn_Exp*)_tmp301))return;Cyc_Tcutil_failure_reason=({
const char*_tmp355="(different array sizes)";_tag_dynforward(_tmp355,sizeof(char),
_get_zero_arr_size(_tmp355,24));});goto _LL228;_LL249: _tmp303=_tmp29B.f1;if(
_tmp303 <= (void*)4)goto _LL24B;if(*((int*)_tmp303)!= 8)goto _LL24B;_tmp304=((
struct Cyc_Absyn_FnType_struct*)_tmp303)->f1;_tmp305=_tmp304.tvars;_tmp306=
_tmp304.effect;_tmp307=(void*)_tmp304.ret_typ;_tmp308=_tmp304.args;_tmp309=
_tmp304.c_varargs;_tmp30A=_tmp304.cyc_varargs;_tmp30B=_tmp304.rgn_po;_tmp30C=
_tmp304.attributes;_tmp30D=_tmp29B.f2;if(_tmp30D <= (void*)4)goto _LL24B;if(*((int*)
_tmp30D)!= 8)goto _LL24B;_tmp30E=((struct Cyc_Absyn_FnType_struct*)_tmp30D)->f1;
_tmp30F=_tmp30E.tvars;_tmp310=_tmp30E.effect;_tmp311=(void*)_tmp30E.ret_typ;
_tmp312=_tmp30E.args;_tmp313=_tmp30E.c_varargs;_tmp314=_tmp30E.cyc_varargs;
_tmp315=_tmp30E.rgn_po;_tmp316=_tmp30E.attributes;_LL24A: {int done=0;{struct
_RegionHandle _tmp356=_new_region("rgn");struct _RegionHandle*rgn=& _tmp356;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp30F != 0){if(_tmp305 == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp357="(second function type has too few type variables)";
_tag_dynforward(_tmp357,sizeof(char),_get_zero_arr_size(_tmp357,50));});(int)
_throw((void*)Cyc_Tcutil_Unify);}{void*_tmp358=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp30F->hd);void*_tmp359=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp305->hd);
if(_tmp358 != _tmp359){Cyc_Tcutil_failure_reason=(struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp35E;_tmp35E.tag=0;_tmp35E.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_kind2string(_tmp359));{struct Cyc_String_pa_struct
_tmp35D;_tmp35D.tag=0;_tmp35D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_kind2string(_tmp358));{struct Cyc_String_pa_struct _tmp35C;_tmp35C.tag=
0;_tmp35C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp30F->hd));{void*_tmp35A[3]={& _tmp35C,& _tmp35D,& _tmp35E};
Cyc_aprintf(({const char*_tmp35B="(type var %s has different kinds %s and %s)";
_tag_dynforward(_tmp35B,sizeof(char),_get_zero_arr_size(_tmp35B,44));}),
_tag_dynforward(_tmp35A,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}
inst=({struct Cyc_List_List*_tmp35F=_region_malloc(rgn,sizeof(*_tmp35F));_tmp35F->hd=({
struct _tuple8*_tmp360=_region_malloc(rgn,sizeof(*_tmp360));_tmp360->f1=(struct
Cyc_Absyn_Tvar*)_tmp305->hd;_tmp360->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp361=_cycalloc(sizeof(*_tmp361));_tmp361[0]=({struct Cyc_Absyn_VarType_struct
_tmp362;_tmp362.tag=1;_tmp362.f1=(struct Cyc_Absyn_Tvar*)_tmp30F->hd;_tmp362;});
_tmp361;});_tmp360;});_tmp35F->tl=inst;_tmp35F;});_tmp30F=_tmp30F->tl;_tmp305=
_tmp305->tl;}}if(_tmp305 != 0){Cyc_Tcutil_failure_reason=({const char*_tmp363="(second function type has too many type variables)";
_tag_dynforward(_tmp363,sizeof(char),_get_zero_arr_size(_tmp363,51));});
_npop_handler(0);goto _LL228;}if(inst != 0){Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_struct*
_tmp364=_cycalloc(sizeof(*_tmp364));_tmp364[0]=({struct Cyc_Absyn_FnType_struct
_tmp365;_tmp365.tag=8;_tmp365.f1=({struct Cyc_Absyn_FnInfo _tmp366;_tmp366.tvars=0;
_tmp366.effect=_tmp310;_tmp366.ret_typ=(void*)_tmp311;_tmp366.args=_tmp312;
_tmp366.c_varargs=_tmp313;_tmp366.cyc_varargs=_tmp314;_tmp366.rgn_po=_tmp315;
_tmp366.attributes=_tmp316;_tmp366;});_tmp365;});_tmp364;}),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*_tmp367=_cycalloc(sizeof(*
_tmp367));_tmp367[0]=({struct Cyc_Absyn_FnType_struct _tmp368;_tmp368.tag=8;
_tmp368.f1=({struct Cyc_Absyn_FnInfo _tmp369;_tmp369.tvars=0;_tmp369.effect=
_tmp306;_tmp369.ret_typ=(void*)_tmp307;_tmp369.args=_tmp308;_tmp369.c_varargs=
_tmp309;_tmp369.cyc_varargs=_tmp30A;_tmp369.rgn_po=_tmp30B;_tmp369.attributes=
_tmp30C;_tmp369;});_tmp368;});_tmp367;})));done=1;}};_pop_region(rgn);}if(done)
return;Cyc_Tcutil_unify_it(_tmp311,_tmp307);for(0;_tmp312 != 0  && _tmp308 != 0;(
_tmp312=_tmp312->tl,_tmp308=_tmp308->tl)){struct Cyc_Absyn_Tqual _tmp36B;void*
_tmp36C;struct _tuple2 _tmp36A=*((struct _tuple2*)_tmp312->hd);_tmp36B=_tmp36A.f2;
_tmp36C=_tmp36A.f3;{struct Cyc_Absyn_Tqual _tmp36E;void*_tmp36F;struct _tuple2
_tmp36D=*((struct _tuple2*)_tmp308->hd);_tmp36E=_tmp36D.f2;_tmp36F=_tmp36D.f3;Cyc_Tcutil_unify_it(
_tmp36C,_tmp36F);Cyc_Tcutil_unify_tqual(_tmp36B,_tmp36C,_tmp36E,_tmp36F);}}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp312 != 0  || _tmp308 != 0){Cyc_Tcutil_failure_reason=({
const char*_tmp370="(function types have different number of arguments)";
_tag_dynforward(_tmp370,sizeof(char),_get_zero_arr_size(_tmp370,52));});goto
_LL228;}if(_tmp313 != _tmp309){Cyc_Tcutil_failure_reason=({const char*_tmp371="(only one function type takes C varargs)";
_tag_dynforward(_tmp371,sizeof(char),_get_zero_arr_size(_tmp371,41));});goto
_LL228;}{int bad_cyc_vararg=0;{struct _tuple11 _tmp373=({struct _tuple11 _tmp372;
_tmp372.f1=_tmp314;_tmp372.f2=_tmp30A;_tmp372;});struct Cyc_Absyn_VarargInfo*
_tmp374;struct Cyc_Absyn_VarargInfo*_tmp375;struct Cyc_Absyn_VarargInfo*_tmp376;
struct Cyc_Absyn_VarargInfo*_tmp377;struct Cyc_Absyn_VarargInfo*_tmp378;struct Cyc_Absyn_VarargInfo
_tmp379;struct Cyc_Core_Opt*_tmp37A;struct Cyc_Absyn_Tqual _tmp37B;void*_tmp37C;int
_tmp37D;struct Cyc_Absyn_VarargInfo*_tmp37E;struct Cyc_Absyn_VarargInfo _tmp37F;
struct Cyc_Core_Opt*_tmp380;struct Cyc_Absyn_Tqual _tmp381;void*_tmp382;int _tmp383;
_LL26B: _tmp374=_tmp373.f1;if(_tmp374 != 0)goto _LL26D;_tmp375=_tmp373.f2;if(
_tmp375 != 0)goto _LL26D;_LL26C: goto _LL26A;_LL26D: _tmp376=_tmp373.f1;if(_tmp376 != 
0)goto _LL26F;_LL26E: goto _LL270;_LL26F: _tmp377=_tmp373.f2;if(_tmp377 != 0)goto
_LL271;_LL270: bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp384="(only one function type takes varargs)";
_tag_dynforward(_tmp384,sizeof(char),_get_zero_arr_size(_tmp384,39));});goto
_LL26A;_LL271: _tmp378=_tmp373.f1;if(_tmp378 == 0)goto _LL26A;_tmp379=*_tmp378;
_tmp37A=_tmp379.name;_tmp37B=_tmp379.tq;_tmp37C=(void*)_tmp379.type;_tmp37D=
_tmp379.inject;_tmp37E=_tmp373.f2;if(_tmp37E == 0)goto _LL26A;_tmp37F=*_tmp37E;
_tmp380=_tmp37F.name;_tmp381=_tmp37F.tq;_tmp382=(void*)_tmp37F.type;_tmp383=
_tmp37F.inject;_LL272: Cyc_Tcutil_unify_it(_tmp37C,_tmp382);Cyc_Tcutil_unify_tqual(
_tmp37B,_tmp37C,_tmp381,_tmp382);if(_tmp37D != _tmp383){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({
const char*_tmp385="(only one function type injects varargs)";_tag_dynforward(
_tmp385,sizeof(char),_get_zero_arr_size(_tmp385,41));});}goto _LL26A;_LL26A:;}if(
bad_cyc_vararg)goto _LL228;{int bad_effect=0;{struct _tuple12 _tmp387=({struct
_tuple12 _tmp386;_tmp386.f1=_tmp310;_tmp386.f2=_tmp306;_tmp386;});struct Cyc_Core_Opt*
_tmp388;struct Cyc_Core_Opt*_tmp389;struct Cyc_Core_Opt*_tmp38A;struct Cyc_Core_Opt*
_tmp38B;_LL274: _tmp388=_tmp387.f1;if(_tmp388 != 0)goto _LL276;_tmp389=_tmp387.f2;
if(_tmp389 != 0)goto _LL276;_LL275: goto _LL273;_LL276: _tmp38A=_tmp387.f1;if(_tmp38A
!= 0)goto _LL278;_LL277: goto _LL279;_LL278: _tmp38B=_tmp387.f2;if(_tmp38B != 0)goto
_LL27A;_LL279: bad_effect=1;goto _LL273;_LL27A:;_LL27B: bad_effect=!Cyc_Tcutil_unify_effect((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp310))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp306))->v);goto _LL273;_LL273:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=({const char*_tmp38C="(function type effects do not unify)";
_tag_dynforward(_tmp38C,sizeof(char),_get_zero_arr_size(_tmp38C,37));});goto
_LL228;}if(!Cyc_Tcutil_same_atts(_tmp30C,_tmp316)){Cyc_Tcutil_failure_reason=({
const char*_tmp38D="(function types have different attributes)";_tag_dynforward(
_tmp38D,sizeof(char),_get_zero_arr_size(_tmp38D,43));});goto _LL228;}if(!Cyc_Tcutil_same_rgn_po(
_tmp30B,_tmp315)){Cyc_Tcutil_failure_reason=({const char*_tmp38E="(function types have different region lifetime orderings)";
_tag_dynforward(_tmp38E,sizeof(char),_get_zero_arr_size(_tmp38E,58));});goto
_LL228;}return;}}}_LL24B: _tmp317=_tmp29B.f1;if(_tmp317 <= (void*)4)goto _LL24D;if(*((
int*)_tmp317)!= 9)goto _LL24D;_tmp318=((struct Cyc_Absyn_TupleType_struct*)_tmp317)->f1;
_tmp319=_tmp29B.f2;if(_tmp319 <= (void*)4)goto _LL24D;if(*((int*)_tmp319)!= 9)goto
_LL24D;_tmp31A=((struct Cyc_Absyn_TupleType_struct*)_tmp319)->f1;_LL24C: for(0;
_tmp31A != 0  && _tmp318 != 0;(_tmp31A=_tmp31A->tl,_tmp318=_tmp318->tl)){struct Cyc_Absyn_Tqual
_tmp390;void*_tmp391;struct _tuple4 _tmp38F=*((struct _tuple4*)_tmp31A->hd);_tmp390=
_tmp38F.f1;_tmp391=_tmp38F.f2;{struct Cyc_Absyn_Tqual _tmp393;void*_tmp394;struct
_tuple4 _tmp392=*((struct _tuple4*)_tmp318->hd);_tmp393=_tmp392.f1;_tmp394=_tmp392.f2;
Cyc_Tcutil_unify_it(_tmp391,_tmp394);Cyc_Tcutil_unify_tqual(_tmp390,_tmp391,
_tmp393,_tmp394);}}if(_tmp31A == 0  && _tmp318 == 0)return;Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp395="(tuple types have different numbers of components)";
_tag_dynforward(_tmp395,sizeof(char),_get_zero_arr_size(_tmp395,51));});goto
_LL228;_LL24D: _tmp31B=_tmp29B.f1;if(_tmp31B <= (void*)4)goto _LL24F;if(*((int*)
_tmp31B)!= 11)goto _LL24F;_tmp31C=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp31B)->f1;_tmp31D=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp31B)->f2;_tmp31E=
_tmp29B.f2;if(_tmp31E <= (void*)4)goto _LL24F;if(*((int*)_tmp31E)!= 11)goto _LL24F;
_tmp31F=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp31E)->f1;_tmp320=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp31E)->f2;_LL24E: if(_tmp31F != _tmp31C){
Cyc_Tcutil_failure_reason=({const char*_tmp396="(struct and union type)";
_tag_dynforward(_tmp396,sizeof(char),_get_zero_arr_size(_tmp396,24));});goto
_LL228;}for(0;_tmp320 != 0  && _tmp31D != 0;(_tmp320=_tmp320->tl,_tmp31D=_tmp31D->tl)){
struct Cyc_Absyn_Aggrfield*_tmp397=(struct Cyc_Absyn_Aggrfield*)_tmp320->hd;struct
Cyc_Absyn_Aggrfield*_tmp398=(struct Cyc_Absyn_Aggrfield*)_tmp31D->hd;if(Cyc_strptrcmp(
_tmp397->name,_tmp398->name)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp399="(different member names)";
_tag_dynforward(_tmp399,sizeof(char),_get_zero_arr_size(_tmp399,25));});(int)
_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_unify_it((void*)_tmp397->type,(void*)
_tmp398->type);Cyc_Tcutil_unify_tqual(_tmp397->tq,(void*)_tmp397->type,_tmp398->tq,(
void*)_tmp398->type);if(!Cyc_Tcutil_same_atts(_tmp397->attributes,_tmp398->attributes)){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({
const char*_tmp39A="(different attributes on member)";_tag_dynforward(_tmp39A,
sizeof(char),_get_zero_arr_size(_tmp39A,33));});(int)_throw((void*)Cyc_Tcutil_Unify);}
if((_tmp397->width != 0  && _tmp398->width == 0  || _tmp398->width != 0  && _tmp397->width
== 0) || (_tmp397->width != 0  && _tmp398->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp397->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp398->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp39B="(different bitfield widths on member)";
_tag_dynforward(_tmp39B,sizeof(char),_get_zero_arr_size(_tmp39B,38));});(int)
_throw((void*)Cyc_Tcutil_Unify);}}if(_tmp320 == 0  && _tmp31D == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp39C="(different number of members)";
_tag_dynforward(_tmp39C,sizeof(char),_get_zero_arr_size(_tmp39C,30));});goto
_LL228;_LL24F: _tmp321=_tmp29B.f1;if((int)_tmp321 != 2)goto _LL251;_tmp322=_tmp29B.f2;
if((int)_tmp322 != 2)goto _LL251;_LL250: return;_LL251: _tmp323=_tmp29B.f1;if((int)
_tmp323 != 3)goto _LL253;_tmp324=_tmp29B.f2;if((int)_tmp324 != 3)goto _LL253;_LL252:
return;_LL253: _tmp325=_tmp29B.f1;if(_tmp325 <= (void*)4)goto _LL255;if(*((int*)
_tmp325)!= 15)goto _LL255;_tmp326=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp325)->f1;_tmp327=_tmp29B.f2;if(_tmp327 <= (void*)4)goto _LL255;if(*((int*)
_tmp327)!= 15)goto _LL255;_tmp328=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp327)->f1;_LL254: Cyc_Tcutil_unify_it(_tmp326,_tmp328);return;_LL255: _tmp329=
_tmp29B.f1;if(_tmp329 <= (void*)4)goto _LL257;if(*((int*)_tmp329)!= 20)goto _LL257;
_LL256: goto _LL258;_LL257: _tmp32A=_tmp29B.f2;if(_tmp32A <= (void*)4)goto _LL259;if(*((
int*)_tmp32A)!= 20)goto _LL259;_LL258: goto _LL25A;_LL259: _tmp32B=_tmp29B.f1;if(
_tmp32B <= (void*)4)goto _LL25B;if(*((int*)_tmp32B)!= 19)goto _LL25B;_LL25A: goto
_LL25C;_LL25B: _tmp32C=_tmp29B.f1;if(_tmp32C <= (void*)4)goto _LL25D;if(*((int*)
_tmp32C)!= 21)goto _LL25D;_LL25C: goto _LL25E;_LL25D: _tmp32D=_tmp29B.f2;if(_tmp32D
<= (void*)4)goto _LL25F;if(*((int*)_tmp32D)!= 21)goto _LL25F;_LL25E: goto _LL260;
_LL25F: _tmp32E=_tmp29B.f2;if(_tmp32E <= (void*)4)goto _LL261;if(*((int*)_tmp32E)!= 
19)goto _LL261;_LL260: if(Cyc_Tcutil_unify_effect(t1,t2))return;Cyc_Tcutil_failure_reason=({
const char*_tmp39D="(effects don't unify)";_tag_dynforward(_tmp39D,sizeof(char),
_get_zero_arr_size(_tmp39D,22));});goto _LL228;_LL261:;_LL262: goto _LL228;_LL228:;}(
int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),
void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2
== 0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int
Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int
_tmp39E=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int _tmp39F=(
tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp39E,_tmp39F);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(x);y=
Cyc_Absyn_compress_conref(y);if(x == y)return 0;{void*_tmp3A0=(void*)x->v;void*
_tmp3A1;_LL27D: if((int)_tmp3A0 != 0)goto _LL27F;_LL27E: return - 1;_LL27F: if(_tmp3A0
<= (void*)1)goto _LL281;if(*((int*)_tmp3A0)!= 0)goto _LL281;_tmp3A1=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp3A0)->f1;_LL280: {void*_tmp3A2=(void*)y->v;
void*_tmp3A3;_LL284: if((int)_tmp3A2 != 0)goto _LL286;_LL285: return 1;_LL286: if(
_tmp3A2 <= (void*)1)goto _LL288;if(*((int*)_tmp3A2)!= 0)goto _LL288;_tmp3A3=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp3A2)->f1;_LL287: return cmp(_tmp3A1,_tmp3A3);
_LL288: if(_tmp3A2 <= (void*)1)goto _LL283;if(*((int*)_tmp3A2)!= 1)goto _LL283;
_LL289:({void*_tmp3A4[0]={};Cyc_Tcutil_impos(({const char*_tmp3A5="unify_conref: forward after compress(2)";
_tag_dynforward(_tmp3A5,sizeof(char),_get_zero_arr_size(_tmp3A5,40));}),
_tag_dynforward(_tmp3A4,sizeof(void*),0));});_LL283:;}_LL281: if(_tmp3A0 <= (void*)
1)goto _LL27C;if(*((int*)_tmp3A0)!= 1)goto _LL27C;_LL282:({void*_tmp3A6[0]={};Cyc_Tcutil_impos(({
const char*_tmp3A7="unify_conref: forward after compress";_tag_dynforward(_tmp3A7,
sizeof(char),_get_zero_arr_size(_tmp3A7,37));}),_tag_dynforward(_tmp3A6,sizeof(
void*),0));});_LL27C:;}}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple4*tqt1,
struct _tuple4*tqt2){struct _tuple4 _tmp3A9;struct Cyc_Absyn_Tqual _tmp3AA;void*
_tmp3AB;struct _tuple4*_tmp3A8=tqt1;_tmp3A9=*_tmp3A8;_tmp3AA=_tmp3A9.f1;_tmp3AB=
_tmp3A9.f2;{struct _tuple4 _tmp3AD;struct Cyc_Absyn_Tqual _tmp3AE;void*_tmp3AF;
struct _tuple4*_tmp3AC=tqt2;_tmp3AD=*_tmp3AC;_tmp3AE=_tmp3AD.f1;_tmp3AF=_tmp3AD.f2;{
int _tmp3B0=Cyc_Tcutil_tqual_cmp(_tmp3AA,_tmp3AE);if(_tmp3B0 != 0)return _tmp3B0;
return Cyc_Tcutil_typecmp(_tmp3AB,_tmp3AF);}}}static int Cyc_Tcutil_aggrfield_cmp(
struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp3B1=Cyc_strptrcmp(
f1->name,f2->name);if(_tmp3B1 != 0)return _tmp3B1;{int _tmp3B2=Cyc_Tcutil_tqual_cmp(
f1->tq,f2->tq);if(_tmp3B2 != 0)return _tmp3B2;{int _tmp3B3=Cyc_Tcutil_typecmp((void*)
f1->type,(void*)f2->type);if(_tmp3B3 != 0)return _tmp3B3;{int _tmp3B4=Cyc_List_list_cmp(
Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);if(_tmp3B4 != 0)return
_tmp3B4;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
f1->width,f2->width);}}}}static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*
e1,struct Cyc_Absyn_Enumfield*e2){int _tmp3B5=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp3B5 != 0)return _tmp3B5;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp3B6=t;_LL28B: if((int)_tmp3B6 != 0)goto _LL28D;_LL28C: return 0;
_LL28D: if(_tmp3B6 <= (void*)4)goto _LL299;if(*((int*)_tmp3B6)!= 0)goto _LL28F;
_LL28E: return 1;_LL28F: if(*((int*)_tmp3B6)!= 1)goto _LL291;_LL290: return 2;_LL291:
if(*((int*)_tmp3B6)!= 2)goto _LL293;_LL292: return 3;_LL293: if(*((int*)_tmp3B6)!= 3)
goto _LL295;_LL294: return 4;_LL295: if(*((int*)_tmp3B6)!= 4)goto _LL297;_LL296:
return 5;_LL297: if(*((int*)_tmp3B6)!= 5)goto _LL299;_LL298: return 6;_LL299: if((int)
_tmp3B6 != 1)goto _LL29B;_LL29A: return 7;_LL29B: if(_tmp3B6 <= (void*)4)goto _LL2AF;
if(*((int*)_tmp3B6)!= 6)goto _LL29D;_LL29C: return 8;_LL29D: if(*((int*)_tmp3B6)!= 7)
goto _LL29F;_LL29E: return 9;_LL29F: if(*((int*)_tmp3B6)!= 8)goto _LL2A1;_LL2A0:
return 10;_LL2A1: if(*((int*)_tmp3B6)!= 9)goto _LL2A3;_LL2A2: return 11;_LL2A3: if(*((
int*)_tmp3B6)!= 10)goto _LL2A5;_LL2A4: return 12;_LL2A5: if(*((int*)_tmp3B6)!= 11)
goto _LL2A7;_LL2A6: return 14;_LL2A7: if(*((int*)_tmp3B6)!= 12)goto _LL2A9;_LL2A8:
return 16;_LL2A9: if(*((int*)_tmp3B6)!= 13)goto _LL2AB;_LL2AA: return 17;_LL2AB: if(*((
int*)_tmp3B6)!= 15)goto _LL2AD;_LL2AC: return 18;_LL2AD: if(*((int*)_tmp3B6)!= 16)
goto _LL2AF;_LL2AE: return 19;_LL2AF: if((int)_tmp3B6 != 3)goto _LL2B1;_LL2B0: return 20;
_LL2B1: if((int)_tmp3B6 != 2)goto _LL2B3;_LL2B2: return 21;_LL2B3: if(_tmp3B6 <= (void*)
4)goto _LL2B5;if(*((int*)_tmp3B6)!= 19)goto _LL2B5;_LL2B4: return 22;_LL2B5: if(
_tmp3B6 <= (void*)4)goto _LL2B7;if(*((int*)_tmp3B6)!= 20)goto _LL2B7;_LL2B6: return
23;_LL2B7: if(_tmp3B6 <= (void*)4)goto _LL2B9;if(*((int*)_tmp3B6)!= 21)goto _LL2B9;
_LL2B8: return 24;_LL2B9: if(_tmp3B6 <= (void*)4)goto _LL2BB;if(*((int*)_tmp3B6)!= 14)
goto _LL2BB;_LL2BA: return 25;_LL2BB: if(_tmp3B6 <= (void*)4)goto _LL2BD;if(*((int*)
_tmp3B6)!= 18)goto _LL2BD;_LL2BC: return 26;_LL2BD: if(_tmp3B6 <= (void*)4)goto _LL28A;
if(*((int*)_tmp3B6)!= 17)goto _LL28A;_LL2BE: return 27;_LL28A:;}int Cyc_Tcutil_typecmp(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == 
t2)return 0;{int _tmp3B7=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp3B7 != 0)return _tmp3B7;{struct _tuple6 _tmp3B9=({struct _tuple6 _tmp3B8;
_tmp3B8.f1=t2;_tmp3B8.f2=t1;_tmp3B8;});void*_tmp3BA;void*_tmp3BB;void*_tmp3BC;
struct Cyc_Absyn_Tvar*_tmp3BD;void*_tmp3BE;struct Cyc_Absyn_Tvar*_tmp3BF;void*
_tmp3C0;struct Cyc_Absyn_AggrInfo _tmp3C1;void*_tmp3C2;struct Cyc_List_List*_tmp3C3;
void*_tmp3C4;struct Cyc_Absyn_AggrInfo _tmp3C5;void*_tmp3C6;struct Cyc_List_List*
_tmp3C7;void*_tmp3C8;struct _tuple1*_tmp3C9;void*_tmp3CA;struct _tuple1*_tmp3CB;
void*_tmp3CC;struct Cyc_List_List*_tmp3CD;void*_tmp3CE;struct Cyc_List_List*
_tmp3CF;void*_tmp3D0;struct Cyc_Absyn_TunionInfo _tmp3D1;void*_tmp3D2;struct Cyc_Absyn_Tuniondecl**
_tmp3D3;struct Cyc_Absyn_Tuniondecl*_tmp3D4;struct Cyc_List_List*_tmp3D5;struct Cyc_Core_Opt*
_tmp3D6;void*_tmp3D7;struct Cyc_Absyn_TunionInfo _tmp3D8;void*_tmp3D9;struct Cyc_Absyn_Tuniondecl**
_tmp3DA;struct Cyc_Absyn_Tuniondecl*_tmp3DB;struct Cyc_List_List*_tmp3DC;struct Cyc_Core_Opt*
_tmp3DD;void*_tmp3DE;struct Cyc_Absyn_TunionFieldInfo _tmp3DF;void*_tmp3E0;struct
Cyc_Absyn_Tuniondecl*_tmp3E1;struct Cyc_Absyn_Tunionfield*_tmp3E2;struct Cyc_List_List*
_tmp3E3;void*_tmp3E4;struct Cyc_Absyn_TunionFieldInfo _tmp3E5;void*_tmp3E6;struct
Cyc_Absyn_Tuniondecl*_tmp3E7;struct Cyc_Absyn_Tunionfield*_tmp3E8;struct Cyc_List_List*
_tmp3E9;void*_tmp3EA;struct Cyc_Absyn_PtrInfo _tmp3EB;void*_tmp3EC;struct Cyc_Absyn_Tqual
_tmp3ED;struct Cyc_Absyn_PtrAtts _tmp3EE;void*_tmp3EF;struct Cyc_Absyn_Conref*
_tmp3F0;struct Cyc_Absyn_Conref*_tmp3F1;struct Cyc_Absyn_Conref*_tmp3F2;void*
_tmp3F3;struct Cyc_Absyn_PtrInfo _tmp3F4;void*_tmp3F5;struct Cyc_Absyn_Tqual _tmp3F6;
struct Cyc_Absyn_PtrAtts _tmp3F7;void*_tmp3F8;struct Cyc_Absyn_Conref*_tmp3F9;
struct Cyc_Absyn_Conref*_tmp3FA;struct Cyc_Absyn_Conref*_tmp3FB;void*_tmp3FC;void*
_tmp3FD;void*_tmp3FE;void*_tmp3FF;void*_tmp400;void*_tmp401;void*_tmp402;void*
_tmp403;void*_tmp404;int _tmp405;void*_tmp406;int _tmp407;void*_tmp408;struct Cyc_Absyn_ArrayInfo
_tmp409;void*_tmp40A;struct Cyc_Absyn_Tqual _tmp40B;struct Cyc_Absyn_Exp*_tmp40C;
struct Cyc_Absyn_Conref*_tmp40D;void*_tmp40E;struct Cyc_Absyn_ArrayInfo _tmp40F;
void*_tmp410;struct Cyc_Absyn_Tqual _tmp411;struct Cyc_Absyn_Exp*_tmp412;struct Cyc_Absyn_Conref*
_tmp413;void*_tmp414;struct Cyc_Absyn_FnInfo _tmp415;struct Cyc_List_List*_tmp416;
struct Cyc_Core_Opt*_tmp417;void*_tmp418;struct Cyc_List_List*_tmp419;int _tmp41A;
struct Cyc_Absyn_VarargInfo*_tmp41B;struct Cyc_List_List*_tmp41C;struct Cyc_List_List*
_tmp41D;void*_tmp41E;struct Cyc_Absyn_FnInfo _tmp41F;struct Cyc_List_List*_tmp420;
struct Cyc_Core_Opt*_tmp421;void*_tmp422;struct Cyc_List_List*_tmp423;int _tmp424;
struct Cyc_Absyn_VarargInfo*_tmp425;struct Cyc_List_List*_tmp426;struct Cyc_List_List*
_tmp427;void*_tmp428;struct Cyc_List_List*_tmp429;void*_tmp42A;struct Cyc_List_List*
_tmp42B;void*_tmp42C;void*_tmp42D;struct Cyc_List_List*_tmp42E;void*_tmp42F;void*
_tmp430;struct Cyc_List_List*_tmp431;void*_tmp432;void*_tmp433;void*_tmp434;void*
_tmp435;void*_tmp436;void*_tmp437;void*_tmp438;void*_tmp439;void*_tmp43A;void*
_tmp43B;void*_tmp43C;void*_tmp43D;void*_tmp43E;int _tmp43F;void*_tmp440;int
_tmp441;void*_tmp442;void*_tmp443;void*_tmp444;void*_tmp445;void*_tmp446;void*
_tmp447;_LL2C0: _tmp3BA=_tmp3B9.f1;if(_tmp3BA <= (void*)4)goto _LL2C2;if(*((int*)
_tmp3BA)!= 0)goto _LL2C2;_tmp3BB=_tmp3B9.f2;if(_tmp3BB <= (void*)4)goto _LL2C2;if(*((
int*)_tmp3BB)!= 0)goto _LL2C2;_LL2C1:({void*_tmp448[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp449="typecmp: can only compare closed types";_tag_dynforward(_tmp449,sizeof(
char),_get_zero_arr_size(_tmp449,39));}),_tag_dynforward(_tmp448,sizeof(void*),0));});
_LL2C2: _tmp3BC=_tmp3B9.f1;if(_tmp3BC <= (void*)4)goto _LL2C4;if(*((int*)_tmp3BC)!= 
1)goto _LL2C4;_tmp3BD=((struct Cyc_Absyn_VarType_struct*)_tmp3BC)->f1;_tmp3BE=
_tmp3B9.f2;if(_tmp3BE <= (void*)4)goto _LL2C4;if(*((int*)_tmp3BE)!= 1)goto _LL2C4;
_tmp3BF=((struct Cyc_Absyn_VarType_struct*)_tmp3BE)->f1;_LL2C3: return Cyc_Core_intcmp(*((
int*)_check_null(_tmp3BF->identity)),*((int*)_check_null(_tmp3BD->identity)));
_LL2C4: _tmp3C0=_tmp3B9.f1;if(_tmp3C0 <= (void*)4)goto _LL2C6;if(*((int*)_tmp3C0)!= 
10)goto _LL2C6;_tmp3C1=((struct Cyc_Absyn_AggrType_struct*)_tmp3C0)->f1;_tmp3C2=(
void*)_tmp3C1.aggr_info;_tmp3C3=_tmp3C1.targs;_tmp3C4=_tmp3B9.f2;if(_tmp3C4 <= (
void*)4)goto _LL2C6;if(*((int*)_tmp3C4)!= 10)goto _LL2C6;_tmp3C5=((struct Cyc_Absyn_AggrType_struct*)
_tmp3C4)->f1;_tmp3C6=(void*)_tmp3C5.aggr_info;_tmp3C7=_tmp3C5.targs;_LL2C5: {
struct _tuple1*_tmp44B;struct _tuple5 _tmp44A=Cyc_Absyn_aggr_kinded_name(_tmp3C2);
_tmp44B=_tmp44A.f2;{struct _tuple1*_tmp44D;struct _tuple5 _tmp44C=Cyc_Absyn_aggr_kinded_name(
_tmp3C6);_tmp44D=_tmp44C.f2;{int _tmp44E=Cyc_Absyn_qvar_cmp(_tmp44B,_tmp44D);if(
_tmp44E != 0)return _tmp44E;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3C3,_tmp3C7);}}}}_LL2C6: _tmp3C8=_tmp3B9.f1;if(_tmp3C8 <= (void*)4)goto _LL2C8;
if(*((int*)_tmp3C8)!= 12)goto _LL2C8;_tmp3C9=((struct Cyc_Absyn_EnumType_struct*)
_tmp3C8)->f1;_tmp3CA=_tmp3B9.f2;if(_tmp3CA <= (void*)4)goto _LL2C8;if(*((int*)
_tmp3CA)!= 12)goto _LL2C8;_tmp3CB=((struct Cyc_Absyn_EnumType_struct*)_tmp3CA)->f1;
_LL2C7: return Cyc_Absyn_qvar_cmp(_tmp3C9,_tmp3CB);_LL2C8: _tmp3CC=_tmp3B9.f1;if(
_tmp3CC <= (void*)4)goto _LL2CA;if(*((int*)_tmp3CC)!= 13)goto _LL2CA;_tmp3CD=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp3CC)->f1;_tmp3CE=_tmp3B9.f2;if(_tmp3CE
<= (void*)4)goto _LL2CA;if(*((int*)_tmp3CE)!= 13)goto _LL2CA;_tmp3CF=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3CE)->f1;_LL2C9: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct
Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp3CD,_tmp3CF);_LL2CA: _tmp3D0=_tmp3B9.f1;if(_tmp3D0 <= (
void*)4)goto _LL2CC;if(*((int*)_tmp3D0)!= 2)goto _LL2CC;_tmp3D1=((struct Cyc_Absyn_TunionType_struct*)
_tmp3D0)->f1;_tmp3D2=(void*)_tmp3D1.tunion_info;if(*((int*)_tmp3D2)!= 1)goto
_LL2CC;_tmp3D3=((struct Cyc_Absyn_KnownTunion_struct*)_tmp3D2)->f1;_tmp3D4=*
_tmp3D3;_tmp3D5=_tmp3D1.targs;_tmp3D6=_tmp3D1.rgn;_tmp3D7=_tmp3B9.f2;if(_tmp3D7
<= (void*)4)goto _LL2CC;if(*((int*)_tmp3D7)!= 2)goto _LL2CC;_tmp3D8=((struct Cyc_Absyn_TunionType_struct*)
_tmp3D7)->f1;_tmp3D9=(void*)_tmp3D8.tunion_info;if(*((int*)_tmp3D9)!= 1)goto
_LL2CC;_tmp3DA=((struct Cyc_Absyn_KnownTunion_struct*)_tmp3D9)->f1;_tmp3DB=*
_tmp3DA;_tmp3DC=_tmp3D8.targs;_tmp3DD=_tmp3D8.rgn;_LL2CB: if(_tmp3DB == _tmp3D4)
return 0;{int _tmp44F=Cyc_Absyn_qvar_cmp(_tmp3DB->name,_tmp3D4->name);if(_tmp44F != 
0)return _tmp44F;if((unsigned int)_tmp3DD  && (unsigned int)_tmp3D6){int _tmp450=
Cyc_Tcutil_typecmp((void*)_tmp3DD->v,(void*)_tmp3D6->v);if(_tmp450 != 0)return
_tmp450;}else{if((unsigned int)_tmp3DD)return - 1;else{return 1;}}return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp3DC,_tmp3D5);}_LL2CC: _tmp3DE=_tmp3B9.f1;if(_tmp3DE <= (
void*)4)goto _LL2CE;if(*((int*)_tmp3DE)!= 3)goto _LL2CE;_tmp3DF=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3DE)->f1;_tmp3E0=(void*)_tmp3DF.field_info;if(*((int*)_tmp3E0)!= 1)goto
_LL2CE;_tmp3E1=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3E0)->f1;_tmp3E2=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3E0)->f2;_tmp3E3=_tmp3DF.targs;
_tmp3E4=_tmp3B9.f2;if(_tmp3E4 <= (void*)4)goto _LL2CE;if(*((int*)_tmp3E4)!= 3)goto
_LL2CE;_tmp3E5=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp3E4)->f1;_tmp3E6=(
void*)_tmp3E5.field_info;if(*((int*)_tmp3E6)!= 1)goto _LL2CE;_tmp3E7=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp3E6)->f1;_tmp3E8=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3E6)->f2;
_tmp3E9=_tmp3E5.targs;_LL2CD: if(_tmp3E7 == _tmp3E1)return 0;{int _tmp451=Cyc_Absyn_qvar_cmp(
_tmp3E1->name,_tmp3E7->name);if(_tmp451 != 0)return _tmp451;{int _tmp452=Cyc_Absyn_qvar_cmp(
_tmp3E2->name,_tmp3E8->name);if(_tmp452 != 0)return _tmp452;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp3E9,_tmp3E3);}}_LL2CE: _tmp3EA=_tmp3B9.f1;if(_tmp3EA <= (
void*)4)goto _LL2D0;if(*((int*)_tmp3EA)!= 4)goto _LL2D0;_tmp3EB=((struct Cyc_Absyn_PointerType_struct*)
_tmp3EA)->f1;_tmp3EC=(void*)_tmp3EB.elt_typ;_tmp3ED=_tmp3EB.elt_tq;_tmp3EE=
_tmp3EB.ptr_atts;_tmp3EF=(void*)_tmp3EE.rgn;_tmp3F0=_tmp3EE.nullable;_tmp3F1=
_tmp3EE.bounds;_tmp3F2=_tmp3EE.zero_term;_tmp3F3=_tmp3B9.f2;if(_tmp3F3 <= (void*)
4)goto _LL2D0;if(*((int*)_tmp3F3)!= 4)goto _LL2D0;_tmp3F4=((struct Cyc_Absyn_PointerType_struct*)
_tmp3F3)->f1;_tmp3F5=(void*)_tmp3F4.elt_typ;_tmp3F6=_tmp3F4.elt_tq;_tmp3F7=
_tmp3F4.ptr_atts;_tmp3F8=(void*)_tmp3F7.rgn;_tmp3F9=_tmp3F7.nullable;_tmp3FA=
_tmp3F7.bounds;_tmp3FB=_tmp3F7.zero_term;_LL2CF: {int _tmp453=Cyc_Tcutil_typecmp(
_tmp3F5,_tmp3EC);if(_tmp453 != 0)return _tmp453;{int _tmp454=Cyc_Tcutil_typecmp(
_tmp3F8,_tmp3EF);if(_tmp454 != 0)return _tmp454;{int _tmp455=Cyc_Tcutil_tqual_cmp(
_tmp3F6,_tmp3ED);if(_tmp455 != 0)return _tmp455;{int _tmp456=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp3FA,_tmp3F1);if(_tmp456 != 0)return _tmp456;{int _tmp457=((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3FB,_tmp3F2);if(_tmp457 != 0)return _tmp457;{void*_tmp458=(
void*)(Cyc_Absyn_compress_conref(_tmp3FA))->v;void*_tmp459;void*_tmp45A;_LL2F5:
if(_tmp458 <= (void*)1)goto _LL2F9;if(*((int*)_tmp458)!= 0)goto _LL2F7;_tmp459=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp458)->f1;if((int)_tmp459 != 0)goto
_LL2F7;_LL2F6: return 0;_LL2F7: if(*((int*)_tmp458)!= 0)goto _LL2F9;_tmp45A=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp458)->f1;if((int)_tmp45A != 1)goto _LL2F9;
_LL2F8: return 0;_LL2F9:;_LL2FA: goto _LL2F4;_LL2F4:;}return((int(*)(int(*cmp)(int,
int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3F9,_tmp3F0);}}}}}_LL2D0: _tmp3FC=_tmp3B9.f1;if(_tmp3FC <= (
void*)4)goto _LL2D2;if(*((int*)_tmp3FC)!= 5)goto _LL2D2;_tmp3FD=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3FC)->f1;_tmp3FE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3FC)->f2;
_tmp3FF=_tmp3B9.f2;if(_tmp3FF <= (void*)4)goto _LL2D2;if(*((int*)_tmp3FF)!= 5)goto
_LL2D2;_tmp400=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3FF)->f1;_tmp401=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp3FF)->f2;_LL2D1: if(_tmp400 != _tmp3FD)
return Cyc_Core_intcmp((int)((unsigned int)_tmp400),(int)((unsigned int)_tmp3FD));
if(_tmp401 != _tmp3FE)return Cyc_Core_intcmp((int)((unsigned int)_tmp401),(int)((
unsigned int)_tmp3FE));return 0;_LL2D2: _tmp402=_tmp3B9.f1;if((int)_tmp402 != 1)
goto _LL2D4;_tmp403=_tmp3B9.f2;if((int)_tmp403 != 1)goto _LL2D4;_LL2D3: return 0;
_LL2D4: _tmp404=_tmp3B9.f1;if(_tmp404 <= (void*)4)goto _LL2D6;if(*((int*)_tmp404)!= 
6)goto _LL2D6;_tmp405=((struct Cyc_Absyn_DoubleType_struct*)_tmp404)->f1;_tmp406=
_tmp3B9.f2;if(_tmp406 <= (void*)4)goto _LL2D6;if(*((int*)_tmp406)!= 6)goto _LL2D6;
_tmp407=((struct Cyc_Absyn_DoubleType_struct*)_tmp406)->f1;_LL2D5: if(_tmp405 == 
_tmp407)return 0;else{if(_tmp405)return - 1;else{return 1;}}_LL2D6: _tmp408=_tmp3B9.f1;
if(_tmp408 <= (void*)4)goto _LL2D8;if(*((int*)_tmp408)!= 7)goto _LL2D8;_tmp409=((
struct Cyc_Absyn_ArrayType_struct*)_tmp408)->f1;_tmp40A=(void*)_tmp409.elt_type;
_tmp40B=_tmp409.tq;_tmp40C=_tmp409.num_elts;_tmp40D=_tmp409.zero_term;_tmp40E=
_tmp3B9.f2;if(_tmp40E <= (void*)4)goto _LL2D8;if(*((int*)_tmp40E)!= 7)goto _LL2D8;
_tmp40F=((struct Cyc_Absyn_ArrayType_struct*)_tmp40E)->f1;_tmp410=(void*)_tmp40F.elt_type;
_tmp411=_tmp40F.tq;_tmp412=_tmp40F.num_elts;_tmp413=_tmp40F.zero_term;_LL2D7: {
int _tmp45B=Cyc_Tcutil_tqual_cmp(_tmp411,_tmp40B);if(_tmp45B != 0)return _tmp45B;{
int _tmp45C=Cyc_Tcutil_typecmp(_tmp410,_tmp40A);if(_tmp45C != 0)return _tmp45C;{int
_tmp45D=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp40D,_tmp413);if(_tmp45D != 0)
return _tmp45D;if(_tmp40C == _tmp412)return 0;if(_tmp40C == 0  || _tmp412 == 0)({void*
_tmp45E[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp45F="missing expression in array index";_tag_dynforward(_tmp45F,
sizeof(char),_get_zero_arr_size(_tmp45F,34));}),_tag_dynforward(_tmp45E,sizeof(
void*),0));});return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp40C,_tmp412);}}}_LL2D8: _tmp414=_tmp3B9.f1;if(_tmp414 <= (void*)4)goto _LL2DA;
if(*((int*)_tmp414)!= 8)goto _LL2DA;_tmp415=((struct Cyc_Absyn_FnType_struct*)
_tmp414)->f1;_tmp416=_tmp415.tvars;_tmp417=_tmp415.effect;_tmp418=(void*)_tmp415.ret_typ;
_tmp419=_tmp415.args;_tmp41A=_tmp415.c_varargs;_tmp41B=_tmp415.cyc_varargs;
_tmp41C=_tmp415.rgn_po;_tmp41D=_tmp415.attributes;_tmp41E=_tmp3B9.f2;if(_tmp41E
<= (void*)4)goto _LL2DA;if(*((int*)_tmp41E)!= 8)goto _LL2DA;_tmp41F=((struct Cyc_Absyn_FnType_struct*)
_tmp41E)->f1;_tmp420=_tmp41F.tvars;_tmp421=_tmp41F.effect;_tmp422=(void*)_tmp41F.ret_typ;
_tmp423=_tmp41F.args;_tmp424=_tmp41F.c_varargs;_tmp425=_tmp41F.cyc_varargs;
_tmp426=_tmp41F.rgn_po;_tmp427=_tmp41F.attributes;_LL2D9:({void*_tmp460[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp461="typecmp: function types not handled";_tag_dynforward(_tmp461,
sizeof(char),_get_zero_arr_size(_tmp461,36));}),_tag_dynforward(_tmp460,sizeof(
void*),0));});_LL2DA: _tmp428=_tmp3B9.f1;if(_tmp428 <= (void*)4)goto _LL2DC;if(*((
int*)_tmp428)!= 9)goto _LL2DC;_tmp429=((struct Cyc_Absyn_TupleType_struct*)_tmp428)->f1;
_tmp42A=_tmp3B9.f2;if(_tmp42A <= (void*)4)goto _LL2DC;if(*((int*)_tmp42A)!= 9)goto
_LL2DC;_tmp42B=((struct Cyc_Absyn_TupleType_struct*)_tmp42A)->f1;_LL2DB: return((
int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp42B,_tmp429);_LL2DC: _tmp42C=
_tmp3B9.f1;if(_tmp42C <= (void*)4)goto _LL2DE;if(*((int*)_tmp42C)!= 11)goto _LL2DE;
_tmp42D=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp42C)->f1;_tmp42E=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp42C)->f2;_tmp42F=_tmp3B9.f2;if(_tmp42F
<= (void*)4)goto _LL2DE;if(*((int*)_tmp42F)!= 11)goto _LL2DE;_tmp430=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp42F)->f1;_tmp431=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp42F)->f2;_LL2DD: if(_tmp430 != _tmp42D){if(_tmp430 == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp431,_tmp42E);_LL2DE: _tmp432=_tmp3B9.f1;if(_tmp432 <= (void*)4)goto _LL2E0;if(*((
int*)_tmp432)!= 15)goto _LL2E0;_tmp433=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp432)->f1;_tmp434=_tmp3B9.f2;if(_tmp434 <= (void*)4)goto _LL2E0;if(*((int*)
_tmp434)!= 15)goto _LL2E0;_tmp435=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp434)->f1;_LL2DF: return Cyc_Tcutil_typecmp(_tmp433,_tmp435);_LL2E0: _tmp436=
_tmp3B9.f1;if(_tmp436 <= (void*)4)goto _LL2E2;if(*((int*)_tmp436)!= 14)goto _LL2E2;
_tmp437=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp436)->f1;_tmp438=_tmp3B9.f2;
if(_tmp438 <= (void*)4)goto _LL2E2;if(*((int*)_tmp438)!= 14)goto _LL2E2;_tmp439=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp438)->f1;_LL2E1: return Cyc_Tcutil_typecmp(
_tmp437,_tmp439);_LL2E2: _tmp43A=_tmp3B9.f1;if(_tmp43A <= (void*)4)goto _LL2E4;if(*((
int*)_tmp43A)!= 17)goto _LL2E4;_tmp43B=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp43A)->f1;_tmp43C=_tmp3B9.f2;if(_tmp43C <= (void*)4)goto _LL2E4;if(*((int*)
_tmp43C)!= 17)goto _LL2E4;_tmp43D=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp43C)->f1;_LL2E3: return Cyc_Tcutil_typecmp(_tmp43B,_tmp43D);_LL2E4: _tmp43E=
_tmp3B9.f1;if(_tmp43E <= (void*)4)goto _LL2E6;if(*((int*)_tmp43E)!= 18)goto _LL2E6;
_tmp43F=((struct Cyc_Absyn_TypeInt_struct*)_tmp43E)->f1;_tmp440=_tmp3B9.f2;if(
_tmp440 <= (void*)4)goto _LL2E6;if(*((int*)_tmp440)!= 18)goto _LL2E6;_tmp441=((
struct Cyc_Absyn_TypeInt_struct*)_tmp440)->f1;_LL2E5: return Cyc_Core_intcmp(
_tmp43F,_tmp441);_LL2E6: _tmp442=_tmp3B9.f1;if(_tmp442 <= (void*)4)goto _LL2E8;if(*((
int*)_tmp442)!= 20)goto _LL2E8;_LL2E7: goto _LL2E9;_LL2E8: _tmp443=_tmp3B9.f2;if(
_tmp443 <= (void*)4)goto _LL2EA;if(*((int*)_tmp443)!= 20)goto _LL2EA;_LL2E9: goto
_LL2EB;_LL2EA: _tmp444=_tmp3B9.f1;if(_tmp444 <= (void*)4)goto _LL2EC;if(*((int*)
_tmp444)!= 19)goto _LL2EC;_LL2EB: goto _LL2ED;_LL2EC: _tmp445=_tmp3B9.f1;if(_tmp445
<= (void*)4)goto _LL2EE;if(*((int*)_tmp445)!= 21)goto _LL2EE;_LL2ED: goto _LL2EF;
_LL2EE: _tmp446=_tmp3B9.f2;if(_tmp446 <= (void*)4)goto _LL2F0;if(*((int*)_tmp446)!= 
21)goto _LL2F0;_LL2EF: goto _LL2F1;_LL2F0: _tmp447=_tmp3B9.f2;if(_tmp447 <= (void*)4)
goto _LL2F2;if(*((int*)_tmp447)!= 19)goto _LL2F2;_LL2F1:({void*_tmp462[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp463="typecmp: effects not handled";_tag_dynforward(_tmp463,sizeof(char),
_get_zero_arr_size(_tmp463,29));}),_tag_dynforward(_tmp462,sizeof(void*),0));});
_LL2F2:;_LL2F3:({void*_tmp464[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp465="Unmatched case in typecmp";
_tag_dynforward(_tmp465,sizeof(char),_get_zero_arr_size(_tmp465,26));}),
_tag_dynforward(_tmp464,sizeof(void*),0));});_LL2BF:;}}}int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp466=Cyc_Tcutil_compress(t);_LL2FC: if(_tmp466 <= (void*)4)goto
_LL2FE;if(*((int*)_tmp466)!= 5)goto _LL2FE;_LL2FD: goto _LL2FF;_LL2FE: if((int)
_tmp466 != 1)goto _LL300;_LL2FF: goto _LL301;_LL300: if(_tmp466 <= (void*)4)goto _LL306;
if(*((int*)_tmp466)!= 6)goto _LL302;_LL301: goto _LL303;_LL302: if(*((int*)_tmp466)
!= 12)goto _LL304;_LL303: goto _LL305;_LL304: if(*((int*)_tmp466)!= 13)goto _LL306;
_LL305: return 1;_LL306:;_LL307: return 0;_LL2FB:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple6 _tmp468=({struct _tuple6 _tmp467;_tmp467.f1=t1;_tmp467.f2=t2;_tmp467;});
void*_tmp469;int _tmp46A;void*_tmp46B;int _tmp46C;void*_tmp46D;void*_tmp46E;void*
_tmp46F;void*_tmp470;void*_tmp471;void*_tmp472;void*_tmp473;void*_tmp474;void*
_tmp475;void*_tmp476;void*_tmp477;void*_tmp478;void*_tmp479;void*_tmp47A;void*
_tmp47B;void*_tmp47C;void*_tmp47D;void*_tmp47E;void*_tmp47F;void*_tmp480;void*
_tmp481;void*_tmp482;void*_tmp483;void*_tmp484;void*_tmp485;void*_tmp486;void*
_tmp487;void*_tmp488;void*_tmp489;void*_tmp48A;void*_tmp48B;void*_tmp48C;void*
_tmp48D;void*_tmp48E;void*_tmp48F;void*_tmp490;void*_tmp491;void*_tmp492;void*
_tmp493;void*_tmp494;void*_tmp495;void*_tmp496;void*_tmp497;void*_tmp498;void*
_tmp499;void*_tmp49A;void*_tmp49B;void*_tmp49C;void*_tmp49D;void*_tmp49E;void*
_tmp49F;void*_tmp4A0;void*_tmp4A1;void*_tmp4A2;void*_tmp4A3;void*_tmp4A4;void*
_tmp4A5;void*_tmp4A6;void*_tmp4A7;void*_tmp4A8;void*_tmp4A9;void*_tmp4AA;void*
_tmp4AB;void*_tmp4AC;void*_tmp4AD;void*_tmp4AE;_LL309: _tmp469=_tmp468.f1;if(
_tmp469 <= (void*)4)goto _LL30B;if(*((int*)_tmp469)!= 6)goto _LL30B;_tmp46A=((
struct Cyc_Absyn_DoubleType_struct*)_tmp469)->f1;_tmp46B=_tmp468.f2;if(_tmp46B <= (
void*)4)goto _LL30B;if(*((int*)_tmp46B)!= 6)goto _LL30B;_tmp46C=((struct Cyc_Absyn_DoubleType_struct*)
_tmp46B)->f1;_LL30A: return !_tmp46C  && _tmp46A;_LL30B: _tmp46D=_tmp468.f1;if(
_tmp46D <= (void*)4)goto _LL30D;if(*((int*)_tmp46D)!= 6)goto _LL30D;_tmp46E=_tmp468.f2;
if((int)_tmp46E != 1)goto _LL30D;_LL30C: goto _LL30E;_LL30D: _tmp46F=_tmp468.f1;if(
_tmp46F <= (void*)4)goto _LL30F;if(*((int*)_tmp46F)!= 6)goto _LL30F;_tmp470=_tmp468.f2;
if(_tmp470 <= (void*)4)goto _LL30F;if(*((int*)_tmp470)!= 5)goto _LL30F;_LL30E: goto
_LL310;_LL30F: _tmp471=_tmp468.f1;if(_tmp471 <= (void*)4)goto _LL311;if(*((int*)
_tmp471)!= 6)goto _LL311;_tmp472=_tmp468.f2;if(_tmp472 <= (void*)4)goto _LL311;if(*((
int*)_tmp472)!= 14)goto _LL311;_LL310: goto _LL312;_LL311: _tmp473=_tmp468.f1;if((
int)_tmp473 != 1)goto _LL313;_tmp474=_tmp468.f2;if(_tmp474 <= (void*)4)goto _LL313;
if(*((int*)_tmp474)!= 14)goto _LL313;_LL312: goto _LL314;_LL313: _tmp475=_tmp468.f1;
if(_tmp475 <= (void*)4)goto _LL315;if(*((int*)_tmp475)!= 6)goto _LL315;_tmp476=
_tmp468.f2;if(_tmp476 <= (void*)4)goto _LL315;if(*((int*)_tmp476)!= 17)goto _LL315;
_LL314: goto _LL316;_LL315: _tmp477=_tmp468.f1;if((int)_tmp477 != 1)goto _LL317;
_tmp478=_tmp468.f2;if(_tmp478 <= (void*)4)goto _LL317;if(*((int*)_tmp478)!= 17)
goto _LL317;_LL316: goto _LL318;_LL317: _tmp479=_tmp468.f1;if((int)_tmp479 != 1)goto
_LL319;_tmp47A=_tmp468.f2;if(_tmp47A <= (void*)4)goto _LL319;if(*((int*)_tmp47A)!= 
5)goto _LL319;_LL318: return 1;_LL319: _tmp47B=_tmp468.f1;if(_tmp47B <= (void*)4)goto
_LL31B;if(*((int*)_tmp47B)!= 5)goto _LL31B;_tmp47C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp47B)->f2;if((int)_tmp47C != 4)goto _LL31B;_tmp47D=_tmp468.f2;if(_tmp47D <= (
void*)4)goto _LL31B;if(*((int*)_tmp47D)!= 5)goto _LL31B;_tmp47E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp47D)->f2;if((int)_tmp47E != 4)goto _LL31B;_LL31A: return 0;_LL31B: _tmp47F=
_tmp468.f1;if(_tmp47F <= (void*)4)goto _LL31D;if(*((int*)_tmp47F)!= 5)goto _LL31D;
_tmp480=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp47F)->f2;if((int)_tmp480 != 
4)goto _LL31D;_LL31C: return 1;_LL31D: _tmp481=_tmp468.f1;if(_tmp481 <= (void*)4)goto
_LL31F;if(*((int*)_tmp481)!= 5)goto _LL31F;_tmp482=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp481)->f2;if((int)_tmp482 != 3)goto _LL31F;_tmp483=_tmp468.f2;if(_tmp483 <= (
void*)4)goto _LL31F;if(*((int*)_tmp483)!= 5)goto _LL31F;_tmp484=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp483)->f2;if((int)_tmp484 != 2)goto _LL31F;_LL31E: goto _LL320;_LL31F: _tmp485=
_tmp468.f1;if(_tmp485 <= (void*)4)goto _LL321;if(*((int*)_tmp485)!= 5)goto _LL321;
_tmp486=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp485)->f2;if((int)_tmp486 != 
2)goto _LL321;_tmp487=_tmp468.f2;if(_tmp487 <= (void*)4)goto _LL321;if(*((int*)
_tmp487)!= 5)goto _LL321;_tmp488=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp487)->f2;
if((int)_tmp488 != 3)goto _LL321;_LL320: return 0;_LL321: _tmp489=_tmp468.f1;if(
_tmp489 <= (void*)4)goto _LL323;if(*((int*)_tmp489)!= 5)goto _LL323;_tmp48A=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp489)->f2;if((int)_tmp48A != 3)goto _LL323;
_tmp48B=_tmp468.f2;if((int)_tmp48B != 1)goto _LL323;_LL322: goto _LL324;_LL323:
_tmp48C=_tmp468.f1;if(_tmp48C <= (void*)4)goto _LL325;if(*((int*)_tmp48C)!= 5)goto
_LL325;_tmp48D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48C)->f2;if((int)
_tmp48D != 2)goto _LL325;_tmp48E=_tmp468.f2;if((int)_tmp48E != 1)goto _LL325;_LL324:
goto _LL326;_LL325: _tmp48F=_tmp468.f1;if(_tmp48F <= (void*)4)goto _LL327;if(*((int*)
_tmp48F)!= 5)goto _LL327;_tmp490=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48F)->f2;
if((int)_tmp490 != 3)goto _LL327;_tmp491=_tmp468.f2;if(_tmp491 <= (void*)4)goto
_LL327;if(*((int*)_tmp491)!= 5)goto _LL327;_tmp492=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp491)->f2;if((int)_tmp492 != 1)goto _LL327;_LL326: goto _LL328;_LL327: _tmp493=
_tmp468.f1;if(_tmp493 <= (void*)4)goto _LL329;if(*((int*)_tmp493)!= 5)goto _LL329;
_tmp494=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp493)->f2;if((int)_tmp494 != 
2)goto _LL329;_tmp495=_tmp468.f2;if(_tmp495 <= (void*)4)goto _LL329;if(*((int*)
_tmp495)!= 5)goto _LL329;_tmp496=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp495)->f2;
if((int)_tmp496 != 1)goto _LL329;_LL328: goto _LL32A;_LL329: _tmp497=_tmp468.f1;if(
_tmp497 <= (void*)4)goto _LL32B;if(*((int*)_tmp497)!= 17)goto _LL32B;_tmp498=
_tmp468.f2;if(_tmp498 <= (void*)4)goto _LL32B;if(*((int*)_tmp498)!= 5)goto _LL32B;
_tmp499=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp498)->f2;if((int)_tmp499 != 
1)goto _LL32B;_LL32A: goto _LL32C;_LL32B: _tmp49A=_tmp468.f1;if(_tmp49A <= (void*)4)
goto _LL32D;if(*((int*)_tmp49A)!= 14)goto _LL32D;_tmp49B=_tmp468.f2;if(_tmp49B <= (
void*)4)goto _LL32D;if(*((int*)_tmp49B)!= 5)goto _LL32D;_tmp49C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49B)->f2;if((int)_tmp49C != 1)goto _LL32D;_LL32C: goto _LL32E;_LL32D: _tmp49D=
_tmp468.f1;if(_tmp49D <= (void*)4)goto _LL32F;if(*((int*)_tmp49D)!= 5)goto _LL32F;
_tmp49E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49D)->f2;if((int)_tmp49E != 
3)goto _LL32F;_tmp49F=_tmp468.f2;if(_tmp49F <= (void*)4)goto _LL32F;if(*((int*)
_tmp49F)!= 5)goto _LL32F;_tmp4A0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49F)->f2;
if((int)_tmp4A0 != 0)goto _LL32F;_LL32E: goto _LL330;_LL32F: _tmp4A1=_tmp468.f1;if(
_tmp4A1 <= (void*)4)goto _LL331;if(*((int*)_tmp4A1)!= 5)goto _LL331;_tmp4A2=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp4A1)->f2;if((int)_tmp4A2 != 2)goto _LL331;
_tmp4A3=_tmp468.f2;if(_tmp4A3 <= (void*)4)goto _LL331;if(*((int*)_tmp4A3)!= 5)goto
_LL331;_tmp4A4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A3)->f2;if((int)
_tmp4A4 != 0)goto _LL331;_LL330: goto _LL332;_LL331: _tmp4A5=_tmp468.f1;if(_tmp4A5 <= (
void*)4)goto _LL333;if(*((int*)_tmp4A5)!= 5)goto _LL333;_tmp4A6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4A5)->f2;if((int)_tmp4A6 != 1)goto _LL333;_tmp4A7=_tmp468.f2;if(_tmp4A7 <= (
void*)4)goto _LL333;if(*((int*)_tmp4A7)!= 5)goto _LL333;_tmp4A8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4A7)->f2;if((int)_tmp4A8 != 0)goto _LL333;_LL332: goto _LL334;_LL333: _tmp4A9=
_tmp468.f1;if(_tmp4A9 <= (void*)4)goto _LL335;if(*((int*)_tmp4A9)!= 17)goto _LL335;
_tmp4AA=_tmp468.f2;if(_tmp4AA <= (void*)4)goto _LL335;if(*((int*)_tmp4AA)!= 5)goto
_LL335;_tmp4AB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4AA)->f2;if((int)
_tmp4AB != 0)goto _LL335;_LL334: goto _LL336;_LL335: _tmp4AC=_tmp468.f1;if(_tmp4AC <= (
void*)4)goto _LL337;if(*((int*)_tmp4AC)!= 14)goto _LL337;_tmp4AD=_tmp468.f2;if(
_tmp4AD <= (void*)4)goto _LL337;if(*((int*)_tmp4AD)!= 5)goto _LL337;_tmp4AE=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp4AD)->f2;if((int)_tmp4AE != 0)goto _LL337;
_LL336: return 1;_LL337:;_LL338: return 0;_LL308:;}}int Cyc_Tcutil_coerce_list(struct
Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct Cyc_Core_Opt*
max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);
if(Cyc_Tcutil_is_arithmetic_type(t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(
t1,(void*)max_arith_type->v))max_arith_type=({struct Cyc_Core_Opt*_tmp4AF=
_cycalloc(sizeof(*_tmp4AF));_tmp4AF->v=(void*)t1;_tmp4AF;});}}}if(max_arith_type
!= 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)
el->hd,t)){({struct Cyc_String_pa_struct _tmp4B3;_tmp4B3.tag=0;_tmp4B3.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v));{struct Cyc_String_pa_struct
_tmp4B2;_tmp4B2.tag=0;_tmp4B2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp4B0[2]={& _tmp4B2,& _tmp4B3};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)el->hd)->loc,({const char*_tmp4B1="type mismatch: expecting %s but found %s";
_tag_dynforward(_tmp4B1,sizeof(char),_get_zero_arr_size(_tmp4B1,41));}),
_tag_dynforward(_tmp4B0,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp4B4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL33A: if(_tmp4B4 <= (void*)4)goto _LL33C;if(*((int*)_tmp4B4)!= 4)goto _LL33C;
_LL33B: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(void*)3);goto _LL339;
_LL33C:;_LL33D: return 0;_LL339:;}return 1;}int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp4B5=Cyc_Tcutil_compress(t);_LL33F: if(_tmp4B5 <= (void*)4)goto _LL349;if(*((
int*)_tmp4B5)!= 5)goto _LL341;_LL340: goto _LL342;_LL341: if(*((int*)_tmp4B5)!= 14)
goto _LL343;_LL342: goto _LL344;_LL343: if(*((int*)_tmp4B5)!= 17)goto _LL345;_LL344:
goto _LL346;_LL345: if(*((int*)_tmp4B5)!= 12)goto _LL347;_LL346: goto _LL348;_LL347:
if(*((int*)_tmp4B5)!= 13)goto _LL349;_LL348: return 1;_LL349:;_LL34A: return 0;_LL33E:;}
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))
return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*_tmp4B6[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp4B7="integral size mismatch; conversion supplied";
_tag_dynforward(_tmp4B7,sizeof(char),_get_zero_arr_size(_tmp4B7,44));}),
_tag_dynforward(_tmp4B6,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*
_tmp4B8[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp4B9="integral size mismatch; conversion supplied";
_tag_dynforward(_tmp4B9,sizeof(char),_get_zero_arr_size(_tmp4B9,44));}),
_tag_dynforward(_tmp4B8,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple6 _tmp4BB=({struct _tuple6 _tmp4BA;_tmp4BA.f1=
t1;_tmp4BA.f2=t2;_tmp4BA;});void*_tmp4BC;struct Cyc_Absyn_PtrInfo _tmp4BD;void*
_tmp4BE;struct Cyc_Absyn_PtrInfo _tmp4BF;void*_tmp4C0;struct Cyc_Absyn_TunionInfo
_tmp4C1;void*_tmp4C2;struct Cyc_Absyn_Tuniondecl**_tmp4C3;struct Cyc_Absyn_Tuniondecl*
_tmp4C4;struct Cyc_List_List*_tmp4C5;struct Cyc_Core_Opt*_tmp4C6;struct Cyc_Core_Opt
_tmp4C7;void*_tmp4C8;void*_tmp4C9;struct Cyc_Absyn_TunionInfo _tmp4CA;void*_tmp4CB;
struct Cyc_Absyn_Tuniondecl**_tmp4CC;struct Cyc_Absyn_Tuniondecl*_tmp4CD;struct Cyc_List_List*
_tmp4CE;struct Cyc_Core_Opt*_tmp4CF;struct Cyc_Core_Opt _tmp4D0;void*_tmp4D1;void*
_tmp4D2;struct Cyc_Absyn_ArrayInfo _tmp4D3;void*_tmp4D4;struct Cyc_Absyn_Tqual
_tmp4D5;struct Cyc_Absyn_Exp*_tmp4D6;struct Cyc_Absyn_Conref*_tmp4D7;void*_tmp4D8;
struct Cyc_Absyn_ArrayInfo _tmp4D9;void*_tmp4DA;struct Cyc_Absyn_Tqual _tmp4DB;
struct Cyc_Absyn_Exp*_tmp4DC;struct Cyc_Absyn_Conref*_tmp4DD;void*_tmp4DE;struct
Cyc_Absyn_TunionFieldInfo _tmp4DF;void*_tmp4E0;struct Cyc_Absyn_Tuniondecl*_tmp4E1;
struct Cyc_Absyn_Tunionfield*_tmp4E2;struct Cyc_List_List*_tmp4E3;void*_tmp4E4;
struct Cyc_Absyn_TunionInfo _tmp4E5;void*_tmp4E6;struct Cyc_Absyn_Tuniondecl**
_tmp4E7;struct Cyc_Absyn_Tuniondecl*_tmp4E8;struct Cyc_List_List*_tmp4E9;void*
_tmp4EA;struct Cyc_Absyn_PtrInfo _tmp4EB;void*_tmp4EC;struct Cyc_Absyn_Tqual _tmp4ED;
struct Cyc_Absyn_PtrAtts _tmp4EE;void*_tmp4EF;struct Cyc_Absyn_Conref*_tmp4F0;
struct Cyc_Absyn_Conref*_tmp4F1;struct Cyc_Absyn_Conref*_tmp4F2;void*_tmp4F3;
struct Cyc_Absyn_TunionInfo _tmp4F4;void*_tmp4F5;struct Cyc_Absyn_Tuniondecl**
_tmp4F6;struct Cyc_Absyn_Tuniondecl*_tmp4F7;struct Cyc_List_List*_tmp4F8;struct Cyc_Core_Opt*
_tmp4F9;void*_tmp4FA;void*_tmp4FB;void*_tmp4FC;void*_tmp4FD;void*_tmp4FE;void*
_tmp4FF;void*_tmp500;void*_tmp501;_LL34C: _tmp4BC=_tmp4BB.f1;if(_tmp4BC <= (void*)
4)goto _LL34E;if(*((int*)_tmp4BC)!= 4)goto _LL34E;_tmp4BD=((struct Cyc_Absyn_PointerType_struct*)
_tmp4BC)->f1;_tmp4BE=_tmp4BB.f2;if(_tmp4BE <= (void*)4)goto _LL34E;if(*((int*)
_tmp4BE)!= 4)goto _LL34E;_tmp4BF=((struct Cyc_Absyn_PointerType_struct*)_tmp4BE)->f1;
_LL34D: {int okay=1;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp4BD.ptr_atts).nullable,(
_tmp4BF.ptr_atts).nullable)){void*_tmp502=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)((_tmp4BD.ptr_atts).nullable))->v;
int _tmp503;_LL35F: if(_tmp502 <= (void*)1)goto _LL361;if(*((int*)_tmp502)!= 0)goto
_LL361;_tmp503=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp502)->f1;_LL360: okay=
!_tmp503;goto _LL35E;_LL361:;_LL362:({void*_tmp504[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp505="silent_castable conref not eq";_tag_dynforward(_tmp505,sizeof(char),
_get_zero_arr_size(_tmp505,30));}),_tag_dynforward(_tmp504,sizeof(void*),0));});
_LL35E:;}if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,(_tmp4BD.ptr_atts).bounds,(
_tmp4BF.ptr_atts).bounds)){struct _tuple6 _tmp507=({struct _tuple6 _tmp506;_tmp506.f1=(
void*)(Cyc_Absyn_compress_conref((_tmp4BD.ptr_atts).bounds))->v;_tmp506.f2=(void*)(
Cyc_Absyn_compress_conref((_tmp4BF.ptr_atts).bounds))->v;_tmp506;});void*_tmp508;
void*_tmp509;void*_tmp50A;void*_tmp50B;void*_tmp50C;_LL364: _tmp508=_tmp507.f1;
if(_tmp508 <= (void*)1)goto _LL366;if(*((int*)_tmp508)!= 0)goto _LL366;_tmp509=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp508)->f1;_tmp50A=_tmp507.f2;if(
_tmp50A <= (void*)1)goto _LL366;if(*((int*)_tmp50A)!= 0)goto _LL366;_tmp50B=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp50A)->f1;_LL365:{struct _tuple6 _tmp50E=({
struct _tuple6 _tmp50D;_tmp50D.f1=_tmp509;_tmp50D.f2=_tmp50B;_tmp50D;});void*
_tmp50F;void*_tmp510;void*_tmp511;void*_tmp512;void*_tmp513;void*_tmp514;void*
_tmp515;void*_tmp516;void*_tmp517;void*_tmp518;void*_tmp519;void*_tmp51A;void*
_tmp51B;struct Cyc_Absyn_Exp*_tmp51C;void*_tmp51D;struct Cyc_Absyn_Exp*_tmp51E;
void*_tmp51F;void*_tmp520;struct Cyc_Absyn_Exp*_tmp521;void*_tmp522;void*_tmp523;
struct Cyc_Absyn_Exp*_tmp524;void*_tmp525;void*_tmp526;void*_tmp527;void*_tmp528;
void*_tmp529;struct Cyc_Absyn_Exp*_tmp52A;void*_tmp52B;void*_tmp52C;void*_tmp52D;
void*_tmp52E;_LL36B: _tmp50F=_tmp50E.f1;if(_tmp50F <= (void*)2)goto _LL36D;if(*((
int*)_tmp50F)!= 0)goto _LL36D;_tmp510=_tmp50E.f2;if((int)_tmp510 != 0)goto _LL36D;
_LL36C: goto _LL36E;_LL36D: _tmp511=_tmp50E.f1;if(_tmp511 <= (void*)2)goto _LL36F;if(*((
int*)_tmp511)!= 0)goto _LL36F;_tmp512=_tmp50E.f2;if((int)_tmp512 != 1)goto _LL36F;
_LL36E: goto _LL370;_LL36F: _tmp513=_tmp50E.f1;if((int)_tmp513 != 1)goto _LL371;
_tmp514=_tmp50E.f2;if((int)_tmp514 != 1)goto _LL371;_LL370: goto _LL372;_LL371:
_tmp515=_tmp50E.f1;if((int)_tmp515 != 0)goto _LL373;_tmp516=_tmp50E.f2;if((int)
_tmp516 != 1)goto _LL373;_LL372: goto _LL374;_LL373: _tmp517=_tmp50E.f1;if((int)
_tmp517 != 0)goto _LL375;_tmp518=_tmp50E.f2;if((int)_tmp518 != 0)goto _LL375;_LL374:
okay=1;goto _LL36A;_LL375: _tmp519=_tmp50E.f1;if((int)_tmp519 != 1)goto _LL377;
_tmp51A=_tmp50E.f2;if((int)_tmp51A != 0)goto _LL377;_LL376: okay=0;goto _LL36A;
_LL377: _tmp51B=_tmp50E.f1;if(_tmp51B <= (void*)2)goto _LL379;if(*((int*)_tmp51B)!= 
0)goto _LL379;_tmp51C=((struct Cyc_Absyn_Upper_b_struct*)_tmp51B)->f1;_tmp51D=
_tmp50E.f2;if(_tmp51D <= (void*)2)goto _LL379;if(*((int*)_tmp51D)!= 0)goto _LL379;
_tmp51E=((struct Cyc_Absyn_Upper_b_struct*)_tmp51D)->f1;_LL378: okay=okay  && Cyc_Evexp_lte_const_exp(
_tmp51E,_tmp51C);if(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp4BF.ptr_atts).zero_term))({void*_tmp52F[0]={};Cyc_Tcutil_warn(loc,({const
char*_tmp530="implicit cast to shorter array";_tag_dynforward(_tmp530,sizeof(
char),_get_zero_arr_size(_tmp530,31));}),_tag_dynforward(_tmp52F,sizeof(void*),0));});
goto _LL36A;_LL379: _tmp51F=_tmp50E.f1;if((int)_tmp51F != 0)goto _LL37B;_tmp520=
_tmp50E.f2;if(_tmp520 <= (void*)2)goto _LL37B;if(*((int*)_tmp520)!= 0)goto _LL37B;
_tmp521=((struct Cyc_Absyn_Upper_b_struct*)_tmp520)->f1;_LL37A: _tmp524=_tmp521;
goto _LL37C;_LL37B: _tmp522=_tmp50E.f1;if((int)_tmp522 != 1)goto _LL37D;_tmp523=
_tmp50E.f2;if(_tmp523 <= (void*)2)goto _LL37D;if(*((int*)_tmp523)!= 0)goto _LL37D;
_tmp524=((struct Cyc_Absyn_Upper_b_struct*)_tmp523)->f1;_LL37C: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp4BD.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp4BF.ptr_atts).bounds))goto _LL36A;okay=0;goto
_LL36A;_LL37D: _tmp525=_tmp50E.f1;if(_tmp525 <= (void*)2)goto _LL37F;if(*((int*)
_tmp525)!= 1)goto _LL37F;_tmp526=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp525)->f1;_tmp527=_tmp50E.f2;if(_tmp527 <= (void*)2)goto _LL37F;if(*((int*)
_tmp527)!= 1)goto _LL37F;_tmp528=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp527)->f1;_LL37E: if(!Cyc_Tcutil_unify(_tmp526,_tmp528)){struct _tuple6 _tmp532=({
struct _tuple6 _tmp531;_tmp531.f1=Cyc_Tcutil_compress(_tmp526);_tmp531.f2=Cyc_Tcutil_compress(
_tmp528);_tmp531;});void*_tmp533;int _tmp534;void*_tmp535;int _tmp536;_LL386:
_tmp533=_tmp532.f1;if(_tmp533 <= (void*)4)goto _LL388;if(*((int*)_tmp533)!= 18)
goto _LL388;_tmp534=((struct Cyc_Absyn_TypeInt_struct*)_tmp533)->f1;_tmp535=
_tmp532.f2;if(_tmp535 <= (void*)4)goto _LL388;if(*((int*)_tmp535)!= 18)goto _LL388;
_tmp536=((struct Cyc_Absyn_TypeInt_struct*)_tmp535)->f1;_LL387: if(_tmp534 < 
_tmp536)okay=0;goto _LL385;_LL388:;_LL389: okay=0;goto _LL385;_LL385:;}goto _LL36A;
_LL37F: _tmp529=_tmp50E.f1;if(_tmp529 <= (void*)2)goto _LL381;if(*((int*)_tmp529)!= 
0)goto _LL381;_tmp52A=((struct Cyc_Absyn_Upper_b_struct*)_tmp529)->f1;_tmp52B=
_tmp50E.f2;if(_tmp52B <= (void*)2)goto _LL381;if(*((int*)_tmp52B)!= 1)goto _LL381;
_tmp52C=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp52B)->f1;_LL380: {
unsigned int _tmp538;int _tmp539;struct _tuple7 _tmp537=Cyc_Evexp_eval_const_uint_exp(
_tmp52A);_tmp538=_tmp537.f1;_tmp539=_tmp537.f2;if(!_tmp539){okay=0;goto _LL36A;}{
void*_tmp53A=Cyc_Tcutil_compress(_tmp52C);int _tmp53B;_LL38B: if(_tmp53A <= (void*)
4)goto _LL38D;if(*((int*)_tmp53A)!= 18)goto _LL38D;_tmp53B=((struct Cyc_Absyn_TypeInt_struct*)
_tmp53A)->f1;_LL38C: if(_tmp538 < _tmp53B)okay=0;goto _LL38A;_LL38D:;_LL38E: okay=0;
goto _LL38A;_LL38A:;}goto _LL36A;}_LL381: _tmp52D=_tmp50E.f1;if(_tmp52D <= (void*)2)
goto _LL383;if(*((int*)_tmp52D)!= 1)goto _LL383;_LL382: goto _LL384;_LL383: _tmp52E=
_tmp50E.f2;if(_tmp52E <= (void*)2)goto _LL36A;if(*((int*)_tmp52E)!= 1)goto _LL36A;
_LL384: okay=0;goto _LL36A;_LL36A:;}goto _LL363;_LL366: _tmp50C=_tmp507.f1;if((int)
_tmp50C != 0)goto _LL368;_LL367: okay=0;goto _LL363;_LL368:;_LL369:({void*_tmp53C[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp53D="silent_castable conrefs didn't unify";_tag_dynforward(_tmp53D,
sizeof(char),_get_zero_arr_size(_tmp53D,37));}),_tag_dynforward(_tmp53C,sizeof(
void*),0));});_LL363:;}okay=okay  && Cyc_Tcutil_unify((void*)_tmp4BD.elt_typ,(
void*)_tmp4BF.elt_typ);okay=okay  && (Cyc_Tcutil_unify((void*)(_tmp4BD.ptr_atts).rgn,(
void*)(_tmp4BF.ptr_atts).rgn) || Cyc_Tcenv_region_outlives(te,(void*)(_tmp4BD.ptr_atts).rgn,(
void*)(_tmp4BF.ptr_atts).rgn));okay=okay  && (!(_tmp4BD.elt_tq).real_const  || (
_tmp4BF.elt_tq).real_const);okay=okay  && (((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp4BD.ptr_atts).zero_term,(
_tmp4BF.ptr_atts).zero_term) || ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,(_tmp4BD.ptr_atts).zero_term) && (_tmp4BF.elt_tq).real_const);return okay;}
_LL34E: _tmp4C0=_tmp4BB.f1;if(_tmp4C0 <= (void*)4)goto _LL350;if(*((int*)_tmp4C0)!= 
2)goto _LL350;_tmp4C1=((struct Cyc_Absyn_TunionType_struct*)_tmp4C0)->f1;_tmp4C2=(
void*)_tmp4C1.tunion_info;if(*((int*)_tmp4C2)!= 1)goto _LL350;_tmp4C3=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp4C2)->f1;_tmp4C4=*_tmp4C3;_tmp4C5=_tmp4C1.targs;_tmp4C6=_tmp4C1.rgn;if(
_tmp4C6 == 0)goto _LL350;_tmp4C7=*_tmp4C6;_tmp4C8=(void*)_tmp4C7.v;_tmp4C9=_tmp4BB.f2;
if(_tmp4C9 <= (void*)4)goto _LL350;if(*((int*)_tmp4C9)!= 2)goto _LL350;_tmp4CA=((
struct Cyc_Absyn_TunionType_struct*)_tmp4C9)->f1;_tmp4CB=(void*)_tmp4CA.tunion_info;
if(*((int*)_tmp4CB)!= 1)goto _LL350;_tmp4CC=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp4CB)->f1;_tmp4CD=*_tmp4CC;_tmp4CE=_tmp4CA.targs;_tmp4CF=_tmp4CA.rgn;if(
_tmp4CF == 0)goto _LL350;_tmp4D0=*_tmp4CF;_tmp4D1=(void*)_tmp4D0.v;_LL34F: if(
_tmp4C4 != _tmp4CD  || !Cyc_Tcenv_region_outlives(te,_tmp4C8,_tmp4D1))return 0;for(
0;_tmp4C5 != 0  && _tmp4CE != 0;(_tmp4C5=_tmp4C5->tl,_tmp4CE=_tmp4CE->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp4C5->hd,(void*)_tmp4CE->hd))return 0;}if(_tmp4C5 != 0  || _tmp4CE != 0)
return 0;return 1;_LL350: _tmp4D2=_tmp4BB.f1;if(_tmp4D2 <= (void*)4)goto _LL352;if(*((
int*)_tmp4D2)!= 7)goto _LL352;_tmp4D3=((struct Cyc_Absyn_ArrayType_struct*)_tmp4D2)->f1;
_tmp4D4=(void*)_tmp4D3.elt_type;_tmp4D5=_tmp4D3.tq;_tmp4D6=_tmp4D3.num_elts;
_tmp4D7=_tmp4D3.zero_term;_tmp4D8=_tmp4BB.f2;if(_tmp4D8 <= (void*)4)goto _LL352;
if(*((int*)_tmp4D8)!= 7)goto _LL352;_tmp4D9=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4D8)->f1;_tmp4DA=(void*)_tmp4D9.elt_type;_tmp4DB=_tmp4D9.tq;_tmp4DC=_tmp4D9.num_elts;
_tmp4DD=_tmp4D9.zero_term;_LL351: {int okay;okay=((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp4D7,_tmp4DD) && ((_tmp4D6 != 0  && _tmp4DC != 0) && Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_tmp4D6,(struct Cyc_Absyn_Exp*)_tmp4DC));return(okay  && Cyc_Tcutil_unify(
_tmp4D4,_tmp4DA)) && (!_tmp4D5.real_const  || _tmp4DB.real_const);}_LL352: _tmp4DE=
_tmp4BB.f1;if(_tmp4DE <= (void*)4)goto _LL354;if(*((int*)_tmp4DE)!= 3)goto _LL354;
_tmp4DF=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp4DE)->f1;_tmp4E0=(void*)
_tmp4DF.field_info;if(*((int*)_tmp4E0)!= 1)goto _LL354;_tmp4E1=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp4E0)->f1;_tmp4E2=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4E0)->f2;
_tmp4E3=_tmp4DF.targs;_tmp4E4=_tmp4BB.f2;if(_tmp4E4 <= (void*)4)goto _LL354;if(*((
int*)_tmp4E4)!= 2)goto _LL354;_tmp4E5=((struct Cyc_Absyn_TunionType_struct*)
_tmp4E4)->f1;_tmp4E6=(void*)_tmp4E5.tunion_info;if(*((int*)_tmp4E6)!= 1)goto
_LL354;_tmp4E7=((struct Cyc_Absyn_KnownTunion_struct*)_tmp4E6)->f1;_tmp4E8=*
_tmp4E7;_tmp4E9=_tmp4E5.targs;_LL353: if((_tmp4E1 == _tmp4E8  || Cyc_Absyn_qvar_cmp(
_tmp4E1->name,_tmp4E8->name)== 0) && (_tmp4E2->typs == 0  || _tmp4E1->is_flat)){
for(0;_tmp4E3 != 0  && _tmp4E9 != 0;(_tmp4E3=_tmp4E3->tl,_tmp4E9=_tmp4E9->tl)){if(!
Cyc_Tcutil_unify((void*)_tmp4E3->hd,(void*)_tmp4E9->hd))break;}if(_tmp4E3 == 0
 && _tmp4E9 == 0)return 1;}return 0;_LL354: _tmp4EA=_tmp4BB.f1;if(_tmp4EA <= (void*)4)
goto _LL356;if(*((int*)_tmp4EA)!= 4)goto _LL356;_tmp4EB=((struct Cyc_Absyn_PointerType_struct*)
_tmp4EA)->f1;_tmp4EC=(void*)_tmp4EB.elt_typ;_tmp4ED=_tmp4EB.elt_tq;_tmp4EE=
_tmp4EB.ptr_atts;_tmp4EF=(void*)_tmp4EE.rgn;_tmp4F0=_tmp4EE.nullable;_tmp4F1=
_tmp4EE.bounds;_tmp4F2=_tmp4EE.zero_term;_tmp4F3=_tmp4BB.f2;if(_tmp4F3 <= (void*)
4)goto _LL356;if(*((int*)_tmp4F3)!= 2)goto _LL356;_tmp4F4=((struct Cyc_Absyn_TunionType_struct*)
_tmp4F3)->f1;_tmp4F5=(void*)_tmp4F4.tunion_info;if(*((int*)_tmp4F5)!= 1)goto
_LL356;_tmp4F6=((struct Cyc_Absyn_KnownTunion_struct*)_tmp4F5)->f1;_tmp4F7=*
_tmp4F6;_tmp4F8=_tmp4F4.targs;_tmp4F9=_tmp4F4.rgn;if(!(!_tmp4F7->is_flat))goto
_LL356;_LL355:{void*_tmp53E=Cyc_Tcutil_compress(_tmp4EC);struct Cyc_Absyn_TunionFieldInfo
_tmp53F;void*_tmp540;struct Cyc_Absyn_Tuniondecl*_tmp541;struct Cyc_Absyn_Tunionfield*
_tmp542;struct Cyc_List_List*_tmp543;_LL390: if(_tmp53E <= (void*)4)goto _LL392;if(*((
int*)_tmp53E)!= 3)goto _LL392;_tmp53F=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp53E)->f1;_tmp540=(void*)_tmp53F.field_info;if(*((int*)_tmp540)!= 1)goto
_LL392;_tmp541=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp540)->f1;_tmp542=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp540)->f2;_tmp543=_tmp53F.targs;
_LL391: if(!Cyc_Tcutil_unify(_tmp4EF,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4F9))->v) && !Cyc_Tcenv_region_outlives(te,_tmp4EF,(void*)_tmp4F9->v))return
0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4F0,Cyc_Absyn_false_conref))
return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,_tmp4F1,Cyc_Absyn_bounds_one_conref))
return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4F2,Cyc_Absyn_false_conref))
return 0;if(Cyc_Absyn_qvar_cmp(_tmp4F7->name,_tmp541->name)== 0  && _tmp542->typs
!= 0){int okay=1;for(0;_tmp543 != 0  && _tmp4F8 != 0;(_tmp543=_tmp543->tl,_tmp4F8=
_tmp4F8->tl)){if(!Cyc_Tcutil_unify((void*)_tmp543->hd,(void*)_tmp4F8->hd)){okay=
0;break;}}if((!okay  || _tmp543 != 0) || _tmp4F8 != 0)return 0;return 1;}goto _LL38F;
_LL392:;_LL393: goto _LL38F;_LL38F:;}return 0;_LL356: _tmp4FA=_tmp4BB.f1;if(_tmp4FA
<= (void*)4)goto _LL358;if(*((int*)_tmp4FA)!= 14)goto _LL358;_tmp4FB=_tmp4BB.f2;
if(_tmp4FB <= (void*)4)goto _LL358;if(*((int*)_tmp4FB)!= 5)goto _LL358;_tmp4FC=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp4FB)->f2;if((int)_tmp4FC != 2)goto
_LL358;_LL357: goto _LL359;_LL358: _tmp4FD=_tmp4BB.f1;if(_tmp4FD <= (void*)4)goto
_LL35A;if(*((int*)_tmp4FD)!= 14)goto _LL35A;_tmp4FE=_tmp4BB.f2;if(_tmp4FE <= (void*)
4)goto _LL35A;if(*((int*)_tmp4FE)!= 5)goto _LL35A;_tmp4FF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4FE)->f2;if((int)_tmp4FF != 3)goto _LL35A;_LL359: return 1;_LL35A: _tmp500=
_tmp4BB.f1;if(_tmp500 <= (void*)4)goto _LL35C;if(*((int*)_tmp500)!= 17)goto _LL35C;
_tmp501=_tmp4BB.f2;if(_tmp501 <= (void*)4)goto _LL35C;if(*((int*)_tmp501)!= 5)goto
_LL35C;_LL35B: return 0;_LL35C:;_LL35D: return Cyc_Tcutil_unify(t1,t2);_LL34B:;}}int
Cyc_Tcutil_is_pointer_type(void*t){void*_tmp544=Cyc_Tcutil_compress(t);_LL395:
if(_tmp544 <= (void*)4)goto _LL397;if(*((int*)_tmp544)!= 4)goto _LL397;_LL396:
return 1;_LL397:;_LL398: return 0;_LL394:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*
e){void*_tmp545=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp546;int _tmp547;union Cyc_Absyn_Cnst_union
_tmp548;char _tmp549;union Cyc_Absyn_Cnst_union _tmp54A;short _tmp54B;union Cyc_Absyn_Cnst_union
_tmp54C;long long _tmp54D;void*_tmp54E;struct Cyc_Absyn_Exp*_tmp54F;_LL39A: if(*((
int*)_tmp545)!= 0)goto _LL39C;_tmp546=((struct Cyc_Absyn_Const_e_struct*)_tmp545)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp545)->f1).Int_c).tag != 2)goto _LL39C;
_tmp547=(_tmp546.Int_c).f2;if(_tmp547 != 0)goto _LL39C;_LL39B: goto _LL39D;_LL39C:
if(*((int*)_tmp545)!= 0)goto _LL39E;_tmp548=((struct Cyc_Absyn_Const_e_struct*)
_tmp545)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp545)->f1).Char_c).tag != 
0)goto _LL39E;_tmp549=(_tmp548.Char_c).f2;if(_tmp549 != 0)goto _LL39E;_LL39D: goto
_LL39F;_LL39E: if(*((int*)_tmp545)!= 0)goto _LL3A0;_tmp54A=((struct Cyc_Absyn_Const_e_struct*)
_tmp545)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp545)->f1).Short_c).tag != 
1)goto _LL3A0;_tmp54B=(_tmp54A.Short_c).f2;if(_tmp54B != 0)goto _LL3A0;_LL39F: goto
_LL3A1;_LL3A0: if(*((int*)_tmp545)!= 0)goto _LL3A2;_tmp54C=((struct Cyc_Absyn_Const_e_struct*)
_tmp545)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp545)->f1).LongLong_c).tag
!= 3)goto _LL3A2;_tmp54D=(_tmp54C.LongLong_c).f2;if(_tmp54D != 0)goto _LL3A2;_LL3A1:
return 1;_LL3A2: if(*((int*)_tmp545)!= 15)goto _LL3A4;_tmp54E=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp545)->f1;_tmp54F=((struct Cyc_Absyn_Cast_e_struct*)_tmp545)->f2;_LL3A3: return
Cyc_Tcutil_is_zero(_tmp54F) && Cyc_Tcutil_admits_zero(_tmp54E);_LL3A4:;_LL3A5:
return 0;_LL399:;}struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt
Cyc_Tcutil_ak={(void*)((void*)0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)
2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)((void*)1)};int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp550=_cycalloc(sizeof(*_tmp550));_tmp550[0]=({struct Cyc_Absyn_Const_e_struct
_tmp551;_tmp551.tag=0;_tmp551.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp552;(_tmp552.Null_c).tag=6;_tmp552;});_tmp551;});_tmp550;})));{struct Cyc_List_List*
_tmp553=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_PointerType_struct*
_tmp554=({struct Cyc_Absyn_PointerType_struct*_tmp555=_cycalloc(sizeof(*_tmp555));
_tmp555[0]=({struct Cyc_Absyn_PointerType_struct _tmp556;_tmp556.tag=4;_tmp556.f1=({
struct Cyc_Absyn_PtrInfo _tmp557;_tmp557.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp55A=_cycalloc(sizeof(*
_tmp55A));_tmp55A->v=_tmp553;_tmp55A;}));_tmp557.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp557.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp558;_tmp558.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp559=_cycalloc(
sizeof(*_tmp559));_tmp559->v=_tmp553;_tmp559;}));_tmp558.nullable=Cyc_Absyn_true_conref;
_tmp558.bounds=Cyc_Absyn_empty_conref();_tmp558.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp558.ptrloc=0;_tmp558;});_tmp557;});_tmp556;});
_tmp555;});(void*)(((struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)
_tmp554));return Cyc_Tcutil_coerce_arg(te,e1,t2);}}return 0;}struct _dynforward_ptr
Cyc_Tcutil_coercion2string(void*c){void*_tmp55B=c;_LL3A7: if((int)_tmp55B != 0)
goto _LL3A9;_LL3A8: return({const char*_tmp55C="unknown";_tag_dynforward(_tmp55C,
sizeof(char),_get_zero_arr_size(_tmp55C,8));});_LL3A9: if((int)_tmp55B != 1)goto
_LL3AB;_LL3AA: return({const char*_tmp55D="no coercion";_tag_dynforward(_tmp55D,
sizeof(char),_get_zero_arr_size(_tmp55D,12));});_LL3AB: if((int)_tmp55B != 2)goto
_LL3AD;_LL3AC: return({const char*_tmp55E="null check";_tag_dynforward(_tmp55E,
sizeof(char),_get_zero_arr_size(_tmp55E,11));});_LL3AD: if((int)_tmp55B != 3)goto
_LL3A6;_LL3AE: return({const char*_tmp55F="other coercion";_tag_dynforward(_tmp55F,
sizeof(char),_get_zero_arr_size(_tmp55F,15));});_LL3A6:;}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*c;if(Cyc_Tcutil_unify(
t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(
t1)){if(Cyc_Tcutil_will_lose_precision(t1,t2))({struct Cyc_String_pa_struct
_tmp563;_tmp563.tag=0;_tmp563.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp562;_tmp562.tag=0;
_tmp562.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp560[2]={& _tmp562,& _tmp563};Cyc_Tcutil_warn(e->loc,({const char*
_tmp561="integral size mismatch; %s -> %s conversion supplied";_tag_dynforward(
_tmp561,sizeof(char),_get_zero_arr_size(_tmp561,53));}),_tag_dynforward(_tmp560,
sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)1);return 1;}
else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2,(void*)3);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;
else{if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= (void*)0){Cyc_Tcutil_unchecked_cast(
te,e,t2,c);if(c != (void*)2)({struct Cyc_String_pa_struct _tmp567;_tmp567.tag=0;
_tmp567.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp566;_tmp566.tag=0;_tmp566.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp564[2]={& _tmp566,& _tmp567};Cyc_Tcutil_warn(e->loc,({const char*_tmp565="implicit cast from %s to %s";
_tag_dynforward(_tmp565,sizeof(char),_get_zero_arr_size(_tmp565,28));}),
_tag_dynforward(_tmp564,sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp568=Cyc_Tcutil_compress(t);
_LL3B0: if(_tmp568 <= (void*)4)goto _LL3B2;if(*((int*)_tmp568)!= 5)goto _LL3B2;
_LL3B1: goto _LL3B3;_LL3B2: if((int)_tmp568 != 1)goto _LL3B4;_LL3B3: goto _LL3B5;_LL3B4:
if(_tmp568 <= (void*)4)goto _LL3B6;if(*((int*)_tmp568)!= 6)goto _LL3B6;_LL3B5:
return 1;_LL3B6:;_LL3B7: return 0;_LL3AF:;}static struct _tuple4*Cyc_Tcutil_flatten_typ_f(
struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*x){return({struct _tuple4*
_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569->f1=x->tq;_tmp569->f2=Cyc_Tcutil_substitute(
inst,(void*)x->type);_tmp569;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv*te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp56A=t1;
struct Cyc_List_List*_tmp56B;struct Cyc_Absyn_AggrInfo _tmp56C;void*_tmp56D;struct
Cyc_Absyn_Aggrdecl**_tmp56E;struct Cyc_Absyn_Aggrdecl*_tmp56F;struct Cyc_List_List*
_tmp570;void*_tmp571;struct Cyc_List_List*_tmp572;struct Cyc_Absyn_FnInfo _tmp573;
_LL3B9: if((int)_tmp56A != 0)goto _LL3BB;_LL3BA: return 0;_LL3BB: if(_tmp56A <= (void*)
4)goto _LL3C3;if(*((int*)_tmp56A)!= 9)goto _LL3BD;_tmp56B=((struct Cyc_Absyn_TupleType_struct*)
_tmp56A)->f1;_LL3BC: return _tmp56B;_LL3BD: if(*((int*)_tmp56A)!= 10)goto _LL3BF;
_tmp56C=((struct Cyc_Absyn_AggrType_struct*)_tmp56A)->f1;_tmp56D=(void*)_tmp56C.aggr_info;
if(*((int*)_tmp56D)!= 1)goto _LL3BF;_tmp56E=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp56D)->f1;_tmp56F=*_tmp56E;_tmp570=_tmp56C.targs;_LL3BE: if((((void*)_tmp56F->kind
== (void*)1  || _tmp56F->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp56F->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp56F->impl))->rgn_po != 0)return({struct Cyc_List_List*_tmp574=_cycalloc(
sizeof(*_tmp574));_tmp574->hd=({struct _tuple4*_tmp575=_cycalloc(sizeof(*_tmp575));
_tmp575->f1=Cyc_Absyn_empty_tqual(0);_tmp575->f2=t1;_tmp575;});_tmp574->tl=0;
_tmp574;});{struct Cyc_List_List*_tmp576=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp56F->tvs,_tmp570);return((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmp576,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp56F->impl))->fields);}_LL3BF: if(*((
int*)_tmp56A)!= 11)goto _LL3C1;_tmp571=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp56A)->f1;if((int)_tmp571 != 0)goto _LL3C1;_tmp572=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp56A)->f2;_LL3C0: return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_flatten_typ_f,0,_tmp572);_LL3C1: if(*((int*)_tmp56A)!= 8)goto _LL3C3;
_tmp573=((struct Cyc_Absyn_FnType_struct*)_tmp56A)->f1;_LL3C2: return({struct Cyc_List_List*
_tmp577=_cycalloc(sizeof(*_tmp577));_tmp577->hd=({struct _tuple4*_tmp578=
_cycalloc(sizeof(*_tmp578));_tmp578->f1=Cyc_Absyn_const_tqual(0);_tmp578->f2=t1;
_tmp578;});_tmp577->tl=0;_tmp577;});_LL3C3:;_LL3C4: return({struct Cyc_List_List*
_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579->hd=({struct _tuple4*_tmp57A=
_cycalloc(sizeof(*_tmp57A));_tmp57A->f1=Cyc_Absyn_empty_tqual(0);_tmp57A->f2=t1;
_tmp57A;});_tmp579->tl=0;_tmp579;});_LL3B8:;}}static int Cyc_Tcutil_sub_attributes(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t
!= 0;t=t->tl){void*_tmp57B=(void*)t->hd;_LL3C6: if((int)_tmp57B != 16)goto _LL3C8;
_LL3C7: goto _LL3C9;_LL3C8: if((int)_tmp57B != 3)goto _LL3CA;_LL3C9: goto _LL3CB;_LL3CA:
if(_tmp57B <= (void*)17)goto _LL3CC;if(*((int*)_tmp57B)!= 4)goto _LL3CC;_LL3CB:
continue;_LL3CC:;_LL3CD: if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)t->hd,
a2))return 0;_LL3C5:;}}for(0;a2 != 0;a2=a2->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple6*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple6*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6
_tmp57D=({struct _tuple6 _tmp57C;_tmp57C.f1=t1;_tmp57C.f2=t2;_tmp57C;});void*
_tmp57E;struct Cyc_Absyn_PtrInfo _tmp57F;void*_tmp580;struct Cyc_Absyn_Tqual _tmp581;
struct Cyc_Absyn_PtrAtts _tmp582;void*_tmp583;struct Cyc_Absyn_Conref*_tmp584;
struct Cyc_Absyn_Conref*_tmp585;struct Cyc_Absyn_Conref*_tmp586;void*_tmp587;
struct Cyc_Absyn_PtrInfo _tmp588;void*_tmp589;struct Cyc_Absyn_Tqual _tmp58A;struct
Cyc_Absyn_PtrAtts _tmp58B;void*_tmp58C;struct Cyc_Absyn_Conref*_tmp58D;struct Cyc_Absyn_Conref*
_tmp58E;struct Cyc_Absyn_Conref*_tmp58F;void*_tmp590;struct Cyc_Absyn_TunionInfo
_tmp591;void*_tmp592;struct Cyc_Absyn_Tuniondecl**_tmp593;struct Cyc_Absyn_Tuniondecl*
_tmp594;struct Cyc_List_List*_tmp595;struct Cyc_Core_Opt*_tmp596;struct Cyc_Core_Opt
_tmp597;void*_tmp598;void*_tmp599;struct Cyc_Absyn_TunionInfo _tmp59A;void*_tmp59B;
struct Cyc_Absyn_Tuniondecl**_tmp59C;struct Cyc_Absyn_Tuniondecl*_tmp59D;struct Cyc_List_List*
_tmp59E;struct Cyc_Core_Opt*_tmp59F;struct Cyc_Core_Opt _tmp5A0;void*_tmp5A1;void*
_tmp5A2;struct Cyc_Absyn_FnInfo _tmp5A3;void*_tmp5A4;struct Cyc_Absyn_FnInfo _tmp5A5;
_LL3CF: _tmp57E=_tmp57D.f1;if(_tmp57E <= (void*)4)goto _LL3D1;if(*((int*)_tmp57E)!= 
4)goto _LL3D1;_tmp57F=((struct Cyc_Absyn_PointerType_struct*)_tmp57E)->f1;_tmp580=(
void*)_tmp57F.elt_typ;_tmp581=_tmp57F.elt_tq;_tmp582=_tmp57F.ptr_atts;_tmp583=(
void*)_tmp582.rgn;_tmp584=_tmp582.nullable;_tmp585=_tmp582.bounds;_tmp586=
_tmp582.zero_term;_tmp587=_tmp57D.f2;if(_tmp587 <= (void*)4)goto _LL3D1;if(*((int*)
_tmp587)!= 4)goto _LL3D1;_tmp588=((struct Cyc_Absyn_PointerType_struct*)_tmp587)->f1;
_tmp589=(void*)_tmp588.elt_typ;_tmp58A=_tmp588.elt_tq;_tmp58B=_tmp588.ptr_atts;
_tmp58C=(void*)_tmp58B.rgn;_tmp58D=_tmp58B.nullable;_tmp58E=_tmp58B.bounds;
_tmp58F=_tmp58B.zero_term;_LL3D0: if(_tmp581.real_const  && !_tmp58A.real_const)
return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp584,_tmp58D) && ((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp584)) && !((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp58D))return 0;if((!((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp586,_tmp58F) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp586)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp58F))return 0;if(!Cyc_Tcutil_unify(_tmp583,_tmp58C) && !Cyc_Tcenv_region_outlives(
te,_tmp583,_tmp58C))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp585,_tmp58E)){struct _tuple6 _tmp5A7=({struct _tuple6 _tmp5A6;_tmp5A6.f1=Cyc_Absyn_conref_val(
_tmp585);_tmp5A6.f2=Cyc_Absyn_conref_val(_tmp58E);_tmp5A6;});void*_tmp5A8;void*
_tmp5A9;void*_tmp5AA;void*_tmp5AB;void*_tmp5AC;struct Cyc_Absyn_Exp*_tmp5AD;void*
_tmp5AE;struct Cyc_Absyn_Exp*_tmp5AF;_LL3D8: _tmp5A8=_tmp5A7.f1;if(_tmp5A8 <= (void*)
2)goto _LL3DA;if(*((int*)_tmp5A8)!= 0)goto _LL3DA;_tmp5A9=_tmp5A7.f2;if((int)
_tmp5A9 != 0)goto _LL3DA;_LL3D9: goto _LL3D7;_LL3DA: _tmp5AA=_tmp5A7.f1;if(_tmp5AA <= (
void*)2)goto _LL3DC;if(*((int*)_tmp5AA)!= 0)goto _LL3DC;_tmp5AB=_tmp5A7.f2;if((int)
_tmp5AB != 1)goto _LL3DC;_LL3DB: goto _LL3D7;_LL3DC: _tmp5AC=_tmp5A7.f1;if(_tmp5AC <= (
void*)2)goto _LL3DE;if(*((int*)_tmp5AC)!= 0)goto _LL3DE;_tmp5AD=((struct Cyc_Absyn_Upper_b_struct*)
_tmp5AC)->f1;_tmp5AE=_tmp5A7.f2;if(_tmp5AE <= (void*)2)goto _LL3DE;if(*((int*)
_tmp5AE)!= 0)goto _LL3DE;_tmp5AF=((struct Cyc_Absyn_Upper_b_struct*)_tmp5AE)->f1;
_LL3DD: if(!Cyc_Evexp_lte_const_exp(_tmp5AF,_tmp5AD))return 0;goto _LL3D7;_LL3DE:;
_LL3DF: return 0;_LL3D7:;}return Cyc_Tcutil_ptrsubtype(te,({struct Cyc_List_List*
_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0->hd=({struct _tuple6*_tmp5B1=
_cycalloc(sizeof(*_tmp5B1));_tmp5B1->f1=t1;_tmp5B1->f2=t2;_tmp5B1;});_tmp5B0->tl=
assume;_tmp5B0;}),_tmp580,_tmp589);_LL3D1: _tmp590=_tmp57D.f1;if(_tmp590 <= (void*)
4)goto _LL3D3;if(*((int*)_tmp590)!= 2)goto _LL3D3;_tmp591=((struct Cyc_Absyn_TunionType_struct*)
_tmp590)->f1;_tmp592=(void*)_tmp591.tunion_info;if(*((int*)_tmp592)!= 1)goto
_LL3D3;_tmp593=((struct Cyc_Absyn_KnownTunion_struct*)_tmp592)->f1;_tmp594=*
_tmp593;_tmp595=_tmp591.targs;_tmp596=_tmp591.rgn;if(_tmp596 == 0)goto _LL3D3;
_tmp597=*_tmp596;_tmp598=(void*)_tmp597.v;_tmp599=_tmp57D.f2;if(_tmp599 <= (void*)
4)goto _LL3D3;if(*((int*)_tmp599)!= 2)goto _LL3D3;_tmp59A=((struct Cyc_Absyn_TunionType_struct*)
_tmp599)->f1;_tmp59B=(void*)_tmp59A.tunion_info;if(*((int*)_tmp59B)!= 1)goto
_LL3D3;_tmp59C=((struct Cyc_Absyn_KnownTunion_struct*)_tmp59B)->f1;_tmp59D=*
_tmp59C;_tmp59E=_tmp59A.targs;_tmp59F=_tmp59A.rgn;if(_tmp59F == 0)goto _LL3D3;
_tmp5A0=*_tmp59F;_tmp5A1=(void*)_tmp5A0.v;_LL3D2: if(_tmp594 != _tmp59D  || !Cyc_Tcenv_region_outlives(
te,_tmp598,_tmp5A1))return 0;for(0;_tmp595 != 0  && _tmp59E != 0;(_tmp595=_tmp595->tl,
_tmp59E=_tmp59E->tl)){if(!Cyc_Tcutil_unify((void*)_tmp595->hd,(void*)_tmp59E->hd))
return 0;}if(_tmp595 != 0  || _tmp59E != 0)return 0;return 1;_LL3D3: _tmp5A2=_tmp57D.f1;
if(_tmp5A2 <= (void*)4)goto _LL3D5;if(*((int*)_tmp5A2)!= 8)goto _LL3D5;_tmp5A3=((
struct Cyc_Absyn_FnType_struct*)_tmp5A2)->f1;_tmp5A4=_tmp57D.f2;if(_tmp5A4 <= (
void*)4)goto _LL3D5;if(*((int*)_tmp5A4)!= 8)goto _LL3D5;_tmp5A5=((struct Cyc_Absyn_FnType_struct*)
_tmp5A4)->f1;_LL3D4: if(_tmp5A3.tvars != 0  || _tmp5A5.tvars != 0){struct Cyc_List_List*
_tmp5B2=_tmp5A3.tvars;struct Cyc_List_List*_tmp5B3=_tmp5A5.tvars;if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp5B2)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp5B3))return 0;{struct _RegionHandle _tmp5B4=_new_region("r");
struct _RegionHandle*r=& _tmp5B4;_push_region(r);{struct Cyc_List_List*inst=0;
while(_tmp5B2 != 0){if(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp5B2->hd)!= 
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp5B3))->hd)){int _tmp5B5=0;_npop_handler(0);return _tmp5B5;}inst=({struct Cyc_List_List*
_tmp5B6=_region_malloc(r,sizeof(*_tmp5B6));_tmp5B6->hd=({struct _tuple8*_tmp5B7=
_region_malloc(r,sizeof(*_tmp5B7));_tmp5B7->f1=(struct Cyc_Absyn_Tvar*)_tmp5B3->hd;
_tmp5B7->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp5B8=_cycalloc(sizeof(*
_tmp5B8));_tmp5B8[0]=({struct Cyc_Absyn_VarType_struct _tmp5B9;_tmp5B9.tag=1;
_tmp5B9.f1=(struct Cyc_Absyn_Tvar*)_tmp5B2->hd;_tmp5B9;});_tmp5B8;});_tmp5B7;});
_tmp5B6->tl=inst;_tmp5B6;});_tmp5B2=_tmp5B2->tl;_tmp5B3=_tmp5B3->tl;}if(inst != 0){
_tmp5A3.tvars=0;_tmp5A5.tvars=0;{int _tmp5BE=Cyc_Tcutil_subtype(te,assume,(void*)({
struct Cyc_Absyn_FnType_struct*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA[0]=({
struct Cyc_Absyn_FnType_struct _tmp5BB;_tmp5BB.tag=8;_tmp5BB.f1=_tmp5A3;_tmp5BB;});
_tmp5BA;}),(void*)({struct Cyc_Absyn_FnType_struct*_tmp5BC=_cycalloc(sizeof(*
_tmp5BC));_tmp5BC[0]=({struct Cyc_Absyn_FnType_struct _tmp5BD;_tmp5BD.tag=8;
_tmp5BD.f1=_tmp5A5;_tmp5BD;});_tmp5BC;}));_npop_handler(0);return _tmp5BE;}}};
_pop_region(r);}}if(!Cyc_Tcutil_subtype(te,assume,(void*)_tmp5A3.ret_typ,(void*)
_tmp5A5.ret_typ))return 0;{struct Cyc_List_List*_tmp5BF=_tmp5A3.args;struct Cyc_List_List*
_tmp5C0=_tmp5A5.args;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5BF)
!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5C0))return 0;for(0;
_tmp5BF != 0;(_tmp5BF=_tmp5BF->tl,_tmp5C0=_tmp5C0->tl)){struct Cyc_Absyn_Tqual
_tmp5C2;void*_tmp5C3;struct _tuple2 _tmp5C1=*((struct _tuple2*)_tmp5BF->hd);_tmp5C2=
_tmp5C1.f2;_tmp5C3=_tmp5C1.f3;{struct Cyc_Absyn_Tqual _tmp5C5;void*_tmp5C6;struct
_tuple2 _tmp5C4=*((struct _tuple2*)((struct Cyc_List_List*)_check_null(_tmp5C0))->hd);
_tmp5C5=_tmp5C4.f2;_tmp5C6=_tmp5C4.f3;if(_tmp5C5.real_const  && !_tmp5C2.real_const
 || !Cyc_Tcutil_subtype(te,assume,_tmp5C6,_tmp5C3))return 0;}}if(_tmp5A3.c_varargs
!= _tmp5A5.c_varargs)return 0;if(_tmp5A3.cyc_varargs != 0  && _tmp5A5.cyc_varargs != 
0){struct Cyc_Absyn_VarargInfo _tmp5C7=*_tmp5A3.cyc_varargs;struct Cyc_Absyn_VarargInfo
_tmp5C8=*_tmp5A5.cyc_varargs;if((_tmp5C8.tq).real_const  && !(_tmp5C7.tq).real_const
 || !Cyc_Tcutil_subtype(te,assume,(void*)_tmp5C8.type,(void*)_tmp5C7.type))
return 0;}else{if(_tmp5A3.cyc_varargs != 0  || _tmp5A5.cyc_varargs != 0)return 0;}if(
!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5A3.effect))->v,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp5A5.effect))->v))return 0;if(!Cyc_Tcutil_sub_rgnpo(
_tmp5A3.rgn_po,_tmp5A5.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(_tmp5A3.attributes,
_tmp5A5.attributes))return 0;return 1;}_LL3D5:;_LL3D6: return 0;_LL3CE:;}}static int
Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple6 _tmp5CA=({struct _tuple6
_tmp5C9;_tmp5C9.f1=Cyc_Tcutil_compress(t1);_tmp5C9.f2=Cyc_Tcutil_compress(t2);
_tmp5C9;});void*_tmp5CB;void*_tmp5CC;void*_tmp5CD;void*_tmp5CE;_LL3E1: _tmp5CB=
_tmp5CA.f1;if(_tmp5CB <= (void*)4)goto _LL3E3;if(*((int*)_tmp5CB)!= 5)goto _LL3E3;
_tmp5CC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5CB)->f2;_tmp5CD=_tmp5CA.f2;
if(_tmp5CD <= (void*)4)goto _LL3E3;if(*((int*)_tmp5CD)!= 5)goto _LL3E3;_tmp5CE=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp5CD)->f2;_LL3E2: return(_tmp5CC == 
_tmp5CE  || _tmp5CC == (void*)2  && _tmp5CE == (void*)3) || _tmp5CC == (void*)3  && 
_tmp5CE == (void*)2;_LL3E3:;_LL3E4: return 0;_LL3E0:;}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*
tqs1=Cyc_Tcutil_flatten_typ(te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(
te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct
_tuple4 _tmp5D0;struct Cyc_Absyn_Tqual _tmp5D1;void*_tmp5D2;struct _tuple4*_tmp5CF=(
struct _tuple4*)tqs1->hd;_tmp5D0=*_tmp5CF;_tmp5D1=_tmp5D0.f1;_tmp5D2=_tmp5D0.f2;{
struct _tuple4 _tmp5D4;struct Cyc_Absyn_Tqual _tmp5D5;void*_tmp5D6;struct _tuple4*
_tmp5D3=(struct _tuple4*)tqs2->hd;_tmp5D4=*_tmp5D3;_tmp5D5=_tmp5D4.f1;_tmp5D6=
_tmp5D4.f2;if(_tmp5D5.real_const  && Cyc_Tcutil_subtype(te,assume,_tmp5D2,_tmp5D6))
continue;else{if(Cyc_Tcutil_unify(_tmp5D2,_tmp5D6))continue;else{if(Cyc_Tcutil_isomorphic(
_tmp5D2,_tmp5D6))continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(
void*t){void*_tmp5D7=Cyc_Tcutil_compress(t);void*_tmp5D8;_LL3E6: if(_tmp5D7 <= (
void*)4)goto _LL3E8;if(*((int*)_tmp5D7)!= 5)goto _LL3E8;_tmp5D8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5D7)->f2;if((int)_tmp5D8 != 0)goto _LL3E8;_LL3E7: return 1;_LL3E8:;_LL3E9: return
0;_LL3E5:;}void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return(void*)1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t2 == (void*)0)return(void*)1;{void*_tmp5D9=t2;
void*_tmp5DA;void*_tmp5DB;_LL3EB: if(_tmp5D9 <= (void*)4)goto _LL3EF;if(*((int*)
_tmp5D9)!= 5)goto _LL3ED;_tmp5DA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5D9)->f2;
if((int)_tmp5DA != 2)goto _LL3ED;_LL3EC: goto _LL3EE;_LL3ED: if(*((int*)_tmp5D9)!= 5)
goto _LL3EF;_tmp5DB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5D9)->f2;if((int)
_tmp5DB != 3)goto _LL3EF;_LL3EE: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return(void*)
1;goto _LL3EA;_LL3EF:;_LL3F0: goto _LL3EA;_LL3EA:;}{void*_tmp5DC=t1;struct Cyc_Absyn_PtrInfo
_tmp5DD;void*_tmp5DE;struct Cyc_Absyn_Tqual _tmp5DF;struct Cyc_Absyn_PtrAtts _tmp5E0;
void*_tmp5E1;struct Cyc_Absyn_Conref*_tmp5E2;struct Cyc_Absyn_Conref*_tmp5E3;
struct Cyc_Absyn_Conref*_tmp5E4;struct Cyc_Absyn_ArrayInfo _tmp5E5;void*_tmp5E6;
struct Cyc_Absyn_Tqual _tmp5E7;struct Cyc_Absyn_Exp*_tmp5E8;struct Cyc_Absyn_Conref*
_tmp5E9;struct Cyc_Absyn_Enumdecl*_tmp5EA;_LL3F2: if(_tmp5DC <= (void*)4)goto _LL3FA;
if(*((int*)_tmp5DC)!= 4)goto _LL3F4;_tmp5DD=((struct Cyc_Absyn_PointerType_struct*)
_tmp5DC)->f1;_tmp5DE=(void*)_tmp5DD.elt_typ;_tmp5DF=_tmp5DD.elt_tq;_tmp5E0=
_tmp5DD.ptr_atts;_tmp5E1=(void*)_tmp5E0.rgn;_tmp5E2=_tmp5E0.nullable;_tmp5E3=
_tmp5E0.bounds;_tmp5E4=_tmp5E0.zero_term;_LL3F3:{void*_tmp5EB=t2;struct Cyc_Absyn_PtrInfo
_tmp5EC;void*_tmp5ED;struct Cyc_Absyn_Tqual _tmp5EE;struct Cyc_Absyn_PtrAtts _tmp5EF;
void*_tmp5F0;struct Cyc_Absyn_Conref*_tmp5F1;struct Cyc_Absyn_Conref*_tmp5F2;
struct Cyc_Absyn_Conref*_tmp5F3;_LL401: if(_tmp5EB <= (void*)4)goto _LL403;if(*((int*)
_tmp5EB)!= 4)goto _LL403;_tmp5EC=((struct Cyc_Absyn_PointerType_struct*)_tmp5EB)->f1;
_tmp5ED=(void*)_tmp5EC.elt_typ;_tmp5EE=_tmp5EC.elt_tq;_tmp5EF=_tmp5EC.ptr_atts;
_tmp5F0=(void*)_tmp5EF.rgn;_tmp5F1=_tmp5EF.nullable;_tmp5F2=_tmp5EF.bounds;
_tmp5F3=_tmp5EF.zero_term;_LL402: {void*coercion=(void*)3;struct Cyc_List_List*
_tmp5F4=({struct Cyc_List_List*_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603->hd=({
struct _tuple6*_tmp604=_cycalloc(sizeof(*_tmp604));_tmp604->f1=t1;_tmp604->f2=t2;
_tmp604;});_tmp603->tl=0;_tmp603;});int _tmp5F5=Cyc_Tcutil_ptrsubtype(te,_tmp5F4,
_tmp5DE,_tmp5ED) && (!_tmp5DF.real_const  || _tmp5EE.real_const);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp5E4,_tmp5F3) || !((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp5F3);int _tmp5F6=_tmp5F5?0:((Cyc_Tcutil_bits_only(_tmp5DE) && Cyc_Tcutil_is_char_type(
_tmp5ED)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5F3)) && (_tmp5EE.real_const  || !_tmp5DF.real_const);int bounds_ok=Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,_tmp5E3,_tmp5F2);if(!bounds_ok  && !_tmp5F6){struct
_tuple6 _tmp5F8=({struct _tuple6 _tmp5F7;_tmp5F7.f1=Cyc_Absyn_conref_val(_tmp5E3);
_tmp5F7.f2=Cyc_Absyn_conref_val(_tmp5F2);_tmp5F7;});void*_tmp5F9;struct Cyc_Absyn_Exp*
_tmp5FA;void*_tmp5FB;struct Cyc_Absyn_Exp*_tmp5FC;void*_tmp5FD;void*_tmp5FE;void*
_tmp5FF;void*_tmp600;void*_tmp601;void*_tmp602;_LL406: _tmp5F9=_tmp5F8.f1;if(
_tmp5F9 <= (void*)2)goto _LL408;if(*((int*)_tmp5F9)!= 0)goto _LL408;_tmp5FA=((
struct Cyc_Absyn_Upper_b_struct*)_tmp5F9)->f1;_tmp5FB=_tmp5F8.f2;if(_tmp5FB <= (
void*)2)goto _LL408;if(*((int*)_tmp5FB)!= 0)goto _LL408;_tmp5FC=((struct Cyc_Absyn_Upper_b_struct*)
_tmp5FB)->f1;_LL407: if(Cyc_Evexp_lte_const_exp(_tmp5FC,_tmp5FA))bounds_ok=1;goto
_LL405;_LL408: _tmp5FD=_tmp5F8.f1;if(_tmp5FD <= (void*)2)goto _LL40A;if(*((int*)
_tmp5FD)!= 1)goto _LL40A;_tmp5FE=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp5FD)->f1;_tmp5FF=_tmp5F8.f2;if(_tmp5FF <= (void*)2)goto _LL40A;if(*((int*)
_tmp5FF)!= 1)goto _LL40A;_tmp600=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp5FF)->f1;_LL409: bounds_ok=Cyc_Tcutil_unify(_tmp5FE,_tmp600);goto _LL405;
_LL40A: _tmp601=_tmp5F8.f1;if(_tmp601 <= (void*)2)goto _LL40C;if(*((int*)_tmp601)!= 
1)goto _LL40C;_LL40B: goto _LL40D;_LL40C: _tmp602=_tmp5F8.f2;if(_tmp602 <= (void*)2)
goto _LL40E;if(*((int*)_tmp602)!= 1)goto _LL40E;_LL40D: bounds_ok=0;goto _LL405;
_LL40E:;_LL40F: bounds_ok=1;goto _LL405;_LL405:;}if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp5E2) && !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp5F1))coercion=(void*)2;if(((bounds_ok  && zeroterm_ok)
 && (_tmp5F5  || _tmp5F6)) && (Cyc_Tcutil_unify(_tmp5E1,_tmp5F0) || Cyc_Tcenv_region_outlives(
te,_tmp5E1,_tmp5F0)))return coercion;else{return(void*)0;}}}_LL403:;_LL404: goto
_LL400;_LL400:;}return(void*)0;_LL3F4: if(*((int*)_tmp5DC)!= 7)goto _LL3F6;_tmp5E5=((
struct Cyc_Absyn_ArrayType_struct*)_tmp5DC)->f1;_tmp5E6=(void*)_tmp5E5.elt_type;
_tmp5E7=_tmp5E5.tq;_tmp5E8=_tmp5E5.num_elts;_tmp5E9=_tmp5E5.zero_term;_LL3F5:{
void*_tmp605=t2;struct Cyc_Absyn_ArrayInfo _tmp606;void*_tmp607;struct Cyc_Absyn_Tqual
_tmp608;struct Cyc_Absyn_Exp*_tmp609;struct Cyc_Absyn_Conref*_tmp60A;_LL411: if(
_tmp605 <= (void*)4)goto _LL413;if(*((int*)_tmp605)!= 7)goto _LL413;_tmp606=((
struct Cyc_Absyn_ArrayType_struct*)_tmp605)->f1;_tmp607=(void*)_tmp606.elt_type;
_tmp608=_tmp606.tq;_tmp609=_tmp606.num_elts;_tmp60A=_tmp606.zero_term;_LL412: {
int okay;okay=((_tmp5E8 != 0  && _tmp609 != 0) && ((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp5E9,_tmp60A)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp609,(
struct Cyc_Absyn_Exp*)_tmp5E8);return(okay  && Cyc_Tcutil_unify(_tmp5E6,_tmp607))
 && (!_tmp5E7.real_const  || _tmp608.real_const)?(void*)3:(void*)0;}_LL413:;
_LL414: return(void*)0;_LL410:;}return(void*)0;_LL3F6: if(*((int*)_tmp5DC)!= 12)
goto _LL3F8;_tmp5EA=((struct Cyc_Absyn_EnumType_struct*)_tmp5DC)->f2;_LL3F7:{void*
_tmp60B=t2;struct Cyc_Absyn_Enumdecl*_tmp60C;_LL416: if(_tmp60B <= (void*)4)goto
_LL418;if(*((int*)_tmp60B)!= 12)goto _LL418;_tmp60C=((struct Cyc_Absyn_EnumType_struct*)
_tmp60B)->f2;_LL417: if((_tmp5EA->fields != 0  && _tmp60C->fields != 0) && ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp5EA->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp60C->fields))->v))
return(void*)1;goto _LL415;_LL418:;_LL419: goto _LL415;_LL415:;}goto _LL3F9;_LL3F8:
if(*((int*)_tmp5DC)!= 5)goto _LL3FA;_LL3F9: goto _LL3FB;_LL3FA: if((int)_tmp5DC != 1)
goto _LL3FC;_LL3FB: goto _LL3FD;_LL3FC: if(_tmp5DC <= (void*)4)goto _LL3FE;if(*((int*)
_tmp5DC)!= 6)goto _LL3FE;_LL3FD: return Cyc_Tcutil_coerceable(t2)?(void*)1:(void*)0;
_LL3FE:;_LL3FF: return(void*)0;_LL3F1:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,void*c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp60D=Cyc_Absyn_copy_exp(e);(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*_tmp60E=_cycalloc(
sizeof(*_tmp60E));_tmp60E[0]=({struct Cyc_Absyn_Cast_e_struct _tmp60F;_tmp60F.tag=
15;_tmp60F.f1=(void*)t;_tmp60F.f2=_tmp60D;_tmp60F.f3=0;_tmp60F.f4=(void*)c;
_tmp60F;});_tmp60E;})));e->topt=({struct Cyc_Core_Opt*_tmp610=_cycalloc(sizeof(*
_tmp610));_tmp610->v=(void*)t;_tmp610;});}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*
e){void*_tmp611=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL41B: if(_tmp611 <= (void*)4)goto _LL427;if(*((int*)_tmp611)!= 5)goto _LL41D;
_LL41C: goto _LL41E;_LL41D: if(*((int*)_tmp611)!= 12)goto _LL41F;_LL41E: goto _LL420;
_LL41F: if(*((int*)_tmp611)!= 13)goto _LL421;_LL420: goto _LL422;_LL421: if(*((int*)
_tmp611)!= 17)goto _LL423;_LL422: goto _LL424;_LL423: if(*((int*)_tmp611)!= 14)goto
_LL425;_LL424: return 1;_LL425: if(*((int*)_tmp611)!= 0)goto _LL427;_LL426: return Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL427:;
_LL428: return 0;_LL41A:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp612=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL42A: if((int)_tmp612 != 1)goto _LL42C;_LL42B: goto
_LL42D;_LL42C: if(_tmp612 <= (void*)4)goto _LL42E;if(*((int*)_tmp612)!= 6)goto
_LL42E;_LL42D: return 1;_LL42E:;_LL42F: return 0;_LL429:;}}int Cyc_Tcutil_is_function_type(
void*t){void*_tmp613=Cyc_Tcutil_compress(t);_LL431: if(_tmp613 <= (void*)4)goto
_LL433;if(*((int*)_tmp613)!= 8)goto _LL433;_LL432: return 1;_LL433:;_LL434: return 0;
_LL430:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct _tuple6
_tmp615=({struct _tuple6 _tmp614;_tmp614.f1=t1;_tmp614.f2=t2;_tmp614;});void*
_tmp616;int _tmp617;void*_tmp618;int _tmp619;void*_tmp61A;void*_tmp61B;void*
_tmp61C;void*_tmp61D;void*_tmp61E;void*_tmp61F;void*_tmp620;void*_tmp621;void*
_tmp622;void*_tmp623;void*_tmp624;void*_tmp625;void*_tmp626;void*_tmp627;void*
_tmp628;void*_tmp629;void*_tmp62A;void*_tmp62B;void*_tmp62C;void*_tmp62D;void*
_tmp62E;void*_tmp62F;void*_tmp630;void*_tmp631;void*_tmp632;void*_tmp633;void*
_tmp634;void*_tmp635;void*_tmp636;void*_tmp637;_LL436: _tmp616=_tmp615.f1;if(
_tmp616 <= (void*)4)goto _LL438;if(*((int*)_tmp616)!= 6)goto _LL438;_tmp617=((
struct Cyc_Absyn_DoubleType_struct*)_tmp616)->f1;_tmp618=_tmp615.f2;if(_tmp618 <= (
void*)4)goto _LL438;if(*((int*)_tmp618)!= 6)goto _LL438;_tmp619=((struct Cyc_Absyn_DoubleType_struct*)
_tmp618)->f1;_LL437: if(_tmp617)return t1;else{return t2;}_LL438: _tmp61A=_tmp615.f1;
if(_tmp61A <= (void*)4)goto _LL43A;if(*((int*)_tmp61A)!= 6)goto _LL43A;_LL439:
return t1;_LL43A: _tmp61B=_tmp615.f2;if(_tmp61B <= (void*)4)goto _LL43C;if(*((int*)
_tmp61B)!= 6)goto _LL43C;_LL43B: return t2;_LL43C: _tmp61C=_tmp615.f1;if((int)
_tmp61C != 1)goto _LL43E;_LL43D: goto _LL43F;_LL43E: _tmp61D=_tmp615.f2;if((int)
_tmp61D != 1)goto _LL440;_LL43F: return(void*)1;_LL440: _tmp61E=_tmp615.f1;if(
_tmp61E <= (void*)4)goto _LL442;if(*((int*)_tmp61E)!= 5)goto _LL442;_tmp61F=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp61E)->f1;if((int)_tmp61F != 1)goto _LL442;
_tmp620=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp61E)->f2;if((int)_tmp620 != 
4)goto _LL442;_LL441: goto _LL443;_LL442: _tmp621=_tmp615.f2;if(_tmp621 <= (void*)4)
goto _LL444;if(*((int*)_tmp621)!= 5)goto _LL444;_tmp622=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp621)->f1;if((int)_tmp622 != 1)goto _LL444;_tmp623=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp621)->f2;if((int)_tmp623 != 4)goto _LL444;_LL443: return Cyc_Absyn_ulonglong_typ;
_LL444: _tmp624=_tmp615.f1;if(_tmp624 <= (void*)4)goto _LL446;if(*((int*)_tmp624)!= 
5)goto _LL446;_tmp625=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp624)->f2;if((
int)_tmp625 != 4)goto _LL446;_LL445: goto _LL447;_LL446: _tmp626=_tmp615.f2;if(
_tmp626 <= (void*)4)goto _LL448;if(*((int*)_tmp626)!= 5)goto _LL448;_tmp627=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp626)->f2;if((int)_tmp627 != 4)goto _LL448;
_LL447: return Cyc_Absyn_slonglong_typ;_LL448: _tmp628=_tmp615.f1;if(_tmp628 <= (
void*)4)goto _LL44A;if(*((int*)_tmp628)!= 5)goto _LL44A;_tmp629=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp628)->f1;if((int)_tmp629 != 1)goto _LL44A;_tmp62A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp628)->f2;if((int)_tmp62A != 3)goto _LL44A;_LL449: goto _LL44B;_LL44A: _tmp62B=
_tmp615.f2;if(_tmp62B <= (void*)4)goto _LL44C;if(*((int*)_tmp62B)!= 5)goto _LL44C;
_tmp62C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp62B)->f1;if((int)_tmp62C != 
1)goto _LL44C;_tmp62D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp62B)->f2;if((
int)_tmp62D != 3)goto _LL44C;_LL44B: return Cyc_Absyn_ulong_typ;_LL44C: _tmp62E=
_tmp615.f1;if(_tmp62E <= (void*)4)goto _LL44E;if(*((int*)_tmp62E)!= 5)goto _LL44E;
_tmp62F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp62E)->f1;if((int)_tmp62F != 
1)goto _LL44E;_tmp630=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp62E)->f2;if((
int)_tmp630 != 2)goto _LL44E;_LL44D: goto _LL44F;_LL44E: _tmp631=_tmp615.f2;if(
_tmp631 <= (void*)4)goto _LL450;if(*((int*)_tmp631)!= 5)goto _LL450;_tmp632=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp631)->f1;if((int)_tmp632 != 1)goto _LL450;
_tmp633=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp631)->f2;if((int)_tmp633 != 
2)goto _LL450;_LL44F: return Cyc_Absyn_uint_typ;_LL450: _tmp634=_tmp615.f1;if(
_tmp634 <= (void*)4)goto _LL452;if(*((int*)_tmp634)!= 5)goto _LL452;_tmp635=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp634)->f2;if((int)_tmp635 != 3)goto _LL452;
_LL451: goto _LL453;_LL452: _tmp636=_tmp615.f2;if(_tmp636 <= (void*)4)goto _LL454;if(*((
int*)_tmp636)!= 5)goto _LL454;_tmp637=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp636)->f2;if((int)_tmp637 != 3)goto _LL454;_LL453: return Cyc_Absyn_slong_typ;
_LL454:;_LL455: return Cyc_Absyn_sint_typ;_LL435:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp638=(void*)e->r;struct Cyc_Core_Opt*_tmp639;
_LL457: if(*((int*)_tmp638)!= 4)goto _LL459;_tmp639=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp638)->f2;if(_tmp639 != 0)goto _LL459;_LL458:({void*_tmp63A[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp63B="assignment in test";_tag_dynforward(_tmp63B,sizeof(
char),_get_zero_arr_size(_tmp63B,19));}),_tag_dynforward(_tmp63A,sizeof(void*),0));});
goto _LL456;_LL459:;_LL45A: goto _LL456;_LL456:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple6 _tmp63D=({struct _tuple6 _tmp63C;_tmp63C.f1=c1;_tmp63C.f2=c2;_tmp63C;});
void*_tmp63E;void*_tmp63F;void*_tmp640;void*_tmp641;void*_tmp642;struct Cyc_Core_Opt*
_tmp643;struct Cyc_Core_Opt**_tmp644;void*_tmp645;struct Cyc_Core_Opt*_tmp646;
struct Cyc_Core_Opt**_tmp647;void*_tmp648;struct Cyc_Core_Opt*_tmp649;struct Cyc_Core_Opt**
_tmp64A;void*_tmp64B;void*_tmp64C;void*_tmp64D;void*_tmp64E;void*_tmp64F;void*
_tmp650;struct Cyc_Core_Opt*_tmp651;struct Cyc_Core_Opt**_tmp652;void*_tmp653;void*
_tmp654;struct Cyc_Core_Opt*_tmp655;struct Cyc_Core_Opt**_tmp656;void*_tmp657;void*
_tmp658;struct Cyc_Core_Opt*_tmp659;struct Cyc_Core_Opt**_tmp65A;void*_tmp65B;
_LL45C: _tmp63E=_tmp63D.f1;if(*((int*)_tmp63E)!= 0)goto _LL45E;_tmp63F=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp63E)->f1;_tmp640=_tmp63D.f2;if(*((int*)_tmp640)
!= 0)goto _LL45E;_tmp641=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp640)->f1;
_LL45D: return _tmp63F == _tmp641;_LL45E: _tmp642=_tmp63D.f2;if(*((int*)_tmp642)!= 1)
goto _LL460;_tmp643=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp642)->f1;_tmp644=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp642)->f1;_LL45F:*
_tmp644=({struct Cyc_Core_Opt*_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C->v=(
void*)c1;_tmp65C;});return 1;_LL460: _tmp645=_tmp63D.f1;if(*((int*)_tmp645)!= 1)
goto _LL462;_tmp646=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp645)->f1;_tmp647=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp645)->f1;_LL461:*
_tmp647=({struct Cyc_Core_Opt*_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D->v=(
void*)c2;_tmp65D;});return 1;_LL462: _tmp648=_tmp63D.f1;if(*((int*)_tmp648)!= 2)
goto _LL464;_tmp649=((struct Cyc_Absyn_Less_kb_struct*)_tmp648)->f1;_tmp64A=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp648)->f1;_tmp64B=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp648)->f2;_tmp64C=_tmp63D.f2;if(*((
int*)_tmp64C)!= 0)goto _LL464;_tmp64D=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp64C)->f1;_LL463: if(Cyc_Tcutil_kind_leq(_tmp64D,_tmp64B)){*_tmp64A=({struct
Cyc_Core_Opt*_tmp65E=_cycalloc(sizeof(*_tmp65E));_tmp65E->v=(void*)c2;_tmp65E;});
return 1;}else{return 0;}_LL464: _tmp64E=_tmp63D.f1;if(*((int*)_tmp64E)!= 0)goto
_LL466;_tmp64F=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp64E)->f1;_tmp650=
_tmp63D.f2;if(*((int*)_tmp650)!= 2)goto _LL466;_tmp651=((struct Cyc_Absyn_Less_kb_struct*)
_tmp650)->f1;_tmp652=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp650)->f1;_tmp653=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp650)->f2;
_LL465: if(Cyc_Tcutil_kind_leq(_tmp64F,_tmp653)){*_tmp652=({struct Cyc_Core_Opt*
_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F->v=(void*)c1;_tmp65F;});return 1;}
else{return 0;}_LL466: _tmp654=_tmp63D.f1;if(*((int*)_tmp654)!= 2)goto _LL45B;
_tmp655=((struct Cyc_Absyn_Less_kb_struct*)_tmp654)->f1;_tmp656=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp654)->f1;_tmp657=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp654)->f2;_tmp658=_tmp63D.f2;if(*((int*)_tmp658)!= 2)goto _LL45B;_tmp659=((
struct Cyc_Absyn_Less_kb_struct*)_tmp658)->f1;_tmp65A=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp658)->f1;_tmp65B=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp658)->f2;_LL467: if(Cyc_Tcutil_kind_leq(_tmp657,_tmp65B)){*_tmp65A=({struct
Cyc_Core_Opt*_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660->v=(void*)c1;_tmp660;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp65B,_tmp657)){*_tmp656=({struct Cyc_Core_Opt*
_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661->v=(void*)c2;_tmp661;});return 1;}
else{return 0;}}_LL45B:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp662=_cycalloc_atomic(sizeof(*_tmp662));_tmp662[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp662;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _dynforward_ptr s=(struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp667;_tmp667.tag=1;_tmp667.f1=(
unsigned long)i;{void*_tmp665[1]={& _tmp667};Cyc_aprintf(({const char*_tmp666="#%d";
_tag_dynforward(_tmp666,sizeof(char),_get_zero_arr_size(_tmp666,4));}),
_tag_dynforward(_tmp665,sizeof(void*),1));}});return({struct Cyc_Absyn_Tvar*
_tmp663=_cycalloc(sizeof(*_tmp663));_tmp663->name=({struct _dynforward_ptr*
_tmp664=_cycalloc(sizeof(struct _dynforward_ptr)* 1);_tmp664[0]=s;_tmp664;});
_tmp663->identity=0;_tmp663->kind=(void*)k;_tmp663;});}int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*t){struct _dynforward_ptr _tmp668=*t->name;return*((const char*)
_check_dynforward_subscript(_tmp668,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_String_pa_struct _tmp66B;_tmp66B.tag=0;
_tmp66B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*t->name);{void*
_tmp669[1]={& _tmp66B};Cyc_printf(({const char*_tmp66A="%s";_tag_dynforward(
_tmp66A,sizeof(char),_get_zero_arr_size(_tmp66A,3));}),_tag_dynforward(_tmp669,
sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(t))return;{struct
_dynforward_ptr _tmp66C=Cyc_strconcat(({const char*_tmp671="`";_tag_dynforward(
_tmp671,sizeof(char),_get_zero_arr_size(_tmp671,2));}),(struct _dynforward_ptr)*t->name);({
struct _dynforward_ptr _tmp66D=_dynforward_ptr_plus(_tmp66C,sizeof(char),1);char
_tmp66E=*((char*)_check_dynforward_subscript(_tmp66D,sizeof(char),0));char
_tmp66F='t';if(_get_dynforward_size(_tmp66D,sizeof(char))== 1  && (_tmp66E == '\000'
 && _tmp66F != '\000'))_throw_arraybounds();*((char*)_tmp66D.curr)=_tmp66F;});t->name=({
struct _dynforward_ptr*_tmp670=_cycalloc(sizeof(struct _dynforward_ptr)* 1);
_tmp670[0]=(struct _dynforward_ptr)_tmp66C;_tmp670;});}}struct _tuple13{struct
_dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple2*Cyc_Tcutil_fndecl2typ_f(
struct _tuple13*x){return({struct _tuple2*_tmp672=_cycalloc(sizeof(*_tmp672));
_tmp672->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp673=_cycalloc(sizeof(*
_tmp673));_tmp673->v=(*x).f1;_tmp673;});_tmp672->f2=(*x).f2;_tmp672->f3=(*x).f3;
_tmp672;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp674=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))_tmp674=({
struct Cyc_List_List*_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675->hd=(void*)((
void*)atts->hd);_tmp675->tl=_tmp674;_tmp675;});}}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676[0]=({struct Cyc_Absyn_FnType_struct
_tmp677;_tmp677.tag=8;_tmp677.f1=({struct Cyc_Absyn_FnInfo _tmp678;_tmp678.tvars=
fd->tvs;_tmp678.effect=fd->effect;_tmp678.ret_typ=(void*)((void*)fd->ret_type);
_tmp678.args=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple13*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp678.c_varargs=
fd->c_varargs;_tmp678.cyc_varargs=fd->cyc_varargs;_tmp678.rgn_po=fd->rgn_po;
_tmp678.attributes=_tmp674;_tmp678;});_tmp677;});_tmp676;});}return(void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}struct _tuple14{void*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple14*
t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*t).f2;}static
struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({struct _tuple4*
_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679->f1=(*pr).f1;_tmp679->f2=t;_tmp679;});}
struct _tuple15{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple16{
struct _tuple15*f1;void*f2;};static struct _tuple16*Cyc_Tcutil_substitute_f1(struct
_RegionHandle*rgn,struct _tuple2*y){return({struct _tuple16*_tmp67A=_region_malloc(
rgn,sizeof(*_tmp67A));_tmp67A->f1=({struct _tuple15*_tmp67B=_region_malloc(rgn,
sizeof(*_tmp67B));_tmp67B->f1=(*y).f1;_tmp67B->f2=(*y).f2;_tmp67B;});_tmp67A->f2=(*
y).f3;_tmp67A;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(struct _tuple16*w){
struct _tuple15*_tmp67D;void*_tmp67E;struct _tuple16 _tmp67C=*w;_tmp67D=_tmp67C.f1;
_tmp67E=_tmp67C.f2;{struct Cyc_Core_Opt*_tmp680;struct Cyc_Absyn_Tqual _tmp681;
struct _tuple15 _tmp67F=*_tmp67D;_tmp680=_tmp67F.f1;_tmp681=_tmp67F.f2;return({
struct _tuple2*_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682->f1=_tmp680;_tmp682->f2=
_tmp681;_tmp682->f3=_tmp67E;_tmp682;});}}static void*Cyc_Tcutil_field_type(struct
Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*_tmp683=
_cycalloc(sizeof(*_tmp683));_tmp683->name=f->name;_tmp683->tq=f->tq;_tmp683->type=(
void*)t;_tmp683->width=f->width;_tmp683->attributes=f->attributes;_tmp683;});}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,void*t){void*_tmp684=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp685;struct Cyc_Absyn_AggrInfo _tmp686;void*_tmp687;struct Cyc_List_List*_tmp688;
struct Cyc_Absyn_TunionInfo _tmp689;void*_tmp68A;struct Cyc_List_List*_tmp68B;
struct Cyc_Core_Opt*_tmp68C;struct Cyc_Absyn_TunionFieldInfo _tmp68D;void*_tmp68E;
struct Cyc_List_List*_tmp68F;struct _tuple1*_tmp690;struct Cyc_List_List*_tmp691;
struct Cyc_Absyn_Typedefdecl*_tmp692;void**_tmp693;struct Cyc_Absyn_ArrayInfo
_tmp694;void*_tmp695;struct Cyc_Absyn_Tqual _tmp696;struct Cyc_Absyn_Exp*_tmp697;
struct Cyc_Absyn_Conref*_tmp698;struct Cyc_Position_Segment*_tmp699;struct Cyc_Absyn_PtrInfo
_tmp69A;void*_tmp69B;struct Cyc_Absyn_Tqual _tmp69C;struct Cyc_Absyn_PtrAtts _tmp69D;
void*_tmp69E;struct Cyc_Absyn_Conref*_tmp69F;struct Cyc_Absyn_Conref*_tmp6A0;
struct Cyc_Absyn_Conref*_tmp6A1;struct Cyc_Absyn_FnInfo _tmp6A2;struct Cyc_List_List*
_tmp6A3;struct Cyc_Core_Opt*_tmp6A4;void*_tmp6A5;struct Cyc_List_List*_tmp6A6;int
_tmp6A7;struct Cyc_Absyn_VarargInfo*_tmp6A8;struct Cyc_List_List*_tmp6A9;struct Cyc_List_List*
_tmp6AA;struct Cyc_List_List*_tmp6AB;void*_tmp6AC;struct Cyc_List_List*_tmp6AD;
struct Cyc_Core_Opt*_tmp6AE;void*_tmp6AF;void*_tmp6B0;void*_tmp6B1;void*_tmp6B2;
void*_tmp6B3;struct Cyc_List_List*_tmp6B4;_LL469: if(_tmp684 <= (void*)4)goto _LL48B;
if(*((int*)_tmp684)!= 1)goto _LL46B;_tmp685=((struct Cyc_Absyn_VarType_struct*)
_tmp684)->f1;_LL46A: {struct _handler_cons _tmp6B5;_push_handler(& _tmp6B5);{int
_tmp6B7=0;if(setjmp(_tmp6B5.handler))_tmp6B7=1;if(!_tmp6B7){{void*_tmp6B8=((void*(*)(
int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp685);
_npop_handler(0);return _tmp6B8;};_pop_handler();}else{void*_tmp6B6=(void*)
_exn_thrown;void*_tmp6BA=_tmp6B6;_LL49E: if(_tmp6BA != Cyc_Core_Not_found)goto
_LL4A0;_LL49F: return t;_LL4A0:;_LL4A1:(void)_throw(_tmp6BA);_LL49D:;}}}_LL46B: if(*((
int*)_tmp684)!= 10)goto _LL46D;_tmp686=((struct Cyc_Absyn_AggrType_struct*)_tmp684)->f1;
_tmp687=(void*)_tmp686.aggr_info;_tmp688=_tmp686.targs;_LL46C: {struct Cyc_List_List*
_tmp6BB=Cyc_Tcutil_substs(rgn,inst,_tmp688);return _tmp6BB == _tmp688?t:(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC[0]=({
struct Cyc_Absyn_AggrType_struct _tmp6BD;_tmp6BD.tag=10;_tmp6BD.f1=({struct Cyc_Absyn_AggrInfo
_tmp6BE;_tmp6BE.aggr_info=(void*)_tmp687;_tmp6BE.targs=_tmp6BB;_tmp6BE;});
_tmp6BD;});_tmp6BC;});}_LL46D: if(*((int*)_tmp684)!= 2)goto _LL46F;_tmp689=((
struct Cyc_Absyn_TunionType_struct*)_tmp684)->f1;_tmp68A=(void*)_tmp689.tunion_info;
_tmp68B=_tmp689.targs;_tmp68C=_tmp689.rgn;_LL46E: {struct Cyc_List_List*_tmp6BF=
Cyc_Tcutil_substs(rgn,inst,_tmp68B);struct Cyc_Core_Opt*new_r;if((unsigned int)
_tmp68C){void*_tmp6C0=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmp68C->v);if(
_tmp6C0 == (void*)_tmp68C->v)new_r=_tmp68C;else{new_r=({struct Cyc_Core_Opt*
_tmp6C1=_cycalloc(sizeof(*_tmp6C1));_tmp6C1->v=(void*)_tmp6C0;_tmp6C1;});}}else{
new_r=_tmp68C;}return _tmp6BF == _tmp68B  && new_r == _tmp68C?t:(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp6C2=_cycalloc(sizeof(*_tmp6C2));_tmp6C2[0]=({struct Cyc_Absyn_TunionType_struct
_tmp6C3;_tmp6C3.tag=2;_tmp6C3.f1=({struct Cyc_Absyn_TunionInfo _tmp6C4;_tmp6C4.tunion_info=(
void*)_tmp68A;_tmp6C4.targs=_tmp6BF;_tmp6C4.rgn=new_r;_tmp6C4;});_tmp6C3;});
_tmp6C2;});}_LL46F: if(*((int*)_tmp684)!= 3)goto _LL471;_tmp68D=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp684)->f1;_tmp68E=(void*)_tmp68D.field_info;_tmp68F=_tmp68D.targs;_LL470: {
struct Cyc_List_List*_tmp6C5=Cyc_Tcutil_substs(rgn,inst,_tmp68F);return _tmp6C5 == 
_tmp68F?t:(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp6C6=_cycalloc(
sizeof(*_tmp6C6));_tmp6C6[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp6C7;
_tmp6C7.tag=3;_tmp6C7.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp6C8;_tmp6C8.field_info=(
void*)_tmp68E;_tmp6C8.targs=_tmp6C5;_tmp6C8;});_tmp6C7;});_tmp6C6;});}_LL471: if(*((
int*)_tmp684)!= 16)goto _LL473;_tmp690=((struct Cyc_Absyn_TypedefType_struct*)
_tmp684)->f1;_tmp691=((struct Cyc_Absyn_TypedefType_struct*)_tmp684)->f2;_tmp692=((
struct Cyc_Absyn_TypedefType_struct*)_tmp684)->f3;_tmp693=((struct Cyc_Absyn_TypedefType_struct*)
_tmp684)->f4;_LL472: {struct Cyc_List_List*_tmp6C9=Cyc_Tcutil_substs(rgn,inst,
_tmp691);return _tmp6C9 == _tmp691?t:(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp6CB;_tmp6CB.tag=16;_tmp6CB.f1=_tmp690;_tmp6CB.f2=_tmp6C9;_tmp6CB.f3=_tmp692;
_tmp6CB.f4=_tmp693;_tmp6CB;});_tmp6CA;});}_LL473: if(*((int*)_tmp684)!= 7)goto
_LL475;_tmp694=((struct Cyc_Absyn_ArrayType_struct*)_tmp684)->f1;_tmp695=(void*)
_tmp694.elt_type;_tmp696=_tmp694.tq;_tmp697=_tmp694.num_elts;_tmp698=_tmp694.zero_term;
_tmp699=_tmp694.zt_loc;_LL474: {void*_tmp6CC=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp695);return _tmp6CC == _tmp695?t:(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp6CD=_cycalloc(sizeof(*_tmp6CD));_tmp6CD[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp6CE;_tmp6CE.tag=7;_tmp6CE.f1=({struct Cyc_Absyn_ArrayInfo _tmp6CF;_tmp6CF.elt_type=(
void*)_tmp6CC;_tmp6CF.tq=_tmp696;_tmp6CF.num_elts=_tmp697;_tmp6CF.zero_term=
_tmp698;_tmp6CF.zt_loc=_tmp699;_tmp6CF;});_tmp6CE;});_tmp6CD;});}_LL475: if(*((
int*)_tmp684)!= 4)goto _LL477;_tmp69A=((struct Cyc_Absyn_PointerType_struct*)
_tmp684)->f1;_tmp69B=(void*)_tmp69A.elt_typ;_tmp69C=_tmp69A.elt_tq;_tmp69D=
_tmp69A.ptr_atts;_tmp69E=(void*)_tmp69D.rgn;_tmp69F=_tmp69D.nullable;_tmp6A0=
_tmp69D.bounds;_tmp6A1=_tmp69D.zero_term;_LL476: {void*_tmp6D0=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp69B);void*_tmp6D1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp69E);struct
Cyc_Absyn_Conref*_tmp6D2=_tmp6A0;{void*_tmp6D3=(void*)(Cyc_Absyn_compress_conref(
_tmp6A0))->v;void*_tmp6D4;void*_tmp6D5;_LL4A3: if(_tmp6D3 <= (void*)1)goto _LL4A5;
if(*((int*)_tmp6D3)!= 0)goto _LL4A5;_tmp6D4=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp6D3)->f1;if(_tmp6D4 <= (void*)2)goto _LL4A5;if(*((int*)_tmp6D4)!= 1)goto _LL4A5;
_tmp6D5=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp6D4)->f1;_LL4A4: {void*
_tmp6D6=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D5);if(_tmp6D5 != _tmp6D6)_tmp6D2=
Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp6D7=
_cycalloc(sizeof(*_tmp6D7));_tmp6D7[0]=({struct Cyc_Absyn_AbsUpper_b_struct
_tmp6D8;_tmp6D8.tag=1;_tmp6D8.f1=(void*)_tmp6D6;_tmp6D8;});_tmp6D7;}));goto
_LL4A2;}_LL4A5:;_LL4A6: goto _LL4A2;_LL4A2:;}if((_tmp6D0 == _tmp69B  && _tmp6D1 == 
_tmp69E) && _tmp6D2 == _tmp6A0)return t;return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp6D9=_cycalloc(sizeof(*_tmp6D9));_tmp6D9[0]=({struct Cyc_Absyn_PointerType_struct
_tmp6DA;_tmp6DA.tag=4;_tmp6DA.f1=({struct Cyc_Absyn_PtrInfo _tmp6DB;_tmp6DB.elt_typ=(
void*)_tmp6D0;_tmp6DB.elt_tq=_tmp69C;_tmp6DB.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp6DC;_tmp6DC.rgn=(void*)_tmp6D1;_tmp6DC.nullable=_tmp69F;_tmp6DC.bounds=
_tmp6D2;_tmp6DC.zero_term=_tmp6A1;_tmp6DC.ptrloc=0;_tmp6DC;});_tmp6DB;});_tmp6DA;});
_tmp6D9;});}_LL477: if(*((int*)_tmp684)!= 8)goto _LL479;_tmp6A2=((struct Cyc_Absyn_FnType_struct*)
_tmp684)->f1;_tmp6A3=_tmp6A2.tvars;_tmp6A4=_tmp6A2.effect;_tmp6A5=(void*)_tmp6A2.ret_typ;
_tmp6A6=_tmp6A2.args;_tmp6A7=_tmp6A2.c_varargs;_tmp6A8=_tmp6A2.cyc_varargs;
_tmp6A9=_tmp6A2.rgn_po;_tmp6AA=_tmp6A2.attributes;_LL478:{struct Cyc_List_List*
_tmp6DD=_tmp6A3;for(0;_tmp6DD != 0;_tmp6DD=_tmp6DD->tl){inst=({struct Cyc_List_List*
_tmp6DE=_region_malloc(rgn,sizeof(*_tmp6DE));_tmp6DE->hd=({struct _tuple8*_tmp6DF=
_region_malloc(rgn,sizeof(*_tmp6DF));_tmp6DF->f1=(struct Cyc_Absyn_Tvar*)_tmp6DD->hd;
_tmp6DF->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp6E0=_cycalloc(sizeof(*
_tmp6E0));_tmp6E0[0]=({struct Cyc_Absyn_VarType_struct _tmp6E1;_tmp6E1.tag=1;
_tmp6E1.f1=(struct Cyc_Absyn_Tvar*)_tmp6DD->hd;_tmp6E1;});_tmp6E0;});_tmp6DF;});
_tmp6DE->tl=inst;_tmp6DE;});}}{struct Cyc_List_List*_tmp6E3;struct Cyc_List_List*
_tmp6E4;struct _tuple0 _tmp6E2=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple16*(*f)(struct _RegionHandle*,struct _tuple2*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmp6A6));_tmp6E3=_tmp6E2.f1;_tmp6E4=_tmp6E2.f2;{struct Cyc_List_List*_tmp6E5=
Cyc_Tcutil_substs(rgn,inst,_tmp6E4);struct Cyc_List_List*_tmp6E6=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp6E3,_tmp6E5));struct Cyc_Core_Opt*
eff2;if(_tmp6A4 == 0)eff2=0;else{void*_tmp6E7=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp6A4->v);if(_tmp6E7 == (void*)_tmp6A4->v)eff2=_tmp6A4;else{eff2=({struct
Cyc_Core_Opt*_tmp6E8=_cycalloc(sizeof(*_tmp6E8));_tmp6E8->v=(void*)_tmp6E7;
_tmp6E8;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp6A8 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp6EA;struct Cyc_Absyn_Tqual _tmp6EB;void*
_tmp6EC;int _tmp6ED;struct Cyc_Absyn_VarargInfo _tmp6E9=*_tmp6A8;_tmp6EA=_tmp6E9.name;
_tmp6EB=_tmp6E9.tq;_tmp6EC=(void*)_tmp6E9.type;_tmp6ED=_tmp6E9.inject;{void*
_tmp6EE=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6EC);if(_tmp6EE == _tmp6EC)
cyc_varargs2=_tmp6A8;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp6EF=
_cycalloc(sizeof(*_tmp6EF));_tmp6EF->name=_tmp6EA;_tmp6EF->tq=_tmp6EB;_tmp6EF->type=(
void*)_tmp6EE;_tmp6EF->inject=_tmp6ED;_tmp6EF;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp6F1;struct Cyc_List_List*_tmp6F2;struct _tuple0 _tmp6F0=Cyc_List_rsplit(
rgn,rgn,_tmp6A9);_tmp6F1=_tmp6F0.f1;_tmp6F2=_tmp6F0.f2;{struct Cyc_List_List*
_tmp6F3=Cyc_Tcutil_substs(rgn,inst,_tmp6F1);struct Cyc_List_List*_tmp6F4=Cyc_Tcutil_substs(
rgn,inst,_tmp6F2);if(_tmp6F3 == _tmp6F1  && _tmp6F4 == _tmp6F2)rgn_po2=_tmp6A9;
else{rgn_po2=Cyc_List_zip(_tmp6F3,_tmp6F4);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp6F5=_cycalloc(sizeof(*_tmp6F5));_tmp6F5[0]=({struct Cyc_Absyn_FnType_struct
_tmp6F6;_tmp6F6.tag=8;_tmp6F6.f1=({struct Cyc_Absyn_FnInfo _tmp6F7;_tmp6F7.tvars=
_tmp6A3;_tmp6F7.effect=eff2;_tmp6F7.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp6A5);_tmp6F7.args=_tmp6E6;_tmp6F7.c_varargs=_tmp6A7;_tmp6F7.cyc_varargs=
cyc_varargs2;_tmp6F7.rgn_po=rgn_po2;_tmp6F7.attributes=_tmp6AA;_tmp6F7;});
_tmp6F6;});_tmp6F5;});}}}}}_LL479: if(*((int*)_tmp684)!= 9)goto _LL47B;_tmp6AB=((
struct Cyc_Absyn_TupleType_struct*)_tmp684)->f1;_LL47A: {struct Cyc_List_List*
_tmp6F8=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple4*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmp6AB);struct Cyc_List_List*
_tmp6F9=Cyc_Tcutil_substs(rgn,inst,_tmp6F8);if(_tmp6F9 == _tmp6F8)return t;{struct
Cyc_List_List*_tmp6FA=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct _tuple4*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmp6AB,_tmp6F9);return(void*)({struct Cyc_Absyn_TupleType_struct*_tmp6FB=
_cycalloc(sizeof(*_tmp6FB));_tmp6FB[0]=({struct Cyc_Absyn_TupleType_struct _tmp6FC;
_tmp6FC.tag=9;_tmp6FC.f1=_tmp6FA;_tmp6FC;});_tmp6FB;});}}_LL47B: if(*((int*)
_tmp684)!= 11)goto _LL47D;_tmp6AC=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp684)->f1;_tmp6AD=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp684)->f2;_LL47C: {
struct Cyc_List_List*_tmp6FD=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp6AD);struct Cyc_List_List*_tmp6FE=Cyc_Tcutil_substs(rgn,inst,_tmp6FD);if(
_tmp6FE == _tmp6FD)return t;{struct Cyc_List_List*_tmp6FF=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp6AD,
_tmp6FE);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp700=_cycalloc(
sizeof(*_tmp700));_tmp700[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp701;
_tmp701.tag=11;_tmp701.f1=(void*)_tmp6AC;_tmp701.f2=_tmp6FF;_tmp701;});_tmp700;});}}
_LL47D: if(*((int*)_tmp684)!= 0)goto _LL47F;_tmp6AE=((struct Cyc_Absyn_Evar_struct*)
_tmp684)->f2;_LL47E: if(_tmp6AE != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)
_tmp6AE->v);else{return t;}_LL47F: if(*((int*)_tmp684)!= 15)goto _LL481;_tmp6AF=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp684)->f1;_LL480: {void*_tmp702=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6AF);return _tmp702 == _tmp6AF?t:(void*)({
struct Cyc_Absyn_RgnHandleType_struct*_tmp703=_cycalloc(sizeof(*_tmp703));_tmp703[
0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp704;_tmp704.tag=15;_tmp704.f1=(void*)
_tmp702;_tmp704;});_tmp703;});}_LL481: if(*((int*)_tmp684)!= 14)goto _LL483;
_tmp6B0=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp684)->f1;_LL482: {void*
_tmp705=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B0);return _tmp705 == _tmp6B0?t:(void*)({
struct Cyc_Absyn_SizeofType_struct*_tmp706=_cycalloc(sizeof(*_tmp706));_tmp706[0]=({
struct Cyc_Absyn_SizeofType_struct _tmp707;_tmp707.tag=14;_tmp707.f1=(void*)
_tmp705;_tmp707;});_tmp706;});}_LL483: if(*((int*)_tmp684)!= 17)goto _LL485;
_tmp6B1=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp684)->f1;_LL484: {void*
_tmp708=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B1);return _tmp708 == _tmp6B1?t:(void*)({
struct Cyc_Absyn_TagType_struct*_tmp709=_cycalloc(sizeof(*_tmp709));_tmp709[0]=({
struct Cyc_Absyn_TagType_struct _tmp70A;_tmp70A.tag=17;_tmp70A.f1=(void*)_tmp708;
_tmp70A;});_tmp709;});}_LL485: if(*((int*)_tmp684)!= 18)goto _LL487;_LL486: goto
_LL488;_LL487: if(*((int*)_tmp684)!= 12)goto _LL489;_LL488: goto _LL48A;_LL489: if(*((
int*)_tmp684)!= 13)goto _LL48B;_LL48A: goto _LL48C;_LL48B: if((int)_tmp684 != 0)goto
_LL48D;_LL48C: goto _LL48E;_LL48D: if(_tmp684 <= (void*)4)goto _LL48F;if(*((int*)
_tmp684)!= 5)goto _LL48F;_LL48E: goto _LL490;_LL48F: if((int)_tmp684 != 1)goto _LL491;
_LL490: goto _LL492;_LL491: if(_tmp684 <= (void*)4)goto _LL493;if(*((int*)_tmp684)!= 
6)goto _LL493;_LL492: goto _LL494;_LL493: if((int)_tmp684 != 3)goto _LL495;_LL494: goto
_LL496;_LL495: if((int)_tmp684 != 2)goto _LL497;_LL496: return t;_LL497: if(_tmp684 <= (
void*)4)goto _LL499;if(*((int*)_tmp684)!= 21)goto _LL499;_tmp6B2=(void*)((struct
Cyc_Absyn_RgnsEff_struct*)_tmp684)->f1;_LL498: {void*_tmp70B=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6B2);return _tmp70B == _tmp6B2?t:(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp70C=_cycalloc(sizeof(*_tmp70C));_tmp70C[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp70D;_tmp70D.tag=21;_tmp70D.f1=(void*)_tmp70B;_tmp70D;});_tmp70C;});}_LL499:
if(_tmp684 <= (void*)4)goto _LL49B;if(*((int*)_tmp684)!= 19)goto _LL49B;_tmp6B3=(
void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp684)->f1;_LL49A: {void*_tmp70E=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6B3);return _tmp70E == _tmp6B3?t:(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp70F=_cycalloc(sizeof(*_tmp70F));_tmp70F[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp710;_tmp710.tag=19;_tmp710.f1=(void*)_tmp70E;_tmp710;});_tmp70F;});}_LL49B:
if(_tmp684 <= (void*)4)goto _LL468;if(*((int*)_tmp684)!= 20)goto _LL468;_tmp6B4=((
struct Cyc_Absyn_JoinEff_struct*)_tmp684)->f1;_LL49C: {struct Cyc_List_List*
_tmp711=Cyc_Tcutil_substs(rgn,inst,_tmp6B4);return _tmp711 == _tmp6B4?t:(void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp712=_cycalloc(sizeof(*_tmp712));_tmp712[0]=({
struct Cyc_Absyn_JoinEff_struct _tmp713;_tmp713.tag=20;_tmp713.f1=_tmp711;_tmp713;});
_tmp712;});}_LL468:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 0)
return 0;{void*_tmp714=(void*)ts->hd;struct Cyc_List_List*_tmp715=ts->tl;void*
_tmp716=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp714);struct Cyc_List_List*_tmp717=Cyc_Tcutil_substs(
rgn,inst,_tmp715);if(_tmp714 == _tmp716  && _tmp715 == _tmp717)return ts;return(
struct Cyc_List_List*)((struct Cyc_List_List*)({struct Cyc_List_List*_tmp718=
_cycalloc(sizeof(*_tmp718));_tmp718->hd=(void*)_tmp716;_tmp718->tl=_tmp717;
_tmp718;}));}}extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){void*k=Cyc_Tcutil_tvar_kind(tv);
return({struct _tuple8*_tmp719=_cycalloc(sizeof(*_tmp719));_tmp719->f1=tv;_tmp719->f2=
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp71A=_cycalloc(sizeof(*_tmp71A));
_tmp71A->v=(void*)k;_tmp71A;}),({struct Cyc_Core_Opt*_tmp71B=_cycalloc(sizeof(*
_tmp71B));_tmp71B->v=s;_tmp71B;}));_tmp719;});}struct _tuple8*Cyc_Tcutil_r_make_inst_var(
struct _tuple9*env,struct Cyc_Absyn_Tvar*tv){struct _tuple9 _tmp71D;struct Cyc_List_List*
_tmp71E;struct _RegionHandle*_tmp71F;struct _tuple9*_tmp71C=env;_tmp71D=*_tmp71C;
_tmp71E=_tmp71D.f1;_tmp71F=_tmp71D.f2;{void*k=Cyc_Tcutil_tvar_kind(tv);return({
struct _tuple8*_tmp720=_region_malloc(_tmp71F,sizeof(*_tmp720));_tmp720->f1=tv;
_tmp720->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp721=_cycalloc(sizeof(*
_tmp721));_tmp721->v=(void*)k;_tmp721;}),({struct Cyc_Core_Opt*_tmp722=_cycalloc(
sizeof(*_tmp722));_tmp722->v=_tmp71E;_tmp722;}));_tmp720;});}}static struct Cyc_List_List*
Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(
void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(
k1,k2))({struct Cyc_String_pa_struct _tmp727;_tmp727.tag=0;_tmp727.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kindbound2string(k2));{
struct Cyc_String_pa_struct _tmp726;_tmp726.tag=0;_tmp726.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kindbound2string(k1));{
struct Cyc_String_pa_struct _tmp725;_tmp725.tag=0;_tmp725.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*tv->name);{void*_tmp723[3]={& _tmp725,&
_tmp726,& _tmp727};Cyc_Tcutil_terr(loc,({const char*_tmp724="type variable %s is used with inconsistent kinds %s and %s";
_tag_dynforward(_tmp724,sizeof(char),_get_zero_arr_size(_tmp724,59));}),
_tag_dynforward(_tmp723,sizeof(void*),3));}}}});if(tv->identity == 0)tv->identity=((
struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{if(*((int*)_check_null(tv->identity))
!= *((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)))({void*
_tmp728[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp729="same type variable has different identity!";_tag_dynforward(
_tmp729,sizeof(char),_get_zero_arr_size(_tmp729,43));}),_tag_dynforward(_tmp728,
sizeof(void*),0));});}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();return({
struct Cyc_List_List*_tmp72A=_cycalloc(sizeof(*_tmp72A));_tmp72A->hd=tv;_tmp72A->tl=
tvs;_tmp72A;});}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({void*_tmp72B[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp72C="fast_add_free_tvar: bad identity in tv";_tag_dynforward(
_tmp72C,sizeof(char),_get_zero_arr_size(_tmp72C,39));}),_tag_dynforward(_tmp72B,
sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity == 0)({void*_tmp72D[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp72E="fast_add_free_tvar: bad identity in tvs2";_tag_dynforward(_tmp72E,
sizeof(char),_get_zero_arr_size(_tmp72E,41));}),_tag_dynforward(_tmp72D,sizeof(
void*),0));});if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity))
== *((int*)_check_null(tv->identity)))return tvs;}}return({struct Cyc_List_List*
_tmp72F=_cycalloc(sizeof(*_tmp72F));_tmp72F->hd=tv;_tmp72F->tl=tvs;_tmp72F;});}
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({struct Cyc_String_pa_struct _tmp732;
_tmp732.tag=0;_tmp732.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcutil_tvar2string(
tv));{void*_tmp730[1]={& _tmp732};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp731="bound tvar id for %s is NULL";
_tag_dynforward(_tmp731,sizeof(char),_get_zero_arr_size(_tmp731,29));}),
_tag_dynforward(_tmp730,sizeof(void*),1));}});return({struct Cyc_List_List*
_tmp733=_cycalloc(sizeof(*_tmp733));_tmp733->hd=tv;_tmp733->tl=tvs;_tmp733;});}
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct Cyc_List_List*es,void*e){
void*_tmp734=Cyc_Tcutil_compress(e);int _tmp735;_LL4A8: if(_tmp734 <= (void*)4)goto
_LL4AA;if(*((int*)_tmp734)!= 0)goto _LL4AA;_tmp735=((struct Cyc_Absyn_Evar_struct*)
_tmp734)->f3;_LL4A9:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){void*
_tmp736=Cyc_Tcutil_compress((void*)es2->hd);int _tmp737;_LL4AD: if(_tmp736 <= (void*)
4)goto _LL4AF;if(*((int*)_tmp736)!= 0)goto _LL4AF;_tmp737=((struct Cyc_Absyn_Evar_struct*)
_tmp736)->f3;_LL4AE: if(_tmp735 == _tmp737)return es;goto _LL4AC;_LL4AF:;_LL4B0: goto
_LL4AC;_LL4AC:;}}return({struct Cyc_List_List*_tmp738=_cycalloc(sizeof(*_tmp738));
_tmp738->hd=(void*)e;_tmp738->tl=es;_tmp738;});_LL4AA:;_LL4AB: return es;_LL4A7:;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs){struct Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int
present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(*((int*)
_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))== *((int*)_check_null(((
struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;break;}}}if(!present)r=({
struct Cyc_List_List*_tmp739=_cycalloc(sizeof(*_tmp739));_tmp739->hd=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp739->tl=r;_tmp739;});}r=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dynforward_ptr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)width))({struct Cyc_String_pa_struct _tmp73C;_tmp73C.tag=0;
_tmp73C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*fn);{void*_tmp73A[1]={&
_tmp73C};Cyc_Tcutil_terr(loc,({const char*_tmp73B="bitfield %s does not have constant width";
_tag_dynforward(_tmp73B,sizeof(char),_get_zero_arr_size(_tmp73B,41));}),
_tag_dynforward(_tmp73A,sizeof(void*),1));}});else{unsigned int _tmp73E;int
_tmp73F;struct _tuple7 _tmp73D=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
width);_tmp73E=_tmp73D.f1;_tmp73F=_tmp73D.f2;if(!_tmp73F)({void*_tmp740[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp741="bitfield width cannot use sizeof or offsetof";
_tag_dynforward(_tmp741,sizeof(char),_get_zero_arr_size(_tmp741,45));}),
_tag_dynforward(_tmp740,sizeof(void*),0));});w=_tmp73E;}{void*_tmp742=Cyc_Tcutil_compress(
field_typ);void*_tmp743;_LL4B2: if(_tmp742 <= (void*)4)goto _LL4B4;if(*((int*)
_tmp742)!= 5)goto _LL4B4;_tmp743=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp742)->f2;
_LL4B3:{void*_tmp744=_tmp743;_LL4B7: if((int)_tmp744 != 0)goto _LL4B9;_LL4B8: if(w > 
8)({void*_tmp745[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp746="bitfield larger than type";
_tag_dynforward(_tmp746,sizeof(char),_get_zero_arr_size(_tmp746,26));}),
_tag_dynforward(_tmp745,sizeof(void*),0));});goto _LL4B6;_LL4B9: if((int)_tmp744 != 
1)goto _LL4BB;_LL4BA: if(w > 16)({void*_tmp747[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp748="bitfield larger than type";_tag_dynforward(_tmp748,sizeof(char),
_get_zero_arr_size(_tmp748,26));}),_tag_dynforward(_tmp747,sizeof(void*),0));});
goto _LL4B6;_LL4BB: if((int)_tmp744 != 3)goto _LL4BD;_LL4BC: goto _LL4BE;_LL4BD: if((
int)_tmp744 != 2)goto _LL4BF;_LL4BE: if(w > 32)({void*_tmp749[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp74A="bitfield larger than type";_tag_dynforward(_tmp74A,
sizeof(char),_get_zero_arr_size(_tmp74A,26));}),_tag_dynforward(_tmp749,sizeof(
void*),0));});goto _LL4B6;_LL4BF: if((int)_tmp744 != 4)goto _LL4B6;_LL4C0: if(w > 64)({
void*_tmp74B[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp74C="bitfield larger than type";
_tag_dynforward(_tmp74C,sizeof(char),_get_zero_arr_size(_tmp74C,26));}),
_tag_dynforward(_tmp74B,sizeof(void*),0));});goto _LL4B6;_LL4B6:;}goto _LL4B1;
_LL4B4:;_LL4B5:({struct Cyc_String_pa_struct _tmp750;_tmp750.tag=0;_tmp750.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(field_typ));{
struct Cyc_String_pa_struct _tmp74F;_tmp74F.tag=0;_tmp74F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*fn);{void*_tmp74D[2]={& _tmp74F,& _tmp750};
Cyc_Tcutil_terr(loc,({const char*_tmp74E="bitfield %s must have integral type but has type %s";
_tag_dynforward(_tmp74E,sizeof(char),_get_zero_arr_size(_tmp74E,52));}),
_tag_dynforward(_tmp74D,sizeof(void*),2));}}});goto _LL4B1;_LL4B1:;}}}static void
Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*loc,struct _dynforward_ptr*
fn,struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*_tmp751=(void*)
atts->hd;_LL4C2: if((int)_tmp751 != 5)goto _LL4C4;_LL4C3: continue;_LL4C4: if(_tmp751
<= (void*)17)goto _LL4C6;if(*((int*)_tmp751)!= 1)goto _LL4C6;_LL4C5: continue;
_LL4C6:;_LL4C7:({struct Cyc_String_pa_struct _tmp755;_tmp755.tag=0;_tmp755.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*fn);{struct Cyc_String_pa_struct
_tmp754;_tmp754.tag=0;_tmp754.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));{void*_tmp752[2]={& _tmp754,& _tmp755};
Cyc_Tcutil_terr(loc,({const char*_tmp753="bad attribute %s on member %s";
_tag_dynforward(_tmp753,sizeof(char),_get_zero_arr_size(_tmp753,30));}),
_tag_dynforward(_tmp752,sizeof(void*),2));}}});_LL4C1:;}}struct Cyc_Tcutil_CVTEnv{
struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*
free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp756=t;struct
Cyc_Absyn_Typedefdecl*_tmp757;void**_tmp758;_LL4C9: if(_tmp756 <= (void*)4)goto
_LL4CB;if(*((int*)_tmp756)!= 16)goto _LL4CB;_tmp757=((struct Cyc_Absyn_TypedefType_struct*)
_tmp756)->f3;_tmp758=((struct Cyc_Absyn_TypedefType_struct*)_tmp756)->f4;_LL4CA:
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp757))->tq).real_const  || (
_tmp757->tq).print_const){if(declared_const)({void*_tmp759[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp75A="extra const";_tag_dynforward(_tmp75A,sizeof(char),
_get_zero_arr_size(_tmp75A,12));}),_tag_dynforward(_tmp759,sizeof(void*),0));});
return 1;}if((unsigned int)_tmp758)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmp758);else{return declared_const;}_LL4CB:;_LL4CC: return
declared_const;_LL4C8:;}struct _tuple17{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,void*t,int allow_unique){{
void*_tmp75B=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp75C;struct Cyc_Core_Opt**
_tmp75D;struct Cyc_Core_Opt*_tmp75E;struct Cyc_Core_Opt**_tmp75F;struct Cyc_Absyn_Tvar*
_tmp760;struct Cyc_List_List*_tmp761;struct _tuple1*_tmp762;struct Cyc_Absyn_Enumdecl*
_tmp763;struct Cyc_Absyn_Enumdecl**_tmp764;struct Cyc_Absyn_TunionInfo _tmp765;void*
_tmp766;void**_tmp767;struct Cyc_List_List*_tmp768;struct Cyc_List_List**_tmp769;
struct Cyc_Core_Opt*_tmp76A;struct Cyc_Core_Opt**_tmp76B;struct Cyc_Absyn_TunionFieldInfo
_tmp76C;void*_tmp76D;void**_tmp76E;struct Cyc_List_List*_tmp76F;struct Cyc_Absyn_PtrInfo
_tmp770;void*_tmp771;struct Cyc_Absyn_Tqual _tmp772;struct Cyc_Absyn_Tqual*_tmp773;
struct Cyc_Absyn_PtrAtts _tmp774;void*_tmp775;struct Cyc_Absyn_Conref*_tmp776;
struct Cyc_Absyn_Conref*_tmp777;struct Cyc_Absyn_Conref*_tmp778;void*_tmp779;void*
_tmp77A;struct Cyc_Absyn_ArrayInfo _tmp77B;void*_tmp77C;struct Cyc_Absyn_Tqual
_tmp77D;struct Cyc_Absyn_Tqual*_tmp77E;struct Cyc_Absyn_Exp*_tmp77F;struct Cyc_Absyn_Exp**
_tmp780;struct Cyc_Absyn_Conref*_tmp781;struct Cyc_Position_Segment*_tmp782;struct
Cyc_Absyn_FnInfo _tmp783;struct Cyc_List_List*_tmp784;struct Cyc_List_List**_tmp785;
struct Cyc_Core_Opt*_tmp786;struct Cyc_Core_Opt**_tmp787;void*_tmp788;struct Cyc_List_List*
_tmp789;int _tmp78A;struct Cyc_Absyn_VarargInfo*_tmp78B;struct Cyc_List_List*
_tmp78C;struct Cyc_List_List*_tmp78D;struct Cyc_List_List*_tmp78E;void*_tmp78F;
struct Cyc_List_List*_tmp790;struct Cyc_Absyn_AggrInfo _tmp791;void*_tmp792;void**
_tmp793;struct Cyc_List_List*_tmp794;struct Cyc_List_List**_tmp795;struct _tuple1*
_tmp796;struct Cyc_List_List*_tmp797;struct Cyc_List_List**_tmp798;struct Cyc_Absyn_Typedefdecl*
_tmp799;struct Cyc_Absyn_Typedefdecl**_tmp79A;void**_tmp79B;void***_tmp79C;void*
_tmp79D;void*_tmp79E;void*_tmp79F;struct Cyc_List_List*_tmp7A0;_LL4CE: if((int)
_tmp75B != 0)goto _LL4D0;_LL4CF: goto _LL4CD;_LL4D0: if(_tmp75B <= (void*)4)goto _LL4E4;
if(*((int*)_tmp75B)!= 0)goto _LL4D2;_tmp75C=((struct Cyc_Absyn_Evar_struct*)
_tmp75B)->f1;_tmp75D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp75B)->f1;_tmp75E=((struct Cyc_Absyn_Evar_struct*)_tmp75B)->f2;_tmp75F=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp75B)->f2;_LL4D1: if(*_tmp75D == 
0)*_tmp75D=({struct Cyc_Core_Opt*_tmp7A1=_cycalloc(sizeof(*_tmp7A1));_tmp7A1->v=(
void*)expected_kind;_tmp7A1;});if((cvtenv.fn_result  && cvtenv.generalize_evars)
 && expected_kind == (void*)3)*_tmp75F=({struct Cyc_Core_Opt*_tmp7A2=_cycalloc(
sizeof(*_tmp7A2));_tmp7A2->v=(void*)((void*)2);_tmp7A2;});else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp7A3=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp7A7=_cycalloc(sizeof(*_tmp7A7));_tmp7A7[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp7A8;_tmp7A8.tag=2;_tmp7A8.f1=0;_tmp7A8.f2=(void*)expected_kind;_tmp7A8;});
_tmp7A7;}));*_tmp75F=({struct Cyc_Core_Opt*_tmp7A4=_cycalloc(sizeof(*_tmp7A4));
_tmp7A4->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp7A5=_cycalloc(
sizeof(*_tmp7A5));_tmp7A5[0]=({struct Cyc_Absyn_VarType_struct _tmp7A6;_tmp7A6.tag=
1;_tmp7A6.f1=_tmp7A3;_tmp7A6;});_tmp7A5;}));_tmp7A4;});_tmp760=_tmp7A3;goto
_LL4D3;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,t);}}
goto _LL4CD;_LL4D2: if(*((int*)_tmp75B)!= 1)goto _LL4D4;_tmp760=((struct Cyc_Absyn_VarType_struct*)
_tmp75B)->f1;_LL4D3:{void*_tmp7A9=Cyc_Absyn_compress_kb((void*)_tmp760->kind);
struct Cyc_Core_Opt*_tmp7AA;struct Cyc_Core_Opt**_tmp7AB;_LL503: if(*((int*)_tmp7A9)
!= 1)goto _LL505;_tmp7AA=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp7A9)->f1;
_tmp7AB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp7A9)->f1;
_LL504:*_tmp7AB=({struct Cyc_Core_Opt*_tmp7AC=_cycalloc(sizeof(*_tmp7AC));_tmp7AC->v=(
void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp7AD=_cycalloc(sizeof(*_tmp7AD));
_tmp7AD[0]=({struct Cyc_Absyn_Less_kb_struct _tmp7AE;_tmp7AE.tag=2;_tmp7AE.f1=0;
_tmp7AE.f2=(void*)expected_kind;_tmp7AE;});_tmp7AD;}));_tmp7AC;});goto _LL502;
_LL505:;_LL506: goto _LL502;_LL502:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmp760);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,
_tmp760);goto _LL4CD;_LL4D4: if(*((int*)_tmp75B)!= 13)goto _LL4D6;_tmp761=((struct
Cyc_Absyn_AnonEnumType_struct*)_tmp75B)->f1;_LL4D5: {struct Cyc_Tcenv_Genv*ge=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);{struct _RegionHandle _tmp7AF=_new_region("uprev_rgn");struct
_RegionHandle*uprev_rgn=& _tmp7AF;_push_region(uprev_rgn);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmp761 != 0;_tmp761=_tmp761->tl){
struct Cyc_Absyn_Enumfield*_tmp7B0=(struct Cyc_Absyn_Enumfield*)_tmp761->hd;if(((
int(*)(int(*compare)(struct _dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*
l,struct _dynforward_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp7B0->name).f2))({
struct Cyc_String_pa_struct _tmp7B3;_tmp7B3.tag=0;_tmp7B3.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*(*_tmp7B0->name).f2);{void*_tmp7B1[1]={&
_tmp7B3};Cyc_Tcutil_terr(_tmp7B0->loc,({const char*_tmp7B2="duplicate enum field name %s";
_tag_dynforward(_tmp7B2,sizeof(char),_get_zero_arr_size(_tmp7B2,29));}),
_tag_dynforward(_tmp7B1,sizeof(void*),1));}});else{prev_fields=({struct Cyc_List_List*
_tmp7B4=_region_malloc(uprev_rgn,sizeof(*_tmp7B4));_tmp7B4->hd=(*_tmp7B0->name).f2;
_tmp7B4->tl=prev_fields;_tmp7B4;});}if(_tmp7B0->tag == 0)_tmp7B0->tag=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(tag_count,_tmp7B0->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)_check_null(_tmp7B0->tag)))({struct Cyc_String_pa_struct
_tmp7B7;_tmp7B7.tag=0;_tmp7B7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*(*
_tmp7B0->name).f2);{void*_tmp7B5[1]={& _tmp7B7};Cyc_Tcutil_terr(loc,({const char*
_tmp7B6="enum field %s: expression is not constant";_tag_dynforward(_tmp7B6,
sizeof(char),_get_zero_arr_size(_tmp7B6,42));}),_tag_dynforward(_tmp7B5,sizeof(
void*),1));}});}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp7B0->tag))).f1;tag_count=t1 + 1;(*_tmp7B0->name).f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp7B8;(_tmp7B8.Abs_n).tag=2;(_tmp7B8.Abs_n).f1=te->ns;
_tmp7B8;});ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k,struct _tuple17*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp7B0->name).f2,(
struct _tuple17*)({struct _tuple17*_tmp7B9=_cycalloc(sizeof(*_tmp7B9));_tmp7B9->f1=(
void*)({struct Cyc_Tcenv_AnonEnumRes_struct*_tmp7BA=_cycalloc(sizeof(*_tmp7BA));
_tmp7BA[0]=({struct Cyc_Tcenv_AnonEnumRes_struct _tmp7BB;_tmp7BB.tag=4;_tmp7BB.f1=(
void*)t;_tmp7BB.f2=_tmp7B0;_tmp7BB;});_tmp7BA;});_tmp7B9->f2=1;_tmp7B9;}));}}};
_pop_region(uprev_rgn);}goto _LL4CD;}_LL4D6: if(*((int*)_tmp75B)!= 12)goto _LL4D8;
_tmp762=((struct Cyc_Absyn_EnumType_struct*)_tmp75B)->f1;_tmp763=((struct Cyc_Absyn_EnumType_struct*)
_tmp75B)->f2;_tmp764=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_struct*)
_tmp75B)->f2;_LL4D7: if(*_tmp764 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*
_tmp764))->fields == 0){struct _handler_cons _tmp7BC;_push_handler(& _tmp7BC);{int
_tmp7BE=0;if(setjmp(_tmp7BC.handler))_tmp7BE=1;if(!_tmp7BE){{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp762);*_tmp764=(struct Cyc_Absyn_Enumdecl*)*
ed;};_pop_handler();}else{void*_tmp7BD=(void*)_exn_thrown;void*_tmp7C0=_tmp7BD;
_LL508: if(_tmp7C0 != Cyc_Dict_Absent)goto _LL50A;_LL509: {struct Cyc_Tcenv_Genv*
_tmp7C1=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Enumdecl*_tmp7C2=({struct Cyc_Absyn_Enumdecl*
_tmp7C3=_cycalloc(sizeof(*_tmp7C3));_tmp7C3->sc=(void*)((void*)3);_tmp7C3->name=
_tmp762;_tmp7C3->fields=0;_tmp7C3;});Cyc_Tc_tcEnumdecl(te,_tmp7C1,loc,_tmp7C2);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp762);*_tmp764=(
struct Cyc_Absyn_Enumdecl*)*ed;goto _LL507;}}_LL50A:;_LL50B:(void)_throw(_tmp7C0);
_LL507:;}}}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*
_tmp764);*_tmp762=(ed->name)[_check_known_subscript_notnull(1,0)];goto _LL4CD;}
_LL4D8: if(*((int*)_tmp75B)!= 2)goto _LL4DA;_tmp765=((struct Cyc_Absyn_TunionType_struct*)
_tmp75B)->f1;_tmp766=(void*)_tmp765.tunion_info;_tmp767=(void**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp75B)->f1).tunion_info;_tmp768=_tmp765.targs;_tmp769=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp75B)->f1).targs;_tmp76A=_tmp765.rgn;
_tmp76B=(struct Cyc_Core_Opt**)&(((struct Cyc_Absyn_TunionType_struct*)_tmp75B)->f1).rgn;
_LL4D9: {struct Cyc_List_List*_tmp7C4=*_tmp769;{void*_tmp7C5=*_tmp767;struct Cyc_Absyn_UnknownTunionInfo
_tmp7C6;struct _tuple1*_tmp7C7;int _tmp7C8;int _tmp7C9;struct Cyc_Absyn_Tuniondecl**
_tmp7CA;struct Cyc_Absyn_Tuniondecl*_tmp7CB;_LL50D: if(*((int*)_tmp7C5)!= 0)goto
_LL50F;_tmp7C6=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp7C5)->f1;_tmp7C7=
_tmp7C6.name;_tmp7C8=_tmp7C6.is_xtunion;_tmp7C9=_tmp7C6.is_flat;_LL50E: {struct
Cyc_Absyn_Tuniondecl**tudp;{struct _handler_cons _tmp7CC;_push_handler(& _tmp7CC);{
int _tmp7CE=0;if(setjmp(_tmp7CC.handler))_tmp7CE=1;if(!_tmp7CE){tudp=Cyc_Tcenv_lookup_tuniondecl(
te,loc,_tmp7C7);;_pop_handler();}else{void*_tmp7CD=(void*)_exn_thrown;void*
_tmp7D0=_tmp7CD;_LL512: if(_tmp7D0 != Cyc_Dict_Absent)goto _LL514;_LL513: {struct
Cyc_Tcenv_Genv*_tmp7D1=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*
_tmp7D2=({struct Cyc_Absyn_Tuniondecl*_tmp7D9=_cycalloc(sizeof(*_tmp7D9));_tmp7D9->sc=(
void*)((void*)3);_tmp7D9->name=_tmp7C7;_tmp7D9->tvs=0;_tmp7D9->fields=0;_tmp7D9->is_xtunion=
_tmp7C8;_tmp7D9->is_flat=_tmp7C9;_tmp7D9;});Cyc_Tc_tcTuniondecl(te,_tmp7D1,loc,
_tmp7D2);tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp7C7);if(_tmp7C4 != 0){({
struct Cyc_String_pa_struct _tmp7D6;_tmp7D6.tag=0;_tmp7D6.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7C7));{
struct Cyc_String_pa_struct _tmp7D5;_tmp7D5.tag=0;_tmp7D5.f1=(struct
_dynforward_ptr)(_tmp7C8?(struct _dynforward_ptr)((struct _dynforward_ptr)({const
char*_tmp7D7="xtunion";_tag_dynforward(_tmp7D7,sizeof(char),_get_zero_arr_size(
_tmp7D7,8));})):(struct _dynforward_ptr)({const char*_tmp7D8="tunion";
_tag_dynforward(_tmp7D8,sizeof(char),_get_zero_arr_size(_tmp7D8,7));}));{void*
_tmp7D3[2]={& _tmp7D5,& _tmp7D6};Cyc_Tcutil_terr(loc,({const char*_tmp7D4="declare parameterized %s %s before using";
_tag_dynforward(_tmp7D4,sizeof(char),_get_zero_arr_size(_tmp7D4,41));}),
_tag_dynforward(_tmp7D3,sizeof(void*),2));}}});return cvtenv;}goto _LL511;}_LL514:;
_LL515:(void)_throw(_tmp7D0);_LL511:;}}}if((*tudp)->is_xtunion != _tmp7C8)({
struct Cyc_String_pa_struct _tmp7DC;_tmp7DC.tag=0;_tmp7DC.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7C7));{void*
_tmp7DA[1]={& _tmp7DC};Cyc_Tcutil_terr(loc,({const char*_tmp7DB="[x]tunion is different from type declaration %s";
_tag_dynforward(_tmp7DB,sizeof(char),_get_zero_arr_size(_tmp7DB,48));}),
_tag_dynforward(_tmp7DA,sizeof(void*),1));}});*_tmp767=(void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp7DD=_cycalloc(sizeof(*_tmp7DD));_tmp7DD[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp7DE;_tmp7DE.tag=1;_tmp7DE.f1=tudp;_tmp7DE;});_tmp7DD;});_tmp7CB=*tudp;goto
_LL510;}_LL50F: if(*((int*)_tmp7C5)!= 1)goto _LL50C;_tmp7CA=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp7C5)->f1;_tmp7CB=*_tmp7CA;_LL510: if(_tmp7CB->is_flat  && (unsigned int)*
_tmp76B)({struct Cyc_String_pa_struct _tmp7E1;_tmp7E1.tag=0;_tmp7E1.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(*_tmp76B))->v));{void*_tmp7DF[1]={& _tmp7E1};Cyc_Tcutil_terr(
loc,({const char*_tmp7E0="flat tunion has region %s";_tag_dynforward(_tmp7E0,
sizeof(char),_get_zero_arr_size(_tmp7E0,26));}),_tag_dynforward(_tmp7DF,sizeof(
void*),1));}});if(!_tmp7CB->is_flat  && !((unsigned int)*_tmp76B))*_tmp76B=({
struct Cyc_Core_Opt*_tmp7E2=_cycalloc(sizeof(*_tmp7E2));_tmp7E2->v=(void*)Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp7E3=_cycalloc(sizeof(*_tmp7E3));_tmp7E3->v=(void*)((void*)
3);_tmp7E3;}),0);_tmp7E2;});if((unsigned int)*_tmp76B){void*_tmp7E4=(void*)((
struct Cyc_Core_Opt*)_check_null(*_tmp76B))->v;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp7E4,allow_unique);}{struct Cyc_List_List*tvs=_tmp7CB->tvs;
for(0;_tmp7C4 != 0  && tvs != 0;(_tmp7C4=_tmp7C4->tl,tvs=tvs->tl)){void*t1=(void*)
_tmp7C4->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1,0);}if(_tmp7C4 != 0)({struct Cyc_String_pa_struct
_tmp7E7;_tmp7E7.tag=0;_tmp7E7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp7CB->name));{void*_tmp7E5[1]={& _tmp7E7};Cyc_Tcutil_terr(
loc,({const char*_tmp7E6="too many type arguments for tunion %s";_tag_dynforward(
_tmp7E6,sizeof(char),_get_zero_arr_size(_tmp7E6,38));}),_tag_dynforward(_tmp7E5,
sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;
tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=
Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp7E8=_cycalloc(
sizeof(*_tmp7E8));_tmp7E8->hd=(void*)e;_tmp7E8->tl=hidden_ts;_tmp7E8;});cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,0);}*_tmp769=Cyc_List_imp_append(*
_tmp769,Cyc_List_imp_rev(hidden_ts));}goto _LL50C;}_LL50C:;}goto _LL4CD;}_LL4DA:
if(*((int*)_tmp75B)!= 3)goto _LL4DC;_tmp76C=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp75B)->f1;_tmp76D=(void*)_tmp76C.field_info;_tmp76E=(void**)&(((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp75B)->f1).field_info;_tmp76F=_tmp76C.targs;_LL4DB:{void*_tmp7E9=*_tmp76E;
struct Cyc_Absyn_UnknownTunionFieldInfo _tmp7EA;struct _tuple1*_tmp7EB;struct
_tuple1*_tmp7EC;int _tmp7ED;struct Cyc_Absyn_Tuniondecl*_tmp7EE;struct Cyc_Absyn_Tunionfield*
_tmp7EF;_LL517: if(*((int*)_tmp7E9)!= 0)goto _LL519;_tmp7EA=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp7E9)->f1;_tmp7EB=_tmp7EA.tunion_name;_tmp7EC=_tmp7EA.field_name;_tmp7ED=
_tmp7EA.is_xtunion;_LL518: {struct Cyc_Absyn_Tuniondecl*tud;struct Cyc_Absyn_Tunionfield*
tuf;{struct _handler_cons _tmp7F0;_push_handler(& _tmp7F0);{int _tmp7F2=0;if(setjmp(
_tmp7F0.handler))_tmp7F2=1;if(!_tmp7F2){*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp7EB);;_pop_handler();}else{void*_tmp7F1=(void*)_exn_thrown;void*_tmp7F4=
_tmp7F1;_LL51C: if(_tmp7F4 != Cyc_Dict_Absent)goto _LL51E;_LL51D:({struct Cyc_String_pa_struct
_tmp7F7;_tmp7F7.tag=0;_tmp7F7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp7EB));{void*_tmp7F5[1]={& _tmp7F7};Cyc_Tcutil_terr(
loc,({const char*_tmp7F6="unbound type tunion %s";_tag_dynforward(_tmp7F6,sizeof(
char),_get_zero_arr_size(_tmp7F6,23));}),_tag_dynforward(_tmp7F5,sizeof(void*),1));}});
return cvtenv;_LL51E:;_LL51F:(void)_throw(_tmp7F4);_LL51B:;}}}{struct
_handler_cons _tmp7F8;_push_handler(& _tmp7F8);{int _tmp7FA=0;if(setjmp(_tmp7F8.handler))
_tmp7FA=1;if(!_tmp7FA){{struct _RegionHandle _tmp7FB=_new_region("r");struct
_RegionHandle*r=& _tmp7FB;_push_region(r);{void*_tmp7FC=Cyc_Tcenv_lookup_ordinary(
r,te,loc,_tmp7EC);struct Cyc_Absyn_Tuniondecl*_tmp7FD;struct Cyc_Absyn_Tunionfield*
_tmp7FE;_LL521: if(*((int*)_tmp7FC)!= 2)goto _LL523;_tmp7FD=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp7FC)->f1;_tmp7FE=((struct Cyc_Tcenv_TunionRes_struct*)_tmp7FC)->f2;_LL522: tuf=
_tmp7FE;tud=_tmp7FD;if(tud->is_xtunion != _tmp7ED)({struct Cyc_String_pa_struct
_tmp801;_tmp801.tag=0;_tmp801.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp7EB));{void*_tmp7FF[1]={& _tmp801};Cyc_Tcutil_terr(
loc,({const char*_tmp800="[x]tunion is different from type declaration %s";
_tag_dynforward(_tmp800,sizeof(char),_get_zero_arr_size(_tmp800,48));}),
_tag_dynforward(_tmp7FF,sizeof(void*),1));}});goto _LL520;_LL523:;_LL524:({struct
Cyc_String_pa_struct _tmp805;_tmp805.tag=0;_tmp805.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7EB));{struct Cyc_String_pa_struct
_tmp804;_tmp804.tag=0;_tmp804.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp7EC));{void*_tmp802[2]={& _tmp804,& _tmp805};Cyc_Tcutil_terr(
loc,({const char*_tmp803="unbound field %s in type tunion %s";_tag_dynforward(
_tmp803,sizeof(char),_get_zero_arr_size(_tmp803,35));}),_tag_dynforward(_tmp802,
sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv _tmp806=cvtenv;_npop_handler(1);
return _tmp806;}_LL520:;};_pop_region(r);};_pop_handler();}else{void*_tmp7F9=(
void*)_exn_thrown;void*_tmp808=_tmp7F9;_LL526: if(_tmp808 != Cyc_Dict_Absent)goto
_LL528;_LL527:({struct Cyc_String_pa_struct _tmp80C;_tmp80C.tag=0;_tmp80C.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7EB));{
struct Cyc_String_pa_struct _tmp80B;_tmp80B.tag=0;_tmp80B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7EC));{void*
_tmp809[2]={& _tmp80B,& _tmp80C};Cyc_Tcutil_terr(loc,({const char*_tmp80A="unbound field %s in type tunion %s";
_tag_dynforward(_tmp80A,sizeof(char),_get_zero_arr_size(_tmp80A,35));}),
_tag_dynforward(_tmp809,sizeof(void*),2));}}});return cvtenv;_LL528:;_LL529:(void)
_throw(_tmp808);_LL525:;}}}*_tmp76E=(void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp80D=_cycalloc(sizeof(*_tmp80D));_tmp80D[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp80E;_tmp80E.tag=1;_tmp80E.f1=tud;_tmp80E.f2=tuf;_tmp80E;});_tmp80D;});
_tmp7EE=tud;_tmp7EF=tuf;goto _LL51A;}_LL519: if(*((int*)_tmp7E9)!= 1)goto _LL516;
_tmp7EE=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp7E9)->f1;_tmp7EF=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp7E9)->f2;_LL51A: {struct Cyc_List_List*tvs=
_tmp7EE->tvs;for(0;_tmp76F != 0  && tvs != 0;(_tmp76F=_tmp76F->tl,tvs=tvs->tl)){
void*t1=(void*)_tmp76F->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1,0);}if(_tmp76F
!= 0)({struct Cyc_String_pa_struct _tmp812;_tmp812.tag=0;_tmp812.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7EF->name));{
struct Cyc_String_pa_struct _tmp811;_tmp811.tag=0;_tmp811.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7EE->name));{
void*_tmp80F[2]={& _tmp811,& _tmp812};Cyc_Tcutil_terr(loc,({const char*_tmp810="too many type arguments for tunion %s.%s";
_tag_dynforward(_tmp810,sizeof(char),_get_zero_arr_size(_tmp810,41));}),
_tag_dynforward(_tmp80F,sizeof(void*),2));}}});if(tvs != 0)({struct Cyc_String_pa_struct
_tmp816;_tmp816.tag=0;_tmp816.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp7EF->name));{struct Cyc_String_pa_struct _tmp815;
_tmp815.tag=0;_tmp815.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp7EE->name));{void*_tmp813[2]={& _tmp815,& _tmp816};Cyc_Tcutil_terr(loc,({const
char*_tmp814="too few type arguments for tunion %s.%s";_tag_dynforward(_tmp814,
sizeof(char),_get_zero_arr_size(_tmp814,40));}),_tag_dynforward(_tmp813,sizeof(
void*),2));}}});goto _LL516;}_LL516:;}goto _LL4CD;_LL4DC: if(*((int*)_tmp75B)!= 4)
goto _LL4DE;_tmp770=((struct Cyc_Absyn_PointerType_struct*)_tmp75B)->f1;_tmp771=(
void*)_tmp770.elt_typ;_tmp772=_tmp770.elt_tq;_tmp773=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_PointerType_struct*)_tmp75B)->f1).elt_tq;_tmp774=_tmp770.ptr_atts;
_tmp775=(void*)_tmp774.rgn;_tmp776=_tmp774.nullable;_tmp777=_tmp774.bounds;
_tmp778=_tmp774.zero_term;_LL4DD: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp771,0);_tmp773->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp773->print_const,_tmp771);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp775,allow_unique);{void*_tmp817=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp778))->v;int _tmp818;
_LL52B: if((int)_tmp817 != 0)goto _LL52D;_LL52C:{void*_tmp819=Cyc_Tcutil_compress(
_tmp771);void*_tmp81A;_LL532: if(_tmp819 <= (void*)4)goto _LL534;if(*((int*)_tmp819)
!= 5)goto _LL534;_tmp81A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp819)->f2;
if((int)_tmp81A != 0)goto _LL534;_LL533:((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp778,
Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL531;_LL534:;_LL535:((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp778,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL531;
_LL531:;}goto _LL52A;_LL52D: if(_tmp817 <= (void*)1)goto _LL52F;if(*((int*)_tmp817)
!= 0)goto _LL52F;_tmp818=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp817)->f1;
if(_tmp818 != 1)goto _LL52F;_LL52E: if(!Cyc_Tcutil_admits_zero(_tmp771))({struct Cyc_String_pa_struct
_tmp81D;_tmp81D.tag=0;_tmp81D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(_tmp771));{void*_tmp81B[1]={& _tmp81D};Cyc_Tcutil_terr(loc,({
const char*_tmp81C="cannot have a pointer to zero-terminated %s elements";
_tag_dynforward(_tmp81C,sizeof(char),_get_zero_arr_size(_tmp81C,53));}),
_tag_dynforward(_tmp81B,sizeof(void*),1));}});is_zero_terminated=1;goto _LL52A;
_LL52F:;_LL530: is_zero_terminated=0;goto _LL52A;_LL52A:;}{void*_tmp81E=(void*)(
Cyc_Absyn_compress_conref(_tmp777))->v;void*_tmp81F;void*_tmp820;void*_tmp821;
struct Cyc_Absyn_Exp*_tmp822;void*_tmp823;void*_tmp824;_LL537: if((int)_tmp81E != 0)
goto _LL539;_LL538: goto _LL53A;_LL539: if(_tmp81E <= (void*)1)goto _LL53B;if(*((int*)
_tmp81E)!= 0)goto _LL53B;_tmp81F=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp81E)->f1;if((int)_tmp81F != 0)goto _LL53B;_LL53A: goto _LL536;_LL53B: if(_tmp81E
<= (void*)1)goto _LL53D;if(*((int*)_tmp81E)!= 0)goto _LL53D;_tmp820=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp81E)->f1;if((int)_tmp820 != 1)goto _LL53D;
_LL53C: goto _LL536;_LL53D: if(_tmp81E <= (void*)1)goto _LL53F;if(*((int*)_tmp81E)!= 
0)goto _LL53F;_tmp821=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp81E)->f1;if(
_tmp821 <= (void*)2)goto _LL53F;if(*((int*)_tmp821)!= 0)goto _LL53F;_tmp822=((
struct Cyc_Absyn_Upper_b_struct*)_tmp821)->f1;_LL53E: Cyc_Tcexp_tcExp(te,0,_tmp822);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp822))({void*_tmp825[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp826="pointer bounds expression is not an unsigned int";
_tag_dynforward(_tmp826,sizeof(char),_get_zero_arr_size(_tmp826,49));}),
_tag_dynforward(_tmp825,sizeof(void*),0));});{unsigned int _tmp828;int _tmp829;
struct _tuple7 _tmp827=Cyc_Evexp_eval_const_uint_exp(_tmp822);_tmp828=_tmp827.f1;
_tmp829=_tmp827.f2;if(is_zero_terminated  && (!_tmp829  || _tmp828 < 1))({void*
_tmp82A[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp82B="zero-terminated pointer cannot point to empty sequence";
_tag_dynforward(_tmp82B,sizeof(char),_get_zero_arr_size(_tmp82B,55));}),
_tag_dynforward(_tmp82A,sizeof(void*),0));});goto _LL536;}_LL53F: if(_tmp81E <= (
void*)1)goto _LL541;if(*((int*)_tmp81E)!= 0)goto _LL541;_tmp823=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp81E)->f1;if(_tmp823 <= (void*)2)goto _LL541;if(*((int*)_tmp823)!= 1)goto _LL541;
_tmp824=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp823)->f1;_LL540: cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,_tmp824,0);goto _LL536;
_LL541: if(_tmp81E <= (void*)1)goto _LL536;if(*((int*)_tmp81E)!= 1)goto _LL536;
_LL542:({void*_tmp82C[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp82D="forward constraint";
_tag_dynforward(_tmp82D,sizeof(char),_get_zero_arr_size(_tmp82D,19));}),
_tag_dynforward(_tmp82C,sizeof(void*),0));});_LL536:;}goto _LL4CD;}_LL4DE: if(*((
int*)_tmp75B)!= 17)goto _LL4E0;_tmp779=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp75B)->f1;_LL4DF: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp779,allow_unique);goto _LL4CD;_LL4E0: if(*((int*)_tmp75B)!= 14)goto _LL4E2;
_tmp77A=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp75B)->f1;_LL4E1: cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp77A,allow_unique);goto
_LL4CD;_LL4E2: if(*((int*)_tmp75B)!= 5)goto _LL4E4;_LL4E3: goto _LL4E5;_LL4E4: if((
int)_tmp75B != 1)goto _LL4E6;_LL4E5: goto _LL4E7;_LL4E6: if(_tmp75B <= (void*)4)goto
_LL4F4;if(*((int*)_tmp75B)!= 6)goto _LL4E8;_LL4E7: goto _LL4CD;_LL4E8: if(*((int*)
_tmp75B)!= 7)goto _LL4EA;_tmp77B=((struct Cyc_Absyn_ArrayType_struct*)_tmp75B)->f1;
_tmp77C=(void*)_tmp77B.elt_type;_tmp77D=_tmp77B.tq;_tmp77E=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp75B)->f1).tq;_tmp77F=_tmp77B.num_elts;
_tmp780=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp75B)->f1).num_elts;
_tmp781=_tmp77B.zero_term;_tmp782=_tmp77B.zt_loc;_LL4E9: {struct Cyc_Absyn_Exp*
_tmp82E=*_tmp780;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,
_tmp77C,0);_tmp77E->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp77E->print_const,
_tmp77C);{int is_zero_terminated;{void*_tmp82F=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp781))->v;int _tmp830;
_LL544: if((int)_tmp82F != 0)goto _LL546;_LL545:((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp781,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL543;_LL546: if(
_tmp82F <= (void*)1)goto _LL548;if(*((int*)_tmp82F)!= 0)goto _LL548;_tmp830=(int)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp82F)->f1;if(_tmp830 != 1)goto _LL548;_LL547:
if(!Cyc_Tcutil_admits_zero(_tmp77C))({struct Cyc_String_pa_struct _tmp833;_tmp833.tag=
0;_tmp833.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
_tmp77C));{void*_tmp831[1]={& _tmp833};Cyc_Tcutil_terr(loc,({const char*_tmp832="cannot have a zero-terminated array of %s elements";
_tag_dynforward(_tmp832,sizeof(char),_get_zero_arr_size(_tmp832,51));}),
_tag_dynforward(_tmp831,sizeof(void*),1));}});is_zero_terminated=1;goto _LL543;
_LL548:;_LL549: is_zero_terminated=0;goto _LL543;_LL543:;}if(_tmp82E == 0){if(
is_zero_terminated)*_tmp780=(_tmp82E=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,
0));else{({void*_tmp834[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp835="array bound defaults to 0 here";
_tag_dynforward(_tmp835,sizeof(char),_get_zero_arr_size(_tmp835,31));}),
_tag_dynforward(_tmp834,sizeof(void*),0));});*_tmp780=(_tmp82E=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(0,0));}}Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp82E);
if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_tmp82E))({void*_tmp836[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp837="array bounds expression is not constant";
_tag_dynforward(_tmp837,sizeof(char),_get_zero_arr_size(_tmp837,40));}),
_tag_dynforward(_tmp836,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_uint_typ(te,(
struct Cyc_Absyn_Exp*)_tmp82E))({void*_tmp838[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp839="array bounds expression is not an unsigned int";_tag_dynforward(
_tmp839,sizeof(char),_get_zero_arr_size(_tmp839,47));}),_tag_dynforward(_tmp838,
sizeof(void*),0));});{unsigned int _tmp83B;int _tmp83C;struct _tuple7 _tmp83A=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp82E);_tmp83B=_tmp83A.f1;_tmp83C=_tmp83A.f2;if((
is_zero_terminated  && _tmp83C) && _tmp83B < 1)({void*_tmp83D[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp83E="zero terminated array cannot have zero elements";
_tag_dynforward(_tmp83E,sizeof(char),_get_zero_arr_size(_tmp83E,48));}),
_tag_dynforward(_tmp83D,sizeof(void*),0));});goto _LL4CD;}}}_LL4EA: if(*((int*)
_tmp75B)!= 8)goto _LL4EC;_tmp783=((struct Cyc_Absyn_FnType_struct*)_tmp75B)->f1;
_tmp784=_tmp783.tvars;_tmp785=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp75B)->f1).tvars;_tmp786=_tmp783.effect;_tmp787=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp75B)->f1).effect;_tmp788=(void*)_tmp783.ret_typ;
_tmp789=_tmp783.args;_tmp78A=_tmp783.c_varargs;_tmp78B=_tmp783.cyc_varargs;
_tmp78C=_tmp783.rgn_po;_tmp78D=_tmp783.attributes;_LL4EB: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp78D != 0;_tmp78D=_tmp78D->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp78D->hd))({struct Cyc_String_pa_struct _tmp841;
_tmp841.tag=0;_tmp841.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absyn_attribute2string((
void*)_tmp78D->hd));{void*_tmp83F[1]={& _tmp841};Cyc_Tcutil_terr(loc,({const char*
_tmp840="bad function type attribute %s";_tag_dynforward(_tmp840,sizeof(char),
_get_zero_arr_size(_tmp840,31));}),_tag_dynforward(_tmp83F,sizeof(void*),1));}});{
void*_tmp842=(void*)_tmp78D->hd;void*_tmp843;int _tmp844;int _tmp845;_LL54B: if((
int)_tmp842 != 0)goto _LL54D;_LL54C: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}
goto _LL54A;_LL54D: if((int)_tmp842 != 1)goto _LL54F;_LL54E: if(!seen_cdecl){
seen_cdecl=1;++ num_convs;}goto _LL54A;_LL54F: if((int)_tmp842 != 2)goto _LL551;
_LL550: if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL54A;_LL551: if(
_tmp842 <= (void*)17)goto _LL553;if(*((int*)_tmp842)!= 3)goto _LL553;_tmp843=(void*)((
struct Cyc_Absyn_Format_att_struct*)_tmp842)->f1;_tmp844=((struct Cyc_Absyn_Format_att_struct*)
_tmp842)->f2;_tmp845=((struct Cyc_Absyn_Format_att_struct*)_tmp842)->f3;_LL552:
if(!seen_format){seen_format=1;ft=_tmp843;fmt_desc_arg=_tmp844;fmt_arg_start=
_tmp845;}else{({void*_tmp846[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp847="function can't have multiple format attributes";
_tag_dynforward(_tmp847,sizeof(char),_get_zero_arr_size(_tmp847,47));}),
_tag_dynforward(_tmp846,sizeof(void*),0));});}goto _LL54A;_LL553:;_LL554: goto
_LL54A;_LL54A:;}}if(num_convs > 1)({void*_tmp848[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp849="function can't have multiple calling conventions";
_tag_dynforward(_tmp849,sizeof(char),_get_zero_arr_size(_tmp849,49));}),
_tag_dynforward(_tmp848,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*
_tmp785);{struct Cyc_List_List*b=*_tmp785;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)
b->hd)->identity=Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(
cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp84A=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp84B;_LL556: if(*((int*)
_tmp84A)!= 0)goto _LL558;_tmp84B=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp84A)->f1;
if((int)_tmp84B != 1)goto _LL558;_LL557:({struct Cyc_String_pa_struct _tmp84E;
_tmp84E.tag=0;_tmp84E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)b->hd)->name);{void*_tmp84C[1]={& _tmp84E};Cyc_Tcutil_terr(
loc,({const char*_tmp84D="function attempts to abstract Mem type variable %s";
_tag_dynforward(_tmp84D,sizeof(char),_get_zero_arr_size(_tmp84D,51));}),
_tag_dynforward(_tmp84C,sizeof(void*),1));}});goto _LL555;_LL558:;_LL559: goto
_LL555;_LL555:;}}}{struct Cyc_Tcutil_CVTEnv _tmp84F=({struct Cyc_Tcutil_CVTEnv
_tmp8CE;_tmp8CE.kind_env=cvtenv.kind_env;_tmp8CE.free_vars=0;_tmp8CE.free_evars=
0;_tmp8CE.generalize_evars=cvtenv.generalize_evars;_tmp8CE.fn_result=1;_tmp8CE;});
_tmp84F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp84F,(void*)1,_tmp788,1);_tmp84F.fn_result=
0;{struct Cyc_List_List*a=_tmp789;for(0;a != 0;a=a->tl){struct _tuple2*_tmp850=(
struct _tuple2*)a->hd;void*_tmp851=(*_tmp850).f3;_tmp84F=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp84F,(void*)1,_tmp851,1);((*_tmp850).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp850).f2).print_const,_tmp851);}}if(_tmp78B != 0){if(_tmp78A)({void*
_tmp852[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp853="both c_vararg and cyc_vararg";_tag_dynforward(_tmp853,sizeof(
char),_get_zero_arr_size(_tmp853,29));}),_tag_dynforward(_tmp852,sizeof(void*),0));});{
void*_tmp855;int _tmp856;struct Cyc_Absyn_VarargInfo _tmp854=*_tmp78B;_tmp855=(void*)
_tmp854.type;_tmp856=_tmp854.inject;_tmp84F=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp84F,(void*)1,_tmp855,1);(_tmp78B->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp78B->tq).print_const,_tmp855);if(_tmp856){void*_tmp857=Cyc_Tcutil_compress(
_tmp855);struct Cyc_Absyn_TunionInfo _tmp858;void*_tmp859;struct Cyc_Absyn_Tuniondecl**
_tmp85A;_LL55B: if(_tmp857 <= (void*)4)goto _LL55D;if(*((int*)_tmp857)!= 2)goto
_LL55D;_tmp858=((struct Cyc_Absyn_TunionType_struct*)_tmp857)->f1;_tmp859=(void*)
_tmp858.tunion_info;if(*((int*)_tmp859)!= 1)goto _LL55D;_tmp85A=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp859)->f1;_LL55C: if((*_tmp85A)->is_flat)({void*_tmp85B[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp85C="cant inject into a flat tunion";_tag_dynforward(_tmp85C,
sizeof(char),_get_zero_arr_size(_tmp85C,31));}),_tag_dynforward(_tmp85B,sizeof(
void*),0));});goto _LL55A;_LL55D:;_LL55E:({void*_tmp85D[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp85E="can't inject a non-[x]tunion type";_tag_dynforward(_tmp85E,
sizeof(char),_get_zero_arr_size(_tmp85E,34));}),_tag_dynforward(_tmp85D,sizeof(
void*),0));});goto _LL55A;_LL55A:;}}}if(seen_format){int _tmp85F=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp789);if((((fmt_desc_arg < 0  || fmt_desc_arg
> _tmp85F) || fmt_arg_start < 0) || _tmp78B == 0  && fmt_arg_start != 0) || _tmp78B
!= 0  && fmt_arg_start != _tmp85F + 1)({void*_tmp860[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp861="bad format descriptor";_tag_dynforward(_tmp861,sizeof(char),
_get_zero_arr_size(_tmp861,22));}),_tag_dynforward(_tmp860,sizeof(void*),0));});
else{void*_tmp863;struct _tuple2 _tmp862=*((struct _tuple2*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp789,fmt_desc_arg - 1);_tmp863=_tmp862.f3;{void*_tmp864=
Cyc_Tcutil_compress(_tmp863);struct Cyc_Absyn_PtrInfo _tmp865;void*_tmp866;struct
Cyc_Absyn_PtrAtts _tmp867;struct Cyc_Absyn_Conref*_tmp868;struct Cyc_Absyn_Conref*
_tmp869;_LL560: if(_tmp864 <= (void*)4)goto _LL562;if(*((int*)_tmp864)!= 4)goto
_LL562;_tmp865=((struct Cyc_Absyn_PointerType_struct*)_tmp864)->f1;_tmp866=(void*)
_tmp865.elt_typ;_tmp867=_tmp865.ptr_atts;_tmp868=_tmp867.bounds;_tmp869=_tmp867.zero_term;
_LL561:{struct _tuple6 _tmp86B=({struct _tuple6 _tmp86A;_tmp86A.f1=Cyc_Tcutil_compress(
_tmp866);_tmp86A.f2=Cyc_Absyn_conref_def((void*)((void*)0),_tmp868);_tmp86A;});
void*_tmp86C;void*_tmp86D;void*_tmp86E;void*_tmp86F;_LL565: _tmp86C=_tmp86B.f1;
if(_tmp86C <= (void*)4)goto _LL567;if(*((int*)_tmp86C)!= 5)goto _LL567;_tmp86D=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp86C)->f1;if((int)_tmp86D != 2)goto
_LL567;_tmp86E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp86C)->f2;if((int)
_tmp86E != 0)goto _LL567;_tmp86F=_tmp86B.f2;if((int)_tmp86F != 0)goto _LL567;_LL566:
goto _LL564;_LL567:;_LL568:({void*_tmp870[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp871="format descriptor is not a char ? type";_tag_dynforward(_tmp871,sizeof(
char),_get_zero_arr_size(_tmp871,39));}),_tag_dynforward(_tmp870,sizeof(void*),0));});
goto _LL564;_LL564:;}goto _LL55F;_LL562:;_LL563:({void*_tmp872[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp873="format descriptor is not a char ? type";_tag_dynforward(
_tmp873,sizeof(char),_get_zero_arr_size(_tmp873,39));}),_tag_dynforward(_tmp872,
sizeof(void*),0));});goto _LL55F;_LL55F:;}if(fmt_arg_start != 0){int problem;{
struct _tuple6 _tmp875=({struct _tuple6 _tmp874;_tmp874.f1=ft;_tmp874.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp78B))->type);_tmp874;});void*
_tmp876;void*_tmp877;struct Cyc_Absyn_TunionInfo _tmp878;void*_tmp879;struct Cyc_Absyn_Tuniondecl**
_tmp87A;struct Cyc_Absyn_Tuniondecl*_tmp87B;void*_tmp87C;void*_tmp87D;struct Cyc_Absyn_TunionInfo
_tmp87E;void*_tmp87F;struct Cyc_Absyn_Tuniondecl**_tmp880;struct Cyc_Absyn_Tuniondecl*
_tmp881;_LL56A: _tmp876=_tmp875.f1;if((int)_tmp876 != 0)goto _LL56C;_tmp877=_tmp875.f2;
if(_tmp877 <= (void*)4)goto _LL56C;if(*((int*)_tmp877)!= 2)goto _LL56C;_tmp878=((
struct Cyc_Absyn_TunionType_struct*)_tmp877)->f1;_tmp879=(void*)_tmp878.tunion_info;
if(*((int*)_tmp879)!= 1)goto _LL56C;_tmp87A=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp879)->f1;_tmp87B=*_tmp87A;_LL56B: problem=Cyc_Absyn_qvar_cmp(_tmp87B->name,
Cyc_Absyn_tunion_print_arg_qvar)!= 0;goto _LL569;_LL56C: _tmp87C=_tmp875.f1;if((
int)_tmp87C != 1)goto _LL56E;_tmp87D=_tmp875.f2;if(_tmp87D <= (void*)4)goto _LL56E;
if(*((int*)_tmp87D)!= 2)goto _LL56E;_tmp87E=((struct Cyc_Absyn_TunionType_struct*)
_tmp87D)->f1;_tmp87F=(void*)_tmp87E.tunion_info;if(*((int*)_tmp87F)!= 1)goto
_LL56E;_tmp880=((struct Cyc_Absyn_KnownTunion_struct*)_tmp87F)->f1;_tmp881=*
_tmp880;_LL56D: problem=Cyc_Absyn_qvar_cmp(_tmp881->name,Cyc_Absyn_tunion_scanf_arg_qvar)
!= 0;goto _LL569;_LL56E:;_LL56F: problem=1;goto _LL569;_LL569:;}if(problem)({void*
_tmp882[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp883="format attribute and vararg types don't match";
_tag_dynforward(_tmp883,sizeof(char),_get_zero_arr_size(_tmp883,46));}),
_tag_dynforward(_tmp882,sizeof(void*),0));});}}}{struct Cyc_List_List*rpo=_tmp78C;
for(0;rpo != 0;rpo=rpo->tl){struct _tuple6 _tmp885;void*_tmp886;void*_tmp887;struct
_tuple6*_tmp884=(struct _tuple6*)rpo->hd;_tmp885=*_tmp884;_tmp886=_tmp885.f1;
_tmp887=_tmp885.f2;_tmp84F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp84F,(void*)4,
_tmp886,0);_tmp84F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp84F,(void*)3,_tmp887,
0);}}if(*_tmp787 != 0)_tmp84F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp84F,(void*)
4,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp787))->v,0);else{struct Cyc_List_List*
effect=0;{struct Cyc_List_List*tvs=_tmp84F.free_vars;for(0;tvs != 0;tvs=tvs->tl){
void*_tmp888=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);
struct Cyc_Core_Opt*_tmp889;struct Cyc_Core_Opt**_tmp88A;void*_tmp88B;void*_tmp88C;
void*_tmp88D;void*_tmp88E;struct Cyc_Core_Opt*_tmp88F;struct Cyc_Core_Opt**_tmp890;
void*_tmp891;void*_tmp892;struct Cyc_Core_Opt*_tmp893;struct Cyc_Core_Opt**_tmp894;
_LL571: if(*((int*)_tmp888)!= 2)goto _LL573;_tmp889=((struct Cyc_Absyn_Less_kb_struct*)
_tmp888)->f1;_tmp88A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp888)->f1;_tmp88B=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp888)->f2;if((
int)_tmp88B != 3)goto _LL573;_LL572:*_tmp88A=({struct Cyc_Core_Opt*_tmp895=
_cycalloc(sizeof(*_tmp895));_tmp895->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp896=_cycalloc(sizeof(*_tmp896));_tmp896[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp897;_tmp897.tag=0;_tmp897.f1=(void*)((void*)3);_tmp897;});_tmp896;}));
_tmp895;});goto _LL574;_LL573: if(*((int*)_tmp888)!= 0)goto _LL575;_tmp88C=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp888)->f1;if((int)_tmp88C != 3)goto _LL575;_LL574:
effect=({struct Cyc_List_List*_tmp898=_cycalloc(sizeof(*_tmp898));_tmp898->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp899=_cycalloc(sizeof(*
_tmp899));_tmp899[0]=({struct Cyc_Absyn_AccessEff_struct _tmp89A;_tmp89A.tag=19;
_tmp89A.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp89B=_cycalloc(
sizeof(*_tmp89B));_tmp89B[0]=({struct Cyc_Absyn_VarType_struct _tmp89C;_tmp89C.tag=
1;_tmp89C.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp89C;});_tmp89B;}));_tmp89A;});
_tmp899;}));_tmp898->tl=effect;_tmp898;});goto _LL570;_LL575: if(*((int*)_tmp888)
!= 2)goto _LL577;_tmp88D=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp888)->f2;
if((int)_tmp88D != 5)goto _LL577;_LL576: goto _LL578;_LL577: if(*((int*)_tmp888)!= 0)
goto _LL579;_tmp88E=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp888)->f1;if((int)
_tmp88E != 5)goto _LL579;_LL578: goto _LL570;_LL579: if(*((int*)_tmp888)!= 2)goto
_LL57B;_tmp88F=((struct Cyc_Absyn_Less_kb_struct*)_tmp888)->f1;_tmp890=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp888)->f1;_tmp891=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp888)->f2;if((int)_tmp891 != 4)goto _LL57B;_LL57A:*_tmp890=({struct Cyc_Core_Opt*
_tmp89D=_cycalloc(sizeof(*_tmp89D));_tmp89D->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp89E=_cycalloc(sizeof(*_tmp89E));_tmp89E[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp89F;_tmp89F.tag=0;_tmp89F.f1=(void*)((void*)4);_tmp89F;});_tmp89E;}));
_tmp89D;});goto _LL57C;_LL57B: if(*((int*)_tmp888)!= 0)goto _LL57D;_tmp892=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp888)->f1;if((int)_tmp892 != 4)goto _LL57D;_LL57C:
effect=({struct Cyc_List_List*_tmp8A0=_cycalloc(sizeof(*_tmp8A0));_tmp8A0->hd=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp8A1=_cycalloc(sizeof(*_tmp8A1));
_tmp8A1[0]=({struct Cyc_Absyn_VarType_struct _tmp8A2;_tmp8A2.tag=1;_tmp8A2.f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp8A2;});_tmp8A1;}));_tmp8A0->tl=effect;_tmp8A0;});
goto _LL570;_LL57D: if(*((int*)_tmp888)!= 1)goto _LL57F;_tmp893=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp888)->f1;_tmp894=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp888)->f1;_LL57E:*_tmp894=({struct Cyc_Core_Opt*_tmp8A3=_cycalloc(sizeof(*
_tmp8A3));_tmp8A3->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp8A4=
_cycalloc(sizeof(*_tmp8A4));_tmp8A4[0]=({struct Cyc_Absyn_Less_kb_struct _tmp8A5;
_tmp8A5.tag=2;_tmp8A5.f1=0;_tmp8A5.f2=(void*)((void*)0);_tmp8A5;});_tmp8A4;}));
_tmp8A3;});goto _LL580;_LL57F:;_LL580: effect=({struct Cyc_List_List*_tmp8A6=
_cycalloc(sizeof(*_tmp8A6));_tmp8A6->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp8A7=_cycalloc(sizeof(*_tmp8A7));_tmp8A7[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp8A8;_tmp8A8.tag=21;_tmp8A8.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp8A9=_cycalloc(sizeof(*_tmp8A9));_tmp8A9[0]=({struct Cyc_Absyn_VarType_struct
_tmp8AA;_tmp8AA.tag=1;_tmp8AA.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp8AA;});
_tmp8A9;}));_tmp8A8;});_tmp8A7;}));_tmp8A6->tl=effect;_tmp8A6;});goto _LL570;
_LL570:;}}{struct Cyc_List_List*ts=_tmp84F.free_evars;for(0;ts != 0;ts=ts->tl){
void*_tmp8AB=Cyc_Tcutil_typ_kind((void*)ts->hd);_LL582: if((int)_tmp8AB != 3)goto
_LL584;_LL583: effect=({struct Cyc_List_List*_tmp8AC=_cycalloc(sizeof(*_tmp8AC));
_tmp8AC->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp8AD=_cycalloc(
sizeof(*_tmp8AD));_tmp8AD[0]=({struct Cyc_Absyn_AccessEff_struct _tmp8AE;_tmp8AE.tag=
19;_tmp8AE.f1=(void*)((void*)ts->hd);_tmp8AE;});_tmp8AD;}));_tmp8AC->tl=effect;
_tmp8AC;});goto _LL581;_LL584: if((int)_tmp8AB != 4)goto _LL586;_LL585: effect=({
struct Cyc_List_List*_tmp8AF=_cycalloc(sizeof(*_tmp8AF));_tmp8AF->hd=(void*)((
void*)ts->hd);_tmp8AF->tl=effect;_tmp8AF;});goto _LL581;_LL586: if((int)_tmp8AB != 
5)goto _LL588;_LL587: goto _LL581;_LL588:;_LL589: effect=({struct Cyc_List_List*
_tmp8B0=_cycalloc(sizeof(*_tmp8B0));_tmp8B0->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp8B1=_cycalloc(sizeof(*_tmp8B1));_tmp8B1[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp8B2;_tmp8B2.tag=21;_tmp8B2.f1=(void*)((void*)ts->hd);_tmp8B2;});_tmp8B1;}));
_tmp8B0->tl=effect;_tmp8B0;});goto _LL581;_LL581:;}}*_tmp787=({struct Cyc_Core_Opt*
_tmp8B3=_cycalloc(sizeof(*_tmp8B3));_tmp8B3->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp8B4=_cycalloc(sizeof(*_tmp8B4));_tmp8B4[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp8B5;_tmp8B5.tag=20;_tmp8B5.f1=effect;_tmp8B5;});_tmp8B4;}));_tmp8B3;});}if(*
_tmp785 != 0){struct Cyc_List_List*bs=*_tmp785;for(0;bs != 0;bs=bs->tl){void*
_tmp8B6=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp8B7;struct Cyc_Core_Opt**_tmp8B8;struct Cyc_Core_Opt*
_tmp8B9;struct Cyc_Core_Opt**_tmp8BA;void*_tmp8BB;struct Cyc_Core_Opt*_tmp8BC;
struct Cyc_Core_Opt**_tmp8BD;void*_tmp8BE;struct Cyc_Core_Opt*_tmp8BF;struct Cyc_Core_Opt**
_tmp8C0;void*_tmp8C1;void*_tmp8C2;_LL58B: if(*((int*)_tmp8B6)!= 1)goto _LL58D;
_tmp8B7=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp8B6)->f1;_tmp8B8=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8B6)->f1;_LL58C:({struct Cyc_String_pa_struct
_tmp8C5;_tmp8C5.tag=0;_tmp8C5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)bs->hd)->name);{void*_tmp8C3[1]={& _tmp8C5};Cyc_Tcutil_warn(
loc,({const char*_tmp8C4="Type variable %s unconstrained, assuming boxed";
_tag_dynforward(_tmp8C4,sizeof(char),_get_zero_arr_size(_tmp8C4,47));}),
_tag_dynforward(_tmp8C3,sizeof(void*),1));}});_tmp8BA=_tmp8B8;goto _LL58E;_LL58D:
if(*((int*)_tmp8B6)!= 2)goto _LL58F;_tmp8B9=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8B6)->f1;_tmp8BA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8B6)->f1;_tmp8BB=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B6)->f2;if((
int)_tmp8BB != 1)goto _LL58F;_LL58E: _tmp8BD=_tmp8BA;goto _LL590;_LL58F: if(*((int*)
_tmp8B6)!= 2)goto _LL591;_tmp8BC=((struct Cyc_Absyn_Less_kb_struct*)_tmp8B6)->f1;
_tmp8BD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8B6)->f1;
_tmp8BE=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B6)->f2;if((int)_tmp8BE != 
0)goto _LL591;_LL590:*_tmp8BD=({struct Cyc_Core_Opt*_tmp8C6=_cycalloc(sizeof(*
_tmp8C6));_tmp8C6->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8C7=
_cycalloc(sizeof(*_tmp8C7));_tmp8C7[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp8C8;
_tmp8C8.tag=0;_tmp8C8.f1=(void*)((void*)2);_tmp8C8;});_tmp8C7;}));_tmp8C6;});
goto _LL58A;_LL591: if(*((int*)_tmp8B6)!= 2)goto _LL593;_tmp8BF=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8B6)->f1;_tmp8C0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8B6)->f1;_tmp8C1=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B6)->f2;
_LL592:*_tmp8C0=({struct Cyc_Core_Opt*_tmp8C9=_cycalloc(sizeof(*_tmp8C9));_tmp8C9->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8CA=_cycalloc(sizeof(*_tmp8CA));
_tmp8CA[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp8CB;_tmp8CB.tag=0;_tmp8CB.f1=(void*)
_tmp8C1;_tmp8CB;});_tmp8CA;}));_tmp8C9;});goto _LL58A;_LL593: if(*((int*)_tmp8B6)
!= 0)goto _LL595;_tmp8C2=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8B6)->f1;if((
int)_tmp8C2 != 1)goto _LL595;_LL594:({void*_tmp8CC[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp8CD="functions can't abstract M types";_tag_dynforward(_tmp8CD,
sizeof(char),_get_zero_arr_size(_tmp8CD,33));}),_tag_dynforward(_tmp8CC,sizeof(
void*),0));});goto _LL58A;_LL595:;_LL596: goto _LL58A;_LL58A:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
_tmp84F.kind_env,*_tmp785);_tmp84F.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmp84F.free_vars,*_tmp785);{struct Cyc_List_List*tvs=_tmp84F.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,(
struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=_tmp84F.free_evars;
for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,(
void*)evs->hd);}}goto _LL4CD;}}_LL4EC: if(*((int*)_tmp75B)!= 9)goto _LL4EE;_tmp78E=((
struct Cyc_Absyn_TupleType_struct*)_tmp75B)->f1;_LL4ED: for(0;_tmp78E != 0;_tmp78E=
_tmp78E->tl){struct _tuple4*_tmp8CF=(struct _tuple4*)_tmp78E->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,(*_tmp8CF).f2,0);((*_tmp8CF).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp8CF).f1).print_const,(*_tmp8CF).f2);}goto _LL4CD;_LL4EE: if(*((int*)
_tmp75B)!= 11)goto _LL4F0;_tmp78F=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp75B)->f1;_tmp790=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp75B)->f2;_LL4EF:{
struct _RegionHandle _tmp8D0=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp8D0;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp790 != 0;_tmp790=_tmp790->tl){struct Cyc_Absyn_Aggrfield _tmp8D2;struct
_dynforward_ptr*_tmp8D3;struct Cyc_Absyn_Tqual _tmp8D4;struct Cyc_Absyn_Tqual*
_tmp8D5;void*_tmp8D6;struct Cyc_Absyn_Exp*_tmp8D7;struct Cyc_List_List*_tmp8D8;
struct Cyc_Absyn_Aggrfield*_tmp8D1=(struct Cyc_Absyn_Aggrfield*)_tmp790->hd;
_tmp8D2=*_tmp8D1;_tmp8D3=_tmp8D2.name;_tmp8D4=_tmp8D2.tq;_tmp8D5=(struct Cyc_Absyn_Tqual*)&(*
_tmp8D1).tq;_tmp8D6=(void*)_tmp8D2.type;_tmp8D7=_tmp8D2.width;_tmp8D8=_tmp8D2.attributes;
if(((int(*)(int(*compare)(struct _dynforward_ptr*,struct _dynforward_ptr*),struct
Cyc_List_List*l,struct _dynforward_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,
_tmp8D3))({struct Cyc_String_pa_struct _tmp8DB;_tmp8DB.tag=0;_tmp8DB.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp8D3);{void*_tmp8D9[1]={& _tmp8DB};
Cyc_Tcutil_terr(loc,({const char*_tmp8DA="duplicate field %s";_tag_dynforward(
_tmp8DA,sizeof(char),_get_zero_arr_size(_tmp8DA,19));}),_tag_dynforward(_tmp8D9,
sizeof(void*),1));}});if(Cyc_strcmp((struct _dynforward_ptr)*_tmp8D3,({const char*
_tmp8DC="";_tag_dynforward(_tmp8DC,sizeof(char),_get_zero_arr_size(_tmp8DC,1));}))
!= 0)prev_fields=({struct Cyc_List_List*_tmp8DD=_region_malloc(aprev_rgn,sizeof(*
_tmp8DD));_tmp8DD->hd=_tmp8D3;_tmp8DD->tl=prev_fields;_tmp8DD;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,_tmp8D6,0);_tmp8D5->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp8D5->print_const,_tmp8D6);if(_tmp78F == (void*)1  && !Cyc_Tcutil_bits_only(
_tmp8D6))({struct Cyc_String_pa_struct _tmp8E0;_tmp8E0.tag=0;_tmp8E0.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp8D3);{void*_tmp8DE[1]={& _tmp8E0};
Cyc_Tcutil_warn(loc,({const char*_tmp8DF="union member %s is not `bits-only' so it can only be written and not read";
_tag_dynforward(_tmp8DF,sizeof(char),_get_zero_arr_size(_tmp8DF,74));}),
_tag_dynforward(_tmp8DE,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(loc,te,
_tmp8D6,_tmp8D7,_tmp8D3);Cyc_Tcutil_check_field_atts(loc,_tmp8D3,_tmp8D8);}};
_pop_region(aprev_rgn);}goto _LL4CD;_LL4F0: if(*((int*)_tmp75B)!= 10)goto _LL4F2;
_tmp791=((struct Cyc_Absyn_AggrType_struct*)_tmp75B)->f1;_tmp792=(void*)_tmp791.aggr_info;
_tmp793=(void**)&(((struct Cyc_Absyn_AggrType_struct*)_tmp75B)->f1).aggr_info;
_tmp794=_tmp791.targs;_tmp795=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp75B)->f1).targs;_LL4F1:{void*_tmp8E1=*_tmp793;void*_tmp8E2;struct _tuple1*
_tmp8E3;struct Cyc_Absyn_Aggrdecl**_tmp8E4;struct Cyc_Absyn_Aggrdecl*_tmp8E5;
_LL598: if(*((int*)_tmp8E1)!= 0)goto _LL59A;_tmp8E2=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp8E1)->f1;_tmp8E3=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp8E1)->f2;_LL599: {
struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp8E6;_push_handler(& _tmp8E6);{
int _tmp8E8=0;if(setjmp(_tmp8E6.handler))_tmp8E8=1;if(!_tmp8E8){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp8E3);*_tmp793=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp8E9=
_cycalloc(sizeof(*_tmp8E9));_tmp8E9[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp8EA;
_tmp8EA.tag=1;_tmp8EA.f1=adp;_tmp8EA;});_tmp8E9;});;_pop_handler();}else{void*
_tmp8E7=(void*)_exn_thrown;void*_tmp8EC=_tmp8E7;_LL59D: if(_tmp8EC != Cyc_Dict_Absent)
goto _LL59F;_LL59E: {struct Cyc_Tcenv_Genv*_tmp8ED=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp8EE=({struct Cyc_Absyn_Aggrdecl*_tmp8F4=_cycalloc(
sizeof(*_tmp8F4));_tmp8F4->kind=(void*)_tmp8E2;_tmp8F4->sc=(void*)((void*)3);
_tmp8F4->name=_tmp8E3;_tmp8F4->tvs=0;_tmp8F4->impl=0;_tmp8F4->attributes=0;
_tmp8F4;});Cyc_Tc_tcAggrdecl(te,_tmp8ED,loc,_tmp8EE);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp8E3);*_tmp793=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp8EF=
_cycalloc(sizeof(*_tmp8EF));_tmp8EF[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp8F0;
_tmp8F0.tag=1;_tmp8F0.f1=adp;_tmp8F0;});_tmp8EF;});if(*_tmp795 != 0){({struct Cyc_String_pa_struct
_tmp8F3;_tmp8F3.tag=0;_tmp8F3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp8E3));{void*_tmp8F1[1]={& _tmp8F3};Cyc_Tcutil_terr(
loc,({const char*_tmp8F2="declare parameterized type %s before using";
_tag_dynforward(_tmp8F2,sizeof(char),_get_zero_arr_size(_tmp8F2,43));}),
_tag_dynforward(_tmp8F1,sizeof(void*),1));}});return cvtenv;}goto _LL59C;}_LL59F:;
_LL5A0:(void)_throw(_tmp8EC);_LL59C:;}}}_tmp8E5=*adp;goto _LL59B;}_LL59A: if(*((
int*)_tmp8E1)!= 1)goto _LL597;_tmp8E4=((struct Cyc_Absyn_KnownAggr_struct*)_tmp8E1)->f1;
_tmp8E5=*_tmp8E4;_LL59B: {struct Cyc_List_List*tvs=_tmp8E5->tvs;struct Cyc_List_List*
ts=*_tmp795;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd,0);}if(ts != 0)({struct Cyc_String_pa_struct _tmp8F7;_tmp8F7.tag=
0;_tmp8F7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp8E5->name));{void*_tmp8F5[1]={& _tmp8F7};Cyc_Tcutil_terr(loc,({const char*
_tmp8F6="too many parameters for type %s";_tag_dynforward(_tmp8F6,sizeof(char),
_get_zero_arr_size(_tmp8F6,32));}),_tag_dynforward(_tmp8F5,sizeof(void*),1));}});
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);hidden_ts=({struct Cyc_List_List*_tmp8F8=_cycalloc(sizeof(*_tmp8F8));_tmp8F8->hd=(
void*)e;_tmp8F8->tl=hidden_ts;_tmp8F8;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,0);}*_tmp795=Cyc_List_imp_append(*_tmp795,Cyc_List_imp_rev(
hidden_ts));}}_LL597:;}goto _LL4CD;_LL4F2: if(*((int*)_tmp75B)!= 16)goto _LL4F4;
_tmp796=((struct Cyc_Absyn_TypedefType_struct*)_tmp75B)->f1;_tmp797=((struct Cyc_Absyn_TypedefType_struct*)
_tmp75B)->f2;_tmp798=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp75B)->f2;_tmp799=((struct Cyc_Absyn_TypedefType_struct*)_tmp75B)->f3;_tmp79A=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp75B)->f3;
_tmp79B=((struct Cyc_Absyn_TypedefType_struct*)_tmp75B)->f4;_tmp79C=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp75B)->f4;_LL4F3: {struct Cyc_List_List*
targs=*_tmp798;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp8F9;
_push_handler(& _tmp8F9);{int _tmp8FB=0;if(setjmp(_tmp8F9.handler))_tmp8FB=1;if(!
_tmp8FB){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp796);;_pop_handler();}else{
void*_tmp8FA=(void*)_exn_thrown;void*_tmp8FD=_tmp8FA;_LL5A2: if(_tmp8FD != Cyc_Dict_Absent)
goto _LL5A4;_LL5A3:({struct Cyc_String_pa_struct _tmp900;_tmp900.tag=0;_tmp900.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp796));{
void*_tmp8FE[1]={& _tmp900};Cyc_Tcutil_terr(loc,({const char*_tmp8FF="unbound typedef name %s";
_tag_dynforward(_tmp8FF,sizeof(char),_get_zero_arr_size(_tmp8FF,24));}),
_tag_dynforward(_tmp8FE,sizeof(void*),1));}});return cvtenv;_LL5A4:;_LL5A5:(void)
_throw(_tmp8FD);_LL5A1:;}}}*_tmp79A=(struct Cyc_Absyn_Typedefdecl*)td;_tmp796[0]=(
td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(
ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,0);inst=({
struct Cyc_List_List*_tmp901=_cycalloc(sizeof(*_tmp901));_tmp901->hd=({struct
_tuple8*_tmp902=_cycalloc(sizeof(*_tmp902));_tmp902->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp902->f2=(void*)ts->hd;_tmp902;});_tmp901->tl=inst;_tmp901;});}if(ts
!= 0)({struct Cyc_String_pa_struct _tmp905;_tmp905.tag=0;_tmp905.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp796));{void*
_tmp903[1]={& _tmp905};Cyc_Tcutil_terr(loc,({const char*_tmp904="too many parameters for typedef %s";
_tag_dynforward(_tmp904,sizeof(char),_get_zero_arr_size(_tmp904,35));}),
_tag_dynforward(_tmp903,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp906=
_cycalloc(sizeof(*_tmp906));_tmp906->hd=(void*)e;_tmp906->tl=hidden_ts;_tmp906;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,0);inst=({struct Cyc_List_List*
_tmp907=_cycalloc(sizeof(*_tmp907));_tmp907->hd=({struct _tuple8*_tmp908=
_cycalloc(sizeof(*_tmp908));_tmp908->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp908->f2=
e;_tmp908;});_tmp907->tl=inst;_tmp907;});}*_tmp798=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_substitute(inst,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp79C=({void**_tmp909=
_cycalloc(sizeof(*_tmp909));_tmp909[0]=new_typ;_tmp909;});}goto _LL4CD;}}_LL4F4:
if((int)_tmp75B != 3)goto _LL4F6;_LL4F5: if(!allow_unique)({void*_tmp90A[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp90B="`U only allowed in outermost pointers";_tag_dynforward(
_tmp90B,sizeof(char),_get_zero_arr_size(_tmp90B,38));}),_tag_dynforward(_tmp90A,
sizeof(void*),0));});goto _LL4CD;_LL4F6: if(_tmp75B <= (void*)4)goto _LL4F8;if(*((
int*)_tmp75B)!= 18)goto _LL4F8;_LL4F7: goto _LL4F9;_LL4F8: if((int)_tmp75B != 2)goto
_LL4FA;_LL4F9: goto _LL4CD;_LL4FA: if(_tmp75B <= (void*)4)goto _LL4FC;if(*((int*)
_tmp75B)!= 15)goto _LL4FC;_tmp79D=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp75B)->f1;_LL4FB: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp79D,1);goto _LL4CD;_LL4FC: if(_tmp75B <= (void*)4)goto _LL4FE;if(*((int*)_tmp75B)
!= 19)goto _LL4FE;_tmp79E=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp75B)->f1;
_LL4FD: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp79E,0);
goto _LL4CD;_LL4FE: if(_tmp75B <= (void*)4)goto _LL500;if(*((int*)_tmp75B)!= 21)goto
_LL500;_tmp79F=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp75B)->f1;_LL4FF:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp79F,0);goto
_LL4CD;_LL500: if(_tmp75B <= (void*)4)goto _LL4CD;if(*((int*)_tmp75B)!= 20)goto
_LL4CD;_tmp7A0=((struct Cyc_Absyn_JoinEff_struct*)_tmp75B)->f1;_LL501: for(0;
_tmp7A0 != 0;_tmp7A0=_tmp7A0->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)4,(void*)_tmp7A0->hd,0);}goto _LL4CD;_LL4CD:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind))({struct Cyc_String_pa_struct _tmp910;
_tmp910.tag=0;_tmp910.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(
expected_kind));{struct Cyc_String_pa_struct _tmp90F;_tmp90F.tag=0;_tmp90F.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(
t)));{struct Cyc_String_pa_struct _tmp90E;_tmp90E.tag=0;_tmp90E.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp90C[
3]={& _tmp90E,& _tmp90F,& _tmp910};Cyc_Tcutil_terr(loc,({const char*_tmp90D="type %s has kind %s but as used here needs kind %s";
_tag_dynforward(_tmp90D,sizeof(char),_get_zero_arr_size(_tmp90D,51));}),
_tag_dynforward(_tmp90C,sizeof(void*),3));}}}});return cvtenv;}static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*kind_env,void*expected_kind,int generalize_evars,void*t,
int allow_unique){struct Cyc_Tcutil_CVTEnv _tmp911=Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp91A;_tmp91A.kind_env=kind_env;_tmp91A.free_vars=
0;_tmp91A.free_evars=0;_tmp91A.generalize_evars=generalize_evars;_tmp91A.fn_result=
0;_tmp91A;}),expected_kind,t,allow_unique);{struct Cyc_List_List*vs=_tmp911.free_vars;
for(0;vs != 0;vs=vs->tl){_tmp911.kind_env=Cyc_Tcutil_fast_add_free_tvar(kind_env,(
struct Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp911.free_evars;for(
0;evs != 0;evs=evs->tl){void*_tmp912=Cyc_Tcutil_compress((void*)evs->hd);struct
Cyc_Core_Opt*_tmp913;struct Cyc_Core_Opt**_tmp914;_LL5A7: if(_tmp912 <= (void*)4)
goto _LL5A9;if(*((int*)_tmp912)!= 0)goto _LL5A9;_tmp913=((struct Cyc_Absyn_Evar_struct*)
_tmp912)->f4;_tmp914=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp912)->f4;_LL5A8: if(*_tmp914 == 0)*_tmp914=({struct Cyc_Core_Opt*_tmp915=
_cycalloc(sizeof(*_tmp915));_tmp915->v=kind_env;_tmp915;});else{struct Cyc_List_List*
_tmp916=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp914))->v;
struct Cyc_List_List*_tmp917=0;for(0;_tmp916 != 0;_tmp916=_tmp916->tl){if(((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct
Cyc_Absyn_Tvar*)_tmp916->hd))_tmp917=({struct Cyc_List_List*_tmp918=_cycalloc(
sizeof(*_tmp918));_tmp918->hd=(struct Cyc_Absyn_Tvar*)_tmp916->hd;_tmp918->tl=
_tmp917;_tmp918;});}*_tmp914=({struct Cyc_Core_Opt*_tmp919=_cycalloc(sizeof(*
_tmp919));_tmp919->v=_tmp917;_tmp919;});}goto _LL5A6;_LL5A9:;_LL5AA: goto _LL5A6;
_LL5A6:;}}return _tmp911;}void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*t,int allow_unique){int generalize_evars=Cyc_Tcutil_is_function_type(
t);struct Cyc_List_List*_tmp91B=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Tcutil_CVTEnv
_tmp91C=Cyc_Tcutil_check_valid_type(loc,te,_tmp91B,(void*)1,generalize_evars,t,
allow_unique);struct Cyc_List_List*_tmp91D=_tmp91C.free_vars;struct Cyc_List_List*
_tmp91E=_tmp91C.free_evars;if(_tmp91B != 0){struct Cyc_List_List*_tmp91F=0;{struct
Cyc_List_List*_tmp920=_tmp91D;for(0;(unsigned int)_tmp920;_tmp920=_tmp920->tl){
struct Cyc_Absyn_Tvar*_tmp921=(struct Cyc_Absyn_Tvar*)_tmp920->hd;int found=0;{
struct Cyc_List_List*_tmp922=_tmp91B;for(0;(unsigned int)_tmp922;_tmp922=_tmp922->tl){
if(Cyc_Absyn_tvar_cmp(_tmp921,(struct Cyc_Absyn_Tvar*)_tmp922->hd)== 0){found=1;
break;}}}if(!found)_tmp91F=({struct Cyc_List_List*_tmp923=_cycalloc(sizeof(*
_tmp923));_tmp923->hd=(struct Cyc_Absyn_Tvar*)_tmp920->hd;_tmp923->tl=_tmp91F;
_tmp923;});}}_tmp91D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp91F);}{struct Cyc_List_List*x=_tmp91D;for(0;x != 0;x=x->tl){void*_tmp924=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmp925;struct Cyc_Core_Opt**
_tmp926;struct Cyc_Core_Opt*_tmp927;struct Cyc_Core_Opt**_tmp928;void*_tmp929;
struct Cyc_Core_Opt*_tmp92A;struct Cyc_Core_Opt**_tmp92B;void*_tmp92C;struct Cyc_Core_Opt*
_tmp92D;struct Cyc_Core_Opt**_tmp92E;void*_tmp92F;void*_tmp930;_LL5AC: if(*((int*)
_tmp924)!= 1)goto _LL5AE;_tmp925=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp924)->f1;
_tmp926=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp924)->f1;
_LL5AD: _tmp928=_tmp926;goto _LL5AF;_LL5AE: if(*((int*)_tmp924)!= 2)goto _LL5B0;
_tmp927=((struct Cyc_Absyn_Less_kb_struct*)_tmp924)->f1;_tmp928=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp924)->f1;_tmp929=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp924)->f2;if((int)_tmp929 != 1)goto _LL5B0;_LL5AF: _tmp92B=_tmp928;goto _LL5B1;
_LL5B0: if(*((int*)_tmp924)!= 2)goto _LL5B2;_tmp92A=((struct Cyc_Absyn_Less_kb_struct*)
_tmp924)->f1;_tmp92B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp924)->f1;_tmp92C=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp924)->f2;if((
int)_tmp92C != 0)goto _LL5B2;_LL5B1:*_tmp92B=({struct Cyc_Core_Opt*_tmp931=
_cycalloc(sizeof(*_tmp931));_tmp931->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp932=_cycalloc(sizeof(*_tmp932));_tmp932[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp933;_tmp933.tag=0;_tmp933.f1=(void*)((void*)2);_tmp933;});_tmp932;}));
_tmp931;});goto _LL5AB;_LL5B2: if(*((int*)_tmp924)!= 2)goto _LL5B4;_tmp92D=((struct
Cyc_Absyn_Less_kb_struct*)_tmp924)->f1;_tmp92E=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp924)->f1;_tmp92F=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp924)->f2;_LL5B3:*_tmp92E=({struct Cyc_Core_Opt*_tmp934=_cycalloc(sizeof(*
_tmp934));_tmp934->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp935=
_cycalloc(sizeof(*_tmp935));_tmp935[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp936;
_tmp936.tag=0;_tmp936.f1=(void*)_tmp92F;_tmp936;});_tmp935;}));_tmp934;});goto
_LL5AB;_LL5B4: if(*((int*)_tmp924)!= 0)goto _LL5B6;_tmp930=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp924)->f1;if((int)_tmp930 != 1)goto _LL5B6;_LL5B5:({struct Cyc_String_pa_struct
_tmp939;_tmp939.tag=0;_tmp939.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));{void*_tmp937[1]={& _tmp939};
Cyc_Tcutil_terr(loc,({const char*_tmp938="type variable %s cannot have kind M";
_tag_dynforward(_tmp938,sizeof(char),_get_zero_arr_size(_tmp938,36));}),
_tag_dynforward(_tmp937,sizeof(void*),1));}});goto _LL5AB;_LL5B6:;_LL5B7: goto
_LL5AB;_LL5AB:;}}if(_tmp91D != 0  || _tmp91E != 0){{void*_tmp93A=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp93B;struct Cyc_List_List*_tmp93C;struct Cyc_List_List**
_tmp93D;struct Cyc_Core_Opt*_tmp93E;void*_tmp93F;struct Cyc_List_List*_tmp940;int
_tmp941;struct Cyc_Absyn_VarargInfo*_tmp942;struct Cyc_List_List*_tmp943;struct Cyc_List_List*
_tmp944;_LL5B9: if(_tmp93A <= (void*)4)goto _LL5BB;if(*((int*)_tmp93A)!= 8)goto
_LL5BB;_tmp93B=((struct Cyc_Absyn_FnType_struct*)_tmp93A)->f1;_tmp93C=_tmp93B.tvars;
_tmp93D=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmp93A)->f1).tvars;
_tmp93E=_tmp93B.effect;_tmp93F=(void*)_tmp93B.ret_typ;_tmp940=_tmp93B.args;
_tmp941=_tmp93B.c_varargs;_tmp942=_tmp93B.cyc_varargs;_tmp943=_tmp93B.rgn_po;
_tmp944=_tmp93B.attributes;_LL5BA: if(*_tmp93D == 0){*_tmp93D=_tmp91D;_tmp91D=0;}
goto _LL5B8;_LL5BB:;_LL5BC: goto _LL5B8;_LL5B8:;}if(_tmp91D != 0)({struct Cyc_String_pa_struct
_tmp947;_tmp947.tag=0;_tmp947.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)_tmp91D->hd)->name);{void*_tmp945[1]={& _tmp947};Cyc_Tcutil_terr(
loc,({const char*_tmp946="unbound type variable %s";_tag_dynforward(_tmp946,
sizeof(char),_get_zero_arr_size(_tmp946,25));}),_tag_dynforward(_tmp945,sizeof(
void*),1));}});if(_tmp91E != 0)for(0;_tmp91E != 0;_tmp91E=_tmp91E->tl){void*e=(
void*)_tmp91E->hd;void*_tmp948=Cyc_Tcutil_typ_kind(e);_LL5BE: if((int)_tmp948 != 3)
goto _LL5C0;_LL5BF: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp949[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp94A="can't unify evar with heap!";_tag_dynforward(_tmp94A,sizeof(char),
_get_zero_arr_size(_tmp94A,28));}),_tag_dynforward(_tmp949,sizeof(void*),0));});
goto _LL5BD;_LL5C0: if((int)_tmp948 != 4)goto _LL5C2;_LL5C1: if(!Cyc_Tcutil_unify(e,
Cyc_Absyn_empty_effect))({void*_tmp94B[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp94C="can't unify evar with {}!";
_tag_dynforward(_tmp94C,sizeof(char),_get_zero_arr_size(_tmp94C,26));}),
_tag_dynforward(_tmp94B,sizeof(void*),0));});goto _LL5BD;_LL5C2:;_LL5C3:({struct
Cyc_String_pa_struct _tmp950;_tmp950.tag=0;_tmp950.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp94F;_tmp94F.tag=0;_tmp94F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(e));{void*_tmp94D[2]={& _tmp94F,& _tmp950};Cyc_Tcutil_terr(
loc,({const char*_tmp94E="hidden type variable %s isn't abstracted in type %s";
_tag_dynforward(_tmp94E,sizeof(char),_get_zero_arr_size(_tmp94E,52));}),
_tag_dynforward(_tmp94D,sizeof(void*),2));}}});goto _LL5BD;_LL5BD:;}}}void Cyc_Tcutil_check_no_unique_region(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp951;_tmp951.kind_env=0;_tmp951.free_vars=0;
_tmp951.free_evars=0;_tmp951.generalize_evars=0;_tmp951.fn_result=0;_tmp951;}),(
void*)1,t,0);}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t,1);{void*_tmp952=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp953;struct Cyc_List_List*_tmp954;struct Cyc_Core_Opt*
_tmp955;void*_tmp956;_LL5C5: if(_tmp952 <= (void*)4)goto _LL5C7;if(*((int*)_tmp952)
!= 8)goto _LL5C7;_tmp953=((struct Cyc_Absyn_FnType_struct*)_tmp952)->f1;_tmp954=
_tmp953.tvars;_tmp955=_tmp953.effect;_tmp956=(void*)_tmp953.ret_typ;_LL5C6: fd->tvs=
_tmp954;fd->effect=_tmp955;goto _LL5C4;_LL5C7:;_LL5C8:({void*_tmp957[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp958="check_fndecl_valid_type: not a FnType";_tag_dynforward(_tmp958,sizeof(
char),_get_zero_arr_size(_tmp958,38));}),_tag_dynforward(_tmp957,sizeof(void*),0));});
_LL5C4:;}{struct _RegionHandle _tmp959=_new_region("r");struct _RegionHandle*r=&
_tmp959;_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr*(*f)(struct _tuple13*),struct Cyc_List_List*
x))Cyc_List_rmap)(r,(struct _dynforward_ptr*(*)(struct _tuple13*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,({const char*_tmp95A="function declaration has repeated parameter";
_tag_dynforward(_tmp95A,sizeof(char),_get_zero_arr_size(_tmp95A,44));}));;
_pop_region(r);}fd->cached_typ=({struct Cyc_Core_Opt*_tmp95B=_cycalloc(sizeof(*
_tmp95B));_tmp95B->v=(void*)t;_tmp95B;});}void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,void*expected_kind,
int allow_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp95C=Cyc_Tcutil_check_valid_type(
loc,te,bound_tvars,expected_kind,0,t,1);struct Cyc_List_List*_tmp95D=Cyc_Tcutil_remove_bound_tvars(
_tmp95C.free_vars,bound_tvars);struct Cyc_List_List*_tmp95E=_tmp95C.free_evars;{
struct Cyc_List_List*fs=_tmp95D;for(0;fs != 0;fs=fs->tl){struct _dynforward_ptr*
_tmp95F=((struct Cyc_Absyn_Tvar*)fs->hd)->name;({struct Cyc_String_pa_struct
_tmp963;_tmp963.tag=0;_tmp963.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct _tmp962;_tmp962.tag=0;
_tmp962.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp95F);{void*
_tmp960[2]={& _tmp962,& _tmp963};Cyc_Tcutil_terr(loc,({const char*_tmp961="unbound type variable %s in type %s";
_tag_dynforward(_tmp961,sizeof(char),_get_zero_arr_size(_tmp961,36));}),
_tag_dynforward(_tmp960,sizeof(void*),2));}}});}}if(!allow_evars  && _tmp95E != 0)
for(0;_tmp95E != 0;_tmp95E=_tmp95E->tl){void*e=(void*)_tmp95E->hd;void*_tmp964=
Cyc_Tcutil_typ_kind(e);_LL5CA: if((int)_tmp964 != 3)goto _LL5CC;_LL5CB: if(!Cyc_Tcutil_unify(
e,(void*)2))({void*_tmp965[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp966="can't unify evar with heap!";
_tag_dynforward(_tmp966,sizeof(char),_get_zero_arr_size(_tmp966,28));}),
_tag_dynforward(_tmp965,sizeof(void*),0));});goto _LL5C9;_LL5CC: if((int)_tmp964 != 
4)goto _LL5CE;_LL5CD: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*
_tmp967[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp968="can't unify evar with {}!";_tag_dynforward(_tmp968,sizeof(
char),_get_zero_arr_size(_tmp968,26));}),_tag_dynforward(_tmp967,sizeof(void*),0));});
goto _LL5C9;_LL5CE:;_LL5CF:({struct Cyc_String_pa_struct _tmp96C;_tmp96C.tag=0;
_tmp96C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp96B;_tmp96B.tag=0;_tmp96B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(e));{void*_tmp969[
2]={& _tmp96B,& _tmp96C};Cyc_Tcutil_terr(loc,({const char*_tmp96A="hidden type variable %s isn't abstracted in type %s";
_tag_dynforward(_tmp96A,sizeof(char),_get_zero_arr_size(_tmp96A,52));}),
_tag_dynforward(_tmp969,sizeof(void*),2));}}});goto _LL5C9;_LL5C9:;}}void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)tv->identity=Cyc_Tcutil_new_tvar_id();}
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,
tvs);}static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct
Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dynforward_ptr(*a2string)(
void*),struct _dynforward_ptr msg){for(0;vs != 0;vs=vs->tl){struct Cyc_List_List*vs2=
vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(void*)vs2->hd)== 0)({
struct Cyc_String_pa_struct _tmp970;_tmp970.tag=0;_tmp970.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)a2string((void*)vs->hd));{struct Cyc_String_pa_struct
_tmp96F;_tmp96F.tag=0;_tmp96F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
msg);{void*_tmp96D[2]={& _tmp96F,& _tmp970};Cyc_Tcutil_terr(loc,({const char*
_tmp96E="%s: %s";_tag_dynforward(_tmp96E,sizeof(char),_get_zero_arr_size(_tmp96E,
7));}),_tag_dynforward(_tmp96D,sizeof(void*),2));}}});}}}static struct
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
Cyc_Tcutil_tvar2string,({const char*_tmp971="duplicate type variable";
_tag_dynforward(_tmp971,sizeof(char),_get_zero_arr_size(_tmp971,24));}));}struct
_tuple18{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple19{struct Cyc_List_List*
f1;void*f2;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){if(Cyc_strcmp((struct _dynforward_ptr)*((struct Cyc_Absyn_Aggrfield*)
sd_fields->hd)->name,({const char*_tmp972="";_tag_dynforward(_tmp972,sizeof(char),
_get_zero_arr_size(_tmp972,1));}))!= 0)fields=({struct Cyc_List_List*_tmp973=
_cycalloc(sizeof(*_tmp973));_tmp973->hd=({struct _tuple18*_tmp974=_cycalloc(
sizeof(*_tmp974));_tmp974->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd;_tmp974->f2=
0;_tmp974;});_tmp973->tl=fields;_tmp973;});}}fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct Cyc_List_List*ans=0;for(
0;des != 0;des=des->tl){struct _tuple19 _tmp976;struct Cyc_List_List*_tmp977;void*
_tmp978;struct _tuple19*_tmp975=(struct _tuple19*)des->hd;_tmp976=*_tmp975;_tmp977=
_tmp976.f1;_tmp978=_tmp976.f2;if(_tmp977 == 0){struct Cyc_List_List*_tmp979=fields;
for(0;_tmp979 != 0;_tmp979=_tmp979->tl){if(!(*((struct _tuple18*)_tmp979->hd)).f2){(*((
struct _tuple18*)_tmp979->hd)).f2=1;(*((struct _tuple19*)des->hd)).f1=(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp97A=_cycalloc(sizeof(*_tmp97A));_tmp97A->hd=(void*)((
void*)({struct Cyc_Absyn_FieldName_struct*_tmp97B=_cycalloc(sizeof(*_tmp97B));
_tmp97B[0]=({struct Cyc_Absyn_FieldName_struct _tmp97C;_tmp97C.tag=1;_tmp97C.f1=((*((
struct _tuple18*)_tmp979->hd)).f1)->name;_tmp97C;});_tmp97B;}));_tmp97A->tl=0;
_tmp97A;});ans=({struct Cyc_List_List*_tmp97D=_region_malloc(rgn,sizeof(*_tmp97D));
_tmp97D->hd=({struct _tuple20*_tmp97E=_region_malloc(rgn,sizeof(*_tmp97E));
_tmp97E->f1=(*((struct _tuple18*)_tmp979->hd)).f1;_tmp97E->f2=_tmp978;_tmp97E;});
_tmp97D->tl=ans;_tmp97D;});break;}}if(_tmp979 == 0)({void*_tmp97F[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp980="too many arguments to struct";_tag_dynforward(_tmp980,
sizeof(char),_get_zero_arr_size(_tmp980,29));}),_tag_dynforward(_tmp97F,sizeof(
void*),0));});}else{if(_tmp977->tl != 0)({void*_tmp981[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp982="multiple designators are not supported";_tag_dynforward(
_tmp982,sizeof(char),_get_zero_arr_size(_tmp982,39));}),_tag_dynforward(_tmp981,
sizeof(void*),0));});else{void*_tmp983=(void*)_tmp977->hd;struct _dynforward_ptr*
_tmp984;_LL5D1: if(*((int*)_tmp983)!= 0)goto _LL5D3;_LL5D2:({void*_tmp985[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp986="array designator used in argument to struct";
_tag_dynforward(_tmp986,sizeof(char),_get_zero_arr_size(_tmp986,44));}),
_tag_dynforward(_tmp985,sizeof(void*),0));});goto _LL5D0;_LL5D3: if(*((int*)
_tmp983)!= 1)goto _LL5D0;_tmp984=((struct Cyc_Absyn_FieldName_struct*)_tmp983)->f1;
_LL5D4: {struct Cyc_List_List*_tmp987=fields;for(0;_tmp987 != 0;_tmp987=_tmp987->tl){
if(Cyc_strptrcmp(_tmp984,((*((struct _tuple18*)_tmp987->hd)).f1)->name)== 0){if((*((
struct _tuple18*)_tmp987->hd)).f2)({struct Cyc_String_pa_struct _tmp98A;_tmp98A.tag=
0;_tmp98A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp984);{void*
_tmp988[1]={& _tmp98A};Cyc_Tcutil_terr(loc,({const char*_tmp989="field %s has already been used as an argument";
_tag_dynforward(_tmp989,sizeof(char),_get_zero_arr_size(_tmp989,46));}),
_tag_dynforward(_tmp988,sizeof(void*),1));}});(*((struct _tuple18*)_tmp987->hd)).f2=
1;ans=({struct Cyc_List_List*_tmp98B=_region_malloc(rgn,sizeof(*_tmp98B));_tmp98B->hd=({
struct _tuple20*_tmp98C=_region_malloc(rgn,sizeof(*_tmp98C));_tmp98C->f1=(*((
struct _tuple18*)_tmp987->hd)).f1;_tmp98C->f2=_tmp978;_tmp98C;});_tmp98B->tl=ans;
_tmp98B;});break;}}if(_tmp987 == 0)({struct Cyc_String_pa_struct _tmp98F;_tmp98F.tag=
0;_tmp98F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp984);{void*
_tmp98D[1]={& _tmp98F};Cyc_Tcutil_terr(loc,({const char*_tmp98E="bad field designator %s";
_tag_dynforward(_tmp98E,sizeof(char),_get_zero_arr_size(_tmp98E,24));}),
_tag_dynforward(_tmp98D,sizeof(void*),1));}});goto _LL5D0;}_LL5D0:;}}}for(0;
fields != 0;fields=fields->tl){if(!(*((struct _tuple18*)fields->hd)).f2){({void*
_tmp990[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp991="too few arguments to struct";
_tag_dynforward(_tmp991,sizeof(char),_get_zero_arr_size(_tmp991,28));}),
_tag_dynforward(_tmp990,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest,int*forward_only){*forward_only=0;{void*_tmp992=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp993;void*_tmp994;struct Cyc_Absyn_PtrAtts _tmp995;
struct Cyc_Absyn_Conref*_tmp996;_LL5D6: if(_tmp992 <= (void*)4)goto _LL5D8;if(*((int*)
_tmp992)!= 4)goto _LL5D8;_tmp993=((struct Cyc_Absyn_PointerType_struct*)_tmp992)->f1;
_tmp994=(void*)_tmp993.elt_typ;_tmp995=_tmp993.ptr_atts;_tmp996=_tmp995.bounds;
_LL5D7: {struct Cyc_Absyn_Conref*_tmp997=Cyc_Absyn_compress_conref(_tmp996);void*
_tmp998=(void*)(Cyc_Absyn_compress_conref(_tmp997))->v;void*_tmp999;void*_tmp99A;
_LL5DB: if(_tmp998 <= (void*)1)goto _LL5DF;if(*((int*)_tmp998)!= 0)goto _LL5DD;
_tmp999=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp998)->f1;if((int)_tmp999
!= 0)goto _LL5DD;_LL5DC:*forward_only=1;*elt_typ_dest=_tmp994;return 1;_LL5DD: if(*((
int*)_tmp998)!= 0)goto _LL5DF;_tmp99A=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp998)->f1;if((int)_tmp99A != 1)goto _LL5DF;_LL5DE:*elt_typ_dest=_tmp994;return 1;
_LL5DF: if((int)_tmp998 != 0)goto _LL5E1;_LL5E0:(void*)(_tmp997->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp99B=_cycalloc(sizeof(*_tmp99B));_tmp99B[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp99C;_tmp99C.tag=0;_tmp99C.f1=(void*)((void*)
0);_tmp99C;});_tmp99B;})));*forward_only=1;*elt_typ_dest=_tmp994;return 1;_LL5E1:;
_LL5E2: return 0;_LL5DA:;}_LL5D8:;_LL5D9: return 0;_LL5D5:;}}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmp99D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp99E;void*_tmp99F;struct Cyc_Absyn_PtrAtts _tmp9A0;struct Cyc_Absyn_Conref*
_tmp9A1;_LL5E4: if(_tmp99D <= (void*)4)goto _LL5E6;if(*((int*)_tmp99D)!= 4)goto
_LL5E6;_tmp99E=((struct Cyc_Absyn_PointerType_struct*)_tmp99D)->f1;_tmp99F=(void*)
_tmp99E.elt_typ;_tmp9A0=_tmp99E.ptr_atts;_tmp9A1=_tmp9A0.zero_term;_LL5E5:*
elt_typ_dest=_tmp99F;return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp9A1);_LL5E6:;_LL5E7: return 0;_LL5E3:;}int Cyc_Tcutil_is_tagged_pointer_typ(
void*t,int*is_forward_only){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& ignore,is_forward_only);}struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){struct _tuple10 bogus_ans=({struct _tuple10 _tmp9E5;
_tmp9E5.f1=0;_tmp9E5.f2=(void*)2;_tmp9E5;});void*_tmp9A2=(void*)e->r;struct
_tuple1*_tmp9A3;void*_tmp9A4;struct Cyc_Absyn_Exp*_tmp9A5;struct _dynforward_ptr*
_tmp9A6;struct Cyc_Absyn_Exp*_tmp9A7;struct _dynforward_ptr*_tmp9A8;struct Cyc_Absyn_Exp*
_tmp9A9;struct Cyc_Absyn_Exp*_tmp9AA;struct Cyc_Absyn_Exp*_tmp9AB;_LL5E9: if(*((int*)
_tmp9A2)!= 1)goto _LL5EB;_tmp9A3=((struct Cyc_Absyn_Var_e_struct*)_tmp9A2)->f1;
_tmp9A4=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9A2)->f2;_LL5EA: {void*
_tmp9AC=_tmp9A4;struct Cyc_Absyn_Vardecl*_tmp9AD;struct Cyc_Absyn_Vardecl*_tmp9AE;
struct Cyc_Absyn_Vardecl*_tmp9AF;struct Cyc_Absyn_Vardecl*_tmp9B0;_LL5F6: if((int)
_tmp9AC != 0)goto _LL5F8;_LL5F7: goto _LL5F9;_LL5F8: if(_tmp9AC <= (void*)1)goto _LL5FA;
if(*((int*)_tmp9AC)!= 1)goto _LL5FA;_LL5F9: return bogus_ans;_LL5FA: if(_tmp9AC <= (
void*)1)goto _LL5FC;if(*((int*)_tmp9AC)!= 0)goto _LL5FC;_tmp9AD=((struct Cyc_Absyn_Global_b_struct*)
_tmp9AC)->f1;_LL5FB: {void*_tmp9B1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL603: if(_tmp9B1 <= (void*)4)goto _LL605;if(*((int*)
_tmp9B1)!= 7)goto _LL605;_LL604: return({struct _tuple10 _tmp9B2;_tmp9B2.f1=1;
_tmp9B2.f2=(void*)2;_tmp9B2;});_LL605:;_LL606: return({struct _tuple10 _tmp9B3;
_tmp9B3.f1=(_tmp9AD->tq).real_const;_tmp9B3.f2=(void*)2;_tmp9B3;});_LL602:;}
_LL5FC: if(_tmp9AC <= (void*)1)goto _LL5FE;if(*((int*)_tmp9AC)!= 3)goto _LL5FE;
_tmp9AE=((struct Cyc_Absyn_Local_b_struct*)_tmp9AC)->f1;_LL5FD: {void*_tmp9B4=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL608: if(_tmp9B4 <= (void*)
4)goto _LL60A;if(*((int*)_tmp9B4)!= 7)goto _LL60A;_LL609: return({struct _tuple10
_tmp9B5;_tmp9B5.f1=1;_tmp9B5.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp9AE->rgn))->v;
_tmp9B5;});_LL60A:;_LL60B: _tmp9AE->escapes=1;return({struct _tuple10 _tmp9B6;
_tmp9B6.f1=(_tmp9AE->tq).real_const;_tmp9B6.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp9AE->rgn))->v;_tmp9B6;});_LL607:;}_LL5FE: if(_tmp9AC <= (void*)1)
goto _LL600;if(*((int*)_tmp9AC)!= 4)goto _LL600;_tmp9AF=((struct Cyc_Absyn_Pat_b_struct*)
_tmp9AC)->f1;_LL5FF: _tmp9B0=_tmp9AF;goto _LL601;_LL600: if(_tmp9AC <= (void*)1)goto
_LL5F5;if(*((int*)_tmp9AC)!= 2)goto _LL5F5;_tmp9B0=((struct Cyc_Absyn_Param_b_struct*)
_tmp9AC)->f1;_LL601: _tmp9B0->escapes=1;return({struct _tuple10 _tmp9B7;_tmp9B7.f1=(
_tmp9B0->tq).real_const;_tmp9B7.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp9B0->rgn))->v;_tmp9B7;});_LL5F5:;}_LL5EB: if(*((int*)_tmp9A2)!= 23)goto _LL5ED;
_tmp9A5=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp9A2)->f1;_tmp9A6=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp9A2)->f2;_LL5EC: {void*_tmp9B8=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp9A5->topt))->v);struct Cyc_List_List*_tmp9B9;struct Cyc_Absyn_AggrInfo
_tmp9BA;void*_tmp9BB;struct Cyc_Absyn_Aggrdecl**_tmp9BC;struct Cyc_Absyn_Aggrdecl*
_tmp9BD;_LL60D: if(_tmp9B8 <= (void*)4)goto _LL611;if(*((int*)_tmp9B8)!= 11)goto
_LL60F;_tmp9B9=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp9B8)->f2;_LL60E: {
struct Cyc_Absyn_Aggrfield*_tmp9BE=Cyc_Absyn_lookup_field(_tmp9B9,_tmp9A6);if(
_tmp9BE != 0  && _tmp9BE->width != 0)return({struct _tuple10 _tmp9BF;_tmp9BF.f1=(
_tmp9BE->tq).real_const;_tmp9BF.f2=(Cyc_Tcutil_addressof_props(te,_tmp9A5)).f2;
_tmp9BF;});return bogus_ans;}_LL60F: if(*((int*)_tmp9B8)!= 10)goto _LL611;_tmp9BA=((
struct Cyc_Absyn_AggrType_struct*)_tmp9B8)->f1;_tmp9BB=(void*)_tmp9BA.aggr_info;
if(*((int*)_tmp9BB)!= 1)goto _LL611;_tmp9BC=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp9BB)->f1;_tmp9BD=*_tmp9BC;_LL610: {struct Cyc_Absyn_Aggrfield*_tmp9C0=Cyc_Absyn_lookup_decl_field(
_tmp9BD,_tmp9A6);if(_tmp9C0 != 0  && _tmp9C0->width != 0)return({struct _tuple10
_tmp9C1;_tmp9C1.f1=(_tmp9C0->tq).real_const;_tmp9C1.f2=(Cyc_Tcutil_addressof_props(
te,_tmp9A5)).f2;_tmp9C1;});return bogus_ans;}_LL611:;_LL612: return bogus_ans;
_LL60C:;}_LL5ED: if(*((int*)_tmp9A2)!= 24)goto _LL5EF;_tmp9A7=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp9A2)->f1;_tmp9A8=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp9A2)->f2;_LL5EE: {
void*_tmp9C2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp9A7->topt))->v);struct Cyc_Absyn_PtrInfo _tmp9C3;void*_tmp9C4;struct Cyc_Absyn_PtrAtts
_tmp9C5;void*_tmp9C6;_LL614: if(_tmp9C2 <= (void*)4)goto _LL616;if(*((int*)_tmp9C2)
!= 4)goto _LL616;_tmp9C3=((struct Cyc_Absyn_PointerType_struct*)_tmp9C2)->f1;
_tmp9C4=(void*)_tmp9C3.elt_typ;_tmp9C5=_tmp9C3.ptr_atts;_tmp9C6=(void*)_tmp9C5.rgn;
_LL615: {struct Cyc_Absyn_Aggrfield*finfo;{void*_tmp9C7=Cyc_Tcutil_compress(
_tmp9C4);struct Cyc_List_List*_tmp9C8;struct Cyc_Absyn_AggrInfo _tmp9C9;void*
_tmp9CA;struct Cyc_Absyn_Aggrdecl**_tmp9CB;struct Cyc_Absyn_Aggrdecl*_tmp9CC;
_LL619: if(_tmp9C7 <= (void*)4)goto _LL61D;if(*((int*)_tmp9C7)!= 11)goto _LL61B;
_tmp9C8=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp9C7)->f2;_LL61A: finfo=Cyc_Absyn_lookup_field(
_tmp9C8,_tmp9A8);goto _LL618;_LL61B: if(*((int*)_tmp9C7)!= 10)goto _LL61D;_tmp9C9=((
struct Cyc_Absyn_AggrType_struct*)_tmp9C7)->f1;_tmp9CA=(void*)_tmp9C9.aggr_info;
if(*((int*)_tmp9CA)!= 1)goto _LL61D;_tmp9CB=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp9CA)->f1;_tmp9CC=*_tmp9CB;_LL61C: finfo=Cyc_Absyn_lookup_decl_field(_tmp9CC,
_tmp9A8);goto _LL618;_LL61D:;_LL61E: return bogus_ans;_LL618:;}if(finfo != 0  && 
finfo->width != 0)return({struct _tuple10 _tmp9CD;_tmp9CD.f1=(finfo->tq).real_const;
_tmp9CD.f2=_tmp9C6;_tmp9CD;});return bogus_ans;}_LL616:;_LL617: return bogus_ans;
_LL613:;}_LL5EF: if(*((int*)_tmp9A2)!= 22)goto _LL5F1;_tmp9A9=((struct Cyc_Absyn_Deref_e_struct*)
_tmp9A2)->f1;_LL5F0: {void*_tmp9CE=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp9A9->topt))->v);struct Cyc_Absyn_PtrInfo _tmp9CF;struct Cyc_Absyn_Tqual
_tmp9D0;struct Cyc_Absyn_PtrAtts _tmp9D1;void*_tmp9D2;_LL620: if(_tmp9CE <= (void*)4)
goto _LL622;if(*((int*)_tmp9CE)!= 4)goto _LL622;_tmp9CF=((struct Cyc_Absyn_PointerType_struct*)
_tmp9CE)->f1;_tmp9D0=_tmp9CF.elt_tq;_tmp9D1=_tmp9CF.ptr_atts;_tmp9D2=(void*)
_tmp9D1.rgn;_LL621: return({struct _tuple10 _tmp9D3;_tmp9D3.f1=_tmp9D0.real_const;
_tmp9D3.f2=_tmp9D2;_tmp9D3;});_LL622:;_LL623: return bogus_ans;_LL61F:;}_LL5F1: if(*((
int*)_tmp9A2)!= 25)goto _LL5F3;_tmp9AA=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp9A2)->f1;_tmp9AB=((struct Cyc_Absyn_Subscript_e_struct*)_tmp9A2)->f2;_LL5F2: {
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp9AA->topt))->v);
void*_tmp9D4=t;struct Cyc_List_List*_tmp9D5;struct Cyc_Absyn_PtrInfo _tmp9D6;struct
Cyc_Absyn_Tqual _tmp9D7;struct Cyc_Absyn_PtrAtts _tmp9D8;void*_tmp9D9;struct Cyc_Absyn_ArrayInfo
_tmp9DA;struct Cyc_Absyn_Tqual _tmp9DB;_LL625: if(_tmp9D4 <= (void*)4)goto _LL62B;if(*((
int*)_tmp9D4)!= 9)goto _LL627;_tmp9D5=((struct Cyc_Absyn_TupleType_struct*)_tmp9D4)->f1;
_LL626: {unsigned int _tmp9DD;int _tmp9DE;struct _tuple7 _tmp9DC=Cyc_Evexp_eval_const_uint_exp(
_tmp9AB);_tmp9DD=_tmp9DC.f1;_tmp9DE=_tmp9DC.f2;if(!_tmp9DE)return bogus_ans;{
struct _tuple4*_tmp9DF=Cyc_Absyn_lookup_tuple_field(_tmp9D5,(int)_tmp9DD);if(
_tmp9DF != 0)return({struct _tuple10 _tmp9E0;_tmp9E0.f1=((*_tmp9DF).f1).real_const;
_tmp9E0.f2=(Cyc_Tcutil_addressof_props(te,_tmp9AA)).f2;_tmp9E0;});return
bogus_ans;}}_LL627: if(*((int*)_tmp9D4)!= 4)goto _LL629;_tmp9D6=((struct Cyc_Absyn_PointerType_struct*)
_tmp9D4)->f1;_tmp9D7=_tmp9D6.elt_tq;_tmp9D8=_tmp9D6.ptr_atts;_tmp9D9=(void*)
_tmp9D8.rgn;_LL628: return({struct _tuple10 _tmp9E1;_tmp9E1.f1=_tmp9D7.real_const;
_tmp9E1.f2=_tmp9D9;_tmp9E1;});_LL629: if(*((int*)_tmp9D4)!= 7)goto _LL62B;_tmp9DA=((
struct Cyc_Absyn_ArrayType_struct*)_tmp9D4)->f1;_tmp9DB=_tmp9DA.tq;_LL62A: return({
struct _tuple10 _tmp9E2;_tmp9E2.f1=_tmp9DB.real_const;_tmp9E2.f2=(Cyc_Tcutil_addressof_props(
te,_tmp9AA)).f2;_tmp9E2;});_LL62B:;_LL62C: return bogus_ans;_LL624:;}_LL5F3:;
_LL5F4:({void*_tmp9E3[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp9E4="unary & applied to non-lvalue";
_tag_dynforward(_tmp9E4,sizeof(char),_get_zero_arr_size(_tmp9E4,30));}),
_tag_dynforward(_tmp9E3,sizeof(void*),0));});return bogus_ans;_LL5E8:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmp9E6=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmp9E7;void*_tmp9E8;struct Cyc_Absyn_Tqual
_tmp9E9;struct Cyc_Absyn_Conref*_tmp9EA;_LL62E: if(_tmp9E6 <= (void*)4)goto _LL630;
if(*((int*)_tmp9E6)!= 7)goto _LL630;_tmp9E7=((struct Cyc_Absyn_ArrayType_struct*)
_tmp9E6)->f1;_tmp9E8=(void*)_tmp9E7.elt_type;_tmp9E9=_tmp9E7.tq;_tmp9EA=_tmp9E7.zero_term;
_LL62F: {void*_tmp9EC;struct _tuple10 _tmp9EB=Cyc_Tcutil_addressof_props(te,e);
_tmp9EC=_tmp9EB.f2;return Cyc_Absyn_atb_typ(_tmp9E8,_tmp9EC,_tmp9E9,(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp9ED=_cycalloc(sizeof(*_tmp9ED));_tmp9ED[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp9EE;_tmp9EE.tag=0;_tmp9EE.f1=e;_tmp9EE;});
_tmp9ED;}),_tmp9EA);}_LL630:;_LL631: return e_typ;_LL62D:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(
b);{void*_tmp9EF=(void*)b->v;void*_tmp9F0;void*_tmp9F1;void*_tmp9F2;struct Cyc_Absyn_Exp*
_tmp9F3;void*_tmp9F4;_LL633: if(_tmp9EF <= (void*)1)goto _LL63B;if(*((int*)_tmp9EF)
!= 0)goto _LL635;_tmp9F0=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9EF)->f1;
if((int)_tmp9F0 != 1)goto _LL635;_LL634: return;_LL635: if(*((int*)_tmp9EF)!= 0)goto
_LL637;_tmp9F1=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9EF)->f1;if((int)
_tmp9F1 != 0)goto _LL637;_LL636: if(i < 0)({void*_tmp9F5[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp9F6="dereference is out of bounds";_tag_dynforward(_tmp9F6,sizeof(
char),_get_zero_arr_size(_tmp9F6,29));}),_tag_dynforward(_tmp9F5,sizeof(void*),0));});
return;_LL637: if(*((int*)_tmp9EF)!= 0)goto _LL639;_tmp9F2=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp9EF)->f1;if(_tmp9F2 <= (void*)2)goto _LL639;if(*((int*)_tmp9F2)!= 0)goto _LL639;
_tmp9F3=((struct Cyc_Absyn_Upper_b_struct*)_tmp9F2)->f1;_LL638: {unsigned int
_tmp9F8;int _tmp9F9;struct _tuple7 _tmp9F7=Cyc_Evexp_eval_const_uint_exp(_tmp9F3);
_tmp9F8=_tmp9F7.f1;_tmp9F9=_tmp9F7.f2;if(_tmp9F9  && _tmp9F8 <= i)({struct Cyc_Int_pa_struct
_tmp9FD;_tmp9FD.tag=1;_tmp9FD.f1=(unsigned long)((int)i);{struct Cyc_Int_pa_struct
_tmp9FC;_tmp9FC.tag=1;_tmp9FC.f1=(unsigned long)((int)_tmp9F8);{void*_tmp9FA[2]={&
_tmp9FC,& _tmp9FD};Cyc_Tcutil_terr(loc,({const char*_tmp9FB="dereference is out of bounds: %d <= %d";
_tag_dynforward(_tmp9FB,sizeof(char),_get_zero_arr_size(_tmp9FB,39));}),
_tag_dynforward(_tmp9FA,sizeof(void*),2));}}});return;}_LL639: if(*((int*)_tmp9EF)
!= 0)goto _LL63B;_tmp9F4=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9EF)->f1;
if(_tmp9F4 <= (void*)2)goto _LL63B;if(*((int*)_tmp9F4)!= 1)goto _LL63B;_LL63A:
return;_LL63B: if((int)_tmp9EF != 0)goto _LL63D;_LL63C:(void*)(b->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp9FE=_cycalloc(sizeof(*_tmp9FE));_tmp9FE[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp9FF;_tmp9FF.tag=0;_tmp9FF.f1=(void*)((void*)((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmpA00=_cycalloc(sizeof(*_tmpA00));
_tmpA00[0]=({struct Cyc_Absyn_Upper_b_struct _tmpA01;_tmpA01.tag=0;_tmpA01.f1=Cyc_Absyn_uint_exp(
i + 1,0);_tmpA01;});_tmpA00;})));_tmp9FF;});_tmp9FE;})));return;_LL63D:;_LL63E:({
void*_tmpA02[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmpA03="check_bound -- bad compressed conref";
_tag_dynforward(_tmpA03,sizeof(char),_get_zero_arr_size(_tmpA03,37));}),
_tag_dynforward(_tmpA02,sizeof(void*),0));});_LL632:;}}void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(
loc,0,b);}int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b){void*_tmpA04=(
void*)(Cyc_Absyn_compress_conref(b))->v;void*_tmpA05;struct Cyc_Absyn_Exp*_tmpA06;
_LL640: if(_tmpA04 <= (void*)1)goto _LL642;if(*((int*)_tmpA04)!= 0)goto _LL642;
_tmpA05=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpA04)->f1;if(_tmpA05 <= (
void*)2)goto _LL642;if(*((int*)_tmpA05)!= 0)goto _LL642;_tmpA06=((struct Cyc_Absyn_Upper_b_struct*)
_tmpA05)->f1;_LL641: {unsigned int _tmpA08;int _tmpA09;struct _tuple7 _tmpA07=Cyc_Evexp_eval_const_uint_exp(
_tmpA06);_tmpA08=_tmpA07.f1;_tmpA09=_tmpA07.f2;return _tmpA09  && _tmpA08 == 1;}
_LL642:;_LL643: return 0;_LL63F:;}int Cyc_Tcutil_bits_only(void*t){void*_tmpA0A=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmpA0B;void*_tmpA0C;struct Cyc_Absyn_Conref*_tmpA0D;
struct Cyc_List_List*_tmpA0E;struct Cyc_Absyn_AggrInfo _tmpA0F;void*_tmpA10;struct
Cyc_Absyn_AggrInfo _tmpA11;void*_tmpA12;struct Cyc_Absyn_Aggrdecl**_tmpA13;struct
Cyc_Absyn_Aggrdecl*_tmpA14;struct Cyc_List_List*_tmpA15;struct Cyc_List_List*
_tmpA16;_LL645: if((int)_tmpA0A != 0)goto _LL647;_LL646: goto _LL648;_LL647: if(
_tmpA0A <= (void*)4)goto _LL649;if(*((int*)_tmpA0A)!= 5)goto _LL649;_LL648: goto
_LL64A;_LL649: if((int)_tmpA0A != 1)goto _LL64B;_LL64A: goto _LL64C;_LL64B: if(_tmpA0A
<= (void*)4)goto _LL65B;if(*((int*)_tmpA0A)!= 6)goto _LL64D;_LL64C: return 1;_LL64D:
if(*((int*)_tmpA0A)!= 12)goto _LL64F;_LL64E: goto _LL650;_LL64F: if(*((int*)_tmpA0A)
!= 13)goto _LL651;_LL650: return 0;_LL651: if(*((int*)_tmpA0A)!= 7)goto _LL653;
_tmpA0B=((struct Cyc_Absyn_ArrayType_struct*)_tmpA0A)->f1;_tmpA0C=(void*)_tmpA0B.elt_type;
_tmpA0D=_tmpA0B.zero_term;_LL652: return !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmpA0D) && Cyc_Tcutil_bits_only(_tmpA0C);_LL653: if(*((
int*)_tmpA0A)!= 9)goto _LL655;_tmpA0E=((struct Cyc_Absyn_TupleType_struct*)_tmpA0A)->f1;
_LL654: for(0;_tmpA0E != 0;_tmpA0E=_tmpA0E->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple4*)_tmpA0E->hd)).f2))return 0;}return 1;_LL655: if(*((int*)_tmpA0A)!= 
10)goto _LL657;_tmpA0F=((struct Cyc_Absyn_AggrType_struct*)_tmpA0A)->f1;_tmpA10=(
void*)_tmpA0F.aggr_info;if(*((int*)_tmpA10)!= 0)goto _LL657;_LL656: return 0;_LL657:
if(*((int*)_tmpA0A)!= 10)goto _LL659;_tmpA11=((struct Cyc_Absyn_AggrType_struct*)
_tmpA0A)->f1;_tmpA12=(void*)_tmpA11.aggr_info;if(*((int*)_tmpA12)!= 1)goto _LL659;
_tmpA13=((struct Cyc_Absyn_KnownAggr_struct*)_tmpA12)->f1;_tmpA14=*_tmpA13;
_tmpA15=_tmpA11.targs;_LL658: if(_tmpA14->impl == 0)return 0;{struct _RegionHandle
_tmpA17=_new_region("rgn");struct _RegionHandle*rgn=& _tmpA17;_push_region(rgn);{
struct Cyc_List_List*_tmpA18=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
rgn,rgn,_tmpA14->tvs,_tmpA15);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpA14->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(
Cyc_Tcutil_rsubstitute(rgn,_tmpA18,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmpA19=0;_npop_handler(0);return _tmpA19;}}}{int _tmpA1A=1;_npop_handler(0);
return _tmpA1A;}};_pop_region(rgn);}_LL659: if(*((int*)_tmpA0A)!= 11)goto _LL65B;
_tmpA16=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpA0A)->f2;_LL65A: for(0;_tmpA16
!= 0;_tmpA16=_tmpA16->tl){if(!Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Aggrfield*)
_tmpA16->hd)->type))return 0;}return 1;_LL65B:;_LL65C: return 0;_LL644:;}struct
_tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmpA1B=(void*)e->r;
struct _tuple1*_tmpA1C;void*_tmpA1D;struct Cyc_Absyn_Exp*_tmpA1E;struct Cyc_Absyn_Exp*
_tmpA1F;struct Cyc_Absyn_Exp*_tmpA20;struct Cyc_Absyn_Exp*_tmpA21;struct Cyc_Absyn_Exp*
_tmpA22;struct Cyc_Absyn_Exp*_tmpA23;struct Cyc_Absyn_Exp*_tmpA24;void*_tmpA25;
struct Cyc_Absyn_Exp*_tmpA26;void*_tmpA27;void*_tmpA28;struct Cyc_Absyn_Exp*
_tmpA29;struct Cyc_Absyn_Exp*_tmpA2A;struct Cyc_Absyn_Exp*_tmpA2B;struct Cyc_Absyn_Exp*
_tmpA2C;struct Cyc_List_List*_tmpA2D;struct Cyc_List_List*_tmpA2E;struct Cyc_List_List*
_tmpA2F;void*_tmpA30;struct Cyc_List_List*_tmpA31;struct Cyc_List_List*_tmpA32;
struct Cyc_List_List*_tmpA33;_LL65E: if(*((int*)_tmpA1B)!= 0)goto _LL660;_LL65F:
goto _LL661;_LL660: if(*((int*)_tmpA1B)!= 18)goto _LL662;_LL661: goto _LL663;_LL662:
if(*((int*)_tmpA1B)!= 19)goto _LL664;_LL663: goto _LL665;_LL664: if(*((int*)_tmpA1B)
!= 20)goto _LL666;_LL665: goto _LL667;_LL666: if(*((int*)_tmpA1B)!= 21)goto _LL668;
_LL667: goto _LL669;_LL668: if(*((int*)_tmpA1B)!= 33)goto _LL66A;_LL669: goto _LL66B;
_LL66A: if(*((int*)_tmpA1B)!= 34)goto _LL66C;_LL66B: return 1;_LL66C: if(*((int*)
_tmpA1B)!= 1)goto _LL66E;_tmpA1C=((struct Cyc_Absyn_Var_e_struct*)_tmpA1B)->f1;
_tmpA1D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA1B)->f2;_LL66D: {void*
_tmpA34=_tmpA1D;struct Cyc_Absyn_Vardecl*_tmpA35;_LL68D: if(_tmpA34 <= (void*)1)
goto _LL691;if(*((int*)_tmpA34)!= 1)goto _LL68F;_LL68E: return 1;_LL68F: if(*((int*)
_tmpA34)!= 0)goto _LL691;_tmpA35=((struct Cyc_Absyn_Global_b_struct*)_tmpA34)->f1;
_LL690: {void*_tmpA36=Cyc_Tcutil_compress((void*)_tmpA35->type);_LL696: if(
_tmpA36 <= (void*)4)goto _LL69A;if(*((int*)_tmpA36)!= 7)goto _LL698;_LL697: goto
_LL699;_LL698: if(*((int*)_tmpA36)!= 8)goto _LL69A;_LL699: return 1;_LL69A:;_LL69B:
return var_okay;_LL695:;}_LL691: if((int)_tmpA34 != 0)goto _LL693;_LL692: return 0;
_LL693:;_LL694: return var_okay;_LL68C:;}_LL66E: if(*((int*)_tmpA1B)!= 6)goto _LL670;
_tmpA1E=((struct Cyc_Absyn_Conditional_e_struct*)_tmpA1B)->f1;_tmpA1F=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpA1B)->f2;_tmpA20=((struct Cyc_Absyn_Conditional_e_struct*)_tmpA1B)->f3;_LL66F:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpA1E) && Cyc_Tcutil_cnst_exp(te,0,_tmpA1F))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpA20);_LL670: if(*((int*)_tmpA1B)!= 9)goto _LL672;
_tmpA21=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpA1B)->f1;_tmpA22=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpA1B)->f2;_LL671: return Cyc_Tcutil_cnst_exp(te,0,_tmpA21) && Cyc_Tcutil_cnst_exp(
te,0,_tmpA22);_LL672: if(*((int*)_tmpA1B)!= 13)goto _LL674;_tmpA23=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpA1B)->f1;_LL673: _tmpA24=_tmpA23;goto _LL675;_LL674: if(*((int*)_tmpA1B)!= 14)
goto _LL676;_tmpA24=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpA1B)->f1;_LL675:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpA24);_LL676: if(*((int*)_tmpA1B)!= 15)
goto _LL678;_tmpA25=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpA1B)->f1;_tmpA26=((
struct Cyc_Absyn_Cast_e_struct*)_tmpA1B)->f2;_tmpA27=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmpA1B)->f4;if((int)_tmpA27 != 1)goto _LL678;_LL677: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpA26);_LL678: if(*((int*)_tmpA1B)!= 15)goto _LL67A;_tmpA28=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmpA1B)->f1;_tmpA29=((struct Cyc_Absyn_Cast_e_struct*)
_tmpA1B)->f2;_LL679: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpA29);_LL67A: if(*((
int*)_tmpA1B)!= 16)goto _LL67C;_tmpA2A=((struct Cyc_Absyn_Address_e_struct*)
_tmpA1B)->f1;_LL67B: return Cyc_Tcutil_cnst_exp(te,1,_tmpA2A);_LL67C: if(*((int*)
_tmpA1B)!= 29)goto _LL67E;_tmpA2B=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpA1B)->f2;_tmpA2C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpA1B)->f3;
_LL67D: return Cyc_Tcutil_cnst_exp(te,0,_tmpA2B) && Cyc_Tcutil_cnst_exp(te,0,
_tmpA2C);_LL67E: if(*((int*)_tmpA1B)!= 28)goto _LL680;_tmpA2D=((struct Cyc_Absyn_Array_e_struct*)
_tmpA1B)->f1;_LL67F: _tmpA2E=_tmpA2D;goto _LL681;_LL680: if(*((int*)_tmpA1B)!= 31)
goto _LL682;_tmpA2E=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpA1B)->f2;_LL681:
_tmpA2F=_tmpA2E;goto _LL683;_LL682: if(*((int*)_tmpA1B)!= 30)goto _LL684;_tmpA2F=((
struct Cyc_Absyn_Struct_e_struct*)_tmpA1B)->f3;_LL683: for(0;_tmpA2F != 0;_tmpA2F=
_tmpA2F->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple21*)_tmpA2F->hd)).f2))
return 0;}return 1;_LL684: if(*((int*)_tmpA1B)!= 3)goto _LL686;_tmpA30=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpA1B)->f1;_tmpA31=((struct Cyc_Absyn_Primop_e_struct*)
_tmpA1B)->f2;_LL685: _tmpA32=_tmpA31;goto _LL687;_LL686: if(*((int*)_tmpA1B)!= 26)
goto _LL688;_tmpA32=((struct Cyc_Absyn_Tuple_e_struct*)_tmpA1B)->f1;_LL687: _tmpA33=
_tmpA32;goto _LL689;_LL688: if(*((int*)_tmpA1B)!= 32)goto _LL68A;_tmpA33=((struct
Cyc_Absyn_Tunion_e_struct*)_tmpA1B)->f1;_LL689: for(0;_tmpA33 != 0;_tmpA33=_tmpA33->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmpA33->hd))return 0;}return 1;
_LL68A:;_LL68B: return 0;_LL65D:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmpA37=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpA38;
struct Cyc_Absyn_PtrAtts _tmpA39;struct Cyc_Absyn_Conref*_tmpA3A;struct Cyc_Absyn_Conref*
_tmpA3B;struct Cyc_Absyn_ArrayInfo _tmpA3C;void*_tmpA3D;struct Cyc_List_List*
_tmpA3E;struct Cyc_Absyn_AggrInfo _tmpA3F;void*_tmpA40;struct Cyc_List_List*_tmpA41;
struct Cyc_List_List*_tmpA42;_LL69D: if((int)_tmpA37 != 0)goto _LL69F;_LL69E: goto
_LL6A0;_LL69F: if(_tmpA37 <= (void*)4)goto _LL6A1;if(*((int*)_tmpA37)!= 5)goto
_LL6A1;_LL6A0: goto _LL6A2;_LL6A1: if((int)_tmpA37 != 1)goto _LL6A3;_LL6A2: goto _LL6A4;
_LL6A3: if(_tmpA37 <= (void*)4)goto _LL6B3;if(*((int*)_tmpA37)!= 6)goto _LL6A5;
_LL6A4: return 1;_LL6A5: if(*((int*)_tmpA37)!= 4)goto _LL6A7;_tmpA38=((struct Cyc_Absyn_PointerType_struct*)
_tmpA37)->f1;_tmpA39=_tmpA38.ptr_atts;_tmpA3A=_tmpA39.nullable;_tmpA3B=_tmpA39.bounds;
_LL6A6: {void*_tmpA43=Cyc_Absyn_conref_def((void*)((void*)0),_tmpA3B);_LL6B6: if((
int)_tmpA43 != 0)goto _LL6B8;_LL6B7: return 1;_LL6B8: if((int)_tmpA43 != 1)goto _LL6BA;
_LL6B9: return 1;_LL6BA:;_LL6BB: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmpA3A);_LL6B5:;}_LL6A7: if(*((int*)_tmpA37)!= 7)goto _LL6A9;_tmpA3C=((struct
Cyc_Absyn_ArrayType_struct*)_tmpA37)->f1;_tmpA3D=(void*)_tmpA3C.elt_type;_LL6A8:
return Cyc_Tcutil_supports_default(_tmpA3D);_LL6A9: if(*((int*)_tmpA37)!= 9)goto
_LL6AB;_tmpA3E=((struct Cyc_Absyn_TupleType_struct*)_tmpA37)->f1;_LL6AA: for(0;
_tmpA3E != 0;_tmpA3E=_tmpA3E->tl){if(!Cyc_Tcutil_supports_default((*((struct
_tuple4*)_tmpA3E->hd)).f2))return 0;}return 1;_LL6AB: if(*((int*)_tmpA37)!= 10)goto
_LL6AD;_tmpA3F=((struct Cyc_Absyn_AggrType_struct*)_tmpA37)->f1;_tmpA40=(void*)
_tmpA3F.aggr_info;_tmpA41=_tmpA3F.targs;_LL6AC: {struct Cyc_Absyn_Aggrdecl*
_tmpA44=Cyc_Absyn_get_known_aggrdecl(_tmpA40);if(_tmpA44->impl == 0)return 0;if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA44->impl))->exist_vars != 0)return
0;return Cyc_Tcutil_fields_support_default(_tmpA44->tvs,_tmpA41,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpA44->impl))->fields);}_LL6AD: if(*((int*)_tmpA37)!= 11)goto _LL6AF;
_tmpA42=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpA37)->f2;_LL6AE: return Cyc_Tcutil_fields_support_default(
0,0,_tmpA42);_LL6AF: if(*((int*)_tmpA37)!= 13)goto _LL6B1;_LL6B0: goto _LL6B2;_LL6B1:
if(*((int*)_tmpA37)!= 12)goto _LL6B3;_LL6B2: return 1;_LL6B3:;_LL6B4: return 0;_LL69C:;}
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*
ts,struct Cyc_List_List*fs){{struct _RegionHandle _tmpA45=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpA45;_push_region(rgn);{struct Cyc_List_List*_tmpA46=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpA46,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpA47=0;_npop_handler(0);return _tmpA47;}}};
_pop_region(rgn);}return 1;}int Cyc_Tcutil_admits_zero(void*t){void*_tmpA48=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpA49;struct Cyc_Absyn_PtrAtts _tmpA4A;struct Cyc_Absyn_Conref*
_tmpA4B;struct Cyc_Absyn_Conref*_tmpA4C;_LL6BD: if(_tmpA48 <= (void*)4)goto _LL6BF;
if(*((int*)_tmpA48)!= 5)goto _LL6BF;_LL6BE: goto _LL6C0;_LL6BF: if((int)_tmpA48 != 1)
goto _LL6C1;_LL6C0: goto _LL6C2;_LL6C1: if(_tmpA48 <= (void*)4)goto _LL6C5;if(*((int*)
_tmpA48)!= 6)goto _LL6C3;_LL6C2: return 1;_LL6C3: if(*((int*)_tmpA48)!= 4)goto _LL6C5;
_tmpA49=((struct Cyc_Absyn_PointerType_struct*)_tmpA48)->f1;_tmpA4A=_tmpA49.ptr_atts;
_tmpA4B=_tmpA4A.nullable;_tmpA4C=_tmpA4A.bounds;_LL6C4: {void*_tmpA4D=(void*)(
Cyc_Absyn_compress_conref(_tmpA4C))->v;void*_tmpA4E;void*_tmpA4F;_LL6C8: if(
_tmpA4D <= (void*)1)goto _LL6CE;if(*((int*)_tmpA4D)!= 0)goto _LL6CA;_tmpA4E=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpA4D)->f1;if((int)_tmpA4E != 0)goto _LL6CA;
_LL6C9: return 0;_LL6CA: if(*((int*)_tmpA4D)!= 0)goto _LL6CC;_tmpA4F=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmpA4D)->f1;if((int)_tmpA4F != 1)goto _LL6CC;_LL6CB:
return 0;_LL6CC: if(*((int*)_tmpA4D)!= 0)goto _LL6CE;_LL6CD: {void*_tmpA50=(void*)(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmpA4B))->v;int _tmpA51;_LL6D1: if(_tmpA50 <= (void*)1)goto _LL6D3;if(*((int*)
_tmpA50)!= 0)goto _LL6D3;_tmpA51=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmpA50)->f1;
_LL6D2: return _tmpA51;_LL6D3:;_LL6D4: return 0;_LL6D0:;}_LL6CE:;_LL6CF: return 0;
_LL6C7:;}_LL6C5:;_LL6C6: return 0;_LL6BC:;}int Cyc_Tcutil_is_noreturn(void*t){{void*
_tmpA52=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpA53;void*_tmpA54;
struct Cyc_Absyn_FnInfo _tmpA55;struct Cyc_List_List*_tmpA56;_LL6D6: if(_tmpA52 <= (
void*)4)goto _LL6DA;if(*((int*)_tmpA52)!= 4)goto _LL6D8;_tmpA53=((struct Cyc_Absyn_PointerType_struct*)
_tmpA52)->f1;_tmpA54=(void*)_tmpA53.elt_typ;_LL6D7: return Cyc_Tcutil_is_noreturn(
_tmpA54);_LL6D8: if(*((int*)_tmpA52)!= 8)goto _LL6DA;_tmpA55=((struct Cyc_Absyn_FnType_struct*)
_tmpA52)->f1;_tmpA56=_tmpA55.attributes;_LL6D9: for(0;_tmpA56 != 0;_tmpA56=_tmpA56->tl){
void*_tmpA57=(void*)_tmpA56->hd;_LL6DD: if((int)_tmpA57 != 3)goto _LL6DF;_LL6DE:
return 1;_LL6DF:;_LL6E0: continue;_LL6DC:;}goto _LL6D5;_LL6DA:;_LL6DB: goto _LL6D5;
_LL6D5:;}return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,
struct Cyc_List_List*atts){void*_tmpA58=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmpA59;struct Cyc_List_List*_tmpA5A;struct Cyc_List_List**_tmpA5B;_LL6E2: if(
_tmpA58 <= (void*)4)goto _LL6E4;if(*((int*)_tmpA58)!= 8)goto _LL6E4;_tmpA59=((
struct Cyc_Absyn_FnType_struct*)_tmpA58)->f1;_tmpA5A=_tmpA59.attributes;_tmpA5B=(
struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmpA58)->f1).attributes;
_LL6E3: {struct Cyc_List_List*_tmpA5C=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((
void*)atts->hd))*_tmpA5B=({struct Cyc_List_List*_tmpA5D=_cycalloc(sizeof(*_tmpA5D));
_tmpA5D->hd=(void*)((void*)atts->hd);_tmpA5D->tl=*_tmpA5B;_tmpA5D;});else{
_tmpA5C=({struct Cyc_List_List*_tmpA5E=_cycalloc(sizeof(*_tmpA5E));_tmpA5E->hd=(
void*)((void*)atts->hd);_tmpA5E->tl=_tmpA5C;_tmpA5E;});}}return _tmpA5C;}_LL6E4:;
_LL6E5:({void*_tmpA5F[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA60="transfer_fn_type_atts";
_tag_dynforward(_tmpA60,sizeof(char),_get_zero_arr_size(_tmpA60,22));}),
_tag_dynforward(_tmpA5F,sizeof(void*),0));});_LL6E1:;}
