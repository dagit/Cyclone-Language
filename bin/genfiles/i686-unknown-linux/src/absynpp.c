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
void*v;};struct _dynforward_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[
16];typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t
Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,
struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*
x);int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2);int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2);extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*
f);struct _dynforward_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);struct Cyc_PP_Doc*
Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_blank_doc();struct Cyc_PP_Doc*Cyc_PP_line_doc();
struct Cyc_PP_Doc*Cyc_PP_text(struct _dynforward_ptr s);struct Cyc_PP_Doc*Cyc_PP_textptr(
struct _dynforward_ptr*p);struct Cyc_PP_Doc*Cyc_PP_text_width(struct
_dynforward_ptr s,int w);struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dynforward_ptr);struct Cyc_PP_Doc*Cyc_PP_seq(
struct _dynforward_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_ppseq(
struct Cyc_PP_Doc*(*pp)(void*),struct _dynforward_ptr sep,struct Cyc_List_List*l);
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dynforward_ptr sep,struct Cyc_List_List*l0);
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct
_dynforward_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_group(struct
_dynforward_ptr start,struct _dynforward_ptr stop,struct _dynforward_ptr sep,struct
Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dynforward_ptr start,
struct _dynforward_ptr stop,struct _dynforward_ptr sep,struct Cyc_List_List*l);struct
Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct _dynforward_ptr line;int
line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _dynforward_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple0{union Cyc_Absyn_Nmspace_union
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_bounds_one;struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct _dynforward_ptr Cyc_Absyn_attribute2string(void*);struct _tuple3{void*f1;
struct _tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU_union);
struct Cyc_Buffer_t;unsigned int Cyc_strlen(struct _dynforward_ptr s);int Cyc_strptrcmp(
struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct _dynforward_ptr Cyc_strconcat(
struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr Cyc_str_sepstr(
struct Cyc_List_List*,struct _dynforward_ptr);struct _tuple4{unsigned int f1;int f2;}
;struct _tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple5{void*f1;void*f2;};struct _tuple5*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple5*Cyc_Dict_rchoose(
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
le;};void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};extern int Cyc_Absynpp_print_scopes;
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params
Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_tc_params_r;void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,
struct Cyc___cycFILE*f);struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*
d);struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct _dynforward_ptr Cyc_Absynpp_typ2cstring(
void*);struct _dynforward_ptr Cyc_Absynpp_kind2string(void*);struct _dynforward_ptr
Cyc_Absynpp_kindbound2string(void*);struct _dynforward_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dynforward_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct
_dynforward_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct
_dynforward_ptr Cyc_Absynpp_prim2string(void*p);struct _dynforward_ptr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat*p);struct _dynforward_ptr Cyc_Absynpp_scope2string(void*sc);
int Cyc_Absynpp_is_anon_aggrtype(void*t);extern struct _dynforward_ptr Cyc_Absynpp_cyc_string;
extern struct _dynforward_ptr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*);struct _dynforward_ptr Cyc_Absynpp_char_escape(char);struct
_dynforward_ptr Cyc_Absynpp_string_escape(struct _dynforward_ptr);struct
_dynforward_ptr Cyc_Absynpp_prim2str(void*p);int Cyc_Absynpp_is_declaration(struct
Cyc_Absyn_Stmt*s);struct _tuple6{struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;
void*f3;};struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple6*arg);struct _tuple7{
struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};struct _tuple7 Cyc_Absynpp_to_tms(
struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);extern void*Cyc_Cyclone_c_compiler;
struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_PP_Doc*
Cyc_Absynpp_dp2doc(struct _tuple8*dp);struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List*cs);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
void*);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*);struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int
inprec,struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct
_tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*);struct
Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst_union);struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
void*);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*
es);struct _tuple9{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_PP_Doc*
Cyc_Absynpp_de2doc(struct _tuple9*de);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*dopt);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct
Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*
vd);static int Cyc_Absynpp_expand_typedefs;static int Cyc_Absynpp_qvar_to_Cids;
static char _tmp0[4]="Cyc";struct _dynforward_ptr Cyc_Absynpp_cyc_string={_tmp0,
_tmp0 + 4};struct _dynforward_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
static int Cyc_Absynpp_add_cyc_prefix;static int Cyc_Absynpp_to_VC;static int Cyc_Absynpp_decls_first;
static int Cyc_Absynpp_rewrite_temp_tvars;static int Cyc_Absynpp_print_all_tvars;
static int Cyc_Absynpp_print_all_kinds;static int Cyc_Absynpp_print_all_effects;
static int Cyc_Absynpp_print_using_stmts;static int Cyc_Absynpp_print_externC_stmts;
static int Cyc_Absynpp_print_full_evars;static int Cyc_Absynpp_generate_line_directives;
static int Cyc_Absynpp_use_curr_namespace;static int Cyc_Absynpp_print_zeroterm;
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;struct Cyc_Absynpp_Params;
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs){Cyc_Absynpp_expand_typedefs=
fs->expand_typedefs;Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
fs->add_cyc_prefix;Cyc_Absynpp_to_VC=fs->to_VC;Cyc_Absynpp_decls_first=fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;Cyc_Absynpp_print_all_tvars=
fs->print_all_tvars;Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;Cyc_Absynpp_print_all_effects=
fs->print_all_effects;Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;Cyc_Absynpp_print_externC_stmts=
fs->print_externC_stmts;Cyc_Absynpp_print_full_evars=fs->print_full_evars;Cyc_Absynpp_print_zeroterm=
fs->print_zeroterm;Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;Cyc_Absynpp_curr_namespace=
fs->curr_namespace;}struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,
1,0,0,0,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,0,
0,0,1,0,0,1,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,
0,1,0,0,0,0,0,0,0,0,0,0,0};struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,
0,0,0,0,0,0,0,1,1,0,1,0,0,0};static void Cyc_Absynpp_curr_namespace_add(struct
_dynforward_ptr*v){Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,({
struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v;_tmp1->tl=0;
_tmp1;}));}static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){if(((struct
Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;else{Cyc_Absynpp_suppr_last(&((
struct Cyc_List_List*)_check_null(*l))->tl);}}static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _dynforward_ptr Cyc_Absynpp_char_escape(char c){switch(c){case '\a': _LL0:
return({const char*_tmp2="\\a";_tag_dynforward(_tmp2,sizeof(char),
_get_zero_arr_size_char(_tmp2,3));});case '\b': _LL1: return({const char*_tmp3="\\b";
_tag_dynforward(_tmp3,sizeof(char),_get_zero_arr_size_char(_tmp3,3));});case '\f':
_LL2: return({const char*_tmp4="\\f";_tag_dynforward(_tmp4,sizeof(char),
_get_zero_arr_size_char(_tmp4,3));});case '\n': _LL3: return({const char*_tmp5="\\n";
_tag_dynforward(_tmp5,sizeof(char),_get_zero_arr_size_char(_tmp5,3));});case '\r':
_LL4: return({const char*_tmp6="\\r";_tag_dynforward(_tmp6,sizeof(char),
_get_zero_arr_size_char(_tmp6,3));});case '\t': _LL5: return({const char*_tmp7="\\t";
_tag_dynforward(_tmp7,sizeof(char),_get_zero_arr_size_char(_tmp7,3));});case '\v':
_LL6: return({const char*_tmp8="\\v";_tag_dynforward(_tmp8,sizeof(char),
_get_zero_arr_size_char(_tmp8,3));});case '\\': _LL7: return({const char*_tmp9="\\\\";
_tag_dynforward(_tmp9,sizeof(char),_get_zero_arr_size_char(_tmp9,3));});case '"':
_LL8: return({const char*_tmpA="\"";_tag_dynforward(_tmpA,sizeof(char),
_get_zero_arr_size_char(_tmpA,2));});case '\'': _LL9: return({const char*_tmpB="\\'";
_tag_dynforward(_tmpB,sizeof(char),_get_zero_arr_size_char(_tmpB,3));});default:
_LLA: if(c >= ' '  && c <= '~'){struct _dynforward_ptr _tmpC=Cyc_Core_new_string(2);({
struct _dynforward_ptr _tmpD=_dynforward_ptr_plus(_tmpC,sizeof(char),0);char _tmpE=*((
char*)_check_dynforward_subscript(_tmpD,sizeof(char),0));char _tmpF=c;if(
_get_dynforward_size(_tmpD,sizeof(char))== 1  && (_tmpE == '\000'  && _tmpF != '\000'))
_throw_arraybounds();*((char*)_tmpD.curr)=_tmpF;});return(struct _dynforward_ptr)
_tmpC;}else{struct _dynforward_ptr _tmp10=Cyc_Core_new_string(5);int j=0;({struct
_dynforward_ptr _tmp11=_dynforward_ptr_plus(_tmp10,sizeof(char),j ++);char _tmp12=*((
char*)_check_dynforward_subscript(_tmp11,sizeof(char),0));char _tmp13='\\';if(
_get_dynforward_size(_tmp11,sizeof(char))== 1  && (_tmp12 == '\000'  && _tmp13 != '\000'))
_throw_arraybounds();*((char*)_tmp11.curr)=_tmp13;});({struct _dynforward_ptr
_tmp14=_dynforward_ptr_plus(_tmp10,sizeof(char),j ++);char _tmp15=*((char*)
_check_dynforward_subscript(_tmp14,sizeof(char),0));char _tmp16=(char)('0' + ((
unsigned char)c >> 6 & 3));if(_get_dynforward_size(_tmp14,sizeof(char))== 1  && (
_tmp15 == '\000'  && _tmp16 != '\000'))_throw_arraybounds();*((char*)_tmp14.curr)=
_tmp16;});({struct _dynforward_ptr _tmp17=_dynforward_ptr_plus(_tmp10,sizeof(char),
j ++);char _tmp18=*((char*)_check_dynforward_subscript(_tmp17,sizeof(char),0));
char _tmp19=(char)('0' + (c >> 3 & 7));if(_get_dynforward_size(_tmp17,sizeof(char))
== 1  && (_tmp18 == '\000'  && _tmp19 != '\000'))_throw_arraybounds();*((char*)
_tmp17.curr)=_tmp19;});({struct _dynforward_ptr _tmp1A=_dynforward_ptr_plus(_tmp10,
sizeof(char),j ++);char _tmp1B=*((char*)_check_dynforward_subscript(_tmp1A,sizeof(
char),0));char _tmp1C=(char)('0' + (c & 7));if(_get_dynforward_size(_tmp1A,sizeof(
char))== 1  && (_tmp1B == '\000'  && _tmp1C != '\000'))_throw_arraybounds();*((char*)
_tmp1A.curr)=_tmp1C;});return(struct _dynforward_ptr)_tmp10;}}}static int Cyc_Absynpp_special(
struct _dynforward_ptr s){int sz=(int)(_get_dynforward_size(s,sizeof(char))- 1);{
int i=0;for(0;i < sz;i ++){char c=*((const char*)_check_dynforward_subscript(s,
sizeof(char),i));if(((c <= ' '  || c >= '~') || c == '"') || c == '\\')return 1;}}
return 0;}struct _dynforward_ptr Cyc_Absynpp_string_escape(struct _dynforward_ptr s){
if(!Cyc_Absynpp_special(s))return s;{int n=(int)(_get_dynforward_size(s,sizeof(
char))- 1);if(n > 0  && *((const char*)_check_dynforward_subscript(s,sizeof(char),n))
== '\000')n --;{int len=0;{int i=0;for(0;i <= n;i ++){char _tmp1D=*((const char*)
_check_dynforward_subscript(s,sizeof(char),i));_LLD: if(_tmp1D != '\a')goto _LLF;
_LLE: goto _LL10;_LLF: if(_tmp1D != '\b')goto _LL11;_LL10: goto _LL12;_LL11: if(_tmp1D != '\f')
goto _LL13;_LL12: goto _LL14;_LL13: if(_tmp1D != '\n')goto _LL15;_LL14: goto _LL16;_LL15:
if(_tmp1D != '\r')goto _LL17;_LL16: goto _LL18;_LL17: if(_tmp1D != '\t')goto _LL19;
_LL18: goto _LL1A;_LL19: if(_tmp1D != '\v')goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp1D
!= '\\')goto _LL1D;_LL1C: goto _LL1E;_LL1D: if(_tmp1D != '"')goto _LL1F;_LL1E: len +=2;
goto _LLC;_LL1F:;_LL20: if(_tmp1D >= ' '  && _tmp1D <= '~')len ++;else{len +=4;}goto
_LLC;_LLC:;}}{struct _dynforward_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));
int j=0;{int i=0;for(0;i <= n;i ++){char _tmp1E=*((const char*)
_check_dynforward_subscript(s,sizeof(char),i));_LL22: if(_tmp1E != '\a')goto _LL24;
_LL23:({struct _dynforward_ptr _tmp1F=_dynforward_ptr_plus(t,sizeof(char),j ++);
char _tmp20=*((char*)_check_dynforward_subscript(_tmp1F,sizeof(char),0));char
_tmp21='\\';if(_get_dynforward_size(_tmp1F,sizeof(char))== 1  && (_tmp20 == '\000'
 && _tmp21 != '\000'))_throw_arraybounds();*((char*)_tmp1F.curr)=_tmp21;});({
struct _dynforward_ptr _tmp22=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp23=*((
char*)_check_dynforward_subscript(_tmp22,sizeof(char),0));char _tmp24='a';if(
_get_dynforward_size(_tmp22,sizeof(char))== 1  && (_tmp23 == '\000'  && _tmp24 != '\000'))
_throw_arraybounds();*((char*)_tmp22.curr)=_tmp24;});goto _LL21;_LL24: if(_tmp1E != '\b')
goto _LL26;_LL25:({struct _dynforward_ptr _tmp25=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp26=*((char*)_check_dynforward_subscript(_tmp25,sizeof(char),0));
char _tmp27='\\';if(_get_dynforward_size(_tmp25,sizeof(char))== 1  && (_tmp26 == '\000'
 && _tmp27 != '\000'))_throw_arraybounds();*((char*)_tmp25.curr)=_tmp27;});({
struct _dynforward_ptr _tmp28=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp29=*((
char*)_check_dynforward_subscript(_tmp28,sizeof(char),0));char _tmp2A='b';if(
_get_dynforward_size(_tmp28,sizeof(char))== 1  && (_tmp29 == '\000'  && _tmp2A != '\000'))
_throw_arraybounds();*((char*)_tmp28.curr)=_tmp2A;});goto _LL21;_LL26: if(_tmp1E != '\f')
goto _LL28;_LL27:({struct _dynforward_ptr _tmp2B=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp2C=*((char*)_check_dynforward_subscript(_tmp2B,sizeof(char),0));
char _tmp2D='\\';if(_get_dynforward_size(_tmp2B,sizeof(char))== 1  && (_tmp2C == '\000'
 && _tmp2D != '\000'))_throw_arraybounds();*((char*)_tmp2B.curr)=_tmp2D;});({
struct _dynforward_ptr _tmp2E=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp2F=*((
char*)_check_dynforward_subscript(_tmp2E,sizeof(char),0));char _tmp30='f';if(
_get_dynforward_size(_tmp2E,sizeof(char))== 1  && (_tmp2F == '\000'  && _tmp30 != '\000'))
_throw_arraybounds();*((char*)_tmp2E.curr)=_tmp30;});goto _LL21;_LL28: if(_tmp1E != '\n')
goto _LL2A;_LL29:({struct _dynforward_ptr _tmp31=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp32=*((char*)_check_dynforward_subscript(_tmp31,sizeof(char),0));
char _tmp33='\\';if(_get_dynforward_size(_tmp31,sizeof(char))== 1  && (_tmp32 == '\000'
 && _tmp33 != '\000'))_throw_arraybounds();*((char*)_tmp31.curr)=_tmp33;});({
struct _dynforward_ptr _tmp34=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp35=*((
char*)_check_dynforward_subscript(_tmp34,sizeof(char),0));char _tmp36='n';if(
_get_dynforward_size(_tmp34,sizeof(char))== 1  && (_tmp35 == '\000'  && _tmp36 != '\000'))
_throw_arraybounds();*((char*)_tmp34.curr)=_tmp36;});goto _LL21;_LL2A: if(_tmp1E != '\r')
goto _LL2C;_LL2B:({struct _dynforward_ptr _tmp37=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp38=*((char*)_check_dynforward_subscript(_tmp37,sizeof(char),0));
char _tmp39='\\';if(_get_dynforward_size(_tmp37,sizeof(char))== 1  && (_tmp38 == '\000'
 && _tmp39 != '\000'))_throw_arraybounds();*((char*)_tmp37.curr)=_tmp39;});({
struct _dynforward_ptr _tmp3A=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp3B=*((
char*)_check_dynforward_subscript(_tmp3A,sizeof(char),0));char _tmp3C='r';if(
_get_dynforward_size(_tmp3A,sizeof(char))== 1  && (_tmp3B == '\000'  && _tmp3C != '\000'))
_throw_arraybounds();*((char*)_tmp3A.curr)=_tmp3C;});goto _LL21;_LL2C: if(_tmp1E != '\t')
goto _LL2E;_LL2D:({struct _dynforward_ptr _tmp3D=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp3E=*((char*)_check_dynforward_subscript(_tmp3D,sizeof(char),0));
char _tmp3F='\\';if(_get_dynforward_size(_tmp3D,sizeof(char))== 1  && (_tmp3E == '\000'
 && _tmp3F != '\000'))_throw_arraybounds();*((char*)_tmp3D.curr)=_tmp3F;});({
struct _dynforward_ptr _tmp40=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp41=*((
char*)_check_dynforward_subscript(_tmp40,sizeof(char),0));char _tmp42='t';if(
_get_dynforward_size(_tmp40,sizeof(char))== 1  && (_tmp41 == '\000'  && _tmp42 != '\000'))
_throw_arraybounds();*((char*)_tmp40.curr)=_tmp42;});goto _LL21;_LL2E: if(_tmp1E != '\v')
goto _LL30;_LL2F:({struct _dynforward_ptr _tmp43=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp44=*((char*)_check_dynforward_subscript(_tmp43,sizeof(char),0));
char _tmp45='\\';if(_get_dynforward_size(_tmp43,sizeof(char))== 1  && (_tmp44 == '\000'
 && _tmp45 != '\000'))_throw_arraybounds();*((char*)_tmp43.curr)=_tmp45;});({
struct _dynforward_ptr _tmp46=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp47=*((
char*)_check_dynforward_subscript(_tmp46,sizeof(char),0));char _tmp48='v';if(
_get_dynforward_size(_tmp46,sizeof(char))== 1  && (_tmp47 == '\000'  && _tmp48 != '\000'))
_throw_arraybounds();*((char*)_tmp46.curr)=_tmp48;});goto _LL21;_LL30: if(_tmp1E != '\\')
goto _LL32;_LL31:({struct _dynforward_ptr _tmp49=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp4A=*((char*)_check_dynforward_subscript(_tmp49,sizeof(char),0));
char _tmp4B='\\';if(_get_dynforward_size(_tmp49,sizeof(char))== 1  && (_tmp4A == '\000'
 && _tmp4B != '\000'))_throw_arraybounds();*((char*)_tmp49.curr)=_tmp4B;});({
struct _dynforward_ptr _tmp4C=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp4D=*((
char*)_check_dynforward_subscript(_tmp4C,sizeof(char),0));char _tmp4E='\\';if(
_get_dynforward_size(_tmp4C,sizeof(char))== 1  && (_tmp4D == '\000'  && _tmp4E != '\000'))
_throw_arraybounds();*((char*)_tmp4C.curr)=_tmp4E;});goto _LL21;_LL32: if(_tmp1E != '"')
goto _LL34;_LL33:({struct _dynforward_ptr _tmp4F=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp50=*((char*)_check_dynforward_subscript(_tmp4F,sizeof(char),0));
char _tmp51='\\';if(_get_dynforward_size(_tmp4F,sizeof(char))== 1  && (_tmp50 == '\000'
 && _tmp51 != '\000'))_throw_arraybounds();*((char*)_tmp4F.curr)=_tmp51;});({
struct _dynforward_ptr _tmp52=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp53=*((
char*)_check_dynforward_subscript(_tmp52,sizeof(char),0));char _tmp54='"';if(
_get_dynforward_size(_tmp52,sizeof(char))== 1  && (_tmp53 == '\000'  && _tmp54 != '\000'))
_throw_arraybounds();*((char*)_tmp52.curr)=_tmp54;});goto _LL21;_LL34:;_LL35: if(
_tmp1E >= ' '  && _tmp1E <= '~')({struct _dynforward_ptr _tmp55=_dynforward_ptr_plus(
t,sizeof(char),j ++);char _tmp56=*((char*)_check_dynforward_subscript(_tmp55,
sizeof(char),0));char _tmp57=_tmp1E;if(_get_dynforward_size(_tmp55,sizeof(char))
== 1  && (_tmp56 == '\000'  && _tmp57 != '\000'))_throw_arraybounds();*((char*)
_tmp55.curr)=_tmp57;});else{({struct _dynforward_ptr _tmp58=_dynforward_ptr_plus(t,
sizeof(char),j ++);char _tmp59=*((char*)_check_dynforward_subscript(_tmp58,sizeof(
char),0));char _tmp5A='\\';if(_get_dynforward_size(_tmp58,sizeof(char))== 1  && (
_tmp59 == '\000'  && _tmp5A != '\000'))_throw_arraybounds();*((char*)_tmp58.curr)=
_tmp5A;});({struct _dynforward_ptr _tmp5B=_dynforward_ptr_plus(t,sizeof(char),j ++);
char _tmp5C=*((char*)_check_dynforward_subscript(_tmp5B,sizeof(char),0));char
_tmp5D=(char)('0' + (_tmp1E >> 6 & 7));if(_get_dynforward_size(_tmp5B,sizeof(char))
== 1  && (_tmp5C == '\000'  && _tmp5D != '\000'))_throw_arraybounds();*((char*)
_tmp5B.curr)=_tmp5D;});({struct _dynforward_ptr _tmp5E=_dynforward_ptr_plus(t,
sizeof(char),j ++);char _tmp5F=*((char*)_check_dynforward_subscript(_tmp5E,sizeof(
char),0));char _tmp60=(char)('0' + (_tmp1E >> 3 & 7));if(_get_dynforward_size(_tmp5E,
sizeof(char))== 1  && (_tmp5F == '\000'  && _tmp60 != '\000'))_throw_arraybounds();*((
char*)_tmp5E.curr)=_tmp60;});({struct _dynforward_ptr _tmp61=_dynforward_ptr_plus(
t,sizeof(char),j ++);char _tmp62=*((char*)_check_dynforward_subscript(_tmp61,
sizeof(char),0));char _tmp63=(char)('0' + (_tmp1E & 7));if(_get_dynforward_size(
_tmp61,sizeof(char))== 1  && (_tmp62 == '\000'  && _tmp63 != '\000'))
_throw_arraybounds();*((char*)_tmp61.curr)=_tmp63;});}goto _LL21;_LL21:;}}return(
struct _dynforward_ptr)t;}}}}static char _tmp64[9]="restrict";static struct
_dynforward_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64 + 9};static char _tmp65[9]="volatile";
static struct _dynforward_ptr Cyc_Absynpp_volatile_string={_tmp65,_tmp65 + 9};static
char _tmp66[6]="const";static struct _dynforward_ptr Cyc_Absynpp_const_str={_tmp66,
_tmp66 + 6};static struct _dynforward_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _dynforward_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _dynforward_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;struct
Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=
0;if(tq.q_restrict)l=({struct Cyc_List_List*_tmp67=_cycalloc(sizeof(*_tmp67));
_tmp67->hd=Cyc_Absynpp_restrict_sp;_tmp67->tl=l;_tmp67;});if(tq.q_volatile)l=({
struct Cyc_List_List*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->hd=Cyc_Absynpp_volatile_sp;
_tmp68->tl=l;_tmp68;});if(tq.print_const)l=({struct Cyc_List_List*_tmp69=
_cycalloc(sizeof(*_tmp69));_tmp69->hd=Cyc_Absynpp_const_sp;_tmp69->tl=l;_tmp69;});
return Cyc_PP_egroup(({const char*_tmp6A="";_tag_dynforward(_tmp6A,sizeof(char),
_get_zero_arr_size_char(_tmp6A,1));}),({const char*_tmp6B=" ";_tag_dynforward(
_tmp6B,sizeof(char),_get_zero_arr_size_char(_tmp6B,2));}),({const char*_tmp6C=" ";
_tag_dynforward(_tmp6C,sizeof(char),_get_zero_arr_size_char(_tmp6C,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,l));}struct _dynforward_ptr Cyc_Absynpp_kind2string(
void*k){void*_tmp6D=k;_LL37: if((int)_tmp6D != 0)goto _LL39;_LL38: return({const char*
_tmp6E="A";_tag_dynforward(_tmp6E,sizeof(char),_get_zero_arr_size_char(_tmp6E,2));});
_LL39: if((int)_tmp6D != 1)goto _LL3B;_LL3A: return({const char*_tmp6F="M";
_tag_dynforward(_tmp6F,sizeof(char),_get_zero_arr_size_char(_tmp6F,2));});_LL3B:
if((int)_tmp6D != 2)goto _LL3D;_LL3C: return({const char*_tmp70="B";_tag_dynforward(
_tmp70,sizeof(char),_get_zero_arr_size_char(_tmp70,2));});_LL3D: if((int)_tmp6D != 
3)goto _LL3F;_LL3E: return({const char*_tmp71="R";_tag_dynforward(_tmp71,sizeof(
char),_get_zero_arr_size_char(_tmp71,2));});_LL3F: if((int)_tmp6D != 4)goto _LL41;
_LL40: return({const char*_tmp72="UR";_tag_dynforward(_tmp72,sizeof(char),
_get_zero_arr_size_char(_tmp72,3));});_LL41: if((int)_tmp6D != 5)goto _LL43;_LL42:
return({const char*_tmp73="TR";_tag_dynforward(_tmp73,sizeof(char),
_get_zero_arr_size_char(_tmp73,3));});_LL43: if((int)_tmp6D != 6)goto _LL45;_LL44:
return({const char*_tmp74="E";_tag_dynforward(_tmp74,sizeof(char),
_get_zero_arr_size_char(_tmp74,2));});_LL45: if((int)_tmp6D != 7)goto _LL36;_LL46:
return({const char*_tmp75="I";_tag_dynforward(_tmp75,sizeof(char),
_get_zero_arr_size_char(_tmp75,2));});_LL36:;}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(
void*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}struct _dynforward_ptr Cyc_Absynpp_kindbound2string(
void*c){void*_tmp76=Cyc_Absyn_compress_kb(c);void*_tmp77;void*_tmp78;_LL48: if(*((
int*)_tmp76)!= 0)goto _LL4A;_tmp77=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp76)->f1;
_LL49: return Cyc_Absynpp_kind2string(_tmp77);_LL4A: if(*((int*)_tmp76)!= 1)goto
_LL4C;_LL4B: if(Cyc_PP_tex_output)return({const char*_tmp79="{?}";_tag_dynforward(
_tmp79,sizeof(char),_get_zero_arr_size_char(_tmp79,4));});else{return({const char*
_tmp7A="?";_tag_dynforward(_tmp7A,sizeof(char),_get_zero_arr_size_char(_tmp7A,2));});}
_LL4C: if(*((int*)_tmp76)!= 2)goto _LL47;_tmp78=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp76)->f2;_LL4D: return Cyc_Absynpp_kind2string(_tmp78);_LL47:;}struct Cyc_PP_Doc*
Cyc_Absynpp_kindbound2doc(void*c){void*_tmp7B=Cyc_Absyn_compress_kb(c);void*
_tmp7C;void*_tmp7D;_LL4F: if(*((int*)_tmp7B)!= 0)goto _LL51;_tmp7C=(void*)((struct
Cyc_Absyn_Eq_kb_struct*)_tmp7B)->f1;_LL50: return Cyc_PP_text(Cyc_Absynpp_kind2string(
_tmp7C));_LL51: if(*((int*)_tmp7B)!= 1)goto _LL53;_LL52: if(Cyc_PP_tex_output)
return Cyc_PP_text_width(({const char*_tmp7E="{?}";_tag_dynforward(_tmp7E,sizeof(
char),_get_zero_arr_size_char(_tmp7E,4));}),1);else{return Cyc_PP_text(({const
char*_tmp7F="?";_tag_dynforward(_tmp7F,sizeof(char),_get_zero_arr_size_char(
_tmp7F,2));}));}_LL53: if(*((int*)_tmp7B)!= 2)goto _LL4E;_tmp7D=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp7B)->f2;_LL54: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7D));_LL4E:;}
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){return Cyc_PP_egroup(({
const char*_tmp80="<";_tag_dynforward(_tmp80,sizeof(char),_get_zero_arr_size_char(
_tmp80,2));}),({const char*_tmp81=">";_tag_dynforward(_tmp81,sizeof(char),
_get_zero_arr_size_char(_tmp81,2));}),({const char*_tmp82=",";_tag_dynforward(
_tmp82,sizeof(char),_get_zero_arr_size_char(_tmp82,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){void*
_tmp83=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmp84;void*_tmp85;void*
_tmp86;_LL56: if(*((int*)_tmp83)!= 1)goto _LL58;_LL57: goto _LL59;_LL58: if(*((int*)
_tmp83)!= 0)goto _LL5A;_tmp84=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp83)->f1;
if((int)_tmp84 != 2)goto _LL5A;_LL59: return Cyc_PP_textptr(tv->name);_LL5A: if(*((
int*)_tmp83)!= 2)goto _LL5C;_tmp85=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp83)->f2;_LL5B: _tmp86=_tmp85;goto _LL5D;_LL5C: if(*((int*)_tmp83)!= 0)goto _LL55;
_tmp86=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp83)->f1;_LL5D: return({struct
Cyc_PP_Doc*_tmp87[3];_tmp87[2]=Cyc_Absynpp_kind2doc(_tmp86);_tmp87[1]=Cyc_PP_text(({
const char*_tmp88="::";_tag_dynforward(_tmp88,sizeof(char),
_get_zero_arr_size_char(_tmp88,3));}));_tmp87[0]=Cyc_PP_textptr(tv->name);Cyc_PP_cat(
_tag_dynforward(_tmp87,sizeof(struct Cyc_PP_Doc*),3));});_LL55:;}struct Cyc_PP_Doc*
Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){return Cyc_PP_egroup(({const char*
_tmp89="<";_tag_dynforward(_tmp89,sizeof(char),_get_zero_arr_size_char(_tmp89,2));}),({
const char*_tmp8A=">";_tag_dynforward(_tmp8A,sizeof(char),_get_zero_arr_size_char(
_tmp8A,2));}),({const char*_tmp8B=",";_tag_dynforward(_tmp8B,sizeof(char),
_get_zero_arr_size_char(_tmp8B,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,
tvs));}static struct _dynforward_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){
return tv->name;}struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)return Cyc_Absynpp_ktvars2doc(tvs);return Cyc_PP_egroup(({
const char*_tmp8C="<";_tag_dynforward(_tmp8C,sizeof(char),_get_zero_arr_size_char(
_tmp8C,2));}),({const char*_tmp8D=">";_tag_dynforward(_tmp8D,sizeof(char),
_get_zero_arr_size_char(_tmp8D,2));}),({const char*_tmp8E=",";_tag_dynforward(
_tmp8E,sizeof(char),_get_zero_arr_size_char(_tmp8E,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dynforward_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));}struct _tuple10{
struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct
_tuple10*t){return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(
struct Cyc_List_List*ts){return Cyc_PP_group(({const char*_tmp8F="(";
_tag_dynforward(_tmp8F,sizeof(char),_get_zero_arr_size_char(_tmp8F,2));}),({
const char*_tmp90=")";_tag_dynforward(_tmp90,sizeof(char),_get_zero_arr_size_char(
_tmp90,2));}),({const char*_tmp91=",";_tag_dynforward(_tmp91,sizeof(char),
_get_zero_arr_size_char(_tmp91,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){void*_tmp92=att;_LL5F: if((
int)_tmp92 != 0)goto _LL61;_LL60: goto _LL62;_LL61: if((int)_tmp92 != 1)goto _LL63;
_LL62: goto _LL64;_LL63: if((int)_tmp92 != 2)goto _LL65;_LL64: return Cyc_PP_nil_doc();
_LL65:;_LL66: return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL5E:;}struct
Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){for(0;atts != 0;
atts=atts->tl){void*_tmp93=(void*)atts->hd;_LL68: if((int)_tmp93 != 0)goto _LL6A;
_LL69: return Cyc_PP_text(({const char*_tmp94=" _stdcall ";_tag_dynforward(_tmp94,
sizeof(char),_get_zero_arr_size_char(_tmp94,11));}));_LL6A: if((int)_tmp93 != 1)
goto _LL6C;_LL6B: return Cyc_PP_text(({const char*_tmp95=" _cdecl ";_tag_dynforward(
_tmp95,sizeof(char),_get_zero_arr_size_char(_tmp95,9));}));_LL6C: if((int)_tmp93
!= 2)goto _LL6E;_LL6D: return Cyc_PP_text(({const char*_tmp96=" _fastcall ";
_tag_dynforward(_tmp96,sizeof(char),_get_zero_arr_size_char(_tmp96,12));}));
_LL6E:;_LL6F: goto _LL67;_LL67:;}return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(
struct Cyc_List_List*atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;
atts2 != 0;atts2=atts2->tl){void*_tmp97=(void*)atts2->hd;_LL71: if((int)_tmp97 != 0)
goto _LL73;_LL72: goto _LL74;_LL73: if((int)_tmp97 != 1)goto _LL75;_LL74: goto _LL76;
_LL75: if((int)_tmp97 != 2)goto _LL77;_LL76: goto _LL70;_LL77:;_LL78: hasatt=1;goto
_LL70;_LL70:;}}if(!hasatt)return Cyc_PP_nil_doc();return({struct Cyc_PP_Doc*_tmp98[
3];_tmp98[2]=Cyc_PP_text(({const char*_tmp9D=")";_tag_dynforward(_tmp9D,sizeof(
char),_get_zero_arr_size_char(_tmp9D,2));}));_tmp98[1]=Cyc_PP_group(({const char*
_tmp9A="";_tag_dynforward(_tmp9A,sizeof(char),_get_zero_arr_size_char(_tmp9A,1));}),({
const char*_tmp9B="";_tag_dynforward(_tmp9B,sizeof(char),_get_zero_arr_size_char(
_tmp9B,1));}),({const char*_tmp9C=" ";_tag_dynforward(_tmp9C,sizeof(char),
_get_zero_arr_size_char(_tmp9C,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));
_tmp98[0]=Cyc_PP_text(({const char*_tmp99=" __declspec(";_tag_dynforward(_tmp99,
sizeof(char),_get_zero_arr_size_char(_tmp99,13));}));Cyc_PP_cat(_tag_dynforward(
_tmp98,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(
void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(
struct Cyc_List_List*atts){if(atts == 0)return Cyc_PP_nil_doc();{void*_tmp9E=Cyc_Cyclone_c_compiler;
_LL7A: if((int)_tmp9E != 1)goto _LL7C;_LL7B: return Cyc_Absynpp_noncallconv2doc(atts);
_LL7C: if((int)_tmp9E != 0)goto _LL79;_LL7D: return({struct Cyc_PP_Doc*_tmp9F[2];
_tmp9F[1]=Cyc_PP_group(({const char*_tmpA1="((";_tag_dynforward(_tmpA1,sizeof(
char),_get_zero_arr_size_char(_tmpA1,3));}),({const char*_tmpA2="))";
_tag_dynforward(_tmpA2,sizeof(char),_get_zero_arr_size_char(_tmpA2,3));}),({
const char*_tmpA3=",";_tag_dynforward(_tmpA3,sizeof(char),_get_zero_arr_size_char(
_tmpA3,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));_tmp9F[0]=Cyc_PP_text(({const char*
_tmpA0=" __attribute__";_tag_dynforward(_tmpA0,sizeof(char),
_get_zero_arr_size_char(_tmpA0,15));}));Cyc_PP_cat(_tag_dynforward(_tmp9F,
sizeof(struct Cyc_PP_Doc*),2));});_LL79:;}}int Cyc_Absynpp_next_is_pointer(struct
Cyc_List_List*tms){if(tms == 0)return 0;{void*_tmpA4=(void*)tms->hd;_LL7F: if(*((
int*)_tmpA4)!= 2)goto _LL81;_LL80: return 1;_LL81: if(*((int*)_tmpA4)!= 5)goto _LL83;
_LL82: {void*_tmpA5=Cyc_Cyclone_c_compiler;_LL86: if((int)_tmpA5 != 0)goto _LL88;
_LL87: return 0;_LL88:;_LL89: return Cyc_Absynpp_next_is_pointer(tms->tl);_LL85:;}
_LL83:;_LL84: return 0;_LL7E:;}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*Cyc_Absynpp_question(){
if(!((unsigned int)Cyc_Absynpp_cache_question)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_question=(
struct Cyc_PP_Doc*)Cyc_PP_text_width(({const char*_tmpA6="{?}";_tag_dynforward(
_tmpA6,sizeof(char),_get_zero_arr_size_char(_tmpA6,4));}),1);else{Cyc_Absynpp_cache_question=(
struct Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpA7="?";_tag_dynforward(_tmpA7,
sizeof(char),_get_zero_arr_size_char(_tmpA7,2));}));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output)Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(({
const char*_tmpA8="{\\lb}";_tag_dynforward(_tmpA8,sizeof(char),
_get_zero_arr_size_char(_tmpA8,6));}),1);else{Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)
Cyc_PP_text(({const char*_tmpA9="{";_tag_dynforward(_tmpA9,sizeof(char),
_get_zero_arr_size_char(_tmpA9,2));}));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_lb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;static
struct Cyc_PP_Doc*Cyc_Absynpp_rb(){if(!((unsigned int)Cyc_Absynpp_cache_rb)){if(
Cyc_PP_tex_output)Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(({
const char*_tmpAA="{\\rb}";_tag_dynforward(_tmpAA,sizeof(char),
_get_zero_arr_size_char(_tmpAA,6));}),1);else{Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)
Cyc_PP_text(({const char*_tmpAB="}";_tag_dynforward(_tmpAB,sizeof(char),
_get_zero_arr_size_char(_tmpAB,2));}));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;static
struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output)Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text_width(({
const char*_tmpAC="\\$";_tag_dynforward(_tmpAC,sizeof(char),
_get_zero_arr_size_char(_tmpAC,3));}),1);else{Cyc_Absynpp_cache_dollar=(struct
Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpAD="$";_tag_dynforward(_tmpAD,sizeof(
char),_get_zero_arr_size_char(_tmpAD,2));}));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_dollar);}struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(
struct _dynforward_ptr sep,struct Cyc_List_List*ss){return({struct Cyc_PP_Doc*_tmpAE[
3];_tmpAE[2]=Cyc_Absynpp_rb();_tmpAE[1]=Cyc_PP_seq(sep,ss);_tmpAE[0]=Cyc_Absynpp_lb();
Cyc_PP_cat(_tag_dynforward(_tmpAE,sizeof(struct Cyc_PP_Doc*),3));});}static void
Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){for(0;tms != 0;tms=tms->tl){void*
_tmpAF=(void*)tms->hd;void*_tmpB0;struct Cyc_List_List*_tmpB1;_LL8B: if(*((int*)
_tmpAF)!= 0)goto _LL8D;_LL8C:({void*_tmpB2=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmpB3="Carray_mod ";_tag_dynforward(_tmpB3,sizeof(char),_get_zero_arr_size_char(
_tmpB3,12));}),_tag_dynforward(_tmpB2,sizeof(void*),0));});goto _LL8A;_LL8D: if(*((
int*)_tmpAF)!= 1)goto _LL8F;_LL8E:({void*_tmpB4=0;Cyc_fprintf(Cyc_stderr,({const
char*_tmpB5="ConstArray_mod ";_tag_dynforward(_tmpB5,sizeof(char),
_get_zero_arr_size_char(_tmpB5,16));}),_tag_dynforward(_tmpB4,sizeof(void*),0));});
goto _LL8A;_LL8F: if(*((int*)_tmpAF)!= 3)goto _LL91;_tmpB0=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpAF)->f1;if(*((int*)_tmpB0)!= 1)goto _LL91;_tmpB1=((struct Cyc_Absyn_WithTypes_struct*)
_tmpB0)->f1;_LL90:({void*_tmpB6=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpB7="Function_mod(";
_tag_dynforward(_tmpB7,sizeof(char),_get_zero_arr_size_char(_tmpB7,14));}),
_tag_dynforward(_tmpB6,sizeof(void*),0));});for(0;_tmpB1 != 0;_tmpB1=_tmpB1->tl){
struct Cyc_Core_Opt*_tmpB8=(*((struct _tuple1*)_tmpB1->hd)).f1;if(_tmpB8 == 0)({
void*_tmpB9=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpBA="?";_tag_dynforward(
_tmpBA,sizeof(char),_get_zero_arr_size_char(_tmpBA,2));}),_tag_dynforward(_tmpB9,
sizeof(void*),0));});else{({void*_tmpBB=0;Cyc_fprintf(Cyc_stderr,*((struct
_dynforward_ptr*)_tmpB8->v),_tag_dynforward(_tmpBB,sizeof(void*),0));});}if(
_tmpB1->tl != 0)({void*_tmpBC=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpBD=",";
_tag_dynforward(_tmpBD,sizeof(char),_get_zero_arr_size_char(_tmpBD,2));}),
_tag_dynforward(_tmpBC,sizeof(void*),0));});}({void*_tmpBE=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmpBF=") ";_tag_dynforward(_tmpBF,sizeof(char),
_get_zero_arr_size_char(_tmpBF,3));}),_tag_dynforward(_tmpBE,sizeof(void*),0));});
goto _LL8A;_LL91: if(*((int*)_tmpAF)!= 3)goto _LL93;_LL92:({void*_tmpC0=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmpC1="Function_mod()";_tag_dynforward(_tmpC1,sizeof(
char),_get_zero_arr_size_char(_tmpC1,15));}),_tag_dynforward(_tmpC0,sizeof(void*),
0));});goto _LL8A;_LL93: if(*((int*)_tmpAF)!= 5)goto _LL95;_LL94:({void*_tmpC2=0;
Cyc_fprintf(Cyc_stderr,({const char*_tmpC3="Attributes_mod ";_tag_dynforward(
_tmpC3,sizeof(char),_get_zero_arr_size_char(_tmpC3,16));}),_tag_dynforward(
_tmpC2,sizeof(void*),0));});goto _LL8A;_LL95: if(*((int*)_tmpAF)!= 4)goto _LL97;
_LL96:({void*_tmpC4=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpC5="TypeParams_mod ";
_tag_dynforward(_tmpC5,sizeof(char),_get_zero_arr_size_char(_tmpC5,16));}),
_tag_dynforward(_tmpC4,sizeof(void*),0));});goto _LL8A;_LL97: if(*((int*)_tmpAF)!= 
2)goto _LL8A;_LL98:({void*_tmpC6=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpC7="Pointer_mod ";
_tag_dynforward(_tmpC7,sizeof(char),_get_zero_arr_size_char(_tmpC7,13));}),
_tag_dynforward(_tmpC6,sizeof(void*),0));});goto _LL8A;_LL8A:;}({void*_tmpC8=0;
Cyc_fprintf(Cyc_stderr,({const char*_tmpC9="\n";_tag_dynforward(_tmpC9,sizeof(
char),_get_zero_arr_size_char(_tmpC9,2));}),_tag_dynforward(_tmpC8,sizeof(void*),
0));});}struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,
struct Cyc_List_List*tms){if(tms == 0)return d;{struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(
0,d,tms->tl);struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmp109[3];_tmp109[2]=
Cyc_PP_text(({const char*_tmp10B=")";_tag_dynforward(_tmp10B,sizeof(char),
_get_zero_arr_size_char(_tmp10B,2));}));_tmp109[1]=rest;_tmp109[0]=Cyc_PP_text(({
const char*_tmp10A="(";_tag_dynforward(_tmp10A,sizeof(char),
_get_zero_arr_size_char(_tmp10A,2));}));Cyc_PP_cat(_tag_dynforward(_tmp109,
sizeof(struct Cyc_PP_Doc*),3));});void*_tmpCA=(void*)tms->hd;struct Cyc_Absyn_Conref*
_tmpCB;struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Absyn_Conref*_tmpCD;void*_tmpCE;
struct Cyc_List_List*_tmpCF;struct Cyc_List_List*_tmpD0;struct Cyc_Position_Segment*
_tmpD1;int _tmpD2;struct Cyc_Absyn_PtrAtts _tmpD3;void*_tmpD4;struct Cyc_Absyn_Conref*
_tmpD5;struct Cyc_Absyn_Conref*_tmpD6;struct Cyc_Absyn_Conref*_tmpD7;struct Cyc_Absyn_Tqual
_tmpD8;_LL9A: if(*((int*)_tmpCA)!= 0)goto _LL9C;_tmpCB=((struct Cyc_Absyn_Carray_mod_struct*)
_tmpCA)->f1;_LL9B: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;return({
struct Cyc_PP_Doc*_tmpD9[2];_tmpD9[1]=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpCB)?Cyc_PP_text(({const char*_tmpDA="[]ZEROTERM ";_tag_dynforward(_tmpDA,
sizeof(char),_get_zero_arr_size_char(_tmpDA,12));})): Cyc_PP_text(({const char*
_tmpDB="[]";_tag_dynforward(_tmpDB,sizeof(char),_get_zero_arr_size_char(_tmpDB,3));}));
_tmpD9[0]=rest;Cyc_PP_cat(_tag_dynforward(_tmpD9,sizeof(struct Cyc_PP_Doc*),2));});
_LL9C: if(*((int*)_tmpCA)!= 1)goto _LL9E;_tmpCC=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpCA)->f1;_tmpCD=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCA)->f2;_LL9D:
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*
_tmpDC[4];_tmpDC[3]=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpCD)?Cyc_PP_text(({const char*_tmpDE="]ZEROTERM ";_tag_dynforward(_tmpDE,
sizeof(char),_get_zero_arr_size_char(_tmpDE,11));})): Cyc_PP_text(({const char*
_tmpDF="]";_tag_dynforward(_tmpDF,sizeof(char),_get_zero_arr_size_char(_tmpDF,2));}));
_tmpDC[2]=Cyc_Absynpp_exp2doc(_tmpCC);_tmpDC[1]=Cyc_PP_text(({const char*_tmpDD="[";
_tag_dynforward(_tmpDD,sizeof(char),_get_zero_arr_size_char(_tmpDD,2));}));
_tmpDC[0]=rest;Cyc_PP_cat(_tag_dynforward(_tmpDC,sizeof(struct Cyc_PP_Doc*),4));});
_LL9E: if(*((int*)_tmpCA)!= 3)goto _LLA0;_tmpCE=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpCA)->f1;_LL9F: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{void*
_tmpE0=_tmpCE;struct Cyc_List_List*_tmpE1;int _tmpE2;struct Cyc_Absyn_VarargInfo*
_tmpE3;struct Cyc_Core_Opt*_tmpE4;struct Cyc_List_List*_tmpE5;struct Cyc_List_List*
_tmpE6;struct Cyc_Position_Segment*_tmpE7;_LLA7: if(*((int*)_tmpE0)!= 1)goto _LLA9;
_tmpE1=((struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f1;_tmpE2=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE0)->f2;_tmpE3=((struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f3;_tmpE4=((
struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f4;_tmpE5=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE0)->f5;_LLA8: return({struct Cyc_PP_Doc*_tmpE8[2];_tmpE8[1]=Cyc_Absynpp_funargs2doc(
_tmpE1,_tmpE2,_tmpE3,_tmpE4,_tmpE5);_tmpE8[0]=rest;Cyc_PP_cat(_tag_dynforward(
_tmpE8,sizeof(struct Cyc_PP_Doc*),2));});_LLA9: if(*((int*)_tmpE0)!= 0)goto _LLA6;
_tmpE6=((struct Cyc_Absyn_NoTypes_struct*)_tmpE0)->f1;_tmpE7=((struct Cyc_Absyn_NoTypes_struct*)
_tmpE0)->f2;_LLAA: return({struct Cyc_PP_Doc*_tmpE9[2];_tmpE9[1]=Cyc_PP_group(({
const char*_tmpEA="(";_tag_dynforward(_tmpEA,sizeof(char),_get_zero_arr_size_char(
_tmpEA,2));}),({const char*_tmpEB=")";_tag_dynforward(_tmpEB,sizeof(char),
_get_zero_arr_size_char(_tmpEB,2));}),({const char*_tmpEC=",";_tag_dynforward(
_tmpEC,sizeof(char),_get_zero_arr_size_char(_tmpEC,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,_tmpE6));_tmpE9[0]=rest;Cyc_PP_cat(_tag_dynforward(_tmpE9,sizeof(
struct Cyc_PP_Doc*),2));});_LLA6:;}_LLA0: if(*((int*)_tmpCA)!= 5)goto _LLA2;_tmpCF=((
struct Cyc_Absyn_Attributes_mod_struct*)_tmpCA)->f2;_LLA1: {void*_tmpED=Cyc_Cyclone_c_compiler;
_LLAC: if((int)_tmpED != 0)goto _LLAE;_LLAD: if(Cyc_Absynpp_next_is_pointer(tms->tl))
rest=p_rest;return({struct Cyc_PP_Doc*_tmpEE[2];_tmpEE[1]=Cyc_Absynpp_atts2doc(
_tmpCF);_tmpEE[0]=rest;Cyc_PP_cat(_tag_dynforward(_tmpEE,sizeof(struct Cyc_PP_Doc*),
2));});_LLAE: if((int)_tmpED != 1)goto _LLAB;_LLAF: if(Cyc_Absynpp_next_is_pointer(
tms->tl))return({struct Cyc_PP_Doc*_tmpEF[2];_tmpEF[1]=rest;_tmpEF[0]=Cyc_Absynpp_callconv2doc(
_tmpCF);Cyc_PP_cat(_tag_dynforward(_tmpEF,sizeof(struct Cyc_PP_Doc*),2));});
return rest;_LLAB:;}_LLA2: if(*((int*)_tmpCA)!= 4)goto _LLA4;_tmpD0=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpCA)->f1;_tmpD1=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCA)->f2;_tmpD2=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCA)->f3;_LLA3: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;if(_tmpD2)return({struct Cyc_PP_Doc*_tmpF0[2];_tmpF0[1]=Cyc_Absynpp_ktvars2doc(
_tmpD0);_tmpF0[0]=rest;Cyc_PP_cat(_tag_dynforward(_tmpF0,sizeof(struct Cyc_PP_Doc*),
2));});else{return({struct Cyc_PP_Doc*_tmpF1[2];_tmpF1[1]=Cyc_Absynpp_tvars2doc(
_tmpD0);_tmpF1[0]=rest;Cyc_PP_cat(_tag_dynforward(_tmpF1,sizeof(struct Cyc_PP_Doc*),
2));});}_LLA4: if(*((int*)_tmpCA)!= 2)goto _LL99;_tmpD3=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpCA)->f1;_tmpD4=(void*)_tmpD3.rgn;_tmpD5=_tmpD3.nullable;_tmpD6=_tmpD3.bounds;
_tmpD7=_tmpD3.zero_term;_tmpD8=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCA)->f2;
_LLA5: {struct Cyc_PP_Doc*ptr;{void*_tmpF2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmpD6);struct Cyc_Absyn_Exp*_tmpF3;void*_tmpF4;_LLB1: if((int)_tmpF2 != 0)goto
_LLB3;_LLB2: ptr=Cyc_Absynpp_question();goto _LLB0;_LLB3: if((int)_tmpF2 != 1)goto
_LLB5;_LLB4: ptr=({struct Cyc_PP_Doc*_tmpF5[2];_tmpF5[1]=Cyc_PP_text(({const char*
_tmpF6="-";_tag_dynforward(_tmpF6,sizeof(char),_get_zero_arr_size_char(_tmpF6,2));}));
_tmpF5[0]=Cyc_Absynpp_question();Cyc_PP_cat(_tag_dynforward(_tmpF5,sizeof(struct
Cyc_PP_Doc*),2));});goto _LLB0;_LLB5: if(_tmpF2 <= (void*)2)goto _LLB7;if(*((int*)
_tmpF2)!= 0)goto _LLB7;_tmpF3=((struct Cyc_Absyn_Upper_b_struct*)_tmpF2)->f1;_LLB6:
ptr=Cyc_PP_text(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,
_tmpD5)?({const char*_tmpF7="*";_tag_dynforward(_tmpF7,sizeof(char),
_get_zero_arr_size_char(_tmpF7,2));}):({const char*_tmpF8="@";_tag_dynforward(
_tmpF8,sizeof(char),_get_zero_arr_size_char(_tmpF8,2));}));{unsigned int _tmpFA;
int _tmpFB;struct _tuple4 _tmpF9=Cyc_Evexp_eval_const_uint_exp(_tmpF3);_tmpFA=
_tmpF9.f1;_tmpFB=_tmpF9.f2;if(!_tmpFB  || _tmpFA != 1)ptr=({struct Cyc_PP_Doc*
_tmpFC[4];_tmpFC[3]=Cyc_Absynpp_rb();_tmpFC[2]=Cyc_Absynpp_exp2doc(_tmpF3);
_tmpFC[1]=Cyc_Absynpp_lb();_tmpFC[0]=ptr;Cyc_PP_cat(_tag_dynforward(_tmpFC,
sizeof(struct Cyc_PP_Doc*),4));});goto _LLB0;}_LLB7: if(_tmpF2 <= (void*)2)goto _LLB0;
if(*((int*)_tmpF2)!= 1)goto _LLB0;_tmpF4=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmpF2)->f1;_LLB8: ptr=Cyc_PP_text(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmpD5)?({const char*_tmpFD="*";_tag_dynforward(_tmpFD,sizeof(char),
_get_zero_arr_size_char(_tmpFD,2));}):({const char*_tmpFE="@";_tag_dynforward(
_tmpFE,sizeof(char),_get_zero_arr_size_char(_tmpFE,2));}));ptr=({struct Cyc_PP_Doc*
_tmpFF[4];_tmpFF[3]=Cyc_Absynpp_rb();_tmpFF[2]=Cyc_Absynpp_typ2doc(_tmpF4);
_tmpFF[1]=Cyc_Absynpp_lb();_tmpFF[0]=ptr;Cyc_PP_cat(_tag_dynforward(_tmpFF,
sizeof(struct Cyc_PP_Doc*),4));});goto _LLB0;_LLB0:;}if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpD7))ptr=({struct Cyc_PP_Doc*_tmp100[2];_tmp100[1]=Cyc_PP_text(({const char*
_tmp101="ZEROTERM ";_tag_dynforward(_tmp101,sizeof(char),_get_zero_arr_size_char(
_tmp101,10));}));_tmp100[0]=ptr;Cyc_PP_cat(_tag_dynforward(_tmp100,sizeof(struct
Cyc_PP_Doc*),2));});else{if(is_char_ptr  && !((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmpD7))ptr=({struct Cyc_PP_Doc*_tmp102[2];_tmp102[1]=
Cyc_PP_text(({const char*_tmp103="NOZEROTERM ";_tag_dynforward(_tmp103,sizeof(
char),_get_zero_arr_size_char(_tmp103,12));}));_tmp102[0]=ptr;Cyc_PP_cat(
_tag_dynforward(_tmp102,sizeof(struct Cyc_PP_Doc*),2));});}}{void*_tmp104=Cyc_Tcutil_compress(
_tmpD4);_LLBA: if((int)_tmp104 != 2)goto _LLBC;_LLBB: goto _LLB9;_LLBC:;_LLBD: ptr=({
struct Cyc_PP_Doc*_tmp105[3];_tmp105[2]=Cyc_PP_text(({const char*_tmp106=" ";
_tag_dynforward(_tmp106,sizeof(char),_get_zero_arr_size_char(_tmp106,2));}));
_tmp105[1]=Cyc_Absynpp_typ2doc(_tmpD4);_tmp105[0]=ptr;Cyc_PP_cat(_tag_dynforward(
_tmp105,sizeof(struct Cyc_PP_Doc*),3));});_LLB9:;}ptr=({struct Cyc_PP_Doc*_tmp107[
2];_tmp107[1]=Cyc_Absynpp_tqual2doc(_tmpD8);_tmp107[0]=ptr;Cyc_PP_cat(
_tag_dynforward(_tmp107,sizeof(struct Cyc_PP_Doc*),2));});return({struct Cyc_PP_Doc*
_tmp108[2];_tmp108[1]=rest;_tmp108[0]=ptr;Cyc_PP_cat(_tag_dynforward(_tmp108,
sizeof(struct Cyc_PP_Doc*),2));});}_LL99:;}}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t){void*_tmp10C=Cyc_Tcutil_compress(t);_LLBF: if((int)_tmp10C != 2)goto _LLC1;
_LLC0: return Cyc_PP_text(({const char*_tmp10D="`H";_tag_dynforward(_tmp10D,sizeof(
char),_get_zero_arr_size_char(_tmp10D,3));}));_LLC1: if((int)_tmp10C != 3)goto
_LLC3;_LLC2: return Cyc_PP_text(({const char*_tmp10E="`U";_tag_dynforward(_tmp10E,
sizeof(char),_get_zero_arr_size_char(_tmp10E,3));}));_LLC3:;_LLC4: return Cyc_Absynpp_ntyp2doc(
t);_LLBE:;}static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct
Cyc_List_List**effects,void*t){void*_tmp10F=Cyc_Tcutil_compress(t);void*_tmp110;
struct Cyc_List_List*_tmp111;_LLC6: if(_tmp10F <= (void*)4)goto _LLCA;if(*((int*)
_tmp10F)!= 20)goto _LLC8;_tmp110=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp10F)->f1;_LLC7:*rgions=({struct Cyc_List_List*_tmp112=_cycalloc(sizeof(*
_tmp112));_tmp112->hd=Cyc_Absynpp_rgn2doc(_tmp110);_tmp112->tl=*rgions;_tmp112;});
goto _LLC5;_LLC8: if(*((int*)_tmp10F)!= 21)goto _LLCA;_tmp111=((struct Cyc_Absyn_JoinEff_struct*)
_tmp10F)->f1;_LLC9: for(0;_tmp111 != 0;_tmp111=_tmp111->tl){Cyc_Absynpp_effects2docs(
rgions,effects,(void*)_tmp111->hd);}goto _LLC5;_LLCA:;_LLCB:*effects=({struct Cyc_List_List*
_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->hd=Cyc_Absynpp_typ2doc(t);_tmp113->tl=*
effects;_tmp113;});goto _LLC5;_LLC5:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(&
rgions,& effects,t);rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
rgions);effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0  && effects != 0)return Cyc_PP_group(({const char*_tmp114="";
_tag_dynforward(_tmp114,sizeof(char),_get_zero_arr_size_char(_tmp114,1));}),({
const char*_tmp115="";_tag_dynforward(_tmp115,sizeof(char),
_get_zero_arr_size_char(_tmp115,1));}),({const char*_tmp116="+";_tag_dynforward(
_tmp116,sizeof(char),_get_zero_arr_size_char(_tmp116,2));}),effects);else{struct
Cyc_PP_Doc*_tmp117=Cyc_Absynpp_group_braces(({const char*_tmp11C=",";
_tag_dynforward(_tmp11C,sizeof(char),_get_zero_arr_size_char(_tmp11C,2));}),
rgions);return Cyc_PP_group(({const char*_tmp118="";_tag_dynforward(_tmp118,
sizeof(char),_get_zero_arr_size_char(_tmp118,1));}),({const char*_tmp119="";
_tag_dynforward(_tmp119,sizeof(char),_get_zero_arr_size_char(_tmp119,1));}),({
const char*_tmp11A="+";_tag_dynforward(_tmp11A,sizeof(char),
_get_zero_arr_size_char(_tmp11A,2));}),((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,({struct Cyc_List_List*
_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->hd=_tmp117;_tmp11B->tl=0;_tmp11B;})));}}
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(void*k){void*_tmp11D=k;_LLCD: if((int)
_tmp11D != 0)goto _LLCF;_LLCE: return Cyc_PP_text(({const char*_tmp11E="struct ";
_tag_dynforward(_tmp11E,sizeof(char),_get_zero_arr_size_char(_tmp11E,8));}));
_LLCF: if((int)_tmp11D != 1)goto _LLCC;_LLD0: return Cyc_PP_text(({const char*_tmp11F="union ";
_tag_dynforward(_tmp11F,sizeof(char),_get_zero_arr_size_char(_tmp11F,7));}));
_LLCC:;}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){struct Cyc_PP_Doc*s;{void*
_tmp120=t;struct Cyc_Core_Opt*_tmp121;struct Cyc_Core_Opt*_tmp122;int _tmp123;
struct Cyc_Core_Opt*_tmp124;struct Cyc_Absyn_Tvar*_tmp125;struct Cyc_Absyn_TunionInfo
_tmp126;union Cyc_Absyn_TunionInfoU_union _tmp127;struct Cyc_List_List*_tmp128;
struct Cyc_Core_Opt*_tmp129;struct Cyc_Absyn_TunionFieldInfo _tmp12A;union Cyc_Absyn_TunionFieldInfoU_union
_tmp12B;struct Cyc_List_List*_tmp12C;void*_tmp12D;void*_tmp12E;int _tmp12F;struct
Cyc_List_List*_tmp130;struct Cyc_Absyn_AggrInfo _tmp131;union Cyc_Absyn_AggrInfoU_union
_tmp132;struct Cyc_List_List*_tmp133;void*_tmp134;struct Cyc_List_List*_tmp135;
struct Cyc_List_List*_tmp136;struct _tuple0*_tmp137;void*_tmp138;struct _tuple0*
_tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_Absyn_Typedefdecl*_tmp13B;void*
_tmp13C;void*_tmp13D;void*_tmp13E;int _tmp13F;void*_tmp140;void*_tmp141;_LLD2: if(
_tmp120 <= (void*)4)goto _LLD8;if(*((int*)_tmp120)!= 7)goto _LLD4;_LLD3: return Cyc_PP_text(({
const char*_tmp142="[[[array]]]";_tag_dynforward(_tmp142,sizeof(char),
_get_zero_arr_size_char(_tmp142,12));}));_LLD4: if(*((int*)_tmp120)!= 8)goto _LLD6;
_LLD5: return Cyc_PP_nil_doc();_LLD6: if(*((int*)_tmp120)!= 4)goto _LLD8;_LLD7:
return Cyc_PP_nil_doc();_LLD8: if((int)_tmp120 != 0)goto _LLDA;_LLD9: s=Cyc_PP_text(({
const char*_tmp143="void";_tag_dynforward(_tmp143,sizeof(char),
_get_zero_arr_size_char(_tmp143,5));}));goto _LLD1;_LLDA: if(_tmp120 <= (void*)4)
goto _LLE4;if(*((int*)_tmp120)!= 0)goto _LLDC;_tmp121=((struct Cyc_Absyn_Evar_struct*)
_tmp120)->f1;_tmp122=((struct Cyc_Absyn_Evar_struct*)_tmp120)->f2;_tmp123=((
struct Cyc_Absyn_Evar_struct*)_tmp120)->f3;_tmp124=((struct Cyc_Absyn_Evar_struct*)
_tmp120)->f4;_LLDB: if(_tmp122 != 0)return Cyc_Absynpp_ntyp2doc((void*)_tmp122->v);
else{s=({struct Cyc_PP_Doc*_tmp144[6];_tmp144[5]=_tmp121 == 0?Cyc_Absynpp_question():
Cyc_Absynpp_kind2doc((void*)_tmp121->v);_tmp144[4]=Cyc_PP_text(({const char*
_tmp14A=")::";_tag_dynforward(_tmp14A,sizeof(char),_get_zero_arr_size_char(
_tmp14A,4));}));_tmp144[3]=(!Cyc_Absynpp_print_full_evars  || _tmp124 == 0)?Cyc_PP_nil_doc():
Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)_tmp124->v);_tmp144[2]=Cyc_PP_text((
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp149;_tmp149.tag=1;_tmp149.f1=(
unsigned long)_tmp123;{void*_tmp147[1]={& _tmp149};Cyc_aprintf(({const char*
_tmp148="%d";_tag_dynforward(_tmp148,sizeof(char),_get_zero_arr_size_char(
_tmp148,3));}),_tag_dynforward(_tmp147,sizeof(void*),1));}}));_tmp144[1]=Cyc_PP_text(({
const char*_tmp146="(";_tag_dynforward(_tmp146,sizeof(char),
_get_zero_arr_size_char(_tmp146,2));}));_tmp144[0]=Cyc_PP_text(({const char*
_tmp145="%";_tag_dynforward(_tmp145,sizeof(char),_get_zero_arr_size_char(_tmp145,
2));}));Cyc_PP_cat(_tag_dynforward(_tmp144,sizeof(struct Cyc_PP_Doc*),6));});}
goto _LLD1;_LLDC: if(*((int*)_tmp120)!= 1)goto _LLDE;_tmp125=((struct Cyc_Absyn_VarType_struct*)
_tmp120)->f1;_LLDD: s=Cyc_PP_textptr(_tmp125->name);if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_tmp14B[3];_tmp14B[2]=Cyc_Absynpp_kindbound2doc((void*)
_tmp125->kind);_tmp14B[1]=Cyc_PP_text(({const char*_tmp14C="::";_tag_dynforward(
_tmp14C,sizeof(char),_get_zero_arr_size_char(_tmp14C,3));}));_tmp14B[0]=s;Cyc_PP_cat(
_tag_dynforward(_tmp14B,sizeof(struct Cyc_PP_Doc*),3));});if(Cyc_Absynpp_rewrite_temp_tvars
 && Cyc_Tcutil_is_temp_tvar(_tmp125))s=({struct Cyc_PP_Doc*_tmp14D[3];_tmp14D[2]=
Cyc_PP_text(({const char*_tmp14F=" */";_tag_dynforward(_tmp14F,sizeof(char),
_get_zero_arr_size_char(_tmp14F,4));}));_tmp14D[1]=s;_tmp14D[0]=Cyc_PP_text(({
const char*_tmp14E="_ /* ";_tag_dynforward(_tmp14E,sizeof(char),
_get_zero_arr_size_char(_tmp14E,6));}));Cyc_PP_cat(_tag_dynforward(_tmp14D,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLD1;_LLDE: if(*((int*)_tmp120)!= 2)goto
_LLE0;_tmp126=((struct Cyc_Absyn_TunionType_struct*)_tmp120)->f1;_tmp127=_tmp126.tunion_info;
_tmp128=_tmp126.targs;_tmp129=_tmp126.rgn;_LLDF:{union Cyc_Absyn_TunionInfoU_union
_tmp150=_tmp127;struct Cyc_Absyn_UnknownTunionInfo _tmp151;struct _tuple0*_tmp152;
int _tmp153;struct Cyc_Absyn_Tuniondecl**_tmp154;struct Cyc_Absyn_Tuniondecl*
_tmp155;struct Cyc_Absyn_Tuniondecl _tmp156;struct _tuple0*_tmp157;int _tmp158;
_LL109: if((_tmp150.UnknownTunion).tag != 0)goto _LL10B;_tmp151=(_tmp150.UnknownTunion).f1;
_tmp152=_tmp151.name;_tmp153=_tmp151.is_xtunion;_LL10A: _tmp157=_tmp152;_tmp158=
_tmp153;goto _LL10C;_LL10B: if((_tmp150.KnownTunion).tag != 1)goto _LL108;_tmp154=(
_tmp150.KnownTunion).f1;_tmp155=*_tmp154;_tmp156=*_tmp155;_tmp157=_tmp156.name;
_tmp158=_tmp156.is_xtunion;_LL10C: {struct Cyc_PP_Doc*_tmp159=Cyc_PP_text(_tmp158?({
const char*_tmp15E="xtunion ";_tag_dynforward(_tmp15E,sizeof(char),
_get_zero_arr_size_char(_tmp15E,9));}):({const char*_tmp15F="tunion ";
_tag_dynforward(_tmp15F,sizeof(char),_get_zero_arr_size_char(_tmp15F,8));}));
void*r=(unsigned int)_tmp129?(void*)_tmp129->v:(void*)2;{void*_tmp15A=Cyc_Tcutil_compress(
r);_LL10E: if((int)_tmp15A != 2)goto _LL110;_LL10F: s=({struct Cyc_PP_Doc*_tmp15B[3];
_tmp15B[2]=Cyc_Absynpp_tps2doc(_tmp128);_tmp15B[1]=Cyc_Absynpp_qvar2doc(_tmp157);
_tmp15B[0]=_tmp159;Cyc_PP_cat(_tag_dynforward(_tmp15B,sizeof(struct Cyc_PP_Doc*),
3));});goto _LL10D;_LL110:;_LL111: s=({struct Cyc_PP_Doc*_tmp15C[5];_tmp15C[4]=Cyc_Absynpp_tps2doc(
_tmp128);_tmp15C[3]=Cyc_Absynpp_qvar2doc(_tmp157);_tmp15C[2]=Cyc_PP_text(({const
char*_tmp15D=" ";_tag_dynforward(_tmp15D,sizeof(char),_get_zero_arr_size_char(
_tmp15D,2));}));_tmp15C[1]=Cyc_Absynpp_typ2doc(r);_tmp15C[0]=_tmp159;Cyc_PP_cat(
_tag_dynforward(_tmp15C,sizeof(struct Cyc_PP_Doc*),5));});goto _LL10D;_LL10D:;}
goto _LL108;}_LL108:;}goto _LLD1;_LLE0: if(*((int*)_tmp120)!= 3)goto _LLE2;_tmp12A=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp120)->f1;_tmp12B=_tmp12A.field_info;
_tmp12C=_tmp12A.targs;_LLE1:{union Cyc_Absyn_TunionFieldInfoU_union _tmp160=
_tmp12B;struct Cyc_Absyn_UnknownTunionFieldInfo _tmp161;struct _tuple0*_tmp162;
struct _tuple0*_tmp163;int _tmp164;struct Cyc_Absyn_Tuniondecl*_tmp165;struct Cyc_Absyn_Tuniondecl
_tmp166;struct _tuple0*_tmp167;int _tmp168;struct Cyc_Absyn_Tunionfield*_tmp169;
struct Cyc_Absyn_Tunionfield _tmp16A;struct _tuple0*_tmp16B;_LL113: if((_tmp160.UnknownTunionfield).tag
!= 0)goto _LL115;_tmp161=(_tmp160.UnknownTunionfield).f1;_tmp162=_tmp161.tunion_name;
_tmp163=_tmp161.field_name;_tmp164=_tmp161.is_xtunion;_LL114: _tmp167=_tmp162;
_tmp168=_tmp164;_tmp16B=_tmp163;goto _LL116;_LL115: if((_tmp160.KnownTunionfield).tag
!= 1)goto _LL112;_tmp165=(_tmp160.KnownTunionfield).f1;_tmp166=*_tmp165;_tmp167=
_tmp166.name;_tmp168=_tmp166.is_xtunion;_tmp169=(_tmp160.KnownTunionfield).f2;
_tmp16A=*_tmp169;_tmp16B=_tmp16A.name;_LL116: {struct Cyc_PP_Doc*_tmp16C=Cyc_PP_text(
_tmp168?({const char*_tmp16F="xtunion ";_tag_dynforward(_tmp16F,sizeof(char),
_get_zero_arr_size_char(_tmp16F,9));}):({const char*_tmp170="tunion ";
_tag_dynforward(_tmp170,sizeof(char),_get_zero_arr_size_char(_tmp170,8));}));s=({
struct Cyc_PP_Doc*_tmp16D[4];_tmp16D[3]=Cyc_Absynpp_qvar2doc(_tmp16B);_tmp16D[2]=
Cyc_PP_text(({const char*_tmp16E=".";_tag_dynforward(_tmp16E,sizeof(char),
_get_zero_arr_size_char(_tmp16E,2));}));_tmp16D[1]=Cyc_Absynpp_qvar2doc(_tmp167);
_tmp16D[0]=_tmp16C;Cyc_PP_cat(_tag_dynforward(_tmp16D,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL112;}_LL112:;}goto _LLD1;_LLE2: if(*((int*)_tmp120)!= 5)goto _LLE4;
_tmp12D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp120)->f1;_tmp12E=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp120)->f2;_LLE3: {struct _dynforward_ptr sns;
struct _dynforward_ptr ts;{void*_tmp171=_tmp12D;_LL118: if((int)_tmp171 != 2)goto
_LL11A;_LL119: goto _LL11B;_LL11A: if((int)_tmp171 != 0)goto _LL11C;_LL11B: sns=({
const char*_tmp172="";_tag_dynforward(_tmp172,sizeof(char),
_get_zero_arr_size_char(_tmp172,1));});goto _LL117;_LL11C: if((int)_tmp171 != 1)
goto _LL117;_LL11D: sns=({const char*_tmp173="unsigned ";_tag_dynforward(_tmp173,
sizeof(char),_get_zero_arr_size_char(_tmp173,10));});goto _LL117;_LL117:;}{void*
_tmp174=_tmp12E;_LL11F: if((int)_tmp174 != 0)goto _LL121;_LL120:{void*_tmp175=
_tmp12D;_LL12A: if((int)_tmp175 != 2)goto _LL12C;_LL12B: sns=({const char*_tmp176="";
_tag_dynforward(_tmp176,sizeof(char),_get_zero_arr_size_char(_tmp176,1));});goto
_LL129;_LL12C: if((int)_tmp175 != 0)goto _LL12E;_LL12D: sns=({const char*_tmp177="signed ";
_tag_dynforward(_tmp177,sizeof(char),_get_zero_arr_size_char(_tmp177,8));});goto
_LL129;_LL12E: if((int)_tmp175 != 1)goto _LL129;_LL12F: sns=({const char*_tmp178="unsigned ";
_tag_dynforward(_tmp178,sizeof(char),_get_zero_arr_size_char(_tmp178,10));});
goto _LL129;_LL129:;}ts=({const char*_tmp179="char";_tag_dynforward(_tmp179,
sizeof(char),_get_zero_arr_size_char(_tmp179,5));});goto _LL11E;_LL121: if((int)
_tmp174 != 1)goto _LL123;_LL122: ts=({const char*_tmp17A="short";_tag_dynforward(
_tmp17A,sizeof(char),_get_zero_arr_size_char(_tmp17A,6));});goto _LL11E;_LL123:
if((int)_tmp174 != 2)goto _LL125;_LL124: ts=({const char*_tmp17B="int";
_tag_dynforward(_tmp17B,sizeof(char),_get_zero_arr_size_char(_tmp17B,4));});goto
_LL11E;_LL125: if((int)_tmp174 != 3)goto _LL127;_LL126: ts=({const char*_tmp17C="long";
_tag_dynforward(_tmp17C,sizeof(char),_get_zero_arr_size_char(_tmp17C,5));});goto
_LL11E;_LL127: if((int)_tmp174 != 4)goto _LL11E;_LL128:{void*_tmp17D=Cyc_Cyclone_c_compiler;
_LL131: if((int)_tmp17D != 0)goto _LL133;_LL132: ts=({const char*_tmp17E="long long";
_tag_dynforward(_tmp17E,sizeof(char),_get_zero_arr_size_char(_tmp17E,10));});
goto _LL130;_LL133: if((int)_tmp17D != 1)goto _LL130;_LL134: ts=({const char*_tmp17F="__int64";
_tag_dynforward(_tmp17F,sizeof(char),_get_zero_arr_size_char(_tmp17F,8));});goto
_LL130;_LL130:;}goto _LL11E;_LL11E:;}s=Cyc_PP_text((struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp183;_tmp183.tag=0;_tmp183.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)ts);{struct Cyc_String_pa_struct _tmp182;
_tmp182.tag=0;_tmp182.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)sns);{
void*_tmp180[2]={& _tmp182,& _tmp183};Cyc_aprintf(({const char*_tmp181="%s%s";
_tag_dynforward(_tmp181,sizeof(char),_get_zero_arr_size_char(_tmp181,5));}),
_tag_dynforward(_tmp180,sizeof(void*),2));}}}));goto _LLD1;}_LLE4: if((int)_tmp120
!= 1)goto _LLE6;_LLE5: s=Cyc_PP_text(({const char*_tmp184="float";_tag_dynforward(
_tmp184,sizeof(char),_get_zero_arr_size_char(_tmp184,6));}));goto _LLD1;_LLE6: if(
_tmp120 <= (void*)4)goto _LLFE;if(*((int*)_tmp120)!= 6)goto _LLE8;_tmp12F=((struct
Cyc_Absyn_DoubleType_struct*)_tmp120)->f1;_LLE7: if(_tmp12F)s=Cyc_PP_text(({const
char*_tmp185="long double";_tag_dynforward(_tmp185,sizeof(char),
_get_zero_arr_size_char(_tmp185,12));}));else{s=Cyc_PP_text(({const char*_tmp186="double";
_tag_dynforward(_tmp186,sizeof(char),_get_zero_arr_size_char(_tmp186,7));}));}
goto _LLD1;_LLE8: if(*((int*)_tmp120)!= 9)goto _LLEA;_tmp130=((struct Cyc_Absyn_TupleType_struct*)
_tmp120)->f1;_LLE9: s=({struct Cyc_PP_Doc*_tmp187[2];_tmp187[1]=Cyc_Absynpp_args2doc(
_tmp130);_tmp187[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dynforward(_tmp187,
sizeof(struct Cyc_PP_Doc*),2));});goto _LLD1;_LLEA: if(*((int*)_tmp120)!= 10)goto
_LLEC;_tmp131=((struct Cyc_Absyn_AggrType_struct*)_tmp120)->f1;_tmp132=_tmp131.aggr_info;
_tmp133=_tmp131.targs;_LLEB: {void*_tmp189;struct _tuple0*_tmp18A;struct _tuple3
_tmp188=Cyc_Absyn_aggr_kinded_name(_tmp132);_tmp189=_tmp188.f1;_tmp18A=_tmp188.f2;
s=({struct Cyc_PP_Doc*_tmp18B[3];_tmp18B[2]=Cyc_Absynpp_tps2doc(_tmp133);_tmp18B[
1]=Cyc_Absynpp_qvar2doc(_tmp18A);_tmp18B[0]=Cyc_Absynpp_aggr_kind2doc(_tmp189);
Cyc_PP_cat(_tag_dynforward(_tmp18B,sizeof(struct Cyc_PP_Doc*),3));});goto _LLD1;}
_LLEC: if(*((int*)_tmp120)!= 11)goto _LLEE;_tmp134=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp120)->f1;_tmp135=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp120)->f2;_LLED: s=({
struct Cyc_PP_Doc*_tmp18C[4];_tmp18C[3]=Cyc_Absynpp_rb();_tmp18C[2]=Cyc_PP_nest(2,
Cyc_Absynpp_aggrfields2doc(_tmp135));_tmp18C[1]=Cyc_Absynpp_lb();_tmp18C[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp134);Cyc_PP_cat(_tag_dynforward(_tmp18C,sizeof(struct Cyc_PP_Doc*),4));});
goto _LLD1;_LLEE: if(*((int*)_tmp120)!= 13)goto _LLF0;_tmp136=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp120)->f1;_LLEF: s=({struct Cyc_PP_Doc*_tmp18D[4];_tmp18D[3]=Cyc_Absynpp_rb();
_tmp18D[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp136));_tmp18D[1]=Cyc_Absynpp_lb();
_tmp18D[0]=Cyc_PP_text(({const char*_tmp18E="enum ";_tag_dynforward(_tmp18E,
sizeof(char),_get_zero_arr_size_char(_tmp18E,6));}));Cyc_PP_cat(_tag_dynforward(
_tmp18D,sizeof(struct Cyc_PP_Doc*),4));});goto _LLD1;_LLF0: if(*((int*)_tmp120)!= 
12)goto _LLF2;_tmp137=((struct Cyc_Absyn_EnumType_struct*)_tmp120)->f1;_LLF1: s=({
struct Cyc_PP_Doc*_tmp18F[2];_tmp18F[1]=Cyc_Absynpp_qvar2doc(_tmp137);_tmp18F[0]=
Cyc_PP_text(({const char*_tmp190="enum ";_tag_dynforward(_tmp190,sizeof(char),
_get_zero_arr_size_char(_tmp190,6));}));Cyc_PP_cat(_tag_dynforward(_tmp18F,
sizeof(struct Cyc_PP_Doc*),2));});goto _LLD1;_LLF2: if(*((int*)_tmp120)!= 14)goto
_LLF4;_tmp138=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp120)->f1;_LLF3: s=({
struct Cyc_PP_Doc*_tmp191[3];_tmp191[2]=Cyc_PP_text(({const char*_tmp193=">";
_tag_dynforward(_tmp193,sizeof(char),_get_zero_arr_size_char(_tmp193,2));}));
_tmp191[1]=Cyc_Absynpp_typ2doc(_tmp138);_tmp191[0]=Cyc_PP_text(({const char*
_tmp192="sizeof_t<";_tag_dynforward(_tmp192,sizeof(char),_get_zero_arr_size_char(
_tmp192,10));}));Cyc_PP_cat(_tag_dynforward(_tmp191,sizeof(struct Cyc_PP_Doc*),3));});
goto _LLD1;_LLF4: if(*((int*)_tmp120)!= 17)goto _LLF6;_tmp139=((struct Cyc_Absyn_TypedefType_struct*)
_tmp120)->f1;_tmp13A=((struct Cyc_Absyn_TypedefType_struct*)_tmp120)->f2;_tmp13B=((
struct Cyc_Absyn_TypedefType_struct*)_tmp120)->f3;_LLF5: s=({struct Cyc_PP_Doc*
_tmp194[2];_tmp194[1]=Cyc_Absynpp_tps2doc(_tmp13A);_tmp194[0]=Cyc_Absynpp_qvar2doc(
_tmp139);Cyc_PP_cat(_tag_dynforward(_tmp194,sizeof(struct Cyc_PP_Doc*),2));});
goto _LLD1;_LLF6: if(*((int*)_tmp120)!= 15)goto _LLF8;_tmp13C=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp120)->f1;_LLF7: s=({struct Cyc_PP_Doc*_tmp195[3];_tmp195[2]=Cyc_PP_text(({
const char*_tmp197=">";_tag_dynforward(_tmp197,sizeof(char),
_get_zero_arr_size_char(_tmp197,2));}));_tmp195[1]=Cyc_Absynpp_rgn2doc(_tmp13C);
_tmp195[0]=Cyc_PP_text(({const char*_tmp196="region_t<";_tag_dynforward(_tmp196,
sizeof(char),_get_zero_arr_size_char(_tmp196,10));}));Cyc_PP_cat(_tag_dynforward(
_tmp195,sizeof(struct Cyc_PP_Doc*),3));});goto _LLD1;_LLF8: if(*((int*)_tmp120)!= 
16)goto _LLFA;_tmp13D=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp120)->f1;
_tmp13E=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp120)->f2;_LLF9: s=({
struct Cyc_PP_Doc*_tmp198[5];_tmp198[4]=Cyc_PP_text(({const char*_tmp19B=">";
_tag_dynforward(_tmp19B,sizeof(char),_get_zero_arr_size_char(_tmp19B,2));}));
_tmp198[3]=Cyc_Absynpp_rgn2doc(_tmp13E);_tmp198[2]=Cyc_PP_text(({const char*
_tmp19A=",";_tag_dynforward(_tmp19A,sizeof(char),_get_zero_arr_size_char(_tmp19A,
2));}));_tmp198[1]=Cyc_Absynpp_rgn2doc(_tmp13D);_tmp198[0]=Cyc_PP_text(({const
char*_tmp199="dynregion_t<";_tag_dynforward(_tmp199,sizeof(char),
_get_zero_arr_size_char(_tmp199,13));}));Cyc_PP_cat(_tag_dynforward(_tmp198,
sizeof(struct Cyc_PP_Doc*),5));});goto _LLD1;_LLFA: if(*((int*)_tmp120)!= 19)goto
_LLFC;_tmp13F=((struct Cyc_Absyn_TypeInt_struct*)_tmp120)->f1;_LLFB: s=Cyc_PP_text((
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp19E;_tmp19E.tag=1;_tmp19E.f1=(
unsigned long)_tmp13F;{void*_tmp19C[1]={& _tmp19E};Cyc_aprintf(({const char*
_tmp19D="`%d";_tag_dynforward(_tmp19D,sizeof(char),_get_zero_arr_size_char(
_tmp19D,4));}),_tag_dynforward(_tmp19C,sizeof(void*),1));}}));goto _LLD1;_LLFC:
if(*((int*)_tmp120)!= 18)goto _LLFE;_tmp140=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp120)->f1;_LLFD: s=({struct Cyc_PP_Doc*_tmp19F[3];_tmp19F[2]=Cyc_PP_text(({
const char*_tmp1A1=">";_tag_dynforward(_tmp1A1,sizeof(char),
_get_zero_arr_size_char(_tmp1A1,2));}));_tmp19F[1]=Cyc_Absynpp_typ2doc(_tmp140);
_tmp19F[0]=Cyc_PP_text(({const char*_tmp1A0="tag_t<";_tag_dynforward(_tmp1A0,
sizeof(char),_get_zero_arr_size_char(_tmp1A0,7));}));Cyc_PP_cat(_tag_dynforward(
_tmp19F,sizeof(struct Cyc_PP_Doc*),3));});goto _LLD1;_LLFE: if((int)_tmp120 != 3)
goto _LL100;_LLFF: goto _LL101;_LL100: if((int)_tmp120 != 2)goto _LL102;_LL101: s=Cyc_Absynpp_rgn2doc(
t);goto _LLD1;_LL102: if(_tmp120 <= (void*)4)goto _LL104;if(*((int*)_tmp120)!= 22)
goto _LL104;_tmp141=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp120)->f1;_LL103:
s=({struct Cyc_PP_Doc*_tmp1A2[3];_tmp1A2[2]=Cyc_PP_text(({const char*_tmp1A4=")";
_tag_dynforward(_tmp1A4,sizeof(char),_get_zero_arr_size_char(_tmp1A4,2));}));
_tmp1A2[1]=Cyc_Absynpp_typ2doc(_tmp141);_tmp1A2[0]=Cyc_PP_text(({const char*
_tmp1A3="regions(";_tag_dynforward(_tmp1A3,sizeof(char),_get_zero_arr_size_char(
_tmp1A3,9));}));Cyc_PP_cat(_tag_dynforward(_tmp1A2,sizeof(struct Cyc_PP_Doc*),3));});
goto _LLD1;_LL104: if(_tmp120 <= (void*)4)goto _LL106;if(*((int*)_tmp120)!= 20)goto
_LL106;_LL105: goto _LL107;_LL106: if(_tmp120 <= (void*)4)goto _LLD1;if(*((int*)
_tmp120)!= 21)goto _LLD1;_LL107: s=Cyc_Absynpp_eff2doc(t);goto _LLD1;_LLD1:;}return
s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo){return vo == 0?Cyc_PP_nil_doc():
Cyc_PP_text(*((struct _dynforward_ptr*)vo->v));}struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(
struct _tuple5*cmp){struct _tuple5 _tmp1A6;void*_tmp1A7;void*_tmp1A8;struct _tuple5*
_tmp1A5=cmp;_tmp1A6=*_tmp1A5;_tmp1A7=_tmp1A6.f1;_tmp1A8=_tmp1A6.f2;return({
struct Cyc_PP_Doc*_tmp1A9[3];_tmp1A9[2]=Cyc_Absynpp_rgn2doc(_tmp1A8);_tmp1A9[1]=
Cyc_PP_text(({const char*_tmp1AA=" > ";_tag_dynforward(_tmp1AA,sizeof(char),
_get_zero_arr_size_char(_tmp1AA,4));}));_tmp1A9[0]=Cyc_Absynpp_rgn2doc(_tmp1A7);
Cyc_PP_cat(_tag_dynforward(_tmp1A9,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*
Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){return Cyc_PP_group(({const char*
_tmp1AB="";_tag_dynforward(_tmp1AB,sizeof(char),_get_zero_arr_size_char(_tmp1AB,
1));}),({const char*_tmp1AC="";_tag_dynforward(_tmp1AC,sizeof(char),
_get_zero_arr_size_char(_tmp1AC,1));}),({const char*_tmp1AD=",";_tag_dynforward(
_tmp1AD,sizeof(char),_get_zero_arr_size_char(_tmp1AD,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,
po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple1*t){struct Cyc_Core_Opt*
dopt=(*t).f1 == 0?0:({struct Cyc_Core_Opt*_tmp1AE=_cycalloc(sizeof(*_tmp1AE));
_tmp1AE->v=Cyc_PP_text(*((struct _dynforward_ptr*)((struct Cyc_Core_Opt*)
_check_null((*t).f1))->v));_tmp1AE;});return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,
dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp1AF=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs)_tmp1AF=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1AF,({struct
Cyc_List_List*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0->hd=Cyc_PP_text(({
const char*_tmp1B1="...";_tag_dynforward(_tmp1B1,sizeof(char),
_get_zero_arr_size_char(_tmp1B1,4));}));_tmp1B0->tl=0;_tmp1B0;}));else{if(
cyc_varargs != 0){struct Cyc_PP_Doc*_tmp1B2=({struct Cyc_PP_Doc*_tmp1B4[3];_tmp1B4[
2]=Cyc_Absynpp_funarg2doc(({struct _tuple1*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));
_tmp1B8->f1=cyc_varargs->name;_tmp1B8->f2=cyc_varargs->tq;_tmp1B8->f3=(void*)
cyc_varargs->type;_tmp1B8;}));_tmp1B4[1]=cyc_varargs->inject?Cyc_PP_text(({const
char*_tmp1B6=" inject ";_tag_dynforward(_tmp1B6,sizeof(char),
_get_zero_arr_size_char(_tmp1B6,9));})): Cyc_PP_text(({const char*_tmp1B7=" ";
_tag_dynforward(_tmp1B7,sizeof(char),_get_zero_arr_size_char(_tmp1B7,2));}));
_tmp1B4[0]=Cyc_PP_text(({const char*_tmp1B5="...";_tag_dynforward(_tmp1B5,sizeof(
char),_get_zero_arr_size_char(_tmp1B5,4));}));Cyc_PP_cat(_tag_dynforward(_tmp1B4,
sizeof(struct Cyc_PP_Doc*),3));});_tmp1AF=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1AF,({struct Cyc_List_List*_tmp1B3=
_cycalloc(sizeof(*_tmp1B3));_tmp1B3->hd=_tmp1B2;_tmp1B3->tl=0;_tmp1B3;}));}}{
struct Cyc_PP_Doc*_tmp1B9=Cyc_PP_group(({const char*_tmp1C1="";_tag_dynforward(
_tmp1C1,sizeof(char),_get_zero_arr_size_char(_tmp1C1,1));}),({const char*_tmp1C2="";
_tag_dynforward(_tmp1C2,sizeof(char),_get_zero_arr_size_char(_tmp1C2,1));}),({
const char*_tmp1C3=",";_tag_dynforward(_tmp1C3,sizeof(char),
_get_zero_arr_size_char(_tmp1C3,2));}),_tmp1AF);if(effopt != 0  && Cyc_Absynpp_print_all_effects)
_tmp1B9=({struct Cyc_PP_Doc*_tmp1BA[3];_tmp1BA[2]=Cyc_Absynpp_eff2doc((void*)
effopt->v);_tmp1BA[1]=Cyc_PP_text(({const char*_tmp1BB=";";_tag_dynforward(
_tmp1BB,sizeof(char),_get_zero_arr_size_char(_tmp1BB,2));}));_tmp1BA[0]=_tmp1B9;
Cyc_PP_cat(_tag_dynforward(_tmp1BA,sizeof(struct Cyc_PP_Doc*),3));});if(rgn_po != 
0)_tmp1B9=({struct Cyc_PP_Doc*_tmp1BC[3];_tmp1BC[2]=Cyc_Absynpp_rgnpo2doc(rgn_po);
_tmp1BC[1]=Cyc_PP_text(({const char*_tmp1BD=":";_tag_dynforward(_tmp1BD,sizeof(
char),_get_zero_arr_size_char(_tmp1BD,2));}));_tmp1BC[0]=_tmp1B9;Cyc_PP_cat(
_tag_dynforward(_tmp1BC,sizeof(struct Cyc_PP_Doc*),3));});return({struct Cyc_PP_Doc*
_tmp1BE[3];_tmp1BE[2]=Cyc_PP_text(({const char*_tmp1C0=")";_tag_dynforward(
_tmp1C0,sizeof(char),_get_zero_arr_size_char(_tmp1C0,2));}));_tmp1BE[1]=_tmp1B9;
_tmp1BE[0]=Cyc_PP_text(({const char*_tmp1BF="(";_tag_dynforward(_tmp1BF,sizeof(
char),_get_zero_arr_size_char(_tmp1BF,2));}));Cyc_PP_cat(_tag_dynforward(_tmp1BE,
sizeof(struct Cyc_PP_Doc*),3));});}}struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct
_tuple6*arg){return({struct _tuple1*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->f1=({
struct Cyc_Core_Opt*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5->v=(*arg).f1;
_tmp1C5;});_tmp1C4->f2=(*arg).f2;_tmp1C4->f3=(*arg).f3;_tmp1C4;});}struct Cyc_PP_Doc*
Cyc_Absynpp_var2doc(struct _dynforward_ptr*v){return Cyc_PP_text(*v);}struct
_dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){struct Cyc_List_List*
_tmp1C6=0;int match;{union Cyc_Absyn_Nmspace_union _tmp1C7=(*q).f1;struct Cyc_List_List*
_tmp1C8;struct Cyc_List_List*_tmp1C9;_LL136: if((_tmp1C7.Loc_n).tag != 0)goto _LL138;
_LL137: _tmp1C8=0;goto _LL139;_LL138: if((_tmp1C7.Rel_n).tag != 1)goto _LL13A;_tmp1C8=(
_tmp1C7.Rel_n).f1;_LL139: match=0;_tmp1C6=_tmp1C8;goto _LL135;_LL13A: if((_tmp1C7.Abs_n).tag
!= 2)goto _LL135;_tmp1C9=(_tmp1C7.Abs_n).f1;_LL13B: match=Cyc_Absynpp_use_curr_namespace
 && ((int(*)(int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1C9,Cyc_Absynpp_curr_namespace);
_tmp1C6=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*
_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->hd=Cyc_Absynpp_cyc_stringptr;
_tmp1CA->tl=_tmp1C9;_tmp1CA;}): _tmp1C9;goto _LL135;_LL135:;}if(Cyc_Absynpp_qvar_to_Cids)
return(struct _dynforward_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1C6,({struct Cyc_List_List*_tmp1CB=
_cycalloc(sizeof(*_tmp1CB));_tmp1CB->hd=(*q).f2;_tmp1CB->tl=0;_tmp1CB;})),({
const char*_tmp1CC="_";_tag_dynforward(_tmp1CC,sizeof(char),
_get_zero_arr_size_char(_tmp1CC,2));}));else{if(match)return*(*q).f2;else{return(
struct _dynforward_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1C6,({struct Cyc_List_List*_tmp1CD=
_cycalloc(sizeof(*_tmp1CD));_tmp1CD->hd=(*q).f2;_tmp1CD->tl=0;_tmp1CD;})),({
const char*_tmp1CE="::";_tag_dynforward(_tmp1CE,sizeof(char),
_get_zero_arr_size_char(_tmp1CE,3));}));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q){return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*
Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){struct _dynforward_ptr _tmp1CF=Cyc_Absynpp_qvar2string(
q);if(Cyc_PP_tex_output)return Cyc_PP_text_width((struct _dynforward_ptr)Cyc_strconcat((
struct _dynforward_ptr)Cyc_strconcat(({const char*_tmp1D0="\\textbf{";
_tag_dynforward(_tmp1D0,sizeof(char),_get_zero_arr_size_char(_tmp1D0,9));}),(
struct _dynforward_ptr)_tmp1CF),({const char*_tmp1D1="}";_tag_dynforward(_tmp1D1,
sizeof(char),_get_zero_arr_size_char(_tmp1D1,2));})),(int)Cyc_strlen((struct
_dynforward_ptr)_tmp1CF));else{return Cyc_PP_text(_tmp1CF);}}struct
_dynforward_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)
return Cyc_Absynpp_qvar2string(v);if(Cyc_Absynpp_use_curr_namespace){union Cyc_Absyn_Nmspace_union
_tmp1D2=(*v).f1;struct Cyc_List_List*_tmp1D3;struct Cyc_List_List*_tmp1D4;_LL13D:
if((_tmp1D2.Loc_n).tag != 0)goto _LL13F;_LL13E: goto _LL140;_LL13F: if((_tmp1D2.Rel_n).tag
!= 1)goto _LL141;_tmp1D3=(_tmp1D2.Rel_n).f1;if(_tmp1D3 != 0)goto _LL141;_LL140:
return*(*v).f2;_LL141: if((_tmp1D2.Abs_n).tag != 2)goto _LL143;_tmp1D4=(_tmp1D2.Abs_n).f1;
_LL142: if(((int(*)(int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,
_tmp1D4,Cyc_Absynpp_curr_namespace)== 0)return*(*v).f2;else{goto _LL144;}_LL143:;
_LL144: return(struct _dynforward_ptr)Cyc_strconcat(({const char*_tmp1D5="/* bad namespace : */ ";
_tag_dynforward(_tmp1D5,sizeof(char),_get_zero_arr_size_char(_tmp1D5,23));}),(
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(v));_LL13C:;}else{return*(*v).f2;}}
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){return Cyc_PP_text(
Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v){struct _dynforward_ptr _tmp1D6=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output)return Cyc_PP_text_width((struct _dynforward_ptr)Cyc_strconcat((
struct _dynforward_ptr)Cyc_strconcat(({const char*_tmp1D7="\\textbf{";
_tag_dynforward(_tmp1D7,sizeof(char),_get_zero_arr_size_char(_tmp1D7,9));}),(
struct _dynforward_ptr)_tmp1D6),({const char*_tmp1D8="}";_tag_dynforward(_tmp1D8,
sizeof(char),_get_zero_arr_size_char(_tmp1D8,2));})),(int)Cyc_strlen((struct
_dynforward_ptr)_tmp1D6));else{return Cyc_PP_text(_tmp1D6);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*t){return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*e){void*_tmp1D9=(void*)e->r;void*_tmp1DA;struct Cyc_Absyn_Exp*
_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DC;_LL146: if(*((int*)_tmp1D9)!= 0)goto _LL148;
_LL147: goto _LL149;_LL148: if(*((int*)_tmp1D9)!= 1)goto _LL14A;_LL149: goto _LL14B;
_LL14A: if(*((int*)_tmp1D9)!= 2)goto _LL14C;_LL14B: return 10000;_LL14C: if(*((int*)
_tmp1D9)!= 3)goto _LL14E;_tmp1DA=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D9)->f1;_LL14D: {void*_tmp1DD=_tmp1DA;_LL195: if((int)_tmp1DD != 0)goto _LL197;
_LL196: return 100;_LL197: if((int)_tmp1DD != 1)goto _LL199;_LL198: return 110;_LL199:
if((int)_tmp1DD != 2)goto _LL19B;_LL19A: return 100;_LL19B: if((int)_tmp1DD != 3)goto
_LL19D;_LL19C: goto _LL19E;_LL19D: if((int)_tmp1DD != 4)goto _LL19F;_LL19E: return 110;
_LL19F: if((int)_tmp1DD != 5)goto _LL1A1;_LL1A0: goto _LL1A2;_LL1A1: if((int)_tmp1DD != 
6)goto _LL1A3;_LL1A2: return 70;_LL1A3: if((int)_tmp1DD != 7)goto _LL1A5;_LL1A4: goto
_LL1A6;_LL1A5: if((int)_tmp1DD != 8)goto _LL1A7;_LL1A6: goto _LL1A8;_LL1A7: if((int)
_tmp1DD != 9)goto _LL1A9;_LL1A8: goto _LL1AA;_LL1A9: if((int)_tmp1DD != 10)goto _LL1AB;
_LL1AA: return 80;_LL1AB: if((int)_tmp1DD != 11)goto _LL1AD;_LL1AC: goto _LL1AE;_LL1AD:
if((int)_tmp1DD != 12)goto _LL1AF;_LL1AE: return 130;_LL1AF: if((int)_tmp1DD != 13)
goto _LL1B1;_LL1B0: return 60;_LL1B1: if((int)_tmp1DD != 14)goto _LL1B3;_LL1B2: return
40;_LL1B3: if((int)_tmp1DD != 15)goto _LL1B5;_LL1B4: return 50;_LL1B5: if((int)_tmp1DD
!= 16)goto _LL1B7;_LL1B6: return 90;_LL1B7: if((int)_tmp1DD != 17)goto _LL1B9;_LL1B8:
return 80;_LL1B9: if((int)_tmp1DD != 18)goto _LL1BB;_LL1BA: return 80;_LL1BB: if((int)
_tmp1DD != 19)goto _LL194;_LL1BC: return 140;_LL194:;}_LL14E: if(*((int*)_tmp1D9)!= 4)
goto _LL150;_LL14F: return 20;_LL150: if(*((int*)_tmp1D9)!= 5)goto _LL152;_LL151:
return 130;_LL152: if(*((int*)_tmp1D9)!= 6)goto _LL154;_LL153: return 30;_LL154: if(*((
int*)_tmp1D9)!= 7)goto _LL156;_LL155: return 35;_LL156: if(*((int*)_tmp1D9)!= 8)goto
_LL158;_LL157: return 30;_LL158: if(*((int*)_tmp1D9)!= 9)goto _LL15A;_LL159: return 10;
_LL15A: if(*((int*)_tmp1D9)!= 10)goto _LL15C;_LL15B: goto _LL15D;_LL15C: if(*((int*)
_tmp1D9)!= 11)goto _LL15E;_LL15D: return 140;_LL15E: if(*((int*)_tmp1D9)!= 12)goto
_LL160;_LL15F: return 130;_LL160: if(*((int*)_tmp1D9)!= 13)goto _LL162;_tmp1DB=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1D9)->f1;_LL161: return Cyc_Absynpp_exp_prec(
_tmp1DB);_LL162: if(*((int*)_tmp1D9)!= 14)goto _LL164;_tmp1DC=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1D9)->f1;_LL163: return Cyc_Absynpp_exp_prec(_tmp1DC);_LL164: if(*((int*)
_tmp1D9)!= 15)goto _LL166;_LL165: return 120;_LL166: if(*((int*)_tmp1D9)!= 17)goto
_LL168;_LL167: return 15;_LL168: if(*((int*)_tmp1D9)!= 16)goto _LL16A;_LL169: goto
_LL16B;_LL16A: if(*((int*)_tmp1D9)!= 18)goto _LL16C;_LL16B: goto _LL16D;_LL16C: if(*((
int*)_tmp1D9)!= 19)goto _LL16E;_LL16D: goto _LL16F;_LL16E: if(*((int*)_tmp1D9)!= 20)
goto _LL170;_LL16F: goto _LL171;_LL170: if(*((int*)_tmp1D9)!= 21)goto _LL172;_LL171:
goto _LL173;_LL172: if(*((int*)_tmp1D9)!= 22)goto _LL174;_LL173: return 130;_LL174:
if(*((int*)_tmp1D9)!= 23)goto _LL176;_LL175: goto _LL177;_LL176: if(*((int*)_tmp1D9)
!= 24)goto _LL178;_LL177: goto _LL179;_LL178: if(*((int*)_tmp1D9)!= 25)goto _LL17A;
_LL179: return 140;_LL17A: if(*((int*)_tmp1D9)!= 26)goto _LL17C;_LL17B: return 150;
_LL17C: if(*((int*)_tmp1D9)!= 27)goto _LL17E;_LL17D: goto _LL17F;_LL17E: if(*((int*)
_tmp1D9)!= 28)goto _LL180;_LL17F: goto _LL181;_LL180: if(*((int*)_tmp1D9)!= 29)goto
_LL182;_LL181: goto _LL183;_LL182: if(*((int*)_tmp1D9)!= 30)goto _LL184;_LL183: goto
_LL185;_LL184: if(*((int*)_tmp1D9)!= 31)goto _LL186;_LL185: goto _LL187;_LL186: if(*((
int*)_tmp1D9)!= 32)goto _LL188;_LL187: goto _LL189;_LL188: if(*((int*)_tmp1D9)!= 33)
goto _LL18A;_LL189: goto _LL18B;_LL18A: if(*((int*)_tmp1D9)!= 34)goto _LL18C;_LL18B:
goto _LL18D;_LL18C: if(*((int*)_tmp1D9)!= 35)goto _LL18E;_LL18D: goto _LL18F;_LL18E:
if(*((int*)_tmp1D9)!= 36)goto _LL190;_LL18F: goto _LL191;_LL190: if(*((int*)_tmp1D9)
!= 37)goto _LL192;_LL191: return 140;_LL192: if(*((int*)_tmp1D9)!= 38)goto _LL145;
_LL193: return 10000;_LL145:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*
e){return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmp1DE=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp1DF;struct _tuple0*
_tmp1E0;struct _tuple0*_tmp1E1;void*_tmp1E2;struct Cyc_List_List*_tmp1E3;struct Cyc_Absyn_Exp*
_tmp1E4;struct Cyc_Core_Opt*_tmp1E5;struct Cyc_Absyn_Exp*_tmp1E6;struct Cyc_Absyn_Exp*
_tmp1E7;void*_tmp1E8;struct Cyc_Absyn_Exp*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1EA;
struct Cyc_Absyn_Exp*_tmp1EB;struct Cyc_Absyn_Exp*_tmp1EC;struct Cyc_Absyn_Exp*
_tmp1ED;struct Cyc_Absyn_Exp*_tmp1EE;struct Cyc_Absyn_Exp*_tmp1EF;struct Cyc_Absyn_Exp*
_tmp1F0;struct Cyc_Absyn_Exp*_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F2;struct Cyc_List_List*
_tmp1F3;struct Cyc_Absyn_Exp*_tmp1F4;struct Cyc_List_List*_tmp1F5;struct Cyc_Absyn_Exp*
_tmp1F6;struct Cyc_Absyn_Exp*_tmp1F7;struct Cyc_Absyn_Exp*_tmp1F8;void*_tmp1F9;
struct Cyc_Absyn_Exp*_tmp1FA;struct Cyc_Absyn_Exp*_tmp1FB;struct Cyc_Absyn_Exp*
_tmp1FC;struct Cyc_Absyn_Exp*_tmp1FD;void*_tmp1FE;struct Cyc_Absyn_Exp*_tmp1FF;
void*_tmp200;void*_tmp201;struct _dynforward_ptr*_tmp202;void*_tmp203;void*
_tmp204;unsigned int _tmp205;struct Cyc_List_List*_tmp206;void*_tmp207;struct Cyc_Absyn_Exp*
_tmp208;struct Cyc_Absyn_Exp*_tmp209;struct _dynforward_ptr*_tmp20A;struct Cyc_Absyn_Exp*
_tmp20B;struct _dynforward_ptr*_tmp20C;struct Cyc_Absyn_Exp*_tmp20D;struct Cyc_Absyn_Exp*
_tmp20E;struct Cyc_List_List*_tmp20F;struct _tuple1*_tmp210;struct Cyc_List_List*
_tmp211;struct Cyc_List_List*_tmp212;struct Cyc_Absyn_Vardecl*_tmp213;struct Cyc_Absyn_Exp*
_tmp214;struct Cyc_Absyn_Exp*_tmp215;struct _tuple0*_tmp216;struct Cyc_List_List*
_tmp217;struct Cyc_List_List*_tmp218;struct Cyc_List_List*_tmp219;struct Cyc_List_List*
_tmp21A;struct Cyc_Absyn_Tunionfield*_tmp21B;struct _tuple0*_tmp21C;struct _tuple0*
_tmp21D;struct Cyc_Absyn_MallocInfo _tmp21E;int _tmp21F;struct Cyc_Absyn_Exp*_tmp220;
void**_tmp221;struct Cyc_Absyn_Exp*_tmp222;struct Cyc_Absyn_Exp*_tmp223;struct Cyc_Absyn_Exp*
_tmp224;struct Cyc_Core_Opt*_tmp225;struct Cyc_List_List*_tmp226;struct Cyc_Absyn_Stmt*
_tmp227;_LL1BE: if(*((int*)_tmp1DE)!= 0)goto _LL1C0;_tmp1DF=((struct Cyc_Absyn_Const_e_struct*)
_tmp1DE)->f1;_LL1BF: s=Cyc_Absynpp_cnst2doc(_tmp1DF);goto _LL1BD;_LL1C0: if(*((int*)
_tmp1DE)!= 1)goto _LL1C2;_tmp1E0=((struct Cyc_Absyn_Var_e_struct*)_tmp1DE)->f1;
_LL1C1: _tmp1E1=_tmp1E0;goto _LL1C3;_LL1C2: if(*((int*)_tmp1DE)!= 2)goto _LL1C4;
_tmp1E1=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp1DE)->f1;_LL1C3: s=Cyc_Absynpp_qvar2doc(
_tmp1E1);goto _LL1BD;_LL1C4: if(*((int*)_tmp1DE)!= 3)goto _LL1C6;_tmp1E2=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp1DE)->f1;_tmp1E3=((struct Cyc_Absyn_Primop_e_struct*)
_tmp1DE)->f2;_LL1C5: s=Cyc_Absynpp_primapp2doc(myprec,_tmp1E2,_tmp1E3);goto _LL1BD;
_LL1C6: if(*((int*)_tmp1DE)!= 4)goto _LL1C8;_tmp1E4=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1DE)->f1;_tmp1E5=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1DE)->f2;_tmp1E6=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp1DE)->f3;_LL1C7: s=({struct Cyc_PP_Doc*
_tmp228[5];_tmp228[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E6);_tmp228[3]=Cyc_PP_text(({
const char*_tmp22A="= ";_tag_dynforward(_tmp22A,sizeof(char),
_get_zero_arr_size_char(_tmp22A,3));}));_tmp228[2]=_tmp1E5 == 0?Cyc_PP_nil_doc():
Cyc_Absynpp_prim2doc((void*)_tmp1E5->v);_tmp228[1]=Cyc_PP_text(({const char*
_tmp229=" ";_tag_dynforward(_tmp229,sizeof(char),_get_zero_arr_size_char(_tmp229,
2));}));_tmp228[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E4);Cyc_PP_cat(
_tag_dynforward(_tmp228,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1BD;_LL1C8: if(*((
int*)_tmp1DE)!= 5)goto _LL1CA;_tmp1E7=((struct Cyc_Absyn_Increment_e_struct*)
_tmp1DE)->f1;_tmp1E8=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp1DE)->f2;
_LL1C9: {struct Cyc_PP_Doc*_tmp22B=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E7);{void*
_tmp22C=_tmp1E8;_LL20F: if((int)_tmp22C != 0)goto _LL211;_LL210: s=({struct Cyc_PP_Doc*
_tmp22D[2];_tmp22D[1]=_tmp22B;_tmp22D[0]=Cyc_PP_text(({const char*_tmp22E="++";
_tag_dynforward(_tmp22E,sizeof(char),_get_zero_arr_size_char(_tmp22E,3));}));Cyc_PP_cat(
_tag_dynforward(_tmp22D,sizeof(struct Cyc_PP_Doc*),2));});goto _LL20E;_LL211: if((
int)_tmp22C != 2)goto _LL213;_LL212: s=({struct Cyc_PP_Doc*_tmp22F[2];_tmp22F[1]=
_tmp22B;_tmp22F[0]=Cyc_PP_text(({const char*_tmp230="--";_tag_dynforward(_tmp230,
sizeof(char),_get_zero_arr_size_char(_tmp230,3));}));Cyc_PP_cat(_tag_dynforward(
_tmp22F,sizeof(struct Cyc_PP_Doc*),2));});goto _LL20E;_LL213: if((int)_tmp22C != 1)
goto _LL215;_LL214: s=({struct Cyc_PP_Doc*_tmp231[2];_tmp231[1]=Cyc_PP_text(({const
char*_tmp232="++";_tag_dynforward(_tmp232,sizeof(char),_get_zero_arr_size_char(
_tmp232,3));}));_tmp231[0]=_tmp22B;Cyc_PP_cat(_tag_dynforward(_tmp231,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL20E;_LL215: if((int)_tmp22C != 3)goto _LL20E;
_LL216: s=({struct Cyc_PP_Doc*_tmp233[2];_tmp233[1]=Cyc_PP_text(({const char*
_tmp234="--";_tag_dynforward(_tmp234,sizeof(char),_get_zero_arr_size_char(
_tmp234,3));}));_tmp233[0]=_tmp22B;Cyc_PP_cat(_tag_dynforward(_tmp233,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL20E;_LL20E:;}goto _LL1BD;}_LL1CA: if(*((int*)
_tmp1DE)!= 6)goto _LL1CC;_tmp1E9=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1DE)->f1;
_tmp1EA=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1DE)->f2;_tmp1EB=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp1DE)->f3;_LL1CB: s=({struct Cyc_PP_Doc*_tmp235[5];_tmp235[4]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EB);_tmp235[3]=Cyc_PP_text(({const char*_tmp237=" : ";_tag_dynforward(
_tmp237,sizeof(char),_get_zero_arr_size_char(_tmp237,4));}));_tmp235[2]=Cyc_Absynpp_exp2doc_prec(
0,_tmp1EA);_tmp235[1]=Cyc_PP_text(({const char*_tmp236=" ? ";_tag_dynforward(
_tmp236,sizeof(char),_get_zero_arr_size_char(_tmp236,4));}));_tmp235[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E9);Cyc_PP_cat(_tag_dynforward(_tmp235,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL1BD;_LL1CC: if(*((int*)_tmp1DE)!= 7)goto _LL1CE;_tmp1EC=((struct Cyc_Absyn_And_e_struct*)
_tmp1DE)->f1;_tmp1ED=((struct Cyc_Absyn_And_e_struct*)_tmp1DE)->f2;_LL1CD: s=({
struct Cyc_PP_Doc*_tmp238[3];_tmp238[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1ED);
_tmp238[1]=Cyc_PP_text(({const char*_tmp239=" && ";_tag_dynforward(_tmp239,
sizeof(char),_get_zero_arr_size_char(_tmp239,5));}));_tmp238[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EC);Cyc_PP_cat(_tag_dynforward(_tmp238,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1BD;_LL1CE: if(*((int*)_tmp1DE)!= 8)goto _LL1D0;_tmp1EE=((struct Cyc_Absyn_Or_e_struct*)
_tmp1DE)->f1;_tmp1EF=((struct Cyc_Absyn_Or_e_struct*)_tmp1DE)->f2;_LL1CF: s=({
struct Cyc_PP_Doc*_tmp23A[3];_tmp23A[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1EF);
_tmp23A[1]=Cyc_PP_text(({const char*_tmp23B=" || ";_tag_dynforward(_tmp23B,
sizeof(char),_get_zero_arr_size_char(_tmp23B,5));}));_tmp23A[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EE);Cyc_PP_cat(_tag_dynforward(_tmp23A,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1BD;_LL1D0: if(*((int*)_tmp1DE)!= 9)goto _LL1D2;_tmp1F0=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1DE)->f1;_tmp1F1=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp1DE)->f2;_LL1D1: s=({
struct Cyc_PP_Doc*_tmp23C[5];_tmp23C[4]=Cyc_PP_text(({const char*_tmp23F=")";
_tag_dynforward(_tmp23F,sizeof(char),_get_zero_arr_size_char(_tmp23F,2));}));
_tmp23C[3]=Cyc_Absynpp_exp2doc(_tmp1F1);_tmp23C[2]=Cyc_PP_text(({const char*
_tmp23E=", ";_tag_dynforward(_tmp23E,sizeof(char),_get_zero_arr_size_char(
_tmp23E,3));}));_tmp23C[1]=Cyc_Absynpp_exp2doc(_tmp1F0);_tmp23C[0]=Cyc_PP_text(({
const char*_tmp23D="(";_tag_dynforward(_tmp23D,sizeof(char),
_get_zero_arr_size_char(_tmp23D,2));}));Cyc_PP_cat(_tag_dynforward(_tmp23C,
sizeof(struct Cyc_PP_Doc*),5));});goto _LL1BD;_LL1D2: if(*((int*)_tmp1DE)!= 10)goto
_LL1D4;_tmp1F2=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1DE)->f1;_tmp1F3=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1DE)->f2;_LL1D3: s=({struct Cyc_PP_Doc*
_tmp240[4];_tmp240[3]=Cyc_PP_text(({const char*_tmp242=")";_tag_dynforward(
_tmp242,sizeof(char),_get_zero_arr_size_char(_tmp242,2));}));_tmp240[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp1F3);_tmp240[1]=Cyc_PP_text(({const char*_tmp241="(";_tag_dynforward(
_tmp241,sizeof(char),_get_zero_arr_size_char(_tmp241,2));}));_tmp240[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F2);Cyc_PP_cat(_tag_dynforward(_tmp240,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL1BD;_LL1D4: if(*((int*)_tmp1DE)!= 11)goto _LL1D6;_tmp1F4=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1DE)->f1;_tmp1F5=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1DE)->f2;_LL1D5: s=({
struct Cyc_PP_Doc*_tmp243[4];_tmp243[3]=Cyc_PP_text(({const char*_tmp245=")";
_tag_dynforward(_tmp245,sizeof(char),_get_zero_arr_size_char(_tmp245,2));}));
_tmp243[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp1F5);_tmp243[1]=Cyc_PP_text(({const
char*_tmp244="(";_tag_dynforward(_tmp244,sizeof(char),_get_zero_arr_size_char(
_tmp244,2));}));_tmp243[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F4);Cyc_PP_cat(
_tag_dynforward(_tmp243,sizeof(struct Cyc_PP_Doc*),4));});goto _LL1BD;_LL1D6: if(*((
int*)_tmp1DE)!= 12)goto _LL1D8;_tmp1F6=((struct Cyc_Absyn_Throw_e_struct*)_tmp1DE)->f1;
_LL1D7: s=({struct Cyc_PP_Doc*_tmp246[2];_tmp246[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F6);_tmp246[0]=Cyc_PP_text(({const char*_tmp247="throw ";
_tag_dynforward(_tmp247,sizeof(char),_get_zero_arr_size_char(_tmp247,7));}));Cyc_PP_cat(
_tag_dynforward(_tmp246,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1BD;_LL1D8: if(*((
int*)_tmp1DE)!= 13)goto _LL1DA;_tmp1F7=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp1DE)->f1;_LL1D9: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1F7);goto _LL1BD;_LL1DA:
if(*((int*)_tmp1DE)!= 14)goto _LL1DC;_tmp1F8=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1DE)->f1;_LL1DB: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1F8);goto _LL1BD;_LL1DC:
if(*((int*)_tmp1DE)!= 15)goto _LL1DE;_tmp1F9=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp1DE)->f1;_tmp1FA=((struct Cyc_Absyn_Cast_e_struct*)_tmp1DE)->f2;_LL1DD: s=({
struct Cyc_PP_Doc*_tmp248[4];_tmp248[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1FA);
_tmp248[2]=Cyc_PP_text(({const char*_tmp24A=")";_tag_dynforward(_tmp24A,sizeof(
char),_get_zero_arr_size_char(_tmp24A,2));}));_tmp248[1]=Cyc_Absynpp_typ2doc(
_tmp1F9);_tmp248[0]=Cyc_PP_text(({const char*_tmp249="(";_tag_dynforward(_tmp249,
sizeof(char),_get_zero_arr_size_char(_tmp249,2));}));Cyc_PP_cat(_tag_dynforward(
_tmp248,sizeof(struct Cyc_PP_Doc*),4));});goto _LL1BD;_LL1DE: if(*((int*)_tmp1DE)!= 
16)goto _LL1E0;_tmp1FB=((struct Cyc_Absyn_Address_e_struct*)_tmp1DE)->f1;_LL1DF: s=({
struct Cyc_PP_Doc*_tmp24B[2];_tmp24B[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1FB);
_tmp24B[0]=Cyc_PP_text(({const char*_tmp24C="&";_tag_dynforward(_tmp24C,sizeof(
char),_get_zero_arr_size_char(_tmp24C,2));}));Cyc_PP_cat(_tag_dynforward(_tmp24B,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL1BD;_LL1E0: if(*((int*)_tmp1DE)!= 17)goto
_LL1E2;_tmp1FC=((struct Cyc_Absyn_New_e_struct*)_tmp1DE)->f1;_tmp1FD=((struct Cyc_Absyn_New_e_struct*)
_tmp1DE)->f2;_LL1E1: if(_tmp1FC == 0)s=({struct Cyc_PP_Doc*_tmp24D[2];_tmp24D[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1FD);_tmp24D[0]=Cyc_PP_text(({const char*
_tmp24E="new ";_tag_dynforward(_tmp24E,sizeof(char),_get_zero_arr_size_char(
_tmp24E,5));}));Cyc_PP_cat(_tag_dynforward(_tmp24D,sizeof(struct Cyc_PP_Doc*),2));});
else{s=({struct Cyc_PP_Doc*_tmp24F[4];_tmp24F[3]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp1FD);_tmp24F[2]=Cyc_PP_text(({const char*_tmp251=") ";_tag_dynforward(_tmp251,
sizeof(char),_get_zero_arr_size_char(_tmp251,3));}));_tmp24F[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp1FC);_tmp24F[0]=Cyc_PP_text(({const char*_tmp250="rnew(";
_tag_dynforward(_tmp250,sizeof(char),_get_zero_arr_size_char(_tmp250,6));}));Cyc_PP_cat(
_tag_dynforward(_tmp24F,sizeof(struct Cyc_PP_Doc*),4));});}goto _LL1BD;_LL1E2: if(*((
int*)_tmp1DE)!= 18)goto _LL1E4;_tmp1FE=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp1DE)->f1;_LL1E3: s=({struct Cyc_PP_Doc*_tmp252[3];_tmp252[2]=Cyc_PP_text(({
const char*_tmp254=")";_tag_dynforward(_tmp254,sizeof(char),
_get_zero_arr_size_char(_tmp254,2));}));_tmp252[1]=Cyc_Absynpp_typ2doc(_tmp1FE);
_tmp252[0]=Cyc_PP_text(({const char*_tmp253="sizeof(";_tag_dynforward(_tmp253,
sizeof(char),_get_zero_arr_size_char(_tmp253,8));}));Cyc_PP_cat(_tag_dynforward(
_tmp252,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1BD;_LL1E4: if(*((int*)_tmp1DE)!= 
19)goto _LL1E6;_tmp1FF=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp1DE)->f1;_LL1E5:
s=({struct Cyc_PP_Doc*_tmp255[3];_tmp255[2]=Cyc_PP_text(({const char*_tmp257=")";
_tag_dynforward(_tmp257,sizeof(char),_get_zero_arr_size_char(_tmp257,2));}));
_tmp255[1]=Cyc_Absynpp_exp2doc(_tmp1FF);_tmp255[0]=Cyc_PP_text(({const char*
_tmp256="sizeof(";_tag_dynforward(_tmp256,sizeof(char),_get_zero_arr_size_char(
_tmp256,8));}));Cyc_PP_cat(_tag_dynforward(_tmp255,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1BD;_LL1E6: if(*((int*)_tmp1DE)!= 20)goto _LL1E8;_tmp200=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1DE)->f1;_tmp201=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1DE)->f2;
if(*((int*)_tmp201)!= 0)goto _LL1E8;_tmp202=((struct Cyc_Absyn_StructField_struct*)
_tmp201)->f1;_LL1E7: s=({struct Cyc_PP_Doc*_tmp258[5];_tmp258[4]=Cyc_PP_text(({
const char*_tmp25B=")";_tag_dynforward(_tmp25B,sizeof(char),
_get_zero_arr_size_char(_tmp25B,2));}));_tmp258[3]=Cyc_PP_textptr(_tmp202);
_tmp258[2]=Cyc_PP_text(({const char*_tmp25A=",";_tag_dynforward(_tmp25A,sizeof(
char),_get_zero_arr_size_char(_tmp25A,2));}));_tmp258[1]=Cyc_Absynpp_typ2doc(
_tmp200);_tmp258[0]=Cyc_PP_text(({const char*_tmp259="offsetof(";_tag_dynforward(
_tmp259,sizeof(char),_get_zero_arr_size_char(_tmp259,10));}));Cyc_PP_cat(
_tag_dynforward(_tmp258,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1BD;_LL1E8: if(*((
int*)_tmp1DE)!= 20)goto _LL1EA;_tmp203=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1DE)->f1;_tmp204=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1DE)->f2;
if(*((int*)_tmp204)!= 1)goto _LL1EA;_tmp205=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp204)->f1;_LL1E9: s=({struct Cyc_PP_Doc*_tmp25C[5];_tmp25C[4]=Cyc_PP_text(({
const char*_tmp262=")";_tag_dynforward(_tmp262,sizeof(char),
_get_zero_arr_size_char(_tmp262,2));}));_tmp25C[3]=Cyc_PP_text((struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp261;_tmp261.tag=1;_tmp261.f1=(
unsigned long)((int)_tmp205);{void*_tmp25F[1]={& _tmp261};Cyc_aprintf(({const char*
_tmp260="%d";_tag_dynforward(_tmp260,sizeof(char),_get_zero_arr_size_char(
_tmp260,3));}),_tag_dynforward(_tmp25F,sizeof(void*),1));}}));_tmp25C[2]=Cyc_PP_text(({
const char*_tmp25E=",";_tag_dynforward(_tmp25E,sizeof(char),
_get_zero_arr_size_char(_tmp25E,2));}));_tmp25C[1]=Cyc_Absynpp_typ2doc(_tmp203);
_tmp25C[0]=Cyc_PP_text(({const char*_tmp25D="offsetof(";_tag_dynforward(_tmp25D,
sizeof(char),_get_zero_arr_size_char(_tmp25D,10));}));Cyc_PP_cat(_tag_dynforward(
_tmp25C,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1BD;_LL1EA: if(*((int*)_tmp1DE)!= 
21)goto _LL1EC;_tmp206=((struct Cyc_Absyn_Gentyp_e_struct*)_tmp1DE)->f1;_tmp207=(
void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp1DE)->f2;_LL1EB: s=({struct Cyc_PP_Doc*
_tmp263[4];_tmp263[3]=Cyc_PP_text(({const char*_tmp265=")";_tag_dynforward(
_tmp265,sizeof(char),_get_zero_arr_size_char(_tmp265,2));}));_tmp263[2]=Cyc_Absynpp_typ2doc(
_tmp207);_tmp263[1]=Cyc_Absynpp_tvars2doc(_tmp206);_tmp263[0]=Cyc_PP_text(({
const char*_tmp264="__gen(";_tag_dynforward(_tmp264,sizeof(char),
_get_zero_arr_size_char(_tmp264,7));}));Cyc_PP_cat(_tag_dynforward(_tmp263,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL1BD;_LL1EC: if(*((int*)_tmp1DE)!= 22)goto
_LL1EE;_tmp208=((struct Cyc_Absyn_Deref_e_struct*)_tmp1DE)->f1;_LL1ED: s=({struct
Cyc_PP_Doc*_tmp266[2];_tmp266[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp208);
_tmp266[0]=Cyc_PP_text(({const char*_tmp267="*";_tag_dynforward(_tmp267,sizeof(
char),_get_zero_arr_size_char(_tmp267,2));}));Cyc_PP_cat(_tag_dynforward(_tmp266,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL1BD;_LL1EE: if(*((int*)_tmp1DE)!= 23)goto
_LL1F0;_tmp209=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1DE)->f1;_tmp20A=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp1DE)->f2;_LL1EF: s=({struct Cyc_PP_Doc*
_tmp268[3];_tmp268[2]=Cyc_PP_textptr(_tmp20A);_tmp268[1]=Cyc_PP_text(({const char*
_tmp269=".";_tag_dynforward(_tmp269,sizeof(char),_get_zero_arr_size_char(_tmp269,
2));}));_tmp268[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp209);Cyc_PP_cat(
_tag_dynforward(_tmp268,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1BD;_LL1F0: if(*((
int*)_tmp1DE)!= 24)goto _LL1F2;_tmp20B=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp1DE)->f1;_tmp20C=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1DE)->f2;_LL1F1: s=({
struct Cyc_PP_Doc*_tmp26A[3];_tmp26A[2]=Cyc_PP_textptr(_tmp20C);_tmp26A[1]=Cyc_PP_text(({
const char*_tmp26B="->";_tag_dynforward(_tmp26B,sizeof(char),
_get_zero_arr_size_char(_tmp26B,3));}));_tmp26A[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp20B);Cyc_PP_cat(_tag_dynforward(_tmp26A,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1BD;_LL1F2: if(*((int*)_tmp1DE)!= 25)goto _LL1F4;_tmp20D=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1DE)->f1;_tmp20E=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1DE)->f2;_LL1F3: s=({
struct Cyc_PP_Doc*_tmp26C[4];_tmp26C[3]=Cyc_PP_text(({const char*_tmp26E="]";
_tag_dynforward(_tmp26E,sizeof(char),_get_zero_arr_size_char(_tmp26E,2));}));
_tmp26C[2]=Cyc_Absynpp_exp2doc(_tmp20E);_tmp26C[1]=Cyc_PP_text(({const char*
_tmp26D="[";_tag_dynforward(_tmp26D,sizeof(char),_get_zero_arr_size_char(_tmp26D,
2));}));_tmp26C[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp20D);Cyc_PP_cat(
_tag_dynforward(_tmp26C,sizeof(struct Cyc_PP_Doc*),4));});goto _LL1BD;_LL1F4: if(*((
int*)_tmp1DE)!= 26)goto _LL1F6;_tmp20F=((struct Cyc_Absyn_Tuple_e_struct*)_tmp1DE)->f1;
_LL1F5: s=({struct Cyc_PP_Doc*_tmp26F[4];_tmp26F[3]=Cyc_PP_text(({const char*
_tmp271=")";_tag_dynforward(_tmp271,sizeof(char),_get_zero_arr_size_char(_tmp271,
2));}));_tmp26F[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp20F);_tmp26F[1]=Cyc_PP_text(({
const char*_tmp270="(";_tag_dynforward(_tmp270,sizeof(char),
_get_zero_arr_size_char(_tmp270,2));}));_tmp26F[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(
_tag_dynforward(_tmp26F,sizeof(struct Cyc_PP_Doc*),4));});goto _LL1BD;_LL1F6: if(*((
int*)_tmp1DE)!= 27)goto _LL1F8;_tmp210=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp1DE)->f1;_tmp211=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp1DE)->f2;_LL1F7:
s=({struct Cyc_PP_Doc*_tmp272[4];_tmp272[3]=Cyc_Absynpp_group_braces(({const char*
_tmp275=",";_tag_dynforward(_tmp275,sizeof(char),_get_zero_arr_size_char(_tmp275,
2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp211));_tmp272[2]=Cyc_PP_text(({const char*
_tmp274=")";_tag_dynforward(_tmp274,sizeof(char),_get_zero_arr_size_char(_tmp274,
2));}));_tmp272[1]=Cyc_Absynpp_typ2doc((*_tmp210).f3);_tmp272[0]=Cyc_PP_text(({
const char*_tmp273="(";_tag_dynforward(_tmp273,sizeof(char),
_get_zero_arr_size_char(_tmp273,2));}));Cyc_PP_cat(_tag_dynforward(_tmp272,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL1BD;_LL1F8: if(*((int*)_tmp1DE)!= 28)goto
_LL1FA;_tmp212=((struct Cyc_Absyn_Array_e_struct*)_tmp1DE)->f1;_LL1F9: s=Cyc_Absynpp_group_braces(({
const char*_tmp276=",";_tag_dynforward(_tmp276,sizeof(char),
_get_zero_arr_size_char(_tmp276,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp212));goto _LL1BD;_LL1FA: if(*((int*)_tmp1DE)!= 29)goto _LL1FC;_tmp213=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1DE)->f1;_tmp214=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1DE)->f2;_tmp215=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1DE)->f3;
_LL1FB: s=({struct Cyc_PP_Doc*_tmp277[8];_tmp277[7]=Cyc_Absynpp_rb();_tmp277[6]=
Cyc_Absynpp_exp2doc(_tmp215);_tmp277[5]=Cyc_PP_text(({const char*_tmp27A=" : ";
_tag_dynforward(_tmp27A,sizeof(char),_get_zero_arr_size_char(_tmp27A,4));}));
_tmp277[4]=Cyc_Absynpp_exp2doc(_tmp214);_tmp277[3]=Cyc_PP_text(({const char*
_tmp279=" < ";_tag_dynforward(_tmp279,sizeof(char),_get_zero_arr_size_char(
_tmp279,4));}));_tmp277[2]=Cyc_PP_text(*(*_tmp213->name).f2);_tmp277[1]=Cyc_PP_text(({
const char*_tmp278="for ";_tag_dynforward(_tmp278,sizeof(char),
_get_zero_arr_size_char(_tmp278,5));}));_tmp277[0]=Cyc_Absynpp_lb();Cyc_PP_cat(
_tag_dynforward(_tmp277,sizeof(struct Cyc_PP_Doc*),8));});goto _LL1BD;_LL1FC: if(*((
int*)_tmp1DE)!= 30)goto _LL1FE;_tmp216=((struct Cyc_Absyn_Struct_e_struct*)_tmp1DE)->f1;
_tmp217=((struct Cyc_Absyn_Struct_e_struct*)_tmp1DE)->f2;_tmp218=((struct Cyc_Absyn_Struct_e_struct*)
_tmp1DE)->f3;_LL1FD: {struct Cyc_List_List*_tmp27B=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp218);s=({struct Cyc_PP_Doc*_tmp27C[2];_tmp27C[1]=Cyc_Absynpp_group_braces(({
const char*_tmp27D=",";_tag_dynforward(_tmp27D,sizeof(char),
_get_zero_arr_size_char(_tmp27D,2));}),_tmp217 != 0?({struct Cyc_List_List*_tmp27E=
_cycalloc(sizeof(*_tmp27E));_tmp27E->hd=Cyc_Absynpp_tps2doc(_tmp217);_tmp27E->tl=
_tmp27B;_tmp27E;}): _tmp27B);_tmp27C[0]=Cyc_Absynpp_qvar2doc(_tmp216);Cyc_PP_cat(
_tag_dynforward(_tmp27C,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1BD;}_LL1FE: if(*((
int*)_tmp1DE)!= 31)goto _LL200;_tmp219=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1DE)->f2;_LL1FF: s=Cyc_Absynpp_group_braces(({const char*_tmp27F=",";
_tag_dynforward(_tmp27F,sizeof(char),_get_zero_arr_size_char(_tmp27F,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp219));goto _LL1BD;_LL200: if(*((int*)
_tmp1DE)!= 32)goto _LL202;_tmp21A=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1DE)->f1;
_tmp21B=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1DE)->f3;_LL201: if(_tmp21A == 0)s=
Cyc_Absynpp_qvar2doc(_tmp21B->name);else{s=({struct Cyc_PP_Doc*_tmp280[2];_tmp280[
1]=Cyc_PP_egroup(({const char*_tmp281="(";_tag_dynforward(_tmp281,sizeof(char),
_get_zero_arr_size_char(_tmp281,2));}),({const char*_tmp282=")";_tag_dynforward(
_tmp282,sizeof(char),_get_zero_arr_size_char(_tmp282,2));}),({const char*_tmp283=",";
_tag_dynforward(_tmp283,sizeof(char),_get_zero_arr_size_char(_tmp283,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp21A));_tmp280[0]=Cyc_Absynpp_qvar2doc(
_tmp21B->name);Cyc_PP_cat(_tag_dynforward(_tmp280,sizeof(struct Cyc_PP_Doc*),2));});}
goto _LL1BD;_LL202: if(*((int*)_tmp1DE)!= 33)goto _LL204;_tmp21C=((struct Cyc_Absyn_Enum_e_struct*)
_tmp1DE)->f1;_LL203: s=Cyc_Absynpp_qvar2doc(_tmp21C);goto _LL1BD;_LL204: if(*((int*)
_tmp1DE)!= 34)goto _LL206;_tmp21D=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp1DE)->f1;
_LL205: s=Cyc_Absynpp_qvar2doc(_tmp21D);goto _LL1BD;_LL206: if(*((int*)_tmp1DE)!= 
35)goto _LL208;_tmp21E=((struct Cyc_Absyn_Malloc_e_struct*)_tmp1DE)->f1;_tmp21F=
_tmp21E.is_calloc;_tmp220=_tmp21E.rgn;_tmp221=_tmp21E.elt_type;_tmp222=_tmp21E.num_elts;
_LL207: if(_tmp21F){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)
_check_null(_tmp221)),0);if(_tmp220 == 0)s=({struct Cyc_PP_Doc*_tmp284[5];_tmp284[
4]=Cyc_PP_text(({const char*_tmp287=")";_tag_dynforward(_tmp287,sizeof(char),
_get_zero_arr_size_char(_tmp287,2));}));_tmp284[3]=Cyc_Absynpp_exp2doc(st);
_tmp284[2]=Cyc_PP_text(({const char*_tmp286=",";_tag_dynforward(_tmp286,sizeof(
char),_get_zero_arr_size_char(_tmp286,2));}));_tmp284[1]=Cyc_Absynpp_exp2doc(
_tmp222);_tmp284[0]=Cyc_PP_text(({const char*_tmp285="calloc(";_tag_dynforward(
_tmp285,sizeof(char),_get_zero_arr_size_char(_tmp285,8));}));Cyc_PP_cat(
_tag_dynforward(_tmp284,sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*
_tmp288[7];_tmp288[6]=Cyc_PP_text(({const char*_tmp28C=")";_tag_dynforward(
_tmp28C,sizeof(char),_get_zero_arr_size_char(_tmp28C,2));}));_tmp288[5]=Cyc_Absynpp_exp2doc(
st);_tmp288[4]=Cyc_PP_text(({const char*_tmp28B=",";_tag_dynforward(_tmp28B,
sizeof(char),_get_zero_arr_size_char(_tmp28B,2));}));_tmp288[3]=Cyc_Absynpp_exp2doc(
_tmp222);_tmp288[2]=Cyc_PP_text(({const char*_tmp28A=",";_tag_dynforward(_tmp28A,
sizeof(char),_get_zero_arr_size_char(_tmp28A,2));}));_tmp288[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp220);_tmp288[0]=Cyc_PP_text(({const char*_tmp289="rcalloc(";
_tag_dynforward(_tmp289,sizeof(char),_get_zero_arr_size_char(_tmp289,9));}));Cyc_PP_cat(
_tag_dynforward(_tmp288,sizeof(struct Cyc_PP_Doc*),7));});}}else{struct Cyc_Absyn_Exp*
new_e;if(_tmp221 == 0)new_e=_tmp222;else{new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*
_tmp221,0),_tmp222,0);}if(_tmp220 == 0)s=({struct Cyc_PP_Doc*_tmp28D[3];_tmp28D[2]=
Cyc_PP_text(({const char*_tmp28F=")";_tag_dynforward(_tmp28F,sizeof(char),
_get_zero_arr_size_char(_tmp28F,2));}));_tmp28D[1]=Cyc_Absynpp_exp2doc(new_e);
_tmp28D[0]=Cyc_PP_text(({const char*_tmp28E="malloc(";_tag_dynforward(_tmp28E,
sizeof(char),_get_zero_arr_size_char(_tmp28E,8));}));Cyc_PP_cat(_tag_dynforward(
_tmp28D,sizeof(struct Cyc_PP_Doc*),3));});else{s=({struct Cyc_PP_Doc*_tmp290[5];
_tmp290[4]=Cyc_PP_text(({const char*_tmp293=")";_tag_dynforward(_tmp293,sizeof(
char),_get_zero_arr_size_char(_tmp293,2));}));_tmp290[3]=Cyc_Absynpp_exp2doc(
new_e);_tmp290[2]=Cyc_PP_text(({const char*_tmp292=",";_tag_dynforward(_tmp292,
sizeof(char),_get_zero_arr_size_char(_tmp292,2));}));_tmp290[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp220);_tmp290[0]=Cyc_PP_text(({const char*_tmp291="rmalloc(";
_tag_dynforward(_tmp291,sizeof(char),_get_zero_arr_size_char(_tmp291,9));}));Cyc_PP_cat(
_tag_dynforward(_tmp290,sizeof(struct Cyc_PP_Doc*),5));});}}goto _LL1BD;_LL208: if(*((
int*)_tmp1DE)!= 36)goto _LL20A;_tmp223=((struct Cyc_Absyn_Swap_e_struct*)_tmp1DE)->f1;
_tmp224=((struct Cyc_Absyn_Swap_e_struct*)_tmp1DE)->f2;_LL209: s=({struct Cyc_PP_Doc*
_tmp294[5];_tmp294[4]=Cyc_PP_text(({const char*_tmp297=")";_tag_dynforward(
_tmp297,sizeof(char),_get_zero_arr_size_char(_tmp297,2));}));_tmp294[3]=Cyc_Absynpp_exp2doc(
_tmp224);_tmp294[2]=Cyc_PP_text(({const char*_tmp296=",";_tag_dynforward(_tmp296,
sizeof(char),_get_zero_arr_size_char(_tmp296,2));}));_tmp294[1]=Cyc_Absynpp_exp2doc(
_tmp223);_tmp294[0]=Cyc_PP_text(({const char*_tmp295="cycswap(";_tag_dynforward(
_tmp295,sizeof(char),_get_zero_arr_size_char(_tmp295,9));}));Cyc_PP_cat(
_tag_dynforward(_tmp294,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1BD;_LL20A: if(*((
int*)_tmp1DE)!= 37)goto _LL20C;_tmp225=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp1DE)->f1;_tmp226=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1DE)->f2;
_LL20B: s=Cyc_Absynpp_group_braces(({const char*_tmp298=",";_tag_dynforward(
_tmp298,sizeof(char),_get_zero_arr_size_char(_tmp298,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp226));goto _LL1BD;_LL20C: if(*((int*)_tmp1DE)!= 38)goto _LL1BD;_tmp227=((struct
Cyc_Absyn_StmtExp_e_struct*)_tmp1DE)->f1;_LL20D: s=({struct Cyc_PP_Doc*_tmp299[7];
_tmp299[6]=Cyc_PP_text(({const char*_tmp29B=")";_tag_dynforward(_tmp29B,sizeof(
char),_get_zero_arr_size_char(_tmp29B,2));}));_tmp299[5]=Cyc_Absynpp_rb();
_tmp299[4]=Cyc_PP_blank_doc();_tmp299[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp227));_tmp299[2]=Cyc_PP_blank_doc();_tmp299[1]=Cyc_Absynpp_lb();_tmp299[0]=
Cyc_PP_text(({const char*_tmp29A="(";_tag_dynforward(_tmp29A,sizeof(char),
_get_zero_arr_size_char(_tmp29A,2));}));Cyc_PP_cat(_tag_dynforward(_tmp299,
sizeof(struct Cyc_PP_Doc*),7));});goto _LL1BD;_LL1BD:;}if(inprec >= myprec)s=({
struct Cyc_PP_Doc*_tmp29C[3];_tmp29C[2]=Cyc_PP_text(({const char*_tmp29E=")";
_tag_dynforward(_tmp29E,sizeof(char),_get_zero_arr_size_char(_tmp29E,2));}));
_tmp29C[1]=s;_tmp29C[0]=Cyc_PP_text(({const char*_tmp29D="(";_tag_dynforward(
_tmp29D,sizeof(char),_get_zero_arr_size_char(_tmp29D,2));}));Cyc_PP_cat(
_tag_dynforward(_tmp29C,sizeof(struct Cyc_PP_Doc*),3));});return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_designator2doc(void*d){void*_tmp29F=d;struct Cyc_Absyn_Exp*_tmp2A0;
struct _dynforward_ptr*_tmp2A1;_LL218: if(*((int*)_tmp29F)!= 0)goto _LL21A;_tmp2A0=((
struct Cyc_Absyn_ArrayElement_struct*)_tmp29F)->f1;_LL219: return({struct Cyc_PP_Doc*
_tmp2A2[3];_tmp2A2[2]=Cyc_PP_text(({const char*_tmp2A4="]";_tag_dynforward(
_tmp2A4,sizeof(char),_get_zero_arr_size_char(_tmp2A4,2));}));_tmp2A2[1]=Cyc_Absynpp_exp2doc(
_tmp2A0);_tmp2A2[0]=Cyc_PP_text(({const char*_tmp2A3=".[";_tag_dynforward(_tmp2A3,
sizeof(char),_get_zero_arr_size_char(_tmp2A3,3));}));Cyc_PP_cat(_tag_dynforward(
_tmp2A2,sizeof(struct Cyc_PP_Doc*),3));});_LL21A: if(*((int*)_tmp29F)!= 1)goto
_LL217;_tmp2A1=((struct Cyc_Absyn_FieldName_struct*)_tmp29F)->f1;_LL21B: return({
struct Cyc_PP_Doc*_tmp2A5[2];_tmp2A5[1]=Cyc_PP_textptr(_tmp2A1);_tmp2A5[0]=Cyc_PP_text(({
const char*_tmp2A6=".";_tag_dynforward(_tmp2A6,sizeof(char),
_get_zero_arr_size_char(_tmp2A6,2));}));Cyc_PP_cat(_tag_dynforward(_tmp2A5,
sizeof(struct Cyc_PP_Doc*),2));});_LL217:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(
struct _tuple9*de){if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
return({struct Cyc_PP_Doc*_tmp2A7[2];_tmp2A7[1]=Cyc_Absynpp_exp2doc((*de).f2);
_tmp2A7[0]=Cyc_PP_egroup(({const char*_tmp2A8="";_tag_dynforward(_tmp2A8,sizeof(
char),_get_zero_arr_size_char(_tmp2A8,1));}),({const char*_tmp2A9="=";
_tag_dynforward(_tmp2A9,sizeof(char),_get_zero_arr_size_char(_tmp2A9,2));}),({
const char*_tmp2AA="=";_tag_dynforward(_tmp2AA,sizeof(char),
_get_zero_arr_size_char(_tmp2AA,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));
Cyc_PP_cat(_tag_dynforward(_tmp2A7,sizeof(struct Cyc_PP_Doc*),2));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){return Cyc_PP_group(({
const char*_tmp2AB="";_tag_dynforward(_tmp2AB,sizeof(char),
_get_zero_arr_size_char(_tmp2AB,1));}),({const char*_tmp2AC="";_tag_dynforward(
_tmp2AC,sizeof(char),_get_zero_arr_size_char(_tmp2AC,1));}),({const char*_tmp2AD=",";
_tag_dynforward(_tmp2AD,sizeof(char),_get_zero_arr_size_char(_tmp2AD,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst_union c){union Cyc_Absyn_Cnst_union
_tmp2AE=c;void*_tmp2AF;char _tmp2B0;void*_tmp2B1;short _tmp2B2;void*_tmp2B3;int
_tmp2B4;void*_tmp2B5;int _tmp2B6;void*_tmp2B7;int _tmp2B8;void*_tmp2B9;long long
_tmp2BA;struct _dynforward_ptr _tmp2BB;struct _dynforward_ptr _tmp2BC;_LL21D: if((
_tmp2AE.Char_c).tag != 0)goto _LL21F;_tmp2AF=(_tmp2AE.Char_c).f1;_tmp2B0=(_tmp2AE.Char_c).f2;
_LL21E: return Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp2BF;_tmp2BF.tag=0;_tmp2BF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_char_escape(_tmp2B0));{void*_tmp2BD[1]={& _tmp2BF};Cyc_aprintf(({
const char*_tmp2BE="'%s'";_tag_dynforward(_tmp2BE,sizeof(char),
_get_zero_arr_size_char(_tmp2BE,5));}),_tag_dynforward(_tmp2BD,sizeof(void*),1));}}));
_LL21F: if((_tmp2AE.Short_c).tag != 1)goto _LL221;_tmp2B1=(_tmp2AE.Short_c).f1;
_tmp2B2=(_tmp2AE.Short_c).f2;_LL220: return Cyc_PP_text((struct _dynforward_ptr)({
struct Cyc_Int_pa_struct _tmp2C2;_tmp2C2.tag=1;_tmp2C2.f1=(unsigned long)((int)
_tmp2B2);{void*_tmp2C0[1]={& _tmp2C2};Cyc_aprintf(({const char*_tmp2C1="%d";
_tag_dynforward(_tmp2C1,sizeof(char),_get_zero_arr_size_char(_tmp2C1,3));}),
_tag_dynforward(_tmp2C0,sizeof(void*),1));}}));_LL221: if((_tmp2AE.Int_c).tag != 2)
goto _LL223;_tmp2B3=(_tmp2AE.Int_c).f1;if((int)_tmp2B3 != 2)goto _LL223;_tmp2B4=(
_tmp2AE.Int_c).f2;_LL222: _tmp2B6=_tmp2B4;goto _LL224;_LL223: if((_tmp2AE.Int_c).tag
!= 2)goto _LL225;_tmp2B5=(_tmp2AE.Int_c).f1;if((int)_tmp2B5 != 0)goto _LL225;
_tmp2B6=(_tmp2AE.Int_c).f2;_LL224: return Cyc_PP_text((struct _dynforward_ptr)({
struct Cyc_Int_pa_struct _tmp2C5;_tmp2C5.tag=1;_tmp2C5.f1=(unsigned long)_tmp2B6;{
void*_tmp2C3[1]={& _tmp2C5};Cyc_aprintf(({const char*_tmp2C4="%d";_tag_dynforward(
_tmp2C4,sizeof(char),_get_zero_arr_size_char(_tmp2C4,3));}),_tag_dynforward(
_tmp2C3,sizeof(void*),1));}}));_LL225: if((_tmp2AE.Int_c).tag != 2)goto _LL227;
_tmp2B7=(_tmp2AE.Int_c).f1;if((int)_tmp2B7 != 1)goto _LL227;_tmp2B8=(_tmp2AE.Int_c).f2;
_LL226: return Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp2C8;
_tmp2C8.tag=1;_tmp2C8.f1=(unsigned int)_tmp2B8;{void*_tmp2C6[1]={& _tmp2C8};Cyc_aprintf(({
const char*_tmp2C7="%u";_tag_dynforward(_tmp2C7,sizeof(char),
_get_zero_arr_size_char(_tmp2C7,3));}),_tag_dynforward(_tmp2C6,sizeof(void*),1));}}));
_LL227: if((_tmp2AE.LongLong_c).tag != 3)goto _LL229;_tmp2B9=(_tmp2AE.LongLong_c).f1;
_tmp2BA=(_tmp2AE.LongLong_c).f2;_LL228: return Cyc_PP_text(({const char*_tmp2C9="<<FIX LONG LONG CONSTANT>>";
_tag_dynforward(_tmp2C9,sizeof(char),_get_zero_arr_size_char(_tmp2C9,27));}));
_LL229: if((_tmp2AE.Float_c).tag != 4)goto _LL22B;_tmp2BB=(_tmp2AE.Float_c).f1;
_LL22A: return Cyc_PP_text(_tmp2BB);_LL22B: if((_tmp2AE.Null_c).tag != 6)goto _LL22D;
_LL22C: return Cyc_PP_text(({const char*_tmp2CA="NULL";_tag_dynforward(_tmp2CA,
sizeof(char),_get_zero_arr_size_char(_tmp2CA,5));}));_LL22D: if((_tmp2AE.String_c).tag
!= 5)goto _LL21C;_tmp2BC=(_tmp2AE.String_c).f1;_LL22E: return({struct Cyc_PP_Doc*
_tmp2CB[3];_tmp2CB[2]=Cyc_PP_text(({const char*_tmp2CD="\"";_tag_dynforward(
_tmp2CD,sizeof(char),_get_zero_arr_size_char(_tmp2CD,2));}));_tmp2CB[1]=Cyc_PP_text(
Cyc_Absynpp_string_escape(_tmp2BC));_tmp2CB[0]=Cyc_PP_text(({const char*_tmp2CC="\"";
_tag_dynforward(_tmp2CC,sizeof(char),_get_zero_arr_size_char(_tmp2CC,2));}));Cyc_PP_cat(
_tag_dynforward(_tmp2CB,sizeof(struct Cyc_PP_Doc*),3));});_LL21C:;}struct Cyc_PP_Doc*
Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*es){struct Cyc_PP_Doc*
ps=Cyc_Absynpp_prim2doc(p);if(p == (void*)((void*)19)){if(es == 0  || es->tl != 0)(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2CE=_cycalloc(sizeof(*
_tmp2CE));_tmp2CE[0]=({struct Cyc_Core_Failure_struct _tmp2CF;_tmp2CF.tag=Cyc_Core_Failure;
_tmp2CF.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp2D2;_tmp2D2.tag=
0;_tmp2D2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_PP_string_of_doc(
ps,72));{void*_tmp2D0[1]={& _tmp2D2};Cyc_aprintf(({const char*_tmp2D1="Absynpp::primapp2doc Size: %s with bad args";
_tag_dynforward(_tmp2D1,sizeof(char),_get_zero_arr_size_char(_tmp2D1,44));}),
_tag_dynforward(_tmp2D0,sizeof(void*),1));}});_tmp2CF;});_tmp2CE;}));{struct Cyc_PP_Doc*
_tmp2D3=Cyc_Absynpp_exp2doc_prec(inprec,(struct Cyc_Absyn_Exp*)es->hd);return({
struct Cyc_PP_Doc*_tmp2D4[2];_tmp2D4[1]=Cyc_PP_text(({const char*_tmp2D5=".size";
_tag_dynforward(_tmp2D5,sizeof(char),_get_zero_arr_size_char(_tmp2D5,6));}));
_tmp2D4[0]=_tmp2D3;Cyc_PP_cat(_tag_dynforward(_tmp2D4,sizeof(struct Cyc_PP_Doc*),
2));});}}else{struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es);if(ds == 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2D6=
_cycalloc(sizeof(*_tmp2D6));_tmp2D6[0]=({struct Cyc_Core_Failure_struct _tmp2D7;
_tmp2D7.tag=Cyc_Core_Failure;_tmp2D7.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp2DA;_tmp2DA.tag=0;_tmp2DA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_PP_string_of_doc(ps,72));{void*_tmp2D8[1]={& _tmp2DA};Cyc_aprintf(({const char*
_tmp2D9="Absynpp::primapp2doc: %s with no args";_tag_dynforward(_tmp2D9,sizeof(
char),_get_zero_arr_size_char(_tmp2D9,38));}),_tag_dynforward(_tmp2D8,sizeof(
void*),1));}});_tmp2D7;});_tmp2D6;}));else{if(ds->tl == 0)return({struct Cyc_PP_Doc*
_tmp2DB[3];_tmp2DB[2]=(struct Cyc_PP_Doc*)ds->hd;_tmp2DB[1]=Cyc_PP_text(({const
char*_tmp2DC=" ";_tag_dynforward(_tmp2DC,sizeof(char),_get_zero_arr_size_char(
_tmp2DC,2));}));_tmp2DB[0]=ps;Cyc_PP_cat(_tag_dynforward(_tmp2DB,sizeof(struct
Cyc_PP_Doc*),3));});else{if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2DD=_cycalloc(sizeof(*
_tmp2DD));_tmp2DD[0]=({struct Cyc_Core_Failure_struct _tmp2DE;_tmp2DE.tag=Cyc_Core_Failure;
_tmp2DE.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp2E1;_tmp2E1.tag=
0;_tmp2E1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_PP_string_of_doc(
ps,72));{void*_tmp2DF[1]={& _tmp2E1};Cyc_aprintf(({const char*_tmp2E0="Absynpp::primapp2doc: %s with more than 2 args";
_tag_dynforward(_tmp2E0,sizeof(char),_get_zero_arr_size_char(_tmp2E0,47));}),
_tag_dynforward(_tmp2DF,sizeof(void*),1));}});_tmp2DE;});_tmp2DD;}));else{return({
struct Cyc_PP_Doc*_tmp2E2[5];_tmp2E2[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)
_check_null(ds->tl))->hd;_tmp2E2[3]=Cyc_PP_text(({const char*_tmp2E4=" ";
_tag_dynforward(_tmp2E4,sizeof(char),_get_zero_arr_size_char(_tmp2E4,2));}));
_tmp2E2[2]=ps;_tmp2E2[1]=Cyc_PP_text(({const char*_tmp2E3=" ";_tag_dynforward(
_tmp2E3,sizeof(char),_get_zero_arr_size_char(_tmp2E3,2));}));_tmp2E2[0]=(struct
Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_dynforward(_tmp2E2,sizeof(struct Cyc_PP_Doc*),
5));});}}}}}struct _dynforward_ptr Cyc_Absynpp_prim2str(void*p){void*_tmp2E5=p;
_LL230: if((int)_tmp2E5 != 0)goto _LL232;_LL231: return({const char*_tmp2E6="+";
_tag_dynforward(_tmp2E6,sizeof(char),_get_zero_arr_size_char(_tmp2E6,2));});
_LL232: if((int)_tmp2E5 != 1)goto _LL234;_LL233: return({const char*_tmp2E7="*";
_tag_dynforward(_tmp2E7,sizeof(char),_get_zero_arr_size_char(_tmp2E7,2));});
_LL234: if((int)_tmp2E5 != 2)goto _LL236;_LL235: return({const char*_tmp2E8="-";
_tag_dynforward(_tmp2E8,sizeof(char),_get_zero_arr_size_char(_tmp2E8,2));});
_LL236: if((int)_tmp2E5 != 3)goto _LL238;_LL237: return({const char*_tmp2E9="/";
_tag_dynforward(_tmp2E9,sizeof(char),_get_zero_arr_size_char(_tmp2E9,2));});
_LL238: if((int)_tmp2E5 != 4)goto _LL23A;_LL239: return({const char*_tmp2EA="%";
_tag_dynforward(_tmp2EA,sizeof(char),_get_zero_arr_size_char(_tmp2EA,2));});
_LL23A: if((int)_tmp2E5 != 5)goto _LL23C;_LL23B: return({const char*_tmp2EB="==";
_tag_dynforward(_tmp2EB,sizeof(char),_get_zero_arr_size_char(_tmp2EB,3));});
_LL23C: if((int)_tmp2E5 != 6)goto _LL23E;_LL23D: return({const char*_tmp2EC="!=";
_tag_dynforward(_tmp2EC,sizeof(char),_get_zero_arr_size_char(_tmp2EC,3));});
_LL23E: if((int)_tmp2E5 != 7)goto _LL240;_LL23F: return({const char*_tmp2ED=">";
_tag_dynforward(_tmp2ED,sizeof(char),_get_zero_arr_size_char(_tmp2ED,2));});
_LL240: if((int)_tmp2E5 != 8)goto _LL242;_LL241: return({const char*_tmp2EE="<";
_tag_dynforward(_tmp2EE,sizeof(char),_get_zero_arr_size_char(_tmp2EE,2));});
_LL242: if((int)_tmp2E5 != 9)goto _LL244;_LL243: return({const char*_tmp2EF=">=";
_tag_dynforward(_tmp2EF,sizeof(char),_get_zero_arr_size_char(_tmp2EF,3));});
_LL244: if((int)_tmp2E5 != 10)goto _LL246;_LL245: return({const char*_tmp2F0="<=";
_tag_dynforward(_tmp2F0,sizeof(char),_get_zero_arr_size_char(_tmp2F0,3));});
_LL246: if((int)_tmp2E5 != 11)goto _LL248;_LL247: return({const char*_tmp2F1="!";
_tag_dynforward(_tmp2F1,sizeof(char),_get_zero_arr_size_char(_tmp2F1,2));});
_LL248: if((int)_tmp2E5 != 12)goto _LL24A;_LL249: return({const char*_tmp2F2="~";
_tag_dynforward(_tmp2F2,sizeof(char),_get_zero_arr_size_char(_tmp2F2,2));});
_LL24A: if((int)_tmp2E5 != 13)goto _LL24C;_LL24B: return({const char*_tmp2F3="&";
_tag_dynforward(_tmp2F3,sizeof(char),_get_zero_arr_size_char(_tmp2F3,2));});
_LL24C: if((int)_tmp2E5 != 14)goto _LL24E;_LL24D: return({const char*_tmp2F4="|";
_tag_dynforward(_tmp2F4,sizeof(char),_get_zero_arr_size_char(_tmp2F4,2));});
_LL24E: if((int)_tmp2E5 != 15)goto _LL250;_LL24F: return({const char*_tmp2F5="^";
_tag_dynforward(_tmp2F5,sizeof(char),_get_zero_arr_size_char(_tmp2F5,2));});
_LL250: if((int)_tmp2E5 != 16)goto _LL252;_LL251: return({const char*_tmp2F6="<<";
_tag_dynforward(_tmp2F6,sizeof(char),_get_zero_arr_size_char(_tmp2F6,3));});
_LL252: if((int)_tmp2E5 != 17)goto _LL254;_LL253: return({const char*_tmp2F7=">>";
_tag_dynforward(_tmp2F7,sizeof(char),_get_zero_arr_size_char(_tmp2F7,3));});
_LL254: if((int)_tmp2E5 != 18)goto _LL256;_LL255: return({const char*_tmp2F8=">>>";
_tag_dynforward(_tmp2F8,sizeof(char),_get_zero_arr_size_char(_tmp2F8,4));});
_LL256: if((int)_tmp2E5 != 19)goto _LL22F;_LL257: return({const char*_tmp2F9="size";
_tag_dynforward(_tmp2F9,sizeof(char),_get_zero_arr_size_char(_tmp2F9,5));});
_LL22F:;}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(void*p){return Cyc_PP_text(Cyc_Absynpp_prim2str(
p));}int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){void*_tmp2FA=(void*)s->r;
_LL259: if(_tmp2FA <= (void*)1)goto _LL25B;if(*((int*)_tmp2FA)!= 11)goto _LL25B;
_LL25A: return 1;_LL25B:;_LL25C: return 0;_LL258:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*st){struct Cyc_PP_Doc*s;{void*_tmp2FB=(void*)st->r;struct Cyc_Absyn_Exp*
_tmp2FC;struct Cyc_Absyn_Stmt*_tmp2FD;struct Cyc_Absyn_Stmt*_tmp2FE;struct Cyc_Absyn_Exp*
_tmp2FF;struct Cyc_Absyn_Exp*_tmp300;struct Cyc_Absyn_Stmt*_tmp301;struct Cyc_Absyn_Stmt*
_tmp302;struct _tuple2 _tmp303;struct Cyc_Absyn_Exp*_tmp304;struct Cyc_Absyn_Stmt*
_tmp305;struct _dynforward_ptr*_tmp306;struct Cyc_Absyn_Exp*_tmp307;struct _tuple2
_tmp308;struct Cyc_Absyn_Exp*_tmp309;struct _tuple2 _tmp30A;struct Cyc_Absyn_Exp*
_tmp30B;struct Cyc_Absyn_Stmt*_tmp30C;struct Cyc_Absyn_Exp*_tmp30D;struct Cyc_List_List*
_tmp30E;struct Cyc_List_List*_tmp30F;struct Cyc_List_List*_tmp310;struct Cyc_Absyn_Decl*
_tmp311;struct Cyc_Absyn_Stmt*_tmp312;struct _dynforward_ptr*_tmp313;struct Cyc_Absyn_Stmt*
_tmp314;struct Cyc_Absyn_Stmt*_tmp315;struct _tuple2 _tmp316;struct Cyc_Absyn_Exp*
_tmp317;struct Cyc_Absyn_Stmt*_tmp318;struct Cyc_List_List*_tmp319;struct Cyc_Absyn_Tvar*
_tmp31A;struct Cyc_Absyn_Vardecl*_tmp31B;int _tmp31C;struct Cyc_Absyn_Exp*_tmp31D;
struct Cyc_Absyn_Stmt*_tmp31E;struct Cyc_Absyn_Exp*_tmp31F;struct Cyc_Absyn_Exp*
_tmp320;struct Cyc_Absyn_Tvar*_tmp321;struct Cyc_Absyn_Vardecl*_tmp322;struct Cyc_Absyn_Stmt*
_tmp323;_LL25E: if((int)_tmp2FB != 0)goto _LL260;_LL25F: s=Cyc_PP_text(({const char*
_tmp324=";";_tag_dynforward(_tmp324,sizeof(char),_get_zero_arr_size_char(_tmp324,
2));}));goto _LL25D;_LL260: if(_tmp2FB <= (void*)1)goto _LL262;if(*((int*)_tmp2FB)!= 
0)goto _LL262;_tmp2FC=((struct Cyc_Absyn_Exp_s_struct*)_tmp2FB)->f1;_LL261: s=({
struct Cyc_PP_Doc*_tmp325[2];_tmp325[1]=Cyc_PP_text(({const char*_tmp326=";";
_tag_dynforward(_tmp326,sizeof(char),_get_zero_arr_size_char(_tmp326,2));}));
_tmp325[0]=Cyc_Absynpp_exp2doc(_tmp2FC);Cyc_PP_cat(_tag_dynforward(_tmp325,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL25D;_LL262: if(_tmp2FB <= (void*)1)goto
_LL264;if(*((int*)_tmp2FB)!= 1)goto _LL264;_tmp2FD=((struct Cyc_Absyn_Seq_s_struct*)
_tmp2FB)->f1;_tmp2FE=((struct Cyc_Absyn_Seq_s_struct*)_tmp2FB)->f2;_LL263: if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp2FD))s=({struct Cyc_PP_Doc*_tmp327[7];_tmp327[6]=
Cyc_Absynpp_is_declaration(_tmp2FE)?({struct Cyc_PP_Doc*_tmp328[5];_tmp328[4]=Cyc_PP_line_doc();
_tmp328[3]=Cyc_Absynpp_rb();_tmp328[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp2FE));_tmp328[1]=Cyc_PP_blank_doc();_tmp328[0]=Cyc_Absynpp_lb();Cyc_PP_cat(
_tag_dynforward(_tmp328,sizeof(struct Cyc_PP_Doc*),5));}): Cyc_Absynpp_stmt2doc(
_tmp2FE);_tmp327[5]=Cyc_PP_line_doc();_tmp327[4]=Cyc_Absynpp_rb();_tmp327[3]=Cyc_PP_line_doc();
_tmp327[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2FD));_tmp327[1]=Cyc_PP_blank_doc();
_tmp327[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dynforward(_tmp327,sizeof(struct Cyc_PP_Doc*),
7));});else{if(Cyc_Absynpp_is_declaration(_tmp2FE))s=({struct Cyc_PP_Doc*_tmp329[
7];_tmp329[6]=Cyc_PP_line_doc();_tmp329[5]=Cyc_Absynpp_rb();_tmp329[4]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp2FE));_tmp329[3]=Cyc_PP_blank_doc();_tmp329[2]=Cyc_Absynpp_lb();
_tmp329[1]=Cyc_PP_line_doc();_tmp329[0]=Cyc_Absynpp_stmt2doc(_tmp2FD);Cyc_PP_cat(
_tag_dynforward(_tmp329,sizeof(struct Cyc_PP_Doc*),7));});else{s=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),struct _dynforward_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,({const char*_tmp32A="";_tag_dynforward(
_tmp32A,sizeof(char),_get_zero_arr_size_char(_tmp32A,1));}),({struct Cyc_List_List*
_tmp32B=_cycalloc(sizeof(*_tmp32B));_tmp32B->hd=_tmp2FD;_tmp32B->tl=({struct Cyc_List_List*
_tmp32C=_cycalloc(sizeof(*_tmp32C));_tmp32C->hd=_tmp2FE;_tmp32C->tl=0;_tmp32C;});
_tmp32B;}));}}}else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),
struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,({
const char*_tmp32D="";_tag_dynforward(_tmp32D,sizeof(char),
_get_zero_arr_size_char(_tmp32D,1));}),({struct Cyc_List_List*_tmp32E=_cycalloc(
sizeof(*_tmp32E));_tmp32E->hd=_tmp2FD;_tmp32E->tl=({struct Cyc_List_List*_tmp32F=
_cycalloc(sizeof(*_tmp32F));_tmp32F->hd=_tmp2FE;_tmp32F->tl=0;_tmp32F;});_tmp32E;}));}
goto _LL25D;_LL264: if(_tmp2FB <= (void*)1)goto _LL266;if(*((int*)_tmp2FB)!= 2)goto
_LL266;_tmp2FF=((struct Cyc_Absyn_Return_s_struct*)_tmp2FB)->f1;_LL265: if(_tmp2FF
== 0)s=Cyc_PP_text(({const char*_tmp330="return;";_tag_dynforward(_tmp330,sizeof(
char),_get_zero_arr_size_char(_tmp330,8));}));else{s=({struct Cyc_PP_Doc*_tmp331[
3];_tmp331[2]=Cyc_PP_text(({const char*_tmp333=";";_tag_dynforward(_tmp333,
sizeof(char),_get_zero_arr_size_char(_tmp333,2));}));_tmp331[1]=_tmp2FF == 0?Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2FF);_tmp331[0]=Cyc_PP_text(({
const char*_tmp332="return ";_tag_dynforward(_tmp332,sizeof(char),
_get_zero_arr_size_char(_tmp332,8));}));Cyc_PP_cat(_tag_dynforward(_tmp331,
sizeof(struct Cyc_PP_Doc*),3));});}goto _LL25D;_LL266: if(_tmp2FB <= (void*)1)goto
_LL268;if(*((int*)_tmp2FB)!= 3)goto _LL268;_tmp300=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp2FB)->f1;_tmp301=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2FB)->f2;_tmp302=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2FB)->f3;_LL267: {int print_else;{void*
_tmp334=(void*)_tmp302->r;_LL287: if((int)_tmp334 != 0)goto _LL289;_LL288:
print_else=0;goto _LL286;_LL289:;_LL28A: print_else=1;goto _LL286;_LL286:;}s=({
struct Cyc_PP_Doc*_tmp335[8];_tmp335[7]=print_else?({struct Cyc_PP_Doc*_tmp339[6];
_tmp339[5]=Cyc_Absynpp_rb();_tmp339[4]=Cyc_PP_line_doc();_tmp339[3]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp33B[2];_tmp33B[1]=Cyc_Absynpp_stmt2doc(_tmp302);_tmp33B[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp33B,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp339[2]=Cyc_Absynpp_lb();_tmp339[1]=Cyc_PP_text(({const char*_tmp33A="else ";
_tag_dynforward(_tmp33A,sizeof(char),_get_zero_arr_size_char(_tmp33A,6));}));
_tmp339[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp339,sizeof(struct Cyc_PP_Doc*),
6));}): Cyc_PP_nil_doc();_tmp335[6]=Cyc_Absynpp_rb();_tmp335[5]=Cyc_PP_line_doc();
_tmp335[4]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp338[2];_tmp338[1]=Cyc_Absynpp_stmt2doc(
_tmp301);_tmp338[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp338,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp335[3]=Cyc_Absynpp_lb();_tmp335[2]=Cyc_PP_text(({
const char*_tmp337=") ";_tag_dynforward(_tmp337,sizeof(char),
_get_zero_arr_size_char(_tmp337,3));}));_tmp335[1]=Cyc_Absynpp_exp2doc(_tmp300);
_tmp335[0]=Cyc_PP_text(({const char*_tmp336="if (";_tag_dynforward(_tmp336,
sizeof(char),_get_zero_arr_size_char(_tmp336,5));}));Cyc_PP_cat(_tag_dynforward(
_tmp335,sizeof(struct Cyc_PP_Doc*),8));});goto _LL25D;}_LL268: if(_tmp2FB <= (void*)
1)goto _LL26A;if(*((int*)_tmp2FB)!= 4)goto _LL26A;_tmp303=((struct Cyc_Absyn_While_s_struct*)
_tmp2FB)->f1;_tmp304=_tmp303.f1;_tmp305=((struct Cyc_Absyn_While_s_struct*)
_tmp2FB)->f2;_LL269: s=({struct Cyc_PP_Doc*_tmp33C[7];_tmp33C[6]=Cyc_Absynpp_rb();
_tmp33C[5]=Cyc_PP_line_doc();_tmp33C[4]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp33F[
2];_tmp33F[1]=Cyc_Absynpp_stmt2doc(_tmp305);_tmp33F[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_dynforward(_tmp33F,sizeof(struct Cyc_PP_Doc*),2));}));_tmp33C[3]=Cyc_Absynpp_lb();
_tmp33C[2]=Cyc_PP_text(({const char*_tmp33E=") ";_tag_dynforward(_tmp33E,sizeof(
char),_get_zero_arr_size_char(_tmp33E,3));}));_tmp33C[1]=Cyc_Absynpp_exp2doc(
_tmp304);_tmp33C[0]=Cyc_PP_text(({const char*_tmp33D="while (";_tag_dynforward(
_tmp33D,sizeof(char),_get_zero_arr_size_char(_tmp33D,8));}));Cyc_PP_cat(
_tag_dynforward(_tmp33C,sizeof(struct Cyc_PP_Doc*),7));});goto _LL25D;_LL26A: if(
_tmp2FB <= (void*)1)goto _LL26C;if(*((int*)_tmp2FB)!= 5)goto _LL26C;_LL26B: s=Cyc_PP_text(({
const char*_tmp340="break;";_tag_dynforward(_tmp340,sizeof(char),
_get_zero_arr_size_char(_tmp340,7));}));goto _LL25D;_LL26C: if(_tmp2FB <= (void*)1)
goto _LL26E;if(*((int*)_tmp2FB)!= 6)goto _LL26E;_LL26D: s=Cyc_PP_text(({const char*
_tmp341="continue;";_tag_dynforward(_tmp341,sizeof(char),_get_zero_arr_size_char(
_tmp341,10));}));goto _LL25D;_LL26E: if(_tmp2FB <= (void*)1)goto _LL270;if(*((int*)
_tmp2FB)!= 7)goto _LL270;_tmp306=((struct Cyc_Absyn_Goto_s_struct*)_tmp2FB)->f1;
_LL26F: s=Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp344;
_tmp344.tag=0;_tmp344.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp306);{
void*_tmp342[1]={& _tmp344};Cyc_aprintf(({const char*_tmp343="goto %s;";
_tag_dynforward(_tmp343,sizeof(char),_get_zero_arr_size_char(_tmp343,9));}),
_tag_dynforward(_tmp342,sizeof(void*),1));}}));goto _LL25D;_LL270: if(_tmp2FB <= (
void*)1)goto _LL272;if(*((int*)_tmp2FB)!= 8)goto _LL272;_tmp307=((struct Cyc_Absyn_For_s_struct*)
_tmp2FB)->f1;_tmp308=((struct Cyc_Absyn_For_s_struct*)_tmp2FB)->f2;_tmp309=
_tmp308.f1;_tmp30A=((struct Cyc_Absyn_For_s_struct*)_tmp2FB)->f3;_tmp30B=_tmp30A.f1;
_tmp30C=((struct Cyc_Absyn_For_s_struct*)_tmp2FB)->f4;_LL271: s=({struct Cyc_PP_Doc*
_tmp345[11];_tmp345[10]=Cyc_Absynpp_rb();_tmp345[9]=Cyc_PP_line_doc();_tmp345[8]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp34A[2];_tmp34A[1]=Cyc_Absynpp_stmt2doc(
_tmp30C);_tmp34A[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp34A,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp345[7]=Cyc_Absynpp_lb();_tmp345[6]=Cyc_PP_text(({
const char*_tmp349=") ";_tag_dynforward(_tmp349,sizeof(char),
_get_zero_arr_size_char(_tmp349,3));}));_tmp345[5]=Cyc_Absynpp_exp2doc(_tmp30B);
_tmp345[4]=Cyc_PP_text(({const char*_tmp348="; ";_tag_dynforward(_tmp348,sizeof(
char),_get_zero_arr_size_char(_tmp348,3));}));_tmp345[3]=Cyc_Absynpp_exp2doc(
_tmp309);_tmp345[2]=Cyc_PP_text(({const char*_tmp347="; ";_tag_dynforward(_tmp347,
sizeof(char),_get_zero_arr_size_char(_tmp347,3));}));_tmp345[1]=Cyc_Absynpp_exp2doc(
_tmp307);_tmp345[0]=Cyc_PP_text(({const char*_tmp346="for(";_tag_dynforward(
_tmp346,sizeof(char),_get_zero_arr_size_char(_tmp346,5));}));Cyc_PP_cat(
_tag_dynforward(_tmp345,sizeof(struct Cyc_PP_Doc*),11));});goto _LL25D;_LL272: if(
_tmp2FB <= (void*)1)goto _LL274;if(*((int*)_tmp2FB)!= 9)goto _LL274;_tmp30D=((
struct Cyc_Absyn_Switch_s_struct*)_tmp2FB)->f1;_tmp30E=((struct Cyc_Absyn_Switch_s_struct*)
_tmp2FB)->f2;_LL273: s=({struct Cyc_PP_Doc*_tmp34B[8];_tmp34B[7]=Cyc_Absynpp_rb();
_tmp34B[6]=Cyc_PP_line_doc();_tmp34B[5]=Cyc_Absynpp_switchclauses2doc(_tmp30E);
_tmp34B[4]=Cyc_PP_line_doc();_tmp34B[3]=Cyc_Absynpp_lb();_tmp34B[2]=Cyc_PP_text(({
const char*_tmp34D=") ";_tag_dynforward(_tmp34D,sizeof(char),
_get_zero_arr_size_char(_tmp34D,3));}));_tmp34B[1]=Cyc_Absynpp_exp2doc(_tmp30D);
_tmp34B[0]=Cyc_PP_text(({const char*_tmp34C="switch (";_tag_dynforward(_tmp34C,
sizeof(char),_get_zero_arr_size_char(_tmp34C,9));}));Cyc_PP_cat(_tag_dynforward(
_tmp34B,sizeof(struct Cyc_PP_Doc*),8));});goto _LL25D;_LL274: if(_tmp2FB <= (void*)1)
goto _LL276;if(*((int*)_tmp2FB)!= 10)goto _LL276;_tmp30F=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp2FB)->f1;if(_tmp30F != 0)goto _LL276;_LL275: s=Cyc_PP_text(({const char*_tmp34E="fallthru;";
_tag_dynforward(_tmp34E,sizeof(char),_get_zero_arr_size_char(_tmp34E,10));}));
goto _LL25D;_LL276: if(_tmp2FB <= (void*)1)goto _LL278;if(*((int*)_tmp2FB)!= 10)goto
_LL278;_tmp310=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp2FB)->f1;_LL277: s=({
struct Cyc_PP_Doc*_tmp34F[3];_tmp34F[2]=Cyc_PP_text(({const char*_tmp351=");";
_tag_dynforward(_tmp351,sizeof(char),_get_zero_arr_size_char(_tmp351,3));}));
_tmp34F[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp310);_tmp34F[0]=Cyc_PP_text(({const
char*_tmp350="fallthru(";_tag_dynforward(_tmp350,sizeof(char),
_get_zero_arr_size_char(_tmp350,10));}));Cyc_PP_cat(_tag_dynforward(_tmp34F,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL25D;_LL278: if(_tmp2FB <= (void*)1)goto
_LL27A;if(*((int*)_tmp2FB)!= 11)goto _LL27A;_tmp311=((struct Cyc_Absyn_Decl_s_struct*)
_tmp2FB)->f1;_tmp312=((struct Cyc_Absyn_Decl_s_struct*)_tmp2FB)->f2;_LL279: s=({
struct Cyc_PP_Doc*_tmp352[3];_tmp352[2]=Cyc_Absynpp_stmt2doc(_tmp312);_tmp352[1]=
Cyc_PP_line_doc();_tmp352[0]=Cyc_Absynpp_decl2doc(_tmp311);Cyc_PP_cat(
_tag_dynforward(_tmp352,sizeof(struct Cyc_PP_Doc*),3));});goto _LL25D;_LL27A: if(
_tmp2FB <= (void*)1)goto _LL27C;if(*((int*)_tmp2FB)!= 12)goto _LL27C;_tmp313=((
struct Cyc_Absyn_Label_s_struct*)_tmp2FB)->f1;_tmp314=((struct Cyc_Absyn_Label_s_struct*)
_tmp2FB)->f2;_LL27B: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(
_tmp314))s=({struct Cyc_PP_Doc*_tmp353[7];_tmp353[6]=Cyc_Absynpp_rb();_tmp353[5]=
Cyc_PP_line_doc();_tmp353[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp314));
_tmp353[3]=Cyc_PP_line_doc();_tmp353[2]=Cyc_Absynpp_lb();_tmp353[1]=Cyc_PP_text(({
const char*_tmp354=": ";_tag_dynforward(_tmp354,sizeof(char),
_get_zero_arr_size_char(_tmp354,3));}));_tmp353[0]=Cyc_PP_textptr(_tmp313);Cyc_PP_cat(
_tag_dynforward(_tmp353,sizeof(struct Cyc_PP_Doc*),7));});else{s=({struct Cyc_PP_Doc*
_tmp355[3];_tmp355[2]=Cyc_Absynpp_stmt2doc(_tmp314);_tmp355[1]=Cyc_PP_text(({
const char*_tmp356=": ";_tag_dynforward(_tmp356,sizeof(char),
_get_zero_arr_size_char(_tmp356,3));}));_tmp355[0]=Cyc_PP_textptr(_tmp313);Cyc_PP_cat(
_tag_dynforward(_tmp355,sizeof(struct Cyc_PP_Doc*),3));});}goto _LL25D;_LL27C: if(
_tmp2FB <= (void*)1)goto _LL27E;if(*((int*)_tmp2FB)!= 13)goto _LL27E;_tmp315=((
struct Cyc_Absyn_Do_s_struct*)_tmp2FB)->f1;_tmp316=((struct Cyc_Absyn_Do_s_struct*)
_tmp2FB)->f2;_tmp317=_tmp316.f1;_LL27D: s=({struct Cyc_PP_Doc*_tmp357[9];_tmp357[8]=
Cyc_PP_text(({const char*_tmp35A=");";_tag_dynforward(_tmp35A,sizeof(char),
_get_zero_arr_size_char(_tmp35A,3));}));_tmp357[7]=Cyc_Absynpp_exp2doc(_tmp317);
_tmp357[6]=Cyc_PP_text(({const char*_tmp359=" while (";_tag_dynforward(_tmp359,
sizeof(char),_get_zero_arr_size_char(_tmp359,9));}));_tmp357[5]=Cyc_Absynpp_rb();
_tmp357[4]=Cyc_PP_line_doc();_tmp357[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp315));_tmp357[2]=Cyc_PP_line_doc();_tmp357[1]=Cyc_Absynpp_lb();_tmp357[0]=
Cyc_PP_text(({const char*_tmp358="do ";_tag_dynforward(_tmp358,sizeof(char),
_get_zero_arr_size_char(_tmp358,4));}));Cyc_PP_cat(_tag_dynforward(_tmp357,
sizeof(struct Cyc_PP_Doc*),9));});goto _LL25D;_LL27E: if(_tmp2FB <= (void*)1)goto
_LL280;if(*((int*)_tmp2FB)!= 14)goto _LL280;_tmp318=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp2FB)->f1;_tmp319=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp2FB)->f2;_LL27F: s=({
struct Cyc_PP_Doc*_tmp35B[12];_tmp35B[11]=Cyc_Absynpp_rb();_tmp35B[10]=Cyc_PP_line_doc();
_tmp35B[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp319));_tmp35B[8]=Cyc_PP_line_doc();
_tmp35B[7]=Cyc_Absynpp_lb();_tmp35B[6]=Cyc_PP_text(({const char*_tmp35D=" catch ";
_tag_dynforward(_tmp35D,sizeof(char),_get_zero_arr_size_char(_tmp35D,8));}));
_tmp35B[5]=Cyc_Absynpp_rb();_tmp35B[4]=Cyc_PP_line_doc();_tmp35B[3]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp318));_tmp35B[2]=Cyc_PP_line_doc();_tmp35B[1]=Cyc_Absynpp_lb();
_tmp35B[0]=Cyc_PP_text(({const char*_tmp35C="try ";_tag_dynforward(_tmp35C,
sizeof(char),_get_zero_arr_size_char(_tmp35C,5));}));Cyc_PP_cat(_tag_dynforward(
_tmp35B,sizeof(struct Cyc_PP_Doc*),12));});goto _LL25D;_LL280: if(_tmp2FB <= (void*)
1)goto _LL282;if(*((int*)_tmp2FB)!= 15)goto _LL282;_tmp31A=((struct Cyc_Absyn_Region_s_struct*)
_tmp2FB)->f1;_tmp31B=((struct Cyc_Absyn_Region_s_struct*)_tmp2FB)->f2;_tmp31C=((
struct Cyc_Absyn_Region_s_struct*)_tmp2FB)->f3;_tmp31D=((struct Cyc_Absyn_Region_s_struct*)
_tmp2FB)->f4;_tmp31E=((struct Cyc_Absyn_Region_s_struct*)_tmp2FB)->f5;_LL281: s=({
struct Cyc_PP_Doc*_tmp35E[12];_tmp35E[11]=Cyc_Absynpp_rb();_tmp35E[10]=Cyc_PP_line_doc();
_tmp35E[9]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp31E));_tmp35E[8]=Cyc_PP_line_doc();
_tmp35E[7]=Cyc_Absynpp_lb();_tmp35E[6]=_tmp31D != 0?({struct Cyc_PP_Doc*_tmp363[3];
_tmp363[2]=Cyc_PP_text(({const char*_tmp365=")";_tag_dynforward(_tmp365,sizeof(
char),_get_zero_arr_size_char(_tmp365,2));}));_tmp363[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp31D);_tmp363[0]=Cyc_PP_text(({const char*_tmp364=" = open(";
_tag_dynforward(_tmp364,sizeof(char),_get_zero_arr_size_char(_tmp364,9));}));Cyc_PP_cat(
_tag_dynforward(_tmp363,sizeof(struct Cyc_PP_Doc*),3));}): Cyc_PP_nil_doc();
_tmp35E[5]=Cyc_Absynpp_qvar2doc(_tmp31B->name);_tmp35E[4]=Cyc_PP_text(({const
char*_tmp362=">";_tag_dynforward(_tmp362,sizeof(char),_get_zero_arr_size_char(
_tmp362,2));}));_tmp35E[3]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp31A));_tmp35E[
2]=Cyc_PP_text(({const char*_tmp361="<";_tag_dynforward(_tmp361,sizeof(char),
_get_zero_arr_size_char(_tmp361,2));}));_tmp35E[1]=_tmp31C?Cyc_PP_nil_doc(): Cyc_PP_text(({
const char*_tmp360="[resetable]";_tag_dynforward(_tmp360,sizeof(char),
_get_zero_arr_size_char(_tmp360,12));}));_tmp35E[0]=Cyc_PP_text(({const char*
_tmp35F="region";_tag_dynforward(_tmp35F,sizeof(char),_get_zero_arr_size_char(
_tmp35F,7));}));Cyc_PP_cat(_tag_dynforward(_tmp35E,sizeof(struct Cyc_PP_Doc*),12));});
goto _LL25D;_LL282: if(_tmp2FB <= (void*)1)goto _LL284;if(*((int*)_tmp2FB)!= 16)goto
_LL284;_tmp31F=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp2FB)->f1;_LL283: s=({
struct Cyc_PP_Doc*_tmp366[3];_tmp366[2]=Cyc_PP_text(({const char*_tmp368=");";
_tag_dynforward(_tmp368,sizeof(char),_get_zero_arr_size_char(_tmp368,3));}));
_tmp366[1]=Cyc_Absynpp_exp2doc(_tmp31F);_tmp366[0]=Cyc_PP_text(({const char*
_tmp367="reset_region(";_tag_dynforward(_tmp367,sizeof(char),
_get_zero_arr_size_char(_tmp367,14));}));Cyc_PP_cat(_tag_dynforward(_tmp366,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL25D;_LL284: if(_tmp2FB <= (void*)1)goto
_LL25D;if(*((int*)_tmp2FB)!= 17)goto _LL25D;_tmp320=((struct Cyc_Absyn_Alias_s_struct*)
_tmp2FB)->f1;_tmp321=((struct Cyc_Absyn_Alias_s_struct*)_tmp2FB)->f2;_tmp322=((
struct Cyc_Absyn_Alias_s_struct*)_tmp2FB)->f3;_tmp323=((struct Cyc_Absyn_Alias_s_struct*)
_tmp2FB)->f4;_LL285: s=({struct Cyc_PP_Doc*_tmp369[12];_tmp369[11]=Cyc_Absynpp_rb();
_tmp369[10]=Cyc_PP_line_doc();_tmp369[9]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp323));_tmp369[8]=Cyc_PP_line_doc();_tmp369[7]=Cyc_Absynpp_lb();_tmp369[6]=
Cyc_Absynpp_qvar2doc(_tmp322->name);_tmp369[5]=Cyc_PP_text(({const char*_tmp36D=">";
_tag_dynforward(_tmp36D,sizeof(char),_get_zero_arr_size_char(_tmp36D,2));}));
_tmp369[4]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp321));_tmp369[3]=Cyc_PP_text(({
const char*_tmp36C="<";_tag_dynforward(_tmp36C,sizeof(char),
_get_zero_arr_size_char(_tmp36C,2));}));_tmp369[2]=Cyc_PP_text(({const char*
_tmp36B=") = ";_tag_dynforward(_tmp36B,sizeof(char),_get_zero_arr_size_char(
_tmp36B,5));}));_tmp369[1]=Cyc_Absynpp_exp2doc(_tmp320);_tmp369[0]=Cyc_PP_text(({
const char*_tmp36A="alias(";_tag_dynforward(_tmp36A,sizeof(char),
_get_zero_arr_size_char(_tmp36A,7));}));Cyc_PP_cat(_tag_dynforward(_tmp369,
sizeof(struct Cyc_PP_Doc*),12));});goto _LL25D;_LL25D:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){struct Cyc_PP_Doc*s;{void*_tmp36E=(
void*)p->r;void*_tmp36F;int _tmp370;char _tmp371;struct _dynforward_ptr _tmp372;
struct Cyc_Absyn_Vardecl*_tmp373;struct Cyc_Absyn_Pat*_tmp374;struct Cyc_Absyn_Pat
_tmp375;void*_tmp376;struct Cyc_Absyn_Vardecl*_tmp377;struct Cyc_Absyn_Pat*_tmp378;
struct Cyc_Absyn_Tvar*_tmp379;struct Cyc_Absyn_Vardecl*_tmp37A;struct Cyc_List_List*
_tmp37B;int _tmp37C;struct Cyc_Absyn_Pat*_tmp37D;struct Cyc_Absyn_Vardecl*_tmp37E;
struct Cyc_Absyn_Pat*_tmp37F;struct Cyc_Absyn_Pat _tmp380;void*_tmp381;struct Cyc_Absyn_Vardecl*
_tmp382;struct Cyc_Absyn_Pat*_tmp383;struct _tuple0*_tmp384;struct _tuple0*_tmp385;
struct Cyc_List_List*_tmp386;int _tmp387;struct Cyc_Absyn_AggrInfo _tmp388;union Cyc_Absyn_AggrInfoU_union
_tmp389;struct Cyc_List_List*_tmp38A;struct Cyc_List_List*_tmp38B;int _tmp38C;
struct Cyc_Absyn_Enumfield*_tmp38D;struct Cyc_Absyn_Enumfield*_tmp38E;struct Cyc_Absyn_Tunionfield*
_tmp38F;struct Cyc_List_List*_tmp390;struct Cyc_Absyn_Tunionfield*_tmp391;struct
Cyc_List_List*_tmp392;int _tmp393;struct Cyc_Absyn_Exp*_tmp394;_LL28C: if((int)
_tmp36E != 0)goto _LL28E;_LL28D: s=Cyc_PP_text(({const char*_tmp395="_";
_tag_dynforward(_tmp395,sizeof(char),_get_zero_arr_size_char(_tmp395,2));}));
goto _LL28B;_LL28E: if((int)_tmp36E != 1)goto _LL290;_LL28F: s=Cyc_PP_text(({const
char*_tmp396="NULL";_tag_dynforward(_tmp396,sizeof(char),_get_zero_arr_size_char(
_tmp396,5));}));goto _LL28B;_LL290: if(_tmp36E <= (void*)2)goto _LL292;if(*((int*)
_tmp36E)!= 7)goto _LL292;_tmp36F=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp36E)->f1;
_tmp370=((struct Cyc_Absyn_Int_p_struct*)_tmp36E)->f2;_LL291: if(_tmp36F != (void*)((
void*)1))s=Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp399;
_tmp399.tag=1;_tmp399.f1=(unsigned long)_tmp370;{void*_tmp397[1]={& _tmp399};Cyc_aprintf(({
const char*_tmp398="%d";_tag_dynforward(_tmp398,sizeof(char),
_get_zero_arr_size_char(_tmp398,3));}),_tag_dynforward(_tmp397,sizeof(void*),1));}}));
else{s=Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp39C;
_tmp39C.tag=1;_tmp39C.f1=(unsigned int)_tmp370;{void*_tmp39A[1]={& _tmp39C};Cyc_aprintf(({
const char*_tmp39B="%u";_tag_dynforward(_tmp39B,sizeof(char),
_get_zero_arr_size_char(_tmp39B,3));}),_tag_dynforward(_tmp39A,sizeof(void*),1));}}));}
goto _LL28B;_LL292: if(_tmp36E <= (void*)2)goto _LL294;if(*((int*)_tmp36E)!= 8)goto
_LL294;_tmp371=((struct Cyc_Absyn_Char_p_struct*)_tmp36E)->f1;_LL293: s=Cyc_PP_text((
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp39F;_tmp39F.tag=0;_tmp39F.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_char_escape(_tmp371));{
void*_tmp39D[1]={& _tmp39F};Cyc_aprintf(({const char*_tmp39E="'%s'";
_tag_dynforward(_tmp39E,sizeof(char),_get_zero_arr_size_char(_tmp39E,5));}),
_tag_dynforward(_tmp39D,sizeof(void*),1));}}));goto _LL28B;_LL294: if(_tmp36E <= (
void*)2)goto _LL296;if(*((int*)_tmp36E)!= 9)goto _LL296;_tmp372=((struct Cyc_Absyn_Float_p_struct*)
_tmp36E)->f1;_LL295: s=Cyc_PP_text(_tmp372);goto _LL28B;_LL296: if(_tmp36E <= (void*)
2)goto _LL298;if(*((int*)_tmp36E)!= 0)goto _LL298;_tmp373=((struct Cyc_Absyn_Var_p_struct*)
_tmp36E)->f1;_tmp374=((struct Cyc_Absyn_Var_p_struct*)_tmp36E)->f2;_tmp375=*
_tmp374;_tmp376=(void*)_tmp375.r;if((int)_tmp376 != 0)goto _LL298;_LL297: s=Cyc_Absynpp_qvar2doc(
_tmp373->name);goto _LL28B;_LL298: if(_tmp36E <= (void*)2)goto _LL29A;if(*((int*)
_tmp36E)!= 0)goto _LL29A;_tmp377=((struct Cyc_Absyn_Var_p_struct*)_tmp36E)->f1;
_tmp378=((struct Cyc_Absyn_Var_p_struct*)_tmp36E)->f2;_LL299: s=({struct Cyc_PP_Doc*
_tmp3A0[3];_tmp3A0[2]=Cyc_Absynpp_pat2doc(_tmp378);_tmp3A0[1]=Cyc_PP_text(({
const char*_tmp3A1=" as ";_tag_dynforward(_tmp3A1,sizeof(char),
_get_zero_arr_size_char(_tmp3A1,5));}));_tmp3A0[0]=Cyc_Absynpp_qvar2doc(_tmp377->name);
Cyc_PP_cat(_tag_dynforward(_tmp3A0,sizeof(struct Cyc_PP_Doc*),3));});goto _LL28B;
_LL29A: if(_tmp36E <= (void*)2)goto _LL29C;if(*((int*)_tmp36E)!= 2)goto _LL29C;
_tmp379=((struct Cyc_Absyn_TagInt_p_struct*)_tmp36E)->f1;_tmp37A=((struct Cyc_Absyn_TagInt_p_struct*)
_tmp36E)->f2;_LL29B: s=({struct Cyc_PP_Doc*_tmp3A2[4];_tmp3A2[3]=Cyc_PP_text(({
const char*_tmp3A4=">";_tag_dynforward(_tmp3A4,sizeof(char),
_get_zero_arr_size_char(_tmp3A4,2));}));_tmp3A2[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp379));_tmp3A2[1]=Cyc_PP_text(({const char*_tmp3A3="<";_tag_dynforward(_tmp3A3,
sizeof(char),_get_zero_arr_size_char(_tmp3A3,2));}));_tmp3A2[0]=Cyc_Absynpp_qvar2doc(
_tmp37A->name);Cyc_PP_cat(_tag_dynforward(_tmp3A2,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL28B;_LL29C: if(_tmp36E <= (void*)2)goto _LL29E;if(*((int*)_tmp36E)!= 3)goto
_LL29E;_tmp37B=((struct Cyc_Absyn_Tuple_p_struct*)_tmp36E)->f1;_tmp37C=((struct
Cyc_Absyn_Tuple_p_struct*)_tmp36E)->f2;_LL29D: s=({struct Cyc_PP_Doc*_tmp3A5[4];
_tmp3A5[3]=_tmp37C?Cyc_PP_text(({const char*_tmp3A8=", ...)";_tag_dynforward(
_tmp3A8,sizeof(char),_get_zero_arr_size_char(_tmp3A8,7));})): Cyc_PP_text(({const
char*_tmp3A9=")";_tag_dynforward(_tmp3A9,sizeof(char),_get_zero_arr_size_char(
_tmp3A9,2));}));_tmp3A5[2]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct
Cyc_Absyn_Pat*),struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_pat2doc,({const char*_tmp3A7=",";_tag_dynforward(_tmp3A7,sizeof(char),
_get_zero_arr_size_char(_tmp3A7,2));}),_tmp37B);_tmp3A5[1]=Cyc_PP_text(({const
char*_tmp3A6="(";_tag_dynforward(_tmp3A6,sizeof(char),_get_zero_arr_size_char(
_tmp3A6,2));}));_tmp3A5[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dynforward(
_tmp3A5,sizeof(struct Cyc_PP_Doc*),4));});goto _LL28B;_LL29E: if(_tmp36E <= (void*)2)
goto _LL2A0;if(*((int*)_tmp36E)!= 4)goto _LL2A0;_tmp37D=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp36E)->f1;_LL29F: s=({struct Cyc_PP_Doc*_tmp3AA[2];_tmp3AA[1]=Cyc_Absynpp_pat2doc(
_tmp37D);_tmp3AA[0]=Cyc_PP_text(({const char*_tmp3AB="&";_tag_dynforward(_tmp3AB,
sizeof(char),_get_zero_arr_size_char(_tmp3AB,2));}));Cyc_PP_cat(_tag_dynforward(
_tmp3AA,sizeof(struct Cyc_PP_Doc*),2));});goto _LL28B;_LL2A0: if(_tmp36E <= (void*)2)
goto _LL2A2;if(*((int*)_tmp36E)!= 1)goto _LL2A2;_tmp37E=((struct Cyc_Absyn_Reference_p_struct*)
_tmp36E)->f1;_tmp37F=((struct Cyc_Absyn_Reference_p_struct*)_tmp36E)->f2;_tmp380=*
_tmp37F;_tmp381=(void*)_tmp380.r;if((int)_tmp381 != 0)goto _LL2A2;_LL2A1: s=({
struct Cyc_PP_Doc*_tmp3AC[2];_tmp3AC[1]=Cyc_Absynpp_qvar2doc(_tmp37E->name);
_tmp3AC[0]=Cyc_PP_text(({const char*_tmp3AD="*";_tag_dynforward(_tmp3AD,sizeof(
char),_get_zero_arr_size_char(_tmp3AD,2));}));Cyc_PP_cat(_tag_dynforward(_tmp3AC,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL28B;_LL2A2: if(_tmp36E <= (void*)2)goto
_LL2A4;if(*((int*)_tmp36E)!= 1)goto _LL2A4;_tmp382=((struct Cyc_Absyn_Reference_p_struct*)
_tmp36E)->f1;_tmp383=((struct Cyc_Absyn_Reference_p_struct*)_tmp36E)->f2;_LL2A3: s=({
struct Cyc_PP_Doc*_tmp3AE[4];_tmp3AE[3]=Cyc_Absynpp_pat2doc(_tmp383);_tmp3AE[2]=
Cyc_PP_text(({const char*_tmp3B0=" as ";_tag_dynforward(_tmp3B0,sizeof(char),
_get_zero_arr_size_char(_tmp3B0,5));}));_tmp3AE[1]=Cyc_Absynpp_qvar2doc(_tmp382->name);
_tmp3AE[0]=Cyc_PP_text(({const char*_tmp3AF="*";_tag_dynforward(_tmp3AF,sizeof(
char),_get_zero_arr_size_char(_tmp3AF,2));}));Cyc_PP_cat(_tag_dynforward(_tmp3AE,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL28B;_LL2A4: if(_tmp36E <= (void*)2)goto
_LL2A6;if(*((int*)_tmp36E)!= 12)goto _LL2A6;_tmp384=((struct Cyc_Absyn_UnknownId_p_struct*)
_tmp36E)->f1;_LL2A5: s=Cyc_Absynpp_qvar2doc(_tmp384);goto _LL28B;_LL2A6: if(_tmp36E
<= (void*)2)goto _LL2A8;if(*((int*)_tmp36E)!= 13)goto _LL2A8;_tmp385=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp36E)->f1;_tmp386=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp36E)->f2;
_tmp387=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp36E)->f3;_LL2A7: {struct
_dynforward_ptr term=_tmp387?({const char*_tmp3B4=", ...)";_tag_dynforward(_tmp3B4,
sizeof(char),_get_zero_arr_size_char(_tmp3B4,7));}):({const char*_tmp3B5=")";
_tag_dynforward(_tmp3B5,sizeof(char),_get_zero_arr_size_char(_tmp3B5,2));});s=({
struct Cyc_PP_Doc*_tmp3B1[2];_tmp3B1[1]=Cyc_PP_group(({const char*_tmp3B2="(";
_tag_dynforward(_tmp3B2,sizeof(char),_get_zero_arr_size_char(_tmp3B2,2));}),term,({
const char*_tmp3B3=",";_tag_dynforward(_tmp3B3,sizeof(char),
_get_zero_arr_size_char(_tmp3B3,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,
_tmp386));_tmp3B1[0]=Cyc_Absynpp_qvar2doc(_tmp385);Cyc_PP_cat(_tag_dynforward(
_tmp3B1,sizeof(struct Cyc_PP_Doc*),2));});goto _LL28B;}_LL2A8: if(_tmp36E <= (void*)
2)goto _LL2AA;if(*((int*)_tmp36E)!= 5)goto _LL2AA;_tmp388=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp36E)->f1;_tmp389=_tmp388.aggr_info;_tmp38A=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp36E)->f2;_tmp38B=((struct Cyc_Absyn_Aggr_p_struct*)_tmp36E)->f3;_tmp38C=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp36E)->f4;_LL2A9: {struct _dynforward_ptr term=
_tmp38C?({const char*_tmp3BE=", ...}";_tag_dynforward(_tmp3BE,sizeof(char),
_get_zero_arr_size_char(_tmp3BE,7));}):({const char*_tmp3BF="}";_tag_dynforward(
_tmp3BF,sizeof(char),_get_zero_arr_size_char(_tmp3BF,2));});struct _tuple0*
_tmp3B7;struct _tuple3 _tmp3B6=Cyc_Absyn_aggr_kinded_name(_tmp389);_tmp3B7=_tmp3B6.f2;
s=({struct Cyc_PP_Doc*_tmp3B8[4];_tmp3B8[3]=Cyc_PP_group(({const char*_tmp3BC="";
_tag_dynforward(_tmp3BC,sizeof(char),_get_zero_arr_size_char(_tmp3BC,1));}),term,({
const char*_tmp3BD=",";_tag_dynforward(_tmp3BD,sizeof(char),
_get_zero_arr_size_char(_tmp3BD,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp38B));_tmp3B8[2]=Cyc_PP_egroup(({const char*_tmp3B9="[";_tag_dynforward(
_tmp3B9,sizeof(char),_get_zero_arr_size_char(_tmp3B9,2));}),({const char*_tmp3BA="]";
_tag_dynforward(_tmp3BA,sizeof(char),_get_zero_arr_size_char(_tmp3BA,2));}),({
const char*_tmp3BB=",";_tag_dynforward(_tmp3BB,sizeof(char),
_get_zero_arr_size_char(_tmp3BB,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((
struct Cyc_List_List*(*)(struct _dynforward_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp38A)));_tmp3B8[1]=Cyc_Absynpp_lb();
_tmp3B8[0]=Cyc_Absynpp_qvar2doc(_tmp3B7);Cyc_PP_cat(_tag_dynforward(_tmp3B8,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL28B;}_LL2AA: if(_tmp36E <= (void*)2)goto
_LL2AC;if(*((int*)_tmp36E)!= 10)goto _LL2AC;_tmp38D=((struct Cyc_Absyn_Enum_p_struct*)
_tmp36E)->f2;_LL2AB: _tmp38E=_tmp38D;goto _LL2AD;_LL2AC: if(_tmp36E <= (void*)2)goto
_LL2AE;if(*((int*)_tmp36E)!= 11)goto _LL2AE;_tmp38E=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp36E)->f2;_LL2AD: s=Cyc_Absynpp_qvar2doc(_tmp38E->name);goto _LL28B;_LL2AE: if(
_tmp36E <= (void*)2)goto _LL2B0;if(*((int*)_tmp36E)!= 6)goto _LL2B0;_tmp38F=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp36E)->f2;_tmp390=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp36E)->f3;if(_tmp390 != 0)goto _LL2B0;_LL2AF: s=Cyc_Absynpp_qvar2doc(_tmp38F->name);
goto _LL28B;_LL2B0: if(_tmp36E <= (void*)2)goto _LL2B2;if(*((int*)_tmp36E)!= 6)goto
_LL2B2;_tmp391=((struct Cyc_Absyn_Tunion_p_struct*)_tmp36E)->f2;_tmp392=((struct
Cyc_Absyn_Tunion_p_struct*)_tmp36E)->f3;_tmp393=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp36E)->f4;_LL2B1: {struct _dynforward_ptr term=_tmp393?({const char*_tmp3C3=", ...)";
_tag_dynforward(_tmp3C3,sizeof(char),_get_zero_arr_size_char(_tmp3C3,7));}):({
const char*_tmp3C4=")";_tag_dynforward(_tmp3C4,sizeof(char),
_get_zero_arr_size_char(_tmp3C4,2));});s=({struct Cyc_PP_Doc*_tmp3C0[2];_tmp3C0[1]=
Cyc_PP_egroup(({const char*_tmp3C1="(";_tag_dynforward(_tmp3C1,sizeof(char),
_get_zero_arr_size_char(_tmp3C1,2));}),term,({const char*_tmp3C2=",";
_tag_dynforward(_tmp3C2,sizeof(char),_get_zero_arr_size_char(_tmp3C2,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp392));_tmp3C0[0]=Cyc_Absynpp_qvar2doc(
_tmp391->name);Cyc_PP_cat(_tag_dynforward(_tmp3C0,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL28B;}_LL2B2: if(_tmp36E <= (void*)2)goto _LL28B;if(*((int*)_tmp36E)!= 14)
goto _LL28B;_tmp394=((struct Cyc_Absyn_Exp_p_struct*)_tmp36E)->f1;_LL2B3: s=Cyc_Absynpp_exp2doc(
_tmp394);goto _LL28B;_LL28B:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple8*dp){return({struct Cyc_PP_Doc*_tmp3C5[2];_tmp3C5[1]=Cyc_Absynpp_pat2doc((*
dp).f2);_tmp3C5[0]=Cyc_PP_egroup(({const char*_tmp3C6="";_tag_dynforward(_tmp3C6,
sizeof(char),_get_zero_arr_size_char(_tmp3C6,1));}),({const char*_tmp3C7="=";
_tag_dynforward(_tmp3C7,sizeof(char),_get_zero_arr_size_char(_tmp3C7,2));}),({
const char*_tmp3C8="=";_tag_dynforward(_tmp3C8,sizeof(char),
_get_zero_arr_size_char(_tmp3C8,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));
Cyc_PP_cat(_tag_dynforward(_tmp3C5,sizeof(struct Cyc_PP_Doc*),2));});}struct Cyc_PP_Doc*
Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){if(c->where_clause
== 0  && (void*)(c->pattern)->r == (void*)((void*)0))return({struct Cyc_PP_Doc*
_tmp3C9[2];_tmp3C9[1]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3CB[2];_tmp3CB[1]=Cyc_Absynpp_stmt2doc(
c->body);_tmp3CB[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp3CB,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp3C9[0]=Cyc_PP_text(({const char*_tmp3CA="default: ";
_tag_dynforward(_tmp3CA,sizeof(char),_get_zero_arr_size_char(_tmp3CA,10));}));
Cyc_PP_cat(_tag_dynforward(_tmp3C9,sizeof(struct Cyc_PP_Doc*),2));});else{if(c->where_clause
== 0)return({struct Cyc_PP_Doc*_tmp3CC[4];_tmp3CC[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp3CF[2];_tmp3CF[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp3CF[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_dynforward(_tmp3CF,sizeof(struct Cyc_PP_Doc*),2));}));_tmp3CC[2]=
Cyc_PP_text(({const char*_tmp3CE=": ";_tag_dynforward(_tmp3CE,sizeof(char),
_get_zero_arr_size_char(_tmp3CE,3));}));_tmp3CC[1]=Cyc_Absynpp_pat2doc(c->pattern);
_tmp3CC[0]=Cyc_PP_text(({const char*_tmp3CD="case ";_tag_dynforward(_tmp3CD,
sizeof(char),_get_zero_arr_size_char(_tmp3CD,6));}));Cyc_PP_cat(_tag_dynforward(
_tmp3CC,sizeof(struct Cyc_PP_Doc*),4));});else{return({struct Cyc_PP_Doc*_tmp3D0[6];
_tmp3D0[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3D4[2];_tmp3D4[1]=Cyc_Absynpp_stmt2doc(
c->body);_tmp3D4[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp3D4,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp3D0[4]=Cyc_PP_text(({const char*_tmp3D3=": ";
_tag_dynforward(_tmp3D3,sizeof(char),_get_zero_arr_size_char(_tmp3D3,3));}));
_tmp3D0[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));
_tmp3D0[2]=Cyc_PP_text(({const char*_tmp3D2=" && ";_tag_dynforward(_tmp3D2,
sizeof(char),_get_zero_arr_size_char(_tmp3D2,5));}));_tmp3D0[1]=Cyc_Absynpp_pat2doc(
c->pattern);_tmp3D0[0]=Cyc_PP_text(({const char*_tmp3D1="case ";_tag_dynforward(
_tmp3D1,sizeof(char),_get_zero_arr_size_char(_tmp3D1,6));}));Cyc_PP_cat(
_tag_dynforward(_tmp3D0,sizeof(struct Cyc_PP_Doc*),6));});}}}struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List*cs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Switch_clause*),struct _dynforward_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,({const char*_tmp3D5="";
_tag_dynforward(_tmp3D5,sizeof(char),_get_zero_arr_size_char(_tmp3D5,1));}),cs);}
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){if(f->tag
== 0)return Cyc_Absynpp_qvar2doc(f->name);else{return({struct Cyc_PP_Doc*_tmp3D6[3];
_tmp3D6[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));
_tmp3D6[1]=Cyc_PP_text(({const char*_tmp3D7=" = ";_tag_dynforward(_tmp3D7,sizeof(
char),_get_zero_arr_size_char(_tmp3D7,4));}));_tmp3D6[0]=Cyc_Absynpp_qvar2doc(f->name);
Cyc_PP_cat(_tag_dynforward(_tmp3D6,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dynforward_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,({const char*
_tmp3D8=",";_tag_dynforward(_tmp3D8,sizeof(char),_get_zero_arr_size_char(_tmp3D8,
2));}),fs);}static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(
struct Cyc_List_List*vds){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Vardecl*),struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_id2doc,({const char*_tmp3D9=",";_tag_dynforward(_tmp3D9,sizeof(char),
_get_zero_arr_size_char(_tmp3D9,2));}),vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(
struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl _tmp3DB;void*_tmp3DC;struct
_tuple0*_tmp3DD;struct Cyc_Absyn_Tqual _tmp3DE;void*_tmp3DF;struct Cyc_Absyn_Exp*
_tmp3E0;struct Cyc_List_List*_tmp3E1;struct Cyc_Absyn_Vardecl*_tmp3DA=vd;_tmp3DB=*
_tmp3DA;_tmp3DC=(void*)_tmp3DB.sc;_tmp3DD=_tmp3DB.name;_tmp3DE=_tmp3DB.tq;
_tmp3DF=(void*)_tmp3DB.type;_tmp3E0=_tmp3DB.initializer;_tmp3E1=_tmp3DB.attributes;{
struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp3DD);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp3E1);struct Cyc_PP_Doc*
beforenamedoc;{void*_tmp3E2=Cyc_Cyclone_c_compiler;_LL2B5: if((int)_tmp3E2 != 0)
goto _LL2B7;_LL2B6: beforenamedoc=attsdoc;goto _LL2B4;_LL2B7: if((int)_tmp3E2 != 1)
goto _LL2B4;_LL2B8:{void*_tmp3E3=Cyc_Tcutil_compress(_tmp3DF);struct Cyc_Absyn_FnInfo
_tmp3E4;struct Cyc_List_List*_tmp3E5;_LL2BA: if(_tmp3E3 <= (void*)4)goto _LL2BC;if(*((
int*)_tmp3E3)!= 8)goto _LL2BC;_tmp3E4=((struct Cyc_Absyn_FnType_struct*)_tmp3E3)->f1;
_tmp3E5=_tmp3E4.attributes;_LL2BB: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp3E5);
goto _LL2B9;_LL2BC:;_LL2BD: beforenamedoc=Cyc_PP_nil_doc();goto _LL2B9;_LL2B9:;}
goto _LL2B4;_LL2B4:;}{struct Cyc_PP_Doc*tmp_doc;{void*_tmp3E6=Cyc_Cyclone_c_compiler;
_LL2BF: if((int)_tmp3E6 != 0)goto _LL2C1;_LL2C0: tmp_doc=Cyc_PP_nil_doc();goto _LL2BE;
_LL2C1: if((int)_tmp3E6 != 1)goto _LL2BE;_LL2C2: tmp_doc=attsdoc;goto _LL2BE;_LL2BE:;}
s=({struct Cyc_PP_Doc*_tmp3E7[5];_tmp3E7[4]=Cyc_PP_text(({const char*_tmp3EC=";";
_tag_dynforward(_tmp3EC,sizeof(char),_get_zero_arr_size_char(_tmp3EC,2));}));
_tmp3E7[3]=_tmp3E0 == 0?Cyc_PP_nil_doc():({struct Cyc_PP_Doc*_tmp3EA[2];_tmp3EA[1]=
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp3E0);_tmp3EA[0]=Cyc_PP_text(({
const char*_tmp3EB=" = ";_tag_dynforward(_tmp3EB,sizeof(char),
_get_zero_arr_size_char(_tmp3EB,4));}));Cyc_PP_cat(_tag_dynforward(_tmp3EA,
sizeof(struct Cyc_PP_Doc*),2));});_tmp3E7[2]=Cyc_Absynpp_tqtd2doc(_tmp3DE,_tmp3DF,({
struct Cyc_Core_Opt*_tmp3E8=_cycalloc(sizeof(*_tmp3E8));_tmp3E8->v=({struct Cyc_PP_Doc*
_tmp3E9[2];_tmp3E9[1]=sn;_tmp3E9[0]=beforenamedoc;Cyc_PP_cat(_tag_dynforward(
_tmp3E9,sizeof(struct Cyc_PP_Doc*),2));});_tmp3E8;}));_tmp3E7[1]=Cyc_Absynpp_scope2doc(
_tmp3DC);_tmp3E7[0]=tmp_doc;Cyc_PP_cat(_tag_dynforward(_tmp3E7,sizeof(struct Cyc_PP_Doc*),
5));});return s;}}}struct _tuple11{struct Cyc_Position_Segment*f1;struct _tuple0*f2;
int f3;};struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple11*x){struct _tuple0*
_tmp3EE;struct _tuple11 _tmp3ED=*x;_tmp3EE=_tmp3ED.f2;return Cyc_Absynpp_qvar2doc(
_tmp3EE);}struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct
Cyc_PP_Doc*s;{void*_tmp3EF=(void*)d->r;struct Cyc_Absyn_Fndecl*_tmp3F0;struct Cyc_Absyn_Aggrdecl*
_tmp3F1;struct Cyc_Absyn_Vardecl*_tmp3F2;struct Cyc_Absyn_Tuniondecl*_tmp3F3;
struct Cyc_Absyn_Tuniondecl _tmp3F4;void*_tmp3F5;struct _tuple0*_tmp3F6;struct Cyc_List_List*
_tmp3F7;struct Cyc_Core_Opt*_tmp3F8;int _tmp3F9;int _tmp3FA;struct Cyc_Absyn_Pat*
_tmp3FB;struct Cyc_Absyn_Exp*_tmp3FC;struct Cyc_List_List*_tmp3FD;struct Cyc_Absyn_Enumdecl*
_tmp3FE;struct Cyc_Absyn_Enumdecl _tmp3FF;void*_tmp400;struct _tuple0*_tmp401;
struct Cyc_Core_Opt*_tmp402;struct Cyc_Absyn_Typedefdecl*_tmp403;struct
_dynforward_ptr*_tmp404;struct Cyc_List_List*_tmp405;struct _tuple0*_tmp406;struct
Cyc_List_List*_tmp407;struct Cyc_List_List*_tmp408;struct Cyc_List_List*_tmp409;
struct Cyc_List_List*_tmp40A;_LL2C4: if(_tmp3EF <= (void*)2)goto _LL2DC;if(*((int*)
_tmp3EF)!= 1)goto _LL2C6;_tmp3F0=((struct Cyc_Absyn_Fn_d_struct*)_tmp3EF)->f1;
_LL2C5: {void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp416=_cycalloc(sizeof(*
_tmp416));_tmp416[0]=({struct Cyc_Absyn_FnType_struct _tmp417;_tmp417.tag=8;
_tmp417.f1=({struct Cyc_Absyn_FnInfo _tmp418;_tmp418.tvars=_tmp3F0->tvs;_tmp418.effect=
_tmp3F0->effect;_tmp418.ret_typ=(void*)((void*)_tmp3F0->ret_type);_tmp418.args=((
struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple6*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp3F0->args);_tmp418.c_varargs=_tmp3F0->c_varargs;
_tmp418.cyc_varargs=_tmp3F0->cyc_varargs;_tmp418.rgn_po=_tmp3F0->rgn_po;_tmp418.attributes=
0;_tmp418;});_tmp417;});_tmp416;});struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp3F0->attributes);struct Cyc_PP_Doc*inlinedoc;if(_tmp3F0->is_inline){void*
_tmp40B=Cyc_Cyclone_c_compiler;_LL2E1: if((int)_tmp40B != 0)goto _LL2E3;_LL2E2:
inlinedoc=Cyc_PP_text(({const char*_tmp40C="inline ";_tag_dynforward(_tmp40C,
sizeof(char),_get_zero_arr_size_char(_tmp40C,8));}));goto _LL2E0;_LL2E3: if((int)
_tmp40B != 1)goto _LL2E0;_LL2E4: inlinedoc=Cyc_PP_text(({const char*_tmp40D="__inline ";
_tag_dynforward(_tmp40D,sizeof(char),_get_zero_arr_size_char(_tmp40D,10));}));
goto _LL2E0;_LL2E0:;}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*scopedoc=
Cyc_Absynpp_scope2doc((void*)_tmp3F0->sc);struct Cyc_PP_Doc*beforenamedoc;{void*
_tmp40E=Cyc_Cyclone_c_compiler;_LL2E6: if((int)_tmp40E != 0)goto _LL2E8;_LL2E7:
beforenamedoc=attsdoc;goto _LL2E5;_LL2E8: if((int)_tmp40E != 1)goto _LL2E5;_LL2E9:
beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp3F0->attributes);goto _LL2E5;_LL2E5:;}{
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp3F0->name);struct Cyc_PP_Doc*
tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,({struct Cyc_Core_Opt*
_tmp414=_cycalloc(sizeof(*_tmp414));_tmp414->v=({struct Cyc_PP_Doc*_tmp415[2];
_tmp415[1]=namedoc;_tmp415[0]=beforenamedoc;Cyc_PP_cat(_tag_dynforward(_tmp415,
sizeof(struct Cyc_PP_Doc*),2));});_tmp414;}));struct Cyc_PP_Doc*bodydoc=({struct
Cyc_PP_Doc*_tmp412[5];_tmp412[4]=Cyc_Absynpp_rb();_tmp412[3]=Cyc_PP_line_doc();
_tmp412[2]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp413[2];_tmp413[1]=Cyc_Absynpp_stmt2doc(
_tmp3F0->body);_tmp413[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp413,
sizeof(struct Cyc_PP_Doc*),2));}));_tmp412[1]=Cyc_Absynpp_lb();_tmp412[0]=Cyc_PP_blank_doc();
Cyc_PP_cat(_tag_dynforward(_tmp412,sizeof(struct Cyc_PP_Doc*),5));});s=({struct
Cyc_PP_Doc*_tmp40F[4];_tmp40F[3]=bodydoc;_tmp40F[2]=tqtddoc;_tmp40F[1]=scopedoc;
_tmp40F[0]=inlinedoc;Cyc_PP_cat(_tag_dynforward(_tmp40F,sizeof(struct Cyc_PP_Doc*),
4));});{void*_tmp410=Cyc_Cyclone_c_compiler;_LL2EB: if((int)_tmp410 != 1)goto
_LL2ED;_LL2EC: s=({struct Cyc_PP_Doc*_tmp411[2];_tmp411[1]=s;_tmp411[0]=attsdoc;
Cyc_PP_cat(_tag_dynforward(_tmp411,sizeof(struct Cyc_PP_Doc*),2));});goto _LL2EA;
_LL2ED:;_LL2EE: goto _LL2EA;_LL2EA:;}goto _LL2C3;}}}_LL2C6: if(*((int*)_tmp3EF)!= 4)
goto _LL2C8;_tmp3F1=((struct Cyc_Absyn_Aggr_d_struct*)_tmp3EF)->f1;_LL2C7: if(
_tmp3F1->impl == 0)s=({struct Cyc_PP_Doc*_tmp419[5];_tmp419[4]=Cyc_PP_text(({const
char*_tmp41A=";";_tag_dynforward(_tmp41A,sizeof(char),_get_zero_arr_size_char(
_tmp41A,2));}));_tmp419[3]=Cyc_Absynpp_ktvars2doc(_tmp3F1->tvs);_tmp419[2]=Cyc_Absynpp_qvar2bolddoc(
_tmp3F1->name);_tmp419[1]=Cyc_Absynpp_aggr_kind2doc((void*)_tmp3F1->kind);
_tmp419[0]=Cyc_Absynpp_scope2doc((void*)_tmp3F1->sc);Cyc_PP_cat(_tag_dynforward(
_tmp419,sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*_tmp41B[12];
_tmp41B[11]=Cyc_PP_text(({const char*_tmp41F=";";_tag_dynforward(_tmp41F,sizeof(
char),_get_zero_arr_size_char(_tmp41F,2));}));_tmp41B[10]=Cyc_Absynpp_atts2doc(
_tmp3F1->attributes);_tmp41B[9]=Cyc_Absynpp_rb();_tmp41B[8]=Cyc_PP_line_doc();
_tmp41B[7]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp41E[2];_tmp41E[1]=Cyc_Absynpp_aggrfields2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3F1->impl))->fields);_tmp41E[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp41E,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp41B[6]=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3F1->impl))->rgn_po == 
0?Cyc_PP_nil_doc():({struct Cyc_PP_Doc*_tmp41C[2];_tmp41C[1]=Cyc_Absynpp_rgnpo2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3F1->impl))->rgn_po);_tmp41C[0]=
Cyc_PP_text(({const char*_tmp41D=":";_tag_dynforward(_tmp41D,sizeof(char),
_get_zero_arr_size_char(_tmp41D,2));}));Cyc_PP_cat(_tag_dynforward(_tmp41C,
sizeof(struct Cyc_PP_Doc*),2));});_tmp41B[5]=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3F1->impl))->exist_vars);_tmp41B[4]=Cyc_Absynpp_lb();_tmp41B[3]=
Cyc_PP_blank_doc();_tmp41B[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3F1->name);_tmp41B[1]=
Cyc_Absynpp_aggr_kind2doc((void*)_tmp3F1->kind);_tmp41B[0]=Cyc_Absynpp_scope2doc((
void*)_tmp3F1->sc);Cyc_PP_cat(_tag_dynforward(_tmp41B,sizeof(struct Cyc_PP_Doc*),
12));});}goto _LL2C3;_LL2C8: if(*((int*)_tmp3EF)!= 0)goto _LL2CA;_tmp3F2=((struct
Cyc_Absyn_Var_d_struct*)_tmp3EF)->f1;_LL2C9: s=Cyc_Absynpp_vardecl2doc(_tmp3F2);
goto _LL2C3;_LL2CA: if(*((int*)_tmp3EF)!= 5)goto _LL2CC;_tmp3F3=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp3EF)->f1;_tmp3F4=*_tmp3F3;_tmp3F5=(void*)_tmp3F4.sc;_tmp3F6=_tmp3F4.name;
_tmp3F7=_tmp3F4.tvs;_tmp3F8=_tmp3F4.fields;_tmp3F9=_tmp3F4.is_xtunion;_tmp3FA=
_tmp3F4.is_flat;_LL2CB: if(_tmp3F8 == 0)s=({struct Cyc_PP_Doc*_tmp420[6];_tmp420[5]=
Cyc_PP_text(({const char*_tmp424=";";_tag_dynforward(_tmp424,sizeof(char),
_get_zero_arr_size_char(_tmp424,2));}));_tmp420[4]=Cyc_Absynpp_ktvars2doc(
_tmp3F7);_tmp420[3]=_tmp3F9?Cyc_Absynpp_qvar2bolddoc(_tmp3F6): Cyc_Absynpp_typedef_name2bolddoc(
_tmp3F6);_tmp420[2]=_tmp3FA?Cyc_PP_text(({const char*_tmp423="__attribute__((flat)) ";
_tag_dynforward(_tmp423,sizeof(char),_get_zero_arr_size_char(_tmp423,23));})):
Cyc_PP_blank_doc();_tmp420[1]=_tmp3F9?Cyc_PP_text(({const char*_tmp421="xtunion ";
_tag_dynforward(_tmp421,sizeof(char),_get_zero_arr_size_char(_tmp421,9));})): Cyc_PP_text(({
const char*_tmp422="tunion ";_tag_dynforward(_tmp422,sizeof(char),
_get_zero_arr_size_char(_tmp422,8));}));_tmp420[0]=Cyc_Absynpp_scope2doc(_tmp3F5);
Cyc_PP_cat(_tag_dynforward(_tmp420,sizeof(struct Cyc_PP_Doc*),6));});else{s=({
struct Cyc_PP_Doc*_tmp425[11];_tmp425[10]=Cyc_PP_text(({const char*_tmp42A=";";
_tag_dynforward(_tmp42A,sizeof(char),_get_zero_arr_size_char(_tmp42A,2));}));
_tmp425[9]=Cyc_Absynpp_rb();_tmp425[8]=Cyc_PP_line_doc();_tmp425[7]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp429[2];_tmp429[1]=Cyc_Absynpp_tunionfields2doc((struct
Cyc_List_List*)_tmp3F8->v);_tmp429[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_dynforward(_tmp429,sizeof(struct Cyc_PP_Doc*),2));}));_tmp425[6]=Cyc_Absynpp_lb();
_tmp425[5]=Cyc_PP_blank_doc();_tmp425[4]=_tmp3FA?Cyc_PP_text(({const char*_tmp428="__attribute__((flat)) ";
_tag_dynforward(_tmp428,sizeof(char),_get_zero_arr_size_char(_tmp428,23));})):
Cyc_PP_blank_doc();_tmp425[3]=Cyc_Absynpp_ktvars2doc(_tmp3F7);_tmp425[2]=_tmp3F9?
Cyc_Absynpp_qvar2bolddoc(_tmp3F6): Cyc_Absynpp_typedef_name2bolddoc(_tmp3F6);
_tmp425[1]=_tmp3F9?Cyc_PP_text(({const char*_tmp426="xtunion ";_tag_dynforward(
_tmp426,sizeof(char),_get_zero_arr_size_char(_tmp426,9));})): Cyc_PP_text(({const
char*_tmp427="tunion ";_tag_dynforward(_tmp427,sizeof(char),
_get_zero_arr_size_char(_tmp427,8));}));_tmp425[0]=Cyc_Absynpp_scope2doc(_tmp3F5);
Cyc_PP_cat(_tag_dynforward(_tmp425,sizeof(struct Cyc_PP_Doc*),11));});}goto _LL2C3;
_LL2CC: if(*((int*)_tmp3EF)!= 2)goto _LL2CE;_tmp3FB=((struct Cyc_Absyn_Let_d_struct*)
_tmp3EF)->f1;_tmp3FC=((struct Cyc_Absyn_Let_d_struct*)_tmp3EF)->f3;_LL2CD: s=({
struct Cyc_PP_Doc*_tmp42B[5];_tmp42B[4]=Cyc_PP_text(({const char*_tmp42E=";";
_tag_dynforward(_tmp42E,sizeof(char),_get_zero_arr_size_char(_tmp42E,2));}));
_tmp42B[3]=Cyc_Absynpp_exp2doc(_tmp3FC);_tmp42B[2]=Cyc_PP_text(({const char*
_tmp42D=" = ";_tag_dynforward(_tmp42D,sizeof(char),_get_zero_arr_size_char(
_tmp42D,4));}));_tmp42B[1]=Cyc_Absynpp_pat2doc(_tmp3FB);_tmp42B[0]=Cyc_PP_text(({
const char*_tmp42C="let ";_tag_dynforward(_tmp42C,sizeof(char),
_get_zero_arr_size_char(_tmp42C,5));}));Cyc_PP_cat(_tag_dynforward(_tmp42B,
sizeof(struct Cyc_PP_Doc*),5));});goto _LL2C3;_LL2CE: if(*((int*)_tmp3EF)!= 3)goto
_LL2D0;_tmp3FD=((struct Cyc_Absyn_Letv_d_struct*)_tmp3EF)->f1;_LL2CF: s=({struct
Cyc_PP_Doc*_tmp42F[3];_tmp42F[2]=Cyc_PP_text(({const char*_tmp431=";";
_tag_dynforward(_tmp431,sizeof(char),_get_zero_arr_size_char(_tmp431,2));}));
_tmp42F[1]=Cyc_Absynpp_ids2doc(_tmp3FD);_tmp42F[0]=Cyc_PP_text(({const char*
_tmp430="let ";_tag_dynforward(_tmp430,sizeof(char),_get_zero_arr_size_char(
_tmp430,5));}));Cyc_PP_cat(_tag_dynforward(_tmp42F,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL2C3;_LL2D0: if(*((int*)_tmp3EF)!= 6)goto _LL2D2;_tmp3FE=((struct Cyc_Absyn_Enum_d_struct*)
_tmp3EF)->f1;_tmp3FF=*_tmp3FE;_tmp400=(void*)_tmp3FF.sc;_tmp401=_tmp3FF.name;
_tmp402=_tmp3FF.fields;_LL2D1: if(_tmp402 == 0)s=({struct Cyc_PP_Doc*_tmp432[4];
_tmp432[3]=Cyc_PP_text(({const char*_tmp434=";";_tag_dynforward(_tmp434,sizeof(
char),_get_zero_arr_size_char(_tmp434,2));}));_tmp432[2]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp401);_tmp432[1]=Cyc_PP_text(({const char*_tmp433="enum ";_tag_dynforward(
_tmp433,sizeof(char),_get_zero_arr_size_char(_tmp433,6));}));_tmp432[0]=Cyc_Absynpp_scope2doc(
_tmp400);Cyc_PP_cat(_tag_dynforward(_tmp432,sizeof(struct Cyc_PP_Doc*),4));});
else{s=({struct Cyc_PP_Doc*_tmp435[9];_tmp435[8]=Cyc_PP_text(({const char*_tmp438=";";
_tag_dynforward(_tmp438,sizeof(char),_get_zero_arr_size_char(_tmp438,2));}));
_tmp435[7]=Cyc_Absynpp_rb();_tmp435[6]=Cyc_PP_line_doc();_tmp435[5]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp437[2];_tmp437[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)
_tmp402->v);_tmp437[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp437,
sizeof(struct Cyc_PP_Doc*),2));}));_tmp435[4]=Cyc_Absynpp_lb();_tmp435[3]=Cyc_PP_blank_doc();
_tmp435[2]=Cyc_Absynpp_qvar2bolddoc(_tmp401);_tmp435[1]=Cyc_PP_text(({const char*
_tmp436="enum ";_tag_dynforward(_tmp436,sizeof(char),_get_zero_arr_size_char(
_tmp436,6));}));_tmp435[0]=Cyc_Absynpp_scope2doc(_tmp400);Cyc_PP_cat(
_tag_dynforward(_tmp435,sizeof(struct Cyc_PP_Doc*),9));});}goto _LL2C3;_LL2D2: if(*((
int*)_tmp3EF)!= 7)goto _LL2D4;_tmp403=((struct Cyc_Absyn_Typedef_d_struct*)_tmp3EF)->f1;
_LL2D3: {void*t;if(_tmp403->defn != 0)t=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp403->defn))->v;else{t=Cyc_Absyn_new_evar(_tmp403->kind,0);}s=({struct Cyc_PP_Doc*
_tmp439[4];_tmp439[3]=Cyc_PP_text(({const char*_tmp43D=";";_tag_dynforward(
_tmp43D,sizeof(char),_get_zero_arr_size_char(_tmp43D,2));}));_tmp439[2]=Cyc_Absynpp_atts2doc(
_tmp403->atts);_tmp439[1]=Cyc_Absynpp_tqtd2doc(_tmp403->tq,t,({struct Cyc_Core_Opt*
_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B->v=({struct Cyc_PP_Doc*_tmp43C[2];
_tmp43C[1]=Cyc_Absynpp_tvars2doc(_tmp403->tvs);_tmp43C[0]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp403->name);Cyc_PP_cat(_tag_dynforward(_tmp43C,sizeof(struct Cyc_PP_Doc*),2));});
_tmp43B;}));_tmp439[0]=Cyc_PP_text(({const char*_tmp43A="typedef ";
_tag_dynforward(_tmp43A,sizeof(char),_get_zero_arr_size_char(_tmp43A,9));}));Cyc_PP_cat(
_tag_dynforward(_tmp439,sizeof(struct Cyc_PP_Doc*),4));});goto _LL2C3;}_LL2D4: if(*((
int*)_tmp3EF)!= 8)goto _LL2D6;_tmp404=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp3EF)->f1;_tmp405=((struct Cyc_Absyn_Namespace_d_struct*)_tmp3EF)->f2;_LL2D5:
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp404);s=({
struct Cyc_PP_Doc*_tmp43E[8];_tmp43E[7]=Cyc_Absynpp_rb();_tmp43E[6]=Cyc_PP_line_doc();
_tmp43E[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp440="";_tag_dynforward(_tmp440,sizeof(char),
_get_zero_arr_size_char(_tmp440,1));}),_tmp405);_tmp43E[4]=Cyc_PP_line_doc();
_tmp43E[3]=Cyc_Absynpp_lb();_tmp43E[2]=Cyc_PP_blank_doc();_tmp43E[1]=Cyc_PP_textptr(
_tmp404);_tmp43E[0]=Cyc_PP_text(({const char*_tmp43F="namespace ";_tag_dynforward(
_tmp43F,sizeof(char),_get_zero_arr_size_char(_tmp43F,11));}));Cyc_PP_cat(
_tag_dynforward(_tmp43E,sizeof(struct Cyc_PP_Doc*),8));});if(Cyc_Absynpp_use_curr_namespace)
Cyc_Absynpp_curr_namespace_drop();goto _LL2C3;_LL2D6: if(*((int*)_tmp3EF)!= 9)goto
_LL2D8;_tmp406=((struct Cyc_Absyn_Using_d_struct*)_tmp3EF)->f1;_tmp407=((struct
Cyc_Absyn_Using_d_struct*)_tmp3EF)->f2;_LL2D7: if(Cyc_Absynpp_print_using_stmts)s=({
struct Cyc_PP_Doc*_tmp441[8];_tmp441[7]=Cyc_Absynpp_rb();_tmp441[6]=Cyc_PP_line_doc();
_tmp441[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp443="";_tag_dynforward(_tmp443,sizeof(char),
_get_zero_arr_size_char(_tmp443,1));}),_tmp407);_tmp441[4]=Cyc_PP_line_doc();
_tmp441[3]=Cyc_Absynpp_lb();_tmp441[2]=Cyc_PP_blank_doc();_tmp441[1]=Cyc_Absynpp_qvar2doc(
_tmp406);_tmp441[0]=Cyc_PP_text(({const char*_tmp442="using ";_tag_dynforward(
_tmp442,sizeof(char),_get_zero_arr_size_char(_tmp442,7));}));Cyc_PP_cat(
_tag_dynforward(_tmp441,sizeof(struct Cyc_PP_Doc*),8));});else{s=({struct Cyc_PP_Doc*
_tmp444[11];_tmp444[10]=Cyc_PP_text(({const char*_tmp449=" */";_tag_dynforward(
_tmp449,sizeof(char),_get_zero_arr_size_char(_tmp449,4));}));_tmp444[9]=Cyc_Absynpp_rb();
_tmp444[8]=Cyc_PP_text(({const char*_tmp448="/* ";_tag_dynforward(_tmp448,sizeof(
char),_get_zero_arr_size_char(_tmp448,4));}));_tmp444[7]=Cyc_PP_line_doc();
_tmp444[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp447="";_tag_dynforward(_tmp447,sizeof(char),
_get_zero_arr_size_char(_tmp447,1));}),_tmp407);_tmp444[5]=Cyc_PP_line_doc();
_tmp444[4]=Cyc_PP_text(({const char*_tmp446=" */";_tag_dynforward(_tmp446,sizeof(
char),_get_zero_arr_size_char(_tmp446,4));}));_tmp444[3]=Cyc_Absynpp_lb();
_tmp444[2]=Cyc_PP_blank_doc();_tmp444[1]=Cyc_Absynpp_qvar2doc(_tmp406);_tmp444[0]=
Cyc_PP_text(({const char*_tmp445="/* using ";_tag_dynforward(_tmp445,sizeof(char),
_get_zero_arr_size_char(_tmp445,10));}));Cyc_PP_cat(_tag_dynforward(_tmp444,
sizeof(struct Cyc_PP_Doc*),11));});}goto _LL2C3;_LL2D8: if(*((int*)_tmp3EF)!= 10)
goto _LL2DA;_tmp408=((struct Cyc_Absyn_ExternC_d_struct*)_tmp3EF)->f1;_LL2D9: if(
Cyc_Absynpp_print_externC_stmts)s=({struct Cyc_PP_Doc*_tmp44A[6];_tmp44A[5]=Cyc_Absynpp_rb();
_tmp44A[4]=Cyc_PP_line_doc();_tmp44A[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp44C="";_tag_dynforward(_tmp44C,sizeof(char),
_get_zero_arr_size_char(_tmp44C,1));}),_tmp408);_tmp44A[2]=Cyc_PP_line_doc();
_tmp44A[1]=Cyc_Absynpp_lb();_tmp44A[0]=Cyc_PP_text(({const char*_tmp44B="extern \"C\" ";
_tag_dynforward(_tmp44B,sizeof(char),_get_zero_arr_size_char(_tmp44B,12));}));
Cyc_PP_cat(_tag_dynforward(_tmp44A,sizeof(struct Cyc_PP_Doc*),6));});else{s=({
struct Cyc_PP_Doc*_tmp44D[9];_tmp44D[8]=Cyc_PP_text(({const char*_tmp452=" */";
_tag_dynforward(_tmp452,sizeof(char),_get_zero_arr_size_char(_tmp452,4));}));
_tmp44D[7]=Cyc_Absynpp_rb();_tmp44D[6]=Cyc_PP_text(({const char*_tmp451="/* ";
_tag_dynforward(_tmp451,sizeof(char),_get_zero_arr_size_char(_tmp451,4));}));
_tmp44D[5]=Cyc_PP_line_doc();_tmp44D[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp450="";_tag_dynforward(_tmp450,sizeof(char),
_get_zero_arr_size_char(_tmp450,1));}),_tmp408);_tmp44D[3]=Cyc_PP_line_doc();
_tmp44D[2]=Cyc_PP_text(({const char*_tmp44F=" */";_tag_dynforward(_tmp44F,sizeof(
char),_get_zero_arr_size_char(_tmp44F,4));}));_tmp44D[1]=Cyc_Absynpp_lb();
_tmp44D[0]=Cyc_PP_text(({const char*_tmp44E="/* extern \"C\" ";_tag_dynforward(
_tmp44E,sizeof(char),_get_zero_arr_size_char(_tmp44E,15));}));Cyc_PP_cat(
_tag_dynforward(_tmp44D,sizeof(struct Cyc_PP_Doc*),9));});}goto _LL2C3;_LL2DA: if(*((
int*)_tmp3EF)!= 11)goto _LL2DC;_tmp409=((struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp3EF)->f1;_tmp40A=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp3EF)->f2;
_LL2DB: if(Cyc_Absynpp_print_externC_stmts){struct Cyc_PP_Doc*exs_doc;if(_tmp40A != 
0)exs_doc=({struct Cyc_PP_Doc*_tmp453[7];_tmp453[6]=Cyc_Absynpp_rb();_tmp453[5]=
Cyc_PP_line_doc();_tmp453[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct _tuple11*),struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_export2doc,({const char*_tmp455=",";_tag_dynforward(_tmp455,sizeof(
char),_get_zero_arr_size_char(_tmp455,2));}),_tmp40A);_tmp453[3]=Cyc_PP_line_doc();
_tmp453[2]=Cyc_Absynpp_lb();_tmp453[1]=Cyc_PP_text(({const char*_tmp454=" export ";
_tag_dynforward(_tmp454,sizeof(char),_get_zero_arr_size_char(_tmp454,9));}));
_tmp453[0]=Cyc_Absynpp_rb();Cyc_PP_cat(_tag_dynforward(_tmp453,sizeof(struct Cyc_PP_Doc*),
7));});else{exs_doc=Cyc_Absynpp_rb();}s=({struct Cyc_PP_Doc*_tmp456[6];_tmp456[5]=
exs_doc;_tmp456[4]=Cyc_PP_line_doc();_tmp456[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp458="";_tag_dynforward(_tmp458,sizeof(char),
_get_zero_arr_size_char(_tmp458,1));}),_tmp409);_tmp456[2]=Cyc_PP_line_doc();
_tmp456[1]=Cyc_Absynpp_lb();_tmp456[0]=Cyc_PP_text(({const char*_tmp457="extern \"C include\" ";
_tag_dynforward(_tmp457,sizeof(char),_get_zero_arr_size_char(_tmp457,20));}));
Cyc_PP_cat(_tag_dynforward(_tmp456,sizeof(struct Cyc_PP_Doc*),6));});}else{s=({
struct Cyc_PP_Doc*_tmp459[9];_tmp459[8]=Cyc_PP_text(({const char*_tmp45E=" */";
_tag_dynforward(_tmp45E,sizeof(char),_get_zero_arr_size_char(_tmp45E,4));}));
_tmp459[7]=Cyc_Absynpp_rb();_tmp459[6]=Cyc_PP_text(({const char*_tmp45D="/* ";
_tag_dynforward(_tmp45D,sizeof(char),_get_zero_arr_size_char(_tmp45D,4));}));
_tmp459[5]=Cyc_PP_line_doc();_tmp459[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp45C="";_tag_dynforward(_tmp45C,sizeof(char),
_get_zero_arr_size_char(_tmp45C,1));}),_tmp409);_tmp459[3]=Cyc_PP_line_doc();
_tmp459[2]=Cyc_PP_text(({const char*_tmp45B=" */";_tag_dynforward(_tmp45B,sizeof(
char),_get_zero_arr_size_char(_tmp45B,4));}));_tmp459[1]=Cyc_Absynpp_lb();
_tmp459[0]=Cyc_PP_text(({const char*_tmp45A="/* extern \"C include\" ";
_tag_dynforward(_tmp45A,sizeof(char),_get_zero_arr_size_char(_tmp45A,23));}));
Cyc_PP_cat(_tag_dynforward(_tmp459,sizeof(struct Cyc_PP_Doc*),9));});}goto _LL2C3;
_LL2DC: if((int)_tmp3EF != 0)goto _LL2DE;_LL2DD: s=({struct Cyc_PP_Doc*_tmp45F[2];
_tmp45F[1]=Cyc_Absynpp_lb();_tmp45F[0]=Cyc_PP_text(({const char*_tmp460="__cyclone_port_on__;";
_tag_dynforward(_tmp460,sizeof(char),_get_zero_arr_size_char(_tmp460,21));}));
Cyc_PP_cat(_tag_dynforward(_tmp45F,sizeof(struct Cyc_PP_Doc*),2));});goto _LL2C3;
_LL2DE: if((int)_tmp3EF != 1)goto _LL2C3;_LL2DF: s=({struct Cyc_PP_Doc*_tmp461[2];
_tmp461[1]=Cyc_Absynpp_lb();_tmp461[0]=Cyc_PP_text(({const char*_tmp462="__cyclone_port_off__;";
_tag_dynforward(_tmp462,sizeof(char),_get_zero_arr_size_char(_tmp462,22));}));
Cyc_PP_cat(_tag_dynforward(_tmp461,sizeof(struct Cyc_PP_Doc*),2));});goto _LL2C3;
_LL2C3:;}return s;}int Cyc_Absynpp_print_scopes=1;struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
void*sc){if(!Cyc_Absynpp_print_scopes)return Cyc_PP_nil_doc();{void*_tmp463=sc;
_LL2F0: if((int)_tmp463 != 0)goto _LL2F2;_LL2F1: return Cyc_PP_text(({const char*
_tmp464="static ";_tag_dynforward(_tmp464,sizeof(char),_get_zero_arr_size_char(
_tmp464,8));}));_LL2F2: if((int)_tmp463 != 2)goto _LL2F4;_LL2F3: return Cyc_PP_nil_doc();
_LL2F4: if((int)_tmp463 != 3)goto _LL2F6;_LL2F5: return Cyc_PP_text(({const char*
_tmp465="extern ";_tag_dynforward(_tmp465,sizeof(char),_get_zero_arr_size_char(
_tmp465,8));}));_LL2F6: if((int)_tmp463 != 4)goto _LL2F8;_LL2F7: return Cyc_PP_text(({
const char*_tmp466="extern \"C\" ";_tag_dynforward(_tmp466,sizeof(char),
_get_zero_arr_size_char(_tmp466,12));}));_LL2F8: if((int)_tmp463 != 1)goto _LL2FA;
_LL2F9: return Cyc_PP_text(({const char*_tmp467="abstract ";_tag_dynforward(_tmp467,
sizeof(char),_get_zero_arr_size_char(_tmp467,10));}));_LL2FA: if((int)_tmp463 != 5)
goto _LL2EF;_LL2FB: return Cyc_PP_text(({const char*_tmp468="register ";
_tag_dynforward(_tmp468,sizeof(char),_get_zero_arr_size_char(_tmp468,10));}));
_LL2EF:;}}int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){void*_tmp469=t;
struct Cyc_Absyn_Tvar*_tmp46A;struct Cyc_List_List*_tmp46B;_LL2FD: if(_tmp469 <= (
void*)4)goto _LL301;if(*((int*)_tmp469)!= 1)goto _LL2FF;_tmp46A=((struct Cyc_Absyn_VarType_struct*)
_tmp469)->f1;_LL2FE: return Cyc_Tcutil_is_temp_tvar(_tmp46A);_LL2FF: if(*((int*)
_tmp469)!= 21)goto _LL301;_tmp46B=((struct Cyc_Absyn_JoinEff_struct*)_tmp469)->f1;
_LL300: return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp46B);
_LL301:;_LL302: return 0;_LL2FC:;}int Cyc_Absynpp_is_anon_aggrtype(void*t){void*
_tmp46C=t;void**_tmp46D;void*_tmp46E;_LL304: if(_tmp46C <= (void*)4)goto _LL30A;if(*((
int*)_tmp46C)!= 11)goto _LL306;_LL305: return 1;_LL306: if(*((int*)_tmp46C)!= 13)
goto _LL308;_LL307: return 1;_LL308: if(*((int*)_tmp46C)!= 17)goto _LL30A;_tmp46D=((
struct Cyc_Absyn_TypedefType_struct*)_tmp46C)->f4;if(_tmp46D == 0)goto _LL30A;
_tmp46E=*_tmp46D;_LL309: return Cyc_Absynpp_is_anon_aggrtype(_tmp46E);_LL30A:;
_LL30B: return 0;_LL303:;}static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(
struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){if(tms != 0  && tms->tl != 
0){struct _tuple5 _tmp470=({struct _tuple5 _tmp46F;_tmp46F.f1=(void*)tms->hd;_tmp46F.f2=(
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp46F;});void*_tmp471;
void*_tmp472;_LL30D: _tmp471=_tmp470.f1;if(*((int*)_tmp471)!= 2)goto _LL30F;
_tmp472=_tmp470.f2;if(*((int*)_tmp472)!= 3)goto _LL30F;_LL30E: return({struct Cyc_List_List*
_tmp473=_region_malloc(r,sizeof(*_tmp473));_tmp473->hd=(void*)((void*)tms->hd);
_tmp473->tl=({struct Cyc_List_List*_tmp474=_region_malloc(r,sizeof(*_tmp474));
_tmp474->hd=(void*)((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd);
_tmp474->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)
_check_null(tms->tl))->tl);_tmp474;});_tmp473;});_LL30F:;_LL310: return({struct
Cyc_List_List*_tmp475=_region_malloc(r,sizeof(*_tmp475));_tmp475->hd=(void*)atts;
_tmp475->tl=tms;_tmp475;});_LL30C:;}else{return({struct Cyc_List_List*_tmp476=
_region_malloc(r,sizeof(*_tmp476));_tmp476->hd=(void*)atts;_tmp476->tl=tms;
_tmp476;});}}struct _tuple7 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t){void*_tmp477=t;struct Cyc_Absyn_ArrayInfo _tmp478;void*_tmp479;struct
Cyc_Absyn_Tqual _tmp47A;struct Cyc_Absyn_Exp*_tmp47B;struct Cyc_Absyn_Conref*
_tmp47C;struct Cyc_Position_Segment*_tmp47D;struct Cyc_Absyn_PtrInfo _tmp47E;void*
_tmp47F;struct Cyc_Absyn_Tqual _tmp480;struct Cyc_Absyn_PtrAtts _tmp481;struct Cyc_Absyn_FnInfo
_tmp482;struct Cyc_List_List*_tmp483;struct Cyc_Core_Opt*_tmp484;void*_tmp485;
struct Cyc_List_List*_tmp486;int _tmp487;struct Cyc_Absyn_VarargInfo*_tmp488;struct
Cyc_List_List*_tmp489;struct Cyc_List_List*_tmp48A;struct Cyc_Core_Opt*_tmp48B;
struct Cyc_Core_Opt*_tmp48C;int _tmp48D;struct _tuple0*_tmp48E;struct Cyc_List_List*
_tmp48F;void**_tmp490;_LL312: if(_tmp477 <= (void*)4)goto _LL31C;if(*((int*)_tmp477)
!= 7)goto _LL314;_tmp478=((struct Cyc_Absyn_ArrayType_struct*)_tmp477)->f1;_tmp479=(
void*)_tmp478.elt_type;_tmp47A=_tmp478.tq;_tmp47B=_tmp478.num_elts;_tmp47C=
_tmp478.zero_term;_tmp47D=_tmp478.zt_loc;_LL313: {struct Cyc_Absyn_Tqual _tmp492;
void*_tmp493;struct Cyc_List_List*_tmp494;struct _tuple7 _tmp491=Cyc_Absynpp_to_tms(
r,_tmp47A,_tmp479);_tmp492=_tmp491.f1;_tmp493=_tmp491.f2;_tmp494=_tmp491.f3;{
void*tm;if(_tmp47B == 0)tm=(void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp495=
_region_malloc(r,sizeof(*_tmp495));_tmp495[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp496;_tmp496.tag=0;_tmp496.f1=_tmp47C;_tmp496.f2=_tmp47D;_tmp496;});_tmp495;});
else{tm=(void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp497=_region_malloc(r,
sizeof(*_tmp497));_tmp497[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp498;
_tmp498.tag=1;_tmp498.f1=(struct Cyc_Absyn_Exp*)_tmp47B;_tmp498.f2=_tmp47C;
_tmp498.f3=_tmp47D;_tmp498;});_tmp497;});}return({struct _tuple7 _tmp499;_tmp499.f1=
_tmp492;_tmp499.f2=_tmp493;_tmp499.f3=({struct Cyc_List_List*_tmp49A=
_region_malloc(r,sizeof(*_tmp49A));_tmp49A->hd=(void*)tm;_tmp49A->tl=_tmp494;
_tmp49A;});_tmp499;});}}_LL314: if(*((int*)_tmp477)!= 4)goto _LL316;_tmp47E=((
struct Cyc_Absyn_PointerType_struct*)_tmp477)->f1;_tmp47F=(void*)_tmp47E.elt_typ;
_tmp480=_tmp47E.elt_tq;_tmp481=_tmp47E.ptr_atts;_LL315: {struct Cyc_Absyn_Tqual
_tmp49C;void*_tmp49D;struct Cyc_List_List*_tmp49E;struct _tuple7 _tmp49B=Cyc_Absynpp_to_tms(
r,_tmp480,_tmp47F);_tmp49C=_tmp49B.f1;_tmp49D=_tmp49B.f2;_tmp49E=_tmp49B.f3;
_tmp49E=({struct Cyc_List_List*_tmp49F=_region_malloc(r,sizeof(*_tmp49F));_tmp49F->hd=(
void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp4A0=_region_malloc(r,
sizeof(*_tmp4A0));_tmp4A0[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp4A1;_tmp4A1.tag=
2;_tmp4A1.f1=_tmp481;_tmp4A1.f2=tq;_tmp4A1;});_tmp4A0;}));_tmp49F->tl=_tmp49E;
_tmp49F;});return({struct _tuple7 _tmp4A2;_tmp4A2.f1=_tmp49C;_tmp4A2.f2=_tmp49D;
_tmp4A2.f3=_tmp49E;_tmp4A2;});}_LL316: if(*((int*)_tmp477)!= 8)goto _LL318;_tmp482=((
struct Cyc_Absyn_FnType_struct*)_tmp477)->f1;_tmp483=_tmp482.tvars;_tmp484=
_tmp482.effect;_tmp485=(void*)_tmp482.ret_typ;_tmp486=_tmp482.args;_tmp487=
_tmp482.c_varargs;_tmp488=_tmp482.cyc_varargs;_tmp489=_tmp482.rgn_po;_tmp48A=
_tmp482.attributes;_LL317: if(!Cyc_Absynpp_print_all_tvars){if(_tmp484 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((
void*)_tmp484->v)){_tmp484=0;_tmp483=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar,_tmp483);}{struct Cyc_Absyn_Tqual _tmp4A4;void*
_tmp4A5;struct Cyc_List_List*_tmp4A6;struct _tuple7 _tmp4A3=Cyc_Absynpp_to_tms(r,
Cyc_Absyn_empty_tqual(0),_tmp485);_tmp4A4=_tmp4A3.f1;_tmp4A5=_tmp4A3.f2;_tmp4A6=
_tmp4A3.f3;{struct Cyc_List_List*tms=_tmp4A6;{void*_tmp4A7=Cyc_Cyclone_c_compiler;
_LL31F: if((int)_tmp4A7 != 0)goto _LL321;_LL320: if(_tmp48A != 0)tms=Cyc_Absynpp_bubble_attributes(
r,(void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp4A8=_region_malloc(r,
sizeof(*_tmp4A8));_tmp4A8[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp4A9;
_tmp4A9.tag=5;_tmp4A9.f1=0;_tmp4A9.f2=_tmp48A;_tmp4A9;});_tmp4A8;}),tms);tms=({
struct Cyc_List_List*_tmp4AA=_region_malloc(r,sizeof(*_tmp4AA));_tmp4AA->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp4AB=_region_malloc(r,sizeof(*
_tmp4AB));_tmp4AB[0]=({struct Cyc_Absyn_Function_mod_struct _tmp4AC;_tmp4AC.tag=3;
_tmp4AC.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp4AD=
_region_malloc(r,sizeof(*_tmp4AD));_tmp4AD[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp4AE;_tmp4AE.tag=1;_tmp4AE.f1=_tmp486;_tmp4AE.f2=_tmp487;_tmp4AE.f3=_tmp488;
_tmp4AE.f4=_tmp484;_tmp4AE.f5=_tmp489;_tmp4AE;});_tmp4AD;}));_tmp4AC;});_tmp4AB;}));
_tmp4AA->tl=tms;_tmp4AA;});goto _LL31E;_LL321: if((int)_tmp4A7 != 1)goto _LL31E;
_LL322: tms=({struct Cyc_List_List*_tmp4AF=_region_malloc(r,sizeof(*_tmp4AF));
_tmp4AF->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp4B0=
_region_malloc(r,sizeof(*_tmp4B0));_tmp4B0[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp4B1;_tmp4B1.tag=3;_tmp4B1.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp4B2=_region_malloc(r,sizeof(*_tmp4B2));_tmp4B2[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp4B3;_tmp4B3.tag=1;_tmp4B3.f1=_tmp486;_tmp4B3.f2=_tmp487;_tmp4B3.f3=_tmp488;
_tmp4B3.f4=_tmp484;_tmp4B3.f5=_tmp489;_tmp4B3;});_tmp4B2;}));_tmp4B1;});_tmp4B0;}));
_tmp4AF->tl=tms;_tmp4AF;});for(0;_tmp48A != 0;_tmp48A=_tmp48A->tl){void*_tmp4B4=(
void*)_tmp48A->hd;_LL324: if((int)_tmp4B4 != 0)goto _LL326;_LL325: goto _LL327;_LL326:
if((int)_tmp4B4 != 1)goto _LL328;_LL327: goto _LL329;_LL328: if((int)_tmp4B4 != 2)goto
_LL32A;_LL329: tms=({struct Cyc_List_List*_tmp4B5=_region_malloc(r,sizeof(*_tmp4B5));
_tmp4B5->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp4B6=
_region_malloc(r,sizeof(*_tmp4B6));_tmp4B6[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp4B7;_tmp4B7.tag=5;_tmp4B7.f1=0;_tmp4B7.f2=({struct Cyc_List_List*_tmp4B8=
_cycalloc(sizeof(*_tmp4B8));_tmp4B8->hd=(void*)((void*)_tmp48A->hd);_tmp4B8->tl=
0;_tmp4B8;});_tmp4B7;});_tmp4B6;}));_tmp4B5->tl=tms;_tmp4B5;});goto AfterAtts;
_LL32A:;_LL32B: goto _LL323;_LL323:;}goto _LL31E;_LL31E:;}AfterAtts: if(_tmp483 != 0)
tms=({struct Cyc_List_List*_tmp4B9=_region_malloc(r,sizeof(*_tmp4B9));_tmp4B9->hd=(
void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp4BA=_region_malloc(r,
sizeof(*_tmp4BA));_tmp4BA[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp4BB;
_tmp4BB.tag=4;_tmp4BB.f1=_tmp483;_tmp4BB.f2=0;_tmp4BB.f3=1;_tmp4BB;});_tmp4BA;}));
_tmp4B9->tl=tms;_tmp4B9;});return({struct _tuple7 _tmp4BC;_tmp4BC.f1=_tmp4A4;
_tmp4BC.f2=_tmp4A5;_tmp4BC.f3=tms;_tmp4BC;});}}_LL318: if(*((int*)_tmp477)!= 0)
goto _LL31A;_tmp48B=((struct Cyc_Absyn_Evar_struct*)_tmp477)->f1;_tmp48C=((struct
Cyc_Absyn_Evar_struct*)_tmp477)->f2;_tmp48D=((struct Cyc_Absyn_Evar_struct*)
_tmp477)->f3;_LL319: if(_tmp48C == 0)return({struct _tuple7 _tmp4BD;_tmp4BD.f1=tq;
_tmp4BD.f2=t;_tmp4BD.f3=0;_tmp4BD;});else{return Cyc_Absynpp_to_tms(r,tq,(void*)
_tmp48C->v);}_LL31A: if(*((int*)_tmp477)!= 17)goto _LL31C;_tmp48E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp477)->f1;_tmp48F=((struct Cyc_Absyn_TypedefType_struct*)_tmp477)->f2;_tmp490=((
struct Cyc_Absyn_TypedefType_struct*)_tmp477)->f4;_LL31B: if((_tmp490 == 0  || !Cyc_Absynpp_expand_typedefs)
 || Cyc_Absynpp_is_anon_aggrtype(*_tmp490))return({struct _tuple7 _tmp4BE;_tmp4BE.f1=
tq;_tmp4BE.f2=t;_tmp4BE.f3=0;_tmp4BE;});else{return Cyc_Absynpp_to_tms(r,tq,*
_tmp490);}_LL31C:;_LL31D: return({struct _tuple7 _tmp4BF;_tmp4BF.f1=tq;_tmp4BF.f2=t;
_tmp4BF.f3=0;_tmp4BF;});_LL311:;}static int Cyc_Absynpp_is_char_ptr(void*t){void*
_tmp4C0=t;struct Cyc_Core_Opt*_tmp4C1;struct Cyc_Core_Opt _tmp4C2;void*_tmp4C3;
struct Cyc_Absyn_PtrInfo _tmp4C4;void*_tmp4C5;_LL32D: if(_tmp4C0 <= (void*)4)goto
_LL331;if(*((int*)_tmp4C0)!= 0)goto _LL32F;_tmp4C1=((struct Cyc_Absyn_Evar_struct*)
_tmp4C0)->f2;if(_tmp4C1 == 0)goto _LL32F;_tmp4C2=*_tmp4C1;_tmp4C3=(void*)_tmp4C2.v;
_LL32E: return Cyc_Absynpp_is_char_ptr(_tmp4C3);_LL32F: if(*((int*)_tmp4C0)!= 4)
goto _LL331;_tmp4C4=((struct Cyc_Absyn_PointerType_struct*)_tmp4C0)->f1;_tmp4C5=(
void*)_tmp4C4.elt_typ;_LL330: L: {void*_tmp4C6=_tmp4C5;struct Cyc_Core_Opt*_tmp4C7;
struct Cyc_Core_Opt _tmp4C8;void*_tmp4C9;void**_tmp4CA;void*_tmp4CB;void*_tmp4CC;
_LL334: if(_tmp4C6 <= (void*)4)goto _LL33A;if(*((int*)_tmp4C6)!= 0)goto _LL336;
_tmp4C7=((struct Cyc_Absyn_Evar_struct*)_tmp4C6)->f2;if(_tmp4C7 == 0)goto _LL336;
_tmp4C8=*_tmp4C7;_tmp4C9=(void*)_tmp4C8.v;_LL335: _tmp4C5=_tmp4C9;goto L;_LL336:
if(*((int*)_tmp4C6)!= 17)goto _LL338;_tmp4CA=((struct Cyc_Absyn_TypedefType_struct*)
_tmp4C6)->f4;if(_tmp4CA == 0)goto _LL338;_tmp4CB=*_tmp4CA;_LL337: _tmp4C5=_tmp4CB;
goto L;_LL338: if(*((int*)_tmp4C6)!= 5)goto _LL33A;_tmp4CC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4C6)->f2;if((int)_tmp4CC != 0)goto _LL33A;_LL339: return 1;_LL33A:;_LL33B: return
0;_LL333:;}_LL331:;_LL332: return 0;_LL32C:;}struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){struct _RegionHandle
_tmp4CD=_new_region("temp");struct _RegionHandle*temp=& _tmp4CD;_push_region(temp);{
struct Cyc_Absyn_Tqual _tmp4CF;void*_tmp4D0;struct Cyc_List_List*_tmp4D1;struct
_tuple7 _tmp4CE=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp4CF=_tmp4CE.f1;_tmp4D0=
_tmp4CE.f2;_tmp4D1=_tmp4CE.f3;_tmp4D1=Cyc_List_imp_rev(_tmp4D1);if(_tmp4D1 == 0
 && dopt == 0){struct Cyc_PP_Doc*_tmp4D3=({struct Cyc_PP_Doc*_tmp4D2[2];_tmp4D2[1]=
Cyc_Absynpp_ntyp2doc(_tmp4D0);_tmp4D2[0]=Cyc_Absynpp_tqual2doc(_tmp4CF);Cyc_PP_cat(
_tag_dynforward(_tmp4D2,sizeof(struct Cyc_PP_Doc*),2));});_npop_handler(0);return
_tmp4D3;}else{struct Cyc_PP_Doc*_tmp4D6=({struct Cyc_PP_Doc*_tmp4D4[4];_tmp4D4[3]=
Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(
struct Cyc_PP_Doc*)dopt->v,_tmp4D1);_tmp4D4[2]=Cyc_PP_text(({const char*_tmp4D5=" ";
_tag_dynforward(_tmp4D5,sizeof(char),_get_zero_arr_size_char(_tmp4D5,2));}));
_tmp4D4[1]=Cyc_Absynpp_ntyp2doc(_tmp4D0);_tmp4D4[0]=Cyc_Absynpp_tqual2doc(
_tmp4CF);Cyc_PP_cat(_tag_dynforward(_tmp4D4,sizeof(struct Cyc_PP_Doc*),4));});
_npop_handler(0);return _tmp4D6;}};_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(
struct Cyc_Absyn_Aggrfield*f){void*_tmp4D7=Cyc_Cyclone_c_compiler;_LL33D: if((int)
_tmp4D7 != 0)goto _LL33F;_LL33E: if(f->width != 0)return({struct Cyc_PP_Doc*_tmp4D8[5];
_tmp4D8[4]=Cyc_PP_text(({const char*_tmp4DB=";";_tag_dynforward(_tmp4DB,sizeof(
char),_get_zero_arr_size_char(_tmp4DB,2));}));_tmp4D8[3]=Cyc_Absynpp_atts2doc(f->attributes);
_tmp4D8[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));
_tmp4D8[1]=Cyc_PP_text(({const char*_tmp4DA=":";_tag_dynforward(_tmp4DA,sizeof(
char),_get_zero_arr_size_char(_tmp4DA,2));}));_tmp4D8[0]=Cyc_Absynpp_tqtd2doc(f->tq,(
void*)f->type,({struct Cyc_Core_Opt*_tmp4D9=_cycalloc(sizeof(*_tmp4D9));_tmp4D9->v=
Cyc_PP_textptr(f->name);_tmp4D9;}));Cyc_PP_cat(_tag_dynforward(_tmp4D8,sizeof(
struct Cyc_PP_Doc*),5));});else{return({struct Cyc_PP_Doc*_tmp4DC[3];_tmp4DC[2]=
Cyc_PP_text(({const char*_tmp4DE=";";_tag_dynforward(_tmp4DE,sizeof(char),
_get_zero_arr_size_char(_tmp4DE,2));}));_tmp4DC[1]=Cyc_Absynpp_atts2doc(f->attributes);
_tmp4DC[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,({struct Cyc_Core_Opt*
_tmp4DD=_cycalloc(sizeof(*_tmp4DD));_tmp4DD->v=Cyc_PP_textptr(f->name);_tmp4DD;}));
Cyc_PP_cat(_tag_dynforward(_tmp4DC,sizeof(struct Cyc_PP_Doc*),3));});}_LL33F: if((
int)_tmp4D7 != 1)goto _LL33C;_LL340: if(f->width != 0)return({struct Cyc_PP_Doc*
_tmp4DF[5];_tmp4DF[4]=Cyc_PP_text(({const char*_tmp4E2=";";_tag_dynforward(
_tmp4E2,sizeof(char),_get_zero_arr_size_char(_tmp4E2,2));}));_tmp4DF[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp4DF[2]=Cyc_PP_text(({const char*
_tmp4E1=":";_tag_dynforward(_tmp4E1,sizeof(char),_get_zero_arr_size_char(_tmp4E1,
2));}));_tmp4DF[1]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,({struct Cyc_Core_Opt*
_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0->v=Cyc_PP_textptr(f->name);_tmp4E0;}));
_tmp4DF[0]=Cyc_Absynpp_atts2doc(f->attributes);Cyc_PP_cat(_tag_dynforward(
_tmp4DF,sizeof(struct Cyc_PP_Doc*),5));});else{return({struct Cyc_PP_Doc*_tmp4E3[3];
_tmp4E3[2]=Cyc_PP_text(({const char*_tmp4E5=";";_tag_dynforward(_tmp4E5,sizeof(
char),_get_zero_arr_size_char(_tmp4E5,2));}));_tmp4E3[1]=Cyc_Absynpp_tqtd2doc(f->tq,(
void*)f->type,({struct Cyc_Core_Opt*_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4->v=
Cyc_PP_textptr(f->name);_tmp4E4;}));_tmp4E3[0]=Cyc_Absynpp_atts2doc(f->attributes);
Cyc_PP_cat(_tag_dynforward(_tmp4E3,sizeof(struct Cyc_PP_Doc*),3));});}_LL33C:;}
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct
_dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,({
const char*_tmp4E6="";_tag_dynforward(_tmp4E6,sizeof(char),
_get_zero_arr_size_char(_tmp4E6,1));}),fields);}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfield2doc(
struct Cyc_Absyn_Tunionfield*f){return({struct Cyc_PP_Doc*_tmp4E7[3];_tmp4E7[2]=f->typs
== 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs);_tmp4E7[1]=Cyc_Absynpp_typedef_name2doc(
f->name);_tmp4E7[0]=Cyc_Absynpp_scope2doc((void*)f->sc);Cyc_PP_cat(
_tag_dynforward(_tmp4E7,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Tunionfield*),struct _dynforward_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_tunionfield2doc,({const char*_tmp4E8=",";
_tag_dynforward(_tmp4E8,sizeof(char),_get_zero_arr_size_char(_tmp4E8,2));}),
fields);}void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*
f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)
tdl->hd),72,f);({void*_tmp4E9=0;Cyc_fprintf(f,({const char*_tmp4EA="\n";
_tag_dynforward(_tmp4EA,sizeof(char),_get_zero_arr_size_char(_tmp4EA,2));}),
_tag_dynforward(_tmp4E9,sizeof(void*),0));});}}struct _dynforward_ptr Cyc_Absynpp_decllist2string(
struct Cyc_List_List*tdl){return Cyc_PP_string_of_doc(Cyc_PP_seql(({const char*
_tmp4EB="";_tag_dynforward(_tmp4EB,sizeof(char),_get_zero_arr_size_char(_tmp4EB,
1));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),72);}struct
_dynforward_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(
Cyc_Absynpp_exp2doc(e),72);}struct _dynforward_ptr Cyc_Absynpp_stmt2string(struct
Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s),72);}struct
_dynforward_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(
t),72);}struct _dynforward_ptr Cyc_Absynpp_typ2cstring(void*t){int old_qvar_to_Cids=
Cyc_Absynpp_qvar_to_Cids;int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;Cyc_Absynpp_qvar_to_Cids=
1;Cyc_Absynpp_add_cyc_prefix=0;{struct _dynforward_ptr s=Cyc_Absynpp_typ2string(t);
Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
old_add_cyc_prefix;return s;}}struct _dynforward_ptr Cyc_Absynpp_prim2string(void*p){
return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}struct _dynforward_ptr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dynforward_ptr Cyc_Absynpp_scope2string(void*sc){return Cyc_PP_string_of_doc(
Cyc_Absynpp_scope2doc(sc),72);}
