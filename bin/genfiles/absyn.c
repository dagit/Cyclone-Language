#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator.  The corresponding definitions are in file
   lib/runtime_cyc.c
*/
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

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
  char data[1];  /*FJS: used to be size 0, but that's forbidden in ansi c*/
};

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#endif
};

struct _DynRegionFrame {
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
};

extern struct _RegionHandle _new_region(const char *);
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void   _free_region(struct _RegionHandle *);
extern void   _reset_region(struct _RegionHandle *);
extern struct _RegionHandle *_open_dynregion(struct _DynRegionFrame *f,
                                             struct _DynRegionHandle *h);
extern void   _pop_dynregion();

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
extern int _throw_match();
extern int _throw(void* e);
#endif

extern struct _xtunion_struct *_exn_thrown;

/* Built-in Exceptions */
struct Cyc_Null_Exception_exn_struct { char *tag; };
struct Cyc_Array_bounds_exn_struct { char *tag; };
struct Cyc_Match_Exception_exn_struct { char *tag; };
struct Cyc_Bad_alloc_exn_struct { char *tag; };
extern char Cyc_Null_Exception[];
extern char Cyc_Array_bounds[];
extern char Cyc_Match_Exception[];
extern char Cyc_Bad_alloc[];

/* Built-in Run-time Checks and company */
#ifdef __APPLE__
#define _INLINE_FUNCTIONS
#endif

#ifdef CYC_ANSI_OUTPUT
#define _INLINE  
#define _INLINE_FUNCTIONS
#else
#define _INLINE inline
#endif

#ifdef VC_C
#define _CYC_U_LONG_LONG_T __int64
#else
#ifdef GCC_C
#define _CYC_U_LONG_LONG_T unsigned long long
#else
#define _CYC_U_LONG_LONG_T unsigned long long
#endif
#endif

