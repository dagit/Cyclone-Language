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
int tag;struct _dyneither_ptr f1;};typedef void*Cyc_sarg_t;int Cyc_vfprintf(struct Cyc___cycFILE*,
struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];
struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];
struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};typedef
unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_NewRegion Cyc_Core__new_dynregion(
const char*file,int lineno);extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{
char*tag;};void Cyc_Core_free_dynregion(struct _DynRegionHandle*);inline static void* 
arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){if(bd >> 20  || sz
>> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < dyn.curr  || dyn.curr
== 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)return 0;return dyn.curr;};}
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};typedef struct Cyc_List_List*
Cyc_List_list_t;typedef struct Cyc_List_List*Cyc_List_List_t;struct Cyc_List_List*
Cyc_List_list(struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{
char*tag;};void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void Cyc_List_iter_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{
char*tag;};void*Cyc_List_nth(struct Cyc_List_List*x,int n);int Cyc_List_forall(int(*
pred)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rzip(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);struct _dyneither_ptr Cyc_strcpy(struct
_dyneither_ptr dest,struct _dyneither_ptr src);struct Cyc_Iter_Iter{void*env;int(*
next)(void*env,void*dest);};typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;typedef struct Cyc_Set_Set*Cyc_Set_set_t;
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*
elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];
struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;typedef const struct
Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;const struct Cyc_Dict_T*t;};typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;
extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct
Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));struct
Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*,struct Cyc_Dict_Dict);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;
int line_no;int col;};typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[
5];struct Cyc_Position_Exit_exn_struct{char*tag;};typedef unsigned int Cyc_Position_seg_t;
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Lex_Position_Error_kind_struct{
int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{
int tag;};typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct
_dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[
10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};struct Cyc_Xarray_Xarray{
struct _RegionHandle*r;struct _dyneither_ptr elmts;int num_elmts;};typedef struct Cyc_Xarray_Xarray*
Cyc_Xarray_xarray_t;int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);void*Cyc_Xarray_get(
struct Cyc_Xarray_Xarray*,int);struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(
struct _RegionHandle*);int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
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
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;
struct _tuple0*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};
struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct
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
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;
struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;
unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*
f5;};typedef void*Cyc_Absyn_funcparams_t;enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft
 = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{
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
_union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};union Cyc_Absyn_Cnst
Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(
enum Cyc_Absyn_Sign,short);union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,
int);union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);enum Cyc_Absyn_Primop{
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
struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;
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
Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};
struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int
loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct
Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;
void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;
};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope
sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*
name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{
enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*
tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct
_tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{
void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{
int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{
char*tag;};extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);void*Cyc_Absyn_conref_val(
union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*
x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_false_conref;extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_sint_typ;void*Cyc_Absyn_exn_typ();extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple0*name);void*Cyc_Absyn_unionq_typ(struct
_tuple0*name);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
union Cyc_Absyn_Cnst,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_char_exp(char c,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dyneither_ptr f,int i,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct
_tuple0*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,
void*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum 
Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct
Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct
Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,
struct Cyc_List_List*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,
struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);struct
Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(
void*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,
struct Cyc_List_List*,unsigned int);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*
s,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,
unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*
e,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(
struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct _dyneither_ptr*Cyc_Absyn_fieldname(int);struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);extern int Cyc_Absyn_no_regions;
struct Cyc_PP_Ppstate;typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int
add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;
int print_all_kinds;int print_all_effects;int print_using_stmts;int
print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple0*);struct Cyc_RgnOrder_RgnPO;typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct
Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn,unsigned int);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
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
struct _dyneither_ptr ap);void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*
k2);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*
def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(
void*t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(void*t);extern
struct Cyc_Absyn_Kind Cyc_Tcutil_ak;extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;extern
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dyneither,void**elt_type);struct _tuple10{struct Cyc_Absyn_Tqual f1;
void*f2;};void*Cyc_Tcutil_snd_tqt(struct _tuple10*);void Cyc_Tcutil_check_no_qual(
unsigned int loc,void*t);struct _tuple11{unsigned int f1;int f2;};struct _tuple11 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{
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
struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp
rop;};typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;typedef struct Cyc_List_List*
Cyc_CfFlowInfo_relns_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{
char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{
char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*
val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{
struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;typedef void*Cyc_CfFlowInfo_absRval_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;typedef struct _dyneither_ptr
Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int
fieldnum;};typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;
struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};
struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel
f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel
f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo
f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{
int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple13{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple13
val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct
_union_FlowInfo_ReachableFL ReachableFL;};typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;
void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*
esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict
mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};typedef struct Cyc_CfFlowInfo_FlowEnv*
Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{
int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};
struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};typedef void*Cyc_CfFlowInfo_killrgn_t;struct Cyc_List_List*Cyc_Toc_toc(struct
Cyc_List_List*ds);struct _tuple0*Cyc_Toc_temp_var();extern struct _dyneither_ptr Cyc_Toc_globals;
extern int Cyc_noexpand_r;int Cyc_Toc_warn_bounds_checks=0;int Cyc_Toc_warn_all_null_deref=
0;unsigned int Cyc_Toc_total_bounds_checks=0;unsigned int Cyc_Toc_bounds_checks_eliminated=
0;static struct Cyc_List_List*Cyc_Toc_result_decls=0;struct Cyc_Toc_TocState{struct
_DynRegionHandle*dyn;struct Cyc_List_List**tuple_types;struct Cyc_Dict_Dict*
aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**
datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*
qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=
0;struct _tuple14{struct _tuple0*f1;struct _tuple0*f2;};int Cyc_Toc_qvar_tag_cmp(
struct _tuple14*x,struct _tuple14*y){struct _tuple0*_tmp1;struct _tuple0*_tmp2;
struct _tuple14 _tmp0=*x;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;{struct _tuple0*_tmp4;struct
_tuple0*_tmp5;struct _tuple14 _tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{int i=Cyc_Absyn_qvar_cmp(
_tmp1,_tmp4);if(i != 0)return i;return Cyc_Absyn_qvar_cmp(_tmp2,_tmp5);};};}struct
_tuple15{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};void*Cyc_Toc_aggrdecl_type(struct
_tuple0*q,void*(*type_maker)(struct _tuple0*)){struct _DynRegionHandle*_tmp7;
struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState _tmp6=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp7=_tmp6.dyn;_tmp8=_tmp6.aggrs_so_far;{struct
_DynRegionFrame _tmp9;struct _RegionHandle*d=_open_dynregion(& _tmp9,_tmp7);{struct
_tuple15**v=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*
_tmp8,q);if(v == 0){void*_tmpA=type_maker(q);_npop_handler(0);return _tmpA;}else{
struct _tuple15 _tmpC;void*_tmpD;struct _tuple15*_tmpB=*v;_tmpC=*_tmpB;_tmpD=_tmpC.f2;{
void*_tmpE=_tmpD;_npop_handler(0);return _tmpE;};}};_pop_dynregion(d);};}static
int Cyc_Toc_tuple_type_counter=0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=
0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_exn_struct{
char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_exn_struct{
char*tag;};static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpA6B;void*_tmpA6A;(_tmpA6A=0,
Cyc_fprintf(Cyc_stderr,((_tmpA6B="\n",_tag_dyneither(_tmpA6B,sizeof(char),2))),
_tag_dyneither(_tmpA6A,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmpA6E;struct Cyc_Toc_Toc_Unimplemented_exn_struct*
_tmpA6D;(int)_throw((void*)((_tmpA6D=_cycalloc_atomic(sizeof(*_tmpA6D)),((
_tmpA6D[0]=((_tmpA6E.tag=Cyc_Toc_Toc_Unimplemented,_tmpA6E)),_tmpA6D)))));};}
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);{const char*_tmpA71;void*_tmpA70;(_tmpA70=0,Cyc_fprintf(Cyc_stderr,((
_tmpA71="\n",_tag_dyneither(_tmpA71,sizeof(char),2))),_tag_dyneither(_tmpA70,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_exn_struct
_tmpA74;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmpA73;(int)_throw((void*)((
_tmpA73=_cycalloc_atomic(sizeof(*_tmpA73)),((_tmpA73[0]=((_tmpA74.tag=Cyc_Toc_Toc_Impossible,
_tmpA74)),_tmpA73)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{
char*tag;};static char _tmp17[5]="curr";static struct _dyneither_ptr Cyc_Toc_curr_string={
_tmp17,_tmp17,_tmp17 + 5};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static char _tmp18[4]="tag";static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp18,
_tmp18,_tmp18 + 4};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;
static char _tmp19[4]="val";static struct _dyneither_ptr Cyc_Toc_val_string={_tmp19,
_tmp19,_tmp19 + 4};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;
static char _tmp1A[14]="_handler_cons";static struct _dyneither_ptr Cyc_Toc__handler_cons_string={
_tmp1A,_tmp1A,_tmp1A + 14};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string;static char _tmp1B[8]="handler";static struct
_dyneither_ptr Cyc_Toc_handler_string={_tmp1B,_tmp1B,_tmp1B + 8};static struct
_dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp1C[14]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp1C,_tmp1C,_tmp1C + 14};
static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static char _tmp1D[17]="_DynRegionHandle";static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={
_tmp1D,_tmp1D,_tmp1D + 17};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=&
Cyc_Toc__DynRegionHandle_string;static char _tmp1E[16]="_DynRegionFrame";static
struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp1E,_tmp1E,_tmp1E + 16};
static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};static char
_tmp1F[7]="_throw";static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp1F,_tmp1F,
_tmp1F + 7};static struct _tuple0 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={
0,(void*)& Cyc_Toc__throw_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*
Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp21[7]="setjmp";static struct
_dyneither_ptr Cyc_Toc_setjmp_str={_tmp21,_tmp21,_tmp21 + 7};static struct _tuple0
Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0,(void*)&
Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;
static char _tmp23[14]="_push_handler";static struct _dyneither_ptr Cyc_Toc__push_handler_str={
_tmp23,_tmp23,_tmp23 + 14};static struct _tuple0 Cyc_Toc__push_handler_pr={{.Loc_n={
4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=&
Cyc_Toc__push_handler_ev;static char _tmp25[13]="_pop_handler";static struct
_dyneither_ptr Cyc_Toc__pop_handler_str={_tmp25,_tmp25,_tmp25 + 13};static struct
_tuple0 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1,& Cyc_Toc__pop_handler_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)& Cyc_Toc__pop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char
_tmp27[12]="_exn_thrown";static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={
_tmp27,_tmp27,_tmp27 + 12};static struct _tuple0 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,
0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=&
Cyc_Toc__exn_thrown_ev;static char _tmp29[14]="_npop_handler";static struct
_dyneither_ptr Cyc_Toc__npop_handler_str={_tmp29,_tmp29,_tmp29 + 14};static struct
_tuple0 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={
0,(void*)& Cyc_Toc__npop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char
_tmp2B[12]="_check_null";static struct _dyneither_ptr Cyc_Toc__check_null_str={
_tmp2B,_tmp2B,_tmp2B + 12};static struct _tuple0 Cyc_Toc__check_null_pr={{.Loc_n={4,
0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev;static char _tmp2D[28]="_check_known_subscript_null";static
struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp2D,_tmp2D,
_tmp2D + 28};static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={
4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)& Cyc_Toc__check_known_subscript_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp2F[31]="_check_known_subscript_notnull";static struct _dyneither_ptr
Cyc_Toc__check_known_subscript_notnull_str={_tmp2F,_tmp2F,_tmp2F + 31};static
struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={
1,& Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)&
Cyc_Toc__check_known_subscript_notnull_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;
static char _tmp31[27]="_check_dyneither_subscript";static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={
_tmp31,_tmp31,_tmp31 + 27};static struct _tuple0 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={
4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={
0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;
static char _tmp33[15]="_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={
_tmp33,_tmp33,_tmp33 + 15};static struct _tuple0 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={
4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__dyneither_ptr_re={1,& Cyc_Toc__dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=&
Cyc_Toc__dyneither_ptr_ev;static char _tmp35[15]="_tag_dyneither";static struct
_dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp35,_tmp35,_tmp35 + 15};static struct
_tuple0 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_dyneither_re={1,&
Cyc_Toc__tag_dyneither_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)& Cyc_Toc__tag_dyneither_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=&
Cyc_Toc__tag_dyneither_ev;static char _tmp37[20]="_init_dyneither_ptr";static
struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={_tmp37,_tmp37,_tmp37 + 20};
static struct _tuple0 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__init_dyneither_ptr_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__init_dyneither_ptr_re={
1,& Cyc_Toc__init_dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=&
Cyc_Toc__init_dyneither_ptr_ev;static char _tmp39[21]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp39,_tmp39,_tmp39
+ 21};static struct _tuple0 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_dyneither_ptr_re={
1,& Cyc_Toc__untag_dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=&
Cyc_Toc__untag_dyneither_ptr_ev;static char _tmp3B[20]="_get_dyneither_size";
static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={_tmp3B,_tmp3B,_tmp3B + 
20};static struct _tuple0 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_dyneither_size_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_dyneither_size_re={
1,& Cyc_Toc__get_dyneither_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=&
Cyc_Toc__get_dyneither_size_ev;static char _tmp3D[19]="_get_zero_arr_size";static
struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp3D,_tmp3D,_tmp3D + 19};
static struct _tuple0 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1,&
Cyc_Toc__get_zero_arr_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=&
Cyc_Toc__get_zero_arr_size_ev;static char _tmp3F[24]="_get_zero_arr_size_char";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp3F,_tmp3F,
_tmp3F + 24};static struct _tuple0 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},&
Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(void*)((
void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;
static char _tmp41[25]="_get_zero_arr_size_short";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={
_tmp41,_tmp41,_tmp41 + 25};static struct _tuple0 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={
4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;
static char _tmp43[23]="_get_zero_arr_size_int";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={
_tmp43,_tmp43,_tmp43 + 23};static struct _tuple0 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={
4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__get_zero_arr_size_int_re={1,& Cyc_Toc__get_zero_arr_size_int_pr,(void*)((
void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;
static char _tmp45[25]="_get_zero_arr_size_float";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={
_tmp45,_tmp45,_tmp45 + 25};static struct _tuple0 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={
4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__get_zero_arr_size_float_re={1,& Cyc_Toc__get_zero_arr_size_float_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;
static char _tmp47[26]="_get_zero_arr_size_double";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={
_tmp47,_tmp47,_tmp47 + 26};static struct _tuple0 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={
4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;
static char _tmp49[30]="_get_zero_arr_size_longdouble";static struct _dyneither_ptr
Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp49,_tmp49,_tmp49 + 30};static
struct _tuple0 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={
1,& Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=&
Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp4B[28]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp4B,_tmp4B,
_tmp4B + 28};static struct _tuple0 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={
4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;
static char _tmp4D[20]="_dyneither_ptr_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={
_tmp4D,_tmp4D,_tmp4D + 20};static struct _tuple0 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={
4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(void*)((void*)&
Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;
static char _tmp4F[15]="_zero_arr_plus";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={
_tmp4F,_tmp4F,_tmp4F + 15};static struct _tuple0 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=&
Cyc_Toc__zero_arr_plus_ev;static char _tmp51[20]="_zero_arr_plus_char";static
struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp51,_tmp51,_tmp51 + 20};
static struct _tuple0 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={
1,& Cyc_Toc__zero_arr_plus_char_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=&
Cyc_Toc__zero_arr_plus_char_ev;static char _tmp53[21]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp53,_tmp53,_tmp53
+ 21};static struct _tuple0 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={
1,& Cyc_Toc__zero_arr_plus_short_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=&
Cyc_Toc__zero_arr_plus_short_ev;static char _tmp55[19]="_zero_arr_plus_int";static
struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp55,_tmp55,_tmp55 + 19};
static struct _tuple0 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1,&
Cyc_Toc__zero_arr_plus_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=&
Cyc_Toc__zero_arr_plus_int_ev;static char _tmp57[21]="_zero_arr_plus_float";static
struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp57,_tmp57,_tmp57 + 21};
static struct _tuple0 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={
1,& Cyc_Toc__zero_arr_plus_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=&
Cyc_Toc__zero_arr_plus_float_ev;static char _tmp59[22]="_zero_arr_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp59,_tmp59,
_tmp59 + 22};static struct _tuple0 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},&
Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__zero_arr_plus_double_re={1,& Cyc_Toc__zero_arr_plus_double_pr,(void*)((
void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;
static char _tmp5B[26]="_zero_arr_plus_longdouble";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={
_tmp5B,_tmp5B,_tmp5B + 26};static struct _tuple0 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;
static char _tmp5D[24]="_zero_arr_plus_voidstar";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={
_tmp5D,_tmp5D,_tmp5D + 24};static struct _tuple0 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(void*)((
void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;
static char _tmp5F[28]="_dyneither_ptr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={
_tmp5F,_tmp5F,_tmp5F + 28};static struct _tuple0 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={
4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;
static char _tmp61[23]="_zero_arr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp61,_tmp61,_tmp61 + 23};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(void*)((
void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;
static char _tmp63[28]="_zero_arr_inplace_plus_char";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={
_tmp63,_tmp63,_tmp63 + 28};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;
static char _tmp65[29]="_zero_arr_inplace_plus_short";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp65,_tmp65,_tmp65 + 29};static struct
_tuple0 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={
1,& Cyc_Toc__zero_arr_inplace_plus_short_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=&
Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp67[27]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp67,_tmp67,
_tmp67 + 27};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,
0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;
static char _tmp69[29]="_zero_arr_inplace_plus_float";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp69,_tmp69,_tmp69 + 29};static struct
_tuple0 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={
1,& Cyc_Toc__zero_arr_inplace_plus_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=&
Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp6B[30]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp6B,
_tmp6B,_tmp6B + 30};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__zero_arr_inplace_plus_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_double_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;
static char _tmp6D[34]="_zero_arr_inplace_plus_longdouble";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp6D,_tmp6D,
_tmp6D + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;
static char _tmp6F[32]="_zero_arr_inplace_plus_voidstar";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp6F,_tmp6F,_tmp6F + 
32};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},&
Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;
static char _tmp71[33]="_dyneither_ptr_inplace_plus_post";static struct
_dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp71,_tmp71,_tmp71
+ 33};static struct _tuple0 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,
0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;
static char _tmp73[28]="_zero_arr_inplace_plus_post";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={
_tmp73,_tmp73,_tmp73 + 28};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp75[33]="_zero_arr_inplace_plus_post_char";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp75,_tmp75,_tmp75
+ 33};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,
0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;
static char _tmp77[34]="_zero_arr_inplace_plus_post_short";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp77,_tmp77,
_tmp77 + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;
static char _tmp79[32]="_zero_arr_inplace_plus_post_int";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp79,_tmp79,_tmp79 + 
32};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},&
Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;
static char _tmp7B[34]="_zero_arr_inplace_plus_post_float";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp7B,_tmp7B,
_tmp7B + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;
static char _tmp7D[35]="_zero_arr_inplace_plus_post_double";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp7D,_tmp7D,
_tmp7D + 35};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;
static char _tmp7F[39]="_zero_arr_inplace_plus_post_longdouble";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp7F,_tmp7F,
_tmp7F + 39};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;
static char _tmp81[37]="_zero_arr_inplace_plus_post_voidstar";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp81,_tmp81,
_tmp81 + 37};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;
static char _tmp83[10]="_cycalloc";static struct _dyneither_ptr Cyc_Toc__cycalloc_str={
_tmp83,_tmp83,_tmp83 + 10};static struct _tuple0 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},&
Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={
1,& Cyc_Toc__cycalloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0,(void*)&
Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;
static char _tmp85[11]="_cyccalloc";static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={
_tmp85,_tmp85,_tmp85 + 11};static struct _tuple0 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},&
Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={
1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=&
Cyc_Toc__cyccalloc_ev;static char _tmp87[17]="_cycalloc_atomic";static struct
_dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp87,_tmp87,_tmp87 + 17};static
struct _tuple0 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1,&
Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=&
Cyc_Toc__cycalloc_atomic_ev;static char _tmp89[18]="_cyccalloc_atomic";static
struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp89,_tmp89,_tmp89 + 18};
static struct _tuple0 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1,&
Cyc_Toc__cyccalloc_atomic_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=&
Cyc_Toc__cyccalloc_atomic_ev;static char _tmp8B[15]="_region_malloc";static struct
_dyneither_ptr Cyc_Toc__region_malloc_str={_tmp8B,_tmp8B,_tmp8B + 15};static struct
_tuple0 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1,&
Cyc_Toc__region_malloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=&
Cyc_Toc__region_malloc_ev;static char _tmp8D[15]="_region_calloc";static struct
_dyneither_ptr Cyc_Toc__region_calloc_str={_tmp8D,_tmp8D,_tmp8D + 15};static struct
_tuple0 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1,&
Cyc_Toc__region_calloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=&
Cyc_Toc__region_calloc_ev;static char _tmp8F[13]="_check_times";static struct
_dyneither_ptr Cyc_Toc__check_times_str={_tmp8F,_tmp8F,_tmp8F + 13};static struct
_tuple0 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={
0,(void*)& Cyc_Toc__check_times_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char
_tmp91[12]="_new_region";static struct _dyneither_ptr Cyc_Toc__new_region_str={
_tmp91,_tmp91,_tmp91 + 12};static struct _tuple0 Cyc_Toc__new_region_pr={{.Loc_n={4,
0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__new_region_re={1,& Cyc_Toc__new_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=&
Cyc_Toc__new_region_ev;static char _tmp93[13]="_push_region";static struct
_dyneither_ptr Cyc_Toc__push_region_str={_tmp93,_tmp93,_tmp93 + 13};static struct
_tuple0 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={
0,(void*)& Cyc_Toc__push_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char
_tmp95[12]="_pop_region";static struct _dyneither_ptr Cyc_Toc__pop_region_str={
_tmp95,_tmp95,_tmp95 + 12};static struct _tuple0 Cyc_Toc__pop_region_pr={{.Loc_n={4,
0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__pop_region_re={1,& Cyc_Toc__pop_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=&
Cyc_Toc__pop_region_ev;static char _tmp97[16]="_open_dynregion";static struct
_dyneither_ptr Cyc_Toc__open_dynregion_str={_tmp97,_tmp97,_tmp97 + 16};static
struct _tuple0 Cyc_Toc__open_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__open_dynregion_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__open_dynregion_re={1,&
Cyc_Toc__open_dynregion_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)& Cyc_Toc__open_dynregion_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=&
Cyc_Toc__open_dynregion_ev;static char _tmp99[16]="_push_dynregion";static struct
_dyneither_ptr Cyc_Toc__push_dynregion_str={_tmp99,_tmp99,_tmp99 + 16};static
struct _tuple0 Cyc_Toc__push_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__push_dynregion_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_dynregion_re={1,&
Cyc_Toc__push_dynregion_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)& Cyc_Toc__push_dynregion_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=&
Cyc_Toc__push_dynregion_ev;static char _tmp9B[15]="_pop_dynregion";static struct
_dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmp9B,_tmp9B,_tmp9B + 15};static struct
_tuple0 Cyc_Toc__pop_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_dynregion_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_dynregion_re={1,&
Cyc_Toc__pop_dynregion_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)& Cyc_Toc__pop_dynregion_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=&
Cyc_Toc__pop_dynregion_ev;static char _tmp9D[14]="_reset_region";static struct
_dyneither_ptr Cyc_Toc__reset_region_str={_tmp9D,_tmp9D,_tmp9D + 14};static struct
_tuple0 Cyc_Toc__reset_region_pr={{.Loc_n={4,0}},& Cyc_Toc__reset_region_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__reset_region_re={1,& Cyc_Toc__reset_region_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={
0,(void*)& Cyc_Toc__reset_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;static char
_tmp9F[19]="_throw_arraybounds";static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={
_tmp9F,_tmp9F,_tmp9F + 19};static struct _tuple0 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={
4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct
Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(void*)((void*)&
Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={
0,(void*)& Cyc_Toc__throw_arraybounds_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmpA1[29]="_dyneither_ptr_decrease_size";static struct _dyneither_ptr
Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpA1,_tmpA1,_tmpA1 + 29};static struct
_tuple0 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={
1,& Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=&
Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpA3[11]="_swap_word";static
struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpA3,_tmpA3,_tmpA3 + 11};static
struct _tuple0 Cyc_Toc__swap_word_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_word_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_word_re={1,& Cyc_Toc__swap_word_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={
0,(void*)& Cyc_Toc__swap_word_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct
Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpA5[16]="_swap_dyneither";
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpA5,_tmpA5,_tmpA5 + 16};
static struct _tuple0 Cyc_Toc__swap_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_dyneither_str};
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_dyneither_re={1,&
Cyc_Toc__swap_dyneither_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={0,(void*)& Cyc_Toc__swap_dyneither_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=&
Cyc_Toc__swap_dyneither_ev;static char _tmpA7[13]="_throw_match";static struct
_dyneither_ptr Cyc_Toc__throw_match_str={_tmpA7,_tmpA7,_tmpA7 + 13};static struct
_tuple0 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1,& Cyc_Toc__throw_match_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={
0,(void*)& Cyc_Toc__throw_match_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static struct
Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11,{{.UnknownAggr={
1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};static void*Cyc_Toc_dyneither_ptr_typ=(
void*)& Cyc_Toc_dyneither_ptr_typ_v;static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,
0,0,0};static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){static struct Cyc_Absyn_Stmt**
skip_stmt_opt=0;if(skip_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmpA75;
skip_stmt_opt=((_tmpA75=_cycalloc(sizeof(*_tmpA75)),((_tmpA75[0]=Cyc_Absyn_skip_stmt(
0),_tmpA75))));}return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(
void*t,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,
0);}static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct
_tmpA78;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpA77;return(void*)((
_tmpA77=_cycalloc(sizeof(*_tmpA77)),((_tmpA77[0]=((_tmpA78.tag=13,((_tmpA78.f1=(
void*)t,((_tmpA78.f2=e,((_tmpA78.f3=0,((_tmpA78.f4=Cyc_Absyn_No_coercion,_tmpA78)))))))))),
_tmpA77))));}static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct
_tmpA7B;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpA7A;return(void*)((
_tmpA7A=_cycalloc(sizeof(*_tmpA7A)),((_tmpA7A[0]=((_tmpA7B.tag=19,((_tmpA7B.f1=e,
_tmpA7B)))),_tmpA7A))));}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct
_tmpA7E;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpA7D;return(void*)((
_tmpA7D=_cycalloc(sizeof(*_tmpA7D)),((_tmpA7D[0]=((_tmpA7E.tag=22,((_tmpA7E.f1=
e1,((_tmpA7E.f2=e2,_tmpA7E)))))),_tmpA7D))));}static void*Cyc_Toc_stmt_exp_r(
struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmpA81;
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpA80;return(void*)((_tmpA80=
_cycalloc(sizeof(*_tmpA80)),((_tmpA80[0]=((_tmpA81.tag=35,((_tmpA81.f1=s,_tmpA81)))),
_tmpA80))));}static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct
_tmpA84;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpA83;return(void*)((
_tmpA83=_cycalloc(sizeof(*_tmpA83)),((_tmpA83[0]=((_tmpA84.tag=16,((_tmpA84.f1=(
void*)t,_tmpA84)))),_tmpA83))));}static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpA87;
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpA86;return(void*)((_tmpA86=
_cycalloc(sizeof(*_tmpA86)),((_tmpA86[0]=((_tmpA87.tag=9,((_tmpA87.f1=e,((
_tmpA87.f2=es,((_tmpA87.f3=0,((_tmpA87.f4=1,_tmpA87)))))))))),_tmpA86))));}
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct
_tmpA8A;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpA89;return(void*)((
_tmpA89=_cycalloc(sizeof(*_tmpA89)),((_tmpA89[0]=((_tmpA8A.tag=1,((_tmpA8A.f1=e,
_tmpA8A)))),_tmpA89))));}static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,
struct Cyc_Absyn_Stmt*s2){struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmpA8D;
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpA8C;return(void*)((_tmpA8C=
_cycalloc(sizeof(*_tmpA8C)),((_tmpA8C[0]=((_tmpA8D.tag=2,((_tmpA8D.f1=s1,((
_tmpA8D.f2=s2,_tmpA8D)))))),_tmpA8C))));}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct
_tmpA90;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpA8F;return(void*)((
_tmpA8F=_cycalloc(sizeof(*_tmpA8F)),((_tmpA8F[0]=((_tmpA90.tag=5,((_tmpA90.f1=e1,((
_tmpA90.f2=e2,((_tmpA90.f3=e3,_tmpA90)))))))),_tmpA8F))));}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct
_tmpA93;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpA92;return(void*)((
_tmpA92=_cycalloc(sizeof(*_tmpA92)),((_tmpA92[0]=((_tmpA93.tag=20,((_tmpA93.f1=e,((
_tmpA93.f2=n,((_tmpA93.f3=0,((_tmpA93.f4=0,_tmpA93)))))))))),_tmpA92))));}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpA96;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*
_tmpA95;return(void*)((_tmpA95=_cycalloc(sizeof(*_tmpA95)),((_tmpA95[0]=((
_tmpA96.tag=21,((_tmpA96.f1=e,((_tmpA96.f2=n,((_tmpA96.f3=0,((_tmpA96.f4=0,
_tmpA96)))))))))),_tmpA95))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct
_tmpA99;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpA98;return(void*)((
_tmpA98=_cycalloc(sizeof(*_tmpA98)),((_tmpA98[0]=((_tmpA99.tag=34,((_tmpA99.f1=
tdopt,((_tmpA99.f2=ds,_tmpA99)))))),_tmpA98))));}static void*Cyc_Toc_goto_stmt_r(
struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct
_tmpA9C;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmpA9B;return(void*)((
_tmpA9B=_cycalloc(sizeof(*_tmpA9B)),((_tmpA9B[0]=((_tmpA9C.tag=8,((_tmpA9C.f1=v,((
_tmpA9C.f2=s,_tmpA9C)))))),_tmpA9B))));}static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct
Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};struct Cyc_Toc_functionSet{
struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;
struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*
flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={&
Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,&
Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,&
Cyc_Toc__zero_arr_plus_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={&
Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,&
Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,&
Cyc_Toc__get_zero_arr_size_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(
struct Cyc_Toc_functionSet*fS,void*t){struct Cyc_Absyn_Exp*function;{void*_tmpC6=
Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmpC8;int _tmpCA;int _tmpCC;_LL1: {
struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpC7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmpC6;if(_tmpC7->tag != 6)goto _LL3;else{_tmpC8=_tmpC7->f2;}}_LL2: switch(_tmpC8){
case Cyc_Absyn_Char_sz: _LLD: function=fS->fchar;break;case Cyc_Absyn_Short_sz: _LLE:
function=fS->fshort;break;case Cyc_Absyn_Int_sz: _LLF: function=fS->fint;break;
default: _LL10: {struct Cyc_Core_Impossible_exn_struct _tmpAA2;const char*_tmpAA1;
struct Cyc_Core_Impossible_exn_struct*_tmpAA0;(int)_throw((void*)((_tmpAA0=
_cycalloc(sizeof(*_tmpAA0)),((_tmpAA0[0]=((_tmpAA2.tag=Cyc_Core_Impossible,((
_tmpAA2.f1=((_tmpAA1="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpAA1,sizeof(char),44))),_tmpAA2)))),_tmpAA0)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpC9=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)
_tmpC6;if(_tmpC9->tag != 7)goto _LL5;else{_tmpCA=_tmpC9->f1;if(_tmpCA != 0)goto _LL5;}}
_LL4: function=fS->ffloat;goto _LL0;_LL5: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmpCB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpC6;if(_tmpCB->tag != 7)
goto _LL7;else{_tmpCC=_tmpCB->f1;if(_tmpCC != 1)goto _LL7;}}_LL6: function=fS->fdouble;
goto _LL0;_LL7: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCD=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)
_tmpC6;if(_tmpCD->tag != 7)goto _LL9;}_LL8: function=fS->flongdouble;goto _LL0;_LL9: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmpC6;if(_tmpCE->tag != 5)goto _LLB;}_LLA: function=fS->fvoidstar;goto _LL0;_LLB:;
_LLC: {struct Cyc_Core_Impossible_exn_struct _tmpAAF;const char*_tmpAAE;void*
_tmpAAD[1];struct Cyc_String_pa_PrintArg_struct _tmpAAC;struct Cyc_Core_Impossible_exn_struct*
_tmpAAB;(int)_throw((void*)((_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB[0]=((
_tmpAAF.tag=Cyc_Core_Impossible,((_tmpAAF.f1=(struct _dyneither_ptr)((_tmpAAC.tag=
0,((_tmpAAC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmpAAD[0]=& _tmpAAC,Cyc_aprintf(((_tmpAAE="impossible expression type %s (not int, float, double, or pointer)",
_tag_dyneither(_tmpAAE,sizeof(char),67))),_tag_dyneither(_tmpAAD,sizeof(void*),1)))))))),
_tmpAAF)))),_tmpAAB)))));}_LL0:;}return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(
fS,(void*)_check_null(arr->topt));}struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){void*_tmpD7=Cyc_Tcutil_compress((
void*)_check_null(arr->topt));struct Cyc_Absyn_PtrInfo _tmpD9;void*_tmpDA;_LL13: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmpD7;if(_tmpD8->tag != 5)goto _LL15;else{_tmpD9=_tmpD8->f1;_tmpDA=_tmpD9.elt_typ;}}
_LL14: return Cyc_Toc_getFunctionType(fS,_tmpDA);_LL15:;_LL16: {struct Cyc_Core_Impossible_exn_struct
_tmpAB5;const char*_tmpAB4;struct Cyc_Core_Impossible_exn_struct*_tmpAB3;(int)
_throw((void*)((_tmpAB3=_cycalloc(sizeof(*_tmpAB3)),((_tmpAB3[0]=((_tmpAB5.tag=
Cyc_Core_Impossible,((_tmpAB5.f1=((_tmpAB4="impossible type (not pointer)",
_tag_dyneither(_tmpAB4,sizeof(char),30))),_tmpAB5)))),_tmpAB3)))));}_LL12:;}
struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmpDE=e->r;union Cyc_Absyn_Cnst _tmpE0;struct _tuple3
_tmpE1;char _tmpE2;union Cyc_Absyn_Cnst _tmpE4;struct _dyneither_ptr _tmpE5;union Cyc_Absyn_Cnst
_tmpE7;struct _tuple4 _tmpE8;short _tmpE9;union Cyc_Absyn_Cnst _tmpEB;struct _tuple5
_tmpEC;int _tmpED;union Cyc_Absyn_Cnst _tmpEF;struct _tuple6 _tmpF0;long long _tmpF1;
union Cyc_Absyn_Cnst _tmpF3;int _tmpF4;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_List_List*
_tmpF8;struct Cyc_List_List*_tmpFA;struct Cyc_List_List*_tmpFC;struct Cyc_List_List*
_tmpFE;struct Cyc_List_List*_tmp100;_LL18: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmpDF=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpDF->tag != 0)
goto _LL1A;else{_tmpE0=_tmpDF->f1;if((_tmpE0.Char_c).tag != 2)goto _LL1A;_tmpE1=(
struct _tuple3)(_tmpE0.Char_c).val;_tmpE2=_tmpE1.f2;}}_LL19: return _tmpE2 == '\000';
_LL1A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmpDE;if(_tmpE3->tag != 0)goto _LL1C;else{_tmpE4=_tmpE3->f1;if((_tmpE4.Wchar_c).tag
!= 3)goto _LL1C;_tmpE5=(struct _dyneither_ptr)(_tmpE4.Wchar_c).val;}}_LL1B: {
unsigned long _tmp101=Cyc_strlen((struct _dyneither_ptr)_tmpE5);int i=0;if(_tmp101
>= 2  && *((const char*)_check_dyneither_subscript(_tmpE5,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmpE5,sizeof(char),1))== '0')i=2;
else{if((*((const char*)_check_dyneither_subscript(_tmpE5,sizeof(char),1))== 'x'
 && _tmp101 >= 3) && *((const char*)_check_dyneither_subscript(_tmpE5,sizeof(char),
2))== '0')i=3;else{return 0;}}for(0;i < _tmp101;++ i){if(*((const char*)
_check_dyneither_subscript(_tmpE5,sizeof(char),i))!= '0')return 0;}return 1;}else{
return 0;}}_LL1C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmpDE;if(_tmpE6->tag != 0)goto _LL1E;else{_tmpE7=_tmpE6->f1;if((_tmpE7.Short_c).tag
!= 4)goto _LL1E;_tmpE8=(struct _tuple4)(_tmpE7.Short_c).val;_tmpE9=_tmpE8.f2;}}
_LL1D: return _tmpE9 == 0;_LL1E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmpEA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpEA->tag != 0)
goto _LL20;else{_tmpEB=_tmpEA->f1;if((_tmpEB.Int_c).tag != 5)goto _LL20;_tmpEC=(
struct _tuple5)(_tmpEB.Int_c).val;_tmpED=_tmpEC.f2;}}_LL1F: return _tmpED == 0;_LL20: {
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpEE=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmpDE;if(_tmpEE->tag != 0)goto _LL22;else{_tmpEF=_tmpEE->f1;if((_tmpEF.LongLong_c).tag
!= 6)goto _LL22;_tmpF0=(struct _tuple6)(_tmpEF.LongLong_c).val;_tmpF1=_tmpF0.f2;}}
_LL21: return _tmpF1 == 0;_LL22: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmpF2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpF2->tag != 0)
goto _LL24;else{_tmpF3=_tmpF2->f1;if((_tmpF3.Null_c).tag != 1)goto _LL24;_tmpF4=(
int)(_tmpF3.Null_c).val;}}_LL23: return 1;_LL24: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*
_tmpF5=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpF5->tag != 13)
goto _LL26;else{_tmpF6=_tmpF5->f2;}}_LL25: return Cyc_Toc_is_zero(_tmpF6);_LL26: {
struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpF7=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)
_tmpDE;if(_tmpF7->tag != 23)goto _LL28;else{_tmpF8=_tmpF7->f1;}}_LL27: return((int(*)(
int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,
_tmpF8);_LL28: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF9=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)
_tmpDE;if(_tmpF9->tag != 25)goto _LL2A;else{_tmpFA=_tmpF9->f1;}}_LL29: _tmpFC=
_tmpFA;goto _LL2B;_LL2A: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpFB=(
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpFB->tag != 27)
goto _LL2C;else{_tmpFC=_tmpFB->f3;}}_LL2B: _tmpFE=_tmpFC;goto _LL2D;_LL2C: {struct
Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpFD=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)
_tmpDE;if(_tmpFD->tag != 24)goto _LL2E;else{_tmpFE=_tmpFD->f2;}}_LL2D: _tmp100=
_tmpFE;goto _LL2F;_LL2E: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*
_tmpFF=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpFF->tag
!= 34)goto _LL30;else{_tmp100=_tmpFF->f2;}}_LL2F: for(0;_tmp100 != 0;_tmp100=
_tmp100->tl){if(!Cyc_Toc_is_zero((*((struct _tuple16*)_tmp100->hd)).f2))return 0;}
return 1;_LL30:;_LL31: return 0;_LL17:;}static int Cyc_Toc_is_nullable(void*t){void*
_tmp102=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp104;struct Cyc_Absyn_PtrAtts
_tmp105;union Cyc_Absyn_Constraint*_tmp106;_LL33: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp103=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp102;if(_tmp103->tag
!= 5)goto _LL35;else{_tmp104=_tmp103->f1;_tmp105=_tmp104.ptr_atts;_tmp106=_tmp105.nullable;}}
_LL34: return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp106);_LL35:;_LL36: {const char*_tmpAB8;void*_tmpAB7;(_tmpAB7=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB8="is_nullable",
_tag_dyneither(_tmpAB8,sizeof(char),12))),_tag_dyneither(_tmpAB7,sizeof(void*),0)));}
_LL32:;}static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr s,
struct _tuple0*x){unsigned int slen=Cyc_strlen((struct _dyneither_ptr)s);
unsigned int len=9 + slen;struct _tuple0 _tmp10A;union Cyc_Absyn_Nmspace _tmp10B;
struct _dyneither_ptr*_tmp10C;struct _dyneither_ptr _tmp10D;struct _tuple0*_tmp109=x;
_tmp10A=*_tmp109;_tmp10B=_tmp10A.f1;_tmp10C=_tmp10A.f2;_tmp10D=*_tmp10C;{
unsigned int vlen=Cyc_strlen((struct _dyneither_ptr)_tmp10D);len +=1 + vlen;{struct
Cyc_List_List*nms;{union Cyc_Absyn_Nmspace _tmp10E=_tmp10B;int _tmp10F;struct Cyc_List_List*
_tmp110;struct Cyc_List_List*_tmp111;struct Cyc_List_List*_tmp112;_LL38: if((
_tmp10E.Loc_n).tag != 4)goto _LL3A;_tmp10F=(int)(_tmp10E.Loc_n).val;_LL39: nms=0;
goto _LL37;_LL3A: if((_tmp10E.Rel_n).tag != 1)goto _LL3C;_tmp110=(struct Cyc_List_List*)(
_tmp10E.Rel_n).val;_LL3B: nms=_tmp110;goto _LL37;_LL3C: if((_tmp10E.Abs_n).tag != 2)
goto _LL3E;_tmp111=(struct Cyc_List_List*)(_tmp10E.Abs_n).val;_LL3D: nms=_tmp111;
goto _LL37;_LL3E: if((_tmp10E.C_n).tag != 3)goto _LL37;_tmp112=(struct Cyc_List_List*)(
_tmp10E.C_n).val;_LL3F: nms=_tmp112;goto _LL37;_LL37:;}{struct Cyc_List_List*
_tmp113=nms;for(0;_tmp113 != 0;_tmp113=_tmp113->tl){len +=1 + Cyc_strlen((struct
_dyneither_ptr)*((struct _dyneither_ptr*)_tmp113->hd));}}{char*_tmpABA;
unsigned int _tmpAB9;struct _dyneither_ptr buf=(_tmpAB9=len,((_tmpABA=
_cyccalloc_atomic(sizeof(char),_tmpAB9),_tag_dyneither(_tmpABA,sizeof(char),
_tmpAB9))));struct _dyneither_ptr p=buf;Cyc_strcpy(p,(struct _dyneither_ptr)s);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr _tmp115;struct _dyneither_ptr*_tmp114=(struct _dyneither_ptr*)
nms->hd;_tmp115=*_tmp114;{char _tmpABD;char _tmpABC;struct _dyneither_ptr _tmpABB;(
_tmpABB=p,((_tmpABC=*((char*)_check_dyneither_subscript(_tmpABB,sizeof(char),0)),((
_tmpABD='_',((_get_dyneither_size(_tmpABB,sizeof(char))== 1  && (_tmpABC == '\000'
 && _tmpABD != '\000')?_throw_arraybounds(): 1,*((char*)_tmpABB.curr)=_tmpABD)))))));}
_dyneither_ptr_inplace_plus(& p,sizeof(char),1);Cyc_strcpy(p,(struct
_dyneither_ptr)_tmp115);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((
struct _dyneither_ptr)_tmp115));}{char _tmpAC0;char _tmpABF;struct _dyneither_ptr
_tmpABE;(_tmpABE=p,((_tmpABF=*((char*)_check_dyneither_subscript(_tmpABE,sizeof(
char),0)),((_tmpAC0='_',((_get_dyneither_size(_tmpABE,sizeof(char))== 1  && (
_tmpABF == '\000'  && _tmpAC0 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpABE.curr)=
_tmpAC0)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);Cyc_strcpy(p,(
struct _dyneither_ptr)_tmp10D);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)
vlen);{const char*_tmpAC1;Cyc_strcpy(p,((_tmpAC1="_struct",_tag_dyneither(_tmpAC1,
sizeof(char),8))));}return(struct _dyneither_ptr)buf;};};};}static char _tmp130[8]="*bogus*";
static struct _tuple0*Cyc_Toc_collapse_qvars(struct _tuple0*fieldname,struct _tuple0*
dtname){struct _DynRegionHandle*_tmp120;struct Cyc_Dict_Dict*_tmp121;struct Cyc_Toc_TocState
_tmp11F=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp120=
_tmp11F.dyn;_tmp121=_tmp11F.qvar_tags;{static struct _dyneither_ptr bogus_string={
_tmp130,_tmp130,_tmp130 + 8};static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},&
bogus_string};static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};{struct _tuple14
_tmpAC2;pair=((_tmpAC2.f1=fieldname,((_tmpAC2.f2=dtname,_tmpAC2))));}{struct
_DynRegionFrame _tmp123;struct _RegionHandle*d=_open_dynregion(& _tmp123,_tmp120);{
struct _tuple0**_tmp124=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,struct _tuple14*
k))Cyc_Dict_lookup_opt)(*_tmp121,(struct _tuple14*)& pair);if(_tmp124 != 0){struct
_tuple0*_tmp125=*_tmp124;_npop_handler(0);return _tmp125;}{struct _tuple14*_tmpAC3;
struct _tuple14*_tmp126=(_tmpAC3=_cycalloc(sizeof(*_tmpAC3)),((_tmpAC3->f1=
fieldname,((_tmpAC3->f2=dtname,_tmpAC3)))));struct _tuple0 _tmp128;union Cyc_Absyn_Nmspace
_tmp129;struct _dyneither_ptr*_tmp12A;struct _dyneither_ptr _tmp12B;struct _tuple0*
_tmp127=fieldname;_tmp128=*_tmp127;_tmp129=_tmp128.f1;_tmp12A=_tmp128.f2;_tmp12B=*
_tmp12A;{struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp12B,dtname);struct
_dyneither_ptr*_tmpAC6;struct _tuple0*_tmpAC5;struct _tuple0*res=(_tmpAC5=
_cycalloc(sizeof(*_tmpAC5)),((_tmpAC5->f1=_tmp129,((_tmpAC5->f2=((_tmpAC6=
_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6[0]=newvar,_tmpAC6)))),_tmpAC5)))));*
_tmp121=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct
_tuple0*v))Cyc_Dict_insert)(*_tmp121,(struct _tuple14*)_tmp126,res);{struct
_tuple0*_tmp12C=res;_npop_handler(0);return _tmp12C;};};};};_pop_dynregion(d);};};}
static void*Cyc_Toc_typ_to_c(void*t);struct _tuple17{void*f1;struct Cyc_List_List*
f2;};static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){struct
_DynRegionHandle*_tmp132;struct Cyc_List_List**_tmp133;struct Cyc_Toc_TocState
_tmp131=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp132=
_tmp131.dyn;_tmp133=_tmp131.tuple_types;{struct _DynRegionFrame _tmp134;struct
_RegionHandle*d=_open_dynregion(& _tmp134,_tmp132);{struct Cyc_List_List*_tmp135=*
_tmp133;for(0;_tmp135 != 0;_tmp135=_tmp135->tl){struct _tuple17 _tmp137;void*
_tmp138;struct Cyc_List_List*_tmp139;struct _tuple17*_tmp136=(struct _tuple17*)
_tmp135->hd;_tmp137=*_tmp136;_tmp138=_tmp137.f1;_tmp139=_tmp137.f2;{struct Cyc_List_List*
_tmp13A=tqs0;for(0;_tmp13A != 0  && _tmp139 != 0;(_tmp13A=_tmp13A->tl,_tmp139=
_tmp139->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple10*)_tmp13A->hd)).f2,(void*)
_tmp139->hd))break;}if(_tmp13A == 0  && _tmp139 == 0){void*_tmp13B=_tmp138;
_npop_handler(0);return _tmp13B;}};}}{struct Cyc_Int_pa_PrintArg_struct _tmpACE;
void*_tmpACD[1];const char*_tmpACC;struct _dyneither_ptr*_tmpACB;struct
_dyneither_ptr*xname=(_tmpACB=_cycalloc(sizeof(*_tmpACB)),((_tmpACB[0]=(struct
_dyneither_ptr)((_tmpACE.tag=1,((_tmpACE.f1=(unsigned long)Cyc_Toc_tuple_type_counter
++,((_tmpACD[0]=& _tmpACE,Cyc_aprintf(((_tmpACC="_tuple%d",_tag_dyneither(_tmpACC,
sizeof(char),9))),_tag_dyneither(_tmpACD,sizeof(void*),1)))))))),_tmpACB)));void*
x=Cyc_Absyn_strct(xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(
d,Cyc_Tcutil_snd_tqt,tqs0);struct Cyc_List_List*_tmp13C=0;struct Cyc_List_List*ts2=
ts;{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpAD1;
struct Cyc_List_List*_tmpAD0;_tmp13C=((_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((
_tmpAD0->hd=((_tmpAD1=_cycalloc(sizeof(*_tmpAD1)),((_tmpAD1->name=Cyc_Absyn_fieldname(
i),((_tmpAD1->tq=Cyc_Toc_mt_tq,((_tmpAD1->type=(void*)ts2->hd,((_tmpAD1->width=0,((
_tmpAD1->attributes=0,_tmpAD1)))))))))))),((_tmpAD0->tl=_tmp13C,_tmpAD0))))));}}
_tmp13C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp13C);{struct Cyc_Absyn_AggrdeclImpl*_tmpAD6;struct _tuple0*_tmpAD5;struct Cyc_Absyn_Aggrdecl*
_tmpAD4;struct Cyc_Absyn_Aggrdecl*_tmp13F=(_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((
_tmpAD4->kind=Cyc_Absyn_StructA,((_tmpAD4->sc=Cyc_Absyn_Public,((_tmpAD4->name=((
_tmpAD5=_cycalloc(sizeof(*_tmpAD5)),((_tmpAD5->f1=Cyc_Absyn_Rel_n(0),((_tmpAD5->f2=
xname,_tmpAD5)))))),((_tmpAD4->tvs=0,((_tmpAD4->impl=((_tmpAD6=_cycalloc(sizeof(*
_tmpAD6)),((_tmpAD6->exist_vars=0,((_tmpAD6->rgn_po=0,((_tmpAD6->fields=_tmp13C,((
_tmpAD6->tagged=0,_tmpAD6)))))))))),((_tmpAD4->attributes=0,_tmpAD4)))))))))))));{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpADC;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct
_tmpADB;struct Cyc_List_List*_tmpADA;Cyc_Toc_result_decls=((_tmpADA=_cycalloc(
sizeof(*_tmpADA)),((_tmpADA->hd=Cyc_Absyn_new_decl((void*)((_tmpADC=_cycalloc(
sizeof(*_tmpADC)),((_tmpADC[0]=((_tmpADB.tag=6,((_tmpADB.f1=_tmp13F,_tmpADB)))),
_tmpADC)))),0),((_tmpADA->tl=Cyc_Toc_result_decls,_tmpADA))))));}{struct _tuple17*
_tmpADF;struct Cyc_List_List*_tmpADE;*_tmp133=((_tmpADE=_region_malloc(d,sizeof(*
_tmpADE)),((_tmpADE->hd=((_tmpADF=_region_malloc(d,sizeof(*_tmpADF)),((_tmpADF->f1=
x,((_tmpADF->f2=ts,_tmpADF)))))),((_tmpADE->tl=*_tmp133,_tmpADE))))));}{void*
_tmp145=x;_npop_handler(0);return _tmp145;};};};;_pop_dynregion(d);};}struct
_tuple18{struct _tuple0*f1;struct Cyc_List_List*f2;void*f3;};static void*Cyc_Toc_add_struct_type(
struct _tuple0*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*
type_args,struct Cyc_List_List*fields){struct _DynRegionHandle*_tmp14E;struct Cyc_List_List**
_tmp14F;struct Cyc_Toc_TocState _tmp14D=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp14E=_tmp14D.dyn;_tmp14F=_tmp14D.abs_struct_types;{struct _DynRegionFrame
_tmp150;struct _RegionHandle*d=_open_dynregion(& _tmp150,_tmp14E);{struct Cyc_List_List*
_tmp151=*_tmp14F;for(0;_tmp151 != 0;_tmp151=_tmp151->tl){struct _tuple18 _tmp153;
struct _tuple0*_tmp154;struct Cyc_List_List*_tmp155;void*_tmp156;struct _tuple18*
_tmp152=(struct _tuple18*)_tmp151->hd;_tmp153=*_tmp152;_tmp154=_tmp153.f1;_tmp155=
_tmp153.f2;_tmp156=_tmp153.f3;if(Cyc_Absyn_qvar_cmp(_tmp154,struct_name)== 0  && ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(type_args)== ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp155)){int okay=1;{struct Cyc_List_List*_tmp157=type_args;
for(0;_tmp157 != 0;(_tmp157=_tmp157->tl,_tmp155=_tmp155->tl)){void*_tmp158=(void*)
_tmp157->hd;void*_tmp159=(void*)((struct Cyc_List_List*)_check_null(_tmp155))->hd;
if(Cyc_Tcutil_unify(_tmp158,_tmp159))continue;okay=0;break;}}if(okay){void*
_tmp15A=_tmp156;_npop_handler(0);return _tmp15A;}}}}{struct Cyc_Int_pa_PrintArg_struct
_tmpAE7;void*_tmpAE6[1];const char*_tmpAE5;struct _dyneither_ptr*_tmpAE4;struct
_dyneither_ptr*xname=(_tmpAE4=_cycalloc(sizeof(*_tmpAE4)),((_tmpAE4[0]=(struct
_dyneither_ptr)((_tmpAE7.tag=1,((_tmpAE7.f1=(unsigned long)Cyc_Toc_tuple_type_counter
++,((_tmpAE6[0]=& _tmpAE7,Cyc_aprintf(((_tmpAE5="_tuple%d",_tag_dyneither(_tmpAE5,
sizeof(char),9))),_tag_dyneither(_tmpAE6,sizeof(void*),1)))))))),_tmpAE4)));void*
x=Cyc_Absyn_strct(xname);struct Cyc_List_List*_tmp15B=0;{struct _tuple18*_tmpAEA;
struct Cyc_List_List*_tmpAE9;*_tmp14F=((_tmpAE9=_region_malloc(d,sizeof(*_tmpAE9)),((
_tmpAE9->hd=((_tmpAEA=_region_malloc(d,sizeof(*_tmpAEA)),((_tmpAEA->f1=
struct_name,((_tmpAEA->f2=type_args,((_tmpAEA->f3=x,_tmpAEA)))))))),((_tmpAE9->tl=*
_tmp14F,_tmpAE9))))));}{struct _RegionHandle _tmp15E=_new_region("r");struct
_RegionHandle*r=& _tmp15E;_push_region(r);{struct Cyc_List_List*_tmp15F=((struct
Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);for(0;fields != 
0;fields=fields->tl){struct Cyc_Absyn_Aggrfield*_tmp160=(struct Cyc_Absyn_Aggrfield*)
fields->hd;void*t=_tmp160->type;t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,
_tmp15F,t));if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))break;{struct
Cyc_Absyn_Aggrfield*_tmpAED;struct Cyc_List_List*_tmpAEC;_tmp15B=((_tmpAEC=
_cycalloc(sizeof(*_tmpAEC)),((_tmpAEC->hd=((_tmpAED=_cycalloc(sizeof(*_tmpAED)),((
_tmpAED->name=_tmp160->name,((_tmpAED->tq=Cyc_Toc_mt_tq,((_tmpAED->type=t,((
_tmpAED->width=_tmp160->width,((_tmpAED->attributes=_tmp160->attributes,_tmpAED)))))))))))),((
_tmpAEC->tl=_tmp15B,_tmpAEC))))));};}_tmp15B=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp15B);{struct Cyc_Absyn_AggrdeclImpl*_tmpAF2;struct
_tuple0*_tmpAF1;struct Cyc_Absyn_Aggrdecl*_tmpAF0;struct Cyc_Absyn_Aggrdecl*
_tmp163=(_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((_tmpAF0->kind=Cyc_Absyn_StructA,((
_tmpAF0->sc=Cyc_Absyn_Public,((_tmpAF0->name=((_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((
_tmpAF1->f1=Cyc_Absyn_Rel_n(0),((_tmpAF1->f2=xname,_tmpAF1)))))),((_tmpAF0->tvs=
0,((_tmpAF0->impl=((_tmpAF2=_cycalloc(sizeof(*_tmpAF2)),((_tmpAF2->exist_vars=0,((
_tmpAF2->rgn_po=0,((_tmpAF2->fields=_tmp15B,((_tmpAF2->tagged=0,_tmpAF2)))))))))),((
_tmpAF0->attributes=0,_tmpAF0)))))))))))));{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*
_tmpAF8;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpAF7;struct Cyc_List_List*
_tmpAF6;Cyc_Toc_result_decls=((_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6->hd=
Cyc_Absyn_new_decl((void*)((_tmpAF8=_cycalloc(sizeof(*_tmpAF8)),((_tmpAF8[0]=((
_tmpAF7.tag=6,((_tmpAF7.f1=_tmp163,_tmpAF7)))),_tmpAF8)))),0),((_tmpAF6->tl=Cyc_Toc_result_decls,
_tmpAF6))))));}{void*_tmp167=x;_npop_handler(1);return _tmp167;};};};_pop_region(
r);};};;_pop_dynregion(d);};}struct _tuple0*Cyc_Toc_temp_var(){int _tmp16F=Cyc_Toc_temp_var_counter
++;struct _dyneither_ptr*_tmpB05;const char*_tmpB04;void*_tmpB03[1];struct Cyc_Int_pa_PrintArg_struct
_tmpB02;struct _tuple0*_tmpB01;struct _tuple0*res=(_tmpB01=_cycalloc(sizeof(*
_tmpB01)),((_tmpB01->f1=Cyc_Absyn_Loc_n,((_tmpB01->f2=((_tmpB05=_cycalloc(
sizeof(*_tmpB05)),((_tmpB05[0]=(struct _dyneither_ptr)((_tmpB02.tag=1,((_tmpB02.f1=(
unsigned int)_tmp16F,((_tmpB03[0]=& _tmpB02,Cyc_aprintf(((_tmpB04="_tmp%X",
_tag_dyneither(_tmpB04,sizeof(char),7))),_tag_dyneither(_tmpB03,sizeof(void*),1)))))))),
_tmpB05)))),_tmpB01)))));return res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp176;struct Cyc_Xarray_Xarray*_tmp177;struct Cyc_Toc_TocState
_tmp175=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp176=
_tmp175.dyn;_tmp177=_tmp175.temp_labels;{struct _DynRegionFrame _tmp178;struct
_RegionHandle*d=_open_dynregion(& _tmp178,_tmp176);{int _tmp179=Cyc_Toc_fresh_label_counter
++;if(_tmp179 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp177)){
struct _dyneither_ptr*_tmp17A=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp177,_tmp179);_npop_handler(0);return _tmp17A;}{struct Cyc_Int_pa_PrintArg_struct
_tmpB0D;void*_tmpB0C[1];const char*_tmpB0B;struct _dyneither_ptr*_tmpB0A;struct
_dyneither_ptr*res=(_tmpB0A=_cycalloc(sizeof(*_tmpB0A)),((_tmpB0A[0]=(struct
_dyneither_ptr)((_tmpB0D.tag=1,((_tmpB0D.f1=(unsigned int)_tmp179,((_tmpB0C[0]=&
_tmpB0D,Cyc_aprintf(((_tmpB0B="_LL%X",_tag_dyneither(_tmpB0B,sizeof(char),6))),
_tag_dyneither(_tmpB0C,sizeof(void*),1)))))))),_tmpB0A)));if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp177,res)!= _tmp179){const char*
_tmpB10;void*_tmpB0F;(_tmpB0F=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB10="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpB10,sizeof(char),43))),_tag_dyneither(_tmpB0F,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp17D=res;_npop_handler(0);return _tmp17D;};};};
_pop_dynregion(d);};}static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*
td,struct _tuple0*name){int ans=0;struct Cyc_List_List*_tmp182=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp182))->hd)->name)
!= 0){++ ans;_tmp182=_tmp182->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);static void Cyc_Toc_aggrdecl_to_c(
struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);static void Cyc_Toc_datatypedecl_to_c(
struct Cyc_Absyn_Datatypedecl*tud);static struct _tuple8*Cyc_Toc_arg_to_c(struct
_tuple8*a){struct _dyneither_ptr*_tmp184;struct Cyc_Absyn_Tqual _tmp185;void*
_tmp186;struct _tuple8 _tmp183=*a;_tmp184=_tmp183.f1;_tmp185=_tmp183.f2;_tmp186=
_tmp183.f3;{struct _tuple8*_tmpB11;return(_tmpB11=_cycalloc(sizeof(*_tmpB11)),((
_tmpB11->f1=_tmp184,((_tmpB11->f2=_tmp185,((_tmpB11->f3=Cyc_Toc_typ_to_c(_tmp186),
_tmpB11)))))));};}static struct _tuple10*Cyc_Toc_typ_to_c_f(struct _tuple10*x){
struct Cyc_Absyn_Tqual _tmp189;void*_tmp18A;struct _tuple10 _tmp188=*x;_tmp189=
_tmp188.f1;_tmp18A=_tmp188.f2;{struct _tuple10*_tmpB12;return(_tmpB12=_cycalloc(
sizeof(*_tmpB12)),((_tmpB12->f1=_tmp189,((_tmpB12->f2=Cyc_Toc_typ_to_c(_tmp18A),
_tmpB12)))));};}static void*Cyc_Toc_typ_to_c_array(void*t){void*_tmp18C=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmp18E;void*_tmp18F;struct Cyc_Absyn_Tqual _tmp190;
struct Cyc_Absyn_Exp*_tmp191;union Cyc_Absyn_Constraint*_tmp192;unsigned int
_tmp193;void*_tmp195;_LL41: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp18D=(
struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18C;if(_tmp18D->tag != 8)goto
_LL43;else{_tmp18E=_tmp18D->f1;_tmp18F=_tmp18E.elt_type;_tmp190=_tmp18E.tq;
_tmp191=_tmp18E.num_elts;_tmp192=_tmp18E.zero_term;_tmp193=_tmp18E.zt_loc;}}
_LL42: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp18F),_tmp190,_tmp191,
Cyc_Absyn_false_conref,_tmp193);_LL43:{struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp194=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp18C;if(_tmp194->tag != 1)
goto _LL45;else{_tmp195=(void*)_tmp194->f2;}}if(!(_tmp195 != 0))goto _LL45;_LL44:
return Cyc_Toc_typ_to_c_array((void*)_tmp195);_LL45:;_LL46: return Cyc_Toc_typ_to_c(
t);_LL40:;}static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*
f){struct Cyc_Absyn_Aggrfield*_tmpB13;return(_tmpB13=_cycalloc(sizeof(*_tmpB13)),((
_tmpB13->name=f->name,((_tmpB13->tq=Cyc_Toc_mt_tq,((_tmpB13->type=Cyc_Toc_typ_to_c(
f->type),((_tmpB13->width=f->width,((_tmpB13->attributes=f->attributes,_tmpB13)))))))))));}
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){return;}static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0){void**_tmpB14;cs=((_tmpB14=_cycalloc(sizeof(*_tmpB14)),((
_tmpB14[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpB14))));}return*cs;}static void*Cyc_Toc_rgn_typ(){
static void**r=0;if(r == 0){void**_tmpB15;r=((_tmpB15=_cycalloc(sizeof(*_tmpB15)),((
_tmpB15[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),
_tmpB15))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0){
void**_tmpB16;r=((_tmpB16=_cycalloc(sizeof(*_tmpB16)),((_tmpB16[0]=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpB16))));}return*
r;}static int Cyc_Toc_is_boxed_tvar(void*t){void*_tmp19A=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_Tvar*_tmp19C;_LL48: {struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp19B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp19A;if(_tmp19B->tag != 2)
goto _LL4A;else{_tmp19C=_tmp19B->f1;}}_LL49: return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t),& Cyc_Tcutil_tbk);_LL4A:;_LL4B: return 0;_LL47:;}static int Cyc_Toc_is_abstract_type(
void*t){struct Cyc_Absyn_Kind*_tmp19D=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind
_tmp19E;enum Cyc_Absyn_KindQual _tmp19F;_LL4D: _tmp19E=*_tmp19D;_tmp19F=_tmp19E.kind;
if(_tmp19F != Cyc_Absyn_AnyKind)goto _LL4F;_LL4E: return 1;_LL4F:;_LL50: return 0;
_LL4C:;}static void*Cyc_Toc_typ_to_c(void*t){void*_tmp1A0=t;void*_tmp1A3;void**
_tmp1A4;struct Cyc_Absyn_Tvar*_tmp1A6;struct Cyc_Absyn_DatatypeFieldInfo _tmp1A9;
union Cyc_Absyn_DatatypeFieldInfoU _tmp1AA;struct _tuple1 _tmp1AB;struct Cyc_Absyn_Datatypedecl*
_tmp1AC;struct Cyc_Absyn_Datatypefield*_tmp1AD;struct Cyc_Absyn_PtrInfo _tmp1B0;
void*_tmp1B1;struct Cyc_Absyn_Tqual _tmp1B2;struct Cyc_Absyn_PtrAtts _tmp1B3;union
Cyc_Absyn_Constraint*_tmp1B4;struct Cyc_Absyn_ArrayInfo _tmp1B8;void*_tmp1B9;
struct Cyc_Absyn_Tqual _tmp1BA;struct Cyc_Absyn_Exp*_tmp1BB;unsigned int _tmp1BC;
struct Cyc_Absyn_FnInfo _tmp1BE;struct Cyc_Absyn_Tqual _tmp1BF;void*_tmp1C0;struct
Cyc_List_List*_tmp1C1;int _tmp1C2;struct Cyc_Absyn_VarargInfo*_tmp1C3;struct Cyc_List_List*
_tmp1C4;struct Cyc_List_List*_tmp1C6;enum Cyc_Absyn_AggrKind _tmp1C8;struct Cyc_List_List*
_tmp1C9;struct Cyc_Absyn_AggrInfo _tmp1CB;union Cyc_Absyn_AggrInfoU _tmp1CC;struct
Cyc_List_List*_tmp1CD;struct _tuple0*_tmp1CF;struct Cyc_List_List*_tmp1D1;struct
_tuple0*_tmp1D3;struct Cyc_List_List*_tmp1D4;struct Cyc_Absyn_Typedefdecl*_tmp1D5;
void*_tmp1D6;void*_tmp1D9;struct Cyc_Absyn_TypeDecl*_tmp1E3;struct Cyc_Absyn_TypeDecl
_tmp1E4;void*_tmp1E5;struct Cyc_Absyn_Aggrdecl*_tmp1E7;struct Cyc_Absyn_TypeDecl*
_tmp1E9;struct Cyc_Absyn_TypeDecl _tmp1EA;void*_tmp1EB;struct Cyc_Absyn_Enumdecl*
_tmp1ED;struct Cyc_Absyn_TypeDecl*_tmp1EF;struct Cyc_Absyn_TypeDecl _tmp1F0;void*
_tmp1F1;struct Cyc_Absyn_Datatypedecl*_tmp1F3;void**_tmp1F4;_LL52: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*
_tmp1A1=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1A0;if(_tmp1A1->tag != 0)
goto _LL54;}_LL53: return t;_LL54: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp1A2=(
struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A0;if(_tmp1A2->tag != 1)goto _LL56;
else{_tmp1A3=(void**)& _tmp1A2->f2;_tmp1A4=(void**)((void**)& _tmp1A2->f2);}}_LL55:
if(*_tmp1A4 == 0){*_tmp1A4=(void*)Cyc_Absyn_sint_typ;return Cyc_Absyn_sint_typ;}
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp1A4));_LL56: {struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp1A5=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A0;if(_tmp1A5->tag != 2)
goto _LL58;else{_tmp1A6=_tmp1A5->f1;}}_LL57: if((Cyc_Tcutil_tvar_kind(_tmp1A6,& Cyc_Tcutil_bk))->kind
== Cyc_Absyn_AnyKind)return(void*)& Cyc_Absyn_VoidType_val;else{return Cyc_Absyn_void_star_typ();}
_LL58: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1A7=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)
_tmp1A0;if(_tmp1A7->tag != 3)goto _LL5A;}_LL59: return(void*)& Cyc_Absyn_VoidType_val;
_LL5A: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1A8=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)
_tmp1A0;if(_tmp1A8->tag != 4)goto _LL5C;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A9.field_info;
if((_tmp1AA.KnownDatatypefield).tag != 2)goto _LL5C;_tmp1AB=(struct _tuple1)(
_tmp1AA.KnownDatatypefield).val;_tmp1AC=_tmp1AB.f1;_tmp1AD=_tmp1AB.f2;}}_LL5B:
return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp1AD->name,_tmp1AC->name));
_LL5C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1AE=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)
_tmp1A0;if(_tmp1AE->tag != 4)goto _LL5E;}_LL5D: {const char*_tmpB19;void*_tmpB18;(
_tmpB18=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB19="unresolved DatatypeFieldType",_tag_dyneither(_tmpB19,sizeof(char),29))),
_tag_dyneither(_tmpB18,sizeof(void*),0)));}_LL5E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp1AF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A0;if(_tmp1AF->tag
!= 5)goto _LL60;else{_tmp1B0=_tmp1AF->f1;_tmp1B1=_tmp1B0.elt_typ;_tmp1B2=_tmp1B0.elt_tq;
_tmp1B3=_tmp1B0.ptr_atts;_tmp1B4=_tmp1B3.bounds;}}_LL5F: _tmp1B1=Cyc_Toc_typ_to_c_array(
_tmp1B1);{void*_tmp1F7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmp1B4);_LL8F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp1F8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1F7;if(_tmp1F8->tag
!= 0)goto _LL91;}_LL90: return Cyc_Toc_dyneither_ptr_typ;_LL91:;_LL92: return Cyc_Absyn_star_typ(
_tmp1B1,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1B2,Cyc_Absyn_false_conref);_LL8E:;};
_LL60: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1B5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp1A0;if(_tmp1B5->tag != 6)goto _LL62;}_LL61: goto _LL63;_LL62: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp1B6=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1A0;if(_tmp1B6->tag != 
7)goto _LL64;}_LL63: return t;_LL64: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp1B7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A0;if(_tmp1B7->tag != 
8)goto _LL66;else{_tmp1B8=_tmp1B7->f1;_tmp1B9=_tmp1B8.elt_type;_tmp1BA=_tmp1B8.tq;
_tmp1BB=_tmp1B8.num_elts;_tmp1BC=_tmp1B8.zt_loc;}}_LL65: return Cyc_Absyn_array_typ(
Cyc_Toc_typ_to_c_array(_tmp1B9),_tmp1BA,_tmp1BB,Cyc_Absyn_false_conref,_tmp1BC);
_LL66: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1BD=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)
_tmp1A0;if(_tmp1BD->tag != 9)goto _LL68;else{_tmp1BE=_tmp1BD->f1;_tmp1BF=_tmp1BE.ret_tqual;
_tmp1C0=_tmp1BE.ret_typ;_tmp1C1=_tmp1BE.args;_tmp1C2=_tmp1BE.c_varargs;_tmp1C3=
_tmp1BE.cyc_varargs;_tmp1C4=_tmp1BE.attributes;}}_LL67: {struct Cyc_List_List*
_tmp1F9=0;for(0;_tmp1C4 != 0;_tmp1C4=_tmp1C4->tl){void*_tmp1FA=(void*)_tmp1C4->hd;
_LL94: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp1FB=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)
_tmp1FA;if(_tmp1FB->tag != 4)goto _LL96;}_LL95: goto _LL97;_LL96: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*
_tmp1FC=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp1FA;if(_tmp1FC->tag
!= 5)goto _LL98;}_LL97: goto _LL99;_LL98: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*
_tmp1FD=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1FA;if(_tmp1FD->tag
!= 19)goto _LL9A;}_LL99: continue;_LL9A: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*
_tmp1FE=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1FA;if(
_tmp1FE->tag != 21)goto _LL9C;}_LL9B: continue;_LL9C: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*
_tmp1FF=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1FA;if(
_tmp1FF->tag != 20)goto _LL9E;}_LL9D: continue;_LL9E:;_LL9F:{struct Cyc_List_List*
_tmpB1A;_tmp1F9=((_tmpB1A=_cycalloc(sizeof(*_tmpB1A)),((_tmpB1A->hd=(void*)
_tmp1C4->hd,((_tmpB1A->tl=_tmp1F9,_tmpB1A))))));}goto _LL93;_LL93:;}{struct Cyc_List_List*
_tmp201=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1C1);if(_tmp1C3 != 0){void*_tmp202=Cyc_Toc_typ_to_c(
Cyc_Absyn_dyneither_typ(_tmp1C3->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,
Cyc_Absyn_false_conref));struct _tuple8*_tmpB1B;struct _tuple8*_tmp203=(_tmpB1B=
_cycalloc(sizeof(*_tmpB1B)),((_tmpB1B->f1=_tmp1C3->name,((_tmpB1B->f2=_tmp1C3->tq,((
_tmpB1B->f3=_tmp202,_tmpB1B)))))));struct Cyc_List_List*_tmpB1C;_tmp201=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp201,((_tmpB1C=_cycalloc(sizeof(*_tmpB1C)),((_tmpB1C->hd=_tmp203,((_tmpB1C->tl=
0,_tmpB1C)))))));}{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpB22;struct Cyc_Absyn_FnInfo
_tmpB21;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB20;return(void*)((_tmpB20=
_cycalloc(sizeof(*_tmpB20)),((_tmpB20[0]=((_tmpB22.tag=9,((_tmpB22.f1=((_tmpB21.tvars=
0,((_tmpB21.effect=0,((_tmpB21.ret_tqual=_tmp1BF,((_tmpB21.ret_typ=Cyc_Toc_typ_to_c(
_tmp1C0),((_tmpB21.args=_tmp201,((_tmpB21.c_varargs=_tmp1C2,((_tmpB21.cyc_varargs=
0,((_tmpB21.rgn_po=0,((_tmpB21.attributes=_tmp1F9,_tmpB21)))))))))))))))))),
_tmpB22)))),_tmpB20))));};};}_LL68: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmp1C5=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1A0;if(_tmp1C5->tag != 
10)goto _LL6A;else{_tmp1C6=_tmp1C5->f1;}}_LL69: _tmp1C6=((struct Cyc_List_List*(*)(
struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,
_tmp1C6);return Cyc_Toc_add_tuple_type(_tmp1C6);_LL6A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmp1C7=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A0;if(_tmp1C7->tag
!= 12)goto _LL6C;else{_tmp1C8=_tmp1C7->f1;_tmp1C9=_tmp1C7->f2;}}_LL6B: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct
_tmpB25;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB24;return(void*)((
_tmpB24=_cycalloc(sizeof(*_tmpB24)),((_tmpB24[0]=((_tmpB25.tag=12,((_tmpB25.f1=
_tmp1C8,((_tmpB25.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,
_tmp1C9),_tmpB25)))))),_tmpB24))));}_LL6C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp1CA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A0;if(_tmp1CA->tag != 
11)goto _LL6E;else{_tmp1CB=_tmp1CA->f1;_tmp1CC=_tmp1CB.aggr_info;_tmp1CD=_tmp1CB.targs;}}
_LL6D:{union Cyc_Absyn_AggrInfoU _tmp20B=_tmp1CC;struct _tuple2 _tmp20C;_LLA1: if((
_tmp20B.UnknownAggr).tag != 1)goto _LLA3;_tmp20C=(struct _tuple2)(_tmp20B.UnknownAggr).val;
_LLA2: return t;_LLA3:;_LLA4: goto _LLA0;_LLA0:;}{struct Cyc_Absyn_Aggrdecl*_tmp20D=
Cyc_Absyn_get_known_aggrdecl(_tmp1CC);if(_tmp20D->kind == Cyc_Absyn_UnionA)return
Cyc_Toc_aggrdecl_type(_tmp20D->name,Cyc_Absyn_unionq_typ);{struct Cyc_List_List*
_tmp20E=_tmp20D->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp20D->impl))->fields;
if(_tmp20E == 0)return Cyc_Toc_aggrdecl_type(_tmp20D->name,Cyc_Absyn_strctq);for(0;
_tmp20E->tl != 0;_tmp20E=_tmp20E->tl){;}{void*_tmp20F=((struct Cyc_Absyn_Aggrfield*)
_tmp20E->hd)->type;if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(
_tmp20F)))return Cyc_Toc_add_struct_type(_tmp20D->name,_tmp20D->tvs,((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c,_tmp1CD),((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp20D->impl))->fields);return Cyc_Toc_aggrdecl_type(
_tmp20D->name,Cyc_Absyn_strctq);};};};_LL6E: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp1CE=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1A0;if(_tmp1CE->tag != 
13)goto _LL70;else{_tmp1CF=_tmp1CE->f1;}}_LL6F: return t;_LL70: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*
_tmp1D0=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1A0;if(_tmp1D0->tag
!= 14)goto _LL72;else{_tmp1D1=_tmp1D0->f1;}}_LL71: Cyc_Toc_enumfields_to_c(_tmp1D1);
return t;_LL72: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1D2=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)
_tmp1A0;if(_tmp1D2->tag != 17)goto _LL74;else{_tmp1D3=_tmp1D2->f1;_tmp1D4=_tmp1D2->f2;
_tmp1D5=_tmp1D2->f3;_tmp1D6=(void*)_tmp1D2->f4;}}_LL73: if(_tmp1D6 == 0  || Cyc_noexpand_r){
if(_tmp1D4 != 0){struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB28;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*
_tmpB27;return(void*)((_tmpB27=_cycalloc(sizeof(*_tmpB27)),((_tmpB27[0]=((
_tmpB28.tag=17,((_tmpB28.f1=_tmp1D3,((_tmpB28.f2=0,((_tmpB28.f3=_tmp1D5,((
_tmpB28.f4=(void*)0,_tmpB28)))))))))),_tmpB27))));}else{return t;}}else{struct Cyc_Absyn_TypedefType_Absyn_Type_struct
_tmpB2B;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB2A;return(void*)((
_tmpB2A=_cycalloc(sizeof(*_tmpB2A)),((_tmpB2A[0]=((_tmpB2B.tag=17,((_tmpB2B.f1=
_tmp1D3,((_tmpB2B.f2=0,((_tmpB2B.f3=_tmp1D5,((_tmpB2B.f4=(void*)((void*)Cyc_Toc_typ_to_c_array((
void*)_tmp1D6)),_tmpB2B)))))))))),_tmpB2A))));}_LL74: {struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmp1D7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1A0;if(_tmp1D7->tag != 19)
goto _LL76;}_LL75: return Cyc_Absyn_uint_typ;_LL76: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*
_tmp1D8=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A0;if(_tmp1D8->tag
!= 15)goto _LL78;else{_tmp1D9=(void*)_tmp1D8->f1;}}_LL77: return Cyc_Toc_rgn_typ();
_LL78: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1DA=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)
_tmp1A0;if(_tmp1DA->tag != 16)goto _LL7A;}_LL79: return Cyc_Toc_dyn_rgn_typ();_LL7A: {
struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp1DB=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)
_tmp1A0;if(_tmp1DB->tag != 20)goto _LL7C;}_LL7B: goto _LL7D;_LL7C: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*
_tmp1DC=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp1A0;if(_tmp1DC->tag != 
21)goto _LL7E;}_LL7D: goto _LL7F;_LL7E: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*
_tmp1DD=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp1A0;if(_tmp1DD->tag != 
22)goto _LL80;}_LL7F: goto _LL81;_LL80: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*
_tmp1DE=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1A0;if(_tmp1DE->tag != 
23)goto _LL82;}_LL81: goto _LL83;_LL82: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmp1DF=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1A0;if(_tmp1DF->tag != 24)
goto _LL84;}_LL83: goto _LL85;_LL84: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*
_tmp1E0=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1A0;if(_tmp1E0->tag != 25)
goto _LL86;}_LL85: goto _LL87;_LL86: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*
_tmp1E1=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1A0;if(_tmp1E1->tag
!= 18)goto _LL88;}_LL87: return Cyc_Absyn_void_star_typ();_LL88: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*
_tmp1E2=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A0;if(_tmp1E2->tag
!= 26)goto _LL8A;else{_tmp1E3=_tmp1E2->f1;_tmp1E4=*_tmp1E3;_tmp1E5=_tmp1E4.r;{
struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp1E6=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)
_tmp1E5;if(_tmp1E6->tag != 0)goto _LL8A;else{_tmp1E7=_tmp1E6->f1;}};}}_LL89: Cyc_Toc_aggrdecl_to_c(
_tmp1E7,1);if(_tmp1E7->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(
_tmp1E7->name,Cyc_Absyn_unionq_typ);else{return Cyc_Toc_aggrdecl_type(_tmp1E7->name,
Cyc_Absyn_strctq);}_LL8A: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*
_tmp1E8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A0;if(_tmp1E8->tag
!= 26)goto _LL8C;else{_tmp1E9=_tmp1E8->f1;_tmp1EA=*_tmp1E9;_tmp1EB=_tmp1EA.r;{
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp1EC=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)
_tmp1EB;if(_tmp1EC->tag != 1)goto _LL8C;else{_tmp1ED=_tmp1EC->f1;}};}}_LL8B: Cyc_Toc_enumdecl_to_c(
_tmp1ED);return t;_LL8C: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1EE=(
struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A0;if(_tmp1EE->tag != 26)
goto _LL51;else{_tmp1EF=_tmp1EE->f1;_tmp1F0=*_tmp1EF;_tmp1F1=_tmp1F0.r;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*
_tmp1F2=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp1F1;if(
_tmp1F2->tag != 2)goto _LL51;else{_tmp1F3=_tmp1F2->f1;}};_tmp1F4=_tmp1EE->f2;}}
_LL8D: Cyc_Toc_datatypedecl_to_c(_tmp1F3);return Cyc_Toc_typ_to_c(*((void**)
_check_null(_tmp1F4)));_LL51:;}static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(
void*t,struct Cyc_Absyn_Exp*e,unsigned int l){void*_tmp214=t;struct Cyc_Absyn_ArrayInfo
_tmp216;void*_tmp217;struct Cyc_Absyn_Tqual _tmp218;_LLA6: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp215=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp214;if(_tmp215->tag != 
8)goto _LLA8;else{_tmp216=_tmp215->f1;_tmp217=_tmp216.elt_type;_tmp218=_tmp216.tq;}}
_LLA7: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp217,(void*)& Cyc_Absyn_HeapRgn_val,
_tmp218,Cyc_Absyn_false_conref),e);_LLA8:;_LLA9: return Cyc_Toc_cast_it(t,e);_LLA5:;}
static int Cyc_Toc_atomic_typ(void*t){void*_tmp219=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_ArrayInfo _tmp223;void*_tmp224;struct Cyc_Absyn_AggrInfo _tmp226;union Cyc_Absyn_AggrInfoU
_tmp227;struct Cyc_List_List*_tmp229;struct Cyc_Absyn_DatatypeFieldInfo _tmp22B;
union Cyc_Absyn_DatatypeFieldInfoU _tmp22C;struct _tuple1 _tmp22D;struct Cyc_Absyn_Datatypedecl*
_tmp22E;struct Cyc_Absyn_Datatypefield*_tmp22F;struct Cyc_List_List*_tmp231;_LLAB: {
struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp21A=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)
_tmp219;if(_tmp21A->tag != 0)goto _LLAD;}_LLAC: return 1;_LLAD: {struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp21B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp219;if(_tmp21B->tag != 2)
goto _LLAF;}_LLAE: return 0;_LLAF: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp21C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp219;if(_tmp21C->tag != 6)
goto _LLB1;}_LLB0: goto _LLB2;_LLB1: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp21D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp219;if(_tmp21D->tag != 
13)goto _LLB3;}_LLB2: goto _LLB4;_LLB3: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*
_tmp21E=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp219;if(_tmp21E->tag
!= 14)goto _LLB5;}_LLB4: goto _LLB6;_LLB5: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp21F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp219;if(_tmp21F->tag != 
7)goto _LLB7;}_LLB6: goto _LLB8;_LLB7: {struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp220=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp219;if(_tmp220->tag != 9)
goto _LLB9;}_LLB8: goto _LLBA;_LLB9: {struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmp221=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp219;if(_tmp221->tag != 19)
goto _LLBB;}_LLBA: return 1;_LLBB: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp222=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp219;if(_tmp222->tag != 
8)goto _LLBD;else{_tmp223=_tmp222->f1;_tmp224=_tmp223.elt_type;}}_LLBC: return Cyc_Toc_atomic_typ(
_tmp224);_LLBD: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp225=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp219;if(_tmp225->tag != 11)goto _LLBF;else{_tmp226=_tmp225->f1;_tmp227=_tmp226.aggr_info;}}
_LLBE:{union Cyc_Absyn_AggrInfoU _tmp236=_tmp227;struct _tuple2 _tmp237;_LLD0: if((
_tmp236.UnknownAggr).tag != 1)goto _LLD2;_tmp237=(struct _tuple2)(_tmp236.UnknownAggr).val;
_LLD1: return 0;_LLD2:;_LLD3: goto _LLCF;_LLCF:;}{struct Cyc_Absyn_Aggrdecl*_tmp238=
Cyc_Absyn_get_known_aggrdecl(_tmp227);if(_tmp238->impl == 0)return 0;{struct Cyc_List_List*
_tmp239=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp238->impl))->fields;for(
0;_tmp239 != 0;_tmp239=_tmp239->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)
_tmp239->hd)->type))return 0;}}return 1;};_LLBF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmp228=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp219;if(_tmp228->tag
!= 12)goto _LLC1;else{_tmp229=_tmp228->f2;}}_LLC0: for(0;_tmp229 != 0;_tmp229=
_tmp229->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp229->hd)->type))
return 0;}return 1;_LLC1: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmp22A=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp219;if(_tmp22A->tag
!= 4)goto _LLC3;else{_tmp22B=_tmp22A->f1;_tmp22C=_tmp22B.field_info;if((_tmp22C.KnownDatatypefield).tag
!= 2)goto _LLC3;_tmp22D=(struct _tuple1)(_tmp22C.KnownDatatypefield).val;_tmp22E=
_tmp22D.f1;_tmp22F=_tmp22D.f2;}}_LLC2: _tmp231=_tmp22F->typs;goto _LLC4;_LLC3: {
struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp230=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)
_tmp219;if(_tmp230->tag != 10)goto _LLC5;else{_tmp231=_tmp230->f1;}}_LLC4: for(0;
_tmp231 != 0;_tmp231=_tmp231->tl){if(!Cyc_Toc_atomic_typ((*((struct _tuple10*)
_tmp231->hd)).f2))return 0;}return 1;_LLC5: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*
_tmp232=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp219;if(_tmp232->tag
!= 3)goto _LLC7;}_LLC6: goto _LLC8;_LLC7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp233=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp219;if(_tmp233->tag
!= 5)goto _LLC9;}_LLC8: goto _LLCA;_LLC9: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*
_tmp234=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp219;if(_tmp234->tag != 
16)goto _LLCB;}_LLCA: goto _LLCC;_LLCB: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*
_tmp235=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp219;if(_tmp235->tag
!= 15)goto _LLCD;}_LLCC: return 0;_LLCD:;_LLCE: {const char*_tmpB2F;void*_tmpB2E[1];
struct Cyc_String_pa_PrintArg_struct _tmpB2D;(_tmpB2D.tag=0,((_tmpB2D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB2E[0]=&
_tmpB2D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB2F="atomic_typ:  bad type %s",_tag_dyneither(_tmpB2F,sizeof(char),25))),
_tag_dyneither(_tmpB2E,sizeof(void*),1)))))));}_LLAA:;}static int Cyc_Toc_is_void_star(
void*t){void*_tmp23D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp23F;void*
_tmp240;_LLD5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp23E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp23D;if(_tmp23E->tag != 5)goto _LLD7;else{_tmp23F=_tmp23E->f1;_tmp240=_tmp23F.elt_typ;}}
_LLD6: {void*_tmp241=Cyc_Tcutil_compress(_tmp240);_LLDA: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*
_tmp242=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp241;if(_tmp242->tag != 0)
goto _LLDC;}_LLDB: return 1;_LLDC:;_LLDD: return 0;_LLD9:;}_LLD7:;_LLD8: return 0;_LLD4:;}
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){return Cyc_Toc_is_void_star(t)
 || Cyc_Toc_is_boxed_tvar(t);}static int Cyc_Toc_is_poly_field(void*t,struct
_dyneither_ptr*f){void*_tmp243=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp245;union Cyc_Absyn_AggrInfoU _tmp246;struct Cyc_List_List*_tmp248;_LLDF: {
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp244=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp243;if(_tmp244->tag != 11)goto _LLE1;else{_tmp245=_tmp244->f1;_tmp246=_tmp245.aggr_info;}}
_LLE0: {struct Cyc_Absyn_Aggrdecl*_tmp249=Cyc_Absyn_get_known_aggrdecl(_tmp246);
if(_tmp249->impl == 0){const char*_tmpB32;void*_tmpB31;(_tmpB31=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB32="is_poly_field: type missing fields",
_tag_dyneither(_tmpB32,sizeof(char),35))),_tag_dyneither(_tmpB31,sizeof(void*),0)));}
_tmp248=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp249->impl))->fields;goto
_LLE2;}_LLE1: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp247=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)
_tmp243;if(_tmp247->tag != 12)goto _LLE3;else{_tmp248=_tmp247->f2;}}_LLE2: {struct
Cyc_Absyn_Aggrfield*_tmp24C=Cyc_Absyn_lookup_field(_tmp248,f);if(_tmp24C == 0){
const char*_tmpB36;void*_tmpB35[1];struct Cyc_String_pa_PrintArg_struct _tmpB34;(
_tmpB34.tag=0,((_tmpB34.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmpB35[0]=& _tmpB34,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB36="is_poly_field: bad field %s",_tag_dyneither(_tmpB36,sizeof(char),28))),
_tag_dyneither(_tmpB35,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_boxed_tvar(
_tmp24C->type);}_LLE3:;_LLE4: {const char*_tmpB3A;void*_tmpB39[1];struct Cyc_String_pa_PrintArg_struct
_tmpB38;(_tmpB38.tag=0,((_tmpB38.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpB39[0]=& _tmpB38,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3A="is_poly_field: bad type %s",
_tag_dyneither(_tmpB3A,sizeof(char),27))),_tag_dyneither(_tmpB39,sizeof(void*),1)))))));}
_LLDE:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp253=e->r;
struct Cyc_Absyn_Exp*_tmp255;struct _dyneither_ptr*_tmp256;struct Cyc_Absyn_Exp*
_tmp258;struct _dyneither_ptr*_tmp259;_LLE6: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*
_tmp254=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp253;if(_tmp254->tag
!= 20)goto _LLE8;else{_tmp255=_tmp254->f1;_tmp256=_tmp254->f2;}}_LLE7: return Cyc_Toc_is_poly_field((
void*)_check_null(_tmp255->topt),_tmp256);_LLE8: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*
_tmp257=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp253;if(_tmp257->tag
!= 21)goto _LLEA;else{_tmp258=_tmp257->f1;_tmp259=_tmp257->f2;}}_LLE9: {void*
_tmp25A=Cyc_Tcutil_compress((void*)_check_null(_tmp258->topt));struct Cyc_Absyn_PtrInfo
_tmp25C;void*_tmp25D;_LLED: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp25B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25A;if(_tmp25B->tag
!= 5)goto _LLEF;else{_tmp25C=_tmp25B->f1;_tmp25D=_tmp25C.elt_typ;}}_LLEE: return
Cyc_Toc_is_poly_field(_tmp25D,_tmp259);_LLEF:;_LLF0: {const char*_tmpB3E;void*
_tmpB3D[1];struct Cyc_String_pa_PrintArg_struct _tmpB3C;(_tmpB3C.tag=0,((_tmpB3C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)
_check_null(_tmp258->topt))),((_tmpB3D[0]=& _tmpB3C,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3E="is_poly_project: bad type %s",
_tag_dyneither(_tmpB3E,sizeof(char),29))),_tag_dyneither(_tmpB3D,sizeof(void*),1)))))));}
_LLEC:;}_LLEA:;_LLEB: return 0;_LLE5:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp*s){struct Cyc_List_List*_tmpB3F;return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_e,((_tmpB3F=_cycalloc(sizeof(*_tmpB3F)),((_tmpB3F->hd=s,((
_tmpB3F->tl=0,_tmpB3F)))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp*s){struct Cyc_List_List*_tmpB40;return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_atomic_e,((_tmpB40=_cycalloc(sizeof(*_tmpB40)),((_tmpB40->hd=s,((
_tmpB40->tl=0,_tmpB40)))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(
void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(
s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmpB41[2];
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpB41[1]=s,((_tmpB41[0]=
rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB41,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*_tmpB42[2];return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_atomic_e,((_tmpB42[1]=n,((_tmpB42[0]=s,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB42,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}else{struct Cyc_Absyn_Exp*_tmpB43[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((
_tmpB43[1]=n,((_tmpB43[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB43,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n){struct Cyc_Absyn_Exp*_tmpB44[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((
_tmpB44[2]=n,((_tmpB44[1]=s,((_tmpB44[0]=rgn,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB44,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpB45;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpB45=_cycalloc(sizeof(*_tmpB45)),((_tmpB45->hd=e,((_tmpB45->tl=0,_tmpB45)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){struct Cyc_Absyn_Exp*
_tmpB46[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpB46,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*
_tmp269=e->r;union Cyc_Absyn_Cnst _tmp26B;struct _dyneither_ptr _tmp26C;union Cyc_Absyn_Cnst
_tmp26E;struct _dyneither_ptr _tmp26F;_LLF2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp26A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp26A->tag != 
0)goto _LLF4;else{_tmp26B=_tmp26A->f1;if((_tmp26B.String_c).tag != 8)goto _LLF4;
_tmp26C=(struct _dyneither_ptr)(_tmp26B.String_c).val;}}_LLF3: is_string=1;goto
_LLF1;_LLF4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp26D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmp269;if(_tmp26D->tag != 0)goto _LLF6;else{_tmp26E=_tmp26D->f1;if((_tmp26E.Wstring_c).tag
!= 9)goto _LLF6;_tmp26F=(struct _dyneither_ptr)(_tmp26E.Wstring_c).val;}}_LLF5:
is_string=1;goto _LLF1;_LLF6:;_LLF7: goto _LLF1;_LLF1:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple0*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*
_tmpB4C;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB4B;struct Cyc_List_List*
_tmpB4A;Cyc_Toc_result_decls=((_tmpB4A=_cycalloc(sizeof(*_tmpB4A)),((_tmpB4A->hd=
Cyc_Absyn_new_decl((void*)((_tmpB4C=_cycalloc(sizeof(*_tmpB4C)),((_tmpB4C[0]=((
_tmpB4B.tag=0,((_tmpB4B.f1=vd,_tmpB4B)))),_tmpB4C)))),0),((_tmpB4A->tl=Cyc_Toc_result_decls,
_tmpB4A))))));}xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}
else{xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*
urm_exp;{struct _tuple16*_tmpB53;struct _tuple16*_tmpB52;struct _tuple16*_tmpB51;
struct _tuple16*_tmpB50[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpB50[2]=((
_tmpB51=_cycalloc(sizeof(*_tmpB51)),((_tmpB51->f1=0,((_tmpB51->f2=xplussz,
_tmpB51)))))),((_tmpB50[1]=((_tmpB52=_cycalloc(sizeof(*_tmpB52)),((_tmpB52->f1=0,((
_tmpB52->f2=xexp,_tmpB52)))))),((_tmpB50[0]=((_tmpB53=_cycalloc(sizeof(*_tmpB53)),((
_tmpB53->f1=0,((_tmpB53->f2=xexp,_tmpB53)))))),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB50,sizeof(struct _tuple16*),3)))))))),
0);}return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;
struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{
struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct
_RegionHandle*rgn;};typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;static int Cyc_Toc_is_toplevel(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp278;int _tmp279;struct Cyc_Toc_Env*
_tmp277=nv;_tmp278=*_tmp277;_tmp279=_tmp278.toplevel;return _tmp279;}static int Cyc_Toc_in_lhs(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp27B;int*_tmp27C;struct Cyc_Toc_Env*
_tmp27A=nv;_tmp27B=*_tmp27A;_tmp27C=_tmp27B.in_lhs;return*_tmp27C;}static struct
Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple0*x){struct
Cyc_Toc_Env _tmp27E;struct Cyc_Dict_Dict _tmp27F;struct Cyc_Toc_Env*_tmp27D=nv;
_tmp27E=*_tmp27D;_tmp27F=_tmp27E.varmap;return((struct Cyc_Absyn_Exp*(*)(struct
Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp27F,x);}static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(struct _RegionHandle*r){int*_tmpB56;struct Cyc_Toc_Env*_tmpB55;
return(_tmpB55=_region_malloc(r,sizeof(*_tmpB55)),((_tmpB55->break_lab=(struct
_dyneither_ptr**)0,((_tmpB55->continue_lab=(struct _dyneither_ptr**)0,((_tmpB55->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB55->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp),((_tmpB55->toplevel=(int)1,((_tmpB55->in_lhs=(int*)((
_tmpB56=_region_malloc(r,sizeof(*_tmpB56)),((_tmpB56[0]=0,_tmpB56)))),((_tmpB55->rgn=(
struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB55)))))))))))))));}static struct
Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct
Cyc_Toc_Env _tmp283;struct _dyneither_ptr**_tmp284;struct _dyneither_ptr**_tmp285;
struct Cyc_Toc_FallthruInfo*_tmp286;struct Cyc_Dict_Dict _tmp287;int _tmp288;int*
_tmp289;struct Cyc_Toc_Env*_tmp282=e;_tmp283=*_tmp282;_tmp284=_tmp283.break_lab;
_tmp285=_tmp283.continue_lab;_tmp286=_tmp283.fallthru_info;_tmp287=_tmp283.varmap;
_tmp288=_tmp283.toplevel;_tmp289=_tmp283.in_lhs;{struct Cyc_Toc_Env*_tmpB57;
return(_tmpB57=_region_malloc(r,sizeof(*_tmpB57)),((_tmpB57->break_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp284),((_tmpB57->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp285),((_tmpB57->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp286,((_tmpB57->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp287),((_tmpB57->toplevel=(int)_tmp288,((_tmpB57->in_lhs=(int*)_tmp289,((
_tmpB57->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB57)))))))))))))));};}
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp28C;struct _dyneither_ptr**_tmp28D;struct _dyneither_ptr**
_tmp28E;struct Cyc_Toc_FallthruInfo*_tmp28F;struct Cyc_Dict_Dict _tmp290;int _tmp291;
int*_tmp292;struct Cyc_Toc_Env*_tmp28B=e;_tmp28C=*_tmp28B;_tmp28D=_tmp28C.break_lab;
_tmp28E=_tmp28C.continue_lab;_tmp28F=_tmp28C.fallthru_info;_tmp290=_tmp28C.varmap;
_tmp291=_tmp28C.toplevel;_tmp292=_tmp28C.in_lhs;{struct Cyc_Toc_Env*_tmpB58;
return(_tmpB58=_region_malloc(r,sizeof(*_tmpB58)),((_tmpB58->break_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp28D),((_tmpB58->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp28E),((_tmpB58->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp28F,((_tmpB58->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp290),((_tmpB58->toplevel=(int)0,((_tmpB58->in_lhs=(int*)_tmp292,((_tmpB58->rgn=(
struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB58)))))))))))))));};}static
struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env _tmp295;struct _dyneither_ptr**_tmp296;struct _dyneither_ptr**
_tmp297;struct Cyc_Toc_FallthruInfo*_tmp298;struct Cyc_Dict_Dict _tmp299;int _tmp29A;
int*_tmp29B;struct Cyc_Toc_Env*_tmp294=e;_tmp295=*_tmp294;_tmp296=_tmp295.break_lab;
_tmp297=_tmp295.continue_lab;_tmp298=_tmp295.fallthru_info;_tmp299=_tmp295.varmap;
_tmp29A=_tmp295.toplevel;_tmp29B=_tmp295.in_lhs;{struct Cyc_Toc_Env*_tmpB59;
return(_tmpB59=_region_malloc(r,sizeof(*_tmpB59)),((_tmpB59->break_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp296),((_tmpB59->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp297),((_tmpB59->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp298,((_tmpB59->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp299),((_tmpB59->toplevel=(int)1,((_tmpB59->in_lhs=(int*)_tmp29B,((_tmpB59->rgn=(
struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB59)))))))))))))));};}static void
Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){struct Cyc_Toc_Env _tmp29E;int*_tmp29F;
struct Cyc_Toc_Env*_tmp29D=e;_tmp29E=*_tmp29D;_tmp29F=_tmp29E.in_lhs;*_tmp29F=b;}
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{union Cyc_Absyn_Nmspace _tmp2A0=(*x).f1;
struct Cyc_List_List*_tmp2A1;_LLF9: if((_tmp2A0.Rel_n).tag != 1)goto _LLFB;_tmp2A1=(
struct Cyc_List_List*)(_tmp2A0.Rel_n).val;_LLFA: {const char*_tmpB5D;void*_tmpB5C[
1];struct Cyc_String_pa_PrintArg_struct _tmpB5B;(_tmpB5B.tag=0,((_tmpB5B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((
_tmpB5C[0]=& _tmpB5B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB5D="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpB5D,sizeof(char),30))),
_tag_dyneither(_tmpB5C,sizeof(void*),1)))))));}_LLFB:;_LLFC: goto _LLF8;_LLF8:;}{
struct Cyc_Toc_Env _tmp2A6;struct _dyneither_ptr**_tmp2A7;struct _dyneither_ptr**
_tmp2A8;struct Cyc_Toc_FallthruInfo*_tmp2A9;struct Cyc_Dict_Dict _tmp2AA;int _tmp2AB;
int*_tmp2AC;struct Cyc_Toc_Env*_tmp2A5=e;_tmp2A6=*_tmp2A5;_tmp2A7=_tmp2A6.break_lab;
_tmp2A8=_tmp2A6.continue_lab;_tmp2A9=_tmp2A6.fallthru_info;_tmp2AA=_tmp2A6.varmap;
_tmp2AB=_tmp2A6.toplevel;_tmp2AC=_tmp2A6.in_lhs;{struct Cyc_Dict_Dict _tmp2AD=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp2AA),x,y);struct Cyc_Toc_Env*_tmpB5E;return(_tmpB5E=
_region_malloc(r,sizeof(*_tmpB5E)),((_tmpB5E->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp2A7),((_tmpB5E->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp2A8),((_tmpB5E->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp2A9,((_tmpB5E->varmap=(struct Cyc_Dict_Dict)_tmp2AD,((_tmpB5E->toplevel=(int)
_tmp2AB,((_tmpB5E->in_lhs=(int*)_tmp2AC,((_tmpB5E->rgn=(struct _RegionHandle*)r,(
struct Cyc_Toc_Env*)_tmpB5E)))))))))))))));};};}static struct Cyc_Toc_Env*Cyc_Toc_loop_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp2B0;struct
_dyneither_ptr**_tmp2B1;struct _dyneither_ptr**_tmp2B2;struct Cyc_Toc_FallthruInfo*
_tmp2B3;struct Cyc_Dict_Dict _tmp2B4;int _tmp2B5;int*_tmp2B6;struct Cyc_Toc_Env*
_tmp2AF=e;_tmp2B0=*_tmp2AF;_tmp2B1=_tmp2B0.break_lab;_tmp2B2=_tmp2B0.continue_lab;
_tmp2B3=_tmp2B0.fallthru_info;_tmp2B4=_tmp2B0.varmap;_tmp2B5=_tmp2B0.toplevel;
_tmp2B6=_tmp2B0.in_lhs;{struct Cyc_Toc_Env*_tmpB5F;return(_tmpB5F=_region_malloc(
r,sizeof(*_tmpB5F)),((_tmpB5F->break_lab=(struct _dyneither_ptr**)0,((_tmpB5F->continue_lab=(
struct _dyneither_ptr**)0,((_tmpB5F->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp2B3,((_tmpB5F->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B4),((_tmpB5F->toplevel=(
int)_tmp2B5,((_tmpB5F->in_lhs=(int*)_tmp2B6,((_tmpB5F->rgn=(struct _RegionHandle*)
r,(struct Cyc_Toc_Env*)_tmpB5F)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpB60;fallthru_vars=((
_tmpB60=_region_malloc(r,sizeof(*_tmpB60)),((_tmpB60->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpB60->tl=fallthru_vars,_tmpB60))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp2BA;struct _dyneither_ptr**_tmp2BB;struct
_dyneither_ptr**_tmp2BC;struct Cyc_Toc_FallthruInfo*_tmp2BD;struct Cyc_Dict_Dict
_tmp2BE;int _tmp2BF;int*_tmp2C0;struct Cyc_Toc_Env*_tmp2B9=e;_tmp2BA=*_tmp2B9;
_tmp2BB=_tmp2BA.break_lab;_tmp2BC=_tmp2BA.continue_lab;_tmp2BD=_tmp2BA.fallthru_info;
_tmp2BE=_tmp2BA.varmap;_tmp2BF=_tmp2BA.toplevel;_tmp2C0=_tmp2BA.in_lhs;{struct
Cyc_Toc_Env _tmp2C2;struct Cyc_Dict_Dict _tmp2C3;struct Cyc_Toc_Env*_tmp2C1=
next_case_env;_tmp2C2=*_tmp2C1;_tmp2C3=_tmp2C2.varmap;{struct Cyc_Toc_FallthruInfo*
_tmpB61;struct Cyc_Toc_FallthruInfo*fi=(_tmpB61=_region_malloc(r,sizeof(*_tmpB61)),((
_tmpB61->label=fallthru_l,((_tmpB61->binders=fallthru_vars,((_tmpB61->next_case_env=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp2C3),_tmpB61)))))));struct _dyneither_ptr**_tmpB64;struct Cyc_Toc_Env*
_tmpB63;return(_tmpB63=_region_malloc(r,sizeof(*_tmpB63)),((_tmpB63->break_lab=(
struct _dyneither_ptr**)((_tmpB64=_region_malloc(r,sizeof(*_tmpB64)),((_tmpB64[0]=
break_l,_tmpB64)))),((_tmpB63->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp2BC),((_tmpB63->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
fi,((_tmpB63->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2BE),((_tmpB63->toplevel=(
int)_tmp2BF,((_tmpB63->in_lhs=(int*)((int*)_tmp2C0),((_tmpB63->rgn=(struct
_RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB63)))))))))))))));};};};}static struct
Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _dyneither_ptr*break_l){struct Cyc_Toc_Env _tmp2C8;struct _dyneither_ptr**
_tmp2C9;struct _dyneither_ptr**_tmp2CA;struct Cyc_Toc_FallthruInfo*_tmp2CB;struct
Cyc_Dict_Dict _tmp2CC;int _tmp2CD;int*_tmp2CE;struct Cyc_Toc_Env*_tmp2C7=e;_tmp2C8=*
_tmp2C7;_tmp2C9=_tmp2C8.break_lab;_tmp2CA=_tmp2C8.continue_lab;_tmp2CB=_tmp2C8.fallthru_info;
_tmp2CC=_tmp2C8.varmap;_tmp2CD=_tmp2C8.toplevel;_tmp2CE=_tmp2C8.in_lhs;{struct
_dyneither_ptr**_tmpB67;struct Cyc_Toc_Env*_tmpB66;return(_tmpB66=_region_malloc(
r,sizeof(*_tmpB66)),((_tmpB66->break_lab=(struct _dyneither_ptr**)((_tmpB67=
_region_malloc(r,sizeof(*_tmpB67)),((_tmpB67[0]=break_l,_tmpB67)))),((_tmpB66->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp2CA),((_tmpB66->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB66->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2CC),((_tmpB66->toplevel=(
int)_tmp2CD,((_tmpB66->in_lhs=(int*)_tmp2CE,((_tmpB66->rgn=(struct _RegionHandle*)
r,(struct Cyc_Toc_Env*)_tmpB66)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp2D2;struct _dyneither_ptr**_tmp2D3;struct _dyneither_ptr**_tmp2D4;struct Cyc_Toc_FallthruInfo*
_tmp2D5;struct Cyc_Dict_Dict _tmp2D6;int _tmp2D7;int*_tmp2D8;struct Cyc_Toc_Env*
_tmp2D1=e;_tmp2D2=*_tmp2D1;_tmp2D3=_tmp2D2.break_lab;_tmp2D4=_tmp2D2.continue_lab;
_tmp2D5=_tmp2D2.fallthru_info;_tmp2D6=_tmp2D2.varmap;_tmp2D7=_tmp2D2.toplevel;
_tmp2D8=_tmp2D2.in_lhs;{struct Cyc_Toc_FallthruInfo*_tmpB6A;struct Cyc_Toc_Env*
_tmpB69;return(_tmpB69=_region_malloc(r,sizeof(*_tmpB69)),((_tmpB69->break_lab=(
struct _dyneither_ptr**)0,((_tmpB69->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp2D4),((_tmpB69->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((
_tmpB6A=_region_malloc(r,sizeof(*_tmpB6A)),((_tmpB6A->label=next_l,((_tmpB6A->binders=
0,((_tmpB6A->next_case_env=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*
cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),
_tmpB6A)))))))),((_tmpB69->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2D6),((_tmpB69->toplevel=(
int)_tmp2D7,((_tmpB69->in_lhs=(int*)((int*)_tmp2D8),((_tmpB69->rgn=(struct
_RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB69)))))))))))))));};}static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct
Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static int Cyc_Toc_need_null_check(struct
Cyc_Absyn_Exp*e){void*_tmp2DB=e->annot;_LLFE: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*
_tmp2DC=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2DB;if(
_tmp2DC->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL100;}_LLFF: return Cyc_Toc_is_nullable((
void*)_check_null(e->topt));_LL100: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*
_tmp2DD=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2DB;if(
_tmp2DD->tag != Cyc_CfFlowInfo_NotZero)goto _LL102;}_LL101: return 0;_LL102: {struct
Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2DE=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)
_tmp2DB;if(_tmp2DE->tag != Cyc_CfFlowInfo_IsZero)goto _LL104;}_LL103:{const char*
_tmpB6D;void*_tmpB6C;(_tmpB6C=0,Cyc_Tcutil_terr(e->loc,((_tmpB6D="dereference of NULL pointer",
_tag_dyneither(_tmpB6D,sizeof(char),28))),_tag_dyneither(_tmpB6C,sizeof(void*),0)));}
return 0;_LL104: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2DF=(
struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2DB;if(_tmp2DF->tag != 
Cyc_Absyn_EmptyAnnot)goto _LL106;}_LL105: return 0;_LL106:;_LL107: {const char*
_tmpB70;void*_tmpB6F;(_tmpB6F=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB70="need_null_check",_tag_dyneither(
_tmpB70,sizeof(char),16))),_tag_dyneither(_tmpB6F,sizeof(void*),0)));}_LLFD:;}
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*_tmp2E4=
e->annot;struct Cyc_List_List*_tmp2E6;struct Cyc_List_List*_tmp2E8;_LL109: {struct
Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2E5=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)
_tmp2E4;if(_tmp2E5->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL10B;else{_tmp2E6=
_tmp2E5->f1;}}_LL10A: return _tmp2E6;_LL10B: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*
_tmp2E7=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2E4;if(
_tmp2E7->tag != Cyc_CfFlowInfo_NotZero)goto _LL10D;else{_tmp2E8=_tmp2E7->f1;}}
_LL10C: return _tmp2E8;_LL10D: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*
_tmp2E9=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2E4;if(_tmp2E9->tag
!= Cyc_CfFlowInfo_IsZero)goto _LL10F;}_LL10E:{const char*_tmpB73;void*_tmpB72;(
_tmpB72=0,Cyc_Tcutil_terr(e->loc,((_tmpB73="dereference of NULL pointer",
_tag_dyneither(_tmpB73,sizeof(char),28))),_tag_dyneither(_tmpB72,sizeof(void*),0)));}
return 0;_LL10F: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2EA=(
struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2E4;if(_tmp2EA->tag != 
Cyc_Absyn_EmptyAnnot)goto _LL111;}_LL110: return 0;_LL111:;_LL112: {const char*
_tmpB76;void*_tmpB75;(_tmpB75=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB76="get_relns",_tag_dyneither(
_tmpB76,sizeof(char),10))),_tag_dyneither(_tmpB75,sizeof(void*),0)));}_LL108:;}
static int Cyc_Toc_check_const_array(unsigned int i,void*t){void*_tmp2EF=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp2F1;struct Cyc_Absyn_PtrAtts _tmp2F2;union Cyc_Absyn_Constraint*
_tmp2F3;union Cyc_Absyn_Constraint*_tmp2F4;struct Cyc_Absyn_ArrayInfo _tmp2F6;
struct Cyc_Absyn_Exp*_tmp2F7;_LL114: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp2F0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EF;if(_tmp2F0->tag
!= 5)goto _LL116;else{_tmp2F1=_tmp2F0->f1;_tmp2F2=_tmp2F1.ptr_atts;_tmp2F3=
_tmp2F2.bounds;_tmp2F4=_tmp2F2.zero_term;}}_LL115: {void*_tmp2F8=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp2F3);struct Cyc_Absyn_Exp*_tmp2FB;_LL11B: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp2F9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2F8;if(_tmp2F9->tag
!= 0)goto _LL11D;}_LL11C: return 0;_LL11D: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp2FA=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2F8;if(_tmp2FA->tag != 
1)goto _LL11A;else{_tmp2FB=_tmp2FA->f1;}}_LL11E: {unsigned int _tmp2FD;int _tmp2FE;
struct _tuple11 _tmp2FC=Cyc_Evexp_eval_const_uint_exp(_tmp2FB);_tmp2FD=_tmp2FC.f1;
_tmp2FE=_tmp2FC.f2;return _tmp2FE  && i <= _tmp2FD;}_LL11A:;}_LL116: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp2F5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2EF;if(_tmp2F5->tag != 
8)goto _LL118;else{_tmp2F6=_tmp2F5->f1;_tmp2F7=_tmp2F6.num_elts;}}_LL117: if(
_tmp2F7 == 0)return 0;{unsigned int _tmp300;int _tmp301;struct _tuple11 _tmp2FF=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp2F7);_tmp300=_tmp2FF.f1;_tmp301=_tmp2FF.f2;return
_tmp301  && i <= _tmp300;};_LL118:;_LL119: return 0;_LL113:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*_tmp302=(struct Cyc_CfFlowInfo_Reln*)
relns->hd;if(_tmp302->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp _tmp303=_tmp302->rop;
struct Cyc_Absyn_Vardecl*_tmp304;struct Cyc_Absyn_Vardecl*_tmp305;_LL120: if((
_tmp303.LessNumelts).tag != 3)goto _LL122;_tmp304=(struct Cyc_Absyn_Vardecl*)(
_tmp303.LessNumelts).val;_LL121: _tmp305=_tmp304;goto _LL123;_LL122: if((_tmp303.LessEqNumelts).tag
!= 5)goto _LL124;_tmp305=(struct Cyc_Absyn_Vardecl*)(_tmp303.LessEqNumelts).val;
_LL123: if(_tmp305 == v)return 1;else{goto _LL11F;}_LL124:;_LL125: continue;_LL11F:;};}
return 0;}static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Exp*e,void*vtype){{void*_tmp306=e->r;void*_tmp308;struct Cyc_Absyn_Vardecl*
_tmp30A;void*_tmp30C;struct Cyc_Absyn_Vardecl*_tmp30E;void*_tmp310;struct Cyc_Absyn_Vardecl*
_tmp312;void*_tmp314;struct Cyc_Absyn_Vardecl*_tmp316;union Cyc_Absyn_Cnst _tmp318;
struct _tuple5 _tmp319;enum Cyc_Absyn_Sign _tmp31A;int _tmp31B;union Cyc_Absyn_Cnst
_tmp31D;struct _tuple5 _tmp31E;enum Cyc_Absyn_Sign _tmp31F;int _tmp320;union Cyc_Absyn_Cnst
_tmp322;struct _tuple5 _tmp323;enum Cyc_Absyn_Sign _tmp324;int _tmp325;enum Cyc_Absyn_Primop
_tmp327;struct Cyc_List_List*_tmp328;struct Cyc_List_List _tmp329;struct Cyc_Absyn_Exp*
_tmp32A;_LL127: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp307=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp306;if(_tmp307->tag != 1)goto _LL129;else{_tmp308=(void*)_tmp307->f2;{struct
Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp309=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)
_tmp308;if(_tmp309->tag != 5)goto _LL129;else{_tmp30A=_tmp309->f1;}};}}_LL128:
_tmp30E=_tmp30A;goto _LL12A;_LL129: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp30B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp306;if(_tmp30B->tag != 1)
goto _LL12B;else{_tmp30C=(void*)_tmp30B->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmp30D=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp30C;if(_tmp30D->tag != 
4)goto _LL12B;else{_tmp30E=_tmp30D->f1;}};}}_LL12A: _tmp312=_tmp30E;goto _LL12C;
_LL12B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp30F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp306;if(_tmp30F->tag != 1)goto _LL12D;else{_tmp310=(void*)_tmp30F->f2;{struct
Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp311=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)
_tmp310;if(_tmp311->tag != 1)goto _LL12D;else{_tmp312=_tmp311->f1;}};}}_LL12C:
_tmp316=_tmp312;goto _LL12E;_LL12D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp313=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp306;if(_tmp313->tag != 1)
goto _LL12F;else{_tmp314=(void*)_tmp313->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*
_tmp315=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp314;if(_tmp315->tag != 
3)goto _LL12F;else{_tmp316=_tmp315->f1;}};}}_LL12E: if(_tmp316->escapes)return 0;
if(Cyc_Toc_check_leq_size_var(relns,v,_tmp316))return 1;goto _LL126;_LL12F: {
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp317=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmp306;if(_tmp317->tag != 0)goto _LL131;else{_tmp318=_tmp317->f1;if((_tmp318.Int_c).tag
!= 5)goto _LL131;_tmp319=(struct _tuple5)(_tmp318.Int_c).val;_tmp31A=_tmp319.f1;
if(_tmp31A != Cyc_Absyn_None)goto _LL131;_tmp31B=_tmp319.f2;}}_LL130: _tmp320=
_tmp31B;goto _LL132;_LL131: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp31C=(
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp306;if(_tmp31C->tag != 0)goto
_LL133;else{_tmp31D=_tmp31C->f1;if((_tmp31D.Int_c).tag != 5)goto _LL133;_tmp31E=(
struct _tuple5)(_tmp31D.Int_c).val;_tmp31F=_tmp31E.f1;if(_tmp31F != Cyc_Absyn_Signed)
goto _LL133;_tmp320=_tmp31E.f2;}}_LL132: return _tmp320 >= 0  && Cyc_Toc_check_const_array((
unsigned int)_tmp320,vtype);_LL133: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp321=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp306;if(_tmp321->tag != 
0)goto _LL135;else{_tmp322=_tmp321->f1;if((_tmp322.Int_c).tag != 5)goto _LL135;
_tmp323=(struct _tuple5)(_tmp322.Int_c).val;_tmp324=_tmp323.f1;if(_tmp324 != Cyc_Absyn_Unsigned)
goto _LL135;_tmp325=_tmp323.f2;}}_LL134: return Cyc_Toc_check_const_array((
unsigned int)_tmp325,vtype);_LL135: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*
_tmp326=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp306;if(_tmp326->tag
!= 2)goto _LL137;else{_tmp327=_tmp326->f1;if(_tmp327 != Cyc_Absyn_Numelts)goto
_LL137;_tmp328=_tmp326->f2;if(_tmp328 == 0)goto _LL137;_tmp329=*_tmp328;_tmp32A=(
struct Cyc_Absyn_Exp*)_tmp329.hd;}}_LL136:{void*_tmp32B=_tmp32A->r;void*_tmp32D;
struct Cyc_Absyn_Vardecl*_tmp32F;void*_tmp331;struct Cyc_Absyn_Vardecl*_tmp333;
void*_tmp335;struct Cyc_Absyn_Vardecl*_tmp337;void*_tmp339;struct Cyc_Absyn_Vardecl*
_tmp33B;_LL13A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp32C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp32B;if(_tmp32C->tag != 1)goto _LL13C;else{_tmp32D=(void*)_tmp32C->f2;{struct
Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp32E=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)
_tmp32D;if(_tmp32E->tag != 5)goto _LL13C;else{_tmp32F=_tmp32E->f1;}};}}_LL13B:
_tmp333=_tmp32F;goto _LL13D;_LL13C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp330=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp32B;if(_tmp330->tag != 1)
goto _LL13E;else{_tmp331=(void*)_tmp330->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmp332=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp331;if(_tmp332->tag != 
4)goto _LL13E;else{_tmp333=_tmp332->f1;}};}}_LL13D: _tmp337=_tmp333;goto _LL13F;
_LL13E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp334=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp32B;if(_tmp334->tag != 1)goto _LL140;else{_tmp335=(void*)_tmp334->f2;{struct
Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp336=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)
_tmp335;if(_tmp336->tag != 1)goto _LL140;else{_tmp337=_tmp336->f1;}};}}_LL13F:
_tmp33B=_tmp337;goto _LL141;_LL140: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp338=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp32B;if(_tmp338->tag != 1)
goto _LL142;else{_tmp339=(void*)_tmp338->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*
_tmp33A=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp339;if(_tmp33A->tag != 
3)goto _LL142;else{_tmp33B=_tmp33A->f1;}};}}_LL141: return _tmp33B == v;_LL142:;
_LL143: goto _LL139;_LL139:;}goto _LL126;_LL137:;_LL138: goto _LL126;_LL126:;}return 0;}
static char _tmp34D[8]="*bogus*";struct _tuple19{void*f1;void*f2;};static int Cyc_Toc_check_bounds(
void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
struct Cyc_Absyn_Vardecl*x;{void*_tmp33C=a->r;void*_tmp33E;struct Cyc_Absyn_Vardecl*
_tmp340;void*_tmp342;struct Cyc_Absyn_Vardecl*_tmp344;void*_tmp346;struct Cyc_Absyn_Vardecl*
_tmp348;void*_tmp34A;struct Cyc_Absyn_Vardecl*_tmp34C;_LL145: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp33D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp33C;if(_tmp33D->tag != 1)
goto _LL147;else{_tmp33E=(void*)_tmp33D->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*
_tmp33F=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp33E;if(_tmp33F->tag != 5)
goto _LL147;else{_tmp340=_tmp33F->f1;}};}}_LL146: _tmp344=_tmp340;goto _LL148;
_LL147: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp341=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp33C;if(_tmp341->tag != 1)goto _LL149;else{_tmp342=(void*)_tmp341->f2;{struct
Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp343=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)
_tmp342;if(_tmp343->tag != 4)goto _LL149;else{_tmp344=_tmp343->f1;}};}}_LL148:
_tmp348=_tmp344;goto _LL14A;_LL149: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp345=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp33C;if(_tmp345->tag != 1)
goto _LL14B;else{_tmp346=(void*)_tmp345->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp347=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp346;if(_tmp347->tag
!= 1)goto _LL14B;else{_tmp348=_tmp347->f1;}};}}_LL14A: _tmp34C=_tmp348;goto _LL14C;
_LL14B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp349=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp33C;if(_tmp349->tag != 1)goto _LL14D;else{_tmp34A=(void*)_tmp349->f2;{struct
Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp34B=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)
_tmp34A;if(_tmp34B->tag != 3)goto _LL14D;else{_tmp34C=_tmp34B->f1;}};}}_LL14C: if(
_tmp34C->escapes)goto _LL14E;x=_tmp34C;goto _LL144;_LL14D:;_LL14E: {static struct
_dyneither_ptr bogus_string={_tmp34D,_tmp34D,_tmp34D + 8};static struct _tuple0
bogus_qvar={{.Loc_n={4,0}},& bogus_string};static struct Cyc_Absyn_Vardecl
bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,
0,0,0,0};x=& bogus_vardecl;x->type=a_typ;}_LL144:;}{void*_tmp34E=a_typ;inner_loop: {
void*_tmp34F=i->r;void*_tmp351;struct Cyc_Absyn_Exp*_tmp352;union Cyc_Absyn_Cnst
_tmp354;struct _tuple5 _tmp355;enum Cyc_Absyn_Sign _tmp356;int _tmp357;union Cyc_Absyn_Cnst
_tmp359;struct _tuple5 _tmp35A;enum Cyc_Absyn_Sign _tmp35B;int _tmp35C;union Cyc_Absyn_Cnst
_tmp35E;struct _tuple5 _tmp35F;enum Cyc_Absyn_Sign _tmp360;int _tmp361;enum Cyc_Absyn_Primop
_tmp363;struct Cyc_List_List*_tmp364;struct Cyc_List_List _tmp365;struct Cyc_Absyn_Exp*
_tmp366;struct Cyc_List_List*_tmp367;struct Cyc_List_List _tmp368;struct Cyc_Absyn_Exp*
_tmp369;void*_tmp36B;struct Cyc_Absyn_Vardecl*_tmp36D;void*_tmp36F;struct Cyc_Absyn_Vardecl*
_tmp371;void*_tmp373;struct Cyc_Absyn_Vardecl*_tmp375;void*_tmp377;struct Cyc_Absyn_Vardecl*
_tmp379;_LL150: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp350=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)
_tmp34F;if(_tmp350->tag != 13)goto _LL152;else{_tmp351=(void*)_tmp350->f1;_tmp352=
_tmp350->f2;}}_LL151: i=_tmp352;goto inner_loop;_LL152: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp353=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp34F;if(_tmp353->tag != 
0)goto _LL154;else{_tmp354=_tmp353->f1;if((_tmp354.Int_c).tag != 5)goto _LL154;
_tmp355=(struct _tuple5)(_tmp354.Int_c).val;_tmp356=_tmp355.f1;if(_tmp356 != Cyc_Absyn_None)
goto _LL154;_tmp357=_tmp355.f2;}}_LL153: _tmp35C=_tmp357;goto _LL155;_LL154: {
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp358=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmp34F;if(_tmp358->tag != 0)goto _LL156;else{_tmp359=_tmp358->f1;if((_tmp359.Int_c).tag
!= 5)goto _LL156;_tmp35A=(struct _tuple5)(_tmp359.Int_c).val;_tmp35B=_tmp35A.f1;
if(_tmp35B != Cyc_Absyn_Signed)goto _LL156;_tmp35C=_tmp35A.f2;}}_LL155: return
_tmp35C >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp35C + 1),_tmp34E);
_LL156: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp35D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmp34F;if(_tmp35D->tag != 0)goto _LL158;else{_tmp35E=_tmp35D->f1;if((_tmp35E.Int_c).tag
!= 5)goto _LL158;_tmp35F=(struct _tuple5)(_tmp35E.Int_c).val;_tmp360=_tmp35F.f1;
if(_tmp360 != Cyc_Absyn_Unsigned)goto _LL158;_tmp361=_tmp35F.f2;}}_LL157: return Cyc_Toc_check_const_array((
unsigned int)(_tmp361 + 1),_tmp34E);_LL158: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*
_tmp362=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp34F;if(_tmp362->tag
!= 2)goto _LL15A;else{_tmp363=_tmp362->f1;if(_tmp363 != Cyc_Absyn_Mod)goto _LL15A;
_tmp364=_tmp362->f2;if(_tmp364 == 0)goto _LL15A;_tmp365=*_tmp364;_tmp366=(struct
Cyc_Absyn_Exp*)_tmp365.hd;_tmp367=_tmp365.tl;if(_tmp367 == 0)goto _LL15A;_tmp368=*
_tmp367;_tmp369=(struct Cyc_Absyn_Exp*)_tmp368.hd;}}_LL159: return Cyc_Toc_check_leq_size(
relns,x,_tmp369,_tmp34E);_LL15A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp36A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp34F;if(_tmp36A->tag != 1)
goto _LL15C;else{_tmp36B=(void*)_tmp36A->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*
_tmp36C=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp36B;if(_tmp36C->tag != 5)
goto _LL15C;else{_tmp36D=_tmp36C->f1;}};}}_LL15B: _tmp371=_tmp36D;goto _LL15D;
_LL15C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp36E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp34F;if(_tmp36E->tag != 1)goto _LL15E;else{_tmp36F=(void*)_tmp36E->f2;{struct
Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp370=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)
_tmp36F;if(_tmp370->tag != 4)goto _LL15E;else{_tmp371=_tmp370->f1;}};}}_LL15D:
_tmp375=_tmp371;goto _LL15F;_LL15E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp372=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp34F;if(_tmp372->tag != 1)
goto _LL160;else{_tmp373=(void*)_tmp372->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp374=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp373;if(_tmp374->tag
!= 1)goto _LL160;else{_tmp375=_tmp374->f1;}};}}_LL15F: _tmp379=_tmp375;goto _LL161;
_LL160: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp376=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp34F;if(_tmp376->tag != 1)goto _LL162;else{_tmp377=(void*)_tmp376->f2;{struct
Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp378=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)
_tmp377;if(_tmp378->tag != 3)goto _LL162;else{_tmp379=_tmp378->f1;}};}}_LL161: if(
_tmp379->escapes)return 0;{struct Cyc_List_List*_tmp37A=relns;for(0;_tmp37A != 0;
_tmp37A=_tmp37A->tl){struct Cyc_CfFlowInfo_Reln*_tmp37B=(struct Cyc_CfFlowInfo_Reln*)
_tmp37A->hd;if(_tmp37B->vd == _tmp379){union Cyc_CfFlowInfo_RelnOp _tmp37C=_tmp37B->rop;
struct Cyc_Absyn_Vardecl*_tmp37D;struct _tuple12 _tmp37E;struct Cyc_Absyn_Vardecl*
_tmp37F;void*_tmp380;unsigned int _tmp381;unsigned int _tmp382;_LL165: if((_tmp37C.LessNumelts).tag
!= 3)goto _LL167;_tmp37D=(struct Cyc_Absyn_Vardecl*)(_tmp37C.LessNumelts).val;
_LL166: if(x == _tmp37D)return 1;else{goto _LL164;}_LL167: if((_tmp37C.LessVar).tag != 
2)goto _LL169;_tmp37E=(struct _tuple12)(_tmp37C.LessVar).val;_tmp37F=_tmp37E.f1;
_tmp380=_tmp37E.f2;_LL168:{struct _tuple19 _tmpB77;struct _tuple19 _tmp384=(_tmpB77.f1=
Cyc_Tcutil_compress(_tmp380),((_tmpB77.f2=Cyc_Tcutil_compress((void*)_check_null(
a->topt)),_tmpB77)));void*_tmp385;void*_tmp387;void*_tmp388;struct Cyc_Absyn_PtrInfo
_tmp38A;struct Cyc_Absyn_PtrAtts _tmp38B;union Cyc_Absyn_Constraint*_tmp38C;_LL170:
_tmp385=_tmp384.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp386=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)
_tmp385;if(_tmp386->tag != 19)goto _LL172;else{_tmp387=(void*)_tmp386->f1;}};
_tmp388=_tmp384.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp389=(
struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp388;if(_tmp389->tag != 5)goto
_LL172;else{_tmp38A=_tmp389->f1;_tmp38B=_tmp38A.ptr_atts;_tmp38C=_tmp38B.bounds;}};
_LL171:{void*_tmp38D=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp38C);struct Cyc_Absyn_Exp*_tmp38F;_LL175: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp38E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp38D;if(_tmp38E->tag != 
1)goto _LL177;else{_tmp38F=_tmp38E->f1;}}_LL176: {struct Cyc_Absyn_Exp*_tmp390=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp387,0),0,Cyc_Absyn_No_coercion,0);
if(Cyc_Evexp_lte_const_exp(_tmp390,_tmp38F))return 1;goto _LL174;}_LL177:;_LL178:
goto _LL174;_LL174:;}goto _LL16F;_LL172:;_LL173: goto _LL16F;_LL16F:;}{struct Cyc_List_List*
_tmp391=relns;for(0;_tmp391 != 0;_tmp391=_tmp391->tl){struct Cyc_CfFlowInfo_Reln*
_tmp392=(struct Cyc_CfFlowInfo_Reln*)_tmp391->hd;if(_tmp392->vd == _tmp37F){union
Cyc_CfFlowInfo_RelnOp _tmp393=_tmp392->rop;struct Cyc_Absyn_Vardecl*_tmp394;struct
Cyc_Absyn_Vardecl*_tmp395;unsigned int _tmp396;unsigned int _tmp397;struct _tuple12
_tmp398;struct Cyc_Absyn_Vardecl*_tmp399;_LL17A: if((_tmp393.LessEqNumelts).tag != 
5)goto _LL17C;_tmp394=(struct Cyc_Absyn_Vardecl*)(_tmp393.LessEqNumelts).val;
_LL17B: _tmp395=_tmp394;goto _LL17D;_LL17C: if((_tmp393.LessNumelts).tag != 3)goto
_LL17E;_tmp395=(struct Cyc_Absyn_Vardecl*)(_tmp393.LessNumelts).val;_LL17D: if(x == 
_tmp395)return 1;goto _LL179;_LL17E: if((_tmp393.EqualConst).tag != 1)goto _LL180;
_tmp396=(unsigned int)(_tmp393.EqualConst).val;_LL17F: return Cyc_Toc_check_const_array(
_tmp396,_tmp34E);_LL180: if((_tmp393.LessEqConst).tag != 6)goto _LL182;_tmp397=(
unsigned int)(_tmp393.LessEqConst).val;if(!(_tmp397 > 0))goto _LL182;_LL181: return
Cyc_Toc_check_const_array(_tmp397,_tmp34E);_LL182: if((_tmp393.LessVar).tag != 2)
goto _LL184;_tmp398=(struct _tuple12)(_tmp393.LessVar).val;_tmp399=_tmp398.f1;
_LL183: if(Cyc_Toc_check_leq_size_var(relns,x,_tmp399))return 1;goto _LL179;_LL184:;
_LL185: goto _LL179;_LL179:;}}}goto _LL164;_LL169: if((_tmp37C.LessConst).tag != 4)
goto _LL16B;_tmp381=(unsigned int)(_tmp37C.LessConst).val;_LL16A: return Cyc_Toc_check_const_array(
_tmp381,_tmp34E);_LL16B: if((_tmp37C.LessEqConst).tag != 6)goto _LL16D;_tmp382=(
unsigned int)(_tmp37C.LessEqConst).val;_LL16C: return Cyc_Toc_check_const_array(
_tmp382 + 1,_tmp34E);_LL16D:;_LL16E: goto _LL164;_LL164:;}}}goto _LL14F;_LL162:;
_LL163: goto _LL14F;_LL14F:;}return 0;};}static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*
e){if(e->topt == 0){const char*_tmpB7A;void*_tmpB79;(_tmpB79=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7A="Missing type in primop ",
_tag_dyneither(_tmpB7A,sizeof(char),24))),_tag_dyneither(_tmpB79,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*_tmpB7D;void*_tmpB7C;(_tmpB7C=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB7D="Missing type in primop ",_tag_dyneither(_tmpB7D,sizeof(char),24))),
_tag_dyneither(_tmpB7C,sizeof(void*),0)));}return(void*)_check_null(e->topt);}
static struct _tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){struct _tuple10*
_tmpB7E;return(_tmpB7E=_cycalloc(sizeof(*_tmpB7E)),((_tmpB7E->f1=Cyc_Toc_mt_tq,((
_tmpB7E->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpB7E)))));}static
struct _tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{struct _tuple16*_tmpB7F;return(_tmpB7F=_cycalloc(sizeof(*
_tmpB7F)),((_tmpB7F->f1=0,((_tmpB7F->f2=e,_tmpB7F)))));};}static struct Cyc_Absyn_Exp*
Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct
Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*
eo;void*t;if(pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp3A0=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(
_tmp3A0): Cyc_Toc_malloc_ptr(_tmp3A0));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp3A0);}};}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp3A1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp3A1 != 0;_tmp3A1=_tmp3A1->tl){struct _tuple16 _tmp3A3;struct Cyc_List_List*
_tmp3A4;struct Cyc_Absyn_Exp*_tmp3A5;struct _tuple16*_tmp3A2=(struct _tuple16*)
_tmp3A1->hd;_tmp3A3=*_tmp3A2;_tmp3A4=_tmp3A3.f1;_tmp3A5=_tmp3A3.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp3A4 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp3A4->tl != 0){const char*_tmpB82;void*_tmpB81;(_tmpB81=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB82="multiple designators in array",
_tag_dyneither(_tmpB82,sizeof(char),30))),_tag_dyneither(_tmpB81,sizeof(void*),0)));}{
void*_tmp3A8=(void*)_tmp3A4->hd;void*_tmp3A9=_tmp3A8;struct Cyc_Absyn_Exp*_tmp3AB;
_LL187: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp3AA=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)
_tmp3A9;if(_tmp3AA->tag != 0)goto _LL189;else{_tmp3AB=_tmp3AA->f1;}}_LL188: Cyc_Toc_exp_to_c(
nv,_tmp3AB);e_index=_tmp3AB;goto _LL186;_LL189: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*
_tmp3AC=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A9;if(_tmp3AC->tag
!= 1)goto _LL186;}_LL18A: {const char*_tmpB85;void*_tmpB84;(_tmpB84=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB85="field name designators in array",
_tag_dyneither(_tmpB85,sizeof(char),32))),_tag_dyneither(_tmpB84,sizeof(void*),0)));}
_LL186:;};}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*
_tmp3AF=_tmp3A5->r;struct Cyc_List_List*_tmp3B1;struct Cyc_Absyn_Vardecl*_tmp3B3;
struct Cyc_Absyn_Exp*_tmp3B4;struct Cyc_Absyn_Exp*_tmp3B5;int _tmp3B6;void*_tmp3B8;
struct Cyc_List_List*_tmp3B9;_LL18C: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*
_tmp3B0=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3AF;if(_tmp3B0->tag != 
25)goto _LL18E;else{_tmp3B1=_tmp3B0->f1;}}_LL18D: s=Cyc_Toc_init_array(nv,lval,
_tmp3B1,s);goto _LL18B;_LL18E: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*
_tmp3B2=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3AF;if(
_tmp3B2->tag != 26)goto _LL190;else{_tmp3B3=_tmp3B2->f1;_tmp3B4=_tmp3B2->f2;
_tmp3B5=_tmp3B2->f3;_tmp3B6=_tmp3B2->f4;}}_LL18F: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp3B3,_tmp3B4,_tmp3B5,_tmp3B6,s,0);goto _LL18B;_LL190: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*
_tmp3B7=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3AF;if(_tmp3B7->tag
!= 28)goto _LL192;else{_tmp3B8=(void*)_tmp3B7->f1;_tmp3B9=_tmp3B7->f2;}}_LL191: s=
Cyc_Toc_init_anon_struct(nv,lval,_tmp3B8,_tmp3B9,s);goto _LL18B;_LL192:;_LL193:
Cyc_Toc_exp_to_c(nv,_tmp3A5);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp3A5,0),s,0);goto _LL18B;_LL18B:;};};}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated){struct _tuple0*_tmp3BA=vd->name;
void*_tmp3BB=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));if(!
e1_already_translated)Cyc_Toc_exp_to_c(nv,e1);{struct _RegionHandle _tmp3BC=
_new_region("r2");struct _RegionHandle*r2=& _tmp3BC;_push_region(r2);{struct Cyc_Absyn_Local_b_Absyn_Binding_struct
_tmpB88;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB87;struct Cyc_Toc_Env*
nv2=Cyc_Toc_add_varmap(r2,nv,_tmp3BA,Cyc_Absyn_varb_exp(_tmp3BA,(void*)((_tmpB87=
_cycalloc(sizeof(*_tmpB87)),((_tmpB87[0]=((_tmpB88.tag=4,((_tmpB88.f1=vd,_tmpB88)))),
_tmpB87)))),0));struct _tuple0*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp3BA,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp3BA,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp3BA,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp3BA,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp3BD=e2->r;struct Cyc_List_List*_tmp3BF;
struct Cyc_Absyn_Vardecl*_tmp3C1;struct Cyc_Absyn_Exp*_tmp3C2;struct Cyc_Absyn_Exp*
_tmp3C3;int _tmp3C4;void*_tmp3C6;struct Cyc_List_List*_tmp3C7;_LL195: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*
_tmp3BE=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3BD;if(_tmp3BE->tag != 
25)goto _LL197;else{_tmp3BF=_tmp3BE->f1;}}_LL196: body=Cyc_Toc_init_array(nv2,lval,
_tmp3BF,Cyc_Toc_skip_stmt_dl());goto _LL194;_LL197: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*
_tmp3C0=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3BD;if(
_tmp3C0->tag != 26)goto _LL199;else{_tmp3C1=_tmp3C0->f1;_tmp3C2=_tmp3C0->f2;
_tmp3C3=_tmp3C0->f3;_tmp3C4=_tmp3C0->f4;}}_LL198: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp3C1,_tmp3C2,_tmp3C3,_tmp3C4,Cyc_Toc_skip_stmt_dl(),0);goto _LL194;
_LL199: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp3C5=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)
_tmp3BD;if(_tmp3C5->tag != 28)goto _LL19B;else{_tmp3C6=(void*)_tmp3C5->f1;_tmp3C7=
_tmp3C5->f2;}}_LL19A: body=Cyc_Toc_init_anon_struct(nv,lval,_tmp3C6,_tmp3C7,Cyc_Toc_skip_stmt_dl());
goto _LL194;_LL19B:;_LL19C: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL194;_LL194:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp3BB,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
struct Cyc_Absyn_Stmt*_tmp3C8=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp3BA,Cyc_Absyn_uint_typ,0,s2,0),
0),s,0);_npop_handler(0);return _tmp3C8;};};};_pop_region(r2);};}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*
struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){{struct Cyc_List_List*
_tmp3CB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);
for(0;_tmp3CB != 0;_tmp3CB=_tmp3CB->tl){struct _tuple16 _tmp3CD;struct Cyc_List_List*
_tmp3CE;struct Cyc_Absyn_Exp*_tmp3CF;struct _tuple16*_tmp3CC=(struct _tuple16*)
_tmp3CB->hd;_tmp3CD=*_tmp3CC;_tmp3CE=_tmp3CD.f1;_tmp3CF=_tmp3CD.f2;if(_tmp3CE == 
0){const char*_tmpB8B;void*_tmpB8A;(_tmpB8A=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8B="empty designator list",
_tag_dyneither(_tmpB8B,sizeof(char),22))),_tag_dyneither(_tmpB8A,sizeof(void*),0)));}
if(_tmp3CE->tl != 0){const char*_tmpB8E;void*_tmpB8D;(_tmpB8D=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8E="too many designators in anonymous struct",
_tag_dyneither(_tmpB8E,sizeof(char),41))),_tag_dyneither(_tmpB8D,sizeof(void*),0)));}{
void*_tmp3D4=(void*)_tmp3CE->hd;struct _dyneither_ptr*_tmp3D6;_LL19E: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*
_tmp3D5=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3D4;if(_tmp3D5->tag
!= 1)goto _LL1A0;else{_tmp3D6=_tmp3D5->f1;}}_LL19F: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(
lhs,_tmp3D6,0);{void*_tmp3D7=_tmp3CF->r;struct Cyc_List_List*_tmp3D9;struct Cyc_Absyn_Vardecl*
_tmp3DB;struct Cyc_Absyn_Exp*_tmp3DC;struct Cyc_Absyn_Exp*_tmp3DD;int _tmp3DE;void*
_tmp3E0;struct Cyc_List_List*_tmp3E1;_LL1A3: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*
_tmp3D8=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3D7;if(_tmp3D8->tag != 
25)goto _LL1A5;else{_tmp3D9=_tmp3D8->f1;}}_LL1A4: s=Cyc_Toc_init_array(nv,lval,
_tmp3D9,s);goto _LL1A2;_LL1A5: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*
_tmp3DA=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3D7;if(
_tmp3DA->tag != 26)goto _LL1A7;else{_tmp3DB=_tmp3DA->f1;_tmp3DC=_tmp3DA->f2;
_tmp3DD=_tmp3DA->f3;_tmp3DE=_tmp3DA->f4;}}_LL1A6: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp3DB,_tmp3DC,_tmp3DD,_tmp3DE,s,0);goto _LL1A2;_LL1A7: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*
_tmp3DF=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3D7;if(_tmp3DF->tag
!= 28)goto _LL1A9;else{_tmp3E0=(void*)_tmp3DF->f1;_tmp3E1=_tmp3DF->f2;}}_LL1A8: s=
Cyc_Toc_init_anon_struct(nv,lval,_tmp3E0,_tmp3E1,s);goto _LL1A2;_LL1A9:;_LL1AA:
Cyc_Toc_exp_to_c(nv,_tmp3CF);if(Cyc_Toc_is_poly_field(struct_type,_tmp3D6))
_tmp3CF=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3CF);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3CF,0),0),s,0);goto _LL1A2;
_LL1A2:;}goto _LL19D;}_LL1A0:;_LL1A1: {const char*_tmpB91;void*_tmpB90;(_tmpB90=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB91="array designator in struct",_tag_dyneither(_tmpB91,sizeof(char),27))),
_tag_dyneither(_tmpB90,sizeof(void*),0)));}_LL19D:;};}}return s;}static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,
struct Cyc_List_List*es){struct _RegionHandle _tmp3E4=_new_region("r");struct
_RegionHandle*r=& _tmp3E4;_push_region(r);{struct Cyc_List_List*_tmp3E5=((struct
Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);void*_tmp3E6=Cyc_Toc_add_tuple_type(
_tmp3E5);struct _tuple0*_tmp3E7=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3E8=
Cyc_Absyn_var_exp(_tmp3E7,0);struct Cyc_Absyn_Stmt*_tmp3E9=Cyc_Absyn_exp_stmt(
_tmp3E8,0);struct Cyc_Absyn_Exp*(*_tmp3EA)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp3EB=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp3EB);for(0;_tmp3EB != 0;(_tmp3EB=_tmp3EB->tl,
-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp3EB->hd;struct Cyc_Absyn_Exp*
lval=_tmp3EA(_tmp3E8,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic  && Cyc_Toc_atomic_typ((
void*)_check_null(e->topt));{void*_tmp3EC=e->r;struct Cyc_List_List*_tmp3EE;
struct Cyc_Absyn_Vardecl*_tmp3F0;struct Cyc_Absyn_Exp*_tmp3F1;struct Cyc_Absyn_Exp*
_tmp3F2;int _tmp3F3;_LL1AC: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3ED=(
struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3EC;if(_tmp3ED->tag != 25)goto
_LL1AE;else{_tmp3EE=_tmp3ED->f1;}}_LL1AD: _tmp3E9=Cyc_Toc_init_array(nv,lval,
_tmp3EE,_tmp3E9);goto _LL1AB;_LL1AE: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*
_tmp3EF=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3EC;if(
_tmp3EF->tag != 26)goto _LL1B0;else{_tmp3F0=_tmp3EF->f1;_tmp3F1=_tmp3EF->f2;
_tmp3F2=_tmp3EF->f3;_tmp3F3=_tmp3EF->f4;}}_LL1AF: _tmp3E9=Cyc_Toc_init_comprehension(
nv,lval,_tmp3F0,_tmp3F1,_tmp3F2,_tmp3F3,_tmp3E9,0);goto _LL1AB;_LL1B0:;_LL1B1: Cyc_Toc_exp_to_c(
nv,e);_tmp3E9=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3EA(
_tmp3E8,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp3E9,0);goto _LL1AB;_LL1AB:;};}}{
struct Cyc_Absyn_Exp*_tmp3F4=Cyc_Toc_make_struct(nv,_tmp3E7,_tmp3E6,_tmp3E9,
pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp3F4;};};_pop_region(r);}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp3F5=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp3F5 != 0;_tmp3F5=_tmp3F5->tl){struct Cyc_Absyn_Aggrfield*
_tmp3F6=(struct Cyc_Absyn_Aggrfield*)_tmp3F5->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp3F6->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}{
struct Cyc_String_pa_PrintArg_struct _tmpB99;void*_tmpB98[1];const char*_tmpB97;
void*_tmpB96;(_tmpB96=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpB99.tag=0,((_tmpB99.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB98[0]=& _tmpB99,Cyc_aprintf(((
_tmpB97="get_member_offset %s failed",_tag_dyneither(_tmpB97,sizeof(char),28))),
_tag_dyneither(_tmpB98,sizeof(void*),1)))))))),_tag_dyneither(_tmpB96,sizeof(
void*),0)));};}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*
nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){struct _tuple0*_tmp3FB=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3FC=Cyc_Absyn_var_exp(_tmp3FB,0);struct Cyc_Absyn_Stmt*
_tmp3FD=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp3FC),0);struct Cyc_Absyn_Exp*(*
_tmp3FE)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;void*_tmp3FF=Cyc_Toc_typ_to_c(struct_type);int is_atomic=
1;struct Cyc_List_List*forall_types;struct Cyc_Absyn_Aggrdecl*ad;struct Cyc_List_List*
aggrfields;{void*_tmp400=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo
_tmp402;union Cyc_Absyn_AggrInfoU _tmp403;struct Cyc_List_List*_tmp404;_LL1B3: {
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp401=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp400;if(_tmp401->tag != 11)goto _LL1B5;else{_tmp402=_tmp401->f1;_tmp403=_tmp402.aggr_info;
_tmp404=_tmp402.targs;}}_LL1B4: ad=Cyc_Absyn_get_known_aggrdecl(_tmp403);
aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
forall_types=_tmp404;goto _LL1B2;_LL1B5:;_LL1B6: {const char*_tmpB9C;void*_tmpB9B;(
_tmpB9B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB9C="init_struct: bad struct type",_tag_dyneither(_tmpB9C,sizeof(char),29))),
_tag_dyneither(_tmpB9B,sizeof(void*),0)));}_LL1B2:;}{void*orig_typ=Cyc_Toc_typ_to_c(
struct_type);void*aggr_typ=orig_typ;if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields != 0){struct Cyc_List_List*_tmp407=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp407->tl != 0;_tmp407=_tmp407->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp408=(struct Cyc_Absyn_Aggrfield*)_tmp407->hd;struct
_RegionHandle _tmp409=_new_region("temp");struct _RegionHandle*temp=& _tmp409;
_push_region(temp);{void*cast_type;if(pointer)cast_type=Cyc_Absyn_cstar_typ(
orig_typ,Cyc_Toc_mt_tq);else{cast_type=orig_typ;}_tmp3FD=Cyc_Absyn_exp_stmt(Cyc_Toc_cast_it(
cast_type,Cyc_Absyn_copy_exp(_tmp3FC)),0);{struct Cyc_List_List*_tmp40A=((struct
Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);if(Cyc_Toc_is_abstract_type(
Cyc_Tcutil_rsubstitute(temp,_tmp40A,_tmp408->type))){struct Cyc_List_List*_tmp40B=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars,exist_types);struct Cyc_List_List*_tmp40C=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(temp,_tmp40A,_tmp40B);struct Cyc_List_List*new_fields=0;for(
_tmp407=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp407 != 
0;_tmp407=_tmp407->tl){struct Cyc_Absyn_Aggrfield*_tmp40D=(struct Cyc_Absyn_Aggrfield*)
_tmp407->hd;struct Cyc_Absyn_Aggrfield*_tmpB9D;struct Cyc_Absyn_Aggrfield*_tmp40E=(
_tmpB9D=_cycalloc(sizeof(*_tmpB9D)),((_tmpB9D->name=_tmp40D->name,((_tmpB9D->tq=
Cyc_Toc_mt_tq,((_tmpB9D->type=Cyc_Tcutil_rsubstitute(temp,_tmp40C,_tmp40D->type),((
_tmpB9D->width=_tmp40D->width,((_tmpB9D->attributes=_tmp40D->attributes,_tmpB9D)))))))))));
struct Cyc_List_List*_tmpB9E;new_fields=((_tmpB9E=_cycalloc(sizeof(*_tmpB9E)),((
_tmpB9E->hd=_tmp40E,((_tmpB9E->tl=new_fields,_tmpB9E))))));}exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
new_fields);{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpBA1;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmpBA0;struct_type=(void*)((_tmpBA0=_cycalloc(sizeof(*_tmpBA0)),((_tmpBA0[0]=((
_tmpBA1.tag=12,((_tmpBA1.f1=ad->kind,((_tmpBA1.f2=aggrfields,_tmpBA1)))))),
_tmpBA0))));}aggr_typ=Cyc_Toc_typ_to_c(struct_type);}};};_pop_region(temp);};}{
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp413=_new_region("r");struct _RegionHandle*r=& _tmp413;
_push_region(r);{struct Cyc_List_List*_tmp414=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp414 != 0;
_tmp414=_tmp414->tl){struct _tuple16 _tmp416;struct Cyc_List_List*_tmp417;struct Cyc_Absyn_Exp*
_tmp418;struct _tuple16*_tmp415=(struct _tuple16*)_tmp414->hd;_tmp416=*_tmp415;
_tmp417=_tmp416.f1;_tmp418=_tmp416.f2;is_atomic=is_atomic  && Cyc_Toc_atomic_typ((
void*)_check_null(_tmp418->topt));if(_tmp417 == 0){const char*_tmpBA4;void*_tmpBA3;(
_tmpBA3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBA4="empty designator list",_tag_dyneither(_tmpBA4,sizeof(char),22))),
_tag_dyneither(_tmpBA3,sizeof(void*),0)));}if(_tmp417->tl != 0){struct _tuple0*
_tmp41B=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp41C=Cyc_Absyn_var_exp(_tmp41B,
0);for(0;_tmp417 != 0;_tmp417=_tmp417->tl){void*_tmp41D=(void*)_tmp417->hd;struct
_dyneither_ptr*_tmp41F;_LL1B8: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*
_tmp41E=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp41D;if(_tmp41E->tag
!= 1)goto _LL1BA;else{_tmp41F=_tmp41E->f1;}}_LL1B9: if(Cyc_Toc_is_poly_field(
struct_type,_tmp41F))_tmp41C=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp41C);
_tmp3FD=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3FE(
_tmp3FC,_tmp41F,0),_tmp41C,0),0),_tmp3FD,0);goto _LL1B7;_LL1BA:;_LL1BB: {const
char*_tmpBA7;void*_tmpBA6;(_tmpBA6=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA7="array designator in struct",
_tag_dyneither(_tmpBA7,sizeof(char),27))),_tag_dyneither(_tmpBA6,sizeof(void*),0)));}
_LL1B7:;}Cyc_Toc_exp_to_c(nv,_tmp418);_tmp3FD=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(_tmp41C,_tmp418,0),0),_tmp3FD,0);}else{void*_tmp422=(void*)
_tmp417->hd;struct _dyneither_ptr*_tmp424;_LL1BD: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*
_tmp423=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp422;if(_tmp423->tag
!= 1)goto _LL1BF;else{_tmp424=_tmp423->f1;}}_LL1BE: {struct Cyc_Absyn_Exp*lval=
_tmp3FE(_tmp3FC,_tmp424,0);if(is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,
_tmp424);struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*
lhs=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=
Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);_tmp3FD=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
assn_exp,0),_tmp3FD,0);lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{
void*_tmp425=_tmp418->r;struct Cyc_List_List*_tmp427;struct Cyc_Absyn_Vardecl*
_tmp429;struct Cyc_Absyn_Exp*_tmp42A;struct Cyc_Absyn_Exp*_tmp42B;int _tmp42C;void*
_tmp42E;struct Cyc_List_List*_tmp42F;_LL1C2: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*
_tmp426=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp425;if(_tmp426->tag != 
25)goto _LL1C4;else{_tmp427=_tmp426->f1;}}_LL1C3: _tmp3FD=Cyc_Toc_init_array(nv,
lval,_tmp427,_tmp3FD);goto _LL1C1;_LL1C4: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*
_tmp428=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp425;if(
_tmp428->tag != 26)goto _LL1C6;else{_tmp429=_tmp428->f1;_tmp42A=_tmp428->f2;
_tmp42B=_tmp428->f3;_tmp42C=_tmp428->f4;}}_LL1C5: _tmp3FD=Cyc_Toc_init_comprehension(
nv,lval,_tmp429,_tmp42A,_tmp42B,_tmp42C,_tmp3FD,0);goto _LL1C1;_LL1C6: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*
_tmp42D=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp425;if(_tmp42D->tag
!= 28)goto _LL1C8;else{_tmp42E=(void*)_tmp42D->f1;_tmp42F=_tmp42D->f2;}}_LL1C7:
_tmp3FD=Cyc_Toc_init_anon_struct(nv,lval,_tmp42E,_tmp42F,_tmp3FD);goto _LL1C1;
_LL1C8:;_LL1C9: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)_check_null(
_tmp418->topt));Cyc_Toc_exp_to_c(nv,_tmp418);{struct Cyc_Absyn_Aggrfield*_tmp430=
Cyc_Absyn_lookup_field(aggrfields,_tmp424);if(Cyc_Toc_is_poly_field(struct_type,
_tmp424) && !was_ptr_type)_tmp418=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp418);if(exist_types != 0)_tmp418=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)
_check_null(_tmp430))->type),_tmp418);_tmp3FD=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(lval,_tmp418,0),0),_tmp3FD,0);goto _LL1C1;};}_LL1C1:;}goto
_LL1BC;}_LL1BF:;_LL1C0: {const char*_tmpBAA;void*_tmpBA9;(_tmpBA9=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAA="array designator in struct",
_tag_dyneither(_tmpBAA,sizeof(char),27))),_tag_dyneither(_tmpBA9,sizeof(void*),0)));}
_LL1BC:;}}}{struct Cyc_Absyn_Exp*_tmp433=Cyc_Toc_make_struct(nv,_tmp3FB,aggr_typ,
_tmp3FD,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp433;};;_pop_region(
r);};};}struct _tuple20{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};static
struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple20*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(
e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,
enum Cyc_Absyn_Incrementor incr){struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct
_tmpBAD;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpBAC;return Cyc_Absyn_new_exp((
void*)((_tmpBAC=_cycalloc(sizeof(*_tmpBAC)),((_tmpBAC[0]=((_tmpBAD.tag=4,((
_tmpBAD.f1=e1,((_tmpBAD.f2=incr,_tmpBAD)))))),_tmpBAC)))),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp436=e1->r;struct Cyc_Absyn_Stmt*_tmp438;void*_tmp43A;struct Cyc_Absyn_Exp*
_tmp43B;struct Cyc_Absyn_Exp*_tmp43D;struct _dyneither_ptr*_tmp43E;int _tmp43F;int
_tmp440;_LL1CB: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp437=(struct
Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp436;if(_tmp437->tag != 35)goto
_LL1CD;else{_tmp438=_tmp437->f1;}}_LL1CC: Cyc_Toc_lvalue_assign_stmt(_tmp438,fs,f,
f_env);goto _LL1CA;_LL1CD: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp439=(
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp436;if(_tmp439->tag != 13)goto
_LL1CF;else{_tmp43A=(void*)_tmp439->f1;_tmp43B=_tmp439->f2;}}_LL1CE: Cyc_Toc_lvalue_assign(
_tmp43B,fs,f,f_env);goto _LL1CA;_LL1CF: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*
_tmp43C=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp436;if(_tmp43C->tag
!= 20)goto _LL1D1;else{_tmp43D=_tmp43C->f1;_tmp43E=_tmp43C->f2;_tmp43F=_tmp43C->f3;
_tmp440=_tmp43C->f4;}}_LL1D0: e1->r=_tmp43D->r;{struct Cyc_List_List*_tmpBAE;Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)((_tmpBAE=_cycalloc(sizeof(*_tmpBAE)),((_tmpBAE->hd=
_tmp43E,((_tmpBAE->tl=fs,_tmpBAE)))))),f,f_env);}goto _LL1CA;_LL1D1:;_LL1D2: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;goto _LL1CA;}_LL1CA:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env){void*_tmp442=s->r;struct Cyc_Absyn_Exp*_tmp444;struct Cyc_Absyn_Decl*
_tmp446;struct Cyc_Absyn_Stmt*_tmp447;struct Cyc_Absyn_Stmt*_tmp449;_LL1D4: {
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp443=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)
_tmp442;if(_tmp443->tag != 1)goto _LL1D6;else{_tmp444=_tmp443->f1;}}_LL1D5: Cyc_Toc_lvalue_assign(
_tmp444,fs,f,f_env);goto _LL1D3;_LL1D6: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*
_tmp445=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp442;if(_tmp445->tag != 
12)goto _LL1D8;else{_tmp446=_tmp445->f1;_tmp447=_tmp445->f2;}}_LL1D7: Cyc_Toc_lvalue_assign_stmt(
_tmp447,fs,f,f_env);goto _LL1D3;_LL1D8: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*
_tmp448=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp442;if(_tmp448->tag != 
2)goto _LL1DA;else{_tmp449=_tmp448->f2;}}_LL1D9: Cyc_Toc_lvalue_assign_stmt(
_tmp449,fs,f,f_env);goto _LL1D3;_LL1DA:;_LL1DB: {const char*_tmpBB2;void*_tmpBB1[1];
struct Cyc_String_pa_PrintArg_struct _tmpBB0;(_tmpBB0.tag=0,((_tmpBB0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpBB1[0]=&
_tmpBB0,Cyc_Toc_toc_impos(((_tmpBB2="lvalue_assign_stmt: %s",_tag_dyneither(
_tmpBB2,sizeof(char),23))),_tag_dyneither(_tmpBB1,sizeof(void*),1)))))));}_LL1D3:;}
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Exp*
Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){void*_tmp44D=e->r;void*_tmp44F;
void**_tmp450;struct Cyc_Absyn_Exp*_tmp451;struct Cyc_Absyn_Exp**_tmp452;struct Cyc_Absyn_Exp*
_tmp454;struct Cyc_Absyn_Stmt*_tmp456;_LL1DD: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*
_tmp44E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp44D;if(_tmp44E->tag != 
13)goto _LL1DF;else{_tmp44F=(void**)& _tmp44E->f1;_tmp450=(void**)((void**)&
_tmp44E->f1);_tmp451=_tmp44E->f2;_tmp452=(struct Cyc_Absyn_Exp**)& _tmp44E->f2;}}
_LL1DE:*_tmp452=Cyc_Toc_push_address_exp(*_tmp452);*_tmp450=Cyc_Absyn_cstar_typ(*
_tmp450,Cyc_Toc_mt_tq);return e;_LL1DF: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*
_tmp453=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp44D;if(_tmp453->tag != 
19)goto _LL1E1;else{_tmp454=_tmp453->f1;}}_LL1E0: return _tmp454;_LL1E1: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*
_tmp455=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp44D;if(_tmp455->tag
!= 35)goto _LL1E3;else{_tmp456=_tmp455->f1;}}_LL1E2: Cyc_Toc_push_address_stmt(
_tmp456);return e;_LL1E3:;_LL1E4: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(
e,0);{const char*_tmpBB6;void*_tmpBB5[1];struct Cyc_String_pa_PrintArg_struct
_tmpBB4;(_tmpBB4.tag=0,((_tmpBB4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpBB5[0]=& _tmpBB4,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB6="can't take & of exp %s",
_tag_dyneither(_tmpBB6,sizeof(char),23))),_tag_dyneither(_tmpBB5,sizeof(void*),1)))))));};
_LL1DC:;}static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){void*
_tmp45A=s->r;struct Cyc_Absyn_Stmt*_tmp45C;struct Cyc_Absyn_Stmt*_tmp45E;struct Cyc_Absyn_Exp*
_tmp460;struct Cyc_Absyn_Exp**_tmp461;_LL1E6: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*
_tmp45B=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp45A;if(_tmp45B->tag != 
2)goto _LL1E8;else{_tmp45C=_tmp45B->f2;}}_LL1E7: _tmp45E=_tmp45C;goto _LL1E9;_LL1E8: {
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp45D=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)
_tmp45A;if(_tmp45D->tag != 12)goto _LL1EA;else{_tmp45E=_tmp45D->f2;}}_LL1E9: Cyc_Toc_push_address_stmt(
_tmp45E);goto _LL1E5;_LL1EA: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp45F=(
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp45A;if(_tmp45F->tag != 1)goto
_LL1EC;else{_tmp460=_tmp45F->f1;_tmp461=(struct Cyc_Absyn_Exp**)& _tmp45F->f1;}}
_LL1EB:*_tmp461=Cyc_Toc_push_address_exp(*_tmp461);goto _LL1E5;_LL1EC:;_LL1ED: {
const char*_tmpBBA;void*_tmpBB9[1];struct Cyc_String_pa_PrintArg_struct _tmpBB8;(
_tmpBB8.tag=0,((_tmpBB8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(
s)),((_tmpBB9[0]=& _tmpBB8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpBBA="can't take & of stmt %s",_tag_dyneither(
_tmpBBA,sizeof(char),24))),_tag_dyneither(_tmpBB9,sizeof(void*),1)))))));}_LL1E5:;}
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,
void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;struct Cyc_List_List*
prev;if(x == 0)return 0;{struct Cyc_List_List*_tmpBBB;result=((_tmpBBB=
_region_malloc(r2,sizeof(*_tmpBBB)),((_tmpBBB->hd=(void*)f((void*)x->hd,env),((
_tmpBBB->tl=0,_tmpBBB))))));}prev=result;for(x=x->tl;x != 0;x=x->tl){{struct Cyc_List_List*
_tmpBBC;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpBBC=_region_malloc(
r2,sizeof(*_tmpBBC)),((_tmpBBC->hd=(void*)f((void*)x->hd,env),((_tmpBBC->tl=0,
_tmpBBC))))));}prev=prev->tl;}return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,
f,env,x);}static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){struct
_tuple16*_tmpBBD;return(_tmpBBD=_cycalloc(sizeof(*_tmpBBD)),((_tmpBBD->f1=0,((
_tmpBBD->f2=e,_tmpBBD)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(
void*t){void*_tmp468=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp46A;
_LL1EF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp469=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp468;if(_tmp469->tag != 5)goto _LL1F1;else{_tmp46A=_tmp469->f1;}}_LL1F0: return
_tmp46A;_LL1F1:;_LL1F2: {const char*_tmpBC0;void*_tmpBBF;(_tmpBBF=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC0="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpBC0,sizeof(char),28))),_tag_dyneither(_tmpBBF,sizeof(void*),0)));}
_LL1EE:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp46D=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp470;enum Cyc_Absyn_Size_of
_tmp471;enum Cyc_Absyn_Sign _tmp473;enum Cyc_Absyn_Size_of _tmp474;enum Cyc_Absyn_Sign
_tmp478;enum Cyc_Absyn_Size_of _tmp479;enum Cyc_Absyn_Sign _tmp47B;enum Cyc_Absyn_Size_of
_tmp47C;enum Cyc_Absyn_Sign _tmp47E;enum Cyc_Absyn_Size_of _tmp47F;int _tmp481;int
_tmp483;int _tmp485;_LL1F4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp46E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46D;if(_tmp46E->tag
!= 5)goto _LL1F6;}_LL1F5: res=Cyc_Absyn_null_exp(0);goto _LL1F3;_LL1F6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp46F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46D;if(_tmp46F->tag != 6)
goto _LL1F8;else{_tmp470=_tmp46F->f1;_tmp471=_tmp46F->f2;if(_tmp471 != Cyc_Absyn_Char_sz)
goto _LL1F8;}}_LL1F7: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp470,'\000'),0);
goto _LL1F3;_LL1F8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp472=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp46D;if(_tmp472->tag != 6)goto _LL1FA;else{_tmp473=_tmp472->f1;_tmp474=_tmp472->f2;
if(_tmp474 != Cyc_Absyn_Short_sz)goto _LL1FA;}}_LL1F9: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(
_tmp473,0),0);goto _LL1F3;_LL1FA: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp475=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp46D;if(_tmp475->tag != 
13)goto _LL1FC;}_LL1FB: goto _LL1FD;_LL1FC: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*
_tmp476=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp46D;if(_tmp476->tag
!= 14)goto _LL1FE;}_LL1FD: _tmp478=Cyc_Absyn_Unsigned;goto _LL1FF;_LL1FE: {struct
Cyc_Absyn_IntType_Absyn_Type_struct*_tmp477=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp46D;if(_tmp477->tag != 6)goto _LL200;else{_tmp478=_tmp477->f1;_tmp479=_tmp477->f2;
if(_tmp479 != Cyc_Absyn_Int_sz)goto _LL200;}}_LL1FF: _tmp47B=_tmp478;goto _LL201;
_LL200: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp47A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp46D;if(_tmp47A->tag != 6)goto _LL202;else{_tmp47B=_tmp47A->f1;_tmp47C=_tmp47A->f2;
if(_tmp47C != Cyc_Absyn_Long_sz)goto _LL202;}}_LL201: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(
_tmp47B,0),0);goto _LL1F3;_LL202: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp47D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46D;if(_tmp47D->tag != 6)
goto _LL204;else{_tmp47E=_tmp47D->f1;_tmp47F=_tmp47D->f2;if(_tmp47F != Cyc_Absyn_LongLong_sz)
goto _LL204;}}_LL203: res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp47E,(
long long)0),0);goto _LL1F3;_LL204: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp480=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46D;if(_tmp480->tag != 
7)goto _LL206;else{_tmp481=_tmp480->f1;if(_tmp481 != 0)goto _LL206;}}_LL205:{const
char*_tmpBC1;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBC1="0.0F",
_tag_dyneither(_tmpBC1,sizeof(char),5))),0),0);}goto _LL1F3;_LL206: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp482=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46D;if(_tmp482->tag != 
7)goto _LL208;else{_tmp483=_tmp482->f1;if(_tmp483 != 1)goto _LL208;}}_LL207:{const
char*_tmpBC2;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBC2="0.0",
_tag_dyneither(_tmpBC2,sizeof(char),4))),1),0);}goto _LL1F3;_LL208: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp484=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46D;if(_tmp484->tag != 
7)goto _LL20A;else{_tmp485=_tmp484->f1;}}_LL209:{const char*_tmpBC3;res=Cyc_Absyn_const_exp(
Cyc_Absyn_Float_c(((_tmpBC3="0.0L",_tag_dyneither(_tmpBC3,sizeof(char),5))),
_tmp485),0);}goto _LL1F3;_LL20A:;_LL20B: {const char*_tmpBC7;void*_tmpBC6[1];
struct Cyc_String_pa_PrintArg_struct _tmpBC5;(_tmpBC5.tag=0,((_tmpBC5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpBC6[0]=&
_tmpBC5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBC7="found non-zero type %s in generate_zero",_tag_dyneither(_tmpBC7,sizeof(
char),40))),_tag_dyneither(_tmpBC6,sizeof(void*),1)))))));}_LL1F3:;}res->topt=(
void*)t;return res;}static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);void*
_tmp48C=Cyc_Toc_typ_to_c(elt_type);void*_tmp48D=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp48E=Cyc_Absyn_cstar_typ(_tmp48C,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpBC8;struct Cyc_Core_Opt*_tmp48F=(_tmpBC8=_cycalloc(sizeof(*_tmpBC8)),((
_tmpBC8->v=_tmp48E,_tmpBC8)));struct Cyc_Absyn_Exp*xinit;{void*_tmp490=e1->r;
struct Cyc_Absyn_Exp*_tmp492;struct Cyc_Absyn_Exp*_tmp494;struct Cyc_Absyn_Exp*
_tmp495;_LL20D: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp491=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)
_tmp490;if(_tmp491->tag != 19)goto _LL20F;else{_tmp492=_tmp491->f1;}}_LL20E: if(!
is_dyneither){_tmp492=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp492,0,Cyc_Absyn_Other_coercion,
0);_tmp492->topt=(void*)fat_ptr_type;}Cyc_Toc_exp_to_c(nv,_tmp492);xinit=_tmp492;
goto _LL20C;_LL20F: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp493=(
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp490;if(_tmp493->tag != 22)
goto _LL211;else{_tmp494=_tmp493->f1;_tmp495=_tmp493->f2;}}_LL210: if(!
is_dyneither){_tmp494=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp494,0,Cyc_Absyn_Other_coercion,
0);_tmp494->topt=(void*)fat_ptr_type;}Cyc_Toc_exp_to_c(nv,_tmp494);Cyc_Toc_exp_to_c(
nv,_tmp495);{struct Cyc_Absyn_Exp*_tmpBC9[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpBC9[2]=_tmp495,((_tmpBC9[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpBC9[0]=_tmp494,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBC9,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL20C;_LL211:;_LL212: {const char*_tmpBCC;void*_tmpBCB;(_tmpBCB=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCC="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmpBCC,sizeof(char),53))),_tag_dyneither(_tmpBCB,sizeof(void*),0)));}
_LL20C:;}{struct _tuple0*_tmp499=Cyc_Toc_temp_var();struct _RegionHandle _tmp49A=
_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp49A;_push_region(rgn2);{struct
Cyc_Toc_Env*_tmp49B=Cyc_Toc_add_varmap(rgn2,nv,_tmp499,Cyc_Absyn_var_exp(_tmp499,
0));struct Cyc_Absyn_Vardecl*_tmpBCD;struct Cyc_Absyn_Vardecl*_tmp49C=(_tmpBCD=
_cycalloc(sizeof(*_tmpBCD)),((_tmpBCD->sc=Cyc_Absyn_Public,((_tmpBCD->name=
_tmp499,((_tmpBCD->tq=Cyc_Toc_mt_tq,((_tmpBCD->type=_tmp48D,((_tmpBCD->initializer=(
struct Cyc_Absyn_Exp*)xinit,((_tmpBCD->rgn=0,((_tmpBCD->attributes=0,((_tmpBCD->escapes=
0,_tmpBCD)))))))))))))))));struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBD0;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBCF;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmp49D=(_tmpBCF=_cycalloc(sizeof(*_tmpBCF)),((_tmpBCF[0]=((_tmpBD0.tag=4,((
_tmpBD0.f1=_tmp49C,_tmpBD0)))),_tmpBCF)));struct Cyc_Absyn_Exp*_tmp49E=Cyc_Absyn_varb_exp(
_tmp499,(void*)_tmp49D,0);_tmp49E->topt=(void*)fat_ptr_type;{struct Cyc_Absyn_Exp*
_tmp49F=Cyc_Absyn_deref_exp(_tmp49E,0);_tmp49F->topt=(void*)elt_type;Cyc_Toc_exp_to_c(
_tmp49B,_tmp49F);{struct _tuple0*_tmp4A0=Cyc_Toc_temp_var();_tmp49B=Cyc_Toc_add_varmap(
rgn2,_tmp49B,_tmp4A0,Cyc_Absyn_var_exp(_tmp4A0,0));{struct Cyc_Absyn_Vardecl*
_tmpBD1;struct Cyc_Absyn_Vardecl*_tmp4A1=(_tmpBD1=_cycalloc(sizeof(*_tmpBD1)),((
_tmpBD1->sc=Cyc_Absyn_Public,((_tmpBD1->name=_tmp4A0,((_tmpBD1->tq=Cyc_Toc_mt_tq,((
_tmpBD1->type=_tmp48C,((_tmpBD1->initializer=(struct Cyc_Absyn_Exp*)_tmp49F,((
_tmpBD1->rgn=0,((_tmpBD1->attributes=0,((_tmpBD1->escapes=0,_tmpBD1)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBD4;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmpBD3;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4A2=(_tmpBD3=_cycalloc(
sizeof(*_tmpBD3)),((_tmpBD3[0]=((_tmpBD4.tag=4,((_tmpBD4.f1=_tmp4A1,_tmpBD4)))),
_tmpBD3)));struct Cyc_Absyn_Exp*z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*
_tmp4A3=Cyc_Absyn_varb_exp(_tmp4A0,(void*)_tmp4A2,0);_tmp4A3->topt=_tmp49F->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp4A3,e2,0);z_init->topt=
_tmp4A3->topt;}Cyc_Toc_exp_to_c(_tmp49B,z_init);{struct _tuple0*_tmp4A4=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpBD5;struct Cyc_Absyn_Vardecl*_tmp4A5=(_tmpBD5=
_cycalloc(sizeof(*_tmpBD5)),((_tmpBD5->sc=Cyc_Absyn_Public,((_tmpBD5->name=
_tmp4A4,((_tmpBD5->tq=Cyc_Toc_mt_tq,((_tmpBD5->type=_tmp48C,((_tmpBD5->initializer=(
struct Cyc_Absyn_Exp*)z_init,((_tmpBD5->rgn=0,((_tmpBD5->attributes=0,((_tmpBD5->escapes=
0,_tmpBD5)))))))))))))))));struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBD8;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBD7;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmp4A6=(_tmpBD7=_cycalloc(sizeof(*_tmpBD7)),((_tmpBD7[0]=((_tmpBD8.tag=4,((
_tmpBD8.f1=_tmp4A5,_tmpBD8)))),_tmpBD7)));_tmp49B=Cyc_Toc_add_varmap(rgn2,
_tmp49B,_tmp4A4,Cyc_Absyn_var_exp(_tmp4A4,0));{struct Cyc_Absyn_Exp*_tmp4A7=Cyc_Absyn_varb_exp(
_tmp4A0,(void*)_tmp4A2,0);_tmp4A7->topt=_tmp49F->topt;{struct Cyc_Absyn_Exp*
_tmp4A8=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*_tmp4A9=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Eq,_tmp4A7,_tmp4A8,0);_tmp4A9->topt=(void*)Cyc_Absyn_sint_typ;Cyc_Toc_exp_to_c(
_tmp49B,_tmp4A9);{struct Cyc_Absyn_Exp*_tmp4AA=Cyc_Absyn_varb_exp(_tmp4A4,(void*)
_tmp4A6,0);_tmp4AA->topt=_tmp49F->topt;{struct Cyc_Absyn_Exp*_tmp4AB=Cyc_Toc_generate_zero(
elt_type);struct Cyc_Absyn_Exp*_tmp4AC=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp4AA,
_tmp4AB,0);_tmp4AC->topt=(void*)Cyc_Absyn_sint_typ;Cyc_Toc_exp_to_c(_tmp49B,
_tmp4AC);{struct Cyc_Absyn_Exp*_tmpBD9[2];struct Cyc_List_List*_tmp4AD=(_tmpBD9[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpBD9[0]=Cyc_Absyn_varb_exp(
_tmp499,(void*)_tmp49D,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBD9,sizeof(struct Cyc_Absyn_Exp*),2)))));struct Cyc_Absyn_Exp*
_tmp4AE=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp4AD,0),
_tmp4AE,0);{struct Cyc_Absyn_Exp*_tmp4AF=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp4A9,_tmp4AC,0),0);struct Cyc_Absyn_Stmt*_tmp4B0=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp4B1=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp499,(void*)_tmp49D,0),Cyc_Toc_curr_sp,0);_tmp4B1=Cyc_Toc_cast_it(
_tmp48E,_tmp4B1);{struct Cyc_Absyn_Exp*_tmp4B2=Cyc_Absyn_deref_exp(_tmp4B1,0);
struct Cyc_Absyn_Exp*_tmp4B3=Cyc_Absyn_assign_exp(_tmp4B2,Cyc_Absyn_var_exp(
_tmp4A4,0),0);struct Cyc_Absyn_Stmt*_tmp4B4=Cyc_Absyn_exp_stmt(_tmp4B3,0);_tmp4B4=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp4AF,_tmp4B0,Cyc_Absyn_skip_stmt(
0),0),_tmp4B4,0);{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBDF;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct
_tmpBDE;struct Cyc_Absyn_Decl*_tmpBDD;_tmp4B4=Cyc_Absyn_decl_stmt(((_tmpBDD=
_cycalloc(sizeof(*_tmpBDD)),((_tmpBDD->r=(void*)((_tmpBDF=_cycalloc(sizeof(*
_tmpBDF)),((_tmpBDF[0]=((_tmpBDE.tag=0,((_tmpBDE.f1=_tmp4A5,_tmpBDE)))),_tmpBDF)))),((
_tmpBDD->loc=0,_tmpBDD)))))),_tmp4B4,0);}{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*
_tmpBE5;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpBE4;struct Cyc_Absyn_Decl*
_tmpBE3;_tmp4B4=Cyc_Absyn_decl_stmt(((_tmpBE3=_cycalloc(sizeof(*_tmpBE3)),((
_tmpBE3->r=(void*)((_tmpBE5=_cycalloc(sizeof(*_tmpBE5)),((_tmpBE5[0]=((_tmpBE4.tag=
0,((_tmpBE4.f1=_tmp4A1,_tmpBE4)))),_tmpBE5)))),((_tmpBE3->loc=0,_tmpBE3)))))),
_tmp4B4,0);}{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBEB;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct
_tmpBEA;struct Cyc_Absyn_Decl*_tmpBE9;_tmp4B4=Cyc_Absyn_decl_stmt(((_tmpBE9=
_cycalloc(sizeof(*_tmpBE9)),((_tmpBE9->r=(void*)((_tmpBEB=_cycalloc(sizeof(*
_tmpBEB)),((_tmpBEB[0]=((_tmpBEA.tag=0,((_tmpBEA.f1=_tmp49C,_tmpBEA)))),_tmpBEB)))),((
_tmpBE9->loc=0,_tmpBE9)))))),_tmp4B4,0);}e->r=Cyc_Toc_stmt_exp_r(_tmp4B4);};};};};};};};};};};};};
_pop_region(rgn2);};}static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*
e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*
aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){struct Cyc_Absyn_Aggrdecl*
ad;{void*_tmp4C9=Cyc_Tcutil_compress(aggrtype);struct Cyc_Absyn_AggrInfo _tmp4CB;
union Cyc_Absyn_AggrInfoU _tmp4CC;_LL214: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp4CA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C9;if(_tmp4CA->tag != 
11)goto _LL216;else{_tmp4CB=_tmp4CA->f1;_tmp4CC=_tmp4CB.aggr_info;}}_LL215: ad=Cyc_Absyn_get_known_aggrdecl(
_tmp4CC);goto _LL213;_LL216:;_LL217: {struct Cyc_String_pa_PrintArg_struct _tmpBF3;
void*_tmpBF2[1];const char*_tmpBF1;void*_tmpBF0;(_tmpBF0=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct
_dyneither_ptr)((_tmpBF3.tag=0,((_tmpBF3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpBF2[0]=& _tmpBF3,Cyc_aprintf(((
_tmpBF1="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpBF1,sizeof(char),51))),_tag_dyneither(_tmpBF2,sizeof(void*),1)))))))),
_tag_dyneither(_tmpBF0,sizeof(void*),0)));}_LL213:;}{struct _tuple0*_tmp4D1=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4D2=Cyc_Absyn_var_exp(_tmp4D1,0);struct Cyc_Absyn_Exp*
_tmp4D3=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);if(in_lhs){
struct Cyc_Absyn_Exp*_tmp4D4=Cyc_Absyn_aggrarrow_exp(_tmp4D2,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4D5=Cyc_Absyn_neq_exp(_tmp4D4,_tmp4D3,0);struct Cyc_Absyn_Exp*
_tmp4D6=Cyc_Absyn_aggrarrow_exp(_tmp4D2,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Stmt*
_tmp4D7=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp4D6,0),0);struct Cyc_Absyn_Stmt*
_tmp4D8=Cyc_Absyn_ifthenelse_stmt(_tmp4D5,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);void*_tmp4D9=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp4DA=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);struct Cyc_Absyn_Stmt*_tmp4DB=
Cyc_Absyn_declare_stmt(_tmp4D1,_tmp4D9,(struct Cyc_Absyn_Exp*)_tmp4DA,Cyc_Absyn_seq_stmt(
_tmp4D8,_tmp4D7,0),0);return Cyc_Toc_stmt_exp_r(_tmp4DB);}else{struct Cyc_Absyn_Exp*
_tmp4DC=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp4D2,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp4DD=Cyc_Absyn_neq_exp(_tmp4DC,_tmp4D3,0);struct Cyc_Absyn_Exp*
_tmp4DE=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp4D2,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp4DF=Cyc_Absyn_exp_stmt(_tmp4DE,0);struct Cyc_Absyn_Stmt*
_tmp4E0=Cyc_Absyn_ifthenelse_stmt(_tmp4DD,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp4E1=Cyc_Absyn_declare_stmt(_tmp4D1,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp4E0,_tmp4DF,0),0);return Cyc_Toc_stmt_exp_r(
_tmp4E1);}};}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read){void*_tmp4E2=e->r;struct Cyc_Absyn_Exp*
_tmp4E4;struct Cyc_Absyn_Exp*_tmp4E6;struct _dyneither_ptr*_tmp4E7;int _tmp4E8;int*
_tmp4E9;struct Cyc_Absyn_Exp*_tmp4EB;struct _dyneither_ptr*_tmp4EC;int _tmp4ED;int*
_tmp4EE;_LL219: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4E3=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)
_tmp4E2;if(_tmp4E3->tag != 13)goto _LL21B;else{_tmp4E4=_tmp4E3->f2;}}_LL21A: {
const char*_tmpBF6;void*_tmpBF5;(_tmpBF5=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF6="cast on lhs!",
_tag_dyneither(_tmpBF6,sizeof(char),13))),_tag_dyneither(_tmpBF5,sizeof(void*),0)));}
_LL21B: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4E5=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)
_tmp4E2;if(_tmp4E5->tag != 20)goto _LL21D;else{_tmp4E6=_tmp4E5->f1;_tmp4E7=_tmp4E5->f2;
_tmp4E8=_tmp4E5->f4;_tmp4E9=(int*)& _tmp4E5->f4;}}_LL21C: {void*_tmp4F1=Cyc_Tcutil_compress((
void*)_check_null(_tmp4E6->topt));struct Cyc_Absyn_AggrInfo _tmp4F3;union Cyc_Absyn_AggrInfoU
_tmp4F4;_LL222: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4F2=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp4F1;if(_tmp4F2->tag != 11)goto _LL224;else{_tmp4F3=_tmp4F2->f1;_tmp4F4=_tmp4F3.aggr_info;}}
_LL223: {struct Cyc_Absyn_Aggrdecl*_tmp4F5=Cyc_Absyn_get_known_aggrdecl(_tmp4F4);*
f_tag=Cyc_Toc_get_member_offset(_tmp4F5,_tmp4E7);{const char*_tmpBFB;void*_tmpBFA[
2];struct Cyc_String_pa_PrintArg_struct _tmpBF9;struct Cyc_String_pa_PrintArg_struct
_tmpBF8;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpBF8.tag=0,((_tmpBF8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4E7),((_tmpBF9.tag=0,((_tmpBF9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4F5->name).f2),((_tmpBFA[0]=&
_tmpBF9,((_tmpBFA[1]=& _tmpBF8,Cyc_aprintf(((_tmpBFB="_union_%s_%s",
_tag_dyneither(_tmpBFB,sizeof(char),13))),_tag_dyneither(_tmpBFA,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpBFC;*tagged_member_type=Cyc_Absyn_strct(((_tmpBFC=
_cycalloc(sizeof(*_tmpBFC)),((_tmpBFC[0]=str,_tmpBFC)))));}if(clear_read)*
_tmp4E9=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4F5->impl))->tagged;};}
_LL224:;_LL225: return 0;_LL221:;}_LL21D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*
_tmp4EA=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4E2;if(_tmp4EA->tag
!= 21)goto _LL21F;else{_tmp4EB=_tmp4EA->f1;_tmp4EC=_tmp4EA->f2;_tmp4ED=_tmp4EA->f4;
_tmp4EE=(int*)& _tmp4EA->f4;}}_LL21E: {void*_tmp4FB=Cyc_Tcutil_compress((void*)
_check_null(_tmp4EB->topt));struct Cyc_Absyn_PtrInfo _tmp4FD;void*_tmp4FE;_LL227: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4FC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp4FB;if(_tmp4FC->tag != 5)goto _LL229;else{_tmp4FD=_tmp4FC->f1;_tmp4FE=_tmp4FD.elt_typ;}}
_LL228: {void*_tmp4FF=Cyc_Tcutil_compress(_tmp4FE);struct Cyc_Absyn_AggrInfo
_tmp501;union Cyc_Absyn_AggrInfoU _tmp502;_LL22C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp500=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4FF;if(_tmp500->tag != 
11)goto _LL22E;else{_tmp501=_tmp500->f1;_tmp502=_tmp501.aggr_info;}}_LL22D: {
struct Cyc_Absyn_Aggrdecl*_tmp503=Cyc_Absyn_get_known_aggrdecl(_tmp502);*f_tag=
Cyc_Toc_get_member_offset(_tmp503,_tmp4EC);{const char*_tmpC01;void*_tmpC00[2];
struct Cyc_String_pa_PrintArg_struct _tmpBFF;struct Cyc_String_pa_PrintArg_struct
_tmpBFE;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpBFE.tag=0,((_tmpBFE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4EC),((_tmpBFF.tag=0,((_tmpBFF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp503->name).f2),((_tmpC00[0]=&
_tmpBFF,((_tmpC00[1]=& _tmpBFE,Cyc_aprintf(((_tmpC01="_union_%s_%s",
_tag_dyneither(_tmpC01,sizeof(char),13))),_tag_dyneither(_tmpC00,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpC02;*tagged_member_type=Cyc_Absyn_strct(((_tmpC02=
_cycalloc(sizeof(*_tmpC02)),((_tmpC02[0]=str,_tmpC02)))));}if(clear_read)*
_tmp4EE=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp503->impl))->tagged;};}
_LL22E:;_LL22F: return 0;_LL22B:;}_LL229:;_LL22A: return 0;_LL226:;}_LL21F:;_LL220:
return 0;_LL218:;}void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*
e,int tag,void*mem_type){struct _tuple0*_tmp509=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
temp_exp=Cyc_Absyn_var_exp(_tmp509,0);struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(
tag,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,
0),0);struct Cyc_Absyn_Exp*_tmp50A=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_ifthenelse_stmt(_tmp50A,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp509,Cyc_Absyn_cstar_typ(
mem_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(
e)),Cyc_Absyn_seq_stmt(s2,s3,0),0);e->r=Cyc_Toc_stmt_exp_r(s1);}static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type){struct _tuple0*_tmp50B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp50B,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp50C=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp50C,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp50B,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple21{struct _tuple0*f1;void*f2;struct
Cyc_Absyn_Exp*f3;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void _tmpC51(unsigned int*_tmpC50,unsigned int*_tmpC4F,struct _tuple0***
_tmpC4D){for(*_tmpC50=0;*_tmpC50 < *_tmpC4F;(*_tmpC50)++){(*_tmpC4D)[*_tmpC50]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp50D=e->r;if(e->topt == 0){const char*_tmpC06;void*_tmpC05[1];struct Cyc_String_pa_PrintArg_struct
_tmpC04;(_tmpC04.tag=0,((_tmpC04.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpC05[0]=& _tmpC04,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC06="exp_to_c: no type for %s",
_tag_dyneither(_tmpC06,sizeof(char),25))),_tag_dyneither(_tmpC05,sizeof(void*),1)))))));}{
void*old_typ=(void*)_check_null(e->topt);void*_tmp511=_tmp50D;union Cyc_Absyn_Cnst
_tmp513;int _tmp514;struct _tuple0*_tmp517;void*_tmp518;enum Cyc_Absyn_Primop
_tmp51A;struct Cyc_List_List*_tmp51B;struct Cyc_Absyn_Exp*_tmp51D;enum Cyc_Absyn_Incrementor
_tmp51E;struct Cyc_Absyn_Exp*_tmp520;struct Cyc_Core_Opt*_tmp521;struct Cyc_Absyn_Exp*
_tmp522;struct Cyc_Absyn_Exp*_tmp524;struct Cyc_Absyn_Exp*_tmp525;struct Cyc_Absyn_Exp*
_tmp526;struct Cyc_Absyn_Exp*_tmp528;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_Absyn_Exp*
_tmp52B;struct Cyc_Absyn_Exp*_tmp52C;struct Cyc_Absyn_Exp*_tmp52E;struct Cyc_Absyn_Exp*
_tmp52F;struct Cyc_Absyn_Exp*_tmp531;struct Cyc_List_List*_tmp532;struct Cyc_Absyn_VarargCallInfo*
_tmp533;struct Cyc_Absyn_Exp*_tmp535;struct Cyc_List_List*_tmp536;struct Cyc_Absyn_VarargCallInfo*
_tmp537;struct Cyc_Absyn_VarargCallInfo _tmp538;int _tmp539;struct Cyc_List_List*
_tmp53A;struct Cyc_Absyn_VarargInfo*_tmp53B;struct Cyc_Absyn_Exp*_tmp53D;struct Cyc_Absyn_Exp*
_tmp53F;struct Cyc_Absyn_Exp*_tmp541;struct Cyc_List_List*_tmp542;void*_tmp544;
void**_tmp545;struct Cyc_Absyn_Exp*_tmp546;int _tmp547;enum Cyc_Absyn_Coercion
_tmp548;struct Cyc_Absyn_Exp*_tmp54A;struct Cyc_Absyn_Exp*_tmp54C;struct Cyc_Absyn_Exp*
_tmp54D;struct Cyc_Absyn_Exp*_tmp54F;void*_tmp551;void*_tmp553;void*_tmp554;
struct _dyneither_ptr*_tmp556;void*_tmp558;void*_tmp559;unsigned int _tmp55B;
struct Cyc_Absyn_Exp*_tmp55D;struct Cyc_Absyn_Exp*_tmp55F;struct _dyneither_ptr*
_tmp560;int _tmp561;int _tmp562;struct Cyc_Absyn_Exp*_tmp564;struct _dyneither_ptr*
_tmp565;int _tmp566;int _tmp567;struct Cyc_Absyn_Exp*_tmp569;struct Cyc_Absyn_Exp*
_tmp56A;struct Cyc_List_List*_tmp56C;struct Cyc_List_List*_tmp56E;struct Cyc_Absyn_Vardecl*
_tmp570;struct Cyc_Absyn_Exp*_tmp571;struct Cyc_Absyn_Exp*_tmp572;int _tmp573;
struct _tuple0*_tmp575;struct Cyc_List_List*_tmp576;struct Cyc_List_List*_tmp577;
struct Cyc_Absyn_Aggrdecl*_tmp578;void*_tmp57A;struct Cyc_List_List*_tmp57B;struct
Cyc_List_List*_tmp57D;struct Cyc_Absyn_Datatypedecl*_tmp57E;struct Cyc_Absyn_Datatypefield*
_tmp57F;struct Cyc_Absyn_MallocInfo _tmp583;int _tmp584;struct Cyc_Absyn_Exp*_tmp585;
void**_tmp586;struct Cyc_Absyn_Exp*_tmp587;int _tmp588;struct Cyc_Absyn_Exp*_tmp58A;
struct Cyc_Absyn_Exp*_tmp58B;struct Cyc_Absyn_Exp*_tmp58D;struct _dyneither_ptr*
_tmp58E;struct Cyc_Absyn_Stmt*_tmp590;_LL231: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp512=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp512->tag != 
0)goto _LL233;else{_tmp513=_tmp512->f1;if((_tmp513.Null_c).tag != 1)goto _LL233;
_tmp514=(int)(_tmp513.Null_c).val;}}_LL232: {struct Cyc_Absyn_Exp*_tmp595=Cyc_Absyn_uint_exp(
0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){if(Cyc_Toc_is_toplevel(nv))e->r=(
Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp595,_tmp595))->r;else{struct Cyc_Absyn_Exp*
_tmpC07[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpC07[2]=
_tmp595,((_tmpC07[1]=_tmp595,((_tmpC07[0]=_tmp595,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC07,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL230;}_LL233: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp515=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp515->tag != 
0)goto _LL235;}_LL234: goto _LL230;_LL235: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp516=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp516->tag != 1)
goto _LL237;else{_tmp517=_tmp516->f1;_tmp518=(void*)_tmp516->f2;}}_LL236:{struct
_handler_cons _tmp597;_push_handler(& _tmp597);{int _tmp599=0;if(setjmp(_tmp597.handler))
_tmp599=1;if(!_tmp599){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp517))->r;;_pop_handler();}
else{void*_tmp598=(void*)_exn_thrown;void*_tmp59B=_tmp598;_LL286: {struct Cyc_Dict_Absent_exn_struct*
_tmp59C=(struct Cyc_Dict_Absent_exn_struct*)_tmp59B;if(_tmp59C->tag != Cyc_Dict_Absent)
goto _LL288;}_LL287: {const char*_tmpC0B;void*_tmpC0A[1];struct Cyc_String_pa_PrintArg_struct
_tmpC09;(_tmpC09.tag=0,((_tmpC09.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp517)),((_tmpC0A[0]=& _tmpC09,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC0B="Can't find %s in exp_to_c, Var\n",
_tag_dyneither(_tmpC0B,sizeof(char),32))),_tag_dyneither(_tmpC0A,sizeof(void*),1)))))));}
_LL288:;_LL289:(void)_throw(_tmp59B);_LL285:;}};}goto _LL230;_LL237: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*
_tmp519=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp519->tag
!= 2)goto _LL239;else{_tmp51A=_tmp519->f1;_tmp51B=_tmp519->f2;}}_LL238: {struct
Cyc_List_List*_tmp5A0=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp51B);((void(*)(void(*
f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp51B);switch(_tmp51A){case Cyc_Absyn_Numelts:
_LL28A: {struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp51B))->hd;{void*_tmp5A1=Cyc_Tcutil_compress((void*)_check_null(
arg->topt));struct Cyc_Absyn_ArrayInfo _tmp5A3;struct Cyc_Absyn_Exp*_tmp5A4;struct
Cyc_Absyn_PtrInfo _tmp5A6;void*_tmp5A7;struct Cyc_Absyn_PtrAtts _tmp5A8;union Cyc_Absyn_Constraint*
_tmp5A9;union Cyc_Absyn_Constraint*_tmp5AA;union Cyc_Absyn_Constraint*_tmp5AB;
_LL28D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5A2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)
_tmp5A1;if(_tmp5A2->tag != 8)goto _LL28F;else{_tmp5A3=_tmp5A2->f1;_tmp5A4=_tmp5A3.num_elts;}}
_LL28E: if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp5A4))){
const char*_tmpC0E;void*_tmpC0D;(_tmpC0D=0,Cyc_Tcutil_terr(e->loc,((_tmpC0E="can't calculate numelts",
_tag_dyneither(_tmpC0E,sizeof(char),24))),_tag_dyneither(_tmpC0D,sizeof(void*),0)));}
e->r=_tmp5A4->r;goto _LL28C;_LL28F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp5A5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A1;if(_tmp5A5->tag
!= 5)goto _LL291;else{_tmp5A6=_tmp5A5->f1;_tmp5A7=_tmp5A6.elt_typ;_tmp5A8=_tmp5A6.ptr_atts;
_tmp5A9=_tmp5A8.nullable;_tmp5AA=_tmp5A8.bounds;_tmp5AB=_tmp5A8.zero_term;}}
_LL290:{void*_tmp5AE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp5AA);struct Cyc_Absyn_Exp*_tmp5B1;_LL294: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp5AF=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5AE;if(_tmp5AF->tag
!= 0)goto _LL296;}_LL295:{struct Cyc_Absyn_Exp*_tmpC0F[2];e->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__get_dyneither_size_e,((_tmpC0F[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp5A7),0),((_tmpC0F[0]=(struct Cyc_Absyn_Exp*)_tmp51B->hd,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0F,sizeof(struct Cyc_Absyn_Exp*),
2)))))));}goto _LL293;_LL296: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp5B0=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5AE;if(_tmp5B0->tag != 
1)goto _LL293;else{_tmp5B1=_tmp5B0->f1;}}_LL297: if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp5AB)){struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp51B->hd);struct
Cyc_Absyn_Exp*_tmpC10[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpC10[1]=
_tmp5B1,((_tmpC10[0]=(struct Cyc_Absyn_Exp*)_tmp51B->hd,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC10,sizeof(struct Cyc_Absyn_Exp*),
2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp5A9)){if(!Cyc_Evexp_c_can_eval(_tmp5B1)){const char*_tmpC13;void*_tmpC12;(
_tmpC12=0,Cyc_Tcutil_terr(e->loc,((_tmpC13="can't calculate numelts",
_tag_dyneither(_tmpC13,sizeof(char),24))),_tag_dyneither(_tmpC12,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp5B1,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp5B1->r;goto _LL293;}}goto _LL293;_LL293:;}goto _LL28C;_LL291:;_LL292: {const
char*_tmpC18;void*_tmpC17[2];struct Cyc_String_pa_PrintArg_struct _tmpC16;struct
Cyc_String_pa_PrintArg_struct _tmpC15;(_tmpC15.tag=0,((_tmpC15.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(
arg->topt))),((_tmpC16.tag=0,((_tmpC16.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC17[
0]=& _tmpC16,((_tmpC17[1]=& _tmpC15,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC18="size primop applied to non-array %s (%s)",
_tag_dyneither(_tmpC18,sizeof(char),41))),_tag_dyneither(_tmpC17,sizeof(void*),2)))))))))))));}
_LL28C:;}break;}case Cyc_Absyn_Plus: _LL28B:{void*_tmp5BA=Cyc_Tcutil_compress((
void*)((struct Cyc_List_List*)_check_null(_tmp5A0))->hd);struct Cyc_Absyn_PtrInfo
_tmp5BC;void*_tmp5BD;struct Cyc_Absyn_PtrAtts _tmp5BE;union Cyc_Absyn_Constraint*
_tmp5BF;union Cyc_Absyn_Constraint*_tmp5C0;_LL29A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp5BB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BA;if(_tmp5BB->tag
!= 5)goto _LL29C;else{_tmp5BC=_tmp5BB->f1;_tmp5BD=_tmp5BC.elt_typ;_tmp5BE=_tmp5BC.ptr_atts;
_tmp5BF=_tmp5BE.bounds;_tmp5C0=_tmp5BE.zero_term;}}_LL29B:{void*_tmp5C1=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp5BF);struct Cyc_Absyn_Exp*_tmp5C4;_LL29F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp5C2=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5C1;if(_tmp5C2->tag
!= 0)goto _LL2A1;}_LL2A0: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp51B))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp51B->tl))->hd;{struct Cyc_Absyn_Exp*_tmpC19[
3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpC19[2]=e2,((
_tmpC19[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5BD),0),((_tmpC19[0]=e1,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC19,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}goto _LL29E;}_LL2A1: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp5C3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5C1;if(_tmp5C3->tag != 
1)goto _LL29E;else{_tmp5C4=_tmp5C3->f1;}}_LL2A2: if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp5C0)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp51B))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp51B->tl))->hd;struct Cyc_Absyn_Exp*_tmpC1A[3];
e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
e1),((_tmpC1A[2]=e2,((_tmpC1A[1]=_tmp5C4,((_tmpC1A[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1A,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL29E;_LL29E:;}goto _LL299;_LL29C:;_LL29D: goto _LL299;
_LL299:;}break;case Cyc_Absyn_Minus: _LL298: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp5A0))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp51B))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp51B->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp5A0->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpC1B[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpC1B[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpC1B[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpC1B[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC1B,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
_LL2A3: goto _LL2A4;case Cyc_Absyn_Neq: _LL2A4: goto _LL2A5;case Cyc_Absyn_Gt: _LL2A5:
goto _LL2A6;case Cyc_Absyn_Gte: _LL2A6: goto _LL2A7;case Cyc_Absyn_Lt: _LL2A7: goto
_LL2A8;case Cyc_Absyn_Lte: _LL2A8: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp51B))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp51B->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp5A0))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp5A0->tl))->hd;void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& elt_typ))e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,
0));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp,0));break;}default: _LL2A9: break;}goto _LL230;}_LL239: {
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp51C=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)
_tmp511;if(_tmp51C->tag != 4)goto _LL23B;else{_tmp51D=_tmp51C->f1;_tmp51E=_tmp51C->f2;}}
_LL23A: {void*e2_cyc_typ=(void*)_check_null(_tmp51D->topt);void*ptr_type=(void*)&
Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;int
is_dyneither=0;const char*_tmpC1C;struct _dyneither_ptr incr_str=(_tmpC1C="increment",
_tag_dyneither(_tmpC1C,sizeof(char),10));if(_tmp51E == Cyc_Absyn_PreDec  || 
_tmp51E == Cyc_Absyn_PostDec){const char*_tmpC1D;incr_str=((_tmpC1D="decrement",
_tag_dyneither(_tmpC1D,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp51D,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpC21;void*_tmpC20[1];
struct Cyc_String_pa_PrintArg_struct _tmpC1F;(_tmpC1F.tag=0,((_tmpC1F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC20[0]=& _tmpC1F,Cyc_Tcutil_terr(
e->loc,((_tmpC21="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpC21,sizeof(char),74))),_tag_dyneither(_tmpC20,sizeof(void*),1)))))));}{
const char*_tmpC24;void*_tmpC23;(_tmpC23=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC24="in-place inc/dec on zero-term",
_tag_dyneither(_tmpC24,sizeof(char),30))),_tag_dyneither(_tmpC23,sizeof(void*),0)));};}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp51D,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp5CE=Cyc_Absyn_signed_int_exp(
1,0);_tmp5CE->topt=(void*)Cyc_Absyn_sint_typ;switch(_tmp51E){case Cyc_Absyn_PreInc:
_LL2AB:{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC2A;struct Cyc_Core_Opt*
_tmpC29;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC28;e->r=(void*)((
_tmpC28=_cycalloc(sizeof(*_tmpC28)),((_tmpC28[0]=((_tmpC2A.tag=3,((_tmpC2A.f1=
_tmp51D,((_tmpC2A.f2=((_tmpC29=_cycalloc_atomic(sizeof(*_tmpC29)),((_tmpC29->v=(
void*)Cyc_Absyn_Plus,_tmpC29)))),((_tmpC2A.f3=_tmp5CE,_tmpC2A)))))))),_tmpC28))));}
Cyc_Toc_exp_to_c(nv,e);return;case Cyc_Absyn_PreDec: _LL2AC:{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct
_tmpC30;struct Cyc_Core_Opt*_tmpC2F;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*
_tmpC2E;e->r=(void*)((_tmpC2E=_cycalloc(sizeof(*_tmpC2E)),((_tmpC2E[0]=((_tmpC30.tag=
3,((_tmpC30.f1=_tmp51D,((_tmpC30.f2=((_tmpC2F=_cycalloc_atomic(sizeof(*_tmpC2F)),((
_tmpC2F->v=(void*)Cyc_Absyn_Minus,_tmpC2F)))),((_tmpC30.f3=_tmp5CE,_tmpC30)))))))),
_tmpC2E))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL2AD:{const char*_tmpC34;
void*_tmpC33[1];struct Cyc_String_pa_PrintArg_struct _tmpC32;(_tmpC32.tag=0,((
_tmpC32.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC33[0]=&
_tmpC32,Cyc_Tcutil_terr(e->loc,((_tmpC34="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpC34,sizeof(char),59))),_tag_dyneither(_tmpC33,sizeof(void*),1)))))));}{
const char*_tmpC37;void*_tmpC36;(_tmpC36=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC37="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpC37,sizeof(char),34))),_tag_dyneither(_tmpC36,sizeof(void*),0)));};}}
Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp51D);Cyc_Toc_set_lhs(nv,0);{void*
elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp51E == Cyc_Absyn_PostInc
 || _tmp51E == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp51E == Cyc_Absyn_PreDec  || _tmp51E == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpC38[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC38[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpC38[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpC38[0]=Cyc_Toc_push_address_exp(_tmp51D),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC38,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Toc_functionSet*_tmp5DB=_tmp51E == Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:&
Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(
_tmp5DB,_tmp51D);struct Cyc_Absyn_Exp*_tmpC39[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((
_tmpC39[1]=Cyc_Absyn_signed_int_exp(1,0),((_tmpC39[0]=Cyc_Toc_push_address_exp(
_tmp51D),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC39,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(elt_typ == (
void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp51D)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp51D,0,Cyc_Toc_incr_lvalue,_tmp51E);e->r=_tmp51D->r;}}}goto _LL230;};};}_LL23B: {
struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp51F=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)
_tmp511;if(_tmp51F->tag != 3)goto _LL23D;else{_tmp520=_tmp51F->f1;_tmp521=_tmp51F->f2;
_tmp522=_tmp51F->f3;}}_LL23C: {void*e1_old_typ=(void*)_check_null(_tmp520->topt);
void*e2_old_typ=(void*)_check_null(_tmp522->topt);int f_tag=0;void*
tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(
_tmp520,& f_tag,& tagged_member_struct_type,1)){Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(
nv,_tmp520);Cyc_Toc_set_lhs(nv,0);Cyc_Toc_exp_to_c(nv,_tmp522);e->r=Cyc_Toc_tagged_union_assignop(
_tmp520,e1_old_typ,_tmp521,_tmp522,e2_old_typ,f_tag,tagged_member_struct_type);
return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(_tmp520,& ptr_type,&
is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp520,_tmp521,
_tmp522,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp520);Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp520);Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp522);{int done=0;if(_tmp521 != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;switch((enum Cyc_Absyn_Primop)_tmp521->v){case Cyc_Absyn_Plus: _LL2AF:
change=_tmp522;break;case Cyc_Absyn_Minus: _LL2B0: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,
_tmp522,0);break;default: _LL2B1: {const char*_tmpC3C;void*_tmpC3B;(_tmpC3B=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC3C="bad t ? pointer arithmetic",_tag_dyneither(_tmpC3C,sizeof(char),27))),
_tag_dyneither(_tmpC3B,sizeof(void*),0)));}}done=1;{struct Cyc_Absyn_Exp*_tmp5E0=
Cyc_Toc__dyneither_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*_tmpC3D[3];e->r=Cyc_Toc_fncall_exp_r(
_tmp5E0,((_tmpC3D[2]=change,((_tmpC3D[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpC3D[0]=Cyc_Toc_push_address_exp(_tmp520),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC3D,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
switch((enum Cyc_Absyn_Primop)_tmp521->v){case Cyc_Absyn_Plus: _LL2B3: done=1;{
struct Cyc_Absyn_Exp*_tmpC3E[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp520),((_tmpC3E[1]=_tmp522,((
_tmpC3E[0]=_tmp520,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC3E,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL2B4: {const char*_tmpC41;void*_tmpC40;(_tmpC40=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC41="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpC41,sizeof(char),39))),_tag_dyneither(_tmpC40,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp522->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp522->r,0));if(!Cyc_Absyn_is_lvalue(_tmp520)){{struct _tuple20 _tmpC44;struct
_tuple20*_tmpC43;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple20*),struct _tuple20*f_env))
Cyc_Toc_lvalue_assign)(_tmp520,0,Cyc_Toc_assignop_lvalue,((_tmpC43=_cycalloc(
sizeof(struct _tuple20)* 1),((_tmpC43[0]=((_tmpC44.f1=_tmp521,((_tmpC44.f2=
_tmp522,_tmpC44)))),_tmpC43)))));}e->r=_tmp520->r;}}goto _LL230;};};};}_LL23D: {
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp523=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)
_tmp511;if(_tmp523->tag != 5)goto _LL23F;else{_tmp524=_tmp523->f1;_tmp525=_tmp523->f2;
_tmp526=_tmp523->f3;}}_LL23E: Cyc_Toc_exp_to_c(nv,_tmp524);Cyc_Toc_exp_to_c(nv,
_tmp525);Cyc_Toc_exp_to_c(nv,_tmp526);goto _LL230;_LL23F: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*
_tmp527=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp527->tag != 6)
goto _LL241;else{_tmp528=_tmp527->f1;_tmp529=_tmp527->f2;}}_LL240: Cyc_Toc_exp_to_c(
nv,_tmp528);Cyc_Toc_exp_to_c(nv,_tmp529);goto _LL230;_LL241: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*
_tmp52A=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp52A->tag != 7)
goto _LL243;else{_tmp52B=_tmp52A->f1;_tmp52C=_tmp52A->f2;}}_LL242: Cyc_Toc_exp_to_c(
nv,_tmp52B);Cyc_Toc_exp_to_c(nv,_tmp52C);goto _LL230;_LL243: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*
_tmp52D=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp52D->tag
!= 8)goto _LL245;else{_tmp52E=_tmp52D->f1;_tmp52F=_tmp52D->f2;}}_LL244: Cyc_Toc_exp_to_c(
nv,_tmp52E);Cyc_Toc_exp_to_c(nv,_tmp52F);goto _LL230;_LL245: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*
_tmp530=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp530->tag
!= 9)goto _LL247;else{_tmp531=_tmp530->f1;_tmp532=_tmp530->f2;_tmp533=_tmp530->f3;
if(_tmp533 != 0)goto _LL247;}}_LL246: Cyc_Toc_exp_to_c(nv,_tmp531);((void(*)(void(*
f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp532);goto _LL230;_LL247: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*
_tmp534=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp534->tag
!= 9)goto _LL249;else{_tmp535=_tmp534->f1;_tmp536=_tmp534->f2;_tmp537=_tmp534->f3;
if(_tmp537 == 0)goto _LL249;_tmp538=*_tmp537;_tmp539=_tmp538.num_varargs;_tmp53A=
_tmp538.injectors;_tmp53B=_tmp538.vai;}}_LL248: {struct _RegionHandle _tmp5E7=
_new_region("r");struct _RegionHandle*r=& _tmp5E7;_push_region(r);{struct _tuple0*
argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp539,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp53B->type);void*arr_type=Cyc_Absyn_array_typ(
cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)num_varargs_exp,Cyc_Absyn_false_conref,
0);int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp536);int
num_normargs=num_args - _tmp539;struct Cyc_List_List*new_args=0;{int i=0;for(0;i < 
num_normargs;(++ i,_tmp536=_tmp536->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp536))->hd);{struct Cyc_List_List*_tmpC45;
new_args=((_tmpC45=_cycalloc(sizeof(*_tmpC45)),((_tmpC45->hd=(struct Cyc_Absyn_Exp*)
_tmp536->hd,((_tmpC45->tl=new_args,_tmpC45))))));};}}{struct Cyc_Absyn_Exp*
_tmpC48[3];struct Cyc_List_List*_tmpC47;new_args=((_tmpC47=_cycalloc(sizeof(*
_tmpC47)),((_tmpC47->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpC48[
2]=num_varargs_exp,((_tmpC48[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpC48[0]=
argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC48,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpC47->tl=
new_args,_tmpC47))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp535);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp535,new_args,0),0);if(_tmp53B->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp5EB=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp53B->type));struct Cyc_Absyn_DatatypeInfo _tmp5ED;union Cyc_Absyn_DatatypeInfoU
_tmp5EE;struct Cyc_Absyn_Datatypedecl**_tmp5EF;struct Cyc_Absyn_Datatypedecl*
_tmp5F0;_LL2B7: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5EC=(struct
Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5EB;if(_tmp5EC->tag != 3)goto _LL2B9;
else{_tmp5ED=_tmp5EC->f1;_tmp5EE=_tmp5ED.datatype_info;if((_tmp5EE.KnownDatatype).tag
!= 2)goto _LL2B9;_tmp5EF=(struct Cyc_Absyn_Datatypedecl**)(_tmp5EE.KnownDatatype).val;
_tmp5F0=*_tmp5EF;}}_LL2B8: tud=_tmp5F0;goto _LL2B6;_LL2B9:;_LL2BA: {const char*
_tmpC4B;void*_tmpC4A;(_tmpC4A=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC4B="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpC4B,sizeof(char),44))),_tag_dyneither(_tmpC4A,sizeof(void*),0)));}
_LL2B6:;}{unsigned int _tmpC50;unsigned int _tmpC4F;struct _dyneither_ptr _tmpC4E;
struct _tuple0**_tmpC4D;unsigned int _tmpC4C;struct _dyneither_ptr vs=(_tmpC4C=(
unsigned int)_tmp539,((_tmpC4D=(struct _tuple0**)_region_malloc(r,_check_times(
sizeof(struct _tuple0*),_tmpC4C)),((_tmpC4E=_tag_dyneither(_tmpC4D,sizeof(struct
_tuple0*),_tmpC4C),((((_tmpC4F=_tmpC4C,_tmpC51(& _tmpC50,& _tmpC4F,& _tmpC4D))),
_tmpC4E)))))));if(_tmp539 != 0){struct Cyc_List_List*_tmp5F3=0;{int i=_tmp539 - 1;
for(0;i >= 0;-- i){struct Cyc_List_List*_tmpC52;_tmp5F3=((_tmpC52=_cycalloc(sizeof(*
_tmpC52)),((_tmpC52->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((
struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i)),0),0)),((
_tmpC52->tl=_tmp5F3,_tmpC52))))));}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp5F3,0),s,0);{int i=0;for(0;
_tmp536 != 0;(((_tmp536=_tmp536->tl,_tmp53A=_tmp53A->tl)),++ i)){struct Cyc_Absyn_Exp*
arg=(struct Cyc_Absyn_Exp*)_tmp536->hd;void*arg_type=(void*)_check_null(arg->topt);
struct _tuple0*var=*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct
_tuple0*),i));struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield
_tmp5F6;struct _tuple0*_tmp5F7;struct Cyc_List_List*_tmp5F8;struct Cyc_Absyn_Datatypefield*
_tmp5F5=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp53A))->hd;_tmp5F6=*_tmp5F5;_tmp5F7=_tmp5F6.name;_tmp5F8=_tmp5F6.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp5F8))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_boxed_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp5F7),0),s,0);s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(
_tmp5F7,tud->name)),0,s,0);};}};}else{struct _tuple16*_tmpC53[3];struct Cyc_List_List*
_tmp5F9=(_tmpC53[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC53[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpC53[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC53,sizeof(struct _tuple16*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{{int i=0;for(0;_tmp536
!= 0;(_tmp536=_tmp536->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp536->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp536->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);};}_npop_handler(0);goto _LL230;;_pop_region(r);}_LL249: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*
_tmp53C=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp53C->tag != 
10)goto _LL24B;else{_tmp53D=_tmp53C->f1;}}_LL24A: Cyc_Toc_exp_to_c(nv,_tmp53D);e->r=(
Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp53D),
0))->r;goto _LL230;_LL24B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*
_tmp53E=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp511;if(
_tmp53E->tag != 11)goto _LL24D;else{_tmp53F=_tmp53E->f1;}}_LL24C: Cyc_Toc_exp_to_c(
nv,_tmp53F);goto _LL230;_LL24D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*
_tmp540=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp540->tag
!= 12)goto _LL24F;else{_tmp541=_tmp540->f1;_tmp542=_tmp540->f2;}}_LL24E: Cyc_Toc_exp_to_c(
nv,_tmp541);for(0;_tmp542 != 0;_tmp542=_tmp542->tl){enum Cyc_Absyn_KindQual
_tmp5FF=(Cyc_Tcutil_typ_kind((void*)_tmp542->hd))->kind;if(_tmp5FF != Cyc_Absyn_EffKind
 && _tmp5FF != Cyc_Absyn_RgnKind){{void*_tmp600=Cyc_Tcutil_compress((void*)
_tmp542->hd);_LL2BC: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp601=(struct
Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp600;if(_tmp601->tag != 2)goto _LL2BE;}
_LL2BD: goto _LL2BF;_LL2BE: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*
_tmp602=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp600;if(_tmp602->tag
!= 3)goto _LL2C0;}_LL2BF: continue;_LL2C0:;_LL2C1: e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp541,0))->r;goto _LL2BB;_LL2BB:;}
break;}}goto _LL230;_LL24F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp543=(
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp543->tag != 13)goto
_LL251;else{_tmp544=(void**)& _tmp543->f1;_tmp545=(void**)((void**)& _tmp543->f1);
_tmp546=_tmp543->f2;_tmp547=_tmp543->f3;_tmp548=_tmp543->f4;}}_LL250: {void*
old_t2=(void*)_check_null(_tmp546->topt);void*new_typ=*_tmp545;*_tmp545=Cyc_Toc_typ_to_c(
new_typ);Cyc_Toc_exp_to_c(nv,_tmp546);{struct _tuple19 _tmpC54;struct _tuple19
_tmp604=(_tmpC54.f1=Cyc_Tcutil_compress(old_t2),((_tmpC54.f2=Cyc_Tcutil_compress(
new_typ),_tmpC54)));void*_tmp605;struct Cyc_Absyn_PtrInfo _tmp607;void*_tmp608;
struct Cyc_Absyn_PtrInfo _tmp60A;void*_tmp60B;struct Cyc_Absyn_PtrInfo _tmp60D;void*
_tmp60E;_LL2C3: _tmp605=_tmp604.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp606=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp605;if(_tmp606->tag
!= 5)goto _LL2C5;else{_tmp607=_tmp606->f1;}};_tmp608=_tmp604.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp609=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp608;if(_tmp609->tag
!= 5)goto _LL2C5;else{_tmp60A=_tmp609->f1;}};_LL2C4: {int _tmp610=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp607.ptr_atts).nullable);
int _tmp611=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp60A.ptr_atts).nullable);void*_tmp612=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp607.ptr_atts).bounds);void*
_tmp613=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(
_tmp60A.ptr_atts).bounds);int _tmp614=((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,(_tmp607.ptr_atts).zero_term);int _tmp615=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp60A.ptr_atts).zero_term);{
struct _tuple19 _tmpC55;struct _tuple19 _tmp617=(_tmpC55.f1=_tmp612,((_tmpC55.f2=
_tmp613,_tmpC55)));void*_tmp618;struct Cyc_Absyn_Exp*_tmp61A;void*_tmp61B;struct
Cyc_Absyn_Exp*_tmp61D;void*_tmp61E;struct Cyc_Absyn_Exp*_tmp620;void*_tmp621;void*
_tmp623;void*_tmp625;struct Cyc_Absyn_Exp*_tmp627;void*_tmp628;void*_tmp62A;
_LL2CA: _tmp618=_tmp617.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp619=(
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp618;if(_tmp619->tag != 1)goto
_LL2CC;else{_tmp61A=_tmp619->f1;}};_tmp61B=_tmp617.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp61C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp61B;if(_tmp61C->tag != 
1)goto _LL2CC;else{_tmp61D=_tmp61C->f1;}};_LL2CB: if((!Cyc_Evexp_c_can_eval(
_tmp61A) || !Cyc_Evexp_c_can_eval(_tmp61D)) && !Cyc_Evexp_same_const_exp(_tmp61A,
_tmp61D)){const char*_tmpC58;void*_tmpC57;(_tmpC57=0,Cyc_Tcutil_terr(e->loc,((
_tmpC58="can't validate cast due to potential size differences",_tag_dyneither(
_tmpC58,sizeof(char),54))),_tag_dyneither(_tmpC57,sizeof(void*),0)));}if(_tmp610
 && !_tmp611){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC5B;void*_tmpC5A;(
_tmpC5A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC5B="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC5B,
sizeof(char),44))),_tag_dyneither(_tmpC5A,sizeof(void*),0)));}if(_tmp548 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC5F;void*_tmpC5E[1];struct Cyc_String_pa_PrintArg_struct _tmpC5D;(
_tmpC5D.tag=0,((_tmpC5D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpC5E[0]=& _tmpC5D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpC5F="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpC5F,sizeof(char),41))),_tag_dyneither(_tmpC5E,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp546);if(do_null_check){if(!_tmp547){
const char*_tmpC62;void*_tmpC61;(_tmpC61=0,Cyc_Tcutil_warn(e->loc,((_tmpC62="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpC62,sizeof(char),58))),_tag_dyneither(_tmpC61,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC63;e->r=Cyc_Toc_cast_it_r(*_tmp545,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC63=_cycalloc(sizeof(*_tmpC63)),((_tmpC63->hd=
_tmp546,((_tmpC63->tl=0,_tmpC63)))))),0));};}};}goto _LL2C9;_LL2CC: _tmp61E=
_tmp617.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp61F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)
_tmp61E;if(_tmp61F->tag != 1)goto _LL2CE;else{_tmp620=_tmp61F->f1;}};_tmp621=
_tmp617.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp622=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)
_tmp621;if(_tmp622->tag != 0)goto _LL2CE;};_LL2CD: if(!Cyc_Evexp_c_can_eval(_tmp620)){
const char*_tmpC66;void*_tmpC65;(_tmpC65=0,Cyc_Tcutil_terr(e->loc,((_tmpC66="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC66,sizeof(char),71))),_tag_dyneither(_tmpC65,sizeof(void*),0)));}
if(_tmp548 == Cyc_Absyn_NonNull_to_Null){const char*_tmpC6A;void*_tmpC69[1];struct
Cyc_String_pa_PrintArg_struct _tmpC68;(_tmpC68.tag=0,((_tmpC68.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC69[0]=&
_tmpC68,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC6A="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpC6A,
sizeof(char),44))),_tag_dyneither(_tmpC69,sizeof(void*),1)))))));}if(Cyc_Toc_is_toplevel(
nv)){if((_tmp614  && !(_tmp60A.elt_tq).real_const) && !_tmp615)_tmp620=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Minus,_tmp620,Cyc_Absyn_uint_exp(1,0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(
old_t2,_tmp620,_tmp546))->r;}else{struct Cyc_Absyn_Exp*_tmp63B=Cyc_Toc__tag_dyneither_e;
if(_tmp614){struct _tuple0*_tmp63C=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp63D=
Cyc_Absyn_var_exp(_tmp63C,0);struct Cyc_Absyn_Exp*arg3;{void*_tmp63E=_tmp546->r;
union Cyc_Absyn_Cnst _tmp640;struct _dyneither_ptr _tmp641;union Cyc_Absyn_Cnst
_tmp643;struct _dyneither_ptr _tmp644;_LL2D3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp63F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp63E;if(_tmp63F->tag != 
0)goto _LL2D5;else{_tmp640=_tmp63F->f1;if((_tmp640.String_c).tag != 8)goto _LL2D5;
_tmp641=(struct _dyneither_ptr)(_tmp640.String_c).val;}}_LL2D4: arg3=_tmp620;goto
_LL2D2;_LL2D5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp642=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmp63E;if(_tmp642->tag != 0)goto _LL2D7;else{_tmp643=_tmp642->f1;if((_tmp643.Wstring_c).tag
!= 9)goto _LL2D7;_tmp644=(struct _dyneither_ptr)(_tmp643.Wstring_c).val;}}_LL2D6:
arg3=_tmp620;goto _LL2D2;_LL2D7:;_LL2D8:{struct Cyc_Absyn_Exp*_tmpC6B[2];arg3=Cyc_Absyn_fncall_exp(
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp546),((
_tmpC6B[1]=_tmp620,((_tmpC6B[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp63D),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC6B,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}goto _LL2D2;
_LL2D2:;}if(!_tmp615  && !(_tmp60A.elt_tq).real_const)arg3=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp646=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp60A.elt_typ),0);struct Cyc_Absyn_Exp*_tmpC6C[3];struct Cyc_Absyn_Exp*
_tmp647=Cyc_Absyn_fncall_exp(_tmp63B,((_tmpC6C[2]=arg3,((_tmpC6C[1]=_tmp646,((
_tmpC6C[0]=_tmp63D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC6C,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);struct Cyc_Absyn_Stmt*
_tmp648=Cyc_Absyn_exp_stmt(_tmp647,0);_tmp648=Cyc_Absyn_declare_stmt(_tmp63C,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp546,_tmp648,0);e->r=Cyc_Toc_stmt_exp_r(_tmp648);};}
else{struct Cyc_Absyn_Exp*_tmpC6D[3];e->r=Cyc_Toc_fncall_exp_r(_tmp63B,((_tmpC6D[
2]=_tmp620,((_tmpC6D[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp60A.elt_typ),
0),((_tmpC6D[0]=_tmp546,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC6D,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2C9;
_LL2CE: _tmp623=_tmp617.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp624=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp623;if(_tmp624->tag
!= 0)goto _LL2D0;};_tmp625=_tmp617.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp626=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp625;if(_tmp626->tag != 
1)goto _LL2D0;else{_tmp627=_tmp626->f1;}};_LL2CF: if(!Cyc_Evexp_c_can_eval(_tmp627)){
const char*_tmpC70;void*_tmpC6F;(_tmpC6F=0,Cyc_Tcutil_terr(e->loc,((_tmpC70="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC70,sizeof(char),71))),_tag_dyneither(_tmpC6F,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC73;void*_tmpC72;(_tmpC72=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC73="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpC73,sizeof(char),45))),_tag_dyneither(_tmpC72,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp64F=_tmp627;if(_tmp614  && !_tmp615)_tmp64F=Cyc_Absyn_add_exp(
_tmp627,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp650=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC74[3];struct Cyc_Absyn_Exp*_tmp651=Cyc_Absyn_fncall_exp(
_tmp650,((_tmpC74[2]=_tmp64F,((_tmpC74[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp607.elt_typ),0),((_tmpC74[0]=_tmp546,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC74,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp611){struct Cyc_List_List*_tmpC75;_tmp651->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpC75=_cycalloc(sizeof(*_tmpC75)),((_tmpC75->hd=Cyc_Absyn_copy_exp(
_tmp651),((_tmpC75->tl=0,_tmpC75)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp545,_tmp651);
goto _LL2C9;};};_LL2D0: _tmp628=_tmp617.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp629=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp628;if(_tmp629->tag
!= 0)goto _LL2C9;};_tmp62A=_tmp617.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp62B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp62A;if(_tmp62B->tag
!= 0)goto _LL2C9;};_LL2D1: DynCast: if((_tmp614  && !_tmp615) && !(_tmp60A.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC78;void*_tmpC77;(_tmpC77=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC78="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpC78,sizeof(char),70))),_tag_dyneither(_tmpC77,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp656=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpC79[3];e->r=Cyc_Toc_fncall_exp_r(_tmp656,((_tmpC79[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpC79[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp607.elt_typ),0),((
_tmpC79[0]=_tmp546,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC79,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}goto _LL2C9;
_LL2C9:;}goto _LL2C2;}_LL2C5: _tmp60B=_tmp604.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp60C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60B;if(_tmp60C->tag
!= 5)goto _LL2C7;else{_tmp60D=_tmp60C->f1;}};_tmp60E=_tmp604.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp60F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp60E;if(_tmp60F->tag != 6)
goto _LL2C7;};_LL2C6:{void*_tmp658=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp60D.ptr_atts).bounds);_LL2DA: {
struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp659=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)
_tmp658;if(_tmp659->tag != 0)goto _LL2DC;}_LL2DB: _tmp546->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(_tmp546->r,_tmp546->loc),Cyc_Toc_curr_sp);goto _LL2D9;_LL2DC:;
_LL2DD: goto _LL2D9;_LL2D9:;}goto _LL2C2;_LL2C7:;_LL2C8: goto _LL2C2;_LL2C2:;}goto
_LL230;}_LL251: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp549=(struct
Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp549->tag != 14)goto
_LL253;else{_tmp54A=_tmp549->f1;}}_LL252:{void*_tmp65A=_tmp54A->r;struct _tuple0*
_tmp65C;struct Cyc_List_List*_tmp65D;struct Cyc_List_List*_tmp65E;struct Cyc_List_List*
_tmp660;_LL2DF: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp65B=(
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp65A;if(_tmp65B->tag != 27)
goto _LL2E1;else{_tmp65C=_tmp65B->f1;_tmp65D=_tmp65B->f2;_tmp65E=_tmp65B->f3;}}
_LL2E0: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC7D;void*_tmpC7C[1];struct Cyc_String_pa_PrintArg_struct
_tmpC7B;(_tmpC7B.tag=0,((_tmpC7B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp54A->loc)),((_tmpC7C[0]=& _tmpC7B,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC7D="%s: & on non-identifiers at the top-level",
_tag_dyneither(_tmpC7D,sizeof(char),42))),_tag_dyneither(_tmpC7C,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp54A->topt),_tmp65D,1,0,
_tmp65E,_tmp65C))->r;goto _LL2DE;_LL2E1: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*
_tmp65F=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp65A;if(_tmp65F->tag != 
23)goto _LL2E3;else{_tmp660=_tmp65F->f1;}}_LL2E2: if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC81;void*_tmpC80[1];struct Cyc_String_pa_PrintArg_struct _tmpC7F;(
_tmpC7F.tag=0,((_tmpC7F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
_tmp54A->loc)),((_tmpC80[0]=& _tmpC7F,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC81="%s: & on non-identifiers at the top-level",
_tag_dyneither(_tmpC81,sizeof(char),42))),_tag_dyneither(_tmpC80,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp660))->r;goto _LL2DE;_LL2E3:;_LL2E4: Cyc_Toc_set_lhs(
nv,1);Cyc_Toc_exp_to_c(nv,_tmp54A);Cyc_Toc_set_lhs(nv,0);if(!Cyc_Absyn_is_lvalue(
_tmp54A)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp54A,0,Cyc_Toc_address_lvalue,
1);e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp54A);}
goto _LL2DE;_LL2DE:;}goto _LL230;_LL253: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*
_tmp54B=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp54B->tag != 
15)goto _LL255;else{_tmp54C=_tmp54B->f1;_tmp54D=_tmp54B->f2;}}_LL254: if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpC85;void*_tmpC84[1];struct Cyc_String_pa_PrintArg_struct
_tmpC83;(_tmpC83.tag=0,((_tmpC83.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp54D->loc)),((_tmpC84[0]=& _tmpC83,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC85="%s: new at top-level",
_tag_dyneither(_tmpC85,sizeof(char),21))),_tag_dyneither(_tmpC84,sizeof(void*),1)))))));}{
void*_tmp66A=_tmp54D->r;struct Cyc_List_List*_tmp66C;struct Cyc_Absyn_Vardecl*
_tmp66E;struct Cyc_Absyn_Exp*_tmp66F;struct Cyc_Absyn_Exp*_tmp670;int _tmp671;
struct _tuple0*_tmp673;struct Cyc_List_List*_tmp674;struct Cyc_List_List*_tmp675;
struct Cyc_Absyn_Aggrdecl*_tmp676;struct Cyc_List_List*_tmp678;_LL2E6: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*
_tmp66B=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp66A;if(_tmp66B->tag != 
25)goto _LL2E8;else{_tmp66C=_tmp66B->f1;}}_LL2E7: {struct _tuple0*_tmp679=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp67A=Cyc_Absyn_var_exp(_tmp679,0);struct Cyc_Absyn_Stmt*
_tmp67B=Cyc_Toc_init_array(nv,_tmp67A,_tmp66C,Cyc_Absyn_exp_stmt(_tmp67A,0));
void*old_elt_typ;{void*_tmp67C=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp67E;void*_tmp67F;struct Cyc_Absyn_Tqual _tmp680;struct Cyc_Absyn_PtrAtts _tmp681;
union Cyc_Absyn_Constraint*_tmp682;_LL2F1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp67D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp67C;if(_tmp67D->tag
!= 5)goto _LL2F3;else{_tmp67E=_tmp67D->f1;_tmp67F=_tmp67E.elt_typ;_tmp680=_tmp67E.elt_tq;
_tmp681=_tmp67E.ptr_atts;_tmp682=_tmp681.zero_term;}}_LL2F2: old_elt_typ=_tmp67F;
goto _LL2F0;_LL2F3:;_LL2F4: {const char*_tmpC88;void*_tmpC87;old_elt_typ=((_tmpC87=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC88="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpC88,sizeof(char),52))),_tag_dyneither(_tmpC87,sizeof(void*),0))));}_LL2F0:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp685=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp686=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp66C),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp54C == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp686);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp54C;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp686);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp679,_tmp685,(struct Cyc_Absyn_Exp*)e1,_tmp67B,0));goto
_LL2E5;};}_LL2E8: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp66D=(
struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp66A;if(_tmp66D->tag != 
26)goto _LL2EA;else{_tmp66E=_tmp66D->f1;_tmp66F=_tmp66D->f2;_tmp670=_tmp66D->f3;
_tmp671=_tmp66D->f4;}}_LL2E9: {int is_dyneither_ptr=0;{void*_tmp687=Cyc_Tcutil_compress(
old_typ);struct Cyc_Absyn_PtrInfo _tmp689;void*_tmp68A;struct Cyc_Absyn_Tqual
_tmp68B;struct Cyc_Absyn_PtrAtts _tmp68C;union Cyc_Absyn_Constraint*_tmp68D;union
Cyc_Absyn_Constraint*_tmp68E;_LL2F6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp688=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp687;if(_tmp688->tag
!= 5)goto _LL2F8;else{_tmp689=_tmp688->f1;_tmp68A=_tmp689.elt_typ;_tmp68B=_tmp689.elt_tq;
_tmp68C=_tmp689.ptr_atts;_tmp68D=_tmp68C.bounds;_tmp68E=_tmp68C.zero_term;}}
_LL2F7: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp68D)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2F5;
_LL2F8:;_LL2F9: {const char*_tmpC8B;void*_tmpC8A;(_tmpC8A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC8B="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpC8B,sizeof(char),42))),_tag_dyneither(_tmpC8A,sizeof(void*),0)));}
_LL2F5:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)_check_null(_tmp670->topt);void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(
nv,_tmp66F);{struct Cyc_Absyn_Exp*_tmp691=Cyc_Absyn_var_exp(max,0);if(_tmp671)
_tmp691=Cyc_Absyn_add_exp(_tmp691,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Stmt*
s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(a,0),_tmp66E,Cyc_Absyn_var_exp(
max,0),_tmp670,_tmp671,Cyc_Toc_skip_stmt_dl(),1);struct _RegionHandle _tmp692=
_new_region("r");struct _RegionHandle*r=& _tmp692;_push_region(r);{struct _tuple21*
_tmpC8E;struct Cyc_List_List*_tmpC8D;struct Cyc_List_List*decls=(_tmpC8D=
_region_malloc(r,sizeof(*_tmpC8D)),((_tmpC8D->hd=((_tmpC8E=_region_malloc(r,
sizeof(*_tmpC8E)),((_tmpC8E->f1=max,((_tmpC8E->f2=Cyc_Absyn_uint_typ,((_tmpC8E->f3=(
struct Cyc_Absyn_Exp*)_tmp66F,_tmpC8E)))))))),((_tmpC8D->tl=0,_tmpC8D)))));struct
Cyc_Absyn_Exp*ai;if(_tmp54C == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*
_tmpC8F[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((
_tmpC8F[1]=_tmp691,((_tmpC8F[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC8F,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp54C;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpC90[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpC90[1]=_tmp691,((_tmpC90[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC90,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple21*_tmpC93;struct Cyc_List_List*
_tmpC92;decls=((_tmpC92=_region_malloc(r,sizeof(*_tmpC92)),((_tmpC92->hd=((
_tmpC93=_region_malloc(r,sizeof(*_tmpC93)),((_tmpC93->f1=a,((_tmpC93->f2=ptr_typ,((
_tmpC93->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpC93)))))))),((_tmpC92->tl=decls,
_tmpC92))))));}if(is_dyneither_ptr){struct _tuple0*_tmp697=Cyc_Toc_temp_var();
void*_tmp698=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp699=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC94[3];struct Cyc_Absyn_Exp*_tmp69A=Cyc_Absyn_fncall_exp(
_tmp699,((_tmpC94[2]=_tmp691,((_tmpC94[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpC94[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC94,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple21*_tmpC97;struct Cyc_List_List*_tmpC96;decls=((_tmpC96=
_region_malloc(r,sizeof(*_tmpC96)),((_tmpC96->hd=((_tmpC97=_region_malloc(r,
sizeof(*_tmpC97)),((_tmpC97->f1=_tmp697,((_tmpC97->f2=_tmp698,((_tmpC97->f3=(
struct Cyc_Absyn_Exp*)_tmp69A,_tmpC97)))))))),((_tmpC96->tl=decls,_tmpC96))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp697,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp69E=decls;for(0;_tmp69E != 0;_tmp69E=_tmp69E->tl){struct
_tuple0*_tmp6A0;void*_tmp6A1;struct Cyc_Absyn_Exp*_tmp6A2;struct _tuple21 _tmp69F=*((
struct _tuple21*)_tmp69E->hd);_tmp6A0=_tmp69F.f1;_tmp6A1=_tmp69F.f2;_tmp6A2=
_tmp69F.f3;s=Cyc_Absyn_declare_stmt(_tmp6A0,_tmp6A1,_tmp6A2,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);};}_npop_handler(0);goto _LL2E5;;_pop_region(r);};};};}_LL2EA: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*
_tmp672=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp66A;if(_tmp672->tag
!= 27)goto _LL2EC;else{_tmp673=_tmp672->f1;_tmp674=_tmp672->f2;_tmp675=_tmp672->f3;
_tmp676=_tmp672->f4;}}_LL2EB: e->r=(Cyc_Toc_init_struct(nv,(void*)_check_null(
_tmp54D->topt),_tmp674,1,_tmp54C,_tmp675,_tmp673))->r;goto _LL2E5;_LL2EC: {struct
Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp677=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)
_tmp66A;if(_tmp677->tag != 23)goto _LL2EE;else{_tmp678=_tmp677->f1;}}_LL2ED: e->r=(
Cyc_Toc_init_tuple(nv,1,_tmp54C,_tmp678))->r;goto _LL2E5;_LL2EE:;_LL2EF: {void*
old_elt_typ=(void*)_check_null(_tmp54D->topt);void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);struct _tuple0*_tmp6A5=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp6A6=Cyc_Absyn_var_exp(_tmp6A5,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(_tmp6A6,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp54C
== 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp54C;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp6A7=_tmp54D->r;void*_tmp6A9;
struct Cyc_Absyn_Exp*_tmp6AA;_LL2FB: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*
_tmp6A8=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6A7;if(_tmp6A8->tag != 
13)goto _LL2FD;else{_tmp6A9=(void*)_tmp6A8->f1;_tmp6AA=_tmp6A8->f2;}}_LL2FC:{
struct _tuple19 _tmpC98;struct _tuple19 _tmp6AC=(_tmpC98.f1=Cyc_Tcutil_compress(
_tmp6A9),((_tmpC98.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp6AA->topt)),
_tmpC98)));void*_tmp6AD;struct Cyc_Absyn_PtrInfo _tmp6AF;void*_tmp6B0;struct Cyc_Absyn_PtrAtts
_tmp6B1;union Cyc_Absyn_Constraint*_tmp6B2;void*_tmp6B3;struct Cyc_Absyn_PtrInfo
_tmp6B5;struct Cyc_Absyn_PtrAtts _tmp6B6;union Cyc_Absyn_Constraint*_tmp6B7;_LL300:
_tmp6AD=_tmp6AC.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6AE=(
struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6AD;if(_tmp6AE->tag != 5)goto
_LL302;else{_tmp6AF=_tmp6AE->f1;_tmp6B0=_tmp6AF.elt_typ;_tmp6B1=_tmp6AF.ptr_atts;
_tmp6B2=_tmp6B1.bounds;}};_tmp6B3=_tmp6AC.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp6B4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B3;if(_tmp6B4->tag
!= 5)goto _LL302;else{_tmp6B5=_tmp6B4->f1;_tmp6B6=_tmp6B5.ptr_atts;_tmp6B7=
_tmp6B6.bounds;}};_LL301:{struct _tuple19 _tmpC99;struct _tuple19 _tmp6B9=(_tmpC99.f1=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6B2),((_tmpC99.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6B7),_tmpC99)));void*_tmp6BA;void*_tmp6BC;struct Cyc_Absyn_Exp*
_tmp6BE;_LL305: _tmp6BA=_tmp6B9.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp6BB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6BA;if(_tmp6BB->tag
!= 0)goto _LL307;};_tmp6BC=_tmp6B9.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp6BD=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6BC;if(_tmp6BD->tag != 
1)goto _LL307;else{_tmp6BE=_tmp6BD->f1;}};_LL306: Cyc_Toc_exp_to_c(nv,_tmp6AA);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp6BF=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpC9A[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp6BF,((_tmpC9A[3]=_tmp6BE,((_tmpC9A[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp6B0),0),((_tmpC9A[1]=_tmp6AA,((_tmpC9A[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC9A,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL304;};_LL307:;_LL308: goto _LL304;_LL304:;}goto _LL2FF;_LL302:;
_LL303: goto _LL2FF;_LL2FF:;}goto _LL2FA;_LL2FD:;_LL2FE: goto _LL2FA;_LL2FA:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp6C1=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp6A6),
0);struct Cyc_Absyn_Exp*_tmp6C2=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,
_tmp54D);_tmp6C1=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
_tmp6A6,_tmp6C2,0),_tmp54D,0),_tmp6C1,0);{void*_tmp6C3=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp6A5,
_tmp6C3,(struct Cyc_Absyn_Exp*)mexp,_tmp6C1,0));};}goto _LL2E5;};}_LL2E5:;}goto
_LL230;_LL255: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp54E=(struct
Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp54E->tag != 17)goto
_LL257;else{_tmp54F=_tmp54E->f1;}}_LL256: Cyc_Toc_exp_to_c(nv,_tmp54F);goto _LL230;
_LL257: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp550=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)
_tmp511;if(_tmp550->tag != 16)goto _LL259;else{_tmp551=(void*)_tmp550->f1;}}_LL258:{
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpC9D;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*
_tmpC9C;e->r=(void*)((_tmpC9C=_cycalloc(sizeof(*_tmpC9C)),((_tmpC9C[0]=((_tmpC9D.tag=
16,((_tmpC9D.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp551),_tmpC9D)))),_tmpC9C))));}
goto _LL230;_LL259: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp552=(
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp552->tag != 18)
goto _LL25B;else{_tmp553=(void*)_tmp552->f1;_tmp554=(void*)_tmp552->f2;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*
_tmp555=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp554;if(
_tmp555->tag != 0)goto _LL25B;else{_tmp556=_tmp555->f1;}};}}_LL25A:{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct
_tmpCA7;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCA6;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*
_tmpCA5;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCA4;e->r=(void*)((
_tmpCA4=_cycalloc(sizeof(*_tmpCA4)),((_tmpCA4[0]=((_tmpCA7.tag=18,((_tmpCA7.f1=(
void*)Cyc_Toc_typ_to_c_array(_tmp553),((_tmpCA7.f2=(void*)((void*)((_tmpCA5=
_cycalloc(sizeof(*_tmpCA5)),((_tmpCA5[0]=((_tmpCA6.tag=0,((_tmpCA6.f1=_tmp556,
_tmpCA6)))),_tmpCA5))))),_tmpCA7)))))),_tmpCA4))));}goto _LL230;_LL25B: {struct
Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp557=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)
_tmp511;if(_tmp557->tag != 18)goto _LL25D;else{_tmp558=(void*)_tmp557->f1;_tmp559=(
void*)_tmp557->f2;{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp55A=(
struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp559;if(_tmp55A->tag != 
1)goto _LL25D;else{_tmp55B=_tmp55A->f1;}};}}_LL25C:{void*_tmp6CA=Cyc_Tcutil_compress(
_tmp558);struct Cyc_Absyn_AggrInfo _tmp6CC;union Cyc_Absyn_AggrInfoU _tmp6CD;struct
Cyc_List_List*_tmp6CF;_LL30A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp6CB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6CA;if(_tmp6CB->tag != 
11)goto _LL30C;else{_tmp6CC=_tmp6CB->f1;_tmp6CD=_tmp6CC.aggr_info;}}_LL30B: {
struct Cyc_Absyn_Aggrdecl*_tmp6D2=Cyc_Absyn_get_known_aggrdecl(_tmp6CD);if(
_tmp6D2->impl == 0){const char*_tmpCAA;void*_tmpCA9;(_tmpCA9=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCAA="struct fields must be known",
_tag_dyneither(_tmpCAA,sizeof(char),28))),_tag_dyneither(_tmpCA9,sizeof(void*),0)));}
_tmp6CF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6D2->impl))->fields;goto
_LL30D;}_LL30C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6CE=(struct
Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6CA;if(_tmp6CE->tag != 12)goto
_LL30E;else{_tmp6CF=_tmp6CE->f2;}}_LL30D: {struct Cyc_Absyn_Aggrfield*_tmp6D5=((
struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp6CF,(
int)_tmp55B);{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpCB4;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct
_tmpCB3;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCB2;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*
_tmpCB1;e->r=(void*)((_tmpCB1=_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1[0]=((_tmpCB4.tag=
18,((_tmpCB4.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp558),((_tmpCB4.f2=(void*)((
void*)((_tmpCB2=_cycalloc(sizeof(*_tmpCB2)),((_tmpCB2[0]=((_tmpCB3.tag=0,((
_tmpCB3.f1=_tmp6D5->name,_tmpCB3)))),_tmpCB2))))),_tmpCB4)))))),_tmpCB1))));}
goto _LL309;}_LL30E: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6D0=(struct
Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6CA;if(_tmp6D0->tag != 10)goto _LL310;}
_LL30F:{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpCBE;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct
_tmpCBD;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCBC;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*
_tmpCBB;e->r=(void*)((_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB[0]=((_tmpCBE.tag=
18,((_tmpCBE.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp558),((_tmpCBE.f2=(void*)((
void*)((_tmpCBC=_cycalloc(sizeof(*_tmpCBC)),((_tmpCBC[0]=((_tmpCBD.tag=0,((
_tmpCBD.f1=Cyc_Absyn_fieldname((int)(_tmp55B + 1)),_tmpCBD)))),_tmpCBC))))),
_tmpCBE)))))),_tmpCBB))));}goto _LL309;_LL310: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmp6D1=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6CA;if(_tmp6D1->tag
!= 4)goto _LL312;}_LL311: if(_tmp55B == 0){struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct
_tmpCC8;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCC7;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*
_tmpCC6;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCC5;e->r=(void*)((
_tmpCC5=_cycalloc(sizeof(*_tmpCC5)),((_tmpCC5[0]=((_tmpCC8.tag=18,((_tmpCC8.f1=(
void*)Cyc_Toc_typ_to_c_array(_tmp558),((_tmpCC8.f2=(void*)((void*)((_tmpCC6=
_cycalloc(sizeof(*_tmpCC6)),((_tmpCC6[0]=((_tmpCC7.tag=0,((_tmpCC7.f1=Cyc_Toc_tag_sp,
_tmpCC7)))),_tmpCC6))))),_tmpCC8)))))),_tmpCC5))));}else{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct
_tmpCD2;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCD1;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*
_tmpCD0;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCCF;e->r=(void*)((
_tmpCCF=_cycalloc(sizeof(*_tmpCCF)),((_tmpCCF[0]=((_tmpCD2.tag=18,((_tmpCD2.f1=(
void*)Cyc_Toc_typ_to_c_array(_tmp558),((_tmpCD2.f2=(void*)((void*)((_tmpCD0=
_cycalloc(sizeof(*_tmpCD0)),((_tmpCD0[0]=((_tmpCD1.tag=0,((_tmpCD1.f1=Cyc_Absyn_fieldname((
int)_tmp55B),_tmpCD1)))),_tmpCD0))))),_tmpCD2)))))),_tmpCCF))));}goto _LL309;
_LL312:;_LL313: {const char*_tmpCD5;void*_tmpCD4;(_tmpCD4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD5="impossible type for offsetof tuple index",
_tag_dyneither(_tmpCD5,sizeof(char),41))),_tag_dyneither(_tmpCD4,sizeof(void*),0)));}
_LL309:;}goto _LL230;_LL25D: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*
_tmp55C=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp55C->tag != 
19)goto _LL25F;else{_tmp55D=_tmp55C->f1;}}_LL25E: {int _tmp6E8=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{void*_tmp6E9=Cyc_Tcutil_compress((void*)_check_null(
_tmp55D->topt));{void*_tmp6EA=_tmp6E9;struct Cyc_Absyn_PtrInfo _tmp6EC;void*
_tmp6ED;struct Cyc_Absyn_Tqual _tmp6EE;struct Cyc_Absyn_PtrAtts _tmp6EF;void*_tmp6F0;
union Cyc_Absyn_Constraint*_tmp6F1;union Cyc_Absyn_Constraint*_tmp6F2;union Cyc_Absyn_Constraint*
_tmp6F3;_LL315: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6EB=(struct
Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6EA;if(_tmp6EB->tag != 5)goto _LL317;
else{_tmp6EC=_tmp6EB->f1;_tmp6ED=_tmp6EC.elt_typ;_tmp6EE=_tmp6EC.elt_tq;_tmp6EF=
_tmp6EC.ptr_atts;_tmp6F0=_tmp6EF.rgn;_tmp6F1=_tmp6EF.nullable;_tmp6F2=_tmp6EF.bounds;
_tmp6F3=_tmp6EF.zero_term;}}_LL316:{void*_tmp6F4=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6F2);struct Cyc_Absyn_Exp*
_tmp6F6;_LL31A: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6F5=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)
_tmp6F4;if(_tmp6F5->tag != 1)goto _LL31C;else{_tmp6F6=_tmp6F5->f1;}}_LL31B: {int
do_null_check=Cyc_Toc_need_null_check(_tmp55D);Cyc_Toc_exp_to_c(nv,_tmp55D);if(
do_null_check){if(Cyc_Toc_warn_all_null_deref){const char*_tmpCD8;void*_tmpCD7;(
_tmpCD7=0,Cyc_Tcutil_warn(e->loc,((_tmpCD8="inserted null check due to dereference",
_tag_dyneither(_tmpCD8,sizeof(char),39))),_tag_dyneither(_tmpCD7,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpCD9;_tmp55D->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp6E9),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpCD9=_cycalloc(sizeof(*
_tmpCD9)),((_tmpCD9->hd=Cyc_Absyn_copy_exp(_tmp55D),((_tmpCD9->tl=0,_tmpCD9)))))),
0));};}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6F3)){unsigned int _tmp6FC;int _tmp6FD;struct _tuple11 _tmp6FB=Cyc_Evexp_eval_const_uint_exp(
_tmp6F6);_tmp6FC=_tmp6FB.f1;_tmp6FD=_tmp6FB.f2;if(!_tmp6FD  || _tmp6FC <= 0){const
char*_tmpCDC;void*_tmpCDB;(_tmpCDB=0,Cyc_Tcutil_terr(e->loc,((_tmpCDC="cannot determine dereference is in bounds",
_tag_dyneither(_tmpCDC,sizeof(char),42))),_tag_dyneither(_tmpCDB,sizeof(void*),0)));}}
goto _LL319;}_LL31C: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6F7=(
struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6F4;if(_tmp6F7->tag != 0)
goto _LL319;}_LL31D: {struct Cyc_Absyn_Exp*_tmp700=Cyc_Absyn_uint_exp(0,0);_tmp700->topt=(
void*)Cyc_Absyn_uint_typ;e->r=Cyc_Toc_subscript_exp_r(_tmp55D,_tmp700);Cyc_Toc_exp_to_c(
nv,e);goto _LL319;}_LL319:;}goto _LL314;_LL317:;_LL318: {const char*_tmpCDF;void*
_tmpCDE;(_tmpCDE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCDF="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpCDF,sizeof(char),29))),
_tag_dyneither(_tmpCDE,sizeof(void*),0)));}_LL314:;}Cyc_Toc_set_lhs(nv,_tmp6E8);
goto _LL230;};}_LL25F: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp55E=(
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp55E->tag != 20)
goto _LL261;else{_tmp55F=_tmp55E->f1;_tmp560=_tmp55E->f2;_tmp561=_tmp55E->f3;
_tmp562=_tmp55E->f4;}}_LL260: {int is_poly=Cyc_Toc_is_poly_project(e);void*
e1_cyc_type=(void*)_check_null(_tmp55F->topt);Cyc_Toc_exp_to_c(nv,_tmp55F);if(
_tmp561  && _tmp562)e->r=Cyc_Toc_check_tagged_union(_tmp55F,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp560,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrmember_exp);
if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),
Cyc_Absyn_new_exp(e->r,0),0))->r;goto _LL230;}_LL261: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*
_tmp563=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp563->tag
!= 21)goto _LL263;else{_tmp564=_tmp563->f1;_tmp565=_tmp563->f2;_tmp566=_tmp563->f3;
_tmp567=_tmp563->f4;}}_LL262: {int _tmp703=Cyc_Toc_in_lhs(nv);Cyc_Toc_set_lhs(nv,
0);{void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp564->topt));int
do_null_check=Cyc_Toc_need_null_check(_tmp564);Cyc_Toc_exp_to_c(nv,_tmp564);{int
is_poly=Cyc_Toc_is_poly_project(e);void*_tmp705;struct Cyc_Absyn_Tqual _tmp706;
struct Cyc_Absyn_PtrAtts _tmp707;void*_tmp708;union Cyc_Absyn_Constraint*_tmp709;
union Cyc_Absyn_Constraint*_tmp70A;union Cyc_Absyn_Constraint*_tmp70B;struct Cyc_Absyn_PtrInfo
_tmp704=Cyc_Toc_get_ptr_type(e1typ);_tmp705=_tmp704.elt_typ;_tmp706=_tmp704.elt_tq;
_tmp707=_tmp704.ptr_atts;_tmp708=_tmp707.rgn;_tmp709=_tmp707.nullable;_tmp70A=
_tmp707.bounds;_tmp70B=_tmp707.zero_term;{void*_tmp70C=((void*(*)(void*y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp70A);
struct Cyc_Absyn_Exp*_tmp70E;_LL31F: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp70D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp70C;if(_tmp70D->tag != 
1)goto _LL321;else{_tmp70E=_tmp70D->f1;}}_LL320: {unsigned int _tmp711;int _tmp712;
struct _tuple11 _tmp710=Cyc_Evexp_eval_const_uint_exp(_tmp70E);_tmp711=_tmp710.f1;
_tmp712=_tmp710.f2;if(_tmp712){if(_tmp711 < 1){const char*_tmpCE2;void*_tmpCE1;(
_tmpCE1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpCE2="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpCE2,
sizeof(char),44))),_tag_dyneither(_tmpCE1,sizeof(void*),0)));}if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCE5;void*_tmpCE4;(_tmpCE4=0,Cyc_Tcutil_warn(
e->loc,((_tmpCE5="inserted null check due to dereference",_tag_dyneither(_tmpCE5,
sizeof(char),39))),_tag_dyneither(_tmpCE4,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCE6[1];_tmp564->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpCE6[0]=Cyc_Absyn_new_exp(_tmp564->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE6,sizeof(struct Cyc_Absyn_Exp*),
1)))),0));};}}else{if(!Cyc_Evexp_c_can_eval(_tmp70E)){const char*_tmpCE9;void*
_tmpCE8;(_tmpCE8=0,Cyc_Tcutil_terr(e->loc,((_tmpCE9="cannot determine pointer dereference in bounds",
_tag_dyneither(_tmpCE9,sizeof(char),47))),_tag_dyneither(_tmpCE8,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpCEA[4];_tmp564->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpCEA[3]=
Cyc_Absyn_uint_exp(0,0),((_tmpCEA[2]=Cyc_Absyn_sizeoftyp_exp(_tmp705,0),((
_tmpCEA[1]=_tmp70E,((_tmpCEA[0]=Cyc_Absyn_new_exp(_tmp564->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCEA,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0));};}goto _LL31E;}_LL321: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp70F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp70C;if(_tmp70F->tag
!= 0)goto _LL31E;}_LL322: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp705);{struct Cyc_Absyn_Exp*
_tmpCEB[3];_tmp564->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp706),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpCEB[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpCEB[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCEB[0]=Cyc_Absyn_new_exp(_tmp564->r,
0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCEB,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}goto _LL31E;}_LL31E:;}if(
_tmp566  && _tmp567)e->r=Cyc_Toc_check_tagged_union(_tmp564,Cyc_Toc_typ_to_c(
e1typ),_tmp705,_tmp565,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);if(is_poly
 && _tmp567)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(
e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;Cyc_Toc_set_lhs(nv,_tmp703);goto
_LL230;};};}_LL263: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp568=(
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp568->tag != 22)
goto _LL265;else{_tmp569=_tmp568->f1;_tmp56A=_tmp568->f2;}}_LL264: {int _tmp71C=
Cyc_Toc_in_lhs(nv);Cyc_Toc_set_lhs(nv,0);{void*_tmp71D=Cyc_Tcutil_compress((void*)
_check_null(_tmp569->topt));{void*_tmp71E=_tmp71D;struct Cyc_List_List*_tmp720;
struct Cyc_Absyn_PtrInfo _tmp722;void*_tmp723;struct Cyc_Absyn_Tqual _tmp724;struct
Cyc_Absyn_PtrAtts _tmp725;void*_tmp726;union Cyc_Absyn_Constraint*_tmp727;union Cyc_Absyn_Constraint*
_tmp728;union Cyc_Absyn_Constraint*_tmp729;_LL324: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmp71F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp71E;if(_tmp71F->tag != 
10)goto _LL326;else{_tmp720=_tmp71F->f1;}}_LL325: Cyc_Toc_exp_to_c(nv,_tmp569);Cyc_Toc_exp_to_c(
nv,_tmp56A);{unsigned int _tmp72B;int _tmp72C;struct _tuple11 _tmp72A=Cyc_Evexp_eval_const_uint_exp(
_tmp56A);_tmp72B=_tmp72A.f1;_tmp72C=_tmp72A.f2;if(!_tmp72C){const char*_tmpCEE;
void*_tmpCED;(_tmpCED=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpCEE="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpCEE,sizeof(char),44))),_tag_dyneither(_tmpCED,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp569,Cyc_Absyn_fieldname((int)(_tmp72B + 1)));
goto _LL323;};_LL326: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp721=(
struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp71E;if(_tmp721->tag != 5)goto
_LL328;else{_tmp722=_tmp721->f1;_tmp723=_tmp722.elt_typ;_tmp724=_tmp722.elt_tq;
_tmp725=_tmp722.ptr_atts;_tmp726=_tmp725.rgn;_tmp727=_tmp725.nullable;_tmp728=
_tmp725.bounds;_tmp729=_tmp725.zero_term;}}_LL327: {struct Cyc_List_List*_tmp72F=
Cyc_Toc_get_relns(_tmp569);int in_bnds=0;{void*_tmp730=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp728);_LL32B:;_LL32C: in_bnds=
Cyc_Toc_check_bounds(_tmp71D,_tmp72F,_tmp569,_tmp56A);if(Cyc_Toc_warn_bounds_checks
 && !in_bnds){const char*_tmpCF2;void*_tmpCF1[1];struct Cyc_String_pa_PrintArg_struct
_tmpCF0;(_tmpCF0.tag=0,((_tmpCF0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpCF1[0]=& _tmpCF0,Cyc_Tcutil_warn(e->loc,((
_tmpCF2="bounds check necessary for %s",_tag_dyneither(_tmpCF2,sizeof(char),30))),
_tag_dyneither(_tmpCF1,sizeof(void*),1)))))));}_LL32A:;}Cyc_Toc_exp_to_c(nv,
_tmp569);Cyc_Toc_exp_to_c(nv,_tmp56A);++ Cyc_Toc_total_bounds_checks;{void*
_tmp734=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp728);struct Cyc_Absyn_Exp*_tmp736;_LL32E: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp735=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp734;if(_tmp735->tag != 
1)goto _LL330;else{_tmp736=_tmp735->f1;}}_LL32F: {int possibly_null=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp727);void*ta1=Cyc_Toc_typ_to_c(
_tmp723);void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp724);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp729)){if(!Cyc_Evexp_c_can_eval(_tmp736)){const char*_tmpCF5;void*_tmpCF4;(
_tmpCF4=0,Cyc_Tcutil_terr(e->loc,((_tmpCF5="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCF5,sizeof(char),40))),_tag_dyneither(_tmpCF4,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp569);struct Cyc_Absyn_Exp*_tmpCF6[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpCF6[2]=_tmp56A,((_tmpCF6[1]=_tmp736,((
_tmpCF6[0]=_tmp569,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCF6,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp736)){const char*_tmpCF9;void*_tmpCF8;(
_tmpCF8=0,Cyc_Tcutil_terr(e->loc,((_tmpCF9="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCF9,sizeof(char),40))),_tag_dyneither(_tmpCF8,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCFC;void*_tmpCFB;(_tmpCFB=0,Cyc_Tcutil_warn(
e->loc,((_tmpCFC="inserted null check due to dereference",_tag_dyneither(_tmpCFC,
sizeof(char),39))),_tag_dyneither(_tmpCFB,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCFD[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpCFD[3]=_tmp56A,((_tmpCFD[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCFD[1]=
_tmp736,((_tmpCFD[0]=_tmp569,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCFD,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{if(
!Cyc_Evexp_c_can_eval(_tmp736)){const char*_tmpD00;void*_tmpCFF;(_tmpCFF=0,Cyc_Tcutil_terr(
e->loc,((_tmpD00="cannot determine subscript is in bounds",_tag_dyneither(
_tmpD00,sizeof(char),40))),_tag_dyneither(_tmpCFF,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpD01[2];_tmp56A->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpD01[1]=Cyc_Absyn_copy_exp(_tmp56A),((_tmpD01[0]=_tmp736,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD01,sizeof(struct Cyc_Absyn_Exp*),
2)))))));};}}}goto _LL32D;}_LL330: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp737=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp734;if(_tmp737->tag
!= 0)goto _LL32D;}_LL331: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp723);if(in_bnds){
++ Cyc_Toc_bounds_checks_eliminated;e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(
Cyc_Absyn_cstar_typ(ta1,_tmp724),Cyc_Absyn_aggrmember_exp(_tmp569,Cyc_Toc_curr_sp,
0)),_tmp56A);}else{struct Cyc_Absyn_Exp*_tmp743=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpD02[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
ta1,_tmp724),Cyc_Absyn_fncall_exp(_tmp743,((_tmpD02[2]=_tmp56A,((_tmpD02[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0),((_tmpD02[0]=_tmp569,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD02,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL32D;}
_LL32D:;}goto _LL323;}_LL328:;_LL329: {const char*_tmpD05;void*_tmpD04;(_tmpD04=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD05="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpD05,sizeof(char),49))),_tag_dyneither(_tmpD04,sizeof(void*),0)));}_LL323:;}
Cyc_Toc_set_lhs(nv,_tmp71C);goto _LL230;};}_LL265: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*
_tmp56B=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp56B->tag != 
23)goto _LL267;else{_tmp56C=_tmp56B->f1;}}_LL266: if(!Cyc_Toc_is_toplevel(nv))e->r=(
Cyc_Toc_init_tuple(nv,0,0,_tmp56C))->r;else{struct Cyc_List_List*_tmp747=((struct
Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp56C);void*_tmp748=Cyc_Toc_add_tuple_type(
_tmp747);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp56C != 0;(_tmp56C=_tmp56C->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp56C->hd);{struct _tuple16*
_tmpD08;struct Cyc_List_List*_tmpD07;dles=((_tmpD07=_cycalloc(sizeof(*_tmpD07)),((
_tmpD07->hd=((_tmpD08=_cycalloc(sizeof(*_tmpD08)),((_tmpD08->f1=0,((_tmpD08->f2=(
struct Cyc_Absyn_Exp*)_tmp56C->hd,_tmpD08)))))),((_tmpD07->tl=dles,_tmpD07))))));};}}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=
Cyc_Toc_unresolvedmem_exp_r(0,dles);}goto _LL230;_LL267: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*
_tmp56D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp56D->tag != 
25)goto _LL269;else{_tmp56E=_tmp56D->f1;}}_LL268: e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp56E);{struct Cyc_List_List*_tmp74B=_tmp56E;for(0;_tmp74B != 0;_tmp74B=
_tmp74B->tl){struct _tuple16 _tmp74D;struct Cyc_Absyn_Exp*_tmp74E;struct _tuple16*
_tmp74C=(struct _tuple16*)_tmp74B->hd;_tmp74D=*_tmp74C;_tmp74E=_tmp74D.f2;Cyc_Toc_exp_to_c(
nv,_tmp74E);}}goto _LL230;_LL269: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*
_tmp56F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp511;if(
_tmp56F->tag != 26)goto _LL26B;else{_tmp570=_tmp56F->f1;_tmp571=_tmp56F->f2;
_tmp572=_tmp56F->f3;_tmp573=_tmp56F->f4;}}_LL26A: {unsigned int _tmp750;int
_tmp751;struct _tuple11 _tmp74F=Cyc_Evexp_eval_const_uint_exp(_tmp571);_tmp750=
_tmp74F.f1;_tmp751=_tmp74F.f2;{void*_tmp752=Cyc_Toc_typ_to_c((void*)_check_null(
_tmp572->topt));Cyc_Toc_exp_to_c(nv,_tmp572);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(
_tmp572)){if(!_tmp751){const char*_tmpD0B;void*_tmpD0A;(_tmpD0A=0,Cyc_Tcutil_terr(
_tmp571->loc,((_tmpD0B="cannot determine value of constant",_tag_dyneither(
_tmpD0B,sizeof(char),35))),_tag_dyneither(_tmpD0A,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp750;++ i){struct _tuple16*_tmpD0E;struct Cyc_List_List*
_tmpD0D;es=((_tmpD0D=_cycalloc(sizeof(*_tmpD0D)),((_tmpD0D->hd=((_tmpD0E=
_cycalloc(sizeof(*_tmpD0E)),((_tmpD0E->f1=0,((_tmpD0E->f2=_tmp572,_tmpD0E)))))),((
_tmpD0D->tl=es,_tmpD0D))))));}}if(_tmp573){struct Cyc_Absyn_Exp*_tmp757=Cyc_Toc_cast_it(
_tmp752,Cyc_Absyn_uint_exp(0,0));struct _tuple16*_tmpD11;struct Cyc_List_List*
_tmpD10;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpD10=_cycalloc(sizeof(*_tmpD10)),((_tmpD10->hd=((
_tmpD11=_cycalloc(sizeof(*_tmpD11)),((_tmpD11->f1=0,((_tmpD11->f2=_tmp757,
_tmpD11)))))),((_tmpD10->tl=0,_tmpD10)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL230;};};}_LL26B: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*
_tmp574=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp574->tag
!= 27)goto _LL26D;else{_tmp575=_tmp574->f1;_tmp576=_tmp574->f2;_tmp577=_tmp574->f3;
_tmp578=_tmp574->f4;}}_LL26C: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(
nv,old_typ,_tmp576,0,0,_tmp577,_tmp575))->r;else{if(_tmp578 == 0){const char*
_tmpD14;void*_tmpD13;(_tmpD13=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD14="Aggregate_e: missing aggrdecl pointer",
_tag_dyneither(_tmpD14,sizeof(char),38))),_tag_dyneither(_tmpD13,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp578;struct
_RegionHandle _tmp75C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp75C;
_push_region(rgn);{struct Cyc_List_List*_tmp75D=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,
_tmp577,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp578->impl))->tagged){struct
_tuple22 _tmp75F;struct Cyc_Absyn_Aggrfield*_tmp760;struct Cyc_Absyn_Exp*_tmp761;
struct _tuple22*_tmp75E=(struct _tuple22*)((struct Cyc_List_List*)_check_null(
_tmp75D))->hd;_tmp75F=*_tmp75E;_tmp760=_tmp75F.f1;_tmp761=_tmp75F.f2;{void*
_tmp762=_tmp760->type;Cyc_Toc_exp_to_c(nv,_tmp761);if(Cyc_Toc_is_void_star_or_boxed_tvar(
_tmp762))_tmp761->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp761->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp578,_tmp760->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple16*_tmpD19;struct _tuple16*_tmpD18;struct _tuple16*_tmpD17[2];
struct Cyc_List_List*_tmp763=(_tmpD17[1]=((_tmpD18=_cycalloc(sizeof(*_tmpD18)),((
_tmpD18->f1=0,((_tmpD18->f2=_tmp761,_tmpD18)))))),((_tmpD17[0]=((_tmpD19=
_cycalloc(sizeof(*_tmpD19)),((_tmpD19->f1=0,((_tmpD19->f2=field_tag_exp,_tmpD19)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD17,sizeof(struct _tuple16*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp763,0);struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD1F;struct Cyc_Absyn_FieldName_Absyn_Designator_struct
_tmpD1E;void*_tmpD1D[1];struct Cyc_List_List*ds=(_tmpD1D[0]=(void*)((_tmpD1F=
_cycalloc(sizeof(*_tmpD1F)),((_tmpD1F[0]=((_tmpD1E.tag=1,((_tmpD1E.f1=_tmp760->name,
_tmpD1E)))),_tmpD1F)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD1D,sizeof(void*),1)));struct _tuple16*_tmpD22;struct _tuple16*
_tmpD21[1];struct Cyc_List_List*dles=(_tmpD21[0]=((_tmpD22=_cycalloc(sizeof(*
_tmpD22)),((_tmpD22->f1=ds,((_tmpD22->f2=umem,_tmpD22)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD21,sizeof(struct
_tuple16*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{struct Cyc_List_List*
_tmp76C=0;struct Cyc_List_List*_tmp76D=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp76D != 0;_tmp76D=_tmp76D->tl){struct Cyc_List_List*
_tmp76E=_tmp75D;for(0;_tmp76E != 0;_tmp76E=_tmp76E->tl){if((*((struct _tuple22*)
_tmp76E->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp76D->hd){struct _tuple22
_tmp770;struct Cyc_Absyn_Aggrfield*_tmp771;struct Cyc_Absyn_Exp*_tmp772;struct
_tuple22*_tmp76F=(struct _tuple22*)_tmp76E->hd;_tmp770=*_tmp76F;_tmp771=_tmp770.f1;
_tmp772=_tmp770.f2;{void*_tmp773=_tmp771->type;Cyc_Toc_exp_to_c(nv,_tmp772);if(
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp773))_tmp772->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp772->r,0));{struct _tuple16*_tmpD25;struct Cyc_List_List*
_tmpD24;_tmp76C=((_tmpD24=_cycalloc(sizeof(*_tmpD24)),((_tmpD24->hd=((_tmpD25=
_cycalloc(sizeof(*_tmpD25)),((_tmpD25->f1=0,((_tmpD25->f2=_tmp772,_tmpD25)))))),((
_tmpD24->tl=_tmp76C,_tmpD24))))));}break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp76C));}};
_pop_region(rgn);};}goto _LL230;_LL26D: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*
_tmp579=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp579->tag
!= 28)goto _LL26F;else{_tmp57A=(void*)_tmp579->f1;_tmp57B=_tmp579->f2;}}_LL26E: {
struct Cyc_List_List*fs;{void*_tmp776=Cyc_Tcutil_compress(_tmp57A);struct Cyc_List_List*
_tmp778;_LL333: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp777=(struct
Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp776;if(_tmp777->tag != 12)goto
_LL335;else{_tmp778=_tmp777->f2;}}_LL334: fs=_tmp778;goto _LL332;_LL335:;_LL336: {
const char*_tmpD29;void*_tmpD28[1];struct Cyc_String_pa_PrintArg_struct _tmpD27;(
_tmpD27.tag=0,((_tmpD27.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp57A)),((_tmpD28[0]=& _tmpD27,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD29="anon struct has type %s",
_tag_dyneither(_tmpD29,sizeof(char),24))),_tag_dyneither(_tmpD28,sizeof(void*),1)))))));}
_LL332:;}{struct _RegionHandle _tmp77C=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp77C;_push_region(rgn);{struct Cyc_List_List*_tmp77D=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,
_tmp57B,Cyc_Absyn_StructA,fs);for(0;_tmp77D != 0;_tmp77D=_tmp77D->tl){struct
_tuple22 _tmp77F;struct Cyc_Absyn_Aggrfield*_tmp780;struct Cyc_Absyn_Exp*_tmp781;
struct _tuple22*_tmp77E=(struct _tuple22*)_tmp77D->hd;_tmp77F=*_tmp77E;_tmp780=
_tmp77F.f1;_tmp781=_tmp77F.f2;{void*_tmp782=_tmp780->type;Cyc_Toc_exp_to_c(nv,
_tmp781);if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp782))_tmp781->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp781->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp57B);}_npop_handler(0);goto _LL230;;_pop_region(rgn);};}_LL26F: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*
_tmp57C=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp57C->tag
!= 29)goto _LL271;else{_tmp57D=_tmp57C->f1;_tmp57E=_tmp57C->f2;_tmp57F=_tmp57C->f3;}}
_LL270: {void*datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple0*_tmp783=
Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp784=Cyc_Absyn_var_exp(_tmp783,0);
struct Cyc_Absyn_Exp*member_exp;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(
_tmp57F->name,_tmp57E->name));tag_exp=_tmp57E->is_extensible?Cyc_Absyn_var_exp(
_tmp57F->name,0): Cyc_Toc_datatype_tag(_tmp57E,_tmp57F->name);member_exp=_tmp784;{
struct Cyc_List_List*_tmp785=_tmp57F->typs;if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*
dles=0;for(0;_tmp57D != 0;(_tmp57D=_tmp57D->tl,_tmp785=_tmp785->tl)){struct Cyc_Absyn_Exp*
cur_e=(struct Cyc_Absyn_Exp*)_tmp57D->hd;void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp785))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))cur_e=Cyc_Toc_cast_it(
field_typ,cur_e);{struct _tuple16*_tmpD2C;struct Cyc_List_List*_tmpD2B;dles=((
_tmpD2B=_cycalloc(sizeof(*_tmpD2B)),((_tmpD2B->hd=((_tmpD2C=_cycalloc(sizeof(*
_tmpD2C)),((_tmpD2C->f1=0,((_tmpD2C->f2=cur_e,_tmpD2C)))))),((_tmpD2B->tl=dles,
_tmpD2B))))));};}{struct _tuple16*_tmpD2F;struct Cyc_List_List*_tmpD2E;dles=((
_tmpD2E=_cycalloc(sizeof(*_tmpD2E)),((_tmpD2E->hd=((_tmpD2F=_cycalloc(sizeof(*
_tmpD2F)),((_tmpD2F->f1=0,((_tmpD2F->f2=tag_exp,_tmpD2F)))))),((_tmpD2E->tl=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpD2E))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*_tmpD30;
struct Cyc_List_List*_tmp78A=(_tmpD30=_cycalloc(sizeof(*_tmpD30)),((_tmpD30->hd=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,0),
tag_exp,0),((_tmpD30->tl=0,_tmpD30)))));{int i=1;for(0;_tmp57D != 0;(((_tmp57D=
_tmp57D->tl,i ++)),_tmp785=_tmp785->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp57D->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp785))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_boxed_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp78B=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpD31;_tmp78A=((_tmpD31=_cycalloc(sizeof(*
_tmpD31)),((_tmpD31->hd=_tmp78B,((_tmpD31->tl=_tmp78A,_tmpD31))))));};}}{struct
Cyc_Absyn_Stmt*_tmp78D=Cyc_Absyn_exp_stmt(_tmp784,0);struct Cyc_List_List*_tmpD32;
struct Cyc_Absyn_Stmt*_tmp78E=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpD32=_cycalloc(sizeof(*_tmpD32)),((
_tmpD32->hd=_tmp78D,((_tmpD32->tl=_tmp78A,_tmpD32))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp783,datatype_ctype,0,_tmp78E,0));};}goto _LL230;};}
_LL271: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp580=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)
_tmp511;if(_tmp580->tag != 30)goto _LL273;}_LL272: goto _LL274;_LL273: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*
_tmp581=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp581->tag
!= 31)goto _LL275;}_LL274: goto _LL230;_LL275: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*
_tmp582=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp582->tag
!= 32)goto _LL277;else{_tmp583=_tmp582->f1;_tmp584=_tmp583.is_calloc;_tmp585=
_tmp583.rgn;_tmp586=_tmp583.elt_type;_tmp587=_tmp583.num_elts;_tmp588=_tmp583.fat_result;}}
_LL276: {void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp586)));Cyc_Toc_exp_to_c(
nv,_tmp587);if(_tmp588){struct _tuple0*_tmp791=Cyc_Toc_temp_var();struct _tuple0*
_tmp792=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp584){xexp=_tmp587;if(_tmp585 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp585;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp791,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp586,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp791,0));}{struct Cyc_Absyn_Exp*_tmpD33[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpD33[2]=Cyc_Absyn_var_exp(_tmp791,0),((_tmpD33[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpD33[0]=Cyc_Absyn_var_exp(_tmp792,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD33,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{if(_tmp585 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp585;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp791,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp586,Cyc_Absyn_var_exp(_tmp791,0));}{struct Cyc_Absyn_Exp*_tmpD34[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpD34[2]=Cyc_Absyn_var_exp(_tmp791,0),((_tmpD34[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpD34[0]=Cyc_Absyn_var_exp(_tmp792,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD34,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);};}{struct Cyc_Absyn_Stmt*_tmp795=Cyc_Absyn_declare_stmt(_tmp791,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)_tmp587,Cyc_Absyn_declare_stmt(_tmp792,Cyc_Absyn_cstar_typ(
t_c,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp795);};}else{if(_tmp584){if(_tmp585 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp585;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp587))->r;}else{e->r=(
Cyc_Toc_calloc_exp(*_tmp586,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp587))->r;}}else{
if(_tmp585 != 0  && !Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp585;Cyc_Toc_exp_to_c(nv,rgn);e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp587))->r;}
else{e->r=(Cyc_Toc_malloc_exp(*_tmp586,_tmp587))->r;}}}goto _LL230;}_LL277: {
struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp589=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)
_tmp511;if(_tmp589->tag != 33)goto _LL279;else{_tmp58A=_tmp589->f1;_tmp58B=_tmp589->f2;}}
_LL278: {int is_dyneither_ptr=0;void*e1_old_typ=(void*)_check_null(_tmp58A->topt);
void*e2_old_typ=(void*)_check_null(_tmp58B->topt);if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dyneither_ptr)){const char*_tmpD37;void*_tmpD36;(_tmpD36=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD37="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpD37,sizeof(char),57))),_tag_dyneither(_tmpD36,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}{int f1_tag=0;void*tagged_mem_type1=(void*)&
Cyc_Absyn_VoidType_val;int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp58A,&
f1_tag,& tagged_mem_type1,1);int f2_tag=0;void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp58B,& f2_tag,& tagged_mem_type2,1);
Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp58A);Cyc_Toc_exp_to_c(nv,_tmp58B);
Cyc_Toc_set_lhs(nv,0);if(e1_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp58A,
f1_tag,tagged_mem_type1);else{_tmp58A=Cyc_Toc_push_address_exp(_tmp58A);}if(
e2_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp58B,f2_tag,tagged_mem_type2);
else{_tmp58B=Cyc_Toc_push_address_exp(_tmp58B);}{struct Cyc_Absyn_Exp*_tmpD38[2];
e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpD38[1]=_tmp58B,((_tmpD38[0]=_tmp58A,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD38,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL230;};};}_LL279: {struct
Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp58C=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)
_tmp511;if(_tmp58C->tag != 36)goto _LL27B;else{_tmp58D=_tmp58C->f1;_tmp58E=_tmp58C->f2;}}
_LL27A: {void*_tmp799=Cyc_Tcutil_compress((void*)_check_null(_tmp58D->topt));Cyc_Toc_exp_to_c(
nv,_tmp58D);{void*_tmp79A=_tmp799;struct Cyc_Absyn_AggrInfo _tmp79C;union Cyc_Absyn_AggrInfoU
_tmp79D;struct Cyc_Absyn_Aggrdecl**_tmp79E;struct Cyc_Absyn_Aggrdecl*_tmp79F;
_LL338: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp79B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp79A;if(_tmp79B->tag != 11)goto _LL33A;else{_tmp79C=_tmp79B->f1;_tmp79D=_tmp79C.aggr_info;
if((_tmp79D.KnownAggr).tag != 2)goto _LL33A;_tmp79E=(struct Cyc_Absyn_Aggrdecl**)(
_tmp79D.KnownAggr).val;_tmp79F=*_tmp79E;}}_LL339: {struct Cyc_Absyn_Exp*_tmp7A0=
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp79F,_tmp58E),0);struct Cyc_Absyn_Exp*
_tmp7A1=Cyc_Absyn_aggrmember_exp(_tmp58D,_tmp58E,0);struct Cyc_Absyn_Exp*_tmp7A2=
Cyc_Absyn_aggrmember_exp(_tmp7A1,Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(
_tmp7A2,_tmp7A0,0))->r;goto _LL337;}_LL33A:;_LL33B: {const char*_tmpD3B;void*
_tmpD3A;(_tmpD3A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD3B="non-aggregate type in tagcheck",_tag_dyneither(_tmpD3B,sizeof(char),31))),
_tag_dyneither(_tmpD3A,sizeof(void*),0)));}_LL337:;}goto _LL230;}_LL27B: {struct
Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp58F=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)
_tmp511;if(_tmp58F->tag != 35)goto _LL27D;else{_tmp590=_tmp58F->f1;}}_LL27C: Cyc_Toc_stmt_to_c(
nv,_tmp590);goto _LL230;_LL27D: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*
_tmp591=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp511;if(
_tmp591->tag != 34)goto _LL27F;}_LL27E: {const char*_tmpD3E;void*_tmpD3D;(_tmpD3D=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD3E="UnresolvedMem",_tag_dyneither(_tmpD3E,sizeof(char),14))),_tag_dyneither(
_tmpD3D,sizeof(void*),0)));}_LL27F: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*
_tmp592=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp592->tag
!= 24)goto _LL281;}_LL280: {const char*_tmpD41;void*_tmpD40;(_tmpD40=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpD41="compoundlit",
_tag_dyneither(_tmpD41,sizeof(char),12))),_tag_dyneither(_tmpD40,sizeof(void*),0)));}
_LL281: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp593=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)
_tmp511;if(_tmp593->tag != 37)goto _LL283;}_LL282: {const char*_tmpD44;void*_tmpD43;(
_tmpD43=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD44="valueof(-)",_tag_dyneither(_tmpD44,sizeof(char),11))),_tag_dyneither(
_tmpD43,sizeof(void*),0)));}_LL283: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*
_tmp594=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp511;if(_tmp594->tag != 
38)goto _LL230;}_LL284: {const char*_tmpD47;void*_tmpD46;(_tmpD46=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD47="__asm__",
_tag_dyneither(_tmpD47,sizeof(char),8))),_tag_dyneither(_tmpD46,sizeof(void*),0)));}
_LL230:;};}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0);}struct _tuple23{
struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct
_tuple24{struct _tuple0*f1;void*f2;};struct _tuple25{struct Cyc_List_List*f1;struct
Cyc_Absyn_Pat*f2;};static struct _tuple23 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,
struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp7AD=p->r;struct Cyc_Absyn_Vardecl*_tmp7AF;struct
Cyc_Absyn_Vardecl _tmp7B0;struct _tuple0*_tmp7B1;struct Cyc_Absyn_Pat*_tmp7B2;
struct Cyc_Absyn_Vardecl*_tmp7B4;struct Cyc_Absyn_Vardecl _tmp7B5;struct _tuple0*
_tmp7B6;struct Cyc_Absyn_Vardecl*_tmp7B9;struct Cyc_Absyn_Pat*_tmp7BA;enum Cyc_Absyn_Sign
_tmp7BD;int _tmp7BE;char _tmp7C0;struct _dyneither_ptr _tmp7C2;int _tmp7C3;struct Cyc_Absyn_Enumdecl*
_tmp7C5;struct Cyc_Absyn_Enumfield*_tmp7C6;void*_tmp7C8;struct Cyc_Absyn_Enumfield*
_tmp7C9;struct Cyc_Absyn_Pat*_tmp7CB;struct Cyc_Absyn_Pat _tmp7CC;void*_tmp7CD;
struct Cyc_Absyn_Datatypedecl*_tmp7CF;struct Cyc_Absyn_Datatypefield*_tmp7D0;
struct Cyc_List_List*_tmp7D1;struct Cyc_List_List*_tmp7D3;struct Cyc_List_List*
_tmp7D5;struct Cyc_Absyn_AggrInfo*_tmp7D7;struct Cyc_Absyn_AggrInfo*_tmp7D9;struct
Cyc_Absyn_AggrInfo _tmp7DA;union Cyc_Absyn_AggrInfoU _tmp7DB;struct Cyc_List_List*
_tmp7DC;struct Cyc_Absyn_Pat*_tmp7DE;_LL33D: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*
_tmp7AE=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7AD;if(_tmp7AE->tag != 1)
goto _LL33F;else{_tmp7AF=_tmp7AE->f1;_tmp7B0=*_tmp7AF;_tmp7B1=_tmp7B0.name;
_tmp7B2=_tmp7AE->f2;}}_LL33E: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,
_tmp7B1,r),rgn,t,r,path,_tmp7B2,fail_stmt,decls);_LL33F: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*
_tmp7B3=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp7AD;if(_tmp7B3->tag
!= 3)goto _LL341;else{_tmp7B4=_tmp7B3->f2;_tmp7B5=*_tmp7B4;_tmp7B6=_tmp7B5.name;}}
_LL340: nv=Cyc_Toc_add_varmap(rgn,nv,_tmp7B6,r);goto _LL342;_LL341: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*
_tmp7B7=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp7AD;if(_tmp7B7->tag != 
0)goto _LL343;}_LL342: s=Cyc_Toc_skip_stmt_dl();goto _LL33C;_LL343: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*
_tmp7B8=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp7AD;if(_tmp7B8->tag
!= 2)goto _LL345;else{_tmp7B9=_tmp7B8->f1;_tmp7BA=_tmp7B8->f2;}}_LL344: {struct
_tuple0*_tmp7E2=Cyc_Toc_temp_var();{struct _tuple24*_tmpD4A;struct Cyc_List_List*
_tmpD49;decls=((_tmpD49=_region_malloc(rgn,sizeof(*_tmpD49)),((_tmpD49->hd=((
_tmpD4A=_region_malloc(rgn,sizeof(*_tmpD4A)),((_tmpD4A->f1=_tmp7E2,((_tmpD4A->f2=
Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpD4A)))))),((_tmpD49->tl=
decls,_tmpD49))))));}nv=Cyc_Toc_add_varmap(rgn,nv,_tmp7B9->name,Cyc_Absyn_var_exp(
_tmp7E2,0));s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7E2,0),Cyc_Toc_cast_it(
Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(
path)),0);{struct _tuple23 _tmp7E5=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp7BA,
fail_stmt,decls);_tmp7E5.f3=Cyc_Absyn_seq_stmt(s,_tmp7E5.f3,0);return _tmp7E5;};}
_LL345: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp7BB=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)
_tmp7AD;if(_tmp7BB->tag != 8)goto _LL347;}_LL346: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL33C;_LL347: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*
_tmp7BC=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp7AD;if(_tmp7BC->tag != 9)
goto _LL349;else{_tmp7BD=_tmp7BC->f1;_tmp7BE=_tmp7BC->f2;}}_LL348: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_int_exp(_tmp7BD,_tmp7BE,0),fail_stmt);goto _LL33C;_LL349: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*
_tmp7BF=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp7AD;if(_tmp7BF->tag != 
10)goto _LL34B;else{_tmp7C0=_tmp7BF->f1;}}_LL34A: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp7C0,0),fail_stmt);goto _LL33C;_LL34B: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*
_tmp7C1=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp7AD;if(_tmp7C1->tag != 
11)goto _LL34D;else{_tmp7C2=_tmp7C1->f1;_tmp7C3=_tmp7C1->f2;}}_LL34C: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_float_exp(_tmp7C2,_tmp7C3,0),fail_stmt);goto _LL33C;_LL34D: {struct
Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp7C4=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)
_tmp7AD;if(_tmp7C4->tag != 12)goto _LL34F;else{_tmp7C5=_tmp7C4->f1;_tmp7C6=_tmp7C4->f2;}}
_LL34E:{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpD4D;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*
_tmpD4C;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD4C=_cycalloc(
sizeof(*_tmpD4C)),((_tmpD4C[0]=((_tmpD4D.tag=30,((_tmpD4D.f1=_tmp7C6->name,((
_tmpD4D.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7C5,((_tmpD4D.f3=(struct Cyc_Absyn_Enumfield*)
_tmp7C6,_tmpD4D)))))))),_tmpD4C)))),0),fail_stmt);}goto _LL33C;_LL34F: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*
_tmp7C7=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp7AD;if(_tmp7C7->tag
!= 13)goto _LL351;else{_tmp7C8=(void*)_tmp7C7->f1;_tmp7C9=_tmp7C7->f2;}}_LL350:{
struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpD50;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*
_tmpD4F;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD4F=_cycalloc(
sizeof(*_tmpD4F)),((_tmpD4F[0]=((_tmpD50.tag=31,((_tmpD50.f1=_tmp7C9->name,((
_tmpD50.f2=(void*)_tmp7C8,((_tmpD50.f3=(struct Cyc_Absyn_Enumfield*)_tmp7C9,
_tmpD50)))))))),_tmpD4F)))),0),fail_stmt);}goto _LL33C;_LL351: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*
_tmp7CA=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp7AD;if(_tmp7CA->tag
!= 5)goto _LL353;else{_tmp7CB=_tmp7CA->f1;_tmp7CC=*_tmp7CB;_tmp7CD=_tmp7CC.r;{
struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp7CE=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)
_tmp7CD;if(_tmp7CE->tag != 7)goto _LL353;else{_tmp7CF=_tmp7CE->f1;_tmp7D0=_tmp7CE->f2;
_tmp7D1=_tmp7CE->f3;}};}}_LL352: s=Cyc_Toc_skip_stmt_dl();{struct _tuple0*_tmp7EA=
Cyc_Toc_temp_var();struct _tuple0*tufstrct=Cyc_Toc_collapse_qvars(_tmp7D0->name,
_tmp7CF->name);void*_tmp7EB=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp7EB,r);struct Cyc_List_List*
_tmp7EC=_tmp7D0->typs;for(0;_tmp7D1 != 0;(((_tmp7D1=_tmp7D1->tl,_tmp7EC=((struct
Cyc_List_List*)_check_null(_tmp7EC))->tl)),++ cnt)){struct Cyc_Absyn_Pat*_tmp7ED=(
struct Cyc_Absyn_Pat*)_tmp7D1->hd;if(_tmp7ED->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{void*_tmp7EE=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(
_tmp7EC))->hd)).f2;struct _tuple0*_tmp7EF=Cyc_Toc_temp_var();void*_tmp7F0=(void*)
_check_null(_tmp7ED->topt);void*_tmp7F1=Cyc_Toc_typ_to_c(_tmp7F0);struct Cyc_Absyn_Exp*
_tmp7F2=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7EA,0),Cyc_Absyn_fieldname(
cnt),0);if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp7EE)))_tmp7F2=
Cyc_Toc_cast_it(_tmp7F1,_tmp7F2);{struct _tuple24*_tmpD53;struct Cyc_List_List*
_tmpD52;decls=((_tmpD52=_region_malloc(rgn,sizeof(*_tmpD52)),((_tmpD52->hd=((
_tmpD53=_region_malloc(rgn,sizeof(*_tmpD53)),((_tmpD53->f1=_tmp7EF,((_tmpD53->f2=
_tmp7F1,_tmpD53)))))),((_tmpD52->tl=decls,_tmpD52))))));}{struct _tuple23 _tmp7F5=
Cyc_Toc_xlate_pat(nv,rgn,_tmp7F0,Cyc_Absyn_var_exp(_tmp7EF,0),_tmp7F2,_tmp7ED,
fail_stmt,decls);nv=_tmp7F5.f1;decls=_tmp7F5.f2;{struct Cyc_Absyn_Stmt*_tmp7F6=
_tmp7F5.f3;struct Cyc_Absyn_Stmt*_tmp7F7=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp7EF,0),_tmp7F2,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7F7,_tmp7F6,
0),0);};};};}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(
_tmp7EA,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=_tmp7CF->is_extensible?
Cyc_Absyn_var_exp(_tmp7D0->name,0): Cyc_Toc_datatype_tag(_tmp7CF,_tmp7D0->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(
test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))s=
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp7EA,0),Cyc_Absyn_uint_exp(
0,0),0),fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(_tmp7EA,_tmp7EB,(struct Cyc_Absyn_Exp*)
rcast,s,0);goto _LL33C;};};_LL353: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*
_tmp7D2=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp7AD;if(_tmp7D2->tag
!= 7)goto _LL355;else{_tmp7D3=_tmp7D2->f3;}}_LL354: _tmp7D5=_tmp7D3;goto _LL356;
_LL355: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp7D4=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)
_tmp7AD;if(_tmp7D4->tag != 4)goto _LL357;else{_tmp7D5=_tmp7D4->f1;}}_LL356: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;for(0;_tmp7D5 != 0;(_tmp7D5=_tmp7D5->tl,++ cnt)){struct Cyc_Absyn_Pat*
_tmp7F8=(struct Cyc_Absyn_Pat*)_tmp7D5->hd;if(_tmp7F8->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{struct _tuple0*_tmp7F9=Cyc_Toc_temp_var();void*_tmp7FA=(void*)
_check_null(_tmp7F8->topt);{struct _tuple24*_tmpD56;struct Cyc_List_List*_tmpD55;
decls=((_tmpD55=_region_malloc(rgn,sizeof(*_tmpD55)),((_tmpD55->hd=((_tmpD56=
_region_malloc(rgn,sizeof(*_tmpD56)),((_tmpD56->f1=_tmp7F9,((_tmpD56->f2=Cyc_Toc_typ_to_c(
_tmp7FA),_tmpD56)))))),((_tmpD55->tl=decls,_tmpD55))))));}{struct _tuple23 _tmp7FD=
Cyc_Toc_xlate_pat(nv,rgn,_tmp7FA,Cyc_Absyn_var_exp(_tmp7F9,0),Cyc_Absyn_aggrmember_exp(
path,Cyc_Absyn_fieldname(cnt),0),_tmp7F8,fail_stmt,decls);nv=_tmp7FD.f1;decls=
_tmp7FD.f2;{struct Cyc_Absyn_Stmt*_tmp7FE=_tmp7FD.f3;struct Cyc_Absyn_Stmt*_tmp7FF=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7F9,0),Cyc_Absyn_aggrmember_exp(r,
Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7FF,
_tmp7FE,0),0);};};};}goto _LL33C;};_LL357: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*
_tmp7D6=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp7AD;if(_tmp7D6->tag != 
6)goto _LL359;else{_tmp7D7=_tmp7D6->f1;if(_tmp7D7 != 0)goto _LL359;}}_LL358: {const
char*_tmpD59;void*_tmpD58;(_tmpD58=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD59="unresolved aggregate pattern!",
_tag_dyneither(_tmpD59,sizeof(char),30))),_tag_dyneither(_tmpD58,sizeof(void*),0)));}
_LL359: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7D8=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)
_tmp7AD;if(_tmp7D8->tag != 6)goto _LL35B;else{_tmp7D9=_tmp7D8->f1;if(_tmp7D9 == 0)
goto _LL35B;_tmp7DA=*_tmp7D9;_tmp7DB=_tmp7DA.aggr_info;_tmp7DC=_tmp7D8->f3;}}
_LL35A: {struct Cyc_Absyn_Aggrdecl*_tmp802=Cyc_Absyn_get_known_aggrdecl(_tmp7DB);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp802->impl))->tagged){struct
_tuple25 _tmp804;struct Cyc_List_List*_tmp805;struct Cyc_Absyn_Pat*_tmp806;struct
_tuple25*_tmp803=(struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp7DC))->hd;
_tmp804=*_tmp803;_tmp805=_tmp804.f1;_tmp806=_tmp804.f2;{struct _dyneither_ptr*f;{
void*_tmp807=(void*)((struct Cyc_List_List*)_check_null(_tmp805))->hd;struct
_dyneither_ptr*_tmp809;_LL364: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*
_tmp808=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp807;if(_tmp808->tag
!= 1)goto _LL366;else{_tmp809=_tmp808->f1;}}_LL365: f=_tmp809;goto _LL363;_LL366:;
_LL367: {const char*_tmpD5C;void*_tmpD5B;(_tmpD5B=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD5C="no field name in tagged union pattern",
_tag_dyneither(_tmpD5C,sizeof(char),38))),_tag_dyneither(_tmpD5B,sizeof(void*),0)));}
_LL363:;}{struct _tuple0*_tmp80C=Cyc_Toc_temp_var();void*_tmp80D=(void*)
_check_null(_tmp806->topt);void*_tmp80E=Cyc_Toc_typ_to_c(_tmp80D);{struct
_tuple24*_tmpD5F;struct Cyc_List_List*_tmpD5E;decls=((_tmpD5E=_region_malloc(rgn,
sizeof(*_tmpD5E)),((_tmpD5E->hd=((_tmpD5F=_region_malloc(rgn,sizeof(*_tmpD5F)),((
_tmpD5F->f1=_tmp80C,((_tmpD5F->f2=_tmp80E,_tmpD5F)))))),((_tmpD5E->tl=decls,
_tmpD5E))))));}{struct Cyc_Absyn_Exp*_tmp811=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(
path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp812=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);_tmp812=Cyc_Toc_cast_it(
_tmp80E,_tmp812);_tmp811=Cyc_Toc_cast_it(_tmp80E,_tmp811);{struct _tuple23 _tmp813=
Cyc_Toc_xlate_pat(nv,rgn,_tmp80D,Cyc_Absyn_var_exp(_tmp80C,0),_tmp811,_tmp806,
fail_stmt,decls);nv=_tmp813.f1;decls=_tmp813.f2;{struct Cyc_Absyn_Stmt*_tmp814=
_tmp813.f3;struct Cyc_Absyn_Stmt*_tmp815=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp802,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp816=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp80C,0),_tmp812,0);s=Cyc_Absyn_seq_stmt(_tmp815,Cyc_Absyn_seq_stmt(_tmp816,
_tmp814,0),0);};};};};};}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp7DC != 0;_tmp7DC=
_tmp7DC->tl){struct _tuple25*_tmp817=(struct _tuple25*)_tmp7DC->hd;struct Cyc_Absyn_Pat*
_tmp818=(*_tmp817).f2;if(_tmp818->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp819=(void*)((struct Cyc_List_List*)_check_null((*
_tmp817).f1))->hd;struct _dyneither_ptr*_tmp81B;_LL369: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*
_tmp81A=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp819;if(_tmp81A->tag
!= 1)goto _LL36B;else{_tmp81B=_tmp81A->f1;}}_LL36A: f=_tmp81B;goto _LL368;_LL36B:;
_LL36C: {struct Cyc_Toc_Match_error_exn_struct _tmpD62;struct Cyc_Toc_Match_error_exn_struct*
_tmpD61;(int)_throw((void*)((_tmpD61=_cycalloc_atomic(sizeof(*_tmpD61)),((
_tmpD61[0]=((_tmpD62.tag=Cyc_Toc_Match_error,_tmpD62)),_tmpD61)))));}_LL368:;}{
struct _tuple0*_tmp81E=Cyc_Toc_temp_var();void*_tmp81F=(void*)_check_null(_tmp818->topt);
void*_tmp820=Cyc_Toc_typ_to_c(_tmp81F);{struct _tuple24*_tmpD65;struct Cyc_List_List*
_tmpD64;decls=((_tmpD64=_region_malloc(rgn,sizeof(*_tmpD64)),((_tmpD64->hd=((
_tmpD65=_region_malloc(rgn,sizeof(*_tmpD65)),((_tmpD65->f1=_tmp81E,((_tmpD65->f2=
_tmp820,_tmpD65)))))),((_tmpD64->tl=decls,_tmpD64))))));}{struct _tuple23 _tmp823=
Cyc_Toc_xlate_pat(nv,rgn,_tmp81F,Cyc_Absyn_var_exp(_tmp81E,0),Cyc_Absyn_aggrmember_exp(
path,f,0),_tmp818,fail_stmt,decls);nv=_tmp823.f1;decls=_tmp823.f2;{struct Cyc_Absyn_Exp*
_tmp824=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star_or_boxed_tvar(((
struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp802->impl))->fields,f)))->type))_tmp824=Cyc_Toc_cast_it(_tmp820,
_tmp824);{struct Cyc_Absyn_Stmt*_tmp825=_tmp823.f3;struct Cyc_Absyn_Stmt*_tmp826=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp81E,0),_tmp824,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp826,_tmp825,0),0);};};};};};}}goto _LL33C;}_LL35B: {
struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7DD=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)
_tmp7AD;if(_tmp7DD->tag != 5)goto _LL35D;else{_tmp7DE=_tmp7DD->f1;}}_LL35C: {
struct _tuple0*_tmp827=Cyc_Toc_temp_var();void*_tmp828=(void*)_check_null(_tmp7DE->topt);{
struct _tuple24*_tmpD68;struct Cyc_List_List*_tmpD67;decls=((_tmpD67=
_region_malloc(rgn,sizeof(*_tmpD67)),((_tmpD67->hd=((_tmpD68=_region_malloc(rgn,
sizeof(*_tmpD68)),((_tmpD68->f1=_tmp827,((_tmpD68->f2=Cyc_Toc_typ_to_c(_tmp828),
_tmpD68)))))),((_tmpD67->tl=decls,_tmpD67))))));}{struct _tuple23 _tmp82B=Cyc_Toc_xlate_pat(
nv,rgn,_tmp828,Cyc_Absyn_var_exp(_tmp827,0),Cyc_Absyn_deref_exp(path,0),_tmp7DE,
fail_stmt,decls);nv=_tmp82B.f1;decls=_tmp82B.f2;{struct Cyc_Absyn_Stmt*_tmp82C=
_tmp82B.f3;struct Cyc_Absyn_Stmt*_tmp82D=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp827,0),Cyc_Absyn_deref_exp(r,0),0),_tmp82C,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp82D,0);else{s=_tmp82D;}goto
_LL33C;};};}_LL35D: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp7DF=(
struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp7AD;if(_tmp7DF->tag != 14)
goto _LL35F;}_LL35E: {const char*_tmpD6B;void*_tmpD6A;(_tmpD6A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD6B="unknownid pat",
_tag_dyneither(_tmpD6B,sizeof(char),14))),_tag_dyneither(_tmpD6A,sizeof(void*),0)));}
_LL35F: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp7E0=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)
_tmp7AD;if(_tmp7E0->tag != 15)goto _LL361;}_LL360: {const char*_tmpD6E;void*_tmpD6D;(
_tmpD6D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD6E="unknowncall pat",_tag_dyneither(_tmpD6E,sizeof(char),16))),
_tag_dyneither(_tmpD6D,sizeof(void*),0)));}_LL361: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*
_tmp7E1=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp7AD;if(_tmp7E1->tag != 
16)goto _LL33C;}_LL362: {const char*_tmpD71;void*_tmpD70;(_tmpD70=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD71="exp pat",
_tag_dyneither(_tmpD71,sizeof(char),8))),_tag_dyneither(_tmpD70,sizeof(void*),0)));}
_LL33C:;}{struct _tuple23 _tmpD72;return(_tmpD72.f1=nv,((_tmpD72.f2=decls,((
_tmpD72.f3=s,_tmpD72)))));};}struct _tuple26{struct _dyneither_ptr*f1;struct
_dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple26*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){struct _tuple26*_tmpD73;
return(_tmpD73=_region_malloc(r,sizeof(*_tmpD73)),((_tmpD73->f1=Cyc_Toc_fresh_label(),((
_tmpD73->f2=Cyc_Toc_fresh_label(),((_tmpD73->f3=sc,_tmpD73)))))));}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*_tmp836=(void*)_check_null(e->topt);
int leave_as_switch;{void*_tmp837=Cyc_Tcutil_compress(_tmp836);_LL36E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp838=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp837;if(_tmp838->tag != 6)
goto _LL370;}_LL36F: goto _LL371;_LL370: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp839=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp837;if(_tmp839->tag != 
13)goto _LL372;}_LL371: leave_as_switch=1;goto _LL36D;_LL372:;_LL373:
leave_as_switch=0;goto _LL36D;_LL36D:;}{struct Cyc_List_List*_tmp83A=scs;for(0;
_tmp83A != 0;_tmp83A=_tmp83A->tl){if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp83A->hd)->pat_vars))->v != 0  || ((
struct Cyc_Absyn_Switch_clause*)_tmp83A->hd)->where_clause != 0){leave_as_switch=0;
break;}}}if(leave_as_switch){struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp83B=scs;for(0;_tmp83B != 0;_tmp83B=_tmp83B->tl){struct Cyc_Absyn_Stmt*
_tmp83C=((struct Cyc_Absyn_Switch_clause*)_tmp83B->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp83B->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp83C,0);next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp83D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp83D;
_push_region(rgn);Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),
_tmp83C);;_pop_region(rgn);};}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle _tmp83E=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp83E;_push_region(rgn);{struct Cyc_Toc_Env*
_tmp83F=Cyc_Toc_share_env(rgn,nv);struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple26*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,
rgn,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*
decls=0;{struct Cyc_List_List*_tmp840=lscs;for(0;_tmp840 != 0;_tmp840=_tmp840->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple26*)_tmp840->hd)).f3;struct
_dyneither_ptr*fail_lab=_tmp840->tl == 0?end_l:(*((struct _tuple26*)((struct Cyc_List_List*)
_check_null(_tmp840->tl))->hd)).f1;struct Cyc_Toc_Env*_tmp842;struct Cyc_List_List*
_tmp843;struct Cyc_Absyn_Stmt*_tmp844;struct _tuple23 _tmp841=Cyc_Toc_xlate_pat(
_tmp83F,rgn,_tmp836,r,path,sc->pattern,Cyc_Absyn_goto_stmt(fail_lab,0),decls);
_tmp842=_tmp841.f1;_tmp843=_tmp841.f2;_tmp844=_tmp841.f3;if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp845=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp842,_tmp845);_tmp844=Cyc_Absyn_seq_stmt(_tmp844,Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp845,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),
0),0);}decls=_tmp843;{struct Cyc_List_List*_tmpD74;nvs=((_tmpD74=_region_malloc(
rgn,sizeof(*_tmpD74)),((_tmpD74->hd=_tmp842,((_tmpD74->tl=nvs,_tmpD74))))));}{
struct Cyc_List_List*_tmpD75;test_stmts=((_tmpD75=_region_malloc(rgn,sizeof(*
_tmpD75)),((_tmpD75->hd=_tmp844,((_tmpD75->tl=test_stmts,_tmpD75))))));};}}nvs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),
test_stmts=test_stmts->tl)){struct _tuple26 _tmp849;struct _dyneither_ptr*_tmp84A;
struct _dyneither_ptr*_tmp84B;struct Cyc_Absyn_Switch_clause*_tmp84C;struct
_tuple26*_tmp848=(struct _tuple26*)lscs->hd;_tmp849=*_tmp848;_tmp84A=_tmp849.f1;
_tmp84B=_tmp849.f2;_tmp84C=_tmp849.f3;{struct Cyc_Toc_Env*_tmp84D=(struct Cyc_Toc_Env*)((
struct Cyc_List_List*)_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp84C->body;
struct _RegionHandle _tmp84E=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp84E;
_push_region(rgn2);if(lscs->tl != 0){struct _tuple26 _tmp850;struct _dyneither_ptr*
_tmp851;struct Cyc_Absyn_Switch_clause*_tmp852;struct _tuple26*_tmp84F=(struct
_tuple26*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp850=*_tmp84F;
_tmp851=_tmp850.f2;_tmp852=_tmp850.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(
rgn2,_tmp84D,end_l,_tmp851,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp852->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)
_check_null(nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(
rgn2,_tmp84D,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp84A,(
struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp84B,s,0),0);{struct Cyc_List_List*_tmpD76;stmts=((_tmpD76=_region_malloc(rgn,
sizeof(*_tmpD76)),((_tmpD76->hd=s,((_tmpD76->tl=stmts,_tmpD76))))));};;
_pop_region(rgn2);};}{struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(
end_l,Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct
_tuple24 _tmp855;struct _tuple0*_tmp856;void*_tmp857;struct _tuple24*_tmp854=(
struct _tuple24*)decls->hd;_tmp855=*_tmp854;_tmp856=_tmp855.f1;_tmp857=_tmp855.f2;
res=Cyc_Absyn_declare_stmt(_tmp856,_tmp857,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),(struct Cyc_Absyn_Exp*)e,res,0))->r;};};};
_pop_region(rgn);};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*
init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*_tmpD77;return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((_tmpD77=_cycalloc(sizeof(*_tmpD77)),((
_tmpD77->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD77->tl=0,_tmpD77)))))),
0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(
Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp859=s->r;struct
Cyc_Absyn_Exp*_tmp85C;struct Cyc_Absyn_Stmt*_tmp85E;struct Cyc_Absyn_Stmt*_tmp85F;
struct Cyc_Absyn_Exp*_tmp861;struct Cyc_Absyn_Exp*_tmp863;struct Cyc_Absyn_Stmt*
_tmp864;struct Cyc_Absyn_Stmt*_tmp865;struct _tuple9 _tmp867;struct Cyc_Absyn_Exp*
_tmp868;struct Cyc_Absyn_Stmt*_tmp869;struct Cyc_Absyn_Stmt*_tmp86B;struct Cyc_Absyn_Stmt*
_tmp86D;struct Cyc_Absyn_Stmt*_tmp86F;struct Cyc_Absyn_Exp*_tmp871;struct _tuple9
_tmp872;struct Cyc_Absyn_Exp*_tmp873;struct _tuple9 _tmp874;struct Cyc_Absyn_Exp*
_tmp875;struct Cyc_Absyn_Stmt*_tmp876;struct Cyc_Absyn_Exp*_tmp878;struct Cyc_List_List*
_tmp879;struct Cyc_List_List*_tmp87B;struct Cyc_Absyn_Switch_clause**_tmp87C;
struct Cyc_Absyn_Decl*_tmp87E;struct Cyc_Absyn_Stmt*_tmp87F;struct _dyneither_ptr*
_tmp881;struct Cyc_Absyn_Stmt*_tmp882;struct Cyc_Absyn_Stmt*_tmp884;struct _tuple9
_tmp885;struct Cyc_Absyn_Exp*_tmp886;struct Cyc_Absyn_Stmt*_tmp888;struct Cyc_List_List*
_tmp889;struct Cyc_Absyn_Exp*_tmp88B;_LL375: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*
_tmp85A=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp859;if(_tmp85A->tag != 
0)goto _LL377;}_LL376: return;_LL377: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*
_tmp85B=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp859;if(_tmp85B->tag != 
1)goto _LL379;else{_tmp85C=_tmp85B->f1;}}_LL378: Cyc_Toc_exp_to_c(nv,_tmp85C);
return;_LL379: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp85D=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)
_tmp859;if(_tmp85D->tag != 2)goto _LL37B;else{_tmp85E=_tmp85D->f1;_tmp85F=_tmp85D->f2;}}
_LL37A: Cyc_Toc_stmt_to_c(nv,_tmp85E);s=_tmp85F;continue;_LL37B: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*
_tmp860=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp859;if(_tmp860->tag
!= 3)goto _LL37D;else{_tmp861=_tmp860->f1;}}_LL37C: {void*topt=0;if(_tmp861 != 0){
topt=(void*)Cyc_Toc_typ_to_c((void*)_check_null(_tmp861->topt));Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp861);}if(s->try_depth > 0){if(topt != 0){struct
_tuple0*_tmp88C=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp88D=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp88C,0),0);s->r=(Cyc_Absyn_declare_stmt(
_tmp88C,(void*)topt,_tmp861,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),
_tmp88D,0),0))->r;}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}return;}_LL37D: {
struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp862=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)
_tmp859;if(_tmp862->tag != 4)goto _LL37F;else{_tmp863=_tmp862->f1;_tmp864=_tmp862->f2;
_tmp865=_tmp862->f3;}}_LL37E: Cyc_Toc_exp_to_c(nv,_tmp863);Cyc_Toc_stmt_to_c(nv,
_tmp864);s=_tmp865;continue;_LL37F: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*
_tmp866=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp859;if(_tmp866->tag
!= 5)goto _LL381;else{_tmp867=_tmp866->f1;_tmp868=_tmp867.f1;_tmp869=_tmp866->f2;}}
_LL380: Cyc_Toc_exp_to_c(nv,_tmp868);{struct _RegionHandle _tmp88E=_new_region("temp");
struct _RegionHandle*temp=& _tmp88E;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
temp,nv),_tmp869);_npop_handler(0);return;;_pop_region(temp);};_LL381: {struct
Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp86A=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)
_tmp859;if(_tmp86A->tag != 6)goto _LL383;else{_tmp86B=_tmp86A->f1;}}_LL382: {
struct Cyc_Toc_Env _tmp890;struct _dyneither_ptr**_tmp891;struct Cyc_Toc_Env*_tmp88F=
nv;_tmp890=*_tmp88F;_tmp891=_tmp890.break_lab;if(_tmp891 != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp891,0);{int dest_depth=_tmp86B == 0?0: _tmp86B->try_depth;Cyc_Toc_do_npop_before(
s->try_depth - dest_depth,s);return;};}_LL383: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*
_tmp86C=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp859;if(_tmp86C->tag
!= 7)goto _LL385;else{_tmp86D=_tmp86C->f1;}}_LL384: {struct Cyc_Toc_Env _tmp893;
struct _dyneither_ptr**_tmp894;struct Cyc_Toc_Env*_tmp892=nv;_tmp893=*_tmp892;
_tmp894=_tmp893.continue_lab;if(_tmp894 != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp894,0);
_tmp86F=_tmp86D;goto _LL386;}_LL385: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*
_tmp86E=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp859;if(_tmp86E->tag != 
8)goto _LL387;else{_tmp86F=_tmp86E->f2;}}_LL386: Cyc_Toc_do_npop_before(s->try_depth
- ((struct Cyc_Absyn_Stmt*)_check_null(_tmp86F))->try_depth,s);return;_LL387: {
struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp870=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)
_tmp859;if(_tmp870->tag != 9)goto _LL389;else{_tmp871=_tmp870->f1;_tmp872=_tmp870->f2;
_tmp873=_tmp872.f1;_tmp874=_tmp870->f3;_tmp875=_tmp874.f1;_tmp876=_tmp870->f4;}}
_LL388: Cyc_Toc_exp_to_c(nv,_tmp871);Cyc_Toc_exp_to_c(nv,_tmp873);Cyc_Toc_exp_to_c(
nv,_tmp875);{struct _RegionHandle _tmp895=_new_region("temp");struct _RegionHandle*
temp=& _tmp895;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp876);_npop_handler(0);return;;_pop_region(temp);};_LL389: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*
_tmp877=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp859;if(_tmp877->tag
!= 10)goto _LL38B;else{_tmp878=_tmp877->f1;_tmp879=_tmp877->f2;}}_LL38A: Cyc_Toc_xlate_switch(
nv,s,_tmp878,_tmp879);return;_LL38B: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*
_tmp87A=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp859;if(_tmp87A->tag
!= 11)goto _LL38D;else{_tmp87B=_tmp87A->f1;_tmp87C=_tmp87A->f2;}}_LL38C: {struct
Cyc_Toc_Env _tmp897;struct Cyc_Toc_FallthruInfo*_tmp898;struct Cyc_Toc_Env*_tmp896=
nv;_tmp897=*_tmp896;_tmp898=_tmp897.fallthru_info;if(_tmp898 == 0){const char*
_tmpD7A;void*_tmpD79;(_tmpD79=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD7A="fallthru in unexpected place",
_tag_dyneither(_tmpD7A,sizeof(char),29))),_tag_dyneither(_tmpD79,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp89C;struct Cyc_List_List*_tmp89D;struct Cyc_Dict_Dict
_tmp89E;struct Cyc_Toc_FallthruInfo _tmp89B=*_tmp898;_tmp89C=_tmp89B.label;_tmp89D=
_tmp89B.binders;_tmp89E=_tmp89B.next_case_env;{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(
_tmp89C,0);Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)
_check_null(_tmp87C)))->body)->try_depth,s2);{struct Cyc_List_List*_tmp89F=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp89D);struct Cyc_List_List*
_tmp8A0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp87B);
for(0;_tmp89F != 0;(_tmp89F=_tmp89F->tl,_tmp8A0=_tmp8A0->tl)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp8A0))->hd);s2=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct
_tuple0*k))Cyc_Dict_lookup)(_tmp89E,(struct _tuple0*)_tmp89F->hd),(struct Cyc_Absyn_Exp*)
_tmp8A0->hd,0),s2,0);}s->r=s2->r;return;};};};}_LL38D: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*
_tmp87D=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp859;if(_tmp87D->tag != 
12)goto _LL38F;else{_tmp87E=_tmp87D->f1;_tmp87F=_tmp87D->f2;}}_LL38E:{void*
_tmp8A1=_tmp87E->r;struct Cyc_Absyn_Vardecl*_tmp8A3;struct Cyc_Absyn_Pat*_tmp8A5;
struct Cyc_Absyn_Exp*_tmp8A6;struct Cyc_List_List*_tmp8A8;struct Cyc_Absyn_Fndecl*
_tmp8AA;struct Cyc_Absyn_Tvar*_tmp8AC;struct Cyc_Absyn_Vardecl*_tmp8AD;int _tmp8AE;
struct Cyc_Absyn_Exp*_tmp8AF;struct Cyc_Absyn_Exp*_tmp8B1;struct Cyc_Absyn_Tvar*
_tmp8B2;struct Cyc_Absyn_Vardecl*_tmp8B3;_LL398: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*
_tmp8A2=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8A1;if(_tmp8A2->tag != 
0)goto _LL39A;else{_tmp8A3=_tmp8A2->f1;}}_LL399: {struct _RegionHandle _tmp8B4=
_new_region("temp");struct _RegionHandle*temp=& _tmp8B4;_push_region(temp);{struct
Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD7D;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmpD7C;struct Cyc_Toc_Env*_tmp8B5=Cyc_Toc_add_varmap(temp,nv,_tmp8A3->name,Cyc_Absyn_varb_exp(
_tmp8A3->name,(void*)((_tmpD7C=_cycalloc(sizeof(*_tmpD7C)),((_tmpD7C[0]=((
_tmpD7D.tag=4,((_tmpD7D.f1=_tmp8A3,_tmpD7D)))),_tmpD7C)))),0));Cyc_Toc_local_decl_to_c(
_tmp8B5,_tmp8B5,_tmp8A3,_tmp87F);}_npop_handler(0);goto _LL397;;_pop_region(temp);}
_LL39A: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp8A4=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)
_tmp8A1;if(_tmp8A4->tag != 2)goto _LL39C;else{_tmp8A5=_tmp8A4->f1;_tmp8A6=_tmp8A4->f3;}}
_LL39B:{void*_tmp8B8=_tmp8A5->r;struct Cyc_Absyn_Vardecl*_tmp8BA;struct Cyc_Absyn_Pat*
_tmp8BB;struct Cyc_Absyn_Pat _tmp8BC;void*_tmp8BD;_LL3A7: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*
_tmp8B9=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp8B8;if(_tmp8B9->tag != 1)
goto _LL3A9;else{_tmp8BA=_tmp8B9->f1;_tmp8BB=_tmp8B9->f2;_tmp8BC=*_tmp8BB;_tmp8BD=
_tmp8BC.r;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp8BE=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)
_tmp8BD;if(_tmp8BE->tag != 0)goto _LL3A9;};}}_LL3A8: {struct _tuple0*old_name=
_tmp8BA->name;struct _tuple0*new_name=Cyc_Toc_temp_var();_tmp8BA->name=new_name;
_tmp8BA->initializer=(struct Cyc_Absyn_Exp*)_tmp8A6;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct
_tmpD80;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD7F;_tmp87E->r=(void*)((
_tmpD7F=_cycalloc(sizeof(*_tmpD7F)),((_tmpD7F[0]=((_tmpD80.tag=0,((_tmpD80.f1=
_tmp8BA,_tmpD80)))),_tmpD7F))));}{struct _RegionHandle _tmp8C1=_new_region("temp");
struct _RegionHandle*temp=& _tmp8C1;_push_region(temp);{struct Cyc_Absyn_Local_b_Absyn_Binding_struct
_tmpD83;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD82;struct Cyc_Toc_Env*
_tmp8C2=Cyc_Toc_add_varmap(temp,nv,old_name,Cyc_Absyn_varb_exp(new_name,(void*)((
_tmpD82=_cycalloc(sizeof(*_tmpD82)),((_tmpD82[0]=((_tmpD83.tag=4,((_tmpD83.f1=
_tmp8BA,_tmpD83)))),_tmpD82)))),0));Cyc_Toc_local_decl_to_c(_tmp8C2,nv,_tmp8BA,
_tmp87F);}_npop_handler(0);goto _LL3A6;;_pop_region(temp);};}_LL3A9:;_LL3AA: s->r=(
Cyc_Toc_letdecl_to_c(nv,_tmp8A5,(void*)_check_null(_tmp8A6->topt),_tmp8A6,
_tmp87F))->r;goto _LL3A6;_LL3A6:;}goto _LL397;_LL39C: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*
_tmp8A7=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp8A1;if(_tmp8A7->tag != 
3)goto _LL39E;else{_tmp8A8=_tmp8A7->f1;}}_LL39D: {struct Cyc_List_List*_tmp8C5=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp8A8);if(_tmp8C5
== 0){const char*_tmpD86;void*_tmpD85;(_tmpD85=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD86="empty Letv_d",
_tag_dyneither(_tmpD86,sizeof(char),13))),_tag_dyneither(_tmpD85,sizeof(void*),0)));}{
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD89;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*
_tmpD88;_tmp87E->r=(void*)((_tmpD88=_cycalloc(sizeof(*_tmpD88)),((_tmpD88[0]=((
_tmpD89.tag=0,((_tmpD89.f1=(struct Cyc_Absyn_Vardecl*)_tmp8C5->hd,_tmpD89)))),
_tmpD88))));}_tmp8C5=_tmp8C5->tl;for(0;_tmp8C5 != 0;_tmp8C5=_tmp8C5->tl){struct
Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD8C;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*
_tmpD8B;struct Cyc_Absyn_Decl*_tmp8CA=Cyc_Absyn_new_decl((void*)((_tmpD8B=
_cycalloc(sizeof(*_tmpD8B)),((_tmpD8B[0]=((_tmpD8C.tag=0,((_tmpD8C.f1=(struct Cyc_Absyn_Vardecl*)
_tmp8C5->hd,_tmpD8C)))),_tmpD8B)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp8CA,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL397;}_LL39E: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*
_tmp8A9=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8A1;if(_tmp8A9->tag != 1)
goto _LL3A0;else{_tmp8AA=_tmp8A9->f1;}}_LL39F: {struct _tuple0*_tmp8CD=_tmp8AA->name;
struct _RegionHandle _tmp8CE=_new_region("temp");struct _RegionHandle*temp=& _tmp8CE;
_push_region(temp);{struct Cyc_Toc_Env*_tmp8CF=Cyc_Toc_add_varmap(temp,nv,_tmp8AA->name,
Cyc_Absyn_var_exp(_tmp8CD,0));Cyc_Toc_fndecl_to_c(_tmp8CF,_tmp8AA,0);Cyc_Toc_stmt_to_c(
_tmp8CF,_tmp87F);}_npop_handler(0);goto _LL397;;_pop_region(temp);}_LL3A0: {
struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp8AB=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)
_tmp8A1;if(_tmp8AB->tag != 4)goto _LL3A2;else{_tmp8AC=_tmp8AB->f1;_tmp8AD=_tmp8AB->f2;
_tmp8AE=_tmp8AB->f3;_tmp8AF=_tmp8AB->f4;}}_LL3A1: {struct Cyc_Absyn_Stmt*_tmp8D0=
_tmp87F;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp8AD->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct _RegionHandle _tmp8D1=_new_region("temp");struct _RegionHandle*temp=&
_tmp8D1;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,
x_exp),_tmp8D0);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp8D0,0))->r;
else{if(_tmp8AF == 0){struct Cyc_Absyn_Exp*_tmpD8F[1];struct Cyc_Absyn_Exp*_tmpD8E[
1];struct Cyc_List_List*_tmpD8D;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpD8D=
_cycalloc(sizeof(*_tmpD8D)),((_tmpD8D->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpD8D->tl=0,_tmpD8D)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD8E[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD8E,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp8D0,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD8F[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD8F,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp8AF);{struct Cyc_Absyn_Exp*
_tmpD91[1];struct Cyc_Absyn_Exp*_tmpD90[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpD90[1]=(struct Cyc_Absyn_Exp*)_tmp8AF,((_tmpD90[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD90,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp8D0,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpD91[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD91,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
_npop_handler(0);return;;_pop_region(temp);}_LL3A2: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*
_tmp8B0=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp8A1;if(_tmp8B0->tag
!= 5)goto _LL3A4;else{_tmp8B1=_tmp8B0->f1;_tmp8B2=_tmp8B0->f2;_tmp8B3=_tmp8B0->f3;}}
_LL3A3: {struct _tuple0*old_name=_tmp8B3->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp8B3->name=new_name;_tmp8B3->initializer=(struct Cyc_Absyn_Exp*)_tmp8B1;{
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmpDA0;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*
_tmpD9F;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD9E;struct Cyc_Absyn_Decl*
_tmpD9D;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpD9C;s->r=(void*)((
_tmpD9C=_cycalloc(sizeof(*_tmpD9C)),((_tmpD9C[0]=((_tmpDA0.tag=12,((_tmpDA0.f1=((
_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D->r=(void*)((_tmpD9F=_cycalloc(
sizeof(*_tmpD9F)),((_tmpD9F[0]=((_tmpD9E.tag=0,((_tmpD9E.f1=_tmp8B3,_tmpD9E)))),
_tmpD9F)))),((_tmpD9D->loc=0,_tmpD9D)))))),((_tmpDA0.f2=_tmp87F,_tmpDA0)))))),
_tmpD9C))));}{struct _RegionHandle _tmp8DC=_new_region("temp");struct _RegionHandle*
temp=& _tmp8DC;_push_region(temp);{struct Cyc_Absyn_Local_b_Absyn_Binding_struct
_tmpDA3;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpDA2;struct Cyc_Toc_Env*
_tmp8DD=Cyc_Toc_add_varmap(temp,nv,old_name,Cyc_Absyn_varb_exp(new_name,(void*)((
_tmpDA2=_cycalloc(sizeof(*_tmpDA2)),((_tmpDA2[0]=((_tmpDA3.tag=4,((_tmpDA3.f1=
_tmp8B3,_tmpDA3)))),_tmpDA2)))),0));Cyc_Toc_local_decl_to_c(_tmp8DD,nv,_tmp8B3,
_tmp87F);}_npop_handler(0);return;;_pop_region(temp);};}_LL3A4:;_LL3A5: {const
char*_tmpDA6;void*_tmpDA5;(_tmpDA5=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDA6="bad nested declaration within function",
_tag_dyneither(_tmpDA6,sizeof(char),39))),_tag_dyneither(_tmpDA5,sizeof(void*),0)));}
_LL397:;}return;_LL38F: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp880=(
struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp859;if(_tmp880->tag != 13)goto
_LL391;else{_tmp881=_tmp880->f1;_tmp882=_tmp880->f2;}}_LL390: s=_tmp882;continue;
_LL391: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp883=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)
_tmp859;if(_tmp883->tag != 14)goto _LL393;else{_tmp884=_tmp883->f1;_tmp885=_tmp883->f2;
_tmp886=_tmp885.f1;}}_LL392: {struct _RegionHandle _tmp8E2=_new_region("temp");
struct _RegionHandle*temp=& _tmp8E2;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
temp,nv),_tmp884);Cyc_Toc_exp_to_c(nv,_tmp886);_npop_handler(0);return;;
_pop_region(temp);}_LL393: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*
_tmp887=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp859;if(_tmp887->tag
!= 15)goto _LL395;else{_tmp888=_tmp887->f1;_tmp889=_tmp887->f2;}}_LL394: {struct
_tuple0*h_var=Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct
_tuple0*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);e_exp->topt=(void*)e_typ;{
struct _RegionHandle _tmp8E3=_new_region("temp");struct _RegionHandle*temp=& _tmp8E3;
_push_region(temp);{struct Cyc_Toc_Env*_tmp8E4=Cyc_Toc_add_varmap(temp,nv,e_var,
e_exp);Cyc_Toc_stmt_to_c(_tmp8E4,_tmp888);{struct Cyc_Absyn_Stmt*_tmp8E5=Cyc_Absyn_seq_stmt(
_tmp888,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
struct _tuple0*_tmp8E6=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp8E7=Cyc_Absyn_var_exp(
_tmp8E6,0);struct Cyc_Absyn_Vardecl*_tmp8E8=Cyc_Absyn_new_vardecl(_tmp8E6,Cyc_Absyn_exn_typ(),
0);_tmp8E7->topt=(void*)Cyc_Absyn_exn_typ();{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*
_tmpDB0;struct Cyc_Absyn_Pat*_tmpDAF;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct
_tmpDAE;struct Cyc_Absyn_Pat*_tmpDAD;struct Cyc_Absyn_Pat*_tmp8E9=(_tmpDAD=
_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD->r=(void*)((_tmpDB0=_cycalloc(sizeof(*
_tmpDB0)),((_tmpDB0[0]=((_tmpDAE.tag=1,((_tmpDAE.f1=_tmp8E8,((_tmpDAE.f2=((
_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((_tmpDAF->r=(void*)& Cyc_Absyn_Wild_p_val,((
_tmpDAF->topt=(void*)Cyc_Absyn_exn_typ(),((_tmpDAF->loc=0,_tmpDAF)))))))),
_tmpDAE)))))),_tmpDB0)))),((_tmpDAD->topt=(void*)Cyc_Absyn_exn_typ(),((_tmpDAD->loc=
0,_tmpDAD)))))));struct Cyc_Absyn_Exp*_tmp8EA=Cyc_Absyn_throw_exp(_tmp8E7,0);
_tmp8EA->topt=(void*)& Cyc_Absyn_VoidType_val;{struct Cyc_Absyn_Stmt*_tmp8EB=Cyc_Absyn_exp_stmt(
_tmp8EA,0);struct Cyc_Core_Opt*_tmpDB6;struct Cyc_List_List*_tmpDB5;struct Cyc_Absyn_Switch_clause*
_tmpDB4;struct Cyc_Absyn_Switch_clause*_tmp8EC=(_tmpDB4=_cycalloc(sizeof(*_tmpDB4)),((
_tmpDB4->pattern=_tmp8E9,((_tmpDB4->pat_vars=((_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((
_tmpDB6->v=((_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((_tmpDB5->hd=_tmp8E8,((_tmpDB5->tl=
0,_tmpDB5)))))),_tmpDB6)))),((_tmpDB4->where_clause=0,((_tmpDB4->body=_tmp8EB,((
_tmpDB4->loc=0,_tmpDB4)))))))))));struct Cyc_List_List*_tmpDB7;struct Cyc_Absyn_Stmt*
_tmp8ED=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp889,((_tmpDB7=_cycalloc(sizeof(*
_tmpDB7)),((_tmpDB7->hd=_tmp8EC,((_tmpDB7->tl=0,_tmpDB7))))))),0);Cyc_Toc_stmt_to_c(
_tmp8E4,_tmp8ED);{struct Cyc_List_List*_tmpDB8;struct Cyc_Absyn_Exp*_tmp8EE=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,((_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8->hd=Cyc_Absyn_aggrmember_exp(
h_exp,Cyc_Toc_handler_sp,0),((_tmpDB8->tl=0,_tmpDB8)))))),0);struct Cyc_List_List*
_tmpDB9;struct Cyc_Absyn_Stmt*_tmp8EF=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpDB9->tl=0,_tmpDB9)))))),0),0);struct Cyc_Absyn_Exp*_tmp8F0=Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp8F1=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
1,0);s->r=(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp8EF,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp8F0,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp8EE,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp8F1,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp8E5,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp8ED,0),0),0),0),0),0))->r;};};};};}
_npop_handler(0);return;;_pop_region(temp);};}_LL395: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*
_tmp88A=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp859;if(_tmp88A->tag
!= 16)goto _LL374;else{_tmp88B=_tmp88A->f1;}}_LL396: if(Cyc_Absyn_no_regions)s->r=(
void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp88B);{struct Cyc_List_List*
_tmpDBA;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((_tmpDBA->hd=_tmp88B,((_tmpDBA->tl=0,
_tmpDBA)))))),0));};}return;_LL374:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,
int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8FD=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8FD;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp8FE=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp8FF=f->args;for(0;_tmp8FF != 0;_tmp8FF=_tmp8FF->tl){struct
_tuple0*_tmpDBB;struct _tuple0*_tmp900=(_tmpDBB=_cycalloc(sizeof(*_tmpDBB)),((
_tmpDBB->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpDBB->f2=(*((struct
_tuple8*)_tmp8FF->hd)).f1,_tmpDBB)))));(*((struct _tuple8*)_tmp8FF->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple8*)_tmp8FF->hd)).f3);_tmp8FE=Cyc_Toc_add_varmap(frgn,_tmp8FE,_tmp900,
Cyc_Absyn_var_exp(_tmp900,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct _dyneither_ptr*_tmp903;struct Cyc_Absyn_Tqual
_tmp904;void*_tmp905;int _tmp906;struct Cyc_Absyn_VarargInfo _tmp902=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp903=_tmp902.name;_tmp904=_tmp902.tq;_tmp905=
_tmp902.type;_tmp906=_tmp902.inject;{void*_tmp907=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp905,(void*)& Cyc_Absyn_HeapRgn_val,_tmp904,Cyc_Absyn_false_conref));struct
_tuple0*_tmpDBC;struct _tuple0*_tmp908=(_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((
_tmpDBC->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpDBC->f2=(struct
_dyneither_ptr*)_check_null(_tmp903),_tmpDBC)))));{struct _tuple8*_tmpDBF;struct
Cyc_List_List*_tmpDBE;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpDBE=_cycalloc(sizeof(*
_tmpDBE)),((_tmpDBE->hd=((_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF->f1=(
struct _dyneither_ptr*)_tmp903,((_tmpDBF->f2=_tmp904,((_tmpDBF->f3=_tmp907,
_tmpDBF)))))))),((_tmpDBE->tl=0,_tmpDBE)))))));}_tmp8FE=Cyc_Toc_add_varmap(frgn,
_tmp8FE,_tmp908,Cyc_Absyn_var_exp(_tmp908,0));f->cyc_varargs=0;};}{struct Cyc_List_List*
_tmp90C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp90C != 0;_tmp90C=_tmp90C->tl){((struct Cyc_Absyn_Vardecl*)_tmp90C->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp90C->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp8FE),f->body);};_pop_region(frgn);};}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL3AB: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3AC: return Cyc_Absyn_Extern;
default: _LL3AD: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad,int add_to_decls){struct _tuple0*_tmp90D=ad->name;struct _DynRegionHandle*
_tmp90F;struct Cyc_Dict_Dict*_tmp910;struct Cyc_Toc_TocState _tmp90E=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp90F=_tmp90E.dyn;_tmp910=_tmp90E.aggrs_so_far;{
struct _DynRegionFrame _tmp911;struct _RegionHandle*d=_open_dynregion(& _tmp911,
_tmp90F);{int seen_defn_before;struct _tuple15**_tmp912=((struct _tuple15**(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp910,_tmp90D);if(
_tmp912 == 0){seen_defn_before=0;{struct _tuple15*v;if(ad->kind == Cyc_Absyn_StructA){
struct _tuple15*_tmpDC0;v=((_tmpDC0=_region_malloc(d,sizeof(*_tmpDC0)),((_tmpDC0->f1=
ad,((_tmpDC0->f2=Cyc_Absyn_strctq(_tmp90D),_tmpDC0))))));}else{struct _tuple15*
_tmpDC1;v=((_tmpDC1=_region_malloc(d,sizeof(*_tmpDC1)),((_tmpDC1->f1=ad,((
_tmpDC1->f2=Cyc_Absyn_unionq_typ(_tmp90D),_tmpDC1))))));}*_tmp910=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp910,
_tmp90D,v);};}else{struct _tuple15 _tmp916;struct Cyc_Absyn_Aggrdecl*_tmp917;void*
_tmp918;struct _tuple15*_tmp915=*_tmp912;_tmp916=*_tmp915;_tmp917=_tmp916.f1;
_tmp918=_tmp916.f2;if(_tmp917->impl == 0){{struct _tuple15*_tmpDC2;*_tmp910=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))
Cyc_Dict_insert)(*_tmp910,_tmp90D,((_tmpDC2=_region_malloc(d,sizeof(*_tmpDC2)),((
_tmpDC2->f1=ad,((_tmpDC2->f2=_tmp918,_tmpDC2)))))));}seen_defn_before=0;}else{
seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*_tmpDC3;struct Cyc_Absyn_Aggrdecl*
new_ad=(_tmpDC3=_cycalloc(sizeof(*_tmpDC3)),((_tmpDC3->kind=ad->kind,((_tmpDC3->sc=
Cyc_Absyn_Public,((_tmpDC3->name=ad->name,((_tmpDC3->tvs=0,((_tmpDC3->impl=0,((
_tmpDC3->attributes=ad->attributes,_tmpDC3)))))))))))));if(ad->impl != 0  && !
seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpDC4;new_ad->impl=((_tmpDC4=
_cycalloc(sizeof(*_tmpDC4)),((_tmpDC4->exist_vars=0,((_tmpDC4->rgn_po=0,((
_tmpDC4->fields=0,((_tmpDC4->tagged=0,_tmpDC4))))))))));}{struct Cyc_List_List*
new_fields=0;{struct Cyc_List_List*_tmp91B=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp91B != 0;_tmp91B=_tmp91B->tl){struct Cyc_Absyn_Aggrfield*
_tmp91C=(struct Cyc_Absyn_Aggrfield*)_tmp91B->hd;if(_tmp91B->tl == 0  && Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp91C->type)))continue;{struct Cyc_Absyn_Aggrfield*
_tmpDC5;struct Cyc_Absyn_Aggrfield*_tmp91D=(_tmpDC5=_cycalloc(sizeof(*_tmpDC5)),((
_tmpDC5->name=_tmp91C->name,((_tmpDC5->tq=Cyc_Toc_mt_tq,((_tmpDC5->type=Cyc_Toc_typ_to_c_array(
_tmp91C->type),((_tmpDC5->width=_tmp91C->width,((_tmpDC5->attributes=_tmp91C->attributes,
_tmpDC5)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp91E=_tmp91D->type;struct _dyneither_ptr*_tmp91F=_tmp91D->name;const char*
_tmpDCA;void*_tmpDC9[2];struct Cyc_String_pa_PrintArg_struct _tmpDC8;struct Cyc_String_pa_PrintArg_struct
_tmpDC7;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpDC7.tag=0,((_tmpDC7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp91F),((_tmpDC8.tag=0,((_tmpDC8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpDC9[0]=&
_tmpDC8,((_tmpDC9[1]=& _tmpDC7,Cyc_aprintf(((_tmpDCA="_union_%s_%s",
_tag_dyneither(_tmpDCA,sizeof(char),13))),_tag_dyneither(_tmpDC9,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpDCB;struct _dyneither_ptr*str=(_tmpDCB=_cycalloc(sizeof(*
_tmpDCB)),((_tmpDCB[0]=s,_tmpDCB)));struct Cyc_Absyn_Aggrfield*_tmpDCC;struct Cyc_Absyn_Aggrfield*
_tmp920=(_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((_tmpDCC->name=Cyc_Toc_val_sp,((
_tmpDCC->tq=Cyc_Toc_mt_tq,((_tmpDCC->type=_tmp91E,((_tmpDCC->width=0,((_tmpDCC->attributes=
0,_tmpDCC)))))))))));struct Cyc_Absyn_Aggrfield*_tmpDCD;struct Cyc_Absyn_Aggrfield*
_tmp921=(_tmpDCD=_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD->name=Cyc_Toc_tag_sp,((
_tmpDCD->tq=Cyc_Toc_mt_tq,((_tmpDCD->type=Cyc_Absyn_sint_typ,((_tmpDCD->width=0,((
_tmpDCD->attributes=0,_tmpDCD)))))))))));struct Cyc_Absyn_Aggrfield*_tmpDCE[2];
struct Cyc_List_List*_tmp922=(_tmpDCE[1]=_tmp920,((_tmpDCE[0]=_tmp921,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDCE,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct Cyc_Absyn_AggrdeclImpl*_tmpDD3;struct _tuple0*_tmpDD2;struct Cyc_Absyn_Aggrdecl*
_tmpDD1;struct Cyc_Absyn_Aggrdecl*_tmp923=(_tmpDD1=_cycalloc(sizeof(*_tmpDD1)),((
_tmpDD1->kind=Cyc_Absyn_StructA,((_tmpDD1->sc=Cyc_Absyn_Public,((_tmpDD1->name=((
_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2->f1=Cyc_Absyn_Loc_n,((_tmpDD2->f2=
str,_tmpDD2)))))),((_tmpDD1->tvs=0,((_tmpDD1->impl=((_tmpDD3=_cycalloc(sizeof(*
_tmpDD3)),((_tmpDD3->exist_vars=0,((_tmpDD3->rgn_po=0,((_tmpDD3->fields=_tmp922,((
_tmpDD3->tagged=0,_tmpDD3)))))))))),((_tmpDD1->attributes=0,_tmpDD1)))))))))))));{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDD9;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct
_tmpDD8;struct Cyc_List_List*_tmpDD7;Cyc_Toc_result_decls=((_tmpDD7=_cycalloc(
sizeof(*_tmpDD7)),((_tmpDD7->hd=Cyc_Absyn_new_decl((void*)((_tmpDD9=_cycalloc(
sizeof(*_tmpDD9)),((_tmpDD9[0]=((_tmpDD8.tag=6,((_tmpDD8.f1=_tmp923,_tmpDD8)))),
_tmpDD9)))),0),((_tmpDD7->tl=Cyc_Toc_result_decls,_tmpDD7))))));}_tmp91D->type=
Cyc_Absyn_strct(str);}{struct Cyc_List_List*_tmpDDA;new_fields=((_tmpDDA=
_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA->hd=_tmp91D,((_tmpDDA->tl=new_fields,
_tmpDDA))))));};};}}(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(new_fields);};}if(add_to_decls){struct Cyc_Absyn_Decl*
_tmpDE4;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDE3;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*
_tmpDE2;struct Cyc_List_List*_tmpDE1;Cyc_Toc_result_decls=((_tmpDE1=_cycalloc(
sizeof(*_tmpDE1)),((_tmpDE1->hd=((_tmpDE4=_cycalloc(sizeof(*_tmpDE4)),((_tmpDE4->r=(
void*)((_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2[0]=((_tmpDE3.tag=6,((
_tmpDE3.f1=new_ad,_tmpDE3)))),_tmpDE2)))),((_tmpDE4->loc=0,_tmpDE4)))))),((
_tmpDE1->tl=Cyc_Toc_result_decls,_tmpDE1))))));}};};_pop_dynregion(d);};}static
void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){struct
_DynRegionHandle*_tmp93A;struct Cyc_Set_Set**_tmp93B;struct Cyc_Toc_TocState
_tmp939=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp93A=
_tmp939.dyn;_tmp93B=_tmp939.datatypes_so_far;{struct _DynRegionFrame _tmp93C;
struct _RegionHandle*d=_open_dynregion(& _tmp93C,_tmp93A);{struct _tuple0*_tmp93D=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))
Cyc_Set_member)(*_tmp93B,_tmp93D)){_npop_handler(0);return;}*_tmp93B=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*
_tmp93B,_tmp93D);}{struct Cyc_List_List*_tmp93E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp93E != 0;_tmp93E=_tmp93E->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp93E->hd;struct Cyc_List_List*_tmp93F=0;int i=
1;{struct Cyc_List_List*_tmp940=f->typs;for(0;_tmp940 != 0;(_tmp940=_tmp940->tl,i
++)){struct _dyneither_ptr*_tmp941=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpDE5;struct Cyc_Absyn_Aggrfield*_tmp942=(_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((
_tmpDE5->name=_tmp941,((_tmpDE5->tq=(*((struct _tuple10*)_tmp940->hd)).f1,((
_tmpDE5->type=Cyc_Toc_typ_to_c_array((*((struct _tuple10*)_tmp940->hd)).f2),((
_tmpDE5->width=0,((_tmpDE5->attributes=0,_tmpDE5)))))))))));struct Cyc_List_List*
_tmpDE6;_tmp93F=((_tmpDE6=_cycalloc(sizeof(*_tmpDE6)),((_tmpDE6->hd=_tmp942,((
_tmpDE6->tl=_tmp93F,_tmpDE6))))));}}{struct Cyc_Absyn_Aggrfield*_tmpDE9;struct Cyc_List_List*
_tmpDE8;_tmp93F=((_tmpDE8=_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8->hd=((_tmpDE9=
_cycalloc(sizeof(*_tmpDE9)),((_tmpDE9->name=Cyc_Toc_tag_sp,((_tmpDE9->tq=Cyc_Toc_mt_tq,((
_tmpDE9->type=Cyc_Absyn_sint_typ,((_tmpDE9->width=0,((_tmpDE9->attributes=0,
_tmpDE9)))))))))))),((_tmpDE8->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp93F),_tmpDE8))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpDEC;struct Cyc_Absyn_Aggrdecl*_tmpDEB;struct Cyc_Absyn_Aggrdecl*_tmp947=(
_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((_tmpDEB->kind=Cyc_Absyn_StructA,((_tmpDEB->sc=
Cyc_Absyn_Public,((_tmpDEB->name=Cyc_Toc_collapse_qvars(f->name,tud->name),((
_tmpDEB->tvs=0,((_tmpDEB->impl=((_tmpDEC=_cycalloc(sizeof(*_tmpDEC)),((_tmpDEC->exist_vars=
0,((_tmpDEC->rgn_po=0,((_tmpDEC->fields=_tmp93F,((_tmpDEC->tagged=0,_tmpDEC)))))))))),((
_tmpDEB->attributes=0,_tmpDEB)))))))))))));struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*
_tmpDF2;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDF1;struct Cyc_List_List*
_tmpDF0;Cyc_Toc_result_decls=((_tmpDF0=_cycalloc(sizeof(*_tmpDF0)),((_tmpDF0->hd=
Cyc_Absyn_new_decl((void*)((_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((_tmpDF2[0]=((
_tmpDF1.tag=6,((_tmpDF1.f1=_tmp947,_tmpDF1)))),_tmpDF2)))),0),((_tmpDF0->tl=Cyc_Toc_result_decls,
_tmpDF0))))));};}};;_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(
struct Cyc_Absyn_Datatypedecl*xd){if(xd->fields == 0)return;{struct
_DynRegionHandle*_tmp94E;struct Cyc_Dict_Dict*_tmp94F;struct Cyc_Toc_TocState
_tmp94D=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp94E=
_tmp94D.dyn;_tmp94F=_tmp94D.xdatatypes_so_far;{struct _DynRegionFrame _tmp950;
struct _RegionHandle*d=_open_dynregion(& _tmp950,_tmp94E);{struct _tuple0*_tmp951=
xd->name;struct Cyc_List_List*_tmp952=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp952 != 0;_tmp952=_tmp952->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp952->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp953=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp954=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp953,Cyc_Absyn_false_conref,0);int*_tmp955=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp94F,f->name);int
_tmp956;_LL3B0: if(_tmp955 != 0)goto _LL3B2;_LL3B1: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp957=Cyc_Absyn_new_vardecl(f->name,_tmp954,
initopt);_tmp957->sc=f->sc;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDF8;
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDF7;struct Cyc_List_List*_tmpDF6;
Cyc_Toc_result_decls=((_tmpDF6=_cycalloc(sizeof(*_tmpDF6)),((_tmpDF6->hd=Cyc_Absyn_new_decl((
void*)((_tmpDF8=_cycalloc(sizeof(*_tmpDF8)),((_tmpDF8[0]=((_tmpDF7.tag=0,((
_tmpDF7.f1=_tmp957,_tmpDF7)))),_tmpDF8)))),0),((_tmpDF6->tl=Cyc_Toc_result_decls,
_tmpDF6))))));}*_tmp94F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_tuple0*k,int v))Cyc_Dict_insert)(*_tmp94F,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;int i=1;{struct Cyc_List_List*_tmp95B=f->typs;for(0;
_tmp95B != 0;(_tmp95B=_tmp95B->tl,i ++)){struct Cyc_Int_pa_PrintArg_struct _tmpE00;
void*_tmpDFF[1];const char*_tmpDFE;struct _dyneither_ptr*_tmpDFD;struct
_dyneither_ptr*_tmp95C=(_tmpDFD=_cycalloc(sizeof(*_tmpDFD)),((_tmpDFD[0]=(struct
_dyneither_ptr)((_tmpE00.tag=1,((_tmpE00.f1=(unsigned long)i,((_tmpDFF[0]=&
_tmpE00,Cyc_aprintf(((_tmpDFE="f%d",_tag_dyneither(_tmpDFE,sizeof(char),4))),
_tag_dyneither(_tmpDFF,sizeof(void*),1)))))))),_tmpDFD)));struct Cyc_Absyn_Aggrfield*
_tmpE01;struct Cyc_Absyn_Aggrfield*_tmp95D=(_tmpE01=_cycalloc(sizeof(*_tmpE01)),((
_tmpE01->name=_tmp95C,((_tmpE01->tq=(*((struct _tuple10*)_tmp95B->hd)).f1,((
_tmpE01->type=Cyc_Toc_typ_to_c_array((*((struct _tuple10*)_tmp95B->hd)).f2),((
_tmpE01->width=0,((_tmpE01->attributes=0,_tmpE01)))))))))));struct Cyc_List_List*
_tmpE02;fields=((_tmpE02=_cycalloc(sizeof(*_tmpE02)),((_tmpE02->hd=_tmp95D,((
_tmpE02->tl=fields,_tmpE02))))));}}{struct Cyc_Absyn_Aggrfield*_tmpE05;struct Cyc_List_List*
_tmpE04;fields=((_tmpE04=_cycalloc(sizeof(*_tmpE04)),((_tmpE04->hd=((_tmpE05=
_cycalloc(sizeof(*_tmpE05)),((_tmpE05->name=Cyc_Toc_tag_sp,((_tmpE05->tq=Cyc_Toc_mt_tq,((
_tmpE05->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpE05->width=
0,((_tmpE05->attributes=0,_tmpE05)))))))))))),((_tmpE04->tl=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpE04))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpE08;struct Cyc_Absyn_Aggrdecl*_tmpE07;struct Cyc_Absyn_Aggrdecl*_tmp966=(
_tmpE07=_cycalloc(sizeof(*_tmpE07)),((_tmpE07->kind=Cyc_Absyn_StructA,((_tmpE07->sc=
Cyc_Absyn_Public,((_tmpE07->name=Cyc_Toc_collapse_qvars(f->name,xd->name),((
_tmpE07->tvs=0,((_tmpE07->impl=((_tmpE08=_cycalloc(sizeof(*_tmpE08)),((_tmpE08->exist_vars=
0,((_tmpE08->rgn_po=0,((_tmpE08->fields=fields,((_tmpE08->tagged=0,_tmpE08)))))))))),((
_tmpE07->attributes=0,_tmpE07)))))))))))));{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*
_tmpE0E;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpE0D;struct Cyc_List_List*
_tmpE0C;Cyc_Toc_result_decls=((_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((_tmpE0C->hd=
Cyc_Absyn_new_decl((void*)((_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E[0]=((
_tmpE0D.tag=6,((_tmpE0D.f1=_tmp966,_tmpE0D)))),_tmpE0E)))),0),((_tmpE0C->tl=Cyc_Toc_result_decls,
_tmpE0C))))));}goto _LL3AF;};};};}_LL3B2: if(_tmp955 == 0)goto _LL3B4;_tmp956=*
_tmp955;if(_tmp956 != 0)goto _LL3B4;_LL3B3: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*
_tmp96C=Cyc_Absyn_string_exp(*fn,0);struct Cyc_Absyn_Vardecl*_tmp96D=Cyc_Absyn_new_vardecl(
f->name,_tmp954,(struct Cyc_Absyn_Exp*)_tmp96C);_tmp96D->sc=f->sc;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*
_tmpE14;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpE13;struct Cyc_List_List*
_tmpE12;Cyc_Toc_result_decls=((_tmpE12=_cycalloc(sizeof(*_tmpE12)),((_tmpE12->hd=
Cyc_Absyn_new_decl((void*)((_tmpE14=_cycalloc(sizeof(*_tmpE14)),((_tmpE14[0]=((
_tmpE13.tag=0,((_tmpE13.f1=_tmp96D,_tmpE13)))),_tmpE14)))),0),((_tmpE12->tl=Cyc_Toc_result_decls,
_tmpE12))))));}*_tmp94F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_tuple0*k,int v))Cyc_Dict_insert)(*_tmp94F,f->name,1);}goto _LL3AF;_LL3B4:;_LL3B5:
goto _LL3AF;_LL3AF:;}};_pop_dynregion(d);};};}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Absyn_Enumdecl*ed){ed->sc=Cyc_Absyn_Public;if(ed->fields != 0)Cyc_Toc_enumfields_to_c((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}static
void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,
struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){void*old_typ=vd->type;vd->type=
Cyc_Toc_typ_to_c_array(old_typ);if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))vd->sc=Cyc_Absyn_Public;Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp971=init->r;struct Cyc_Absyn_Vardecl*_tmp973;struct Cyc_Absyn_Exp*_tmp974;
struct Cyc_Absyn_Exp*_tmp975;int _tmp976;_LL3B7: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*
_tmp972=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp971;if(
_tmp972->tag != 26)goto _LL3B9;else{_tmp973=_tmp972->f1;_tmp974=_tmp972->f2;
_tmp975=_tmp972->f3;_tmp976=_tmp972->f4;}}_LL3B8: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp973,_tmp974,_tmp975,_tmp976,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3B6;_LL3B9:;_LL3BA: if(vd->sc == Cyc_Absyn_Static){struct
_RegionHandle _tmp977=_new_region("temp");struct _RegionHandle*temp=& _tmp977;
_push_region(temp);{struct Cyc_Toc_Env*_tmp978=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp978,init);};_pop_region(temp);}else{Cyc_Toc_exp_to_c(
init_nv,init);}goto _LL3B6;_LL3B6:;}else{void*_tmp979=Cyc_Tcutil_compress(old_typ);
struct Cyc_Absyn_ArrayInfo _tmp97B;void*_tmp97C;struct Cyc_Absyn_Exp*_tmp97D;union
Cyc_Absyn_Constraint*_tmp97E;_LL3BC:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp97A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp979;if(_tmp97A->tag != 
8)goto _LL3BE;else{_tmp97B=_tmp97A->f1;_tmp97C=_tmp97B.elt_type;_tmp97D=_tmp97B.num_elts;
_tmp97E=_tmp97B.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp97E))goto _LL3BE;_LL3BD: if(_tmp97D == 0){const char*_tmpE17;void*_tmpE16;(
_tmpE16=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE17="can't initialize zero-terminated array -- size unknown",_tag_dyneither(
_tmpE17,sizeof(char),55))),_tag_dyneither(_tmpE16,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
num_elts=(struct Cyc_Absyn_Exp*)_tmp97D;struct Cyc_Absyn_Exp*_tmp981=Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(
- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp982=Cyc_Absyn_signed_int_exp(0,0);s->r=Cyc_Toc_seq_stmt_r(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp981,_tmp982,0),0),Cyc_Absyn_new_stmt(
s->r,0));goto _LL3BB;};_LL3BE:;_LL3BF: goto _LL3BB;_LL3BB:;}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{struct _tuple0*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp983=_new_region("prgn");struct _RegionHandle*prgn=& _tmp983;
_push_region(prgn);{struct Cyc_Absyn_Stmt*_tmp984=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp985=Cyc_Toc_share_env(prgn,nv);struct Cyc_Toc_Env*_tmp987;
struct Cyc_List_List*_tmp988;struct Cyc_Absyn_Stmt*_tmp989;struct _tuple23 _tmp986=
Cyc_Toc_xlate_pat(_tmp985,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);_tmp987=_tmp986.f1;_tmp988=_tmp986.f2;_tmp989=
_tmp986.f3;Cyc_Toc_stmt_to_c(_tmp987,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(
t),(struct Cyc_Absyn_Exp*)e,Cyc_Absyn_seq_stmt(_tmp989,s,0),0);for(0;_tmp988 != 0;
_tmp988=_tmp988->tl){struct _tuple24 _tmp98B;struct _tuple0*_tmp98C;void*_tmp98D;
struct _tuple24*_tmp98A=(struct _tuple24*)_tmp988->hd;_tmp98B=*_tmp98A;_tmp98C=
_tmp98B.f1;_tmp98D=_tmp98B.f2;s=Cyc_Absyn_declare_stmt(_tmp98C,_tmp98D,0,s,0);}}{
struct Cyc_Absyn_Stmt*_tmp98E=s;_npop_handler(0);return _tmp98E;};;_pop_region(
prgn);};}static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){void*_tmp98F=e->r;
struct Cyc_Absyn_Exp*_tmp991;struct Cyc_Absyn_Exp*_tmp993;struct Cyc_Absyn_Exp*
_tmp995;struct Cyc_Absyn_Exp*_tmp997;struct Cyc_Absyn_Exp*_tmp999;struct Cyc_Absyn_Exp*
_tmp99B;struct Cyc_Absyn_Exp*_tmp99D;struct Cyc_Absyn_Exp*_tmp99F;struct Cyc_List_List*
_tmp9A1;struct Cyc_Absyn_Exp*_tmp9A3;struct Cyc_Absyn_Exp*_tmp9A4;struct Cyc_Absyn_Exp*
_tmp9A6;struct Cyc_Absyn_Exp*_tmp9A7;struct Cyc_Absyn_Exp*_tmp9A9;struct Cyc_Absyn_Exp*
_tmp9AA;struct Cyc_Absyn_Exp*_tmp9AC;struct Cyc_Absyn_Exp*_tmp9AD;struct Cyc_Absyn_Exp*
_tmp9AF;struct Cyc_Absyn_Exp*_tmp9B0;struct Cyc_Absyn_Exp*_tmp9B2;struct Cyc_Absyn_Exp*
_tmp9B3;struct Cyc_Absyn_Exp*_tmp9B5;struct Cyc_Absyn_Exp*_tmp9B6;struct Cyc_Absyn_Exp*
_tmp9B7;struct Cyc_Absyn_Exp*_tmp9B9;struct Cyc_List_List*_tmp9BA;void*_tmp9BC;
void**_tmp9BD;struct Cyc_Absyn_Exp*_tmp9BE;struct _tuple8*_tmp9C0;struct _tuple8
_tmp9C1;void*_tmp9C2;void**_tmp9C3;struct Cyc_List_List*_tmp9C4;struct Cyc_List_List*
_tmp9C6;struct Cyc_List_List*_tmp9C8;void*_tmp9CA;void**_tmp9CB;void*_tmp9CD;void**
_tmp9CE;struct Cyc_Absyn_Stmt*_tmp9D0;struct Cyc_Absyn_MallocInfo _tmp9D2;struct Cyc_Absyn_MallocInfo*
_tmp9D3;_LL3C1: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp990=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)
_tmp98F;if(_tmp990->tag != 19)goto _LL3C3;else{_tmp991=_tmp990->f1;}}_LL3C2:
_tmp993=_tmp991;goto _LL3C4;_LL3C3: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*
_tmp992=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp992->tag
!= 20)goto _LL3C5;else{_tmp993=_tmp992->f1;}}_LL3C4: _tmp995=_tmp993;goto _LL3C6;
_LL3C5: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp994=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)
_tmp98F;if(_tmp994->tag != 21)goto _LL3C7;else{_tmp995=_tmp994->f1;}}_LL3C6:
_tmp997=_tmp995;goto _LL3C8;_LL3C7: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*
_tmp996=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp996->tag
!= 14)goto _LL3C9;else{_tmp997=_tmp996->f1;}}_LL3C8: _tmp999=_tmp997;goto _LL3CA;
_LL3C9: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp998=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)
_tmp98F;if(_tmp998->tag != 10)goto _LL3CB;else{_tmp999=_tmp998->f1;}}_LL3CA:
_tmp99B=_tmp999;goto _LL3CC;_LL3CB: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*
_tmp99A=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp98F;if(
_tmp99A->tag != 11)goto _LL3CD;else{_tmp99B=_tmp99A->f1;}}_LL3CC: _tmp99D=_tmp99B;
goto _LL3CE;_LL3CD: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp99C=(
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp99C->tag != 17)
goto _LL3CF;else{_tmp99D=_tmp99C->f1;}}_LL3CE: _tmp99F=_tmp99D;goto _LL3D0;_LL3CF: {
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp99E=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)
_tmp98F;if(_tmp99E->tag != 4)goto _LL3D1;else{_tmp99F=_tmp99E->f1;}}_LL3D0: Cyc_Toc_exptypes_to_c(
_tmp99F);goto _LL3C0;_LL3D1: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*
_tmp9A0=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9A0->tag
!= 2)goto _LL3D3;else{_tmp9A1=_tmp9A0->f2;}}_LL3D2:((void(*)(void(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp9A1);goto _LL3C0;
_LL3D3: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp9A2=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)
_tmp98F;if(_tmp9A2->tag != 6)goto _LL3D5;else{_tmp9A3=_tmp9A2->f1;_tmp9A4=_tmp9A2->f2;}}
_LL3D4: _tmp9A6=_tmp9A3;_tmp9A7=_tmp9A4;goto _LL3D6;_LL3D5: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*
_tmp9A5=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9A5->tag != 7)
goto _LL3D7;else{_tmp9A6=_tmp9A5->f1;_tmp9A7=_tmp9A5->f2;}}_LL3D6: _tmp9A9=_tmp9A6;
_tmp9AA=_tmp9A7;goto _LL3D8;_LL3D7: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*
_tmp9A8=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9A8->tag
!= 8)goto _LL3D9;else{_tmp9A9=_tmp9A8->f1;_tmp9AA=_tmp9A8->f2;}}_LL3D8: _tmp9AC=
_tmp9A9;_tmp9AD=_tmp9AA;goto _LL3DA;_LL3D9: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*
_tmp9AB=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9AB->tag
!= 22)goto _LL3DB;else{_tmp9AC=_tmp9AB->f1;_tmp9AD=_tmp9AB->f2;}}_LL3DA: _tmp9AF=
_tmp9AC;_tmp9B0=_tmp9AD;goto _LL3DC;_LL3DB: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*
_tmp9AE=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9AE->tag != 
33)goto _LL3DD;else{_tmp9AF=_tmp9AE->f1;_tmp9B0=_tmp9AE->f2;}}_LL3DC: _tmp9B2=
_tmp9AF;_tmp9B3=_tmp9B0;goto _LL3DE;_LL3DD: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*
_tmp9B1=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9B1->tag
!= 3)goto _LL3DF;else{_tmp9B2=_tmp9B1->f1;_tmp9B3=_tmp9B1->f3;}}_LL3DE: Cyc_Toc_exptypes_to_c(
_tmp9B2);Cyc_Toc_exptypes_to_c(_tmp9B3);goto _LL3C0;_LL3DF: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*
_tmp9B4=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9B4->tag
!= 5)goto _LL3E1;else{_tmp9B5=_tmp9B4->f1;_tmp9B6=_tmp9B4->f2;_tmp9B7=_tmp9B4->f3;}}
_LL3E0: Cyc_Toc_exptypes_to_c(_tmp9B5);Cyc_Toc_exptypes_to_c(_tmp9B6);Cyc_Toc_exptypes_to_c(
_tmp9B7);goto _LL3C0;_LL3E1: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*
_tmp9B8=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9B8->tag
!= 9)goto _LL3E3;else{_tmp9B9=_tmp9B8->f1;_tmp9BA=_tmp9B8->f2;}}_LL3E2: Cyc_Toc_exptypes_to_c(
_tmp9B9);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp9BA);goto _LL3C0;_LL3E3: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*
_tmp9BB=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9BB->tag != 
13)goto _LL3E5;else{_tmp9BC=(void**)& _tmp9BB->f1;_tmp9BD=(void**)((void**)&
_tmp9BB->f1);_tmp9BE=_tmp9BB->f2;}}_LL3E4:*_tmp9BD=Cyc_Toc_typ_to_c(*_tmp9BD);
Cyc_Toc_exptypes_to_c(_tmp9BE);goto _LL3C0;_LL3E5: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*
_tmp9BF=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9BF->tag
!= 24)goto _LL3E7;else{_tmp9C0=_tmp9BF->f1;_tmp9C1=*_tmp9C0;_tmp9C2=_tmp9C1.f3;
_tmp9C3=(void**)&(*_tmp9BF->f1).f3;_tmp9C4=_tmp9BF->f2;}}_LL3E6:*_tmp9C3=Cyc_Toc_typ_to_c(*
_tmp9C3);_tmp9C6=_tmp9C4;goto _LL3E8;_LL3E7: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*
_tmp9C5=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp98F;if(
_tmp9C5->tag != 34)goto _LL3E9;else{_tmp9C6=_tmp9C5->f2;}}_LL3E8: _tmp9C8=_tmp9C6;
goto _LL3EA;_LL3E9: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9C7=(struct
Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9C7->tag != 25)goto _LL3EB;
else{_tmp9C8=_tmp9C7->f1;}}_LL3EA: for(0;_tmp9C8 != 0;_tmp9C8=_tmp9C8->tl){struct
Cyc_Absyn_Exp*_tmp9E3;struct _tuple16 _tmp9E2=*((struct _tuple16*)_tmp9C8->hd);
_tmp9E3=_tmp9E2.f2;Cyc_Toc_exptypes_to_c(_tmp9E3);}goto _LL3C0;_LL3EB: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*
_tmp9C9=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9C9->tag
!= 18)goto _LL3ED;else{_tmp9CA=(void**)& _tmp9C9->f1;_tmp9CB=(void**)((void**)&
_tmp9C9->f1);}}_LL3EC: _tmp9CE=_tmp9CB;goto _LL3EE;_LL3ED: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*
_tmp9CC=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9CC->tag
!= 16)goto _LL3EF;else{_tmp9CD=(void**)& _tmp9CC->f1;_tmp9CE=(void**)((void**)&
_tmp9CC->f1);}}_LL3EE:*_tmp9CE=Cyc_Toc_typ_to_c(*_tmp9CE);goto _LL3C0;_LL3EF: {
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp9CF=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)
_tmp98F;if(_tmp9CF->tag != 35)goto _LL3F1;else{_tmp9D0=_tmp9CF->f1;}}_LL3F0: Cyc_Toc_stmttypes_to_c(
_tmp9D0);goto _LL3C0;_LL3F1: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*
_tmp9D1=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9D1->tag
!= 32)goto _LL3F3;else{_tmp9D2=_tmp9D1->f1;_tmp9D3=(struct Cyc_Absyn_MallocInfo*)&
_tmp9D1->f1;}}_LL3F2: if(_tmp9D3->elt_type != 0){void**_tmpE18;_tmp9D3->elt_type=((
_tmpE18=_cycalloc(sizeof(*_tmpE18)),((_tmpE18[0]=Cyc_Toc_typ_to_c(*((void**)
_check_null(_tmp9D3->elt_type))),_tmpE18))));}Cyc_Toc_exptypes_to_c(_tmp9D3->num_elts);
goto _LL3C0;_LL3F3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp9D4=(struct
Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9D4->tag != 0)goto _LL3F5;}
_LL3F4: goto _LL3F6;_LL3F5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp9D5=(
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9D5->tag != 1)goto
_LL3F7;}_LL3F6: goto _LL3F8;_LL3F7: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*
_tmp9D6=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9D6->tag != 
30)goto _LL3F9;}_LL3F8: goto _LL3FA;_LL3F9: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*
_tmp9D7=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9D7->tag != 
38)goto _LL3FB;}_LL3FA: goto _LL3FC;_LL3FB: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*
_tmp9D8=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9D8->tag
!= 31)goto _LL3FD;}_LL3FC: goto _LL3C0;_LL3FD: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*
_tmp9D9=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9D9->tag
!= 28)goto _LL3FF;}_LL3FE: goto _LL400;_LL3FF: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*
_tmp9DA=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9DA->tag
!= 29)goto _LL401;}_LL400: goto _LL402;_LL401: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*
_tmp9DB=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9DB->tag
!= 27)goto _LL403;}_LL402: goto _LL404;_LL403: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*
_tmp9DC=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp98F;if(
_tmp9DC->tag != 26)goto _LL405;}_LL404: goto _LL406;_LL405: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*
_tmp9DD=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9DD->tag != 
23)goto _LL407;}_LL406: goto _LL408;_LL407: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*
_tmp9DE=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9DE->tag
!= 12)goto _LL409;}_LL408: goto _LL40A;_LL409: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*
_tmp9DF=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9DF->tag != 
15)goto _LL40B;}_LL40A: goto _LL40C;_LL40B: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*
_tmp9E0=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9E0->tag
!= 37)goto _LL40D;}_LL40C: goto _LL40E;_LL40D: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*
_tmp9E1=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp98F;if(_tmp9E1->tag
!= 36)goto _LL3C0;}_LL40E:{const char*_tmpE1B;void*_tmpE1A;(_tmpE1A=0,Cyc_Tcutil_terr(
e->loc,((_tmpE1B="Cyclone expression within C code",_tag_dyneither(_tmpE1B,
sizeof(char),33))),_tag_dyneither(_tmpE1A,sizeof(void*),0)));}goto _LL3C0;_LL3C0:;}
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){void*_tmp9E7=d->r;
struct Cyc_Absyn_Vardecl*_tmp9E9;struct Cyc_Absyn_Fndecl*_tmp9EB;struct Cyc_Absyn_Aggrdecl*
_tmp9ED;struct Cyc_Absyn_Enumdecl*_tmp9EF;struct Cyc_Absyn_Typedefdecl*_tmp9F1;
_LL410: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9E8=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)
_tmp9E7;if(_tmp9E8->tag != 0)goto _LL412;else{_tmp9E9=_tmp9E8->f1;}}_LL411: _tmp9E9->type=
Cyc_Toc_typ_to_c(_tmp9E9->type);if(_tmp9E9->initializer != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmp9E9->initializer));goto _LL40F;_LL412: {
struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp9EA=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)
_tmp9E7;if(_tmp9EA->tag != 1)goto _LL414;else{_tmp9EB=_tmp9EA->f1;}}_LL413: _tmp9EB->ret_type=
Cyc_Toc_typ_to_c(_tmp9EB->ret_type);{struct Cyc_List_List*_tmp9FD=_tmp9EB->args;
for(0;_tmp9FD != 0;_tmp9FD=_tmp9FD->tl){(*((struct _tuple8*)_tmp9FD->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple8*)_tmp9FD->hd)).f3);}}goto _LL40F;_LL414: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*
_tmp9EC=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp9E7;if(_tmp9EC->tag != 
6)goto _LL416;else{_tmp9ED=_tmp9EC->f1;}}_LL415: Cyc_Toc_aggrdecl_to_c(_tmp9ED,1);
goto _LL40F;_LL416: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9EE=(struct
Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp9E7;if(_tmp9EE->tag != 8)goto _LL418;
else{_tmp9EF=_tmp9EE->f1;}}_LL417: if(_tmp9EF->fields != 0){struct Cyc_List_List*
_tmp9FE=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp9EF->fields))->v;
for(0;_tmp9FE != 0;_tmp9FE=_tmp9FE->tl){struct Cyc_Absyn_Enumfield*_tmp9FF=(struct
Cyc_Absyn_Enumfield*)_tmp9FE->hd;if(_tmp9FF->tag != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmp9FF->tag));}}goto _LL40F;_LL418: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*
_tmp9F0=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp9E7;if(_tmp9F0->tag
!= 9)goto _LL41A;else{_tmp9F1=_tmp9F0->f1;}}_LL419: _tmp9F1->defn=(void*)Cyc_Toc_typ_to_c_array((
void*)_check_null(_tmp9F1->defn));goto _LL40F;_LL41A: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*
_tmp9F2=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp9E7;if(_tmp9F2->tag != 
2)goto _LL41C;}_LL41B: goto _LL41D;_LL41C: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*
_tmp9F3=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp9E7;if(_tmp9F3->tag != 
3)goto _LL41E;}_LL41D: goto _LL41F;_LL41E: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*
_tmp9F4=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp9E7;if(_tmp9F4->tag
!= 7)goto _LL420;}_LL41F: goto _LL421;_LL420: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*
_tmp9F5=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp9E7;if(_tmp9F5->tag
!= 10)goto _LL422;}_LL421: goto _LL423;_LL422: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*
_tmp9F6=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp9E7;if(_tmp9F6->tag
!= 11)goto _LL424;}_LL423: goto _LL425;_LL424: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*
_tmp9F7=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp9E7;if(_tmp9F7->tag
!= 12)goto _LL426;}_LL425: goto _LL427;_LL426: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*
_tmp9F8=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp9E7;if(
_tmp9F8->tag != 13)goto _LL428;}_LL427: goto _LL429;_LL428: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*
_tmp9F9=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp9E7;if(_tmp9F9->tag
!= 4)goto _LL42A;}_LL429: goto _LL42B;_LL42A: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*
_tmp9FA=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp9E7;if(_tmp9FA->tag
!= 5)goto _LL42C;}_LL42B:{const char*_tmpE1E;void*_tmpE1D;(_tmpE1D=0,Cyc_Tcutil_terr(
d->loc,((_tmpE1E="Cyclone declaration within C code",_tag_dyneither(_tmpE1E,
sizeof(char),34))),_tag_dyneither(_tmpE1D,sizeof(void*),0)));}goto _LL40F;_LL42C: {
struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp9FB=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)
_tmp9E7;if(_tmp9FB->tag != 14)goto _LL42E;}_LL42D: goto _LL42F;_LL42E: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*
_tmp9FC=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp9E7;if(_tmp9FC->tag
!= 15)goto _LL40F;}_LL42F: goto _LL40F;_LL40F:;}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s){void*_tmpA02=s->r;struct Cyc_Absyn_Exp*_tmpA04;struct Cyc_Absyn_Stmt*
_tmpA06;struct Cyc_Absyn_Stmt*_tmpA07;struct Cyc_Absyn_Exp*_tmpA09;struct Cyc_Absyn_Exp*
_tmpA0B;struct Cyc_Absyn_Stmt*_tmpA0C;struct Cyc_Absyn_Stmt*_tmpA0D;struct _tuple9
_tmpA0F;struct Cyc_Absyn_Exp*_tmpA10;struct Cyc_Absyn_Stmt*_tmpA11;struct Cyc_Absyn_Exp*
_tmpA13;struct _tuple9 _tmpA14;struct Cyc_Absyn_Exp*_tmpA15;struct _tuple9 _tmpA16;
struct Cyc_Absyn_Exp*_tmpA17;struct Cyc_Absyn_Stmt*_tmpA18;struct Cyc_Absyn_Exp*
_tmpA1A;struct Cyc_List_List*_tmpA1B;struct Cyc_Absyn_Decl*_tmpA1D;struct Cyc_Absyn_Stmt*
_tmpA1E;struct Cyc_Absyn_Stmt*_tmpA20;struct _tuple9 _tmpA21;struct Cyc_Absyn_Exp*
_tmpA22;_LL431: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpA03=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)
_tmpA02;if(_tmpA03->tag != 1)goto _LL433;else{_tmpA04=_tmpA03->f1;}}_LL432: Cyc_Toc_exptypes_to_c(
_tmpA04);goto _LL430;_LL433: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpA05=(
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA02;if(_tmpA05->tag != 2)goto
_LL435;else{_tmpA06=_tmpA05->f1;_tmpA07=_tmpA05->f2;}}_LL434: Cyc_Toc_stmttypes_to_c(
_tmpA06);Cyc_Toc_stmttypes_to_c(_tmpA07);goto _LL430;_LL435: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*
_tmpA08=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmpA02;if(_tmpA08->tag
!= 3)goto _LL437;else{_tmpA09=_tmpA08->f1;}}_LL436: if(_tmpA09 != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_tmpA09);goto _LL430;_LL437: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*
_tmpA0A=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpA02;if(_tmpA0A->tag
!= 4)goto _LL439;else{_tmpA0B=_tmpA0A->f1;_tmpA0C=_tmpA0A->f2;_tmpA0D=_tmpA0A->f3;}}
_LL438: Cyc_Toc_exptypes_to_c(_tmpA0B);Cyc_Toc_stmttypes_to_c(_tmpA0C);Cyc_Toc_stmttypes_to_c(
_tmpA0D);goto _LL430;_LL439: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*
_tmpA0E=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmpA02;if(_tmpA0E->tag
!= 5)goto _LL43B;else{_tmpA0F=_tmpA0E->f1;_tmpA10=_tmpA0F.f1;_tmpA11=_tmpA0E->f2;}}
_LL43A: Cyc_Toc_exptypes_to_c(_tmpA10);Cyc_Toc_stmttypes_to_c(_tmpA11);goto _LL430;
_LL43B: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmpA12=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)
_tmpA02;if(_tmpA12->tag != 9)goto _LL43D;else{_tmpA13=_tmpA12->f1;_tmpA14=_tmpA12->f2;
_tmpA15=_tmpA14.f1;_tmpA16=_tmpA12->f3;_tmpA17=_tmpA16.f1;_tmpA18=_tmpA12->f4;}}
_LL43C: Cyc_Toc_exptypes_to_c(_tmpA13);Cyc_Toc_exptypes_to_c(_tmpA15);Cyc_Toc_exptypes_to_c(
_tmpA17);Cyc_Toc_stmttypes_to_c(_tmpA18);goto _LL430;_LL43D: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*
_tmpA19=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmpA02;if(_tmpA19->tag
!= 10)goto _LL43F;else{_tmpA1A=_tmpA19->f1;_tmpA1B=_tmpA19->f2;}}_LL43E: Cyc_Toc_exptypes_to_c(
_tmpA1A);for(0;_tmpA1B != 0;_tmpA1B=_tmpA1B->tl){Cyc_Toc_stmttypes_to_c(((struct
Cyc_Absyn_Switch_clause*)_tmpA1B->hd)->body);}goto _LL430;_LL43F: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*
_tmpA1C=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA02;if(_tmpA1C->tag != 
12)goto _LL441;else{_tmpA1D=_tmpA1C->f1;_tmpA1E=_tmpA1C->f2;}}_LL440: Cyc_Toc_decltypes_to_c(
_tmpA1D);Cyc_Toc_stmttypes_to_c(_tmpA1E);goto _LL430;_LL441: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*
_tmpA1F=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmpA02;if(_tmpA1F->tag != 
14)goto _LL443;else{_tmpA20=_tmpA1F->f1;_tmpA21=_tmpA1F->f2;_tmpA22=_tmpA21.f1;}}
_LL442: Cyc_Toc_stmttypes_to_c(_tmpA20);Cyc_Toc_exptypes_to_c(_tmpA22);goto _LL430;
_LL443: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmpA23=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)
_tmpA02;if(_tmpA23->tag != 0)goto _LL445;}_LL444: goto _LL446;_LL445: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*
_tmpA24=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmpA02;if(_tmpA24->tag
!= 6)goto _LL447;}_LL446: goto _LL448;_LL447: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*
_tmpA25=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmpA02;if(_tmpA25->tag
!= 7)goto _LL449;}_LL448: goto _LL44A;_LL449: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*
_tmpA26=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmpA02;if(_tmpA26->tag != 
8)goto _LL44B;}_LL44A: goto _LL430;_LL44B: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*
_tmpA27=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmpA02;if(_tmpA27->tag
!= 11)goto _LL44D;}_LL44C: goto _LL44E;_LL44D: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*
_tmpA28=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmpA02;if(_tmpA28->tag
!= 13)goto _LL44F;}_LL44E: goto _LL450;_LL44F: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*
_tmpA29=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmpA02;if(_tmpA29->tag
!= 15)goto _LL451;}_LL450: goto _LL452;_LL451: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*
_tmpA2A=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmpA02;if(_tmpA2A->tag
!= 16)goto _LL430;}_LL452:{const char*_tmpE21;void*_tmpE20;(_tmpE20=0,Cyc_Tcutil_terr(
s->loc,((_tmpE21="Cyclone statement in C code",_tag_dyneither(_tmpE21,sizeof(
char),28))),_tag_dyneither(_tmpE20,sizeof(void*),0)));}goto _LL430;_LL430:;}
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*
nv,struct Cyc_List_List*ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(
nv)){const char*_tmpE24;void*_tmpE23;(_tmpE23=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE24="decls_to_c: not at toplevel!",
_tag_dyneither(_tmpE24,sizeof(char),29))),_tag_dyneither(_tmpE23,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmpA2F=d->r;struct Cyc_Absyn_Vardecl*
_tmpA31;struct Cyc_Absyn_Fndecl*_tmpA33;struct Cyc_Absyn_Aggrdecl*_tmpA39;struct
Cyc_Absyn_Datatypedecl*_tmpA3B;struct Cyc_Absyn_Enumdecl*_tmpA3D;struct Cyc_Absyn_Typedefdecl*
_tmpA3F;struct Cyc_List_List*_tmpA43;struct Cyc_List_List*_tmpA45;struct Cyc_List_List*
_tmpA47;struct Cyc_List_List*_tmpA49;_LL454: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*
_tmpA30=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA2F;if(_tmpA30->tag != 
0)goto _LL456;else{_tmpA31=_tmpA30->f1;}}_LL455: {struct _tuple0*_tmpA4A=_tmpA31->name;
if(_tmpA31->sc == Cyc_Absyn_ExternC){struct _tuple0*_tmpE25;_tmpA4A=((_tmpE25=
_cycalloc(sizeof(*_tmpE25)),((_tmpE25->f1=Cyc_Absyn_Rel_n(0),((_tmpE25->f2=(*
_tmpA4A).f2,_tmpE25))))));}if(_tmpA31->initializer != 0){if(cinclude)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmpA31->initializer));else{Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_check_null(_tmpA31->initializer));}}{struct Cyc_Absyn_Global_b_Absyn_Binding_struct
_tmpE28;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE27;nv=Cyc_Toc_add_varmap(
r,nv,_tmpA31->name,Cyc_Absyn_varb_exp(_tmpA4A,(void*)((_tmpE27=_cycalloc(sizeof(*
_tmpE27)),((_tmpE27[0]=((_tmpE28.tag=1,((_tmpE28.f1=_tmpA31,_tmpE28)))),_tmpE27)))),
0));}_tmpA31->name=_tmpA4A;_tmpA31->sc=Cyc_Toc_scope_to_c(_tmpA31->sc);_tmpA31->type=
Cyc_Toc_typ_to_c_array(_tmpA31->type);{struct Cyc_List_List*_tmpE29;Cyc_Toc_result_decls=((
_tmpE29=_cycalloc(sizeof(*_tmpE29)),((_tmpE29->hd=d,((_tmpE29->tl=Cyc_Toc_result_decls,
_tmpE29))))));}goto _LL453;}_LL456: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*
_tmpA32=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpA2F;if(_tmpA32->tag != 1)
goto _LL458;else{_tmpA33=_tmpA32->f1;}}_LL457: {struct _tuple0*_tmpA4F=_tmpA33->name;
if(_tmpA33->sc == Cyc_Absyn_ExternC){{struct _tuple0*_tmpE2A;_tmpA4F=((_tmpE2A=
_cycalloc(sizeof(*_tmpE2A)),((_tmpE2A->f1=Cyc_Absyn_Rel_n(0),((_tmpE2A->f2=(*
_tmpA4F).f2,_tmpE2A))))));}_tmpA33->sc=Cyc_Absyn_Public;}nv=Cyc_Toc_add_varmap(r,
nv,_tmpA33->name,Cyc_Absyn_var_exp(_tmpA4F,0));_tmpA33->name=_tmpA4F;Cyc_Toc_fndecl_to_c(
nv,_tmpA33,cinclude);{struct Cyc_List_List*_tmpE2B;Cyc_Toc_result_decls=((_tmpE2B=
_cycalloc(sizeof(*_tmpE2B)),((_tmpE2B->hd=d,((_tmpE2B->tl=Cyc_Toc_result_decls,
_tmpE2B))))));}goto _LL453;}_LL458: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*
_tmpA34=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA2F;if(_tmpA34->tag != 
2)goto _LL45A;}_LL459: goto _LL45B;_LL45A: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*
_tmpA35=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpA2F;if(_tmpA35->tag != 
3)goto _LL45C;}_LL45B: {const char*_tmpE2E;void*_tmpE2D;(_tmpE2D=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE2E="letdecl at toplevel",
_tag_dyneither(_tmpE2E,sizeof(char),20))),_tag_dyneither(_tmpE2D,sizeof(void*),0)));}
_LL45C: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmpA36=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)
_tmpA2F;if(_tmpA36->tag != 4)goto _LL45E;}_LL45D: {const char*_tmpE31;void*_tmpE30;(
_tmpE30=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE31="region decl at toplevel",_tag_dyneither(_tmpE31,sizeof(char),24))),
_tag_dyneither(_tmpE30,sizeof(void*),0)));}_LL45E: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*
_tmpA37=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmpA2F;if(_tmpA37->tag
!= 5)goto _LL460;}_LL45F: {const char*_tmpE34;void*_tmpE33;(_tmpE33=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE34="alias decl at toplevel",
_tag_dyneither(_tmpE34,sizeof(char),23))),_tag_dyneither(_tmpE33,sizeof(void*),0)));}
_LL460: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA38=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)
_tmpA2F;if(_tmpA38->tag != 6)goto _LL462;else{_tmpA39=_tmpA38->f1;}}_LL461: Cyc_Toc_aggrdecl_to_c(
_tmpA39,1);goto _LL453;_LL462: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*
_tmpA3A=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmpA2F;if(_tmpA3A->tag
!= 7)goto _LL464;else{_tmpA3B=_tmpA3A->f1;}}_LL463: if(_tmpA3B->is_extensible)Cyc_Toc_xdatatypedecl_to_c(
_tmpA3B);else{Cyc_Toc_datatypedecl_to_c(_tmpA3B);}goto _LL453;_LL464: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*
_tmpA3C=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpA2F;if(_tmpA3C->tag != 
8)goto _LL466;else{_tmpA3D=_tmpA3C->f1;}}_LL465: Cyc_Toc_enumdecl_to_c(_tmpA3D);{
struct Cyc_List_List*_tmpE35;Cyc_Toc_result_decls=((_tmpE35=_cycalloc(sizeof(*
_tmpE35)),((_tmpE35->hd=d,((_tmpE35->tl=Cyc_Toc_result_decls,_tmpE35))))));}goto
_LL453;_LL466: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpA3E=(struct
Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpA2F;if(_tmpA3E->tag != 9)goto
_LL468;else{_tmpA3F=_tmpA3E->f1;}}_LL467: _tmpA3F->name=_tmpA3F->name;_tmpA3F->tvs=
0;if(_tmpA3F->defn != 0)_tmpA3F->defn=(void*)Cyc_Toc_typ_to_c_array((void*)
_check_null(_tmpA3F->defn));else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)
_check_null(_tmpA3F->kind))->v)->kind){case Cyc_Absyn_BoxKind: _LL474: _tmpA3F->defn=(
void*)Cyc_Absyn_void_star_typ();break;default: _LL475: _tmpA3F->defn=(void*)& Cyc_Absyn_VoidType_val;
break;}}{struct Cyc_List_List*_tmpE36;Cyc_Toc_result_decls=((_tmpE36=_cycalloc(
sizeof(*_tmpE36)),((_tmpE36->hd=d,((_tmpE36->tl=Cyc_Toc_result_decls,_tmpE36))))));}
goto _LL453;_LL468: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmpA40=(
struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmpA2F;if(_tmpA40->tag != 14)
goto _LL46A;}_LL469: goto _LL46B;_LL46A: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*
_tmpA41=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmpA2F;if(_tmpA41->tag
!= 15)goto _LL46C;}_LL46B: goto _LL453;_LL46C: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*
_tmpA42=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA2F;if(_tmpA42->tag
!= 10)goto _LL46E;else{_tmpA43=_tmpA42->f2;}}_LL46D: _tmpA45=_tmpA43;goto _LL46F;
_LL46E: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA44=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)
_tmpA2F;if(_tmpA44->tag != 11)goto _LL470;else{_tmpA45=_tmpA44->f2;}}_LL46F:
_tmpA47=_tmpA45;goto _LL471;_LL470: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*
_tmpA46=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmpA2F;if(_tmpA46->tag
!= 12)goto _LL472;else{_tmpA47=_tmpA46->f1;}}_LL471: nv=Cyc_Toc_decls_to_c(r,nv,
_tmpA47,top,cinclude);goto _LL453;_LL472: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*
_tmpA48=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmpA2F;if(
_tmpA48->tag != 13)goto _LL453;else{_tmpA49=_tmpA48->f1;}}_LL473: nv=Cyc_Toc_decls_to_c(
r,nv,_tmpA49,top,1);goto _LL453;_LL453:;};}return nv;}static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmpA5B;struct Cyc_Core_NewRegion _tmpA5A=Cyc_Core__new_dynregion((
const char*)"internal-error",0);_tmpA5B=_tmpA5A.dynregion;{struct _DynRegionFrame
_tmpA5C;struct _RegionHandle*d=_open_dynregion(& _tmpA5C,_tmpA5B);{struct Cyc_Dict_Dict*
_tmpE43;struct Cyc_Dict_Dict*_tmpE42;struct Cyc_Set_Set**_tmpE41;struct Cyc_List_List**
_tmpE40;struct Cyc_Dict_Dict*_tmpE3F;struct Cyc_List_List**_tmpE3E;struct Cyc_Toc_TocState*
_tmpE3D;Cyc_Toc_toc_state=((_tmpE3D=_cycalloc(sizeof(*_tmpE3D)),((_tmpE3D->dyn=(
struct _DynRegionHandle*)_tmpA5B,((_tmpE3D->tuple_types=(struct Cyc_List_List**)((
_tmpE3E=_region_malloc(d,sizeof(*_tmpE3E)),((_tmpE3E[0]=0,_tmpE3E)))),((_tmpE3D->aggrs_so_far=(
struct Cyc_Dict_Dict*)((_tmpE3F=_region_malloc(d,sizeof(*_tmpE3F)),((_tmpE3F[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE3F)))),((_tmpE3D->abs_struct_types=(
struct Cyc_List_List**)((_tmpE40=_region_malloc(d,sizeof(*_tmpE40)),((_tmpE40[0]=
0,_tmpE40)))),((_tmpE3D->datatypes_so_far=(struct Cyc_Set_Set**)((_tmpE41=
_region_malloc(d,sizeof(*_tmpE41)),((_tmpE41[0]=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE41)))),((_tmpE3D->xdatatypes_so_far=(struct Cyc_Dict_Dict*)((_tmpE42=
_region_malloc(d,sizeof(*_tmpE42)),((_tmpE42[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE42)))),((_tmpE3D->qvar_tags=(struct Cyc_Dict_Dict*)((_tmpE43=_region_malloc(
d,sizeof(*_tmpE43)),((_tmpE43[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,
int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),
_tmpE43)))),((_tmpE3D->temp_labels=(struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(
struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),(struct Cyc_Toc_TocState*)
_tmpE3D))))))))))))))))));}Cyc_Toc_result_decls=0;Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_total_bounds_checks=
0;Cyc_Toc_bounds_checks_eliminated=0;{struct _dyneither_ptr**_tmpE44;Cyc_Toc_globals=
_tag_dyneither(((_tmpE44=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpE44[
0]=& Cyc_Toc__throw_str,((_tmpE44[1]=& Cyc_Toc_setjmp_str,((_tmpE44[2]=& Cyc_Toc__push_handler_str,((
_tmpE44[3]=& Cyc_Toc__pop_handler_str,((_tmpE44[4]=& Cyc_Toc__exn_thrown_str,((
_tmpE44[5]=& Cyc_Toc__npop_handler_str,((_tmpE44[6]=& Cyc_Toc__check_null_str,((
_tmpE44[7]=& Cyc_Toc__check_known_subscript_null_str,((_tmpE44[8]=& Cyc_Toc__check_known_subscript_notnull_str,((
_tmpE44[9]=& Cyc_Toc__check_dyneither_subscript_str,((_tmpE44[10]=& Cyc_Toc__dyneither_ptr_str,((
_tmpE44[11]=& Cyc_Toc__tag_dyneither_str,((_tmpE44[12]=& Cyc_Toc__init_dyneither_ptr_str,((
_tmpE44[13]=& Cyc_Toc__untag_dyneither_ptr_str,((_tmpE44[14]=& Cyc_Toc__get_dyneither_size_str,((
_tmpE44[15]=& Cyc_Toc__get_zero_arr_size_str,((_tmpE44[16]=& Cyc_Toc__dyneither_ptr_plus_str,((
_tmpE44[17]=& Cyc_Toc__zero_arr_plus_str,((_tmpE44[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((
_tmpE44[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpE44[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((
_tmpE44[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpE44[22]=& Cyc_Toc__cycalloc_str,((
_tmpE44[23]=& Cyc_Toc__cyccalloc_str,((_tmpE44[24]=& Cyc_Toc__cycalloc_atomic_str,((
_tmpE44[25]=& Cyc_Toc__cyccalloc_atomic_str,((_tmpE44[26]=& Cyc_Toc__region_malloc_str,((
_tmpE44[27]=& Cyc_Toc__region_calloc_str,((_tmpE44[28]=& Cyc_Toc__check_times_str,((
_tmpE44[29]=& Cyc_Toc__new_region_str,((_tmpE44[30]=& Cyc_Toc__push_region_str,((
_tmpE44[31]=& Cyc_Toc__pop_region_str,((_tmpE44[32]=& Cyc_Toc__open_dynregion_str,((
_tmpE44[33]=& Cyc_Toc__push_dynregion_str,((_tmpE44[34]=& Cyc_Toc__pop_dynregion_str,((
_tmpE44[35]=& Cyc_Toc__reset_region_str,((_tmpE44[36]=& Cyc_Toc__throw_arraybounds_str,((
_tmpE44[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpE44[38]=& Cyc_Toc__throw_match_str,((
_tmpE44[39]=& Cyc_Toc__swap_word_str,((_tmpE44[40]=& Cyc_Toc__swap_dyneither_str,
_tmpE44)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),41);};;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();{struct _RegionHandle _tmpA65=
_new_region("start");struct _RegionHandle*start=& _tmpA65;_push_region(start);Cyc_Toc_decls_to_c(
start,Cyc_Toc_empty_env(start),ds,1,0);{struct _DynRegionHandle*_tmpA67;struct Cyc_Toc_TocState
_tmpA66=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA67=
_tmpA66.dyn;Cyc_Core_free_dynregion(_tmpA67);};{struct Cyc_List_List*_tmpA68=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);
_npop_handler(0);return _tmpA68;};;_pop_region(start);};}
