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
/*#define _zero_arr_inplace_plus(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  *_zap_x = _zero_arr_plus(*_zap_x,1,(orig_i)); })
  */
static _INLINE void 
_zero_arr_inplace_plus_char(char *x, int orig_i) {
  char **_zap_x = &x;
  *_zap_x = _zero_arr_plus_char(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_short(short *x, int orig_i) {
  short **_zap_x = &x;
  *_zap_x = _zero_arr_plus_short(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_int(int *x, int orig_i) {
  int **_zap_x = &x;
  *_zap_x = _zero_arr_plus_int(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_float(float *x, int orig_i) {
  float **_zap_x = &x;
  *_zap_x = _zero_arr_plus_float(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_double(double *x, int orig_i) {
  double **_zap_x = &x;
  *_zap_x = _zero_arr_plus_double(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_longdouble(long double *x, int orig_i) {
  long double **_zap_x = &x;
  *_zap_x = _zero_arr_plus_longdouble(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_voidstar(void **x, int orig_i) {
  void ***_zap_x = &x;
  *_zap_x = _zero_arr_plus_voidstar(*_zap_x,1,orig_i);
}




/* Does in-place increment of a zero-terminated pointer (e.g., x++).
   Note that this expands to call _zero_arr_plus. */
/*#define _zero_arr_inplace_plus_post(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  _zap_tx *_zap_res = *_zap_x; \
  *_zap_x = _zero_arr_plus(_zap_res,1,(orig_i)); \
  _zap_res; })*/
  
static _INLINE char *
_zero_arr_inplace_plus_post_char(char *x, int orig_i){
  char ** _zap_x = &x;
  char * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_char(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE short *
_zero_arr_inplace_plus_post_short(short *x, int orig_i){
  short **_zap_x = &x;
  short * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_short(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE int *
_zero_arr_inplace_plus_post_int(int *x, int orig_i){
  int **_zap_x = &x;
  int * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_int(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE float *
_zero_arr_inplace_plus_post_float(float *x, int orig_i){
  float **_zap_x = &x;
  float * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_float(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE double *
_zero_arr_inplace_plus_post_double(double *x, int orig_i){
  double **_zap_x = &x;
  double * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_double(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE long double *
_zero_arr_inplace_plus_post_longdouble(long double *x, int orig_i){
  long double **_zap_x = &x;
  long double * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_longdouble(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE void **
_zero_arr_inplace_plus_post_voidstar(void **x, int orig_i){
  void ***_zap_x = &x;
  void ** _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_voidstar(_zap_res,1,orig_i);
  return _zap_res;
}



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
static _INLINE unsigned char *
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
static _INLINE unsigned char *
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
static _INLINE unsigned char *
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
static _INLINE struct _dyneither_ptr
_tag_dyneither(const void *tcurr,unsigned elt_sz,unsigned num_elts) {
  struct _dyneither_ptr _tag_arr_ans;
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr);
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts);
  return _tag_arr_ans;
}
static _INLINE struct _dynforward_ptr
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
static _INLINE struct _dyneither_ptr *
_init_dyneither_ptr(struct _dyneither_ptr *arr_ptr,
                    void *arr, unsigned elt_sz, unsigned num_elts) {
  struct _dyneither_ptr *_itarr_ptr = (arr_ptr);
  void* _itarr = (arr);
  _itarr_ptr->base = _itarr_ptr->curr = _itarr;
  _itarr_ptr->last_plus_one = ((unsigned char *)_itarr) + (elt_sz) * (num_elts);
  return _itarr_ptr;
}
static _INLINE struct _dynforward_ptr *
_init_dynforward_ptr(struct _dynforward_ptr *arr_ptr,
                    void *arr, unsigned elt_sz, unsigned num_elts) {
  struct _dynforward_ptr *_itarr_ptr = (arr_ptr);
  void* _itarr = (arr);
  _itarr_ptr->curr = _itarr;
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
static _INLINE unsigned char *
_untag_dyneither_ptr(struct _dyneither_ptr arr, 
                     unsigned elt_sz,unsigned num_elts) {
  struct _dyneither_ptr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)
    _throw_arraybounds();
  return _curr;
}
static _INLINE unsigned char *
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
static _INLINE unsigned
_get_dyneither_size(struct _dyneither_ptr arr,unsigned elt_sz) {
  struct _dyneither_ptr _get_arr_size_temp = (arr);
  unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr;
  unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one;
  return (_get_arr_size_curr < _get_arr_size_temp.base ||
          _get_arr_size_curr >= _get_arr_size_last) ? 0 :
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));
}
static _INLINE unsigned
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
static _INLINE struct _dyneither_ptr
_dyneither_ptr_plus(struct _dyneither_ptr arr,unsigned elt_sz,int change) {
  struct _dyneither_ptr _ans = (arr);
  _ans.curr += ((int)(elt_sz))*(change);
  return _ans;
}
/* Here we have to worry about wrapping around, so if we go past the
 * end, we set the end to 0. */
static _INLINE struct _dynforward_ptr
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
static _INLINE struct _dyneither_ptr
_dyneither_ptr_inplace_plus(struct _dyneither_ptr *arr_ptr,unsigned elt_sz,
                            int change) {
  struct _dyneither_ptr * _arr_ptr = (arr_ptr);
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return *_arr_ptr;
}
static _INLINE struct _dynforward_ptr
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
static _INLINE struct _dyneither_ptr
_dyneither_ptr_inplace_plus_post(struct _dyneither_ptr *arr_ptr,unsigned elt_sz,int change) {
  struct _dyneither_ptr * _arr_ptr = (arr_ptr);
  struct _dyneither_ptr _ans = *_arr_ptr;
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return _ans;
}
static _INLINE struct _dynforward_ptr
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

/* the next three routines swap [x] and [y]; not thread safe! */
static _INLINE void _swap_word(void *x, void *y) {
  unsigned long *lx = (unsigned long *)x, *ly = (unsigned long *)y, tmp;
  tmp = *lx;
  *lx = *ly;
  *ly = tmp;
}
static _INLINE void _swap_dynforward(struct _dynforward_ptr *x, 
				    struct _dynforward_ptr *y) {
  struct _dynforward_ptr tmp = *x;
  *x = *y;
  *y = tmp;
}
static _INLINE void _swap_dyneither(struct _dyneither_ptr *x, 
				   struct _dyneither_ptr *y) {
  struct _dyneither_ptr tmp = *x;
  *x = *y;
  *y = tmp;
}
 struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};struct Cyc_Core_Opt{
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);int
Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[
16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct
_dynforward_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*,int(*cmp)(void*,void*),
struct Cyc_List_List*a,struct Cyc_List_List*b);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_rzip(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc___cycFILE;extern struct
Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,
struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,
void**ans);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict
d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,
void*);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict
d);int Cyc_strptrcmp(struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct Cyc_Hashtable_Table;
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*
cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*
t,void*key,void*val);void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,
void*key);struct Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct
_dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _dynforward_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];
int Cyc_Position_error_p();struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
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
Cyc_Absyn_Tvar{struct _dynforward_ptr*name;int identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{
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
};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_TagType_struct{
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
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
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
f2;int f3;struct Cyc_Absyn_Exp*f4;struct Cyc_Absyn_Stmt*f5;};struct Cyc_Absyn_ResetRegion_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Alias_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;struct Cyc_Absyn_Stmt*f4;};
struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*loc;struct Cyc_List_List*
non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Var_p_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
info);int Cyc_Absyn_is_union_type(void*);int Cyc_Absyn_is_aggr_type(void*t);struct
Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct
_RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct
Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
loc);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int
Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*
r);int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*
constraints,void*default_bound,int do_pin);int Cyc_RgnOrder_eff_outlives_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);void Cyc_RgnOrder_print_region_po(
struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*
tl;};struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;};void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_compress(void*t);
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_is_noalias_path(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);int Cyc_Tcutil_is_noreturn(
void*);struct _tuple4{unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};union Cyc_CfFlowInfo_RelnOp_union{struct Cyc_CfFlowInfo_EqualConst_struct
EqualConst;struct Cyc_CfFlowInfo_LessVar_struct LessVar;struct Cyc_CfFlowInfo_LessSize_struct
LessSize;struct Cyc_CfFlowInfo_LessConst_struct LessConst;struct Cyc_CfFlowInfo_LessEqSize_struct
LessEqSize;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp_union
rop;};struct Cyc_CfFlowInfo_TagCmp{void*cmp;void*bd;};extern char Cyc_CfFlowInfo_HasTagCmps[
15];struct Cyc_CfFlowInfo_HasTagCmps_struct{char*tag;struct Cyc_List_List*f1;};
extern char Cyc_CfFlowInfo_IsZero[11];extern char Cyc_CfFlowInfo_NotZero[12];struct
Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[
13];struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};
struct Cyc_List_List*Cyc_CfFlowInfo_copy_tagcmps(struct _RegionHandle*,struct Cyc_List_List*);
struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct
Cyc_CfFlowInfo_UnknownL_struct{int tag;};union Cyc_CfFlowInfo_AbsLVal_union{struct
Cyc_CfFlowInfo_PlaceL_struct PlaceL;struct Cyc_CfFlowInfo_UnknownL_struct UnknownL;
};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct _dynforward_ptr f1;};int Cyc_CfFlowInfo_update_place_set(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,
struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict
consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(
struct _RegionHandle*,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2);struct Cyc_CfFlowInfo_BottomFL_struct{int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{
int tag;struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo
f3;};union Cyc_CfFlowInfo_FlowInfo_union{struct Cyc_CfFlowInfo_BottomFL_struct
BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{
struct _RegionHandle*r;void*unknown_none;void*unknown_this;void*unknown_all;void*
esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict
mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*
Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);struct _dynforward_ptr
Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,
void*);void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,
void*leafval);void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict d,void*r);void*
Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo_union
f1,union Cyc_CfFlowInfo_FlowInfo_union f2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct
Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);struct
Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_join_flow(struct
Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo_union,
union Cyc_CfFlowInfo_FlowInfo_union,int);struct _tuple5{union Cyc_CfFlowInfo_FlowInfo_union
f1;void*f2;};struct _tuple5 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict*all_changed,struct _tuple5 pr1,struct _tuple5 pr2,int);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,
union Cyc_CfFlowInfo_FlowInfo_union,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Dict_Dict,
struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union f,void*rgn);
struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_readthrough_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);struct _tuple6{struct Cyc_CfFlowInfo_ConsumeInfo f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};struct _tuple6 Cyc_CfFlowInfo_save_consume_info(struct Cyc_CfFlowInfo_FlowEnv*,
union Cyc_CfFlowInfo_FlowInfo_union f,int clear);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_restore_consume_info(union Cyc_CfFlowInfo_FlowInfo_union f,struct
Cyc_CfFlowInfo_ConsumeInfo c);struct _dynforward_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place);void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*
enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_cf_check(struct
Cyc_List_List*ds);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_all_effects: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct
_dynforward_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt*encloser;int visited;};static char Cyc_NewControlFlow_CFAnnot[
12]="\000\000\000\000CFAnnot\000";struct Cyc_NewControlFlow_CFAnnot_struct{char*
tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){(void*)(enclosee->annot=(
void*)((void*)({struct Cyc_NewControlFlow_CFAnnot_struct*_tmp0=_cycalloc(sizeof(*
_tmp0));_tmp0[0]=({struct Cyc_NewControlFlow_CFAnnot_struct _tmp1;_tmp1.tag=Cyc_NewControlFlow_CFAnnot;
_tmp1.f1=({struct Cyc_NewControlFlow_CFStmtAnnot _tmp2;_tmp2.encloser=encloser;
_tmp2.visited=0;_tmp2;});_tmp1;});_tmp0;})));}struct Cyc_NewControlFlow_AnalEnv{
struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int
iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo_union tryflow;struct Cyc_Dict_Dict*
all_changed;int noreturn;struct Cyc_List_List*param_roots;struct Cyc_Hashtable_Table*
flow_table;};static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Stmt*);
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Decl*);struct _tuple7{union Cyc_CfFlowInfo_FlowInfo_union
f1;union Cyc_CfFlowInfo_AbsLVal_union f2;};static struct _tuple7 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);
static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);struct _tuple8{union Cyc_CfFlowInfo_FlowInfo_union
f1;union Cyc_CfFlowInfo_FlowInfo_union f2;};static struct _tuple8 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,void*t);static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){void*_tmp3=(void*)s->annot;
struct Cyc_NewControlFlow_CFStmtAnnot _tmp4;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp5;_LL1: if(*((void**)_tmp3)!= Cyc_NewControlFlow_CFAnnot)goto _LL3;_tmp4=((
struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_tmp5=(struct Cyc_NewControlFlow_CFStmtAnnot*)&((
struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_LL2: return _tmp5;_LL3:;_LL4:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp6=_cycalloc(sizeof(*
_tmp6));_tmp6[0]=({struct Cyc_Core_Impossible_struct _tmp7;_tmp7.tag=Cyc_Core_Impossible;
_tmp7.f1=({const char*_tmp8="ControlFlow -- wrong stmt annotation";
_tag_dynforward(_tmp8,sizeof(char),_get_zero_arr_size_char(_tmp8,37));});_tmp7;});
_tmp6;}));_LL0:;}static union Cyc_CfFlowInfo_FlowInfo_union*Cyc_NewControlFlow_get_stmt_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union**
sflow=((union Cyc_CfFlowInfo_FlowInfo_union**(*)(struct Cyc_Hashtable_Table*t,
struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);if(sflow
== 0){union Cyc_CfFlowInfo_FlowInfo_union*res=({union Cyc_CfFlowInfo_FlowInfo_union*
_tmp9=_region_malloc(env->r,sizeof(*_tmp9));_tmp9[0]=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmpA;(_tmpA.BottomFL).tag=0;_tmpA;});_tmp9;});((
void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo_union*
val))Cyc_Hashtable_insert)(env->flow_table,s,res);return res;}return*sflow;}
struct _tuple9{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo_union*
f2;};static struct _tuple9 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*
_tmpB=Cyc_NewControlFlow_get_stmt_annot(s);union Cyc_CfFlowInfo_FlowInfo_union*
_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);*_tmpC=Cyc_CfFlowInfo_join_flow(
env->fenv,env->all_changed,inflow,*_tmpC,1);_tmpB->visited=env->iteration_num;
return({struct _tuple9 _tmpD;_tmpD.f1=_tmpB;_tmpD.f2=_tmpC;_tmpD;});}static int Cyc_NewControlFlow_get_field_index_fs(
struct Cyc_List_List*fs,struct _dynforward_ptr*f){int n=0;for(0;(unsigned int)fs;fs=
fs->tl){struct _dynforward_ptr*_tmpE=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(_tmpE,f)== 0)return n;n=n + 1;}(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Core_Impossible_struct
_tmp10;_tmp10.tag=Cyc_Core_Impossible;_tmp10.f1=(struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp13;_tmp13.tag=0;_tmp13.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*f);{void*_tmp11[1]={& _tmp13};Cyc_aprintf(({const char*
_tmp12="get_field_index_fs failed: %s";_tag_dynforward(_tmp12,sizeof(char),
_get_zero_arr_size_char(_tmp12,30));}),_tag_dynforward(_tmp11,sizeof(void*),1));}});
_tmp10;});_tmpF;}));}static int Cyc_NewControlFlow_get_field_index(void*t,struct
_dynforward_ptr*f){void*_tmp14=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp15;union Cyc_Absyn_AggrInfoU_union _tmp16;struct Cyc_List_List*_tmp17;_LL6: if(
_tmp14 <= (void*)4)goto _LLA;if(*((int*)_tmp14)!= 10)goto _LL8;_tmp15=((struct Cyc_Absyn_AggrType_struct*)
_tmp14)->f1;_tmp16=_tmp15.aggr_info;_LL7: {struct Cyc_List_List*_tmp18=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp16))->impl))->fields;
_tmp17=_tmp18;goto _LL9;}_LL8: if(*((int*)_tmp14)!= 11)goto _LLA;_tmp17=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp14)->f2;_LL9: return Cyc_NewControlFlow_get_field_index_fs(_tmp17,f);_LLA:;
_LLB:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp19=_cycalloc(
sizeof(*_tmp19));_tmp19[0]=({struct Cyc_Core_Impossible_struct _tmp1A;_tmp1A.tag=
Cyc_Core_Impossible;_tmp1A.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp1D;_tmp1D.tag=0;_tmp1D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp1B[1]={& _tmp1D};Cyc_aprintf(({const char*_tmp1C="get_field_index failed: %s";
_tag_dynforward(_tmp1C,sizeof(char),_get_zero_arr_size_char(_tmp1C,27));}),
_tag_dynforward(_tmp1B,sizeof(void*),1));}});_tmp1A;});_tmp19;}));_LL5:;}static
void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union new_flow){if(env->in_try)env->tryflow=Cyc_CfFlowInfo_join_flow(
env->fenv,env->all_changed,new_flow,env->tryflow,1);}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo_union
flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1E=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo_union*_tmp1F=Cyc_NewControlFlow_get_stmt_flow(
env,s);union Cyc_CfFlowInfo_FlowInfo_union _tmp20=Cyc_CfFlowInfo_join_flow(env->fenv,
env->all_changed,flow,*_tmp1F,1);if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp20,*
_tmp1F)){*_tmp1F=_tmp20;if(_tmp1E->visited == env->iteration_num)env->iterate_again=
1;}}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_add_vars(struct
Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_List_List*
vds,void*leafval,struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo_union
_tmp21=inflow;struct Cyc_Dict_Dict _tmp22;struct Cyc_List_List*_tmp23;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp24;_LLD: if((_tmp21.BottomFL).tag != 0)goto _LLF;_LLE: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp25;(_tmp25.BottomFL).tag=0;_tmp25;});_LLF:
if((_tmp21.ReachableFL).tag != 1)goto _LLC;_tmp22=(_tmp21.ReachableFL).f1;_tmp23=(
_tmp21.ReachableFL).f2;_tmp24=(_tmp21.ReachableFL).f3;_LL10: for(0;vds != 0;vds=
vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct*_tmp26=({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp27=_region_malloc(fenv->r,sizeof(*_tmp27));_tmp27[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp28;_tmp28.tag=0;_tmp28.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp28;});_tmp27;});
_tmp22=Cyc_Dict_insert(_tmp22,(void*)_tmp26,Cyc_CfFlowInfo_typ_to_absrval(fenv,(
void*)((struct Cyc_Absyn_Vardecl*)vds->hd)->type,leafval));}return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp29;(_tmp29.ReachableFL).tag=1;(_tmp29.ReachableFL).f1=
_tmp22;(_tmp29.ReachableFL).f2=_tmp23;(_tmp29.ReachableFL).f3=_tmp24;_tmp29;});
_LLC:;}struct _tuple10{struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*
f2;};static void Cyc_NewControlFlow_remove_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,
union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Dict_Dict*out_unique_places,
struct Cyc_Dict_Dict old_unique_places){struct Cyc_CfFlowInfo_ConsumeInfo _tmp2B;
struct Cyc_Dict_Dict _tmp2C;struct _tuple6 _tmp2A=Cyc_CfFlowInfo_save_consume_info(
fenv,outflow,0);_tmp2B=_tmp2A.f1;_tmp2C=_tmp2B.consumed;{struct _RegionHandle
_tmp2D=_new_region("tmp");struct _RegionHandle*tmp=& _tmp2D;_push_region(tmp);{
struct Cyc_Iter_Iter _tmp2E=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,
struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(tmp,*((struct Cyc_Dict_Dict*)
_check_null(out_unique_places)));struct _tuple10 _tmp2F=*((struct _tuple10*(*)(
struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(tmp,*
out_unique_places);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple10*))Cyc_Iter_next)(
_tmp2E,& _tmp2F)){struct Cyc_CfFlowInfo_Place*_tmp30=_tmp2F.f1;if(!((int(*)(struct
Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp2C,_tmp30))({
struct Cyc_String_pa_struct _tmp33;_tmp33.tag=0;_tmp33.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_CfFlowInfo_place_err_string(_tmp30));{void*_tmp31[1]={&
_tmp33};Cyc_Tcutil_terr(_tmp2F.f2,({const char*_tmp32="Failed to consume unique variable %s";
_tag_dynforward(_tmp32,sizeof(char),_get_zero_arr_size_char(_tmp32,37));}),
_tag_dynforward(_tmp31,sizeof(void*),1));}});}};_pop_region(tmp);}*
out_unique_places=old_unique_places;}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_use_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
inflow,void*r){union Cyc_CfFlowInfo_FlowInfo_union _tmp34=inflow;struct Cyc_Dict_Dict
_tmp35;struct Cyc_List_List*_tmp36;struct Cyc_CfFlowInfo_ConsumeInfo _tmp37;_LL12:
if((_tmp34.BottomFL).tag != 0)goto _LL14;_LL13: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp38;(_tmp38.BottomFL).tag=0;_tmp38;});_LL14:
if((_tmp34.ReachableFL).tag != 1)goto _LL11;_tmp35=(_tmp34.ReachableFL).f1;_tmp36=(
_tmp34.ReachableFL).f2;_tmp37=(_tmp34.ReachableFL).f3;_LL15: if(Cyc_CfFlowInfo_initlevel(
_tmp35,r)!= (void*)2)({void*_tmp39=0;Cyc_Tcutil_terr(loc,({const char*_tmp3A="expression may not be fully initialized";
_tag_dynforward(_tmp3A,sizeof(char),_get_zero_arr_size_char(_tmp3A,40));}),
_tag_dynforward(_tmp39,sizeof(void*),0));});{struct Cyc_Dict_Dict _tmp3B=Cyc_CfFlowInfo_escape_deref(
env->fenv,_tmp35,env->all_changed,r);if(_tmp35.t == _tmp3B.t)return inflow;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp3C=({union Cyc_CfFlowInfo_FlowInfo_union _tmp3D;(
_tmp3D.ReachableFL).tag=1;(_tmp3D.ReachableFL).f1=_tmp3B;(_tmp3D.ReachableFL).f2=
_tmp36;(_tmp3D.ReachableFL).f3=_tmp37;_tmp3D;});Cyc_NewControlFlow_update_tryflow(
env,(union Cyc_CfFlowInfo_FlowInfo_union)_tmp3C);return(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp3C;}}_LL11:;}struct _tuple11{union Cyc_CfFlowInfo_FlowInfo_union f1;struct Cyc_List_List*
f2;};static struct _tuple11 Cyc_NewControlFlow_anal_unordered_Rexps(struct
_RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*es,int arg1_unconsumed){if(es == 0)return({struct
_tuple11 _tmp3E;_tmp3E.f1=inflow;_tmp3E.f2=0;_tmp3E;});if(es->tl == 0){union Cyc_CfFlowInfo_FlowInfo_union
_tmp40;void*_tmp41;struct _tuple5 _tmp3F=Cyc_NewControlFlow_anal_Rexp(env,inflow,(
struct Cyc_Absyn_Exp*)es->hd);_tmp40=_tmp3F.f1;_tmp41=_tmp3F.f2;return({struct
_tuple11 _tmp42;_tmp42.f1=_tmp40;_tmp42.f2=({struct Cyc_List_List*_tmp43=
_region_malloc(rgn,sizeof(*_tmp43));_tmp43->hd=(void*)_tmp41;_tmp43->tl=0;_tmp43;});
_tmp42;});}{struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;struct Cyc_Dict_Dict
this_all_changed;union Cyc_CfFlowInfo_FlowInfo_union old_inflow;union Cyc_CfFlowInfo_FlowInfo_union
outflow;struct Cyc_List_List*rvals;struct Cyc_CfFlowInfo_ConsumeInfo _tmp45;union
Cyc_CfFlowInfo_FlowInfo_union _tmp46;struct _tuple6 _tmp44=Cyc_CfFlowInfo_save_consume_info(
env->fenv,inflow,1);_tmp45=_tmp44.f1;_tmp46=_tmp44.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
outflow_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp5B;_tmp5B.consumed=(env->fenv)->mt_place_set;
_tmp5B.may_consume=0;_tmp5B;});int init_consume=0;do{this_all_changed=(env->fenv)->mt_place_set;
_tmp46=Cyc_CfFlowInfo_restore_consume_info(_tmp46,_tmp45);env->all_changed=({
struct Cyc_Dict_Dict*_tmp47=_region_malloc(env->r,sizeof(*_tmp47));_tmp47[0]=(env->fenv)->mt_place_set;
_tmp47;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp49;void*_tmp4A;struct _tuple5
_tmp48=Cyc_NewControlFlow_anal_Rexp(env,_tmp46,(struct Cyc_Absyn_Exp*)es->hd);
_tmp49=_tmp48.f1;_tmp4A=_tmp48.f2;outflow=_tmp49;rvals=({struct Cyc_List_List*
_tmp4B=_region_malloc(rgn,sizeof(*_tmp4B));_tmp4B->hd=(void*)_tmp4A;_tmp4B->tl=0;
_tmp4B;});this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((
struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);if(arg1_unconsumed){union
Cyc_CfFlowInfo_FlowInfo_union _tmp4C=outflow;struct Cyc_Dict_Dict _tmp4D;struct Cyc_List_List*
_tmp4E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4F;_LL17: if((_tmp4C.BottomFL).tag != 0)
goto _LL19;_LL18: goto _LL16;_LL19: if((_tmp4C.ReachableFL).tag != 1)goto _LL16;_tmp4D=(
_tmp4C.ReachableFL).f1;_tmp4E=(_tmp4C.ReachableFL).f2;_tmp4F=(_tmp4C.ReachableFL).f3;
_LL1A: outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp50;(_tmp50.ReachableFL).tag=1;(_tmp50.ReachableFL).f1=_tmp4D;(_tmp50.ReachableFL).f2=
_tmp4E;(_tmp50.ReachableFL).f3=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp51;_tmp51.consumed=
_tmp4F.consumed;_tmp51.may_consume=_tmp45.may_consume;_tmp51;});_tmp50;});_LL16:;}{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){env->all_changed=({
struct Cyc_Dict_Dict*_tmp52=_region_malloc(env->r,sizeof(*_tmp52));_tmp52[0]=(env->fenv)->mt_place_set;
_tmp52;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp54;void*_tmp55;struct _tuple5
_tmp53=Cyc_NewControlFlow_anal_Rexp(env,_tmp46,(struct Cyc_Absyn_Exp*)es2->hd);
_tmp54=_tmp53.f1;_tmp55=_tmp53.f2;rvals=({struct Cyc_List_List*_tmp56=
_region_malloc(rgn,sizeof(*_tmp56));_tmp56->hd=(void*)_tmp55;_tmp56->tl=rvals;
_tmp56;});outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp54,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);}}}{
struct Cyc_CfFlowInfo_ConsumeInfo _tmp58;struct _tuple6 _tmp57=Cyc_CfFlowInfo_save_consume_info(
env->fenv,outflow,0);_tmp58=_tmp57.f1;if(init_consume)old_inflow=Cyc_CfFlowInfo_restore_consume_info(
_tmp46,outflow_consume);else{old_inflow=_tmp46;}init_consume=1;outflow_consume=
_tmp58;_tmp46=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,_tmp46,
outflow,1);}}}while(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp46,old_inflow));if(
outer_all_changed == 0)env->all_changed=0;else{env->all_changed=({struct Cyc_Dict_Dict*
_tmp59=_region_malloc(env->r,sizeof(*_tmp59));_tmp59[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0);_tmp59;});}Cyc_NewControlFlow_update_tryflow(
env,outflow);return({struct _tuple11 _tmp5A;_tmp5A.f1=outflow;_tmp5A.f2=Cyc_List_imp_rev(
rvals);_tmp5A;});}}}static struct _tuple5 Cyc_NewControlFlow_anal_use_ints(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct
Cyc_List_List*es,int arg1_unconsumed){struct _RegionHandle _tmp5C=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp5C;_push_region(rgn);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5E;struct Cyc_List_List*_tmp5F;struct _tuple11 _tmp5D=Cyc_NewControlFlow_anal_unordered_Rexps(
rgn,env,inflow,es,arg1_unconsumed);_tmp5E=_tmp5D.f1;_tmp5F=_tmp5D.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp60=_tmp5E;struct Cyc_Dict_Dict _tmp61;_LL1C: if((_tmp60.ReachableFL).tag != 1)
goto _LL1E;_tmp61=(_tmp60.ReachableFL).f1;_LL1D: for(0;_tmp5F != 0;(_tmp5F=_tmp5F->tl,
es=((struct Cyc_List_List*)_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(
_tmp61,(void*)_tmp5F->hd)== (void*)0)({void*_tmp62=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,({const char*_tmp63="expression may not be initialized";
_tag_dynforward(_tmp63,sizeof(char),_get_zero_arr_size_char(_tmp63,34));}),
_tag_dynforward(_tmp62,sizeof(void*),0));});}goto _LL1B;_LL1E: if((_tmp60.BottomFL).tag
!= 0)goto _LL1B;_LL1F: goto _LL1B;_LL1B:;}{struct _tuple5 _tmp65=({struct _tuple5
_tmp64;_tmp64.f1=_tmp5E;_tmp64.f2=(void*)(env->fenv)->unknown_all;_tmp64;});
_npop_handler(0);return _tmp65;}};_pop_region(rgn);}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Absyn_Exp*e,void*il){
union Cyc_CfFlowInfo_FlowInfo_union _tmp66=outflow;struct Cyc_Dict_Dict _tmp67;
struct Cyc_List_List*_tmp68;struct Cyc_CfFlowInfo_ConsumeInfo _tmp69;_LL21: if((
_tmp66.BottomFL).tag != 0)goto _LL23;_LL22: return outflow;_LL23: if((_tmp66.ReachableFL).tag
!= 1)goto _LL20;_tmp67=(_tmp66.ReachableFL).f1;_tmp68=(_tmp66.ReachableFL).f2;
_tmp69=(_tmp66.ReachableFL).f3;_LL24: {union Cyc_CfFlowInfo_AbsLVal_union _tmp6A=(
Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp6B;
_LL26: if((_tmp6A.UnknownL).tag != 1)goto _LL28;_LL27: return outflow;_LL28: if((
_tmp6A.PlaceL).tag != 0)goto _LL25;_tmp6B=(_tmp6A.PlaceL).f1;_LL29: {void*nzval=il
== (void*)2?(void*)1:(void*)2;return(union Cyc_CfFlowInfo_FlowInfo_union)({union
Cyc_CfFlowInfo_FlowInfo_union _tmp6C;(_tmp6C.ReachableFL).tag=1;(_tmp6C.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp67,env->all_changed,_tmp6B,
nzval);(_tmp6C.ReachableFL).f2=_tmp68;(_tmp6C.ReachableFL).f3=_tmp69;_tmp6C;});}
_LL25:;}_LL20:;}static struct _tuple8 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union
outflow,struct Cyc_Absyn_Exp*e,void*il){union Cyc_CfFlowInfo_FlowInfo_union _tmp6D=
outflow;struct Cyc_Dict_Dict _tmp6E;struct Cyc_List_List*_tmp6F;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp70;_LL2B: if((_tmp6D.BottomFL).tag != 0)goto _LL2D;_LL2C: return({struct _tuple8
_tmp71;_tmp71.f1=outflow;_tmp71.f2=outflow;_tmp71;});_LL2D: if((_tmp6D.ReachableFL).tag
!= 1)goto _LL2A;_tmp6E=(_tmp6D.ReachableFL).f1;_tmp6F=(_tmp6D.ReachableFL).f2;
_tmp70=(_tmp6D.ReachableFL).f3;_LL2E: {union Cyc_CfFlowInfo_AbsLVal_union _tmp72=(
Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp73;
_LL30: if((_tmp72.UnknownL).tag != 1)goto _LL32;_LL31: return({struct _tuple8 _tmp74;
_tmp74.f1=outflow;_tmp74.f2=outflow;_tmp74;});_LL32: if((_tmp72.PlaceL).tag != 0)
goto _LL2F;_tmp73=(_tmp72.PlaceL).f1;_LL33: {void*nzval=il == (void*)2?(void*)1:(
void*)2;return({struct _tuple8 _tmp75;_tmp75.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp77;(_tmp77.ReachableFL).tag=1;(_tmp77.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp6E,env->all_changed,_tmp73,
nzval);(_tmp77.ReachableFL).f2=_tmp6F;(_tmp77.ReachableFL).f3=_tmp70;_tmp77;});
_tmp75.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp76;(_tmp76.ReachableFL).tag=1;(_tmp76.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp6E,env->all_changed,_tmp73,(void*)0);(_tmp76.ReachableFL).f2=
_tmp6F;(_tmp76.ReachableFL).f3=_tmp70;_tmp76;});_tmp75;});}_LL2F:;}_LL2A:;}
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_if_tagcmp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union flow,struct Cyc_Absyn_Exp*e1,void*r1,void*p,
void*r2){union Cyc_CfFlowInfo_FlowInfo_union _tmp78=flow;struct Cyc_Dict_Dict _tmp79;
struct Cyc_List_List*_tmp7A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp7B;_LL35: if((
_tmp78.BottomFL).tag != 0)goto _LL37;_LL36: return flow;_LL37: if((_tmp78.ReachableFL).tag
!= 1)goto _LL34;_tmp79=(_tmp78.ReachableFL).f1;_tmp7A=(_tmp78.ReachableFL).f2;
_tmp7B=(_tmp78.ReachableFL).f3;_LL38: {void*_tmp7C=r2;struct Cyc_List_List*_tmp7D;
_LL3A: if(_tmp7C <= (void*)3)goto _LL3C;if(*((int*)_tmp7C)!= 3)goto _LL3C;_tmp7D=((
struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp7C)->f1;_LL3B: {union Cyc_CfFlowInfo_AbsLVal_union
_tmp7E=(Cyc_NewControlFlow_anal_Lexp(env,flow,e1)).f2;struct Cyc_CfFlowInfo_Place*
_tmp7F;_LL3F: if((_tmp7E.UnknownL).tag != 1)goto _LL41;_LL40: return flow;_LL41: if((
_tmp7E.PlaceL).tag != 0)goto _LL3E;_tmp7F=(_tmp7E.PlaceL).f1;_LL42: {struct Cyc_List_List*
new_cl;{void*_tmp80=r1;struct Cyc_List_List*_tmp81;void*_tmp82;_LL44: if(_tmp80 <= (
void*)3)goto _LL48;if(*((int*)_tmp80)!= 3)goto _LL46;_tmp81=((struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp80)->f1;_LL45: new_cl=_tmp81;goto _LL43;_LL46: if(*((int*)_tmp80)!= 0)goto _LL48;
_tmp82=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp80)->f1;if((int)_tmp82
!= 2)goto _LL48;_LL47: goto _LL49;_LL48: if((int)_tmp80 != 0)goto _LL4A;_LL49: goto
_LL4B;_LL4A: if((int)_tmp80 != 1)goto _LL4C;_LL4B: new_cl=0;goto _LL43;_LL4C:;_LL4D:
return flow;_LL43:;}for(0;_tmp7D != 0;_tmp7D=_tmp7D->tl){void*new_cmp;{struct
_tuple0 _tmp84=({struct _tuple0 _tmp83;_tmp83.f1=p;_tmp83.f2=(void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp7D->hd)->cmp;_tmp83;});void*_tmp85;void*_tmp86;void*_tmp87;void*_tmp88;_LL4F:
_tmp85=_tmp84.f1;if((int)_tmp85 != 8)goto _LL51;_LL50: goto _LL52;_LL51: _tmp86=
_tmp84.f2;if((int)_tmp86 != 8)goto _LL53;_LL52: new_cmp=(void*)8;goto _LL4E;_LL53:
_tmp87=_tmp84.f1;if((int)_tmp87 != 5)goto _LL55;_tmp88=_tmp84.f2;if((int)_tmp88 != 
5)goto _LL55;_LL54: new_cmp=(void*)5;goto _LL4E;_LL55:;_LL56: new_cmp=(void*)10;goto
_LL4E;_LL4E:;}new_cl=({struct Cyc_List_List*_tmp89=_region_malloc(env->r,sizeof(*
_tmp89));_tmp89->hd=({struct Cyc_CfFlowInfo_TagCmp*_tmp8A=_region_malloc(env->r,
sizeof(*_tmp8A));_tmp8A->cmp=(void*)new_cmp;_tmp8A->bd=(void*)((void*)((struct
Cyc_CfFlowInfo_TagCmp*)_tmp7D->hd)->bd);_tmp8A;});_tmp89->tl=new_cl;_tmp89;});}
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp8B;(_tmp8B.ReachableFL).tag=1;(_tmp8B.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
env->fenv,e1->loc,_tmp79,env->all_changed,_tmp7F,(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp8C=_region_malloc(env->r,sizeof(*_tmp8C));_tmp8C[0]=({struct Cyc_CfFlowInfo_TagCmps_struct
_tmp8D;_tmp8D.tag=3;_tmp8D.f1=new_cl;_tmp8D;});_tmp8C;}));(_tmp8B.ReachableFL).f2=
_tmp7A;(_tmp8B.ReachableFL).f3=_tmp7B;_tmp8B;});}_LL3E:;}_LL3C:;_LL3D: return flow;
_LL39:;}_LL34:;}static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={
Cyc_CfFlowInfo_NotZero,0};static struct Cyc_CfFlowInfo_UnknownZ_struct Cyc_NewControlFlow_mt_unknownz_v={
Cyc_CfFlowInfo_UnknownZ,0};static struct _tuple5 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
union Cyc_CfFlowInfo_FlowInfo_union f,struct Cyc_Absyn_Exp*e,void*r){void*_tmp90=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp91;void*_tmp92;struct Cyc_Absyn_PtrAtts _tmp93;struct
Cyc_Absyn_Conref*_tmp94;struct Cyc_Absyn_Conref*_tmp95;_LL58: if(_tmp90 <= (void*)4)
goto _LL5A;if(*((int*)_tmp90)!= 4)goto _LL5A;_tmp91=((struct Cyc_Absyn_PointerType_struct*)
_tmp90)->f1;_tmp92=(void*)_tmp91.elt_typ;_tmp93=_tmp91.ptr_atts;_tmp94=_tmp93.bounds;
_tmp95=_tmp93.zero_term;_LL59: {union Cyc_CfFlowInfo_FlowInfo_union _tmp96=f;
struct Cyc_Dict_Dict _tmp97;struct Cyc_List_List*_tmp98;_LL5D: if((_tmp96.BottomFL).tag
!= 0)goto _LL5F;_LL5E: return({struct _tuple5 _tmp99;_tmp99.f1=f;_tmp99.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp92,(void*)(env->fenv)->unknown_all);_tmp99;});_LL5F: if((_tmp96.ReachableFL).tag
!= 1)goto _LL5C;_tmp97=(_tmp96.ReachableFL).f1;_tmp98=(_tmp96.ReachableFL).f2;
_LL60: if(Cyc_Tcutil_is_bound_one(_tmp94)){void*_tmp9A=r;struct Cyc_CfFlowInfo_Place*
_tmp9B;void*_tmp9C;_LL62: if((int)_tmp9A != 1)goto _LL64;_LL63: goto _LL65;_LL64: if((
int)_tmp9A != 2)goto _LL66;_LL65:{void*_tmp9D=(void*)e->annot;struct Cyc_List_List*
_tmp9E;_LL6F: if(*((void**)_tmp9D)!= Cyc_CfFlowInfo_NotZero)goto _LL71;_tmp9E=((
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp9D)->f1;_LL70: if(!Cyc_CfFlowInfo_same_relns(
_tmp98,_tmp9E))goto _LL72;goto _LL6E;_LL71:;_LL72:{void*_tmp9F=(void*)e->r;_LL74:
if(*((int*)_tmp9F)!= 25)goto _LL76;_LL75:(void*)(e->annot=(void*)((void*)({struct
Cyc_CfFlowInfo_NotZero_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=({
struct Cyc_CfFlowInfo_NotZero_struct _tmpA1;_tmpA1.tag=Cyc_CfFlowInfo_NotZero;
_tmpA1.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp98);_tmpA1;});
_tmpA0;})));goto _LL73;_LL76:;_LL77:(void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_notzero_v));
goto _LL73;_LL73:;}goto _LL6E;_LL6E:;}goto _LL61;_LL66: if(_tmp9A <= (void*)3)goto
_LL68;if(*((int*)_tmp9A)!= 2)goto _LL68;_tmp9B=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp9A)->f1;_LL67:{void*_tmpA2=(void*)e->annot;struct Cyc_List_List*_tmpA3;_LL79:
if(*((void**)_tmpA2)!= Cyc_CfFlowInfo_NotZero)goto _LL7B;_tmpA3=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpA2)->f1;_LL7A: if(!Cyc_CfFlowInfo_same_relns(_tmp98,_tmpA3))goto _LL7C;goto
_LL78;_LL7B:;_LL7C:{void*_tmpA4=(void*)e->r;_LL7E: if(*((int*)_tmpA4)!= 25)goto
_LL80;_LL7F:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmpA6;_tmpA6.tag=Cyc_CfFlowInfo_NotZero;_tmpA6.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp98);_tmpA6;});_tmpA5;})));goto _LL7D;_LL80:;_LL81:(void*)(e->annot=(void*)((
void*)& Cyc_NewControlFlow_mt_notzero_v));goto _LL7D;_LL7D:;}goto _LL78;_LL78:;}
return({struct _tuple5 _tmpA7;_tmpA7.f1=f;_tmpA7.f2=Cyc_CfFlowInfo_lookup_place(
_tmp97,_tmp9B);_tmpA7;});_LL68: if((int)_tmp9A != 0)goto _LL6A;_LL69:(void*)(e->annot=(
void*)((void*)Cyc_CfFlowInfo_IsZero));return({struct _tuple5 _tmpA8;_tmpA8.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmpA9;(
_tmpA9.BottomFL).tag=0;_tmpA9;});_tmpA8.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,
_tmp92,(void*)(env->fenv)->unknown_all);_tmpA8;});_LL6A: if(_tmp9A <= (void*)3)
goto _LL6C;if(*((int*)_tmp9A)!= 0)goto _LL6C;_tmp9C=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp9A)->f1;_LL6B: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp9C);goto _LL6D;
_LL6C:;_LL6D:{void*_tmpAA=(void*)e->r;_LL83: if(*((int*)_tmpAA)!= 25)goto _LL85;
_LL84:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmpAC;_tmpAC.tag=Cyc_CfFlowInfo_UnknownZ;_tmpAC.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp98);_tmpAC;});_tmpAB;})));goto _LL82;_LL85:;_LL86:(void*)(
e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_unknownz_v));goto _LL82;_LL82:;}
goto _LL61;_LL61:;}else{void*_tmpAD=(void*)e->annot;struct Cyc_List_List*_tmpAE;
_LL88: if(*((void**)_tmpAD)!= Cyc_CfFlowInfo_UnknownZ)goto _LL8A;_tmpAE=((struct
Cyc_CfFlowInfo_UnknownZ_struct*)_tmpAD)->f1;_LL89: if(!Cyc_CfFlowInfo_same_relns(
_tmp98,_tmpAE))goto _LL8B;goto _LL87;_LL8A:;_LL8B:{void*_tmpAF=(void*)e->r;_LL8D:
if(*((int*)_tmpAF)!= 25)goto _LL8F;_LL8E:(void*)(e->annot=(void*)((void*)({struct
Cyc_CfFlowInfo_UnknownZ_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=({
struct Cyc_CfFlowInfo_UnknownZ_struct _tmpB1;_tmpB1.tag=Cyc_CfFlowInfo_UnknownZ;
_tmpB1.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp98);_tmpB1;});
_tmpB0;})));goto _LL8C;_LL8F:;_LL90:(void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_unknownz_v));
goto _LL8C;_LL8C:;}goto _LL87;_LL87:;}{void*_tmpB2=Cyc_CfFlowInfo_initlevel(_tmp97,
r);_LL92: if((int)_tmpB2 != 0)goto _LL94;_LL93:({void*_tmpB3=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmpB4="dereference of possibly uninitialized pointer";_tag_dynforward(
_tmpB4,sizeof(char),_get_zero_arr_size_char(_tmpB4,46));}),_tag_dynforward(
_tmpB3,sizeof(void*),0));});goto _LL95;_LL94: if((int)_tmpB2 != 2)goto _LL96;_LL95:
return({struct _tuple5 _tmpB5;_tmpB5.f1=f;_tmpB5.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp92,(void*)(env->fenv)->unknown_all);_tmpB5;});_LL96: if((int)_tmpB2
!= 1)goto _LL91;_LL97: return({struct _tuple5 _tmpB6;_tmpB6.f1=f;_tmpB6.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp92,(void*)(env->fenv)->unknown_none);_tmpB6;});_LL91:;}_LL5C:;}
_LL5A:;_LL5B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpB7=
_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Core_Impossible_struct _tmpB8;
_tmpB8.tag=Cyc_Core_Impossible;_tmpB8.f1=({const char*_tmpB9="right deref of non-pointer-type";
_tag_dynforward(_tmpB9,sizeof(char),_get_zero_arr_size_char(_tmpB9,32));});
_tmpB8;});_tmpB7;}));_LL57:;}static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(
struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){void*_tmpBA=(void*)e1->r;void*_tmpBB;struct Cyc_Absyn_Vardecl*
_tmpBC;void*_tmpBD;struct Cyc_Absyn_Vardecl*_tmpBE;void*_tmpBF;struct Cyc_Absyn_Vardecl*
_tmpC0;void*_tmpC1;struct Cyc_Absyn_Vardecl*_tmpC2;_LL99: if(*((int*)_tmpBA)!= 1)
goto _LL9B;_tmpBB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(_tmpBB <= (
void*)1)goto _LL9B;if(*((int*)_tmpBB)!= 4)goto _LL9B;_tmpBC=((struct Cyc_Absyn_Pat_b_struct*)
_tmpBB)->f1;_LL9A: _tmpBE=_tmpBC;goto _LL9C;_LL9B: if(*((int*)_tmpBA)!= 1)goto _LL9D;
_tmpBD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(_tmpBD <= (void*)1)
goto _LL9D;if(*((int*)_tmpBD)!= 3)goto _LL9D;_tmpBE=((struct Cyc_Absyn_Local_b_struct*)
_tmpBD)->f1;_LL9C: _tmpC0=_tmpBE;goto _LL9E;_LL9D: if(*((int*)_tmpBA)!= 1)goto _LL9F;
_tmpBF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(_tmpBF <= (void*)1)
goto _LL9F;if(*((int*)_tmpBF)!= 2)goto _LL9F;_tmpC0=((struct Cyc_Absyn_Param_b_struct*)
_tmpBF)->f1;_LL9E: _tmpC2=_tmpC0;goto _LLA0;_LL9F: if(*((int*)_tmpBA)!= 1)goto _LLA1;
_tmpC1=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(_tmpC1 <= (void*)1)
goto _LLA1;if(*((int*)_tmpC1)!= 0)goto _LLA1;_tmpC2=((struct Cyc_Absyn_Global_b_struct*)
_tmpC1)->f1;_LLA0: if(!_tmpC2->escapes){void*_tmpC3=(void*)e2->r;void*_tmpC4;
struct Cyc_Absyn_Vardecl*_tmpC5;void*_tmpC6;struct Cyc_Absyn_Vardecl*_tmpC7;void*
_tmpC8;struct Cyc_Absyn_Vardecl*_tmpC9;void*_tmpCA;struct Cyc_Absyn_Vardecl*_tmpCB;
_LLA4: if(*((int*)_tmpC3)!= 1)goto _LLA6;_tmpC4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f2;if(_tmpC4 <= (void*)1)goto _LLA6;if(*((int*)_tmpC4)!= 4)goto _LLA6;
_tmpC5=((struct Cyc_Absyn_Pat_b_struct*)_tmpC4)->f1;_LLA5: _tmpC7=_tmpC5;goto _LLA7;
_LLA6: if(*((int*)_tmpC3)!= 1)goto _LLA8;_tmpC6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f2;if(_tmpC6 <= (void*)1)goto _LLA8;if(*((int*)_tmpC6)!= 3)goto _LLA8;
_tmpC7=((struct Cyc_Absyn_Local_b_struct*)_tmpC6)->f1;_LLA7: _tmpC9=_tmpC7;goto
_LLA9;_LLA8: if(*((int*)_tmpC3)!= 1)goto _LLAA;_tmpC8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f2;if(_tmpC8 <= (void*)1)goto _LLAA;if(*((int*)_tmpC8)!= 2)goto _LLAA;
_tmpC9=((struct Cyc_Absyn_Param_b_struct*)_tmpC8)->f1;_LLA9: _tmpCB=_tmpC9;goto
_LLAB;_LLAA: if(*((int*)_tmpC3)!= 1)goto _LLAC;_tmpCA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f2;if(_tmpCA <= (void*)1)goto _LLAC;if(*((int*)_tmpCA)!= 0)goto _LLAC;
_tmpCB=((struct Cyc_Absyn_Global_b_struct*)_tmpCA)->f1;_LLAB: if(!_tmpCB->escapes){
int found=0;{struct Cyc_List_List*_tmpCC=relns;for(0;_tmpCC != 0;_tmpCC=_tmpCC->tl){
struct Cyc_CfFlowInfo_Reln*_tmpCD=(struct Cyc_CfFlowInfo_Reln*)_tmpCC->hd;if(
_tmpCD->vd == _tmpCB){union Cyc_CfFlowInfo_RelnOp_union _tmpCE=_tmpCD->rop;struct
Cyc_Absyn_Vardecl*_tmpCF;_LLAF: if((_tmpCE.LessSize).tag != 2)goto _LLB1;_tmpCF=(
_tmpCE.LessSize).f1;if(!(_tmpCF == _tmpC2))goto _LLB1;_LLB0: return relns;_LLB1:;
_LLB2: continue;_LLAE:;}}}if(!found)return({struct Cyc_List_List*_tmpD0=
_region_malloc(rgn,sizeof(*_tmpD0));_tmpD0->hd=({struct Cyc_CfFlowInfo_Reln*
_tmpD1=_region_malloc(rgn,sizeof(*_tmpD1));_tmpD1->vd=_tmpCB;_tmpD1->rop=(union
Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmpD2;(_tmpD2.LessSize).tag=
2;(_tmpD2.LessSize).f1=_tmpC2;_tmpD2;});_tmpD1;});_tmpD0->tl=relns;_tmpD0;});}
return relns;_LLAC:;_LLAD: return relns;_LLA3:;}else{return relns;}_LLA1:;_LLA2:
return relns;_LL98:;}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_may_consume_place(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,
struct Cyc_CfFlowInfo_Place*place,void*t,struct Cyc_List_List**ps){struct Cyc_CfFlowInfo_FlowEnv*
_tmpD3=env->fenv;struct Cyc_List_List*_tmpD4=Cyc_NewControlFlow_noalias_ptrs_rec(
env,place,t);if(ps != 0)*ps=_tmpD4;if(_tmpD4 == 0)return cinfo;cinfo.may_consume=((
struct Cyc_List_List*(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(
_tmpD3->r,Cyc_CfFlowInfo_place_cmp,_tmpD4,cinfo.may_consume);return cinfo;}struct
_tuple12{union Cyc_CfFlowInfo_AbsLVal_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_may_consume_exp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_noalias_path(e)){struct Cyc_CfFlowInfo_FlowEnv*
_tmpD5=env->fenv;union Cyc_CfFlowInfo_AbsLVal_union _tmpD7;struct _tuple7 _tmpD6=Cyc_NewControlFlow_anal_Lexp(
env,inflow,e);_tmpD7=_tmpD6.f2;{struct _tuple12 _tmpD9=({struct _tuple12 _tmpD8;
_tmpD8.f1=_tmpD7;_tmpD8.f2=inflow;_tmpD8;});union Cyc_CfFlowInfo_AbsLVal_union
_tmpDA;struct Cyc_CfFlowInfo_Place*_tmpDB;union Cyc_CfFlowInfo_FlowInfo_union
_tmpDC;struct Cyc_Dict_Dict _tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_CfFlowInfo_ConsumeInfo
_tmpDF;_LLB4: _tmpDA=_tmpD9.f1;if(((_tmpD9.f1).PlaceL).tag != 0)goto _LLB6;_tmpDB=(
_tmpDA.PlaceL).f1;_tmpDC=_tmpD9.f2;if(((_tmpD9.f2).ReachableFL).tag != 1)goto
_LLB6;_tmpDD=(_tmpDC.ReachableFL).f1;_tmpDE=(_tmpDC.ReachableFL).f2;_tmpDF=(
_tmpDC.ReachableFL).f3;_LLB5: {struct Cyc_CfFlowInfo_ConsumeInfo _tmpE0=Cyc_NewControlFlow_may_consume_place(
env,_tmpDF,_tmpDB,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0);
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmpE1;(_tmpE1.ReachableFL).tag=1;(_tmpE1.ReachableFL).f1=_tmpDD;(_tmpE1.ReachableFL).f2=
_tmpDE;(_tmpE1.ReachableFL).f3=_tmpE0;_tmpE1;});}_LLB6:;_LLB7:({struct Cyc_String_pa_struct
_tmpE4;_tmpE4.tag=0;_tmpE4.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmpE2[1]={& _tmpE4};Cyc_fprintf(Cyc_stderr,({const char*_tmpE3="Oops---no location for noalias_path |%s|\n";
_tag_dynforward(_tmpE3,sizeof(char),_get_zero_arr_size_char(_tmpE3,42));}),
_tag_dynforward(_tmpE2,sizeof(void*),1));}});return inflow;_LLB3:;}}return inflow;}
static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_consume_place(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_Dict_Dict
outdict,struct Cyc_Position_Segment*loc){if(!((int(*)(struct Cyc_Dict_Dict d,struct
Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(cinfo.consumed,p)){struct Cyc_CfFlowInfo_Place
_tmpE6;void*_tmpE7;struct Cyc_List_List*_tmpE8;struct Cyc_CfFlowInfo_Place*_tmpE5=
p;_tmpE6=*_tmpE5;_tmpE7=(void*)_tmpE6.root;_tmpE8=_tmpE6.fields;{void*rval=(void*)
0;if((Cyc_Dict_lookup_bool(outdict,_tmpE7,& rval) && Cyc_CfFlowInfo_initlevel(
outdict,rval)!= (void*)0) && rval != (void*)0){void*_tmpE9=_tmpE7;struct Cyc_Absyn_Vardecl*
_tmpEA;_LLB9: if(*((int*)_tmpE9)!= 0)goto _LLBB;_tmpEA=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmpE9)->f1;_LLBA: {struct _dynforward_ptr _tmpEB=Cyc_Absynpp_qvar2string(_tmpEA->name);
if(_tmpE8 == 0)({struct Cyc_String_pa_struct _tmpEE;_tmpEE.tag=0;_tmpEE.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmpEB);{void*_tmpEC[1]={& _tmpEE};Cyc_Tcutil_warn(
loc,({const char*_tmpED="may clobber unique pointer %s";_tag_dynforward(_tmpED,
sizeof(char),_get_zero_arr_size_char(_tmpED,30));}),_tag_dynforward(_tmpEC,
sizeof(void*),1));}});else{({struct Cyc_String_pa_struct _tmpF1;_tmpF1.tag=0;
_tmpF1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmpEB);{void*_tmpEF[1]={&
_tmpF1};Cyc_Tcutil_warn(loc,({const char*_tmpF0="may clobber unique pointer contained in %s";
_tag_dynforward(_tmpF0,sizeof(char),_get_zero_arr_size_char(_tmpF0,43));}),
_tag_dynforward(_tmpEF,sizeof(void*),1));}});}goto _LLB8;}_LLBB:;_LLBC:({void*
_tmpF2=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpF3="warning locations not for VarRoots";_tag_dynforward(_tmpF3,
sizeof(char),_get_zero_arr_size_char(_tmpF3,35));}),_tag_dynforward(_tmpF2,
sizeof(void*),0));});_LLB8:;}}}if(env->all_changed == 0)cinfo.consumed=((struct
Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))
Cyc_Dict_rdelete)((env->fenv)->r,cinfo.consumed,p);return cinfo;}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_consume_assignment(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_Dict_Dict
outdict,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_noalias_path(e)){struct Cyc_CfFlowInfo_FlowEnv*
_tmpF4=env->fenv;struct Cyc_List_List*_tmpF5=0;cinfo=Cyc_NewControlFlow_may_consume_place(
env,cinfo,p,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(struct Cyc_List_List**)&
_tmpF5);while(_tmpF5 != 0){cinfo=Cyc_NewControlFlow_consume_place(env,(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmpF5))->hd,cinfo,outdict,e->loc);_tmpF5=((
struct Cyc_List_List*)_check_null(_tmpF5))->tl;}}return cinfo;}static struct Cyc_List_List*
Cyc_NewControlFlow_noalias_ptrs_aux(struct Cyc_NewControlFlow_AnalEnv*env,struct
Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*ts){struct Cyc_List_List*l=0;struct Cyc_CfFlowInfo_Place
_tmpF7;void*_tmpF8;struct Cyc_List_List*_tmpF9;struct Cyc_CfFlowInfo_Place*_tmpF6=
p;_tmpF7=*_tmpF6;_tmpF8=(void*)_tmpF7.root;_tmpF9=_tmpF7.fields;{int fld=0;for(0;
ts != 0;(fld ++,ts=ts->tl)){void*_tmpFA=(void*)ts->hd;if(Cyc_Tcutil_is_noalias_pointer(
_tmpFA)){struct Cyc_List_List*_tmpFB=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
env->r,_tmpF9,({int _tmpFE[1];_tmpFE[0]=fld;((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(env->r,_tag_dynforward(
_tmpFE,sizeof(int),1));}));l=((struct Cyc_List_List*(*)(struct _RegionHandle*,int(*
cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
a,struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmpFC=_region_malloc(env->r,sizeof(*_tmpFC));_tmpFC->hd=({
struct Cyc_CfFlowInfo_Place*_tmpFD=_region_malloc(env->r,sizeof(*_tmpFD));_tmpFD->root=(
void*)_tmpF8;_tmpFD->fields=_tmpFB;_tmpFD;});_tmpFC->tl=0;_tmpFC;}),l);}else{if(
Cyc_Absyn_is_aggr_type(_tmpFA)){struct Cyc_List_List*_tmpFF=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
env->r,_tmpF9,({int _tmp102[1];_tmp102[0]=fld;((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(env->r,_tag_dynforward(
_tmp102,sizeof(int),1));}));struct Cyc_CfFlowInfo_Place*_tmp100=({struct Cyc_CfFlowInfo_Place*
_tmp101=_region_malloc(env->r,sizeof(*_tmp101));_tmp101->root=(void*)_tmpF8;
_tmp101->fields=_tmpFF;_tmp101;});l=((struct Cyc_List_List*(*)(struct
_RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,
l,Cyc_NewControlFlow_noalias_ptrs_rec(env,_tmp100,_tmpFA));}}}}return l;}struct
_tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t){if(Cyc_Tcutil_is_noalias_pointer(
t))return({struct Cyc_List_List*_tmp103=_region_malloc(env->r,sizeof(*_tmp103));
_tmp103->hd=p;_tmp103->tl=0;_tmp103;});{void*_tmp104=Cyc_Tcutil_compress(t);
struct Cyc_List_List*_tmp105;struct Cyc_List_List*_tmp106;struct Cyc_Absyn_AggrInfo
_tmp107;union Cyc_Absyn_AggrInfoU_union _tmp108;struct Cyc_Absyn_Aggrdecl**_tmp109;
struct Cyc_List_List*_tmp10A;struct Cyc_Absyn_AggrInfo _tmp10B;union Cyc_Absyn_AggrInfoU_union
_tmp10C;struct Cyc_Absyn_TunionFieldInfo _tmp10D;union Cyc_Absyn_TunionFieldInfoU_union
_tmp10E;struct Cyc_List_List*_tmp10F;_LLBE: if(_tmp104 <= (void*)4)goto _LLCA;if(*((
int*)_tmp104)!= 9)goto _LLC0;_tmp105=((struct Cyc_Absyn_TupleType_struct*)_tmp104)->f1;
_LLBF: {struct Cyc_List_List*_tmp110=0;while(_tmp105 != 0){_tmp110=({struct Cyc_List_List*
_tmp111=_region_malloc(env->r,sizeof(*_tmp111));_tmp111->hd=(void*)(*((struct
_tuple13*)_tmp105->hd)).f2;_tmp111->tl=_tmp110;_tmp111;});_tmp105=_tmp105->tl;}
_tmp110=Cyc_List_imp_rev(_tmp110);return Cyc_NewControlFlow_noalias_ptrs_aux(env,
p,_tmp110);}_LLC0: if(*((int*)_tmp104)!= 11)goto _LLC2;_tmp106=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp104)->f2;_LLC1: {struct Cyc_List_List*_tmp112=0;while(_tmp106 != 0){_tmp112=({
struct Cyc_List_List*_tmp113=_region_malloc(env->r,sizeof(*_tmp113));_tmp113->hd=(
void*)((void*)((struct Cyc_Absyn_Aggrfield*)_tmp106->hd)->type);_tmp113->tl=
_tmp112;_tmp113;});_tmp106=_tmp106->tl;}_tmp112=Cyc_List_imp_rev(_tmp112);return
Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp112);}_LLC2: if(*((int*)_tmp104)!= 
10)goto _LLC4;_tmp107=((struct Cyc_Absyn_AggrType_struct*)_tmp104)->f1;_tmp108=
_tmp107.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp104)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LLC4;_tmp109=(_tmp108.KnownAggr).f1;_tmp10A=_tmp107.targs;_LLC3: if((*
_tmp109)->impl == 0)return 0;else{struct Cyc_List_List*_tmp114=0;{struct
_RegionHandle _tmp115=_new_region("rgn");struct _RegionHandle*rgn=& _tmp115;
_push_region(rgn);{struct Cyc_List_List*_tmp116=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmp109)->tvs,_tmp10A);struct Cyc_List_List*_tmp117=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp109)->impl))->fields;while(
_tmp117 != 0){_tmp114=({struct Cyc_List_List*_tmp118=_region_malloc(env->r,sizeof(*
_tmp118));_tmp118->hd=(void*)Cyc_Tcutil_rsubstitute(rgn,_tmp116,(void*)((struct
Cyc_Absyn_Aggrfield*)_tmp117->hd)->type);_tmp118->tl=_tmp114;_tmp118;});_tmp117=
_tmp117->tl;}};_pop_region(rgn);}_tmp114=Cyc_List_imp_rev(_tmp114);return Cyc_NewControlFlow_noalias_ptrs_aux(
env,p,_tmp114);}_LLC4: if(*((int*)_tmp104)!= 10)goto _LLC6;_tmp10B=((struct Cyc_Absyn_AggrType_struct*)
_tmp104)->f1;_tmp10C=_tmp10B.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp104)->f1).aggr_info).UnknownAggr).tag != 0)goto _LLC6;_LLC5:({void*_tmp119=0;((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp11A="got unknown aggr in noalias_ptrs_rec";_tag_dynforward(_tmp11A,
sizeof(char),_get_zero_arr_size_char(_tmp11A,37));}),_tag_dynforward(_tmp119,
sizeof(void*),0));});_LLC6: if(*((int*)_tmp104)!= 2)goto _LLC8;_LLC7: if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
t))return({struct Cyc_List_List*_tmp11B=_region_malloc(env->r,sizeof(*_tmp11B));
_tmp11B->hd=p;_tmp11B->tl=0;_tmp11B;});else{return 0;}_LLC8: if(*((int*)_tmp104)!= 
3)goto _LLCA;_tmp10D=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp104)->f1;
_tmp10E=_tmp10D.field_info;_tmp10F=_tmp10D.targs;_LLC9: {union Cyc_Absyn_TunionFieldInfoU_union
_tmp11C=_tmp10E;struct Cyc_Absyn_Tuniondecl*_tmp11D;struct Cyc_Absyn_Tunionfield*
_tmp11E;_LLCD: if((_tmp11C.UnknownTunionfield).tag != 0)goto _LLCF;_LLCE:({void*
_tmp11F=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp120="got unknown tunion field in noalias_ptrs_rec";_tag_dynforward(
_tmp120,sizeof(char),_get_zero_arr_size_char(_tmp120,45));}),_tag_dynforward(
_tmp11F,sizeof(void*),0));});_LLCF: if((_tmp11C.KnownTunionfield).tag != 1)goto
_LLCC;_tmp11D=(_tmp11C.KnownTunionfield).f1;_tmp11E=(_tmp11C.KnownTunionfield).f2;
_LLD0: {struct Cyc_List_List*_tmp121=0;{struct _RegionHandle _tmp122=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp122;_push_region(rgn);{struct Cyc_List_List*_tmp123=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp11D->tvs,_tmp10F);struct Cyc_List_List*
_tmp124=_tmp11E->typs;while(_tmp124 != 0){_tmp121=({struct Cyc_List_List*_tmp125=
_region_malloc(env->r,sizeof(*_tmp125));_tmp125->hd=(void*)Cyc_Tcutil_rsubstitute(
rgn,_tmp123,(*((struct _tuple13*)_tmp124->hd)).f2);_tmp125->tl=_tmp121;_tmp125;});
_tmp124=_tmp124->tl;}};_pop_region(rgn);}_tmp121=Cyc_List_imp_rev(_tmp121);
return Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp121);}_LLCC:;}_LLCA:;_LLCB:
return 0;_LLBD:;}}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp126=env->fenv;struct Cyc_Dict_Dict d;struct Cyc_List_List*relns;struct Cyc_CfFlowInfo_ConsumeInfo
cinfo;{union Cyc_CfFlowInfo_FlowInfo_union _tmp127=inflow;struct Cyc_Dict_Dict
_tmp128;struct Cyc_List_List*_tmp129;struct Cyc_CfFlowInfo_ConsumeInfo _tmp12A;
_LLD2: if((_tmp127.BottomFL).tag != 0)goto _LLD4;_LLD3: return({struct _tuple5 _tmp12B;
_tmp12B.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp12C;(_tmp12C.BottomFL).tag=0;_tmp12C;});_tmp12B.f2=(void*)_tmp126->unknown_all;
_tmp12B;});_LLD4: if((_tmp127.ReachableFL).tag != 1)goto _LLD1;_tmp128=(_tmp127.ReachableFL).f1;
_tmp129=(_tmp127.ReachableFL).f2;_tmp12A=(_tmp127.ReachableFL).f3;_LLD5: d=
_tmp128;relns=_tmp129;cinfo=_tmp12A;_LLD1:;}{void*_tmp12D=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp12E;void*_tmp12F;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp131;
struct Cyc_Absyn_Exp*_tmp132;union Cyc_Absyn_Cnst_union _tmp133;union Cyc_Absyn_Cnst_union
_tmp134;int _tmp135;union Cyc_Absyn_Cnst_union _tmp136;void*_tmp137;struct Cyc_List_List*
_tmp138;void*_tmp139;void*_tmp13A;struct Cyc_Absyn_Vardecl*_tmp13B;void*_tmp13C;
struct Cyc_Absyn_Vardecl*_tmp13D;void*_tmp13E;struct Cyc_Absyn_Vardecl*_tmp13F;
void*_tmp140;struct Cyc_List_List*_tmp141;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Exp*
_tmp143;struct Cyc_Core_Opt*_tmp144;struct Cyc_Core_Opt _tmp145;struct Cyc_Absyn_Exp*
_tmp146;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Core_Opt*_tmp148;struct Cyc_Absyn_Exp*
_tmp149;struct Cyc_Absyn_Exp*_tmp14A;struct Cyc_Absyn_Exp*_tmp14B;struct Cyc_Absyn_Exp*
_tmp14C;struct Cyc_Absyn_Exp*_tmp14D;struct Cyc_List_List*_tmp14E;struct Cyc_Absyn_MallocInfo
_tmp14F;int _tmp150;struct Cyc_Absyn_Exp*_tmp151;void**_tmp152;struct Cyc_Absyn_Exp*
_tmp153;int _tmp154;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Exp*_tmp156;
struct Cyc_Absyn_Exp*_tmp157;struct Cyc_Absyn_Exp*_tmp158;struct Cyc_Absyn_Exp*
_tmp159;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_Exp*_tmp15B;struct
_dynforward_ptr*_tmp15C;struct Cyc_Absyn_Exp*_tmp15D;struct _dynforward_ptr*
_tmp15E;struct Cyc_Absyn_Exp*_tmp15F;struct Cyc_Absyn_Exp*_tmp160;struct Cyc_Absyn_Exp*
_tmp161;struct Cyc_Absyn_Exp*_tmp162;struct Cyc_Absyn_Exp*_tmp163;struct Cyc_Absyn_Exp*
_tmp164;struct Cyc_Absyn_Exp*_tmp165;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_Absyn_Exp*
_tmp167;struct Cyc_List_List*_tmp168;struct Cyc_Absyn_Tuniondecl*_tmp169;struct Cyc_List_List*
_tmp16A;struct Cyc_List_List*_tmp16B;struct Cyc_List_List*_tmp16C;struct Cyc_Absyn_Aggrdecl*
_tmp16D;struct Cyc_Absyn_Aggrdecl _tmp16E;struct Cyc_Absyn_AggrdeclImpl*_tmp16F;
struct Cyc_Absyn_AggrdeclImpl _tmp170;struct Cyc_List_List*_tmp171;struct Cyc_List_List*
_tmp172;struct Cyc_Absyn_Vardecl*_tmp173;struct Cyc_Absyn_Exp*_tmp174;struct Cyc_Absyn_Exp*
_tmp175;int _tmp176;struct Cyc_Absyn_Stmt*_tmp177;void*_tmp178;_LLD7: if(*((int*)
_tmp12D)!= 15)goto _LLD9;_tmp12E=((struct Cyc_Absyn_Cast_e_struct*)_tmp12D)->f2;
_tmp12F=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp12D)->f4;if((int)_tmp12F != 2)
goto _LLD9;_LLD8: {union Cyc_CfFlowInfo_FlowInfo_union _tmp17A;void*_tmp17B;struct
_tuple5 _tmp179=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp12E);_tmp17A=_tmp179.f1;
_tmp17B=_tmp179.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp17D;void*_tmp17E;
struct _tuple5 _tmp17C=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp17A,_tmp12E,
_tmp17B);_tmp17D=_tmp17C.f1;_tmp17E=_tmp17C.f2;return({struct _tuple5 _tmp17F;
_tmp17F.f1=_tmp17D;_tmp17F.f2=_tmp17B;_tmp17F;});}}_LLD9: if(*((int*)_tmp12D)!= 
15)goto _LLDB;_tmp130=((struct Cyc_Absyn_Cast_e_struct*)_tmp12D)->f2;_LLDA: _tmp131=
_tmp130;goto _LLDC;_LLDB: if(*((int*)_tmp12D)!= 13)goto _LLDD;_tmp131=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp12D)->f1;_LLDC: _tmp132=_tmp131;goto _LLDE;_LLDD: if(*((int*)_tmp12D)!= 14)goto
_LLDF;_tmp132=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp12D)->f1;_LLDE: return
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp132);_LLDF: if(*((int*)_tmp12D)!= 0)
goto _LLE1;_tmp133=((struct Cyc_Absyn_Const_e_struct*)_tmp12D)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmp12D)->f1).Null_c).tag != 6)goto _LLE1;_LLE0: goto
_LLE2;_LLE1: if(*((int*)_tmp12D)!= 0)goto _LLE3;_tmp134=((struct Cyc_Absyn_Const_e_struct*)
_tmp12D)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp12D)->f1).Int_c).tag != 2)
goto _LLE3;_tmp135=(_tmp134.Int_c).f2;if(_tmp135 != 0)goto _LLE3;_LLE2: return({
struct _tuple5 _tmp180;_tmp180.f1=inflow;_tmp180.f2=(void*)0;_tmp180;});_LLE3: if(*((
int*)_tmp12D)!= 0)goto _LLE5;_tmp136=((struct Cyc_Absyn_Const_e_struct*)_tmp12D)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp12D)->f1).Int_c).tag != 2)goto _LLE5;
_LLE4: goto _LLE6;_LLE5: if(*((int*)_tmp12D)!= 1)goto _LLE7;_tmp137=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp12D)->f2;if(_tmp137 <= (void*)1)goto _LLE7;if(*((int*)
_tmp137)!= 1)goto _LLE7;_LLE6: return({struct _tuple5 _tmp181;_tmp181.f1=inflow;
_tmp181.f2=(void*)1;_tmp181;});_LLE7: if(*((int*)_tmp12D)!= 32)goto _LLE9;_tmp138=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp12D)->f1;if(_tmp138 != 0)goto _LLE9;_LLE8:
goto _LLEA;_LLE9: if(*((int*)_tmp12D)!= 0)goto _LLEB;_LLEA: goto _LLEC;_LLEB: if(*((
int*)_tmp12D)!= 19)goto _LLED;_LLEC: goto _LLEE;_LLED: if(*((int*)_tmp12D)!= 18)goto
_LLEF;_LLEE: goto _LLF0;_LLEF: if(*((int*)_tmp12D)!= 20)goto _LLF1;_LLF0: goto _LLF2;
_LLF1: if(*((int*)_tmp12D)!= 21)goto _LLF3;_LLF2: goto _LLF4;_LLF3: if(*((int*)
_tmp12D)!= 34)goto _LLF5;_LLF4: goto _LLF6;_LLF5: if(*((int*)_tmp12D)!= 33)goto _LLF7;
_LLF6: return({struct _tuple5 _tmp182;_tmp182.f1=inflow;_tmp182.f2=(void*)_tmp126->unknown_all;
_tmp182;});_LLF7: if(*((int*)_tmp12D)!= 1)goto _LLF9;_tmp139=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp12D)->f2;if(_tmp139 <= (void*)1)goto _LLF9;if(*((int*)_tmp139)!= 0)goto _LLF9;
_LLF8: return({struct _tuple5 _tmp183;_tmp183.f1=inflow;_tmp183.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp126,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp126->unknown_all);
_tmp183;});_LLF9: if(*((int*)_tmp12D)!= 1)goto _LLFB;_tmp13A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp12D)->f2;if(_tmp13A <= (void*)1)goto _LLFB;if(*((int*)_tmp13A)!= 2)goto _LLFB;
_tmp13B=((struct Cyc_Absyn_Param_b_struct*)_tmp13A)->f1;_LLFA: _tmp13D=_tmp13B;
goto _LLFC;_LLFB: if(*((int*)_tmp12D)!= 1)goto _LLFD;_tmp13C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp12D)->f2;if(_tmp13C <= (void*)1)goto _LLFD;if(*((int*)_tmp13C)!= 3)goto _LLFD;
_tmp13D=((struct Cyc_Absyn_Local_b_struct*)_tmp13C)->f1;_LLFC: _tmp13F=_tmp13D;
goto _LLFE;_LLFD: if(*((int*)_tmp12D)!= 1)goto _LLFF;_tmp13E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp12D)->f2;if(_tmp13E <= (void*)1)goto _LLFF;if(*((int*)_tmp13E)!= 4)goto _LLFF;
_tmp13F=((struct Cyc_Absyn_Pat_b_struct*)_tmp13E)->f1;_LLFE: inflow=Cyc_NewControlFlow_may_consume_exp(
env,inflow,e);return({struct _tuple5 _tmp184;_tmp184.f1=inflow;_tmp184.f2=Cyc_Dict_lookup(
d,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp185=_region_malloc(env->r,
sizeof(*_tmp185));_tmp185[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp186;
_tmp186.tag=0;_tmp186.f1=_tmp13F;_tmp186;});_tmp185;}));_tmp184;});_LLFF: if(*((
int*)_tmp12D)!= 3)goto _LL101;_tmp140=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp12D)->f1;_tmp141=((struct Cyc_Absyn_Primop_e_struct*)_tmp12D)->f2;_LL100: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp188;void*_tmp189;struct _tuple5 _tmp187=Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmp141,0);_tmp188=_tmp187.f1;_tmp189=_tmp187.f2;{void*_tmp18A=
_tmp140;_LL13E: if((int)_tmp18A != 0)goto _LL140;_LL13F: goto _LL141;_LL140: if((int)
_tmp18A != 2)goto _LL142;_LL141: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp141))->hd)->loc,_tmp188);goto _LL13D;_LL142:;
_LL143: _tmp188=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp141))->hd)->loc,_tmp188);goto _LL13D;_LL13D:;}
return({struct _tuple5 _tmp18B;_tmp18B.f1=_tmp188;_tmp18B.f2=_tmp189;_tmp18B;});}
_LL101: if(*((int*)_tmp12D)!= 5)goto _LL103;_tmp142=((struct Cyc_Absyn_Increment_e_struct*)
_tmp12D)->f1;_LL102: {struct Cyc_List_List _tmp18C=({struct Cyc_List_List _tmp19B;
_tmp19B.hd=_tmp142;_tmp19B.tl=0;_tmp19B;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp18E;struct _tuple5 _tmp18D=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct
Cyc_List_List*)& _tmp18C,0);_tmp18E=_tmp18D.f1;Cyc_CfFlowInfo_check_unique_rvals(
_tmp142->loc,_tmp18E);{union Cyc_CfFlowInfo_AbsLVal_union _tmp190;struct _tuple7
_tmp18F=Cyc_NewControlFlow_anal_Lexp(env,_tmp18E,_tmp142);_tmp190=_tmp18F.f2;{
struct _tuple12 _tmp192=({struct _tuple12 _tmp191;_tmp191.f1=_tmp190;_tmp191.f2=
_tmp18E;_tmp191;});union Cyc_CfFlowInfo_AbsLVal_union _tmp193;struct Cyc_CfFlowInfo_Place*
_tmp194;union Cyc_CfFlowInfo_FlowInfo_union _tmp195;struct Cyc_Dict_Dict _tmp196;
struct Cyc_List_List*_tmp197;struct Cyc_CfFlowInfo_ConsumeInfo _tmp198;_LL145:
_tmp193=_tmp192.f1;if(((_tmp192.f1).PlaceL).tag != 0)goto _LL147;_tmp194=(_tmp193.PlaceL).f1;
_tmp195=_tmp192.f2;if(((_tmp192.f2).ReachableFL).tag != 1)goto _LL147;_tmp196=(
_tmp195.ReachableFL).f1;_tmp197=(_tmp195.ReachableFL).f2;_tmp198=(_tmp195.ReachableFL).f3;
_LL146: _tmp197=Cyc_CfFlowInfo_reln_kill_exp(_tmp126->r,_tmp197,_tmp142);_tmp18E=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp199;(
_tmp199.ReachableFL).tag=1;(_tmp199.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
_tmp126,_tmp142->loc,_tmp196,env->all_changed,_tmp194,(void*)_tmp126->unknown_all);(
_tmp199.ReachableFL).f2=_tmp197;(_tmp199.ReachableFL).f3=_tmp198;_tmp199;});goto
_LL144;_LL147:;_LL148: goto _LL144;_LL144:;}return({struct _tuple5 _tmp19A;_tmp19A.f1=
_tmp18E;_tmp19A.f2=(void*)_tmp126->unknown_all;_tmp19A;});}}_LL103: if(*((int*)
_tmp12D)!= 4)goto _LL105;_tmp143=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp12D)->f1;
_tmp144=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp12D)->f2;if(_tmp144 == 0)goto
_LL105;_tmp145=*_tmp144;_tmp146=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp12D)->f3;
_LL104: {struct Cyc_List_List _tmp19C=({struct Cyc_List_List _tmp1AB;_tmp1AB.hd=
_tmp146;_tmp1AB.tl=0;_tmp1AB;});struct Cyc_List_List _tmp19D=({struct Cyc_List_List
_tmp1AA;_tmp1AA.hd=_tmp143;_tmp1AA.tl=(struct Cyc_List_List*)& _tmp19C;_tmp1AA;});
union Cyc_CfFlowInfo_FlowInfo_union _tmp19F;struct _tuple5 _tmp19E=Cyc_NewControlFlow_anal_use_ints(
env,inflow,(struct Cyc_List_List*)& _tmp19D,1);_tmp19F=_tmp19E.f1;{union Cyc_CfFlowInfo_AbsLVal_union
_tmp1A1;struct _tuple7 _tmp1A0=Cyc_NewControlFlow_anal_Lexp(env,_tmp19F,e);_tmp1A1=
_tmp1A0.f2;_tmp19F=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp19F);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1A2=_tmp19F;struct Cyc_Dict_Dict _tmp1A3;struct Cyc_List_List*_tmp1A4;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1A5;_LL14A: if((_tmp1A2.ReachableFL).tag != 1)goto _LL14C;_tmp1A3=(_tmp1A2.ReachableFL).f1;
_tmp1A4=(_tmp1A2.ReachableFL).f2;_tmp1A5=(_tmp1A2.ReachableFL).f3;_LL14B:{union
Cyc_CfFlowInfo_AbsLVal_union _tmp1A6=_tmp1A1;struct Cyc_CfFlowInfo_Place*_tmp1A7;
_LL14F: if((_tmp1A6.PlaceL).tag != 0)goto _LL151;_tmp1A7=(_tmp1A6.PlaceL).f1;_LL150:
_tmp1A5=Cyc_NewControlFlow_consume_assignment(env,_tmp1A7,_tmp1A5,_tmp1A3,
_tmp143);_tmp1A4=Cyc_CfFlowInfo_reln_kill_exp(_tmp126->r,_tmp1A4,_tmp143);
_tmp1A3=Cyc_CfFlowInfo_assign_place(_tmp126,_tmp143->loc,_tmp1A3,env->all_changed,
_tmp1A7,(void*)_tmp126->unknown_all);_tmp19F=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp1A8;(_tmp1A8.ReachableFL).tag=1;(_tmp1A8.ReachableFL).f1=
_tmp1A3;(_tmp1A8.ReachableFL).f2=_tmp1A4;(_tmp1A8.ReachableFL).f3=_tmp1A5;
_tmp1A8;});goto _LL14E;_LL151: if((_tmp1A6.UnknownL).tag != 1)goto _LL14E;_LL152:
goto _LL14E;_LL14E:;}goto _LL149;_LL14C:;_LL14D: goto _LL149;_LL149:;}return({struct
_tuple5 _tmp1A9;_tmp1A9.f1=_tmp19F;_tmp1A9.f2=(void*)_tmp126->unknown_all;_tmp1A9;});}}
_LL105: if(*((int*)_tmp12D)!= 4)goto _LL107;_tmp147=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp12D)->f1;_tmp148=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp12D)->f2;if(
_tmp148 != 0)goto _LL107;_tmp149=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp12D)->f3;
_LL106: {struct Cyc_Dict_Dict*_tmp1AC=env->all_changed;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1AE;union Cyc_CfFlowInfo_FlowInfo_union _tmp1AF;struct _tuple6 _tmp1AD=Cyc_CfFlowInfo_save_consume_info(
_tmp126,inflow,1);_tmp1AE=_tmp1AD.f1;_tmp1AF=_tmp1AD.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
empty_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp1C9;_tmp1C9.consumed=_tmp126->mt_place_set;
_tmp1C9.may_consume=0;_tmp1C9;});struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=
empty_consume;int init_consume=0;while(1){env->all_changed=({struct Cyc_Dict_Dict*
_tmp1B0=_region_malloc(env->r,sizeof(*_tmp1B0));_tmp1B0[0]=_tmp126->mt_place_set;
_tmp1B0;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp1B2;union Cyc_CfFlowInfo_AbsLVal_union
_tmp1B3;struct _tuple7 _tmp1B1=Cyc_NewControlFlow_anal_Lexp(env,_tmp1AF,_tmp147);
_tmp1B2=_tmp1B1.f1;_tmp1B3=_tmp1B1.f2;{struct Cyc_Dict_Dict _tmp1B4=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));env->all_changed=({struct Cyc_Dict_Dict*_tmp1B5=
_region_malloc(env->r,sizeof(*_tmp1B5));_tmp1B5[0]=_tmp126->mt_place_set;_tmp1B5;});{
union Cyc_CfFlowInfo_FlowInfo_union _tmp1B7;void*_tmp1B8;struct _tuple5 _tmp1B6=Cyc_NewControlFlow_anal_Rexp(
env,_tmp1AF,_tmp149);_tmp1B7=_tmp1B6.f1;_tmp1B8=_tmp1B6.f2;{struct Cyc_Dict_Dict
_tmp1B9=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo_union
_tmp1BA=Cyc_CfFlowInfo_after_flow(_tmp126,(struct Cyc_Dict_Dict*)& _tmp1B4,_tmp1B2,
_tmp1B7,_tmp1B4,_tmp1B9);union Cyc_CfFlowInfo_FlowInfo_union _tmp1BB=Cyc_CfFlowInfo_join_flow(
_tmp126,_tmp1AC,_tmp1AF,_tmp1BA,1);struct Cyc_CfFlowInfo_ConsumeInfo _tmp1BD;
struct _tuple6 _tmp1BC=Cyc_CfFlowInfo_save_consume_info(_tmp126,_tmp1BB,0);_tmp1BD=
_tmp1BC.f1;if(init_consume)_tmp1AF=Cyc_CfFlowInfo_restore_consume_info(_tmp1AF,
outflow_consume);outflow_consume=_tmp1BD;init_consume=1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp1BB,_tmp1AF)){if(_tmp1AC == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp1AC,Cyc_CfFlowInfo_union_place_set(
_tmp1B4,_tmp1B9,0),0);}_tmp1BA=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp1BA);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1BE=_tmp1BA;struct Cyc_Dict_Dict
_tmp1BF;struct Cyc_List_List*_tmp1C0;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1C1;
_LL154: if((_tmp1BE.BottomFL).tag != 0)goto _LL156;_LL155: return({struct _tuple5
_tmp1C2;_tmp1C2.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1C3;(_tmp1C3.BottomFL).tag=0;_tmp1C3;});_tmp1C2.f2=_tmp1B8;_tmp1C2;});_LL156:
if((_tmp1BE.ReachableFL).tag != 1)goto _LL153;_tmp1BF=(_tmp1BE.ReachableFL).f1;
_tmp1C0=(_tmp1BE.ReachableFL).f2;_tmp1C1=(_tmp1BE.ReachableFL).f3;_LL157: _tmp1C1=
Cyc_CfFlowInfo_and_consume(_tmp126->r,_tmp1AE,_tmp1C1);{union Cyc_CfFlowInfo_AbsLVal_union
_tmp1C4=_tmp1B3;struct Cyc_CfFlowInfo_Place*_tmp1C5;_LL159: if((_tmp1C4.PlaceL).tag
!= 0)goto _LL15B;_tmp1C5=(_tmp1C4.PlaceL).f1;_LL15A: _tmp1C1=Cyc_NewControlFlow_consume_assignment(
env,_tmp1C5,_tmp1C1,_tmp1BF,_tmp147);_tmp1BF=Cyc_CfFlowInfo_assign_place(_tmp126,
e->loc,_tmp1BF,env->all_changed,_tmp1C5,_tmp1B8);_tmp1C0=Cyc_CfFlowInfo_reln_assign_exp(
_tmp126->r,_tmp1C0,_tmp147,_tmp149);_tmp1BA=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp1C6;(_tmp1C6.ReachableFL).tag=1;(_tmp1C6.ReachableFL).f1=
_tmp1BF;(_tmp1C6.ReachableFL).f2=_tmp1C0;(_tmp1C6.ReachableFL).f3=_tmp1C1;
_tmp1C6;});Cyc_NewControlFlow_update_tryflow(env,_tmp1BA);return({struct _tuple5
_tmp1C7;_tmp1C7.f1=_tmp1BA;_tmp1C7.f2=_tmp1B8;_tmp1C7;});_LL15B: if((_tmp1C4.UnknownL).tag
!= 1)goto _LL158;_LL15C: return({struct _tuple5 _tmp1C8;_tmp1C8.f1=Cyc_NewControlFlow_use_Rval(
env,_tmp149->loc,_tmp1BA,_tmp1B8);_tmp1C8.f2=_tmp1B8;_tmp1C8;});_LL158:;}_LL153:;}}
_tmp1AF=Cyc_CfFlowInfo_restore_consume_info(_tmp1BB,empty_consume);}}}}}}}_LL107:
if(*((int*)_tmp12D)!= 9)goto _LL109;_tmp14A=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp12D)->f1;_tmp14B=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp12D)->f2;_LL108: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp1CB;void*_tmp1CC;struct _tuple5 _tmp1CA=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp14A);_tmp1CB=_tmp1CA.f1;_tmp1CC=_tmp1CA.f2;_tmp1CB=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp14A->loc,_tmp1CB);return Cyc_NewControlFlow_anal_Rexp(env,_tmp1CB,_tmp14B);}
_LL109: if(*((int*)_tmp12D)!= 12)goto _LL10B;_tmp14C=((struct Cyc_Absyn_Throw_e_struct*)
_tmp12D)->f1;_LL10A: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1CE;void*_tmp1CF;
struct _tuple5 _tmp1CD=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp14C);_tmp1CE=
_tmp1CD.f1;_tmp1CF=_tmp1CD.f2;_tmp1CE=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp14C->loc,_tmp1CE);Cyc_NewControlFlow_use_Rval(env,_tmp14C->loc,_tmp1CE,
_tmp1CF);return({struct _tuple5 _tmp1D0;_tmp1D0.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp1D1;(_tmp1D1.BottomFL).tag=0;_tmp1D1;});
_tmp1D0.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp126,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,(void*)_tmp126->unknown_all);_tmp1D0;});}_LL10B: if(*((
int*)_tmp12D)!= 11)goto _LL10D;_tmp14D=((struct Cyc_Absyn_FnCall_e_struct*)_tmp12D)->f1;
_tmp14E=((struct Cyc_Absyn_FnCall_e_struct*)_tmp12D)->f2;_LL10C: {struct
_RegionHandle _tmp1D2=_new_region("temp");struct _RegionHandle*temp=& _tmp1D2;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1D5;struct Cyc_List_List*
_tmp1D6;struct _tuple11 _tmp1D4=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_List_List*_tmp1D3=_region_malloc(temp,sizeof(*_tmp1D3));
_tmp1D3->hd=_tmp14D;_tmp1D3->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp14E);_tmp1D3;}),1);_tmp1D5=
_tmp1D4.f1;_tmp1D6=_tmp1D4.f2;_tmp1D5=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp1D5);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1D7=Cyc_NewControlFlow_use_Rval(
env,_tmp14D->loc,_tmp1D5,(void*)((struct Cyc_List_List*)_check_null(_tmp1D6))->hd);
_tmp1D6=_tmp1D6->tl;{struct Cyc_List_List*init_params=0;{void*_tmp1D8=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp14D->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1D9;void*_tmp1DA;_LL15E: if(_tmp1D8 <= (void*)4)goto _LL160;if(*((int*)_tmp1D8)
!= 4)goto _LL160;_tmp1D9=((struct Cyc_Absyn_PointerType_struct*)_tmp1D8)->f1;
_tmp1DA=(void*)_tmp1D9.elt_typ;_LL15F:{void*_tmp1DB=Cyc_Tcutil_compress(_tmp1DA);
struct Cyc_Absyn_FnInfo _tmp1DC;struct Cyc_List_List*_tmp1DD;_LL163: if(_tmp1DB <= (
void*)4)goto _LL165;if(*((int*)_tmp1DB)!= 8)goto _LL165;_tmp1DC=((struct Cyc_Absyn_FnType_struct*)
_tmp1DB)->f1;_tmp1DD=_tmp1DC.attributes;_LL164: for(0;_tmp1DD != 0;_tmp1DD=_tmp1DD->tl){
void*_tmp1DE=(void*)_tmp1DD->hd;int _tmp1DF;_LL168: if(_tmp1DE <= (void*)17)goto
_LL16A;if(*((int*)_tmp1DE)!= 4)goto _LL16A;_tmp1DF=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1DE)->f1;_LL169: init_params=({struct Cyc_List_List*_tmp1E0=_region_malloc(
temp,sizeof(*_tmp1E0));_tmp1E0->hd=(void*)_tmp1DF;_tmp1E0->tl=init_params;
_tmp1E0;});goto _LL167;_LL16A:;_LL16B: goto _LL167;_LL167:;}goto _LL162;_LL165:;
_LL166:({void*_tmp1E1=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr
ap))Cyc_Tcutil_impos)(({const char*_tmp1E2="anal_Rexp: bad function type";
_tag_dynforward(_tmp1E2,sizeof(char),_get_zero_arr_size_char(_tmp1E2,29));}),
_tag_dynforward(_tmp1E1,sizeof(void*),0));});_LL162:;}goto _LL15D;_LL160:;_LL161:({
void*_tmp1E3=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1E4="anal_Rexp: bad function type";_tag_dynforward(_tmp1E4,sizeof(
char),_get_zero_arr_size_char(_tmp1E4,29));}),_tag_dynforward(_tmp1E3,sizeof(
void*),0));});_LL15D:;}{int i=1;for(0;_tmp1D6 != 0;(((_tmp1D6=_tmp1D6->tl,_tmp14E=((
struct Cyc_List_List*)_check_null(_tmp14E))->tl)),++ i)){if(!((int(*)(struct Cyc_List_List*
l,int x))Cyc_List_memq)(init_params,i)){_tmp1D7=Cyc_NewControlFlow_use_Rval(env,((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp14E))->hd)->loc,
_tmp1D7,(void*)_tmp1D6->hd);continue;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp1E5=
_tmp1D5;struct Cyc_Dict_Dict _tmp1E6;_LL16D: if((_tmp1E5.BottomFL).tag != 0)goto
_LL16F;_LL16E: goto _LL16C;_LL16F: if((_tmp1E5.ReachableFL).tag != 1)goto _LL16C;
_tmp1E6=(_tmp1E5.ReachableFL).f1;_LL170: if(Cyc_CfFlowInfo_initlevel(_tmp1E6,(
void*)_tmp1D6->hd)== (void*)0)({void*_tmp1E7=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp14E))->hd)->loc,({const char*_tmp1E8="expression may not be initialized";
_tag_dynforward(_tmp1E8,sizeof(char),_get_zero_arr_size_char(_tmp1E8,34));}),
_tag_dynforward(_tmp1E7,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1E9=_tmp1D7;struct Cyc_Dict_Dict _tmp1EA;struct Cyc_List_List*_tmp1EB;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1EC;_LL172: if((_tmp1E9.BottomFL).tag != 0)goto _LL174;_LL173: goto _LL171;_LL174:
if((_tmp1E9.ReachableFL).tag != 1)goto _LL171;_tmp1EA=(_tmp1E9.ReachableFL).f1;
_tmp1EB=(_tmp1E9.ReachableFL).f2;_tmp1EC=(_tmp1E9.ReachableFL).f3;_LL175: {
struct Cyc_Dict_Dict _tmp1ED=Cyc_CfFlowInfo_escape_deref(_tmp126,_tmp1EA,env->all_changed,(
void*)_tmp1D6->hd);{void*_tmp1EE=(void*)_tmp1D6->hd;struct Cyc_CfFlowInfo_Place*
_tmp1EF;_LL177: if(_tmp1EE <= (void*)3)goto _LL179;if(*((int*)_tmp1EE)!= 2)goto
_LL179;_tmp1EF=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1EE)->f1;_LL178:{
void*_tmp1F0=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp14E))->hd)->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1F1;void*_tmp1F2;_LL17C: if(_tmp1F0 <= (void*)4)goto
_LL17E;if(*((int*)_tmp1F0)!= 4)goto _LL17E;_tmp1F1=((struct Cyc_Absyn_PointerType_struct*)
_tmp1F0)->f1;_tmp1F2=(void*)_tmp1F1.elt_typ;_LL17D: _tmp1ED=Cyc_CfFlowInfo_assign_place(
_tmp126,((struct Cyc_Absyn_Exp*)_tmp14E->hd)->loc,_tmp1ED,env->all_changed,
_tmp1EF,Cyc_CfFlowInfo_typ_to_absrval(_tmp126,_tmp1F2,(void*)_tmp126->esc_all));
goto _LL17B;_LL17E:;_LL17F:({void*_tmp1F3=0;((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1F4="anal_Rexp:bad type for initialized arg";
_tag_dynforward(_tmp1F4,sizeof(char),_get_zero_arr_size_char(_tmp1F4,39));}),
_tag_dynforward(_tmp1F3,sizeof(void*),0));});_LL17B:;}goto _LL176;_LL179:;_LL17A:
goto _LL176;_LL176:;}_tmp1D7=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1F5;(_tmp1F5.ReachableFL).tag=1;(_tmp1F5.ReachableFL).f1=_tmp1ED;(_tmp1F5.ReachableFL).f2=
_tmp1EB;(_tmp1F5.ReachableFL).f3=_tmp1EC;_tmp1F5;});goto _LL171;}_LL171:;}goto
_LL16C;_LL16C:;}}}if(Cyc_Tcutil_is_noreturn((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp14D->topt))->v)){struct _tuple5 _tmp1F8=({struct _tuple5 _tmp1F6;
_tmp1F6.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1F7;(_tmp1F7.BottomFL).tag=0;_tmp1F7;});_tmp1F6.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp126,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp126->unknown_all);
_tmp1F6;});_npop_handler(0);return _tmp1F8;}else{struct _tuple5 _tmp1FA=({struct
_tuple5 _tmp1F9;_tmp1F9.f1=_tmp1D7;_tmp1F9.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp126,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp126->unknown_all);
_tmp1F9;});_npop_handler(0);return _tmp1FA;}}}};_pop_region(temp);}_LL10D: if(*((
int*)_tmp12D)!= 35)goto _LL10F;_tmp14F=((struct Cyc_Absyn_Malloc_e_struct*)_tmp12D)->f1;
_tmp150=_tmp14F.is_calloc;_tmp151=_tmp14F.rgn;_tmp152=_tmp14F.elt_type;_tmp153=
_tmp14F.num_elts;_tmp154=_tmp14F.fat_result;_LL10E: {void*root=(void*)({struct
Cyc_CfFlowInfo_MallocPt_struct*_tmp20C=_region_malloc(_tmp126->r,sizeof(*_tmp20C));
_tmp20C[0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp20D;_tmp20D.tag=1;_tmp20D.f1=
_tmp153;_tmp20D.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp20D;});_tmp20C;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp20B=_region_malloc(_tmp126->r,sizeof(*_tmp20B));_tmp20B->root=(void*)root;
_tmp20B->fields=0;_tmp20B;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp209=_region_malloc(_tmp126->r,sizeof(*_tmp209));_tmp209[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp20A;_tmp20A.tag=2;_tmp20A.f1=place;_tmp20A;});_tmp209;});void*place_val=
_tmp154?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(_tmp126,*((void**)_check_null(
_tmp152)),(void*)_tmp126->unknown_none);union Cyc_CfFlowInfo_FlowInfo_union
outflow;Cyc_CfFlowInfo_update_place_set(env->all_changed,place,0);if(_tmp151 != 0){
struct _RegionHandle _tmp1FB=_new_region("temp");struct _RegionHandle*temp=& _tmp1FB;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1FE;struct Cyc_List_List*
_tmp1FF;struct _tuple11 _tmp1FD=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_Absyn_Exp*_tmp1FC[2];_tmp1FC[1]=_tmp153;_tmp1FC[0]=(struct Cyc_Absyn_Exp*)
_tmp151;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))
Cyc_List_rlist)(temp,_tag_dynforward(_tmp1FC,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);_tmp1FE=_tmp1FD.f1;_tmp1FF=_tmp1FD.f2;outflow=_tmp1FE;};_pop_region(temp);}
else{union Cyc_CfFlowInfo_FlowInfo_union _tmp201;struct _tuple5 _tmp200=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp153);_tmp201=_tmp200.f1;outflow=_tmp201;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp153->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp202=outflow;struct
Cyc_Dict_Dict _tmp203;struct Cyc_List_List*_tmp204;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp205;_LL181: if((_tmp202.BottomFL).tag != 0)goto _LL183;_LL182: return({struct
_tuple5 _tmp206;_tmp206.f1=outflow;_tmp206.f2=rval;_tmp206;});_LL183: if((_tmp202.ReachableFL).tag
!= 1)goto _LL180;_tmp203=(_tmp202.ReachableFL).f1;_tmp204=(_tmp202.ReachableFL).f2;
_tmp205=(_tmp202.ReachableFL).f3;_LL184: return({struct _tuple5 _tmp207;_tmp207.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp208;(
_tmp208.ReachableFL).tag=1;(_tmp208.ReachableFL).f1=Cyc_Dict_insert(_tmp203,root,
place_val);(_tmp208.ReachableFL).f2=_tmp204;(_tmp208.ReachableFL).f3=_tmp205;
_tmp208;});_tmp207.f2=rval;_tmp207;});_LL180:;}}_LL10F: if(*((int*)_tmp12D)!= 36)
goto _LL111;_tmp155=((struct Cyc_Absyn_Swap_e_struct*)_tmp12D)->f1;_tmp156=((
struct Cyc_Absyn_Swap_e_struct*)_tmp12D)->f2;_LL110: {void*left_rval;void*
right_rval;union Cyc_CfFlowInfo_FlowInfo_union outflow;{struct _RegionHandle _tmp20E=
_new_region("temp");struct _RegionHandle*temp=& _tmp20E;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp211;struct Cyc_List_List*_tmp212;struct _tuple11
_tmp210=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*
_tmp20F[2];_tmp20F[1]=_tmp156;_tmp20F[0]=_tmp155;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp20F,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp211=_tmp210.f1;_tmp212=
_tmp210.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp212))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp212->tl))->hd;outflow=
_tmp211;};_pop_region(temp);}Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
outflow);{union Cyc_CfFlowInfo_AbsLVal_union _tmp214;struct _tuple7 _tmp213=Cyc_NewControlFlow_anal_Lexp(
env,outflow,_tmp155);_tmp214=_tmp213.f2;{union Cyc_CfFlowInfo_AbsLVal_union
_tmp216;struct _tuple7 _tmp215=Cyc_NewControlFlow_anal_Lexp(env,outflow,_tmp156);
_tmp216=_tmp215.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp217=outflow;struct Cyc_Dict_Dict
_tmp218;struct Cyc_List_List*_tmp219;struct Cyc_CfFlowInfo_ConsumeInfo _tmp21A;
_LL186: if((_tmp217.ReachableFL).tag != 1)goto _LL188;_tmp218=(_tmp217.ReachableFL).f1;
_tmp219=(_tmp217.ReachableFL).f2;_tmp21A=(_tmp217.ReachableFL).f3;_LL187:{union
Cyc_CfFlowInfo_AbsLVal_union _tmp21B=_tmp214;struct Cyc_CfFlowInfo_Place*_tmp21C;
_LL18B: if((_tmp21B.PlaceL).tag != 0)goto _LL18D;_tmp21C=(_tmp21B.PlaceL).f1;_LL18C:
_tmp218=Cyc_CfFlowInfo_assign_place(_tmp126,_tmp155->loc,_tmp218,env->all_changed,
_tmp21C,right_rval);goto _LL18A;_LL18D: if((_tmp21B.UnknownL).tag != 1)goto _LL18A;
_LL18E: goto _LL18A;_LL18A:;}{union Cyc_CfFlowInfo_AbsLVal_union _tmp21D=_tmp216;
struct Cyc_CfFlowInfo_Place*_tmp21E;_LL190: if((_tmp21D.PlaceL).tag != 0)goto _LL192;
_tmp21E=(_tmp21D.PlaceL).f1;_LL191: _tmp218=Cyc_CfFlowInfo_assign_place(_tmp126,
_tmp156->loc,_tmp218,env->all_changed,_tmp21E,left_rval);goto _LL18F;_LL192: if((
_tmp21D.UnknownL).tag != 1)goto _LL18F;_LL193: goto _LL18F;_LL18F:;}_tmp219=Cyc_CfFlowInfo_reln_kill_exp(
_tmp126->r,_tmp219,_tmp155);_tmp219=Cyc_CfFlowInfo_reln_kill_exp(_tmp126->r,
_tmp219,_tmp156);outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp21F;(_tmp21F.ReachableFL).tag=1;(_tmp21F.ReachableFL).f1=_tmp218;(_tmp21F.ReachableFL).f2=
_tmp219;(_tmp21F.ReachableFL).f3=_tmp21A;_tmp21F;});goto _LL185;_LL188:;_LL189:
goto _LL185;_LL185:;}return({struct _tuple5 _tmp220;_tmp220.f1=outflow;_tmp220.f2=(
void*)_tmp126->unknown_all;_tmp220;});}}}_LL111: if(*((int*)_tmp12D)!= 17)goto
_LL113;_tmp157=((struct Cyc_Absyn_New_e_struct*)_tmp12D)->f1;_tmp158=((struct Cyc_Absyn_New_e_struct*)
_tmp12D)->f2;_LL112: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp233=_region_malloc(_tmp126->r,sizeof(*_tmp233));_tmp233[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp234;_tmp234.tag=1;_tmp234.f1=_tmp158;_tmp234.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp234;});_tmp233;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp232=_region_malloc(_tmp126->r,sizeof(*_tmp232));
_tmp232->root=(void*)root;_tmp232->fields=0;_tmp232;});void*rval=(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp230=_region_malloc(_tmp126->r,sizeof(*
_tmp230));_tmp230[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp231;_tmp231.tag=
2;_tmp231.f1=place;_tmp231;});_tmp230;});Cyc_CfFlowInfo_update_place_set(env->all_changed,
place,0);{union Cyc_CfFlowInfo_FlowInfo_union outflow;void*place_val;if(_tmp157 != 
0){struct _RegionHandle _tmp221=_new_region("temp");struct _RegionHandle*temp=&
_tmp221;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp224;struct Cyc_List_List*
_tmp225;struct _tuple11 _tmp223=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_Absyn_Exp*_tmp222[2];_tmp222[1]=_tmp158;_tmp222[0]=(struct Cyc_Absyn_Exp*)
_tmp157;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))
Cyc_List_rlist)(temp,_tag_dynforward(_tmp222,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);_tmp224=_tmp223.f1;_tmp225=_tmp223.f2;outflow=_tmp224;place_val=(void*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp225))->tl))->hd;};
_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo_union _tmp227;void*_tmp228;
struct _tuple5 _tmp226=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp158);_tmp227=
_tmp226.f1;_tmp228=_tmp226.f2;outflow=_tmp227;place_val=_tmp228;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp158->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp229=outflow;struct
Cyc_Dict_Dict _tmp22A;struct Cyc_List_List*_tmp22B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp22C;_LL195: if((_tmp229.BottomFL).tag != 0)goto _LL197;_LL196: return({struct
_tuple5 _tmp22D;_tmp22D.f1=outflow;_tmp22D.f2=rval;_tmp22D;});_LL197: if((_tmp229.ReachableFL).tag
!= 1)goto _LL194;_tmp22A=(_tmp229.ReachableFL).f1;_tmp22B=(_tmp229.ReachableFL).f2;
_tmp22C=(_tmp229.ReachableFL).f3;_LL198: return({struct _tuple5 _tmp22E;_tmp22E.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp22F;(
_tmp22F.ReachableFL).tag=1;(_tmp22F.ReachableFL).f1=Cyc_Dict_insert(_tmp22A,root,
place_val);(_tmp22F.ReachableFL).f2=_tmp22B;(_tmp22F.ReachableFL).f3=_tmp22C;
_tmp22F;});_tmp22E.f2=rval;_tmp22E;});_LL194:;}}}_LL113: if(*((int*)_tmp12D)!= 16)
goto _LL115;_tmp159=((struct Cyc_Absyn_Address_e_struct*)_tmp12D)->f1;_LL114: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp236;struct _tuple5 _tmp235=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp159);_tmp236=_tmp235.f1;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp238;
struct _tuple6 _tmp237=Cyc_CfFlowInfo_save_consume_info(env->fenv,_tmp236,0);
_tmp238=_tmp237.f1;{union Cyc_CfFlowInfo_FlowInfo_union _tmp23A;union Cyc_CfFlowInfo_AbsLVal_union
_tmp23B;struct _tuple7 _tmp239=Cyc_NewControlFlow_anal_Lexp(env,inflow,_tmp159);
_tmp23A=_tmp239.f1;_tmp23B=_tmp239.f2;_tmp23A=Cyc_CfFlowInfo_restore_consume_info(
_tmp23A,_tmp238);{union Cyc_CfFlowInfo_AbsLVal_union _tmp23C=_tmp23B;struct Cyc_CfFlowInfo_Place*
_tmp23D;_LL19A: if((_tmp23C.UnknownL).tag != 1)goto _LL19C;_LL19B: return({struct
_tuple5 _tmp23E;_tmp23E.f1=_tmp23A;_tmp23E.f2=(void*)1;_tmp23E;});_LL19C: if((
_tmp23C.PlaceL).tag != 0)goto _LL199;_tmp23D=(_tmp23C.PlaceL).f1;_LL19D: return({
struct _tuple5 _tmp23F;_tmp23F.f1=_tmp23A;_tmp23F.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp240=_region_malloc(env->r,sizeof(*_tmp240));_tmp240[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp241;_tmp241.tag=2;_tmp241.f1=_tmp23D;_tmp241;});_tmp240;});_tmp23F;});_LL199:;}}}}
_LL115: if(*((int*)_tmp12D)!= 22)goto _LL117;_tmp15A=((struct Cyc_Absyn_Deref_e_struct*)
_tmp12D)->f1;_LL116: {union Cyc_CfFlowInfo_FlowInfo_union _tmp243;void*_tmp244;
struct _tuple5 _tmp242=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp15A);_tmp243=
_tmp242.f1;_tmp244=_tmp242.f2;_tmp243=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp243);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp243,_tmp15A,_tmp244);}
_LL117: if(*((int*)_tmp12D)!= 23)goto _LL119;_tmp15B=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp12D)->f1;_tmp15C=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp12D)->f2;_LL118: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp246;void*_tmp247;struct _tuple5 _tmp245=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp15B);_tmp246=_tmp245.f1;_tmp247=_tmp245.f2;_tmp246=Cyc_CfFlowInfo_drop_unique_rvals(
e->loc,_tmp246);_tmp246=Cyc_NewControlFlow_may_consume_exp(env,_tmp246,e);if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp15B->topt))->v))return({struct
_tuple5 _tmp248;_tmp248.f1=_tmp246;_tmp248.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp126,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp126->unknown_all);
_tmp248;});{void*_tmp249=_tmp247;struct _dynforward_ptr _tmp24A;_LL19F: if(_tmp249
<= (void*)3)goto _LL1A1;if(*((int*)_tmp249)!= 4)goto _LL1A1;_tmp24A=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp249)->f1;_LL1A0: {int _tmp24B=Cyc_NewControlFlow_get_field_index((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp15B->topt))->v,_tmp15C);return({struct
_tuple5 _tmp24C;_tmp24C.f1=_tmp246;_tmp24C.f2=*((void**)
_check_dynforward_subscript(_tmp24A,sizeof(void*),_tmp24B));_tmp24C;});}_LL1A1:;
_LL1A2:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp24D=_cycalloc(
sizeof(*_tmp24D));_tmp24D[0]=({struct Cyc_Core_Impossible_struct _tmp24E;_tmp24E.tag=
Cyc_Core_Impossible;_tmp24E.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp251;_tmp251.tag=0;_tmp251.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp24F[1]={& _tmp251};Cyc_aprintf(({const char*
_tmp250="anal_Rexp: AggrMember: %s";_tag_dynforward(_tmp250,sizeof(char),
_get_zero_arr_size_char(_tmp250,26));}),_tag_dynforward(_tmp24F,sizeof(void*),1));}});
_tmp24E;});_tmp24D;}));_LL19E:;}}_LL119: if(*((int*)_tmp12D)!= 24)goto _LL11B;
_tmp15D=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp12D)->f1;_tmp15E=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp12D)->f2;_LL11A: {union Cyc_CfFlowInfo_FlowInfo_union _tmp253;void*_tmp254;
struct _tuple5 _tmp252=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp15D);_tmp253=
_tmp252.f1;_tmp254=_tmp252.f2;_tmp253=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp253);{union Cyc_CfFlowInfo_FlowInfo_union _tmp256;void*_tmp257;struct _tuple5
_tmp255=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp253,_tmp15D,_tmp254);
_tmp256=_tmp255.f1;_tmp257=_tmp255.f2;{void*_tmp258=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp15D->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp259;void*_tmp25A;_LL1A4: if(_tmp258 <= (void*)4)goto _LL1A6;if(*((int*)_tmp258)
!= 4)goto _LL1A6;_tmp259=((struct Cyc_Absyn_PointerType_struct*)_tmp258)->f1;
_tmp25A=(void*)_tmp259.elt_typ;_LL1A5: if(Cyc_Absyn_is_union_type(_tmp25A))return({
struct _tuple5 _tmp25B;_tmp25B.f1=_tmp256;_tmp25B.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp126,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp126->unknown_all);
_tmp25B;});{void*_tmp25C=_tmp257;struct _dynforward_ptr _tmp25D;_LL1A9: if(_tmp25C
<= (void*)3)goto _LL1AB;if(*((int*)_tmp25C)!= 4)goto _LL1AB;_tmp25D=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp25C)->f1;_LL1AA: {int _tmp25E=Cyc_NewControlFlow_get_field_index(_tmp25A,
_tmp15E);return({struct _tuple5 _tmp25F;_tmp25F.f1=_tmp256;_tmp25F.f2=*((void**)
_check_dynforward_subscript(_tmp25D,sizeof(void*),_tmp25E));_tmp25F;});}_LL1AB:;
_LL1AC:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp260=_cycalloc(
sizeof(*_tmp260));_tmp260[0]=({struct Cyc_Core_Impossible_struct _tmp261;_tmp261.tag=
Cyc_Core_Impossible;_tmp261.f1=({const char*_tmp262="anal_Rexp: AggrArrow";
_tag_dynforward(_tmp262,sizeof(char),_get_zero_arr_size_char(_tmp262,21));});
_tmp261;});_tmp260;}));_LL1A8:;}_LL1A6:;_LL1A7:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263[0]=({struct Cyc_Core_Impossible_struct
_tmp264;_tmp264.tag=Cyc_Core_Impossible;_tmp264.f1=({const char*_tmp265="anal_Rexp: AggrArrow ptr";
_tag_dynforward(_tmp265,sizeof(char),_get_zero_arr_size_char(_tmp265,25));});
_tmp264;});_tmp263;}));_LL1A3:;}}}_LL11B: if(*((int*)_tmp12D)!= 6)goto _LL11D;
_tmp15F=((struct Cyc_Absyn_Conditional_e_struct*)_tmp12D)->f1;_tmp160=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp12D)->f2;_tmp161=((struct Cyc_Absyn_Conditional_e_struct*)_tmp12D)->f3;_LL11C: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp267;union Cyc_CfFlowInfo_FlowInfo_union
_tmp268;struct _tuple8 _tmp266=Cyc_NewControlFlow_anal_test(env,inflow,_tmp15F);
_tmp267=_tmp266.f1;_tmp268=_tmp266.f2;_tmp267=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp15F->loc,_tmp267);_tmp268=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp15F->loc,
_tmp268);{struct _tuple5 _tmp269=Cyc_NewControlFlow_anal_Rexp(env,_tmp267,_tmp160);
struct _tuple5 _tmp26A=Cyc_NewControlFlow_anal_Rexp(env,_tmp268,_tmp161);return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp126,env->all_changed,_tmp269,_tmp26A,1);}}_LL11D: if(*((int*)_tmp12D)!= 7)
goto _LL11F;_tmp162=((struct Cyc_Absyn_And_e_struct*)_tmp12D)->f1;_tmp163=((struct
Cyc_Absyn_And_e_struct*)_tmp12D)->f2;_LL11E: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp26C;union Cyc_CfFlowInfo_FlowInfo_union _tmp26D;struct _tuple8 _tmp26B=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp162);_tmp26C=_tmp26B.f1;_tmp26D=_tmp26B.f2;_tmp26C=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp162->loc,_tmp26C);_tmp26D=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp162->loc,
_tmp26D);{union Cyc_CfFlowInfo_FlowInfo_union _tmp26F;void*_tmp270;struct _tuple5
_tmp26E=Cyc_NewControlFlow_anal_Rexp(env,_tmp26C,_tmp163);_tmp26F=_tmp26E.f1;
_tmp270=_tmp26E.f2;_tmp26F=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp163->loc,
_tmp26F);{struct _tuple5 _tmp271=({struct _tuple5 _tmp274;_tmp274.f1=_tmp26F;_tmp274.f2=
_tmp270;_tmp274;});struct _tuple5 _tmp272=({struct _tuple5 _tmp273;_tmp273.f1=
_tmp26D;_tmp273.f2=(void*)((void*)0);_tmp273;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp126,env->all_changed,_tmp271,_tmp272,0);}}}_LL11F: if(*((int*)_tmp12D)!= 8)
goto _LL121;_tmp164=((struct Cyc_Absyn_Or_e_struct*)_tmp12D)->f1;_tmp165=((struct
Cyc_Absyn_Or_e_struct*)_tmp12D)->f2;_LL120: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp276;union Cyc_CfFlowInfo_FlowInfo_union _tmp277;struct _tuple8 _tmp275=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp164);_tmp276=_tmp275.f1;_tmp277=_tmp275.f2;_tmp276=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp164->loc,_tmp276);_tmp277=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp164->loc,
_tmp277);{union Cyc_CfFlowInfo_FlowInfo_union _tmp279;void*_tmp27A;struct _tuple5
_tmp278=Cyc_NewControlFlow_anal_Rexp(env,_tmp277,_tmp165);_tmp279=_tmp278.f1;
_tmp27A=_tmp278.f2;_tmp279=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp165->loc,
_tmp279);{struct _tuple5 _tmp27B=({struct _tuple5 _tmp27E;_tmp27E.f1=_tmp279;_tmp27E.f2=
_tmp27A;_tmp27E;});struct _tuple5 _tmp27C=({struct _tuple5 _tmp27D;_tmp27D.f1=
_tmp276;_tmp27D.f2=(void*)((void*)1);_tmp27D;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp126,env->all_changed,_tmp27B,_tmp27C,0);}}}_LL121: if(*((int*)_tmp12D)!= 25)
goto _LL123;_tmp166=((struct Cyc_Absyn_Subscript_e_struct*)_tmp12D)->f1;_tmp167=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp12D)->f2;_LL122: {struct _RegionHandle
_tmp27F=_new_region("temp");struct _RegionHandle*temp=& _tmp27F;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp282;struct Cyc_List_List*_tmp283;struct
_tuple11 _tmp281=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct
Cyc_Absyn_Exp*_tmp280[2];_tmp280[1]=_tmp167;_tmp280[0]=_tmp166;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp280,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp282=_tmp281.f1;_tmp283=
_tmp281.f2;_tmp282=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp167->loc,_tmp282);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp284=_tmp282;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp285=_tmp282;struct Cyc_Dict_Dict _tmp286;struct Cyc_List_List*_tmp287;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp288;_LL1AE: if((_tmp285.ReachableFL).tag != 1)goto _LL1B0;_tmp286=(_tmp285.ReachableFL).f1;
_tmp287=(_tmp285.ReachableFL).f2;_tmp288=(_tmp285.ReachableFL).f3;_LL1AF: if(Cyc_CfFlowInfo_initlevel(
_tmp286,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp283))->tl))->hd)== (void*)0)({void*_tmp289=0;Cyc_Tcutil_terr(
_tmp167->loc,({const char*_tmp28A="expression may not be initialized";
_tag_dynforward(_tmp28A,sizeof(char),_get_zero_arr_size_char(_tmp28A,34));}),
_tag_dynforward(_tmp289,sizeof(void*),0));});{struct Cyc_List_List*_tmp28B=Cyc_NewControlFlow_add_subscript_reln(
_tmp126->r,_tmp287,_tmp166,_tmp167);if(_tmp287 != _tmp28B)_tmp284=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp28C;(_tmp28C.ReachableFL).tag=1;(_tmp28C.ReachableFL).f1=
_tmp286;(_tmp28C.ReachableFL).f2=_tmp28B;(_tmp28C.ReachableFL).f3=_tmp288;
_tmp28C;});goto _LL1AD;}_LL1B0:;_LL1B1: goto _LL1AD;_LL1AD:;}{void*_tmp28D=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp166->topt))->v);struct Cyc_List_List*
_tmp28E;struct Cyc_Absyn_PtrInfo _tmp28F;struct Cyc_Absyn_PtrAtts _tmp290;struct Cyc_Absyn_Conref*
_tmp291;_LL1B3: if(_tmp28D <= (void*)4)goto _LL1B7;if(*((int*)_tmp28D)!= 9)goto
_LL1B5;_tmp28E=((struct Cyc_Absyn_TupleType_struct*)_tmp28D)->f1;_LL1B4: {void*
_tmp292=(void*)((struct Cyc_List_List*)_check_null(_tmp283))->hd;struct
_dynforward_ptr _tmp293;_LL1BA: if(_tmp292 <= (void*)3)goto _LL1BC;if(*((int*)
_tmp292)!= 4)goto _LL1BC;_tmp293=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp292)->f1;
_LL1BB: _tmp284=Cyc_NewControlFlow_may_consume_exp(env,_tmp284,e);{unsigned int i=(
Cyc_Evexp_eval_const_uint_exp(_tmp167)).f1;struct _tuple5 _tmp295=({struct _tuple5
_tmp294;_tmp294.f1=_tmp284;_tmp294.f2=*((void**)_check_dynforward_subscript(
_tmp293,sizeof(void*),(int)i));_tmp294;});_npop_handler(0);return _tmp295;}_LL1BC:;
_LL1BD:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp296=_cycalloc(
sizeof(*_tmp296));_tmp296[0]=({struct Cyc_Core_Impossible_struct _tmp297;_tmp297.tag=
Cyc_Core_Impossible;_tmp297.f1=({const char*_tmp298="anal_Rexp: Subscript";
_tag_dynforward(_tmp298,sizeof(char),_get_zero_arr_size_char(_tmp298,21));});
_tmp297;});_tmp296;}));_LL1B9:;}_LL1B5: if(*((int*)_tmp28D)!= 4)goto _LL1B7;
_tmp28F=((struct Cyc_Absyn_PointerType_struct*)_tmp28D)->f1;_tmp290=_tmp28F.ptr_atts;
_tmp291=_tmp290.bounds;_LL1B6:{union Cyc_Absyn_Constraint_union _tmp299=(Cyc_Absyn_compress_conref(
_tmp291))->v;void*_tmp29A;_LL1BF: if((_tmp299.Eq_constr).tag != 0)goto _LL1C1;
_tmp29A=(_tmp299.Eq_constr).f1;if(_tmp29A <= (void*)2)goto _LL1C1;if(*((int*)
_tmp29A)!= 1)goto _LL1C1;_LL1C0:{void*_tmp29B=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp283))->tl))->hd;struct Cyc_List_List*
_tmp29C;_LL1C4: if(_tmp29B <= (void*)3)goto _LL1C6;if(*((int*)_tmp29B)!= 3)goto
_LL1C6;_tmp29C=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp29B)->f1;_LL1C5:(void*)(
_tmp167->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp29D=
_cycalloc(sizeof(*_tmp29D));_tmp29D[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp29E;_tmp29E.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp29E.f1=Cyc_CfFlowInfo_copy_tagcmps(
Cyc_Core_heap_region,_tmp29C);_tmp29E;});_tmp29D;})));goto _LL1C3;_LL1C6:;_LL1C7:
goto _LL1C3;_LL1C3:;}goto _LL1BE;_LL1C1:;_LL1C2: goto _LL1BE;_LL1BE:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2A0;void*_tmp2A1;struct _tuple5 _tmp29F=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp282,_tmp166,(void*)((struct Cyc_List_List*)_check_null(_tmp283))->hd);
_tmp2A0=_tmp29F.f1;_tmp2A1=_tmp29F.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp2A2=
_tmp2A0;_LL1C9: if((_tmp2A2.BottomFL).tag != 0)goto _LL1CB;_LL1CA: {struct _tuple5
_tmp2A4=({struct _tuple5 _tmp2A3;_tmp2A3.f1=_tmp2A0;_tmp2A3.f2=_tmp2A1;_tmp2A3;});
_npop_handler(0);return _tmp2A4;}_LL1CB:;_LL1CC: {struct _tuple5 _tmp2A6=({struct
_tuple5 _tmp2A5;_tmp2A5.f1=_tmp284;_tmp2A5.f2=_tmp2A1;_tmp2A5;});_npop_handler(0);
return _tmp2A6;}_LL1C8:;}}_LL1B7:;_LL1B8:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2A7=_cycalloc(sizeof(*_tmp2A7));_tmp2A7[0]=({struct Cyc_Core_Impossible_struct
_tmp2A8;_tmp2A8.tag=Cyc_Core_Impossible;_tmp2A8.f1=({const char*_tmp2A9="anal_Rexp: Subscript -- bad type";
_tag_dynforward(_tmp2A9,sizeof(char),_get_zero_arr_size_char(_tmp2A9,33));});
_tmp2A8;});_tmp2A7;}));_LL1B2:;}}};_pop_region(temp);}_LL123: if(*((int*)_tmp12D)
!= 32)goto _LL125;_tmp168=((struct Cyc_Absyn_Tunion_e_struct*)_tmp12D)->f1;_tmp169=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp12D)->f2;_LL124: if(_tmp169->is_flat){struct
_RegionHandle _tmp2AA=_new_region("temp");struct _RegionHandle*temp=& _tmp2AA;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2AC;struct Cyc_List_List*
_tmp2AD;struct _tuple11 _tmp2AB=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,_tmp168,0);_tmp2AC=_tmp2AB.f1;_tmp2AD=_tmp2AB.f2;_tmp2AC=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2AC);for(0;(unsigned int)_tmp168;(_tmp168=_tmp168->tl,_tmp2AD=_tmp2AD->tl)){
_tmp2AC=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)_tmp168->hd)->loc,
_tmp2AC,(void*)((struct Cyc_List_List*)_check_null(_tmp2AD))->hd);}{struct _tuple5
_tmp2AF=({struct _tuple5 _tmp2AE;_tmp2AE.f1=_tmp2AC;_tmp2AE.f2=(void*)_tmp126->unknown_all;
_tmp2AE;});_npop_handler(0);return _tmp2AF;}};_pop_region(temp);}_tmp16A=_tmp168;
goto _LL126;_LL125: if(*((int*)_tmp12D)!= 26)goto _LL127;_tmp16A=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp12D)->f1;_LL126: {struct _RegionHandle _tmp2B0=_new_region("temp");struct
_RegionHandle*temp=& _tmp2B0;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2B2;struct Cyc_List_List*_tmp2B3;struct _tuple11 _tmp2B1=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp16A,0);_tmp2B2=_tmp2B1.f1;_tmp2B3=_tmp2B1.f2;_tmp2B2=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2B2);{struct _dynforward_ptr aggrdict=({unsigned int _tmp2B8=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp16A);void**
_tmp2B9=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp2B8));
struct _dynforward_ptr _tmp2BC=_tag_dynforward(_tmp2B9,sizeof(void*),_tmp2B8);{
unsigned int _tmp2BA=_tmp2B8;unsigned int i;for(i=0;i < _tmp2BA;i ++){_tmp2B9[i]=({
void*_tmp2BB=(void*)((struct Cyc_List_List*)_check_null(_tmp2B3))->hd;_tmp2B3=
_tmp2B3->tl;_tmp2BB;});}}_tmp2BC;});struct _tuple5 _tmp2B7=({struct _tuple5 _tmp2B4;
_tmp2B4.f1=_tmp2B2;_tmp2B4.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2B5=_region_malloc(env->r,sizeof(*_tmp2B5));_tmp2B5[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp2B6;_tmp2B6.tag=4;_tmp2B6.f1=aggrdict;_tmp2B6;});_tmp2B5;});_tmp2B4;});
_npop_handler(0);return _tmp2B7;}};_pop_region(temp);}_LL127: if(*((int*)_tmp12D)
!= 31)goto _LL129;_tmp16B=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp12D)->f2;
_LL128: {struct Cyc_List_List*fs;{void*_tmp2BD=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_List_List*_tmp2BE;_LL1CE:
if(_tmp2BD <= (void*)4)goto _LL1D0;if(*((int*)_tmp2BD)!= 11)goto _LL1D0;_tmp2BE=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp2BD)->f2;_LL1CF: fs=_tmp2BE;goto _LL1CD;
_LL1D0:;_LL1D1:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp2BF=
_cycalloc(sizeof(*_tmp2BF));_tmp2BF[0]=({struct Cyc_Core_Impossible_struct _tmp2C0;
_tmp2C0.tag=Cyc_Core_Impossible;_tmp2C0.f1=({const char*_tmp2C1="anal_Rexp:anon struct has bad type";
_tag_dynforward(_tmp2C1,sizeof(char),_get_zero_arr_size_char(_tmp2C1,35));});
_tmp2C0;});_tmp2BF;}));_LL1CD:;}_tmp16C=_tmp16B;_tmp171=fs;goto _LL12A;}_LL129:
if(*((int*)_tmp12D)!= 30)goto _LL12B;_tmp16C=((struct Cyc_Absyn_Struct_e_struct*)
_tmp12D)->f3;_tmp16D=((struct Cyc_Absyn_Struct_e_struct*)_tmp12D)->f4;if(_tmp16D
== 0)goto _LL12B;_tmp16E=*_tmp16D;_tmp16F=_tmp16E.impl;if(_tmp16F == 0)goto _LL12B;
_tmp170=*_tmp16F;_tmp171=_tmp170.fields;_LL12A: {struct _RegionHandle _tmp2C2=
_new_region("temp");struct _RegionHandle*temp=& _tmp2C2;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp2C4;struct Cyc_List_List*_tmp2C5;struct _tuple11
_tmp2C3=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple14*),struct Cyc_List_List*
x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple14*))Cyc_Core_snd,
_tmp16C),0);_tmp2C4=_tmp2C3.f1;_tmp2C5=_tmp2C3.f2;_tmp2C4=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2C4);{struct _dynforward_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(
_tmp126,_tmp171,(void*)_tmp126->unknown_all);{int i=0;for(0;_tmp2C5 != 0;(((
_tmp2C5=_tmp2C5->tl,_tmp16C=_tmp16C->tl)),++ i)){struct Cyc_List_List*ds=(*((
struct _tuple14*)((struct Cyc_List_List*)_check_null(_tmp16C))->hd)).f1;for(0;ds != 
0;ds=ds->tl){void*_tmp2C6=(void*)ds->hd;struct _dynforward_ptr*_tmp2C7;_LL1D3: if(*((
int*)_tmp2C6)!= 0)goto _LL1D5;_LL1D4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2C8=_cycalloc(sizeof(*_tmp2C8));_tmp2C8[0]=({struct Cyc_Core_Impossible_struct
_tmp2C9;_tmp2C9.tag=Cyc_Core_Impossible;_tmp2C9.f1=({const char*_tmp2CA="anal_Rexp:Struct_e";
_tag_dynforward(_tmp2CA,sizeof(char),_get_zero_arr_size_char(_tmp2CA,19));});
_tmp2C9;});_tmp2C8;}));_LL1D5: if(*((int*)_tmp2C6)!= 1)goto _LL1D2;_tmp2C7=((
struct Cyc_Absyn_FieldName_struct*)_tmp2C6)->f1;_LL1D6: {int _tmp2CB=Cyc_NewControlFlow_get_field_index_fs(
_tmp171,_tmp2C7);*((void**)_check_dynforward_subscript(aggrdict,sizeof(void*),
_tmp2CB))=(void*)_tmp2C5->hd;}_LL1D2:;}}}{struct _tuple5 _tmp2CF=({struct _tuple5
_tmp2CC;_tmp2CC.f1=_tmp2C4;_tmp2CC.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2CD=_region_malloc(env->r,sizeof(*_tmp2CD));_tmp2CD[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp2CE;_tmp2CE.tag=4;_tmp2CE.f1=aggrdict;_tmp2CE;});_tmp2CD;});_tmp2CC;});
_npop_handler(0);return _tmp2CF;}}};_pop_region(temp);}_LL12B: if(*((int*)_tmp12D)
!= 30)goto _LL12D;_LL12C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2D0=_cycalloc(sizeof(*_tmp2D0));_tmp2D0[0]=({struct Cyc_Core_Impossible_struct
_tmp2D1;_tmp2D1.tag=Cyc_Core_Impossible;_tmp2D1.f1=({const char*_tmp2D2="anal_Rexp:missing aggrdeclimpl";
_tag_dynforward(_tmp2D2,sizeof(char),_get_zero_arr_size_char(_tmp2D2,31));});
_tmp2D1;});_tmp2D0;}));_LL12D: if(*((int*)_tmp12D)!= 28)goto _LL12F;_tmp172=((
struct Cyc_Absyn_Array_e_struct*)_tmp12D)->f1;_LL12E: {struct _RegionHandle _tmp2D3=
_new_region("temp");struct _RegionHandle*temp=& _tmp2D3;_push_region(temp);{struct
Cyc_List_List*_tmp2D4=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple14*))Cyc_Core_snd,_tmp172);union Cyc_CfFlowInfo_FlowInfo_union _tmp2D6;
struct Cyc_List_List*_tmp2D7;struct _tuple11 _tmp2D5=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp2D4,0);_tmp2D6=_tmp2D5.f1;_tmp2D7=_tmp2D5.f2;_tmp2D6=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2D6);for(0;_tmp2D7 != 0;(_tmp2D7=_tmp2D7->tl,_tmp2D4=_tmp2D4->tl)){
_tmp2D6=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2D4))->hd)->loc,_tmp2D6,(void*)_tmp2D7->hd);}{struct _tuple5
_tmp2D9=({struct _tuple5 _tmp2D8;_tmp2D8.f1=_tmp2D6;_tmp2D8.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp126,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp126->unknown_all);
_tmp2D8;});_npop_handler(0);return _tmp2D9;}};_pop_region(temp);}_LL12F: if(*((int*)
_tmp12D)!= 29)goto _LL131;_tmp173=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp12D)->f1;_tmp174=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp12D)->f2;
_tmp175=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp12D)->f3;_tmp176=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp12D)->f4;_LL130: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp2DB;void*_tmp2DC;struct _tuple5 _tmp2DA=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp174);_tmp2DB=_tmp2DA.f1;_tmp2DC=_tmp2DA.f2;_tmp2DB=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp174->loc,_tmp2DB);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2DD=_tmp2DB;struct
Cyc_Dict_Dict _tmp2DE;struct Cyc_List_List*_tmp2DF;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2E0;_LL1D8: if((_tmp2DD.BottomFL).tag != 0)goto _LL1DA;_LL1D9: return({struct
_tuple5 _tmp2E1;_tmp2E1.f1=_tmp2DB;_tmp2E1.f2=(void*)_tmp126->unknown_all;_tmp2E1;});
_LL1DA: if((_tmp2DD.ReachableFL).tag != 1)goto _LL1D7;_tmp2DE=(_tmp2DD.ReachableFL).f1;
_tmp2DF=(_tmp2DD.ReachableFL).f2;_tmp2E0=(_tmp2DD.ReachableFL).f3;_LL1DB: if(Cyc_CfFlowInfo_initlevel(
_tmp2DE,_tmp2DC)== (void*)0)({void*_tmp2E2=0;Cyc_Tcutil_terr(_tmp174->loc,({
const char*_tmp2E3="expression may not be initialized";_tag_dynforward(_tmp2E3,
sizeof(char),_get_zero_arr_size_char(_tmp2E3,34));}),_tag_dynforward(_tmp2E2,
sizeof(void*),0));});{struct Cyc_List_List*new_relns=_tmp2DF;comp_loop: {void*
_tmp2E4=(void*)_tmp174->r;struct Cyc_Absyn_Exp*_tmp2E5;void*_tmp2E6;struct Cyc_Absyn_Vardecl*
_tmp2E7;void*_tmp2E8;struct Cyc_Absyn_Vardecl*_tmp2E9;void*_tmp2EA;struct Cyc_Absyn_Vardecl*
_tmp2EB;void*_tmp2EC;struct Cyc_Absyn_Vardecl*_tmp2ED;union Cyc_Absyn_Cnst_union
_tmp2EE;int _tmp2EF;void*_tmp2F0;struct Cyc_List_List*_tmp2F1;struct Cyc_List_List
_tmp2F2;struct Cyc_Absyn_Exp*_tmp2F3;_LL1DD: if(*((int*)_tmp2E4)!= 15)goto _LL1DF;
_tmp2E5=((struct Cyc_Absyn_Cast_e_struct*)_tmp2E4)->f2;_LL1DE: _tmp174=_tmp2E5;
goto comp_loop;_LL1DF: if(*((int*)_tmp2E4)!= 1)goto _LL1E1;_tmp2E6=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp2E4)->f2;if(_tmp2E6 <= (void*)1)goto _LL1E1;if(*((int*)
_tmp2E6)!= 0)goto _LL1E1;_tmp2E7=((struct Cyc_Absyn_Global_b_struct*)_tmp2E6)->f1;
if(!(!_tmp2E7->escapes))goto _LL1E1;_LL1E0: _tmp2E9=_tmp2E7;goto _LL1E2;_LL1E1: if(*((
int*)_tmp2E4)!= 1)goto _LL1E3;_tmp2E8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2E4)->f2;if(_tmp2E8 <= (void*)1)goto _LL1E3;if(*((int*)_tmp2E8)!= 3)goto _LL1E3;
_tmp2E9=((struct Cyc_Absyn_Local_b_struct*)_tmp2E8)->f1;if(!(!_tmp2E9->escapes))
goto _LL1E3;_LL1E2: _tmp2EB=_tmp2E9;goto _LL1E4;_LL1E3: if(*((int*)_tmp2E4)!= 1)goto
_LL1E5;_tmp2EA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2E4)->f2;if(_tmp2EA <= (
void*)1)goto _LL1E5;if(*((int*)_tmp2EA)!= 4)goto _LL1E5;_tmp2EB=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2EA)->f1;if(!(!_tmp2EB->escapes))goto _LL1E5;_LL1E4: _tmp2ED=_tmp2EB;goto
_LL1E6;_LL1E5: if(*((int*)_tmp2E4)!= 1)goto _LL1E7;_tmp2EC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2E4)->f2;if(_tmp2EC <= (void*)1)goto _LL1E7;if(*((int*)_tmp2EC)!= 2)goto _LL1E7;
_tmp2ED=((struct Cyc_Absyn_Param_b_struct*)_tmp2EC)->f1;if(!(!_tmp2ED->escapes))
goto _LL1E7;_LL1E6: new_relns=({struct Cyc_List_List*_tmp2F4=_region_malloc(env->r,
sizeof(*_tmp2F4));_tmp2F4->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2F5=_region_malloc(
env->r,sizeof(*_tmp2F5));_tmp2F5->vd=_tmp173;_tmp2F5->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp2F6;(_tmp2F6.LessVar).tag=1;(_tmp2F6.LessVar).f1=
_tmp2ED;_tmp2F6;});_tmp2F5;});_tmp2F4->tl=_tmp2DF;_tmp2F4;});goto _LL1DC;_LL1E7:
if(*((int*)_tmp2E4)!= 0)goto _LL1E9;_tmp2EE=((struct Cyc_Absyn_Const_e_struct*)
_tmp2E4)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp2E4)->f1).Int_c).tag != 2)
goto _LL1E9;_tmp2EF=(_tmp2EE.Int_c).f2;_LL1E8: new_relns=({struct Cyc_List_List*
_tmp2F7=_region_malloc(env->r,sizeof(*_tmp2F7));_tmp2F7->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp2F8=_region_malloc(env->r,sizeof(*_tmp2F8));_tmp2F8->vd=_tmp173;_tmp2F8->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp2F9;(
_tmp2F9.LessConst).tag=3;(_tmp2F9.LessConst).f1=(unsigned int)_tmp2EF;_tmp2F9;});
_tmp2F8;});_tmp2F7->tl=_tmp2DF;_tmp2F7;});goto _LL1DC;_LL1E9: if(*((int*)_tmp2E4)
!= 3)goto _LL1EB;_tmp2F0=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2E4)->f1;
if((int)_tmp2F0 != 19)goto _LL1EB;_tmp2F1=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2E4)->f2;if(_tmp2F1 == 0)goto _LL1EB;_tmp2F2=*_tmp2F1;_tmp2F3=(struct Cyc_Absyn_Exp*)
_tmp2F2.hd;_LL1EA:{void*_tmp2FA=(void*)_tmp2F3->r;void*_tmp2FB;struct Cyc_Absyn_Vardecl*
_tmp2FC;void*_tmp2FD;struct Cyc_Absyn_Vardecl*_tmp2FE;void*_tmp2FF;struct Cyc_Absyn_Vardecl*
_tmp300;void*_tmp301;struct Cyc_Absyn_Vardecl*_tmp302;_LL1EE: if(*((int*)_tmp2FA)
!= 1)goto _LL1F0;_tmp2FB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2FA)->f2;if(
_tmp2FB <= (void*)1)goto _LL1F0;if(*((int*)_tmp2FB)!= 0)goto _LL1F0;_tmp2FC=((
struct Cyc_Absyn_Global_b_struct*)_tmp2FB)->f1;if(!(!_tmp2FC->escapes))goto _LL1F0;
_LL1EF: _tmp2FE=_tmp2FC;goto _LL1F1;_LL1F0: if(*((int*)_tmp2FA)!= 1)goto _LL1F2;
_tmp2FD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2FA)->f2;if(_tmp2FD <= (void*)
1)goto _LL1F2;if(*((int*)_tmp2FD)!= 3)goto _LL1F2;_tmp2FE=((struct Cyc_Absyn_Local_b_struct*)
_tmp2FD)->f1;if(!(!_tmp2FE->escapes))goto _LL1F2;_LL1F1: _tmp300=_tmp2FE;goto
_LL1F3;_LL1F2: if(*((int*)_tmp2FA)!= 1)goto _LL1F4;_tmp2FF=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2FA)->f2;if(_tmp2FF <= (void*)1)goto _LL1F4;if(*((int*)_tmp2FF)!= 4)goto _LL1F4;
_tmp300=((struct Cyc_Absyn_Pat_b_struct*)_tmp2FF)->f1;if(!(!_tmp300->escapes))
goto _LL1F4;_LL1F3: _tmp302=_tmp300;goto _LL1F5;_LL1F4: if(*((int*)_tmp2FA)!= 1)goto
_LL1F6;_tmp301=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2FA)->f2;if(_tmp301 <= (
void*)1)goto _LL1F6;if(*((int*)_tmp301)!= 2)goto _LL1F6;_tmp302=((struct Cyc_Absyn_Param_b_struct*)
_tmp301)->f1;if(!(!_tmp302->escapes))goto _LL1F6;_LL1F5: new_relns=({struct Cyc_List_List*
_tmp303=_region_malloc(env->r,sizeof(*_tmp303));_tmp303->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp304=_region_malloc(env->r,sizeof(*_tmp304));_tmp304->vd=_tmp173;_tmp304->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp305;(
_tmp305.LessSize).tag=2;(_tmp305.LessSize).f1=_tmp302;_tmp305;});_tmp304;});
_tmp303->tl=_tmp2DF;_tmp303;});goto _LL1ED;_LL1F6:;_LL1F7: goto _LL1ED;_LL1ED:;}
goto _LL1DC;_LL1EB:;_LL1EC: goto _LL1DC;_LL1DC:;}if(_tmp2DF != new_relns)_tmp2DB=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp306;(
_tmp306.ReachableFL).tag=1;(_tmp306.ReachableFL).f1=_tmp2DE;(_tmp306.ReachableFL).f2=
new_relns;(_tmp306.ReachableFL).f3=_tmp2E0;_tmp306;});{void*_tmp307=_tmp2DC;
_LL1F9: if((int)_tmp307 != 0)goto _LL1FB;_LL1FA: return({struct _tuple5 _tmp308;
_tmp308.f1=_tmp2DB;_tmp308.f2=(void*)_tmp126->unknown_all;_tmp308;});_LL1FB: if((
int)_tmp307 != 2)goto _LL1FD;_LL1FC: goto _LL1FE;_LL1FD: if((int)_tmp307 != 1)goto
_LL1FF;_LL1FE: goto _LL200;_LL1FF: if(_tmp307 <= (void*)3)goto _LL201;if(*((int*)
_tmp307)!= 2)goto _LL201;_LL200: {struct Cyc_List_List _tmp309=({struct Cyc_List_List
_tmp315;_tmp315.hd=_tmp173;_tmp315.tl=0;_tmp315;});_tmp2DB=Cyc_NewControlFlow_add_vars(
_tmp126,_tmp2DB,(struct Cyc_List_List*)& _tmp309,(void*)_tmp126->unknown_all,e->loc);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp30B;void*_tmp30C;struct _tuple5 _tmp30A=Cyc_NewControlFlow_anal_Rexp(
env,_tmp2DB,_tmp175);_tmp30B=_tmp30A.f1;_tmp30C=_tmp30A.f2;_tmp30B=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp175->loc,_tmp30B);{union Cyc_CfFlowInfo_FlowInfo_union _tmp30D=_tmp30B;struct
Cyc_Dict_Dict _tmp30E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp30F;_LL204: if((_tmp30D.BottomFL).tag
!= 0)goto _LL206;_LL205: return({struct _tuple5 _tmp310;_tmp310.f1=_tmp30B;_tmp310.f2=(
void*)_tmp126->unknown_all;_tmp310;});_LL206: if((_tmp30D.ReachableFL).tag != 1)
goto _LL203;_tmp30E=(_tmp30D.ReachableFL).f1;_tmp30F=(_tmp30D.ReachableFL).f3;
_LL207: if(Cyc_CfFlowInfo_initlevel(_tmp30E,_tmp30C)!= (void*)2){({void*_tmp311=0;
Cyc_Tcutil_terr(_tmp174->loc,({const char*_tmp312="expression may not be initialized";
_tag_dynforward(_tmp312,sizeof(char),_get_zero_arr_size_char(_tmp312,34));}),
_tag_dynforward(_tmp311,sizeof(void*),0));});return({struct _tuple5 _tmp313;
_tmp313.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp314;(_tmp314.BottomFL).tag=0;_tmp314;});_tmp313.f2=(void*)_tmp126->unknown_all;
_tmp313;});}_LL203:;}_tmp2DB=_tmp30B;goto _LL202;}}_LL201:;_LL202: while(1){struct
Cyc_List_List _tmp316=({struct Cyc_List_List _tmp322;_tmp322.hd=_tmp173;_tmp322.tl=
0;_tmp322;});_tmp2DB=Cyc_NewControlFlow_add_vars(_tmp126,_tmp2DB,(struct Cyc_List_List*)&
_tmp316,(void*)_tmp126->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp318;void*_tmp319;struct _tuple5 _tmp317=Cyc_NewControlFlow_anal_Rexp(env,
_tmp2DB,_tmp175);_tmp318=_tmp317.f1;_tmp319=_tmp317.f2;_tmp318=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp175->loc,_tmp318);{union Cyc_CfFlowInfo_FlowInfo_union _tmp31A=_tmp318;struct
Cyc_Dict_Dict _tmp31B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp31C;_LL209: if((_tmp31A.BottomFL).tag
!= 0)goto _LL20B;_LL20A: goto _LL208;_LL20B: if((_tmp31A.ReachableFL).tag != 1)goto
_LL208;_tmp31B=(_tmp31A.ReachableFL).f1;_tmp31C=(_tmp31A.ReachableFL).f3;_LL20C:
if(Cyc_CfFlowInfo_initlevel(_tmp31B,_tmp319)!= (void*)2){({void*_tmp31D=0;Cyc_Tcutil_terr(
_tmp174->loc,({const char*_tmp31E="expression may not be initialized";
_tag_dynforward(_tmp31E,sizeof(char),_get_zero_arr_size_char(_tmp31E,34));}),
_tag_dynforward(_tmp31D,sizeof(void*),0));});return({struct _tuple5 _tmp31F;
_tmp31F.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp320;(_tmp320.BottomFL).tag=0;_tmp320;});_tmp31F.f2=(void*)_tmp126->unknown_all;
_tmp31F;});}_LL208:;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp321=Cyc_CfFlowInfo_join_flow(
_tmp126,env->all_changed,_tmp2DB,_tmp318,1);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp321,_tmp2DB))break;_tmp2DB=_tmp321;}}}return({struct _tuple5 _tmp323;_tmp323.f1=
_tmp2DB;_tmp323.f2=(void*)_tmp126->unknown_all;_tmp323;});_LL1F8:;}}_LL1D7:;}}
_LL131: if(*((int*)_tmp12D)!= 38)goto _LL133;_tmp177=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp12D)->f1;_LL132: while(1){union Cyc_CfFlowInfo_FlowInfo_union*_tmp325;struct
_tuple9 _tmp324=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp177);_tmp325=
_tmp324.f2;inflow=*_tmp325;{void*_tmp326=(void*)_tmp177->r;struct Cyc_Absyn_Stmt*
_tmp327;struct Cyc_Absyn_Stmt*_tmp328;struct Cyc_Absyn_Decl*_tmp329;struct Cyc_Absyn_Stmt*
_tmp32A;struct Cyc_Absyn_Exp*_tmp32B;_LL20E: if(_tmp326 <= (void*)1)goto _LL214;if(*((
int*)_tmp326)!= 1)goto _LL210;_tmp327=((struct Cyc_Absyn_Seq_s_struct*)_tmp326)->f1;
_tmp328=((struct Cyc_Absyn_Seq_s_struct*)_tmp326)->f2;_LL20F: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp327);_tmp177=_tmp328;goto _LL20D;_LL210: if(*((int*)_tmp326)!= 11)
goto _LL212;_tmp329=((struct Cyc_Absyn_Decl_s_struct*)_tmp326)->f1;_tmp32A=((
struct Cyc_Absyn_Decl_s_struct*)_tmp326)->f2;_LL211: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp329);_tmp177=_tmp32A;goto _LL20D;_LL212: if(*((int*)_tmp326)!= 0)
goto _LL214;_tmp32B=((struct Cyc_Absyn_Exp_s_struct*)_tmp326)->f1;_LL213: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp32B);_LL214:;_LL215:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp32C=_cycalloc(sizeof(*_tmp32C));_tmp32C[0]=({struct Cyc_Core_Impossible_struct
_tmp32D;_tmp32D.tag=Cyc_Core_Impossible;_tmp32D.f1=({const char*_tmp32E="analyze_Rexp: ill-formed StmtExp";
_tag_dynforward(_tmp32E,sizeof(char),_get_zero_arr_size_char(_tmp32E,33));});
_tmp32D;});_tmp32C;}));_LL20D:;}}_LL133: if(*((int*)_tmp12D)!= 1)goto _LL135;
_tmp178=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp12D)->f2;if((int)_tmp178 != 0)
goto _LL135;_LL134: goto _LL136;_LL135: if(*((int*)_tmp12D)!= 2)goto _LL137;_LL136:
goto _LL138;_LL137: if(*((int*)_tmp12D)!= 10)goto _LL139;_LL138: goto _LL13A;_LL139:
if(*((int*)_tmp12D)!= 37)goto _LL13B;_LL13A: goto _LL13C;_LL13B: if(*((int*)_tmp12D)
!= 27)goto _LLD6;_LL13C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp32F=_cycalloc(sizeof(*_tmp32F));_tmp32F[0]=({struct Cyc_Core_Impossible_struct
_tmp330;_tmp330.tag=Cyc_Core_Impossible;_tmp330.f1=({const char*_tmp331="anal_Rexp, unexpected exp form";
_tag_dynforward(_tmp331,sizeof(char),_get_zero_arr_size_char(_tmp331,31));});
_tmp330;});_tmp32F;}));_LLD6:;}}static struct _tuple7 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
union Cyc_CfFlowInfo_FlowInfo_union f,struct Cyc_Absyn_Exp*e,void*r,struct Cyc_List_List*
flds){struct Cyc_CfFlowInfo_FlowEnv*_tmp332=env->fenv;void*_tmp333=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp334;void*_tmp335;struct Cyc_Absyn_PtrAtts _tmp336;struct Cyc_Absyn_Conref*
_tmp337;struct Cyc_Absyn_Conref*_tmp338;_LL217: if(_tmp333 <= (void*)4)goto _LL219;
if(*((int*)_tmp333)!= 4)goto _LL219;_tmp334=((struct Cyc_Absyn_PointerType_struct*)
_tmp333)->f1;_tmp335=(void*)_tmp334.elt_typ;_tmp336=_tmp334.ptr_atts;_tmp337=
_tmp336.bounds;_tmp338=_tmp336.zero_term;_LL218: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp339=f;struct Cyc_Dict_Dict _tmp33A;struct Cyc_List_List*_tmp33B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp33C;_LL21C: if((_tmp339.BottomFL).tag != 0)goto _LL21E;_LL21D: return({struct
_tuple7 _tmp33D;_tmp33D.f1=f;_tmp33D.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp33E;(_tmp33E.UnknownL).tag=1;_tmp33E;});
_tmp33D;});_LL21E: if((_tmp339.ReachableFL).tag != 1)goto _LL21B;_tmp33A=(_tmp339.ReachableFL).f1;
_tmp33B=(_tmp339.ReachableFL).f2;_tmp33C=(_tmp339.ReachableFL).f3;_LL21F: if(Cyc_Tcutil_is_bound_one(
_tmp337)){void*_tmp33F=r;struct Cyc_CfFlowInfo_Place*_tmp340;struct Cyc_CfFlowInfo_Place
_tmp341;void*_tmp342;struct Cyc_List_List*_tmp343;void*_tmp344;_LL221: if((int)
_tmp33F != 1)goto _LL223;_LL222: goto _LL224;_LL223: if((int)_tmp33F != 2)goto _LL225;
_LL224:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp345=_cycalloc(sizeof(*_tmp345));_tmp345[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp346;_tmp346.tag=Cyc_CfFlowInfo_NotZero;_tmp346.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp33B);_tmp346;});_tmp345;})));goto _LL220;_LL225: if(
_tmp33F <= (void*)3)goto _LL227;if(*((int*)_tmp33F)!= 2)goto _LL227;_tmp340=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp33F)->f1;_tmp341=*_tmp340;_tmp342=(
void*)_tmp341.root;_tmp343=_tmp341.fields;_LL226:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_NotZero_struct*_tmp347=_cycalloc(sizeof(*_tmp347));_tmp347[
0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp348;_tmp348.tag=Cyc_CfFlowInfo_NotZero;
_tmp348.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp33B);_tmp348;});
_tmp347;})));return({struct _tuple7 _tmp349;_tmp349.f1=f;_tmp349.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp34A;(_tmp34A.PlaceL).tag=0;(_tmp34A.PlaceL).f1=({
struct Cyc_CfFlowInfo_Place*_tmp34B=_region_malloc(_tmp332->r,sizeof(*_tmp34B));
_tmp34B->root=(void*)_tmp342;_tmp34B->fields=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp332->r,_tmp343,flds);_tmp34B;});_tmp34A;});_tmp349;});_LL227: if((int)_tmp33F
!= 0)goto _LL229;_LL228:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));
return({struct _tuple7 _tmp34C;_tmp34C.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp34E;(_tmp34E.BottomFL).tag=0;_tmp34E;});
_tmp34C.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp34D;(_tmp34D.UnknownL).tag=1;_tmp34D;});_tmp34C;});_LL229: if(_tmp33F <= (void*)
3)goto _LL22B;if(*((int*)_tmp33F)!= 0)goto _LL22B;_tmp344=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp33F)->f1;_LL22A: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp344);goto
_LL22C;_LL22B:;_LL22C:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp34F=_cycalloc(sizeof(*_tmp34F));_tmp34F[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp350;_tmp350.tag=Cyc_CfFlowInfo_UnknownZ;_tmp350.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp33B);_tmp350;});_tmp34F;})));_LL220:;}else{(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp351=_cycalloc(sizeof(*
_tmp351));_tmp351[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp352;_tmp352.tag=
Cyc_CfFlowInfo_UnknownZ;_tmp352.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp33B);_tmp352;});_tmp351;})));}if(Cyc_CfFlowInfo_initlevel(_tmp33A,r)== (void*)
0)({void*_tmp353=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp354="dereference of possibly uninitialized pointer";
_tag_dynforward(_tmp354,sizeof(char),_get_zero_arr_size_char(_tmp354,46));}),
_tag_dynforward(_tmp353,sizeof(void*),0));});return({struct _tuple7 _tmp355;
_tmp355.f1=f;_tmp355.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp356;(_tmp356.UnknownL).tag=1;_tmp356;});_tmp355;});_LL21B:;}_LL219:;_LL21A:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp357=_cycalloc(sizeof(*
_tmp357));_tmp357[0]=({struct Cyc_Core_Impossible_struct _tmp358;_tmp358.tag=Cyc_Core_Impossible;
_tmp358.f1=({const char*_tmp359="left deref of non-pointer-type";_tag_dynforward(
_tmp359,sizeof(char),_get_zero_arr_size_char(_tmp359,31));});_tmp358;});_tmp357;}));
_LL216:;}static struct _tuple7 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*_tmp35A=env->fenv;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp35B=inflow;struct Cyc_Dict_Dict _tmp35C;struct Cyc_List_List*
_tmp35D;_LL22E: if((_tmp35B.BottomFL).tag != 0)goto _LL230;_LL22F: return({struct
_tuple7 _tmp35E;_tmp35E.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp360;(_tmp360.BottomFL).tag=0;_tmp360;});_tmp35E.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp35F;(_tmp35F.UnknownL).tag=1;_tmp35F;});
_tmp35E;});_LL230: if((_tmp35B.ReachableFL).tag != 1)goto _LL22D;_tmp35C=(_tmp35B.ReachableFL).f1;
_tmp35D=(_tmp35B.ReachableFL).f2;_LL231: d=_tmp35C;_LL22D:;}{void*_tmp361=(void*)
e->r;struct Cyc_Absyn_Exp*_tmp362;struct Cyc_Absyn_Exp*_tmp363;struct Cyc_Absyn_Exp*
_tmp364;void*_tmp365;struct Cyc_Absyn_Vardecl*_tmp366;void*_tmp367;struct Cyc_Absyn_Vardecl*
_tmp368;void*_tmp369;struct Cyc_Absyn_Vardecl*_tmp36A;void*_tmp36B;struct Cyc_Absyn_Vardecl*
_tmp36C;struct Cyc_Absyn_Exp*_tmp36D;struct _dynforward_ptr*_tmp36E;struct Cyc_Absyn_Exp*
_tmp36F;struct Cyc_Absyn_Exp*_tmp370;struct Cyc_Absyn_Exp*_tmp371;struct Cyc_Absyn_Exp*
_tmp372;struct _dynforward_ptr*_tmp373;_LL233: if(*((int*)_tmp361)!= 15)goto _LL235;
_tmp362=((struct Cyc_Absyn_Cast_e_struct*)_tmp361)->f2;_LL234: _tmp363=_tmp362;
goto _LL236;_LL235: if(*((int*)_tmp361)!= 13)goto _LL237;_tmp363=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp361)->f1;_LL236: _tmp364=_tmp363;goto _LL238;_LL237: if(*((int*)_tmp361)!= 14)
goto _LL239;_tmp364=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp361)->f1;_LL238:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp364,flds);_LL239: if(*((int*)
_tmp361)!= 1)goto _LL23B;_tmp365=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp361)->f2;
if(_tmp365 <= (void*)1)goto _LL23B;if(*((int*)_tmp365)!= 2)goto _LL23B;_tmp366=((
struct Cyc_Absyn_Param_b_struct*)_tmp365)->f1;_LL23A: _tmp368=_tmp366;goto _LL23C;
_LL23B: if(*((int*)_tmp361)!= 1)goto _LL23D;_tmp367=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp361)->f2;if(_tmp367 <= (void*)1)goto _LL23D;if(*((int*)_tmp367)!= 3)goto _LL23D;
_tmp368=((struct Cyc_Absyn_Local_b_struct*)_tmp367)->f1;_LL23C: _tmp36A=_tmp368;
goto _LL23E;_LL23D: if(*((int*)_tmp361)!= 1)goto _LL23F;_tmp369=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp361)->f2;if(_tmp369 <= (void*)1)goto _LL23F;if(*((int*)_tmp369)!= 4)goto _LL23F;
_tmp36A=((struct Cyc_Absyn_Pat_b_struct*)_tmp369)->f1;_LL23E: return({struct
_tuple7 _tmp374;_tmp374.f1=inflow;_tmp374.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp375;(_tmp375.PlaceL).tag=0;(_tmp375.PlaceL).f1=({
struct Cyc_CfFlowInfo_Place*_tmp376=_region_malloc(env->r,sizeof(*_tmp376));
_tmp376->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp377=
_region_malloc(env->r,sizeof(*_tmp377));_tmp377[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp378;_tmp378.tag=0;_tmp378.f1=_tmp36A;_tmp378;});_tmp377;}));_tmp376->fields=
flds;_tmp376;});_tmp375;});_tmp374;});_LL23F: if(*((int*)_tmp361)!= 1)goto _LL241;
_tmp36B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp361)->f2;if(_tmp36B <= (void*)
1)goto _LL241;if(*((int*)_tmp36B)!= 0)goto _LL241;_tmp36C=((struct Cyc_Absyn_Global_b_struct*)
_tmp36B)->f1;_LL240: return({struct _tuple7 _tmp379;_tmp379.f1=inflow;_tmp379.f2=(
union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union _tmp37A;(
_tmp37A.UnknownL).tag=1;_tmp37A;});_tmp379;});_LL241: if(*((int*)_tmp361)!= 24)
goto _LL243;_tmp36D=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp361)->f1;_tmp36E=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp361)->f2;_LL242:{void*_tmp37B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp36D->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp37C;void*_tmp37D;_LL24C: if(_tmp37B <= (void*)4)goto _LL24E;if(*((int*)_tmp37B)
!= 4)goto _LL24E;_tmp37C=((struct Cyc_Absyn_PointerType_struct*)_tmp37B)->f1;
_tmp37D=(void*)_tmp37C.elt_typ;_LL24D: if(!Cyc_Absyn_is_union_type(_tmp37D))flds=({
struct Cyc_List_List*_tmp37E=_region_malloc(env->r,sizeof(*_tmp37E));_tmp37E->hd=(
void*)Cyc_NewControlFlow_get_field_index(_tmp37D,_tmp36E);_tmp37E->tl=flds;
_tmp37E;});goto _LL24B;_LL24E:;_LL24F:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp37F=_cycalloc(sizeof(*_tmp37F));_tmp37F[0]=({struct Cyc_Core_Impossible_struct
_tmp380;_tmp380.tag=Cyc_Core_Impossible;_tmp380.f1=({const char*_tmp381="anal_Rexp: AggrArrow ptr";
_tag_dynforward(_tmp381,sizeof(char),_get_zero_arr_size_char(_tmp381,25));});
_tmp380;});_tmp37F;}));_LL24B:;}_tmp36F=_tmp36D;goto _LL244;_LL243: if(*((int*)
_tmp361)!= 22)goto _LL245;_tmp36F=((struct Cyc_Absyn_Deref_e_struct*)_tmp361)->f1;
_LL244: {union Cyc_CfFlowInfo_FlowInfo_union _tmp383;void*_tmp384;struct _tuple5
_tmp382=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp36F);_tmp383=_tmp382.f1;
_tmp384=_tmp382.f2;_tmp383=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp383);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp383,_tmp36F,_tmp384,
flds);}_LL245: if(*((int*)_tmp361)!= 25)goto _LL247;_tmp370=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp361)->f1;_tmp371=((struct Cyc_Absyn_Subscript_e_struct*)_tmp361)->f2;_LL246: {
void*_tmp385=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp370->topt))->v);struct Cyc_Absyn_PtrInfo _tmp386;struct Cyc_Absyn_PtrAtts
_tmp387;struct Cyc_Absyn_Conref*_tmp388;_LL251: if(_tmp385 <= (void*)4)goto _LL255;
if(*((int*)_tmp385)!= 9)goto _LL253;_LL252: {unsigned int _tmp389=(Cyc_Evexp_eval_const_uint_exp(
_tmp371)).f1;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp370,({struct
Cyc_List_List*_tmp38A=_region_malloc(env->r,sizeof(*_tmp38A));_tmp38A->hd=(void*)((
int)_tmp389);_tmp38A->tl=flds;_tmp38A;}));}_LL253: if(*((int*)_tmp385)!= 4)goto
_LL255;_tmp386=((struct Cyc_Absyn_PointerType_struct*)_tmp385)->f1;_tmp387=
_tmp386.ptr_atts;_tmp388=_tmp387.bounds;_LL254: {struct _RegionHandle _tmp38B=
_new_region("temp");struct _RegionHandle*temp=& _tmp38B;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp38E;struct Cyc_List_List*_tmp38F;struct _tuple11
_tmp38D=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*
_tmp38C[2];_tmp38C[1]=_tmp371;_tmp38C[0]=_tmp370;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp38C,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp38E=_tmp38D.f1;_tmp38F=
_tmp38D.f2;_tmp38E=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp371->loc,_tmp38E);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp390=_tmp38E;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp391=_tmp38E;struct Cyc_Dict_Dict _tmp392;struct Cyc_List_List*_tmp393;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp394;_LL258: if((_tmp391.ReachableFL).tag != 1)goto _LL25A;_tmp392=(_tmp391.ReachableFL).f1;
_tmp393=(_tmp391.ReachableFL).f2;_tmp394=(_tmp391.ReachableFL).f3;_LL259: if(Cyc_CfFlowInfo_initlevel(
_tmp392,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp38F))->tl))->hd)== (void*)0)({void*_tmp395=0;Cyc_Tcutil_terr(
_tmp371->loc,({const char*_tmp396="expression may not be initialized";
_tag_dynforward(_tmp396,sizeof(char),_get_zero_arr_size_char(_tmp396,34));}),
_tag_dynforward(_tmp395,sizeof(void*),0));});{struct Cyc_List_List*_tmp397=Cyc_NewControlFlow_add_subscript_reln(
_tmp35A->r,_tmp393,_tmp370,_tmp371);if(_tmp393 != _tmp397)_tmp390=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp398;(_tmp398.ReachableFL).tag=1;(_tmp398.ReachableFL).f1=
_tmp392;(_tmp398.ReachableFL).f2=_tmp397;(_tmp398.ReachableFL).f3=_tmp394;
_tmp398;});goto _LL257;}_LL25A:;_LL25B: goto _LL257;_LL257:;}{union Cyc_Absyn_Constraint_union
_tmp399=(Cyc_Absyn_compress_conref(_tmp388))->v;void*_tmp39A;_LL25D: if((_tmp399.Eq_constr).tag
!= 0)goto _LL25F;_tmp39A=(_tmp399.Eq_constr).f1;if(_tmp39A <= (void*)2)goto _LL25F;
if(*((int*)_tmp39A)!= 1)goto _LL25F;_LL25E:{void*_tmp39B=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp38F))->tl))->hd;struct Cyc_List_List*
_tmp39C;_LL262: if(_tmp39B <= (void*)3)goto _LL264;if(*((int*)_tmp39B)!= 3)goto
_LL264;_tmp39C=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp39B)->f1;_LL263:(void*)(
_tmp371->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp39D=
_cycalloc(sizeof(*_tmp39D));_tmp39D[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp39E;_tmp39E.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp39E.f1=Cyc_CfFlowInfo_copy_tagcmps(
Cyc_Core_heap_region,_tmp39C);_tmp39E;});_tmp39D;})));goto _LL261;_LL264:;_LL265:
goto _LL261;_LL261:;}goto _LL25C;_LL25F:;_LL260: goto _LL25C;_LL25C:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A0;union Cyc_CfFlowInfo_AbsLVal_union _tmp3A1;struct _tuple7 _tmp39F=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp38E,_tmp370,(void*)((struct Cyc_List_List*)_check_null(_tmp38F))->hd,
flds);_tmp3A0=_tmp39F.f1;_tmp3A1=_tmp39F.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A2=_tmp3A0;_LL267: if((_tmp3A2.BottomFL).tag != 0)goto _LL269;_LL268: {struct
_tuple7 _tmp3A4=({struct _tuple7 _tmp3A3;_tmp3A3.f1=_tmp3A0;_tmp3A3.f2=_tmp3A1;
_tmp3A3;});_npop_handler(0);return _tmp3A4;}_LL269:;_LL26A: {struct _tuple7 _tmp3A6=({
struct _tuple7 _tmp3A5;_tmp3A5.f1=_tmp390;_tmp3A5.f2=_tmp3A1;_tmp3A5;});
_npop_handler(0);return _tmp3A6;}_LL266:;}}}};_pop_region(temp);}_LL255:;_LL256:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp3A7=_cycalloc(sizeof(*
_tmp3A7));_tmp3A7[0]=({struct Cyc_Core_Impossible_struct _tmp3A8;_tmp3A8.tag=Cyc_Core_Impossible;
_tmp3A8.f1=({const char*_tmp3A9="anal_Lexp: Subscript -- bad type";
_tag_dynforward(_tmp3A9,sizeof(char),_get_zero_arr_size_char(_tmp3A9,33));});
_tmp3A8;});_tmp3A7;}));_LL250:;}_LL247: if(*((int*)_tmp361)!= 23)goto _LL249;
_tmp372=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp361)->f1;_tmp373=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp361)->f2;_LL248: if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp372->topt))->v))return({struct _tuple7 _tmp3AA;_tmp3AA.f1=inflow;
_tmp3AA.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp3AB;(_tmp3AB.UnknownL).tag=1;_tmp3AB;});_tmp3AA;});return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp372,({struct Cyc_List_List*_tmp3AC=_region_malloc(env->r,sizeof(*
_tmp3AC));_tmp3AC->hd=(void*)Cyc_NewControlFlow_get_field_index((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp372->topt))->v,_tmp373);_tmp3AC->tl=flds;_tmp3AC;}));
_LL249:;_LL24A: return({struct _tuple7 _tmp3AD;_tmp3AD.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3AF;(_tmp3AF.BottomFL).tag=0;_tmp3AF;});
_tmp3AD.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp3AE;(_tmp3AE.UnknownL).tag=1;_tmp3AE;});_tmp3AD;});_LL232:;}}static struct
_tuple7 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){union Cyc_CfFlowInfo_FlowInfo_union
_tmp3B1;union Cyc_CfFlowInfo_AbsLVal_union _tmp3B2;struct _tuple7 _tmp3B0=Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,e,0);_tmp3B1=_tmp3B0.f1;_tmp3B2=_tmp3B0.f2;return({struct _tuple7
_tmp3B3;_tmp3B3.f1=_tmp3B1;_tmp3B3.f2=_tmp3B2;_tmp3B3;});}static struct _tuple8 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp3B4=env->fenv;{void*
_tmp3B5=(void*)e->r;struct Cyc_Absyn_Exp*_tmp3B6;struct Cyc_Absyn_Exp*_tmp3B7;
struct Cyc_Absyn_Exp*_tmp3B8;struct Cyc_Absyn_Exp*_tmp3B9;struct Cyc_Absyn_Exp*
_tmp3BA;struct Cyc_Absyn_Exp*_tmp3BB;struct Cyc_Absyn_Exp*_tmp3BC;struct Cyc_Absyn_Exp*
_tmp3BD;struct Cyc_Absyn_Exp*_tmp3BE;void*_tmp3BF;struct Cyc_List_List*_tmp3C0;
struct Cyc_List_List _tmp3C1;struct Cyc_Absyn_Exp*_tmp3C2;struct Cyc_List_List*
_tmp3C3;void*_tmp3C4;struct Cyc_List_List*_tmp3C5;_LL26C: if(*((int*)_tmp3B5)!= 6)
goto _LL26E;_tmp3B6=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3B5)->f1;_tmp3B7=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp3B5)->f2;_tmp3B8=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp3B5)->f3;_LL26D: {union Cyc_CfFlowInfo_FlowInfo_union _tmp3C7;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3C8;struct _tuple8 _tmp3C6=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3B6);
_tmp3C7=_tmp3C6.f1;_tmp3C8=_tmp3C6.f2;_tmp3C7=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3B6->loc,_tmp3C7);_tmp3C8=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3B6->loc,
_tmp3C8);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3CA;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3CB;struct _tuple8 _tmp3C9=Cyc_NewControlFlow_anal_test(env,_tmp3C7,_tmp3B7);
_tmp3CA=_tmp3C9.f1;_tmp3CB=_tmp3C9.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp3CD;
union Cyc_CfFlowInfo_FlowInfo_union _tmp3CE;struct _tuple8 _tmp3CC=Cyc_NewControlFlow_anal_test(
env,_tmp3C8,_tmp3B8);_tmp3CD=_tmp3CC.f1;_tmp3CE=_tmp3CC.f2;return({struct _tuple8
_tmp3CF;_tmp3CF.f1=Cyc_CfFlowInfo_join_flow(_tmp3B4,env->all_changed,_tmp3CA,
_tmp3CD,1);_tmp3CF.f2=Cyc_CfFlowInfo_join_flow(_tmp3B4,env->all_changed,_tmp3CB,
_tmp3CE,1);_tmp3CF;});}}}_LL26E: if(*((int*)_tmp3B5)!= 7)goto _LL270;_tmp3B9=((
struct Cyc_Absyn_And_e_struct*)_tmp3B5)->f1;_tmp3BA=((struct Cyc_Absyn_And_e_struct*)
_tmp3B5)->f2;_LL26F: {union Cyc_CfFlowInfo_FlowInfo_union _tmp3D1;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D2;struct _tuple8 _tmp3D0=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3B9);
_tmp3D1=_tmp3D0.f1;_tmp3D2=_tmp3D0.f2;_tmp3D1=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3B9->loc,_tmp3D1);_tmp3D2=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3B9->loc,
_tmp3D2);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3D4;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D5;struct _tuple8 _tmp3D3=Cyc_NewControlFlow_anal_test(env,_tmp3D1,_tmp3BA);
_tmp3D4=_tmp3D3.f1;_tmp3D5=_tmp3D3.f2;_tmp3D4=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3BA->loc,_tmp3D4);_tmp3D5=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3BA->loc,
_tmp3D5);return({struct _tuple8 _tmp3D6;_tmp3D6.f1=_tmp3D4;_tmp3D6.f2=Cyc_CfFlowInfo_join_flow(
_tmp3B4,env->all_changed,_tmp3D2,_tmp3D5,0);_tmp3D6;});}}_LL270: if(*((int*)
_tmp3B5)!= 8)goto _LL272;_tmp3BB=((struct Cyc_Absyn_Or_e_struct*)_tmp3B5)->f1;
_tmp3BC=((struct Cyc_Absyn_Or_e_struct*)_tmp3B5)->f2;_LL271: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D8;union Cyc_CfFlowInfo_FlowInfo_union _tmp3D9;struct _tuple8 _tmp3D7=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3BB);_tmp3D8=_tmp3D7.f1;_tmp3D9=_tmp3D7.f2;_tmp3D8=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3BB->loc,_tmp3D8);_tmp3D9=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3BB->loc,
_tmp3D9);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3DB;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3DC;struct _tuple8 _tmp3DA=Cyc_NewControlFlow_anal_test(env,_tmp3D9,_tmp3BC);
_tmp3DB=_tmp3DA.f1;_tmp3DC=_tmp3DA.f2;_tmp3DB=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3BC->loc,_tmp3DB);_tmp3DC=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3BC->loc,
_tmp3DC);return({struct _tuple8 _tmp3DD;_tmp3DD.f1=Cyc_CfFlowInfo_join_flow(
_tmp3B4,env->all_changed,_tmp3D8,_tmp3DB,0);_tmp3DD.f2=_tmp3DC;_tmp3DD;});}}
_LL272: if(*((int*)_tmp3B5)!= 9)goto _LL274;_tmp3BD=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp3B5)->f1;_tmp3BE=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp3B5)->f2;_LL273: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp3DF;void*_tmp3E0;struct _tuple5 _tmp3DE=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp3BD);_tmp3DF=_tmp3DE.f1;_tmp3E0=_tmp3DE.f2;_tmp3DF=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp3BD->loc,_tmp3DF);return Cyc_NewControlFlow_anal_test(env,_tmp3DF,_tmp3BE);}
_LL274: if(*((int*)_tmp3B5)!= 3)goto _LL276;_tmp3BF=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3B5)->f1;if((int)_tmp3BF != 11)goto _LL276;_tmp3C0=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3B5)->f2;if(_tmp3C0 == 0)goto _LL276;_tmp3C1=*_tmp3C0;_tmp3C2=(struct Cyc_Absyn_Exp*)
_tmp3C1.hd;_tmp3C3=_tmp3C1.tl;if(_tmp3C3 != 0)goto _LL276;_LL275: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3E2;union Cyc_CfFlowInfo_FlowInfo_union _tmp3E3;struct _tuple8 _tmp3E1=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3C2);_tmp3E2=_tmp3E1.f1;_tmp3E3=_tmp3E1.f2;return({struct _tuple8
_tmp3E4;_tmp3E4.f1=_tmp3E3;_tmp3E4.f2=_tmp3E2;_tmp3E4;});}_LL276: if(*((int*)
_tmp3B5)!= 3)goto _LL278;_tmp3C4=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3B5)->f1;_tmp3C5=((struct Cyc_Absyn_Primop_e_struct*)_tmp3B5)->f2;_LL277: {
void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo_union f;{struct _RegionHandle _tmp3E5=
_new_region("temp");struct _RegionHandle*temp=& _tmp3E5;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp3E7;struct Cyc_List_List*_tmp3E8;struct _tuple11
_tmp3E6=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp3C5,0);
_tmp3E7=_tmp3E6.f1;_tmp3E8=_tmp3E6.f2;r1=(void*)((struct Cyc_List_List*)
_check_null(_tmp3E8))->hd;r2=(void*)((struct Cyc_List_List*)_check_null(_tmp3E8->tl))->hd;
f=_tmp3E7;};_pop_region(temp);}{void*_tmp3E9=_tmp3C4;_LL27B: if((int)_tmp3E9 != 0)
goto _LL27D;_LL27C: goto _LL27E;_LL27D: if((int)_tmp3E9 != 2)goto _LL27F;_LL27E: Cyc_CfFlowInfo_check_unique_rvals(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3C5))->hd)->loc,f);
goto _LL27A;_LL27F:;_LL280: f=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3C5))->hd)->loc,f);goto _LL27A;_LL27A:;}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp3EA=f;struct Cyc_Dict_Dict _tmp3EB;struct Cyc_List_List*
_tmp3EC;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3ED;_LL282: if((_tmp3EA.BottomFL).tag
!= 0)goto _LL284;_LL283: return({struct _tuple8 _tmp3EE;_tmp3EE.f1=f;_tmp3EE.f2=f;
_tmp3EE;});_LL284: if((_tmp3EA.ReachableFL).tag != 1)goto _LL281;_tmp3EB=(_tmp3EA.ReachableFL).f1;
_tmp3EC=(_tmp3EA.ReachableFL).f2;_tmp3ED=(_tmp3EA.ReachableFL).f3;_LL285: {
struct Cyc_Absyn_Exp*_tmp3EF=(struct Cyc_Absyn_Exp*)_tmp3C5->hd;struct Cyc_Absyn_Exp*
_tmp3F0=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3C5->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(_tmp3EB,r1)== (void*)0)({void*_tmp3F1=0;Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp3C5->hd)->loc,({const char*_tmp3F2="expression may not be initialized";
_tag_dynforward(_tmp3F2,sizeof(char),_get_zero_arr_size_char(_tmp3F2,34));}),
_tag_dynforward(_tmp3F1,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(_tmp3EB,
r2)== (void*)0)({void*_tmp3F3=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp3C5->tl))->hd)->loc,({const char*_tmp3F4="expression may not be initialized";
_tag_dynforward(_tmp3F4,sizeof(char),_get_zero_arr_size_char(_tmp3F4,34));}),
_tag_dynforward(_tmp3F3,sizeof(void*),0));});if(_tmp3C4 == (void*)5  || _tmp3C4 == (
void*)6){struct _tuple0 _tmp3F6=({struct _tuple0 _tmp3F5;_tmp3F5.f1=r1;_tmp3F5.f2=r2;
_tmp3F5;});void*_tmp3F7;void*_tmp3F8;void*_tmp3F9;void*_tmp3FA;void*_tmp3FB;void*
_tmp3FC;void*_tmp3FD;void*_tmp3FE;void*_tmp3FF;void*_tmp400;void*_tmp401;void*
_tmp402;void*_tmp403;void*_tmp404;void*_tmp405;void*_tmp406;void*_tmp407;void*
_tmp408;void*_tmp409;void*_tmp40A;_LL287: _tmp3F7=_tmp3F6.f1;if(_tmp3F7 <= (void*)
3)goto _LL289;if(*((int*)_tmp3F7)!= 0)goto _LL289;_tmp3F8=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp3F7)->f1;_tmp3F9=_tmp3F6.f2;if((int)_tmp3F9 != 0)goto _LL289;_LL288: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp40C;union Cyc_CfFlowInfo_FlowInfo_union _tmp40D;struct _tuple8 _tmp40B=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp3EF,_tmp3F8);_tmp40C=_tmp40B.f1;_tmp40D=_tmp40B.f2;{void*_tmp40E=
_tmp3C4;_LL29C: if((int)_tmp40E != 5)goto _LL29E;_LL29D: return({struct _tuple8
_tmp40F;_tmp40F.f1=_tmp40D;_tmp40F.f2=_tmp40C;_tmp40F;});_LL29E: if((int)_tmp40E
!= 6)goto _LL2A0;_LL29F: return({struct _tuple8 _tmp410;_tmp410.f1=_tmp40C;_tmp410.f2=
_tmp40D;_tmp410;});_LL2A0:;_LL2A1:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp411=_cycalloc(sizeof(*_tmp411));_tmp411[0]=({struct Cyc_Core_Impossible_struct
_tmp412;_tmp412.tag=Cyc_Core_Impossible;_tmp412.f1=({const char*_tmp413="anal_test, zero-split";
_tag_dynforward(_tmp413,sizeof(char),_get_zero_arr_size_char(_tmp413,22));});
_tmp412;});_tmp411;}));_LL29B:;}}_LL289: _tmp3FA=_tmp3F6.f1;if((int)_tmp3FA != 0)
goto _LL28B;_tmp3FB=_tmp3F6.f2;if(_tmp3FB <= (void*)3)goto _LL28B;if(*((int*)
_tmp3FB)!= 0)goto _LL28B;_tmp3FC=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp3FB)->f1;_LL28A: {union Cyc_CfFlowInfo_FlowInfo_union _tmp415;union Cyc_CfFlowInfo_FlowInfo_union
_tmp416;struct _tuple8 _tmp414=Cyc_NewControlFlow_splitzero(env,f,f,_tmp3F0,
_tmp3FC);_tmp415=_tmp414.f1;_tmp416=_tmp414.f2;{void*_tmp417=_tmp3C4;_LL2A3: if((
int)_tmp417 != 5)goto _LL2A5;_LL2A4: return({struct _tuple8 _tmp418;_tmp418.f1=
_tmp416;_tmp418.f2=_tmp415;_tmp418;});_LL2A5: if((int)_tmp417 != 6)goto _LL2A7;
_LL2A6: return({struct _tuple8 _tmp419;_tmp419.f1=_tmp415;_tmp419.f2=_tmp416;
_tmp419;});_LL2A7:;_LL2A8:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp41A=_cycalloc(sizeof(*_tmp41A));_tmp41A[0]=({struct Cyc_Core_Impossible_struct
_tmp41B;_tmp41B.tag=Cyc_Core_Impossible;_tmp41B.f1=({const char*_tmp41C="anal_test, zero-split";
_tag_dynforward(_tmp41C,sizeof(char),_get_zero_arr_size_char(_tmp41C,22));});
_tmp41B;});_tmp41A;}));_LL2A2:;}}_LL28B: _tmp3FD=_tmp3F6.f1;if((int)_tmp3FD != 0)
goto _LL28D;_tmp3FE=_tmp3F6.f2;if((int)_tmp3FE != 0)goto _LL28D;_LL28C: if(_tmp3C4 == (
void*)5)return({struct _tuple8 _tmp41D;_tmp41D.f1=f;_tmp41D.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp41E;(_tmp41E.BottomFL).tag=0;_tmp41E;});
_tmp41D;});else{return({struct _tuple8 _tmp41F;_tmp41F.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp420;(_tmp420.BottomFL).tag=0;_tmp420;});
_tmp41F.f2=f;_tmp41F;});}_LL28D: _tmp3FF=_tmp3F6.f1;if((int)_tmp3FF != 0)goto
_LL28F;_tmp400=_tmp3F6.f2;if((int)_tmp400 != 1)goto _LL28F;_LL28E: goto _LL290;
_LL28F: _tmp401=_tmp3F6.f1;if((int)_tmp401 != 0)goto _LL291;_tmp402=_tmp3F6.f2;if((
int)_tmp402 != 2)goto _LL291;_LL290: goto _LL292;_LL291: _tmp403=_tmp3F6.f1;if((int)
_tmp403 != 0)goto _LL293;_tmp404=_tmp3F6.f2;if(_tmp404 <= (void*)3)goto _LL293;if(*((
int*)_tmp404)!= 2)goto _LL293;_LL292: goto _LL294;_LL293: _tmp405=_tmp3F6.f1;if((int)
_tmp405 != 1)goto _LL295;_tmp406=_tmp3F6.f2;if((int)_tmp406 != 0)goto _LL295;_LL294:
goto _LL296;_LL295: _tmp407=_tmp3F6.f1;if((int)_tmp407 != 2)goto _LL297;_tmp408=
_tmp3F6.f2;if((int)_tmp408 != 0)goto _LL297;_LL296: goto _LL298;_LL297: _tmp409=
_tmp3F6.f1;if(_tmp409 <= (void*)3)goto _LL299;if(*((int*)_tmp409)!= 2)goto _LL299;
_tmp40A=_tmp3F6.f2;if((int)_tmp40A != 0)goto _LL299;_LL298: if(_tmp3C4 == (void*)6)
return({struct _tuple8 _tmp421;_tmp421.f1=f;_tmp421.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp422;(_tmp422.BottomFL).tag=0;_tmp422;});
_tmp421;});else{return({struct _tuple8 _tmp423;_tmp423.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp424;(_tmp424.BottomFL).tag=0;_tmp424;});
_tmp423.f2=f;_tmp423;});}_LL299:;_LL29A: goto _LL286;_LL286:;}{struct _tuple0
_tmp426=({struct _tuple0 _tmp425;_tmp425.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3EF->topt))->v);_tmp425.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp3F0->topt))->v);_tmp425;});void*_tmp427;void*
_tmp428;void*_tmp429;void*_tmp42A;void*_tmp42B;void*_tmp42C;_LL2AA: _tmp427=
_tmp426.f1;if(_tmp427 <= (void*)4)goto _LL2AC;if(*((int*)_tmp427)!= 5)goto _LL2AC;
_tmp428=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp427)->f1;if((int)_tmp428 != 
1)goto _LL2AC;_LL2AB: goto _LL2AD;_LL2AC: _tmp429=_tmp426.f2;if(_tmp429 <= (void*)4)
goto _LL2AE;if(*((int*)_tmp429)!= 5)goto _LL2AE;_tmp42A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp429)->f1;if((int)_tmp42A != 1)goto _LL2AE;_LL2AD: goto _LL2AF;_LL2AE: _tmp42B=
_tmp426.f1;if(_tmp42B <= (void*)4)goto _LL2B0;if(*((int*)_tmp42B)!= 18)goto _LL2B0;
_LL2AF: goto _LL2B1;_LL2B0: _tmp42C=_tmp426.f2;if(_tmp42C <= (void*)4)goto _LL2B2;if(*((
int*)_tmp42C)!= 18)goto _LL2B2;_LL2B1: goto _LL2A9;_LL2B2:;_LL2B3: return({struct
_tuple8 _tmp42D;_tmp42D.f1=f;_tmp42D.f2=f;_tmp42D;});_LL2A9:;}{void*_tmp42E=
_tmp3C4;_LL2B5: if((int)_tmp42E != 5)goto _LL2B7;_LL2B6: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp42F=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3EF,r1,(void*)5,r2);_tmp42F=Cyc_NewControlFlow_if_tagcmp(
env,_tmp42F,_tmp3F0,r2,(void*)5,r1);return({struct _tuple8 _tmp430;_tmp430.f1=
_tmp42F;_tmp430.f2=f;_tmp430;});}_LL2B7: if((int)_tmp42E != 6)goto _LL2B9;_LL2B8: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp431=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3EF,r1,(void*)5,r2);_tmp431=Cyc_NewControlFlow_if_tagcmp(env,_tmp431,_tmp3F0,
r2,(void*)5,r1);return({struct _tuple8 _tmp432;_tmp432.f1=f;_tmp432.f2=_tmp431;
_tmp432;});}_LL2B9: if((int)_tmp42E != 7)goto _LL2BB;_LL2BA: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp433=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F0,r2,(void*)8,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp434=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3EF,r1,(void*)10,r2);return({
struct _tuple8 _tmp435;_tmp435.f1=_tmp433;_tmp435.f2=_tmp434;_tmp435;});}_LL2BB:
if((int)_tmp42E != 9)goto _LL2BD;_LL2BC: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp436=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F0,r2,(void*)10,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp437=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3EF,r1,(void*)8,r2);return({
struct _tuple8 _tmp438;_tmp438.f1=_tmp436;_tmp438.f2=_tmp437;_tmp438;});}_LL2BD:
if((int)_tmp42E != 8)goto _LL2BF;_LL2BE: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp439=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3EF,r1,(void*)8,r2);union Cyc_CfFlowInfo_FlowInfo_union
_tmp43A=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F0,r2,(void*)10,r1);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp43B=_tmp439;struct Cyc_Dict_Dict _tmp43C;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp43D;_LL2C4: if((_tmp43B.BottomFL).tag != 0)goto _LL2C6;_LL2C5:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp43E=_cycalloc(sizeof(*_tmp43E));_tmp43E[0]=({
struct Cyc_Core_Impossible_struct _tmp43F;_tmp43F.tag=Cyc_Core_Impossible;_tmp43F.f1=({
const char*_tmp440="anal_test, Lt";_tag_dynforward(_tmp440,sizeof(char),
_get_zero_arr_size_char(_tmp440,14));});_tmp43F;});_tmp43E;}));_LL2C6: if((
_tmp43B.ReachableFL).tag != 1)goto _LL2C3;_tmp43C=(_tmp43B.ReachableFL).f1;_tmp43D=(
_tmp43B.ReachableFL).f3;_LL2C7: _tmp3EB=_tmp43C;_tmp3ED=_tmp43D;_LL2C3:;}{void*
_tmp441=(void*)_tmp3EF->r;void*_tmp442;struct Cyc_Absyn_Vardecl*_tmp443;void*
_tmp444;struct Cyc_Absyn_Vardecl*_tmp445;void*_tmp446;struct Cyc_Absyn_Vardecl*
_tmp447;void*_tmp448;struct Cyc_Absyn_Vardecl*_tmp449;_LL2C9: if(*((int*)_tmp441)
!= 1)goto _LL2CB;_tmp442=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp441)->f2;if(
_tmp442 <= (void*)1)goto _LL2CB;if(*((int*)_tmp442)!= 0)goto _LL2CB;_tmp443=((
struct Cyc_Absyn_Global_b_struct*)_tmp442)->f1;if(!(!_tmp443->escapes))goto _LL2CB;
_LL2CA: _tmp445=_tmp443;goto _LL2CC;_LL2CB: if(*((int*)_tmp441)!= 1)goto _LL2CD;
_tmp444=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp441)->f2;if(_tmp444 <= (void*)
1)goto _LL2CD;if(*((int*)_tmp444)!= 3)goto _LL2CD;_tmp445=((struct Cyc_Absyn_Local_b_struct*)
_tmp444)->f1;if(!(!_tmp445->escapes))goto _LL2CD;_LL2CC: _tmp447=_tmp445;goto
_LL2CE;_LL2CD: if(*((int*)_tmp441)!= 1)goto _LL2CF;_tmp446=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp441)->f2;if(_tmp446 <= (void*)1)goto _LL2CF;if(*((int*)_tmp446)!= 4)goto _LL2CF;
_tmp447=((struct Cyc_Absyn_Pat_b_struct*)_tmp446)->f1;if(!(!_tmp447->escapes))
goto _LL2CF;_LL2CE: _tmp449=_tmp447;goto _LL2D0;_LL2CF: if(*((int*)_tmp441)!= 1)goto
_LL2D1;_tmp448=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp441)->f2;if(_tmp448 <= (
void*)1)goto _LL2D1;if(*((int*)_tmp448)!= 2)goto _LL2D1;_tmp449=((struct Cyc_Absyn_Param_b_struct*)
_tmp448)->f1;if(!(!_tmp449->escapes))goto _LL2D1;_LL2D0: {void*_tmp44A=(void*)
_tmp3F0->r;void*_tmp44B;struct Cyc_Absyn_Vardecl*_tmp44C;void*_tmp44D;struct Cyc_Absyn_Vardecl*
_tmp44E;void*_tmp44F;struct Cyc_Absyn_Vardecl*_tmp450;void*_tmp451;struct Cyc_Absyn_Vardecl*
_tmp452;union Cyc_Absyn_Cnst_union _tmp453;int _tmp454;void*_tmp455;struct Cyc_List_List*
_tmp456;struct Cyc_List_List _tmp457;struct Cyc_Absyn_Exp*_tmp458;_LL2D4: if(*((int*)
_tmp44A)!= 1)goto _LL2D6;_tmp44B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp44A)->f2;
if(_tmp44B <= (void*)1)goto _LL2D6;if(*((int*)_tmp44B)!= 0)goto _LL2D6;_tmp44C=((
struct Cyc_Absyn_Global_b_struct*)_tmp44B)->f1;if(!(!_tmp44C->escapes))goto _LL2D6;
_LL2D5: _tmp44E=_tmp44C;goto _LL2D7;_LL2D6: if(*((int*)_tmp44A)!= 1)goto _LL2D8;
_tmp44D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp44A)->f2;if(_tmp44D <= (void*)
1)goto _LL2D8;if(*((int*)_tmp44D)!= 3)goto _LL2D8;_tmp44E=((struct Cyc_Absyn_Local_b_struct*)
_tmp44D)->f1;if(!(!_tmp44E->escapes))goto _LL2D8;_LL2D7: _tmp450=_tmp44E;goto
_LL2D9;_LL2D8: if(*((int*)_tmp44A)!= 1)goto _LL2DA;_tmp44F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp44A)->f2;if(_tmp44F <= (void*)1)goto _LL2DA;if(*((int*)_tmp44F)!= 4)goto _LL2DA;
_tmp450=((struct Cyc_Absyn_Pat_b_struct*)_tmp44F)->f1;if(!(!_tmp450->escapes))
goto _LL2DA;_LL2D9: _tmp452=_tmp450;goto _LL2DB;_LL2DA: if(*((int*)_tmp44A)!= 1)goto
_LL2DC;_tmp451=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp44A)->f2;if(_tmp451 <= (
void*)1)goto _LL2DC;if(*((int*)_tmp451)!= 2)goto _LL2DC;_tmp452=((struct Cyc_Absyn_Param_b_struct*)
_tmp451)->f1;if(!(!_tmp452->escapes))goto _LL2DC;_LL2DB: {struct _RegionHandle*
_tmp459=(env->fenv)->r;struct Cyc_List_List*_tmp45A=({struct Cyc_List_List*_tmp45D=
_region_malloc(_tmp459,sizeof(*_tmp45D));_tmp45D->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp45E=_region_malloc(_tmp459,sizeof(*_tmp45E));_tmp45E->vd=_tmp449;_tmp45E->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp45F;(
_tmp45F.LessVar).tag=1;(_tmp45F.LessVar).f1=_tmp452;_tmp45F;});_tmp45E;});
_tmp45D->tl=_tmp3EC;_tmp45D;});return({struct _tuple8 _tmp45B;_tmp45B.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp45C;(_tmp45C.ReachableFL).tag=1;(_tmp45C.ReachableFL).f1=
_tmp3EB;(_tmp45C.ReachableFL).f2=_tmp45A;(_tmp45C.ReachableFL).f3=_tmp3ED;
_tmp45C;});_tmp45B.f2=_tmp43A;_tmp45B;});}_LL2DC: if(*((int*)_tmp44A)!= 0)goto
_LL2DE;_tmp453=((struct Cyc_Absyn_Const_e_struct*)_tmp44A)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp44A)->f1).Int_c).tag != 2)goto _LL2DE;_tmp454=(_tmp453.Int_c).f2;_LL2DD: {
struct _RegionHandle*_tmp460=(env->fenv)->r;struct Cyc_List_List*_tmp461=({struct
Cyc_List_List*_tmp464=_region_malloc(_tmp460,sizeof(*_tmp464));_tmp464->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp465=_region_malloc(_tmp460,sizeof(*_tmp465));
_tmp465->vd=_tmp449;_tmp465->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp466;(_tmp466.LessConst).tag=3;(_tmp466.LessConst).f1=(unsigned int)_tmp454;
_tmp466;});_tmp465;});_tmp464->tl=_tmp3EC;_tmp464;});return({struct _tuple8
_tmp462;_tmp462.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp463;(_tmp463.ReachableFL).tag=1;(_tmp463.ReachableFL).f1=_tmp3EB;(_tmp463.ReachableFL).f2=
_tmp461;(_tmp463.ReachableFL).f3=_tmp3ED;_tmp463;});_tmp462.f2=_tmp43A;_tmp462;});}
_LL2DE: if(*((int*)_tmp44A)!= 3)goto _LL2E0;_tmp455=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp44A)->f1;if((int)_tmp455 != 19)goto _LL2E0;_tmp456=((struct Cyc_Absyn_Primop_e_struct*)
_tmp44A)->f2;if(_tmp456 == 0)goto _LL2E0;_tmp457=*_tmp456;_tmp458=(struct Cyc_Absyn_Exp*)
_tmp457.hd;_LL2DF: {void*_tmp467=(void*)_tmp458->r;void*_tmp468;struct Cyc_Absyn_Vardecl*
_tmp469;void*_tmp46A;struct Cyc_Absyn_Vardecl*_tmp46B;void*_tmp46C;struct Cyc_Absyn_Vardecl*
_tmp46D;void*_tmp46E;struct Cyc_Absyn_Vardecl*_tmp46F;_LL2E3: if(*((int*)_tmp467)
!= 1)goto _LL2E5;_tmp468=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp467)->f2;if(
_tmp468 <= (void*)1)goto _LL2E5;if(*((int*)_tmp468)!= 0)goto _LL2E5;_tmp469=((
struct Cyc_Absyn_Global_b_struct*)_tmp468)->f1;if(!(!_tmp469->escapes))goto _LL2E5;
_LL2E4: _tmp46B=_tmp469;goto _LL2E6;_LL2E5: if(*((int*)_tmp467)!= 1)goto _LL2E7;
_tmp46A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp467)->f2;if(_tmp46A <= (void*)
1)goto _LL2E7;if(*((int*)_tmp46A)!= 3)goto _LL2E7;_tmp46B=((struct Cyc_Absyn_Local_b_struct*)
_tmp46A)->f1;if(!(!_tmp46B->escapes))goto _LL2E7;_LL2E6: _tmp46D=_tmp46B;goto
_LL2E8;_LL2E7: if(*((int*)_tmp467)!= 1)goto _LL2E9;_tmp46C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp467)->f2;if(_tmp46C <= (void*)1)goto _LL2E9;if(*((int*)_tmp46C)!= 4)goto _LL2E9;
_tmp46D=((struct Cyc_Absyn_Pat_b_struct*)_tmp46C)->f1;if(!(!_tmp46D->escapes))
goto _LL2E9;_LL2E8: _tmp46F=_tmp46D;goto _LL2EA;_LL2E9: if(*((int*)_tmp467)!= 1)goto
_LL2EB;_tmp46E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp467)->f2;if(_tmp46E <= (
void*)1)goto _LL2EB;if(*((int*)_tmp46E)!= 2)goto _LL2EB;_tmp46F=((struct Cyc_Absyn_Param_b_struct*)
_tmp46E)->f1;if(!(!_tmp46F->escapes))goto _LL2EB;_LL2EA: {struct _RegionHandle*
_tmp470=(env->fenv)->r;struct Cyc_List_List*_tmp471=({struct Cyc_List_List*_tmp474=
_region_malloc(_tmp470,sizeof(*_tmp474));_tmp474->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp475=_region_malloc(_tmp470,sizeof(*_tmp475));_tmp475->vd=_tmp449;_tmp475->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp476;(
_tmp476.LessSize).tag=2;(_tmp476.LessSize).f1=_tmp46F;_tmp476;});_tmp475;});
_tmp474->tl=_tmp3EC;_tmp474;});return({struct _tuple8 _tmp472;_tmp472.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp473;(_tmp473.ReachableFL).tag=1;(_tmp473.ReachableFL).f1=
_tmp3EB;(_tmp473.ReachableFL).f2=_tmp471;(_tmp473.ReachableFL).f3=_tmp3ED;
_tmp473;});_tmp472.f2=_tmp43A;_tmp472;});}_LL2EB:;_LL2EC: return({struct _tuple8
_tmp477;_tmp477.f1=_tmp439;_tmp477.f2=_tmp43A;_tmp477;});_LL2E2:;}_LL2E0:;_LL2E1:
return({struct _tuple8 _tmp478;_tmp478.f1=_tmp439;_tmp478.f2=_tmp43A;_tmp478;});
_LL2D3:;}_LL2D1:;_LL2D2: return({struct _tuple8 _tmp479;_tmp479.f1=_tmp439;_tmp479.f2=
_tmp43A;_tmp479;});_LL2C8:;}}_LL2BF: if((int)_tmp42E != 10)goto _LL2C1;_LL2C0: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp47A=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3EF,r1,(void*)10,r2);union Cyc_CfFlowInfo_FlowInfo_union _tmp47B=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3F0,r1,(void*)8,r1);{union Cyc_CfFlowInfo_FlowInfo_union _tmp47C=_tmp47A;
struct Cyc_Dict_Dict _tmp47D;struct Cyc_CfFlowInfo_ConsumeInfo _tmp47E;_LL2EE: if((
_tmp47C.BottomFL).tag != 0)goto _LL2F0;_LL2EF:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp47F=_cycalloc(sizeof(*_tmp47F));_tmp47F[0]=({struct Cyc_Core_Impossible_struct
_tmp480;_tmp480.tag=Cyc_Core_Impossible;_tmp480.f1=({const char*_tmp481="anal_test, Lte";
_tag_dynforward(_tmp481,sizeof(char),_get_zero_arr_size_char(_tmp481,15));});
_tmp480;});_tmp47F;}));_LL2F0: if((_tmp47C.ReachableFL).tag != 1)goto _LL2ED;
_tmp47D=(_tmp47C.ReachableFL).f1;_tmp47E=(_tmp47C.ReachableFL).f3;_LL2F1: _tmp3EB=
_tmp47D;_tmp3ED=_tmp47E;_LL2ED:;}{void*_tmp482=(void*)_tmp3EF->r;void*_tmp483;
struct Cyc_Absyn_Vardecl*_tmp484;void*_tmp485;struct Cyc_Absyn_Vardecl*_tmp486;
void*_tmp487;struct Cyc_Absyn_Vardecl*_tmp488;void*_tmp489;struct Cyc_Absyn_Vardecl*
_tmp48A;_LL2F3: if(*((int*)_tmp482)!= 1)goto _LL2F5;_tmp483=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp482)->f2;if(_tmp483 <= (void*)1)goto _LL2F5;if(*((int*)_tmp483)!= 0)goto _LL2F5;
_tmp484=((struct Cyc_Absyn_Global_b_struct*)_tmp483)->f1;if(!(!_tmp484->escapes))
goto _LL2F5;_LL2F4: _tmp486=_tmp484;goto _LL2F6;_LL2F5: if(*((int*)_tmp482)!= 1)goto
_LL2F7;_tmp485=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp482)->f2;if(_tmp485 <= (
void*)1)goto _LL2F7;if(*((int*)_tmp485)!= 3)goto _LL2F7;_tmp486=((struct Cyc_Absyn_Local_b_struct*)
_tmp485)->f1;if(!(!_tmp486->escapes))goto _LL2F7;_LL2F6: _tmp488=_tmp486;goto
_LL2F8;_LL2F7: if(*((int*)_tmp482)!= 1)goto _LL2F9;_tmp487=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp482)->f2;if(_tmp487 <= (void*)1)goto _LL2F9;if(*((int*)_tmp487)!= 4)goto _LL2F9;
_tmp488=((struct Cyc_Absyn_Pat_b_struct*)_tmp487)->f1;if(!(!_tmp488->escapes))
goto _LL2F9;_LL2F8: _tmp48A=_tmp488;goto _LL2FA;_LL2F9: if(*((int*)_tmp482)!= 1)goto
_LL2FB;_tmp489=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp482)->f2;if(_tmp489 <= (
void*)1)goto _LL2FB;if(*((int*)_tmp489)!= 2)goto _LL2FB;_tmp48A=((struct Cyc_Absyn_Param_b_struct*)
_tmp489)->f1;if(!(!_tmp48A->escapes))goto _LL2FB;_LL2FA: {void*_tmp48B=(void*)
_tmp3F0->r;void*_tmp48C;struct Cyc_List_List*_tmp48D;struct Cyc_List_List _tmp48E;
struct Cyc_Absyn_Exp*_tmp48F;_LL2FE: if(*((int*)_tmp48B)!= 3)goto _LL300;_tmp48C=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp48B)->f1;if((int)_tmp48C != 19)goto
_LL300;_tmp48D=((struct Cyc_Absyn_Primop_e_struct*)_tmp48B)->f2;if(_tmp48D == 0)
goto _LL300;_tmp48E=*_tmp48D;_tmp48F=(struct Cyc_Absyn_Exp*)_tmp48E.hd;_LL2FF: {
void*_tmp490=(void*)_tmp48F->r;void*_tmp491;struct Cyc_Absyn_Vardecl*_tmp492;void*
_tmp493;struct Cyc_Absyn_Vardecl*_tmp494;void*_tmp495;struct Cyc_Absyn_Vardecl*
_tmp496;void*_tmp497;struct Cyc_Absyn_Vardecl*_tmp498;_LL303: if(*((int*)_tmp490)
!= 1)goto _LL305;_tmp491=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp490)->f2;if(
_tmp491 <= (void*)1)goto _LL305;if(*((int*)_tmp491)!= 0)goto _LL305;_tmp492=((
struct Cyc_Absyn_Global_b_struct*)_tmp491)->f1;if(!(!_tmp492->escapes))goto _LL305;
_LL304: _tmp494=_tmp492;goto _LL306;_LL305: if(*((int*)_tmp490)!= 1)goto _LL307;
_tmp493=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp490)->f2;if(_tmp493 <= (void*)
1)goto _LL307;if(*((int*)_tmp493)!= 3)goto _LL307;_tmp494=((struct Cyc_Absyn_Local_b_struct*)
_tmp493)->f1;if(!(!_tmp494->escapes))goto _LL307;_LL306: _tmp496=_tmp494;goto
_LL308;_LL307: if(*((int*)_tmp490)!= 1)goto _LL309;_tmp495=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp490)->f2;if(_tmp495 <= (void*)1)goto _LL309;if(*((int*)_tmp495)!= 4)goto _LL309;
_tmp496=((struct Cyc_Absyn_Pat_b_struct*)_tmp495)->f1;if(!(!_tmp496->escapes))
goto _LL309;_LL308: _tmp498=_tmp496;goto _LL30A;_LL309: if(*((int*)_tmp490)!= 1)goto
_LL30B;_tmp497=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp490)->f2;if(_tmp497 <= (
void*)1)goto _LL30B;if(*((int*)_tmp497)!= 2)goto _LL30B;_tmp498=((struct Cyc_Absyn_Param_b_struct*)
_tmp497)->f1;if(!(!_tmp498->escapes))goto _LL30B;_LL30A: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp499=env->fenv;struct Cyc_List_List*_tmp49A=({struct Cyc_List_List*_tmp49D=
_region_malloc(_tmp499->r,sizeof(*_tmp49D));_tmp49D->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp49E=_region_malloc(_tmp499->r,sizeof(*_tmp49E));_tmp49E->vd=_tmp48A;_tmp49E->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp49F;(
_tmp49F.LessEqSize).tag=4;(_tmp49F.LessEqSize).f1=_tmp498;_tmp49F;});_tmp49E;});
_tmp49D->tl=_tmp3EC;_tmp49D;});return({struct _tuple8 _tmp49B;_tmp49B.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp49C;(_tmp49C.ReachableFL).tag=1;(_tmp49C.ReachableFL).f1=
_tmp3EB;(_tmp49C.ReachableFL).f2=_tmp49A;(_tmp49C.ReachableFL).f3=_tmp3ED;
_tmp49C;});_tmp49B.f2=_tmp47B;_tmp49B;});}_LL30B:;_LL30C: return({struct _tuple8
_tmp4A0;_tmp4A0.f1=_tmp47A;_tmp4A0.f2=_tmp47B;_tmp4A0;});_LL302:;}_LL300:;_LL301:
return({struct _tuple8 _tmp4A1;_tmp4A1.f1=_tmp47A;_tmp4A1.f2=_tmp47B;_tmp4A1;});
_LL2FD:;}_LL2FB:;_LL2FC: return({struct _tuple8 _tmp4A2;_tmp4A2.f1=_tmp47A;_tmp4A2.f2=
_tmp47B;_tmp4A2;});_LL2F2:;}}_LL2C1:;_LL2C2: return({struct _tuple8 _tmp4A3;_tmp4A3.f1=
f;_tmp4A3.f2=f;_tmp4A3;});_LL2B4:;}}_LL281:;}}_LL278:;_LL279: goto _LL26B;_LL26B:;}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp4A5;void*_tmp4A6;struct _tuple5 _tmp4A4=Cyc_NewControlFlow_anal_Rexp(
env,inflow,e);_tmp4A5=_tmp4A4.f1;_tmp4A6=_tmp4A4.f2;_tmp4A5=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp4A5);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4A7=_tmp4A5;struct Cyc_Dict_Dict
_tmp4A8;_LL30E: if((_tmp4A7.BottomFL).tag != 0)goto _LL310;_LL30F: return({struct
_tuple8 _tmp4A9;_tmp4A9.f1=_tmp4A5;_tmp4A9.f2=_tmp4A5;_tmp4A9;});_LL310: if((
_tmp4A7.ReachableFL).tag != 1)goto _LL30D;_tmp4A8=(_tmp4A7.ReachableFL).f1;_LL311: {
void*_tmp4AA=_tmp4A6;void*_tmp4AB;void*_tmp4AC;void*_tmp4AD;_LL313: if((int)
_tmp4AA != 0)goto _LL315;_LL314: return({struct _tuple8 _tmp4AE;_tmp4AE.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4AF;(_tmp4AF.BottomFL).tag=0;_tmp4AF;});
_tmp4AE.f2=_tmp4A5;_tmp4AE;});_LL315: if((int)_tmp4AA != 2)goto _LL317;_LL316: goto
_LL318;_LL317: if((int)_tmp4AA != 1)goto _LL319;_LL318: goto _LL31A;_LL319: if(_tmp4AA
<= (void*)3)goto _LL31B;if(*((int*)_tmp4AA)!= 2)goto _LL31B;_LL31A: return({struct
_tuple8 _tmp4B0;_tmp4B0.f1=_tmp4A5;_tmp4B0.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4B1;(_tmp4B1.BottomFL).tag=0;_tmp4B1;});
_tmp4B0;});_LL31B: if(_tmp4AA <= (void*)3)goto _LL31D;if(*((int*)_tmp4AA)!= 0)goto
_LL31D;_tmp4AB=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp4AA)->f1;if((
int)_tmp4AB != 0)goto _LL31D;_LL31C: goto _LL31E;_LL31D: if(_tmp4AA <= (void*)3)goto
_LL31F;if(*((int*)_tmp4AA)!= 1)goto _LL31F;_tmp4AC=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp4AA)->f1;if((int)_tmp4AC != 0)goto _LL31F;_LL31E:({void*_tmp4B2=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmp4B3="expression may not be initialized";_tag_dynforward(
_tmp4B3,sizeof(char),_get_zero_arr_size_char(_tmp4B3,34));}),_tag_dynforward(
_tmp4B2,sizeof(void*),0));});return({struct _tuple8 _tmp4B4;_tmp4B4.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4B6;(_tmp4B6.BottomFL).tag=0;_tmp4B6;});
_tmp4B4.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4B5;(_tmp4B5.BottomFL).tag=0;_tmp4B5;});_tmp4B4;});_LL31F: if(_tmp4AA <= (void*)
3)goto _LL321;if(*((int*)_tmp4AA)!= 0)goto _LL321;_tmp4AD=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp4AA)->f1;_LL320: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4A5,e,
_tmp4AD);_LL321: if(_tmp4AA <= (void*)3)goto _LL323;if(*((int*)_tmp4AA)!= 1)goto
_LL323;_LL322: return({struct _tuple8 _tmp4B7;_tmp4B7.f1=_tmp4A5;_tmp4B7.f2=_tmp4A5;
_tmp4B7;});_LL323: if(_tmp4AA <= (void*)3)goto _LL325;if(*((int*)_tmp4AA)!= 3)goto
_LL325;_LL324: return({struct _tuple8 _tmp4B8;_tmp4B8.f1=_tmp4A5;_tmp4B8.f2=_tmp4A5;
_tmp4B8;});_LL325: if(_tmp4AA <= (void*)3)goto _LL312;if(*((int*)_tmp4AA)!= 4)goto
_LL312;_LL326:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp4B9=
_cycalloc(sizeof(*_tmp4B9));_tmp4B9[0]=({struct Cyc_Core_Impossible_struct _tmp4BA;
_tmp4BA.tag=Cyc_Core_Impossible;_tmp4BA.f1=({const char*_tmp4BB="anal_test";
_tag_dynforward(_tmp4BB,sizeof(char),_get_zero_arr_size_char(_tmp4BB,10));});
_tmp4BA;});_tmp4B9;}));_LL312:;}_LL30D:;}}}static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
flow){union Cyc_CfFlowInfo_FlowInfo_union _tmp4BC=flow;struct Cyc_Dict_Dict _tmp4BD;
_LL328: if((_tmp4BC.BottomFL).tag != 0)goto _LL32A;_LL329: return;_LL32A: if((_tmp4BC.ReachableFL).tag
!= 1)goto _LL327;_tmp4BD=(_tmp4BC.ReachableFL).f1;_LL32B:{struct Cyc_List_List*
_tmp4BE=env->param_roots;for(0;_tmp4BE != 0;_tmp4BE=_tmp4BE->tl){if(Cyc_CfFlowInfo_initlevel(
_tmp4BD,Cyc_CfFlowInfo_lookup_place(_tmp4BD,(struct Cyc_CfFlowInfo_Place*)_tmp4BE->hd))
!= (void*)2)({void*_tmp4BF=0;Cyc_Tcutil_terr(loc,({const char*_tmp4C0="function may not initialize all the parameters with attribute 'initializes'";
_tag_dynforward(_tmp4C0,sizeof(char),_get_zero_arr_size_char(_tmp4C0,76));}),
_tag_dynforward(_tmp4BF,sizeof(void*),0));});}}return;_LL327:;}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp4C1=env->fenv;
for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp4C3;struct Cyc_Core_Opt*
_tmp4C4;struct Cyc_Absyn_Exp*_tmp4C5;struct Cyc_Absyn_Stmt*_tmp4C6;struct Cyc_Position_Segment*
_tmp4C7;struct Cyc_Absyn_Switch_clause*_tmp4C2=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp4C3=*_tmp4C2;_tmp4C4=_tmp4C3.pat_vars;_tmp4C5=_tmp4C3.where_clause;
_tmp4C6=_tmp4C3.body;_tmp4C7=_tmp4C3.loc;{union Cyc_CfFlowInfo_FlowInfo_union
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp4C1,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp4C4))->v,(void*)_tmp4C1->unknown_all,_tmp4C7);
union Cyc_CfFlowInfo_FlowInfo_union clause_outflow;if(_tmp4C5 != 0){struct Cyc_Absyn_Exp*
wexp=(struct Cyc_Absyn_Exp*)_tmp4C5;union Cyc_CfFlowInfo_FlowInfo_union _tmp4C9;
union Cyc_CfFlowInfo_FlowInfo_union _tmp4CA;struct _tuple8 _tmp4C8=Cyc_NewControlFlow_anal_test(
env,clause_inflow,wexp);_tmp4C9=_tmp4C8.f1;_tmp4CA=_tmp4C8.f2;_tmp4C9=Cyc_CfFlowInfo_readthrough_unique_rvals(
wexp->loc,_tmp4C9);_tmp4CA=Cyc_CfFlowInfo_readthrough_unique_rvals(wexp->loc,
_tmp4CA);inflow=_tmp4CA;clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp4C9,
_tmp4C6);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,
_tmp4C6);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp4CB=clause_outflow;_LL32D: if((
_tmp4CB.BottomFL).tag != 0)goto _LL32F;_LL32E: goto _LL32C;_LL32F:;_LL330: if(scs->tl
== 0)return clause_outflow;else{if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(
scs->tl))->hd)->pat_vars))->v != 0)({void*_tmp4CC=0;Cyc_Tcutil_terr(_tmp4C6->loc,({
const char*_tmp4CD="switch clause may implicitly fallthru";_tag_dynforward(
_tmp4CD,sizeof(char),_get_zero_arr_size_char(_tmp4CD,38));}),_tag_dynforward(
_tmp4CC,sizeof(void*),0));});else{({void*_tmp4CE=0;Cyc_Tcutil_warn(_tmp4C6->loc,({
const char*_tmp4CF="switch clause may implicitly fallthru";_tag_dynforward(
_tmp4CF,sizeof(char),_get_zero_arr_size_char(_tmp4CF,38));}),_tag_dynforward(
_tmp4CE,sizeof(void*),0));});}Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}goto
_LL32C;_LL32C:;}}}return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4D0;(_tmp4D0.BottomFL).tag=0;_tmp4D0;});}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union outflow;struct
Cyc_NewControlFlow_CFStmtAnnot*_tmp4D2;union Cyc_CfFlowInfo_FlowInfo_union*
_tmp4D3;struct _tuple9 _tmp4D1=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);
_tmp4D2=_tmp4D1.f1;_tmp4D3=_tmp4D1.f2;inflow=*_tmp4D3;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp4D4=env->fenv;{void*_tmp4D5=(void*)s->r;struct Cyc_Absyn_Exp*_tmp4D6;struct
Cyc_Absyn_Exp*_tmp4D7;struct Cyc_Absyn_Exp*_tmp4D8;struct Cyc_Absyn_Stmt*_tmp4D9;
struct Cyc_Absyn_Stmt*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_Absyn_Stmt*
_tmp4DC;struct Cyc_Absyn_Stmt*_tmp4DD;struct _tuple3 _tmp4DE;struct Cyc_Absyn_Exp*
_tmp4DF;struct Cyc_Absyn_Stmt*_tmp4E0;struct Cyc_Absyn_Stmt*_tmp4E1;struct Cyc_Absyn_Stmt*
_tmp4E2;struct _tuple3 _tmp4E3;struct Cyc_Absyn_Exp*_tmp4E4;struct Cyc_Absyn_Stmt*
_tmp4E5;struct Cyc_Absyn_Exp*_tmp4E6;struct _tuple3 _tmp4E7;struct Cyc_Absyn_Exp*
_tmp4E8;struct Cyc_Absyn_Stmt*_tmp4E9;struct _tuple3 _tmp4EA;struct Cyc_Absyn_Exp*
_tmp4EB;struct Cyc_Absyn_Stmt*_tmp4EC;struct Cyc_Absyn_Stmt*_tmp4ED;struct Cyc_Absyn_Stmt*
_tmp4EE;struct Cyc_List_List*_tmp4EF;struct Cyc_Absyn_Switch_clause**_tmp4F0;
struct Cyc_Absyn_Switch_clause*_tmp4F1;struct Cyc_Absyn_Stmt*_tmp4F2;struct Cyc_Absyn_Stmt*
_tmp4F3;struct Cyc_Absyn_Stmt*_tmp4F4;struct Cyc_Absyn_Exp*_tmp4F5;struct Cyc_List_List*
_tmp4F6;struct Cyc_Absyn_Stmt*_tmp4F7;struct Cyc_List_List*_tmp4F8;struct Cyc_Absyn_Decl*
_tmp4F9;struct Cyc_Absyn_Stmt*_tmp4FA;struct Cyc_Absyn_Stmt*_tmp4FB;struct Cyc_Absyn_Tvar*
_tmp4FC;struct Cyc_Absyn_Vardecl*_tmp4FD;int _tmp4FE;struct Cyc_Absyn_Exp*_tmp4FF;
struct Cyc_Absyn_Stmt*_tmp500;struct Cyc_Absyn_Exp*_tmp501;struct Cyc_Absyn_Exp*
_tmp502;struct Cyc_Absyn_Tvar*_tmp503;struct Cyc_Absyn_Vardecl*_tmp504;struct Cyc_Absyn_Stmt*
_tmp505;_LL332: if((int)_tmp4D5 != 0)goto _LL334;_LL333: return inflow;_LL334: if(
_tmp4D5 <= (void*)1)goto _LL35C;if(*((int*)_tmp4D5)!= 2)goto _LL336;_tmp4D6=((
struct Cyc_Absyn_Return_s_struct*)_tmp4D5)->f1;if(_tmp4D6 != 0)goto _LL336;_LL335:
if(env->noreturn)({void*_tmp506=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp507="`noreturn' function might return";
_tag_dynforward(_tmp507,sizeof(char),_get_zero_arr_size_char(_tmp507,33));}),
_tag_dynforward(_tmp506,sizeof(void*),0));});Cyc_NewControlFlow_check_init_params(
s->loc,env,inflow);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp508;(_tmp508.BottomFL).tag=0;_tmp508;});_LL336: if(*((int*)_tmp4D5)!= 2)goto
_LL338;_tmp4D7=((struct Cyc_Absyn_Return_s_struct*)_tmp4D5)->f1;_LL337: if(env->noreturn)({
void*_tmp509=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp50A="`noreturn' function might return";
_tag_dynforward(_tmp50A,sizeof(char),_get_zero_arr_size_char(_tmp50A,33));}),
_tag_dynforward(_tmp509,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp50C;void*_tmp50D;struct _tuple5 _tmp50B=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp4D7));_tmp50C=_tmp50B.f1;_tmp50D=
_tmp50B.f2;_tmp50C=Cyc_CfFlowInfo_consume_unique_rvals(_tmp4D7->loc,_tmp50C);
_tmp50C=Cyc_NewControlFlow_use_Rval(env,_tmp4D7->loc,_tmp50C,_tmp50D);Cyc_NewControlFlow_check_init_params(
s->loc,env,_tmp50C);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp50E;(_tmp50E.BottomFL).tag=0;_tmp50E;});}_LL338: if(*((int*)_tmp4D5)!= 0)goto
_LL33A;_tmp4D8=((struct Cyc_Absyn_Exp_s_struct*)_tmp4D5)->f1;_LL339: outflow=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4D8)).f1;goto _LL331;_LL33A: if(*((int*)_tmp4D5)!= 1)goto _LL33C;
_tmp4D9=((struct Cyc_Absyn_Seq_s_struct*)_tmp4D5)->f1;_tmp4DA=((struct Cyc_Absyn_Seq_s_struct*)
_tmp4D5)->f2;_LL33B: outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp4D9),_tmp4DA);goto _LL331;_LL33C: if(*((int*)_tmp4D5)!= 3)goto
_LL33E;_tmp4DB=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4D5)->f1;_tmp4DC=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4D5)->f2;_tmp4DD=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp4D5)->f3;_LL33D: {union Cyc_CfFlowInfo_FlowInfo_union _tmp510;union Cyc_CfFlowInfo_FlowInfo_union
_tmp511;struct _tuple8 _tmp50F=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4DB);
_tmp510=_tmp50F.f1;_tmp511=_tmp50F.f2;_tmp510=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4DB->loc,_tmp510);_tmp511=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4DB->loc,
_tmp511);outflow=Cyc_CfFlowInfo_join_flow(_tmp4D4,env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp510,_tmp4DC),Cyc_NewControlFlow_anal_stmt(env,_tmp511,_tmp4DD),1);goto
_LL331;}_LL33E: if(*((int*)_tmp4D5)!= 4)goto _LL340;_tmp4DE=((struct Cyc_Absyn_While_s_struct*)
_tmp4D5)->f1;_tmp4DF=_tmp4DE.f1;_tmp4E0=_tmp4DE.f2;_tmp4E1=((struct Cyc_Absyn_While_s_struct*)
_tmp4D5)->f2;_LL33F: {union Cyc_CfFlowInfo_FlowInfo_union*_tmp513;struct _tuple9
_tmp512=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp4E0);_tmp513=_tmp512.f2;{
union Cyc_CfFlowInfo_FlowInfo_union _tmp514=*_tmp513;union Cyc_CfFlowInfo_FlowInfo_union
_tmp516;union Cyc_CfFlowInfo_FlowInfo_union _tmp517;struct _tuple8 _tmp515=Cyc_NewControlFlow_anal_test(
env,_tmp514,_tmp4DF);_tmp516=_tmp515.f1;_tmp517=_tmp515.f2;_tmp516=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4DF->loc,_tmp516);_tmp517=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4DF->loc,
_tmp517);{union Cyc_CfFlowInfo_FlowInfo_union _tmp518=Cyc_NewControlFlow_anal_stmt(
env,_tmp516,_tmp4E1);Cyc_NewControlFlow_update_flow(env,_tmp4E0,_tmp518);outflow=
_tmp517;goto _LL331;}}}_LL340: if(*((int*)_tmp4D5)!= 13)goto _LL342;_tmp4E2=((
struct Cyc_Absyn_Do_s_struct*)_tmp4D5)->f1;_tmp4E3=((struct Cyc_Absyn_Do_s_struct*)
_tmp4D5)->f2;_tmp4E4=_tmp4E3.f1;_tmp4E5=_tmp4E3.f2;_LL341: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp519=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4E2);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp51B;struct _tuple9 _tmp51A=Cyc_NewControlFlow_pre_stmt_check(env,_tmp519,
_tmp4E5);_tmp51B=_tmp51A.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp51C=*_tmp51B;
union Cyc_CfFlowInfo_FlowInfo_union _tmp51E;union Cyc_CfFlowInfo_FlowInfo_union
_tmp51F;struct _tuple8 _tmp51D=Cyc_NewControlFlow_anal_test(env,_tmp51C,_tmp4E4);
_tmp51E=_tmp51D.f1;_tmp51F=_tmp51D.f2;Cyc_NewControlFlow_update_flow(env,_tmp4E2,
_tmp51E);outflow=_tmp51F;goto _LL331;}}_LL342: if(*((int*)_tmp4D5)!= 8)goto _LL344;
_tmp4E6=((struct Cyc_Absyn_For_s_struct*)_tmp4D5)->f1;_tmp4E7=((struct Cyc_Absyn_For_s_struct*)
_tmp4D5)->f2;_tmp4E8=_tmp4E7.f1;_tmp4E9=_tmp4E7.f2;_tmp4EA=((struct Cyc_Absyn_For_s_struct*)
_tmp4D5)->f3;_tmp4EB=_tmp4EA.f1;_tmp4EC=_tmp4EA.f2;_tmp4ED=((struct Cyc_Absyn_For_s_struct*)
_tmp4D5)->f4;_LL343: {union Cyc_CfFlowInfo_FlowInfo_union _tmp520=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4E6)).f1;_tmp520=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4E6->loc,
_tmp520);{union Cyc_CfFlowInfo_FlowInfo_union*_tmp522;struct _tuple9 _tmp521=Cyc_NewControlFlow_pre_stmt_check(
env,_tmp520,_tmp4E9);_tmp522=_tmp521.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp523=*_tmp522;union Cyc_CfFlowInfo_FlowInfo_union _tmp525;union Cyc_CfFlowInfo_FlowInfo_union
_tmp526;struct _tuple8 _tmp524=Cyc_NewControlFlow_anal_test(env,_tmp523,_tmp4E8);
_tmp525=_tmp524.f1;_tmp526=_tmp524.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp527=
Cyc_NewControlFlow_anal_stmt(env,_tmp525,_tmp4ED);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp529;struct _tuple9 _tmp528=Cyc_NewControlFlow_pre_stmt_check(env,_tmp527,
_tmp4EC);_tmp529=_tmp528.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp52A=*_tmp529;
union Cyc_CfFlowInfo_FlowInfo_union _tmp52B=(Cyc_NewControlFlow_anal_Rexp(env,
_tmp52A,_tmp4EB)).f1;_tmp52B=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4EB->loc,
_tmp52B);Cyc_NewControlFlow_update_flow(env,_tmp4E9,_tmp52B);outflow=_tmp526;
goto _LL331;}}}}}_LL344: if(*((int*)_tmp4D5)!= 5)goto _LL346;_tmp4EE=((struct Cyc_Absyn_Break_s_struct*)
_tmp4D5)->f1;if(_tmp4EE != 0)goto _LL346;_LL345: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp52C;(_tmp52C.BottomFL).tag=0;_tmp52C;});
_LL346: if(*((int*)_tmp4D5)!= 10)goto _LL348;_tmp4EF=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp4D5)->f1;_tmp4F0=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp4D5)->f2;if(
_tmp4F0 == 0)goto _LL348;_tmp4F1=*_tmp4F0;_LL347: {struct _RegionHandle _tmp52D=
_new_region("temp");struct _RegionHandle*temp=& _tmp52D;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp52F;struct Cyc_List_List*_tmp530;struct _tuple11
_tmp52E=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp4EF,0);
_tmp52F=_tmp52E.f1;_tmp530=_tmp52E.f2;for(0;_tmp530 != 0;(_tmp530=_tmp530->tl,
_tmp4EF=_tmp4EF->tl)){_tmp52F=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4EF))->hd)->loc,_tmp52F,(void*)_tmp530->hd);}
_tmp52F=Cyc_CfFlowInfo_consume_unique_rvals(s->loc,_tmp52F);_tmp52F=Cyc_NewControlFlow_add_vars(
_tmp4D4,_tmp52F,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4F1->pat_vars))->v,(
void*)_tmp4D4->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp4F1->body,_tmp52F);{union Cyc_CfFlowInfo_FlowInfo_union _tmp532=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp531;(_tmp531.BottomFL).tag=0;_tmp531;});
_npop_handler(0);return _tmp532;}};_pop_region(temp);}_LL348: if(*((int*)_tmp4D5)
!= 5)goto _LL34A;_tmp4F2=((struct Cyc_Absyn_Break_s_struct*)_tmp4D5)->f1;_LL349:
_tmp4F3=_tmp4F2;goto _LL34B;_LL34A: if(*((int*)_tmp4D5)!= 6)goto _LL34C;_tmp4F3=((
struct Cyc_Absyn_Continue_s_struct*)_tmp4D5)->f1;_LL34B: _tmp4F4=_tmp4F3;goto
_LL34D;_LL34C: if(*((int*)_tmp4D5)!= 7)goto _LL34E;_tmp4F4=((struct Cyc_Absyn_Goto_s_struct*)
_tmp4D5)->f2;_LL34D: if(env->iteration_num == 1){struct Cyc_Absyn_Stmt*_tmp533=
_tmp4D2->encloser;struct Cyc_Absyn_Stmt*_tmp534=(Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)_check_null(_tmp4F4)))->encloser;while(_tmp534 != _tmp533){
struct Cyc_Absyn_Stmt*_tmp535=(Cyc_NewControlFlow_get_stmt_annot(_tmp533))->encloser;
if(_tmp535 == _tmp533){({void*_tmp536=0;Cyc_Tcutil_terr(s->loc,({const char*
_tmp537="goto enters local scope or exception handler";_tag_dynforward(_tmp537,
sizeof(char),_get_zero_arr_size_char(_tmp537,45));}),_tag_dynforward(_tmp536,
sizeof(void*),0));});break;}_tmp533=_tmp535;}}Cyc_NewControlFlow_update_flow(env,(
struct Cyc_Absyn_Stmt*)_check_null(_tmp4F4),inflow);return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp538;(_tmp538.BottomFL).tag=0;_tmp538;});
_LL34E: if(*((int*)_tmp4D5)!= 9)goto _LL350;_tmp4F5=((struct Cyc_Absyn_Switch_s_struct*)
_tmp4D5)->f1;_tmp4F6=((struct Cyc_Absyn_Switch_s_struct*)_tmp4D5)->f2;_LL34F: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp53A;void*_tmp53B;struct _tuple5 _tmp539=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4F5);_tmp53A=_tmp539.f1;_tmp53B=_tmp539.f2;_tmp53A=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4F5->loc,_tmp53A);_tmp53A=Cyc_NewControlFlow_use_Rval(env,_tmp4F5->loc,
_tmp53A,_tmp53B);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp53A,_tmp4F6);goto
_LL331;}_LL350: if(*((int*)_tmp4D5)!= 14)goto _LL352;_tmp4F7=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp4D5)->f1;_tmp4F8=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp4D5)->f2;_LL351: {
int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo_union old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo_union s1_outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4F7);union Cyc_CfFlowInfo_FlowInfo_union
scs_inflow=env->tryflow;env->in_try=old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(
env,scs_inflow);{union Cyc_CfFlowInfo_FlowInfo_union scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp4F8);{union Cyc_CfFlowInfo_FlowInfo_union _tmp53C=scs_outflow;
_LL35F: if((_tmp53C.BottomFL).tag != 0)goto _LL361;_LL360: goto _LL35E;_LL361:;_LL362:({
void*_tmp53D=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp53E="last catch clause may implicitly fallthru";
_tag_dynforward(_tmp53E,sizeof(char),_get_zero_arr_size_char(_tmp53E,42));}),
_tag_dynforward(_tmp53D,sizeof(void*),0));});_LL35E:;}outflow=s1_outflow;goto
_LL331;}}}_LL352: if(*((int*)_tmp4D5)!= 11)goto _LL354;_tmp4F9=((struct Cyc_Absyn_Decl_s_struct*)
_tmp4D5)->f1;_tmp4FA=((struct Cyc_Absyn_Decl_s_struct*)_tmp4D5)->f2;_LL353:
outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,
_tmp4F9),_tmp4FA);goto _LL331;_LL354: if(*((int*)_tmp4D5)!= 12)goto _LL356;_tmp4FB=((
struct Cyc_Absyn_Label_s_struct*)_tmp4D5)->f2;_LL355: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp4FB);goto _LL331;_LL356: if(*((int*)_tmp4D5)!= 15)goto _LL358;
_tmp4FC=((struct Cyc_Absyn_Region_s_struct*)_tmp4D5)->f1;_tmp4FD=((struct Cyc_Absyn_Region_s_struct*)
_tmp4D5)->f2;_tmp4FE=((struct Cyc_Absyn_Region_s_struct*)_tmp4D5)->f3;_tmp4FF=((
struct Cyc_Absyn_Region_s_struct*)_tmp4D5)->f4;_tmp500=((struct Cyc_Absyn_Region_s_struct*)
_tmp4D5)->f5;_LL357: if(_tmp4FF != 0){struct Cyc_Absyn_Exp*_tmp53F=(struct Cyc_Absyn_Exp*)
_tmp4FF;union Cyc_CfFlowInfo_FlowInfo_union _tmp541;void*_tmp542;struct _tuple5
_tmp540=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp53F);_tmp541=_tmp540.f1;
_tmp542=_tmp540.f2;_tmp541=Cyc_CfFlowInfo_consume_unique_rvals(_tmp53F->loc,
_tmp541);inflow=Cyc_NewControlFlow_use_Rval(env,_tmp53F->loc,_tmp541,_tmp542);}{
struct Cyc_List_List _tmp543=({struct Cyc_List_List _tmp544;_tmp544.hd=_tmp4FD;
_tmp544.tl=0;_tmp544;});inflow=Cyc_NewControlFlow_add_vars(_tmp4D4,inflow,(
struct Cyc_List_List*)& _tmp543,(void*)_tmp4D4->unknown_all,_tmp500->loc);outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp500);goto _LL331;}_LL358: if(*((int*)
_tmp4D5)!= 16)goto _LL35A;_tmp501=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp4D5)->f1;
_LL359: {union Cyc_CfFlowInfo_FlowInfo_union _tmp546;void*_tmp547;struct _tuple5
_tmp545=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp501);_tmp546=_tmp545.f1;
_tmp547=_tmp545.f2;_tmp546=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp501->loc,
_tmp546);{union Cyc_CfFlowInfo_FlowInfo_union _tmp548=Cyc_NewControlFlow_use_Rval(
env,_tmp501->loc,_tmp546,_tmp547);{void*_tmp549=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp501->topt))->v);void*_tmp54A;_LL364: if(
_tmp549 <= (void*)4)goto _LL366;if(*((int*)_tmp549)!= 15)goto _LL366;_tmp54A=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp549)->f1;_LL365: outflow=Cyc_CfFlowInfo_kill_flow_region(
_tmp4D4,_tmp546,_tmp54A);goto _LL363;_LL366:;_LL367:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B[0]=({
struct Cyc_Core_Impossible_struct _tmp54C;_tmp54C.tag=Cyc_Core_Impossible;_tmp54C.f1=({
const char*_tmp54D="anal_stmt -- reset_region";_tag_dynforward(_tmp54D,sizeof(
char),_get_zero_arr_size_char(_tmp54D,26));});_tmp54C;});_tmp54B;}));_LL363:;}
goto _LL331;}}_LL35A: if(*((int*)_tmp4D5)!= 17)goto _LL35C;_tmp502=((struct Cyc_Absyn_Alias_s_struct*)
_tmp4D5)->f1;_tmp503=((struct Cyc_Absyn_Alias_s_struct*)_tmp4D5)->f2;_tmp504=((
struct Cyc_Absyn_Alias_s_struct*)_tmp4D5)->f3;_tmp505=((struct Cyc_Absyn_Alias_s_struct*)
_tmp4D5)->f4;_LL35B: {union Cyc_CfFlowInfo_FlowInfo_union _tmp54F;void*_tmp550;
struct _tuple5 _tmp54E=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp502);_tmp54F=
_tmp54E.f1;_tmp550=_tmp54E.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp552;struct Cyc_List_List*
_tmp553;struct _tuple6 _tmp551=Cyc_CfFlowInfo_save_consume_info(_tmp4D4,_tmp54F,0);
_tmp552=_tmp551.f1;_tmp553=_tmp552.may_consume;_tmp54F=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp502->loc,_tmp54F);_tmp54F=Cyc_NewControlFlow_use_Rval(env,_tmp502->loc,
_tmp54F,_tmp550);{struct Cyc_List_List _tmp554=({struct Cyc_List_List _tmp55B;
_tmp55B.hd=_tmp504;_tmp55B.tl=0;_tmp55B;});_tmp54F=Cyc_NewControlFlow_add_vars(
_tmp4D4,_tmp54F,(struct Cyc_List_List*)& _tmp554,(void*)_tmp4D4->unknown_all,s->loc);
outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp54F,_tmp505);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp555=outflow;struct Cyc_Dict_Dict _tmp556;struct Cyc_List_List*_tmp557;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp558;_LL369: if((_tmp555.BottomFL).tag != 0)goto _LL36B;_LL36A: goto _LL368;_LL36B:
if((_tmp555.ReachableFL).tag != 1)goto _LL368;_tmp556=(_tmp555.ReachableFL).f1;
_tmp557=(_tmp555.ReachableFL).f2;_tmp558=(_tmp555.ReachableFL).f3;_LL36C: while(
_tmp553 != 0){struct Cyc_Dict_Dict _tmp559=_tmp558.consumed;_tmp558.consumed=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))
Cyc_Dict_rdelete)(_tmp4D4->r,_tmp558.consumed,(struct Cyc_CfFlowInfo_Place*)
_tmp553->hd);if((_tmp558.consumed).t != _tmp559.t);_tmp553=_tmp553->tl;}outflow=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp55A;(
_tmp55A.ReachableFL).tag=1;(_tmp55A.ReachableFL).f1=_tmp556;(_tmp55A.ReachableFL).f2=
_tmp557;(_tmp55A.ReachableFL).f3=_tmp558;_tmp55A;});goto _LL368;_LL368:;}goto
_LL331;}}}_LL35C:;_LL35D:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C[0]=({struct Cyc_Core_Impossible_struct
_tmp55D;_tmp55D.tag=Cyc_Core_Impossible;_tmp55D.f1=({const char*_tmp55E="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_dynforward(_tmp55E,sizeof(char),_get_zero_arr_size_char(_tmp55E,56));});
_tmp55D;});_tmp55C;}));_LL331:;}outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,
outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp55F=outflow;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp560;_LL36E: if((_tmp55F.ReachableFL).tag != 1)goto _LL370;_tmp560=(_tmp55F.ReachableFL).f3;
_LL36F: goto _LL36D;_LL370:;_LL371: goto _LL36D;_LL36D:;}return outflow;}}static void
Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_Absyn_Fndecl*fd);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Decl*decl){void*_tmp561=(void*)decl->r;struct Cyc_Absyn_Vardecl*
_tmp562;struct Cyc_Core_Opt*_tmp563;struct Cyc_Core_Opt _tmp564;struct Cyc_List_List*
_tmp565;struct Cyc_Absyn_Exp*_tmp566;struct Cyc_List_List*_tmp567;struct Cyc_Absyn_Fndecl*
_tmp568;_LL373: if(_tmp561 <= (void*)2)goto _LL37B;if(*((int*)_tmp561)!= 0)goto
_LL375;_tmp562=((struct Cyc_Absyn_Var_d_struct*)_tmp561)->f1;_LL374: {struct Cyc_List_List
_tmp569=({struct Cyc_List_List _tmp578;_tmp578.hd=_tmp562;_tmp578.tl=0;_tmp578;});
inflow=Cyc_NewControlFlow_add_vars(env->fenv,inflow,(struct Cyc_List_List*)&
_tmp569,(void*)(env->fenv)->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*_tmp56A=
_tmp562->initializer;if(_tmp56A == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp56C;void*_tmp56D;struct _tuple5 _tmp56B=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_tmp56A);_tmp56C=_tmp56B.f1;_tmp56D=_tmp56B.f2;
_tmp56C=Cyc_CfFlowInfo_consume_unique_rvals(_tmp56A->loc,_tmp56C);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp56E=_tmp56C;struct Cyc_Dict_Dict _tmp56F;struct Cyc_List_List*_tmp570;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp571;_LL37E: if((_tmp56E.BottomFL).tag != 0)goto _LL380;_LL37F: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp572;(_tmp572.BottomFL).tag=0;_tmp572;});
_LL380: if((_tmp56E.ReachableFL).tag != 1)goto _LL37D;_tmp56F=(_tmp56E.ReachableFL).f1;
_tmp570=(_tmp56E.ReachableFL).f2;_tmp571=(_tmp56E.ReachableFL).f3;_LL381: _tmp56F=
Cyc_CfFlowInfo_assign_place(env->fenv,decl->loc,_tmp56F,env->all_changed,({
struct Cyc_CfFlowInfo_Place*_tmp573=_region_malloc(env->r,sizeof(*_tmp573));
_tmp573->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp574=
_region_malloc(env->r,sizeof(*_tmp574));_tmp574[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp575;_tmp575.tag=0;_tmp575.f1=_tmp562;_tmp575;});_tmp574;}));_tmp573->fields=
0;_tmp573;}),_tmp56D);_tmp570=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp570,
_tmp562,(struct Cyc_Absyn_Exp*)_check_null(_tmp562->initializer));{union Cyc_CfFlowInfo_FlowInfo_union
_tmp576=({union Cyc_CfFlowInfo_FlowInfo_union _tmp577;(_tmp577.ReachableFL).tag=1;(
_tmp577.ReachableFL).f1=_tmp56F;(_tmp577.ReachableFL).f2=_tmp570;(_tmp577.ReachableFL).f3=
_tmp571;_tmp577;});Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp576);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp576;}_LL37D:;}}}}_LL375:
if(*((int*)_tmp561)!= 2)goto _LL377;_tmp563=((struct Cyc_Absyn_Let_d_struct*)
_tmp561)->f2;if(_tmp563 == 0)goto _LL377;_tmp564=*_tmp563;_tmp565=(struct Cyc_List_List*)
_tmp564.v;_tmp566=((struct Cyc_Absyn_Let_d_struct*)_tmp561)->f3;_LL376: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp57A;void*_tmp57B;struct _tuple5 _tmp579=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp566);_tmp57A=_tmp579.f1;_tmp57B=_tmp579.f2;_tmp57A=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp566->loc,_tmp57A);_tmp57A=Cyc_NewControlFlow_use_Rval(env,_tmp566->loc,
_tmp57A,_tmp57B);return Cyc_NewControlFlow_add_vars(env->fenv,_tmp57A,_tmp565,(
void*)(env->fenv)->unknown_all,decl->loc);}_LL377: if(*((int*)_tmp561)!= 3)goto
_LL379;_tmp567=((struct Cyc_Absyn_Letv_d_struct*)_tmp561)->f1;_LL378: return Cyc_NewControlFlow_add_vars(
env->fenv,inflow,_tmp567,(void*)(env->fenv)->unknown_none,decl->loc);_LL379: if(*((
int*)_tmp561)!= 1)goto _LL37B;_tmp568=((struct Cyc_Absyn_Fn_d_struct*)_tmp561)->f1;
_LL37A: Cyc_NewControlFlow_check_nested_fun(env->fenv,inflow,_tmp568);{void*
_tmp57C=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp568->cached_typ))->v;struct
Cyc_Absyn_Vardecl*_tmp57D=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp568->fn_vardecl);
return Cyc_NewControlFlow_add_vars(env->fenv,inflow,({struct Cyc_List_List*_tmp57E=
_region_malloc(env->r,sizeof(*_tmp57E));_tmp57E->hd=_tmp57D;_tmp57E->tl=0;
_tmp57E;}),(void*)(env->fenv)->unknown_all,decl->loc);}_LL37B:;_LL37C:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp57F=_cycalloc(sizeof(*
_tmp57F));_tmp57F[0]=({struct Cyc_Core_Impossible_struct _tmp580;_tmp580.tag=Cyc_Core_Impossible;
_tmp580.f1=({const char*_tmp581="anal_decl: unexpected decl variant";
_tag_dynforward(_tmp581,sizeof(char),_get_zero_arr_size_char(_tmp581,35));});
_tmp580;});_tmp57F;}));_LL372:;}static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle _tmp582=_new_region("frgn");struct _RegionHandle*frgn=&
_tmp582;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);Cyc_NewControlFlow_check_nested_fun(fenv,(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp583;(_tmp583.ReachableFL).tag=1;(_tmp583.ReachableFL).f1=
fenv->mt_flowdict;(_tmp583.ReachableFL).f2=0;(_tmp583.ReachableFL).f3=({struct
Cyc_CfFlowInfo_ConsumeInfo _tmp584;_tmp584.consumed=fenv->mt_place_set;_tmp584.may_consume=
0;_tmp584;});_tmp583;}),fd);};_pop_region(frgn);}static int Cyc_NewControlFlow_hash_ptr(
void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp585=fenv->r;struct Cyc_Position_Segment*_tmp586=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(void*)fenv->unknown_all,_tmp586);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo_union _tmp587=inflow;struct Cyc_Dict_Dict
_tmp588;struct Cyc_List_List*_tmp589;struct Cyc_CfFlowInfo_ConsumeInfo _tmp58A;
_LL383: if((_tmp587.BottomFL).tag != 0)goto _LL385;_LL384:({void*_tmp58B=0;((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp58C="check_fun";_tag_dynforward(_tmp58C,sizeof(char),_get_zero_arr_size_char(
_tmp58C,10));}),_tag_dynforward(_tmp58B,sizeof(void*),0));});_LL385: if((_tmp587.ReachableFL).tag
!= 1)goto _LL382;_tmp588=(_tmp587.ReachableFL).f1;_tmp589=(_tmp587.ReachableFL).f2;
_tmp58A=(_tmp587.ReachableFL).f3;_LL386: {struct Cyc_List_List*atts;{void*_tmp58D=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v);
struct Cyc_Absyn_FnInfo _tmp58E;struct Cyc_List_List*_tmp58F;_LL388: if(_tmp58D <= (
void*)4)goto _LL38A;if(*((int*)_tmp58D)!= 8)goto _LL38A;_tmp58E=((struct Cyc_Absyn_FnType_struct*)
_tmp58D)->f1;_tmp58F=_tmp58E.attributes;_LL389: atts=_tmp58F;goto _LL387;_LL38A:;
_LL38B:({void*_tmp590=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr
ap))Cyc_Tcutil_impos)(({const char*_tmp591="check_fun: non-function type cached with fndecl_t";
_tag_dynforward(_tmp591,sizeof(char),_get_zero_arr_size_char(_tmp591,50));}),
_tag_dynforward(_tmp590,sizeof(void*),0));});_LL387:;}for(0;atts != 0;atts=atts->tl){
void*_tmp592=(void*)atts->hd;int _tmp593;_LL38D: if(_tmp592 <= (void*)17)goto _LL38F;
if(*((int*)_tmp592)!= 4)goto _LL38F;_tmp593=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp592)->f1;_LL38E: {unsigned int j=(unsigned int)_tmp593;if(j > ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v)){({void*_tmp594=0;Cyc_Tcutil_terr(_tmp586,({
const char*_tmp595="initializes attribute exceeds number of parameters";
_tag_dynforward(_tmp595,sizeof(char),_get_zero_arr_size_char(_tmp595,51));}),
_tag_dynforward(_tmp594,sizeof(void*),0));});continue;}{struct Cyc_Absyn_Vardecl*
_tmp596=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(
int)(j - 1));{void*_tmp597=Cyc_Tcutil_compress((void*)_tmp596->type);struct Cyc_Absyn_PtrInfo
_tmp598;void*_tmp599;struct Cyc_Absyn_PtrAtts _tmp59A;struct Cyc_Absyn_Conref*
_tmp59B;struct Cyc_Absyn_Conref*_tmp59C;struct Cyc_Absyn_Conref*_tmp59D;_LL392: if(
_tmp597 <= (void*)4)goto _LL394;if(*((int*)_tmp597)!= 4)goto _LL394;_tmp598=((
struct Cyc_Absyn_PointerType_struct*)_tmp597)->f1;_tmp599=(void*)_tmp598.elt_typ;
_tmp59A=_tmp598.ptr_atts;_tmp59B=_tmp59A.nullable;_tmp59C=_tmp59A.bounds;_tmp59D=
_tmp59A.zero_term;_LL393: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp59B))({void*_tmp59E=0;Cyc_Tcutil_terr(_tmp586,({const char*_tmp59F="initializes attribute not allowed on nullable pointers";
_tag_dynforward(_tmp59F,sizeof(char),_get_zero_arr_size_char(_tmp59F,55));}),
_tag_dynforward(_tmp59E,sizeof(void*),0));});if(!Cyc_Tcutil_is_bound_one(_tmp59C))({
void*_tmp5A0=0;Cyc_Tcutil_terr(_tmp586,({const char*_tmp5A1="initializes attribute allowed only on pointers of size 1";
_tag_dynforward(_tmp5A1,sizeof(char),_get_zero_arr_size_char(_tmp5A1,57));}),
_tag_dynforward(_tmp5A0,sizeof(void*),0));});if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp59D))({void*_tmp5A2=0;Cyc_Tcutil_terr(_tmp586,({
const char*_tmp5A3="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_dynforward(_tmp5A3,sizeof(char),_get_zero_arr_size_char(_tmp5A3,77));}),
_tag_dynforward(_tmp5A2,sizeof(void*),0));});{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp5A4=({struct Cyc_CfFlowInfo_InitParam_struct*_tmp5AC=_region_malloc(_tmp585,
sizeof(*_tmp5AC));_tmp5AC[0]=({struct Cyc_CfFlowInfo_InitParam_struct _tmp5AD;
_tmp5AD.tag=2;_tmp5AD.f1=(int)j;_tmp5AD.f2=(void*)_tmp599;_tmp5AD;});_tmp5AC;});
struct Cyc_CfFlowInfo_Place*_tmp5A5=({struct Cyc_CfFlowInfo_Place*_tmp5AB=
_region_malloc(_tmp585,sizeof(*_tmp5AB));_tmp5AB->root=(void*)((void*)_tmp5A4);
_tmp5AB->fields=0;_tmp5AB;});_tmp588=Cyc_Dict_insert(_tmp588,(void*)_tmp5A4,Cyc_CfFlowInfo_typ_to_absrval(
fenv,_tmp599,(void*)fenv->esc_none));_tmp588=Cyc_Dict_insert(_tmp588,(void*)({
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp5A6=_region_malloc(_tmp585,sizeof(*
_tmp5A6));_tmp5A6[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp5A7;_tmp5A7.tag=0;
_tmp5A7.f1=_tmp596;_tmp5A7;});_tmp5A6;}),(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp5A8=_region_malloc(_tmp585,sizeof(*_tmp5A8));_tmp5A8[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp5A9;_tmp5A9.tag=2;_tmp5A9.f1=_tmp5A5;_tmp5A9;});_tmp5A8;}));param_roots=({
struct Cyc_List_List*_tmp5AA=_region_malloc(_tmp585,sizeof(*_tmp5AA));_tmp5AA->hd=
_tmp5A5;_tmp5AA->tl=param_roots;_tmp5AA;});goto _LL391;}_LL394:;_LL395:({void*
_tmp5AE=0;Cyc_Tcutil_terr(_tmp586,({const char*_tmp5AF="initializes attribute on non-pointer";
_tag_dynforward(_tmp5AF,sizeof(char),_get_zero_arr_size_char(_tmp5AF,37));}),
_tag_dynforward(_tmp5AE,sizeof(void*),0));});_LL391:;}goto _LL38C;}}_LL38F:;
_LL390: goto _LL38C;_LL38C:;}inflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp5B0;(_tmp5B0.ReachableFL).tag=1;(_tmp5B0.ReachableFL).f1=_tmp588;(_tmp5B0.ReachableFL).f2=
_tmp589;(_tmp5B0.ReachableFL).f3=_tmp58A;_tmp5B0;});}_LL382:;}{int noreturn=Cyc_Tcutil_is_noreturn(
Cyc_Tcutil_fndecl2typ(fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(
struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),
int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp585,33,(int(*)(
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp5B9=_region_malloc(_tmp585,sizeof(*_tmp5B9));_tmp5B9->r=_tmp585;_tmp5B9->fenv=
fenv;_tmp5B9->iterate_again=1;_tmp5B9->iteration_num=0;_tmp5B9->in_try=0;_tmp5B9->tryflow=
inflow;_tmp5B9->all_changed=0;_tmp5B9->noreturn=noreturn;_tmp5B9->param_roots=
param_roots;_tmp5B9->flow_table=flow_table;_tmp5B9;});union Cyc_CfFlowInfo_FlowInfo_union
outflow=inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5B1=outflow;_LL397: if((_tmp5B1.BottomFL).tag != 0)goto _LL399;_LL398: goto
_LL396;_LL399:;_LL39A: Cyc_NewControlFlow_check_init_params(_tmp586,env,outflow);
if(noreturn)({void*_tmp5B2=0;Cyc_Tcutil_terr(_tmp586,({const char*_tmp5B3="`noreturn' function might (implicitly) return";
_tag_dynforward(_tmp5B3,sizeof(char),_get_zero_arr_size_char(_tmp5B3,46));}),
_tag_dynforward(_tmp5B2,sizeof(void*),0));});else{void*_tmp5B4=Cyc_Tcutil_compress((
void*)fd->ret_type);_LL39C: if((int)_tmp5B4 != 0)goto _LL39E;_LL39D: goto _LL39B;
_LL39E: if(_tmp5B4 <= (void*)4)goto _LL3A0;if(*((int*)_tmp5B4)!= 6)goto _LL3A0;
_LL39F: goto _LL3A1;_LL3A0: if((int)_tmp5B4 != 1)goto _LL3A2;_LL3A1: goto _LL3A3;_LL3A2:
if(_tmp5B4 <= (void*)4)goto _LL3A4;if(*((int*)_tmp5B4)!= 5)goto _LL3A4;_LL3A3:({
void*_tmp5B5=0;Cyc_Tcutil_warn(_tmp586,({const char*_tmp5B6="function may not return a value";
_tag_dynforward(_tmp5B6,sizeof(char),_get_zero_arr_size_char(_tmp5B6,32));}),
_tag_dynforward(_tmp5B5,sizeof(void*),0));});goto _LL39B;_LL3A4:;_LL3A5:({void*
_tmp5B7=0;Cyc_Tcutil_terr(_tmp586,({const char*_tmp5B8="function may not return a value";
_tag_dynforward(_tmp5B8,sizeof(char),_get_zero_arr_size_char(_tmp5B8,32));}),
_tag_dynforward(_tmp5B7,sizeof(void*),0));});goto _LL39B;_LL39B:;}goto _LL396;
_LL396:;}}}}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){for(0;ds != 0;
ds=ds->tl){void*_tmp5BA=(void*)((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*
_tmp5BB;struct Cyc_List_List*_tmp5BC;struct Cyc_List_List*_tmp5BD;struct Cyc_List_List*
_tmp5BE;_LL3A7: if(_tmp5BA <= (void*)2)goto _LL3B1;if(*((int*)_tmp5BA)!= 1)goto
_LL3A9;_tmp5BB=((struct Cyc_Absyn_Fn_d_struct*)_tmp5BA)->f1;_LL3A8: Cyc_NewControlFlow_check_fun(
_tmp5BB);goto _LL3A6;_LL3A9: if(*((int*)_tmp5BA)!= 10)goto _LL3AB;_tmp5BC=((struct
Cyc_Absyn_ExternC_d_struct*)_tmp5BA)->f1;_LL3AA: _tmp5BD=_tmp5BC;goto _LL3AC;
_LL3AB: if(*((int*)_tmp5BA)!= 9)goto _LL3AD;_tmp5BD=((struct Cyc_Absyn_Using_d_struct*)
_tmp5BA)->f2;_LL3AC: _tmp5BE=_tmp5BD;goto _LL3AE;_LL3AD: if(*((int*)_tmp5BA)!= 8)
goto _LL3AF;_tmp5BE=((struct Cyc_Absyn_Namespace_d_struct*)_tmp5BA)->f2;_LL3AE: Cyc_NewControlFlow_cf_check(
_tmp5BE);goto _LL3A6;_LL3AF: if(*((int*)_tmp5BA)!= 11)goto _LL3B1;_LL3B0: goto _LL3A6;
_LL3B1:;_LL3B2: goto _LL3A6;_LL3A6:;}}
