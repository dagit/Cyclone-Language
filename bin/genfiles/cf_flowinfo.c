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
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;
struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};
struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};typedef void*Cyc_parg_t;
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);int
Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};
struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;
double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{
int tag;struct _dyneither_ptr f1;};typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[
15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[
14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};typedef
unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t;int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[
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
int Cyc_List_length(struct Cyc_List_List*x);void*Cyc_List_hd(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_tl(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{
char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern
char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};int Cyc_List_exists_c(
int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);int Cyc_List_mem(int(*
compare)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_List_List*
Cyc_List_rfilter_c(struct _RegionHandle*r,int(*f)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};typedef struct
Cyc_Iter_Iter Cyc_Iter_iter_t;int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct
Cyc_Set_Set;typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];
struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;typedef const struct
Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;const struct Cyc_Dict_T*t;};typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;
extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct
Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));int Cyc_Dict_member(
struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void Cyc_Dict_iter(
void(*f)(void*,void*),struct Cyc_Dict_Dict d);void Cyc_Dict_iter_c(void(*f)(void*,
void*,void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_union_two_c(
void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2);struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),
void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);int Cyc_Dict_forall_intersect(
int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct
_tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict d);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*
rgn,struct Cyc_Dict_Dict d);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct
_dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{
int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{
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
_tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};typedef struct _tuple1*
Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;typedef struct _tuple1*
Cyc_Absyn_typedef_name_t;typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
_tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int
tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;
struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};
struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct
Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};
struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*
f1;};typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;
unsigned int loc;};typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct
Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int
tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};
struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;
struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;
struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{
int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;
void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{
int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};extern struct Cyc_Absyn_VoidType_Absyn_Type_struct
Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;
struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{
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
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{
int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};
struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts
f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int
tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{
int tag;unsigned int f1;struct Cyc_List_List*f2;};typedef void*Cyc_Absyn_type_modifier_t;
struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;
char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct
_union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};
struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct
_union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;
struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct
_dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct
_union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};enum Cyc_Absyn_Primop{
Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod
 = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte
 = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand
 = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift
 = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{
Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec
 = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*
injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{
int tag;unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,
Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion
 = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**
elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;
struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;
enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;
enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{
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
int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct
_tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};
struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{
int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;
};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*
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
struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{
int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*
f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int
tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;
struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{
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
int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int
tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;
unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;
struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;
};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual
ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;
struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;
struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind
kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct
_tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};
struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{
struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{
enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{
int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{
void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{
int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{
char*tag;};extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);struct Cyc_RgnOrder_RgnPO;typedef struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_rgn_po_t;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct
_RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct
Cyc_Absyn_Tvar*fst_rgn,unsigned int);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int
loc);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int
Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*
r);int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*
constraints,void*default_bound,int do_pin);int Cyc_RgnOrder_eff_outlives_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);void Cyc_RgnOrder_print_region_po(
struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*
tl;};typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;typedef const struct Cyc_Tcenv_CList*
const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*
f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*
f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{
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
struct _dyneither_ptr ap);void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t,struct Cyc_Absyn_Kind*def);void*Cyc_Tcutil_compress(void*t);extern struct Cyc_Absyn_Kind
Cyc_Tcutil_bk;void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,
void*);int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_bits_only(void*t);
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;typedef struct Cyc_PP_Out*
Cyc_PP_out_t;struct Cyc_PP_Doc;typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{
int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int
rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;
int print_using_stmts;int print_externC_stmts;int print_full_evars;int
print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{
int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{
int tag;int f1;void*f2;};typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 
1,Cyc_CfFlowInfo_AllIL  = 2};typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;
struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple11{struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple11 val;};struct
_union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessConst{int tag;unsigned int val;};struct
_union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};typedef union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_reln_op_t;
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(unsigned int);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*,void*);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessConst(unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(
struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(
unsigned int);struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp
rop;};typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;typedef struct Cyc_List_List*
Cyc_CfFlowInfo_relns_t;char Cyc_CfFlowInfo_IsZero[7]="IsZero";struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{
char*tag;};char Cyc_CfFlowInfo_NotZero[8]="NotZero";struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[9]="UnknownZ";
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*
f1;};extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;
struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct
_union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct
_union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};typedef union
Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(
struct Cyc_CfFlowInfo_Place*);union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
typedef void*Cyc_CfFlowInfo_absRval_t;typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{
int is_union;int fieldnum;};typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;
struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};
struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel
f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel
f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo
f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{
int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,
unsigned int loc);int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct
Cyc_Dict_Dict s2);int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct
Cyc_Dict_Dict s2);typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct Cyc_Dict_Dict
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int
disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple12{struct
Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int
tag;struct _tuple12 val;};union Cyc_CfFlowInfo_FlowInfo{struct
_union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_BottomFL();union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(
struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct
_RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;
void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;
struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);int Cyc_CfFlowInfo_get_field_index_fs(
struct Cyc_List_List*fs,struct _dyneither_ptr*f);int Cyc_CfFlowInfo_root_cmp(void*,
void*);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_List_List*,int no_init_bits_only,void*);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*);int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int
env_iteration,void*r,int*needs_unconsume);void*Cyc_CfFlowInfo_make_unique_unconsumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);void Cyc_CfFlowInfo_print_absrval(
void*rval);void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il);
void Cyc_CfFlowInfo_print_root(void*root);void Cyc_CfFlowInfo_print_place(struct
Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,
void(*pr)(void*));void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(
void*));void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);void Cyc_CfFlowInfo_print_flow(
union Cyc_CfFlowInfo_FlowInfo f);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct
Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct
_RegionHandle*,struct Cyc_List_List*);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,
struct Cyc_List_List*);void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,
struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
struct _tuple13{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,
struct _tuple13 pr2);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct
Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union
Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo
f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};
struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};extern struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct
Cyc_CfFlowInfo_UniqueRgn_k_val;extern struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct
Cyc_CfFlowInfo_RefCntRgn_k_val;typedef void*Cyc_CfFlowInfo_killrgn_t;int Cyc_CfFlowInfo_contains_region(
void*rgn,void*t);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*
place);struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val={
Cyc_CfFlowInfo_IsZero};struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct
Cyc_CfFlowInfo_UniqueRgn_k_val={0};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct
Cyc_CfFlowInfo_RefCntRgn_k_val={1};union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(
struct Cyc_CfFlowInfo_Place*x){union Cyc_CfFlowInfo_AbsLVal _tmp4A7;return((_tmp4A7.PlaceL).val=
x,(((_tmp4A7.PlaceL).tag=1,_tmp4A7)));}union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){
union Cyc_CfFlowInfo_AbsLVal _tmp4A8;return((_tmp4A8.UnknownL).val=0,(((_tmp4A8.UnknownL).tag=
2,_tmp4A8)));}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){union Cyc_CfFlowInfo_FlowInfo
_tmp4A9;return((_tmp4A9.BottomFL).val=0,(((_tmp4A9.BottomFL).tag=1,_tmp4A9)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,
struct Cyc_List_List*r){struct _tuple12 _tmp4AC;union Cyc_CfFlowInfo_FlowInfo _tmp4AB;
return((_tmp4AB.ReachableFL).val=((_tmp4AC.f1=fd,((_tmp4AC.f2=r,_tmp4AC)))),(((
_tmp4AB.ReachableFL).tag=2,_tmp4AB)));}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*
p1,struct Cyc_CfFlowInfo_Place*p2);int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0,{.Null_c={1,0}}};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_CfFlowInfo_Place*_tmp4E3;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct
_tmp4E2;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp4E1;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*
_tmp4E0;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp4DF;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*
_tmp4DE;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp4DD;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*
_tmp4DC;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp4DB;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*
_tmp4DA;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp4D9;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*
_tmp4D8;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp4D7;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*
_tmp4D6;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp4D5;struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*
_tmp4D4;struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct _tmp4D3;struct
Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4D2;struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct
_tmp4D1;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4D0;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct
_tmp4CF;struct Cyc_CfFlowInfo_FlowEnv*_tmp4CE;return(_tmp4CE=_region_malloc(r,
sizeof(*_tmp4CE)),((_tmp4CE->r=r,((_tmp4CE->zero=(void*)((_tmp4D0=_region_malloc(
r,sizeof(*_tmp4D0)),((_tmp4D0[0]=((_tmp4CF.tag=0,_tmp4CF)),_tmp4D0)))),((_tmp4CE->notzeroall=(
void*)((_tmp4D2=_region_malloc(r,sizeof(*_tmp4D2)),((_tmp4D2[0]=((_tmp4D1.tag=1,
_tmp4D1)),_tmp4D2)))),((_tmp4CE->notzerothis=(void*)((_tmp4D4=_region_malloc(r,
sizeof(*_tmp4D4)),((_tmp4D4[0]=((_tmp4D3.tag=2,_tmp4D3)),_tmp4D4)))),((_tmp4CE->unknown_none=(
void*)((_tmp4D6=_region_malloc(r,sizeof(*_tmp4D6)),((_tmp4D6[0]=((_tmp4D5.tag=3,((
_tmp4D5.f1=Cyc_CfFlowInfo_NoneIL,_tmp4D5)))),_tmp4D6)))),((_tmp4CE->unknown_this=(
void*)((_tmp4D8=_region_malloc(r,sizeof(*_tmp4D8)),((_tmp4D8[0]=((_tmp4D7.tag=3,((
_tmp4D7.f1=Cyc_CfFlowInfo_ThisIL,_tmp4D7)))),_tmp4D8)))),((_tmp4CE->unknown_all=(
void*)((_tmp4DA=_region_malloc(r,sizeof(*_tmp4DA)),((_tmp4DA[0]=((_tmp4D9.tag=3,((
_tmp4D9.f1=Cyc_CfFlowInfo_AllIL,_tmp4D9)))),_tmp4DA)))),((_tmp4CE->esc_none=(
void*)((_tmp4DC=_region_malloc(r,sizeof(*_tmp4DC)),((_tmp4DC[0]=((_tmp4DB.tag=4,((
_tmp4DB.f1=Cyc_CfFlowInfo_NoneIL,_tmp4DB)))),_tmp4DC)))),((_tmp4CE->esc_this=(
void*)((_tmp4DE=_region_malloc(r,sizeof(*_tmp4DE)),((_tmp4DE[0]=((_tmp4DD.tag=4,((
_tmp4DD.f1=Cyc_CfFlowInfo_ThisIL,_tmp4DD)))),_tmp4DE)))),((_tmp4CE->esc_all=(
void*)((_tmp4E0=_region_malloc(r,sizeof(*_tmp4E0)),((_tmp4E0[0]=((_tmp4DF.tag=4,((
_tmp4DF.f1=Cyc_CfFlowInfo_AllIL,_tmp4DF)))),_tmp4E0)))),((_tmp4CE->mt_flowdict=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(
r,Cyc_CfFlowInfo_root_cmp),((_tmp4CE->mt_place_set=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))
Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp4CE->dummy_place=((_tmp4E3=
_region_malloc(r,sizeof(*_tmp4E3)),((_tmp4E3->root=(void*)((_tmp4E1=
_region_malloc(r,sizeof(*_tmp4E1)),((_tmp4E1[0]=((_tmp4E2.tag=1,((_tmp4E2.f1=&
dummy_exp,((_tmp4E2.f2=(void*)((void*)& Cyc_Absyn_VoidType_val),_tmp4E2)))))),
_tmp4E1)))),((_tmp4E3->fields=0,_tmp4E3)))))),_tmp4CE)))))))))))))))))))))))))));}
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,
unsigned int loc){if(set != 0){if(((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(*
set,place))return 1;else{*set=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*
k,unsigned int v))Cyc_Dict_insert)(*set,place,loc);}}return 0;}struct
_dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
void*_tmp20;struct Cyc_List_List*_tmp21;struct Cyc_CfFlowInfo_Place _tmp1F=*place;
_tmp20=_tmp1F.root;_tmp21=_tmp1F.fields;{void*_tmp22=_tmp20;struct Cyc_Absyn_Vardecl*
_tmp24;_LL1: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp23=(struct
Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp22;if(_tmp23->tag != 0)goto
_LL3;else{_tmp24=_tmp23->f1;}}_LL2: if(_tmp21 == 0)return Cyc_Absynpp_qvar2string(
_tmp24->name);else{const char*_tmp4E7;void*_tmp4E6[1];struct Cyc_String_pa_PrintArg_struct
_tmp4E5;return(struct _dyneither_ptr)((_tmp4E5.tag=0,((_tmp4E5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp24->name)),((
_tmp4E6[0]=& _tmp4E5,Cyc_aprintf(((_tmp4E7="reachable from %s",_tag_dyneither(
_tmp4E7,sizeof(char),18))),_tag_dyneither(_tmp4E6,sizeof(void*),1))))))));}_LL3:;
_LL4: {const char*_tmp4EA;void*_tmp4E9;(_tmp4E9=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4EA="error locations not for VarRoots",
_tag_dyneither(_tmp4EA,sizeof(char),33))),_tag_dyneither(_tmp4E9,sizeof(void*),0)));}
_LL0:;};}static unsigned int Cyc_CfFlowInfo_combine_consume_f(void*env,void*place,
unsigned int loc1,unsigned int loc2){return loc1;}struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint){int _tmp2A=0;struct Cyc_Dict_Dict
_tmp2B=((struct Cyc_Dict_Dict(*)(unsigned int(*f)(int*,struct Cyc_CfFlowInfo_Place*,
unsigned int,unsigned int),int*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))
Cyc_Dict_union_two_c)((unsigned int(*)(int*env,struct Cyc_CfFlowInfo_Place*place,
unsigned int loc1,unsigned int loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp2A,s1,
s2);return _tmp2B;}struct _tuple14{void*f1;unsigned int f2;};int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){if((int)s1.t == (int)s2.t)return 1;{
struct _handler_cons _tmp2C;_push_handler(& _tmp2C);{int _tmp2E=0;if(setjmp(_tmp2C.handler))
_tmp2E=1;if(!_tmp2E){{struct _RegionHandle _tmp2F=_new_region("temp");struct
_RegionHandle*temp=& _tmp2F;_push_region(temp);{struct Cyc_Iter_Iter iter=((struct
Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
temp,s1);struct _tuple14 _tmp30=*((struct _tuple14*(*)(struct _RegionHandle*,struct
Cyc_Dict_Dict d))Cyc_Dict_rchoose)(temp,s1);while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple14*))Cyc_Iter_next)(iter,& _tmp30)){void*_tmp31=_tmp30.f1;if(!((int(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(s2,_tmp31)){int _tmp32=0;
_npop_handler(1);return _tmp32;}}{int _tmp33=1;_npop_handler(1);return _tmp33;};};
_pop_region(temp);};_pop_handler();}else{void*_tmp2D=(void*)_exn_thrown;void*
_tmp35=_tmp2D;_LL6:;_LL7: return 1;_LL8:;_LL9:(void)_throw(_tmp35);_LL5:;}};};}int
Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(
s2,s1);}int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct
_dyneither_ptr*f){int n=0;for(0;(unsigned int)fs;fs=fs->tl){struct _dyneither_ptr*
_tmp36=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;if(Cyc_strptrcmp(_tmp36,f)== 0)
return n;n=n + 1;}{struct Cyc_Core_Impossible_exn_struct _tmp4F7;const char*_tmp4F6;
void*_tmp4F5[1];struct Cyc_String_pa_PrintArg_struct _tmp4F4;struct Cyc_Core_Impossible_exn_struct*
_tmp4F3;(int)_throw((void*)((_tmp4F3=_cycalloc(sizeof(*_tmp4F3)),((_tmp4F3[0]=((
_tmp4F7.tag=Cyc_Core_Impossible,((_tmp4F7.f1=(struct _dyneither_ptr)((_tmp4F4.tag=
0,((_tmp4F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp4F5[0]=&
_tmp4F4,Cyc_aprintf(((_tmp4F6="get_field_index_fs failed: %s",_tag_dyneither(
_tmp4F6,sizeof(char),30))),_tag_dyneither(_tmp4F5,sizeof(void*),1)))))))),
_tmp4F7)))),_tmp4F3)))));};}int Cyc_CfFlowInfo_get_field_index(void*t,struct
_dyneither_ptr*f){void*_tmp3C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp3E;union Cyc_Absyn_AggrInfoU _tmp3F;struct Cyc_List_List*_tmp41;_LLB: {struct
Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp3C;if(_tmp3D->tag != 11)goto _LLD;else{_tmp3E=_tmp3D->f1;_tmp3F=_tmp3E.aggr_info;}}
_LLC: {struct Cyc_List_List*_tmp42=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((
Cyc_Absyn_get_known_aggrdecl(_tmp3F))->impl))->fields;_tmp41=_tmp42;goto _LLE;}
_LLD: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp40=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)
_tmp3C;if(_tmp40->tag != 12)goto _LLF;else{_tmp41=_tmp40->f2;}}_LLE: return Cyc_CfFlowInfo_get_field_index_fs(
_tmp41,f);_LLF:;_LL10: {struct Cyc_Core_Impossible_exn_struct _tmp504;const char*
_tmp503;void*_tmp502[1];struct Cyc_String_pa_PrintArg_struct _tmp501;struct Cyc_Core_Impossible_exn_struct*
_tmp500;(int)_throw((void*)((_tmp500=_cycalloc(sizeof(*_tmp500)),((_tmp500[0]=((
_tmp504.tag=Cyc_Core_Impossible,((_tmp504.f1=(struct _dyneither_ptr)((_tmp501.tag=
0,((_tmp501.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp502[0]=& _tmp501,Cyc_aprintf(((_tmp503="get_field_index failed: %s",
_tag_dyneither(_tmp503,sizeof(char),27))),_tag_dyneither(_tmp502,sizeof(void*),1)))))))),
_tmp504)))),_tmp500)))));}_LLA:;}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((
int)r1 == (int)r2)return 0;{struct _tuple0 _tmp505;struct _tuple0 _tmp49=(_tmp505.f1=
r1,((_tmp505.f2=r2,_tmp505)));void*_tmp4A;struct Cyc_Absyn_Vardecl*_tmp4C;void*
_tmp4D;struct Cyc_Absyn_Vardecl*_tmp4F;void*_tmp50;void*_tmp52;void*_tmp54;struct
Cyc_Absyn_Exp*_tmp56;void*_tmp57;struct Cyc_Absyn_Exp*_tmp59;void*_tmp5A;void*
_tmp5C;void*_tmp5E;int _tmp60;void*_tmp61;int _tmp63;_LL12: _tmp4A=_tmp49.f1;{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4B=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)
_tmp4A;if(_tmp4B->tag != 0)goto _LL14;else{_tmp4C=_tmp4B->f1;}};_tmp4D=_tmp49.f2;{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4E=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)
_tmp4D;if(_tmp4E->tag != 0)goto _LL14;else{_tmp4F=_tmp4E->f1;}};_LL13: return(int)
_tmp4C - (int)_tmp4F;_LL14: _tmp50=_tmp49.f1;{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*
_tmp51=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp50;if(_tmp51->tag
!= 0)goto _LL16;};_LL15: return - 1;_LL16: _tmp52=_tmp49.f2;{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*
_tmp53=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp52;if(_tmp53->tag
!= 0)goto _LL18;};_LL17: return 1;_LL18: _tmp54=_tmp49.f1;{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*
_tmp55=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp54;if(_tmp55->tag
!= 1)goto _LL1A;else{_tmp56=_tmp55->f1;}};_tmp57=_tmp49.f2;{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*
_tmp58=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp57;if(_tmp58->tag
!= 1)goto _LL1A;else{_tmp59=_tmp58->f1;}};_LL19: return(int)_tmp56 - (int)_tmp59;
_LL1A: _tmp5A=_tmp49.f1;{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*
_tmp5B=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp5A;if(_tmp5B->tag
!= 1)goto _LL1C;};_LL1B: return - 1;_LL1C: _tmp5C=_tmp49.f2;{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*
_tmp5D=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp5C;if(_tmp5D->tag
!= 1)goto _LL1E;};_LL1D: return 1;_LL1E: _tmp5E=_tmp49.f1;{struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*
_tmp5F=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp5E;if(_tmp5F->tag
!= 2)goto _LL11;else{_tmp60=_tmp5F->f1;}};_tmp61=_tmp49.f2;{struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*
_tmp62=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp61;if(_tmp62->tag
!= 2)goto _LL11;else{_tmp63=_tmp62->f1;}};_LL1F: return _tmp60 - _tmp63;_LL11:;};}
static int Cyc_CfFlowInfo_intlist_cmp(struct Cyc_List_List*list1,struct Cyc_List_List*
list2){return((int(*)(int(*cmp)(int,int),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,list1,list2);}int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)
p2)return 0;{void*_tmp65;struct Cyc_List_List*_tmp66;struct Cyc_CfFlowInfo_Place
_tmp64=*p1;_tmp65=_tmp64.root;_tmp66=_tmp64.fields;{void*_tmp68;struct Cyc_List_List*
_tmp69;struct Cyc_CfFlowInfo_Place _tmp67=*p2;_tmp68=_tmp67.root;_tmp69=_tmp67.fields;{
int i=Cyc_CfFlowInfo_root_cmp(_tmp65,_tmp68);if(i != 0)return i;return Cyc_CfFlowInfo_intlist_cmp(
_tmp66,_tmp69);};};};}static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(
struct Cyc_CfFlowInfo_Place*p){struct Cyc_List_List*sl=0;void*_tmp6B;struct Cyc_List_List*
_tmp6C;struct Cyc_CfFlowInfo_Place _tmp6A=*p;_tmp6B=_tmp6A.root;_tmp6C=_tmp6A.fields;{
void*_tmp6D=_tmp6B;struct Cyc_Absyn_Vardecl*_tmp6F;struct Cyc_Absyn_Exp*_tmp71;int
_tmp73;_LL21: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp6E=(
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp6D;if(_tmp6E->tag != 0)
goto _LL23;else{_tmp6F=_tmp6E->f1;}}_LL22:{struct _dyneither_ptr*_tmp512;const char*
_tmp511;void*_tmp510[1];struct Cyc_String_pa_PrintArg_struct _tmp50F;struct Cyc_List_List*
_tmp50E;sl=((_tmp50E=_cycalloc(sizeof(*_tmp50E)),((_tmp50E->hd=((_tmp512=
_cycalloc(sizeof(*_tmp512)),((_tmp512[0]=((_tmp50F.tag=0,((_tmp50F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6F->name).f2),((_tmp510[0]=& _tmp50F,
Cyc_aprintf(((_tmp511="%s",_tag_dyneither(_tmp511,sizeof(char),3))),
_tag_dyneither(_tmp510,sizeof(void*),1)))))))),_tmp512)))),((_tmp50E->tl=sl,
_tmp50E))))));}goto _LL20;_LL23: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*
_tmp70=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp6D;if(_tmp70->tag
!= 1)goto _LL25;else{_tmp71=_tmp70->f1;}}_LL24:{struct _dyneither_ptr*_tmp51F;
const char*_tmp51E;void*_tmp51D[1];struct Cyc_Int_pa_PrintArg_struct _tmp51C;struct
Cyc_List_List*_tmp51B;sl=((_tmp51B=_cycalloc(sizeof(*_tmp51B)),((_tmp51B->hd=((
_tmp51F=_cycalloc(sizeof(*_tmp51F)),((_tmp51F[0]=((_tmp51C.tag=1,((_tmp51C.f1=(
unsigned long)((int)_tmp71),((_tmp51D[0]=& _tmp51C,Cyc_aprintf(((_tmp51E="mpt%d",
_tag_dyneither(_tmp51E,sizeof(char),6))),_tag_dyneither(_tmp51D,sizeof(void*),1)))))))),
_tmp51F)))),((_tmp51B->tl=sl,_tmp51B))))));}goto _LL20;_LL25: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*
_tmp72=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp6D;if(_tmp72->tag
!= 2)goto _LL20;else{_tmp73=_tmp72->f1;}}_LL26:{struct _dyneither_ptr*_tmp52C;
const char*_tmp52B;void*_tmp52A[1];struct Cyc_Int_pa_PrintArg_struct _tmp529;struct
Cyc_List_List*_tmp528;sl=((_tmp528=_cycalloc(sizeof(*_tmp528)),((_tmp528->hd=((
_tmp52C=_cycalloc(sizeof(*_tmp52C)),((_tmp52C[0]=((_tmp529.tag=1,((_tmp529.f1=(
unsigned long)_tmp73,((_tmp52A[0]=& _tmp529,Cyc_aprintf(((_tmp52B="param%d",
_tag_dyneither(_tmp52B,sizeof(char),8))),_tag_dyneither(_tmp52A,sizeof(void*),1)))))))),
_tmp52C)))),((_tmp528->tl=sl,_tmp528))))));}goto _LL20;_LL20:;}for(0;_tmp6C != 0;
_tmp6C=_tmp6C->tl){struct _dyneither_ptr*_tmp539;const char*_tmp538;void*_tmp537[1];
struct Cyc_Int_pa_PrintArg_struct _tmp536;struct Cyc_List_List*_tmp535;sl=((_tmp535=
_cycalloc(sizeof(*_tmp535)),((_tmp535->hd=((_tmp539=_cycalloc(sizeof(*_tmp539)),((
_tmp539[0]=((_tmp536.tag=1,((_tmp536.f1=(unsigned long)((int)_tmp6C->hd),((
_tmp537[0]=& _tmp536,Cyc_aprintf(((_tmp538="%d",_tag_dyneither(_tmp538,sizeof(
char),3))),_tag_dyneither(_tmp537,sizeof(void*),1)))))))),_tmp539)))),((_tmp535->tl=
sl,_tmp535))))));}{struct Cyc_String_pa_PrintArg_struct _tmp544;const char*_tmp543;
void*_tmp542[1];const char*_tmp541;struct _dyneither_ptr*_tmp540;struct
_dyneither_ptr*_tmp88=(_tmp540=_cycalloc(sizeof(*_tmp540)),((_tmp540[0]=((
_tmp544.tag=0,((_tmp544.f1=(struct _dyneither_ptr)((_tmp543="",_tag_dyneither(
_tmp543,sizeof(char),1))),((_tmp542[0]=& _tmp544,Cyc_aprintf(((_tmp541="%s",
_tag_dyneither(_tmp541,sizeof(char),3))),_tag_dyneither(_tmp542,sizeof(void*),1)))))))),
_tmp540)));for(0;sl != 0;sl=sl->tl){const char*_tmp549;void*_tmp548[2];struct Cyc_String_pa_PrintArg_struct
_tmp547;struct Cyc_String_pa_PrintArg_struct _tmp546;*_tmp88=((_tmp546.tag=0,((
_tmp546.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp88),((_tmp547.tag=0,((
_tmp547.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)
sl->hd)),((_tmp548[0]=& _tmp547,((_tmp548[1]=& _tmp546,Cyc_aprintf(((_tmp549="%s.%s",
_tag_dyneither(_tmp549,sizeof(char),6))),_tag_dyneither(_tmp548,sizeof(void*),2))))))))))))));}
return _tmp88;};}static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*
fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);struct
_dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval);static void _tmp54F(
struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp54E,unsigned int*_tmp54D,
void***_tmp54B){for(*_tmp54E=0;*_tmp54E < *_tmp54D;(*_tmp54E)++){(*_tmp54B)[*
_tmp54E]=(*fenv)->unknown_all;}}struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,
void*leafval){unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
fs);unsigned int _tmp54E;unsigned int _tmp54D;struct _dyneither_ptr _tmp54C;void**
_tmp54B;unsigned int _tmp54A;struct _dyneither_ptr d=(_tmp54A=sz,((_tmp54B=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp54A)),((_tmp54C=
_tag_dyneither(_tmp54B,sizeof(void*),_tmp54A),((((_tmp54D=_tmp54A,_tmp54F(& fenv,&
_tmp54E,& _tmp54D,& _tmp54B))),_tmp54C)))))));{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield _tmp93;struct _dyneither_ptr*_tmp94;void*_tmp95;struct
Cyc_Absyn_Aggrfield*_tmp92=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)
_check_null(fs))->hd;_tmp93=*_tmp92;_tmp94=_tmp93.name;_tmp95=_tmp93.type;if(
_get_dyneither_size(*_tmp94,sizeof(char))!= 1)*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,no_init_bits_only,_tmp95,leafval);}}return d;}struct _tuple15{struct
_RegionHandle*f1;struct Cyc_List_List*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(
struct _tuple15*env,struct Cyc_Absyn_Aggrfield*f){struct _tuple15 _tmp9B;struct
_RegionHandle*_tmp9C;struct Cyc_List_List*_tmp9D;struct _tuple15*_tmp9A=env;_tmp9B=*
_tmp9A;_tmp9C=_tmp9B.f1;_tmp9D=_tmp9B.f2;{void*_tmp9E=Cyc_Tcutil_rsubstitute(
_tmp9C,_tmp9D,f->type);struct Cyc_Absyn_Aggrfield*_tmp550;return(_tmp550=
_region_malloc(_tmp9C,sizeof(*_tmp550)),((_tmp550->name=f->name,((_tmp550->tq=f->tq,((
_tmp550->type=_tmp9E,((_tmp550->width=f->width,((_tmp550->attributes=f->attributes,
_tmp550)))))))))));};}struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};static
struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*targs,struct Cyc_List_List*fs,
int no_init_bits_only,void*leafval){struct _RegionHandle _tmpA0=_new_region("temp");
struct _RegionHandle*temp=& _tmpA0;_push_region(temp);{struct Cyc_List_List*inst=0;
for(0;tvs != 0;(tvs=tvs->tl,targs=targs->tl)){struct Cyc_Absyn_Tvar*_tmpA1=(struct
Cyc_Absyn_Tvar*)tvs->hd;void*_tmpA2=(void*)((struct Cyc_List_List*)_check_null(
targs))->hd;{struct Cyc_Absyn_Kind*_tmpA3=Cyc_Tcutil_tvar_kind(_tmpA1,& Cyc_Tcutil_bk);
struct Cyc_Absyn_Kind _tmpA4;enum Cyc_Absyn_KindQual _tmpA5;struct Cyc_Absyn_Kind
_tmpA6;enum Cyc_Absyn_KindQual _tmpA7;struct Cyc_Absyn_Kind _tmpA8;enum Cyc_Absyn_KindQual
_tmpA9;_LL28: _tmpA4=*_tmpA3;_tmpA5=_tmpA4.kind;if(_tmpA5 != Cyc_Absyn_RgnKind)
goto _LL2A;_LL29: goto _LL2B;_LL2A: _tmpA6=*_tmpA3;_tmpA7=_tmpA6.kind;if(_tmpA7 != 
Cyc_Absyn_EffKind)goto _LL2C;_LL2B: goto _LL2D;_LL2C: _tmpA8=*_tmpA3;_tmpA9=_tmpA8.kind;
if(_tmpA9 != Cyc_Absyn_IntKind)goto _LL2E;_LL2D: continue;_LL2E:;_LL2F: goto _LL27;
_LL27:;}{struct _tuple16*_tmp553;struct Cyc_List_List*_tmp552;inst=((_tmp552=
_region_malloc(temp,sizeof(*_tmp552)),((_tmp552->hd=((_tmp553=_region_malloc(
temp,sizeof(*_tmp553)),((_tmp553->f1=_tmpA1,((_tmp553->f2=_tmpA2,_tmp553)))))),((
_tmp552->tl=inst,_tmp552))))));};}if(inst != 0){struct _tuple15 _tmp554;struct
_tuple15 _tmpAC=(_tmp554.f1=temp,((_tmp554.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(inst),_tmp554)));struct Cyc_List_List*subs_fs=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(struct _tuple15*,struct Cyc_Absyn_Aggrfield*),
struct _tuple15*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,&
_tmpAC,fs);struct _dyneither_ptr _tmpAD=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,
subs_fs,no_init_bits_only,leafval);_npop_handler(0);return _tmpAD;}else{struct
_dyneither_ptr _tmpAF=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,
no_init_bits_only,leafval);_npop_handler(0);return _tmpAF;}};_pop_region(temp);}
struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,
void*leafval);static void _tmp55A(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*
_tmp559,unsigned int*_tmp558,void***_tmp556){for(*_tmp559=0;*_tmp559 < *_tmp558;(*
_tmp559)++){(*_tmp556)[*_tmp559]=(*fenv)->unknown_all;}}static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,
void*leafval){{void*_tmpB0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_DatatypeFieldInfo
_tmpB2;union Cyc_Absyn_DatatypeFieldInfoU _tmpB3;struct _tuple2 _tmpB4;struct Cyc_Absyn_Datatypefield*
_tmpB5;struct Cyc_List_List*_tmpB7;struct Cyc_Absyn_AggrInfo _tmpB9;union Cyc_Absyn_AggrInfoU
_tmpBA;struct Cyc_List_List*_tmpBB;enum Cyc_Absyn_AggrKind _tmpBD;struct Cyc_List_List*
_tmpBE;struct Cyc_Absyn_ArrayInfo _tmpC0;void*_tmpC1;union Cyc_Absyn_Constraint*
_tmpC2;void*_tmpC4;struct Cyc_Absyn_PtrInfo _tmpC6;struct Cyc_Absyn_PtrAtts _tmpC7;
union Cyc_Absyn_Constraint*_tmpC8;_LL31: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmpB1=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpB0;if(_tmpB1->tag
!= 4)goto _LL33;else{_tmpB2=_tmpB1->f1;_tmpB3=_tmpB2.field_info;if((_tmpB3.KnownDatatypefield).tag
!= 2)goto _LL33;_tmpB4=(struct _tuple2)(_tmpB3.KnownDatatypefield).val;_tmpB5=
_tmpB4.f2;}}_LL32: if(_tmpB5->typs == 0)return leafval;_tmpB7=_tmpB5->typs;goto
_LL34;_LL33: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpB6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)
_tmpB0;if(_tmpB6->tag != 10)goto _LL35;else{_tmpB7=_tmpB6->f1;}}_LL34: {
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpB7);unsigned int _tmp559;unsigned int _tmp558;struct _dyneither_ptr _tmp557;void**
_tmp556;unsigned int _tmp555;struct _dyneither_ptr d=(_tmp555=sz,((_tmp556=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp555)),((_tmp557=
_tag_dyneither(_tmp556,sizeof(void*),_tmp555),((((_tmp558=_tmp555,_tmp55A(& fenv,&
_tmp559,& _tmp558,& _tmp556))),_tmp557)))))));{int i=0;for(0;i < sz;++ i){*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,no_init_bits_only,(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(
_tmpB7))->hd)).f2,leafval);_tmpB7=_tmpB7->tl;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct
_tmp560;struct Cyc_CfFlowInfo_UnionRInfo _tmp55F;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp55E;return(void*)((_tmp55E=_region_malloc(fenv->r,sizeof(*_tmp55E)),((
_tmp55E[0]=((_tmp560.tag=6,((_tmp560.f1=((_tmp55F.is_union=0,((_tmp55F.fieldnum=
- 1,_tmp55F)))),((_tmp560.f2=d,_tmp560)))))),_tmp55E))));};}_LL35: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmpB8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB0;if(_tmpB8->tag != 11)
goto _LL37;else{_tmpB9=_tmpB8->f1;_tmpBA=_tmpB9.aggr_info;_tmpBB=_tmpB9.targs;}}
_LL36: {struct Cyc_Absyn_Aggrdecl*_tmpD0=Cyc_Absyn_get_known_aggrdecl(_tmpBA);if(
_tmpD0->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmpD0->impl))->tagged)return fenv->unknown_all;if(_tmpD0->impl == 0)goto _LL30;{
struct Cyc_List_List*_tmpD1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD0->impl))->fields;
if(_tmpBB == 0){_tmpBD=_tmpD0->kind;_tmpBE=_tmpD1;goto _LL38;}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct
_tmp566;struct Cyc_CfFlowInfo_UnionRInfo _tmp565;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp564;return(void*)((_tmp564=_region_malloc(fenv->r,sizeof(*_tmp564)),((
_tmp564[0]=((_tmp566.tag=6,((_tmp566.f1=((_tmp565.is_union=_tmpD0->kind == Cyc_Absyn_UnionA,((
_tmp565.fieldnum=- 1,_tmp565)))),((_tmp566.f2=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(
fenv,_tmpD0->tvs,_tmpBB,_tmpD1,_tmpD0->kind == Cyc_Absyn_UnionA,leafval),_tmp566)))))),
_tmp564))));};};}_LL37: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpBC=(
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpB0;if(_tmpBC->tag != 12)goto
_LL39;else{_tmpBD=_tmpBC->f1;_tmpBE=_tmpBC->f2;}}_LL38: if(_tmpBD == Cyc_Absyn_UnionA)
return fenv->unknown_all;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct
_tmp56C;struct Cyc_CfFlowInfo_UnionRInfo _tmp56B;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp56A;return(void*)((_tmp56A=_region_malloc(fenv->r,sizeof(*_tmp56A)),((
_tmp56A[0]=((_tmp56C.tag=6,((_tmp56C.f1=((_tmp56B.is_union=_tmpBD == Cyc_Absyn_UnionA,((
_tmp56B.fieldnum=- 1,_tmp56B)))),((_tmp56C.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(
fenv,_tmpBE,_tmpBD == Cyc_Absyn_UnionA,leafval),_tmp56C)))))),_tmp56A))));};_LL39:{
struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpBF=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)
_tmpB0;if(_tmpBF->tag != 8)goto _LL3B;else{_tmpC0=_tmpBF->f1;_tmpC1=_tmpC0.elt_type;
_tmpC2=_tmpC0.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmpC2))goto _LL3B;_LL3A: return(allow_zeroterm  && !no_init_bits_only) && Cyc_Tcutil_bits_only(
_tmpC1)?fenv->unknown_all: leafval;_LL3B: {struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmpC3=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpB0;if(_tmpC3->tag != 19)
goto _LL3D;else{_tmpC4=(void*)_tmpC3->f1;}}_LL3C: return leafval;_LL3D:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmpC5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB0;if(_tmpC5->tag != 5)
goto _LL3F;else{_tmpC6=_tmpC5->f1;_tmpC7=_tmpC6.ptr_atts;_tmpC8=_tmpC7.nullable;}}
if(!(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpC8)))
goto _LL3F;_LL3E:{void*_tmpD8=leafval;enum Cyc_CfFlowInfo_InitLevel _tmpDA;enum 
Cyc_CfFlowInfo_InitLevel _tmpDC;_LL42: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*
_tmpD9=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpD8;if(
_tmpD9->tag != 3)goto _LL44;else{_tmpDA=_tmpD9->f1;if(_tmpDA != Cyc_CfFlowInfo_ThisIL)
goto _LL44;}}_LL43: return fenv->notzerothis;_LL44: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*
_tmpDB=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpD8;if(
_tmpDB->tag != 3)goto _LL46;else{_tmpDC=_tmpDB->f1;if(_tmpDC != Cyc_CfFlowInfo_AllIL)
goto _LL46;}}_LL45: return fenv->notzeroall;_LL46:;_LL47: goto _LL41;_LL41:;}goto
_LL30;_LL3F:;_LL40: goto _LL30;_LL30:;}return !no_init_bits_only  && Cyc_Tcutil_bits_only(
t)?fenv->unknown_all: leafval;}void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*
fenv,void*t,int no_init_bits_only,void*leafval){return Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,1,no_init_bits_only,t,leafval);}int Cyc_CfFlowInfo_is_unique_consumed(struct
Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){void*_tmpDD=r;
struct Cyc_Absyn_Exp*_tmpDF;int _tmpE0;void*_tmpE1;struct Cyc_CfFlowInfo_UnionRInfo
_tmpE3;int _tmpE4;int _tmpE5;struct _dyneither_ptr _tmpE6;_LL49: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*
_tmpDE=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpDD;if(
_tmpDE->tag != 7)goto _LL4B;else{_tmpDF=_tmpDE->f1;_tmpE0=_tmpDE->f2;_tmpE1=(void*)
_tmpDE->f3;}}_LL4A: if(_tmpDF == e  && _tmpE0 == env_iteration){*needs_unconsume=1;
return 0;}return 1;_LL4B: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmpE2=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpDD;if(
_tmpE2->tag != 6)goto _LL4D;else{_tmpE3=_tmpE2->f1;_tmpE4=_tmpE3.is_union;_tmpE5=
_tmpE3.fieldnum;_tmpE6=_tmpE2->f2;}}_LL4C: if(!_tmpE4  || _tmpE5 == - 1){
unsigned int _tmpE7=_get_dyneither_size(_tmpE6,sizeof(void*));{int i=0;for(0;i < 
_tmpE7;++ i){if(Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)
_check_dyneither_subscript(_tmpE6,sizeof(void*),i)),needs_unconsume))return 1;}}
return 0;}else{return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)
_check_dyneither_subscript(_tmpE6,sizeof(void*),_tmpE5)),needs_unconsume);}_LL4D:;
_LL4E: return 0;_LL48:;}void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*
fenv,void*r);static void _tmp572(struct _dyneither_ptr*_tmpEF,unsigned int*_tmp571,
unsigned int*_tmp570,void***_tmp56E){for(*_tmp571=0;*_tmp571 < *_tmp570;(*_tmp571)
++){(*_tmp56E)[*_tmp571]=*((void**)_check_dyneither_subscript(*_tmpEF,sizeof(
void*),(int)*_tmp571));}}void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*
fenv,void*r){void*_tmpE8=r;struct Cyc_Absyn_Exp*_tmpEA;int _tmpEB;void*_tmpEC;
struct Cyc_CfFlowInfo_UnionRInfo _tmpEE;struct _dyneither_ptr _tmpEF;_LL50: {struct
Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpE9=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)
_tmpE8;if(_tmpE9->tag != 7)goto _LL52;else{_tmpEA=_tmpE9->f1;_tmpEB=_tmpE9->f2;
_tmpEC=(void*)_tmpE9->f3;}}_LL51: return _tmpEC;_LL52: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmpED=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE8;if(
_tmpED->tag != 6)goto _LL54;else{_tmpEE=_tmpED->f1;_tmpEF=_tmpED->f2;}}_LL53: {
unsigned int _tmpF0=_get_dyneither_size(_tmpEF,sizeof(void*));int change=0;
unsigned int _tmp571;unsigned int _tmp570;struct _dyneither_ptr _tmp56F;void**
_tmp56E;unsigned int _tmp56D;struct _dyneither_ptr d2=(_tmp56D=_tmpF0,((_tmp56E=(
void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp56D)),((_tmp56F=
_tag_dyneither(_tmp56E,sizeof(void*),_tmp56D),((((_tmp570=_tmp56D,_tmp572(&
_tmpEF,& _tmp571,& _tmp570,& _tmp56E))),_tmp56F)))))));{int i=0;for(0;i < _tmpF0;++ i){*((
void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_unconsumed(
fenv,*((void**)_check_dyneither_subscript(_tmpEF,sizeof(void*),i)));if(*((void**)
_check_dyneither_subscript(d2,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmpEF,sizeof(void*),i)))change=1;}}if(change){struct
Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp575;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp574;return(void*)((_tmp574=_region_malloc(fenv->r,sizeof(*_tmp574)),((
_tmp574[0]=((_tmp575.tag=6,((_tmp575.f1=_tmpEE,((_tmp575.f2=d2,_tmp575)))))),
_tmp574))));}else{return r;}}_LL54:;_LL55: return r;_LL4F:;}void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*r);static void _tmp57F(struct Cyc_CfFlowInfo_FlowEnv**fenv,
unsigned int*_tmp57E,unsigned int*_tmp57D,void***_tmp57B){for(*_tmp57E=0;*
_tmp57E < *_tmp57D;(*_tmp57E)++){(*_tmp57B)[*_tmp57E]=(*fenv)->unknown_all;}}
static void _tmp588(unsigned int*_tmp587,unsigned int*_tmp586,void***_tmp584,
struct _dyneither_ptr*_tmp111){for(*_tmp587=0;*_tmp587 < *_tmp586;(*_tmp587)++){(*
_tmp584)[*_tmp587]=((void**)(*_tmp111).curr)[(int)*_tmp587];}}void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*r){if(Cyc_Tcutil_is_noalias_pointer(t)){struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct
_tmp578;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp577;return(
void*)((_tmp577=_region_malloc(fenv->r,sizeof(*_tmp577)),((_tmp577[0]=((_tmp578.tag=
7,((_tmp578.f1=consumer,((_tmp578.f2=iteration,((_tmp578.f3=(void*)r,_tmp578)))))))),
_tmp577))));}{struct _tuple0 _tmp579;struct _tuple0 _tmpFA=(_tmp579.f1=Cyc_Tcutil_compress(
t),((_tmp579.f2=r,_tmp579)));void*_tmpFB;struct Cyc_List_List*_tmpFD;void*_tmpFE;
struct Cyc_CfFlowInfo_UnionRInfo _tmp100;struct _dyneither_ptr _tmp101;void*_tmp102;
struct Cyc_Absyn_AggrInfo _tmp104;union Cyc_Absyn_AggrInfoU _tmp105;void*_tmp106;
struct Cyc_CfFlowInfo_UnionRInfo _tmp108;struct _dyneither_ptr _tmp109;void*_tmp10A;
enum Cyc_Absyn_AggrKind _tmp10C;struct Cyc_List_List*_tmp10D;void*_tmp10E;struct
Cyc_CfFlowInfo_UnionRInfo _tmp110;struct _dyneither_ptr _tmp111;_LL57: _tmpFB=_tmpFA.f1;{
struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpFC=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)
_tmpFB;if(_tmpFC->tag != 10)goto _LL59;else{_tmpFD=_tmpFC->f1;}};_tmpFE=_tmpFA.f2;{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpFF=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)
_tmpFE;if(_tmpFF->tag != 6)goto _LL59;else{_tmp100=_tmpFF->f1;_tmp101=_tmpFF->f2;}};
_LL58: {unsigned int _tmp112=_get_dyneither_size(_tmp101,sizeof(void*));
unsigned int _tmp57E;unsigned int _tmp57D;struct _dyneither_ptr _tmp57C;void**
_tmp57B;unsigned int _tmp57A;struct _dyneither_ptr d2=(_tmp57A=_tmp112,((_tmp57B=(
void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp57A)),((_tmp57C=
_tag_dyneither(_tmp57B,sizeof(void*),_tmp57A),((((_tmp57D=_tmp57A,_tmp57F(& fenv,&
_tmp57E,& _tmp57D,& _tmp57B))),_tmp57C)))))));{int i=0;for(0;i < _tmp112;++ i){*((
void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(
fenv,(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmpFD))->hd)).f2,
consumer,iteration,*((void**)_check_dyneither_subscript(_tmp101,sizeof(void*),i)));
_tmpFD=_tmpFD->tl;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct
_tmp582;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp581;return(
void*)((_tmp581=_region_malloc(fenv->r,sizeof(*_tmp581)),((_tmp581[0]=((_tmp582.tag=
6,((_tmp582.f1=_tmp100,((_tmp582.f2=d2,_tmp582)))))),_tmp581))));};}_LL59:
_tmp102=_tmpFA.f1;{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp103=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp102;if(_tmp103->tag != 11)goto _LL5B;else{_tmp104=_tmp103->f1;_tmp105=_tmp104.aggr_info;}};
_tmp106=_tmpFA.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp107=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp106;if(
_tmp107->tag != 6)goto _LL5B;else{_tmp108=_tmp107->f1;_tmp109=_tmp107->f2;}};_LL5A: {
struct Cyc_Absyn_Aggrdecl*_tmp119=Cyc_Absyn_get_known_aggrdecl(_tmp105);if(
_tmp119->impl == 0)return r;_tmp10C=_tmp119->kind;_tmp10D=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp119->impl))->fields;_tmp110=_tmp108;_tmp111=_tmp109;goto _LL5C;}
_LL5B: _tmp10A=_tmpFA.f1;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp10B=(
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp10A;if(_tmp10B->tag != 12)
goto _LL5D;else{_tmp10C=_tmp10B->f1;_tmp10D=_tmp10B->f2;}};_tmp10E=_tmpFA.f2;{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp10F=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)
_tmp10E;if(_tmp10F->tag != 6)goto _LL5D;else{_tmp110=_tmp10F->f1;_tmp111=_tmp10F->f2;}};
_LL5C: {unsigned int _tmp587;unsigned int _tmp586;struct _dyneither_ptr _tmp585;void**
_tmp584;unsigned int _tmp583;struct _dyneither_ptr d2=(_tmp583=_get_dyneither_size(
_tmp111,sizeof(void*)),((_tmp584=(void**)_region_malloc(fenv->r,_check_times(
sizeof(void*),_tmp583)),((_tmp585=_tag_dyneither(_tmp584,sizeof(void*),_tmp583),((((
_tmp586=_tmp583,_tmp588(& _tmp587,& _tmp586,& _tmp584,& _tmp111))),_tmp585)))))));
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp10D);{int i=0;for(0;i < sz;(i ++,_tmp10D=_tmp10D->tl)){struct Cyc_Absyn_Aggrfield
_tmp11B;struct _dyneither_ptr*_tmp11C;void*_tmp11D;struct Cyc_Absyn_Aggrfield*
_tmp11A=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp10D))->hd;
_tmp11B=*_tmp11A;_tmp11C=_tmp11B.name;_tmp11D=_tmp11B.type;if(
_get_dyneither_size(*_tmp11C,sizeof(char))!= 1)*((void**)
_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(
fenv,_tmp11D,consumer,iteration,*((void**)_check_dyneither_subscript(_tmp111,
sizeof(void*),i)));}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct
_tmp58B;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp58A;return(
void*)((_tmp58A=_region_malloc(fenv->r,sizeof(*_tmp58A)),((_tmp58A[0]=((_tmp58B.tag=
6,((_tmp58B.f1=_tmp110,((_tmp58B.f2=d2,_tmp58B)))))),_tmp58A))));};}_LL5D:;_LL5E:
return r;_LL56:;};}struct _tuple18{struct Cyc_CfFlowInfo_Place*f1;unsigned int f2;};
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct
Cyc_Dict_Dict set){struct _RegionHandle _tmp124=_new_region("r");struct
_RegionHandle*r=& _tmp124;_push_region(r);{struct _tuple18 _tmp58C;struct _tuple18
elem=(_tmp58C.f1=place,((_tmp58C.f2=0,_tmp58C)));struct Cyc_Iter_Iter iter=((
struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
r,set);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,&
elem)){struct Cyc_CfFlowInfo_Place*_tmp125=elem.f1;void*_tmp127;struct Cyc_List_List*
_tmp128;struct Cyc_CfFlowInfo_Place _tmp126=*place;_tmp127=_tmp126.root;_tmp128=
_tmp126.fields;{void*_tmp12A;struct Cyc_List_List*_tmp12B;struct Cyc_CfFlowInfo_Place
_tmp129=*_tmp125;_tmp12A=_tmp129.root;_tmp12B=_tmp129.fields;if(Cyc_CfFlowInfo_root_cmp(
_tmp127,_tmp12A)!= 0)continue;for(0;_tmp128 != 0  && _tmp12B != 0;(_tmp128=_tmp128->tl,
_tmp12B=_tmp12B->tl)){if((int)_tmp128->hd != (int)_tmp12B->hd)break;}if(_tmp128 == 
0){int _tmp12C=1;_npop_handler(0);return _tmp12C;}};}{int _tmp12D=0;_npop_handler(0);
return _tmp12D;};};_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct
_RegionHandle*rgn;struct Cyc_List_List*places;};typedef struct Cyc_CfFlowInfo_EscPile*
Cyc_CfFlowInfo_escpile_t;static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*
pile,struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place)){struct Cyc_List_List*
_tmp58D;pile->places=((_tmp58D=_region_malloc(pile->rgn,sizeof(*_tmp58D)),((
_tmp58D->hd=place,((_tmp58D->tl=pile->places,_tmp58D))))));}}static void Cyc_CfFlowInfo_add_places(
struct Cyc_CfFlowInfo_EscPile*pile,void*r){void*_tmp130=r;void*_tmp132;struct Cyc_CfFlowInfo_Place*
_tmp134;struct Cyc_CfFlowInfo_UnionRInfo _tmp136;struct _dyneither_ptr _tmp137;_LL60: {
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp131=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)
_tmp130;if(_tmp131->tag != 7)goto _LL62;else{_tmp132=(void*)_tmp131->f3;}}_LL61:
Cyc_CfFlowInfo_add_places(pile,_tmp132);return;_LL62: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp133=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp130;if(
_tmp133->tag != 5)goto _LL64;else{_tmp134=_tmp133->f1;}}_LL63: Cyc_CfFlowInfo_add_place(
pile,_tmp134);return;_LL64: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp135=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp130;if(
_tmp135->tag != 6)goto _LL66;else{_tmp136=_tmp135->f1;_tmp137=_tmp135->f2;}}_LL65:{
int i=0;for(0;i < _get_dyneither_size(_tmp137,sizeof(void*));++ i){Cyc_CfFlowInfo_add_places(
pile,((void**)_tmp137.curr)[i]);}}return;_LL66:;_LL67: return;_LL5F:;}static void*
Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*
old_val);static void _tmp593(struct _RegionHandle**r,void**new_val,unsigned int*
_tmp592,unsigned int*_tmp591,void***_tmp58F,struct _dyneither_ptr*_tmp13C){for(*
_tmp592=0;*_tmp592 < *_tmp591;(*_tmp592)++){(*_tmp58F)[*_tmp592]=Cyc_CfFlowInfo_insert_place_inner(*
r,*new_val,((void**)(*_tmp13C).curr)[(int)*_tmp592]);}}static void*Cyc_CfFlowInfo_insert_place_inner(
struct _RegionHandle*r,void*new_val,void*old_val){void*_tmp138=old_val;struct Cyc_CfFlowInfo_UnionRInfo
_tmp13A;int _tmp13B;struct _dyneither_ptr _tmp13C;struct Cyc_Absyn_Exp*_tmp13E;int
_tmp13F;void*_tmp140;_LL69: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp139=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp138;if(
_tmp139->tag != 6)goto _LL6B;else{_tmp13A=_tmp139->f1;_tmp13B=_tmp13A.is_union;
_tmp13C=_tmp139->f2;}}_LL6A: {unsigned int _tmp592;unsigned int _tmp591;struct
_dyneither_ptr _tmp590;void**_tmp58F;unsigned int _tmp58E;struct _dyneither_ptr d2=(
_tmp58E=_get_dyneither_size(_tmp13C,sizeof(void*)),((_tmp58F=(void**)
_region_malloc(r,_check_times(sizeof(void*),_tmp58E)),((_tmp590=_tag_dyneither(
_tmp58F,sizeof(void*),_tmp58E),((((_tmp591=_tmp58E,_tmp593(& r,& new_val,& _tmp592,&
_tmp591,& _tmp58F,& _tmp13C))),_tmp590)))))));struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct
_tmp599;struct Cyc_CfFlowInfo_UnionRInfo _tmp598;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp597;return(void*)((_tmp597=_region_malloc(r,sizeof(*_tmp597)),((_tmp597[0]=((
_tmp599.tag=6,((_tmp599.f1=((_tmp598.is_union=_tmp13B,((_tmp598.fieldnum=- 1,
_tmp598)))),((_tmp599.f2=d2,_tmp599)))))),_tmp597))));}_LL6B: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*
_tmp13D=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp138;if(
_tmp13D->tag != 7)goto _LL6D;else{_tmp13E=_tmp13D->f1;_tmp13F=_tmp13D->f2;_tmp140=(
void*)_tmp13D->f3;}}_LL6C: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct
_tmp59C;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp59B;return(
void*)((_tmp59B=_region_malloc(r,sizeof(*_tmp59B)),((_tmp59B[0]=((_tmp59C.tag=7,((
_tmp59C.f1=_tmp13E,((_tmp59C.f2=_tmp13F,((_tmp59C.f3=(void*)Cyc_CfFlowInfo_insert_place_inner(
r,new_val,_tmp140),_tmp59C)))))))),_tmp59B))));}_LL6D:;_LL6E: return new_val;_LL68:;}
static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,
struct _dyneither_ptr d,int n,void*rval);static void _tmp5A2(struct _dyneither_ptr*d,
unsigned int*_tmp5A1,unsigned int*_tmp5A0,void***_tmp59E){for(*_tmp5A1=0;*
_tmp5A1 < *_tmp5A0;(*_tmp5A1)++){(*_tmp59E)[*_tmp5A1]=((void**)(*d).curr)[(int)*
_tmp5A1];}}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct
_RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){void*_tmp14A=*((void**)
_check_dyneither_subscript(d,sizeof(void*),n));if(_tmp14A == rval)return d;{
unsigned int _tmp5A1;unsigned int _tmp5A0;struct _dyneither_ptr _tmp59F;void**
_tmp59E;unsigned int _tmp59D;struct _dyneither_ptr res=(_tmp59D=_get_dyneither_size(
d,sizeof(void*)),((_tmp59E=(void**)_region_malloc(r,_check_times(sizeof(void*),
_tmp59D)),((_tmp59F=_tag_dyneither(_tmp59E,sizeof(void*),_tmp59D),((((_tmp5A0=
_tmp59D,_tmp5A2(& d,& _tmp5A1,& _tmp5A0,& _tmp59E))),_tmp59F)))))));*((void**)
_check_dyneither_subscript(res,sizeof(void*),n))=rval;return res;};}struct
_tuple19{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){if(fs == 
0)return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{struct _tuple19
_tmp5A3;struct _tuple19 _tmp150=(_tmp5A3.f1=fs,((_tmp5A3.f2=old_val,_tmp5A3)));
struct Cyc_List_List*_tmp151;struct Cyc_List_List _tmp152;int _tmp153;struct Cyc_List_List*
_tmp154;void*_tmp155;struct Cyc_CfFlowInfo_UnionRInfo _tmp157;int _tmp158;struct
_dyneither_ptr _tmp159;_LL70: _tmp151=_tmp150.f1;if(_tmp151 == 0)goto _LL72;_tmp152=*
_tmp151;_tmp153=(int)_tmp152.hd;_tmp154=_tmp152.tl;_tmp155=_tmp150.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp156=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp155;if(
_tmp156->tag != 6)goto _LL72;else{_tmp157=_tmp156->f1;_tmp158=_tmp157.is_union;
_tmp159=_tmp156->f2;}};_LL71: {void*_tmp15A=Cyc_CfFlowInfo_insert_place_outer(r,
_tmp154,*((void**)_check_dyneither_subscript(_tmp159,sizeof(void*),_tmp153)),
new_val);struct _dyneither_ptr _tmp15B=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp159,
_tmp153,_tmp15A);if((void**)_tmp15B.curr == (void**)_tmp159.curr)return old_val;
else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp5A9;struct Cyc_CfFlowInfo_UnionRInfo
_tmp5A8;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp5A7;return(
void*)((_tmp5A7=_region_malloc(r,sizeof(*_tmp5A7)),((_tmp5A7[0]=((_tmp5A9.tag=6,((
_tmp5A9.f1=((_tmp5A8.is_union=_tmp158,((_tmp5A8.fieldnum=- 1,_tmp5A8)))),((
_tmp5A9.f2=_tmp15B,_tmp5A9)))))),_tmp5A7))));}}_LL72:;_LL73: {struct Cyc_Core_Impossible_exn_struct
_tmp5AF;const char*_tmp5AE;struct Cyc_Core_Impossible_exn_struct*_tmp5AD;(int)
_throw((void*)((_tmp5AD=_cycalloc(sizeof(*_tmp5AD)),((_tmp5AD[0]=((_tmp5AF.tag=
Cyc_Core_Impossible,((_tmp5AF.f1=((_tmp5AE="bad insert place",_tag_dyneither(
_tmp5AE,sizeof(char),17))),_tmp5AF)))),_tmp5AD)))));}_LL6F:;};}static struct Cyc_Dict_Dict
Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*
pile,struct Cyc_Dict_Dict*all_changed,struct Cyc_Dict_Dict d){while(pile->places != 
0){struct Cyc_CfFlowInfo_Place*_tmp162=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)
_check_null(pile->places))->hd;pile->places=((struct Cyc_List_List*)_check_null(
pile->places))->tl;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*
place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,_tmp162,0);{
void*oldval;void*newval;{struct _handler_cons _tmp163;_push_handler(& _tmp163);{int
_tmp165=0;if(setjmp(_tmp163.handler))_tmp165=1;if(!_tmp165){oldval=Cyc_CfFlowInfo_lookup_place(
d,_tmp162);;_pop_handler();}else{void*_tmp164=(void*)_exn_thrown;void*_tmp167=
_tmp164;_LL75: {struct Cyc_Dict_Absent_exn_struct*_tmp168=(struct Cyc_Dict_Absent_exn_struct*)
_tmp167;if(_tmp168->tag != Cyc_Dict_Absent)goto _LL77;}_LL76: continue;_LL77:;_LL78:(
void)_throw(_tmp167);_LL74:;}};}switch(Cyc_CfFlowInfo_initlevel(fenv,d,oldval)){
case Cyc_CfFlowInfo_AllIL: _LL79: newval=fenv->esc_all;break;case Cyc_CfFlowInfo_ThisIL:
_LL7A: newval=fenv->esc_this;break;case Cyc_CfFlowInfo_NoneIL: _LL7B: newval=fenv->esc_none;
break;}Cyc_CfFlowInfo_add_places(pile,oldval);{void*_tmp16A;struct Cyc_List_List*
_tmp16B;struct Cyc_CfFlowInfo_Place _tmp169=*_tmp162;_tmp16A=_tmp169.root;_tmp16B=
_tmp169.fields;d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))
Cyc_Dict_insert)(d,_tmp16A,Cyc_CfFlowInfo_insert_place_outer(fenv->r,_tmp16B,((
void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp16A),newval));};};}
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct _RegionHandle*e;struct Cyc_Dict_Dict
d;struct Cyc_List_List*seen;};static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(
void*r){void*_tmp16C=r;enum Cyc_CfFlowInfo_InitLevel _tmp16E;enum Cyc_CfFlowInfo_InitLevel
_tmp170;void*_tmp175;_LL7E: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*
_tmp16D=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp16C;if(
_tmp16D->tag != 3)goto _LL80;else{_tmp16E=_tmp16D->f1;}}_LL7F: return _tmp16E;_LL80: {
struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp16F=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)
_tmp16C;if(_tmp16F->tag != 4)goto _LL82;else{_tmp170=_tmp16F->f1;}}_LL81: return
_tmp170;_LL82: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp171=(
struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp16C;if(_tmp171->tag != 0)
goto _LL84;}_LL83: goto _LL85;_LL84: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*
_tmp172=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp16C;if(
_tmp172->tag != 1)goto _LL86;}_LL85: return Cyc_CfFlowInfo_AllIL;_LL86: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*
_tmp173=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp16C;if(
_tmp173->tag != 2)goto _LL88;}_LL87: return Cyc_CfFlowInfo_ThisIL;_LL88: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*
_tmp174=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp16C;if(
_tmp174->tag != 7)goto _LL8A;else{_tmp175=(void*)_tmp174->f3;}}_LL89: return Cyc_CfFlowInfo_NoneIL;
_LL8A:;_LL8B: {struct Cyc_Core_Impossible_exn_struct _tmp5B5;const char*_tmp5B4;
struct Cyc_Core_Impossible_exn_struct*_tmp5B3;(int)_throw((void*)((_tmp5B3=
_cycalloc(sizeof(*_tmp5B3)),((_tmp5B3[0]=((_tmp5B5.tag=Cyc_Core_Impossible,((
_tmp5B5.f1=((_tmp5B4="initlevel_approx",_tag_dyneither(_tmp5B4,sizeof(char),17))),
_tmp5B5)))),_tmp5B3)))));}_LL7D:;}static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(
struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){
enum Cyc_CfFlowInfo_InitLevel this_ans;if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;{
void*_tmp179=r;struct Cyc_CfFlowInfo_UnionRInfo _tmp17B;int _tmp17C;int _tmp17D;
struct _dyneither_ptr _tmp17E;struct Cyc_CfFlowInfo_UnionRInfo _tmp180;int _tmp181;
struct _dyneither_ptr _tmp182;struct Cyc_CfFlowInfo_Place*_tmp184;_LL8D: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp17A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp179;if(
_tmp17A->tag != 6)goto _LL8F;else{_tmp17B=_tmp17A->f1;_tmp17C=_tmp17B.is_union;if(
_tmp17C != 1)goto _LL8F;_tmp17D=_tmp17B.fieldnum;_tmp17E=_tmp17A->f2;}}_LL8E: {
unsigned int _tmp185=_get_dyneither_size(_tmp17E,sizeof(void*));this_ans=Cyc_CfFlowInfo_NoneIL;
if(_tmp17D == - 1){int i=0;for(0;i < _tmp185;++ i){if(Cyc_CfFlowInfo_initlevel_rec(
env,*((void**)_check_dyneither_subscript(_tmp17E,sizeof(void*),i)),Cyc_CfFlowInfo_AllIL)
== Cyc_CfFlowInfo_AllIL){this_ans=Cyc_CfFlowInfo_AllIL;break;}}}else{if(Cyc_CfFlowInfo_initlevel_rec(
env,*((void**)_check_dyneither_subscript(_tmp17E,sizeof(void*),_tmp17D)),Cyc_CfFlowInfo_AllIL)
== Cyc_CfFlowInfo_AllIL)this_ans=Cyc_CfFlowInfo_AllIL;}goto _LL8C;}_LL8F: {struct
Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp17F=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)
_tmp179;if(_tmp17F->tag != 6)goto _LL91;else{_tmp180=_tmp17F->f1;_tmp181=_tmp180.is_union;
if(_tmp181 != 0)goto _LL91;_tmp182=_tmp17F->f2;}}_LL90: {unsigned int _tmp186=
_get_dyneither_size(_tmp182,sizeof(void*));this_ans=Cyc_CfFlowInfo_AllIL;{int i=0;
for(0;i < _tmp186;++ i){this_ans=Cyc_CfFlowInfo_initlevel_rec(env,*((void**)
_check_dyneither_subscript(_tmp182,sizeof(void*),i)),this_ans);}}goto _LL8C;}
_LL91: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp183=(struct
Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp179;if(_tmp183->tag != 5)
goto _LL93;else{_tmp184=_tmp183->f1;}}_LL92: if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,_tmp184))this_ans=Cyc_CfFlowInfo_AllIL;
else{{struct Cyc_List_List*_tmp5B6;env->seen=((_tmp5B6=_region_malloc(env->e,
sizeof(*_tmp5B6)),((_tmp5B6->hd=_tmp184,((_tmp5B6->tl=env->seen,_tmp5B6))))));}
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(env->d,
_tmp184),Cyc_CfFlowInfo_AllIL);env->seen=((struct Cyc_List_List*)_check_null(env->seen))->tl;
if(this_ans == Cyc_CfFlowInfo_NoneIL)this_ans=Cyc_CfFlowInfo_ThisIL;}goto _LL8C;
_LL93:;_LL94: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL8C:;}if(this_ans == 
Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;if(this_ans == Cyc_CfFlowInfo_ThisIL
 || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;return Cyc_CfFlowInfo_AllIL;}
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*
env,struct Cyc_Dict_Dict d,void*r){struct _RegionHandle*_tmp188=env->r;struct Cyc_CfFlowInfo_InitlevelEnv
_tmp5B7;struct Cyc_CfFlowInfo_InitlevelEnv _tmp189=(_tmp5B7.e=_tmp188,((_tmp5B7.d=
d,((_tmp5B7.seen=0,_tmp5B7)))));return Cyc_CfFlowInfo_initlevel_rec(& _tmp189,r,
Cyc_CfFlowInfo_AllIL);}struct _tuple20{void*f1;int f2;};void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){void*_tmp18C;struct Cyc_List_List*
_tmp18D;struct Cyc_CfFlowInfo_Place _tmp18B=*place;_tmp18C=_tmp18B.root;_tmp18D=
_tmp18B.fields;{void*_tmp18E=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
d,_tmp18C);for(0;_tmp18D != 0;_tmp18D=_tmp18D->tl){struct _tuple20 _tmp5B8;struct
_tuple20 _tmp190=(_tmp5B8.f1=_tmp18E,((_tmp5B8.f2=(int)_tmp18D->hd,_tmp5B8)));
void*_tmp191;struct Cyc_CfFlowInfo_UnionRInfo _tmp193;struct _dyneither_ptr _tmp194;
int _tmp195;_LL96: _tmp191=_tmp190.f1;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp192=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp191;if(
_tmp192->tag != 6)goto _LL98;else{_tmp193=_tmp192->f1;_tmp194=_tmp192->f2;}};
_tmp195=_tmp190.f2;_LL97: _tmp18E=*((void**)_check_dyneither_subscript(_tmp194,
sizeof(void*),_tmp195));goto _LL95;_LL98:;_LL99: {struct Cyc_Core_Impossible_exn_struct
_tmp5BE;const char*_tmp5BD;struct Cyc_Core_Impossible_exn_struct*_tmp5BC;(int)
_throw((void*)((_tmp5BC=_cycalloc(sizeof(*_tmp5BC)),((_tmp5BC[0]=((_tmp5BE.tag=
Cyc_Core_Impossible,((_tmp5BE.f1=((_tmp5BD="bad lookup_place",_tag_dyneither(
_tmp5BD,sizeof(char),17))),_tmp5BE)))),_tmp5BC)))));}_LL95:;}return _tmp18E;};}
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){void*_tmp199=rval;void*
_tmp19C;struct Cyc_CfFlowInfo_UnionRInfo _tmp19E;int _tmp19F;int _tmp1A0;struct
_dyneither_ptr _tmp1A1;_LL9B: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*
_tmp19A=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp199;if(_tmp19A->tag
!= 4)goto _LL9D;}_LL9C: return 0;_LL9D: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*
_tmp19B=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp199;if(
_tmp19B->tag != 7)goto _LL9F;else{_tmp19C=(void*)_tmp19B->f3;}}_LL9E: return Cyc_CfFlowInfo_is_rval_unescaped(
rval);_LL9F: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp19D=(
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp199;if(_tmp19D->tag
!= 6)goto _LLA1;else{_tmp19E=_tmp19D->f1;_tmp19F=_tmp19E.is_union;_tmp1A0=_tmp19E.fieldnum;
_tmp1A1=_tmp19D->f2;}}_LLA0: if(_tmp19F  && _tmp1A0 != - 1)return !Cyc_CfFlowInfo_is_rval_unescaped(*((
void**)_check_dyneither_subscript(_tmp1A1,sizeof(void*),_tmp1A0)));else{
unsigned int sz=_get_dyneither_size(_tmp1A1,sizeof(void*));{int i=0;for(0;i < sz;++
i){if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp1A1.curr)[i]))return 0;}}
return 1;}_LLA1:;_LLA2: return 1;_LL9A:;}int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place){return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(
d,place));}struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){struct
_RegionHandle*_tmp1A2=fenv->r;struct Cyc_CfFlowInfo_EscPile*_tmp5BF;struct Cyc_CfFlowInfo_EscPile*
pile=(_tmp5BF=_region_malloc(_tmp1A2,sizeof(*_tmp5BF)),((_tmp5BF->rgn=_tmp1A2,((
_tmp5BF->places=0,_tmp5BF)))));Cyc_CfFlowInfo_add_places(pile,r);return Cyc_CfFlowInfo_escape_these(
fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*
fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned int loc;
struct Cyc_CfFlowInfo_Place*root_place;struct Cyc_Dict_Dict*all_changed;};static
void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*
oldval,void*newval);static void _tmp5C9(struct Cyc_CfFlowInfo_AssignEnv**env,
unsigned int*_tmp5C8,unsigned int*_tmp5C7,void***_tmp5C5,struct _dyneither_ptr*
_tmp1B4,struct _dyneither_ptr*_tmp1B0){for(*_tmp5C8=0;*_tmp5C8 < *_tmp5C7;(*
_tmp5C8)++){(*_tmp5C5)[*_tmp5C8]=Cyc_CfFlowInfo_assign_place_inner(*env,((void**)(*
_tmp1B0).curr)[(int)*_tmp5C8],*((void**)_check_dyneither_subscript(*_tmp1B4,
sizeof(void*),(int)*_tmp5C8)));}}static void*Cyc_CfFlowInfo_assign_place_inner(
struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){struct _tuple0 _tmp5C0;
struct _tuple0 _tmp1A5=(_tmp5C0.f1=oldval,((_tmp5C0.f2=newval,_tmp5C0)));void*
_tmp1A6;void*_tmp1A8;struct Cyc_CfFlowInfo_Place*_tmp1AA;void*_tmp1AB;void*
_tmp1AD;struct Cyc_CfFlowInfo_UnionRInfo _tmp1AF;struct _dyneither_ptr _tmp1B0;void*
_tmp1B1;struct Cyc_CfFlowInfo_UnionRInfo _tmp1B3;struct _dyneither_ptr _tmp1B4;void*
_tmp1B5;enum Cyc_CfFlowInfo_InitLevel _tmp1B7;_LLA4: _tmp1A6=_tmp1A5.f1;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*
_tmp1A7=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1A6;if(_tmp1A7->tag
!= 4)goto _LLA6;};_tmp1A8=_tmp1A5.f2;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp1A9=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A8;if(
_tmp1A9->tag != 5)goto _LLA6;else{_tmp1AA=_tmp1A9->f1;}};_LLA5: Cyc_CfFlowInfo_add_place(
env->pile,_tmp1AA);goto _LLA7;_LLA6: _tmp1AB=_tmp1A5.f1;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*
_tmp1AC=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1AB;if(_tmp1AC->tag
!= 4)goto _LLA8;};_LLA7: if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= 
Cyc_CfFlowInfo_AllIL){const char*_tmp5C3;void*_tmp5C2;(_tmp5C2=0,Cyc_Tcutil_terr(
env->loc,((_tmp5C3="assignment puts possibly-uninitialized data in an escaped location",
_tag_dyneither(_tmp5C3,sizeof(char),67))),_tag_dyneither(_tmp5C2,sizeof(void*),0)));}
return(env->fenv)->esc_all;_LLA8: _tmp1AD=_tmp1A5.f1;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp1AE=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AD;if(
_tmp1AE->tag != 6)goto _LLAA;else{_tmp1AF=_tmp1AE->f1;_tmp1B0=_tmp1AE->f2;}};
_tmp1B1=_tmp1A5.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp1B2=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1B1;if(
_tmp1B2->tag != 6)goto _LLAA;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B2->f2;}};_LLA9: {
unsigned int _tmp5C8;unsigned int _tmp5C7;struct _dyneither_ptr _tmp5C6;void**
_tmp5C5;unsigned int _tmp5C4;struct _dyneither_ptr new_d=(_tmp5C4=
_get_dyneither_size(_tmp1B0,sizeof(void*)),((_tmp5C5=(void**)_region_malloc((env->fenv)->r,
_check_times(sizeof(void*),_tmp5C4)),((_tmp5C6=_tag_dyneither(_tmp5C5,sizeof(
void*),_tmp5C4),((((_tmp5C7=_tmp5C4,_tmp5C9(& env,& _tmp5C8,& _tmp5C7,& _tmp5C5,&
_tmp1B4,& _tmp1B0))),_tmp5C6)))))));int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp1B0,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1B0.curr)[i]){
change=1;break;}}}if(!change){if(!_tmp1AF.is_union)return oldval;new_d=_tmp1B0;}
else{change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp1B0,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp1B4,sizeof(void*),i))){change=1;break;}}}if(!
change){if(!_tmp1AF.is_union)return newval;new_d=_tmp1B4;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct
_tmp5CC;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp5CB;return(
void*)((_tmp5CB=_region_malloc((env->fenv)->r,sizeof(*_tmp5CB)),((_tmp5CB[0]=((
_tmp5CC.tag=6,((_tmp5CC.f1=_tmp1B3,((_tmp5CC.f2=new_d,_tmp5CC)))))),_tmp5CB))));};}
_LLAA: _tmp1B5=_tmp1A5.f2;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*
_tmp1B6=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1B5;if(_tmp1B6->tag
!= 4)goto _LLAC;else{_tmp1B7=_tmp1B6->f1;}};_LLAB: switch(_tmp1B7){case Cyc_CfFlowInfo_NoneIL:
_LLAE: return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL: _LLAF: return(env->fenv)->unknown_this;
case Cyc_CfFlowInfo_AllIL: _LLB0: return(env->fenv)->unknown_all;}_LLAC:;_LLAD:
return newval;_LLA3:;}static int Cyc_CfFlowInfo_nprefix(int*n,void*unused){if(*n > 0){*
n=*n - 1;return 1;}else{return 0;}}static void*Cyc_CfFlowInfo_assign_place_outer(
struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,int fs_prefix,void*
oldval,void*newval){if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,
newval);{struct _tuple19 _tmp5CD;struct _tuple19 _tmp1C1=(_tmp5CD.f1=fs,((_tmp5CD.f2=
oldval,_tmp5CD)));struct Cyc_List_List*_tmp1C2;struct Cyc_List_List _tmp1C3;int
_tmp1C4;struct Cyc_List_List*_tmp1C5;void*_tmp1C6;struct Cyc_CfFlowInfo_UnionRInfo
_tmp1C8;int _tmp1C9;int _tmp1CA;struct _dyneither_ptr _tmp1CB;_LLB3: _tmp1C2=_tmp1C1.f1;
if(_tmp1C2 == 0)goto _LLB5;_tmp1C3=*_tmp1C2;_tmp1C4=(int)_tmp1C3.hd;_tmp1C5=
_tmp1C3.tl;_tmp1C6=_tmp1C1.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp1C7=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1C6;if(
_tmp1C7->tag != 6)goto _LLB5;else{_tmp1C8=_tmp1C7->f1;_tmp1C9=_tmp1C8.is_union;
_tmp1CA=_tmp1C8.fieldnum;_tmp1CB=_tmp1C7->f2;}};_LLB4: {void*_tmp1CC=Cyc_CfFlowInfo_assign_place_outer(
env,_tmp1C5,fs_prefix + 1,*((void**)_check_dyneither_subscript(_tmp1CB,sizeof(
void*),_tmp1C4)),newval);struct _dyneither_ptr _tmp1CD=Cyc_CfFlowInfo_aggr_dict_insert((
env->fenv)->r,_tmp1CB,_tmp1C4,_tmp1CC);if((void**)_tmp1CD.curr == (void**)_tmp1CB.curr
 && (!_tmp1C9  || _tmp1CA == _tmp1C4))return oldval;_tmp1CB=_tmp1CD;if(_tmp1C9){int
changed=0;int sz=(int)_get_dyneither_size(_tmp1CB,sizeof(void*));{int i=0;for(0;i < 
sz;++ i){if(i != _tmp1C4){struct _dyneither_ptr _tmp1CE=Cyc_CfFlowInfo_aggr_dict_insert((
env->fenv)->r,_tmp1CB,i,Cyc_CfFlowInfo_insert_place_inner((env->fenv)->r,(env->fenv)->unknown_all,*((
void**)_check_dyneither_subscript(_tmp1CB,sizeof(void*),i))));if((void**)_tmp1CE.curr
!= (void**)_tmp1CB.curr){_tmp1CB=_tmp1CE;changed=1;}}}}if(changed){struct Cyc_CfFlowInfo_Place
_tmp1D0;void*_tmp1D1;struct Cyc_List_List*_tmp1D2;struct Cyc_CfFlowInfo_Place*
_tmp1CF=env->root_place;_tmp1D0=*_tmp1CF;_tmp1D1=_tmp1D0.root;_tmp1D2=_tmp1D0.fields;{
int*_tmp5CE;struct Cyc_List_List*_tmp1D3=((struct Cyc_List_List*(*)(struct
_RegionHandle*r,int(*f)(int*,int),int*env,struct Cyc_List_List*x))Cyc_List_rfilter_c)((
env->pile)->rgn,(int(*)(int*n,int unused))Cyc_CfFlowInfo_nprefix,((_tmp5CE=
_cycalloc_atomic(sizeof(*_tmp5CE)),((_tmp5CE[0]=fs_prefix,_tmp5CE)))),_tmp1D2);
struct Cyc_CfFlowInfo_Place*_tmp5CF;struct Cyc_CfFlowInfo_Place*_tmp1D4=(_tmp5CF=
_region_malloc((env->pile)->rgn,sizeof(*_tmp5CF)),((_tmp5CF->root=_tmp1D1,((
_tmp5CF->fields=_tmp1D3,_tmp5CF)))));((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*
place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,_tmp1D4,
0);};}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp5D5;struct
Cyc_CfFlowInfo_UnionRInfo _tmp5D4;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp5D3;return(void*)((_tmp5D3=_region_malloc((env->fenv)->r,sizeof(*_tmp5D3)),((
_tmp5D3[0]=((_tmp5D5.tag=6,((_tmp5D5.f1=((_tmp5D4.is_union=_tmp1C9,((_tmp5D4.fieldnum=
_tmp1C4,_tmp5D4)))),((_tmp5D5.f2=_tmp1CB,_tmp5D5)))))),_tmp5D3))));};}_LLB5:;
_LLB6: {struct Cyc_Core_Impossible_exn_struct _tmp5DB;const char*_tmp5DA;struct Cyc_Core_Impossible_exn_struct*
_tmp5D9;(int)_throw((void*)((_tmp5D9=_cycalloc(sizeof(*_tmp5D9)),((_tmp5D9[0]=((
_tmp5DB.tag=Cyc_Core_Impossible,((_tmp5DB.f1=((_tmp5DA="bad assign place",
_tag_dyneither(_tmp5DA,sizeof(char),17))),_tmp5DB)))),_tmp5D9)))));}_LLB2:;};}
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,
unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r){((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,
unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,place,0);{struct
_RegionHandle*_tmp1DD=fenv->r;struct Cyc_CfFlowInfo_Place _tmp1DF;void*_tmp1E0;
struct Cyc_List_List*_tmp1E1;struct Cyc_CfFlowInfo_Place*_tmp1DE=place;_tmp1DF=*
_tmp1DE;_tmp1E0=_tmp1DF.root;_tmp1E1=_tmp1DF.fields;{struct Cyc_CfFlowInfo_EscPile*
_tmp5DE;struct Cyc_CfFlowInfo_AssignEnv _tmp5DD;struct Cyc_CfFlowInfo_AssignEnv env=(
_tmp5DD.fenv=fenv,((_tmp5DD.pile=((_tmp5DE=_region_malloc(_tmp1DD,sizeof(*
_tmp5DE)),((_tmp5DE->rgn=_tmp1DD,((_tmp5DE->places=0,_tmp5DE)))))),((_tmp5DD.d=d,((
_tmp5DD.loc=loc,((_tmp5DD.root_place=place,((_tmp5DD.all_changed=all_changed,
_tmp5DD)))))))))));void*newval=Cyc_CfFlowInfo_assign_place_outer(& env,_tmp1E1,0,((
void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp1E0),r);return Cyc_CfFlowInfo_escape_these(
fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,
void*v))Cyc_Dict_insert)(d,_tmp1E0,newval));};};}struct Cyc_CfFlowInfo_JoinEnv{
struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict
d1;struct Cyc_Dict_Dict d2;};typedef struct Cyc_CfFlowInfo_JoinEnv*Cyc_CfFlowInfo_joinenv_t;
enum Cyc_CfFlowInfo_WhoIsChanged{Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One
 = 1,Cyc_CfFlowInfo_Two  = 2};struct Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv
