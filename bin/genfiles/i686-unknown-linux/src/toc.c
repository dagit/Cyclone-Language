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
struct _dynforward_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct
Cyc___cycFILE*,struct _dynforward_ptr,struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dynforward_ptr f1;}
;struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,struct _dynforward_ptr);
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*
x);void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rrev(
struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*
x,int n);int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);struct
_dynforward_ptr Cyc_strconcat(struct _dynforward_ptr,struct _dynforward_ptr);struct
Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct
Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(
void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);int
Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];struct
Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict*
Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict*d,void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*
d,void*k);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Lineno_Pos{struct _dynforward_ptr
logical_file;struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct _dynforward_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dynforward_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};
extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{int tag;};
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple1*,struct _tuple1*);struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*
x);struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern
void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_star_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dynforward_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct
_dynforward_ptr*name);void*Cyc_Absyn_strctq(struct _tuple1*name);void*Cyc_Absyn_unionq_typ(
struct _tuple1*name);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual
tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dynforward_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dynforward_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple1*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(
struct _tuple1*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,
struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct
Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dynforward_ptr*lab,struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dynforward_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dynforward_ptr*);struct _dynforward_ptr*Cyc_Absyn_fieldname(
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dynforward_ptr
Cyc_Absynpp_typ2string(void*);struct _dynforward_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dynforward_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
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
le;};void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);
struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*
rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*,int*is_forward_only);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest,int*is_forward_only);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;};void*
Cyc_Tcutil_snd_tqt(struct _tuple4*);struct _tuple5{unsigned int f1;int f2;};struct
_tuple5 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{
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
f1;};struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();
int Cyc_Toc_warn_bounds_checks=0;int Cyc_Toc_warn_all_null_deref=0;unsigned int Cyc_Toc_total_bounds_checks=
0;unsigned int Cyc_Toc_bounds_checks_eliminated=0;static struct Cyc_List_List*Cyc_Toc_result_decls=
0;static struct Cyc_List_List*Cyc_Toc_tuple_types=0;static int Cyc_Toc_tuple_type_counter=
0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=0;
char Cyc_Toc_Toc_Unimplemented[22]="\000\000\000\000Toc_Unimplemented\000";char
Cyc_Toc_Toc_Impossible[19]="\000\000\000\000Toc_Impossible\000";static void*Cyc_Toc_unimp(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({
void*_tmp0[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="\n";_tag_dynforward(
_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,2));}),_tag_dynforward(_tmp0,sizeof(
void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)
Cyc_Toc_Toc_Unimplemented);}static void*Cyc_Toc_toc_impos(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({void*_tmp2[0]={};
Cyc_fprintf(Cyc_stderr,({const char*_tmp3="\n";_tag_dynforward(_tmp3,sizeof(char),
_get_zero_arr_size(_tmp3,2));}),_tag_dynforward(_tmp2,sizeof(void*),0));});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)Cyc_Toc_Toc_Impossible);}
char Cyc_Toc_Match_error[16]="\000\000\000\000Match_error\000";static char _tmp4[5]="curr";
static struct _dynforward_ptr Cyc_Toc_curr_string={_tmp4,_tmp4 + 5};static struct
_dynforward_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp5[4]="tag";
static struct _dynforward_ptr Cyc_Toc_tag_string={_tmp5,_tmp5 + 4};static struct
_dynforward_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp6[4]="val";
static struct _dynforward_ptr Cyc_Toc_val_string={_tmp6,_tmp6 + 4};static struct
_dynforward_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp7[14]="_handler_cons";
static struct _dynforward_ptr Cyc_Toc__handler_cons_string={_tmp7,_tmp7 + 14};static
struct _dynforward_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;
static char _tmp8[8]="handler";static struct _dynforward_ptr Cyc_Toc_handler_string={
_tmp8,_tmp8 + 8};static struct _dynforward_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;
static char _tmp9[14]="_RegionHandle";static struct _dynforward_ptr Cyc_Toc__RegionHandle_string={
_tmp9,_tmp9 + 14};static struct _dynforward_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static char _tmpA[7]="_throw";static struct _dynforward_ptr Cyc_Toc__throw_str={_tmpA,
_tmpA + 7};static struct _tuple1 Cyc_Toc__throw_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__throw_ev={0,(void*)((void*)& Cyc_Toc__throw_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmpD[7]="setjmp";
static struct _dynforward_ptr Cyc_Toc_setjmp_str={_tmpD,_tmpD + 7};static struct
_tuple1 Cyc_Toc_setjmp_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={1,&
Cyc_Toc_setjmp_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={
0,(void*)((void*)& Cyc_Toc_setjmp_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp10[
14]="_push_handler";static struct _dynforward_ptr Cyc_Toc__push_handler_str={_tmp10,
_tmp10 + 14};static struct _tuple1 Cyc_Toc__push_handler_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)((void*)& Cyc_Toc__push_handler_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=&
Cyc_Toc__push_handler_ev;static char _tmp13[13]="_pop_handler";static struct
_dynforward_ptr Cyc_Toc__pop_handler_str={_tmp13,_tmp13 + 13};static struct _tuple1
Cyc_Toc__pop_handler_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)((void*)& Cyc_Toc__pop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static
char _tmp16[12]="_exn_thrown";static struct _dynforward_ptr Cyc_Toc__exn_thrown_str={
_tmp16,_tmp16 + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)((void*)& Cyc_Toc__exn_thrown_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=&
Cyc_Toc__exn_thrown_ev;static char _tmp19[14]="_npop_handler";static struct
_dynforward_ptr Cyc_Toc__npop_handler_str={_tmp19,_tmp19 + 14};static struct _tuple1
Cyc_Toc__npop_handler_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={
1,& Cyc_Toc__npop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={
0,(void*)((void*)& Cyc_Toc__npop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;
static char _tmp1C[12]="_check_null";static struct _dynforward_ptr Cyc_Toc__check_null_str={
_tmp1C,_tmp1C + 12};static struct _tuple1 Cyc_Toc__check_null_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)((void*)& Cyc_Toc__check_null_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev;static char _tmp1F[28]="_check_known_subscript_null";static
struct _dynforward_ptr Cyc_Toc__check_known_subscript_null_str={_tmp1F,_tmp1F + 28};
static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_known_subscript_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp22[31]="_check_known_subscript_notnull";static struct
_dynforward_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp22,_tmp22 + 31};
static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)((void*)& Cyc_Toc__check_known_subscript_notnull_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp25[28]="_check_dynforward_subscript";
static struct _dynforward_ptr Cyc_Toc__check_dynforward_subscript_str={_tmp25,
_tmp25 + 28};static struct _tuple1 Cyc_Toc__check_dynforward_subscript_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_dynforward_subscript_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dynforward_subscript_re={1,& Cyc_Toc__check_dynforward_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dynforward_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dynforward_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dynforward_subscript_e=& Cyc_Toc__check_dynforward_subscript_ev;
static char _tmp28[27]="_check_dyneither_subscript";static struct _dynforward_ptr Cyc_Toc__check_dyneither_subscript_str={
_tmp28,_tmp28 + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_dyneither_subscript_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dyneither_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;
static char _tmp2B[16]="_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_str={
_tmp2B,_tmp2B + 16};static struct _tuple1 Cyc_Toc__dynforward_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dynforward_ptr_re={1,& Cyc_Toc__dynforward_ptr_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_e=&
Cyc_Toc__dynforward_ptr_ev;static char _tmp2E[15]="_dyneither_ptr";static struct
_dynforward_ptr Cyc_Toc__dyneither_ptr_str={_tmp2E,_tmp2E + 15};static struct
_tuple1 Cyc_Toc__dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_re={
1,& Cyc_Toc__dyneither_ptr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;
static char _tmp31[16]="_tag_dynforward";static struct _dynforward_ptr Cyc_Toc__tag_dynforward_str={
_tmp31,_tmp31 + 16};static struct _tuple1 Cyc_Toc__tag_dynforward_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__tag_dynforward_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tag_dynforward_re={1,& Cyc_Toc__tag_dynforward_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__tag_dynforward_ev={0,(void*)((void*)& Cyc_Toc__tag_dynforward_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dynforward_e=&
Cyc_Toc__tag_dynforward_ev;static char _tmp34[15]="_tag_dyneither";static struct
_dynforward_ptr Cyc_Toc__tag_dyneither_str={_tmp34,_tmp34 + 15};static struct
_tuple1 Cyc_Toc__tag_dyneither_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dyneither_re={
1,& Cyc_Toc__tag_dyneither_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={
0,(void*)((void*)& Cyc_Toc__tag_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;
static char _tmp37[21]="_init_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__init_dynforward_ptr_str={
_tmp37,_tmp37 + 21};static struct _tuple1 Cyc_Toc__init_dynforward_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__init_dynforward_ptr_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_dynforward_ptr_re={1,& Cyc_Toc__init_dynforward_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dynforward_ptr_ev={0,(
void*)((void*)& Cyc_Toc__init_dynforward_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__init_dynforward_ptr_e=& Cyc_Toc__init_dynforward_ptr_ev;
static char _tmp3A[20]="_init_dyneither_ptr";static struct _dynforward_ptr Cyc_Toc__init_dyneither_ptr_str={
_tmp3A,_tmp3A + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__init_dyneither_ptr_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__init_dyneither_ptr_re={1,& Cyc_Toc__init_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(
void*)((void*)& Cyc_Toc__init_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;
static char _tmp3D[22]="_untag_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__untag_dynforward_ptr_str={
_tmp3D,_tmp3D + 22};static struct _tuple1 Cyc_Toc__untag_dynforward_ptr_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__untag_dynforward_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dynforward_ptr_re={1,& Cyc_Toc__untag_dynforward_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dynforward_ptr_ev={0,(
void*)((void*)& Cyc_Toc__untag_dynforward_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dynforward_ptr_e=& Cyc_Toc__untag_dynforward_ptr_ev;
static char _tmp40[21]="_untag_dyneither_ptr";static struct _dynforward_ptr Cyc_Toc__untag_dyneither_ptr_str={
_tmp40,_tmp40 + 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__untag_dyneither_ptr_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(
void*)((void*)& Cyc_Toc__untag_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;
static char _tmp43[21]="_get_dynforward_size";static struct _dynforward_ptr Cyc_Toc__get_dynforward_size_str={
_tmp43,_tmp43 + 21};static struct _tuple1 Cyc_Toc__get_dynforward_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_dynforward_size_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_dynforward_size_re={1,& Cyc_Toc__get_dynforward_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dynforward_size_ev={0,(
void*)((void*)& Cyc_Toc__get_dynforward_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_dynforward_size_e=& Cyc_Toc__get_dynforward_size_ev;
static char _tmp46[20]="_get_dyneither_size";static struct _dynforward_ptr Cyc_Toc__get_dyneither_size_str={
_tmp46,_tmp46 + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_dyneither_size_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__get_dyneither_size_re={1,& Cyc_Toc__get_dyneither_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(
void*)((void*)& Cyc_Toc__get_dyneither_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;
static char _tmp49[19]="_get_zero_arr_size";static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_str={
_tmp49,_tmp49 + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;
static char _tmp4C[21]="_dynforward_ptr_plus";static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_plus_str={
_tmp4C,_tmp4C + 21};static struct _tuple1 Cyc_Toc__dynforward_ptr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_plus_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_plus_re={1,& Cyc_Toc__dynforward_ptr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_plus_ev={0,(
void*)((void*)& Cyc_Toc__dynforward_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_plus_e=& Cyc_Toc__dynforward_ptr_plus_ev;
static char _tmp4F[20]="_dyneither_ptr_plus";static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_plus_str={
_tmp4F,_tmp4F + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_plus_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(
void*)((void*)& Cyc_Toc__dyneither_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;
static char _tmp52[15]="_zero_arr_plus";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_str={
_tmp52,_tmp52 + 15};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=&
Cyc_Toc__zero_arr_plus_ev;static char _tmp55[29]="_dynforward_ptr_inplace_plus";
static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_inplace_plus_str={_tmp55,
_tmp55 + 29};static struct _tuple1 Cyc_Toc__dynforward_ptr_inplace_plus_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_inplace_plus_re={1,&
Cyc_Toc__dynforward_ptr_inplace_plus_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dynforward_ptr_inplace_plus_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_inplace_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_inplace_plus_e=&
Cyc_Toc__dynforward_ptr_inplace_plus_ev;static char _tmp58[28]="_dyneither_ptr_inplace_plus";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp58,
_tmp58 + 28};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;
static char _tmp5B[23]="_zero_arr_inplace_plus";static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp5B,_tmp5B + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;
static char _tmp5E[34]="_dynforward_ptr_inplace_plus_post";static struct
_dynforward_ptr Cyc_Toc__dynforward_ptr_inplace_plus_post_str={_tmp5E,_tmp5E + 34};
static struct _tuple1 Cyc_Toc__dynforward_ptr_inplace_plus_post_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_inplace_plus_post_re={
1,& Cyc_Toc__dynforward_ptr_inplace_plus_post_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_inplace_plus_post_e=&
Cyc_Toc__dynforward_ptr_inplace_plus_post_ev;static char _tmp61[33]="_dyneither_ptr_inplace_plus_post";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp61,
_tmp61 + 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,&
Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=&
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp64[25]="_dynforward_to_dyneither";
static struct _dynforward_ptr Cyc_Toc__dynforward_to_dyneither_str={_tmp64,_tmp64 + 
25};static struct _tuple1 Cyc_Toc__dynforward_to_dyneither_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_to_dyneither_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_to_dyneither_re={1,& Cyc_Toc__dynforward_to_dyneither_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_to_dyneither_ev={
0,(void*)((void*)& Cyc_Toc__dynforward_to_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_to_dyneither_e=& Cyc_Toc__dynforward_to_dyneither_ev;
static char _tmp67[25]="_dyneither_to_dynforward";static struct _dynforward_ptr Cyc_Toc__dyneither_to_dynforward_str={
_tmp67,_tmp67 + 25};static struct _tuple1 Cyc_Toc__dyneither_to_dynforward_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_to_dynforward_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_to_dynforward_re={1,& Cyc_Toc__dyneither_to_dynforward_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_to_dynforward_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_to_dynforward_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_to_dynforward_e=& Cyc_Toc__dyneither_to_dynforward_ev;
static char _tmp6A[28]="_zero_arr_inplace_plus_post";static struct _dynforward_ptr
Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp6A,_tmp6A + 28};static struct _tuple1
Cyc_Toc__zero_arr_inplace_plus_post_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp6D[10]="_cycalloc";static struct _dynforward_ptr Cyc_Toc__cycalloc_str={
_tmp6D,_tmp6D + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_re={1,& Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__cycalloc_ev={0,(void*)((void*)& Cyc_Toc__cycalloc_re),0,(void*)((void*)
Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;
static char _tmp70[11]="_cyccalloc";static struct _dynforward_ptr Cyc_Toc__cyccalloc_str={
_tmp70,_tmp70 + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)((void*)& Cyc_Toc__cyccalloc_re),0,(
void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=&
Cyc_Toc__cyccalloc_ev;static char _tmp73[17]="_cycalloc_atomic";static struct
_dynforward_ptr Cyc_Toc__cycalloc_atomic_str={_tmp73,_tmp73 + 17};static struct
_tuple1 Cyc_Toc__cycalloc_atomic_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_atomic_re={
1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={
0,(void*)((void*)& Cyc_Toc__cycalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;
static char _tmp76[18]="_cyccalloc_atomic";static struct _dynforward_ptr Cyc_Toc__cyccalloc_atomic_str={
_tmp76,_tmp76 + 18};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cyccalloc_atomic_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={1,& Cyc_Toc__cyccalloc_atomic_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)((
void*)& Cyc_Toc__cyccalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;
static char _tmp79[15]="_region_malloc";static struct _dynforward_ptr Cyc_Toc__region_malloc_str={
_tmp79,_tmp79 + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)((void*)& Cyc_Toc__region_malloc_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=&
Cyc_Toc__region_malloc_ev;static char _tmp7C[15]="_region_calloc";static struct
_dynforward_ptr Cyc_Toc__region_calloc_str={_tmp7C,_tmp7C + 15};static struct
_tuple1 Cyc_Toc__region_calloc_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={
1,& Cyc_Toc__region_calloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={
0,(void*)((void*)& Cyc_Toc__region_calloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;
static char _tmp7F[13]="_check_times";static struct _dynforward_ptr Cyc_Toc__check_times_str={
_tmp7F,_tmp7F + 13};static struct _tuple1 Cyc_Toc__check_times_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)((void*)& Cyc_Toc__check_times_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev;static char _tmp82[12]="_new_region";static struct
_dynforward_ptr Cyc_Toc__new_region_str={_tmp82,_tmp82 + 12};static struct _tuple1
Cyc_Toc__new_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1,& Cyc_Toc__new_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={
0,(void*)((void*)& Cyc_Toc__new_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static
char _tmp85[13]="_push_region";static struct _dynforward_ptr Cyc_Toc__push_region_str={
_tmp85,_tmp85 + 13};static struct _tuple1 Cyc_Toc__push_region_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)((void*)& Cyc_Toc__push_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev;static char _tmp88[12]="_pop_region";static struct
_dynforward_ptr Cyc_Toc__pop_region_str={_tmp88,_tmp88 + 12};static struct _tuple1
Cyc_Toc__pop_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1,& Cyc_Toc__pop_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={
0,(void*)((void*)& Cyc_Toc__pop_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static
char _tmp8B[14]="_reset_region";static struct _dynforward_ptr Cyc_Toc__reset_region_str={
_tmp8B,_tmp8B + 14};static struct _tuple1 Cyc_Toc__reset_region_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__reset_region_re={1,& Cyc_Toc__reset_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)((void*)& Cyc_Toc__reset_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=&
Cyc_Toc__reset_region_ev;static char _tmp8E[19]="_throw_arraybounds";static struct
_dynforward_ptr Cyc_Toc__throw_arraybounds_str={_tmp8E,_tmp8E + 19};static struct
_tuple1 Cyc_Toc__throw_arraybounds_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={
1,& Cyc_Toc__throw_arraybounds_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={
0,(void*)((void*)& Cyc_Toc__throw_arraybounds_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmp91[30]="_dynforward_ptr_decrease_size";static struct _dynforward_ptr
Cyc_Toc__dynforward_ptr_decrease_size_str={_tmp91,_tmp91 + 30};static struct
_tuple1 Cyc_Toc__dynforward_ptr_decrease_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_decrease_size_re={1,&
Cyc_Toc__dynforward_ptr_decrease_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dynforward_ptr_decrease_size_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_decrease_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_decrease_size_e=&
Cyc_Toc__dynforward_ptr_decrease_size_ev;static char _tmp94[29]="_dyneither_ptr_decrease_size";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmp94,
_tmp94 + 29};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,&
Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_decrease_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=&
Cyc_Toc__dyneither_ptr_decrease_size_ev;static struct Cyc_Absyn_UnknownAggr_struct
Cyc_Toc_dynforward_ptr_typ_u={0,(void*)((void*)0),& Cyc_Toc__dynforward_ptr_pr};
static struct Cyc_Absyn_AggrType_struct Cyc_Toc_dynforward_ptr_typ_v={10,{(void*)((
void*)& Cyc_Toc_dynforward_ptr_typ_u),0}};static void*Cyc_Toc_dynforward_ptr_typ=(
void*)& Cyc_Toc_dynforward_ptr_typ_v;static struct Cyc_Absyn_UnknownAggr_struct Cyc_Toc_dyneither_ptr_typ_u={
0,(void*)((void*)0),& Cyc_Toc__dyneither_ptr_pr};static struct Cyc_Absyn_AggrType_struct
Cyc_Toc_dyneither_ptr_typ_v={10,{(void*)((void*)& Cyc_Toc_dyneither_ptr_typ_u),0}};
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;static
struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(skip_stmt_opt == 0)skip_stmt_opt=({
struct Cyc_Absyn_Stmt**_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B[0]=Cyc_Absyn_skip_stmt(
0);_tmp9B;});return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(
void*t,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(t,e,0,(void*)1,0);}static
void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Cast_e_struct*
_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C[0]=({struct Cyc_Absyn_Cast_e_struct
_tmp9D;_tmp9D.tag=15;_tmp9D.f1=(void*)t;_tmp9D.f2=e;_tmp9D.f3=0;_tmp9D.f4=(void*)((
void*)1);_tmp9D;});_tmp9C;});}static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*
e){return(void*)({struct Cyc_Absyn_Deref_e_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));
_tmp9E[0]=({struct Cyc_Absyn_Deref_e_struct _tmp9F;_tmp9F.tag=22;_tmp9F.f1=e;
_tmp9F;});_tmp9E;});}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){return(void*)({struct Cyc_Absyn_Subscript_e_struct*_tmpA0=
_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=({struct Cyc_Absyn_Subscript_e_struct _tmpA1;
_tmpA1.tag=25;_tmpA1.f1=e1;_tmpA1.f2=e2;_tmpA1;});_tmpA0;});}static void*Cyc_Toc_stmt_exp_r(
struct Cyc_Absyn_Stmt*s){return(void*)({struct Cyc_Absyn_StmtExp_e_struct*_tmpA2=
_cycalloc(sizeof(*_tmpA2));_tmpA2[0]=({struct Cyc_Absyn_StmtExp_e_struct _tmpA3;
_tmpA3.tag=37;_tmpA3.f1=s;_tmpA3;});_tmpA2;});}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t){return(void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpA4=_cycalloc(
sizeof(*_tmpA4));_tmpA4[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct _tmpA5;_tmpA5.tag=
18;_tmpA5.f1=(void*)t;_tmpA5;});_tmpA4;});}static void*Cyc_Toc_fncall_exp_r(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){return(void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmpA7;_tmpA7.tag=11;_tmpA7.f1=e;_tmpA7.f2=es;_tmpA7.f3=0;_tmpA7;});_tmpA6;});}
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Exp_s_struct*
_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8[0]=({struct Cyc_Absyn_Exp_s_struct _tmpA9;
_tmpA9.tag=0;_tmpA9.f1=e;_tmpA9;});_tmpA8;});}static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){return(void*)({struct Cyc_Absyn_Seq_s_struct*
_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=({struct Cyc_Absyn_Seq_s_struct _tmpAB;
_tmpAB.tag=1;_tmpAB.f1=s1;_tmpAB.f2=s2;_tmpAB;});_tmpAA;});}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){return(
void*)({struct Cyc_Absyn_Conditional_e_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));
_tmpAC[0]=({struct Cyc_Absyn_Conditional_e_struct _tmpAD;_tmpAD.tag=6;_tmpAD.f1=e1;
_tmpAD.f2=e2;_tmpAD.f3=e3;_tmpAD;});_tmpAC;});}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*n){return(void*)({struct Cyc_Absyn_AggrMember_e_struct*
_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE[0]=({struct Cyc_Absyn_AggrMember_e_struct
_tmpAF;_tmpAF.tag=23;_tmpAF.f1=e;_tmpAF.f2=n;_tmpAF;});_tmpAE;});}static void*Cyc_Toc_aggrarrow_exp_r(
struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*n){return(void*)({struct Cyc_Absyn_AggrArrow_e_struct*
_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=({struct Cyc_Absyn_AggrArrow_e_struct
_tmpB1;_tmpB1.tag=24;_tmpB1.f1=e;_tmpB1.f2=n;_tmpB1;});_tmpB0;});}static void*Cyc_Toc_unresolvedmem_exp_r(
struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){return(void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpB3;_tmpB3.tag=36;_tmpB3.f1=tdopt;_tmpB3.f2=ds;_tmpB3;});_tmpB2;});}static
void*Cyc_Toc_goto_stmt_r(struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*s){return(
void*)({struct Cyc_Absyn_Goto_s_struct*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({
struct Cyc_Absyn_Goto_s_struct _tmpB5;_tmpB5.tag=7;_tmpB5.f1=v;_tmpB5.f2=s;_tmpB5;});
_tmpB4;});}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,(union Cyc_Absyn_Cnst_union)((
struct Cyc_Absyn_Int_c_struct){2,(void*)((void*)0),0})};struct _tuple6{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){void*
_tmpB8=(void*)e->r;union Cyc_Absyn_Cnst_union _tmpB9;char _tmpBA;union Cyc_Absyn_Cnst_union
_tmpBB;short _tmpBC;union Cyc_Absyn_Cnst_union _tmpBD;int _tmpBE;union Cyc_Absyn_Cnst_union
_tmpBF;long long _tmpC0;union Cyc_Absyn_Cnst_union _tmpC1;struct Cyc_Absyn_Exp*
_tmpC2;struct Cyc_List_List*_tmpC3;struct Cyc_List_List*_tmpC4;struct Cyc_List_List*
_tmpC5;struct Cyc_List_List*_tmpC6;struct Cyc_List_List*_tmpC7;_LL1: if(*((int*)
_tmpB8)!= 0)goto _LL3;_tmpB9=((struct Cyc_Absyn_Const_e_struct*)_tmpB8)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpB8)->f1).Char_c).tag != 0)goto _LL3;_tmpBA=(
_tmpB9.Char_c).f2;_LL2: return _tmpBA == '\000';_LL3: if(*((int*)_tmpB8)!= 0)goto
_LL5;_tmpBB=((struct Cyc_Absyn_Const_e_struct*)_tmpB8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpB8)->f1).Short_c).tag != 1)goto _LL5;_tmpBC=(_tmpBB.Short_c).f2;_LL4: return
_tmpBC == 0;_LL5: if(*((int*)_tmpB8)!= 0)goto _LL7;_tmpBD=((struct Cyc_Absyn_Const_e_struct*)
_tmpB8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpB8)->f1).Int_c).tag != 2)
goto _LL7;_tmpBE=(_tmpBD.Int_c).f2;_LL6: return _tmpBE == 0;_LL7: if(*((int*)_tmpB8)
!= 0)goto _LL9;_tmpBF=((struct Cyc_Absyn_Const_e_struct*)_tmpB8)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmpB8)->f1).LongLong_c).tag != 3)goto _LL9;_tmpC0=(
_tmpBF.LongLong_c).f2;_LL8: return _tmpC0 == 0;_LL9: if(*((int*)_tmpB8)!= 0)goto _LLB;
_tmpC1=((struct Cyc_Absyn_Const_e_struct*)_tmpB8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpB8)->f1).Null_c).tag != 6)goto _LLB;_LLA: return 1;_LLB: if(*((int*)_tmpB8)!= 15)
goto _LLD;_tmpC2=((struct Cyc_Absyn_Cast_e_struct*)_tmpB8)->f2;_LLC: return Cyc_Toc_is_zero(
_tmpC2);_LLD: if(*((int*)_tmpB8)!= 26)goto _LLF;_tmpC3=((struct Cyc_Absyn_Tuple_e_struct*)
_tmpB8)->f1;_LLE: return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpC3);_LLF: if(*((int*)_tmpB8)!= 28)goto
_LL11;_tmpC4=((struct Cyc_Absyn_Array_e_struct*)_tmpB8)->f1;_LL10: _tmpC5=_tmpC4;
goto _LL12;_LL11: if(*((int*)_tmpB8)!= 30)goto _LL13;_tmpC5=((struct Cyc_Absyn_Struct_e_struct*)
_tmpB8)->f3;_LL12: _tmpC6=_tmpC5;goto _LL14;_LL13: if(*((int*)_tmpB8)!= 27)goto
_LL15;_tmpC6=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmpB8)->f2;_LL14: _tmpC7=
_tmpC6;goto _LL16;_LL15: if(*((int*)_tmpB8)!= 36)goto _LL17;_tmpC7=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpB8)->f2;_LL16: for(0;_tmpC7 != 0;_tmpC7=_tmpC7->tl){if(!Cyc_Toc_is_zero((*((
struct _tuple6*)_tmpC7->hd)).f2))return 0;}return 1;_LL17:;_LL18: return 0;_LL0:;}
static int Cyc_Toc_is_nullable(void*t){void*_tmpC8=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmpC9;struct Cyc_Absyn_PtrAtts _tmpCA;struct Cyc_Absyn_Conref*
_tmpCB;_LL1A: if(_tmpC8 <= (void*)4)goto _LL1C;if(*((int*)_tmpC8)!= 4)goto _LL1C;
_tmpC9=((struct Cyc_Absyn_PointerType_struct*)_tmpC8)->f1;_tmpCA=_tmpC9.ptr_atts;
_tmpCB=_tmpCA.nullable;_LL1B: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpCB);_LL1C:;_LL1D:({void*_tmpCC[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmpCD="is_nullable";
_tag_dynforward(_tmpCD,sizeof(char),_get_zero_arr_size(_tmpCD,12));}),
_tag_dynforward(_tmpCC,sizeof(void*),0));});_LL19:;}static struct _tuple1*Cyc_Toc_collapse_qvar_tag(
struct _tuple1*x,struct _dynforward_ptr tag){return({struct _tuple1*_tmpCE=_cycalloc(
sizeof(*_tmpCE));_tmpCE->f1=(*x).f1;_tmpCE->f2=({struct _dynforward_ptr*_tmpCF=
_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=(struct _dynforward_ptr)Cyc_strconcat((
struct _dynforward_ptr)*(*x).f2,(struct _dynforward_ptr)tag);_tmpCF;});_tmpCE;});}
struct _tuple7{struct _dynforward_ptr*f1;struct Cyc_List_List*f2;};static struct
_dynforward_ptr*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){{struct Cyc_List_List*
_tmpD0=Cyc_Toc_tuple_types;for(0;_tmpD0 != 0;_tmpD0=_tmpD0->tl){struct _tuple7
_tmpD2;struct _dynforward_ptr*_tmpD3;struct Cyc_List_List*_tmpD4;struct _tuple7*
_tmpD1=(struct _tuple7*)_tmpD0->hd;_tmpD2=*_tmpD1;_tmpD3=_tmpD2.f1;_tmpD4=_tmpD2.f2;{
struct Cyc_List_List*_tmpD5=tqs0;for(0;_tmpD5 != 0  && _tmpD4 != 0;(_tmpD5=_tmpD5->tl,
_tmpD4=_tmpD4->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple4*)_tmpD5->hd)).f2,(
void*)_tmpD4->hd))break;}if(_tmpD5 == 0  && _tmpD4 == 0)return _tmpD3;}}}{struct
_dynforward_ptr*x=({struct _dynforward_ptr*_tmpE3=_cycalloc(sizeof(*_tmpE3));
_tmpE3[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmpE6;_tmpE6.tag=1;
_tmpE6.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;{void*_tmpE4[1]={& _tmpE6};
Cyc_aprintf(({const char*_tmpE5="_tuple%d";_tag_dynforward(_tmpE5,sizeof(char),
_get_zero_arr_size(_tmpE5,9));}),_tag_dynforward(_tmpE4,sizeof(void*),1));}});
_tmpE3;});struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_snd_tqt,tqs0);struct
Cyc_List_List*_tmpD6=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,
i ++)){_tmpD6=({struct Cyc_List_List*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7->hd=({
struct Cyc_Absyn_Aggrfield*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->name=Cyc_Absyn_fieldname(
i);_tmpD8->tq=Cyc_Toc_mt_tq;_tmpD8->type=(void*)((void*)ts2->hd);_tmpD8->width=0;
_tmpD8->attributes=0;_tmpD8;});_tmpD7->tl=_tmpD6;_tmpD7;});}}_tmpD6=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD6);{struct Cyc_Absyn_Aggrdecl*
_tmpD9=({struct Cyc_Absyn_Aggrdecl*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF->kind=(
void*)((void*)0);_tmpDF->sc=(void*)((void*)2);_tmpDF->name=({struct _tuple1*
_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmpE2;(_tmpE2.Rel_n).tag=1;(_tmpE2.Rel_n).f1=0;
_tmpE2;});_tmpE1->f2=x;_tmpE1;});_tmpDF->tvs=0;_tmpDF->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->exist_vars=0;_tmpE0->rgn_po=0;_tmpE0->fields=
_tmpD6;_tmpE0;});_tmpDF->attributes=0;_tmpDF;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA->hd=Cyc_Absyn_new_decl((void*)({struct
Cyc_Absyn_Aggr_d_struct*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmpDC;_tmpDC.tag=4;_tmpDC.f1=_tmpD9;_tmpDC;});_tmpDB;}),0);_tmpDA->tl=Cyc_Toc_result_decls;
_tmpDA;});Cyc_Toc_tuple_types=({struct Cyc_List_List*_tmpDD=_cycalloc(sizeof(*
_tmpDD));_tmpDD->hd=({struct _tuple7*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->f1=
x;_tmpDE->f2=ts;_tmpDE;});_tmpDD->tl=Cyc_Toc_tuple_types;_tmpDD;});return x;}}}
struct _tuple1*Cyc_Toc_temp_var(){return({struct _tuple1*_tmpE7=_cycalloc(sizeof(*
_tmpE7));_tmpE7->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmpEC;(_tmpEC.Loc_n).tag=0;_tmpEC;});_tmpE7->f2=({struct _dynforward_ptr*_tmpE8=
_cycalloc(sizeof(*_tmpE8));_tmpE8[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmpEB;_tmpEB.tag=1;_tmpEB.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;{void*
_tmpE9[1]={& _tmpEB};Cyc_aprintf(({const char*_tmpEA="_tmp%X";_tag_dynforward(
_tmpEA,sizeof(char),_get_zero_arr_size(_tmpEA,7));}),_tag_dynforward(_tmpE9,
sizeof(void*),1));}});_tmpE8;});_tmpE7;});}static struct _dynforward_ptr*Cyc_Toc_fresh_label(){
return({struct _dynforward_ptr*_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED[0]=(
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmpF0;_tmpF0.tag=1;_tmpF0.f1=(
unsigned int)Cyc_Toc_fresh_label_counter ++;{void*_tmpEE[1]={& _tmpF0};Cyc_aprintf(({
const char*_tmpEF="_LL%X";_tag_dynforward(_tmpEF,sizeof(char),_get_zero_arr_size(
_tmpEF,6));}),_tag_dynforward(_tmpEE,sizeof(void*),1));}});_tmpED;});}static
struct Cyc_Absyn_Exp*Cyc_Toc_tunion_tag(struct Cyc_Absyn_Tuniondecl*td,struct
_tuple1*name,int carries_value){int ans=0;struct Cyc_List_List*_tmpF1=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmpF1))->hd)->name)
!= 0){if((td->is_flat  || carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmpF1->hd)->typs
!= 0) || !carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmpF1->hd)->typs == 0)
++ ans;_tmpF1=_tmpF1->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}static
int Cyc_Toc_num_void_tags(struct Cyc_Absyn_Tuniondecl*td){int ans=0;{struct Cyc_List_List*
_tmpF2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
for(0;_tmpF2 != 0;_tmpF2=_tmpF2->tl){if(((struct Cyc_Absyn_Tunionfield*)_tmpF2->hd)->typs
== 0)++ ans;}}return ans;}static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple2*
Cyc_Toc_arg_to_c(struct _tuple2*a){struct Cyc_Core_Opt*_tmpF4;struct Cyc_Absyn_Tqual
_tmpF5;void*_tmpF6;struct _tuple2 _tmpF3=*a;_tmpF4=_tmpF3.f1;_tmpF5=_tmpF3.f2;
_tmpF6=_tmpF3.f3;return({struct _tuple2*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7->f1=
_tmpF4;_tmpF7->f2=_tmpF5;_tmpF7->f3=Cyc_Toc_typ_to_c(_tmpF6);_tmpF7;});}static
struct _tuple4*Cyc_Toc_typ_to_c_f(struct _tuple4*x){struct Cyc_Absyn_Tqual _tmpF9;
void*_tmpFA;struct _tuple4 _tmpF8=*x;_tmpF9=_tmpF8.f1;_tmpFA=_tmpF8.f2;return({
struct _tuple4*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->f1=_tmpF9;_tmpFB->f2=Cyc_Toc_typ_to_c(
_tmpFA);_tmpFB;});}static void*Cyc_Toc_typ_to_c_array(void*t){void*_tmpFC=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmpFD;void*_tmpFE;struct Cyc_Absyn_Tqual _tmpFF;
struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Conref*_tmp101;struct Cyc_Position_Segment*
_tmp102;struct Cyc_Core_Opt*_tmp103;struct Cyc_Core_Opt _tmp104;void*_tmp105;_LL1F:
if(_tmpFC <= (void*)4)goto _LL23;if(*((int*)_tmpFC)!= 7)goto _LL21;_tmpFD=((struct
Cyc_Absyn_ArrayType_struct*)_tmpFC)->f1;_tmpFE=(void*)_tmpFD.elt_type;_tmpFF=
_tmpFD.tq;_tmp100=_tmpFD.num_elts;_tmp101=_tmpFD.zero_term;_tmp102=_tmpFD.zt_loc;
_LL20: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmpFE),_tmpFF,_tmp100,
Cyc_Absyn_false_conref,_tmp102);_LL21: if(*((int*)_tmpFC)!= 0)goto _LL23;_tmp103=((
struct Cyc_Absyn_Evar_struct*)_tmpFC)->f2;if(_tmp103 == 0)goto _LL23;_tmp104=*
_tmp103;_tmp105=(void*)_tmp104.v;_LL22: return Cyc_Toc_typ_to_c_array(_tmp105);
_LL23:;_LL24: return Cyc_Toc_typ_to_c(t);_LL1E:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){return({struct Cyc_Absyn_Aggrfield*
_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->name=f->name;_tmp106->tq=Cyc_Toc_mt_tq;
_tmp106->type=(void*)Cyc_Toc_typ_to_c((void*)f->type);_tmp106->width=f->width;
_tmp106->attributes=f->attributes;_tmp106;});}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs){return;}static void*Cyc_Toc_typ_to_c(void*t){void*_tmp107=
t;struct Cyc_Core_Opt*_tmp108;struct Cyc_Core_Opt*_tmp109;struct Cyc_Core_Opt
_tmp10A;void*_tmp10B;struct Cyc_Absyn_Tvar*_tmp10C;struct Cyc_Absyn_TunionInfo
_tmp10D;void*_tmp10E;struct Cyc_Absyn_Tuniondecl**_tmp10F;struct Cyc_Absyn_Tuniondecl*
_tmp110;struct Cyc_Absyn_TunionFieldInfo _tmp111;void*_tmp112;struct Cyc_Absyn_Tuniondecl*
_tmp113;struct Cyc_Absyn_Tunionfield*_tmp114;struct Cyc_Absyn_PtrInfo _tmp115;void*
_tmp116;struct Cyc_Absyn_Tqual _tmp117;struct Cyc_Absyn_PtrAtts _tmp118;struct Cyc_Absyn_Conref*
_tmp119;struct Cyc_Absyn_ArrayInfo _tmp11A;void*_tmp11B;struct Cyc_Absyn_Tqual
_tmp11C;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Position_Segment*_tmp11E;struct Cyc_Absyn_FnInfo
_tmp11F;void*_tmp120;struct Cyc_List_List*_tmp121;int _tmp122;struct Cyc_Absyn_VarargInfo*
_tmp123;struct Cyc_List_List*_tmp124;struct Cyc_List_List*_tmp125;void*_tmp126;
struct Cyc_List_List*_tmp127;struct Cyc_Absyn_AggrInfo _tmp128;void*_tmp129;struct
Cyc_List_List*_tmp12A;struct _tuple1*_tmp12B;struct Cyc_List_List*_tmp12C;struct
_tuple1*_tmp12D;struct Cyc_List_List*_tmp12E;struct Cyc_Absyn_Typedefdecl*_tmp12F;
void**_tmp130;void*_tmp131;_LL26: if((int)_tmp107 != 0)goto _LL28;_LL27: return t;
_LL28: if(_tmp107 <= (void*)4)goto _LL3A;if(*((int*)_tmp107)!= 0)goto _LL2A;_tmp108=((
struct Cyc_Absyn_Evar_struct*)_tmp107)->f2;if(_tmp108 != 0)goto _LL2A;_LL29: return
Cyc_Absyn_sint_typ;_LL2A: if(*((int*)_tmp107)!= 0)goto _LL2C;_tmp109=((struct Cyc_Absyn_Evar_struct*)
_tmp107)->f2;if(_tmp109 == 0)goto _LL2C;_tmp10A=*_tmp109;_tmp10B=(void*)_tmp10A.v;
_LL2B: return Cyc_Toc_typ_to_c(_tmp10B);_LL2C: if(*((int*)_tmp107)!= 1)goto _LL2E;
_tmp10C=((struct Cyc_Absyn_VarType_struct*)_tmp107)->f1;_LL2D: if(Cyc_Tcutil_tvar_kind(
_tmp10C)== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}_LL2E:
if(*((int*)_tmp107)!= 2)goto _LL30;_tmp10D=((struct Cyc_Absyn_TunionType_struct*)
_tmp107)->f1;_tmp10E=(void*)_tmp10D.tunion_info;if(*((int*)_tmp10E)!= 1)goto
_LL30;_tmp10F=((struct Cyc_Absyn_KnownTunion_struct*)_tmp10E)->f1;_tmp110=*
_tmp10F;_LL2F: if(_tmp110->is_flat)return Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(
_tmp110->name,({const char*_tmp132="_union";_tag_dynforward(_tmp132,sizeof(char),
_get_zero_arr_size(_tmp132,7));})));else{return Cyc_Absyn_void_star_typ();}_LL30:
if(*((int*)_tmp107)!= 2)goto _LL32;_LL31:({void*_tmp133[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp134="unresolved TunionType";_tag_dynforward(_tmp134,sizeof(char),
_get_zero_arr_size(_tmp134,22));}),_tag_dynforward(_tmp133,sizeof(void*),0));});
_LL32: if(*((int*)_tmp107)!= 3)goto _LL34;_tmp111=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp107)->f1;_tmp112=(void*)_tmp111.field_info;if(*((int*)_tmp112)!= 1)goto _LL34;
_tmp113=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp112)->f1;_tmp114=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp112)->f2;_LL33: if(_tmp113->is_flat)return
Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(_tmp113->name,({const char*_tmp135="_union";
_tag_dynforward(_tmp135,sizeof(char),_get_zero_arr_size(_tmp135,7));})));if(
_tmp114->typs == 0){if(_tmp113->is_xtunion)return Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);else{return Cyc_Absyn_uint_typ;}}
else{return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp114->name,({const char*
_tmp136="_struct";_tag_dynforward(_tmp136,sizeof(char),_get_zero_arr_size(
_tmp136,8));})));}_LL34: if(*((int*)_tmp107)!= 3)goto _LL36;_LL35:({void*_tmp137[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp138="unresolved TunionFieldType";_tag_dynforward(_tmp138,sizeof(
char),_get_zero_arr_size(_tmp138,27));}),_tag_dynforward(_tmp137,sizeof(void*),0));});
_LL36: if(*((int*)_tmp107)!= 4)goto _LL38;_tmp115=((struct Cyc_Absyn_PointerType_struct*)
_tmp107)->f1;_tmp116=(void*)_tmp115.elt_typ;_tmp117=_tmp115.elt_tq;_tmp118=
_tmp115.ptr_atts;_tmp119=_tmp118.bounds;_LL37: _tmp116=Cyc_Toc_typ_to_c_array(
_tmp116);{void*_tmp139=(void*)(Cyc_Absyn_compress_conref(_tmp119))->v;void*
_tmp13A;void*_tmp13B;_LL61: if((int)_tmp139 != 0)goto _LL63;_LL62: goto _LL64;_LL63:
if(_tmp139 <= (void*)1)goto _LL67;if(*((int*)_tmp139)!= 0)goto _LL65;_tmp13A=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp139)->f1;if((int)_tmp13A != 0)goto _LL65;
_LL64: return Cyc_Toc_dynforward_ptr_typ;_LL65: if(*((int*)_tmp139)!= 0)goto _LL67;
_tmp13B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp139)->f1;if((int)_tmp13B
!= 1)goto _LL67;_LL66: return Cyc_Toc_dyneither_ptr_typ;_LL67:;_LL68: return Cyc_Absyn_star_typ(
_tmp116,(void*)2,_tmp117,Cyc_Absyn_false_conref);_LL60:;}_LL38: if(*((int*)
_tmp107)!= 5)goto _LL3A;_LL39: goto _LL3B;_LL3A: if((int)_tmp107 != 1)goto _LL3C;_LL3B:
goto _LL3D;_LL3C: if(_tmp107 <= (void*)4)goto _LL56;if(*((int*)_tmp107)!= 6)goto
_LL3E;_LL3D: return t;_LL3E: if(*((int*)_tmp107)!= 7)goto _LL40;_tmp11A=((struct Cyc_Absyn_ArrayType_struct*)
_tmp107)->f1;_tmp11B=(void*)_tmp11A.elt_type;_tmp11C=_tmp11A.tq;_tmp11D=_tmp11A.num_elts;
_tmp11E=_tmp11A.zt_loc;_LL3F: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp11B),_tmp11C,_tmp11D,Cyc_Absyn_false_conref,_tmp11E);_LL40: if(*((int*)
_tmp107)!= 8)goto _LL42;_tmp11F=((struct Cyc_Absyn_FnType_struct*)_tmp107)->f1;
_tmp120=(void*)_tmp11F.ret_typ;_tmp121=_tmp11F.args;_tmp122=_tmp11F.c_varargs;
_tmp123=_tmp11F.cyc_varargs;_tmp124=_tmp11F.attributes;_LL41: {struct Cyc_List_List*
_tmp13C=0;for(0;_tmp124 != 0;_tmp124=_tmp124->tl){void*_tmp13D=(void*)_tmp124->hd;
_LL6A: if((int)_tmp13D != 3)goto _LL6C;_LL6B: goto _LL6D;_LL6C: if((int)_tmp13D != 4)
goto _LL6E;_LL6D: goto _LL6F;_LL6E: if(_tmp13D <= (void*)17)goto _LL72;if(*((int*)
_tmp13D)!= 3)goto _LL70;_LL6F: continue;_LL70: if(*((int*)_tmp13D)!= 4)goto _LL72;
_LL71: continue;_LL72:;_LL73: _tmp13C=({struct Cyc_List_List*_tmp13E=_cycalloc(
sizeof(*_tmp13E));_tmp13E->hd=(void*)((void*)_tmp124->hd);_tmp13E->tl=_tmp13C;
_tmp13E;});goto _LL69;_LL69:;}{struct Cyc_List_List*_tmp13F=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,
_tmp121);if(_tmp123 != 0){void*_tmp140=Cyc_Toc_typ_to_c(Cyc_Absyn_dynforward_typ((
void*)_tmp123->type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));struct
_tuple2*_tmp141=({struct _tuple2*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->f1=
_tmp123->name;_tmp143->f2=_tmp123->tq;_tmp143->f3=_tmp140;_tmp143;});_tmp13F=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp13F,({struct Cyc_List_List*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->hd=
_tmp141;_tmp142->tl=0;_tmp142;}));}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144[0]=({struct Cyc_Absyn_FnType_struct
_tmp145;_tmp145.tag=8;_tmp145.f1=({struct Cyc_Absyn_FnInfo _tmp146;_tmp146.tvars=0;
_tmp146.effect=0;_tmp146.ret_typ=(void*)Cyc_Toc_typ_to_c(_tmp120);_tmp146.args=
_tmp13F;_tmp146.c_varargs=_tmp122;_tmp146.cyc_varargs=0;_tmp146.rgn_po=0;_tmp146.attributes=
_tmp13C;_tmp146;});_tmp145;});_tmp144;});}}_LL42: if(*((int*)_tmp107)!= 9)goto
_LL44;_tmp125=((struct Cyc_Absyn_TupleType_struct*)_tmp107)->f1;_LL43: _tmp125=((
struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct _tuple4*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp125);{struct _dynforward_ptr*_tmp147=Cyc_Toc_add_tuple_type(
_tmp125);return Cyc_Absyn_strct(_tmp147);}_LL44: if(*((int*)_tmp107)!= 11)goto
_LL46;_tmp126=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp107)->f1;_tmp127=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp107)->f2;_LL45: return(void*)({struct Cyc_Absyn_AnonAggrType_struct*
_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmp149;_tmp149.tag=11;_tmp149.f1=(void*)_tmp126;_tmp149.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp127);_tmp149;});_tmp148;});_LL46: if(*((
int*)_tmp107)!= 10)goto _LL48;_tmp128=((struct Cyc_Absyn_AggrType_struct*)_tmp107)->f1;
_tmp129=(void*)_tmp128.aggr_info;_tmp12A=_tmp128.targs;_LL47: {struct Cyc_Absyn_Aggrdecl*
_tmp14A=Cyc_Absyn_get_known_aggrdecl(_tmp129);if((void*)_tmp14A->kind == (void*)1)
return Cyc_Absyn_unionq_typ(_tmp14A->name);return Cyc_Absyn_strctq(_tmp14A->name);}
_LL48: if(*((int*)_tmp107)!= 12)goto _LL4A;_tmp12B=((struct Cyc_Absyn_EnumType_struct*)
_tmp107)->f1;_LL49: return t;_LL4A: if(*((int*)_tmp107)!= 13)goto _LL4C;_tmp12C=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp107)->f1;_LL4B: Cyc_Toc_enumfields_to_c(
_tmp12C);return t;_LL4C: if(*((int*)_tmp107)!= 16)goto _LL4E;_tmp12D=((struct Cyc_Absyn_TypedefType_struct*)
_tmp107)->f1;_tmp12E=((struct Cyc_Absyn_TypedefType_struct*)_tmp107)->f2;_tmp12F=((
struct Cyc_Absyn_TypedefType_struct*)_tmp107)->f3;_tmp130=((struct Cyc_Absyn_TypedefType_struct*)
_tmp107)->f4;_LL4D: if(_tmp130 == 0)return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp14C;_tmp14C.tag=16;_tmp14C.f1=_tmp12D;_tmp14C.f2=0;_tmp14C.f3=_tmp12F;
_tmp14C.f4=0;_tmp14C;});_tmp14B;});else{return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp14E;_tmp14E.tag=16;_tmp14E.f1=_tmp12D;_tmp14E.f2=0;_tmp14E.f3=_tmp12F;
_tmp14E.f4=({void**_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F[0]=Cyc_Toc_typ_to_c_array(*
_tmp130);_tmp14F;});_tmp14E;});_tmp14D;});}_LL4E: if(*((int*)_tmp107)!= 14)goto
_LL50;_LL4F: goto _LL51;_LL50: if(*((int*)_tmp107)!= 17)goto _LL52;_LL51: return Cyc_Absyn_uint_typ;
_LL52: if(*((int*)_tmp107)!= 15)goto _LL54;_tmp131=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp107)->f1;_LL53: return Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq);_LL54: if(*((int*)_tmp107)!= 18)goto _LL56;_LL55:({void*_tmp150[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp151="Toc::typ_to_c: type translation passed a type integer";
_tag_dynforward(_tmp151,sizeof(char),_get_zero_arr_size(_tmp151,54));}),
_tag_dynforward(_tmp150,sizeof(void*),0));});_LL56: if((int)_tmp107 != 2)goto _LL58;
_LL57:({void*_tmp152[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp153="Toc::typ_to_c: type translation passed the heap region";
_tag_dynforward(_tmp153,sizeof(char),_get_zero_arr_size(_tmp153,55));}),
_tag_dynforward(_tmp152,sizeof(void*),0));});_LL58: if((int)_tmp107 != 3)goto _LL5A;
_LL59:({void*_tmp154[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp155="Toc::typ_to_c: type translation passed the unique region";
_tag_dynforward(_tmp155,sizeof(char),_get_zero_arr_size(_tmp155,57));}),
_tag_dynforward(_tmp154,sizeof(void*),0));});_LL5A: if(_tmp107 <= (void*)4)goto
_LL5C;if(*((int*)_tmp107)!= 19)goto _LL5C;_LL5B: goto _LL5D;_LL5C: if(_tmp107 <= (
void*)4)goto _LL5E;if(*((int*)_tmp107)!= 20)goto _LL5E;_LL5D: goto _LL5F;_LL5E: if(
_tmp107 <= (void*)4)goto _LL25;if(*((int*)_tmp107)!= 21)goto _LL25;_LL5F:({void*
_tmp156[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp157="Toc::typ_to_c: type translation passed an effect";
_tag_dynforward(_tmp157,sizeof(char),_get_zero_arr_size(_tmp157,49));}),
_tag_dynforward(_tmp156,sizeof(void*),0));});_LL25:;}static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
l){void*_tmp158=t;struct Cyc_Absyn_ArrayInfo _tmp159;void*_tmp15A;struct Cyc_Absyn_Tqual
_tmp15B;_LL75: if(_tmp158 <= (void*)4)goto _LL77;if(*((int*)_tmp158)!= 7)goto _LL77;
_tmp159=((struct Cyc_Absyn_ArrayType_struct*)_tmp158)->f1;_tmp15A=(void*)_tmp159.elt_type;
_tmp15B=_tmp159.tq;_LL76: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp15A,(void*)
2,_tmp15B,Cyc_Absyn_false_conref),e);_LL77:;_LL78: return Cyc_Toc_cast_it(t,e);
_LL74:;}static int Cyc_Toc_atomic_typ(void*t){void*_tmp15C=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_ArrayInfo _tmp15D;void*_tmp15E;struct Cyc_Absyn_AggrInfo _tmp15F;
void*_tmp160;struct Cyc_List_List*_tmp161;struct Cyc_Absyn_TunionFieldInfo _tmp162;
void*_tmp163;struct Cyc_Absyn_Tuniondecl*_tmp164;struct Cyc_Absyn_Tunionfield*
_tmp165;struct Cyc_List_List*_tmp166;_LL7A: if((int)_tmp15C != 0)goto _LL7C;_LL7B:
return 1;_LL7C: if(_tmp15C <= (void*)4)goto _LL84;if(*((int*)_tmp15C)!= 1)goto _LL7E;
_LL7D: return 0;_LL7E: if(*((int*)_tmp15C)!= 5)goto _LL80;_LL7F: goto _LL81;_LL80: if(*((
int*)_tmp15C)!= 12)goto _LL82;_LL81: goto _LL83;_LL82: if(*((int*)_tmp15C)!= 13)goto
_LL84;_LL83: goto _LL85;_LL84: if((int)_tmp15C != 1)goto _LL86;_LL85: goto _LL87;_LL86:
if(_tmp15C <= (void*)4)goto _LL9E;if(*((int*)_tmp15C)!= 6)goto _LL88;_LL87: goto
_LL89;_LL88: if(*((int*)_tmp15C)!= 8)goto _LL8A;_LL89: goto _LL8B;_LL8A: if(*((int*)
_tmp15C)!= 17)goto _LL8C;_LL8B: goto _LL8D;_LL8C: if(*((int*)_tmp15C)!= 14)goto _LL8E;
_LL8D: return 1;_LL8E: if(*((int*)_tmp15C)!= 7)goto _LL90;_tmp15D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp15C)->f1;_tmp15E=(void*)_tmp15D.elt_type;_LL8F: return Cyc_Toc_atomic_typ(
_tmp15E);_LL90: if(*((int*)_tmp15C)!= 10)goto _LL92;_tmp15F=((struct Cyc_Absyn_AggrType_struct*)
_tmp15C)->f1;_tmp160=(void*)_tmp15F.aggr_info;_LL91:{void*_tmp167=_tmp160;_LLA1:
if(*((int*)_tmp167)!= 0)goto _LLA3;_LLA2: return 0;_LLA3:;_LLA4: goto _LLA0;_LLA0:;}{
struct Cyc_Absyn_Aggrdecl*_tmp168=Cyc_Absyn_get_known_aggrdecl(_tmp160);if(
_tmp168->impl == 0)return 0;{struct Cyc_List_List*_tmp169=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp168->impl))->fields;for(0;_tmp169 != 0;_tmp169=_tmp169->tl){if(!
Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)_tmp169->hd)->type))
return 0;}}return 1;}_LL92: if(*((int*)_tmp15C)!= 11)goto _LL94;_tmp161=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp15C)->f2;_LL93: for(0;_tmp161 != 0;_tmp161=_tmp161->tl){if(!Cyc_Toc_atomic_typ((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp161->hd)->type))return 0;}return 1;_LL94:
if(*((int*)_tmp15C)!= 3)goto _LL96;_tmp162=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp15C)->f1;_tmp163=(void*)_tmp162.field_info;if(*((int*)_tmp163)!= 1)goto _LL96;
_tmp164=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp163)->f1;_tmp165=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp163)->f2;_LL95: _tmp166=_tmp165->typs;goto
_LL97;_LL96: if(*((int*)_tmp15C)!= 9)goto _LL98;_tmp166=((struct Cyc_Absyn_TupleType_struct*)
_tmp15C)->f1;_LL97: for(0;_tmp166 != 0;_tmp166=_tmp166->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple4*)_tmp166->hd)).f2))return 0;}return 1;_LL98: if(*((int*)_tmp15C)!= 2)
goto _LL9A;_LL99: goto _LL9B;_LL9A: if(*((int*)_tmp15C)!= 4)goto _LL9C;_LL9B: goto
_LL9D;_LL9C: if(*((int*)_tmp15C)!= 15)goto _LL9E;_LL9D: return 0;_LL9E:;_LL9F:({
struct Cyc_String_pa_struct _tmp16C;_tmp16C.tag=0;_tmp16C.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp16A[
1]={& _tmp16C};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp16B="atomic_typ:  bad type %s";_tag_dynforward(_tmp16B,sizeof(char),
_get_zero_arr_size(_tmp16B,25));}),_tag_dynforward(_tmp16A,sizeof(void*),1));}});
_LL79:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp16D=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp16E;void*_tmp16F;_LLA6: if(_tmp16D <= (void*)4)goto
_LLA8;if(*((int*)_tmp16D)!= 4)goto _LLA8;_tmp16E=((struct Cyc_Absyn_PointerType_struct*)
_tmp16D)->f1;_tmp16F=(void*)_tmp16E.elt_typ;_LLA7: {void*_tmp170=Cyc_Tcutil_compress(
_tmp16F);_LLAB: if((int)_tmp170 != 0)goto _LLAD;_LLAC: return 1;_LLAD:;_LLAE: return 0;
_LLAA:;}_LLA8:;_LLA9: return 0;_LLA5:;}static int Cyc_Toc_is_poly_field(void*t,
struct _dynforward_ptr*f){void*_tmp171=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp172;void*_tmp173;struct Cyc_List_List*_tmp174;_LLB0: if(_tmp171 <= (void*)4)
goto _LLB4;if(*((int*)_tmp171)!= 10)goto _LLB2;_tmp172=((struct Cyc_Absyn_AggrType_struct*)
_tmp171)->f1;_tmp173=(void*)_tmp172.aggr_info;_LLB1: {struct Cyc_Absyn_Aggrdecl*
_tmp175=Cyc_Absyn_get_known_aggrdecl(_tmp173);if(_tmp175->impl == 0)({void*
_tmp176[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp177="is_poly_field: type missing fields";_tag_dynforward(_tmp177,
sizeof(char),_get_zero_arr_size(_tmp177,35));}),_tag_dynforward(_tmp176,sizeof(
void*),0));});_tmp174=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp175->impl))->fields;
goto _LLB3;}_LLB2: if(*((int*)_tmp171)!= 11)goto _LLB4;_tmp174=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp171)->f2;_LLB3: {struct Cyc_Absyn_Aggrfield*_tmp178=Cyc_Absyn_lookup_field(
_tmp174,f);if(_tmp178 == 0)({struct Cyc_String_pa_struct _tmp17B;_tmp17B.tag=0;
_tmp17B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp179[1]={&
_tmp17B};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp17A="is_poly_field: bad field %s";_tag_dynforward(_tmp17A,sizeof(
char),_get_zero_arr_size(_tmp17A,28));}),_tag_dynforward(_tmp179,sizeof(void*),1));}});
return Cyc_Toc_is_void_star((void*)_tmp178->type);}_LLB4:;_LLB5:({struct Cyc_String_pa_struct
_tmp17E;_tmp17E.tag=0;_tmp17E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp17C[1]={& _tmp17E};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp17D="is_poly_field: bad type %s";_tag_dynforward(_tmp17D,sizeof(char),
_get_zero_arr_size(_tmp17D,27));}),_tag_dynforward(_tmp17C,sizeof(void*),1));}});
_LLAF:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp17F=(
void*)e->r;struct Cyc_Absyn_Exp*_tmp180;struct _dynforward_ptr*_tmp181;struct Cyc_Absyn_Exp*
_tmp182;struct _dynforward_ptr*_tmp183;_LLB7: if(*((int*)_tmp17F)!= 23)goto _LLB9;
_tmp180=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp17F)->f1;_tmp181=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp17F)->f2;_LLB8: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp180->topt))->v,_tmp181);_LLB9: if(*((int*)_tmp17F)!= 24)goto _LLBB;
_tmp182=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp17F)->f1;_tmp183=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp17F)->f2;_LLBA: {void*_tmp184=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp182->topt))->v);struct Cyc_Absyn_PtrInfo _tmp185;void*_tmp186;
_LLBE: if(_tmp184 <= (void*)4)goto _LLC0;if(*((int*)_tmp184)!= 4)goto _LLC0;_tmp185=((
struct Cyc_Absyn_PointerType_struct*)_tmp184)->f1;_tmp186=(void*)_tmp185.elt_typ;
_LLBF: return Cyc_Toc_is_poly_field(_tmp186,_tmp183);_LLC0:;_LLC1:({struct Cyc_String_pa_struct
_tmp189;_tmp189.tag=0;_tmp189.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp182->topt))->v));{
void*_tmp187[1]={& _tmp189};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp188="is_poly_project: bad type %s";
_tag_dynforward(_tmp188,sizeof(char),_get_zero_arr_size(_tmp188,29));}),
_tag_dynforward(_tmp187,sizeof(void*),1));}});_LLBD:;}_LLBB:;_LLBC: return 0;_LLB6:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp18A=_cycalloc(sizeof(*_tmp18A));
_tmp18A->hd=s;_tmp18A->tl=0;_tmp18A;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,({
struct Cyc_List_List*_tmp18B=_cycalloc(sizeof(*_tmp18B));_tmp18B->hd=s;_tmp18B->tl=
0;_tmp18B;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*
s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(
s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*
_tmp18C[2];_tmp18C[1]=s;_tmp18C[0]=rgn;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp18C,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*
s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type))return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmp18D[2];_tmp18D[1]=n;
_tmp18D[0]=s;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp18D,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp18E[2];_tmp18E[1]=n;_tmp18E[0]=s;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp18E,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*_tmp18F[3];_tmp18F[2]=n;_tmp18F[1]=
s;_tmp18F[0]=rgn;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp18F,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({
struct Cyc_List_List*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190->hd=e;_tmp190->tl=
0;_tmp190;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,
struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e,int forward_only){int is_string=0;{
void*_tmp191=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp192;_LLC3: if(*((int*)
_tmp191)!= 0)goto _LLC5;_tmp192=((struct Cyc_Absyn_Const_e_struct*)_tmp191)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp191)->f1).String_c).tag != 5)goto _LLC5;
_LLC4: is_string=1;goto _LLC2;_LLC5:;_LLC6: goto _LLC2;_LLC2:;}{struct Cyc_Absyn_Exp*
xexp;struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194[0]=({
struct Cyc_Absyn_Var_d_struct _tmp195;_tmp195.tag=0;_tmp195.f1=vd;_tmp195;});
_tmp194;}),0);_tmp193->tl=Cyc_Toc_result_decls;_tmp193;});xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
struct Cyc_Absyn_Exp*urm_exp;if(!forward_only)urm_exp=Cyc_Absyn_unresolvedmem_exp(
0,({struct _tuple6*_tmp196[3];_tmp196[2]=({struct _tuple6*_tmp199=_cycalloc(
sizeof(*_tmp199));_tmp199->f1=0;_tmp199->f2=xplussz;_tmp199;});_tmp196[1]=({
struct _tuple6*_tmp198=_cycalloc(sizeof(*_tmp198));_tmp198->f1=0;_tmp198->f2=xexp;
_tmp198;});_tmp196[0]=({struct _tuple6*_tmp197=_cycalloc(sizeof(*_tmp197));
_tmp197->f1=0;_tmp197->f2=xexp;_tmp197;});((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp196,sizeof(struct _tuple6*),3));}),
0);else{urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple6*_tmp19A[2];_tmp19A[
1]=({struct _tuple6*_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C->f1=0;_tmp19C->f2=
xplussz;_tmp19C;});_tmp19A[0]=({struct _tuple6*_tmp19B=_cycalloc(sizeof(*_tmp19B));
_tmp19B->f1=0;_tmp19B->f2=xexp;_tmp19B;});((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp19A,sizeof(struct _tuple6*),2));}),
0);}return urm_exp;}}}struct Cyc_Toc_Env{struct Cyc_Core_Opt*break_lab;struct Cyc_Core_Opt*
continue_lab;struct Cyc_Core_Opt*fallthru_info;struct Cyc_Dict_Dict*varmap;int
toplevel;};static struct Cyc_Toc_Env*Cyc_Toc_empty_env(){return({struct Cyc_Toc_Env*
_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D->break_lab=0;_tmp19D->continue_lab=0;
_tmp19D->fallthru_info=0;_tmp19D->varmap=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp19D->toplevel=
1;_tmp19D;});}static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp19E=_cycalloc(sizeof(*_tmp19E));
_tmp19E[0]=*e;_tmp19E;});ans->toplevel=0;return ans;}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){{union Cyc_Absyn_Nmspace_union
_tmp19F=(*x).f1;_LLC8: if((_tmp19F.Rel_n).tag != 1)goto _LLCA;_LLC9:({struct Cyc_String_pa_struct
_tmp1A2;_tmp1A2.tag=0;_tmp1A2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(x));{void*_tmp1A0[1]={& _tmp1A2};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp1A1="Toc::add_varmap on Rel_n: %s\n";_tag_dynforward(_tmp1A1,sizeof(char),
_get_zero_arr_size(_tmp1A1,30));}),_tag_dynforward(_tmp1A0,sizeof(void*),1));}});
_LLCA:;_LLCB: goto _LLC7;_LLC7:;}{struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*
_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3[0]=*e;_tmp1A3;});ans->varmap=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,struct Cyc_Absyn_Exp*
v))Cyc_Dict_insert)(ans->varmap,x,y);return ans;}}static struct Cyc_Toc_Env*Cyc_Toc_loop_env(
struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp1A4=_cycalloc(
sizeof(*_tmp1A4));_tmp1A4[0]=*e;_tmp1A4;});ans->break_lab=0;ans->continue_lab=0;
return ans;}struct _tuple8{struct _dynforward_ptr*f1;struct Cyc_List_List*f2;struct
Cyc_Dict_Dict*f3;};static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env*e,struct _dynforward_ptr*break_l,struct _dynforward_ptr*
fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
struct Cyc_List_List*_tmp1A5=0;for(0;fallthru_binders != 0;fallthru_binders=
fallthru_binders->tl){_tmp1A5=({struct Cyc_List_List*_tmp1A6=_cycalloc(sizeof(*
_tmp1A6));_tmp1A6->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name;
_tmp1A6->tl=_tmp1A5;_tmp1A6;});}_tmp1A5=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp1A5);{struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp1AA=
_cycalloc(sizeof(*_tmp1AA));_tmp1AA[0]=*e;_tmp1AA;});ans->break_lab=({struct Cyc_Core_Opt*
_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7->v=break_l;_tmp1A7;});ans->fallthru_info=({
struct Cyc_Core_Opt*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));_tmp1A8->v=({struct
_tuple8*_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9->f1=fallthru_l;_tmp1A9->f2=
_tmp1A5;_tmp1A9->f3=next_case_env->varmap;_tmp1A9;});_tmp1A8;});return ans;}}
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct Cyc_Toc_Env*e,struct
_dynforward_ptr*break_l){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp1AC=
_cycalloc(sizeof(*_tmp1AC));_tmp1AC[0]=*e;_tmp1AC;});ans->break_lab=({struct Cyc_Core_Opt*
_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB->v=break_l;_tmp1AB;});ans->fallthru_info=
0;return ans;}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct Cyc_Toc_Env*
e,struct _dynforward_ptr*next_l){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*
_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF[0]=*e;_tmp1AF;});ans->break_lab=0;
ans->fallthru_info=({struct Cyc_Core_Opt*_tmp1AD=_cycalloc(sizeof(*_tmp1AD));
_tmp1AD->v=({struct _tuple8*_tmp1AE=_cycalloc(sizeof(*_tmp1AE));_tmp1AE->f1=
next_l;_tmp1AE->f2=0;_tmp1AE->f3=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp1AE;});_tmp1AD;});
return ans;}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static
int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*_tmp1B0=(void*)e->annot;
_LLCD: if(*((void**)_tmp1B0)!= Cyc_CfFlowInfo_UnknownZ)goto _LLCF;_LLCE: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLCF: if(*((void**)_tmp1B0)
!= Cyc_CfFlowInfo_NotZero)goto _LLD1;_LLD0: return 0;_LLD1: if(_tmp1B0 != Cyc_CfFlowInfo_IsZero)
goto _LLD3;_LLD2:({void*_tmp1B1[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp1B2="dereference of NULL pointer";
_tag_dynforward(_tmp1B2,sizeof(char),_get_zero_arr_size(_tmp1B2,28));}),
_tag_dynforward(_tmp1B1,sizeof(void*),0));});return 0;_LLD3: if(_tmp1B0 != Cyc_Absyn_EmptyAnnot)
goto _LLD5;_LLD4: return 0;_LLD5: if(*((void**)_tmp1B0)!= Cyc_CfFlowInfo_HasTagCmps)
goto _LLD7;_LLD6:({void*_tmp1B3[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp1B4="compiler oddity: pointer compared to tag type";
_tag_dynforward(_tmp1B4,sizeof(char),_get_zero_arr_size(_tmp1B4,46));}),
_tag_dynforward(_tmp1B3,sizeof(void*),0));});return Cyc_Toc_is_nullable((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLD7:;_LLD8:({void*_tmp1B5[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp1B6="need_null_check";_tag_dynforward(_tmp1B6,sizeof(char),
_get_zero_arr_size(_tmp1B6,16));}),_tag_dynforward(_tmp1B5,sizeof(void*),0));});
_LLCC:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*
_tmp1B7=(void*)e->annot;struct Cyc_List_List*_tmp1B8;struct Cyc_List_List*_tmp1B9;
_LLDA: if(*((void**)_tmp1B7)!= Cyc_CfFlowInfo_UnknownZ)goto _LLDC;_tmp1B8=((struct
Cyc_CfFlowInfo_UnknownZ_struct*)_tmp1B7)->f1;_LLDB: return _tmp1B8;_LLDC: if(*((
void**)_tmp1B7)!= Cyc_CfFlowInfo_NotZero)goto _LLDE;_tmp1B9=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmp1B7)->f1;_LLDD: return _tmp1B9;_LLDE: if(_tmp1B7 != Cyc_CfFlowInfo_IsZero)goto
_LLE0;_LLDF:({void*_tmp1BA[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp1BB="dereference of NULL pointer";
_tag_dynforward(_tmp1BB,sizeof(char),_get_zero_arr_size(_tmp1BB,28));}),
_tag_dynforward(_tmp1BA,sizeof(void*),0));});return 0;_LLE0: if(*((void**)_tmp1B7)
!= Cyc_CfFlowInfo_HasTagCmps)goto _LLE2;_LLE1: goto _LLE3;_LLE2: if(_tmp1B7 != Cyc_Absyn_EmptyAnnot)
goto _LLE4;_LLE3: return 0;_LLE4:;_LLE5:({void*_tmp1BC[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp1BD="get_relns";_tag_dynforward(_tmp1BD,sizeof(char),_get_zero_arr_size(
_tmp1BD,10));}),_tag_dynforward(_tmp1BC,sizeof(void*),0));});_LLD9:;}static int
Cyc_Toc_check_const_array(unsigned int i,void*t){void*_tmp1BE=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp1BF;struct Cyc_Absyn_PtrAtts _tmp1C0;struct Cyc_Absyn_Conref*
_tmp1C1;struct Cyc_Absyn_Conref*_tmp1C2;struct Cyc_Absyn_ArrayInfo _tmp1C3;struct
Cyc_Absyn_Exp*_tmp1C4;_LLE7: if(_tmp1BE <= (void*)4)goto _LLEB;if(*((int*)_tmp1BE)
!= 4)goto _LLE9;_tmp1BF=((struct Cyc_Absyn_PointerType_struct*)_tmp1BE)->f1;
_tmp1C0=_tmp1BF.ptr_atts;_tmp1C1=_tmp1C0.bounds;_tmp1C2=_tmp1C0.zero_term;_LLE8: {
void*_tmp1C5=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp1C1);struct Cyc_Absyn_Exp*
_tmp1C6;_LLEE: if((int)_tmp1C5 != 0)goto _LLF0;_LLEF: return 0;_LLF0: if((int)_tmp1C5
!= 1)goto _LLF2;_LLF1: return 0;_LLF2: if(_tmp1C5 <= (void*)2)goto _LLF4;if(*((int*)
_tmp1C5)!= 0)goto _LLF4;_tmp1C6=((struct Cyc_Absyn_Upper_b_struct*)_tmp1C5)->f1;
_LLF3: {unsigned int _tmp1C8;int _tmp1C9;struct _tuple5 _tmp1C7=Cyc_Evexp_eval_const_uint_exp(
_tmp1C6);_tmp1C8=_tmp1C7.f1;_tmp1C9=_tmp1C7.f2;return _tmp1C9  && i <= _tmp1C8;}
_LLF4: if(_tmp1C5 <= (void*)2)goto _LLED;if(*((int*)_tmp1C5)!= 1)goto _LLED;_LLF5:({
void*_tmp1CA[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp1CB="check_const_array: AbsUpper_b";
_tag_dynforward(_tmp1CB,sizeof(char),_get_zero_arr_size(_tmp1CB,30));}),
_tag_dynforward(_tmp1CA,sizeof(void*),0));});_LLED:;}_LLE9: if(*((int*)_tmp1BE)!= 
7)goto _LLEB;_tmp1C3=((struct Cyc_Absyn_ArrayType_struct*)_tmp1BE)->f1;_tmp1C4=
_tmp1C3.num_elts;_LLEA: if(_tmp1C4 == 0)return 0;{unsigned int _tmp1CD;int _tmp1CE;
struct _tuple5 _tmp1CC=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp1C4);
_tmp1CD=_tmp1CC.f1;_tmp1CE=_tmp1CC.f2;return _tmp1CE  && i <= _tmp1CD;}_LLEB:;_LLEC:
return 0;_LLE6:;}static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp1CF=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(
_tmp1CF->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp_union _tmp1D0=_tmp1CF->rop;
struct Cyc_Absyn_Vardecl*_tmp1D1;struct Cyc_Absyn_Vardecl*_tmp1D2;_LLF7: if((
_tmp1D0.LessSize).tag != 2)goto _LLF9;_tmp1D1=(_tmp1D0.LessSize).f1;_LLF8: _tmp1D2=
_tmp1D1;goto _LLFA;_LLF9: if((_tmp1D0.LessEqSize).tag != 4)goto _LLFB;_tmp1D2=(
_tmp1D0.LessEqSize).f1;_LLFA: if(_tmp1D2 == v)return 1;else{goto _LLF6;}_LLFB:;_LLFC:
continue;_LLF6:;}}return 0;}static int Cyc_Toc_check_leq_size(struct Cyc_List_List*
relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*_tmp1D3=(void*)e->r;
void*_tmp1D4;struct Cyc_Absyn_Vardecl*_tmp1D5;void*_tmp1D6;struct Cyc_Absyn_Vardecl*
_tmp1D7;void*_tmp1D8;struct Cyc_Absyn_Vardecl*_tmp1D9;void*_tmp1DA;struct Cyc_Absyn_Vardecl*
_tmp1DB;void*_tmp1DC;struct Cyc_List_List*_tmp1DD;struct Cyc_List_List _tmp1DE;
struct Cyc_Absyn_Exp*_tmp1DF;_LLFE: if(*((int*)_tmp1D3)!= 1)goto _LL100;_tmp1D4=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1D3)->f2;if(_tmp1D4 <= (void*)1)goto
_LL100;if(*((int*)_tmp1D4)!= 4)goto _LL100;_tmp1D5=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1D4)->f1;_LLFF: _tmp1D7=_tmp1D5;goto _LL101;_LL100: if(*((int*)_tmp1D3)!= 1)
goto _LL102;_tmp1D6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1D3)->f2;if(
_tmp1D6 <= (void*)1)goto _LL102;if(*((int*)_tmp1D6)!= 3)goto _LL102;_tmp1D7=((
struct Cyc_Absyn_Local_b_struct*)_tmp1D6)->f1;_LL101: _tmp1D9=_tmp1D7;goto _LL103;
_LL102: if(*((int*)_tmp1D3)!= 1)goto _LL104;_tmp1D8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1D3)->f2;if(_tmp1D8 <= (void*)1)goto _LL104;if(*((int*)_tmp1D8)!= 0)goto _LL104;
_tmp1D9=((struct Cyc_Absyn_Global_b_struct*)_tmp1D8)->f1;_LL103: _tmp1DB=_tmp1D9;
goto _LL105;_LL104: if(*((int*)_tmp1D3)!= 1)goto _LL106;_tmp1DA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1D3)->f2;if(_tmp1DA <= (void*)1)goto _LL106;if(*((int*)_tmp1DA)!= 2)goto _LL106;
_tmp1DB=((struct Cyc_Absyn_Param_b_struct*)_tmp1DA)->f1;_LL105: if(_tmp1DB->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp1DB))return 1;goto _LLFD;_LL106:
if(*((int*)_tmp1D3)!= 3)goto _LL108;_tmp1DC=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D3)->f1;if((int)_tmp1DC != 19)goto _LL108;_tmp1DD=((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D3)->f2;if(_tmp1DD == 0)goto _LL108;_tmp1DE=*_tmp1DD;_tmp1DF=(struct Cyc_Absyn_Exp*)
_tmp1DE.hd;_LL107:{void*_tmp1E0=(void*)_tmp1DF->r;void*_tmp1E1;struct Cyc_Absyn_Vardecl*
_tmp1E2;void*_tmp1E3;struct Cyc_Absyn_Vardecl*_tmp1E4;void*_tmp1E5;struct Cyc_Absyn_Vardecl*
_tmp1E6;void*_tmp1E7;struct Cyc_Absyn_Vardecl*_tmp1E8;_LL10B: if(*((int*)_tmp1E0)
!= 1)goto _LL10D;_tmp1E1=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1E0)->f2;if(
_tmp1E1 <= (void*)1)goto _LL10D;if(*((int*)_tmp1E1)!= 4)goto _LL10D;_tmp1E2=((
struct Cyc_Absyn_Pat_b_struct*)_tmp1E1)->f1;_LL10C: _tmp1E4=_tmp1E2;goto _LL10E;
_LL10D: if(*((int*)_tmp1E0)!= 1)goto _LL10F;_tmp1E3=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1E0)->f2;if(_tmp1E3 <= (void*)1)goto _LL10F;if(*((int*)_tmp1E3)!= 3)goto _LL10F;
_tmp1E4=((struct Cyc_Absyn_Local_b_struct*)_tmp1E3)->f1;_LL10E: _tmp1E6=_tmp1E4;
goto _LL110;_LL10F: if(*((int*)_tmp1E0)!= 1)goto _LL111;_tmp1E5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1E0)->f2;if(_tmp1E5 <= (void*)1)goto _LL111;if(*((int*)_tmp1E5)!= 0)goto _LL111;
_tmp1E6=((struct Cyc_Absyn_Global_b_struct*)_tmp1E5)->f1;_LL110: _tmp1E8=_tmp1E6;
goto _LL112;_LL111: if(*((int*)_tmp1E0)!= 1)goto _LL113;_tmp1E7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1E0)->f2;if(_tmp1E7 <= (void*)1)goto _LL113;if(*((int*)_tmp1E7)!= 2)goto _LL113;
_tmp1E8=((struct Cyc_Absyn_Param_b_struct*)_tmp1E7)->f1;_LL112: return _tmp1E8 == v;
_LL113:;_LL114: goto _LL10A;_LL10A:;}goto _LLFD;_LL108:;_LL109: goto _LLFD;_LLFD:;}
return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i){{void*_tmp1E9=(void*)a->r;void*_tmp1EA;struct Cyc_Absyn_Vardecl*
_tmp1EB;void*_tmp1EC;struct Cyc_Absyn_Vardecl*_tmp1ED;void*_tmp1EE;struct Cyc_Absyn_Vardecl*
_tmp1EF;void*_tmp1F0;struct Cyc_Absyn_Vardecl*_tmp1F1;_LL116: if(*((int*)_tmp1E9)
!= 1)goto _LL118;_tmp1EA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1E9)->f2;if(
_tmp1EA <= (void*)1)goto _LL118;if(*((int*)_tmp1EA)!= 4)goto _LL118;_tmp1EB=((
struct Cyc_Absyn_Pat_b_struct*)_tmp1EA)->f1;_LL117: _tmp1ED=_tmp1EB;goto _LL119;
_LL118: if(*((int*)_tmp1E9)!= 1)goto _LL11A;_tmp1EC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1E9)->f2;if(_tmp1EC <= (void*)1)goto _LL11A;if(*((int*)_tmp1EC)!= 3)goto _LL11A;
_tmp1ED=((struct Cyc_Absyn_Local_b_struct*)_tmp1EC)->f1;_LL119: _tmp1EF=_tmp1ED;
goto _LL11B;_LL11A: if(*((int*)_tmp1E9)!= 1)goto _LL11C;_tmp1EE=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1E9)->f2;if(_tmp1EE <= (void*)1)goto _LL11C;if(*((int*)_tmp1EE)!= 0)goto _LL11C;
_tmp1EF=((struct Cyc_Absyn_Global_b_struct*)_tmp1EE)->f1;_LL11B: _tmp1F1=_tmp1EF;
goto _LL11D;_LL11C: if(*((int*)_tmp1E9)!= 1)goto _LL11E;_tmp1F0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1E9)->f2;if(_tmp1F0 <= (void*)1)goto _LL11E;if(*((int*)_tmp1F0)!= 2)goto _LL11E;
_tmp1F1=((struct Cyc_Absyn_Param_b_struct*)_tmp1F0)->f1;_LL11D: if(_tmp1F1->escapes)
return 0;inner_loop: {void*_tmp1F2=(void*)i->r;void*_tmp1F3;struct Cyc_Absyn_Exp*
_tmp1F4;union Cyc_Absyn_Cnst_union _tmp1F5;void*_tmp1F6;int _tmp1F7;union Cyc_Absyn_Cnst_union
_tmp1F8;void*_tmp1F9;int _tmp1FA;union Cyc_Absyn_Cnst_union _tmp1FB;void*_tmp1FC;
int _tmp1FD;void*_tmp1FE;struct Cyc_List_List*_tmp1FF;struct Cyc_List_List _tmp200;
struct Cyc_Absyn_Exp*_tmp201;struct Cyc_List_List*_tmp202;struct Cyc_List_List
_tmp203;struct Cyc_Absyn_Exp*_tmp204;void*_tmp205;struct Cyc_Absyn_Vardecl*_tmp206;
void*_tmp207;struct Cyc_Absyn_Vardecl*_tmp208;void*_tmp209;struct Cyc_Absyn_Vardecl*
_tmp20A;void*_tmp20B;struct Cyc_Absyn_Vardecl*_tmp20C;_LL121: if(*((int*)_tmp1F2)
!= 15)goto _LL123;_tmp1F3=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1F2)->f1;
_tmp1F4=((struct Cyc_Absyn_Cast_e_struct*)_tmp1F2)->f2;_LL122: i=_tmp1F4;goto
inner_loop;_LL123: if(*((int*)_tmp1F2)!= 0)goto _LL125;_tmp1F5=((struct Cyc_Absyn_Const_e_struct*)
_tmp1F2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp1F2)->f1).Int_c).tag != 2)
goto _LL125;_tmp1F6=(_tmp1F5.Int_c).f1;if((int)_tmp1F6 != 2)goto _LL125;_tmp1F7=(
_tmp1F5.Int_c).f2;_LL124: _tmp1FA=_tmp1F7;goto _LL126;_LL125: if(*((int*)_tmp1F2)!= 
0)goto _LL127;_tmp1F8=((struct Cyc_Absyn_Const_e_struct*)_tmp1F2)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmp1F2)->f1).Int_c).tag != 2)goto _LL127;_tmp1F9=(
_tmp1F8.Int_c).f1;if((int)_tmp1F9 != 0)goto _LL127;_tmp1FA=(_tmp1F8.Int_c).f2;
_LL126: return _tmp1FA >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp1FA + 1),(
void*)_tmp1F1->type);_LL127: if(*((int*)_tmp1F2)!= 0)goto _LL129;_tmp1FB=((struct
Cyc_Absyn_Const_e_struct*)_tmp1F2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp1F2)->f1).Int_c).tag != 2)goto _LL129;_tmp1FC=(_tmp1FB.Int_c).f1;if((int)
_tmp1FC != 1)goto _LL129;_tmp1FD=(_tmp1FB.Int_c).f2;_LL128: return Cyc_Toc_check_const_array((
unsigned int)(_tmp1FD + 1),(void*)_tmp1F1->type);_LL129: if(*((int*)_tmp1F2)!= 3)
goto _LL12B;_tmp1FE=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp1F2)->f1;if((
int)_tmp1FE != 4)goto _LL12B;_tmp1FF=((struct Cyc_Absyn_Primop_e_struct*)_tmp1F2)->f2;
if(_tmp1FF == 0)goto _LL12B;_tmp200=*_tmp1FF;_tmp201=(struct Cyc_Absyn_Exp*)_tmp200.hd;
_tmp202=_tmp200.tl;if(_tmp202 == 0)goto _LL12B;_tmp203=*_tmp202;_tmp204=(struct Cyc_Absyn_Exp*)
_tmp203.hd;_LL12A: return Cyc_Toc_check_leq_size(relns,_tmp1F1,_tmp204);_LL12B: if(*((
int*)_tmp1F2)!= 1)goto _LL12D;_tmp205=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1F2)->f2;if(_tmp205 <= (void*)1)goto _LL12D;if(*((int*)_tmp205)!= 4)goto _LL12D;
_tmp206=((struct Cyc_Absyn_Pat_b_struct*)_tmp205)->f1;_LL12C: _tmp208=_tmp206;goto
_LL12E;_LL12D: if(*((int*)_tmp1F2)!= 1)goto _LL12F;_tmp207=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1F2)->f2;if(_tmp207 <= (void*)1)goto _LL12F;if(*((int*)_tmp207)!= 3)goto _LL12F;
_tmp208=((struct Cyc_Absyn_Local_b_struct*)_tmp207)->f1;_LL12E: _tmp20A=_tmp208;
goto _LL130;_LL12F: if(*((int*)_tmp1F2)!= 1)goto _LL131;_tmp209=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1F2)->f2;if(_tmp209 <= (void*)1)goto _LL131;if(*((int*)_tmp209)!= 0)goto _LL131;
_tmp20A=((struct Cyc_Absyn_Global_b_struct*)_tmp209)->f1;_LL130: _tmp20C=_tmp20A;
goto _LL132;_LL131: if(*((int*)_tmp1F2)!= 1)goto _LL133;_tmp20B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1F2)->f2;if(_tmp20B <= (void*)1)goto _LL133;if(*((int*)_tmp20B)!= 2)goto _LL133;
_tmp20C=((struct Cyc_Absyn_Param_b_struct*)_tmp20B)->f1;_LL132: if(_tmp20C->escapes)
return 0;{struct Cyc_List_List*_tmp20D=relns;for(0;_tmp20D != 0;_tmp20D=_tmp20D->tl){
struct Cyc_CfFlowInfo_Reln*_tmp20E=(struct Cyc_CfFlowInfo_Reln*)_tmp20D->hd;if(
_tmp20E->vd == _tmp20C){union Cyc_CfFlowInfo_RelnOp_union _tmp20F=_tmp20E->rop;
struct Cyc_Absyn_Vardecl*_tmp210;struct Cyc_Absyn_Vardecl*_tmp211;unsigned int
_tmp212;_LL136: if((_tmp20F.LessSize).tag != 2)goto _LL138;_tmp210=(_tmp20F.LessSize).f1;
_LL137: if(_tmp1F1 == _tmp210)return 1;else{goto _LL135;}_LL138: if((_tmp20F.LessVar).tag
!= 1)goto _LL13A;_tmp211=(_tmp20F.LessVar).f1;_LL139:{struct Cyc_List_List*_tmp213=
relns;for(0;_tmp213 != 0;_tmp213=_tmp213->tl){struct Cyc_CfFlowInfo_Reln*_tmp214=(
struct Cyc_CfFlowInfo_Reln*)_tmp213->hd;if(_tmp214->vd == _tmp211){union Cyc_CfFlowInfo_RelnOp_union
_tmp215=_tmp214->rop;struct Cyc_Absyn_Vardecl*_tmp216;struct Cyc_Absyn_Vardecl*
_tmp217;unsigned int _tmp218;struct Cyc_Absyn_Vardecl*_tmp219;_LL13F: if((_tmp215.LessEqSize).tag
!= 4)goto _LL141;_tmp216=(_tmp215.LessEqSize).f1;_LL140: _tmp217=_tmp216;goto
_LL142;_LL141: if((_tmp215.LessSize).tag != 2)goto _LL143;_tmp217=(_tmp215.LessSize).f1;
_LL142: if(_tmp1F1 == _tmp217)return 1;goto _LL13E;_LL143: if((_tmp215.EqualConst).tag
!= 0)goto _LL145;_tmp218=(_tmp215.EqualConst).f1;_LL144: return Cyc_Toc_check_const_array(
_tmp218,(void*)_tmp1F1->type);_LL145: if((_tmp215.LessVar).tag != 1)goto _LL147;
_tmp219=(_tmp215.LessVar).f1;_LL146: if(Cyc_Toc_check_leq_size_var(relns,_tmp1F1,
_tmp219))return 1;goto _LL13E;_LL147:;_LL148: goto _LL13E;_LL13E:;}}}goto _LL135;
_LL13A: if((_tmp20F.LessConst).tag != 3)goto _LL13C;_tmp212=(_tmp20F.LessConst).f1;
_LL13B: return Cyc_Toc_check_const_array(_tmp212,(void*)_tmp1F1->type);_LL13C:;
_LL13D: goto _LL135;_LL135:;}}}goto _LL120;_LL133:;_LL134: goto _LL120;_LL120:;}goto
_LL115;_LL11E:;_LL11F: goto _LL115;_LL115:;}return 0;}static void*Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp21A[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp21B="Missing type in primop ";_tag_dynforward(_tmp21B,sizeof(char),
_get_zero_arr_size(_tmp21B,24));}),_tag_dynforward(_tmp21A,sizeof(void*),0));});
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*
_tmp21C[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp21D="Missing type in primop ";_tag_dynforward(_tmp21D,sizeof(char),
_get_zero_arr_size(_tmp21D,24));}),_tag_dynforward(_tmp21C,sizeof(void*),0));});
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple4*
Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){return({struct _tuple4*_tmp21E=_cycalloc(
sizeof(*_tmp21E));_tmp21E->f1=Cyc_Toc_mt_tq;_tmp21E->f2=Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_tmp21E;});}static struct _tuple6*
Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);return({struct _tuple6*_tmp21F=_cycalloc(sizeof(*_tmp21F));_tmp21F->f1=0;
_tmp21F->f2=e;_tmp21F;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct
Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,
struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(
pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp220=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp220):
Cyc_Toc_malloc_ptr(_tmp220));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp220);}}}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp221=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp221 != 0;_tmp221=_tmp221->tl){struct _tuple6 _tmp223;struct Cyc_List_List*
_tmp224;struct Cyc_Absyn_Exp*_tmp225;struct _tuple6*_tmp222=(struct _tuple6*)
_tmp221->hd;_tmp223=*_tmp222;_tmp224=_tmp223.f1;_tmp225=_tmp223.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp224 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp224->tl != 0)({void*_tmp226[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp227="multiple designators in array";
_tag_dynforward(_tmp227,sizeof(char),_get_zero_arr_size(_tmp227,30));}),
_tag_dynforward(_tmp226,sizeof(void*),0));});{void*_tmp228=(void*)_tmp224->hd;
void*_tmp229=_tmp228;struct Cyc_Absyn_Exp*_tmp22A;_LL14A: if(*((int*)_tmp229)!= 0)
goto _LL14C;_tmp22A=((struct Cyc_Absyn_ArrayElement_struct*)_tmp229)->f1;_LL14B:
Cyc_Toc_exp_to_c(nv,_tmp22A);e_index=_tmp22A;goto _LL149;_LL14C: if(*((int*)
_tmp229)!= 1)goto _LL149;_LL14D:({void*_tmp22B[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp22C="field name designators in array";
_tag_dynforward(_tmp22C,sizeof(char),_get_zero_arr_size(_tmp22C,32));}),
_tag_dynforward(_tmp22B,sizeof(void*),0));});_LL149:;}}{struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,e_index,0);void*_tmp22D=(void*)_tmp225->r;struct Cyc_List_List*
_tmp22E;struct Cyc_Absyn_Vardecl*_tmp22F;struct Cyc_Absyn_Exp*_tmp230;struct Cyc_Absyn_Exp*
_tmp231;int _tmp232;void*_tmp233;struct Cyc_List_List*_tmp234;_LL14F: if(*((int*)
_tmp22D)!= 28)goto _LL151;_tmp22E=((struct Cyc_Absyn_Array_e_struct*)_tmp22D)->f1;
_LL150: s=Cyc_Toc_init_array(nv,lval,_tmp22E,s);goto _LL14E;_LL151: if(*((int*)
_tmp22D)!= 29)goto _LL153;_tmp22F=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp22D)->f1;_tmp230=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp22D)->f2;
_tmp231=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp22D)->f3;_tmp232=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp22D)->f4;_LL152: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp22F,_tmp230,_tmp231,_tmp232,s,0);goto _LL14E;_LL153: if(*((int*)
_tmp22D)!= 31)goto _LL155;_tmp233=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp22D)->f1;_tmp234=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp22D)->f2;_LL154:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp233,_tmp234,s);goto _LL14E;_LL155:;_LL156:
Cyc_Toc_exp_to_c(nv,_tmp225);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp225,0),s,0);goto _LL14E;_LL14E:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated){struct _tuple1*_tmp235=vd->name;
void*_tmp236=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!e1_already_translated)Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
nv,_tmp235,Cyc_Absyn_varb_exp(_tmp235,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp23F=_cycalloc(sizeof(*_tmp23F));_tmp23F[0]=({struct Cyc_Absyn_Local_b_struct
_tmp240;_tmp240.tag=3;_tmp240.f1=vd;_tmp240;});_tmp23F;}),0));struct _tuple1*max=
Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(
_tmp235,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp(_tmp235,0),Cyc_Absyn_var_exp(max,0),0);struct Cyc_Absyn_Exp*ec=
Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp235,0),0);struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp235,0),0);struct Cyc_Absyn_Stmt*
body;{void*_tmp237=(void*)e2->r;struct Cyc_List_List*_tmp238;struct Cyc_Absyn_Vardecl*
_tmp239;struct Cyc_Absyn_Exp*_tmp23A;struct Cyc_Absyn_Exp*_tmp23B;int _tmp23C;void*
_tmp23D;struct Cyc_List_List*_tmp23E;_LL158: if(*((int*)_tmp237)!= 28)goto _LL15A;
_tmp238=((struct Cyc_Absyn_Array_e_struct*)_tmp237)->f1;_LL159: body=Cyc_Toc_init_array(
nv2,lval,_tmp238,Cyc_Toc_skip_stmt_dl());goto _LL157;_LL15A: if(*((int*)_tmp237)!= 
29)goto _LL15C;_tmp239=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp237)->f1;
_tmp23A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp237)->f2;_tmp23B=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp237)->f3;_tmp23C=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp237)->f4;_LL15B: body=Cyc_Toc_init_comprehension(nv2,lval,_tmp239,_tmp23A,
_tmp23B,_tmp23C,Cyc_Toc_skip_stmt_dl(),0);goto _LL157;_LL15C: if(*((int*)_tmp237)
!= 31)goto _LL15E;_tmp23D=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp237)->f1;
_tmp23E=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp237)->f2;_LL15D: body=Cyc_Toc_init_anon_struct(
nv,lval,_tmp23D,_tmp23E,Cyc_Toc_skip_stmt_dl());goto _LL157;_LL15E:;_LL15F: Cyc_Toc_exp_to_c(
nv2,e2);body=Cyc_Absyn_assign_stmt(lval,e2,0);goto _LL157;_LL157:;}{struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(
Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp236,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp235,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp241=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp241 != 0;_tmp241=_tmp241->tl){struct _tuple6 _tmp243;
struct Cyc_List_List*_tmp244;struct Cyc_Absyn_Exp*_tmp245;struct _tuple6*_tmp242=(
struct _tuple6*)_tmp241->hd;_tmp243=*_tmp242;_tmp244=_tmp243.f1;_tmp245=_tmp243.f2;
if(_tmp244 == 0)({void*_tmp246[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp247="empty designator list";
_tag_dynforward(_tmp247,sizeof(char),_get_zero_arr_size(_tmp247,22));}),
_tag_dynforward(_tmp246,sizeof(void*),0));});if(_tmp244->tl != 0)({void*_tmp248[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp249="too many designators in anonymous struct";_tag_dynforward(
_tmp249,sizeof(char),_get_zero_arr_size(_tmp249,41));}),_tag_dynforward(_tmp248,
sizeof(void*),0));});{void*_tmp24A=(void*)_tmp244->hd;struct _dynforward_ptr*
_tmp24B;_LL161: if(*((int*)_tmp24A)!= 1)goto _LL163;_tmp24B=((struct Cyc_Absyn_FieldName_struct*)
_tmp24A)->f1;_LL162: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,
_tmp24B,0);{void*_tmp24C=(void*)_tmp245->r;struct Cyc_List_List*_tmp24D;struct Cyc_Absyn_Vardecl*
_tmp24E;struct Cyc_Absyn_Exp*_tmp24F;struct Cyc_Absyn_Exp*_tmp250;int _tmp251;void*
_tmp252;struct Cyc_List_List*_tmp253;_LL166: if(*((int*)_tmp24C)!= 28)goto _LL168;
_tmp24D=((struct Cyc_Absyn_Array_e_struct*)_tmp24C)->f1;_LL167: s=Cyc_Toc_init_array(
nv,lval,_tmp24D,s);goto _LL165;_LL168: if(*((int*)_tmp24C)!= 29)goto _LL16A;_tmp24E=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp24C)->f1;_tmp24F=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp24C)->f2;_tmp250=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp24C)->f3;
_tmp251=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp24C)->f4;_LL169: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp24E,_tmp24F,_tmp250,_tmp251,s,0);goto _LL165;_LL16A: if(*((int*)
_tmp24C)!= 31)goto _LL16C;_tmp252=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp24C)->f1;_tmp253=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp24C)->f2;_LL16B:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp252,_tmp253,s);goto _LL165;_LL16C:;_LL16D:
Cyc_Toc_exp_to_c(nv,_tmp245);if(Cyc_Toc_is_poly_field(struct_type,_tmp24B))
_tmp245=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp245);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp245,0),0),s,0);goto _LL165;
_LL165:;}goto _LL160;}_LL163:;_LL164:({void*_tmp254[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp255="array designator in struct";_tag_dynforward(_tmp255,sizeof(char),
_get_zero_arr_size(_tmp255,27));}),_tag_dynforward(_tmp254,sizeof(void*),0));});
_LL160:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct
_RegionHandle _tmp256=_new_region("r");struct _RegionHandle*r=& _tmp256;
_push_region(r);{struct Cyc_List_List*_tmp257=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);struct _dynforward_ptr*_tmp258=Cyc_Toc_add_tuple_type(
_tmp257);struct _tuple1*_tmp259=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp25A=
Cyc_Absyn_var_exp(_tmp259,0);struct Cyc_Absyn_Stmt*_tmp25B=Cyc_Absyn_exp_stmt(
_tmp25A,0);struct Cyc_Absyn_Exp*(*_tmp25C)(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_List_List*_tmp25D=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp25D);for(0;_tmp25D != 0;(_tmp25D=
_tmp25D->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp25D->hd;struct
Cyc_Absyn_Exp*lval=_tmp25C(_tmp25A,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp25E=(void*)e->r;struct Cyc_List_List*_tmp25F;struct Cyc_Absyn_Vardecl*
_tmp260;struct Cyc_Absyn_Exp*_tmp261;struct Cyc_Absyn_Exp*_tmp262;int _tmp263;
_LL16F: if(*((int*)_tmp25E)!= 28)goto _LL171;_tmp25F=((struct Cyc_Absyn_Array_e_struct*)
_tmp25E)->f1;_LL170: _tmp25B=Cyc_Toc_init_array(nv,lval,_tmp25F,_tmp25B);goto
_LL16E;_LL171: if(*((int*)_tmp25E)!= 29)goto _LL173;_tmp260=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp25E)->f1;_tmp261=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp25E)->f2;
_tmp262=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp25E)->f3;_tmp263=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp25E)->f4;_LL172: _tmp25B=Cyc_Toc_init_comprehension(
nv,lval,_tmp260,_tmp261,_tmp262,_tmp263,_tmp25B,0);goto _LL16E;_LL173:;_LL174: Cyc_Toc_exp_to_c(
nv,e);_tmp25B=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp25C(
_tmp25A,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp25B,0);goto _LL16E;_LL16E:;}}}{
struct Cyc_Absyn_Exp*_tmp264=Cyc_Toc_make_struct(nv,_tmp259,Cyc_Absyn_strct(
_tmp258),_tmp25B,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp264;}};
_pop_region(r);}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*
nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct
Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp265=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp266=Cyc_Absyn_var_exp(_tmp265,0);struct Cyc_Absyn_Stmt*
_tmp267=Cyc_Absyn_exp_stmt(_tmp266,0);struct Cyc_Absyn_Exp*(*_tmp268)(struct Cyc_Absyn_Exp*,
struct _dynforward_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*
_tmp269=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp26A;void*
_tmp26B;_LL176: if(_tmp269 <= (void*)4)goto _LL178;if(*((int*)_tmp269)!= 10)goto
_LL178;_tmp26A=((struct Cyc_Absyn_AggrType_struct*)_tmp269)->f1;_tmp26B=(void*)
_tmp26A.aggr_info;_LL177: ad=Cyc_Absyn_get_known_aggrdecl(_tmp26B);goto _LL175;
_LL178:;_LL179:({void*_tmp26C[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp26D="init_struct: bad struct type";
_tag_dynforward(_tmp26D,sizeof(char),_get_zero_arr_size(_tmp26D,29));}),
_tag_dynforward(_tmp26C,sizeof(void*),0));});_LL175:;}{struct _RegionHandle
_tmp26E=_new_region("r");struct _RegionHandle*r=& _tmp26E;_push_region(r);{struct
Cyc_List_List*_tmp26F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rrev)(r,dles);for(0;_tmp26F != 0;_tmp26F=_tmp26F->tl){struct _tuple6
_tmp271;struct Cyc_List_List*_tmp272;struct Cyc_Absyn_Exp*_tmp273;struct _tuple6*
_tmp270=(struct _tuple6*)_tmp26F->hd;_tmp271=*_tmp270;_tmp272=_tmp271.f1;_tmp273=
_tmp271.f2;is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp273->topt))->v);if(_tmp272 == 0)({void*_tmp274[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp275="empty designator list";_tag_dynforward(_tmp275,sizeof(char),
_get_zero_arr_size(_tmp275,22));}),_tag_dynforward(_tmp274,sizeof(void*),0));});
if(_tmp272->tl != 0){struct _tuple1*_tmp276=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp277=Cyc_Absyn_var_exp(_tmp276,0);for(0;_tmp272 != 0;_tmp272=_tmp272->tl){void*
_tmp278=(void*)_tmp272->hd;struct _dynforward_ptr*_tmp279;_LL17B: if(*((int*)
_tmp278)!= 1)goto _LL17D;_tmp279=((struct Cyc_Absyn_FieldName_struct*)_tmp278)->f1;
_LL17C: if(Cyc_Toc_is_poly_field(struct_type,_tmp279))_tmp277=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp277);_tmp267=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp268(_tmp266,_tmp279,0),_tmp277,0),0),_tmp267,0);goto _LL17A;_LL17D:;_LL17E:({
void*_tmp27A[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp27B="array designator in struct";
_tag_dynforward(_tmp27B,sizeof(char),_get_zero_arr_size(_tmp27B,27));}),
_tag_dynforward(_tmp27A,sizeof(void*),0));});_LL17A:;}Cyc_Toc_exp_to_c(nv,
_tmp273);_tmp267=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp277,_tmp273,0),0),_tmp267,0);}else{void*_tmp27C=(void*)_tmp272->hd;struct
_dynforward_ptr*_tmp27D;_LL180: if(*((int*)_tmp27C)!= 1)goto _LL182;_tmp27D=((
struct Cyc_Absyn_FieldName_struct*)_tmp27C)->f1;_LL181: {struct Cyc_Absyn_Exp*lval=
_tmp268(_tmp266,_tmp27D,0);{void*_tmp27E=(void*)_tmp273->r;struct Cyc_List_List*
_tmp27F;struct Cyc_Absyn_Vardecl*_tmp280;struct Cyc_Absyn_Exp*_tmp281;struct Cyc_Absyn_Exp*
_tmp282;int _tmp283;void*_tmp284;struct Cyc_List_List*_tmp285;_LL185: if(*((int*)
_tmp27E)!= 28)goto _LL187;_tmp27F=((struct Cyc_Absyn_Array_e_struct*)_tmp27E)->f1;
_LL186: _tmp267=Cyc_Toc_init_array(nv,lval,_tmp27F,_tmp267);goto _LL184;_LL187: if(*((
int*)_tmp27E)!= 29)goto _LL189;_tmp280=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp27E)->f1;_tmp281=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp27E)->f2;
_tmp282=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp27E)->f3;_tmp283=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp27E)->f4;_LL188: _tmp267=Cyc_Toc_init_comprehension(
nv,lval,_tmp280,_tmp281,_tmp282,_tmp283,_tmp267,0);goto _LL184;_LL189: if(*((int*)
_tmp27E)!= 31)goto _LL18B;_tmp284=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp27E)->f1;_tmp285=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp27E)->f2;_LL18A:
_tmp267=Cyc_Toc_init_anon_struct(nv,lval,_tmp284,_tmp285,_tmp267);goto _LL184;
_LL18B:;_LL18C: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp273->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp273);{struct Cyc_Absyn_Aggrfield*
_tmp286=Cyc_Absyn_lookup_decl_field(ad,_tmp27D);if(Cyc_Toc_is_poly_field(
struct_type,_tmp27D) && !was_ptr_type)_tmp273=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp273);if(has_exists)_tmp273=Cyc_Toc_cast_it((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(_tmp286))->type,_tmp273);_tmp267=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(lval,_tmp273,0),0),_tmp267,0);goto _LL184;}}_LL184:;}goto
_LL17F;}_LL182:;_LL183:({void*_tmp287[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp288="array designator in struct";
_tag_dynforward(_tmp288,sizeof(char),_get_zero_arr_size(_tmp288,27));}),
_tag_dynforward(_tmp287,sizeof(void*),0));});_LL17F:;}}}{struct Cyc_Absyn_Exp*
_tmp289=Cyc_Toc_make_struct(nv,_tmp265,Cyc_Absyn_strctq(tdn),_tmp267,pointer,
rgnopt,is_atomic);_npop_handler(0);return _tmp289;};_pop_region(r);}}struct
_tuple9{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple9*pr){return Cyc_Absyn_assignop_exp(
el,(*pr).f1,(*pr).f2,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct
Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Increment_e_struct*_tmp28A=_cycalloc(sizeof(*_tmp28A));
_tmp28A[0]=({struct Cyc_Absyn_Increment_e_struct _tmp28B;_tmp28B.tag=5;_tmp28B.f1=
e1;_tmp28B.f2=(void*)incr;_tmp28B;});_tmp28A;}),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp28C=(void*)e1->r;struct Cyc_Absyn_Stmt*_tmp28D;void*_tmp28E;struct Cyc_Absyn_Exp*
_tmp28F;struct Cyc_Absyn_Exp*_tmp290;struct _dynforward_ptr*_tmp291;_LL18E: if(*((
int*)_tmp28C)!= 37)goto _LL190;_tmp28D=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp28C)->f1;_LL18F: Cyc_Toc_lvalue_assign_stmt(_tmp28D,fs,f,f_env);goto _LL18D;
_LL190: if(*((int*)_tmp28C)!= 15)goto _LL192;_tmp28E=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp28C)->f1;_tmp28F=((struct Cyc_Absyn_Cast_e_struct*)_tmp28C)->f2;_LL191: Cyc_Toc_lvalue_assign(
_tmp28F,fs,f,f_env);goto _LL18D;_LL192: if(*((int*)_tmp28C)!= 23)goto _LL194;
_tmp290=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp28C)->f1;_tmp291=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp28C)->f2;_LL193:(void*)(e1->r=(void*)((void*)_tmp290->r));Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)({struct Cyc_List_List*_tmp292=_cycalloc(sizeof(*_tmp292));
_tmp292->hd=_tmp291;_tmp292->tl=fs;_tmp292;}),f,f_env);goto _LL18D;_LL194:;_LL195: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dynforward_ptr*)fs->hd,e1_copy->loc);}(
void*)(e1->r=(void*)((void*)(f(e1_copy,f_env))->r));goto _LL18D;}_LL18D:;}static
void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp293=(
void*)s->r;struct Cyc_Absyn_Exp*_tmp294;struct Cyc_Absyn_Decl*_tmp295;struct Cyc_Absyn_Stmt*
_tmp296;struct Cyc_Absyn_Stmt*_tmp297;_LL197: if(_tmp293 <= (void*)1)goto _LL19D;if(*((
int*)_tmp293)!= 0)goto _LL199;_tmp294=((struct Cyc_Absyn_Exp_s_struct*)_tmp293)->f1;
_LL198: Cyc_Toc_lvalue_assign(_tmp294,fs,f,f_env);goto _LL196;_LL199: if(*((int*)
_tmp293)!= 11)goto _LL19B;_tmp295=((struct Cyc_Absyn_Decl_s_struct*)_tmp293)->f1;
_tmp296=((struct Cyc_Absyn_Decl_s_struct*)_tmp293)->f2;_LL19A: Cyc_Toc_lvalue_assign_stmt(
_tmp296,fs,f,f_env);goto _LL196;_LL19B: if(*((int*)_tmp293)!= 1)goto _LL19D;_tmp297=((
struct Cyc_Absyn_Seq_s_struct*)_tmp293)->f2;_LL19C: Cyc_Toc_lvalue_assign_stmt(
_tmp297,fs,f,f_env);goto _LL196;_LL19D:;_LL19E:({struct Cyc_String_pa_struct
_tmp29A;_tmp29A.tag=0;_tmp29A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_stmt2string(s));{void*_tmp298[1]={& _tmp29A};Cyc_Toc_toc_impos(({
const char*_tmp299="lvalue_assign_stmt: %s";_tag_dynforward(_tmp299,sizeof(char),
_get_zero_arr_size(_tmp299,23));}),_tag_dynforward(_tmp298,sizeof(void*),1));}});
_LL196:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;
struct Cyc_List_List*prev;if(x == 0)return 0;result=({struct Cyc_List_List*_tmp29B=
_region_malloc(r2,sizeof(*_tmp29B));_tmp29B->hd=(void*)f((void*)x->hd,env);
_tmp29B->tl=0;_tmp29B;});prev=result;for(x=x->tl;x != 0;x=x->tl){((struct Cyc_List_List*)
_check_null(prev))->tl=({struct Cyc_List_List*_tmp29C=_region_malloc(r2,sizeof(*
_tmp29C));_tmp29C->hd=(void*)f((void*)x->hd,env);_tmp29C->tl=0;_tmp29C;});prev=((
struct Cyc_List_List*)_check_null(prev))->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region,f,env,x);}static struct _tuple6*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){return({struct _tuple6*_tmp29D=_cycalloc(sizeof(*_tmp29D));_tmp29D->f1=0;
_tmp29D->f2=e;_tmp29D;});}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*
t){void*_tmp29E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp29F;_LL1A0: if(
_tmp29E <= (void*)4)goto _LL1A2;if(*((int*)_tmp29E)!= 4)goto _LL1A2;_tmp29F=((
struct Cyc_Absyn_PointerType_struct*)_tmp29E)->f1;_LL1A1: return _tmp29F;_LL1A2:;
_LL1A3:({void*_tmp2A0[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2A1="get_ptr_typ: not a pointer!";
_tag_dynforward(_tmp2A1,sizeof(char),_get_zero_arr_size(_tmp2A1,28));}),
_tag_dynforward(_tmp2A0,sizeof(void*),0));});_LL19F:;}static int Cyc_Toc_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dynforward,int*is_dyneither,void**elt_type){void*
_tmp2A2=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp2A3;void*_tmp2A4;
struct Cyc_Absyn_PtrAtts _tmp2A5;struct Cyc_Absyn_Conref*_tmp2A6;struct Cyc_Absyn_Conref*
_tmp2A7;struct Cyc_Absyn_ArrayInfo _tmp2A8;void*_tmp2A9;struct Cyc_Absyn_Tqual
_tmp2AA;struct Cyc_Absyn_Exp*_tmp2AB;struct Cyc_Absyn_Conref*_tmp2AC;_LL1A5: if(
_tmp2A2 <= (void*)4)goto _LL1A9;if(*((int*)_tmp2A2)!= 4)goto _LL1A7;_tmp2A3=((
struct Cyc_Absyn_PointerType_struct*)_tmp2A2)->f1;_tmp2A4=(void*)_tmp2A3.elt_typ;
_tmp2A5=_tmp2A3.ptr_atts;_tmp2A6=_tmp2A5.bounds;_tmp2A7=_tmp2A5.zero_term;_LL1A6:
if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2A7)){*
ptr_type=t;*elt_type=_tmp2A4;{void*_tmp2AD=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp2A6);_LL1AC: if((int)_tmp2AD != 0)goto _LL1AE;_LL1AD:*is_dyneither=0;*
is_dynforward=1;goto _LL1AB;_LL1AE: if((int)_tmp2AD != 1)goto _LL1B0;_LL1AF:*
is_dynforward=0;*is_dyneither=1;goto _LL1AB;_LL1B0:;_LL1B1:*is_dynforward=(*
is_dyneither=0);goto _LL1AB;_LL1AB:;}return 1;}else{return 0;}_LL1A7: if(*((int*)
_tmp2A2)!= 7)goto _LL1A9;_tmp2A8=((struct Cyc_Absyn_ArrayType_struct*)_tmp2A2)->f1;
_tmp2A9=(void*)_tmp2A8.elt_type;_tmp2AA=_tmp2A8.tq;_tmp2AB=_tmp2A8.num_elts;
_tmp2AC=_tmp2A8.zero_term;_LL1A8: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp2AC)){*elt_type=_tmp2A9;*is_dynforward=(*is_dyneither=0);*ptr_type=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp2AE=_cycalloc(sizeof(*_tmp2AE));_tmp2AE[0]=({
struct Cyc_Absyn_PointerType_struct _tmp2AF;_tmp2AF.tag=4;_tmp2AF.f1=({struct Cyc_Absyn_PtrInfo
_tmp2B0;_tmp2B0.elt_typ=(void*)_tmp2A9;_tmp2B0.elt_tq=_tmp2AA;_tmp2B0.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmp2B1;_tmp2B1.rgn=(void*)((void*)2);_tmp2B1.nullable=
Cyc_Absyn_false_conref;_tmp2B1.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp2B2=_cycalloc(sizeof(*_tmp2B2));_tmp2B2[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp2B3;_tmp2B3.tag=0;_tmp2B3.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp2AB);
_tmp2B3;});_tmp2B2;}));_tmp2B1.zero_term=_tmp2AC;_tmp2B1.ptrloc=0;_tmp2B1;});
_tmp2B0;});_tmp2AF;});_tmp2AE;});return 1;}else{return 0;}_LL1A9:;_LL1AA: return 0;
_LL1A4:;}static int Cyc_Toc_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dynforward,int*is_dyneither,void**elt_type){void*_tmp2B4=(void*)
e1->r;struct Cyc_Absyn_Exp*_tmp2B5;struct Cyc_Absyn_Exp*_tmp2B6;struct Cyc_Absyn_Exp*
_tmp2B7;struct Cyc_Absyn_Exp*_tmp2B8;struct Cyc_Absyn_Exp*_tmp2B9;struct Cyc_Absyn_Exp*
_tmp2BA;_LL1B3: if(*((int*)_tmp2B4)!= 15)goto _LL1B5;_LL1B4:({struct Cyc_String_pa_struct
_tmp2BD;_tmp2BD.tag=0;_tmp2BD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e1));{void*_tmp2BB[1]={& _tmp2BD};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp2BC="we have a cast in a lhs:  %s";_tag_dynforward(_tmp2BC,sizeof(char),
_get_zero_arr_size(_tmp2BC,29));}),_tag_dynforward(_tmp2BB,sizeof(void*),1));}});
_LL1B5: if(*((int*)_tmp2B4)!= 22)goto _LL1B7;_tmp2B5=((struct Cyc_Absyn_Deref_e_struct*)
_tmp2B4)->f1;_LL1B6: _tmp2B6=_tmp2B5;goto _LL1B8;_LL1B7: if(*((int*)_tmp2B4)!= 25)
goto _LL1B9;_tmp2B6=((struct Cyc_Absyn_Subscript_e_struct*)_tmp2B4)->f1;_LL1B8:
return Cyc_Toc_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2B6->topt))->v,
ptr_type,is_dynforward,is_dyneither,elt_type);_LL1B9: if(*((int*)_tmp2B4)!= 24)
goto _LL1BB;_tmp2B7=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2B4)->f1;_LL1BA:
_tmp2B8=_tmp2B7;goto _LL1BC;_LL1BB: if(*((int*)_tmp2B4)!= 23)goto _LL1BD;_tmp2B8=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp2B4)->f1;_LL1BC: if(Cyc_Toc_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp2B8->topt))->v,ptr_type,
is_dynforward,is_dyneither,elt_type))({struct Cyc_String_pa_struct _tmp2C0;_tmp2C0.tag=
0;_tmp2C0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e1));{void*_tmp2BE[1]={& _tmp2C0};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2BF="found zero pointer aggregate member assignment: %s";
_tag_dynforward(_tmp2BF,sizeof(char),_get_zero_arr_size(_tmp2BF,51));}),
_tag_dynforward(_tmp2BE,sizeof(void*),1));}});return 0;_LL1BD: if(*((int*)_tmp2B4)
!= 14)goto _LL1BF;_tmp2B9=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp2B4)->f1;
_LL1BE: _tmp2BA=_tmp2B9;goto _LL1C0;_LL1BF: if(*((int*)_tmp2B4)!= 13)goto _LL1C1;
_tmp2BA=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp2B4)->f1;_LL1C0: if(Cyc_Toc_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp2BA->topt))->v,ptr_type,
is_dynforward,is_dyneither,elt_type))({struct Cyc_String_pa_struct _tmp2C3;_tmp2C3.tag=
0;_tmp2C3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e1));{void*_tmp2C1[1]={& _tmp2C3};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2C2="found zero pointer instantiate/noinstantiate: %s";
_tag_dynforward(_tmp2C2,sizeof(char),_get_zero_arr_size(_tmp2C2,49));}),
_tag_dynforward(_tmp2C1,sizeof(void*),1));}});return 0;_LL1C1: if(*((int*)_tmp2B4)
!= 1)goto _LL1C3;_LL1C2: return 0;_LL1C3:;_LL1C4:({struct Cyc_String_pa_struct
_tmp2C6;_tmp2C6.tag=0;_tmp2C6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e1));{void*_tmp2C4[1]={& _tmp2C6};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp2C5="found bad lhs in is_zero_ptr_deref: %s";_tag_dynforward(_tmp2C5,sizeof(
char),_get_zero_arr_size(_tmp2C5,39));}),_tag_dynforward(_tmp2C4,sizeof(void*),1));}});
_LL1B2:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp2C7=Cyc_Tcutil_compress(t);void*_tmp2C8;void*_tmp2C9;void*_tmp2CA;
void*_tmp2CB;void*_tmp2CC;void*_tmp2CD;void*_tmp2CE;void*_tmp2CF;void*_tmp2D0;
void*_tmp2D1;_LL1C6: if(_tmp2C7 <= (void*)4)goto _LL1D6;if(*((int*)_tmp2C7)!= 4)
goto _LL1C8;_LL1C7: res=Cyc_Absyn_null_exp(0);goto _LL1C5;_LL1C8: if(*((int*)_tmp2C7)
!= 5)goto _LL1CA;_tmp2C8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2C7)->f1;
_tmp2C9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2C7)->f2;if((int)_tmp2C9 != 
0)goto _LL1CA;_LL1C9: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union
Cyc_Absyn_Cnst_union _tmp2D2;(_tmp2D2.Char_c).tag=0;(_tmp2D2.Char_c).f1=(void*)
_tmp2C8;(_tmp2D2.Char_c).f2='\000';_tmp2D2;}),0);goto _LL1C5;_LL1CA: if(*((int*)
_tmp2C7)!= 5)goto _LL1CC;_tmp2CA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2C7)->f1;
_tmp2CB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2C7)->f2;if((int)_tmp2CB != 
1)goto _LL1CC;_LL1CB: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union
Cyc_Absyn_Cnst_union _tmp2D3;(_tmp2D3.Short_c).tag=1;(_tmp2D3.Short_c).f1=(void*)
_tmp2CA;(_tmp2D3.Short_c).f2=0;_tmp2D3;}),0);goto _LL1C5;_LL1CC: if(*((int*)
_tmp2C7)!= 12)goto _LL1CE;_LL1CD: goto _LL1CF;_LL1CE: if(*((int*)_tmp2C7)!= 13)goto
_LL1D0;_LL1CF: _tmp2CC=(void*)1;goto _LL1D1;_LL1D0: if(*((int*)_tmp2C7)!= 5)goto
_LL1D2;_tmp2CC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2C7)->f1;_tmp2CD=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2C7)->f2;if((int)_tmp2CD != 2)goto
_LL1D2;_LL1D1: _tmp2CE=_tmp2CC;goto _LL1D3;_LL1D2: if(*((int*)_tmp2C7)!= 5)goto
_LL1D4;_tmp2CE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2C7)->f1;_tmp2CF=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2C7)->f2;if((int)_tmp2CF != 3)goto
_LL1D4;_LL1D3: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp2D4;(_tmp2D4.Int_c).tag=2;(_tmp2D4.Int_c).f1=(void*)_tmp2CE;(_tmp2D4.Int_c).f2=
0;_tmp2D4;}),0);goto _LL1C5;_LL1D4: if(*((int*)_tmp2C7)!= 5)goto _LL1D6;_tmp2D0=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2C7)->f1;_tmp2D1=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2C7)->f2;if((int)_tmp2D1 != 4)goto _LL1D6;_LL1D5: res=Cyc_Absyn_const_exp((
union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp2D5;(_tmp2D5.LongLong_c).tag=
3;(_tmp2D5.LongLong_c).f1=(void*)_tmp2D0;(_tmp2D5.LongLong_c).f2=(long long)0;
_tmp2D5;}),0);goto _LL1C5;_LL1D6: if((int)_tmp2C7 != 1)goto _LL1D8;_LL1D7: goto _LL1D9;
_LL1D8: if(_tmp2C7 <= (void*)4)goto _LL1DA;if(*((int*)_tmp2C7)!= 6)goto _LL1DA;
_LL1D9: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp2D6;(_tmp2D6.Float_c).tag=4;(_tmp2D6.Float_c).f1=({const char*_tmp2D7="0.0";
_tag_dynforward(_tmp2D7,sizeof(char),_get_zero_arr_size(_tmp2D7,4));});_tmp2D6;}),
0);goto _LL1C5;_LL1DA:;_LL1DB:({struct Cyc_String_pa_struct _tmp2DA;_tmp2DA.tag=0;
_tmp2DA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp2D8[1]={& _tmp2DA};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2D9="found non-zero type %s in generate_zero";
_tag_dynforward(_tmp2D9,sizeof(char),_get_zero_arr_size(_tmp2D9,40));}),
_tag_dynforward(_tmp2D8,sizeof(void*),1));}});_LL1C5:;}res->topt=({struct Cyc_Core_Opt*
_tmp2DB=_cycalloc(sizeof(*_tmp2DB));_tmp2DB->v=(void*)t;_tmp2DB;});return res;}
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dynforward,int is_dyneither,void*elt_type){int is_fat=is_dynforward
 || is_dyneither;void*fat_ptr_type;if(is_dynforward)fat_ptr_type=Cyc_Absyn_dynforward_typ(
elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);else{fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);}{void*_tmp2DC=Cyc_Toc_typ_to_c(
elt_type);void*_tmp2DD=Cyc_Toc_typ_to_c(fat_ptr_type);void*_tmp2DE=Cyc_Absyn_cstar_typ(
_tmp2DC,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*_tmp2DF=({struct Cyc_Core_Opt*_tmp31B=
_cycalloc(sizeof(*_tmp31B));_tmp31B->v=(void*)_tmp2DE;_tmp31B;});struct Cyc_Absyn_Exp*
xinit;{void*_tmp2E0=(void*)e1->r;struct Cyc_Absyn_Exp*_tmp2E1;struct Cyc_Absyn_Exp*
_tmp2E2;struct Cyc_Absyn_Exp*_tmp2E3;_LL1DD: if(*((int*)_tmp2E0)!= 22)goto _LL1DF;
_tmp2E1=((struct Cyc_Absyn_Deref_e_struct*)_tmp2E0)->f1;_LL1DE: if(!is_fat){
_tmp2E1=Cyc_Toc_cast_it(fat_ptr_type,_tmp2E1);_tmp2E1->topt=({struct Cyc_Core_Opt*
_tmp2E4=_cycalloc(sizeof(*_tmp2E4));_tmp2E4->v=(void*)fat_ptr_type;_tmp2E4;});}
Cyc_Toc_exp_to_c(nv,_tmp2E1);xinit=_tmp2E1;goto _LL1DC;_LL1DF: if(*((int*)_tmp2E0)
!= 25)goto _LL1E1;_tmp2E2=((struct Cyc_Absyn_Subscript_e_struct*)_tmp2E0)->f1;
_tmp2E3=((struct Cyc_Absyn_Subscript_e_struct*)_tmp2E0)->f2;_LL1E0: if(!is_fat){
_tmp2E2=Cyc_Toc_cast_it(fat_ptr_type,_tmp2E2);_tmp2E2->topt=({struct Cyc_Core_Opt*
_tmp2E5=_cycalloc(sizeof(*_tmp2E5));_tmp2E5->v=(void*)fat_ptr_type;_tmp2E5;});}
Cyc_Toc_exp_to_c(nv,_tmp2E2);Cyc_Toc_exp_to_c(nv,_tmp2E3);if(is_dynforward)xinit=
Cyc_Absyn_fncall_exp(Cyc_Toc__dynforward_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp2E6[3];_tmp2E6[2]=_tmp2E3;_tmp2E6[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp2E6[0]=_tmp2E2;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp2E6,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
else{xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp2E7[3];_tmp2E7[2]=_tmp2E3;_tmp2E7[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp2E7[0]=_tmp2E2;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp2E7,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}
goto _LL1DC;_LL1E1:;_LL1E2:({void*_tmp2E8[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2E9="found bad lhs for zero-terminated pointer assignment";
_tag_dynforward(_tmp2E9,sizeof(char),_get_zero_arr_size(_tmp2E9,53));}),
_tag_dynforward(_tmp2E8,sizeof(void*),0));});_LL1DC:;}{struct _tuple1*_tmp2EA=Cyc_Toc_temp_var();
nv=Cyc_Toc_add_varmap(nv,_tmp2EA,Cyc_Absyn_var_exp(_tmp2EA,0));{struct Cyc_Absyn_Vardecl*
_tmp2EB=({struct Cyc_Absyn_Vardecl*_tmp31A=_cycalloc(sizeof(*_tmp31A));_tmp31A->sc=(
void*)((void*)2);_tmp31A->name=_tmp2EA;_tmp31A->tq=Cyc_Toc_mt_tq;_tmp31A->type=(
void*)_tmp2DD;_tmp31A->initializer=(struct Cyc_Absyn_Exp*)xinit;_tmp31A->rgn=0;
_tmp31A->attributes=0;_tmp31A->escapes=0;_tmp31A;});struct Cyc_Absyn_Local_b_struct*
_tmp2EC=({struct Cyc_Absyn_Local_b_struct*_tmp318=_cycalloc(sizeof(*_tmp318));
_tmp318[0]=({struct Cyc_Absyn_Local_b_struct _tmp319;_tmp319.tag=3;_tmp319.f1=
_tmp2EB;_tmp319;});_tmp318;});struct Cyc_Absyn_Exp*_tmp2ED=Cyc_Absyn_varb_exp(
_tmp2EA,(void*)_tmp2EC,0);_tmp2ED->topt=({struct Cyc_Core_Opt*_tmp2EE=_cycalloc(
sizeof(*_tmp2EE));_tmp2EE->v=(void*)fat_ptr_type;_tmp2EE;});{struct Cyc_Absyn_Exp*
_tmp2EF=Cyc_Absyn_deref_exp(_tmp2ED,0);_tmp2EF->topt=({struct Cyc_Core_Opt*
_tmp2F0=_cycalloc(sizeof(*_tmp2F0));_tmp2F0->v=(void*)elt_type;_tmp2F0;});Cyc_Toc_exp_to_c(
nv,_tmp2EF);{struct _tuple1*_tmp2F1=Cyc_Toc_temp_var();nv=Cyc_Toc_add_varmap(nv,
_tmp2F1,Cyc_Absyn_var_exp(_tmp2F1,0));{struct Cyc_Absyn_Vardecl*_tmp2F2=({struct
Cyc_Absyn_Vardecl*_tmp317=_cycalloc(sizeof(*_tmp317));_tmp317->sc=(void*)((void*)
2);_tmp317->name=_tmp2F1;_tmp317->tq=Cyc_Toc_mt_tq;_tmp317->type=(void*)_tmp2DC;
_tmp317->initializer=(struct Cyc_Absyn_Exp*)_tmp2EF;_tmp317->rgn=0;_tmp317->attributes=
0;_tmp317->escapes=0;_tmp317;});struct Cyc_Absyn_Local_b_struct*_tmp2F3=({struct
Cyc_Absyn_Local_b_struct*_tmp315=_cycalloc(sizeof(*_tmp315));_tmp315[0]=({struct
Cyc_Absyn_Local_b_struct _tmp316;_tmp316.tag=3;_tmp316.f1=_tmp2F2;_tmp316;});
_tmp315;});struct Cyc_Absyn_Exp*z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*
_tmp2F4=Cyc_Absyn_varb_exp(_tmp2F1,(void*)_tmp2F3,0);_tmp2F4->topt=_tmp2EF->topt;
z_init=Cyc_Absyn_prim2_exp((void*)popt->v,_tmp2F4,e2,0);z_init->topt=_tmp2F4->topt;}
Cyc_Toc_exp_to_c(nv,z_init);{struct _tuple1*_tmp2F5=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmp2F6=({struct Cyc_Absyn_Vardecl*_tmp314=_cycalloc(sizeof(*_tmp314));_tmp314->sc=(
void*)((void*)2);_tmp314->name=_tmp2F5;_tmp314->tq=Cyc_Toc_mt_tq;_tmp314->type=(
void*)_tmp2DC;_tmp314->initializer=(struct Cyc_Absyn_Exp*)z_init;_tmp314->rgn=0;
_tmp314->attributes=0;_tmp314->escapes=0;_tmp314;});struct Cyc_Absyn_Local_b_struct*
_tmp2F7=({struct Cyc_Absyn_Local_b_struct*_tmp312=_cycalloc(sizeof(*_tmp312));
_tmp312[0]=({struct Cyc_Absyn_Local_b_struct _tmp313;_tmp313.tag=3;_tmp313.f1=
_tmp2F6;_tmp313;});_tmp312;});nv=Cyc_Toc_add_varmap(nv,_tmp2F5,Cyc_Absyn_var_exp(
_tmp2F5,0));{struct Cyc_Absyn_Exp*_tmp2F8=Cyc_Absyn_varb_exp(_tmp2F1,(void*)
_tmp2F3,0);_tmp2F8->topt=_tmp2EF->topt;{struct Cyc_Absyn_Exp*_tmp2F9=Cyc_Toc_generate_zero(
elt_type);struct Cyc_Absyn_Exp*_tmp2FA=Cyc_Absyn_prim2_exp((void*)5,_tmp2F8,
_tmp2F9,0);_tmp2FA->topt=({struct Cyc_Core_Opt*_tmp2FB=_cycalloc(sizeof(*_tmp2FB));
_tmp2FB->v=(void*)Cyc_Absyn_sint_typ;_tmp2FB;});Cyc_Toc_exp_to_c(nv,_tmp2FA);{
struct Cyc_Absyn_Exp*_tmp2FC=Cyc_Absyn_varb_exp(_tmp2F5,(void*)_tmp2F7,0);_tmp2FC->topt=
_tmp2EF->topt;{struct Cyc_Absyn_Exp*_tmp2FD=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2FE=Cyc_Absyn_prim2_exp((void*)6,_tmp2FC,_tmp2FD,0);
_tmp2FE->topt=({struct Cyc_Core_Opt*_tmp2FF=_cycalloc(sizeof(*_tmp2FF));_tmp2FF->v=(
void*)Cyc_Absyn_sint_typ;_tmp2FF;});Cyc_Toc_exp_to_c(nv,_tmp2FE);{struct Cyc_List_List*
_tmp300=({struct Cyc_Absyn_Exp*_tmp311[2];_tmp311[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp311[0]=Cyc_Absyn_varb_exp(_tmp2EA,(void*)_tmp2EC,0);((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp311,sizeof(struct Cyc_Absyn_Exp*),
2));});struct Cyc_Absyn_Exp*_tmp301=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*
xsize;if(is_dynforward)xsize=Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(
Cyc_Toc__get_dynforward_size_e,_tmp300,0),_tmp301,0);else{xsize=Cyc_Absyn_prim2_exp((
void*)5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp300,0),_tmp301,0);}{
struct Cyc_Absyn_Exp*_tmp302=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp2FA,
_tmp2FE,0),0);struct Cyc_Absyn_Stmt*_tmp303=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp304=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp2EA,(void*)_tmp2EC,0),Cyc_Toc_curr_sp,0);_tmp304=Cyc_Toc_cast_it(
_tmp2DE,_tmp304);{struct Cyc_Absyn_Exp*_tmp305=Cyc_Absyn_deref_exp(_tmp304,0);
struct Cyc_Absyn_Exp*_tmp306=Cyc_Absyn_assign_exp(_tmp305,Cyc_Absyn_var_exp(
_tmp2F5,0),0);struct Cyc_Absyn_Stmt*_tmp307=Cyc_Absyn_exp_stmt(_tmp306,0);_tmp307=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp302,_tmp303,Cyc_Absyn_skip_stmt(
0),0),_tmp307,0);_tmp307=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp308=
_cycalloc(sizeof(*_tmp308));_tmp308->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp309=_cycalloc(sizeof(*_tmp309));_tmp309[0]=({struct Cyc_Absyn_Var_d_struct
_tmp30A;_tmp30A.tag=0;_tmp30A.f1=_tmp2F6;_tmp30A;});_tmp309;}));_tmp308->loc=0;
_tmp308;}),_tmp307,0);_tmp307=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp30B=
_cycalloc(sizeof(*_tmp30B));_tmp30B->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp30C=_cycalloc(sizeof(*_tmp30C));_tmp30C[0]=({struct Cyc_Absyn_Var_d_struct
_tmp30D;_tmp30D.tag=0;_tmp30D.f1=_tmp2F2;_tmp30D;});_tmp30C;}));_tmp30B->loc=0;
_tmp30B;}),_tmp307,0);_tmp307=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp30E=
_cycalloc(sizeof(*_tmp30E));_tmp30E->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp30F=_cycalloc(sizeof(*_tmp30F));_tmp30F[0]=({struct Cyc_Absyn_Var_d_struct
_tmp310;_tmp310.tag=0;_tmp310.f1=_tmp2EB;_tmp310;});_tmp30F;}));_tmp30E->loc=0;
_tmp30E;}),_tmp307,0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(_tmp307));}}}}}}}}}}}}}}}
struct _tuple10{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple11{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){void*_tmp31C=(void*)e->r;if(e->topt
== 0)({struct Cyc_String_pa_struct _tmp31F;_tmp31F.tag=0;_tmp31F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{void*_tmp31D[
1]={& _tmp31F};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp31E="exp_to_c: no type for %s";_tag_dynforward(_tmp31E,sizeof(char),
_get_zero_arr_size(_tmp31E,25));}),_tag_dynforward(_tmp31D,sizeof(void*),1));}});{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp320=
_tmp31C;union Cyc_Absyn_Cnst_union _tmp321;struct _tuple1*_tmp322;void*_tmp323;
struct _tuple1*_tmp324;void*_tmp325;struct Cyc_List_List*_tmp326;struct Cyc_Absyn_Exp*
_tmp327;void*_tmp328;struct Cyc_Absyn_Exp*_tmp329;struct Cyc_Core_Opt*_tmp32A;
struct Cyc_Absyn_Exp*_tmp32B;struct Cyc_Absyn_Exp*_tmp32C;struct Cyc_Absyn_Exp*
_tmp32D;struct Cyc_Absyn_Exp*_tmp32E;struct Cyc_Absyn_Exp*_tmp32F;struct Cyc_Absyn_Exp*
_tmp330;struct Cyc_Absyn_Exp*_tmp331;struct Cyc_Absyn_Exp*_tmp332;struct Cyc_Absyn_Exp*
_tmp333;struct Cyc_Absyn_Exp*_tmp334;struct Cyc_Absyn_Exp*_tmp335;struct Cyc_List_List*
_tmp336;struct Cyc_Absyn_Exp*_tmp337;struct Cyc_List_List*_tmp338;struct Cyc_Absyn_VarargCallInfo*
_tmp339;struct Cyc_Absyn_Exp*_tmp33A;struct Cyc_List_List*_tmp33B;struct Cyc_Absyn_VarargCallInfo*
_tmp33C;struct Cyc_Absyn_VarargCallInfo _tmp33D;int _tmp33E;struct Cyc_List_List*
_tmp33F;struct Cyc_Absyn_VarargInfo*_tmp340;struct Cyc_Absyn_Exp*_tmp341;struct Cyc_Absyn_Exp*
_tmp342;struct Cyc_Absyn_Exp*_tmp343;struct Cyc_List_List*_tmp344;void*_tmp345;
void**_tmp346;struct Cyc_Absyn_Exp*_tmp347;int _tmp348;void*_tmp349;struct Cyc_Absyn_Exp*
_tmp34A;struct Cyc_Absyn_Exp*_tmp34B;struct Cyc_Absyn_Exp*_tmp34C;struct Cyc_Absyn_Exp*
_tmp34D;void*_tmp34E;void*_tmp34F;void*_tmp350;struct _dynforward_ptr*_tmp351;
void*_tmp352;void*_tmp353;unsigned int _tmp354;struct Cyc_Absyn_Exp*_tmp355;struct
Cyc_Absyn_Exp*_tmp356;struct _dynforward_ptr*_tmp357;struct Cyc_Absyn_Exp*_tmp358;
struct _dynforward_ptr*_tmp359;struct Cyc_Absyn_Exp*_tmp35A;struct Cyc_Absyn_Exp*
_tmp35B;struct Cyc_List_List*_tmp35C;struct Cyc_List_List*_tmp35D;struct Cyc_Absyn_Vardecl*
_tmp35E;struct Cyc_Absyn_Exp*_tmp35F;struct Cyc_Absyn_Exp*_tmp360;int _tmp361;
struct _tuple1*_tmp362;struct Cyc_List_List*_tmp363;struct Cyc_List_List*_tmp364;
struct Cyc_Absyn_Aggrdecl*_tmp365;void*_tmp366;struct Cyc_List_List*_tmp367;struct
Cyc_List_List*_tmp368;struct Cyc_Absyn_Tuniondecl*_tmp369;struct Cyc_Absyn_Tunionfield*
_tmp36A;struct Cyc_List_List*_tmp36B;struct Cyc_Absyn_Tuniondecl*_tmp36C;struct Cyc_Absyn_Tunionfield*
_tmp36D;struct Cyc_Absyn_MallocInfo _tmp36E;int _tmp36F;struct Cyc_Absyn_Exp*_tmp370;
void**_tmp371;struct Cyc_Absyn_Exp*_tmp372;int _tmp373;struct Cyc_Absyn_Stmt*
_tmp374;_LL1E4: if(*((int*)_tmp320)!= 0)goto _LL1E6;_tmp321=((struct Cyc_Absyn_Const_e_struct*)
_tmp320)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp320)->f1).Null_c).tag != 
6)goto _LL1E6;_LL1E5: {struct Cyc_Absyn_Exp*_tmp375=Cyc_Absyn_uint_exp(0,0);int
forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ,& forward_only)){if(nv->toplevel)(
void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp375,
_tmp375,forward_only))->r));else{if(forward_only)(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*_tmp376[3];_tmp376[2]=_tmp375;
_tmp376[1]=_tmp375;_tmp376[0]=_tmp375;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp376,sizeof(struct Cyc_Absyn_Exp*),
3));})));else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,({
struct Cyc_Absyn_Exp*_tmp377[3];_tmp377[2]=_tmp375;_tmp377[1]=_tmp375;_tmp377[0]=
_tmp375;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp377,sizeof(struct Cyc_Absyn_Exp*),3));})));}}}else{(void*)(e->r=(
void*)((void*)& Cyc_Toc_zero_exp));}goto _LL1E3;}_LL1E6: if(*((int*)_tmp320)!= 0)
goto _LL1E8;_LL1E7: goto _LL1E3;_LL1E8: if(*((int*)_tmp320)!= 1)goto _LL1EA;_tmp322=((
struct Cyc_Absyn_Var_e_struct*)_tmp320)->f1;_tmp323=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp320)->f2;_LL1E9:{struct _handler_cons _tmp378;_push_handler(& _tmp378);{int
_tmp37A=0;if(setjmp(_tmp378.handler))_tmp37A=1;if(!_tmp37A){(void*)(e->r=(void*)((
void*)(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup)(
nv->varmap,_tmp322))->r));;_pop_handler();}else{void*_tmp379=(void*)_exn_thrown;
void*_tmp37C=_tmp379;_LL239: if(_tmp37C != Cyc_Dict_Absent)goto _LL23B;_LL23A:({
struct Cyc_String_pa_struct _tmp37F;_tmp37F.tag=0;_tmp37F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp322));{void*
_tmp37D[1]={& _tmp37F};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp37E="Can't find %s in exp_to_c, Var\n";
_tag_dynforward(_tmp37E,sizeof(char),_get_zero_arr_size(_tmp37E,32));}),
_tag_dynforward(_tmp37D,sizeof(void*),1));}});_LL23B:;_LL23C:(void)_throw(
_tmp37C);_LL238:;}}}goto _LL1E3;_LL1EA: if(*((int*)_tmp320)!= 2)goto _LL1EC;_tmp324=((
struct Cyc_Absyn_UnknownId_e_struct*)_tmp320)->f1;_LL1EB:({void*_tmp380[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp381="unknownid";_tag_dynforward(_tmp381,sizeof(char),
_get_zero_arr_size(_tmp381,10));}),_tag_dynforward(_tmp380,sizeof(void*),0));});
_LL1EC: if(*((int*)_tmp320)!= 3)goto _LL1EE;_tmp325=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp320)->f1;_tmp326=((struct Cyc_Absyn_Primop_e_struct*)_tmp320)->f2;_LL1ED: {
struct Cyc_List_List*_tmp382=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp326);((void(*)(void(*
f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp326);{void*_tmp383=_tmp325;_LL23E:
if((int)_tmp383 != 19)goto _LL240;_LL23F: {struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp326))->hd;{void*_tmp384=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo
_tmp385;struct Cyc_Absyn_Exp*_tmp386;struct Cyc_Absyn_PtrInfo _tmp387;void*_tmp388;
struct Cyc_Absyn_PtrAtts _tmp389;struct Cyc_Absyn_Conref*_tmp38A;struct Cyc_Absyn_Conref*
_tmp38B;struct Cyc_Absyn_Conref*_tmp38C;_LL253: if(_tmp384 <= (void*)4)goto _LL257;
if(*((int*)_tmp384)!= 7)goto _LL255;_tmp385=((struct Cyc_Absyn_ArrayType_struct*)
_tmp384)->f1;_tmp386=_tmp385.num_elts;_LL254:(void*)(e->r=(void*)((void*)((
struct Cyc_Absyn_Exp*)_check_null(_tmp386))->r));goto _LL252;_LL255: if(*((int*)
_tmp384)!= 4)goto _LL257;_tmp387=((struct Cyc_Absyn_PointerType_struct*)_tmp384)->f1;
_tmp388=(void*)_tmp387.elt_typ;_tmp389=_tmp387.ptr_atts;_tmp38A=_tmp389.nullable;
_tmp38B=_tmp389.bounds;_tmp38C=_tmp389.zero_term;_LL256:{void*_tmp38D=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp38B);struct Cyc_Absyn_Exp*_tmp38E;_LL25A: if((int)_tmp38D
!= 0)goto _LL25C;_LL25B:(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dynforward_size_e,({
struct Cyc_Absyn_Exp*_tmp38F[2];_tmp38F[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp388),0);_tmp38F[0]=(struct Cyc_Absyn_Exp*)_tmp326->hd;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp38F,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL259;_LL25C: if((int)_tmp38D != 1)goto _LL25E;_LL25D:(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,({struct Cyc_Absyn_Exp*
_tmp390[2];_tmp390[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp388),0);
_tmp390[0]=(struct Cyc_Absyn_Exp*)_tmp326->hd;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp390,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL259;_LL25E: if(_tmp38D <= (void*)2)goto _LL260;if(*((int*)_tmp38D)
!= 0)goto _LL260;_tmp38E=((struct Cyc_Absyn_Upper_b_struct*)_tmp38D)->f1;_LL25F:
if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp38C))(void*)(
e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_zero_arr_size_e,({struct Cyc_Absyn_Exp*
_tmp391[2];_tmp391[1]=_tmp38E;_tmp391[0]=(struct Cyc_Absyn_Exp*)_tmp326->hd;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp391,sizeof(struct Cyc_Absyn_Exp*),2));})));else{if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp38A))(void*)(e->r=(void*)Cyc_Toc_conditional_exp_r(
arg,_tmp38E,Cyc_Absyn_uint_exp(0,0)));else{(void*)(e->r=(void*)((void*)_tmp38E->r));
goto _LL259;}}goto _LL259;_LL260: if(_tmp38D <= (void*)2)goto _LL259;if(*((int*)
_tmp38D)!= 1)goto _LL259;_LL261:({void*_tmp392[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp393="toc: size of AbsUpper_b";
_tag_dynforward(_tmp393,sizeof(char),_get_zero_arr_size(_tmp393,24));}),
_tag_dynforward(_tmp392,sizeof(void*),0));});_LL259:;}goto _LL252;_LL257:;_LL258:({
struct Cyc_String_pa_struct _tmp397;_tmp397.tag=0;_tmp397.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(arg->topt))->v));{struct Cyc_String_pa_struct _tmp396;
_tmp396.tag=0;_tmp396.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{void*_tmp394[2]={&
_tmp396,& _tmp397};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp395="size primop applied to non-array %s (%s)";_tag_dynforward(
_tmp395,sizeof(char),_get_zero_arr_size(_tmp395,41));}),_tag_dynforward(_tmp394,
sizeof(void*),2));}}});_LL252:;}goto _LL23D;}_LL240: if((int)_tmp383 != 0)goto
_LL242;_LL241:{void*_tmp398=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)
_check_null(_tmp382))->hd);struct Cyc_Absyn_PtrInfo _tmp399;void*_tmp39A;struct Cyc_Absyn_PtrAtts
_tmp39B;struct Cyc_Absyn_Conref*_tmp39C;struct Cyc_Absyn_Conref*_tmp39D;_LL263: if(
_tmp398 <= (void*)4)goto _LL265;if(*((int*)_tmp398)!= 4)goto _LL265;_tmp399=((
struct Cyc_Absyn_PointerType_struct*)_tmp398)->f1;_tmp39A=(void*)_tmp399.elt_typ;
_tmp39B=_tmp399.ptr_atts;_tmp39C=_tmp39B.bounds;_tmp39D=_tmp39B.zero_term;_LL264:{
void*_tmp39E=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp39C);struct Cyc_Absyn_Exp*
_tmp39F;_LL268: if((int)_tmp39E != 0)goto _LL26A;_LL269: {struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp326))->hd;struct Cyc_Absyn_Exp*
e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp326->tl))->hd;(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_ptr_plus_e,({struct
Cyc_Absyn_Exp*_tmp3A0[3];_tmp3A0[2]=e2;_tmp3A0[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp39A),0);_tmp3A0[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3A0,sizeof(struct Cyc_Absyn_Exp*),3));})));goto _LL267;}_LL26A:
if((int)_tmp39E != 1)goto _LL26C;_LL26B: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp326))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp326->tl))->hd;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp3A1[3];_tmp3A1[2]=e2;
_tmp3A1[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp39A),0);_tmp3A1[0]=e1;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp3A1,sizeof(struct Cyc_Absyn_Exp*),3));})));goto _LL267;}_LL26C: if(_tmp39E <= (
void*)2)goto _LL26E;if(*((int*)_tmp39E)!= 0)goto _LL26E;_tmp39F=((struct Cyc_Absyn_Upper_b_struct*)
_tmp39E)->f1;_LL26D: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp39D)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp326))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp326->tl))->hd;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__zero_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp3A2[3];_tmp3A2[2]=e2;_tmp3A2[
1]=_tmp39F;_tmp3A2[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3A2,sizeof(struct Cyc_Absyn_Exp*),3));})));}goto _LL267;_LL26E:
if(_tmp39E <= (void*)2)goto _LL267;if(*((int*)_tmp39E)!= 1)goto _LL267;_LL26F:({
void*_tmp3A3[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp3A4="toc: plus on AbsUpper_b";_tag_dynforward(
_tmp3A4,sizeof(char),_get_zero_arr_size(_tmp3A4,24));}),_tag_dynforward(_tmp3A3,
sizeof(void*),0));});_LL267:;}goto _LL262;_LL265:;_LL266: goto _LL262;_LL262:;}goto
_LL23D;_LL242: if((int)_tmp383 != 2)goto _LL244;_LL243: {void*elt_typ=(void*)0;int
forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp382))->hd,& elt_typ,& forward_only)){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp326))->hd;struct Cyc_Absyn_Exp*
e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp326->tl))->hd;
int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)
_check_null(_tmp382->tl))->hd,& forward_only)){(void*)(e1->r=(void*)Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp));(void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp));(void*)(e->r=(void*)((void*)(
Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),0))->r));}else{if(forward_only)({void*_tmp3A5[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp3A6="subtraction not allowed on forward-only ? pointer";
_tag_dynforward(_tmp3A6,sizeof(char),_get_zero_arr_size(_tmp3A6,50));}),
_tag_dynforward(_tmp3A5,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp3A7[3];_tmp3A7[2]=Cyc_Absyn_prim1_exp((
void*)2,e2,0);_tmp3A7[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);
_tmp3A7[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3A7,sizeof(struct Cyc_Absyn_Exp*),3));})));}}goto _LL23D;}
_LL244: if((int)_tmp383 != 5)goto _LL246;_LL245: goto _LL247;_LL246: if((int)_tmp383 != 
6)goto _LL248;_LL247: goto _LL249;_LL248: if((int)_tmp383 != 7)goto _LL24A;_LL249: goto
_LL24B;_LL24A: if((int)_tmp383 != 9)goto _LL24C;_LL24B: goto _LL24D;_LL24C: if((int)
_tmp383 != 8)goto _LL24E;_LL24D: goto _LL24F;_LL24E: if((int)_tmp383 != 10)goto _LL250;
_LL24F: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp326))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp326->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp382))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp382->tl))->hd;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ(t1,&
forward_only))(void*)(e1->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((
void*)e1->r,0),Cyc_Toc_curr_sp));if(Cyc_Tcutil_is_tagged_pointer_typ(t2,&
forward_only))(void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((
void*)e2->r,0),Cyc_Toc_curr_sp));goto _LL23D;}_LL250:;_LL251: goto _LL23D;_LL23D:;}
goto _LL1E3;}_LL1EE: if(*((int*)_tmp320)!= 5)goto _LL1F0;_tmp327=((struct Cyc_Absyn_Increment_e_struct*)
_tmp320)->f1;_tmp328=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp320)->f2;
_LL1EF: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp327->topt))->v;
void*ptr_type=(void*)0;void*elt_type=(void*)0;int is_dynforward=0;int is_dyneither=
0;struct _dynforward_ptr incr_str=({const char*_tmp3B3="increment";_tag_dynforward(
_tmp3B3,sizeof(char),_get_zero_arr_size(_tmp3B3,10));});if(_tmp328 == (void*)2
 || _tmp328 == (void*)3)incr_str=({const char*_tmp3A8="decrement";_tag_dynforward(
_tmp3A8,sizeof(char),_get_zero_arr_size(_tmp3A8,10));});if(Cyc_Toc_is_zero_ptr_deref(
_tmp327,& ptr_type,& is_dynforward,& is_dyneither,& elt_type)){({struct Cyc_String_pa_struct
_tmp3AB;_tmp3AB.tag=0;_tmp3AB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
incr_str);{void*_tmp3A9[1]={& _tmp3AB};Cyc_Tcutil_terr(e->loc,({const char*_tmp3AA="in-place %s is not supported when dereferencing a zero-terminated pointer";
_tag_dynforward(_tmp3AA,sizeof(char),_get_zero_arr_size(_tmp3AA,74));}),
_tag_dynforward(_tmp3A9,sizeof(void*),1));}});({void*_tmp3AC[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp3AD="in-place inc on zero-term";_tag_dynforward(_tmp3AD,sizeof(char),
_get_zero_arr_size(_tmp3AD,26));}),_tag_dynforward(_tmp3AC,sizeof(void*),0));});}
Cyc_Toc_exp_to_c(nv,_tmp327);{void*elt_typ=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ,& forward_only)){struct Cyc_Absyn_Exp*fn_e;int change=1;if(
forward_only){if(_tmp328 == (void*)3  || _tmp328 == (void*)2)({void*_tmp3AE[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp3AF="decrement not allowed on forward ? pointer";
_tag_dynforward(_tmp3AF,sizeof(char),_get_zero_arr_size(_tmp3AF,43));}),
_tag_dynforward(_tmp3AE,sizeof(void*),0));});fn_e=_tmp328 == (void*)1?Cyc_Toc__dynforward_ptr_inplace_plus_post_e:
Cyc_Toc__dynforward_ptr_inplace_plus_e;}else{fn_e=(_tmp328 == (void*)1  || _tmp328
== (void*)3)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp328 == (void*)2  || _tmp328 == (void*)3)change=- 1;}(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
fn_e,({struct Cyc_Absyn_Exp*_tmp3B0[3];_tmp3B0[2]=Cyc_Absyn_signed_int_exp(change,
0);_tmp3B0[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp3B0[0]=
Cyc_Absyn_address_exp(_tmp327,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp3B0,sizeof(struct Cyc_Absyn_Exp*),3));})));}
else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
_tmp3B1=_tmp328 == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_e: Cyc_Toc__zero_arr_inplace_plus_e;(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp3B1,({struct Cyc_Absyn_Exp*_tmp3B2[2];
_tmp3B2[1]=Cyc_Absyn_signed_int_exp(1,0);_tmp3B2[0]=_tmp327;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3B2,sizeof(struct Cyc_Absyn_Exp*),
2));})));}else{if(elt_typ == (void*)0  && !Cyc_Absyn_is_lvalue(_tmp327)){Cyc_Toc_lvalue_assign(
_tmp327,0,Cyc_Toc_incr_lvalue,_tmp328);(void*)(e->r=(void*)((void*)_tmp327->r));}}}
goto _LL1E3;}}_LL1F0: if(*((int*)_tmp320)!= 4)goto _LL1F2;_tmp329=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp320)->f1;_tmp32A=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp320)->f2;_tmp32B=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp320)->f3;_LL1F1: {void*ptr_type=(void*)0;
void*elt_type=(void*)0;int is_dynforward=0;int is_dyneither=0;if(Cyc_Toc_is_zero_ptr_deref(
_tmp329,& ptr_type,& is_dynforward,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(
nv,e,_tmp329,_tmp32A,_tmp32B,ptr_type,is_dynforward,is_dyneither,elt_type);
return;}{int e1_poly=Cyc_Toc_is_poly_project(_tmp329);void*e1_old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp329->topt))->v;void*e2_old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp32B->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp329);
Cyc_Toc_exp_to_c(nv,_tmp32B);{int done=0;if(_tmp32A != 0){void*elt_typ=(void*)0;
int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ,&
forward_only)){struct Cyc_Absyn_Exp*change;{void*_tmp3B4=(void*)_tmp32A->v;_LL271:
if((int)_tmp3B4 != 0)goto _LL273;_LL272: change=_tmp32B;goto _LL270;_LL273: if((int)
_tmp3B4 != 2)goto _LL275;_LL274: if(forward_only)({void*_tmp3B5[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp3B6="subtraction not allowed on forward ? pointers";
_tag_dynforward(_tmp3B6,sizeof(char),_get_zero_arr_size(_tmp3B6,46));}),
_tag_dynforward(_tmp3B5,sizeof(void*),0));});change=Cyc_Absyn_prim1_exp((void*)2,
_tmp32B,0);goto _LL270;_LL275:;_LL276:({void*_tmp3B7[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp3B8="bad t ? pointer arithmetic";_tag_dynforward(_tmp3B8,sizeof(char),
_get_zero_arr_size(_tmp3B8,27));}),_tag_dynforward(_tmp3B7,sizeof(void*),0));});
_LL270:;}done=1;{struct Cyc_Absyn_Exp*_tmp3B9=forward_only?Cyc_Toc__dynforward_ptr_inplace_plus_e:
Cyc_Toc__dyneither_ptr_inplace_plus_e;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
_tmp3B9,({struct Cyc_Absyn_Exp*_tmp3BA[3];_tmp3BA[2]=change;_tmp3BA[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp3BA[0]=Cyc_Absyn_address_exp(_tmp329,0);((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp3BA,sizeof(struct Cyc_Absyn_Exp*),3));})));}}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){void*_tmp3BB=(void*)_tmp32A->v;_LL278: if((int)_tmp3BB != 0)
goto _LL27A;_LL279: done=1;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__zero_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp3BC[2];_tmp3BC[1]=_tmp32B;_tmp3BC[0]=_tmp329;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3BC,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL277;_LL27A:;_LL27B:({void*_tmp3BD[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp3BE="bad zero-terminated pointer arithmetic";_tag_dynforward(_tmp3BE,sizeof(
char),_get_zero_arr_size(_tmp3BE,39));}),_tag_dynforward(_tmp3BD,sizeof(void*),0));});
_LL277:;}}}if(!done){if(e1_poly)(void*)(_tmp32B->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp32B->r,0)));if(!Cyc_Absyn_is_lvalue(_tmp329)){((
void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Absyn_Exp*,struct _tuple9*),struct _tuple9*f_env))Cyc_Toc_lvalue_assign)(
_tmp329,0,Cyc_Toc_assignop_lvalue,({struct _tuple9*_tmp3BF=_cycalloc(sizeof(
struct _tuple9)* 1);_tmp3BF[0]=({struct _tuple9 _tmp3C0;_tmp3C0.f1=_tmp32A;_tmp3C0.f2=
_tmp32B;_tmp3C0;});_tmp3BF;}));(void*)(e->r=(void*)((void*)_tmp329->r));}}goto
_LL1E3;}}}_LL1F2: if(*((int*)_tmp320)!= 6)goto _LL1F4;_tmp32C=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp320)->f1;_tmp32D=((struct Cyc_Absyn_Conditional_e_struct*)_tmp320)->f2;
_tmp32E=((struct Cyc_Absyn_Conditional_e_struct*)_tmp320)->f3;_LL1F3: Cyc_Toc_exp_to_c(
nv,_tmp32C);Cyc_Toc_exp_to_c(nv,_tmp32D);Cyc_Toc_exp_to_c(nv,_tmp32E);goto _LL1E3;
_LL1F4: if(*((int*)_tmp320)!= 7)goto _LL1F6;_tmp32F=((struct Cyc_Absyn_And_e_struct*)
_tmp320)->f1;_tmp330=((struct Cyc_Absyn_And_e_struct*)_tmp320)->f2;_LL1F5: Cyc_Toc_exp_to_c(
nv,_tmp32F);Cyc_Toc_exp_to_c(nv,_tmp330);goto _LL1E3;_LL1F6: if(*((int*)_tmp320)!= 
8)goto _LL1F8;_tmp331=((struct Cyc_Absyn_Or_e_struct*)_tmp320)->f1;_tmp332=((
struct Cyc_Absyn_Or_e_struct*)_tmp320)->f2;_LL1F7: Cyc_Toc_exp_to_c(nv,_tmp331);
Cyc_Toc_exp_to_c(nv,_tmp332);goto _LL1E3;_LL1F8: if(*((int*)_tmp320)!= 9)goto
_LL1FA;_tmp333=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp320)->f1;_tmp334=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp320)->f2;_LL1F9: Cyc_Toc_exp_to_c(nv,_tmp333);Cyc_Toc_exp_to_c(
nv,_tmp334);goto _LL1E3;_LL1FA: if(*((int*)_tmp320)!= 10)goto _LL1FC;_tmp335=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp320)->f1;_tmp336=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp320)->f2;_LL1FB: _tmp337=_tmp335;_tmp338=_tmp336;goto _LL1FD;_LL1FC: if(*((int*)
_tmp320)!= 11)goto _LL1FE;_tmp337=((struct Cyc_Absyn_FnCall_e_struct*)_tmp320)->f1;
_tmp338=((struct Cyc_Absyn_FnCall_e_struct*)_tmp320)->f2;_tmp339=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp320)->f3;if(_tmp339 != 0)goto _LL1FE;_LL1FD: Cyc_Toc_exp_to_c(nv,_tmp337);((
void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp338);goto _LL1E3;
_LL1FE: if(*((int*)_tmp320)!= 11)goto _LL200;_tmp33A=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp320)->f1;_tmp33B=((struct Cyc_Absyn_FnCall_e_struct*)_tmp320)->f2;_tmp33C=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp320)->f3;if(_tmp33C == 0)goto _LL200;_tmp33D=*
_tmp33C;_tmp33E=_tmp33D.num_varargs;_tmp33F=_tmp33D.injectors;_tmp340=_tmp33D.vai;
_LL1FF:{struct _RegionHandle _tmp3C1=_new_region("r");struct _RegionHandle*r=&
_tmp3C1;_push_region(r);{struct _tuple1*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp33E,0);void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp340->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp33B);int num_normargs=num_args - _tmp33E;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp33B=_tmp33B->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp33B))->hd);
new_args=({struct Cyc_List_List*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));_tmp3C2->hd=(
struct Cyc_Absyn_Exp*)_tmp33B->hd;_tmp3C2->tl=new_args;_tmp3C2;});}}new_args=({
struct Cyc_List_List*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*_tmp3C4[3];_tmp3C4[2]=
num_varargs_exp;_tmp3C4[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp3C4[0]=
argvexp;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3C4,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp3C3->tl=
new_args;_tmp3C3;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp33A);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp33A,new_args,0),0);if(_tmp340->inject){
struct Cyc_Absyn_Tuniondecl*tud;{void*_tmp3C5=Cyc_Tcutil_compress((void*)_tmp340->type);
struct Cyc_Absyn_TunionInfo _tmp3C6;void*_tmp3C7;struct Cyc_Absyn_Tuniondecl**
_tmp3C8;struct Cyc_Absyn_Tuniondecl*_tmp3C9;_LL27D: if(_tmp3C5 <= (void*)4)goto
_LL27F;if(*((int*)_tmp3C5)!= 2)goto _LL27F;_tmp3C6=((struct Cyc_Absyn_TunionType_struct*)
_tmp3C5)->f1;_tmp3C7=(void*)_tmp3C6.tunion_info;if(*((int*)_tmp3C7)!= 1)goto
_LL27F;_tmp3C8=((struct Cyc_Absyn_KnownTunion_struct*)_tmp3C7)->f1;_tmp3C9=*
_tmp3C8;_LL27E: tud=_tmp3C9;goto _LL27C;_LL27F:;_LL280:({void*_tmp3CA[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp3CB="toc: unknown tunion in vararg with inject";_tag_dynforward(_tmp3CB,
sizeof(char),_get_zero_arr_size(_tmp3CB,42));}),_tag_dynforward(_tmp3CA,sizeof(
void*),0));});_LL27C:;}{struct _dynforward_ptr vs=({unsigned int _tmp3D3=(
unsigned int)_tmp33E;struct _tuple1**_tmp3D4=(struct _tuple1**)_region_malloc(r,
_check_times(sizeof(struct _tuple1*),_tmp3D3));struct _dynforward_ptr _tmp3D6=
_tag_dynforward(_tmp3D4,sizeof(struct _tuple1*),_tmp3D3);{unsigned int _tmp3D5=
_tmp3D3;unsigned int i;for(i=0;i < _tmp3D5;i ++){_tmp3D4[i]=Cyc_Toc_temp_var();}}
_tmp3D6;});struct Cyc_List_List*_tmp3CC=0;{int i=_tmp33E - 1;for(0;i >= 0;-- i){
_tmp3CC=({struct Cyc_List_List*_tmp3CD=_cycalloc(sizeof(*_tmp3CD));_tmp3CD->hd=
Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)
_check_dynforward_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp3CD->tl=
_tmp3CC;_tmp3CD;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp(0,_tmp3CC,0),s,0);{int i=0;for(0;_tmp33B != 0;(((
_tmp33B=_tmp33B->tl,_tmp33F=_tmp33F->tl)),++ i)){struct Cyc_Absyn_Exp*arg=(struct
Cyc_Absyn_Exp*)_tmp33B->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple1*var=*((struct _tuple1**)
_check_dynforward_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield _tmp3CF;struct _tuple1*
_tmp3D0;struct Cyc_List_List*_tmp3D1;struct Cyc_Absyn_Tunionfield*_tmp3CE=(struct
Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp33F))->hd;_tmp3CF=*
_tmp3CE;_tmp3D0=_tmp3CF.name;_tmp3D1=_tmp3CF.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp3D1))->hd)).f2);Cyc_Toc_exp_to_c(
nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(
1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp3D0,1),0),s,0);s=Cyc_Absyn_declare_stmt(
var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp3D0,({const char*_tmp3D2="_struct";
_tag_dynforward(_tmp3D2,sizeof(char),_get_zero_arr_size(_tmp3D2,8));}))),0,s,0);}}}}}
else{{int i=0;for(0;_tmp33B != 0;(_tmp33B=_tmp33B->tl,++ i)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp33B->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp33B->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
s));}};_pop_region(r);}goto _LL1E3;_LL200: if(*((int*)_tmp320)!= 12)goto _LL202;
_tmp341=((struct Cyc_Absyn_Throw_e_struct*)_tmp320)->f1;_LL201: Cyc_Toc_exp_to_c(
nv,_tmp341);(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(
old_typ),Cyc_Toc_newthrow_exp(_tmp341),0))->r));goto _LL1E3;_LL202: if(*((int*)
_tmp320)!= 13)goto _LL204;_tmp342=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp320)->f1;_LL203: Cyc_Toc_exp_to_c(nv,_tmp342);goto _LL1E3;_LL204: if(*((int*)
_tmp320)!= 14)goto _LL206;_tmp343=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp320)->f1;
_tmp344=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp320)->f2;_LL205: Cyc_Toc_exp_to_c(
nv,_tmp343);for(0;_tmp344 != 0;_tmp344=_tmp344->tl){void*k=Cyc_Tcutil_typ_kind((
void*)_tmp344->hd);if(k != (void*)4  && k != (void*)3){{void*_tmp3D7=Cyc_Tcutil_compress((
void*)_tmp344->hd);_LL282: if(_tmp3D7 <= (void*)4)goto _LL286;if(*((int*)_tmp3D7)!= 
1)goto _LL284;_LL283: goto _LL285;_LL284: if(*((int*)_tmp3D7)!= 2)goto _LL286;_LL285:
continue;_LL286:;_LL287:(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp343,
0))->r));goto _LL281;_LL281:;}break;}}goto _LL1E3;_LL206: if(*((int*)_tmp320)!= 15)
goto _LL208;_tmp345=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp320)->f1;_tmp346=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp320)->f1);_tmp347=((struct
Cyc_Absyn_Cast_e_struct*)_tmp320)->f2;_tmp348=((struct Cyc_Absyn_Cast_e_struct*)
_tmp320)->f3;_tmp349=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp320)->f4;_LL207: {
void*old_t2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp347->topt))->v;void*
new_typ=*_tmp346;*_tmp346=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp347);{
struct _tuple0 _tmp3D9=({struct _tuple0 _tmp3D8;_tmp3D8.f1=Cyc_Tcutil_compress(
old_t2);_tmp3D8.f2=Cyc_Tcutil_compress(new_typ);_tmp3D8;});void*_tmp3DA;struct
Cyc_Absyn_PtrInfo _tmp3DB;void*_tmp3DC;struct Cyc_Absyn_PtrInfo _tmp3DD;void*
_tmp3DE;struct Cyc_Absyn_PtrInfo _tmp3DF;void*_tmp3E0;_LL289: _tmp3DA=_tmp3D9.f1;
if(_tmp3DA <= (void*)4)goto _LL28B;if(*((int*)_tmp3DA)!= 4)goto _LL28B;_tmp3DB=((
struct Cyc_Absyn_PointerType_struct*)_tmp3DA)->f1;_tmp3DC=_tmp3D9.f2;if(_tmp3DC <= (
void*)4)goto _LL28B;if(*((int*)_tmp3DC)!= 4)goto _LL28B;_tmp3DD=((struct Cyc_Absyn_PointerType_struct*)
_tmp3DC)->f1;_LL28A: {int _tmp3E1=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp3DB.ptr_atts).nullable);int _tmp3E2=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,(_tmp3DD.ptr_atts).nullable);void*_tmp3E3=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp3DB.ptr_atts).bounds);void*_tmp3E4=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp3DD.ptr_atts).bounds);int _tmp3E5=((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp3DB.ptr_atts).zero_term);int
_tmp3E6=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp3DD.ptr_atts).zero_term);
int p1_forward_only=0;int p2_forward_only=0;{struct _tuple0 _tmp3E8=({struct _tuple0
_tmp3E7;_tmp3E7.f1=_tmp3E3;_tmp3E7.f2=_tmp3E4;_tmp3E7;});void*_tmp3E9;struct Cyc_Absyn_Exp*
_tmp3EA;void*_tmp3EB;struct Cyc_Absyn_Exp*_tmp3EC;void*_tmp3ED;struct Cyc_Absyn_Exp*
_tmp3EE;void*_tmp3EF;void*_tmp3F0;struct Cyc_Absyn_Exp*_tmp3F1;void*_tmp3F2;void*
_tmp3F3;void*_tmp3F4;struct Cyc_Absyn_Exp*_tmp3F5;void*_tmp3F6;void*_tmp3F7;
struct Cyc_Absyn_Exp*_tmp3F8;void*_tmp3F9;void*_tmp3FA;void*_tmp3FB;void*_tmp3FC;
void*_tmp3FD;void*_tmp3FE;void*_tmp3FF;void*_tmp400;void*_tmp401;void*_tmp402;
void*_tmp403;void*_tmp404;void*_tmp405;void*_tmp406;_LL290: _tmp3E9=_tmp3E8.f1;
if(_tmp3E9 <= (void*)2)goto _LL292;if(*((int*)_tmp3E9)!= 0)goto _LL292;_tmp3EA=((
struct Cyc_Absyn_Upper_b_struct*)_tmp3E9)->f1;_tmp3EB=_tmp3E8.f2;if(_tmp3EB <= (
void*)2)goto _LL292;if(*((int*)_tmp3EB)!= 0)goto _LL292;_tmp3EC=((struct Cyc_Absyn_Upper_b_struct*)
_tmp3EB)->f1;_LL291: if(_tmp3E1  && !_tmp3E2){if(nv->toplevel)({void*_tmp407[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const
char*_tmp408="can't do NULL-check conversion at top-level";_tag_dynforward(
_tmp408,sizeof(char),_get_zero_arr_size(_tmp408,44));}),_tag_dynforward(_tmp407,
sizeof(void*),0));});if(_tmp349 != (void*)2)({struct Cyc_String_pa_struct _tmp40B;
_tmp40B.tag=0;_tmp40B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp409[1]={& _tmp40B};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp40A="null-check conversion mis-classified: %s";
_tag_dynforward(_tmp40A,sizeof(char),_get_zero_arr_size(_tmp40A,41));}),
_tag_dynforward(_tmp409,sizeof(void*),1));}});{int do_null_check=Cyc_Toc_need_null_check(
_tmp347);if(do_null_check){if(!_tmp348)({void*_tmp40C[0]={};Cyc_Tcutil_warn(e->loc,({
const char*_tmp40D="inserted null check due to implicit cast from * to @ type";
_tag_dynforward(_tmp40D,sizeof(char),_get_zero_arr_size(_tmp40D,58));}),
_tag_dynforward(_tmp40C,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp346,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp40E=
_cycalloc(sizeof(*_tmp40E));_tmp40E->hd=_tmp347;_tmp40E->tl=0;_tmp40E;}),0)));}}}
goto _LL28F;_LL292: _tmp3ED=_tmp3E8.f1;if(_tmp3ED <= (void*)2)goto _LL294;if(*((int*)
_tmp3ED)!= 0)goto _LL294;_tmp3EE=((struct Cyc_Absyn_Upper_b_struct*)_tmp3ED)->f1;
_tmp3EF=_tmp3E8.f2;if((int)_tmp3EF != 0)goto _LL294;_LL293: p2_forward_only=1;
_tmp3F1=_tmp3EE;goto _LL295;_LL294: _tmp3F0=_tmp3E8.f1;if(_tmp3F0 <= (void*)2)goto
_LL296;if(*((int*)_tmp3F0)!= 0)goto _LL296;_tmp3F1=((struct Cyc_Absyn_Upper_b_struct*)
_tmp3F0)->f1;_tmp3F2=_tmp3E8.f2;if((int)_tmp3F2 != 1)goto _LL296;_LL295: if(_tmp349
== (void*)2)({struct Cyc_String_pa_struct _tmp411;_tmp411.tag=0;_tmp411.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{void*_tmp40F[
1]={& _tmp411};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp410="conversion mis-classified as null-check: %s";_tag_dynforward(
_tmp410,sizeof(char),_get_zero_arr_size(_tmp410,44));}),_tag_dynforward(_tmp40F,
sizeof(void*),1));}});if(nv->toplevel){if((_tmp3E5  && !(_tmp3DD.elt_tq).real_const)
 && !_tmp3E6)_tmp3F1=Cyc_Absyn_prim2_exp((void*)2,_tmp3F1,Cyc_Absyn_uint_exp(1,0),
0);(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp3F1,
_tmp347,p2_forward_only))->r));}else{struct Cyc_Absyn_Exp*_tmp412=p2_forward_only?
Cyc_Toc__tag_dynforward_e: Cyc_Toc__tag_dyneither_e;if(_tmp3E5){struct _tuple1*
_tmp413=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp414=Cyc_Absyn_var_exp(_tmp413,
0);struct Cyc_Absyn_Exp*_tmp415=Cyc_Absyn_fncall_exp(Cyc_Toc__get_zero_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp41A[2];_tmp41A[1]=_tmp3F1;_tmp41A[0]=_tmp414;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp41A,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);if(!_tmp3E6  && !(_tmp3DD.elt_tq).real_const)_tmp415=Cyc_Absyn_prim2_exp((
void*)2,_tmp415,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp416=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp3DD.elt_typ),0);struct Cyc_Absyn_Exp*_tmp417=Cyc_Absyn_fncall_exp(
_tmp412,({struct Cyc_Absyn_Exp*_tmp419[3];_tmp419[2]=_tmp415;_tmp419[1]=_tmp416;
_tmp419[0]=_tmp414;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp419,sizeof(struct Cyc_Absyn_Exp*),3));}),0);struct Cyc_Absyn_Stmt*
_tmp418=Cyc_Absyn_exp_stmt(_tmp417,0);_tmp418=Cyc_Absyn_declare_stmt(_tmp413,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp347,_tmp418,0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
_tmp418));}}else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp412,({struct Cyc_Absyn_Exp*
_tmp41B[3];_tmp41B[2]=_tmp3F1;_tmp41B[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp3DD.elt_typ),0);_tmp41B[0]=_tmp347;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp41B,sizeof(struct Cyc_Absyn_Exp*),
3));})));}}goto _LL28F;_LL296: _tmp3F3=_tmp3E8.f1;if((int)_tmp3F3 != 0)goto _LL298;
_tmp3F4=_tmp3E8.f2;if(_tmp3F4 <= (void*)2)goto _LL298;if(*((int*)_tmp3F4)!= 0)goto
_LL298;_tmp3F5=((struct Cyc_Absyn_Upper_b_struct*)_tmp3F4)->f1;_LL297:
p1_forward_only=1;_tmp3F8=_tmp3F5;goto _LL299;_LL298: _tmp3F6=_tmp3E8.f1;if((int)
_tmp3F6 != 1)goto _LL29A;_tmp3F7=_tmp3E8.f2;if(_tmp3F7 <= (void*)2)goto _LL29A;if(*((
int*)_tmp3F7)!= 0)goto _LL29A;_tmp3F8=((struct Cyc_Absyn_Upper_b_struct*)_tmp3F7)->f1;
_LL299: if(nv->toplevel)({void*_tmp41C[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp41D="can't coerce t? to t* or t@ at the top-level";
_tag_dynforward(_tmp41D,sizeof(char),_get_zero_arr_size(_tmp41D,45));}),
_tag_dynforward(_tmp41C,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp41E=
_tmp3F8;if(_tmp3E5  && !_tmp3E6)_tmp41E=Cyc_Absyn_add_exp(_tmp3F8,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Exp*_tmp41F=p1_forward_only?Cyc_Toc__untag_dynforward_ptr_e:
Cyc_Toc__untag_dyneither_ptr_e;struct Cyc_Absyn_Exp*_tmp420=Cyc_Absyn_fncall_exp(
_tmp41F,({struct Cyc_Absyn_Exp*_tmp422[3];_tmp422[2]=_tmp41E;_tmp422[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp3DB.elt_typ),0);_tmp422[0]=_tmp347;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp422,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);if(_tmp3E2)(void*)(_tmp420->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421->hd=Cyc_Absyn_copy_exp(
_tmp420);_tmp421->tl=0;_tmp421;})));(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp346,_tmp420));goto _LL28F;}}_LL29A: _tmp3F9=_tmp3E8.f1;if((int)_tmp3F9 != 0)
goto _LL29C;_tmp3FA=_tmp3E8.f2;if((int)_tmp3FA != 0)goto _LL29C;_LL29B:
p1_forward_only=1;p2_forward_only=1;goto DynCast;_LL29C: _tmp3FB=_tmp3E8.f1;if((
int)_tmp3FB != 0)goto _LL29E;_tmp3FC=_tmp3E8.f2;if((int)_tmp3FC != 1)goto _LL29E;
_LL29D: p1_forward_only=1;goto DynCast;_LL29E: _tmp3FD=_tmp3E8.f1;if((int)_tmp3FD != 
1)goto _LL2A0;_tmp3FE=_tmp3E8.f2;if((int)_tmp3FE != 0)goto _LL2A0;_LL29F:
p2_forward_only=1;goto DynCast;_LL2A0: _tmp3FF=_tmp3E8.f1;if((int)_tmp3FF != 1)goto
_LL2A2;_tmp400=_tmp3E8.f2;if((int)_tmp400 != 1)goto _LL2A2;_LL2A1: DynCast: if((
_tmp3E5  && !_tmp3E6) && !(_tmp3DD.elt_tq).real_const){if(nv->toplevel)({void*
_tmp423[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp424="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";
_tag_dynforward(_tmp424,sizeof(char),_get_zero_arr_size(_tmp424,70));}),
_tag_dynforward(_tmp423,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp425=
p1_forward_only?Cyc_Toc__dynforward_ptr_decrease_size_e: Cyc_Toc__dyneither_ptr_decrease_size_e;(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp425,({struct Cyc_Absyn_Exp*_tmp426[3];
_tmp426[2]=Cyc_Absyn_uint_exp(1,0);_tmp426[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp3DB.elt_typ),0);_tmp426[0]=_tmp347;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp426,sizeof(struct Cyc_Absyn_Exp*),
3));})));if(p1_forward_only != p2_forward_only){if(p1_forward_only)(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_to_dyneither_e,({struct Cyc_Absyn_Exp*
_tmp427[1];_tmp427[0]=Cyc_Absyn_copy_exp(e);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp427,sizeof(struct Cyc_Absyn_Exp*),
1));})));else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_to_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp428[1];_tmp428[0]=Cyc_Absyn_copy_exp(e);((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp428,sizeof(struct Cyc_Absyn_Exp*),
1));})));}}}}else{if(p1_forward_only != p2_forward_only){if(nv->toplevel)({void*
_tmp429[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp42A="can't coerce between ? and ?+- at toplevel";_tag_dynforward(
_tmp42A,sizeof(char),_get_zero_arr_size(_tmp42A,43));}),_tag_dynforward(_tmp429,
sizeof(void*),0));});if(p1_forward_only)(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__dynforward_to_dyneither_e,({struct Cyc_Absyn_Exp*_tmp42B[1];_tmp42B[0]=
_tmp347;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp42B,sizeof(struct Cyc_Absyn_Exp*),1));})));else{(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_to_dynforward_e,({struct Cyc_Absyn_Exp*
_tmp42C[1];_tmp42C[0]=_tmp347;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp42C,sizeof(struct Cyc_Absyn_Exp*),1));})));}}}
goto _LL28F;_LL2A2: _tmp401=_tmp3E8.f1;if(_tmp401 <= (void*)2)goto _LL2A4;if(*((int*)
_tmp401)!= 1)goto _LL2A4;_tmp402=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp401)->f1;_tmp403=_tmp3E8.f2;if(_tmp403 <= (void*)2)goto _LL2A4;if(*((int*)
_tmp403)!= 1)goto _LL2A4;_tmp404=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp403)->f1;_LL2A3: if(_tmp3E1  && !_tmp3E2){if(nv->toplevel)({void*_tmp42D[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const
char*_tmp42E="can't do NULL-check conversion at top-level";_tag_dynforward(
_tmp42E,sizeof(char),_get_zero_arr_size(_tmp42E,44));}),_tag_dynforward(_tmp42D,
sizeof(void*),0));});if(_tmp349 != (void*)2)({struct Cyc_String_pa_struct _tmp431;
_tmp431.tag=0;_tmp431.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp42F[1]={& _tmp431};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp430="null-check conversion mis-classified: %s";
_tag_dynforward(_tmp430,sizeof(char),_get_zero_arr_size(_tmp430,41));}),
_tag_dynforward(_tmp42F,sizeof(void*),1));}});{int do_null_check=Cyc_Toc_need_null_check(
_tmp347);if(do_null_check){if(!_tmp348)({void*_tmp432[0]={};Cyc_Tcutil_warn(e->loc,({
const char*_tmp433="inserted null check due to implicit cast from * to @ type";
_tag_dynforward(_tmp433,sizeof(char),_get_zero_arr_size(_tmp433,58));}),
_tag_dynforward(_tmp432,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp346,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp434=
_cycalloc(sizeof(*_tmp434));_tmp434->hd=_tmp347;_tmp434->tl=0;_tmp434;}),0)));}}}
goto _LL28F;_LL2A4: _tmp405=_tmp3E8.f1;if(_tmp405 <= (void*)2)goto _LL2A6;if(*((int*)
_tmp405)!= 1)goto _LL2A6;_LL2A5:({struct Cyc_String_pa_struct _tmp437;_tmp437.tag=0;
_tmp437.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp347->loc));{void*_tmp435[1]={& _tmp437};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp436="%s: toc, cast from AbsUpper_b";
_tag_dynforward(_tmp436,sizeof(char),_get_zero_arr_size(_tmp436,30));}),
_tag_dynforward(_tmp435,sizeof(void*),1));}});_LL2A6: _tmp406=_tmp3E8.f2;if(
_tmp406 <= (void*)2)goto _LL28F;if(*((int*)_tmp406)!= 1)goto _LL28F;_LL2A7: goto
_LL28F;_LL28F:;}goto _LL288;}_LL28B: _tmp3DE=_tmp3D9.f1;if(_tmp3DE <= (void*)4)goto
_LL28D;if(*((int*)_tmp3DE)!= 4)goto _LL28D;_tmp3DF=((struct Cyc_Absyn_PointerType_struct*)
_tmp3DE)->f1;_tmp3E0=_tmp3D9.f2;if(_tmp3E0 <= (void*)4)goto _LL28D;if(*((int*)
_tmp3E0)!= 5)goto _LL28D;_LL28C:{void*_tmp438=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(
_tmp3DF.ptr_atts).bounds);_LL2A9: if((int)_tmp438 != 0)goto _LL2AB;_LL2AA: goto
_LL2AC;_LL2AB: if((int)_tmp438 != 1)goto _LL2AD;_LL2AC:(void*)(_tmp347->r=(void*)
Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)_tmp347->r,_tmp347->loc),Cyc_Toc_curr_sp));
goto _LL2A8;_LL2AD:;_LL2AE: goto _LL2A8;_LL2A8:;}goto _LL288;_LL28D:;_LL28E: goto
_LL288;_LL288:;}goto _LL1E3;}_LL208: if(*((int*)_tmp320)!= 16)goto _LL20A;_tmp34A=((
struct Cyc_Absyn_Address_e_struct*)_tmp320)->f1;_LL209:{void*_tmp439=(void*)
_tmp34A->r;struct _tuple1*_tmp43A;struct Cyc_List_List*_tmp43B;struct Cyc_List_List*
_tmp43C;struct Cyc_List_List*_tmp43D;_LL2B0: if(*((int*)_tmp439)!= 30)goto _LL2B2;
_tmp43A=((struct Cyc_Absyn_Struct_e_struct*)_tmp439)->f1;_tmp43B=((struct Cyc_Absyn_Struct_e_struct*)
_tmp439)->f2;_tmp43C=((struct Cyc_Absyn_Struct_e_struct*)_tmp439)->f3;_LL2B1: if(
nv->toplevel)({struct Cyc_String_pa_struct _tmp440;_tmp440.tag=0;_tmp440.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp34A->loc));{void*_tmp43E[1]={& _tmp440};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp43F="%s: & on non-identifiers at the top-level";
_tag_dynforward(_tmp43F,sizeof(char),_get_zero_arr_size(_tmp43F,42));}),
_tag_dynforward(_tmp43E,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp34A->topt))->v,_tmp43B != 0,1,0,
_tmp43C,_tmp43A))->r));goto _LL2AF;_LL2B2: if(*((int*)_tmp439)!= 26)goto _LL2B4;
_tmp43D=((struct Cyc_Absyn_Tuple_e_struct*)_tmp439)->f1;_LL2B3: if(nv->toplevel)({
struct Cyc_String_pa_struct _tmp443;_tmp443.tag=0;_tmp443.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(_tmp34A->loc));{
void*_tmp441[1]={& _tmp443};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp442="%s: & on non-identifiers at the top-level";
_tag_dynforward(_tmp442,sizeof(char),_get_zero_arr_size(_tmp442,42));}),
_tag_dynforward(_tmp441,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,1,0,_tmp43D))->r));goto _LL2AF;_LL2B4:;_LL2B5: Cyc_Toc_exp_to_c(nv,_tmp34A);if(
!Cyc_Absyn_is_lvalue(_tmp34A)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp34A,0,Cyc_Toc_address_lvalue,1);(void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp34A));}goto _LL2AF;
_LL2AF:;}goto _LL1E3;_LL20A: if(*((int*)_tmp320)!= 17)goto _LL20C;_tmp34B=((struct
Cyc_Absyn_New_e_struct*)_tmp320)->f1;_tmp34C=((struct Cyc_Absyn_New_e_struct*)
_tmp320)->f2;_LL20B: if(nv->toplevel)({struct Cyc_String_pa_struct _tmp446;_tmp446.tag=
0;_tmp446.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp34C->loc));{void*_tmp444[1]={& _tmp446};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp445="%s: new at top-level";
_tag_dynforward(_tmp445,sizeof(char),_get_zero_arr_size(_tmp445,21));}),
_tag_dynforward(_tmp444,sizeof(void*),1));}});{void*_tmp447=(void*)_tmp34C->r;
struct Cyc_List_List*_tmp448;struct Cyc_Absyn_Vardecl*_tmp449;struct Cyc_Absyn_Exp*
_tmp44A;struct Cyc_Absyn_Exp*_tmp44B;int _tmp44C;struct _tuple1*_tmp44D;struct Cyc_List_List*
_tmp44E;struct Cyc_List_List*_tmp44F;struct Cyc_List_List*_tmp450;_LL2B7: if(*((int*)
_tmp447)!= 28)goto _LL2B9;_tmp448=((struct Cyc_Absyn_Array_e_struct*)_tmp447)->f1;
_LL2B8: {struct _tuple1*_tmp451=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp452=
Cyc_Absyn_var_exp(_tmp451,0);struct Cyc_Absyn_Stmt*_tmp453=Cyc_Toc_init_array(nv,
_tmp452,_tmp448,Cyc_Absyn_exp_stmt(_tmp452,0));void*old_elt_typ;{void*_tmp454=
Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp455;void*_tmp456;struct
Cyc_Absyn_Tqual _tmp457;struct Cyc_Absyn_PtrAtts _tmp458;struct Cyc_Absyn_Conref*
_tmp459;_LL2C2: if(_tmp454 <= (void*)4)goto _LL2C4;if(*((int*)_tmp454)!= 4)goto
_LL2C4;_tmp455=((struct Cyc_Absyn_PointerType_struct*)_tmp454)->f1;_tmp456=(void*)
_tmp455.elt_typ;_tmp457=_tmp455.elt_tq;_tmp458=_tmp455.ptr_atts;_tmp459=_tmp458.zero_term;
_LL2C3: old_elt_typ=_tmp456;goto _LL2C1;_LL2C4:;_LL2C5: old_elt_typ=({void*_tmp45A[
0]={};Cyc_Toc_toc_impos(({const char*_tmp45B="exp_to_c:new array expression doesn't have ptr type";
_tag_dynforward(_tmp45B,sizeof(char),_get_zero_arr_size(_tmp45B,52));}),
_tag_dynforward(_tmp45A,sizeof(void*),0));});_LL2C1:;}{void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);void*_tmp45C=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp45D=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),Cyc_Absyn_signed_int_exp(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp448),0),0);struct Cyc_Absyn_Exp*
e1;if(_tmp34B == 0)e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp45D);else{struct Cyc_Absyn_Exp*
r=(struct Cyc_Absyn_Exp*)_tmp34B;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,
_tmp45D);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp451,
_tmp45C,(struct Cyc_Absyn_Exp*)e1,_tmp453,0)));goto _LL2B6;}}_LL2B9: if(*((int*)
_tmp447)!= 29)goto _LL2BB;_tmp449=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp447)->f1;_tmp44A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp447)->f2;
_tmp44B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp447)->f3;_tmp44C=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp447)->f4;_LL2BA: {int is_dynforward_ptr=0;
int is_dyneither_ptr=0;{void*_tmp45E=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp45F;void*_tmp460;struct Cyc_Absyn_Tqual _tmp461;struct Cyc_Absyn_PtrAtts _tmp462;
struct Cyc_Absyn_Conref*_tmp463;struct Cyc_Absyn_Conref*_tmp464;_LL2C7: if(_tmp45E
<= (void*)4)goto _LL2C9;if(*((int*)_tmp45E)!= 4)goto _LL2C9;_tmp45F=((struct Cyc_Absyn_PointerType_struct*)
_tmp45E)->f1;_tmp460=(void*)_tmp45F.elt_typ;_tmp461=_tmp45F.elt_tq;_tmp462=
_tmp45F.ptr_atts;_tmp463=_tmp462.bounds;_tmp464=_tmp462.zero_term;_LL2C8:
is_dynforward_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp463)== (void*)0;
is_dyneither_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp463)== (void*)1;
goto _LL2C6;_LL2C9:;_LL2CA:({void*_tmp465[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp466="exp_to_c: comprehension not an array type";
_tag_dynforward(_tmp466,sizeof(char),_get_zero_arr_size(_tmp466,42));}),
_tag_dynforward(_tmp465,sizeof(void*),0));});_LL2C6:;}{struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp44B->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*
ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp44A);{
struct Cyc_Absyn_Exp*_tmp467=Cyc_Absyn_var_exp(max,0);if(_tmp44C)_tmp467=Cyc_Absyn_add_exp(
_tmp467,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(
nv,Cyc_Absyn_var_exp(a,0),_tmp449,Cyc_Absyn_var_exp(max,0),_tmp44B,_tmp44C,Cyc_Toc_skip_stmt_dl(),
1);{struct _RegionHandle _tmp468=_new_region("r");struct _RegionHandle*r=& _tmp468;
_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp479=
_region_malloc(r,sizeof(*_tmp479));_tmp479->hd=({struct _tuple10*_tmp47A=
_region_malloc(r,sizeof(*_tmp47A));_tmp47A->f1=max;_tmp47A->f2=Cyc_Absyn_uint_typ;
_tmp47A->f3=(struct Cyc_Absyn_Exp*)_tmp44A;_tmp47A;});_tmp479->tl=0;_tmp479;});
struct Cyc_Absyn_Exp*ai;if(_tmp34B == 0)ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp469[2];_tmp469[1]=_tmp467;
_tmp469[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp469,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp34B;Cyc_Toc_exp_to_c(
nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp*_tmp46A[2];_tmp46A[1]=_tmp467;_tmp46A[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp46A,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);decls=({struct Cyc_List_List*_tmp46B=
_region_malloc(r,sizeof(*_tmp46B));_tmp46B->hd=({struct _tuple10*_tmp46C=
_region_malloc(r,sizeof(*_tmp46C));_tmp46C->f1=a;_tmp46C->f2=ptr_typ;_tmp46C->f3=(
struct Cyc_Absyn_Exp*)ainit;_tmp46C;});_tmp46B->tl=decls;_tmp46B;});if(
is_dynforward_ptr  || is_dyneither_ptr){struct _tuple1*_tmp46D=Cyc_Toc_temp_var();
void*_tmp46E=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp46F=
is_dynforward_ptr?Cyc_Toc__tag_dynforward_e: Cyc_Toc__tag_dyneither_e;struct Cyc_Absyn_Exp*
_tmp470=Cyc_Absyn_fncall_exp(_tmp46F,({struct Cyc_Absyn_Exp*_tmp473[3];_tmp473[2]=
_tmp467;_tmp473[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp473[0]=Cyc_Absyn_var_exp(
a,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp473,sizeof(struct Cyc_Absyn_Exp*),3));}),0);decls=({struct Cyc_List_List*
_tmp471=_region_malloc(r,sizeof(*_tmp471));_tmp471->hd=({struct _tuple10*_tmp472=
_region_malloc(r,sizeof(*_tmp472));_tmp472->f1=_tmp46D;_tmp472->f2=_tmp46E;
_tmp472->f3=(struct Cyc_Absyn_Exp*)_tmp470;_tmp472;});_tmp471->tl=decls;_tmp471;});
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp46D,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp474=decls;for(0;_tmp474 != 0;_tmp474=_tmp474->tl){struct
_tuple1*_tmp476;void*_tmp477;struct Cyc_Absyn_Exp*_tmp478;struct _tuple10 _tmp475=*((
struct _tuple10*)_tmp474->hd);_tmp476=_tmp475.f1;_tmp477=_tmp475.f2;_tmp478=
_tmp475.f3;s=Cyc_Absyn_declare_stmt(_tmp476,_tmp477,_tmp478,s,0);}}(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(s));}};_pop_region(r);}goto _LL2B6;}}}}_LL2BB: if(*((int*)
_tmp447)!= 30)goto _LL2BD;_tmp44D=((struct Cyc_Absyn_Struct_e_struct*)_tmp447)->f1;
_tmp44E=((struct Cyc_Absyn_Struct_e_struct*)_tmp447)->f2;_tmp44F=((struct Cyc_Absyn_Struct_e_struct*)
_tmp447)->f3;_LL2BC:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(nv,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp34C->topt))->v,_tmp44E != 0,1,_tmp34B,_tmp44F,
_tmp44D))->r));goto _LL2B6;_LL2BD: if(*((int*)_tmp447)!= 26)goto _LL2BF;_tmp450=((
struct Cyc_Absyn_Tuple_e_struct*)_tmp447)->f1;_LL2BE:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_tuple(nv,1,_tmp34B,_tmp450))->r));goto _LL2B6;_LL2BF:;_LL2C0: {void*
old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp34C->topt))->v;void*
elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct _tuple1*_tmp47B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp47C=Cyc_Absyn_var_exp(_tmp47B,0);struct Cyc_Absyn_Exp*
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp47C,0),0);struct Cyc_Absyn_Exp*
inner_mexp=mexp;if(_tmp34B == 0)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp34B;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;int forward_only=0;{void*_tmp47D=(void*)
_tmp34C->r;void*_tmp47E;struct Cyc_Absyn_Exp*_tmp47F;_LL2CC: if(*((int*)_tmp47D)!= 
15)goto _LL2CE;_tmp47E=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp47D)->f1;
_tmp47F=((struct Cyc_Absyn_Cast_e_struct*)_tmp47D)->f2;_LL2CD:{struct _tuple0
_tmp481=({struct _tuple0 _tmp480;_tmp480.f1=Cyc_Tcutil_compress(_tmp47E);_tmp480.f2=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp47F->topt))->v);
_tmp480;});void*_tmp482;struct Cyc_Absyn_PtrInfo _tmp483;void*_tmp484;struct Cyc_Absyn_PtrAtts
_tmp485;struct Cyc_Absyn_Conref*_tmp486;void*_tmp487;struct Cyc_Absyn_PtrInfo
_tmp488;struct Cyc_Absyn_PtrAtts _tmp489;struct Cyc_Absyn_Conref*_tmp48A;_LL2D1:
_tmp482=_tmp481.f1;if(_tmp482 <= (void*)4)goto _LL2D3;if(*((int*)_tmp482)!= 4)goto
_LL2D3;_tmp483=((struct Cyc_Absyn_PointerType_struct*)_tmp482)->f1;_tmp484=(void*)
_tmp483.elt_typ;_tmp485=_tmp483.ptr_atts;_tmp486=_tmp485.bounds;_tmp487=_tmp481.f2;
if(_tmp487 <= (void*)4)goto _LL2D3;if(*((int*)_tmp487)!= 4)goto _LL2D3;_tmp488=((
struct Cyc_Absyn_PointerType_struct*)_tmp487)->f1;_tmp489=_tmp488.ptr_atts;
_tmp48A=_tmp489.bounds;_LL2D2:{struct _tuple0 _tmp48C=({struct _tuple0 _tmp48B;
_tmp48B.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp486);_tmp48B.f2=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp48A);_tmp48B;});void*_tmp48D;void*_tmp48E;struct Cyc_Absyn_Exp*
_tmp48F;void*_tmp490;void*_tmp491;struct Cyc_Absyn_Exp*_tmp492;_LL2D6: _tmp48D=
_tmp48C.f1;if((int)_tmp48D != 0)goto _LL2D8;_tmp48E=_tmp48C.f2;if(_tmp48E <= (void*)
2)goto _LL2D8;if(*((int*)_tmp48E)!= 0)goto _LL2D8;_tmp48F=((struct Cyc_Absyn_Upper_b_struct*)
_tmp48E)->f1;_LL2D7: forward_only=1;_tmp492=_tmp48F;goto _LL2D9;_LL2D8: _tmp490=
_tmp48C.f1;if((int)_tmp490 != 1)goto _LL2DA;_tmp491=_tmp48C.f2;if(_tmp491 <= (void*)
2)goto _LL2DA;if(*((int*)_tmp491)!= 0)goto _LL2DA;_tmp492=((struct Cyc_Absyn_Upper_b_struct*)
_tmp491)->f1;_LL2D9: Cyc_Toc_exp_to_c(nv,_tmp47F);(void*)(inner_mexp->r=(void*)
Cyc_Toc_sizeoftyp_exp_r(elt_typ));done=1;{struct Cyc_Absyn_Exp*_tmp493=
forward_only?Cyc_Toc__init_dynforward_ptr_e: Cyc_Toc__init_dyneither_ptr_e;(void*)(
e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp493,({struct Cyc_Absyn_Exp*_tmp494[4];
_tmp494[3]=_tmp492;_tmp494[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp484),
0);_tmp494[1]=_tmp47F;_tmp494[0]=mexp;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp494,sizeof(struct Cyc_Absyn_Exp*),
4));})));goto _LL2D5;}_LL2DA:;_LL2DB: goto _LL2D5;_LL2D5:;}goto _LL2D0;_LL2D3:;
_LL2D4: goto _LL2D0;_LL2D0:;}goto _LL2CB;_LL2CE:;_LL2CF: goto _LL2CB;_LL2CB:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp495=Cyc_Absyn_exp_stmt(_tmp47C,0);struct Cyc_Absyn_Exp*
_tmp496=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp34C);_tmp495=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp47C,_tmp496,0),_tmp34C,0),
_tmp495,0);{void*_tmp497=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp47B,_tmp497,(struct Cyc_Absyn_Exp*)
mexp,_tmp495,0)));}}goto _LL2B6;}}_LL2B6:;}goto _LL1E3;_LL20C: if(*((int*)_tmp320)
!= 19)goto _LL20E;_tmp34D=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp320)->f1;
_LL20D: Cyc_Toc_exp_to_c(nv,_tmp34D);goto _LL1E3;_LL20E: if(*((int*)_tmp320)!= 18)
goto _LL210;_tmp34E=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp320)->f1;
_LL20F:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp498=
_cycalloc(sizeof(*_tmp498));_tmp498[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp499;_tmp499.tag=18;_tmp499.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp34E);_tmp499;});
_tmp498;})));goto _LL1E3;_LL210: if(*((int*)_tmp320)!= 21)goto _LL212;_LL211:({void*
_tmp49A[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp49B="__gen() in code generator";_tag_dynforward(_tmp49B,sizeof(
char),_get_zero_arr_size(_tmp49B,26));}),_tag_dynforward(_tmp49A,sizeof(void*),0));});
_LL212: if(*((int*)_tmp320)!= 20)goto _LL214;_tmp34F=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp320)->f1;_tmp350=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp320)->f2;
if(*((int*)_tmp350)!= 0)goto _LL214;_tmp351=((struct Cyc_Absyn_StructField_struct*)
_tmp350)->f1;_LL213:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp49C=_cycalloc(sizeof(*_tmp49C));_tmp49C[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp49D;_tmp49D.tag=20;_tmp49D.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp34F);_tmp49D.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp49E=_cycalloc(sizeof(*
_tmp49E));_tmp49E[0]=({struct Cyc_Absyn_StructField_struct _tmp49F;_tmp49F.tag=0;
_tmp49F.f1=_tmp351;_tmp49F;});_tmp49E;}));_tmp49D;});_tmp49C;})));goto _LL1E3;
_LL214: if(*((int*)_tmp320)!= 20)goto _LL216;_tmp352=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp320)->f1;_tmp353=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp320)->f2;
if(*((int*)_tmp353)!= 1)goto _LL216;_tmp354=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp353)->f1;_LL215:{void*_tmp4A0=Cyc_Tcutil_compress(_tmp352);struct Cyc_Absyn_AggrInfo
_tmp4A1;void*_tmp4A2;struct Cyc_List_List*_tmp4A3;_LL2DD: if(_tmp4A0 <= (void*)4)
goto _LL2E5;if(*((int*)_tmp4A0)!= 10)goto _LL2DF;_tmp4A1=((struct Cyc_Absyn_AggrType_struct*)
_tmp4A0)->f1;_tmp4A2=(void*)_tmp4A1.aggr_info;_LL2DE: {struct Cyc_Absyn_Aggrdecl*
_tmp4A4=Cyc_Absyn_get_known_aggrdecl(_tmp4A2);if(_tmp4A4->impl == 0)({void*
_tmp4A5[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp4A6="struct fields must be known";_tag_dynforward(_tmp4A6,sizeof(
char),_get_zero_arr_size(_tmp4A6,28));}),_tag_dynforward(_tmp4A5,sizeof(void*),0));});
_tmp4A3=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4A4->impl))->fields;goto
_LL2E0;}_LL2DF: if(*((int*)_tmp4A0)!= 11)goto _LL2E1;_tmp4A3=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp4A0)->f2;_LL2E0: {struct Cyc_Absyn_Aggrfield*_tmp4A7=((struct Cyc_Absyn_Aggrfield*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp4A3,(int)_tmp354);(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp4A8=_cycalloc(sizeof(*
_tmp4A8));_tmp4A8[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp4A9;_tmp4A9.tag=20;
_tmp4A9.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp352);_tmp4A9.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp4AA=_cycalloc(sizeof(*_tmp4AA));_tmp4AA[0]=({
struct Cyc_Absyn_StructField_struct _tmp4AB;_tmp4AB.tag=0;_tmp4AB.f1=_tmp4A7->name;
_tmp4AB;});_tmp4AA;}));_tmp4A9;});_tmp4A8;})));goto _LL2DC;}_LL2E1: if(*((int*)
_tmp4A0)!= 9)goto _LL2E3;_LL2E2:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp4AD;_tmp4AD.tag=20;_tmp4AD.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp352);_tmp4AD.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp4AE=_cycalloc(sizeof(*
_tmp4AE));_tmp4AE[0]=({struct Cyc_Absyn_StructField_struct _tmp4AF;_tmp4AF.tag=0;
_tmp4AF.f1=Cyc_Absyn_fieldname((int)(_tmp354 + 1));_tmp4AF;});_tmp4AE;}));_tmp4AD;});
_tmp4AC;})));goto _LL2DC;_LL2E3: if(*((int*)_tmp4A0)!= 3)goto _LL2E5;_LL2E4: if(
_tmp354 == 0)(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp4B0=_cycalloc(sizeof(*_tmp4B0));_tmp4B0[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp4B1;_tmp4B1.tag=20;_tmp4B1.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp352);_tmp4B1.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp4B2=_cycalloc(sizeof(*
_tmp4B2));_tmp4B2[0]=({struct Cyc_Absyn_StructField_struct _tmp4B3;_tmp4B3.tag=0;
_tmp4B3.f1=Cyc_Toc_tag_sp;_tmp4B3;});_tmp4B2;}));_tmp4B1;});_tmp4B0;})));else{(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp4B4=_cycalloc(
sizeof(*_tmp4B4));_tmp4B4[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp4B5;_tmp4B5.tag=
20;_tmp4B5.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp352);_tmp4B5.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp4B6=_cycalloc(sizeof(*_tmp4B6));_tmp4B6[0]=({
struct Cyc_Absyn_StructField_struct _tmp4B7;_tmp4B7.tag=0;_tmp4B7.f1=Cyc_Absyn_fieldname((
int)_tmp354);_tmp4B7;});_tmp4B6;}));_tmp4B5;});_tmp4B4;})));}goto _LL2DC;_LL2E5:;
_LL2E6:({void*_tmp4B8[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4B9="impossible type for offsetof tuple index";
_tag_dynforward(_tmp4B9,sizeof(char),_get_zero_arr_size(_tmp4B9,41));}),
_tag_dynforward(_tmp4B8,sizeof(void*),0));});_LL2DC:;}goto _LL1E3;_LL216: if(*((
int*)_tmp320)!= 22)goto _LL218;_tmp355=((struct Cyc_Absyn_Deref_e_struct*)_tmp320)->f1;
_LL217: {void*_tmp4BA=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp355->topt))->v);{void*_tmp4BB=_tmp4BA;struct Cyc_Absyn_PtrInfo
_tmp4BC;void*_tmp4BD;struct Cyc_Absyn_Tqual _tmp4BE;struct Cyc_Absyn_PtrAtts _tmp4BF;
void*_tmp4C0;struct Cyc_Absyn_Conref*_tmp4C1;struct Cyc_Absyn_Conref*_tmp4C2;
struct Cyc_Absyn_Conref*_tmp4C3;_LL2E8: if(_tmp4BB <= (void*)4)goto _LL2EA;if(*((int*)
_tmp4BB)!= 4)goto _LL2EA;_tmp4BC=((struct Cyc_Absyn_PointerType_struct*)_tmp4BB)->f1;
_tmp4BD=(void*)_tmp4BC.elt_typ;_tmp4BE=_tmp4BC.elt_tq;_tmp4BF=_tmp4BC.ptr_atts;
_tmp4C0=(void*)_tmp4BF.rgn;_tmp4C1=_tmp4BF.nullable;_tmp4C2=_tmp4BF.bounds;
_tmp4C3=_tmp4BF.zero_term;_LL2E9:{void*_tmp4C4=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp4C2);_LL2ED: if(_tmp4C4 <= (void*)2)goto _LL2EF;if(*((int*)_tmp4C4)!= 0)goto
_LL2EF;_LL2EE: {int do_null_check=Cyc_Toc_need_null_check(_tmp355);Cyc_Toc_exp_to_c(
nv,_tmp355);if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*_tmp4C5[0]={};
Cyc_Tcutil_warn(e->loc,({const char*_tmp4C6="inserted null check due to dereference";
_tag_dynforward(_tmp4C6,sizeof(char),_get_zero_arr_size(_tmp4C6,39));}),
_tag_dynforward(_tmp4C5,sizeof(void*),0));});(void*)(_tmp355->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Toc_typ_to_c(_tmp4BA),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp4C7=_cycalloc(sizeof(*_tmp4C7));_tmp4C7->hd=Cyc_Absyn_copy_exp(_tmp355);
_tmp4C7->tl=0;_tmp4C7;}),0)));}goto _LL2EC;}_LL2EF: if((int)_tmp4C4 != 0)goto _LL2F1;
_LL2F0: goto _LL2F2;_LL2F1: if((int)_tmp4C4 != 1)goto _LL2F3;_LL2F2: {struct Cyc_Absyn_Exp*
_tmp4C8=Cyc_Absyn_uint_exp(0,0);_tmp4C8->topt=({struct Cyc_Core_Opt*_tmp4C9=
_cycalloc(sizeof(*_tmp4C9));_tmp4C9->v=(void*)Cyc_Absyn_uint_typ;_tmp4C9;});(
void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(_tmp355,_tmp4C8));Cyc_Toc_exp_to_c(nv,
e);goto _LL2EC;}_LL2F3: if(_tmp4C4 <= (void*)2)goto _LL2EC;if(*((int*)_tmp4C4)!= 1)
goto _LL2EC;_LL2F4:({void*_tmp4CA[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp4CB="exp_to_c: deref w/ AbsUpper_b";
_tag_dynforward(_tmp4CB,sizeof(char),_get_zero_arr_size(_tmp4CB,30));}),
_tag_dynforward(_tmp4CA,sizeof(void*),0));});_LL2EC:;}goto _LL2E7;_LL2EA:;_LL2EB:({
void*_tmp4CC[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp4CD="exp_to_c: Deref: non-pointer";
_tag_dynforward(_tmp4CD,sizeof(char),_get_zero_arr_size(_tmp4CD,29));}),
_tag_dynforward(_tmp4CC,sizeof(void*),0));});_LL2E7:;}goto _LL1E3;}_LL218: if(*((
int*)_tmp320)!= 23)goto _LL21A;_tmp356=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp320)->f1;_tmp357=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp320)->f2;_LL219:
Cyc_Toc_exp_to_c(nv,_tmp356);if(Cyc_Toc_is_poly_project(e))(void*)(e->r=(void*)((
void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1E3;
_LL21A: if(*((int*)_tmp320)!= 24)goto _LL21C;_tmp358=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp320)->f1;_tmp359=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp320)->f2;_LL21B: {
void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp358->topt))->v);
int do_null_check=Cyc_Toc_need_null_check(_tmp358);Cyc_Toc_exp_to_c(nv,_tmp358);{
int is_poly=Cyc_Toc_is_poly_project(e);int forward_only=0;void*_tmp4CF;struct Cyc_Absyn_Tqual
_tmp4D0;struct Cyc_Absyn_PtrAtts _tmp4D1;void*_tmp4D2;struct Cyc_Absyn_Conref*
_tmp4D3;struct Cyc_Absyn_Conref*_tmp4D4;struct Cyc_Absyn_Conref*_tmp4D5;struct Cyc_Absyn_PtrInfo
_tmp4CE=Cyc_Toc_get_ptr_type(e1typ);_tmp4CF=(void*)_tmp4CE.elt_typ;_tmp4D0=
_tmp4CE.elt_tq;_tmp4D1=_tmp4CE.ptr_atts;_tmp4D2=(void*)_tmp4D1.rgn;_tmp4D3=
_tmp4D1.nullable;_tmp4D4=_tmp4D1.bounds;_tmp4D5=_tmp4D1.zero_term;{void*_tmp4D6=
Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4D4);struct Cyc_Absyn_Exp*_tmp4D7;
_LL2F6: if(_tmp4D6 <= (void*)2)goto _LL2F8;if(*((int*)_tmp4D6)!= 0)goto _LL2F8;
_tmp4D7=((struct Cyc_Absyn_Upper_b_struct*)_tmp4D6)->f1;_LL2F7: {unsigned int
_tmp4D9;int _tmp4DA;struct _tuple5 _tmp4D8=Cyc_Evexp_eval_const_uint_exp(_tmp4D7);
_tmp4D9=_tmp4D8.f1;_tmp4DA=_tmp4D8.f2;if(_tmp4DA){if(_tmp4D9 < 1)({void*_tmp4DB[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp4DC="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dynforward(
_tmp4DC,sizeof(char),_get_zero_arr_size(_tmp4DC,44));}),_tag_dynforward(_tmp4DB,
sizeof(void*),0));});if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*
_tmp4DD[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp4DE="inserted null check due to dereference";
_tag_dynforward(_tmp4DE,sizeof(char),_get_zero_arr_size(_tmp4DE,39));}),
_tag_dynforward(_tmp4DD,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_aggrarrow_exp_r(
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp358->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp4DF=_cycalloc(sizeof(*_tmp4DF));_tmp4DF->hd=_tmp358;_tmp4DF->tl=0;_tmp4DF;}),
0)),_tmp359));}}else{(void*)(e->r=(void*)Cyc_Toc_aggrarrow_exp_r(Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp358->topt))->v),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*
_tmp4E0[4];_tmp4E0[3]=Cyc_Absyn_uint_exp(0,0);_tmp4E0[2]=Cyc_Absyn_sizeoftyp_exp(
_tmp4CF,0);_tmp4E0[1]=_tmp4D7;_tmp4E0[0]=_tmp358;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4E0,sizeof(struct Cyc_Absyn_Exp*),
4));}),0)),_tmp359));}goto _LL2F5;}_LL2F8: if((int)_tmp4D6 != 0)goto _LL2FA;_LL2F9:
forward_only=1;goto _LL2FB;_LL2FA: if((int)_tmp4D6 != 1)goto _LL2FC;_LL2FB: {struct
Cyc_Absyn_Exp*_tmp4E1=forward_only?Cyc_Toc__check_dynforward_subscript_e: Cyc_Toc__check_dyneither_subscript_e;
void*ta1=Cyc_Toc_typ_to_c_array(_tmp4CF);(void*)(_tmp358->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_cstar_typ(ta1,_tmp4D0),Cyc_Absyn_fncall_exp(_tmp4E1,({struct Cyc_Absyn_Exp*
_tmp4E2[3];_tmp4E2[2]=Cyc_Absyn_uint_exp(0,0);_tmp4E2[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp4E2[0]=Cyc_Absyn_copy_exp(_tmp358);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4E2,sizeof(struct Cyc_Absyn_Exp*),
3));}),0)));goto _LL2F5;}_LL2FC: if(_tmp4D6 <= (void*)2)goto _LL2F5;if(*((int*)
_tmp4D6)!= 1)goto _LL2F5;_LL2FD:({void*_tmp4E3[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp4E4="exp_to_c: AggrArrow w/ AbsUpper_b";
_tag_dynforward(_tmp4E4,sizeof(char),_get_zero_arr_size(_tmp4E4,34));}),
_tag_dynforward(_tmp4E3,sizeof(void*),0));});_LL2F5:;}if(is_poly)(void*)(e->r=(
void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1E3;}}
_LL21C: if(*((int*)_tmp320)!= 25)goto _LL21E;_tmp35A=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp320)->f1;_tmp35B=((struct Cyc_Absyn_Subscript_e_struct*)_tmp320)->f2;_LL21D: {
void*_tmp4E5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp35A->topt))->v);{void*_tmp4E6=_tmp4E5;struct Cyc_List_List*_tmp4E7;struct Cyc_Absyn_PtrInfo
_tmp4E8;void*_tmp4E9;struct Cyc_Absyn_Tqual _tmp4EA;struct Cyc_Absyn_PtrAtts _tmp4EB;
void*_tmp4EC;struct Cyc_Absyn_Conref*_tmp4ED;struct Cyc_Absyn_Conref*_tmp4EE;
struct Cyc_Absyn_Conref*_tmp4EF;_LL2FF: if(_tmp4E6 <= (void*)4)goto _LL303;if(*((int*)
_tmp4E6)!= 9)goto _LL301;_tmp4E7=((struct Cyc_Absyn_TupleType_struct*)_tmp4E6)->f1;
_LL300: Cyc_Toc_exp_to_c(nv,_tmp35A);Cyc_Toc_exp_to_c(nv,_tmp35B);{unsigned int
_tmp4F1;int _tmp4F2;struct _tuple5 _tmp4F0=Cyc_Evexp_eval_const_uint_exp(_tmp35B);
_tmp4F1=_tmp4F0.f1;_tmp4F2=_tmp4F0.f2;if(!_tmp4F2)({void*_tmp4F3[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp4F4="unknown tuple subscript in translation to C";_tag_dynforward(_tmp4F4,
sizeof(char),_get_zero_arr_size(_tmp4F4,44));}),_tag_dynforward(_tmp4F3,sizeof(
void*),0));});(void*)(e->r=(void*)Cyc_Toc_aggrmember_exp_r(_tmp35A,Cyc_Absyn_fieldname((
int)(_tmp4F1 + 1))));goto _LL2FE;}_LL301: if(*((int*)_tmp4E6)!= 4)goto _LL303;
_tmp4E8=((struct Cyc_Absyn_PointerType_struct*)_tmp4E6)->f1;_tmp4E9=(void*)
_tmp4E8.elt_typ;_tmp4EA=_tmp4E8.elt_tq;_tmp4EB=_tmp4E8.ptr_atts;_tmp4EC=(void*)
_tmp4EB.rgn;_tmp4ED=_tmp4EB.nullable;_tmp4EE=_tmp4EB.bounds;_tmp4EF=_tmp4EB.zero_term;
_LL302: {struct Cyc_List_List*_tmp4F5=Cyc_Toc_get_relns(_tmp35A);int in_bnds=0;{
void*_tmp4F6=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4EE);_LL306: if(
_tmp4F6 <= (void*)2)goto _LL308;if(*((int*)_tmp4F6)!= 1)goto _LL308;_LL307: goto
_LL305;_LL308:;_LL309: in_bnds=Cyc_Toc_check_bounds(_tmp4F5,_tmp35A,_tmp35B);if(
Cyc_Toc_warn_bounds_checks  && !in_bnds)({struct Cyc_String_pa_struct _tmp4F9;
_tmp4F9.tag=0;_tmp4F9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp4F7[1]={& _tmp4F9};Cyc_Tcutil_warn(e->loc,({const char*_tmp4F8="bounds check necessary for %s";
_tag_dynforward(_tmp4F8,sizeof(char),_get_zero_arr_size(_tmp4F8,30));}),
_tag_dynforward(_tmp4F7,sizeof(void*),1));}});_LL305:;}Cyc_Toc_exp_to_c(nv,
_tmp35A);Cyc_Toc_exp_to_c(nv,_tmp35B);Cyc_Toc_total_bounds_checks ++;{int
forward_only=0;{void*_tmp4FA=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4EE);
struct Cyc_Absyn_Exp*_tmp4FB;void*_tmp4FC;_LL30B: if(_tmp4FA <= (void*)2)goto _LL30D;
if(*((int*)_tmp4FA)!= 0)goto _LL30D;_tmp4FB=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4FA)->f1;_LL30C: {int possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp4ED);void*ta1=Cyc_Toc_typ_to_c(_tmp4E9);void*ta2=Cyc_Absyn_cstar_typ(
ta1,_tmp4EA);if(in_bnds)Cyc_Toc_bounds_checks_eliminated ++;else{if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp4EF))(void*)(e->r=(void*)
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp4FD[3];_tmp4FD[2]=_tmp35B;_tmp4FD[1]=_tmp4FB;_tmp4FD[0]=
_tmp35A;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp4FD,sizeof(struct Cyc_Absyn_Exp*),3));}),0))));else{if(
possibly_null){if(Cyc_Toc_warn_all_null_deref)({void*_tmp4FE[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp4FF="inserted null check due to dereference";
_tag_dynforward(_tmp4FF,sizeof(char),_get_zero_arr_size(_tmp4FF,39));}),
_tag_dynforward(_tmp4FE,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_deref_exp_r(
Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp500[4];_tmp500[3]=_tmp35B;_tmp500[2]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp500[1]=_tmp4FB;_tmp500[0]=_tmp35A;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp500,sizeof(struct Cyc_Absyn_Exp*),
4));}),0))));}else{(void*)(_tmp35B->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp501[2];_tmp501[1]=Cyc_Absyn_copy_exp(_tmp35B);_tmp501[0]=
_tmp4FB;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp501,sizeof(struct Cyc_Absyn_Exp*),2));})));}}}goto _LL30A;}
_LL30D: if((int)_tmp4FA != 0)goto _LL30F;_LL30E: forward_only=1;goto _LL310;_LL30F:
if((int)_tmp4FA != 1)goto _LL311;_LL310: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp4E9);
if(in_bnds){Cyc_Toc_bounds_checks_eliminated ++;(void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp4EA),Cyc_Absyn_aggrmember_exp(
_tmp35A,Cyc_Toc_curr_sp,0)),_tmp35B));}else{struct Cyc_Absyn_Exp*_tmp502=
forward_only?Cyc_Toc__check_dynforward_subscript_e: Cyc_Toc__check_dyneither_subscript_e;(
void*)(e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,
_tmp4EA),Cyc_Absyn_fncall_exp(_tmp502,({struct Cyc_Absyn_Exp*_tmp503[3];_tmp503[2]=
_tmp35B;_tmp503[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp503[0]=_tmp35A;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp503,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))));}goto _LL30A;}_LL311: if(_tmp4FA <= (void*)2)goto _LL30A;if(*((int*)
_tmp4FA)!= 1)goto _LL30A;_tmp4FC=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp4FA)->f1;_LL312:{void*_tmp504=(void*)_tmp35B->annot;struct Cyc_List_List*
_tmp505;_LL314: if(*((void**)_tmp504)!= Cyc_CfFlowInfo_HasTagCmps)goto _LL316;
_tmp505=((struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp504)->f1;_LL315: for(0;
_tmp505 != 0;_tmp505=_tmp505->tl){if((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp505->hd)->cmp == (void*)8  && Cyc_Tcutil_typecmp((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp505->hd)->bd,_tmp4FC)== 0){in_bnds=1;break;}}goto _LL313;_LL316:;_LL317: goto
_LL313;_LL313:;}if(!in_bnds)({void*_tmp506[0]={};Cyc_Tcutil_terr(e->loc,({const
char*_tmp507="cannot determine that subscript is in bounds";_tag_dynforward(
_tmp507,sizeof(char),_get_zero_arr_size(_tmp507,45));}),_tag_dynforward(_tmp506,
sizeof(void*),0));});goto _LL30A;_LL30A:;}goto _LL2FE;}}_LL303:;_LL304:({void*
_tmp508[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp509="exp_to_c: Subscript on non-tuple/array/tuple ptr";
_tag_dynforward(_tmp509,sizeof(char),_get_zero_arr_size(_tmp509,49));}),
_tag_dynforward(_tmp508,sizeof(void*),0));});_LL2FE:;}goto _LL1E3;}_LL21E: if(*((
int*)_tmp320)!= 26)goto _LL220;_tmp35C=((struct Cyc_Absyn_Tuple_e_struct*)_tmp320)->f1;
_LL21F: if(!nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,0,0,
_tmp35C))->r));else{struct Cyc_List_List*_tmp50A=((struct Cyc_List_List*(*)(struct
_tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,
_tmp35C);struct _dynforward_ptr*_tmp50B=Cyc_Toc_add_tuple_type(_tmp50A);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp35C != 0;(_tmp35C=_tmp35C->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp35C->hd);dles=({struct Cyc_List_List*_tmp50C=_cycalloc(
sizeof(*_tmp50C));_tmp50C->hd=({struct _tuple6*_tmp50D=_cycalloc(sizeof(*_tmp50D));
_tmp50D->f1=0;_tmp50D->f2=(struct Cyc_Absyn_Exp*)_tmp35C->hd;_tmp50D;});_tmp50C->tl=
dles;_tmp50C;});}}dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles);(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,dles));}goto _LL1E3;
_LL220: if(*((int*)_tmp320)!= 28)goto _LL222;_tmp35D=((struct Cyc_Absyn_Array_e_struct*)
_tmp320)->f1;_LL221:(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,_tmp35D));{
struct Cyc_List_List*_tmp50E=_tmp35D;for(0;_tmp50E != 0;_tmp50E=_tmp50E->tl){
struct _tuple6 _tmp510;struct Cyc_Absyn_Exp*_tmp511;struct _tuple6*_tmp50F=(struct
_tuple6*)_tmp50E->hd;_tmp510=*_tmp50F;_tmp511=_tmp510.f2;Cyc_Toc_exp_to_c(nv,
_tmp511);}}goto _LL1E3;_LL222: if(*((int*)_tmp320)!= 29)goto _LL224;_tmp35E=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp320)->f1;_tmp35F=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp320)->f2;_tmp360=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp320)->f3;
_tmp361=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp320)->f4;_LL223: {
unsigned int _tmp513;int _tmp514;struct _tuple5 _tmp512=Cyc_Evexp_eval_const_uint_exp(
_tmp35F);_tmp513=_tmp512.f1;_tmp514=_tmp512.f2;{void*_tmp515=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp360->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp360);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp360)){if(!_tmp514)({
void*_tmp516[0]={};Cyc_Tcutil_terr(_tmp35F->loc,({const char*_tmp517="cannot determine value of constant";
_tag_dynforward(_tmp517,sizeof(char),_get_zero_arr_size(_tmp517,35));}),
_tag_dynforward(_tmp516,sizeof(void*),0));});{unsigned int i=0;for(0;i < _tmp513;i
++){es=({struct Cyc_List_List*_tmp518=_cycalloc(sizeof(*_tmp518));_tmp518->hd=({
struct _tuple6*_tmp519=_cycalloc(sizeof(*_tmp519));_tmp519->f1=0;_tmp519->f2=
_tmp360;_tmp519;});_tmp518->tl=es;_tmp518;});}}if(_tmp361){struct Cyc_Absyn_Exp*
_tmp51A=Cyc_Toc_cast_it(_tmp515,Cyc_Absyn_uint_exp(0,0));es=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*
_tmp51B=_cycalloc(sizeof(*_tmp51B));_tmp51B->hd=({struct _tuple6*_tmp51C=
_cycalloc(sizeof(*_tmp51C));_tmp51C->f1=0;_tmp51C->f2=_tmp51A;_tmp51C;});_tmp51B->tl=
0;_tmp51B;}));}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,es));goto
_LL1E3;}}}_LL224: if(*((int*)_tmp320)!= 30)goto _LL226;_tmp362=((struct Cyc_Absyn_Struct_e_struct*)
_tmp320)->f1;_tmp363=((struct Cyc_Absyn_Struct_e_struct*)_tmp320)->f2;_tmp364=((
struct Cyc_Absyn_Struct_e_struct*)_tmp320)->f3;_tmp365=((struct Cyc_Absyn_Struct_e_struct*)
_tmp320)->f4;_LL225: if(!nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,old_typ,_tmp363 != 0,0,0,_tmp364,_tmp362))->r));else{if(_tmp365 == 0)({void*
_tmp51D[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp51E="Struct_e: missing aggrdecl pointer";_tag_dynforward(_tmp51E,
sizeof(char),_get_zero_arr_size(_tmp51E,35));}),_tag_dynforward(_tmp51D,sizeof(
void*),0));});{struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp365;
struct _RegionHandle _tmp51F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp51F;
_push_region(rgn);{struct Cyc_List_List*_tmp520=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp364,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);struct Cyc_List_List*
_tmp521=0;struct Cyc_List_List*_tmp522=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp522 != 0;_tmp522=_tmp522->tl){struct Cyc_List_List*
_tmp523=_tmp520;for(0;_tmp523 != 0;_tmp523=_tmp523->tl){if((*((struct _tuple11*)
_tmp523->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp522->hd){struct _tuple11
_tmp525;struct Cyc_Absyn_Aggrfield*_tmp526;struct Cyc_Absyn_Exp*_tmp527;struct
_tuple11*_tmp524=(struct _tuple11*)_tmp523->hd;_tmp525=*_tmp524;_tmp526=_tmp525.f1;
_tmp527=_tmp525.f2;{void*_tmp528=(void*)_tmp526->type;Cyc_Toc_exp_to_c(nv,
_tmp527);if(Cyc_Toc_is_void_star(_tmp528))(void*)(_tmp527->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp527->r,0)));_tmp521=({
struct Cyc_List_List*_tmp529=_cycalloc(sizeof(*_tmp529));_tmp529->hd=({struct
_tuple6*_tmp52A=_cycalloc(sizeof(*_tmp52A));_tmp52A->f1=0;_tmp52A->f2=_tmp527;
_tmp52A;});_tmp529->tl=_tmp521;_tmp529;});break;}}}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp521)));};
_pop_region(rgn);}}goto _LL1E3;_LL226: if(*((int*)_tmp320)!= 31)goto _LL228;_tmp366=(
void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp320)->f1;_tmp367=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp320)->f2;_LL227: {struct Cyc_List_List*fs;{void*_tmp52B=Cyc_Tcutil_compress(
_tmp366);struct Cyc_List_List*_tmp52C;_LL319: if(_tmp52B <= (void*)4)goto _LL31B;if(*((
int*)_tmp52B)!= 11)goto _LL31B;_tmp52C=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp52B)->f2;_LL31A: fs=_tmp52C;goto _LL318;_LL31B:;_LL31C:({struct Cyc_String_pa_struct
_tmp52F;_tmp52F.tag=0;_tmp52F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(_tmp366));{void*_tmp52D[1]={& _tmp52F};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp52E="anon struct has type %s";_tag_dynforward(_tmp52E,sizeof(char),
_get_zero_arr_size(_tmp52E,24));}),_tag_dynforward(_tmp52D,sizeof(void*),1));}});
_LL318:;}{struct _RegionHandle _tmp530=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp530;_push_region(rgn);{struct Cyc_List_List*_tmp531=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,
_tmp367,fs);for(0;_tmp531 != 0;_tmp531=_tmp531->tl){struct _tuple11 _tmp533;struct
Cyc_Absyn_Aggrfield*_tmp534;struct Cyc_Absyn_Exp*_tmp535;struct _tuple11*_tmp532=(
struct _tuple11*)_tmp531->hd;_tmp533=*_tmp532;_tmp534=_tmp533.f1;_tmp535=_tmp533.f2;{
void*_tmp536=(void*)_tmp534->type;Cyc_Toc_exp_to_c(nv,_tmp535);if(Cyc_Toc_is_void_star(
_tmp536))(void*)(_tmp535->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp535->r,0)));}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,_tmp367));};_pop_region(rgn);}goto _LL1E3;}_LL228: if(*((int*)_tmp320)!= 32)goto
_LL22A;_tmp368=((struct Cyc_Absyn_Tunion_e_struct*)_tmp320)->f1;if(_tmp368 != 0)
goto _LL22A;_tmp369=((struct Cyc_Absyn_Tunion_e_struct*)_tmp320)->f2;_tmp36A=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp320)->f3;if(!(!_tmp369->is_flat))goto _LL22A;
_LL229: {struct _tuple1*qv=_tmp36A->name;struct Cyc_Absyn_Exp*tag_exp=_tmp369->is_xtunion?
Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(_tmp369,qv,0);(void*)(e->r=(void*)((
void*)tag_exp->r));goto _LL1E3;}_LL22A: if(*((int*)_tmp320)!= 32)goto _LL22C;
_tmp36B=((struct Cyc_Absyn_Tunion_e_struct*)_tmp320)->f1;_tmp36C=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp320)->f2;_tmp36D=((struct Cyc_Absyn_Tunion_e_struct*)_tmp320)->f3;_LL22B: {
void*tunion_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple1*_tmp537=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp538=Cyc_Absyn_var_exp(_tmp537,0);struct Cyc_Absyn_Exp*
member_exp;if(_tmp36C->is_flat){tunion_ctype=Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(
_tmp36C->name,({const char*_tmp539="_union";_tag_dynforward(_tmp539,sizeof(char),
_get_zero_arr_size(_tmp539,7));})));tag_exp=Cyc_Toc_tunion_tag(_tmp36C,_tmp36D->name,
1);{struct _tuple1 _tmp53B;struct _dynforward_ptr*_tmp53C;struct _tuple1*_tmp53A=
_tmp36D->name;_tmp53B=*_tmp53A;_tmp53C=_tmp53B.f2;member_exp=Cyc_Absyn_aggrmember_exp(
_tmp538,_tmp53C,0);}}else{tunion_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp36D->name,({const char*_tmp53D="_struct";_tag_dynforward(_tmp53D,sizeof(char),
_get_zero_arr_size(_tmp53D,8));})));tag_exp=_tmp36C->is_xtunion?Cyc_Absyn_var_exp(
_tmp36D->name,0): Cyc_Toc_tunion_tag(_tmp36C,_tmp36D->name,1);member_exp=_tmp538;}{
struct Cyc_List_List*_tmp53E=_tmp36D->typs;if(nv->toplevel){struct Cyc_List_List*
dles=0;for(0;_tmp36B != 0;(_tmp36B=_tmp36B->tl,_tmp53E=_tmp53E->tl)){struct Cyc_Absyn_Exp*
cur_e=(struct Cyc_Absyn_Exp*)_tmp36B->hd;void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp53E))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))cur_e=Cyc_Toc_cast_it(field_typ,
cur_e);dles=({struct Cyc_List_List*_tmp53F=_cycalloc(sizeof(*_tmp53F));_tmp53F->hd=({
struct _tuple6*_tmp540=_cycalloc(sizeof(*_tmp540));_tmp540->f1=0;_tmp540->f2=
cur_e;_tmp540;});_tmp53F->tl=dles;_tmp53F;});}dles=({struct Cyc_List_List*_tmp541=
_cycalloc(sizeof(*_tmp541));_tmp541->hd=({struct _tuple6*_tmp542=_cycalloc(
sizeof(*_tmp542));_tmp542->f1=0;_tmp542->f2=tag_exp;_tmp542;});_tmp541->tl=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp541;});(
void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,dles));if(_tmp36C->is_flat)(
void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp36D->name,({const char*_tmp543="_struct";_tag_dynforward(_tmp543,sizeof(char),
_get_zero_arr_size(_tmp543,8));}))),Cyc_Absyn_copy_exp(e)));}else{struct Cyc_List_List*
_tmp544=({struct Cyc_List_List*_tmp54A=_cycalloc(sizeof(*_tmp54A));_tmp54A->hd=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,0),
tag_exp,0);_tmp54A->tl=0;_tmp54A;});{int i=1;for(0;_tmp36B != 0;(((_tmp36B=_tmp36B->tl,
i ++)),_tmp53E=_tmp53E->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp36B->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp53E))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp545=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);_tmp544=({struct Cyc_List_List*_tmp546=_cycalloc(sizeof(*_tmp546));
_tmp546->hd=_tmp545;_tmp546->tl=_tmp544;_tmp546;});}}}{struct Cyc_Absyn_Stmt*
_tmp547=Cyc_Absyn_exp_stmt(_tmp538,0);struct Cyc_Absyn_Stmt*_tmp548=Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*
_tmp549=_cycalloc(sizeof(*_tmp549));_tmp549->hd=_tmp547;_tmp549->tl=_tmp544;
_tmp549;})),0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(
_tmp537,tunion_ctype,0,_tmp548,0)));}}goto _LL1E3;}}_LL22C: if(*((int*)_tmp320)!= 
33)goto _LL22E;_LL22D: goto _LL22F;_LL22E: if(*((int*)_tmp320)!= 34)goto _LL230;
_LL22F: goto _LL1E3;_LL230: if(*((int*)_tmp320)!= 35)goto _LL232;_tmp36E=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp320)->f1;_tmp36F=_tmp36E.is_calloc;_tmp370=_tmp36E.rgn;_tmp371=_tmp36E.elt_type;
_tmp372=_tmp36E.num_elts;_tmp373=_tmp36E.fat_result;_LL231: {void*t_c=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp371)));Cyc_Toc_exp_to_c(nv,_tmp372);if(_tmp373){struct
_tuple1*_tmp54B=Cyc_Toc_temp_var();struct _tuple1*_tmp54C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(
_tmp36F){xexp=_tmp372;if(_tmp370 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp370;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp54B,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp371,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp54B,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp54D[3];_tmp54D[2]=Cyc_Absyn_var_exp(_tmp54B,0);_tmp54D[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp54D[0]=Cyc_Absyn_var_exp(_tmp54C,0);((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp54D,
sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp372,0);if(_tmp370 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp370;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(
_tmp54B,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp371,Cyc_Absyn_var_exp(_tmp54B,0));}
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*
_tmp54E[3];_tmp54E[2]=Cyc_Absyn_var_exp(_tmp54B,0);_tmp54E[1]=Cyc_Absyn_uint_exp(
1,0);_tmp54E[0]=Cyc_Absyn_var_exp(_tmp54C,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp54E,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);}{struct Cyc_Absyn_Stmt*_tmp54F=Cyc_Absyn_declare_stmt(_tmp54B,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp54C,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)Cyc_Toc_stmt_exp_r(_tmp54F));}}else{if(_tmp370 != 0){struct Cyc_Absyn_Exp*
rgn=(struct Cyc_Absyn_Exp*)_tmp370;Cyc_Toc_exp_to_c(nv,rgn);(void*)(e->r=(void*)((
void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0)))->r));}else{(void*)(
e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp371,Cyc_Absyn_sizeoftyp_exp(t_c,0)))->r));}}
goto _LL1E3;}_LL232: if(*((int*)_tmp320)!= 37)goto _LL234;_tmp374=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp320)->f1;_LL233: Cyc_Toc_stmt_to_c(nv,_tmp374);goto _LL1E3;_LL234: if(*((int*)
_tmp320)!= 36)goto _LL236;_LL235:({void*_tmp550[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp551="UnresolvedMem";_tag_dynforward(_tmp551,sizeof(char),_get_zero_arr_size(
_tmp551,14));}),_tag_dynforward(_tmp550,sizeof(void*),0));});_LL236: if(*((int*)
_tmp320)!= 27)goto _LL1E3;_LL237:({void*_tmp552[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp553="compoundlit";
_tag_dynforward(_tmp553,sizeof(char),_get_zero_arr_size(_tmp553,12));}),
_tag_dynforward(_tmp552,sizeof(void*),0));});_LL1E3:;}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);}struct _tuple12{struct Cyc_Toc_Env*f1;struct Cyc_List_List*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple13{struct _tuple1*f1;void*f2;};struct
_tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple12 Cyc_Toc_xlate_pat(
struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct
Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt**tag_fail_stmt,
struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){struct Cyc_Absyn_Stmt*s;{
void*_tmp554=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp555;struct Cyc_Absyn_Vardecl
_tmp556;struct _tuple1*_tmp557;struct Cyc_Absyn_Pat*_tmp558;struct Cyc_Absyn_Vardecl*
_tmp559;struct Cyc_Absyn_Vardecl _tmp55A;struct _tuple1*_tmp55B;struct Cyc_Absyn_Vardecl*
_tmp55C;struct Cyc_Absyn_Pat*_tmp55D;void*_tmp55E;int _tmp55F;char _tmp560;struct
_dynforward_ptr _tmp561;struct Cyc_Absyn_Enumdecl*_tmp562;struct Cyc_Absyn_Enumfield*
_tmp563;void*_tmp564;struct Cyc_Absyn_Enumfield*_tmp565;struct Cyc_Absyn_Tuniondecl*
_tmp566;struct Cyc_Absyn_Tunionfield*_tmp567;struct Cyc_List_List*_tmp568;struct
Cyc_Absyn_Pat*_tmp569;struct Cyc_Absyn_Pat _tmp56A;void*_tmp56B;struct Cyc_Absyn_Tuniondecl*
_tmp56C;struct Cyc_Absyn_Tunionfield*_tmp56D;struct Cyc_List_List*_tmp56E;struct
Cyc_Absyn_Tuniondecl*_tmp56F;struct Cyc_Absyn_Tunionfield*_tmp570;struct Cyc_List_List*
_tmp571;struct Cyc_List_List*_tmp572;struct Cyc_List_List*_tmp573;struct Cyc_Absyn_AggrInfo
_tmp574;void*_tmp575;struct Cyc_List_List*_tmp576;struct Cyc_Absyn_Pat*_tmp577;
_LL31E: if(_tmp554 <= (void*)2)goto _LL322;if(*((int*)_tmp554)!= 0)goto _LL320;
_tmp555=((struct Cyc_Absyn_Var_p_struct*)_tmp554)->f1;_tmp556=*_tmp555;_tmp557=
_tmp556.name;_tmp558=((struct Cyc_Absyn_Var_p_struct*)_tmp554)->f2;_LL31F: return
Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(nv,_tmp557,r),rgn,t,r,path,_tmp558,
tag_fail_stmt,fail_stmt,decls);_LL320: if(*((int*)_tmp554)!= 2)goto _LL322;_tmp559=((
struct Cyc_Absyn_TagInt_p_struct*)_tmp554)->f2;_tmp55A=*_tmp559;_tmp55B=_tmp55A.name;
_LL321: nv=Cyc_Toc_add_varmap(nv,_tmp55B,r);goto _LL323;_LL322: if((int)_tmp554 != 0)
goto _LL324;_LL323: s=Cyc_Toc_skip_stmt_dl();goto _LL31D;_LL324: if(_tmp554 <= (void*)
2)goto _LL326;if(*((int*)_tmp554)!= 1)goto _LL326;_tmp55C=((struct Cyc_Absyn_Reference_p_struct*)
_tmp554)->f1;_tmp55D=((struct Cyc_Absyn_Reference_p_struct*)_tmp554)->f2;_LL325: {
struct _tuple1*_tmp578=Cyc_Toc_temp_var();decls=({struct Cyc_List_List*_tmp579=
_region_malloc(rgn,sizeof(*_tmp579));_tmp579->hd=({struct _tuple13*_tmp57A=
_region_malloc(rgn,sizeof(*_tmp57A));_tmp57A->f1=_tmp578;_tmp57A->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp57A;});_tmp579->tl=decls;_tmp579;});nv=
Cyc_Toc_add_varmap(nv,_tmp55C->name,Cyc_Absyn_var_exp(_tmp578,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp578,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(path,0)),0);{struct _tuple12 _tmp57B=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp55D,tag_fail_stmt,fail_stmt,decls);_tmp57B.f3=Cyc_Absyn_seq_stmt(
s,_tmp57B.f3,0);return _tmp57B;}}_LL326: if((int)_tmp554 != 1)goto _LL328;_LL327: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL31D;_LL328:
if(_tmp554 <= (void*)2)goto _LL32A;if(*((int*)_tmp554)!= 7)goto _LL32A;_tmp55E=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp554)->f1;_tmp55F=((struct Cyc_Absyn_Int_p_struct*)
_tmp554)->f2;_LL329: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp55E,_tmp55F,0),
fail_stmt);goto _LL31D;_LL32A: if(_tmp554 <= (void*)2)goto _LL32C;if(*((int*)_tmp554)
!= 8)goto _LL32C;_tmp560=((struct Cyc_Absyn_Char_p_struct*)_tmp554)->f1;_LL32B: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp560,0),fail_stmt);goto _LL31D;_LL32C:
if(_tmp554 <= (void*)2)goto _LL32E;if(*((int*)_tmp554)!= 9)goto _LL32E;_tmp561=((
struct Cyc_Absyn_Float_p_struct*)_tmp554)->f1;_LL32D: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(
_tmp561,0),fail_stmt);goto _LL31D;_LL32E: if(_tmp554 <= (void*)2)goto _LL330;if(*((
int*)_tmp554)!= 10)goto _LL330;_tmp562=((struct Cyc_Absyn_Enum_p_struct*)_tmp554)->f1;
_tmp563=((struct Cyc_Absyn_Enum_p_struct*)_tmp554)->f2;_LL32F: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp57C=_cycalloc(
sizeof(*_tmp57C));_tmp57C[0]=({struct Cyc_Absyn_Enum_e_struct _tmp57D;_tmp57D.tag=
33;_tmp57D.f1=_tmp563->name;_tmp57D.f2=(struct Cyc_Absyn_Enumdecl*)_tmp562;
_tmp57D.f3=(struct Cyc_Absyn_Enumfield*)_tmp563;_tmp57D;});_tmp57C;}),0),
fail_stmt);goto _LL31D;_LL330: if(_tmp554 <= (void*)2)goto _LL332;if(*((int*)_tmp554)
!= 11)goto _LL332;_tmp564=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp554)->f1;
_tmp565=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp554)->f2;_LL331: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp57E=_cycalloc(
sizeof(*_tmp57E));_tmp57E[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp57F;_tmp57F.tag=
34;_tmp57F.f1=_tmp565->name;_tmp57F.f2=(void*)_tmp564;_tmp57F.f3=(struct Cyc_Absyn_Enumfield*)
_tmp565;_tmp57F;});_tmp57E;}),0),fail_stmt);goto _LL31D;_LL332: if(_tmp554 <= (void*)
2)goto _LL334;if(*((int*)_tmp554)!= 6)goto _LL334;_tmp566=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp554)->f1;_tmp567=((struct Cyc_Absyn_Tunion_p_struct*)_tmp554)->f2;_tmp568=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp554)->f3;if(_tmp568 != 0)goto _LL334;if(!(!
_tmp566->is_flat))goto _LL334;_LL333: {struct Cyc_Absyn_Exp*cmp_exp;if(_tmp566->is_xtunion)
cmp_exp=Cyc_Absyn_var_exp(_tmp567->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(
_tmp566,_tmp567->name,0);r=Cyc_Toc_cast_it(Cyc_Absyn_sint_typ,r);}s=Cyc_Toc_if_neq_stmt(
r,cmp_exp,fail_stmt);goto _LL31D;}_LL334: if(_tmp554 <= (void*)2)goto _LL336;if(*((
int*)_tmp554)!= 4)goto _LL336;_tmp569=((struct Cyc_Absyn_Pointer_p_struct*)_tmp554)->f1;
_tmp56A=*_tmp569;_tmp56B=(void*)_tmp56A.r;if(_tmp56B <= (void*)2)goto _LL336;if(*((
int*)_tmp56B)!= 6)goto _LL336;_tmp56C=((struct Cyc_Absyn_Tunion_p_struct*)_tmp56B)->f1;
_tmp56D=((struct Cyc_Absyn_Tunion_p_struct*)_tmp56B)->f2;_tmp56E=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp56B)->f3;if(!(_tmp56E != 0  && !_tmp56C->is_flat))goto _LL336;_LL335: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp56D->name,({const
char*_tmp58C="_struct";_tag_dynforward(_tmp58C,sizeof(char),_get_zero_arr_size(
_tmp58C,8));}));struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),r);struct Cyc_List_List*_tmp580=_tmp56D->typs;
for(0;_tmp56E != 0;(((_tmp56E=_tmp56E->tl,_tmp580=((struct Cyc_List_List*)
_check_null(_tmp580))->tl)),++ cnt)){struct Cyc_Absyn_Pat*_tmp581=(struct Cyc_Absyn_Pat*)
_tmp56E->hd;if((void*)_tmp581->r == (void*)0)continue;{void*_tmp582=(*((struct
_tuple4*)((struct Cyc_List_List*)_check_null(_tmp580))->hd)).f2;struct _tuple1*
_tmp583=Cyc_Toc_temp_var();void*_tmp584=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp581->topt))->v;void*_tmp585=Cyc_Toc_typ_to_c(_tmp584);struct Cyc_Absyn_Exp*
_tmp586=Cyc_Absyn_aggrarrow_exp(rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c(_tmp582)))_tmp586=Cyc_Toc_cast_it(_tmp585,_tmp586);decls=({
struct Cyc_List_List*_tmp587=_region_malloc(rgn,sizeof(*_tmp587));_tmp587->hd=({
struct _tuple13*_tmp588=_region_malloc(rgn,sizeof(*_tmp588));_tmp588->f1=_tmp583;
_tmp588->f2=_tmp585;_tmp588;});_tmp587->tl=decls;_tmp587;});{struct _tuple12
_tmp589=Cyc_Toc_xlate_pat(nv,rgn,_tmp584,Cyc_Absyn_var_exp(_tmp583,0),_tmp586,
_tmp581,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp589.f1;decls=
_tmp589.f2;{struct Cyc_Absyn_Stmt*_tmp58A=_tmp589.f3;struct Cyc_Absyn_Stmt*_tmp58B=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp583,0),_tmp586,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp58B,_tmp58A,0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(
_tmp56C->is_xtunion){struct Cyc_Absyn_Exp*e2=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(
e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(_tmp56D->name,0);test_exp=Cyc_Absyn_neq_exp(
e1,e,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);}else{struct Cyc_Absyn_Exp*e3=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,
Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*
e=Cyc_Toc_tunion_tag(_tmp56C,_tmp56D->name,1);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(e1,e,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);if(
tag_fail_stmt != 0){int max_tag=Cyc_Toc_num_void_tags(_tmp56C);if(max_tag != 0){
struct Cyc_Absyn_Exp*max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)max_tag,0);
struct Cyc_Absyn_Exp*e5=Cyc_Absyn_lte_exp(r,Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
max_tag_exp),0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(e5,*tag_fail_stmt,
Cyc_Toc_skip_stmt_dl(),0),s,0);}}}goto _LL31D;}}_LL336: if(_tmp554 <= (void*)2)goto
_LL338;if(*((int*)_tmp554)!= 6)goto _LL338;_tmp56F=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp554)->f1;_tmp570=((struct Cyc_Absyn_Tunion_p_struct*)_tmp554)->f2;_tmp571=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp554)->f3;if(!_tmp56F->is_flat)goto _LL338;
_LL337: {struct _tuple1 _tmp58E;struct _dynforward_ptr*_tmp58F;struct _tuple1*
_tmp58D=_tmp570->name;_tmp58E=*_tmp58D;_tmp58F=_tmp58E.f2;r=Cyc_Absyn_aggrmember_exp(
r,_tmp58F,0);path=Cyc_Absyn_aggrmember_exp(path,_tmp58F,0);s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(Cyc_Absyn_aggrmember_exp(path,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(
_tmp56F,_tmp570->name,1),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0);{int cnt=1;for(0;
_tmp571 != 0;(_tmp571=_tmp571->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp590=(struct Cyc_Absyn_Pat*)
_tmp571->hd;if((void*)_tmp590->r == (void*)0)continue;{struct _tuple1*_tmp591=Cyc_Toc_temp_var();
void*_tmp592=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp590->topt))->v;decls=({
struct Cyc_List_List*_tmp593=_region_malloc(rgn,sizeof(*_tmp593));_tmp593->hd=({
struct _tuple13*_tmp594=_region_malloc(rgn,sizeof(*_tmp594));_tmp594->f1=_tmp591;
_tmp594->f2=Cyc_Toc_typ_to_c(_tmp592);_tmp594;});_tmp593->tl=decls;_tmp593;});{
struct _tuple12 _tmp595=Cyc_Toc_xlate_pat(nv,rgn,_tmp592,Cyc_Absyn_var_exp(_tmp591,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp590,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp595.f1;decls=_tmp595.f2;{struct Cyc_Absyn_Stmt*
_tmp596=_tmp595.f3;struct Cyc_Absyn_Stmt*_tmp597=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp591,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp597,_tmp596,0),0);}}}}goto _LL31D;}}_LL338: if(_tmp554 <= (
void*)2)goto _LL33A;if(*((int*)_tmp554)!= 6)goto _LL33A;_tmp572=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp554)->f3;_LL339: _tmp573=_tmp572;goto _LL33B;_LL33A: if(_tmp554 <= (void*)2)goto
_LL33C;if(*((int*)_tmp554)!= 3)goto _LL33C;_tmp573=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp554)->f1;_LL33B: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;_tmp573 != 0;(
_tmp573=_tmp573->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp598=(struct Cyc_Absyn_Pat*)
_tmp573->hd;if((void*)_tmp598->r == (void*)0)continue;{struct _tuple1*_tmp599=Cyc_Toc_temp_var();
void*_tmp59A=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp598->topt))->v;decls=({
struct Cyc_List_List*_tmp59B=_region_malloc(rgn,sizeof(*_tmp59B));_tmp59B->hd=({
struct _tuple13*_tmp59C=_region_malloc(rgn,sizeof(*_tmp59C));_tmp59C->f1=_tmp599;
_tmp59C->f2=Cyc_Toc_typ_to_c(_tmp59A);_tmp59C;});_tmp59B->tl=decls;_tmp59B;});{
struct _tuple12 _tmp59D=Cyc_Toc_xlate_pat(nv,rgn,_tmp59A,Cyc_Absyn_var_exp(_tmp599,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp598,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp59D.f1;decls=_tmp59D.f2;{struct Cyc_Absyn_Stmt*
_tmp59E=_tmp59D.f3;struct Cyc_Absyn_Stmt*_tmp59F=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp599,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp59F,_tmp59E,0),0);}}}}goto _LL31D;}_LL33C: if(_tmp554 <= (
void*)2)goto _LL33E;if(*((int*)_tmp554)!= 5)goto _LL33E;_tmp574=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp554)->f1;_tmp575=(void*)_tmp574.aggr_info;_tmp576=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp554)->f3;_LL33D: {struct Cyc_Absyn_Aggrdecl*_tmp5A0=Cyc_Absyn_get_known_aggrdecl(
_tmp575);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp576 != 0;_tmp576=_tmp576->tl){struct
_tuple14*_tmp5A1=(struct _tuple14*)_tmp576->hd;struct Cyc_Absyn_Pat*_tmp5A2=(*
_tmp5A1).f2;if((void*)_tmp5A2->r == (void*)0)continue;{struct _dynforward_ptr*f;{
void*_tmp5A3=(void*)((struct Cyc_List_List*)_check_null((*_tmp5A1).f1))->hd;
struct _dynforward_ptr*_tmp5A4;_LL347: if(*((int*)_tmp5A3)!= 1)goto _LL349;_tmp5A4=((
struct Cyc_Absyn_FieldName_struct*)_tmp5A3)->f1;_LL348: f=_tmp5A4;goto _LL346;
_LL349:;_LL34A:(int)_throw((void*)Cyc_Toc_Match_error);_LL346:;}{struct _tuple1*
_tmp5A5=Cyc_Toc_temp_var();void*_tmp5A6=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp5A2->topt))->v;void*_tmp5A7=Cyc_Toc_typ_to_c(_tmp5A6);decls=({struct Cyc_List_List*
_tmp5A8=_region_malloc(rgn,sizeof(*_tmp5A8));_tmp5A8->hd=({struct _tuple13*
_tmp5A9=_region_malloc(rgn,sizeof(*_tmp5A9));_tmp5A9->f1=_tmp5A5;_tmp5A9->f2=
_tmp5A7;_tmp5A9;});_tmp5A8->tl=decls;_tmp5A8;});{struct _tuple12 _tmp5AA=Cyc_Toc_xlate_pat(
nv,rgn,_tmp5A6,Cyc_Absyn_var_exp(_tmp5A5,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp5A2,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp5AA.f1;decls=
_tmp5AA.f2;{struct Cyc_Absyn_Exp*_tmp5AB=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star((
void*)((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5A0->impl))->fields,f)))->type))_tmp5AB=
Cyc_Toc_cast_it(_tmp5A7,_tmp5AB);{struct Cyc_Absyn_Stmt*_tmp5AC=_tmp5AA.f3;struct
Cyc_Absyn_Stmt*_tmp5AD=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp5A5,0),
_tmp5AB,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp5AD,_tmp5AC,0),0);}}}}}}
goto _LL31D;}_LL33E: if(_tmp554 <= (void*)2)goto _LL340;if(*((int*)_tmp554)!= 4)goto
_LL340;_tmp577=((struct Cyc_Absyn_Pointer_p_struct*)_tmp554)->f1;_LL33F: {struct
_tuple1*_tmp5AE=Cyc_Toc_temp_var();void*_tmp5AF=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp577->topt))->v;decls=({struct Cyc_List_List*_tmp5B0=
_region_malloc(rgn,sizeof(*_tmp5B0));_tmp5B0->hd=({struct _tuple13*_tmp5B1=
_region_malloc(rgn,sizeof(*_tmp5B1));_tmp5B1->f1=_tmp5AE;_tmp5B1->f2=Cyc_Toc_typ_to_c(
_tmp5AF);_tmp5B1;});_tmp5B0->tl=decls;_tmp5B0;});{struct _tuple12 _tmp5B2=Cyc_Toc_xlate_pat(
nv,rgn,_tmp5AF,Cyc_Absyn_var_exp(_tmp5AE,0),Cyc_Absyn_deref_exp(path,0),_tmp577,(
struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp5B2.f1;decls=_tmp5B2.f2;{
struct Cyc_Absyn_Stmt*_tmp5B3=_tmp5B2.f3;struct Cyc_Absyn_Stmt*_tmp5B4=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp5AE,0),Cyc_Absyn_deref_exp(r,0),0),
_tmp5B3,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp5B4,0);else{s=_tmp5B4;}goto _LL31D;}}}_LL340: if(_tmp554 <= (void*)2)goto
_LL342;if(*((int*)_tmp554)!= 12)goto _LL342;_LL341:({void*_tmp5B5[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp5B6="unknownid pat";_tag_dynforward(_tmp5B6,sizeof(char),_get_zero_arr_size(
_tmp5B6,14));}),_tag_dynforward(_tmp5B5,sizeof(void*),0));});_LL342: if(_tmp554 <= (
void*)2)goto _LL344;if(*((int*)_tmp554)!= 13)goto _LL344;_LL343:({void*_tmp5B7[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp5B8="unknowncall pat";_tag_dynforward(_tmp5B8,sizeof(char),
_get_zero_arr_size(_tmp5B8,16));}),_tag_dynforward(_tmp5B7,sizeof(void*),0));});
_LL344: if(_tmp554 <= (void*)2)goto _LL31D;if(*((int*)_tmp554)!= 14)goto _LL31D;
_LL345:({void*_tmp5B9[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5BA="exp pat";
_tag_dynforward(_tmp5BA,sizeof(char),_get_zero_arr_size(_tmp5BA,8));}),
_tag_dynforward(_tmp5B9,sizeof(void*),0));});_LL31D:;}return({struct _tuple12
_tmp5BB;_tmp5BB.f1=nv;_tmp5BB.f2=decls;_tmp5BB.f3=s;_tmp5BB;});}struct _tuple15{
struct _dynforward_ptr*f1;struct _dynforward_ptr*f2;struct Cyc_Absyn_Switch_clause*
f3;};static struct _tuple15*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){return({struct _tuple15*_tmp5BC=_region_malloc(r,sizeof(*_tmp5BC));_tmp5BC->f1=
Cyc_Toc_fresh_label();_tmp5BC->f2=Cyc_Toc_fresh_label();_tmp5BC->f3=sc;_tmp5BC;});}
static int Cyc_Toc_is_mixed_tunion(void*t){{void*_tmp5BD=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_TunionInfo _tmp5BE;void*_tmp5BF;struct Cyc_Absyn_Tuniondecl**
_tmp5C0;struct Cyc_Absyn_Tuniondecl*_tmp5C1;_LL34C: if(_tmp5BD <= (void*)4)goto
_LL34E;if(*((int*)_tmp5BD)!= 2)goto _LL34E;_tmp5BE=((struct Cyc_Absyn_TunionType_struct*)
_tmp5BD)->f1;_tmp5BF=(void*)_tmp5BE.tunion_info;if(*((int*)_tmp5BF)!= 1)goto
_LL34E;_tmp5C0=((struct Cyc_Absyn_KnownTunion_struct*)_tmp5BF)->f1;_tmp5C1=*
_tmp5C0;if(!(!_tmp5C1->is_flat))goto _LL34E;_LL34D: {int seen_novalue=0;int
seen_value=0;{struct Cyc_List_List*_tmp5C2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp5C1->fields))->v;for(0;(unsigned int)_tmp5C2;_tmp5C2=_tmp5C2->tl){
if(((struct Cyc_Absyn_Tunionfield*)_tmp5C2->hd)->typs == 0)seen_value=1;else{
seen_novalue=1;}if(seen_value  && seen_novalue)return 1;}}goto _LL34B;}_LL34E:;
_LL34F: goto _LL34B;_LL34B:;}return 0;}static int Cyc_Toc_no_tag_test(struct Cyc_Absyn_Pat*
p){void*_tmp5C3=(void*)p->r;struct Cyc_Absyn_Pat*_tmp5C4;struct Cyc_Absyn_Pat*
_tmp5C5;struct Cyc_Absyn_Pat _tmp5C6;void*_tmp5C7;struct Cyc_Absyn_Tuniondecl*
_tmp5C8;struct Cyc_Absyn_Tunionfield*_tmp5C9;struct Cyc_List_List*_tmp5CA;_LL351:
if(_tmp5C3 <= (void*)2)goto _LL355;if(*((int*)_tmp5C3)!= 0)goto _LL353;_tmp5C4=((
struct Cyc_Absyn_Var_p_struct*)_tmp5C3)->f2;_LL352: return Cyc_Toc_no_tag_test(
_tmp5C4);_LL353: if(*((int*)_tmp5C3)!= 4)goto _LL355;_tmp5C5=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp5C3)->f1;_tmp5C6=*_tmp5C5;_tmp5C7=(void*)_tmp5C6.r;if(_tmp5C7 <= (void*)2)
goto _LL355;if(*((int*)_tmp5C7)!= 6)goto _LL355;_tmp5C8=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp5C7)->f1;_tmp5C9=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5C7)->f2;_tmp5CA=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp5C7)->f3;if(!(_tmp5CA != 0  && !_tmp5C8->is_flat))
goto _LL355;_LL354: return 0;_LL355:;_LL356: return 1;_LL350:;}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*_tmp5CB=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;int leave_as_switch;{void*_tmp5CC=Cyc_Tcutil_compress(
_tmp5CB);_LL358: if(_tmp5CC <= (void*)4)goto _LL35C;if(*((int*)_tmp5CC)!= 5)goto
_LL35A;_LL359: goto _LL35B;_LL35A: if(*((int*)_tmp5CC)!= 12)goto _LL35C;_LL35B:
leave_as_switch=1;goto _LL357;_LL35C:;_LL35D: leave_as_switch=0;goto _LL357;_LL357:;}{
struct Cyc_List_List*_tmp5CD=scs;for(0;_tmp5CD != 0;_tmp5CD=_tmp5CD->tl){if((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)
_tmp5CD->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp5CD->hd)->where_clause
!= 0){leave_as_switch=0;break;}}}if(leave_as_switch){struct _dynforward_ptr*
next_l=Cyc_Toc_fresh_label();{struct Cyc_List_List*_tmp5CE=scs;for(0;_tmp5CE != 0;
_tmp5CE=_tmp5CE->tl){struct Cyc_Absyn_Stmt*_tmp5CF=((struct Cyc_Absyn_Switch_clause*)
_tmp5CE->hd)->body;((struct Cyc_Absyn_Switch_clause*)_tmp5CE->hd)->body=Cyc_Absyn_label_stmt(
next_l,_tmp5CF,0);next_l=Cyc_Toc_fresh_label();Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
nv,next_l),_tmp5CF);}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);struct
_dynforward_ptr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle _tmp5D0=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp5D0;_push_region(rgn);{struct Cyc_List_List*
lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple15*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);struct Cyc_List_List*
test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*decls=0;int is_tunion=
Cyc_Toc_is_mixed_tunion(_tmp5CB);int needs_tag_test=is_tunion;{struct Cyc_List_List*
_tmp5D1=lscs;for(0;_tmp5D1 != 0;_tmp5D1=_tmp5D1->tl){struct Cyc_Absyn_Switch_clause*
sc=(*((struct _tuple15*)_tmp5D1->hd)).f3;struct _dynforward_ptr*fail_lab=_tmp5D1->tl
== 0?end_l:(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(_tmp5D1->tl))->hd)).f1;
struct Cyc_Absyn_Stmt**tag_fail_stmt=0;if(needs_tag_test  && !Cyc_Toc_no_tag_test(
sc->pattern)){{struct Cyc_List_List*_tmp5D2=_tmp5D1->tl;for(0;(unsigned int)
_tmp5D2;_tmp5D2=_tmp5D2->tl){if(Cyc_Toc_no_tag_test(((*((struct _tuple15*)_tmp5D2->hd)).f3)->pattern)){
tag_fail_stmt=({struct Cyc_Absyn_Stmt**_tmp5D3=_region_malloc(rgn,sizeof(*_tmp5D3));
_tmp5D3[0]=Cyc_Absyn_goto_stmt((*((struct _tuple15*)_tmp5D2->hd)).f1,0);_tmp5D3;});
needs_tag_test=0;break;}}}if(tag_fail_stmt == 0)tag_fail_stmt=({struct Cyc_Absyn_Stmt**
_tmp5D4=_region_malloc(rgn,sizeof(*_tmp5D4));_tmp5D4[0]=Cyc_Absyn_goto_stmt(
fail_lab,0);_tmp5D4;});}{struct Cyc_Toc_Env*_tmp5D6;struct Cyc_List_List*_tmp5D7;
struct Cyc_Absyn_Stmt*_tmp5D8;struct _tuple12 _tmp5D5=Cyc_Toc_xlate_pat(nv,rgn,
_tmp5CB,r,path,sc->pattern,tag_fail_stmt,Cyc_Absyn_goto_stmt(fail_lab,0),decls);
_tmp5D6=_tmp5D5.f1;_tmp5D7=_tmp5D5.f2;_tmp5D8=_tmp5D5.f3;if(is_tunion  && Cyc_Toc_no_tag_test(
sc->pattern))needs_tag_test=1;if(sc->where_clause != 0){struct Cyc_Absyn_Exp*
_tmp5D9=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);Cyc_Toc_exp_to_c(
_tmp5D6,_tmp5D9);_tmp5D8=Cyc_Absyn_seq_stmt(_tmp5D8,Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp((void*)11,_tmp5D9,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),
0),0);}decls=_tmp5D7;nvs=({struct Cyc_List_List*_tmp5DA=_region_malloc(rgn,
sizeof(*_tmp5DA));_tmp5DA->hd=_tmp5D6;_tmp5DA->tl=nvs;_tmp5DA;});test_stmts=({
struct Cyc_List_List*_tmp5DB=_region_malloc(rgn,sizeof(*_tmp5DB));_tmp5DB->hd=
_tmp5D8;_tmp5DB->tl=test_stmts;_tmp5DB;});}}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple15 _tmp5DD;struct _dynforward_ptr*_tmp5DE;struct _dynforward_ptr*
_tmp5DF;struct Cyc_Absyn_Switch_clause*_tmp5E0;struct _tuple15*_tmp5DC=(struct
_tuple15*)lscs->hd;_tmp5DD=*_tmp5DC;_tmp5DE=_tmp5DD.f1;_tmp5DF=_tmp5DD.f2;
_tmp5E0=_tmp5DD.f3;{struct Cyc_Toc_Env*_tmp5E1=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)
_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp5E0->body;if(lscs->tl != 0){
struct _tuple15 _tmp5E3;struct _dynforward_ptr*_tmp5E4;struct Cyc_Absyn_Switch_clause*
_tmp5E5;struct _tuple15*_tmp5E2=(struct _tuple15*)((struct Cyc_List_List*)
_check_null(lscs->tl))->hd;_tmp5E3=*_tmp5E2;_tmp5E4=_tmp5E3.f2;_tmp5E5=_tmp5E3.f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp5E1,end_l,_tmp5E4,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5E5->pat_vars))->v,(
struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp5E1,end_l),s);}s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_label_stmt(_tmp5DE,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)
_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(_tmp5DF,s,0),0);stmts=({
struct Cyc_List_List*_tmp5E6=_region_malloc(rgn,sizeof(*_tmp5E6));_tmp5E6->hd=s;
_tmp5E6->tl=stmts;_tmp5E6;});}}{struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(
end_l,Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct
_tuple13 _tmp5E8;struct _tuple1*_tmp5E9;void*_tmp5EA;struct _tuple13*_tmp5E7=(
struct _tuple13*)decls->hd;_tmp5E8=*_tmp5E7;_tmp5E9=_tmp5E8.f1;_tmp5EA=_tmp5E8.f2;
res=Cyc_Absyn_declare_stmt(_tmp5E9,_tmp5EA,0,res,0);}(void*)(whole_s->r=(void*)((
void*)(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),(struct Cyc_Absyn_Exp*)e,res,0))->r));}}};_pop_region(
rgn);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*
init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp5EB=_cycalloc(sizeof(*_tmp5EB));
_tmp5EB->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0);_tmp5EB->tl=0;_tmp5EB;}),
0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){if(n > 0)(void*)(s->r=(
void*)Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt((void*)
s->r,0)));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s){while(1){void*_tmp5EC=(void*)s->r;struct Cyc_Absyn_Exp*_tmp5ED;struct Cyc_Absyn_Stmt*
_tmp5EE;struct Cyc_Absyn_Stmt*_tmp5EF;struct Cyc_Absyn_Exp*_tmp5F0;struct Cyc_Absyn_Exp*
_tmp5F1;struct Cyc_Absyn_Stmt*_tmp5F2;struct Cyc_Absyn_Stmt*_tmp5F3;struct _tuple3
_tmp5F4;struct Cyc_Absyn_Exp*_tmp5F5;struct Cyc_Absyn_Stmt*_tmp5F6;struct Cyc_Absyn_Stmt*
_tmp5F7;struct Cyc_Absyn_Stmt*_tmp5F8;struct Cyc_Absyn_Stmt*_tmp5F9;struct Cyc_Absyn_Exp*
_tmp5FA;struct _tuple3 _tmp5FB;struct Cyc_Absyn_Exp*_tmp5FC;struct _tuple3 _tmp5FD;
struct Cyc_Absyn_Exp*_tmp5FE;struct Cyc_Absyn_Stmt*_tmp5FF;struct Cyc_Absyn_Exp*
_tmp600;struct Cyc_List_List*_tmp601;struct Cyc_List_List*_tmp602;struct Cyc_Absyn_Switch_clause**
_tmp603;struct Cyc_Absyn_Decl*_tmp604;struct Cyc_Absyn_Stmt*_tmp605;struct
_dynforward_ptr*_tmp606;struct Cyc_Absyn_Stmt*_tmp607;struct Cyc_Absyn_Stmt*
_tmp608;struct _tuple3 _tmp609;struct Cyc_Absyn_Exp*_tmp60A;struct Cyc_Absyn_Stmt*
_tmp60B;struct Cyc_List_List*_tmp60C;struct Cyc_Absyn_Tvar*_tmp60D;struct Cyc_Absyn_Vardecl*
_tmp60E;int _tmp60F;struct Cyc_Absyn_Stmt*_tmp610;struct Cyc_Absyn_Exp*_tmp611;
_LL35F: if((int)_tmp5EC != 0)goto _LL361;_LL360: return;_LL361: if(_tmp5EC <= (void*)1)
goto _LL363;if(*((int*)_tmp5EC)!= 0)goto _LL363;_tmp5ED=((struct Cyc_Absyn_Exp_s_struct*)
_tmp5EC)->f1;_LL362: Cyc_Toc_exp_to_c(nv,_tmp5ED);return;_LL363: if(_tmp5EC <= (
void*)1)goto _LL365;if(*((int*)_tmp5EC)!= 1)goto _LL365;_tmp5EE=((struct Cyc_Absyn_Seq_s_struct*)
_tmp5EC)->f1;_tmp5EF=((struct Cyc_Absyn_Seq_s_struct*)_tmp5EC)->f2;_LL364: Cyc_Toc_stmt_to_c(
nv,_tmp5EE);s=_tmp5EF;continue;_LL365: if(_tmp5EC <= (void*)1)goto _LL367;if(*((int*)
_tmp5EC)!= 2)goto _LL367;_tmp5F0=((struct Cyc_Absyn_Return_s_struct*)_tmp5EC)->f1;
_LL366: {struct Cyc_Core_Opt*topt=0;if(_tmp5F0 != 0){topt=({struct Cyc_Core_Opt*
_tmp612=_cycalloc(sizeof(*_tmp612));_tmp612->v=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp5F0->topt))->v);_tmp612;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp5F0);}if(s->try_depth > 0){if(topt != 0){struct
_tuple1*_tmp613=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp614=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp613,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp613,(void*)topt->v,_tmp5F0,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp614,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL367: if(_tmp5EC <= (void*)1)goto _LL369;if(*((int*)_tmp5EC)!= 3)goto
_LL369;_tmp5F1=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp5EC)->f1;_tmp5F2=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp5EC)->f2;_tmp5F3=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp5EC)->f3;_LL368: Cyc_Toc_exp_to_c(nv,_tmp5F1);Cyc_Toc_stmt_to_c(nv,_tmp5F2);s=
_tmp5F3;continue;_LL369: if(_tmp5EC <= (void*)1)goto _LL36B;if(*((int*)_tmp5EC)!= 4)
goto _LL36B;_tmp5F4=((struct Cyc_Absyn_While_s_struct*)_tmp5EC)->f1;_tmp5F5=
_tmp5F4.f1;_tmp5F6=((struct Cyc_Absyn_While_s_struct*)_tmp5EC)->f2;_LL36A: Cyc_Toc_exp_to_c(
nv,_tmp5F5);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),_tmp5F6);return;_LL36B: if(
_tmp5EC <= (void*)1)goto _LL36D;if(*((int*)_tmp5EC)!= 5)goto _LL36D;_tmp5F7=((
struct Cyc_Absyn_Break_s_struct*)_tmp5EC)->f1;_LL36C: if(nv->break_lab != 0)(void*)(
s->r=(void*)Cyc_Toc_goto_stmt_r((struct _dynforward_ptr*)((struct Cyc_Core_Opt*)
_check_null(nv->break_lab))->v,0));{int dest_depth=_tmp5F7 == 0?0: _tmp5F7->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;}_LL36D: if(_tmp5EC <= (
void*)1)goto _LL36F;if(*((int*)_tmp5EC)!= 6)goto _LL36F;_tmp5F8=((struct Cyc_Absyn_Continue_s_struct*)
_tmp5EC)->f1;_LL36E: if(nv->continue_lab != 0)(void*)(s->r=(void*)Cyc_Toc_goto_stmt_r((
struct _dynforward_ptr*)((struct Cyc_Core_Opt*)_check_null(nv->continue_lab))->v,0));
_tmp5F9=_tmp5F8;goto _LL370;_LL36F: if(_tmp5EC <= (void*)1)goto _LL371;if(*((int*)
_tmp5EC)!= 7)goto _LL371;_tmp5F9=((struct Cyc_Absyn_Goto_s_struct*)_tmp5EC)->f2;
_LL370: Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(
_tmp5F9))->try_depth,s);return;_LL371: if(_tmp5EC <= (void*)1)goto _LL373;if(*((int*)
_tmp5EC)!= 8)goto _LL373;_tmp5FA=((struct Cyc_Absyn_For_s_struct*)_tmp5EC)->f1;
_tmp5FB=((struct Cyc_Absyn_For_s_struct*)_tmp5EC)->f2;_tmp5FC=_tmp5FB.f1;_tmp5FD=((
struct Cyc_Absyn_For_s_struct*)_tmp5EC)->f3;_tmp5FE=_tmp5FD.f1;_tmp5FF=((struct
Cyc_Absyn_For_s_struct*)_tmp5EC)->f4;_LL372: Cyc_Toc_exp_to_c(nv,_tmp5FA);Cyc_Toc_exp_to_c(
nv,_tmp5FC);Cyc_Toc_exp_to_c(nv,_tmp5FE);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),
_tmp5FF);return;_LL373: if(_tmp5EC <= (void*)1)goto _LL375;if(*((int*)_tmp5EC)!= 9)
goto _LL375;_tmp600=((struct Cyc_Absyn_Switch_s_struct*)_tmp5EC)->f1;_tmp601=((
struct Cyc_Absyn_Switch_s_struct*)_tmp5EC)->f2;_LL374: Cyc_Toc_xlate_switch(nv,s,
_tmp600,_tmp601);return;_LL375: if(_tmp5EC <= (void*)1)goto _LL377;if(*((int*)
_tmp5EC)!= 10)goto _LL377;_tmp602=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp5EC)->f1;
_tmp603=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp5EC)->f2;_LL376: if(nv->fallthru_info
== 0)({void*_tmp615[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp616="fallthru in unexpected place";
_tag_dynforward(_tmp616,sizeof(char),_get_zero_arr_size(_tmp616,29));}),
_tag_dynforward(_tmp615,sizeof(void*),0));});{struct _tuple8 _tmp618;struct
_dynforward_ptr*_tmp619;struct Cyc_List_List*_tmp61A;struct Cyc_Dict_Dict*_tmp61B;
struct _tuple8*_tmp617=(struct _tuple8*)((struct Cyc_Core_Opt*)_check_null(nv->fallthru_info))->v;
_tmp618=*_tmp617;_tmp619=_tmp618.f1;_tmp61A=_tmp618.f2;_tmp61B=_tmp618.f3;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp619,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp603)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp61C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp61A);struct Cyc_List_List*_tmp61D=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp602);for(0;_tmp61C != 0;(_tmp61C=_tmp61C->tl,
_tmp61D=_tmp61D->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp61D))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup)(
_tmp61B,(struct _tuple1*)_tmp61C->hd),(struct Cyc_Absyn_Exp*)_tmp61D->hd,0),s2,0);}(
void*)(s->r=(void*)((void*)s2->r));return;}}}_LL377: if(_tmp5EC <= (void*)1)goto
_LL379;if(*((int*)_tmp5EC)!= 11)goto _LL379;_tmp604=((struct Cyc_Absyn_Decl_s_struct*)
_tmp5EC)->f1;_tmp605=((struct Cyc_Absyn_Decl_s_struct*)_tmp5EC)->f2;_LL378:{void*
_tmp61E=(void*)_tmp604->r;struct Cyc_Absyn_Vardecl*_tmp61F;struct Cyc_Absyn_Pat*
_tmp620;struct Cyc_Absyn_Exp*_tmp621;struct Cyc_List_List*_tmp622;struct Cyc_Absyn_Fndecl*
_tmp623;_LL384: if(_tmp61E <= (void*)2)goto _LL38C;if(*((int*)_tmp61E)!= 0)goto
_LL386;_tmp61F=((struct Cyc_Absyn_Var_d_struct*)_tmp61E)->f1;_LL385: {struct Cyc_Toc_Env*
_tmp624=Cyc_Toc_add_varmap(nv,_tmp61F->name,Cyc_Absyn_varb_exp(_tmp61F->name,(
void*)({struct Cyc_Absyn_Local_b_struct*_tmp625=_cycalloc(sizeof(*_tmp625));
_tmp625[0]=({struct Cyc_Absyn_Local_b_struct _tmp626;_tmp626.tag=3;_tmp626.f1=
_tmp61F;_tmp626;});_tmp625;}),0));Cyc_Toc_local_decl_to_c(_tmp624,_tmp624,
_tmp61F,_tmp605);goto _LL383;}_LL386: if(*((int*)_tmp61E)!= 2)goto _LL388;_tmp620=((
struct Cyc_Absyn_Let_d_struct*)_tmp61E)->f1;_tmp621=((struct Cyc_Absyn_Let_d_struct*)
_tmp61E)->f3;_LL387:{void*_tmp627=(void*)_tmp620->r;struct Cyc_Absyn_Vardecl*
_tmp628;struct Cyc_Absyn_Pat*_tmp629;struct Cyc_Absyn_Pat _tmp62A;void*_tmp62B;
_LL38F: if(_tmp627 <= (void*)2)goto _LL391;if(*((int*)_tmp627)!= 0)goto _LL391;
_tmp628=((struct Cyc_Absyn_Var_p_struct*)_tmp627)->f1;_tmp629=((struct Cyc_Absyn_Var_p_struct*)
_tmp627)->f2;_tmp62A=*_tmp629;_tmp62B=(void*)_tmp62A.r;if((int)_tmp62B != 0)goto
_LL391;_LL390: {struct _tuple1*old_name=_tmp628->name;struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp628->name=new_name;_tmp628->initializer=(struct Cyc_Absyn_Exp*)_tmp621;(void*)(
_tmp604->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp62C=_cycalloc(
sizeof(*_tmp62C));_tmp62C[0]=({struct Cyc_Absyn_Var_d_struct _tmp62D;_tmp62D.tag=0;
_tmp62D.f1=_tmp628;_tmp62D;});_tmp62C;})));{struct Cyc_Toc_Env*_tmp62E=Cyc_Toc_add_varmap(
nv,old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F[0]=({struct Cyc_Absyn_Local_b_struct
_tmp630;_tmp630.tag=3;_tmp630.f1=_tmp628;_tmp630;});_tmp62F;}),0));Cyc_Toc_local_decl_to_c(
_tmp62E,nv,_tmp628,_tmp605);goto _LL38E;}}_LL391:;_LL392:(void*)(s->r=(void*)((
void*)(Cyc_Toc_letdecl_to_c(nv,_tmp620,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp621->topt))->v,_tmp621,_tmp605))->r));goto _LL38E;_LL38E:;}goto _LL383;_LL388:
if(*((int*)_tmp61E)!= 3)goto _LL38A;_tmp622=((struct Cyc_Absyn_Letv_d_struct*)
_tmp61E)->f1;_LL389: {struct Cyc_List_List*_tmp631=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp622);if(_tmp631 == 0)({void*_tmp632[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp633="empty Letv_d";_tag_dynforward(_tmp633,sizeof(char),
_get_zero_arr_size(_tmp633,13));}),_tag_dynforward(_tmp632,sizeof(void*),0));});(
void*)(_tmp604->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp634=
_cycalloc(sizeof(*_tmp634));_tmp634[0]=({struct Cyc_Absyn_Var_d_struct _tmp635;
_tmp635.tag=0;_tmp635.f1=(struct Cyc_Absyn_Vardecl*)_tmp631->hd;_tmp635;});
_tmp634;})));_tmp631=_tmp631->tl;for(0;_tmp631 != 0;_tmp631=_tmp631->tl){struct
Cyc_Absyn_Decl*_tmp636=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp637=_cycalloc(sizeof(*_tmp637));_tmp637[0]=({struct Cyc_Absyn_Var_d_struct
_tmp638;_tmp638.tag=0;_tmp638.f1=(struct Cyc_Absyn_Vardecl*)_tmp631->hd;_tmp638;});
_tmp637;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(_tmp636,Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto _LL383;}_LL38A: if(*((int*)
_tmp61E)!= 1)goto _LL38C;_tmp623=((struct Cyc_Absyn_Fn_d_struct*)_tmp61E)->f1;
_LL38B: {struct _tuple1*_tmp639=_tmp623->name;nv=Cyc_Toc_add_varmap(nv,_tmp623->name,
Cyc_Absyn_var_exp(_tmp639,0));Cyc_Toc_fndecl_to_c(nv,_tmp623,0);Cyc_Toc_stmt_to_c(
nv,_tmp605);goto _LL383;}_LL38C:;_LL38D:({void*_tmp63A[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp63B="bad nested declaration within function";_tag_dynforward(_tmp63B,sizeof(
char),_get_zero_arr_size(_tmp63B,39));}),_tag_dynforward(_tmp63A,sizeof(void*),0));});
_LL383:;}return;_LL379: if(_tmp5EC <= (void*)1)goto _LL37B;if(*((int*)_tmp5EC)!= 12)
goto _LL37B;_tmp606=((struct Cyc_Absyn_Label_s_struct*)_tmp5EC)->f1;_tmp607=((
struct Cyc_Absyn_Label_s_struct*)_tmp5EC)->f2;_LL37A: s=_tmp607;continue;_LL37B:
if(_tmp5EC <= (void*)1)goto _LL37D;if(*((int*)_tmp5EC)!= 13)goto _LL37D;_tmp608=((
struct Cyc_Absyn_Do_s_struct*)_tmp5EC)->f1;_tmp609=((struct Cyc_Absyn_Do_s_struct*)
_tmp5EC)->f2;_tmp60A=_tmp609.f1;_LL37C: Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),
_tmp608);Cyc_Toc_exp_to_c(nv,_tmp60A);return;_LL37D: if(_tmp5EC <= (void*)1)goto
_LL37F;if(*((int*)_tmp5EC)!= 14)goto _LL37F;_tmp60B=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp5EC)->f1;_tmp60C=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp5EC)->f2;_LL37E: {
struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ);void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);e_exp->topt=({struct Cyc_Core_Opt*
_tmp63C=_cycalloc(sizeof(*_tmp63C));_tmp63C->v=(void*)e_typ;_tmp63C;});nv=Cyc_Toc_add_varmap(
nv,e_var,e_exp);Cyc_Toc_stmt_to_c(nv,_tmp60B);{struct Cyc_Absyn_Stmt*_tmp63D=Cyc_Absyn_seq_stmt(
_tmp60B,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
struct _tuple1*_tmp63E=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp63F=Cyc_Absyn_var_exp(
_tmp63E,0);struct Cyc_Absyn_Vardecl*_tmp640=Cyc_Absyn_new_vardecl(_tmp63E,Cyc_Absyn_exn_typ,
0);_tmp63F->topt=({struct Cyc_Core_Opt*_tmp641=_cycalloc(sizeof(*_tmp641));
_tmp641->v=(void*)Cyc_Absyn_exn_typ;_tmp641;});{struct Cyc_Absyn_Pat*_tmp642=({
struct Cyc_Absyn_Pat*_tmp652=_cycalloc(sizeof(*_tmp652));_tmp652->r=(void*)((void*)({
struct Cyc_Absyn_Var_p_struct*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654[0]=({
struct Cyc_Absyn_Var_p_struct _tmp655;_tmp655.tag=0;_tmp655.f1=_tmp640;_tmp655.f2=({
struct Cyc_Absyn_Pat*_tmp656=_cycalloc(sizeof(*_tmp656));_tmp656->r=(void*)((void*)
0);_tmp656->topt=({struct Cyc_Core_Opt*_tmp657=_cycalloc(sizeof(*_tmp657));
_tmp657->v=(void*)Cyc_Absyn_exn_typ;_tmp657;});_tmp656->loc=0;_tmp656;});_tmp655;});
_tmp654;}));_tmp652->topt=({struct Cyc_Core_Opt*_tmp653=_cycalloc(sizeof(*_tmp653));
_tmp653->v=(void*)Cyc_Absyn_exn_typ;_tmp653;});_tmp652->loc=0;_tmp652;});struct
Cyc_Absyn_Exp*_tmp643=Cyc_Absyn_throw_exp(_tmp63F,0);_tmp643->topt=({struct Cyc_Core_Opt*
_tmp644=_cycalloc(sizeof(*_tmp644));_tmp644->v=(void*)((void*)0);_tmp644;});{
struct Cyc_Absyn_Stmt*_tmp645=Cyc_Absyn_exp_stmt(_tmp643,0);struct Cyc_Absyn_Switch_clause*
_tmp646=({struct Cyc_Absyn_Switch_clause*_tmp64F=_cycalloc(sizeof(*_tmp64F));
_tmp64F->pattern=_tmp642;_tmp64F->pat_vars=({struct Cyc_Core_Opt*_tmp650=
_cycalloc(sizeof(*_tmp650));_tmp650->v=({struct Cyc_List_List*_tmp651=_cycalloc(
sizeof(*_tmp651));_tmp651->hd=_tmp640;_tmp651->tl=0;_tmp651;});_tmp650;});
_tmp64F->where_clause=0;_tmp64F->body=_tmp645;_tmp64F->loc=0;_tmp64F;});struct
Cyc_Absyn_Stmt*_tmp647=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp60C,({struct
Cyc_List_List*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E->hd=_tmp646;_tmp64E->tl=
0;_tmp64E;})),0);Cyc_Toc_stmt_to_c(nv,_tmp647);{struct Cyc_Absyn_Exp*_tmp648=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp64D=_cycalloc(sizeof(*_tmp64D));
_tmp64D->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp64D->tl=0;
_tmp64D;}),0);struct Cyc_Absyn_Stmt*_tmp649=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp64C=_cycalloc(sizeof(*_tmp64C));
_tmp64C->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp64C->tl=0;_tmp64C;}),0),0);struct
Cyc_Absyn_Exp*_tmp64A=Cyc_Absyn_int_exp((void*)0,0,0);struct Cyc_Absyn_Exp*
_tmp64B=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp649,Cyc_Absyn_declare_stmt(was_thrown_var,
was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp64A,Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp648,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp64B,0),Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)11,was_thrown_exp,0),
_tmp63D,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)Cyc_Toc_cast_it(
e_typ,Cyc_Toc__exn_thrown_e),_tmp647,0),0),0),0),0),0))->r));return;}}}}}_LL37F:
if(_tmp5EC <= (void*)1)goto _LL381;if(*((int*)_tmp5EC)!= 15)goto _LL381;_tmp60D=((
struct Cyc_Absyn_Region_s_struct*)_tmp5EC)->f1;_tmp60E=((struct Cyc_Absyn_Region_s_struct*)
_tmp5EC)->f2;_tmp60F=((struct Cyc_Absyn_Region_s_struct*)_tmp5EC)->f3;_tmp610=((
struct Cyc_Absyn_Region_s_struct*)_tmp5EC)->f4;_LL380: {void*rh_struct_typ=Cyc_Absyn_strct(
Cyc_Toc__RegionHandle_sp);void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(
rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple1*rh_var=Cyc_Toc_temp_var();struct
_tuple1*x_var=_tmp60E->name;struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,
0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
nv,x_var,x_exp),_tmp610);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({
struct Cyc_List_List*_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658->hd=Cyc_Absyn_string_exp(
Cyc_Absynpp_qvar2string(x_var),0);_tmp658->tl=0;_tmp658;}),0),Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp*_tmp659[1];_tmp659[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp659,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),Cyc_Absyn_seq_stmt(_tmp610,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__pop_region_e,({struct Cyc_Absyn_Exp*_tmp65A[1];_tmp65A[0]=x_exp;((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp65A,
sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),0),0),0),0))->r));return;}_LL381: if(
_tmp5EC <= (void*)1)goto _LL35E;if(*((int*)_tmp5EC)!= 16)goto _LL35E;_tmp611=((
struct Cyc_Absyn_ResetRegion_s_struct*)_tmp5EC)->f1;_LL382: Cyc_Toc_exp_to_c(nv,
_tmp611);(void*)(s->r=(void*)Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({
struct Cyc_List_List*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->hd=_tmp611;
_tmp65B->tl=0;_tmp65B;}),0)));return;_LL35E:;}}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s);struct _tuple16{struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;(void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((
void*)f->ret_type));{struct Cyc_List_List*_tmp65C=f->args;for(0;_tmp65C != 0;
_tmp65C=_tmp65C->tl){struct _tuple1*_tmp65D=({struct _tuple1*_tmp65E=_cycalloc(
sizeof(*_tmp65E));_tmp65E->f1=(union Cyc_Absyn_Nmspace_union)((union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp65F;(_tmp65F.Loc_n).tag=0;_tmp65F;}));_tmp65E->f2=(*((
struct _tuple16*)_tmp65C->hd)).f1;_tmp65E;});(*((struct _tuple16*)_tmp65C->hd)).f3=
Cyc_Toc_typ_to_c((*((struct _tuple16*)_tmp65C->hd)).f3);nv=Cyc_Toc_add_varmap(nv,
_tmp65D,Cyc_Absyn_var_exp(_tmp65D,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp661;struct Cyc_Absyn_Tqual
_tmp662;void*_tmp663;int _tmp664;struct Cyc_Absyn_VarargInfo _tmp660=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp661=_tmp660.name;_tmp662=_tmp660.tq;_tmp663=(
void*)_tmp660.type;_tmp664=_tmp660.inject;{void*_tmp665=Cyc_Toc_typ_to_c(Cyc_Absyn_dynforward_typ(
_tmp663,(void*)2,_tmp662,Cyc_Absyn_false_conref));struct _tuple1*_tmp666=({struct
_tuple1*_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669->f1=(union Cyc_Absyn_Nmspace_union)((
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp66A;(_tmp66A.Loc_n).tag=
0;_tmp66A;}));_tmp669->f2=(struct _dynforward_ptr*)((struct Cyc_Core_Opt*)
_check_null(_tmp661))->v;_tmp669;});f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp667=
_cycalloc(sizeof(*_tmp667));_tmp667->hd=({struct _tuple16*_tmp668=_cycalloc(
sizeof(*_tmp668));_tmp668->f1=(struct _dynforward_ptr*)_tmp661->v;_tmp668->f2=
_tmp662;_tmp668->f3=_tmp665;_tmp668;});_tmp667->tl=0;_tmp667;}));nv=Cyc_Toc_add_varmap(
nv,_tmp666,Cyc_Absyn_var_exp(_tmp666,0));f->cyc_varargs=0;}}{struct Cyc_List_List*
_tmp66B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp66B != 0;_tmp66B=_tmp66B->tl){(void*)(((struct Cyc_Absyn_Vardecl*)
_tmp66B->hd)->type=(void*)Cyc_Toc_typ_to_c((void*)((struct Cyc_Absyn_Vardecl*)
_tmp66B->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(nv),f->body);}
static void*Cyc_Toc_scope_to_c(void*s){void*_tmp66C=s;_LL394: if((int)_tmp66C != 1)
goto _LL396;_LL395: return(void*)2;_LL396: if((int)_tmp66C != 4)goto _LL398;_LL397:
return(void*)3;_LL398:;_LL399: return s;_LL393:;}struct Cyc_Core_Opt*Cyc_Toc_aggrs_so_far=
0;static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){struct _tuple1*
_tmp66D=ad->name;if(Cyc_Toc_aggrs_so_far == 0)Cyc_Toc_aggrs_so_far=({struct Cyc_Core_Opt*
_tmp66E=_cycalloc(sizeof(*_tmp66E));_tmp66E->v=((struct Cyc_Dict_Dict*(*)(int(*
cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp66E;});{
int seen_defn_before;struct Cyc_Core_Opt*_tmp66F=((struct Cyc_Core_Opt*(*)(struct
Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,_tmp66D);if(_tmp66F == 
0){seen_defn_before=0;(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)_check_null(
Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,_tmp66D,ad));}else{if(((
struct Cyc_Absyn_Aggrdecl*)_tmp66F->v)->impl == 0){(struct Cyc_Dict_Dict*)(((struct
Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,
_tmp66D,ad));seen_defn_before=0;}else{seen_defn_before=1;}}(void*)(ad->sc=(void*)((
void*)2));ad->tvs=0;if(ad->impl != 0){((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
ad->impl))->exist_vars=0;((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po=
0;if(seen_defn_before)ad->impl=0;else{struct Cyc_List_List*_tmp670=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp670 != 0;_tmp670=_tmp670->tl){((struct
Cyc_Absyn_Aggrfield*)_tmp670->hd)->tq=Cyc_Toc_mt_tq;(void*)(((struct Cyc_Absyn_Aggrfield*)
_tmp670->hd)->type=(void*)Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp670->hd)->type));}}}}}static struct Cyc_Core_Opt*Cyc_Toc_tunions_so_far=0;
static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*tud){if(Cyc_Toc_tunions_so_far
== 0)Cyc_Toc_tunions_so_far=({struct Cyc_Core_Opt*_tmp671=_cycalloc(sizeof(*
_tmp671));_tmp671->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _tuple1*,struct
_tuple1*)))Cyc_Set_empty)(Cyc_Absyn_qvar_cmp);_tmp671;});{struct _tuple1*_tmp672=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))
Cyc_Set_member)((struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,
_tmp672))return;(struct Cyc_Set_Set*)(((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v=(
void*)((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_insert)((
struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,
_tmp672));{struct Cyc_List_List*flat_structs=0;{struct Cyc_List_List*_tmp673=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;
_tmp673 != 0;_tmp673=_tmp673->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp673->hd;if(f->typs != 0  || tud->is_flat){struct Cyc_List_List*_tmp674=0;int i=1;{
struct Cyc_List_List*_tmp675=f->typs;for(0;_tmp675 != 0;(_tmp675=_tmp675->tl,i ++)){
struct _dynforward_ptr*_tmp676=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmp677=({struct Cyc_Absyn_Aggrfield*_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679->name=
_tmp676;_tmp679->tq=(*((struct _tuple4*)_tmp675->hd)).f1;_tmp679->type=(void*)Cyc_Toc_typ_to_c_array((*((
struct _tuple4*)_tmp675->hd)).f2);_tmp679->width=0;_tmp679->attributes=0;_tmp679;});
_tmp674=({struct Cyc_List_List*_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678->hd=
_tmp677;_tmp678->tl=_tmp674;_tmp678;});}}_tmp674=({struct Cyc_List_List*_tmp67A=
_cycalloc(sizeof(*_tmp67A));_tmp67A->hd=({struct Cyc_Absyn_Aggrfield*_tmp67B=
_cycalloc(sizeof(*_tmp67B));_tmp67B->name=Cyc_Toc_tag_sp;_tmp67B->tq=Cyc_Toc_mt_tq;
_tmp67B->type=(void*)Cyc_Absyn_sint_typ;_tmp67B->width=0;_tmp67B->attributes=0;
_tmp67B;});_tmp67A->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp674);_tmp67A;});{struct Cyc_Absyn_Aggrdecl*_tmp67C=({struct Cyc_Absyn_Aggrdecl*
_tmp689=_cycalloc(sizeof(*_tmp689));_tmp689->kind=(void*)((void*)0);_tmp689->sc=(
void*)((void*)2);_tmp689->name=Cyc_Toc_collapse_qvar_tag(f->name,({const char*
_tmp68B="_struct";_tag_dynforward(_tmp68B,sizeof(char),_get_zero_arr_size(
_tmp68B,8));}));_tmp689->tvs=0;_tmp689->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp68A=_cycalloc(sizeof(*_tmp68A));_tmp68A->exist_vars=0;_tmp68A->rgn_po=0;
_tmp68A->fields=_tmp674;_tmp68A;});_tmp689->attributes=0;_tmp689;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp67D=_cycalloc(sizeof(*_tmp67D));_tmp67D->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp67E=_cycalloc(sizeof(*_tmp67E));
_tmp67E[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp67F;_tmp67F.tag=4;_tmp67F.f1=
_tmp67C;_tmp67F;});_tmp67E;}),0);_tmp67D->tl=Cyc_Toc_result_decls;_tmp67D;});if(
tud->is_flat){struct Cyc_Absyn_Aggrfield*_tmp680=({struct Cyc_Absyn_Aggrfield*
_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682->name=(*f->name).f2;_tmp682->tq=Cyc_Toc_mt_tq;
_tmp682->type=(void*)((void*)({struct Cyc_Absyn_AggrType_struct*_tmp683=_cycalloc(
sizeof(*_tmp683));_tmp683[0]=({struct Cyc_Absyn_AggrType_struct _tmp684;_tmp684.tag=
10;_tmp684.f1=({struct Cyc_Absyn_AggrInfo _tmp685;_tmp685.aggr_info=(void*)((void*)({
struct Cyc_Absyn_KnownAggr_struct*_tmp686=_cycalloc(sizeof(*_tmp686));_tmp686[0]=({
struct Cyc_Absyn_KnownAggr_struct _tmp687;_tmp687.tag=1;_tmp687.f1=({struct Cyc_Absyn_Aggrdecl**
_tmp688=_cycalloc(sizeof(*_tmp688));_tmp688[0]=_tmp67C;_tmp688;});_tmp687;});
_tmp686;}));_tmp685.targs=0;_tmp685;});_tmp684;});_tmp683;}));_tmp682->width=0;
_tmp682->attributes=0;_tmp682;});flat_structs=({struct Cyc_List_List*_tmp681=
_cycalloc(sizeof(*_tmp681));_tmp681->hd=_tmp680;_tmp681->tl=flat_structs;_tmp681;});}}}}}
if(tud->is_flat){flat_structs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(flat_structs);{struct Cyc_Absyn_Aggrdecl*_tmp68C=({struct Cyc_Absyn_Aggrdecl*
_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690->kind=(void*)((void*)1);_tmp690->sc=(
void*)((void*)2);_tmp690->name=Cyc_Toc_collapse_qvar_tag(tud->name,({const char*
_tmp692="_union";_tag_dynforward(_tmp692,sizeof(char),_get_zero_arr_size(_tmp692,
7));}));_tmp690->tvs=0;_tmp690->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp691=
_cycalloc(sizeof(*_tmp691));_tmp691->exist_vars=0;_tmp691->rgn_po=0;_tmp691->fields=
flat_structs;_tmp691;});_tmp690->attributes=0;_tmp690;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp68D=_cycalloc(sizeof(*_tmp68D));_tmp68D->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp68E=_cycalloc(sizeof(*_tmp68E));
_tmp68E[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp68F;_tmp68F.tag=4;_tmp68F.f1=
_tmp68C;_tmp68F;});_tmp68E;}),0);_tmp68D->tl=Cyc_Toc_result_decls;_tmp68D;});}}}}}
static struct Cyc_Core_Opt*Cyc_Toc_xtunion_fields_so_far=0;static void Cyc_Toc_xtuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl*xd){if(Cyc_Toc_xtunion_fields_so_far == 0)Cyc_Toc_xtunion_fields_so_far=({
struct Cyc_Core_Opt*_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693->v=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp693;});if(xd->fields == 0)return;{struct _tuple1*_tmp694=xd->name;struct Cyc_List_List*
_tmp695=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;
for(0;_tmp695 != 0;_tmp695=_tmp695->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp695->hd;struct _dynforward_ptr*fn=(*f->name).f2;struct Cyc_Absyn_Exp*_tmp696=
Cyc_Absyn_uint_exp(_get_dynforward_size(*fn,sizeof(char))+ 4,0);void*_tmp697=Cyc_Absyn_array_typ(
Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)_tmp696,Cyc_Absyn_false_conref,
0);struct Cyc_Core_Opt*_tmp698=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,
struct _tuple1*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_xtunion_fields_so_far))->v,f->name);struct Cyc_Core_Opt
_tmp699;int _tmp69A;_LL39B: if(_tmp698 != 0)goto _LL39D;_LL39C: {struct Cyc_Absyn_Exp*
initopt=0;if((void*)f->sc != (void*)3){char zero='\000';initopt=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_string_exp((struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp6A1;
_tmp6A1.tag=0;_tmp6A1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*fn);{
struct Cyc_Int_pa_struct _tmp6A0;_tmp6A0.tag=1;_tmp6A0.f1=(unsigned long)((int)
zero);{struct Cyc_Int_pa_struct _tmp69F;_tmp69F.tag=1;_tmp69F.f1=(unsigned long)((
int)zero);{struct Cyc_Int_pa_struct _tmp69E;_tmp69E.tag=1;_tmp69E.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp69D;_tmp69D.tag=1;_tmp69D.f1=(
unsigned long)((int)zero);{void*_tmp69B[5]={& _tmp69D,& _tmp69E,& _tmp69F,& _tmp6A0,&
_tmp6A1};Cyc_aprintf(({const char*_tmp69C="%c%c%c%c%s";_tag_dynforward(_tmp69C,
sizeof(char),_get_zero_arr_size(_tmp69C,11));}),_tag_dynforward(_tmp69B,sizeof(
void*),5));}}}}}}),0);}{struct Cyc_Absyn_Vardecl*_tmp6A2=Cyc_Absyn_new_vardecl(f->name,
_tmp697,initopt);(void*)(_tmp6A2->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp6A3=_cycalloc(sizeof(*_tmp6A3));_tmp6A3->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp6A4=_cycalloc(sizeof(*_tmp6A4));_tmp6A4[
0]=({struct Cyc_Absyn_Var_d_struct _tmp6A5;_tmp6A5.tag=0;_tmp6A5.f1=_tmp6A2;
_tmp6A5;});_tmp6A4;}),0);_tmp6A3->tl=Cyc_Toc_result_decls;_tmp6A3;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,(void*)f->sc != (void*)3));if(f->typs != 0){struct Cyc_List_List*fields=0;
int i=1;{struct Cyc_List_List*_tmp6A6=f->typs;for(0;_tmp6A6 != 0;(_tmp6A6=_tmp6A6->tl,
i ++)){struct _dynforward_ptr*_tmp6A7=({struct _dynforward_ptr*_tmp6AB=_cycalloc(
sizeof(*_tmp6AB));_tmp6AB[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp6AE;_tmp6AE.tag=1;_tmp6AE.f1=(unsigned long)i;{void*_tmp6AC[1]={& _tmp6AE};
Cyc_aprintf(({const char*_tmp6AD="f%d";_tag_dynforward(_tmp6AD,sizeof(char),
_get_zero_arr_size(_tmp6AD,4));}),_tag_dynforward(_tmp6AC,sizeof(void*),1));}});
_tmp6AB;});struct Cyc_Absyn_Aggrfield*_tmp6A8=({struct Cyc_Absyn_Aggrfield*_tmp6AA=
_cycalloc(sizeof(*_tmp6AA));_tmp6AA->name=_tmp6A7;_tmp6AA->tq=(*((struct _tuple4*)
_tmp6A6->hd)).f1;_tmp6AA->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)
_tmp6A6->hd)).f2);_tmp6AA->width=0;_tmp6AA->attributes=0;_tmp6AA;});fields=({
struct Cyc_List_List*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));_tmp6A9->hd=_tmp6A8;
_tmp6A9->tl=fields;_tmp6A9;});}}fields=({struct Cyc_List_List*_tmp6AF=_cycalloc(
sizeof(*_tmp6AF));_tmp6AF->hd=({struct Cyc_Absyn_Aggrfield*_tmp6B0=_cycalloc(
sizeof(*_tmp6B0));_tmp6B0->name=Cyc_Toc_tag_sp;_tmp6B0->tq=Cyc_Toc_mt_tq;_tmp6B0->type=(
void*)Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp6B0->width=0;
_tmp6B0->attributes=0;_tmp6B0;});_tmp6AF->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(fields);_tmp6AF;});{struct Cyc_Absyn_Aggrdecl*_tmp6B1=({
struct Cyc_Absyn_Aggrdecl*_tmp6B5=_cycalloc(sizeof(*_tmp6B5));_tmp6B5->kind=(void*)((
void*)0);_tmp6B5->sc=(void*)((void*)2);_tmp6B5->name=Cyc_Toc_collapse_qvar_tag(f->name,({
const char*_tmp6B7="_struct";_tag_dynforward(_tmp6B7,sizeof(char),
_get_zero_arr_size(_tmp6B7,8));}));_tmp6B5->tvs=0;_tmp6B5->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp6B6=_cycalloc(sizeof(*_tmp6B6));_tmp6B6->exist_vars=0;_tmp6B6->rgn_po=0;
_tmp6B6->fields=fields;_tmp6B6;});_tmp6B5->attributes=0;_tmp6B5;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp6B2=_cycalloc(sizeof(*_tmp6B2));_tmp6B2->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp6B3=_cycalloc(sizeof(*_tmp6B3));
_tmp6B3[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp6B4;_tmp6B4.tag=4;_tmp6B4.f1=
_tmp6B1;_tmp6B4;});_tmp6B3;}),0);_tmp6B2->tl=Cyc_Toc_result_decls;_tmp6B2;});}}
goto _LL39A;}}_LL39D: if(_tmp698 == 0)goto _LL39F;_tmp699=*_tmp698;_tmp69A=(int)
_tmp699.v;if(_tmp69A != 0)goto _LL39F;_LL39E: if((void*)f->sc != (void*)3){char zero='\000';
struct Cyc_Absyn_Exp*_tmp6B8=Cyc_Absyn_string_exp((struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp6C3;_tmp6C3.tag=0;_tmp6C3.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*fn);{struct Cyc_Int_pa_struct _tmp6C2;_tmp6C2.tag=1;_tmp6C2.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp6C1;_tmp6C1.tag=1;_tmp6C1.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp6C0;_tmp6C0.tag=1;_tmp6C0.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp6BF;_tmp6BF.tag=1;_tmp6BF.f1=(
unsigned long)((int)zero);{void*_tmp6BD[5]={& _tmp6BF,& _tmp6C0,& _tmp6C1,& _tmp6C2,&
_tmp6C3};Cyc_aprintf(({const char*_tmp6BE="%c%c%c%c%s";_tag_dynforward(_tmp6BE,
sizeof(char),_get_zero_arr_size(_tmp6BE,11));}),_tag_dynforward(_tmp6BD,sizeof(
void*),5));}}}}}}),0);struct Cyc_Absyn_Vardecl*_tmp6B9=Cyc_Absyn_new_vardecl(f->name,
_tmp697,(struct Cyc_Absyn_Exp*)_tmp6B8);(void*)(_tmp6B9->sc=(void*)((void*)f->sc));
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));
_tmp6BA->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp6BB=
_cycalloc(sizeof(*_tmp6BB));_tmp6BB[0]=({struct Cyc_Absyn_Var_d_struct _tmp6BC;
_tmp6BC.tag=0;_tmp6BC.f1=_tmp6B9;_tmp6BC;});_tmp6BB;}),0);_tmp6BA->tl=Cyc_Toc_result_decls;
_tmp6BA;});(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(
void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,1));}goto _LL39A;_LL39F:;_LL3A0: goto _LL39A;_LL39A:;}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)
2));if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){void*old_typ=(void*)vd->type;(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array(
old_typ));{int forward_only=0;if((void*)vd->sc == (void*)5  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ,& forward_only))(void*)(vd->sc=(void*)((void*)2));Cyc_Toc_stmt_to_c(
body_nv,s);if(vd->initializer != 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)
_check_null(vd->initializer);void*_tmp6C4=(void*)init->r;struct Cyc_Absyn_Vardecl*
_tmp6C5;struct Cyc_Absyn_Exp*_tmp6C6;struct Cyc_Absyn_Exp*_tmp6C7;int _tmp6C8;
_LL3A2: if(*((int*)_tmp6C4)!= 29)goto _LL3A4;_tmp6C5=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp6C4)->f1;_tmp6C6=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp6C4)->f2;
_tmp6C7=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp6C4)->f3;_tmp6C8=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp6C4)->f4;_LL3A3: vd->initializer=0;(void*)(
s->r=(void*)((void*)(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(vd->name,
0),_tmp6C5,_tmp6C6,_tmp6C7,_tmp6C8,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));
goto _LL3A1;_LL3A4:;_LL3A5: if((void*)vd->sc == (void*)0){struct Cyc_Toc_Env*_tmp6C9=({
struct Cyc_Toc_Env*_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA[0]=*init_nv;
_tmp6CA;});_tmp6C9->toplevel=1;Cyc_Toc_exp_to_c(_tmp6C9,init);}else{Cyc_Toc_exp_to_c(
init_nv,init);}goto _LL3A1;_LL3A1:;}else{void*_tmp6CB=Cyc_Tcutil_compress(old_typ);
struct Cyc_Absyn_ArrayInfo _tmp6CC;void*_tmp6CD;struct Cyc_Absyn_Exp*_tmp6CE;struct
Cyc_Absyn_Conref*_tmp6CF;_LL3A7: if(_tmp6CB <= (void*)4)goto _LL3A9;if(*((int*)
_tmp6CB)!= 7)goto _LL3A9;_tmp6CC=((struct Cyc_Absyn_ArrayType_struct*)_tmp6CB)->f1;
_tmp6CD=(void*)_tmp6CC.elt_type;_tmp6CE=_tmp6CC.num_elts;_tmp6CF=_tmp6CC.zero_term;
if(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp6CF))goto
_LL3A9;_LL3A8: if(_tmp6CE == 0)({void*_tmp6D0[0]={};((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp6D1="can't initialize zero-terminated array -- size unknown";
_tag_dynforward(_tmp6D1,sizeof(char),_get_zero_arr_size(_tmp6D1,55));}),
_tag_dynforward(_tmp6D0,sizeof(void*),0));});{struct Cyc_Absyn_Exp*num_elts=(
struct Cyc_Absyn_Exp*)_tmp6CE;struct Cyc_Absyn_Exp*_tmp6D2=Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(
- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp6D3=Cyc_Absyn_signed_int_exp(0,0);(void*)(s->r=(
void*)Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp6D2,_tmp6D3,
0),0),Cyc_Absyn_new_stmt((void*)s->r,0)));goto _LL3A6;}_LL3A9:;_LL3AA: goto _LL3A6;
_LL3A6:;}}}static struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=0;static
struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){if(Cyc_Toc_throw_match_stmt_opt
== 0)Cyc_Toc_throw_match_stmt_opt=({struct Cyc_Absyn_Stmt**_tmp6D4=_cycalloc(
sizeof(*_tmp6D4));_tmp6D4[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(
0)),0);_tmp6D4;});return*((struct Cyc_Absyn_Stmt**)_check_null(Cyc_Toc_throw_match_stmt_opt));}
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();{struct _RegionHandle _tmp6D5=_new_region("prgn");
struct _RegionHandle*prgn=& _tmp6D5;_push_region(prgn);{struct Cyc_Absyn_Stmt*
_tmp6D6=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*_tmp6D8;struct Cyc_List_List*
_tmp6D9;struct Cyc_Absyn_Stmt*_tmp6DA;struct _tuple12 _tmp6D7=Cyc_Toc_xlate_pat(nv,
prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,(struct Cyc_Absyn_Stmt**)&
_tmp6D6,Cyc_Toc_throw_match_stmt(),0);_tmp6D8=_tmp6D7.f1;_tmp6D9=_tmp6D7.f2;
_tmp6DA=_tmp6D7.f3;Cyc_Toc_stmt_to_c(_tmp6D8,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(
t),(struct Cyc_Absyn_Exp*)e,Cyc_Absyn_seq_stmt(_tmp6DA,s,0),0);for(0;_tmp6D9 != 0;
_tmp6D9=_tmp6D9->tl){struct _tuple13 _tmp6DC;struct _tuple1*_tmp6DD;void*_tmp6DE;
struct _tuple13*_tmp6DB=(struct _tuple13*)_tmp6D9->hd;_tmp6DC=*_tmp6DB;_tmp6DD=
_tmp6DC.f1;_tmp6DE=_tmp6DC.f2;s=Cyc_Absyn_declare_stmt(_tmp6DD,_tmp6DE,0,s,0);}};
_pop_region(prgn);}return s;}}static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*
e){void*_tmp6DF=(void*)e->r;struct Cyc_Absyn_Exp*_tmp6E0;struct Cyc_Absyn_Exp*
_tmp6E1;struct Cyc_Absyn_Exp*_tmp6E2;struct Cyc_Absyn_Exp*_tmp6E3;struct Cyc_Absyn_Exp*
_tmp6E4;struct Cyc_Absyn_Exp*_tmp6E5;struct Cyc_Absyn_Exp*_tmp6E6;struct Cyc_Absyn_Exp*
_tmp6E7;struct Cyc_List_List*_tmp6E8;struct Cyc_Absyn_Exp*_tmp6E9;struct Cyc_Absyn_Exp*
_tmp6EA;struct Cyc_Absyn_Exp*_tmp6EB;struct Cyc_Absyn_Exp*_tmp6EC;struct Cyc_Absyn_Exp*
_tmp6ED;struct Cyc_Absyn_Exp*_tmp6EE;struct Cyc_Absyn_Exp*_tmp6EF;struct Cyc_Absyn_Exp*
_tmp6F0;struct Cyc_Absyn_Exp*_tmp6F1;struct Cyc_Absyn_Exp*_tmp6F2;struct Cyc_Absyn_Exp*
_tmp6F3;struct Cyc_Absyn_Exp*_tmp6F4;struct Cyc_Absyn_Exp*_tmp6F5;struct Cyc_Absyn_Exp*
_tmp6F6;struct Cyc_List_List*_tmp6F7;struct Cyc_Absyn_Exp*_tmp6F8;struct Cyc_List_List*
_tmp6F9;void*_tmp6FA;void**_tmp6FB;struct Cyc_Absyn_Exp*_tmp6FC;struct _tuple2*
_tmp6FD;struct _tuple2 _tmp6FE;void*_tmp6FF;void**_tmp700;struct Cyc_List_List*
_tmp701;struct Cyc_List_List*_tmp702;struct Cyc_List_List*_tmp703;void*_tmp704;
void**_tmp705;void*_tmp706;void**_tmp707;struct Cyc_Absyn_Stmt*_tmp708;struct Cyc_Absyn_MallocInfo
_tmp709;struct Cyc_Absyn_MallocInfo*_tmp70A;_LL3AC: if(*((int*)_tmp6DF)!= 22)goto
_LL3AE;_tmp6E0=((struct Cyc_Absyn_Deref_e_struct*)_tmp6DF)->f1;_LL3AD: _tmp6E1=
_tmp6E0;goto _LL3AF;_LL3AE: if(*((int*)_tmp6DF)!= 23)goto _LL3B0;_tmp6E1=((struct
Cyc_Absyn_AggrMember_e_struct*)_tmp6DF)->f1;_LL3AF: _tmp6E2=_tmp6E1;goto _LL3B1;
_LL3B0: if(*((int*)_tmp6DF)!= 24)goto _LL3B2;_tmp6E2=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp6DF)->f1;_LL3B1: _tmp6E3=_tmp6E2;goto _LL3B3;_LL3B2: if(*((int*)_tmp6DF)!= 16)
goto _LL3B4;_tmp6E3=((struct Cyc_Absyn_Address_e_struct*)_tmp6DF)->f1;_LL3B3:
_tmp6E4=_tmp6E3;goto _LL3B5;_LL3B4: if(*((int*)_tmp6DF)!= 12)goto _LL3B6;_tmp6E4=((
struct Cyc_Absyn_Throw_e_struct*)_tmp6DF)->f1;_LL3B5: _tmp6E5=_tmp6E4;goto _LL3B7;
_LL3B6: if(*((int*)_tmp6DF)!= 13)goto _LL3B8;_tmp6E5=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp6DF)->f1;_LL3B7: _tmp6E6=_tmp6E5;goto _LL3B9;_LL3B8: if(*((int*)_tmp6DF)!= 19)
goto _LL3BA;_tmp6E6=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp6DF)->f1;_LL3B9:
_tmp6E7=_tmp6E6;goto _LL3BB;_LL3BA: if(*((int*)_tmp6DF)!= 5)goto _LL3BC;_tmp6E7=((
struct Cyc_Absyn_Increment_e_struct*)_tmp6DF)->f1;_LL3BB: Cyc_Toc_exptypes_to_c(
_tmp6E7);goto _LL3AB;_LL3BC: if(*((int*)_tmp6DF)!= 3)goto _LL3BE;_tmp6E8=((struct
Cyc_Absyn_Primop_e_struct*)_tmp6DF)->f2;_LL3BD:((void(*)(void(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp6E8);goto _LL3AB;
_LL3BE: if(*((int*)_tmp6DF)!= 7)goto _LL3C0;_tmp6E9=((struct Cyc_Absyn_And_e_struct*)
_tmp6DF)->f1;_tmp6EA=((struct Cyc_Absyn_And_e_struct*)_tmp6DF)->f2;_LL3BF: _tmp6EB=
_tmp6E9;_tmp6EC=_tmp6EA;goto _LL3C1;_LL3C0: if(*((int*)_tmp6DF)!= 8)goto _LL3C2;
_tmp6EB=((struct Cyc_Absyn_Or_e_struct*)_tmp6DF)->f1;_tmp6EC=((struct Cyc_Absyn_Or_e_struct*)
_tmp6DF)->f2;_LL3C1: _tmp6ED=_tmp6EB;_tmp6EE=_tmp6EC;goto _LL3C3;_LL3C2: if(*((int*)
_tmp6DF)!= 9)goto _LL3C4;_tmp6ED=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp6DF)->f1;
_tmp6EE=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp6DF)->f2;_LL3C3: _tmp6EF=_tmp6ED;
_tmp6F0=_tmp6EE;goto _LL3C5;_LL3C4: if(*((int*)_tmp6DF)!= 25)goto _LL3C6;_tmp6EF=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp6DF)->f1;_tmp6F0=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp6DF)->f2;_LL3C5: _tmp6F1=_tmp6EF;_tmp6F2=_tmp6F0;goto _LL3C7;_LL3C6: if(*((int*)
_tmp6DF)!= 4)goto _LL3C8;_tmp6F1=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp6DF)->f1;
_tmp6F2=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp6DF)->f3;_LL3C7: Cyc_Toc_exptypes_to_c(
_tmp6F1);Cyc_Toc_exptypes_to_c(_tmp6F2);goto _LL3AB;_LL3C8: if(*((int*)_tmp6DF)!= 
6)goto _LL3CA;_tmp6F3=((struct Cyc_Absyn_Conditional_e_struct*)_tmp6DF)->f1;
_tmp6F4=((struct Cyc_Absyn_Conditional_e_struct*)_tmp6DF)->f2;_tmp6F5=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp6DF)->f3;_LL3C9: Cyc_Toc_exptypes_to_c(_tmp6F3);Cyc_Toc_exptypes_to_c(_tmp6F4);
Cyc_Toc_exptypes_to_c(_tmp6F5);goto _LL3AB;_LL3CA: if(*((int*)_tmp6DF)!= 11)goto
_LL3CC;_tmp6F6=((struct Cyc_Absyn_FnCall_e_struct*)_tmp6DF)->f1;_tmp6F7=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp6DF)->f2;_LL3CB: _tmp6F8=_tmp6F6;_tmp6F9=_tmp6F7;
goto _LL3CD;_LL3CC: if(*((int*)_tmp6DF)!= 10)goto _LL3CE;_tmp6F8=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp6DF)->f1;_tmp6F9=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp6DF)->f2;_LL3CD:
Cyc_Toc_exptypes_to_c(_tmp6F8);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct
Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp6F9);goto _LL3AB;_LL3CE:
if(*((int*)_tmp6DF)!= 15)goto _LL3D0;_tmp6FA=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp6DF)->f1;_tmp6FB=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp6DF)->f1);
_tmp6FC=((struct Cyc_Absyn_Cast_e_struct*)_tmp6DF)->f2;_LL3CF:*_tmp6FB=Cyc_Toc_typ_to_c(*
_tmp6FB);Cyc_Toc_exptypes_to_c(_tmp6FC);goto _LL3AB;_LL3D0: if(*((int*)_tmp6DF)!= 
27)goto _LL3D2;_tmp6FD=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp6DF)->f1;
_tmp6FE=*_tmp6FD;_tmp6FF=_tmp6FE.f3;_tmp700=(void**)&(*((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp6DF)->f1).f3;_tmp701=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp6DF)->f2;
_LL3D1:*_tmp700=Cyc_Toc_typ_to_c(*_tmp700);_tmp702=_tmp701;goto _LL3D3;_LL3D2: if(*((
int*)_tmp6DF)!= 36)goto _LL3D4;_tmp702=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp6DF)->f2;_LL3D3: _tmp703=_tmp702;goto _LL3D5;_LL3D4: if(*((int*)_tmp6DF)!= 28)
goto _LL3D6;_tmp703=((struct Cyc_Absyn_Array_e_struct*)_tmp6DF)->f1;_LL3D5: for(0;
_tmp703 != 0;_tmp703=_tmp703->tl){struct Cyc_Absyn_Exp*_tmp70C;struct _tuple6
_tmp70B=*((struct _tuple6*)_tmp703->hd);_tmp70C=_tmp70B.f2;Cyc_Toc_exptypes_to_c(
_tmp70C);}goto _LL3AB;_LL3D6: if(*((int*)_tmp6DF)!= 20)goto _LL3D8;_tmp704=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp6DF)->f1;_tmp705=(void**)&((void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp6DF)->f1);_LL3D7: _tmp707=_tmp705;goto
_LL3D9;_LL3D8: if(*((int*)_tmp6DF)!= 18)goto _LL3DA;_tmp706=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp6DF)->f1;_tmp707=(void**)&((void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp6DF)->f1);_LL3D9:*_tmp707=Cyc_Toc_typ_to_c(*_tmp707);goto _LL3AB;_LL3DA: if(*((
int*)_tmp6DF)!= 37)goto _LL3DC;_tmp708=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp6DF)->f1;_LL3DB: Cyc_Toc_stmttypes_to_c(_tmp708);goto _LL3AB;_LL3DC: if(*((int*)
_tmp6DF)!= 35)goto _LL3DE;_tmp709=((struct Cyc_Absyn_Malloc_e_struct*)_tmp6DF)->f1;
_tmp70A=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_struct*)_tmp6DF)->f1;
_LL3DD: if(_tmp70A->elt_type != 0)_tmp70A->elt_type=({void**_tmp70D=_cycalloc(
sizeof(*_tmp70D));_tmp70D[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp70A->elt_type)));
_tmp70D;});Cyc_Toc_exptypes_to_c(_tmp70A->num_elts);goto _LL3AB;_LL3DE: if(*((int*)
_tmp6DF)!= 0)goto _LL3E0;_LL3DF: goto _LL3E1;_LL3E0: if(*((int*)_tmp6DF)!= 1)goto
_LL3E2;_LL3E1: goto _LL3E3;_LL3E2: if(*((int*)_tmp6DF)!= 2)goto _LL3E4;_LL3E3: goto
_LL3E5;_LL3E4: if(*((int*)_tmp6DF)!= 33)goto _LL3E6;_LL3E5: goto _LL3E7;_LL3E6: if(*((
int*)_tmp6DF)!= 34)goto _LL3E8;_LL3E7: goto _LL3AB;_LL3E8: if(*((int*)_tmp6DF)!= 31)
goto _LL3EA;_LL3E9: goto _LL3EB;_LL3EA: if(*((int*)_tmp6DF)!= 32)goto _LL3EC;_LL3EB:
goto _LL3ED;_LL3EC: if(*((int*)_tmp6DF)!= 30)goto _LL3EE;_LL3ED: goto _LL3EF;_LL3EE:
if(*((int*)_tmp6DF)!= 29)goto _LL3F0;_LL3EF: goto _LL3F1;_LL3F0: if(*((int*)_tmp6DF)
!= 26)goto _LL3F2;_LL3F1: goto _LL3F3;_LL3F2: if(*((int*)_tmp6DF)!= 14)goto _LL3F4;
_LL3F3: goto _LL3F5;_LL3F4: if(*((int*)_tmp6DF)!= 17)goto _LL3F6;_LL3F5: goto _LL3F7;
_LL3F6: if(*((int*)_tmp6DF)!= 21)goto _LL3AB;_LL3F7:({void*_tmp70E[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp70F="Cyclone expression within C code";_tag_dynforward(
_tmp70F,sizeof(char),_get_zero_arr_size(_tmp70F,33));}),_tag_dynforward(_tmp70E,
sizeof(void*),0));});goto _LL3AB;_LL3AB:;}static void Cyc_Toc_decltypes_to_c(struct
Cyc_Absyn_Decl*d){void*_tmp710=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp711;
struct Cyc_Absyn_Fndecl*_tmp712;struct Cyc_Absyn_Aggrdecl*_tmp713;struct Cyc_Absyn_Enumdecl*
_tmp714;struct Cyc_Absyn_Typedefdecl*_tmp715;_LL3F9: if(_tmp710 <= (void*)2)goto
_LL411;if(*((int*)_tmp710)!= 0)goto _LL3FB;_tmp711=((struct Cyc_Absyn_Var_d_struct*)
_tmp710)->f1;_LL3FA:(void*)(_tmp711->type=(void*)Cyc_Toc_typ_to_c((void*)_tmp711->type));
if(_tmp711->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp711->initializer));goto _LL3F8;_LL3FB: if(*((int*)_tmp710)!= 1)
goto _LL3FD;_tmp712=((struct Cyc_Absyn_Fn_d_struct*)_tmp710)->f1;_LL3FC:(void*)(
_tmp712->ret_type=(void*)Cyc_Toc_typ_to_c((void*)_tmp712->ret_type));{struct Cyc_List_List*
_tmp716=_tmp712->args;for(0;_tmp716 != 0;_tmp716=_tmp716->tl){(*((struct _tuple16*)
_tmp716->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple16*)_tmp716->hd)).f3);}}goto
_LL3F8;_LL3FD: if(*((int*)_tmp710)!= 4)goto _LL3FF;_tmp713=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp710)->f1;_LL3FE: Cyc_Toc_aggrdecl_to_c(_tmp713);goto _LL3F8;_LL3FF: if(*((int*)
_tmp710)!= 6)goto _LL401;_tmp714=((struct Cyc_Absyn_Enum_d_struct*)_tmp710)->f1;
_LL400: if(_tmp714->fields != 0){struct Cyc_List_List*_tmp717=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp714->fields))->v;for(0;_tmp717 != 0;_tmp717=
_tmp717->tl){struct Cyc_Absyn_Enumfield*_tmp718=(struct Cyc_Absyn_Enumfield*)
_tmp717->hd;if(_tmp718->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp718->tag));}}goto _LL3F8;_LL401: if(*((int*)_tmp710)!= 7)goto
_LL403;_tmp715=((struct Cyc_Absyn_Typedef_d_struct*)_tmp710)->f1;_LL402: _tmp715->defn=({
struct Cyc_Core_Opt*_tmp719=_cycalloc(sizeof(*_tmp719));_tmp719->v=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp715->defn))->v);_tmp719;});goto
_LL3F8;_LL403: if(*((int*)_tmp710)!= 2)goto _LL405;_LL404: goto _LL406;_LL405: if(*((
int*)_tmp710)!= 3)goto _LL407;_LL406: goto _LL408;_LL407: if(*((int*)_tmp710)!= 5)
goto _LL409;_LL408: goto _LL40A;_LL409: if(*((int*)_tmp710)!= 8)goto _LL40B;_LL40A:
goto _LL40C;_LL40B: if(*((int*)_tmp710)!= 9)goto _LL40D;_LL40C: goto _LL40E;_LL40D:
if(*((int*)_tmp710)!= 10)goto _LL40F;_LL40E: goto _LL410;_LL40F: if(*((int*)_tmp710)
!= 11)goto _LL411;_LL410:({void*_tmp71A[0]={};Cyc_Tcutil_terr(d->loc,({const char*
_tmp71B="Cyclone declaration within C code";_tag_dynforward(_tmp71B,sizeof(char),
_get_zero_arr_size(_tmp71B,34));}),_tag_dynforward(_tmp71A,sizeof(void*),0));});
goto _LL3F8;_LL411: if((int)_tmp710 != 0)goto _LL413;_LL412: goto _LL414;_LL413: if((
int)_tmp710 != 1)goto _LL3F8;_LL414: goto _LL3F8;_LL3F8:;}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s){void*_tmp71C=(void*)s->r;struct Cyc_Absyn_Exp*_tmp71D;
struct Cyc_Absyn_Stmt*_tmp71E;struct Cyc_Absyn_Stmt*_tmp71F;struct Cyc_Absyn_Exp*
_tmp720;struct Cyc_Absyn_Exp*_tmp721;struct Cyc_Absyn_Stmt*_tmp722;struct Cyc_Absyn_Stmt*
_tmp723;struct _tuple3 _tmp724;struct Cyc_Absyn_Exp*_tmp725;struct Cyc_Absyn_Stmt*
_tmp726;struct Cyc_Absyn_Exp*_tmp727;struct _tuple3 _tmp728;struct Cyc_Absyn_Exp*
_tmp729;struct _tuple3 _tmp72A;struct Cyc_Absyn_Exp*_tmp72B;struct Cyc_Absyn_Stmt*
_tmp72C;struct Cyc_Absyn_Exp*_tmp72D;struct Cyc_List_List*_tmp72E;struct Cyc_Absyn_Decl*
_tmp72F;struct Cyc_Absyn_Stmt*_tmp730;struct Cyc_Absyn_Stmt*_tmp731;struct _tuple3
_tmp732;struct Cyc_Absyn_Exp*_tmp733;_LL416: if(_tmp71C <= (void*)1)goto _LL428;if(*((
int*)_tmp71C)!= 0)goto _LL418;_tmp71D=((struct Cyc_Absyn_Exp_s_struct*)_tmp71C)->f1;
_LL417: Cyc_Toc_exptypes_to_c(_tmp71D);goto _LL415;_LL418: if(*((int*)_tmp71C)!= 1)
goto _LL41A;_tmp71E=((struct Cyc_Absyn_Seq_s_struct*)_tmp71C)->f1;_tmp71F=((struct
Cyc_Absyn_Seq_s_struct*)_tmp71C)->f2;_LL419: Cyc_Toc_stmttypes_to_c(_tmp71E);Cyc_Toc_stmttypes_to_c(
_tmp71F);goto _LL415;_LL41A: if(*((int*)_tmp71C)!= 2)goto _LL41C;_tmp720=((struct
Cyc_Absyn_Return_s_struct*)_tmp71C)->f1;_LL41B: if(_tmp720 != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_tmp720);goto _LL415;_LL41C: if(*((int*)_tmp71C)!= 3)goto
_LL41E;_tmp721=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp71C)->f1;_tmp722=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp71C)->f2;_tmp723=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp71C)->f3;_LL41D: Cyc_Toc_exptypes_to_c(_tmp721);Cyc_Toc_stmttypes_to_c(
_tmp722);Cyc_Toc_stmttypes_to_c(_tmp723);goto _LL415;_LL41E: if(*((int*)_tmp71C)!= 
4)goto _LL420;_tmp724=((struct Cyc_Absyn_While_s_struct*)_tmp71C)->f1;_tmp725=
_tmp724.f1;_tmp726=((struct Cyc_Absyn_While_s_struct*)_tmp71C)->f2;_LL41F: Cyc_Toc_exptypes_to_c(
_tmp725);Cyc_Toc_stmttypes_to_c(_tmp726);goto _LL415;_LL420: if(*((int*)_tmp71C)!= 
8)goto _LL422;_tmp727=((struct Cyc_Absyn_For_s_struct*)_tmp71C)->f1;_tmp728=((
struct Cyc_Absyn_For_s_struct*)_tmp71C)->f2;_tmp729=_tmp728.f1;_tmp72A=((struct
Cyc_Absyn_For_s_struct*)_tmp71C)->f3;_tmp72B=_tmp72A.f1;_tmp72C=((struct Cyc_Absyn_For_s_struct*)
_tmp71C)->f4;_LL421: Cyc_Toc_exptypes_to_c(_tmp727);Cyc_Toc_exptypes_to_c(_tmp729);
Cyc_Toc_exptypes_to_c(_tmp72B);Cyc_Toc_stmttypes_to_c(_tmp72C);goto _LL415;_LL422:
if(*((int*)_tmp71C)!= 9)goto _LL424;_tmp72D=((struct Cyc_Absyn_Switch_s_struct*)
_tmp71C)->f1;_tmp72E=((struct Cyc_Absyn_Switch_s_struct*)_tmp71C)->f2;_LL423: Cyc_Toc_exptypes_to_c(
_tmp72D);for(0;_tmp72E != 0;_tmp72E=_tmp72E->tl){Cyc_Toc_stmttypes_to_c(((struct
Cyc_Absyn_Switch_clause*)_tmp72E->hd)->body);}goto _LL415;_LL424: if(*((int*)
_tmp71C)!= 11)goto _LL426;_tmp72F=((struct Cyc_Absyn_Decl_s_struct*)_tmp71C)->f1;
_tmp730=((struct Cyc_Absyn_Decl_s_struct*)_tmp71C)->f2;_LL425: Cyc_Toc_decltypes_to_c(
_tmp72F);Cyc_Toc_stmttypes_to_c(_tmp730);goto _LL415;_LL426: if(*((int*)_tmp71C)!= 
13)goto _LL428;_tmp731=((struct Cyc_Absyn_Do_s_struct*)_tmp71C)->f1;_tmp732=((
struct Cyc_Absyn_Do_s_struct*)_tmp71C)->f2;_tmp733=_tmp732.f1;_LL427: Cyc_Toc_stmttypes_to_c(
_tmp731);Cyc_Toc_exptypes_to_c(_tmp733);goto _LL415;_LL428: if((int)_tmp71C != 0)
goto _LL42A;_LL429: goto _LL42B;_LL42A: if(_tmp71C <= (void*)1)goto _LL42C;if(*((int*)
_tmp71C)!= 5)goto _LL42C;_LL42B: goto _LL42D;_LL42C: if(_tmp71C <= (void*)1)goto
_LL42E;if(*((int*)_tmp71C)!= 6)goto _LL42E;_LL42D: goto _LL42F;_LL42E: if(_tmp71C <= (
void*)1)goto _LL430;if(*((int*)_tmp71C)!= 7)goto _LL430;_LL42F: goto _LL415;_LL430:
if(_tmp71C <= (void*)1)goto _LL432;if(*((int*)_tmp71C)!= 10)goto _LL432;_LL431: goto
_LL433;_LL432: if(_tmp71C <= (void*)1)goto _LL434;if(*((int*)_tmp71C)!= 12)goto
_LL434;_LL433: goto _LL435;_LL434: if(_tmp71C <= (void*)1)goto _LL436;if(*((int*)
_tmp71C)!= 14)goto _LL436;_LL435: goto _LL437;_LL436: if(_tmp71C <= (void*)1)goto
_LL438;if(*((int*)_tmp71C)!= 15)goto _LL438;_LL437: goto _LL439;_LL438: if(_tmp71C <= (
void*)1)goto _LL415;if(*((int*)_tmp71C)!= 16)goto _LL415;_LL439:({void*_tmp734[0]={};
Cyc_Tcutil_terr(s->loc,({const char*_tmp735="Cyclone statement in C code";
_tag_dynforward(_tmp735,sizeof(char),_get_zero_arr_size(_tmp735,28));}),
_tag_dynforward(_tmp734,sizeof(void*),0));});goto _LL415;_LL415:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int
cinclude){for(0;ds != 0;ds=ds->tl){if(!nv->toplevel)({void*_tmp736[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp737="decls_to_c: not at toplevel!";_tag_dynforward(_tmp737,sizeof(char),
_get_zero_arr_size(_tmp737,29));}),_tag_dynforward(_tmp736,sizeof(void*),0));});{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmp738=(void*)d->r;
struct Cyc_Absyn_Vardecl*_tmp739;struct Cyc_Absyn_Fndecl*_tmp73A;struct Cyc_Absyn_Aggrdecl*
_tmp73B;struct Cyc_Absyn_Tuniondecl*_tmp73C;struct Cyc_Absyn_Enumdecl*_tmp73D;
struct Cyc_Absyn_Typedefdecl*_tmp73E;struct Cyc_List_List*_tmp73F;struct Cyc_List_List*
_tmp740;struct Cyc_List_List*_tmp741;struct Cyc_List_List*_tmp742;_LL43B: if(
_tmp738 <= (void*)2)goto _LL44B;if(*((int*)_tmp738)!= 0)goto _LL43D;_tmp739=((
struct Cyc_Absyn_Var_d_struct*)_tmp738)->f1;_LL43C: {struct _tuple1*_tmp743=
_tmp739->name;if((void*)_tmp739->sc == (void*)4)_tmp743=({struct _tuple1*_tmp744=
_cycalloc(sizeof(*_tmp744));_tmp744->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp745;(_tmp745.Rel_n).tag=1;(_tmp745.Rel_n).f1=0;_tmp745;});_tmp744->f2=(*
_tmp743).f2;_tmp744;});if(_tmp739->initializer != 0){if(cinclude)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmp739->initializer));else{Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_check_null(_tmp739->initializer));}}nv=Cyc_Toc_add_varmap(
nv,_tmp739->name,Cyc_Absyn_varb_exp(_tmp743,(void*)({struct Cyc_Absyn_Global_b_struct*
_tmp746=_cycalloc(sizeof(*_tmp746));_tmp746[0]=({struct Cyc_Absyn_Global_b_struct
_tmp747;_tmp747.tag=0;_tmp747.f1=_tmp739;_tmp747;});_tmp746;}),0));_tmp739->name=
_tmp743;(void*)(_tmp739->sc=(void*)Cyc_Toc_scope_to_c((void*)_tmp739->sc));(void*)(
_tmp739->type=(void*)Cyc_Toc_typ_to_c_array((void*)_tmp739->type));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp748=_cycalloc(sizeof(*_tmp748));_tmp748->hd=d;_tmp748->tl=
Cyc_Toc_result_decls;_tmp748;});goto _LL43A;}_LL43D: if(*((int*)_tmp738)!= 1)goto
_LL43F;_tmp73A=((struct Cyc_Absyn_Fn_d_struct*)_tmp738)->f1;_LL43E: {struct
_tuple1*_tmp749=_tmp73A->name;if((void*)_tmp73A->sc == (void*)4){_tmp749=({struct
_tuple1*_tmp74A=_cycalloc(sizeof(*_tmp74A));_tmp74A->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp74B;(_tmp74B.Rel_n).tag=1;(_tmp74B.Rel_n).f1=0;
_tmp74B;});_tmp74A->f2=(*_tmp749).f2;_tmp74A;});(void*)(_tmp73A->sc=(void*)((
void*)2));}nv=Cyc_Toc_add_varmap(nv,_tmp73A->name,Cyc_Absyn_var_exp(_tmp749,0));
_tmp73A->name=_tmp749;Cyc_Toc_fndecl_to_c(nv,_tmp73A,cinclude);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp74C=_cycalloc(sizeof(*_tmp74C));_tmp74C->hd=d;_tmp74C->tl=
Cyc_Toc_result_decls;_tmp74C;});goto _LL43A;}_LL43F: if(*((int*)_tmp738)!= 2)goto
_LL441;_LL440: goto _LL442;_LL441: if(*((int*)_tmp738)!= 3)goto _LL443;_LL442:({void*
_tmp74D[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp74E="letdecl at toplevel";_tag_dynforward(_tmp74E,sizeof(char),
_get_zero_arr_size(_tmp74E,20));}),_tag_dynforward(_tmp74D,sizeof(void*),0));});
_LL443: if(*((int*)_tmp738)!= 4)goto _LL445;_tmp73B=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp738)->f1;_LL444: Cyc_Toc_aggrdecl_to_c(_tmp73B);Cyc_Toc_result_decls=({struct
Cyc_List_List*_tmp74F=_cycalloc(sizeof(*_tmp74F));_tmp74F->hd=d;_tmp74F->tl=Cyc_Toc_result_decls;
_tmp74F;});goto _LL43A;_LL445: if(*((int*)_tmp738)!= 5)goto _LL447;_tmp73C=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp738)->f1;_LL446: if(_tmp73C->is_xtunion)Cyc_Toc_xtuniondecl_to_c(
_tmp73C);else{Cyc_Toc_tuniondecl_to_c(_tmp73C);}goto _LL43A;_LL447: if(*((int*)
_tmp738)!= 6)goto _LL449;_tmp73D=((struct Cyc_Absyn_Enum_d_struct*)_tmp738)->f1;
_LL448: Cyc_Toc_enumdecl_to_c(nv,_tmp73D);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp750=_cycalloc(sizeof(*_tmp750));_tmp750->hd=d;_tmp750->tl=Cyc_Toc_result_decls;
_tmp750;});goto _LL43A;_LL449: if(*((int*)_tmp738)!= 7)goto _LL44B;_tmp73E=((struct
Cyc_Absyn_Typedef_d_struct*)_tmp738)->f1;_LL44A: _tmp73E->name=_tmp73E->name;
_tmp73E->tvs=0;if(_tmp73E->defn != 0)_tmp73E->defn=({struct Cyc_Core_Opt*_tmp751=
_cycalloc(sizeof(*_tmp751));_tmp751->v=(void*)Cyc_Toc_typ_to_c_array((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp73E->defn))->v);_tmp751;});else{void*_tmp752=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp73E->kind))->v;_LL458: if((int)
_tmp752 != 2)goto _LL45A;_LL459: _tmp73E->defn=({struct Cyc_Core_Opt*_tmp753=
_cycalloc(sizeof(*_tmp753));_tmp753->v=(void*)Cyc_Absyn_void_star_typ();_tmp753;});
goto _LL457;_LL45A:;_LL45B: _tmp73E->defn=({struct Cyc_Core_Opt*_tmp754=_cycalloc(
sizeof(*_tmp754));_tmp754->v=(void*)((void*)0);_tmp754;});goto _LL457;_LL457:;}
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp755=_cycalloc(sizeof(*_tmp755));
_tmp755->hd=d;_tmp755->tl=Cyc_Toc_result_decls;_tmp755;});goto _LL43A;_LL44B: if((
int)_tmp738 != 0)goto _LL44D;_LL44C: goto _LL44E;_LL44D: if((int)_tmp738 != 1)goto
_LL44F;_LL44E: goto _LL43A;_LL44F: if(_tmp738 <= (void*)2)goto _LL451;if(*((int*)
_tmp738)!= 8)goto _LL451;_tmp73F=((struct Cyc_Absyn_Namespace_d_struct*)_tmp738)->f2;
_LL450: _tmp740=_tmp73F;goto _LL452;_LL451: if(_tmp738 <= (void*)2)goto _LL453;if(*((
int*)_tmp738)!= 9)goto _LL453;_tmp740=((struct Cyc_Absyn_Using_d_struct*)_tmp738)->f2;
_LL452: _tmp741=_tmp740;goto _LL454;_LL453: if(_tmp738 <= (void*)2)goto _LL455;if(*((
int*)_tmp738)!= 10)goto _LL455;_tmp741=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp738)->f1;_LL454: nv=Cyc_Toc_decls_to_c(nv,_tmp741,top,cinclude);goto _LL43A;
_LL455: if(_tmp738 <= (void*)2)goto _LL43A;if(*((int*)_tmp738)!= 11)goto _LL43A;
_tmp742=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp738)->f1;_LL456: nv=Cyc_Toc_decls_to_c(
nv,_tmp742,top,1);goto _LL43A;_LL43A:;}}return nv;}static void Cyc_Toc_init(){Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_types=0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;Cyc_Toc_aggrs_so_far=0;Cyc_Toc_tunions_so_far=0;
Cyc_Toc_xtunion_fields_so_far=0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=
0;}struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();Cyc_Toc_decls_to_c(
Cyc_Toc_empty_env(),ds,1,0);return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
