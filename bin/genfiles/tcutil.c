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
Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int
tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{
int tag;struct _dyneither_ptr f1;};typedef void*Cyc_sarg_t;int Cyc_printf(struct
_dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_vrprintf(struct
_RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[
15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[
14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};typedef
unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};void*Cyc_Core_fst(struct _tuple0*);
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{
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
Cyc_List_list(struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);
void*Cyc_List_hd(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_tl(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{
char*tag;};struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y);void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_Nth[
4];struct Cyc_List_Nth_exn_struct{char*tag;};void*Cyc_List_nth(struct Cyc_List_List*
x,int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};struct _tuple1 Cyc_List_rsplit(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x);int Cyc_List_mem(int(*compare)(void*,
void*),struct Cyc_List_List*l,void*x);void*Cyc_List_assoc_cmp(int(*cmp)(void*,
void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*cmp)(void*,void*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};typedef
struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{
char*tag;};typedef unsigned int Cyc_Position_seg_t;struct _dyneither_ptr Cyc_Position_string_of_segment(
unsigned int);struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);
struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{
int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};typedef void*
Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;
unsigned int seg;void*kind;struct _dyneither_ptr desc;};typedef struct Cyc_Position_Error*
Cyc_Position_error_t;struct Cyc_Position_Error*Cyc_Position_mk_err_elab(
unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct
Cyc_Position_Nocontext_exn_struct{char*tag;};extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;void Cyc_Position_post_error(struct Cyc_Position_Error*);
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;typedef struct _dyneither_ptr*
Cyc_Absyn_var_t;typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;typedef struct
_dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n
Loc_n;};typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct
_tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};typedef struct _tuple2*
Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;typedef struct _tuple2*
Cyc_Absyn_typedef_name_t;typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
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
_tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int
tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{
struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);
struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;
struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct
_tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;
struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**
val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;
struct _union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(
enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);union Cyc_Absyn_AggrInfoU
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
struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{
int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;
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
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{
int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};
struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts
f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int
tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{
int tag;unsigned int f1;struct Cyc_List_List*f2;};typedef void*Cyc_Absyn_type_modifier_t;
struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;
char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct
_union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};
struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct
_union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;
struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct
_dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct
_union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};extern union
Cyc_Absyn_Cnst Cyc_Absyn_Null_c;enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times
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
f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;
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
int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct
_tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};
struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{
int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;
};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*
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
struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{
int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*
f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int
tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{
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
int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int
tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;
unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;
struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;
};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual
ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;
struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;
struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind
kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct
_tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};
struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{
struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{
enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{
int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{
void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{
int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{
char*tag;};int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
unsigned int);union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);union Cyc_Absyn_Constraint*
Cyc_Absyn_empty_conref();union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(
union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_constr(
void*y,union Cyc_Absyn_Constraint*x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_bounds_one_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_empty_effect;
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,union Cyc_Absyn_Constraint*zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*
rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple2*,void*,unsigned int);struct
Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct
Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(
void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_offsetof_exp(void*,void*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(
struct Cyc_List_List*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(
void*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct
_dyneither_ptr body,unsigned int);struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*
init);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual
f1;void*f2;};struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,
int);struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(
void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};struct
_tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;typedef struct Cyc_PP_Out*
Cyc_PP_out_t;struct Cyc_PP_Doc;typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{
int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int
rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;
int print_using_stmts;int print_externC_stmts;int print_full_evars;int
print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};typedef struct Cyc_Iter_Iter
Cyc_Iter_iter_t;int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{
char*tag;};struct Cyc_Dict_T;typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};typedef
struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*
tag;};struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;void*Cyc_Tcenv_lookup_ordinary(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);struct Cyc_Absyn_Aggrdecl**
Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,
unsigned int,struct _tuple2*);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(
struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);struct Cyc_Absyn_Typedefdecl*
Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*
Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*,void*r1,void*r2);unsigned long Cyc_strlen(struct
_dyneither_ptr s);int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct
_dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);struct
_tuple14{unsigned int f1;int f2;};struct _tuple14 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(
unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(
unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_flush_warnings();
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;void*Cyc_Tcutil_copy_type(void*
t);struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_kind_leq(
struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(
void*t);int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,
void*,struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple15{struct Cyc_Absyn_Decl*
f1;struct Cyc_Absyn_Exp*f2;};struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*
e,void*e_typ);extern int Cyc_Tcutil_warn_alias_coerce;extern int Cyc_Tcutil_warn_region_coerce;
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_is_function_type(void*t);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_pointer_region(
void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);void*Cyc_Tcutil_kind_to_bound(
struct Cyc_Absyn_Kind*k);int Cyc_Tcutil_unify_kindbound(void*,void*);struct
_tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple16 Cyc_Tcutil_swap_kind(
void*t,void*kb);int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct
Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();
int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,
void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*
Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple16*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple17{struct Cyc_List_List*
f1;struct _RegionHandle*f2;};struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct
_tuple17*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_bitfield(unsigned int loc,
struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct
_dyneither_ptr err_msg);void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);void
Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*
is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_region(
void*r,int must_be_unique);int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*,void*t);void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*,
void*t,struct Cyc_Absyn_Exp*e);struct _tuple18{int f1;void*f2;};struct _tuple18 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_normalize_effect(
void*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int Cyc_Tcutil_new_tvar_id();
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*);int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_snd_tqt(struct _tuple12*);int Cyc_Tcutil_supports_default(
void*);int Cyc_Tcutil_admits_zero(void*t);int Cyc_Tcutil_is_noreturn(void*);int Cyc_Tcutil_extract_const_from_typedef(
unsigned int,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);void*Cyc_Tcexp_tcExp(struct
Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct
_tuple0*eq;int isTrue;};typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;struct
Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,
struct _dyneither_ptr msg_part);void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcDatatypedecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,
struct Cyc_Absyn_Enumdecl*);extern int Cyc_Cyclone_tovc_r;enum Cyc_Cyclone_C_Compilers{
Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};char Cyc_Tcutil_Unify[6]="Unify";
struct Cyc_Tcutil_Unify_exn_struct{char*tag;};struct Cyc_Tcutil_Unify_exn_struct
Cyc_Tcutil_Unify_val={Cyc_Tcutil_Unify};void Cyc_Tcutil_unify_it(void*t1,void*t2);
int Cyc_Tcutil_warn_region_coerce=0;void*Cyc_Tcutil_t1_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq1_const=0;void*Cyc_Tcutil_t2_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq2_const=0;struct _dyneither_ptr Cyc_Tcutil_failure_reason={(void*)
0,(void*)0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure(){if(Cyc_Position_num_errors
>= Cyc_Position_max_errors)return;Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
const char*_tmp1072;void*_tmp1071[2];const char*_tmp1070;const char*_tmp106F;struct
Cyc_String_pa_PrintArg_struct _tmp106E;struct Cyc_String_pa_PrintArg_struct
_tmp106D;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmp106D.tag=0,((
_tmp106D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure)),((_tmp106E.tag=0,((_tmp106E.f1=(struct _dyneither_ptr)(
Cyc_Tcutil_tq1_const?(_tmp106F="const ",_tag_dyneither(_tmp106F,sizeof(char),7)):((
_tmp1070="",_tag_dyneither(_tmp1070,sizeof(char),1)))),((_tmp1071[0]=& _tmp106E,((
_tmp1071[1]=& _tmp106D,Cyc_aprintf(((_tmp1072="%s%s",_tag_dyneither(_tmp1072,
sizeof(char),5))),_tag_dyneither(_tmp1071,sizeof(void*),2))))))))))))));const
char*_tmp107B;void*_tmp107A[2];const char*_tmp1079;const char*_tmp1078;struct Cyc_String_pa_PrintArg_struct
_tmp1077;struct Cyc_String_pa_PrintArg_struct _tmp1076;struct _dyneither_ptr s2=(
struct _dyneither_ptr)((_tmp1076.tag=0,((_tmp1076.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((_tmp1077.tag=
0,((_tmp1077.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(_tmp1078="const ",
_tag_dyneither(_tmp1078,sizeof(char),7)):((_tmp1079="",_tag_dyneither(_tmp1079,
sizeof(char),1)))),((_tmp107A[0]=& _tmp1077,((_tmp107A[1]=& _tmp1076,Cyc_aprintf(((
_tmp107B="%s%s",_tag_dyneither(_tmp107B,sizeof(char),5))),_tag_dyneither(
_tmp107A,sizeof(void*),2))))))))))))));int pos=2;{const char*_tmp107F;void*
_tmp107E[1];struct Cyc_String_pa_PrintArg_struct _tmp107D;(_tmp107D.tag=0,((
_tmp107D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp107E[0]=&
_tmp107D,Cyc_fprintf(Cyc_stderr,((_tmp107F="  %s",_tag_dyneither(_tmp107F,
sizeof(char),5))),_tag_dyneither(_tmp107E,sizeof(void*),1)))))));}pos +=
_get_dyneither_size(s1,sizeof(char));if(pos + 5 >= 80){{const char*_tmp1082;void*
_tmp1081;(_tmp1081=0,Cyc_fprintf(Cyc_stderr,((_tmp1082="\n\t",_tag_dyneither(
_tmp1082,sizeof(char),3))),_tag_dyneither(_tmp1081,sizeof(void*),0)));}pos=8;}
else{{const char*_tmp1085;void*_tmp1084;(_tmp1084=0,Cyc_fprintf(Cyc_stderr,((
_tmp1085=" ",_tag_dyneither(_tmp1085,sizeof(char),2))),_tag_dyneither(_tmp1084,
sizeof(void*),0)));}++ pos;}{const char*_tmp1088;void*_tmp1087;(_tmp1087=0,Cyc_fprintf(
Cyc_stderr,((_tmp1088="and ",_tag_dyneither(_tmp1088,sizeof(char),5))),
_tag_dyneither(_tmp1087,sizeof(void*),0)));}pos +=4;if(pos + _get_dyneither_size(
s2,sizeof(char))>= 80){{const char*_tmp108B;void*_tmp108A;(_tmp108A=0,Cyc_fprintf(
Cyc_stderr,((_tmp108B="\n\t",_tag_dyneither(_tmp108B,sizeof(char),3))),
_tag_dyneither(_tmp108A,sizeof(void*),0)));}pos=8;}{const char*_tmp108F;void*
_tmp108E[1];struct Cyc_String_pa_PrintArg_struct _tmp108D;(_tmp108D.tag=0,((
_tmp108D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmp108E[0]=&
_tmp108D,Cyc_fprintf(Cyc_stderr,((_tmp108F="%s ",_tag_dyneither(_tmp108F,sizeof(
char),4))),_tag_dyneither(_tmp108E,sizeof(void*),1)))))));}pos +=
_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const char*_tmp1092;
void*_tmp1091;(_tmp1091=0,Cyc_fprintf(Cyc_stderr,((_tmp1092="\n\t",
_tag_dyneither(_tmp1092,sizeof(char),3))),_tag_dyneither(_tmp1091,sizeof(void*),
0)));}pos=8;}{const char*_tmp1095;void*_tmp1094;(_tmp1094=0,Cyc_fprintf(Cyc_stderr,((
_tmp1095="are not compatible. ",_tag_dyneither(_tmp1095,sizeof(char),21))),
_tag_dyneither(_tmp1094,sizeof(void*),0)));}pos +=17;if((char*)Cyc_Tcutil_failure_reason.curr
!= (char*)(_tag_dyneither(0,0,0)).curr){if(pos + Cyc_strlen((struct _dyneither_ptr)
Cyc_Tcutil_failure_reason)>= 80){const char*_tmp1098;void*_tmp1097;(_tmp1097=0,
Cyc_fprintf(Cyc_stderr,((_tmp1098="\n\t",_tag_dyneither(_tmp1098,sizeof(char),3))),
_tag_dyneither(_tmp1097,sizeof(void*),0)));}{const char*_tmp109C;void*_tmp109B[1];
struct Cyc_String_pa_PrintArg_struct _tmp109A;(_tmp109A.tag=0,((_tmp109A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((
_tmp109B[0]=& _tmp109A,Cyc_fprintf(Cyc_stderr,((_tmp109C="%s",_tag_dyneither(
_tmp109C,sizeof(char),3))),_tag_dyneither(_tmp109B,sizeof(void*),1)))))));};}{
const char*_tmp109F;void*_tmp109E;(_tmp109E=0,Cyc_fprintf(Cyc_stderr,((_tmp109F="\n",
_tag_dyneither(_tmp109F,sizeof(char),2))),_tag_dyneither(_tmp109E,sizeof(void*),
0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);};}void Cyc_Tcutil_terr(
unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(
Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap)));}void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap);{const char*_tmp10A3;void*_tmp10A2[1];struct Cyc_String_pa_PrintArg_struct
_tmp10A1;(_tmp10A1.tag=0,((_tmp10A1.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)msg),((_tmp10A2[0]=& _tmp10A1,Cyc_fprintf(Cyc_stderr,((_tmp10A3="Compiler Error (Tcutil::impos): %s\n",
_tag_dyneither(_tmp10A3,sizeof(char),36))),_tag_dyneither(_tmp10A2,sizeof(void*),
1)))))));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_exn_struct
_tmp10A6;struct Cyc_Core_Impossible_exn_struct*_tmp10A5;(int)_throw((void*)((
_tmp10A5=_cycalloc(sizeof(*_tmp10A5)),((_tmp10A5[0]=((_tmp10A6.tag=Cyc_Core_Impossible,((
_tmp10A6.f1=msg,_tmp10A6)))),_tmp10A5)))));};}static struct _dyneither_ptr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs){for(0;tvs != 0;tvs=tvs->tl){const char*_tmp10AB;void*_tmp10AA[2];struct Cyc_String_pa_PrintArg_struct
_tmp10A9;struct Cyc_String_pa_PrintArg_struct _tmp10A8;(_tmp10A8.tag=0,((_tmp10A8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(((
struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmp10A9.tag=0,((_tmp10A9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
tvs->hd)),((_tmp10AA[0]=& _tmp10A9,((_tmp10AA[1]=& _tmp10A8,Cyc_fprintf(Cyc_stderr,((
_tmp10AB="%s::%s ",_tag_dyneither(_tmp10AB,sizeof(char),8))),_tag_dyneither(
_tmp10AA,sizeof(void*),2)))))))))))));}{const char*_tmp10AE;void*_tmp10AD;(
_tmp10AD=0,Cyc_fprintf(Cyc_stderr,((_tmp10AE="\n",_tag_dyneither(_tmp10AE,
sizeof(char),2))),_tag_dyneither(_tmp10AD,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(
unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct
_dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{
struct Cyc_List_List*_tmp10AF;Cyc_Tcutil_warning_segs=((_tmp10AF=_cycalloc(
sizeof(*_tmp10AF)),((_tmp10AF->hd=(void*)sg,((_tmp10AF->tl=Cyc_Tcutil_warning_segs,
_tmp10AF))))));}{struct _dyneither_ptr*_tmp10B2;struct Cyc_List_List*_tmp10B1;Cyc_Tcutil_warning_msgs=((
_tmp10B1=_cycalloc(sizeof(*_tmp10B1)),((_tmp10B1->hd=((_tmp10B2=_cycalloc(
sizeof(*_tmp10B2)),((_tmp10B2[0]=msg,_tmp10B2)))),((_tmp10B1->tl=Cyc_Tcutil_warning_msgs,
_tmp10B1))))));};}void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs == 0)
return;{const char*_tmp10B5;void*_tmp10B4;(_tmp10B4=0,Cyc_fprintf(Cyc_stderr,((
_tmp10B5="***Warnings***\n",_tag_dyneither(_tmp10B5,sizeof(char),16))),
_tag_dyneither(_tmp10B4,sizeof(void*),0)));}{struct Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){{const char*_tmp10BA;void*_tmp10B9[2];struct
Cyc_String_pa_PrintArg_struct _tmp10B8;struct Cyc_String_pa_PrintArg_struct
_tmp10B7;(_tmp10B7.tag=0,((_tmp10B7.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((
_tmp10B8.tag=0,((_tmp10B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((
_tmp10B9[0]=& _tmp10B8,((_tmp10B9[1]=& _tmp10B7,Cyc_fprintf(Cyc_stderr,((_tmp10BA="%s: %s\n",
_tag_dyneither(_tmp10BA,sizeof(char),8))),_tag_dyneither(_tmp10B9,sizeof(void*),
2)))))))))))));}_tmp36=_tmp36->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->tl;}{const char*_tmp10BD;void*_tmp10BC;(
_tmp10BC=0,Cyc_fprintf(Cyc_stderr,((_tmp10BD="**************\n",_tag_dyneither(
_tmp10BD,sizeof(char),16))),_tag_dyneither(_tmp10BC,sizeof(void*),0)));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);};}struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=
0;static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2){return tv1->identity - tv2->identity;}void*Cyc_Tcutil_compress(void*t){void*
_tmp3D=t;void*_tmp3F;void*_tmp41;void*_tmp43;void**_tmp44;void*_tmp46;void**
_tmp47;struct Cyc_Absyn_Exp*_tmp49;void**_tmp4B;void*_tmp4C;_LL1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp3E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D;if(_tmp3E->tag != 1)goto
_LL3;else{_tmp3F=(void*)_tmp3E->f2;if(_tmp3F != 0)goto _LL3;}}_LL2: goto _LL4;_LL3: {
struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp40=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)
_tmp3D;if(_tmp40->tag != 17)goto _LL5;else{_tmp41=(void*)_tmp40->f4;if(_tmp41 != 0)
goto _LL5;}}_LL4: return t;_LL5: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*
_tmp42=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3D;if(_tmp42->tag != 
17)goto _LL7;else{_tmp43=(void**)& _tmp42->f4;_tmp44=(void**)((void**)& _tmp42->f4);}}
_LL6: {void*ta=(void*)_check_null(*_tmp44);void*t2=Cyc_Tcutil_compress(ta);if(t2
!= ta)*_tmp44=(void*)t2;return t2;}_LL7: {struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp45=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D;if(_tmp45->tag != 1)goto
_LL9;else{_tmp46=(void**)& _tmp45->f2;_tmp47=(void**)((void**)& _tmp45->f2);}}_LL8: {
void*ta=(void*)_check_null(*_tmp47);void*t2=Cyc_Tcutil_compress(ta);if(t2 != ta)*
_tmp47=(void*)t2;return t2;}_LL9: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*
_tmp48=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3D;if(_tmp48->tag != 
18)goto _LLB;else{_tmp49=_tmp48->f1;}}_LLA: Cyc_Evexp_eval_const_uint_exp(_tmp49);{
void*_tmp4D=_tmp49->r;void*_tmp4F;_LL10: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*
_tmp4E=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4D;if(_tmp4E->tag != 
37)goto _LL12;else{_tmp4F=(void*)_tmp4E->f1;}}_LL11: return Cyc_Tcutil_compress(
_tmp4F);_LL12:;_LL13: return t;_LLF:;};_LLB: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*
_tmp4A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp3D;if(_tmp4A->tag != 
26)goto _LLD;else{_tmp4B=_tmp4A->f2;if(_tmp4B == 0)goto _LLD;_tmp4C=*_tmp4B;}}_LLC:
return Cyc_Tcutil_compress(_tmp4C);_LLD:;_LLE: return t;_LL0:;}void*Cyc_Tcutil_copy_type(
void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_type,ts);}static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(
union Cyc_Absyn_Constraint*cptr){union Cyc_Absyn_Constraint*_tmp50=cptr;union Cyc_Absyn_Constraint
_tmp51;int _tmp52;union Cyc_Absyn_Constraint _tmp53;void*_tmp54;union Cyc_Absyn_Constraint
_tmp55;union Cyc_Absyn_Constraint*_tmp56;_LL15: _tmp51=*_tmp50;if((_tmp51.No_constr).tag
!= 3)goto _LL17;_tmp52=(int)(_tmp51.No_constr).val;_LL16: return Cyc_Absyn_empty_conref();
_LL17: _tmp53=*_tmp50;if((_tmp53.Eq_constr).tag != 1)goto _LL19;_tmp54=(void*)(
_tmp53.Eq_constr).val;_LL18: return Cyc_Absyn_new_conref(_tmp54);_LL19: _tmp55=*
_tmp50;if((_tmp55.Forward_constr).tag != 2)goto _LL14;_tmp56=(union Cyc_Absyn_Constraint*)(
_tmp55.Forward_constr).val;_LL1A: return Cyc_Tcutil_copy_conref(_tmp56);_LL14:;}
static void*Cyc_Tcutil_copy_kindbound(void*kb){void*_tmp57=Cyc_Absyn_compress_kb(
kb);struct Cyc_Absyn_Kind*_tmp5A;_LL1C: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*
_tmp58=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp57;if(_tmp58->tag
!= 1)goto _LL1E;}_LL1D: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct
_tmp10C0;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp10BF;return(void*)((
_tmp10BF=_cycalloc(sizeof(*_tmp10BF)),((_tmp10BF[0]=((_tmp10C0.tag=1,((_tmp10C0.f1=
0,_tmp10C0)))),_tmp10BF))));}_LL1E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmp59=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp57;if(_tmp59->tag != 
2)goto _LL20;else{_tmp5A=_tmp59->f2;}}_LL1F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct
_tmp10C3;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp10C2;return(void*)((
_tmp10C2=_cycalloc(sizeof(*_tmp10C2)),((_tmp10C2[0]=((_tmp10C3.tag=2,((_tmp10C3.f1=
0,((_tmp10C3.f2=_tmp5A,_tmp10C3)))))),_tmp10C2))));}_LL20:;_LL21: return kb;_LL1B:;}
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){struct
Cyc_Absyn_Tvar*_tmp10C4;return(_tmp10C4=_cycalloc(sizeof(*_tmp10C4)),((_tmp10C4->name=
tv->name,((_tmp10C4->identity=- 1,((_tmp10C4->kind=Cyc_Tcutil_copy_kindbound(tv->kind),
_tmp10C4)))))));}static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
struct _tuple10 _tmp61;struct _dyneither_ptr*_tmp62;struct Cyc_Absyn_Tqual _tmp63;
void*_tmp64;struct _tuple10*_tmp60=arg;_tmp61=*_tmp60;_tmp62=_tmp61.f1;_tmp63=
_tmp61.f2;_tmp64=_tmp61.f3;{struct _tuple10*_tmp10C5;return(_tmp10C5=_cycalloc(
sizeof(*_tmp10C5)),((_tmp10C5->f1=_tmp62,((_tmp10C5->f2=_tmp63,((_tmp10C5->f3=
Cyc_Tcutil_copy_type(_tmp64),_tmp10C5)))))));};}static struct _tuple12*Cyc_Tcutil_copy_tqt(
struct _tuple12*arg){struct _tuple12 _tmp67;struct Cyc_Absyn_Tqual _tmp68;void*_tmp69;
struct _tuple12*_tmp66=arg;_tmp67=*_tmp66;_tmp68=_tmp67.f1;_tmp69=_tmp67.f2;{
struct _tuple12*_tmp10C6;return(_tmp10C6=_cycalloc(sizeof(*_tmp10C6)),((_tmp10C6->f1=
_tmp68,((_tmp10C6->f2=Cyc_Tcutil_copy_type(_tmp69),_tmp10C6)))));};}static struct
Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmp10C7;return(_tmp10C7=_cycalloc(sizeof(*_tmp10C7)),((_tmp10C7->name=f->name,((
_tmp10C7->tq=f->tq,((_tmp10C7->type=Cyc_Tcutil_copy_type(f->type),((_tmp10C7->width=
f->width,((_tmp10C7->attributes=f->attributes,_tmp10C7)))))))))));}static struct
_tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){struct _tuple0 _tmp6D;void*_tmp6E;
void*_tmp6F;struct _tuple0*_tmp6C=x;_tmp6D=*_tmp6C;_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;{
struct _tuple0*_tmp10C8;return(_tmp10C8=_cycalloc(sizeof(*_tmp10C8)),((_tmp10C8->f1=
Cyc_Tcutil_copy_type(_tmp6E),((_tmp10C8->f2=Cyc_Tcutil_copy_type(_tmp6F),
_tmp10C8)))));};}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f){struct Cyc_Absyn_Enumfield*_tmp10C9;return(_tmp10C9=
_cycalloc(sizeof(*_tmp10C9)),((_tmp10C9->name=f->name,((_tmp10C9->tag=f->tag,((
_tmp10C9->loc=f->loc,_tmp10C9)))))));}static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*
t){struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10CC;struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp10CB;return(void*)((_tmp10CB=_cycalloc(sizeof(*_tmp10CB)),((_tmp10CB[0]=((
_tmp10CC.tag=2,((_tmp10CC.f1=Cyc_Tcutil_copy_tvar(t),_tmp10CC)))),_tmp10CB))));}
void*Cyc_Tcutil_copy_type(void*t){void*_tmp74=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp78;struct Cyc_Absyn_DatatypeInfo _tmp7A;union Cyc_Absyn_DatatypeInfoU _tmp7B;
struct Cyc_List_List*_tmp7C;struct Cyc_Absyn_DatatypeFieldInfo _tmp7E;union Cyc_Absyn_DatatypeFieldInfoU
_tmp7F;struct Cyc_List_List*_tmp80;struct Cyc_Absyn_PtrInfo _tmp82;void*_tmp83;
struct Cyc_Absyn_Tqual _tmp84;struct Cyc_Absyn_PtrAtts _tmp85;void*_tmp86;union Cyc_Absyn_Constraint*
_tmp87;union Cyc_Absyn_Constraint*_tmp88;union Cyc_Absyn_Constraint*_tmp89;struct
Cyc_Absyn_PtrLoc*_tmp8A;struct Cyc_Absyn_ArrayInfo _tmp8E;void*_tmp8F;struct Cyc_Absyn_Tqual
_tmp90;struct Cyc_Absyn_Exp*_tmp91;union Cyc_Absyn_Constraint*_tmp92;unsigned int
_tmp93;struct Cyc_Absyn_FnInfo _tmp95;struct Cyc_List_List*_tmp96;void*_tmp97;
struct Cyc_Absyn_Tqual _tmp98;void*_tmp99;struct Cyc_List_List*_tmp9A;int _tmp9B;
struct Cyc_Absyn_VarargInfo*_tmp9C;struct Cyc_List_List*_tmp9D;struct Cyc_List_List*
_tmp9E;struct Cyc_List_List*_tmpA0;struct Cyc_Absyn_AggrInfo _tmpA2;union Cyc_Absyn_AggrInfoU
_tmpA3;struct _tuple4 _tmpA4;enum Cyc_Absyn_AggrKind _tmpA5;struct _tuple2*_tmpA6;
struct Cyc_Core_Opt*_tmpA7;struct Cyc_List_List*_tmpA8;struct Cyc_Absyn_AggrInfo
_tmpAA;union Cyc_Absyn_AggrInfoU _tmpAB;struct Cyc_Absyn_Aggrdecl**_tmpAC;struct Cyc_List_List*
_tmpAD;enum Cyc_Absyn_AggrKind _tmpAF;struct Cyc_List_List*_tmpB0;struct _tuple2*
_tmpB2;struct Cyc_Absyn_Enumdecl*_tmpB3;struct Cyc_List_List*_tmpB5;void*_tmpB7;
struct Cyc_Absyn_Exp*_tmpB9;void*_tmpBB;void*_tmpBD;void*_tmpBE;struct _tuple2*
_tmpC0;struct Cyc_List_List*_tmpC1;struct Cyc_Absyn_Typedefdecl*_tmpC2;void*_tmpC7;
struct Cyc_List_List*_tmpC9;void*_tmpCB;struct Cyc_Absyn_TypeDecl*_tmpCD;struct Cyc_Absyn_TypeDecl
_tmpCE;void*_tmpCF;struct Cyc_Absyn_Aggrdecl*_tmpD1;struct Cyc_Absyn_TypeDecl*
_tmpD3;struct Cyc_Absyn_TypeDecl _tmpD4;void*_tmpD5;struct Cyc_Absyn_Enumdecl*
_tmpD7;struct Cyc_Absyn_TypeDecl*_tmpD9;struct Cyc_Absyn_TypeDecl _tmpDA;void*
_tmpDB;struct Cyc_Absyn_Datatypedecl*_tmpDD;_LL23: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*
_tmp75=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp74;if(_tmp75->tag != 0)
goto _LL25;}_LL24: goto _LL26;_LL25: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp76=(
struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp74;if(_tmp76->tag != 1)goto _LL27;}
_LL26: return t;_LL27: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp77=(struct
Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp74;if(_tmp77->tag != 2)goto _LL29;else{
_tmp78=_tmp77->f1;}}_LL28: {struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10CF;
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10CE;return(void*)((_tmp10CE=
_cycalloc(sizeof(*_tmp10CE)),((_tmp10CE[0]=((_tmp10CF.tag=2,((_tmp10CF.f1=Cyc_Tcutil_copy_tvar(
_tmp78),_tmp10CF)))),_tmp10CE))));}_LL29: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*
_tmp79=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp74;if(_tmp79->tag != 
3)goto _LL2B;else{_tmp7A=_tmp79->f1;_tmp7B=_tmp7A.datatype_info;_tmp7C=_tmp7A.targs;}}
_LL2A: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp10D5;struct Cyc_Absyn_DatatypeInfo
_tmp10D4;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp10D3;return(void*)((
_tmp10D3=_cycalloc(sizeof(*_tmp10D3)),((_tmp10D3[0]=((_tmp10D5.tag=3,((_tmp10D5.f1=((
_tmp10D4.datatype_info=_tmp7B,((_tmp10D4.targs=Cyc_Tcutil_copy_types(_tmp7C),
_tmp10D4)))),_tmp10D5)))),_tmp10D3))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmp7D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp74;if(_tmp7D->tag
!= 4)goto _LL2D;else{_tmp7E=_tmp7D->f1;_tmp7F=_tmp7E.field_info;_tmp80=_tmp7E.targs;}}
_LL2C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp10DB;struct Cyc_Absyn_DatatypeFieldInfo
_tmp10DA;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp10D9;return(
void*)((_tmp10D9=_cycalloc(sizeof(*_tmp10D9)),((_tmp10D9[0]=((_tmp10DB.tag=4,((
_tmp10DB.f1=((_tmp10DA.field_info=_tmp7F,((_tmp10DA.targs=Cyc_Tcutil_copy_types(
_tmp80),_tmp10DA)))),_tmp10DB)))),_tmp10D9))));}_LL2D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp81=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74;if(_tmp81->tag != 5)
goto _LL2F;else{_tmp82=_tmp81->f1;_tmp83=_tmp82.elt_typ;_tmp84=_tmp82.elt_tq;
_tmp85=_tmp82.ptr_atts;_tmp86=_tmp85.rgn;_tmp87=_tmp85.nullable;_tmp88=_tmp85.bounds;
_tmp89=_tmp85.zero_term;_tmp8A=_tmp85.ptrloc;}}_LL2E: {void*_tmpE6=Cyc_Tcutil_copy_type(
_tmp83);void*_tmpE7=Cyc_Tcutil_copy_type(_tmp86);union Cyc_Absyn_Constraint*
_tmpE8=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp87);struct Cyc_Absyn_Tqual _tmpE9=_tmp84;union Cyc_Absyn_Constraint*_tmpEA=((
union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp88);union Cyc_Absyn_Constraint*_tmpEB=((union Cyc_Absyn_Constraint*(*)(union
Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp89);struct Cyc_Absyn_PointerType_Absyn_Type_struct
_tmp10E5;struct Cyc_Absyn_PtrAtts _tmp10E4;struct Cyc_Absyn_PtrInfo _tmp10E3;struct
Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp10E2;return(void*)((_tmp10E2=
_cycalloc(sizeof(*_tmp10E2)),((_tmp10E2[0]=((_tmp10E5.tag=5,((_tmp10E5.f1=((
_tmp10E3.elt_typ=_tmpE6,((_tmp10E3.elt_tq=_tmpE9,((_tmp10E3.ptr_atts=((_tmp10E4.rgn=
_tmpE7,((_tmp10E4.nullable=_tmpE8,((_tmp10E4.bounds=_tmpEA,((_tmp10E4.zero_term=
_tmpEB,((_tmp10E4.ptrloc=_tmp8A,_tmp10E4)))))))))),_tmp10E3)))))),_tmp10E5)))),
_tmp10E2))));}_LL2F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8B=(struct
Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74;if(_tmp8B->tag != 6)goto _LL31;}_LL30:
goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp8C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)
_tmp74;if(_tmp8C->tag != 7)goto _LL33;}_LL32: return t;_LL33: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp8D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp74;if(_tmp8D->tag != 8)
goto _LL35;else{_tmp8E=_tmp8D->f1;_tmp8F=_tmp8E.elt_type;_tmp90=_tmp8E.tq;_tmp91=
_tmp8E.num_elts;_tmp92=_tmp8E.zero_term;_tmp93=_tmp8E.zt_loc;}}_LL34: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct
_tmp10EB;struct Cyc_Absyn_ArrayInfo _tmp10EA;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp10E9;return(void*)((_tmp10E9=_cycalloc(sizeof(*_tmp10E9)),((_tmp10E9[0]=((
_tmp10EB.tag=8,((_tmp10EB.f1=((_tmp10EA.elt_type=Cyc_Tcutil_copy_type(_tmp8F),((
_tmp10EA.tq=_tmp90,((_tmp10EA.num_elts=_tmp91,((_tmp10EA.zero_term=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp92),((_tmp10EA.zt_loc=
_tmp93,_tmp10EA)))))))))),_tmp10EB)))),_tmp10E9))));}_LL35: {struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp94=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp74;if(_tmp94->tag != 9)goto
_LL37;else{_tmp95=_tmp94->f1;_tmp96=_tmp95.tvars;_tmp97=_tmp95.effect;_tmp98=
_tmp95.ret_tqual;_tmp99=_tmp95.ret_typ;_tmp9A=_tmp95.args;_tmp9B=_tmp95.c_varargs;
_tmp9C=_tmp95.cyc_varargs;_tmp9D=_tmp95.rgn_po;_tmp9E=_tmp95.attributes;}}_LL36: {
struct Cyc_List_List*_tmpF3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,
_tmp96);void*effopt2=_tmp97 == 0?0:(void*)Cyc_Tcutil_copy_type((void*)_tmp97);
void*_tmpF4=Cyc_Tcutil_copy_type(_tmp99);struct Cyc_List_List*_tmpF5=((struct Cyc_List_List*(*)(
struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,
_tmp9A);int _tmpF6=_tmp9B;struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;if(_tmp9C != 0){
struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)_tmp9C;struct Cyc_Absyn_VarargInfo*
_tmp10EC;cyc_varargs2=((_tmp10EC=_cycalloc(sizeof(*_tmp10EC)),((_tmp10EC->name=
cv->name,((_tmp10EC->tq=cv->tq,((_tmp10EC->type=Cyc_Tcutil_copy_type(cv->type),((
_tmp10EC->inject=cv->inject,_tmp10EC))))))))));}{struct Cyc_List_List*_tmpF8=((
struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp9D);struct Cyc_List_List*_tmpF9=_tmp9E;
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10F2;struct Cyc_Absyn_FnInfo _tmp10F1;
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10F0;return(void*)((_tmp10F0=
_cycalloc(sizeof(*_tmp10F0)),((_tmp10F0[0]=((_tmp10F2.tag=9,((_tmp10F2.f1=((
_tmp10F1.tvars=_tmpF3,((_tmp10F1.effect=effopt2,((_tmp10F1.ret_tqual=_tmp98,((
_tmp10F1.ret_typ=_tmpF4,((_tmp10F1.args=_tmpF5,((_tmp10F1.c_varargs=_tmpF6,((
_tmp10F1.cyc_varargs=cyc_varargs2,((_tmp10F1.rgn_po=_tmpF8,((_tmp10F1.attributes=
_tmpF9,_tmp10F1)))))))))))))))))),_tmp10F2)))),_tmp10F0))));};}_LL37: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmp9F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp74;if(_tmp9F->tag != 10)
goto _LL39;else{_tmpA0=_tmp9F->f1;}}_LL38: {struct Cyc_Absyn_TupleType_Absyn_Type_struct
_tmp10F5;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp10F4;return(void*)((
_tmp10F4=_cycalloc(sizeof(*_tmp10F4)),((_tmp10F4[0]=((_tmp10F5.tag=10,((_tmp10F5.f1=((
struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpA0),_tmp10F5)))),_tmp10F4))));}_LL39: {
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpA1=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp74;if(_tmpA1->tag != 11)goto _LL3B;else{_tmpA2=_tmpA1->f1;_tmpA3=_tmpA2.aggr_info;
if((_tmpA3.UnknownAggr).tag != 1)goto _LL3B;_tmpA4=(struct _tuple4)(_tmpA3.UnknownAggr).val;
_tmpA5=_tmpA4.f1;_tmpA6=_tmpA4.f2;_tmpA7=_tmpA4.f3;_tmpA8=_tmpA2.targs;}}_LL3A: {
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp10FB;struct Cyc_Absyn_AggrInfo
_tmp10FA;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp10F9;return(void*)((
_tmp10F9=_cycalloc(sizeof(*_tmp10F9)),((_tmp10F9[0]=((_tmp10FB.tag=11,((_tmp10FB.f1=((
_tmp10FA.aggr_info=Cyc_Absyn_UnknownAggr(_tmpA5,_tmpA6,_tmpA7),((_tmp10FA.targs=
Cyc_Tcutil_copy_types(_tmpA8),_tmp10FA)))),_tmp10FB)))),_tmp10F9))));}_LL3B: {
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpA9=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp74;if(_tmpA9->tag != 11)goto _LL3D;else{_tmpAA=_tmpA9->f1;_tmpAB=_tmpAA.aggr_info;
if((_tmpAB.KnownAggr).tag != 2)goto _LL3D;_tmpAC=(struct Cyc_Absyn_Aggrdecl**)(
_tmpAB.KnownAggr).val;_tmpAD=_tmpAA.targs;}}_LL3C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct
_tmp1101;struct Cyc_Absyn_AggrInfo _tmp1100;struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp10FF;return(void*)((_tmp10FF=_cycalloc(sizeof(*_tmp10FF)),((_tmp10FF[0]=((
_tmp1101.tag=11,((_tmp1101.f1=((_tmp1100.aggr_info=Cyc_Absyn_KnownAggr(_tmpAC),((
_tmp1100.targs=Cyc_Tcutil_copy_types(_tmpAD),_tmp1100)))),_tmp1101)))),_tmp10FF))));}
_LL3D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)
_tmp74;if(_tmpAE->tag != 12)goto _LL3F;else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;}}
_LL3E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp1104;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmp1103;return(void*)((_tmp1103=_cycalloc(sizeof(*_tmp1103)),((_tmp1103[0]=((
_tmp1104.tag=12,((_tmp1104.f1=_tmpAF,((_tmp1104.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpB0),_tmp1104)))))),_tmp1103))));}_LL3F: {
struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpB1=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)
_tmp74;if(_tmpB1->tag != 13)goto _LL41;else{_tmpB2=_tmpB1->f1;_tmpB3=_tmpB1->f2;}}
_LL40: {struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp1107;struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp1106;return(void*)((_tmp1106=_cycalloc(sizeof(*_tmp1106)),((_tmp1106[0]=((
_tmp1107.tag=13,((_tmp1107.f1=_tmpB2,((_tmp1107.f2=_tmpB3,_tmp1107)))))),
_tmp1106))));}_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpB4=(
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp74;if(_tmpB4->tag != 14)goto
_LL43;else{_tmpB5=_tmpB4->f1;}}_LL42: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct
_tmp110A;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1109;return(void*)((
_tmp1109=_cycalloc(sizeof(*_tmp1109)),((_tmp1109[0]=((_tmp110A.tag=14,((_tmp110A.f1=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpB5),_tmp110A)))),
_tmp1109))));}_LL43: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB6=(struct
Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp74;if(_tmpB6->tag != 19)goto _LL45;else{
_tmpB7=(void*)_tmpB6->f1;}}_LL44: {struct Cyc_Absyn_TagType_Absyn_Type_struct
_tmp110D;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp110C;return(void*)((
_tmp110C=_cycalloc(sizeof(*_tmp110C)),((_tmp110C[0]=((_tmp110D.tag=19,((_tmp110D.f1=(
void*)Cyc_Tcutil_copy_type(_tmpB7),_tmp110D)))),_tmp110C))));}_LL45: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*
_tmpB8=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp74;if(_tmpB8->tag != 
18)goto _LL47;else{_tmpB9=_tmpB8->f1;}}_LL46: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct
_tmp1110;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp110F;return(void*)((
_tmp110F=_cycalloc(sizeof(*_tmp110F)),((_tmp110F[0]=((_tmp1110.tag=18,((_tmp1110.f1=
_tmpB9,_tmp1110)))),_tmp110F))));}_LL47: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*
_tmpBA=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp74;if(_tmpBA->tag != 
15)goto _LL49;else{_tmpBB=(void*)_tmpBA->f1;}}_LL48: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct
_tmp1113;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1112;return(void*)((
_tmp1112=_cycalloc(sizeof(*_tmp1112)),((_tmp1112[0]=((_tmp1113.tag=15,((_tmp1113.f1=(
void*)Cyc_Tcutil_copy_type(_tmpBB),_tmp1113)))),_tmp1112))));}_LL49: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*
_tmpBC=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp74;if(_tmpBC->tag != 16)
goto _LL4B;else{_tmpBD=(void*)_tmpBC->f1;_tmpBE=(void*)_tmpBC->f2;}}_LL4A: {
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp1116;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*
_tmp1115;return(void*)((_tmp1115=_cycalloc(sizeof(*_tmp1115)),((_tmp1115[0]=((
_tmp1116.tag=16,((_tmp1116.f1=(void*)Cyc_Tcutil_copy_type(_tmpBD),((_tmp1116.f2=(
void*)Cyc_Tcutil_copy_type(_tmpBE),_tmp1116)))))),_tmp1115))));}_LL4B: {struct
Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpBF=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)
_tmp74;if(_tmpBF->tag != 17)goto _LL4D;else{_tmpC0=_tmpBF->f1;_tmpC1=_tmpBF->f2;
_tmpC2=_tmpBF->f3;}}_LL4C: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct
_tmp1119;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1118;return(void*)((
_tmp1118=_cycalloc(sizeof(*_tmp1118)),((_tmp1118[0]=((_tmp1119.tag=17,((_tmp1119.f1=
_tmpC0,((_tmp1119.f2=Cyc_Tcutil_copy_types(_tmpC1),((_tmp1119.f3=_tmpC2,((
_tmp1119.f4=(void*)0,_tmp1119)))))))))),_tmp1118))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*
_tmpC3=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp74;if(_tmpC3->tag != 21)
goto _LL4F;}_LL4E: goto _LL50;_LL4F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*
_tmpC4=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp74;if(_tmpC4->tag != 22)
goto _LL51;}_LL50: goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*
_tmpC5=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp74;if(_tmpC5->tag != 20)
goto _LL53;}_LL52: return t;_LL53: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*
_tmpC6=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp74;if(_tmpC6->tag != 23)
goto _LL55;else{_tmpC7=(void*)_tmpC6->f1;}}_LL54: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct
_tmp111C;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp111B;return(void*)((
_tmp111B=_cycalloc(sizeof(*_tmp111B)),((_tmp111B[0]=((_tmp111C.tag=23,((_tmp111C.f1=(
void*)Cyc_Tcutil_copy_type(_tmpC7),_tmp111C)))),_tmp111B))));}_LL55: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmpC8=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp74;if(_tmpC8->tag != 24)
goto _LL57;else{_tmpC9=_tmpC8->f1;}}_LL56: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct
_tmp111F;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp111E;return(void*)((
_tmp111E=_cycalloc(sizeof(*_tmp111E)),((_tmp111E[0]=((_tmp111F.tag=24,((_tmp111F.f1=
Cyc_Tcutil_copy_types(_tmpC9),_tmp111F)))),_tmp111E))));}_LL57: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*
_tmpCA=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp74;if(_tmpCA->tag != 25)
goto _LL59;else{_tmpCB=(void*)_tmpCA->f1;}}_LL58: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct
_tmp1122;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1121;return(void*)((
_tmp1121=_cycalloc(sizeof(*_tmp1121)),((_tmp1121[0]=((_tmp1122.tag=25,((_tmp1122.f1=(
void*)Cyc_Tcutil_copy_type(_tmpCB),_tmp1122)))),_tmp1121))));}_LL59: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*
_tmpCC=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp74;if(_tmpCC->tag != 
26)goto _LL5B;else{_tmpCD=_tmpCC->f1;_tmpCE=*_tmpCD;_tmpCF=_tmpCE.r;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*
_tmpD0=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmpCF;if(_tmpD0->tag
!= 0)goto _LL5B;else{_tmpD1=_tmpD0->f1;}};}}_LL5A: {struct Cyc_List_List*_tmp11B=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD1->tvs);struct Cyc_Absyn_AggrType_Absyn_Type_struct
_tmp1128;struct Cyc_Absyn_AggrInfo _tmp1127;struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp1126;return(void*)((_tmp1126=_cycalloc(sizeof(*_tmp1126)),((_tmp1126[0]=((
_tmp1128.tag=11,((_tmp1128.f1=((_tmp1127.aggr_info=Cyc_Absyn_UnknownAggr(_tmpD1->kind,
_tmpD1->name,0),((_tmp1127.targs=_tmp11B,_tmp1127)))),_tmp1128)))),_tmp1126))));}
_LL5B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpD2=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)
_tmp74;if(_tmpD2->tag != 26)goto _LL5D;else{_tmpD3=_tmpD2->f1;_tmpD4=*_tmpD3;
_tmpD5=_tmpD4.r;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpD6=(
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmpD5;if(_tmpD6->tag != 1)
goto _LL5D;else{_tmpD7=_tmpD6->f1;}};}}_LL5C: {struct Cyc_Absyn_EnumType_Absyn_Type_struct
_tmp112B;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp112A;return(void*)((
_tmp112A=_cycalloc(sizeof(*_tmp112A)),((_tmp112A[0]=((_tmp112B.tag=13,((_tmp112B.f1=
_tmpD7->name,((_tmp112B.f2=0,_tmp112B)))))),_tmp112A))));}_LL5D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*
_tmpD8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp74;if(_tmpD8->tag != 
26)goto _LL22;else{_tmpD9=_tmpD8->f1;_tmpDA=*_tmpD9;_tmpDB=_tmpDA.r;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*
_tmpDC=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmpDB;if(_tmpDC->tag
!= 2)goto _LL22;else{_tmpDD=_tmpDC->f1;}};}}_LL5E: {struct Cyc_List_List*_tmp121=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpDD->tvs);struct Cyc_Absyn_DatatypeType_Absyn_Type_struct
_tmp1135;struct Cyc_Absyn_UnknownDatatypeInfo _tmp1134;struct Cyc_Absyn_DatatypeInfo
_tmp1133;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1132;return(void*)((
_tmp1132=_cycalloc(sizeof(*_tmp1132)),((_tmp1132[0]=((_tmp1135.tag=3,((_tmp1135.f1=((
_tmp1133.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmp1134.name=_tmpDD->name,((
_tmp1134.is_extensible=0,_tmp1134))))),((_tmp1133.targs=_tmp121,_tmp1133)))),
_tmp1135)))),_tmp1132))));}_LL22:;}static void*Cyc_Tcutil_copy_designator(void*d){
void*_tmp126=d;struct Cyc_Absyn_Exp*_tmp128;struct _dyneither_ptr*_tmp12A;_LL60: {
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp127=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)
_tmp126;if(_tmp127->tag != 0)goto _LL62;else{_tmp128=_tmp127->f1;}}_LL61: {struct
Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmp1138;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*
_tmp1137;return(void*)((_tmp1137=_cycalloc(sizeof(*_tmp1137)),((_tmp1137[0]=((
_tmp1138.tag=0,((_tmp1138.f1=Cyc_Tcutil_deep_copy_exp(_tmp128),_tmp1138)))),
_tmp1137))));}_LL62: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp129=(
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp126;if(_tmp129->tag != 1)
goto _LL5F;else{_tmp12A=_tmp129->f1;}}_LL63: return d;_LL5F:;}struct _tuple19{struct
Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple19*Cyc_Tcutil_copy_eds(
struct _tuple19*e){struct _tuple19*_tmp1139;return(_tmp1139=_cycalloc(sizeof(*
_tmp1139)),((_tmp1139->f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_designator,(e[0]).f1),((_tmp1139->f2=Cyc_Tcutil_deep_copy_exp((
e[0]).f2),_tmp1139)))));}struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*
e){void*_tmp12E=e->r;union Cyc_Absyn_Cnst _tmp130;struct _tuple2*_tmp132;void*
_tmp133;enum Cyc_Absyn_Primop _tmp135;struct Cyc_List_List*_tmp136;struct Cyc_Absyn_Exp*
_tmp138;struct Cyc_Core_Opt*_tmp139;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_Absyn_Exp*
_tmp13C;enum Cyc_Absyn_Incrementor _tmp13D;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*
_tmp140;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_Absyn_Exp*
_tmp144;struct Cyc_Absyn_Exp*_tmp146;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Absyn_Exp*
_tmp149;struct Cyc_Absyn_Exp*_tmp14A;struct Cyc_Absyn_Exp*_tmp14C;struct Cyc_List_List*
_tmp14D;struct Cyc_Absyn_VarargCallInfo*_tmp14E;int _tmp14F;struct Cyc_Absyn_Exp*
_tmp151;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_List_List*
_tmp156;void*_tmp158;struct Cyc_Absyn_Exp*_tmp159;int _tmp15A;enum Cyc_Absyn_Coercion
_tmp15B;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_Absyn_Exp*_tmp15F;struct Cyc_Absyn_Exp*
_tmp160;void*_tmp162;struct Cyc_Absyn_Exp*_tmp164;void*_tmp166;void*_tmp167;
struct Cyc_Absyn_Exp*_tmp169;struct Cyc_Absyn_Exp*_tmp16B;struct _dyneither_ptr*
_tmp16C;int _tmp16D;int _tmp16E;struct Cyc_Absyn_Exp*_tmp170;struct _dyneither_ptr*
_tmp171;int _tmp172;int _tmp173;struct Cyc_Absyn_Exp*_tmp175;struct Cyc_Absyn_Exp*
_tmp176;struct Cyc_List_List*_tmp178;struct _tuple10*_tmp17A;struct _tuple10 _tmp17B;
struct _dyneither_ptr*_tmp17C;struct Cyc_Absyn_Tqual _tmp17D;void*_tmp17E;struct Cyc_List_List*
_tmp17F;struct Cyc_List_List*_tmp181;struct Cyc_Absyn_Vardecl*_tmp183;struct Cyc_Absyn_Exp*
_tmp184;struct Cyc_Absyn_Exp*_tmp185;int _tmp186;struct _tuple2*_tmp188;struct Cyc_List_List*
_tmp189;struct Cyc_List_List*_tmp18A;struct Cyc_Absyn_Aggrdecl*_tmp18B;void*
_tmp18D;struct Cyc_List_List*_tmp18E;struct Cyc_List_List*_tmp190;struct Cyc_Absyn_Datatypedecl*
_tmp191;struct Cyc_Absyn_Datatypefield*_tmp192;struct _tuple2*_tmp194;struct Cyc_Absyn_Enumdecl*
_tmp195;struct Cyc_Absyn_Enumfield*_tmp196;struct _tuple2*_tmp198;void*_tmp199;
struct Cyc_Absyn_Enumfield*_tmp19A;struct Cyc_Absyn_MallocInfo _tmp19C;int _tmp19D;
struct Cyc_Absyn_Exp*_tmp19E;void**_tmp19F;struct Cyc_Absyn_Exp*_tmp1A0;int _tmp1A1;
struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_Core_Opt*
_tmp1A6;struct Cyc_List_List*_tmp1A7;struct Cyc_Absyn_Exp*_tmp1AA;struct
_dyneither_ptr*_tmp1AB;void*_tmp1AD;int _tmp1AF;struct _dyneither_ptr _tmp1B0;_LL65: {
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp12F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmp12E;if(_tmp12F->tag != 0)goto _LL67;else{_tmp130=_tmp12F->f1;}}_LL66: return Cyc_Absyn_const_exp(
_tmp130,e->loc);_LL67: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp131=(
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp131->tag != 1)goto
_LL69;else{_tmp132=_tmp131->f1;_tmp133=(void*)_tmp131->f2;}}_LL68: return Cyc_Absyn_varb_exp(
_tmp132,_tmp133,e->loc);_LL69: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*
_tmp134=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp134->tag
!= 2)goto _LL6B;else{_tmp135=_tmp134->f1;_tmp136=_tmp134->f2;}}_LL6A: return Cyc_Absyn_primop_exp(
_tmp135,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp136),e->loc);
_LL6B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp137=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)
_tmp12E;if(_tmp137->tag != 3)goto _LL6D;else{_tmp138=_tmp137->f1;_tmp139=_tmp137->f2;
_tmp13A=_tmp137->f3;}}_LL6C: {struct Cyc_Core_Opt*_tmp113A;return Cyc_Absyn_assignop_exp(
Cyc_Tcutil_deep_copy_exp(_tmp138),(unsigned int)_tmp139?(_tmp113A=
_cycalloc_atomic(sizeof(*_tmp113A)),((_tmp113A->v=(void*)((enum Cyc_Absyn_Primop)
_tmp139->v),_tmp113A))): 0,Cyc_Tcutil_deep_copy_exp(_tmp13A),e->loc);}_LL6D: {
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp13B=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)
_tmp12E;if(_tmp13B->tag != 4)goto _LL6F;else{_tmp13C=_tmp13B->f1;_tmp13D=_tmp13B->f2;}}
_LL6E: return Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp13C),_tmp13D,e->loc);
_LL6F: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp13E=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)
_tmp12E;if(_tmp13E->tag != 5)goto _LL71;else{_tmp13F=_tmp13E->f1;_tmp140=_tmp13E->f2;
_tmp141=_tmp13E->f3;}}_LL70: return Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(
_tmp13F),Cyc_Tcutil_deep_copy_exp(_tmp140),Cyc_Tcutil_deep_copy_exp(_tmp141),e->loc);
_LL71: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp142=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)
_tmp12E;if(_tmp142->tag != 6)goto _LL73;else{_tmp143=_tmp142->f1;_tmp144=_tmp142->f2;}}
_LL72: return Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp143),Cyc_Tcutil_deep_copy_exp(
_tmp144),e->loc);_LL73: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp145=(
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp145->tag != 7)goto _LL75;
else{_tmp146=_tmp145->f1;_tmp147=_tmp145->f2;}}_LL74: return Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(
_tmp146),Cyc_Tcutil_deep_copy_exp(_tmp147),e->loc);_LL75: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*
_tmp148=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp148->tag
!= 8)goto _LL77;else{_tmp149=_tmp148->f1;_tmp14A=_tmp148->f2;}}_LL76: return Cyc_Absyn_seq_exp(
Cyc_Tcutil_deep_copy_exp(_tmp149),Cyc_Tcutil_deep_copy_exp(_tmp14A),e->loc);
_LL77: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp14B=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)
_tmp12E;if(_tmp14B->tag != 9)goto _LL79;else{_tmp14C=_tmp14B->f1;_tmp14D=_tmp14B->f2;
_tmp14E=_tmp14B->f3;_tmp14F=_tmp14B->f4;}}_LL78: {struct Cyc_Absyn_VarargCallInfo*
_tmp1B2=_tmp14E;struct Cyc_Absyn_VarargCallInfo _tmp1B3;int _tmp1B4;struct Cyc_List_List*
_tmp1B5;struct Cyc_Absyn_VarargInfo*_tmp1B6;_LLB4: if(_tmp1B2 == 0)goto _LLB6;
_tmp1B3=*_tmp1B2;_tmp1B4=_tmp1B3.num_varargs;_tmp1B5=_tmp1B3.injectors;_tmp1B6=
_tmp1B3.vai;_LLB5: {struct Cyc_Absyn_VarargInfo _tmp1B8;struct _dyneither_ptr*
_tmp1B9;struct Cyc_Absyn_Tqual _tmp1BA;void*_tmp1BB;int _tmp1BC;struct Cyc_Absyn_VarargInfo*
_tmp1B7=_tmp1B6;_tmp1B8=*_tmp1B7;_tmp1B9=_tmp1B8.name;_tmp1BA=_tmp1B8.tq;_tmp1BB=
_tmp1B8.type;_tmp1BC=_tmp1B8.inject;{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct
_tmp1144;struct Cyc_Absyn_VarargInfo*_tmp1143;struct Cyc_Absyn_VarargCallInfo*
_tmp1142;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp1141;return Cyc_Absyn_new_exp((
void*)((_tmp1141=_cycalloc(sizeof(*_tmp1141)),((_tmp1141[0]=((_tmp1144.tag=9,((
_tmp1144.f1=Cyc_Tcutil_deep_copy_exp(_tmp14C),((_tmp1144.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_deep_copy_exp,_tmp14D),((_tmp1144.f3=((_tmp1142=_cycalloc(sizeof(*
_tmp1142)),((_tmp1142->num_varargs=_tmp1B4,((_tmp1142->injectors=_tmp1B5,((
_tmp1142->vai=((_tmp1143=_cycalloc(sizeof(*_tmp1143)),((_tmp1143->name=_tmp1B9,((
_tmp1143->tq=_tmp1BA,((_tmp1143->type=Cyc_Tcutil_copy_type(_tmp1BB),((_tmp1143->inject=
_tmp1BC,_tmp1143)))))))))),_tmp1142)))))))),((_tmp1144.f4=_tmp14F,_tmp1144)))))))))),
_tmp1141)))),e->loc);};}_LLB6:;_LLB7: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct
_tmp1147;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp1146;return Cyc_Absyn_new_exp((
void*)((_tmp1146=_cycalloc(sizeof(*_tmp1146)),((_tmp1146[0]=((_tmp1147.tag=9,((
_tmp1147.f1=Cyc_Tcutil_deep_copy_exp(_tmp14C),((_tmp1147.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_deep_copy_exp,_tmp14D),((_tmp1147.f3=_tmp14E,((_tmp1147.f4=_tmp14F,
_tmp1147)))))))))),_tmp1146)))),e->loc);}_LLB3:;}_LL79: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*
_tmp150=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp150->tag != 
10)goto _LL7B;else{_tmp151=_tmp150->f1;}}_LL7A: return Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(
_tmp151),e->loc);_LL7B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*
_tmp152=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp12E;if(
_tmp152->tag != 11)goto _LL7D;else{_tmp153=_tmp152->f1;}}_LL7C: return Cyc_Absyn_noinstantiate_exp(
Cyc_Tcutil_deep_copy_exp(_tmp153),e->loc);_LL7D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*
_tmp154=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp154->tag
!= 12)goto _LL7F;else{_tmp155=_tmp154->f1;_tmp156=_tmp154->f2;}}_LL7E: return Cyc_Absyn_instantiate_exp(
Cyc_Tcutil_deep_copy_exp(_tmp155),((struct Cyc_List_List*(*)(void*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp156),e->loc);_LL7F: {
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp157=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)
_tmp12E;if(_tmp157->tag != 13)goto _LL81;else{_tmp158=(void*)_tmp157->f1;_tmp159=
_tmp157->f2;_tmp15A=_tmp157->f3;_tmp15B=_tmp157->f4;}}_LL80: return Cyc_Absyn_cast_exp(
Cyc_Tcutil_copy_type(_tmp158),Cyc_Tcutil_deep_copy_exp(_tmp159),_tmp15A,_tmp15B,
e->loc);_LL81: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp15C=(struct
Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp15C->tag != 14)goto _LL83;
else{_tmp15D=_tmp15C->f1;}}_LL82: return Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(
_tmp15D),e->loc);_LL83: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp15E=(
struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp15E->tag != 15)goto
_LL85;else{_tmp15F=_tmp15E->f1;_tmp160=_tmp15E->f2;}}_LL84: {struct Cyc_Absyn_Exp*
eo1=_tmp15F;if(_tmp15F != 0)eo1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((
struct Cyc_Absyn_Exp*)_tmp15F);return Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(
_tmp160),e->loc);}_LL85: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*
_tmp161=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp161->tag
!= 16)goto _LL87;else{_tmp162=(void*)_tmp161->f1;}}_LL86: return Cyc_Absyn_sizeoftyp_exp(
Cyc_Tcutil_copy_type(_tmp162),e->loc);_LL87: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*
_tmp163=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp163->tag
!= 17)goto _LL89;else{_tmp164=_tmp163->f1;}}_LL88: return Cyc_Absyn_sizeofexp_exp(
Cyc_Tcutil_deep_copy_exp(_tmp164),e->loc);_LL89: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*
_tmp165=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp165->tag
!= 18)goto _LL8B;else{_tmp166=(void*)_tmp165->f1;_tmp167=(void*)_tmp165->f2;}}
_LL8A: return Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp166),_tmp167,e->loc);
_LL8B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp168=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)
_tmp12E;if(_tmp168->tag != 19)goto _LL8D;else{_tmp169=_tmp168->f1;}}_LL8C: return
Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp169),e->loc);_LL8D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*
_tmp16A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp16A->tag
!= 20)goto _LL8F;else{_tmp16B=_tmp16A->f1;_tmp16C=_tmp16A->f2;_tmp16D=_tmp16A->f3;
_tmp16E=_tmp16A->f4;}}_LL8E: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct
_tmp114A;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1149;return Cyc_Absyn_new_exp((
void*)((_tmp1149=_cycalloc(sizeof(*_tmp1149)),((_tmp1149[0]=((_tmp114A.tag=20,((
_tmp114A.f1=Cyc_Tcutil_deep_copy_exp(_tmp16B),((_tmp114A.f2=_tmp16C,((_tmp114A.f3=
_tmp16D,((_tmp114A.f4=_tmp16E,_tmp114A)))))))))),_tmp1149)))),e->loc);}_LL8F: {
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp16F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)
_tmp12E;if(_tmp16F->tag != 21)goto _LL91;else{_tmp170=_tmp16F->f1;_tmp171=_tmp16F->f2;
_tmp172=_tmp16F->f3;_tmp173=_tmp16F->f4;}}_LL90: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct
_tmp114D;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp114C;return Cyc_Absyn_new_exp((
void*)((_tmp114C=_cycalloc(sizeof(*_tmp114C)),((_tmp114C[0]=((_tmp114D.tag=21,((
_tmp114D.f1=Cyc_Tcutil_deep_copy_exp(_tmp170),((_tmp114D.f2=_tmp171,((_tmp114D.f3=
_tmp172,((_tmp114D.f4=_tmp173,_tmp114D)))))))))),_tmp114C)))),e->loc);}_LL91: {
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)
_tmp12E;if(_tmp174->tag != 22)goto _LL93;else{_tmp175=_tmp174->f1;_tmp176=_tmp174->f2;}}
_LL92: return Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmp175),Cyc_Tcutil_deep_copy_exp(
_tmp176),e->loc);_LL93: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp177=(
struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp177->tag != 23)goto
_LL95;else{_tmp178=_tmp177->f1;}}_LL94: return Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_deep_copy_exp,_tmp178),e->loc);_LL95: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*
_tmp179=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp179->tag
!= 24)goto _LL97;else{_tmp17A=_tmp179->f1;_tmp17B=*_tmp17A;_tmp17C=_tmp17B.f1;
_tmp17D=_tmp17B.f2;_tmp17E=_tmp17B.f3;_tmp17F=_tmp179->f2;}}_LL96: {struct
_dyneither_ptr*vopt1=_tmp17C;if(_tmp17C != 0)vopt1=_tmp17C;{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct
_tmp1153;struct _tuple10*_tmp1152;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*
_tmp1151;return Cyc_Absyn_new_exp((void*)((_tmp1151=_cycalloc(sizeof(*_tmp1151)),((
_tmp1151[0]=((_tmp1153.tag=24,((_tmp1153.f1=((_tmp1152=_cycalloc(sizeof(*
_tmp1152)),((_tmp1152->f1=vopt1,((_tmp1152->f2=_tmp17D,((_tmp1152->f3=Cyc_Tcutil_copy_type(
_tmp17E),_tmp1152)))))))),((_tmp1153.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*
f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp17F),_tmp1153)))))),_tmp1151)))),e->loc);};}_LL97: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*
_tmp180=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp180->tag != 
25)goto _LL99;else{_tmp181=_tmp180->f1;}}_LL98: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct
_tmp1156;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1155;return Cyc_Absyn_new_exp((
void*)((_tmp1155=_cycalloc(sizeof(*_tmp1155)),((_tmp1155[0]=((_tmp1156.tag=25,((
_tmp1156.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp181),_tmp1156)))),
_tmp1155)))),e->loc);}_LL99: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*
_tmp182=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp12E;if(
_tmp182->tag != 26)goto _LL9B;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;_tmp185=
_tmp182->f3;_tmp186=_tmp182->f4;}}_LL9A: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct
_tmp1159;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1158;return Cyc_Absyn_new_exp((
void*)((_tmp1158=_cycalloc(sizeof(*_tmp1158)),((_tmp1158[0]=((_tmp1159.tag=26,((
_tmp1159.f1=_tmp183,((_tmp1159.f2=Cyc_Tcutil_deep_copy_exp(_tmp184),((_tmp1159.f3=
Cyc_Tcutil_deep_copy_exp(_tmp185),((_tmp1159.f4=_tmp186,_tmp1159)))))))))),
_tmp1158)))),e->loc);}_LL9B: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*
_tmp187=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp187->tag
!= 27)goto _LL9D;else{_tmp188=_tmp187->f1;_tmp189=_tmp187->f2;_tmp18A=_tmp187->f3;
_tmp18B=_tmp187->f4;}}_LL9C: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct
_tmp115C;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp115B;return Cyc_Absyn_new_exp((
void*)((_tmp115B=_cycalloc(sizeof(*_tmp115B)),((_tmp115B[0]=((_tmp115C.tag=27,((
_tmp115C.f1=_tmp188,((_tmp115C.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp189),((_tmp115C.f3=((
struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp18A),((_tmp115C.f4=_tmp18B,_tmp115C)))))))))),
_tmp115B)))),e->loc);}_LL9D: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*
_tmp18C=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp18C->tag
!= 28)goto _LL9F;else{_tmp18D=(void*)_tmp18C->f1;_tmp18E=_tmp18C->f2;}}_LL9E: {
struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp115F;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*
_tmp115E;return Cyc_Absyn_new_exp((void*)((_tmp115E=_cycalloc(sizeof(*_tmp115E)),((
_tmp115E[0]=((_tmp115F.tag=28,((_tmp115F.f1=(void*)Cyc_Tcutil_copy_type(_tmp18D),((
_tmp115F.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp18E),_tmp115F)))))),
_tmp115E)))),e->loc);}_LL9F: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*
_tmp18F=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp18F->tag
!= 29)goto _LLA1;else{_tmp190=_tmp18F->f1;_tmp191=_tmp18F->f2;_tmp192=_tmp18F->f3;}}
_LLA0: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp1162;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*
_tmp1161;return Cyc_Absyn_new_exp((void*)((_tmp1161=_cycalloc(sizeof(*_tmp1161)),((
_tmp1161[0]=((_tmp1162.tag=29,((_tmp1162.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,
_tmp190),((_tmp1162.f2=_tmp191,((_tmp1162.f3=_tmp192,_tmp1162)))))))),_tmp1161)))),
e->loc);}_LLA1: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp193=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)
_tmp12E;if(_tmp193->tag != 30)goto _LLA3;else{_tmp194=_tmp193->f1;_tmp195=_tmp193->f2;
_tmp196=_tmp193->f3;}}_LLA2: return e;_LLA3: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*
_tmp197=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp197->tag
!= 31)goto _LLA5;else{_tmp198=_tmp197->f1;_tmp199=(void*)_tmp197->f2;_tmp19A=
_tmp197->f3;}}_LLA4: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp1165;
struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp1164;return Cyc_Absyn_new_exp((
void*)((_tmp1164=_cycalloc(sizeof(*_tmp1164)),((_tmp1164[0]=((_tmp1165.tag=31,((
_tmp1165.f1=_tmp198,((_tmp1165.f2=(void*)Cyc_Tcutil_copy_type(_tmp199),((
_tmp1165.f3=_tmp19A,_tmp1165)))))))),_tmp1164)))),e->loc);}_LLA5: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*
_tmp19B=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp19B->tag
!= 32)goto _LLA7;else{_tmp19C=_tmp19B->f1;_tmp19D=_tmp19C.is_calloc;_tmp19E=
_tmp19C.rgn;_tmp19F=_tmp19C.elt_type;_tmp1A0=_tmp19C.num_elts;_tmp1A1=_tmp19C.fat_result;}}
_LLA6: {struct Cyc_Absyn_Exp*_tmp1D6=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*r1=
_tmp19E;if(_tmp19E != 0)r1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((struct
Cyc_Absyn_Exp*)_tmp19E);{void**t1=_tmp19F;if(_tmp19F != 0){void**_tmp1166;t1=((
_tmp1166=_cycalloc(sizeof(*_tmp1166)),((_tmp1166[0]=Cyc_Tcutil_copy_type(*
_tmp19F),_tmp1166))));}{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp116C;
struct Cyc_Absyn_MallocInfo _tmp116B;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*
_tmp116A;_tmp1D6->r=(void*)((_tmp116A=_cycalloc(sizeof(*_tmp116A)),((_tmp116A[0]=((
_tmp116C.tag=32,((_tmp116C.f1=((_tmp116B.is_calloc=_tmp19D,((_tmp116B.rgn=r1,((
_tmp116B.elt_type=t1,((_tmp116B.num_elts=_tmp1A0,((_tmp116B.fat_result=_tmp1A1,
_tmp116B)))))))))),_tmp116C)))),_tmp116A))));}return _tmp1D6;};}_LLA7: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*
_tmp1A2=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1A2->tag != 
33)goto _LLA9;else{_tmp1A3=_tmp1A2->f1;_tmp1A4=_tmp1A2->f2;}}_LLA8: return Cyc_Absyn_swap_exp(
Cyc_Tcutil_deep_copy_exp(_tmp1A3),Cyc_Tcutil_deep_copy_exp(_tmp1A4),e->loc);
_LLA9: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1A5=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)
_tmp12E;if(_tmp1A5->tag != 34)goto _LLAB;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A5->f2;}}
_LLAA: {struct Cyc_Core_Opt*nopt1=_tmp1A6;if(_tmp1A6 != 0){struct Cyc_Core_Opt*
_tmp116D;nopt1=((_tmp116D=_cycalloc(sizeof(*_tmp116D)),((_tmp116D->v=(struct
_tuple2*)_tmp1A6->v,_tmp116D))));}{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct
_tmp1170;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp116F;return Cyc_Absyn_new_exp((
void*)((_tmp116F=_cycalloc(sizeof(*_tmp116F)),((_tmp116F[0]=((_tmp1170.tag=34,((
_tmp1170.f1=nopt1,((_tmp1170.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(
struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp1A7),_tmp1170)))))),_tmp116F)))),e->loc);};}_LLAB: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*
_tmp1A8=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1A8->tag
!= 35)goto _LLAD;}_LLAC: {struct Cyc_Core_Failure_exn_struct _tmp1176;const char*
_tmp1175;struct Cyc_Core_Failure_exn_struct*_tmp1174;(int)_throw((void*)((
_tmp1174=_cycalloc(sizeof(*_tmp1174)),((_tmp1174[0]=((_tmp1176.tag=Cyc_Core_Failure,((
_tmp1176.f1=((_tmp1175="deep_copy: statement expressions unsupported",
_tag_dyneither(_tmp1175,sizeof(char),45))),_tmp1176)))),_tmp1174)))));}_LLAD: {
struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1A9=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)
_tmp12E;if(_tmp1A9->tag != 36)goto _LLAF;else{_tmp1AA=_tmp1A9->f1;_tmp1AB=_tmp1A9->f2;}}
_LLAE: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp1179;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*
_tmp1178;return Cyc_Absyn_new_exp((void*)((_tmp1178=_cycalloc(sizeof(*_tmp1178)),((
_tmp1178[0]=((_tmp1179.tag=36,((_tmp1179.f1=Cyc_Tcutil_deep_copy_exp(_tmp1AA),((
_tmp1179.f2=_tmp1AB,_tmp1179)))))),_tmp1178)))),e->loc);}_LLAF: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*
_tmp1AC=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1AC->tag
!= 37)goto _LLB1;else{_tmp1AD=(void*)_tmp1AC->f1;}}_LLB0: return Cyc_Absyn_valueof_exp(
Cyc_Tcutil_copy_type(_tmp1AD),e->loc);_LLB1: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*
_tmp1AE=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1AE->tag != 
38)goto _LL64;else{_tmp1AF=_tmp1AE->f1;_tmp1B0=_tmp1AE->f2;}}_LLB2: return Cyc_Absyn_asm_exp(
_tmp1AF,_tmp1B0,e->loc);_LL64:;}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum 
Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual
f2;};int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind _tmp1E4;enum Cyc_Absyn_KindQual _tmp1E5;enum Cyc_Absyn_AliasQual
_tmp1E6;struct Cyc_Absyn_Kind*_tmp1E3=ka1;_tmp1E4=*_tmp1E3;_tmp1E5=_tmp1E4.kind;
_tmp1E6=_tmp1E4.aliasqual;{struct Cyc_Absyn_Kind _tmp1E8;enum Cyc_Absyn_KindQual
_tmp1E9;enum Cyc_Absyn_AliasQual _tmp1EA;struct Cyc_Absyn_Kind*_tmp1E7=ka2;_tmp1E8=*
_tmp1E7;_tmp1E9=_tmp1E8.kind;_tmp1EA=_tmp1E8.aliasqual;if(_tmp1E5 != _tmp1E9){
struct _tuple20 _tmp117A;struct _tuple20 _tmp1EC=(_tmp117A.f1=_tmp1E5,((_tmp117A.f2=
_tmp1E9,_tmp117A)));enum Cyc_Absyn_KindQual _tmp1ED;enum Cyc_Absyn_KindQual
_tmp1EE;enum Cyc_Absyn_KindQual _tmp1EF;enum Cyc_Absyn_KindQual _tmp1F0;enum Cyc_Absyn_KindQual
_tmp1F1;enum Cyc_Absyn_KindQual _tmp1F2;_LLB9: _tmp1ED=_tmp1EC.f1;if(_tmp1ED != Cyc_Absyn_BoxKind)
goto _LLBB;_tmp1EE=_tmp1EC.f2;if(_tmp1EE != Cyc_Absyn_MemKind)goto _LLBB;_LLBA: goto
_LLBC;_LLBB: _tmp1EF=_tmp1EC.f1;if(_tmp1EF != Cyc_Absyn_BoxKind)goto _LLBD;_tmp1F0=
_tmp1EC.f2;if(_tmp1F0 != Cyc_Absyn_AnyKind)goto _LLBD;_LLBC: goto _LLBE;_LLBD:
_tmp1F1=_tmp1EC.f1;if(_tmp1F1 != Cyc_Absyn_MemKind)goto _LLBF;_tmp1F2=_tmp1EC.f2;
if(_tmp1F2 != Cyc_Absyn_AnyKind)goto _LLBF;_LLBE: goto _LLB8;_LLBF:;_LLC0: return 0;
_LLB8:;}if(_tmp1E6 != _tmp1EA){struct _tuple21 _tmp117B;struct _tuple21 _tmp1F4=(
_tmp117B.f1=_tmp1E6,((_tmp117B.f2=_tmp1EA,_tmp117B)));enum Cyc_Absyn_AliasQual
_tmp1F5;enum Cyc_Absyn_AliasQual _tmp1F6;enum Cyc_Absyn_AliasQual _tmp1F7;enum Cyc_Absyn_AliasQual
_tmp1F8;_LLC2: _tmp1F5=_tmp1F4.f1;if(_tmp1F5 != Cyc_Absyn_Aliasable)goto _LLC4;
_tmp1F6=_tmp1F4.f2;if(_tmp1F6 != Cyc_Absyn_Top)goto _LLC4;_LLC3: goto _LLC5;_LLC4:
_tmp1F7=_tmp1F4.f1;if(_tmp1F7 != Cyc_Absyn_Unique)goto _LLC6;_tmp1F8=_tmp1F4.f2;
if(_tmp1F8 != Cyc_Absyn_Top)goto _LLC6;_LLC5: return 1;_LLC6:;_LLC7: return 0;_LLC1:;}
return 1;};}struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,
struct Cyc_Absyn_Kind*def){void*_tmp1F9=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*
_tmp1FB;struct Cyc_Absyn_Kind*_tmp1FD;_LLC9: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*
_tmp1FA=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1F9;if(_tmp1FA->tag != 
0)goto _LLCB;else{_tmp1FB=_tmp1FA->f1;}}_LLCA: return _tmp1FB;_LLCB: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmp1FC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1F9;if(_tmp1FC->tag
!= 2)goto _LLCD;else{_tmp1FD=_tmp1FC->f2;}}_LLCC: return _tmp1FD;_LLCD:;_LLCE:{
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp117E;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmp117D;tv->kind=(void*)((_tmp117D=_cycalloc(sizeof(*_tmp117D)),((_tmp117D[0]=((
_tmp117E.tag=2,((_tmp117E.f1=0,((_tmp117E.f2=def,_tmp117E)))))),_tmp117D))));}
return def;_LLC8:;}int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){struct _tuple0
_tmp117F;struct _tuple0 _tmp201=(_tmp117F.f1=Cyc_Absyn_compress_kb(kb1),((_tmp117F.f2=
Cyc_Absyn_compress_kb(kb2),_tmp117F)));void*_tmp202;struct Cyc_Absyn_Kind*_tmp204;
void*_tmp205;struct Cyc_Absyn_Kind*_tmp207;void*_tmp208;struct Cyc_Core_Opt*
_tmp20A;struct Cyc_Core_Opt**_tmp20B;struct Cyc_Absyn_Kind*_tmp20C;void*_tmp20D;
struct Cyc_Absyn_Kind*_tmp20F;void*_tmp210;struct Cyc_Absyn_Kind*_tmp212;void*
_tmp213;struct Cyc_Core_Opt*_tmp215;struct Cyc_Core_Opt**_tmp216;struct Cyc_Absyn_Kind*
_tmp217;void*_tmp218;struct Cyc_Core_Opt*_tmp21A;struct Cyc_Core_Opt**_tmp21B;
struct Cyc_Absyn_Kind*_tmp21C;void*_tmp21D;struct Cyc_Core_Opt*_tmp21F;struct Cyc_Core_Opt**
_tmp220;struct Cyc_Absyn_Kind*_tmp221;void*_tmp222;struct Cyc_Core_Opt*_tmp224;
struct Cyc_Core_Opt**_tmp225;void*_tmp226;void*_tmp227;void*_tmp228;struct Cyc_Core_Opt*
_tmp22A;struct Cyc_Core_Opt**_tmp22B;_LLD0: _tmp202=_tmp201.f1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*
_tmp203=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp202;if(_tmp203->tag != 
0)goto _LLD2;else{_tmp204=_tmp203->f1;}};_tmp205=_tmp201.f2;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*
_tmp206=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp205;if(_tmp206->tag != 
0)goto _LLD2;else{_tmp207=_tmp206->f1;}};_LLD1: return _tmp204 == _tmp207;_LLD2:
_tmp208=_tmp201.f1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp209=(
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp208;if(_tmp209->tag != 2)goto
_LLD4;else{_tmp20A=_tmp209->f1;_tmp20B=(struct Cyc_Core_Opt**)& _tmp209->f1;
_tmp20C=_tmp209->f2;}};_tmp20D=_tmp201.f2;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*
_tmp20E=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp20D;if(_tmp20E->tag != 
0)goto _LLD4;else{_tmp20F=_tmp20E->f1;}};_LLD3: if(Cyc_Tcutil_kind_leq(_tmp20F,
_tmp20C)){{struct Cyc_Core_Opt*_tmp1180;*_tmp20B=((_tmp1180=_cycalloc(sizeof(*
_tmp1180)),((_tmp1180->v=kb2,_tmp1180))));}return 1;}else{return 0;}_LLD4: _tmp210=
_tmp201.f1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp211=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)
_tmp210;if(_tmp211->tag != 0)goto _LLD6;else{_tmp212=_tmp211->f1;}};_tmp213=
_tmp201.f2;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp214=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)
_tmp213;if(_tmp214->tag != 2)goto _LLD6;else{_tmp215=_tmp214->f1;_tmp216=(struct
Cyc_Core_Opt**)& _tmp214->f1;_tmp217=_tmp214->f2;}};_LLD5: if(Cyc_Tcutil_kind_leq(
_tmp212,_tmp217)){{struct Cyc_Core_Opt*_tmp1181;*_tmp216=((_tmp1181=_cycalloc(
sizeof(*_tmp1181)),((_tmp1181->v=kb1,_tmp1181))));}return 1;}else{return 0;}_LLD6:
_tmp218=_tmp201.f1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp219=(
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp218;if(_tmp219->tag != 2)goto
_LLD8;else{_tmp21A=_tmp219->f1;_tmp21B=(struct Cyc_Core_Opt**)& _tmp219->f1;
_tmp21C=_tmp219->f2;}};_tmp21D=_tmp201.f2;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmp21E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp21D;if(_tmp21E->tag
!= 2)goto _LLD8;else{_tmp21F=_tmp21E->f1;_tmp220=(struct Cyc_Core_Opt**)& _tmp21E->f1;
_tmp221=_tmp21E->f2;}};_LLD7: if(Cyc_Tcutil_kind_leq(_tmp21C,_tmp221)){{struct Cyc_Core_Opt*
_tmp1182;*_tmp220=((_tmp1182=_cycalloc(sizeof(*_tmp1182)),((_tmp1182->v=kb1,
_tmp1182))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp221,_tmp21C)){{struct Cyc_Core_Opt*
_tmp1183;*_tmp21B=((_tmp1183=_cycalloc(sizeof(*_tmp1183)),((_tmp1183->v=kb2,
_tmp1183))));}return 1;}else{return 0;}}_LLD8: _tmp222=_tmp201.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*
_tmp223=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp222;if(_tmp223->tag
!= 1)goto _LLDA;else{_tmp224=_tmp223->f1;_tmp225=(struct Cyc_Core_Opt**)& _tmp223->f1;}};
_tmp226=_tmp201.f2;_LLD9: _tmp227=_tmp226;_tmp22B=_tmp225;goto _LLDB;_LLDA: _tmp227=
_tmp201.f1;_tmp228=_tmp201.f2;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*
_tmp229=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp228;if(_tmp229->tag
!= 1)goto _LLCF;else{_tmp22A=_tmp229->f1;_tmp22B=(struct Cyc_Core_Opt**)& _tmp229->f1;}};
_LLDB:{struct Cyc_Core_Opt*_tmp1184;*_tmp22B=((_tmp1184=_cycalloc(sizeof(*
_tmp1184)),((_tmp1184->v=_tmp227,_tmp1184))));}return 1;_LLCF:;}struct _tuple16 Cyc_Tcutil_swap_kind(
void*t,void*kb){void*_tmp231=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp233;
_LLDD: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp232=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)
_tmp231;if(_tmp232->tag != 2)goto _LLDF;else{_tmp233=_tmp232->f1;}}_LLDE: {void*
_tmp234=_tmp233->kind;_tmp233->kind=kb;{struct _tuple16 _tmp1185;return(_tmp1185.f1=
_tmp233,((_tmp1185.f2=_tmp234,_tmp1185)));};}_LLDF:;_LLE0: {const char*_tmp1189;
void*_tmp1188[1];struct Cyc_String_pa_PrintArg_struct _tmp1187;(_tmp1187.tag=0,((
_tmp1187.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp1188[0]=& _tmp1187,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1189="swap_kind: cannot update the kind of %s",
_tag_dyneither(_tmp1189,sizeof(char),40))),_tag_dyneither(_tmp1188,sizeof(void*),
1)))))));}_LLDC:;}struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){void*_tmp239=
Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp23B;struct Cyc_Absyn_Tvar*_tmp23D;
enum Cyc_Absyn_Size_of _tmp240;struct Cyc_Absyn_DatatypeFieldInfo _tmp24C;union Cyc_Absyn_DatatypeFieldInfoU
_tmp24D;struct _tuple3 _tmp24E;struct Cyc_Absyn_Datatypedecl*_tmp24F;struct Cyc_Absyn_Datatypefield*
_tmp250;struct Cyc_Absyn_DatatypeFieldInfo _tmp252;union Cyc_Absyn_DatatypeFieldInfoU
_tmp253;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp254;struct Cyc_Absyn_AggrInfo
_tmp256;union Cyc_Absyn_AggrInfoU _tmp257;struct _tuple4 _tmp258;struct Cyc_Absyn_AggrInfo
_tmp25A;union Cyc_Absyn_AggrInfoU _tmp25B;struct Cyc_Absyn_Aggrdecl**_tmp25C;struct
Cyc_Absyn_Aggrdecl*_tmp25D;struct Cyc_Absyn_Aggrdecl _tmp25E;struct Cyc_List_List*
_tmp25F;struct Cyc_Absyn_AggrdeclImpl*_tmp260;struct Cyc_List_List*_tmp261;struct
Cyc_Absyn_PtrInfo _tmp264;struct Cyc_Absyn_Typedefdecl*_tmp26A;struct Cyc_Absyn_TypeDecl*
_tmp26C;struct Cyc_Absyn_TypeDecl _tmp26D;void*_tmp26E;struct Cyc_Absyn_TypeDecl*
_tmp271;struct Cyc_Absyn_TypeDecl _tmp272;void*_tmp273;struct Cyc_Absyn_TypeDecl*
_tmp276;struct Cyc_Absyn_TypeDecl _tmp277;void*_tmp278;_LLE2: {struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp23A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp239;if(_tmp23A->tag != 1)
goto _LLE4;else{_tmp23B=_tmp23A->f1;}}_LLE3: return(struct Cyc_Absyn_Kind*)((struct
Cyc_Core_Opt*)_check_null(_tmp23B))->v;_LLE4: {struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp23C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp239;if(_tmp23C->tag != 2)
goto _LLE6;else{_tmp23D=_tmp23C->f1;}}_LLE5: return Cyc_Tcutil_tvar_kind(_tmp23D,&
Cyc_Tcutil_bk);_LLE6: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp23E=(
struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp239;if(_tmp23E->tag != 0)goto
_LLE8;}_LLE7: return& Cyc_Tcutil_mk;_LLE8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp23F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp239;if(_tmp23F->tag != 6)
goto _LLEA;else{_tmp240=_tmp23F->f2;}}_LLE9: return(_tmp240 == (enum Cyc_Absyn_Size_of)
Cyc_Absyn_Int_sz  || _tmp240 == (enum Cyc_Absyn_Size_of)Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:&
Cyc_Tcutil_mk;_LLEA: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp241=(
struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp239;if(_tmp241->tag != 7)goto
_LLEC;}_LLEB: goto _LLED;_LLEC: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp242=(
struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp239;if(_tmp242->tag != 9)goto _LLEE;}
_LLED: return& Cyc_Tcutil_mk;_LLEE: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*
_tmp243=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp239;if(_tmp243->tag != 
16)goto _LLF0;}_LLEF: goto _LLF1;_LLF0: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp244=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp239;if(_tmp244->tag != 
13)goto _LLF2;}_LLF1: goto _LLF3;_LLF2: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*
_tmp245=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp239;if(_tmp245->tag
!= 14)goto _LLF4;}_LLF3: goto _LLF5;_LLF4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*
_tmp246=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp239;if(_tmp246->tag
!= 15)goto _LLF6;}_LLF5: return& Cyc_Tcutil_bk;_LLF6: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*
_tmp247=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp239;if(_tmp247->tag != 
21)goto _LLF8;}_LLF7: return& Cyc_Tcutil_urk;_LLF8: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*
_tmp248=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp239;if(_tmp248->tag != 20)
goto _LLFA;}_LLF9: return& Cyc_Tcutil_rk;_LLFA: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*
_tmp249=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp239;if(_tmp249->tag != 
22)goto _LLFC;}_LLFB: return& Cyc_Tcutil_trk;_LLFC: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*
_tmp24A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp239;if(_tmp24A->tag
!= 3)goto _LLFE;}_LLFD: return& Cyc_Tcutil_ak;_LLFE: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmp24B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp239;if(_tmp24B->tag
!= 4)goto _LL100;else{_tmp24C=_tmp24B->f1;_tmp24D=_tmp24C.field_info;if((_tmp24D.KnownDatatypefield).tag
!= 2)goto _LL100;_tmp24E=(struct _tuple3)(_tmp24D.KnownDatatypefield).val;_tmp24F=
_tmp24E.f1;_tmp250=_tmp24E.f2;}}_LLFF: return& Cyc_Tcutil_mk;_LL100: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmp251=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp239;if(_tmp251->tag
!= 4)goto _LL102;else{_tmp252=_tmp251->f1;_tmp253=_tmp252.field_info;if((_tmp253.UnknownDatatypefield).tag
!= 1)goto _LL102;_tmp254=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp253.UnknownDatatypefield).val;}}
_LL101: {const char*_tmp118C;void*_tmp118B;(_tmp118B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp118C="typ_kind: Unresolved DatatypeFieldType",
_tag_dyneither(_tmp118C,sizeof(char),39))),_tag_dyneither(_tmp118B,sizeof(void*),
0)));}_LL102: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp255=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp239;if(_tmp255->tag != 11)goto _LL104;else{_tmp256=_tmp255->f1;_tmp257=_tmp256.aggr_info;
if((_tmp257.UnknownAggr).tag != 1)goto _LL104;_tmp258=(struct _tuple4)(_tmp257.UnknownAggr).val;}}
_LL103: return& Cyc_Tcutil_ak;_LL104: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp259=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp239;if(_tmp259->tag != 
11)goto _LL106;else{_tmp25A=_tmp259->f1;_tmp25B=_tmp25A.aggr_info;if((_tmp25B.KnownAggr).tag
!= 2)goto _LL106;_tmp25C=(struct Cyc_Absyn_Aggrdecl**)(_tmp25B.KnownAggr).val;
_tmp25D=*_tmp25C;_tmp25E=*_tmp25D;_tmp25F=_tmp25E.tvs;_tmp260=_tmp25E.impl;
_tmp261=_tmp25A.targs;}}_LL105: if(_tmp260 == 0)return& Cyc_Tcutil_ak;{struct Cyc_List_List*
_tmp27F=_tmp260->fields;if(_tmp27F == 0)return& Cyc_Tcutil_mk;for(0;_tmp27F->tl != 
0;_tmp27F=_tmp27F->tl){;}{void*_tmp280=((struct Cyc_Absyn_Aggrfield*)_tmp27F->hd)->type;
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(_tmp280);if(_tmp261 != 0  && (k == & Cyc_Tcutil_ak
 || k == & Cyc_Tcutil_tak)){struct _RegionHandle _tmp281=_new_region("temp");struct
_RegionHandle*temp=& _tmp281;_push_region(temp);{struct Cyc_List_List*_tmp282=0;
for(0;_tmp25F != 0;(_tmp25F=_tmp25F->tl,_tmp261=_tmp261->tl)){struct Cyc_Absyn_Tvar*
_tmp283=(struct Cyc_Absyn_Tvar*)_tmp25F->hd;void*_tmp284=(void*)((struct Cyc_List_List*)
_check_null(_tmp261))->hd;struct Cyc_Absyn_Kind*_tmp285=Cyc_Tcutil_tvar_kind(
_tmp283,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind _tmp286;enum Cyc_Absyn_KindQual
_tmp287;_LL121: _tmp286=*_tmp285;_tmp287=_tmp286.kind;if(_tmp287 != Cyc_Absyn_AnyKind)
goto _LL123;_LL122:{struct _tuple16*_tmp118F;struct Cyc_List_List*_tmp118E;_tmp282=((
_tmp118E=_region_malloc(temp,sizeof(*_tmp118E)),((_tmp118E->hd=((_tmp118F=
_region_malloc(temp,sizeof(*_tmp118F)),((_tmp118F->f1=_tmp283,((_tmp118F->f2=
_tmp284,_tmp118F)))))),((_tmp118E->tl=_tmp282,_tmp118E))))));}goto _LL120;_LL123:;
_LL124: goto _LL120;_LL120:;}if(_tmp282 != 0){_tmp280=Cyc_Tcutil_rsubstitute(temp,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp282),
_tmp280);k=Cyc_Tcutil_typ_kind(_tmp280);}if(k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak){
struct Cyc_Absyn_Kind*_tmp28A=k;_npop_handler(0);return _tmp28A;}};_pop_region(
temp);}return& Cyc_Tcutil_mk;};};_LL106: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmp262=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp239;if(_tmp262->tag
!= 12)goto _LL108;}_LL107: return& Cyc_Tcutil_mk;_LL108: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp263=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp239;if(_tmp263->tag
!= 5)goto _LL10A;else{_tmp264=_tmp263->f1;}}_LL109: {void*_tmp28B=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(
_tmp264.ptr_atts).bounds);_LL126: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp28C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp28B;if(_tmp28C->tag
!= 0)goto _LL128;}_LL127: switch((Cyc_Tcutil_typ_kind((_tmp264.ptr_atts).rgn))->aliasqual){
case Cyc_Absyn_Aliasable: _LL12A: return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL12B:
return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL12C: return& Cyc_Tcutil_tmk;}_LL128: {
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp28D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)
_tmp28B;if(_tmp28D->tag != 1)goto _LL125;}_LL129: switch((Cyc_Tcutil_typ_kind((
_tmp264.ptr_atts).rgn))->aliasqual){case Cyc_Absyn_Aliasable: _LL12E: return& Cyc_Tcutil_bk;
case Cyc_Absyn_Unique: _LL12F: return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL130:
return& Cyc_Tcutil_tbk;}_LL125:;}_LL10A: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*
_tmp265=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp239;if(_tmp265->tag
!= 18)goto _LL10C;}_LL10B: return& Cyc_Tcutil_ik;_LL10C: {struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmp266=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp239;if(_tmp266->tag != 19)
goto _LL10E;}_LL10D: return& Cyc_Tcutil_bk;_LL10E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp267=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp239;if(_tmp267->tag != 
8)goto _LL110;}_LL10F: goto _LL111;_LL110: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmp268=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp239;if(_tmp268->tag != 
10)goto _LL112;}_LL111: return& Cyc_Tcutil_mk;_LL112: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*
_tmp269=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp239;if(_tmp269->tag
!= 17)goto _LL114;else{_tmp26A=_tmp269->f3;}}_LL113: if(_tmp26A == 0  || _tmp26A->kind
== 0){const char*_tmp1193;void*_tmp1192[1];struct Cyc_String_pa_PrintArg_struct
_tmp1191;(_tmp1191.tag=0,((_tmp1191.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1192[0]=& _tmp1191,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1193="typ_kind: typedef found: %s",
_tag_dyneither(_tmp1193,sizeof(char),28))),_tag_dyneither(_tmp1192,sizeof(void*),
1)))))));}return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp26A->kind))->v;
_LL114: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp26B=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)
_tmp239;if(_tmp26B->tag != 26)goto _LL116;else{_tmp26C=_tmp26B->f1;_tmp26D=*
_tmp26C;_tmp26E=_tmp26D.r;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*
_tmp26F=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp26E;if(_tmp26F->tag
!= 0)goto _LL116;};}}_LL115: return& Cyc_Tcutil_ak;_LL116: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*
_tmp270=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp239;if(_tmp270->tag
!= 26)goto _LL118;else{_tmp271=_tmp270->f1;_tmp272=*_tmp271;_tmp273=_tmp272.r;{
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp274=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)
_tmp273;if(_tmp274->tag != 1)goto _LL118;};}}_LL117: return& Cyc_Tcutil_bk;_LL118: {
struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp275=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)
_tmp239;if(_tmp275->tag != 26)goto _LL11A;else{_tmp276=_tmp275->f1;_tmp277=*
_tmp276;_tmp278=_tmp277.r;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*
_tmp279=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp278;if(
_tmp279->tag != 2)goto _LL11A;};}}_LL119: return& Cyc_Tcutil_ak;_LL11A: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*
_tmp27A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp239;if(_tmp27A->tag != 
23)goto _LL11C;}_LL11B: goto _LL11D;_LL11C: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmp27B=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp239;if(_tmp27B->tag != 24)
goto _LL11E;}_LL11D: goto _LL11F;_LL11E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*
_tmp27C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp239;if(_tmp27C->tag != 25)
goto _LLE1;}_LL11F: return& Cyc_Tcutil_ek;_LLE1:;}int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*
k1,struct Cyc_Absyn_Kind*k2){return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual
== k2->aliasqual;}int Cyc_Tcutil_unify(void*t1,void*t2){struct _handler_cons
_tmp291;_push_handler(& _tmp291);{int _tmp293=0;if(setjmp(_tmp291.handler))_tmp293=
1;if(!_tmp293){Cyc_Tcutil_unify_it(t1,t2);{int _tmp294=1;_npop_handler(0);return
_tmp294;};;_pop_handler();}else{void*_tmp292=(void*)_exn_thrown;void*_tmp296=
_tmp292;_LL133: {struct Cyc_Tcutil_Unify_exn_struct*_tmp297=(struct Cyc_Tcutil_Unify_exn_struct*)
_tmp296;if(_tmp297->tag != Cyc_Tcutil_Unify)goto _LL135;}_LL134: return 0;_LL135:;
_LL136:(void)_throw(_tmp296);_LL132:;}};}static void Cyc_Tcutil_occurslist(void*
evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,void*t){t=Cyc_Tcutil_compress(t);{void*_tmp298=t;struct Cyc_Absyn_Tvar*
_tmp29A;void*_tmp29C;struct Cyc_Core_Opt*_tmp29D;struct Cyc_Core_Opt**_tmp29E;
struct Cyc_Absyn_PtrInfo _tmp2A0;struct Cyc_Absyn_ArrayInfo _tmp2A2;void*_tmp2A3;
struct Cyc_Absyn_FnInfo _tmp2A5;struct Cyc_List_List*_tmp2A6;void*_tmp2A7;struct Cyc_Absyn_Tqual
_tmp2A8;void*_tmp2A9;struct Cyc_List_List*_tmp2AA;int _tmp2AB;struct Cyc_Absyn_VarargInfo*
_tmp2AC;struct Cyc_List_List*_tmp2AD;struct Cyc_List_List*_tmp2AE;struct Cyc_List_List*
_tmp2B0;struct Cyc_Absyn_DatatypeInfo _tmp2B2;struct Cyc_List_List*_tmp2B3;struct
Cyc_List_List*_tmp2B5;struct Cyc_Absyn_DatatypeFieldInfo _tmp2B7;struct Cyc_List_List*
_tmp2B8;struct Cyc_Absyn_AggrInfo _tmp2BA;struct Cyc_List_List*_tmp2BB;struct Cyc_List_List*
_tmp2BD;void*_tmp2BF;void*_tmp2C1;void*_tmp2C3;void*_tmp2C5;struct Cyc_List_List*
_tmp2C7;_LL138: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp299=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)
_tmp298;if(_tmp299->tag != 2)goto _LL13A;else{_tmp29A=_tmp299->f1;}}_LL139: if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp29A)){{
const char*_tmp1194;Cyc_Tcutil_failure_reason=((_tmp1194="(type variable would escape scope)",
_tag_dyneither(_tmp1194,sizeof(char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
goto _LL137;_LL13A: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp29B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)
_tmp298;if(_tmp29B->tag != 1)goto _LL13C;else{_tmp29C=(void*)_tmp29B->f2;_tmp29D=
_tmp29B->f4;_tmp29E=(struct Cyc_Core_Opt**)& _tmp29B->f4;}}_LL13B: if(t == evar){{
const char*_tmp1195;Cyc_Tcutil_failure_reason=((_tmp1195="(occurs check)",
_tag_dyneither(_tmp1195,sizeof(char),15)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
else{if(_tmp29C != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp29C);else{int problem=
0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*
_tmp29E))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){problem=1;break;}}}
if(problem){struct Cyc_List_List*_tmp2CA=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp29E))->v;for(0;s != 0;s=s->tl){if(((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){struct Cyc_List_List*_tmp1196;_tmp2CA=((_tmp1196=_cycalloc(sizeof(*
_tmp1196)),((_tmp1196->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmp1196->tl=_tmp2CA,
_tmp1196))))));}}}{struct Cyc_Core_Opt*_tmp1197;*_tmp29E=((_tmp1197=_cycalloc(
sizeof(*_tmp1197)),((_tmp1197->v=_tmp2CA,_tmp1197))));};}}}goto _LL137;_LL13C: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp29F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp298;if(_tmp29F->tag != 5)goto _LL13E;else{_tmp2A0=_tmp29F->f1;}}_LL13D: Cyc_Tcutil_occurs(
evar,r,env,_tmp2A0.elt_typ);Cyc_Tcutil_occurs(evar,r,env,(_tmp2A0.ptr_atts).rgn);
goto _LL137;_LL13E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2A1=(struct
Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp298;if(_tmp2A1->tag != 8)goto _LL140;
else{_tmp2A2=_tmp2A1->f1;_tmp2A3=_tmp2A2.elt_type;}}_LL13F: Cyc_Tcutil_occurs(
evar,r,env,_tmp2A3);goto _LL137;_LL140: {struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp2A4=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp298;if(_tmp2A4->tag != 9)
goto _LL142;else{_tmp2A5=_tmp2A4->f1;_tmp2A6=_tmp2A5.tvars;_tmp2A7=_tmp2A5.effect;
_tmp2A8=_tmp2A5.ret_tqual;_tmp2A9=_tmp2A5.ret_typ;_tmp2AA=_tmp2A5.args;_tmp2AB=
_tmp2A5.c_varargs;_tmp2AC=_tmp2A5.cyc_varargs;_tmp2AD=_tmp2A5.rgn_po;_tmp2AE=
_tmp2A5.attributes;}}_LL141: env=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp2A6,env);
if(_tmp2A7 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp2A7);Cyc_Tcutil_occurs(
evar,r,env,_tmp2A9);for(0;_tmp2AA != 0;_tmp2AA=_tmp2AA->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple10*)_tmp2AA->hd)).f3);}if(_tmp2AC != 0)Cyc_Tcutil_occurs(
evar,r,env,_tmp2AC->type);for(0;_tmp2AD != 0;_tmp2AD=_tmp2AD->tl){struct _tuple0
_tmp2CE;void*_tmp2CF;void*_tmp2D0;struct _tuple0*_tmp2CD=(struct _tuple0*)_tmp2AD->hd;
_tmp2CE=*_tmp2CD;_tmp2CF=_tmp2CE.f1;_tmp2D0=_tmp2CE.f2;Cyc_Tcutil_occurs(evar,r,
env,_tmp2CF);Cyc_Tcutil_occurs(evar,r,env,_tmp2D0);}goto _LL137;_LL142: {struct
Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2AF=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)
_tmp298;if(_tmp2AF->tag != 10)goto _LL144;else{_tmp2B0=_tmp2AF->f1;}}_LL143: for(0;
_tmp2B0 != 0;_tmp2B0=_tmp2B0->tl){Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)
_tmp2B0->hd)).f2);}goto _LL137;_LL144: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*
_tmp2B1=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp298;if(_tmp2B1->tag
!= 3)goto _LL146;else{_tmp2B2=_tmp2B1->f1;_tmp2B3=_tmp2B2.targs;}}_LL145: Cyc_Tcutil_occurslist(
evar,r,env,_tmp2B3);goto _LL137;_LL146: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*
_tmp2B4=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp298;if(_tmp2B4->tag
!= 17)goto _LL148;else{_tmp2B5=_tmp2B4->f2;}}_LL147: _tmp2B8=_tmp2B5;goto _LL149;
_LL148: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp2B6=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)
_tmp298;if(_tmp2B6->tag != 4)goto _LL14A;else{_tmp2B7=_tmp2B6->f1;_tmp2B8=_tmp2B7.targs;}}
_LL149: _tmp2BB=_tmp2B8;goto _LL14B;_LL14A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp2B9=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp298;if(_tmp2B9->tag != 
11)goto _LL14C;else{_tmp2BA=_tmp2B9->f1;_tmp2BB=_tmp2BA.targs;}}_LL14B: Cyc_Tcutil_occurslist(
evar,r,env,_tmp2BB);goto _LL137;_LL14C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmp2BC=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp298;if(_tmp2BC->tag
!= 12)goto _LL14E;else{_tmp2BD=_tmp2BC->f2;}}_LL14D: for(0;_tmp2BD != 0;_tmp2BD=
_tmp2BD->tl){Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp2BD->hd)->type);}
goto _LL137;_LL14E: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2BE=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)
_tmp298;if(_tmp2BE->tag != 19)goto _LL150;else{_tmp2BF=(void*)_tmp2BE->f1;}}_LL14F:
_tmp2C1=_tmp2BF;goto _LL151;_LL150: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*
_tmp2C0=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp298;if(_tmp2C0->tag != 
23)goto _LL152;else{_tmp2C1=(void*)_tmp2C0->f1;}}_LL151: _tmp2C3=_tmp2C1;goto
_LL153;_LL152: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp2C2=(struct
Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp298;if(_tmp2C2->tag != 15)goto
_LL154;else{_tmp2C3=(void*)_tmp2C2->f1;}}_LL153: _tmp2C5=_tmp2C3;goto _LL155;
_LL154: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp2C4=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)
_tmp298;if(_tmp2C4->tag != 25)goto _LL156;else{_tmp2C5=(void*)_tmp2C4->f1;}}_LL155:
Cyc_Tcutil_occurs(evar,r,env,_tmp2C5);goto _LL137;_LL156: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmp2C6=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp298;if(_tmp2C6->tag != 24)
goto _LL158;else{_tmp2C7=_tmp2C6->f1;}}_LL157: Cyc_Tcutil_occurslist(evar,r,env,
_tmp2C7);goto _LL137;_LL158:;_LL159: goto _LL137;_LL137:;};}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}static void
Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(0;t1 != 
0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)(int)_throw((void*)& Cyc_Tcutil_Unify_val);}static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const
 && !tq1.real_const){const char*_tmp119A;void*_tmp1199;(_tmp1199=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp119A="tq1 real_const not set.",
_tag_dyneither(_tmp119A,sizeof(char),24))),_tag_dyneither(_tmp1199,sizeof(void*),
0)));}if(tq2.print_const  && !tq2.real_const){const char*_tmp119D;void*_tmp119C;(
_tmp119C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp119D="tq2 real_const not set.",_tag_dyneither(_tmp119D,sizeof(char),24))),
_tag_dyneither(_tmp119C,sizeof(void*),0)));}if((tq1.real_const != tq2.real_const
 || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=
tq2.real_const;{const char*_tmp119E;Cyc_Tcutil_failure_reason=((_tmp119E="(qualifiers don't match)",
_tag_dyneither(_tmp119E,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const
 && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static
void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return;{union Cyc_Absyn_Constraint*
_tmp2D6=x;union Cyc_Absyn_Constraint _tmp2D7;int _tmp2D8;union Cyc_Absyn_Constraint
_tmp2D9;void*_tmp2DA;union Cyc_Absyn_Constraint _tmp2DB;union Cyc_Absyn_Constraint*
_tmp2DC;_LL15B: _tmp2D7=*_tmp2D6;if((_tmp2D7.No_constr).tag != 3)goto _LL15D;
_tmp2D8=(int)(_tmp2D7.No_constr).val;_LL15C:{union Cyc_Absyn_Constraint _tmp119F;*
x=(((_tmp119F.Forward_constr).val=y,(((_tmp119F.Forward_constr).tag=2,_tmp119F))));}
return;_LL15D: _tmp2D9=*_tmp2D6;if((_tmp2D9.Eq_constr).tag != 1)goto _LL15F;_tmp2DA=(
void*)(_tmp2D9.Eq_constr).val;_LL15E: {union Cyc_Absyn_Constraint*_tmp2DE=y;union
Cyc_Absyn_Constraint _tmp2DF;int _tmp2E0;union Cyc_Absyn_Constraint _tmp2E1;void*
_tmp2E2;union Cyc_Absyn_Constraint _tmp2E3;union Cyc_Absyn_Constraint*_tmp2E4;
_LL162: _tmp2DF=*_tmp2DE;if((_tmp2DF.No_constr).tag != 3)goto _LL164;_tmp2E0=(int)(
_tmp2DF.No_constr).val;_LL163:*y=*x;return;_LL164: _tmp2E1=*_tmp2DE;if((_tmp2E1.Eq_constr).tag
!= 1)goto _LL166;_tmp2E2=(void*)(_tmp2E1.Eq_constr).val;_LL165: if(cmp(_tmp2DA,
_tmp2E2)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
return;_LL166: _tmp2E3=*_tmp2DE;if((_tmp2E3.Forward_constr).tag != 2)goto _LL161;
_tmp2E4=(union Cyc_Absyn_Constraint*)(_tmp2E3.Forward_constr).val;_LL167: {const
char*_tmp11A2;void*_tmp11A1;(_tmp11A1=0,Cyc_Tcutil_impos(((_tmp11A2="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp11A2,sizeof(char),40))),_tag_dyneither(_tmp11A1,sizeof(void*),
0)));}_LL161:;}_LL15F: _tmp2DB=*_tmp2D6;if((_tmp2DB.Forward_constr).tag != 2)goto
_LL15A;_tmp2DC=(union Cyc_Absyn_Constraint*)(_tmp2DB.Forward_constr).val;_LL160: {
const char*_tmp11A5;void*_tmp11A4;(_tmp11A4=0,Cyc_Tcutil_impos(((_tmp11A5="unify_conref: forward after compress",
_tag_dyneither(_tmp11A5,sizeof(char),37))),_tag_dyneither(_tmp11A4,sizeof(void*),
0)));}_LL15A:;};}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){struct _handler_cons _tmp2E9;
_push_handler(& _tmp2E9);{int _tmp2EB=0;if(setjmp(_tmp2E9.handler))_tmp2EB=1;if(!
_tmp2EB){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{int _tmp2EC=
1;_npop_handler(0);return _tmp2EC;};;_pop_handler();}else{void*_tmp2EA=(void*)
_exn_thrown;void*_tmp2EE=_tmp2EA;_LL169: {struct Cyc_Tcutil_Unify_exn_struct*
_tmp2EF=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp2EE;if(_tmp2EF->tag != Cyc_Tcutil_Unify)
goto _LL16B;}_LL16A: return 0;_LL16B:;_LL16C:(void)_throw(_tmp2EE);_LL168:;}};}
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){struct _tuple0 _tmp11A6;struct
_tuple0 _tmp2F1=(_tmp11A6.f1=b1,((_tmp11A6.f2=b2,_tmp11A6)));void*_tmp2F2;void*
_tmp2F4;void*_tmp2F6;void*_tmp2F8;void*_tmp2FA;struct Cyc_Absyn_Exp*_tmp2FC;void*
_tmp2FD;struct Cyc_Absyn_Exp*_tmp2FF;_LL16E: _tmp2F2=_tmp2F1.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp2F3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2F2;if(_tmp2F3->tag
!= 0)goto _LL170;};_tmp2F4=_tmp2F1.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp2F5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2F4;if(_tmp2F5->tag
!= 0)goto _LL170;};_LL16F: return 0;_LL170: _tmp2F6=_tmp2F1.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp2F7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2F6;if(_tmp2F7->tag
!= 0)goto _LL172;};_LL171: return - 1;_LL172: _tmp2F8=_tmp2F1.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp2F9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2F8;if(_tmp2F9->tag
!= 0)goto _LL174;};_LL173: return 1;_LL174: _tmp2FA=_tmp2F1.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp2FB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2FA;if(_tmp2FB->tag != 
1)goto _LL16D;else{_tmp2FC=_tmp2FB->f1;}};_tmp2FD=_tmp2F1.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp2FE=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2FD;if(_tmp2FE->tag != 
1)goto _LL16D;else{_tmp2FF=_tmp2FE->f1;}};_LL175: return Cyc_Evexp_const_exp_cmp(
_tmp2FC,_tmp2FF);_LL16D:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmp11A7;struct _tuple0 _tmp301=(_tmp11A7.f1=b1,((_tmp11A7.f2=b2,
_tmp11A7)));void*_tmp302;void*_tmp304;void*_tmp306;void*_tmp308;void*_tmp30A;
struct Cyc_Absyn_Exp*_tmp30C;void*_tmp30D;struct Cyc_Absyn_Exp*_tmp30F;_LL177:
_tmp302=_tmp301.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp303=(
struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp302;if(_tmp303->tag != 0)
goto _LL179;};_tmp304=_tmp301.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp305=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp304;if(_tmp305->tag
!= 0)goto _LL179;};_LL178: return 0;_LL179: _tmp306=_tmp301.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp307=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp306;if(_tmp307->tag
!= 0)goto _LL17B;};_LL17A: return - 1;_LL17B: _tmp308=_tmp301.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp309=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp308;if(_tmp309->tag
!= 0)goto _LL17D;};_LL17C: return 1;_LL17D: _tmp30A=_tmp301.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp30B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp30A;if(_tmp30B->tag != 
1)goto _LL176;else{_tmp30C=_tmp30B->f1;}};_tmp30D=_tmp301.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp30E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp30D;if(_tmp30E->tag != 
1)goto _LL176;else{_tmp30F=_tmp30E->f1;}};_LL17E: return Cyc_Evexp_const_exp_cmp(
_tmp30C,_tmp30F);_LL176:;}static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp310=att;_LL180: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*
_tmp311=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp310;if(_tmp311->tag
!= 0)goto _LL182;}_LL181: return 0;_LL182: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*
_tmp312=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp310;if(_tmp312->tag
!= 1)goto _LL184;}_LL183: return 1;_LL184: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*
_tmp313=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp310;if(_tmp313->tag
!= 2)goto _LL186;}_LL185: return 2;_LL186: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*
_tmp314=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp310;if(_tmp314->tag
!= 3)goto _LL188;}_LL187: return 3;_LL188: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*
_tmp315=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp310;if(_tmp315->tag
!= 4)goto _LL18A;}_LL189: return 4;_LL18A: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*
_tmp316=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp310;if(_tmp316->tag
!= 5)goto _LL18C;}_LL18B: return 5;_LL18C: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*
_tmp317=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp310;if(_tmp317->tag
!= 6)goto _LL18E;}_LL18D: return 6;_LL18E: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*
_tmp318=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp310;if(_tmp318->tag
!= 7)goto _LL190;}_LL18F: return 7;_LL190: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*
_tmp319=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp310;if(_tmp319->tag
!= 8)goto _LL192;}_LL191: return 8;_LL192: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*
_tmp31A=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp310;if(_tmp31A->tag
!= 9)goto _LL194;}_LL193: return 9;_LL194: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*
_tmp31B=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp310;if(_tmp31B->tag
!= 10)goto _LL196;}_LL195: return 10;_LL196: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*
_tmp31C=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp310;if(_tmp31C->tag
!= 11)goto _LL198;}_LL197: return 11;_LL198: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*
_tmp31D=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp310;if(_tmp31D->tag
!= 12)goto _LL19A;}_LL199: return 12;_LL19A: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*
_tmp31E=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp310;if(
_tmp31E->tag != 13)goto _LL19C;}_LL19B: return 13;_LL19C: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*
_tmp31F=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp310;if(
_tmp31F->tag != 14)goto _LL19E;}_LL19D: return 14;_LL19E: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*
_tmp320=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)
_tmp310;if(_tmp320->tag != 15)goto _LL1A0;}_LL19F: return 15;_LL1A0: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*
_tmp321=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp310;if(
_tmp321->tag != 16)goto _LL1A2;}_LL1A1: return 16;_LL1A2: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*
_tmp322=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp310;if(
_tmp322->tag != 17)goto _LL1A4;}_LL1A3: return 17;_LL1A4: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*
_tmp323=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)
_tmp310;if(_tmp323->tag != 18)goto _LL1A6;}_LL1A5: return 18;_LL1A6: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*
_tmp324=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp310;if(_tmp324->tag
!= 19)goto _LL1A8;}_LL1A7: return 19;_LL1A8: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*
_tmp325=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp310;if(
_tmp325->tag != 20)goto _LL1AA;}_LL1A9: return 20;_LL1AA:;_LL1AB: return 21;_LL17F:;}
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){struct _tuple0 _tmp11A8;
struct _tuple0 _tmp327=(_tmp11A8.f1=att1,((_tmp11A8.f2=att2,_tmp11A8)));void*
_tmp328;int _tmp32A;void*_tmp32B;int _tmp32D;void*_tmp32E;int _tmp330;void*_tmp331;
int _tmp333;void*_tmp334;int _tmp336;void*_tmp337;int _tmp339;void*_tmp33A;struct
_dyneither_ptr _tmp33C;void*_tmp33D;struct _dyneither_ptr _tmp33F;void*_tmp340;
enum Cyc_Absyn_Format_Type _tmp342;int _tmp343;int _tmp344;void*_tmp345;enum Cyc_Absyn_Format_Type
_tmp347;int _tmp348;int _tmp349;_LL1AD: _tmp328=_tmp327.f1;{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*
_tmp329=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp328;if(_tmp329->tag
!= 0)goto _LL1AF;else{_tmp32A=_tmp329->f1;}};_tmp32B=_tmp327.f2;{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*
_tmp32C=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp32B;if(_tmp32C->tag
!= 0)goto _LL1AF;else{_tmp32D=_tmp32C->f1;}};_LL1AE: _tmp330=_tmp32A;_tmp333=
_tmp32D;goto _LL1B0;_LL1AF: _tmp32E=_tmp327.f1;{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*
_tmp32F=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp32E;if(
_tmp32F->tag != 20)goto _LL1B1;else{_tmp330=_tmp32F->f1;}};_tmp331=_tmp327.f2;{
struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp332=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)
_tmp331;if(_tmp332->tag != 20)goto _LL1B1;else{_tmp333=_tmp332->f1;}};_LL1B0:
_tmp336=_tmp330;_tmp339=_tmp333;goto _LL1B2;_LL1B1: _tmp334=_tmp327.f1;{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*
_tmp335=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp334;if(_tmp335->tag
!= 6)goto _LL1B3;else{_tmp336=_tmp335->f1;}};_tmp337=_tmp327.f2;{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*
_tmp338=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp337;if(_tmp338->tag
!= 6)goto _LL1B3;else{_tmp339=_tmp338->f1;}};_LL1B2: return Cyc_Core_intcmp(_tmp336,
_tmp339);_LL1B3: _tmp33A=_tmp327.f1;{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*
_tmp33B=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp33A;if(_tmp33B->tag
!= 8)goto _LL1B5;else{_tmp33C=_tmp33B->f1;}};_tmp33D=_tmp327.f2;{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*
_tmp33E=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp33D;if(_tmp33E->tag
!= 8)goto _LL1B5;else{_tmp33F=_tmp33E->f1;}};_LL1B4: return Cyc_strcmp((struct
_dyneither_ptr)_tmp33C,(struct _dyneither_ptr)_tmp33F);_LL1B5: _tmp340=_tmp327.f1;{
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp341=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)
_tmp340;if(_tmp341->tag != 19)goto _LL1B7;else{_tmp342=_tmp341->f1;_tmp343=_tmp341->f2;
_tmp344=_tmp341->f3;}};_tmp345=_tmp327.f2;{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*
_tmp346=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp345;if(_tmp346->tag
!= 19)goto _LL1B7;else{_tmp347=_tmp346->f1;_tmp348=_tmp346->f2;_tmp349=_tmp346->f3;}};
_LL1B6: {int _tmp34A=Cyc_Core_intcmp((int)((unsigned int)_tmp342),(int)((
unsigned int)_tmp347));if(_tmp34A != 0)return _tmp34A;{int _tmp34B=Cyc_Core_intcmp(
_tmp343,_tmp348);if(_tmp34B != 0)return _tmp34B;return Cyc_Core_intcmp(_tmp344,
_tmp349);};}_LL1B7:;_LL1B8: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(
att1),Cyc_Tcutil_attribute_case_number(att2));_LL1AC:;}static int Cyc_Tcutil_equal_att(
void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a
!= 0;a=a->tl){if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}{struct Cyc_List_List*
a=a2;for(0;a != 0;a=a->tl){if(!((int(*)(int(*pred)(void*,void*),void*env,struct
Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return
0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static void*Cyc_Tcutil_rgns_of_field(
struct Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of(af->type);}static struct
_tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){void*t;{struct Cyc_Absyn_Kind*
_tmp34C=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind _tmp34D;
enum Cyc_Absyn_KindQual _tmp34E;enum Cyc_Absyn_AliasQual _tmp34F;struct Cyc_Absyn_Kind
_tmp350;enum Cyc_Absyn_KindQual _tmp351;enum Cyc_Absyn_AliasQual _tmp352;struct Cyc_Absyn_Kind
_tmp353;enum Cyc_Absyn_KindQual _tmp354;struct Cyc_Absyn_Kind _tmp355;enum Cyc_Absyn_KindQual
_tmp356;_LL1BA: _tmp34D=*_tmp34C;_tmp34E=_tmp34D.kind;if(_tmp34E != Cyc_Absyn_RgnKind)
goto _LL1BC;_tmp34F=_tmp34D.aliasqual;if(_tmp34F != Cyc_Absyn_Unique)goto _LL1BC;
_LL1BB: t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1B9;_LL1BC: _tmp350=*_tmp34C;
_tmp351=_tmp350.kind;if(_tmp351 != Cyc_Absyn_RgnKind)goto _LL1BE;_tmp352=_tmp350.aliasqual;
if(_tmp352 != Cyc_Absyn_Aliasable)goto _LL1BE;_LL1BD: t=(void*)& Cyc_Absyn_HeapRgn_val;
goto _LL1B9;_LL1BE: _tmp353=*_tmp34C;_tmp354=_tmp353.kind;if(_tmp354 != Cyc_Absyn_EffKind)
goto _LL1C0;_LL1BF: t=Cyc_Absyn_empty_effect;goto _LL1B9;_LL1C0: _tmp355=*_tmp34C;
_tmp356=_tmp355.kind;if(_tmp356 != Cyc_Absyn_IntKind)goto _LL1C2;_LL1C1:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct
_tmp11AB;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp11AA;t=(void*)((
_tmp11AA=_cycalloc(sizeof(*_tmp11AA)),((_tmp11AA[0]=((_tmp11AB.tag=18,((_tmp11AB.f1=
Cyc_Absyn_uint_exp(0,0),_tmp11AB)))),_tmp11AA))));}goto _LL1B9;_LL1C2:;_LL1C3: t=
Cyc_Absyn_sint_typ;goto _LL1B9;_LL1B9:;}{struct _tuple16*_tmp11AC;return(_tmp11AC=
_cycalloc(sizeof(*_tmp11AC)),((_tmp11AC->f1=tv,((_tmp11AC->f2=t,_tmp11AC)))));};}
static void*Cyc_Tcutil_rgns_of(void*t){void*_tmp35A=Cyc_Tcutil_compress(t);void*
_tmp364;void*_tmp366;void*_tmp367;struct Cyc_Absyn_DatatypeInfo _tmp369;struct Cyc_List_List*
_tmp36A;struct Cyc_Absyn_PtrInfo _tmp36C;void*_tmp36D;struct Cyc_Absyn_PtrAtts
_tmp36E;void*_tmp36F;struct Cyc_Absyn_ArrayInfo _tmp371;void*_tmp372;struct Cyc_List_List*
_tmp374;struct Cyc_Absyn_DatatypeFieldInfo _tmp376;struct Cyc_List_List*_tmp377;
struct Cyc_Absyn_AggrInfo _tmp379;struct Cyc_List_List*_tmp37A;struct Cyc_List_List*
_tmp37C;struct Cyc_Absyn_FnInfo _tmp37F;struct Cyc_List_List*_tmp380;void*_tmp381;
struct Cyc_Absyn_Tqual _tmp382;void*_tmp383;struct Cyc_List_List*_tmp384;struct Cyc_Absyn_VarargInfo*
_tmp385;struct Cyc_List_List*_tmp386;void*_tmp38D;struct Cyc_List_List*_tmp38F;
_LL1C5: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp35B=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)
_tmp35A;if(_tmp35B->tag != 0)goto _LL1C7;}_LL1C6: goto _LL1C8;_LL1C7: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp35C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp35A;if(_tmp35C->tag != 
7)goto _LL1C9;}_LL1C8: goto _LL1CA;_LL1C9: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp35D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp35A;if(_tmp35D->tag != 
13)goto _LL1CB;}_LL1CA: goto _LL1CC;_LL1CB: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*
_tmp35E=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp35A;if(_tmp35E->tag
!= 14)goto _LL1CD;}_LL1CC: goto _LL1CE;_LL1CD: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*
_tmp35F=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp35A;if(_tmp35F->tag
!= 18)goto _LL1CF;}_LL1CE: goto _LL1D0;_LL1CF: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp360=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp35A;if(_tmp360->tag != 6)
goto _LL1D1;}_LL1D0: return Cyc_Absyn_empty_effect;_LL1D1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp361=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp35A;if(_tmp361->tag != 1)
goto _LL1D3;}_LL1D2: goto _LL1D4;_LL1D3: {struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp362=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp35A;if(_tmp362->tag != 2)
goto _LL1D5;}_LL1D4: {struct Cyc_Absyn_Kind*_tmp391=Cyc_Tcutil_typ_kind(t);struct
Cyc_Absyn_Kind _tmp392;enum Cyc_Absyn_KindQual _tmp393;struct Cyc_Absyn_Kind _tmp394;
enum Cyc_Absyn_KindQual _tmp395;struct Cyc_Absyn_Kind _tmp396;enum Cyc_Absyn_KindQual
_tmp397;_LL1FC: _tmp392=*_tmp391;_tmp393=_tmp392.kind;if(_tmp393 != Cyc_Absyn_RgnKind)
goto _LL1FE;_LL1FD: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11AF;struct
Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11AE;return(void*)((_tmp11AE=_cycalloc(
sizeof(*_tmp11AE)),((_tmp11AE[0]=((_tmp11AF.tag=23,((_tmp11AF.f1=(void*)t,
_tmp11AF)))),_tmp11AE))));}_LL1FE: _tmp394=*_tmp391;_tmp395=_tmp394.kind;if(
_tmp395 != Cyc_Absyn_EffKind)goto _LL200;_LL1FF: return t;_LL200: _tmp396=*_tmp391;
_tmp397=_tmp396.kind;if(_tmp397 != Cyc_Absyn_IntKind)goto _LL202;_LL201: return Cyc_Absyn_empty_effect;
_LL202:;_LL203: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp11B2;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*
_tmp11B1;return(void*)((_tmp11B1=_cycalloc(sizeof(*_tmp11B1)),((_tmp11B1[0]=((
_tmp11B2.tag=25,((_tmp11B2.f1=(void*)t,_tmp11B2)))),_tmp11B1))));}_LL1FB:;}
_LL1D5: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp363=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)
_tmp35A;if(_tmp363->tag != 15)goto _LL1D7;else{_tmp364=(void*)_tmp363->f1;}}_LL1D6: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11B5;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*
_tmp11B4;return(void*)((_tmp11B4=_cycalloc(sizeof(*_tmp11B4)),((_tmp11B4[0]=((
_tmp11B5.tag=23,((_tmp11B5.f1=(void*)_tmp364,_tmp11B5)))),_tmp11B4))));}_LL1D7: {
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp365=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)
_tmp35A;if(_tmp365->tag != 16)goto _LL1D9;else{_tmp366=(void*)_tmp365->f1;_tmp367=(
void*)_tmp365->f2;}}_LL1D8: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11B8;
struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11B7;return(void*)((_tmp11B7=
_cycalloc(sizeof(*_tmp11B7)),((_tmp11B7[0]=((_tmp11B8.tag=23,((_tmp11B8.f1=(void*)
_tmp367,_tmp11B8)))),_tmp11B7))));}_LL1D9: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*
_tmp368=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp35A;if(_tmp368->tag
!= 3)goto _LL1DB;else{_tmp369=_tmp368->f1;_tmp36A=_tmp369.targs;}}_LL1DA: {struct
Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp36A);struct Cyc_Absyn_JoinEff_Absyn_Type_struct
_tmp11BB;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11BA;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11BA=_cycalloc(sizeof(*_tmp11BA)),((_tmp11BA[0]=((_tmp11BB.tag=24,((
_tmp11BB.f1=ts,_tmp11BB)))),_tmp11BA)))));}_LL1DB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp36B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35A;if(_tmp36B->tag
!= 5)goto _LL1DD;else{_tmp36C=_tmp36B->f1;_tmp36D=_tmp36C.elt_typ;_tmp36E=_tmp36C.ptr_atts;
_tmp36F=_tmp36E.rgn;}}_LL1DC: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11CA;
struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11C9;struct Cyc_Absyn_AccessEff_Absyn_Type_struct
_tmp11C8;void*_tmp11C7[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11C6;
return Cyc_Tcutil_normalize_effect((void*)((_tmp11C6=_cycalloc(sizeof(*_tmp11C6)),((
_tmp11C6[0]=((_tmp11CA.tag=24,((_tmp11CA.f1=((_tmp11C7[1]=Cyc_Tcutil_rgns_of(
_tmp36D),((_tmp11C7[0]=(void*)((_tmp11C9=_cycalloc(sizeof(*_tmp11C9)),((_tmp11C9[
0]=((_tmp11C8.tag=23,((_tmp11C8.f1=(void*)_tmp36F,_tmp11C8)))),_tmp11C9)))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp11C7,sizeof(void*),2)))))),_tmp11CA)))),_tmp11C6)))));}_LL1DD: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp370=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp35A;if(_tmp370->tag != 
8)goto _LL1DF;else{_tmp371=_tmp370->f1;_tmp372=_tmp371.elt_type;}}_LL1DE: return
Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp372));_LL1DF: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmp373=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp35A;if(_tmp373->tag != 
10)goto _LL1E1;else{_tmp374=_tmp373->f1;}}_LL1E0: {struct Cyc_List_List*_tmp3A7=0;
for(0;_tmp374 != 0;_tmp374=_tmp374->tl){struct Cyc_List_List*_tmp11CB;_tmp3A7=((
_tmp11CB=_cycalloc(sizeof(*_tmp11CB)),((_tmp11CB->hd=(*((struct _tuple12*)_tmp374->hd)).f2,((
_tmp11CB->tl=_tmp3A7,_tmp11CB))))));}_tmp377=_tmp3A7;goto _LL1E2;}_LL1E1: {struct
Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp375=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)
_tmp35A;if(_tmp375->tag != 4)goto _LL1E3;else{_tmp376=_tmp375->f1;_tmp377=_tmp376.targs;}}
_LL1E2: _tmp37A=_tmp377;goto _LL1E4;_LL1E3: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp378=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp35A;if(_tmp378->tag != 
11)goto _LL1E5;else{_tmp379=_tmp378->f1;_tmp37A=_tmp379.targs;}}_LL1E4: {struct
Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11CE;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmp11CD;return Cyc_Tcutil_normalize_effect((void*)((_tmp11CD=_cycalloc(sizeof(*
_tmp11CD)),((_tmp11CD[0]=((_tmp11CE.tag=24,((_tmp11CE.f1=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp37A),
_tmp11CE)))),_tmp11CD)))));}_LL1E5: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmp37B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp35A;if(_tmp37B->tag
!= 12)goto _LL1E7;else{_tmp37C=_tmp37B->f2;}}_LL1E6: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct
_tmp11D1;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11D0;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11D0=_cycalloc(sizeof(*_tmp11D0)),((_tmp11D0[0]=((_tmp11D1.tag=24,((
_tmp11D1.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp37C),_tmp11D1)))),
_tmp11D0)))));}_LL1E7: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp37D=(
struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp35A;if(_tmp37D->tag != 19)goto
_LL1E9;}_LL1E8: return Cyc_Absyn_empty_effect;_LL1E9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp37E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp35A;if(_tmp37E->tag != 9)
goto _LL1EB;else{_tmp37F=_tmp37E->f1;_tmp380=_tmp37F.tvars;_tmp381=_tmp37F.effect;
_tmp382=_tmp37F.ret_tqual;_tmp383=_tmp37F.ret_typ;_tmp384=_tmp37F.args;_tmp385=
_tmp37F.cyc_varargs;_tmp386=_tmp37F.rgn_po;}}_LL1EA: {void*_tmp3AD=Cyc_Tcutil_substitute(((
struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp380),(void*)_check_null(
_tmp381));return Cyc_Tcutil_normalize_effect(_tmp3AD);}_LL1EB: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*
_tmp387=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp35A;if(_tmp387->tag != 
21)goto _LL1ED;}_LL1EC: goto _LL1EE;_LL1ED: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*
_tmp388=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp35A;if(_tmp388->tag != 
22)goto _LL1EF;}_LL1EE: goto _LL1F0;_LL1EF: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*
_tmp389=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp35A;if(_tmp389->tag != 20)
goto _LL1F1;}_LL1F0: return Cyc_Absyn_empty_effect;_LL1F1: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*
_tmp38A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp35A;if(_tmp38A->tag != 
23)goto _LL1F3;}_LL1F2: goto _LL1F4;_LL1F3: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmp38B=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp35A;if(_tmp38B->tag != 24)
goto _LL1F5;}_LL1F4: return t;_LL1F5: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*
_tmp38C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp35A;if(_tmp38C->tag != 25)
goto _LL1F7;else{_tmp38D=(void*)_tmp38C->f1;}}_LL1F6: return Cyc_Tcutil_rgns_of(
_tmp38D);_LL1F7: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp38E=(struct
Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp35A;if(_tmp38E->tag != 17)goto _LL1F9;
else{_tmp38F=_tmp38E->f2;}}_LL1F8: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct
_tmp11D4;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11D3;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11D3=_cycalloc(sizeof(*_tmp11D3)),((_tmp11D3[0]=((_tmp11D4.tag=24,((
_tmp11D4.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp38F),_tmp11D4)))),_tmp11D3)))));}_LL1F9: {
struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp390=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)
_tmp35A;if(_tmp390->tag != 26)goto _LL1C4;}_LL1FA: {const char*_tmp11D7;void*
_tmp11D6;(_tmp11D6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp11D7="typedecl in rgns_of",_tag_dyneither(_tmp11D7,
sizeof(char),20))),_tag_dyneither(_tmp11D6,sizeof(void*),0)));}_LL1C4:;}void*Cyc_Tcutil_normalize_effect(
void*e){e=Cyc_Tcutil_compress(e);{void*_tmp3B2=e;struct Cyc_List_List*_tmp3B4;
struct Cyc_List_List**_tmp3B5;void*_tmp3B7;_LL205: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmp3B3=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3B2;if(_tmp3B3->tag != 24)
goto _LL207;else{_tmp3B4=_tmp3B3->f1;_tmp3B5=(struct Cyc_List_List**)& _tmp3B3->f1;}}
_LL206: {int redo_join=0;{struct Cyc_List_List*effs=*_tmp3B5;for(0;effs != 0;effs=
effs->tl){void*_tmp3B8=(void*)effs->hd;effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(
_tmp3B8));{void*_tmp3B9=(void*)effs->hd;void*_tmp3BC;void*_tmp3BF;void*_tmp3C2;
_LL20C: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3BA=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)
_tmp3B9;if(_tmp3BA->tag != 24)goto _LL20E;}_LL20D: goto _LL20F;_LL20E: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*
_tmp3BB=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3B9;if(_tmp3BB->tag != 
23)goto _LL210;else{_tmp3BC=(void*)_tmp3BB->f1;{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*
_tmp3BD=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3BC;if(_tmp3BD->tag != 20)
goto _LL210;};}}_LL20F: redo_join=1;goto _LL20B;_LL210: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*
_tmp3BE=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3B9;if(_tmp3BE->tag != 
23)goto _LL212;else{_tmp3BF=(void*)_tmp3BE->f1;{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*
_tmp3C0=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3BF;if(_tmp3C0->tag != 
22)goto _LL212;};}}_LL211: redo_join=1;goto _LL20B;_LL212: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*
_tmp3C1=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3B9;if(_tmp3C1->tag != 
23)goto _LL214;else{_tmp3C2=(void*)_tmp3C1->f1;{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*
_tmp3C3=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3C2;if(_tmp3C3->tag != 
21)goto _LL214;};}}_LL213: redo_join=1;goto _LL20B;_LL214:;_LL215: goto _LL20B;_LL20B:;};}}
if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct Cyc_List_List*effs=*
_tmp3B5;for(0;effs != 0;effs=effs->tl){void*_tmp3C4=Cyc_Tcutil_compress((void*)
effs->hd);struct Cyc_List_List*_tmp3C6;void*_tmp3C8;void*_tmp3CB;void*_tmp3CE;
_LL217: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3C5=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)
_tmp3C4;if(_tmp3C5->tag != 24)goto _LL219;else{_tmp3C6=_tmp3C5->f1;}}_LL218:
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_revappend)(_tmp3C6,effects);goto _LL216;_LL219: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*
_tmp3C7=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3C4;if(_tmp3C7->tag != 
23)goto _LL21B;else{_tmp3C8=(void*)_tmp3C7->f1;{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*
_tmp3C9=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3C8;if(_tmp3C9->tag != 20)
goto _LL21B;};}}_LL21A: goto _LL216;_LL21B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*
_tmp3CA=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3C4;if(_tmp3CA->tag != 
23)goto _LL21D;else{_tmp3CB=(void*)_tmp3CA->f1;{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*
_tmp3CC=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3CB;if(_tmp3CC->tag != 
22)goto _LL21D;};}}_LL21C: goto _LL216;_LL21D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*
_tmp3CD=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3C4;if(_tmp3CD->tag != 
23)goto _LL21F;else{_tmp3CE=(void*)_tmp3CD->f1;{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*
_tmp3CF=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3CE;if(_tmp3CF->tag != 
21)goto _LL21F;};}}_LL21E: goto _LL216;_LL21F:;_LL220:{struct Cyc_List_List*_tmp11D8;
effects=((_tmp11D8=_cycalloc(sizeof(*_tmp11D8)),((_tmp11D8->hd=_tmp3C4,((
_tmp11D8->tl=effects,_tmp11D8))))));}goto _LL216;_LL216:;}}*_tmp3B5=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);return e;};}_LL207: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*
_tmp3B6=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B2;if(_tmp3B6->tag != 25)
goto _LL209;else{_tmp3B7=(void*)_tmp3B6->f1;}}_LL208: {void*_tmp3D1=Cyc_Tcutil_compress(
_tmp3B7);_LL222: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3D2=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)
_tmp3D1;if(_tmp3D2->tag != 1)goto _LL224;}_LL223: goto _LL225;_LL224: {struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp3D3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3D1;if(_tmp3D3->tag != 2)
goto _LL226;}_LL225: return e;_LL226:;_LL227: return Cyc_Tcutil_rgns_of(_tmp3B7);
_LL221:;}_LL209:;_LL20A: return e;_LL204:;};}static void*Cyc_Tcutil_dummy_fntype(
void*eff){struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp11DE;struct Cyc_Absyn_FnInfo
_tmp11DD;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp11DC;struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp3D4=(_tmp11DC=_cycalloc(sizeof(*_tmp11DC)),((_tmp11DC[0]=((_tmp11DE.tag=9,((
_tmp11DE.f1=((_tmp11DD.tvars=0,((_tmp11DD.effect=(void*)eff,((_tmp11DD.ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmp11DD.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((
_tmp11DD.args=0,((_tmp11DD.c_varargs=0,((_tmp11DD.cyc_varargs=0,((_tmp11DD.rgn_po=
0,((_tmp11DD.attributes=0,_tmp11DD)))))))))))))))))),_tmp11DE)))),_tmp11DC)));
return Cyc_Absyn_atb_typ((void*)_tmp3D4,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e){r=Cyc_Tcutil_compress(r);if((r == (void*)& Cyc_Absyn_HeapRgn_val
 || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{void*_tmp3D8=Cyc_Tcutil_compress(e);void*_tmp3DA;struct Cyc_List_List*
_tmp3DC;void*_tmp3DE;struct Cyc_Core_Opt*_tmp3E0;void*_tmp3E1;void**_tmp3E2;
struct Cyc_Core_Opt*_tmp3E3;_LL229: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*
_tmp3D9=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3D8;if(_tmp3D9->tag != 
23)goto _LL22B;else{_tmp3DA=(void*)_tmp3D9->f1;}}_LL22A: if(constrain)return Cyc_Tcutil_unify(
r,_tmp3DA);_tmp3DA=Cyc_Tcutil_compress(_tmp3DA);if(r == _tmp3DA)return 1;{struct
_tuple0 _tmp11DF;struct _tuple0 _tmp3E5=(_tmp11DF.f1=r,((_tmp11DF.f2=_tmp3DA,
_tmp11DF)));void*_tmp3E6;struct Cyc_Absyn_Tvar*_tmp3E8;void*_tmp3E9;struct Cyc_Absyn_Tvar*
_tmp3EB;_LL234: _tmp3E6=_tmp3E5.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp3E7=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3E6;if(_tmp3E7->tag != 2)
goto _LL236;else{_tmp3E8=_tmp3E7->f1;}};_tmp3E9=_tmp3E5.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp3EA=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3E9;if(_tmp3EA->tag != 2)
goto _LL236;else{_tmp3EB=_tmp3EA->f1;}};_LL235: return Cyc_Absyn_tvar_cmp(_tmp3E8,
_tmp3EB)== 0;_LL236:;_LL237: return 0;_LL233:;};_LL22B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmp3DB=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3D8;if(_tmp3DB->tag != 24)
goto _LL22D;else{_tmp3DC=_tmp3DB->f1;}}_LL22C: for(0;_tmp3DC != 0;_tmp3DC=_tmp3DC->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp3DC->hd))return 1;}return 0;
_LL22D: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3DD=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)
_tmp3D8;if(_tmp3DD->tag != 25)goto _LL22F;else{_tmp3DE=(void*)_tmp3DD->f1;}}_LL22E: {
void*_tmp3EC=Cyc_Tcutil_rgns_of(_tmp3DE);void*_tmp3EE;_LL239: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*
_tmp3ED=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3EC;if(_tmp3ED->tag != 25)
goto _LL23B;else{_tmp3EE=(void*)_tmp3ED->f1;}}_LL23A: if(!constrain)return 0;{void*
_tmp3EF=Cyc_Tcutil_compress(_tmp3EE);struct Cyc_Core_Opt*_tmp3F1;void*_tmp3F2;
void**_tmp3F3;struct Cyc_Core_Opt*_tmp3F4;_LL23E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp3F0=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3EF;if(_tmp3F0->tag != 1)
goto _LL240;else{_tmp3F1=_tmp3F0->f1;_tmp3F2=(void**)& _tmp3F0->f2;_tmp3F3=(void**)((
void**)& _tmp3F0->f2);_tmp3F4=_tmp3F0->f4;}}_LL23F: {void*_tmp3F5=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3F4);Cyc_Tcutil_occurs(_tmp3F5,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3F4))->v,r);{struct
Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11EE;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*
_tmp11ED;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11EC;void*_tmp11EB[2];
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11EA;void*_tmp3F6=Cyc_Tcutil_dummy_fntype((
void*)((_tmp11EA=_cycalloc(sizeof(*_tmp11EA)),((_tmp11EA[0]=((_tmp11EE.tag=24,((
_tmp11EE.f1=((_tmp11EB[1]=(void*)((_tmp11ED=_cycalloc(sizeof(*_tmp11ED)),((
_tmp11ED[0]=((_tmp11EC.tag=23,((_tmp11EC.f1=(void*)r,_tmp11EC)))),_tmp11ED)))),((
_tmp11EB[0]=_tmp3F5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp11EB,sizeof(void*),2)))))),_tmp11EE)))),_tmp11EA)))));*
_tmp3F3=(void*)_tmp3F6;return 1;};}_LL240:;_LL241: return 0;_LL23D:;};_LL23B:;
_LL23C: return Cyc_Tcutil_region_in_effect(constrain,r,_tmp3EC);_LL238:;}_LL22F: {
struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3DF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)
_tmp3D8;if(_tmp3DF->tag != 1)goto _LL231;else{_tmp3E0=_tmp3DF->f1;_tmp3E1=(void**)&
_tmp3DF->f2;_tmp3E2=(void**)((void**)& _tmp3DF->f2);_tmp3E3=_tmp3DF->f4;}}_LL230:
if(_tmp3E0 == 0  || ((struct Cyc_Absyn_Kind*)_tmp3E0->v)->kind != Cyc_Absyn_EffKind){
const char*_tmp11F1;void*_tmp11F0;(_tmp11F0=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11F1="effect evar has wrong kind",
_tag_dyneither(_tmp11F1,sizeof(char),27))),_tag_dyneither(_tmp11F0,sizeof(void*),
0)));}if(!constrain)return 0;{void*_tmp3FE=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp3E3);Cyc_Tcutil_occurs(_tmp3FE,Cyc_Core_heap_region,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3E3))->v,r);{struct Cyc_Absyn_JoinEff_Absyn_Type_struct
_tmp1206;struct Cyc_List_List*_tmp1205;struct Cyc_Absyn_AccessEff_Absyn_Type_struct
_tmp1204;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1203;struct Cyc_List_List*
_tmp1202;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1201;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmp3FF=(_tmp1201=_cycalloc(sizeof(*_tmp1201)),((_tmp1201[0]=((_tmp1206.tag=24,((
_tmp1206.f1=((_tmp1202=_cycalloc(sizeof(*_tmp1202)),((_tmp1202->hd=_tmp3FE,((
_tmp1202->tl=((_tmp1205=_cycalloc(sizeof(*_tmp1205)),((_tmp1205->hd=(void*)((
_tmp1203=_cycalloc(sizeof(*_tmp1203)),((_tmp1203[0]=((_tmp1204.tag=23,((_tmp1204.f1=(
void*)r,_tmp1204)))),_tmp1203)))),((_tmp1205->tl=0,_tmp1205)))))),_tmp1202)))))),
_tmp1206)))),_tmp1201)));*_tmp3E2=(void*)_tmp3FF;return 1;};};_LL231:;_LL232:
return 0;_LL228:;};}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,
void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp406=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp409;void*_tmp40B;struct Cyc_Core_Opt*
_tmp40D;void*_tmp40E;void**_tmp40F;struct Cyc_Core_Opt*_tmp410;_LL243: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*
_tmp407=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp406;if(_tmp407->tag != 
23)goto _LL245;}_LL244: return 0;_LL245: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmp408=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp406;if(_tmp408->tag != 24)
goto _LL247;else{_tmp409=_tmp408->f1;}}_LL246: for(0;_tmp409 != 0;_tmp409=_tmp409->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp409->hd))return 1;}
return 0;_LL247: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp40A=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)
_tmp406;if(_tmp40A->tag != 25)goto _LL249;else{_tmp40B=(void*)_tmp40A->f1;}}_LL248:
_tmp40B=Cyc_Tcutil_compress(_tmp40B);if(t == _tmp40B)return 1;if(
may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp40B);{void*_tmp411=Cyc_Tcutil_rgns_of(
t);void*_tmp413;_LL24E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp412=(
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp411;if(_tmp412->tag != 25)goto
_LL250;else{_tmp413=(void*)_tmp412->f1;}}_LL24F: {struct _tuple0 _tmp1207;struct
_tuple0 _tmp415=(_tmp1207.f1=Cyc_Tcutil_compress(_tmp413),((_tmp1207.f2=_tmp40B,
_tmp1207)));void*_tmp416;struct Cyc_Absyn_Tvar*_tmp418;void*_tmp419;struct Cyc_Absyn_Tvar*
_tmp41B;_LL253: _tmp416=_tmp415.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp417=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp416;if(_tmp417->tag != 2)
goto _LL255;else{_tmp418=_tmp417->f1;}};_tmp419=_tmp415.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp41A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp419;if(_tmp41A->tag != 2)
goto _LL255;else{_tmp41B=_tmp41A->f1;}};_LL254: return Cyc_Tcutil_unify(t,_tmp40B);
_LL255:;_LL256: return _tmp413 == _tmp40B;_LL252:;}_LL250:;_LL251: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,_tmp411);_LL24D:;};_LL249: {struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp40C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp406;if(_tmp40C->tag != 1)
goto _LL24B;else{_tmp40D=_tmp40C->f1;_tmp40E=(void**)& _tmp40C->f2;_tmp40F=(void**)((
void**)& _tmp40C->f2);_tmp410=_tmp40C->f4;}}_LL24A: if(_tmp40D == 0  || ((struct Cyc_Absyn_Kind*)
_tmp40D->v)->kind != Cyc_Absyn_EffKind){const char*_tmp120A;void*_tmp1209;(
_tmp1209=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp120A="effect evar has wrong kind",_tag_dyneither(_tmp120A,sizeof(char),27))),
_tag_dyneither(_tmp1209,sizeof(void*),0)));}if(!may_constrain_evars)return 0;{
void*_tmp41E=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp410);
Cyc_Tcutil_occurs(_tmp41E,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp410))->v,t);{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp121F;
struct Cyc_List_List*_tmp121E;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp121D;
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp121C;struct Cyc_List_List*_tmp121B;
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp121A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmp41F=(_tmp121A=_cycalloc(sizeof(*_tmp121A)),((_tmp121A[0]=((_tmp121F.tag=24,((
_tmp121F.f1=((_tmp121B=_cycalloc(sizeof(*_tmp121B)),((_tmp121B->hd=_tmp41E,((
_tmp121B->tl=((_tmp121E=_cycalloc(sizeof(*_tmp121E)),((_tmp121E->hd=(void*)((
_tmp121C=_cycalloc(sizeof(*_tmp121C)),((_tmp121C[0]=((_tmp121D.tag=25,((_tmp121D.f1=(
void*)t,_tmp121D)))),_tmp121C)))),((_tmp121E->tl=0,_tmp121E)))))),_tmp121B)))))),
_tmp121F)))),_tmp121A)));*_tmp40F=(void*)_tmp41F;return 1;};};_LL24B:;_LL24C:
return 0;_LL242:;};}static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,
struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp426=e;struct
Cyc_Absyn_Tvar*_tmp428;struct Cyc_List_List*_tmp42A;void*_tmp42C;struct Cyc_Core_Opt*
_tmp42E;void*_tmp42F;void**_tmp430;struct Cyc_Core_Opt*_tmp431;_LL258: {struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp427=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp426;if(_tmp427->tag != 2)
goto _LL25A;else{_tmp428=_tmp427->f1;}}_LL259: return Cyc_Absyn_tvar_cmp(v,_tmp428)
== 0;_LL25A: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp429=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)
_tmp426;if(_tmp429->tag != 24)goto _LL25C;else{_tmp42A=_tmp429->f1;}}_LL25B: for(0;
_tmp42A != 0;_tmp42A=_tmp42A->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp42A->hd))return 1;}return 0;_LL25C: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*
_tmp42B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp426;if(_tmp42B->tag != 25)
goto _LL25E;else{_tmp42C=(void*)_tmp42B->f1;}}_LL25D: {void*_tmp432=Cyc_Tcutil_rgns_of(
_tmp42C);void*_tmp434;_LL263: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp433=(
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp432;if(_tmp433->tag != 25)goto
_LL265;else{_tmp434=(void*)_tmp433->f1;}}_LL264: if(!may_constrain_evars)return 0;{
void*_tmp435=Cyc_Tcutil_compress(_tmp434);struct Cyc_Core_Opt*_tmp437;void*
_tmp438;void**_tmp439;struct Cyc_Core_Opt*_tmp43A;_LL268: {struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp436=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp435;if(_tmp436->tag != 1)
goto _LL26A;else{_tmp437=_tmp436->f1;_tmp438=(void**)& _tmp436->f2;_tmp439=(void**)((
void**)& _tmp436->f2);_tmp43A=_tmp436->f4;}}_LL269: {void*_tmp43B=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp43A);if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp43A))->v,v))return 0;{struct Cyc_Absyn_JoinEff_Absyn_Type_struct
_tmp122E;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp122D;struct Cyc_Absyn_VarType_Absyn_Type_struct
_tmp122C;void*_tmp122B[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp122A;
void*_tmp43C=Cyc_Tcutil_dummy_fntype((void*)((_tmp122A=_cycalloc(sizeof(*
_tmp122A)),((_tmp122A[0]=((_tmp122E.tag=24,((_tmp122E.f1=((_tmp122B[1]=(void*)((
_tmp122D=_cycalloc(sizeof(*_tmp122D)),((_tmp122D[0]=((_tmp122C.tag=2,((_tmp122C.f1=
v,_tmp122C)))),_tmp122D)))),((_tmp122B[0]=_tmp43B,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp122B,sizeof(void*),2)))))),
_tmp122E)))),_tmp122A)))));*_tmp439=(void*)_tmp43C;return 1;};}_LL26A:;_LL26B:
return 0;_LL267:;};_LL265:;_LL266: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp432);_LL262:;}_LL25E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp42D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp426;if(_tmp42D->tag != 1)
goto _LL260;else{_tmp42E=_tmp42D->f1;_tmp42F=(void**)& _tmp42D->f2;_tmp430=(void**)((
void**)& _tmp42D->f2);_tmp431=_tmp42D->f4;}}_LL25F: if(_tmp42E == 0  || ((struct Cyc_Absyn_Kind*)
_tmp42E->v)->kind != Cyc_Absyn_EffKind){const char*_tmp1231;void*_tmp1230;(
_tmp1230=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1231="effect evar has wrong kind",_tag_dyneither(_tmp1231,sizeof(char),27))),
_tag_dyneither(_tmp1230,sizeof(void*),0)));}{void*_tmp444=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp431);if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp431))->v,v))return 0;{struct Cyc_Absyn_JoinEff_Absyn_Type_struct
_tmp1246;struct Cyc_List_List*_tmp1245;struct Cyc_Absyn_VarType_Absyn_Type_struct
_tmp1244;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1243;struct Cyc_List_List*
_tmp1242;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1241;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmp445=(_tmp1241=_cycalloc(sizeof(*_tmp1241)),((_tmp1241[0]=((_tmp1246.tag=24,((
_tmp1246.f1=((_tmp1242=_cycalloc(sizeof(*_tmp1242)),((_tmp1242->hd=_tmp444,((
_tmp1242->tl=((_tmp1245=_cycalloc(sizeof(*_tmp1245)),((_tmp1245->hd=(void*)((
_tmp1243=_cycalloc(sizeof(*_tmp1243)),((_tmp1243[0]=((_tmp1244.tag=2,((_tmp1244.f1=
v,_tmp1244)))),_tmp1243)))),((_tmp1245->tl=0,_tmp1245)))))),_tmp1242)))))),
_tmp1246)))),_tmp1241)));*_tmp430=(void*)_tmp445;return 1;};};_LL260:;_LL261:
return 0;_LL257:;};}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp44C=e;struct Cyc_List_List*_tmp44E;void*_tmp450;_LL26D: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmp44D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp44C;if(_tmp44D->tag != 24)
goto _LL26F;else{_tmp44E=_tmp44D->f1;}}_LL26E: for(0;_tmp44E != 0;_tmp44E=_tmp44E->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp44E->hd))return 1;}return 0;_LL26F: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp44F=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)
_tmp44C;if(_tmp44F->tag != 25)goto _LL271;else{_tmp450=(void*)_tmp44F->f1;}}_LL270: {
void*_tmp452=Cyc_Tcutil_rgns_of(_tmp450);void*_tmp454;_LL276: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*
_tmp453=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp452;if(_tmp453->tag != 25)
goto _LL278;else{_tmp454=(void*)_tmp453->f1;}}_LL277: return 0;_LL278:;_LL279:
return Cyc_Tcutil_evar_in_effect(evar,_tmp452);_LL275:;}_LL271: {struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp451=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp44C;if(_tmp451->tag != 1)
goto _LL273;}_LL272: return evar == e;_LL273:;_LL274: return 0;_LL26C:;};}int Cyc_Tcutil_subset_effect(
int may_constrain_evars,void*e1,void*e2){void*_tmp455=Cyc_Tcutil_compress(e1);
struct Cyc_List_List*_tmp457;void*_tmp459;struct Cyc_Absyn_Tvar*_tmp45B;void*
_tmp45D;void*_tmp45F;void**_tmp460;struct Cyc_Core_Opt*_tmp461;_LL27B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmp456=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp455;if(_tmp456->tag != 24)
goto _LL27D;else{_tmp457=_tmp456->f1;}}_LL27C: for(0;_tmp457 != 0;_tmp457=_tmp457->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp457->hd,e2))return 0;}
return 1;_LL27D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp458=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)
_tmp455;if(_tmp458->tag != 23)goto _LL27F;else{_tmp459=(void*)_tmp458->f1;}}_LL27E:
return Cyc_Tcutil_region_in_effect(0,_tmp459,e2) || may_constrain_evars  && Cyc_Tcutil_unify(
_tmp459,(void*)& Cyc_Absyn_HeapRgn_val);_LL27F: {struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp45A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp455;if(_tmp45A->tag != 2)
goto _LL281;else{_tmp45B=_tmp45A->f1;}}_LL280: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,_tmp45B,e2);_LL281: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*
_tmp45C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp455;if(_tmp45C->tag != 25)
goto _LL283;else{_tmp45D=(void*)_tmp45C->f1;}}_LL282: {void*_tmp462=Cyc_Tcutil_rgns_of(
_tmp45D);void*_tmp464;_LL288: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp463=(
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp462;if(_tmp463->tag != 25)goto
_LL28A;else{_tmp464=(void*)_tmp463->f1;}}_LL289: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,_tmp464,e2) || may_constrain_evars  && Cyc_Tcutil_unify(
_tmp464,Cyc_Absyn_sint_typ);_LL28A:;_LL28B: return Cyc_Tcutil_subset_effect(
may_constrain_evars,_tmp462,e2);_LL287:;}_LL283: {struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp45E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp455;if(_tmp45E->tag != 1)
goto _LL285;else{_tmp45F=(void**)& _tmp45E->f2;_tmp460=(void**)((void**)& _tmp45E->f2);
_tmp461=_tmp45E->f4;}}_LL284: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*_tmp460=(void*)
Cyc_Absyn_empty_effect;return 1;_LL285:;_LL286: {const char*_tmp124A;void*_tmp1249[
1];struct Cyc_String_pa_PrintArg_struct _tmp1248;(_tmp1248.tag=0,((_tmp1248.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((
_tmp1249[0]=& _tmp1248,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp124A="subset_effect: bad effect: %s",_tag_dyneither(
_tmp124A,sizeof(char),30))),_tag_dyneither(_tmp1249,sizeof(void*),1)))))));}
_LL27A:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && 
Cyc_Tcutil_subset_effect(0,e2,e1))return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2)
 && Cyc_Tcutil_subset_effect(1,e2,e1))return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple0 _tmp469;void*_tmp46A;void*_tmp46B;struct
_tuple0*_tmp468=(struct _tuple0*)r1->hd;_tmp469=*_tmp468;_tmp46A=_tmp469.f1;
_tmp46B=_tmp469.f2;{int found=_tmp46A == (void*)& Cyc_Absyn_HeapRgn_val;{struct Cyc_List_List*
r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){struct _tuple0 _tmp46D;void*_tmp46E;void*
_tmp46F;struct _tuple0*_tmp46C=(struct _tuple0*)r2->hd;_tmp46D=*_tmp46C;_tmp46E=
_tmp46D.f1;_tmp46F=_tmp46D.f2;if(Cyc_Tcutil_unify(_tmp46A,_tmp46E) && Cyc_Tcutil_unify(
_tmp46B,_tmp46F)){found=1;break;}}}if(!found)return 0;};}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);t1=
Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return;{void*
_tmp470=t1;struct Cyc_Core_Opt*_tmp472;void*_tmp473;void**_tmp474;struct Cyc_Core_Opt*
_tmp475;_LL28D: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp471=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)
_tmp470;if(_tmp471->tag != 1)goto _LL28F;else{_tmp472=_tmp471->f1;_tmp473=(void**)&
_tmp471->f2;_tmp474=(void**)((void**)& _tmp471->f2);_tmp475=_tmp471->f4;}}_LL28E:
Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp475))->v,t2);{struct Cyc_Absyn_Kind*_tmp476=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp476,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)
_check_null(_tmp472))->v)){*_tmp474=(void*)t2;return;}else{{void*_tmp477=t2;void*
_tmp479;void**_tmp47A;struct Cyc_Core_Opt*_tmp47B;struct Cyc_Absyn_PtrInfo _tmp47D;
_LL292: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp478=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)
_tmp477;if(_tmp478->tag != 1)goto _LL294;else{_tmp479=(void**)& _tmp478->f2;_tmp47A=(
void**)((void**)& _tmp478->f2);_tmp47B=_tmp478->f4;}}_LL293: {struct Cyc_List_List*
_tmp47E=(struct Cyc_List_List*)_tmp475->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp47B))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp47E,(struct
Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmp124B;Cyc_Tcutil_failure_reason=((
_tmp124B="(type variable would escape scope)",_tag_dyneither(_tmp124B,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}if(Cyc_Tcutil_kind_leq((
struct Cyc_Absyn_Kind*)_tmp472->v,_tmp476)){*_tmp47A=(void*)t1;return;}{const char*
_tmp124C;Cyc_Tcutil_failure_reason=((_tmp124C="(kinds are incompatible)",
_tag_dyneither(_tmp124C,sizeof(char),25)));}goto _LL291;}_LL294:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp47C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp477;if(_tmp47C->tag
!= 5)goto _LL296;else{_tmp47D=_tmp47C->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp472->v)->kind
== Cyc_Absyn_BoxKind))goto _LL296;_LL295: {union Cyc_Absyn_Constraint*_tmp481=((
union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((
_tmp47D.ptr_atts).bounds);{union Cyc_Absyn_Constraint*_tmp482=_tmp481;union Cyc_Absyn_Constraint
_tmp483;int _tmp484;_LL299: _tmp483=*_tmp482;if((_tmp483.No_constr).tag != 3)goto
_LL29B;_tmp484=(int)(_tmp483.No_constr).val;_LL29A:{struct
_union_Constraint_Eq_constr*_tmp124D;(_tmp124D=& _tmp481->Eq_constr,((_tmp124D->tag=
1,_tmp124D->val=Cyc_Absyn_bounds_one)));}*_tmp474=(void*)t2;return;_LL29B:;
_LL29C: goto _LL298;_LL298:;}goto _LL291;}_LL296:;_LL297: goto _LL291;_LL291:;}{const
char*_tmp124E;Cyc_Tcutil_failure_reason=((_tmp124E="(kinds are incompatible)",
_tag_dyneither(_tmp124E,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};
_LL28F:;_LL290: goto _LL28C;_LL28C:;}{struct _tuple0 _tmp124F;struct _tuple0 _tmp488=(
_tmp124F.f1=t2,((_tmp124F.f2=t1,_tmp124F)));void*_tmp489;void*_tmp48B;void*
_tmp48D;void*_tmp48F;struct Cyc_Absyn_Tvar*_tmp491;void*_tmp492;struct Cyc_Absyn_Tvar*
_tmp494;void*_tmp495;struct Cyc_Absyn_AggrInfo _tmp497;union Cyc_Absyn_AggrInfoU
_tmp498;struct Cyc_List_List*_tmp499;void*_tmp49A;struct Cyc_Absyn_AggrInfo _tmp49C;
union Cyc_Absyn_AggrInfoU _tmp49D;struct Cyc_List_List*_tmp49E;void*_tmp49F;struct
_tuple2*_tmp4A1;void*_tmp4A2;struct _tuple2*_tmp4A4;void*_tmp4A5;struct Cyc_List_List*
_tmp4A7;void*_tmp4A8;struct Cyc_List_List*_tmp4AA;void*_tmp4AB;struct Cyc_Absyn_DatatypeInfo
_tmp4AD;union Cyc_Absyn_DatatypeInfoU _tmp4AE;struct Cyc_Absyn_Datatypedecl**
_tmp4AF;struct Cyc_Absyn_Datatypedecl*_tmp4B0;struct Cyc_List_List*_tmp4B1;void*
_tmp4B2;struct Cyc_Absyn_DatatypeInfo _tmp4B4;union Cyc_Absyn_DatatypeInfoU _tmp4B5;
struct Cyc_Absyn_Datatypedecl**_tmp4B6;struct Cyc_Absyn_Datatypedecl*_tmp4B7;
struct Cyc_List_List*_tmp4B8;void*_tmp4B9;struct Cyc_Absyn_DatatypeFieldInfo
_tmp4BB;union Cyc_Absyn_DatatypeFieldInfoU _tmp4BC;struct _tuple3 _tmp4BD;struct Cyc_Absyn_Datatypedecl*
_tmp4BE;struct Cyc_Absyn_Datatypefield*_tmp4BF;struct Cyc_List_List*_tmp4C0;void*
_tmp4C1;struct Cyc_Absyn_DatatypeFieldInfo _tmp4C3;union Cyc_Absyn_DatatypeFieldInfoU
_tmp4C4;struct _tuple3 _tmp4C5;struct Cyc_Absyn_Datatypedecl*_tmp4C6;struct Cyc_Absyn_Datatypefield*
_tmp4C7;struct Cyc_List_List*_tmp4C8;void*_tmp4C9;struct Cyc_Absyn_PtrInfo _tmp4CB;
void*_tmp4CC;struct Cyc_Absyn_Tqual _tmp4CD;struct Cyc_Absyn_PtrAtts _tmp4CE;void*
_tmp4CF;union Cyc_Absyn_Constraint*_tmp4D0;union Cyc_Absyn_Constraint*_tmp4D1;
union Cyc_Absyn_Constraint*_tmp4D2;void*_tmp4D3;struct Cyc_Absyn_PtrInfo _tmp4D5;
void*_tmp4D6;struct Cyc_Absyn_Tqual _tmp4D7;struct Cyc_Absyn_PtrAtts _tmp4D8;void*
_tmp4D9;union Cyc_Absyn_Constraint*_tmp4DA;union Cyc_Absyn_Constraint*_tmp4DB;
union Cyc_Absyn_Constraint*_tmp4DC;void*_tmp4DD;enum Cyc_Absyn_Sign _tmp4DF;enum 
Cyc_Absyn_Size_of _tmp4E0;void*_tmp4E1;enum Cyc_Absyn_Sign _tmp4E3;enum Cyc_Absyn_Size_of
_tmp4E4;void*_tmp4E5;int _tmp4E7;void*_tmp4E8;int _tmp4EA;void*_tmp4EB;void*
_tmp4ED;void*_tmp4EE;void*_tmp4F0;void*_tmp4F1;struct Cyc_Absyn_Exp*_tmp4F3;void*
_tmp4F4;struct Cyc_Absyn_Exp*_tmp4F6;void*_tmp4F7;struct Cyc_Absyn_ArrayInfo
_tmp4F9;void*_tmp4FA;struct Cyc_Absyn_Tqual _tmp4FB;struct Cyc_Absyn_Exp*_tmp4FC;
union Cyc_Absyn_Constraint*_tmp4FD;void*_tmp4FE;struct Cyc_Absyn_ArrayInfo _tmp500;
void*_tmp501;struct Cyc_Absyn_Tqual _tmp502;struct Cyc_Absyn_Exp*_tmp503;union Cyc_Absyn_Constraint*
_tmp504;void*_tmp505;struct Cyc_Absyn_FnInfo _tmp507;struct Cyc_List_List*_tmp508;
void*_tmp509;struct Cyc_Absyn_Tqual _tmp50A;void*_tmp50B;struct Cyc_List_List*
_tmp50C;int _tmp50D;struct Cyc_Absyn_VarargInfo*_tmp50E;struct Cyc_List_List*
_tmp50F;struct Cyc_List_List*_tmp510;void*_tmp511;struct Cyc_Absyn_FnInfo _tmp513;
struct Cyc_List_List*_tmp514;void*_tmp515;struct Cyc_Absyn_Tqual _tmp516;void*
_tmp517;struct Cyc_List_List*_tmp518;int _tmp519;struct Cyc_Absyn_VarargInfo*
_tmp51A;struct Cyc_List_List*_tmp51B;struct Cyc_List_List*_tmp51C;void*_tmp51D;
struct Cyc_List_List*_tmp51F;void*_tmp520;struct Cyc_List_List*_tmp522;void*
_tmp523;enum Cyc_Absyn_AggrKind _tmp525;struct Cyc_List_List*_tmp526;void*_tmp527;
enum Cyc_Absyn_AggrKind _tmp529;struct Cyc_List_List*_tmp52A;void*_tmp52B;struct
Cyc_List_List*_tmp52D;struct Cyc_Absyn_Typedefdecl*_tmp52E;void*_tmp52F;struct Cyc_List_List*
_tmp531;struct Cyc_Absyn_Typedefdecl*_tmp532;void*_tmp533;void*_tmp535;void*
_tmp537;void*_tmp539;void*_tmp53B;void*_tmp53D;void*_tmp53F;void*_tmp541;void*
_tmp542;void*_tmp544;void*_tmp545;void*_tmp547;void*_tmp548;void*_tmp549;void*
_tmp54B;void*_tmp54C;void*_tmp54D;void*_tmp54F;void*_tmp551;void*_tmp553;void*
_tmp555;void*_tmp557;_LL29E: _tmp489=_tmp488.f1;{struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp48A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp489;if(_tmp48A->tag != 1)
goto _LL2A0;};_LL29F: Cyc_Tcutil_unify_it(t2,t1);return;_LL2A0: _tmp48B=_tmp488.f1;{
struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp48C=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)
_tmp48B;if(_tmp48C->tag != 0)goto _LL2A2;};_tmp48D=_tmp488.f2;{struct Cyc_Absyn_VoidType_Absyn_Type_struct*
_tmp48E=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp48D;if(_tmp48E->tag != 0)
goto _LL2A2;};_LL2A1: return;_LL2A2: _tmp48F=_tmp488.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp490=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp48F;if(_tmp490->tag != 2)
goto _LL2A4;else{_tmp491=_tmp490->f1;}};_tmp492=_tmp488.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp493=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp492;if(_tmp493->tag != 2)
goto _LL2A4;else{_tmp494=_tmp493->f1;}};_LL2A3: {struct _dyneither_ptr*_tmp559=
_tmp491->name;struct _dyneither_ptr*_tmp55A=_tmp494->name;int _tmp55B=_tmp491->identity;
int _tmp55C=_tmp494->identity;if(_tmp55C == _tmp55B)return;{const char*_tmp1250;Cyc_Tcutil_failure_reason=((
_tmp1250="(variable types are not the same)",_tag_dyneither(_tmp1250,sizeof(char),
34)));}goto _LL29D;}_LL2A4: _tmp495=_tmp488.f1;{struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp496=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp495;if(_tmp496->tag != 
11)goto _LL2A6;else{_tmp497=_tmp496->f1;_tmp498=_tmp497.aggr_info;_tmp499=_tmp497.targs;}};
_tmp49A=_tmp488.f2;{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp49B=(struct
Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp49A;if(_tmp49B->tag != 11)goto _LL2A6;
else{_tmp49C=_tmp49B->f1;_tmp49D=_tmp49C.aggr_info;_tmp49E=_tmp49C.targs;}};
_LL2A5: {enum Cyc_Absyn_AggrKind _tmp55F;struct _tuple2*_tmp560;struct _tuple13
_tmp55E=Cyc_Absyn_aggr_kinded_name(_tmp49D);_tmp55F=_tmp55E.f1;_tmp560=_tmp55E.f2;{
enum Cyc_Absyn_AggrKind _tmp562;struct _tuple2*_tmp563;struct _tuple13 _tmp561=Cyc_Absyn_aggr_kinded_name(
_tmp498);_tmp562=_tmp561.f1;_tmp563=_tmp561.f2;if(_tmp55F != _tmp562){{const char*
_tmp1251;Cyc_Tcutil_failure_reason=((_tmp1251="(struct and union type)",
_tag_dyneither(_tmp1251,sizeof(char),24)));}goto _LL29D;}if(Cyc_Absyn_qvar_cmp(
_tmp560,_tmp563)!= 0){{const char*_tmp1252;Cyc_Tcutil_failure_reason=((_tmp1252="(different type name)",
_tag_dyneither(_tmp1252,sizeof(char),22)));}goto _LL29D;}Cyc_Tcutil_unify_list(
_tmp49E,_tmp499);return;};}_LL2A6: _tmp49F=_tmp488.f1;{struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp4A0=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp49F;if(_tmp4A0->tag != 
13)goto _LL2A8;else{_tmp4A1=_tmp4A0->f1;}};_tmp4A2=_tmp488.f2;{struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp4A3=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4A2;if(_tmp4A3->tag != 
13)goto _LL2A8;else{_tmp4A4=_tmp4A3->f1;}};_LL2A7: if(Cyc_Absyn_qvar_cmp(_tmp4A1,
_tmp4A4)== 0)return;{const char*_tmp1253;Cyc_Tcutil_failure_reason=((_tmp1253="(different enum types)",
_tag_dyneither(_tmp1253,sizeof(char),23)));}goto _LL29D;_LL2A8: _tmp4A5=_tmp488.f1;{
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4A6=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)
_tmp4A5;if(_tmp4A6->tag != 14)goto _LL2AA;else{_tmp4A7=_tmp4A6->f1;}};_tmp4A8=
_tmp488.f2;{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4A9=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)
_tmp4A8;if(_tmp4A9->tag != 14)goto _LL2AA;else{_tmp4AA=_tmp4A9->f1;}};_LL2A9: {int
bad=0;for(0;_tmp4A7 != 0  && _tmp4AA != 0;(_tmp4A7=_tmp4A7->tl,_tmp4AA=_tmp4AA->tl)){
struct Cyc_Absyn_Enumfield*_tmp567=(struct Cyc_Absyn_Enumfield*)_tmp4A7->hd;struct
Cyc_Absyn_Enumfield*_tmp568=(struct Cyc_Absyn_Enumfield*)_tmp4AA->hd;if(Cyc_Absyn_qvar_cmp(
_tmp567->name,_tmp568->name)!= 0){{const char*_tmp1254;Cyc_Tcutil_failure_reason=((
_tmp1254="(different names for enum fields)",_tag_dyneither(_tmp1254,sizeof(char),
34)));}bad=1;break;}if(_tmp567->tag == _tmp568->tag)continue;if(_tmp567->tag == 0
 || _tmp568->tag == 0){{const char*_tmp1255;Cyc_Tcutil_failure_reason=((_tmp1255="(different tag values for enum fields)",
_tag_dyneither(_tmp1255,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp567->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp568->tag))){{const char*_tmp1256;Cyc_Tcutil_failure_reason=((_tmp1256="(different tag values for enum fields)",
_tag_dyneither(_tmp1256,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL29D;if(
_tmp4A7 == 0  && _tmp4AA == 0)return;{const char*_tmp1257;Cyc_Tcutil_failure_reason=((
_tmp1257="(different number of fields for enums)",_tag_dyneither(_tmp1257,
sizeof(char),39)));}goto _LL29D;}_LL2AA: _tmp4AB=_tmp488.f1;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*
_tmp4AC=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4AB;if(_tmp4AC->tag
!= 3)goto _LL2AC;else{_tmp4AD=_tmp4AC->f1;_tmp4AE=_tmp4AD.datatype_info;if((
_tmp4AE.KnownDatatype).tag != 2)goto _LL2AC;_tmp4AF=(struct Cyc_Absyn_Datatypedecl**)(
_tmp4AE.KnownDatatype).val;_tmp4B0=*_tmp4AF;_tmp4B1=_tmp4AD.targs;}};_tmp4B2=
_tmp488.f2;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4B3=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)
_tmp4B2;if(_tmp4B3->tag != 3)goto _LL2AC;else{_tmp4B4=_tmp4B3->f1;_tmp4B5=_tmp4B4.datatype_info;
if((_tmp4B5.KnownDatatype).tag != 2)goto _LL2AC;_tmp4B6=(struct Cyc_Absyn_Datatypedecl**)(
_tmp4B5.KnownDatatype).val;_tmp4B7=*_tmp4B6;_tmp4B8=_tmp4B4.targs;}};_LL2AB: if(
_tmp4B0 == _tmp4B7  || Cyc_Absyn_qvar_cmp(_tmp4B0->name,_tmp4B7->name)== 0){Cyc_Tcutil_unify_list(
_tmp4B8,_tmp4B1);return;}goto _LL29D;_LL2AC: _tmp4B9=_tmp488.f1;{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmp4BA=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4B9;if(_tmp4BA->tag
!= 4)goto _LL2AE;else{_tmp4BB=_tmp4BA->f1;_tmp4BC=_tmp4BB.field_info;if((_tmp4BC.KnownDatatypefield).tag
!= 2)goto _LL2AE;_tmp4BD=(struct _tuple3)(_tmp4BC.KnownDatatypefield).val;_tmp4BE=
_tmp4BD.f1;_tmp4BF=_tmp4BD.f2;_tmp4C0=_tmp4BB.targs;}};_tmp4C1=_tmp488.f2;{
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4C2=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)
_tmp4C1;if(_tmp4C2->tag != 4)goto _LL2AE;else{_tmp4C3=_tmp4C2->f1;_tmp4C4=_tmp4C3.field_info;
if((_tmp4C4.KnownDatatypefield).tag != 2)goto _LL2AE;_tmp4C5=(struct _tuple3)(
_tmp4C4.KnownDatatypefield).val;_tmp4C6=_tmp4C5.f1;_tmp4C7=_tmp4C5.f2;_tmp4C8=
_tmp4C3.targs;}};_LL2AD: if((_tmp4BE == _tmp4C6  || Cyc_Absyn_qvar_cmp(_tmp4BE->name,
_tmp4C6->name)== 0) && (_tmp4BF == _tmp4C7  || Cyc_Absyn_qvar_cmp(_tmp4BF->name,
_tmp4C7->name)== 0)){Cyc_Tcutil_unify_list(_tmp4C8,_tmp4C0);return;}{const char*
_tmp1258;Cyc_Tcutil_failure_reason=((_tmp1258="(different datatype field types)",
_tag_dyneither(_tmp1258,sizeof(char),33)));}goto _LL29D;_LL2AE: _tmp4C9=_tmp488.f1;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4CA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp4C9;if(_tmp4CA->tag != 5)goto _LL2B0;else{_tmp4CB=_tmp4CA->f1;_tmp4CC=_tmp4CB.elt_typ;
_tmp4CD=_tmp4CB.elt_tq;_tmp4CE=_tmp4CB.ptr_atts;_tmp4CF=_tmp4CE.rgn;_tmp4D0=
_tmp4CE.nullable;_tmp4D1=_tmp4CE.bounds;_tmp4D2=_tmp4CE.zero_term;}};_tmp4D3=
_tmp488.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4D4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp4D3;if(_tmp4D4->tag != 5)goto _LL2B0;else{_tmp4D5=_tmp4D4->f1;_tmp4D6=_tmp4D5.elt_typ;
_tmp4D7=_tmp4D5.elt_tq;_tmp4D8=_tmp4D5.ptr_atts;_tmp4D9=_tmp4D8.rgn;_tmp4DA=
_tmp4D8.nullable;_tmp4DB=_tmp4D8.bounds;_tmp4DC=_tmp4D8.zero_term;}};_LL2AF: Cyc_Tcutil_unify_it(
_tmp4D6,_tmp4CC);Cyc_Tcutil_unify_it(_tmp4CF,_tmp4D9);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{const char*_tmp1259;((void(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp4DC,_tmp4D2,((_tmp1259="(not both zero terminated)",
_tag_dyneither(_tmp1259,sizeof(char),27))));}Cyc_Tcutil_unify_tqual(_tmp4D7,
_tmp4D6,_tmp4CD,_tmp4CC);{const char*_tmp125A;((void(*)(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr
reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp4DB,_tmp4D1,((
_tmp125A="(different pointer bounds)",_tag_dyneither(_tmp125A,sizeof(char),27))));}{
void*_tmp570=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp4DB);_LL2DB: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp571=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp570;if(_tmp571->tag
!= 0)goto _LL2DD;}_LL2DC: return;_LL2DD:;_LL2DE: goto _LL2DA;_LL2DA:;}{const char*
_tmp125B;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp4DA,_tmp4D0,((_tmp125B="(incompatible pointer types)",_tag_dyneither(
_tmp125B,sizeof(char),29))));}return;_LL2B0: _tmp4DD=_tmp488.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp4DE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4DD;if(_tmp4DE->tag != 6)
goto _LL2B2;else{_tmp4DF=_tmp4DE->f1;_tmp4E0=_tmp4DE->f2;}};_tmp4E1=_tmp488.f2;{
struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4E2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp4E1;if(_tmp4E2->tag != 6)goto _LL2B2;else{_tmp4E3=_tmp4E2->f1;_tmp4E4=_tmp4E2->f2;}};
_LL2B1: if(_tmp4E3 == _tmp4DF  && ((_tmp4E4 == _tmp4E0  || _tmp4E4 == Cyc_Absyn_Int_sz
 && _tmp4E0 == Cyc_Absyn_Long_sz) || _tmp4E4 == Cyc_Absyn_Long_sz  && _tmp4E0 == Cyc_Absyn_Int_sz))
return;{const char*_tmp125C;Cyc_Tcutil_failure_reason=((_tmp125C="(different integral types)",
_tag_dyneither(_tmp125C,sizeof(char),27)));}goto _LL29D;_LL2B2: _tmp4E5=_tmp488.f1;{
struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4E6=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)
_tmp4E5;if(_tmp4E6->tag != 7)goto _LL2B4;else{_tmp4E7=_tmp4E6->f1;}};_tmp4E8=
_tmp488.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4E9=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)
_tmp4E8;if(_tmp4E9->tag != 7)goto _LL2B4;else{_tmp4EA=_tmp4E9->f1;}};_LL2B3: if(
_tmp4EA == 0  && _tmp4E7 == 0)return;else{if(_tmp4EA == 1  && _tmp4E7 == 1)return;
else{if(((_tmp4EA != 0  && _tmp4EA != 1) && _tmp4E7 != 0) && _tmp4E7 != 1)return;}}
goto _LL29D;_LL2B4: _tmp4EB=_tmp488.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmp4EC=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4EB;if(_tmp4EC->tag != 19)
goto _LL2B6;else{_tmp4ED=(void*)_tmp4EC->f1;}};_tmp4EE=_tmp488.f2;{struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmp4EF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4EE;if(_tmp4EF->tag != 19)
goto _LL2B6;else{_tmp4F0=(void*)_tmp4EF->f1;}};_LL2B5: Cyc_Tcutil_unify_it(_tmp4ED,
_tmp4F0);return;_LL2B6: _tmp4F1=_tmp488.f1;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*
_tmp4F2=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4F1;if(_tmp4F2->tag
!= 18)goto _LL2B8;else{_tmp4F3=_tmp4F2->f1;}};_tmp4F4=_tmp488.f2;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*
_tmp4F5=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4F4;if(_tmp4F5->tag
!= 18)goto _LL2B8;else{_tmp4F6=_tmp4F5->f1;}};_LL2B7: if(!Cyc_Evexp_same_const_exp(
_tmp4F3,_tmp4F6)){{const char*_tmp125D;Cyc_Tcutil_failure_reason=((_tmp125D="(cannot prove expressions are the same)",
_tag_dyneither(_tmp125D,sizeof(char),40)));}goto _LL29D;}return;_LL2B8: _tmp4F7=
_tmp488.f1;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4F8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)
_tmp4F7;if(_tmp4F8->tag != 8)goto _LL2BA;else{_tmp4F9=_tmp4F8->f1;_tmp4FA=_tmp4F9.elt_type;
_tmp4FB=_tmp4F9.tq;_tmp4FC=_tmp4F9.num_elts;_tmp4FD=_tmp4F9.zero_term;}};_tmp4FE=
_tmp488.f2;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4FF=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)
_tmp4FE;if(_tmp4FF->tag != 8)goto _LL2BA;else{_tmp500=_tmp4FF->f1;_tmp501=_tmp500.elt_type;
_tmp502=_tmp500.tq;_tmp503=_tmp500.num_elts;_tmp504=_tmp500.zero_term;}};_LL2B9:
Cyc_Tcutil_unify_it(_tmp501,_tmp4FA);Cyc_Tcutil_unify_tqual(_tmp502,_tmp501,
_tmp4FB,_tmp4FA);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*
_tmp125E;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp4FD,_tmp504,((_tmp125E="(not both zero terminated)",_tag_dyneither(_tmp125E,
sizeof(char),27))));}if(_tmp4FC == _tmp503)return;if(_tmp4FC == 0  || _tmp503 == 0)
goto _LL29D;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp4FC,(struct Cyc_Absyn_Exp*)
_tmp503))return;{const char*_tmp125F;Cyc_Tcutil_failure_reason=((_tmp125F="(different array sizes)",
_tag_dyneither(_tmp125F,sizeof(char),24)));}goto _LL29D;_LL2BA: _tmp505=_tmp488.f1;{
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp506=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)
_tmp505;if(_tmp506->tag != 9)goto _LL2BC;else{_tmp507=_tmp506->f1;_tmp508=_tmp507.tvars;
_tmp509=_tmp507.effect;_tmp50A=_tmp507.ret_tqual;_tmp50B=_tmp507.ret_typ;_tmp50C=
_tmp507.args;_tmp50D=_tmp507.c_varargs;_tmp50E=_tmp507.cyc_varargs;_tmp50F=
_tmp507.rgn_po;_tmp510=_tmp507.attributes;}};_tmp511=_tmp488.f2;{struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp512=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp511;if(_tmp512->tag != 9)
goto _LL2BC;else{_tmp513=_tmp512->f1;_tmp514=_tmp513.tvars;_tmp515=_tmp513.effect;
_tmp516=_tmp513.ret_tqual;_tmp517=_tmp513.ret_typ;_tmp518=_tmp513.args;_tmp519=
_tmp513.c_varargs;_tmp51A=_tmp513.cyc_varargs;_tmp51B=_tmp513.rgn_po;_tmp51C=
_tmp513.attributes;}};_LL2BB: {int done=0;struct _RegionHandle _tmp577=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp577;_push_region(rgn);{struct Cyc_List_List*inst=0;
while(_tmp514 != 0){if(_tmp508 == 0){{const char*_tmp1260;Cyc_Tcutil_failure_reason=((
_tmp1260="(second function type has too few type variables)",_tag_dyneither(
_tmp1260,sizeof(char),50)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{void*
_tmp579=((struct Cyc_Absyn_Tvar*)_tmp514->hd)->kind;void*_tmp57A=((struct Cyc_Absyn_Tvar*)
_tmp508->hd)->kind;if(!Cyc_Tcutil_unify_kindbound(_tmp579,_tmp57A)){{const char*
_tmp1266;void*_tmp1265[3];struct Cyc_String_pa_PrintArg_struct _tmp1264;struct Cyc_String_pa_PrintArg_struct
_tmp1263;struct Cyc_String_pa_PrintArg_struct _tmp1262;Cyc_Tcutil_failure_reason=(
struct _dyneither_ptr)((_tmp1262.tag=0,((_tmp1262.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp508->hd,& Cyc_Tcutil_bk))),((_tmp1263.tag=0,((_tmp1263.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp514->hd,& Cyc_Tcutil_bk))),((_tmp1264.tag=0,((_tmp1264.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
_tmp514->hd)),((_tmp1265[0]=& _tmp1264,((_tmp1265[1]=& _tmp1263,((_tmp1265[2]=&
_tmp1262,Cyc_aprintf(((_tmp1266="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmp1266,sizeof(char),44))),_tag_dyneither(_tmp1265,sizeof(void*),
3))))))))))))))))))));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{struct
_tuple16*_tmp1270;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp126F;struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp126E;struct Cyc_List_List*_tmp126D;inst=((_tmp126D=_region_malloc(rgn,sizeof(*
_tmp126D)),((_tmp126D->hd=((_tmp1270=_region_malloc(rgn,sizeof(*_tmp1270)),((
_tmp1270->f1=(struct Cyc_Absyn_Tvar*)_tmp508->hd,((_tmp1270->f2=(void*)((_tmp126E=
_cycalloc(sizeof(*_tmp126E)),((_tmp126E[0]=((_tmp126F.tag=2,((_tmp126F.f1=(
struct Cyc_Absyn_Tvar*)_tmp514->hd,_tmp126F)))),_tmp126E)))),_tmp1270)))))),((
_tmp126D->tl=inst,_tmp126D))))));}_tmp514=_tmp514->tl;_tmp508=_tmp508->tl;};}if(
_tmp508 != 0){{const char*_tmp1271;Cyc_Tcutil_failure_reason=((_tmp1271="(second function type has too many type variables)",
_tag_dyneither(_tmp1271,sizeof(char),51)));}_npop_handler(0);goto _LL29D;}if(inst
!= 0){{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp127D;struct Cyc_Absyn_FnInfo
_tmp127C;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp127B;struct Cyc_Absyn_FnType_Absyn_Type_struct
_tmp1277;struct Cyc_Absyn_FnInfo _tmp1276;struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp1275;Cyc_Tcutil_unify_it((void*)((_tmp1275=_cycalloc(sizeof(*_tmp1275)),((
_tmp1275[0]=((_tmp1277.tag=9,((_tmp1277.f1=((_tmp1276.tvars=0,((_tmp1276.effect=
_tmp515,((_tmp1276.ret_tqual=_tmp516,((_tmp1276.ret_typ=_tmp517,((_tmp1276.args=
_tmp518,((_tmp1276.c_varargs=_tmp519,((_tmp1276.cyc_varargs=_tmp51A,((_tmp1276.rgn_po=
_tmp51B,((_tmp1276.attributes=_tmp51C,_tmp1276)))))))))))))))))),_tmp1277)))),
_tmp1275)))),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)((_tmp127B=_cycalloc(sizeof(*
_tmp127B)),((_tmp127B[0]=((_tmp127D.tag=9,((_tmp127D.f1=((_tmp127C.tvars=0,((
_tmp127C.effect=_tmp509,((_tmp127C.ret_tqual=_tmp50A,((_tmp127C.ret_typ=_tmp50B,((
_tmp127C.args=_tmp50C,((_tmp127C.c_varargs=_tmp50D,((_tmp127C.cyc_varargs=
_tmp50E,((_tmp127C.rgn_po=_tmp50F,((_tmp127C.attributes=_tmp510,_tmp127C)))))))))))))))))),
_tmp127D)))),_tmp127B))))));}done=1;}}if(done){_npop_handler(0);return;}Cyc_Tcutil_unify_it(
_tmp517,_tmp50B);Cyc_Tcutil_unify_tqual(_tmp516,_tmp517,_tmp50A,_tmp50B);for(0;
_tmp518 != 0  && _tmp50C != 0;(_tmp518=_tmp518->tl,_tmp50C=_tmp50C->tl)){struct Cyc_Absyn_Tqual
_tmp58C;void*_tmp58D;struct _tuple10 _tmp58B=*((struct _tuple10*)_tmp518->hd);
_tmp58C=_tmp58B.f2;_tmp58D=_tmp58B.f3;{struct Cyc_Absyn_Tqual _tmp58F;void*_tmp590;
struct _tuple10 _tmp58E=*((struct _tuple10*)_tmp50C->hd);_tmp58F=_tmp58E.f2;_tmp590=
_tmp58E.f3;Cyc_Tcutil_unify_it(_tmp58D,_tmp590);Cyc_Tcutil_unify_tqual(_tmp58C,
_tmp58D,_tmp58F,_tmp590);};}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;
if(_tmp518 != 0  || _tmp50C != 0){{const char*_tmp127E;Cyc_Tcutil_failure_reason=((
_tmp127E="(function types have different number of arguments)",_tag_dyneither(
_tmp127E,sizeof(char),52)));}_npop_handler(0);goto _LL29D;}if(_tmp519 != _tmp50D){{
const char*_tmp127F;Cyc_Tcutil_failure_reason=((_tmp127F="(only one function type takes C varargs)",
_tag_dyneither(_tmp127F,sizeof(char),41)));}_npop_handler(0);goto _LL29D;}{int
bad_cyc_vararg=0;{struct _tuple22 _tmp1280;struct _tuple22 _tmp594=(_tmp1280.f1=
_tmp51A,((_tmp1280.f2=_tmp50E,_tmp1280)));struct Cyc_Absyn_VarargInfo*_tmp595;
struct Cyc_Absyn_VarargInfo*_tmp596;struct Cyc_Absyn_VarargInfo*_tmp597;struct Cyc_Absyn_VarargInfo*
_tmp598;struct Cyc_Absyn_VarargInfo*_tmp599;struct Cyc_Absyn_VarargInfo _tmp59A;
struct _dyneither_ptr*_tmp59B;struct Cyc_Absyn_Tqual _tmp59C;void*_tmp59D;int
_tmp59E;struct Cyc_Absyn_VarargInfo*_tmp59F;struct Cyc_Absyn_VarargInfo _tmp5A0;
struct _dyneither_ptr*_tmp5A1;struct Cyc_Absyn_Tqual _tmp5A2;void*_tmp5A3;int
_tmp5A4;_LL2E0: _tmp595=_tmp594.f1;if(_tmp595 != 0)goto _LL2E2;_tmp596=_tmp594.f2;
if(_tmp596 != 0)goto _LL2E2;_LL2E1: goto _LL2DF;_LL2E2: _tmp597=_tmp594.f1;if(_tmp597
!= 0)goto _LL2E4;_LL2E3: goto _LL2E5;_LL2E4: _tmp598=_tmp594.f2;if(_tmp598 != 0)goto
_LL2E6;_LL2E5: bad_cyc_vararg=1;{const char*_tmp1281;Cyc_Tcutil_failure_reason=((
_tmp1281="(only one function type takes varargs)",_tag_dyneither(_tmp1281,
sizeof(char),39)));}goto _LL2DF;_LL2E6: _tmp599=_tmp594.f1;if(_tmp599 == 0)goto
_LL2DF;_tmp59A=*_tmp599;_tmp59B=_tmp59A.name;_tmp59C=_tmp59A.tq;_tmp59D=_tmp59A.type;
_tmp59E=_tmp59A.inject;_tmp59F=_tmp594.f2;if(_tmp59F == 0)goto _LL2DF;_tmp5A0=*
_tmp59F;_tmp5A1=_tmp5A0.name;_tmp5A2=_tmp5A0.tq;_tmp5A3=_tmp5A0.type;_tmp5A4=
_tmp5A0.inject;_LL2E7: Cyc_Tcutil_unify_it(_tmp59D,_tmp5A3);Cyc_Tcutil_unify_tqual(
_tmp59C,_tmp59D,_tmp5A2,_tmp5A3);if(_tmp59E != _tmp5A4){bad_cyc_vararg=1;{const
char*_tmp1282;Cyc_Tcutil_failure_reason=((_tmp1282="(only one function type injects varargs)",
_tag_dyneither(_tmp1282,sizeof(char),41)));};}goto _LL2DF;_LL2DF:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL29D;}{int bad_effect=0;{struct _tuple0
_tmp1283;struct _tuple0 _tmp5A8=(_tmp1283.f1=_tmp515,((_tmp1283.f2=_tmp509,
_tmp1283)));void*_tmp5A9;void*_tmp5AA;void*_tmp5AB;void*_tmp5AC;_LL2E9: _tmp5A9=
_tmp5A8.f1;if(_tmp5A9 != 0)goto _LL2EB;_tmp5AA=_tmp5A8.f2;if(_tmp5AA != 0)goto
_LL2EB;_LL2EA: goto _LL2E8;_LL2EB: _tmp5AB=_tmp5A8.f1;if(_tmp5AB != 0)goto _LL2ED;
_LL2EC: goto _LL2EE;_LL2ED: _tmp5AC=_tmp5A8.f2;if(_tmp5AC != 0)goto _LL2EF;_LL2EE:
bad_effect=1;goto _LL2E8;_LL2EF:;_LL2F0: bad_effect=!Cyc_Tcutil_unify_effect((void*)
_check_null(_tmp515),(void*)_check_null(_tmp509));goto _LL2E8;_LL2E8:;}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(bad_effect){{const char*_tmp1284;Cyc_Tcutil_failure_reason=((
_tmp1284="(function type effects do not unify)",_tag_dyneither(_tmp1284,sizeof(
char),37)));}_npop_handler(0);goto _LL29D;}if(!Cyc_Tcutil_same_atts(_tmp510,
_tmp51C)){{const char*_tmp1285;Cyc_Tcutil_failure_reason=((_tmp1285="(function types have different attributes)",
_tag_dyneither(_tmp1285,sizeof(char),43)));}_npop_handler(0);goto _LL29D;}if(!Cyc_Tcutil_same_rgn_po(
_tmp50F,_tmp51B)){{const char*_tmp1286;Cyc_Tcutil_failure_reason=((_tmp1286="(function types have different region lifetime orderings)",
_tag_dyneither(_tmp1286,sizeof(char),58)));}_npop_handler(0);goto _LL29D;}
_npop_handler(0);return;};};;_pop_region(rgn);}_LL2BC: _tmp51D=_tmp488.f1;{struct
Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp51E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)
_tmp51D;if(_tmp51E->tag != 10)goto _LL2BE;else{_tmp51F=_tmp51E->f1;}};_tmp520=
_tmp488.f2;{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp521=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)
_tmp520;if(_tmp521->tag != 10)goto _LL2BE;else{_tmp522=_tmp521->f1;}};_LL2BD: for(0;
_tmp522 != 0  && _tmp51F != 0;(_tmp522=_tmp522->tl,_tmp51F=_tmp51F->tl)){struct Cyc_Absyn_Tqual
_tmp5B1;void*_tmp5B2;struct _tuple12 _tmp5B0=*((struct _tuple12*)_tmp522->hd);
_tmp5B1=_tmp5B0.f1;_tmp5B2=_tmp5B0.f2;{struct Cyc_Absyn_Tqual _tmp5B4;void*_tmp5B5;
struct _tuple12 _tmp5B3=*((struct _tuple12*)_tmp51F->hd);_tmp5B4=_tmp5B3.f1;_tmp5B5=
_tmp5B3.f2;Cyc_Tcutil_unify_it(_tmp5B2,_tmp5B5);Cyc_Tcutil_unify_tqual(_tmp5B1,
_tmp5B2,_tmp5B4,_tmp5B5);};}if(_tmp522 == 0  && _tmp51F == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmp1287;Cyc_Tcutil_failure_reason=((
_tmp1287="(tuple types have different numbers of components)",_tag_dyneither(
_tmp1287,sizeof(char),51)));}goto _LL29D;_LL2BE: _tmp523=_tmp488.f1;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmp524=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp523;if(_tmp524->tag
!= 12)goto _LL2C0;else{_tmp525=_tmp524->f1;_tmp526=_tmp524->f2;}};_tmp527=_tmp488.f2;{
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp528=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)
_tmp527;if(_tmp528->tag != 12)goto _LL2C0;else{_tmp529=_tmp528->f1;_tmp52A=_tmp528->f2;}};
_LL2BF: if(_tmp529 != _tmp525){{const char*_tmp1288;Cyc_Tcutil_failure_reason=((
_tmp1288="(struct and union type)",_tag_dyneither(_tmp1288,sizeof(char),24)));}
goto _LL29D;}for(0;_tmp52A != 0  && _tmp526 != 0;(_tmp52A=_tmp52A->tl,_tmp526=
_tmp526->tl)){struct Cyc_Absyn_Aggrfield*_tmp5B8=(struct Cyc_Absyn_Aggrfield*)
_tmp52A->hd;struct Cyc_Absyn_Aggrfield*_tmp5B9=(struct Cyc_Absyn_Aggrfield*)
_tmp526->hd;if(Cyc_strptrcmp(_tmp5B8->name,_tmp5B9->name)!= 0){{const char*
_tmp1289;Cyc_Tcutil_failure_reason=((_tmp1289="(different member names)",
_tag_dyneither(_tmp1289,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_unify_it(_tmp5B8->type,_tmp5B9->type);Cyc_Tcutil_unify_tqual(_tmp5B8->tq,
_tmp5B8->type,_tmp5B9->tq,_tmp5B9->type);if(!Cyc_Tcutil_same_atts(_tmp5B8->attributes,
_tmp5B9->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmp128A;Cyc_Tcutil_failure_reason=((_tmp128A="(different attributes on member)",
_tag_dyneither(_tmp128A,sizeof(char),33)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
if((_tmp5B8->width != 0  && _tmp5B9->width == 0  || _tmp5B9->width != 0  && _tmp5B8->width
== 0) || (_tmp5B8->width != 0  && _tmp5B9->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp5B8->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp5B9->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmp128B;Cyc_Tcutil_failure_reason=((_tmp128B="(different bitfield widths on member)",
_tag_dyneither(_tmp128B,sizeof(char),38)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
if(_tmp52A == 0  && _tmp526 == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmp128C;Cyc_Tcutil_failure_reason=((_tmp128C="(different number of members)",
_tag_dyneither(_tmp128C,sizeof(char),30)));}goto _LL29D;_LL2C0: _tmp52B=_tmp488.f1;{
struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp52C=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)
_tmp52B;if(_tmp52C->tag != 17)goto _LL2C2;else{_tmp52D=_tmp52C->f2;_tmp52E=_tmp52C->f3;}};
_tmp52F=_tmp488.f2;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp530=(
struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp52F;if(_tmp530->tag != 17)goto
_LL2C2;else{_tmp531=_tmp530->f2;_tmp532=_tmp530->f3;}};_LL2C1: if(_tmp52E != 
_tmp532){{const char*_tmp128D;Cyc_Tcutil_failure_reason=((_tmp128D="(different abstract typedefs)",
_tag_dyneither(_tmp128D,sizeof(char),30)));}goto _LL29D;}{const char*_tmp128E;Cyc_Tcutil_failure_reason=((
_tmp128E="(type parameters to typedef differ)",_tag_dyneither(_tmp128E,sizeof(
char),36)));}Cyc_Tcutil_unify_list(_tmp52D,_tmp531);return;_LL2C2: _tmp533=
_tmp488.f1;{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp534=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)
_tmp533;if(_tmp534->tag != 20)goto _LL2C4;};_tmp535=_tmp488.f2;{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*
_tmp536=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp535;if(_tmp536->tag != 20)
goto _LL2C4;};_LL2C3: return;_LL2C4: _tmp537=_tmp488.f1;{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*
_tmp538=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp537;if(_tmp538->tag != 
21)goto _LL2C6;};_tmp539=_tmp488.f2;{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*
_tmp53A=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp539;if(_tmp53A->tag != 
21)goto _LL2C6;};_LL2C5: return;_LL2C6: _tmp53B=_tmp488.f1;{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*
_tmp53C=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp53B;if(_tmp53C->tag != 
22)goto _LL2C8;};_tmp53D=_tmp488.f2;{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*
_tmp53E=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp53D;if(_tmp53E->tag != 
22)goto _LL2C8;};_LL2C7: return;_LL2C8: _tmp53F=_tmp488.f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*
_tmp540=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp53F;if(_tmp540->tag
!= 15)goto _LL2CA;else{_tmp541=(void*)_tmp540->f1;}};_tmp542=_tmp488.f2;{struct
Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp543=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)
_tmp542;if(_tmp543->tag != 15)goto _LL2CA;else{_tmp544=(void*)_tmp543->f1;}};
_LL2C9: Cyc_Tcutil_unify_it(_tmp541,_tmp544);return;_LL2CA: _tmp545=_tmp488.f1;{
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp546=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)
_tmp545;if(_tmp546->tag != 16)goto _LL2CC;else{_tmp547=(void*)_tmp546->f1;_tmp548=(
void*)_tmp546->f2;}};_tmp549=_tmp488.f2;{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*
_tmp54A=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp549;if(_tmp54A->tag != 
16)goto _LL2CC;else{_tmp54B=(void*)_tmp54A->f1;_tmp54C=(void*)_tmp54A->f2;}};
_LL2CB: Cyc_Tcutil_unify_it(_tmp547,_tmp54B);Cyc_Tcutil_unify_it(_tmp548,_tmp54C);
return;_LL2CC: _tmp54D=_tmp488.f1;{struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmp54E=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp54D;if(_tmp54E->tag != 24)
goto _LL2CE;};_LL2CD: goto _LL2CF;_LL2CE: _tmp54F=_tmp488.f2;{struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmp550=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp54F;if(_tmp550->tag != 24)
goto _LL2D0;};_LL2CF: goto _LL2D1;_LL2D0: _tmp551=_tmp488.f1;{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*
_tmp552=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp551;if(_tmp552->tag != 
23)goto _LL2D2;};_LL2D1: goto _LL2D3;_LL2D2: _tmp553=_tmp488.f1;{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*
_tmp554=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp553;if(_tmp554->tag != 25)
goto _LL2D4;};_LL2D3: goto _LL2D5;_LL2D4: _tmp555=_tmp488.f2;{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*
_tmp556=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp555;if(_tmp556->tag != 25)
goto _LL2D6;};_LL2D5: goto _LL2D7;_LL2D6: _tmp557=_tmp488.f2;{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*
_tmp558=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp557;if(_tmp558->tag != 
23)goto _LL2D8;};_LL2D7: if(Cyc_Tcutil_unify_effect(t1,t2))return;{const char*
_tmp128F;Cyc_Tcutil_failure_reason=((_tmp128F="(effects don't unify)",
_tag_dyneither(_tmp128F,sizeof(char),22)));}goto _LL29D;_LL2D8:;_LL2D9: goto _LL29D;
_LL29D:;}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}int Cyc_Tcutil_star_cmp(int(*
cmp)(void*,void*),void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return
- 1;if(a1 != 0  && a2 == 0)return 1;return cmp((void*)_check_null(a1),(void*)
_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct
Cyc_Absyn_Tqual tq2){int _tmp5C1=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict
<< 2);int _tmp5C2=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp5C1,_tmp5C2);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(
void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{union Cyc_Absyn_Constraint*
_tmp5C3=x;union Cyc_Absyn_Constraint _tmp5C4;int _tmp5C5;union Cyc_Absyn_Constraint
_tmp5C6;void*_tmp5C7;union Cyc_Absyn_Constraint _tmp5C8;union Cyc_Absyn_Constraint*
_tmp5C9;_LL2F2: _tmp5C4=*_tmp5C3;if((_tmp5C4.No_constr).tag != 3)goto _LL2F4;
_tmp5C5=(int)(_tmp5C4.No_constr).val;_LL2F3: return - 1;_LL2F4: _tmp5C6=*_tmp5C3;if((
_tmp5C6.Eq_constr).tag != 1)goto _LL2F6;_tmp5C7=(void*)(_tmp5C6.Eq_constr).val;
_LL2F5: {union Cyc_Absyn_Constraint*_tmp5CA=y;union Cyc_Absyn_Constraint _tmp5CB;
int _tmp5CC;union Cyc_Absyn_Constraint _tmp5CD;void*_tmp5CE;union Cyc_Absyn_Constraint
_tmp5CF;union Cyc_Absyn_Constraint*_tmp5D0;_LL2F9: _tmp5CB=*_tmp5CA;if((_tmp5CB.No_constr).tag
!= 3)goto _LL2FB;_tmp5CC=(int)(_tmp5CB.No_constr).val;_LL2FA: return 1;_LL2FB:
_tmp5CD=*_tmp5CA;if((_tmp5CD.Eq_constr).tag != 1)goto _LL2FD;_tmp5CE=(void*)(
_tmp5CD.Eq_constr).val;_LL2FC: return cmp(_tmp5C7,_tmp5CE);_LL2FD: _tmp5CF=*_tmp5CA;
if((_tmp5CF.Forward_constr).tag != 2)goto _LL2F8;_tmp5D0=(union Cyc_Absyn_Constraint*)(
_tmp5CF.Forward_constr).val;_LL2FE: {const char*_tmp1292;void*_tmp1291;(_tmp1291=
0,Cyc_Tcutil_impos(((_tmp1292="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp1292,sizeof(char),40))),_tag_dyneither(_tmp1291,sizeof(void*),
0)));}_LL2F8:;}_LL2F6: _tmp5C8=*_tmp5C3;if((_tmp5C8.Forward_constr).tag != 2)goto
_LL2F1;_tmp5C9=(union Cyc_Absyn_Constraint*)(_tmp5C8.Forward_constr).val;_LL2F7: {
const char*_tmp1295;void*_tmp1294;(_tmp1294=0,Cyc_Tcutil_impos(((_tmp1295="unify_conref: forward after compress",
_tag_dyneither(_tmp1295,sizeof(char),37))),_tag_dyneither(_tmp1294,sizeof(void*),
0)));}_LL2F1:;};}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct
_tuple12*tqt2){struct _tuple12 _tmp5D6;struct Cyc_Absyn_Tqual _tmp5D7;void*_tmp5D8;
struct _tuple12*_tmp5D5=tqt1;_tmp5D6=*_tmp5D5;_tmp5D7=_tmp5D6.f1;_tmp5D8=_tmp5D6.f2;{
struct _tuple12 _tmp5DA;struct Cyc_Absyn_Tqual _tmp5DB;void*_tmp5DC;struct _tuple12*
_tmp5D9=tqt2;_tmp5DA=*_tmp5D9;_tmp5DB=_tmp5DA.f1;_tmp5DC=_tmp5DA.f2;{int _tmp5DD=
Cyc_Tcutil_tqual_cmp(_tmp5D7,_tmp5DB);if(_tmp5DD != 0)return _tmp5DD;return Cyc_Tcutil_typecmp(
_tmp5D8,_tmp5DC);};};}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp5DE=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp5DE != 0)return _tmp5DE;{int _tmp5DF=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp5DF != 0)return _tmp5DF;{int _tmp5E0=Cyc_Tcutil_typecmp(f1->type,f2->type);if(
_tmp5E0 != 0)return _tmp5E0;{int _tmp5E1=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,
f2->attributes);if(_tmp5E1 != 0)return _tmp5E1;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){int _tmp5E2=Cyc_Absyn_qvar_cmp(
e1->name,e2->name);if(_tmp5E2 != 0)return _tmp5E2;return((int(*)(int(*cmp)(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp5E3=t;int _tmp5EC;_LL300: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*
_tmp5E4=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp5E3;if(_tmp5E4->tag != 0)
goto _LL302;}_LL301: return 0;_LL302: {struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp5E5=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5E3;if(_tmp5E5->tag != 1)
goto _LL304;}_LL303: return 1;_LL304: {struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp5E6=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5E3;if(_tmp5E6->tag != 2)
goto _LL306;}_LL305: return 2;_LL306: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*
_tmp5E7=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5E3;if(_tmp5E7->tag
!= 3)goto _LL308;}_LL307: return 3;_LL308: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmp5E8=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5E3;if(_tmp5E8->tag
!= 4)goto _LL30A;}_LL309: return 4;_LL30A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp5E9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E3;if(_tmp5E9->tag
!= 5)goto _LL30C;}_LL30B: return 5;_LL30C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp5EA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5E3;if(_tmp5EA->tag != 6)
goto _LL30E;}_LL30D: return 6;_LL30E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp5EB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5E3;if(_tmp5EB->tag != 
7)goto _LL310;else{_tmp5EC=_tmp5EB->f1;if(_tmp5EC != 0)goto _LL310;}}_LL30F: return 7;
_LL310: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5ED=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)
_tmp5E3;if(_tmp5ED->tag != 7)goto _LL312;}_LL311: return 8;_LL312: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp5EE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E3;if(_tmp5EE->tag != 
8)goto _LL314;}_LL313: return 9;_LL314: {struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp5EF=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E3;if(_tmp5EF->tag != 9)
goto _LL316;}_LL315: return 10;_LL316: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmp5F0=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5E3;if(_tmp5F0->tag != 
10)goto _LL318;}_LL317: return 11;_LL318: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp5F1=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E3;if(_tmp5F1->tag != 
11)goto _LL31A;}_LL319: return 12;_LL31A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmp5F2=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E3;if(_tmp5F2->tag
!= 12)goto _LL31C;}_LL31B: return 14;_LL31C: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp5F3=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5E3;if(_tmp5F3->tag != 
13)goto _LL31E;}_LL31D: return 16;_LL31E: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*
_tmp5F4=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp5E3;if(_tmp5F4->tag
!= 14)goto _LL320;}_LL31F: return 17;_LL320: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*
_tmp5F5=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5E3;if(_tmp5F5->tag
!= 15)goto _LL322;}_LL321: return 18;_LL322: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*
_tmp5F6=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E3;if(_tmp5F6->tag
!= 17)goto _LL324;}_LL323: return 19;_LL324: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*
_tmp5F7=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp5E3;if(_tmp5F7->tag != 
21)goto _LL326;}_LL325: return 20;_LL326: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*
_tmp5F8=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp5E3;if(_tmp5F8->tag != 20)
goto _LL328;}_LL327: return 21;_LL328: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*
_tmp5F9=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp5E3;if(_tmp5F9->tag != 
23)goto _LL32A;}_LL329: return 22;_LL32A: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmp5FA=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp5E3;if(_tmp5FA->tag != 24)
goto _LL32C;}_LL32B: return 23;_LL32C: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*
_tmp5FB=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp5E3;if(_tmp5FB->tag != 25)
goto _LL32E;}_LL32D: return 24;_LL32E: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*
_tmp5FC=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp5E3;if(_tmp5FC->tag != 
22)goto _LL330;}_LL32F: return 25;_LL330: {struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmp5FD=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5E3;if(_tmp5FD->tag != 19)
goto _LL332;}_LL331: return 26;_LL332: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*
_tmp5FE=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp5E3;if(_tmp5FE->tag != 
16)goto _LL334;}_LL333: return 27;_LL334: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*
_tmp5FF=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5E3;if(_tmp5FF->tag
!= 18)goto _LL336;}_LL335: return 28;_LL336: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*
_tmp600=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E3;if(_tmp600->tag
!= 26)goto _LL2FF;}_LL337: return 29;_LL2FF:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int
_tmp601=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp601 != 0)return _tmp601;{struct _tuple0 _tmp1296;struct _tuple0 _tmp603=(
_tmp1296.f1=t2,((_tmp1296.f2=t1,_tmp1296)));void*_tmp604;void*_tmp606;void*
_tmp608;struct Cyc_Absyn_Tvar*_tmp60A;void*_tmp60B;struct Cyc_Absyn_Tvar*_tmp60D;
void*_tmp60E;struct Cyc_Absyn_AggrInfo _tmp610;union Cyc_Absyn_AggrInfoU _tmp611;
struct Cyc_List_List*_tmp612;void*_tmp613;struct Cyc_Absyn_AggrInfo _tmp615;union
Cyc_Absyn_AggrInfoU _tmp616;struct Cyc_List_List*_tmp617;void*_tmp618;struct
_tuple2*_tmp61A;void*_tmp61B;struct _tuple2*_tmp61D;void*_tmp61E;struct Cyc_List_List*
_tmp620;void*_tmp621;struct Cyc_List_List*_tmp623;void*_tmp624;struct Cyc_Absyn_DatatypeInfo
_tmp626;union Cyc_Absyn_DatatypeInfoU _tmp627;struct Cyc_Absyn_Datatypedecl**
_tmp628;struct Cyc_Absyn_Datatypedecl*_tmp629;struct Cyc_List_List*_tmp62A;void*
_tmp62B;struct Cyc_Absyn_DatatypeInfo _tmp62D;union Cyc_Absyn_DatatypeInfoU _tmp62E;
struct Cyc_Absyn_Datatypedecl**_tmp62F;struct Cyc_Absyn_Datatypedecl*_tmp630;
struct Cyc_List_List*_tmp631;void*_tmp632;struct Cyc_Absyn_DatatypeFieldInfo
_tmp634;union Cyc_Absyn_DatatypeFieldInfoU _tmp635;struct _tuple3 _tmp636;struct Cyc_Absyn_Datatypedecl*
_tmp637;struct Cyc_Absyn_Datatypefield*_tmp638;struct Cyc_List_List*_tmp639;void*
_tmp63A;struct Cyc_Absyn_DatatypeFieldInfo _tmp63C;union Cyc_Absyn_DatatypeFieldInfoU
_tmp63D;struct _tuple3 _tmp63E;struct Cyc_Absyn_Datatypedecl*_tmp63F;struct Cyc_Absyn_Datatypefield*
_tmp640;struct Cyc_List_List*_tmp641;void*_tmp642;struct Cyc_Absyn_PtrInfo _tmp644;
void*_tmp645;struct Cyc_Absyn_Tqual _tmp646;struct Cyc_Absyn_PtrAtts _tmp647;void*
_tmp648;union Cyc_Absyn_Constraint*_tmp649;union Cyc_Absyn_Constraint*_tmp64A;
union Cyc_Absyn_Constraint*_tmp64B;void*_tmp64C;struct Cyc_Absyn_PtrInfo _tmp64E;
void*_tmp64F;struct Cyc_Absyn_Tqual _tmp650;struct Cyc_Absyn_PtrAtts _tmp651;void*
_tmp652;union Cyc_Absyn_Constraint*_tmp653;union Cyc_Absyn_Constraint*_tmp654;
union Cyc_Absyn_Constraint*_tmp655;void*_tmp656;enum Cyc_Absyn_Sign _tmp658;enum 
Cyc_Absyn_Size_of _tmp659;void*_tmp65A;enum Cyc_Absyn_Sign _tmp65C;enum Cyc_Absyn_Size_of
_tmp65D;void*_tmp65E;int _tmp660;void*_tmp661;int _tmp663;void*_tmp664;struct Cyc_Absyn_ArrayInfo
_tmp666;void*_tmp667;struct Cyc_Absyn_Tqual _tmp668;struct Cyc_Absyn_Exp*_tmp669;
union Cyc_Absyn_Constraint*_tmp66A;void*_tmp66B;struct Cyc_Absyn_ArrayInfo _tmp66D;
void*_tmp66E;struct Cyc_Absyn_Tqual _tmp66F;struct Cyc_Absyn_Exp*_tmp670;union Cyc_Absyn_Constraint*
_tmp671;void*_tmp672;struct Cyc_Absyn_FnInfo _tmp674;struct Cyc_List_List*_tmp675;
void*_tmp676;struct Cyc_Absyn_Tqual _tmp677;void*_tmp678;struct Cyc_List_List*
_tmp679;int _tmp67A;struct Cyc_Absyn_VarargInfo*_tmp67B;struct Cyc_List_List*
_tmp67C;struct Cyc_List_List*_tmp67D;void*_tmp67E;struct Cyc_Absyn_FnInfo _tmp680;
struct Cyc_List_List*_tmp681;void*_tmp682;struct Cyc_Absyn_Tqual _tmp683;void*
_tmp684;struct Cyc_List_List*_tmp685;int _tmp686;struct Cyc_Absyn_VarargInfo*
_tmp687;struct Cyc_List_List*_tmp688;struct Cyc_List_List*_tmp689;void*_tmp68A;
struct Cyc_List_List*_tmp68C;void*_tmp68D;struct Cyc_List_List*_tmp68F;void*
_tmp690;enum Cyc_Absyn_AggrKind _tmp692;struct Cyc_List_List*_tmp693;void*_tmp694;
enum Cyc_Absyn_AggrKind _tmp696;struct Cyc_List_List*_tmp697;void*_tmp698;void*
_tmp69A;void*_tmp69B;void*_tmp69D;void*_tmp69E;void*_tmp6A0;void*_tmp6A1;void*
_tmp6A2;void*_tmp6A4;void*_tmp6A5;void*_tmp6A6;void*_tmp6A8;void*_tmp6A9;void*
_tmp6AB;void*_tmp6AC;struct Cyc_Absyn_Exp*_tmp6AE;void*_tmp6AF;struct Cyc_Absyn_Exp*
_tmp6B1;void*_tmp6B2;void*_tmp6B4;void*_tmp6B6;void*_tmp6B8;void*_tmp6BA;void*
_tmp6BC;_LL339: _tmp604=_tmp603.f1;{struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp605=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp604;if(_tmp605->tag != 1)
goto _LL33B;};_tmp606=_tmp603.f2;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp607=(
struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp606;if(_tmp607->tag != 1)goto _LL33B;};
_LL33A: {const char*_tmp1299;void*_tmp1298;(_tmp1298=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1299="typecmp: can only compare closed types",
_tag_dyneither(_tmp1299,sizeof(char),39))),_tag_dyneither(_tmp1298,sizeof(void*),
0)));}_LL33B: _tmp608=_tmp603.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp609=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp608;if(_tmp609->tag != 2)
goto _LL33D;else{_tmp60A=_tmp609->f1;}};_tmp60B=_tmp603.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp60C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp60B;if(_tmp60C->tag != 2)
goto _LL33D;else{_tmp60D=_tmp60C->f1;}};_LL33C: return Cyc_Core_intcmp(_tmp60D->identity,
_tmp60A->identity);_LL33D: _tmp60E=_tmp603.f1;{struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp60F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp60E;if(_tmp60F->tag != 
11)goto _LL33F;else{_tmp610=_tmp60F->f1;_tmp611=_tmp610.aggr_info;_tmp612=_tmp610.targs;}};
_tmp613=_tmp603.f2;{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp614=(struct
Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp613;if(_tmp614->tag != 11)goto _LL33F;
else{_tmp615=_tmp614->f1;_tmp616=_tmp615.aggr_info;_tmp617=_tmp615.targs;}};
_LL33E: {struct _tuple2*_tmp6C1;struct _tuple13 _tmp6C0=Cyc_Absyn_aggr_kinded_name(
_tmp611);_tmp6C1=_tmp6C0.f2;{struct _tuple2*_tmp6C3;struct _tuple13 _tmp6C2=Cyc_Absyn_aggr_kinded_name(
_tmp616);_tmp6C3=_tmp6C2.f2;{int _tmp6C4=Cyc_Absyn_qvar_cmp(_tmp6C1,_tmp6C3);if(
_tmp6C4 != 0)return _tmp6C4;else{return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp612,_tmp617);}};};}
_LL33F: _tmp618=_tmp603.f1;{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp619=(
struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp618;if(_tmp619->tag != 13)goto
_LL341;else{_tmp61A=_tmp619->f1;}};_tmp61B=_tmp603.f2;{struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp61C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp61B;if(_tmp61C->tag != 
13)goto _LL341;else{_tmp61D=_tmp61C->f1;}};_LL340: return Cyc_Absyn_qvar_cmp(
_tmp61A,_tmp61D);_LL341: _tmp61E=_tmp603.f1;{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*
_tmp61F=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp61E;if(_tmp61F->tag
!= 14)goto _LL343;else{_tmp620=_tmp61F->f1;}};_tmp621=_tmp603.f2;{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*
_tmp622=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp621;if(_tmp622->tag
!= 14)goto _LL343;else{_tmp623=_tmp622->f1;}};_LL342: return((int(*)(int(*cmp)(
struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp620,
_tmp623);_LL343: _tmp624=_tmp603.f1;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*
_tmp625=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp624;if(_tmp625->tag
!= 3)goto _LL345;else{_tmp626=_tmp625->f1;_tmp627=_tmp626.datatype_info;if((
_tmp627.KnownDatatype).tag != 2)goto _LL345;_tmp628=(struct Cyc_Absyn_Datatypedecl**)(
_tmp627.KnownDatatype).val;_tmp629=*_tmp628;_tmp62A=_tmp626.targs;}};_tmp62B=
_tmp603.f2;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp62C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)
_tmp62B;if(_tmp62C->tag != 3)goto _LL345;else{_tmp62D=_tmp62C->f1;_tmp62E=_tmp62D.datatype_info;
if((_tmp62E.KnownDatatype).tag != 2)goto _LL345;_tmp62F=(struct Cyc_Absyn_Datatypedecl**)(
_tmp62E.KnownDatatype).val;_tmp630=*_tmp62F;_tmp631=_tmp62D.targs;}};_LL344: if(
_tmp630 == _tmp629)return 0;{int _tmp6C5=Cyc_Absyn_qvar_cmp(_tmp630->name,_tmp629->name);
if(_tmp6C5 != 0)return _tmp6C5;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp631,_tmp62A);};
_LL345: _tmp632=_tmp603.f1;{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmp633=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp632;if(_tmp633->tag
!= 4)goto _LL347;else{_tmp634=_tmp633->f1;_tmp635=_tmp634.field_info;if((_tmp635.KnownDatatypefield).tag
!= 2)goto _LL347;_tmp636=(struct _tuple3)(_tmp635.KnownDatatypefield).val;_tmp637=
_tmp636.f1;_tmp638=_tmp636.f2;_tmp639=_tmp634.targs;}};_tmp63A=_tmp603.f2;{
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp63B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)
_tmp63A;if(_tmp63B->tag != 4)goto _LL347;else{_tmp63C=_tmp63B->f1;_tmp63D=_tmp63C.field_info;
if((_tmp63D.KnownDatatypefield).tag != 2)goto _LL347;_tmp63E=(struct _tuple3)(
_tmp63D.KnownDatatypefield).val;_tmp63F=_tmp63E.f1;_tmp640=_tmp63E.f2;_tmp641=
_tmp63C.targs;}};_LL346: if(_tmp63F == _tmp637)return 0;{int _tmp6C6=Cyc_Absyn_qvar_cmp(
_tmp637->name,_tmp63F->name);if(_tmp6C6 != 0)return _tmp6C6;{int _tmp6C7=Cyc_Absyn_qvar_cmp(
_tmp638->name,_tmp640->name);if(_tmp6C7 != 0)return _tmp6C7;return((int(*)(int(*
cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_typecmp,_tmp641,_tmp639);};};_LL347: _tmp642=_tmp603.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp643=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp642;if(_tmp643->tag
!= 5)goto _LL349;else{_tmp644=_tmp643->f1;_tmp645=_tmp644.elt_typ;_tmp646=_tmp644.elt_tq;
_tmp647=_tmp644.ptr_atts;_tmp648=_tmp647.rgn;_tmp649=_tmp647.nullable;_tmp64A=
_tmp647.bounds;_tmp64B=_tmp647.zero_term;}};_tmp64C=_tmp603.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp64D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp64C;if(_tmp64D->tag
!= 5)goto _LL349;else{_tmp64E=_tmp64D->f1;_tmp64F=_tmp64E.elt_typ;_tmp650=_tmp64E.elt_tq;
_tmp651=_tmp64E.ptr_atts;_tmp652=_tmp651.rgn;_tmp653=_tmp651.nullable;_tmp654=
_tmp651.bounds;_tmp655=_tmp651.zero_term;}};_LL348: {int _tmp6C8=Cyc_Tcutil_typecmp(
_tmp64F,_tmp645);if(_tmp6C8 != 0)return _tmp6C8;{int _tmp6C9=Cyc_Tcutil_typecmp(
_tmp652,_tmp648);if(_tmp6C9 != 0)return _tmp6C9;{int _tmp6CA=Cyc_Tcutil_tqual_cmp(
_tmp650,_tmp646);if(_tmp6CA != 0)return _tmp6CA;{int _tmp6CB=((int(*)(int(*cmp)(
void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Tcutil_boundscmp,_tmp654,_tmp64A);if(_tmp6CB != 0)return _tmp6CB;{int _tmp6CC=((
int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp655,_tmp64B);if(_tmp6CC != 0)
return _tmp6CC;{void*_tmp6CD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp654);_LL36C: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp6CE=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6CD;if(_tmp6CE->tag
!= 0)goto _LL36E;}_LL36D: return 0;_LL36E:;_LL36F: goto _LL36B;_LL36B:;}return((int(*)(
int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp653,_tmp649);};};};};}_LL349: _tmp656=_tmp603.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp657=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp656;if(_tmp657->tag != 6)
goto _LL34B;else{_tmp658=_tmp657->f1;_tmp659=_tmp657->f2;}};_tmp65A=_tmp603.f2;{
struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp65B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp65A;if(_tmp65B->tag != 6)goto _LL34B;else{_tmp65C=_tmp65B->f1;_tmp65D=_tmp65B->f2;}};
_LL34A: if(_tmp65C != _tmp658)return Cyc_Core_intcmp((int)((unsigned int)_tmp65C),(
int)((unsigned int)_tmp658));if(_tmp65D != _tmp659)return Cyc_Core_intcmp((int)((
unsigned int)_tmp65D),(int)((unsigned int)_tmp659));return 0;_LL34B: _tmp65E=
_tmp603.f1;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp65F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)
_tmp65E;if(_tmp65F->tag != 7)goto _LL34D;else{_tmp660=_tmp65F->f1;}};_tmp661=
_tmp603.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp662=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)
_tmp661;if(_tmp662->tag != 7)goto _LL34D;else{_tmp663=_tmp662->f1;}};_LL34C: if(
_tmp660 == _tmp663)return 0;else{if(_tmp663 == 0)return - 1;else{if(_tmp663 == 1  && 
_tmp660 == 0)return - 1;else{return 1;}}}_LL34D: _tmp664=_tmp603.f1;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp665=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp664;if(_tmp665->tag != 
8)goto _LL34F;else{_tmp666=_tmp665->f1;_tmp667=_tmp666.elt_type;_tmp668=_tmp666.tq;
_tmp669=_tmp666.num_elts;_tmp66A=_tmp666.zero_term;}};_tmp66B=_tmp603.f2;{struct
Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp66C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)
_tmp66B;if(_tmp66C->tag != 8)goto _LL34F;else{_tmp66D=_tmp66C->f1;_tmp66E=_tmp66D.elt_type;
_tmp66F=_tmp66D.tq;_tmp670=_tmp66D.num_elts;_tmp671=_tmp66D.zero_term;}};_LL34E: {
int _tmp6CF=Cyc_Tcutil_tqual_cmp(_tmp66F,_tmp668);if(_tmp6CF != 0)return _tmp6CF;{
int _tmp6D0=Cyc_Tcutil_typecmp(_tmp66E,_tmp667);if(_tmp6D0 != 0)return _tmp6D0;{int
_tmp6D1=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp66A,_tmp671);if(_tmp6D1 != 0)
return _tmp6D1;if(_tmp669 == _tmp670)return 0;if(_tmp669 == 0  || _tmp670 == 0){const
char*_tmp129C;void*_tmp129B;(_tmp129B=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp129C="missing expression in array index",
_tag_dyneither(_tmp129C,sizeof(char),34))),_tag_dyneither(_tmp129B,sizeof(void*),
0)));}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct
Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp669,_tmp670);};};}_LL34F: _tmp672=_tmp603.f1;{struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp673=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp672;if(_tmp673->tag != 9)
goto _LL351;else{_tmp674=_tmp673->f1;_tmp675=_tmp674.tvars;_tmp676=_tmp674.effect;
_tmp677=_tmp674.ret_tqual;_tmp678=_tmp674.ret_typ;_tmp679=_tmp674.args;_tmp67A=
_tmp674.c_varargs;_tmp67B=_tmp674.cyc_varargs;_tmp67C=_tmp674.rgn_po;_tmp67D=
_tmp674.attributes;}};_tmp67E=_tmp603.f2;{struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp67F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp67E;if(_tmp67F->tag != 9)
goto _LL351;else{_tmp680=_tmp67F->f1;_tmp681=_tmp680.tvars;_tmp682=_tmp680.effect;
_tmp683=_tmp680.ret_tqual;_tmp684=_tmp680.ret_typ;_tmp685=_tmp680.args;_tmp686=
_tmp680.c_varargs;_tmp687=_tmp680.cyc_varargs;_tmp688=_tmp680.rgn_po;_tmp689=
_tmp680.attributes;}};_LL350: {const char*_tmp129F;void*_tmp129E;(_tmp129E=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp129F="typecmp: function types not handled",_tag_dyneither(_tmp129F,sizeof(
char),36))),_tag_dyneither(_tmp129E,sizeof(void*),0)));}_LL351: _tmp68A=_tmp603.f1;{
struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp68B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)
_tmp68A;if(_tmp68B->tag != 10)goto _LL353;else{_tmp68C=_tmp68B->f1;}};_tmp68D=
_tmp603.f2;{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp68E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)
_tmp68D;if(_tmp68E->tag != 10)goto _LL353;else{_tmp68F=_tmp68E->f1;}};_LL352:
return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp68F,
_tmp68C);_LL353: _tmp690=_tmp603.f1;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmp691=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp690;if(_tmp691->tag
!= 12)goto _LL355;else{_tmp692=_tmp691->f1;_tmp693=_tmp691->f2;}};_tmp694=_tmp603.f2;{
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp695=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)
_tmp694;if(_tmp695->tag != 12)goto _LL355;else{_tmp696=_tmp695->f1;_tmp697=_tmp695->f2;}};
_LL354: if(_tmp696 != _tmp692){if(_tmp696 == Cyc_Absyn_StructA)return - 1;else{return
1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp697,_tmp693);_LL355: _tmp698=_tmp603.f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*
_tmp699=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp698;if(_tmp699->tag
!= 15)goto _LL357;else{_tmp69A=(void*)_tmp699->f1;}};_tmp69B=_tmp603.f2;{struct
Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp69C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)
_tmp69B;if(_tmp69C->tag != 15)goto _LL357;else{_tmp69D=(void*)_tmp69C->f1;}};
_LL356: return Cyc_Tcutil_typecmp(_tmp69A,_tmp69D);_LL357: _tmp69E=_tmp603.f1;{
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp69F=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)
_tmp69E;if(_tmp69F->tag != 16)goto _LL359;else{_tmp6A0=(void*)_tmp69F->f1;_tmp6A1=(
void*)_tmp69F->f2;}};_tmp6A2=_tmp603.f2;{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*
_tmp6A3=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp6A2;if(_tmp6A3->tag != 
16)goto _LL359;else{_tmp6A4=(void*)_tmp6A3->f1;_tmp6A5=(void*)_tmp6A3->f2;}};
_LL358: {int _tmp6D6=Cyc_Tcutil_typecmp(_tmp6A0,_tmp6A4);if(_tmp6D6 != 0)return
_tmp6D6;else{return Cyc_Tcutil_typecmp(_tmp6A1,_tmp6A5);}}_LL359: _tmp6A6=_tmp603.f1;{
struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6A7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)
_tmp6A6;if(_tmp6A7->tag != 19)goto _LL35B;else{_tmp6A8=(void*)_tmp6A7->f1;}};
_tmp6A9=_tmp603.f2;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6AA=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)
_tmp6A9;if(_tmp6AA->tag != 19)goto _LL35B;else{_tmp6AB=(void*)_tmp6AA->f1;}};
_LL35A: return Cyc_Tcutil_typecmp(_tmp6A8,_tmp6AB);_LL35B: _tmp6AC=_tmp603.f1;{
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6AD=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)
_tmp6AC;if(_tmp6AD->tag != 18)goto _LL35D;else{_tmp6AE=_tmp6AD->f1;}};_tmp6AF=
_tmp603.f2;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6B0=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)
_tmp6AF;if(_tmp6B0->tag != 18)goto _LL35D;else{_tmp6B1=_tmp6B0->f1;}};_LL35C:
return Cyc_Evexp_const_exp_cmp(_tmp6AE,_tmp6B1);_LL35D: _tmp6B2=_tmp603.f1;{struct
Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6B3=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)
_tmp6B2;if(_tmp6B3->tag != 24)goto _LL35F;};_LL35E: goto _LL360;_LL35F: _tmp6B4=
_tmp603.f2;{struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6B5=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)
_tmp6B4;if(_tmp6B5->tag != 24)goto _LL361;};_LL360: goto _LL362;_LL361: _tmp6B6=
_tmp603.f1;{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp6B7=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)
_tmp6B6;if(_tmp6B7->tag != 23)goto _LL363;};_LL362: goto _LL364;_LL363: _tmp6B8=
_tmp603.f1;{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6B9=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)
_tmp6B8;if(_tmp6B9->tag != 25)goto _LL365;};_LL364: goto _LL366;_LL365: _tmp6BA=
_tmp603.f2;{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6BB=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)
_tmp6BA;if(_tmp6BB->tag != 25)goto _LL367;};_LL366: goto _LL368;_LL367: _tmp6BC=
_tmp603.f2;{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp6BD=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)
_tmp6BC;if(_tmp6BD->tag != 23)goto _LL369;};_LL368: {const char*_tmp12A2;void*
_tmp12A1;(_tmp12A1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12A2="typecmp: effects not handled",_tag_dyneither(
_tmp12A2,sizeof(char),29))),_tag_dyneither(_tmp12A1,sizeof(void*),0)));}_LL369:;
_LL36A: {const char*_tmp12A5;void*_tmp12A4;(_tmp12A4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12A5="Unmatched case in typecmp",
_tag_dyneither(_tmp12A5,sizeof(char),26))),_tag_dyneither(_tmp12A4,sizeof(void*),
0)));}_LL338:;};};}int Cyc_Tcutil_is_arithmetic_type(void*t){void*_tmp6DB=Cyc_Tcutil_compress(
t);_LL371: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6DC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp6DB;if(_tmp6DC->tag != 6)goto _LL373;}_LL372: goto _LL374;_LL373: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp6DD=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6DB;if(_tmp6DD->tag != 
7)goto _LL375;}_LL374: goto _LL376;_LL375: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp6DE=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp6DB;if(_tmp6DE->tag != 
13)goto _LL377;}_LL376: goto _LL378;_LL377: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*
_tmp6DF=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp6DB;if(_tmp6DF->tag
!= 14)goto _LL379;}_LL378: return 1;_LL379:;_LL37A: return 0;_LL370:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple0 _tmp12A6;struct _tuple0 _tmp6E1=(_tmp12A6.f1=t1,((_tmp12A6.f2=t2,_tmp12A6)));
void*_tmp6E2;int _tmp6E4;void*_tmp6E5;int _tmp6E7;void*_tmp6E8;void*_tmp6EA;void*
_tmp6EC;void*_tmp6EE;void*_tmp6F0;enum Cyc_Absyn_Size_of _tmp6F2;void*_tmp6F3;
enum Cyc_Absyn_Size_of _tmp6F5;void*_tmp6F6;enum Cyc_Absyn_Size_of _tmp6F8;void*
_tmp6F9;enum Cyc_Absyn_Size_of _tmp6FB;void*_tmp6FC;enum Cyc_Absyn_Size_of _tmp6FE;
void*_tmp6FF;enum Cyc_Absyn_Size_of _tmp701;void*_tmp702;enum Cyc_Absyn_Size_of
_tmp704;void*_tmp705;enum Cyc_Absyn_Size_of _tmp707;void*_tmp708;int _tmp70A;void*
_tmp70B;enum Cyc_Absyn_Size_of _tmp70D;void*_tmp70E;int _tmp710;void*_tmp711;enum 
Cyc_Absyn_Size_of _tmp713;void*_tmp714;enum Cyc_Absyn_Size_of _tmp716;void*_tmp717;
enum Cyc_Absyn_Size_of _tmp719;void*_tmp71A;enum Cyc_Absyn_Size_of _tmp71C;void*
_tmp71D;void*_tmp71F;enum Cyc_Absyn_Size_of _tmp721;void*_tmp722;enum Cyc_Absyn_Size_of
_tmp724;void*_tmp725;enum Cyc_Absyn_Size_of _tmp727;void*_tmp728;enum Cyc_Absyn_Size_of
_tmp72A;void*_tmp72B;enum Cyc_Absyn_Size_of _tmp72D;void*_tmp72E;enum Cyc_Absyn_Size_of
_tmp730;void*_tmp731;enum Cyc_Absyn_Size_of _tmp733;void*_tmp734;void*_tmp736;
enum Cyc_Absyn_Size_of _tmp738;_LL37C: _tmp6E2=_tmp6E1.f1;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp6E3=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6E2;if(_tmp6E3->tag != 
7)goto _LL37E;else{_tmp6E4=_tmp6E3->f1;}};_tmp6E5=_tmp6E1.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp6E6=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6E5;if(_tmp6E6->tag != 
7)goto _LL37E;else{_tmp6E7=_tmp6E6->f1;}};_LL37D: return _tmp6E7 == 0  && _tmp6E4 != 0
 || (_tmp6E7 == 1  && _tmp6E4 != 0) && _tmp6E4 != 1;_LL37E: _tmp6E8=_tmp6E1.f1;{
struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6E9=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)
_tmp6E8;if(_tmp6E9->tag != 7)goto _LL380;};_tmp6EA=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp6EB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6EA;if(_tmp6EB->tag != 6)
goto _LL380;};_LL37F: goto _LL381;_LL380: _tmp6EC=_tmp6E1.f1;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp6ED=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6EC;if(_tmp6ED->tag != 
7)goto _LL382;};_tmp6EE=_tmp6E1.f2;{struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmp6EF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp6EE;if(_tmp6EF->tag != 19)
goto _LL382;};_LL381: return 1;_LL382: _tmp6F0=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp6F1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6F0;if(_tmp6F1->tag != 6)
goto _LL384;else{_tmp6F2=_tmp6F1->f2;if(_tmp6F2 != Cyc_Absyn_LongLong_sz)goto
_LL384;}};_tmp6F3=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6F4=(
struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6F3;if(_tmp6F4->tag != 6)goto
_LL384;else{_tmp6F5=_tmp6F4->f2;if(_tmp6F5 != Cyc_Absyn_LongLong_sz)goto _LL384;}};
_LL383: return 0;_LL384: _tmp6F6=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp6F7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6F6;if(_tmp6F7->tag != 6)
goto _LL386;else{_tmp6F8=_tmp6F7->f2;if(_tmp6F8 != Cyc_Absyn_LongLong_sz)goto
_LL386;}};_LL385: return 1;_LL386: _tmp6F9=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp6FA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6F9;if(_tmp6FA->tag != 6)
goto _LL388;else{_tmp6FB=_tmp6FA->f2;if(_tmp6FB != Cyc_Absyn_Long_sz)goto _LL388;}};
_tmp6FC=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6FD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp6FC;if(_tmp6FD->tag != 6)goto _LL388;else{_tmp6FE=_tmp6FD->f2;if(_tmp6FE != Cyc_Absyn_Int_sz)
goto _LL388;}};_LL387: goto _LL389;_LL388: _tmp6FF=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp700=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6FF;if(_tmp700->tag != 6)
goto _LL38A;else{_tmp701=_tmp700->f2;if(_tmp701 != Cyc_Absyn_Int_sz)goto _LL38A;}};
_tmp702=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp703=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp702;if(_tmp703->tag != 6)goto _LL38A;else{_tmp704=_tmp703->f2;if(_tmp704 != Cyc_Absyn_Long_sz)
goto _LL38A;}};_LL389: return 0;_LL38A: _tmp705=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp706=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp705;if(_tmp706->tag != 6)
goto _LL38C;else{_tmp707=_tmp706->f2;if(_tmp707 != Cyc_Absyn_Long_sz)goto _LL38C;}};
_tmp708=_tmp6E1.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp709=(struct
Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp708;if(_tmp709->tag != 7)goto _LL38C;
else{_tmp70A=_tmp709->f1;if(_tmp70A != 0)goto _LL38C;}};_LL38B: goto _LL38D;_LL38C:
_tmp70B=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp70C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp70B;if(_tmp70C->tag != 6)goto _LL38E;else{_tmp70D=_tmp70C->f2;if(_tmp70D != Cyc_Absyn_Int_sz)
goto _LL38E;}};_tmp70E=_tmp6E1.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp70F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp70E;if(_tmp70F->tag != 
7)goto _LL38E;else{_tmp710=_tmp70F->f1;if(_tmp710 != 0)goto _LL38E;}};_LL38D: goto
_LL38F;_LL38E: _tmp711=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp712=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp711;if(_tmp712->tag != 6)
goto _LL390;else{_tmp713=_tmp712->f2;if(_tmp713 != Cyc_Absyn_Long_sz)goto _LL390;}};
_tmp714=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp715=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp714;if(_tmp715->tag != 6)goto _LL390;else{_tmp716=_tmp715->f2;if(_tmp716 != Cyc_Absyn_Short_sz)
goto _LL390;}};_LL38F: goto _LL391;_LL390: _tmp717=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp718=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp717;if(_tmp718->tag != 6)
goto _LL392;else{_tmp719=_tmp718->f2;if(_tmp719 != Cyc_Absyn_Int_sz)goto _LL392;}};
_tmp71A=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp71B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp71A;if(_tmp71B->tag != 6)goto _LL392;else{_tmp71C=_tmp71B->f2;if(_tmp71C != Cyc_Absyn_Short_sz)
goto _LL392;}};_LL391: goto _LL393;_LL392: _tmp71D=_tmp6E1.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmp71E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp71D;if(_tmp71E->tag != 19)
goto _LL394;};_tmp71F=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp720=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71F;if(_tmp720->tag != 6)
goto _LL394;else{_tmp721=_tmp720->f2;if(_tmp721 != Cyc_Absyn_Short_sz)goto _LL394;}};
_LL393: goto _LL395;_LL394: _tmp722=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp723=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp722;if(_tmp723->tag != 6)
goto _LL396;else{_tmp724=_tmp723->f2;if(_tmp724 != Cyc_Absyn_Long_sz)goto _LL396;}};
_tmp725=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp726=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp725;if(_tmp726->tag != 6)goto _LL396;else{_tmp727=_tmp726->f2;if(_tmp727 != Cyc_Absyn_Char_sz)
goto _LL396;}};_LL395: goto _LL397;_LL396: _tmp728=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp729=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp728;if(_tmp729->tag != 6)
goto _LL398;else{_tmp72A=_tmp729->f2;if(_tmp72A != Cyc_Absyn_Int_sz)goto _LL398;}};
_tmp72B=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp72C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp72B;if(_tmp72C->tag != 6)goto _LL398;else{_tmp72D=_tmp72C->f2;if(_tmp72D != Cyc_Absyn_Char_sz)
goto _LL398;}};_LL397: goto _LL399;_LL398: _tmp72E=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp72F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp72E;if(_tmp72F->tag != 6)
goto _LL39A;else{_tmp730=_tmp72F->f2;if(_tmp730 != Cyc_Absyn_Short_sz)goto _LL39A;}};
_tmp731=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp732=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp731;if(_tmp732->tag != 6)goto _LL39A;else{_tmp733=_tmp732->f2;if(_tmp733 != Cyc_Absyn_Char_sz)
goto _LL39A;}};_LL399: goto _LL39B;_LL39A: _tmp734=_tmp6E1.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmp735=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp734;if(_tmp735->tag != 19)
goto _LL39C;};_tmp736=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp737=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp736;if(_tmp737->tag != 6)
goto _LL39C;else{_tmp738=_tmp737->f2;if(_tmp738 != Cyc_Absyn_Char_sz)goto _LL39C;}};
_LL39B: return 1;_LL39C:;_LL39D: return 0;_LL37B:;};}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle
_tmp739=_new_region("r");struct _RegionHandle*r=& _tmp739;_push_region(r);{struct
Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(
t1,(void*)max_arith_type->v)){struct Cyc_Core_Opt*_tmp12A7;max_arith_type=((
_tmp12A7=_region_malloc(r,sizeof(*_tmp12A7)),((_tmp12A7->v=t1,_tmp12A7))));}}}}
if(max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){int
_tmp73B=0;_npop_handler(0);return _tmp73B;}}}{struct Cyc_List_List*el=es;for(0;el
!= 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){{
const char*_tmp12AC;void*_tmp12AB[2];struct Cyc_String_pa_PrintArg_struct _tmp12AA;
struct Cyc_String_pa_PrintArg_struct _tmp12A9;(_tmp12A9.tag=0,((_tmp12A9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))),((_tmp12AA.tag=0,((_tmp12AA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp12AB[0]=& _tmp12AA,((_tmp12AB[1]=& _tmp12A9,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmp12AC="type mismatch: expecting %s but found %s",
_tag_dyneither(_tmp12AC,sizeof(char),41))),_tag_dyneither(_tmp12AB,sizeof(void*),
2)))))))))))));}{int _tmp740=0;_npop_handler(0);return _tmp740;};}}}{int _tmp741=1;
_npop_handler(0);return _tmp741;};;_pop_region(r);}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp742=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL39F: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp743=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp742;if(_tmp743->tag != 5)goto _LL3A1;}_LL3A0: Cyc_Tcutil_unchecked_cast(te,e,
Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL39E;_LL3A1:;_LL3A2: return 0;
_LL39E:;}return 1;}int Cyc_Tcutil_is_integral_type(void*t){void*_tmp744=Cyc_Tcutil_compress(
t);_LL3A4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp745=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp744;if(_tmp745->tag != 6)goto _LL3A6;}_LL3A5: goto _LL3A7;_LL3A6: {struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmp746=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp744;if(_tmp746->tag != 19)
goto _LL3A8;}_LL3A7: goto _LL3A9;_LL3A8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp747=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp744;if(_tmp747->tag != 
13)goto _LL3AA;}_LL3A9: goto _LL3AB;_LL3AA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*
_tmp748=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp744;if(_tmp748->tag
!= 14)goto _LL3AC;}_LL3AB: return 1;_LL3AC:;_LL3AD: return 0;_LL3A3:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)
_check_null(e->topt),Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)_check_null(e->topt))){if(Cyc_Tcutil_will_lose_precision((void*)
_check_null(e->topt),Cyc_Absyn_uint_typ)){const char*_tmp12AF;void*_tmp12AE;(
_tmp12AE=0,Cyc_Tcutil_warn(e->loc,((_tmp12AF="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp12AF,sizeof(char),44))),_tag_dyneither(_tmp12AE,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){if(Cyc_Tcutil_will_lose_precision((
void*)_check_null(e->topt),Cyc_Absyn_sint_typ)){const char*_tmp12B2;void*_tmp12B1;(
_tmp12B1=0,Cyc_Tcutil_warn(e->loc,((_tmp12B2="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp12B2,sizeof(char),44))),_tag_dyneither(_tmp12B1,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct
Cyc_List_List*assume,void*t1,void*t2);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,unsigned int loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);{struct _tuple0 _tmp12B3;struct _tuple0 _tmp74E=(_tmp12B3.f1=t1,((_tmp12B3.f2=t2,
_tmp12B3)));void*_tmp74F;struct Cyc_Absyn_PtrInfo _tmp751;void*_tmp752;struct Cyc_Absyn_PtrInfo
_tmp754;void*_tmp755;struct Cyc_Absyn_ArrayInfo _tmp757;void*_tmp758;struct Cyc_Absyn_Tqual
_tmp759;struct Cyc_Absyn_Exp*_tmp75A;union Cyc_Absyn_Constraint*_tmp75B;void*
_tmp75C;struct Cyc_Absyn_ArrayInfo _tmp75E;void*_tmp75F;struct Cyc_Absyn_Tqual
_tmp760;struct Cyc_Absyn_Exp*_tmp761;union Cyc_Absyn_Constraint*_tmp762;void*
_tmp763;void*_tmp765;_LL3AF: _tmp74F=_tmp74E.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp750=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74F;if(_tmp750->tag
!= 5)goto _LL3B1;else{_tmp751=_tmp750->f1;}};_tmp752=_tmp74E.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp753=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp752;if(_tmp753->tag
!= 5)goto _LL3B1;else{_tmp754=_tmp753->f1;}};_LL3B0: {int okay=1;if(!((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp751.ptr_atts).nullable,(_tmp754.ptr_atts).nullable))okay=!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp751.ptr_atts).nullable);
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp751.ptr_atts).bounds,(
_tmp754.ptr_atts).bounds)){struct _tuple0 _tmp12B4;struct _tuple0 _tmp768=(_tmp12B4.f1=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(
_tmp751.ptr_atts).bounds),((_tmp12B4.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp754.ptr_atts).bounds),
_tmp12B4)));void*_tmp769;void*_tmp76B;void*_tmp76D;void*_tmp76F;void*_tmp771;
struct Cyc_Absyn_Exp*_tmp773;void*_tmp774;struct Cyc_Absyn_Exp*_tmp776;void*
_tmp777;void*_tmp779;struct Cyc_Absyn_Exp*_tmp77B;_LL3B8: _tmp769=_tmp768.f1;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp76A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)
_tmp769;if(_tmp76A->tag != 1)goto _LL3BA;};_tmp76B=_tmp768.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp76C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp76B;if(_tmp76C->tag
!= 0)goto _LL3BA;};_LL3B9: goto _LL3BB;_LL3BA: _tmp76D=_tmp768.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp76E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp76D;if(_tmp76E->tag
!= 0)goto _LL3BC;};_tmp76F=_tmp768.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp770=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp76F;if(_tmp770->tag
!= 0)goto _LL3BC;};_LL3BB: okay=1;goto _LL3B7;_LL3BC: _tmp771=_tmp768.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp772=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp771;if(_tmp772->tag != 
1)goto _LL3BE;else{_tmp773=_tmp772->f1;}};_tmp774=_tmp768.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp775=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp774;if(_tmp775->tag != 
1)goto _LL3BE;else{_tmp776=_tmp775->f1;}};_LL3BD: okay=okay  && Cyc_Evexp_lte_const_exp(
_tmp776,_tmp773);if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp754.ptr_atts).zero_term)){const char*_tmp12B7;void*_tmp12B6;(_tmp12B6=0,
Cyc_Tcutil_warn(loc,((_tmp12B7="implicit cast to shorter array",_tag_dyneither(
_tmp12B7,sizeof(char),31))),_tag_dyneither(_tmp12B6,sizeof(void*),0)));}goto
_LL3B7;_LL3BE: _tmp777=_tmp768.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp778=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp777;if(_tmp778->tag
!= 0)goto _LL3B7;};_tmp779=_tmp768.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp77A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp779;if(_tmp77A->tag != 
1)goto _LL3B7;else{_tmp77B=_tmp77A->f1;}};_LL3BF: if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(0,(_tmp751.ptr_atts).zero_term) && Cyc_Tcutil_is_bound_one((
_tmp754.ptr_atts).bounds))goto _LL3B7;okay=0;goto _LL3B7;_LL3B7:;}okay=okay  && (!(
_tmp751.elt_tq).real_const  || (_tmp754.elt_tq).real_const);if(!Cyc_Tcutil_unify((
_tmp751.ptr_atts).rgn,(_tmp754.ptr_atts).rgn)){if(Cyc_Tcenv_region_outlives(te,(
_tmp751.ptr_atts).rgn,(_tmp754.ptr_atts).rgn)){if(Cyc_Tcutil_warn_region_coerce){
const char*_tmp12BC;void*_tmp12BB[2];struct Cyc_String_pa_PrintArg_struct _tmp12BA;
struct Cyc_String_pa_PrintArg_struct _tmp12B9;(_tmp12B9.tag=0,((_tmp12B9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((_tmp754.ptr_atts).rgn)),((
_tmp12BA.tag=0,((_tmp12BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
_tmp751.ptr_atts).rgn)),((_tmp12BB[0]=& _tmp12BA,((_tmp12BB[1]=& _tmp12B9,Cyc_Tcutil_warn(
loc,((_tmp12BC="implicit cast form region %s to region %s",_tag_dyneither(
_tmp12BC,sizeof(char),42))),_tag_dyneither(_tmp12BB,sizeof(void*),2)))))))))))));}}
else{okay=0;}}okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(
_tmp751.ptr_atts).zero_term,(_tmp754.ptr_atts).zero_term) || ((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp751.ptr_atts).zero_term)
 && (_tmp754.elt_tq).real_const);{int _tmp782=((int(*)(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_unify_it_bounds,(_tmp754.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref)
 && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(
_tmp754.ptr_atts).zero_term);okay=okay  && (Cyc_Tcutil_unify(_tmp751.elt_typ,
_tmp754.elt_typ) || (_tmp782  && ((_tmp754.elt_tq).real_const  || Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp754.elt_typ)))) && Cyc_Tcutil_ptrsubtype(
te,0,_tmp751.elt_typ,_tmp754.elt_typ));return okay;};}_LL3B1: _tmp755=_tmp74E.f1;{
struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp756=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)
_tmp755;if(_tmp756->tag != 8)goto _LL3B3;else{_tmp757=_tmp756->f1;_tmp758=_tmp757.elt_type;
_tmp759=_tmp757.tq;_tmp75A=_tmp757.num_elts;_tmp75B=_tmp757.zero_term;}};_tmp75C=
_tmp74E.f2;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp75D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)
_tmp75C;if(_tmp75D->tag != 8)goto _LL3B3;else{_tmp75E=_tmp75D->f1;_tmp75F=_tmp75E.elt_type;
_tmp760=_tmp75E.tq;_tmp761=_tmp75E.num_elts;_tmp762=_tmp75E.zero_term;}};_LL3B2: {
int okay;okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp75B,_tmp762) && ((_tmp75A != 0
 && _tmp761 != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp75A,(
struct Cyc_Absyn_Exp*)_tmp761));return(okay  && Cyc_Tcutil_unify(_tmp758,_tmp75F))
 && (!_tmp759.real_const  || _tmp760.real_const);}_LL3B3: _tmp763=_tmp74E.f1;{
struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp764=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)
_tmp763;if(_tmp764->tag != 19)goto _LL3B5;};_tmp765=_tmp74E.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp766=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp765;if(_tmp766->tag != 6)
goto _LL3B5;};_LL3B4: return 0;_LL3B5:;_LL3B6: return Cyc_Tcutil_unify(t1,t2);_LL3AE:;};}
int Cyc_Tcutil_is_pointer_type(void*t){void*_tmp783=Cyc_Tcutil_compress(t);_LL3C1: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp784=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp783;if(_tmp784->tag != 5)goto _LL3C3;}_LL3C2: return 1;_LL3C3:;_LL3C4: return 0;
_LL3C0:;}void*Cyc_Tcutil_pointer_elt_type(void*t){void*_tmp785=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp787;void*_tmp788;_LL3C6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp786=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp785;if(_tmp786->tag
!= 5)goto _LL3C8;else{_tmp787=_tmp786->f1;_tmp788=_tmp787.elt_typ;}}_LL3C7: return
_tmp788;_LL3C8:;_LL3C9: {const char*_tmp12BF;void*_tmp12BE;(_tmp12BE=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12BF="pointer_elt_type",
_tag_dyneither(_tmp12BF,sizeof(char),17))),_tag_dyneither(_tmp12BE,sizeof(void*),
0)));}_LL3C5:;}void*Cyc_Tcutil_pointer_region(void*t){void*_tmp78B=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp78D;struct Cyc_Absyn_PtrAtts _tmp78E;struct Cyc_Absyn_PtrAtts*
_tmp78F;_LL3CB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp78C=(struct
Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp78B;if(_tmp78C->tag != 5)goto _LL3CD;
else{_tmp78D=_tmp78C->f1;_tmp78E=_tmp78D.ptr_atts;_tmp78F=(struct Cyc_Absyn_PtrAtts*)&(
_tmp78C->f1).ptr_atts;}}_LL3CC: return _tmp78F->rgn;_LL3CD:;_LL3CE: {const char*
_tmp12C2;void*_tmp12C1;(_tmp12C1=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C2="pointer_elt_type",_tag_dyneither(
_tmp12C2,sizeof(char),17))),_tag_dyneither(_tmp12C1,sizeof(void*),0)));}_LL3CA:;}
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){void*_tmp792=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp794;struct Cyc_Absyn_PtrAtts _tmp795;void*_tmp796;
_LL3D0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp793=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp792;if(_tmp793->tag != 5)goto _LL3D2;else{_tmp794=_tmp793->f1;_tmp795=_tmp794.ptr_atts;
_tmp796=_tmp795.rgn;}}_LL3D1:*rgn=_tmp796;return 1;_LL3D2:;_LL3D3: return 0;_LL3CF:;}
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){void*_tmp797=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp799;struct Cyc_Absyn_PtrAtts _tmp79A;union Cyc_Absyn_Constraint*
_tmp79B;_LL3D5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp798=(struct
Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp797;if(_tmp798->tag != 5)goto _LL3D7;
else{_tmp799=_tmp798->f1;_tmp79A=_tmp799.ptr_atts;_tmp79B=_tmp79A.bounds;}}
_LL3D6:*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
Cyc_Absyn_bounds_one,_tmp79B)== (void*)& Cyc_Absyn_DynEither_b_val;return 1;_LL3D7:;
_LL3D8: return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;_LL3D4:;}int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp79C=e->r;union Cyc_Absyn_Cnst _tmp79E;struct
_tuple7 _tmp79F;int _tmp7A0;union Cyc_Absyn_Cnst _tmp7A2;struct _tuple5 _tmp7A3;char
_tmp7A4;union Cyc_Absyn_Cnst _tmp7A6;struct _tuple6 _tmp7A7;short _tmp7A8;union Cyc_Absyn_Cnst
_tmp7AA;struct _tuple8 _tmp7AB;long long _tmp7AC;union Cyc_Absyn_Cnst _tmp7AE;struct
_dyneither_ptr _tmp7AF;void*_tmp7B1;struct Cyc_Absyn_Exp*_tmp7B2;_LL3DA: {struct
Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp79D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmp79C;if(_tmp79D->tag != 0)goto _LL3DC;else{_tmp79E=_tmp79D->f1;if((_tmp79E.Int_c).tag
!= 5)goto _LL3DC;_tmp79F=(struct _tuple7)(_tmp79E.Int_c).val;_tmp7A0=_tmp79F.f2;
if(_tmp7A0 != 0)goto _LL3DC;}}_LL3DB: goto _LL3DD;_LL3DC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp7A1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp79C;if(_tmp7A1->tag != 
0)goto _LL3DE;else{_tmp7A2=_tmp7A1->f1;if((_tmp7A2.Char_c).tag != 2)goto _LL3DE;
_tmp7A3=(struct _tuple5)(_tmp7A2.Char_c).val;_tmp7A4=_tmp7A3.f2;if(_tmp7A4 != 0)
goto _LL3DE;}}_LL3DD: goto _LL3DF;_LL3DE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp7A5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp79C;if(_tmp7A5->tag != 
0)goto _LL3E0;else{_tmp7A6=_tmp7A5->f1;if((_tmp7A6.Short_c).tag != 4)goto _LL3E0;
_tmp7A7=(struct _tuple6)(_tmp7A6.Short_c).val;_tmp7A8=_tmp7A7.f2;if(_tmp7A8 != 0)
goto _LL3E0;}}_LL3DF: goto _LL3E1;_LL3E0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp7A9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp79C;if(_tmp7A9->tag != 
0)goto _LL3E2;else{_tmp7AA=_tmp7A9->f1;if((_tmp7AA.LongLong_c).tag != 6)goto _LL3E2;
_tmp7AB=(struct _tuple8)(_tmp7AA.LongLong_c).val;_tmp7AC=_tmp7AB.f2;if(_tmp7AC != 
0)goto _LL3E2;}}_LL3E1: return 1;_LL3E2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp7AD=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp79C;if(_tmp7AD->tag != 
0)goto _LL3E4;else{_tmp7AE=_tmp7AD->f1;if((_tmp7AE.Wchar_c).tag != 3)goto _LL3E4;
_tmp7AF=(struct _dyneither_ptr)(_tmp7AE.Wchar_c).val;}}_LL3E3: {unsigned long
_tmp7B3=Cyc_strlen((struct _dyneither_ptr)_tmp7AF);int i=0;if(_tmp7B3 >= 2  && *((
const char*)_check_dyneither_subscript(_tmp7AF,sizeof(char),0))== '\\'){if(*((
const char*)_check_dyneither_subscript(_tmp7AF,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp7AF,sizeof(char),1))== 'x'  && 
_tmp7B3 >= 3) && *((const char*)_check_dyneither_subscript(_tmp7AF,sizeof(char),2))
== '0')i=3;else{return 0;}}for(0;i < _tmp7B3;++ i){if(*((const char*)
_check_dyneither_subscript(_tmp7AF,sizeof(char),i))!= '0')return 0;}return 1;}
else{return 0;}}_LL3E4: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp7B0=(
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp79C;if(_tmp7B0->tag != 13)goto
_LL3E6;else{_tmp7B1=(void*)_tmp7B0->f1;_tmp7B2=_tmp7B0->f2;}}_LL3E5: return Cyc_Tcutil_is_zero(
_tmp7B2) && Cyc_Tcutil_admits_zero(_tmp7B1);_LL3E6:;_LL3E7: return 0;_LL3D9:;}
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};struct
Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};struct Cyc_Absyn_Kind
Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};struct Cyc_Absyn_Kind Cyc_Tcutil_mk={
Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,
Cyc_Absyn_Aliasable};struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};struct Cyc_Absyn_Kind
Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={
Cyc_Absyn_BoxKind,Cyc_Absyn_Top};struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,
Cyc_Absyn_Top};struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};struct
Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};struct Cyc_Absyn_Kind
Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};struct Cyc_Core_Opt Cyc_Tcutil_rko={(
void*)& Cyc_Tcutil_rk};struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};struct Cyc_Core_Opt Cyc_Tcutil_mko={(
void*)& Cyc_Tcutil_mk};struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};struct Cyc_Core_Opt Cyc_Tcutil_trko={(
void*)& Cyc_Tcutil_trk};struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};struct Cyc_Core_Opt Cyc_Tcutil_tmko={(
void*)& Cyc_Tcutil_tmk};struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};struct Cyc_Core_Opt Cyc_Tcutil_ubko={(
void*)& Cyc_Tcutil_ubk};struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind
_tmp7B5;enum Cyc_Absyn_KindQual _tmp7B6;enum Cyc_Absyn_AliasQual _tmp7B7;struct Cyc_Absyn_Kind*
_tmp7B4=ka;_tmp7B5=*_tmp7B4;_tmp7B6=_tmp7B5.kind;_tmp7B7=_tmp7B5.aliasqual;
switch(_tmp7B7){case Cyc_Absyn_Aliasable: _LL3E8: switch(_tmp7B6){case Cyc_Absyn_AnyKind:
_LL3EA: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3EB:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3EC: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3ED: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3EE: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3EF: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_iko;}
case Cyc_Absyn_Unique: _LL3E9: switch(_tmp7B6){case Cyc_Absyn_AnyKind: _LL3F2: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3F3: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3F4: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3F5: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_urko;
default: _LL3F6: break;}break;case Cyc_Absyn_Top: _LL3F1: switch(_tmp7B6){case Cyc_Absyn_AnyKind:
_LL3F9: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3FA:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL3FB: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL3FC: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_trko;default: _LL3FD: break;}break;}{const char*_tmp12C6;
void*_tmp12C5[1];struct Cyc_String_pa_PrintArg_struct _tmp12C4;(_tmp12C4.tag=0,((
_tmp12C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
ka)),((_tmp12C5[0]=& _tmp12C4,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C6="kind_to_opt: bad kind %s\n",
_tag_dyneither(_tmp12C6,sizeof(char),26))),_tag_dyneither(_tmp12C5,sizeof(void*),
1)))))));};}static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){static
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};static struct
Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct
bb_v={0,& Cyc_Tcutil_bk};static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={
0,& Cyc_Tcutil_rk};static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};static
void*ab=(void*)& ab_v;static void*mb=(void*)& mb_v;static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;static void*eb=(void*)& eb_v;static void*ib=(void*)& ib_v;
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
static void*tab=(void*)& tab_v;static void*tmb=(void*)& tmb_v;static void*tbb=(void*)&
tbb_v;static void*trb=(void*)& trb_v;static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct
uab_v={0,& Cyc_Tcutil_uak};static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct
umb_v={0,& Cyc_Tcutil_umk};static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct
ubb_v={0,& Cyc_Tcutil_ubk};static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct
urb_v={0,& Cyc_Tcutil_urk};static void*uab=(void*)& uab_v;static void*umb=(void*)&
umb_v;static void*ubb=(void*)& ubb_v;static void*urb=(void*)& urb_v;struct Cyc_Absyn_Kind
_tmp7BC;enum Cyc_Absyn_KindQual _tmp7BD;enum Cyc_Absyn_AliasQual _tmp7BE;struct Cyc_Absyn_Kind*
_tmp7BB=ka;_tmp7BC=*_tmp7BB;_tmp7BD=_tmp7BC.kind;_tmp7BE=_tmp7BC.aliasqual;
switch(_tmp7BE){case Cyc_Absyn_Aliasable: _LL3FF: switch(_tmp7BD){case Cyc_Absyn_AnyKind:
_LL401: return& ab;case Cyc_Absyn_MemKind: _LL402: return& mb;case Cyc_Absyn_BoxKind:
_LL403: return& bb;case Cyc_Absyn_RgnKind: _LL404: return& rb;case Cyc_Absyn_EffKind:
_LL405: return& eb;case Cyc_Absyn_IntKind: _LL406: return& ib;}case Cyc_Absyn_Unique:
_LL400: switch(_tmp7BD){case Cyc_Absyn_AnyKind: _LL409: return& uab;case Cyc_Absyn_MemKind:
_LL40A: return& umb;case Cyc_Absyn_BoxKind: _LL40B: return& ubb;case Cyc_Absyn_RgnKind:
_LL40C: return& urb;default: _LL40D: break;}break;case Cyc_Absyn_Top: _LL408: switch(
_tmp7BD){case Cyc_Absyn_AnyKind: _LL410: return& tab;case Cyc_Absyn_MemKind: _LL411:
return& tmb;case Cyc_Absyn_BoxKind: _LL412: return& tbb;case Cyc_Absyn_RgnKind: _LL413:
return& trb;default: _LL414: break;}break;}{const char*_tmp12CA;void*_tmp12C9[1];
struct Cyc_String_pa_PrintArg_struct _tmp12C8;(_tmp12C8.tag=0,((_tmp12C8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((
_tmp12C9[0]=& _tmp12C8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12CA="kind_to_b: bad kind %s\n",_tag_dyneither(_tmp12CA,
sizeof(char),24))),_tag_dyneither(_tmp12C9,sizeof(void*),1)))))));};}void*Cyc_Tcutil_kind_to_bound(
struct Cyc_Absyn_Kind*k){return*Cyc_Tcutil_kind_to_b(k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(
struct Cyc_Absyn_Kind*k){struct Cyc_Core_Opt*_tmp12CB;return(_tmp12CB=_cycalloc(
sizeof(*_tmp12CB)),((_tmp12CB->v=Cyc_Tcutil_kind_to_bound(k),_tmp12CB)));}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct
_tmp12CE;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp12CD;e1->r=(void*)((
_tmp12CD=_cycalloc(sizeof(*_tmp12CD)),((_tmp12CD[0]=((_tmp12CE.tag=0,((_tmp12CE.f1=
Cyc_Absyn_Null_c,_tmp12CE)))),_tmp12CD))));}{struct Cyc_Core_Opt*_tmp7D3=Cyc_Tcenv_lookup_opt_type_vars(
te);struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp12D8;struct Cyc_Absyn_PtrAtts
_tmp12D7;struct Cyc_Absyn_PtrInfo _tmp12D6;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp12D5;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7D4=(_tmp12D5=
_cycalloc(sizeof(*_tmp12D5)),((_tmp12D5[0]=((_tmp12D8.tag=5,((_tmp12D8.f1=((
_tmp12D6.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmp7D3),((
_tmp12D6.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp12D6.ptr_atts=((_tmp12D7.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,_tmp7D3),((_tmp12D7.nullable=Cyc_Absyn_true_conref,((
_tmp12D7.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp12D7.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp12D7.ptrloc=0,_tmp12D7)))))))))),_tmp12D6)))))),_tmp12D8)))),_tmp12D5)));e1->topt=(
void*)_tmp7D4;{int bogus=0;int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);if(
bogus != 0){const char*_tmp12DD;void*_tmp12DC[2];struct Cyc_String_pa_PrintArg_struct
_tmp12DB;struct Cyc_String_pa_PrintArg_struct _tmp12DA;(_tmp12DA.tag=0,((_tmp12DA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(e1->loc)),((
_tmp12DB.tag=0,((_tmp12DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp12DC[0]=& _tmp12DB,((_tmp12DC[1]=& _tmp12DA,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12DD="zero_to_null resulted in an alias coercion on %s at %s\n",
_tag_dyneither(_tmp12DD,sizeof(char),56))),_tag_dyneither(_tmp12DC,sizeof(void*),
2)))))))))))));}return retv;};};}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(
enum Cyc_Absyn_Coercion c){switch(c){case Cyc_Absyn_Unknown_coercion: _LL416: {
const char*_tmp12DE;return(_tmp12DE="unknown",_tag_dyneither(_tmp12DE,sizeof(char),
8));}case Cyc_Absyn_No_coercion: _LL417: {const char*_tmp12DF;return(_tmp12DF="no coercion",
_tag_dyneither(_tmp12DF,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL418: {
const char*_tmp12E0;return(_tmp12E0="null check",_tag_dyneither(_tmp12E0,sizeof(
char),11));}case Cyc_Absyn_Other_coercion: _LL419: {const char*_tmp12E1;return(
_tmp12E1="other coercion",_tag_dyneither(_tmp12E1,sizeof(char),15));}}}int Cyc_Tcutil_warn_alias_coerce=
0;struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){static
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};static int
counter=0;struct _dyneither_ptr*_tmp12EE;const char*_tmp12ED;void*_tmp12EC[1];
struct Cyc_Int_pa_PrintArg_struct _tmp12EB;struct _tuple2*_tmp12EA;struct _tuple2*v=(
_tmp12EA=_cycalloc(sizeof(*_tmp12EA)),((_tmp12EA->f1=Cyc_Absyn_Loc_n,((_tmp12EA->f2=((
_tmp12EE=_cycalloc(sizeof(*_tmp12EE)),((_tmp12EE[0]=(struct _dyneither_ptr)((
_tmp12EB.tag=1,((_tmp12EB.f1=(unsigned long)counter ++,((_tmp12EC[0]=& _tmp12EB,
Cyc_aprintf(((_tmp12ED="__aliasvar%d",_tag_dyneither(_tmp12ED,sizeof(char),13))),
_tag_dyneither(_tmp12EC,sizeof(void*),1)))))))),_tmp12EE)))),_tmp12EA)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,(void*)& Cyc_Absyn_VoidType_val,
0);struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp12F1;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmp12F0;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmp12F0=_cycalloc(
sizeof(*_tmp12F0)),((_tmp12F0[0]=((_tmp12F1.tag=4,((_tmp12F1.f1=vd,_tmp12F1)))),
_tmp12F0)))),e->loc);struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);{
void*_tmp7E1=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_PtrInfo _tmp7E3;struct Cyc_Absyn_PtrAtts
_tmp7E4;void*_tmp7E5;void**_tmp7E6;_LL41C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp7E2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7E1;if(_tmp7E2->tag
!= 5)goto _LL41E;else{_tmp7E3=_tmp7E2->f1;_tmp7E4=_tmp7E3.ptr_atts;_tmp7E5=
_tmp7E4.rgn;_tmp7E6=(void**)&((_tmp7E2->f1).ptr_atts).rgn;}}_LL41D:{struct Cyc_Absyn_VarType_Absyn_Type_struct
_tmp12F4;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp12F3;*_tmp7E6=(void*)((
_tmp12F3=_cycalloc(sizeof(*_tmp12F3)),((_tmp12F3[0]=((_tmp12F4.tag=2,((_tmp12F4.f1=
tv,_tmp12F4)))),_tmp12F3))));}goto _LL41B;_LL41E:;_LL41F: goto _LL41B;_LL41B:;}e->topt=
0;e=Cyc_Absyn_cast_exp(e_typ,e,0,Cyc_Absyn_Unknown_coercion,e->loc);{struct Cyc_Absyn_Decl*
d=Cyc_Absyn_alias_decl(e,tv,vd,e->loc);struct _tuple15 _tmp12F5;return(_tmp12F5.f1=
d,((_tmp12F5.f2=ve,_tmp12F5)));};}static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*
e,void*e_typ,void*wants_typ,unsigned int loc){struct _RegionHandle _tmp7F2=
_new_region("r");struct _RegionHandle*r=& _tmp7F2;_push_region(r);if((Cyc_Tcutil_is_noalias_path(
r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ)) && Cyc_Tcutil_is_pointer_type(
e_typ)){void*_tmp7F3=Cyc_Tcutil_compress(wants_typ);struct Cyc_Absyn_PtrInfo
_tmp7F5;struct Cyc_Absyn_PtrAtts _tmp7F6;void*_tmp7F7;_LL421: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp7F4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7F3;if(_tmp7F4->tag
!= 5)goto _LL423;else{_tmp7F5=_tmp7F4->f1;_tmp7F6=_tmp7F5.ptr_atts;_tmp7F7=
_tmp7F6.rgn;}}_LL422: {void*_tmp7F9=Cyc_Tcutil_compress(_tmp7F7);_LL428: {struct
Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp7FA=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)
_tmp7F9;if(_tmp7FA->tag != 20)goto _LL42A;}_LL429: {int _tmp7FB=0;_npop_handler(0);
return _tmp7FB;}_LL42A:;_LL42B: {struct Cyc_Absyn_Kind*_tmp7FC=Cyc_Tcutil_typ_kind(
_tmp7F7);int _tmp7FD=_tmp7FC->kind == Cyc_Absyn_RgnKind  && _tmp7FC->aliasqual == 
Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp7FD;}_LL427:;}_LL423: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*
_tmp7F8=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp7F3;if(_tmp7F8->tag
!= 17)goto _LL425;}_LL424: {const char*_tmp12FA;void*_tmp12F9[2];struct Cyc_String_pa_PrintArg_struct
_tmp12F8;struct Cyc_String_pa_PrintArg_struct _tmp12F7;(_tmp12F7.tag=0,((_tmp12F7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(loc)),((
_tmp12F8.tag=0,((_tmp12F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_compress(wants_typ))),((_tmp12F9[0]=& _tmp12F8,((_tmp12F9[1]=& _tmp12F7,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp12FA="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmp12FA,
sizeof(char),42))),_tag_dyneither(_tmp12F9,sizeof(void*),2)))))))))))));}_LL425:;
_LL426: {int _tmp802=0;_npop_handler(0);return _tmp802;}_LL420:;}{int _tmp803=0;
_npop_handler(0);return _tmp803;};;_pop_region(r);}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){void*
t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));enum Cyc_Absyn_Coercion c;int
do_alias_coercion=0;if(Cyc_Tcutil_unify(t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(
t2) && Cyc_Tcutil_is_arithmetic_type(t1)){if(Cyc_Tcutil_will_lose_precision(t1,
t2)){const char*_tmp12FF;void*_tmp12FE[2];struct Cyc_String_pa_PrintArg_struct
_tmp12FD;struct Cyc_String_pa_PrintArg_struct _tmp12FC;(_tmp12FC.tag=0,((_tmp12FC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((
_tmp12FD.tag=0,((_tmp12FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp12FE[0]=& _tmp12FD,((_tmp12FE[1]=& _tmp12FC,Cyc_Tcutil_warn(e->loc,((
_tmp12FF="integral size mismatch; %s -> %s conversion supplied",_tag_dyneither(
_tmp12FF,sizeof(char),53))),_tag_dyneither(_tmp12FE,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);return 1;}else{if(Cyc_Tcutil_can_insert_alias(
e,t1,t2,e->loc)){if(Cyc_Tcutil_warn_alias_coerce){const char*_tmp1305;void*
_tmp1304[3];struct Cyc_String_pa_PrintArg_struct _tmp1303;struct Cyc_String_pa_PrintArg_struct
_tmp1302;struct Cyc_String_pa_PrintArg_struct _tmp1301;(_tmp1301.tag=0,((_tmp1301.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((
_tmp1302.tag=0,((_tmp1302.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp1303.tag=0,((_tmp1303.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp1304[0]=& _tmp1303,((_tmp1304[1]=& _tmp1302,((
_tmp1304[2]=& _tmp1301,Cyc_Tcutil_warn(e->loc,((_tmp1305="implicit alias coercion for %s:%s to %s",
_tag_dyneither(_tmp1305,sizeof(char),40))),_tag_dyneither(_tmp1304,sizeof(void*),
3)))))))))))))))))));}*alias_coercion=1;}if(Cyc_Tcutil_silent_castable(te,e->loc,
t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);return 1;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if((c=Cyc_Tcutil_castable(
te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){Cyc_Tcutil_unchecked_cast(te,e,
t2,c);if(c != Cyc_Absyn_NonNull_to_Null){const char*_tmp130A;void*_tmp1309[2];
struct Cyc_String_pa_PrintArg_struct _tmp1308;struct Cyc_String_pa_PrintArg_struct
_tmp1307;(_tmp1307.tag=0,((_tmp1307.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp1308.tag=0,((_tmp1308.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((
_tmp1309[0]=& _tmp1308,((_tmp1309[1]=& _tmp1307,Cyc_Tcutil_warn(e->loc,((_tmp130A="implicit cast from %s to %s",
_tag_dyneither(_tmp130A,sizeof(char),28))),_tag_dyneither(_tmp1309,sizeof(void*),
2)))))))))))));}return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t){int bogus=0;return Cyc_Tcutil_coerce_arg(te,e,t,&
bogus);}int Cyc_Tcutil_coerceable(void*t){void*_tmp811=Cyc_Tcutil_compress(t);
_LL42D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp812=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp811;if(_tmp812->tag != 6)goto _LL42F;}_LL42E: goto _LL430;_LL42F: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp813=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp811;if(_tmp813->tag != 
7)goto _LL431;}_LL430: return 1;_LL431:;_LL432: return 0;_LL42C:;}static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,
void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*
f3;int f4;};static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,
struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp815;struct _RegionHandle*
_tmp816;struct Cyc_Tcenv_Tenv*_tmp817;int _tmp818;struct _tuple23 _tmp814=*env;
_tmp815=_tmp814.f1;_tmp816=_tmp814.f2;_tmp817=_tmp814.f3;_tmp818=_tmp814.f4;{
void*_tmp819=_tmp815 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp816,_tmp815,x->type);
struct Cyc_List_List*_tmp81A=Cyc_Tcutil_flatten_typ(_tmp816,_tmp818,_tmp817,
_tmp819);if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp81A)== 1){
struct _tuple12*_tmp130D;struct Cyc_List_List*_tmp130C;return(_tmp130C=
_region_malloc(_tmp816,sizeof(*_tmp130C)),((_tmp130C->hd=((_tmp130D=
_region_malloc(_tmp816,sizeof(*_tmp130D)),((_tmp130D->f1=x->tq,((_tmp130D->f2=
_tmp819,_tmp130D)))))),((_tmp130C->tl=0,_tmp130C)))));}else{return _tmp81A;}};}
struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};static
struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
struct _RegionHandle*_tmp81E;struct Cyc_Tcenv_Tenv*_tmp81F;int _tmp820;struct
_tuple24 _tmp81D=*env;_tmp81E=_tmp81D.f1;_tmp81F=_tmp81D.f2;_tmp820=_tmp81D.f3;{
struct Cyc_Absyn_Tqual _tmp822;void*_tmp823;struct _tuple12 _tmp821=*x;_tmp822=
_tmp821.f1;_tmp823=_tmp821.f2;{struct Cyc_List_List*_tmp824=Cyc_Tcutil_flatten_typ(
_tmp81E,_tmp820,_tmp81F,_tmp823);if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp824)== 1){struct _tuple12*_tmp1310;struct Cyc_List_List*_tmp130F;return(
_tmp130F=_region_malloc(_tmp81E,sizeof(*_tmp130F)),((_tmp130F->hd=((_tmp1310=
_region_malloc(_tmp81E,sizeof(*_tmp1310)),((_tmp1310->f1=_tmp822,((_tmp1310->f2=
_tmp823,_tmp1310)))))),((_tmp130F->tl=0,_tmp130F)))));}else{return _tmp824;}};};}
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int
flatten,struct Cyc_Tcenv_Tenv*te,void*t1){if(flatten){t1=Cyc_Tcutil_compress(t1);{
void*_tmp827=t1;struct Cyc_List_List*_tmp82A;struct Cyc_Absyn_AggrInfo _tmp82C;
union Cyc_Absyn_AggrInfoU _tmp82D;struct Cyc_Absyn_Aggrdecl**_tmp82E;struct Cyc_Absyn_Aggrdecl*
_tmp82F;struct Cyc_List_List*_tmp830;enum Cyc_Absyn_AggrKind _tmp832;struct Cyc_List_List*
_tmp833;_LL434: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp828=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)
_tmp827;if(_tmp828->tag != 0)goto _LL436;}_LL435: return 0;_LL436: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmp829=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp827;if(_tmp829->tag != 
10)goto _LL438;else{_tmp82A=_tmp829->f1;}}_LL437: {struct _tuple24 _tmp1311;struct
_tuple24 _tmp834=(_tmp1311.f1=r,((_tmp1311.f2=te,((_tmp1311.f3=flatten,_tmp1311)))));
struct Cyc_List_List*_tmp835=_tmp82A;struct Cyc_List_List _tmp836;struct _tuple12*
_tmp837;struct Cyc_List_List*_tmp838;_LL43F: if(_tmp835 != 0)goto _LL441;_LL440:
return 0;_LL441: if(_tmp835 == 0)goto _LL43E;_tmp836=*_tmp835;_tmp837=(struct
_tuple12*)_tmp836.hd;_tmp838=_tmp836.tl;_LL442: {struct Cyc_List_List*_tmp839=Cyc_Tcutil_rcopy_tqt(&
_tmp834,_tmp837);_tmp834.f3=0;{struct Cyc_List_List*_tmp83A=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),
struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,&
_tmp834,_tmp82A);struct Cyc_List_List*_tmp1312;struct Cyc_List_List*_tmp83B=(
_tmp1312=_region_malloc(r,sizeof(*_tmp1312)),((_tmp1312->hd=_tmp839,((_tmp1312->tl=
_tmp83A,_tmp1312)))));struct Cyc_List_List*_tmp1313;return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp1313=
_region_malloc(r,sizeof(*_tmp1313)),((_tmp1313->hd=_tmp839,((_tmp1313->tl=
_tmp83A,_tmp1313)))))));};}_LL43E:;}_LL438: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp82B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp827;if(_tmp82B->tag != 
11)goto _LL43A;else{_tmp82C=_tmp82B->f1;_tmp82D=_tmp82C.aggr_info;if((_tmp82D.KnownAggr).tag
!= 2)goto _LL43A;_tmp82E=(struct Cyc_Absyn_Aggrdecl**)(_tmp82D.KnownAggr).val;
_tmp82F=*_tmp82E;_tmp830=_tmp82C.targs;}}_LL439: if(((_tmp82F->kind == Cyc_Absyn_UnionA
 || _tmp82F->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp82F->impl))->exist_vars
!= 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp82F->impl))->rgn_po != 0){
struct _tuple12*_tmp1316;struct Cyc_List_List*_tmp1315;return(_tmp1315=
_region_malloc(r,sizeof(*_tmp1315)),((_tmp1315->hd=((_tmp1316=_region_malloc(r,
sizeof(*_tmp1316)),((_tmp1316->f1=Cyc_Absyn_empty_tqual(0),((_tmp1316->f2=t1,
_tmp1316)))))),((_tmp1315->tl=0,_tmp1315)))));}{struct Cyc_List_List*_tmp841=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp82F->tvs,_tmp830);struct _tuple23
_tmp1317;struct _tuple23 env=(_tmp1317.f1=_tmp841,((_tmp1317.f2=r,((_tmp1317.f3=te,((
_tmp1317.f4=flatten,_tmp1317)))))));struct Cyc_List_List*_tmp842=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp82F->impl))->fields;struct Cyc_List_List _tmp843;struct Cyc_Absyn_Aggrfield*
_tmp844;struct Cyc_List_List*_tmp845;_LL444: if(_tmp842 != 0)goto _LL446;_LL445:
return 0;_LL446: if(_tmp842 == 0)goto _LL443;_tmp843=*_tmp842;_tmp844=(struct Cyc_Absyn_Aggrfield*)
_tmp843.hd;_tmp845=_tmp843.tl;_LL447: {struct Cyc_List_List*_tmp846=Cyc_Tcutil_flatten_typ_f(&
env,_tmp844);env.f4=0;{struct Cyc_List_List*_tmp847=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),
struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp845);struct Cyc_List_List*_tmp1318;struct Cyc_List_List*_tmp848=(_tmp1318=
_region_malloc(r,sizeof(*_tmp1318)),((_tmp1318->hd=_tmp846,((_tmp1318->tl=
_tmp847,_tmp1318)))));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp848);};}_LL443:;};_LL43A: {
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp831=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)
_tmp827;if(_tmp831->tag != 12)goto _LL43C;else{_tmp832=_tmp831->f1;if(_tmp832 != 
Cyc_Absyn_StructA)goto _LL43C;_tmp833=_tmp831->f2;}}_LL43B: {struct _tuple23
_tmp1319;struct _tuple23 env=(_tmp1319.f1=0,((_tmp1319.f2=r,((_tmp1319.f3=te,((
_tmp1319.f4=flatten,_tmp1319)))))));struct Cyc_List_List*_tmp84B=_tmp833;struct
Cyc_List_List _tmp84C;struct Cyc_Absyn_Aggrfield*_tmp84D;struct Cyc_List_List*
_tmp84E;_LL449: if(_tmp84B != 0)goto _LL44B;_LL44A: return 0;_LL44B: if(_tmp84B == 0)
goto _LL448;_tmp84C=*_tmp84B;_tmp84D=(struct Cyc_Absyn_Aggrfield*)_tmp84C.hd;
_tmp84E=_tmp84C.tl;_LL44C: {struct Cyc_List_List*_tmp84F=Cyc_Tcutil_flatten_typ_f(&
env,_tmp84D);env.f4=0;{struct Cyc_List_List*_tmp850=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),
struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp84E);struct Cyc_List_List*_tmp131A;struct Cyc_List_List*_tmp851=(_tmp131A=
_region_malloc(r,sizeof(*_tmp131A)),((_tmp131A->hd=_tmp84F,((_tmp131A->tl=
_tmp850,_tmp131A)))));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp851);};}_LL448:;}_LL43C:;_LL43D:
goto _LL433;_LL433:;};}{struct _tuple12*_tmp131D;struct Cyc_List_List*_tmp131C;
return(_tmp131C=_region_malloc(r,sizeof(*_tmp131C)),((_tmp131C->hd=((_tmp131D=
_region_malloc(r,sizeof(*_tmp131D)),((_tmp131D->f1=Cyc_Absyn_empty_tqual(0),((
_tmp131D->f2=t1,_tmp131D)))))),((_tmp131C->tl=0,_tmp131C)))));};}static int Cyc_Tcutil_sub_attributes(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t
!= 0;t=t->tl){void*_tmp856=(void*)t->hd;_LL44E: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*
_tmp857=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp856;if(_tmp857->tag
!= 22)goto _LL450;}_LL44F: goto _LL451;_LL450: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*
_tmp858=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp856;if(_tmp858->tag
!= 4)goto _LL452;}_LL451: goto _LL453;_LL452: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*
_tmp859=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp856;if(
_tmp859->tag != 20)goto _LL454;}_LL453: continue;_LL454:;_LL455: if(!((int(*)(int(*
pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(
void*)t->hd,a2))return 0;_LL44D:;}}for(0;a2 != 0;a2=a2->tl){if(!((int(*)(int(*pred)(
void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(
void*)a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_isomorphic(void*t1,void*
t2){struct _tuple0 _tmp131E;struct _tuple0 _tmp85B=(_tmp131E.f1=Cyc_Tcutil_compress(
t1),((_tmp131E.f2=Cyc_Tcutil_compress(t2),_tmp131E)));void*_tmp85C;enum Cyc_Absyn_Size_of
_tmp85E;void*_tmp85F;enum Cyc_Absyn_Size_of _tmp861;_LL457: _tmp85C=_tmp85B.f1;{
struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp85D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp85C;if(_tmp85D->tag != 6)goto _LL459;else{_tmp85E=_tmp85D->f2;}};_tmp85F=
_tmp85B.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp860=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp85F;if(_tmp860->tag != 6)goto _LL459;else{_tmp861=_tmp860->f2;}};_LL458: return(
_tmp85E == _tmp861  || _tmp85E == Cyc_Absyn_Int_sz  && _tmp861 == Cyc_Absyn_Long_sz)
 || _tmp85E == Cyc_Absyn_Long_sz  && _tmp861 == Cyc_Absyn_Int_sz;_LL459:;_LL45A:
return 0;_LL456:;}static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return 1;{struct Cyc_List_List*
a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1)
 && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))return 1;}}t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp131F;struct _tuple0 _tmp863=(
_tmp131F.f1=t1,((_tmp131F.f2=t2,_tmp131F)));void*_tmp864;struct Cyc_Absyn_PtrInfo
_tmp866;void*_tmp867;struct Cyc_Absyn_Tqual _tmp868;struct Cyc_Absyn_PtrAtts _tmp869;
void*_tmp86A;union Cyc_Absyn_Constraint*_tmp86B;union Cyc_Absyn_Constraint*_tmp86C;
union Cyc_Absyn_Constraint*_tmp86D;void*_tmp86E;struct Cyc_Absyn_PtrInfo _tmp870;
void*_tmp871;struct Cyc_Absyn_Tqual _tmp872;struct Cyc_Absyn_PtrAtts _tmp873;void*
_tmp874;union Cyc_Absyn_Constraint*_tmp875;union Cyc_Absyn_Constraint*_tmp876;
union Cyc_Absyn_Constraint*_tmp877;void*_tmp878;struct Cyc_Absyn_DatatypeFieldInfo
_tmp87A;union Cyc_Absyn_DatatypeFieldInfoU _tmp87B;struct _tuple3 _tmp87C;struct Cyc_Absyn_Datatypedecl*
_tmp87D;struct Cyc_Absyn_Datatypefield*_tmp87E;struct Cyc_List_List*_tmp87F;void*
_tmp880;struct Cyc_Absyn_DatatypeInfo _tmp882;union Cyc_Absyn_DatatypeInfoU _tmp883;
struct Cyc_Absyn_Datatypedecl**_tmp884;struct Cyc_Absyn_Datatypedecl*_tmp885;
struct Cyc_List_List*_tmp886;void*_tmp887;struct Cyc_Absyn_FnInfo _tmp889;void*
_tmp88A;struct Cyc_Absyn_FnInfo _tmp88C;_LL45C: _tmp864=_tmp863.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp865=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp864;if(_tmp865->tag
!= 5)goto _LL45E;else{_tmp866=_tmp865->f1;_tmp867=_tmp866.elt_typ;_tmp868=_tmp866.elt_tq;
_tmp869=_tmp866.ptr_atts;_tmp86A=_tmp869.rgn;_tmp86B=_tmp869.nullable;_tmp86C=
_tmp869.bounds;_tmp86D=_tmp869.zero_term;}};_tmp86E=_tmp863.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp86F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp86E;if(_tmp86F->tag
!= 5)goto _LL45E;else{_tmp870=_tmp86F->f1;_tmp871=_tmp870.elt_typ;_tmp872=_tmp870.elt_tq;
_tmp873=_tmp870.ptr_atts;_tmp874=_tmp873.rgn;_tmp875=_tmp873.nullable;_tmp876=
_tmp873.bounds;_tmp877=_tmp873.zero_term;}};_LL45D: if(_tmp868.real_const  && !
_tmp872.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp86B,
_tmp875) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp86B)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp875))return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,
union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp86D,
_tmp877) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp86D)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp877))return 0;if(!Cyc_Tcutil_unify(_tmp86A,_tmp874) && !Cyc_Tcenv_region_outlives(
te,_tmp86A,_tmp874))return 0;if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp86C,_tmp876)){struct _tuple0 _tmp1320;struct _tuple0 _tmp88E=(_tmp1320.f1=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp86C),((_tmp1320.f2=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp876),_tmp1320)));void*
_tmp88F;void*_tmp891;void*_tmp893;struct Cyc_Absyn_Exp*_tmp895;void*_tmp896;
struct Cyc_Absyn_Exp*_tmp898;_LL465: _tmp88F=_tmp88E.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp890=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp88F;if(_tmp890->tag != 
1)goto _LL467;};_tmp891=_tmp88E.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp892=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp891;if(_tmp892->tag
!= 0)goto _LL467;};_LL466: goto _LL464;_LL467: _tmp893=_tmp88E.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp894=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp893;if(_tmp894->tag != 
1)goto _LL469;else{_tmp895=_tmp894->f1;}};_tmp896=_tmp88E.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp897=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp896;if(_tmp897->tag != 
1)goto _LL469;else{_tmp898=_tmp897->f1;}};_LL468: if(!Cyc_Evexp_lte_const_exp(
_tmp898,_tmp895))return 0;goto _LL464;_LL469:;_LL46A: return 0;_LL464:;}if(!_tmp872.real_const
 && _tmp868.real_const){if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(
_tmp871)))return 0;}{int _tmp899=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp876,Cyc_Absyn_bounds_one_conref) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(0,_tmp877);struct _tuple0*_tmp1323;struct Cyc_List_List*
_tmp1322;return(_tmp899  && Cyc_Tcutil_ptrsubtype(te,((_tmp1322=_cycalloc(sizeof(*
_tmp1322)),((_tmp1322->hd=((_tmp1323=_cycalloc(sizeof(*_tmp1323)),((_tmp1323->f1=
t1,((_tmp1323->f2=t2,_tmp1323)))))),((_tmp1322->tl=assume,_tmp1322)))))),_tmp867,
_tmp871) || Cyc_Tcutil_unify(t1,t2)) || Cyc_Tcutil_isomorphic(t1,t2);};_LL45E:
_tmp878=_tmp863.f1;{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp879=(
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp878;if(_tmp879->tag != 4)
goto _LL460;else{_tmp87A=_tmp879->f1;_tmp87B=_tmp87A.field_info;if((_tmp87B.KnownDatatypefield).tag
!= 2)goto _LL460;_tmp87C=(struct _tuple3)(_tmp87B.KnownDatatypefield).val;_tmp87D=
_tmp87C.f1;_tmp87E=_tmp87C.f2;_tmp87F=_tmp87A.targs;}};_tmp880=_tmp863.f2;{
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp881=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)
_tmp880;if(_tmp881->tag != 3)goto _LL460;else{_tmp882=_tmp881->f1;_tmp883=_tmp882.datatype_info;
if((_tmp883.KnownDatatype).tag != 2)goto _LL460;_tmp884=(struct Cyc_Absyn_Datatypedecl**)(
_tmp883.KnownDatatype).val;_tmp885=*_tmp884;_tmp886=_tmp882.targs;}};_LL45F: if(
_tmp87D != _tmp885  && Cyc_Absyn_qvar_cmp(_tmp87D->name,_tmp885->name)!= 0)return 0;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp87F)!= ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp886))return 0;for(0;_tmp87F != 0;(_tmp87F=
_tmp87F->tl,_tmp886=_tmp886->tl)){if(!Cyc_Tcutil_unify((void*)_tmp87F->hd,(void*)((
struct Cyc_List_List*)_check_null(_tmp886))->hd))return 0;}return 1;_LL460: _tmp887=
_tmp863.f1;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp888=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)
_tmp887;if(_tmp888->tag != 9)goto _LL462;else{_tmp889=_tmp888->f1;}};_tmp88A=
_tmp863.f2;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp88B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)
_tmp88A;if(_tmp88B->tag != 9)goto _LL462;else{_tmp88C=_tmp88B->f1;}};_LL461: if(
_tmp889.tvars != 0  || _tmp88C.tvars != 0){struct Cyc_List_List*_tmp89C=_tmp889.tvars;
struct Cyc_List_List*_tmp89D=_tmp88C.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp89C)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp89D))return 0;{
struct _RegionHandle _tmp89E=_new_region("r");struct _RegionHandle*r=& _tmp89E;
_push_region(r);{struct Cyc_List_List*inst=0;while(_tmp89C != 0){if(!Cyc_Tcutil_unify_kindbound(((
struct Cyc_Absyn_Tvar*)_tmp89C->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)
_check_null(_tmp89D))->hd)->kind)){int _tmp89F=0;_npop_handler(0);return _tmp89F;}{
struct _tuple16*_tmp132D;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp132C;struct
Cyc_Absyn_VarType_Absyn_Type_struct*_tmp132B;struct Cyc_List_List*_tmp132A;inst=((
_tmp132A=_region_malloc(r,sizeof(*_tmp132A)),((_tmp132A->hd=((_tmp132D=
_region_malloc(r,sizeof(*_tmp132D)),((_tmp132D->f1=(struct Cyc_Absyn_Tvar*)
_tmp89D->hd,((_tmp132D->f2=(void*)((_tmp132B=_cycalloc(sizeof(*_tmp132B)),((
_tmp132B[0]=((_tmp132C.tag=2,((_tmp132C.f1=(struct Cyc_Absyn_Tvar*)_tmp89C->hd,
_tmp132C)))),_tmp132B)))),_tmp132D)))))),((_tmp132A->tl=inst,_tmp132A))))));}
_tmp89C=_tmp89C->tl;_tmp89D=_tmp89D->tl;}if(inst != 0){_tmp889.tvars=0;_tmp88C.tvars=
0;{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1333;struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp1332;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1330;struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp132F;int _tmp8A8=Cyc_Tcutil_subtype(te,assume,(void*)((_tmp132F=_cycalloc(
sizeof(*_tmp132F)),((_tmp132F[0]=((_tmp1330.tag=9,((_tmp1330.f1=_tmp889,_tmp1330)))),
_tmp132F)))),(void*)((_tmp1332=_cycalloc(sizeof(*_tmp1332)),((_tmp1332[0]=((
_tmp1333.tag=9,((_tmp1333.f1=_tmp88C,_tmp1333)))),_tmp1332)))));_npop_handler(0);
return _tmp8A8;};}};_pop_region(r);};}if(!Cyc_Tcutil_subtype(te,assume,_tmp889.ret_typ,
_tmp88C.ret_typ))return 0;{struct Cyc_List_List*_tmp8A9=_tmp889.args;struct Cyc_List_List*
_tmp8AA=_tmp88C.args;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8A9)
!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8AA))return 0;for(0;
_tmp8A9 != 0;(_tmp8A9=_tmp8A9->tl,_tmp8AA=_tmp8AA->tl)){struct Cyc_Absyn_Tqual
_tmp8AC;void*_tmp8AD;struct _tuple10 _tmp8AB=*((struct _tuple10*)_tmp8A9->hd);
_tmp8AC=_tmp8AB.f2;_tmp8AD=_tmp8AB.f3;{struct Cyc_Absyn_Tqual _tmp8AF;void*_tmp8B0;
struct _tuple10 _tmp8AE=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(
_tmp8AA))->hd);_tmp8AF=_tmp8AE.f2;_tmp8B0=_tmp8AE.f3;if(_tmp8AF.real_const  && !
_tmp8AC.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp8B0,_tmp8AD))return 0;};}
if(_tmp889.c_varargs != _tmp88C.c_varargs)return 0;if(_tmp889.cyc_varargs != 0  && 
_tmp88C.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp8B1=*_tmp889.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp8B2=*_tmp88C.cyc_varargs;if((_tmp8B2.tq).real_const
 && !(_tmp8B1.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp8B2.type,
_tmp8B1.type))return 0;}else{if(_tmp889.cyc_varargs != 0  || _tmp88C.cyc_varargs != 
0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp889.effect),(
void*)_check_null(_tmp88C.effect)))return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp889.rgn_po,
_tmp88C.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(_tmp889.attributes,_tmp88C.attributes))
return 0;return 1;};_LL462:;_LL463: return 0;_LL45B:;};}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){struct
_RegionHandle _tmp8B3=_new_region("temp");struct _RegionHandle*temp=& _tmp8B3;
_push_region(temp);{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);for(0;tqs2 != 0;(
tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0){int _tmp8B4=0;_npop_handler(0);return
_tmp8B4;}{struct _tuple12 _tmp8B6;struct Cyc_Absyn_Tqual _tmp8B7;void*_tmp8B8;struct
_tuple12*_tmp8B5=(struct _tuple12*)tqs1->hd;_tmp8B6=*_tmp8B5;_tmp8B7=_tmp8B6.f1;
_tmp8B8=_tmp8B6.f2;{struct _tuple12 _tmp8BA;struct Cyc_Absyn_Tqual _tmp8BB;void*
_tmp8BC;struct _tuple12*_tmp8B9=(struct _tuple12*)tqs2->hd;_tmp8BA=*_tmp8B9;
_tmp8BB=_tmp8BA.f1;_tmp8BC=_tmp8BA.f2;if(_tmp8B7.real_const  && !_tmp8BB.real_const){
int _tmp8BD=0;_npop_handler(0);return _tmp8BD;}if((_tmp8BB.real_const  || Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp8BC))) && Cyc_Tcutil_subtype(te,assume,
_tmp8B8,_tmp8BC))continue;if(Cyc_Tcutil_unify(_tmp8B8,_tmp8BC))continue;if(Cyc_Tcutil_isomorphic(
_tmp8B8,_tmp8BC))continue;{int _tmp8BE=0;_npop_handler(0);return _tmp8BE;};};};}{
int _tmp8BF=1;_npop_handler(0);return _tmp8BF;};};_pop_region(temp);}static int Cyc_Tcutil_is_char_type(
void*t){void*_tmp8C0=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmp8C2;_LL46C: {
struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8C1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp8C0;if(_tmp8C1->tag != 6)goto _LL46E;else{_tmp8C2=_tmp8C1->f2;if(_tmp8C2 != Cyc_Absyn_Char_sz)
goto _LL46E;}}_LL46D: return 1;_LL46E:;_LL46F: return 0;_LL46B:;}enum Cyc_Absyn_Coercion
Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){if(
Cyc_Tcutil_unify(t1,t2))return Cyc_Absyn_No_coercion;t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);if(t2 == (void*)& Cyc_Absyn_VoidType_val)return Cyc_Absyn_No_coercion;{
void*_tmp8C3=t2;enum Cyc_Absyn_Size_of _tmp8C5;enum Cyc_Absyn_Size_of _tmp8C7;
_LL471: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8C4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp8C3;if(_tmp8C4->tag != 6)goto _LL473;else{_tmp8C5=_tmp8C4->f2;if(_tmp8C5 != Cyc_Absyn_Int_sz)
goto _LL473;}}_LL472: goto _LL474;_LL473: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp8C6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8C3;if(_tmp8C6->tag != 6)
goto _LL475;else{_tmp8C7=_tmp8C6->f2;if(_tmp8C7 != Cyc_Absyn_Long_sz)goto _LL475;}}
_LL474: if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_No_coercion;
goto _LL470;_LL475:;_LL476: goto _LL470;_LL470:;}{void*_tmp8C8=t1;struct Cyc_Absyn_PtrInfo
_tmp8CA;void*_tmp8CB;struct Cyc_Absyn_Tqual _tmp8CC;struct Cyc_Absyn_PtrAtts _tmp8CD;
void*_tmp8CE;union Cyc_Absyn_Constraint*_tmp8CF;union Cyc_Absyn_Constraint*_tmp8D0;
union Cyc_Absyn_Constraint*_tmp8D1;struct Cyc_Absyn_ArrayInfo _tmp8D3;void*_tmp8D4;
struct Cyc_Absyn_Tqual _tmp8D5;struct Cyc_Absyn_Exp*_tmp8D6;union Cyc_Absyn_Constraint*
_tmp8D7;struct Cyc_Absyn_Enumdecl*_tmp8D9;void*_tmp8DD;_LL478: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp8C9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8C8;if(_tmp8C9->tag
!= 5)goto _LL47A;else{_tmp8CA=_tmp8C9->f1;_tmp8CB=_tmp8CA.elt_typ;_tmp8CC=_tmp8CA.elt_tq;
_tmp8CD=_tmp8CA.ptr_atts;_tmp8CE=_tmp8CD.rgn;_tmp8CF=_tmp8CD.nullable;_tmp8D0=
_tmp8CD.bounds;_tmp8D1=_tmp8CD.zero_term;}}_LL479:{void*_tmp8DE=t2;struct Cyc_Absyn_PtrInfo
_tmp8E0;void*_tmp8E1;struct Cyc_Absyn_Tqual _tmp8E2;struct Cyc_Absyn_PtrAtts _tmp8E3;
void*_tmp8E4;union Cyc_Absyn_Constraint*_tmp8E5;union Cyc_Absyn_Constraint*_tmp8E6;
union Cyc_Absyn_Constraint*_tmp8E7;_LL487: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp8DF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8DE;if(_tmp8DF->tag
!= 5)goto _LL489;else{_tmp8E0=_tmp8DF->f1;_tmp8E1=_tmp8E0.elt_typ;_tmp8E2=_tmp8E0.elt_tq;
_tmp8E3=_tmp8E0.ptr_atts;_tmp8E4=_tmp8E3.rgn;_tmp8E5=_tmp8E3.nullable;_tmp8E6=
_tmp8E3.bounds;_tmp8E7=_tmp8E3.zero_term;}}_LL488: {enum Cyc_Absyn_Coercion
coercion=Cyc_Absyn_Other_coercion;struct _tuple0*_tmp1336;struct Cyc_List_List*
_tmp1335;struct Cyc_List_List*_tmp8E8=(_tmp1335=_cycalloc(sizeof(*_tmp1335)),((
_tmp1335->hd=((_tmp1336=_cycalloc(sizeof(*_tmp1336)),((_tmp1336->f1=t1,((
_tmp1336->f2=t2,_tmp1336)))))),((_tmp1335->tl=0,_tmp1335)))));int _tmp8E9=_tmp8E2.real_const
 || !_tmp8CC.real_const;int _tmp8EA=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp8E6,Cyc_Absyn_bounds_one_conref) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(0,_tmp8E7);int _tmp8EB=_tmp8E9  && (((_tmp8EA  && Cyc_Tcutil_ptrsubtype(
te,_tmp8E8,_tmp8CB,_tmp8E1) || Cyc_Tcutil_unify(_tmp8CB,_tmp8E1)) || Cyc_Tcutil_isomorphic(
_tmp8CB,_tmp8E1)) || Cyc_Tcutil_unify(_tmp8E1,(void*)& Cyc_Absyn_VoidType_val));
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp8D1,_tmp8E7) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp8E7);int _tmp8EC=_tmp8EB?0:((Cyc_Tcutil_bits_only(_tmp8CB) && Cyc_Tcutil_is_char_type(
_tmp8E1)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp8E7)) && (_tmp8E2.real_const  || !_tmp8CC.real_const);int bounds_ok=((int(*)(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))
Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp8D0,_tmp8E6);if(!
bounds_ok  && !_tmp8EC){struct _tuple0 _tmp1337;struct _tuple0 _tmp8EE=(_tmp1337.f1=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8D0),((_tmp1337.f2=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8E6),_tmp1337)));
void*_tmp8EF;struct Cyc_Absyn_Exp*_tmp8F1;void*_tmp8F2;struct Cyc_Absyn_Exp*
_tmp8F4;_LL48C: _tmp8EF=_tmp8EE.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp8F0=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp8EF;if(_tmp8F0->tag != 
1)goto _LL48E;else{_tmp8F1=_tmp8F0->f1;}};_tmp8F2=_tmp8EE.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp8F3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp8F2;if(_tmp8F3->tag != 
1)goto _LL48E;else{_tmp8F4=_tmp8F3->f1;}};_LL48D: if(Cyc_Evexp_lte_const_exp(
_tmp8F4,_tmp8F1))bounds_ok=1;goto _LL48B;_LL48E:;_LL48F: bounds_ok=1;goto _LL48B;
_LL48B:;}if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp8CF) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp8E5))coercion=Cyc_Absyn_NonNull_to_Null;if(((bounds_ok  && zeroterm_ok) && (
_tmp8EB  || _tmp8EC)) && (Cyc_Tcutil_unify(_tmp8CE,_tmp8E4) || Cyc_Tcenv_region_outlives(
te,_tmp8CE,_tmp8E4)))return coercion;else{return Cyc_Absyn_Unknown_coercion;}};}
_LL489:;_LL48A: goto _LL486;_LL486:;}return Cyc_Absyn_Unknown_coercion;_LL47A: {
struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8D2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)
_tmp8C8;if(_tmp8D2->tag != 8)goto _LL47C;else{_tmp8D3=_tmp8D2->f1;_tmp8D4=_tmp8D3.elt_type;
_tmp8D5=_tmp8D3.tq;_tmp8D6=_tmp8D3.num_elts;_tmp8D7=_tmp8D3.zero_term;}}_LL47B:{
void*_tmp8F7=t2;struct Cyc_Absyn_ArrayInfo _tmp8F9;void*_tmp8FA;struct Cyc_Absyn_Tqual
_tmp8FB;struct Cyc_Absyn_Exp*_tmp8FC;union Cyc_Absyn_Constraint*_tmp8FD;_LL491: {
struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8F8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)
_tmp8F7;if(_tmp8F8->tag != 8)goto _LL493;else{_tmp8F9=_tmp8F8->f1;_tmp8FA=_tmp8F9.elt_type;
_tmp8FB=_tmp8F9.tq;_tmp8FC=_tmp8F9.num_elts;_tmp8FD=_tmp8F9.zero_term;}}_LL492: {
int okay;okay=((_tmp8D6 != 0  && _tmp8FC != 0) && ((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp8D7,_tmp8FD)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)
_tmp8FC,(struct Cyc_Absyn_Exp*)_tmp8D6);return(okay  && Cyc_Tcutil_unify(_tmp8D4,
_tmp8FA)) && (!_tmp8D5.real_const  || _tmp8FB.real_const)?Cyc_Absyn_Other_coercion:
Cyc_Absyn_Unknown_coercion;}_LL493:;_LL494: return Cyc_Absyn_Unknown_coercion;
_LL490:;}return Cyc_Absyn_Unknown_coercion;_LL47C: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp8D8=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp8C8;if(_tmp8D8->tag != 
13)goto _LL47E;else{_tmp8D9=_tmp8D8->f2;}}_LL47D:{void*_tmp8FE=t2;struct Cyc_Absyn_Enumdecl*
_tmp900;_LL496: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp8FF=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)
_tmp8FE;if(_tmp8FF->tag != 13)goto _LL498;else{_tmp900=_tmp8FF->f2;}}_LL497: if((
_tmp8D9->fields != 0  && _tmp900->fields != 0) && ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp8D9->fields))->v)
>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp900->fields))->v))return Cyc_Absyn_No_coercion;
goto _LL495;_LL498:;_LL499: goto _LL495;_LL495:;}goto _LL47F;_LL47E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp8DA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8C8;if(_tmp8DA->tag != 6)
goto _LL480;}_LL47F: goto _LL481;_LL480: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp8DB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp8C8;if(_tmp8DB->tag != 
7)goto _LL482;}_LL481: return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;
_LL482: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp8DC=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)
_tmp8C8;if(_tmp8DC->tag != 15)goto _LL484;else{_tmp8DD=(void*)_tmp8DC->f1;}}_LL483:{
void*_tmp901=t2;void*_tmp903;_LL49B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*
_tmp902=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp901;if(_tmp902->tag
!= 15)goto _LL49D;else{_tmp903=(void*)_tmp902->f1;}}_LL49C: if(Cyc_Tcenv_region_outlives(
te,_tmp8DD,_tmp903))return Cyc_Absyn_No_coercion;goto _LL49A;_LL49D:;_LL49E: goto
_LL49A;_LL49A:;}return Cyc_Absyn_Unknown_coercion;_LL484:;_LL485: return Cyc_Absyn_Unknown_coercion;
_LL477:;};}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t,enum Cyc_Absyn_Coercion c){if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),
t)){struct Cyc_Absyn_Exp*_tmp904=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct
_tmp133A;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1339;e->r=(void*)((
_tmp1339=_cycalloc(sizeof(*_tmp1339)),((_tmp1339[0]=((_tmp133A.tag=13,((_tmp133A.f1=(
void*)t,((_tmp133A.f2=_tmp904,((_tmp133A.f3=0,((_tmp133A.f4=c,_tmp133A)))))))))),
_tmp1339))));}e->topt=(void*)t;}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp907=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL4A0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp908=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp907;if(_tmp908->tag != 6)
goto _LL4A2;}_LL4A1: goto _LL4A3;_LL4A2: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp909=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp907;if(_tmp909->tag != 
13)goto _LL4A4;}_LL4A3: goto _LL4A5;_LL4A4: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*
_tmp90A=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp907;if(_tmp90A->tag
!= 14)goto _LL4A6;}_LL4A5: goto _LL4A7;_LL4A6: {struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmp90B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp907;if(_tmp90B->tag != 19)
goto _LL4A8;}_LL4A7: return 1;_LL4A8: {struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp90C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp907;if(_tmp90C->tag != 1)
goto _LL4AA;}_LL4A9: return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);
_LL4AA:;_LL4AB: return 0;_LL49F:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))return 1;{void*_tmp90D=Cyc_Tcutil_compress((void*)
_check_null(e->topt));_LL4AD: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp90E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp90D;if(_tmp90E->tag != 
7)goto _LL4AF;}_LL4AE: return 1;_LL4AF:;_LL4B0: return 0;_LL4AC:;};}int Cyc_Tcutil_is_function_type(
void*t){void*_tmp90F=Cyc_Tcutil_compress(t);_LL4B2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp910=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp90F;if(_tmp910->tag != 9)
goto _LL4B4;}_LL4B3: return 1;_LL4B4:;_LL4B5: return 0;_LL4B1:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2){struct _tuple0 _tmp133B;struct _tuple0 _tmp912=(_tmp133B.f1=t1,((
_tmp133B.f2=t2,_tmp133B)));void*_tmp913;int _tmp915;void*_tmp916;int _tmp918;void*
_tmp919;void*_tmp91B;void*_tmp91D;enum Cyc_Absyn_Sign _tmp91F;enum Cyc_Absyn_Size_of
_tmp920;void*_tmp921;enum Cyc_Absyn_Sign _tmp923;enum Cyc_Absyn_Size_of _tmp924;
void*_tmp925;enum Cyc_Absyn_Size_of _tmp927;void*_tmp928;enum Cyc_Absyn_Size_of
_tmp92A;void*_tmp92B;enum Cyc_Absyn_Sign _tmp92D;enum Cyc_Absyn_Size_of _tmp92E;
void*_tmp92F;enum Cyc_Absyn_Sign _tmp931;enum Cyc_Absyn_Size_of _tmp932;void*
_tmp933;enum Cyc_Absyn_Sign _tmp935;enum Cyc_Absyn_Size_of _tmp936;void*_tmp937;
enum Cyc_Absyn_Sign _tmp939;enum Cyc_Absyn_Size_of _tmp93A;void*_tmp93B;enum Cyc_Absyn_Size_of
_tmp93D;void*_tmp93E;enum Cyc_Absyn_Size_of _tmp940;_LL4B7: _tmp913=_tmp912.f1;{
struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp914=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)
_tmp913;if(_tmp914->tag != 7)goto _LL4B9;else{_tmp915=_tmp914->f1;}};_tmp916=
_tmp912.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp917=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)
_tmp916;if(_tmp917->tag != 7)goto _LL4B9;else{_tmp918=_tmp917->f1;}};_LL4B8: if(
_tmp915 != 0  && _tmp915 != 1)return t1;else{if(_tmp918 != 0  && _tmp918 != 1)return t2;
else{if(_tmp915 >= _tmp918)return t1;else{return t2;}}}_LL4B9: _tmp919=_tmp912.f1;{
struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp91A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)
_tmp919;if(_tmp91A->tag != 7)goto _LL4BB;};_LL4BA: return t1;_LL4BB: _tmp91B=_tmp912.f2;{
struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp91C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)
_tmp91B;if(_tmp91C->tag != 7)goto _LL4BD;};_LL4BC: return t2;_LL4BD: _tmp91D=_tmp912.f1;{
struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp91E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp91D;if(_tmp91E->tag != 6)goto _LL4BF;else{_tmp91F=_tmp91E->f1;if(_tmp91F != Cyc_Absyn_Unsigned)
goto _LL4BF;_tmp920=_tmp91E->f2;if(_tmp920 != Cyc_Absyn_LongLong_sz)goto _LL4BF;}};
_LL4BE: goto _LL4C0;_LL4BF: _tmp921=_tmp912.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp922=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp921;if(_tmp922->tag != 6)
goto _LL4C1;else{_tmp923=_tmp922->f1;if(_tmp923 != Cyc_Absyn_Unsigned)goto _LL4C1;
_tmp924=_tmp922->f2;if(_tmp924 != Cyc_Absyn_LongLong_sz)goto _LL4C1;}};_LL4C0:
return Cyc_Absyn_ulonglong_typ;_LL4C1: _tmp925=_tmp912.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp926=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp925;if(_tmp926->tag != 6)
goto _LL4C3;else{_tmp927=_tmp926->f2;if(_tmp927 != Cyc_Absyn_LongLong_sz)goto
_LL4C3;}};_LL4C2: goto _LL4C4;_LL4C3: _tmp928=_tmp912.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp929=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp928;if(_tmp929->tag != 6)
goto _LL4C5;else{_tmp92A=_tmp929->f2;if(_tmp92A != Cyc_Absyn_LongLong_sz)goto
_LL4C5;}};_LL4C4: return Cyc_Absyn_slonglong_typ;_LL4C5: _tmp92B=_tmp912.f1;{struct
Cyc_Absyn_IntType_Absyn_Type_struct*_tmp92C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp92B;if(_tmp92C->tag != 6)goto _LL4C7;else{_tmp92D=_tmp92C->f1;if(_tmp92D != Cyc_Absyn_Unsigned)
goto _LL4C7;_tmp92E=_tmp92C->f2;if(_tmp92E != Cyc_Absyn_Long_sz)goto _LL4C7;}};
_LL4C6: goto _LL4C8;_LL4C7: _tmp92F=_tmp912.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp930=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp92F;if(_tmp930->tag != 6)
goto _LL4C9;else{_tmp931=_tmp930->f1;if(_tmp931 != Cyc_Absyn_Unsigned)goto _LL4C9;
_tmp932=_tmp930->f2;if(_tmp932 != Cyc_Absyn_Long_sz)goto _LL4C9;}};_LL4C8: return
Cyc_Absyn_ulong_typ;_LL4C9: _tmp933=_tmp912.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp934=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp933;if(_tmp934->tag != 6)
goto _LL4CB;else{_tmp935=_tmp934->f1;if(_tmp935 != Cyc_Absyn_Unsigned)goto _LL4CB;
_tmp936=_tmp934->f2;if(_tmp936 != Cyc_Absyn_Int_sz)goto _LL4CB;}};_LL4CA: goto
_LL4CC;_LL4CB: _tmp937=_tmp912.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp938=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp937;if(_tmp938->tag != 6)
goto _LL4CD;else{_tmp939=_tmp938->f1;if(_tmp939 != Cyc_Absyn_Unsigned)goto _LL4CD;
_tmp93A=_tmp938->f2;if(_tmp93A != Cyc_Absyn_Int_sz)goto _LL4CD;}};_LL4CC: return Cyc_Absyn_uint_typ;
_LL4CD: _tmp93B=_tmp912.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp93C=(
struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp93B;if(_tmp93C->tag != 6)goto
_LL4CF;else{_tmp93D=_tmp93C->f2;if(_tmp93D != Cyc_Absyn_Long_sz)goto _LL4CF;}};
_LL4CE: goto _LL4D0;_LL4CF: _tmp93E=_tmp912.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp93F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp93E;if(_tmp93F->tag != 6)
goto _LL4D1;else{_tmp940=_tmp93F->f2;if(_tmp940 != Cyc_Absyn_Long_sz)goto _LL4D1;}};
_LL4D0: return Cyc_Absyn_slong_typ;_LL4D1:;_LL4D2: return Cyc_Absyn_sint_typ;_LL4B6:;}
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){void*_tmp941=e->r;
struct Cyc_Core_Opt*_tmp943;_LL4D4: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*
_tmp942=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp941;if(_tmp942->tag
!= 3)goto _LL4D6;else{_tmp943=_tmp942->f2;if(_tmp943 != 0)goto _LL4D6;}}_LL4D5:{
const char*_tmp133E;void*_tmp133D;(_tmp133D=0,Cyc_Tcutil_warn(e->loc,((_tmp133E="assignment in test",
_tag_dyneither(_tmp133E,sizeof(char),19))),_tag_dyneither(_tmp133D,sizeof(void*),
0)));}goto _LL4D3;_LL4D6:;_LL4D7: goto _LL4D3;_LL4D3:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple0 _tmp133F;struct _tuple0 _tmp947=(_tmp133F.f1=c1,((_tmp133F.f2=c2,
_tmp133F)));void*_tmp948;struct Cyc_Absyn_Kind*_tmp94A;void*_tmp94B;struct Cyc_Absyn_Kind*
_tmp94D;void*_tmp94E;struct Cyc_Core_Opt*_tmp950;struct Cyc_Core_Opt**_tmp951;void*
_tmp952;struct Cyc_Core_Opt*_tmp954;struct Cyc_Core_Opt**_tmp955;void*_tmp956;
struct Cyc_Core_Opt*_tmp958;struct Cyc_Core_Opt**_tmp959;struct Cyc_Absyn_Kind*
_tmp95A;void*_tmp95B;struct Cyc_Absyn_Kind*_tmp95D;void*_tmp95E;struct Cyc_Absyn_Kind*
_tmp960;void*_tmp961;struct Cyc_Core_Opt*_tmp963;struct Cyc_Core_Opt**_tmp964;
struct Cyc_Absyn_Kind*_tmp965;void*_tmp966;struct Cyc_Core_Opt*_tmp968;struct Cyc_Core_Opt**
_tmp969;struct Cyc_Absyn_Kind*_tmp96A;void*_tmp96B;struct Cyc_Core_Opt*_tmp96D;
struct Cyc_Core_Opt**_tmp96E;struct Cyc_Absyn_Kind*_tmp96F;_LL4D9: _tmp948=_tmp947.f1;{
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp949=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)
_tmp948;if(_tmp949->tag != 0)goto _LL4DB;else{_tmp94A=_tmp949->f1;}};_tmp94B=
_tmp947.f2;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp94C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)
_tmp94B;if(_tmp94C->tag != 0)goto _LL4DB;else{_tmp94D=_tmp94C->f1;}};_LL4DA: return
_tmp94A == _tmp94D;_LL4DB: _tmp94E=_tmp947.f2;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*
_tmp94F=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp94E;if(_tmp94F->tag
!= 1)goto _LL4DD;else{_tmp950=_tmp94F->f1;_tmp951=(struct Cyc_Core_Opt**)& _tmp94F->f1;}};
_LL4DC:{struct Cyc_Core_Opt*_tmp1340;*_tmp951=((_tmp1340=_cycalloc(sizeof(*
_tmp1340)),((_tmp1340->v=c1,_tmp1340))));}return 1;_LL4DD: _tmp952=_tmp947.f1;{
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp953=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)
_tmp952;if(_tmp953->tag != 1)goto _LL4DF;else{_tmp954=_tmp953->f1;_tmp955=(struct
Cyc_Core_Opt**)& _tmp953->f1;}};_LL4DE:{struct Cyc_Core_Opt*_tmp1341;*_tmp955=((
_tmp1341=_cycalloc(sizeof(*_tmp1341)),((_tmp1341->v=c2,_tmp1341))));}return 1;
_LL4DF: _tmp956=_tmp947.f1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmp957=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp956;if(_tmp957->tag
!= 2)goto _LL4E1;else{_tmp958=_tmp957->f1;_tmp959=(struct Cyc_Core_Opt**)& _tmp957->f1;
_tmp95A=_tmp957->f2;}};_tmp95B=_tmp947.f2;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*
_tmp95C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp95B;if(_tmp95C->tag != 
0)goto _LL4E1;else{_tmp95D=_tmp95C->f1;}};_LL4E0: if(Cyc_Tcutil_kind_leq(_tmp95D,
_tmp95A)){{struct Cyc_Core_Opt*_tmp1342;*_tmp959=((_tmp1342=_cycalloc(sizeof(*
_tmp1342)),((_tmp1342->v=c2,_tmp1342))));}return 1;}else{return 0;}_LL4E1: _tmp95E=
_tmp947.f1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp95F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)
_tmp95E;if(_tmp95F->tag != 0)goto _LL4E3;else{_tmp960=_tmp95F->f1;}};_tmp961=
_tmp947.f2;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp962=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)
_tmp961;if(_tmp962->tag != 2)goto _LL4E3;else{_tmp963=_tmp962->f1;_tmp964=(struct
Cyc_Core_Opt**)& _tmp962->f1;_tmp965=_tmp962->f2;}};_LL4E2: if(Cyc_Tcutil_kind_leq(
_tmp960,_tmp965)){{struct Cyc_Core_Opt*_tmp1343;*_tmp964=((_tmp1343=_cycalloc(
sizeof(*_tmp1343)),((_tmp1343->v=c1,_tmp1343))));}return 1;}else{return 0;}_LL4E3:
_tmp966=_tmp947.f1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp967=(
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp966;if(_tmp967->tag != 2)goto
_LL4D8;else{_tmp968=_tmp967->f1;_tmp969=(struct Cyc_Core_Opt**)& _tmp967->f1;
_tmp96A=_tmp967->f2;}};_tmp96B=_tmp947.f2;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmp96C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp96B;if(_tmp96C->tag
!= 2)goto _LL4D8;else{_tmp96D=_tmp96C->f1;_tmp96E=(struct Cyc_Core_Opt**)& _tmp96C->f1;
_tmp96F=_tmp96C->f2;}};_LL4E4: if(Cyc_Tcutil_kind_leq(_tmp96A,_tmp96F)){{struct
Cyc_Core_Opt*_tmp1344;*_tmp96E=((_tmp1344=_cycalloc(sizeof(*_tmp1344)),((
_tmp1344->v=c1,_tmp1344))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp96F,
_tmp96A)){{struct Cyc_Core_Opt*_tmp1345;*_tmp969=((_tmp1345=_cycalloc(sizeof(*
_tmp1345)),((_tmp1345->v=c2,_tmp1345))));}return 1;}else{return 0;}}_LL4D8:;};}
static int Cyc_Tcutil_tvar_id_counter=0;int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter
++;}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;const char*_tmp1349;void*_tmp1348[1];
struct Cyc_Int_pa_PrintArg_struct _tmp1347;struct _dyneither_ptr s=(struct
_dyneither_ptr)((_tmp1347.tag=1,((_tmp1347.f1=(unsigned long)i,((_tmp1348[0]=&
_tmp1347,Cyc_aprintf(((_tmp1349="#%d",_tag_dyneither(_tmp1349,sizeof(char),4))),
_tag_dyneither(_tmp1348,sizeof(void*),1))))))));struct _dyneither_ptr*_tmp134C;
struct Cyc_Absyn_Tvar*_tmp134B;return(_tmp134B=_cycalloc(sizeof(*_tmp134B)),((
_tmp134B->name=((_tmp134C=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp134C[
0]=s,_tmp134C)))),((_tmp134B->identity=- 1,((_tmp134B->kind=k,_tmp134B)))))));}
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){struct _dyneither_ptr _tmp97B=*
t->name;return*((const char*)_check_dyneither_subscript(_tmp97B,sizeof(char),0))
== '#';}void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){{const char*
_tmp1350;void*_tmp134F[1];struct Cyc_String_pa_PrintArg_struct _tmp134E;(_tmp134E.tag=
0,((_tmp134E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((
_tmp134F[0]=& _tmp134E,Cyc_printf(((_tmp1350="%s",_tag_dyneither(_tmp1350,sizeof(
char),3))),_tag_dyneither(_tmp134F,sizeof(void*),1)))))));}if(!Cyc_Tcutil_is_temp_tvar(
t))return;{const char*_tmp1351;struct _dyneither_ptr _tmp97F=Cyc_strconcat(((
_tmp1351="`",_tag_dyneither(_tmp1351,sizeof(char),2))),(struct _dyneither_ptr)*t->name);{
char _tmp1354;char _tmp1353;struct _dyneither_ptr _tmp1352;(_tmp1352=
_dyneither_ptr_plus(_tmp97F,sizeof(char),1),((_tmp1353=*((char*)
_check_dyneither_subscript(_tmp1352,sizeof(char),0)),((_tmp1354='t',((
_get_dyneither_size(_tmp1352,sizeof(char))== 1  && (_tmp1353 == '\000'  && _tmp1354
!= '\000')?_throw_arraybounds(): 1,*((char*)_tmp1352.curr)=_tmp1354)))))));}{
struct _dyneither_ptr*_tmp1355;t->name=((_tmp1355=_cycalloc(sizeof(struct
_dyneither_ptr)* 1),((_tmp1355[0]=(struct _dyneither_ptr)_tmp97F,_tmp1355))));};};}
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){struct _tuple10*
_tmp1356;return(_tmp1356=_cycalloc(sizeof(*_tmp1356)),((_tmp1356->f1=(struct
_dyneither_ptr*)(*x).f1,((_tmp1356->f2=(*x).f2,((_tmp1356->f3=(*x).f3,_tmp1356)))))));}
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ == 0){
struct Cyc_List_List*_tmp986=0;{struct Cyc_List_List*atts=fd->attributes;for(0;
atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){struct Cyc_List_List*
_tmp1357;_tmp986=((_tmp1357=_cycalloc(sizeof(*_tmp1357)),((_tmp1357->hd=(void*)
atts->hd,((_tmp1357->tl=_tmp986,_tmp1357))))));}}}{struct Cyc_Absyn_FnType_Absyn_Type_struct
_tmp135D;struct Cyc_Absyn_FnInfo _tmp135C;struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp135B;return(void*)((_tmp135B=_cycalloc(sizeof(*_tmp135B)),((_tmp135B[0]=((
_tmp135D.tag=9,((_tmp135D.f1=((_tmp135C.tvars=fd->tvs,((_tmp135C.effect=fd->effect,((
_tmp135C.ret_tqual=fd->ret_tqual,((_tmp135C.ret_typ=fd->ret_type,((_tmp135C.args=((
struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp135C.c_varargs=fd->c_varargs,((
_tmp135C.cyc_varargs=fd->cyc_varargs,((_tmp135C.rgn_po=fd->rgn_po,((_tmp135C.attributes=
_tmp986,_tmp135C)))))))))))))))))),_tmp135D)))),_tmp135B))));};}return(void*)
_check_null(fd->cached_typ);}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*
f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(
struct _tuple12*t){return(*t).f2;}static struct _tuple12*Cyc_Tcutil_map2_tq(struct
_tuple12*pr,void*t){struct _tuple12 _tmp98C;struct Cyc_Absyn_Tqual _tmp98D;void*
_tmp98E;struct _tuple12*_tmp98B=pr;_tmp98C=*_tmp98B;_tmp98D=_tmp98C.f1;_tmp98E=
_tmp98C.f2;if(_tmp98E == t)return pr;else{struct _tuple12*_tmp135E;return(_tmp135E=
_cycalloc(sizeof(*_tmp135E)),((_tmp135E->f1=_tmp98D,((_tmp135E->f2=t,_tmp135E)))));}}
struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{
struct _tuple26*f1;void*f2;};static struct _tuple27*Cyc_Tcutil_substitute_f1(struct
_RegionHandle*rgn,struct _tuple10*y){struct _tuple26*_tmp1361;struct _tuple27*
_tmp1360;return(_tmp1360=_region_malloc(rgn,sizeof(*_tmp1360)),((_tmp1360->f1=((
_tmp1361=_region_malloc(rgn,sizeof(*_tmp1361)),((_tmp1361->f1=(*y).f1,((_tmp1361->f2=(*
y).f2,_tmp1361)))))),((_tmp1360->f2=(*y).f3,_tmp1360)))));}static struct _tuple10*
Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){struct _dyneither_ptr*
_tmp993;struct Cyc_Absyn_Tqual _tmp994;void*_tmp995;struct _tuple10 _tmp992=*
orig_arg;_tmp993=_tmp992.f1;_tmp994=_tmp992.f2;_tmp995=_tmp992.f3;if(t == _tmp995)
return orig_arg;{struct _tuple10*_tmp1362;return(_tmp1362=_cycalloc(sizeof(*
_tmp1362)),((_tmp1362->f1=_tmp993,((_tmp1362->f2=_tmp994,((_tmp1362->f3=t,
_tmp1362)))))));};}static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(struct
Cyc_Absyn_Aggrfield*f,void*t){if(f->type == t)return f;{struct Cyc_Absyn_Aggrfield*
_tmp1363;return(_tmp1363=_cycalloc(sizeof(*_tmp1363)),((_tmp1363->name=f->name,((
_tmp1363->tq=f->tq,((_tmp1363->type=t,((_tmp1363->width=f->width,((_tmp1363->attributes=
f->attributes,_tmp1363)))))))))));};}static struct Cyc_List_List*Cyc_Tcutil_substs(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static
struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*
_tmp1364;return(_tmp1364=_cycalloc(sizeof(*_tmp1364)),((_tmp1364->topt=old->topt,((
_tmp1364->r=r,((_tmp1364->loc=old->loc,((_tmp1364->annot=old->annot,_tmp1364)))))))));}
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*
inst,struct Cyc_Absyn_Exp*e){void*_tmp999=e->r;enum Cyc_Absyn_Primop _tmp99F;
struct Cyc_List_List*_tmp9A0;struct Cyc_Absyn_Exp*_tmp9A2;struct Cyc_Absyn_Exp*
_tmp9A3;struct Cyc_Absyn_Exp*_tmp9A4;struct Cyc_Absyn_Exp*_tmp9A6;struct Cyc_Absyn_Exp*
_tmp9A7;struct Cyc_Absyn_Exp*_tmp9A9;struct Cyc_Absyn_Exp*_tmp9AA;struct Cyc_Absyn_Exp*
_tmp9AC;struct Cyc_Absyn_Exp*_tmp9AD;void*_tmp9AF;struct Cyc_Absyn_Exp*_tmp9B0;int
_tmp9B1;enum Cyc_Absyn_Coercion _tmp9B2;void*_tmp9B4;struct Cyc_Absyn_Exp*_tmp9B6;
void*_tmp9B8;void*_tmp9B9;void*_tmp9BB;_LL4E6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp99A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp99A->tag != 
0)goto _LL4E8;}_LL4E7: goto _LL4E9;_LL4E8: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*
_tmp99B=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp99B->tag != 
30)goto _LL4EA;}_LL4E9: goto _LL4EB;_LL4EA: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*
_tmp99C=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp99C->tag
!= 31)goto _LL4EC;}_LL4EB: goto _LL4ED;_LL4EC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp99D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp99D->tag != 1)
goto _LL4EE;}_LL4ED: return e;_LL4EE: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*
_tmp99E=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp99E->tag
!= 2)goto _LL4F0;else{_tmp99F=_tmp99E->f1;_tmp9A0=_tmp99E->f2;}}_LL4EF: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp9A0)== 1){struct Cyc_Absyn_Exp*
_tmp9BC=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp9A0))->hd;
struct Cyc_Absyn_Exp*_tmp9BD=Cyc_Tcutil_rsubsexp(r,inst,_tmp9BC);if(_tmp9BD == 
_tmp9BC)return e;{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp136A;struct Cyc_Absyn_Exp*
_tmp1369[1];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1368;return Cyc_Tcutil_copye(
e,(void*)((_tmp1368=_cycalloc(sizeof(*_tmp1368)),((_tmp1368[0]=((_tmp136A.tag=2,((
_tmp136A.f1=_tmp99F,((_tmp136A.f2=((_tmp1369[0]=_tmp9BD,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1369,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmp136A)))))),_tmp1368)))));};}else{if(((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp9A0)== 2){struct Cyc_Absyn_Exp*_tmp9C1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp9A0))->hd;struct Cyc_Absyn_Exp*_tmp9C2=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp9A0->tl))->hd;struct
Cyc_Absyn_Exp*_tmp9C3=Cyc_Tcutil_rsubsexp(r,inst,_tmp9C1);struct Cyc_Absyn_Exp*
_tmp9C4=Cyc_Tcutil_rsubsexp(r,inst,_tmp9C2);if(_tmp9C3 == _tmp9C1  && _tmp9C4 == 
_tmp9C2)return e;{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1370;struct Cyc_Absyn_Exp*
_tmp136F[2];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp136E;return Cyc_Tcutil_copye(
e,(void*)((_tmp136E=_cycalloc(sizeof(*_tmp136E)),((_tmp136E[0]=((_tmp1370.tag=2,((
_tmp1370.f1=_tmp99F,((_tmp1370.f2=((_tmp136F[1]=_tmp9C4,((_tmp136F[0]=_tmp9C3,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp136F,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp1370)))))),_tmp136E)))));};}
else{const char*_tmp1373;void*_tmp1372;return(_tmp1372=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1373="primop does not have 1 or 2 args!",
_tag_dyneither(_tmp1373,sizeof(char),34))),_tag_dyneither(_tmp1372,sizeof(void*),
0)));}}_LL4F0: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp9A1=(
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9A1->tag != 5)
goto _LL4F2;else{_tmp9A2=_tmp9A1->f1;_tmp9A3=_tmp9A1->f2;_tmp9A4=_tmp9A1->f3;}}
_LL4F1: {struct Cyc_Absyn_Exp*_tmp9CA=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A2);struct
Cyc_Absyn_Exp*_tmp9CB=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A3);struct Cyc_Absyn_Exp*
_tmp9CC=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A4);if((_tmp9CA == _tmp9A2  && _tmp9CB == 
_tmp9A3) && _tmp9CC == _tmp9A4)return e;{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct
_tmp1376;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1375;return Cyc_Tcutil_copye(
e,(void*)((_tmp1375=_cycalloc(sizeof(*_tmp1375)),((_tmp1375[0]=((_tmp1376.tag=5,((
_tmp1376.f1=_tmp9CA,((_tmp1376.f2=_tmp9CB,((_tmp1376.f3=_tmp9CC,_tmp1376)))))))),
_tmp1375)))));};}_LL4F2: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp9A5=(
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9A5->tag != 6)goto
_LL4F4;else{_tmp9A6=_tmp9A5->f1;_tmp9A7=_tmp9A5->f2;}}_LL4F3: {struct Cyc_Absyn_Exp*
_tmp9CF=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A6);struct Cyc_Absyn_Exp*_tmp9D0=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9A7);if(_tmp9CF == _tmp9A6  && _tmp9D0 == _tmp9A7)return e;{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct
_tmp1379;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1378;return Cyc_Tcutil_copye(
e,(void*)((_tmp1378=_cycalloc(sizeof(*_tmp1378)),((_tmp1378[0]=((_tmp1379.tag=6,((
_tmp1379.f1=_tmp9CF,((_tmp1379.f2=_tmp9D0,_tmp1379)))))),_tmp1378)))));};}_LL4F4: {
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp9A8=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)
_tmp999;if(_tmp9A8->tag != 7)goto _LL4F6;else{_tmp9A9=_tmp9A8->f1;_tmp9AA=_tmp9A8->f2;}}
_LL4F5: {struct Cyc_Absyn_Exp*_tmp9D3=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A9);struct
Cyc_Absyn_Exp*_tmp9D4=Cyc_Tcutil_rsubsexp(r,inst,_tmp9AA);if(_tmp9D3 == _tmp9A9
 && _tmp9D4 == _tmp9AA)return e;{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp137C;
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp137B;return Cyc_Tcutil_copye(e,(
void*)((_tmp137B=_cycalloc(sizeof(*_tmp137B)),((_tmp137B[0]=((_tmp137C.tag=7,((
_tmp137C.f1=_tmp9D3,((_tmp137C.f2=_tmp9D4,_tmp137C)))))),_tmp137B)))));};}_LL4F6: {
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp9AB=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)
_tmp999;if(_tmp9AB->tag != 8)goto _LL4F8;else{_tmp9AC=_tmp9AB->f1;_tmp9AD=_tmp9AB->f2;}}
_LL4F7: {struct Cyc_Absyn_Exp*_tmp9D7=Cyc_Tcutil_rsubsexp(r,inst,_tmp9AC);struct
Cyc_Absyn_Exp*_tmp9D8=Cyc_Tcutil_rsubsexp(r,inst,_tmp9AD);if(_tmp9D7 == _tmp9AC
 && _tmp9D8 == _tmp9AD)return e;{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct
_tmp137F;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp137E;return Cyc_Tcutil_copye(
e,(void*)((_tmp137E=_cycalloc(sizeof(*_tmp137E)),((_tmp137E[0]=((_tmp137F.tag=8,((
_tmp137F.f1=_tmp9D7,((_tmp137F.f2=_tmp9D8,_tmp137F)))))),_tmp137E)))));};}_LL4F8: {
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp9AE=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)
_tmp999;if(_tmp9AE->tag != 13)goto _LL4FA;else{_tmp9AF=(void*)_tmp9AE->f1;_tmp9B0=
_tmp9AE->f2;_tmp9B1=_tmp9AE->f3;_tmp9B2=_tmp9AE->f4;}}_LL4F9: {struct Cyc_Absyn_Exp*
_tmp9DB=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B0);void*_tmp9DC=Cyc_Tcutil_rsubstitute(
r,inst,_tmp9AF);if(_tmp9DB == _tmp9B0  && _tmp9DC == _tmp9AF)return e;{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct
_tmp1382;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1381;return Cyc_Tcutil_copye(
e,(void*)((_tmp1381=_cycalloc(sizeof(*_tmp1381)),((_tmp1381[0]=((_tmp1382.tag=13,((
_tmp1382.f1=(void*)_tmp9DC,((_tmp1382.f2=_tmp9DB,((_tmp1382.f3=_tmp9B1,((
_tmp1382.f4=_tmp9B2,_tmp1382)))))))))),_tmp1381)))));};}_LL4FA: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*
_tmp9B3=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9B3->tag
!= 16)goto _LL4FC;else{_tmp9B4=(void*)_tmp9B3->f1;}}_LL4FB: {void*_tmp9DF=Cyc_Tcutil_rsubstitute(
r,inst,_tmp9B4);if(_tmp9DF == _tmp9B4)return e;{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct
_tmp1385;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp1384;return Cyc_Tcutil_copye(
e,(void*)((_tmp1384=_cycalloc(sizeof(*_tmp1384)),((_tmp1384[0]=((_tmp1385.tag=16,((
_tmp1385.f1=(void*)_tmp9DF,_tmp1385)))),_tmp1384)))));};}_LL4FC: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*
_tmp9B5=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9B5->tag
!= 17)goto _LL4FE;else{_tmp9B6=_tmp9B5->f1;}}_LL4FD: {struct Cyc_Absyn_Exp*_tmp9E2=
Cyc_Tcutil_rsubsexp(r,inst,_tmp9B6);if(_tmp9E2 == _tmp9B6)return e;{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct
_tmp1388;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp1387;return Cyc_Tcutil_copye(
e,(void*)((_tmp1387=_cycalloc(sizeof(*_tmp1387)),((_tmp1387[0]=((_tmp1388.tag=17,((
_tmp1388.f1=_tmp9E2,_tmp1388)))),_tmp1387)))));};}_LL4FE: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*
_tmp9B7=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9B7->tag
!= 18)goto _LL500;else{_tmp9B8=(void*)_tmp9B7->f1;_tmp9B9=(void*)_tmp9B7->f2;}}
_LL4FF: {void*_tmp9E5=Cyc_Tcutil_rsubstitute(r,inst,_tmp9B8);if(_tmp9E5 == 
_tmp9B8)return e;{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp138B;struct
Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp138A;return Cyc_Tcutil_copye(e,(
void*)((_tmp138A=_cycalloc(sizeof(*_tmp138A)),((_tmp138A[0]=((_tmp138B.tag=18,((
_tmp138B.f1=(void*)_tmp9E5,((_tmp138B.f2=(void*)_tmp9B9,_tmp138B)))))),_tmp138A)))));};}
_LL500: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp9BA=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)
_tmp999;if(_tmp9BA->tag != 37)goto _LL502;else{_tmp9BB=(void*)_tmp9BA->f1;}}_LL501: {
void*_tmp9E8=Cyc_Tcutil_rsubstitute(r,inst,_tmp9BB);if(_tmp9E8 == _tmp9BB)return e;{
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp138E;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*
_tmp138D;return Cyc_Tcutil_copye(e,(void*)((_tmp138D=_cycalloc(sizeof(*_tmp138D)),((
_tmp138D[0]=((_tmp138E.tag=37,((_tmp138E.f1=(void*)_tmp9E8,_tmp138E)))),_tmp138D)))));};}
_LL502:;_LL503: {const char*_tmp1391;void*_tmp1390;return(_tmp1390=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1391="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmp1391,sizeof(char),46))),_tag_dyneither(_tmp1390,sizeof(void*),
0)));}_LL4E5:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp9ED=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp9EF;
struct Cyc_Absyn_AggrInfo _tmp9F1;union Cyc_Absyn_AggrInfoU _tmp9F2;struct Cyc_List_List*
_tmp9F3;struct Cyc_Absyn_DatatypeInfo _tmp9F5;union Cyc_Absyn_DatatypeInfoU _tmp9F6;
struct Cyc_List_List*_tmp9F7;struct Cyc_Absyn_DatatypeFieldInfo _tmp9F9;union Cyc_Absyn_DatatypeFieldInfoU
_tmp9FA;struct Cyc_List_List*_tmp9FB;struct _tuple2*_tmp9FD;struct Cyc_List_List*
_tmp9FE;struct Cyc_Absyn_Typedefdecl*_tmp9FF;void*_tmpA00;struct Cyc_Absyn_ArrayInfo
_tmpA02;void*_tmpA03;struct Cyc_Absyn_Tqual _tmpA04;struct Cyc_Absyn_Exp*_tmpA05;
union Cyc_Absyn_Constraint*_tmpA06;unsigned int _tmpA07;struct Cyc_Absyn_PtrInfo
_tmpA09;void*_tmpA0A;struct Cyc_Absyn_Tqual _tmpA0B;struct Cyc_Absyn_PtrAtts _tmpA0C;
void*_tmpA0D;union Cyc_Absyn_Constraint*_tmpA0E;union Cyc_Absyn_Constraint*_tmpA0F;
union Cyc_Absyn_Constraint*_tmpA10;struct Cyc_Absyn_FnInfo _tmpA12;struct Cyc_List_List*
_tmpA13;void*_tmpA14;struct Cyc_Absyn_Tqual _tmpA15;void*_tmpA16;struct Cyc_List_List*
_tmpA17;int _tmpA18;struct Cyc_Absyn_VarargInfo*_tmpA19;struct Cyc_List_List*
_tmpA1A;struct Cyc_List_List*_tmpA1B;struct Cyc_List_List*_tmpA1D;enum Cyc_Absyn_AggrKind
_tmpA1F;struct Cyc_List_List*_tmpA20;void*_tmpA22;void*_tmpA24;void*_tmpA26;void*
_tmpA27;void*_tmpA29;struct Cyc_Absyn_Exp*_tmpA2B;void*_tmpA35;void*_tmpA37;
struct Cyc_List_List*_tmpA39;_LL505: {struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp9EE=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9ED;if(_tmp9EE->tag != 2)
goto _LL507;else{_tmp9EF=_tmp9EE->f1;}}_LL506: {struct _handler_cons _tmpA3B;
_push_handler(& _tmpA3B);{int _tmpA3D=0;if(setjmp(_tmpA3B.handler))_tmpA3D=1;if(!
_tmpA3D){{void*_tmpA3E=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,
inst,_tmp9EF);_npop_handler(0);return _tmpA3E;};_pop_handler();}else{void*_tmpA3C=(
void*)_exn_thrown;void*_tmpA40=_tmpA3C;_LL53C: {struct Cyc_Core_Not_found_exn_struct*
_tmpA41=(struct Cyc_Core_Not_found_exn_struct*)_tmpA40;if(_tmpA41->tag != Cyc_Core_Not_found)
goto _LL53E;}_LL53D: return t;_LL53E:;_LL53F:(void)_throw(_tmpA40);_LL53B:;}};}
_LL507: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp9F0=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp9ED;if(_tmp9F0->tag != 11)goto _LL509;else{_tmp9F1=_tmp9F0->f1;_tmp9F2=_tmp9F1.aggr_info;
_tmp9F3=_tmp9F1.targs;}}_LL508: {struct Cyc_List_List*_tmpA42=Cyc_Tcutil_substs(
rgn,inst,_tmp9F3);struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp1397;struct Cyc_Absyn_AggrInfo
_tmp1396;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1395;return _tmpA42 == 
_tmp9F3?t:(void*)((_tmp1395=_cycalloc(sizeof(*_tmp1395)),((_tmp1395[0]=((
_tmp1397.tag=11,((_tmp1397.f1=((_tmp1396.aggr_info=_tmp9F2,((_tmp1396.targs=
_tmpA42,_tmp1396)))),_tmp1397)))),_tmp1395))));}_LL509: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*
_tmp9F4=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp9ED;if(_tmp9F4->tag
!= 3)goto _LL50B;else{_tmp9F5=_tmp9F4->f1;_tmp9F6=_tmp9F5.datatype_info;_tmp9F7=
_tmp9F5.targs;}}_LL50A: {struct Cyc_List_List*_tmpA46=Cyc_Tcutil_substs(rgn,inst,
_tmp9F7);struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp139D;struct Cyc_Absyn_DatatypeInfo
_tmp139C;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp139B;return _tmpA46 == 
_tmp9F7?t:(void*)((_tmp139B=_cycalloc(sizeof(*_tmp139B)),((_tmp139B[0]=((
_tmp139D.tag=3,((_tmp139D.f1=((_tmp139C.datatype_info=_tmp9F6,((_tmp139C.targs=
_tmpA46,_tmp139C)))),_tmp139D)))),_tmp139B))));}_LL50B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmp9F8=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp9ED;if(_tmp9F8->tag
!= 4)goto _LL50D;else{_tmp9F9=_tmp9F8->f1;_tmp9FA=_tmp9F9.field_info;_tmp9FB=
_tmp9F9.targs;}}_LL50C: {struct Cyc_List_List*_tmpA4A=Cyc_Tcutil_substs(rgn,inst,
_tmp9FB);struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp13A3;struct Cyc_Absyn_DatatypeFieldInfo
_tmp13A2;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp13A1;return
_tmpA4A == _tmp9FB?t:(void*)((_tmp13A1=_cycalloc(sizeof(*_tmp13A1)),((_tmp13A1[0]=((
_tmp13A3.tag=4,((_tmp13A3.f1=((_tmp13A2.field_info=_tmp9FA,((_tmp13A2.targs=
_tmpA4A,_tmp13A2)))),_tmp13A3)))),_tmp13A1))));}_LL50D: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*
_tmp9FC=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9ED;if(_tmp9FC->tag
!= 17)goto _LL50F;else{_tmp9FD=_tmp9FC->f1;_tmp9FE=_tmp9FC->f2;_tmp9FF=_tmp9FC->f3;
_tmpA00=(void*)_tmp9FC->f4;}}_LL50E: {struct Cyc_List_List*_tmpA4E=Cyc_Tcutil_substs(
rgn,inst,_tmp9FE);struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp13A6;struct
Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp13A5;return _tmpA4E == _tmp9FE?t:(void*)((
_tmp13A5=_cycalloc(sizeof(*_tmp13A5)),((_tmp13A5[0]=((_tmp13A6.tag=17,((_tmp13A6.f1=
_tmp9FD,((_tmp13A6.f2=_tmpA4E,((_tmp13A6.f3=_tmp9FF,((_tmp13A6.f4=(void*)_tmpA00,
_tmp13A6)))))))))),_tmp13A5))));}_LL50F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmpA01=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9ED;if(_tmpA01->tag != 
8)goto _LL511;else{_tmpA02=_tmpA01->f1;_tmpA03=_tmpA02.elt_type;_tmpA04=_tmpA02.tq;
_tmpA05=_tmpA02.num_elts;_tmpA06=_tmpA02.zero_term;_tmpA07=_tmpA02.zt_loc;}}
_LL510: {void*_tmpA51=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA03);struct Cyc_Absyn_ArrayType_Absyn_Type_struct
_tmp13AC;struct Cyc_Absyn_ArrayInfo _tmp13AB;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp13AA;return _tmpA51 == _tmpA03?t:(void*)((_tmp13AA=_cycalloc(sizeof(*_tmp13AA)),((
_tmp13AA[0]=((_tmp13AC.tag=8,((_tmp13AC.f1=((_tmp13AB.elt_type=_tmpA51,((
_tmp13AB.tq=_tmpA04,((_tmp13AB.num_elts=_tmpA05,((_tmp13AB.zero_term=_tmpA06,((
_tmp13AB.zt_loc=_tmpA07,_tmp13AB)))))))))),_tmp13AC)))),_tmp13AA))));}_LL511: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA08=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp9ED;if(_tmpA08->tag != 5)goto _LL513;else{_tmpA09=_tmpA08->f1;_tmpA0A=_tmpA09.elt_typ;
_tmpA0B=_tmpA09.elt_tq;_tmpA0C=_tmpA09.ptr_atts;_tmpA0D=_tmpA0C.rgn;_tmpA0E=
_tmpA0C.nullable;_tmpA0F=_tmpA0C.bounds;_tmpA10=_tmpA0C.zero_term;}}_LL512: {
void*_tmpA55=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA0A);void*_tmpA56=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA0D);union Cyc_Absyn_Constraint*_tmpA57=_tmpA0F;{void*_tmpA58=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpA0F);struct Cyc_Absyn_Exp*_tmpA5A;_LL541: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmpA59=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA58;if(_tmpA59->tag != 
1)goto _LL543;else{_tmpA5A=_tmpA59->f1;}}_LL542: {struct Cyc_Absyn_Exp*_tmpA5B=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmpA5A);if(_tmpA5B != _tmpA5A){struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct
_tmp13AF;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp13AE;_tmpA57=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp13AE=_cycalloc(sizeof(*_tmp13AE)),((
_tmp13AE[0]=((_tmp13AF.tag=1,((_tmp13AF.f1=_tmpA5B,_tmp13AF)))),_tmp13AE)))));}
goto _LL540;}_LL543:;_LL544: goto _LL540;_LL540:;}if((_tmpA55 == _tmpA0A  && _tmpA56
== _tmpA0D) && _tmpA57 == _tmpA0F)return t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct
_tmp13B9;struct Cyc_Absyn_PtrAtts _tmp13B8;struct Cyc_Absyn_PtrInfo _tmp13B7;struct
Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp13B6;return(void*)((_tmp13B6=
_cycalloc(sizeof(*_tmp13B6)),((_tmp13B6[0]=((_tmp13B9.tag=5,((_tmp13B9.f1=((
_tmp13B7.elt_typ=_tmpA55,((_tmp13B7.elt_tq=_tmpA0B,((_tmp13B7.ptr_atts=((
_tmp13B8.rgn=_tmpA56,((_tmp13B8.nullable=_tmpA0E,((_tmp13B8.bounds=_tmpA57,((
_tmp13B8.zero_term=_tmpA10,((_tmp13B8.ptrloc=0,_tmp13B8)))))))))),_tmp13B7)))))),
_tmp13B9)))),_tmp13B6))));};}_LL513: {struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmpA11=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9ED;if(_tmpA11->tag != 9)
goto _LL515;else{_tmpA12=_tmpA11->f1;_tmpA13=_tmpA12.tvars;_tmpA14=_tmpA12.effect;
_tmpA15=_tmpA12.ret_tqual;_tmpA16=_tmpA12.ret_typ;_tmpA17=_tmpA12.args;_tmpA18=
_tmpA12.c_varargs;_tmpA19=_tmpA12.cyc_varargs;_tmpA1A=_tmpA12.rgn_po;_tmpA1B=
_tmpA12.attributes;}}_LL514:{struct Cyc_List_List*_tmpA62=_tmpA13;for(0;_tmpA62 != 
0;_tmpA62=_tmpA62->tl){struct _tuple16*_tmp13C3;struct Cyc_Absyn_VarType_Absyn_Type_struct
_tmp13C2;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp13C1;struct Cyc_List_List*
_tmp13C0;inst=((_tmp13C0=_region_malloc(rgn,sizeof(*_tmp13C0)),((_tmp13C0->hd=((
_tmp13C3=_region_malloc(rgn,sizeof(*_tmp13C3)),((_tmp13C3->f1=(struct Cyc_Absyn_Tvar*)
_tmpA62->hd,((_tmp13C3->f2=(void*)((_tmp13C1=_cycalloc(sizeof(*_tmp13C1)),((
_tmp13C1[0]=((_tmp13C2.tag=2,((_tmp13C2.f1=(struct Cyc_Absyn_Tvar*)_tmpA62->hd,
_tmp13C2)))),_tmp13C1)))),_tmp13C3)))))),((_tmp13C0->tl=inst,_tmp13C0))))));}}{
struct Cyc_List_List*_tmpA68;struct Cyc_List_List*_tmpA69;struct _tuple1 _tmpA67=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*
env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,
_tmpA17));_tmpA68=_tmpA67.f1;_tmpA69=_tmpA67.f2;{struct Cyc_List_List*_tmpA6A=
_tmpA17;struct Cyc_List_List*_tmpA6B=Cyc_Tcutil_substs(rgn,inst,_tmpA69);if(
_tmpA6B != _tmpA69)_tmpA6A=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct
_tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,
_tmpA17,_tmpA6B);{void*eff2;if(_tmpA14 == 0)eff2=0;else{void*new_eff=(void*)Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmpA14);if(new_eff == _tmpA14)eff2=_tmpA14;else{eff2=new_eff;}}{
struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmpA19 == 0)cyc_varargs2=0;else{
struct _dyneither_ptr*_tmpA6D;struct Cyc_Absyn_Tqual _tmpA6E;void*_tmpA6F;int
_tmpA70;struct Cyc_Absyn_VarargInfo _tmpA6C=*_tmpA19;_tmpA6D=_tmpA6C.name;_tmpA6E=
_tmpA6C.tq;_tmpA6F=_tmpA6C.type;_tmpA70=_tmpA6C.inject;{void*_tmpA71=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA6F);if(_tmpA71 == _tmpA6F)cyc_varargs2=_tmpA19;else{struct Cyc_Absyn_VarargInfo*
_tmp13C4;cyc_varargs2=((_tmp13C4=_cycalloc(sizeof(*_tmp13C4)),((_tmp13C4->name=
_tmpA6D,((_tmp13C4->tq=_tmpA6E,((_tmp13C4->type=_tmpA71,((_tmp13C4->inject=
_tmpA70,_tmp13C4))))))))));}};}{struct Cyc_List_List*rgn_po2;struct Cyc_List_List*
_tmpA74;struct Cyc_List_List*_tmpA75;struct _tuple1 _tmpA73=((struct _tuple1(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,_tmpA1A);_tmpA74=_tmpA73.f1;_tmpA75=_tmpA73.f2;{struct Cyc_List_List*
_tmpA76=Cyc_Tcutil_substs(rgn,inst,_tmpA74);struct Cyc_List_List*_tmpA77=Cyc_Tcutil_substs(
rgn,inst,_tmpA75);if(_tmpA76 == _tmpA74  && _tmpA77 == _tmpA75)rgn_po2=_tmpA1A;
else{rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(_tmpA76,_tmpA77);}{struct Cyc_Absyn_FnType_Absyn_Type_struct
_tmp13CA;struct Cyc_Absyn_FnInfo _tmp13C9;struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp13C8;return(void*)((_tmp13C8=_cycalloc(sizeof(*_tmp13C8)),((_tmp13C8[0]=((
_tmp13CA.tag=9,((_tmp13CA.f1=((_tmp13C9.tvars=_tmpA13,((_tmp13C9.effect=eff2,((
_tmp13C9.ret_tqual=_tmpA15,((_tmp13C9.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmpA16),((_tmp13C9.args=_tmpA6A,((_tmp13C9.c_varargs=_tmpA18,((_tmp13C9.cyc_varargs=
cyc_varargs2,((_tmp13C9.rgn_po=rgn_po2,((_tmp13C9.attributes=_tmpA1B,_tmp13C9)))))))))))))))))),
_tmp13CA)))),_tmp13C8))));};};};};};};};_LL515: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmpA1C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9ED;if(_tmpA1C->tag != 
10)goto _LL517;else{_tmpA1D=_tmpA1C->f1;}}_LL516: {struct Cyc_List_List*ts2=0;int
change=0;{struct Cyc_List_List*_tmpA7B=_tmpA1D;for(0;_tmpA7B != 0;_tmpA7B=_tmpA7B->tl){
void*_tmpA7C=(*((struct _tuple12*)_tmpA7B->hd)).f2;void*_tmpA7D=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA7C);if(_tmpA7C != _tmpA7D)change=1;{struct Cyc_List_List*_tmp13CB;
ts2=((_tmp13CB=_region_malloc(rgn,sizeof(*_tmp13CB)),((_tmp13CB->hd=_tmpA7D,((
_tmp13CB->tl=ts2,_tmp13CB))))));};}}if(!change)return t;{struct Cyc_List_List*
_tmpA7F=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmpA1D,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp13CE;struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmp13CD;return(void*)((_tmp13CD=_cycalloc(sizeof(*_tmp13CD)),((_tmp13CD[0]=((
_tmp13CE.tag=10,((_tmp13CE.f1=_tmpA7F,_tmp13CE)))),_tmp13CD))));};}_LL517: {
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA1E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)
_tmp9ED;if(_tmpA1E->tag != 12)goto _LL519;else{_tmpA1F=_tmpA1E->f1;_tmpA20=_tmpA1E->f2;}}
_LL518: {struct Cyc_List_List*_tmpA82=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(
rgn,Cyc_Tcutil_field_type,_tmpA20);struct Cyc_List_List*_tmpA83=Cyc_Tcutil_substs(
rgn,inst,_tmpA82);if(_tmpA83 == _tmpA82)return t;{struct Cyc_List_List*_tmpA84=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,
_tmpA20,_tmpA83);struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp13D1;struct
Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp13D0;return(void*)((_tmp13D0=
_cycalloc(sizeof(*_tmp13D0)),((_tmp13D0[0]=((_tmp13D1.tag=12,((_tmp13D1.f1=
_tmpA1F,((_tmp13D1.f2=_tmpA84,_tmp13D1)))))),_tmp13D0))));};}_LL519: {struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmpA21=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp9ED;if(_tmpA21->tag != 1)
goto _LL51B;else{_tmpA22=(void*)_tmpA21->f2;}}_LL51A: if(_tmpA22 != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmpA22);else{return t;}_LL51B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*
_tmpA23=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp9ED;if(_tmpA23->tag
!= 15)goto _LL51D;else{_tmpA24=(void*)_tmpA23->f1;}}_LL51C: {void*_tmpA87=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA24);struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp13D4;struct
Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp13D3;return _tmpA87 == _tmpA24?t:(
void*)((_tmp13D3=_cycalloc(sizeof(*_tmp13D3)),((_tmp13D3[0]=((_tmp13D4.tag=15,((
_tmp13D4.f1=(void*)_tmpA87,_tmp13D4)))),_tmp13D3))));}_LL51D: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*
_tmpA25=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp9ED;if(_tmpA25->tag != 
16)goto _LL51F;else{_tmpA26=(void*)_tmpA25->f1;_tmpA27=(void*)_tmpA25->f2;}}
_LL51E: {void*_tmpA8A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA26);void*_tmpA8B=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA27);struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp13D7;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*
_tmp13D6;return _tmpA8A == _tmpA26  && _tmpA8B == _tmpA27?t:(void*)((_tmp13D6=
_cycalloc(sizeof(*_tmp13D6)),((_tmp13D6[0]=((_tmp13D7.tag=16,((_tmp13D7.f1=(void*)
_tmpA8A,((_tmp13D7.f2=(void*)_tmpA8B,_tmp13D7)))))),_tmp13D6))));}_LL51F: {
struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpA28=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)
_tmp9ED;if(_tmpA28->tag != 19)goto _LL521;else{_tmpA29=(void*)_tmpA28->f1;}}_LL520: {
void*_tmpA8E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA29);struct Cyc_Absyn_TagType_Absyn_Type_struct
_tmp13DA;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp13D9;return _tmpA8E == 
_tmpA29?t:(void*)((_tmp13D9=_cycalloc(sizeof(*_tmp13D9)),((_tmp13D9[0]=((
_tmp13DA.tag=19,((_tmp13DA.f1=(void*)_tmpA8E,_tmp13DA)))),_tmp13D9))));}_LL521: {
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpA2A=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)
_tmp9ED;if(_tmpA2A->tag != 18)goto _LL523;else{_tmpA2B=_tmpA2A->f1;}}_LL522: {
struct Cyc_Absyn_Exp*_tmpA91=Cyc_Tcutil_rsubsexp(rgn,inst,_tmpA2B);struct Cyc_Absyn_ValueofType_Absyn_Type_struct
_tmp13DD;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp13DC;return _tmpA91 == 
_tmpA2B?t:(void*)((_tmp13DC=_cycalloc(sizeof(*_tmp13DC)),((_tmp13DC[0]=((
_tmp13DD.tag=18,((_tmp13DD.f1=_tmpA91,_tmp13DD)))),_tmp13DC))));}_LL523: {struct
Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpA2C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)
_tmp9ED;if(_tmpA2C->tag != 13)goto _LL525;}_LL524: goto _LL526;_LL525: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*
_tmpA2D=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp9ED;if(_tmpA2D->tag
!= 14)goto _LL527;}_LL526: goto _LL528;_LL527: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*
_tmpA2E=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp9ED;if(_tmpA2E->tag != 0)
goto _LL529;}_LL528: goto _LL52A;_LL529: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmpA2F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp9ED;if(_tmpA2F->tag != 6)
goto _LL52B;}_LL52A: goto _LL52C;_LL52B: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmpA30=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp9ED;if(_tmpA30->tag != 
7)goto _LL52D;}_LL52C: goto _LL52E;_LL52D: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*
_tmpA31=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp9ED;if(_tmpA31->tag != 
22)goto _LL52F;}_LL52E: goto _LL530;_LL52F: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*
_tmpA32=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp9ED;if(_tmpA32->tag != 
21)goto _LL531;}_LL530: goto _LL532;_LL531: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*
_tmpA33=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp9ED;if(_tmpA33->tag != 20)
goto _LL533;}_LL532: return t;_LL533: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*
_tmpA34=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp9ED;if(_tmpA34->tag != 25)
goto _LL535;else{_tmpA35=(void*)_tmpA34->f1;}}_LL534: {void*_tmpA94=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA35);struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp13E0;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*
_tmp13DF;return _tmpA94 == _tmpA35?t:(void*)((_tmp13DF=_cycalloc(sizeof(*_tmp13DF)),((
_tmp13DF[0]=((_tmp13E0.tag=25,((_tmp13E0.f1=(void*)_tmpA94,_tmp13E0)))),_tmp13DF))));}
_LL535: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpA36=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)
_tmp9ED;if(_tmpA36->tag != 23)goto _LL537;else{_tmpA37=(void*)_tmpA36->f1;}}_LL536: {
void*_tmpA97=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA37);struct Cyc_Absyn_AccessEff_Absyn_Type_struct
_tmp13E3;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp13E2;return _tmpA97 == 
_tmpA37?t:(void*)((_tmp13E2=_cycalloc(sizeof(*_tmp13E2)),((_tmp13E2[0]=((
_tmp13E3.tag=23,((_tmp13E3.f1=(void*)_tmpA97,_tmp13E3)))),_tmp13E2))));}_LL537: {
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpA38=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)
_tmp9ED;if(_tmpA38->tag != 24)goto _LL539;else{_tmpA39=_tmpA38->f1;}}_LL538: {
struct Cyc_List_List*_tmpA9A=Cyc_Tcutil_substs(rgn,inst,_tmpA39);struct Cyc_Absyn_JoinEff_Absyn_Type_struct
_tmp13E6;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp13E5;return _tmpA9A == 
_tmpA39?t:(void*)((_tmp13E5=_cycalloc(sizeof(*_tmp13E5)),((_tmp13E5[0]=((
_tmp13E6.tag=24,((_tmp13E6.f1=_tmpA9A,_tmp13E6)))),_tmp13E5))));}_LL539: {struct
Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpA3A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)
_tmp9ED;if(_tmpA3A->tag != 26)goto _LL504;}_LL53A: {const char*_tmp13E9;void*
_tmp13E8;(_tmp13E8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp13E9="found typedecltype in rsubs",_tag_dyneither(
_tmp13E9,sizeof(char),28))),_tag_dyneither(_tmp13E8,sizeof(void*),0)));}_LL504:;}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmpA9F=(void*)ts->hd;
struct Cyc_List_List*_tmpAA0=ts->tl;void*_tmpAA1=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmpA9F);struct Cyc_List_List*_tmpAA2=Cyc_Tcutil_substs(rgn,inst,_tmpAA0);if(
_tmpA9F == _tmpAA1  && _tmpAA0 == _tmpAA2)return ts;{struct Cyc_List_List*_tmp13EA;
return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp13EA=_cycalloc(sizeof(*
_tmp13EA)),((_tmp13EA->hd=_tmpAA1,((_tmp13EA->tl=_tmpAA2,_tmp13EA)))))));};};}
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{return t;}}struct
_tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmpAA4=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp13ED;struct _tuple16*_tmp13EC;return(_tmp13EC=_cycalloc(
sizeof(*_tmp13EC)),((_tmp13EC->f1=tv,((_tmp13EC->f2=Cyc_Absyn_new_evar(_tmpAA4,((
_tmp13ED=_cycalloc(sizeof(*_tmp13ED)),((_tmp13ED->v=s,_tmp13ED))))),_tmp13EC)))));}
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*
tv){struct _tuple17 _tmpAA8;struct Cyc_List_List*_tmpAA9;struct _RegionHandle*
_tmpAAA;struct _tuple17*_tmpAA7=env;_tmpAA8=*_tmpAA7;_tmpAA9=_tmpAA8.f1;_tmpAAA=
_tmpAA8.f2;{struct Cyc_Core_Opt*_tmpAAB=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(
tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp13F0;struct _tuple16*_tmp13EF;return(
_tmp13EF=_region_malloc(_tmpAAA,sizeof(*_tmp13EF)),((_tmp13EF->f1=tv,((_tmp13EF->f2=
Cyc_Absyn_new_evar(_tmpAAB,((_tmp13F0=_cycalloc(sizeof(*_tmp13F0)),((_tmp13F0->v=
_tmpAA9,_tmp13F0))))),_tmp13EF)))));};}static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*
tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)
tvs2->hd)->name,tv->name)== 0){void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2)){const char*_tmp13F6;void*
_tmp13F5[3];struct Cyc_String_pa_PrintArg_struct _tmp13F4;struct Cyc_String_pa_PrintArg_struct
_tmp13F3;struct Cyc_String_pa_PrintArg_struct _tmp13F2;(_tmp13F2.tag=0,((_tmp13F2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k2)),((
_tmp13F3.tag=0,((_tmp13F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(
k1)),((_tmp13F4.tag=0,((_tmp13F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
tv->name),((_tmp13F5[0]=& _tmp13F4,((_tmp13F5[1]=& _tmp13F3,((_tmp13F5[2]=&
_tmp13F2,Cyc_Tcutil_terr(loc,((_tmp13F6="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmp13F6,sizeof(char),59))),_tag_dyneither(_tmp13F5,sizeof(void*),
3)))))))))))))))))));}if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp13F9;void*_tmp13F8;(_tmp13F8=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13F9="same type variable has different identity!",
_tag_dyneither(_tmp13F9,sizeof(char),43))),_tag_dyneither(_tmp13F8,sizeof(void*),
0)));}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*
_tmp13FA;return(_tmp13FA=_cycalloc(sizeof(*_tmp13FA)),((_tmp13FA->hd=tv,((
_tmp13FA->tl=tvs,_tmp13FA)))));};}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp13FD;void*_tmp13FC;(_tmp13FC=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13FD="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmp13FD,sizeof(char),39))),_tag_dyneither(_tmp13FC,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmpAB8=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmpAB8->identity == - 1){const char*
_tmp1400;void*_tmp13FF;(_tmp13FF=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1400="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmp1400,sizeof(char),41))),_tag_dyneither(_tmp13FF,sizeof(void*),
0)));}if(_tmpAB8->identity == tv->identity)return tvs;}}{struct Cyc_List_List*
_tmp1401;return(_tmp1401=_cycalloc(sizeof(*_tmp1401)),((_tmp1401->hd=tv,((
_tmp1401->tl=tvs,_tmp1401)))));};}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity == - 1){
const char*_tmp1404;void*_tmp1403;(_tmp1403=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1404="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmp1404,sizeof(char),44))),_tag_dyneither(_tmp1403,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple28
_tmpABF;struct Cyc_Absyn_Tvar*_tmpAC0;int _tmpAC1;int*_tmpAC2;struct _tuple28*
_tmpABE=(struct _tuple28*)tvs2->hd;_tmpABF=*_tmpABE;_tmpAC0=_tmpABF.f1;_tmpAC1=
_tmpABF.f2;_tmpAC2=(int*)&(*_tmpABE).f2;if(_tmpAC0->identity == - 1){const char*
_tmp1407;void*_tmp1406;(_tmp1406=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1407="fast_add_free_tvar_bool: bad identity in tvs2",
_tag_dyneither(_tmp1407,sizeof(char),46))),_tag_dyneither(_tmp1406,sizeof(void*),
0)));}if(_tmpAC0->identity == tv->identity){*_tmpAC2=*_tmpAC2  || b;return tvs;}}}{
struct _tuple28*_tmp140A;struct Cyc_List_List*_tmp1409;return(_tmp1409=
_region_malloc(r,sizeof(*_tmp1409)),((_tmp1409->hd=((_tmp140A=_region_malloc(r,
sizeof(*_tmp140A)),((_tmp140A->f1=tv,((_tmp140A->f2=b,_tmp140A)))))),((_tmp1409->tl=
tvs,_tmp1409)))));};}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp140E;void*_tmp140D[1];struct Cyc_String_pa_PrintArg_struct _tmp140C;(_tmp140C.tag=
0,((_tmp140C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(
tv)),((_tmp140D[0]=& _tmp140C,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp140E="bound tvar id for %s is NULL",
_tag_dyneither(_tmp140E,sizeof(char),29))),_tag_dyneither(_tmp140D,sizeof(void*),
1)))))));}{struct Cyc_List_List*_tmp140F;return(_tmp140F=_cycalloc(sizeof(*
_tmp140F)),((_tmp140F->hd=tv,((_tmp140F->tl=tvs,_tmp140F)))));};}struct _tuple29{
void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct
_RegionHandle*r,struct Cyc_List_List*es,void*e,int b){void*_tmpACB=Cyc_Tcutil_compress(
e);int _tmpACD;_LL546: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpACC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)
_tmpACB;if(_tmpACC->tag != 1)goto _LL548;else{_tmpACD=_tmpACC->f3;}}_LL547:{struct
Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){struct _tuple29 _tmpACF;void*
_tmpAD0;int _tmpAD1;int*_tmpAD2;struct _tuple29*_tmpACE=(struct _tuple29*)es2->hd;
_tmpACF=*_tmpACE;_tmpAD0=_tmpACF.f1;_tmpAD1=_tmpACF.f2;_tmpAD2=(int*)&(*_tmpACE).f2;{
void*_tmpAD3=Cyc_Tcutil_compress(_tmpAD0);int _tmpAD5;_LL54B: {struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmpAD4=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpAD3;if(_tmpAD4->tag != 1)
goto _LL54D;else{_tmpAD5=_tmpAD4->f3;}}_LL54C: if(_tmpACD == _tmpAD5){if(b != *
_tmpAD2)*_tmpAD2=1;return es;}goto _LL54A;_LL54D:;_LL54E: goto _LL54A;_LL54A:;};}}{
struct _tuple29*_tmp1412;struct Cyc_List_List*_tmp1411;return(_tmp1411=
_region_malloc(r,sizeof(*_tmp1411)),((_tmp1411->hd=((_tmp1412=_region_malloc(r,
sizeof(*_tmp1412)),((_tmp1412->f1=e,((_tmp1412->f2=b,_tmp1412)))))),((_tmp1411->tl=
es,_tmp1411)))));};_LL548:;_LL549: return es;_LL545:;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs){struct Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int
present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)
tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}
if(!present){struct Cyc_List_List*_tmp1413;r=((_tmp1413=_region_malloc(rgn,
sizeof(*_tmp1413)),((_tmp1413->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1413->tl=
r,_tmp1413))))));}}r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
r);return r;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct
_RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*
res=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpADA;int _tmpADB;struct
_tuple28 _tmpAD9=*((struct _tuple28*)tvs->hd);_tmpADA=_tmpAD9.f1;_tmpADB=_tmpAD9.f2;{
int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(_tmpADA->identity
== ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){
struct Cyc_List_List*_tmp1414;res=((_tmp1414=_region_malloc(r,sizeof(*_tmp1414)),((
_tmp1414->hd=(struct _tuple28*)tvs->hd,((_tmp1414->tl=res,_tmp1414))))));}};}res=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);return res;}
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*
field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){if(width != 0){
unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)width)){
const char*_tmp1418;void*_tmp1417[1];struct Cyc_String_pa_PrintArg_struct _tmp1416;(
_tmp1416.tag=0,((_tmp1416.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((
_tmp1417[0]=& _tmp1416,Cyc_Tcutil_terr(loc,((_tmp1418="bitfield %s does not have constant width",
_tag_dyneither(_tmp1418,sizeof(char),41))),_tag_dyneither(_tmp1417,sizeof(void*),
1)))))));}else{unsigned int _tmpAE1;int _tmpAE2;struct _tuple14 _tmpAE0=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmpAE1=_tmpAE0.f1;_tmpAE2=_tmpAE0.f2;if(!_tmpAE2){
const char*_tmp141B;void*_tmp141A;(_tmp141A=0,Cyc_Tcutil_terr(loc,((_tmp141B="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmp141B,sizeof(char),45))),_tag_dyneither(_tmp141A,sizeof(void*),
0)));}w=_tmpAE1;}{void*_tmpAE5=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of
_tmpAE7;_LL550: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpAE6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmpAE5;if(_tmpAE6->tag != 6)goto _LL552;else{_tmpAE7=_tmpAE6->f2;}}_LL551: switch(
_tmpAE7){case Cyc_Absyn_Char_sz: _LL554: if(w > 8){const char*_tmp141E;void*_tmp141D;(
_tmp141D=0,Cyc_Tcutil_terr(loc,((_tmp141E="bitfield larger than type",
_tag_dyneither(_tmp141E,sizeof(char),26))),_tag_dyneither(_tmp141D,sizeof(void*),
0)));}break;case Cyc_Absyn_Short_sz: _LL555: if(w > 16){const char*_tmp1421;void*
_tmp1420;(_tmp1420=0,Cyc_Tcutil_terr(loc,((_tmp1421="bitfield larger than type",
_tag_dyneither(_tmp1421,sizeof(char),26))),_tag_dyneither(_tmp1420,sizeof(void*),
0)));}break;case Cyc_Absyn_Long_sz: _LL556: goto _LL557;case Cyc_Absyn_Int_sz: _LL557:
if(w > 32){const char*_tmp1424;void*_tmp1423;(_tmp1423=0,Cyc_Tcutil_terr(loc,((
_tmp1424="bitfield larger than type",_tag_dyneither(_tmp1424,sizeof(char),26))),
_tag_dyneither(_tmp1423,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz:
_LL558: if(w > 64){const char*_tmp1427;void*_tmp1426;(_tmp1426=0,Cyc_Tcutil_terr(
loc,((_tmp1427="bitfield larger than type",_tag_dyneither(_tmp1427,sizeof(char),
26))),_tag_dyneither(_tmp1426,sizeof(void*),0)));}break;}goto _LL54F;_LL552:;
_LL553:{const char*_tmp142C;void*_tmp142B[2];struct Cyc_String_pa_PrintArg_struct
_tmp142A;struct Cyc_String_pa_PrintArg_struct _tmp1429;(_tmp1429.tag=0,((_tmp1429.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ)),((
_tmp142A.tag=0,((_tmp142A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((
_tmp142B[0]=& _tmp142A,((_tmp142B[1]=& _tmp1429,Cyc_Tcutil_terr(loc,((_tmp142C="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmp142C,sizeof(char),52))),_tag_dyneither(_tmp142B,sizeof(void*),
2)))))))))))));}goto _LL54F;_LL54F:;};}}static void Cyc_Tcutil_check_field_atts(
unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){for(0;atts != 0;
atts=atts->tl){void*_tmpAF4=(void*)atts->hd;_LL55B: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*
_tmpAF5=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmpAF4;if(_tmpAF5->tag
!= 7)goto _LL55D;}_LL55C: continue;_LL55D: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*
_tmpAF6=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpAF4;if(_tmpAF6->tag
!= 6)goto _LL55F;}_LL55E: continue;_LL55F:;_LL560: {const char*_tmp1431;void*
_tmp1430[2];struct Cyc_String_pa_PrintArg_struct _tmp142F;struct Cyc_String_pa_PrintArg_struct
_tmp142E;(_tmp142E.tag=0,((_tmp142E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmp142F.tag=0,((_tmp142F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp1430[0]=&
_tmp142F,((_tmp1430[1]=& _tmp142E,Cyc_Tcutil_terr(loc,((_tmp1431="bad attribute %s on member %s",
_tag_dyneither(_tmp1431,sizeof(char),30))),_tag_dyneither(_tmp1430,sizeof(void*),
2)))))))))))));}_LL55A:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct
Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*
free_evars;int generalize_evars;int fn_result;};typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*
t){void*_tmpAFB=t;struct Cyc_Absyn_Typedefdecl*_tmpAFD;void*_tmpAFE;_LL562: {
struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAFC=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)
_tmpAFB;if(_tmpAFC->tag != 17)goto _LL564;else{_tmpAFD=_tmpAFC->f3;_tmpAFE=(void*)
_tmpAFC->f4;}}_LL563: if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmpAFD))->tq).real_const
 || (_tmpAFD->tq).print_const){if(declared_const){const char*_tmp1434;void*
_tmp1433;(_tmp1433=0,Cyc_Tcutil_warn(loc,((_tmp1434="extra const",_tag_dyneither(
_tmp1434,sizeof(char),12))),_tag_dyneither(_tmp1433,sizeof(void*),0)));}return 1;}
if((unsigned int)_tmpAFE)return Cyc_Tcutil_extract_const_from_typedef(loc,
declared_const,(void*)_tmpAFE);else{return declared_const;}_LL564:;_LL565: return
declared_const;_LL561:;}static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Typedefdecl*td){if(td != 0){if(td->defn != 0){void*_tmpB01=
Cyc_Tcutil_compress((void*)_check_null(td->defn));struct Cyc_Absyn_PtrInfo _tmpB03;
struct Cyc_Absyn_PtrAtts _tmpB04;void*_tmpB05;_LL567: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmpB02=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB01;if(_tmpB02->tag
!= 5)goto _LL569;else{_tmpB03=_tmpB02->f1;_tmpB04=_tmpB03.ptr_atts;_tmpB05=
_tmpB04.rgn;}}_LL568:{void*_tmpB06=Cyc_Tcutil_compress(_tmpB05);struct Cyc_Absyn_Tvar*
_tmpB08;_LL56C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpB07=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)
_tmpB06;if(_tmpB07->tag != 2)goto _LL56E;else{_tmpB08=_tmpB07->f1;}}_LL56D: return
Cyc_Absyn_tvar_cmp(tvar,_tmpB08)== 0;_LL56E:;_LL56F: goto _LL56B;_LL56B:;}goto
_LL566;_LL569:;_LL56A: goto _LL566;_LL566:;}}else{return 1;}return 0;}static struct
Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*
def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){void*
_tmpB09=Cyc_Absyn_compress_kb(tvar->kind);struct Cyc_Absyn_Kind*_tmpB0B;struct Cyc_Absyn_Kind
_tmpB0C;enum Cyc_Absyn_KindQual _tmpB0D;enum Cyc_Absyn_AliasQual _tmpB0E;struct Cyc_Absyn_Kind*
_tmpB10;struct Cyc_Absyn_Kind _tmpB11;enum Cyc_Absyn_KindQual _tmpB12;enum Cyc_Absyn_AliasQual
_tmpB13;_LL571: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB0A=(struct
Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB09;if(_tmpB0A->tag != 2)goto _LL573;
else{_tmpB0B=_tmpB0A->f2;_tmpB0C=*_tmpB0B;_tmpB0D=_tmpB0C.kind;if(_tmpB0D != Cyc_Absyn_RgnKind)
goto _LL573;_tmpB0E=_tmpB0C.aliasqual;if(_tmpB0E != Cyc_Absyn_Top)goto _LL573;}}
_LL572: goto _LL574;_LL573: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB0F=(
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB09;if(_tmpB0F->tag != 0)goto
_LL575;else{_tmpB10=_tmpB0F->f1;_tmpB11=*_tmpB10;_tmpB12=_tmpB11.kind;if(_tmpB12
!= Cyc_Absyn_RgnKind)goto _LL575;_tmpB13=_tmpB11.aliasqual;if(_tmpB13 != Cyc_Absyn_Top)
goto _LL575;}}_LL574: if(((expected_kind->kind == Cyc_Absyn_BoxKind  || 
expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind)
 && Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){if(expected_kind->aliasqual == 
Cyc_Absyn_Aliasable)return& Cyc_Tcutil_rk;else{if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}return& Cyc_Tcutil_trk;_LL575:;_LL576: return Cyc_Tcutil_tvar_kind(
tvar,def_kind);_LL570:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
struct _tuple30{enum Cyc_Absyn_Format_Type f1;void*f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){{void*_tmpB14=
Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpB17;struct Cyc_Core_Opt**_tmpB18;
void*_tmpB19;void**_tmpB1A;struct Cyc_Absyn_Tvar*_tmpB1C;struct Cyc_Absyn_TypeDecl*
_tmpB1E;struct Cyc_Absyn_TypeDecl _tmpB1F;void*_tmpB20;void**_tmpB21;void***
_tmpB22;struct Cyc_List_List*_tmpB24;struct _tuple2*_tmpB26;struct Cyc_Absyn_Enumdecl*
_tmpB27;struct Cyc_Absyn_Enumdecl**_tmpB28;struct Cyc_Absyn_DatatypeInfo _tmpB2A;
union Cyc_Absyn_DatatypeInfoU _tmpB2B;union Cyc_Absyn_DatatypeInfoU*_tmpB2C;struct
Cyc_List_List*_tmpB2D;struct Cyc_List_List**_tmpB2E;struct Cyc_Absyn_DatatypeFieldInfo
_tmpB30;union Cyc_Absyn_DatatypeFieldInfoU _tmpB31;union Cyc_Absyn_DatatypeFieldInfoU*
_tmpB32;struct Cyc_List_List*_tmpB33;struct Cyc_Absyn_PtrInfo _tmpB35;void*_tmpB36;
struct Cyc_Absyn_Tqual _tmpB37;struct Cyc_Absyn_Tqual*_tmpB38;struct Cyc_Absyn_PtrAtts
_tmpB39;void*_tmpB3A;union Cyc_Absyn_Constraint*_tmpB3B;union Cyc_Absyn_Constraint*
_tmpB3C;union Cyc_Absyn_Constraint*_tmpB3D;void*_tmpB3F;struct Cyc_Absyn_Exp*
_tmpB41;struct Cyc_Absyn_ArrayInfo _tmpB45;void*_tmpB46;struct Cyc_Absyn_Tqual
_tmpB47;struct Cyc_Absyn_Tqual*_tmpB48;struct Cyc_Absyn_Exp*_tmpB49;struct Cyc_Absyn_Exp**
_tmpB4A;union Cyc_Absyn_Constraint*_tmpB4B;unsigned int _tmpB4C;struct Cyc_Absyn_FnInfo
_tmpB4E;struct Cyc_List_List*_tmpB4F;struct Cyc_List_List**_tmpB50;void*_tmpB51;
void**_tmpB52;struct Cyc_Absyn_Tqual _tmpB53;struct Cyc_Absyn_Tqual*_tmpB54;void*
_tmpB55;struct Cyc_List_List*_tmpB56;int _tmpB57;struct Cyc_Absyn_VarargInfo*
_tmpB58;struct Cyc_List_List*_tmpB59;struct Cyc_List_List*_tmpB5A;struct Cyc_List_List*
_tmpB5C;enum Cyc_Absyn_AggrKind _tmpB5E;struct Cyc_List_List*_tmpB5F;struct Cyc_Absyn_AggrInfo
_tmpB61;union Cyc_Absyn_AggrInfoU _tmpB62;union Cyc_Absyn_AggrInfoU*_tmpB63;struct
Cyc_List_List*_tmpB64;struct Cyc_List_List**_tmpB65;struct _tuple2*_tmpB67;struct
Cyc_List_List*_tmpB68;struct Cyc_List_List**_tmpB69;struct Cyc_Absyn_Typedefdecl*
_tmpB6A;struct Cyc_Absyn_Typedefdecl**_tmpB6B;void*_tmpB6C;void**_tmpB6D;void*
_tmpB72;void*_tmpB74;void*_tmpB75;void*_tmpB77;void*_tmpB79;struct Cyc_List_List*
_tmpB7B;_LL578: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpB15=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)
_tmpB14;if(_tmpB15->tag != 0)goto _LL57A;}_LL579: goto _LL577;_LL57A: {struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmpB16=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpB14;if(_tmpB16->tag != 1)
goto _LL57C;else{_tmpB17=_tmpB16->f1;_tmpB18=(struct Cyc_Core_Opt**)& _tmpB16->f1;
_tmpB19=(void**)& _tmpB16->f2;_tmpB1A=(void**)((void**)& _tmpB16->f2);}}_LL57B: if(*
_tmpB18 == 0  || Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct
Cyc_Core_Opt*)_check_null(*_tmpB18))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((
struct Cyc_Core_Opt*)_check_null(*_tmpB18))->v,expected_kind))*_tmpB18=Cyc_Tcutil_kind_to_opt(
expected_kind);if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind
== Cyc_Absyn_RgnKind){if(expected_kind->aliasqual == Cyc_Absyn_Unique)*_tmpB1A=(
void*)& Cyc_Absyn_UniqueRgn_val;else{*_tmpB1A=(void*)& Cyc_Absyn_HeapRgn_val;}}
else{if(cvtenv.generalize_evars){struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct
_tmp1437;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1436;struct Cyc_Absyn_Tvar*
_tmpB7C=Cyc_Tcutil_new_tvar((void*)((_tmp1436=_cycalloc(sizeof(*_tmp1436)),((
_tmp1436[0]=((_tmp1437.tag=2,((_tmp1437.f1=0,((_tmp1437.f2=expected_kind,
_tmp1437)))))),_tmp1436)))));{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp143A;
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1439;*_tmpB1A=(void*)((_tmp1439=
_cycalloc(sizeof(*_tmp1439)),((_tmp1439[0]=((_tmp143A.tag=2,((_tmp143A.f1=
_tmpB7C,_tmp143A)))),_tmp1439))));}_tmpB1C=_tmpB7C;goto _LL57D;}else{cvtenv.free_evars=
Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}goto
_LL577;_LL57C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpB1B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)
_tmpB14;if(_tmpB1B->tag != 2)goto _LL57E;else{_tmpB1C=_tmpB1B->f1;}}_LL57D:{void*
_tmpB81=Cyc_Absyn_compress_kb(_tmpB1C->kind);struct Cyc_Core_Opt*_tmpB83;struct
Cyc_Core_Opt**_tmpB84;_LL5AF: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*
_tmpB82=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB81;if(_tmpB82->tag
!= 1)goto _LL5B1;else{_tmpB83=_tmpB82->f1;_tmpB84=(struct Cyc_Core_Opt**)& _tmpB82->f1;}}
_LL5B0:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1440;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct
_tmp143F;struct Cyc_Core_Opt*_tmp143E;*_tmpB84=((_tmp143E=_cycalloc(sizeof(*
_tmp143E)),((_tmp143E->v=(void*)((_tmp1440=_cycalloc(sizeof(*_tmp1440)),((
_tmp1440[0]=((_tmp143F.tag=2,((_tmp143F.f1=0,((_tmp143F.f2=expected_kind,
_tmp143F)))))),_tmp1440)))),_tmp143E))));}goto _LL5AE;_LL5B1:;_LL5B2: goto _LL5AE;
_LL5AE:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpB1C);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpB1C,put_in_effect);{void*_tmpB88=Cyc_Absyn_compress_kb(_tmpB1C->kind);struct
Cyc_Core_Opt*_tmpB8A;struct Cyc_Core_Opt**_tmpB8B;struct Cyc_Absyn_Kind*_tmpB8C;
_LL5B4: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB89=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)
_tmpB88;if(_tmpB89->tag != 2)goto _LL5B6;else{_tmpB8A=_tmpB89->f1;_tmpB8B=(struct
Cyc_Core_Opt**)& _tmpB89->f1;_tmpB8C=_tmpB89->f2;}}_LL5B5: if(Cyc_Tcutil_kind_leq(
expected_kind,_tmpB8C)){struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1446;
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1445;struct Cyc_Core_Opt*
_tmp1444;*_tmpB8B=((_tmp1444=_cycalloc(sizeof(*_tmp1444)),((_tmp1444->v=(void*)((
_tmp1446=_cycalloc(sizeof(*_tmp1446)),((_tmp1446[0]=((_tmp1445.tag=2,((_tmp1445.f1=
0,((_tmp1445.f2=expected_kind,_tmp1445)))))),_tmp1446)))),_tmp1444))));}goto
_LL5B3;_LL5B6:;_LL5B7: goto _LL5B3;_LL5B3:;}goto _LL577;_LL57E: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*
_tmpB1D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmpB14;if(_tmpB1D->tag
!= 26)goto _LL580;else{_tmpB1E=_tmpB1D->f1;_tmpB1F=*_tmpB1E;_tmpB20=_tmpB1F.r;
_tmpB21=_tmpB1D->f2;_tmpB22=(void***)& _tmpB1D->f2;}}_LL57F: {void*new_t=Cyc_Tcutil_copy_type(
Cyc_Tcutil_compress(t));{void*_tmpB90=_tmpB20;struct Cyc_Absyn_Aggrdecl*_tmpB92;
struct Cyc_Absyn_Enumdecl*_tmpB94;struct Cyc_Absyn_Datatypedecl*_tmpB96;_LL5B9: {
struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpB91=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)
_tmpB90;if(_tmpB91->tag != 0)goto _LL5BB;else{_tmpB92=_tmpB91->f1;}}_LL5BA: Cyc_Tc_tcAggrdecl(
te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns),loc,_tmpB92);goto _LL5B8;_LL5BB: {struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*
_tmpB93=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmpB90;if(_tmpB93->tag
!= 1)goto _LL5BD;else{_tmpB94=_tmpB93->f1;}}_LL5BC: Cyc_Tc_tcEnumdecl(te,((struct
Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns),loc,_tmpB94);goto _LL5B8;_LL5BD: {struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*
_tmpB95=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmpB90;if(
_tmpB95->tag != 2)goto _LL5B8;else{_tmpB96=_tmpB95->f1;}}_LL5BE: Cyc_Tc_tcDatatypedecl(
te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns),loc,_tmpB96);goto _LL5B8;_LL5B8:;}{void**_tmp1447;*_tmpB22=((
_tmp1447=_cycalloc(sizeof(*_tmp1447)),((_tmp1447[0]=new_t,_tmp1447))));}return
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect);}
_LL580: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpB23=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)
_tmpB14;if(_tmpB23->tag != 14)goto _LL582;else{_tmpB24=_tmpB23->f1;}}_LL581: {
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct _RegionHandle _tmpB98=
_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpB98;_push_region(
uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=0;for(0;
_tmpB24 != 0;_tmpB24=_tmpB24->tl){struct Cyc_Absyn_Enumfield*_tmpB99=(struct Cyc_Absyn_Enumfield*)
_tmpB24->hd;if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),
struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,
prev_fields,(*_tmpB99->name).f2)){const char*_tmp144B;void*_tmp144A[1];struct Cyc_String_pa_PrintArg_struct
_tmp1449;(_tmp1449.tag=0,((_tmp1449.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*(*_tmpB99->name).f2),((_tmp144A[0]=& _tmp1449,Cyc_Tcutil_terr(
_tmpB99->loc,((_tmp144B="duplicate enum field name %s",_tag_dyneither(_tmp144B,
sizeof(char),29))),_tag_dyneither(_tmp144A,sizeof(void*),1)))))));}else{struct
Cyc_List_List*_tmp144C;prev_fields=((_tmp144C=_region_malloc(uprev_rgn,sizeof(*
_tmp144C)),((_tmp144C->hd=(*_tmpB99->name).f2,((_tmp144C->tl=prev_fields,
_tmp144C))))));}if(_tmpB99->tag == 0)_tmpB99->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmpB99->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmpB99->tag))){const char*_tmp1450;void*_tmp144F[1];struct Cyc_String_pa_PrintArg_struct
_tmp144E;(_tmp144E.tag=0,((_tmp144E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*(*_tmpB99->name).f2),((_tmp144F[0]=& _tmp144E,Cyc_Tcutil_terr(loc,((
_tmp1450="enum field %s: expression is not constant",_tag_dyneither(_tmp1450,
sizeof(char),42))),_tag_dyneither(_tmp144F,sizeof(void*),1)))))));}}{
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmpB99->tag))).f1;tag_count=t1 + 1;(*_tmpB99->name).f1=Cyc_Absyn_Abs_n(te->ns,0);{
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp1456;struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct
_tmp1455;struct _tuple29*_tmp1454;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmpB99->name).f2,(struct _tuple29*)((_tmp1454=_cycalloc(sizeof(*_tmp1454)),((
_tmp1454->f1=(void*)((_tmp1456=_cycalloc(sizeof(*_tmp1456)),((_tmp1456[0]=((
_tmp1455.tag=4,((_tmp1455.f1=(void*)t,((_tmp1455.f2=_tmpB99,_tmp1455)))))),
_tmp1456)))),((_tmp1454->f2=1,_tmp1454)))))));};};}}_npop_handler(0);goto _LL577;;
_pop_region(uprev_rgn);}_LL582: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmpB25=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpB14;if(_tmpB25->tag != 
13)goto _LL584;else{_tmpB26=_tmpB25->f1;_tmpB27=_tmpB25->f2;_tmpB28=(struct Cyc_Absyn_Enumdecl**)&
_tmpB25->f2;}}_LL583: if(*_tmpB28 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*
_tmpB28))->fields == 0){struct _handler_cons _tmpBA4;_push_handler(& _tmpBA4);{int
_tmpBA6=0;if(setjmp(_tmpBA4.handler))_tmpBA6=1;if(!_tmpBA6){{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpB26);*_tmpB28=(struct Cyc_Absyn_Enumdecl*)*
ed;};_pop_handler();}else{void*_tmpBA5=(void*)_exn_thrown;void*_tmpBA8=_tmpBA5;
_LL5C0: {struct Cyc_Dict_Absent_exn_struct*_tmpBA9=(struct Cyc_Dict_Absent_exn_struct*)
_tmpBA8;if(_tmpBA9->tag != Cyc_Dict_Absent)goto _LL5C2;}_LL5C1: {struct Cyc_Tcenv_Genv*
_tmpBAA=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Enumdecl*_tmp1457;struct Cyc_Absyn_Enumdecl*
_tmpBAB=(_tmp1457=_cycalloc(sizeof(*_tmp1457)),((_tmp1457->sc=Cyc_Absyn_Extern,((
_tmp1457->name=_tmpB26,((_tmp1457->fields=0,_tmp1457)))))));Cyc_Tc_tcEnumdecl(te,
_tmpBAA,loc,_tmpBAB);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,
loc,_tmpB26);*_tmpB28=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL5BF;};}_LL5C2:;
_LL5C3:(void)_throw(_tmpBA8);_LL5BF:;}};}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmpB28);*_tmpB26=(ed->name)[0];goto _LL577;};_LL584: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*
_tmpB29=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpB14;if(_tmpB29->tag
!= 3)goto _LL586;else{_tmpB2A=_tmpB29->f1;_tmpB2B=_tmpB2A.datatype_info;_tmpB2C=(
union Cyc_Absyn_DatatypeInfoU*)&(_tmpB29->f1).datatype_info;_tmpB2D=_tmpB2A.targs;
_tmpB2E=(struct Cyc_List_List**)&(_tmpB29->f1).targs;}}_LL585: {struct Cyc_List_List*
_tmpBAD=*_tmpB2E;{union Cyc_Absyn_DatatypeInfoU _tmpBAE=*_tmpB2C;struct Cyc_Absyn_UnknownDatatypeInfo
_tmpBAF;struct _tuple2*_tmpBB0;int _tmpBB1;struct Cyc_Absyn_Datatypedecl**_tmpBB2;
struct Cyc_Absyn_Datatypedecl*_tmpBB3;_LL5C5: if((_tmpBAE.UnknownDatatype).tag != 1)
goto _LL5C7;_tmpBAF=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpBAE.UnknownDatatype).val;
_tmpBB0=_tmpBAF.name;_tmpBB1=_tmpBAF.is_extensible;_LL5C6: {struct Cyc_Absyn_Datatypedecl**
tudp;{struct _handler_cons _tmpBB4;_push_handler(& _tmpBB4);{int _tmpBB6=0;if(setjmp(
_tmpBB4.handler))_tmpBB6=1;if(!_tmpBB6){tudp=Cyc_Tcenv_lookup_datatypedecl(te,
loc,_tmpBB0);;_pop_handler();}else{void*_tmpBB5=(void*)_exn_thrown;void*_tmpBB8=
_tmpBB5;_LL5CA: {struct Cyc_Dict_Absent_exn_struct*_tmpBB9=(struct Cyc_Dict_Absent_exn_struct*)
_tmpBB8;if(_tmpBB9->tag != Cyc_Dict_Absent)goto _LL5CC;}_LL5CB: {struct Cyc_Tcenv_Genv*
_tmpBBA=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Datatypedecl*_tmp1458;struct Cyc_Absyn_Datatypedecl*
_tmpBBB=(_tmp1458=_cycalloc(sizeof(*_tmp1458)),((_tmp1458->sc=Cyc_Absyn_Extern,((
_tmp1458->name=_tmpBB0,((_tmp1458->tvs=0,((_tmp1458->fields=0,((_tmp1458->is_extensible=
_tmpBB1,_tmp1458)))))))))));Cyc_Tc_tcDatatypedecl(te,_tmpBBA,loc,_tmpBBB);tudp=
Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpBB0);if(_tmpBAD != 0){{const char*
_tmp145C;void*_tmp145B[1];struct Cyc_String_pa_PrintArg_struct _tmp145A;(_tmp145A.tag=
0,((_tmp145A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBB0)),((_tmp145B[0]=& _tmp145A,Cyc_Tcutil_terr(loc,((_tmp145C="declare parameterized datatype %s before using",
_tag_dyneither(_tmp145C,sizeof(char),47))),_tag_dyneither(_tmp145B,sizeof(void*),
1)))))));}return cvtenv;}goto _LL5C9;}_LL5CC:;_LL5CD:(void)_throw(_tmpBB8);_LL5C9:;}};}
if(_tmpBB1  && !(*tudp)->is_extensible){const char*_tmp1460;void*_tmp145F[1];
struct Cyc_String_pa_PrintArg_struct _tmp145E;(_tmp145E.tag=0,((_tmp145E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBB0)),((
_tmp145F[0]=& _tmp145E,Cyc_Tcutil_terr(loc,((_tmp1460="datatype %s was not declared @extensible",
_tag_dyneither(_tmp1460,sizeof(char),41))),_tag_dyneither(_tmp145F,sizeof(void*),
1)))))));}*_tmpB2C=Cyc_Absyn_KnownDatatype(tudp);_tmpBB3=*tudp;goto _LL5C8;}
_LL5C7: if((_tmpBAE.KnownDatatype).tag != 2)goto _LL5C4;_tmpBB2=(struct Cyc_Absyn_Datatypedecl**)(
_tmpBAE.KnownDatatype).val;_tmpBB3=*_tmpBB2;_LL5C8: {struct Cyc_List_List*tvs=
_tmpBB3->tvs;for(0;_tmpBAD != 0  && tvs != 0;(_tmpBAD=_tmpBAD->tl,tvs=tvs->tl)){
void*t=(void*)_tmpBAD->hd;struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;{
struct _tuple0 _tmp1461;struct _tuple0 _tmpBC4=(_tmp1461.f1=Cyc_Absyn_compress_kb(tv->kind),((
_tmp1461.f2=t,_tmp1461)));void*_tmpBC5;void*_tmpBC7;struct Cyc_Absyn_Tvar*_tmpBC9;
_LL5CF: _tmpBC5=_tmpBC4.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*
_tmpBC6=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpBC5;if(_tmpBC6->tag
!= 1)goto _LL5D1;};_tmpBC7=_tmpBC4.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmpBC8=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpBC7;if(_tmpBC8->tag != 2)
goto _LL5D1;else{_tmpBC9=_tmpBC8->f1;}};_LL5D0: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(
loc,cvtenv.kind_env,_tmpBC9);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpBC9,1);continue;_LL5D1:;_LL5D2: goto _LL5CE;_LL5CE:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);Cyc_Tcutil_check_no_qual(loc,t);};}if(_tmpBAD != 0){const
char*_tmp1465;void*_tmp1464[1];struct Cyc_String_pa_PrintArg_struct _tmp1463;(
_tmp1463.tag=0,((_tmp1463.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBB3->name)),((_tmp1464[0]=& _tmp1463,Cyc_Tcutil_terr(loc,((_tmp1465="too many type arguments for datatype %s",
_tag_dyneither(_tmp1465,sizeof(char),40))),_tag_dyneither(_tmp1464,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_bk,expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*
_tmp1466;hidden_ts=((_tmp1466=_cycalloc(sizeof(*_tmp1466)),((_tmp1466->hd=e,((
_tmp1466->tl=hidden_ts,_tmp1466))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,k1,e,1);}*_tmpB2E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB2E,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}goto _LL5C4;}_LL5C4:;}goto
_LL577;}_LL586: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpB2F=(
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpB14;if(_tmpB2F->tag != 4)
goto _LL588;else{_tmpB30=_tmpB2F->f1;_tmpB31=_tmpB30.field_info;_tmpB32=(union Cyc_Absyn_DatatypeFieldInfoU*)&(
_tmpB2F->f1).field_info;_tmpB33=_tmpB30.targs;}}_LL587:{union Cyc_Absyn_DatatypeFieldInfoU
_tmpBCE=*_tmpB32;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpBCF;struct _tuple2*
_tmpBD0;struct _tuple2*_tmpBD1;int _tmpBD2;struct _tuple3 _tmpBD3;struct Cyc_Absyn_Datatypedecl*
_tmpBD4;struct Cyc_Absyn_Datatypefield*_tmpBD5;_LL5D4: if((_tmpBCE.UnknownDatatypefield).tag
!= 1)goto _LL5D6;_tmpBCF=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpBCE.UnknownDatatypefield).val;
_tmpBD0=_tmpBCF.datatype_name;_tmpBD1=_tmpBCF.field_name;_tmpBD2=_tmpBCF.is_extensible;
_LL5D5: {struct Cyc_Absyn_Datatypedecl*tud;struct Cyc_Absyn_Datatypefield*tuf;{
struct _handler_cons _tmpBD6;_push_handler(& _tmpBD6);{int _tmpBD8=0;if(setjmp(
_tmpBD6.handler))_tmpBD8=1;if(!_tmpBD8){*Cyc_Tcenv_lookup_datatypedecl(te,loc,
_tmpBD0);;_pop_handler();}else{void*_tmpBD7=(void*)_exn_thrown;void*_tmpBDA=
_tmpBD7;_LL5D9: {struct Cyc_Dict_Absent_exn_struct*_tmpBDB=(struct Cyc_Dict_Absent_exn_struct*)
_tmpBDA;if(_tmpBDB->tag != Cyc_Dict_Absent)goto _LL5DB;}_LL5DA:{const char*_tmp146A;
void*_tmp1469[1];struct Cyc_String_pa_PrintArg_struct _tmp1468;(_tmp1468.tag=0,((
_tmp1468.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBD0)),((_tmp1469[0]=& _tmp1468,Cyc_Tcutil_terr(loc,((_tmp146A="unbound datatype %s",
_tag_dyneither(_tmp146A,sizeof(char),20))),_tag_dyneither(_tmp1469,sizeof(void*),
1)))))));}return cvtenv;_LL5DB:;_LL5DC:(void)_throw(_tmpBDA);_LL5D8:;}};}{struct
_handler_cons _tmpBDF;_push_handler(& _tmpBDF);{int _tmpBE1=0;if(setjmp(_tmpBDF.handler))
_tmpBE1=1;if(!_tmpBE1){{struct _RegionHandle _tmpBE2=_new_region("r");struct
_RegionHandle*r=& _tmpBE2;_push_region(r);{void*_tmpBE3=Cyc_Tcenv_lookup_ordinary(
r,te,loc,_tmpBD1);struct Cyc_Absyn_Datatypedecl*_tmpBE5;struct Cyc_Absyn_Datatypefield*
_tmpBE6;_LL5DE: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpBE4=(
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpBE3;if(_tmpBE4->tag != 2)
goto _LL5E0;else{_tmpBE5=_tmpBE4->f1;_tmpBE6=_tmpBE4->f2;}}_LL5DF: tuf=_tmpBE6;tud=
_tmpBE5;if(_tmpBD2  && !tud->is_extensible){const char*_tmp146E;void*_tmp146D[1];
struct Cyc_String_pa_PrintArg_struct _tmp146C;(_tmp146C.tag=0,((_tmp146C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD0)),((
_tmp146D[0]=& _tmp146C,Cyc_Tcutil_terr(loc,((_tmp146E="datatype %s was not declared @extensible",
_tag_dyneither(_tmp146E,sizeof(char),41))),_tag_dyneither(_tmp146D,sizeof(void*),
1)))))));}goto _LL5DD;_LL5E0:;_LL5E1:{const char*_tmp1473;void*_tmp1472[2];struct
Cyc_String_pa_PrintArg_struct _tmp1471;struct Cyc_String_pa_PrintArg_struct
_tmp1470;(_tmp1470.tag=0,((_tmp1470.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD0)),((_tmp1471.tag=0,((_tmp1471.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD1)),((
_tmp1472[0]=& _tmp1471,((_tmp1472[1]=& _tmp1470,Cyc_Tcutil_terr(loc,((_tmp1473="unbound field %s in type datatype %s",
_tag_dyneither(_tmp1473,sizeof(char),37))),_tag_dyneither(_tmp1472,sizeof(void*),
2)))))))))))));}{struct Cyc_Tcutil_CVTEnv _tmpBEE=cvtenv;_npop_handler(1);return
_tmpBEE;};_LL5DD:;};_pop_region(r);};_pop_handler();}else{void*_tmpBE0=(void*)
_exn_thrown;void*_tmpBF0=_tmpBE0;_LL5E3: {struct Cyc_Dict_Absent_exn_struct*
_tmpBF1=(struct Cyc_Dict_Absent_exn_struct*)_tmpBF0;if(_tmpBF1->tag != Cyc_Dict_Absent)
goto _LL5E5;}_LL5E4:{const char*_tmp1478;void*_tmp1477[2];struct Cyc_String_pa_PrintArg_struct
_tmp1476;struct Cyc_String_pa_PrintArg_struct _tmp1475;(_tmp1475.tag=0,((_tmp1475.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD0)),((
_tmp1476.tag=0,((_tmp1476.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBD1)),((_tmp1477[0]=& _tmp1476,((_tmp1477[1]=& _tmp1475,Cyc_Tcutil_terr(loc,((
_tmp1478="unbound field %s in type datatype %s",_tag_dyneither(_tmp1478,sizeof(
char),37))),_tag_dyneither(_tmp1477,sizeof(void*),2)))))))))))));}return cvtenv;
_LL5E5:;_LL5E6:(void)_throw(_tmpBF0);_LL5E2:;}};}*_tmpB32=Cyc_Absyn_KnownDatatypefield(
tud,tuf);_tmpBD4=tud;_tmpBD5=tuf;goto _LL5D7;}_LL5D6: if((_tmpBCE.KnownDatatypefield).tag
!= 2)goto _LL5D3;_tmpBD3=(struct _tuple3)(_tmpBCE.KnownDatatypefield).val;_tmpBD4=
_tmpBD3.f1;_tmpBD5=_tmpBD3.f2;_LL5D7: {struct Cyc_List_List*tvs=_tmpBD4->tvs;for(
0;_tmpB33 != 0  && tvs != 0;(_tmpB33=_tmpB33->tl,tvs=tvs->tl)){void*t=(void*)
_tmpB33->hd;struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;{struct
_tuple0 _tmp1479;struct _tuple0 _tmpBF7=(_tmp1479.f1=Cyc_Absyn_compress_kb(tv->kind),((
_tmp1479.f2=t,_tmp1479)));void*_tmpBF8;void*_tmpBFA;struct Cyc_Absyn_Tvar*_tmpBFC;
_LL5E8: _tmpBF8=_tmpBF7.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*
_tmpBF9=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpBF8;if(_tmpBF9->tag
!= 1)goto _LL5EA;};_tmpBFA=_tmpBF7.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmpBFB=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpBFA;if(_tmpBFB->tag != 2)
goto _LL5EA;else{_tmpBFC=_tmpBFB->f1;}};_LL5E9: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(
loc,cvtenv.kind_env,_tmpBFC);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpBFC,1);continue;_LL5EA:;_LL5EB: goto _LL5E7;_LL5E7:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);Cyc_Tcutil_check_no_qual(loc,t);};}if(_tmpB33 != 0){const
char*_tmp147E;void*_tmp147D[2];struct Cyc_String_pa_PrintArg_struct _tmp147C;
struct Cyc_String_pa_PrintArg_struct _tmp147B;(_tmp147B.tag=0,((_tmp147B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD5->name)),((
_tmp147C.tag=0,((_tmp147C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBD4->name)),((_tmp147D[0]=& _tmp147C,((_tmp147D[1]=& _tmp147B,Cyc_Tcutil_terr(
loc,((_tmp147E="too many type arguments for datatype %s.%s",_tag_dyneither(
_tmp147E,sizeof(char),43))),_tag_dyneither(_tmp147D,sizeof(void*),2)))))))))))));}
if(tvs != 0){const char*_tmp1483;void*_tmp1482[2];struct Cyc_String_pa_PrintArg_struct
_tmp1481;struct Cyc_String_pa_PrintArg_struct _tmp1480;(_tmp1480.tag=0,((_tmp1480.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD5->name)),((
_tmp1481.tag=0,((_tmp1481.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBD4->name)),((_tmp1482[0]=& _tmp1481,((_tmp1482[1]=& _tmp1480,Cyc_Tcutil_terr(
loc,((_tmp1483="too few type arguments for datatype %s.%s",_tag_dyneither(
_tmp1483,sizeof(char),42))),_tag_dyneither(_tmp1482,sizeof(void*),2)))))))))))));}
goto _LL5D3;}_LL5D3:;}goto _LL577;_LL588: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmpB34=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB14;if(_tmpB34->tag
!= 5)goto _LL58A;else{_tmpB35=_tmpB34->f1;_tmpB36=_tmpB35.elt_typ;_tmpB37=_tmpB35.elt_tq;
_tmpB38=(struct Cyc_Absyn_Tqual*)&(_tmpB34->f1).elt_tq;_tmpB39=_tmpB35.ptr_atts;
_tmpB3A=_tmpB39.rgn;_tmpB3B=_tmpB39.nullable;_tmpB3C=_tmpB39.bounds;_tmpB3D=
_tmpB39.zero_term;}}_LL589: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB36,1);_tmpB38->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB38->print_const,_tmpB36);{struct Cyc_Absyn_Kind*k;switch(expected_kind->aliasqual){
case Cyc_Absyn_Aliasable: _LL5EC: k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique:
_LL5ED: k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL5EE: k=& Cyc_Tcutil_trk;break;}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmpB3A,1);{union Cyc_Absyn_Constraint*
_tmpC05=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
_tmpB3D);union Cyc_Absyn_Constraint _tmpC06;int _tmpC07;union Cyc_Absyn_Constraint
_tmpC08;int _tmpC09;_LL5F1: _tmpC06=*_tmpC05;if((_tmpC06.No_constr).tag != 3)goto
_LL5F3;_tmpC07=(int)(_tmpC06.No_constr).val;_LL5F2:{void*_tmpC0A=Cyc_Tcutil_compress(
_tmpB36);enum Cyc_Absyn_Size_of _tmpC0C;_LL5F8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmpC0B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC0A;if(_tmpC0B->tag != 6)
goto _LL5FA;else{_tmpC0C=_tmpC0B->f2;if(_tmpC0C != Cyc_Absyn_Char_sz)goto _LL5FA;}}
_LL5F9:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB3D,Cyc_Absyn_true_conref);
is_zero_terminated=1;goto _LL5F7;_LL5FA:;_LL5FB:((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmpB3D,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL5F7;
_LL5F7:;}goto _LL5F0;_LL5F3: _tmpC08=*_tmpC05;if((_tmpC08.Eq_constr).tag != 1)goto
_LL5F5;_tmpC09=(int)(_tmpC08.Eq_constr).val;if(_tmpC09 != 1)goto _LL5F5;_LL5F4: if(
!Cyc_Tcutil_admits_zero(_tmpB36)){const char*_tmp1487;void*_tmp1486[1];struct Cyc_String_pa_PrintArg_struct
_tmp1485;(_tmp1485.tag=0,((_tmp1485.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB36)),((_tmp1486[0]=& _tmp1485,Cyc_Tcutil_terr(
loc,((_tmp1487="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmp1487,sizeof(char),53))),_tag_dyneither(_tmp1486,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL5F0;_LL5F5:;_LL5F6: is_zero_terminated=0;
goto _LL5F0;_LL5F0:;}{void*_tmpC10=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpB3C);struct Cyc_Absyn_Exp*
_tmpC13;_LL5FD: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpC11=(struct
Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpC10;if(_tmpC11->tag != 0)goto
_LL5FF;}_LL5FE: goto _LL5FC;_LL5FF: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmpC12=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpC10;if(_tmpC12->tag != 
1)goto _LL5FC;else{_tmpC13=_tmpC12->f1;}}_LL600: {struct _RegionHandle _tmpC14=
_new_region("temp");struct _RegionHandle*temp=& _tmpC14;_push_region(temp);{struct
Cyc_Tcenv_Tenv*_tmpC15=Cyc_Tcenv_allow_valueof(temp,te);Cyc_Tcexp_tcExp(_tmpC15,
0,_tmpC13);}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC13,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpC13)){const char*_tmp148A;void*_tmp1489;(
_tmp1489=0,Cyc_Tcutil_terr(loc,((_tmp148A="pointer bounds expression is not an unsigned int",
_tag_dyneither(_tmp148A,sizeof(char),49))),_tag_dyneither(_tmp1489,sizeof(void*),
0)));}{unsigned int _tmpC19;int _tmpC1A;struct _tuple14 _tmpC18=Cyc_Evexp_eval_const_uint_exp(
_tmpC13);_tmpC19=_tmpC18.f1;_tmpC1A=_tmpC18.f2;if(is_zero_terminated  && (!
_tmpC1A  || _tmpC19 < 1)){const char*_tmp148D;void*_tmp148C;(_tmp148C=0,Cyc_Tcutil_terr(
loc,((_tmp148D="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmp148D,sizeof(char),55))),_tag_dyneither(_tmp148C,sizeof(void*),
0)));}_npop_handler(0);goto _LL5FC;};;_pop_region(temp);}_LL5FC:;}goto _LL577;};}
_LL58A: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB3E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)
_tmpB14;if(_tmpB3E->tag != 19)goto _LL58C;else{_tmpB3F=(void*)_tmpB3E->f1;}}_LL58B:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB3F,1);
goto _LL577;_LL58C: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpB40=(
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmpB14;if(_tmpB40->tag != 18)goto
_LL58E;else{_tmpB41=_tmpB40->f1;}}_LL58D: {struct _RegionHandle _tmpC1D=
_new_region("temp");struct _RegionHandle*temp=& _tmpC1D;_push_region(temp);{struct
Cyc_Tcenv_Tenv*_tmpC1E=Cyc_Tcenv_allow_valueof(temp,te);Cyc_Tcexp_tcExp(_tmpC1E,
0,_tmpB41);}if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpB41)){const char*_tmp1490;void*
_tmp148F;(_tmp148F=0,Cyc_Tcutil_terr(loc,((_tmp1490="valueof_t requires an int expression",
_tag_dyneither(_tmp1490,sizeof(char),37))),_tag_dyneither(_tmp148F,sizeof(void*),
0)));}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB41,te,cvtenv);
_npop_handler(0);goto _LL577;;_pop_region(temp);}_LL58E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmpB42=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpB14;if(_tmpB42->tag != 6)
goto _LL590;}_LL58F: goto _LL591;_LL590: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmpB43=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpB14;if(_tmpB43->tag != 
7)goto _LL592;}_LL591: goto _LL577;_LL592: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmpB44=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB14;if(_tmpB44->tag != 
8)goto _LL594;else{_tmpB45=_tmpB44->f1;_tmpB46=_tmpB45.elt_type;_tmpB47=_tmpB45.tq;
_tmpB48=(struct Cyc_Absyn_Tqual*)&(_tmpB44->f1).tq;_tmpB49=_tmpB45.num_elts;
_tmpB4A=(struct Cyc_Absyn_Exp**)&(_tmpB44->f1).num_elts;_tmpB4B=_tmpB45.zero_term;
_tmpB4C=_tmpB45.zt_loc;}}_LL593: {struct Cyc_Absyn_Exp*_tmpC21=*_tmpB4A;cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpB46,1);_tmpB48->real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB48->print_const,_tmpB46);{int
is_zero_terminated;{union Cyc_Absyn_Constraint*_tmpC22=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmpB4B);union Cyc_Absyn_Constraint
_tmpC23;int _tmpC24;union Cyc_Absyn_Constraint _tmpC25;int _tmpC26;_LL602: _tmpC23=*
_tmpC22;if((_tmpC23.No_constr).tag != 3)goto _LL604;_tmpC24=(int)(_tmpC23.No_constr).val;
_LL603:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB4B,Cyc_Absyn_false_conref);
is_zero_terminated=0;goto _LL601;_LL604: _tmpC25=*_tmpC22;if((_tmpC25.Eq_constr).tag
!= 1)goto _LL606;_tmpC26=(int)(_tmpC25.Eq_constr).val;if(_tmpC26 != 1)goto _LL606;
_LL605: if(!Cyc_Tcutil_admits_zero(_tmpB46)){const char*_tmp1494;void*_tmp1493[1];
struct Cyc_String_pa_PrintArg_struct _tmp1492;(_tmp1492.tag=0,((_tmp1492.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB46)),((
_tmp1493[0]=& _tmp1492,Cyc_Tcutil_terr(loc,((_tmp1494="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmp1494,sizeof(char),51))),_tag_dyneither(_tmp1493,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL601;_LL606:;_LL607: is_zero_terminated=0;
goto _LL601;_LL601:;}if(_tmpC21 == 0){if(is_zero_terminated)*_tmpB4A=(_tmpC21=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{{const char*_tmp1497;void*
_tmp1496;(_tmp1496=0,Cyc_Tcutil_warn(loc,((_tmp1497="array bound defaults to 1 here",
_tag_dyneither(_tmp1497,sizeof(char),31))),_tag_dyneither(_tmp1496,sizeof(void*),
0)));}*_tmpB4A=(_tmpC21=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmpC21);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmpC21)){const char*_tmp149A;void*_tmp1499;(_tmp1499=0,Cyc_Tcutil_terr(loc,((
_tmp149A="array bounds expression is not constant",_tag_dyneither(_tmp149A,
sizeof(char),40))),_tag_dyneither(_tmp1499,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_tmpC21)){const char*_tmp149D;void*_tmp149C;(_tmp149C=0,
Cyc_Tcutil_terr(loc,((_tmp149D="array bounds expression is not an unsigned int",
_tag_dyneither(_tmp149D,sizeof(char),47))),_tag_dyneither(_tmp149C,sizeof(void*),
0)));}{unsigned int _tmpC31;int _tmpC32;struct _tuple14 _tmpC30=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmpC21);_tmpC31=_tmpC30.f1;_tmpC32=_tmpC30.f2;if((
is_zero_terminated  && _tmpC32) && _tmpC31 < 1){const char*_tmp14A0;void*_tmp149F;(
_tmp149F=0,Cyc_Tcutil_warn(loc,((_tmp14A0="zero terminated array cannot have zero elements",
_tag_dyneither(_tmp14A0,sizeof(char),48))),_tag_dyneither(_tmp149F,sizeof(void*),
0)));}if((_tmpC32  && _tmpC31 < 1) && Cyc_Cyclone_tovc_r){{const char*_tmp14A3;void*
_tmp14A2;(_tmp14A2=0,Cyc_Tcutil_warn(loc,((_tmp14A3="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmp14A3,sizeof(char),75))),_tag_dyneither(_tmp14A2,sizeof(void*),
0)));}*_tmpB4A=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL577;};};}
_LL594: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB4D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)
_tmpB14;if(_tmpB4D->tag != 9)goto _LL596;else{_tmpB4E=_tmpB4D->f1;_tmpB4F=_tmpB4E.tvars;
_tmpB50=(struct Cyc_List_List**)&(_tmpB4D->f1).tvars;_tmpB51=_tmpB4E.effect;
_tmpB52=(void**)&(_tmpB4D->f1).effect;_tmpB53=_tmpB4E.ret_tqual;_tmpB54=(struct
Cyc_Absyn_Tqual*)&(_tmpB4D->f1).ret_tqual;_tmpB55=_tmpB4E.ret_typ;_tmpB56=
_tmpB4E.args;_tmpB57=_tmpB4E.c_varargs;_tmpB58=_tmpB4E.cyc_varargs;_tmpB59=
_tmpB4E.rgn_po;_tmpB5A=_tmpB4E.attributes;}}_LL595: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;enum Cyc_Absyn_Format_Type
ft=Cyc_Absyn_Printf_ft;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmpB5A != 0;
_tmpB5A=_tmpB5A->tl){if(!Cyc_Absyn_fntype_att((void*)_tmpB5A->hd)){const char*
_tmp14A7;void*_tmp14A6[1];struct Cyc_String_pa_PrintArg_struct _tmp14A5;(_tmp14A5.tag=
0,((_tmp14A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmpB5A->hd)),((_tmp14A6[0]=& _tmp14A5,Cyc_Tcutil_terr(loc,((_tmp14A7="bad function type attribute %s",
_tag_dyneither(_tmp14A7,sizeof(char),31))),_tag_dyneither(_tmp14A6,sizeof(void*),
1)))))));}{void*_tmpC3A=(void*)_tmpB5A->hd;enum Cyc_Absyn_Format_Type _tmpC3F;int
_tmpC40;int _tmpC41;_LL609: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*
_tmpC3B=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpC3A;if(_tmpC3B->tag
!= 1)goto _LL60B;}_LL60A: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL608;
_LL60B: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpC3C=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)
_tmpC3A;if(_tmpC3C->tag != 2)goto _LL60D;}_LL60C: if(!seen_cdecl){seen_cdecl=1;++
num_convs;}goto _LL608;_LL60D: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*
_tmpC3D=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpC3A;if(_tmpC3D->tag
!= 3)goto _LL60F;}_LL60E: if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto
_LL608;_LL60F: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpC3E=(
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC3A;if(_tmpC3E->tag != 19)
goto _LL611;else{_tmpC3F=_tmpC3E->f1;_tmpC40=_tmpC3E->f2;_tmpC41=_tmpC3E->f3;}}
_LL610: if(!seen_format){seen_format=1;ft=_tmpC3F;fmt_desc_arg=_tmpC40;
fmt_arg_start=_tmpC41;}else{const char*_tmp14AA;void*_tmp14A9;(_tmp14A9=0,Cyc_Tcutil_terr(
loc,((_tmp14AA="function can't have multiple format attributes",_tag_dyneither(
_tmp14AA,sizeof(char),47))),_tag_dyneither(_tmp14A9,sizeof(void*),0)));}goto
_LL608;_LL611:;_LL612: goto _LL608;_LL608:;};}if(num_convs > 1){const char*_tmp14AD;
void*_tmp14AC;(_tmp14AC=0,Cyc_Tcutil_terr(loc,((_tmp14AD="function can't have multiple calling conventions",
_tag_dyneither(_tmp14AD,sizeof(char),49))),_tag_dyneither(_tmp14AC,sizeof(void*),
0)));}Cyc_Tcutil_check_unique_tvars(loc,*_tmpB50);{struct Cyc_List_List*b=*
_tmpB50;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmpC46=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);
struct Cyc_Absyn_Kind*_tmpC48;struct Cyc_Absyn_Kind _tmpC49;enum Cyc_Absyn_KindQual
_tmpC4A;_LL614: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpC47=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)
_tmpC46;if(_tmpC47->tag != 0)goto _LL616;else{_tmpC48=_tmpC47->f1;_tmpC49=*_tmpC48;
_tmpC4A=_tmpC49.kind;if(_tmpC4A != Cyc_Absyn_MemKind)goto _LL616;}}_LL615:{const
char*_tmp14B1;void*_tmp14B0[1];struct Cyc_String_pa_PrintArg_struct _tmp14AF;(
_tmp14AF.tag=0,((_tmp14AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmp14B0[0]=& _tmp14AF,Cyc_Tcutil_terr(loc,((
_tmp14B1="function attempts to abstract Mem type variable %s",_tag_dyneither(
_tmp14B1,sizeof(char),51))),_tag_dyneither(_tmp14B0,sizeof(void*),1)))))));}goto
_LL613;_LL616:;_LL617: goto _LL613;_LL613:;};}}{struct _RegionHandle _tmpC4E=
_new_region("newr");struct _RegionHandle*newr=& _tmpC4E;_push_region(newr);{struct
Cyc_Tcutil_CVTEnv _tmp14B2;struct Cyc_Tcutil_CVTEnv _tmpC4F=(_tmp14B2.r=newr,((
_tmp14B2.kind_env=cvtenv.kind_env,((_tmp14B2.free_vars=0,((_tmp14B2.free_evars=0,((
_tmp14B2.generalize_evars=cvtenv.generalize_evars,((_tmp14B2.fn_result=1,
_tmp14B2)))))))))));_tmpC4F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC4F,& Cyc_Tcutil_tmk,
_tmpB55,1);_tmpB54->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB54->print_const,
_tmpB55);_tmpC4F.fn_result=0;{struct Cyc_List_List*a=_tmpB56;for(0;a != 0;a=a->tl){
struct _tuple10*_tmpC50=(struct _tuple10*)a->hd;void*_tmpC51=(*_tmpC50).f3;_tmpC4F=
Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC4F,& Cyc_Tcutil_tmk,_tmpC51,1);{int
_tmpC52=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpC50).f2).print_const,
_tmpC51);((*_tmpC50).f2).real_const=_tmpC52;{void*_tmpC53=Cyc_Tcutil_compress(
_tmpC51);struct Cyc_Absyn_ArrayInfo _tmpC55;void*_tmpC56;struct Cyc_Absyn_Tqual
_tmpC57;struct Cyc_Absyn_Exp*_tmpC58;union Cyc_Absyn_Constraint*_tmpC59;
unsigned int _tmpC5A;_LL619: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC54=(
struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC53;if(_tmpC54->tag != 8)goto
_LL61B;else{_tmpC55=_tmpC54->f1;_tmpC56=_tmpC55.elt_type;_tmpC57=_tmpC55.tq;
_tmpC58=_tmpC55.num_elts;_tmpC59=_tmpC55.zero_term;_tmpC5A=_tmpC55.zt_loc;}}
_LL61A: {void*_tmpC5B=Cyc_Absyn_new_evar(0,0);_tmpC4F=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC4F,& Cyc_Tcutil_rk,_tmpC5B,1);{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct
_tmp14B5;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp14B4;void*_tmpC5C=Cyc_Absyn_atb_typ(
_tmpC56,_tmpC5B,_tmpC57,(void*)((_tmp14B4=_cycalloc(sizeof(*_tmp14B4)),((
_tmp14B4[0]=((_tmp14B5.tag=1,((_tmp14B5.f1=(struct Cyc_Absyn_Exp*)_check_null(
_tmpC58),_tmp14B5)))),_tmp14B4)))),_tmpC59);(*_tmpC50).f3=_tmpC5C;goto _LL618;};}
_LL61B:;_LL61C: goto _LL618;_LL618:;};};}}if(_tmpB58 != 0){if(_tmpB57){const char*
_tmp14B8;void*_tmp14B7;(_tmp14B7=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp14B8="both c_vararg and cyc_vararg",
_tag_dyneither(_tmp14B8,sizeof(char),29))),_tag_dyneither(_tmp14B7,sizeof(void*),
0)));}{void*_tmpC62;int _tmpC63;struct Cyc_Absyn_VarargInfo _tmpC61=*_tmpB58;
_tmpC62=_tmpC61.type;_tmpC63=_tmpC61.inject;_tmpC4F=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC4F,& Cyc_Tcutil_tmk,_tmpC62,1);(_tmpB58->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmpB58->tq).print_const,_tmpC62);if(_tmpC63){void*_tmpC64=Cyc_Tcutil_compress(
_tmpC62);struct Cyc_Absyn_PtrInfo _tmpC66;void*_tmpC67;struct Cyc_Absyn_PtrAtts
_tmpC68;union Cyc_Absyn_Constraint*_tmpC69;union Cyc_Absyn_Constraint*_tmpC6A;
_LL61E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC65=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmpC64;if(_tmpC65->tag != 5)goto _LL620;else{_tmpC66=_tmpC65->f1;_tmpC67=_tmpC66.elt_typ;
_tmpC68=_tmpC66.ptr_atts;_tmpC69=_tmpC68.bounds;_tmpC6A=_tmpC68.zero_term;}}
_LL61F:{void*_tmpC6B=Cyc_Tcutil_compress(_tmpC67);_LL623: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*
_tmpC6C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpC6B;if(_tmpC6C->tag
!= 3)goto _LL625;}_LL624: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,_tmpC6A)){const char*_tmp14BB;void*_tmp14BA;(_tmp14BA=0,Cyc_Tcutil_terr(loc,((
_tmp14BB="can't inject into a zeroterm pointer",_tag_dyneither(_tmp14BB,sizeof(
char),37))),_tag_dyneither(_tmp14BA,sizeof(void*),0)));}{void*_tmpC6F=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,
_tmpC69);_LL628: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpC70=(
struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpC6F;if(_tmpC70->tag != 0)
goto _LL62A;}_LL629:{const char*_tmp14BE;void*_tmp14BD;(_tmp14BD=0,Cyc_Tcutil_terr(
loc,((_tmp14BE="can't inject into a fat pointer to datatype",_tag_dyneither(
_tmp14BE,sizeof(char),44))),_tag_dyneither(_tmp14BD,sizeof(void*),0)));}goto
_LL627;_LL62A:;_LL62B: goto _LL627;_LL627:;}goto _LL622;_LL625:;_LL626:{const char*
_tmp14C1;void*_tmp14C0;(_tmp14C0=0,Cyc_Tcutil_terr(loc,((_tmp14C1="can't inject a non-datatype type",
_tag_dyneither(_tmp14C1,sizeof(char),33))),_tag_dyneither(_tmp14C0,sizeof(void*),
0)));}goto _LL622;_LL622:;}goto _LL61D;_LL620:;_LL621:{const char*_tmp14C4;void*
_tmp14C3;(_tmp14C3=0,Cyc_Tcutil_terr(loc,((_tmp14C4="expecting a datatype pointer type",
_tag_dyneither(_tmp14C4,sizeof(char),34))),_tag_dyneither(_tmp14C3,sizeof(void*),
0)));}goto _LL61D;_LL61D:;}};}if(seen_format){int _tmpC77=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmpB56);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpC77) || 
fmt_arg_start < 0) || _tmpB58 == 0  && fmt_arg_start != 0) || _tmpB58 != 0  && 
fmt_arg_start != _tmpC77 + 1){const char*_tmp14C7;void*_tmp14C6;(_tmp14C6=0,Cyc_Tcutil_terr(
loc,((_tmp14C7="bad format descriptor",_tag_dyneither(_tmp14C7,sizeof(char),22))),
_tag_dyneither(_tmp14C6,sizeof(void*),0)));}else{void*_tmpC7B;struct _tuple10
_tmpC7A=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpB56,
fmt_desc_arg - 1);_tmpC7B=_tmpC7A.f3;{void*_tmpC7C=Cyc_Tcutil_compress(_tmpC7B);
struct Cyc_Absyn_PtrInfo _tmpC7E;void*_tmpC7F;struct Cyc_Absyn_PtrAtts _tmpC80;union
Cyc_Absyn_Constraint*_tmpC81;union Cyc_Absyn_Constraint*_tmpC82;_LL62D: {struct
Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC7D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmpC7C;if(_tmpC7D->tag != 5)goto _LL62F;else{_tmpC7E=_tmpC7D->f1;_tmpC7F=_tmpC7E.elt_typ;
_tmpC80=_tmpC7E.ptr_atts;_tmpC81=_tmpC80.bounds;_tmpC82=_tmpC80.zero_term;}}
_LL62E:{struct _tuple0 _tmp14C8;struct _tuple0 _tmpC84=(_tmp14C8.f1=Cyc_Tcutil_compress(
_tmpC7F),((_tmp14C8.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmpC81),_tmp14C8)));void*_tmpC85;enum Cyc_Absyn_Sign
_tmpC87;enum Cyc_Absyn_Size_of _tmpC88;void*_tmpC89;_LL632: _tmpC85=_tmpC84.f1;{
struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpC86=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmpC85;if(_tmpC86->tag != 6)goto _LL634;else{_tmpC87=_tmpC86->f1;if(_tmpC87 != Cyc_Absyn_None)
goto _LL634;_tmpC88=_tmpC86->f2;if(_tmpC88 != Cyc_Absyn_Char_sz)goto _LL634;}};
_tmpC89=_tmpC84.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpC8A=(
struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpC89;if(_tmpC8A->tag != 0)
goto _LL634;};_LL633: goto _LL631;_LL634:;_LL635:{const char*_tmp14CB;void*_tmp14CA;(
_tmp14CA=0,Cyc_Tcutil_terr(loc,((_tmp14CB="format descriptor is not a char ? type",
_tag_dyneither(_tmp14CB,sizeof(char),39))),_tag_dyneither(_tmp14CA,sizeof(void*),
0)));}goto _LL631;_LL631:;}goto _LL62C;_LL62F:;_LL630:{const char*_tmp14CE;void*
_tmp14CD;(_tmp14CD=0,Cyc_Tcutil_terr(loc,((_tmp14CE="format descriptor is not a char ? type",
_tag_dyneither(_tmp14CE,sizeof(char),39))),_tag_dyneither(_tmp14CD,sizeof(void*),
0)));}goto _LL62C;_LL62C:;}if(fmt_arg_start != 0){int problem;{struct _tuple30
_tmp14CF;struct _tuple30 _tmpC90=(_tmp14CF.f1=ft,((_tmp14CF.f2=Cyc_Tcutil_compress(
Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpB58))->type)),
_tmp14CF)));enum Cyc_Absyn_Format_Type _tmpC91;void*_tmpC92;struct Cyc_Absyn_DatatypeInfo
_tmpC94;union Cyc_Absyn_DatatypeInfoU _tmpC95;struct Cyc_Absyn_Datatypedecl**
_tmpC96;struct Cyc_Absyn_Datatypedecl*_tmpC97;enum Cyc_Absyn_Format_Type _tmpC98;
void*_tmpC99;struct Cyc_Absyn_DatatypeInfo _tmpC9B;union Cyc_Absyn_DatatypeInfoU
_tmpC9C;struct Cyc_Absyn_Datatypedecl**_tmpC9D;struct Cyc_Absyn_Datatypedecl*
_tmpC9E;_LL637: _tmpC91=_tmpC90.f1;if(_tmpC91 != Cyc_Absyn_Printf_ft)goto _LL639;
_tmpC92=_tmpC90.f2;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpC93=(
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpC92;if(_tmpC93->tag != 3)goto
_LL639;else{_tmpC94=_tmpC93->f1;_tmpC95=_tmpC94.datatype_info;if((_tmpC95.KnownDatatype).tag
!= 2)goto _LL639;_tmpC96=(struct Cyc_Absyn_Datatypedecl**)(_tmpC95.KnownDatatype).val;
_tmpC97=*_tmpC96;}};_LL638: problem=Cyc_Absyn_qvar_cmp(_tmpC97->name,Cyc_Absyn_datatype_print_arg_qvar)
!= 0;goto _LL636;_LL639: _tmpC98=_tmpC90.f1;if(_tmpC98 != Cyc_Absyn_Scanf_ft)goto
_LL63B;_tmpC99=_tmpC90.f2;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*
_tmpC9A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpC99;if(_tmpC9A->tag
!= 3)goto _LL63B;else{_tmpC9B=_tmpC9A->f1;_tmpC9C=_tmpC9B.datatype_info;if((
_tmpC9C.KnownDatatype).tag != 2)goto _LL63B;_tmpC9D=(struct Cyc_Absyn_Datatypedecl**)(
_tmpC9C.KnownDatatype).val;_tmpC9E=*_tmpC9D;}};_LL63A: problem=Cyc_Absyn_qvar_cmp(
_tmpC9E->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL636;_LL63B:;_LL63C:
problem=1;goto _LL636;_LL636:;}if(problem){const char*_tmp14D2;void*_tmp14D1;(
_tmp14D1=0,Cyc_Tcutil_terr(loc,((_tmp14D2="format attribute and vararg types don't match",
_tag_dyneither(_tmp14D2,sizeof(char),46))),_tag_dyneither(_tmp14D1,sizeof(void*),
0)));}}}}{struct Cyc_List_List*rpo=_tmpB59;for(0;rpo != 0;rpo=rpo->tl){struct
_tuple0 _tmpCA2;void*_tmpCA3;void*_tmpCA4;struct _tuple0*_tmpCA1=(struct _tuple0*)
rpo->hd;_tmpCA2=*_tmpCA1;_tmpCA3=_tmpCA2.f1;_tmpCA4=_tmpCA2.f2;_tmpC4F=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC4F,& Cyc_Tcutil_ek,_tmpCA3,1);_tmpC4F=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC4F,& Cyc_Tcutil_trk,_tmpCA4,1);}}if(*_tmpB52 != 0)_tmpC4F=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC4F,& Cyc_Tcutil_ek,(void*)_check_null(*_tmpB52),1);else{struct Cyc_List_List*
effect=0;{struct Cyc_List_List*tvs=_tmpC4F.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmpCA6;int _tmpCA7;struct _tuple28 _tmpCA5=*((struct _tuple28*)
tvs->hd);_tmpCA6=_tmpCA5.f1;_tmpCA7=_tmpCA5.f2;if(!_tmpCA7)continue;{void*
_tmpCA8=Cyc_Absyn_compress_kb(_tmpCA6->kind);struct Cyc_Core_Opt*_tmpCAA;struct
Cyc_Core_Opt**_tmpCAB;struct Cyc_Absyn_Kind*_tmpCAC;struct Cyc_Absyn_Kind*_tmpCAE;
struct Cyc_Absyn_Kind*_tmpCB0;struct Cyc_Absyn_Kind _tmpCB1;enum Cyc_Absyn_KindQual
_tmpCB2;struct Cyc_Absyn_Kind*_tmpCB4;struct Cyc_Absyn_Kind _tmpCB5;enum Cyc_Absyn_KindQual
_tmpCB6;struct Cyc_Core_Opt*_tmpCB8;struct Cyc_Core_Opt**_tmpCB9;struct Cyc_Absyn_Kind*
_tmpCBA;struct Cyc_Absyn_Kind _tmpCBB;enum Cyc_Absyn_KindQual _tmpCBC;struct Cyc_Absyn_Kind*
_tmpCBE;struct Cyc_Absyn_Kind _tmpCBF;enum Cyc_Absyn_KindQual _tmpCC0;struct Cyc_Core_Opt*
_tmpCC2;struct Cyc_Core_Opt**_tmpCC3;_LL63E:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmpCA9=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCA8;if(_tmpCA9->tag
!= 2)goto _LL640;else{_tmpCAA=_tmpCA9->f1;_tmpCAB=(struct Cyc_Core_Opt**)& _tmpCA9->f1;
_tmpCAC=_tmpCA9->f2;}}if(!(_tmpCAC->kind == Cyc_Absyn_RgnKind))goto _LL640;_LL63F:
if(_tmpCAC->aliasqual == Cyc_Absyn_Top){*_tmpCAB=Cyc_Tcutil_kind_to_bound_opt(&
Cyc_Tcutil_rk);_tmpCAE=_tmpCAC;goto _LL641;}*_tmpCAB=Cyc_Tcutil_kind_to_bound_opt(
_tmpCAC);_tmpCAE=_tmpCAC;goto _LL641;_LL640:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*
_tmpCAD=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpCA8;if(_tmpCAD->tag != 
0)goto _LL642;else{_tmpCAE=_tmpCAD->f1;}}if(!(_tmpCAE->kind == Cyc_Absyn_RgnKind))
goto _LL642;_LL641:{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp14E1;struct
Cyc_Absyn_VarType_Absyn_Type_struct*_tmp14E0;struct Cyc_Absyn_VarType_Absyn_Type_struct
_tmp14DF;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp14DE;struct Cyc_List_List*
_tmp14DD;effect=((_tmp14DD=_cycalloc(sizeof(*_tmp14DD)),((_tmp14DD->hd=(void*)((
_tmp14E1=_cycalloc(sizeof(*_tmp14E1)),((_tmp14E1[0]=((_tmp14DE.tag=23,((_tmp14DE.f1=(
void*)((void*)((_tmp14E0=_cycalloc(sizeof(*_tmp14E0)),((_tmp14E0[0]=((_tmp14DF.tag=
2,((_tmp14DF.f1=_tmpCA6,_tmp14DF)))),_tmp14E0))))),_tmp14DE)))),_tmp14E1)))),((
_tmp14DD->tl=effect,_tmp14DD))))));}goto _LL63D;_LL642: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmpCAF=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCA8;if(_tmpCAF->tag
!= 2)goto _LL644;else{_tmpCB0=_tmpCAF->f2;_tmpCB1=*_tmpCB0;_tmpCB2=_tmpCB1.kind;
if(_tmpCB2 != Cyc_Absyn_IntKind)goto _LL644;}}_LL643: goto _LL645;_LL644: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*
_tmpCB3=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpCA8;if(_tmpCB3->tag != 
0)goto _LL646;else{_tmpCB4=_tmpCB3->f1;_tmpCB5=*_tmpCB4;_tmpCB6=_tmpCB5.kind;if(
_tmpCB6 != Cyc_Absyn_IntKind)goto _LL646;}}_LL645: goto _LL63D;_LL646: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmpCB7=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCA8;if(_tmpCB7->tag
!= 2)goto _LL648;else{_tmpCB8=_tmpCB7->f1;_tmpCB9=(struct Cyc_Core_Opt**)& _tmpCB7->f1;
_tmpCBA=_tmpCB7->f2;_tmpCBB=*_tmpCBA;_tmpCBC=_tmpCBB.kind;if(_tmpCBC != Cyc_Absyn_EffKind)
goto _LL648;}}_LL647:*_tmpCB9=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto
_LL649;_LL648: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpCBD=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)
_tmpCA8;if(_tmpCBD->tag != 0)goto _LL64A;else{_tmpCBE=_tmpCBD->f1;_tmpCBF=*_tmpCBE;
_tmpCC0=_tmpCBF.kind;if(_tmpCC0 != Cyc_Absyn_EffKind)goto _LL64A;}}_LL649:{struct
Cyc_Absyn_VarType_Absyn_Type_struct*_tmp14E7;struct Cyc_Absyn_VarType_Absyn_Type_struct
_tmp14E6;struct Cyc_List_List*_tmp14E5;effect=((_tmp14E5=_cycalloc(sizeof(*
_tmp14E5)),((_tmp14E5->hd=(void*)((_tmp14E7=_cycalloc(sizeof(*_tmp14E7)),((
_tmp14E7[0]=((_tmp14E6.tag=2,((_tmp14E6.f1=_tmpCA6,_tmp14E6)))),_tmp14E7)))),((
_tmp14E5->tl=effect,_tmp14E5))))));}goto _LL63D;_LL64A: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*
_tmpCC1=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpCA8;if(_tmpCC1->tag
!= 1)goto _LL64C;else{_tmpCC2=_tmpCC1->f1;_tmpCC3=(struct Cyc_Core_Opt**)& _tmpCC1->f1;}}
_LL64B:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp14ED;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct
_tmp14EC;struct Cyc_Core_Opt*_tmp14EB;*_tmpCC3=((_tmp14EB=_cycalloc(sizeof(*
_tmp14EB)),((_tmp14EB->v=(void*)((_tmp14ED=_cycalloc(sizeof(*_tmp14ED)),((
_tmp14ED[0]=((_tmp14EC.tag=2,((_tmp14EC.f1=0,((_tmp14EC.f2=& Cyc_Tcutil_ak,
_tmp14EC)))))),_tmp14ED)))),_tmp14EB))));}goto _LL64D;_LL64C:;_LL64D:{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*
_tmp14FC;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp14FB;struct Cyc_Absyn_VarType_Absyn_Type_struct
_tmp14FA;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp14F9;struct Cyc_List_List*
_tmp14F8;effect=((_tmp14F8=_cycalloc(sizeof(*_tmp14F8)),((_tmp14F8->hd=(void*)((
_tmp14FC=_cycalloc(sizeof(*_tmp14FC)),((_tmp14FC[0]=((_tmp14F9.tag=25,((_tmp14F9.f1=(
void*)((void*)((_tmp14FB=_cycalloc(sizeof(*_tmp14FB)),((_tmp14FB[0]=((_tmp14FA.tag=
2,((_tmp14FA.f1=_tmpCA6,_tmp14FA)))),_tmp14FB))))),_tmp14F9)))),_tmp14FC)))),((
_tmp14F8->tl=effect,_tmp14F8))))));}goto _LL63D;_LL63D:;};}}{struct Cyc_List_List*
ts=_tmpC4F.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmpCD5;int _tmpCD6;struct
_tuple29 _tmpCD4=*((struct _tuple29*)ts->hd);_tmpCD5=_tmpCD4.f1;_tmpCD6=_tmpCD4.f2;
if(!_tmpCD6)continue;{struct Cyc_Absyn_Kind*_tmpCD7=Cyc_Tcutil_typ_kind(_tmpCD5);
struct Cyc_Absyn_Kind _tmpCD8;enum Cyc_Absyn_KindQual _tmpCD9;struct Cyc_Absyn_Kind
_tmpCDA;enum Cyc_Absyn_KindQual _tmpCDB;struct Cyc_Absyn_Kind _tmpCDC;enum Cyc_Absyn_KindQual
_tmpCDD;_LL64F: _tmpCD8=*_tmpCD7;_tmpCD9=_tmpCD8.kind;if(_tmpCD9 != Cyc_Absyn_RgnKind)
goto _LL651;_LL650:{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1502;struct
Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1501;struct Cyc_List_List*_tmp1500;
effect=((_tmp1500=_cycalloc(sizeof(*_tmp1500)),((_tmp1500->hd=(void*)((_tmp1502=
_cycalloc(sizeof(*_tmp1502)),((_tmp1502[0]=((_tmp1501.tag=23,((_tmp1501.f1=(void*)
_tmpCD5,_tmp1501)))),_tmp1502)))),((_tmp1500->tl=effect,_tmp1500))))));}goto
_LL64E;_LL651: _tmpCDA=*_tmpCD7;_tmpCDB=_tmpCDA.kind;if(_tmpCDB != Cyc_Absyn_EffKind)
goto _LL653;_LL652:{struct Cyc_List_List*_tmp1503;effect=((_tmp1503=_cycalloc(
sizeof(*_tmp1503)),((_tmp1503->hd=_tmpCD5,((_tmp1503->tl=effect,_tmp1503))))));}
goto _LL64E;_LL653: _tmpCDC=*_tmpCD7;_tmpCDD=_tmpCDC.kind;if(_tmpCDD != Cyc_Absyn_IntKind)
goto _LL655;_LL654: goto _LL64E;_LL655:;_LL656:{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*
_tmp1509;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1508;struct Cyc_List_List*
_tmp1507;effect=((_tmp1507=_cycalloc(sizeof(*_tmp1507)),((_tmp1507->hd=(void*)((
_tmp1509=_cycalloc(sizeof(*_tmp1509)),((_tmp1509[0]=((_tmp1508.tag=25,((_tmp1508.f1=(
void*)_tmpCD5,_tmp1508)))),_tmp1509)))),((_tmp1507->tl=effect,_tmp1507))))));}
goto _LL64E;_LL64E:;};}}{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp150C;struct
Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp150B;*_tmpB52=(void*)((_tmp150B=
_cycalloc(sizeof(*_tmp150B)),((_tmp150B[0]=((_tmp150C.tag=24,((_tmp150C.f1=
effect,_tmp150C)))),_tmp150B))));};}if(*_tmpB50 != 0){struct Cyc_List_List*bs=*
_tmpB50;for(0;bs != 0;bs=bs->tl){void*_tmpCE7=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)
bs->hd)->kind);struct Cyc_Core_Opt*_tmpCE9;struct Cyc_Core_Opt**_tmpCEA;struct Cyc_Core_Opt*
_tmpCEC;struct Cyc_Core_Opt**_tmpCED;struct Cyc_Absyn_Kind*_tmpCEE;struct Cyc_Absyn_Kind
_tmpCEF;enum Cyc_Absyn_KindQual _tmpCF0;enum Cyc_Absyn_AliasQual _tmpCF1;struct Cyc_Core_Opt*
_tmpCF3;struct Cyc_Core_Opt**_tmpCF4;struct Cyc_Absyn_Kind*_tmpCF5;struct Cyc_Absyn_Kind
_tmpCF6;enum Cyc_Absyn_KindQual _tmpCF7;enum Cyc_Absyn_AliasQual _tmpCF8;struct Cyc_Core_Opt*
_tmpCFA;struct Cyc_Core_Opt**_tmpCFB;struct Cyc_Absyn_Kind*_tmpCFC;struct Cyc_Absyn_Kind
_tmpCFD;enum Cyc_Absyn_KindQual _tmpCFE;enum Cyc_Absyn_AliasQual _tmpCFF;struct Cyc_Core_Opt*
_tmpD01;struct Cyc_Core_Opt**_tmpD02;struct Cyc_Absyn_Kind*_tmpD03;struct Cyc_Absyn_Kind
_tmpD04;enum Cyc_Absyn_KindQual _tmpD05;enum Cyc_Absyn_AliasQual _tmpD06;struct Cyc_Core_Opt*
_tmpD08;struct Cyc_Core_Opt**_tmpD09;struct Cyc_Absyn_Kind*_tmpD0A;struct Cyc_Absyn_Kind
_tmpD0B;enum Cyc_Absyn_KindQual _tmpD0C;enum Cyc_Absyn_AliasQual _tmpD0D;struct Cyc_Core_Opt*
_tmpD0F;struct Cyc_Core_Opt**_tmpD10;struct Cyc_Absyn_Kind*_tmpD11;struct Cyc_Absyn_Kind
_tmpD12;enum Cyc_Absyn_KindQual _tmpD13;enum Cyc_Absyn_AliasQual _tmpD14;struct Cyc_Core_Opt*
_tmpD16;struct Cyc_Core_Opt**_tmpD17;struct Cyc_Absyn_Kind*_tmpD18;struct Cyc_Absyn_Kind
_tmpD19;enum Cyc_Absyn_KindQual _tmpD1A;enum Cyc_Absyn_AliasQual _tmpD1B;struct Cyc_Core_Opt*
_tmpD1D;struct Cyc_Core_Opt**_tmpD1E;struct Cyc_Absyn_Kind*_tmpD1F;struct Cyc_Absyn_Kind
_tmpD20;enum Cyc_Absyn_KindQual _tmpD21;enum Cyc_Absyn_AliasQual _tmpD22;struct Cyc_Core_Opt*
_tmpD24;struct Cyc_Core_Opt**_tmpD25;struct Cyc_Absyn_Kind*_tmpD26;struct Cyc_Absyn_Kind*
_tmpD28;struct Cyc_Absyn_Kind _tmpD29;enum Cyc_Absyn_KindQual _tmpD2A;_LL658: {
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpCE8=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)
_tmpCE7;if(_tmpCE8->tag != 1)goto _LL65A;else{_tmpCE9=_tmpCE8->f1;_tmpCEA=(struct
Cyc_Core_Opt**)& _tmpCE8->f1;}}_LL659:{const char*_tmp1510;void*_tmp150F[1];struct
Cyc_String_pa_PrintArg_struct _tmp150E;(_tmp150E.tag=0,((_tmp150E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((
_tmp150F[0]=& _tmp150E,Cyc_Tcutil_warn(loc,((_tmp1510="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmp1510,sizeof(char),47))),_tag_dyneither(_tmp150F,sizeof(void*),
1)))))));}_tmpCED=_tmpCEA;goto _LL65B;_LL65A: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmpCEB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCE7;if(_tmpCEB->tag
!= 2)goto _LL65C;else{_tmpCEC=_tmpCEB->f1;_tmpCED=(struct Cyc_Core_Opt**)& _tmpCEB->f1;
_tmpCEE=_tmpCEB->f2;_tmpCEF=*_tmpCEE;_tmpCF0=_tmpCEF.kind;if(_tmpCF0 != Cyc_Absyn_BoxKind)
goto _LL65C;_tmpCF1=_tmpCEF.aliasqual;if(_tmpCF1 != Cyc_Absyn_Top)goto _LL65C;}}
_LL65B: _tmpCF4=_tmpCED;goto _LL65D;_LL65C: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmpCF2=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCE7;if(_tmpCF2->tag
!= 2)goto _LL65E;else{_tmpCF3=_tmpCF2->f1;_tmpCF4=(struct Cyc_Core_Opt**)& _tmpCF2->f1;
_tmpCF5=_tmpCF2->f2;_tmpCF6=*_tmpCF5;_tmpCF7=_tmpCF6.kind;if(_tmpCF7 != Cyc_Absyn_MemKind)
goto _LL65E;_tmpCF8=_tmpCF6.aliasqual;if(_tmpCF8 != Cyc_Absyn_Top)goto _LL65E;}}
_LL65D: _tmpCFB=_tmpCF4;goto _LL65F;_LL65E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmpCF9=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCE7;if(_tmpCF9->tag
!= 2)goto _LL660;else{_tmpCFA=_tmpCF9->f1;_tmpCFB=(struct Cyc_Core_Opt**)& _tmpCF9->f1;
_tmpCFC=_tmpCF9->f2;_tmpCFD=*_tmpCFC;_tmpCFE=_tmpCFD.kind;if(_tmpCFE != Cyc_Absyn_MemKind)
goto _LL660;_tmpCFF=_tmpCFD.aliasqual;if(_tmpCFF != Cyc_Absyn_Aliasable)goto _LL660;}}
_LL65F: _tmpD02=_tmpCFB;goto _LL661;_LL660: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmpD00=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCE7;if(_tmpD00->tag
!= 2)goto _LL662;else{_tmpD01=_tmpD00->f1;_tmpD02=(struct Cyc_Core_Opt**)& _tmpD00->f1;
_tmpD03=_tmpD00->f2;_tmpD04=*_tmpD03;_tmpD05=_tmpD04.kind;if(_tmpD05 != Cyc_Absyn_AnyKind)
goto _LL662;_tmpD06=_tmpD04.aliasqual;if(_tmpD06 != Cyc_Absyn_Top)goto _LL662;}}
_LL661: _tmpD09=_tmpD02;goto _LL663;_LL662: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmpD07=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCE7;if(_tmpD07->tag
!= 2)goto _LL664;else{_tmpD08=_tmpD07->f1;_tmpD09=(struct Cyc_Core_Opt**)& _tmpD07->f1;
_tmpD0A=_tmpD07->f2;_tmpD0B=*_tmpD0A;_tmpD0C=_tmpD0B.kind;if(_tmpD0C != Cyc_Absyn_AnyKind)
goto _LL664;_tmpD0D=_tmpD0B.aliasqual;if(_tmpD0D != Cyc_Absyn_Aliasable)goto _LL664;}}
_LL663:*_tmpD09=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL657;_LL664: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD0E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)
_tmpCE7;if(_tmpD0E->tag != 2)goto _LL666;else{_tmpD0F=_tmpD0E->f1;_tmpD10=(struct
Cyc_Core_Opt**)& _tmpD0E->f1;_tmpD11=_tmpD0E->f2;_tmpD12=*_tmpD11;_tmpD13=_tmpD12.kind;
if(_tmpD13 != Cyc_Absyn_MemKind)goto _LL666;_tmpD14=_tmpD12.aliasqual;if(_tmpD14 != 
Cyc_Absyn_Unique)goto _LL666;}}_LL665: _tmpD17=_tmpD10;goto _LL667;_LL666: {struct
Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD15=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)
_tmpCE7;if(_tmpD15->tag != 2)goto _LL668;else{_tmpD16=_tmpD15->f1;_tmpD17=(struct
Cyc_Core_Opt**)& _tmpD15->f1;_tmpD18=_tmpD15->f2;_tmpD19=*_tmpD18;_tmpD1A=_tmpD19.kind;
if(_tmpD1A != Cyc_Absyn_AnyKind)goto _LL668;_tmpD1B=_tmpD19.aliasqual;if(_tmpD1B != 
Cyc_Absyn_Unique)goto _LL668;}}_LL667:*_tmpD17=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);
goto _LL657;_LL668: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD1C=(
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCE7;if(_tmpD1C->tag != 2)goto
_LL66A;else{_tmpD1D=_tmpD1C->f1;_tmpD1E=(struct Cyc_Core_Opt**)& _tmpD1C->f1;
_tmpD1F=_tmpD1C->f2;_tmpD20=*_tmpD1F;_tmpD21=_tmpD20.kind;if(_tmpD21 != Cyc_Absyn_RgnKind)
goto _LL66A;_tmpD22=_tmpD20.aliasqual;if(_tmpD22 != Cyc_Absyn_Top)goto _LL66A;}}
_LL669:*_tmpD1E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL657;_LL66A: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD23=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)
_tmpCE7;if(_tmpD23->tag != 2)goto _LL66C;else{_tmpD24=_tmpD23->f1;_tmpD25=(struct
Cyc_Core_Opt**)& _tmpD23->f1;_tmpD26=_tmpD23->f2;}}_LL66B:*_tmpD25=Cyc_Tcutil_kind_to_bound_opt(
_tmpD26);goto _LL657;_LL66C: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*
_tmpD27=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpCE7;if(_tmpD27->tag != 
0)goto _LL66E;else{_tmpD28=_tmpD27->f1;_tmpD29=*_tmpD28;_tmpD2A=_tmpD29.kind;if(
_tmpD2A != Cyc_Absyn_MemKind)goto _LL66E;}}_LL66D:{const char*_tmp1513;void*
_tmp1512;(_tmp1512=0,Cyc_Tcutil_terr(loc,((_tmp1513="functions can't abstract M types",
_tag_dyneither(_tmp1513,sizeof(char),33))),_tag_dyneither(_tmp1512,sizeof(void*),
0)));}goto _LL657;_LL66E:;_LL66F: goto _LL657;_LL657:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmpC4F.kind_env,*_tmpB50);_tmpC4F.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmpC4F.r,_tmpC4F.free_vars,*_tmpB50);{struct Cyc_List_List*tvs=_tmpC4F.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpD31;int _tmpD32;struct
_tuple28 _tmpD30=*((struct _tuple28*)tvs->hd);_tmpD31=_tmpD30.f1;_tmpD32=_tmpD30.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpD31,_tmpD32);}}{struct Cyc_List_List*evs=_tmpC4F.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmpD34;int _tmpD35;struct _tuple29 _tmpD33=*((struct _tuple29*)evs->hd);
_tmpD34=_tmpD33.f1;_tmpD35=_tmpD33.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmpD34,_tmpD35);}};}_npop_handler(0);goto _LL577;;
_pop_region(newr);};}_LL596: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmpB5B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpB14;if(_tmpB5B->tag != 
10)goto _LL598;else{_tmpB5C=_tmpB5B->f1;}}_LL597: for(0;_tmpB5C != 0;_tmpB5C=
_tmpB5C->tl){struct _tuple12*_tmpD37=(struct _tuple12*)_tmpB5C->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpD37).f2,1);((*_tmpD37).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmpD37).f1).print_const,(*_tmpD37).f2);}goto _LL577;_LL598: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmpB5D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpB14;if(_tmpB5D->tag
!= 12)goto _LL59A;else{_tmpB5E=_tmpB5D->f1;_tmpB5F=_tmpB5D->f2;}}_LL599: {struct
_RegionHandle _tmpD38=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=&
_tmpD38;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;for(0;_tmpB5F
!= 0;_tmpB5F=_tmpB5F->tl){struct Cyc_Absyn_Aggrfield _tmpD3A;struct _dyneither_ptr*
_tmpD3B;struct Cyc_Absyn_Tqual _tmpD3C;struct Cyc_Absyn_Tqual*_tmpD3D;void*_tmpD3E;
struct Cyc_Absyn_Exp*_tmpD3F;struct Cyc_List_List*_tmpD40;struct Cyc_Absyn_Aggrfield*
_tmpD39=(struct Cyc_Absyn_Aggrfield*)_tmpB5F->hd;_tmpD3A=*_tmpD39;_tmpD3B=_tmpD3A.name;
_tmpD3C=_tmpD3A.tq;_tmpD3D=(struct Cyc_Absyn_Tqual*)&(*_tmpD39).tq;_tmpD3E=
_tmpD3A.type;_tmpD3F=_tmpD3A.width;_tmpD40=_tmpD3A.attributes;if(((int(*)(int(*
compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,
struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpD3B)){const
char*_tmp1517;void*_tmp1516[1];struct Cyc_String_pa_PrintArg_struct _tmp1515;(
_tmp1515.tag=0,((_tmp1515.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpD3B),((_tmp1516[0]=& _tmp1515,Cyc_Tcutil_terr(loc,((_tmp1517="duplicate field %s",
_tag_dyneither(_tmp1517,sizeof(char),19))),_tag_dyneither(_tmp1516,sizeof(void*),
1)))))));}{const char*_tmp1518;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpD3B,((
_tmp1518="",_tag_dyneither(_tmp1518,sizeof(char),1))))!= 0){struct Cyc_List_List*
_tmp1519;prev_fields=((_tmp1519=_region_malloc(aprev_rgn,sizeof(*_tmp1519)),((
_tmp1519->hd=_tmpD3B,((_tmp1519->tl=prev_fields,_tmp1519))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpD3E,1);_tmpD3D->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpD3D->print_const,_tmpD3E);if(_tmpB5E == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(
_tmpD3E)){const char*_tmp151D;void*_tmp151C[1];struct Cyc_String_pa_PrintArg_struct
_tmp151B;(_tmp151B.tag=0,((_tmp151B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmpD3B),((_tmp151C[0]=& _tmp151B,Cyc_Tcutil_warn(loc,((_tmp151D="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp151D,sizeof(char),74))),_tag_dyneither(_tmp151C,sizeof(void*),
1)))))));}Cyc_Tcutil_check_bitfield(loc,te,_tmpD3E,_tmpD3F,_tmpD3B);Cyc_Tcutil_check_field_atts(
loc,_tmpD3B,_tmpD40);}}_npop_handler(0);goto _LL577;;_pop_region(aprev_rgn);}
_LL59A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB60=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmpB14;if(_tmpB60->tag != 11)goto _LL59C;else{_tmpB61=_tmpB60->f1;_tmpB62=_tmpB61.aggr_info;
_tmpB63=(union Cyc_Absyn_AggrInfoU*)&(_tmpB60->f1).aggr_info;_tmpB64=_tmpB61.targs;
_tmpB65=(struct Cyc_List_List**)&(_tmpB60->f1).targs;}}_LL59B:{union Cyc_Absyn_AggrInfoU
_tmpD49=*_tmpB63;struct _tuple4 _tmpD4A;enum Cyc_Absyn_AggrKind _tmpD4B;struct
_tuple2*_tmpD4C;struct Cyc_Core_Opt*_tmpD4D;struct Cyc_Absyn_Aggrdecl**_tmpD4E;
struct Cyc_Absyn_Aggrdecl*_tmpD4F;_LL671: if((_tmpD49.UnknownAggr).tag != 1)goto
_LL673;_tmpD4A=(struct _tuple4)(_tmpD49.UnknownAggr).val;_tmpD4B=_tmpD4A.f1;
_tmpD4C=_tmpD4A.f2;_tmpD4D=_tmpD4A.f3;_LL672: {struct Cyc_Absyn_Aggrdecl**adp;{
struct _handler_cons _tmpD50;_push_handler(& _tmpD50);{int _tmpD52=0;if(setjmp(
_tmpD50.handler))_tmpD52=1;if(!_tmpD52){adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,
_tmpD4C);{struct Cyc_Absyn_Aggrdecl*_tmpD53=*adp;if(_tmpD53->kind != _tmpD4B){if(
_tmpD53->kind == Cyc_Absyn_StructA){const char*_tmp1522;void*_tmp1521[2];struct Cyc_String_pa_PrintArg_struct
_tmp1520;struct Cyc_String_pa_PrintArg_struct _tmp151F;(_tmp151F.tag=0,((_tmp151F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD4C)),((
_tmp1520.tag=0,((_tmp1520.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD4C)),((_tmp1521[0]=& _tmp1520,((_tmp1521[1]=& _tmp151F,Cyc_Tcutil_terr(loc,((
_tmp1522="expecting struct %s instead of union %s",_tag_dyneither(_tmp1522,
sizeof(char),40))),_tag_dyneither(_tmp1521,sizeof(void*),2)))))))))))));}else{
const char*_tmp1527;void*_tmp1526[2];struct Cyc_String_pa_PrintArg_struct _tmp1525;
struct Cyc_String_pa_PrintArg_struct _tmp1524;(_tmp1524.tag=0,((_tmp1524.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD4C)),((
_tmp1525.tag=0,((_tmp1525.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD4C)),((_tmp1526[0]=& _tmp1525,((_tmp1526[1]=& _tmp1524,Cyc_Tcutil_terr(loc,((
_tmp1527="expecting union %s instead of struct %s",_tag_dyneither(_tmp1527,
sizeof(char),40))),_tag_dyneither(_tmp1526,sizeof(void*),2)))))))))))));}}if((
unsigned int)_tmpD4D  && (int)_tmpD4D->v){if(!((unsigned int)_tmpD53->impl) || !((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD53->impl))->tagged){const char*
_tmp152B;void*_tmp152A[1];struct Cyc_String_pa_PrintArg_struct _tmp1529;(_tmp1529.tag=
0,((_tmp1529.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD4C)),((_tmp152A[0]=& _tmp1529,Cyc_Tcutil_terr(loc,((_tmp152B="@tagged qualfiers don't agree on union %s",
_tag_dyneither(_tmp152B,sizeof(char),42))),_tag_dyneither(_tmp152A,sizeof(void*),
1)))))));}}*_tmpB63=Cyc_Absyn_KnownAggr(adp);};;_pop_handler();}else{void*
_tmpD51=(void*)_exn_thrown;void*_tmpD60=_tmpD51;_LL676: {struct Cyc_Dict_Absent_exn_struct*
_tmpD61=(struct Cyc_Dict_Absent_exn_struct*)_tmpD60;if(_tmpD61->tag != Cyc_Dict_Absent)
goto _LL678;}_LL677: {struct Cyc_Tcenv_Genv*_tmpD62=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp152C;struct Cyc_Absyn_Aggrdecl*_tmpD63=(_tmp152C=
_cycalloc(sizeof(*_tmp152C)),((_tmp152C->kind=_tmpD4B,((_tmp152C->sc=Cyc_Absyn_Extern,((
_tmp152C->name=_tmpD4C,((_tmp152C->tvs=0,((_tmp152C->impl=0,((_tmp152C->attributes=
0,_tmp152C)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmpD62,loc,_tmpD63);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpD4C);*_tmpB63=Cyc_Absyn_KnownAggr(adp);if(*_tmpB65 != 0){{const char*
_tmp1530;void*_tmp152F[1];struct Cyc_String_pa_PrintArg_struct _tmp152E;(_tmp152E.tag=
0,((_tmp152E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD4C)),((_tmp152F[0]=& _tmp152E,Cyc_Tcutil_terr(loc,((_tmp1530="declare parameterized type %s before using",
_tag_dyneither(_tmp1530,sizeof(char),43))),_tag_dyneither(_tmp152F,sizeof(void*),
1)))))));}return cvtenv;}goto _LL675;}_LL678:;_LL679:(void)_throw(_tmpD60);_LL675:;}};}
_tmpD4F=*adp;goto _LL674;}_LL673: if((_tmpD49.KnownAggr).tag != 2)goto _LL670;
_tmpD4E=(struct Cyc_Absyn_Aggrdecl**)(_tmpD49.KnownAggr).val;_tmpD4F=*_tmpD4E;
_LL674: {struct Cyc_List_List*tvs=_tmpD4F->tvs;struct Cyc_List_List*ts=*_tmpB65;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Tvar*_tmpD68=(
struct Cyc_Absyn_Tvar*)tvs->hd;void*_tmpD69=(void*)ts->hd;{struct _tuple0 _tmp1531;
struct _tuple0 _tmpD6B=(_tmp1531.f1=Cyc_Absyn_compress_kb(_tmpD68->kind),((
_tmp1531.f2=_tmpD69,_tmp1531)));void*_tmpD6C;void*_tmpD6E;struct Cyc_Absyn_Tvar*
_tmpD70;_LL67B: _tmpD6C=_tmpD6B.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*
_tmpD6D=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpD6C;if(_tmpD6D->tag
!= 1)goto _LL67D;};_tmpD6E=_tmpD6B.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmpD6F=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpD6E;if(_tmpD6F->tag != 2)
goto _LL67D;else{_tmpD70=_tmpD6F->f1;}};_LL67C: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(
loc,cvtenv.kind_env,_tmpD70);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpD70,1);continue;_LL67D:;_LL67E: goto _LL67A;_LL67A:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);Cyc_Tcutil_check_no_qual(
loc,(void*)ts->hd);};}if(ts != 0){const char*_tmp1535;void*_tmp1534[1];struct Cyc_String_pa_PrintArg_struct
_tmp1533;(_tmp1533.tag=0,((_tmp1533.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD4F->name)),((_tmp1534[0]=& _tmp1533,
Cyc_Tcutil_terr(loc,((_tmp1535="too many parameters for type %s",_tag_dyneither(
_tmp1535,sizeof(char),32))),_tag_dyneither(_tmp1534,sizeof(void*),1)))))));}if(
tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmp1536;
hidden_ts=((_tmp1536=_cycalloc(sizeof(*_tmp1536)),((_tmp1536->hd=e,((_tmp1536->tl=
hidden_ts,_tmp1536))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,
1);}*_tmpB65=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(*_tmpB65,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(hidden_ts));}}_LL670:;}goto _LL577;_LL59C: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*
_tmpB66=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpB14;if(_tmpB66->tag
!= 17)goto _LL59E;else{_tmpB67=_tmpB66->f1;_tmpB68=_tmpB66->f2;_tmpB69=(struct Cyc_List_List**)&
_tmpB66->f2;_tmpB6A=_tmpB66->f3;_tmpB6B=(struct Cyc_Absyn_Typedefdecl**)& _tmpB66->f3;
_tmpB6C=(void**)& _tmpB66->f4;_tmpB6D=(void**)((void**)& _tmpB66->f4);}}_LL59D: {
struct Cyc_List_List*targs=*_tmpB69;struct Cyc_Absyn_Typedefdecl*td;{struct
_handler_cons _tmpD75;_push_handler(& _tmpD75);{int _tmpD77=0;if(setjmp(_tmpD75.handler))
_tmpD77=1;if(!_tmpD77){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmpB67);;
_pop_handler();}else{void*_tmpD76=(void*)_exn_thrown;void*_tmpD79=_tmpD76;_LL680: {
struct Cyc_Dict_Absent_exn_struct*_tmpD7A=(struct Cyc_Dict_Absent_exn_struct*)
_tmpD79;if(_tmpD7A->tag != Cyc_Dict_Absent)goto _LL682;}_LL681:{const char*_tmp153A;
void*_tmp1539[1];struct Cyc_String_pa_PrintArg_struct _tmp1538;(_tmp1538.tag=0,((
_tmp1538.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB67)),((_tmp1539[0]=& _tmp1538,Cyc_Tcutil_terr(loc,((_tmp153A="unbound typedef name %s",
_tag_dyneither(_tmp153A,sizeof(char),24))),_tag_dyneither(_tmp1539,sizeof(void*),
1)))))));}return cvtenv;_LL682:;_LL683:(void)_throw(_tmpD79);_LL67F:;}};}*_tmpB6B=(
struct Cyc_Absyn_Typedefdecl*)td;_tmpB67[0]=(td->name)[0];{struct Cyc_List_List*
tvs=td->tvs;struct Cyc_List_List*ts=targs;struct _RegionHandle _tmpD7E=_new_region("temp");
struct _RegionHandle*temp=& _tmpD7E;_push_region(temp);{struct Cyc_List_List*inst=0;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((
struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,(struct Cyc_Absyn_Typedefdecl*)
td);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);Cyc_Tcutil_check_no_qual(
loc,(void*)ts->hd);{struct _tuple16*_tmp153D;struct Cyc_List_List*_tmp153C;inst=((
_tmp153C=_region_malloc(temp,sizeof(*_tmp153C)),((_tmp153C->hd=((_tmp153D=
_region_malloc(temp,sizeof(*_tmp153D)),((_tmp153D->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((
_tmp153D->f2=(void*)ts->hd,_tmp153D)))))),((_tmp153C->tl=inst,_tmp153C))))));};}
if(ts != 0){const char*_tmp1541;void*_tmp1540[1];struct Cyc_String_pa_PrintArg_struct
_tmp153F;(_tmp153F.tag=0,((_tmp153F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB67)),((_tmp1540[0]=& _tmp153F,Cyc_Tcutil_terr(
loc,((_tmp1541="too many parameters for typedef %s",_tag_dyneither(_tmp1541,
sizeof(char),35))),_tag_dyneither(_tmp1540,sizeof(void*),1)))))));}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);void*e=Cyc_Absyn_new_evar(0,0);{
struct Cyc_List_List*_tmp1542;hidden_ts=((_tmp1542=_cycalloc(sizeof(*_tmp1542)),((
_tmp1542->hd=e,((_tmp1542->tl=hidden_ts,_tmp1542))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);{struct _tuple16*_tmp1545;struct Cyc_List_List*_tmp1544;inst=(
struct Cyc_List_List*)((_tmp1544=_cycalloc(sizeof(*_tmp1544)),((_tmp1544->hd=(
struct _tuple16*)((_tmp1545=_cycalloc(sizeof(*_tmp1545)),((_tmp1545->f1=(struct
Cyc_Absyn_Tvar*)tvs->hd,((_tmp1545->f2=e,_tmp1545)))))),((_tmp1544->tl=inst,
_tmp1544))))));};}*_tmpB69=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}if(td->defn != 0){void*
new_typ=inst == 0?(void*)_check_null(td->defn): Cyc_Tcutil_rsubstitute(temp,inst,(
void*)_check_null(td->defn));*_tmpB6D=(void*)new_typ;}}_npop_handler(0);goto
_LL577;;_pop_region(temp);};}_LL59E: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*
_tmpB6E=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpB14;if(_tmpB6E->tag != 
22)goto _LL5A0;}_LL59F: goto _LL5A1;_LL5A0: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*
_tmpB6F=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpB14;if(_tmpB6F->tag != 
21)goto _LL5A2;}_LL5A1: goto _LL5A3;_LL5A2: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*
_tmpB70=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmpB14;if(_tmpB70->tag != 20)
goto _LL5A4;}_LL5A3: goto _LL577;_LL5A4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*
_tmpB71=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpB14;if(_tmpB71->tag
!= 15)goto _LL5A6;else{_tmpB72=(void*)_tmpB71->f1;}}_LL5A5: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB72,1);goto _LL577;_LL5A6: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*
_tmpB73=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmpB14;if(_tmpB73->tag != 
16)goto _LL5A8;else{_tmpB74=(void*)_tmpB73->f1;_tmpB75=(void*)_tmpB73->f2;}}
_LL5A7: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpB74,
0);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpB75,1);
goto _LL577;_LL5A8: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpB76=(struct
Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmpB14;if(_tmpB76->tag != 23)goto _LL5AA;
else{_tmpB77=(void*)_tmpB76->f1;}}_LL5A9: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB77,1);goto _LL577;_LL5AA: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*
_tmpB78=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmpB14;if(_tmpB78->tag != 25)
goto _LL5AC;else{_tmpB79=(void*)_tmpB78->f1;}}_LL5AB: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB79,1);goto _LL577;_LL5AC: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmpB7A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmpB14;if(_tmpB7A->tag != 24)
goto _LL577;else{_tmpB7B=_tmpB7A->f1;}}_LL5AD: for(0;_tmpB7B != 0;_tmpB7B=_tmpB7B->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmpB7B->hd,
1);}goto _LL577;_LL577:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),
expected_kind)){{void*_tmpD87=t;struct Cyc_Core_Opt*_tmpD89;void*_tmpD8A;_LL685: {
struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpD88=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)
_tmpD87;if(_tmpD88->tag != 1)goto _LL687;else{_tmpD89=_tmpD88->f1;_tmpD8A=(void*)
_tmpD88->f2;}}_LL686: {struct _dyneither_ptr s;{struct Cyc_Core_Opt*_tmpD8B=_tmpD89;
struct Cyc_Core_Opt _tmpD8C;struct Cyc_Absyn_Kind*_tmpD8D;_LL68A: if(_tmpD8B != 0)
goto _LL68C;_LL68B:{const char*_tmp1546;s=((_tmp1546="kind=NULL ",_tag_dyneither(
_tmp1546,sizeof(char),11)));}goto _LL689;_LL68C: if(_tmpD8B == 0)goto _LL689;_tmpD8C=*
_tmpD8B;_tmpD8D=(struct Cyc_Absyn_Kind*)_tmpD8C.v;_LL68D:{const char*_tmp154A;void*
_tmp1549[1];struct Cyc_String_pa_PrintArg_struct _tmp1548;s=(struct _dyneither_ptr)((
_tmp1548.tag=0,((_tmp1548.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmpD8D)),((_tmp1549[0]=& _tmp1548,Cyc_aprintf(((_tmp154A="kind=%s ",
_tag_dyneither(_tmp154A,sizeof(char),9))),_tag_dyneither(_tmp1549,sizeof(void*),
1))))))));}goto _LL689;_LL689:;}if(_tmpD8A == 0){const char*_tmp154E;void*_tmp154D[
1];struct Cyc_String_pa_PrintArg_struct _tmp154C;s=(struct _dyneither_ptr)((
_tmp154C.tag=0,((_tmp154C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp154D[0]=& _tmp154C,Cyc_aprintf(((_tmp154E="%s ref=NULL",_tag_dyneither(
_tmp154E,sizeof(char),12))),_tag_dyneither(_tmp154D,sizeof(void*),1))))))));}
else{const char*_tmp1553;void*_tmp1552[2];struct Cyc_String_pa_PrintArg_struct
_tmp1551;struct Cyc_String_pa_PrintArg_struct _tmp1550;s=(struct _dyneither_ptr)((
_tmp1550.tag=0,((_tmp1550.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)_tmpD8A)),((_tmp1551.tag=0,((_tmp1551.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmp1552[0]=& _tmp1551,((_tmp1552[1]=& _tmp1550,Cyc_aprintf(((
_tmp1553="%s ref=%s",_tag_dyneither(_tmp1553,sizeof(char),10))),_tag_dyneither(
_tmp1552,sizeof(void*),2))))))))))))));}goto _LL684;}_LL687:;_LL688: goto _LL684;
_LL684:;}{const char*_tmp1559;void*_tmp1558[3];struct Cyc_String_pa_PrintArg_struct
_tmp1557;struct Cyc_String_pa_PrintArg_struct _tmp1556;struct Cyc_String_pa_PrintArg_struct
_tmp1555;(_tmp1555.tag=0,((_tmp1555.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(expected_kind)),((_tmp1556.tag=0,((
_tmp1556.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
Cyc_Tcutil_typ_kind(t))),((_tmp1557.tag=0,((_tmp1557.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1558[0]=& _tmp1557,((
_tmp1558[1]=& _tmp1556,((_tmp1558[2]=& _tmp1555,Cyc_Tcutil_terr(loc,((_tmp1559="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmp1559,sizeof(char),51))),_tag_dyneither(_tmp1558,sizeof(void*),
3)))))))))))))))))));};}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{
void*_tmpD9E=e->r;struct Cyc_List_List*_tmpDA4;struct Cyc_Absyn_Exp*_tmpDA6;struct
Cyc_Absyn_Exp*_tmpDA7;struct Cyc_Absyn_Exp*_tmpDA8;struct Cyc_Absyn_Exp*_tmpDAA;
struct Cyc_Absyn_Exp*_tmpDAB;struct Cyc_Absyn_Exp*_tmpDAD;struct Cyc_Absyn_Exp*
_tmpDAE;struct Cyc_Absyn_Exp*_tmpDB0;struct Cyc_Absyn_Exp*_tmpDB1;void*_tmpDB3;
struct Cyc_Absyn_Exp*_tmpDB4;void*_tmpDB6;void*_tmpDB8;void*_tmpDBA;struct Cyc_Absyn_Exp*
_tmpDBC;_LL68F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpD9F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmpD9E;if(_tmpD9F->tag != 0)goto _LL691;}_LL690: goto _LL692;_LL691: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*
_tmpDA0=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpD9E;if(_tmpDA0->tag != 
30)goto _LL693;}_LL692: goto _LL694;_LL693: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*
_tmpDA1=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpD9E;if(_tmpDA1->tag
!= 31)goto _LL695;}_LL694: goto _LL696;_LL695: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmpDA2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD9E;if(_tmpDA2->tag != 1)
goto _LL697;}_LL696: goto _LL68E;_LL697: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*
_tmpDA3=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD9E;if(_tmpDA3->tag
!= 2)goto _LL699;else{_tmpDA4=_tmpDA3->f2;}}_LL698: for(0;_tmpDA4 != 0;_tmpDA4=
_tmpDA4->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)
_tmpDA4->hd,te,cvtenv);}goto _LL68E;_LL699: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*
_tmpDA5=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD9E;if(_tmpDA5->tag
!= 5)goto _LL69B;else{_tmpDA6=_tmpDA5->f1;_tmpDA7=_tmpDA5->f2;_tmpDA8=_tmpDA5->f3;}}
_LL69A: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDA6,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDA7,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpDA8,te,cvtenv);goto _LL68E;_LL69B: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*
_tmpDA9=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD9E;if(_tmpDA9->tag != 6)
goto _LL69D;else{_tmpDAA=_tmpDA9->f1;_tmpDAB=_tmpDA9->f2;}}_LL69C: _tmpDAD=_tmpDAA;
_tmpDAE=_tmpDAB;goto _LL69E;_LL69D: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*
_tmpDAC=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD9E;if(_tmpDAC->tag != 7)
goto _LL69F;else{_tmpDAD=_tmpDAC->f1;_tmpDAE=_tmpDAC->f2;}}_LL69E: _tmpDB0=_tmpDAD;
_tmpDB1=_tmpDAE;goto _LL6A0;_LL69F: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*
_tmpDAF=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD9E;if(_tmpDAF->tag
!= 8)goto _LL6A1;else{_tmpDB0=_tmpDAF->f1;_tmpDB1=_tmpDAF->f2;}}_LL6A0: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpDB0,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDB1,te,
cvtenv);goto _LL68E;_LL6A1: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpDB2=(
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD9E;if(_tmpDB2->tag != 13)goto
_LL6A3;else{_tmpDB3=(void*)_tmpDB2->f1;_tmpDB4=_tmpDB2->f2;}}_LL6A2: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpDB4,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,
_tmpDB3,1);goto _LL68E;_LL6A3: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*
_tmpDB5=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD9E;if(_tmpDB5->tag
!= 18)goto _LL6A5;else{_tmpDB6=(void*)_tmpDB5->f1;}}_LL6A4: _tmpDB8=_tmpDB6;goto
_LL6A6;_LL6A5: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpDB7=(struct
Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpD9E;if(_tmpDB7->tag != 16)goto
_LL6A7;else{_tmpDB8=(void*)_tmpDB7->f1;}}_LL6A6: cvtenv=Cyc_Tcutil_i_check_valid_type(
e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpDB8,1);goto _LL68E;_LL6A7: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*
_tmpDB9=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpD9E;if(_tmpDB9->tag
!= 37)goto _LL6A9;else{_tmpDBA=(void*)_tmpDB9->f1;}}_LL6A8: cvtenv=Cyc_Tcutil_i_check_valid_type(
e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpDBA,1);goto _LL68E;_LL6A9: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*
_tmpDBB=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD9E;if(_tmpDBB->tag
!= 17)goto _LL6AB;else{_tmpDBC=_tmpDBB->f1;}}_LL6AA: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpDBC,te,cvtenv);goto _LL68E;_LL6AB:;_LL6AC: {const char*_tmp155C;void*_tmp155B;(
_tmp155B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp155C="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmp155C,sizeof(char),66))),_tag_dyneither(_tmp155B,sizeof(void*),
0)));}_LL68E:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*
expected_kind,void*t){struct Cyc_List_List*_tmpDBF=cvt.kind_env;cvt=Cyc_Tcutil_i_check_valid_type(
loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;
vs=vs->tl){struct Cyc_Absyn_Tvar*_tmpDC1;struct _tuple28 _tmpDC0=*((struct _tuple28*)
vs->hd);_tmpDC1=_tmpDC0.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmpDBF,
_tmpDC1);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpDC3;struct _tuple29 _tmpDC2=*((struct _tuple29*)evs->hd);_tmpDC3=_tmpDC2.f1;{
void*_tmpDC4=Cyc_Tcutil_compress(_tmpDC3);struct Cyc_Core_Opt*_tmpDC6;struct Cyc_Core_Opt**
_tmpDC7;_LL6AE: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpDC5=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)
_tmpDC4;if(_tmpDC5->tag != 1)goto _LL6B0;else{_tmpDC6=_tmpDC5->f4;_tmpDC7=(struct
Cyc_Core_Opt**)& _tmpDC5->f4;}}_LL6AF: if(*_tmpDC7 == 0){struct Cyc_Core_Opt*
_tmp155D;*_tmpDC7=((_tmp155D=_cycalloc(sizeof(*_tmp155D)),((_tmp155D->v=_tmpDBF,
_tmp155D))));}else{struct Cyc_List_List*_tmpDC9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpDC7))->v;struct Cyc_List_List*_tmpDCA=0;for(0;_tmpDC9 != 0;
_tmpDC9=_tmpDC9->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmpDBF,(struct Cyc_Absyn_Tvar*)_tmpDC9->hd)){struct Cyc_List_List*_tmp155E;
_tmpDCA=((_tmp155E=_cycalloc(sizeof(*_tmp155E)),((_tmp155E->hd=(struct Cyc_Absyn_Tvar*)
_tmpDC9->hd,((_tmp155E->tl=_tmpDCA,_tmp155E))))));}}{struct Cyc_Core_Opt*_tmp155F;*
_tmpDC7=((_tmp155F=_cycalloc(sizeof(*_tmp155F)),((_tmp155F->v=_tmpDCA,_tmp155F))));};}
goto _LL6AD;_LL6B0:;_LL6B1: goto _LL6AD;_LL6AD:;};}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(
t);struct Cyc_List_List*_tmpDCD=Cyc_Tcenv_lookup_type_vars(te);struct
_RegionHandle _tmpDCE=_new_region("temp");struct _RegionHandle*temp=& _tmpDCE;
_push_region(temp);{struct Cyc_Tcutil_CVTEnv _tmp1560;struct Cyc_Tcutil_CVTEnv
_tmpDCF=Cyc_Tcutil_check_valid_type(loc,te,((_tmp1560.r=temp,((_tmp1560.kind_env=
_tmpDCD,((_tmp1560.free_vars=0,((_tmp1560.free_evars=0,((_tmp1560.generalize_evars=
generalize_evars,((_tmp1560.fn_result=0,_tmp1560)))))))))))),& Cyc_Tcutil_tmk,t);
struct Cyc_List_List*_tmpDD0=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(
temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpDCF.free_vars);
struct Cyc_List_List*_tmpDD1=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct
_tuple29*))Cyc_Core_fst,_tmpDCF.free_evars);if(_tmpDCD != 0){struct Cyc_List_List*
_tmpDD2=0;{struct Cyc_List_List*_tmpDD3=_tmpDD0;for(0;(unsigned int)_tmpDD3;
_tmpDD3=_tmpDD3->tl){struct Cyc_Absyn_Tvar*_tmpDD4=(struct Cyc_Absyn_Tvar*)_tmpDD3->hd;
int found=0;{struct Cyc_List_List*_tmpDD5=_tmpDCD;for(0;(unsigned int)_tmpDD5;
_tmpDD5=_tmpDD5->tl){if(Cyc_Absyn_tvar_cmp(_tmpDD4,(struct Cyc_Absyn_Tvar*)
_tmpDD5->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*_tmp1561;
_tmpDD2=((_tmp1561=_region_malloc(temp,sizeof(*_tmp1561)),((_tmp1561->hd=(struct
Cyc_Absyn_Tvar*)_tmpDD3->hd,((_tmp1561->tl=_tmpDD2,_tmp1561))))));}}}_tmpDD0=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDD2);}{
struct Cyc_List_List*x=_tmpDD0;for(0;x != 0;x=x->tl){void*_tmpDD7=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmpDD9;struct Cyc_Core_Opt**
_tmpDDA;struct Cyc_Core_Opt*_tmpDDC;struct Cyc_Core_Opt**_tmpDDD;struct Cyc_Absyn_Kind*
_tmpDDE;struct Cyc_Absyn_Kind _tmpDDF;enum Cyc_Absyn_KindQual _tmpDE0;enum Cyc_Absyn_AliasQual
_tmpDE1;struct Cyc_Core_Opt*_tmpDE3;struct Cyc_Core_Opt**_tmpDE4;struct Cyc_Absyn_Kind*
_tmpDE5;struct Cyc_Absyn_Kind _tmpDE6;enum Cyc_Absyn_KindQual _tmpDE7;enum Cyc_Absyn_AliasQual
_tmpDE8;struct Cyc_Core_Opt*_tmpDEA;struct Cyc_Core_Opt**_tmpDEB;struct Cyc_Absyn_Kind*
_tmpDEC;struct Cyc_Absyn_Kind _tmpDED;enum Cyc_Absyn_KindQual _tmpDEE;enum Cyc_Absyn_AliasQual
_tmpDEF;struct Cyc_Core_Opt*_tmpDF1;struct Cyc_Core_Opt**_tmpDF2;struct Cyc_Absyn_Kind*
_tmpDF3;struct Cyc_Absyn_Kind _tmpDF4;enum Cyc_Absyn_KindQual _tmpDF5;enum Cyc_Absyn_AliasQual
_tmpDF6;struct Cyc_Core_Opt*_tmpDF8;struct Cyc_Core_Opt**_tmpDF9;struct Cyc_Absyn_Kind*
_tmpDFA;struct Cyc_Absyn_Kind _tmpDFB;enum Cyc_Absyn_KindQual _tmpDFC;enum Cyc_Absyn_AliasQual
_tmpDFD;struct Cyc_Core_Opt*_tmpDFF;struct Cyc_Core_Opt**_tmpE00;struct Cyc_Absyn_Kind*
_tmpE01;struct Cyc_Absyn_Kind*_tmpE03;struct Cyc_Absyn_Kind _tmpE04;enum Cyc_Absyn_KindQual
_tmpE05;enum Cyc_Absyn_AliasQual _tmpE06;_LL6B3: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*
_tmpDD8=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpDD7;if(_tmpDD8->tag
!= 1)goto _LL6B5;else{_tmpDD9=_tmpDD8->f1;_tmpDDA=(struct Cyc_Core_Opt**)& _tmpDD8->f1;}}
_LL6B4: _tmpDDD=_tmpDDA;goto _LL6B6;_LL6B5: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmpDDB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDD7;if(_tmpDDB->tag
!= 2)goto _LL6B7;else{_tmpDDC=_tmpDDB->f1;_tmpDDD=(struct Cyc_Core_Opt**)& _tmpDDB->f1;
_tmpDDE=_tmpDDB->f2;_tmpDDF=*_tmpDDE;_tmpDE0=_tmpDDF.kind;if(_tmpDE0 != Cyc_Absyn_BoxKind)
goto _LL6B7;_tmpDE1=_tmpDDF.aliasqual;if(_tmpDE1 != Cyc_Absyn_Top)goto _LL6B7;}}
_LL6B6: _tmpDE4=_tmpDDD;goto _LL6B8;_LL6B7: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmpDE2=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDD7;if(_tmpDE2->tag
!= 2)goto _LL6B9;else{_tmpDE3=_tmpDE2->f1;_tmpDE4=(struct Cyc_Core_Opt**)& _tmpDE2->f1;
_tmpDE5=_tmpDE2->f2;_tmpDE6=*_tmpDE5;_tmpDE7=_tmpDE6.kind;if(_tmpDE7 != Cyc_Absyn_MemKind)
goto _LL6B9;_tmpDE8=_tmpDE6.aliasqual;if(_tmpDE8 != Cyc_Absyn_Top)goto _LL6B9;}}
_LL6B8: _tmpDEB=_tmpDE4;goto _LL6BA;_LL6B9: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmpDE9=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDD7;if(_tmpDE9->tag
!= 2)goto _LL6BB;else{_tmpDEA=_tmpDE9->f1;_tmpDEB=(struct Cyc_Core_Opt**)& _tmpDE9->f1;
_tmpDEC=_tmpDE9->f2;_tmpDED=*_tmpDEC;_tmpDEE=_tmpDED.kind;if(_tmpDEE != Cyc_Absyn_MemKind)
goto _LL6BB;_tmpDEF=_tmpDED.aliasqual;if(_tmpDEF != Cyc_Absyn_Aliasable)goto _LL6BB;}}
_LL6BA:*_tmpDEB=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6B2;_LL6BB: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDF0=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)
_tmpDD7;if(_tmpDF0->tag != 2)goto _LL6BD;else{_tmpDF1=_tmpDF0->f1;_tmpDF2=(struct
Cyc_Core_Opt**)& _tmpDF0->f1;_tmpDF3=_tmpDF0->f2;_tmpDF4=*_tmpDF3;_tmpDF5=_tmpDF4.kind;
if(_tmpDF5 != Cyc_Absyn_MemKind)goto _LL6BD;_tmpDF6=_tmpDF4.aliasqual;if(_tmpDF6 != 
Cyc_Absyn_Unique)goto _LL6BD;}}_LL6BC:*_tmpDF2=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);
goto _LL6B2;_LL6BD: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDF7=(
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDD7;if(_tmpDF7->tag != 2)goto
_LL6BF;else{_tmpDF8=_tmpDF7->f1;_tmpDF9=(struct Cyc_Core_Opt**)& _tmpDF7->f1;
_tmpDFA=_tmpDF7->f2;_tmpDFB=*_tmpDFA;_tmpDFC=_tmpDFB.kind;if(_tmpDFC != Cyc_Absyn_RgnKind)
goto _LL6BF;_tmpDFD=_tmpDFB.aliasqual;if(_tmpDFD != Cyc_Absyn_Top)goto _LL6BF;}}
_LL6BE:*_tmpDF9=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL6B2;_LL6BF: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDFE=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)
_tmpDD7;if(_tmpDFE->tag != 2)goto _LL6C1;else{_tmpDFF=_tmpDFE->f1;_tmpE00=(struct
Cyc_Core_Opt**)& _tmpDFE->f1;_tmpE01=_tmpDFE->f2;}}_LL6C0:*_tmpE00=Cyc_Tcutil_kind_to_bound_opt(
_tmpE01);goto _LL6B2;_LL6C1: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*
_tmpE02=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpDD7;if(_tmpE02->tag != 
0)goto _LL6C3;else{_tmpE03=_tmpE02->f1;_tmpE04=*_tmpE03;_tmpE05=_tmpE04.kind;if(
_tmpE05 != Cyc_Absyn_MemKind)goto _LL6C3;_tmpE06=_tmpE04.aliasqual;}}_LL6C2:{const
char*_tmp1568;void*_tmp1567[2];struct Cyc_String_pa_PrintArg_struct _tmp1566;
struct Cyc_Absyn_Kind*_tmp1565;struct Cyc_String_pa_PrintArg_struct _tmp1564;(
_tmp1564.tag=0,((_tmp1564.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((
_tmp1565=_cycalloc_atomic(sizeof(*_tmp1565)),((_tmp1565->kind=Cyc_Absyn_MemKind,((
_tmp1565->aliasqual=_tmpE06,_tmp1565)))))))),((_tmp1566.tag=0,((_tmp1566.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd)),((_tmp1567[0]=& _tmp1566,((_tmp1567[1]=& _tmp1564,Cyc_Tcutil_terr(loc,((
_tmp1568="type variable %s cannot have kind %s",_tag_dyneither(_tmp1568,sizeof(
char),37))),_tag_dyneither(_tmp1567,sizeof(void*),2)))))))))))));}goto _LL6B2;
_LL6C3:;_LL6C4: goto _LL6B2;_LL6B2:;}}if(_tmpDD0 != 0  || _tmpDD1 != 0){{void*_tmpE0C=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpE0E;struct Cyc_List_List*_tmpE0F;
struct Cyc_List_List**_tmpE10;void*_tmpE11;struct Cyc_Absyn_Tqual _tmpE12;void*
_tmpE13;struct Cyc_List_List*_tmpE14;int _tmpE15;struct Cyc_Absyn_VarargInfo*
_tmpE16;struct Cyc_List_List*_tmpE17;struct Cyc_List_List*_tmpE18;_LL6C6: {struct
Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE0D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)
_tmpE0C;if(_tmpE0D->tag != 9)goto _LL6C8;else{_tmpE0E=_tmpE0D->f1;_tmpE0F=_tmpE0E.tvars;
_tmpE10=(struct Cyc_List_List**)&(_tmpE0D->f1).tvars;_tmpE11=_tmpE0E.effect;
_tmpE12=_tmpE0E.ret_tqual;_tmpE13=_tmpE0E.ret_typ;_tmpE14=_tmpE0E.args;_tmpE15=
_tmpE0E.c_varargs;_tmpE16=_tmpE0E.cyc_varargs;_tmpE17=_tmpE0E.rgn_po;_tmpE18=
_tmpE0E.attributes;}}_LL6C7: if(*_tmpE10 == 0){*_tmpE10=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_copy)(_tmpDD0);_tmpDD0=0;}goto _LL6C5;_LL6C8:;
_LL6C9: goto _LL6C5;_LL6C5:;}if(_tmpDD0 != 0){const char*_tmp156C;void*_tmp156B[1];
struct Cyc_String_pa_PrintArg_struct _tmp156A;(_tmp156A.tag=0,((_tmp156A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpDD0->hd)->name),((
_tmp156B[0]=& _tmp156A,Cyc_Tcutil_terr(loc,((_tmp156C="unbound type variable %s",
_tag_dyneither(_tmp156C,sizeof(char),25))),_tag_dyneither(_tmp156B,sizeof(void*),
1)))))));}if(_tmpDD1 != 0)for(0;_tmpDD1 != 0;_tmpDD1=_tmpDD1->tl){void*e=(void*)
_tmpDD1->hd;struct Cyc_Absyn_Kind*_tmpE1C=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind
_tmpE1D;enum Cyc_Absyn_KindQual _tmpE1E;enum Cyc_Absyn_AliasQual _tmpE1F;struct Cyc_Absyn_Kind
_tmpE20;enum Cyc_Absyn_KindQual _tmpE21;enum Cyc_Absyn_AliasQual _tmpE22;struct Cyc_Absyn_Kind
_tmpE23;enum Cyc_Absyn_KindQual _tmpE24;enum Cyc_Absyn_AliasQual _tmpE25;struct Cyc_Absyn_Kind
_tmpE26;enum Cyc_Absyn_KindQual _tmpE27;_LL6CB: _tmpE1D=*_tmpE1C;_tmpE1E=_tmpE1D.kind;
if(_tmpE1E != Cyc_Absyn_RgnKind)goto _LL6CD;_tmpE1F=_tmpE1D.aliasqual;if(_tmpE1F != 
Cyc_Absyn_Unique)goto _LL6CD;_LL6CC: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp156F;void*_tmp156E;(_tmp156E=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp156F="can't unify evar with unique region!",
_tag_dyneither(_tmp156F,sizeof(char),37))),_tag_dyneither(_tmp156E,sizeof(void*),
0)));}goto _LL6CA;_LL6CD: _tmpE20=*_tmpE1C;_tmpE21=_tmpE20.kind;if(_tmpE21 != Cyc_Absyn_RgnKind)
goto _LL6CF;_tmpE22=_tmpE20.aliasqual;if(_tmpE22 != Cyc_Absyn_Aliasable)goto _LL6CF;
_LL6CE: goto _LL6D0;_LL6CF: _tmpE23=*_tmpE1C;_tmpE24=_tmpE23.kind;if(_tmpE24 != Cyc_Absyn_RgnKind)
goto _LL6D1;_tmpE25=_tmpE23.aliasqual;if(_tmpE25 != Cyc_Absyn_Top)goto _LL6D1;
_LL6D0: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1572;
void*_tmp1571;(_tmp1571=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1572="can't unify evar with heap!",_tag_dyneither(
_tmp1572,sizeof(char),28))),_tag_dyneither(_tmp1571,sizeof(void*),0)));}goto
_LL6CA;_LL6D1: _tmpE26=*_tmpE1C;_tmpE27=_tmpE26.kind;if(_tmpE27 != Cyc_Absyn_EffKind)
goto _LL6D3;_LL6D2: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp1575;void*_tmp1574;(_tmp1574=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1575="can't unify evar with {}!",
_tag_dyneither(_tmp1575,sizeof(char),26))),_tag_dyneither(_tmp1574,sizeof(void*),
0)));}goto _LL6CA;_LL6D3:;_LL6D4:{const char*_tmp157A;void*_tmp1579[2];struct Cyc_String_pa_PrintArg_struct
_tmp1578;struct Cyc_String_pa_PrintArg_struct _tmp1577;(_tmp1577.tag=0,((_tmp1577.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp1578.tag=0,((_tmp1578.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp1579[0]=& _tmp1578,((_tmp1579[1]=& _tmp1577,Cyc_Tcutil_terr(loc,((
_tmp157A="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp157A,sizeof(char),52))),_tag_dyneither(_tmp1579,sizeof(void*),2)))))))))))));}
goto _LL6CA;_LL6CA:;}}};_pop_region(temp);}void Cyc_Tcutil_check_fndecl_valid_type(
unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{void*_tmpE33=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmpE35;struct Cyc_List_List*_tmpE36;void*_tmpE37;struct
Cyc_Absyn_Tqual _tmpE38;void*_tmpE39;struct Cyc_List_List*_tmpE3A;_LL6D6: {struct
Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE34=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)
_tmpE33;if(_tmpE34->tag != 9)goto _LL6D8;else{_tmpE35=_tmpE34->f1;_tmpE36=_tmpE35.tvars;
_tmpE37=_tmpE35.effect;_tmpE38=_tmpE35.ret_tqual;_tmpE39=_tmpE35.ret_typ;_tmpE3A=
_tmpE35.args;}}_LL6D7: fd->tvs=_tmpE36;fd->effect=_tmpE37;{struct Cyc_List_List*
_tmpE3B=fd->args;for(0;_tmpE3B != 0;(_tmpE3B=_tmpE3B->tl,_tmpE3A=_tmpE3A->tl)){(*((
struct _tuple10*)_tmpE3B->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmpE3A))->hd)).f2;(*((struct _tuple10*)_tmpE3B->hd)).f3=(*((struct
_tuple10*)_tmpE3A->hd)).f3;}}fd->ret_tqual=_tmpE38;fd->ret_type=_tmpE39;(fd->ret_tqual).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpE39);
goto _LL6D5;_LL6D8:;_LL6D9: {const char*_tmp157D;void*_tmp157C;(_tmp157C=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp157D="check_fndecl_valid_type: not a FnType",
_tag_dyneither(_tmp157D,sizeof(char),38))),_tag_dyneither(_tmp157C,sizeof(void*),
0)));}_LL6D5:;}{struct _RegionHandle _tmpE3E=_new_region("r");struct _RegionHandle*
r=& _tmpE3E;_push_region(r);{const char*_tmp157E;Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct
_tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(
struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,((_tmp157E="function declaration has repeated parameter",
_tag_dyneither(_tmp157E,sizeof(char),44))));}fd->cached_typ=(void*)t;;
_pop_region(r);};}void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int
allow_evars,void*t){struct _RegionHandle _tmpE40=_new_region("r");struct
_RegionHandle*r=& _tmpE40;_push_region(r);{struct Cyc_Tcutil_CVTEnv _tmp157F;struct
Cyc_Tcutil_CVTEnv _tmpE41=Cyc_Tcutil_check_valid_type(loc,te,((_tmp157F.r=r,((
_tmp157F.kind_env=bound_tvars,((_tmp157F.free_vars=0,((_tmp157F.free_evars=0,((
_tmp157F.generalize_evars=0,((_tmp157F.fn_result=0,_tmp157F)))))))))))),
expected_kind,t);struct Cyc_List_List*_tmpE42=Cyc_Tcutil_remove_bound_tvars(r,((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct
_tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple28*))Cyc_Core_fst,_tmpE41.free_vars),bound_tvars);struct Cyc_List_List*
_tmpE43=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),
struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,
_tmpE41.free_evars);{struct Cyc_List_List*fs=_tmpE42;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpE44=((struct Cyc_Absyn_Tvar*)fs->hd)->name;const char*
_tmp1584;void*_tmp1583[2];struct Cyc_String_pa_PrintArg_struct _tmp1582;struct Cyc_String_pa_PrintArg_struct
_tmp1581;(_tmp1581.tag=0,((_tmp1581.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1582.tag=0,((_tmp1582.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmpE44),((_tmp1583[0]=& _tmp1582,((
_tmp1583[1]=& _tmp1581,Cyc_Tcutil_terr(loc,((_tmp1584="unbound type variable %s in type %s",
_tag_dyneither(_tmp1584,sizeof(char),36))),_tag_dyneither(_tmp1583,sizeof(void*),
2)))))))))))));}}if(!allow_evars  && _tmpE43 != 0)for(0;_tmpE43 != 0;_tmpE43=
_tmpE43->tl){void*e=(void*)_tmpE43->hd;struct Cyc_Absyn_Kind*_tmpE49=Cyc_Tcutil_typ_kind(
e);struct Cyc_Absyn_Kind _tmpE4A;enum Cyc_Absyn_KindQual _tmpE4B;enum Cyc_Absyn_AliasQual
_tmpE4C;struct Cyc_Absyn_Kind _tmpE4D;enum Cyc_Absyn_KindQual _tmpE4E;enum Cyc_Absyn_AliasQual
_tmpE4F;struct Cyc_Absyn_Kind _tmpE50;enum Cyc_Absyn_KindQual _tmpE51;enum Cyc_Absyn_AliasQual
_tmpE52;struct Cyc_Absyn_Kind _tmpE53;enum Cyc_Absyn_KindQual _tmpE54;_LL6DB:
_tmpE4A=*_tmpE49;_tmpE4B=_tmpE4A.kind;if(_tmpE4B != Cyc_Absyn_RgnKind)goto _LL6DD;
_tmpE4C=_tmpE4A.aliasqual;if(_tmpE4C != Cyc_Absyn_Unique)goto _LL6DD;_LL6DC: if(!
Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*_tmp1587;void*
_tmp1586;(_tmp1586=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1587="can't unify evar with unique region!",
_tag_dyneither(_tmp1587,sizeof(char),37))),_tag_dyneither(_tmp1586,sizeof(void*),
0)));}goto _LL6DA;_LL6DD: _tmpE4D=*_tmpE49;_tmpE4E=_tmpE4D.kind;if(_tmpE4E != Cyc_Absyn_RgnKind)
goto _LL6DF;_tmpE4F=_tmpE4D.aliasqual;if(_tmpE4F != Cyc_Absyn_Aliasable)goto _LL6DF;
_LL6DE: goto _LL6E0;_LL6DF: _tmpE50=*_tmpE49;_tmpE51=_tmpE50.kind;if(_tmpE51 != Cyc_Absyn_RgnKind)
goto _LL6E1;_tmpE52=_tmpE50.aliasqual;if(_tmpE52 != Cyc_Absyn_Top)goto _LL6E1;
_LL6E0: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp158A;
void*_tmp1589;(_tmp1589=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp158A="can't unify evar with heap!",_tag_dyneither(
_tmp158A,sizeof(char),28))),_tag_dyneither(_tmp1589,sizeof(void*),0)));}goto
_LL6DA;_LL6E1: _tmpE53=*_tmpE49;_tmpE54=_tmpE53.kind;if(_tmpE54 != Cyc_Absyn_EffKind)
goto _LL6E3;_LL6E2: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp158D;void*_tmp158C;(_tmp158C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp158D="can't unify evar with {}!",
_tag_dyneither(_tmp158D,sizeof(char),26))),_tag_dyneither(_tmp158C,sizeof(void*),
0)));}goto _LL6DA;_LL6E3:;_LL6E4:{const char*_tmp1592;void*_tmp1591[2];struct Cyc_String_pa_PrintArg_struct
_tmp1590;struct Cyc_String_pa_PrintArg_struct _tmp158F;(_tmp158F.tag=0,((_tmp158F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp1590.tag=0,((_tmp1590.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp1591[0]=& _tmp1590,((_tmp1591[1]=& _tmp158F,Cyc_Tcutil_terr(loc,((
_tmp1592="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp1592,sizeof(char),52))),_tag_dyneither(_tmp1591,sizeof(void*),2)))))))))))));}
goto _LL6DA;_LL6DA:;}};_pop_region(r);}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*
tv){if(tv->identity == - 1)tv->identity=Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct
_dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(
void*)vs2->hd)== 0){const char*_tmp1597;void*_tmp1596[2];struct Cyc_String_pa_PrintArg_struct
_tmp1595;struct Cyc_String_pa_PrintArg_struct _tmp1594;(_tmp1594.tag=0,((_tmp1594.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp1595.tag=
0,((_tmp1595.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp1596[0]=&
_tmp1595,((_tmp1596[1]=& _tmp1594,Cyc_Tcutil_terr(loc,((_tmp1597="%s: %s",
_tag_dyneither(_tmp1597,sizeof(char),7))),_tag_dyneither(_tmp1596,sizeof(void*),
2)))))))))))));}}}}static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct
_dyneither_ptr*s){return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,unsigned int loc,struct _dyneither_ptr msg){((void(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,
struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))
Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,
msg);}void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
const char*_tmp1598;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),
struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,
tvs,loc,Cyc_Tcutil_tvar2string,((_tmp1598="duplicate type variable",
_tag_dyneither(_tmp1598,sizeof(char),24))));}struct _tuple31{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple32{struct Cyc_List_List*f1;void*f2;};struct _tuple33{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind
aggr_kind,struct Cyc_List_List*sdfields){struct _RegionHandle _tmpE65=_new_region("temp");
struct _RegionHandle*temp=& _tmpE65;_push_region(temp);{struct Cyc_List_List*fields=
0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){const char*_tmp1599;if(Cyc_strcmp((struct _dyneither_ptr)*((struct
Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp1599="",_tag_dyneither(_tmp1599,
sizeof(char),1))))!= 0){struct _tuple31*_tmp159C;struct Cyc_List_List*_tmp159B;
fields=((_tmp159B=_region_malloc(temp,sizeof(*_tmp159B)),((_tmp159B->hd=((
_tmp159C=_region_malloc(temp,sizeof(*_tmp159C)),((_tmp159C->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd,((_tmp159C->f2=0,_tmp159C)))))),((_tmp159B->tl=fields,_tmp159B))))));}}}
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
const char*_tmp159E;const char*_tmp159D;struct _dyneither_ptr aggr_str=aggr_kind == 
Cyc_Absyn_StructA?(_tmp159E="struct",_tag_dyneither(_tmp159E,sizeof(char),7)):((
_tmp159D="union",_tag_dyneither(_tmp159D,sizeof(char),6)));struct Cyc_List_List*
ans=0;for(0;des != 0;des=des->tl){struct _tuple32 _tmpE6A;struct Cyc_List_List*
_tmpE6B;void*_tmpE6C;struct _tuple32*_tmpE69=(struct _tuple32*)des->hd;_tmpE6A=*
_tmpE69;_tmpE6B=_tmpE6A.f1;_tmpE6C=_tmpE6A.f2;if(_tmpE6B == 0){struct Cyc_List_List*
_tmpE6D=fields;for(0;_tmpE6D != 0;_tmpE6D=_tmpE6D->tl){if(!(*((struct _tuple31*)
_tmpE6D->hd)).f2){(*((struct _tuple31*)_tmpE6D->hd)).f2=1;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*
_tmp15A4;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp15A3;struct Cyc_List_List*
_tmp15A2;(*((struct _tuple32*)des->hd)).f1=(struct Cyc_List_List*)((_tmp15A2=
_cycalloc(sizeof(*_tmp15A2)),((_tmp15A2->hd=(void*)((_tmp15A4=_cycalloc(sizeof(*
_tmp15A4)),((_tmp15A4[0]=((_tmp15A3.tag=1,((_tmp15A3.f1=((*((struct _tuple31*)
_tmpE6D->hd)).f1)->name,_tmp15A3)))),_tmp15A4)))),((_tmp15A2->tl=0,_tmp15A2))))));}{
struct _tuple33*_tmp15A7;struct Cyc_List_List*_tmp15A6;ans=((_tmp15A6=
_region_malloc(rgn,sizeof(*_tmp15A6)),((_tmp15A6->hd=((_tmp15A7=_region_malloc(
rgn,sizeof(*_tmp15A7)),((_tmp15A7->f1=(*((struct _tuple31*)_tmpE6D->hd)).f1,((
_tmp15A7->f2=_tmpE6C,_tmp15A7)))))),((_tmp15A6->tl=ans,_tmp15A6))))));}break;}}
if(_tmpE6D == 0){const char*_tmp15AB;void*_tmp15AA[1];struct Cyc_String_pa_PrintArg_struct
_tmp15A9;(_tmp15A9.tag=0,((_tmp15A9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)aggr_str),((_tmp15AA[0]=& _tmp15A9,Cyc_Tcutil_terr(loc,((_tmp15AB="too many arguments to %s",
_tag_dyneither(_tmp15AB,sizeof(char),25))),_tag_dyneither(_tmp15AA,sizeof(void*),
1)))))));}}else{if(_tmpE6B->tl != 0){const char*_tmp15AE;void*_tmp15AD;(_tmp15AD=0,
Cyc_Tcutil_terr(loc,((_tmp15AE="multiple designators are not yet supported",
_tag_dyneither(_tmp15AE,sizeof(char),43))),_tag_dyneither(_tmp15AD,sizeof(void*),
0)));}else{void*_tmpE78=(void*)_tmpE6B->hd;struct _dyneither_ptr*_tmpE7B;_LL6E6: {
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpE79=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)
_tmpE78;if(_tmpE79->tag != 0)goto _LL6E8;}_LL6E7:{const char*_tmp15B2;void*_tmp15B1[
1];struct Cyc_String_pa_PrintArg_struct _tmp15B0;(_tmp15B0.tag=0,((_tmp15B0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp15B1[0]=& _tmp15B0,
Cyc_Tcutil_terr(loc,((_tmp15B2="array designator used in argument to %s",
_tag_dyneither(_tmp15B2,sizeof(char),40))),_tag_dyneither(_tmp15B1,sizeof(void*),
1)))))));}goto _LL6E5;_LL6E8: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*
_tmpE7A=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpE78;if(_tmpE7A->tag
!= 1)goto _LL6E5;else{_tmpE7B=_tmpE7A->f1;}}_LL6E9: {struct Cyc_List_List*_tmpE7F=
fields;for(0;_tmpE7F != 0;_tmpE7F=_tmpE7F->tl){if(Cyc_strptrcmp(_tmpE7B,((*((
struct _tuple31*)_tmpE7F->hd)).f1)->name)== 0){if((*((struct _tuple31*)_tmpE7F->hd)).f2){
const char*_tmp15B6;void*_tmp15B5[1];struct Cyc_String_pa_PrintArg_struct _tmp15B4;(
_tmp15B4.tag=0,((_tmp15B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpE7B),((_tmp15B5[0]=& _tmp15B4,Cyc_Tcutil_terr(loc,((_tmp15B6="member %s has already been used as an argument",
_tag_dyneither(_tmp15B6,sizeof(char),47))),_tag_dyneither(_tmp15B5,sizeof(void*),
1)))))));}(*((struct _tuple31*)_tmpE7F->hd)).f2=1;{struct _tuple33*_tmp15B9;struct
Cyc_List_List*_tmp15B8;ans=((_tmp15B8=_region_malloc(rgn,sizeof(*_tmp15B8)),((
_tmp15B8->hd=((_tmp15B9=_region_malloc(rgn,sizeof(*_tmp15B9)),((_tmp15B9->f1=(*((
struct _tuple31*)_tmpE7F->hd)).f1,((_tmp15B9->f2=_tmpE6C,_tmp15B9)))))),((
_tmp15B8->tl=ans,_tmp15B8))))));}break;}}if(_tmpE7F == 0){const char*_tmp15BD;void*
_tmp15BC[1];struct Cyc_String_pa_PrintArg_struct _tmp15BB;(_tmp15BB.tag=0,((
_tmp15BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE7B),((_tmp15BC[0]=&
_tmp15BB,Cyc_Tcutil_terr(loc,((_tmp15BD="bad field designator %s",_tag_dyneither(
_tmp15BD,sizeof(char),24))),_tag_dyneither(_tmp15BC,sizeof(void*),1)))))));}goto
_LL6E5;}_LL6E5:;}}}if(aggr_kind == Cyc_Absyn_StructA)for(0;fields != 0;fields=
fields->tl){if(!(*((struct _tuple31*)fields->hd)).f2){{const char*_tmp15C0;void*
_tmp15BF;(_tmp15BF=0,Cyc_Tcutil_terr(loc,((_tmp15C0="too few arguments to struct",
_tag_dyneither(_tmp15C0,sizeof(char),28))),_tag_dyneither(_tmp15BF,sizeof(void*),
0)));}break;}}else{int found=0;for(0;fields != 0;fields=fields->tl){if((*((struct
_tuple31*)fields->hd)).f2){if(found){const char*_tmp15C3;void*_tmp15C2;(_tmp15C2=
0,Cyc_Tcutil_terr(loc,((_tmp15C3="only one member of a union is allowed",
_tag_dyneither(_tmp15C3,sizeof(char),38))),_tag_dyneither(_tmp15C2,sizeof(void*),
0)));}found=1;}}if(!found){const char*_tmp15C6;void*_tmp15C5;(_tmp15C5=0,Cyc_Tcutil_terr(
loc,((_tmp15C6="missing member for union",_tag_dyneither(_tmp15C6,sizeof(char),
25))),_tag_dyneither(_tmp15C5,sizeof(void*),0)));}}{struct Cyc_List_List*_tmpE8E=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);
_npop_handler(0);return _tmpE8E;};};};_pop_region(temp);}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpE91=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpE93;void*_tmpE94;struct Cyc_Absyn_PtrAtts _tmpE95;union Cyc_Absyn_Constraint*
_tmpE96;_LL6EB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE92=(struct
Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE91;if(_tmpE92->tag != 5)goto _LL6ED;
else{_tmpE93=_tmpE92->f1;_tmpE94=_tmpE93.elt_typ;_tmpE95=_tmpE93.ptr_atts;
_tmpE96=_tmpE95.bounds;}}_LL6EC: {void*_tmpE97=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpE96);_LL6F0: {
struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpE98=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)
_tmpE97;if(_tmpE98->tag != 0)goto _LL6F2;}_LL6F1:*elt_typ_dest=_tmpE94;return 1;
_LL6F2:;_LL6F3: return 0;_LL6EF:;}_LL6ED:;_LL6EE: return 0;_LL6EA:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpE99=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpE9B;void*_tmpE9C;struct Cyc_Absyn_PtrAtts _tmpE9D;union Cyc_Absyn_Constraint*
_tmpE9E;_LL6F5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE9A=(struct
Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE99;if(_tmpE9A->tag != 5)goto _LL6F7;
else{_tmpE9B=_tmpE9A->f1;_tmpE9C=_tmpE9B.elt_typ;_tmpE9D=_tmpE9B.ptr_atts;
_tmpE9E=_tmpE9D.zero_term;}}_LL6F6:*elt_typ_dest=_tmpE9C;return((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE9E);_LL6F7:;_LL6F8:
return 0;_LL6F4:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*
is_dyneither,void**elt_type){void*_tmpE9F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpEA1;void*_tmpEA2;struct Cyc_Absyn_PtrAtts _tmpEA3;union Cyc_Absyn_Constraint*
_tmpEA4;union Cyc_Absyn_Constraint*_tmpEA5;struct Cyc_Absyn_ArrayInfo _tmpEA7;void*
_tmpEA8;struct Cyc_Absyn_Tqual _tmpEA9;struct Cyc_Absyn_Exp*_tmpEAA;union Cyc_Absyn_Constraint*
_tmpEAB;_LL6FA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpEA0=(struct
Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE9F;if(_tmpEA0->tag != 5)goto _LL6FC;
else{_tmpEA1=_tmpEA0->f1;_tmpEA2=_tmpEA1.elt_typ;_tmpEA3=_tmpEA1.ptr_atts;
_tmpEA4=_tmpEA3.bounds;_tmpEA5=_tmpEA3.zero_term;}}_LL6FB: if(((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEA5)){*ptr_type=t;*elt_type=
_tmpEA2;{void*_tmpEAC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmpEA4);_LL701: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmpEAD=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpEAC;if(_tmpEAD->tag
!= 0)goto _LL703;}_LL702:*is_dyneither=1;goto _LL700;_LL703:;_LL704:*is_dyneither=
0;goto _LL700;_LL700:;}return 1;}else{return 0;}_LL6FC: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmpEA6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE9F;if(_tmpEA6->tag != 
8)goto _LL6FE;else{_tmpEA7=_tmpEA6->f1;_tmpEA8=_tmpEA7.elt_type;_tmpEA9=_tmpEA7.tq;
_tmpEAA=_tmpEA7.num_elts;_tmpEAB=_tmpEA7.zero_term;}}_LL6FD: if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEAB)){*elt_type=_tmpEA8;*
is_dyneither=0;{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp15DB;struct Cyc_Absyn_PtrAtts
_tmp15DA;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp15D9;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp15D8;struct Cyc_Absyn_PtrInfo _tmp15D7;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp15D6;*ptr_type=(void*)((_tmp15D6=_cycalloc(sizeof(*_tmp15D6)),((_tmp15D6[0]=((
_tmp15DB.tag=5,((_tmp15DB.f1=((_tmp15D7.elt_typ=_tmpEA8,((_tmp15D7.elt_tq=
_tmpEA9,((_tmp15D7.ptr_atts=((_tmp15DA.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((
_tmp15DA.nullable=Cyc_Absyn_false_conref,((_tmp15DA.bounds=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp15D8=_cycalloc(sizeof(*_tmp15D8)),((
_tmp15D8[0]=((_tmp15D9.tag=1,((_tmp15D9.f1=(struct Cyc_Absyn_Exp*)_check_null(
_tmpEAA),_tmp15D9)))),_tmp15D8))))),((_tmp15DA.zero_term=_tmpEAB,((_tmp15DA.ptrloc=
0,_tmp15DA)))))))))),_tmp15D7)))))),_tmp15DB)))),_tmp15D6))));}return 1;}else{
return 0;}_LL6FE:;_LL6FF: return 0;_LL6F9:;}int Cyc_Tcutil_is_zero_ptr_deref(struct
Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpEB4=e1->r;
struct Cyc_Absyn_Exp*_tmpEB7;struct Cyc_Absyn_Exp*_tmpEB9;struct Cyc_Absyn_Exp*
_tmpEBB;struct Cyc_Absyn_Exp*_tmpEBD;struct Cyc_Absyn_Exp*_tmpEBF;struct Cyc_Absyn_Exp*
_tmpEC1;_LL706: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpEB5=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)
_tmpEB4;if(_tmpEB5->tag != 13)goto _LL708;}_LL707: {const char*_tmp15DF;void*
_tmp15DE[1];struct Cyc_String_pa_PrintArg_struct _tmp15DD;(_tmp15DD.tag=0,((
_tmp15DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp15DE[0]=& _tmp15DD,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15DF="we have a cast in a lhs:  %s",
_tag_dyneither(_tmp15DF,sizeof(char),29))),_tag_dyneither(_tmp15DE,sizeof(void*),
1)))))));}_LL708: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpEB6=(struct
Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpEB4;if(_tmpEB6->tag != 19)goto _LL70A;
else{_tmpEB7=_tmpEB6->f1;}}_LL709: _tmpEB9=_tmpEB7;goto _LL70B;_LL70A: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*
_tmpEB8=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpEB4;if(_tmpEB8->tag
!= 22)goto _LL70C;else{_tmpEB9=_tmpEB8->f1;}}_LL70B: return Cyc_Tcutil_is_zero_ptr_type((
void*)_check_null(_tmpEB9->topt),ptr_type,is_dyneither,elt_type);_LL70C: {struct
Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpEBA=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)
_tmpEB4;if(_tmpEBA->tag != 21)goto _LL70E;else{_tmpEBB=_tmpEBA->f1;}}_LL70D:
_tmpEBD=_tmpEBB;goto _LL70F;_LL70E: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*
_tmpEBC=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpEB4;if(_tmpEBC->tag
!= 20)goto _LL710;else{_tmpEBD=_tmpEBC->f1;}}_LL70F: if(Cyc_Tcutil_is_zero_ptr_type((
void*)_check_null(_tmpEBD->topt),ptr_type,is_dyneither,elt_type)){const char*
_tmp15E3;void*_tmp15E2[1];struct Cyc_String_pa_PrintArg_struct _tmp15E1;(_tmp15E1.tag=
0,((_tmp15E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp15E2[0]=& _tmp15E1,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15E3="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp15E3,sizeof(char),51))),_tag_dyneither(_tmp15E2,sizeof(void*),
1)))))));}return 0;_LL710: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*
_tmpEBE=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpEB4;if(_tmpEBE->tag
!= 12)goto _LL712;else{_tmpEBF=_tmpEBE->f1;}}_LL711: _tmpEC1=_tmpEBF;goto _LL713;
_LL712: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpEC0=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)
_tmpEB4;if(_tmpEC0->tag != 11)goto _LL714;else{_tmpEC1=_tmpEC0->f1;}}_LL713: if(Cyc_Tcutil_is_zero_ptr_type((
void*)_check_null(_tmpEC1->topt),ptr_type,is_dyneither,elt_type)){const char*
_tmp15E7;void*_tmp15E6[1];struct Cyc_String_pa_PrintArg_struct _tmp15E5;(_tmp15E5.tag=
0,((_tmp15E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp15E6[0]=& _tmp15E5,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15E7="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp15E7,sizeof(char),49))),_tag_dyneither(_tmp15E6,sizeof(void*),
1)))))));}return 0;_LL714: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpEC2=(
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpEB4;if(_tmpEC2->tag != 1)goto
_LL716;}_LL715: return 0;_LL716:;_LL717: {const char*_tmp15EB;void*_tmp15EA[1];
struct Cyc_String_pa_PrintArg_struct _tmp15E9;(_tmp15E9.tag=0,((_tmp15E9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp15EA[0]=& _tmp15E9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp15EB="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp15EB,sizeof(char),39))),_tag_dyneither(_tmp15EA,sizeof(void*),
1)))))));}_LL705:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t){void*ignore=(void*)&
Cyc_Absyn_VoidType_val;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){void*_tmpECF=Cyc_Tcutil_compress(
r);struct Cyc_Absyn_Tvar*_tmpED3;_LL719: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*
_tmpED0=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpECF;if(_tmpED0->tag != 
22)goto _LL71B;}_LL71A: return !must_be_unique;_LL71B: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*
_tmpED1=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpECF;if(_tmpED1->tag != 
21)goto _LL71D;}_LL71C: return 1;_LL71D: {struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmpED2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpECF;if(_tmpED2->tag != 2)
goto _LL71F;else{_tmpED3=_tmpED2->f1;}}_LL71E: {struct Cyc_Absyn_Kind _tmpED5;enum 
Cyc_Absyn_KindQual _tmpED6;enum Cyc_Absyn_AliasQual _tmpED7;struct Cyc_Absyn_Kind*
_tmpED4=Cyc_Tcutil_tvar_kind(_tmpED3,& Cyc_Tcutil_rk);_tmpED5=*_tmpED4;_tmpED6=
_tmpED5.kind;_tmpED7=_tmpED5.aliasqual;if(_tmpED6 == Cyc_Absyn_RgnKind  && (
_tmpED7 == Cyc_Absyn_Unique  || _tmpED7 == Cyc_Absyn_Top  && !must_be_unique)){void*
_tmpED8=Cyc_Absyn_compress_kb(_tmpED3->kind);struct Cyc_Core_Opt*_tmpEDA;struct
Cyc_Core_Opt**_tmpEDB;struct Cyc_Absyn_Kind*_tmpEDC;struct Cyc_Absyn_Kind _tmpEDD;
enum Cyc_Absyn_KindQual _tmpEDE;enum Cyc_Absyn_AliasQual _tmpEDF;_LL722: {struct
Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpED9=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)
_tmpED8;if(_tmpED9->tag != 2)goto _LL724;else{_tmpEDA=_tmpED9->f1;_tmpEDB=(struct
Cyc_Core_Opt**)& _tmpED9->f1;_tmpEDC=_tmpED9->f2;_tmpEDD=*_tmpEDC;_tmpEDE=_tmpEDD.kind;
if(_tmpEDE != Cyc_Absyn_RgnKind)goto _LL724;_tmpEDF=_tmpEDD.aliasqual;if(_tmpEDF != 
Cyc_Absyn_Top)goto _LL724;}}_LL723:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmp15F1;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp15F0;struct Cyc_Core_Opt*
_tmp15EF;*_tmpEDB=((_tmp15EF=_cycalloc(sizeof(*_tmp15EF)),((_tmp15EF->v=(void*)((
_tmp15F1=_cycalloc(sizeof(*_tmp15F1)),((_tmp15F1[0]=((_tmp15F0.tag=2,((_tmp15F0.f1=
0,((_tmp15F0.f2=& Cyc_Tcutil_rk,_tmp15F0)))))),_tmp15F1)))),_tmp15EF))));}return 0;
_LL724:;_LL725: return 1;_LL721:;}return 0;}_LL71F:;_LL720: return 0;_LL718:;}static
int Cyc_Tcutil_is_noalias_pointer_aux(void*t,int must_be_unique){void*_tmpEE3=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpEE5;struct Cyc_Absyn_PtrAtts _tmpEE6;void*_tmpEE7;
struct Cyc_Absyn_Tvar*_tmpEE9;_LL727: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmpEE4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEE3;if(_tmpEE4->tag
!= 5)goto _LL729;else{_tmpEE5=_tmpEE4->f1;_tmpEE6=_tmpEE5.ptr_atts;_tmpEE7=
_tmpEE6.rgn;}}_LL728: return Cyc_Tcutil_is_noalias_region(_tmpEE7,must_be_unique);
_LL729: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpEE8=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)
_tmpEE3;if(_tmpEE8->tag != 2)goto _LL72B;else{_tmpEE9=_tmpEE8->f1;}}_LL72A: {
struct Cyc_Absyn_Kind _tmpEEB;enum Cyc_Absyn_KindQual _tmpEEC;enum Cyc_Absyn_AliasQual
_tmpEED;struct Cyc_Absyn_Kind*_tmpEEA=Cyc_Tcutil_tvar_kind(_tmpEE9,& Cyc_Tcutil_bk);
_tmpEEB=*_tmpEEA;_tmpEEC=_tmpEEB.kind;_tmpEED=_tmpEEB.aliasqual;switch(_tmpEEC){
case Cyc_Absyn_BoxKind: _LL72D: goto _LL72E;case Cyc_Absyn_AnyKind: _LL72E: goto _LL72F;
case Cyc_Absyn_MemKind: _LL72F: if(_tmpEED == Cyc_Absyn_Unique  || _tmpEED == Cyc_Absyn_Top){
void*_tmpEEE=Cyc_Absyn_compress_kb(_tmpEE9->kind);struct Cyc_Core_Opt*_tmpEF0;
struct Cyc_Core_Opt**_tmpEF1;struct Cyc_Absyn_Kind*_tmpEF2;struct Cyc_Absyn_Kind
_tmpEF3;enum Cyc_Absyn_KindQual _tmpEF4;enum Cyc_Absyn_AliasQual _tmpEF5;_LL732: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpEEF=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)
_tmpEEE;if(_tmpEEF->tag != 2)goto _LL734;else{_tmpEF0=_tmpEEF->f1;_tmpEF1=(struct
Cyc_Core_Opt**)& _tmpEEF->f1;_tmpEF2=_tmpEEF->f2;_tmpEF3=*_tmpEF2;_tmpEF4=_tmpEF3.kind;
_tmpEF5=_tmpEF3.aliasqual;if(_tmpEF5 != Cyc_Absyn_Top)goto _LL734;}}_LL733:{struct
Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp15FB;struct Cyc_Absyn_Kind*_tmp15FA;
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp15F9;struct Cyc_Core_Opt*
_tmp15F8;*_tmpEF1=((_tmp15F8=_cycalloc(sizeof(*_tmp15F8)),((_tmp15F8->v=(void*)((
_tmp15FB=_cycalloc(sizeof(*_tmp15FB)),((_tmp15FB[0]=((_tmp15F9.tag=2,((_tmp15F9.f1=
0,((_tmp15F9.f2=((_tmp15FA=_cycalloc_atomic(sizeof(*_tmp15FA)),((_tmp15FA->kind=
_tmpEF4,((_tmp15FA->aliasqual=Cyc_Absyn_Aliasable,_tmp15FA)))))),_tmp15F9)))))),
_tmp15FB)))),_tmp15F8))));}return 0;_LL734:;_LL735: return 1;_LL731:;}return 0;
default: _LL730: return 0;}}_LL72B:;_LL72C: return 0;_LL726:;}int Cyc_Tcutil_is_noalias_pointer(
void*t){return Cyc_Tcutil_is_noalias_pointer_aux(t,0);}int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*rgn,void*t){void*_tmpEFA=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(
_tmpEFA))return 1;{void*_tmpEFB=_tmpEFA;struct Cyc_List_List*_tmpEFD;struct Cyc_Absyn_AggrInfo
_tmpEFF;union Cyc_Absyn_AggrInfoU _tmpF00;struct Cyc_Absyn_Aggrdecl**_tmpF01;struct
Cyc_List_List*_tmpF02;struct Cyc_List_List*_tmpF04;struct Cyc_Absyn_AggrInfo
_tmpF06;union Cyc_Absyn_AggrInfoU _tmpF07;struct _tuple4 _tmpF08;struct Cyc_Absyn_DatatypeInfo
_tmpF0A;union Cyc_Absyn_DatatypeInfoU _tmpF0B;struct Cyc_List_List*_tmpF0C;struct
Cyc_Absyn_DatatypeFieldInfo _tmpF0E;union Cyc_Absyn_DatatypeFieldInfoU _tmpF0F;
struct Cyc_List_List*_tmpF10;_LL738: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmpEFC=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpEFB;if(_tmpEFC->tag != 
10)goto _LL73A;else{_tmpEFD=_tmpEFC->f1;}}_LL739: while(_tmpEFD != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,(*((struct _tuple12*)_tmpEFD->hd)).f2))return 1;_tmpEFD=_tmpEFD->tl;}return 0;
_LL73A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpEFE=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmpEFB;if(_tmpEFE->tag != 11)goto _LL73C;else{_tmpEFF=_tmpEFE->f1;_tmpF00=_tmpEFF.aggr_info;
if((_tmpF00.KnownAggr).tag != 2)goto _LL73C;_tmpF01=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF00.KnownAggr).val;_tmpF02=_tmpEFF.targs;}}_LL73B: if((*_tmpF01)->impl == 0)
return 0;else{struct Cyc_List_List*_tmpF11=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpF01)->tvs,_tmpF02);struct Cyc_List_List*_tmpF12=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpF01)->impl))->fields;void*t;
while(_tmpF12 != 0){t=_tmpF11 == 0?((struct Cyc_Absyn_Aggrfield*)_tmpF12->hd)->type:
Cyc_Tcutil_rsubstitute(rgn,_tmpF11,((struct Cyc_Absyn_Aggrfield*)_tmpF12->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;_tmpF12=_tmpF12->tl;}
return 0;}_LL73C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF03=(struct
Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpEFB;if(_tmpF03->tag != 12)goto
_LL73E;else{_tmpF04=_tmpF03->f2;}}_LL73D: while(_tmpF04 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,((struct Cyc_Absyn_Aggrfield*)_tmpF04->hd)->type))return 1;_tmpF04=_tmpF04->tl;}
return 0;_LL73E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF05=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmpEFB;if(_tmpF05->tag != 11)goto _LL740;else{_tmpF06=_tmpF05->f1;_tmpF07=_tmpF06.aggr_info;
if((_tmpF07.UnknownAggr).tag != 1)goto _LL740;_tmpF08=(struct _tuple4)(_tmpF07.UnknownAggr).val;}}
_LL73F: {const char*_tmp15FE;void*_tmp15FD;(_tmp15FD=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15FE="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp15FE,sizeof(char),36))),_tag_dyneither(_tmp15FD,sizeof(void*),
0)));}_LL740: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpF09=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)
_tmpEFB;if(_tmpF09->tag != 3)goto _LL742;else{_tmpF0A=_tmpF09->f1;_tmpF0B=_tmpF0A.datatype_info;
_tmpF0C=_tmpF0A.targs;}}_LL741: {union Cyc_Absyn_DatatypeInfoU _tmpF15=_tmpF0B;
struct Cyc_Absyn_UnknownDatatypeInfo _tmpF16;struct _tuple2*_tmpF17;int _tmpF18;
struct Cyc_Absyn_Datatypedecl**_tmpF19;struct Cyc_Absyn_Datatypedecl*_tmpF1A;
struct Cyc_Absyn_Datatypedecl _tmpF1B;struct Cyc_List_List*_tmpF1C;struct Cyc_Core_Opt*
_tmpF1D;_LL747: if((_tmpF15.UnknownDatatype).tag != 1)goto _LL749;_tmpF16=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpF15.UnknownDatatype).val;_tmpF17=_tmpF16.name;
_tmpF18=_tmpF16.is_extensible;_LL748: {const char*_tmp1601;void*_tmp1600;(
_tmp1600=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1601="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp1601,
sizeof(char),40))),_tag_dyneither(_tmp1600,sizeof(void*),0)));}_LL749: if((
_tmpF15.KnownDatatype).tag != 2)goto _LL746;_tmpF19=(struct Cyc_Absyn_Datatypedecl**)(
_tmpF15.KnownDatatype).val;_tmpF1A=*_tmpF19;_tmpF1B=*_tmpF1A;_tmpF1C=_tmpF1B.tvs;
_tmpF1D=_tmpF1B.fields;_LL74A: return 0;_LL746:;}_LL742: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmpF0D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpEFB;if(_tmpF0D->tag
!= 4)goto _LL744;else{_tmpF0E=_tmpF0D->f1;_tmpF0F=_tmpF0E.field_info;_tmpF10=
_tmpF0E.targs;}}_LL743: {union Cyc_Absyn_DatatypeFieldInfoU _tmpF20=_tmpF0F;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmpF21;struct _tuple3 _tmpF22;struct Cyc_Absyn_Datatypedecl*
_tmpF23;struct Cyc_Absyn_Datatypefield*_tmpF24;_LL74C: if((_tmpF20.UnknownDatatypefield).tag
!= 1)goto _LL74E;_tmpF21=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpF20.UnknownDatatypefield).val;
_LL74D: {const char*_tmp1604;void*_tmp1603;(_tmp1603=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1604="got unknown datatype field in is_noalias_aggr",
_tag_dyneither(_tmp1604,sizeof(char),46))),_tag_dyneither(_tmp1603,sizeof(void*),
0)));}_LL74E: if((_tmpF20.KnownDatatypefield).tag != 2)goto _LL74B;_tmpF22=(struct
_tuple3)(_tmpF20.KnownDatatypefield).val;_tmpF23=_tmpF22.f1;_tmpF24=_tmpF22.f2;
_LL74F: {struct Cyc_List_List*_tmpF27=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpF23->tvs,_tmpF10);struct Cyc_List_List*_tmpF28=
_tmpF24->typs;while(_tmpF28 != 0){_tmpEFA=_tmpF27 == 0?(*((struct _tuple12*)_tmpF28->hd)).f2:
Cyc_Tcutil_rsubstitute(rgn,_tmpF27,(*((struct _tuple12*)_tmpF28->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,_tmpEFA))return 1;_tmpF28=_tmpF28->tl;}return 0;}_LL74B:;}_LL744:;_LL745:
return 0;_LL737:;};}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,
void*t,struct _dyneither_ptr*f){void*_tmpF29=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpF2B;union Cyc_Absyn_AggrInfoU _tmpF2C;struct Cyc_Absyn_Aggrdecl**_tmpF2D;struct
Cyc_Absyn_Aggrdecl*_tmpF2E;struct Cyc_List_List*_tmpF2F;struct Cyc_List_List*
_tmpF31;_LL751: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF2A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmpF29;if(_tmpF2A->tag != 11)goto _LL753;else{_tmpF2B=_tmpF2A->f1;_tmpF2C=_tmpF2B.aggr_info;
if((_tmpF2C.KnownAggr).tag != 2)goto _LL753;_tmpF2D=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF2C.KnownAggr).val;_tmpF2E=*_tmpF2D;_tmpF2F=_tmpF2B.targs;}}_LL752: _tmpF31=
_tmpF2E->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF2E->impl))->fields;
goto _LL754;_LL753: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF30=(
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF29;if(_tmpF30->tag != 12)
goto _LL755;else{_tmpF31=_tmpF30->f2;}}_LL754: {struct Cyc_Absyn_Aggrfield*_tmpF32=
Cyc_Absyn_lookup_field(_tmpF31,f);{struct Cyc_Absyn_Aggrfield*_tmpF33=_tmpF32;
struct Cyc_Absyn_Aggrfield _tmpF34;void*_tmpF35;_LL758: if(_tmpF33 != 0)goto _LL75A;
_LL759: {const char*_tmp1607;void*_tmp1606;(_tmp1606=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1607="is_noalias_field: missing field",
_tag_dyneither(_tmp1607,sizeof(char),32))),_tag_dyneither(_tmp1606,sizeof(void*),
0)));}_LL75A: if(_tmpF33 == 0)goto _LL757;_tmpF34=*_tmpF33;_tmpF35=_tmpF34.type;
_LL75B: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpF35);_LL757:;}return 0;}
_LL755:;_LL756: {const char*_tmp160B;void*_tmp160A[1];struct Cyc_String_pa_PrintArg_struct
_tmp1609;(_tmp1609.tag=0,((_tmp1609.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp160A[0]=& _tmp1609,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp160B="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp160B,sizeof(char),36))),_tag_dyneither(_tmp160A,sizeof(void*),
1)))))));}_LL750:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e){void*_tmpF3B=e->r;void*_tmpF3D;struct Cyc_Absyn_Exp*
_tmpF40;struct Cyc_Absyn_Exp*_tmpF42;struct Cyc_Absyn_Exp*_tmpF44;struct
_dyneither_ptr*_tmpF45;struct Cyc_Absyn_Exp*_tmpF47;struct Cyc_Absyn_Exp*_tmpF48;
struct Cyc_Absyn_Exp*_tmpF4A;struct Cyc_Absyn_Exp*_tmpF4B;struct Cyc_Absyn_Exp*
_tmpF4D;struct Cyc_Absyn_Exp*_tmpF4F;struct Cyc_Absyn_Stmt*_tmpF51;_LL75D: {struct
Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpF3C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmpF3B;if(_tmpF3C->tag != 1)goto _LL75F;else{_tmpF3D=(void*)_tmpF3C->f2;{struct
Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpF3E=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)
_tmpF3D;if(_tmpF3E->tag != 1)goto _LL75F;};}}_LL75E: return 0;_LL75F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*
_tmpF3F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF3B;if(_tmpF3F->tag
!= 21)goto _LL761;else{_tmpF40=_tmpF3F->f1;}}_LL760: _tmpF42=_tmpF40;goto _LL762;
_LL761: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpF41=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)
_tmpF3B;if(_tmpF41->tag != 19)goto _LL763;else{_tmpF42=_tmpF41->f1;}}_LL762: return
Cyc_Tcutil_is_noalias_pointer_aux((void*)_check_null(_tmpF42->topt),1) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpF42);_LL763: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpF43=(
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF3B;if(_tmpF43->tag != 20)
goto _LL765;else{_tmpF44=_tmpF43->f1;_tmpF45=_tmpF43->f2;}}_LL764: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpF44);_LL765: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpF46=(
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF3B;if(_tmpF46->tag != 22)
goto _LL767;else{_tmpF47=_tmpF46->f1;_tmpF48=_tmpF46->f2;}}_LL766: {void*_tmpF52=
Cyc_Tcutil_compress((void*)_check_null(_tmpF47->topt));_LL772: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmpF53=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF52;if(_tmpF53->tag != 
10)goto _LL774;}_LL773: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF47);_LL774:;
_LL775: return 0;_LL771:;}_LL767: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*
_tmpF49=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF3B;if(_tmpF49->tag
!= 5)goto _LL769;else{_tmpF4A=_tmpF49->f2;_tmpF4B=_tmpF49->f3;}}_LL768: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpF4A) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpF4B);_LL769: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*
_tmpF4C=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpF3B;if(_tmpF4C->tag
!= 8)goto _LL76B;else{_tmpF4D=_tmpF4C->f2;}}_LL76A: _tmpF4F=_tmpF4D;goto _LL76C;
_LL76B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpF4E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)
_tmpF3B;if(_tmpF4E->tag != 13)goto _LL76D;else{_tmpF4F=_tmpF4E->f2;}}_LL76C: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpF4F);_LL76D: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*
_tmpF50=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpF3B;if(_tmpF50->tag
!= 35)goto _LL76F;else{_tmpF51=_tmpF50->f1;}}_LL76E: while(1){void*_tmpF54=_tmpF51->r;
struct Cyc_Absyn_Stmt*_tmpF56;struct Cyc_Absyn_Stmt*_tmpF57;struct Cyc_Absyn_Decl*
_tmpF59;struct Cyc_Absyn_Stmt*_tmpF5A;struct Cyc_Absyn_Exp*_tmpF5C;_LL777: {struct
Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpF55=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)
_tmpF54;if(_tmpF55->tag != 2)goto _LL779;else{_tmpF56=_tmpF55->f1;_tmpF57=_tmpF55->f2;}}
_LL778: _tmpF51=_tmpF57;goto _LL776;_LL779: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*
_tmpF58=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpF54;if(_tmpF58->tag != 
12)goto _LL77B;else{_tmpF59=_tmpF58->f1;_tmpF5A=_tmpF58->f2;}}_LL77A: _tmpF51=
_tmpF5A;goto _LL776;_LL77B: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpF5B=(
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpF54;if(_tmpF5B->tag != 1)goto
_LL77D;else{_tmpF5C=_tmpF5B->f1;}}_LL77C: return Cyc_Tcutil_is_noalias_path_aux(r,
_tmpF5C);_LL77D:;_LL77E: {const char*_tmp160E;void*_tmp160D;(_tmp160D=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp160E="is_noalias_stmt_exp: ill-formed StmtExp",
_tag_dyneither(_tmp160E,sizeof(char),40))),_tag_dyneither(_tmp160D,sizeof(void*),
0)));}_LL776:;}_LL76F:;_LL770: return 1;_LL75C:;}int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
r,e);}struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){struct _tuple18 _tmp160F;struct _tuple18 bogus_ans=(_tmp160F.f1=0,((_tmp160F.f2=(
void*)& Cyc_Absyn_HeapRgn_val,_tmp160F)));void*_tmpF5F=e->r;struct _tuple2*_tmpF61;
void*_tmpF62;struct Cyc_Absyn_Exp*_tmpF64;struct _dyneither_ptr*_tmpF65;int _tmpF66;
struct Cyc_Absyn_Exp*_tmpF68;struct _dyneither_ptr*_tmpF69;int _tmpF6A;struct Cyc_Absyn_Exp*
_tmpF6C;struct Cyc_Absyn_Exp*_tmpF6E;struct Cyc_Absyn_Exp*_tmpF6F;_LL780: {struct
Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpF60=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmpF5F;if(_tmpF60->tag != 1)goto _LL782;else{_tmpF61=_tmpF60->f1;_tmpF62=(void*)
_tmpF60->f2;}}_LL781: {void*_tmpF70=_tmpF62;struct Cyc_Absyn_Vardecl*_tmpF74;
struct Cyc_Absyn_Vardecl*_tmpF76;struct Cyc_Absyn_Vardecl*_tmpF78;struct Cyc_Absyn_Vardecl*
_tmpF7A;_LL78D: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpF71=(
struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpF70;if(_tmpF71->tag != 0)
goto _LL78F;}_LL78E: goto _LL790;_LL78F: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*
_tmpF72=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpF70;if(_tmpF72->tag
!= 2)goto _LL791;}_LL790: return bogus_ans;_LL791: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmpF73=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpF70;if(_tmpF73->tag
!= 1)goto _LL793;else{_tmpF74=_tmpF73->f1;}}_LL792: {void*_tmpF7B=Cyc_Tcutil_compress((
void*)_check_null(e->topt));_LL79A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmpF7C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF7B;if(_tmpF7C->tag != 
8)goto _LL79C;}_LL79B: {struct _tuple18 _tmp1610;return(_tmp1610.f1=1,((_tmp1610.f2=(
void*)& Cyc_Absyn_HeapRgn_val,_tmp1610)));}_LL79C:;_LL79D: {struct _tuple18
_tmp1611;return(_tmp1611.f1=(_tmpF74->tq).real_const,((_tmp1611.f2=(void*)& Cyc_Absyn_HeapRgn_val,
_tmp1611)));}_LL799:;}_LL793: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmpF75=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpF70;if(_tmpF75->tag != 
4)goto _LL795;else{_tmpF76=_tmpF75->f1;}}_LL794: {void*_tmpF7F=Cyc_Tcutil_compress((
void*)_check_null(e->topt));_LL79F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmpF80=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF7F;if(_tmpF80->tag != 
8)goto _LL7A1;}_LL7A0: {struct _tuple18 _tmp1612;return(_tmp1612.f1=1,((_tmp1612.f2=(
void*)_check_null(_tmpF76->rgn),_tmp1612)));}_LL7A1:;_LL7A2: _tmpF76->escapes=1;{
struct _tuple18 _tmp1613;return(_tmp1613.f1=(_tmpF76->tq).real_const,((_tmp1613.f2=(
void*)_check_null(_tmpF76->rgn),_tmp1613)));};_LL79E:;}_LL795: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*
_tmpF77=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpF70;if(_tmpF77->tag != 5)
goto _LL797;else{_tmpF78=_tmpF77->f1;}}_LL796: _tmpF7A=_tmpF78;goto _LL798;_LL797: {
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpF79=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)
_tmpF70;if(_tmpF79->tag != 3)goto _LL78C;else{_tmpF7A=_tmpF79->f1;}}_LL798: _tmpF7A->escapes=
1;{struct _tuple18 _tmp1614;return(_tmp1614.f1=(_tmpF7A->tq).real_const,((_tmp1614.f2=(
void*)_check_null(_tmpF7A->rgn),_tmp1614)));};_LL78C:;}_LL782: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*
_tmpF63=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF5F;if(_tmpF63->tag
!= 20)goto _LL784;else{_tmpF64=_tmpF63->f1;_tmpF65=_tmpF63->f2;_tmpF66=_tmpF63->f3;}}
_LL783: if(_tmpF66)return bogus_ans;{void*_tmpF84=Cyc_Tcutil_compress((void*)
_check_null(_tmpF64->topt));struct Cyc_List_List*_tmpF86;struct Cyc_Absyn_AggrInfo
_tmpF88;union Cyc_Absyn_AggrInfoU _tmpF89;struct Cyc_Absyn_Aggrdecl**_tmpF8A;struct
Cyc_Absyn_Aggrdecl*_tmpF8B;_LL7A4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmpF85=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF84;if(_tmpF85->tag
!= 12)goto _LL7A6;else{_tmpF86=_tmpF85->f2;}}_LL7A5: {struct Cyc_Absyn_Aggrfield*
_tmpF8C=Cyc_Absyn_lookup_field(_tmpF86,_tmpF65);if(_tmpF8C != 0  && _tmpF8C->width
== 0){struct _tuple18 _tmp1615;return(_tmp1615.f1=(_tmpF8C->tq).real_const,((
_tmp1615.f2=(Cyc_Tcutil_addressof_props(te,_tmpF64)).f2,_tmp1615)));}return
bogus_ans;}_LL7A6: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF87=(struct
Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF84;if(_tmpF87->tag != 11)goto _LL7A8;
else{_tmpF88=_tmpF87->f1;_tmpF89=_tmpF88.aggr_info;if((_tmpF89.KnownAggr).tag != 
2)goto _LL7A8;_tmpF8A=(struct Cyc_Absyn_Aggrdecl**)(_tmpF89.KnownAggr).val;_tmpF8B=*
_tmpF8A;}}_LL7A7: {struct Cyc_Absyn_Aggrfield*_tmpF8E=Cyc_Absyn_lookup_decl_field(
_tmpF8B,_tmpF65);if(_tmpF8E != 0  && _tmpF8E->width == 0){struct _tuple18 _tmp1616;
return(_tmp1616.f1=(_tmpF8E->tq).real_const,((_tmp1616.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF64)).f2,_tmp1616)));}return bogus_ans;}_LL7A8:;_LL7A9: return bogus_ans;
_LL7A3:;};_LL784: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpF67=(
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF5F;if(_tmpF67->tag != 21)
goto _LL786;else{_tmpF68=_tmpF67->f1;_tmpF69=_tmpF67->f2;_tmpF6A=_tmpF67->f3;}}
_LL785: if(_tmpF6A)return bogus_ans;{void*_tmpF90=Cyc_Tcutil_compress((void*)
_check_null(_tmpF68->topt));struct Cyc_Absyn_PtrInfo _tmpF92;void*_tmpF93;struct
Cyc_Absyn_PtrAtts _tmpF94;void*_tmpF95;_LL7AB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmpF91=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF90;if(_tmpF91->tag
!= 5)goto _LL7AD;else{_tmpF92=_tmpF91->f1;_tmpF93=_tmpF92.elt_typ;_tmpF94=_tmpF92.ptr_atts;
_tmpF95=_tmpF94.rgn;}}_LL7AC: {struct Cyc_Absyn_Aggrfield*finfo;{void*_tmpF96=Cyc_Tcutil_compress(
_tmpF93);struct Cyc_List_List*_tmpF98;struct Cyc_Absyn_AggrInfo _tmpF9A;union Cyc_Absyn_AggrInfoU
_tmpF9B;struct Cyc_Absyn_Aggrdecl**_tmpF9C;struct Cyc_Absyn_Aggrdecl*_tmpF9D;
_LL7B0: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF97=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)
_tmpF96;if(_tmpF97->tag != 12)goto _LL7B2;else{_tmpF98=_tmpF97->f2;}}_LL7B1: finfo=
Cyc_Absyn_lookup_field(_tmpF98,_tmpF69);goto _LL7AF;_LL7B2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmpF99=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF96;if(_tmpF99->tag != 
11)goto _LL7B4;else{_tmpF9A=_tmpF99->f1;_tmpF9B=_tmpF9A.aggr_info;if((_tmpF9B.KnownAggr).tag
!= 2)goto _LL7B4;_tmpF9C=(struct Cyc_Absyn_Aggrdecl**)(_tmpF9B.KnownAggr).val;
_tmpF9D=*_tmpF9C;}}_LL7B3: finfo=Cyc_Absyn_lookup_decl_field(_tmpF9D,_tmpF69);
goto _LL7AF;_LL7B4:;_LL7B5: return bogus_ans;_LL7AF:;}if(finfo != 0  && finfo->width
== 0){struct _tuple18 _tmp1617;return(_tmp1617.f1=(finfo->tq).real_const,((
_tmp1617.f2=_tmpF95,_tmp1617)));}return bogus_ans;}_LL7AD:;_LL7AE: return bogus_ans;
_LL7AA:;};_LL786: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpF6B=(struct
Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpF5F;if(_tmpF6B->tag != 19)goto _LL788;
else{_tmpF6C=_tmpF6B->f1;}}_LL787: {void*_tmpF9F=Cyc_Tcutil_compress((void*)
_check_null(_tmpF6C->topt));struct Cyc_Absyn_PtrInfo _tmpFA1;struct Cyc_Absyn_Tqual
_tmpFA2;struct Cyc_Absyn_PtrAtts _tmpFA3;void*_tmpFA4;_LL7B7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmpFA0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF9F;if(_tmpFA0->tag
!= 5)goto _LL7B9;else{_tmpFA1=_tmpFA0->f1;_tmpFA2=_tmpFA1.elt_tq;_tmpFA3=_tmpFA1.ptr_atts;
_tmpFA4=_tmpFA3.rgn;}}_LL7B8: {struct _tuple18 _tmp1618;return(_tmp1618.f1=_tmpFA2.real_const,((
_tmp1618.f2=_tmpFA4,_tmp1618)));}_LL7B9:;_LL7BA: return bogus_ans;_LL7B6:;}_LL788: {
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpF6D=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)
_tmpF5F;if(_tmpF6D->tag != 22)goto _LL78A;else{_tmpF6E=_tmpF6D->f1;_tmpF6F=_tmpF6D->f2;}}
_LL789: {void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpF6E->topt));void*
_tmpFA6=t;struct Cyc_List_List*_tmpFA8;struct Cyc_Absyn_PtrInfo _tmpFAA;struct Cyc_Absyn_Tqual
_tmpFAB;struct Cyc_Absyn_PtrAtts _tmpFAC;void*_tmpFAD;struct Cyc_Absyn_ArrayInfo
_tmpFAF;struct Cyc_Absyn_Tqual _tmpFB0;_LL7BC: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmpFA7=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpFA6;if(_tmpFA7->tag != 
10)goto _LL7BE;else{_tmpFA8=_tmpFA7->f1;}}_LL7BD: {unsigned int _tmpFB2;int _tmpFB3;
struct _tuple14 _tmpFB1=Cyc_Evexp_eval_const_uint_exp(_tmpF6F);_tmpFB2=_tmpFB1.f1;
_tmpFB3=_tmpFB1.f2;if(!_tmpFB3)return bogus_ans;{struct _tuple12*_tmpFB4=Cyc_Absyn_lookup_tuple_field(
_tmpFA8,(int)_tmpFB2);if(_tmpFB4 != 0){struct _tuple18 _tmp1619;return(_tmp1619.f1=((*
_tmpFB4).f1).real_const,((_tmp1619.f2=(Cyc_Tcutil_addressof_props(te,_tmpF6E)).f2,
_tmp1619)));}return bogus_ans;};}_LL7BE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmpFA9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpFA6;if(_tmpFA9->tag
!= 5)goto _LL7C0;else{_tmpFAA=_tmpFA9->f1;_tmpFAB=_tmpFAA.elt_tq;_tmpFAC=_tmpFAA.ptr_atts;
_tmpFAD=_tmpFAC.rgn;}}_LL7BF: {struct _tuple18 _tmp161A;return(_tmp161A.f1=_tmpFAB.real_const,((
_tmp161A.f2=_tmpFAD,_tmp161A)));}_LL7C0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmpFAE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFA6;if(_tmpFAE->tag != 
8)goto _LL7C2;else{_tmpFAF=_tmpFAE->f1;_tmpFB0=_tmpFAF.tq;}}_LL7C1: {struct
_tuple18 _tmp161B;return(_tmp161B.f1=_tmpFB0.real_const,((_tmp161B.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF6E)).f2,_tmp161B)));}_LL7C2:;_LL7C3: return bogus_ans;_LL7BB:;}_LL78A:;
_LL78B:{const char*_tmp161E;void*_tmp161D;(_tmp161D=0,Cyc_Tcutil_terr(e->loc,((
_tmp161E="unary & applied to non-lvalue",_tag_dyneither(_tmp161E,sizeof(char),30))),
_tag_dyneither(_tmp161D,sizeof(void*),0)));}return bogus_ans;_LL77F:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpFBB=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpFBD;void*_tmpFBE;struct Cyc_Absyn_Tqual
_tmpFBF;union Cyc_Absyn_Constraint*_tmpFC0;_LL7C5: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmpFBC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFBB;if(_tmpFBC->tag != 
8)goto _LL7C7;else{_tmpFBD=_tmpFBC->f1;_tmpFBE=_tmpFBD.elt_type;_tmpFBF=_tmpFBD.tq;
_tmpFC0=_tmpFBD.zero_term;}}_LL7C6: {void*_tmpFC2;struct _tuple18 _tmpFC1=Cyc_Tcutil_addressof_props(
te,e);_tmpFC2=_tmpFC1.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1621;
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1620;return Cyc_Absyn_atb_typ(
_tmpFBE,_tmpFC2,_tmpFBF,(void*)((_tmp1620=_cycalloc(sizeof(*_tmp1620)),((
_tmp1620[0]=((_tmp1621.tag=1,((_tmp1621.f1=e,_tmp1621)))),_tmp1620)))),_tmpFC0);};}
_LL7C7:;_LL7C8: return e_typ;_LL7C4:;}void Cyc_Tcutil_check_bound(unsigned int loc,
unsigned int i,union Cyc_Absyn_Constraint*b){b=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{void*_tmpFC5=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,
b);struct Cyc_Absyn_Exp*_tmpFC8;_LL7CA: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmpFC6=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpFC5;if(_tmpFC6->tag
!= 0)goto _LL7CC;}_LL7CB: return;_LL7CC: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmpFC7=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpFC5;if(_tmpFC7->tag != 
1)goto _LL7C9;else{_tmpFC8=_tmpFC7->f1;}}_LL7CD: {unsigned int _tmpFCA;int _tmpFCB;
struct _tuple14 _tmpFC9=Cyc_Evexp_eval_const_uint_exp(_tmpFC8);_tmpFCA=_tmpFC9.f1;
_tmpFCB=_tmpFC9.f2;if(_tmpFCB  && _tmpFCA <= i){const char*_tmp1626;void*_tmp1625[2];
struct Cyc_Int_pa_PrintArg_struct _tmp1624;struct Cyc_Int_pa_PrintArg_struct
_tmp1623;(_tmp1623.tag=1,((_tmp1623.f1=(unsigned long)((int)i),((_tmp1624.tag=1,((
_tmp1624.f1=(unsigned long)((int)_tmpFCA),((_tmp1625[0]=& _tmp1624,((_tmp1625[1]=&
_tmp1623,Cyc_Tcutil_terr(loc,((_tmp1626="dereference is out of bounds: %d <= %d",
_tag_dyneither(_tmp1626,sizeof(char),39))),_tag_dyneither(_tmp1625,sizeof(void*),
2)))))))))))));}return;}_LL7C9:;};}void Cyc_Tcutil_check_nonzero_bound(
unsigned int loc,union Cyc_Absyn_Constraint*b){Cyc_Tcutil_check_bound(loc,0,b);}
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){void*_tmpFD0=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
b);struct Cyc_Absyn_Exp*_tmpFD2;_LL7CF: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmpFD1=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpFD0;if(_tmpFD1->tag != 
1)goto _LL7D1;else{_tmpFD2=_tmpFD1->f1;}}_LL7D0: {unsigned int _tmpFD4;int _tmpFD5;
struct _tuple14 _tmpFD3=Cyc_Evexp_eval_const_uint_exp(_tmpFD2);_tmpFD4=_tmpFD3.f1;
_tmpFD5=_tmpFD3.f2;return _tmpFD5  && _tmpFD4 == 1;}_LL7D1:;_LL7D2: return 0;_LL7CE:;}
int Cyc_Tcutil_bits_only(void*t){void*_tmpFD6=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo
_tmpFDD;void*_tmpFDE;union Cyc_Absyn_Constraint*_tmpFDF;struct Cyc_List_List*
_tmpFE1;struct Cyc_Absyn_AggrInfo _tmpFE3;union Cyc_Absyn_AggrInfoU _tmpFE4;struct
_tuple4 _tmpFE5;struct Cyc_Absyn_AggrInfo _tmpFE7;union Cyc_Absyn_AggrInfoU _tmpFE8;
struct Cyc_Absyn_Aggrdecl**_tmpFE9;struct Cyc_Absyn_Aggrdecl*_tmpFEA;struct Cyc_List_List*
_tmpFEB;struct Cyc_List_List*_tmpFED;_LL7D4: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*
_tmpFD7=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpFD6;if(_tmpFD7->tag != 0)
goto _LL7D6;}_LL7D5: goto _LL7D7;_LL7D6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmpFD8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpFD6;if(_tmpFD8->tag != 6)
goto _LL7D8;}_LL7D7: goto _LL7D9;_LL7D8: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmpFD9=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpFD6;if(_tmpFD9->tag != 
7)goto _LL7DA;}_LL7D9: return 1;_LL7DA: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmpFDA=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpFD6;if(_tmpFDA->tag != 
13)goto _LL7DC;}_LL7DB: goto _LL7DD;_LL7DC: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*
_tmpFDB=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpFD6;if(_tmpFDB->tag
!= 14)goto _LL7DE;}_LL7DD: return 0;_LL7DE: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmpFDC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFD6;if(_tmpFDC->tag != 
8)goto _LL7E0;else{_tmpFDD=_tmpFDC->f1;_tmpFDE=_tmpFDD.elt_type;_tmpFDF=_tmpFDD.zero_term;}}
_LL7DF: return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpFDF) && Cyc_Tcutil_bits_only(_tmpFDE);_LL7E0: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmpFE0=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpFD6;if(_tmpFE0->tag != 
10)goto _LL7E2;else{_tmpFE1=_tmpFE0->f1;}}_LL7E1: for(0;_tmpFE1 != 0;_tmpFE1=
_tmpFE1->tl){if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpFE1->hd)).f2))
return 0;}return 1;_LL7E2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpFE2=(
struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpFD6;if(_tmpFE2->tag != 11)goto
_LL7E4;else{_tmpFE3=_tmpFE2->f1;_tmpFE4=_tmpFE3.aggr_info;if((_tmpFE4.UnknownAggr).tag
!= 1)goto _LL7E4;_tmpFE5=(struct _tuple4)(_tmpFE4.UnknownAggr).val;}}_LL7E3: return
0;_LL7E4: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpFE6=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmpFD6;if(_tmpFE6->tag != 11)goto _LL7E6;else{_tmpFE7=_tmpFE6->f1;_tmpFE8=_tmpFE7.aggr_info;
if((_tmpFE8.KnownAggr).tag != 2)goto _LL7E6;_tmpFE9=(struct Cyc_Absyn_Aggrdecl**)(
_tmpFE8.KnownAggr).val;_tmpFEA=*_tmpFE9;_tmpFEB=_tmpFE7.targs;}}_LL7E5: if(
_tmpFEA->impl == 0)return 0;{int okay=1;{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpFEA->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(((
struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}if(okay)return 1;{
struct _RegionHandle _tmpFEE=_new_region("rgn");struct _RegionHandle*rgn=& _tmpFEE;
_push_region(rgn);{struct Cyc_List_List*_tmpFEF=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpFEA->tvs,_tmpFEB);{struct Cyc_List_List*fs=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFEA->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpFEF,((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type))){int _tmpFF0=0;_npop_handler(0);return _tmpFF0;}}}{int _tmpFF1=1;
_npop_handler(0);return _tmpFF1;};};_pop_region(rgn);};};_LL7E6: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmpFEC=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpFD6;if(_tmpFEC->tag
!= 12)goto _LL7E8;else{_tmpFED=_tmpFEC->f2;}}_LL7E7: for(0;_tmpFED != 0;_tmpFED=
_tmpFED->tl){if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpFED->hd)->type))
return 0;}return 1;_LL7E8:;_LL7E9: return 0;_LL7D3:;}static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmpFF2=e->r;
struct _tuple2*_tmpFFA;void*_tmpFFB;struct Cyc_Absyn_Exp*_tmpFFD;struct Cyc_Absyn_Exp*
_tmpFFE;struct Cyc_Absyn_Exp*_tmpFFF;struct Cyc_Absyn_Exp*_tmp1001;struct Cyc_Absyn_Exp*
_tmp1002;struct Cyc_Absyn_Exp*_tmp1004;struct Cyc_Absyn_Exp*_tmp1006;void*_tmp1008;
struct Cyc_Absyn_Exp*_tmp1009;enum Cyc_Absyn_Coercion _tmp100A;void*_tmp100C;
struct Cyc_Absyn_Exp*_tmp100D;struct Cyc_Absyn_Exp*_tmp100F;struct Cyc_Absyn_Exp*
_tmp1011;struct Cyc_Absyn_Exp*_tmp1012;struct Cyc_List_List*_tmp1014;struct Cyc_List_List*
_tmp1016;struct Cyc_List_List*_tmp1018;enum Cyc_Absyn_Primop _tmp101A;struct Cyc_List_List*
_tmp101B;struct Cyc_List_List*_tmp101D;struct Cyc_List_List*_tmp101F;_LL7EB: {
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFF3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmpFF2;if(_tmpFF3->tag != 0)goto _LL7ED;}_LL7EC: goto _LL7EE;_LL7ED: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*
_tmpFF4=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpFF2;if(_tmpFF4->tag
!= 16)goto _LL7EF;}_LL7EE: goto _LL7F0;_LL7EF: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*
_tmpFF5=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpFF2;if(_tmpFF5->tag
!= 17)goto _LL7F1;}_LL7F0: goto _LL7F2;_LL7F1: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*
_tmpFF6=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpFF2;if(_tmpFF6->tag
!= 18)goto _LL7F3;}_LL7F2: goto _LL7F4;_LL7F3: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*
_tmpFF7=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpFF2;if(_tmpFF7->tag != 
30)goto _LL7F5;}_LL7F4: goto _LL7F6;_LL7F5: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*
_tmpFF8=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpFF2;if(_tmpFF8->tag
!= 31)goto _LL7F7;}_LL7F6: return 1;_LL7F7: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmpFF9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpFF2;if(_tmpFF9->tag != 1)
goto _LL7F9;else{_tmpFFA=_tmpFF9->f1;_tmpFFB=(void*)_tmpFF9->f2;}}_LL7F8: {void*
_tmp1020=_tmpFFB;struct Cyc_Absyn_Vardecl*_tmp1023;_LL818: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*
_tmp1021=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp1020;if(_tmp1021->tag
!= 2)goto _LL81A;}_LL819: return 1;_LL81A: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp1022=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp1020;if(_tmp1022->tag
!= 1)goto _LL81C;else{_tmp1023=_tmp1022->f1;}}_LL81B: {void*_tmp1025=Cyc_Tcutil_compress(
_tmp1023->type);_LL821: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1026=(
struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1025;if(_tmp1026->tag != 8)goto
_LL823;}_LL822: goto _LL824;_LL823: {struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp1027=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1025;if(_tmp1027->tag != 
9)goto _LL825;}_LL824: return 1;_LL825:;_LL826: return var_okay;_LL820:;}_LL81C: {
struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp1024=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)
_tmp1020;if(_tmp1024->tag != 0)goto _LL81E;}_LL81D: return 0;_LL81E:;_LL81F: return
var_okay;_LL817:;}_LL7F9: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*
_tmpFFC=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpFF2;if(_tmpFFC->tag
!= 5)goto _LL7FB;else{_tmpFFD=_tmpFFC->f1;_tmpFFE=_tmpFFC->f2;_tmpFFF=_tmpFFC->f3;}}
_LL7FA: return(Cyc_Tcutil_cnst_exp(te,0,_tmpFFD) && Cyc_Tcutil_cnst_exp(te,0,
_tmpFFE)) && Cyc_Tcutil_cnst_exp(te,0,_tmpFFF);_LL7FB: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*
_tmp1000=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpFF2;if(_tmp1000->tag
!= 8)goto _LL7FD;else{_tmp1001=_tmp1000->f1;_tmp1002=_tmp1000->f2;}}_LL7FC: return
Cyc_Tcutil_cnst_exp(te,0,_tmp1001) && Cyc_Tcutil_cnst_exp(te,0,_tmp1002);_LL7FD: {
struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp1003=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)
_tmpFF2;if(_tmp1003->tag != 11)goto _LL7FF;else{_tmp1004=_tmp1003->f1;}}_LL7FE:
_tmp1006=_tmp1004;goto _LL800;_LL7FF: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*
_tmp1005=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpFF2;if(
_tmp1005->tag != 12)goto _LL801;else{_tmp1006=_tmp1005->f1;}}_LL800: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp1006);_LL801: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*
_tmp1007=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpFF2;if(_tmp1007->tag
!= 13)goto _LL803;else{_tmp1008=(void*)_tmp1007->f1;_tmp1009=_tmp1007->f2;
_tmp100A=_tmp1007->f4;if(_tmp100A != Cyc_Absyn_No_coercion)goto _LL803;}}_LL802:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp1009);_LL803: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*
_tmp100B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpFF2;if(_tmp100B->tag
!= 13)goto _LL805;else{_tmp100C=(void*)_tmp100B->f1;_tmp100D=_tmp100B->f2;}}
_LL804: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp100D);_LL805: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*
_tmp100E=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpFF2;if(_tmp100E->tag
!= 14)goto _LL807;else{_tmp100F=_tmp100E->f1;}}_LL806: return Cyc_Tcutil_cnst_exp(
te,1,_tmp100F);_LL807: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*
_tmp1010=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpFF2;if(
_tmp1010->tag != 26)goto _LL809;else{_tmp1011=_tmp1010->f2;_tmp1012=_tmp1010->f3;}}
_LL808: return Cyc_Tcutil_cnst_exp(te,0,_tmp1011) && Cyc_Tcutil_cnst_exp(te,0,
_tmp1012);_LL809: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1013=(struct
Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpFF2;if(_tmp1013->tag != 25)goto _LL80B;
else{_tmp1014=_tmp1013->f1;}}_LL80A: _tmp1016=_tmp1014;goto _LL80C;_LL80B: {struct
Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1015=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)
_tmpFF2;if(_tmp1015->tag != 28)goto _LL80D;else{_tmp1016=_tmp1015->f2;}}_LL80C:
_tmp1018=_tmp1016;goto _LL80E;_LL80D: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*
_tmp1017=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpFF2;if(_tmp1017->tag
!= 27)goto _LL80F;else{_tmp1018=_tmp1017->f3;}}_LL80E: for(0;_tmp1018 != 0;_tmp1018=
_tmp1018->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple19*)_tmp1018->hd)).f2))
return 0;}return 1;_LL80F: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1019=(
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpFF2;if(_tmp1019->tag != 2)goto
_LL811;else{_tmp101A=_tmp1019->f1;_tmp101B=_tmp1019->f2;}}_LL810: _tmp101D=
_tmp101B;goto _LL812;_LL811: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*
_tmp101C=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpFF2;if(_tmp101C->tag
!= 23)goto _LL813;else{_tmp101D=_tmp101C->f1;}}_LL812: _tmp101F=_tmp101D;goto
_LL814;_LL813: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp101E=(struct
Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpFF2;if(_tmp101E->tag != 29)goto
_LL815;else{_tmp101F=_tmp101E->f1;}}_LL814: for(0;_tmp101F != 0;_tmp101F=_tmp101F->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmp101F->hd))return 0;}return
1;_LL815:;_LL816: return 0;_LL7EA:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp1028=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp102D;
struct Cyc_Absyn_PtrAtts _tmp102E;union Cyc_Absyn_Constraint*_tmp102F;union Cyc_Absyn_Constraint*
_tmp1030;struct Cyc_Absyn_ArrayInfo _tmp1032;void*_tmp1033;struct Cyc_List_List*
_tmp1035;struct Cyc_Absyn_AggrInfo _tmp1037;union Cyc_Absyn_AggrInfoU _tmp1038;
struct Cyc_List_List*_tmp1039;struct Cyc_List_List*_tmp103B;_LL828: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*
_tmp1029=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1028;if(_tmp1029->tag
!= 0)goto _LL82A;}_LL829: goto _LL82B;_LL82A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp102A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1028;if(_tmp102A->tag != 
6)goto _LL82C;}_LL82B: goto _LL82D;_LL82C: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp102B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1028;if(_tmp102B->tag
!= 7)goto _LL82E;}_LL82D: return 1;_LL82E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp102C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1028;if(_tmp102C->tag
!= 5)goto _LL830;else{_tmp102D=_tmp102C->f1;_tmp102E=_tmp102D.ptr_atts;_tmp102F=
_tmp102E.nullable;_tmp1030=_tmp102E.bounds;}}_LL82F: {void*_tmp103E=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp1030);_LL83F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp103F=(
struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp103E;if(_tmp103F->tag != 0)
goto _LL841;}_LL840: return 1;_LL841:;_LL842: return((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(1,_tmp102F);_LL83E:;}_LL830: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp1031=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1028;if(_tmp1031->tag
!= 8)goto _LL832;else{_tmp1032=_tmp1031->f1;_tmp1033=_tmp1032.elt_type;}}_LL831:
return Cyc_Tcutil_supports_default(_tmp1033);_LL832: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmp1034=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1028;if(_tmp1034->tag
!= 10)goto _LL834;else{_tmp1035=_tmp1034->f1;}}_LL833: for(0;_tmp1035 != 0;_tmp1035=
_tmp1035->tl){if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmp1035->hd)).f2))
return 0;}return 1;_LL834: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1036=(
struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1028;if(_tmp1036->tag != 11)goto
_LL836;else{_tmp1037=_tmp1036->f1;_tmp1038=_tmp1037.aggr_info;_tmp1039=_tmp1037.targs;}}
_LL835: {struct Cyc_Absyn_Aggrdecl*_tmp1040=Cyc_Absyn_get_known_aggrdecl(_tmp1038);
if(_tmp1040->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp1040->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmp1040->tvs,_tmp1039,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1040->impl))->fields);}
_LL836: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp103A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)
_tmp1028;if(_tmp103A->tag != 12)goto _LL838;else{_tmp103B=_tmp103A->f2;}}_LL837:
return Cyc_Tcutil_fields_support_default(0,0,_tmp103B);_LL838: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*
_tmp103C=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1028;if(_tmp103C->tag
!= 14)goto _LL83A;}_LL839: goto _LL83B;_LL83A: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp103D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1028;if(_tmp103D->tag
!= 13)goto _LL83C;}_LL83B: return 1;_LL83C:;_LL83D: return 0;_LL827:;}void Cyc_Tcutil_check_no_qual(
unsigned int loc,void*t){void*_tmp1041=t;struct Cyc_Absyn_Typedefdecl*_tmp1043;
_LL844: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1042=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)
_tmp1041;if(_tmp1042->tag != 17)goto _LL846;else{_tmp1043=_tmp1042->f3;}}_LL845:
if(_tmp1043 != 0){struct Cyc_Absyn_Tqual _tmp1044=_tmp1043->tq;if(((_tmp1044.print_const
 || _tmp1044.q_volatile) || _tmp1044.q_restrict) || _tmp1044.real_const){const
char*_tmp162A;void*_tmp1629[1];struct Cyc_String_pa_PrintArg_struct _tmp1628;(
_tmp1628.tag=0,((_tmp1628.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp1629[0]=& _tmp1628,Cyc_Tcutil_warn(loc,((_tmp162A="qualifier within typedef type %s is ignored",
_tag_dyneither(_tmp162A,sizeof(char),44))),_tag_dyneither(_tmp1629,sizeof(void*),
1)))))));}}goto _LL843;_LL846:;_LL847: goto _LL843;_LL843:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){struct
_RegionHandle _tmp1048=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1048;
_push_region(rgn);{struct Cyc_List_List*_tmp1049=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type;if(Cyc_Tcutil_supports_default(t)){int _tmp104A=1;_npop_handler(0);
return _tmp104A;}t=Cyc_Tcutil_rsubstitute(rgn,_tmp1049,((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type);if(!Cyc_Tcutil_supports_default(t)){int _tmp104B=0;_npop_handler(0);
return _tmp104B;}}}{int _tmp104C=1;_npop_handler(0);return _tmp104C;};;_pop_region(
rgn);}int Cyc_Tcutil_admits_zero(void*t){void*_tmp104D=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp1051;struct Cyc_Absyn_PtrAtts _tmp1052;union Cyc_Absyn_Constraint*
_tmp1053;union Cyc_Absyn_Constraint*_tmp1054;_LL849: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp104E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp104D;if(_tmp104E->tag != 
6)goto _LL84B;}_LL84A: goto _LL84C;_LL84B: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp104F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp104D;if(_tmp104F->tag
!= 7)goto _LL84D;}_LL84C: return 1;_LL84D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp1050=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp104D;if(_tmp1050->tag
!= 5)goto _LL84F;else{_tmp1051=_tmp1050->f1;_tmp1052=_tmp1051.ptr_atts;_tmp1053=
_tmp1052.nullable;_tmp1054=_tmp1052.bounds;}}_LL84E: {void*_tmp1055=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp1054);_LL852: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp1056=(
struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1055;if(_tmp1056->tag != 0)
goto _LL854;}_LL853: return 0;_LL854:;_LL855: return((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp1053);_LL851:;}_LL84F:;_LL850: return 0;_LL848:;}
int Cyc_Tcutil_is_noreturn(void*t){{void*_tmp1057=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmp1059;void*_tmp105A;struct Cyc_Absyn_FnInfo _tmp105C;struct Cyc_List_List*
_tmp105D;_LL857: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1058=(struct
Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1057;if(_tmp1058->tag != 5)goto
_LL859;else{_tmp1059=_tmp1058->f1;_tmp105A=_tmp1059.elt_typ;}}_LL858: return Cyc_Tcutil_is_noreturn(
_tmp105A);_LL859: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp105B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)
_tmp1057;if(_tmp105B->tag != 9)goto _LL85B;else{_tmp105C=_tmp105B->f1;_tmp105D=
_tmp105C.attributes;}}_LL85A: for(0;_tmp105D != 0;_tmp105D=_tmp105D->tl){void*
_tmp105E=(void*)_tmp105D->hd;_LL85E: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*
_tmp105F=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp105E;if(
_tmp105F->tag != 4)goto _LL860;}_LL85F: return 1;_LL860:;_LL861: continue;_LL85D:;}
goto _LL856;_LL85B:;_LL85C: goto _LL856;_LL856:;}return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts){void*_tmp1060=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmp1062;struct Cyc_List_List*_tmp1063;struct Cyc_List_List**_tmp1064;_LL863: {
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1061=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)
_tmp1060;if(_tmp1061->tag != 9)goto _LL865;else{_tmp1062=_tmp1061->f1;_tmp1063=
_tmp1062.attributes;_tmp1064=(struct Cyc_List_List**)&(_tmp1061->f1).attributes;}}
_LL864: {struct Cyc_List_List*_tmp1065=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((
void*)atts->hd)){struct Cyc_List_List*_tmp162B;*_tmp1064=((_tmp162B=_cycalloc(
sizeof(*_tmp162B)),((_tmp162B->hd=(void*)atts->hd,((_tmp162B->tl=*_tmp1064,
_tmp162B))))));}else{struct Cyc_List_List*_tmp162C;_tmp1065=((_tmp162C=_cycalloc(
sizeof(*_tmp162C)),((_tmp162C->hd=(void*)atts->hd,((_tmp162C->tl=_tmp1065,
_tmp162C))))));}}return _tmp1065;}_LL865:;_LL866: {const char*_tmp162F;void*
_tmp162E;(_tmp162E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp162F="transfer_fn_type_atts",_tag_dyneither(_tmp162F,
sizeof(char),22))),_tag_dyneither(_tmp162E,sizeof(void*),0)));}_LL862:;}
