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
 struct Cyc_Core_Opt{void*v;};int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_rcopy(struct
_RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];void
Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);extern char Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;
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
char*tag;struct _dynforward_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_rempty(struct
_RegionHandle*,int(*cmp)(void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict d,
void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct _tuple0{void*f1;void*f2;
};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);struct
Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict,void*);int Cyc_strptrcmp(
struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct Cyc_Lineno_Pos{struct
_dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dynforward_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{
int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct
Loc_n;struct Cyc_Absyn_Rel_n_struct Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};
struct _tuple1{union Cyc_Absyn_Nmspace_union f1;struct _dynforward_ptr*f2;};struct
Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int
q_restrict;int real_const;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Eq_constr_struct{
int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_Absyn_No_constr_struct{int tag;};union Cyc_Absyn_Constraint_union{
struct Cyc_Absyn_Eq_constr_struct Eq_constr;struct Cyc_Absyn_Forward_constr_struct
Forward_constr;struct Cyc_Absyn_No_constr_struct No_constr;};struct Cyc_Absyn_Conref{
union Cyc_Absyn_Constraint_union v;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);extern struct _tuple1*Cyc_Absyn_exn_name;void*Cyc_Absyn_cstar_typ(
void*t,struct Cyc_Absyn_Tqual tq);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,
struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);struct Cyc_Set_Set;
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*
elt);extern char Cyc_Set_Absent[11];struct Cyc_Iter_Iter Cyc_Set_make_iter(struct
_RegionHandle*rgn,struct Cyc_Set_Set*s);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;};void*Cyc_Tcutil_compress(void*t);struct _tuple1*Cyc_Toc_temp_var();struct Cyc_List_List*
Cyc_Tovc_tovc(struct Cyc_List_List*decls);struct Cyc_Tovc_BoxingEnv{struct
_RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict varmap;
struct Cyc_List_List*boundvars;struct Cyc_Set_Set**freevars;};struct Cyc_Tovc_ToExpEnv{
struct _RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict varmap;
struct Cyc_Absyn_Stmt*encloser;struct Cyc_List_List**gen_ds;};struct _tuple4{struct
Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Tovc_box_free_vars_exp(
struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Exp*e){while(1){void*_tmp0=(void*)e->r;
struct _tuple1*_tmp1;struct _tuple1 _tmp2;union Cyc_Absyn_Nmspace_union _tmp3;struct
_dynforward_ptr*_tmp4;struct Cyc_Absyn_Exp*_tmp5;struct Cyc_List_List*_tmp6;struct
Cyc_List_List*_tmp7;struct Cyc_Absyn_Exp*_tmp8;struct Cyc_Absyn_Exp*_tmp9;struct
Cyc_Absyn_Exp*_tmpA;struct Cyc_Absyn_Exp*_tmpB;struct Cyc_Absyn_Exp*_tmpC;struct
Cyc_Absyn_Exp*_tmpD;struct Cyc_Absyn_Exp*_tmpE;struct Cyc_Absyn_Exp*_tmpF;struct
Cyc_Absyn_Exp*_tmp10;struct Cyc_Absyn_Exp*_tmp11;struct Cyc_Absyn_Exp*_tmp12;
struct Cyc_Absyn_Exp*_tmp13;struct Cyc_Absyn_Exp*_tmp14;struct Cyc_Absyn_Exp*_tmp15;
struct Cyc_Absyn_Exp*_tmp16;struct Cyc_Absyn_Exp*_tmp17;struct Cyc_Absyn_Exp*_tmp18;
struct Cyc_Absyn_Exp*_tmp19;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_Absyn_Exp*_tmp1B;
struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_List_List*_tmp1E;
_LL1: if(*((int*)_tmp0)!= 1)goto _LL3;_tmp1=((struct Cyc_Absyn_Var_e_struct*)_tmp0)->f1;
_tmp2=*_tmp1;_tmp3=_tmp2.f1;if((((*((struct Cyc_Absyn_Var_e_struct*)_tmp0)->f1).f1).Loc_n).tag
!= 0)goto _LL3;_tmp4=_tmp2.f2;_LL2: if(((int(*)(struct Cyc_Dict_Dict d,struct
_dynforward_ptr*k))Cyc_Dict_member)(env.varmap,_tmp4)){(void*)(e->r=(void*)((
void*)(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _dynforward_ptr*k))
Cyc_Dict_lookup)(env.varmap,_tmp4))->r));continue;}for(0;env.boundvars != 0;env.boundvars=(
env.boundvars)->tl){if(Cyc_strptrcmp(_tmp4,(*((struct Cyc_Absyn_Vardecl*)(env.boundvars)->hd)->name).f2)
== 0)return;}for(0;env.all_locals != 0;env.all_locals=(env.all_locals)->tl){
struct Cyc_Absyn_Vardecl*_tmp1F=(struct Cyc_Absyn_Vardecl*)(env.all_locals)->hd;
if(Cyc_strptrcmp(_tmp4,(*_tmp1F->name).f2)== 0){{void*_tmp20=(void*)_tmp1F->type;
_LL36: if(_tmp20 <= (void*)4)goto _LL38;if(*((int*)_tmp20)!= 7)goto _LL38;_LL37: goto
_LL35;_LL38:;_LL39:(void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(
_tmp1F->name,0),0))->r));goto _LL35;_LL35:;}*env.freevars=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Absyn_Vardecl*elt))Cyc_Set_rinsert)(
env.rgn,*env.freevars,_tmp1F);return;}}(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21[0]=({struct Cyc_Core_Impossible_struct
_tmp22;_tmp22.tag=Cyc_Core_Impossible;_tmp22.f1=(struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp25;_tmp25.tag=0;_tmp25.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*_tmp4);{void*_tmp23[1]={& _tmp25};Cyc_aprintf(({const char*
_tmp24="unbound variable %s found in box_free_vars_exp";_tag_dynforward(_tmp24,
sizeof(char),_get_zero_arr_size(_tmp24,47));}),_tag_dynforward(_tmp23,sizeof(
void*),1));}});_tmp22;});_tmp21;}));_LL3: if(*((int*)_tmp0)!= 1)goto _LL5;_LL4:
return;_LL5: if(*((int*)_tmp0)!= 0)goto _LL7;_LL6: return;_LL7: if(*((int*)_tmp0)!= 
11)goto _LL9;_tmp5=((struct Cyc_Absyn_FnCall_e_struct*)_tmp0)->f1;_tmp6=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp0)->f2;_LL8: Cyc_Tovc_box_free_vars_exp(env,_tmp5);
_tmp7=_tmp6;goto _LLA;_LL9: if(*((int*)_tmp0)!= 3)goto _LLB;_tmp7=((struct Cyc_Absyn_Primop_e_struct*)
_tmp0)->f2;_LLA: for(0;_tmp7 != 0;_tmp7=_tmp7->tl){Cyc_Tovc_box_free_vars_exp(env,(
struct Cyc_Absyn_Exp*)_tmp7->hd);}return;_LLB: if(*((int*)_tmp0)!= 6)goto _LLD;
_tmp8=((struct Cyc_Absyn_Conditional_e_struct*)_tmp0)->f1;_tmp9=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp0)->f2;_tmpA=((struct Cyc_Absyn_Conditional_e_struct*)_tmp0)->f3;_LLC: Cyc_Tovc_box_free_vars_exp(
env,_tmpA);_tmpB=_tmp8;_tmpC=_tmp9;goto _LLE;_LLD: if(*((int*)_tmp0)!= 7)goto _LLF;
_tmpB=((struct Cyc_Absyn_And_e_struct*)_tmp0)->f1;_tmpC=((struct Cyc_Absyn_And_e_struct*)
_tmp0)->f2;_LLE: _tmpD=_tmpB;_tmpE=_tmpC;goto _LL10;_LLF: if(*((int*)_tmp0)!= 8)
goto _LL11;_tmpD=((struct Cyc_Absyn_Or_e_struct*)_tmp0)->f1;_tmpE=((struct Cyc_Absyn_Or_e_struct*)
_tmp0)->f2;_LL10: _tmpF=_tmpD;_tmp10=_tmpE;goto _LL12;_LL11: if(*((int*)_tmp0)!= 25)
goto _LL13;_tmpF=((struct Cyc_Absyn_Subscript_e_struct*)_tmp0)->f1;_tmp10=((struct
Cyc_Absyn_Subscript_e_struct*)_tmp0)->f2;_LL12: _tmp11=_tmpF;_tmp12=_tmp10;goto
_LL14;_LL13: if(*((int*)_tmp0)!= 9)goto _LL15;_tmp11=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp0)->f1;_tmp12=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp0)->f2;_LL14: _tmp13=
_tmp11;_tmp14=_tmp12;goto _LL16;_LL15: if(*((int*)_tmp0)!= 4)goto _LL17;_tmp13=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp0)->f1;_tmp14=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp0)->f3;_LL16: Cyc_Tovc_box_free_vars_exp(env,_tmp14);_tmp15=_tmp13;goto _LL18;
_LL17: if(*((int*)_tmp0)!= 13)goto _LL19;_tmp15=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp0)->f1;_LL18: _tmp16=_tmp15;goto _LL1A;_LL19: if(*((int*)_tmp0)!= 14)goto _LL1B;
_tmp16=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp0)->f1;_LL1A: _tmp17=_tmp16;
goto _LL1C;_LL1B: if(*((int*)_tmp0)!= 15)goto _LL1D;_tmp17=((struct Cyc_Absyn_Cast_e_struct*)
_tmp0)->f2;_LL1C: _tmp18=_tmp17;goto _LL1E;_LL1D: if(*((int*)_tmp0)!= 16)goto _LL1F;
_tmp18=((struct Cyc_Absyn_Address_e_struct*)_tmp0)->f1;_LL1E: _tmp19=_tmp18;goto
_LL20;_LL1F: if(*((int*)_tmp0)!= 19)goto _LL21;_tmp19=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp0)->f1;_LL20: _tmp1A=_tmp19;goto _LL22;_LL21: if(*((int*)_tmp0)!= 22)goto _LL23;
_tmp1A=((struct Cyc_Absyn_Deref_e_struct*)_tmp0)->f1;_LL22: _tmp1B=_tmp1A;goto
_LL24;_LL23: if(*((int*)_tmp0)!= 23)goto _LL25;_tmp1B=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp0)->f1;_LL24: _tmp1C=_tmp1B;goto _LL26;_LL25: if(*((int*)_tmp0)!= 24)goto _LL27;
_tmp1C=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp0)->f1;_LL26: _tmp1D=_tmp1C;goto
_LL28;_LL27: if(*((int*)_tmp0)!= 5)goto _LL29;_tmp1D=((struct Cyc_Absyn_Increment_e_struct*)
_tmp0)->f1;_LL28: Cyc_Tovc_box_free_vars_exp(env,_tmp1D);return;_LL29: if(*((int*)
_tmp0)!= 18)goto _LL2B;_LL2A: goto _LL2C;_LL2B: if(*((int*)_tmp0)!= 20)goto _LL2D;
_LL2C: goto _LL2E;_LL2D: if(*((int*)_tmp0)!= 21)goto _LL2F;_LL2E: goto _LL30;_LL2F: if(*((
int*)_tmp0)!= 33)goto _LL31;_LL30: return;_LL31: if(*((int*)_tmp0)!= 36)goto _LL33;
_tmp1E=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp0)->f2;_LL32: for(0;_tmp1E != 
0;_tmp1E=_tmp1E->tl){Cyc_Tovc_box_free_vars_exp(env,(*((struct _tuple4*)_tmp1E->hd)).f2);}
return;_LL33:;_LL34:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp26=
_cycalloc(sizeof(*_tmp26));_tmp26[0]=({struct Cyc_Core_Impossible_struct _tmp27;
_tmp27.tag=Cyc_Core_Impossible;_tmp27.f1=({const char*_tmp28="bad exp form in Tocv::box_free_vars_exp";
_tag_dynforward(_tmp28,sizeof(char),_get_zero_arr_size(_tmp28,40));});_tmp27;});
_tmp26;}));_LL0:;}}static void Cyc_Tovc_box_free_vars_stmt(struct Cyc_Tovc_BoxingEnv
env,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp29=(void*)s->r;struct Cyc_Absyn_Stmt*
_tmp2A;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Exp*_tmp2C;struct Cyc_Absyn_Stmt*
_tmp2D;struct Cyc_Absyn_Stmt*_tmp2E;struct Cyc_Absyn_Exp*_tmp2F;struct Cyc_Absyn_Stmt*
_tmp30;struct Cyc_Absyn_Stmt*_tmp31;struct _tuple3 _tmp32;struct Cyc_Absyn_Exp*
_tmp33;struct Cyc_Absyn_Stmt*_tmp34;struct Cyc_Absyn_Exp*_tmp35;struct _tuple3
_tmp36;struct Cyc_Absyn_Exp*_tmp37;struct _tuple3 _tmp38;struct Cyc_Absyn_Exp*_tmp39;
struct Cyc_Absyn_Stmt*_tmp3A;struct Cyc_Absyn_Stmt*_tmp3B;struct _tuple3 _tmp3C;
struct Cyc_Absyn_Exp*_tmp3D;struct Cyc_Absyn_Exp*_tmp3E;struct Cyc_List_List*_tmp3F;
struct Cyc_Absyn_Decl*_tmp40;struct Cyc_Absyn_Stmt*_tmp41;_LL3B: if((int)_tmp29 != 0)
goto _LL3D;_LL3C: goto _LL3E;_LL3D: if(_tmp29 <= (void*)1)goto _LL57;if(*((int*)_tmp29)
!= 5)goto _LL3F;_LL3E: goto _LL40;_LL3F: if(*((int*)_tmp29)!= 6)goto _LL41;_LL40:
return;_LL41: if(*((int*)_tmp29)!= 7)goto _LL43;_LL42: return;_LL43: if(*((int*)
_tmp29)!= 12)goto _LL45;_tmp2A=((struct Cyc_Absyn_Label_s_struct*)_tmp29)->f2;
_LL44: s=_tmp2A;continue;_LL45: if(*((int*)_tmp29)!= 2)goto _LL47;_tmp2B=((struct
Cyc_Absyn_Return_s_struct*)_tmp29)->f1;_LL46: if(_tmp2B == 0)return;_tmp2C=(struct
Cyc_Absyn_Exp*)_tmp2B;goto _LL48;_LL47: if(*((int*)_tmp29)!= 0)goto _LL49;_tmp2C=((
struct Cyc_Absyn_Exp_s_struct*)_tmp29)->f1;_LL48: Cyc_Tovc_box_free_vars_exp(env,
_tmp2C);return;_LL49: if(*((int*)_tmp29)!= 1)goto _LL4B;_tmp2D=((struct Cyc_Absyn_Seq_s_struct*)
_tmp29)->f1;_tmp2E=((struct Cyc_Absyn_Seq_s_struct*)_tmp29)->f2;_LL4A: Cyc_Tovc_box_free_vars_stmt(
env,_tmp2D);s=_tmp2E;continue;_LL4B: if(*((int*)_tmp29)!= 3)goto _LL4D;_tmp2F=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp29)->f1;_tmp30=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp29)->f2;_tmp31=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp29)->f3;_LL4C: Cyc_Tovc_box_free_vars_exp(
env,_tmp2F);Cyc_Tovc_box_free_vars_stmt(env,_tmp30);s=_tmp31;continue;_LL4D: if(*((
int*)_tmp29)!= 4)goto _LL4F;_tmp32=((struct Cyc_Absyn_While_s_struct*)_tmp29)->f1;
_tmp33=_tmp32.f1;_tmp34=((struct Cyc_Absyn_While_s_struct*)_tmp29)->f2;_LL4E: Cyc_Tovc_box_free_vars_exp(
env,_tmp33);s=_tmp34;continue;_LL4F: if(*((int*)_tmp29)!= 8)goto _LL51;_tmp35=((
struct Cyc_Absyn_For_s_struct*)_tmp29)->f1;_tmp36=((struct Cyc_Absyn_For_s_struct*)
_tmp29)->f2;_tmp37=_tmp36.f1;_tmp38=((struct Cyc_Absyn_For_s_struct*)_tmp29)->f3;
_tmp39=_tmp38.f1;_tmp3A=((struct Cyc_Absyn_For_s_struct*)_tmp29)->f4;_LL50: Cyc_Tovc_box_free_vars_exp(
env,_tmp35);Cyc_Tovc_box_free_vars_exp(env,_tmp37);Cyc_Tovc_box_free_vars_exp(
env,_tmp39);s=_tmp3A;continue;_LL51: if(*((int*)_tmp29)!= 13)goto _LL53;_tmp3B=((
struct Cyc_Absyn_Do_s_struct*)_tmp29)->f1;_tmp3C=((struct Cyc_Absyn_Do_s_struct*)
_tmp29)->f2;_tmp3D=_tmp3C.f1;_LL52: Cyc_Tovc_box_free_vars_exp(env,_tmp3D);s=
_tmp3B;continue;_LL53: if(*((int*)_tmp29)!= 9)goto _LL55;_tmp3E=((struct Cyc_Absyn_Switch_s_struct*)
_tmp29)->f1;_tmp3F=((struct Cyc_Absyn_Switch_s_struct*)_tmp29)->f2;_LL54: Cyc_Tovc_box_free_vars_exp(
env,_tmp3E);for(0;_tmp3F != 0;_tmp3F=_tmp3F->tl){Cyc_Tovc_box_free_vars_stmt(env,((
struct Cyc_Absyn_Switch_clause*)_tmp3F->hd)->body);}return;_LL55: if(*((int*)
_tmp29)!= 11)goto _LL57;_tmp40=((struct Cyc_Absyn_Decl_s_struct*)_tmp29)->f1;
_tmp41=((struct Cyc_Absyn_Decl_s_struct*)_tmp29)->f2;_LL56:{void*_tmp42=(void*)
_tmp40->r;struct Cyc_Absyn_Vardecl*_tmp43;_LL5A: if(_tmp42 <= (void*)2)goto _LL5C;
if(*((int*)_tmp42)!= 0)goto _LL5C;_tmp43=((struct Cyc_Absyn_Var_d_struct*)_tmp42)->f1;
_LL5B: env.boundvars=({struct Cyc_List_List*_tmp44=_region_malloc(env.rgn,sizeof(*
_tmp44));_tmp44->hd=_tmp43;_tmp44->tl=env.boundvars;_tmp44;});env.varmap=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _dynforward_ptr*))Cyc_Dict_rdelete_same)(
env.varmap,(*_tmp43->name).f2);if(_tmp43->initializer != 0)Cyc_Tovc_box_free_vars_exp(
env,(struct Cyc_Absyn_Exp*)_check_null(_tmp43->initializer));s=_tmp41;continue;
_LL5C:;_LL5D: goto _LL59;_LL59:;}goto _LL58;_LL57:;_LL58:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45[0]=({
struct Cyc_Core_Impossible_struct _tmp46;_tmp46.tag=Cyc_Core_Impossible;_tmp46.f1=({
const char*_tmp47="bad stmt after xlation to C";_tag_dynforward(_tmp47,sizeof(
char),_get_zero_arr_size(_tmp47,28));});_tmp46;});_tmp45;}));_LL3A:;}}struct
_tuple5{struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct
Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_fun(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*
s,void*rettype){struct Cyc_Set_Set*freevars=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))
Cyc_Set_rempty)(env.rgn,(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))
Cyc_Core_ptrcmp);struct Cyc_Tovc_BoxingEnv _tmp48=({struct Cyc_Tovc_BoxingEnv _tmp59;
_tmp59.rgn=env.rgn;_tmp59.all_locals=env.all_locals;_tmp59.varmap=env.varmap;
_tmp59.boundvars=0;_tmp59.freevars=& freevars;_tmp59;});Cyc_Tovc_box_free_vars_stmt(
_tmp48,s);{struct Cyc_List_List*params=0;struct Cyc_List_List*args=0;struct Cyc_Iter_Iter
_tmp49=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(
Cyc_Core_heap_region,freevars);struct Cyc_Absyn_Vardecl*_tmp4A=({struct Cyc_Absyn_Vardecl*
_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->sc=(void*)((void*)2);_tmp58->name=Cyc_Absyn_exn_name;
_tmp58->tq=Cyc_Absyn_empty_tqual(0);_tmp58->type=(void*)((void*)0);_tmp58->initializer=
0;_tmp58->rgn=0;_tmp58->attributes=0;_tmp58->escapes=0;_tmp58;});while(((int(*)(
struct Cyc_Iter_Iter,struct Cyc_Absyn_Vardecl**))Cyc_Iter_next)(_tmp49,& _tmp4A)){
void*_tmp4B=(void*)_tmp4A->type;struct Cyc_Absyn_ArrayInfo _tmp4C;void*_tmp4D;
_LL5F: if(_tmp4B <= (void*)4)goto _LL61;if(*((int*)_tmp4B)!= 7)goto _LL61;_tmp4C=((
struct Cyc_Absyn_ArrayType_struct*)_tmp4B)->f1;_tmp4D=(void*)_tmp4C.elt_type;
_LL60: params=({struct Cyc_List_List*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->hd=({
struct _tuple5*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->f1=(*_tmp4A->name).f2;
_tmp4F->f2=_tmp4A->tq;_tmp4F->f3=Cyc_Absyn_cstar_typ(_tmp4D,Cyc_Absyn_empty_tqual(
0));_tmp4F;});_tmp4E->tl=params;_tmp4E;});args=({struct Cyc_List_List*_tmp50=
_cycalloc(sizeof(*_tmp50));_tmp50->hd=Cyc_Absyn_var_exp(_tmp4A->name,0);_tmp50->tl=
args;_tmp50;});goto _LL5E;_LL61:;_LL62: params=({struct Cyc_List_List*_tmp51=
_cycalloc(sizeof(*_tmp51));_tmp51->hd=({struct _tuple5*_tmp52=_cycalloc(sizeof(*
_tmp52));_tmp52->f1=(*_tmp4A->name).f2;_tmp52->f2=_tmp4A->tq;_tmp52->f3=Cyc_Absyn_cstar_typ((
void*)_tmp4A->type,Cyc_Absyn_empty_tqual(0));_tmp52;});_tmp51->tl=params;_tmp51;});
args=({struct Cyc_List_List*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->hd=Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(_tmp4A->name,0),0);_tmp53->tl=args;_tmp53;});goto _LL5E;_LL5E:;}{
struct _tuple1*funname=Cyc_Toc_temp_var();*env.gen_ds=({struct Cyc_List_List*
_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->hd=Cyc_Absyn_new_decl((void*)({struct
Cyc_Absyn_Fn_d_struct*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp56;_tmp56.tag=1;_tmp56.f1=({struct Cyc_Absyn_Fndecl*_tmp57=_cycalloc(sizeof(*
_tmp57));_tmp57->sc=(void*)((void*)0);_tmp57->is_inline=1;_tmp57->name=funname;
_tmp57->tvs=0;_tmp57->effect=0;_tmp57->ret_type=(void*)rettype;_tmp57->args=
params;_tmp57->c_varargs=0;_tmp57->cyc_varargs=0;_tmp57->rgn_po=0;_tmp57->body=s;
_tmp57->cached_typ=0;_tmp57->param_vardecls=0;_tmp57->fn_vardecl=0;_tmp57->attributes=
0;_tmp57;});_tmp56;});_tmp55;}),0);_tmp54->tl=*env.gen_ds;_tmp54;});return Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp(funname,0),args,0);}}}static void Cyc_Tovc_apply_varmap(struct
Cyc_Dict_Dict varmap,struct Cyc_Absyn_Exp*e){void*_tmp5A=(void*)e->r;struct _tuple1*
_tmp5B;struct _tuple1 _tmp5C;union Cyc_Absyn_Nmspace_union _tmp5D;struct
_dynforward_ptr*_tmp5E;struct Cyc_Absyn_Exp*_tmp5F;struct Cyc_List_List*_tmp60;
struct Cyc_List_List*_tmp61;struct Cyc_Absyn_Exp*_tmp62;struct Cyc_Absyn_Exp*_tmp63;
struct Cyc_Absyn_Exp*_tmp64;struct Cyc_Absyn_Exp*_tmp65;struct Cyc_Absyn_Exp*_tmp66;
struct Cyc_Absyn_Exp*_tmp67;struct Cyc_Absyn_Exp*_tmp68;struct Cyc_Absyn_Exp*_tmp69;
struct Cyc_Absyn_Exp*_tmp6A;struct Cyc_Absyn_Exp*_tmp6B;struct Cyc_Absyn_Exp*_tmp6C;
struct Cyc_Absyn_Exp*_tmp6D;struct Cyc_Absyn_Exp*_tmp6E;struct Cyc_Absyn_Exp*_tmp6F;
struct Cyc_Absyn_Exp*_tmp70;struct Cyc_Absyn_Exp*_tmp71;struct Cyc_Absyn_Exp*_tmp72;
struct Cyc_Absyn_Exp*_tmp73;struct Cyc_Absyn_Exp*_tmp74;struct Cyc_Absyn_Exp*_tmp75;
struct Cyc_Absyn_Exp*_tmp76;struct Cyc_Absyn_Exp*_tmp77;struct Cyc_List_List*_tmp78;
_LL64: if(*((int*)_tmp5A)!= 1)goto _LL66;_tmp5B=((struct Cyc_Absyn_Var_e_struct*)
_tmp5A)->f1;_tmp5C=*_tmp5B;_tmp5D=_tmp5C.f1;if((((*((struct Cyc_Absyn_Var_e_struct*)
_tmp5A)->f1).f1).Loc_n).tag != 0)goto _LL66;_tmp5E=_tmp5C.f2;_LL65: if(((int(*)(
struct Cyc_Dict_Dict d,struct _dynforward_ptr*k))Cyc_Dict_member)(varmap,_tmp5E))(
void*)(e->r=(void*)((void*)(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,
struct _dynforward_ptr*k))Cyc_Dict_lookup)(varmap,_tmp5E))->r));return;_LL66: if(*((
int*)_tmp5A)!= 1)goto _LL68;_LL67: return;_LL68: if(*((int*)_tmp5A)!= 0)goto _LL6A;
_LL69: return;_LL6A: if(*((int*)_tmp5A)!= 11)goto _LL6C;_tmp5F=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp5A)->f1;_tmp60=((struct Cyc_Absyn_FnCall_e_struct*)_tmp5A)->f2;_LL6B: Cyc_Tovc_apply_varmap(
varmap,_tmp5F);_tmp61=_tmp60;goto _LL6D;_LL6C: if(*((int*)_tmp5A)!= 3)goto _LL6E;
_tmp61=((struct Cyc_Absyn_Primop_e_struct*)_tmp5A)->f2;_LL6D: for(0;_tmp61 != 0;
_tmp61=_tmp61->tl){Cyc_Tovc_apply_varmap(varmap,(struct Cyc_Absyn_Exp*)_tmp61->hd);}
return;_LL6E: if(*((int*)_tmp5A)!= 6)goto _LL70;_tmp62=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp5A)->f1;_tmp63=((struct Cyc_Absyn_Conditional_e_struct*)_tmp5A)->f2;_tmp64=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp5A)->f3;_LL6F: Cyc_Tovc_apply_varmap(
varmap,_tmp64);_tmp65=_tmp62;_tmp66=_tmp63;goto _LL71;_LL70: if(*((int*)_tmp5A)!= 
7)goto _LL72;_tmp65=((struct Cyc_Absyn_And_e_struct*)_tmp5A)->f1;_tmp66=((struct
Cyc_Absyn_And_e_struct*)_tmp5A)->f2;_LL71: _tmp67=_tmp65;_tmp68=_tmp66;goto _LL73;
_LL72: if(*((int*)_tmp5A)!= 8)goto _LL74;_tmp67=((struct Cyc_Absyn_Or_e_struct*)
_tmp5A)->f1;_tmp68=((struct Cyc_Absyn_Or_e_struct*)_tmp5A)->f2;_LL73: _tmp69=
_tmp67;_tmp6A=_tmp68;goto _LL75;_LL74: if(*((int*)_tmp5A)!= 25)goto _LL76;_tmp69=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp5A)->f1;_tmp6A=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp5A)->f2;_LL75: _tmp6B=_tmp69;_tmp6C=_tmp6A;goto _LL77;_LL76: if(*((int*)_tmp5A)
!= 9)goto _LL78;_tmp6B=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp5A)->f1;_tmp6C=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp5A)->f2;_LL77: _tmp6D=_tmp6B;_tmp6E=_tmp6C;
goto _LL79;_LL78: if(*((int*)_tmp5A)!= 4)goto _LL7A;_tmp6D=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp5A)->f1;_tmp6E=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp5A)->f3;_LL79: Cyc_Tovc_apply_varmap(
varmap,_tmp6E);_tmp6F=_tmp6D;goto _LL7B;_LL7A: if(*((int*)_tmp5A)!= 13)goto _LL7C;
_tmp6F=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp5A)->f1;_LL7B: _tmp70=_tmp6F;
goto _LL7D;_LL7C: if(*((int*)_tmp5A)!= 14)goto _LL7E;_tmp70=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp5A)->f1;_LL7D: _tmp71=_tmp70;goto _LL7F;_LL7E: if(*((int*)_tmp5A)!= 15)goto
_LL80;_tmp71=((struct Cyc_Absyn_Cast_e_struct*)_tmp5A)->f2;_LL7F: _tmp72=_tmp71;
goto _LL81;_LL80: if(*((int*)_tmp5A)!= 16)goto _LL82;_tmp72=((struct Cyc_Absyn_Address_e_struct*)
_tmp5A)->f1;_LL81: _tmp73=_tmp72;goto _LL83;_LL82: if(*((int*)_tmp5A)!= 19)goto
_LL84;_tmp73=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp5A)->f1;_LL83: _tmp74=
_tmp73;goto _LL85;_LL84: if(*((int*)_tmp5A)!= 22)goto _LL86;_tmp74=((struct Cyc_Absyn_Deref_e_struct*)
_tmp5A)->f1;_LL85: _tmp75=_tmp74;goto _LL87;_LL86: if(*((int*)_tmp5A)!= 23)goto
_LL88;_tmp75=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp5A)->f1;_LL87: _tmp76=
_tmp75;goto _LL89;_LL88: if(*((int*)_tmp5A)!= 24)goto _LL8A;_tmp76=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp5A)->f1;_LL89: _tmp77=_tmp76;goto _LL8B;_LL8A: if(*((int*)_tmp5A)!= 5)goto _LL8C;
_tmp77=((struct Cyc_Absyn_Increment_e_struct*)_tmp5A)->f1;_LL8B: Cyc_Tovc_apply_varmap(
varmap,_tmp77);return;_LL8C: if(*((int*)_tmp5A)!= 18)goto _LL8E;_LL8D: goto _LL8F;
_LL8E: if(*((int*)_tmp5A)!= 20)goto _LL90;_LL8F: goto _LL91;_LL90: if(*((int*)_tmp5A)
!= 21)goto _LL92;_LL91: goto _LL93;_LL92: if(*((int*)_tmp5A)!= 33)goto _LL94;_LL93:
return;_LL94: if(*((int*)_tmp5A)!= 36)goto _LL96;_tmp78=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp5A)->f2;_LL95: for(0;_tmp78 != 0;_tmp78=_tmp78->tl){Cyc_Tovc_apply_varmap(
varmap,(*((struct _tuple4*)_tmp78->hd)).f2);}return;_LL96:;_LL97:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp79=_cycalloc(sizeof(*_tmp79));
_tmp79[0]=({struct Cyc_Core_Impossible_struct _tmp7A;_tmp7A.tag=Cyc_Core_Impossible;
_tmp7A.f1=({const char*_tmp7B="bad exp form in Tocv::apply_varmap";
_tag_dynforward(_tmp7B,sizeof(char),_get_zero_arr_size(_tmp7B,35));});_tmp7A;});
_tmp79;}));_LL63:;}static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_exp(struct Cyc_Tovc_ToExpEnv
env,struct Cyc_Absyn_Stmt*s){void*_tmp7C=(void*)s->r;struct Cyc_Absyn_Exp*_tmp7D;
struct Cyc_Absyn_Stmt*_tmp7E;struct Cyc_Absyn_Stmt*_tmp7F;struct Cyc_Absyn_Exp*
_tmp80;struct Cyc_Absyn_Stmt*_tmp81;struct Cyc_Absyn_Stmt*_tmp82;struct Cyc_Absyn_Decl*
_tmp83;struct Cyc_Absyn_Stmt*_tmp84;_LL99: if((int)_tmp7C != 0)goto _LL9B;_LL9A:
return Cyc_Absyn_true_exp(0);_LL9B: if(_tmp7C <= (void*)1)goto _LLA3;if(*((int*)
_tmp7C)!= 0)goto _LL9D;_tmp7D=((struct Cyc_Absyn_Exp_s_struct*)_tmp7C)->f1;_LL9C:
Cyc_Tovc_apply_varmap(env.varmap,_tmp7D);return _tmp7D;_LL9D: if(*((int*)_tmp7C)!= 
1)goto _LL9F;_tmp7E=((struct Cyc_Absyn_Seq_s_struct*)_tmp7C)->f1;_tmp7F=((struct
Cyc_Absyn_Seq_s_struct*)_tmp7C)->f2;_LL9E: return Cyc_Absyn_seq_exp(Cyc_Tovc_stmt_to_exp(
env,_tmp7E),Cyc_Tovc_stmt_to_exp(env,_tmp7F),0);_LL9F: if(*((int*)_tmp7C)!= 3)
goto _LLA1;_tmp80=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp7C)->f1;_tmp81=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp7C)->f2;_tmp82=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp7C)->f3;_LLA0: Cyc_Tovc_apply_varmap(env.varmap,_tmp80);return Cyc_Absyn_conditional_exp(
_tmp80,Cyc_Tovc_stmt_to_exp(env,_tmp81),Cyc_Tovc_stmt_to_exp(env,_tmp82),0);
_LLA1: if(*((int*)_tmp7C)!= 11)goto _LLA3;_tmp83=((struct Cyc_Absyn_Decl_s_struct*)
_tmp7C)->f1;_tmp84=((struct Cyc_Absyn_Decl_s_struct*)_tmp7C)->f2;_LLA2: {void*
_tmp85=(void*)_tmp83->r;struct Cyc_Absyn_Vardecl*_tmp86;_LLA6: if(_tmp85 <= (void*)
2)goto _LLA8;if(*((int*)_tmp85)!= 0)goto _LLA8;_tmp86=((struct Cyc_Absyn_Var_d_struct*)
_tmp85)->f1;_LLA7: {struct _dynforward_ptr*_tmp87=(*_tmp86->name).f2;struct
_tuple1*_tmp88=Cyc_Toc_temp_var();_tmp86->name=_tmp88;env.varmap=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dynforward_ptr*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(
env.varmap,_tmp87,Cyc_Absyn_var_exp(_tmp88,0));if(_tmp86->initializer != 0){Cyc_Tovc_apply_varmap(
env.varmap,(struct Cyc_Absyn_Exp*)_check_null(_tmp86->initializer));{void*_tmp89=(
void*)((struct Cyc_Absyn_Exp*)_check_null(_tmp86->initializer))->r;_LLAB: if(*((
int*)_tmp89)!= 36)goto _LLAD;_LLAC: {struct _tuple1*tmp=Cyc_Toc_temp_var();struct
Cyc_Absyn_Stmt*body=Cyc_Absyn_declare_stmt(tmp,(void*)_tmp86->type,_tmp86->initializer,
Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(tmp,0),0),0);
_tmp86->initializer=(struct Cyc_Absyn_Exp*)Cyc_Tovc_stmt_to_fun(env,body,(void*)
_tmp86->type);goto _LLAA;}_LLAD:;_LLAE: goto _LLAA;_LLAA:;}}env.all_locals=({struct
Cyc_List_List*_tmp8A=_region_malloc(env.rgn,sizeof(*_tmp8A));_tmp8A->hd=_tmp86;
_tmp8A->tl=env.all_locals;_tmp8A;});{struct Cyc_Absyn_Exp*_tmp8B=_tmp86->initializer;
_tmp86->initializer=0;(void*)((env.encloser)->r=(void*)((void*)(Cyc_Absyn_decl_stmt(
_tmp83,Cyc_Absyn_new_stmt((void*)(env.encloser)->r,0),0))->r));{struct Cyc_Absyn_Exp*
_tmp8C=Cyc_Tovc_stmt_to_exp(env,_tmp84);if(_tmp8B != 0)_tmp8C=Cyc_Absyn_seq_exp(
Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp86->name,0),(struct Cyc_Absyn_Exp*)
_tmp8B,0),_tmp8C,0);return _tmp8C;}}}_LLA8:;_LLA9:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_Core_Impossible_struct
_tmp8E;_tmp8E.tag=Cyc_Core_Impossible;_tmp8E.f1=({const char*_tmp8F="bad local decl in Tovc::stmt_to_exp";
_tag_dynforward(_tmp8F,sizeof(char),_get_zero_arr_size(_tmp8F,36));});_tmp8E;});
_tmp8D;}));_LLA5:;}_LLA3:;_LLA4: return Cyc_Tovc_stmt_to_fun(env,s,(void*)0);_LL98:;}
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s);
static void Cyc_Tovc_exp_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Exp*e){
void*_tmp90=(void*)e->r;struct Cyc_Absyn_Exp*_tmp91;struct Cyc_List_List*_tmp92;
struct Cyc_List_List*_tmp93;struct Cyc_Absyn_Exp*_tmp94;struct Cyc_Absyn_Exp*_tmp95;
struct Cyc_Absyn_Exp*_tmp96;struct Cyc_Absyn_Exp*_tmp97;struct Cyc_Absyn_Exp*_tmp98;
struct Cyc_Absyn_Exp*_tmp99;struct Cyc_Absyn_Exp*_tmp9A;struct Cyc_Absyn_Exp*_tmp9B;
struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_Absyn_Exp*_tmp9E;
struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Absyn_Exp*_tmpA0;struct Cyc_Absyn_Exp*_tmpA1;
struct Cyc_Absyn_Exp*_tmpA2;struct Cyc_Absyn_Exp*_tmpA3;struct Cyc_Absyn_Exp*_tmpA4;
struct Cyc_Absyn_Exp*_tmpA5;struct Cyc_Absyn_Exp*_tmpA6;struct Cyc_Absyn_Exp*_tmpA7;
struct Cyc_Absyn_Exp*_tmpA8;struct Cyc_Absyn_Exp*_tmpA9;struct Cyc_List_List*_tmpAA;
struct Cyc_Absyn_Stmt*_tmpAB;_LLB0: if(*((int*)_tmp90)!= 0)goto _LLB2;_LLB1: return;
_LLB2: if(*((int*)_tmp90)!= 1)goto _LLB4;_LLB3: return;_LLB4: if(*((int*)_tmp90)!= 
11)goto _LLB6;_tmp91=((struct Cyc_Absyn_FnCall_e_struct*)_tmp90)->f1;_tmp92=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp90)->f2;_LLB5: Cyc_Tovc_exp_to_vc(env,_tmp91);
_tmp93=_tmp92;goto _LLB7;_LLB6: if(*((int*)_tmp90)!= 3)goto _LLB8;_tmp93=((struct
Cyc_Absyn_Primop_e_struct*)_tmp90)->f2;_LLB7: for(0;_tmp93 != 0;_tmp93=_tmp93->tl){
Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)_tmp93->hd);}return;_LLB8: if(*((int*)
_tmp90)!= 6)goto _LLBA;_tmp94=((struct Cyc_Absyn_Conditional_e_struct*)_tmp90)->f1;
_tmp95=((struct Cyc_Absyn_Conditional_e_struct*)_tmp90)->f2;_tmp96=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp90)->f3;_LLB9: Cyc_Tovc_exp_to_vc(env,_tmp96);_tmp97=_tmp94;_tmp98=_tmp95;
goto _LLBB;_LLBA: if(*((int*)_tmp90)!= 7)goto _LLBC;_tmp97=((struct Cyc_Absyn_And_e_struct*)
_tmp90)->f1;_tmp98=((struct Cyc_Absyn_And_e_struct*)_tmp90)->f2;_LLBB: _tmp99=
_tmp97;_tmp9A=_tmp98;goto _LLBD;_LLBC: if(*((int*)_tmp90)!= 8)goto _LLBE;_tmp99=((
struct Cyc_Absyn_Or_e_struct*)_tmp90)->f1;_tmp9A=((struct Cyc_Absyn_Or_e_struct*)
_tmp90)->f2;_LLBD: _tmp9B=_tmp99;_tmp9C=_tmp9A;goto _LLBF;_LLBE: if(*((int*)_tmp90)
!= 25)goto _LLC0;_tmp9B=((struct Cyc_Absyn_Subscript_e_struct*)_tmp90)->f1;_tmp9C=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp90)->f2;_LLBF: _tmp9D=_tmp9B;_tmp9E=
_tmp9C;goto _LLC1;_LLC0: if(*((int*)_tmp90)!= 9)goto _LLC2;_tmp9D=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp90)->f1;_tmp9E=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp90)->f2;_LLC1: _tmp9F=
_tmp9D;_tmpA0=_tmp9E;goto _LLC3;_LLC2: if(*((int*)_tmp90)!= 4)goto _LLC4;_tmp9F=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp90)->f1;_tmpA0=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp90)->f3;_LLC3: Cyc_Tovc_exp_to_vc(env,_tmpA0);_tmpA1=_tmp9F;goto _LLC5;_LLC4:
if(*((int*)_tmp90)!= 13)goto _LLC6;_tmpA1=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp90)->f1;_LLC5: _tmpA2=_tmpA1;goto _LLC7;_LLC6: if(*((int*)_tmp90)!= 14)goto
_LLC8;_tmpA2=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp90)->f1;_LLC7: _tmpA3=
_tmpA2;goto _LLC9;_LLC8: if(*((int*)_tmp90)!= 15)goto _LLCA;_tmpA3=((struct Cyc_Absyn_Cast_e_struct*)
_tmp90)->f2;_LLC9: _tmpA4=_tmpA3;goto _LLCB;_LLCA: if(*((int*)_tmp90)!= 16)goto
_LLCC;_tmpA4=((struct Cyc_Absyn_Address_e_struct*)_tmp90)->f1;_LLCB: _tmpA5=_tmpA4;
goto _LLCD;_LLCC: if(*((int*)_tmp90)!= 19)goto _LLCE;_tmpA5=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp90)->f1;_LLCD: _tmpA6=_tmpA5;goto _LLCF;_LLCE: if(*((int*)_tmp90)!= 22)goto
_LLD0;_tmpA6=((struct Cyc_Absyn_Deref_e_struct*)_tmp90)->f1;_LLCF: _tmpA7=_tmpA6;
goto _LLD1;_LLD0: if(*((int*)_tmp90)!= 23)goto _LLD2;_tmpA7=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp90)->f1;_LLD1: _tmpA8=_tmpA7;goto _LLD3;_LLD2: if(*((int*)_tmp90)!= 24)goto
_LLD4;_tmpA8=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp90)->f1;_LLD3: _tmpA9=
_tmpA8;goto _LLD5;_LLD4: if(*((int*)_tmp90)!= 5)goto _LLD6;_tmpA9=((struct Cyc_Absyn_Increment_e_struct*)
_tmp90)->f1;_LLD5: Cyc_Tovc_exp_to_vc(env,_tmpA9);return;_LLD6: if(*((int*)_tmp90)
!= 18)goto _LLD8;_LLD7: goto _LLD9;_LLD8: if(*((int*)_tmp90)!= 20)goto _LLDA;_LLD9:
goto _LLDB;_LLDA: if(*((int*)_tmp90)!= 21)goto _LLDC;_LLDB: goto _LLDD;_LLDC: if(*((
int*)_tmp90)!= 33)goto _LLDE;_LLDD: return;_LLDE: if(*((int*)_tmp90)!= 36)goto _LLE0;
_tmpAA=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp90)->f2;_LLDF: for(0;_tmpAA
!= 0;_tmpAA=_tmpAA->tl){Cyc_Tovc_exp_to_vc(env,(*((struct _tuple4*)_tmpAA->hd)).f2);}
return;_LLE0: if(*((int*)_tmp90)!= 37)goto _LLE2;_tmpAB=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp90)->f1;_LLE1: Cyc_Tovc_stmt_to_vc(env,_tmpAB);(void*)(e->r=(void*)((void*)(
Cyc_Tovc_stmt_to_exp(env,_tmpAB))->r));return;_LLE2:;_LLE3:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC[0]=({
struct Cyc_Core_Impossible_struct _tmpAD;_tmpAD.tag=Cyc_Core_Impossible;_tmpAD.f1=({
const char*_tmpAE="bad exp form after xlation to C";_tag_dynforward(_tmpAE,
sizeof(char),_get_zero_arr_size(_tmpAE,32));});_tmpAD;});_tmpAC;}));_LLAF:;}
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
while(1){env.encloser=s;{void*_tmpAF=(void*)s->r;struct Cyc_Absyn_Stmt*_tmpB0;
struct Cyc_Absyn_Exp*_tmpB1;struct Cyc_Absyn_Exp*_tmpB2;struct Cyc_Absyn_Stmt*
_tmpB3;struct Cyc_Absyn_Stmt*_tmpB4;struct Cyc_Absyn_Exp*_tmpB5;struct Cyc_Absyn_Stmt*
_tmpB6;struct Cyc_Absyn_Stmt*_tmpB7;struct _tuple3 _tmpB8;struct Cyc_Absyn_Exp*
_tmpB9;struct Cyc_Absyn_Stmt*_tmpBA;struct Cyc_Absyn_Exp*_tmpBB;struct _tuple3
_tmpBC;struct Cyc_Absyn_Exp*_tmpBD;struct _tuple3 _tmpBE;struct Cyc_Absyn_Exp*_tmpBF;
struct Cyc_Absyn_Stmt*_tmpC0;struct Cyc_Absyn_Stmt*_tmpC1;struct _tuple3 _tmpC2;
struct Cyc_Absyn_Exp*_tmpC3;struct Cyc_Absyn_Exp*_tmpC4;struct Cyc_List_List*_tmpC5;
struct Cyc_Absyn_Decl*_tmpC6;struct Cyc_Absyn_Stmt*_tmpC7;_LLE5: if((int)_tmpAF != 0)
goto _LLE7;_LLE6: goto _LLE8;_LLE7: if(_tmpAF <= (void*)1)goto _LL101;if(*((int*)
_tmpAF)!= 5)goto _LLE9;_LLE8: goto _LLEA;_LLE9: if(*((int*)_tmpAF)!= 6)goto _LLEB;
_LLEA: goto _LLEC;_LLEB: if(*((int*)_tmpAF)!= 7)goto _LLED;_LLEC: return;_LLED: if(*((
int*)_tmpAF)!= 12)goto _LLEF;_tmpB0=((struct Cyc_Absyn_Label_s_struct*)_tmpAF)->f2;
_LLEE: s=_tmpB0;continue;_LLEF: if(*((int*)_tmpAF)!= 2)goto _LLF1;_tmpB1=((struct
Cyc_Absyn_Return_s_struct*)_tmpAF)->f1;_LLF0: if(_tmpB1 == 0)return;_tmpB2=(struct
Cyc_Absyn_Exp*)_tmpB1;goto _LLF2;_LLF1: if(*((int*)_tmpAF)!= 0)goto _LLF3;_tmpB2=((
struct Cyc_Absyn_Exp_s_struct*)_tmpAF)->f1;_LLF2: Cyc_Tovc_exp_to_vc(env,_tmpB2);
return;_LLF3: if(*((int*)_tmpAF)!= 1)goto _LLF5;_tmpB3=((struct Cyc_Absyn_Seq_s_struct*)
_tmpAF)->f1;_tmpB4=((struct Cyc_Absyn_Seq_s_struct*)_tmpAF)->f2;_LLF4: Cyc_Tovc_stmt_to_vc(
env,_tmpB3);s=_tmpB4;continue;_LLF5: if(*((int*)_tmpAF)!= 3)goto _LLF7;_tmpB5=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmpAF)->f1;_tmpB6=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmpAF)->f2;_tmpB7=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmpAF)->f3;_LLF6: Cyc_Tovc_exp_to_vc(
env,_tmpB5);Cyc_Tovc_stmt_to_vc(env,_tmpB6);s=_tmpB7;continue;_LLF7: if(*((int*)
_tmpAF)!= 4)goto _LLF9;_tmpB8=((struct Cyc_Absyn_While_s_struct*)_tmpAF)->f1;
_tmpB9=_tmpB8.f1;_tmpBA=((struct Cyc_Absyn_While_s_struct*)_tmpAF)->f2;_LLF8: Cyc_Tovc_exp_to_vc(
env,_tmpB9);s=_tmpBA;continue;_LLF9: if(*((int*)_tmpAF)!= 8)goto _LLFB;_tmpBB=((
struct Cyc_Absyn_For_s_struct*)_tmpAF)->f1;_tmpBC=((struct Cyc_Absyn_For_s_struct*)
_tmpAF)->f2;_tmpBD=_tmpBC.f1;_tmpBE=((struct Cyc_Absyn_For_s_struct*)_tmpAF)->f3;
_tmpBF=_tmpBE.f1;_tmpC0=((struct Cyc_Absyn_For_s_struct*)_tmpAF)->f4;_LLFA: Cyc_Tovc_exp_to_vc(
env,_tmpBB);Cyc_Tovc_exp_to_vc(env,_tmpBD);Cyc_Tovc_exp_to_vc(env,_tmpBF);s=
_tmpC0;continue;_LLFB: if(*((int*)_tmpAF)!= 13)goto _LLFD;_tmpC1=((struct Cyc_Absyn_Do_s_struct*)
_tmpAF)->f1;_tmpC2=((struct Cyc_Absyn_Do_s_struct*)_tmpAF)->f2;_tmpC3=_tmpC2.f1;
_LLFC: Cyc_Tovc_exp_to_vc(env,_tmpC3);s=_tmpC1;continue;_LLFD: if(*((int*)_tmpAF)
!= 9)goto _LLFF;_tmpC4=((struct Cyc_Absyn_Switch_s_struct*)_tmpAF)->f1;_tmpC5=((
struct Cyc_Absyn_Switch_s_struct*)_tmpAF)->f2;_LLFE: Cyc_Tovc_exp_to_vc(env,_tmpC4);
for(0;_tmpC5 != 0;_tmpC5=_tmpC5->tl){Cyc_Tovc_stmt_to_vc(env,((struct Cyc_Absyn_Switch_clause*)
_tmpC5->hd)->body);}return;_LLFF: if(*((int*)_tmpAF)!= 11)goto _LL101;_tmpC6=((
struct Cyc_Absyn_Decl_s_struct*)_tmpAF)->f1;_tmpC7=((struct Cyc_Absyn_Decl_s_struct*)
_tmpAF)->f2;_LL100:{void*_tmpC8=(void*)_tmpC6->r;struct Cyc_Absyn_Vardecl*_tmpC9;
_LL104: if(_tmpC8 <= (void*)2)goto _LL106;if(*((int*)_tmpC8)!= 0)goto _LL106;_tmpC9=((
struct Cyc_Absyn_Var_d_struct*)_tmpC8)->f1;_LL105:{void*_tmpCA=Cyc_Tcutil_compress((
void*)_tmpC9->type);_LL109: if(_tmpCA <= (void*)4)goto _LL10B;if(*((int*)_tmpCA)!= 
7)goto _LL10B;_LL10A: if(_tmpC9->initializer != 0){void*_tmpCB=(void*)((struct Cyc_Absyn_Exp*)
_check_null(_tmpC9->initializer))->r;struct Cyc_List_List*_tmpCC;_LL10E: if(*((int*)
_tmpCB)!= 36)goto _LL110;_tmpCC=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpCB)->f2;
_LL10F: _tmpC9->initializer=0;{struct Cyc_Absyn_Exp*arr=Cyc_Absyn_var_exp(_tmpC9->name,
0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_new_stmt((void*)_tmpC7->r,0);{unsigned int i=
0;for(0;_tmpCC != 0;(_tmpCC=_tmpCC->tl,++ i)){s3=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(arr,Cyc_Absyn_uint_exp(i,0),0),(*((
struct _tuple4*)_tmpCC->hd)).f2,0),0),s3,0);}}(void*)(_tmpC7->r=(void*)((void*)s3->r));
goto _LL10D;}_LL110:;_LL111:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD[0]=({struct Cyc_Core_Impossible_struct
_tmpCE;_tmpCE.tag=Cyc_Core_Impossible;_tmpCE.f1=({const char*_tmpCF="array type bad initializer";
_tag_dynforward(_tmpCF,sizeof(char),_get_zero_arr_size(_tmpCF,27));});_tmpCE;});
_tmpCD;}));_LL10D:;}goto _LL108;_LL10B:;_LL10C: goto _LL108;_LL108:;}env.all_locals=({
struct Cyc_List_List*_tmpD0=_region_malloc(env.rgn,sizeof(*_tmpD0));_tmpD0->hd=
_tmpC9;_tmpD0->tl=env.all_locals;_tmpD0;});if(_tmpC9->initializer != 0)Cyc_Tovc_exp_to_vc(
env,(struct Cyc_Absyn_Exp*)_check_null(_tmpC9->initializer));s=_tmpC7;continue;
_LL106:;_LL107: goto _LL103;_LL103:;}goto _LL102;_LL101:;_LL102:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1[0]=({
struct Cyc_Core_Impossible_struct _tmpD2;_tmpD2.tag=Cyc_Core_Impossible;_tmpD2.f1=({
const char*_tmpD3="bad stmt after xlation to C";_tag_dynforward(_tmpD3,sizeof(
char),_get_zero_arr_size(_tmpD3,28));});_tmpD2;});_tmpD1;}));_LLE4:;}}}struct Cyc_List_List*
Cyc_Tovc_tovc(struct Cyc_List_List*old_ds){struct Cyc_List_List*_tmpD4=0;for(0;
old_ds != 0;old_ds=old_ds->tl){struct Cyc_Absyn_Decl*_tmpD5=(struct Cyc_Absyn_Decl*)
old_ds->hd;void*_tmpD6=(void*)_tmpD5->r;struct Cyc_Absyn_Fndecl*_tmpD7;struct Cyc_List_List*
_tmpD8;struct Cyc_List_List*_tmpD9;struct Cyc_List_List*_tmpDA;struct Cyc_List_List*
_tmpDB;_LL113: if(_tmpD6 <= (void*)2)goto _LL12B;if(*((int*)_tmpD6)!= 1)goto _LL115;
_tmpD7=((struct Cyc_Absyn_Fn_d_struct*)_tmpD6)->f1;_LL114:{struct _RegionHandle
_tmpDC=_new_region("rgn");struct _RegionHandle*rgn=& _tmpDC;_push_region(rgn);{
struct Cyc_List_List**_tmpDD=({struct Cyc_List_List**_tmpDF=_region_malloc(rgn,
sizeof(*_tmpDF));_tmpDF[0]=0;_tmpDF;});Cyc_Tovc_stmt_to_vc(({struct Cyc_Tovc_ToExpEnv
_tmpDE;_tmpDE.rgn=rgn;_tmpDE.all_locals=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(rgn,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmpD7->param_vardecls))->v);_tmpDE.varmap=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,
struct _dynforward_ptr*)))Cyc_Dict_rempty)(rgn,Cyc_strptrcmp);_tmpDE.encloser=
_tmpD7->body;_tmpDE.gen_ds=_tmpDD;_tmpDE;}),_tmpD7->body);_tmpD4=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(*_tmpDD,_tmpD4);};
_pop_region(rgn);}goto _LL116;_LL115: if(*((int*)_tmpD6)!= 0)goto _LL117;_LL116:
goto _LL118;_LL117: if(*((int*)_tmpD6)!= 2)goto _LL119;_LL118: goto _LL11A;_LL119: if(*((
int*)_tmpD6)!= 3)goto _LL11B;_LL11A: goto _LL11C;_LL11B: if(*((int*)_tmpD6)!= 4)goto
_LL11D;_LL11C: goto _LL11E;_LL11D: if(*((int*)_tmpD6)!= 5)goto _LL11F;_LL11E: goto
_LL120;_LL11F: if(*((int*)_tmpD6)!= 6)goto _LL121;_LL120: goto _LL122;_LL121: if(*((
int*)_tmpD6)!= 7)goto _LL123;_LL122: _tmpD4=({struct Cyc_List_List*_tmpE0=_cycalloc(
sizeof(*_tmpE0));_tmpE0->hd=_tmpD5;_tmpE0->tl=_tmpD4;_tmpE0;});goto _LL112;_LL123:
if(*((int*)_tmpD6)!= 8)goto _LL125;_tmpD8=((struct Cyc_Absyn_Namespace_d_struct*)
_tmpD6)->f2;_LL124: _tmpD9=_tmpD8;goto _LL126;_LL125: if(*((int*)_tmpD6)!= 9)goto
_LL127;_tmpD9=((struct Cyc_Absyn_Using_d_struct*)_tmpD6)->f2;_LL126: _tmpDA=_tmpD9;
goto _LL128;_LL127: if(*((int*)_tmpD6)!= 10)goto _LL129;_tmpDA=((struct Cyc_Absyn_ExternC_d_struct*)
_tmpD6)->f1;_LL128: _tmpDB=_tmpDA;goto _LL12A;_LL129: if(*((int*)_tmpD6)!= 11)goto
_LL12B;_tmpDB=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmpD6)->f1;_LL12A: goto
_LL12C;_LL12B: if((int)_tmpD6 != 0)goto _LL12D;_LL12C: goto _LL12E;_LL12D: if((int)
_tmpD6 != 1)goto _LL112;_LL12E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1[0]=({struct Cyc_Core_Impossible_struct
_tmpE2;_tmpE2.tag=Cyc_Core_Impossible;_tmpE2.f1=({const char*_tmpE3="nested translation unit after translation to C";
_tag_dynforward(_tmpE3,sizeof(char),_get_zero_arr_size(_tmpE3,47));});_tmpE2;});
_tmpE1;}));_LL112:;}return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpD4);}