#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE void *
_check_null(const void *ptr) {
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
static _INLINE char *
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
static _INLINE unsigned
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
#define _zero_arr_plus_char(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_short(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_int(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_float(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_double(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_longdouble(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_voidstar(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#else
static _INLINE char *
_zero_arr_plus_char(char *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE short *
_zero_arr_plus_short(short *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE int *
_zero_arr_plus_int(int *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE float *
_zero_arr_plus_float(float *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE double *
_zero_arr_plus_double(double *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE long double *
_zero_arr_plus_longdouble(long double *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE void *
_zero_arr_plus_voidstar(void **orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
#endif


/* Calculates the number of elements in a zero-terminated, thin array.
   If non-null, the array is guaranteed to have orig_offset elements. */
static _INLINE int
_get_zero_arr_size_char(const char *orig_x, unsigned int orig_offset) {
  const char *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_short(const short *orig_x, unsigned int orig_offset) {
  const short *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_int(const int *orig_x, unsigned int orig_offset) {
  const int *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_float(const float *orig_x, unsigned int orig_offset) {
  const float *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_double(const double *orig_x, unsigned int orig_offset) {
  const double *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_longdouble(const long double *orig_x, unsigned int orig_offset) {
  const long double *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_voidstar(const void **orig_x, unsigned int orig_offset) {
  const void **_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}


/* Does in-place addition of a zero-terminated pointer (x += e and ++x).  
   Note that this expands to call _zero_arr_plus. */
static _INLINE void 
_zero_arr_inplace_plus_char(char **x, int orig_i) {
  *x = _zero_arr_plus_char(*x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_short(short **x, int orig_i) {
  *x = _zero_arr_plus_short(*x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_int(int **x, int orig_i) {
  *x = _zero_arr_plus_int(*x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_float(float **x, int orig_i) {
  *x = _zero_arr_plus_float(*x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_double(double **x, int orig_i) {
  *x = _zero_arr_plus_double(*x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_longdouble(long double **x, int orig_i) {
  *x = _zero_arr_plus_longdouble(*x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_voidstar(void ***x, int orig_i) {
  *x = _zero_arr_plus_voidstar(*x,1,orig_i);
}

/* Does in-place increment of a zero-terminated pointer (e.g., x++). */
static _INLINE char *
_zero_arr_inplace_plus_post_char_fn(char **x, int orig_i){
  char * _zap_res = *x;
  *x = _zero_arr_plus_char(_zap_res,1,orig_i);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_char(x,i) \
  _zero_arr_inplace_plus_post_char_fn((char **)(x),(i))
static _INLINE short *
_zero_arr_inplace_plus_post_short_fn(short **x, int orig_i){
  short * _zap_res = *x;
  *x = _zero_arr_plus_short(_zap_res,1,orig_i);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_short(x,i) \
  _zero_arr_inplace_plus_post_short_fn((short **)(x),(i))
static _INLINE int *
_zero_arr_inplace_plus_post_int_fn(int **x, int orig_i){
  int * _zap_res = *x;
  *x = _zero_arr_plus_int(_zap_res,1,orig_i);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_int(x,i) \
  _zero_arr_inplace_plus_post_int_fn((int **)(x),(i))
static _INLINE float *
_zero_arr_inplace_plus_post_float_fn(float **x, int orig_i){
  float * _zap_res = *x;
  *x = _zero_arr_plus_float(_zap_res,1,orig_i);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_float(x,i) \
  _zero_arr_inplace_plus_post_float_fn((float **)(x),(i))
static _INLINE double *
_zero_arr_inplace_plus_post_double_fn(double **x, int orig_i){
  double * _zap_res = *x;
  *x = _zero_arr_plus_double(_zap_res,1,orig_i);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_double(x,i) \
  _zero_arr_inplace_plus_post_double_fn((double **)(x),(i))
static _INLINE long double *
_zero_arr_inplace_plus_post_longdouble_fn(long double **x, int orig_i){
  long double * _zap_res = *x;
  *x = _zero_arr_plus_longdouble(_zap_res,1,orig_i);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_longdouble(x,i) \
  _zero_arr_inplace_plus_post_longdouble_fn((long double **)(x),(i))
static _INLINE void **
_zero_arr_inplace_plus_post_voidstar_fn(void ***x, int orig_i){
  void ** _zap_res = *x;
  *x = _zero_arr_plus_voidstar(_zap_res,1,orig_i);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_voidstar(x,i) \
  _zero_arr_inplace_plus_post_voidstar_fn((void***)(x),(i))

/* functions for dealing with dynamically sized pointers */
#ifdef NO_CYC_BOUNDS_CHECKS
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_check_dyneither_subscript(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dyneither_ptr _cus_arr = (arr);
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
#endif
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_check_dyneither_subscript(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dyneither_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  /* JGM: not needed! if (!_cus_arr.base) _throw_null(); */ 
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one)
    _throw_arraybounds();
  return _cus_ans;
}
#else
#define _check_dyneither_subscript(arr,elt_sz,index) ({ \
  struct _dyneither_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#endif
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_tag_dyneither(const void *tcurr,unsigned elt_sz,unsigned num_elts) {
  struct _dyneither_ptr _tag_arr_ans;
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr);
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts);
  return _tag_arr_ans;
}
#else
#define _tag_dyneither(tcurr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr *
_init_dyneither_ptr(struct _dyneither_ptr *arr_ptr,
                    void *arr, unsigned elt_sz, unsigned num_elts) {
  struct _dyneither_ptr *_itarr_ptr = (arr_ptr);
  void* _itarr = (arr);
  _itarr_ptr->base = _itarr_ptr->curr = _itarr;
  _itarr_ptr->last_plus_one = ((unsigned char *)_itarr) + (elt_sz) * (num_elts);
  return _itarr_ptr;
}
#else
#define _init_dyneither_ptr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ((arr).curr)
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_untag_dyneither_ptr(struct _dyneither_ptr arr, 
                     unsigned elt_sz,unsigned num_elts) {
  struct _dyneither_ptr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)
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
#endif
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned
_get_dyneither_size(struct _dyneither_ptr arr,unsigned elt_sz) {
  struct _dyneither_ptr _get_arr_size_temp = (arr);
  unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr;
  unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one;
  return (_get_arr_size_curr < _get_arr_size_temp.base ||
          _get_arr_size_curr >= _get_arr_size_last) ? 0 :
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
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_plus(struct _dyneither_ptr arr,unsigned elt_sz,int change) {
  struct _dyneither_ptr _ans = (arr);
  _ans.curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_plus(arr,elt_sz,change) ({ \
  struct _dyneither_ptr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_inplace_plus(struct _dyneither_ptr *arr_ptr,unsigned elt_sz,
                            int change) {
  struct _dyneither_ptr * _arr_ptr = (arr_ptr);
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return *_arr_ptr;
}
#else
#define _dyneither_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_inplace_plus_post(struct _dyneither_ptr *arr_ptr,unsigned elt_sz,int change) {
  struct _dyneither_ptr * _arr_ptr = (arr_ptr);
  struct _dyneither_ptr _ans = *_arr_ptr;
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  struct _dyneither_ptr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

/* Decrease the upper bound on a fat pointer by numelts where sz is
   the size of the pointer's type.  Note that this can't be a macro
   if we're to get initializers right. */
static struct 
_dyneither_ptr _dyneither_ptr_decrease_size(struct _dyneither_ptr x,
                                            unsigned int sz,
                                            unsigned int numelts) {
  x.last_plus_one -= sz * numelts; 
  return x; 
}

/* Allocation */
extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);

static _INLINE void* _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cyccalloc(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cyccalloc_atomic(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static _INLINE unsigned int _check_times(unsigned x, unsigned y) {
  _CYC_U_LONG_LONG_T whole_ans = 
    ((_CYC_U_LONG_LONG_T)x)*((_CYC_U_LONG_LONG_T)y);
  unsigned word_ans = (unsigned)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#if defined(CYC_REGION_PROFILE) 
extern void* _profile_GC_malloc(int,const char *file,const char *func,
                                int lineno);
extern void* _profile_GC_malloc_atomic(int,const char *file,
                                       const char *func,int lineno);
extern void* _profile_region_malloc(struct _RegionHandle *, unsigned,
                                    const char *file,
                                    const char *func,
                                    int lineno);
extern struct _RegionHandle _profile_new_region(const char *rgn_name,
						const char *file,
						const char *func,
                                                int lineno);
extern void _profile_free_region(struct _RegionHandle *,
				 const char *file,
                                 const char *func,
                                 int lineno);
#  if !defined(RUNTIME_CYC)
#define _new_region(n) _profile_new_region(n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__,__FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#endif
#endif

/* the next two routines swap [x] and [y]; not thread safe! */
static _INLINE void _swap_word(void *x, void *y) {
  unsigned long *lx = (unsigned long *)x, *ly = (unsigned long *)y, tmp;
  tmp = *lx;
  *lx = *ly;
  *ly = tmp;
}
static _INLINE void _swap_dyneither(struct _dyneither_ptr *x, 
				   struct _dyneither_ptr *y) {
  struct _dyneither_ptr tmp = *x;
  *x = *y;
  *y = tmp;
}
 typedef char*Cyc_Cstring;typedef char*Cyc_CstringNN;typedef struct _dyneither_ptr Cyc_string_t;
typedef struct _dyneither_ptr Cyc_mstring_t;typedef struct _dyneither_ptr*Cyc_stringptr_t;
typedef struct _dyneither_ptr*Cyc_mstringptr_t;typedef char*Cyc_Cbuffer_t;typedef
char*Cyc_CbufferNN_t;typedef struct _dyneither_ptr Cyc_buffer_t;typedef struct
_dyneither_ptr Cyc_mbuffer_t;typedef int Cyc_bool;struct Cyc_Core_NewRegion{struct
_DynRegionHandle*dynregion;};typedef unsigned long Cyc_size_t;typedef
unsigned short Cyc_mode_t;struct Cyc___cycFILE;typedef struct Cyc___cycFILE Cyc_FILE;
struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};
struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{
int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
typedef void*Cyc_parg_t;struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};
struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};
struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{
int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};
struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};typedef void*
Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{
char*tag;struct _dyneither_ptr f1;};typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{
void*v;};typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[
17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_unique_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*
tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{
char*tag;};inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,
unsigned int sz){if(bd >> 20  || sz >> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * 
sz;if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
return 0;return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
typedef struct Cyc_List_List*Cyc_List_list_t;typedef struct Cyc_List_List*Cyc_List_List_t;
extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{
char*tag;};void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{
char*tag;};struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
typedef unsigned int Cyc_Position_seg_t;unsigned int Cyc_Position_segment_join(
unsigned int,unsigned int);struct Cyc_Position_Lex_Position_Error_kind_struct{int
tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{
int tag;};typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct
_dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[
10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};typedef struct
_dyneither_ptr*Cyc_Absyn_field_name_t;typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;
struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n
Loc_n;};typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct
_tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};typedef struct _tuple0*
Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;typedef struct _tuple0*
Cyc_Absyn_typedef_name_t;typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;
typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;
typedef void*Cyc_Absyn_kindbound_t;typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;
typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;
typedef void*Cyc_Absyn_bounds_t;typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t;typedef struct Cyc_Absyn_VarargInfo
Cyc_Absyn_vararg_info_t;typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;typedef
struct Cyc_Absyn_DatatypeInfo Cyc_Absyn_datatype_info_t;typedef struct Cyc_Absyn_DatatypeFieldInfo
Cyc_Absyn_datatype_field_info_t;typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;
typedef struct Cyc_Absyn_ArrayInfo Cyc_Absyn_array_info_t;typedef void*Cyc_Absyn_type_t;
typedef void*Cyc_Absyn_rgntype_t;typedef void*Cyc_Absyn_type_opt_t;typedef union Cyc_Absyn_Cnst
Cyc_Absyn_cnst_t;typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;typedef enum Cyc_Absyn_Incrementor
Cyc_Absyn_incrementor_t;typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
typedef void*Cyc_Absyn_raw_exp_t;typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_t;
typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_opt_t;typedef void*Cyc_Absyn_raw_stmt_t;
typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_t;typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_opt_t;
typedef void*Cyc_Absyn_raw_pat_t;typedef struct Cyc_Absyn_Pat*Cyc_Absyn_pat_t;
typedef void*Cyc_Absyn_binding_t;typedef struct Cyc_Absyn_Switch_clause*Cyc_Absyn_switch_clause_t;
typedef struct Cyc_Absyn_Fndecl*Cyc_Absyn_fndecl_t;typedef struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_aggrdecl_t;typedef struct Cyc_Absyn_Datatypefield*Cyc_Absyn_datatypefield_t;
typedef struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_datatypedecl_t;typedef struct Cyc_Absyn_Typedefdecl*
Cyc_Absyn_typedefdecl_t;typedef struct Cyc_Absyn_Enumfield*Cyc_Absyn_enumfield_t;
typedef struct Cyc_Absyn_Enumdecl*Cyc_Absyn_enumdecl_t;typedef struct Cyc_Absyn_Vardecl*
Cyc_Absyn_vardecl_t;typedef void*Cyc_Absyn_raw_decl_t;typedef struct Cyc_Absyn_Decl*
Cyc_Absyn_decl_t;typedef void*Cyc_Absyn_designator_t;typedef void*Cyc_Absyn_absyn_annot_t;
typedef void*Cyc_Absyn_attribute_t;typedef struct Cyc_List_List*Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Aggrfield*Cyc_Absyn_aggrfield_t;typedef void*Cyc_Absyn_offsetof_field_t;
typedef struct Cyc_Absyn_MallocInfo Cyc_Absyn_malloc_info_t;typedef enum Cyc_Absyn_Coercion
Cyc_Absyn_coercion_t;typedef struct Cyc_Absyn_PtrLoc*Cyc_Absyn_ptrloc_t;enum Cyc_Absyn_Scope{
Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern
 = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int
print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};enum Cyc_Absyn_Size_of{
Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz
 = 3,Cyc_Absyn_LongLong_sz  = 4};enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,
Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind
 = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind
 = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;
enum Cyc_Absyn_AliasQual aliasqual;};enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned
 = 1,Cyc_Absyn_None  = 2};enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA
 = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct
_union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct
_union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr
Forward_constr;struct _union_Constraint_No_constr No_constr;};typedef union Cyc_Absyn_Constraint*
Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*
f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int
identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;
struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int
zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;
union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*
ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct
Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{
int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};
struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{
int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};
struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{
int tag;};typedef void*Cyc_Absyn_pointer_qual_t;typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;
struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;
void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*
effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct
_tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int
tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{
struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);union
Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,
struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU
field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;
struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int
tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**
val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;
struct _union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(
enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);union Cyc_Absyn_AggrInfoU
Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union
Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{
void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*
zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;};typedef void*Cyc_Absyn_raw_type_decl_t;
struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};typedef struct Cyc_Absyn_TypeDecl*
Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct
Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;
struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct
Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct
Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int
tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};
struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;
struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;
struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{
int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;
void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{
int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;extern
struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{
int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct
Cyc_List_List*f5;};typedef void*Cyc_Absyn_funcparams_t;enum Cyc_Absyn_Format_Type{
Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{
int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};
struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};
struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct
Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type
f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;
int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};
struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{
int tag;struct _dyneither_ptr f1;};extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct
Cyc_Absyn_Stdcall_att_val;extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct
Cyc_Absyn_Cdecl_att_val;extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct
Cyc_Absyn_Fastcall_att_val;extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct
Cyc_Absyn_Noreturn_att_val;extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct
Cyc_Absyn_Const_att_val;extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct
Cyc_Absyn_Packed_att_val;extern struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct
Cyc_Absyn_Nocommon_att_val;extern struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct
Cyc_Absyn_Shared_att_val;extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct
Cyc_Absyn_Unused_att_val;extern struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct
Cyc_Absyn_Weak_att_val;extern struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct
Cyc_Absyn_Dllimport_att_val;extern struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct
Cyc_Absyn_Dllexport_att_val;extern struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct
Cyc_Absyn_No_instrument_function_att_val;extern struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct
Cyc_Absyn_Constructor_att_val;extern struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct
Cyc_Absyn_Destructor_att_val;extern struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct
Cyc_Absyn_No_check_memory_usage_att_val;extern struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct
Cyc_Absyn_Pure_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int
tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};
struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts
f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int
tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{
int tag;unsigned int f1;struct Cyc_List_List*f2;};typedef void*Cyc_Absyn_type_modifier_t;
struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;
char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct
_union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};
struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct
_union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;
struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct
_dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct
_union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};extern union
Cyc_Absyn_Cnst Cyc_Absyn_Null_c;union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,
char);union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _dyneither_ptr);union Cyc_Absyn_Cnst
Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(
enum Cyc_Absyn_Sign,int);union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,
long long);union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);union
Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr);union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(
struct _dyneither_ptr);enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times
 = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,
Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte
 = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor
 = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,
Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc
 = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct
_dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;
unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion
 = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{
int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;
int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst
f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;
};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;
struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*
f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;
void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{
int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*
f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct
_tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};
struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{
int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;
};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{
int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct
_dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*
annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int
tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};
struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{
int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*
f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int
tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct
Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*
non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{
int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int
tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int
tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;
int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*
f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{
int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;
int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{
int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{
int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int
tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct
Cyc_Absyn_Wild_p_val;extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;
struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{
int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;
struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual
ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;
struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;
struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind
kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct
_tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};
struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{
struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{
enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;
struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;
};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*
f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*
f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{
int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};extern struct
Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct
Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{
int tag;struct _dyneither_ptr*f1;};char Cyc_Absyn_EmptyAnnot[11]="EmptyAnnot";
struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct
Cyc_Absyn_EmptyAnnot_val;int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);int Cyc_Absyn_is_qvar_qualified(
struct _tuple0*);struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);struct
Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual
y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);union Cyc_Absyn_Constraint*
Cyc_Absyn_new_conref(void*x);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*
x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_false_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(
void*);struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;void*Cyc_Absyn_float_typ(
int);void*Cyc_Absyn_wchar_typ();extern void*Cyc_Absyn_empty_effect;extern struct
_tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
void*Cyc_Absyn_exn_typ();extern struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar;void*Cyc_Absyn_string_typ(
void*rgn);void*Cyc_Absyn_const_string_typ(void*rgn);void*Cyc_Absyn_file_typ();
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,
union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_star_typ(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple0*name);void*Cyc_Absyn_unionq_typ(struct
_tuple0*name);void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name);void*Cyc_Absyn_array_typ(
void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*
zero_term,unsigned int ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*
rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_bool_exp(int,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);struct
Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(
struct _dyneither_ptr s,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct
_tuple0*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,
void*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,
struct Cyc_List_List*es,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct
Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct
Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(
void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_offsetof_exp(void*,void*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(
struct Cyc_List_List*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct
Cyc_Absyn_Stmt*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,
struct Cyc_List_List*,unsigned int);extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(
void*s,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,
unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*
e,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_continue_stmt(unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*
s,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,
unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*
v,struct Cyc_Absyn_Stmt*s,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,unsigned int);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,
unsigned int s);struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,
int,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,
struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(
struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int
tagged);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum 
Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*
i,struct Cyc_List_List*atts,unsigned int loc);struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(
enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,
struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,
unsigned int loc);struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,
struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,unsigned int loc);struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope
s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,
unsigned int loc);struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope
s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,
unsigned int loc);void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*
eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,
struct Cyc_List_List*atts);void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);int
Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(
struct Cyc_List_List*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple10{struct Cyc_Absyn_Tqual
f1;void*f2;};struct _tuple10*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,
int);struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(
void*a);struct _dyneither_ptr*Cyc_Absyn_fieldname(int);struct _tuple11{enum Cyc_Absyn_AggrKind
f1;struct _tuple0*f2;};struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU
info);int Cyc_Absyn_is_union_type(void*);int Cyc_Absyn_is_nontagged_union_type(
void*);int Cyc_Absyn_is_aggr_type(void*t);extern int Cyc_Absyn_porting_c_code;
extern int Cyc_Absyn_no_regions;int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct
_dyneither_ptr*s2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[
7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;typedef const
struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;const struct Cyc_Dict_T*t;};typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;
extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct
Cyc_RgnOrder_RgnPO;typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;struct
Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*
tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct
_RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct
Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int Cyc_RgnOrder_is_region_resetable(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin);int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*
eff1,void*eff2);void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);
struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};typedef const struct
Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct
Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;
struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};typedef void*Cyc_Tcenv_resolved_t;
struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Set_Set*namespaces;
struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict
enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*
availables;};typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct{
int tag;};struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct{
int tag;};struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};typedef struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);extern struct Cyc_Absyn_Kind
Cyc_Tcutil_rk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;extern struct Cyc_Core_Opt
Cyc_Tcutil_tmko;void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);void Cyc_Tcutil_check_no_qual(
unsigned int loc,void*t);extern int Cyc_Cyclone_tovc_r;enum Cyc_Cyclone_C_Compilers{
Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};extern enum Cyc_Cyclone_C_Compilers
Cyc_Cyclone_c_compiler;int Cyc_Cyclone_tovc_r=0;enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler=
Cyc_Cyclone_Gcc_c;static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,struct
Cyc_List_List*ss2){for(0;ss1 != 0;ss1=ss1->tl){if(ss2 == 0)return 1;{int i=Cyc_strptrcmp((
struct _dyneither_ptr*)ss1->hd,(struct _dyneither_ptr*)ss2->hd);if(i != 0)return i;
ss2=ss2->tl;};}if(ss2 != 0)return - 1;return 0;}int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*
vs1,struct Cyc_List_List*vs2){if((int)vs1 == (int)vs2)return 0;return Cyc_Absyn_strlist_cmp(
vs1,vs2);}struct _tuple12{union Cyc_Absyn_Nmspace f1;union Cyc_Absyn_Nmspace f2;};int
Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2){if(q1 == q2)return 0;{struct
_tuple0 _tmp1;union Cyc_Absyn_Nmspace _tmp2;struct _dyneither_ptr*_tmp3;struct
_tuple0*_tmp0=q1;_tmp1=*_tmp0;_tmp2=_tmp1.f1;_tmp3=_tmp1.f2;{struct _tuple0 _tmp5;
union Cyc_Absyn_Nmspace _tmp6;struct _dyneither_ptr*_tmp7;struct _tuple0*_tmp4=q2;
_tmp5=*_tmp4;_tmp6=_tmp5.f1;_tmp7=_tmp5.f2;{int i=Cyc_strptrcmp(_tmp3,_tmp7);if(i
!= 0)return i;{struct _tuple12 _tmp240;struct _tuple12 _tmp9=(_tmp240.f1=_tmp2,((
_tmp240.f2=_tmp6,_tmp240)));union Cyc_Absyn_Nmspace _tmpA;int _tmpB;union Cyc_Absyn_Nmspace
_tmpC;int _tmpD;union Cyc_Absyn_Nmspace _tmpE;struct Cyc_List_List*_tmpF;union Cyc_Absyn_Nmspace
_tmp10;struct Cyc_List_List*_tmp11;union Cyc_Absyn_Nmspace _tmp12;struct Cyc_List_List*
_tmp13;union Cyc_Absyn_Nmspace _tmp14;struct Cyc_List_List*_tmp15;union Cyc_Absyn_Nmspace
_tmp16;struct Cyc_List_List*_tmp17;union Cyc_Absyn_Nmspace _tmp18;struct Cyc_List_List*
_tmp19;union Cyc_Absyn_Nmspace _tmp1A;int _tmp1B;union Cyc_Absyn_Nmspace _tmp1C;int
_tmp1D;union Cyc_Absyn_Nmspace _tmp1E;struct Cyc_List_List*_tmp1F;union Cyc_Absyn_Nmspace
_tmp20;struct Cyc_List_List*_tmp21;union Cyc_Absyn_Nmspace _tmp22;struct Cyc_List_List*
_tmp23;union Cyc_Absyn_Nmspace _tmp24;struct Cyc_List_List*_tmp25;_LL1: _tmpA=_tmp9.f1;
if((_tmpA.Loc_n).tag != 4)goto _LL3;_tmpB=(int)(_tmpA.Loc_n).val;_tmpC=_tmp9.f2;
if((_tmpC.Loc_n).tag != 4)goto _LL3;_tmpD=(int)(_tmpC.Loc_n).val;_LL2: return 0;_LL3:
_tmpE=_tmp9.f1;if((_tmpE.Rel_n).tag != 1)goto _LL5;_tmpF=(struct Cyc_List_List*)(
_tmpE.Rel_n).val;_tmp10=_tmp9.f2;if((_tmp10.Rel_n).tag != 1)goto _LL5;_tmp11=(
struct Cyc_List_List*)(_tmp10.Rel_n).val;_LL4: return Cyc_Absyn_strlist_cmp(_tmpF,
_tmp11);_LL5: _tmp12=_tmp9.f1;if((_tmp12.Abs_n).tag != 2)goto _LL7;_tmp13=(struct
Cyc_List_List*)(_tmp12.Abs_n).val;_tmp14=_tmp9.f2;if((_tmp14.Abs_n).tag != 2)goto
_LL7;_tmp15=(struct Cyc_List_List*)(_tmp14.Abs_n).val;_LL6: return Cyc_Absyn_strlist_cmp(
_tmp13,_tmp15);_LL7: _tmp16=_tmp9.f1;if((_tmp16.C_n).tag != 3)goto _LL9;_tmp17=(
struct Cyc_List_List*)(_tmp16.C_n).val;_tmp18=_tmp9.f2;if((_tmp18.C_n).tag != 3)
goto _LL9;_tmp19=(struct Cyc_List_List*)(_tmp18.C_n).val;_LL8: return Cyc_Absyn_strlist_cmp(
_tmp17,_tmp19);_LL9: _tmp1A=_tmp9.f1;if((_tmp1A.Loc_n).tag != 4)goto _LLB;_tmp1B=(
int)(_tmp1A.Loc_n).val;_LLA: return - 1;_LLB: _tmp1C=_tmp9.f2;if((_tmp1C.Loc_n).tag
!= 4)goto _LLD;_tmp1D=(int)(_tmp1C.Loc_n).val;_LLC: return 1;_LLD: _tmp1E=_tmp9.f1;
if((_tmp1E.Rel_n).tag != 1)goto _LLF;_tmp1F=(struct Cyc_List_List*)(_tmp1E.Rel_n).val;
_LLE: return - 1;_LLF: _tmp20=_tmp9.f2;if((_tmp20.Rel_n).tag != 1)goto _LL11;_tmp21=(
struct Cyc_List_List*)(_tmp20.Rel_n).val;_LL10: return 1;_LL11: _tmp22=_tmp9.f1;if((
_tmp22.Abs_n).tag != 2)goto _LL13;_tmp23=(struct Cyc_List_List*)(_tmp22.Abs_n).val;
_LL12: return - 1;_LL13: _tmp24=_tmp9.f2;if((_tmp24.Abs_n).tag != 2)goto _LL0;_tmp25=(
struct Cyc_List_List*)(_tmp24.Abs_n).val;_LL14: return 1;_LL0:;};};};};}int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){int i=Cyc_strptrcmp(tv1->name,
tv2->name);if(i != 0)return i;return tv1->identity - tv2->identity;}union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n={.Loc_n={4,0}};union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*
x,int C_scope){if(C_scope){union Cyc_Absyn_Nmspace _tmp241;return((_tmp241.C_n).val=
x,(((_tmp241.C_n).tag=3,_tmp241)));}else{union Cyc_Absyn_Nmspace _tmp242;return((
_tmp242.Abs_n).val=x,(((_tmp242.Abs_n).tag=2,_tmp242)));}}union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*x){union Cyc_Absyn_Nmspace _tmp243;return((
_tmp243.Rel_n).val=x,(((_tmp243.Rel_n).tag=1,_tmp243)));}union Cyc_Absyn_Nmspace
Cyc_Absyn_rel_ns_null={.Rel_n={1,0}};int Cyc_Absyn_is_qvar_qualified(struct
_tuple0*qv){union Cyc_Absyn_Nmspace _tmp29=(*qv).f1;struct Cyc_List_List*_tmp2A;
struct Cyc_List_List*_tmp2B;int _tmp2C;_LL16: if((_tmp29.Rel_n).tag != 1)goto _LL18;
_tmp2A=(struct Cyc_List_List*)(_tmp29.Rel_n).val;if(_tmp2A != 0)goto _LL18;_LL17:
goto _LL19;_LL18: if((_tmp29.Abs_n).tag != 2)goto _LL1A;_tmp2B=(struct Cyc_List_List*)(
_tmp29.Abs_n).val;if(_tmp2B != 0)goto _LL1A;_LL19: goto _LL1B;_LL1A: if((_tmp29.Loc_n).tag
!= 4)goto _LL1C;_tmp2C=(int)(_tmp29.Loc_n).val;_LL1B: return 0;_LL1C:;_LL1D: return 1;
_LL15:;}static int Cyc_Absyn_new_type_counter=0;void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*env){struct Cyc_Absyn_Evar_Absyn_Type_struct _tmp246;struct
Cyc_Absyn_Evar_Absyn_Type_struct*_tmp245;return(void*)((_tmp245=_cycalloc(
sizeof(*_tmp245)),((_tmp245[0]=((_tmp246.tag=1,((_tmp246.f1=k,((_tmp246.f2=(void*)
0,((_tmp246.f3=Cyc_Absyn_new_type_counter ++,((_tmp246.f4=env,_tmp246)))))))))),
_tmp245))));}void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko,tenv);}struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){struct Cyc_Absyn_Tqual _tmp247;
return(_tmp247.print_const=x.print_const  || y.print_const,((_tmp247.q_volatile=x.q_volatile
 || y.q_volatile,((_tmp247.q_restrict=x.q_restrict  || y.q_restrict,((_tmp247.real_const=
x.real_const  || y.real_const,((_tmp247.loc=Cyc_Position_segment_join(x.loc,y.loc),
_tmp247)))))))));}struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int loc){
struct Cyc_Absyn_Tqual _tmp248;return(_tmp248.print_const=0,((_tmp248.q_volatile=0,((
_tmp248.q_restrict=0,((_tmp248.real_const=0,((_tmp248.loc=loc,_tmp248)))))))));}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int loc){struct Cyc_Absyn_Tqual
_tmp249;return(_tmp249.print_const=1,((_tmp249.q_volatile=0,((_tmp249.q_restrict=
0,((_tmp249.real_const=1,((_tmp249.loc=loc,_tmp249)))))))));}struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct
Cyc_Absyn_EmptyAnnot_val={Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct
Cyc_Absyn_one_b_raw={0,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};struct Cyc_Absyn_Exp
Cyc_Absyn_exp_unsigned_one_v={0,(void*)& Cyc_Absyn_one_b_raw,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=& Cyc_Absyn_exp_unsigned_one_v;
static struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct Cyc_Absyn_one_bt={1,& Cyc_Absyn_exp_unsigned_one_v};
void*Cyc_Absyn_bounds_one=(void*)& Cyc_Absyn_one_bt;union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo udi){union Cyc_Absyn_DatatypeInfoU
_tmp24A;return((_tmp24A.UnknownDatatype).val=udi,(((_tmp24A.UnknownDatatype).tag=
1,_tmp24A)));}extern union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct
Cyc_Absyn_Datatypedecl**d){union Cyc_Absyn_DatatypeInfoU _tmp24B;return((_tmp24B.KnownDatatype).val=
d,(((_tmp24B.KnownDatatype).tag=2,_tmp24B)));}union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo s){union
Cyc_Absyn_DatatypeFieldInfoU _tmp24C;return((_tmp24C.UnknownDatatypefield).val=s,(((
_tmp24C.UnknownDatatypefield).tag=1,_tmp24C)));}union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*
df){struct _tuple1 _tmp24F;union Cyc_Absyn_DatatypeFieldInfoU _tmp24E;return((
_tmp24E.KnownDatatypefield).val=((_tmp24F.f1=dd,((_tmp24F.f2=df,_tmp24F)))),(((
_tmp24E.KnownDatatypefield).tag=2,_tmp24E)));}union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(
enum Cyc_Absyn_AggrKind ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){struct
_tuple2 _tmp252;union Cyc_Absyn_AggrInfoU _tmp251;return((_tmp251.UnknownAggr).val=((
_tmp252.f1=ak,((_tmp252.f2=n,((_tmp252.f3=tagged,_tmp252)))))),(((_tmp251.UnknownAggr).tag=
1,_tmp251)));}union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**
ad){union Cyc_Absyn_AggrInfoU _tmp253;return((_tmp253.KnownAggr).val=ad,(((_tmp253.KnownAggr).tag=
2,_tmp253)));}union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x){union Cyc_Absyn_Constraint*
_tmp254;return(_tmp254=_cycalloc(sizeof(*_tmp254)),(((_tmp254->Eq_constr).val=(
void*)x,(((_tmp254->Eq_constr).tag=1,_tmp254)))));}union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref(){
union Cyc_Absyn_Constraint*_tmp255;return(_tmp255=_cycalloc_atomic(sizeof(*
_tmp255)),(((_tmp255->No_constr).val=0,(((_tmp255->No_constr).tag=3,_tmp255)))));}
struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val={0};
static union Cyc_Absyn_Constraint Cyc_Absyn_true_conref_v={.Eq_constr={1,(void*)1}};
union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v;static
union Cyc_Absyn_Constraint Cyc_Absyn_false_conref_v={.Eq_constr={1,(void*)0}};
union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_one_conref_v={.Eq_constr={1,(
void*)((void*)& Cyc_Absyn_one_bt)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref=&
Cyc_Absyn_bounds_one_conref_v;static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_dyneither_conref_v={.Eq_constr={
1,(void*)((void*)& Cyc_Absyn_DynEither_b_val)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref=&
Cyc_Absyn_bounds_dyneither_conref_v;union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(
union Cyc_Absyn_Constraint*x){union Cyc_Absyn_Constraint*_tmp40=x;union Cyc_Absyn_Constraint
_tmp41;int _tmp42;union Cyc_Absyn_Constraint _tmp43;void*_tmp44;union Cyc_Absyn_Constraint
_tmp45;union Cyc_Absyn_Constraint*_tmp46;_LL1F: _tmp41=*_tmp40;if((_tmp41.No_constr).tag
!= 3)goto _LL21;_tmp42=(int)(_tmp41.No_constr).val;_LL20: goto _LL22;_LL21: _tmp43=*
_tmp40;if((_tmp43.Eq_constr).tag != 1)goto _LL23;_tmp44=(void*)(_tmp43.Eq_constr).val;
_LL22: return x;_LL23: _tmp45=*_tmp40;if((_tmp45.Forward_constr).tag != 2)goto _LL1E;
_tmp46=(union Cyc_Absyn_Constraint*)(_tmp45.Forward_constr).val;_LL24: {union Cyc_Absyn_Constraint*
_tmp47=Cyc_Absyn_compress_conref(_tmp46);{struct _union_Constraint_Forward_constr*
_tmp256;(_tmp256=& x->Forward_constr,((_tmp256->tag=2,_tmp256->val=_tmp47)));}
return _tmp47;}_LL1E:;}void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x){
union Cyc_Absyn_Constraint*_tmp49=Cyc_Absyn_compress_conref(x);union Cyc_Absyn_Constraint
_tmp4A;void*_tmp4B;_LL26: _tmp4A=*_tmp49;if((_tmp4A.Eq_constr).tag != 1)goto _LL28;
_tmp4B=(void*)(_tmp4A.Eq_constr).val;_LL27: return _tmp4B;_LL28:;_LL29: {const char*
_tmp259;void*_tmp258;(_tmp258=0,Cyc_Tcutil_impos(((_tmp259="conref_val",
_tag_dyneither(_tmp259,sizeof(char),11))),_tag_dyneither(_tmp258,sizeof(void*),0)));}
_LL25:;}void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x){union Cyc_Absyn_Constraint*
_tmp4E=x;union Cyc_Absyn_Constraint _tmp4F;void*_tmp50;union Cyc_Absyn_Constraint
_tmp51;int _tmp52;_LL2B: _tmp4F=*_tmp4E;if((_tmp4F.Eq_constr).tag != 1)goto _LL2D;
_tmp50=(void*)(_tmp4F.Eq_constr).val;_LL2C: return _tmp50;_LL2D: _tmp51=*_tmp4E;if((
_tmp51.No_constr).tag != 3)goto _LL2F;_tmp52=(int)(_tmp51.No_constr).val;_LL2E:
return y;_LL2F:;_LL30: return Cyc_Absyn_conref_def(y,Cyc_Absyn_compress_conref(x));
_LL2A:;}void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x){x=Cyc_Absyn_compress_conref(
x);{union Cyc_Absyn_Constraint*_tmp53=x;union Cyc_Absyn_Constraint _tmp54;void*
_tmp55;_LL32: _tmp54=*_tmp53;if((_tmp54.Eq_constr).tag != 1)goto _LL34;_tmp55=(void*)(
_tmp54.Eq_constr).val;_LL33: return _tmp55;_LL34:;_LL35:{struct
_union_Constraint_Eq_constr*_tmp25A;(_tmp25A=& x->Eq_constr,((_tmp25A->tag=1,
_tmp25A->val=y)));}return y;_LL31:;};}void*Cyc_Absyn_compress_kb(void*k){void*
_tmp57=k;struct Cyc_Core_Opt*_tmp5A;struct Cyc_Core_Opt*_tmp5C;struct Cyc_Core_Opt*
_tmp5E;struct Cyc_Core_Opt _tmp5F;void*_tmp60;void**_tmp61;struct Cyc_Core_Opt*
_tmp63;struct Cyc_Core_Opt _tmp64;void*_tmp65;void**_tmp66;_LL37: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*
_tmp58=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp57;if(_tmp58->tag != 0)
goto _LL39;}_LL38: goto _LL3A;_LL39: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*
_tmp59=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp57;if(_tmp59->tag
!= 1)goto _LL3B;else{_tmp5A=_tmp59->f1;if(_tmp5A != 0)goto _LL3B;}}_LL3A: goto _LL3C;
_LL3B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp5B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)
_tmp57;if(_tmp5B->tag != 2)goto _LL3D;else{_tmp5C=_tmp5B->f1;if(_tmp5C != 0)goto
_LL3D;}}_LL3C: return k;_LL3D: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*
_tmp5D=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp57;if(_tmp5D->tag
!= 1)goto _LL3F;else{_tmp5E=_tmp5D->f1;if(_tmp5E == 0)goto _LL3F;_tmp5F=*_tmp5E;
_tmp60=(void*)_tmp5F.v;_tmp61=(void**)&(*_tmp5D->f1).v;}}_LL3E: _tmp66=_tmp61;
goto _LL40;_LL3F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp62=(struct
Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp57;if(_tmp62->tag != 2)goto _LL36;
else{_tmp63=_tmp62->f1;if(_tmp63 == 0)goto _LL36;_tmp64=*_tmp63;_tmp65=(void*)
_tmp64.v;_tmp66=(void**)&(*_tmp62->f1).v;}}_LL40:*_tmp66=Cyc_Absyn_compress_kb(*
_tmp66);return*_tmp66;_LL36:;}struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb){
void*_tmp67=Cyc_Absyn_compress_kb(kb);struct Cyc_Absyn_Kind*_tmp69;struct Cyc_Core_Opt*
_tmp6B;struct Cyc_Core_Opt**_tmp6C;struct Cyc_Core_Opt*_tmp6E;struct Cyc_Core_Opt**
_tmp6F;struct Cyc_Absyn_Kind*_tmp70;_LL42: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*
_tmp68=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp67;if(_tmp68->tag != 0)
goto _LL44;else{_tmp69=_tmp68->f1;}}_LL43: return _tmp69;_LL44: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*
_tmp6A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp67;if(_tmp6A->tag
!= 1)goto _LL46;else{_tmp6B=_tmp6A->f1;_tmp6C=(struct Cyc_Core_Opt**)& _tmp6A->f1;}}
_LL45: _tmp6F=_tmp6C;_tmp70=& Cyc_Tcutil_bk;goto _LL47;_LL46: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmp6D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp67;if(_tmp6D->tag != 
2)goto _LL41;else{_tmp6E=_tmp6D->f1;_tmp6F=(struct Cyc_Core_Opt**)& _tmp6D->f1;
_tmp70=_tmp6D->f2;}}_LL47:{struct Cyc_Core_Opt*_tmp25B;*_tmp6F=((_tmp25B=
_cycalloc(sizeof(*_tmp25B)),((_tmp25B->v=Cyc_Tcutil_kind_to_bound(_tmp70),
_tmp25B))));}return _tmp70;_LL41:;}struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val={
20};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val={21};
struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val={22};struct
Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val={0};static struct Cyc_Absyn_IntType_Absyn_Type_struct
Cyc_Absyn_char_tt={6,Cyc_Absyn_None,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_IntType_Absyn_Type_struct
Cyc_Absyn_uchar_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_IntType_Absyn_Type_struct
Cyc_Absyn_ushort_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_IntType_Absyn_Type_struct
Cyc_Absyn_uint_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_IntType_Absyn_Type_struct
Cyc_Absyn_ulong_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_IntType_Absyn_Type_struct
Cyc_Absyn_ulonglong_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_LongLong_sz};void*Cyc_Absyn_char_typ=(
void*)& Cyc_Absyn_char_tt;void*Cyc_Absyn_uchar_typ=(void*)& Cyc_Absyn_uchar_tt;
void*Cyc_Absyn_ushort_typ=(void*)& Cyc_Absyn_ushort_tt;void*Cyc_Absyn_uint_typ=(
void*)& Cyc_Absyn_uint_tt;void*Cyc_Absyn_ulong_typ=(void*)& Cyc_Absyn_ulong_tt;
void*Cyc_Absyn_ulonglong_typ=(void*)& Cyc_Absyn_ulonglong_tt;static struct Cyc_Absyn_IntType_Absyn_Type_struct
Cyc_Absyn_schar_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_IntType_Absyn_Type_struct
Cyc_Absyn_sshort_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_IntType_Absyn_Type_struct
Cyc_Absyn_sint_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_IntType_Absyn_Type_struct
Cyc_Absyn_slong_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_IntType_Absyn_Type_struct
Cyc_Absyn_slonglong_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_LongLong_sz};void*Cyc_Absyn_schar_typ=(
void*)& Cyc_Absyn_schar_tt;void*Cyc_Absyn_sshort_typ=(void*)& Cyc_Absyn_sshort_tt;
void*Cyc_Absyn_sint_typ=(void*)& Cyc_Absyn_sint_tt;void*Cyc_Absyn_slong_typ=(void*)&
Cyc_Absyn_slong_tt;void*Cyc_Absyn_slonglong_typ=(void*)& Cyc_Absyn_slonglong_tt;
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nshort_tt={6,Cyc_Absyn_None,
Cyc_Absyn_Short_sz};static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nint_tt={
6,Cyc_Absyn_None,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_IntType_Absyn_Type_struct
Cyc_Absyn_nlong_tt={6,Cyc_Absyn_None,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_IntType_Absyn_Type_struct
Cyc_Absyn_nlonglong_tt={6,Cyc_Absyn_None,Cyc_Absyn_LongLong_sz};void*Cyc_Absyn_nshort_typ=(
void*)& Cyc_Absyn_nshort_tt;void*Cyc_Absyn_nint_typ=(void*)& Cyc_Absyn_nint_tt;
void*Cyc_Absyn_nlong_typ=(void*)& Cyc_Absyn_nlong_tt;void*Cyc_Absyn_nlonglong_typ=(
void*)& Cyc_Absyn_nlonglong_tt;void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign sn,enum 
Cyc_Absyn_Size_of sz){switch(sn){case Cyc_Absyn_Signed: _LL48: switch(sz){case Cyc_Absyn_Char_sz:
_LL4A: return Cyc_Absyn_schar_typ;case Cyc_Absyn_Short_sz: _LL4B: return Cyc_Absyn_sshort_typ;
case Cyc_Absyn_Int_sz: _LL4C: return Cyc_Absyn_sint_typ;case Cyc_Absyn_Long_sz: _LL4D:
return Cyc_Absyn_slong_typ;case Cyc_Absyn_LongLong_sz: _LL4E: return Cyc_Absyn_slonglong_typ;}
case Cyc_Absyn_Unsigned: _LL49: switch(sz){case Cyc_Absyn_Char_sz: _LL51: return Cyc_Absyn_uchar_typ;
case Cyc_Absyn_Short_sz: _LL52: return Cyc_Absyn_ushort_typ;case Cyc_Absyn_Int_sz:
_LL53: return Cyc_Absyn_uint_typ;case Cyc_Absyn_Long_sz: _LL54: return Cyc_Absyn_ulong_typ;
case Cyc_Absyn_LongLong_sz: _LL55: return Cyc_Absyn_ulonglong_typ;}case Cyc_Absyn_None:
_LL50: switch(sz){case Cyc_Absyn_Char_sz: _LL58: return Cyc_Absyn_char_typ;case Cyc_Absyn_Short_sz:
_LL59: return Cyc_Absyn_nshort_typ;case Cyc_Absyn_Int_sz: _LL5A: return Cyc_Absyn_nint_typ;
case Cyc_Absyn_Long_sz: _LL5B: return Cyc_Absyn_nlong_typ;case Cyc_Absyn_LongLong_sz:
_LL5C: return Cyc_Absyn_nlonglong_typ;}}}void*Cyc_Absyn_float_typ(int i){static
struct Cyc_Absyn_FloatType_Absyn_Type_struct fl={7,0};static struct Cyc_Absyn_FloatType_Absyn_Type_struct
db={7,1};static struct Cyc_Absyn_FloatType_Absyn_Type_struct ldb={7,2};if(i == 0)
return(void*)& fl;if(i == 1)return(void*)& db;return(void*)& ldb;}extern int
Wchar_t_unsigned;extern int Sizeof_wchar_t;void*Cyc_Absyn_wchar_typ(){switch(
Sizeof_wchar_t){case 1: _LL5E: return Wchar_t_unsigned?Cyc_Absyn_uchar_typ: Cyc_Absyn_schar_typ;
case 2: _LL5F: return Wchar_t_unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
default: _LL60: return Wchar_t_unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;}}
static char _tmp88[4]="exn";static struct _dyneither_ptr Cyc_Absyn_exn_str={_tmp88,
_tmp88,_tmp88 + 4};static struct _tuple0 Cyc_Absyn_exn_name_v={{.Abs_n={2,0}},& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp89[15]="Null_Exception";
static struct _dyneither_ptr Cyc_Absyn_Null_Exception_str={_tmp89,_tmp89,_tmp89 + 15};
static struct _tuple0 Cyc_Absyn_Null_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Null_Exception_str};
struct _tuple0*Cyc_Absyn_Null_Exception_name=& Cyc_Absyn_Null_Exception_pr;static
struct Cyc_Absyn_Datatypefield Cyc_Absyn_Null_Exception_tuf_v={& Cyc_Absyn_Null_Exception_pr,
0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Null_Exception_tuf=&
Cyc_Absyn_Null_Exception_tuf_v;static char _tmp8A[13]="Array_bounds";static struct
_dyneither_ptr Cyc_Absyn_Array_bounds_str={_tmp8A,_tmp8A,_tmp8A + 13};static struct
_tuple0 Cyc_Absyn_Array_bounds_pr={{.Abs_n={2,0}},& Cyc_Absyn_Array_bounds_str};
struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static
struct Cyc_Absyn_Datatypefield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,
0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Array_bounds_tuf=&
Cyc_Absyn_Array_bounds_tuf_v;static char _tmp8B[16]="Match_Exception";static struct
_dyneither_ptr Cyc_Absyn_Match_Exception_str={_tmp8B,_tmp8B,_tmp8B + 16};static
struct _tuple0 Cyc_Absyn_Match_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Match_Exception_str};
struct _tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;
static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,
0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Match_Exception_tuf=&
Cyc_Absyn_Match_Exception_tuf_v;static char _tmp8C[10]="Bad_alloc";static struct
_dyneither_ptr Cyc_Absyn_Bad_alloc_str={_tmp8C,_tmp8C,_tmp8C + 10};static struct
_tuple0 Cyc_Absyn_Bad_alloc_pr={{.Abs_n={2,0}},& Cyc_Absyn_Bad_alloc_str};struct
_tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;static struct Cyc_Absyn_Datatypefield
Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,0,0,Cyc_Absyn_Extern};struct
Cyc_Absyn_Datatypefield*Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;
static struct Cyc_List_List Cyc_Absyn_exn_l0={(void*)& Cyc_Absyn_Null_Exception_tuf_v,
0};static struct Cyc_List_List Cyc_Absyn_exn_l1={(void*)& Cyc_Absyn_Array_bounds_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l0};static struct Cyc_List_List Cyc_Absyn_exn_l2={(
void*)& Cyc_Absyn_Match_Exception_tuf_v,(struct Cyc_List_List*)& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={(void*)& Cyc_Absyn_Bad_alloc_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l2};static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(
void*)((struct Cyc_List_List*)& Cyc_Absyn_exn_l3)};static struct Cyc_Absyn_Datatypedecl
Cyc_Absyn_exn_tud_v={Cyc_Absyn_Extern,& Cyc_Absyn_exn_name_v,0,(struct Cyc_Core_Opt*)&
Cyc_Absyn_exn_ol,1};struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud=& Cyc_Absyn_exn_tud_v;
static struct Cyc_Absyn_DatatypeType_Absyn_Type_struct Cyc_Absyn_exn_typ_tt={3,{{.KnownDatatype={
2,& Cyc_Absyn_exn_tud}},0}};void*Cyc_Absyn_exn_typ(){static struct Cyc_Core_Opt*
exn_type_val=0;if(exn_type_val == 0){struct Cyc_Core_Opt*_tmp25C;exn_type_val=((
_tmp25C=_cycalloc(sizeof(*_tmp25C)),((_tmp25C->v=Cyc_Absyn_at_typ((void*)& Cyc_Absyn_exn_typ_tt,(
void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),
_tmp25C))));}return(void*)exn_type_val->v;}static char _tmp8F[9]="PrintArg";static
struct _dyneither_ptr Cyc_Absyn_printarg_str={_tmp8F,_tmp8F,_tmp8F + 9};static char
_tmp90[9]="ScanfArg";static struct _dyneither_ptr Cyc_Absyn_scanfarg_str={_tmp90,
_tmp90,_tmp90 + 9};static struct _tuple0 Cyc_Absyn_datatype_print_arg_qvar_p={{.Abs_n={
2,0}},& Cyc_Absyn_printarg_str};static struct _tuple0 Cyc_Absyn_datatype_scanf_arg_qvar_p={{.Abs_n={
2,0}},& Cyc_Absyn_scanfarg_str};struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar=&
Cyc_Absyn_datatype_print_arg_qvar_p;struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar=&
Cyc_Absyn_datatype_scanf_arg_qvar_p;static char _tmp91[14]="unique_region";static
struct _dyneither_ptr Cyc_Absyn_uniquergn_str={_tmp91,_tmp91,_tmp91 + 14};static
char _tmp92[5]="Core";static struct _dyneither_ptr Cyc_Absyn_Core_str={_tmp92,_tmp92,
_tmp92 + 5};static struct Cyc_List_List Cyc_Absyn_uniquergn_nmspace={(void*)((struct
_dyneither_ptr*)& Cyc_Absyn_Core_str),(struct Cyc_List_List*)0};static struct
_tuple0 Cyc_Absyn_uniquergn_qvar_p={{.Abs_n={2,(struct Cyc_List_List*)& Cyc_Absyn_uniquergn_nmspace}},&
Cyc_Absyn_uniquergn_str};struct _tuple0*Cyc_Absyn_uniquergn_qvar=& Cyc_Absyn_uniquergn_qvar_p;
static struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct Cyc_Absyn_uniquergn_typ={15,(
void*)((void*)& Cyc_Absyn_UniqueRgn_val)};static struct Cyc_Absyn_Vardecl Cyc_Absyn_uniquergn_var_d={
Cyc_Absyn_Extern,& Cyc_Absyn_uniquergn_qvar_p,{0,0,0,0,0},(void*)& Cyc_Absyn_uniquergn_typ,
0,0,0,0};static struct Cyc_Absyn_Global_b_Absyn_Binding_struct Cyc_Absyn_uniquergn_bind_p={
1,& Cyc_Absyn_uniquergn_var_d};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Absyn_uniquergn_exp_r={1,& Cyc_Absyn_uniquergn_qvar_p,(void*)((void*)& Cyc_Absyn_uniquergn_bind_p)};
static struct Cyc_Absyn_Exp Cyc_Absyn_uniquergn_exp_p={(void*)& Cyc_Absyn_uniquergn_typ,(
void*)& Cyc_Absyn_uniquergn_exp_r,0,(void*)& Cyc_Absyn_EmptyAnnot_val};struct Cyc_Absyn_Exp*
Cyc_Absyn_uniquergn_exp=& Cyc_Absyn_uniquergn_exp_p;static void**Cyc_Absyn_string_t_opt=
0;void*Cyc_Absyn_string_typ(void*rgn){void*_tmp96=Cyc_Tcutil_compress(rgn);_LL63: {
struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp97=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)
_tmp96;if(_tmp97->tag != 20)goto _LL65;}_LL64: if(Cyc_Absyn_string_t_opt == 0){void*
t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(
0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);void**_tmp25D;Cyc_Absyn_string_t_opt=((
_tmp25D=_cycalloc(sizeof(*_tmp25D)),((_tmp25D[0]=t,_tmp25D))));}return*((void**)
_check_null(Cyc_Absyn_string_t_opt));_LL65:;_LL66: return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,
rgn,Cyc_Absyn_empty_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
_LL62:;}static void**Cyc_Absyn_const_string_t_opt=0;void*Cyc_Absyn_const_string_typ(
void*rgn){void*_tmp99=Cyc_Tcutil_compress(rgn);_LL68: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*
_tmp9A=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp99;if(_tmp9A->tag != 20)
goto _LL6A;}_LL69: if(Cyc_Absyn_const_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)&
Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);void**_tmp25E;Cyc_Absyn_const_string_t_opt=((
_tmp25E=_cycalloc(sizeof(*_tmp25E)),((_tmp25E[0]=t,_tmp25E))));}return*((void**)
_check_null(Cyc_Absyn_const_string_t_opt));_LL6A:;_LL6B: return Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);_LL67:;}void*Cyc_Absyn_starb_typ(void*t,void*r,struct Cyc_Absyn_Tqual
tq,void*b,union Cyc_Absyn_Constraint*zeroterm){struct Cyc_Absyn_PointerType_Absyn_Type_struct
_tmp268;struct Cyc_Absyn_PtrAtts _tmp267;struct Cyc_Absyn_PtrInfo _tmp266;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp265;return(void*)((_tmp265=_cycalloc(sizeof(*_tmp265)),((_tmp265[0]=((
_tmp268.tag=5,((_tmp268.f1=((_tmp266.elt_typ=t,((_tmp266.elt_tq=tq,((_tmp266.ptr_atts=((
_tmp267.rgn=r,((_tmp267.nullable=Cyc_Absyn_true_conref,((_tmp267.bounds=((union
Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),((_tmp267.zero_term=
zeroterm,((_tmp267.ptrloc=0,_tmp267)))))))))),_tmp266)))))),_tmp268)))),_tmp265))));}
void*Cyc_Absyn_atb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zeroterm){struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp272;struct Cyc_Absyn_PtrAtts
_tmp271;struct Cyc_Absyn_PtrInfo _tmp270;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp26F;return(void*)((_tmp26F=_cycalloc(sizeof(*_tmp26F)),((_tmp26F[0]=((
_tmp272.tag=5,((_tmp272.f1=((_tmp270.elt_typ=t,((_tmp270.elt_tq=tq,((_tmp270.ptr_atts=((
_tmp271.rgn=r,((_tmp271.nullable=Cyc_Absyn_false_conref,((_tmp271.bounds=((union
Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),((_tmp271.zero_term=
zeroterm,((_tmp271.ptrloc=0,_tmp271)))))))))),_tmp270)))))),_tmp272)))),_tmp26F))));}
void*Cyc_Absyn_star_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zeroterm){struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp27C;struct Cyc_Absyn_PtrAtts
_tmp27B;struct Cyc_Absyn_PtrInfo _tmp27A;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp279;return(void*)((_tmp279=_cycalloc(sizeof(*_tmp279)),((_tmp279[0]=((
_tmp27C.tag=5,((_tmp27C.f1=((_tmp27A.elt_typ=t,((_tmp27A.elt_tq=tq,((_tmp27A.ptr_atts=((
_tmp27B.rgn=r,((_tmp27B.nullable=Cyc_Absyn_true_conref,((_tmp27B.bounds=Cyc_Absyn_bounds_one_conref,((
_tmp27B.zero_term=zeroterm,((_tmp27B.ptrloc=0,_tmp27B)))))))))),_tmp27A)))))),
_tmp27C)))),_tmp279))));}void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp286;struct Cyc_Absyn_PtrAtts
_tmp285;struct Cyc_Absyn_PtrInfo _tmp284;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp283;return(void*)((_tmp283=_cycalloc(sizeof(*_tmp283)),((_tmp283[0]=((
_tmp286.tag=5,((_tmp286.f1=((_tmp284.elt_typ=t,((_tmp284.elt_tq=tq,((_tmp284.ptr_atts=((
_tmp285.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((_tmp285.nullable=Cyc_Absyn_true_conref,((
_tmp285.bounds=Cyc_Absyn_bounds_one_conref,((_tmp285.zero_term=Cyc_Absyn_false_conref,((
_tmp285.ptrloc=0,_tmp285)))))))))),_tmp284)))))),_tmp286)))),_tmp283))));}void*
Cyc_Absyn_at_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zeroterm){struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp290;struct Cyc_Absyn_PtrAtts
_tmp28F;struct Cyc_Absyn_PtrInfo _tmp28E;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp28D;return(void*)((_tmp28D=_cycalloc(sizeof(*_tmp28D)),((_tmp28D[0]=((
_tmp290.tag=5,((_tmp290.f1=((_tmp28E.elt_typ=t,((_tmp28E.elt_tq=tq,((_tmp28E.ptr_atts=((
_tmp28F.rgn=r,((_tmp28F.nullable=Cyc_Absyn_false_conref,((_tmp28F.bounds=Cyc_Absyn_bounds_one_conref,((
_tmp28F.zero_term=zeroterm,((_tmp28F.ptrloc=0,_tmp28F)))))))))),_tmp28E)))))),
_tmp290)))),_tmp28D))));}void*Cyc_Absyn_dyneither_typ(void*t,void*r,struct Cyc_Absyn_Tqual
tq,union Cyc_Absyn_Constraint*zeroterm){struct Cyc_Absyn_PointerType_Absyn_Type_struct
_tmp29A;struct Cyc_Absyn_PtrAtts _tmp299;struct Cyc_Absyn_PtrInfo _tmp298;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp297;return(void*)((_tmp297=_cycalloc(sizeof(*_tmp297)),((_tmp297[0]=((
_tmp29A.tag=5,((_tmp29A.f1=((_tmp298.elt_typ=t,((_tmp298.elt_tq=tq,((_tmp298.ptr_atts=((
_tmp299.rgn=r,((_tmp299.nullable=Cyc_Absyn_true_conref,((_tmp299.bounds=Cyc_Absyn_bounds_dyneither_conref,((
_tmp299.zero_term=zeroterm,((_tmp299.ptrloc=0,_tmp299)))))))))),_tmp298)))))),
_tmp29A)))),_tmp297))));}void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual
tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int
ztloc){struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp2A0;struct Cyc_Absyn_ArrayInfo
_tmp29F;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp29E;return(void*)((
_tmp29E=_cycalloc(sizeof(*_tmp29E)),((_tmp29E[0]=((_tmp2A0.tag=8,((_tmp2A0.f1=((
_tmp29F.elt_type=elt_type,((_tmp29F.tq=tq,((_tmp29F.num_elts=num_elts,((_tmp29F.zero_term=
zero_term,((_tmp29F.zt_loc=ztloc,_tmp29F)))))))))),_tmp2A0)))),_tmp29E))));}
static char _tmpBE[8]="__sFILE";void*Cyc_Absyn_file_typ(){static void**file_t_opt=0;
static struct _dyneither_ptr sf_str={_tmpBE,_tmpBE,_tmpBE + 8};static struct
_dyneither_ptr*sf=& sf_str;if(file_t_opt == 0){struct _tuple0*_tmp2A1;struct _tuple0*
file_t_name=(_tmp2A1=_cycalloc(sizeof(*_tmp2A1)),((_tmp2A1->f1=Cyc_Absyn_Abs_n(0,
0),((_tmp2A1->f2=sf,_tmp2A1)))));struct Cyc_Absyn_Aggrdecl*_tmp2A2;struct Cyc_Absyn_Aggrdecl*
sd=(_tmp2A2=_cycalloc(sizeof(*_tmp2A2)),((_tmp2A2->kind=Cyc_Absyn_StructA,((
_tmp2A2->sc=Cyc_Absyn_Abstract,((_tmp2A2->name=file_t_name,((_tmp2A2->tvs=0,((
_tmp2A2->impl=0,((_tmp2A2->attributes=0,_tmp2A2)))))))))))));struct Cyc_Absyn_AggrType_Absyn_Type_struct
_tmp2AC;struct Cyc_Absyn_Aggrdecl**_tmp2AB;struct Cyc_Absyn_AggrInfo _tmp2AA;struct
Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2A9;void*file_struct_typ=(void*)((
_tmp2A9=_cycalloc(sizeof(*_tmp2A9)),((_tmp2A9[0]=((_tmp2AC.tag=11,((_tmp2AC.f1=((
_tmp2AA.aggr_info=Cyc_Absyn_KnownAggr(((_tmp2AB=_cycalloc(sizeof(*_tmp2AB)),((
_tmp2AB[0]=sd,_tmp2AB))))),((_tmp2AA.targs=0,_tmp2AA)))),_tmp2AC)))),_tmp2A9))));
void**_tmp2AD;file_t_opt=((_tmp2AD=_cycalloc(sizeof(*_tmp2AD)),((_tmp2AD[0]=Cyc_Absyn_at_typ(
file_struct_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),
_tmp2AD))));}return*file_t_opt;}void*Cyc_Absyn_void_star_typ(){static void**
void_star_t_opt=0;if(void_star_t_opt == 0){void**_tmp2AE;void_star_t_opt=((
_tmp2AE=_cycalloc(sizeof(*_tmp2AE)),((_tmp2AE[0]=Cyc_Absyn_star_typ((void*)& Cyc_Absyn_VoidType_val,(
void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),
_tmp2AE))));}return*void_star_t_opt;}static struct Cyc_Absyn_JoinEff_Absyn_Type_struct
Cyc_Absyn_empty_eff={24,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;
void*Cyc_Absyn_aggr_typ(enum Cyc_Absyn_AggrKind k,struct _dyneither_ptr*name){
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp2B8;struct _tuple0*_tmp2B7;struct
Cyc_Absyn_AggrInfo _tmp2B6;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2B5;
return(void*)((_tmp2B5=_cycalloc(sizeof(*_tmp2B5)),((_tmp2B5[0]=((_tmp2B8.tag=11,((
_tmp2B8.f1=((_tmp2B6.aggr_info=Cyc_Absyn_UnknownAggr(k,((_tmp2B7=_cycalloc(
sizeof(*_tmp2B7)),((_tmp2B7->f1=Cyc_Absyn_rel_ns_null,((_tmp2B7->f2=name,_tmp2B7)))))),
0),((_tmp2B6.targs=0,_tmp2B6)))),_tmp2B8)))),_tmp2B5))));}void*Cyc_Absyn_strct(
struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ(Cyc_Absyn_StructA,name);}
void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ(Cyc_Absyn_UnionA,
name);}void*Cyc_Absyn_strctq(struct _tuple0*name){struct Cyc_Absyn_AggrType_Absyn_Type_struct
_tmp2BE;struct Cyc_Absyn_AggrInfo _tmp2BD;struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp2BC;return(void*)((_tmp2BC=_cycalloc(sizeof(*_tmp2BC)),((_tmp2BC[0]=((
_tmp2BE.tag=11,((_tmp2BE.f1=((_tmp2BD.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,
name,0),((_tmp2BD.targs=0,_tmp2BD)))),_tmp2BE)))),_tmp2BC))));}void*Cyc_Absyn_unionq_typ(
struct _tuple0*name){struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp2C4;struct Cyc_Absyn_AggrInfo
_tmp2C3;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2C2;return(void*)((
_tmp2C2=_cycalloc(sizeof(*_tmp2C2)),((_tmp2C2[0]=((_tmp2C4.tag=11,((_tmp2C4.f1=((
_tmp2C3.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_UnionA,name,0),((_tmp2C3.targs=
0,_tmp2C3)))),_tmp2C4)))),_tmp2C2))));}union Cyc_Absyn_Cnst Cyc_Absyn_Null_c={.Null_c={
1,0}};union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c){struct
_tuple3 _tmp2C7;union Cyc_Absyn_Cnst _tmp2C6;return((_tmp2C6.Char_c).val=((_tmp2C7.f1=
sn,((_tmp2C7.f2=c,_tmp2C7)))),(((_tmp2C6.Char_c).tag=2,_tmp2C6)));}union Cyc_Absyn_Cnst
Cyc_Absyn_Wchar_c(struct _dyneither_ptr s){union Cyc_Absyn_Cnst _tmp2C8;return((
_tmp2C8.Wchar_c).val=s,(((_tmp2C8.Wchar_c).tag=3,_tmp2C8)));}union Cyc_Absyn_Cnst
Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign sn,short s){struct _tuple4 _tmp2CB;union Cyc_Absyn_Cnst
_tmp2CA;return((_tmp2CA.Short_c).val=((_tmp2CB.f1=sn,((_tmp2CB.f2=s,_tmp2CB)))),(((
_tmp2CA.Short_c).tag=4,_tmp2CA)));}union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign
sn,int i){struct _tuple5 _tmp2CE;union Cyc_Absyn_Cnst _tmp2CD;return((_tmp2CD.Int_c).val=((
_tmp2CE.f1=sn,((_tmp2CE.f2=i,_tmp2CE)))),(((_tmp2CD.Int_c).tag=5,_tmp2CD)));}
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign sn,long long l){struct
_tuple6 _tmp2D1;union Cyc_Absyn_Cnst _tmp2D0;return((_tmp2D0.LongLong_c).val=((
_tmp2D1.f1=sn,((_tmp2D1.f2=l,_tmp2D1)))),(((_tmp2D0.LongLong_c).tag=6,_tmp2D0)));}
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr s,int i){struct _tuple7
_tmp2D4;union Cyc_Absyn_Cnst _tmp2D3;return((_tmp2D3.Float_c).val=((_tmp2D4.f1=s,((
_tmp2D4.f2=i,_tmp2D4)))),(((_tmp2D3.Float_c).tag=7,_tmp2D3)));}union Cyc_Absyn_Cnst
Cyc_Absyn_String_c(struct _dyneither_ptr s){union Cyc_Absyn_Cnst _tmp2D5;return((
_tmp2D5.String_c).val=s,(((_tmp2D5.String_c).tag=8,_tmp2D5)));}union Cyc_Absyn_Cnst
Cyc_Absyn_Wstring_c(struct _dyneither_ptr s){union Cyc_Absyn_Cnst _tmp2D6;return((
_tmp2D6.Wstring_c).val=s,(((_tmp2D6.Wstring_c).tag=9,_tmp2D6)));}struct Cyc_Absyn_Exp*
Cyc_Absyn_new_exp(void*r,unsigned int loc){struct Cyc_Absyn_Exp*_tmp2D7;return(
_tmp2D7=_cycalloc(sizeof(*_tmp2D7)),((_tmp2D7->topt=0,((_tmp2D7->r=r,((_tmp2D7->loc=
loc,((_tmp2D7->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,_tmp2D7)))))))));}struct
Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*
e,unsigned int loc){struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct _tmp2DA;struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*
_tmp2D9;return Cyc_Absyn_new_exp((void*)((_tmp2D9=_cycalloc(sizeof(*_tmp2D9)),((
_tmp2D9[0]=((_tmp2DA.tag=15,((_tmp2DA.f1=rgn_handle,((_tmp2DA.f2=e,_tmp2DA)))))),
_tmp2D9)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*_tmp2DB;return(_tmp2DB=_cycalloc(sizeof(*_tmp2DB)),((_tmp2DB[
0]=*e,_tmp2DB)));}struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,
unsigned int loc){struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp2DE;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp2DD;return Cyc_Absyn_new_exp((void*)((_tmp2DD=_cycalloc(sizeof(*_tmp2DD)),((
_tmp2DD[0]=((_tmp2DE.tag=0,((_tmp2DE.f1=c,_tmp2DE)))),_tmp2DD)))),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int loc){static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct
null_const={0,{.Null_c={1,0}}};return Cyc_Absyn_new_exp((void*)& null_const,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign s,int i,unsigned int seg){
return Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(s,i),seg);}struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int i,unsigned int loc){static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0,{.Int_c={
5,{Cyc_Absyn_Signed,0}}}};static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={
0,{.Int_c={5,{Cyc_Absyn_Signed,1}}}};if(i == 0)return Cyc_Absyn_new_exp((void*)&
szero,loc);else{if(i == 1)return Cyc_Absyn_new_exp((void*)& sone,loc);}return Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,
unsigned int loc){static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0,{.Int_c={
5,{Cyc_Absyn_Unsigned,0}}}};static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct
uone={0,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};if(i == 0)return Cyc_Absyn_new_exp((
void*)& uzero,loc);else{if(i == 1)return Cyc_Absyn_new_exp((void*)& uone,loc);else{
return Cyc_Absyn_int_exp(Cyc_Absyn_Unsigned,(int)i,loc);}}}struct Cyc_Absyn_Exp*
Cyc_Absyn_bool_exp(int b,unsigned int loc){return Cyc_Absyn_signed_int_exp(b?1: 0,
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int loc){return Cyc_Absyn_bool_exp(
1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int loc){return Cyc_Absyn_bool_exp(
0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int loc){return Cyc_Absyn_const_exp(
Cyc_Absyn_Char_c(Cyc_Absyn_None,c),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(
struct _dyneither_ptr s,unsigned int loc){return Cyc_Absyn_const_exp(Cyc_Absyn_Wchar_c(
s),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,
unsigned int loc){return Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(f,i),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int loc){return
Cyc_Absyn_const_exp(Cyc_Absyn_String_c(s),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(
struct _dyneither_ptr s,unsigned int loc){return Cyc_Absyn_const_exp(Cyc_Absyn_Wstring_c(
s),loc);}struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val={
0};struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,unsigned int loc){struct
Cyc_Absyn_Var_e_Absyn_Raw_exp_struct _tmp2E1;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp2E0;return Cyc_Absyn_new_exp((void*)((_tmp2E0=_cycalloc(sizeof(*_tmp2E0)),((
_tmp2E0[0]=((_tmp2E1.tag=1,((_tmp2E1.f1=q,((_tmp2E1.f2=(void*)((void*)& Cyc_Absyn_Unresolved_b_val),
_tmp2E1)))))),_tmp2E0)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct
_tuple0*q,void*b,unsigned int loc){struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
_tmp2E4;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E3;return Cyc_Absyn_new_exp((
void*)((_tmp2E3=_cycalloc(sizeof(*_tmp2E3)),((_tmp2E3[0]=((_tmp2E4.tag=1,((
_tmp2E4.f1=q,((_tmp2E4.f2=(void*)b,_tmp2E4)))))),_tmp2E3)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_unknownid_exp(struct _tuple0*q,unsigned int loc){struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
_tmp2E7;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E6;return Cyc_Absyn_new_exp((
void*)((_tmp2E6=_cycalloc(sizeof(*_tmp2E6)),((_tmp2E6[0]=((_tmp2E7.tag=1,((
_tmp2E7.f1=q,((_tmp2E7.f2=(void*)((void*)& Cyc_Absyn_Unresolved_b_val),_tmp2E7)))))),
_tmp2E6)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop
p,struct Cyc_List_List*es,unsigned int loc){struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct
_tmp2EA;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp2E9;return Cyc_Absyn_new_exp((
void*)((_tmp2E9=_cycalloc(sizeof(*_tmp2E9)),((_tmp2E9[0]=((_tmp2EA.tag=2,((
_tmp2EA.f1=p,((_tmp2EA.f2=es,_tmp2EA)))))),_tmp2E9)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e,unsigned int loc){
struct Cyc_List_List*_tmp2EB;return Cyc_Absyn_primop_exp(p,((_tmp2EB=_cycalloc(
sizeof(*_tmp2EB)),((_tmp2EB->hd=e,((_tmp2EB->tl=0,_tmp2EB)))))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,unsigned int loc){struct Cyc_List_List*_tmp2EE;struct Cyc_List_List*_tmp2ED;
return Cyc_Absyn_primop_exp(p,((_tmp2ED=_cycalloc(sizeof(*_tmp2ED)),((_tmp2ED->hd=
e1,((_tmp2ED->tl=((_tmp2EE=_cycalloc(sizeof(*_tmp2EE)),((_tmp2EE->hd=e2,((
_tmp2EE->tl=0,_tmp2EE)))))),_tmp2ED)))))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct
_tmp2F1;struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp2F0;return Cyc_Absyn_new_exp((
void*)((_tmp2F0=_cycalloc(sizeof(*_tmp2F0)),((_tmp2F0[0]=((_tmp2F1.tag=33,((
_tmp2F1.f1=e1,((_tmp2F1.f2=e2,_tmp2F1)))))),_tmp2F0)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(
Cyc_Absyn_Times,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,
e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,e1,e2,
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,e1,e2,loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,
unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gt,e1,e2,loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lt,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(
Cyc_Absyn_Gte,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lte,
e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*e1,
struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,unsigned int loc){struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct
_tmp2F4;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp2F3;return Cyc_Absyn_new_exp((
void*)((_tmp2F3=_cycalloc(sizeof(*_tmp2F3)),((_tmp2F3[0]=((_tmp2F4.tag=3,((
_tmp2F4.f1=e1,((_tmp2F4.f2=popt,((_tmp2F4.f3=e2,_tmp2F4)))))))),_tmp2F3)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,unsigned int loc){return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i,
unsigned int loc){struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp2F7;struct
Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp2F6;return Cyc_Absyn_new_exp((void*)((
_tmp2F6=_cycalloc(sizeof(*_tmp2F6)),((_tmp2F6[0]=((_tmp2F7.tag=4,((_tmp2F7.f1=e,((
_tmp2F7.f2=i,_tmp2F7)))))),_tmp2F6)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostInc,
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*e,
unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreInc,loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreDec,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostDec,
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,unsigned int loc){struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct
_tmp2FA;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp2F9;return Cyc_Absyn_new_exp((
void*)((_tmp2F9=_cycalloc(sizeof(*_tmp2F9)),((_tmp2F9[0]=((_tmp2FA.tag=5,((
_tmp2FA.f1=e1,((_tmp2FA.f2=e2,((_tmp2FA.f3=e3,_tmp2FA)))))))),_tmp2F9)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,unsigned int loc){struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp2FD;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*
_tmp2FC;return Cyc_Absyn_new_exp((void*)((_tmp2FC=_cycalloc(sizeof(*_tmp2FC)),((
_tmp2FC[0]=((_tmp2FD.tag=6,((_tmp2FD.f1=e1,((_tmp2FD.f2=e2,_tmp2FD)))))),_tmp2FC)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,unsigned int loc){struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp300;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*
_tmp2FF;return Cyc_Absyn_new_exp((void*)((_tmp2FF=_cycalloc(sizeof(*_tmp2FF)),((
_tmp2FF[0]=((_tmp300.tag=7,((_tmp300.f1=e1,((_tmp300.f2=e2,_tmp300)))))),_tmp2FF)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,unsigned int loc){struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp303;struct
Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp302;return Cyc_Absyn_new_exp((void*)((
_tmp302=_cycalloc(sizeof(*_tmp302)),((_tmp302[0]=((_tmp303.tag=8,((_tmp303.f1=e1,((
_tmp303.f2=e2,_tmp303)))))),_tmp302)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned int loc){struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct
_tmp306;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp305;return Cyc_Absyn_new_exp((
void*)((_tmp305=_cycalloc(sizeof(*_tmp305)),((_tmp305[0]=((_tmp306.tag=9,((
_tmp306.f1=e,((_tmp306.f2=es,((_tmp306.f3=0,((_tmp306.f4=0,_tmp306)))))))))),
_tmp305)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es,unsigned int loc){struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct
_tmp309;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp308;return Cyc_Absyn_new_exp((
void*)((_tmp308=_cycalloc(sizeof(*_tmp308)),((_tmp308[0]=((_tmp309.tag=9,((
_tmp309.f1=e,((_tmp309.f2=es,((_tmp309.f3=0,((_tmp309.f4=1,_tmp309)))))))))),
_tmp308)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*
e,unsigned int loc){struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct _tmp30C;
struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp30B;return Cyc_Absyn_new_exp((
void*)((_tmp30B=_cycalloc(sizeof(*_tmp30B)),((_tmp30B[0]=((_tmp30C.tag=11,((
_tmp30C.f1=e,_tmp30C)))),_tmp30B)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,unsigned int loc){struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct
_tmp30F;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp30E;return Cyc_Absyn_new_exp((
void*)((_tmp30E=_cycalloc(sizeof(*_tmp30E)),((_tmp30E[0]=((_tmp30F.tag=12,((
_tmp30F.f1=e,((_tmp30F.f2=ts,_tmp30F)))))),_tmp30E)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,enum Cyc_Absyn_Coercion
c,unsigned int loc){struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp312;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*
_tmp311;return Cyc_Absyn_new_exp((void*)((_tmp311=_cycalloc(sizeof(*_tmp311)),((
_tmp311[0]=((_tmp312.tag=13,((_tmp312.f1=(void*)t,((_tmp312.f2=e,((_tmp312.f3=
user_cast,((_tmp312.f4=c,_tmp312)))))))))),_tmp311)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct
_tmp315;struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp314;return Cyc_Absyn_new_exp((
void*)((_tmp314=_cycalloc(sizeof(*_tmp314)),((_tmp314[0]=((_tmp315.tag=10,((
_tmp315.f1=e,_tmp315)))),_tmp314)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*e,unsigned int loc){struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct
_tmp318;struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp317;return Cyc_Absyn_new_exp((
void*)((_tmp317=_cycalloc(sizeof(*_tmp317)),((_tmp317[0]=((_tmp318.tag=14,((
_tmp318.f1=e,_tmp318)))),_tmp317)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,unsigned int loc){struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp31B;
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp31A;return Cyc_Absyn_new_exp((
void*)((_tmp31A=_cycalloc(sizeof(*_tmp31A)),((_tmp31A[0]=((_tmp31B.tag=16,((
_tmp31B.f1=(void*)t,_tmp31B)))),_tmp31A)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,unsigned int loc){struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct
_tmp31E;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp31D;return Cyc_Absyn_new_exp((
void*)((_tmp31D=_cycalloc(sizeof(*_tmp31D)),((_tmp31D[0]=((_tmp31E.tag=17,((
_tmp31E.f1=e,_tmp31E)))),_tmp31D)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(
void*t,void*of,unsigned int loc){struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct
_tmp321;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp320;return Cyc_Absyn_new_exp((
void*)((_tmp320=_cycalloc(sizeof(*_tmp320)),((_tmp320[0]=((_tmp321.tag=18,((
_tmp321.f1=(void*)t,((_tmp321.f2=(void*)of,_tmp321)))))),_tmp320)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp324;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*
_tmp323;return Cyc_Absyn_new_exp((void*)((_tmp323=_cycalloc(sizeof(*_tmp323)),((
_tmp323[0]=((_tmp324.tag=19,((_tmp324.f1=e,_tmp324)))),_tmp323)))),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*n,unsigned int loc){struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct
_tmp327;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp326;return Cyc_Absyn_new_exp((
void*)((_tmp326=_cycalloc(sizeof(*_tmp326)),((_tmp326[0]=((_tmp327.tag=20,((
_tmp327.f1=e,((_tmp327.f2=n,((_tmp327.f3=0,((_tmp327.f4=0,_tmp327)))))))))),
_tmp326)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr*n,unsigned int loc){struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct
_tmp32A;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp329;return Cyc_Absyn_new_exp((
void*)((_tmp329=_cycalloc(sizeof(*_tmp329)),((_tmp329[0]=((_tmp32A.tag=21,((
_tmp32A.f1=e,((_tmp32A.f2=n,((_tmp32A.f3=0,((_tmp32A.f4=0,_tmp32A)))))))))),
_tmp329)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct
_tmp32D;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp32C;return Cyc_Absyn_new_exp((
void*)((_tmp32C=_cycalloc(sizeof(*_tmp32C)),((_tmp32C[0]=((_tmp32D.tag=22,((
_tmp32D.f1=e1,((_tmp32D.f2=e2,_tmp32D)))))),_tmp32C)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,unsigned int loc){struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct
_tmp330;struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp32F;return Cyc_Absyn_new_exp((
void*)((_tmp32F=_cycalloc(sizeof(*_tmp32F)),((_tmp32F[0]=((_tmp330.tag=23,((
_tmp330.f1=es,_tmp330)))),_tmp32F)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*s,unsigned int loc){struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct
_tmp333;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp332;return Cyc_Absyn_new_exp((
void*)((_tmp332=_cycalloc(sizeof(*_tmp332)),((_tmp332[0]=((_tmp333.tag=35,((
_tmp333.f1=s,_tmp333)))),_tmp332)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(
unsigned int loc){return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*t,unsigned int loc){struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct
_tmp336;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp335;return Cyc_Absyn_new_exp((
void*)((_tmp335=_cycalloc(sizeof(*_tmp335)),((_tmp335[0]=((_tmp336.tag=37,((
_tmp336.f1=(void*)t,_tmp336)))),_tmp335)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(
int volatile_kw,struct _dyneither_ptr body,unsigned int loc){struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct
_tmp339;struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp338;return Cyc_Absyn_new_exp((
void*)((_tmp338=_cycalloc(sizeof(*_tmp338)),((_tmp338[0]=((_tmp339.tag=38,((
_tmp339.f1=volatile_kw,((_tmp339.f2=body,_tmp339)))))),_tmp338)))),loc);}struct
_tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(
struct Cyc_List_List*es,unsigned int loc){struct Cyc_List_List*dles=0;for(0;es != 0;
es=es->tl){struct _tuple13*_tmp33C;struct Cyc_List_List*_tmp33B;dles=((_tmp33B=
_cycalloc(sizeof(*_tmp33B)),((_tmp33B->hd=((_tmp33C=_cycalloc(sizeof(*_tmp33C)),((
_tmp33C->f1=0,((_tmp33C->f2=(struct Cyc_Absyn_Exp*)es->hd,_tmp33C)))))),((_tmp33B->tl=
dles,_tmp33B))))));}dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles);{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp33F;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*
_tmp33E;return Cyc_Absyn_new_exp((void*)((_tmp33E=_cycalloc(sizeof(*_tmp33E)),((
_tmp33E[0]=((_tmp33F.tag=25,((_tmp33F.f1=dles,_tmp33F)))),_tmp33E)))),loc);};}
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*
dles,unsigned int loc){struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct
_tmp342;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp341;return Cyc_Absyn_new_exp((
void*)((_tmp341=_cycalloc(sizeof(*_tmp341)),((_tmp341[0]=((_tmp342.tag=34,((
_tmp342.f1=n,((_tmp342.f2=dles,_tmp342)))))),_tmp341)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,unsigned int loc){struct Cyc_Absyn_Stmt*_tmp343;return(
_tmp343=_cycalloc(sizeof(*_tmp343)),((_tmp343->r=s,((_tmp343->loc=loc,((_tmp343->non_local_preds=
0,((_tmp343->try_depth=0,((_tmp343->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,
_tmp343)))))))))));}struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val={
0};struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc){return Cyc_Absyn_new_stmt((
void*)& Cyc_Absyn_Skip_s_val,loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct
Cyc_Absyn_Exp*e,unsigned int loc){struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct
_tmp346;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp345;return Cyc_Absyn_new_stmt((
void*)((_tmp345=_cycalloc(sizeof(*_tmp345)),((_tmp345[0]=((_tmp346.tag=1,((
_tmp346.f1=e,_tmp346)))),_tmp345)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(
struct Cyc_List_List*ss,unsigned int loc){if(ss == 0)return Cyc_Absyn_skip_stmt(loc);
else{if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{return Cyc_Absyn_seq_stmt((
struct Cyc_Absyn_Stmt*)ss->hd,Cyc_Absyn_seq_stmts(ss->tl,loc),loc);}}}struct Cyc_Absyn_Stmt*
Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc){struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct
_tmp349;struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp348;return Cyc_Absyn_new_stmt((
void*)((_tmp348=_cycalloc(sizeof(*_tmp348)),((_tmp348[0]=((_tmp349.tag=3,((
_tmp349.f1=e,_tmp349)))),_tmp348)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,
unsigned int loc){struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct _tmp34C;
struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp34B;return Cyc_Absyn_new_stmt((
void*)((_tmp34B=_cycalloc(sizeof(*_tmp34B)),((_tmp34B[0]=((_tmp34C.tag=4,((
_tmp34C.f1=e,((_tmp34C.f2=s1,((_tmp34C.f3=s2,_tmp34C)))))))),_tmp34B)))),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*
s,unsigned int loc){struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct _tmp352;struct
_tuple9 _tmp351;struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp350;return Cyc_Absyn_new_stmt((
void*)((_tmp350=_cycalloc(sizeof(*_tmp350)),((_tmp350[0]=((_tmp352.tag=5,((
_tmp352.f1=((_tmp351.f1=e,((_tmp351.f2=Cyc_Absyn_skip_stmt(e->loc),_tmp351)))),((
_tmp352.f2=s,_tmp352)))))),_tmp350)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(
unsigned int loc){struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct _tmp355;struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*
_tmp354;return Cyc_Absyn_new_stmt((void*)((_tmp354=_cycalloc(sizeof(*_tmp354)),((
_tmp354[0]=((_tmp355.tag=6,((_tmp355.f1=0,_tmp355)))),_tmp354)))),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc){struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct
_tmp358;struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp357;return Cyc_Absyn_new_stmt((
void*)((_tmp357=_cycalloc(sizeof(*_tmp357)),((_tmp357[0]=((_tmp358.tag=7,((
_tmp358.f1=0,_tmp358)))),_tmp357)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*
s,unsigned int loc){struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct _tmp361;struct
_tuple9 _tmp360;struct _tuple9 _tmp35F;struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*
_tmp35E;return Cyc_Absyn_new_stmt((void*)((_tmp35E=_cycalloc(sizeof(*_tmp35E)),((
_tmp35E[0]=((_tmp361.tag=9,((_tmp361.f1=e1,((_tmp361.f2=((_tmp360.f1=e2,((
_tmp360.f2=Cyc_Absyn_skip_stmt(e3->loc),_tmp360)))),((_tmp361.f3=((_tmp35F.f1=e3,((
_tmp35F.f2=Cyc_Absyn_skip_stmt(e3->loc),_tmp35F)))),((_tmp361.f4=s,_tmp361)))))))))),
_tmp35E)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*scs,unsigned int loc){struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct
_tmp364;struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp363;return Cyc_Absyn_new_stmt((
void*)((_tmp363=_cycalloc(sizeof(*_tmp363)),((_tmp363[0]=((_tmp364.tag=10,((
_tmp364.f1=e,((_tmp364.f2=scs,_tmp364)))))),_tmp363)))),loc);}struct _tuple14{
void*f1;void*f2;};struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*
s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc){struct _tuple14 _tmp365;struct _tuple14
_tmp13D=(_tmp365.f1=s1->r,((_tmp365.f2=s2->r,_tmp365)));void*_tmp13E;void*
_tmp140;_LL6D: _tmp13E=_tmp13D.f1;{struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*
_tmp13F=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp13E;if(_tmp13F->tag != 
0)goto _LL6F;};_LL6E: return s2;_LL6F: _tmp140=_tmp13D.f2;{struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*
_tmp141=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp140;if(_tmp141->tag != 
0)goto _LL71;};_LL70: return s1;_LL71:;_LL72: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct
_tmp368;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp367;return Cyc_Absyn_new_stmt((
void*)((_tmp367=_cycalloc(sizeof(*_tmp367)),((_tmp367[0]=((_tmp368.tag=2,((
_tmp368.f1=s1,((_tmp368.f2=s2,_tmp368)))))),_tmp367)))),loc);}_LL6C:;}struct Cyc_Absyn_Stmt*
Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc){struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct
_tmp36B;struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp36A;return Cyc_Absyn_new_stmt((
void*)((_tmp36A=_cycalloc(sizeof(*_tmp36A)),((_tmp36A[0]=((_tmp36B.tag=11,((
_tmp36B.f1=el,((_tmp36B.f2=0,_tmp36B)))))),_tmp36A)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int
loc){struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp36E;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*
_tmp36D;return Cyc_Absyn_new_stmt((void*)((_tmp36D=_cycalloc(sizeof(*_tmp36D)),((
_tmp36D[0]=((_tmp36E.tag=12,((_tmp36E.f1=d,((_tmp36E.f2=s,_tmp36E)))))),_tmp36D)))),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct
Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*s,unsigned int loc){struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct
_tmp371;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp370;struct Cyc_Absyn_Decl*
d=Cyc_Absyn_new_decl((void*)((_tmp370=_cycalloc(sizeof(*_tmp370)),((_tmp370[0]=((
_tmp371.tag=0,((_tmp371.f1=Cyc_Absyn_new_vardecl(x,t,init),_tmp371)))),_tmp370)))),
loc);struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp374;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*
_tmp373;return Cyc_Absyn_new_stmt((void*)((_tmp373=_cycalloc(sizeof(*_tmp373)),((
_tmp373[0]=((_tmp374.tag=12,((_tmp374.f1=d,((_tmp374.f2=s,_tmp374)))))),_tmp373)))),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*
s,unsigned int loc){struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmp377;struct
Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp376;return Cyc_Absyn_new_stmt((void*)((
_tmp376=_cycalloc(sizeof(*_tmp376)),((_tmp376[0]=((_tmp377.tag=13,((_tmp377.f1=v,((
_tmp377.f2=s,_tmp377)))))),_tmp376)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc){struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct
_tmp37D;struct _tuple9 _tmp37C;struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp37B;
return Cyc_Absyn_new_stmt((void*)((_tmp37B=_cycalloc(sizeof(*_tmp37B)),((_tmp37B[
0]=((_tmp37D.tag=14,((_tmp37D.f1=s,((_tmp37D.f2=((_tmp37C.f1=e,((_tmp37C.f2=Cyc_Absyn_skip_stmt(
e->loc),_tmp37C)))),_tmp37D)))))),_tmp37B)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,unsigned int loc){struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct
_tmp380;struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp37F;return Cyc_Absyn_new_stmt((
void*)((_tmp37F=_cycalloc(sizeof(*_tmp37F)),((_tmp37F[0]=((_tmp380.tag=15,((
_tmp380.f1=s,((_tmp380.f2=scs,_tmp380)))))),_tmp37F)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc){struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct
_tmp383;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp382;return Cyc_Absyn_new_stmt((
void*)((_tmp382=_cycalloc(sizeof(*_tmp382)),((_tmp382[0]=((_tmp383.tag=8,((
_tmp383.f1=lab,((_tmp383.f2=0,_tmp383)))))),_tmp382)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int
loc){return Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(e1,e2,loc),loc);}struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat(void*p,unsigned int s){struct Cyc_Absyn_Pat*_tmp384;return(
_tmp384=_cycalloc(sizeof(*_tmp384)),((_tmp384->r=p,((_tmp384->topt=0,((_tmp384->loc=
s,_tmp384)))))));}struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmp387;struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*
_tmp386;return Cyc_Absyn_new_pat((void*)((_tmp386=_cycalloc(sizeof(*_tmp386)),((
_tmp386[0]=((_tmp387.tag=16,((_tmp387.f1=e,_tmp387)))),_tmp386)))),e->loc);}
struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val={0};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct
Cyc_Absyn_Null_p_val={8};struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,
unsigned int loc){struct Cyc_Absyn_Decl*_tmp388;return(_tmp388=_cycalloc(sizeof(*
_tmp388)),((_tmp388->r=r,((_tmp388->loc=loc,_tmp388)))));}struct Cyc_Absyn_Decl*
Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc){
struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct _tmp38B;struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*
_tmp38A;return Cyc_Absyn_new_decl((void*)((_tmp38A=_cycalloc(sizeof(*_tmp38A)),((
_tmp38A[0]=((_tmp38B.tag=2,((_tmp38B.f1=p,((_tmp38B.f2=0,((_tmp38B.f3=e,_tmp38B)))))))),
_tmp38A)))),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*
vds,unsigned int loc){struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct _tmp38E;struct
Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp38D;return Cyc_Absyn_new_decl((void*)((
_tmp38D=_cycalloc(sizeof(*_tmp38D)),((_tmp38D[0]=((_tmp38E.tag=3,((_tmp38E.f1=
vds,_tmp38E)))),_tmp38D)))),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(
struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,int resetable,struct Cyc_Absyn_Exp*
eo,unsigned int loc){struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct _tmp391;struct
Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp390;return Cyc_Absyn_new_decl((void*)((
_tmp390=_cycalloc(sizeof(*_tmp390)),((_tmp390[0]=((_tmp391.tag=4,((_tmp391.f1=tv,((
_tmp391.f2=vd,((_tmp391.f3=resetable,((_tmp391.f4=eo,_tmp391)))))))))),_tmp390)))),
loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Tvar*
tv,struct Cyc_Absyn_Vardecl*vd,unsigned int loc){struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct
_tmp394;struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp393;return Cyc_Absyn_new_decl((
void*)((_tmp393=_cycalloc(sizeof(*_tmp393)),((_tmp393[0]=((_tmp394.tag=5,((
_tmp394.f1=e,((_tmp394.f2=tv,((_tmp394.f3=vd,_tmp394)))))))),_tmp393)))),loc);}
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init){struct Cyc_Absyn_Vardecl*_tmp395;return(_tmp395=_cycalloc(sizeof(*_tmp395)),((
_tmp395->sc=Cyc_Absyn_Public,((_tmp395->name=x,((_tmp395->tq=Cyc_Absyn_empty_tqual(
0),((_tmp395->type=t,((_tmp395->initializer=init,((_tmp395->rgn=0,((_tmp395->attributes=
0,((_tmp395->escapes=0,_tmp395)))))))))))))))));}struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){struct Cyc_Absyn_Vardecl*_tmp396;
return(_tmp396=_cycalloc(sizeof(*_tmp396)),((_tmp396->sc=Cyc_Absyn_Static,((
_tmp396->name=x,((_tmp396->tq=Cyc_Absyn_empty_tqual(0),((_tmp396->type=t,((
_tmp396->initializer=init,((_tmp396->rgn=0,((_tmp396->attributes=0,((_tmp396->escapes=
0,_tmp396)))))))))))))))));}struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(
struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int
tagged){struct Cyc_Absyn_AggrdeclImpl*_tmp397;return(_tmp397=_cycalloc(sizeof(*
_tmp397)),((_tmp397->exist_vars=exists,((_tmp397->rgn_po=po,((_tmp397->fields=fs,((
_tmp397->tagged=tagged,_tmp397)))))))));}struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(
enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp39D;struct Cyc_Absyn_Aggrdecl*
_tmp39C;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp39B;return Cyc_Absyn_new_decl((
void*)((_tmp39B=_cycalloc(sizeof(*_tmp39B)),((_tmp39B[0]=((_tmp39D.tag=6,((
_tmp39D.f1=((_tmp39C=_cycalloc(sizeof(*_tmp39C)),((_tmp39C->kind=k,((_tmp39C->sc=
s,((_tmp39C->name=n,((_tmp39C->tvs=ts,((_tmp39C->impl=i,((_tmp39C->attributes=
atts,_tmp39C)))))))))))))),_tmp39D)))),_tmp39B)))),loc);}struct Cyc_Absyn_TypeDecl*
Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct
_tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,unsigned int loc){struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp3A7;
struct Cyc_Absyn_Aggrdecl*_tmp3A6;struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct
_tmp3A5;struct Cyc_Absyn_TypeDecl*_tmp3A4;return(_tmp3A4=_cycalloc(sizeof(*
_tmp3A4)),((_tmp3A4->r=(void*)((_tmp3A7=_cycalloc(sizeof(*_tmp3A7)),((_tmp3A7[0]=((
_tmp3A5.tag=0,((_tmp3A5.f1=((_tmp3A6=_cycalloc(sizeof(*_tmp3A6)),((_tmp3A6->kind=
k,((_tmp3A6->sc=s,((_tmp3A6->name=n,((_tmp3A6->tvs=ts,((_tmp3A6->impl=i,((
_tmp3A6->attributes=atts,_tmp3A6)))))))))))))),_tmp3A5)))),_tmp3A7)))),((_tmp3A4->loc=
loc,_tmp3A4)))));}struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope
s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct
Cyc_List_List*atts,unsigned int loc){return Cyc_Absyn_aggr_decl(Cyc_Absyn_StructA,
s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope
s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct
Cyc_List_List*atts,unsigned int loc){return Cyc_Absyn_aggr_decl(Cyc_Absyn_UnionA,s,
n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope
s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,
unsigned int loc){struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp3AD;struct
Cyc_Absyn_Datatypedecl*_tmp3AC;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*
_tmp3AB;return Cyc_Absyn_new_decl((void*)((_tmp3AB=_cycalloc(sizeof(*_tmp3AB)),((
_tmp3AB[0]=((_tmp3AD.tag=7,((_tmp3AD.f1=((_tmp3AC=_cycalloc(sizeof(*_tmp3AC)),((
_tmp3AC->sc=s,((_tmp3AC->name=n,((_tmp3AC->tvs=ts,((_tmp3AC->fields=fs,((_tmp3AC->is_extensible=
is_extensible,_tmp3AC)))))))))))),_tmp3AD)))),_tmp3AB)))),loc);}struct Cyc_Absyn_TypeDecl*
Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc){struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*
_tmp3B7;struct Cyc_Absyn_Datatypedecl*_tmp3B6;struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct
_tmp3B5;struct Cyc_Absyn_TypeDecl*_tmp3B4;return(_tmp3B4=_cycalloc(sizeof(*
_tmp3B4)),((_tmp3B4->r=(void*)((_tmp3B7=_cycalloc(sizeof(*_tmp3B7)),((_tmp3B7[0]=((
_tmp3B5.tag=2,((_tmp3B5.f1=((_tmp3B6=_cycalloc(sizeof(*_tmp3B6)),((_tmp3B6->sc=s,((
_tmp3B6->name=n,((_tmp3B6->tvs=ts,((_tmp3B6->fields=fs,((_tmp3B6->is_extensible=
is_extensible,_tmp3B6)))))))))))),_tmp3B5)))),_tmp3B7)))),((_tmp3B4->loc=loc,
_tmp3B4)))));}static void Cyc_Absyn_expand_arg(struct _tuple8*a){void*_tmp174=(*a).f3;
void*_tmp175=Cyc_Absyn_pointer_expand(_tmp174,1);if(_tmp174 != _tmp175)(*a).f3=
_tmp175;}void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct
Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,
struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*
atts){((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Absyn_expand_arg,args);{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp3BD;
struct Cyc_Absyn_FnInfo _tmp3BC;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3BB;
return(void*)((_tmp3BB=_cycalloc(sizeof(*_tmp3BB)),((_tmp3BB[0]=((_tmp3BD.tag=9,((
_tmp3BD.f1=((_tmp3BC.tvars=tvs,((_tmp3BC.ret_tqual=ret_tqual,((_tmp3BC.ret_typ=
Cyc_Absyn_pointer_expand(ret_typ,0),((_tmp3BC.effect=eff_typ,((_tmp3BC.args=args,((
_tmp3BC.c_varargs=c_varargs,((_tmp3BC.cyc_varargs=cyc_varargs,((_tmp3BC.rgn_po=
rgn_po,((_tmp3BC.attributes=atts,_tmp3BC)))))))))))))))))),_tmp3BD)))),_tmp3BB))));};}
void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar){void*_tmp179=Cyc_Tcutil_compress(
t);_LL74: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp17A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)
_tmp179;if(_tmp17A->tag != 9)goto _LL76;}_LL75: {struct Cyc_Core_Opt*_tmp3BE;return
Cyc_Absyn_at_typ(t,fresh_evar?Cyc_Absyn_new_evar(((_tmp3BE=_cycalloc(sizeof(*
_tmp3BE)),((_tmp3BE->v=& Cyc_Tcutil_rk,_tmp3BE)))),0):(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);}_LL76:;_LL77: return t;_LL73:;}
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){void*_tmp17C=e->r;void*_tmp17E;
void*_tmp181;struct Cyc_Absyn_Vardecl*_tmp183;void*_tmp185;struct Cyc_Absyn_Vardecl*
_tmp187;struct Cyc_Absyn_Exp*_tmp18D;struct Cyc_Absyn_Exp*_tmp18F;struct Cyc_Absyn_Exp*
_tmp191;_LL79: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp17D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp17C;if(_tmp17D->tag != 1)goto _LL7B;else{_tmp17E=(void*)_tmp17D->f2;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*
_tmp17F=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp17E;if(_tmp17F->tag
!= 2)goto _LL7B;};}}_LL7A: return 0;_LL7B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp180=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17C;if(_tmp180->tag != 1)
goto _LL7D;else{_tmp181=(void*)_tmp180->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp182=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp181;if(_tmp182->tag
!= 1)goto _LL7D;else{_tmp183=_tmp182->f1;}};}}_LL7C: _tmp187=_tmp183;goto _LL7E;
_LL7D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp184=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp17C;if(_tmp184->tag != 1)goto _LL7F;else{_tmp185=(void*)_tmp184->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmp186=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp185;if(_tmp186->tag != 
4)goto _LL7F;else{_tmp187=_tmp186->f1;}};}}_LL7E: {void*_tmp192=Cyc_Tcutil_compress(
_tmp187->type);_LL90: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp193=(
struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp192;if(_tmp193->tag != 8)goto
_LL92;}_LL91: return 0;_LL92:;_LL93: return 1;_LL8F:;}_LL7F: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp188=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17C;if(_tmp188->tag != 1)
goto _LL81;}_LL80: goto _LL82;_LL81: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*
_tmp189=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp17C;if(_tmp189->tag
!= 21)goto _LL83;}_LL82: goto _LL84;_LL83: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*
_tmp18A=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp17C;if(_tmp18A->tag != 
19)goto _LL85;}_LL84: goto _LL86;_LL85: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*
_tmp18B=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp17C;if(_tmp18B->tag
!= 22)goto _LL87;}_LL86: return 1;_LL87: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*
_tmp18C=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp17C;if(_tmp18C->tag
!= 20)goto _LL89;else{_tmp18D=_tmp18C->f1;}}_LL88: return Cyc_Absyn_is_lvalue(
_tmp18D);_LL89: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp18E=(
struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp17C;if(_tmp18E->tag != 12)
goto _LL8B;else{_tmp18F=_tmp18E->f1;}}_LL8A: return Cyc_Absyn_is_lvalue(_tmp18F);
_LL8B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp190=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)
_tmp17C;if(_tmp190->tag != 11)goto _LL8D;else{_tmp191=_tmp190->f1;}}_LL8C: return
Cyc_Absyn_is_lvalue(_tmp191);_LL8D:;_LL8E: return 0;_LL78:;}struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _dyneither_ptr*v){{
struct Cyc_List_List*_tmp194=fields;for(0;_tmp194 != 0;_tmp194=_tmp194->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Aggrfield*)_tmp194->hd)->name,v)== 0)return(struct Cyc_Absyn_Aggrfield*)((
struct Cyc_Absyn_Aggrfield*)_tmp194->hd);}}return 0;}struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*v){return ad->impl == 0?0: Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple10*
Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){for(0;i != 0;-- i){if(ts
== 0)return 0;ts=ts->tl;}if(ts == 0)return 0;return(struct _tuple10*)((struct
_tuple10*)ts->hd);}struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val={
1};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val={2};
struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val={3};
struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val={4};
struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val={5};
struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val={7};
struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct Cyc_Absyn_Nocommon_att_val={9};
struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val={10};
struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val={11};
struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val={12};
struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val={
13};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val={
14};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val={
15};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val={
16};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val={
17};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val={
18};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val={22};
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*a){void*_tmp1A6=a;int _tmp1A8;
int _tmp1AF;struct _dyneither_ptr _tmp1B2;enum Cyc_Absyn_Format_Type _tmp1BE;int
_tmp1BF;int _tmp1C0;enum Cyc_Absyn_Format_Type _tmp1C2;int _tmp1C3;int _tmp1C4;int
_tmp1C6;int _tmp1C8;struct _dyneither_ptr _tmp1CB;_LL95: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*
_tmp1A7=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1A7->tag
!= 0)goto _LL97;else{_tmp1A8=_tmp1A7->f1;}}_LL96: {const char*_tmp3C2;void*_tmp3C1[
1];struct Cyc_Int_pa_PrintArg_struct _tmp3C0;return(struct _dyneither_ptr)((_tmp3C0.tag=
1,((_tmp3C0.f1=(unsigned long)_tmp1A8,((_tmp3C1[0]=& _tmp3C0,Cyc_aprintf(((
_tmp3C2="regparm(%d)",_tag_dyneither(_tmp3C2,sizeof(char),12))),_tag_dyneither(
_tmp3C1,sizeof(void*),1))))))));}_LL97: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*
_tmp1A9=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1A9->tag
!= 1)goto _LL99;}_LL98: {const char*_tmp3C3;return(_tmp3C3="stdcall",
_tag_dyneither(_tmp3C3,sizeof(char),8));}_LL99: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*
_tmp1AA=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1AA->tag
!= 2)goto _LL9B;}_LL9A: {const char*_tmp3C4;return(_tmp3C4="cdecl",_tag_dyneither(
_tmp3C4,sizeof(char),6));}_LL9B: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*
_tmp1AB=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1AB->tag
!= 3)goto _LL9D;}_LL9C: {const char*_tmp3C5;return(_tmp3C5="fastcall",
_tag_dyneither(_tmp3C5,sizeof(char),9));}_LL9D: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*
_tmp1AC=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1AC->tag
!= 4)goto _LL9F;}_LL9E: {const char*_tmp3C6;return(_tmp3C6="noreturn",
_tag_dyneither(_tmp3C6,sizeof(char),9));}_LL9F: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*
_tmp1AD=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1AD->tag
!= 5)goto _LLA1;}_LLA0: {const char*_tmp3C7;return(_tmp3C7="const",_tag_dyneither(
_tmp3C7,sizeof(char),6));}_LLA1: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*
_tmp1AE=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1AE->tag
!= 6)goto _LLA3;else{_tmp1AF=_tmp1AE->f1;}}_LLA2: if(_tmp1AF == - 1){const char*
_tmp3C8;return(_tmp3C8="aligned",_tag_dyneither(_tmp3C8,sizeof(char),8));}else{
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLC7: {const char*_tmp3CC;
void*_tmp3CB[1];struct Cyc_Int_pa_PrintArg_struct _tmp3CA;return(struct
_dyneither_ptr)((_tmp3CA.tag=1,((_tmp3CA.f1=(unsigned long)_tmp1AF,((_tmp3CB[0]=&
_tmp3CA,Cyc_aprintf(((_tmp3CC="aligned(%d)",_tag_dyneither(_tmp3CC,sizeof(char),
12))),_tag_dyneither(_tmp3CB,sizeof(void*),1))))))));}case Cyc_Cyclone_Vc_c: _LLC8: {
const char*_tmp3D0;void*_tmp3CF[1];struct Cyc_Int_pa_PrintArg_struct _tmp3CE;return(
struct _dyneither_ptr)((_tmp3CE.tag=1,((_tmp3CE.f1=(unsigned long)_tmp1AF,((
_tmp3CF[0]=& _tmp3CE,Cyc_aprintf(((_tmp3D0="align(%d)",_tag_dyneither(_tmp3D0,
sizeof(char),10))),_tag_dyneither(_tmp3CF,sizeof(void*),1))))))));}}}_LLA3: {
struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp1B0=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)
_tmp1A6;if(_tmp1B0->tag != 7)goto _LLA5;}_LLA4: {const char*_tmp3D1;return(_tmp3D1="packed",
_tag_dyneither(_tmp3D1,sizeof(char),7));}_LLA5: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*
_tmp1B1=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1B1->tag
!= 8)goto _LLA7;else{_tmp1B2=_tmp1B1->f1;}}_LLA6: {const char*_tmp3D5;void*_tmp3D4[
1];struct Cyc_String_pa_PrintArg_struct _tmp3D3;return(struct _dyneither_ptr)((
_tmp3D3.tag=0,((_tmp3D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1B2),((
_tmp3D4[0]=& _tmp3D3,Cyc_aprintf(((_tmp3D5="section(\"%s\")",_tag_dyneither(
_tmp3D5,sizeof(char),14))),_tag_dyneither(_tmp3D4,sizeof(void*),1))))))));}_LLA7: {
struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp1B3=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)
_tmp1A6;if(_tmp1B3->tag != 9)goto _LLA9;}_LLA8: {const char*_tmp3D6;return(_tmp3D6="nocommon",
_tag_dyneither(_tmp3D6,sizeof(char),9));}_LLA9: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*
_tmp1B4=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1B4->tag
!= 10)goto _LLAB;}_LLAA: {const char*_tmp3D7;return(_tmp3D7="shared",
_tag_dyneither(_tmp3D7,sizeof(char),7));}_LLAB: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*
_tmp1B5=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1B5->tag
!= 11)goto _LLAD;}_LLAC: {const char*_tmp3D8;return(_tmp3D8="unused",
_tag_dyneither(_tmp3D8,sizeof(char),7));}_LLAD: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*
_tmp1B6=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1B6->tag
!= 12)goto _LLAF;}_LLAE: {const char*_tmp3D9;return(_tmp3D9="weak",_tag_dyneither(
_tmp3D9,sizeof(char),5));}_LLAF: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*
_tmp1B7=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp1A6;if(
_tmp1B7->tag != 13)goto _LLB1;}_LLB0: {const char*_tmp3DA;return(_tmp3DA="dllimport",
_tag_dyneither(_tmp3DA,sizeof(char),10));}_LLB1: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*
_tmp1B8=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp1A6;if(
_tmp1B8->tag != 14)goto _LLB3;}_LLB2: {const char*_tmp3DB;return(_tmp3DB="dllexport",
_tag_dyneither(_tmp3DB,sizeof(char),10));}_LLB3: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*
_tmp1B9=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)
_tmp1A6;if(_tmp1B9->tag != 15)goto _LLB5;}_LLB4: {const char*_tmp3DC;return(_tmp3DC="no_instrument_function",
_tag_dyneither(_tmp3DC,sizeof(char),23));}_LLB5: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*
_tmp1BA=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp1A6;if(
_tmp1BA->tag != 16)goto _LLB7;}_LLB6: {const char*_tmp3DD;return(_tmp3DD="constructor",
_tag_dyneither(_tmp3DD,sizeof(char),12));}_LLB7: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*
_tmp1BB=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp1A6;if(
_tmp1BB->tag != 17)goto _LLB9;}_LLB8: {const char*_tmp3DE;return(_tmp3DE="destructor",
_tag_dyneither(_tmp3DE,sizeof(char),11));}_LLB9: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*
_tmp1BC=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)
_tmp1A6;if(_tmp1BC->tag != 18)goto _LLBB;}_LLBA: {const char*_tmp3DF;return(_tmp3DF="no_check_memory_usage",
_tag_dyneither(_tmp3DF,sizeof(char),22));}_LLBB: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*
_tmp1BD=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1BD->tag
!= 19)goto _LLBD;else{_tmp1BE=_tmp1BD->f1;if(_tmp1BE != Cyc_Absyn_Printf_ft)goto
_LLBD;_tmp1BF=_tmp1BD->f2;_tmp1C0=_tmp1BD->f3;}}_LLBC: {const char*_tmp3E4;void*
_tmp3E3[2];struct Cyc_Int_pa_PrintArg_struct _tmp3E2;struct Cyc_Int_pa_PrintArg_struct
_tmp3E1;return(struct _dyneither_ptr)((_tmp3E1.tag=1,((_tmp3E1.f1=(unsigned int)
_tmp1C0,((_tmp3E2.tag=1,((_tmp3E2.f1=(unsigned int)_tmp1BF,((_tmp3E3[0]=& _tmp3E2,((
_tmp3E3[1]=& _tmp3E1,Cyc_aprintf(((_tmp3E4="format(printf,%u,%u)",_tag_dyneither(
_tmp3E4,sizeof(char),21))),_tag_dyneither(_tmp3E3,sizeof(void*),2))))))))))))));}
_LLBD: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp1C1=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)
_tmp1A6;if(_tmp1C1->tag != 19)goto _LLBF;else{_tmp1C2=_tmp1C1->f1;if(_tmp1C2 != Cyc_Absyn_Scanf_ft)
goto _LLBF;_tmp1C3=_tmp1C1->f2;_tmp1C4=_tmp1C1->f3;}}_LLBE: {const char*_tmp3E9;
void*_tmp3E8[2];struct Cyc_Int_pa_PrintArg_struct _tmp3E7;struct Cyc_Int_pa_PrintArg_struct
_tmp3E6;return(struct _dyneither_ptr)((_tmp3E6.tag=1,((_tmp3E6.f1=(unsigned int)
_tmp1C4,((_tmp3E7.tag=1,((_tmp3E7.f1=(unsigned int)_tmp1C3,((_tmp3E8[0]=& _tmp3E7,((
_tmp3E8[1]=& _tmp3E6,Cyc_aprintf(((_tmp3E9="format(scanf,%u,%u)",_tag_dyneither(
_tmp3E9,sizeof(char),20))),_tag_dyneither(_tmp3E8,sizeof(void*),2))))))))))))));}
_LLBF: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp1C5=(struct
Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1C5->tag != 20)
goto _LLC1;else{_tmp1C6=_tmp1C5->f1;}}_LLC0: {const char*_tmp3ED;void*_tmp3EC[1];
struct Cyc_Int_pa_PrintArg_struct _tmp3EB;return(struct _dyneither_ptr)((_tmp3EB.tag=
1,((_tmp3EB.f1=(unsigned long)_tmp1C6,((_tmp3EC[0]=& _tmp3EB,Cyc_aprintf(((
_tmp3ED="initializes(%d)",_tag_dyneither(_tmp3ED,sizeof(char),16))),
_tag_dyneither(_tmp3EC,sizeof(void*),1))))))));}_LLC1: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*
_tmp1C7=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1A6;if(
_tmp1C7->tag != 21)goto _LLC3;else{_tmp1C8=_tmp1C7->f1;}}_LLC2: {const char*_tmp3F1;
void*_tmp3F0[1];struct Cyc_Int_pa_PrintArg_struct _tmp3EF;return(struct
_dyneither_ptr)((_tmp3EF.tag=1,((_tmp3EF.f1=(unsigned long)_tmp1C8,((_tmp3F0[0]=&
_tmp3EF,Cyc_aprintf(((_tmp3F1="noliveunique(%d)",_tag_dyneither(_tmp3F1,sizeof(
char),17))),_tag_dyneither(_tmp3F0,sizeof(void*),1))))))));}_LLC3: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*
_tmp1C9=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1C9->tag
!= 22)goto _LLC5;}_LLC4: {const char*_tmp3F2;return(_tmp3F2="pure",_tag_dyneither(
_tmp3F2,sizeof(char),5));}_LLC5: {struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*
_tmp1CA=(struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1CA->tag
!= 23)goto _LL94;else{_tmp1CB=_tmp1CA->f1;}}_LLC6: {const char*_tmp3F6;void*
_tmp3F5[1];struct Cyc_String_pa_PrintArg_struct _tmp3F4;return(struct
_dyneither_ptr)((_tmp3F4.tag=0,((_tmp3F4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp1CB),((_tmp3F5[0]=& _tmp3F4,Cyc_aprintf(((_tmp3F6="__mode__(\"%s\")",
_tag_dyneither(_tmp3F6,sizeof(char),15))),_tag_dyneither(_tmp3F5,sizeof(void*),1))))))));}
_LL94:;}int Cyc_Absyn_fntype_att(void*a){void*_tmp1FB=a;int _tmp1FD;_LLCB: {struct
Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp1FC=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)
_tmp1FB;if(_tmp1FC->tag != 0)goto _LLCD;else{_tmp1FD=_tmp1FC->f1;}}_LLCC: goto _LLCE;
_LLCD: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp1FE=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)
_tmp1FB;if(_tmp1FE->tag != 3)goto _LLCF;}_LLCE: goto _LLD0;_LLCF: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*
_tmp1FF=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp1FB;if(_tmp1FF->tag
!= 1)goto _LLD1;}_LLD0: goto _LLD2;_LLD1: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*
_tmp200=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp1FB;if(_tmp200->tag
!= 2)goto _LLD3;}_LLD2: goto _LLD4;_LLD3: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*
_tmp201=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp1FB;if(_tmp201->tag
!= 4)goto _LLD5;}_LLD4: goto _LLD6;_LLD5: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*
_tmp202=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp1FB;if(_tmp202->tag
!= 22)goto _LLD7;}_LLD6: goto _LLD8;_LLD7: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*
_tmp203=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1FB;if(_tmp203->tag
!= 19)goto _LLD9;}_LLD8: goto _LLDA;_LLD9: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*
_tmp204=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp1FB;if(_tmp204->tag
!= 5)goto _LLDB;}_LLDA: return 1;_LLDB: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*
_tmp205=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1FB;if(
_tmp205->tag != 21)goto _LLDD;}_LLDC: return 1;_LLDD: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*
_tmp206=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1FB;if(
_tmp206->tag != 20)goto _LLDF;}_LLDE: return 1;_LLDF:;_LLE0: return 0;_LLCA:;}static
char _tmp207[3]="f0";static struct _dyneither_ptr Cyc_Absyn_f0={_tmp207,_tmp207,
_tmp207 + 3};static struct _dyneither_ptr*Cyc_Absyn_field_names_v[1]={& Cyc_Absyn_f0};
static struct _dyneither_ptr Cyc_Absyn_field_names={(void*)((struct _dyneither_ptr**)
Cyc_Absyn_field_names_v),(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(
void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v + 1)};struct _dyneither_ptr*
Cyc_Absyn_fieldname(int i);static void _tmp404(unsigned int*fsz,unsigned int*
_tmp403,unsigned int*_tmp402,struct _dyneither_ptr***_tmp400){for(*_tmp403=0;*
_tmp403 < *_tmp402;(*_tmp403)++){struct Cyc_Int_pa_PrintArg_struct _tmp3FE;void*
_tmp3FD[1];const char*_tmp3FC;struct _dyneither_ptr*_tmp3FB;(*_tmp400)[*_tmp403]=*
_tmp403 < *fsz?*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,
sizeof(struct _dyneither_ptr*),(int)*_tmp403)):((_tmp3FB=_cycalloc(sizeof(*
_tmp3FB)),((_tmp3FB[0]=(struct _dyneither_ptr)((_tmp3FE.tag=1,((_tmp3FE.f1=(
unsigned long)((int)*_tmp403),((_tmp3FD[0]=& _tmp3FE,Cyc_aprintf(((_tmp3FC="f%d",
_tag_dyneither(_tmp3FC,sizeof(char),4))),_tag_dyneither(_tmp3FD,sizeof(void*),1)))))))),
_tmp3FB))));}}struct _dyneither_ptr*Cyc_Absyn_fieldname(int i){unsigned int fsz=
_get_dyneither_size(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*));if(i >= 
fsz){unsigned int _tmp403;unsigned int _tmp402;struct _dyneither_ptr _tmp401;struct
_dyneither_ptr**_tmp400;unsigned int _tmp3FF;Cyc_Absyn_field_names=((_tmp3FF=(
unsigned int)(i + 1),((_tmp400=(struct _dyneither_ptr**)_cycalloc(_check_times(
sizeof(struct _dyneither_ptr*),_tmp3FF)),((_tmp401=_tag_dyneither(_tmp400,sizeof(
struct _dyneither_ptr*),_tmp3FF),((((_tmp402=_tmp3FF,_tmp404(& fsz,& _tmp403,&
_tmp402,& _tmp400))),_tmp401))))))));}return*((struct _dyneither_ptr**)
_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),i));}
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU info){union Cyc_Absyn_AggrInfoU
_tmp210=info;struct _tuple2 _tmp211;enum Cyc_Absyn_AggrKind _tmp212;struct _tuple0*
_tmp213;struct Cyc_Absyn_Aggrdecl**_tmp214;struct Cyc_Absyn_Aggrdecl*_tmp215;
struct Cyc_Absyn_Aggrdecl _tmp216;enum Cyc_Absyn_AggrKind _tmp217;struct _tuple0*
_tmp218;_LLE2: if((_tmp210.UnknownAggr).tag != 1)goto _LLE4;_tmp211=(struct _tuple2)(
_tmp210.UnknownAggr).val;_tmp212=_tmp211.f1;_tmp213=_tmp211.f2;_LLE3: {struct
_tuple11 _tmp405;return(_tmp405.f1=_tmp212,((_tmp405.f2=_tmp213,_tmp405)));}_LLE4:
if((_tmp210.KnownAggr).tag != 2)goto _LLE1;_tmp214=(struct Cyc_Absyn_Aggrdecl**)(
_tmp210.KnownAggr).val;_tmp215=*_tmp214;_tmp216=*_tmp215;_tmp217=_tmp216.kind;
_tmp218=_tmp216.name;_LLE5: {struct _tuple11 _tmp406;return(_tmp406.f1=_tmp217,((
_tmp406.f2=_tmp218,_tmp406)));}_LLE1:;}struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info){union Cyc_Absyn_AggrInfoU _tmp21B=info;struct _tuple2
_tmp21C;struct Cyc_Absyn_Aggrdecl**_tmp21D;struct Cyc_Absyn_Aggrdecl*_tmp21E;_LLE7:
if((_tmp21B.UnknownAggr).tag != 1)goto _LLE9;_tmp21C=(struct _tuple2)(_tmp21B.UnknownAggr).val;
_LLE8: {const char*_tmp409;void*_tmp408;(_tmp408=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp409="unchecked aggrdecl",
_tag_dyneither(_tmp409,sizeof(char),19))),_tag_dyneither(_tmp408,sizeof(void*),0)));}
_LLE9: if((_tmp21B.KnownAggr).tag != 2)goto _LLE6;_tmp21D=(struct Cyc_Absyn_Aggrdecl**)(
_tmp21B.KnownAggr).val;_tmp21E=*_tmp21D;_LLEA: return _tmp21E;_LLE6:;}int Cyc_Absyn_is_union_type(
void*t){void*_tmp221=Cyc_Tcutil_compress(t);enum Cyc_Absyn_AggrKind _tmp223;
struct Cyc_Absyn_AggrInfo _tmp225;union Cyc_Absyn_AggrInfoU _tmp226;_LLEC: {struct
Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp222=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)
_tmp221;if(_tmp222->tag != 12)goto _LLEE;else{_tmp223=_tmp222->f1;if(_tmp223 != Cyc_Absyn_UnionA)
goto _LLEE;}}_LLED: return 1;_LLEE: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp224=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp221;if(_tmp224->tag != 
11)goto _LLF0;else{_tmp225=_tmp224->f1;_tmp226=_tmp225.aggr_info;}}_LLEF: return(
Cyc_Absyn_aggr_kinded_name(_tmp226)).f1 == Cyc_Absyn_UnionA;_LLF0:;_LLF1: return 0;
_LLEB:;}int Cyc_Absyn_is_nontagged_union_type(void*t){void*_tmp227=Cyc_Tcutil_compress(
t);enum Cyc_Absyn_AggrKind _tmp229;struct Cyc_Absyn_AggrInfo _tmp22B;union Cyc_Absyn_AggrInfoU
_tmp22C;_LLF3: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp228=(struct
Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp227;if(_tmp228->tag != 12)goto _LLF5;
else{_tmp229=_tmp228->f1;if(_tmp229 != Cyc_Absyn_UnionA)goto _LLF5;}}_LLF4: return 1;
_LLF5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp22A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp227;if(_tmp22A->tag != 11)goto _LLF7;else{_tmp22B=_tmp22A->f1;_tmp22C=_tmp22B.aggr_info;}}
_LLF6: {union Cyc_Absyn_AggrInfoU _tmp22D=_tmp22C;struct Cyc_Absyn_Aggrdecl**
_tmp22E;struct Cyc_Absyn_Aggrdecl*_tmp22F;struct _tuple2 _tmp230;enum Cyc_Absyn_AggrKind
_tmp231;struct Cyc_Core_Opt*_tmp232;struct _tuple2 _tmp233;enum Cyc_Absyn_AggrKind
_tmp234;struct Cyc_Core_Opt*_tmp235;struct Cyc_Core_Opt _tmp236;int _tmp237;_LLFA:
if((_tmp22D.KnownAggr).tag != 2)goto _LLFC;_tmp22E=(struct Cyc_Absyn_Aggrdecl**)(
_tmp22D.KnownAggr).val;_tmp22F=*_tmp22E;_LLFB: return _tmp22F->kind == Cyc_Absyn_UnionA
 && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp22F->impl))->tagged == 0;
_LLFC: if((_tmp22D.UnknownAggr).tag != 1)goto _LLFE;_tmp230=(struct _tuple2)(_tmp22D.UnknownAggr).val;
_tmp231=_tmp230.f1;_tmp232=_tmp230.f3;if(_tmp232 != 0)goto _LLFE;_LLFD: return
_tmp231 == Cyc_Absyn_UnionA;_LLFE: if((_tmp22D.UnknownAggr).tag != 1)goto _LLF9;
_tmp233=(struct _tuple2)(_tmp22D.UnknownAggr).val;_tmp234=_tmp233.f1;_tmp235=
_tmp233.f3;if(_tmp235 == 0)goto _LLF9;_tmp236=*_tmp235;_tmp237=(int)_tmp236.v;
_LLFF: return _tmp234 == Cyc_Absyn_UnionA  && !_tmp237;_LLF9:;}_LLF7:;_LLF8: return 0;
_LLF2:;}int Cyc_Absyn_is_aggr_type(void*t){void*_tmp238=Cyc_Tcutil_compress(t);
_LL101: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp239=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)
_tmp238;if(_tmp239->tag != 3)goto _LL103;}_LL102: goto _LL104;_LL103: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmp23A=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp238;if(_tmp23A->tag
!= 4)goto _LL105;}_LL104: goto _LL106;_LL105: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmp23B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp238;if(_tmp23B->tag != 
10)goto _LL107;}_LL106: goto _LL108;_LL107: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp23C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp238;if(_tmp23C->tag != 
11)goto _LL109;}_LL108: goto _LL10A;_LL109: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmp23D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp238;if(_tmp23D->tag
!= 12)goto _LL10B;}_LL10A: return 1;_LL10B:;_LL10C: return 0;_LL100:;}struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct
Cyc_Absyn_Porton_d_val={14};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val={
15};int Cyc_Absyn_porting_c_code=0;int Cyc_Absyn_no_regions=0;
