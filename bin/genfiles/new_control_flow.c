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
void*v;};typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;
};void*Cyc_Core_snd(struct _tuple0*);int Cyc_Core_ptrcmp(void*,void*);extern char
Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*
tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[
12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[
12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};inline static void* arrcast(
struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){if(bd >> 20  || sz >> 12)
return 0;{unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < dyn.curr  || dyn.curr == 
0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)return 0;return dyn.curr;};}
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};typedef struct Cyc_List_List*
Cyc_List_list_t;typedef struct Cyc_List_List*Cyc_List_List_t;struct Cyc_List_List*
Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,
void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];
struct Cyc_List_List_mismatch_exn_struct{char*tag;};void Cyc_List_iter_c(void(*f)(
void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{
char*tag;};void*Cyc_List_nth(struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_rzip(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;int
Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;typedef struct Cyc_Set_Set*
Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*
tag;};struct Cyc_Dict_T;typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};typedef
struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*
tag;};struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);void Cyc_Dict_iter_c(
void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Hashtable_Table;
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;struct Cyc_Hashtable_Table*
Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*
hash)(void*));void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*
val);void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);struct
Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int
line_no;int col;};typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[
5];struct Cyc_Position_Exit_exn_struct{char*tag;};typedef unsigned int Cyc_Position_seg_t;
struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{
int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};typedef void*
Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;
unsigned int seg;void*kind;struct _dyneither_ptr desc;};typedef struct Cyc_Position_Error*
Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{
char*tag;};int Cyc_Position_error_p();typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int
tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};typedef union Cyc_Absyn_Nmspace
Cyc_Absyn_nmspace_t;union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct
_dyneither_ptr*f2;};typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*
Cyc_Absyn_qvar_opt_t;typedef struct _tuple1*Cyc_Absyn_typedef_name_t;typedef struct
_tuple1*Cyc_Absyn_typedef_name_opt_t;typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;
typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;typedef void*Cyc_Absyn_kindbound_t;
typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;
typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;typedef void*Cyc_Absyn_bounds_t;
typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;typedef struct Cyc_Absyn_PtrInfo
Cyc_Absyn_ptr_info_t;typedef struct Cyc_Absyn_VarargInfo Cyc_Absyn_vararg_info_t;
typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;typedef struct Cyc_Absyn_DatatypeInfo
Cyc_Absyn_datatype_info_t;typedef struct Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_datatype_field_info_t;
typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;typedef struct Cyc_Absyn_ArrayInfo
Cyc_Absyn_array_info_t;typedef void*Cyc_Absyn_type_t;typedef void*Cyc_Absyn_rgntype_t;
typedef void*Cyc_Absyn_type_opt_t;typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;
typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;typedef enum Cyc_Absyn_Incrementor
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
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{
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
char*tag;};void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,unsigned int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU
info);int Cyc_Absyn_is_nontagged_union_type(void*);struct Cyc_RgnOrder_RgnPO;
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct
Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);struct
Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,
struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int Cyc_RgnOrder_is_region_resetable(
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
struct _dyneither_ptr ap);void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);void*Cyc_Tcutil_pointer_elt_type(
void*t);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,
void*);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*,void*t);int Cyc_Tcutil_is_noreturn(void*);void Cyc_Tcutil_check_no_qual(
unsigned int loc,void*t);struct _tuple11{unsigned int f1;int f2;};struct _tuple11 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int
tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{
int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{
int tag;int f1;void*f2;};typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 
1,Cyc_CfFlowInfo_AllIL  = 2};typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;
struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple12{struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple12 val;};struct
_union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessConst{int tag;unsigned int val;};struct
_union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};typedef union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_reln_op_t;
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*,void*);
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*);
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(unsigned int);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessEqConst(unsigned int);struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;union Cyc_CfFlowInfo_RelnOp rop;};typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;
typedef struct Cyc_List_List*Cyc_CfFlowInfo_relns_t;extern char Cyc_CfFlowInfo_IsZero[
7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[
8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*
f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{
char*tag;struct Cyc_List_List*f1;};extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct
Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*
val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{
struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_UnknownL();typedef void*Cyc_CfFlowInfo_absRval_t;typedef struct Cyc_Dict_Dict
Cyc_CfFlowInfo_flowdict_t;typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;
struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};typedef struct Cyc_CfFlowInfo_UnionRInfo
Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};
struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{
int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{
int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,
unsigned int loc);typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct Cyc_Dict_Dict
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int
disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple13{struct
Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int
tag;struct _tuple13 val;};union Cyc_CfFlowInfo_FlowInfo{struct
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
void*);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_List_List*,int no_init_bits_only,void*);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*);int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int
env_iteration,void*r,int*needs_unconsume);void*Cyc_CfFlowInfo_make_unique_unconsumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct
Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct
_RegionHandle*,struct Cyc_List_List*);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,
struct Cyc_List_List*);struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,
struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
struct _tuple14{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};struct _tuple14 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple14 pr1,
struct _tuple14 pr2);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct
Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union
Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo
f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};
struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};typedef void*Cyc_CfFlowInfo_killrgn_t;void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds);extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;typedef struct Cyc_PP_Out*
Cyc_PP_out_t;struct Cyc_PP_Doc;typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{
int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int
rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;
int print_using_stmts;int print_externC_stmts;int print_full_evars;int
print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);typedef struct Cyc_Dict_Dict
Cyc_NewControlFlow_dict_t;typedef unsigned int Cyc_NewControlFlow_seg_t;int Cyc_NewControlFlow_warn_lose_unique=
0;struct Cyc_NewControlFlow_CFStmtAnnot{struct Cyc_Absyn_Stmt*encloser;int visited;
};typedef struct Cyc_NewControlFlow_CFStmtAnnot Cyc_NewControlFlow_cf_stmt_annot_t;
static char Cyc_NewControlFlow_CFAnnot[8]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{
char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct
_tmp778;struct Cyc_NewControlFlow_CFStmtAnnot _tmp777;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*
_tmp776;enclosee->annot=(void*)((_tmp776=_cycalloc(sizeof(*_tmp776)),((_tmp776[0]=((
_tmp778.tag=Cyc_NewControlFlow_CFAnnot,((_tmp778.f1=((_tmp777.encloser=encloser,((
_tmp777.visited=0,_tmp777)))),_tmp778)))),_tmp776))));}struct Cyc_NewControlFlow_AnalEnv{
struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int
iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*
all_changed;int noreturn;struct Cyc_List_List*param_roots;struct Cyc_Hashtable_Table*
flow_table;};typedef struct Cyc_NewControlFlow_AnalEnv*Cyc_NewControlFlow_analenv_t;
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*);static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,
struct Cyc_Absyn_Decl*);struct _tuple15{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal
f2;};static struct _tuple15 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo,int expand_unique,struct Cyc_Absyn_Exp*);static struct
_tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int
copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple16{
union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};static struct
_tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,
struct Cyc_Absyn_Exp*);static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);static
struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*
s){void*_tmp3=s->annot;struct Cyc_NewControlFlow_CFStmtAnnot _tmp5;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp6;_LL1: {struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp4=(
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp3;if(_tmp4->tag != 
Cyc_NewControlFlow_CFAnnot)goto _LL3;else{_tmp5=_tmp4->f1;_tmp6=(struct Cyc_NewControlFlow_CFStmtAnnot*)&
_tmp4->f1;}}_LL2: return _tmp6;_LL3:;_LL4: {struct Cyc_Core_Impossible_exn_struct
_tmp77E;const char*_tmp77D;struct Cyc_Core_Impossible_exn_struct*_tmp77C;(int)
_throw((void*)((_tmp77C=_cycalloc(sizeof(*_tmp77C)),((_tmp77C[0]=((_tmp77E.tag=
Cyc_Core_Impossible,((_tmp77E.f1=((_tmp77D="ControlFlow -- wrong stmt annotation",
_tag_dyneither(_tmp77D,sizeof(char),37))),_tmp77E)))),_tmp77C)))));}_LL0:;}
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(
env->flow_table,s);if(sflow == 0){union Cyc_CfFlowInfo_FlowInfo*_tmp77F;union Cyc_CfFlowInfo_FlowInfo*
res=(_tmp77F=_region_malloc(env->r,sizeof(*_tmp77F)),((_tmp77F[0]=Cyc_CfFlowInfo_BottomFL(),
_tmp77F)));((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union
Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);return
res;}return*sflow;}struct _tuple17{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union
Cyc_CfFlowInfo_FlowInfo*f2;};static struct _tuple17 Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);*
_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC);_tmpB->visited=
env->iteration_num;{struct _tuple17 _tmp780;return(_tmp780.f1=_tmpB,((_tmp780.f2=
_tmpC,_tmp780)));};}static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo new_flow){if(env->in_try)env->tryflow=Cyc_CfFlowInfo_join_flow(
env->fenv,env->all_changed,new_flow,env->tryflow);}struct _tuple18{struct Cyc_NewControlFlow_AnalEnv*
f1;unsigned int f2;struct Cyc_Dict_Dict f3;};static void Cyc_NewControlFlow_check_unique_root(
struct _tuple18*ckenv,void*root,void*rval){struct _tuple18 _tmpF;struct Cyc_NewControlFlow_AnalEnv*
_tmp10;unsigned int _tmp11;struct Cyc_Dict_Dict _tmp12;struct _tuple18*_tmpE=ckenv;
_tmpF=*_tmpE;_tmp10=_tmpF.f1;_tmp11=_tmpF.f2;_tmp12=_tmpF.f3;{void*_tmp13=root;
struct Cyc_Absyn_Vardecl*_tmp15;_LL6: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*
_tmp14=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp13;if(_tmp14->tag
!= 0)goto _LL8;else{_tmp15=_tmp14->f1;}}_LL7: if(!((int(*)(struct Cyc_Dict_Dict d,
void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp12,root,& rval) && Cyc_Tcutil_is_noalias_pointer_or_aggr((
_tmp10->fenv)->r,_tmp15->type)){{void*_tmp16=rval;enum Cyc_CfFlowInfo_InitLevel
_tmp19;_LLB: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp17=(
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp16;if(_tmp17->tag != 
7)goto _LLD;}_LLC: goto _LLE;_LLD: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*
_tmp18=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp16;if(
_tmp18->tag != 3)goto _LLF;else{_tmp19=_tmp18->f1;if(_tmp19 != Cyc_CfFlowInfo_NoneIL)
goto _LLF;}}_LLE: goto _LL10;_LLF: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*
_tmp1A=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp16;if(_tmp1A->tag
!= 0)goto _LL11;}_LL10: goto _LLA;_LL11:;_LL12:{const char*_tmp784;void*_tmp783[1];
struct Cyc_String_pa_PrintArg_struct _tmp782;(_tmp782.tag=0,((_tmp782.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15->name)),((
_tmp783[0]=& _tmp782,Cyc_Tcutil_warn(_tmp11,((_tmp784="unique pointer(s) reachable from %s may become unreachable.",
_tag_dyneither(_tmp784,sizeof(char),60))),_tag_dyneither(_tmp783,sizeof(void*),1)))))));}
goto _LLA;_LLA:;}goto _LL5;}goto _LL5;_LL8:;_LL9: goto _LL5;_LL5:;};}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo
flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1E=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmp1F=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp20=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,
flow,*_tmp1F);if(Cyc_NewControlFlow_warn_lose_unique){struct _tuple16 _tmp785;
struct _tuple16 _tmp22=(_tmp785.f1=flow,((_tmp785.f2=_tmp20,_tmp785)));union Cyc_CfFlowInfo_FlowInfo
_tmp23;struct _tuple13 _tmp24;struct Cyc_Dict_Dict _tmp25;union Cyc_CfFlowInfo_FlowInfo
_tmp26;struct _tuple13 _tmp27;struct Cyc_Dict_Dict _tmp28;_LL14: _tmp23=_tmp22.f1;if((
_tmp23.ReachableFL).tag != 2)goto _LL16;_tmp24=(struct _tuple13)(_tmp23.ReachableFL).val;
_tmp25=_tmp24.f1;_tmp26=_tmp22.f2;if((_tmp26.ReachableFL).tag != 2)goto _LL16;
_tmp27=(struct _tuple13)(_tmp26.ReachableFL).val;_tmp28=_tmp27.f1;_LL15: {struct
_tuple18 _tmp786;struct _tuple18 _tmp29=(_tmp786.f1=env,((_tmp786.f2=s->loc,((
_tmp786.f3=_tmp28,_tmp786)))));((void(*)(void(*f)(struct _tuple18*,void*,void*),
struct _tuple18*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,&
_tmp29,_tmp25);goto _LL13;}_LL16:;_LL17: goto _LL13;_LL13:;}if(!Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp20,*_tmp1F)){*_tmp1F=_tmp20;if(_tmp1E->visited == env->iteration_num)env->iterate_again=
1;}}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,
unsigned int loc){union Cyc_CfFlowInfo_FlowInfo _tmp2B=inflow;int _tmp2C;struct
_tuple13 _tmp2D;struct Cyc_Dict_Dict _tmp2E;struct Cyc_List_List*_tmp2F;_LL19: if((
_tmp2B.BottomFL).tag != 1)goto _LL1B;_tmp2C=(int)(_tmp2B.BottomFL).val;_LL1A:
return Cyc_CfFlowInfo_BottomFL();_LL1B: if((_tmp2B.ReachableFL).tag != 2)goto _LL18;
_tmp2D=(struct _tuple13)(_tmp2B.ReachableFL).val;_tmp2E=_tmp2D.f1;_tmp2F=_tmp2D.f2;
_LL1C: for(0;vds != 0;vds=vds->tl){struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct
_tmp789;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp788;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*
_tmp30=(_tmp788=_region_malloc(fenv->r,sizeof(*_tmp788)),((_tmp788[0]=((_tmp789.tag=
0,((_tmp789.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp789)))),_tmp788)));_tmp2E=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp2E,(void*)_tmp30,Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)
vds->hd)->type,0,leafval));}return Cyc_CfFlowInfo_ReachableFL(_tmp2E,_tmp2F);
_LL18:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*
env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){union Cyc_CfFlowInfo_FlowInfo
_tmp33=inflow;int _tmp34;struct _tuple13 _tmp35;struct Cyc_Dict_Dict _tmp36;struct Cyc_List_List*
_tmp37;_LL1E: if((_tmp33.BottomFL).tag != 1)goto _LL20;_tmp34=(int)(_tmp33.BottomFL).val;
_LL1F: return Cyc_CfFlowInfo_BottomFL();_LL20: if((_tmp33.ReachableFL).tag != 2)goto
_LL1D;_tmp35=(struct _tuple13)(_tmp33.ReachableFL).val;_tmp36=_tmp35.f1;_tmp37=
_tmp35.f2;_LL21: if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp36,r)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp78C;void*_tmp78B;(_tmp78B=0,Cyc_Tcutil_terr(loc,((_tmp78C="expression may not be fully initialized",
_tag_dyneither(_tmp78C,sizeof(char),40))),_tag_dyneither(_tmp78B,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp3A=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp36,env->all_changed,
r);if(_tmp36.t == _tmp3A.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo _tmp3B=Cyc_CfFlowInfo_ReachableFL(
_tmp3A,_tmp37);Cyc_NewControlFlow_update_tryflow(env,_tmp3B);return _tmp3B;};};
_LL1D:;}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_NewControlFlow_check_nounique(
struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){struct
_tuple0 _tmp78D;struct _tuple0 _tmp3D=(_tmp78D.f1=Cyc_Tcutil_compress(t),((_tmp78D.f2=
r,_tmp78D)));void*_tmp3E;enum Cyc_CfFlowInfo_InitLevel _tmp40;void*_tmp41;void*
_tmp43;void*_tmp45;struct Cyc_Absyn_DatatypeFieldInfo _tmp47;union Cyc_Absyn_DatatypeFieldInfoU
_tmp48;struct _tuple2 _tmp49;struct Cyc_Absyn_Datatypefield*_tmp4A;void*_tmp4B;
struct _dyneither_ptr _tmp4D;void*_tmp4E;struct Cyc_List_List*_tmp50;void*_tmp51;
struct _dyneither_ptr _tmp53;void*_tmp54;struct Cyc_Absyn_AggrInfo _tmp56;union Cyc_Absyn_AggrInfoU
_tmp57;struct Cyc_List_List*_tmp58;void*_tmp59;struct _dyneither_ptr _tmp5B;void*
_tmp5C;enum Cyc_Absyn_AggrKind _tmp5E;struct Cyc_List_List*_tmp5F;void*_tmp60;
struct _dyneither_ptr _tmp62;void*_tmp63;_LL23: _tmp3E=_tmp3D.f2;{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*
_tmp3F=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3E;if(
_tmp3F->tag != 3)goto _LL25;else{_tmp40=_tmp3F->f1;if(_tmp40 != Cyc_CfFlowInfo_NoneIL)
goto _LL25;}};_LL24: return;_LL25: _tmp41=_tmp3D.f2;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*
_tmp42=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp41;if(_tmp42->tag
!= 0)goto _LL27;};_LL26: return;_LL27: _tmp43=_tmp3D.f2;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*
_tmp44=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp43;if(
_tmp44->tag != 7)goto _LL29;};_LL28: return;_LL29: _tmp45=_tmp3D.f1;{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmp46=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp45;if(_tmp46->tag
!= 4)goto _LL2B;else{_tmp47=_tmp46->f1;_tmp48=_tmp47.field_info;if((_tmp48.KnownDatatypefield).tag
!= 2)goto _LL2B;_tmp49=(struct _tuple2)(_tmp48.KnownDatatypefield).val;_tmp4A=
_tmp49.f2;}};_tmp4B=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp4C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4B;if(
_tmp4C->tag != 6)goto _LL2B;else{_tmp4D=_tmp4C->f2;}};_LL2A: if(_tmp4A->typs == 0)
return;_tmp50=_tmp4A->typs;_tmp53=_tmp4D;goto _LL2C;_LL2B: _tmp4E=_tmp3D.f1;{
struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)
_tmp4E;if(_tmp4F->tag != 10)goto _LL2D;else{_tmp50=_tmp4F->f1;}};_tmp51=_tmp3D.f2;{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp52=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)
_tmp51;if(_tmp52->tag != 6)goto _LL2D;else{_tmp53=_tmp52->f2;}};_LL2C: {
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp50);{int i=0;for(0;i < sz;++ i){Cyc_NewControlFlow_check_nounique(env,loc,(*((
struct _tuple19*)((struct Cyc_List_List*)_check_null(_tmp50))->hd)).f2,*((void**)
_check_dyneither_subscript(_tmp53,sizeof(void*),i)));}}return;}_LL2D: _tmp54=
_tmp3D.f1;{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp55=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp54;if(_tmp55->tag != 11)goto _LL2F;else{_tmp56=_tmp55->f1;_tmp57=_tmp56.aggr_info;
_tmp58=_tmp56.targs;}};_tmp59=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp5A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp59;if(
_tmp5A->tag != 6)goto _LL2F;else{_tmp5B=_tmp5A->f2;}};_LL2E: {struct Cyc_Absyn_Aggrdecl*
_tmp65=Cyc_Absyn_get_known_aggrdecl(_tmp57);if(_tmp65->impl == 0)return;{struct
Cyc_List_List*_tmp66=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp65->impl))->fields;
struct _RegionHandle _tmp67=_new_region("temp");struct _RegionHandle*temp=& _tmp67;
_push_region(temp);{struct Cyc_List_List*_tmp68=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(temp,temp,_tmp65->tvs,_tmp58);{int i=0;for(0;i < 
_get_dyneither_size(_tmp5B,sizeof(void*));(i ++,_tmp66=_tmp66->tl)){void*t=((
struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp66))->hd)->type;
if(_tmp68 != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp68,t);Cyc_NewControlFlow_check_nounique(
env,loc,t,((void**)_tmp5B.curr)[i]);}}_npop_handler(0);return;};_pop_region(temp);};}
_LL2F: _tmp5C=_tmp3D.f1;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp5D=(
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5C;if(_tmp5D->tag != 12)goto
_LL31;else{_tmp5E=_tmp5D->f1;_tmp5F=_tmp5D->f2;}};_tmp60=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp61=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp60;if(
_tmp61->tag != 6)goto _LL31;else{_tmp62=_tmp61->f2;}};_LL30:{int i=0;for(0;i < 
_get_dyneither_size(_tmp62,sizeof(void*));(i ++,_tmp5F=_tmp5F->tl)){Cyc_NewControlFlow_check_nounique(
env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp5F))->hd)->type,((
void**)_tmp62.curr)[i]);}}return;_LL31: _tmp63=_tmp3D.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp64=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63;if(_tmp64->tag != 5)
goto _LL33;};if(!Cyc_Tcutil_is_noalias_pointer(t))goto _LL33;_LL32:{const char*
_tmp790;void*_tmp78F;(_tmp78F=0,Cyc_Tcutil_warn(loc,((_tmp790="argument may still contain unique pointers",
_tag_dyneither(_tmp790,sizeof(char),43))),_tag_dyneither(_tmp78F,sizeof(void*),0)));}
return;_LL33:;_LL34: return;_LL22:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo
inflow,void*r){union Cyc_CfFlowInfo_FlowInfo _tmp6B=inflow;int _tmp6C;struct
_tuple13 _tmp6D;struct Cyc_Dict_Dict _tmp6E;struct Cyc_List_List*_tmp6F;_LL36: if((
_tmp6B.BottomFL).tag != 1)goto _LL38;_tmp6C=(int)(_tmp6B.BottomFL).val;_LL37:
return Cyc_CfFlowInfo_BottomFL();_LL38: if((_tmp6B.ReachableFL).tag != 2)goto _LL35;
_tmp6D=(struct _tuple13)(_tmp6B.ReachableFL).val;_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;
_LL39: if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp793;void*_tmp792;(
_tmp792=0,Cyc_Tcutil_terr(loc,((_tmp793="noliveunique attribute requires unique pointer",
_tag_dyneither(_tmp793,sizeof(char),47))),_tag_dyneither(_tmp792,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{void*_tmp72=Cyc_Tcutil_pointer_elt_type(t);{
void*_tmp73=r;struct Cyc_CfFlowInfo_Place*_tmp75;_LL3B: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp74=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp73;if(
_tmp74->tag != 5)goto _LL3D;else{_tmp75=_tmp74->f1;}}_LL3C: Cyc_NewControlFlow_check_nounique(
env,loc,_tmp72,Cyc_CfFlowInfo_lookup_place(_tmp6E,_tmp75));goto _LL3A;_LL3D:;
_LL3E: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,_tmp72)){const char*
_tmp796;void*_tmp795;(_tmp795=0,Cyc_Tcutil_warn(loc,((_tmp796="argument may contain live unique pointers",
_tag_dyneither(_tmp796,sizeof(char),42))),_tag_dyneither(_tmp795,sizeof(void*),0)));}
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);_LL3A:;}{struct Cyc_Dict_Dict
_tmp78=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp6E,env->all_changed,r);if(
_tmp6E.t == _tmp78.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo _tmp79=Cyc_CfFlowInfo_ReachableFL(
_tmp78,_tmp6F);Cyc_NewControlFlow_update_tryflow(env,_tmp79);return _tmp79;};};};
_LL35:;}struct _tuple20{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
static struct _tuple20 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*
rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,
struct Cyc_List_List*es,int first_is_copy,int others_are_copy){if(es == 0){struct
_tuple20 _tmp797;return(_tmp797.f1=inflow,((_tmp797.f2=0,_tmp797)));}if(es->tl == 
0){union Cyc_CfFlowInfo_FlowInfo _tmp7C;void*_tmp7D;struct _tuple14 _tmp7B=Cyc_NewControlFlow_anal_Rexp(
env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp7C=_tmp7B.f1;_tmp7D=
_tmp7B.f2;{struct Cyc_List_List*_tmp79A;struct _tuple20 _tmp799;return(_tmp799.f1=
_tmp7C,((_tmp799.f2=((_tmp79A=_region_malloc(rgn,sizeof(*_tmp79A)),((_tmp79A->hd=
_tmp7D,((_tmp79A->tl=0,_tmp79A)))))),_tmp799)));};}{struct Cyc_Dict_Dict*
outer_all_changed=env->all_changed;struct Cyc_Dict_Dict this_all_changed;union Cyc_CfFlowInfo_FlowInfo
old_inflow;union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_List_List*rvals;do{
this_all_changed=(env->fenv)->mt_place_set;{struct Cyc_Dict_Dict*_tmp79B;env->all_changed=((
_tmp79B=_region_malloc(env->r,sizeof(*_tmp79B)),((_tmp79B[0]=(env->fenv)->mt_place_set,
_tmp79B))));}{union Cyc_CfFlowInfo_FlowInfo _tmp82;void*_tmp83;struct _tuple14
_tmp81=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_tmp82=_tmp81.f1;_tmp83=_tmp81.f2;outflow=_tmp82;{struct Cyc_List_List*
_tmp79C;rvals=((_tmp79C=_region_malloc(rgn,sizeof(*_tmp79C)),((_tmp79C->hd=
_tmp83,((_tmp79C->tl=0,_tmp79C))))));}this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){{struct Cyc_Dict_Dict*
_tmp79D;env->all_changed=((_tmp79D=_region_malloc(env->r,sizeof(*_tmp79D)),((
_tmp79D[0]=(env->fenv)->mt_place_set,_tmp79D))));}{union Cyc_CfFlowInfo_FlowInfo
_tmp87;void*_tmp88;struct _tuple14 _tmp86=Cyc_NewControlFlow_anal_Rexp(env,
others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmp87=_tmp86.f1;_tmp88=
_tmp86.f2;{struct Cyc_List_List*_tmp79E;rvals=((_tmp79E=_region_malloc(rgn,
sizeof(*_tmp79E)),((_tmp79E->hd=_tmp88,((_tmp79E->tl=rvals,_tmp79E))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp87,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
old_inflow=inflow;inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,
inflow,outflow);};}while(!Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)env->all_changed=0;else{struct Cyc_Dict_Dict*_tmp79F;env->all_changed=((
_tmp79F=_region_malloc(env->r,sizeof(*_tmp79F)),((_tmp79F[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0),_tmp79F))));}Cyc_NewControlFlow_update_tryflow(
env,outflow);{struct _tuple20 _tmp7A0;return(_tmp7A0.f1=outflow,((_tmp7A0.f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp7A0)));};};}
static struct _tuple14 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){struct
_RegionHandle _tmp8C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp8C;
_push_region(rgn);{union Cyc_CfFlowInfo_FlowInfo _tmp8E;struct Cyc_List_List*_tmp8F;
struct _tuple20 _tmp8D=Cyc_NewControlFlow_anal_unordered_Rexps(rgn,env,inflow,es,0,
0);_tmp8E=_tmp8D.f1;_tmp8F=_tmp8D.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp90=_tmp8E;
struct _tuple13 _tmp91;struct Cyc_Dict_Dict _tmp92;int _tmp93;_LL40: if((_tmp90.ReachableFL).tag
!= 2)goto _LL42;_tmp91=(struct _tuple13)(_tmp90.ReachableFL).val;_tmp92=_tmp91.f1;
_LL41: for(0;_tmp8F != 0;(_tmp8F=_tmp8F->tl,es=((struct Cyc_List_List*)_check_null(
es))->tl)){if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp92,(void*)_tmp8F->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7A3;void*_tmp7A2;(_tmp7A2=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp7A3="expression may not be initialized",
_tag_dyneither(_tmp7A3,sizeof(char),34))),_tag_dyneither(_tmp7A2,sizeof(void*),0)));}}
goto _LL3F;_LL42: if((_tmp90.BottomFL).tag != 1)goto _LL3F;_tmp93=(int)(_tmp90.BottomFL).val;
_LL43: goto _LL3F;_LL3F:;}{struct _tuple14 _tmp7A4;struct _tuple14 _tmp97=(_tmp7A4.f1=
_tmp8E,((_tmp7A4.f2=(env->fenv)->unknown_all,_tmp7A4)));_npop_handler(0);return
_tmp97;};};_pop_region(rgn);}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){union Cyc_CfFlowInfo_FlowInfo
_tmp98=outflow;int _tmp99;struct _tuple13 _tmp9A;struct Cyc_Dict_Dict _tmp9B;struct
Cyc_List_List*_tmp9C;_LL45: if((_tmp98.BottomFL).tag != 1)goto _LL47;_tmp99=(int)(
_tmp98.BottomFL).val;_LL46: return outflow;_LL47: if((_tmp98.ReachableFL).tag != 2)
goto _LL44;_tmp9A=(struct _tuple13)(_tmp98.ReachableFL).val;_tmp9B=_tmp9A.f1;
_tmp9C=_tmp9A.f2;_LL48: {union Cyc_CfFlowInfo_AbsLVal _tmp9D=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,0,e)).f2;int _tmp9E;struct Cyc_CfFlowInfo_Place*_tmp9F;_LL4A: if((_tmp9D.UnknownL).tag
!= 2)goto _LL4C;_tmp9E=(int)(_tmp9D.UnknownL).val;_LL4B: return outflow;_LL4C: if((
_tmp9D.PlaceL).tag != 1)goto _LL49;_tmp9F=(struct Cyc_CfFlowInfo_Place*)(_tmp9D.PlaceL).val;
_LL4D: {void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
union Cyc_CfFlowInfo_FlowInfo _tmpA0=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp9B,env->all_changed,_tmp9F,nzval),_tmp9C);return _tmpA0;}
_LL49:;}_LL44:;}static struct _tuple16 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,
struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){union Cyc_CfFlowInfo_FlowInfo
_tmpA1=outflow;int _tmpA2;struct _tuple13 _tmpA3;struct Cyc_Dict_Dict _tmpA4;struct
Cyc_List_List*_tmpA5;_LL4F: if((_tmpA1.BottomFL).tag != 1)goto _LL51;_tmpA2=(int)(
_tmpA1.BottomFL).val;_LL50: {struct _tuple16 _tmp7A5;return(_tmp7A5.f1=outflow,((
_tmp7A5.f2=outflow,_tmp7A5)));}_LL51: if((_tmpA1.ReachableFL).tag != 2)goto _LL4E;
_tmpA3=(struct _tuple13)(_tmpA1.ReachableFL).val;_tmpA4=_tmpA3.f1;_tmpA5=_tmpA3.f2;
_LL52: {union Cyc_CfFlowInfo_AbsLVal _tmpA7=(Cyc_NewControlFlow_anal_Lexp(env,
inflow,0,e)).f2;int _tmpA8;struct Cyc_CfFlowInfo_Place*_tmpA9;_LL54: if((_tmpA7.UnknownL).tag
!= 2)goto _LL56;_tmpA8=(int)(_tmpA7.UnknownL).val;_LL55: {struct _tuple16 _tmp7A6;
return(_tmp7A6.f1=outflow,((_tmp7A6.f2=outflow,_tmp7A6)));}_LL56: if((_tmpA7.PlaceL).tag
!= 1)goto _LL53;_tmpA9=(struct Cyc_CfFlowInfo_Place*)(_tmpA7.PlaceL).val;_LL57: {
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
struct _tuple16 _tmp7A7;return(_tmp7A7.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmpA4,env->all_changed,_tmpA9,nzval),_tmpA5),((_tmp7A7.f2=Cyc_CfFlowInfo_ReachableFL(
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA4,env->all_changed,_tmpA9,(env->fenv)->zero),
_tmpA5),_tmp7A7)));}_LL53:;}_LL4E:;}static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct
Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct
Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};static struct
_tuple14 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,
union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*
e,void*r){void*_tmpAE=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_Absyn_PtrInfo
_tmpB0;void*_tmpB1;struct Cyc_Absyn_PtrAtts _tmpB2;union Cyc_Absyn_Constraint*
_tmpB3;union Cyc_Absyn_Constraint*_tmpB4;_LL59: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmpAF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAE;if(_tmpAF->tag != 5)
goto _LL5B;else{_tmpB0=_tmpAF->f1;_tmpB1=_tmpB0.elt_typ;_tmpB2=_tmpB0.ptr_atts;
_tmpB3=_tmpB2.bounds;_tmpB4=_tmpB2.zero_term;}}_LL5A: {union Cyc_CfFlowInfo_FlowInfo
_tmpB5=f;int _tmpB6;struct _tuple13 _tmpB7;struct Cyc_Dict_Dict _tmpB8;struct Cyc_List_List*
_tmpB9;_LL5E: if((_tmpB5.BottomFL).tag != 1)goto _LL60;_tmpB6=(int)(_tmpB5.BottomFL).val;
_LL5F: {struct _tuple14 _tmp7A8;return(_tmp7A8.f1=f,((_tmp7A8.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmpB1,0,(env->fenv)->unknown_all),_tmp7A8)));}_LL60: if((_tmpB5.ReachableFL).tag
!= 2)goto _LL5D;_tmpB7=(struct _tuple13)(_tmpB5.ReachableFL).val;_tmpB8=_tmpB7.f1;
_tmpB9=_tmpB7.f2;_LL61: if(Cyc_Tcutil_is_bound_one(_tmpB3)){void*_tmpBB=r;struct
Cyc_CfFlowInfo_Place*_tmpBF;enum Cyc_CfFlowInfo_InitLevel _tmpC2;_LL63: {struct
Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmpBC=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)
_tmpBB;if(_tmpBC->tag != 1)goto _LL65;}_LL64: goto _LL66;_LL65: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*
_tmpBD=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmpBB;if(
_tmpBD->tag != 2)goto _LL67;}_LL66:{void*_tmpC3=e->annot;struct Cyc_List_List*
_tmpC5;_LL70: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpC4=(
struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpC3;if(_tmpC4->tag != 
Cyc_CfFlowInfo_NotZero)goto _LL72;else{_tmpC5=_tmpC4->f1;}}_LL71: if(!Cyc_CfFlowInfo_same_relns(
_tmpB9,_tmpC5))goto _LL73;goto _LL6F;_LL72:;_LL73:{void*_tmpC6=e->r;_LL75: {struct
Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC7=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)
_tmpC6;if(_tmpC7->tag != 22)goto _LL77;}_LL76:{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct
_tmp7AB;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp7AA;e->annot=(
void*)((_tmp7AA=_cycalloc(sizeof(*_tmp7AA)),((_tmp7AA[0]=((_tmp7AB.tag=Cyc_CfFlowInfo_NotZero,((
_tmp7AB.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpB9),_tmp7AB)))),
_tmp7AA))));}goto _LL74;_LL77:;_LL78: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL74;_LL74:;}goto _LL6F;_LL6F:;}goto _LL62;_LL67: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmpBE=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpBB;if(
_tmpBE->tag != 5)goto _LL69;else{_tmpBF=_tmpBE->f1;}}_LL68:{void*_tmpCA=e->annot;
struct Cyc_List_List*_tmpCC;_LL7A: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*
_tmpCB=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpCA;if(_tmpCB->tag
!= Cyc_CfFlowInfo_NotZero)goto _LL7C;else{_tmpCC=_tmpCB->f1;}}_LL7B: if(!Cyc_CfFlowInfo_same_relns(
_tmpB9,_tmpCC))goto _LL7D;goto _LL79;_LL7C:;_LL7D:{void*_tmpCD=e->r;_LL7F: {struct
Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpCE=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)
_tmpCD;if(_tmpCE->tag != 22)goto _LL81;}_LL80:{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct
_tmp7AE;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp7AD;e->annot=(
void*)((_tmp7AD=_cycalloc(sizeof(*_tmp7AD)),((_tmp7AD[0]=((_tmp7AE.tag=Cyc_CfFlowInfo_NotZero,((
_tmp7AE.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpB9),_tmp7AE)))),
_tmp7AD))));}goto _LL7E;_LL81:;_LL82: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL7E;_LL7E:;}goto _LL79;_LL79:;}{struct _tuple14 _tmp7AF;return(_tmp7AF.f1=f,((
_tmp7AF.f2=Cyc_CfFlowInfo_lookup_place(_tmpB8,_tmpBF),_tmp7AF)));};_LL69: {
struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpC0=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)
_tmpBB;if(_tmpC0->tag != 0)goto _LL6B;}_LL6A: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple14 _tmp7B0;return(_tmp7B0.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7B0.f2=
Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB1,0,(env->fenv)->unknown_all),
_tmp7B0)));};_LL6B: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*
_tmpC1=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpBB;if(
_tmpC1->tag != 3)goto _LL6D;else{_tmpC2=_tmpC1->f1;}}_LL6C: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmpC2);goto _LL6E;_LL6D:;_LL6E:{void*_tmpD3=e->r;_LL84: {struct
Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpD4=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)
_tmpD3;if(_tmpD4->tag != 22)goto _LL86;}_LL85:{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct
_tmp7B3;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp7B2;e->annot=(
void*)((_tmp7B2=_cycalloc(sizeof(*_tmp7B2)),((_tmp7B2[0]=((_tmp7B3.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp7B3.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpB9),_tmp7B3)))),
_tmp7B2))));}goto _LL83;_LL86:;_LL87: e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL83;_LL83:;}goto _LL62;_LL62:;}else{void*_tmpD7=e->annot;struct Cyc_List_List*
_tmpD9;_LL89: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpD8=(
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpD7;if(_tmpD8->tag != 
Cyc_CfFlowInfo_UnknownZ)goto _LL8B;else{_tmpD9=_tmpD8->f1;}}_LL8A: if(!Cyc_CfFlowInfo_same_relns(
_tmpB9,_tmpD9))goto _LL8C;goto _LL88;_LL8B:;_LL8C:{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct
_tmp7B6;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp7B5;e->annot=(
void*)((_tmp7B5=_cycalloc(sizeof(*_tmp7B5)),((_tmp7B5[0]=((_tmp7B6.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp7B6.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpB9),_tmp7B6)))),
_tmp7B5))));}goto _LL88;_LL88:;}switch(Cyc_CfFlowInfo_initlevel(env->fenv,_tmpB8,
r)){case Cyc_CfFlowInfo_NoneIL: _LL8D:{void*_tmpDC=r;_LL90: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*
_tmpDD=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpDC;if(
_tmpDD->tag != 7)goto _LL92;}_LL91:{const char*_tmp7B9;void*_tmp7B8;(_tmp7B8=0,Cyc_Tcutil_terr(
e->loc,((_tmp7B9="attempt to use a unique pointer that has already been copied",
_tag_dyneither(_tmp7B9,sizeof(char),61))),_tag_dyneither(_tmp7B8,sizeof(void*),0)));}
goto _LL8F;_LL92:;_LL93: {const char*_tmp7BC;void*_tmp7BB;(_tmp7BB=0,Cyc_Tcutil_terr(
e->loc,((_tmp7BC="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp7BC,sizeof(char),46))),_tag_dyneither(_tmp7BB,sizeof(void*),0)));}_LL8F:;}
goto _LL8E;case Cyc_CfFlowInfo_AllIL: _LL8E: {struct _tuple14 _tmp7BD;return(_tmp7BD.f1=
f,((_tmp7BD.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB1,0,(env->fenv)->unknown_all),
_tmp7BD)));}case Cyc_CfFlowInfo_ThisIL: _LL94: {struct _tuple14 _tmp7BE;return(
_tmp7BE.f1=f,((_tmp7BE.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB1,0,(env->fenv)->unknown_none),
_tmp7BE)));}}_LL5D:;}_LL5B:;_LL5C: {struct Cyc_Core_Impossible_exn_struct _tmp7C4;
const char*_tmp7C3;struct Cyc_Core_Impossible_exn_struct*_tmp7C2;(int)_throw((void*)((
_tmp7C2=_cycalloc(sizeof(*_tmp7C2)),((_tmp7C2[0]=((_tmp7C4.tag=Cyc_Core_Impossible,((
_tmp7C4.f1=((_tmp7C3="right deref of non-pointer-type",_tag_dyneither(_tmp7C3,
sizeof(char),32))),_tmp7C4)))),_tmp7C2)))));}_LL58:;}static struct Cyc_List_List*
Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmpE7=e1->r;void*
_tmpE9;struct Cyc_Absyn_Vardecl*_tmpEB;void*_tmpED;struct Cyc_Absyn_Vardecl*_tmpEF;
void*_tmpF1;struct Cyc_Absyn_Vardecl*_tmpF3;void*_tmpF5;struct Cyc_Absyn_Vardecl*
_tmpF7;_LL97: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpE8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmpE7;if(_tmpE8->tag != 1)goto _LL99;else{_tmpE9=(void*)_tmpE8->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*
_tmpEA=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpE9;if(_tmpEA->tag != 5)
goto _LL99;else{_tmpEB=_tmpEA->f1;}};}}_LL98: _tmpEF=_tmpEB;goto _LL9A;_LL99: {
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpEC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmpE7;if(_tmpEC->tag != 1)goto _LL9B;else{_tmpED=(void*)_tmpEC->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmpEE=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpED;if(_tmpEE->tag != 4)
goto _LL9B;else{_tmpEF=_tmpEE->f1;}};}}_LL9A: _tmpF3=_tmpEF;goto _LL9C;_LL9B: {
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpF0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmpE7;if(_tmpF0->tag != 1)goto _LL9D;else{_tmpF1=(void*)_tmpF0->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*
_tmpF2=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpF1;if(_tmpF2->tag != 3)
goto _LL9D;else{_tmpF3=_tmpF2->f1;}};}}_LL9C: _tmpF7=_tmpF3;goto _LL9E;_LL9D: {
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpF4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmpE7;if(_tmpF4->tag != 1)goto _LL9F;else{_tmpF5=(void*)_tmpF4->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmpF6=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpF5;if(_tmpF6->tag != 1)
goto _LL9F;else{_tmpF7=_tmpF6->f1;}};}}_LL9E: if(!_tmpF7->escapes){void*_tmpF8=e2->r;
void*_tmpFA;struct Cyc_Absyn_Vardecl*_tmpFC;void*_tmpFE;struct Cyc_Absyn_Vardecl*
_tmp100;void*_tmp102;struct Cyc_Absyn_Vardecl*_tmp104;void*_tmp106;struct Cyc_Absyn_Vardecl*
_tmp108;_LLA2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpF9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmpF8;if(_tmpF9->tag != 1)goto _LLA4;else{_tmpFA=(void*)_tmpF9->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*
_tmpFB=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpFA;if(_tmpFB->tag != 5)
goto _LLA4;else{_tmpFC=_tmpFB->f1;}};}}_LLA3: _tmp100=_tmpFC;goto _LLA5;_LLA4: {
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpFD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmpF8;if(_tmpFD->tag != 1)goto _LLA6;else{_tmpFE=(void*)_tmpFD->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmpFF=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpFE;if(_tmpFF->tag != 4)
goto _LLA6;else{_tmp100=_tmpFF->f1;}};}}_LLA5: _tmp104=_tmp100;goto _LLA7;_LLA6: {
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp101=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmpF8;if(_tmp101->tag != 1)goto _LLA8;else{_tmp102=(void*)_tmp101->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*
_tmp103=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp102;if(_tmp103->tag != 
3)goto _LLA8;else{_tmp104=_tmp103->f1;}};}}_LLA7: _tmp108=_tmp104;goto _LLA9;_LLA8: {
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp105=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmpF8;if(_tmp105->tag != 1)goto _LLAA;else{_tmp106=(void*)_tmp105->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp107=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp106;if(_tmp107->tag
!= 1)goto _LLAA;else{_tmp108=_tmp107->f1;}};}}_LLA9: if(!_tmp108->escapes){int
found=0;{struct Cyc_List_List*_tmp109=relns;for(0;_tmp109 != 0;_tmp109=_tmp109->tl){
struct Cyc_CfFlowInfo_Reln*_tmp10A=(struct Cyc_CfFlowInfo_Reln*)_tmp109->hd;if(
_tmp10A->vd == _tmp108){union Cyc_CfFlowInfo_RelnOp _tmp10B=_tmp10A->rop;struct Cyc_Absyn_Vardecl*
_tmp10C;_LLAD: if((_tmp10B.LessNumelts).tag != 3)goto _LLAF;_tmp10C=(struct Cyc_Absyn_Vardecl*)(
_tmp10B.LessNumelts).val;if(!(_tmp10C == _tmpF7))goto _LLAF;_LLAE: return relns;
_LLAF:;_LLB0: continue;_LLAC:;}}}if(!found){struct Cyc_CfFlowInfo_Reln*_tmp7C7;
struct Cyc_List_List*_tmp7C6;return(_tmp7C6=_region_malloc(rgn,sizeof(*_tmp7C6)),((
_tmp7C6->hd=((_tmp7C7=_region_malloc(rgn,sizeof(*_tmp7C7)),((_tmp7C7->vd=_tmp108,((
_tmp7C7->rop=Cyc_CfFlowInfo_LessNumelts(_tmpF7),_tmp7C7)))))),((_tmp7C6->tl=
relns,_tmp7C6)))));}}return relns;_LLAA:;_LLAB: return relns;_LLA1:;}else{return
relns;}_LL9F:;_LLA0: return relns;_LL96:;}static struct _tuple14 Cyc_NewControlFlow_do_assign(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,unsigned int loc){union Cyc_CfFlowInfo_FlowInfo _tmp10F=outflow;int
_tmp110;struct _tuple13 _tmp111;struct Cyc_Dict_Dict _tmp112;struct Cyc_List_List*
_tmp113;_LLB2: if((_tmp10F.BottomFL).tag != 1)goto _LLB4;_tmp110=(int)(_tmp10F.BottomFL).val;
_LLB3: {struct _tuple14 _tmp7C8;return(_tmp7C8.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp7C8.f2=rval,_tmp7C8)));}_LLB4: if((_tmp10F.ReachableFL).tag != 2)goto _LLB1;
_tmp111=(struct _tuple13)(_tmp10F.ReachableFL).val;_tmp112=_tmp111.f1;_tmp113=
_tmp111.f2;_LLB5: {union Cyc_CfFlowInfo_AbsLVal _tmp115=lval;struct Cyc_CfFlowInfo_Place*
_tmp116;int _tmp117;_LLB7: if((_tmp115.PlaceL).tag != 1)goto _LLB9;_tmp116=(struct
Cyc_CfFlowInfo_Place*)(_tmp115.PlaceL).val;_LLB8: {struct Cyc_Dict_Dict _tmp118=
Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp112,env->all_changed,_tmp116,rval);
_tmp113=Cyc_CfFlowInfo_reln_assign_exp(fenv->r,_tmp113,lexp,rexp);outflow=Cyc_CfFlowInfo_ReachableFL(
_tmp118,_tmp113);if(Cyc_NewControlFlow_warn_lose_unique  && Cyc_Tcutil_is_noalias_pointer_or_aggr((
env->fenv)->r,(void*)_check_null(lexp->topt))){void*_tmp119=Cyc_CfFlowInfo_lookup_place(
_tmp112,_tmp116);enum Cyc_CfFlowInfo_InitLevel _tmp11B;_LLBC: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*
_tmp11A=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp119;if(
_tmp11A->tag != 3)goto _LLBE;else{_tmp11B=_tmp11A->f1;if(_tmp11B != Cyc_CfFlowInfo_NoneIL)
goto _LLBE;}}_LLBD: goto _LLBF;_LLBE: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*
_tmp11C=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp119;if(
_tmp11C->tag != 7)goto _LLC0;}_LLBF: goto _LLC1;_LLC0: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*
_tmp11D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp119;if(_tmp11D->tag
!= 0)goto _LLC2;}_LLC1: goto _LLBB;_LLC2:;_LLC3:{const char*_tmp7CB;void*_tmp7CA;(
_tmp7CA=0,Cyc_Tcutil_warn(lexp->loc,((_tmp7CB="assignment may overwrite unique pointer(s)",
_tag_dyneither(_tmp7CB,sizeof(char),43))),_tag_dyneither(_tmp7CA,sizeof(void*),0)));}
goto _LLBB;_LLBB:;}Cyc_NewControlFlow_update_tryflow(env,outflow);{struct _tuple14
_tmp7CC;return(_tmp7CC.f1=outflow,((_tmp7CC.f2=rval,_tmp7CC)));};}_LLB9: if((
_tmp115.UnknownL).tag != 2)goto _LLB6;_tmp117=(int)(_tmp115.UnknownL).val;_LLBA: {
struct _tuple14 _tmp7CD;return(_tmp7CD.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,
outflow,rval),((_tmp7CD.f2=rval,_tmp7CD)));}_LLB6:;}_LLB1:;}static int Cyc_NewControlFlow_is_local_var_rooted_path(
struct Cyc_Absyn_Exp*e){void*_tmp122=e->r;void*_tmp124;void*_tmp127;void*_tmp12A;
struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*
_tmp131;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp135;_LLC5: {struct
Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp123=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp122;if(_tmp123->tag != 1)goto _LLC7;else{_tmp124=(void*)_tmp123->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*
_tmp125=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp124;if(_tmp125->tag != 
3)goto _LLC7;};}}_LLC6: goto _LLC8;_LLC7: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp126=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp122;if(_tmp126->tag != 1)
goto _LLC9;else{_tmp127=(void*)_tmp126->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmp128=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp127;if(_tmp128->tag != 
4)goto _LLC9;};}}_LLC8: goto _LLCA;_LLC9: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp129=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp122;if(_tmp129->tag != 1)
goto _LLCB;else{_tmp12A=(void*)_tmp129->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*
_tmp12B=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp12A;if(_tmp12B->tag != 5)
goto _LLCB;};}}_LLCA: return 1;_LLCB: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*
_tmp12C=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp122;if(_tmp12C->tag != 
19)goto _LLCD;else{_tmp12D=_tmp12C->f1;}}_LLCC: _tmp12F=_tmp12D;goto _LLCE;_LLCD: {
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp12E=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)
_tmp122;if(_tmp12E->tag != 20)goto _LLCF;else{_tmp12F=_tmp12E->f1;}}_LLCE: _tmp131=
_tmp12F;goto _LLD0;_LLCF: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*
_tmp130=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp122;if(_tmp130->tag
!= 21)goto _LLD1;else{_tmp131=_tmp130->f1;}}_LLD0: return Cyc_NewControlFlow_is_local_var_rooted_path(
_tmp131);_LLD1: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp132=(
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp122;if(_tmp132->tag != 22)
goto _LLD3;else{_tmp133=_tmp132->f1;}}_LLD2: {void*_tmp136=Cyc_Tcutil_compress((
void*)_check_null(_tmp133->topt));_LLD8: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmp137=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp136;if(_tmp137->tag != 
10)goto _LLDA;}_LLD9: return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp133);
_LLDA:;_LLDB: return 0;_LLD7:;}_LLD3: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*
_tmp134=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp122;if(_tmp134->tag != 
13)goto _LLD5;else{_tmp135=_tmp134->f2;}}_LLD4: return Cyc_NewControlFlow_is_local_var_rooted_path(
_tmp135);_LLD5:;_LLD6: return 0;_LLC4:;}static struct _tuple14 Cyc_NewControlFlow_anal_stmt_exp(
struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_Absyn_Stmt*s){struct Cyc_CfFlowInfo_FlowEnv*_tmp138=env->fenv;
while(1){union Cyc_CfFlowInfo_FlowInfo*_tmp13A;struct _tuple17 _tmp139=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);_tmp13A=_tmp139.f2;inflow=*_tmp13A;{void*_tmp13B=s->r;struct Cyc_Absyn_Stmt*
_tmp13D;struct Cyc_Absyn_Stmt*_tmp13E;struct Cyc_Absyn_Decl*_tmp140;struct Cyc_Absyn_Decl
_tmp141;void*_tmp142;struct Cyc_Absyn_Exp*_tmp144;struct Cyc_Absyn_Tvar*_tmp145;
struct Cyc_Absyn_Vardecl*_tmp146;struct Cyc_Absyn_Stmt*_tmp147;struct Cyc_Absyn_Decl*
_tmp149;struct Cyc_Absyn_Stmt*_tmp14A;struct Cyc_Absyn_Exp*_tmp14C;_LLDD: {struct
Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp13C=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)
_tmp13B;if(_tmp13C->tag != 2)goto _LLDF;else{_tmp13D=_tmp13C->f1;_tmp13E=_tmp13C->f2;}}
_LLDE: inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp13D);s=_tmp13E;goto
_LLDC;_LLDF: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp13F=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)
_tmp13B;if(_tmp13F->tag != 12)goto _LLE1;else{_tmp140=_tmp13F->f1;_tmp141=*_tmp140;
_tmp142=_tmp141.r;{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp143=(struct
Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp142;if(_tmp143->tag != 5)goto _LLE1;
else{_tmp144=_tmp143->f1;_tmp145=_tmp143->f2;_tmp146=_tmp143->f3;}};_tmp147=
_tmp13F->f2;}}_LLE0: {union Cyc_CfFlowInfo_FlowInfo _tmp14E;union Cyc_CfFlowInfo_AbsLVal
_tmp14F;struct _tuple15 _tmp14D=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp144);
_tmp14E=_tmp14D.f1;_tmp14F=_tmp14D.f2;{struct _tuple15 _tmp7CE;struct _tuple15
_tmp151=(_tmp7CE.f1=_tmp14E,((_tmp7CE.f2=_tmp14F,_tmp7CE)));union Cyc_CfFlowInfo_FlowInfo
_tmp152;struct _tuple13 _tmp153;struct Cyc_Dict_Dict _tmp154;struct Cyc_List_List*
_tmp155;union Cyc_CfFlowInfo_AbsLVal _tmp156;struct Cyc_CfFlowInfo_Place*_tmp157;
union Cyc_CfFlowInfo_FlowInfo _tmp158;int _tmp159;_LLE8: _tmp152=_tmp151.f1;if((
_tmp152.ReachableFL).tag != 2)goto _LLEA;_tmp153=(struct _tuple13)(_tmp152.ReachableFL).val;
_tmp154=_tmp153.f1;_tmp155=_tmp153.f2;_tmp156=_tmp151.f2;if((_tmp156.PlaceL).tag
!= 1)goto _LLEA;_tmp157=(struct Cyc_CfFlowInfo_Place*)(_tmp156.PlaceL).val;_LLE9: {
void*_tmp15A=Cyc_CfFlowInfo_lookup_place(_tmp154,_tmp157);void*t=(void*)
_check_null(_tmp144->topt);{void*_tmp15B=_tmp144->r;struct Cyc_Absyn_Exp*_tmp15D;
_LLEF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp15C=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)
_tmp15B;if(_tmp15C->tag != 13)goto _LLF1;else{_tmp15D=_tmp15C->f2;}}_LLF0: t=(void*)
_check_null(_tmp15D->topt);goto _LLEE;_LLF1:;_LLF2: goto _LLEE;_LLEE:;}{void*
_tmp15E=Cyc_CfFlowInfo_make_unique_consumed(_tmp138,t,_tmp144,env->iteration_num,
_tmp15A);_tmp154=Cyc_CfFlowInfo_assign_place(_tmp138,_tmp144->loc,_tmp154,env->all_changed,
_tmp157,_tmp15E);_tmp14E=Cyc_CfFlowInfo_ReachableFL(_tmp154,_tmp155);{void*leaf;
switch(Cyc_CfFlowInfo_initlevel(_tmp138,_tmp154,_tmp15A)){case Cyc_CfFlowInfo_AllIL:
_LLF3: leaf=_tmp138->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LLF4: leaf=
_tmp138->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LLF5: leaf=_tmp138->unknown_this;
break;}{void*_tmp15F=Cyc_CfFlowInfo_typ_to_absrval(_tmp138,t,0,leaf);_tmp14E=Cyc_NewControlFlow_use_Rval(
env,_tmp144->loc,_tmp14E,_tmp15A);{struct Cyc_List_List _tmp7CF;struct Cyc_List_List
_tmp160=(_tmp7CF.hd=_tmp146,((_tmp7CF.tl=0,_tmp7CF)));_tmp14E=Cyc_NewControlFlow_add_vars(
_tmp138,_tmp14E,(struct Cyc_List_List*)& _tmp160,_tmp138->unknown_all,s->loc);{
union Cyc_CfFlowInfo_FlowInfo _tmp162;void*_tmp163;struct _tuple14 _tmp161=Cyc_NewControlFlow_anal_stmt_exp(
env,copy_ctxt,_tmp14E,_tmp147);_tmp162=_tmp161.f1;_tmp163=_tmp161.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp164=_tmp162;struct _tuple13 _tmp165;struct Cyc_Dict_Dict _tmp166;struct Cyc_List_List*
_tmp167;_LLF8: if((_tmp164.ReachableFL).tag != 2)goto _LLFA;_tmp165=(struct _tuple13)(
_tmp164.ReachableFL).val;_tmp166=_tmp165.f1;_tmp167=_tmp165.f2;_LLF9: _tmp166=Cyc_CfFlowInfo_assign_place(
_tmp138,s->loc,_tmp166,env->all_changed,_tmp157,_tmp15F);{union Cyc_CfFlowInfo_FlowInfo
_tmp168=Cyc_CfFlowInfo_ReachableFL(_tmp166,_tmp167);struct _tuple14 _tmp7D0;return(
_tmp7D0.f1=_tmp168,((_tmp7D0.f2=_tmp163,_tmp7D0)));};_LLFA:;_LLFB: {struct
_tuple14 _tmp7D1;return(_tmp7D1.f1=_tmp162,((_tmp7D1.f2=_tmp163,_tmp7D1)));}_LLF7:;};};};};};};}
_LLEA: _tmp158=_tmp151.f1;if((_tmp158.BottomFL).tag != 1)goto _LLEC;_tmp159=(int)(
_tmp158.BottomFL).val;_LLEB: {struct _tuple14 _tmp7D2;return(_tmp7D2.f1=_tmp14E,((
_tmp7D2.f2=_tmp138->unknown_all,_tmp7D2)));}_LLEC:;_LLED:{const char*_tmp7D5;void*
_tmp7D4;(_tmp7D4=0,Cyc_Tcutil_terr(_tmp144->loc,((_tmp7D5="bad alias expression--no unique path found",
_tag_dyneither(_tmp7D5,sizeof(char),43))),_tag_dyneither(_tmp7D4,sizeof(void*),0)));}{
struct _tuple14 _tmp7D6;return(_tmp7D6.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7D6.f2=
_tmp138->unknown_all,_tmp7D6)));};_LLE7:;};}_LLE1: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*
_tmp148=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp13B;if(_tmp148->tag != 
12)goto _LLE3;else{_tmp149=_tmp148->f1;_tmp14A=_tmp148->f2;}}_LLE2: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp149);s=_tmp14A;goto _LLDC;_LLE3: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*
_tmp14B=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp13B;if(_tmp14B->tag != 
1)goto _LLE5;else{_tmp14C=_tmp14B->f1;}}_LLE4: return Cyc_NewControlFlow_anal_Rexp(
env,copy_ctxt,inflow,_tmp14C);_LLE5:;_LLE6: {struct Cyc_Core_Impossible_exn_struct
_tmp7DC;const char*_tmp7DB;struct Cyc_Core_Impossible_exn_struct*_tmp7DA;(int)
_throw((void*)((_tmp7DA=_cycalloc(sizeof(*_tmp7DA)),((_tmp7DA[0]=((_tmp7DC.tag=
Cyc_Core_Impossible,((_tmp7DC.f1=((_tmp7DB="analyze_Rexp: ill-formed StmtExp",
_tag_dyneither(_tmp7DB,sizeof(char),33))),_tmp7DC)))),_tmp7DA)))));}_LLDC:;};}}
struct _tuple21{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};
struct _tuple22{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct
_tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int
copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void
_tmp879(unsigned int*_tmp878,unsigned int*_tmp877,void***_tmp875,struct Cyc_List_List**
_tmp360){for(*_tmp878=0;*_tmp878 < *_tmp877;(*_tmp878)++){void*_tmp873;(*_tmp875)[*
_tmp878]=((_tmp873=(void*)((struct Cyc_List_List*)_check_null(*_tmp360))->hd,((*
_tmp360=(*_tmp360)->tl,_tmp873))));}}static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp173=env->fenv;
struct Cyc_Dict_Dict d;struct Cyc_List_List*relns;{union Cyc_CfFlowInfo_FlowInfo
_tmp174=inflow;int _tmp175;struct _tuple13 _tmp176;struct Cyc_Dict_Dict _tmp177;
struct Cyc_List_List*_tmp178;_LLFD: if((_tmp174.BottomFL).tag != 1)goto _LLFF;
_tmp175=(int)(_tmp174.BottomFL).val;_LLFE: {struct _tuple14 _tmp7DD;return(_tmp7DD.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp7DD.f2=_tmp173->unknown_all,_tmp7DD)));}_LLFF:
if((_tmp174.ReachableFL).tag != 2)goto _LLFC;_tmp176=(struct _tuple13)(_tmp174.ReachableFL).val;
_tmp177=_tmp176.f1;_tmp178=_tmp176.f2;_LL100: d=_tmp177;relns=_tmp178;_LLFC:;}if((
copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)_check_null(e->topt))){union Cyc_CfFlowInfo_FlowInfo _tmp17B;union
Cyc_CfFlowInfo_AbsLVal _tmp17C;struct _tuple15 _tmp17A=Cyc_NewControlFlow_anal_Lexp(
env,inflow,1,e);_tmp17B=_tmp17A.f1;_tmp17C=_tmp17A.f2;{struct _tuple15 _tmp7DE;
struct _tuple15 _tmp17E=(_tmp7DE.f1=_tmp17B,((_tmp7DE.f2=_tmp17C,_tmp7DE)));union
Cyc_CfFlowInfo_FlowInfo _tmp17F;struct _tuple13 _tmp180;struct Cyc_Dict_Dict _tmp181;
struct Cyc_List_List*_tmp182;union Cyc_CfFlowInfo_AbsLVal _tmp183;struct Cyc_CfFlowInfo_Place*
_tmp184;_LL102: _tmp17F=_tmp17E.f1;if((_tmp17F.ReachableFL).tag != 2)goto _LL104;
_tmp180=(struct _tuple13)(_tmp17F.ReachableFL).val;_tmp181=_tmp180.f1;_tmp182=
_tmp180.f2;_tmp183=_tmp17E.f2;if((_tmp183.PlaceL).tag != 1)goto _LL104;_tmp184=(
struct Cyc_CfFlowInfo_Place*)(_tmp183.PlaceL).val;_LL103: {void*_tmp185=Cyc_CfFlowInfo_lookup_place(
_tmp181,_tmp184);int needs_unconsume=0;if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,
_tmp185,& needs_unconsume)){{const char*_tmp7E1;void*_tmp7E0;(_tmp7E0=0,Cyc_Tcutil_terr(
e->loc,((_tmp7E1="expression attempts to copy unique value that may be copied elsewhere.",
_tag_dyneither(_tmp7E1,sizeof(char),71))),_tag_dyneither(_tmp7E0,sizeof(void*),0)));}{
struct _tuple14 _tmp7E2;return(_tmp7E2.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7E2.f2=
_tmp173->unknown_all,_tmp7E2)));};}else{if(needs_unconsume){struct _tuple14
_tmp7E3;return(_tmp7E3.f1=_tmp17B,((_tmp7E3.f2=Cyc_CfFlowInfo_make_unique_unconsumed(
_tmp173,_tmp185),_tmp7E3)));}else{void*_tmp18A=Cyc_CfFlowInfo_make_unique_consumed(
_tmp173,(void*)_check_null(e->topt),e,env->iteration_num,_tmp185);struct Cyc_Dict_Dict
_tmp18B=Cyc_CfFlowInfo_assign_place(_tmp173,e->loc,_tmp181,env->all_changed,
_tmp184,_tmp18A);struct _tuple14 _tmp7E4;return(_tmp7E4.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp18B,_tmp182),((_tmp7E4.f2=_tmp185,_tmp7E4)));}}}_LL104:;_LL105: goto _LL101;
_LL101:;};}{void*_tmp18D=e->r;struct Cyc_Absyn_Exp*_tmp18F;enum Cyc_Absyn_Coercion
_tmp190;struct Cyc_Absyn_Exp*_tmp192;struct Cyc_Absyn_Exp*_tmp194;struct Cyc_Absyn_Exp*
_tmp196;union Cyc_Absyn_Cnst _tmp198;int _tmp199;union Cyc_Absyn_Cnst _tmp19B;struct
_tuple6 _tmp19C;int _tmp19D;union Cyc_Absyn_Cnst _tmp19F;struct _tuple6 _tmp1A0;void*
_tmp1A2;struct Cyc_List_List*_tmp1A5;void*_tmp1AD;void*_tmp1B0;struct Cyc_Absyn_Vardecl*
_tmp1B2;void*_tmp1B4;struct Cyc_Absyn_Vardecl*_tmp1B6;void*_tmp1B8;struct Cyc_Absyn_Vardecl*
_tmp1BA;enum Cyc_Absyn_Primop _tmp1BC;struct Cyc_List_List*_tmp1BD;struct Cyc_Absyn_Exp*
_tmp1BF;struct Cyc_Absyn_Exp*_tmp1C1;struct Cyc_Core_Opt*_tmp1C2;struct Cyc_Core_Opt
_tmp1C3;struct Cyc_Absyn_Exp*_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C6;struct Cyc_Core_Opt*
_tmp1C7;struct Cyc_Absyn_Exp*_tmp1C8;struct Cyc_Absyn_Exp*_tmp1CA;struct Cyc_Absyn_Exp*
_tmp1CB;struct Cyc_Absyn_Exp*_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CF;struct Cyc_List_List*
_tmp1D0;struct Cyc_Absyn_MallocInfo _tmp1D2;int _tmp1D3;struct Cyc_Absyn_Exp*_tmp1D4;
void**_tmp1D5;struct Cyc_Absyn_Exp*_tmp1D6;int _tmp1D7;struct Cyc_Absyn_Exp*_tmp1D9;
struct Cyc_Absyn_Exp*_tmp1DA;struct Cyc_Absyn_Exp*_tmp1DC;struct Cyc_Absyn_Exp*
_tmp1DD;struct Cyc_Absyn_Exp*_tmp1DF;struct Cyc_Absyn_Exp*_tmp1E1;struct Cyc_Absyn_Exp*
_tmp1E3;struct _dyneither_ptr*_tmp1E4;struct Cyc_Absyn_Exp*_tmp1E6;struct
_dyneither_ptr*_tmp1E7;struct Cyc_Absyn_Exp*_tmp1E9;struct _dyneither_ptr*_tmp1EA;
struct Cyc_Absyn_Exp*_tmp1EC;struct Cyc_Absyn_Exp*_tmp1ED;struct Cyc_Absyn_Exp*
_tmp1EE;struct Cyc_Absyn_Exp*_tmp1F0;struct Cyc_Absyn_Exp*_tmp1F1;struct Cyc_Absyn_Exp*
_tmp1F3;struct Cyc_Absyn_Exp*_tmp1F4;struct Cyc_Absyn_Exp*_tmp1F6;struct Cyc_Absyn_Exp*
_tmp1F7;struct Cyc_List_List*_tmp1F9;struct Cyc_Absyn_Datatypedecl*_tmp1FA;struct
Cyc_List_List*_tmp1FC;struct Cyc_List_List*_tmp1FE;struct Cyc_List_List*_tmp200;
struct Cyc_Absyn_Aggrdecl*_tmp201;struct Cyc_Absyn_Aggrdecl _tmp202;enum Cyc_Absyn_AggrKind
_tmp203;struct Cyc_Absyn_AggrdeclImpl*_tmp204;struct Cyc_Absyn_AggrdeclImpl _tmp205;
struct Cyc_List_List*_tmp206;struct Cyc_List_List*_tmp209;struct Cyc_Absyn_Vardecl*
_tmp20B;struct Cyc_Absyn_Exp*_tmp20C;struct Cyc_Absyn_Exp*_tmp20D;int _tmp20E;
struct Cyc_Absyn_Stmt*_tmp210;void*_tmp212;_LL107: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*
_tmp18E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp18E->tag != 
13)goto _LL109;else{_tmp18F=_tmp18E->f2;_tmp190=_tmp18E->f4;if(_tmp190 != Cyc_Absyn_NonNull_to_Null)
goto _LL109;}}_LL108: {union Cyc_CfFlowInfo_FlowInfo _tmp219;void*_tmp21A;struct
_tuple14 _tmp218=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp18F);
_tmp219=_tmp218.f1;_tmp21A=_tmp218.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp21C;void*
_tmp21D;struct _tuple14 _tmp21B=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp219,
_tmp18F,_tmp21A);_tmp21C=_tmp21B.f1;_tmp21D=_tmp21B.f2;{struct _tuple14 _tmp7E5;
return(_tmp7E5.f1=_tmp21C,((_tmp7E5.f2=_tmp21A,_tmp7E5)));};};}_LL109: {struct
Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp191=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)
_tmp18D;if(_tmp191->tag != 13)goto _LL10B;else{_tmp192=_tmp191->f2;}}_LL10A:
_tmp194=_tmp192;goto _LL10C;_LL10B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*
_tmp193=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp18D;if(
_tmp193->tag != 11)goto _LL10D;else{_tmp194=_tmp193->f1;}}_LL10C: _tmp196=_tmp194;
goto _LL10E;_LL10D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp195=(
struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp195->tag != 12)
goto _LL10F;else{_tmp196=_tmp195->f1;}}_LL10E: return Cyc_NewControlFlow_anal_Rexp(
env,copy_ctxt,inflow,_tmp196);_LL10F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp197=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp197->tag != 
0)goto _LL111;else{_tmp198=_tmp197->f1;if((_tmp198.Null_c).tag != 1)goto _LL111;
_tmp199=(int)(_tmp198.Null_c).val;}}_LL110: goto _LL112;_LL111: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp19A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp19A->tag != 
0)goto _LL113;else{_tmp19B=_tmp19A->f1;if((_tmp19B.Int_c).tag != 5)goto _LL113;
_tmp19C=(struct _tuple6)(_tmp19B.Int_c).val;_tmp19D=_tmp19C.f2;if(_tmp19D != 0)
goto _LL113;}}_LL112: {struct _tuple14 _tmp7E6;return(_tmp7E6.f1=inflow,((_tmp7E6.f2=
_tmp173->zero,_tmp7E6)));}_LL113: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp19E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp19E->tag != 
0)goto _LL115;else{_tmp19F=_tmp19E->f1;if((_tmp19F.Int_c).tag != 5)goto _LL115;
_tmp1A0=(struct _tuple6)(_tmp19F.Int_c).val;}}_LL114: goto _LL116;_LL115: {struct
Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1A1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp18D;if(_tmp1A1->tag != 1)goto _LL117;else{_tmp1A2=(void*)_tmp1A1->f2;{struct
Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp1A3=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)
_tmp1A2;if(_tmp1A3->tag != 2)goto _LL117;};}}_LL116: {struct _tuple14 _tmp7E7;return(
_tmp7E7.f1=inflow,((_tmp7E7.f2=_tmp173->notzeroall,_tmp7E7)));}_LL117: {struct
Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1A4=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)
_tmp18D;if(_tmp1A4->tag != 29)goto _LL119;else{_tmp1A5=_tmp1A4->f1;if(_tmp1A5 != 0)
goto _LL119;}}_LL118: goto _LL11A;_LL119: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp1A6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1A6->tag != 
0)goto _LL11B;}_LL11A: goto _LL11C;_LL11B: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*
_tmp1A7=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1A7->tag
!= 17)goto _LL11D;}_LL11C: goto _LL11E;_LL11D: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*
_tmp1A8=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1A8->tag
!= 16)goto _LL11F;}_LL11E: goto _LL120;_LL11F: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*
_tmp1A9=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1A9->tag
!= 18)goto _LL121;}_LL120: goto _LL122;_LL121: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*
_tmp1AA=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1AA->tag
!= 31)goto _LL123;}_LL122: goto _LL124;_LL123: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*
_tmp1AB=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1AB->tag != 
30)goto _LL125;}_LL124: {struct _tuple14 _tmp7E8;return(_tmp7E8.f1=inflow,((_tmp7E8.f2=
_tmp173->unknown_all,_tmp7E8)));}_LL125: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp1AC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1AC->tag != 1)
goto _LL127;else{_tmp1AD=(void*)_tmp1AC->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp1AE=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp1AD;if(_tmp1AE->tag
!= 1)goto _LL127;};}}_LL126: {struct _tuple14 _tmp7E9;return(_tmp7E9.f1=inflow,((
_tmp7E9.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp173,(void*)_check_null(e->topt),0,
_tmp173->unknown_all),_tmp7E9)));}_LL127: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp1AF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1AF->tag != 1)
goto _LL129;else{_tmp1B0=(void*)_tmp1AF->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*
_tmp1B1=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp1B0;if(_tmp1B1->tag != 
3)goto _LL129;else{_tmp1B2=_tmp1B1->f1;}};}}_LL128: _tmp1B6=_tmp1B2;goto _LL12A;
_LL129: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1B3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp18D;if(_tmp1B3->tag != 1)goto _LL12B;else{_tmp1B4=(void*)_tmp1B3->f2;{struct
Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp1B5=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)
_tmp1B4;if(_tmp1B5->tag != 4)goto _LL12B;else{_tmp1B6=_tmp1B5->f1;}};}}_LL12A:
_tmp1BA=_tmp1B6;goto _LL12C;_LL12B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp1B7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1B7->tag != 1)
goto _LL12D;else{_tmp1B8=(void*)_tmp1B7->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*
_tmp1B9=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp1B8;if(_tmp1B9->tag != 5)
goto _LL12D;else{_tmp1BA=_tmp1B9->f1;}};}}_LL12C: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct
_tmp7EA;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp7EA.tag=0,((
_tmp7EA.f1=_tmp1BA,_tmp7EA)));struct _tuple14 _tmp7EB;return(_tmp7EB.f1=inflow,((
_tmp7EB.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(
d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp7EB)));}_LL12D: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*
_tmp1BB=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1BB->tag
!= 2)goto _LL12F;else{_tmp1BC=_tmp1BB->f1;_tmp1BD=_tmp1BB->f2;}}_LL12E: {union Cyc_CfFlowInfo_FlowInfo
_tmp226;void*_tmp227;struct _tuple14 _tmp225=Cyc_NewControlFlow_anal_use_ints(env,
inflow,_tmp1BD);_tmp226=_tmp225.f1;_tmp227=_tmp225.f2;{struct _tuple14 _tmp7EC;
return(_tmp7EC.f1=_tmp226,((_tmp7EC.f2=_tmp227,_tmp7EC)));};}_LL12F: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*
_tmp1BE=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1BE->tag
!= 4)goto _LL131;else{_tmp1BF=_tmp1BE->f1;}}_LL130: {struct Cyc_List_List _tmp7ED;
struct Cyc_List_List _tmp229=(_tmp7ED.hd=_tmp1BF,((_tmp7ED.tl=0,_tmp7ED)));union
Cyc_CfFlowInfo_FlowInfo _tmp22B;struct _tuple14 _tmp22A=Cyc_NewControlFlow_anal_use_ints(
env,inflow,(struct Cyc_List_List*)& _tmp229);_tmp22B=_tmp22A.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp22D;struct _tuple15 _tmp22C=Cyc_NewControlFlow_anal_Lexp(env,_tmp22B,0,_tmp1BF);
_tmp22D=_tmp22C.f2;{struct _tuple21 _tmp7EE;struct _tuple21 _tmp22F=(_tmp7EE.f1=
_tmp22D,((_tmp7EE.f2=_tmp22B,_tmp7EE)));union Cyc_CfFlowInfo_AbsLVal _tmp230;
struct Cyc_CfFlowInfo_Place*_tmp231;union Cyc_CfFlowInfo_FlowInfo _tmp232;struct
_tuple13 _tmp233;struct Cyc_Dict_Dict _tmp234;struct Cyc_List_List*_tmp235;_LL16E:
_tmp230=_tmp22F.f1;if((_tmp230.PlaceL).tag != 1)goto _LL170;_tmp231=(struct Cyc_CfFlowInfo_Place*)(
_tmp230.PlaceL).val;_tmp232=_tmp22F.f2;if((_tmp232.ReachableFL).tag != 2)goto
_LL170;_tmp233=(struct _tuple13)(_tmp232.ReachableFL).val;_tmp234=_tmp233.f1;
_tmp235=_tmp233.f2;_LL16F: _tmp235=Cyc_CfFlowInfo_reln_kill_exp(_tmp173->r,
_tmp235,_tmp1BF);_tmp22B=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
_tmp173,_tmp1BF->loc,_tmp234,env->all_changed,_tmp231,_tmp173->unknown_all),
_tmp235);goto _LL16D;_LL170:;_LL171: goto _LL16D;_LL16D:;}{struct _tuple14 _tmp7EF;
return(_tmp7EF.f1=_tmp22B,((_tmp7EF.f2=_tmp173->unknown_all,_tmp7EF)));};};}
_LL131: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp1C0=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)
_tmp18D;if(_tmp1C0->tag != 3)goto _LL133;else{_tmp1C1=_tmp1C0->f1;_tmp1C2=_tmp1C0->f2;
if(_tmp1C2 == 0)goto _LL133;_tmp1C3=*_tmp1C2;_tmp1C4=_tmp1C0->f3;}}_LL132: if(
copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){{
const char*_tmp7F2;void*_tmp7F1;(_tmp7F1=0,Cyc_Tcutil_terr(e->loc,((_tmp7F2="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp7F2,sizeof(char),58))),_tag_dyneither(_tmp7F1,sizeof(void*),0)));}{
struct _tuple14 _tmp7F3;return(_tmp7F3.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7F3.f2=
_tmp173->unknown_all,_tmp7F3)));};}{struct Cyc_List_List _tmp7F4;struct Cyc_List_List
_tmp23B=(_tmp7F4.hd=_tmp1C4,((_tmp7F4.tl=0,_tmp7F4)));struct Cyc_List_List _tmp7F5;
struct Cyc_List_List _tmp23C=(_tmp7F5.hd=_tmp1C1,((_tmp7F5.tl=(struct Cyc_List_List*)&
_tmp23B,_tmp7F5)));union Cyc_CfFlowInfo_FlowInfo _tmp23E;struct _tuple14 _tmp23D=Cyc_NewControlFlow_anal_use_ints(
env,inflow,(struct Cyc_List_List*)& _tmp23C);_tmp23E=_tmp23D.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp240;struct _tuple15 _tmp23F=Cyc_NewControlFlow_anal_Lexp(env,_tmp23E,0,_tmp1C1);
_tmp240=_tmp23F.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp241=_tmp23E;struct _tuple13
_tmp242;struct Cyc_Dict_Dict _tmp243;struct Cyc_List_List*_tmp244;_LL173: if((
_tmp241.ReachableFL).tag != 2)goto _LL175;_tmp242=(struct _tuple13)(_tmp241.ReachableFL).val;
_tmp243=_tmp242.f1;_tmp244=_tmp242.f2;_LL174:{union Cyc_CfFlowInfo_AbsLVal _tmp245=
_tmp240;struct Cyc_CfFlowInfo_Place*_tmp246;int _tmp247;_LL178: if((_tmp245.PlaceL).tag
!= 1)goto _LL17A;_tmp246=(struct Cyc_CfFlowInfo_Place*)(_tmp245.PlaceL).val;_LL179:
_tmp244=Cyc_CfFlowInfo_reln_kill_exp(_tmp173->r,_tmp244,_tmp1C1);_tmp243=Cyc_CfFlowInfo_assign_place(
_tmp173,_tmp1C1->loc,_tmp243,env->all_changed,_tmp246,_tmp173->unknown_all);
_tmp23E=Cyc_CfFlowInfo_ReachableFL(_tmp243,_tmp244);goto _LL177;_LL17A: if((
_tmp245.UnknownL).tag != 2)goto _LL177;_tmp247=(int)(_tmp245.UnknownL).val;_LL17B:
goto _LL177;_LL177:;}goto _LL172;_LL175:;_LL176: goto _LL172;_LL172:;}{struct
_tuple14 _tmp7F6;return(_tmp7F6.f1=_tmp23E,((_tmp7F6.f2=_tmp173->unknown_all,
_tmp7F6)));};};};_LL133: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*
_tmp1C5=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1C5->tag
!= 3)goto _LL135;else{_tmp1C6=_tmp1C5->f1;_tmp1C7=_tmp1C5->f2;if(_tmp1C7 != 0)goto
_LL135;_tmp1C8=_tmp1C5->f3;}}_LL134: if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)_check_null(e->topt))){{const char*_tmp7F9;void*_tmp7F8;(_tmp7F8=0,
Cyc_Tcutil_terr(e->loc,((_tmp7F9="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp7F9,sizeof(char),58))),_tag_dyneither(_tmp7F8,sizeof(void*),0)));}{
struct _tuple14 _tmp7FA;return(_tmp7FA.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7FA.f2=
_tmp173->unknown_all,_tmp7FA)));};}{struct Cyc_Dict_Dict*_tmp24E=env->all_changed;
while(1){{struct Cyc_Dict_Dict*_tmp7FB;env->all_changed=((_tmp7FB=_region_malloc(
env->r,sizeof(*_tmp7FB)),((_tmp7FB[0]=_tmp173->mt_place_set,_tmp7FB))));}{union
Cyc_CfFlowInfo_FlowInfo _tmp251;union Cyc_CfFlowInfo_AbsLVal _tmp252;struct _tuple15
_tmp250=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp1C6);_tmp251=_tmp250.f1;
_tmp252=_tmp250.f2;{struct Cyc_Dict_Dict _tmp253=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));{struct Cyc_Dict_Dict*_tmp7FC;env->all_changed=((
_tmp7FC=_region_malloc(env->r,sizeof(*_tmp7FC)),((_tmp7FC[0]=_tmp173->mt_place_set,
_tmp7FC))));}{union Cyc_CfFlowInfo_FlowInfo _tmp256;void*_tmp257;struct _tuple14
_tmp255=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1C8);_tmp256=_tmp255.f1;
_tmp257=_tmp255.f2;{struct Cyc_Dict_Dict _tmp258=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo _tmp259=Cyc_CfFlowInfo_after_flow(
_tmp173,(struct Cyc_Dict_Dict*)& _tmp253,_tmp251,_tmp256,_tmp253,_tmp258);union Cyc_CfFlowInfo_FlowInfo
_tmp25A=Cyc_CfFlowInfo_join_flow(_tmp173,_tmp24E,inflow,_tmp259);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp25A,inflow)){if(_tmp24E == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp24E,Cyc_CfFlowInfo_union_place_set(
_tmp253,_tmp258,0),0);}return Cyc_NewControlFlow_do_assign(_tmp173,env,_tmp259,
_tmp1C6,_tmp252,_tmp1C8,_tmp257,e->loc);}inflow=_tmp25A;};};};};}};_LL135: {
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1C9=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)
_tmp18D;if(_tmp1C9->tag != 8)goto _LL137;else{_tmp1CA=_tmp1C9->f1;_tmp1CB=_tmp1C9->f2;}}
_LL136: {union Cyc_CfFlowInfo_FlowInfo _tmp25C;void*_tmp25D;struct _tuple14 _tmp25B=
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1CA);_tmp25C=_tmp25B.f1;_tmp25D=
_tmp25B.f2;return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp25C,_tmp1CB);}
_LL137: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp1CC=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)
_tmp18D;if(_tmp1CC->tag != 10)goto _LL139;else{_tmp1CD=_tmp1CC->f1;}}_LL138: {
union Cyc_CfFlowInfo_FlowInfo _tmp25F;void*_tmp260;struct _tuple14 _tmp25E=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp1CD);_tmp25F=_tmp25E.f1;_tmp260=_tmp25E.f2;Cyc_NewControlFlow_use_Rval(
env,_tmp1CD->loc,_tmp25F,_tmp260);{struct _tuple14 _tmp7FD;return(_tmp7FD.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp7FD.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp173,(void*)_check_null(e->topt),0,
_tmp173->unknown_all),_tmp7FD)));};}_LL139: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*
_tmp1CE=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1CE->tag
!= 9)goto _LL13B;else{_tmp1CF=_tmp1CE->f1;_tmp1D0=_tmp1CE->f2;}}_LL13A: {struct
_RegionHandle _tmp262=_new_region("temp");struct _RegionHandle*temp=& _tmp262;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo _tmp265;struct Cyc_List_List*
_tmp266;struct Cyc_List_List*_tmp7FE;struct _tuple20 _tmp264=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,((_tmp7FE=_region_malloc(temp,sizeof(*_tmp7FE)),((_tmp7FE->hd=
_tmp1CF,((_tmp7FE->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rcopy)(temp,_tmp1D0),_tmp7FE)))))),0,1);_tmp265=_tmp264.f1;_tmp266=
_tmp264.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp267=Cyc_NewControlFlow_use_Rval(env,
_tmp1CF->loc,_tmp265,(void*)((struct Cyc_List_List*)_check_null(_tmp266))->hd);
_tmp266=_tmp266->tl;{struct Cyc_List_List*init_params=0;struct Cyc_List_List*
nolive_unique_params=0;{void*_tmp268=Cyc_Tcutil_compress((void*)_check_null(
_tmp1CF->topt));struct Cyc_Absyn_PtrInfo _tmp26A;void*_tmp26B;_LL17D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp269=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp268;if(_tmp269->tag
!= 5)goto _LL17F;else{_tmp26A=_tmp269->f1;_tmp26B=_tmp26A.elt_typ;}}_LL17E:{void*
_tmp26C=Cyc_Tcutil_compress(_tmp26B);struct Cyc_Absyn_FnInfo _tmp26E;struct Cyc_List_List*
_tmp26F;_LL182: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp26D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)
_tmp26C;if(_tmp26D->tag != 9)goto _LL184;else{_tmp26E=_tmp26D->f1;_tmp26F=_tmp26E.attributes;}}
_LL183: for(0;_tmp26F != 0;_tmp26F=_tmp26F->tl){void*_tmp270=(void*)_tmp26F->hd;
int _tmp272;int _tmp274;_LL187: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*
_tmp271=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp270;if(
_tmp271->tag != 20)goto _LL189;else{_tmp272=_tmp271->f1;}}_LL188:{struct Cyc_List_List*
_tmp7FF;init_params=((_tmp7FF=_region_malloc(temp,sizeof(*_tmp7FF)),((_tmp7FF->hd=(
void*)_tmp272,((_tmp7FF->tl=init_params,_tmp7FF))))));}goto _LL186;_LL189: {
struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp273=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)
_tmp270;if(_tmp273->tag != 21)goto _LL18B;else{_tmp274=_tmp273->f1;}}_LL18A:{
struct Cyc_List_List*_tmp800;nolive_unique_params=((_tmp800=_region_malloc(temp,
sizeof(*_tmp800)),((_tmp800->hd=(void*)_tmp274,((_tmp800->tl=
nolive_unique_params,_tmp800))))));}goto _LL186;_LL18B:;_LL18C: goto _LL186;_LL186:;}
goto _LL181;_LL184:;_LL185: {const char*_tmp803;void*_tmp802;(_tmp802=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp803="anal_Rexp: bad function type",
_tag_dyneither(_tmp803,sizeof(char),29))),_tag_dyneither(_tmp802,sizeof(void*),0)));}
_LL181:;}goto _LL17C;_LL17F:;_LL180: {const char*_tmp806;void*_tmp805;(_tmp805=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp806="anal_Rexp: bad function type",_tag_dyneither(_tmp806,sizeof(char),29))),
_tag_dyneither(_tmp805,sizeof(void*),0)));}_LL17C:;}{int i=1;for(0;_tmp266 != 0;(((
_tmp266=_tmp266->tl,_tmp1D0=((struct Cyc_List_List*)_check_null(_tmp1D0))->tl)),
++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i) && 
!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i)){
_tmp267=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1D0))->hd)->loc,_tmp267,(void*)_tmp266->hd);continue;}if(((int(*)(
struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i)){_tmp267=Cyc_NewControlFlow_use_nounique_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1D0))->hd)->loc,(
void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp1D0))->hd)->topt),_tmp267,(void*)_tmp266->hd);continue;}{union Cyc_CfFlowInfo_FlowInfo
_tmp27B=_tmp265;int _tmp27C;struct _tuple13 _tmp27D;struct Cyc_Dict_Dict _tmp27E;
_LL18E: if((_tmp27B.BottomFL).tag != 1)goto _LL190;_tmp27C=(int)(_tmp27B.BottomFL).val;
_LL18F: goto _LL18D;_LL190: if((_tmp27B.ReachableFL).tag != 2)goto _LL18D;_tmp27D=(
struct _tuple13)(_tmp27B.ReachableFL).val;_tmp27E=_tmp27D.f1;_LL191: if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp27E,(void*)_tmp266->hd)== Cyc_CfFlowInfo_NoneIL){const char*_tmp809;
void*_tmp808;(_tmp808=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1D0))->hd)->loc,((_tmp809="expression may not be initialized",
_tag_dyneither(_tmp809,sizeof(char),34))),_tag_dyneither(_tmp808,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp281=_tmp267;int _tmp282;struct _tuple13 _tmp283;
struct Cyc_Dict_Dict _tmp284;struct Cyc_List_List*_tmp285;_LL193: if((_tmp281.BottomFL).tag
!= 1)goto _LL195;_tmp282=(int)(_tmp281.BottomFL).val;_LL194: goto _LL192;_LL195: if((
_tmp281.ReachableFL).tag != 2)goto _LL192;_tmp283=(struct _tuple13)(_tmp281.ReachableFL).val;
_tmp284=_tmp283.f1;_tmp285=_tmp283.f2;_LL196: {struct Cyc_Dict_Dict _tmp286=Cyc_CfFlowInfo_escape_deref(
_tmp173,_tmp284,env->all_changed,(void*)_tmp266->hd);{void*_tmp287=(void*)
_tmp266->hd;struct Cyc_CfFlowInfo_Place*_tmp289;_LL198: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp288=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp287;if(
_tmp288->tag != 5)goto _LL19A;else{_tmp289=_tmp288->f1;}}_LL199:{void*_tmp28A=Cyc_Tcutil_compress((
void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp1D0))->hd)->topt));struct Cyc_Absyn_PtrInfo _tmp28C;void*_tmp28D;_LL19D: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp28B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp28A;if(_tmp28B->tag != 5)goto _LL19F;else{_tmp28C=_tmp28B->f1;_tmp28D=_tmp28C.elt_typ;}}
_LL19E: _tmp286=Cyc_CfFlowInfo_assign_place(_tmp173,((struct Cyc_Absyn_Exp*)
_tmp1D0->hd)->loc,_tmp286,env->all_changed,_tmp289,Cyc_CfFlowInfo_typ_to_absrval(
_tmp173,_tmp28D,0,_tmp173->esc_all));goto _LL19C;_LL19F:;_LL1A0: {const char*
_tmp80C;void*_tmp80B;(_tmp80B=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp80C="anal_Rexp:bad type for initialized arg",
_tag_dyneither(_tmp80C,sizeof(char),39))),_tag_dyneither(_tmp80B,sizeof(void*),0)));}
_LL19C:;}goto _LL197;_LL19A:;_LL19B: goto _LL197;_LL197:;}_tmp267=Cyc_CfFlowInfo_ReachableFL(
_tmp286,_tmp285);goto _LL192;}_LL192:;}goto _LL18D;_LL18D:;};}}if(Cyc_Tcutil_is_noreturn((
void*)_check_null(_tmp1CF->topt))){struct _tuple14 _tmp80D;struct _tuple14 _tmp291=(
_tmp80D.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp80D.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp173,(void*)_check_null(e->topt),0,_tmp173->unknown_all),_tmp80D)));
_npop_handler(0);return _tmp291;}else{struct _tuple14 _tmp80E;struct _tuple14 _tmp293=(
_tmp80E.f1=_tmp267,((_tmp80E.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp173,(void*)
_check_null(e->topt),0,_tmp173->unknown_all),_tmp80E)));_npop_handler(0);return
_tmp293;}};};};_pop_region(temp);}_LL13B: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*
_tmp1D1=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1D1->tag
!= 32)goto _LL13D;else{_tmp1D2=_tmp1D1->f1;_tmp1D3=_tmp1D2.is_calloc;_tmp1D4=
_tmp1D2.rgn;_tmp1D5=_tmp1D2.elt_type;_tmp1D6=_tmp1D2.num_elts;_tmp1D7=_tmp1D2.fat_result;}}
_LL13C: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp811;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*
_tmp810;void*root=(void*)((_tmp810=_region_malloc(_tmp173->r,sizeof(*_tmp810)),((
_tmp810[0]=((_tmp811.tag=1,((_tmp811.f1=_tmp1D6,((_tmp811.f2=(void*)((void*)
_check_null(e->topt)),_tmp811)))))),_tmp810))));struct Cyc_CfFlowInfo_Place*
_tmp812;struct Cyc_CfFlowInfo_Place*place=(_tmp812=_region_malloc(_tmp173->r,
sizeof(*_tmp812)),((_tmp812->root=root,((_tmp812->fields=0,_tmp812)))));struct
Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp815;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp814;void*rval=(void*)((_tmp814=_region_malloc(_tmp173->r,sizeof(*_tmp814)),((
_tmp814[0]=((_tmp815.tag=5,((_tmp815.f1=place,_tmp815)))),_tmp814))));void*
place_val;if(_tmp1D7)place_val=_tmp173->notzeroall;else{if(_tmp1D3)place_val=Cyc_CfFlowInfo_typ_to_absrval(
_tmp173,*((void**)_check_null(_tmp1D5)),0,_tmp173->zero);else{place_val=Cyc_CfFlowInfo_typ_to_absrval(
_tmp173,*((void**)_check_null(_tmp1D5)),0,_tmp173->unknown_none);}}{union Cyc_CfFlowInfo_FlowInfo
outflow;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,
unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);if(
_tmp1D4 != 0){struct _RegionHandle _tmp294=_new_region("temp");struct _RegionHandle*
temp=& _tmp294;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo _tmp297;struct Cyc_List_List*
_tmp298;struct Cyc_Absyn_Exp*_tmp816[2];struct _tuple20 _tmp296=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,((_tmp816[1]=_tmp1D6,((_tmp816[0]=(struct Cyc_Absyn_Exp*)_tmp1D4,((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(
temp,_tag_dyneither(_tmp816,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp297=
_tmp296.f1;_tmp298=_tmp296.f2;outflow=_tmp297;};_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo
_tmp29A;struct _tuple14 _tmp299=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1D6);
_tmp29A=_tmp299.f1;outflow=_tmp29A;}{union Cyc_CfFlowInfo_FlowInfo _tmp29B=outflow;
int _tmp29C;struct _tuple13 _tmp29D;struct Cyc_Dict_Dict _tmp29E;struct Cyc_List_List*
_tmp29F;_LL1A2: if((_tmp29B.BottomFL).tag != 1)goto _LL1A4;_tmp29C=(int)(_tmp29B.BottomFL).val;
_LL1A3: {struct _tuple14 _tmp817;return(_tmp817.f1=outflow,((_tmp817.f2=rval,
_tmp817)));}_LL1A4: if((_tmp29B.ReachableFL).tag != 2)goto _LL1A1;_tmp29D=(struct
_tuple13)(_tmp29B.ReachableFL).val;_tmp29E=_tmp29D.f1;_tmp29F=_tmp29D.f2;_LL1A5: {
struct _tuple14 _tmp818;return(_tmp818.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp29E,root,place_val),
_tmp29F),((_tmp818.f2=rval,_tmp818)));}_LL1A1:;};};}_LL13D: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*
_tmp1D8=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1D8->tag != 
33)goto _LL13F;else{_tmp1D9=_tmp1D8->f1;_tmp1DA=_tmp1D8->f2;}}_LL13E: {void*
left_rval;void*right_rval;union Cyc_CfFlowInfo_FlowInfo outflow;struct
_RegionHandle _tmp2A7=_new_region("temp");struct _RegionHandle*temp=& _tmp2A7;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo _tmp2AA;struct Cyc_List_List*
_tmp2AB;struct Cyc_Absyn_Exp*_tmp819[2];struct _tuple20 _tmp2A9=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,((_tmp819[1]=_tmp1DA,((_tmp819[0]=_tmp1D9,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(
_tmp819,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);_tmp2AA=_tmp2A9.f1;_tmp2AB=
_tmp2A9.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2AB))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2AB->tl))->hd;outflow=
_tmp2AA;}{union Cyc_CfFlowInfo_AbsLVal _tmp2AD;struct _tuple15 _tmp2AC=Cyc_NewControlFlow_anal_Lexp(
env,outflow,0,_tmp1D9);_tmp2AD=_tmp2AC.f2;{union Cyc_CfFlowInfo_AbsLVal _tmp2AF;
struct _tuple15 _tmp2AE=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp1DA);
_tmp2AF=_tmp2AE.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2B0=outflow;struct _tuple13
_tmp2B1;struct Cyc_Dict_Dict _tmp2B2;struct Cyc_List_List*_tmp2B3;_LL1A7: if((
_tmp2B0.ReachableFL).tag != 2)goto _LL1A9;_tmp2B1=(struct _tuple13)(_tmp2B0.ReachableFL).val;
_tmp2B2=_tmp2B1.f1;_tmp2B3=_tmp2B1.f2;_LL1A8:{union Cyc_CfFlowInfo_AbsLVal _tmp2B4=
_tmp2AD;struct Cyc_CfFlowInfo_Place*_tmp2B5;int _tmp2B6;_LL1AC: if((_tmp2B4.PlaceL).tag
!= 1)goto _LL1AE;_tmp2B5=(struct Cyc_CfFlowInfo_Place*)(_tmp2B4.PlaceL).val;_LL1AD:
_tmp2B2=Cyc_CfFlowInfo_assign_place(_tmp173,_tmp1D9->loc,_tmp2B2,env->all_changed,
_tmp2B5,right_rval);goto _LL1AB;_LL1AE: if((_tmp2B4.UnknownL).tag != 2)goto _LL1AB;
_tmp2B6=(int)(_tmp2B4.UnknownL).val;_LL1AF: goto _LL1AB;_LL1AB:;}{union Cyc_CfFlowInfo_AbsLVal
_tmp2B7=_tmp2AF;struct Cyc_CfFlowInfo_Place*_tmp2B8;int _tmp2B9;_LL1B1: if((_tmp2B7.PlaceL).tag
!= 1)goto _LL1B3;_tmp2B8=(struct Cyc_CfFlowInfo_Place*)(_tmp2B7.PlaceL).val;_LL1B2:
_tmp2B2=Cyc_CfFlowInfo_assign_place(_tmp173,_tmp1DA->loc,_tmp2B2,env->all_changed,
_tmp2B8,left_rval);goto _LL1B0;_LL1B3: if((_tmp2B7.UnknownL).tag != 2)goto _LL1B0;
_tmp2B9=(int)(_tmp2B7.UnknownL).val;_LL1B4: goto _LL1B0;_LL1B0:;}_tmp2B3=Cyc_CfFlowInfo_reln_kill_exp(
_tmp173->r,_tmp2B3,_tmp1D9);_tmp2B3=Cyc_CfFlowInfo_reln_kill_exp(_tmp173->r,
_tmp2B3,_tmp1DA);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp2B2,_tmp2B3);goto _LL1A6;
_LL1A9:;_LL1AA: goto _LL1A6;_LL1A6:;}{struct _tuple14 _tmp81A;struct _tuple14 _tmp2BB=(
_tmp81A.f1=outflow,((_tmp81A.f2=_tmp173->unknown_all,_tmp81A)));_npop_handler(0);
return _tmp2BB;};};};;_pop_region(temp);}_LL13F: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*
_tmp1DB=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1DB->tag != 
15)goto _LL141;else{_tmp1DC=_tmp1DB->f1;_tmp1DD=_tmp1DB->f2;}}_LL140: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct
_tmp81D;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp81C;void*root=(
void*)((_tmp81C=_region_malloc(_tmp173->r,sizeof(*_tmp81C)),((_tmp81C[0]=((
_tmp81D.tag=1,((_tmp81D.f1=_tmp1DD,((_tmp81D.f2=(void*)((void*)_check_null(e->topt)),
_tmp81D)))))),_tmp81C))));struct Cyc_CfFlowInfo_Place*_tmp81E;struct Cyc_CfFlowInfo_Place*
place=(_tmp81E=_region_malloc(_tmp173->r,sizeof(*_tmp81E)),((_tmp81E->root=root,((
_tmp81E->fields=0,_tmp81E)))));struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct
_tmp821;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp820;void*
rval=(void*)((_tmp820=_region_malloc(_tmp173->r,sizeof(*_tmp820)),((_tmp820[0]=((
_tmp821.tag=5,((_tmp821.f1=place,_tmp821)))),_tmp820))));((int(*)(struct Cyc_Dict_Dict*
set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);{union Cyc_CfFlowInfo_FlowInfo outflow;void*place_val;
if(_tmp1DC != 0){struct _RegionHandle _tmp2BC=_new_region("temp");struct
_RegionHandle*temp=& _tmp2BC;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo
_tmp2BF;struct Cyc_List_List*_tmp2C0;struct Cyc_Absyn_Exp*_tmp822[2];struct
_tuple20 _tmp2BE=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((
_tmp822[1]=_tmp1DD,((_tmp822[0]=(struct Cyc_Absyn_Exp*)_tmp1DC,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(
_tmp822,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp2BF=_tmp2BE.f1;_tmp2C0=
_tmp2BE.f2;outflow=_tmp2BF;place_val=(void*)((struct Cyc_List_List*)_check_null(((
struct Cyc_List_List*)_check_null(_tmp2C0))->tl))->hd;};_pop_region(temp);}else{
union Cyc_CfFlowInfo_FlowInfo _tmp2C2;void*_tmp2C3;struct _tuple14 _tmp2C1=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp1DD);_tmp2C2=_tmp2C1.f1;_tmp2C3=_tmp2C1.f2;outflow=_tmp2C2;
place_val=_tmp2C3;}{union Cyc_CfFlowInfo_FlowInfo _tmp2C4=outflow;int _tmp2C5;
struct _tuple13 _tmp2C6;struct Cyc_Dict_Dict _tmp2C7;struct Cyc_List_List*_tmp2C8;
_LL1B6: if((_tmp2C4.BottomFL).tag != 1)goto _LL1B8;_tmp2C5=(int)(_tmp2C4.BottomFL).val;
_LL1B7: {struct _tuple14 _tmp823;return(_tmp823.f1=outflow,((_tmp823.f2=rval,
_tmp823)));}_LL1B8: if((_tmp2C4.ReachableFL).tag != 2)goto _LL1B5;_tmp2C6=(struct
_tuple13)(_tmp2C4.ReachableFL).val;_tmp2C7=_tmp2C6.f1;_tmp2C8=_tmp2C6.f2;_LL1B9: {
struct _tuple14 _tmp824;return(_tmp824.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2C7,root,place_val),
_tmp2C8),((_tmp824.f2=rval,_tmp824)));}_LL1B5:;};};}_LL141: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*
_tmp1DE=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1DE->tag
!= 14)goto _LL143;else{_tmp1DF=_tmp1DE->f1;}}_LL142: {union Cyc_CfFlowInfo_FlowInfo
_tmp2D1;union Cyc_CfFlowInfo_AbsLVal _tmp2D2;struct _tuple15 _tmp2D0=Cyc_NewControlFlow_anal_Lexp(
env,inflow,0,_tmp1DF);_tmp2D1=_tmp2D0.f1;_tmp2D2=_tmp2D0.f2;{union Cyc_CfFlowInfo_AbsLVal
_tmp2D3=_tmp2D2;int _tmp2D4;struct Cyc_CfFlowInfo_Place*_tmp2D5;_LL1BB: if((_tmp2D3.UnknownL).tag
!= 2)goto _LL1BD;_tmp2D4=(int)(_tmp2D3.UnknownL).val;_LL1BC: {struct _tuple14
_tmp825;return(_tmp825.f1=_tmp2D1,((_tmp825.f2=_tmp173->notzeroall,_tmp825)));}
_LL1BD: if((_tmp2D3.PlaceL).tag != 1)goto _LL1BA;_tmp2D5=(struct Cyc_CfFlowInfo_Place*)(
_tmp2D3.PlaceL).val;_LL1BE: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp82B;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp82A;struct
_tuple14 _tmp829;return(_tmp829.f1=_tmp2D1,((_tmp829.f2=(void*)((_tmp82B=
_region_malloc(env->r,sizeof(*_tmp82B)),((_tmp82B[0]=((_tmp82A.tag=5,((_tmp82A.f1=
_tmp2D5,_tmp82A)))),_tmp82B)))),_tmp829)));}_LL1BA:;};}_LL143: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*
_tmp1E0=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1E0->tag != 
19)goto _LL145;else{_tmp1E1=_tmp1E0->f1;}}_LL144: {union Cyc_CfFlowInfo_FlowInfo
_tmp2DB;void*_tmp2DC;struct _tuple14 _tmp2DA=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1E1);_tmp2DB=_tmp2DA.f1;_tmp2DC=_tmp2DA.f2;return Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp2DB,_tmp1E1,_tmp2DC);}_LL145: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*
_tmp1E2=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1E2->tag
!= 20)goto _LL147;else{_tmp1E3=_tmp1E2->f1;_tmp1E4=_tmp1E2->f2;}}_LL146: {union
Cyc_CfFlowInfo_FlowInfo _tmp2DE;void*_tmp2DF;struct _tuple14 _tmp2DD=Cyc_NewControlFlow_anal_Rexp(
env,0,inflow,_tmp1E3);_tmp2DE=_tmp2DD.f1;_tmp2DF=_tmp2DD.f2;if(Cyc_Absyn_is_nontagged_union_type((
void*)_check_null(_tmp1E3->topt))){struct _tuple14 _tmp82C;return(_tmp82C.f1=
_tmp2DE,((_tmp82C.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp173,(void*)_check_null(e->topt),
0,_tmp173->unknown_all),_tmp82C)));}{void*_tmp2E1=_tmp2DF;struct Cyc_CfFlowInfo_UnionRInfo
_tmp2E3;int _tmp2E4;int _tmp2E5;struct _dyneither_ptr _tmp2E6;_LL1C0: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp2E2=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2E1;if(
_tmp2E2->tag != 6)goto _LL1C2;else{_tmp2E3=_tmp2E2->f1;_tmp2E4=_tmp2E3.is_union;
_tmp2E5=_tmp2E3.fieldnum;_tmp2E6=_tmp2E2->f2;}}_LL1C1: {int _tmp2E7=Cyc_CfFlowInfo_get_field_index((
void*)_check_null(_tmp1E3->topt),_tmp1E4);if((_tmp2E4  && _tmp2E5 != - 1) && 
_tmp2E5 != _tmp2E7){struct _tuple14 _tmp82D;return(_tmp82D.f1=_tmp2DE,((_tmp82D.f2=
Cyc_CfFlowInfo_typ_to_absrval(_tmp173,(void*)_check_null(e->topt),1,_tmp173->unknown_none),
_tmp82D)));}{struct _tuple14 _tmp82E;return(_tmp82E.f1=_tmp2DE,((_tmp82E.f2=*((
void**)_check_dyneither_subscript(_tmp2E6,sizeof(void*),_tmp2E7)),_tmp82E)));};}
_LL1C2:;_LL1C3: {struct Cyc_Core_Impossible_exn_struct _tmp83B;const char*_tmp83A;
void*_tmp839[1];struct Cyc_String_pa_PrintArg_struct _tmp838;struct Cyc_Core_Impossible_exn_struct*
_tmp837;(int)_throw((void*)((_tmp837=_cycalloc(sizeof(*_tmp837)),((_tmp837[0]=((
_tmp83B.tag=Cyc_Core_Impossible,((_tmp83B.f1=(struct _dyneither_ptr)((_tmp838.tag=
0,((_tmp838.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp839[0]=& _tmp838,Cyc_aprintf(((_tmp83A="anal_Rexp: AggrMember: %s",
_tag_dyneither(_tmp83A,sizeof(char),26))),_tag_dyneither(_tmp839,sizeof(void*),1)))))))),
_tmp83B)))),_tmp837)))));}_LL1BF:;};}_LL147: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*
_tmp1E5=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1E5->tag
!= 36)goto _LL149;else{_tmp1E6=_tmp1E5->f1;_tmp1E7=_tmp1E5->f2;}}_LL148: {union
Cyc_CfFlowInfo_FlowInfo _tmp2F0;void*_tmp2F1;struct _tuple14 _tmp2EF=Cyc_NewControlFlow_anal_Rexp(
env,0,inflow,_tmp1E6);_tmp2F0=_tmp2EF.f1;_tmp2F1=_tmp2EF.f2;if(Cyc_Absyn_is_nontagged_union_type((
void*)_check_null(_tmp1E6->topt))){struct _tuple14 _tmp83C;return(_tmp83C.f1=
_tmp2F0,((_tmp83C.f2=_tmp173->unknown_all,_tmp83C)));}{void*_tmp2F3=_tmp2F1;
struct Cyc_CfFlowInfo_UnionRInfo _tmp2F5;int _tmp2F6;int _tmp2F7;struct
_dyneither_ptr _tmp2F8;_LL1C5: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp2F4=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2F3;if(
_tmp2F4->tag != 6)goto _LL1C7;else{_tmp2F5=_tmp2F4->f1;_tmp2F6=_tmp2F5.is_union;
_tmp2F7=_tmp2F5.fieldnum;_tmp2F8=_tmp2F4->f2;}}_LL1C6: {int _tmp2F9=Cyc_CfFlowInfo_get_field_index((
void*)_check_null(_tmp1E6->topt),_tmp1E7);if(_tmp2F6  && _tmp2F7 != - 1){if(_tmp2F7
!= _tmp2F9){struct _tuple14 _tmp83D;return(_tmp83D.f1=_tmp2F0,((_tmp83D.f2=_tmp173->zero,
_tmp83D)));}else{struct _tuple14 _tmp83E;return(_tmp83E.f1=_tmp2F0,((_tmp83E.f2=
_tmp173->notzeroall,_tmp83E)));}}else{struct _tuple14 _tmp83F;return(_tmp83F.f1=
_tmp2F0,((_tmp83F.f2=_tmp173->unknown_all,_tmp83F)));}}_LL1C7:;_LL1C8: {struct
Cyc_Core_Impossible_exn_struct _tmp84C;const char*_tmp84B;void*_tmp84A[1];struct
Cyc_String_pa_PrintArg_struct _tmp849;struct Cyc_Core_Impossible_exn_struct*
_tmp848;(int)_throw((void*)((_tmp848=_cycalloc(sizeof(*_tmp848)),((_tmp848[0]=((
_tmp84C.tag=Cyc_Core_Impossible,((_tmp84C.f1=(struct _dyneither_ptr)((_tmp849.tag=
0,((_tmp849.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp84A[0]=& _tmp849,Cyc_aprintf(((_tmp84B="anal_Rexp: TagCheck_e: %s",
_tag_dyneither(_tmp84B,sizeof(char),26))),_tag_dyneither(_tmp84A,sizeof(void*),1)))))))),
_tmp84C)))),_tmp848)))));}_LL1C4:;};}_LL149: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*
_tmp1E8=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1E8->tag
!= 21)goto _LL14B;else{_tmp1E9=_tmp1E8->f1;_tmp1EA=_tmp1E8->f2;}}_LL14A: {union
Cyc_CfFlowInfo_FlowInfo _tmp303;void*_tmp304;struct _tuple14 _tmp302=Cyc_NewControlFlow_anal_Rexp(
env,0,inflow,_tmp1E9);_tmp303=_tmp302.f1;_tmp304=_tmp302.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp306;void*_tmp307;struct _tuple14 _tmp305=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp303,_tmp1E9,_tmp304);_tmp306=_tmp305.f1;_tmp307=_tmp305.f2;{void*
_tmp308=Cyc_Tcutil_compress((void*)_check_null(_tmp1E9->topt));struct Cyc_Absyn_PtrInfo
_tmp30A;void*_tmp30B;_LL1CA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp309=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp308;if(_tmp309->tag
!= 5)goto _LL1CC;else{_tmp30A=_tmp309->f1;_tmp30B=_tmp30A.elt_typ;}}_LL1CB: if(Cyc_Absyn_is_nontagged_union_type(
_tmp30B)){struct _tuple14 _tmp84D;return(_tmp84D.f1=_tmp306,((_tmp84D.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp173,(void*)_check_null(e->topt),0,_tmp173->unknown_all),_tmp84D)));}{void*
_tmp30D=_tmp307;struct Cyc_CfFlowInfo_UnionRInfo _tmp30F;int _tmp310;int _tmp311;
struct _dyneither_ptr _tmp312;_LL1CF: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp30E=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp30D;if(
_tmp30E->tag != 6)goto _LL1D1;else{_tmp30F=_tmp30E->f1;_tmp310=_tmp30F.is_union;
_tmp311=_tmp30F.fieldnum;_tmp312=_tmp30E->f2;}}_LL1D0: {int _tmp313=Cyc_CfFlowInfo_get_field_index(
_tmp30B,_tmp1EA);if((_tmp310  && _tmp311 != - 1) && _tmp311 != _tmp313){struct
_tuple14 _tmp84E;return(_tmp84E.f1=_tmp306,((_tmp84E.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp173,(void*)_check_null(e->topt),1,_tmp173->unknown_none),_tmp84E)));}{struct
_tuple14 _tmp84F;return(_tmp84F.f1=_tmp306,((_tmp84F.f2=*((void**)
_check_dyneither_subscript(_tmp312,sizeof(void*),_tmp313)),_tmp84F)));};}_LL1D1:;
_LL1D2: {struct Cyc_Core_Impossible_exn_struct _tmp855;const char*_tmp854;struct Cyc_Core_Impossible_exn_struct*
_tmp853;(int)_throw((void*)((_tmp853=_cycalloc(sizeof(*_tmp853)),((_tmp853[0]=((
_tmp855.tag=Cyc_Core_Impossible,((_tmp855.f1=((_tmp854="anal_Rexp: AggrArrow",
_tag_dyneither(_tmp854,sizeof(char),21))),_tmp855)))),_tmp853)))));}_LL1CE:;};
_LL1CC:;_LL1CD: {struct Cyc_Core_Impossible_exn_struct _tmp85B;const char*_tmp85A;
struct Cyc_Core_Impossible_exn_struct*_tmp859;(int)_throw((void*)((_tmp859=
_cycalloc(sizeof(*_tmp859)),((_tmp859[0]=((_tmp85B.tag=Cyc_Core_Impossible,((
_tmp85B.f1=((_tmp85A="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp85A,sizeof(
char),25))),_tmp85B)))),_tmp859)))));}_LL1C9:;};};}_LL14B: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*
_tmp1EB=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1EB->tag
!= 5)goto _LL14D;else{_tmp1EC=_tmp1EB->f1;_tmp1ED=_tmp1EB->f2;_tmp1EE=_tmp1EB->f3;}}
_LL14C: {union Cyc_CfFlowInfo_FlowInfo _tmp31D;union Cyc_CfFlowInfo_FlowInfo _tmp31E;
struct _tuple16 _tmp31C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1EC);_tmp31D=
_tmp31C.f1;_tmp31E=_tmp31C.f2;{struct _tuple14 _tmp31F=Cyc_NewControlFlow_anal_Rexp(
env,copy_ctxt,_tmp31D,_tmp1ED);struct _tuple14 _tmp320=Cyc_NewControlFlow_anal_Rexp(
env,copy_ctxt,_tmp31E,_tmp1EE);return Cyc_CfFlowInfo_join_flow_and_rval(_tmp173,
env->all_changed,_tmp31F,_tmp320);};}_LL14D: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*
_tmp1EF=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1EF->tag != 6)
goto _LL14F;else{_tmp1F0=_tmp1EF->f1;_tmp1F1=_tmp1EF->f2;}}_LL14E: {union Cyc_CfFlowInfo_FlowInfo
_tmp322;union Cyc_CfFlowInfo_FlowInfo _tmp323;struct _tuple16 _tmp321=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp1F0);_tmp322=_tmp321.f1;_tmp323=_tmp321.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp325;void*_tmp326;struct _tuple14 _tmp324=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp322,_tmp1F1);_tmp325=_tmp324.f1;_tmp326=_tmp324.f2;{struct _tuple14
_tmp85C;struct _tuple14 _tmp327=(_tmp85C.f1=_tmp325,((_tmp85C.f2=_tmp326,_tmp85C)));
struct _tuple14 _tmp85D;struct _tuple14 _tmp328=(_tmp85D.f1=_tmp323,((_tmp85D.f2=
_tmp173->zero,_tmp85D)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp173,env->all_changed,
_tmp327,_tmp328);};};}_LL14F: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1F2=(
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1F2->tag != 7)goto
_LL151;else{_tmp1F3=_tmp1F2->f1;_tmp1F4=_tmp1F2->f2;}}_LL150: {union Cyc_CfFlowInfo_FlowInfo
_tmp32C;union Cyc_CfFlowInfo_FlowInfo _tmp32D;struct _tuple16 _tmp32B=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp1F3);_tmp32C=_tmp32B.f1;_tmp32D=_tmp32B.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp32F;void*_tmp330;struct _tuple14 _tmp32E=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp32D,_tmp1F4);_tmp32F=_tmp32E.f1;_tmp330=_tmp32E.f2;{struct _tuple14
_tmp85E;struct _tuple14 _tmp331=(_tmp85E.f1=_tmp32F,((_tmp85E.f2=_tmp330,_tmp85E)));
struct _tuple14 _tmp85F;struct _tuple14 _tmp332=(_tmp85F.f1=_tmp32C,((_tmp85F.f2=
_tmp173->notzeroall,_tmp85F)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp173,
env->all_changed,_tmp331,_tmp332);};};}_LL151: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*
_tmp1F5=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1F5->tag
!= 22)goto _LL153;else{_tmp1F6=_tmp1F5->f1;_tmp1F7=_tmp1F5->f2;}}_LL152: {struct
_RegionHandle _tmp335=_new_region("temp");struct _RegionHandle*temp=& _tmp335;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo _tmp338;struct Cyc_List_List*
_tmp339;struct Cyc_Absyn_Exp*_tmp860[2];struct _tuple20 _tmp337=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,((_tmp860[1]=_tmp1F7,((_tmp860[0]=_tmp1F6,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(
_tmp860,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp338=_tmp337.f1;_tmp339=
_tmp337.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp33A=_tmp338;{union Cyc_CfFlowInfo_FlowInfo
_tmp33B=_tmp338;struct _tuple13 _tmp33C;struct Cyc_Dict_Dict _tmp33D;struct Cyc_List_List*
_tmp33E;_LL1D4: if((_tmp33B.ReachableFL).tag != 2)goto _LL1D6;_tmp33C=(struct
_tuple13)(_tmp33B.ReachableFL).val;_tmp33D=_tmp33C.f1;_tmp33E=_tmp33C.f2;_LL1D5:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp33D,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp339))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp863;void*_tmp862;(_tmp862=0,Cyc_Tcutil_terr(_tmp1F7->loc,((_tmp863="expression may not be initialized",
_tag_dyneither(_tmp863,sizeof(char),34))),_tag_dyneither(_tmp862,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp341=Cyc_NewControlFlow_add_subscript_reln(_tmp173->r,
_tmp33E,_tmp1F6,_tmp1F7);if(_tmp33E != _tmp341)_tmp33A=Cyc_CfFlowInfo_ReachableFL(
_tmp33D,_tmp341);goto _LL1D3;};_LL1D6:;_LL1D7: goto _LL1D3;_LL1D3:;}{void*_tmp342=
Cyc_Tcutil_compress((void*)_check_null(_tmp1F6->topt));struct Cyc_List_List*
_tmp344;struct Cyc_Absyn_PtrInfo _tmp346;struct Cyc_Absyn_PtrAtts _tmp347;union Cyc_Absyn_Constraint*
_tmp348;_LL1D9: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp343=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)
_tmp342;if(_tmp343->tag != 10)goto _LL1DB;else{_tmp344=_tmp343->f1;}}_LL1DA: {void*
_tmp349=(void*)((struct Cyc_List_List*)_check_null(_tmp339))->hd;struct
_dyneither_ptr _tmp34B;_LL1E0: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp34A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp349;if(
_tmp34A->tag != 6)goto _LL1E2;else{_tmp34B=_tmp34A->f2;}}_LL1E1: {unsigned int i=(
Cyc_Evexp_eval_const_uint_exp(_tmp1F7)).f1;struct _tuple14 _tmp864;struct _tuple14
_tmp34D=(_tmp864.f1=_tmp33A,((_tmp864.f2=*((void**)_check_dyneither_subscript(
_tmp34B,sizeof(void*),(int)i)),_tmp864)));_npop_handler(0);return _tmp34D;}_LL1E2:;
_LL1E3: {struct Cyc_Core_Impossible_exn_struct _tmp86A;const char*_tmp869;struct Cyc_Core_Impossible_exn_struct*
_tmp868;(int)_throw((void*)((_tmp868=_cycalloc(sizeof(*_tmp868)),((_tmp868[0]=((
_tmp86A.tag=Cyc_Core_Impossible,((_tmp86A.f1=((_tmp869="anal_Rexp: Subscript",
_tag_dyneither(_tmp869,sizeof(char),21))),_tmp86A)))),_tmp868)))));}_LL1DF:;}
_LL1DB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp345=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp342;if(_tmp345->tag != 5)goto _LL1DD;else{_tmp346=_tmp345->f1;_tmp347=_tmp346.ptr_atts;
_tmp348=_tmp347.bounds;}}_LL1DC: {union Cyc_CfFlowInfo_FlowInfo _tmp352;void*
_tmp353;struct _tuple14 _tmp351=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp338,
_tmp1F6,(void*)((struct Cyc_List_List*)_check_null(_tmp339))->hd);_tmp352=_tmp351.f1;
_tmp353=_tmp351.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp354=_tmp352;int _tmp355;
_LL1E5: if((_tmp354.BottomFL).tag != 1)goto _LL1E7;_tmp355=(int)(_tmp354.BottomFL).val;
_LL1E6: {struct _tuple14 _tmp86B;struct _tuple14 _tmp357=(_tmp86B.f1=_tmp352,((
_tmp86B.f2=_tmp353,_tmp86B)));_npop_handler(0);return _tmp357;}_LL1E7:;_LL1E8: {
struct _tuple14 _tmp86C;struct _tuple14 _tmp359=(_tmp86C.f1=_tmp33A,((_tmp86C.f2=
_tmp353,_tmp86C)));_npop_handler(0);return _tmp359;}_LL1E4:;};}_LL1DD:;_LL1DE: {
struct Cyc_Core_Impossible_exn_struct _tmp872;const char*_tmp871;struct Cyc_Core_Impossible_exn_struct*
_tmp870;(int)_throw((void*)((_tmp870=_cycalloc(sizeof(*_tmp870)),((_tmp870[0]=((
_tmp872.tag=Cyc_Core_Impossible,((_tmp872.f1=((_tmp871="anal_Rexp: Subscript -- bad type",
_tag_dyneither(_tmp871,sizeof(char),33))),_tmp872)))),_tmp870)))));}_LL1D8:;};};};
_pop_region(temp);}_LL153: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*
_tmp1F8=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1F8->tag
!= 29)goto _LL155;else{_tmp1F9=_tmp1F8->f1;_tmp1FA=_tmp1F8->f2;}}_LL154: _tmp1FC=
_tmp1F9;goto _LL156;_LL155: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1FB=(
struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1FB->tag != 23)goto
_LL157;else{_tmp1FC=_tmp1FB->f1;}}_LL156: {struct _RegionHandle _tmp35D=
_new_region("temp");struct _RegionHandle*temp=& _tmp35D;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo _tmp35F;struct Cyc_List_List*_tmp360;struct _tuple20 _tmp35E=
Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp1FC,1,1);_tmp35F=
_tmp35E.f1;_tmp360=_tmp35E.f2;{unsigned int _tmp878;unsigned int _tmp877;struct
_dyneither_ptr _tmp876;void**_tmp875;unsigned int _tmp874;struct _dyneither_ptr
aggrdict=(_tmp874=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp1FC),((_tmp875=(void**)_region_malloc(env->r,_check_times(sizeof(void*),
_tmp874)),((_tmp876=_tag_dyneither(_tmp875,sizeof(void*),_tmp874),((((_tmp877=
_tmp874,_tmp879(& _tmp878,& _tmp877,& _tmp875,& _tmp360))),_tmp876)))))));struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp883;struct Cyc_CfFlowInfo_UnionRInfo _tmp882;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct
_tmp881;struct _tuple14 _tmp880;struct _tuple14 _tmp365=(_tmp880.f1=_tmp35F,((
_tmp880.f2=(void*)((_tmp883=_region_malloc(env->r,sizeof(*_tmp883)),((_tmp883[0]=((
_tmp881.tag=6,((_tmp881.f1=((_tmp882.is_union=0,((_tmp882.fieldnum=- 1,_tmp882)))),((
_tmp881.f2=aggrdict,_tmp881)))))),_tmp883)))),_tmp880)));_npop_handler(0);return
_tmp365;};};_pop_region(temp);}_LL157: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*
_tmp1FD=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp1FD->tag
!= 28)goto _LL159;else{_tmp1FE=_tmp1FD->f2;}}_LL158: {struct Cyc_List_List*fs;{
void*_tmp36B=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_List_List*
_tmp36D;_LL1EA: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp36C=(struct
Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp36B;if(_tmp36C->tag != 12)goto
_LL1EC;else{_tmp36D=_tmp36C->f2;}}_LL1EB: fs=_tmp36D;goto _LL1E9;_LL1EC:;_LL1ED: {
struct Cyc_Core_Impossible_exn_struct _tmp889;const char*_tmp888;struct Cyc_Core_Impossible_exn_struct*
_tmp887;(int)_throw((void*)((_tmp887=_cycalloc(sizeof(*_tmp887)),((_tmp887[0]=((
_tmp889.tag=Cyc_Core_Impossible,((_tmp889.f1=((_tmp888="anal_Rexp:anon struct has bad type",
_tag_dyneither(_tmp888,sizeof(char),35))),_tmp889)))),_tmp887)))));}_LL1E9:;}
_tmp200=_tmp1FE;_tmp203=Cyc_Absyn_StructA;_tmp206=fs;goto _LL15A;}_LL159: {struct
Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1FF=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)
_tmp18D;if(_tmp1FF->tag != 27)goto _LL15B;else{_tmp200=_tmp1FF->f3;_tmp201=_tmp1FF->f4;
if(_tmp201 == 0)goto _LL15B;_tmp202=*_tmp201;_tmp203=_tmp202.kind;_tmp204=_tmp202.impl;
if(_tmp204 == 0)goto _LL15B;_tmp205=*_tmp204;_tmp206=_tmp205.fields;}}_LL15A: {
struct _RegionHandle _tmp371=_new_region("temp");struct _RegionHandle*temp=& _tmp371;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo _tmp373;struct Cyc_List_List*
_tmp374;struct _tuple20 _tmp372=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple22*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple22*))Cyc_Core_snd,_tmp200),1,1);_tmp373=_tmp372.f1;_tmp374=_tmp372.f2;{
struct _dyneither_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp173,
_tmp206,_tmp203 == Cyc_Absyn_UnionA,_tmp203 == Cyc_Absyn_UnionA?_tmp173->unknown_none:
_tmp173->unknown_all);int field_no=- 1;{int i=0;for(0;_tmp374 != 0;(((_tmp374=
_tmp374->tl,_tmp200=_tmp200->tl)),++ i)){struct Cyc_List_List*ds=(*((struct
_tuple22*)((struct Cyc_List_List*)_check_null(_tmp200))->hd)).f1;for(0;ds != 0;ds=
ds->tl){void*_tmp375=(void*)ds->hd;struct _dyneither_ptr*_tmp378;_LL1EF: {struct
Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp376=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)
_tmp375;if(_tmp376->tag != 0)goto _LL1F1;}_LL1F0: {struct Cyc_Core_Impossible_exn_struct
_tmp88F;const char*_tmp88E;struct Cyc_Core_Impossible_exn_struct*_tmp88D;(int)
_throw((void*)((_tmp88D=_cycalloc(sizeof(*_tmp88D)),((_tmp88D[0]=((_tmp88F.tag=
Cyc_Core_Impossible,((_tmp88F.f1=((_tmp88E="anal_Rexp:Aggregate_e",
_tag_dyneither(_tmp88E,sizeof(char),22))),_tmp88F)))),_tmp88D)))));}_LL1F1: {
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp377=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)
_tmp375;if(_tmp377->tag != 1)goto _LL1EE;else{_tmp378=_tmp377->f1;}}_LL1F2:
field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp206,_tmp378);*((void**)
_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp374->hd;
if(_tmp203 == Cyc_Absyn_UnionA){struct Cyc_Absyn_Exp*_tmp37C=(*((struct _tuple22*)
_tmp200->hd)).f2;_tmp373=Cyc_NewControlFlow_use_Rval(env,_tmp37C->loc,_tmp373,(
void*)_tmp374->hd);}_LL1EE:;}}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp899;struct Cyc_CfFlowInfo_UnionRInfo _tmp898;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct
_tmp897;struct _tuple14 _tmp896;struct _tuple14 _tmp381=(_tmp896.f1=_tmp373,((
_tmp896.f2=(void*)((_tmp899=_region_malloc(env->r,sizeof(*_tmp899)),((_tmp899[0]=((
_tmp897.tag=6,((_tmp897.f1=((_tmp898.is_union=_tmp203 == Cyc_Absyn_UnionA,((
_tmp898.fieldnum=field_no,_tmp898)))),((_tmp897.f2=aggrdict,_tmp897)))))),
_tmp899)))),_tmp896)));_npop_handler(0);return _tmp381;};};};_pop_region(temp);}
_LL15B: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp207=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)
_tmp18D;if(_tmp207->tag != 27)goto _LL15D;}_LL15C: {struct Cyc_Core_Impossible_exn_struct
_tmp89F;const char*_tmp89E;struct Cyc_Core_Impossible_exn_struct*_tmp89D;(int)
_throw((void*)((_tmp89D=_cycalloc(sizeof(*_tmp89D)),((_tmp89D[0]=((_tmp89F.tag=
Cyc_Core_Impossible,((_tmp89F.f1=((_tmp89E="anal_Rexp:missing aggrdeclimpl",
_tag_dyneither(_tmp89E,sizeof(char),31))),_tmp89F)))),_tmp89D)))));}_LL15D: {
struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp208=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)
_tmp18D;if(_tmp208->tag != 25)goto _LL15F;else{_tmp209=_tmp208->f1;}}_LL15E: {
struct _RegionHandle _tmp385=_new_region("temp");struct _RegionHandle*temp=& _tmp385;
_push_region(temp);{struct Cyc_List_List*_tmp386=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple22*),struct Cyc_List_List*x))
Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple22*))Cyc_Core_snd,
_tmp209);union Cyc_CfFlowInfo_FlowInfo _tmp388;struct Cyc_List_List*_tmp389;struct
_tuple20 _tmp387=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp386,
1,1);_tmp388=_tmp387.f1;_tmp389=_tmp387.f2;for(0;_tmp389 != 0;(_tmp389=_tmp389->tl,
_tmp386=_tmp386->tl)){_tmp388=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp386))->hd)->loc,_tmp388,(void*)_tmp389->hd);}{
struct _tuple14 _tmp8A0;struct _tuple14 _tmp38B=(_tmp8A0.f1=_tmp388,((_tmp8A0.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp173,(void*)_check_null(e->topt),0,_tmp173->unknown_all),_tmp8A0)));
_npop_handler(0);return _tmp38B;};};_pop_region(temp);}_LL15F: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*
_tmp20A=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp18D;if(
_tmp20A->tag != 26)goto _LL161;else{_tmp20B=_tmp20A->f1;_tmp20C=_tmp20A->f2;
_tmp20D=_tmp20A->f3;_tmp20E=_tmp20A->f4;}}_LL160: {union Cyc_CfFlowInfo_FlowInfo
_tmp38D;void*_tmp38E;struct _tuple14 _tmp38C=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,_tmp20C);_tmp38D=_tmp38C.f1;_tmp38E=_tmp38C.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp38F=_tmp38D;int _tmp390;struct _tuple13 _tmp391;struct Cyc_Dict_Dict _tmp392;
struct Cyc_List_List*_tmp393;_LL1F4: if((_tmp38F.BottomFL).tag != 1)goto _LL1F6;
_tmp390=(int)(_tmp38F.BottomFL).val;_LL1F5: {struct _tuple14 _tmp8A1;return(
_tmp8A1.f1=_tmp38D,((_tmp8A1.f2=_tmp173->unknown_all,_tmp8A1)));}_LL1F6: if((
_tmp38F.ReachableFL).tag != 2)goto _LL1F3;_tmp391=(struct _tuple13)(_tmp38F.ReachableFL).val;
_tmp392=_tmp391.f1;_tmp393=_tmp391.f2;_LL1F7: if(Cyc_CfFlowInfo_initlevel(env->fenv,
_tmp392,_tmp38E)== Cyc_CfFlowInfo_NoneIL){const char*_tmp8A4;void*_tmp8A3;(
_tmp8A3=0,Cyc_Tcutil_terr(_tmp20C->loc,((_tmp8A4="expression may not be initialized",
_tag_dyneither(_tmp8A4,sizeof(char),34))),_tag_dyneither(_tmp8A3,sizeof(void*),0)));}{
struct Cyc_List_List*new_relns=_tmp393;comp_loop: {void*_tmp397=_tmp20C->r;struct
Cyc_Absyn_Exp*_tmp399;void*_tmp39B;struct Cyc_Absyn_Vardecl*_tmp39D;void*_tmp39F;
struct Cyc_Absyn_Vardecl*_tmp3A1;void*_tmp3A3;struct Cyc_Absyn_Vardecl*_tmp3A5;
void*_tmp3A7;struct Cyc_Absyn_Vardecl*_tmp3A9;union Cyc_Absyn_Cnst _tmp3AB;struct
_tuple6 _tmp3AC;int _tmp3AD;enum Cyc_Absyn_Primop _tmp3AF;struct Cyc_List_List*
_tmp3B0;struct Cyc_List_List _tmp3B1;struct Cyc_Absyn_Exp*_tmp3B2;_LL1F9: {struct
Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp398=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)
_tmp397;if(_tmp398->tag != 13)goto _LL1FB;else{_tmp399=_tmp398->f2;}}_LL1FA:
_tmp20C=_tmp399;goto comp_loop;_LL1FB:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp39A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp397;if(_tmp39A->tag != 1)
goto _LL1FD;else{_tmp39B=(void*)_tmp39A->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp39C=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp39B;if(_tmp39C->tag
!= 1)goto _LL1FD;else{_tmp39D=_tmp39C->f1;}};}}if(!(!_tmp39D->escapes))goto _LL1FD;
_LL1FC: _tmp3A1=_tmp39D;goto _LL1FE;_LL1FD:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp39E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp397;if(_tmp39E->tag != 1)
goto _LL1FF;else{_tmp39F=(void*)_tmp39E->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmp3A0=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp39F;if(_tmp3A0->tag != 
4)goto _LL1FF;else{_tmp3A1=_tmp3A0->f1;}};}}if(!(!_tmp3A1->escapes))goto _LL1FF;
_LL1FE: _tmp3A5=_tmp3A1;goto _LL200;_LL1FF:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp3A2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp397;if(_tmp3A2->tag != 1)
goto _LL201;else{_tmp3A3=(void*)_tmp3A2->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*
_tmp3A4=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp3A3;if(_tmp3A4->tag != 5)
goto _LL201;else{_tmp3A5=_tmp3A4->f1;}};}}if(!(!_tmp3A5->escapes))goto _LL201;
_LL200: _tmp3A9=_tmp3A5;goto _LL202;_LL201:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp3A6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp397;if(_tmp3A6->tag != 1)
goto _LL203;else{_tmp3A7=(void*)_tmp3A6->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*
_tmp3A8=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp3A7;if(_tmp3A8->tag != 
3)goto _LL203;else{_tmp3A9=_tmp3A8->f1;}};}}if(!(!_tmp3A9->escapes))goto _LL203;
_LL202:{struct Cyc_CfFlowInfo_Reln*_tmp8A7;struct Cyc_List_List*_tmp8A6;new_relns=((
_tmp8A6=_region_malloc(env->r,sizeof(*_tmp8A6)),((_tmp8A6->hd=((_tmp8A7=
_region_malloc(env->r,sizeof(*_tmp8A7)),((_tmp8A7->vd=_tmp20B,((_tmp8A7->rop=Cyc_CfFlowInfo_LessVar(
_tmp3A9,_tmp3A9->type),_tmp8A7)))))),((_tmp8A6->tl=_tmp393,_tmp8A6))))));}goto
_LL1F8;_LL203: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp3AA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmp397;if(_tmp3AA->tag != 0)goto _LL205;else{_tmp3AB=_tmp3AA->f1;if((_tmp3AB.Int_c).tag
!= 5)goto _LL205;_tmp3AC=(struct _tuple6)(_tmp3AB.Int_c).val;_tmp3AD=_tmp3AC.f2;}}
_LL204:{struct Cyc_CfFlowInfo_Reln*_tmp8AA;struct Cyc_List_List*_tmp8A9;new_relns=((
_tmp8A9=_region_malloc(env->r,sizeof(*_tmp8A9)),((_tmp8A9->hd=((_tmp8AA=
_region_malloc(env->r,sizeof(*_tmp8AA)),((_tmp8AA->vd=_tmp20B,((_tmp8AA->rop=Cyc_CfFlowInfo_LessConst((
unsigned int)_tmp3AD),_tmp8AA)))))),((_tmp8A9->tl=_tmp393,_tmp8A9))))));}goto
_LL1F8;_LL205: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp3AE=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)
_tmp397;if(_tmp3AE->tag != 2)goto _LL207;else{_tmp3AF=_tmp3AE->f1;_tmp3B0=_tmp3AE->f2;
if(_tmp3B0 == 0)goto _LL207;_tmp3B1=*_tmp3B0;_tmp3B2=(struct Cyc_Absyn_Exp*)_tmp3B1.hd;}}
_LL206:{void*_tmp3B7=_tmp3B2->r;void*_tmp3B9;struct Cyc_Absyn_Vardecl*_tmp3BB;
void*_tmp3BD;struct Cyc_Absyn_Vardecl*_tmp3BF;void*_tmp3C1;struct Cyc_Absyn_Vardecl*
_tmp3C3;void*_tmp3C5;struct Cyc_Absyn_Vardecl*_tmp3C7;_LL20A:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp3B8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3B7;if(_tmp3B8->tag != 1)
goto _LL20C;else{_tmp3B9=(void*)_tmp3B8->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp3BA=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp3B9;if(_tmp3BA->tag
!= 1)goto _LL20C;else{_tmp3BB=_tmp3BA->f1;}};}}if(!(!_tmp3BB->escapes))goto _LL20C;
_LL20B: _tmp3BF=_tmp3BB;goto _LL20D;_LL20C:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp3BC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3B7;if(_tmp3BC->tag != 1)
goto _LL20E;else{_tmp3BD=(void*)_tmp3BC->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmp3BE=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp3BD;if(_tmp3BE->tag != 
4)goto _LL20E;else{_tmp3BF=_tmp3BE->f1;}};}}if(!(!_tmp3BF->escapes))goto _LL20E;
_LL20D: _tmp3C3=_tmp3BF;goto _LL20F;_LL20E:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp3C0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3B7;if(_tmp3C0->tag != 1)
goto _LL210;else{_tmp3C1=(void*)_tmp3C0->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*
_tmp3C2=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp3C1;if(_tmp3C2->tag != 5)
goto _LL210;else{_tmp3C3=_tmp3C2->f1;}};}}if(!(!_tmp3C3->escapes))goto _LL210;
_LL20F: _tmp3C7=_tmp3C3;goto _LL211;_LL210:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp3C4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3B7;if(_tmp3C4->tag != 1)
goto _LL212;else{_tmp3C5=(void*)_tmp3C4->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*
_tmp3C6=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp3C5;if(_tmp3C6->tag != 
3)goto _LL212;else{_tmp3C7=_tmp3C6->f1;}};}}if(!(!_tmp3C7->escapes))goto _LL212;
_LL211:{struct Cyc_CfFlowInfo_Reln*_tmp8AD;struct Cyc_List_List*_tmp8AC;new_relns=((
_tmp8AC=_region_malloc(env->r,sizeof(*_tmp8AC)),((_tmp8AC->hd=((_tmp8AD=
_region_malloc(env->r,sizeof(*_tmp8AD)),((_tmp8AD->vd=_tmp20B,((_tmp8AD->rop=Cyc_CfFlowInfo_LessNumelts(
_tmp3C7),_tmp8AD)))))),((_tmp8AC->tl=_tmp393,_tmp8AC))))));}goto _LL209;_LL212:;
_LL213: goto _LL209;_LL209:;}goto _LL1F8;_LL207:;_LL208: goto _LL1F8;_LL1F8:;}if(
_tmp393 != new_relns)_tmp38D=Cyc_CfFlowInfo_ReachableFL(_tmp392,new_relns);{void*
_tmp3CA=_tmp38E;_LL215: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*
_tmp3CB=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp3CA;if(_tmp3CB->tag
!= 0)goto _LL217;}_LL216: {struct _tuple14 _tmp8AE;return(_tmp8AE.f1=_tmp38D,((
_tmp8AE.f2=_tmp173->unknown_all,_tmp8AE)));}_LL217: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*
_tmp3CC=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp3CA;if(
_tmp3CC->tag != 2)goto _LL219;}_LL218: goto _LL21A;_LL219: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*
_tmp3CD=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp3CA;if(
_tmp3CD->tag != 1)goto _LL21B;}_LL21A: goto _LL21C;_LL21B: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp3CE=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3CA;if(
_tmp3CE->tag != 5)goto _LL21D;}_LL21C: {struct Cyc_List_List _tmp8AF;struct Cyc_List_List
_tmp3D0=(_tmp8AF.hd=_tmp20B,((_tmp8AF.tl=0,_tmp8AF)));_tmp38D=Cyc_NewControlFlow_add_vars(
_tmp173,_tmp38D,(struct Cyc_List_List*)& _tmp3D0,_tmp173->unknown_all,e->loc);{
union Cyc_CfFlowInfo_FlowInfo _tmp3D2;void*_tmp3D3;struct _tuple14 _tmp3D1=Cyc_NewControlFlow_anal_Rexp(
env,1,_tmp38D,_tmp20D);_tmp3D2=_tmp3D1.f1;_tmp3D3=_tmp3D1.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp3D4=_tmp3D2;int _tmp3D5;struct _tuple13 _tmp3D6;struct Cyc_Dict_Dict _tmp3D7;
_LL220: if((_tmp3D4.BottomFL).tag != 1)goto _LL222;_tmp3D5=(int)(_tmp3D4.BottomFL).val;
_LL221: {struct _tuple14 _tmp8B0;return(_tmp8B0.f1=_tmp3D2,((_tmp8B0.f2=_tmp173->unknown_all,
_tmp8B0)));}_LL222: if((_tmp3D4.ReachableFL).tag != 2)goto _LL21F;_tmp3D6=(struct
_tuple13)(_tmp3D4.ReachableFL).val;_tmp3D7=_tmp3D6.f1;_LL223: if(Cyc_CfFlowInfo_initlevel(
_tmp173,_tmp3D7,_tmp3D3)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp8B3;void*_tmp8B2;(
_tmp8B2=0,Cyc_Tcutil_terr(_tmp20C->loc,((_tmp8B3="expression may not be initialized",
_tag_dyneither(_tmp8B3,sizeof(char),34))),_tag_dyneither(_tmp8B2,sizeof(void*),0)));}{
struct _tuple14 _tmp8B4;return(_tmp8B4.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8B4.f2=
_tmp173->unknown_all,_tmp8B4)));};}_LL21F:;}_tmp38D=_tmp3D2;goto _LL21E;};}_LL21D:;
_LL21E: while(1){struct Cyc_List_List _tmp8B5;struct Cyc_List_List _tmp3DD=(_tmp8B5.hd=
_tmp20B,((_tmp8B5.tl=0,_tmp8B5)));_tmp38D=Cyc_NewControlFlow_add_vars(_tmp173,
_tmp38D,(struct Cyc_List_List*)& _tmp3DD,_tmp173->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp3DF;void*_tmp3E0;struct _tuple14 _tmp3DE=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp38D,_tmp20D);_tmp3DF=_tmp3DE.f1;_tmp3E0=_tmp3DE.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp3E1=_tmp3DF;int _tmp3E2;struct _tuple13 _tmp3E3;struct Cyc_Dict_Dict _tmp3E4;
_LL225: if((_tmp3E1.BottomFL).tag != 1)goto _LL227;_tmp3E2=(int)(_tmp3E1.BottomFL).val;
_LL226: goto _LL224;_LL227: if((_tmp3E1.ReachableFL).tag != 2)goto _LL224;_tmp3E3=(
struct _tuple13)(_tmp3E1.ReachableFL).val;_tmp3E4=_tmp3E3.f1;_LL228: if(Cyc_CfFlowInfo_initlevel(
_tmp173,_tmp3E4,_tmp3E0)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp8B8;void*_tmp8B7;(
_tmp8B7=0,Cyc_Tcutil_terr(_tmp20C->loc,((_tmp8B8="expression may not be initialized",
_tag_dyneither(_tmp8B8,sizeof(char),34))),_tag_dyneither(_tmp8B7,sizeof(void*),0)));}{
struct _tuple14 _tmp8B9;return(_tmp8B9.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8B9.f2=
_tmp173->unknown_all,_tmp8B9)));};}_LL224:;}{union Cyc_CfFlowInfo_FlowInfo _tmp3E8=
Cyc_CfFlowInfo_join_flow(_tmp173,env->all_changed,_tmp38D,_tmp3DF);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp3E8,_tmp38D))break;_tmp38D=_tmp3E8;};};}{struct _tuple14 _tmp8BA;return(
_tmp8BA.f1=_tmp38D,((_tmp8BA.f2=_tmp173->unknown_all,_tmp8BA)));};_LL214:;};};
_LL1F3:;};}_LL161: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp20F=(
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp20F->tag != 35)
goto _LL163;else{_tmp210=_tmp20F->f1;}}_LL162: return Cyc_NewControlFlow_anal_stmt_exp(
env,copy_ctxt,inflow,_tmp210);_LL163: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp211=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp211->tag != 1)
goto _LL165;else{_tmp212=(void*)_tmp211->f2;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*
_tmp213=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp212;if(_tmp213->tag
!= 0)goto _LL165;};}}_LL164: goto _LL166;_LL165: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*
_tmp214=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp18D;if(
_tmp214->tag != 34)goto _LL167;}_LL166: goto _LL168;_LL167: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*
_tmp215=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp215->tag
!= 24)goto _LL169;}_LL168: goto _LL16A;_LL169: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*
_tmp216=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp216->tag
!= 37)goto _LL16B;}_LL16A: goto _LL16C;_LL16B: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*
_tmp217=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp18D;if(_tmp217->tag != 
38)goto _LL106;}_LL16C: {struct Cyc_Core_Impossible_exn_struct _tmp8C0;const char*
_tmp8BF;struct Cyc_Core_Impossible_exn_struct*_tmp8BE;(int)_throw((void*)((
_tmp8BE=_cycalloc(sizeof(*_tmp8BE)),((_tmp8BE[0]=((_tmp8C0.tag=Cyc_Core_Impossible,((
_tmp8C0.f1=((_tmp8BF="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp8BF,
sizeof(char),31))),_tmp8C0)))),_tmp8BE)))));}_LL106:;};}static struct _tuple15 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,struct Cyc_List_List*flds){
struct Cyc_CfFlowInfo_FlowEnv*_tmp3EE=env->fenv;void*_tmp3EF=Cyc_Tcutil_compress((
void*)_check_null(e->topt));struct Cyc_Absyn_PtrInfo _tmp3F1;void*_tmp3F2;struct
Cyc_Absyn_PtrAtts _tmp3F3;union Cyc_Absyn_Constraint*_tmp3F4;union Cyc_Absyn_Constraint*
_tmp3F5;_LL22A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F0=(struct
Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EF;if(_tmp3F0->tag != 5)goto _LL22C;
else{_tmp3F1=_tmp3F0->f1;_tmp3F2=_tmp3F1.elt_typ;_tmp3F3=_tmp3F1.ptr_atts;
_tmp3F4=_tmp3F3.bounds;_tmp3F5=_tmp3F3.zero_term;}}_LL22B: {union Cyc_CfFlowInfo_FlowInfo
_tmp3F6=f;int _tmp3F7;struct _tuple13 _tmp3F8;struct Cyc_Dict_Dict _tmp3F9;struct Cyc_List_List*
_tmp3FA;_LL22F: if((_tmp3F6.BottomFL).tag != 1)goto _LL231;_tmp3F7=(int)(_tmp3F6.BottomFL).val;
_LL230: {struct _tuple15 _tmp8C1;return(_tmp8C1.f1=f,((_tmp8C1.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8C1)));}_LL231: if((_tmp3F6.ReachableFL).tag != 2)goto _LL22E;_tmp3F8=(struct
_tuple13)(_tmp3F6.ReachableFL).val;_tmp3F9=_tmp3F8.f1;_tmp3FA=_tmp3F8.f2;_LL232:
if(Cyc_Tcutil_is_bound_one(_tmp3F4)){void*_tmp3FC=r;struct Cyc_CfFlowInfo_Place*
_tmp400;struct Cyc_CfFlowInfo_Place _tmp401;void*_tmp402;struct Cyc_List_List*
_tmp403;enum Cyc_CfFlowInfo_InitLevel _tmp406;_LL234: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*
_tmp3FD=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp3FC;if(
_tmp3FD->tag != 1)goto _LL236;}_LL235: goto _LL237;_LL236: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*
_tmp3FE=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp3FC;if(
_tmp3FE->tag != 2)goto _LL238;}_LL237:{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct
_tmp8C4;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp8C3;e->annot=(
void*)((_tmp8C3=_cycalloc(sizeof(*_tmp8C3)),((_tmp8C3[0]=((_tmp8C4.tag=Cyc_CfFlowInfo_NotZero,((
_tmp8C4.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3FA),_tmp8C4)))),
_tmp8C3))));}goto _LL233;_LL238: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp3FF=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3FC;if(
_tmp3FF->tag != 5)goto _LL23A;else{_tmp400=_tmp3FF->f1;_tmp401=*_tmp400;_tmp402=
_tmp401.root;_tmp403=_tmp401.fields;}}_LL239:{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct
_tmp8C7;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp8C6;e->annot=(
void*)((_tmp8C6=_cycalloc(sizeof(*_tmp8C6)),((_tmp8C6[0]=((_tmp8C7.tag=Cyc_CfFlowInfo_NotZero,((
_tmp8C7.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3FA),_tmp8C7)))),
_tmp8C6))));}{struct Cyc_CfFlowInfo_Place*_tmp8CA;struct _tuple15 _tmp8C9;return(
_tmp8C9.f1=f,((_tmp8C9.f2=Cyc_CfFlowInfo_PlaceL(((_tmp8CA=_region_malloc(_tmp3EE->r,
sizeof(*_tmp8CA)),((_tmp8CA->root=_tmp402,((_tmp8CA->fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp3EE->r,_tmp403,flds),_tmp8CA))))))),_tmp8C9)));};_LL23A: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*
_tmp404=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp3FC;if(_tmp404->tag
!= 0)goto _LL23C;}_LL23B: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{struct
_tuple15 _tmp8CB;return(_tmp8CB.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8CB.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8CB)));};_LL23C: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*
_tmp405=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3FC;if(
_tmp405->tag != 3)goto _LL23E;else{_tmp406=_tmp405->f1;}}_LL23D: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmp406);goto _LL23F;_LL23E:;_LL23F: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct
_tmp8CE;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp8CD;e->annot=(
void*)((_tmp8CD=_cycalloc(sizeof(*_tmp8CD)),((_tmp8CD[0]=((_tmp8CE.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp8CE.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3FA),_tmp8CE)))),
_tmp8CD))));}_LL233:;}else{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct
_tmp8D1;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp8D0;e->annot=(
void*)((_tmp8D0=_cycalloc(sizeof(*_tmp8D0)),((_tmp8D0[0]=((_tmp8D1.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp8D1.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3FA),_tmp8D1)))),
_tmp8D0))));}if(Cyc_CfFlowInfo_initlevel(_tmp3EE,_tmp3F9,r)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8D4;void*_tmp8D3;(_tmp8D3=0,Cyc_Tcutil_terr(e->loc,((_tmp8D4="dereference of possibly uninitialized pointer",
_tag_dyneither(_tmp8D4,sizeof(char),46))),_tag_dyneither(_tmp8D3,sizeof(void*),0)));}{
struct _tuple15 _tmp8D5;return(_tmp8D5.f1=f,((_tmp8D5.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8D5)));};_LL22E:;}_LL22C:;_LL22D: {struct Cyc_Core_Impossible_exn_struct
_tmp8DB;const char*_tmp8DA;struct Cyc_Core_Impossible_exn_struct*_tmp8D9;(int)
_throw((void*)((_tmp8D9=_cycalloc(sizeof(*_tmp8D9)),((_tmp8D9[0]=((_tmp8DB.tag=
Cyc_Core_Impossible,((_tmp8DB.f1=((_tmp8DA="left deref of non-pointer-type",
_tag_dyneither(_tmp8DA,sizeof(char),31))),_tmp8DB)))),_tmp8D9)))));}_LL229:;}
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*
_tmp418=env->fenv;{union Cyc_CfFlowInfo_FlowInfo _tmp419=inflow;int _tmp41A;struct
_tuple13 _tmp41B;struct Cyc_Dict_Dict _tmp41C;struct Cyc_List_List*_tmp41D;_LL241:
if((_tmp419.BottomFL).tag != 1)goto _LL243;_tmp41A=(int)(_tmp419.BottomFL).val;
_LL242: {struct _tuple15 _tmp8DC;return(_tmp8DC.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp8DC.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8DC)));}_LL243: if((_tmp419.ReachableFL).tag
!= 2)goto _LL240;_tmp41B=(struct _tuple13)(_tmp419.ReachableFL).val;_tmp41C=
_tmp41B.f1;_tmp41D=_tmp41B.f2;_LL244: d=_tmp41C;_LL240:;}{void*_tmp41F=e->r;
struct Cyc_Absyn_Exp*_tmp421;struct Cyc_Absyn_Exp*_tmp423;struct Cyc_Absyn_Exp*
_tmp425;void*_tmp427;struct Cyc_Absyn_Vardecl*_tmp429;void*_tmp42B;struct Cyc_Absyn_Vardecl*
_tmp42D;void*_tmp42F;struct Cyc_Absyn_Vardecl*_tmp431;void*_tmp433;struct Cyc_Absyn_Vardecl*
_tmp435;struct Cyc_Absyn_Exp*_tmp437;struct _dyneither_ptr*_tmp438;struct Cyc_Absyn_Exp*
_tmp43A;struct Cyc_Absyn_Exp*_tmp43C;struct Cyc_Absyn_Exp*_tmp43D;struct Cyc_Absyn_Exp*
_tmp43F;struct _dyneither_ptr*_tmp440;_LL246: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*
_tmp420=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp41F;if(_tmp420->tag != 
13)goto _LL248;else{_tmp421=_tmp420->f2;}}_LL247: _tmp423=_tmp421;goto _LL249;
_LL248: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp422=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)
_tmp41F;if(_tmp422->tag != 11)goto _LL24A;else{_tmp423=_tmp422->f1;}}_LL249:
_tmp425=_tmp423;goto _LL24B;_LL24A: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*
_tmp424=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp41F;if(_tmp424->tag
!= 12)goto _LL24C;else{_tmp425=_tmp424->f1;}}_LL24B: return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,expand_unique,_tmp425,flds);_LL24C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp426=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp41F;if(_tmp426->tag != 1)
goto _LL24E;else{_tmp427=(void*)_tmp426->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*
_tmp428=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp427;if(_tmp428->tag != 
3)goto _LL24E;else{_tmp429=_tmp428->f1;}};}}_LL24D: _tmp42D=_tmp429;goto _LL24F;
_LL24E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp42A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp41F;if(_tmp42A->tag != 1)goto _LL250;else{_tmp42B=(void*)_tmp42A->f2;{struct
Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp42C=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)
_tmp42B;if(_tmp42C->tag != 4)goto _LL250;else{_tmp42D=_tmp42C->f1;}};}}_LL24F:
_tmp431=_tmp42D;goto _LL251;_LL250: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp42E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp41F;if(_tmp42E->tag != 1)
goto _LL252;else{_tmp42F=(void*)_tmp42E->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*
_tmp430=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp42F;if(_tmp430->tag != 5)
goto _LL252;else{_tmp431=_tmp430->f1;}};}}_LL251: {struct Cyc_CfFlowInfo_Place*
_tmp8E6;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8E5;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*
_tmp8E4;struct _tuple15 _tmp8E3;return(_tmp8E3.f1=inflow,((_tmp8E3.f2=Cyc_CfFlowInfo_PlaceL(((
_tmp8E6=_region_malloc(env->r,sizeof(*_tmp8E6)),((_tmp8E6->root=(void*)((_tmp8E4=
_region_malloc(env->r,sizeof(*_tmp8E4)),((_tmp8E4[0]=((_tmp8E5.tag=0,((_tmp8E5.f1=
_tmp431,_tmp8E5)))),_tmp8E4)))),((_tmp8E6->fields=flds,_tmp8E6))))))),_tmp8E3)));}
_LL252: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp432=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp41F;if(_tmp432->tag != 1)goto _LL254;else{_tmp433=(void*)_tmp432->f2;{struct
Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp434=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)
_tmp433;if(_tmp434->tag != 1)goto _LL254;else{_tmp435=_tmp434->f1;}};}}_LL253: {
struct _tuple15 _tmp8E7;return(_tmp8E7.f1=inflow,((_tmp8E7.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8E7)));}_LL254: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp436=(
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp41F;if(_tmp436->tag != 21)
goto _LL256;else{_tmp437=_tmp436->f1;_tmp438=_tmp436->f2;}}_LL255:{void*_tmp446=
Cyc_Tcutil_compress((void*)_check_null(_tmp437->topt));struct Cyc_Absyn_PtrInfo
_tmp448;void*_tmp449;_LL25F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp447=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp446;if(_tmp447->tag
!= 5)goto _LL261;else{_tmp448=_tmp447->f1;_tmp449=_tmp448.elt_typ;}}_LL260: if(!
Cyc_Absyn_is_nontagged_union_type(_tmp449)){struct Cyc_List_List*_tmp8E8;flds=((
_tmp8E8=_region_malloc(env->r,sizeof(*_tmp8E8)),((_tmp8E8->hd=(void*)Cyc_CfFlowInfo_get_field_index(
_tmp449,_tmp438),((_tmp8E8->tl=flds,_tmp8E8))))));}goto _LL25E;_LL261:;_LL262: {
struct Cyc_Core_Impossible_exn_struct _tmp8EE;const char*_tmp8ED;struct Cyc_Core_Impossible_exn_struct*
_tmp8EC;(int)_throw((void*)((_tmp8EC=_cycalloc(sizeof(*_tmp8EC)),((_tmp8EC[0]=((
_tmp8EE.tag=Cyc_Core_Impossible,((_tmp8EE.f1=((_tmp8ED="anal_Rexp: AggrArrow ptr",
_tag_dyneither(_tmp8ED,sizeof(char),25))),_tmp8EE)))),_tmp8EC)))));}_LL25E:;}
_tmp43A=_tmp437;goto _LL257;_LL256: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*
_tmp439=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp41F;if(_tmp439->tag != 
19)goto _LL258;else{_tmp43A=_tmp439->f1;}}_LL257: if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((
void*)_check_null(_tmp43A->topt))){union Cyc_CfFlowInfo_FlowInfo _tmp44F;union Cyc_CfFlowInfo_AbsLVal
_tmp450;struct _tuple15 _tmp44E=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp43A);
_tmp44F=_tmp44E.f1;_tmp450=_tmp44E.f2;{struct _tuple15 _tmp8EF;struct _tuple15
_tmp452=(_tmp8EF.f1=_tmp44F,((_tmp8EF.f2=_tmp450,_tmp8EF)));union Cyc_CfFlowInfo_FlowInfo
_tmp453;struct _tuple13 _tmp454;struct Cyc_Dict_Dict _tmp455;struct Cyc_List_List*
_tmp456;union Cyc_CfFlowInfo_AbsLVal _tmp457;struct Cyc_CfFlowInfo_Place*_tmp458;
_LL264: _tmp453=_tmp452.f1;if((_tmp453.ReachableFL).tag != 2)goto _LL266;_tmp454=(
struct _tuple13)(_tmp453.ReachableFL).val;_tmp455=_tmp454.f1;_tmp456=_tmp454.f2;
_tmp457=_tmp452.f2;if((_tmp457.PlaceL).tag != 1)goto _LL266;_tmp458=(struct Cyc_CfFlowInfo_Place*)(
_tmp457.PlaceL).val;_LL265: {void*_tmp459=Cyc_CfFlowInfo_lookup_place(_tmp455,
_tmp458);void*_tmp45A=_tmp459;_LL269: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp45B=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp45A;if(
_tmp45B->tag != 5)goto _LL26B;}_LL26A: return Cyc_NewControlFlow_anal_derefL(env,
_tmp44F,_tmp43A,_tmp44F,_tmp459,flds);_LL26B:;_LL26C: {enum Cyc_CfFlowInfo_InitLevel
il=Cyc_CfFlowInfo_initlevel(_tmp418,_tmp455,_tmp459);void*leaf=il == Cyc_CfFlowInfo_AllIL?
_tmp418->unknown_all: _tmp418->unknown_none;void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(
_tmp418,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp43A->topt)),0,leaf);
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp8F2;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*
_tmp8F1;void*new_root=(void*)((_tmp8F1=_region_malloc(_tmp418->r,sizeof(*_tmp8F1)),((
_tmp8F1[0]=((_tmp8F2.tag=1,((_tmp8F2.f1=e,((_tmp8F2.f2=(void*)((void*)
_check_null(e->topt)),_tmp8F2)))))),_tmp8F1))));struct Cyc_CfFlowInfo_Place*
_tmp8F3;struct Cyc_CfFlowInfo_Place*place=(_tmp8F3=_region_malloc(_tmp418->r,
sizeof(*_tmp8F3)),((_tmp8F3->root=new_root,((_tmp8F3->fields=0,_tmp8F3)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8F6;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp8F5;void*res=(void*)((_tmp8F5=_region_malloc(_tmp418->r,sizeof(*_tmp8F5)),((
_tmp8F5[0]=((_tmp8F6.tag=5,((_tmp8F6.f1=place,_tmp8F6)))),_tmp8F5))));((int(*)(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);_tmp455=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(_tmp455,new_root,new_rval);_tmp455=Cyc_CfFlowInfo_assign_place(
_tmp418,e->loc,_tmp455,env->all_changed,_tmp458,res);{union Cyc_CfFlowInfo_FlowInfo
_tmp45C=Cyc_CfFlowInfo_ReachableFL(_tmp455,_tmp456);return Cyc_NewControlFlow_anal_derefL(
env,_tmp45C,_tmp43A,_tmp45C,res,flds);};}_LL268:;}_LL266:;_LL267: goto _LL263;
_LL263:;};}{union Cyc_CfFlowInfo_FlowInfo _tmp463;void*_tmp464;struct _tuple14
_tmp462=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp43A);_tmp463=_tmp462.f1;
_tmp464=_tmp462.f2;return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp43A,
_tmp463,_tmp464,flds);};_LL258: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*
_tmp43B=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp41F;if(_tmp43B->tag
!= 22)goto _LL25A;else{_tmp43C=_tmp43B->f1;_tmp43D=_tmp43B->f2;}}_LL259: {void*
_tmp465=Cyc_Tcutil_compress((void*)_check_null(_tmp43C->topt));struct Cyc_Absyn_PtrInfo
_tmp468;struct Cyc_Absyn_PtrAtts _tmp469;union Cyc_Absyn_Constraint*_tmp46A;_LL26E: {
struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp466=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)
_tmp465;if(_tmp466->tag != 10)goto _LL270;}_LL26F: {unsigned int _tmp46B=(Cyc_Evexp_eval_const_uint_exp(
_tmp43D)).f1;struct Cyc_List_List*_tmp8F7;return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,expand_unique,_tmp43C,((_tmp8F7=_region_malloc(env->r,sizeof(*_tmp8F7)),((
_tmp8F7->hd=(void*)((int)_tmp46B),((_tmp8F7->tl=flds,_tmp8F7)))))));}_LL270: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp467=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp465;if(_tmp467->tag != 5)goto _LL272;else{_tmp468=_tmp467->f1;_tmp469=_tmp468.ptr_atts;
_tmp46A=_tmp469.bounds;}}_LL271: {struct _RegionHandle _tmp46D=_new_region("temp");
struct _RegionHandle*temp=& _tmp46D;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo
_tmp470;struct Cyc_List_List*_tmp471;struct Cyc_Absyn_Exp*_tmp8F8[2];struct
_tuple20 _tmp46F=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((
_tmp8F8[1]=_tmp43D,((_tmp8F8[0]=_tmp43C,((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp8F8,
sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp470=_tmp46F.f1;_tmp471=_tmp46F.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp472=_tmp470;{union Cyc_CfFlowInfo_FlowInfo _tmp473=
_tmp470;struct _tuple13 _tmp474;struct Cyc_Dict_Dict _tmp475;struct Cyc_List_List*
_tmp476;_LL275: if((_tmp473.ReachableFL).tag != 2)goto _LL277;_tmp474=(struct
_tuple13)(_tmp473.ReachableFL).val;_tmp475=_tmp474.f1;_tmp476=_tmp474.f2;_LL276:
if(Cyc_CfFlowInfo_initlevel(_tmp418,_tmp475,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp471))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8FB;void*_tmp8FA;(_tmp8FA=0,Cyc_Tcutil_terr(_tmp43D->loc,((_tmp8FB="expression may not be initialized",
_tag_dyneither(_tmp8FB,sizeof(char),34))),_tag_dyneither(_tmp8FA,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp479=Cyc_NewControlFlow_add_subscript_reln(_tmp418->r,
_tmp476,_tmp43C,_tmp43D);if(_tmp476 != _tmp479)_tmp472=Cyc_CfFlowInfo_ReachableFL(
_tmp475,_tmp479);goto _LL274;};_LL277:;_LL278: goto _LL274;_LL274:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp47B;union Cyc_CfFlowInfo_AbsLVal _tmp47C;struct _tuple15 _tmp47A=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp43C,_tmp470,(void*)((struct Cyc_List_List*)_check_null(_tmp471))->hd,
flds);_tmp47B=_tmp47A.f1;_tmp47C=_tmp47A.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp47D=
_tmp47B;int _tmp47E;_LL27A: if((_tmp47D.BottomFL).tag != 1)goto _LL27C;_tmp47E=(int)(
_tmp47D.BottomFL).val;_LL27B: {struct _tuple15 _tmp8FC;struct _tuple15 _tmp480=(
_tmp8FC.f1=_tmp47B,((_tmp8FC.f2=_tmp47C,_tmp8FC)));_npop_handler(0);return
_tmp480;}_LL27C:;_LL27D: {struct _tuple15 _tmp8FD;struct _tuple15 _tmp482=(_tmp8FD.f1=
_tmp472,((_tmp8FD.f2=_tmp47C,_tmp8FD)));_npop_handler(0);return _tmp482;}_LL279:;};};};};
_pop_region(temp);}_LL272:;_LL273: {struct Cyc_Core_Impossible_exn_struct _tmp903;
const char*_tmp902;struct Cyc_Core_Impossible_exn_struct*_tmp901;(int)_throw((void*)((
_tmp901=_cycalloc(sizeof(*_tmp901)),((_tmp901[0]=((_tmp903.tag=Cyc_Core_Impossible,((
_tmp903.f1=((_tmp902="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp902,
sizeof(char),33))),_tmp903)))),_tmp901)))));}_LL26D:;}_LL25A: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*
_tmp43E=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp41F;if(_tmp43E->tag
!= 20)goto _LL25C;else{_tmp43F=_tmp43E->f1;_tmp440=_tmp43E->f2;}}_LL25B: if(Cyc_Absyn_is_nontagged_union_type((
void*)_check_null(_tmp43F->topt))){struct _tuple15 _tmp904;return(_tmp904.f1=
inflow,((_tmp904.f2=Cyc_CfFlowInfo_UnknownL(),_tmp904)));}{struct Cyc_List_List*
_tmp905;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp43F,((
_tmp905=_region_malloc(env->r,sizeof(*_tmp905)),((_tmp905->hd=(void*)Cyc_CfFlowInfo_get_field_index((
void*)_check_null(_tmp43F->topt),_tmp440),((_tmp905->tl=flds,_tmp905)))))));};
_LL25C:;_LL25D: {struct _tuple15 _tmp906;return(_tmp906.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp906.f2=Cyc_CfFlowInfo_UnknownL(),_tmp906)));}_LL245:;};}static struct _tuple15
Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,int expand_unique,struct Cyc_Absyn_Exp*e){union Cyc_CfFlowInfo_FlowInfo
_tmp48A;union Cyc_CfFlowInfo_AbsLVal _tmp48B;struct _tuple15 _tmp489=Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,expand_unique,e,0);_tmp48A=_tmp489.f1;_tmp48B=_tmp489.f2;{struct
_tuple15 _tmp907;return(_tmp907.f1=_tmp48A,((_tmp907.f2=_tmp48B,_tmp907)));};}
static struct _tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp48D=env->fenv;{void*_tmp48E=e->r;struct Cyc_Absyn_Exp*_tmp490;struct Cyc_Absyn_Exp*
_tmp491;struct Cyc_Absyn_Exp*_tmp492;struct Cyc_Absyn_Exp*_tmp494;struct Cyc_Absyn_Exp*
_tmp495;struct Cyc_Absyn_Exp*_tmp497;struct Cyc_Absyn_Exp*_tmp498;struct Cyc_Absyn_Exp*
_tmp49A;struct Cyc_Absyn_Exp*_tmp49B;enum Cyc_Absyn_Primop _tmp49D;struct Cyc_List_List*
_tmp49E;struct Cyc_List_List _tmp49F;struct Cyc_Absyn_Exp*_tmp4A0;struct Cyc_List_List*
_tmp4A1;enum Cyc_Absyn_Primop _tmp4A3;struct Cyc_List_List*_tmp4A4;_LL27F: {struct
Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp48F=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)
_tmp48E;if(_tmp48F->tag != 5)goto _LL281;else{_tmp490=_tmp48F->f1;_tmp491=_tmp48F->f2;
_tmp492=_tmp48F->f3;}}_LL280: {union Cyc_CfFlowInfo_FlowInfo _tmp4A6;union Cyc_CfFlowInfo_FlowInfo
_tmp4A7;struct _tuple16 _tmp4A5=Cyc_NewControlFlow_anal_test(env,inflow,_tmp490);
_tmp4A6=_tmp4A5.f1;_tmp4A7=_tmp4A5.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp4A9;union
Cyc_CfFlowInfo_FlowInfo _tmp4AA;struct _tuple16 _tmp4A8=Cyc_NewControlFlow_anal_test(
env,_tmp4A6,_tmp491);_tmp4A9=_tmp4A8.f1;_tmp4AA=_tmp4A8.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp4AC;union Cyc_CfFlowInfo_FlowInfo _tmp4AD;struct _tuple16 _tmp4AB=Cyc_NewControlFlow_anal_test(
env,_tmp4A7,_tmp492);_tmp4AC=_tmp4AB.f1;_tmp4AD=_tmp4AB.f2;{struct _tuple16
_tmp908;return(_tmp908.f1=Cyc_CfFlowInfo_join_flow(_tmp48D,env->all_changed,
_tmp4A9,_tmp4AC),((_tmp908.f2=Cyc_CfFlowInfo_join_flow(_tmp48D,env->all_changed,
_tmp4AA,_tmp4AD),_tmp908)));};};};}_LL281: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*
_tmp493=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp48E;if(_tmp493->tag != 6)
goto _LL283;else{_tmp494=_tmp493->f1;_tmp495=_tmp493->f2;}}_LL282: {union Cyc_CfFlowInfo_FlowInfo
_tmp4B0;union Cyc_CfFlowInfo_FlowInfo _tmp4B1;struct _tuple16 _tmp4AF=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp494);_tmp4B0=_tmp4AF.f1;_tmp4B1=_tmp4AF.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp4B3;union Cyc_CfFlowInfo_FlowInfo _tmp4B4;struct _tuple16 _tmp4B2=Cyc_NewControlFlow_anal_test(
env,_tmp4B0,_tmp495);_tmp4B3=_tmp4B2.f1;_tmp4B4=_tmp4B2.f2;{struct _tuple16
_tmp909;return(_tmp909.f1=_tmp4B3,((_tmp909.f2=Cyc_CfFlowInfo_join_flow(_tmp48D,
env->all_changed,_tmp4B1,_tmp4B4),_tmp909)));};};}_LL283: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*
_tmp496=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp48E;if(_tmp496->tag != 7)
goto _LL285;else{_tmp497=_tmp496->f1;_tmp498=_tmp496->f2;}}_LL284: {union Cyc_CfFlowInfo_FlowInfo
_tmp4B7;union Cyc_CfFlowInfo_FlowInfo _tmp4B8;struct _tuple16 _tmp4B6=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp497);_tmp4B7=_tmp4B6.f1;_tmp4B8=_tmp4B6.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp4BA;union Cyc_CfFlowInfo_FlowInfo _tmp4BB;struct _tuple16 _tmp4B9=Cyc_NewControlFlow_anal_test(
env,_tmp4B8,_tmp498);_tmp4BA=_tmp4B9.f1;_tmp4BB=_tmp4B9.f2;{struct _tuple16
_tmp90A;return(_tmp90A.f1=Cyc_CfFlowInfo_join_flow(_tmp48D,env->all_changed,
_tmp4B7,_tmp4BA),((_tmp90A.f2=_tmp4BB,_tmp90A)));};};}_LL285: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*
_tmp499=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp48E;if(_tmp499->tag
!= 8)goto _LL287;else{_tmp49A=_tmp499->f1;_tmp49B=_tmp499->f2;}}_LL286: {union Cyc_CfFlowInfo_FlowInfo
_tmp4BE;void*_tmp4BF;struct _tuple14 _tmp4BD=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp49A);_tmp4BE=_tmp4BD.f1;_tmp4BF=_tmp4BD.f2;return Cyc_NewControlFlow_anal_test(
env,_tmp4BE,_tmp49B);}_LL287: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*
_tmp49C=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp48E;if(_tmp49C->tag
!= 2)goto _LL289;else{_tmp49D=_tmp49C->f1;if(_tmp49D != Cyc_Absyn_Not)goto _LL289;
_tmp49E=_tmp49C->f2;if(_tmp49E == 0)goto _LL289;_tmp49F=*_tmp49E;_tmp4A0=(struct
Cyc_Absyn_Exp*)_tmp49F.hd;_tmp4A1=_tmp49F.tl;if(_tmp4A1 != 0)goto _LL289;}}_LL288: {
union Cyc_CfFlowInfo_FlowInfo _tmp4C1;union Cyc_CfFlowInfo_FlowInfo _tmp4C2;struct
_tuple16 _tmp4C0=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4A0);_tmp4C1=_tmp4C0.f1;
_tmp4C2=_tmp4C0.f2;{struct _tuple16 _tmp90B;return(_tmp90B.f1=_tmp4C2,((_tmp90B.f2=
_tmp4C1,_tmp90B)));};}_LL289: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*
_tmp4A2=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp48E;if(_tmp4A2->tag
!= 2)goto _LL28B;else{_tmp4A3=_tmp4A2->f1;_tmp4A4=_tmp4A2->f2;}}_LL28A: {void*r1;
void*r2;union Cyc_CfFlowInfo_FlowInfo f;struct _RegionHandle _tmp4C4=_new_region("temp");
struct _RegionHandle*temp=& _tmp4C4;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo
_tmp4C6;struct Cyc_List_List*_tmp4C7;struct _tuple20 _tmp4C5=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp4A4,0,0);_tmp4C6=_tmp4C5.f1;_tmp4C7=_tmp4C5.f2;r1=(void*)((
struct Cyc_List_List*)_check_null(_tmp4C7))->hd;r2=(void*)((struct Cyc_List_List*)
_check_null(_tmp4C7->tl))->hd;f=_tmp4C6;}{union Cyc_CfFlowInfo_FlowInfo _tmp4C8=f;
int _tmp4C9;struct _tuple13 _tmp4CA;struct Cyc_Dict_Dict _tmp4CB;struct Cyc_List_List*
_tmp4CC;_LL28E: if((_tmp4C8.BottomFL).tag != 1)goto _LL290;_tmp4C9=(int)(_tmp4C8.BottomFL).val;
_LL28F: {struct _tuple16 _tmp90C;struct _tuple16 _tmp4CE=(_tmp90C.f1=f,((_tmp90C.f2=
f,_tmp90C)));_npop_handler(0);return _tmp4CE;}_LL290: if((_tmp4C8.ReachableFL).tag
!= 2)goto _LL28D;_tmp4CA=(struct _tuple13)(_tmp4C8.ReachableFL).val;_tmp4CB=
_tmp4CA.f1;_tmp4CC=_tmp4CA.f2;_LL291: {struct Cyc_Absyn_Exp*_tmp4CF=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4A4))->hd;struct Cyc_Absyn_Exp*_tmp4D0=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4A4->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp4CB,r1)== Cyc_CfFlowInfo_NoneIL){const char*_tmp90F;void*_tmp90E;(
_tmp90E=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp4A4->hd)->loc,((_tmp90F="expression may not be initialized",
_tag_dyneither(_tmp90F,sizeof(char),34))),_tag_dyneither(_tmp90E,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4CB,r2)== Cyc_CfFlowInfo_NoneIL){const
char*_tmp912;void*_tmp911;(_tmp911=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4A4->tl))->hd)->loc,((_tmp912="expression may not be initialized",
_tag_dyneither(_tmp912,sizeof(char),34))),_tag_dyneither(_tmp911,sizeof(void*),0)));}
if(_tmp4A3 == Cyc_Absyn_Eq  || _tmp4A3 == Cyc_Absyn_Neq){struct _tuple0 _tmp913;
struct _tuple0 _tmp4D6=(_tmp913.f1=r1,((_tmp913.f2=r2,_tmp913)));void*_tmp4D7;
enum Cyc_CfFlowInfo_InitLevel _tmp4D9;void*_tmp4DA;void*_tmp4DC;void*_tmp4DE;
enum Cyc_CfFlowInfo_InitLevel _tmp4E0;void*_tmp4E1;void*_tmp4E3;void*_tmp4E5;void*
_tmp4E7;void*_tmp4E9;void*_tmp4EB;void*_tmp4ED;void*_tmp4EF;void*_tmp4F1;void*
_tmp4F3;void*_tmp4F5;void*_tmp4F7;void*_tmp4F9;void*_tmp4FB;_LL293: _tmp4D7=
_tmp4D6.f1;{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4D8=(
struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4D7;if(_tmp4D8->tag
!= 3)goto _LL295;else{_tmp4D9=_tmp4D8->f1;}};_tmp4DA=_tmp4D6.f2;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*
_tmp4DB=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4DA;if(_tmp4DB->tag
!= 0)goto _LL295;};_LL294: {union Cyc_CfFlowInfo_FlowInfo _tmp4FE;union Cyc_CfFlowInfo_FlowInfo
_tmp4FF;struct _tuple16 _tmp4FD=Cyc_NewControlFlow_splitzero(env,f,f,_tmp4CF,
_tmp4D9);_tmp4FE=_tmp4FD.f1;_tmp4FF=_tmp4FD.f2;switch(_tmp4A3){case Cyc_Absyn_Eq:
_LL2A7: {struct _tuple16 _tmp914;struct _tuple16 _tmp501=(_tmp914.f1=_tmp4FF,((
_tmp914.f2=_tmp4FE,_tmp914)));_npop_handler(0);return _tmp501;}case Cyc_Absyn_Neq:
_LL2A8: {struct _tuple16 _tmp915;struct _tuple16 _tmp503=(_tmp915.f1=_tmp4FE,((
_tmp915.f2=_tmp4FF,_tmp915)));_npop_handler(0);return _tmp503;}default: _LL2A9: {
struct Cyc_Core_Impossible_exn_struct _tmp91B;const char*_tmp91A;struct Cyc_Core_Impossible_exn_struct*
_tmp919;(int)_throw((void*)((_tmp919=_cycalloc(sizeof(*_tmp919)),((_tmp919[0]=((
_tmp91B.tag=Cyc_Core_Impossible,((_tmp91B.f1=((_tmp91A="anal_test, zero-split",
_tag_dyneither(_tmp91A,sizeof(char),22))),_tmp91B)))),_tmp919)))));}}}_LL295:
_tmp4DC=_tmp4D6.f1;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4DD=(
struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4DC;if(_tmp4DD->tag != 0)
goto _LL297;};_tmp4DE=_tmp4D6.f2;{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*
_tmp4DF=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4DE;if(
_tmp4DF->tag != 3)goto _LL297;else{_tmp4E0=_tmp4DF->f1;}};_LL296: {union Cyc_CfFlowInfo_FlowInfo
_tmp508;union Cyc_CfFlowInfo_FlowInfo _tmp509;struct _tuple16 _tmp507=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4D0,_tmp4E0);_tmp508=_tmp507.f1;_tmp509=_tmp507.f2;switch(_tmp4A3){
case Cyc_Absyn_Eq: _LL2AB: {struct _tuple16 _tmp91C;struct _tuple16 _tmp50B=(_tmp91C.f1=
_tmp509,((_tmp91C.f2=_tmp508,_tmp91C)));_npop_handler(0);return _tmp50B;}case Cyc_Absyn_Neq:
_LL2AC: {struct _tuple16 _tmp91D;struct _tuple16 _tmp50D=(_tmp91D.f1=_tmp508,((
_tmp91D.f2=_tmp509,_tmp91D)));_npop_handler(0);return _tmp50D;}default: _LL2AD: {
struct Cyc_Core_Impossible_exn_struct _tmp923;const char*_tmp922;struct Cyc_Core_Impossible_exn_struct*
_tmp921;(int)_throw((void*)((_tmp921=_cycalloc(sizeof(*_tmp921)),((_tmp921[0]=((
_tmp923.tag=Cyc_Core_Impossible,((_tmp923.f1=((_tmp922="anal_test, zero-split",
_tag_dyneither(_tmp922,sizeof(char),22))),_tmp923)))),_tmp921)))));}}}_LL297:
_tmp4E1=_tmp4D6.f1;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4E2=(
struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4E1;if(_tmp4E2->tag != 0)
goto _LL299;};_tmp4E3=_tmp4D6.f2;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*
_tmp4E4=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4E3;if(_tmp4E4->tag
!= 0)goto _LL299;};_LL298: if(_tmp4A3 == Cyc_Absyn_Eq){struct _tuple16 _tmp924;struct
_tuple16 _tmp512=(_tmp924.f1=f,((_tmp924.f2=Cyc_CfFlowInfo_BottomFL(),_tmp924)));
_npop_handler(0);return _tmp512;}else{struct _tuple16 _tmp925;struct _tuple16 _tmp514=(
_tmp925.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp925.f2=f,_tmp925)));_npop_handler(0);
return _tmp514;}_LL299: _tmp4E5=_tmp4D6.f1;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*
_tmp4E6=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4E5;if(_tmp4E6->tag
!= 0)goto _LL29B;};_tmp4E7=_tmp4D6.f2;{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*
_tmp4E8=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp4E7;if(
_tmp4E8->tag != 1)goto _LL29B;};_LL29A: goto _LL29C;_LL29B: _tmp4E9=_tmp4D6.f1;{
struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4EA=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)
_tmp4E9;if(_tmp4EA->tag != 0)goto _LL29D;};_tmp4EB=_tmp4D6.f2;{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*
_tmp4EC=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4EB;if(
_tmp4EC->tag != 2)goto _LL29D;};_LL29C: goto _LL29E;_LL29D: _tmp4ED=_tmp4D6.f1;{
struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4EE=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)
_tmp4ED;if(_tmp4EE->tag != 0)goto _LL29F;};_tmp4EF=_tmp4D6.f2;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp4F0=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4EF;if(
_tmp4F0->tag != 5)goto _LL29F;};_LL29E: goto _LL2A0;_LL29F: _tmp4F1=_tmp4D6.f1;{
struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4F2=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)
_tmp4F1;if(_tmp4F2->tag != 1)goto _LL2A1;};_tmp4F3=_tmp4D6.f2;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*
_tmp4F4=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4F3;if(_tmp4F4->tag
!= 0)goto _LL2A1;};_LL2A0: goto _LL2A2;_LL2A1: _tmp4F5=_tmp4D6.f1;{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*
_tmp4F6=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4F5;if(
_tmp4F6->tag != 2)goto _LL2A3;};_tmp4F7=_tmp4D6.f2;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*
_tmp4F8=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4F7;if(_tmp4F8->tag
!= 0)goto _LL2A3;};_LL2A2: goto _LL2A4;_LL2A3: _tmp4F9=_tmp4D6.f1;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp4FA=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4F9;if(
_tmp4FA->tag != 5)goto _LL2A5;};_tmp4FB=_tmp4D6.f2;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*
_tmp4FC=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4FB;if(_tmp4FC->tag
!= 0)goto _LL2A5;};_LL2A4: if(_tmp4A3 == Cyc_Absyn_Neq){struct _tuple16 _tmp926;
struct _tuple16 _tmp516=(_tmp926.f1=f,((_tmp926.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp926)));_npop_handler(0);return _tmp516;}else{struct _tuple16 _tmp927;struct
_tuple16 _tmp518=(_tmp927.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp927.f2=f,_tmp927)));
_npop_handler(0);return _tmp518;}_LL2A5:;_LL2A6: goto _LL292;_LL292:;}{struct
_tuple0 _tmp928;struct _tuple0 _tmp51A=(_tmp928.f1=Cyc_Tcutil_compress((void*)
_check_null(_tmp4CF->topt)),((_tmp928.f2=Cyc_Tcutil_compress((void*)_check_null(
_tmp4D0->topt)),_tmp928)));void*_tmp51B;enum Cyc_Absyn_Sign _tmp51D;void*_tmp51E;
enum Cyc_Absyn_Sign _tmp520;void*_tmp521;void*_tmp523;_LL2B0: _tmp51B=_tmp51A.f1;{
struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp51C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp51B;if(_tmp51C->tag != 6)goto _LL2B2;else{_tmp51D=_tmp51C->f1;if(_tmp51D != Cyc_Absyn_Unsigned)
goto _LL2B2;}};_LL2B1: goto _LL2B3;_LL2B2: _tmp51E=_tmp51A.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp51F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp51E;if(_tmp51F->tag != 6)
goto _LL2B4;else{_tmp520=_tmp51F->f1;if(_tmp520 != Cyc_Absyn_Unsigned)goto _LL2B4;}};
_LL2B3: goto _LL2B5;_LL2B4: _tmp521=_tmp51A.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmp522=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp521;if(_tmp522->tag != 19)
goto _LL2B6;};_LL2B5: goto _LL2B7;_LL2B6: _tmp523=_tmp51A.f2;{struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmp524=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp523;if(_tmp524->tag != 19)
goto _LL2B8;};_LL2B7: goto _LL2AF;_LL2B8:;_LL2B9: {struct _tuple16 _tmp929;struct
_tuple16 _tmp526=(_tmp929.f1=f,((_tmp929.f2=f,_tmp929)));_npop_handler(0);return
_tmp526;}_LL2AF:;}switch(_tmp4A3){case Cyc_Absyn_Eq: _LL2BA: goto _LL2BB;case Cyc_Absyn_Neq:
_LL2BB: goto _LL2BC;case Cyc_Absyn_Gt: _LL2BC: goto _LL2BD;case Cyc_Absyn_Gte: _LL2BD: {
struct _tuple16 _tmp92A;struct _tuple16 _tmp528=(_tmp92A.f1=f,((_tmp92A.f2=f,_tmp92A)));
_npop_handler(0);return _tmp528;}case Cyc_Absyn_Lt: _LL2BE: {union Cyc_CfFlowInfo_FlowInfo
_tmp529=f;union Cyc_CfFlowInfo_FlowInfo _tmp52A=f;{union Cyc_CfFlowInfo_FlowInfo
_tmp52B=_tmp529;int _tmp52C;struct _tuple13 _tmp52D;struct Cyc_Dict_Dict _tmp52E;
_LL2C1: if((_tmp52B.BottomFL).tag != 1)goto _LL2C3;_tmp52C=(int)(_tmp52B.BottomFL).val;
_LL2C2: {struct Cyc_Core_Impossible_exn_struct _tmp930;const char*_tmp92F;struct Cyc_Core_Impossible_exn_struct*
_tmp92E;(int)_throw((void*)((_tmp92E=_cycalloc(sizeof(*_tmp92E)),((_tmp92E[0]=((
_tmp930.tag=Cyc_Core_Impossible,((_tmp930.f1=((_tmp92F="anal_test, Lt",
_tag_dyneither(_tmp92F,sizeof(char),14))),_tmp930)))),_tmp92E)))));}_LL2C3: if((
_tmp52B.ReachableFL).tag != 2)goto _LL2C0;_tmp52D=(struct _tuple13)(_tmp52B.ReachableFL).val;
_tmp52E=_tmp52D.f1;_LL2C4: _tmp4CB=_tmp52E;_LL2C0:;}{void*_tmp532=_tmp4CF->r;void*
_tmp534;struct Cyc_Absyn_Vardecl*_tmp536;void*_tmp538;struct Cyc_Absyn_Vardecl*
_tmp53A;void*_tmp53C;struct Cyc_Absyn_Vardecl*_tmp53E;void*_tmp540;struct Cyc_Absyn_Vardecl*
_tmp542;_LL2C6:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp533=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp532;if(_tmp533->tag != 1)goto _LL2C8;else{_tmp534=(void*)_tmp533->f2;{struct
Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp535=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)
_tmp534;if(_tmp535->tag != 1)goto _LL2C8;else{_tmp536=_tmp535->f1;}};}}if(!(!
_tmp536->escapes))goto _LL2C8;_LL2C7: _tmp53A=_tmp536;goto _LL2C9;_LL2C8:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp537=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp532;if(_tmp537->tag != 1)
goto _LL2CA;else{_tmp538=(void*)_tmp537->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmp539=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp538;if(_tmp539->tag != 
4)goto _LL2CA;else{_tmp53A=_tmp539->f1;}};}}if(!(!_tmp53A->escapes))goto _LL2CA;
_LL2C9: _tmp53E=_tmp53A;goto _LL2CB;_LL2CA:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp53B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp532;if(_tmp53B->tag != 1)
goto _LL2CC;else{_tmp53C=(void*)_tmp53B->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*
_tmp53D=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp53C;if(_tmp53D->tag != 5)
goto _LL2CC;else{_tmp53E=_tmp53D->f1;}};}}if(!(!_tmp53E->escapes))goto _LL2CC;
_LL2CB: _tmp542=_tmp53E;goto _LL2CD;_LL2CC:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp53F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp532;if(_tmp53F->tag != 1)
goto _LL2CE;else{_tmp540=(void*)_tmp53F->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*
_tmp541=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp540;if(_tmp541->tag != 
3)goto _LL2CE;else{_tmp542=_tmp541->f1;}};}}if(!(!_tmp542->escapes))goto _LL2CE;
_LL2CD: {void*_tmp543=_tmp4D0->r;void*_tmp545;struct Cyc_Absyn_Vardecl*_tmp547;
void*_tmp549;struct Cyc_Absyn_Vardecl*_tmp54B;void*_tmp54D;struct Cyc_Absyn_Vardecl*
_tmp54F;void*_tmp551;struct Cyc_Absyn_Vardecl*_tmp553;union Cyc_Absyn_Cnst _tmp555;
struct _tuple6 _tmp556;int _tmp557;struct Cyc_Absyn_Exp*_tmp559;struct Cyc_Absyn_Exp
_tmp55A;void*_tmp55B;union Cyc_Absyn_Cnst _tmp55D;struct _tuple6 _tmp55E;int _tmp55F;
enum Cyc_Absyn_Primop _tmp561;struct Cyc_List_List*_tmp562;struct Cyc_List_List
_tmp563;struct Cyc_Absyn_Exp*_tmp564;_LL2D1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp544=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp543;if(_tmp544->tag != 1)
goto _LL2D3;else{_tmp545=(void*)_tmp544->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp546=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp545;if(_tmp546->tag
!= 1)goto _LL2D3;else{_tmp547=_tmp546->f1;}};}}if(!(!_tmp547->escapes))goto _LL2D3;
_LL2D2: _tmp54B=_tmp547;goto _LL2D4;_LL2D3:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp548=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp543;if(_tmp548->tag != 1)
goto _LL2D5;else{_tmp549=(void*)_tmp548->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmp54A=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp549;if(_tmp54A->tag != 
4)goto _LL2D5;else{_tmp54B=_tmp54A->f1;}};}}if(!(!_tmp54B->escapes))goto _LL2D5;
_LL2D4: _tmp54F=_tmp54B;goto _LL2D6;_LL2D5:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp54C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp543;if(_tmp54C->tag != 1)
goto _LL2D7;else{_tmp54D=(void*)_tmp54C->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*
_tmp54E=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp54D;if(_tmp54E->tag != 5)
goto _LL2D7;else{_tmp54F=_tmp54E->f1;}};}}if(!(!_tmp54F->escapes))goto _LL2D7;
_LL2D6: _tmp553=_tmp54F;goto _LL2D8;_LL2D7:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp550=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp543;if(_tmp550->tag != 1)
goto _LL2D9;else{_tmp551=(void*)_tmp550->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*
_tmp552=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp551;if(_tmp552->tag != 
3)goto _LL2D9;else{_tmp553=_tmp552->f1;}};}}if(!(!_tmp553->escapes))goto _LL2D9;
_LL2D8: {struct _RegionHandle*_tmp565=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*
_tmp933;struct Cyc_List_List*_tmp932;struct Cyc_List_List*_tmp566=(_tmp932=
_region_malloc(_tmp565,sizeof(*_tmp932)),((_tmp932->hd=((_tmp933=_region_malloc(
_tmp565,sizeof(*_tmp933)),((_tmp933->vd=_tmp542,((_tmp933->rop=Cyc_CfFlowInfo_LessVar(
_tmp553,_tmp553->type),_tmp933)))))),((_tmp932->tl=_tmp4CC,_tmp932)))));struct
_tuple16 _tmp934;struct _tuple16 _tmp568=(_tmp934.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4CB,_tmp566),((_tmp934.f2=_tmp52A,_tmp934)));_npop_handler(0);return _tmp568;}
_LL2D9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp554=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmp543;if(_tmp554->tag != 0)goto _LL2DB;else{_tmp555=_tmp554->f1;if((_tmp555.Int_c).tag
!= 5)goto _LL2DB;_tmp556=(struct _tuple6)(_tmp555.Int_c).val;_tmp557=_tmp556.f2;}}
_LL2DA: _tmp55F=_tmp557;goto _LL2DC;_LL2DB: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*
_tmp558=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp543;if(_tmp558->tag != 
13)goto _LL2DD;else{_tmp559=_tmp558->f2;_tmp55A=*_tmp559;_tmp55B=_tmp55A.r;{
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp55C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmp55B;if(_tmp55C->tag != 0)goto _LL2DD;else{_tmp55D=_tmp55C->f1;if((_tmp55D.Int_c).tag
!= 5)goto _LL2DD;_tmp55E=(struct _tuple6)(_tmp55D.Int_c).val;_tmp55F=_tmp55E.f2;}};}}
_LL2DC: {struct _RegionHandle*_tmp56B=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*
_tmp937;struct Cyc_List_List*_tmp936;struct Cyc_List_List*_tmp56C=(_tmp936=
_region_malloc(_tmp56B,sizeof(*_tmp936)),((_tmp936->hd=((_tmp937=_region_malloc(
_tmp56B,sizeof(*_tmp937)),((_tmp937->vd=_tmp542,((_tmp937->rop=Cyc_CfFlowInfo_LessConst((
unsigned int)_tmp55F),_tmp937)))))),((_tmp936->tl=_tmp4CC,_tmp936)))));struct
_tuple16 _tmp938;struct _tuple16 _tmp56E=(_tmp938.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4CB,_tmp56C),((_tmp938.f2=_tmp52A,_tmp938)));_npop_handler(0);return _tmp56E;}
_LL2DD: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp560=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)
_tmp543;if(_tmp560->tag != 2)goto _LL2DF;else{_tmp561=_tmp560->f1;_tmp562=_tmp560->f2;
if(_tmp562 == 0)goto _LL2DF;_tmp563=*_tmp562;_tmp564=(struct Cyc_Absyn_Exp*)_tmp563.hd;}}
_LL2DE: {void*_tmp571=_tmp564->r;void*_tmp573;struct Cyc_Absyn_Vardecl*_tmp575;
void*_tmp577;struct Cyc_Absyn_Vardecl*_tmp579;void*_tmp57B;struct Cyc_Absyn_Vardecl*
_tmp57D;void*_tmp57F;struct Cyc_Absyn_Vardecl*_tmp581;_LL2E2:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp572=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp571;if(_tmp572->tag != 1)
goto _LL2E4;else{_tmp573=(void*)_tmp572->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp574=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp573;if(_tmp574->tag
!= 1)goto _LL2E4;else{_tmp575=_tmp574->f1;}};}}if(!(!_tmp575->escapes))goto _LL2E4;
_LL2E3: _tmp579=_tmp575;goto _LL2E5;_LL2E4:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp576=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp571;if(_tmp576->tag != 1)
goto _LL2E6;else{_tmp577=(void*)_tmp576->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmp578=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp577;if(_tmp578->tag != 
4)goto _LL2E6;else{_tmp579=_tmp578->f1;}};}}if(!(!_tmp579->escapes))goto _LL2E6;
_LL2E5: _tmp57D=_tmp579;goto _LL2E7;_LL2E6:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp57A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp571;if(_tmp57A->tag != 1)
goto _LL2E8;else{_tmp57B=(void*)_tmp57A->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*
_tmp57C=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp57B;if(_tmp57C->tag != 5)
goto _LL2E8;else{_tmp57D=_tmp57C->f1;}};}}if(!(!_tmp57D->escapes))goto _LL2E8;
_LL2E7: _tmp581=_tmp57D;goto _LL2E9;_LL2E8:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp57E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp571;if(_tmp57E->tag != 1)
goto _LL2EA;else{_tmp57F=(void*)_tmp57E->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*
_tmp580=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp57F;if(_tmp580->tag != 
3)goto _LL2EA;else{_tmp581=_tmp580->f1;}};}}if(!(!_tmp581->escapes))goto _LL2EA;
_LL2E9: {struct _RegionHandle*_tmp582=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*
_tmp93B;struct Cyc_List_List*_tmp93A;struct Cyc_List_List*_tmp583=(_tmp93A=
_region_malloc(_tmp582,sizeof(*_tmp93A)),((_tmp93A->hd=((_tmp93B=_region_malloc(
_tmp582,sizeof(*_tmp93B)),((_tmp93B->vd=_tmp542,((_tmp93B->rop=Cyc_CfFlowInfo_LessNumelts(
_tmp581),_tmp93B)))))),((_tmp93A->tl=_tmp4CC,_tmp93A)))));struct _tuple16 _tmp93C;
struct _tuple16 _tmp585=(_tmp93C.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4CB,_tmp583),((
_tmp93C.f2=_tmp52A,_tmp93C)));_npop_handler(0);return _tmp585;}_LL2EA:;_LL2EB: {
struct _tuple16 _tmp93D;struct _tuple16 _tmp589=(_tmp93D.f1=_tmp529,((_tmp93D.f2=
_tmp52A,_tmp93D)));_npop_handler(0);return _tmp589;}_LL2E1:;}_LL2DF:;_LL2E0: {
struct _tuple16 _tmp93E;struct _tuple16 _tmp58B=(_tmp93E.f1=_tmp529,((_tmp93E.f2=
_tmp52A,_tmp93E)));_npop_handler(0);return _tmp58B;}_LL2D0:;}_LL2CE:;_LL2CF: {
struct _tuple16 _tmp93F;struct _tuple16 _tmp58D=(_tmp93F.f1=_tmp529,((_tmp93F.f2=
_tmp52A,_tmp93F)));_npop_handler(0);return _tmp58D;}_LL2C5:;};}case Cyc_Absyn_Lte:
_LL2BF: {union Cyc_CfFlowInfo_FlowInfo _tmp58E=f;union Cyc_CfFlowInfo_FlowInfo
_tmp58F=f;{union Cyc_CfFlowInfo_FlowInfo _tmp590=_tmp58E;int _tmp591;struct _tuple13
_tmp592;struct Cyc_Dict_Dict _tmp593;_LL2EE: if((_tmp590.BottomFL).tag != 1)goto
_LL2F0;_tmp591=(int)(_tmp590.BottomFL).val;_LL2EF: {struct Cyc_Core_Impossible_exn_struct
_tmp945;const char*_tmp944;struct Cyc_Core_Impossible_exn_struct*_tmp943;(int)
_throw((void*)((_tmp943=_cycalloc(sizeof(*_tmp943)),((_tmp943[0]=((_tmp945.tag=
Cyc_Core_Impossible,((_tmp945.f1=((_tmp944="anal_test, Lte",_tag_dyneither(
_tmp944,sizeof(char),15))),_tmp945)))),_tmp943)))));}_LL2F0: if((_tmp590.ReachableFL).tag
!= 2)goto _LL2ED;_tmp592=(struct _tuple13)(_tmp590.ReachableFL).val;_tmp593=
_tmp592.f1;_LL2F1: _tmp4CB=_tmp593;_LL2ED:;}{void*_tmp597=_tmp4CF->r;void*_tmp599;
struct Cyc_Absyn_Vardecl*_tmp59B;void*_tmp59D;struct Cyc_Absyn_Vardecl*_tmp59F;
void*_tmp5A1;struct Cyc_Absyn_Vardecl*_tmp5A3;void*_tmp5A5;struct Cyc_Absyn_Vardecl*
_tmp5A7;_LL2F3:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp598=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp597;if(_tmp598->tag != 1)goto _LL2F5;else{_tmp599=(void*)_tmp598->f2;{struct
Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp59A=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)
_tmp599;if(_tmp59A->tag != 1)goto _LL2F5;else{_tmp59B=_tmp59A->f1;}};}}if(!(!
_tmp59B->escapes))goto _LL2F5;_LL2F4: _tmp59F=_tmp59B;goto _LL2F6;_LL2F5:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp59C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp597;if(_tmp59C->tag != 1)
goto _LL2F7;else{_tmp59D=(void*)_tmp59C->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmp59E=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp59D;if(_tmp59E->tag != 
4)goto _LL2F7;else{_tmp59F=_tmp59E->f1;}};}}if(!(!_tmp59F->escapes))goto _LL2F7;
_LL2F6: _tmp5A3=_tmp59F;goto _LL2F8;_LL2F7:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp5A0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp597;if(_tmp5A0->tag != 1)
goto _LL2F9;else{_tmp5A1=(void*)_tmp5A0->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*
_tmp5A2=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp5A1;if(_tmp5A2->tag != 5)
goto _LL2F9;else{_tmp5A3=_tmp5A2->f1;}};}}if(!(!_tmp5A3->escapes))goto _LL2F9;
_LL2F8: _tmp5A7=_tmp5A3;goto _LL2FA;_LL2F9:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp5A4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp597;if(_tmp5A4->tag != 1)
goto _LL2FB;else{_tmp5A5=(void*)_tmp5A4->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*
_tmp5A6=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp5A5;if(_tmp5A6->tag != 
3)goto _LL2FB;else{_tmp5A7=_tmp5A6->f1;}};}}if(!(!_tmp5A7->escapes))goto _LL2FB;
_LL2FA: {void*_tmp5A8=_tmp4D0->r;union Cyc_Absyn_Cnst _tmp5AA;struct _tuple6 _tmp5AB;
int _tmp5AC;struct Cyc_Absyn_Exp*_tmp5AE;struct Cyc_Absyn_Exp _tmp5AF;void*_tmp5B0;
union Cyc_Absyn_Cnst _tmp5B2;struct _tuple6 _tmp5B3;int _tmp5B4;enum Cyc_Absyn_Primop
_tmp5B6;struct Cyc_List_List*_tmp5B7;struct Cyc_List_List _tmp5B8;struct Cyc_Absyn_Exp*
_tmp5B9;_LL2FE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5A9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmp5A8;if(_tmp5A9->tag != 0)goto _LL300;else{_tmp5AA=_tmp5A9->f1;if((_tmp5AA.Int_c).tag
!= 5)goto _LL300;_tmp5AB=(struct _tuple6)(_tmp5AA.Int_c).val;_tmp5AC=_tmp5AB.f2;}}
_LL2FF: _tmp5B4=_tmp5AC;goto _LL301;_LL300: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*
_tmp5AD=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A8;if(_tmp5AD->tag != 
13)goto _LL302;else{_tmp5AE=_tmp5AD->f2;_tmp5AF=*_tmp5AE;_tmp5B0=_tmp5AF.r;{
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5B1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmp5B0;if(_tmp5B1->tag != 0)goto _LL302;else{_tmp5B2=_tmp5B1->f1;if((_tmp5B2.Int_c).tag
!= 5)goto _LL302;_tmp5B3=(struct _tuple6)(_tmp5B2.Int_c).val;_tmp5B4=_tmp5B3.f2;}};}}
_LL301: {struct _RegionHandle*_tmp5BA=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*
_tmp948;struct Cyc_List_List*_tmp947;struct Cyc_List_List*_tmp5BB=(_tmp947=
_region_malloc(_tmp5BA,sizeof(*_tmp947)),((_tmp947->hd=((_tmp948=_region_malloc(
_tmp5BA,sizeof(*_tmp948)),((_tmp948->vd=_tmp5A7,((_tmp948->rop=Cyc_CfFlowInfo_LessEqConst((
unsigned int)_tmp5B4),_tmp948)))))),((_tmp947->tl=_tmp4CC,_tmp947)))));struct
_tuple16 _tmp949;struct _tuple16 _tmp5BD=(_tmp949.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4CB,_tmp5BB),((_tmp949.f2=_tmp58F,_tmp949)));_npop_handler(0);return _tmp5BD;}
_LL302: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp5B5=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)
_tmp5A8;if(_tmp5B5->tag != 2)goto _LL304;else{_tmp5B6=_tmp5B5->f1;_tmp5B7=_tmp5B5->f2;
if(_tmp5B7 == 0)goto _LL304;_tmp5B8=*_tmp5B7;_tmp5B9=(struct Cyc_Absyn_Exp*)_tmp5B8.hd;}}
_LL303: {void*_tmp5C0=_tmp5B9->r;void*_tmp5C2;struct Cyc_Absyn_Vardecl*_tmp5C4;
void*_tmp5C6;struct Cyc_Absyn_Vardecl*_tmp5C8;void*_tmp5CA;struct Cyc_Absyn_Vardecl*
_tmp5CC;void*_tmp5CE;struct Cyc_Absyn_Vardecl*_tmp5D0;_LL307:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp5C1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5C0;if(_tmp5C1->tag != 1)
goto _LL309;else{_tmp5C2=(void*)_tmp5C1->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp5C3=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5C2;if(_tmp5C3->tag
!= 1)goto _LL309;else{_tmp5C4=_tmp5C3->f1;}};}}if(!(!_tmp5C4->escapes))goto _LL309;
_LL308: _tmp5C8=_tmp5C4;goto _LL30A;_LL309:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp5C5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5C0;if(_tmp5C5->tag != 1)
goto _LL30B;else{_tmp5C6=(void*)_tmp5C5->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmp5C7=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5C6;if(_tmp5C7->tag != 
4)goto _LL30B;else{_tmp5C8=_tmp5C7->f1;}};}}if(!(!_tmp5C8->escapes))goto _LL30B;
_LL30A: _tmp5CC=_tmp5C8;goto _LL30C;_LL30B:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp5C9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5C0;if(_tmp5C9->tag != 1)
goto _LL30D;else{_tmp5CA=(void*)_tmp5C9->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*
_tmp5CB=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp5CA;if(_tmp5CB->tag != 5)
goto _LL30D;else{_tmp5CC=_tmp5CB->f1;}};}}if(!(!_tmp5CC->escapes))goto _LL30D;
_LL30C: _tmp5D0=_tmp5CC;goto _LL30E;_LL30D:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp5CD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5C0;if(_tmp5CD->tag != 1)
goto _LL30F;else{_tmp5CE=(void*)_tmp5CD->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*
_tmp5CF=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp5CE;if(_tmp5CF->tag != 
3)goto _LL30F;else{_tmp5D0=_tmp5CF->f1;}};}}if(!(!_tmp5D0->escapes))goto _LL30F;
_LL30E: {struct Cyc_CfFlowInfo_FlowEnv*_tmp5D1=env->fenv;struct Cyc_CfFlowInfo_Reln*
_tmp94C;struct Cyc_List_List*_tmp94B;struct Cyc_List_List*_tmp5D2=(_tmp94B=
_region_malloc(_tmp5D1->r,sizeof(*_tmp94B)),((_tmp94B->hd=((_tmp94C=
_region_malloc(_tmp5D1->r,sizeof(*_tmp94C)),((_tmp94C->vd=_tmp5A7,((_tmp94C->rop=
Cyc_CfFlowInfo_LessEqNumelts(_tmp5D0),_tmp94C)))))),((_tmp94B->tl=_tmp4CC,
_tmp94B)))));struct _tuple16 _tmp94D;struct _tuple16 _tmp5D4=(_tmp94D.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4CB,_tmp5D2),((_tmp94D.f2=_tmp58F,_tmp94D)));_npop_handler(0);return _tmp5D4;}
_LL30F:;_LL310: {struct _tuple16 _tmp94E;struct _tuple16 _tmp5D8=(_tmp94E.f1=_tmp58E,((
_tmp94E.f2=_tmp58F,_tmp94E)));_npop_handler(0);return _tmp5D8;}_LL306:;}_LL304:;
_LL305: {struct _tuple16 _tmp94F;struct _tuple16 _tmp5DA=(_tmp94F.f1=_tmp58E,((
_tmp94F.f2=_tmp58F,_tmp94F)));_npop_handler(0);return _tmp5DA;}_LL2FD:;}_LL2FB:;
_LL2FC: {struct _tuple16 _tmp950;struct _tuple16 _tmp5DC=(_tmp950.f1=_tmp58E,((
_tmp950.f2=_tmp58F,_tmp950)));_npop_handler(0);return _tmp5DC;}_LL2F2:;};}
default: _LL2EC: {struct _tuple16 _tmp951;struct _tuple16 _tmp5DE=(_tmp951.f1=f,((
_tmp951.f2=f,_tmp951)));_npop_handler(0);return _tmp5DE;}}}_LL28D:;};;_pop_region(
temp);}_LL28B:;_LL28C: goto _LL27E;_LL27E:;}{union Cyc_CfFlowInfo_FlowInfo _tmp5E0;
void*_tmp5E1;struct _tuple14 _tmp5DF=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);
_tmp5E0=_tmp5DF.f1;_tmp5E1=_tmp5DF.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp5E2=
_tmp5E0;int _tmp5E3;struct _tuple13 _tmp5E4;struct Cyc_Dict_Dict _tmp5E5;_LL313: if((
_tmp5E2.BottomFL).tag != 1)goto _LL315;_tmp5E3=(int)(_tmp5E2.BottomFL).val;_LL314: {
struct _tuple16 _tmp952;return(_tmp952.f1=_tmp5E0,((_tmp952.f2=_tmp5E0,_tmp952)));}
_LL315: if((_tmp5E2.ReachableFL).tag != 2)goto _LL312;_tmp5E4=(struct _tuple13)(
_tmp5E2.ReachableFL).val;_tmp5E5=_tmp5E4.f1;_LL316: {void*_tmp5E7=_tmp5E1;enum 
Cyc_CfFlowInfo_InitLevel _tmp5ED;enum Cyc_CfFlowInfo_InitLevel _tmp5EF;enum Cyc_CfFlowInfo_InitLevel
_tmp5F2;_LL318: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp5E8=(
struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp5E7;if(_tmp5E8->tag != 0)
goto _LL31A;}_LL319: {struct _tuple16 _tmp953;return(_tmp953.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp953.f2=_tmp5E0,_tmp953)));}_LL31A: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*
_tmp5E9=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp5E7;if(
_tmp5E9->tag != 2)goto _LL31C;}_LL31B: goto _LL31D;_LL31C: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*
_tmp5EA=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp5E7;if(
_tmp5EA->tag != 1)goto _LL31E;}_LL31D: goto _LL31F;_LL31E: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp5EB=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp5E7;if(
_tmp5EB->tag != 5)goto _LL320;}_LL31F: {struct _tuple16 _tmp954;return(_tmp954.f1=
_tmp5E0,((_tmp954.f2=Cyc_CfFlowInfo_BottomFL(),_tmp954)));}_LL320: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*
_tmp5EC=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp5E7;if(
_tmp5EC->tag != 3)goto _LL322;else{_tmp5ED=_tmp5EC->f1;if(_tmp5ED != Cyc_CfFlowInfo_NoneIL)
goto _LL322;}}_LL321: goto _LL323;_LL322: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*
_tmp5EE=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp5E7;if(_tmp5EE->tag
!= 4)goto _LL324;else{_tmp5EF=_tmp5EE->f1;if(_tmp5EF != Cyc_CfFlowInfo_NoneIL)goto
_LL324;}}_LL323: goto _LL325;_LL324: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*
_tmp5F0=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5E7;if(
_tmp5F0->tag != 7)goto _LL326;}_LL325:{const char*_tmp957;void*_tmp956;(_tmp956=0,
Cyc_Tcutil_terr(e->loc,((_tmp957="expression may not be initialized",
_tag_dyneither(_tmp957,sizeof(char),34))),_tag_dyneither(_tmp956,sizeof(void*),0)));}{
struct _tuple16 _tmp958;return(_tmp958.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp958.f2=
Cyc_CfFlowInfo_BottomFL(),_tmp958)));};_LL326: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*
_tmp5F1=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp5E7;if(
_tmp5F1->tag != 3)goto _LL328;else{_tmp5F2=_tmp5F1->f1;}}_LL327: return Cyc_NewControlFlow_splitzero(
env,inflow,_tmp5E0,e,_tmp5F2);_LL328: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*
_tmp5F3=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp5E7;if(_tmp5F3->tag
!= 4)goto _LL32A;}_LL329: {struct _tuple16 _tmp959;return(_tmp959.f1=_tmp5E0,((
_tmp959.f2=_tmp5E0,_tmp959)));}_LL32A: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*
_tmp5F4=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5E7;if(
_tmp5F4->tag != 6)goto _LL317;}_LL32B: {struct Cyc_Core_Impossible_exn_struct
_tmp95F;const char*_tmp95E;struct Cyc_Core_Impossible_exn_struct*_tmp95D;(int)
_throw((void*)((_tmp95D=_cycalloc(sizeof(*_tmp95D)),((_tmp95D[0]=((_tmp95F.tag=
Cyc_Core_Impossible,((_tmp95F.f1=((_tmp95E="anal_test",_tag_dyneither(_tmp95E,
sizeof(char),10))),_tmp95F)))),_tmp95D)))));}_LL317:;}_LL312:;};};}struct
_tuple23{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict
f3;};static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple23*ckenv,
void*root,void*rval){struct _tuple23 _tmp5FF;unsigned int _tmp600;struct Cyc_NewControlFlow_AnalEnv*
_tmp601;struct Cyc_Dict_Dict _tmp602;struct _tuple23*_tmp5FE=ckenv;_tmp5FF=*_tmp5FE;
_tmp600=_tmp5FF.f1;_tmp601=_tmp5FF.f2;_tmp602=_tmp5FF.f3;{void*_tmp603=root;
struct Cyc_Absyn_Vardecl*_tmp605;_LL32D: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*
_tmp604=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp603;if(_tmp604->tag
!= 0)goto _LL32F;else{_tmp605=_tmp604->f1;}}_LL32E: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
_tmp601->fenv)->r,_tmp605->type)){void*_tmp606=rval;enum Cyc_CfFlowInfo_InitLevel
_tmp60A;_LL332: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp607=(
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp606;if(_tmp607->tag
!= 7)goto _LL334;}_LL333: goto _LL335;_LL334: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*
_tmp608=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp606;if(_tmp608->tag
!= 0)goto _LL336;}_LL335: goto _LL337;_LL336: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*
_tmp609=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp606;if(
_tmp609->tag != 3)goto _LL338;else{_tmp60A=_tmp609->f1;if(_tmp60A != Cyc_CfFlowInfo_NoneIL)
goto _LL338;}}_LL337: goto _LL331;_LL338:;_LL339:{const char*_tmp963;void*_tmp962[1];
struct Cyc_String_pa_PrintArg_struct _tmp961;(_tmp961.tag=0,((_tmp961.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp605->name)),((
_tmp962[0]=& _tmp961,Cyc_Tcutil_warn(_tmp600,((_tmp963="unique pointers reachable from %s may become unreachable",
_tag_dyneither(_tmp963,sizeof(char),57))),_tag_dyneither(_tmp962,sizeof(void*),1)))))));}
goto _LL331;_LL331:;}goto _LL32C;_LL32F:;_LL330: goto _LL32C;_LL32C:;};}static void
Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo flow){union Cyc_CfFlowInfo_FlowInfo _tmp60E=flow;
int _tmp60F;struct _tuple13 _tmp610;struct Cyc_Dict_Dict _tmp611;_LL33B: if((_tmp60E.BottomFL).tag
!= 1)goto _LL33D;_tmp60F=(int)(_tmp60E.BottomFL).val;_LL33C: return;_LL33D: if((
_tmp60E.ReachableFL).tag != 2)goto _LL33A;_tmp610=(struct _tuple13)(_tmp60E.ReachableFL).val;
_tmp611=_tmp610.f1;_LL33E:{struct Cyc_List_List*_tmp612=env->param_roots;for(0;
_tmp612 != 0;_tmp612=_tmp612->tl){if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp611,
Cyc_CfFlowInfo_lookup_place(_tmp611,(struct Cyc_CfFlowInfo_Place*)_tmp612->hd))!= 
Cyc_CfFlowInfo_AllIL){const char*_tmp966;void*_tmp965;(_tmp965=0,Cyc_Tcutil_terr(
loc,((_tmp966="function may not initialize all the parameters with attribute 'initializes'",
_tag_dyneither(_tmp966,sizeof(char),76))),_tag_dyneither(_tmp965,sizeof(void*),0)));}}}
if(Cyc_NewControlFlow_warn_lose_unique){struct _tuple23 _tmp967;struct _tuple23
_tmp615=(_tmp967.f1=loc,((_tmp967.f2=env,((_tmp967.f3=_tmp611,_tmp967)))));((
void(*)(void(*f)(struct _tuple23*,void*,void*),struct _tuple23*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp615,_tmp611);}
return;_LL33A:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp617=env->fenv;for(0;scs != 0;
scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp619;struct Cyc_Core_Opt*_tmp61A;
struct Cyc_Absyn_Exp*_tmp61B;struct Cyc_Absyn_Stmt*_tmp61C;unsigned int _tmp61D;
struct Cyc_Absyn_Switch_clause*_tmp618=(struct Cyc_Absyn_Switch_clause*)scs->hd;
_tmp619=*_tmp618;_tmp61A=_tmp619.pat_vars;_tmp61B=_tmp619.where_clause;_tmp61C=
_tmp619.body;_tmp61D=_tmp619.loc;{union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_add_vars(
_tmp617,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp61A))->v,
_tmp617->unknown_all,_tmp61D);union Cyc_CfFlowInfo_FlowInfo clause_outflow;if(
_tmp61B != 0){struct Cyc_Absyn_Exp*wexp=(struct Cyc_Absyn_Exp*)_tmp61B;union Cyc_CfFlowInfo_FlowInfo
_tmp61F;union Cyc_CfFlowInfo_FlowInfo _tmp620;struct _tuple16 _tmp61E=Cyc_NewControlFlow_anal_test(
env,clause_inflow,wexp);_tmp61F=_tmp61E.f1;_tmp620=_tmp61E.f2;inflow=_tmp620;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp61F,_tmp61C);}else{
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp61C);}{union
Cyc_CfFlowInfo_FlowInfo _tmp621=clause_outflow;int _tmp622;_LL340: if((_tmp621.BottomFL).tag
!= 1)goto _LL342;_tmp622=(int)(_tmp621.BottomFL).val;_LL341: goto _LL33F;_LL342:;
_LL343: if(scs->tl == 0)return clause_outflow;else{if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->pat_vars))->v != 0){const char*_tmp96A;void*_tmp969;(
_tmp969=0,Cyc_Tcutil_terr(_tmp61C->loc,((_tmp96A="switch clause may implicitly fallthru",
_tag_dyneither(_tmp96A,sizeof(char),38))),_tag_dyneither(_tmp969,sizeof(void*),0)));}
else{const char*_tmp96D;void*_tmp96C;(_tmp96C=0,Cyc_Tcutil_warn(_tmp61C->loc,((
_tmp96D="switch clause may implicitly fallthru",_tag_dyneither(_tmp96D,sizeof(
char),38))),_tag_dyneither(_tmp96C,sizeof(void*),0)));}Cyc_NewControlFlow_update_flow(
env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL33F;_LL33F:;};};}return Cyc_CfFlowInfo_BottomFL();}struct
_tuple24{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int
f3;};static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple24*vdenv,
struct Cyc_Absyn_Vardecl*vd){struct _tuple24 _tmp628;struct Cyc_NewControlFlow_AnalEnv*
_tmp629;struct Cyc_Dict_Dict _tmp62A;unsigned int _tmp62B;struct _tuple24*_tmp627=
vdenv;_tmp628=*_tmp627;_tmp629=_tmp628.f1;_tmp62A=_tmp628.f2;_tmp62B=_tmp628.f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp629->fenv)->r,vd->type)){struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct
_tmp96E;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp96E.tag=0,((
_tmp96E.f1=vd,_tmp96E)));void*_tmp62C=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(
void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp62A,Cyc_CfFlowInfo_root_cmp,(
void*)& vdroot);void*_tmp62D=_tmp62C;enum Cyc_CfFlowInfo_InitLevel _tmp631;_LL345: {
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp62E=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)
_tmp62D;if(_tmp62E->tag != 7)goto _LL347;}_LL346: goto _LL348;_LL347: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*
_tmp62F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp62D;if(_tmp62F->tag
!= 0)goto _LL349;}_LL348: goto _LL34A;_LL349: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*
_tmp630=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp62D;if(
_tmp630->tag != 3)goto _LL34B;else{_tmp631=_tmp630->f1;if(_tmp631 != Cyc_CfFlowInfo_NoneIL)
goto _LL34B;}}_LL34A: goto _LL344;_LL34B:;_LL34C:{const char*_tmp972;void*_tmp971[1];
struct Cyc_String_pa_PrintArg_struct _tmp970;(_tmp970.tag=0,((_tmp970.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((
_tmp971[0]=& _tmp970,Cyc_Tcutil_warn(_tmp62B,((_tmp972="unique pointers may still exist after variable %s goes out of scope",
_tag_dyneither(_tmp972,sizeof(char),68))),_tag_dyneither(_tmp971,sizeof(void*),1)))))));}
goto _LL344;_LL344:;}}static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){{union Cyc_CfFlowInfo_FlowInfo
_tmp636=inflow;struct _tuple13 _tmp637;struct Cyc_Dict_Dict _tmp638;_LL34E: if((
_tmp636.ReachableFL).tag != 2)goto _LL350;_tmp637=(struct _tuple13)(_tmp636.ReachableFL).val;
_tmp638=_tmp637.f1;_LL34F: {struct _tuple24 _tmp973;struct _tuple24 _tmp639=(_tmp973.f1=
env,((_tmp973.f2=_tmp638,((_tmp973.f3=decl->loc,_tmp973)))));struct Cyc_CfFlowInfo_FlowEnv*
_tmp63A=env->fenv;{void*_tmp63B=decl->r;struct Cyc_Absyn_Vardecl*_tmp63D;struct
Cyc_Core_Opt*_tmp63F;struct Cyc_Core_Opt _tmp640;struct Cyc_List_List*_tmp641;
struct Cyc_List_List*_tmp643;_LL353: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*
_tmp63C=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp63B;if(_tmp63C->tag != 
0)goto _LL355;else{_tmp63D=_tmp63C->f1;}}_LL354: Cyc_NewControlFlow_check_dropped_unique_vd(&
_tmp639,_tmp63D);goto _LL352;_LL355: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*
_tmp63E=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp63B;if(_tmp63E->tag != 
2)goto _LL357;else{_tmp63F=_tmp63E->f2;if(_tmp63F == 0)goto _LL357;_tmp640=*_tmp63F;
_tmp641=(struct Cyc_List_List*)_tmp640.v;}}_LL356: _tmp643=_tmp641;goto _LL358;
_LL357: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp642=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)
_tmp63B;if(_tmp642->tag != 3)goto _LL359;else{_tmp643=_tmp642->f1;}}_LL358:((void(*)(
void(*f)(struct _tuple24*,struct Cyc_Absyn_Vardecl*),struct _tuple24*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp639,_tmp643);
goto _LL352;_LL359:;_LL35A: goto _LL352;_LL352:;}goto _LL34D;}_LL350:;_LL351: goto
_LL34D;_LL34D:;}return;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp646;union Cyc_CfFlowInfo_FlowInfo*_tmp647;struct _tuple17 _tmp645=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);_tmp646=_tmp645.f1;_tmp647=_tmp645.f2;inflow=*_tmp647;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp648=env->fenv;void*_tmp649=s->r;struct Cyc_Absyn_Exp*_tmp64C;struct Cyc_Absyn_Exp*
_tmp64E;struct Cyc_Absyn_Exp*_tmp650;struct Cyc_Absyn_Stmt*_tmp652;struct Cyc_Absyn_Stmt*
_tmp653;struct Cyc_Absyn_Exp*_tmp655;struct Cyc_Absyn_Stmt*_tmp656;struct Cyc_Absyn_Stmt*
_tmp657;struct _tuple10 _tmp659;struct Cyc_Absyn_Exp*_tmp65A;struct Cyc_Absyn_Stmt*
_tmp65B;struct Cyc_Absyn_Stmt*_tmp65C;struct Cyc_Absyn_Stmt*_tmp65E;struct _tuple10
_tmp65F;struct Cyc_Absyn_Exp*_tmp660;struct Cyc_Absyn_Stmt*_tmp661;struct Cyc_Absyn_Exp*
_tmp663;struct _tuple10 _tmp664;struct Cyc_Absyn_Exp*_tmp665;struct Cyc_Absyn_Stmt*
_tmp666;struct _tuple10 _tmp667;struct Cyc_Absyn_Exp*_tmp668;struct Cyc_Absyn_Stmt*
_tmp669;struct Cyc_Absyn_Stmt*_tmp66A;struct Cyc_Absyn_Stmt*_tmp66C;struct Cyc_List_List*
_tmp66E;struct Cyc_Absyn_Switch_clause**_tmp66F;struct Cyc_Absyn_Switch_clause*
_tmp670;struct Cyc_Absyn_Stmt*_tmp672;struct Cyc_Absyn_Stmt*_tmp674;struct Cyc_Absyn_Stmt*
_tmp676;struct Cyc_Absyn_Exp*_tmp678;struct Cyc_List_List*_tmp679;struct Cyc_Absyn_Stmt*
_tmp67B;struct Cyc_List_List*_tmp67C;struct Cyc_Absyn_Decl*_tmp67E;struct Cyc_Absyn_Decl
_tmp67F;void*_tmp680;struct Cyc_Absyn_Exp*_tmp682;struct Cyc_Absyn_Tvar*_tmp683;
struct Cyc_Absyn_Vardecl*_tmp684;struct Cyc_Absyn_Stmt*_tmp685;struct Cyc_Absyn_Decl*
_tmp687;struct Cyc_Absyn_Stmt*_tmp688;struct Cyc_Absyn_Stmt*_tmp68A;struct Cyc_Absyn_Exp*
_tmp68C;_LL35C: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp64A=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)
_tmp649;if(_tmp64A->tag != 0)goto _LL35E;}_LL35D: return inflow;_LL35E: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*
_tmp64B=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp649;if(_tmp64B->tag
!= 3)goto _LL360;else{_tmp64C=_tmp64B->f1;if(_tmp64C != 0)goto _LL360;}}_LL35F: if(
env->noreturn){const char*_tmp976;void*_tmp975;(_tmp975=0,Cyc_Tcutil_terr(s->loc,((
_tmp976="`noreturn' function might return",_tag_dyneither(_tmp976,sizeof(char),
33))),_tag_dyneither(_tmp975,sizeof(void*),0)));}Cyc_NewControlFlow_check_init_params(
s->loc,env,inflow);return Cyc_CfFlowInfo_BottomFL();_LL360: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*
_tmp64D=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp649;if(_tmp64D->tag
!= 3)goto _LL362;else{_tmp64E=_tmp64D->f1;}}_LL361: if(env->noreturn){const char*
_tmp979;void*_tmp978;(_tmp978=0,Cyc_Tcutil_terr(s->loc,((_tmp979="`noreturn' function might return",
_tag_dyneither(_tmp979,sizeof(char),33))),_tag_dyneither(_tmp978,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp692;void*_tmp693;struct _tuple14 _tmp691=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp64E));_tmp692=_tmp691.f1;
_tmp693=_tmp691.f2;_tmp692=Cyc_NewControlFlow_use_Rval(env,_tmp64E->loc,_tmp692,
_tmp693);Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp692);return Cyc_CfFlowInfo_BottomFL();};
_LL362: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp64F=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)
_tmp649;if(_tmp64F->tag != 1)goto _LL364;else{_tmp650=_tmp64F->f1;}}_LL363: return(
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp650)).f1;_LL364: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*
_tmp651=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp649;if(_tmp651->tag != 
2)goto _LL366;else{_tmp652=_tmp651->f1;_tmp653=_tmp651->f2;}}_LL365: return Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp652),_tmp653);_LL366: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*
_tmp654=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp649;if(_tmp654->tag
!= 4)goto _LL368;else{_tmp655=_tmp654->f1;_tmp656=_tmp654->f2;_tmp657=_tmp654->f3;}}
_LL367: {union Cyc_CfFlowInfo_FlowInfo _tmp695;union Cyc_CfFlowInfo_FlowInfo _tmp696;
struct _tuple16 _tmp694=Cyc_NewControlFlow_anal_test(env,inflow,_tmp655);_tmp695=
_tmp694.f1;_tmp696=_tmp694.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp697=Cyc_NewControlFlow_anal_stmt(
env,_tmp696,_tmp657);union Cyc_CfFlowInfo_FlowInfo _tmp698=Cyc_NewControlFlow_anal_stmt(
env,_tmp695,_tmp656);return Cyc_CfFlowInfo_join_flow(_tmp648,env->all_changed,
_tmp698,_tmp697);};}_LL368: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*
_tmp658=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp649;if(_tmp658->tag
!= 5)goto _LL36A;else{_tmp659=_tmp658->f1;_tmp65A=_tmp659.f1;_tmp65B=_tmp659.f2;
_tmp65C=_tmp658->f2;}}_LL369: {union Cyc_CfFlowInfo_FlowInfo*_tmp69A;struct
_tuple17 _tmp699=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp65B);_tmp69A=
_tmp699.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp69B=*_tmp69A;union Cyc_CfFlowInfo_FlowInfo
_tmp69D;union Cyc_CfFlowInfo_FlowInfo _tmp69E;struct _tuple16 _tmp69C=Cyc_NewControlFlow_anal_test(
env,_tmp69B,_tmp65A);_tmp69D=_tmp69C.f1;_tmp69E=_tmp69C.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp69F=Cyc_NewControlFlow_anal_stmt(env,_tmp69D,_tmp65C);Cyc_NewControlFlow_update_flow(
env,_tmp65B,_tmp69F);return _tmp69E;};};}_LL36A: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*
_tmp65D=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp649;if(_tmp65D->tag != 
14)goto _LL36C;else{_tmp65E=_tmp65D->f1;_tmp65F=_tmp65D->f2;_tmp660=_tmp65F.f1;
_tmp661=_tmp65F.f2;}}_LL36B: {union Cyc_CfFlowInfo_FlowInfo _tmp6A0=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp65E);union Cyc_CfFlowInfo_FlowInfo*_tmp6A2;struct _tuple17 _tmp6A1=
Cyc_NewControlFlow_pre_stmt_check(env,_tmp6A0,_tmp661);_tmp6A2=_tmp6A1.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp6A3=*_tmp6A2;union Cyc_CfFlowInfo_FlowInfo _tmp6A5;
union Cyc_CfFlowInfo_FlowInfo _tmp6A6;struct _tuple16 _tmp6A4=Cyc_NewControlFlow_anal_test(
env,_tmp6A3,_tmp660);_tmp6A5=_tmp6A4.f1;_tmp6A6=_tmp6A4.f2;Cyc_NewControlFlow_update_flow(
env,_tmp65E,_tmp6A5);return _tmp6A6;};}_LL36C: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*
_tmp662=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp649;if(_tmp662->tag != 
9)goto _LL36E;else{_tmp663=_tmp662->f1;_tmp664=_tmp662->f2;_tmp665=_tmp664.f1;
_tmp666=_tmp664.f2;_tmp667=_tmp662->f3;_tmp668=_tmp667.f1;_tmp669=_tmp667.f2;
_tmp66A=_tmp662->f4;}}_LL36D: {union Cyc_CfFlowInfo_FlowInfo _tmp6A7=(Cyc_NewControlFlow_anal_Rexp(
env,0,inflow,_tmp663)).f1;union Cyc_CfFlowInfo_FlowInfo*_tmp6A9;struct _tuple17
_tmp6A8=Cyc_NewControlFlow_pre_stmt_check(env,_tmp6A7,_tmp666);_tmp6A9=_tmp6A8.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6AA=*_tmp6A9;union Cyc_CfFlowInfo_FlowInfo _tmp6AC;
union Cyc_CfFlowInfo_FlowInfo _tmp6AD;struct _tuple16 _tmp6AB=Cyc_NewControlFlow_anal_test(
env,_tmp6AA,_tmp665);_tmp6AC=_tmp6AB.f1;_tmp6AD=_tmp6AB.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp6AE=Cyc_NewControlFlow_anal_stmt(env,_tmp6AC,_tmp66A);union Cyc_CfFlowInfo_FlowInfo*
_tmp6B0;struct _tuple17 _tmp6AF=Cyc_NewControlFlow_pre_stmt_check(env,_tmp6AE,
_tmp669);_tmp6B0=_tmp6AF.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp6B1=*_tmp6B0;union
Cyc_CfFlowInfo_FlowInfo _tmp6B2=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp6B1,
_tmp668)).f1;Cyc_NewControlFlow_update_flow(env,_tmp666,_tmp6B2);return _tmp6AD;};};};}
_LL36E: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp66B=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)
_tmp649;if(_tmp66B->tag != 6)goto _LL370;else{_tmp66C=_tmp66B->f1;if(_tmp66C != 0)
goto _LL370;}}_LL36F: return Cyc_CfFlowInfo_BottomFL();_LL370: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*
_tmp66D=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp649;if(_tmp66D->tag
!= 11)goto _LL372;else{_tmp66E=_tmp66D->f1;_tmp66F=_tmp66D->f2;if(_tmp66F == 0)
goto _LL372;_tmp670=*_tmp66F;}}_LL371: {struct _RegionHandle _tmp6B3=_new_region("temp");
struct _RegionHandle*temp=& _tmp6B3;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo
_tmp6B5;struct Cyc_List_List*_tmp6B6;struct _tuple20 _tmp6B4=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp66E,1,1);_tmp6B5=_tmp6B4.f1;_tmp6B6=_tmp6B4.f2;for(0;_tmp6B6
!= 0;(_tmp6B6=_tmp6B6->tl,_tmp66E=_tmp66E->tl)){_tmp6B5=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp66E))->hd)->loc,
_tmp6B5,(void*)_tmp6B6->hd);}_tmp6B5=Cyc_NewControlFlow_add_vars(_tmp648,_tmp6B5,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp670->pat_vars))->v,
_tmp648->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp670->body,_tmp6B5);{union Cyc_CfFlowInfo_FlowInfo _tmp6B7=Cyc_CfFlowInfo_BottomFL();
_npop_handler(0);return _tmp6B7;};};_pop_region(temp);}_LL372: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*
_tmp671=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp649;if(_tmp671->tag
!= 6)goto _LL374;else{_tmp672=_tmp671->f1;}}_LL373: _tmp674=_tmp672;goto _LL375;
_LL374: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp673=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)
_tmp649;if(_tmp673->tag != 7)goto _LL376;else{_tmp674=_tmp673->f1;}}_LL375: _tmp676=
_tmp674;goto _LL377;_LL376: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp675=(
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp649;if(_tmp675->tag != 8)goto
_LL378;else{_tmp676=_tmp675->f2;}}_LL377: if(env->iteration_num == 1){struct Cyc_Absyn_Stmt*
_tmp6B8=_tmp646->encloser;struct Cyc_Absyn_Stmt*_tmp6B9=(Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)_check_null(_tmp676)))->encloser;while(_tmp6B9 != _tmp6B8){
struct Cyc_Absyn_Stmt*_tmp6BA=(Cyc_NewControlFlow_get_stmt_annot(_tmp6B8))->encloser;
if(_tmp6BA == _tmp6B8){{const char*_tmp97C;void*_tmp97B;(_tmp97B=0,Cyc_Tcutil_terr(
s->loc,((_tmp97C="goto enters local scope or exception handler",_tag_dyneither(
_tmp97C,sizeof(char),45))),_tag_dyneither(_tmp97B,sizeof(void*),0)));}break;}
_tmp6B8=_tmp6BA;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_check_null(_tmp676),inflow);return Cyc_CfFlowInfo_BottomFL();_LL378: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*
_tmp677=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp649;if(_tmp677->tag
!= 10)goto _LL37A;else{_tmp678=_tmp677->f1;_tmp679=_tmp677->f2;}}_LL379: {union
Cyc_CfFlowInfo_FlowInfo _tmp6BE;void*_tmp6BF;struct _tuple14 _tmp6BD=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp678);_tmp6BE=_tmp6BD.f1;_tmp6BF=_tmp6BD.f2;_tmp6BE=Cyc_NewControlFlow_use_Rval(
env,_tmp678->loc,_tmp6BE,_tmp6BF);return Cyc_NewControlFlow_anal_scs(env,_tmp6BE,
_tmp679);}_LL37A: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp67A=(
struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp649;if(_tmp67A->tag != 15)
goto _LL37C;else{_tmp67B=_tmp67A->f1;_tmp67C=_tmp67A->f2;}}_LL37B: {int old_in_try=
env->in_try;union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;env->in_try=1;
env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp67B);union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;env->in_try=
old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp67C);{union Cyc_CfFlowInfo_FlowInfo _tmp6C0=scs_outflow;int
_tmp6C1;_LL387: if((_tmp6C0.BottomFL).tag != 1)goto _LL389;_tmp6C1=(int)(_tmp6C0.BottomFL).val;
_LL388: goto _LL386;_LL389:;_LL38A: {const char*_tmp97F;void*_tmp97E;(_tmp97E=0,Cyc_Tcutil_terr(
s->loc,((_tmp97F="last catch clause may implicitly fallthru",_tag_dyneither(
_tmp97F,sizeof(char),42))),_tag_dyneither(_tmp97E,sizeof(void*),0)));}_LL386:;}
outflow=s1_outflow;return outflow;};};}_LL37C: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*
_tmp67D=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp649;if(_tmp67D->tag != 
12)goto _LL37E;else{_tmp67E=_tmp67D->f1;_tmp67F=*_tmp67E;_tmp680=_tmp67F.r;{
struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp681=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)
_tmp680;if(_tmp681->tag != 5)goto _LL37E;else{_tmp682=_tmp681->f1;_tmp683=_tmp681->f2;
_tmp684=_tmp681->f3;}};_tmp685=_tmp67D->f2;}}_LL37D: {union Cyc_CfFlowInfo_FlowInfo
_tmp6C5;union Cyc_CfFlowInfo_AbsLVal _tmp6C6;struct _tuple15 _tmp6C4=Cyc_NewControlFlow_anal_Lexp(
env,inflow,1,_tmp682);_tmp6C5=_tmp6C4.f1;_tmp6C6=_tmp6C4.f2;{struct _tuple15
_tmp980;struct _tuple15 _tmp6C8=(_tmp980.f1=_tmp6C5,((_tmp980.f2=_tmp6C6,_tmp980)));
union Cyc_CfFlowInfo_FlowInfo _tmp6C9;struct _tuple13 _tmp6CA;struct Cyc_Dict_Dict
_tmp6CB;struct Cyc_List_List*_tmp6CC;union Cyc_CfFlowInfo_AbsLVal _tmp6CD;struct Cyc_CfFlowInfo_Place*
_tmp6CE;union Cyc_CfFlowInfo_FlowInfo _tmp6CF;int _tmp6D0;_LL38C: _tmp6C9=_tmp6C8.f1;
if((_tmp6C9.ReachableFL).tag != 2)goto _LL38E;_tmp6CA=(struct _tuple13)(_tmp6C9.ReachableFL).val;
_tmp6CB=_tmp6CA.f1;_tmp6CC=_tmp6CA.f2;_tmp6CD=_tmp6C8.f2;if((_tmp6CD.PlaceL).tag
!= 1)goto _LL38E;_tmp6CE=(struct Cyc_CfFlowInfo_Place*)(_tmp6CD.PlaceL).val;_LL38D: {
void*_tmp6D1=Cyc_CfFlowInfo_lookup_place(_tmp6CB,_tmp6CE);void*t=(void*)
_check_null(_tmp682->topt);{void*_tmp6D2=_tmp682->r;struct Cyc_Absyn_Exp*_tmp6D4;
_LL393: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp6D3=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)
_tmp6D2;if(_tmp6D3->tag != 13)goto _LL395;else{_tmp6D4=_tmp6D3->f2;}}_LL394: t=(
void*)_check_null(_tmp6D4->topt);goto _LL392;_LL395:;_LL396: goto _LL392;_LL392:;}{
void*_tmp6D5=Cyc_CfFlowInfo_make_unique_consumed(_tmp648,t,_tmp682,env->iteration_num,
_tmp6D1);_tmp6CB=Cyc_CfFlowInfo_assign_place(_tmp648,_tmp682->loc,_tmp6CB,env->all_changed,
_tmp6CE,_tmp6D5);_tmp6C5=Cyc_CfFlowInfo_ReachableFL(_tmp6CB,_tmp6CC);{void*leaf;
switch(Cyc_CfFlowInfo_initlevel(_tmp648,_tmp6CB,_tmp6D1)){case Cyc_CfFlowInfo_AllIL:
_LL397: leaf=_tmp648->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL398: leaf=
_tmp648->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL399: leaf=_tmp648->unknown_this;
break;}{void*_tmp6D6=Cyc_CfFlowInfo_typ_to_absrval(_tmp648,t,0,leaf);_tmp6C5=Cyc_NewControlFlow_use_Rval(
env,_tmp682->loc,_tmp6C5,_tmp6D1);{struct Cyc_List_List _tmp981;struct Cyc_List_List
_tmp6D7=(_tmp981.hd=_tmp684,((_tmp981.tl=0,_tmp981)));_tmp6C5=Cyc_NewControlFlow_add_vars(
_tmp648,_tmp6C5,(struct Cyc_List_List*)& _tmp6D7,_tmp648->unknown_all,s->loc);
_tmp6C5=Cyc_NewControlFlow_anal_stmt(env,_tmp6C5,_tmp685);{union Cyc_CfFlowInfo_FlowInfo
_tmp6D8=_tmp6C5;struct _tuple13 _tmp6D9;struct Cyc_Dict_Dict _tmp6DA;struct Cyc_List_List*
_tmp6DB;_LL39C: if((_tmp6D8.ReachableFL).tag != 2)goto _LL39E;_tmp6D9=(struct
_tuple13)(_tmp6D8.ReachableFL).val;_tmp6DA=_tmp6D9.f1;_tmp6DB=_tmp6D9.f2;_LL39D:
_tmp6DA=Cyc_CfFlowInfo_assign_place(_tmp648,s->loc,_tmp6DA,env->all_changed,
_tmp6CE,_tmp6D6);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp6DA,_tmp6DB);return
outflow;_LL39E:;_LL39F: return _tmp6C5;_LL39B:;};};};};};}_LL38E: _tmp6CF=_tmp6C8.f1;
if((_tmp6CF.BottomFL).tag != 1)goto _LL390;_tmp6D0=(int)(_tmp6CF.BottomFL).val;
_LL38F: return _tmp6C5;_LL390:;_LL391:{const char*_tmp984;void*_tmp983;(_tmp983=0,
Cyc_Tcutil_terr(_tmp682->loc,((_tmp984="bad alias expression--no unique path found",
_tag_dyneither(_tmp984,sizeof(char),43))),_tag_dyneither(_tmp983,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL38B:;};}_LL37E: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*
_tmp686=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp649;if(_tmp686->tag != 
12)goto _LL380;else{_tmp687=_tmp686->f1;_tmp688=_tmp686->f2;}}_LL37F: outflow=Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp687),_tmp688);if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp687);return outflow;
_LL380: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp689=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)
_tmp649;if(_tmp689->tag != 13)goto _LL382;else{_tmp68A=_tmp689->f2;}}_LL381: return
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp68A);_LL382: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*
_tmp68B=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp649;if(_tmp68B->tag
!= 16)goto _LL384;else{_tmp68C=_tmp68B->f1;}}_LL383: {union Cyc_CfFlowInfo_FlowInfo
_tmp6E0;void*_tmp6E1;struct _tuple14 _tmp6DF=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,_tmp68C);_tmp6E0=_tmp6DF.f1;_tmp6E1=_tmp6DF.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp6E2=Cyc_NewControlFlow_use_Rval(env,_tmp68C->loc,_tmp6E0,_tmp6E1);void*
_tmp6E3=Cyc_Tcutil_compress((void*)_check_null(_tmp68C->topt));void*_tmp6E5;
_LL3A1: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp6E4=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)
_tmp6E3;if(_tmp6E4->tag != 15)goto _LL3A3;else{_tmp6E5=(void*)_tmp6E4->f1;}}_LL3A2:
return Cyc_CfFlowInfo_kill_flow_region(_tmp648,_tmp6E0,_tmp6E5);_LL3A3:;_LL3A4: {
struct Cyc_Core_Impossible_exn_struct _tmp98A;const char*_tmp989;struct Cyc_Core_Impossible_exn_struct*
_tmp988;(int)_throw((void*)((_tmp988=_cycalloc(sizeof(*_tmp988)),((_tmp988[0]=((
_tmp98A.tag=Cyc_Core_Impossible,((_tmp98A.f1=((_tmp989="anal_stmt -- reset_region",
_tag_dyneither(_tmp989,sizeof(char),26))),_tmp98A)))),_tmp988)))));}_LL3A0:;};}
_LL384:;_LL385: {struct Cyc_Core_Impossible_exn_struct _tmp990;const char*_tmp98F;
struct Cyc_Core_Impossible_exn_struct*_tmp98E;(int)_throw((void*)((_tmp98E=
_cycalloc(sizeof(*_tmp98E)),((_tmp98E[0]=((_tmp990.tag=Cyc_Core_Impossible,((
_tmp990.f1=((_tmp98F="anal_stmt -- bad stmt syntax or unimplemented stmt form",
_tag_dyneither(_tmp98F,sizeof(char),56))),_tmp990)))),_tmp98E)))));}_LL35B:;};}
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,
union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_Absyn_Decl*decl){struct Cyc_CfFlowInfo_FlowEnv*_tmp6EC=env->fenv;
void*_tmp6ED=decl->r;struct Cyc_Absyn_Vardecl*_tmp6EF;struct Cyc_Core_Opt*_tmp6F1;
struct Cyc_Core_Opt _tmp6F2;struct Cyc_List_List*_tmp6F3;struct Cyc_Absyn_Exp*
_tmp6F4;struct Cyc_List_List*_tmp6F6;struct Cyc_Absyn_Fndecl*_tmp6F8;struct Cyc_Absyn_Tvar*
_tmp6FA;struct Cyc_Absyn_Vardecl*_tmp6FB;int _tmp6FC;struct Cyc_Absyn_Exp*_tmp6FD;
_LL3A6: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6EE=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)
_tmp6ED;if(_tmp6EE->tag != 0)goto _LL3A8;else{_tmp6EF=_tmp6EE->f1;}}_LL3A7: {
struct Cyc_List_List _tmp991;struct Cyc_List_List _tmp6FE=(_tmp991.hd=_tmp6EF,((
_tmp991.tl=0,_tmp991)));inflow=Cyc_NewControlFlow_add_vars(_tmp6EC,inflow,(
struct Cyc_List_List*)& _tmp6FE,_tmp6EC->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*
_tmp6FF=_tmp6EF->initializer;if(_tmp6FF == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo
_tmp701;void*_tmp702;struct _tuple14 _tmp700=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,(struct Cyc_Absyn_Exp*)_tmp6FF);_tmp701=_tmp700.f1;_tmp702=_tmp700.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp703=_tmp701;int _tmp704;struct _tuple13 _tmp705;
struct Cyc_Dict_Dict _tmp706;struct Cyc_List_List*_tmp707;_LL3B3: if((_tmp703.BottomFL).tag
!= 1)goto _LL3B5;_tmp704=(int)(_tmp703.BottomFL).val;_LL3B4: return Cyc_CfFlowInfo_BottomFL();
_LL3B5: if((_tmp703.ReachableFL).tag != 2)goto _LL3B2;_tmp705=(struct _tuple13)(
_tmp703.ReachableFL).val;_tmp706=_tmp705.f1;_tmp707=_tmp705.f2;_LL3B6:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*
_tmp997;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp996;struct Cyc_CfFlowInfo_Place*
_tmp995;_tmp706=Cyc_CfFlowInfo_assign_place(_tmp6EC,decl->loc,_tmp706,env->all_changed,((
_tmp995=_region_malloc(env->r,sizeof(*_tmp995)),((_tmp995->root=(void*)((_tmp997=
_region_malloc(env->r,sizeof(*_tmp997)),((_tmp997[0]=((_tmp996.tag=0,((_tmp996.f1=
_tmp6EF,_tmp996)))),_tmp997)))),((_tmp995->fields=0,_tmp995)))))),_tmp702);}
_tmp707=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp707,_tmp6EF,(struct Cyc_Absyn_Exp*)
_check_null(_tmp6EF->initializer));{union Cyc_CfFlowInfo_FlowInfo _tmp70B=Cyc_CfFlowInfo_ReachableFL(
_tmp706,_tmp707);Cyc_NewControlFlow_update_tryflow(env,_tmp70B);return _tmp70B;};
_LL3B2:;};};};}_LL3A8: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp6F0=(
struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp6ED;if(_tmp6F0->tag != 2)goto
_LL3AA;else{_tmp6F1=_tmp6F0->f2;if(_tmp6F1 == 0)goto _LL3AA;_tmp6F2=*_tmp6F1;
_tmp6F3=(struct Cyc_List_List*)_tmp6F2.v;_tmp6F4=_tmp6F0->f3;}}_LL3A9: {union Cyc_CfFlowInfo_FlowInfo
_tmp70E;void*_tmp70F;struct _tuple14 _tmp70D=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,_tmp6F4);_tmp70E=_tmp70D.f1;_tmp70F=_tmp70D.f2;_tmp70E=Cyc_NewControlFlow_use_Rval(
env,_tmp6F4->loc,_tmp70E,_tmp70F);return Cyc_NewControlFlow_add_vars(_tmp6EC,
_tmp70E,_tmp6F3,_tmp6EC->unknown_all,decl->loc);}_LL3AA: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*
_tmp6F5=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp6ED;if(_tmp6F5->tag != 
3)goto _LL3AC;else{_tmp6F6=_tmp6F5->f1;}}_LL3AB: return Cyc_NewControlFlow_add_vars(
_tmp6EC,inflow,_tmp6F6,_tmp6EC->unknown_none,decl->loc);_LL3AC: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*
_tmp6F7=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6ED;if(_tmp6F7->tag != 1)
goto _LL3AE;else{_tmp6F8=_tmp6F7->f1;}}_LL3AD: Cyc_NewControlFlow_check_nested_fun(
_tmp6EC,inflow,_tmp6F8);{void*t=(void*)_check_null(_tmp6F8->cached_typ);struct
Cyc_Absyn_Vardecl*_tmp710=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp6F8->fn_vardecl);
struct Cyc_List_List*_tmp998;return Cyc_NewControlFlow_add_vars(_tmp6EC,inflow,((
_tmp998=_region_malloc(env->r,sizeof(*_tmp998)),((_tmp998->hd=_tmp710,((_tmp998->tl=
0,_tmp998)))))),_tmp6EC->unknown_all,decl->loc);};_LL3AE: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*
_tmp6F9=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp6ED;if(_tmp6F9->tag
!= 4)goto _LL3B0;else{_tmp6FA=_tmp6F9->f1;_tmp6FB=_tmp6F9->f2;_tmp6FC=_tmp6F9->f3;
_tmp6FD=_tmp6F9->f4;}}_LL3AF: if(_tmp6FD != 0){struct Cyc_Absyn_Exp*_tmp712=(struct
Cyc_Absyn_Exp*)_tmp6FD;union Cyc_CfFlowInfo_FlowInfo _tmp714;void*_tmp715;struct
_tuple14 _tmp713=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp712);_tmp714=
_tmp713.f1;_tmp715=_tmp713.f2;inflow=Cyc_NewControlFlow_use_Rval(env,_tmp712->loc,
_tmp714,_tmp715);}{struct Cyc_List_List _tmp999;struct Cyc_List_List _tmp716=(
_tmp999.hd=_tmp6FB,((_tmp999.tl=0,_tmp999)));return Cyc_NewControlFlow_add_vars(
_tmp6EC,inflow,(struct Cyc_List_List*)& _tmp716,_tmp6EC->unknown_all,decl->loc);};
_LL3B0:;_LL3B1: {struct Cyc_Core_Impossible_exn_struct _tmp99F;const char*_tmp99E;
struct Cyc_Core_Impossible_exn_struct*_tmp99D;(int)_throw((void*)((_tmp99D=
_cycalloc(sizeof(*_tmp99D)),((_tmp99D[0]=((_tmp99F.tag=Cyc_Core_Impossible,((
_tmp99F.f1=((_tmp99E="anal_decl: unexpected decl variant",_tag_dyneither(_tmp99E,
sizeof(char),35))),_tmp99F)))),_tmp99D)))));}_LL3A5:;}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd){struct _RegionHandle _tmp71B=_new_region("frgn");struct
_RegionHandle*frgn=& _tmp71B;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*
fenv=Cyc_CfFlowInfo_new_flow_env(frgn);Cyc_NewControlFlow_check_nested_fun(fenv,
Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);};_pop_region(frgn);}static
int Cyc_NewControlFlow_hash_ptr(void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle*_tmp71C=fenv->r;unsigned int _tmp71D=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp71D);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo _tmp71E=inflow;int _tmp71F;struct
_tuple13 _tmp720;struct Cyc_Dict_Dict _tmp721;struct Cyc_List_List*_tmp722;_LL3B8:
if((_tmp71E.BottomFL).tag != 1)goto _LL3BA;_tmp71F=(int)(_tmp71E.BottomFL).val;
_LL3B9: {const char*_tmp9A2;void*_tmp9A1;(_tmp9A1=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9A2="check_fun",
_tag_dyneither(_tmp9A2,sizeof(char),10))),_tag_dyneither(_tmp9A1,sizeof(void*),0)));}
_LL3BA: if((_tmp71E.ReachableFL).tag != 2)goto _LL3B7;_tmp720=(struct _tuple13)(
_tmp71E.ReachableFL).val;_tmp721=_tmp720.f1;_tmp722=_tmp720.f2;_LL3BB: {struct
Cyc_List_List*atts;{void*_tmp725=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));
struct Cyc_Absyn_FnInfo _tmp727;struct Cyc_List_List*_tmp728;_LL3BD: {struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp726=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp725;if(_tmp726->tag != 9)
goto _LL3BF;else{_tmp727=_tmp726->f1;_tmp728=_tmp727.attributes;}}_LL3BE: atts=
_tmp728;goto _LL3BC;_LL3BF:;_LL3C0: {const char*_tmp9A5;void*_tmp9A4;(_tmp9A4=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp9A5="check_fun: non-function type cached with fndecl_t",_tag_dyneither(
_tmp9A5,sizeof(char),50))),_tag_dyneither(_tmp9A4,sizeof(void*),0)));}_LL3BC:;}
for(0;atts != 0;atts=atts->tl){void*_tmp72B=(void*)atts->hd;int _tmp72D;int _tmp72F;
_LL3C2: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp72C=(struct
Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp72B;if(_tmp72C->tag != 21)
goto _LL3C4;else{_tmp72D=_tmp72C->f1;}}_LL3C3: {unsigned int j=(unsigned int)
_tmp72D;struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);if(j > ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v)){{const char*_tmp9A8;void*_tmp9A7;(_tmp9A7=0,
Cyc_Tcutil_terr(_tmp71D,((_tmp9A8="noliveunique attribute exceeds number of parameters",
_tag_dyneither(_tmp9A8,sizeof(char),52))),_tag_dyneither(_tmp9A7,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp732=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));void*t=Cyc_Tcutil_compress(
_tmp732->type);if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp9AB;void*
_tmp9AA;(_tmp9AA=0,Cyc_Tcutil_terr(_tmp71D,((_tmp9AB="noliveunique attribute requires unique pointer type",
_tag_dyneither(_tmp9AB,sizeof(char),52))),_tag_dyneither(_tmp9AA,sizeof(void*),0)));}
continue;}{void*elttype=Cyc_Tcutil_pointer_elt_type(t);void*_tmp735=Cyc_CfFlowInfo_make_unique_consumed(
fenv,elttype,bogus_exp,- 1,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp9AE;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*
_tmp9AD;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp736=(_tmp9AD=
_region_malloc(_tmp71C,sizeof(*_tmp9AD)),((_tmp9AD[0]=((_tmp9AE.tag=2,((_tmp9AE.f1=(
int)j,((_tmp9AE.f2=(void*)t,_tmp9AE)))))),_tmp9AD)));struct Cyc_CfFlowInfo_Place*
_tmp9AF;struct Cyc_CfFlowInfo_Place*_tmp737=(_tmp9AF=_region_malloc(_tmp71C,
sizeof(*_tmp9AF)),((_tmp9AF->root=(void*)_tmp736,((_tmp9AF->fields=0,_tmp9AF)))));
_tmp721=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp721,(void*)_tmp736,_tmp735);{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct
_tmp9B5;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp9B4;struct
Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp9B2;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*
_tmp9B1;_tmp721=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp721,(void*)((_tmp9B1=_region_malloc(_tmp71C,sizeof(*_tmp9B1)),((_tmp9B1[0]=((
_tmp9B2.tag=0,((_tmp9B2.f1=_tmp732,_tmp9B2)))),_tmp9B1)))),(void*)((_tmp9B4=
_region_malloc(_tmp71C,sizeof(*_tmp9B4)),((_tmp9B4[0]=((_tmp9B5.tag=5,((_tmp9B5.f1=
_tmp737,_tmp9B5)))),_tmp9B4)))));}goto _LL3C1;};};}_LL3C4: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*
_tmp72E=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp72B;if(
_tmp72E->tag != 20)goto _LL3C6;else{_tmp72F=_tmp72E->f1;}}_LL3C5: {unsigned int j=(
unsigned int)_tmp72F;if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){{
const char*_tmp9B8;void*_tmp9B7;(_tmp9B7=0,Cyc_Tcutil_terr(_tmp71D,((_tmp9B8="initializes attribute exceeds number of parameters",
_tag_dyneither(_tmp9B8,sizeof(char),51))),_tag_dyneither(_tmp9B7,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp741=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*_tmp742=Cyc_Tcutil_compress(
_tmp741->type);struct Cyc_Absyn_PtrInfo _tmp744;void*_tmp745;struct Cyc_Absyn_PtrAtts
_tmp746;union Cyc_Absyn_Constraint*_tmp747;union Cyc_Absyn_Constraint*_tmp748;
union Cyc_Absyn_Constraint*_tmp749;_LL3C9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp743=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp742;if(_tmp743->tag
!= 5)goto _LL3CB;else{_tmp744=_tmp743->f1;_tmp745=_tmp744.elt_typ;_tmp746=_tmp744.ptr_atts;
_tmp747=_tmp746.nullable;_tmp748=_tmp746.bounds;_tmp749=_tmp746.zero_term;}}
_LL3CA: if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp747)){
const char*_tmp9BB;void*_tmp9BA;(_tmp9BA=0,Cyc_Tcutil_terr(_tmp71D,((_tmp9BB="initializes attribute not allowed on nullable pointers",
_tag_dyneither(_tmp9BB,sizeof(char),55))),_tag_dyneither(_tmp9BA,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_bound_one(_tmp748)){const char*_tmp9BE;void*_tmp9BD;(_tmp9BD=0,
Cyc_Tcutil_terr(_tmp71D,((_tmp9BE="initializes attribute allowed only on pointers of size 1",
_tag_dyneither(_tmp9BE,sizeof(char),57))),_tag_dyneither(_tmp9BD,sizeof(void*),0)));}
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp749)){
const char*_tmp9C1;void*_tmp9C0;(_tmp9C0=0,Cyc_Tcutil_terr(_tmp71D,((_tmp9C1="initializes attribute allowed only on pointers to non-zero-terminated arrays",
_tag_dyneither(_tmp9C1,sizeof(char),77))),_tag_dyneither(_tmp9C0,sizeof(void*),0)));}{
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp9C4;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*
_tmp9C3;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp750=(_tmp9C3=
_region_malloc(_tmp71C,sizeof(*_tmp9C3)),((_tmp9C3[0]=((_tmp9C4.tag=2,((_tmp9C4.f1=(
int)j,((_tmp9C4.f2=(void*)_tmp745,_tmp9C4)))))),_tmp9C3)));struct Cyc_CfFlowInfo_Place*
_tmp9C5;struct Cyc_CfFlowInfo_Place*_tmp751=(_tmp9C5=_region_malloc(_tmp71C,
sizeof(*_tmp9C5)),((_tmp9C5->root=(void*)_tmp750,((_tmp9C5->fields=0,_tmp9C5)))));
_tmp721=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp721,(void*)_tmp750,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp745,0,fenv->esc_none));{
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp9CB;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*
_tmp9CA;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp9C8;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*
_tmp9C7;_tmp721=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp721,(void*)((_tmp9C7=_region_malloc(_tmp71C,sizeof(*_tmp9C7)),((_tmp9C7[0]=((
_tmp9C8.tag=0,((_tmp9C8.f1=_tmp741,_tmp9C8)))),_tmp9C7)))),(void*)((_tmp9CA=
_region_malloc(_tmp71C,sizeof(*_tmp9CA)),((_tmp9CA[0]=((_tmp9CB.tag=5,((_tmp9CB.f1=
_tmp751,_tmp9CB)))),_tmp9CA)))));}{struct Cyc_List_List*_tmp9CC;param_roots=((
_tmp9CC=_region_malloc(_tmp71C,sizeof(*_tmp9CC)),((_tmp9CC->hd=_tmp751,((_tmp9CC->tl=
param_roots,_tmp9CC))))));}goto _LL3C8;};_LL3CB:;_LL3CC: {const char*_tmp9CF;void*
_tmp9CE;(_tmp9CE=0,Cyc_Tcutil_terr(_tmp71D,((_tmp9CF="initializes attribute on non-pointer",
_tag_dyneither(_tmp9CF,sizeof(char),37))),_tag_dyneither(_tmp9CE,sizeof(void*),0)));}
_LL3C8:;}goto _LL3C1;};}_LL3C6:;_LL3C7: goto _LL3C1;_LL3C1:;}inflow=Cyc_CfFlowInfo_ReachableFL(
_tmp721,_tmp722);}_LL3B7:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(struct
_RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*
hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp71C,33,(int(*)(struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*_tmp9D0;struct
Cyc_NewControlFlow_AnalEnv*env=(_tmp9D0=_region_malloc(_tmp71C,sizeof(*_tmp9D0)),((
_tmp9D0->r=_tmp71C,((_tmp9D0->fenv=fenv,((_tmp9D0->iterate_again=1,((_tmp9D0->iteration_num=
0,((_tmp9D0->in_try=0,((_tmp9D0->tryflow=inflow,((_tmp9D0->all_changed=0,((
_tmp9D0->noreturn=noreturn,((_tmp9D0->param_roots=param_roots,((_tmp9D0->flow_table=
flow_table,_tmp9D0)))))))))))))))))))));union Cyc_CfFlowInfo_FlowInfo outflow=
inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);}{
union Cyc_CfFlowInfo_FlowInfo _tmp75C=outflow;int _tmp75D;_LL3CE: if((_tmp75C.BottomFL).tag
!= 1)goto _LL3D0;_tmp75D=(int)(_tmp75C.BottomFL).val;_LL3CF: goto _LL3CD;_LL3D0:;
_LL3D1: Cyc_NewControlFlow_check_init_params(_tmp71D,env,outflow);if(noreturn){
const char*_tmp9D3;void*_tmp9D2;(_tmp9D2=0,Cyc_Tcutil_terr(_tmp71D,((_tmp9D3="`noreturn' function might (implicitly) return",
_tag_dyneither(_tmp9D3,sizeof(char),46))),_tag_dyneither(_tmp9D2,sizeof(void*),0)));}
else{void*_tmp760=Cyc_Tcutil_compress(fd->ret_type);_LL3D3: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*
_tmp761=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp760;if(_tmp761->tag != 0)
goto _LL3D5;}_LL3D4: goto _LL3D2;_LL3D5: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp762=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp760;if(_tmp762->tag != 
7)goto _LL3D7;}_LL3D6: goto _LL3D8;_LL3D7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp763=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp760;if(_tmp763->tag != 6)
goto _LL3D9;}_LL3D8:{const char*_tmp9D6;void*_tmp9D5;(_tmp9D5=0,Cyc_Tcutil_warn(
_tmp71D,((_tmp9D6="function may not return a value",_tag_dyneither(_tmp9D6,
sizeof(char),32))),_tag_dyneither(_tmp9D5,sizeof(void*),0)));}goto _LL3D2;_LL3D9:;
_LL3DA:{const char*_tmp9D9;void*_tmp9D8;(_tmp9D8=0,Cyc_Tcutil_terr(_tmp71D,((
_tmp9D9="function may not return a value",_tag_dyneither(_tmp9D9,sizeof(char),32))),
_tag_dyneither(_tmp9D8,sizeof(void*),0)));}goto _LL3D2;_LL3D2:;}goto _LL3CD;_LL3CD:;};};};}
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){
void*_tmp769=((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp76B;
struct Cyc_List_List*_tmp76D;struct Cyc_List_List*_tmp76F;struct Cyc_List_List*
_tmp771;_LL3DC: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp76A=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)
_tmp769;if(_tmp76A->tag != 1)goto _LL3DE;else{_tmp76B=_tmp76A->f1;}}_LL3DD: Cyc_NewControlFlow_check_fun(
_tmp76B);goto _LL3DB;_LL3DE: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*
_tmp76C=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp769;if(_tmp76C->tag
!= 12)goto _LL3E0;else{_tmp76D=_tmp76C->f1;}}_LL3DF: _tmp76F=_tmp76D;goto _LL3E1;
_LL3E0: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp76E=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)
_tmp769;if(_tmp76E->tag != 11)goto _LL3E2;else{_tmp76F=_tmp76E->f2;}}_LL3E1:
_tmp771=_tmp76F;goto _LL3E3;_LL3E2: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*
_tmp770=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp769;if(_tmp770->tag
!= 10)goto _LL3E4;else{_tmp771=_tmp770->f2;}}_LL3E3: Cyc_NewControlFlow_cf_check(
_tmp771);goto _LL3DB;_LL3E4: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*
_tmp772=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp769;if(
_tmp772->tag != 13)goto _LL3E6;}_LL3E5: goto _LL3DB;_LL3E6:;_LL3E7: goto _LL3DB;_LL3DB:;}}
