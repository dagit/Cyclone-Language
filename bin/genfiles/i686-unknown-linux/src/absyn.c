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
void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[
16];struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[
8];int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2);struct Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct _dynforward_ptr
line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Segment*Cyc_Position_segment_join(struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dynforward_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};
extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{int tag;};
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct
Loc_n;struct Cyc_Absyn_Rel_n_struct Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};
struct _tuple0{union Cyc_Absyn_Nmspace_union f1;struct _dynforward_ptr*f2;};struct
Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int
q_restrict;int real_const;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Eq_constr_struct{
int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_Absyn_No_constr_struct{int tag;};union Cyc_Absyn_Constraint_union{
struct Cyc_Absyn_Eq_constr_struct Eq_constr;struct Cyc_Absyn_Forward_constr_struct
Forward_constr;struct Cyc_Absyn_No_constr_struct No_constr;};struct Cyc_Absyn_Conref{
union Cyc_Absyn_Constraint_union v;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct
_dynforward_ptr*name;int identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int
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
_tuple0*name;int is_xtunion;int is_flat;};struct Cyc_Absyn_UnknownTunion_struct{int
tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int
tag;struct Cyc_Absyn_Tuniondecl**f1;};union Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct
UnknownTunion;struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{
union Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*tunion_name;struct
_tuple0*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int
tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};union Cyc_Absyn_TunionFieldInfoU_union{
struct Cyc_Absyn_UnknownTunionfield_struct UnknownTunionfield;struct Cyc_Absyn_KnownTunionfield_struct
KnownTunionfield;};struct Cyc_Absyn_TunionFieldInfo{union Cyc_Absyn_TunionFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple0*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
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
int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;
};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple0*
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
_tuple0*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;
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
struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;
void*r;struct Cyc_Position_Segment*loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple2{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple2 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dynforward_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple2 f2;struct _tuple2 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dynforward_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple2 f2;
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dynforward_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{void*
kind;void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple0*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;int is_flat;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _dynforward_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dynforward_ptr*f1;};char Cyc_Absyn_EmptyAnnot[15]="\000\000\000\000EmptyAnnot\000";
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);
struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern
struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_dynforward_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dyneither_conref;
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_force_kb(void*kb);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(void*,void*);extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;
extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;
extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_double_typ(int);extern void*Cyc_Absyn_empty_effect;
extern struct _tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;
extern void*Cyc_Absyn_exn_typ;extern struct _tuple0*Cyc_Absyn_tunion_print_arg_qvar;
extern struct _tuple0*Cyc_Absyn_tunion_scanf_arg_qvar;void*Cyc_Absyn_string_typ(
void*rgn);void*Cyc_Absyn_const_string_typ(void*rgn);void*Cyc_Absyn_file_typ();
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct
Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_star_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dynforward_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct
_dynforward_ptr*name);void*Cyc_Absyn_strctq(struct _tuple0*name);void*Cyc_Absyn_unionq_typ(
struct _tuple0*name);void*Cyc_Absyn_union_typ(struct _dynforward_ptr*name);void*
Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_bool_exp(int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dynforward_ptr f,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dynforward_ptr s,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*,struct Cyc_List_List*es,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct
Cyc_Absyn_Exp*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _dynforward_ptr*lab,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*
Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct
Cyc_List_List*fs);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct
_tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_union_decl(void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*
i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_xtunion,int is_flat,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);void*Cyc_Absyn_pointer_expand(void*,int
fresh_evar);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dynforward_ptr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dynforward_ptr*);
struct _tuple3{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple3*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*,int);struct _dynforward_ptr Cyc_Absyn_attribute2string(void*);
int Cyc_Absyn_fntype_att(void*a);struct _dynforward_ptr*Cyc_Absyn_fieldname(int);
struct _tuple4{void*f1;struct _tuple0*f2;};struct _tuple4 Cyc_Absyn_aggr_kinded_name(
union Cyc_Absyn_AggrInfoU_union);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info);int Cyc_Absyn_is_union_type(void*);int Cyc_Absyn_is_aggr_type(
void*t);void Cyc_Absyn_print_decls(struct Cyc_List_List*);extern int Cyc_Absyn_porting_c_code;
extern int Cyc_Absyn_no_regions;struct Cyc_Typerep_Int_struct{int tag;int f1;
unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;void*f2;
};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct _tuple5{unsigned int f1;
struct _dynforward_ptr f2;void*f3;};struct Cyc_Typerep_Struct_struct{int tag;struct
_dynforward_ptr*f1;unsigned int f2;struct _dynforward_ptr f3;};struct _tuple6{
unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;
struct _dynforward_ptr f2;};struct _tuple7{unsigned int f1;struct _dynforward_ptr f2;}
;struct Cyc_Typerep_TUnion_struct{int tag;struct _dynforward_ptr f1;struct
_dynforward_ptr f2;struct _dynforward_ptr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _dynforward_ptr f1;struct _dynforward_ptr f2;unsigned int f3;struct
_dynforward_ptr f4;};struct _tuple8{struct _dynforward_ptr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _dynforward_ptr f1;struct _dynforward_ptr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _dynforward_ptr*f1;int f2;struct _dynforward_ptr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _dynforward_ptr*f1;int f2;struct _dynforward_ptr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);extern void*Cyc_decls_rep;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;
int Cyc_strptrcmp(struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);typedef
struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
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
int Cyc_printf(struct _dynforward_ptr,struct _dynforward_ptr);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dynforward_ptr f1;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char
Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);
struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern
char Cyc_Dict_Absent[11];struct _tuple9{void*f1;void*f2;};struct _tuple9*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple9*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct
Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct
_RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
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
void*Cyc_Tcutil_compress(void*t);void*Cyc_Tcutil_kind_to_bound(void*k);void Cyc_Marshal_print_type(
void*rep,void*val);struct _tuple10{struct Cyc_Dict_Dict f1;int f2;};struct _tuple11{
struct _dynforward_ptr f1;int f2;};extern int Cyc_Cyclone_tovc_r;extern void*Cyc_Cyclone_c_compiler;
int Cyc_Cyclone_tovc_r=0;void*Cyc_Cyclone_c_compiler=(void*)0;static int Cyc_Absyn_strlist_cmp(
struct Cyc_List_List*ss1,struct Cyc_List_List*ss2){return((int(*)(int(*cmp)(struct
_dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_strptrcmp,ss1,ss2);}int Cyc_Absyn_varlist_cmp(struct
Cyc_List_List*vs1,struct Cyc_List_List*vs2){if((int)vs1 == (int)vs2)return 0;return
Cyc_Absyn_strlist_cmp(vs1,vs2);}struct _tuple12{union Cyc_Absyn_Nmspace_union f1;
union Cyc_Absyn_Nmspace_union f2;};int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct
_tuple0*q2){union Cyc_Absyn_Nmspace_union _tmp0=(*q1).f1;union Cyc_Absyn_Nmspace_union
_tmp1=(*q2).f1;{struct _tuple12 _tmp3=({struct _tuple12 _tmp2;_tmp2.f1=_tmp0;_tmp2.f2=
_tmp1;_tmp2;});union Cyc_Absyn_Nmspace_union _tmp4;union Cyc_Absyn_Nmspace_union
_tmp5;union Cyc_Absyn_Nmspace_union _tmp6;struct Cyc_List_List*_tmp7;union Cyc_Absyn_Nmspace_union
_tmp8;struct Cyc_List_List*_tmp9;union Cyc_Absyn_Nmspace_union _tmpA;struct Cyc_List_List*
_tmpB;union Cyc_Absyn_Nmspace_union _tmpC;struct Cyc_List_List*_tmpD;union Cyc_Absyn_Nmspace_union
_tmpE;union Cyc_Absyn_Nmspace_union _tmpF;union Cyc_Absyn_Nmspace_union _tmp10;union
Cyc_Absyn_Nmspace_union _tmp11;_LL1: _tmp4=_tmp3.f1;if(((_tmp3.f1).Loc_n).tag != 0)
goto _LL3;_tmp5=_tmp3.f2;if(((_tmp3.f2).Loc_n).tag != 0)goto _LL3;_LL2: goto _LL0;
_LL3: _tmp6=_tmp3.f1;if(((_tmp3.f1).Rel_n).tag != 1)goto _LL5;_tmp7=(_tmp6.Rel_n).f1;
_tmp8=_tmp3.f2;if(((_tmp3.f2).Rel_n).tag != 1)goto _LL5;_tmp9=(_tmp8.Rel_n).f1;
_LL4: _tmpB=_tmp7;_tmpD=_tmp9;goto _LL6;_LL5: _tmpA=_tmp3.f1;if(((_tmp3.f1).Abs_n).tag
!= 2)goto _LL7;_tmpB=(_tmpA.Abs_n).f1;_tmpC=_tmp3.f2;if(((_tmp3.f2).Abs_n).tag != 
2)goto _LL7;_tmpD=(_tmpC.Abs_n).f1;_LL6: {int i=Cyc_Absyn_strlist_cmp(_tmpB,_tmpD);
if(i != 0)return i;goto _LL0;}_LL7: _tmpE=_tmp3.f1;if(((_tmp3.f1).Loc_n).tag != 0)
goto _LL9;_LL8: return - 1;_LL9: _tmpF=_tmp3.f2;if(((_tmp3.f2).Loc_n).tag != 0)goto
_LLB;_LLA: return 1;_LLB: _tmp10=_tmp3.f1;if(((_tmp3.f1).Rel_n).tag != 1)goto _LLD;
_LLC: return - 1;_LLD: _tmp11=_tmp3.f2;if(((_tmp3.f2).Rel_n).tag != 1)goto _LL0;_LLE:
return 1;_LL0:;}return Cyc_strptrcmp((*q1).f2,(*q2).f2);}int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){int i=Cyc_strptrcmp(tv1->name,
tv2->name);if(i != 0)return i;return tv1->identity - tv2->identity;}union Cyc_Absyn_Nmspace_union
Cyc_Absyn_rel_ns_null=(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Rel_n_struct){
1,0});int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv){union Cyc_Absyn_Nmspace_union
_tmp13=(*qv).f1;struct Cyc_List_List*_tmp14;struct Cyc_List_List*_tmp15;_LL10: if((
_tmp13.Rel_n).tag != 1)goto _LL12;_tmp14=(_tmp13.Rel_n).f1;if(_tmp14 != 0)goto _LL12;
_LL11: goto _LL13;_LL12: if((_tmp13.Abs_n).tag != 2)goto _LL14;_tmp15=(_tmp13.Abs_n).f1;
if(_tmp15 != 0)goto _LL14;_LL13: goto _LL15;_LL14: if((_tmp13.Loc_n).tag != 0)goto
_LL16;_LL15: return 0;_LL16:;_LL17: return 1;_LLF:;}static int Cyc_Absyn_new_type_counter=
0;void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){return(
void*)({struct Cyc_Absyn_Evar_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16[0]=({
struct Cyc_Absyn_Evar_struct _tmp17;_tmp17.tag=0;_tmp17.f1=k;_tmp17.f2=0;_tmp17.f3=
Cyc_Absyn_new_type_counter ++;_tmp17.f4=env;_tmp17;});_tmp16;});}static struct Cyc_Core_Opt
Cyc_Absyn_mk={(void*)((void*)1)};void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
return Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Absyn_mk,tenv);}struct Cyc_Absyn_Tqual
Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){return({
struct Cyc_Absyn_Tqual _tmp18;_tmp18.print_const=x.print_const  || y.print_const;
_tmp18.q_volatile=x.q_volatile  || y.q_volatile;_tmp18.q_restrict=x.q_restrict
 || y.q_restrict;_tmp18.real_const=x.real_const  || y.real_const;_tmp18.loc=Cyc_Position_segment_join(
x.loc,y.loc);_tmp18;});}struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Tqual _tmp19;_tmp19.print_const=0;_tmp19.q_volatile=
0;_tmp19.q_restrict=0;_tmp19.real_const=0;_tmp19.loc=loc;_tmp19;});}struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Tqual
_tmp1A;_tmp1A.print_const=1;_tmp1A.q_volatile=0;_tmp1A.q_restrict=0;_tmp1A.real_const=
1;_tmp1A.loc=loc;_tmp1A;});}static struct Cyc_Absyn_Const_e_struct Cyc_Absyn_one_b_raw={
0,(union Cyc_Absyn_Cnst_union)((struct Cyc_Absyn_Int_c_struct){2,(void*)((void*)1),
1})};struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={0,(void*)((void*)& Cyc_Absyn_one_b_raw),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=&
Cyc_Absyn_exp_unsigned_one_v;static struct Cyc_Absyn_Upper_b_struct Cyc_Absyn_one_bt={
0,& Cyc_Absyn_exp_unsigned_one_v};void*Cyc_Absyn_bounds_one=(void*)& Cyc_Absyn_one_bt;
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x){return({struct Cyc_Absyn_Conref*
_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->v=(union Cyc_Absyn_Constraint_union)({
union Cyc_Absyn_Constraint_union _tmp1F;(_tmp1F.Eq_constr).tag=0;(_tmp1F.Eq_constr).f1=(
void*)x;_tmp1F;});_tmp1E;});}struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref(){
return({struct Cyc_Absyn_Conref*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20->v=(
union Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union _tmp21;(_tmp21.No_constr).tag=
2;_tmp21;});_tmp20;});}static struct Cyc_Absyn_Conref Cyc_Absyn_true_conref_v={(
union Cyc_Absyn_Constraint_union)((struct Cyc_Absyn_Eq_constr_struct){0,(void*)1})};
struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v;static
struct Cyc_Absyn_Conref Cyc_Absyn_false_conref_v={(union Cyc_Absyn_Constraint_union)((
struct Cyc_Absyn_Eq_constr_struct){0,(void*)0})};struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref=&
Cyc_Absyn_false_conref_v;static struct Cyc_Absyn_Conref Cyc_Absyn_bounds_one_conref_v={(
union Cyc_Absyn_Constraint_union)((struct Cyc_Absyn_Eq_constr_struct){0,(void*)((
void*)& Cyc_Absyn_one_bt)})};struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref=&
Cyc_Absyn_bounds_one_conref_v;static struct Cyc_Absyn_Conref Cyc_Absyn_bounds_dynforward_conref_v={(
union Cyc_Absyn_Constraint_union)((struct Cyc_Absyn_Eq_constr_struct){0,(void*)((
void*)0)})};struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dynforward_conref=& Cyc_Absyn_bounds_dynforward_conref_v;
static struct Cyc_Absyn_Conref Cyc_Absyn_bounds_dyneither_conref_v={(union Cyc_Absyn_Constraint_union)((
struct Cyc_Absyn_Eq_constr_struct){0,(void*)((void*)1)})};struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_dyneither_conref=& Cyc_Absyn_bounds_dyneither_conref_v;struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x){union Cyc_Absyn_Constraint_union
_tmp27=x->v;struct Cyc_Absyn_Conref*_tmp28;_LL19: if((_tmp27.No_constr).tag != 2)
goto _LL1B;_LL1A: goto _LL1C;_LL1B: if((_tmp27.Eq_constr).tag != 0)goto _LL1D;_LL1C:
return x;_LL1D: if((_tmp27.Forward_constr).tag != 1)goto _LL18;_tmp28=(_tmp27.Forward_constr).f1;
_LL1E: {struct Cyc_Absyn_Conref*_tmp29=Cyc_Absyn_compress_conref(_tmp28);x->v=(
union Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union _tmp2A;(_tmp2A.Forward_constr).tag=
1;(_tmp2A.Forward_constr).f1=_tmp29;_tmp2A;});return _tmp29;}_LL18:;}void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x){union Cyc_Absyn_Constraint_union _tmp2B=(Cyc_Absyn_compress_conref(
x))->v;void*_tmp2C;_LL20: if((_tmp2B.Eq_constr).tag != 0)goto _LL22;_tmp2C=(_tmp2B.Eq_constr).f1;
_LL21: return _tmp2C;_LL22:;_LL23:({void*_tmp2D=0;Cyc_Tcutil_impos(({const char*
_tmp2E="conref_val";_tag_dynforward(_tmp2E,sizeof(char),_get_zero_arr_size_char(
_tmp2E,11));}),_tag_dynforward(_tmp2D,sizeof(void*),0));});_LL1F:;}void*Cyc_Absyn_conref_def(
void*y,struct Cyc_Absyn_Conref*x){union Cyc_Absyn_Constraint_union _tmp2F=(Cyc_Absyn_compress_conref(
x))->v;void*_tmp30;_LL25: if((_tmp2F.Eq_constr).tag != 0)goto _LL27;_tmp30=(_tmp2F.Eq_constr).f1;
_LL26: return _tmp30;_LL27:;_LL28: return y;_LL24:;}void*Cyc_Absyn_compress_kb(void*
k){void*_tmp31=k;struct Cyc_Core_Opt*_tmp32;struct Cyc_Core_Opt*_tmp33;struct Cyc_Core_Opt*
_tmp34;struct Cyc_Core_Opt _tmp35;void*_tmp36;void**_tmp37;struct Cyc_Core_Opt*
_tmp38;struct Cyc_Core_Opt _tmp39;void*_tmp3A;void**_tmp3B;_LL2A: if(*((int*)_tmp31)
!= 0)goto _LL2C;_LL2B: goto _LL2D;_LL2C: if(*((int*)_tmp31)!= 1)goto _LL2E;_tmp32=((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp31)->f1;if(_tmp32 != 0)goto _LL2E;_LL2D:
goto _LL2F;_LL2E: if(*((int*)_tmp31)!= 2)goto _LL30;_tmp33=((struct Cyc_Absyn_Less_kb_struct*)
_tmp31)->f1;if(_tmp33 != 0)goto _LL30;_LL2F: return k;_LL30: if(*((int*)_tmp31)!= 1)
goto _LL32;_tmp34=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp31)->f1;if(_tmp34 == 0)
goto _LL32;_tmp35=*_tmp34;_tmp36=(void*)_tmp35.v;_tmp37=(void**)&(*((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp31)->f1).v;_LL31: _tmp3B=_tmp37;goto _LL33;_LL32: if(*((int*)_tmp31)!= 2)goto
_LL29;_tmp38=((struct Cyc_Absyn_Less_kb_struct*)_tmp31)->f1;if(_tmp38 == 0)goto
_LL29;_tmp39=*_tmp38;_tmp3A=(void*)_tmp39.v;_tmp3B=(void**)&(*((struct Cyc_Absyn_Less_kb_struct*)
_tmp31)->f1).v;_LL33:*_tmp3B=Cyc_Absyn_compress_kb(*_tmp3B);return*_tmp3B;_LL29:;}
void*Cyc_Absyn_force_kb(void*kb){void*_tmp3C=Cyc_Absyn_compress_kb(kb);void*
_tmp3D;struct Cyc_Core_Opt*_tmp3E;struct Cyc_Core_Opt**_tmp3F;struct Cyc_Core_Opt*
_tmp40;struct Cyc_Core_Opt**_tmp41;void*_tmp42;_LL35: if(*((int*)_tmp3C)!= 0)goto
_LL37;_tmp3D=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp3C)->f1;_LL36: return
_tmp3D;_LL37: if(*((int*)_tmp3C)!= 1)goto _LL39;_tmp3E=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp3C)->f1;_tmp3F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp3C)->f1;_LL38: _tmp41=_tmp3F;_tmp42=(void*)2;goto _LL3A;_LL39: if(*((int*)
_tmp3C)!= 2)goto _LL34;_tmp40=((struct Cyc_Absyn_Less_kb_struct*)_tmp3C)->f1;
_tmp41=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp3C)->f1;
_tmp42=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp3C)->f2;_LL3A:*_tmp41=({
struct Cyc_Core_Opt*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->v=(void*)Cyc_Tcutil_kind_to_bound(
_tmp42);_tmp43;});return _tmp42;_LL34:;}static struct Cyc_Absyn_IntType_struct Cyc_Absyn_char_tt={
5,(void*)((void*)2),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_uchar_tt={
5,(void*)((void*)1),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_ushort_tt={
5,(void*)((void*)1),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_uint_tt={
5,(void*)((void*)1),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_ulong_tt={
5,(void*)((void*)1),(void*)((void*)3)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_ulonglong_tt={
5,(void*)((void*)1),(void*)((void*)4)};void*Cyc_Absyn_char_typ=(void*)& Cyc_Absyn_char_tt;
void*Cyc_Absyn_uchar_typ=(void*)& Cyc_Absyn_uchar_tt;void*Cyc_Absyn_ushort_typ=(
void*)& Cyc_Absyn_ushort_tt;void*Cyc_Absyn_uint_typ=(void*)& Cyc_Absyn_uint_tt;
void*Cyc_Absyn_ulong_typ=(void*)& Cyc_Absyn_ulong_tt;void*Cyc_Absyn_ulonglong_typ=(
void*)& Cyc_Absyn_ulonglong_tt;static struct Cyc_Absyn_IntType_struct Cyc_Absyn_schar_tt={
5,(void*)((void*)0),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_sshort_tt={
5,(void*)((void*)0),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_sint_tt={
5,(void*)((void*)0),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_slong_tt={
5,(void*)((void*)0),(void*)((void*)3)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_slonglong_tt={
5,(void*)((void*)0),(void*)((void*)4)};void*Cyc_Absyn_schar_typ=(void*)& Cyc_Absyn_schar_tt;
void*Cyc_Absyn_sshort_typ=(void*)& Cyc_Absyn_sshort_tt;void*Cyc_Absyn_sint_typ=(
void*)& Cyc_Absyn_sint_tt;void*Cyc_Absyn_slong_typ=(void*)& Cyc_Absyn_slong_tt;
void*Cyc_Absyn_slonglong_typ=(void*)& Cyc_Absyn_slonglong_tt;static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_nshort_tt={5,(void*)((void*)2),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_nint_tt={5,(void*)((void*)2),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_nlong_tt={5,(void*)((void*)2),(void*)((void*)3)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_nlonglong_tt={5,(void*)((void*)2),(void*)((void*)4)};void*Cyc_Absyn_nshort_typ=(
void*)& Cyc_Absyn_nshort_tt;void*Cyc_Absyn_nint_typ=(void*)& Cyc_Absyn_nint_tt;
void*Cyc_Absyn_nlong_typ=(void*)& Cyc_Absyn_nlong_tt;void*Cyc_Absyn_nlonglong_typ=(
void*)& Cyc_Absyn_nlonglong_tt;void*Cyc_Absyn_int_typ(void*sn,void*sz){void*
_tmp53=sn;_LL3C: if((int)_tmp53 != 0)goto _LL3E;_LL3D: {void*_tmp54=sz;_LL43: if((
int)_tmp54 != 0)goto _LL45;_LL44: return Cyc_Absyn_schar_typ;_LL45: if((int)_tmp54 != 
1)goto _LL47;_LL46: return Cyc_Absyn_sshort_typ;_LL47: if((int)_tmp54 != 2)goto _LL49;
_LL48: return Cyc_Absyn_sint_typ;_LL49: if((int)_tmp54 != 3)goto _LL4B;_LL4A: return
Cyc_Absyn_slong_typ;_LL4B: if((int)_tmp54 != 4)goto _LL42;_LL4C: return Cyc_Absyn_slonglong_typ;
_LL42:;}_LL3E: if((int)_tmp53 != 1)goto _LL40;_LL3F: {void*_tmp55=sz;_LL4E: if((int)
_tmp55 != 0)goto _LL50;_LL4F: return Cyc_Absyn_uchar_typ;_LL50: if((int)_tmp55 != 1)
goto _LL52;_LL51: return Cyc_Absyn_ushort_typ;_LL52: if((int)_tmp55 != 2)goto _LL54;
_LL53: return Cyc_Absyn_uint_typ;_LL54: if((int)_tmp55 != 3)goto _LL56;_LL55: return
Cyc_Absyn_ulong_typ;_LL56: if((int)_tmp55 != 4)goto _LL4D;_LL57: return Cyc_Absyn_ulonglong_typ;
_LL4D:;}_LL40: if((int)_tmp53 != 2)goto _LL3B;_LL41: {void*_tmp56=sz;_LL59: if((int)
_tmp56 != 0)goto _LL5B;_LL5A: return Cyc_Absyn_char_typ;_LL5B: if((int)_tmp56 != 1)
goto _LL5D;_LL5C: return Cyc_Absyn_nshort_typ;_LL5D: if((int)_tmp56 != 2)goto _LL5F;
_LL5E: return Cyc_Absyn_nint_typ;_LL5F: if((int)_tmp56 != 3)goto _LL61;_LL60: return
Cyc_Absyn_nlong_typ;_LL61: if((int)_tmp56 != 4)goto _LL58;_LL62: return Cyc_Absyn_nlonglong_typ;
_LL58:;}_LL3B:;}void*Cyc_Absyn_float_typ=(void*)1;void*Cyc_Absyn_double_typ(int b){
static struct Cyc_Absyn_DoubleType_struct dt={6,1};static struct Cyc_Absyn_DoubleType_struct
df={6,0};return(void*)(b?& dt:& df);}static char _tmp59[4]="exn";static struct
_dynforward_ptr Cyc_Absyn_exn_str={_tmp59,_tmp59 + 4};static struct _tuple0 Cyc_Absyn_exn_name_v={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp5B[15]="Null_Exception";
static struct _dynforward_ptr Cyc_Absyn_Null_Exception_str={_tmp5B,_tmp5B + 15};
static struct _tuple0 Cyc_Absyn_Null_Exception_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_Null_Exception_str};struct _tuple0*
Cyc_Absyn_Null_Exception_name=& Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Tunionfield
Cyc_Absyn_Null_Exception_tuf_v={& Cyc_Absyn_Null_Exception_pr,0,0,(void*)((void*)
3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v;
static char _tmp5D[13]="Array_bounds";static struct _dynforward_ptr Cyc_Absyn_Array_bounds_str={
_tmp5D,_tmp5D + 13};static struct _tuple0 Cyc_Absyn_Array_bounds_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_Array_bounds_str};struct _tuple0*
Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static struct Cyc_Absyn_Tunionfield
Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,0,0,(void*)((void*)3)};
struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Array_bounds_tuf=& Cyc_Absyn_Array_bounds_tuf_v;
static char _tmp5F[16]="Match_Exception";static struct _dynforward_ptr Cyc_Absyn_Match_Exception_str={
_tmp5F,_tmp5F + 16};static struct _tuple0 Cyc_Absyn_Match_Exception_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_Match_Exception_str};struct
_tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;static
struct Cyc_Absyn_Tunionfield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Match_Exception_tuf=&
Cyc_Absyn_Match_Exception_tuf_v;static char _tmp61[10]="Bad_alloc";static struct
_dynforward_ptr Cyc_Absyn_Bad_alloc_str={_tmp61,_tmp61 + 10};static struct _tuple0
Cyc_Absyn_Bad_alloc_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Abs_n_struct){
2,0}),& Cyc_Absyn_Bad_alloc_str};struct _tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;
static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;
static struct Cyc_List_List Cyc_Absyn_exn_l0={(void*)& Cyc_Absyn_Null_Exception_tuf_v,
0};static struct Cyc_List_List Cyc_Absyn_exn_l1={(void*)& Cyc_Absyn_Array_bounds_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l0};static struct Cyc_List_List Cyc_Absyn_exn_l2={(
void*)& Cyc_Absyn_Match_Exception_tuf_v,(struct Cyc_List_List*)& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={(void*)& Cyc_Absyn_Bad_alloc_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l2};static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(
void*)((struct Cyc_List_List*)& Cyc_Absyn_exn_l3)};static struct Cyc_Absyn_Tuniondecl
Cyc_Absyn_exn_tud_v={(void*)((void*)3),& Cyc_Absyn_exn_name_v,0,(struct Cyc_Core_Opt*)&
Cyc_Absyn_exn_ol,1,0};struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud=& Cyc_Absyn_exn_tud_v;
static struct Cyc_Core_Opt Cyc_Absyn_heap_opt={(void*)((void*)2)};static struct Cyc_Absyn_TunionType_struct
Cyc_Absyn_exn_typ_tt={2,{(union Cyc_Absyn_TunionInfoU_union)((struct Cyc_Absyn_KnownTunion_struct){
1,& Cyc_Absyn_exn_tud}),0,(struct Cyc_Core_Opt*)& Cyc_Absyn_heap_opt}};void*Cyc_Absyn_exn_typ=(
void*)& Cyc_Absyn_exn_typ_tt;static char _tmp65[9]="PrintArg";static struct
_dynforward_ptr Cyc_Absyn_printarg_str={_tmp65,_tmp65 + 9};static char _tmp66[9]="ScanfArg";
static struct _dynforward_ptr Cyc_Absyn_scanfarg_str={_tmp66,_tmp66 + 9};static
struct _tuple0 Cyc_Absyn_tunion_print_arg_qvar_p={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_printarg_str};static struct _tuple0
Cyc_Absyn_tunion_scanf_arg_qvar_p={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Abs_n_struct){
2,0}),& Cyc_Absyn_scanfarg_str};struct _tuple0*Cyc_Absyn_tunion_print_arg_qvar=&
Cyc_Absyn_tunion_print_arg_qvar_p;struct _tuple0*Cyc_Absyn_tunion_scanf_arg_qvar=&
Cyc_Absyn_tunion_scanf_arg_qvar_p;static void**Cyc_Absyn_string_t_opt=0;void*Cyc_Absyn_string_typ(
void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_empty_tqual(
0),(void*)0,Cyc_Absyn_true_conref);if(Cyc_Absyn_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_empty_tqual(0),(void*)0,Cyc_Absyn_true_conref);
Cyc_Absyn_string_t_opt=({void**_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69[0]=t;
_tmp69;});}return*((void**)_check_null(Cyc_Absyn_string_t_opt));}static void**Cyc_Absyn_const_string_t_opt=
0;void*Cyc_Absyn_const_string_typ(void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);
if(Cyc_Absyn_const_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);Cyc_Absyn_const_string_t_opt=({
void**_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A[0]=t;_tmp6A;});}return*((void**)
_check_null(Cyc_Absyn_const_string_t_opt));}void*Cyc_Absyn_starb_typ(void*t,void*
r,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zeroterm){return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B[0]=({
struct Cyc_Absyn_PointerType_struct _tmp6C;_tmp6C.tag=4;_tmp6C.f1=({struct Cyc_Absyn_PtrInfo
_tmp6D;_tmp6D.elt_typ=(void*)t;_tmp6D.elt_tq=tq;_tmp6D.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp6E;_tmp6E.rgn=(void*)r;_tmp6E.nullable=Cyc_Absyn_true_conref;_tmp6E.bounds=
Cyc_Absyn_new_conref(b);_tmp6E.zero_term=zeroterm;_tmp6E.ptrloc=0;_tmp6E;});
_tmp6D;});_tmp6C;});_tmp6B;});}void*Cyc_Absyn_atb_typ(void*t,void*r,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F[0]=({struct Cyc_Absyn_PointerType_struct
_tmp70;_tmp70.tag=4;_tmp70.f1=({struct Cyc_Absyn_PtrInfo _tmp71;_tmp71.elt_typ=(
void*)t;_tmp71.elt_tq=tq;_tmp71.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp72;_tmp72.rgn=(
void*)r;_tmp72.nullable=Cyc_Absyn_false_conref;_tmp72.bounds=Cyc_Absyn_new_conref(
b);_tmp72.zero_term=zeroterm;_tmp72.ptrloc=0;_tmp72;});_tmp71;});_tmp70;});
_tmp6F;});}void*Cyc_Absyn_star_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Conref*zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73[0]=({struct Cyc_Absyn_PointerType_struct
_tmp74;_tmp74.tag=4;_tmp74.f1=({struct Cyc_Absyn_PtrInfo _tmp75;_tmp75.elt_typ=(
void*)t;_tmp75.elt_tq=tq;_tmp75.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp76;_tmp76.rgn=(
void*)r;_tmp76.nullable=Cyc_Absyn_true_conref;_tmp76.bounds=Cyc_Absyn_bounds_one_conref;
_tmp76.zero_term=zeroterm;_tmp76.ptrloc=0;_tmp76;});_tmp75;});_tmp74;});_tmp73;});}
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq){return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77[0]=({
struct Cyc_Absyn_PointerType_struct _tmp78;_tmp78.tag=4;_tmp78.f1=({struct Cyc_Absyn_PtrInfo
_tmp79;_tmp79.elt_typ=(void*)t;_tmp79.elt_tq=tq;_tmp79.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp7A;_tmp7A.rgn=(void*)((void*)2);_tmp7A.nullable=Cyc_Absyn_true_conref;_tmp7A.bounds=
Cyc_Absyn_bounds_one_conref;_tmp7A.zero_term=Cyc_Absyn_false_conref;_tmp7A.ptrloc=
0;_tmp7A;});_tmp79;});_tmp78;});_tmp77;});}void*Cyc_Absyn_at_typ(void*t,void*r,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zeroterm){return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B[0]=({
struct Cyc_Absyn_PointerType_struct _tmp7C;_tmp7C.tag=4;_tmp7C.f1=({struct Cyc_Absyn_PtrInfo
_tmp7D;_tmp7D.elt_typ=(void*)t;_tmp7D.elt_tq=tq;_tmp7D.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp7E;_tmp7E.rgn=(void*)r;_tmp7E.nullable=Cyc_Absyn_false_conref;_tmp7E.bounds=
Cyc_Absyn_bounds_one_conref;_tmp7E.zero_term=zeroterm;_tmp7E.ptrloc=0;_tmp7E;});
_tmp7D;});_tmp7C;});_tmp7B;});}void*Cyc_Absyn_dynforward_typ(void*t,void*r,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zeroterm){return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F[0]=({
struct Cyc_Absyn_PointerType_struct _tmp80;_tmp80.tag=4;_tmp80.f1=({struct Cyc_Absyn_PtrInfo
_tmp81;_tmp81.elt_typ=(void*)t;_tmp81.elt_tq=tq;_tmp81.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp82;_tmp82.rgn=(void*)r;_tmp82.nullable=Cyc_Absyn_true_conref;_tmp82.bounds=
Cyc_Absyn_bounds_dynforward_conref;_tmp82.zero_term=zeroterm;_tmp82.ptrloc=0;
_tmp82;});_tmp81;});_tmp80;});_tmp7F;});}void*Cyc_Absyn_dyneither_typ(void*t,
void*r,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zeroterm){return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83[0]=({
struct Cyc_Absyn_PointerType_struct _tmp84;_tmp84.tag=4;_tmp84.f1=({struct Cyc_Absyn_PtrInfo
_tmp85;_tmp85.elt_typ=(void*)t;_tmp85.elt_tq=tq;_tmp85.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp86;_tmp86.rgn=(void*)r;_tmp86.nullable=Cyc_Absyn_true_conref;_tmp86.bounds=
Cyc_Absyn_bounds_dyneither_conref;_tmp86.zero_term=zeroterm;_tmp86.ptrloc=0;
_tmp86;});_tmp85;});_tmp84;});_tmp83;});}void*Cyc_Absyn_array_typ(void*elt_type,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*
zero_term,struct Cyc_Position_Segment*ztloc){return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp88;_tmp88.tag=7;_tmp88.f1=({struct Cyc_Absyn_ArrayInfo _tmp89;_tmp89.elt_type=(
void*)elt_type;_tmp89.tq=tq;_tmp89.num_elts=num_elts;_tmp89.zero_term=zero_term;
_tmp89.zt_loc=ztloc;_tmp89;});_tmp88;});_tmp87;});}static char _tmp93[8]="__sFILE";
void*Cyc_Absyn_file_typ(){static void**file_t_opt=0;static struct _dynforward_ptr
sf_str={_tmp93,_tmp93 + 8};static struct _dynforward_ptr*sf=& sf_str;if(file_t_opt == 
0){struct _tuple0*file_t_name=({struct _tuple0*_tmp91=_cycalloc(sizeof(*_tmp91));
_tmp91->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp92;(
_tmp92.Abs_n).tag=2;(_tmp92.Abs_n).f1=0;_tmp92;});_tmp91->f2=sf;_tmp91;});struct
Cyc_Absyn_Aggrdecl*sd=({struct Cyc_Absyn_Aggrdecl*_tmp90=_cycalloc(sizeof(*_tmp90));
_tmp90->kind=(void*)((void*)0);_tmp90->sc=(void*)((void*)1);_tmp90->name=
file_t_name;_tmp90->tvs=0;_tmp90->impl=0;_tmp90->attributes=0;_tmp90;});void*
file_struct_typ=(void*)({struct Cyc_Absyn_AggrType_struct*_tmp8B=_cycalloc(
sizeof(*_tmp8B));_tmp8B[0]=({struct Cyc_Absyn_AggrType_struct _tmp8C;_tmp8C.tag=10;
_tmp8C.f1=({struct Cyc_Absyn_AggrInfo _tmp8D;_tmp8D.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp8E;(_tmp8E.KnownAggr).tag=1;(_tmp8E.KnownAggr).f1=({
struct Cyc_Absyn_Aggrdecl**_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=sd;_tmp8F;});
_tmp8E;});_tmp8D.targs=0;_tmp8D;});_tmp8C;});_tmp8B;});file_t_opt=({void**_tmp8A=
_cycalloc(sizeof(*_tmp8A));_tmp8A[0]=Cyc_Absyn_at_typ(file_struct_typ,(void*)2,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);_tmp8A;});}return*file_t_opt;}
void*Cyc_Absyn_void_star_typ(){static void**void_star_t_opt=0;if(void_star_t_opt
== 0)void_star_t_opt=({void**_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94[0]=Cyc_Absyn_star_typ((
void*)0,(void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);_tmp94;});
return*void_star_t_opt;}static struct Cyc_Absyn_JoinEff_struct Cyc_Absyn_empty_eff={
21,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;void*Cyc_Absyn_aggr_typ(
void*k,struct _dynforward_ptr*name){return(void*)({struct Cyc_Absyn_AggrType_struct*
_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96[0]=({struct Cyc_Absyn_AggrType_struct
_tmp97;_tmp97.tag=10;_tmp97.f1=({struct Cyc_Absyn_AggrInfo _tmp98;_tmp98.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmp99;(_tmp99.UnknownAggr).tag=
0;(_tmp99.UnknownAggr).f1=(void*)k;(_tmp99.UnknownAggr).f2=({struct _tuple0*
_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->f1=Cyc_Absyn_rel_ns_null;_tmp9A->f2=
name;_tmp9A;});_tmp99;});_tmp98.targs=0;_tmp98;});_tmp97;});_tmp96;});}void*Cyc_Absyn_strct(
struct _dynforward_ptr*name){return Cyc_Absyn_aggr_typ((void*)0,name);}void*Cyc_Absyn_union_typ(
struct _dynforward_ptr*name){return Cyc_Absyn_aggr_typ((void*)1,name);}void*Cyc_Absyn_strctq(
struct _tuple0*name){return(void*)({struct Cyc_Absyn_AggrType_struct*_tmp9B=
_cycalloc(sizeof(*_tmp9B));_tmp9B[0]=({struct Cyc_Absyn_AggrType_struct _tmp9C;
_tmp9C.tag=10;_tmp9C.f1=({struct Cyc_Absyn_AggrInfo _tmp9D;_tmp9D.aggr_info=(union
Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmp9E;(_tmp9E.UnknownAggr).tag=
0;(_tmp9E.UnknownAggr).f1=(void*)((void*)0);(_tmp9E.UnknownAggr).f2=name;_tmp9E;});
_tmp9D.targs=0;_tmp9D;});_tmp9C;});_tmp9B;});}void*Cyc_Absyn_unionq_typ(struct
_tuple0*name){return(void*)({struct Cyc_Absyn_AggrType_struct*_tmp9F=_cycalloc(
sizeof(*_tmp9F));_tmp9F[0]=({struct Cyc_Absyn_AggrType_struct _tmpA0;_tmpA0.tag=10;
_tmpA0.f1=({struct Cyc_Absyn_AggrInfo _tmpA1;_tmpA1.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmpA2;(_tmpA2.UnknownAggr).tag=0;(_tmpA2.UnknownAggr).f1=(
void*)((void*)1);(_tmpA2.UnknownAggr).f2=name;_tmpA2;});_tmpA1.targs=0;_tmpA1;});
_tmpA0;});_tmp9F;});}struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Exp*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->topt=
0;_tmpA3->r=(void*)r;_tmpA3->loc=loc;_tmpA3->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot);
_tmpA3;});}struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_New_e_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=({
struct Cyc_Absyn_New_e_struct _tmpA5;_tmpA5.tag=17;_tmpA5.f1=rgn_handle;_tmpA5.f2=
e;_tmpA5;});_tmpA4;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*
e){return({struct Cyc_Absyn_Exp*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=*e;
_tmpA6;});}struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst_union c,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Const_e_struct*
_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7[0]=({struct Cyc_Absyn_Const_e_struct
_tmpA8;_tmpA8.tag=0;_tmpA8.f1=c;_tmpA8;});_tmpA7;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Const_e_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({
struct Cyc_Absyn_Const_e_struct _tmpAA;_tmpAA.tag=0;_tmpAA.f1=(union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmpAB;(_tmpAB.Null_c).tag=6;_tmpAB;});_tmpAA;});_tmpA9;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*s,int i,struct Cyc_Position_Segment*
seg){return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmpAC;(_tmpAC.Int_c).tag=2;(_tmpAC.Int_c).f1=(void*)s;(_tmpAC.Int_c).f2=i;
_tmpAC;}),seg);}struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_int_exp((void*)0,i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int i,struct Cyc_Position_Segment*loc){return Cyc_Absyn_int_exp((void*)1,(
int)i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(1,loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(0,
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmpAD;(_tmpAD.Char_c).tag=0;(_tmpAD.Char_c).f1=(void*)((void*)2);(_tmpAD.Char_c).f2=
c;_tmpAD;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dynforward_ptr f,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmpAE;(_tmpAE.Float_c).tag=4;(_tmpAE.Float_c).f1=f;
_tmpAE;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dynforward_ptr s,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmpAF;(_tmpAF.String_c).tag=5;(_tmpAF.String_c).f1=s;
_tmpAF;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_struct*_tmpB0=
_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=({struct Cyc_Absyn_Var_e_struct _tmpB1;_tmpB1.tag=
1;_tmpB1.f1=q;_tmpB1.f2=(void*)((void*)0);_tmpB1;});_tmpB0;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_varb_exp(struct _tuple0*q,void*b,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_struct*_tmpB2=_cycalloc(sizeof(*
_tmpB2));_tmpB2[0]=({struct Cyc_Absyn_Var_e_struct _tmpB3;_tmpB3.tag=1;_tmpB3.f1=q;
_tmpB3.f2=(void*)b;_tmpB3;});_tmpB2;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(
struct _tuple0*q,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnknownId_e_struct*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmpB5;_tmpB5.tag=2;_tmpB5.f1=q;_tmpB5;});
_tmpB4;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*p,struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_Primop_e_struct
_tmpB7;_tmpB7.tag=3;_tmpB7.f1=(void*)p;_tmpB7.f2=es;_tmpB7;});_tmpB6;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmpB8=_cycalloc(sizeof(*
_tmpB8));_tmpB8->hd=e;_tmpB8->tl=0;_tmpB8;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmpB9=_cycalloc(sizeof(*
_tmpB9));_tmpB9->hd=e1;_tmpB9->tl=({struct Cyc_List_List*_tmpBA=_cycalloc(sizeof(*
_tmpBA));_tmpBA->hd=e2;_tmpBA->tl=0;_tmpBA;});_tmpB9;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_prim2_exp((void*)0,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)1,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)3,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)5,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)6,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)7,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)8,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)9,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)10,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AssignOp_e_struct*_tmpBB=
_cycalloc(sizeof(*_tmpBB));_tmpBB[0]=({struct Cyc_Absyn_AssignOp_e_struct _tmpBC;
_tmpBC.tag=4;_tmpBC.f1=e1;_tmpBC.f2=popt;_tmpBC.f3=e2;_tmpBC;});_tmpBB;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,void*i,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Absyn_Increment_e_struct
_tmpBE;_tmpBE.tag=5;_tmpBE.f1=e;_tmpBE.f2=(void*)i;_tmpBE;});_tmpBD;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(
e,(void*)0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(e,(void*)2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)3,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Conditional_e_struct*
_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({struct Cyc_Absyn_Conditional_e_struct
_tmpC0;_tmpC0.tag=6;_tmpC0.f1=e1;_tmpC0.f2=e2;_tmpC0.f3=e3;_tmpC0;});_tmpBF;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_And_e_struct*
_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1[0]=({struct Cyc_Absyn_And_e_struct _tmpC2;
_tmpC2.tag=7;_tmpC2.f1=e1;_tmpC2.f2=e2;_tmpC2;});_tmpC1;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Or_e_struct*_tmpC3=
_cycalloc(sizeof(*_tmpC3));_tmpC3[0]=({struct Cyc_Absyn_Or_e_struct _tmpC4;_tmpC4.tag=
8;_tmpC4.f1=e1;_tmpC4.f2=e2;_tmpC4;});_tmpC3;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_SeqExp_e_struct*_tmpC5=
_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_Absyn_SeqExp_e_struct _tmpC6;
_tmpC6.tag=9;_tmpC6.f1=e1;_tmpC6.f2=e2;_tmpC6;});_tmpC5;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownCall_e_struct*
_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7[0]=({struct Cyc_Absyn_UnknownCall_e_struct
_tmpC8;_tmpC8.tag=10;_tmpC8.f1=e;_tmpC8.f2=es;_tmpC8;});_tmpC7;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmpCA;_tmpCA.tag=11;_tmpCA.f1=e;_tmpCA.f2=es;_tmpCA.f3=0;_tmpCA;});_tmpC9;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({struct Cyc_Absyn_NoInstantiate_e_struct
_tmpCC;_tmpCC.tag=13;_tmpCC.f1=e;_tmpCC;});_tmpCB;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmpCE;_tmpCE.tag=14;_tmpCE.f1=e;_tmpCE.f2=ts;_tmpCE;});_tmpCD;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,void*
c,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Cast_e_struct*
_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=({struct Cyc_Absyn_Cast_e_struct
_tmpD0;_tmpD0.tag=15;_tmpD0.f1=(void*)t;_tmpD0.f2=e;_tmpD0.f3=user_cast;_tmpD0.f4=(
void*)c;_tmpD0;});_tmpCF;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Throw_e_struct*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1[0]=({
struct Cyc_Absyn_Throw_e_struct _tmpD2;_tmpD2.tag=12;_tmpD2.f1=e;_tmpD2;});_tmpD1;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Address_e_struct*_tmpD3=
_cycalloc(sizeof(*_tmpD3));_tmpD3[0]=({struct Cyc_Absyn_Address_e_struct _tmpD4;
_tmpD4.tag=16;_tmpD4.f1=e;_tmpD4;});_tmpD3;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Sizeoftyp_e_struct*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpD6;_tmpD6.tag=18;_tmpD6.f1=(void*)t;_tmpD6;});
_tmpD5;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7[0]=({struct Cyc_Absyn_Sizeofexp_e_struct
_tmpD8;_tmpD8.tag=19;_tmpD8.f1=e;_tmpD8;});_tmpD7;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_offsetof_exp(void*t,void*of,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmpD9=_cycalloc(sizeof(*_tmpD9));
_tmpD9[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmpDA;_tmpDA.tag=20;_tmpDA.f1=(
void*)t;_tmpDA.f2=(void*)of;_tmpDA;});_tmpD9;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(
struct Cyc_List_List*tvs,void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Gentyp_e_struct*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB[
0]=({struct Cyc_Absyn_Gentyp_e_struct _tmpDC;_tmpDC.tag=21;_tmpDC.f1=tvs;_tmpDC.f2=(
void*)t;_tmpDC;});_tmpDB;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Deref_e_struct*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD[0]=({
struct Cyc_Absyn_Deref_e_struct _tmpDE;_tmpDE.tag=22;_tmpDE.f1=e;_tmpDE;});_tmpDD;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct
_dynforward_ptr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_AggrMember_e_struct*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF[0]=({
struct Cyc_Absyn_AggrMember_e_struct _tmpE0;_tmpE0.tag=23;_tmpE0.f1=e;_tmpE0.f2=n;
_tmpE0;});_tmpDF;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*
e,struct _dynforward_ptr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_AggrArrow_e_struct*_tmpE1=_cycalloc(sizeof(*_tmpE1));
_tmpE1[0]=({struct Cyc_Absyn_AggrArrow_e_struct _tmpE2;_tmpE2.tag=24;_tmpE2.f1=e;
_tmpE2.f2=n;_tmpE2;});_tmpE1;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Subscript_e_struct*_tmpE3=
_cycalloc(sizeof(*_tmpE3));_tmpE3[0]=({struct Cyc_Absyn_Subscript_e_struct _tmpE4;
_tmpE4.tag=25;_tmpE4.f1=e1;_tmpE4.f2=e2;_tmpE4;});_tmpE3;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tuple_e_struct*_tmpE5=_cycalloc(
sizeof(*_tmpE5));_tmpE5[0]=({struct Cyc_Absyn_Tuple_e_struct _tmpE6;_tmpE6.tag=26;
_tmpE6.f1=es;_tmpE6;});_tmpE5;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_StmtExp_e_struct*_tmpE7=_cycalloc(sizeof(*_tmpE7));
_tmpE7[0]=({struct Cyc_Absyn_StmtExp_e_struct _tmpE8;_tmpE8.tag=38;_tmpE8.f1=s;
_tmpE8;});_tmpE7;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*
loc){return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}struct _tuple13{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_List_List*dles=
0;for(0;es != 0;es=es->tl){dles=({struct Cyc_List_List*_tmpE9=_cycalloc(sizeof(*
_tmpE9));_tmpE9->hd=({struct _tuple13*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->f1=
0;_tmpEA->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmpEA;});_tmpE9->tl=dles;_tmpE9;});}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Array_e_struct*_tmpEB=_cycalloc(
sizeof(*_tmpEB));_tmpEB[0]=({struct Cyc_Absyn_Array_e_struct _tmpEC;_tmpEC.tag=28;
_tmpEC.f1=dles;_tmpEC;});_tmpEB;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpED=
_cycalloc(sizeof(*_tmpED));_tmpED[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpEE;_tmpEE.tag=37;_tmpEE.f1=n;_tmpEE.f2=dles;_tmpEE;});_tmpED;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc){return({
struct Cyc_Absyn_Stmt*_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF->r=(void*)s;_tmpEF->loc=
loc;_tmpEF->non_local_preds=0;_tmpEF->try_depth=0;_tmpEF->annot=(void*)((void*)
Cyc_Absyn_EmptyAnnot);_tmpEF;});}struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)0,loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Exp_s_struct*_tmpF0=_cycalloc(
sizeof(*_tmpF0));_tmpF0[0]=({struct Cyc_Absyn_Exp_s_struct _tmpF1;_tmpF1.tag=0;
_tmpF1.f1=e;_tmpF1;});_tmpF0;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(
struct Cyc_List_List*ss,struct Cyc_Position_Segment*loc){if(ss == 0)return Cyc_Absyn_skip_stmt(
loc);else{if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{return Cyc_Absyn_seq_stmt((
struct Cyc_Absyn_Stmt*)ss->hd,Cyc_Absyn_seq_stmts(ss->tl,loc),loc);}}}struct Cyc_Absyn_Stmt*
Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Return_s_struct*_tmpF2=
_cycalloc(sizeof(*_tmpF2));_tmpF2[0]=({struct Cyc_Absyn_Return_s_struct _tmpF3;
_tmpF3.tag=2;_tmpF3.f1=e;_tmpF3;});_tmpF2;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_IfThenElse_s_struct*
_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4[0]=({struct Cyc_Absyn_IfThenElse_s_struct
_tmpF5;_tmpF5.tag=3;_tmpF5.f1=e;_tmpF5.f2=s1;_tmpF5.f3=s2;_tmpF5;});_tmpF4;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_While_s_struct*
_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6[0]=({struct Cyc_Absyn_While_s_struct
_tmpF7;_tmpF7.tag=4;_tmpF7.f1=({struct _tuple2 _tmpF8;_tmpF8.f1=e;_tmpF8.f2=Cyc_Absyn_skip_stmt(
e->loc);_tmpF8;});_tmpF7.f2=s;_tmpF7;});_tmpF6;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Break_s_struct*
_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9[0]=({struct Cyc_Absyn_Break_s_struct
_tmpFA;_tmpFA.tag=5;_tmpFA.f1=0;_tmpFA;});_tmpF9;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Continue_s_struct*_tmpFB=_cycalloc(sizeof(*_tmpFB));
_tmpFB[0]=({struct Cyc_Absyn_Continue_s_struct _tmpFC;_tmpFC.tag=6;_tmpFC.f1=0;
_tmpFC;});_tmpFB;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_For_s_struct*
_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD[0]=({struct Cyc_Absyn_For_s_struct _tmpFE;
_tmpFE.tag=8;_tmpFE.f1=e1;_tmpFE.f2=({struct _tuple2 _tmpFF;_tmpFF.f1=e2;_tmpFF.f2=
Cyc_Absyn_skip_stmt(e3->loc);_tmpFF;});_tmpFE.f3=({struct _tuple2 _tmp100;_tmp100.f1=
e3;_tmp100.f2=Cyc_Absyn_skip_stmt(e3->loc);_tmp100;});_tmpFE.f4=s;_tmpFE;});
_tmpFD;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Switch_s_struct*_tmp101=_cycalloc(sizeof(*_tmp101));
_tmp101[0]=({struct Cyc_Absyn_Switch_s_struct _tmp102;_tmp102.tag=9;_tmp102.f1=e;
_tmp102.f2=scs;_tmp102;});_tmp101;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc){
struct _tuple9 _tmp104=({struct _tuple9 _tmp103;_tmp103.f1=(void*)s1->r;_tmp103.f2=(
void*)s2->r;_tmp103;});void*_tmp105;void*_tmp106;_LL64: _tmp105=_tmp104.f1;if((
int)_tmp105 != 0)goto _LL66;_LL65: return s2;_LL66: _tmp106=_tmp104.f2;if((int)
_tmp106 != 0)goto _LL68;_LL67: return s1;_LL68:;_LL69: return Cyc_Absyn_new_stmt((void*)({
struct Cyc_Absyn_Seq_s_struct*_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107[0]=({
struct Cyc_Absyn_Seq_s_struct _tmp108;_tmp108.tag=1;_tmp108.f1=s1;_tmp108.f2=s2;
_tmp108;});_tmp107;}),loc);_LL63:;}struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Fallthru_s_struct*_tmp109=_cycalloc(sizeof(*_tmp109));
_tmp109[0]=({struct Cyc_Absyn_Fallthru_s_struct _tmp10A;_tmp10A.tag=10;_tmp10A.f1=
el;_tmp10A.f2=0;_tmp10A;});_tmp109;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*_tmp10B=
_cycalloc(sizeof(*_tmp10B));_tmp10B[0]=({struct Cyc_Absyn_Decl_s_struct _tmp10C;
_tmp10C.tag=11;_tmp10C.f1=d;_tmp10C.f2=s;_tmp10C;});_tmp10B;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl*d=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F[
0]=({struct Cyc_Absyn_Var_d_struct _tmp110;_tmp110.tag=0;_tmp110.f1=Cyc_Absyn_new_vardecl(
x,t,init);_tmp110;});_tmp10F;}),loc);return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp10E;_tmp10E.tag=11;_tmp10E.f1=d;_tmp10E.f2=s;_tmp10E;});_tmp10D;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_struct*
_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111[0]=({struct Cyc_Absyn_Label_s_struct
_tmp112;_tmp112.tag=12;_tmp112.f1=v;_tmp112.f2=s;_tmp112;});_tmp111;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Do_s_struct*
_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113[0]=({struct Cyc_Absyn_Do_s_struct
_tmp114;_tmp114.tag=13;_tmp114.f1=s;_tmp114.f2=({struct _tuple2 _tmp115;_tmp115.f1=
e;_tmp115.f2=Cyc_Absyn_skip_stmt(e->loc);_tmp115;});_tmp114;});_tmp113;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_TryCatch_s_struct*
_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116[0]=({struct Cyc_Absyn_TryCatch_s_struct
_tmp117;_tmp117.tag=14;_tmp117.f1=s;_tmp117.f2=scs;_tmp117;});_tmp116;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dynforward_ptr*lab,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Goto_s_struct*_tmp118=
_cycalloc(sizeof(*_tmp118));_tmp118[0]=({struct Cyc_Absyn_Goto_s_struct _tmp119;
_tmp119.tag=7;_tmp119.f1=lab;_tmp119.f2=0;_tmp119;});_tmp118;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(e1,e2,loc),loc);}struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s){return({struct Cyc_Absyn_Pat*
_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->r=(void*)p;_tmp11A->topt=0;_tmp11A->loc=
s;_tmp11A;});}struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_struct*_tmp11B=_cycalloc(
sizeof(*_tmp11B));_tmp11B[0]=({struct Cyc_Absyn_Exp_p_struct _tmp11C;_tmp11C.tag=
14;_tmp11C.f1=e;_tmp11C;});_tmp11B;}),e->loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(
void*r,struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Decl*_tmp11D=
_cycalloc(sizeof(*_tmp11D));_tmp11D->r=(void*)r;_tmp11D->loc=loc;_tmp11D;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Let_d_struct*
_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E[0]=({struct Cyc_Absyn_Let_d_struct
_tmp11F;_tmp11F.tag=2;_tmp11F.f1=p;_tmp11F.f2=0;_tmp11F.f3=e;_tmp11F;});_tmp11E;}),
loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Letv_d_struct*_tmp120=
_cycalloc(sizeof(*_tmp120));_tmp120[0]=({struct Cyc_Absyn_Letv_d_struct _tmp121;
_tmp121.tag=3;_tmp121.f1=vds;_tmp121;});_tmp120;}),loc);}struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){return({
struct Cyc_Absyn_Vardecl*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->sc=(void*)((
void*)2);_tmp122->name=x;_tmp122->tq=Cyc_Absyn_empty_tqual(0);_tmp122->type=(
void*)t;_tmp122->initializer=init;_tmp122->rgn=0;_tmp122->attributes=0;_tmp122->escapes=
0;_tmp122;});}struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,
void*t,struct Cyc_Absyn_Exp*init){return({struct Cyc_Absyn_Vardecl*_tmp123=
_cycalloc(sizeof(*_tmp123));_tmp123->sc=(void*)((void*)0);_tmp123->name=x;
_tmp123->tq=Cyc_Absyn_empty_tqual(0);_tmp123->type=(void*)t;_tmp123->initializer=
init;_tmp123->rgn=0;_tmp123->attributes=0;_tmp123->escapes=0;_tmp123;});}struct
Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct
Cyc_List_List*po,struct Cyc_List_List*fs){return({struct Cyc_Absyn_AggrdeclImpl*
_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->exist_vars=exists;_tmp124->rgn_po=
po;_tmp124->fields=fs;_tmp124;});}struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*
k,void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp125=_cycalloc(sizeof(*_tmp125));
_tmp125[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp126;_tmp126.tag=4;_tmp126.f1=({
struct Cyc_Absyn_Aggrdecl*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->kind=(void*)
k;_tmp127->sc=(void*)s;_tmp127->name=n;_tmp127->tvs=ts;_tmp127->impl=i;_tmp127->attributes=
atts;_tmp127;});_tmp126;});_tmp125;}),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((
void*)0,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(void*s,
struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((void*)1,s,n,ts,i,
atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,
struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_xtunion,int is_flat,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp128=
_cycalloc(sizeof(*_tmp128));_tmp128[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp129;
_tmp129.tag=5;_tmp129.f1=({struct Cyc_Absyn_Tuniondecl*_tmp12A=_cycalloc(sizeof(*
_tmp12A));_tmp12A->sc=(void*)s;_tmp12A->name=n;_tmp12A->tvs=ts;_tmp12A->fields=
fs;_tmp12A->is_xtunion=is_xtunion;_tmp12A->is_flat=is_flat;_tmp12A;});_tmp129;});
_tmp128;}),loc);}static struct _tuple1*Cyc_Absyn_expand_arg(struct _tuple1*a){
return({struct _tuple1*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->f1=(*a).f1;
_tmp12B->f2=(*a).f2;_tmp12B->f3=Cyc_Absyn_pointer_expand((*a).f3,1);_tmp12B;});}
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,
void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts){return(void*)({
struct Cyc_Absyn_FnType_struct*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C[0]=({
struct Cyc_Absyn_FnType_struct _tmp12D;_tmp12D.tag=8;_tmp12D.f1=({struct Cyc_Absyn_FnInfo
_tmp12E;_tmp12E.tvars=tvs;_tmp12E.ret_typ=(void*)Cyc_Absyn_pointer_expand(
ret_typ,0);_tmp12E.effect=eff_typ;_tmp12E.args=((struct Cyc_List_List*(*)(struct
_tuple1*(*f)(struct _tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absyn_expand_arg,
args);_tmp12E.c_varargs=c_varargs;_tmp12E.cyc_varargs=cyc_varargs;_tmp12E.rgn_po=
rgn_po;_tmp12E.attributes=atts;_tmp12E;});_tmp12D;});_tmp12C;});}void*Cyc_Absyn_pointer_expand(
void*t,int fresh_evar){void*_tmp12F=Cyc_Tcutil_compress(t);_LL6B: if(_tmp12F <= (
void*)4)goto _LL6D;if(*((int*)_tmp12F)!= 8)goto _LL6D;_LL6C: return Cyc_Absyn_at_typ(
t,fresh_evar?Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp130=_cycalloc(sizeof(*
_tmp130));_tmp130->v=(void*)((void*)3);_tmp130;}),0):(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);_LL6D:;_LL6E: return t;_LL6A:;}int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*e){void*_tmp131=(void*)e->r;void*_tmp132;void*_tmp133;struct
Cyc_Absyn_Vardecl*_tmp134;void*_tmp135;struct Cyc_Absyn_Vardecl*_tmp136;struct Cyc_Absyn_Exp*
_tmp137;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Exp*_tmp139;_LL70: if(*((int*)
_tmp131)!= 1)goto _LL72;_tmp132=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp131)->f2;
if(_tmp132 <= (void*)1)goto _LL72;if(*((int*)_tmp132)!= 1)goto _LL72;_LL71: return 0;
_LL72: if(*((int*)_tmp131)!= 1)goto _LL74;_tmp133=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp131)->f2;if(_tmp133 <= (void*)1)goto _LL74;if(*((int*)_tmp133)!= 0)goto _LL74;
_tmp134=((struct Cyc_Absyn_Global_b_struct*)_tmp133)->f1;_LL73: _tmp136=_tmp134;
goto _LL75;_LL74: if(*((int*)_tmp131)!= 1)goto _LL76;_tmp135=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp131)->f2;if(_tmp135 <= (void*)1)goto _LL76;if(*((int*)_tmp135)!= 3)goto _LL76;
_tmp136=((struct Cyc_Absyn_Local_b_struct*)_tmp135)->f1;_LL75: {void*_tmp13A=Cyc_Tcutil_compress((
void*)_tmp136->type);_LL87: if(_tmp13A <= (void*)4)goto _LL89;if(*((int*)_tmp13A)!= 
7)goto _LL89;_LL88: return 0;_LL89:;_LL8A: return 1;_LL86:;}_LL76: if(*((int*)_tmp131)
!= 1)goto _LL78;_LL77: goto _LL79;_LL78: if(*((int*)_tmp131)!= 24)goto _LL7A;_LL79:
goto _LL7B;_LL7A: if(*((int*)_tmp131)!= 22)goto _LL7C;_LL7B: goto _LL7D;_LL7C: if(*((
int*)_tmp131)!= 25)goto _LL7E;_LL7D: return 1;_LL7E: if(*((int*)_tmp131)!= 23)goto
_LL80;_tmp137=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp131)->f1;_LL7F: return
Cyc_Absyn_is_lvalue(_tmp137);_LL80: if(*((int*)_tmp131)!= 14)goto _LL82;_tmp138=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmp131)->f1;_LL81: return Cyc_Absyn_is_lvalue(
_tmp138);_LL82: if(*((int*)_tmp131)!= 13)goto _LL84;_tmp139=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp131)->f1;_LL83: return Cyc_Absyn_is_lvalue(_tmp139);_LL84:;_LL85: return 0;_LL6F:;}
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,
struct _dynforward_ptr*v){{struct Cyc_List_List*_tmp13B=fields;for(0;_tmp13B != 0;
_tmp13B=_tmp13B->tl){if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)_tmp13B->hd)->name,
v)== 0)return(struct Cyc_Absyn_Aggrfield*)((struct Cyc_Absyn_Aggrfield*)_tmp13B->hd);}}
return 0;}struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*
ad,struct _dynforward_ptr*v){return ad->impl == 0?0: Cyc_Absyn_lookup_field(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple3*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*ts,int i){for(0;i != 0;-- i){if(ts == 0)return 0;ts=ts->tl;}if(ts
== 0)return 0;return(struct _tuple3*)((struct _tuple3*)ts->hd);}struct
_dynforward_ptr Cyc_Absyn_attribute2string(void*a){void*_tmp13C=a;int _tmp13D;int
_tmp13E;struct _dynforward_ptr _tmp13F;void*_tmp140;int _tmp141;int _tmp142;void*
_tmp143;int _tmp144;int _tmp145;int _tmp146;struct _dynforward_ptr _tmp147;_LL8C: if(
_tmp13C <= (void*)17)goto _LL8E;if(*((int*)_tmp13C)!= 0)goto _LL8E;_tmp13D=((struct
Cyc_Absyn_Regparm_att_struct*)_tmp13C)->f1;_LL8D: return(struct _dynforward_ptr)({
struct Cyc_Int_pa_struct _tmp14A;_tmp14A.tag=1;_tmp14A.f1=(unsigned long)_tmp13D;{
void*_tmp148[1]={& _tmp14A};Cyc_aprintf(({const char*_tmp149="regparm(%d)";
_tag_dynforward(_tmp149,sizeof(char),_get_zero_arr_size_char(_tmp149,12));}),
_tag_dynforward(_tmp148,sizeof(void*),1));}});_LL8E: if((int)_tmp13C != 0)goto
_LL90;_LL8F: return({const char*_tmp14B="stdcall";_tag_dynforward(_tmp14B,sizeof(
char),_get_zero_arr_size_char(_tmp14B,8));});_LL90: if((int)_tmp13C != 1)goto _LL92;
_LL91: return({const char*_tmp14C="cdecl";_tag_dynforward(_tmp14C,sizeof(char),
_get_zero_arr_size_char(_tmp14C,6));});_LL92: if((int)_tmp13C != 2)goto _LL94;_LL93:
return({const char*_tmp14D="fastcall";_tag_dynforward(_tmp14D,sizeof(char),
_get_zero_arr_size_char(_tmp14D,9));});_LL94: if((int)_tmp13C != 3)goto _LL96;_LL95:
return({const char*_tmp14E="noreturn";_tag_dynforward(_tmp14E,sizeof(char),
_get_zero_arr_size_char(_tmp14E,9));});_LL96: if((int)_tmp13C != 4)goto _LL98;_LL97:
return({const char*_tmp14F="const";_tag_dynforward(_tmp14F,sizeof(char),
_get_zero_arr_size_char(_tmp14F,6));});_LL98: if(_tmp13C <= (void*)17)goto _LL9A;
if(*((int*)_tmp13C)!= 1)goto _LL9A;_tmp13E=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp13C)->f1;_LL99: if(_tmp13E == - 1)return({const char*_tmp150="aligned";
_tag_dynforward(_tmp150,sizeof(char),_get_zero_arr_size_char(_tmp150,8));});
else{void*_tmp151=Cyc_Cyclone_c_compiler;_LLBD: if((int)_tmp151 != 0)goto _LLBF;
_LLBE: return(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp154;_tmp154.tag=
1;_tmp154.f1=(unsigned long)_tmp13E;{void*_tmp152[1]={& _tmp154};Cyc_aprintf(({
const char*_tmp153="aligned(%d)";_tag_dynforward(_tmp153,sizeof(char),
_get_zero_arr_size_char(_tmp153,12));}),_tag_dynforward(_tmp152,sizeof(void*),1));}});
_LLBF: if((int)_tmp151 != 1)goto _LLBC;_LLC0: return(struct _dynforward_ptr)({struct
Cyc_Int_pa_struct _tmp157;_tmp157.tag=1;_tmp157.f1=(unsigned long)_tmp13E;{void*
_tmp155[1]={& _tmp157};Cyc_aprintf(({const char*_tmp156="align(%d)";
_tag_dynforward(_tmp156,sizeof(char),_get_zero_arr_size_char(_tmp156,10));}),
_tag_dynforward(_tmp155,sizeof(void*),1));}});_LLBC:;}_LL9A: if((int)_tmp13C != 5)
goto _LL9C;_LL9B: return({const char*_tmp158="packed";_tag_dynforward(_tmp158,
sizeof(char),_get_zero_arr_size_char(_tmp158,7));});_LL9C: if(_tmp13C <= (void*)17)
goto _LL9E;if(*((int*)_tmp13C)!= 2)goto _LL9E;_tmp13F=((struct Cyc_Absyn_Section_att_struct*)
_tmp13C)->f1;_LL9D: return(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp15B;_tmp15B.tag=0;_tmp15B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp13F);{void*_tmp159[1]={& _tmp15B};Cyc_aprintf(({const char*_tmp15A="section(\"%s\")";
_tag_dynforward(_tmp15A,sizeof(char),_get_zero_arr_size_char(_tmp15A,14));}),
_tag_dynforward(_tmp159,sizeof(void*),1));}});_LL9E: if((int)_tmp13C != 6)goto
_LLA0;_LL9F: return({const char*_tmp15C="nocommon";_tag_dynforward(_tmp15C,sizeof(
char),_get_zero_arr_size_char(_tmp15C,9));});_LLA0: if((int)_tmp13C != 7)goto _LLA2;
_LLA1: return({const char*_tmp15D="shared";_tag_dynforward(_tmp15D,sizeof(char),
_get_zero_arr_size_char(_tmp15D,7));});_LLA2: if((int)_tmp13C != 8)goto _LLA4;_LLA3:
return({const char*_tmp15E="unused";_tag_dynforward(_tmp15E,sizeof(char),
_get_zero_arr_size_char(_tmp15E,7));});_LLA4: if((int)_tmp13C != 9)goto _LLA6;_LLA5:
return({const char*_tmp15F="weak";_tag_dynforward(_tmp15F,sizeof(char),
_get_zero_arr_size_char(_tmp15F,5));});_LLA6: if((int)_tmp13C != 10)goto _LLA8;
_LLA7: return({const char*_tmp160="dllimport";_tag_dynforward(_tmp160,sizeof(char),
_get_zero_arr_size_char(_tmp160,10));});_LLA8: if((int)_tmp13C != 11)goto _LLAA;
_LLA9: return({const char*_tmp161="dllexport";_tag_dynforward(_tmp161,sizeof(char),
_get_zero_arr_size_char(_tmp161,10));});_LLAA: if((int)_tmp13C != 12)goto _LLAC;
_LLAB: return({const char*_tmp162="no_instrument_function";_tag_dynforward(_tmp162,
sizeof(char),_get_zero_arr_size_char(_tmp162,23));});_LLAC: if((int)_tmp13C != 13)
goto _LLAE;_LLAD: return({const char*_tmp163="constructor";_tag_dynforward(_tmp163,
sizeof(char),_get_zero_arr_size_char(_tmp163,12));});_LLAE: if((int)_tmp13C != 14)
goto _LLB0;_LLAF: return({const char*_tmp164="destructor";_tag_dynforward(_tmp164,
sizeof(char),_get_zero_arr_size_char(_tmp164,11));});_LLB0: if((int)_tmp13C != 15)
goto _LLB2;_LLB1: return({const char*_tmp165="no_check_memory_usage";
_tag_dynforward(_tmp165,sizeof(char),_get_zero_arr_size_char(_tmp165,22));});
_LLB2: if(_tmp13C <= (void*)17)goto _LLB8;if(*((int*)_tmp13C)!= 3)goto _LLB4;_tmp140=(
void*)((struct Cyc_Absyn_Format_att_struct*)_tmp13C)->f1;if((int)_tmp140 != 0)goto
_LLB4;_tmp141=((struct Cyc_Absyn_Format_att_struct*)_tmp13C)->f2;_tmp142=((struct
Cyc_Absyn_Format_att_struct*)_tmp13C)->f3;_LLB3: return(struct _dynforward_ptr)({
struct Cyc_Int_pa_struct _tmp169;_tmp169.tag=1;_tmp169.f1=(unsigned int)_tmp142;{
struct Cyc_Int_pa_struct _tmp168;_tmp168.tag=1;_tmp168.f1=(unsigned int)_tmp141;{
void*_tmp166[2]={& _tmp168,& _tmp169};Cyc_aprintf(({const char*_tmp167="format(printf,%u,%u)";
_tag_dynforward(_tmp167,sizeof(char),_get_zero_arr_size_char(_tmp167,21));}),
_tag_dynforward(_tmp166,sizeof(void*),2));}}});_LLB4: if(*((int*)_tmp13C)!= 3)
goto _LLB6;_tmp143=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp13C)->f1;if((
int)_tmp143 != 1)goto _LLB6;_tmp144=((struct Cyc_Absyn_Format_att_struct*)_tmp13C)->f2;
_tmp145=((struct Cyc_Absyn_Format_att_struct*)_tmp13C)->f3;_LLB5: return(struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp16D;_tmp16D.tag=1;_tmp16D.f1=(
unsigned int)_tmp145;{struct Cyc_Int_pa_struct _tmp16C;_tmp16C.tag=1;_tmp16C.f1=(
unsigned int)_tmp144;{void*_tmp16A[2]={& _tmp16C,& _tmp16D};Cyc_aprintf(({const
char*_tmp16B="format(scanf,%u,%u)";_tag_dynforward(_tmp16B,sizeof(char),
_get_zero_arr_size_char(_tmp16B,20));}),_tag_dynforward(_tmp16A,sizeof(void*),2));}}});
_LLB6: if(*((int*)_tmp13C)!= 4)goto _LLB8;_tmp146=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp13C)->f1;_LLB7: return(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp170;
_tmp170.tag=1;_tmp170.f1=(unsigned long)_tmp146;{void*_tmp16E[1]={& _tmp170};Cyc_aprintf(({
const char*_tmp16F="initializes(%d)";_tag_dynforward(_tmp16F,sizeof(char),
_get_zero_arr_size_char(_tmp16F,16));}),_tag_dynforward(_tmp16E,sizeof(void*),1));}});
_LLB8: if((int)_tmp13C != 16)goto _LLBA;_LLB9: return({const char*_tmp171="pure";
_tag_dynforward(_tmp171,sizeof(char),_get_zero_arr_size_char(_tmp171,5));});
_LLBA: if(_tmp13C <= (void*)17)goto _LL8B;if(*((int*)_tmp13C)!= 5)goto _LL8B;_tmp147=((
struct Cyc_Absyn_Mode_att_struct*)_tmp13C)->f1;_LLBB: return(struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp174;_tmp174.tag=0;_tmp174.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp147);{void*_tmp172[1]={& _tmp174};Cyc_aprintf(({
const char*_tmp173="__mode__(\"%s\")";_tag_dynforward(_tmp173,sizeof(char),
_get_zero_arr_size_char(_tmp173,15));}),_tag_dynforward(_tmp172,sizeof(void*),1));}});
_LL8B:;}int Cyc_Absyn_fntype_att(void*a){void*_tmp175=a;_LLC2: if(_tmp175 <= (void*)
17)goto _LLC4;if(*((int*)_tmp175)!= 0)goto _LLC4;_LLC3: goto _LLC5;_LLC4: if((int)
_tmp175 != 2)goto _LLC6;_LLC5: goto _LLC7;_LLC6: if((int)_tmp175 != 0)goto _LLC8;_LLC7:
goto _LLC9;_LLC8: if((int)_tmp175 != 1)goto _LLCA;_LLC9: goto _LLCB;_LLCA: if((int)
_tmp175 != 3)goto _LLCC;_LLCB: goto _LLCD;_LLCC: if((int)_tmp175 != 16)goto _LLCE;_LLCD:
goto _LLCF;_LLCE: if(_tmp175 <= (void*)17)goto _LLD0;if(*((int*)_tmp175)!= 3)goto
_LLD0;_LLCF: goto _LLD1;_LLD0: if((int)_tmp175 != 4)goto _LLD2;_LLD1: return 1;_LLD2:
if(_tmp175 <= (void*)17)goto _LLD4;if(*((int*)_tmp175)!= 4)goto _LLD4;_LLD3: return 1;
_LLD4:;_LLD5: return 0;_LLC1:;}static char _tmp176[3]="f0";static struct
_dynforward_ptr Cyc_Absyn_f0={_tmp176,_tmp176 + 3};static struct _dynforward_ptr*Cyc_Absyn_field_names_v[
1]={& Cyc_Absyn_f0};static struct _dynforward_ptr Cyc_Absyn_field_names={(void*)((
struct _dynforward_ptr**)Cyc_Absyn_field_names_v),(void*)((struct _dynforward_ptr**)
Cyc_Absyn_field_names_v + 1)};struct _dynforward_ptr*Cyc_Absyn_fieldname(int i){
unsigned int fsz=_get_dynforward_size(Cyc_Absyn_field_names,sizeof(struct
_dynforward_ptr*));if(i >= fsz)Cyc_Absyn_field_names=({unsigned int _tmp177=(
unsigned int)(i + 1);struct _dynforward_ptr**_tmp178=(struct _dynforward_ptr**)
_cycalloc(_check_times(sizeof(struct _dynforward_ptr*),_tmp177));struct
_dynforward_ptr _tmp17E=_tag_dynforward(_tmp178,sizeof(struct _dynforward_ptr*),
_tmp177);{unsigned int _tmp179=_tmp177;unsigned int j;for(j=0;j < _tmp179;j ++){
_tmp178[j]=j < fsz?*((struct _dynforward_ptr**)_check_dynforward_subscript(Cyc_Absyn_field_names,
sizeof(struct _dynforward_ptr*),(int)j)):({struct _dynforward_ptr*_tmp17A=
_cycalloc(sizeof(*_tmp17A));_tmp17A[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp17D;_tmp17D.tag=1;_tmp17D.f1=(unsigned long)((int)j);{void*_tmp17B[1]={&
_tmp17D};Cyc_aprintf(({const char*_tmp17C="f%d";_tag_dynforward(_tmp17C,sizeof(
char),_get_zero_arr_size_char(_tmp17C,4));}),_tag_dynforward(_tmp17B,sizeof(void*),
1));}});_tmp17A;});}}_tmp17E;});return*((struct _dynforward_ptr**)
_check_dynforward_subscript(Cyc_Absyn_field_names,sizeof(struct _dynforward_ptr*),
i));}struct _tuple4 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU_union info){
union Cyc_Absyn_AggrInfoU_union _tmp17F=info;void*_tmp180;struct _tuple0*_tmp181;
struct Cyc_Absyn_Aggrdecl**_tmp182;struct Cyc_Absyn_Aggrdecl*_tmp183;struct Cyc_Absyn_Aggrdecl
_tmp184;void*_tmp185;struct _tuple0*_tmp186;_LLD7: if((_tmp17F.UnknownAggr).tag != 
0)goto _LLD9;_tmp180=(_tmp17F.UnknownAggr).f1;_tmp181=(_tmp17F.UnknownAggr).f2;
_LLD8: return({struct _tuple4 _tmp187;_tmp187.f1=_tmp180;_tmp187.f2=_tmp181;_tmp187;});
_LLD9: if((_tmp17F.KnownAggr).tag != 1)goto _LLD6;_tmp182=(_tmp17F.KnownAggr).f1;
_tmp183=*_tmp182;_tmp184=*_tmp183;_tmp185=(void*)_tmp184.kind;_tmp186=_tmp184.name;
_LLDA: return({struct _tuple4 _tmp188;_tmp188.f1=_tmp185;_tmp188.f2=_tmp186;_tmp188;});
_LLD6:;}struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
info){union Cyc_Absyn_AggrInfoU_union _tmp189=info;void*_tmp18A;struct _tuple0*
_tmp18B;struct Cyc_Absyn_Aggrdecl**_tmp18C;struct Cyc_Absyn_Aggrdecl*_tmp18D;_LLDC:
if((_tmp189.UnknownAggr).tag != 0)goto _LLDE;_tmp18A=(_tmp189.UnknownAggr).f1;
_tmp18B=(_tmp189.UnknownAggr).f2;_LLDD:({void*_tmp18E=0;((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp18F="unchecked aggrdecl";_tag_dynforward(_tmp18F,sizeof(char),
_get_zero_arr_size_char(_tmp18F,19));}),_tag_dynforward(_tmp18E,sizeof(void*),0));});
_LLDE: if((_tmp189.KnownAggr).tag != 1)goto _LLDB;_tmp18C=(_tmp189.KnownAggr).f1;
_tmp18D=*_tmp18C;_LLDF: return _tmp18D;_LLDB:;}int Cyc_Absyn_is_union_type(void*t){
void*_tmp190=Cyc_Tcutil_compress(t);void*_tmp191;struct Cyc_Absyn_AggrInfo _tmp192;
union Cyc_Absyn_AggrInfoU_union _tmp193;_LLE1: if(_tmp190 <= (void*)4)goto _LLE5;if(*((
int*)_tmp190)!= 11)goto _LLE3;_tmp191=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp190)->f1;if((int)_tmp191 != 1)goto _LLE3;_LLE2: return 1;_LLE3: if(*((int*)
_tmp190)!= 10)goto _LLE5;_tmp192=((struct Cyc_Absyn_AggrType_struct*)_tmp190)->f1;
_tmp193=_tmp192.aggr_info;_LLE4: return(Cyc_Absyn_aggr_kinded_name(_tmp193)).f1 == (
void*)1;_LLE5:;_LLE6: return 0;_LLE0:;}int Cyc_Absyn_is_aggr_type(void*t){void*
_tmp194=Cyc_Tcutil_compress(t);_LLE8: if(_tmp194 <= (void*)4)goto _LLF2;if(*((int*)
_tmp194)!= 2)goto _LLEA;_LLE9: goto _LLEB;_LLEA: if(*((int*)_tmp194)!= 3)goto _LLEC;
_LLEB: goto _LLED;_LLEC: if(*((int*)_tmp194)!= 9)goto _LLEE;_LLED: goto _LLEF;_LLEE:
if(*((int*)_tmp194)!= 10)goto _LLF0;_LLEF: goto _LLF1;_LLF0: if(*((int*)_tmp194)!= 
11)goto _LLF2;_LLF1: return 1;_LLF2:;_LLF3: return 0;_LLE7:;}void Cyc_Absyn_print_decls(
struct Cyc_List_List*decls){((void(*)(void*rep,struct Cyc_List_List**val))Cyc_Marshal_print_type)(
Cyc_decls_rep,& decls);({void*_tmp195=0;Cyc_printf(({const char*_tmp196="\n";
_tag_dynforward(_tmp196,sizeof(char),_get_zero_arr_size_char(_tmp196,2));}),
_tag_dynforward(_tmp195,sizeof(void*),0));});}int Cyc_Absyn_porting_c_code=0;int
Cyc_Absyn_no_regions=0;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_0;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Decl_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_825;static struct Cyc_Typerep_Int_struct
Cyc__genrep_24={0,0,32};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_159;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep;extern struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep;static char _tmp198[7]="Static";
static struct _tuple7 Cyc__gentuple_167={0,(struct _dynforward_ptr){_tmp198,_tmp198 + 
7}};static char _tmp199[9]="Abstract";static struct _tuple7 Cyc__gentuple_168={1,(
struct _dynforward_ptr){_tmp199,_tmp199 + 9}};static char _tmp19A[7]="Public";static
struct _tuple7 Cyc__gentuple_169={2,(struct _dynforward_ptr){_tmp19A,_tmp19A + 7}};
static char _tmp19B[7]="Extern";static struct _tuple7 Cyc__gentuple_170={3,(struct
_dynforward_ptr){_tmp19B,_tmp19B + 7}};static char _tmp19C[8]="ExternC";static
struct _tuple7 Cyc__gentuple_171={4,(struct _dynforward_ptr){_tmp19C,_tmp19C + 8}};
static char _tmp19D[9]="Register";static struct _tuple7 Cyc__gentuple_172={5,(struct
_dynforward_ptr){_tmp19D,_tmp19D + 9}};static struct _tuple7*Cyc__genarr_173[6]={&
Cyc__gentuple_167,& Cyc__gentuple_168,& Cyc__gentuple_169,& Cyc__gentuple_170,& Cyc__gentuple_171,&
Cyc__gentuple_172};static struct _tuple5*Cyc__genarr_174[0]={};static char _tmp19F[6]="Scope";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep={5,(struct _dynforward_ptr){
_tmp19F,_tmp19F + 6},{(void*)((struct _tuple7**)Cyc__genarr_173),(void*)((struct
_tuple7**)Cyc__genarr_173 + 6)},{(void*)((struct _tuple5**)Cyc__genarr_174),(void*)((
struct _tuple5**)Cyc__genarr_174 + 0)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_11;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_12;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_nmspace_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_18;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_19;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_var_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_13;extern struct Cyc_Typerep_FatPtr_struct Cyc__genrep_14;static struct
Cyc_Typerep_Int_struct Cyc__genrep_15={0,0,8};static struct Cyc_Typerep_FatPtr_struct
Cyc__genrep_14={2,(void*)((void*)& Cyc__genrep_15)};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_13={1,1,(void*)((void*)& Cyc__genrep_14)};static char _tmp1A3[5]="List";
static struct _dynforward_ptr Cyc__genname_23=(struct _dynforward_ptr){_tmp1A3,
_tmp1A3 + 5};static char _tmp1A4[3]="hd";static struct _tuple5 Cyc__gentuple_20={
offsetof(struct Cyc_List_List,hd),(struct _dynforward_ptr){_tmp1A4,_tmp1A4 + 3},(
void*)& Cyc__genrep_13};static char _tmp1A5[3]="tl";static struct _tuple5 Cyc__gentuple_21={
offsetof(struct Cyc_List_List,tl),(struct _dynforward_ptr){_tmp1A5,_tmp1A5 + 3},(
void*)& Cyc__genrep_19};static struct _tuple5*Cyc__genarr_22[2]={& Cyc__gentuple_20,&
Cyc__gentuple_21};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_var_t46H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_23,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_22),(void*)((struct _tuple5**)Cyc__genarr_22 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_19={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_var_t46H2_rep)};
struct _tuple14{unsigned int f1;struct Cyc_List_List*f2;};static struct _tuple6 Cyc__gentuple_25={
offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_26={
offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_19};static struct _tuple6*Cyc__genarr_27[
2]={& Cyc__gentuple_25,& Cyc__gentuple_26};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_18={
4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_27),(void*)((
struct _tuple6**)Cyc__genarr_27 + 2)}};static char _tmp1A9[6]="Loc_n";static struct
_tuple7 Cyc__gentuple_16={0,(struct _dynforward_ptr){_tmp1A9,_tmp1A9 + 6}};static
struct _tuple7*Cyc__genarr_17[1]={& Cyc__gentuple_16};static char _tmp1AA[6]="Rel_n";
static struct _tuple5 Cyc__gentuple_28={0,(struct _dynforward_ptr){_tmp1AA,_tmp1AA + 
6},(void*)& Cyc__genrep_18};static char _tmp1AB[6]="Abs_n";static struct _tuple5 Cyc__gentuple_29={
1,(struct _dynforward_ptr){_tmp1AB,_tmp1AB + 6},(void*)& Cyc__genrep_18};static
struct _tuple5*Cyc__genarr_30[2]={& Cyc__gentuple_28,& Cyc__gentuple_29};static char
_tmp1AD[8]="Nmspace";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep={5,(
struct _dynforward_ptr){_tmp1AD,_tmp1AD + 8},{(void*)((struct _tuple7**)Cyc__genarr_17),(
void*)((struct _tuple7**)Cyc__genarr_17 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_30),(
void*)((struct _tuple5**)Cyc__genarr_30 + 2)}};static struct _tuple6 Cyc__gentuple_31={
offsetof(struct _tuple0,f1),(void*)& Cyc_Absyn_nmspace_t_rep};static struct _tuple6
Cyc__gentuple_32={offsetof(struct _tuple0,f2),(void*)& Cyc__genrep_13};static
struct _tuple6*Cyc__genarr_33[2]={& Cyc__gentuple_31,& Cyc__gentuple_32};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_12={4,sizeof(struct _tuple0),{(void*)((
struct _tuple6**)Cyc__genarr_33),(void*)((struct _tuple6**)Cyc__genarr_33 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_11={1,1,(void*)((void*)& Cyc__genrep_12)};
extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_tqual_t_rep;static struct Cyc_Typerep_Int_struct
Cyc__genrep_10={0,1,32};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2={1,1,(void*)((void*)& Cyc_struct_Position_Segment_rep)};
static char _tmp1B2[6]="Tqual";static struct _dynforward_ptr Cyc__genname_166=(struct
_dynforward_ptr){_tmp1B2,_tmp1B2 + 6};static char _tmp1B3[12]="print_const";static
struct _tuple5 Cyc__gentuple_160={offsetof(struct Cyc_Absyn_Tqual,print_const),(
struct _dynforward_ptr){_tmp1B3,_tmp1B3 + 12},(void*)& Cyc__genrep_10};static char
_tmp1B4[11]="q_volatile";static struct _tuple5 Cyc__gentuple_161={offsetof(struct
Cyc_Absyn_Tqual,q_volatile),(struct _dynforward_ptr){_tmp1B4,_tmp1B4 + 11},(void*)&
Cyc__genrep_10};static char _tmp1B5[11]="q_restrict";static struct _tuple5 Cyc__gentuple_162={
offsetof(struct Cyc_Absyn_Tqual,q_restrict),(struct _dynforward_ptr){_tmp1B5,
_tmp1B5 + 11},(void*)& Cyc__genrep_10};static char _tmp1B6[11]="real_const";static
struct _tuple5 Cyc__gentuple_163={offsetof(struct Cyc_Absyn_Tqual,real_const),(
struct _dynforward_ptr){_tmp1B6,_tmp1B6 + 11},(void*)& Cyc__genrep_10};static char
_tmp1B7[4]="loc";static struct _tuple5 Cyc__gentuple_164={offsetof(struct Cyc_Absyn_Tqual,loc),(
struct _dynforward_ptr){_tmp1B7,_tmp1B7 + 4},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_165[5]={& Cyc__gentuple_160,& Cyc__gentuple_161,& Cyc__gentuple_162,&
Cyc__gentuple_163,& Cyc__gentuple_164};struct Cyc_Typerep_Struct_struct Cyc_Absyn_tqual_t_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_166,sizeof(struct Cyc_Absyn_Tqual),{(void*)((
struct _tuple5**)Cyc__genarr_165),(void*)((struct _tuple5**)Cyc__genarr_165 + 5)}};
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1118;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1123;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kind_t2_rep;extern
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kind_t_rep;static char _tmp1B9[8]="AnyKind";
static struct _tuple7 Cyc__gentuple_188={0,(struct _dynforward_ptr){_tmp1B9,_tmp1B9 + 
8}};static char _tmp1BA[8]="MemKind";static struct _tuple7 Cyc__gentuple_189={1,(
struct _dynforward_ptr){_tmp1BA,_tmp1BA + 8}};static char _tmp1BB[8]="BoxKind";
static struct _tuple7 Cyc__gentuple_190={2,(struct _dynforward_ptr){_tmp1BB,_tmp1BB + 
8}};static char _tmp1BC[8]="RgnKind";static struct _tuple7 Cyc__gentuple_191={3,(
struct _dynforward_ptr){_tmp1BC,_tmp1BC + 8}};static char _tmp1BD[14]="UniqueRgnKind";
static struct _tuple7 Cyc__gentuple_192={4,(struct _dynforward_ptr){_tmp1BD,_tmp1BD + 
14}};static char _tmp1BE[11]="TopRgnKind";static struct _tuple7 Cyc__gentuple_193={5,(
struct _dynforward_ptr){_tmp1BE,_tmp1BE + 11}};static char _tmp1BF[8]="EffKind";
static struct _tuple7 Cyc__gentuple_194={6,(struct _dynforward_ptr){_tmp1BF,_tmp1BF + 
8}};static char _tmp1C0[8]="IntKind";static struct _tuple7 Cyc__gentuple_195={7,(
struct _dynforward_ptr){_tmp1C0,_tmp1C0 + 8}};static struct _tuple7*Cyc__genarr_196[
8]={& Cyc__gentuple_188,& Cyc__gentuple_189,& Cyc__gentuple_190,& Cyc__gentuple_191,&
Cyc__gentuple_192,& Cyc__gentuple_193,& Cyc__gentuple_194,& Cyc__gentuple_195};
static struct _tuple5*Cyc__genarr_197[0]={};static char _tmp1C2[5]="Kind";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_kind_t_rep={5,(struct _dynforward_ptr){_tmp1C2,_tmp1C2 + 5},{(void*)((
struct _tuple7**)Cyc__genarr_196),(void*)((struct _tuple7**)Cyc__genarr_196 + 8)},{(
void*)((struct _tuple5**)Cyc__genarr_197),(void*)((struct _tuple5**)Cyc__genarr_197
+ 0)}};static char _tmp1C3[4]="Opt";static struct _dynforward_ptr Cyc__genname_1126=(
struct _dynforward_ptr){_tmp1C3,_tmp1C3 + 4};static char _tmp1C4[2]="v";static struct
_tuple5 Cyc__gentuple_1124={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp1C4,_tmp1C4 + 2},(void*)& Cyc_Absyn_kind_t_rep};static struct _tuple5*Cyc__genarr_1125[
1]={& Cyc__gentuple_1124};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kind_t2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_1126,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_1125),(void*)((struct _tuple5**)Cyc__genarr_1125 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1123={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kind_t2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_110;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_type_t2_rep;static char _tmp1C7[4]="Opt";static struct
_dynforward_ptr Cyc__genname_1159=(struct _dynforward_ptr){_tmp1C7,_tmp1C7 + 4};
static char _tmp1C8[2]="v";static struct _tuple5 Cyc__gentuple_1157={offsetof(struct
Cyc_Core_Opt,v),(struct _dynforward_ptr){_tmp1C8,_tmp1C8 + 2},(void*)& Cyc_Absyn_type_t_rep};
static struct _tuple5*Cyc__genarr_1158[1]={& Cyc__gentuple_1157};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_type_t2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_1159,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_1158),(void*)((
struct _tuple5**)Cyc__genarr_1158 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_110={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_type_t2_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1119;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_335;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tvar_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_185;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kindbound_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_210;static struct _tuple6 Cyc__gentuple_211={offsetof(struct _tuple6,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_212={offsetof(struct
_tuple6,f2),(void*)& Cyc_Absyn_kind_t_rep};static struct _tuple6*Cyc__genarr_213[2]={&
Cyc__gentuple_211,& Cyc__gentuple_212};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_210={
4,sizeof(struct _tuple6),{(void*)((struct _tuple6**)Cyc__genarr_213),(void*)((
struct _tuple6**)Cyc__genarr_213 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_206;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_198;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep;static char _tmp1CC[4]="Opt";static
struct _dynforward_ptr Cyc__genname_201=(struct _dynforward_ptr){_tmp1CC,_tmp1CC + 4};
static char _tmp1CD[2]="v";static struct _tuple5 Cyc__gentuple_199={offsetof(struct
Cyc_Core_Opt,v),(struct _dynforward_ptr){_tmp1CD,_tmp1CD + 2},(void*)& Cyc_Absyn_kindbound_t_rep};
static struct _tuple5*Cyc__genarr_200[1]={& Cyc__gentuple_199};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_201,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_200),(void*)((
struct _tuple5**)Cyc__genarr_200 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_198={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep)};struct _tuple15{
unsigned int f1;struct Cyc_Core_Opt*f2;};static struct _tuple6 Cyc__gentuple_207={
offsetof(struct _tuple15,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_208={
offsetof(struct _tuple15,f2),(void*)& Cyc__genrep_198};static struct _tuple6*Cyc__genarr_209[
2]={& Cyc__gentuple_207,& Cyc__gentuple_208};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_206={4,sizeof(struct _tuple15),{(void*)((struct _tuple6**)Cyc__genarr_209),(
void*)((struct _tuple6**)Cyc__genarr_209 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_187;struct _tuple16{unsigned int f1;struct Cyc_Core_Opt*f2;void*f3;};
static struct _tuple6 Cyc__gentuple_202={offsetof(struct _tuple16,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_203={offsetof(struct _tuple16,f2),(void*)& Cyc__genrep_198};
static struct _tuple6 Cyc__gentuple_204={offsetof(struct _tuple16,f3),(void*)& Cyc_Absyn_kind_t_rep};
static struct _tuple6*Cyc__genarr_205[3]={& Cyc__gentuple_202,& Cyc__gentuple_203,&
Cyc__gentuple_204};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_187={4,
sizeof(struct _tuple16),{(void*)((struct _tuple6**)Cyc__genarr_205),(void*)((
struct _tuple6**)Cyc__genarr_205 + 3)}};static struct _tuple7*Cyc__genarr_186[0]={};
static char _tmp1D2[6]="Eq_kb";static struct _tuple5 Cyc__gentuple_214={0,(struct
_dynforward_ptr){_tmp1D2,_tmp1D2 + 6},(void*)& Cyc__genrep_210};static char _tmp1D3[
11]="Unknown_kb";static struct _tuple5 Cyc__gentuple_215={1,(struct _dynforward_ptr){
_tmp1D3,_tmp1D3 + 11},(void*)& Cyc__genrep_206};static char _tmp1D4[8]="Less_kb";
static struct _tuple5 Cyc__gentuple_216={2,(struct _dynforward_ptr){_tmp1D4,_tmp1D4 + 
8},(void*)& Cyc__genrep_187};static struct _tuple5*Cyc__genarr_217[3]={& Cyc__gentuple_214,&
Cyc__gentuple_215,& Cyc__gentuple_216};static char _tmp1D6[10]="KindBound";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_kindbound_t_rep={5,(struct _dynforward_ptr){
_tmp1D6,_tmp1D6 + 10},{(void*)((struct _tuple7**)Cyc__genarr_186),(void*)((struct
_tuple7**)Cyc__genarr_186 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_217),(void*)((
struct _tuple5**)Cyc__genarr_217 + 3)}};static char _tmp1D7[5]="Tvar";static struct
_dynforward_ptr Cyc__genname_222=(struct _dynforward_ptr){_tmp1D7,_tmp1D7 + 5};
static char _tmp1D8[5]="name";static struct _tuple5 Cyc__gentuple_218={offsetof(
struct Cyc_Absyn_Tvar,name),(struct _dynforward_ptr){_tmp1D8,_tmp1D8 + 5},(void*)&
Cyc__genrep_13};static char _tmp1D9[9]="identity";static struct _tuple5 Cyc__gentuple_219={
offsetof(struct Cyc_Absyn_Tvar,identity),(struct _dynforward_ptr){_tmp1D9,_tmp1D9 + 
9},(void*)& Cyc__genrep_10};static char _tmp1DA[5]="kind";static struct _tuple5 Cyc__gentuple_220={
offsetof(struct Cyc_Absyn_Tvar,kind),(struct _dynforward_ptr){_tmp1DA,_tmp1DA + 5},(
void*)& Cyc_Absyn_kindbound_t_rep};static struct _tuple5*Cyc__genarr_221[3]={& Cyc__gentuple_218,&
Cyc__gentuple_219,& Cyc__gentuple_220};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_222,sizeof(struct Cyc_Absyn_Tvar),{(void*)((
struct _tuple5**)Cyc__genarr_221),(void*)((struct _tuple5**)Cyc__genarr_221 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_185={1,1,(void*)((void*)& Cyc_struct_Absyn_Tvar_rep)};
static char _tmp1DD[5]="List";static struct _dynforward_ptr Cyc__genname_339=(struct
_dynforward_ptr){_tmp1DD,_tmp1DD + 5};static char _tmp1DE[3]="hd";static struct
_tuple5 Cyc__gentuple_336={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp1DE,_tmp1DE + 3},(void*)& Cyc__genrep_185};static char _tmp1DF[
3]="tl";static struct _tuple5 Cyc__gentuple_337={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp1DF,_tmp1DF + 3},(void*)& Cyc__genrep_335};static struct
_tuple5*Cyc__genarr_338[2]={& Cyc__gentuple_336,& Cyc__gentuple_337};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tvar_t46H2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_339,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_338),(void*)((
struct _tuple5**)Cyc__genarr_338 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_335={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tvar_t46H2_rep)};static char
_tmp1E2[4]="Opt";static struct _dynforward_ptr Cyc__genname_1122=(struct
_dynforward_ptr){_tmp1E2,_tmp1E2 + 4};static char _tmp1E3[2]="v";static struct
_tuple5 Cyc__gentuple_1120={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp1E3,_tmp1E3 + 2},(void*)& Cyc__genrep_335};static struct _tuple5*Cyc__genarr_1121[
1]={& Cyc__gentuple_1120};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_1122,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_1121),(void*)((struct _tuple5**)Cyc__genarr_1121 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1119={1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep)};
struct _tuple17{unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;int f4;
struct Cyc_Core_Opt*f5;};static struct _tuple6 Cyc__gentuple_1127={offsetof(struct
_tuple17,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1128={
offsetof(struct _tuple17,f2),(void*)& Cyc__genrep_1123};static struct _tuple6 Cyc__gentuple_1129={
offsetof(struct _tuple17,f3),(void*)& Cyc__genrep_110};static struct _tuple6 Cyc__gentuple_1130={
offsetof(struct _tuple17,f4),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_1131={
offsetof(struct _tuple17,f5),(void*)& Cyc__genrep_1119};static struct _tuple6*Cyc__genarr_1132[
5]={& Cyc__gentuple_1127,& Cyc__gentuple_1128,& Cyc__gentuple_1129,& Cyc__gentuple_1130,&
Cyc__gentuple_1131};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1118={4,
sizeof(struct _tuple17),{(void*)((struct _tuple6**)Cyc__genarr_1132),(void*)((
struct _tuple6**)Cyc__genarr_1132 + 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1114;
struct _tuple18{unsigned int f1;struct Cyc_Absyn_Tvar*f2;};static struct _tuple6 Cyc__gentuple_1115={
offsetof(struct _tuple18,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1116={
offsetof(struct _tuple18,f2),(void*)& Cyc__genrep_185};static struct _tuple6*Cyc__genarr_1117[
2]={& Cyc__gentuple_1115,& Cyc__gentuple_1116};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1114={4,sizeof(struct _tuple18),{(void*)((struct _tuple6**)Cyc__genarr_1117),(
void*)((struct _tuple6**)Cyc__genarr_1117 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1087;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_info_t_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep;extern
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1094;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionInfo_rep;static char _tmp1E8[18]="UnknownTunionInfo";
static struct _dynforward_ptr Cyc__genname_1099=(struct _dynforward_ptr){_tmp1E8,
_tmp1E8 + 18};static char _tmp1E9[5]="name";static struct _tuple5 Cyc__gentuple_1095={
offsetof(struct Cyc_Absyn_UnknownTunionInfo,name),(struct _dynforward_ptr){_tmp1E9,
_tmp1E9 + 5},(void*)& Cyc__genrep_11};static char _tmp1EA[11]="is_xtunion";static
struct _tuple5 Cyc__gentuple_1096={offsetof(struct Cyc_Absyn_UnknownTunionInfo,is_xtunion),(
struct _dynforward_ptr){_tmp1EA,_tmp1EA + 11},(void*)& Cyc__genrep_10};static char
_tmp1EB[8]="is_flat";static struct _tuple5 Cyc__gentuple_1097={offsetof(struct Cyc_Absyn_UnknownTunionInfo,is_flat),(
struct _dynforward_ptr){_tmp1EB,_tmp1EB + 8},(void*)& Cyc__genrep_10};static struct
_tuple5*Cyc__genarr_1098[3]={& Cyc__gentuple_1095,& Cyc__gentuple_1096,& Cyc__gentuple_1097};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_UnknownTunionInfo_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_1099,sizeof(struct Cyc_Absyn_UnknownTunionInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1098),(void*)((struct _tuple5**)Cyc__genarr_1098
+ 3)}};struct _tuple19{unsigned int f1;struct Cyc_Absyn_UnknownTunionInfo f2;};
static struct _tuple6 Cyc__gentuple_1100={offsetof(struct _tuple19,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1101={offsetof(struct _tuple19,f2),(void*)& Cyc_struct_Absyn_UnknownTunionInfo_rep};
static struct _tuple6*Cyc__genarr_1102[2]={& Cyc__gentuple_1100,& Cyc__gentuple_1101};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1094={4,sizeof(struct _tuple19),{(
void*)((struct _tuple6**)Cyc__genarr_1102),(void*)((struct _tuple6**)Cyc__genarr_1102
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1089;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1090;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_325;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tuniondecl_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_326;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_327;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_308;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_309;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_310;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_311;static struct
_tuple6 Cyc__gentuple_312={offsetof(struct _tuple3,f1),(void*)& Cyc_Absyn_tqual_t_rep};
static struct _tuple6 Cyc__gentuple_313={offsetof(struct _tuple3,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_314[2]={& Cyc__gentuple_312,&
Cyc__gentuple_313};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_311={4,
sizeof(struct _tuple3),{(void*)((struct _tuple6**)Cyc__genarr_314),(void*)((struct
_tuple6**)Cyc__genarr_314 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_310={
1,1,(void*)((void*)& Cyc__genrep_311)};static char _tmp1F0[5]="List";static struct
_dynforward_ptr Cyc__genname_318=(struct _dynforward_ptr){_tmp1F0,_tmp1F0 + 5};
static char _tmp1F1[3]="hd";static struct _tuple5 Cyc__gentuple_315={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp1F1,_tmp1F1 + 3},(void*)& Cyc__genrep_310};
static char _tmp1F2[3]="tl";static struct _tuple5 Cyc__gentuple_316={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp1F2,_tmp1F2 + 3},(void*)& Cyc__genrep_309};
static struct _tuple5*Cyc__genarr_317[2]={& Cyc__gentuple_315,& Cyc__gentuple_316};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_318,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_317),(void*)((struct _tuple5**)Cyc__genarr_317 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_309={1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
static char _tmp1F5[12]="Tunionfield";static struct _dynforward_ptr Cyc__genname_324=(
struct _dynforward_ptr){_tmp1F5,_tmp1F5 + 12};static char _tmp1F6[5]="name";static
struct _tuple5 Cyc__gentuple_319={offsetof(struct Cyc_Absyn_Tunionfield,name),(
struct _dynforward_ptr){_tmp1F6,_tmp1F6 + 5},(void*)& Cyc__genrep_11};static char
_tmp1F7[5]="typs";static struct _tuple5 Cyc__gentuple_320={offsetof(struct Cyc_Absyn_Tunionfield,typs),(
struct _dynforward_ptr){_tmp1F7,_tmp1F7 + 5},(void*)& Cyc__genrep_309};static char
_tmp1F8[4]="loc";static struct _tuple5 Cyc__gentuple_321={offsetof(struct Cyc_Absyn_Tunionfield,loc),(
struct _dynforward_ptr){_tmp1F8,_tmp1F8 + 4},(void*)& Cyc__genrep_2};static char
_tmp1F9[3]="sc";static struct _tuple5 Cyc__gentuple_322={offsetof(struct Cyc_Absyn_Tunionfield,sc),(
struct _dynforward_ptr){_tmp1F9,_tmp1F9 + 3},(void*)& Cyc_Absyn_scope_t_rep};static
struct _tuple5*Cyc__genarr_323[4]={& Cyc__gentuple_319,& Cyc__gentuple_320,& Cyc__gentuple_321,&
Cyc__gentuple_322};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_324,sizeof(struct Cyc_Absyn_Tunionfield),{(
void*)((struct _tuple5**)Cyc__genarr_323),(void*)((struct _tuple5**)Cyc__genarr_323
+ 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_308={1,1,(void*)((void*)&
Cyc_struct_Absyn_Tunionfield_rep)};static char _tmp1FC[5]="List";static struct
_dynforward_ptr Cyc__genname_331=(struct _dynforward_ptr){_tmp1FC,_tmp1FC + 5};
static char _tmp1FD[3]="hd";static struct _tuple5 Cyc__gentuple_328={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp1FD,_tmp1FD + 3},(void*)& Cyc__genrep_308};
static char _tmp1FE[3]="tl";static struct _tuple5 Cyc__gentuple_329={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp1FE,_tmp1FE + 3},(void*)& Cyc__genrep_327};
static struct _tuple5*Cyc__genarr_330[2]={& Cyc__gentuple_328,& Cyc__gentuple_329};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_331,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_330),(void*)((struct _tuple5**)Cyc__genarr_330 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_327={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep)};
static char _tmp201[4]="Opt";static struct _dynforward_ptr Cyc__genname_334=(struct
_dynforward_ptr){_tmp201,_tmp201 + 4};static char _tmp202[2]="v";static struct
_tuple5 Cyc__gentuple_332={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp202,_tmp202 + 2},(void*)& Cyc__genrep_327};static struct _tuple5*Cyc__genarr_333[
1]={& Cyc__gentuple_332};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_334,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_333),(void*)((struct _tuple5**)Cyc__genarr_333 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_326={1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep)};
static char _tmp205[11]="Tuniondecl";static struct _dynforward_ptr Cyc__genname_347=(
struct _dynforward_ptr){_tmp205,_tmp205 + 11};static char _tmp206[3]="sc";static
struct _tuple5 Cyc__gentuple_340={offsetof(struct Cyc_Absyn_Tuniondecl,sc),(struct
_dynforward_ptr){_tmp206,_tmp206 + 3},(void*)& Cyc_Absyn_scope_t_rep};static char
_tmp207[5]="name";static struct _tuple5 Cyc__gentuple_341={offsetof(struct Cyc_Absyn_Tuniondecl,name),(
struct _dynforward_ptr){_tmp207,_tmp207 + 5},(void*)& Cyc__genrep_11};static char
_tmp208[4]="tvs";static struct _tuple5 Cyc__gentuple_342={offsetof(struct Cyc_Absyn_Tuniondecl,tvs),(
struct _dynforward_ptr){_tmp208,_tmp208 + 4},(void*)& Cyc__genrep_335};static char
_tmp209[7]="fields";static struct _tuple5 Cyc__gentuple_343={offsetof(struct Cyc_Absyn_Tuniondecl,fields),(
struct _dynforward_ptr){_tmp209,_tmp209 + 7},(void*)& Cyc__genrep_326};static char
_tmp20A[11]="is_xtunion";static struct _tuple5 Cyc__gentuple_344={offsetof(struct
Cyc_Absyn_Tuniondecl,is_xtunion),(struct _dynforward_ptr){_tmp20A,_tmp20A + 11},(
void*)& Cyc__genrep_10};static char _tmp20B[8]="is_flat";static struct _tuple5 Cyc__gentuple_345={
offsetof(struct Cyc_Absyn_Tuniondecl,is_flat),(struct _dynforward_ptr){_tmp20B,
_tmp20B + 8},(void*)& Cyc__genrep_10};static struct _tuple5*Cyc__genarr_346[6]={& Cyc__gentuple_340,&
Cyc__gentuple_341,& Cyc__gentuple_342,& Cyc__gentuple_343,& Cyc__gentuple_344,& Cyc__gentuple_345};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tuniondecl_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_347,sizeof(struct Cyc_Absyn_Tuniondecl),{(void*)((
struct _tuple5**)Cyc__genarr_346),(void*)((struct _tuple5**)Cyc__genarr_346 + 6)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_325={1,1,(void*)((void*)& Cyc_struct_Absyn_Tuniondecl_rep)};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1090={1,1,(void*)((void*)& Cyc__genrep_325)};
struct _tuple20{unsigned int f1;struct Cyc_Absyn_Tuniondecl**f2;};static struct
_tuple6 Cyc__gentuple_1091={offsetof(struct _tuple20,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1092={offsetof(struct _tuple20,f2),(void*)& Cyc__genrep_1090};
static struct _tuple6*Cyc__genarr_1093[2]={& Cyc__gentuple_1091,& Cyc__gentuple_1092};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1089={4,sizeof(struct _tuple20),{(
void*)((struct _tuple6**)Cyc__genarr_1093),(void*)((struct _tuple6**)Cyc__genarr_1093
+ 2)}};static struct _tuple7*Cyc__genarr_1088[0]={};static char _tmp210[14]="UnknownTunion";
static struct _tuple5 Cyc__gentuple_1103={0,(struct _dynforward_ptr){_tmp210,_tmp210
+ 14},(void*)& Cyc__genrep_1094};static char _tmp211[12]="KnownTunion";static struct
_tuple5 Cyc__gentuple_1104={1,(struct _dynforward_ptr){_tmp211,_tmp211 + 12},(void*)&
Cyc__genrep_1089};static struct _tuple5*Cyc__genarr_1105[2]={& Cyc__gentuple_1103,&
Cyc__gentuple_1104};static char _tmp213[12]="TunionInfoU";struct Cyc_Typerep_TUnion_struct
Cyc_tunion_Absyn_TunionInfoU_rep={5,(struct _dynforward_ptr){_tmp213,_tmp213 + 12},{(
void*)((struct _tuple7**)Cyc__genarr_1088),(void*)((struct _tuple7**)Cyc__genarr_1088
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_1105),(void*)((struct _tuple5**)Cyc__genarr_1105
+ 2)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_121;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_type_t46H2_rep;static char _tmp214[5]="List";static
struct _dynforward_ptr Cyc__genname_125=(struct _dynforward_ptr){_tmp214,_tmp214 + 5};
static char _tmp215[3]="hd";static struct _tuple5 Cyc__gentuple_122={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp215,_tmp215 + 3},(void*)((void*)&
Cyc_Absyn_type_t_rep)};static char _tmp216[3]="tl";static struct _tuple5 Cyc__gentuple_123={
offsetof(struct Cyc_List_List,tl),(struct _dynforward_ptr){_tmp216,_tmp216 + 3},(
void*)& Cyc__genrep_121};static struct _tuple5*Cyc__genarr_124[2]={& Cyc__gentuple_122,&
Cyc__gentuple_123};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_125,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_124),(void*)((struct _tuple5**)Cyc__genarr_124 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_121={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_type_t46H2_rep)};
static char _tmp219[11]="TunionInfo";static struct _dynforward_ptr Cyc__genname_1110=(
struct _dynforward_ptr){_tmp219,_tmp219 + 11};static char _tmp21A[12]="tunion_info";
static struct _tuple5 Cyc__gentuple_1106={offsetof(struct Cyc_Absyn_TunionInfo,tunion_info),(
struct _dynforward_ptr){_tmp21A,_tmp21A + 12},(void*)& Cyc_tunion_Absyn_TunionInfoU_rep};
static char _tmp21B[6]="targs";static struct _tuple5 Cyc__gentuple_1107={offsetof(
struct Cyc_Absyn_TunionInfo,targs),(struct _dynforward_ptr){_tmp21B,_tmp21B + 6},(
void*)& Cyc__genrep_121};static char _tmp21C[4]="rgn";static struct _tuple5 Cyc__gentuple_1108={
offsetof(struct Cyc_Absyn_TunionInfo,rgn),(struct _dynforward_ptr){_tmp21C,_tmp21C
+ 4},(void*)& Cyc__genrep_110};static struct _tuple5*Cyc__genarr_1109[3]={& Cyc__gentuple_1106,&
Cyc__gentuple_1107,& Cyc__gentuple_1108};struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_info_t_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_1110,sizeof(struct Cyc_Absyn_TunionInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1109),(void*)((struct _tuple5**)Cyc__genarr_1109
+ 3)}};struct _tuple21{unsigned int f1;struct Cyc_Absyn_TunionInfo f2;};static struct
_tuple6 Cyc__gentuple_1111={offsetof(struct _tuple21,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1112={offsetof(struct _tuple21,f2),(void*)& Cyc_Absyn_tunion_info_t_rep};
static struct _tuple6*Cyc__genarr_1113[2]={& Cyc__gentuple_1111,& Cyc__gentuple_1112};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1087={4,sizeof(struct _tuple21),{(
void*)((struct _tuple6**)Cyc__genarr_1113),(void*)((struct _tuple6**)Cyc__genarr_1113
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1061;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_field_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1068;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionFieldInfo_rep;static char _tmp21F[23]="UnknownTunionFieldInfo";
static struct _dynforward_ptr Cyc__genname_1073=(struct _dynforward_ptr){_tmp21F,
_tmp21F + 23};static char _tmp220[12]="tunion_name";static struct _tuple5 Cyc__gentuple_1069={
offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,tunion_name),(struct
_dynforward_ptr){_tmp220,_tmp220 + 12},(void*)& Cyc__genrep_11};static char _tmp221[
11]="field_name";static struct _tuple5 Cyc__gentuple_1070={offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,field_name),(
struct _dynforward_ptr){_tmp221,_tmp221 + 11},(void*)& Cyc__genrep_11};static char
_tmp222[11]="is_xtunion";static struct _tuple5 Cyc__gentuple_1071={offsetof(struct
Cyc_Absyn_UnknownTunionFieldInfo,is_xtunion),(struct _dynforward_ptr){_tmp222,
_tmp222 + 11},(void*)& Cyc__genrep_10};static struct _tuple5*Cyc__genarr_1072[3]={&
Cyc__gentuple_1069,& Cyc__gentuple_1070,& Cyc__gentuple_1071};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionFieldInfo_rep={3,(struct _dynforward_ptr*)& Cyc__genname_1073,
sizeof(struct Cyc_Absyn_UnknownTunionFieldInfo),{(void*)((struct _tuple5**)Cyc__genarr_1072),(
void*)((struct _tuple5**)Cyc__genarr_1072 + 3)}};struct _tuple22{unsigned int f1;
struct Cyc_Absyn_UnknownTunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1074={
offsetof(struct _tuple22,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1075={
offsetof(struct _tuple22,f2),(void*)& Cyc_struct_Absyn_UnknownTunionFieldInfo_rep};
static struct _tuple6*Cyc__genarr_1076[2]={& Cyc__gentuple_1074,& Cyc__gentuple_1075};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1068={4,sizeof(struct _tuple22),{(
void*)((struct _tuple6**)Cyc__genarr_1076),(void*)((struct _tuple6**)Cyc__genarr_1076
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1063;struct _tuple23{
unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*f3;};
static struct _tuple6 Cyc__gentuple_1064={offsetof(struct _tuple23,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1065={offsetof(struct _tuple23,f2),(void*)((void*)&
Cyc__genrep_325)};static struct _tuple6 Cyc__gentuple_1066={offsetof(struct _tuple23,f3),(
void*)& Cyc__genrep_308};static struct _tuple6*Cyc__genarr_1067[3]={& Cyc__gentuple_1064,&
Cyc__gentuple_1065,& Cyc__gentuple_1066};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1063={
4,sizeof(struct _tuple23),{(void*)((struct _tuple6**)Cyc__genarr_1067),(void*)((
struct _tuple6**)Cyc__genarr_1067 + 3)}};static struct _tuple7*Cyc__genarr_1062[0]={};
static char _tmp226[19]="UnknownTunionfield";static struct _tuple5 Cyc__gentuple_1077={
0,(struct _dynforward_ptr){_tmp226,_tmp226 + 19},(void*)& Cyc__genrep_1068};static
char _tmp227[17]="KnownTunionfield";static struct _tuple5 Cyc__gentuple_1078={1,(
struct _dynforward_ptr){_tmp227,_tmp227 + 17},(void*)& Cyc__genrep_1063};static
struct _tuple5*Cyc__genarr_1079[2]={& Cyc__gentuple_1077,& Cyc__gentuple_1078};
static char _tmp229[17]="TunionFieldInfoU";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep={
5,(struct _dynforward_ptr){_tmp229,_tmp229 + 17},{(void*)((struct _tuple7**)Cyc__genarr_1062),(
void*)((struct _tuple7**)Cyc__genarr_1062 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_1079),(
void*)((struct _tuple5**)Cyc__genarr_1079 + 2)}};static char _tmp22A[16]="TunionFieldInfo";
static struct _dynforward_ptr Cyc__genname_1083=(struct _dynforward_ptr){_tmp22A,
_tmp22A + 16};static char _tmp22B[11]="field_info";static struct _tuple5 Cyc__gentuple_1080={
offsetof(struct Cyc_Absyn_TunionFieldInfo,field_info),(struct _dynforward_ptr){
_tmp22B,_tmp22B + 11},(void*)& Cyc_tunion_Absyn_TunionFieldInfoU_rep};static char
_tmp22C[6]="targs";static struct _tuple5 Cyc__gentuple_1081={offsetof(struct Cyc_Absyn_TunionFieldInfo,targs),(
struct _dynforward_ptr){_tmp22C,_tmp22C + 6},(void*)& Cyc__genrep_121};static struct
_tuple5*Cyc__genarr_1082[2]={& Cyc__gentuple_1080,& Cyc__gentuple_1081};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_field_info_t_rep={3,(struct _dynforward_ptr*)& Cyc__genname_1083,
sizeof(struct Cyc_Absyn_TunionFieldInfo),{(void*)((struct _tuple5**)Cyc__genarr_1082),(
void*)((struct _tuple5**)Cyc__genarr_1082 + 2)}};struct _tuple24{unsigned int f1;
struct Cyc_Absyn_TunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1084={
offsetof(struct _tuple24,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1085={
offsetof(struct _tuple24,f2),(void*)& Cyc_Absyn_tunion_field_info_t_rep};static
struct _tuple6*Cyc__genarr_1086[2]={& Cyc__gentuple_1084,& Cyc__gentuple_1085};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1061={4,sizeof(struct _tuple24),{(
void*)((struct _tuple6**)Cyc__genarr_1086),(void*)((struct _tuple6**)Cyc__genarr_1086
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1035;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_ptr_info_t_rep;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_atts_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1000;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0bool2_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0bool2_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_85;struct _tuple25{unsigned int f1;
int f2;};static struct _tuple6 Cyc__gentuple_86={offsetof(struct _tuple25,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_87={offsetof(struct _tuple25,f2),(
void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_88[2]={& Cyc__gentuple_86,&
Cyc__gentuple_87};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_85={4,sizeof(
struct _tuple25),{(void*)((struct _tuple6**)Cyc__genarr_88),(void*)((struct _tuple6**)
Cyc__genarr_88 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1003;struct
_tuple26{unsigned int f1;struct Cyc_Absyn_Conref*f2;};static struct _tuple6 Cyc__gentuple_1004={
offsetof(struct _tuple26,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1005={
offsetof(struct _tuple26,f2),(void*)& Cyc__genrep_1000};static struct _tuple6*Cyc__genarr_1006[
2]={& Cyc__gentuple_1004,& Cyc__gentuple_1005};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1003={4,sizeof(struct _tuple26),{(void*)((struct _tuple6**)Cyc__genarr_1006),(
void*)((struct _tuple6**)Cyc__genarr_1006 + 2)}};static char _tmp231[10]="No_constr";
static struct _tuple7 Cyc__gentuple_1001={0,(struct _dynforward_ptr){_tmp231,_tmp231
+ 10}};static struct _tuple7*Cyc__genarr_1002[1]={& Cyc__gentuple_1001};static char
_tmp232[10]="Eq_constr";static struct _tuple5 Cyc__gentuple_1007={0,(struct
_dynforward_ptr){_tmp232,_tmp232 + 10},(void*)& Cyc__genrep_85};static char _tmp233[
15]="Forward_constr";static struct _tuple5 Cyc__gentuple_1008={1,(struct
_dynforward_ptr){_tmp233,_tmp233 + 15},(void*)& Cyc__genrep_1003};static struct
_tuple5*Cyc__genarr_1009[2]={& Cyc__gentuple_1007,& Cyc__gentuple_1008};static char
_tmp235[11]="Constraint";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0bool2_rep={
5,(struct _dynforward_ptr){_tmp235,_tmp235 + 11},{(void*)((struct _tuple7**)Cyc__genarr_1002),(
void*)((struct _tuple7**)Cyc__genarr_1002 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_1009),(
void*)((struct _tuple5**)Cyc__genarr_1009 + 2)}};static char _tmp236[7]="Conref";
static struct _dynforward_ptr Cyc__genname_1012=(struct _dynforward_ptr){_tmp236,
_tmp236 + 7};static char _tmp237[2]="v";static struct _tuple5 Cyc__gentuple_1010={
offsetof(struct Cyc_Absyn_Conref,v),(struct _dynforward_ptr){_tmp237,_tmp237 + 2},(
void*)& Cyc_tunion_Absyn_Constraint0bool2_rep};static struct _tuple5*Cyc__genarr_1011[
1]={& Cyc__gentuple_1010};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Conref0bool2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_1012,sizeof(struct Cyc_Absyn_Conref),{(
void*)((struct _tuple5**)Cyc__genarr_1011),(void*)((struct _tuple5**)Cyc__genarr_1011
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1000={1,1,(void*)((void*)&
Cyc_struct_Absyn_Conref0bool2_rep)};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1042;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep;
static char _tmp23A[7]="Conref";static struct _dynforward_ptr Cyc__genname_1045=(
struct _dynforward_ptr){_tmp23A,_tmp23A + 7};static char _tmp23B[2]="v";static struct
_tuple5 Cyc__gentuple_1043={offsetof(struct Cyc_Absyn_Conref,v),(struct
_dynforward_ptr){_tmp23B,_tmp23B + 2},(void*)& Cyc_tunion_Absyn_Constraint0bool2_rep};
static struct _tuple5*Cyc__genarr_1044[1]={& Cyc__gentuple_1043};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_1045,
sizeof(struct Cyc_Absyn_Conref),{(void*)((struct _tuple5**)Cyc__genarr_1044),(void*)((
struct _tuple5**)Cyc__genarr_1044 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1042={
1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep)};extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_1036;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_PtrLoc_rep;static char _tmp23E[7]="PtrLoc";static struct
_dynforward_ptr Cyc__genname_1041=(struct _dynforward_ptr){_tmp23E,_tmp23E + 7};
static char _tmp23F[8]="ptr_loc";static struct _tuple5 Cyc__gentuple_1037={offsetof(
struct Cyc_Absyn_PtrLoc,ptr_loc),(struct _dynforward_ptr){_tmp23F,_tmp23F + 8},(
void*)& Cyc__genrep_2};static char _tmp240[8]="rgn_loc";static struct _tuple5 Cyc__gentuple_1038={
offsetof(struct Cyc_Absyn_PtrLoc,rgn_loc),(struct _dynforward_ptr){_tmp240,_tmp240
+ 8},(void*)& Cyc__genrep_2};static char _tmp241[7]="zt_loc";static struct _tuple5 Cyc__gentuple_1039={
offsetof(struct Cyc_Absyn_PtrLoc,zt_loc),(struct _dynforward_ptr){_tmp241,_tmp241 + 
7},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_1040[3]={& Cyc__gentuple_1037,&
Cyc__gentuple_1038,& Cyc__gentuple_1039};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_PtrLoc_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_1041,sizeof(struct Cyc_Absyn_PtrLoc),{(
void*)((struct _tuple5**)Cyc__genarr_1040),(void*)((struct _tuple5**)Cyc__genarr_1040
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1036={1,1,(void*)((void*)&
Cyc_struct_Absyn_PtrLoc_rep)};static char _tmp244[8]="PtrAtts";static struct
_dynforward_ptr Cyc__genname_1052=(struct _dynforward_ptr){_tmp244,_tmp244 + 8};
static char _tmp245[4]="rgn";static struct _tuple5 Cyc__gentuple_1046={offsetof(
struct Cyc_Absyn_PtrAtts,rgn),(struct _dynforward_ptr){_tmp245,_tmp245 + 4},(void*)((
void*)& Cyc_Absyn_type_t_rep)};static char _tmp246[9]="nullable";static struct
_tuple5 Cyc__gentuple_1047={offsetof(struct Cyc_Absyn_PtrAtts,nullable),(struct
_dynforward_ptr){_tmp246,_tmp246 + 9},(void*)& Cyc__genrep_1000};static char _tmp247[
7]="bounds";static struct _tuple5 Cyc__gentuple_1048={offsetof(struct Cyc_Absyn_PtrAtts,bounds),(
struct _dynforward_ptr){_tmp247,_tmp247 + 7},(void*)& Cyc__genrep_1042};static char
_tmp248[10]="zero_term";static struct _tuple5 Cyc__gentuple_1049={offsetof(struct
Cyc_Absyn_PtrAtts,zero_term),(struct _dynforward_ptr){_tmp248,_tmp248 + 10},(void*)&
Cyc__genrep_1000};static char _tmp249[7]="ptrloc";static struct _tuple5 Cyc__gentuple_1050={
offsetof(struct Cyc_Absyn_PtrAtts,ptrloc),(struct _dynforward_ptr){_tmp249,_tmp249
+ 7},(void*)& Cyc__genrep_1036};static struct _tuple5*Cyc__genarr_1051[5]={& Cyc__gentuple_1046,&
Cyc__gentuple_1047,& Cyc__gentuple_1048,& Cyc__gentuple_1049,& Cyc__gentuple_1050};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_atts_t_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_1052,sizeof(struct Cyc_Absyn_PtrAtts),{(void*)((
struct _tuple5**)Cyc__genarr_1051),(void*)((struct _tuple5**)Cyc__genarr_1051 + 5)}};
static char _tmp24B[8]="PtrInfo";static struct _dynforward_ptr Cyc__genname_1057=(
struct _dynforward_ptr){_tmp24B,_tmp24B + 8};static char _tmp24C[8]="elt_typ";static
struct _tuple5 Cyc__gentuple_1053={offsetof(struct Cyc_Absyn_PtrInfo,elt_typ),(
struct _dynforward_ptr){_tmp24C,_tmp24C + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp24D[7]="elt_tq";static struct _tuple5 Cyc__gentuple_1054={offsetof(
struct Cyc_Absyn_PtrInfo,elt_tq),(struct _dynforward_ptr){_tmp24D,_tmp24D + 7},(
void*)& Cyc_Absyn_tqual_t_rep};static char _tmp24E[9]="ptr_atts";static struct
_tuple5 Cyc__gentuple_1055={offsetof(struct Cyc_Absyn_PtrInfo,ptr_atts),(struct
_dynforward_ptr){_tmp24E,_tmp24E + 9},(void*)& Cyc_Absyn_ptr_atts_t_rep};static
struct _tuple5*Cyc__genarr_1056[3]={& Cyc__gentuple_1053,& Cyc__gentuple_1054,& Cyc__gentuple_1055};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_info_t_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_1057,sizeof(struct Cyc_Absyn_PtrInfo),{(void*)((
struct _tuple5**)Cyc__genarr_1056),(void*)((struct _tuple5**)Cyc__genarr_1056 + 3)}};
struct _tuple27{unsigned int f1;struct Cyc_Absyn_PtrInfo f2;};static struct _tuple6 Cyc__gentuple_1058={
offsetof(struct _tuple27,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1059={
offsetof(struct _tuple27,f2),(void*)& Cyc_Absyn_ptr_info_t_rep};static struct
_tuple6*Cyc__genarr_1060[2]={& Cyc__gentuple_1058,& Cyc__gentuple_1059};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1035={4,sizeof(struct _tuple27),{(void*)((
struct _tuple6**)Cyc__genarr_1060),(void*)((struct _tuple6**)Cyc__genarr_1060 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1023;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_sign_t_rep;static char _tmp251[7]="Signed";static struct _tuple7 Cyc__gentuple_298={
0,(struct _dynforward_ptr){_tmp251,_tmp251 + 7}};static char _tmp252[9]="Unsigned";
static struct _tuple7 Cyc__gentuple_299={1,(struct _dynforward_ptr){_tmp252,_tmp252 + 
9}};static char _tmp253[5]="None";static struct _tuple7 Cyc__gentuple_300={2,(struct
_dynforward_ptr){_tmp253,_tmp253 + 5}};static struct _tuple7*Cyc__genarr_301[3]={&
Cyc__gentuple_298,& Cyc__gentuple_299,& Cyc__gentuple_300};static struct _tuple5*Cyc__genarr_302[
0]={};static char _tmp255[5]="Sign";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_sign_t_rep={
5,(struct _dynforward_ptr){_tmp255,_tmp255 + 5},{(void*)((struct _tuple7**)Cyc__genarr_301),(
void*)((struct _tuple7**)Cyc__genarr_301 + 3)},{(void*)((struct _tuple5**)Cyc__genarr_302),(
void*)((struct _tuple5**)Cyc__genarr_302 + 0)}};extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_size_of_t_rep;static char _tmp256[8]="Char_sz";static struct _tuple7 Cyc__gentuple_1024={
0,(struct _dynforward_ptr){_tmp256,_tmp256 + 8}};static char _tmp257[9]="Short_sz";
static struct _tuple7 Cyc__gentuple_1025={1,(struct _dynforward_ptr){_tmp257,_tmp257
+ 9}};static char _tmp258[7]="Int_sz";static struct _tuple7 Cyc__gentuple_1026={2,(
struct _dynforward_ptr){_tmp258,_tmp258 + 7}};static char _tmp259[8]="Long_sz";
static struct _tuple7 Cyc__gentuple_1027={3,(struct _dynforward_ptr){_tmp259,_tmp259
+ 8}};static char _tmp25A[12]="LongLong_sz";static struct _tuple7 Cyc__gentuple_1028={
4,(struct _dynforward_ptr){_tmp25A,_tmp25A + 12}};static struct _tuple7*Cyc__genarr_1029[
5]={& Cyc__gentuple_1024,& Cyc__gentuple_1025,& Cyc__gentuple_1026,& Cyc__gentuple_1027,&
Cyc__gentuple_1028};static struct _tuple5*Cyc__genarr_1030[0]={};static char _tmp25C[
8]="Size_of";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep={5,(struct
_dynforward_ptr){_tmp25C,_tmp25C + 8},{(void*)((struct _tuple7**)Cyc__genarr_1029),(
void*)((struct _tuple7**)Cyc__genarr_1029 + 5)},{(void*)((struct _tuple5**)Cyc__genarr_1030),(
void*)((struct _tuple5**)Cyc__genarr_1030 + 0)}};struct _tuple28{unsigned int f1;
void*f2;void*f3;};static struct _tuple6 Cyc__gentuple_1031={offsetof(struct _tuple28,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1032={offsetof(struct
_tuple28,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_1033={
offsetof(struct _tuple28,f3),(void*)& Cyc_Absyn_size_of_t_rep};static struct _tuple6*
Cyc__genarr_1034[3]={& Cyc__gentuple_1031,& Cyc__gentuple_1032,& Cyc__gentuple_1033};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1023={4,sizeof(struct _tuple28),{(
void*)((struct _tuple6**)Cyc__genarr_1034),(void*)((struct _tuple6**)Cyc__genarr_1034
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_999;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_array_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_146;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Exp_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_exp_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_874;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_cnst_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_889;struct _tuple29{unsigned int f1;void*f2;char f3;};static struct
_tuple6 Cyc__gentuple_890={offsetof(struct _tuple29,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_891={offsetof(struct _tuple29,f2),(void*)& Cyc_Absyn_sign_t_rep};
static struct _tuple6 Cyc__gentuple_892={offsetof(struct _tuple29,f3),(void*)((void*)&
Cyc__genrep_15)};static struct _tuple6*Cyc__genarr_893[3]={& Cyc__gentuple_890,& Cyc__gentuple_891,&
Cyc__gentuple_892};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_889={4,
sizeof(struct _tuple29),{(void*)((struct _tuple6**)Cyc__genarr_893),(void*)((
struct _tuple6**)Cyc__genarr_893 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_883;
static struct Cyc_Typerep_Int_struct Cyc__genrep_884={0,1,16};struct _tuple30{
unsigned int f1;void*f2;short f3;};static struct _tuple6 Cyc__gentuple_885={offsetof(
struct _tuple30,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_886={
offsetof(struct _tuple30,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_887={
offsetof(struct _tuple30,f3),(void*)& Cyc__genrep_884};static struct _tuple6*Cyc__genarr_888[
3]={& Cyc__gentuple_885,& Cyc__gentuple_886,& Cyc__gentuple_887};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_883={4,sizeof(struct _tuple30),{(void*)((struct _tuple6**)Cyc__genarr_888),(
void*)((struct _tuple6**)Cyc__genarr_888 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_297;struct _tuple31{unsigned int f1;void*f2;int f3;};static struct
_tuple6 Cyc__gentuple_303={offsetof(struct _tuple31,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_304={offsetof(struct _tuple31,f2),(void*)& Cyc_Absyn_sign_t_rep};
static struct _tuple6 Cyc__gentuple_305={offsetof(struct _tuple31,f3),(void*)& Cyc__genrep_10};
static struct _tuple6*Cyc__genarr_306[3]={& Cyc__gentuple_303,& Cyc__gentuple_304,&
Cyc__gentuple_305};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_297={4,
sizeof(struct _tuple31),{(void*)((struct _tuple6**)Cyc__genarr_306),(void*)((
struct _tuple6**)Cyc__genarr_306 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_877;
static struct Cyc_Typerep_Int_struct Cyc__genrep_878={0,1,64};struct _tuple32{
unsigned int f1;void*f2;long long f3;};static struct _tuple6 Cyc__gentuple_879={
offsetof(struct _tuple32,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_880={
offsetof(struct _tuple32,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_881={
offsetof(struct _tuple32,f3),(void*)& Cyc__genrep_878};static struct _tuple6*Cyc__genarr_882[
3]={& Cyc__gentuple_879,& Cyc__gentuple_880,& Cyc__gentuple_881};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_877={4,sizeof(struct _tuple32),{(void*)((struct _tuple6**)Cyc__genarr_882),(
void*)((struct _tuple6**)Cyc__genarr_882 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_81;static struct _tuple6 Cyc__gentuple_82={offsetof(struct _tuple7,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_83={offsetof(struct
_tuple7,f2),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*Cyc__genarr_84[
2]={& Cyc__gentuple_82,& Cyc__gentuple_83};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_81={
4,sizeof(struct _tuple7),{(void*)((struct _tuple6**)Cyc__genarr_84),(void*)((
struct _tuple6**)Cyc__genarr_84 + 2)}};static char _tmp265[7]="Null_c";static struct
_tuple7 Cyc__gentuple_875={0,(struct _dynforward_ptr){_tmp265,_tmp265 + 7}};static
struct _tuple7*Cyc__genarr_876[1]={& Cyc__gentuple_875};static char _tmp266[7]="Char_c";
static struct _tuple5 Cyc__gentuple_894={0,(struct _dynforward_ptr){_tmp266,_tmp266 + 
7},(void*)& Cyc__genrep_889};static char _tmp267[8]="Short_c";static struct _tuple5
Cyc__gentuple_895={1,(struct _dynforward_ptr){_tmp267,_tmp267 + 8},(void*)& Cyc__genrep_883};
static char _tmp268[6]="Int_c";static struct _tuple5 Cyc__gentuple_896={2,(struct
_dynforward_ptr){_tmp268,_tmp268 + 6},(void*)& Cyc__genrep_297};static char _tmp269[
11]="LongLong_c";static struct _tuple5 Cyc__gentuple_897={3,(struct _dynforward_ptr){
_tmp269,_tmp269 + 11},(void*)& Cyc__genrep_877};static char _tmp26A[8]="Float_c";
static struct _tuple5 Cyc__gentuple_898={4,(struct _dynforward_ptr){_tmp26A,_tmp26A + 
8},(void*)& Cyc__genrep_81};static char _tmp26B[9]="String_c";static struct _tuple5
Cyc__gentuple_899={5,(struct _dynforward_ptr){_tmp26B,_tmp26B + 9},(void*)& Cyc__genrep_81};
static struct _tuple5*Cyc__genarr_900[6]={& Cyc__gentuple_894,& Cyc__gentuple_895,&
Cyc__gentuple_896,& Cyc__gentuple_897,& Cyc__gentuple_898,& Cyc__gentuple_899};
static char _tmp26D[5]="Cnst";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_cnst_t_rep={
5,(struct _dynforward_ptr){_tmp26D,_tmp26D + 5},{(void*)((struct _tuple7**)Cyc__genarr_876),(
void*)((struct _tuple7**)Cyc__genarr_876 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_900),(
void*)((struct _tuple5**)Cyc__genarr_900 + 6)}};struct _tuple33{unsigned int f1;
union Cyc_Absyn_Cnst_union f2;};static struct _tuple6 Cyc__gentuple_901={offsetof(
struct _tuple33,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_902={
offsetof(struct _tuple33,f2),(void*)& Cyc_Absyn_cnst_t_rep};static struct _tuple6*
Cyc__genarr_903[2]={& Cyc__gentuple_901,& Cyc__gentuple_902};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_874={4,sizeof(struct _tuple33),{(void*)((struct _tuple6**)Cyc__genarr_903),(
void*)((struct _tuple6**)Cyc__genarr_903 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_822;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_binding_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_829;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_830;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Fndecl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_833;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_834;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_835;struct _tuple34{struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;
void*f3;};static struct _tuple6 Cyc__gentuple_836={offsetof(struct _tuple34,f1),(
void*)& Cyc__genrep_13};static struct _tuple6 Cyc__gentuple_837={offsetof(struct
_tuple34,f2),(void*)& Cyc_Absyn_tqual_t_rep};static struct _tuple6 Cyc__gentuple_838={
offsetof(struct _tuple34,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_839[3]={& Cyc__gentuple_836,& Cyc__gentuple_837,& Cyc__gentuple_838};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_835={4,sizeof(struct _tuple34),{(
void*)((struct _tuple6**)Cyc__genarr_839),(void*)((struct _tuple6**)Cyc__genarr_839
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_834={1,1,(void*)((void*)&
Cyc__genrep_835)};static char _tmp271[5]="List";static struct _dynforward_ptr Cyc__genname_843=(
struct _dynforward_ptr){_tmp271,_tmp271 + 5};static char _tmp272[3]="hd";static
struct _tuple5 Cyc__gentuple_840={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp272,_tmp272 + 3},(void*)& Cyc__genrep_834};static char _tmp273[
3]="tl";static struct _tuple5 Cyc__gentuple_841={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp273,_tmp273 + 3},(void*)& Cyc__genrep_833};static struct
_tuple5*Cyc__genarr_842[2]={& Cyc__gentuple_840,& Cyc__gentuple_841};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep={3,(
struct _dynforward_ptr*)& Cyc__genname_843,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_842),(void*)((struct _tuple5**)Cyc__genarr_842 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_833={1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_832;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_682;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_var_t2_rep;static
char _tmp276[4]="Opt";static struct _dynforward_ptr Cyc__genname_685=(struct
_dynforward_ptr){_tmp276,_tmp276 + 4};static char _tmp277[2]="v";static struct
_tuple5 Cyc__gentuple_683={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp277,_tmp277 + 2},(void*)& Cyc__genrep_13};static struct _tuple5*Cyc__genarr_684[
1]={& Cyc__gentuple_683};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_var_t2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_685,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_684),(void*)((struct _tuple5**)Cyc__genarr_684 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_682={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_var_t2_rep)};
static char _tmp27A[11]="VarargInfo";static struct _dynforward_ptr Cyc__genname_752=(
struct _dynforward_ptr){_tmp27A,_tmp27A + 11};static char _tmp27B[5]="name";static
struct _tuple5 Cyc__gentuple_747={offsetof(struct Cyc_Absyn_VarargInfo,name),(
struct _dynforward_ptr){_tmp27B,_tmp27B + 5},(void*)& Cyc__genrep_682};static char
_tmp27C[3]="tq";static struct _tuple5 Cyc__gentuple_748={offsetof(struct Cyc_Absyn_VarargInfo,tq),(
struct _dynforward_ptr){_tmp27C,_tmp27C + 3},(void*)& Cyc_Absyn_tqual_t_rep};static
char _tmp27D[5]="type";static struct _tuple5 Cyc__gentuple_749={offsetof(struct Cyc_Absyn_VarargInfo,type),(
struct _dynforward_ptr){_tmp27D,_tmp27D + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp27E[7]="inject";static struct _tuple5 Cyc__gentuple_750={offsetof(
struct Cyc_Absyn_VarargInfo,inject),(struct _dynforward_ptr){_tmp27E,_tmp27E + 7},(
void*)& Cyc__genrep_10};static struct _tuple5*Cyc__genarr_751[4]={& Cyc__gentuple_747,&
Cyc__gentuple_748,& Cyc__gentuple_749,& Cyc__gentuple_750};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep={3,(struct _dynforward_ptr*)& Cyc__genname_752,sizeof(
struct Cyc_Absyn_VarargInfo),{(void*)((struct _tuple5**)Cyc__genarr_751),(void*)((
struct _tuple5**)Cyc__genarr_751 + 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_832={
1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_396;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_397;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_398;static struct _tuple6 Cyc__gentuple_399={offsetof(struct _tuple9,f1),(
void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_400={
offsetof(struct _tuple9,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_401[2]={& Cyc__gentuple_399,& Cyc__gentuple_400};static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_398={4,sizeof(struct _tuple9),{(void*)((
struct _tuple6**)Cyc__genarr_401),(void*)((struct _tuple6**)Cyc__genarr_401 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_397={1,1,(void*)((void*)& Cyc__genrep_398)};
static char _tmp283[5]="List";static struct _dynforward_ptr Cyc__genname_405=(struct
_dynforward_ptr){_tmp283,_tmp283 + 5};static char _tmp284[3]="hd";static struct
_tuple5 Cyc__gentuple_402={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp284,_tmp284 + 3},(void*)& Cyc__genrep_397};static char _tmp285[
3]="tl";static struct _tuple5 Cyc__gentuple_403={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp285,_tmp285 + 3},(void*)& Cyc__genrep_396};static struct
_tuple5*Cyc__genarr_404[2]={& Cyc__gentuple_402,& Cyc__gentuple_403};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_405,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_404),(void*)((struct _tuple5**)Cyc__genarr_404 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_396={1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_150;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_230;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_223;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_223={1,1,(void*)((
void*)& Cyc_struct_Absyn_Exp_rep)};struct _tuple35{unsigned int f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple6 Cyc__gentuple_231={offsetof(struct _tuple35,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_232={offsetof(struct _tuple35,f2),(
void*)& Cyc__genrep_223};static struct _tuple6*Cyc__genarr_233[2]={& Cyc__gentuple_231,&
Cyc__gentuple_232};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_230={4,
sizeof(struct _tuple35),{(void*)((struct _tuple6**)Cyc__genarr_233),(void*)((
struct _tuple6**)Cyc__genarr_233 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_566;
struct _tuple36{unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};
static struct _tuple6 Cyc__gentuple_567={offsetof(struct _tuple36,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_568={offsetof(struct _tuple36,f2),(void*)& Cyc__genrep_150};
static struct _tuple6 Cyc__gentuple_569={offsetof(struct _tuple36,f3),(void*)& Cyc__genrep_150};
static struct _tuple6*Cyc__genarr_570[3]={& Cyc__gentuple_567,& Cyc__gentuple_568,&
Cyc__gentuple_569};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_566={4,
sizeof(struct _tuple36),{(void*)((struct _tuple6**)Cyc__genarr_570),(void*)((
struct _tuple6**)Cyc__genarr_570 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_562;
static struct _tuple6 Cyc__gentuple_563={offsetof(struct _tuple35,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_564={offsetof(struct _tuple35,f2),(void*)& Cyc__genrep_146};
static struct _tuple6*Cyc__genarr_565[2]={& Cyc__gentuple_563,& Cyc__gentuple_564};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_562={4,sizeof(struct _tuple35),{(
void*)((struct _tuple6**)Cyc__genarr_565),(void*)((struct _tuple6**)Cyc__genarr_565
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_556;struct _tuple37{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Stmt*f3;struct Cyc_Absyn_Stmt*
f4;};static struct _tuple6 Cyc__gentuple_557={offsetof(struct _tuple37,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_558={offsetof(struct _tuple37,f2),(
void*)& Cyc__genrep_223};static struct _tuple6 Cyc__gentuple_559={offsetof(struct
_tuple37,f3),(void*)& Cyc__genrep_150};static struct _tuple6 Cyc__gentuple_560={
offsetof(struct _tuple37,f4),(void*)& Cyc__genrep_150};static struct _tuple6*Cyc__genarr_561[
4]={& Cyc__gentuple_557,& Cyc__gentuple_558,& Cyc__gentuple_559,& Cyc__gentuple_560};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_556={4,sizeof(struct _tuple37),{(
void*)((struct _tuple6**)Cyc__genarr_561),(void*)((struct _tuple6**)Cyc__genarr_561
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_551;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_500;static struct _tuple6 Cyc__gentuple_501={offsetof(struct _tuple2,f1),(
void*)& Cyc__genrep_223};static struct _tuple6 Cyc__gentuple_502={offsetof(struct
_tuple2,f2),(void*)& Cyc__genrep_150};static struct _tuple6*Cyc__genarr_503[2]={&
Cyc__gentuple_501,& Cyc__gentuple_502};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_500={
4,sizeof(struct _tuple2),{(void*)((struct _tuple6**)Cyc__genarr_503),(void*)((
struct _tuple6**)Cyc__genarr_503 + 2)}};struct _tuple38{unsigned int f1;struct
_tuple2 f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_552={
offsetof(struct _tuple38,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_553={
offsetof(struct _tuple38,f2),(void*)& Cyc__genrep_500};static struct _tuple6 Cyc__gentuple_554={
offsetof(struct _tuple38,f3),(void*)& Cyc__genrep_150};static struct _tuple6*Cyc__genarr_555[
3]={& Cyc__gentuple_552,& Cyc__gentuple_553,& Cyc__gentuple_554};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_551={4,sizeof(struct _tuple38),{(void*)((struct _tuple6**)Cyc__genarr_555),(
void*)((struct _tuple6**)Cyc__genarr_555 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_547;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_542;static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_542={1,1,(void*)((void*)& Cyc_struct_Absyn_Stmt_rep)};
struct _tuple39{unsigned int f1;struct Cyc_Absyn_Stmt*f2;};static struct _tuple6 Cyc__gentuple_548={
offsetof(struct _tuple39,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_549={
offsetof(struct _tuple39,f2),(void*)& Cyc__genrep_542};static struct _tuple6*Cyc__genarr_550[
2]={& Cyc__gentuple_548,& Cyc__gentuple_549};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_547={4,sizeof(struct _tuple39),{(void*)((struct _tuple6**)Cyc__genarr_550),(
void*)((struct _tuple6**)Cyc__genarr_550 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_541;struct _tuple40{unsigned int f1;struct _dynforward_ptr*f2;struct Cyc_Absyn_Stmt*
f3;};static struct _tuple6 Cyc__gentuple_543={offsetof(struct _tuple40,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_544={offsetof(struct _tuple40,f2),(
void*)& Cyc__genrep_13};static struct _tuple6 Cyc__gentuple_545={offsetof(struct
_tuple40,f3),(void*)& Cyc__genrep_542};static struct _tuple6*Cyc__genarr_546[3]={&
Cyc__gentuple_543,& Cyc__gentuple_544,& Cyc__gentuple_545};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_541={4,sizeof(struct _tuple40),{(void*)((struct _tuple6**)Cyc__genarr_546),(
void*)((struct _tuple6**)Cyc__genarr_546 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_534;struct _tuple41{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct
_tuple2 f3;struct _tuple2 f4;struct Cyc_Absyn_Stmt*f5;};static struct _tuple6 Cyc__gentuple_535={
offsetof(struct _tuple41,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_536={
offsetof(struct _tuple41,f2),(void*)& Cyc__genrep_223};static struct _tuple6 Cyc__gentuple_537={
offsetof(struct _tuple41,f3),(void*)& Cyc__genrep_500};static struct _tuple6 Cyc__gentuple_538={
offsetof(struct _tuple41,f4),(void*)& Cyc__genrep_500};static struct _tuple6 Cyc__gentuple_539={
offsetof(struct _tuple41,f5),(void*)& Cyc__genrep_150};static struct _tuple6*Cyc__genarr_540[
5]={& Cyc__gentuple_535,& Cyc__gentuple_536,& Cyc__gentuple_537,& Cyc__gentuple_538,&
Cyc__gentuple_539};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_534={4,
sizeof(struct _tuple41),{(void*)((struct _tuple6**)Cyc__genarr_540),(void*)((
struct _tuple6**)Cyc__genarr_540 + 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_529;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_243;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_244;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Switch_clause_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_254;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Pat_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_pat_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_458;struct _tuple42{unsigned int
f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Pat*f3;};static struct _tuple6 Cyc__gentuple_459={
offsetof(struct _tuple42,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_460={
offsetof(struct _tuple42,f2),(void*)& Cyc__genrep_159};static struct _tuple6 Cyc__gentuple_461={
offsetof(struct _tuple42,f3),(void*)& Cyc__genrep_254};static struct _tuple6*Cyc__genarr_462[
3]={& Cyc__gentuple_459,& Cyc__gentuple_460,& Cyc__gentuple_461};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_458={4,sizeof(struct _tuple42),{(void*)((struct _tuple6**)Cyc__genarr_462),(
void*)((struct _tuple6**)Cyc__genarr_462 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_453;struct _tuple43{unsigned int f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*
f3;};static struct _tuple6 Cyc__gentuple_454={offsetof(struct _tuple43,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_455={offsetof(struct _tuple43,f2),(
void*)& Cyc__genrep_185};static struct _tuple6 Cyc__gentuple_456={offsetof(struct
_tuple43,f3),(void*)& Cyc__genrep_159};static struct _tuple6*Cyc__genarr_457[3]={&
Cyc__gentuple_454,& Cyc__gentuple_455,& Cyc__gentuple_456};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_453={4,sizeof(struct _tuple43),{(void*)((struct _tuple6**)Cyc__genarr_457),(
void*)((struct _tuple6**)Cyc__genarr_457 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_448;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_259;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep;static
char _tmp295[5]="List";static struct _dynforward_ptr Cyc__genname_263=(struct
_dynforward_ptr){_tmp295,_tmp295 + 5};static char _tmp296[3]="hd";static struct
_tuple5 Cyc__gentuple_260={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp296,_tmp296 + 3},(void*)& Cyc__genrep_254};static char _tmp297[
3]="tl";static struct _tuple5 Cyc__gentuple_261={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp297,_tmp297 + 3},(void*)& Cyc__genrep_259};static struct
_tuple5*Cyc__genarr_262[2]={& Cyc__gentuple_260,& Cyc__gentuple_261};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_pat_t46H2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_263,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_262),(void*)((
struct _tuple5**)Cyc__genarr_262 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_259={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_pat_t46H2_rep)};struct _tuple44{
unsigned int f1;struct Cyc_List_List*f2;int f3;};static struct _tuple6 Cyc__gentuple_449={
offsetof(struct _tuple44,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_450={
offsetof(struct _tuple44,f2),(void*)& Cyc__genrep_259};static struct _tuple6 Cyc__gentuple_451={
offsetof(struct _tuple44,f3),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_452[
3]={& Cyc__gentuple_449,& Cyc__gentuple_450,& Cyc__gentuple_451};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_448={4,sizeof(struct _tuple44),{(void*)((struct _tuple6**)Cyc__genarr_452),(
void*)((struct _tuple6**)Cyc__genarr_452 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_444;struct _tuple45{unsigned int f1;struct Cyc_Absyn_Pat*f2;};static
struct _tuple6 Cyc__gentuple_445={offsetof(struct _tuple45,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_446={offsetof(struct _tuple45,f2),(void*)& Cyc__genrep_254};
static struct _tuple6*Cyc__genarr_447[2]={& Cyc__gentuple_445,& Cyc__gentuple_446};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_444={4,sizeof(struct _tuple45),{(
void*)((struct _tuple6**)Cyc__genarr_447),(void*)((struct _tuple6**)Cyc__genarr_447
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_354;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_aggr_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_AggrInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_426;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_aggr_kind_t_rep;static char _tmp29C[8]="StructA";static struct _tuple7 Cyc__gentuple_411={
0,(struct _dynforward_ptr){_tmp29C,_tmp29C + 8}};static char _tmp29D[7]="UnionA";
static struct _tuple7 Cyc__gentuple_412={1,(struct _dynforward_ptr){_tmp29D,_tmp29D + 
7}};static struct _tuple7*Cyc__genarr_413[2]={& Cyc__gentuple_411,& Cyc__gentuple_412};
static struct _tuple5*Cyc__genarr_414[0]={};static char _tmp29F[9]="AggrKind";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_aggr_kind_t_rep={5,(struct _dynforward_ptr){
_tmp29F,_tmp29F + 9},{(void*)((struct _tuple7**)Cyc__genarr_413),(void*)((struct
_tuple7**)Cyc__genarr_413 + 2)},{(void*)((struct _tuple5**)Cyc__genarr_414),(void*)((
struct _tuple5**)Cyc__genarr_414 + 0)}};struct _tuple46{unsigned int f1;void*f2;
struct _tuple0*f3;};static struct _tuple6 Cyc__gentuple_427={offsetof(struct _tuple46,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_428={offsetof(struct
_tuple46,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};static struct _tuple6 Cyc__gentuple_429={
offsetof(struct _tuple46,f3),(void*)& Cyc__genrep_11};static struct _tuple6*Cyc__genarr_430[
3]={& Cyc__gentuple_427,& Cyc__gentuple_428,& Cyc__gentuple_429};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_426={4,sizeof(struct _tuple46),{(void*)((struct _tuple6**)Cyc__genarr_430),(
void*)((struct _tuple6**)Cyc__genarr_430 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_379;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_380;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_381;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Aggrdecl_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_382;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_AggrdeclImpl_rep;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_383;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_384;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_62;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_attribute_t46H2_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_attribute_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_89;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep;static char
_tmp2A1[10]="Printf_ft";static struct _tuple7 Cyc__gentuple_90={0,(struct
_dynforward_ptr){_tmp2A1,_tmp2A1 + 10}};static char _tmp2A2[9]="Scanf_ft";static
struct _tuple7 Cyc__gentuple_91={1,(struct _dynforward_ptr){_tmp2A2,_tmp2A2 + 9}};
static struct _tuple7*Cyc__genarr_92[2]={& Cyc__gentuple_90,& Cyc__gentuple_91};
static struct _tuple5*Cyc__genarr_93[0]={};static char _tmp2A4[12]="Format_Type";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep={5,(struct
_dynforward_ptr){_tmp2A4,_tmp2A4 + 12},{(void*)((struct _tuple7**)Cyc__genarr_92),(
void*)((struct _tuple7**)Cyc__genarr_92 + 2)},{(void*)((struct _tuple5**)Cyc__genarr_93),(
void*)((struct _tuple5**)Cyc__genarr_93 + 0)}};struct _tuple47{unsigned int f1;void*
f2;int f3;int f4;};static struct _tuple6 Cyc__gentuple_94={offsetof(struct _tuple47,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_95={offsetof(struct
_tuple47,f2),(void*)& Cyc_tunion_Absyn_Format_Type_rep};static struct _tuple6 Cyc__gentuple_96={
offsetof(struct _tuple47,f3),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_97={
offsetof(struct _tuple47,f4),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_98[
4]={& Cyc__gentuple_94,& Cyc__gentuple_95,& Cyc__gentuple_96,& Cyc__gentuple_97};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_89={4,sizeof(struct _tuple47),{(
void*)((struct _tuple6**)Cyc__genarr_98),(void*)((struct _tuple6**)Cyc__genarr_98 + 
4)}};static char _tmp2A6[12]="Stdcall_att";static struct _tuple7 Cyc__gentuple_63={0,(
struct _dynforward_ptr){_tmp2A6,_tmp2A6 + 12}};static char _tmp2A7[10]="Cdecl_att";
static struct _tuple7 Cyc__gentuple_64={1,(struct _dynforward_ptr){_tmp2A7,_tmp2A7 + 
10}};static char _tmp2A8[13]="Fastcall_att";static struct _tuple7 Cyc__gentuple_65={2,(
struct _dynforward_ptr){_tmp2A8,_tmp2A8 + 13}};static char _tmp2A9[13]="Noreturn_att";
static struct _tuple7 Cyc__gentuple_66={3,(struct _dynforward_ptr){_tmp2A9,_tmp2A9 + 
13}};static char _tmp2AA[10]="Const_att";static struct _tuple7 Cyc__gentuple_67={4,(
struct _dynforward_ptr){_tmp2AA,_tmp2AA + 10}};static char _tmp2AB[11]="Packed_att";
static struct _tuple7 Cyc__gentuple_68={5,(struct _dynforward_ptr){_tmp2AB,_tmp2AB + 
11}};static char _tmp2AC[13]="Nocommon_att";static struct _tuple7 Cyc__gentuple_69={6,(
struct _dynforward_ptr){_tmp2AC,_tmp2AC + 13}};static char _tmp2AD[11]="Shared_att";
static struct _tuple7 Cyc__gentuple_70={7,(struct _dynforward_ptr){_tmp2AD,_tmp2AD + 
11}};static char _tmp2AE[11]="Unused_att";static struct _tuple7 Cyc__gentuple_71={8,(
struct _dynforward_ptr){_tmp2AE,_tmp2AE + 11}};static char _tmp2AF[9]="Weak_att";
static struct _tuple7 Cyc__gentuple_72={9,(struct _dynforward_ptr){_tmp2AF,_tmp2AF + 
9}};static char _tmp2B0[14]="Dllimport_att";static struct _tuple7 Cyc__gentuple_73={
10,(struct _dynforward_ptr){_tmp2B0,_tmp2B0 + 14}};static char _tmp2B1[14]="Dllexport_att";
static struct _tuple7 Cyc__gentuple_74={11,(struct _dynforward_ptr){_tmp2B1,_tmp2B1 + 
14}};static char _tmp2B2[27]="No_instrument_function_att";static struct _tuple7 Cyc__gentuple_75={
12,(struct _dynforward_ptr){_tmp2B2,_tmp2B2 + 27}};static char _tmp2B3[16]="Constructor_att";
static struct _tuple7 Cyc__gentuple_76={13,(struct _dynforward_ptr){_tmp2B3,_tmp2B3 + 
16}};static char _tmp2B4[15]="Destructor_att";static struct _tuple7 Cyc__gentuple_77={
14,(struct _dynforward_ptr){_tmp2B4,_tmp2B4 + 15}};static char _tmp2B5[26]="No_check_memory_usage_att";
static struct _tuple7 Cyc__gentuple_78={15,(struct _dynforward_ptr){_tmp2B5,_tmp2B5 + 
26}};static char _tmp2B6[9]="Pure_att";static struct _tuple7 Cyc__gentuple_79={16,(
struct _dynforward_ptr){_tmp2B6,_tmp2B6 + 9}};static struct _tuple7*Cyc__genarr_80[
17]={& Cyc__gentuple_63,& Cyc__gentuple_64,& Cyc__gentuple_65,& Cyc__gentuple_66,&
Cyc__gentuple_67,& Cyc__gentuple_68,& Cyc__gentuple_69,& Cyc__gentuple_70,& Cyc__gentuple_71,&
Cyc__gentuple_72,& Cyc__gentuple_73,& Cyc__gentuple_74,& Cyc__gentuple_75,& Cyc__gentuple_76,&
Cyc__gentuple_77,& Cyc__gentuple_78,& Cyc__gentuple_79};static char _tmp2B7[12]="Regparm_att";
static struct _tuple5 Cyc__gentuple_99={0,(struct _dynforward_ptr){_tmp2B7,_tmp2B7 + 
12},(void*)& Cyc__genrep_85};static char _tmp2B8[12]="Aligned_att";static struct
_tuple5 Cyc__gentuple_100={1,(struct _dynforward_ptr){_tmp2B8,_tmp2B8 + 12},(void*)&
Cyc__genrep_85};static char _tmp2B9[12]="Section_att";static struct _tuple5 Cyc__gentuple_101={
2,(struct _dynforward_ptr){_tmp2B9,_tmp2B9 + 12},(void*)& Cyc__genrep_81};static
char _tmp2BA[11]="Format_att";static struct _tuple5 Cyc__gentuple_102={3,(struct
_dynforward_ptr){_tmp2BA,_tmp2BA + 11},(void*)& Cyc__genrep_89};static char _tmp2BB[
16]="Initializes_att";static struct _tuple5 Cyc__gentuple_103={4,(struct
_dynforward_ptr){_tmp2BB,_tmp2BB + 16},(void*)& Cyc__genrep_85};static char _tmp2BC[
9]="Mode_att";static struct _tuple5 Cyc__gentuple_104={5,(struct _dynforward_ptr){
_tmp2BC,_tmp2BC + 9},(void*)& Cyc__genrep_81};static struct _tuple5*Cyc__genarr_105[
6]={& Cyc__gentuple_99,& Cyc__gentuple_100,& Cyc__gentuple_101,& Cyc__gentuple_102,&
Cyc__gentuple_103,& Cyc__gentuple_104};static char _tmp2BE[10]="Attribute";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_attribute_t_rep={5,(struct _dynforward_ptr){
_tmp2BE,_tmp2BE + 10},{(void*)((struct _tuple7**)Cyc__genarr_80),(void*)((struct
_tuple7**)Cyc__genarr_80 + 17)},{(void*)((struct _tuple5**)Cyc__genarr_105),(void*)((
struct _tuple5**)Cyc__genarr_105 + 6)}};static char _tmp2BF[5]="List";static struct
_dynforward_ptr Cyc__genname_109=(struct _dynforward_ptr){_tmp2BF,_tmp2BF + 5};
static char _tmp2C0[3]="hd";static struct _tuple5 Cyc__gentuple_106={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp2C0,_tmp2C0 + 3},(void*)& Cyc_Absyn_attribute_t_rep};
static char _tmp2C1[3]="tl";static struct _tuple5 Cyc__gentuple_107={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp2C1,_tmp2C1 + 3},(void*)& Cyc__genrep_62};
static struct _tuple5*Cyc__genarr_108[2]={& Cyc__gentuple_106,& Cyc__gentuple_107};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_attribute_t46H2_rep={3,(
struct _dynforward_ptr*)& Cyc__genname_109,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_108),(void*)((struct _tuple5**)Cyc__genarr_108 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_62={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_attribute_t46H2_rep)};
static char _tmp2C4[10]="Aggrfield";static struct _dynforward_ptr Cyc__genname_391=(
struct _dynforward_ptr){_tmp2C4,_tmp2C4 + 10};static char _tmp2C5[5]="name";static
struct _tuple5 Cyc__gentuple_385={offsetof(struct Cyc_Absyn_Aggrfield,name),(struct
_dynforward_ptr){_tmp2C5,_tmp2C5 + 5},(void*)& Cyc__genrep_13};static char _tmp2C6[3]="tq";
static struct _tuple5 Cyc__gentuple_386={offsetof(struct Cyc_Absyn_Aggrfield,tq),(
struct _dynforward_ptr){_tmp2C6,_tmp2C6 + 3},(void*)& Cyc_Absyn_tqual_t_rep};static
char _tmp2C7[5]="type";static struct _tuple5 Cyc__gentuple_387={offsetof(struct Cyc_Absyn_Aggrfield,type),(
struct _dynforward_ptr){_tmp2C7,_tmp2C7 + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp2C8[6]="width";static struct _tuple5 Cyc__gentuple_388={offsetof(
struct Cyc_Absyn_Aggrfield,width),(struct _dynforward_ptr){_tmp2C8,_tmp2C8 + 6},(
void*)& Cyc__genrep_146};static char _tmp2C9[11]="attributes";static struct _tuple5
Cyc__gentuple_389={offsetof(struct Cyc_Absyn_Aggrfield,attributes),(struct
_dynforward_ptr){_tmp2C9,_tmp2C9 + 11},(void*)& Cyc__genrep_62};static struct
_tuple5*Cyc__genarr_390[5]={& Cyc__gentuple_385,& Cyc__gentuple_386,& Cyc__gentuple_387,&
Cyc__gentuple_388,& Cyc__gentuple_389};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_391,sizeof(struct Cyc_Absyn_Aggrfield),{(
void*)((struct _tuple5**)Cyc__genarr_390),(void*)((struct _tuple5**)Cyc__genarr_390
+ 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_384={1,1,(void*)((void*)&
Cyc_struct_Absyn_Aggrfield_rep)};static char _tmp2CC[5]="List";static struct
_dynforward_ptr Cyc__genname_395=(struct _dynforward_ptr){_tmp2CC,_tmp2CC + 5};
static char _tmp2CD[3]="hd";static struct _tuple5 Cyc__gentuple_392={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp2CD,_tmp2CD + 3},(void*)& Cyc__genrep_384};
static char _tmp2CE[3]="tl";static struct _tuple5 Cyc__gentuple_393={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp2CE,_tmp2CE + 3},(void*)& Cyc__genrep_383};
static struct _tuple5*Cyc__genarr_394[2]={& Cyc__gentuple_392,& Cyc__gentuple_393};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep={3,(
struct _dynforward_ptr*)& Cyc__genname_395,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_394),(void*)((struct _tuple5**)Cyc__genarr_394 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_383={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep)};
static char _tmp2D1[13]="AggrdeclImpl";static struct _dynforward_ptr Cyc__genname_410=(
struct _dynforward_ptr){_tmp2D1,_tmp2D1 + 13};static char _tmp2D2[11]="exist_vars";
static struct _tuple5 Cyc__gentuple_406={offsetof(struct Cyc_Absyn_AggrdeclImpl,exist_vars),(
struct _dynforward_ptr){_tmp2D2,_tmp2D2 + 11},(void*)& Cyc__genrep_335};static char
_tmp2D3[7]="rgn_po";static struct _tuple5 Cyc__gentuple_407={offsetof(struct Cyc_Absyn_AggrdeclImpl,rgn_po),(
struct _dynforward_ptr){_tmp2D3,_tmp2D3 + 7},(void*)& Cyc__genrep_396};static char
_tmp2D4[7]="fields";static struct _tuple5 Cyc__gentuple_408={offsetof(struct Cyc_Absyn_AggrdeclImpl,fields),(
struct _dynforward_ptr){_tmp2D4,_tmp2D4 + 7},(void*)& Cyc__genrep_383};static struct
_tuple5*Cyc__genarr_409[3]={& Cyc__gentuple_406,& Cyc__gentuple_407,& Cyc__gentuple_408};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_AggrdeclImpl_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_410,sizeof(struct Cyc_Absyn_AggrdeclImpl),{(void*)((
struct _tuple5**)Cyc__genarr_409),(void*)((struct _tuple5**)Cyc__genarr_409 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_382={1,1,(void*)((void*)& Cyc_struct_Absyn_AggrdeclImpl_rep)};
static char _tmp2D7[9]="Aggrdecl";static struct _dynforward_ptr Cyc__genname_422=(
struct _dynforward_ptr){_tmp2D7,_tmp2D7 + 9};static char _tmp2D8[5]="kind";static
struct _tuple5 Cyc__gentuple_415={offsetof(struct Cyc_Absyn_Aggrdecl,kind),(struct
_dynforward_ptr){_tmp2D8,_tmp2D8 + 5},(void*)& Cyc_Absyn_aggr_kind_t_rep};static
char _tmp2D9[3]="sc";static struct _tuple5 Cyc__gentuple_416={offsetof(struct Cyc_Absyn_Aggrdecl,sc),(
struct _dynforward_ptr){_tmp2D9,_tmp2D9 + 3},(void*)& Cyc_Absyn_scope_t_rep};static
char _tmp2DA[5]="name";static struct _tuple5 Cyc__gentuple_417={offsetof(struct Cyc_Absyn_Aggrdecl,name),(
struct _dynforward_ptr){_tmp2DA,_tmp2DA + 5},(void*)& Cyc__genrep_11};static char
_tmp2DB[4]="tvs";static struct _tuple5 Cyc__gentuple_418={offsetof(struct Cyc_Absyn_Aggrdecl,tvs),(
struct _dynforward_ptr){_tmp2DB,_tmp2DB + 4},(void*)& Cyc__genrep_335};static char
_tmp2DC[5]="impl";static struct _tuple5 Cyc__gentuple_419={offsetof(struct Cyc_Absyn_Aggrdecl,impl),(
struct _dynforward_ptr){_tmp2DC,_tmp2DC + 5},(void*)& Cyc__genrep_382};static char
_tmp2DD[11]="attributes";static struct _tuple5 Cyc__gentuple_420={offsetof(struct
Cyc_Absyn_Aggrdecl,attributes),(struct _dynforward_ptr){_tmp2DD,_tmp2DD + 11},(
void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_421[6]={& Cyc__gentuple_415,&
Cyc__gentuple_416,& Cyc__gentuple_417,& Cyc__gentuple_418,& Cyc__gentuple_419,& Cyc__gentuple_420};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_422,sizeof(struct Cyc_Absyn_Aggrdecl),{(void*)((
struct _tuple5**)Cyc__genarr_421),(void*)((struct _tuple5**)Cyc__genarr_421 + 6)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_381={1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrdecl_rep)};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_380={1,1,(void*)((void*)& Cyc__genrep_381)};
struct _tuple48{unsigned int f1;struct Cyc_Absyn_Aggrdecl**f2;};static struct _tuple6
Cyc__gentuple_423={offsetof(struct _tuple48,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_424={offsetof(struct _tuple48,f2),(void*)& Cyc__genrep_380};
static struct _tuple6*Cyc__genarr_425[2]={& Cyc__gentuple_423,& Cyc__gentuple_424};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_379={4,sizeof(struct _tuple48),{(
void*)((struct _tuple6**)Cyc__genarr_425),(void*)((struct _tuple6**)Cyc__genarr_425
+ 2)}};static struct _tuple7*Cyc__genarr_378[0]={};static char _tmp2E2[12]="UnknownAggr";
static struct _tuple5 Cyc__gentuple_431={0,(struct _dynforward_ptr){_tmp2E2,_tmp2E2 + 
12},(void*)& Cyc__genrep_426};static char _tmp2E3[10]="KnownAggr";static struct
_tuple5 Cyc__gentuple_432={1,(struct _dynforward_ptr){_tmp2E3,_tmp2E3 + 10},(void*)&
Cyc__genrep_379};static struct _tuple5*Cyc__genarr_433[2]={& Cyc__gentuple_431,& Cyc__gentuple_432};
static char _tmp2E5[10]="AggrInfoU";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_AggrInfoU_rep={
5,(struct _dynforward_ptr){_tmp2E5,_tmp2E5 + 10},{(void*)((struct _tuple7**)Cyc__genarr_378),(
void*)((struct _tuple7**)Cyc__genarr_378 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_433),(
void*)((struct _tuple5**)Cyc__genarr_433 + 2)}};static char _tmp2E6[9]="AggrInfo";
static struct _dynforward_ptr Cyc__genname_437=(struct _dynforward_ptr){_tmp2E6,
_tmp2E6 + 9};static char _tmp2E7[10]="aggr_info";static struct _tuple5 Cyc__gentuple_434={
offsetof(struct Cyc_Absyn_AggrInfo,aggr_info),(struct _dynforward_ptr){_tmp2E7,
_tmp2E7 + 10},(void*)& Cyc_tunion_Absyn_AggrInfoU_rep};static char _tmp2E8[6]="targs";
static struct _tuple5 Cyc__gentuple_435={offsetof(struct Cyc_Absyn_AggrInfo,targs),(
struct _dynforward_ptr){_tmp2E8,_tmp2E8 + 6},(void*)& Cyc__genrep_121};static struct
_tuple5*Cyc__genarr_436[2]={& Cyc__gentuple_434,& Cyc__gentuple_435};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_aggr_info_t_rep={3,(struct _dynforward_ptr*)& Cyc__genname_437,sizeof(
struct Cyc_Absyn_AggrInfo),{(void*)((struct _tuple5**)Cyc__genarr_436),(void*)((
struct _tuple5**)Cyc__genarr_436 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_355;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_356;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_357;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_358;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_360;struct _tuple49{unsigned int f1;struct _dynforward_ptr*f2;};static
struct _tuple6 Cyc__gentuple_361={offsetof(struct _tuple49,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_362={offsetof(struct _tuple49,f2),(void*)& Cyc__genrep_13};
static struct _tuple6*Cyc__genarr_363[2]={& Cyc__gentuple_361,& Cyc__gentuple_362};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_360={4,sizeof(struct _tuple49),{(
void*)((struct _tuple6**)Cyc__genarr_363),(void*)((struct _tuple6**)Cyc__genarr_363
+ 2)}};static struct _tuple7*Cyc__genarr_359[0]={};static char _tmp2EB[13]="ArrayElement";
static struct _tuple5 Cyc__gentuple_364={0,(struct _dynforward_ptr){_tmp2EB,_tmp2EB + 
13},(void*)& Cyc__genrep_230};static char _tmp2EC[10]="FieldName";static struct
_tuple5 Cyc__gentuple_365={1,(struct _dynforward_ptr){_tmp2EC,_tmp2EC + 10},(void*)&
Cyc__genrep_360};static struct _tuple5*Cyc__genarr_366[2]={& Cyc__gentuple_364,& Cyc__gentuple_365};
static char _tmp2EE[11]="Designator";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep={
5,(struct _dynforward_ptr){_tmp2EE,_tmp2EE + 11},{(void*)((struct _tuple7**)Cyc__genarr_359),(
void*)((struct _tuple7**)Cyc__genarr_359 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_366),(
void*)((struct _tuple5**)Cyc__genarr_366 + 2)}};static char _tmp2EF[5]="List";static
struct _dynforward_ptr Cyc__genname_370=(struct _dynforward_ptr){_tmp2EF,_tmp2EF + 5};
static char _tmp2F0[3]="hd";static struct _tuple5 Cyc__gentuple_367={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp2F0,_tmp2F0 + 3},(void*)& Cyc_Absyn_designator_t_rep};
static char _tmp2F1[3]="tl";static struct _tuple5 Cyc__gentuple_368={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp2F1,_tmp2F1 + 3},(void*)& Cyc__genrep_358};
static struct _tuple5*Cyc__genarr_369[2]={& Cyc__gentuple_367,& Cyc__gentuple_368};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_370,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_369),(void*)((struct _tuple5**)Cyc__genarr_369 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_358={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_designator_t46H2_rep)};
struct _tuple50{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct
_tuple6 Cyc__gentuple_371={offsetof(struct _tuple50,f1),(void*)& Cyc__genrep_358};
static struct _tuple6 Cyc__gentuple_372={offsetof(struct _tuple50,f2),(void*)& Cyc__genrep_254};
static struct _tuple6*Cyc__genarr_373[2]={& Cyc__gentuple_371,& Cyc__gentuple_372};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_357={4,sizeof(struct _tuple50),{(
void*)((struct _tuple6**)Cyc__genarr_373),(void*)((struct _tuple6**)Cyc__genarr_373
+ 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_356={1,1,(void*)((void*)&
Cyc__genrep_357)};static char _tmp2F6[5]="List";static struct _dynforward_ptr Cyc__genname_377=(
struct _dynforward_ptr){_tmp2F6,_tmp2F6 + 5};static char _tmp2F7[3]="hd";static
struct _tuple5 Cyc__gentuple_374={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp2F7,_tmp2F7 + 3},(void*)& Cyc__genrep_356};static char _tmp2F8[
3]="tl";static struct _tuple5 Cyc__gentuple_375={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp2F8,_tmp2F8 + 3},(void*)& Cyc__genrep_355};static struct
_tuple5*Cyc__genarr_376[2]={& Cyc__gentuple_374,& Cyc__gentuple_375};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_377,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_376),(void*)((struct _tuple5**)Cyc__genarr_376 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_355={1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep)};
struct _tuple51{unsigned int f1;struct Cyc_Absyn_AggrInfo f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;int f5;};static struct _tuple6 Cyc__gentuple_438={offsetof(
struct _tuple51,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_439={
offsetof(struct _tuple51,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};static struct
_tuple6 Cyc__gentuple_440={offsetof(struct _tuple51,f3),(void*)& Cyc__genrep_335};
static struct _tuple6 Cyc__gentuple_441={offsetof(struct _tuple51,f4),(void*)& Cyc__genrep_355};
static struct _tuple6 Cyc__gentuple_442={offsetof(struct _tuple51,f5),(void*)& Cyc__genrep_10};
static struct _tuple6*Cyc__genarr_443[5]={& Cyc__gentuple_438,& Cyc__gentuple_439,&
Cyc__gentuple_440,& Cyc__gentuple_441,& Cyc__gentuple_442};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_354={4,sizeof(struct _tuple51),{(void*)((struct _tuple6**)Cyc__genarr_443),(
void*)((struct _tuple6**)Cyc__genarr_443 + 5)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_307;struct _tuple52{unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;
struct Cyc_Absyn_Tunionfield*f3;struct Cyc_List_List*f4;int f5;};static struct
_tuple6 Cyc__gentuple_348={offsetof(struct _tuple52,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_349={offsetof(struct _tuple52,f2),(void*)((void*)&
Cyc__genrep_325)};static struct _tuple6 Cyc__gentuple_350={offsetof(struct _tuple52,f3),(
void*)& Cyc__genrep_308};static struct _tuple6 Cyc__gentuple_351={offsetof(struct
_tuple52,f4),(void*)& Cyc__genrep_259};static struct _tuple6 Cyc__gentuple_352={
offsetof(struct _tuple52,f5),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_353[
5]={& Cyc__gentuple_348,& Cyc__gentuple_349,& Cyc__gentuple_350,& Cyc__gentuple_351,&
Cyc__gentuple_352};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_307={4,
sizeof(struct _tuple52),{(void*)((struct _tuple6**)Cyc__genarr_353),(void*)((
struct _tuple6**)Cyc__genarr_353 + 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_293;
struct _tuple53{unsigned int f1;char f2;};static struct _tuple6 Cyc__gentuple_294={
offsetof(struct _tuple53,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_295={
offsetof(struct _tuple53,f2),(void*)((void*)& Cyc__genrep_15)};static struct _tuple6*
Cyc__genarr_296[2]={& Cyc__gentuple_294,& Cyc__gentuple_295};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_293={4,sizeof(struct _tuple53),{(void*)((struct _tuple6**)Cyc__genarr_296),(
void*)((struct _tuple6**)Cyc__genarr_296 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_278;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_279;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumdecl_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_280;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_144;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_enumfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_145;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumfield_rep;
static char _tmp2FE[10]="Enumfield";static struct _dynforward_ptr Cyc__genname_954=(
struct _dynforward_ptr){_tmp2FE,_tmp2FE + 10};static char _tmp2FF[5]="name";static
struct _tuple5 Cyc__gentuple_950={offsetof(struct Cyc_Absyn_Enumfield,name),(struct
_dynforward_ptr){_tmp2FF,_tmp2FF + 5},(void*)& Cyc__genrep_11};static char _tmp300[4]="tag";
static struct _tuple5 Cyc__gentuple_951={offsetof(struct Cyc_Absyn_Enumfield,tag),(
struct _dynforward_ptr){_tmp300,_tmp300 + 4},(void*)& Cyc__genrep_146};static char
_tmp301[4]="loc";static struct _tuple5 Cyc__gentuple_952={offsetof(struct Cyc_Absyn_Enumfield,loc),(
struct _dynforward_ptr){_tmp301,_tmp301 + 4},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_953[3]={& Cyc__gentuple_950,& Cyc__gentuple_951,& Cyc__gentuple_952};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumfield_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_954,sizeof(struct Cyc_Absyn_Enumfield),{(void*)((
struct _tuple5**)Cyc__genarr_953),(void*)((struct _tuple5**)Cyc__genarr_953 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_145={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};
static char _tmp304[5]="List";static struct _dynforward_ptr Cyc__genname_958=(struct
_dynforward_ptr){_tmp304,_tmp304 + 5};static char _tmp305[3]="hd";static struct
_tuple5 Cyc__gentuple_955={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp305,_tmp305 + 3},(void*)& Cyc__genrep_145};static char _tmp306[
3]="tl";static struct _tuple5 Cyc__gentuple_956={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp306,_tmp306 + 3},(void*)& Cyc__genrep_144};static struct
_tuple5*Cyc__genarr_957[2]={& Cyc__gentuple_955,& Cyc__gentuple_956};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_enumfield_t46H2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_958,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_957),(void*)((
struct _tuple5**)Cyc__genarr_957 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_144={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_enumfield_t46H2_rep)};static char
_tmp309[4]="Opt";static struct _dynforward_ptr Cyc__genname_283=(struct
_dynforward_ptr){_tmp309,_tmp309 + 4};static char _tmp30A[2]="v";static struct
_tuple5 Cyc__gentuple_281={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp30A,_tmp30A + 2},(void*)& Cyc__genrep_144};static struct _tuple5*Cyc__genarr_282[
1]={& Cyc__gentuple_281};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_283,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_282),(void*)((struct _tuple5**)Cyc__genarr_282 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_280={1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep)};
static char _tmp30D[9]="Enumdecl";static struct _dynforward_ptr Cyc__genname_288=(
struct _dynforward_ptr){_tmp30D,_tmp30D + 9};static char _tmp30E[3]="sc";static
struct _tuple5 Cyc__gentuple_284={offsetof(struct Cyc_Absyn_Enumdecl,sc),(struct
_dynforward_ptr){_tmp30E,_tmp30E + 3},(void*)& Cyc_Absyn_scope_t_rep};static char
_tmp30F[5]="name";static struct _tuple5 Cyc__gentuple_285={offsetof(struct Cyc_Absyn_Enumdecl,name),(
struct _dynforward_ptr){_tmp30F,_tmp30F + 5},(void*)& Cyc__genrep_11};static char
_tmp310[7]="fields";static struct _tuple5 Cyc__gentuple_286={offsetof(struct Cyc_Absyn_Enumdecl,fields),(
struct _dynforward_ptr){_tmp310,_tmp310 + 7},(void*)& Cyc__genrep_280};static struct
_tuple5*Cyc__genarr_287[3]={& Cyc__gentuple_284,& Cyc__gentuple_285,& Cyc__gentuple_286};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumdecl_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_288,sizeof(struct Cyc_Absyn_Enumdecl),{(void*)((
struct _tuple5**)Cyc__genarr_287),(void*)((struct _tuple5**)Cyc__genarr_287 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_279={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};
struct _tuple54{unsigned int f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*
f3;};static struct _tuple6 Cyc__gentuple_289={offsetof(struct _tuple54,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_290={offsetof(struct _tuple54,f2),(
void*)& Cyc__genrep_279};static struct _tuple6 Cyc__gentuple_291={offsetof(struct
_tuple54,f3),(void*)& Cyc__genrep_145};static struct _tuple6*Cyc__genarr_292[3]={&
Cyc__gentuple_289,& Cyc__gentuple_290,& Cyc__gentuple_291};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_278={4,sizeof(struct _tuple54),{(void*)((struct _tuple6**)Cyc__genarr_292),(
void*)((struct _tuple6**)Cyc__genarr_292 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_273;struct _tuple55{unsigned int f1;void*f2;struct Cyc_Absyn_Enumfield*
f3;};static struct _tuple6 Cyc__gentuple_274={offsetof(struct _tuple55,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_275={offsetof(struct _tuple55,f2),(
void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_276={
offsetof(struct _tuple55,f3),(void*)& Cyc__genrep_145};static struct _tuple6*Cyc__genarr_277[
3]={& Cyc__gentuple_274,& Cyc__gentuple_275,& Cyc__gentuple_276};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_273={4,sizeof(struct _tuple55),{(void*)((struct _tuple6**)Cyc__genarr_277),(
void*)((struct _tuple6**)Cyc__genarr_277 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_269;struct _tuple56{unsigned int f1;struct _tuple0*f2;};static struct
_tuple6 Cyc__gentuple_270={offsetof(struct _tuple56,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_271={offsetof(struct _tuple56,f2),(void*)& Cyc__genrep_11};
static struct _tuple6*Cyc__genarr_272[2]={& Cyc__gentuple_270,& Cyc__gentuple_271};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_269={4,sizeof(struct _tuple56),{(
void*)((struct _tuple6**)Cyc__genarr_272),(void*)((struct _tuple6**)Cyc__genarr_272
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_258;struct _tuple57{
unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;int f4;};static struct
_tuple6 Cyc__gentuple_264={offsetof(struct _tuple57,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_265={offsetof(struct _tuple57,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_266={offsetof(struct _tuple57,f3),(void*)& Cyc__genrep_259};
static struct _tuple6 Cyc__gentuple_267={offsetof(struct _tuple57,f4),(void*)& Cyc__genrep_10};
static struct _tuple6*Cyc__genarr_268[4]={& Cyc__gentuple_264,& Cyc__gentuple_265,&
Cyc__gentuple_266,& Cyc__gentuple_267};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_258={
4,sizeof(struct _tuple57),{(void*)((struct _tuple6**)Cyc__genarr_268),(void*)((
struct _tuple6**)Cyc__genarr_268 + 4)}};static char _tmp317[7]="Wild_p";static struct
_tuple7 Cyc__gentuple_255={0,(struct _dynforward_ptr){_tmp317,_tmp317 + 7}};static
char _tmp318[7]="Null_p";static struct _tuple7 Cyc__gentuple_256={1,(struct
_dynforward_ptr){_tmp318,_tmp318 + 7}};static struct _tuple7*Cyc__genarr_257[2]={&
Cyc__gentuple_255,& Cyc__gentuple_256};static char _tmp319[6]="Var_p";static struct
_tuple5 Cyc__gentuple_463={0,(struct _dynforward_ptr){_tmp319,_tmp319 + 6},(void*)&
Cyc__genrep_458};static char _tmp31A[12]="Reference_p";static struct _tuple5 Cyc__gentuple_464={
1,(struct _dynforward_ptr){_tmp31A,_tmp31A + 12},(void*)& Cyc__genrep_458};static
char _tmp31B[9]="TagInt_p";static struct _tuple5 Cyc__gentuple_465={2,(struct
_dynforward_ptr){_tmp31B,_tmp31B + 9},(void*)& Cyc__genrep_453};static char _tmp31C[
8]="Tuple_p";static struct _tuple5 Cyc__gentuple_466={3,(struct _dynforward_ptr){
_tmp31C,_tmp31C + 8},(void*)& Cyc__genrep_448};static char _tmp31D[10]="Pointer_p";
static struct _tuple5 Cyc__gentuple_467={4,(struct _dynforward_ptr){_tmp31D,_tmp31D + 
10},(void*)& Cyc__genrep_444};static char _tmp31E[7]="Aggr_p";static struct _tuple5
Cyc__gentuple_468={5,(struct _dynforward_ptr){_tmp31E,_tmp31E + 7},(void*)& Cyc__genrep_354};
static char _tmp31F[9]="Tunion_p";static struct _tuple5 Cyc__gentuple_469={6,(struct
_dynforward_ptr){_tmp31F,_tmp31F + 9},(void*)& Cyc__genrep_307};static char _tmp320[
6]="Int_p";static struct _tuple5 Cyc__gentuple_470={7,(struct _dynforward_ptr){
_tmp320,_tmp320 + 6},(void*)& Cyc__genrep_297};static char _tmp321[7]="Char_p";
static struct _tuple5 Cyc__gentuple_471={8,(struct _dynforward_ptr){_tmp321,_tmp321 + 
7},(void*)& Cyc__genrep_293};static char _tmp322[8]="Float_p";static struct _tuple5
Cyc__gentuple_472={9,(struct _dynforward_ptr){_tmp322,_tmp322 + 8},(void*)& Cyc__genrep_81};
static char _tmp323[7]="Enum_p";static struct _tuple5 Cyc__gentuple_473={10,(struct
_dynforward_ptr){_tmp323,_tmp323 + 7},(void*)& Cyc__genrep_278};static char _tmp324[
11]="AnonEnum_p";static struct _tuple5 Cyc__gentuple_474={11,(struct _dynforward_ptr){
_tmp324,_tmp324 + 11},(void*)& Cyc__genrep_273};static char _tmp325[12]="UnknownId_p";
static struct _tuple5 Cyc__gentuple_475={12,(struct _dynforward_ptr){_tmp325,_tmp325
+ 12},(void*)& Cyc__genrep_269};static char _tmp326[14]="UnknownCall_p";static
struct _tuple5 Cyc__gentuple_476={13,(struct _dynforward_ptr){_tmp326,_tmp326 + 14},(
void*)& Cyc__genrep_258};static char _tmp327[6]="Exp_p";static struct _tuple5 Cyc__gentuple_477={
14,(struct _dynforward_ptr){_tmp327,_tmp327 + 6},(void*)& Cyc__genrep_230};static
struct _tuple5*Cyc__genarr_478[15]={& Cyc__gentuple_463,& Cyc__gentuple_464,& Cyc__gentuple_465,&
Cyc__gentuple_466,& Cyc__gentuple_467,& Cyc__gentuple_468,& Cyc__gentuple_469,& Cyc__gentuple_470,&
Cyc__gentuple_471,& Cyc__gentuple_472,& Cyc__gentuple_473,& Cyc__gentuple_474,& Cyc__gentuple_475,&
Cyc__gentuple_476,& Cyc__gentuple_477};static char _tmp329[8]="Raw_pat";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep={5,(struct _dynforward_ptr){_tmp329,_tmp329 + 8},{(void*)((
struct _tuple7**)Cyc__genarr_257),(void*)((struct _tuple7**)Cyc__genarr_257 + 2)},{(
void*)((struct _tuple5**)Cyc__genarr_478),(void*)((struct _tuple5**)Cyc__genarr_478
+ 15)}};static char _tmp32A[4]="Pat";static struct _dynforward_ptr Cyc__genname_483=(
struct _dynforward_ptr){_tmp32A,_tmp32A + 4};static char _tmp32B[2]="r";static struct
_tuple5 Cyc__gentuple_479={offsetof(struct Cyc_Absyn_Pat,r),(struct _dynforward_ptr){
_tmp32B,_tmp32B + 2},(void*)& Cyc_Absyn_raw_pat_t_rep};static char _tmp32C[5]="topt";
static struct _tuple5 Cyc__gentuple_480={offsetof(struct Cyc_Absyn_Pat,topt),(struct
_dynforward_ptr){_tmp32C,_tmp32C + 5},(void*)& Cyc__genrep_110};static char _tmp32D[
4]="loc";static struct _tuple5 Cyc__gentuple_481={offsetof(struct Cyc_Absyn_Pat,loc),(
struct _dynforward_ptr){_tmp32D,_tmp32D + 4},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_482[3]={& Cyc__gentuple_479,& Cyc__gentuple_480,& Cyc__gentuple_481};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_483,sizeof(struct Cyc_Absyn_Pat),{(void*)((struct
_tuple5**)Cyc__genarr_482),(void*)((struct _tuple5**)Cyc__genarr_482 + 3)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_254={1,1,(void*)((void*)& Cyc_struct_Absyn_Pat_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_245;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_246;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_vardecl_t46H2_rep;
static char _tmp330[5]="List";static struct _dynforward_ptr Cyc__genname_250=(struct
_dynforward_ptr){_tmp330,_tmp330 + 5};static char _tmp331[3]="hd";static struct
_tuple5 Cyc__gentuple_247={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp331,_tmp331 + 3},(void*)& Cyc__genrep_159};static char _tmp332[
3]="tl";static struct _tuple5 Cyc__gentuple_248={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp332,_tmp332 + 3},(void*)& Cyc__genrep_246};static struct
_tuple5*Cyc__genarr_249[2]={& Cyc__gentuple_247,& Cyc__gentuple_248};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_vardecl_t46H2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_250,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_249),(void*)((
struct _tuple5**)Cyc__genarr_249 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_246={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_vardecl_t46H2_rep)};static char
_tmp335[4]="Opt";static struct _dynforward_ptr Cyc__genname_253=(struct
_dynforward_ptr){_tmp335,_tmp335 + 4};static char _tmp336[2]="v";static struct
_tuple5 Cyc__gentuple_251={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp336,_tmp336 + 2},(void*)& Cyc__genrep_246};static struct _tuple5*Cyc__genarr_252[
1]={& Cyc__gentuple_251};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_253,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_252),(void*)((struct _tuple5**)Cyc__genarr_252 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_245={1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep)};
static char _tmp339[14]="Switch_clause";static struct _dynforward_ptr Cyc__genname_490=(
struct _dynforward_ptr){_tmp339,_tmp339 + 14};static char _tmp33A[8]="pattern";
static struct _tuple5 Cyc__gentuple_484={offsetof(struct Cyc_Absyn_Switch_clause,pattern),(
struct _dynforward_ptr){_tmp33A,_tmp33A + 8},(void*)& Cyc__genrep_254};static char
_tmp33B[9]="pat_vars";static struct _tuple5 Cyc__gentuple_485={offsetof(struct Cyc_Absyn_Switch_clause,pat_vars),(
struct _dynforward_ptr){_tmp33B,_tmp33B + 9},(void*)& Cyc__genrep_245};static char
_tmp33C[13]="where_clause";static struct _tuple5 Cyc__gentuple_486={offsetof(struct
Cyc_Absyn_Switch_clause,where_clause),(struct _dynforward_ptr){_tmp33C,_tmp33C + 
13},(void*)& Cyc__genrep_146};static char _tmp33D[5]="body";static struct _tuple5 Cyc__gentuple_487={
offsetof(struct Cyc_Absyn_Switch_clause,body),(struct _dynforward_ptr){_tmp33D,
_tmp33D + 5},(void*)& Cyc__genrep_150};static char _tmp33E[4]="loc";static struct
_tuple5 Cyc__gentuple_488={offsetof(struct Cyc_Absyn_Switch_clause,loc),(struct
_dynforward_ptr){_tmp33E,_tmp33E + 4},(void*)& Cyc__genrep_2};static struct _tuple5*
Cyc__genarr_489[5]={& Cyc__gentuple_484,& Cyc__gentuple_485,& Cyc__gentuple_486,&
Cyc__gentuple_487,& Cyc__gentuple_488};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Switch_clause_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_490,sizeof(struct Cyc_Absyn_Switch_clause),{(
void*)((struct _tuple5**)Cyc__genarr_489),(void*)((struct _tuple5**)Cyc__genarr_489
+ 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_244={1,1,(void*)((void*)&
Cyc_struct_Absyn_Switch_clause_rep)};static char _tmp341[5]="List";static struct
_dynforward_ptr Cyc__genname_494=(struct _dynforward_ptr){_tmp341,_tmp341 + 5};
static char _tmp342[3]="hd";static struct _tuple5 Cyc__gentuple_491={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp342,_tmp342 + 3},(void*)((void*)&
Cyc__genrep_244)};static char _tmp343[3]="tl";static struct _tuple5 Cyc__gentuple_492={
offsetof(struct Cyc_List_List,tl),(struct _dynforward_ptr){_tmp343,_tmp343 + 3},(
void*)& Cyc__genrep_243};static struct _tuple5*Cyc__genarr_493[2]={& Cyc__gentuple_491,&
Cyc__gentuple_492};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_494,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_493),(void*)((struct _tuple5**)Cyc__genarr_493 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_243={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep)};
struct _tuple58{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_List_List*f3;};
static struct _tuple6 Cyc__gentuple_530={offsetof(struct _tuple58,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_531={offsetof(struct _tuple58,f2),(void*)& Cyc__genrep_223};
static struct _tuple6 Cyc__gentuple_532={offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_243};
static struct _tuple6*Cyc__genarr_533[3]={& Cyc__gentuple_530,& Cyc__gentuple_531,&
Cyc__gentuple_532};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_529={4,
sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_533),(void*)((
struct _tuple6**)Cyc__genarr_533 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_518;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_520;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_exp_t46H2_rep;static char _tmp347[5]="List";static
struct _dynforward_ptr Cyc__genname_524=(struct _dynforward_ptr){_tmp347,_tmp347 + 5};
static char _tmp348[3]="hd";static struct _tuple5 Cyc__gentuple_521={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp348,_tmp348 + 3},(void*)& Cyc__genrep_223};
static char _tmp349[3]="tl";static struct _tuple5 Cyc__gentuple_522={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp349,_tmp349 + 3},(void*)& Cyc__genrep_520};
static struct _tuple5*Cyc__genarr_523[2]={& Cyc__gentuple_521,& Cyc__gentuple_522};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep={3,(
struct _dynforward_ptr*)& Cyc__genname_524,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_523),(void*)((struct _tuple5**)Cyc__genarr_523 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_520={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_exp_t46H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_519;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_519={1,1,(void*)((void*)& Cyc__genrep_244)};struct _tuple59{
unsigned int f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Switch_clause**f3;};static
struct _tuple6 Cyc__gentuple_525={offsetof(struct _tuple59,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_526={offsetof(struct _tuple59,f2),(void*)& Cyc__genrep_520};
static struct _tuple6 Cyc__gentuple_527={offsetof(struct _tuple59,f3),(void*)& Cyc__genrep_519};
static struct _tuple6*Cyc__genarr_528[3]={& Cyc__gentuple_525,& Cyc__gentuple_526,&
Cyc__gentuple_527};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_518={4,
sizeof(struct _tuple59),{(void*)((struct _tuple6**)Cyc__genarr_528),(void*)((
struct _tuple6**)Cyc__genarr_528 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_513;
struct _tuple60{unsigned int f1;struct Cyc_Absyn_Decl*f2;struct Cyc_Absyn_Stmt*f3;};
static struct _tuple6 Cyc__gentuple_514={offsetof(struct _tuple60,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_515={offsetof(struct _tuple60,f2),(void*)& Cyc__genrep_1};
static struct _tuple6 Cyc__gentuple_516={offsetof(struct _tuple60,f3),(void*)& Cyc__genrep_150};
static struct _tuple6*Cyc__genarr_517[3]={& Cyc__gentuple_514,& Cyc__gentuple_515,&
Cyc__gentuple_516};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_513={4,
sizeof(struct _tuple60),{(void*)((struct _tuple6**)Cyc__genarr_517),(void*)((
struct _tuple6**)Cyc__genarr_517 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_508;
static struct _tuple6 Cyc__gentuple_509={offsetof(struct _tuple40,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_510={offsetof(struct _tuple40,f2),(void*)& Cyc__genrep_13};
static struct _tuple6 Cyc__gentuple_511={offsetof(struct _tuple40,f3),(void*)& Cyc__genrep_150};
static struct _tuple6*Cyc__genarr_512[3]={& Cyc__gentuple_509,& Cyc__gentuple_510,&
Cyc__gentuple_511};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_508={4,
sizeof(struct _tuple40),{(void*)((struct _tuple6**)Cyc__genarr_512),(void*)((
struct _tuple6**)Cyc__genarr_512 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_499;
struct _tuple61{unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct _tuple2 f3;};static
struct _tuple6 Cyc__gentuple_504={offsetof(struct _tuple61,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_505={offsetof(struct _tuple61,f2),(void*)& Cyc__genrep_150};
static struct _tuple6 Cyc__gentuple_506={offsetof(struct _tuple61,f3),(void*)& Cyc__genrep_500};
static struct _tuple6*Cyc__genarr_507[3]={& Cyc__gentuple_504,& Cyc__gentuple_505,&
Cyc__gentuple_506};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_499={4,
sizeof(struct _tuple61),{(void*)((struct _tuple6**)Cyc__genarr_507),(void*)((
struct _tuple6**)Cyc__genarr_507 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_242;
struct _tuple62{unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;};
static struct _tuple6 Cyc__gentuple_495={offsetof(struct _tuple62,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_496={offsetof(struct _tuple62,f2),(void*)& Cyc__genrep_150};
static struct _tuple6 Cyc__gentuple_497={offsetof(struct _tuple62,f3),(void*)& Cyc__genrep_243};
static struct _tuple6*Cyc__genarr_498[3]={& Cyc__gentuple_495,& Cyc__gentuple_496,&
Cyc__gentuple_497};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_242={4,
sizeof(struct _tuple62),{(void*)((struct _tuple6**)Cyc__genarr_498),(void*)((
struct _tuple6**)Cyc__genarr_498 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_234;
struct _tuple63{unsigned int f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;
int f4;struct Cyc_Absyn_Exp*f5;struct Cyc_Absyn_Stmt*f6;};static struct _tuple6 Cyc__gentuple_235={
offsetof(struct _tuple63,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_236={
offsetof(struct _tuple63,f2),(void*)& Cyc__genrep_185};static struct _tuple6 Cyc__gentuple_237={
offsetof(struct _tuple63,f3),(void*)& Cyc__genrep_159};static struct _tuple6 Cyc__gentuple_238={
offsetof(struct _tuple63,f4),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_239={
offsetof(struct _tuple63,f5),(void*)& Cyc__genrep_146};static struct _tuple6 Cyc__gentuple_240={
offsetof(struct _tuple63,f6),(void*)& Cyc__genrep_150};static struct _tuple6*Cyc__genarr_241[
6]={& Cyc__gentuple_235,& Cyc__gentuple_236,& Cyc__gentuple_237,& Cyc__gentuple_238,&
Cyc__gentuple_239,& Cyc__gentuple_240};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_234={
4,sizeof(struct _tuple63),{(void*)((struct _tuple6**)Cyc__genarr_241),(void*)((
struct _tuple6**)Cyc__genarr_241 + 6)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_158;
struct _tuple64{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Tvar*f3;
struct Cyc_Absyn_Vardecl*f4;struct Cyc_Absyn_Stmt*f5;};static struct _tuple6 Cyc__gentuple_224={
offsetof(struct _tuple64,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_225={
offsetof(struct _tuple64,f2),(void*)& Cyc__genrep_223};static struct _tuple6 Cyc__gentuple_226={
offsetof(struct _tuple64,f3),(void*)& Cyc__genrep_185};static struct _tuple6 Cyc__gentuple_227={
offsetof(struct _tuple64,f4),(void*)& Cyc__genrep_159};static struct _tuple6 Cyc__gentuple_228={
offsetof(struct _tuple64,f5),(void*)& Cyc__genrep_150};static struct _tuple6*Cyc__genarr_229[
5]={& Cyc__gentuple_224,& Cyc__gentuple_225,& Cyc__gentuple_226,& Cyc__gentuple_227,&
Cyc__gentuple_228};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_158={4,
sizeof(struct _tuple64),{(void*)((struct _tuple6**)Cyc__genarr_229),(void*)((
struct _tuple6**)Cyc__genarr_229 + 5)}};static char _tmp354[7]="Skip_s";static struct
_tuple7 Cyc__gentuple_156={0,(struct _dynforward_ptr){_tmp354,_tmp354 + 7}};static
struct _tuple7*Cyc__genarr_157[1]={& Cyc__gentuple_156};static char _tmp355[6]="Exp_s";
static struct _tuple5 Cyc__gentuple_571={0,(struct _dynforward_ptr){_tmp355,_tmp355 + 
6},(void*)& Cyc__genrep_230};static char _tmp356[6]="Seq_s";static struct _tuple5 Cyc__gentuple_572={
1,(struct _dynforward_ptr){_tmp356,_tmp356 + 6},(void*)& Cyc__genrep_566};static
char _tmp357[9]="Return_s";static struct _tuple5 Cyc__gentuple_573={2,(struct
_dynforward_ptr){_tmp357,_tmp357 + 9},(void*)& Cyc__genrep_562};static char _tmp358[
13]="IfThenElse_s";static struct _tuple5 Cyc__gentuple_574={3,(struct
_dynforward_ptr){_tmp358,_tmp358 + 13},(void*)& Cyc__genrep_556};static char _tmp359[
8]="While_s";static struct _tuple5 Cyc__gentuple_575={4,(struct _dynforward_ptr){
_tmp359,_tmp359 + 8},(void*)& Cyc__genrep_551};static char _tmp35A[8]="Break_s";
static struct _tuple5 Cyc__gentuple_576={5,(struct _dynforward_ptr){_tmp35A,_tmp35A + 
8},(void*)& Cyc__genrep_547};static char _tmp35B[11]="Continue_s";static struct
_tuple5 Cyc__gentuple_577={6,(struct _dynforward_ptr){_tmp35B,_tmp35B + 11},(void*)&
Cyc__genrep_547};static char _tmp35C[7]="Goto_s";static struct _tuple5 Cyc__gentuple_578={
7,(struct _dynforward_ptr){_tmp35C,_tmp35C + 7},(void*)& Cyc__genrep_541};static
char _tmp35D[6]="For_s";static struct _tuple5 Cyc__gentuple_579={8,(struct
_dynforward_ptr){_tmp35D,_tmp35D + 6},(void*)& Cyc__genrep_534};static char _tmp35E[
9]="Switch_s";static struct _tuple5 Cyc__gentuple_580={9,(struct _dynforward_ptr){
_tmp35E,_tmp35E + 9},(void*)& Cyc__genrep_529};static char _tmp35F[11]="Fallthru_s";
static struct _tuple5 Cyc__gentuple_581={10,(struct _dynforward_ptr){_tmp35F,_tmp35F
+ 11},(void*)& Cyc__genrep_518};static char _tmp360[7]="Decl_s";static struct _tuple5
Cyc__gentuple_582={11,(struct _dynforward_ptr){_tmp360,_tmp360 + 7},(void*)& Cyc__genrep_513};
static char _tmp361[8]="Label_s";static struct _tuple5 Cyc__gentuple_583={12,(struct
_dynforward_ptr){_tmp361,_tmp361 + 8},(void*)& Cyc__genrep_508};static char _tmp362[
5]="Do_s";static struct _tuple5 Cyc__gentuple_584={13,(struct _dynforward_ptr){
_tmp362,_tmp362 + 5},(void*)& Cyc__genrep_499};static char _tmp363[11]="TryCatch_s";
static struct _tuple5 Cyc__gentuple_585={14,(struct _dynforward_ptr){_tmp363,_tmp363
+ 11},(void*)& Cyc__genrep_242};static char _tmp364[9]="Region_s";static struct
_tuple5 Cyc__gentuple_586={15,(struct _dynforward_ptr){_tmp364,_tmp364 + 9},(void*)&
Cyc__genrep_234};static char _tmp365[14]="ResetRegion_s";static struct _tuple5 Cyc__gentuple_587={
16,(struct _dynforward_ptr){_tmp365,_tmp365 + 14},(void*)& Cyc__genrep_230};static
char _tmp366[8]="Alias_s";static struct _tuple5 Cyc__gentuple_588={17,(struct
_dynforward_ptr){_tmp366,_tmp366 + 8},(void*)& Cyc__genrep_158};static struct
_tuple5*Cyc__genarr_589[18]={& Cyc__gentuple_571,& Cyc__gentuple_572,& Cyc__gentuple_573,&
Cyc__gentuple_574,& Cyc__gentuple_575,& Cyc__gentuple_576,& Cyc__gentuple_577,& Cyc__gentuple_578,&
Cyc__gentuple_579,& Cyc__gentuple_580,& Cyc__gentuple_581,& Cyc__gentuple_582,& Cyc__gentuple_583,&
Cyc__gentuple_584,& Cyc__gentuple_585,& Cyc__gentuple_586,& Cyc__gentuple_587,& Cyc__gentuple_588};
static char _tmp368[9]="Raw_stmt";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep={
5,(struct _dynforward_ptr){_tmp368,_tmp368 + 9},{(void*)((struct _tuple7**)Cyc__genarr_157),(
void*)((struct _tuple7**)Cyc__genarr_157 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_589),(
void*)((struct _tuple5**)Cyc__genarr_589 + 18)}};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_151;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_stmt_t46H2_rep;
static char _tmp369[5]="List";static struct _dynforward_ptr Cyc__genname_155=(struct
_dynforward_ptr){_tmp369,_tmp369 + 5};static char _tmp36A[3]="hd";static struct
_tuple5 Cyc__gentuple_152={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp36A,_tmp36A + 3},(void*)& Cyc__genrep_150};static char _tmp36B[
3]="tl";static struct _tuple5 Cyc__gentuple_153={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp36B,_tmp36B + 3},(void*)& Cyc__genrep_151};static struct
_tuple5*Cyc__genarr_154[2]={& Cyc__gentuple_152,& Cyc__gentuple_153};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_stmt_t46H2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_155,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_154),(void*)((
struct _tuple5**)Cyc__genarr_154 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_151={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_stmt_t46H2_rep)};extern struct Cyc_Typerep_XTUnion_struct
Cyc_Absyn_absyn_annot_t_rep;static struct _tuple8*Cyc__genarr_147[0]={};static char
_tmp36F[11]="AbsynAnnot";struct Cyc_Typerep_XTUnion_struct Cyc_Absyn_absyn_annot_t_rep={
7,(struct _dynforward_ptr){_tmp36F,_tmp36F + 11},{(void*)((struct _tuple8**)Cyc__genarr_147),(
void*)((struct _tuple8**)Cyc__genarr_147 + 0)}};static char _tmp370[5]="Stmt";static
struct _dynforward_ptr Cyc__genname_596=(struct _dynforward_ptr){_tmp370,_tmp370 + 5};
static char _tmp371[2]="r";static struct _tuple5 Cyc__gentuple_590={offsetof(struct
Cyc_Absyn_Stmt,r),(struct _dynforward_ptr){_tmp371,_tmp371 + 2},(void*)& Cyc_Absyn_raw_stmt_t_rep};
static char _tmp372[4]="loc";static struct _tuple5 Cyc__gentuple_591={offsetof(struct
Cyc_Absyn_Stmt,loc),(struct _dynforward_ptr){_tmp372,_tmp372 + 4},(void*)& Cyc__genrep_2};
static char _tmp373[16]="non_local_preds";static struct _tuple5 Cyc__gentuple_592={
offsetof(struct Cyc_Absyn_Stmt,non_local_preds),(struct _dynforward_ptr){_tmp373,
_tmp373 + 16},(void*)& Cyc__genrep_151};static char _tmp374[10]="try_depth";static
struct _tuple5 Cyc__gentuple_593={offsetof(struct Cyc_Absyn_Stmt,try_depth),(struct
_dynforward_ptr){_tmp374,_tmp374 + 10},(void*)& Cyc__genrep_10};static char _tmp375[
6]="annot";static struct _tuple5 Cyc__gentuple_594={offsetof(struct Cyc_Absyn_Stmt,annot),(
struct _dynforward_ptr){_tmp375,_tmp375 + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};
static struct _tuple5*Cyc__genarr_595[5]={& Cyc__gentuple_590,& Cyc__gentuple_591,&
Cyc__gentuple_592,& Cyc__gentuple_593,& Cyc__gentuple_594};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep={3,(struct _dynforward_ptr*)& Cyc__genname_596,sizeof(
struct Cyc_Absyn_Stmt),{(void*)((struct _tuple5**)Cyc__genarr_595),(void*)((struct
_tuple5**)Cyc__genarr_595 + 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_150={
1,1,(void*)((void*)& Cyc_struct_Absyn_Stmt_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_831;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_831={1,1,(void*)((
void*)& Cyc_struct_Absyn_Vardecl_rep)};static char _tmp379[7]="Fndecl";static struct
_dynforward_ptr Cyc__genname_860=(struct _dynforward_ptr){_tmp379,_tmp379 + 7};
static char _tmp37A[3]="sc";static struct _tuple5 Cyc__gentuple_844={offsetof(struct
Cyc_Absyn_Fndecl,sc),(struct _dynforward_ptr){_tmp37A,_tmp37A + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp37B[10]="is_inline";static struct _tuple5 Cyc__gentuple_845={
offsetof(struct Cyc_Absyn_Fndecl,is_inline),(struct _dynforward_ptr){_tmp37B,
_tmp37B + 10},(void*)& Cyc__genrep_10};static char _tmp37C[5]="name";static struct
_tuple5 Cyc__gentuple_846={offsetof(struct Cyc_Absyn_Fndecl,name),(struct
_dynforward_ptr){_tmp37C,_tmp37C + 5},(void*)& Cyc__genrep_11};static char _tmp37D[4]="tvs";
static struct _tuple5 Cyc__gentuple_847={offsetof(struct Cyc_Absyn_Fndecl,tvs),(
struct _dynforward_ptr){_tmp37D,_tmp37D + 4},(void*)& Cyc__genrep_335};static char
_tmp37E[7]="effect";static struct _tuple5 Cyc__gentuple_848={offsetof(struct Cyc_Absyn_Fndecl,effect),(
struct _dynforward_ptr){_tmp37E,_tmp37E + 7},(void*)& Cyc__genrep_110};static char
_tmp37F[9]="ret_type";static struct _tuple5 Cyc__gentuple_849={offsetof(struct Cyc_Absyn_Fndecl,ret_type),(
struct _dynforward_ptr){_tmp37F,_tmp37F + 9},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp380[5]="args";static struct _tuple5 Cyc__gentuple_850={offsetof(
struct Cyc_Absyn_Fndecl,args),(struct _dynforward_ptr){_tmp380,_tmp380 + 5},(void*)&
Cyc__genrep_833};static char _tmp381[10]="c_varargs";static struct _tuple5 Cyc__gentuple_851={
offsetof(struct Cyc_Absyn_Fndecl,c_varargs),(struct _dynforward_ptr){_tmp381,
_tmp381 + 10},(void*)& Cyc__genrep_10};static char _tmp382[12]="cyc_varargs";static
struct _tuple5 Cyc__gentuple_852={offsetof(struct Cyc_Absyn_Fndecl,cyc_varargs),(
struct _dynforward_ptr){_tmp382,_tmp382 + 12},(void*)& Cyc__genrep_832};static char
_tmp383[7]="rgn_po";static struct _tuple5 Cyc__gentuple_853={offsetof(struct Cyc_Absyn_Fndecl,rgn_po),(
struct _dynforward_ptr){_tmp383,_tmp383 + 7},(void*)& Cyc__genrep_396};static char
_tmp384[5]="body";static struct _tuple5 Cyc__gentuple_854={offsetof(struct Cyc_Absyn_Fndecl,body),(
struct _dynforward_ptr){_tmp384,_tmp384 + 5},(void*)& Cyc__genrep_150};static char
_tmp385[11]="cached_typ";static struct _tuple5 Cyc__gentuple_855={offsetof(struct
Cyc_Absyn_Fndecl,cached_typ),(struct _dynforward_ptr){_tmp385,_tmp385 + 11},(void*)&
Cyc__genrep_110};static char _tmp386[15]="param_vardecls";static struct _tuple5 Cyc__gentuple_856={
offsetof(struct Cyc_Absyn_Fndecl,param_vardecls),(struct _dynforward_ptr){_tmp386,
_tmp386 + 15},(void*)& Cyc__genrep_245};static char _tmp387[11]="fn_vardecl";static
struct _tuple5 Cyc__gentuple_857={offsetof(struct Cyc_Absyn_Fndecl,fn_vardecl),(
struct _dynforward_ptr){_tmp387,_tmp387 + 11},(void*)& Cyc__genrep_831};static char
_tmp388[11]="attributes";static struct _tuple5 Cyc__gentuple_858={offsetof(struct
Cyc_Absyn_Fndecl,attributes),(struct _dynforward_ptr){_tmp388,_tmp388 + 11},(void*)&
Cyc__genrep_62};static struct _tuple5*Cyc__genarr_859[15]={& Cyc__gentuple_844,& Cyc__gentuple_845,&
Cyc__gentuple_846,& Cyc__gentuple_847,& Cyc__gentuple_848,& Cyc__gentuple_849,& Cyc__gentuple_850,&
Cyc__gentuple_851,& Cyc__gentuple_852,& Cyc__gentuple_853,& Cyc__gentuple_854,& Cyc__gentuple_855,&
Cyc__gentuple_856,& Cyc__gentuple_857,& Cyc__gentuple_858};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Fndecl_rep={3,(struct _dynforward_ptr*)& Cyc__genname_860,sizeof(
struct Cyc_Absyn_Fndecl),{(void*)((struct _tuple5**)Cyc__genarr_859),(void*)((
struct _tuple5**)Cyc__genarr_859 + 15)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_830={
1,1,(void*)((void*)& Cyc_struct_Absyn_Fndecl_rep)};struct _tuple65{unsigned int f1;
struct Cyc_Absyn_Fndecl*f2;};static struct _tuple6 Cyc__gentuple_861={offsetof(
struct _tuple65,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_862={
offsetof(struct _tuple65,f2),(void*)& Cyc__genrep_830};static struct _tuple6*Cyc__genarr_863[
2]={& Cyc__gentuple_861,& Cyc__gentuple_862};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_829={4,sizeof(struct _tuple65),{(void*)((struct _tuple6**)Cyc__genarr_863),(
void*)((struct _tuple6**)Cyc__genarr_863 + 2)}};static char _tmp38C[13]="Unresolved_b";
static struct _tuple7 Cyc__gentuple_823={0,(struct _dynforward_ptr){_tmp38C,_tmp38C + 
13}};static struct _tuple7*Cyc__genarr_824[1]={& Cyc__gentuple_823};static char
_tmp38D[9]="Global_b";static struct _tuple5 Cyc__gentuple_864={0,(struct
_dynforward_ptr){_tmp38D,_tmp38D + 9},(void*)& Cyc__genrep_825};static char _tmp38E[
10]="Funname_b";static struct _tuple5 Cyc__gentuple_865={1,(struct _dynforward_ptr){
_tmp38E,_tmp38E + 10},(void*)& Cyc__genrep_829};static char _tmp38F[8]="Param_b";
static struct _tuple5 Cyc__gentuple_866={2,(struct _dynforward_ptr){_tmp38F,_tmp38F + 
8},(void*)& Cyc__genrep_825};static char _tmp390[8]="Local_b";static struct _tuple5
Cyc__gentuple_867={3,(struct _dynforward_ptr){_tmp390,_tmp390 + 8},(void*)& Cyc__genrep_825};
static char _tmp391[6]="Pat_b";static struct _tuple5 Cyc__gentuple_868={4,(struct
_dynforward_ptr){_tmp391,_tmp391 + 6},(void*)& Cyc__genrep_825};static struct
_tuple5*Cyc__genarr_869[5]={& Cyc__gentuple_864,& Cyc__gentuple_865,& Cyc__gentuple_866,&
Cyc__gentuple_867,& Cyc__gentuple_868};static char _tmp393[8]="Binding";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_binding_t_rep={5,(struct _dynforward_ptr){_tmp393,_tmp393 + 8},{(void*)((
struct _tuple7**)Cyc__genarr_824),(void*)((struct _tuple7**)Cyc__genarr_824 + 1)},{(
void*)((struct _tuple5**)Cyc__genarr_869),(void*)((struct _tuple5**)Cyc__genarr_869
+ 5)}};struct _tuple66{unsigned int f1;struct _tuple0*f2;void*f3;};static struct
_tuple6 Cyc__gentuple_870={offsetof(struct _tuple66,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_871={offsetof(struct _tuple66,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_872={offsetof(struct _tuple66,f3),(void*)& Cyc_Absyn_binding_t_rep};
static struct _tuple6*Cyc__genarr_873[3]={& Cyc__gentuple_870,& Cyc__gentuple_871,&
Cyc__gentuple_872};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_822={4,
sizeof(struct _tuple66),{(void*)((struct _tuple6**)Cyc__genarr_873),(void*)((
struct _tuple6**)Cyc__genarr_873 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_817;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep;static char _tmp395[5]="Plus";
static struct _tuple7 Cyc__gentuple_787={0,(struct _dynforward_ptr){_tmp395,_tmp395 + 
5}};static char _tmp396[6]="Times";static struct _tuple7 Cyc__gentuple_788={1,(struct
_dynforward_ptr){_tmp396,_tmp396 + 6}};static char _tmp397[6]="Minus";static struct
_tuple7 Cyc__gentuple_789={2,(struct _dynforward_ptr){_tmp397,_tmp397 + 6}};static
char _tmp398[4]="Div";static struct _tuple7 Cyc__gentuple_790={3,(struct
_dynforward_ptr){_tmp398,_tmp398 + 4}};static char _tmp399[4]="Mod";static struct
_tuple7 Cyc__gentuple_791={4,(struct _dynforward_ptr){_tmp399,_tmp399 + 4}};static
char _tmp39A[3]="Eq";static struct _tuple7 Cyc__gentuple_792={5,(struct
_dynforward_ptr){_tmp39A,_tmp39A + 3}};static char _tmp39B[4]="Neq";static struct
_tuple7 Cyc__gentuple_793={6,(struct _dynforward_ptr){_tmp39B,_tmp39B + 4}};static
char _tmp39C[3]="Gt";static struct _tuple7 Cyc__gentuple_794={7,(struct
_dynforward_ptr){_tmp39C,_tmp39C + 3}};static char _tmp39D[3]="Lt";static struct
_tuple7 Cyc__gentuple_795={8,(struct _dynforward_ptr){_tmp39D,_tmp39D + 3}};static
char _tmp39E[4]="Gte";static struct _tuple7 Cyc__gentuple_796={9,(struct
_dynforward_ptr){_tmp39E,_tmp39E + 4}};static char _tmp39F[4]="Lte";static struct
_tuple7 Cyc__gentuple_797={10,(struct _dynforward_ptr){_tmp39F,_tmp39F + 4}};static
char _tmp3A0[4]="Not";static struct _tuple7 Cyc__gentuple_798={11,(struct
_dynforward_ptr){_tmp3A0,_tmp3A0 + 4}};static char _tmp3A1[7]="Bitnot";static struct
_tuple7 Cyc__gentuple_799={12,(struct _dynforward_ptr){_tmp3A1,_tmp3A1 + 7}};static
char _tmp3A2[7]="Bitand";static struct _tuple7 Cyc__gentuple_800={13,(struct
_dynforward_ptr){_tmp3A2,_tmp3A2 + 7}};static char _tmp3A3[6]="Bitor";static struct
_tuple7 Cyc__gentuple_801={14,(struct _dynforward_ptr){_tmp3A3,_tmp3A3 + 6}};static
char _tmp3A4[7]="Bitxor";static struct _tuple7 Cyc__gentuple_802={15,(struct
_dynforward_ptr){_tmp3A4,_tmp3A4 + 7}};static char _tmp3A5[10]="Bitlshift";static
struct _tuple7 Cyc__gentuple_803={16,(struct _dynforward_ptr){_tmp3A5,_tmp3A5 + 10}};
static char _tmp3A6[11]="Bitlrshift";static struct _tuple7 Cyc__gentuple_804={17,(
struct _dynforward_ptr){_tmp3A6,_tmp3A6 + 11}};static char _tmp3A7[11]="Bitarshift";
static struct _tuple7 Cyc__gentuple_805={18,(struct _dynforward_ptr){_tmp3A7,_tmp3A7
+ 11}};static char _tmp3A8[5]="Size";static struct _tuple7 Cyc__gentuple_806={19,(
struct _dynforward_ptr){_tmp3A8,_tmp3A8 + 5}};static struct _tuple7*Cyc__genarr_807[
20]={& Cyc__gentuple_787,& Cyc__gentuple_788,& Cyc__gentuple_789,& Cyc__gentuple_790,&
Cyc__gentuple_791,& Cyc__gentuple_792,& Cyc__gentuple_793,& Cyc__gentuple_794,& Cyc__gentuple_795,&
Cyc__gentuple_796,& Cyc__gentuple_797,& Cyc__gentuple_798,& Cyc__gentuple_799,& Cyc__gentuple_800,&
Cyc__gentuple_801,& Cyc__gentuple_802,& Cyc__gentuple_803,& Cyc__gentuple_804,& Cyc__gentuple_805,&
Cyc__gentuple_806};static struct _tuple5*Cyc__genarr_808[0]={};static char _tmp3AA[7]="Primop";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep={5,(struct _dynforward_ptr){
_tmp3AA,_tmp3AA + 7},{(void*)((struct _tuple7**)Cyc__genarr_807),(void*)((struct
_tuple7**)Cyc__genarr_807 + 20)},{(void*)((struct _tuple5**)Cyc__genarr_808),(void*)((
struct _tuple5**)Cyc__genarr_808 + 0)}};struct _tuple67{unsigned int f1;void*f2;
struct Cyc_List_List*f3;};static struct _tuple6 Cyc__gentuple_818={offsetof(struct
_tuple67,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_819={
offsetof(struct _tuple67,f2),(void*)& Cyc_Absyn_primop_t_rep};static struct _tuple6
Cyc__gentuple_820={offsetof(struct _tuple67,f3),(void*)& Cyc__genrep_520};static
struct _tuple6*Cyc__genarr_821[3]={& Cyc__gentuple_818,& Cyc__gentuple_819,& Cyc__gentuple_820};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_817={4,sizeof(struct _tuple67),{(
void*)((struct _tuple6**)Cyc__genarr_821),(void*)((struct _tuple6**)Cyc__genarr_821
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_785;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_786;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_primop_t2_rep;
static char _tmp3AC[4]="Opt";static struct _dynforward_ptr Cyc__genname_811=(struct
_dynforward_ptr){_tmp3AC,_tmp3AC + 4};static char _tmp3AD[2]="v";static struct
_tuple5 Cyc__gentuple_809={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp3AD,_tmp3AD + 2},(void*)& Cyc_Absyn_primop_t_rep};static struct _tuple5*Cyc__genarr_810[
1]={& Cyc__gentuple_809};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_primop_t2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_811,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_810),(void*)((struct _tuple5**)Cyc__genarr_810 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_786={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_primop_t2_rep)};
struct _tuple68{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Core_Opt*f3;
struct Cyc_Absyn_Exp*f4;};static struct _tuple6 Cyc__gentuple_812={offsetof(struct
_tuple68,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_813={
offsetof(struct _tuple68,f2),(void*)& Cyc__genrep_223};static struct _tuple6 Cyc__gentuple_814={
offsetof(struct _tuple68,f3),(void*)& Cyc__genrep_786};static struct _tuple6 Cyc__gentuple_815={
offsetof(struct _tuple68,f4),(void*)& Cyc__genrep_223};static struct _tuple6*Cyc__genarr_816[
4]={& Cyc__gentuple_812,& Cyc__gentuple_813,& Cyc__gentuple_814,& Cyc__gentuple_815};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_785={4,sizeof(struct _tuple68),{(
void*)((struct _tuple6**)Cyc__genarr_816),(void*)((struct _tuple6**)Cyc__genarr_816
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_774;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_incrementor_t_rep;static char _tmp3B1[7]="PreInc";static struct _tuple7 Cyc__gentuple_775={
0,(struct _dynforward_ptr){_tmp3B1,_tmp3B1 + 7}};static char _tmp3B2[8]="PostInc";
static struct _tuple7 Cyc__gentuple_776={1,(struct _dynforward_ptr){_tmp3B2,_tmp3B2 + 
8}};static char _tmp3B3[7]="PreDec";static struct _tuple7 Cyc__gentuple_777={2,(
struct _dynforward_ptr){_tmp3B3,_tmp3B3 + 7}};static char _tmp3B4[8]="PostDec";
static struct _tuple7 Cyc__gentuple_778={3,(struct _dynforward_ptr){_tmp3B4,_tmp3B4 + 
8}};static struct _tuple7*Cyc__genarr_779[4]={& Cyc__gentuple_775,& Cyc__gentuple_776,&
Cyc__gentuple_777,& Cyc__gentuple_778};static struct _tuple5*Cyc__genarr_780[0]={};
static char _tmp3B6[12]="Incrementor";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep={
5,(struct _dynforward_ptr){_tmp3B6,_tmp3B6 + 12},{(void*)((struct _tuple7**)Cyc__genarr_779),(
void*)((struct _tuple7**)Cyc__genarr_779 + 4)},{(void*)((struct _tuple5**)Cyc__genarr_780),(
void*)((struct _tuple5**)Cyc__genarr_780 + 0)}};struct _tuple69{unsigned int f1;
struct Cyc_Absyn_Exp*f2;void*f3;};static struct _tuple6 Cyc__gentuple_781={offsetof(
struct _tuple69,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_782={
offsetof(struct _tuple69,f2),(void*)& Cyc__genrep_223};static struct _tuple6 Cyc__gentuple_783={
offsetof(struct _tuple69,f3),(void*)& Cyc_Absyn_incrementor_t_rep};static struct
_tuple6*Cyc__genarr_784[3]={& Cyc__gentuple_781,& Cyc__gentuple_782,& Cyc__gentuple_783};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_774={4,sizeof(struct _tuple69),{(
void*)((struct _tuple6**)Cyc__genarr_784),(void*)((struct _tuple6**)Cyc__genarr_784
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_768;struct _tuple70{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Exp*
f4;};static struct _tuple6 Cyc__gentuple_769={offsetof(struct _tuple70,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_770={offsetof(struct _tuple70,f2),(
void*)& Cyc__genrep_223};static struct _tuple6 Cyc__gentuple_771={offsetof(struct
_tuple70,f3),(void*)& Cyc__genrep_223};static struct _tuple6 Cyc__gentuple_772={
offsetof(struct _tuple70,f4),(void*)& Cyc__genrep_223};static struct _tuple6*Cyc__genarr_773[
4]={& Cyc__gentuple_769,& Cyc__gentuple_770,& Cyc__gentuple_771,& Cyc__gentuple_772};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_768={4,sizeof(struct _tuple70),{(
void*)((struct _tuple6**)Cyc__genarr_773),(void*)((struct _tuple6**)Cyc__genarr_773
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_619;struct _tuple71{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};static struct
_tuple6 Cyc__gentuple_620={offsetof(struct _tuple71,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_621={offsetof(struct _tuple71,f2),(void*)& Cyc__genrep_223};
static struct _tuple6 Cyc__gentuple_622={offsetof(struct _tuple71,f3),(void*)& Cyc__genrep_223};
static struct _tuple6*Cyc__genarr_623[3]={& Cyc__gentuple_620,& Cyc__gentuple_621,&
Cyc__gentuple_622};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_619={4,
sizeof(struct _tuple71),{(void*)((struct _tuple6**)Cyc__genarr_623),(void*)((
struct _tuple6**)Cyc__genarr_623 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_763;
static struct _tuple6 Cyc__gentuple_764={offsetof(struct _tuple58,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_765={offsetof(struct _tuple58,f2),(void*)& Cyc__genrep_223};
static struct _tuple6 Cyc__gentuple_766={offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_520};
static struct _tuple6*Cyc__genarr_767[3]={& Cyc__gentuple_764,& Cyc__gentuple_765,&
Cyc__gentuple_766};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_763={4,
sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_767),(void*)((
struct _tuple6**)Cyc__genarr_767 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_744;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_745;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_call_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_746;
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_746={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};
static char _tmp3BC[15]="VarargCallInfo";static struct _dynforward_ptr Cyc__genname_757=(
struct _dynforward_ptr){_tmp3BC,_tmp3BC + 15};static char _tmp3BD[12]="num_varargs";
static struct _tuple5 Cyc__gentuple_753={offsetof(struct Cyc_Absyn_VarargCallInfo,num_varargs),(
struct _dynforward_ptr){_tmp3BD,_tmp3BD + 12},(void*)& Cyc__genrep_10};static char
_tmp3BE[10]="injectors";static struct _tuple5 Cyc__gentuple_754={offsetof(struct Cyc_Absyn_VarargCallInfo,injectors),(
struct _dynforward_ptr){_tmp3BE,_tmp3BE + 10},(void*)& Cyc__genrep_327};static char
_tmp3BF[4]="vai";static struct _tuple5 Cyc__gentuple_755={offsetof(struct Cyc_Absyn_VarargCallInfo,vai),(
struct _dynforward_ptr){_tmp3BF,_tmp3BF + 4},(void*)& Cyc__genrep_746};static struct
_tuple5*Cyc__genarr_756[3]={& Cyc__gentuple_753,& Cyc__gentuple_754,& Cyc__gentuple_755};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_vararg_call_info_t_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_757,sizeof(struct Cyc_Absyn_VarargCallInfo),{(void*)((
struct _tuple5**)Cyc__genarr_756),(void*)((struct _tuple5**)Cyc__genarr_756 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_745={1,1,(void*)((void*)& Cyc_Absyn_vararg_call_info_t_rep)};
struct _tuple72{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_List_List*f3;
struct Cyc_Absyn_VarargCallInfo*f4;};static struct _tuple6 Cyc__gentuple_758={
offsetof(struct _tuple72,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_759={
offsetof(struct _tuple72,f2),(void*)& Cyc__genrep_223};static struct _tuple6 Cyc__gentuple_760={
offsetof(struct _tuple72,f3),(void*)& Cyc__genrep_520};static struct _tuple6 Cyc__gentuple_761={
offsetof(struct _tuple72,f4),(void*)& Cyc__genrep_745};static struct _tuple6*Cyc__genarr_762[
4]={& Cyc__gentuple_758,& Cyc__gentuple_759,& Cyc__gentuple_760,& Cyc__gentuple_761};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_744={4,sizeof(struct _tuple72),{(
void*)((struct _tuple6**)Cyc__genarr_762),(void*)((struct _tuple6**)Cyc__genarr_762
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_739;static struct _tuple6
Cyc__gentuple_740={offsetof(struct _tuple58,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_741={offsetof(struct _tuple58,f2),(void*)& Cyc__genrep_223};
static struct _tuple6 Cyc__gentuple_742={offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_121};
static struct _tuple6*Cyc__genarr_743[3]={& Cyc__gentuple_740,& Cyc__gentuple_741,&
Cyc__gentuple_742};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_739={4,
sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_743),(void*)((
struct _tuple6**)Cyc__genarr_743 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_726;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_coercion_t_rep;static char _tmp3C4[
17]="Unknown_coercion";static struct _tuple7 Cyc__gentuple_727={0,(struct
_dynforward_ptr){_tmp3C4,_tmp3C4 + 17}};static char _tmp3C5[12]="No_coercion";
static struct _tuple7 Cyc__gentuple_728={1,(struct _dynforward_ptr){_tmp3C5,_tmp3C5 + 
12}};static char _tmp3C6[16]="NonNull_to_Null";static struct _tuple7 Cyc__gentuple_729={
2,(struct _dynforward_ptr){_tmp3C6,_tmp3C6 + 16}};static char _tmp3C7[15]="Other_coercion";
static struct _tuple7 Cyc__gentuple_730={3,(struct _dynforward_ptr){_tmp3C7,_tmp3C7 + 
15}};static struct _tuple7*Cyc__genarr_731[4]={& Cyc__gentuple_727,& Cyc__gentuple_728,&
Cyc__gentuple_729,& Cyc__gentuple_730};static struct _tuple5*Cyc__genarr_732[0]={};
static char _tmp3C9[9]="Coercion";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_coercion_t_rep={
5,(struct _dynforward_ptr){_tmp3C9,_tmp3C9 + 9},{(void*)((struct _tuple7**)Cyc__genarr_731),(
void*)((struct _tuple7**)Cyc__genarr_731 + 4)},{(void*)((struct _tuple5**)Cyc__genarr_732),(
void*)((struct _tuple5**)Cyc__genarr_732 + 0)}};struct _tuple73{unsigned int f1;void*
f2;struct Cyc_Absyn_Exp*f3;int f4;void*f5;};static struct _tuple6 Cyc__gentuple_733={
offsetof(struct _tuple73,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_734={
offsetof(struct _tuple73,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_735={offsetof(struct _tuple73,f3),(void*)& Cyc__genrep_223};
static struct _tuple6 Cyc__gentuple_736={offsetof(struct _tuple73,f4),(void*)& Cyc__genrep_10};
static struct _tuple6 Cyc__gentuple_737={offsetof(struct _tuple73,f5),(void*)& Cyc_Absyn_coercion_t_rep};
static struct _tuple6*Cyc__genarr_738[5]={& Cyc__gentuple_733,& Cyc__gentuple_734,&
Cyc__gentuple_735,& Cyc__gentuple_736,& Cyc__gentuple_737};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_726={4,sizeof(struct _tuple73),{(void*)((struct _tuple6**)Cyc__genarr_738),(
void*)((struct _tuple6**)Cyc__genarr_738 + 5)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_721;static struct _tuple6 Cyc__gentuple_722={offsetof(struct _tuple71,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_723={offsetof(struct
_tuple71,f2),(void*)& Cyc__genrep_146};static struct _tuple6 Cyc__gentuple_724={
offsetof(struct _tuple71,f3),(void*)& Cyc__genrep_223};static struct _tuple6*Cyc__genarr_725[
3]={& Cyc__gentuple_722,& Cyc__gentuple_723,& Cyc__gentuple_724};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_721={4,sizeof(struct _tuple71),{(void*)((struct _tuple6**)Cyc__genarr_725),(
void*)((struct _tuple6**)Cyc__genarr_725 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_116;static struct _tuple6 Cyc__gentuple_117={offsetof(struct _tuple6,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_118={offsetof(struct
_tuple6,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_119[
2]={& Cyc__gentuple_117,& Cyc__gentuple_118};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_116={4,sizeof(struct _tuple6),{(void*)((struct _tuple6**)Cyc__genarr_119),(
void*)((struct _tuple6**)Cyc__genarr_119 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_708;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_offsetof_field_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_710;struct _tuple74{unsigned int
f1;unsigned int f2;};static struct _tuple6 Cyc__gentuple_711={offsetof(struct
_tuple74,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_712={
offsetof(struct _tuple74,f2),(void*)& Cyc__genrep_24};static struct _tuple6*Cyc__genarr_713[
2]={& Cyc__gentuple_711,& Cyc__gentuple_712};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_710={4,sizeof(struct _tuple74),{(void*)((struct _tuple6**)Cyc__genarr_713),(
void*)((struct _tuple6**)Cyc__genarr_713 + 2)}};static struct _tuple7*Cyc__genarr_709[
0]={};static char _tmp3CE[12]="StructField";static struct _tuple5 Cyc__gentuple_714={
0,(struct _dynforward_ptr){_tmp3CE,_tmp3CE + 12},(void*)& Cyc__genrep_360};static
char _tmp3CF[11]="TupleIndex";static struct _tuple5 Cyc__gentuple_715={1,(struct
_dynforward_ptr){_tmp3CF,_tmp3CF + 11},(void*)& Cyc__genrep_710};static struct
_tuple5*Cyc__genarr_716[2]={& Cyc__gentuple_714,& Cyc__gentuple_715};static char
_tmp3D1[14]="OffsetofField";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_offsetof_field_t_rep={
5,(struct _dynforward_ptr){_tmp3D1,_tmp3D1 + 14},{(void*)((struct _tuple7**)Cyc__genarr_709),(
void*)((struct _tuple7**)Cyc__genarr_709 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_716),(
void*)((struct _tuple5**)Cyc__genarr_716 + 2)}};static struct _tuple6 Cyc__gentuple_717={
offsetof(struct _tuple28,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_718={
offsetof(struct _tuple28,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_719={offsetof(struct _tuple28,f3),(void*)& Cyc_Absyn_offsetof_field_t_rep};
static struct _tuple6*Cyc__genarr_720[3]={& Cyc__gentuple_717,& Cyc__gentuple_718,&
Cyc__gentuple_719};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_708={4,
sizeof(struct _tuple28),{(void*)((struct _tuple6**)Cyc__genarr_720),(void*)((
struct _tuple6**)Cyc__genarr_720 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_703;
struct _tuple75{unsigned int f1;struct Cyc_List_List*f2;void*f3;};static struct
_tuple6 Cyc__gentuple_704={offsetof(struct _tuple75,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_705={offsetof(struct _tuple75,f2),(void*)& Cyc__genrep_335};
static struct _tuple6 Cyc__gentuple_706={offsetof(struct _tuple75,f3),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_707[3]={& Cyc__gentuple_704,&
Cyc__gentuple_705,& Cyc__gentuple_706};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_703={
4,sizeof(struct _tuple75),{(void*)((struct _tuple6**)Cyc__genarr_707),(void*)((
struct _tuple6**)Cyc__genarr_707 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_698;
struct _tuple76{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct _dynforward_ptr*f3;};
static struct _tuple6 Cyc__gentuple_699={offsetof(struct _tuple76,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_700={offsetof(struct _tuple76,f2),(void*)& Cyc__genrep_223};
static struct _tuple6 Cyc__gentuple_701={offsetof(struct _tuple76,f3),(void*)& Cyc__genrep_13};
static struct _tuple6*Cyc__genarr_702[3]={& Cyc__gentuple_699,& Cyc__gentuple_700,&
Cyc__gentuple_701};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_698={4,
sizeof(struct _tuple76),{(void*)((struct _tuple6**)Cyc__genarr_702),(void*)((
struct _tuple6**)Cyc__genarr_702 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_694;
static struct _tuple6 Cyc__gentuple_695={offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_696={offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_520};
static struct _tuple6*Cyc__genarr_697[2]={& Cyc__gentuple_695,& Cyc__gentuple_696};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_694={4,sizeof(struct _tuple14),{(
void*)((struct _tuple6**)Cyc__genarr_697),(void*)((struct _tuple6**)Cyc__genarr_697
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_679;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_680;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_681;static struct
_tuple6 Cyc__gentuple_686={offsetof(struct _tuple1,f1),(void*)& Cyc__genrep_682};
static struct _tuple6 Cyc__gentuple_687={offsetof(struct _tuple1,f2),(void*)& Cyc_Absyn_tqual_t_rep};
static struct _tuple6 Cyc__gentuple_688={offsetof(struct _tuple1,f3),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_689[3]={& Cyc__gentuple_686,&
Cyc__gentuple_687,& Cyc__gentuple_688};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_681={
4,sizeof(struct _tuple1),{(void*)((struct _tuple6**)Cyc__genarr_689),(void*)((
struct _tuple6**)Cyc__genarr_689 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_680={
1,1,(void*)((void*)& Cyc__genrep_681)};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_601;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_602;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_603;static struct _tuple6 Cyc__gentuple_604={offsetof(struct _tuple13,f1),(
void*)& Cyc__genrep_358};static struct _tuple6 Cyc__gentuple_605={offsetof(struct
_tuple13,f2),(void*)& Cyc__genrep_223};static struct _tuple6*Cyc__genarr_606[2]={&
Cyc__gentuple_604,& Cyc__gentuple_605};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_603={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_606),(void*)((
struct _tuple6**)Cyc__genarr_606 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_602={
1,1,(void*)((void*)& Cyc__genrep_603)};static char _tmp3DA[5]="List";static struct
_dynforward_ptr Cyc__genname_610=(struct _dynforward_ptr){_tmp3DA,_tmp3DA + 5};
static char _tmp3DB[3]="hd";static struct _tuple5 Cyc__gentuple_607={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp3DB,_tmp3DB + 3},(void*)& Cyc__genrep_602};
static char _tmp3DC[3]="tl";static struct _tuple5 Cyc__gentuple_608={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp3DC,_tmp3DC + 3},(void*)& Cyc__genrep_601};
static struct _tuple5*Cyc__genarr_609[2]={& Cyc__gentuple_607,& Cyc__gentuple_608};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_610,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_609),(void*)((struct _tuple5**)Cyc__genarr_609 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_601={1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep)};
struct _tuple77{unsigned int f1;struct _tuple1*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_690={offsetof(struct _tuple77,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_691={offsetof(struct _tuple77,f2),(void*)& Cyc__genrep_680};
static struct _tuple6 Cyc__gentuple_692={offsetof(struct _tuple77,f3),(void*)& Cyc__genrep_601};
static struct _tuple6*Cyc__genarr_693[3]={& Cyc__gentuple_690,& Cyc__gentuple_691,&
Cyc__gentuple_692};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_679={4,
sizeof(struct _tuple77),{(void*)((struct _tuple6**)Cyc__genarr_693),(void*)((
struct _tuple6**)Cyc__genarr_693 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_675;
static struct _tuple6 Cyc__gentuple_676={offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_677={offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_601};
static struct _tuple6*Cyc__genarr_678[2]={& Cyc__gentuple_676,& Cyc__gentuple_677};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_675={4,sizeof(struct _tuple14),{(
void*)((struct _tuple6**)Cyc__genarr_678),(void*)((struct _tuple6**)Cyc__genarr_678
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_668;struct _tuple78{
unsigned int f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Exp*
f4;int f5;};static struct _tuple6 Cyc__gentuple_669={offsetof(struct _tuple78,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_670={offsetof(struct
_tuple78,f2),(void*)& Cyc__genrep_159};static struct _tuple6 Cyc__gentuple_671={
offsetof(struct _tuple78,f3),(void*)& Cyc__genrep_223};static struct _tuple6 Cyc__gentuple_672={
offsetof(struct _tuple78,f4),(void*)& Cyc__genrep_223};static struct _tuple6 Cyc__gentuple_673={
offsetof(struct _tuple78,f5),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_674[
5]={& Cyc__gentuple_669,& Cyc__gentuple_670,& Cyc__gentuple_671,& Cyc__gentuple_672,&
Cyc__gentuple_673};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_668={4,
sizeof(struct _tuple78),{(void*)((struct _tuple6**)Cyc__genarr_674),(void*)((
struct _tuple6**)Cyc__genarr_674 + 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_660;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_661;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_661={1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrdecl_rep)};struct
_tuple79{unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;struct Cyc_List_List*
f4;struct Cyc_Absyn_Aggrdecl*f5;};static struct _tuple6 Cyc__gentuple_662={offsetof(
struct _tuple79,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_663={
offsetof(struct _tuple79,f2),(void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_664={
offsetof(struct _tuple79,f3),(void*)& Cyc__genrep_121};static struct _tuple6 Cyc__gentuple_665={
offsetof(struct _tuple79,f4),(void*)& Cyc__genrep_601};static struct _tuple6 Cyc__gentuple_666={
offsetof(struct _tuple79,f5),(void*)& Cyc__genrep_661};static struct _tuple6*Cyc__genarr_667[
5]={& Cyc__gentuple_662,& Cyc__gentuple_663,& Cyc__gentuple_664,& Cyc__gentuple_665,&
Cyc__gentuple_666};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_660={4,
sizeof(struct _tuple79),{(void*)((struct _tuple6**)Cyc__genarr_667),(void*)((
struct _tuple6**)Cyc__genarr_667 + 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_655;
static struct _tuple6 Cyc__gentuple_656={offsetof(struct _tuple67,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_657={offsetof(struct _tuple67,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_658={offsetof(struct
_tuple67,f3),(void*)& Cyc__genrep_601};static struct _tuple6*Cyc__genarr_659[3]={&
Cyc__gentuple_656,& Cyc__gentuple_657,& Cyc__gentuple_658};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_655={4,sizeof(struct _tuple67),{(void*)((struct _tuple6**)Cyc__genarr_659),(
void*)((struct _tuple6**)Cyc__genarr_659 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_649;struct _tuple80{unsigned int f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Tuniondecl*
f3;struct Cyc_Absyn_Tunionfield*f4;};static struct _tuple6 Cyc__gentuple_650={
offsetof(struct _tuple80,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_651={
offsetof(struct _tuple80,f2),(void*)& Cyc__genrep_520};static struct _tuple6 Cyc__gentuple_652={
offsetof(struct _tuple80,f3),(void*)((void*)& Cyc__genrep_325)};static struct
_tuple6 Cyc__gentuple_653={offsetof(struct _tuple80,f4),(void*)& Cyc__genrep_308};
static struct _tuple6*Cyc__genarr_654[4]={& Cyc__gentuple_650,& Cyc__gentuple_651,&
Cyc__gentuple_652,& Cyc__gentuple_653};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_649={
4,sizeof(struct _tuple80),{(void*)((struct _tuple6**)Cyc__genarr_654),(void*)((
struct _tuple6**)Cyc__genarr_654 + 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_642;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_643;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_643={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_636;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_636={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};struct
_tuple81{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple6 Cyc__gentuple_644={offsetof(struct _tuple81,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_645={offsetof(struct _tuple81,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_646={offsetof(struct
_tuple81,f3),(void*)& Cyc__genrep_643};static struct _tuple6 Cyc__gentuple_647={
offsetof(struct _tuple81,f4),(void*)& Cyc__genrep_636};static struct _tuple6*Cyc__genarr_648[
4]={& Cyc__gentuple_644,& Cyc__gentuple_645,& Cyc__gentuple_646,& Cyc__gentuple_647};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_642={4,sizeof(struct _tuple81),{(
void*)((struct _tuple6**)Cyc__genarr_648),(void*)((struct _tuple6**)Cyc__genarr_648
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_635;struct _tuple82{
unsigned int f1;struct _tuple0*f2;void*f3;struct Cyc_Absyn_Enumfield*f4;};static
struct _tuple6 Cyc__gentuple_637={offsetof(struct _tuple82,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_638={offsetof(struct _tuple82,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_639={offsetof(struct _tuple82,f3),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_640={offsetof(struct
_tuple82,f4),(void*)& Cyc__genrep_636};static struct _tuple6*Cyc__genarr_641[4]={&
Cyc__gentuple_637,& Cyc__gentuple_638,& Cyc__gentuple_639,& Cyc__gentuple_640};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_635={4,sizeof(struct _tuple82),{(
void*)((struct _tuple6**)Cyc__genarr_641),(void*)((struct _tuple6**)Cyc__genarr_641
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_624;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_malloc_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_130;
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_130={1,1,(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp3EB[11]="MallocInfo";static struct _dynforward_ptr Cyc__genname_631=(
struct _dynforward_ptr){_tmp3EB,_tmp3EB + 11};static char _tmp3EC[10]="is_calloc";
static struct _tuple5 Cyc__gentuple_625={offsetof(struct Cyc_Absyn_MallocInfo,is_calloc),(
struct _dynforward_ptr){_tmp3EC,_tmp3EC + 10},(void*)& Cyc__genrep_10};static char
_tmp3ED[4]="rgn";static struct _tuple5 Cyc__gentuple_626={offsetof(struct Cyc_Absyn_MallocInfo,rgn),(
struct _dynforward_ptr){_tmp3ED,_tmp3ED + 4},(void*)& Cyc__genrep_146};static char
_tmp3EE[9]="elt_type";static struct _tuple5 Cyc__gentuple_627={offsetof(struct Cyc_Absyn_MallocInfo,elt_type),(
struct _dynforward_ptr){_tmp3EE,_tmp3EE + 9},(void*)& Cyc__genrep_130};static char
_tmp3EF[9]="num_elts";static struct _tuple5 Cyc__gentuple_628={offsetof(struct Cyc_Absyn_MallocInfo,num_elts),(
struct _dynforward_ptr){_tmp3EF,_tmp3EF + 9},(void*)& Cyc__genrep_223};static char
_tmp3F0[11]="fat_result";static struct _tuple5 Cyc__gentuple_629={offsetof(struct
Cyc_Absyn_MallocInfo,fat_result),(struct _dynforward_ptr){_tmp3F0,_tmp3F0 + 11},(
void*)& Cyc__genrep_10};static struct _tuple5*Cyc__genarr_630[5]={& Cyc__gentuple_625,&
Cyc__gentuple_626,& Cyc__gentuple_627,& Cyc__gentuple_628,& Cyc__gentuple_629};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_malloc_info_t_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_631,sizeof(struct Cyc_Absyn_MallocInfo),{(void*)((
struct _tuple5**)Cyc__genarr_630),(void*)((struct _tuple5**)Cyc__genarr_630 + 5)}};
struct _tuple83{unsigned int f1;struct Cyc_Absyn_MallocInfo f2;};static struct _tuple6
Cyc__gentuple_632={offsetof(struct _tuple83,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_633={offsetof(struct _tuple83,f2),(void*)& Cyc_Absyn_malloc_info_t_rep};
static struct _tuple6*Cyc__genarr_634[2]={& Cyc__gentuple_632,& Cyc__gentuple_633};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_624={4,sizeof(struct _tuple83),{(
void*)((struct _tuple6**)Cyc__genarr_634),(void*)((struct _tuple6**)Cyc__genarr_634
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_600;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_611;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep;
static char _tmp3F3[4]="Opt";static struct _dynforward_ptr Cyc__genname_614=(struct
_dynforward_ptr){_tmp3F3,_tmp3F3 + 4};static char _tmp3F4[2]="v";static struct
_tuple5 Cyc__gentuple_612={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp3F4,_tmp3F4 + 2},(void*)& Cyc__genrep_11};static struct _tuple5*Cyc__genarr_613[
1]={& Cyc__gentuple_612};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_614,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_613),(void*)((struct _tuple5**)Cyc__genarr_613 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_611={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep)};
struct _tuple84{unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;};
static struct _tuple6 Cyc__gentuple_615={offsetof(struct _tuple84,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_616={offsetof(struct _tuple84,f2),(void*)& Cyc__genrep_611};
static struct _tuple6 Cyc__gentuple_617={offsetof(struct _tuple84,f3),(void*)& Cyc__genrep_601};
static struct _tuple6*Cyc__genarr_618[3]={& Cyc__gentuple_615,& Cyc__gentuple_616,&
Cyc__gentuple_617};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_600={4,
sizeof(struct _tuple84),{(void*)((struct _tuple6**)Cyc__genarr_618),(void*)((
struct _tuple6**)Cyc__genarr_618 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_149;
static struct _tuple6 Cyc__gentuple_597={offsetof(struct _tuple39,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_598={offsetof(struct _tuple39,f2),(void*)& Cyc__genrep_150};
static struct _tuple6*Cyc__genarr_599[2]={& Cyc__gentuple_597,& Cyc__gentuple_598};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_149={4,sizeof(struct _tuple39),{(
void*)((struct _tuple6**)Cyc__genarr_599),(void*)((struct _tuple6**)Cyc__genarr_599
+ 2)}};static struct _tuple7*Cyc__genarr_148[0]={};static char _tmp3F9[8]="Const_e";
static struct _tuple5 Cyc__gentuple_904={0,(struct _dynforward_ptr){_tmp3F9,_tmp3F9 + 
8},(void*)& Cyc__genrep_874};static char _tmp3FA[6]="Var_e";static struct _tuple5 Cyc__gentuple_905={
1,(struct _dynforward_ptr){_tmp3FA,_tmp3FA + 6},(void*)& Cyc__genrep_822};static
char _tmp3FB[12]="UnknownId_e";static struct _tuple5 Cyc__gentuple_906={2,(struct
_dynforward_ptr){_tmp3FB,_tmp3FB + 12},(void*)& Cyc__genrep_269};static char _tmp3FC[
9]="Primop_e";static struct _tuple5 Cyc__gentuple_907={3,(struct _dynforward_ptr){
_tmp3FC,_tmp3FC + 9},(void*)& Cyc__genrep_817};static char _tmp3FD[11]="AssignOp_e";
static struct _tuple5 Cyc__gentuple_908={4,(struct _dynforward_ptr){_tmp3FD,_tmp3FD + 
11},(void*)& Cyc__genrep_785};static char _tmp3FE[12]="Increment_e";static struct
_tuple5 Cyc__gentuple_909={5,(struct _dynforward_ptr){_tmp3FE,_tmp3FE + 12},(void*)&
Cyc__genrep_774};static char _tmp3FF[14]="Conditional_e";static struct _tuple5 Cyc__gentuple_910={
6,(struct _dynforward_ptr){_tmp3FF,_tmp3FF + 14},(void*)& Cyc__genrep_768};static
char _tmp400[6]="And_e";static struct _tuple5 Cyc__gentuple_911={7,(struct
_dynforward_ptr){_tmp400,_tmp400 + 6},(void*)& Cyc__genrep_619};static char _tmp401[
5]="Or_e";static struct _tuple5 Cyc__gentuple_912={8,(struct _dynforward_ptr){
_tmp401,_tmp401 + 5},(void*)& Cyc__genrep_619};static char _tmp402[9]="SeqExp_e";
static struct _tuple5 Cyc__gentuple_913={9,(struct _dynforward_ptr){_tmp402,_tmp402 + 
9},(void*)& Cyc__genrep_619};static char _tmp403[14]="UnknownCall_e";static struct
_tuple5 Cyc__gentuple_914={10,(struct _dynforward_ptr){_tmp403,_tmp403 + 14},(void*)&
Cyc__genrep_763};static char _tmp404[9]="FnCall_e";static struct _tuple5 Cyc__gentuple_915={
11,(struct _dynforward_ptr){_tmp404,_tmp404 + 9},(void*)& Cyc__genrep_744};static
char _tmp405[8]="Throw_e";static struct _tuple5 Cyc__gentuple_916={12,(struct
_dynforward_ptr){_tmp405,_tmp405 + 8},(void*)& Cyc__genrep_230};static char _tmp406[
16]="NoInstantiate_e";static struct _tuple5 Cyc__gentuple_917={13,(struct
_dynforward_ptr){_tmp406,_tmp406 + 16},(void*)& Cyc__genrep_230};static char _tmp407[
14]="Instantiate_e";static struct _tuple5 Cyc__gentuple_918={14,(struct
_dynforward_ptr){_tmp407,_tmp407 + 14},(void*)& Cyc__genrep_739};static char _tmp408[
7]="Cast_e";static struct _tuple5 Cyc__gentuple_919={15,(struct _dynforward_ptr){
_tmp408,_tmp408 + 7},(void*)& Cyc__genrep_726};static char _tmp409[10]="Address_e";
static struct _tuple5 Cyc__gentuple_920={16,(struct _dynforward_ptr){_tmp409,_tmp409
+ 10},(void*)& Cyc__genrep_230};static char _tmp40A[6]="New_e";static struct _tuple5
Cyc__gentuple_921={17,(struct _dynforward_ptr){_tmp40A,_tmp40A + 6},(void*)& Cyc__genrep_721};
static char _tmp40B[12]="Sizeoftyp_e";static struct _tuple5 Cyc__gentuple_922={18,(
struct _dynforward_ptr){_tmp40B,_tmp40B + 12},(void*)& Cyc__genrep_116};static char
_tmp40C[12]="Sizeofexp_e";static struct _tuple5 Cyc__gentuple_923={19,(struct
_dynforward_ptr){_tmp40C,_tmp40C + 12},(void*)& Cyc__genrep_230};static char _tmp40D[
11]="Offsetof_e";static struct _tuple5 Cyc__gentuple_924={20,(struct _dynforward_ptr){
_tmp40D,_tmp40D + 11},(void*)& Cyc__genrep_708};static char _tmp40E[9]="Gentyp_e";
static struct _tuple5 Cyc__gentuple_925={21,(struct _dynforward_ptr){_tmp40E,_tmp40E
+ 9},(void*)& Cyc__genrep_703};static char _tmp40F[8]="Deref_e";static struct _tuple5
Cyc__gentuple_926={22,(struct _dynforward_ptr){_tmp40F,_tmp40F + 8},(void*)& Cyc__genrep_230};
static char _tmp410[13]="AggrMember_e";static struct _tuple5 Cyc__gentuple_927={23,(
struct _dynforward_ptr){_tmp410,_tmp410 + 13},(void*)& Cyc__genrep_698};static char
_tmp411[12]="AggrArrow_e";static struct _tuple5 Cyc__gentuple_928={24,(struct
_dynforward_ptr){_tmp411,_tmp411 + 12},(void*)& Cyc__genrep_698};static char _tmp412[
12]="Subscript_e";static struct _tuple5 Cyc__gentuple_929={25,(struct
_dynforward_ptr){_tmp412,_tmp412 + 12},(void*)& Cyc__genrep_619};static char _tmp413[
8]="Tuple_e";static struct _tuple5 Cyc__gentuple_930={26,(struct _dynforward_ptr){
_tmp413,_tmp413 + 8},(void*)& Cyc__genrep_694};static char _tmp414[14]="CompoundLit_e";
static struct _tuple5 Cyc__gentuple_931={27,(struct _dynforward_ptr){_tmp414,_tmp414
+ 14},(void*)& Cyc__genrep_679};static char _tmp415[8]="Array_e";static struct
_tuple5 Cyc__gentuple_932={28,(struct _dynforward_ptr){_tmp415,_tmp415 + 8},(void*)&
Cyc__genrep_675};static char _tmp416[16]="Comprehension_e";static struct _tuple5 Cyc__gentuple_933={
29,(struct _dynforward_ptr){_tmp416,_tmp416 + 16},(void*)& Cyc__genrep_668};static
char _tmp417[9]="Struct_e";static struct _tuple5 Cyc__gentuple_934={30,(struct
_dynforward_ptr){_tmp417,_tmp417 + 9},(void*)& Cyc__genrep_660};static char _tmp418[
13]="AnonStruct_e";static struct _tuple5 Cyc__gentuple_935={31,(struct
_dynforward_ptr){_tmp418,_tmp418 + 13},(void*)& Cyc__genrep_655};static char _tmp419[
9]="Tunion_e";static struct _tuple5 Cyc__gentuple_936={32,(struct _dynforward_ptr){
_tmp419,_tmp419 + 9},(void*)& Cyc__genrep_649};static char _tmp41A[7]="Enum_e";
static struct _tuple5 Cyc__gentuple_937={33,(struct _dynforward_ptr){_tmp41A,_tmp41A
+ 7},(void*)& Cyc__genrep_642};static char _tmp41B[11]="AnonEnum_e";static struct
_tuple5 Cyc__gentuple_938={34,(struct _dynforward_ptr){_tmp41B,_tmp41B + 11},(void*)&
Cyc__genrep_635};static char _tmp41C[9]="Malloc_e";static struct _tuple5 Cyc__gentuple_939={
35,(struct _dynforward_ptr){_tmp41C,_tmp41C + 9},(void*)& Cyc__genrep_624};static
char _tmp41D[7]="Swap_e";static struct _tuple5 Cyc__gentuple_940={36,(struct
_dynforward_ptr){_tmp41D,_tmp41D + 7},(void*)& Cyc__genrep_619};static char _tmp41E[
16]="UnresolvedMem_e";static struct _tuple5 Cyc__gentuple_941={37,(struct
_dynforward_ptr){_tmp41E,_tmp41E + 16},(void*)& Cyc__genrep_600};static char _tmp41F[
10]="StmtExp_e";static struct _tuple5 Cyc__gentuple_942={38,(struct _dynforward_ptr){
_tmp41F,_tmp41F + 10},(void*)& Cyc__genrep_149};static struct _tuple5*Cyc__genarr_943[
39]={& Cyc__gentuple_904,& Cyc__gentuple_905,& Cyc__gentuple_906,& Cyc__gentuple_907,&
Cyc__gentuple_908,& Cyc__gentuple_909,& Cyc__gentuple_910,& Cyc__gentuple_911,& Cyc__gentuple_912,&
Cyc__gentuple_913,& Cyc__gentuple_914,& Cyc__gentuple_915,& Cyc__gentuple_916,& Cyc__gentuple_917,&
Cyc__gentuple_918,& Cyc__gentuple_919,& Cyc__gentuple_920,& Cyc__gentuple_921,& Cyc__gentuple_922,&
Cyc__gentuple_923,& Cyc__gentuple_924,& Cyc__gentuple_925,& Cyc__gentuple_926,& Cyc__gentuple_927,&
Cyc__gentuple_928,& Cyc__gentuple_929,& Cyc__gentuple_930,& Cyc__gentuple_931,& Cyc__gentuple_932,&
Cyc__gentuple_933,& Cyc__gentuple_934,& Cyc__gentuple_935,& Cyc__gentuple_936,& Cyc__gentuple_937,&
Cyc__gentuple_938,& Cyc__gentuple_939,& Cyc__gentuple_940,& Cyc__gentuple_941,& Cyc__gentuple_942};
static char _tmp421[8]="Raw_exp";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_exp_t_rep={
5,(struct _dynforward_ptr){_tmp421,_tmp421 + 8},{(void*)((struct _tuple7**)Cyc__genarr_148),(
void*)((struct _tuple7**)Cyc__genarr_148 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_943),(
void*)((struct _tuple5**)Cyc__genarr_943 + 39)}};static char _tmp422[4]="Exp";static
struct _dynforward_ptr Cyc__genname_949=(struct _dynforward_ptr){_tmp422,_tmp422 + 4};
static char _tmp423[5]="topt";static struct _tuple5 Cyc__gentuple_944={offsetof(
struct Cyc_Absyn_Exp,topt),(struct _dynforward_ptr){_tmp423,_tmp423 + 5},(void*)&
Cyc__genrep_110};static char _tmp424[2]="r";static struct _tuple5 Cyc__gentuple_945={
offsetof(struct Cyc_Absyn_Exp,r),(struct _dynforward_ptr){_tmp424,_tmp424 + 2},(
void*)& Cyc_Absyn_raw_exp_t_rep};static char _tmp425[4]="loc";static struct _tuple5
Cyc__gentuple_946={offsetof(struct Cyc_Absyn_Exp,loc),(struct _dynforward_ptr){
_tmp425,_tmp425 + 4},(void*)& Cyc__genrep_2};static char _tmp426[6]="annot";static
struct _tuple5 Cyc__gentuple_947={offsetof(struct Cyc_Absyn_Exp,annot),(struct
_dynforward_ptr){_tmp426,_tmp426 + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};static
struct _tuple5*Cyc__genarr_948[4]={& Cyc__gentuple_944,& Cyc__gentuple_945,& Cyc__gentuple_946,&
Cyc__gentuple_947};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Exp_rep={3,(
struct _dynforward_ptr*)& Cyc__genname_949,sizeof(struct Cyc_Absyn_Exp),{(void*)((
struct _tuple5**)Cyc__genarr_948),(void*)((struct _tuple5**)Cyc__genarr_948 + 4)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_146={1,1,(void*)((void*)& Cyc_struct_Absyn_Exp_rep)};
static char _tmp429[10]="ArrayInfo";static struct _dynforward_ptr Cyc__genname_1019=(
struct _dynforward_ptr){_tmp429,_tmp429 + 10};static char _tmp42A[9]="elt_type";
static struct _tuple5 Cyc__gentuple_1013={offsetof(struct Cyc_Absyn_ArrayInfo,elt_type),(
struct _dynforward_ptr){_tmp42A,_tmp42A + 9},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp42B[3]="tq";static struct _tuple5 Cyc__gentuple_1014={offsetof(struct
Cyc_Absyn_ArrayInfo,tq),(struct _dynforward_ptr){_tmp42B,_tmp42B + 3},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp42C[9]="num_elts";static struct _tuple5 Cyc__gentuple_1015={offsetof(
struct Cyc_Absyn_ArrayInfo,num_elts),(struct _dynforward_ptr){_tmp42C,_tmp42C + 9},(
void*)& Cyc__genrep_146};static char _tmp42D[10]="zero_term";static struct _tuple5 Cyc__gentuple_1016={
offsetof(struct Cyc_Absyn_ArrayInfo,zero_term),(struct _dynforward_ptr){_tmp42D,
_tmp42D + 10},(void*)& Cyc__genrep_1000};static char _tmp42E[7]="zt_loc";static
struct _tuple5 Cyc__gentuple_1017={offsetof(struct Cyc_Absyn_ArrayInfo,zt_loc),(
struct _dynforward_ptr){_tmp42E,_tmp42E + 7},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_1018[5]={& Cyc__gentuple_1013,& Cyc__gentuple_1014,& Cyc__gentuple_1015,&
Cyc__gentuple_1016,& Cyc__gentuple_1017};struct Cyc_Typerep_Struct_struct Cyc_Absyn_array_info_t_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_1019,sizeof(struct Cyc_Absyn_ArrayInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1018),(void*)((struct _tuple5**)Cyc__genarr_1018
+ 5)}};struct _tuple85{unsigned int f1;struct Cyc_Absyn_ArrayInfo f2;};static struct
_tuple6 Cyc__gentuple_1020={offsetof(struct _tuple85,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1021={offsetof(struct _tuple85,f2),(void*)& Cyc_Absyn_array_info_t_rep};
static struct _tuple6*Cyc__genarr_1022[2]={& Cyc__gentuple_1020,& Cyc__gentuple_1021};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_999={4,sizeof(struct _tuple85),{(
void*)((struct _tuple6**)Cyc__genarr_1022),(void*)((struct _tuple6**)Cyc__genarr_1022
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_980;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_fn_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_981;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep;
static char _tmp431[5]="List";static struct _dynforward_ptr Cyc__genname_985=(struct
_dynforward_ptr){_tmp431,_tmp431 + 5};static char _tmp432[3]="hd";static struct
_tuple5 Cyc__gentuple_982={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp432,_tmp432 + 3},(void*)& Cyc__genrep_680};static char _tmp433[
3]="tl";static struct _tuple5 Cyc__gentuple_983={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp433,_tmp433 + 3},(void*)& Cyc__genrep_981};static struct
_tuple5*Cyc__genarr_984[2]={& Cyc__gentuple_982,& Cyc__gentuple_983};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_985,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_984),(void*)((struct _tuple5**)Cyc__genarr_984 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_981={1,1,(void*)((void*)& Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep)};
static char _tmp436[7]="FnInfo";static struct _dynforward_ptr Cyc__genname_995=(
struct _dynforward_ptr){_tmp436,_tmp436 + 7};static char _tmp437[6]="tvars";static
struct _tuple5 Cyc__gentuple_986={offsetof(struct Cyc_Absyn_FnInfo,tvars),(struct
_dynforward_ptr){_tmp437,_tmp437 + 6},(void*)& Cyc__genrep_335};static char _tmp438[
7]="effect";static struct _tuple5 Cyc__gentuple_987={offsetof(struct Cyc_Absyn_FnInfo,effect),(
struct _dynforward_ptr){_tmp438,_tmp438 + 7},(void*)& Cyc__genrep_110};static char
_tmp439[8]="ret_typ";static struct _tuple5 Cyc__gentuple_988={offsetof(struct Cyc_Absyn_FnInfo,ret_typ),(
struct _dynforward_ptr){_tmp439,_tmp439 + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp43A[5]="args";static struct _tuple5 Cyc__gentuple_989={offsetof(
struct Cyc_Absyn_FnInfo,args),(struct _dynforward_ptr){_tmp43A,_tmp43A + 5},(void*)&
Cyc__genrep_981};static char _tmp43B[10]="c_varargs";static struct _tuple5 Cyc__gentuple_990={
offsetof(struct Cyc_Absyn_FnInfo,c_varargs),(struct _dynforward_ptr){_tmp43B,
_tmp43B + 10},(void*)& Cyc__genrep_10};static char _tmp43C[12]="cyc_varargs";static
struct _tuple5 Cyc__gentuple_991={offsetof(struct Cyc_Absyn_FnInfo,cyc_varargs),(
struct _dynforward_ptr){_tmp43C,_tmp43C + 12},(void*)& Cyc__genrep_832};static char
_tmp43D[7]="rgn_po";static struct _tuple5 Cyc__gentuple_992={offsetof(struct Cyc_Absyn_FnInfo,rgn_po),(
struct _dynforward_ptr){_tmp43D,_tmp43D + 7},(void*)& Cyc__genrep_396};static char
_tmp43E[11]="attributes";static struct _tuple5 Cyc__gentuple_993={offsetof(struct
Cyc_Absyn_FnInfo,attributes),(struct _dynforward_ptr){_tmp43E,_tmp43E + 11},(void*)&
Cyc__genrep_62};static struct _tuple5*Cyc__genarr_994[8]={& Cyc__gentuple_986,& Cyc__gentuple_987,&
Cyc__gentuple_988,& Cyc__gentuple_989,& Cyc__gentuple_990,& Cyc__gentuple_991,& Cyc__gentuple_992,&
Cyc__gentuple_993};struct Cyc_Typerep_Struct_struct Cyc_Absyn_fn_info_t_rep={3,(
struct _dynforward_ptr*)& Cyc__genname_995,sizeof(struct Cyc_Absyn_FnInfo),{(void*)((
struct _tuple5**)Cyc__genarr_994),(void*)((struct _tuple5**)Cyc__genarr_994 + 8)}};
struct _tuple86{unsigned int f1;struct Cyc_Absyn_FnInfo f2;};static struct _tuple6 Cyc__gentuple_996={
offsetof(struct _tuple86,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_997={
offsetof(struct _tuple86,f2),(void*)& Cyc_Absyn_fn_info_t_rep};static struct _tuple6*
Cyc__genarr_998[2]={& Cyc__gentuple_996,& Cyc__gentuple_997};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_980={4,sizeof(struct _tuple86),{(void*)((struct _tuple6**)Cyc__genarr_998),(
void*)((struct _tuple6**)Cyc__genarr_998 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_976;static struct _tuple6 Cyc__gentuple_977={offsetof(struct _tuple14,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_978={offsetof(struct
_tuple14,f2),(void*)& Cyc__genrep_309};static struct _tuple6*Cyc__genarr_979[2]={&
Cyc__gentuple_977,& Cyc__gentuple_978};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_976={
4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_979),(void*)((
struct _tuple6**)Cyc__genarr_979 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_972;
struct _tuple87{unsigned int f1;struct Cyc_Absyn_AggrInfo f2;};static struct _tuple6
Cyc__gentuple_973={offsetof(struct _tuple87,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_974={offsetof(struct _tuple87,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};
static struct _tuple6*Cyc__genarr_975[2]={& Cyc__gentuple_973,& Cyc__gentuple_974};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_972={4,sizeof(struct _tuple87),{(
void*)((struct _tuple6**)Cyc__genarr_975),(void*)((struct _tuple6**)Cyc__genarr_975
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_967;static struct _tuple6
Cyc__gentuple_968={offsetof(struct _tuple67,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_969={offsetof(struct _tuple67,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};
static struct _tuple6 Cyc__gentuple_970={offsetof(struct _tuple67,f3),(void*)& Cyc__genrep_383};
static struct _tuple6*Cyc__genarr_971[3]={& Cyc__gentuple_968,& Cyc__gentuple_969,&
Cyc__gentuple_970};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_967={4,
sizeof(struct _tuple67),{(void*)((struct _tuple6**)Cyc__genarr_971),(void*)((
struct _tuple6**)Cyc__genarr_971 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_962;
struct _tuple88{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;};
static struct _tuple6 Cyc__gentuple_963={offsetof(struct _tuple88,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_964={offsetof(struct _tuple88,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_965={offsetof(struct _tuple88,f3),(void*)& Cyc__genrep_643};
static struct _tuple6*Cyc__genarr_966[3]={& Cyc__gentuple_963,& Cyc__gentuple_964,&
Cyc__gentuple_965};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_962={4,
sizeof(struct _tuple88),{(void*)((struct _tuple6**)Cyc__genarr_966),(void*)((
struct _tuple6**)Cyc__genarr_966 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_143;
static struct _tuple6 Cyc__gentuple_959={offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_960={offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_144};
static struct _tuple6*Cyc__genarr_961[2]={& Cyc__gentuple_959,& Cyc__gentuple_960};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_143={4,sizeof(struct _tuple14),{(
void*)((struct _tuple6**)Cyc__genarr_961),(void*)((struct _tuple6**)Cyc__genarr_961
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_138;static struct _tuple6
Cyc__gentuple_139={offsetof(struct _tuple28,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_140={offsetof(struct _tuple28,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};
static struct _tuple6 Cyc__gentuple_141={offsetof(struct _tuple28,f3),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_142[3]={& Cyc__gentuple_139,&
Cyc__gentuple_140,& Cyc__gentuple_141};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_138={
4,sizeof(struct _tuple28),{(void*)((struct _tuple6**)Cyc__genarr_142),(void*)((
struct _tuple6**)Cyc__genarr_142 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_129;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_131;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Typedefdecl_rep;static char _tmp447[12]="Typedefdecl";static
struct _dynforward_ptr Cyc__genname_1167=(struct _dynforward_ptr){_tmp447,_tmp447 + 
12};static char _tmp448[5]="name";static struct _tuple5 Cyc__gentuple_1160={offsetof(
struct Cyc_Absyn_Typedefdecl,name),(struct _dynforward_ptr){_tmp448,_tmp448 + 5},(
void*)& Cyc__genrep_11};static char _tmp449[3]="tq";static struct _tuple5 Cyc__gentuple_1161={
offsetof(struct Cyc_Absyn_Typedefdecl,tq),(struct _dynforward_ptr){_tmp449,_tmp449
+ 3},(void*)& Cyc_Absyn_tqual_t_rep};static char _tmp44A[4]="tvs";static struct
_tuple5 Cyc__gentuple_1162={offsetof(struct Cyc_Absyn_Typedefdecl,tvs),(struct
_dynforward_ptr){_tmp44A,_tmp44A + 4},(void*)& Cyc__genrep_335};static char _tmp44B[
5]="kind";static struct _tuple5 Cyc__gentuple_1163={offsetof(struct Cyc_Absyn_Typedefdecl,kind),(
struct _dynforward_ptr){_tmp44B,_tmp44B + 5},(void*)& Cyc__genrep_1123};static char
_tmp44C[5]="defn";static struct _tuple5 Cyc__gentuple_1164={offsetof(struct Cyc_Absyn_Typedefdecl,defn),(
struct _dynforward_ptr){_tmp44C,_tmp44C + 5},(void*)& Cyc__genrep_110};static char
_tmp44D[5]="atts";static struct _tuple5 Cyc__gentuple_1165={offsetof(struct Cyc_Absyn_Typedefdecl,atts),(
struct _dynforward_ptr){_tmp44D,_tmp44D + 5},(void*)& Cyc__genrep_62};static struct
_tuple5*Cyc__genarr_1166[6]={& Cyc__gentuple_1160,& Cyc__gentuple_1161,& Cyc__gentuple_1162,&
Cyc__gentuple_1163,& Cyc__gentuple_1164,& Cyc__gentuple_1165};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Typedefdecl_rep={3,(struct _dynforward_ptr*)& Cyc__genname_1167,
sizeof(struct Cyc_Absyn_Typedefdecl),{(void*)((struct _tuple5**)Cyc__genarr_1166),(
void*)((struct _tuple5**)Cyc__genarr_1166 + 6)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_131={1,1,(void*)((void*)& Cyc_struct_Absyn_Typedefdecl_rep)};struct
_tuple89{unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Typedefdecl*
f4;void**f5;};static struct _tuple6 Cyc__gentuple_132={offsetof(struct _tuple89,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_133={offsetof(struct
_tuple89,f2),(void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_134={
offsetof(struct _tuple89,f3),(void*)& Cyc__genrep_121};static struct _tuple6 Cyc__gentuple_135={
offsetof(struct _tuple89,f4),(void*)& Cyc__genrep_131};static struct _tuple6 Cyc__gentuple_136={
offsetof(struct _tuple89,f5),(void*)& Cyc__genrep_130};static struct _tuple6*Cyc__genarr_137[
5]={& Cyc__gentuple_132,& Cyc__gentuple_133,& Cyc__gentuple_134,& Cyc__gentuple_135,&
Cyc__gentuple_136};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_129={4,
sizeof(struct _tuple89),{(void*)((struct _tuple6**)Cyc__genarr_137),(void*)((
struct _tuple6**)Cyc__genarr_137 + 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_120;
static struct _tuple6 Cyc__gentuple_126={offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_127={offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_121};
static struct _tuple6*Cyc__genarr_128[2]={& Cyc__gentuple_126,& Cyc__gentuple_127};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_120={4,sizeof(struct _tuple14),{(
void*)((struct _tuple6**)Cyc__genarr_128),(void*)((struct _tuple6**)Cyc__genarr_128
+ 2)}};static char _tmp452[9]="VoidType";static struct _tuple7 Cyc__gentuple_111={0,(
struct _dynforward_ptr){_tmp452,_tmp452 + 9}};static char _tmp453[10]="FloatType";
static struct _tuple7 Cyc__gentuple_112={1,(struct _dynforward_ptr){_tmp453,_tmp453 + 
10}};static char _tmp454[8]="HeapRgn";static struct _tuple7 Cyc__gentuple_113={2,(
struct _dynforward_ptr){_tmp454,_tmp454 + 8}};static char _tmp455[10]="UniqueRgn";
static struct _tuple7 Cyc__gentuple_114={3,(struct _dynforward_ptr){_tmp455,_tmp455 + 
10}};static struct _tuple7*Cyc__genarr_115[4]={& Cyc__gentuple_111,& Cyc__gentuple_112,&
Cyc__gentuple_113,& Cyc__gentuple_114};static char _tmp456[5]="Evar";static struct
_tuple5 Cyc__gentuple_1133={0,(struct _dynforward_ptr){_tmp456,_tmp456 + 5},(void*)&
Cyc__genrep_1118};static char _tmp457[8]="VarType";static struct _tuple5 Cyc__gentuple_1134={
1,(struct _dynforward_ptr){_tmp457,_tmp457 + 8},(void*)& Cyc__genrep_1114};static
char _tmp458[11]="TunionType";static struct _tuple5 Cyc__gentuple_1135={2,(struct
_dynforward_ptr){_tmp458,_tmp458 + 11},(void*)& Cyc__genrep_1087};static char
_tmp459[16]="TunionFieldType";static struct _tuple5 Cyc__gentuple_1136={3,(struct
_dynforward_ptr){_tmp459,_tmp459 + 16},(void*)& Cyc__genrep_1061};static char
_tmp45A[12]="PointerType";static struct _tuple5 Cyc__gentuple_1137={4,(struct
_dynforward_ptr){_tmp45A,_tmp45A + 12},(void*)& Cyc__genrep_1035};static char
_tmp45B[8]="IntType";static struct _tuple5 Cyc__gentuple_1138={5,(struct
_dynforward_ptr){_tmp45B,_tmp45B + 8},(void*)& Cyc__genrep_1023};static char _tmp45C[
11]="DoubleType";static struct _tuple5 Cyc__gentuple_1139={6,(struct _dynforward_ptr){
_tmp45C,_tmp45C + 11},(void*)& Cyc__genrep_85};static char _tmp45D[10]="ArrayType";
static struct _tuple5 Cyc__gentuple_1140={7,(struct _dynforward_ptr){_tmp45D,_tmp45D
+ 10},(void*)& Cyc__genrep_999};static char _tmp45E[7]="FnType";static struct _tuple5
Cyc__gentuple_1141={8,(struct _dynforward_ptr){_tmp45E,_tmp45E + 7},(void*)& Cyc__genrep_980};
static char _tmp45F[10]="TupleType";static struct _tuple5 Cyc__gentuple_1142={9,(
struct _dynforward_ptr){_tmp45F,_tmp45F + 10},(void*)& Cyc__genrep_976};static char
_tmp460[9]="AggrType";static struct _tuple5 Cyc__gentuple_1143={10,(struct
_dynforward_ptr){_tmp460,_tmp460 + 9},(void*)& Cyc__genrep_972};static char _tmp461[
13]="AnonAggrType";static struct _tuple5 Cyc__gentuple_1144={11,(struct
_dynforward_ptr){_tmp461,_tmp461 + 13},(void*)& Cyc__genrep_967};static char _tmp462[
9]="EnumType";static struct _tuple5 Cyc__gentuple_1145={12,(struct _dynforward_ptr){
_tmp462,_tmp462 + 9},(void*)& Cyc__genrep_962};static char _tmp463[13]="AnonEnumType";
static struct _tuple5 Cyc__gentuple_1146={13,(struct _dynforward_ptr){_tmp463,
_tmp463 + 13},(void*)& Cyc__genrep_143};static char _tmp464[11]="SizeofType";static
struct _tuple5 Cyc__gentuple_1147={14,(struct _dynforward_ptr){_tmp464,_tmp464 + 11},(
void*)& Cyc__genrep_116};static char _tmp465[14]="RgnHandleType";static struct
_tuple5 Cyc__gentuple_1148={15,(struct _dynforward_ptr){_tmp465,_tmp465 + 14},(void*)&
Cyc__genrep_116};static char _tmp466[11]="DynRgnType";static struct _tuple5 Cyc__gentuple_1149={
16,(struct _dynforward_ptr){_tmp466,_tmp466 + 11},(void*)& Cyc__genrep_138};static
char _tmp467[12]="TypedefType";static struct _tuple5 Cyc__gentuple_1150={17,(struct
_dynforward_ptr){_tmp467,_tmp467 + 12},(void*)& Cyc__genrep_129};static char _tmp468[
8]="TagType";static struct _tuple5 Cyc__gentuple_1151={18,(struct _dynforward_ptr){
_tmp468,_tmp468 + 8},(void*)& Cyc__genrep_116};static char _tmp469[8]="TypeInt";
static struct _tuple5 Cyc__gentuple_1152={19,(struct _dynforward_ptr){_tmp469,
_tmp469 + 8},(void*)& Cyc__genrep_85};static char _tmp46A[10]="AccessEff";static
struct _tuple5 Cyc__gentuple_1153={20,(struct _dynforward_ptr){_tmp46A,_tmp46A + 10},(
void*)& Cyc__genrep_116};static char _tmp46B[8]="JoinEff";static struct _tuple5 Cyc__gentuple_1154={
21,(struct _dynforward_ptr){_tmp46B,_tmp46B + 8},(void*)& Cyc__genrep_120};static
char _tmp46C[8]="RgnsEff";static struct _tuple5 Cyc__gentuple_1155={22,(struct
_dynforward_ptr){_tmp46C,_tmp46C + 8},(void*)& Cyc__genrep_116};static struct
_tuple5*Cyc__genarr_1156[23]={& Cyc__gentuple_1133,& Cyc__gentuple_1134,& Cyc__gentuple_1135,&
Cyc__gentuple_1136,& Cyc__gentuple_1137,& Cyc__gentuple_1138,& Cyc__gentuple_1139,&
Cyc__gentuple_1140,& Cyc__gentuple_1141,& Cyc__gentuple_1142,& Cyc__gentuple_1143,&
Cyc__gentuple_1144,& Cyc__gentuple_1145,& Cyc__gentuple_1146,& Cyc__gentuple_1147,&
Cyc__gentuple_1148,& Cyc__gentuple_1149,& Cyc__gentuple_1150,& Cyc__gentuple_1151,&
Cyc__gentuple_1152,& Cyc__gentuple_1153,& Cyc__gentuple_1154,& Cyc__gentuple_1155};
static char _tmp46E[5]="Type";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep={
5,(struct _dynforward_ptr){_tmp46E,_tmp46E + 5},{(void*)((struct _tuple7**)Cyc__genarr_115),(
void*)((struct _tuple7**)Cyc__genarr_115 + 4)},{(void*)((struct _tuple5**)Cyc__genarr_1156),(
void*)((struct _tuple5**)Cyc__genarr_1156 + 23)}};static char _tmp46F[8]="Vardecl";
static struct _dynforward_ptr Cyc__genname_184=(struct _dynforward_ptr){_tmp46F,
_tmp46F + 8};static char _tmp470[3]="sc";static struct _tuple5 Cyc__gentuple_175={
offsetof(struct Cyc_Absyn_Vardecl,sc),(struct _dynforward_ptr){_tmp470,_tmp470 + 3},(
void*)& Cyc_Absyn_scope_t_rep};static char _tmp471[5]="name";static struct _tuple5 Cyc__gentuple_176={
offsetof(struct Cyc_Absyn_Vardecl,name),(struct _dynforward_ptr){_tmp471,_tmp471 + 
5},(void*)& Cyc__genrep_11};static char _tmp472[3]="tq";static struct _tuple5 Cyc__gentuple_177={
offsetof(struct Cyc_Absyn_Vardecl,tq),(struct _dynforward_ptr){_tmp472,_tmp472 + 3},(
void*)& Cyc_Absyn_tqual_t_rep};static char _tmp473[5]="type";static struct _tuple5 Cyc__gentuple_178={
offsetof(struct Cyc_Absyn_Vardecl,type),(struct _dynforward_ptr){_tmp473,_tmp473 + 
5},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp474[12]="initializer";
static struct _tuple5 Cyc__gentuple_179={offsetof(struct Cyc_Absyn_Vardecl,initializer),(
struct _dynforward_ptr){_tmp474,_tmp474 + 12},(void*)& Cyc__genrep_146};static char
_tmp475[4]="rgn";static struct _tuple5 Cyc__gentuple_180={offsetof(struct Cyc_Absyn_Vardecl,rgn),(
struct _dynforward_ptr){_tmp475,_tmp475 + 4},(void*)& Cyc__genrep_110};static char
_tmp476[11]="attributes";static struct _tuple5 Cyc__gentuple_181={offsetof(struct
Cyc_Absyn_Vardecl,attributes),(struct _dynforward_ptr){_tmp476,_tmp476 + 11},(void*)&
Cyc__genrep_62};static char _tmp477[8]="escapes";static struct _tuple5 Cyc__gentuple_182={
offsetof(struct Cyc_Absyn_Vardecl,escapes),(struct _dynforward_ptr){_tmp477,
_tmp477 + 8},(void*)& Cyc__genrep_10};static struct _tuple5*Cyc__genarr_183[8]={& Cyc__gentuple_175,&
Cyc__gentuple_176,& Cyc__gentuple_177,& Cyc__gentuple_178,& Cyc__gentuple_179,& Cyc__gentuple_180,&
Cyc__gentuple_181,& Cyc__gentuple_182};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_184,sizeof(struct Cyc_Absyn_Vardecl),{(
void*)((struct _tuple5**)Cyc__genarr_183),(void*)((struct _tuple5**)Cyc__genarr_183
+ 8)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_159={1,1,(void*)((void*)&
Cyc_struct_Absyn_Vardecl_rep)};struct _tuple90{unsigned int f1;struct Cyc_Absyn_Vardecl*
f2;};static struct _tuple6 Cyc__gentuple_826={offsetof(struct _tuple90,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_827={offsetof(struct _tuple90,f2),(
void*)& Cyc__genrep_159};static struct _tuple6*Cyc__genarr_828[2]={& Cyc__gentuple_826,&
Cyc__gentuple_827};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_825={4,
sizeof(struct _tuple90),{(void*)((struct _tuple6**)Cyc__genarr_828),(void*)((
struct _tuple6**)Cyc__genarr_828 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1187;
struct _tuple91{unsigned int f1;struct Cyc_Absyn_Pat*f2;struct Cyc_Core_Opt*f3;
struct Cyc_Absyn_Exp*f4;};static struct _tuple6 Cyc__gentuple_1188={offsetof(struct
_tuple91,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1189={
offsetof(struct _tuple91,f2),(void*)& Cyc__genrep_254};static struct _tuple6 Cyc__gentuple_1190={
offsetof(struct _tuple91,f3),(void*)& Cyc__genrep_245};static struct _tuple6 Cyc__gentuple_1191={
offsetof(struct _tuple91,f4),(void*)& Cyc__genrep_223};static struct _tuple6*Cyc__genarr_1192[
4]={& Cyc__gentuple_1188,& Cyc__gentuple_1189,& Cyc__gentuple_1190,& Cyc__gentuple_1191};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1187={4,sizeof(struct _tuple91),{(
void*)((struct _tuple6**)Cyc__genarr_1192),(void*)((struct _tuple6**)Cyc__genarr_1192
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1183;static struct _tuple6
Cyc__gentuple_1184={offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_1185={offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_246};
static struct _tuple6*Cyc__genarr_1186[2]={& Cyc__gentuple_1184,& Cyc__gentuple_1185};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1183={4,sizeof(struct _tuple14),{(
void*)((struct _tuple6**)Cyc__genarr_1186),(void*)((struct _tuple6**)Cyc__genarr_1186
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1179;struct _tuple92{
unsigned int f1;struct Cyc_Absyn_Aggrdecl*f2;};static struct _tuple6 Cyc__gentuple_1180={
offsetof(struct _tuple92,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1181={
offsetof(struct _tuple92,f2),(void*)((void*)& Cyc__genrep_381)};static struct
_tuple6*Cyc__genarr_1182[2]={& Cyc__gentuple_1180,& Cyc__gentuple_1181};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1179={4,sizeof(struct _tuple92),{(void*)((
struct _tuple6**)Cyc__genarr_1182),(void*)((struct _tuple6**)Cyc__genarr_1182 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1175;struct _tuple93{unsigned int
f1;struct Cyc_Absyn_Tuniondecl*f2;};static struct _tuple6 Cyc__gentuple_1176={
offsetof(struct _tuple93,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1177={
offsetof(struct _tuple93,f2),(void*)((void*)& Cyc__genrep_325)};static struct
_tuple6*Cyc__genarr_1178[2]={& Cyc__gentuple_1176,& Cyc__gentuple_1177};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1175={4,sizeof(struct _tuple93),{(void*)((
struct _tuple6**)Cyc__genarr_1178),(void*)((struct _tuple6**)Cyc__genarr_1178 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1171;struct _tuple94{unsigned int
f1;struct Cyc_Absyn_Enumdecl*f2;};static struct _tuple6 Cyc__gentuple_1172={
offsetof(struct _tuple94,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1173={
offsetof(struct _tuple94,f2),(void*)& Cyc__genrep_279};static struct _tuple6*Cyc__genarr_1174[
2]={& Cyc__gentuple_1172,& Cyc__gentuple_1173};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1171={4,sizeof(struct _tuple94),{(void*)((struct _tuple6**)Cyc__genarr_1174),(
void*)((struct _tuple6**)Cyc__genarr_1174 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_60;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_61;static struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_61={1,1,(void*)((void*)& Cyc_struct_Absyn_Typedefdecl_rep)};
struct _tuple95{unsigned int f1;struct Cyc_Absyn_Typedefdecl*f2;};static struct
_tuple6 Cyc__gentuple_1168={offsetof(struct _tuple95,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1169={offsetof(struct _tuple95,f2),(void*)& Cyc__genrep_61};
static struct _tuple6*Cyc__genarr_1170[2]={& Cyc__gentuple_1168,& Cyc__gentuple_1169};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_60={4,sizeof(struct _tuple95),{(
void*)((struct _tuple6**)Cyc__genarr_1170),(void*)((struct _tuple6**)Cyc__genarr_1170
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_55;struct _tuple96{
unsigned int f1;struct _dynforward_ptr*f2;struct Cyc_List_List*f3;};static struct
_tuple6 Cyc__gentuple_56={offsetof(struct _tuple96,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_57={offsetof(struct _tuple96,f2),(void*)& Cyc__genrep_13};
static struct _tuple6 Cyc__gentuple_58={offsetof(struct _tuple96,f3),(void*)& Cyc__genrep_0};
static struct _tuple6*Cyc__genarr_59[3]={& Cyc__gentuple_56,& Cyc__gentuple_57,& Cyc__gentuple_58};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_55={4,sizeof(struct _tuple96),{(
void*)((struct _tuple6**)Cyc__genarr_59),(void*)((struct _tuple6**)Cyc__genarr_59 + 
3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_50;struct _tuple97{
unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;};static struct _tuple6 Cyc__gentuple_51={
offsetof(struct _tuple97,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_52={
offsetof(struct _tuple97,f2),(void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_53={
offsetof(struct _tuple97,f3),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_54[
3]={& Cyc__gentuple_51,& Cyc__gentuple_52,& Cyc__gentuple_53};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_50={4,sizeof(struct _tuple97),{(void*)((struct _tuple6**)Cyc__genarr_54),(
void*)((struct _tuple6**)Cyc__genarr_54 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_46;static struct _tuple6 Cyc__gentuple_47={offsetof(struct _tuple14,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_48={offsetof(struct
_tuple14,f2),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_49[2]={& Cyc__gentuple_47,&
Cyc__gentuple_48};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_46={4,sizeof(
struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_49),(void*)((struct _tuple6**)
Cyc__genarr_49 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_6;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_7;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep;extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_8;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_9;
struct _tuple98{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};static
struct _tuple6 Cyc__gentuple_34={offsetof(struct _tuple98,f1),(void*)& Cyc__genrep_2};
static struct _tuple6 Cyc__gentuple_35={offsetof(struct _tuple98,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_36={offsetof(struct _tuple98,f3),(void*)& Cyc__genrep_10};
static struct _tuple6*Cyc__genarr_37[3]={& Cyc__gentuple_34,& Cyc__gentuple_35,& Cyc__gentuple_36};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_9={4,sizeof(struct _tuple98),{(
void*)((struct _tuple6**)Cyc__genarr_37),(void*)((struct _tuple6**)Cyc__genarr_37 + 
3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_8={1,1,(void*)((void*)&
Cyc__genrep_9)};static char _tmp487[5]="List";static struct _dynforward_ptr Cyc__genname_41=(
struct _dynforward_ptr){_tmp487,_tmp487 + 5};static char _tmp488[3]="hd";static
struct _tuple5 Cyc__gentuple_38={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp488,_tmp488 + 3},(void*)& Cyc__genrep_8};static char _tmp489[3]="tl";
static struct _tuple5 Cyc__gentuple_39={offsetof(struct Cyc_List_List,tl),(struct
_dynforward_ptr){_tmp489,_tmp489 + 3},(void*)& Cyc__genrep_7};static struct _tuple5*
Cyc__genarr_40[2]={& Cyc__gentuple_38,& Cyc__gentuple_39};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_41,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_40),(void*)((struct _tuple5**)Cyc__genarr_40 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_7={1,1,(void*)((void*)& Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep)};
struct _tuple99{unsigned int f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static struct _tuple6 Cyc__gentuple_42={offsetof(struct _tuple99,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_43={offsetof(struct _tuple99,f2),(void*)& Cyc__genrep_0};
static struct _tuple6 Cyc__gentuple_44={offsetof(struct _tuple99,f3),(void*)& Cyc__genrep_7};
static struct _tuple6*Cyc__genarr_45[3]={& Cyc__gentuple_42,& Cyc__gentuple_43,& Cyc__gentuple_44};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_6={4,sizeof(struct _tuple99),{(
void*)((struct _tuple6**)Cyc__genarr_45),(void*)((struct _tuple6**)Cyc__genarr_45 + 
3)}};static char _tmp48D[9]="Porton_d";static struct _tuple7 Cyc__gentuple_3={0,(
struct _dynforward_ptr){_tmp48D,_tmp48D + 9}};static char _tmp48E[10]="Portoff_d";
static struct _tuple7 Cyc__gentuple_4={1,(struct _dynforward_ptr){_tmp48E,_tmp48E + 
10}};static struct _tuple7*Cyc__genarr_5[2]={& Cyc__gentuple_3,& Cyc__gentuple_4};
static char _tmp48F[6]="Var_d";static struct _tuple5 Cyc__gentuple_1193={0,(struct
_dynforward_ptr){_tmp48F,_tmp48F + 6},(void*)& Cyc__genrep_825};static char _tmp490[
5]="Fn_d";static struct _tuple5 Cyc__gentuple_1194={1,(struct _dynforward_ptr){
_tmp490,_tmp490 + 5},(void*)& Cyc__genrep_829};static char _tmp491[6]="Let_d";static
struct _tuple5 Cyc__gentuple_1195={2,(struct _dynforward_ptr){_tmp491,_tmp491 + 6},(
void*)& Cyc__genrep_1187};static char _tmp492[7]="Letv_d";static struct _tuple5 Cyc__gentuple_1196={
3,(struct _dynforward_ptr){_tmp492,_tmp492 + 7},(void*)& Cyc__genrep_1183};static
char _tmp493[7]="Aggr_d";static struct _tuple5 Cyc__gentuple_1197={4,(struct
_dynforward_ptr){_tmp493,_tmp493 + 7},(void*)& Cyc__genrep_1179};static char _tmp494[
9]="Tunion_d";static struct _tuple5 Cyc__gentuple_1198={5,(struct _dynforward_ptr){
_tmp494,_tmp494 + 9},(void*)& Cyc__genrep_1175};static char _tmp495[7]="Enum_d";
static struct _tuple5 Cyc__gentuple_1199={6,(struct _dynforward_ptr){_tmp495,_tmp495
+ 7},(void*)& Cyc__genrep_1171};static char _tmp496[10]="Typedef_d";static struct
_tuple5 Cyc__gentuple_1200={7,(struct _dynforward_ptr){_tmp496,_tmp496 + 10},(void*)&
Cyc__genrep_60};static char _tmp497[12]="Namespace_d";static struct _tuple5 Cyc__gentuple_1201={
8,(struct _dynforward_ptr){_tmp497,_tmp497 + 12},(void*)& Cyc__genrep_55};static
char _tmp498[8]="Using_d";static struct _tuple5 Cyc__gentuple_1202={9,(struct
_dynforward_ptr){_tmp498,_tmp498 + 8},(void*)& Cyc__genrep_50};static char _tmp499[
10]="ExternC_d";static struct _tuple5 Cyc__gentuple_1203={10,(struct _dynforward_ptr){
_tmp499,_tmp499 + 10},(void*)& Cyc__genrep_46};static char _tmp49A[17]="ExternCinclude_d";
static struct _tuple5 Cyc__gentuple_1204={11,(struct _dynforward_ptr){_tmp49A,
_tmp49A + 17},(void*)& Cyc__genrep_6};static struct _tuple5*Cyc__genarr_1205[12]={&
Cyc__gentuple_1193,& Cyc__gentuple_1194,& Cyc__gentuple_1195,& Cyc__gentuple_1196,&
Cyc__gentuple_1197,& Cyc__gentuple_1198,& Cyc__gentuple_1199,& Cyc__gentuple_1200,&
Cyc__gentuple_1201,& Cyc__gentuple_1202,& Cyc__gentuple_1203,& Cyc__gentuple_1204};
static char _tmp49C[9]="Raw_decl";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep={
5,(struct _dynforward_ptr){_tmp49C,_tmp49C + 9},{(void*)((struct _tuple7**)Cyc__genarr_5),(
void*)((struct _tuple7**)Cyc__genarr_5 + 2)},{(void*)((struct _tuple5**)Cyc__genarr_1205),(
void*)((struct _tuple5**)Cyc__genarr_1205 + 12)}};static char _tmp49D[5]="Decl";
static struct _dynforward_ptr Cyc__genname_1209=(struct _dynforward_ptr){_tmp49D,
_tmp49D + 5};static char _tmp49E[2]="r";static struct _tuple5 Cyc__gentuple_1206={
offsetof(struct Cyc_Absyn_Decl,r),(struct _dynforward_ptr){_tmp49E,_tmp49E + 2},(
void*)& Cyc_Absyn_raw_decl_t_rep};static char _tmp49F[4]="loc";static struct _tuple5
Cyc__gentuple_1207={offsetof(struct Cyc_Absyn_Decl,loc),(struct _dynforward_ptr){
_tmp49F,_tmp49F + 4},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_1208[
2]={& Cyc__gentuple_1206,& Cyc__gentuple_1207};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Decl_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_1209,sizeof(struct Cyc_Absyn_Decl),{(void*)((
struct _tuple5**)Cyc__genarr_1208),(void*)((struct _tuple5**)Cyc__genarr_1208 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1={1,1,(void*)((void*)& Cyc_struct_Absyn_Decl_rep)};
static char _tmp4A2[5]="List";static struct _dynforward_ptr Cyc__genname_1213=(struct
_dynforward_ptr){_tmp4A2,_tmp4A2 + 5};static char _tmp4A3[3]="hd";static struct
_tuple5 Cyc__gentuple_1210={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp4A3,_tmp4A3 + 3},(void*)& Cyc__genrep_1};static char _tmp4A4[3]="tl";
static struct _tuple5 Cyc__gentuple_1211={offsetof(struct Cyc_List_List,tl),(struct
_dynforward_ptr){_tmp4A4,_tmp4A4 + 3},(void*)& Cyc__genrep_0};static struct _tuple5*
Cyc__genarr_1212[2]={& Cyc__gentuple_1210,& Cyc__gentuple_1211};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_decl_t46H2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_1213,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_1212),(void*)((
struct _tuple5**)Cyc__genarr_1212 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_0={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_decl_t46H2_rep)};void*Cyc_decls_rep=(
void*)& Cyc__genrep_0;