joinenv;struct Cyc_Dict_Dict chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*
curr_place;struct Cyc_List_List**last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged
changed;};typedef struct Cyc_CfFlowInfo_AfterEnv*Cyc_CfFlowInfo_afterenv_t;static
int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2);struct
_tuple21{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;};
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,
void*r1,void*r2);static void _tmp5EE(struct Cyc_CfFlowInfo_JoinEnv**env,
unsigned int*_tmp5ED,unsigned int*_tmp5EC,void***_tmp5EA,struct _dyneither_ptr*
_tmp22E,struct _dyneither_ptr*_tmp228){for(*_tmp5ED=0;*_tmp5ED < *_tmp5EC;(*
_tmp5ED)++){(*_tmp5EA)[*_tmp5ED]=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int
a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp228).curr)[(
int)*_tmp5ED],*((void**)_check_dyneither_subscript(*_tmp22E,sizeof(void*),(int)*
_tmp5ED)));}}static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*
env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{struct _RegionHandle*_tmp1E4=(
env->fenv)->r;{struct _tuple0 _tmp5DF;struct _tuple0 _tmp1E6=(_tmp5DF.f1=r1,((
_tmp5DF.f2=r2,_tmp5DF)));void*_tmp1E7;struct Cyc_Absyn_Exp*_tmp1E9;int _tmp1EA;
void*_tmp1EB;void*_tmp1EC;struct Cyc_Absyn_Exp*_tmp1EE;int _tmp1EF;void*_tmp1F0;
void*_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F3;int _tmp1F4;void*_tmp1F5;void*_tmp1F6;
struct Cyc_Absyn_Exp*_tmp1F8;int _tmp1F9;void*_tmp1FA;void*_tmp1FB;struct Cyc_CfFlowInfo_Place*
_tmp1FD;void*_tmp1FE;struct Cyc_CfFlowInfo_Place*_tmp200;void*_tmp201;struct Cyc_CfFlowInfo_Place*
_tmp203;void*_tmp204;void*_tmp206;void*_tmp208;struct Cyc_CfFlowInfo_Place*
_tmp20A;void*_tmp20B;struct Cyc_CfFlowInfo_Place*_tmp20D;void*_tmp20E;void*
_tmp210;void*_tmp212;struct Cyc_CfFlowInfo_Place*_tmp214;void*_tmp215;struct Cyc_CfFlowInfo_Place*
_tmp217;void*_tmp218;struct Cyc_CfFlowInfo_Place*_tmp21A;void*_tmp21B;void*
_tmp21D;void*_tmp21F;void*_tmp221;void*_tmp223;struct Cyc_CfFlowInfo_UnionRInfo
_tmp225;int _tmp226;int _tmp227;struct _dyneither_ptr _tmp228;void*_tmp229;struct Cyc_CfFlowInfo_UnionRInfo
_tmp22B;int _tmp22C;int _tmp22D;struct _dyneither_ptr _tmp22E;_LLB8: _tmp1E7=_tmp1E6.f1;{
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1E8=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)
_tmp1E7;if(_tmp1E8->tag != 7)goto _LLBA;else{_tmp1E9=_tmp1E8->f1;_tmp1EA=_tmp1E8->f2;
_tmp1EB=(void*)_tmp1E8->f3;}};_tmp1EC=_tmp1E6.f2;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*
_tmp1ED=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1EC;if(
_tmp1ED->tag != 7)goto _LLBA;else{_tmp1EE=_tmp1ED->f1;_tmp1EF=_tmp1ED->f2;_tmp1F0=(
void*)_tmp1ED->f3;}};_LLB9: if(_tmp1E9 == _tmp1EE  && _tmp1EA == _tmp1EF){struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct
_tmp5E2;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp5E1;return(
void*)((_tmp5E1=_region_malloc(_tmp1E4,sizeof(*_tmp5E1)),((_tmp5E1[0]=((_tmp5E2.tag=
7,((_tmp5E2.f1=_tmp1E9,((_tmp5E2.f2=_tmp1EA,((_tmp5E2.f3=(void*)Cyc_CfFlowInfo_join_absRval(
env,a,_tmp1EB,_tmp1F0),_tmp5E2)))))))),_tmp5E1))));}{void*_tmp231=_tmp1EB;struct
Cyc_CfFlowInfo_Place*_tmp233;_LLD5: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp232=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp231;if(
_tmp232->tag != 5)goto _LLD7;else{_tmp233=_tmp232->f1;}}_LLD6: Cyc_CfFlowInfo_add_place(
env->pile,_tmp233);goto _LLD4;_LLD7:;_LLD8: goto _LLD4;_LLD4:;}{void*_tmp234=
_tmp1F0;struct Cyc_CfFlowInfo_Place*_tmp236;_LLDA: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp235=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp234;if(
_tmp235->tag != 5)goto _LLDC;else{_tmp236=_tmp235->f1;}}_LLDB: Cyc_CfFlowInfo_add_place(
env->pile,_tmp236);goto _LLD9;_LLDC:;_LLDD: goto _LLD9;_LLD9:;}goto _LLB7;_LLBA:
_tmp1F1=_tmp1E6.f1;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*
_tmp1F2=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1F1;if(
_tmp1F2->tag != 7)goto _LLBC;else{_tmp1F3=_tmp1F2->f1;_tmp1F4=_tmp1F2->f2;_tmp1F5=(
void*)_tmp1F2->f3;}};_LLBB: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct
_tmp5E5;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp5E4;return(
void*)((_tmp5E4=_region_malloc(_tmp1E4,sizeof(*_tmp5E4)),((_tmp5E4[0]=((_tmp5E5.tag=
7,((_tmp5E5.f1=_tmp1F3,((_tmp5E5.f2=_tmp1F4,((_tmp5E5.f3=(void*)Cyc_CfFlowInfo_join_absRval(
env,a,_tmp1F5,r2),_tmp5E5)))))))),_tmp5E4))));}_LLBC: _tmp1F6=_tmp1E6.f2;{struct
Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1F7=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)
_tmp1F6;if(_tmp1F7->tag != 7)goto _LLBE;else{_tmp1F8=_tmp1F7->f1;_tmp1F9=_tmp1F7->f2;
_tmp1FA=(void*)_tmp1F7->f3;}};_LLBD: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct
_tmp5E8;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp5E7;return(
void*)((_tmp5E7=_region_malloc(_tmp1E4,sizeof(*_tmp5E7)),((_tmp5E7[0]=((_tmp5E8.tag=
7,((_tmp5E8.f1=_tmp1F8,((_tmp5E8.f2=_tmp1F9,((_tmp5E8.f3=(void*)Cyc_CfFlowInfo_join_absRval(
env,a,r1,_tmp1FA),_tmp5E8)))))))),_tmp5E7))));}_LLBE: _tmp1FB=_tmp1E6.f1;{struct
Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1FC=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)
_tmp1FB;if(_tmp1FC->tag != 5)goto _LLC0;else{_tmp1FD=_tmp1FC->f1;}};_tmp1FE=
_tmp1E6.f2;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1FF=(
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1FE;if(_tmp1FF->tag
!= 5)goto _LLC0;else{_tmp200=_tmp1FF->f1;}};_LLBF: if(Cyc_CfFlowInfo_place_cmp(
_tmp1FD,_tmp200)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmp1FD);Cyc_CfFlowInfo_add_place(
env->pile,_tmp200);goto _LLB7;_LLC0: _tmp201=_tmp1E6.f1;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp202=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp201;if(
_tmp202->tag != 5)goto _LLC2;else{_tmp203=_tmp202->f1;}};_tmp204=_tmp1E6.f2;{
struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp205=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)
_tmp204;if(_tmp205->tag != 2)goto _LLC2;};_LLC1: _tmp20A=_tmp203;goto _LLC3;_LLC2:
_tmp206=_tmp1E6.f1;{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*
_tmp207=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp206;if(
_tmp207->tag != 2)goto _LLC4;};_tmp208=_tmp1E6.f2;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp209=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp208;if(
_tmp209->tag != 5)goto _LLC4;else{_tmp20A=_tmp209->f1;}};_LLC3: Cyc_CfFlowInfo_add_place(
env->pile,_tmp20A);return(env->fenv)->notzerothis;_LLC4: _tmp20B=_tmp1E6.f1;{
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp20C=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)
_tmp20B;if(_tmp20C->tag != 5)goto _LLC6;else{_tmp20D=_tmp20C->f1;}};_tmp20E=
_tmp1E6.f2;{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp20F=(
struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp20E;if(_tmp20F->tag
!= 1)goto _LLC6;};_LLC5: Cyc_CfFlowInfo_add_place(env->pile,_tmp20D);switch(Cyc_CfFlowInfo_initlevel(
env->fenv,env->d1,r1)){case Cyc_CfFlowInfo_AllIL: _LLDE: return(env->fenv)->notzeroall;
default: _LLDF: return(env->fenv)->notzerothis;}_LLC6: _tmp210=_tmp1E6.f1;{struct
Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp211=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)
_tmp210;if(_tmp211->tag != 1)goto _LLC8;};_tmp212=_tmp1E6.f2;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp213=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp212;if(
_tmp213->tag != 5)goto _LLC8;else{_tmp214=_tmp213->f1;}};_LLC7: Cyc_CfFlowInfo_add_place(
env->pile,_tmp214);switch(Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2)){case
Cyc_CfFlowInfo_AllIL: _LLE1: return(env->fenv)->notzeroall;default: _LLE2: return(
env->fenv)->notzerothis;}_LLC8: _tmp215=_tmp1E6.f1;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp216=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp215;if(
_tmp216->tag != 5)goto _LLCA;else{_tmp217=_tmp216->f1;}};_LLC9: Cyc_CfFlowInfo_add_place(
env->pile,_tmp217);goto _LLB7;_LLCA: _tmp218=_tmp1E6.f2;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp219=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp218;if(
_tmp219->tag != 5)goto _LLCC;else{_tmp21A=_tmp219->f1;}};_LLCB: Cyc_CfFlowInfo_add_place(
env->pile,_tmp21A);goto _LLB7;_LLCC: _tmp21B=_tmp1E6.f1;{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*
_tmp21C=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp21B;if(
_tmp21C->tag != 1)goto _LLCE;};_tmp21D=_tmp1E6.f2;{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*
_tmp21E=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp21D;if(
_tmp21E->tag != 2)goto _LLCE;};_LLCD: goto _LLCF;_LLCE: _tmp21F=_tmp1E6.f1;{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*
_tmp220=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp21F;if(
_tmp220->tag != 2)goto _LLD0;};_tmp221=_tmp1E6.f2;{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*
_tmp222=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp221;if(
_tmp222->tag != 1)goto _LLD0;};_LLCF: return(env->fenv)->notzerothis;_LLD0: _tmp223=
_tmp1E6.f1;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp224=(
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp223;if(_tmp224->tag
!= 6)goto _LLD2;else{_tmp225=_tmp224->f1;_tmp226=_tmp225.is_union;_tmp227=_tmp225.fieldnum;
_tmp228=_tmp224->f2;}};_tmp229=_tmp1E6.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp22A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp229;if(
_tmp22A->tag != 6)goto _LLD2;else{_tmp22B=_tmp22A->f1;_tmp22C=_tmp22B.is_union;
_tmp22D=_tmp22B.fieldnum;_tmp22E=_tmp22A->f2;}};_LLD1: {unsigned int _tmp5ED;
unsigned int _tmp5EC;struct _dyneither_ptr _tmp5EB;void**_tmp5EA;unsigned int
_tmp5E9;struct _dyneither_ptr new_d=(_tmp5E9=_get_dyneither_size(_tmp228,sizeof(
void*)),((_tmp5EA=(void**)_region_malloc(_tmp1E4,_check_times(sizeof(void*),
_tmp5E9)),((_tmp5EB=_tag_dyneither(_tmp5EA,sizeof(void*),_tmp5E9),((((_tmp5EC=
_tmp5E9,_tmp5EE(& env,& _tmp5ED,& _tmp5EC,& _tmp5EA,& _tmp22E,& _tmp228))),_tmp5EB)))))));
int change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp228,sizeof(void*));++ i){if(*((
void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp228.curr)[
i]){change=1;break;}}}if(!change){if(!_tmp226)return r1;new_d=_tmp228;}else{
change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp228,sizeof(void*));++ i){if(*((
void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp22E,sizeof(void*),i))){change=1;break;}}}if(!
change){if(!_tmp226)return r2;new_d=_tmp22E;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct
_tmp5F4;struct Cyc_CfFlowInfo_UnionRInfo _tmp5F3;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp5F2;return(void*)((_tmp5F2=_region_malloc(_tmp1E4,sizeof(*_tmp5F2)),((
_tmp5F2[0]=((_tmp5F4.tag=6,((_tmp5F4.f1=((_tmp5F3.is_union=_tmp226,((_tmp5F3.fieldnum=
_tmp227 == _tmp22D?_tmp227: - 1,_tmp5F3)))),((_tmp5F4.f2=new_d,_tmp5F4)))))),
_tmp5F2))));};}_LLD2:;_LLD3: goto _LLB7;_LLB7:;}{enum Cyc_CfFlowInfo_InitLevel il1=
Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel il2=
Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);struct _tuple0 _tmp5F5;struct
_tuple0 _tmp243=(_tmp5F5.f1=r1,((_tmp5F5.f2=r2,_tmp5F5)));void*_tmp244;void*
_tmp246;_LLE5: _tmp244=_tmp243.f1;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*
_tmp245=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp244;if(_tmp245->tag
!= 4)goto _LLE7;};_LLE6: goto _LLE8;_LLE7: _tmp246=_tmp243.f2;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*
_tmp247=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp246;if(_tmp247->tag
!= 4)goto _LLE9;};_LLE8: {struct _tuple21 _tmp5F6;struct _tuple21 _tmp249=(_tmp5F6.f1=
il1,((_tmp5F6.f2=il2,_tmp5F6)));enum Cyc_CfFlowInfo_InitLevel _tmp24A;enum Cyc_CfFlowInfo_InitLevel
_tmp24B;enum Cyc_CfFlowInfo_InitLevel _tmp24C;enum Cyc_CfFlowInfo_InitLevel
_tmp24D;_LLEC: _tmp24A=_tmp249.f2;if(_tmp24A != Cyc_CfFlowInfo_NoneIL)goto _LLEE;
_LLED: goto _LLEF;_LLEE: _tmp24B=_tmp249.f1;if(_tmp24B != Cyc_CfFlowInfo_NoneIL)goto
_LLF0;_LLEF: return(env->fenv)->esc_none;_LLF0: _tmp24C=_tmp249.f2;if(_tmp24C != 
Cyc_CfFlowInfo_ThisIL)goto _LLF2;_LLF1: goto _LLF3;_LLF2: _tmp24D=_tmp249.f1;if(
_tmp24D != Cyc_CfFlowInfo_ThisIL)goto _LLF4;_LLF3: return(env->fenv)->esc_this;
_LLF4:;_LLF5: return(env->fenv)->esc_all;_LLEB:;}_LLE9:;_LLEA: {struct _tuple21
_tmp5F7;struct _tuple21 _tmp24F=(_tmp5F7.f1=il1,((_tmp5F7.f2=il2,_tmp5F7)));enum 
Cyc_CfFlowInfo_InitLevel _tmp250;enum Cyc_CfFlowInfo_InitLevel _tmp251;enum Cyc_CfFlowInfo_InitLevel
_tmp252;enum Cyc_CfFlowInfo_InitLevel _tmp253;_LLF7: _tmp250=_tmp24F.f2;if(_tmp250
!= Cyc_CfFlowInfo_NoneIL)goto _LLF9;_LLF8: goto _LLFA;_LLF9: _tmp251=_tmp24F.f1;if(
_tmp251 != Cyc_CfFlowInfo_NoneIL)goto _LLFB;_LLFA: return(env->fenv)->unknown_none;
_LLFB: _tmp252=_tmp24F.f2;if(_tmp252 != Cyc_CfFlowInfo_ThisIL)goto _LLFD;_LLFC: goto
_LLFE;_LLFD: _tmp253=_tmp24F.f1;if(_tmp253 != Cyc_CfFlowInfo_ThisIL)goto _LLFF;
_LLFE: return(env->fenv)->unknown_this;_LLFF:;_LL100: return(env->fenv)->unknown_all;
_LLF6:;}_LLE4:;};};}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(
unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5F8;return((_tmp5F8.EqualConst).val=
i,(((_tmp5F8.EqualConst).tag=1,_tmp5F8)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(
struct Cyc_Absyn_Vardecl*v,void*t){struct _tuple11 _tmp5FB;union Cyc_CfFlowInfo_RelnOp
_tmp5FA;return((_tmp5FA.LessVar).val=((_tmp5FB.f1=v,((_tmp5FB.f2=t,_tmp5FB)))),(((
_tmp5FA.LessVar).tag=2,_tmp5FA)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(
struct Cyc_Absyn_Vardecl*v){union Cyc_CfFlowInfo_RelnOp _tmp5FC;return((_tmp5FC.LessNumelts).val=
v,(((_tmp5FC.LessNumelts).tag=3,_tmp5FC)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(
unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5FD;return((_tmp5FD.LessConst).val=
i,(((_tmp5FD.LessConst).tag=4,_tmp5FD)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(
unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5FE;return((_tmp5FE.LessEqConst).val=
i,(((_tmp5FE.LessEqConst).tag=6,_tmp5FE)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(
struct Cyc_Absyn_Vardecl*v){union Cyc_CfFlowInfo_RelnOp _tmp5FF;return((_tmp5FF.LessEqNumelts).val=
v,(((_tmp5FF.LessEqNumelts).tag=5,_tmp5FF)));}struct _tuple22{union Cyc_CfFlowInfo_RelnOp
f1;union Cyc_CfFlowInfo_RelnOp f2;};static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp
r1,union Cyc_CfFlowInfo_RelnOp r2){struct _tuple22 _tmp600;struct _tuple22 _tmp25C=(
_tmp600.f1=r1,((_tmp600.f2=r2,_tmp600)));union Cyc_CfFlowInfo_RelnOp _tmp25D;
unsigned int _tmp25E;union Cyc_CfFlowInfo_RelnOp _tmp25F;unsigned int _tmp260;union
Cyc_CfFlowInfo_RelnOp _tmp261;struct _tuple11 _tmp262;struct Cyc_Absyn_Vardecl*
_tmp263;union Cyc_CfFlowInfo_RelnOp _tmp264;struct _tuple11 _tmp265;struct Cyc_Absyn_Vardecl*
_tmp266;union Cyc_CfFlowInfo_RelnOp _tmp267;struct Cyc_Absyn_Vardecl*_tmp268;union
Cyc_CfFlowInfo_RelnOp _tmp269;struct Cyc_Absyn_Vardecl*_tmp26A;union Cyc_CfFlowInfo_RelnOp
_tmp26B;unsigned int _tmp26C;union Cyc_CfFlowInfo_RelnOp _tmp26D;unsigned int
_tmp26E;union Cyc_CfFlowInfo_RelnOp _tmp26F;unsigned int _tmp270;union Cyc_CfFlowInfo_RelnOp
_tmp271;unsigned int _tmp272;union Cyc_CfFlowInfo_RelnOp _tmp273;struct Cyc_Absyn_Vardecl*
_tmp274;union Cyc_CfFlowInfo_RelnOp _tmp275;struct Cyc_Absyn_Vardecl*_tmp276;_LL102:
_tmp25D=_tmp25C.f1;if((_tmp25D.EqualConst).tag != 1)goto _LL104;_tmp25E=(
unsigned int)(_tmp25D.EqualConst).val;_tmp25F=_tmp25C.f2;if((_tmp25F.EqualConst).tag
!= 1)goto _LL104;_tmp260=(unsigned int)(_tmp25F.EqualConst).val;_LL103: return
_tmp25E == _tmp260;_LL104: _tmp261=_tmp25C.f1;if((_tmp261.LessVar).tag != 2)goto
_LL106;_tmp262=(struct _tuple11)(_tmp261.LessVar).val;_tmp263=_tmp262.f1;_tmp264=
_tmp25C.f2;if((_tmp264.LessVar).tag != 2)goto _LL106;_tmp265=(struct _tuple11)(
_tmp264.LessVar).val;_tmp266=_tmp265.f1;_LL105: return _tmp263 == _tmp266;_LL106:
_tmp267=_tmp25C.f1;if((_tmp267.LessNumelts).tag != 3)goto _LL108;_tmp268=(struct
Cyc_Absyn_Vardecl*)(_tmp267.LessNumelts).val;_tmp269=_tmp25C.f2;if((_tmp269.LessNumelts).tag
!= 3)goto _LL108;_tmp26A=(struct Cyc_Absyn_Vardecl*)(_tmp269.LessNumelts).val;
_LL107: return _tmp268 == _tmp26A;_LL108: _tmp26B=_tmp25C.f1;if((_tmp26B.LessConst).tag
!= 4)goto _LL10A;_tmp26C=(unsigned int)(_tmp26B.LessConst).val;_tmp26D=_tmp25C.f2;
if((_tmp26D.LessConst).tag != 4)goto _LL10A;_tmp26E=(unsigned int)(_tmp26D.LessConst).val;
_LL109: return _tmp26C == _tmp26E;_LL10A: _tmp26F=_tmp25C.f1;if((_tmp26F.LessEqConst).tag
!= 6)goto _LL10C;_tmp270=(unsigned int)(_tmp26F.LessEqConst).val;_tmp271=_tmp25C.f2;
if((_tmp271.LessEqConst).tag != 6)goto _LL10C;_tmp272=(unsigned int)(_tmp271.LessEqConst).val;
_LL10B: return _tmp270 == _tmp272;_LL10C: _tmp273=_tmp25C.f1;if((_tmp273.LessEqNumelts).tag
!= 5)goto _LL10E;_tmp274=(struct Cyc_Absyn_Vardecl*)(_tmp273.LessEqNumelts).val;
_tmp275=_tmp25C.f2;if((_tmp275.LessEqNumelts).tag != 5)goto _LL10E;_tmp276=(struct
Cyc_Absyn_Vardecl*)(_tmp275.LessEqNumelts).val;_LL10D: return _tmp274 == _tmp276;
_LL10E:;_LL10F: return 0;_LL101:;}static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(
struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){if(r1s == 
r2s)return r1s;{struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*_tmp277=
r1s;for(0;_tmp277 != 0;_tmp277=_tmp277->tl){struct Cyc_CfFlowInfo_Reln*_tmp278=(
struct Cyc_CfFlowInfo_Reln*)_tmp277->hd;int found=0;{struct Cyc_List_List*_tmp279=
r2s;for(0;_tmp279 != 0;_tmp279=_tmp279->tl){struct Cyc_CfFlowInfo_Reln*_tmp27A=(
struct Cyc_CfFlowInfo_Reln*)_tmp279->hd;if(_tmp278 == _tmp27A  || _tmp278->vd == 
_tmp27A->vd  && Cyc_CfFlowInfo_same_relop(_tmp278->rop,_tmp27A->rop)){{struct Cyc_List_List*
_tmp601;res=((_tmp601=_region_malloc(r,sizeof(*_tmp601)),((_tmp601->hd=_tmp278,((
_tmp601->tl=res,_tmp601))))));}found=1;break;}}}if(!found)diff=1;}}if(!diff)res=
r1s;return res;};}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo
f2;};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2){struct _tuple23 _tmp602;struct _tuple23 _tmp27D=(_tmp602.f1=f1,((_tmp602.f2=f2,
_tmp602)));union Cyc_CfFlowInfo_FlowInfo _tmp27E;int _tmp27F;union Cyc_CfFlowInfo_FlowInfo
_tmp280;int _tmp281;union Cyc_CfFlowInfo_FlowInfo _tmp282;struct _tuple12 _tmp283;
struct Cyc_Dict_Dict _tmp284;struct Cyc_List_List*_tmp285;union Cyc_CfFlowInfo_FlowInfo
_tmp286;struct _tuple12 _tmp287;struct Cyc_Dict_Dict _tmp288;struct Cyc_List_List*
_tmp289;_LL111: _tmp27E=_tmp27D.f1;if((_tmp27E.BottomFL).tag != 1)goto _LL113;
_tmp27F=(int)(_tmp27E.BottomFL).val;_LL112: return f2;_LL113: _tmp280=_tmp27D.f2;
if((_tmp280.BottomFL).tag != 1)goto _LL115;_tmp281=(int)(_tmp280.BottomFL).val;
_LL114: return f1;_LL115: _tmp282=_tmp27D.f1;if((_tmp282.ReachableFL).tag != 2)goto
_LL110;_tmp283=(struct _tuple12)(_tmp282.ReachableFL).val;_tmp284=_tmp283.f1;
_tmp285=_tmp283.f2;_tmp286=_tmp27D.f2;if((_tmp286.ReachableFL).tag != 2)goto
_LL110;_tmp287=(struct _tuple12)(_tmp286.ReachableFL).val;_tmp288=_tmp287.f1;
_tmp289=_tmp287.f2;_LL116: if(_tmp284.t == _tmp288.t  && _tmp285 == _tmp289)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(
f2,f1))return f1;{struct _RegionHandle*_tmp28A=fenv->r;struct Cyc_CfFlowInfo_EscPile*
_tmp605;struct Cyc_CfFlowInfo_JoinEnv _tmp604;struct Cyc_CfFlowInfo_JoinEnv _tmp28B=(
_tmp604.fenv=fenv,((_tmp604.pile=((_tmp605=_region_malloc(_tmp28A,sizeof(*
_tmp605)),((_tmp605->rgn=_tmp28A,((_tmp605->places=0,_tmp605)))))),((_tmp604.d1=
_tmp284,((_tmp604.d2=_tmp288,_tmp604)))))));struct Cyc_Dict_Dict _tmp28C=((struct
Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),
struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))
Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,
void*r2))Cyc_CfFlowInfo_join_absRval,& _tmp28B,_tmp284,_tmp288);struct Cyc_List_List*
_tmp28D=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp285,_tmp289);return Cyc_CfFlowInfo_ReachableFL(
Cyc_CfFlowInfo_escape_these(fenv,_tmp28B.pile,all_changed,_tmp28C),_tmp28D);};
_LL110:;}struct _tuple24{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo
f2;union Cyc_CfFlowInfo_FlowInfo f3;};struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple13
pr1,struct _tuple13 pr2){union Cyc_CfFlowInfo_FlowInfo _tmp291;void*_tmp292;struct
_tuple13 _tmp290=pr1;_tmp291=_tmp290.f1;_tmp292=_tmp290.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp294;void*_tmp295;struct _tuple13 _tmp293=pr2;_tmp294=_tmp293.f1;_tmp295=
_tmp293.f2;{union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,
all_changed,_tmp291,_tmp294);struct _tuple24 _tmp606;struct _tuple24 _tmp297=(
_tmp606.f1=_tmp291,((_tmp606.f2=_tmp294,((_tmp606.f3=outflow,_tmp606)))));union
Cyc_CfFlowInfo_FlowInfo _tmp298;int _tmp299;union Cyc_CfFlowInfo_FlowInfo _tmp29A;
int _tmp29B;union Cyc_CfFlowInfo_FlowInfo _tmp29C;struct _tuple12 _tmp29D;struct Cyc_Dict_Dict
_tmp29E;union Cyc_CfFlowInfo_FlowInfo _tmp29F;struct _tuple12 _tmp2A0;struct Cyc_Dict_Dict
_tmp2A1;union Cyc_CfFlowInfo_FlowInfo _tmp2A2;struct _tuple12 _tmp2A3;struct Cyc_Dict_Dict
_tmp2A4;struct Cyc_List_List*_tmp2A5;_LL118: _tmp298=_tmp297.f1;if((_tmp298.BottomFL).tag
!= 1)goto _LL11A;_tmp299=(int)(_tmp298.BottomFL).val;_LL119: {struct _tuple13
_tmp607;return(_tmp607.f1=outflow,((_tmp607.f2=_tmp295,_tmp607)));}_LL11A:
_tmp29A=_tmp297.f2;if((_tmp29A.BottomFL).tag != 1)goto _LL11C;_tmp29B=(int)(
_tmp29A.BottomFL).val;_LL11B: {struct _tuple13 _tmp608;return(_tmp608.f1=outflow,((
_tmp608.f2=_tmp292,_tmp608)));}_LL11C: _tmp29C=_tmp297.f1;if((_tmp29C.ReachableFL).tag
!= 2)goto _LL11E;_tmp29D=(struct _tuple12)(_tmp29C.ReachableFL).val;_tmp29E=
_tmp29D.f1;_tmp29F=_tmp297.f2;if((_tmp29F.ReachableFL).tag != 2)goto _LL11E;
_tmp2A0=(struct _tuple12)(_tmp29F.ReachableFL).val;_tmp2A1=_tmp2A0.f1;_tmp2A2=
_tmp297.f3;if((_tmp2A2.ReachableFL).tag != 2)goto _LL11E;_tmp2A3=(struct _tuple12)(
_tmp2A2.ReachableFL).val;_tmp2A4=_tmp2A3.f1;_tmp2A5=_tmp2A3.f2;_LL11D: if(((int(*)(
int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp292,
_tmp295)){struct _tuple13 _tmp609;return(_tmp609.f1=outflow,((_tmp609.f2=_tmp295,
_tmp609)));}if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(
0,_tmp295,_tmp292)){struct _tuple13 _tmp60A;return(_tmp60A.f1=outflow,((_tmp60A.f2=
_tmp292,_tmp60A)));}{struct _RegionHandle*_tmp2AA=fenv->r;struct Cyc_CfFlowInfo_EscPile*
_tmp60D;struct Cyc_CfFlowInfo_JoinEnv _tmp60C;struct Cyc_CfFlowInfo_JoinEnv _tmp2AB=(
_tmp60C.fenv=fenv,((_tmp60C.pile=((_tmp60D=_region_malloc(_tmp2AA,sizeof(*
_tmp60D)),((_tmp60D->rgn=_tmp2AA,((_tmp60D->places=0,_tmp60D)))))),((_tmp60C.d1=
_tmp29E,((_tmp60C.d2=_tmp2A1,_tmp60C)))))));void*_tmp2AC=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp2AB,0,_tmp292,
_tmp295);struct _tuple13 _tmp60E;return(_tmp60E.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,_tmp2AB.pile,all_changed,_tmp2A4),_tmp2A5),((_tmp60E.f2=_tmp2AC,_tmp60E)));};
_LL11E:;_LL11F: {struct Cyc_Core_Impossible_exn_struct _tmp614;const char*_tmp613;
struct Cyc_Core_Impossible_exn_struct*_tmp612;(int)_throw((void*)((_tmp612=
_cycalloc(sizeof(*_tmp612)),((_tmp612[0]=((_tmp614.tag=Cyc_Core_Impossible,((
_tmp614.f1=((_tmp613="join_flow_and_rval: BottomFL outflow",_tag_dyneither(
_tmp613,sizeof(char),37))),_tmp614)))),_tmp612)))));}_LL117:;};};}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(
struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp61B(struct Cyc_CfFlowInfo_AfterEnv**
env,unsigned int*_tmp61A,unsigned int*_tmp619,void***_tmp617,struct
_dyneither_ptr*_tmp2C4,struct _dyneither_ptr*_tmp2BE){for(*_tmp61A=0;*_tmp61A < *
_tmp619;(*_tmp61A)++){(*_tmp617)[*_tmp61A]=Cyc_CfFlowInfo_after_absRval_d(*env,(
int)*_tmp61A,((void**)(*_tmp2BE).curr)[(int)*_tmp61A],*((void**)
_check_dyneither_subscript(*_tmp2C4,sizeof(void*),(int)*_tmp61A)));}}static void*
Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);int
changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);if(
changed1  && changed2)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*
r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);if(changed1){if(
!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))return r1;env->changed=
Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,
env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{struct _tuple0 _tmp615;
struct _tuple0 _tmp2B4=(_tmp615.f1=r1,((_tmp615.f2=r2,_tmp615)));void*_tmp2B5;void*
_tmp2B7;void*_tmp2B9;struct Cyc_CfFlowInfo_UnionRInfo _tmp2BB;int _tmp2BC;int
_tmp2BD;struct _dyneither_ptr _tmp2BE;void*_tmp2BF;struct Cyc_CfFlowInfo_UnionRInfo
_tmp2C1;int _tmp2C2;int _tmp2C3;struct _dyneither_ptr _tmp2C4;_LL121: _tmp2B5=_tmp2B4.f1;{
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp2B6=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)
_tmp2B5;if(_tmp2B6->tag != 7)goto _LL123;};_LL122: goto _LL124;_LL123: _tmp2B7=
_tmp2B4.f2;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp2B8=(
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp2B7;if(_tmp2B8->tag
!= 7)goto _LL125;};_LL124: return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,
void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);_LL125:
_tmp2B9=_tmp2B4.f1;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp2BA=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2B9;if(
_tmp2BA->tag != 6)goto _LL127;else{_tmp2BB=_tmp2BA->f1;_tmp2BC=_tmp2BB.is_union;
_tmp2BD=_tmp2BB.fieldnum;_tmp2BE=_tmp2BA->f2;}};_tmp2BF=_tmp2B4.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp2C0=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2BF;if(
_tmp2C0->tag != 6)goto _LL127;else{_tmp2C1=_tmp2C0->f1;_tmp2C2=_tmp2C1.is_union;
_tmp2C3=_tmp2C1.fieldnum;_tmp2C4=_tmp2C0->f2;}};_LL126: {unsigned int _tmp61A;
unsigned int _tmp619;struct _dyneither_ptr _tmp618;void**_tmp617;unsigned int
_tmp616;struct _dyneither_ptr new_d=(_tmp616=_get_dyneither_size(_tmp2BE,sizeof(
void*)),((_tmp617=(void**)_region_malloc(((env->joinenv).fenv)->r,_check_times(
sizeof(void*),_tmp616)),((_tmp618=_tag_dyneither(_tmp617,sizeof(void*),_tmp616),((((
_tmp619=_tmp616,_tmp61B(& env,& _tmp61A,& _tmp619,& _tmp617,& _tmp2C4,& _tmp2BE))),
_tmp618)))))));int change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp2BE,sizeof(
void*));++ i){if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((
void**)_tmp2BE.curr)[i]){change=1;break;}}}if(!change)return r1;change=0;{int i=0;
for(0;i < _get_dyneither_size(_tmp2BE,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp2C4,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp621;
struct Cyc_CfFlowInfo_UnionRInfo _tmp620;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp61F;return(void*)((_tmp61F=_region_malloc(((env->joinenv).fenv)->r,sizeof(*
_tmp61F)),((_tmp61F[0]=((_tmp621.tag=6,((_tmp621.f1=((_tmp620.is_union=_tmp2BC,((
_tmp620.fieldnum=_tmp2BD == _tmp2C3?_tmp2BD: - 1,_tmp620)))),((_tmp621.f2=new_d,
_tmp621)))))),_tmp61F))));};}_LL127:;_LL128: {struct Cyc_Core_Impossible_exn_struct
_tmp627;const char*_tmp626;struct Cyc_Core_Impossible_exn_struct*_tmp625;(int)
_throw((void*)((_tmp625=_cycalloc(sizeof(*_tmp625)),((_tmp625[0]=((_tmp627.tag=
Cyc_Core_Impossible,((_tmp627.f1=((_tmp626="after_absRval -- non-aggregates!",
_tag_dyneither(_tmp626,sizeof(char),33))),_tmp627)))),_tmp625)))));}_LL120:;};}
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,int
key,void*r1,void*r2){if(r1 == r2)return r1;{struct Cyc_List_List**_tmp2CF=env->last_field_cell;
enum Cyc_CfFlowInfo_WhoIsChanged _tmp2D0=env->changed;{struct Cyc_List_List*
_tmp628;*env->last_field_cell=((_tmp628=_region_malloc(((env->joinenv).fenv)->r,
sizeof(*_tmp628)),((_tmp628->hd=(void*)key,((_tmp628->tl=0,_tmp628))))));}env->last_field_cell=&((
struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{void*_tmp2D2=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp2CF;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp2D0;return _tmp2D2;};};}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_CfFlowInfo_Place _tmp629;*env->curr_place=((_tmp629.root=root,((_tmp629.fields=
0,_tmp629))));}env->last_field_cell=&(env->curr_place)->fields;env->changed=Cyc_CfFlowInfo_Neither;
return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo
f1,union Cyc_CfFlowInfo_FlowInfo f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict
chg2){struct _tuple23 _tmp62A;struct _tuple23 _tmp2D5=(_tmp62A.f1=f1,((_tmp62A.f2=f2,
_tmp62A)));union Cyc_CfFlowInfo_FlowInfo _tmp2D6;int _tmp2D7;union Cyc_CfFlowInfo_FlowInfo
_tmp2D8;int _tmp2D9;union Cyc_CfFlowInfo_FlowInfo _tmp2DA;struct _tuple12 _tmp2DB;
struct Cyc_Dict_Dict _tmp2DC;struct Cyc_List_List*_tmp2DD;union Cyc_CfFlowInfo_FlowInfo
_tmp2DE;struct _tuple12 _tmp2DF;struct Cyc_Dict_Dict _tmp2E0;struct Cyc_List_List*
_tmp2E1;_LL12A: _tmp2D6=_tmp2D5.f1;if((_tmp2D6.BottomFL).tag != 1)goto _LL12C;
_tmp2D7=(int)(_tmp2D6.BottomFL).val;_LL12B: goto _LL12D;_LL12C: _tmp2D8=_tmp2D5.f2;
if((_tmp2D8.BottomFL).tag != 1)goto _LL12E;_tmp2D9=(int)(_tmp2D8.BottomFL).val;
_LL12D: return Cyc_CfFlowInfo_BottomFL();_LL12E: _tmp2DA=_tmp2D5.f1;if((_tmp2DA.ReachableFL).tag
!= 2)goto _LL129;_tmp2DB=(struct _tuple12)(_tmp2DA.ReachableFL).val;_tmp2DC=
_tmp2DB.f1;_tmp2DD=_tmp2DB.f2;_tmp2DE=_tmp2D5.f2;if((_tmp2DE.ReachableFL).tag != 
2)goto _LL129;_tmp2DF=(struct _tuple12)(_tmp2DE.ReachableFL).val;_tmp2E0=_tmp2DF.f1;
_tmp2E1=_tmp2DF.f2;_LL12F: if(_tmp2DC.t == _tmp2E0.t  && _tmp2DD == _tmp2E1)return f1;{
struct _RegionHandle*_tmp2E2=fenv->r;struct Cyc_CfFlowInfo_Place*_tmp2E3=fenv->dummy_place;
struct Cyc_CfFlowInfo_JoinEnv _tmp630;struct Cyc_CfFlowInfo_EscPile*_tmp62F;struct
Cyc_CfFlowInfo_AfterEnv _tmp62E;struct Cyc_CfFlowInfo_AfterEnv _tmp2E4=(_tmp62E.joinenv=((
_tmp630.fenv=fenv,((_tmp630.pile=((_tmp62F=_region_malloc(_tmp2E2,sizeof(*
_tmp62F)),((_tmp62F->rgn=_tmp2E2,((_tmp62F->places=0,_tmp62F)))))),((_tmp630.d1=
_tmp2DC,((_tmp630.d2=_tmp2E0,_tmp630)))))))),((_tmp62E.chg1=chg1,((_tmp62E.chg2=
chg2,((_tmp62E.curr_place=_tmp2E3,((_tmp62E.last_field_cell=& _tmp2E3->fields,((
_tmp62E.changed=Cyc_CfFlowInfo_Neither,_tmp62E)))))))))));struct Cyc_Dict_Dict
_tmp2E5=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,
void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,& _tmp2E4,_tmp2DC,_tmp2E0);
struct Cyc_List_List*_tmp2E6=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp2DD,_tmp2E1);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,(_tmp2E4.joinenv).pile,
all_changed,_tmp2E5),_tmp2E6);};_LL129:;}static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp631;struct
_tuple0 _tmp2EB=(_tmp631.f1=r1,((_tmp631.f2=r2,_tmp631)));void*_tmp2EC;struct Cyc_CfFlowInfo_Place*
_tmp2EE;void*_tmp2EF;struct Cyc_CfFlowInfo_Place*_tmp2F1;void*_tmp2F2;void*
_tmp2F4;void*_tmp2F6;struct Cyc_CfFlowInfo_UnionRInfo _tmp2F8;struct _dyneither_ptr
_tmp2F9;void*_tmp2FA;struct Cyc_CfFlowInfo_UnionRInfo _tmp2FC;struct _dyneither_ptr
_tmp2FD;void*_tmp2FE;void*_tmp300;void*_tmp302;void*_tmp304;void*_tmp306;void*
_tmp308;void*_tmp30A;void*_tmp30C;_LL131: _tmp2EC=_tmp2EB.f1;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp2ED=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2EC;if(
_tmp2ED->tag != 5)goto _LL133;else{_tmp2EE=_tmp2ED->f1;}};_tmp2EF=_tmp2EB.f2;{
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2F0=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)
_tmp2EF;if(_tmp2F0->tag != 5)goto _LL133;else{_tmp2F1=_tmp2F0->f1;}};_LL132: return
Cyc_CfFlowInfo_place_cmp(_tmp2EE,_tmp2F1)== 0;_LL133: _tmp2F2=_tmp2EB.f1;{struct
Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2F3=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)
_tmp2F2;if(_tmp2F3->tag != 5)goto _LL135;};_LL134: goto _LL136;_LL135: _tmp2F4=
_tmp2EB.f2;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2F5=(
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2F4;if(_tmp2F5->tag
!= 5)goto _LL137;};_LL136: return 0;_LL137: _tmp2F6=_tmp2EB.f1;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp2F7=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2F6;if(
_tmp2F7->tag != 6)goto _LL139;else{_tmp2F8=_tmp2F7->f1;_tmp2F9=_tmp2F7->f2;}};
_tmp2FA=_tmp2EB.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp2FB=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2FA;if(
_tmp2FB->tag != 6)goto _LL139;else{_tmp2FC=_tmp2FB->f1;_tmp2FD=_tmp2FB->f2;}};
_LL138: if((void**)_tmp2F9.curr == (void**)_tmp2FD.curr)return 1;{int i=0;for(0;i < 
_get_dyneither_size(_tmp2F9,sizeof(void*));++ i){if(!((int(*)(int ignore,void*r1,
void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp2F9.curr)[i],*((
void**)_check_dyneither_subscript(_tmp2FD,sizeof(void*),i))))return 0;}}return 1;
_LL139: _tmp2FE=_tmp2EB.f1;{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*
_tmp2FF=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp2FE;if(
_tmp2FF->tag != 1)goto _LL13B;};_tmp300=_tmp2EB.f2;{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*
_tmp301=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp300;if(
_tmp301->tag != 2)goto _LL13B;};_LL13A: return 1;_LL13B: _tmp302=_tmp2EB.f2;{struct
Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp303=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)
_tmp302;if(_tmp303->tag != 2)goto _LL13D;};_LL13C: return 0;_LL13D: _tmp304=_tmp2EB.f2;{
struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp305=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)
_tmp304;if(_tmp305->tag != 0)goto _LL13F;};_LL13E: goto _LL140;_LL13F: _tmp306=
_tmp2EB.f2;{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp307=(
struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp306;if(_tmp307->tag
!= 1)goto _LL141;};_LL140: return 0;_LL141: _tmp308=_tmp2EB.f1;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*
_tmp309=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp308;if(_tmp309->tag
!= 4)goto _LL143;};_tmp30A=_tmp2EB.f2;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*
_tmp30B=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp30A;if(_tmp30B->tag
!= 4)goto _LL143;};_LL142: goto _LL130;_LL143: _tmp30C=_tmp2EB.f1;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*
_tmp30D=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp30C;if(_tmp30D->tag
!= 4)goto _LL145;};_LL144: return 0;_LL145:;_LL146: goto _LL130;_LL130:;}{struct
_tuple21 _tmp632;struct _tuple21 _tmp30F=(_tmp632.f1=Cyc_CfFlowInfo_initlevel_approx(
r1),((_tmp632.f2=Cyc_CfFlowInfo_initlevel_approx(r2),_tmp632)));enum Cyc_CfFlowInfo_InitLevel
_tmp310;enum Cyc_CfFlowInfo_InitLevel _tmp311;enum Cyc_CfFlowInfo_InitLevel
_tmp312;enum Cyc_CfFlowInfo_InitLevel _tmp313;enum Cyc_CfFlowInfo_InitLevel
_tmp314;enum Cyc_CfFlowInfo_InitLevel _tmp315;_LL148: _tmp310=_tmp30F.f1;if(
_tmp310 != Cyc_CfFlowInfo_AllIL)goto _LL14A;_tmp311=_tmp30F.f2;if(_tmp311 != Cyc_CfFlowInfo_AllIL)
goto _LL14A;_LL149: return 1;_LL14A: _tmp312=_tmp30F.f2;if(_tmp312 != Cyc_CfFlowInfo_NoneIL)
goto _LL14C;_LL14B: return 1;_LL14C: _tmp313=_tmp30F.f1;if(_tmp313 != Cyc_CfFlowInfo_NoneIL)
goto _LL14E;_LL14D: return 0;_LL14E: _tmp314=_tmp30F.f2;if(_tmp314 != Cyc_CfFlowInfo_ThisIL)
goto _LL150;_LL14F: return 1;_LL150: _tmp315=_tmp30F.f1;if(_tmp315 != Cyc_CfFlowInfo_ThisIL)
goto _LL147;_LL151: return 0;_LL147:;};}static int Cyc_CfFlowInfo_relns_approx(struct
Cyc_List_List*r2s,struct Cyc_List_List*r1s){if(r1s == r2s)return 1;for(0;r1s != 0;
r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*_tmp316=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;
int found=0;{struct Cyc_List_List*_tmp317=r2s;for(0;_tmp317 != 0;_tmp317=_tmp317->tl){
struct Cyc_CfFlowInfo_Reln*_tmp318=(struct Cyc_CfFlowInfo_Reln*)_tmp317->hd;if(
_tmp316 == _tmp318  || _tmp316->vd == _tmp318->vd  && Cyc_CfFlowInfo_same_relop(
_tmp316->rop,_tmp318->rop)){found=1;break;}}}if(!found)return 0;}return 1;}int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){struct _tuple23
_tmp633;struct _tuple23 _tmp31A=(_tmp633.f1=f1,((_tmp633.f2=f2,_tmp633)));union Cyc_CfFlowInfo_FlowInfo
_tmp31B;int _tmp31C;union Cyc_CfFlowInfo_FlowInfo _tmp31D;int _tmp31E;union Cyc_CfFlowInfo_FlowInfo
_tmp31F;struct _tuple12 _tmp320;struct Cyc_Dict_Dict _tmp321;struct Cyc_List_List*
_tmp322;union Cyc_CfFlowInfo_FlowInfo _tmp323;struct _tuple12 _tmp324;struct Cyc_Dict_Dict
_tmp325;struct Cyc_List_List*_tmp326;_LL153: _tmp31B=_tmp31A.f1;if((_tmp31B.BottomFL).tag
!= 1)goto _LL155;_tmp31C=(int)(_tmp31B.BottomFL).val;_LL154: return 1;_LL155:
_tmp31D=_tmp31A.f2;if((_tmp31D.BottomFL).tag != 1)goto _LL157;_tmp31E=(int)(
_tmp31D.BottomFL).val;_LL156: return 0;_LL157: _tmp31F=_tmp31A.f1;if((_tmp31F.ReachableFL).tag
!= 2)goto _LL152;_tmp320=(struct _tuple12)(_tmp31F.ReachableFL).val;_tmp321=
_tmp320.f1;_tmp322=_tmp320.f2;_tmp323=_tmp31A.f2;if((_tmp323.ReachableFL).tag != 
2)goto _LL152;_tmp324=(struct _tuple12)(_tmp323.ReachableFL).val;_tmp325=_tmp324.f1;
_tmp326=_tmp324.f2;_LL158: if(_tmp321.t == _tmp325.t  && _tmp322 == _tmp326)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp321,_tmp325) && Cyc_CfFlowInfo_relns_approx(_tmp322,_tmp326);_LL152:;}struct
Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*
rs,struct Cyc_Absyn_Vardecl*v){struct Cyc_List_List*p;int found=0;for(p=rs;!found
 && p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp327=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp327->vd == v){found=1;break;}{union Cyc_CfFlowInfo_RelnOp _tmp328=
_tmp327->rop;struct _tuple11 _tmp329;struct Cyc_Absyn_Vardecl*_tmp32A;struct Cyc_Absyn_Vardecl*
_tmp32B;struct Cyc_Absyn_Vardecl*_tmp32C;_LL15A: if((_tmp328.LessVar).tag != 2)goto
_LL15C;_tmp329=(struct _tuple11)(_tmp328.LessVar).val;_tmp32A=_tmp329.f1;_LL15B:
_tmp32B=_tmp32A;goto _LL15D;_LL15C: if((_tmp328.LessNumelts).tag != 3)goto _LL15E;
_tmp32B=(struct Cyc_Absyn_Vardecl*)(_tmp328.LessNumelts).val;_LL15D: _tmp32C=
_tmp32B;goto _LL15F;_LL15E: if((_tmp328.LessEqNumelts).tag != 5)goto _LL160;_tmp32C=(
struct Cyc_Absyn_Vardecl*)(_tmp328.LessEqNumelts).val;_LL15F: if(v == _tmp32C)found=
1;goto _LL159;_LL160:;_LL161: goto _LL159;_LL159:;};}if(!found)return rs;{struct Cyc_List_List*
_tmp32D=0;for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp32E=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp32E->vd != v){{union Cyc_CfFlowInfo_RelnOp _tmp32F=_tmp32E->rop;struct
_tuple11 _tmp330;struct Cyc_Absyn_Vardecl*_tmp331;struct Cyc_Absyn_Vardecl*_tmp332;
struct Cyc_Absyn_Vardecl*_tmp333;_LL163: if((_tmp32F.LessVar).tag != 2)goto _LL165;
_tmp330=(struct _tuple11)(_tmp32F.LessVar).val;_tmp331=_tmp330.f1;_LL164: _tmp332=
_tmp331;goto _LL166;_LL165: if((_tmp32F.LessNumelts).tag != 3)goto _LL167;_tmp332=(
struct Cyc_Absyn_Vardecl*)(_tmp32F.LessNumelts).val;_LL166: _tmp333=_tmp332;goto
_LL168;_LL167: if((_tmp32F.LessEqNumelts).tag != 5)goto _LL169;_tmp333=(struct Cyc_Absyn_Vardecl*)(
_tmp32F.LessEqNumelts).val;_LL168: if(v == _tmp333)continue;goto _LL162;_LL169:;
_LL16A: goto _LL162;_LL162:;}{struct Cyc_List_List*_tmp634;_tmp32D=((_tmp634=
_region_malloc(rgn,sizeof(*_tmp634)),((_tmp634->hd=_tmp32E,((_tmp634->tl=_tmp32D,
_tmp634))))));};}}return _tmp32D;};}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*
_tmp335=e->r;void*_tmp337;struct Cyc_Absyn_Vardecl*_tmp339;void*_tmp33B;struct Cyc_Absyn_Vardecl*
_tmp33D;void*_tmp33F;struct Cyc_Absyn_Vardecl*_tmp341;void*_tmp343;struct Cyc_Absyn_Vardecl*
_tmp345;_LL16C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp336=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp335;if(_tmp336->tag != 1)goto _LL16E;else{_tmp337=(void*)_tmp336->f2;{struct
Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp338=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)
_tmp337;if(_tmp338->tag != 1)goto _LL16E;else{_tmp339=_tmp338->f1;}};}}_LL16D:
_tmp33D=_tmp339;goto _LL16F;_LL16E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp33A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp335;if(_tmp33A->tag != 1)
goto _LL170;else{_tmp33B=(void*)_tmp33A->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*
_tmp33C=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp33B;if(_tmp33C->tag != 
3)goto _LL170;else{_tmp33D=_tmp33C->f1;}};}}_LL16F: _tmp341=_tmp33D;goto _LL171;
_LL170: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp33E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp335;if(_tmp33E->tag != 1)goto _LL172;else{_tmp33F=(void*)_tmp33E->f2;{struct
Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp340=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)
_tmp33F;if(_tmp340->tag != 4)goto _LL172;else{_tmp341=_tmp340->f1;}};}}_LL171:
_tmp345=_tmp341;goto _LL173;_LL172: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp342=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp335;if(_tmp342->tag != 1)
goto _LL174;else{_tmp343=(void*)_tmp342->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*
_tmp344=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp343;if(_tmp344->tag != 5)
goto _LL174;else{_tmp345=_tmp344->f1;}};}}_LL173: if(!_tmp345->escapes)return Cyc_CfFlowInfo_reln_kill_var(
rgn,r,_tmp345);goto _LL16B;_LL174:;_LL175: goto _LL16B;_LL16B:;}return r;}struct Cyc_CfFlowInfo_Reln*
Cyc_CfFlowInfo_copy_reln(struct _RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r){
struct Cyc_CfFlowInfo_Reln*_tmp635;return(_tmp635=_region_malloc(r2,sizeof(*
_tmp635)),((_tmp635[0]=*r,_tmp635)));}struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(
struct _RegionHandle*r2,struct Cyc_List_List*relns){return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*f)(struct _RegionHandle*,struct
Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*
r1,struct Cyc_List_List*r2){for(0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){struct
Cyc_CfFlowInfo_Reln*_tmp347=(struct Cyc_CfFlowInfo_Reln*)r1->hd;struct Cyc_CfFlowInfo_Reln*
_tmp348=(struct Cyc_CfFlowInfo_Reln*)r2->hd;if(_tmp347->vd != _tmp348->vd  || !Cyc_CfFlowInfo_same_relop(
_tmp347->rop,_tmp348->rop))return 0;}if(r1 != 0  || r2 != 0)return 0;else{return 1;}}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,
struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes)
return r;r=Cyc_CfFlowInfo_reln_kill_var(rgn,r,v);{void*_tmp349=e->r;struct Cyc_Absyn_MallocInfo
_tmp34B;struct Cyc_Absyn_Exp*_tmp34C;int _tmp34D;_LL177: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*
_tmp34A=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp349;if(_tmp34A->tag
!= 32)goto _LL179;else{_tmp34B=_tmp34A->f1;_tmp34C=_tmp34B.num_elts;_tmp34D=
_tmp34B.fat_result;if(_tmp34D != 1)goto _LL179;}}_LL178: malloc_loop: {void*_tmp34E=
_tmp34C->r;struct Cyc_Absyn_Exp*_tmp350;void*_tmp352;struct Cyc_Absyn_Vardecl*
_tmp354;void*_tmp356;struct Cyc_Absyn_Vardecl*_tmp358;void*_tmp35A;struct Cyc_Absyn_Vardecl*
_tmp35C;void*_tmp35E;struct Cyc_Absyn_Vardecl*_tmp360;_LL17C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*
_tmp34F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp34E;if(_tmp34F->tag != 
13)goto _LL17E;else{_tmp350=_tmp34F->f2;}}_LL17D: _tmp34C=_tmp350;goto malloc_loop;
_LL17E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp351=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp34E;if(_tmp351->tag != 1)goto _LL180;else{_tmp352=(void*)_tmp351->f2;{struct
Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp353=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)
_tmp352;if(_tmp353->tag != 5)goto _LL180;else{_tmp354=_tmp353->f1;}};}}_LL17F:
_tmp358=_tmp354;goto _LL181;_LL180: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp355=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp34E;if(_tmp355->tag != 1)
goto _LL182;else{_tmp356=(void*)_tmp355->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmp357=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp356;if(_tmp357->tag != 
4)goto _LL182;else{_tmp358=_tmp357->f1;}};}}_LL181: _tmp35C=_tmp358;goto _LL183;
_LL182: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp359=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp34E;if(_tmp359->tag != 1)goto _LL184;else{_tmp35A=(void*)_tmp359->f2;{struct
Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp35B=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)
_tmp35A;if(_tmp35B->tag != 3)goto _LL184;else{_tmp35C=_tmp35B->f1;}};}}_LL183:
_tmp360=_tmp35C;goto _LL185;_LL184: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp35D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp34E;if(_tmp35D->tag != 1)
goto _LL186;else{_tmp35E=(void*)_tmp35D->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp35F=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp35E;if(_tmp35F->tag
!= 1)goto _LL186;else{_tmp360=_tmp35F->f1;}};}}_LL185: if(_tmp360->escapes)return r;{
struct Cyc_CfFlowInfo_Reln*_tmp638;struct Cyc_List_List*_tmp637;return(_tmp637=
_region_malloc(rgn,sizeof(*_tmp637)),((_tmp637->hd=((_tmp638=_region_malloc(rgn,
sizeof(*_tmp638)),((_tmp638->vd=_tmp360,((_tmp638->rop=Cyc_CfFlowInfo_LessEqNumelts(
v),_tmp638)))))),((_tmp637->tl=r,_tmp637)))));};_LL186:;_LL187: return r;_LL17B:;}
_LL179:;_LL17A: goto _LL176;_LL176:;}{void*_tmp363=Cyc_Tcutil_compress(v->type);
_LL189: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp364=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp363;if(_tmp364->tag != 6)goto _LL18B;}_LL18A: goto _LL188;_LL18B:;_LL18C: return r;
_LL188:;}loop: {void*_tmp365=e->r;struct Cyc_Absyn_Exp*_tmp367;union Cyc_Absyn_Cnst
_tmp369;struct _tuple6 _tmp36A;int _tmp36B;enum Cyc_Absyn_Primop _tmp36D;struct Cyc_List_List*
_tmp36E;struct Cyc_List_List _tmp36F;struct Cyc_List_List*_tmp370;struct Cyc_List_List
_tmp371;struct Cyc_Absyn_Exp*_tmp372;enum Cyc_Absyn_Primop _tmp374;struct Cyc_List_List*
_tmp375;struct Cyc_List_List _tmp376;struct Cyc_Absyn_Exp*_tmp377;_LL18E: {struct
Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp366=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)
_tmp365;if(_tmp366->tag != 13)goto _LL190;else{_tmp367=_tmp366->f2;}}_LL18F: e=
_tmp367;goto loop;_LL190: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp368=(
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp365;if(_tmp368->tag != 0)goto
_LL192;else{_tmp369=_tmp368->f1;if((_tmp369.Int_c).tag != 5)goto _LL192;_tmp36A=(
struct _tuple6)(_tmp369.Int_c).val;_tmp36B=_tmp36A.f2;}}_LL191: {struct Cyc_CfFlowInfo_Reln*
_tmp63B;struct Cyc_List_List*_tmp63A;return(_tmp63A=_region_malloc(rgn,sizeof(*
_tmp63A)),((_tmp63A->hd=((_tmp63B=_region_malloc(rgn,sizeof(*_tmp63B)),((_tmp63B->vd=
v,((_tmp63B->rop=Cyc_CfFlowInfo_EqualConst((unsigned int)_tmp36B),_tmp63B)))))),((
_tmp63A->tl=r,_tmp63A)))));}_LL192: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*
_tmp36C=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp365;if(_tmp36C->tag
!= 2)goto _LL194;else{_tmp36D=_tmp36C->f1;if(_tmp36D != Cyc_Absyn_Mod)goto _LL194;
_tmp36E=_tmp36C->f2;if(_tmp36E == 0)goto _LL194;_tmp36F=*_tmp36E;_tmp370=_tmp36F.tl;
if(_tmp370 == 0)goto _LL194;_tmp371=*_tmp370;_tmp372=(struct Cyc_Absyn_Exp*)_tmp371.hd;}}
_LL193:{void*_tmp37A=_tmp372->r;enum Cyc_Absyn_Primop _tmp37C;struct Cyc_List_List*
_tmp37D;struct Cyc_List_List _tmp37E;struct Cyc_Absyn_Exp*_tmp37F;_LL199: {struct
Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp37B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)
_tmp37A;if(_tmp37B->tag != 2)goto _LL19B;else{_tmp37C=_tmp37B->f1;_tmp37D=_tmp37B->f2;
if(_tmp37D == 0)goto _LL19B;_tmp37E=*_tmp37D;_tmp37F=(struct Cyc_Absyn_Exp*)_tmp37E.hd;}}
_LL19A:{void*_tmp380=_tmp37F->r;void*_tmp382;struct Cyc_Absyn_Vardecl*_tmp384;
void*_tmp386;struct Cyc_Absyn_Vardecl*_tmp388;void*_tmp38A;struct Cyc_Absyn_Vardecl*
_tmp38C;void*_tmp38E;struct Cyc_Absyn_Vardecl*_tmp390;_LL19E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp381=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp380;if(_tmp381->tag != 1)
goto _LL1A0;else{_tmp382=(void*)_tmp381->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp383=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp382;if(_tmp383->tag
!= 1)goto _LL1A0;else{_tmp384=_tmp383->f1;}};}}_LL19F: _tmp388=_tmp384;goto _LL1A1;
_LL1A0: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp385=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp380;if(_tmp385->tag != 1)goto _LL1A2;else{_tmp386=(void*)_tmp385->f2;{struct
Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp387=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)
_tmp386;if(_tmp387->tag != 4)goto _LL1A2;else{_tmp388=_tmp387->f1;}};}}_LL1A1:
_tmp38C=_tmp388;goto _LL1A3;_LL1A2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp389=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp380;if(_tmp389->tag != 1)
goto _LL1A4;else{_tmp38A=(void*)_tmp389->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*
_tmp38B=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp38A;if(_tmp38B->tag != 
3)goto _LL1A4;else{_tmp38C=_tmp38B->f1;}};}}_LL1A3: _tmp390=_tmp38C;goto _LL1A5;
_LL1A4: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp38D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp380;if(_tmp38D->tag != 1)goto _LL1A6;else{_tmp38E=(void*)_tmp38D->f2;{struct
Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp38F=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)
_tmp38E;if(_tmp38F->tag != 5)goto _LL1A6;else{_tmp390=_tmp38F->f1;}};}}_LL1A5: if(
_tmp390->escapes)return r;{struct Cyc_CfFlowInfo_Reln*_tmp63E;struct Cyc_List_List*
_tmp63D;return(_tmp63D=_region_malloc(rgn,sizeof(*_tmp63D)),((_tmp63D->hd=((
_tmp63E=_region_malloc(rgn,sizeof(*_tmp63E)),((_tmp63E->vd=v,((_tmp63E->rop=Cyc_CfFlowInfo_LessNumelts(
_tmp390),_tmp63E)))))),((_tmp63D->tl=r,_tmp63D)))));};_LL1A6:;_LL1A7: goto _LL19D;
_LL19D:;}goto _LL198;_LL19B:;_LL19C: goto _LL198;_LL198:;}goto _LL18D;_LL194: {
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp373=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)
_tmp365;if(_tmp373->tag != 2)goto _LL196;else{_tmp374=_tmp373->f1;_tmp375=_tmp373->f2;
if(_tmp375 == 0)goto _LL196;_tmp376=*_tmp375;_tmp377=(struct Cyc_Absyn_Exp*)_tmp376.hd;}}
_LL195:{void*_tmp393=_tmp377->r;void*_tmp395;struct Cyc_Absyn_Vardecl*_tmp397;
void*_tmp399;struct Cyc_Absyn_Vardecl*_tmp39B;void*_tmp39D;struct Cyc_Absyn_Vardecl*
_tmp39F;void*_tmp3A1;struct Cyc_Absyn_Vardecl*_tmp3A3;_LL1A9: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp394=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp393;if(_tmp394->tag != 1)
goto _LL1AB;else{_tmp395=(void*)_tmp394->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp396=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp395;if(_tmp396->tag
!= 1)goto _LL1AB;else{_tmp397=_tmp396->f1;}};}}_LL1AA: _tmp39B=_tmp397;goto _LL1AC;
_LL1AB: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp398=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp393;if(_tmp398->tag != 1)goto _LL1AD;else{_tmp399=(void*)_tmp398->f2;{struct
Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp39A=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)
_tmp399;if(_tmp39A->tag != 4)goto _LL1AD;else{_tmp39B=_tmp39A->f1;}};}}_LL1AC:
_tmp39F=_tmp39B;goto _LL1AE;_LL1AD: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp39C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp393;if(_tmp39C->tag != 1)
goto _LL1AF;else{_tmp39D=(void*)_tmp39C->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*
_tmp39E=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp39D;if(_tmp39E->tag != 
3)goto _LL1AF;else{_tmp39F=_tmp39E->f1;}};}}_LL1AE: _tmp3A3=_tmp39F;goto _LL1B0;
_LL1AF: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3A0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp393;if(_tmp3A0->tag != 1)goto _LL1B1;else{_tmp3A1=(void*)_tmp3A0->f2;{struct
Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp3A2=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)
_tmp3A1;if(_tmp3A2->tag != 5)goto _LL1B1;else{_tmp3A3=_tmp3A2->f1;}};}}_LL1B0: if(
_tmp3A3->escapes)return r;{struct Cyc_CfFlowInfo_Reln*_tmp641;struct Cyc_List_List*
_tmp640;return(_tmp640=_region_malloc(rgn,sizeof(*_tmp640)),((_tmp640->hd=((
_tmp641=_region_malloc(rgn,sizeof(*_tmp641)),((_tmp641->vd=v,((_tmp641->rop=Cyc_CfFlowInfo_LessEqNumelts(
_tmp3A3),_tmp641)))))),((_tmp640->tl=r,_tmp640)))));};_LL1B1:;_LL1B2: goto _LL1A8;
_LL1A8:;}goto _LL18D;_LL196:;_LL197: goto _LL18D;_LL18D:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*_tmp3A6=e1->r;void*_tmp3A8;
struct Cyc_Absyn_Vardecl*_tmp3AA;void*_tmp3AC;struct Cyc_Absyn_Vardecl*_tmp3AE;
void*_tmp3B0;struct Cyc_Absyn_Vardecl*_tmp3B2;void*_tmp3B4;struct Cyc_Absyn_Vardecl*
_tmp3B6;_LL1B4: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3A7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp3A6;if(_tmp3A7->tag != 1)goto _LL1B6;else{_tmp3A8=(void*)_tmp3A7->f2;{struct
Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp3A9=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)
_tmp3A8;if(_tmp3A9->tag != 1)goto _LL1B6;else{_tmp3AA=_tmp3A9->f1;}};}}_LL1B5:
_tmp3AE=_tmp3AA;goto _LL1B7;_LL1B6: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp3AB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A6;if(_tmp3AB->tag != 1)
goto _LL1B8;else{_tmp3AC=(void*)_tmp3AB->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*
_tmp3AD=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp3AC;if(_tmp3AD->tag != 
3)goto _LL1B8;else{_tmp3AE=_tmp3AD->f1;}};}}_LL1B7: _tmp3B2=_tmp3AE;goto _LL1B9;
_LL1B8: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3AF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp3A6;if(_tmp3AF->tag != 1)goto _LL1BA;else{_tmp3B0=(void*)_tmp3AF->f2;{struct
Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3B1=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)
_tmp3B0;if(_tmp3B1->tag != 4)goto _LL1BA;else{_tmp3B2=_tmp3B1->f1;}};}}_LL1B9:
_tmp3B6=_tmp3B2;goto _LL1BB;_LL1BA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp3B3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A6;if(_tmp3B3->tag != 1)
goto _LL1BC;else{_tmp3B4=(void*)_tmp3B3->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*
_tmp3B5=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp3B4;if(_tmp3B5->tag != 5)
goto _LL1BC;else{_tmp3B6=_tmp3B5->f1;}};}}_LL1BB: if(!_tmp3B6->escapes)return Cyc_CfFlowInfo_reln_assign_var(
rgn,r,_tmp3B6,e2);goto _LL1B3;_LL1BC:;_LL1BD: goto _LL1B3;_LL1B3:;}return r;}static
void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*r){{const char*_tmp645;
void*_tmp644[1];struct Cyc_String_pa_PrintArg_struct _tmp643;(_tmp643.tag=0,((
_tmp643.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((
r->vd)->name)),((_tmp644[0]=& _tmp643,Cyc_fprintf(Cyc_stderr,((_tmp645="%s",
_tag_dyneither(_tmp645,sizeof(char),3))),_tag_dyneither(_tmp644,sizeof(void*),1)))))));}{
union Cyc_CfFlowInfo_RelnOp _tmp3BA=r->rop;unsigned int _tmp3BB;struct _tuple11
_tmp3BC;struct Cyc_Absyn_Vardecl*_tmp3BD;struct Cyc_Absyn_Vardecl*_tmp3BE;
unsigned int _tmp3BF;unsigned int _tmp3C0;struct Cyc_Absyn_Vardecl*_tmp3C1;_LL1BF:
if((_tmp3BA.EqualConst).tag != 1)goto _LL1C1;_tmp3BB=(unsigned int)(_tmp3BA.EqualConst).val;
_LL1C0:{const char*_tmp649;void*_tmp648[1];struct Cyc_Int_pa_PrintArg_struct
_tmp647;(_tmp647.tag=1,((_tmp647.f1=(unsigned long)((int)_tmp3BB),((_tmp648[0]=&
_tmp647,Cyc_fprintf(Cyc_stderr,((_tmp649="==%d",_tag_dyneither(_tmp649,sizeof(
char),5))),_tag_dyneither(_tmp648,sizeof(void*),1)))))));}goto _LL1BE;_LL1C1: if((
_tmp3BA.LessVar).tag != 2)goto _LL1C3;_tmp3BC=(struct _tuple11)(_tmp3BA.LessVar).val;
_tmp3BD=_tmp3BC.f1;_LL1C2:{const char*_tmp64D;void*_tmp64C[1];struct Cyc_String_pa_PrintArg_struct
_tmp64B;(_tmp64B.tag=0,((_tmp64B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp3BD->name)),((_tmp64C[0]=& _tmp64B,Cyc_fprintf(Cyc_stderr,((
_tmp64D="<%s",_tag_dyneither(_tmp64D,sizeof(char),4))),_tag_dyneither(_tmp64C,
sizeof(void*),1)))))));}goto _LL1BE;_LL1C3: if((_tmp3BA.LessNumelts).tag != 3)goto
_LL1C5;_tmp3BE=(struct Cyc_Absyn_Vardecl*)(_tmp3BA.LessNumelts).val;_LL1C4:{const
char*_tmp651;void*_tmp650[1];struct Cyc_String_pa_PrintArg_struct _tmp64F;(_tmp64F.tag=
0,((_tmp64F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp3BE->name)),((_tmp650[0]=& _tmp64F,Cyc_fprintf(Cyc_stderr,((_tmp651="<numelts(%s)",
_tag_dyneither(_tmp651,sizeof(char),13))),_tag_dyneither(_tmp650,sizeof(void*),1)))))));}
goto _LL1BE;_LL1C5: if((_tmp3BA.LessConst).tag != 4)goto _LL1C7;_tmp3BF=(
unsigned int)(_tmp3BA.LessConst).val;_LL1C6:{const char*_tmp655;void*_tmp654[1];
struct Cyc_Int_pa_PrintArg_struct _tmp653;(_tmp653.tag=1,((_tmp653.f1=(
unsigned long)((int)_tmp3BF),((_tmp654[0]=& _tmp653,Cyc_fprintf(Cyc_stderr,((
_tmp655="<%d",_tag_dyneither(_tmp655,sizeof(char),4))),_tag_dyneither(_tmp654,
sizeof(void*),1)))))));}goto _LL1BE;_LL1C7: if((_tmp3BA.LessEqConst).tag != 6)goto
_LL1C9;_tmp3C0=(unsigned int)(_tmp3BA.LessEqConst).val;_LL1C8:{const char*_tmp659;
void*_tmp658[1];struct Cyc_Int_pa_PrintArg_struct _tmp657;(_tmp657.tag=1,((_tmp657.f1=(
unsigned long)((int)_tmp3C0),((_tmp658[0]=& _tmp657,Cyc_fprintf(Cyc_stderr,((
_tmp659="<=%d",_tag_dyneither(_tmp659,sizeof(char),5))),_tag_dyneither(_tmp658,
sizeof(void*),1)))))));}goto _LL1BE;_LL1C9: if((_tmp3BA.LessEqNumelts).tag != 5)
goto _LL1BE;_tmp3C1=(struct Cyc_Absyn_Vardecl*)(_tmp3BA.LessEqNumelts).val;_LL1CA:{
const char*_tmp65D;void*_tmp65C[1];struct Cyc_String_pa_PrintArg_struct _tmp65B;(
_tmp65B.tag=0,((_tmp65B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp3C1->name)),((_tmp65C[0]=& _tmp65B,Cyc_fprintf(Cyc_stderr,((_tmp65D="<=numelts(%s)",
_tag_dyneither(_tmp65D,sizeof(char),14))),_tag_dyneither(_tmp65C,sizeof(void*),1)))))));}
goto _LL1BE;_LL1BE:;};}void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){for(
0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)r->hd);
if(r->tl != 0){const char*_tmp660;void*_tmp65F;(_tmp65F=0,Cyc_fprintf(Cyc_stderr,((
_tmp660=",",_tag_dyneither(_tmp660,sizeof(char),2))),_tag_dyneither(_tmp65F,
sizeof(void*),0)));}}}void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel
il){switch(il){case Cyc_CfFlowInfo_NoneIL: _LL1CB:{const char*_tmp663;void*_tmp662;(
_tmp662=0,Cyc_fprintf(Cyc_stderr,((_tmp663="uninitialized",_tag_dyneither(
_tmp663,sizeof(char),14))),_tag_dyneither(_tmp662,sizeof(void*),0)));}break;case
Cyc_CfFlowInfo_ThisIL: _LL1CC:{const char*_tmp666;void*_tmp665;(_tmp665=0,Cyc_fprintf(
Cyc_stderr,((_tmp666="this-initialized",_tag_dyneither(_tmp666,sizeof(char),17))),
_tag_dyneither(_tmp665,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_AllIL: _LL1CD:{
const char*_tmp669;void*_tmp668;(_tmp668=0,Cyc_fprintf(Cyc_stderr,((_tmp669="all-initialized",
_tag_dyneither(_tmp669,sizeof(char),16))),_tag_dyneither(_tmp668,sizeof(void*),0)));}
break;}}void Cyc_CfFlowInfo_print_root(void*root){void*_tmp3DC=root;struct Cyc_Absyn_Vardecl*
_tmp3DE;struct Cyc_Absyn_Exp*_tmp3E0;void*_tmp3E1;_LL1D0: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*
_tmp3DD=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp3DC;if(_tmp3DD->tag
!= 0)goto _LL1D2;else{_tmp3DE=_tmp3DD->f1;}}_LL1D1:{const char*_tmp66D;void*
_tmp66C[1];struct Cyc_String_pa_PrintArg_struct _tmp66B;(_tmp66B.tag=0,((_tmp66B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3DE->name)),((
_tmp66C[0]=& _tmp66B,Cyc_fprintf(Cyc_stderr,((_tmp66D="Root(%s)",_tag_dyneither(
_tmp66D,sizeof(char),9))),_tag_dyneither(_tmp66C,sizeof(void*),1)))))));}goto
_LL1CF;_LL1D2: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp3DF=(
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp3DC;if(_tmp3DF->tag != 
1)goto _LL1D4;else{_tmp3E0=_tmp3DF->f1;_tmp3E1=(void*)_tmp3DF->f2;}}_LL1D3:{const
char*_tmp672;void*_tmp671[2];struct Cyc_String_pa_PrintArg_struct _tmp670;struct
Cyc_String_pa_PrintArg_struct _tmp66F;(_tmp66F.tag=0,((_tmp66F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp3E1)),((_tmp670.tag=
0,((_tmp670.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
_tmp3E0)),((_tmp671[0]=& _tmp670,((_tmp671[1]=& _tmp66F,Cyc_fprintf(Cyc_stderr,((
_tmp672="MallocPt(%s,%s)",_tag_dyneither(_tmp672,sizeof(char),16))),
_tag_dyneither(_tmp671,sizeof(void*),2)))))))))))));}goto _LL1CF;_LL1D4: {struct
Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp3E2=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)
_tmp3DC;if(_tmp3E2->tag != 2)goto _LL1CF;}_LL1D5:{const char*_tmp675;void*_tmp674;(
_tmp674=0,Cyc_fprintf(Cyc_stderr,((_tmp675="InitParam(_,_)",_tag_dyneither(
_tmp675,sizeof(char),15))),_tag_dyneither(_tmp674,sizeof(void*),0)));}goto _LL1CF;
_LL1CF:;}void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){Cyc_CfFlowInfo_print_root(
p->root);{struct Cyc_List_List*_tmp3EC=p->fields;while(_tmp3EC != 0){{const char*
_tmp679;void*_tmp678[1];struct Cyc_Int_pa_PrintArg_struct _tmp677;(_tmp677.tag=1,((
_tmp677.f1=(unsigned long)((int)_tmp3EC->hd),((_tmp678[0]=& _tmp677,Cyc_fprintf(
Cyc_stderr,((_tmp679=".%d",_tag_dyneither(_tmp679,sizeof(char),4))),
_tag_dyneither(_tmp678,sizeof(void*),1)))))));}_tmp3EC=_tmp3EC->tl;}{const char*
_tmp67C;void*_tmp67B;(_tmp67B=0,Cyc_fprintf(Cyc_stderr,((_tmp67C=" ",
_tag_dyneither(_tmp67C,sizeof(char),2))),_tag_dyneither(_tmp67B,sizeof(void*),0)));};};}
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){int _tmp3F2=
1;while(x != 0){if(!_tmp3F2){{const char*_tmp67F;void*_tmp67E;(_tmp67E=0,Cyc_fprintf(
Cyc_stderr,((_tmp67F=", ",_tag_dyneither(_tmp67F,sizeof(char),3))),
_tag_dyneither(_tmp67E,sizeof(void*),0)));}_tmp3F2=0;}pr((void*)x->hd);x=x->tl;}{
const char*_tmp682;void*_tmp681;(_tmp681=0,Cyc_fprintf(Cyc_stderr,((_tmp682="\n",
_tag_dyneither(_tmp682,sizeof(char),2))),_tag_dyneither(_tmp681,sizeof(void*),0)));};}
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*)){struct
_RegionHandle _tmp3F7=_new_region("r");struct _RegionHandle*r=& _tmp3F7;
_push_region(r);{struct _handler_cons _tmp3F8;_push_handler(& _tmp3F8);{int _tmp3FA=
0;if(setjmp(_tmp3F8.handler))_tmp3FA=1;if(!_tmp3FA){{struct Cyc_Iter_Iter iter=((
struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
r,p);struct _tuple14 _tmp3FB=*((struct _tuple14*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict
d))Cyc_Dict_rchoose)(r,p);int first=1;{const char*_tmp685;void*_tmp684;(_tmp684=0,
Cyc_fprintf(Cyc_stderr,((_tmp685="{ ",_tag_dyneither(_tmp685,sizeof(char),3))),
_tag_dyneither(_tmp684,sizeof(void*),0)));}while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple14*))Cyc_Iter_next)(iter,& _tmp3FB)){void*_tmp3FE=_tmp3FB.f1;if(!
first){{const char*_tmp688;void*_tmp687;(_tmp687=0,Cyc_fprintf(Cyc_stderr,((
_tmp688=", ",_tag_dyneither(_tmp688,sizeof(char),3))),_tag_dyneither(_tmp687,
sizeof(void*),0)));}first=0;}pr(_tmp3FE);}{const char*_tmp68B;void*_tmp68A;(
_tmp68A=0,Cyc_fprintf(Cyc_stderr,((_tmp68B="}\n",_tag_dyneither(_tmp68B,sizeof(
char),3))),_tag_dyneither(_tmp68A,sizeof(void*),0)));};};_pop_handler();}else{
void*_tmp3F9=(void*)_exn_thrown;void*_tmp404=_tmp3F9;_LL1D7:;_LL1D8:{const char*
_tmp68E;void*_tmp68D;(_tmp68D=0,Cyc_fprintf(Cyc_stderr,((_tmp68E="{ }\n",
_tag_dyneither(_tmp68E,sizeof(char),5))),_tag_dyneither(_tmp68D,sizeof(void*),0)));}
_npop_handler(0);return;_LL1D9:;_LL1DA:(void)_throw(_tmp404);_LL1D6:;}};};
_pop_region(r);}void Cyc_CfFlowInfo_print_absrval(void*rval){void*_tmp407=rval;
enum Cyc_CfFlowInfo_InitLevel _tmp40C;enum Cyc_CfFlowInfo_InitLevel _tmp40E;struct
Cyc_CfFlowInfo_Place*_tmp410;struct Cyc_CfFlowInfo_UnionRInfo _tmp412;int _tmp413;
int _tmp414;struct _dyneither_ptr _tmp415;struct Cyc_Absyn_Exp*_tmp417;int _tmp418;
void*_tmp419;_LL1DC: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*
_tmp408=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp407;if(_tmp408->tag
!= 0)goto _LL1DE;}_LL1DD:{const char*_tmp691;void*_tmp690;(_tmp690=0,Cyc_fprintf(
Cyc_stderr,((_tmp691="Zero",_tag_dyneither(_tmp691,sizeof(char),5))),
_tag_dyneither(_tmp690,sizeof(void*),0)));}goto _LL1DB;_LL1DE: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*
_tmp409=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp407;if(
_tmp409->tag != 1)goto _LL1E0;}_LL1DF:{const char*_tmp694;void*_tmp693;(_tmp693=0,
Cyc_fprintf(Cyc_stderr,((_tmp694="NotZeroAll",_tag_dyneither(_tmp694,sizeof(char),
11))),_tag_dyneither(_tmp693,sizeof(void*),0)));}goto _LL1DB;_LL1E0: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*
_tmp40A=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp407;if(
_tmp40A->tag != 2)goto _LL1E2;}_LL1E1:{const char*_tmp697;void*_tmp696;(_tmp696=0,
Cyc_fprintf(Cyc_stderr,((_tmp697="NotZeroThis",_tag_dyneither(_tmp697,sizeof(
char),12))),_tag_dyneither(_tmp696,sizeof(void*),0)));}goto _LL1DB;_LL1E2: {
struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp40B=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)
_tmp407;if(_tmp40B->tag != 3)goto _LL1E4;else{_tmp40C=_tmp40B->f1;}}_LL1E3:{const
char*_tmp69A;void*_tmp699;(_tmp699=0,Cyc_fprintf(Cyc_stderr,((_tmp69A="Unknown(",
_tag_dyneither(_tmp69A,sizeof(char),9))),_tag_dyneither(_tmp699,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_initlevel(_tmp40C);{const char*_tmp69D;void*_tmp69C;(_tmp69C=
0,Cyc_fprintf(Cyc_stderr,((_tmp69D=")",_tag_dyneither(_tmp69D,sizeof(char),2))),
_tag_dyneither(_tmp69C,sizeof(void*),0)));}goto _LL1DB;_LL1E4: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*
_tmp40D=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp407;if(_tmp40D->tag
!= 4)goto _LL1E6;else{_tmp40E=_tmp40D->f1;}}_LL1E5:{const char*_tmp6A0;void*
_tmp69F;(_tmp69F=0,Cyc_fprintf(Cyc_stderr,((_tmp6A0="Esc(",_tag_dyneither(
_tmp6A0,sizeof(char),5))),_tag_dyneither(_tmp69F,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(
_tmp40E);{const char*_tmp6A3;void*_tmp6A2;(_tmp6A2=0,Cyc_fprintf(Cyc_stderr,((
_tmp6A3=")",_tag_dyneither(_tmp6A3,sizeof(char),2))),_tag_dyneither(_tmp6A2,
sizeof(void*),0)));}goto _LL1DB;_LL1E6: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp40F=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp407;if(
_tmp40F->tag != 5)goto _LL1E8;else{_tmp410=_tmp40F->f1;}}_LL1E7:{const char*_tmp6A6;
void*_tmp6A5;(_tmp6A5=0,Cyc_fprintf(Cyc_stderr,((_tmp6A6="AddrOf(",
_tag_dyneither(_tmp6A6,sizeof(char),8))),_tag_dyneither(_tmp6A5,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_place(_tmp410);{const char*_tmp6A9;void*_tmp6A8;(_tmp6A8=0,
Cyc_fprintf(Cyc_stderr,((_tmp6A9=")",_tag_dyneither(_tmp6A9,sizeof(char),2))),
_tag_dyneither(_tmp6A8,sizeof(void*),0)));}goto _LL1DB;_LL1E8: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp411=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp407;if(
_tmp411->tag != 6)goto _LL1EA;else{_tmp412=_tmp411->f1;_tmp413=_tmp412.is_union;
_tmp414=_tmp412.fieldnum;_tmp415=_tmp411->f2;}}_LL1E9: if(_tmp413){{const char*
_tmp6AC;void*_tmp6AB;(_tmp6AB=0,Cyc_fprintf(Cyc_stderr,((_tmp6AC="AggrUnion{",
_tag_dyneither(_tmp6AC,sizeof(char),11))),_tag_dyneither(_tmp6AB,sizeof(void*),0)));}
if(_tmp414 != - 1){const char*_tmp6B0;void*_tmp6AF[1];struct Cyc_Int_pa_PrintArg_struct
_tmp6AE;(_tmp6AE.tag=1,((_tmp6AE.f1=(unsigned long)_tmp414,((_tmp6AF[0]=& _tmp6AE,
Cyc_fprintf(Cyc_stderr,((_tmp6B0="tag == %d;",_tag_dyneither(_tmp6B0,sizeof(char),
11))),_tag_dyneither(_tmp6AF,sizeof(void*),1)))))));}}else{const char*_tmp6B3;
void*_tmp6B2;(_tmp6B2=0,Cyc_fprintf(Cyc_stderr,((_tmp6B3="AggrStruct{",
_tag_dyneither(_tmp6B3,sizeof(char),12))),_tag_dyneither(_tmp6B2,sizeof(void*),0)));}{
int i=0;for(0;i < _get_dyneither_size(_tmp415,sizeof(void*));++ i){Cyc_CfFlowInfo_print_absrval(((
void**)_tmp415.curr)[i]);if(i + 1 < _get_dyneither_size(_tmp415,sizeof(void*))){
const char*_tmp6B6;void*_tmp6B5;(_tmp6B5=0,Cyc_fprintf(Cyc_stderr,((_tmp6B6=",",
_tag_dyneither(_tmp6B6,sizeof(char),2))),_tag_dyneither(_tmp6B5,sizeof(void*),0)));}}}{
const char*_tmp6B9;void*_tmp6B8;(_tmp6B8=0,Cyc_fprintf(Cyc_stderr,((_tmp6B9="}",
_tag_dyneither(_tmp6B9,sizeof(char),2))),_tag_dyneither(_tmp6B8,sizeof(void*),0)));}
goto _LL1DB;_LL1EA: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*
_tmp416=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp407;if(
_tmp416->tag != 7)goto _LL1DB;else{_tmp417=_tmp416->f1;_tmp418=_tmp416->f2;_tmp419=(
void*)_tmp416->f3;}}_LL1EB:{const char*_tmp6BE;void*_tmp6BD[2];struct Cyc_String_pa_PrintArg_struct
_tmp6BC;struct Cyc_Int_pa_PrintArg_struct _tmp6BB;(_tmp6BB.tag=1,((_tmp6BB.f1=(
unsigned long)_tmp418,((_tmp6BC.tag=0,((_tmp6BC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp417)),((_tmp6BD[0]=& _tmp6BC,((
_tmp6BD[1]=& _tmp6BB,Cyc_fprintf(Cyc_stderr,((_tmp6BE="[Consumed(%s,%d,",
_tag_dyneither(_tmp6BE,sizeof(char),17))),_tag_dyneither(_tmp6BD,sizeof(void*),2)))))))))))));}
Cyc_CfFlowInfo_print_absrval(_tmp419);{const char*_tmp6C1;void*_tmp6C0;(_tmp6C0=0,
Cyc_fprintf(Cyc_stderr,((_tmp6C1="]",_tag_dyneither(_tmp6C1,sizeof(char),2))),
_tag_dyneither(_tmp6C0,sizeof(void*),0)));};_LL1DB:;}static void Cyc_CfFlowInfo_print_flow_mapping(
void*root,void*rval){{const char*_tmp6C4;void*_tmp6C3;(_tmp6C3=0,Cyc_fprintf(Cyc_stderr,((
_tmp6C4="    ",_tag_dyneither(_tmp6C4,sizeof(char),5))),_tag_dyneither(_tmp6C3,
sizeof(void*),0)));}Cyc_CfFlowInfo_print_root(root);{const char*_tmp6C7;void*
_tmp6C6;(_tmp6C6=0,Cyc_fprintf(Cyc_stderr,((_tmp6C7=" --> ",_tag_dyneither(
_tmp6C7,sizeof(char),6))),_tag_dyneither(_tmp6C6,sizeof(void*),0)));}Cyc_CfFlowInfo_print_absrval(
rval);{const char*_tmp6CA;void*_tmp6C9;(_tmp6C9=0,Cyc_fprintf(Cyc_stderr,((
_tmp6CA="\n",_tag_dyneither(_tmp6CA,sizeof(char),2))),_tag_dyneither(_tmp6C9,
sizeof(void*),0)));};}void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){((
void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,
d);}void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo
_tmp443=f;int _tmp444;struct _tuple12 _tmp445;struct Cyc_Dict_Dict _tmp446;_LL1ED: if((
_tmp443.BottomFL).tag != 1)goto _LL1EF;_tmp444=(int)(_tmp443.BottomFL).val;_LL1EE:{
const char*_tmp6CD;void*_tmp6CC;(_tmp6CC=0,Cyc_fprintf(Cyc_stderr,((_tmp6CD="  BottomFL\n",
_tag_dyneither(_tmp6CD,sizeof(char),12))),_tag_dyneither(_tmp6CC,sizeof(void*),0)));}
goto _LL1EC;_LL1EF: if((_tmp443.ReachableFL).tag != 2)goto _LL1EC;_tmp445=(struct
_tuple12)(_tmp443.ReachableFL).val;_tmp446=_tmp445.f1;_LL1F0:{const char*_tmp6D0;
void*_tmp6CF;(_tmp6CF=0,Cyc_fprintf(Cyc_stderr,((_tmp6D0="  ReachableFL:\n",
_tag_dyneither(_tmp6D0,sizeof(char),16))),_tag_dyneither(_tmp6CF,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_flowdict(_tmp446);{const char*_tmp6D3;void*_tmp6D2;(_tmp6D2=
0,Cyc_fprintf(Cyc_stderr,((_tmp6D3="\n",_tag_dyneither(_tmp6D3,sizeof(char),2))),
_tag_dyneither(_tmp6D2,sizeof(void*),0)));}goto _LL1EC;_LL1EC:;}int Cyc_CfFlowInfo_contains_region(
void*rgn,void*t){void*_tmp44D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp45A;struct Cyc_Absyn_DatatypeInfo _tmp45C;struct Cyc_List_List*_tmp45D;struct
Cyc_List_List*_tmp45F;struct Cyc_Absyn_AggrInfo _tmp461;struct Cyc_List_List*
_tmp462;struct Cyc_Absyn_DatatypeFieldInfo _tmp464;struct Cyc_List_List*_tmp465;
struct Cyc_List_List*_tmp467;struct Cyc_Absyn_PtrInfo _tmp469;void*_tmp46A;struct
Cyc_Absyn_PtrAtts _tmp46B;void*_tmp46C;struct Cyc_List_List*_tmp46F;struct Cyc_List_List*
_tmp471;struct Cyc_Absyn_ArrayInfo _tmp473;void*_tmp474;void*_tmp476;void*_tmp478;
void*_tmp47A;void*_tmp47B;void*_tmp47D;_LL1F2: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*
_tmp44E=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp44D;if(_tmp44E->tag
!= 26)goto _LL1F4;}_LL1F3: goto _LL1F5;_LL1F4: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*
_tmp44F=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp44D;if(_tmp44F->tag != 0)
goto _LL1F6;}_LL1F5: goto _LL1F7;_LL1F6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp450=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp44D;if(_tmp450->tag != 6)
goto _LL1F8;}_LL1F7: goto _LL1F9;_LL1F8: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp451=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp44D;if(_tmp451->tag != 
7)goto _LL1FA;}_LL1F9: goto _LL1FB;_LL1FA: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp452=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp44D;if(_tmp452->tag != 
13)goto _LL1FC;}_LL1FB: goto _LL1FD;_LL1FC: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*
_tmp453=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp44D;if(_tmp453->tag
!= 14)goto _LL1FE;}_LL1FD: goto _LL1FF;_LL1FE: {struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmp454=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp44D;if(_tmp454->tag != 19)
goto _LL200;}_LL1FF: goto _LL201;_LL200: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*
_tmp455=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp44D;if(_tmp455->tag != 20)
goto _LL202;}_LL201: goto _LL203;_LL202: {struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp456=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp44D;if(_tmp456->tag != 1)
goto _LL204;}_LL203: return 0;_LL204: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*
_tmp457=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp44D;if(_tmp457->tag != 
21)goto _LL206;}_LL205: {void*_tmp47F=rgn;_LL229: {struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*
_tmp480=(struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*)_tmp47F;if(
_tmp480->tag != 0)goto _LL22B;}_LL22A: return 1;_LL22B:;_LL22C: return 0;_LL228:;}
_LL206: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp458=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)
_tmp44D;if(_tmp458->tag != 22)goto _LL208;}_LL207: {void*_tmp481=rgn;_LL22E: {
struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*_tmp482=(struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*)
_tmp481;if(_tmp482->tag != 1)goto _LL230;}_LL22F: return 1;_LL230:;_LL231: return 0;
_LL22D:;}_LL208: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp459=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)
_tmp44D;if(_tmp459->tag != 2)goto _LL20A;else{_tmp45A=_tmp459->f1;}}_LL209: {void*
_tmp483=rgn;struct Cyc_Absyn_Tvar*_tmp487;_LL233: {struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*
_tmp484=(struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*)_tmp483;if(
_tmp484->tag != 1)goto _LL235;}_LL234: goto _LL236;_LL235: {struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*
_tmp485=(struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*)_tmp483;if(
_tmp485->tag != 0)goto _LL237;}_LL236: return 0;_LL237: {struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct*
_tmp486=(struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct*)_tmp483;if(
_tmp486->tag != 2)goto _LL232;else{_tmp487=_tmp486->f1;}}_LL238: return Cyc_Absyn_tvar_cmp(
_tmp45A,_tmp487)== 0;_LL232:;}_LL20A: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*
_tmp45B=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp44D;if(_tmp45B->tag
!= 3)goto _LL20C;else{_tmp45C=_tmp45B->f1;_tmp45D=_tmp45C.targs;}}_LL20B: _tmp45F=
_tmp45D;goto _LL20D;_LL20C: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*
_tmp45E=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp44D;if(_tmp45E->tag
!= 17)goto _LL20E;else{_tmp45F=_tmp45E->f2;}}_LL20D: _tmp462=_tmp45F;goto _LL20F;
_LL20E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp460=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp44D;if(_tmp460->tag != 11)goto _LL210;else{_tmp461=_tmp460->f1;_tmp462=_tmp461.targs;}}
_LL20F: _tmp465=_tmp462;goto _LL211;_LL210: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmp463=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp44D;if(_tmp463->tag
!= 4)goto _LL212;else{_tmp464=_tmp463->f1;_tmp465=_tmp464.targs;}}_LL211: _tmp467=
_tmp465;goto _LL213;_LL212: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp466=(
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp44D;if(_tmp466->tag != 24)goto
_LL214;else{_tmp467=_tmp466->f1;}}_LL213: return((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_CfFlowInfo_contains_region,
rgn,_tmp467);_LL214: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp468=(
struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44D;if(_tmp468->tag != 5)goto
_LL216;else{_tmp469=_tmp468->f1;_tmp46A=_tmp469.elt_typ;_tmp46B=_tmp469.ptr_atts;
_tmp46C=_tmp46B.rgn;}}_LL215: return Cyc_CfFlowInfo_contains_region(rgn,_tmp46C)
 || Cyc_CfFlowInfo_contains_region(rgn,_tmp46A);_LL216: {struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp46D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44D;if(_tmp46D->tag != 9)
goto _LL218;}_LL217: return 0;_LL218: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmp46E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp44D;if(_tmp46E->tag != 
10)goto _LL21A;else{_tmp46F=_tmp46E->f1;}}_LL219: for(0;_tmp46F != 0;_tmp46F=
_tmp46F->tl){if(Cyc_CfFlowInfo_contains_region(rgn,(*((struct _tuple17*)_tmp46F->hd)).f2))
return 1;}return 0;_LL21A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp470=(
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp44D;if(_tmp470->tag != 12)
goto _LL21C;else{_tmp471=_tmp470->f2;}}_LL21B: for(0;_tmp471 != 0;_tmp471=_tmp471->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp471->hd)->type))
return 1;}return 0;_LL21C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp472=(
struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp44D;if(_tmp472->tag != 8)goto
_LL21E;else{_tmp473=_tmp472->f1;_tmp474=_tmp473.elt_type;}}_LL21D: _tmp476=
_tmp474;goto _LL21F;_LL21E: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp475=(
struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp44D;if(_tmp475->tag != 23)goto
_LL220;else{_tmp476=(void*)_tmp475->f1;}}_LL21F: _tmp478=_tmp476;goto _LL221;
_LL220: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp477=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)
_tmp44D;if(_tmp477->tag != 25)goto _LL222;else{_tmp478=(void*)_tmp477->f1;}}_LL221:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp478);_LL222: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*
_tmp479=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp44D;if(_tmp479->tag != 
16)goto _LL224;else{_tmp47A=(void*)_tmp479->f1;_tmp47B=(void*)_tmp479->f2;}}
_LL223: return Cyc_CfFlowInfo_contains_region(rgn,_tmp47A) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp47B);_LL224: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp47C=(
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp44D;if(_tmp47C->tag != 15)
goto _LL226;else{_tmp47D=(void*)_tmp47C->f1;}}_LL225: return 0;_LL226: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*
_tmp47E=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp44D;if(_tmp47E->tag
!= 18)goto _LL1F1;}_LL227: return 0;_LL1F1:;}struct _tuple25{struct Cyc_CfFlowInfo_FlowEnv*
f1;struct Cyc_Dict_Dict f2;void*f3;};static void Cyc_CfFlowInfo_kill_root(struct
_tuple25*env,void*root,void*rval){struct _tuple25 _tmp489;struct Cyc_CfFlowInfo_FlowEnv*
_tmp48A;struct Cyc_Dict_Dict _tmp48B;struct Cyc_Dict_Dict*_tmp48C;void*_tmp48D;
struct _tuple25*_tmp488=env;_tmp489=*_tmp488;_tmp48A=_tmp489.f1;_tmp48B=_tmp489.f2;
_tmp48C=(struct Cyc_Dict_Dict*)&(*_tmp488).f2;_tmp48D=_tmp489.f3;{void*_tmp48E=
root;struct Cyc_Absyn_Vardecl*_tmp490;void*_tmp492;_LL23A: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*
_tmp48F=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp48E;if(_tmp48F->tag
!= 0)goto _LL23C;else{_tmp490=_tmp48F->f1;}}_LL23B: if(Cyc_CfFlowInfo_contains_region(
_tmp48D,_tmp490->type))rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp48A,_tmp490->type,
0,_tmp48A->unknown_none);*_tmp48C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(*_tmp48C,root,rval);goto _LL239;_LL23C: {struct
Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp491=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)
_tmp48E;if(_tmp491->tag != 1)goto _LL23E;else{_tmp492=(void*)_tmp491->f2;}}_LL23D:
if(!Cyc_CfFlowInfo_contains_region(_tmp48D,_tmp492))*_tmp48C=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp48C,root,rval);goto
_LL239;_LL23E: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp493=(
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp48E;if(_tmp493->tag != 
2)goto _LL239;}_LL23F: goto _LL239;_LL239:;};}static struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict fd,void*rgn){void*_tmp494=
Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp496;_LL241: {struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp495=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp494;if(_tmp495->tag != 2)
goto _LL243;else{_tmp496=_tmp495->f1;}}_LL242: {struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct
_tmp6D4;struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct v=(_tmp6D4.tag=2,((
_tmp6D4.f1=_tmp496,_tmp6D4)));void*v2=(void*)& v;struct _tuple25 _tmp6D5;struct
_tuple25 env=(_tmp6D5.f1=fenv,((_tmp6D5.f2=fenv->mt_flowdict,((_tmp6D5.f3=v2,
_tmp6D5)))));((void(*)(void(*f)(struct _tuple25*,void*,void*),struct _tuple25*env,
struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);return
env.f2;}_LL243: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp497=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)
_tmp494;if(_tmp497->tag != 21)goto _LL245;}_LL244: {struct _tuple25 _tmp6D6;struct
_tuple25 env=(_tmp6D6.f1=fenv,((_tmp6D6.f2=fenv->mt_flowdict,((_tmp6D6.f3=(void*)&
Cyc_CfFlowInfo_UniqueRgn_k_val,_tmp6D6)))));((void(*)(void(*f)(struct _tuple25*,
void*,void*),struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,&
env,fd);return env.f2;}_LL245: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*
_tmp498=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp494;if(_tmp498->tag != 
22)goto _LL247;}_LL246: {struct _tuple25 _tmp6D7;struct _tuple25 env=(_tmp6D7.f1=fenv,((
_tmp6D7.f2=fenv->mt_flowdict,((_tmp6D7.f3=(void*)& Cyc_CfFlowInfo_RefCntRgn_k_val,
_tmp6D7)))));((void(*)(void(*f)(struct _tuple25*,void*,void*),struct _tuple25*env,
struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);return
env.f2;}_LL247:;_LL248: {struct Cyc_Core_Impossible_exn_struct _tmp6DD;const char*
_tmp6DC;struct Cyc_Core_Impossible_exn_struct*_tmp6DB;(int)_throw((void*)((
_tmp6DB=_cycalloc(sizeof(*_tmp6DB)),((_tmp6DB[0]=((_tmp6DD.tag=Cyc_Core_Impossible,((
_tmp6DD.f1=((_tmp6DC="kill_flowdict_region",_tag_dyneither(_tmp6DC,sizeof(char),
21))),_tmp6DD)))),_tmp6DB)))));}_LL240:;}static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn){return 0;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){union
Cyc_CfFlowInfo_FlowInfo _tmp4A0=f;int _tmp4A1;struct _tuple12 _tmp4A2;struct Cyc_Dict_Dict
_tmp4A3;struct Cyc_List_List*_tmp4A4;_LL24A: if((_tmp4A0.BottomFL).tag != 1)goto
_LL24C;_tmp4A1=(int)(_tmp4A0.BottomFL).val;_LL24B: return f;_LL24C: if((_tmp4A0.ReachableFL).tag
!= 2)goto _LL249;_tmp4A2=(struct _tuple12)(_tmp4A0.ReachableFL).val;_tmp4A3=
_tmp4A2.f1;_tmp4A4=_tmp4A2.f2;_LL24D: {struct Cyc_Dict_Dict _tmp4A5=Cyc_CfFlowInfo_kill_flowdict_region(
fenv,_tmp4A3,rgn);struct Cyc_List_List*_tmp4A6=Cyc_CfFlowInfo_kill_relns_region(
_tmp4A4,rgn);return Cyc_CfFlowInfo_ReachableFL(_tmp4A5,_tmp4A6);}_LL249:;}
