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
 void exit(int);void*abort();struct Cyc_Core_NewRegion{struct _DynRegionHandle*
dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[
16];struct _dynforward_ptr Cstring_to_string(char*);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dynforward_ptr);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern
char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
struct _dynforward_ptr Cyc_List_to_array(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);extern char Cyc_Arg_Bad[8];
struct Cyc_Arg_Bad_struct{char*tag;struct _dynforward_ptr f1;};extern char Cyc_Arg_Error[
10];struct Cyc_Arg_Unit_spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{
int tag;void(*f1)(struct _dynforward_ptr);};struct Cyc_Arg_FlagString_spec_struct{
int tag;void(*f1)(struct _dynforward_ptr,struct _dynforward_ptr);};struct Cyc_Arg_Set_spec_struct{
int tag;int*f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _dynforward_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;
void(*f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct
_dynforward_ptr);};extern int Cyc_Arg_current;void Cyc_Arg_parse(struct Cyc_List_List*
specs,void(*anonfun)(struct _dynforward_ptr),struct _dynforward_ptr errmsg,struct
_dynforward_ptr args);typedef struct{int __count;union{unsigned int __wch;char __wchb[
4];}__value;}Cyc___mbstate_t;typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;
typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_feof(struct Cyc___cycFILE*);
int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fgetc(struct Cyc___cycFILE*);struct Cyc___cycFILE*
Cyc_fopen(const char*,const char*);int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dynforward_ptr,struct _dynforward_ptr);unsigned int Cyc_fread(struct
_dynforward_ptr,unsigned int,unsigned int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dynforward_ptr f1;}
;struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
unsigned int Cyc_fwrite(struct _dynforward_ptr,unsigned int,unsigned int,struct Cyc___cycFILE*);
int Cyc_printf(struct _dynforward_ptr,struct _dynforward_ptr);int remove(const char*);
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc___cycFILE*Cyc_file_open(struct
_dynforward_ptr,struct _dynforward_ptr);void Cyc_file_close(struct Cyc___cycFILE*);
typedef struct{int quot;int rem;}Cyc_div_t;typedef struct{long quot;long rem;}Cyc_ldiv_t;
void*abort();void exit(int);char*getenv(const char*);int system(const char*);
unsigned int Cyc_strlen(struct _dynforward_ptr s);int Cyc_strcmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2);int Cyc_strptrcmp(struct
_dynforward_ptr*s1,struct _dynforward_ptr*s2);int Cyc_strncmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2,unsigned int len);struct _dynforward_ptr
Cyc_strconcat(struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr
Cyc_strconcat_l(struct Cyc_List_List*);struct _dynforward_ptr Cyc_str_sepstr(struct
Cyc_List_List*,struct _dynforward_ptr);struct _dynforward_ptr Cyc_strdup(struct
_dynforward_ptr src);struct _dynforward_ptr Cyc_substring(struct _dynforward_ptr,int
ofs,unsigned int n);struct _dynforward_ptr Cyc_strchr(struct _dynforward_ptr s,char c);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct _dynforward_ptr Cyc_Filename_concat(
struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr Cyc_Filename_chop_extension(
struct _dynforward_ptr);struct _dynforward_ptr Cyc_Filename_dirname(struct
_dynforward_ptr);struct _dynforward_ptr Cyc_Filename_basename(struct
_dynforward_ptr);int Cyc_Filename_check_suffix(struct _dynforward_ptr,struct
_dynforward_ptr);struct Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct
_dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];void Cyc_Position_reset_position(
struct _dynforward_ptr);struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dynforward_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];extern int Cyc_Position_use_gcc_style_location;
extern int Cyc_Position_max_errors;int Cyc_Position_error_p();int isspace(int);
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];extern int Cyc_Absyn_porting_c_code;
extern int Cyc_Absyn_no_regions;extern char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(
struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dynforward_ptr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dynforward_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dynforward_ptr lex_base;struct _dynforward_ptr lex_backtrk;struct
_dynforward_ptr lex_default;struct _dynforward_ptr lex_trans;struct _dynforward_ptr
lex_check;};struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
extern int Cyc_Parse_no_register;struct Cyc_Declaration_spec;struct Cyc_Declarator;
struct Cyc_Abstractdeclarator;struct _tuple4{void*f1;int f2;};struct Cyc_Int_tok_struct{
int tag;struct _tuple4 f1;};struct Cyc_Char_tok_struct{int tag;char f1;};struct Cyc_String_tok_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Stringopt_tok_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_QualId_tok_struct{int tag;struct _tuple1*f1;};struct _tuple5{struct
Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Absyn_Conref*f3;};
struct Cyc_YY1_struct{int tag;struct _tuple5*f1;};struct Cyc_YY2_struct{int tag;
struct Cyc_Absyn_Conref*f1;};struct Cyc_YY3_struct{int tag;struct Cyc_Absyn_Exp*f1;}
;struct Cyc_YY4_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_YY5_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY6_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY7_struct{int tag;void*f1;};struct Cyc_YY8_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_YY9_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_YY10_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY11_struct{int tag;struct Cyc_Absyn_Pat*
f1;};struct _tuple6{struct Cyc_List_List*f1;int f2;};struct Cyc_YY12_struct{int tag;
struct _tuple6*f1;};struct Cyc_YY13_struct{int tag;struct Cyc_List_List*f1;};struct
_tuple7{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_YY14_struct{
int tag;struct _tuple7*f1;};struct Cyc_YY15_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY16_struct{int tag;struct _tuple6*f1;};struct Cyc_YY17_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_YY18_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY19_struct{int tag;struct Cyc_Declaration_spec*f1;};struct _tuple8{
struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_YY20_struct{int tag;
struct _tuple8*f1;};struct Cyc_YY21_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_YY22_struct{int tag;void*f1;};struct Cyc_YY23_struct{int tag;void*f1;};struct
Cyc_YY24_struct{int tag;void*f1;};struct Cyc_YY25_struct{int tag;struct Cyc_Absyn_Tqual
f1;};struct Cyc_YY26_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY27_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY28_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY29_struct{int tag;struct Cyc_Declarator*f1;};struct Cyc_YY30_struct{
int tag;struct Cyc_Abstractdeclarator*f1;};struct Cyc_YY31_struct{int tag;int f1;};
struct Cyc_YY32_struct{int tag;void*f1;};struct Cyc_YY33_struct{int tag;struct Cyc_Absyn_Tunionfield*
f1;};struct Cyc_YY34_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct
Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_YY35_struct{
int tag;struct _tuple9*f1;};struct Cyc_YY36_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY37_struct{int tag;struct _tuple2*f1;};struct Cyc_YY38_struct{int tag;
struct Cyc_List_List*f1;};struct _tuple10{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*
f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_YY39_struct{int tag;
struct _tuple10*f1;};struct Cyc_YY40_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_YY41_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY42_struct{int tag;
void*f1;};struct Cyc_YY43_struct{int tag;void*f1;};struct Cyc_YY44_struct{int tag;
void*f1;};struct Cyc_YY45_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY46_struct{
int tag;void*f1;};struct Cyc_YY47_struct{int tag;struct Cyc_Absyn_Enumfield*f1;};
struct Cyc_YY48_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY49_struct{int
tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY50_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY51_struct{int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_YY52_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YYINITIALSVAL_struct{int tag;};union Cyc_YYSTYPE_union{
struct Cyc_Int_tok_struct Int_tok;struct Cyc_Char_tok_struct Char_tok;struct Cyc_String_tok_struct
String_tok;struct Cyc_Stringopt_tok_struct Stringopt_tok;struct Cyc_QualId_tok_struct
QualId_tok;struct Cyc_YY1_struct YY1;struct Cyc_YY2_struct YY2;struct Cyc_YY3_struct
YY3;struct Cyc_YY4_struct YY4;struct Cyc_YY5_struct YY5;struct Cyc_YY6_struct YY6;
struct Cyc_YY7_struct YY7;struct Cyc_YY8_struct YY8;struct Cyc_YY9_struct YY9;struct
Cyc_YY10_struct YY10;struct Cyc_YY11_struct YY11;struct Cyc_YY12_struct YY12;struct
Cyc_YY13_struct YY13;struct Cyc_YY14_struct YY14;struct Cyc_YY15_struct YY15;struct
Cyc_YY16_struct YY16;struct Cyc_YY17_struct YY17;struct Cyc_YY18_struct YY18;struct
Cyc_YY19_struct YY19;struct Cyc_YY20_struct YY20;struct Cyc_YY21_struct YY21;struct
Cyc_YY22_struct YY22;struct Cyc_YY23_struct YY23;struct Cyc_YY24_struct YY24;struct
Cyc_YY25_struct YY25;struct Cyc_YY26_struct YY26;struct Cyc_YY27_struct YY27;struct
Cyc_YY28_struct YY28;struct Cyc_YY29_struct YY29;struct Cyc_YY30_struct YY30;struct
Cyc_YY31_struct YY31;struct Cyc_YY32_struct YY32;struct Cyc_YY33_struct YY33;struct
Cyc_YY34_struct YY34;struct Cyc_YY35_struct YY35;struct Cyc_YY36_struct YY36;struct
Cyc_YY37_struct YY37;struct Cyc_YY38_struct YY38;struct Cyc_YY39_struct YY39;struct
Cyc_YY40_struct YY40;struct Cyc_YY41_struct YY41;struct Cyc_YY42_struct YY42;struct
Cyc_YY43_struct YY43;struct Cyc_YY44_struct YY44;struct Cyc_YY45_struct YY45;struct
Cyc_YY46_struct YY46;struct Cyc_YY47_struct YY47;struct Cyc_YY48_struct YY48;struct
Cyc_YY49_struct YY49;struct Cyc_YY50_struct YY50;struct Cyc_YY51_struct YY51;struct
Cyc_YY52_struct YY52;struct Cyc_YYINITIALSVAL_struct YYINITIALSVAL;};struct Cyc_Yyltype{
int timestamp;int first_line;int first_column;int last_line;int last_column;};struct
Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int
expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params*fs);void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*
tdl,struct Cyc___cycFILE*f);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
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
le;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);void Cyc_Tcutil_flush_warnings();
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,
struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*);struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);
extern int Cyc_Tovc_elim_array_initializers;struct Cyc_List_List*Cyc_Tovc_tovc(
struct Cyc_List_List*decls);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
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
struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct
Cyc_CfFlowInfo_UnknownL_struct{int tag;};union Cyc_CfFlowInfo_AbsLVal_union{struct
Cyc_CfFlowInfo_PlaceL_struct PlaceL;struct Cyc_CfFlowInfo_UnknownL_struct UnknownL;
};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);struct Cyc_Interface_I;
struct Cyc_Interface_I*Cyc_Interface_empty();struct Cyc_Interface_I*Cyc_Interface_final();
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae);struct
_tuple11{struct _dynforward_ptr f1;struct _dynforward_ptr f2;};int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple11*info);struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*
la,struct Cyc_List_List*linfo);struct Cyc_Interface_I*Cyc_Interface_parse(struct
Cyc___cycFILE*);void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);extern int Cyc_Cyclone_tovc_r;
extern void*Cyc_Cyclone_c_compiler;void Cyc_Port_port(struct Cyc_List_List*);void
Cyc_Lex_lex_init(int use_cyclone_keywords);int Cyc_Toc_warn_bounds_checks;int Cyc_Toc_warn_all_null_deref;
static int Cyc_pp_r=0;int Cyc_noexpand_r=0;static int Cyc_noshake_r=0;static int Cyc_stop_after_cpp_r=
0;static int Cyc_parseonly_r=0;static int Cyc_tc_r=0;static int Cyc_cf_r=0;static int
Cyc_noprint_r=0;static int Cyc_ic_r=0;static int Cyc_toc_r=0;static int Cyc_stop_after_objectfile_r=
0;static int Cyc_stop_after_asmfile_r=0;static int Cyc_inline_functions_r=0;static
int Cyc_elim_se_r=0;static int Cyc_v_r=0;static int Cyc_save_temps_r=0;static int Cyc_save_c_r=
0;static int Cyc_nogc_r=0;static int Cyc_pa_r=0;static int Cyc_pg_r=0;static int Cyc_nocheck_r=
0;static int Cyc_add_cyc_namespace_r=1;static int Cyc_generate_line_directives_r=0;
static int Cyc_print_full_evars_r=0;static int Cyc_print_all_tvars_r=0;static int Cyc_print_all_kinds_r=
0;static int Cyc_print_all_effects_r=0;static struct Cyc_List_List*Cyc_ccargs=0;
static void Cyc_add_ccarg(struct _dynforward_ptr s){Cyc_ccargs=({struct Cyc_List_List*
_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->hd=({struct _dynforward_ptr*_tmp1=
_cycalloc(sizeof(*_tmp1));_tmp1[0]=s;_tmp1;});_tmp0->tl=Cyc_ccargs;_tmp0;});}
void Cyc_set_c_compiler(struct _dynforward_ptr s){if(Cyc_strcmp((struct
_dynforward_ptr)s,({const char*_tmp2="vc";_tag_dynforward(_tmp2,sizeof(char),
_get_zero_arr_size_char(_tmp2,3));}))== 0){Cyc_Cyclone_c_compiler=(void*)1;Cyc_add_ccarg(({
const char*_tmp3="-DVC_C";_tag_dynforward(_tmp3,sizeof(char),
_get_zero_arr_size_char(_tmp3,7));}));}else{if(Cyc_strcmp((struct _dynforward_ptr)
s,({const char*_tmp4="gcc";_tag_dynforward(_tmp4,sizeof(char),
_get_zero_arr_size_char(_tmp4,4));}))== 0){Cyc_Cyclone_c_compiler=(void*)0;Cyc_add_ccarg(({
const char*_tmp5="-DGCC_C";_tag_dynforward(_tmp5,sizeof(char),
_get_zero_arr_size_char(_tmp5,8));}));}}}void Cyc_set_port_c_code(){Cyc_Absyn_porting_c_code=
1;Cyc_Position_max_errors=65535;Cyc_save_c_r=1;Cyc_parseonly_r=1;}static struct
_dynforward_ptr*Cyc_output_file=0;static void Cyc_set_output_file(struct
_dynforward_ptr s){Cyc_output_file=({struct _dynforward_ptr*_tmp6=_cycalloc(
sizeof(*_tmp6));_tmp6[0]=s;_tmp6;});}extern char*Cdef_inc_path;extern char*
Cdef_lib_path;extern char*Carch;extern char*Ccomp;extern char*Cversion;static char
_tmp7[1]="";static struct _dynforward_ptr Cyc_cpp={_tmp7,_tmp7 + 1};static void Cyc_set_cpp(
struct _dynforward_ptr s){Cyc_cpp=s;}static struct Cyc_List_List*Cyc_cppargs=0;
static void Cyc_add_cpparg(struct _dynforward_ptr s){Cyc_cppargs=({struct Cyc_List_List*
_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8->hd=({struct _dynforward_ptr*_tmp9=
_cycalloc(sizeof(*_tmp9));_tmp9[0]=s;_tmp9;});_tmp8->tl=Cyc_cppargs;_tmp8;});}
static struct _dynforward_ptr Cyc_target_arch=(struct _dynforward_ptr){(void*)0,(
void*)(0 + 0)};static void Cyc_set_target_arch(struct _dynforward_ptr s){Cyc_toc_r=1;
Cyc_target_arch=s;}static void Cyc_print_version(){({struct Cyc_String_pa_struct
_tmpC;_tmpC.tag=0;_tmpC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cstring_to_string(Cversion));{void*_tmpA[1]={& _tmpC};Cyc_printf(({const char*
_tmpB="The Cyclone compiler, version %s\n";_tag_dynforward(_tmpB,sizeof(char),
_get_zero_arr_size_char(_tmpB,34));}),_tag_dynforward(_tmpA,sizeof(void*),1));}});({
struct Cyc_String_pa_struct _tmp10;_tmp10.tag=0;_tmp10.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cstring_to_string(Ccomp));{struct Cyc_String_pa_struct _tmpF;
_tmpF.tag=0;_tmpF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cstring_to_string(Carch));{void*_tmpD[2]={& _tmpF,& _tmp10};Cyc_printf(({const char*
_tmpE="Compiled for architecture: %s CC=\"%s\"\n";_tag_dynforward(_tmpE,sizeof(
char),_get_zero_arr_size_char(_tmpE,39));}),_tag_dynforward(_tmpD,sizeof(void*),
2));}}});({struct Cyc_String_pa_struct _tmp13;_tmp13.tag=0;_tmp13.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cstring_to_string(Cdef_lib_path));{void*
_tmp11[1]={& _tmp13};Cyc_printf(({const char*_tmp12="Standard library directory: %s\n";
_tag_dynforward(_tmp12,sizeof(char),_get_zero_arr_size_char(_tmp12,32));}),
_tag_dynforward(_tmp11,sizeof(void*),1));}});({struct Cyc_String_pa_struct _tmp16;
_tmp16.tag=0;_tmp16.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cstring_to_string(Cdef_inc_path));{void*_tmp14[1]={& _tmp16};Cyc_printf(({const
char*_tmp15="Standard include directory: %s\n";_tag_dynforward(_tmp15,sizeof(
char),_get_zero_arr_size_char(_tmp15,32));}),_tag_dynforward(_tmp14,sizeof(void*),
1));}});exit(0);}static int Cyc_is_cyclone_sourcefile(struct _dynforward_ptr s){
unsigned int _tmp17=Cyc_strlen((struct _dynforward_ptr)s);if(_tmp17 <= 4)return 0;
else{return Cyc_strcmp((struct _dynforward_ptr)_dynforward_ptr_plus(s,sizeof(char),(
int)(_tmp17 - 4)),({const char*_tmp18=".cyc";_tag_dynforward(_tmp18,sizeof(char),
_get_zero_arr_size_char(_tmp18,5));}))== 0;}}static struct Cyc_List_List*Cyc_cyclone_exec_path=
0;static void Cyc_add_cyclone_exec_path(struct _dynforward_ptr s){unsigned int _tmp19=
Cyc_strlen((struct _dynforward_ptr)s);if(_tmp19 <= 2)return;{struct _dynforward_ptr
_tmp1A=(struct _dynforward_ptr)Cyc_substring((struct _dynforward_ptr)s,2,_tmp19 - 2);
Cyc_cyclone_exec_path=({struct Cyc_List_List*_tmp1B=_cycalloc(sizeof(*_tmp1B));
_tmp1B->hd=({struct _dynforward_ptr*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C[0]=
_tmp1A;_tmp1C;});_tmp1B->tl=Cyc_cyclone_exec_path;_tmp1B;});}}static struct Cyc_List_List*
Cyc_libargs=0;static void Cyc_add_libarg(struct _dynforward_ptr s){if(Cyc_strcmp((
struct _dynforward_ptr)s,({const char*_tmp1D="-lxml";_tag_dynforward(_tmp1D,
sizeof(char),_get_zero_arr_size_char(_tmp1D,6));}))== 0)Cyc_add_ccarg(s);else{
Cyc_libargs=({struct Cyc_List_List*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->hd=({
struct _dynforward_ptr*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F[0]=s;_tmp1F;});
_tmp1E->tl=Cyc_libargs;_tmp1E;});}}static void Cyc_add_marg(struct _dynforward_ptr s){
Cyc_add_ccarg(s);}static void Cyc_set_save_temps(){Cyc_save_temps_r=1;Cyc_add_ccarg(({
const char*_tmp20="-save-temps";_tag_dynforward(_tmp20,sizeof(char),
_get_zero_arr_size_char(_tmp20,12));}));}static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies(){Cyc_stop_after_cpp_r=1;Cyc_produce_dependencies=
1;Cyc_add_cpparg(({const char*_tmp21="-M";_tag_dynforward(_tmp21,sizeof(char),
_get_zero_arr_size_char(_tmp21,3));}));}static struct _dynforward_ptr*Cyc_dependencies_target=
0;static void Cyc_set_dependencies_target(struct _dynforward_ptr s){Cyc_dependencies_target=({
struct _dynforward_ptr*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22[0]=s;_tmp22;});}
static void Cyc_set_stop_after_objectfile(){Cyc_stop_after_objectfile_r=1;Cyc_add_ccarg(({
const char*_tmp23="-c";_tag_dynforward(_tmp23,sizeof(char),
_get_zero_arr_size_char(_tmp23,3));}));}static void Cyc_set_nocppprecomp(){Cyc_add_cpparg(({
const char*_tmp24="-no-cpp-precomp";_tag_dynforward(_tmp24,sizeof(char),
_get_zero_arr_size_char(_tmp24,16));}));Cyc_add_ccarg(({const char*_tmp25="-no-cpp-precomp";
_tag_dynforward(_tmp25,sizeof(char),_get_zero_arr_size_char(_tmp25,16));}));}
static void Cyc_set_lineno(){Cyc_generate_line_directives_r=1;Cyc_set_save_temps();}
static void Cyc_set_inline_functions(){Cyc_inline_functions_r=1;}static void Cyc_set_elim_se(){
Cyc_elim_se_r=1;Cyc_set_inline_functions();}static void Cyc_set_tovc(){Cyc_Cyclone_tovc_r=
1;Cyc_add_ccarg(({const char*_tmp26="-DCYC_ANSI_OUTPUT";_tag_dynforward(_tmp26,
sizeof(char),_get_zero_arr_size_char(_tmp26,18));}));Cyc_set_elim_se();}static
void Cyc_set_noboundschecks(){Cyc_add_ccarg(({const char*_tmp27="-DNO_CYC_BOUNDS_CHECKS";
_tag_dynforward(_tmp27,sizeof(char),_get_zero_arr_size_char(_tmp27,23));}));}
static void Cyc_set_nonullchecks(){Cyc_add_ccarg(({const char*_tmp28="-DNO_CYC_NULL_CHECKS";
_tag_dynforward(_tmp28,sizeof(char),_get_zero_arr_size_char(_tmp28,21));}));}
static void Cyc_set_nochecks(){Cyc_set_noboundschecks();Cyc_set_nonullchecks();Cyc_nocheck_r=
1;}static void Cyc_set_nocyc(){Cyc_add_cyc_namespace_r=0;Cyc_add_ccarg(({const char*
_tmp29="-DNO_CYC_PREFIX";_tag_dynforward(_tmp29,sizeof(char),
_get_zero_arr_size_char(_tmp29,16));}));}static void Cyc_set_pa(){Cyc_pa_r=1;Cyc_add_ccarg(({
const char*_tmp2A="-DCYC_REGION_PROFILE";_tag_dynforward(_tmp2A,sizeof(char),
_get_zero_arr_size_char(_tmp2A,21));}));}static void Cyc_set_pg(){Cyc_pg_r=1;Cyc_add_ccarg(({
const char*_tmp2B="-pg";_tag_dynforward(_tmp2B,sizeof(char),
_get_zero_arr_size_char(_tmp2B,4));}));}static void Cyc_set_stop_after_asmfile(){
Cyc_stop_after_asmfile_r=1;Cyc_add_ccarg(({const char*_tmp2C="-S";_tag_dynforward(
_tmp2C,sizeof(char),_get_zero_arr_size_char(_tmp2C,3));}));}static void Cyc_set_all_warnings(){
Cyc_Toc_warn_bounds_checks=1;Cyc_Toc_warn_all_null_deref=1;}static void*Cyc_intype=(
void*)0;static void Cyc_set_inputtype(struct _dynforward_ptr s){if(Cyc_strcmp((
struct _dynforward_ptr)s,({const char*_tmp2D="cyc";_tag_dynforward(_tmp2D,sizeof(
char),_get_zero_arr_size_char(_tmp2D,4));}))== 0)Cyc_intype=(void*)1;else{if(Cyc_strcmp((
struct _dynforward_ptr)s,({const char*_tmp2E="none";_tag_dynforward(_tmp2E,sizeof(
char),_get_zero_arr_size_char(_tmp2E,5));}))== 0)Cyc_intype=(void*)0;else{({
struct Cyc_String_pa_struct _tmp31;_tmp31.tag=0;_tmp31.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)s);{void*_tmp2F[1]={& _tmp31};Cyc_fprintf(Cyc_stderr,({
const char*_tmp30="Input type '%s' not supported\n";_tag_dynforward(_tmp30,
sizeof(char),_get_zero_arr_size_char(_tmp30,31));}),_tag_dynforward(_tmp2F,
sizeof(void*),1));}});}}}struct _dynforward_ptr Cyc_make_base_filename(struct
_dynforward_ptr s,struct _dynforward_ptr*output_file){struct _dynforward_ptr _tmp32=
Cyc_Filename_dirname(output_file == 0?s:*output_file);struct _dynforward_ptr _tmp33=
Cyc_Filename_chop_extension((struct _dynforward_ptr)Cyc_Filename_basename(s));
struct _dynforward_ptr _tmp34=Cyc_strlen((struct _dynforward_ptr)_tmp32)> 0?Cyc_Filename_concat((
struct _dynforward_ptr)_tmp32,(struct _dynforward_ptr)_tmp33): _tmp33;return(struct
_dynforward_ptr)_tmp34;}static struct Cyc_List_List*Cyc_cyclone_files=0;static void
Cyc_add_other(struct _dynforward_ptr s){if(Cyc_intype == (void*)1  || Cyc_is_cyclone_sourcefile(
s)){Cyc_cyclone_files=({struct Cyc_List_List*_tmp35=_cycalloc(sizeof(*_tmp35));
_tmp35->hd=({struct _dynforward_ptr*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36[0]=s;
_tmp36;});_tmp35->tl=Cyc_cyclone_files;_tmp35;});{struct _dynforward_ptr _tmp37=
Cyc_make_base_filename(s,Cyc_output_file);struct _dynforward_ptr _tmp38=Cyc_strconcat((
struct _dynforward_ptr)_tmp37,({const char*_tmp39=".c";_tag_dynforward(_tmp39,
sizeof(char),_get_zero_arr_size_char(_tmp39,3));}));Cyc_add_ccarg((struct
_dynforward_ptr)_tmp38);}}else{Cyc_add_ccarg(s);}}static void Cyc_remove_file(
struct _dynforward_ptr s){if(Cyc_save_temps_r)return;else{remove((const char*)
_untag_dynforward_ptr(s,sizeof(char),1));}}struct _tuple12{struct _dynforward_ptr*
f1;struct _dynforward_ptr*f2;};struct Cyc_List_List*Cyc_read_specs(struct
_dynforward_ptr file){struct Cyc_List_List*_tmp3A=0;int c;int i;char strname[256];
char strvalue[4096];struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)
_untag_dynforward_ptr(file,sizeof(char),1),(const char*)"r");if(spec_file == 0){({
struct Cyc_String_pa_struct _tmp3D;_tmp3D.tag=0;_tmp3D.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)file);{void*_tmp3B[1]={& _tmp3D};Cyc_fprintf(Cyc_stderr,({
const char*_tmp3C="Error opening spec file %s\n";_tag_dynforward(_tmp3C,sizeof(
char),_get_zero_arr_size_char(_tmp3C,28));}),_tag_dynforward(_tmp3B,sizeof(void*),
1));}});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;}while(1){while(1){
c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if(isspace(c))continue;if(c == '*')
break;if(c != - 1){({struct Cyc_Int_pa_struct _tmp41;_tmp41.tag=1;_tmp41.f1=(
unsigned long)c;{struct Cyc_String_pa_struct _tmp40;_tmp40.tag=0;_tmp40.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)file);{void*_tmp3E[2]={& _tmp40,& _tmp41};
Cyc_fprintf(Cyc_stderr,({const char*_tmp3F="Error reading spec file %s: unexpected character '%c'\n";
_tag_dynforward(_tmp3F,sizeof(char),_get_zero_arr_size_char(_tmp3F,55));}),
_tag_dynforward(_tmp3E,sizeof(void*),2));}}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);}goto CLEANUP_AND_RETURN;}JUST_AFTER_STAR: i=0;while(1){c=Cyc_fgetc((
struct Cyc___cycFILE*)spec_file);if(c == - 1){({struct Cyc_String_pa_struct _tmp44;
_tmp44.tag=0;_tmp44.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)file);{
void*_tmp42[1]={& _tmp44};Cyc_fprintf(Cyc_stderr,({const char*_tmp43="Error reading spec file %s: unexpected EOF\n";
_tag_dynforward(_tmp43,sizeof(char),_get_zero_arr_size_char(_tmp43,44));}),
_tag_dynforward(_tmp42,sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);goto CLEANUP_AND_RETURN;}if(c == ':'){strname[
_check_known_subscript_notnull(256,i)]='\000';break;}strname[
_check_known_subscript_notnull(256,i)]=(char)c;i ++;if(i >= 256){({struct Cyc_String_pa_struct
_tmp47;_tmp47.tag=0;_tmp47.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
file);{void*_tmp45[1]={& _tmp47};Cyc_fprintf(Cyc_stderr,({const char*_tmp46="Error reading spec file %s: string name too long\n";
_tag_dynforward(_tmp46,sizeof(char),_get_zero_arr_size_char(_tmp46,50));}),
_tag_dynforward(_tmp45,sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);goto CLEANUP_AND_RETURN;}}while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)
spec_file);if(isspace(c))continue;break;}if(c == '*'){_tmp3A=({struct Cyc_List_List*
_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->hd=({struct _tuple12*_tmp49=_cycalloc(
sizeof(*_tmp49));_tmp49->f1=({struct _dynforward_ptr*_tmp4C=_cycalloc(sizeof(*
_tmp4C));_tmp4C[0]=(struct _dynforward_ptr)Cyc_strdup(_tag_dynforward(strname,
sizeof(char),256));_tmp4C;});_tmp49->f2=({struct _dynforward_ptr*_tmp4A=_cycalloc(
sizeof(*_tmp4A));_tmp4A[0]=(struct _dynforward_ptr)Cyc_strdup(({const char*_tmp4B="";
_tag_dynforward(_tmp4B,sizeof(char),_get_zero_arr_size_char(_tmp4B,1));}));
_tmp4A;});_tmp49;});_tmp48->tl=_tmp3A;_tmp48;});goto JUST_AFTER_STAR;}strvalue[0]=(
char)c;i=1;while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if((c == - 1  || c
== '\n') || c == '\r'){strvalue[_check_known_subscript_notnull(4096,i)]='\000';
break;}strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;i ++;if(i >= 4096){({
struct Cyc_String_pa_struct _tmp50;_tmp50.tag=0;_tmp50.f1=(struct _dynforward_ptr)
_tag_dynforward(strname,sizeof(char),256);{struct Cyc_String_pa_struct _tmp4F;
_tmp4F.tag=0;_tmp4F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)file);{
void*_tmp4D[2]={& _tmp4F,& _tmp50};Cyc_fprintf(Cyc_stderr,({const char*_tmp4E="Error reading spec file %s: value of %s too long\n";
_tag_dynforward(_tmp4E,sizeof(char),_get_zero_arr_size_char(_tmp4E,50));}),
_tag_dynforward(_tmp4D,sizeof(void*),2));}}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);goto CLEANUP_AND_RETURN;}}_tmp3A=({struct Cyc_List_List*_tmp51=
_cycalloc(sizeof(*_tmp51));_tmp51->hd=({struct _tuple12*_tmp52=_cycalloc(sizeof(*
_tmp52));_tmp52->f1=({struct _dynforward_ptr*_tmp54=_cycalloc(sizeof(*_tmp54));
_tmp54[0]=(struct _dynforward_ptr)Cyc_strdup(_tag_dynforward(strname,sizeof(char),
256));_tmp54;});_tmp52->f2=({struct _dynforward_ptr*_tmp53=_cycalloc(sizeof(*
_tmp53));_tmp53[0]=(struct _dynforward_ptr)Cyc_strdup(_tag_dynforward(strvalue,
sizeof(char),4096));_tmp53;});_tmp52;});_tmp51->tl=_tmp3A;_tmp51;});if(c == - 1)
goto CLEANUP_AND_RETURN;}CLEANUP_AND_RETURN: Cyc_fclose((struct Cyc___cycFILE*)
spec_file);return _tmp3A;}struct _dynforward_ptr Cyc_split_specs(struct
_dynforward_ptr cmdline){if(cmdline.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr)return _tag_dynforward(0,0,0);{unsigned int _tmp55=
Cyc_strlen((struct _dynforward_ptr)cmdline);struct Cyc_List_List*_tmp56=0;char buf[
4096];int i=0;int j=0;if(_tmp55 > 4096)goto DONE;while(1){while(1){if(i >= _tmp55)
goto DONE;if(*((const char*)_check_dynforward_subscript(cmdline,sizeof(char),i))== 
0)goto DONE;if(!isspace((int)*((const char*)_check_dynforward_subscript(cmdline,
sizeof(char),i))))break;i ++;}j=0;while(1){if(i >= _tmp55)break;if(*((const char*)
_check_dynforward_subscript(cmdline,sizeof(char),i))== 0)break;if(isspace((int)*((
const char*)_check_dynforward_subscript(cmdline,sizeof(char),i))))break;if(*((
const char*)_check_dynforward_subscript(cmdline,sizeof(char),i))== '\\'){i ++;if(i
>= _tmp55)break;if(*((const char*)_check_dynforward_subscript(cmdline,sizeof(char),
i))== 0)break;buf[_check_known_subscript_notnull(4096,j)]=*((const char*)
_check_dynforward_subscript(cmdline,sizeof(char),i));j ++;}else{buf[
_check_known_subscript_notnull(4096,j)]=*((const char*)
_check_dynforward_subscript(cmdline,sizeof(char),i));j ++;}i ++;}if(j < 4096)buf[
_check_known_subscript_notnull(4096,j)]='\000';_tmp56=({struct Cyc_List_List*
_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->hd=({struct _dynforward_ptr*_tmp58=
_cycalloc(sizeof(*_tmp58));_tmp58[0]=(struct _dynforward_ptr)Cyc_strdup(
_tag_dynforward(buf,sizeof(char),4096));_tmp58;});_tmp57->tl=_tmp56;_tmp57;});}
DONE: _tmp56=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp56);_tmp56=({struct Cyc_List_List*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->hd=
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"",sizeof(char),1);
_tmp59->tl=_tmp56;_tmp59;});{struct _dynforward_ptr _tmp5B=((struct _dynforward_ptr(*)(
struct Cyc_List_List*x))Cyc_List_to_array)(_tmp56);struct _dynforward_ptr _tmp5C=({
unsigned int _tmp5D=_get_dynforward_size(_tmp5B,sizeof(struct _dynforward_ptr*));
struct _dynforward_ptr*_tmp5E=(struct _dynforward_ptr*)_cycalloc(_check_times(
sizeof(struct _dynforward_ptr),_tmp5D));struct _dynforward_ptr _tmp60=
_tag_dynforward(_tmp5E,sizeof(struct _dynforward_ptr),_tmp5D);{unsigned int _tmp5F=
_tmp5D;unsigned int k;for(k=0;k < _tmp5F;k ++){_tmp5E[k]=*(*((struct _dynforward_ptr**)
_check_dynforward_subscript(_tmp5B,sizeof(struct _dynforward_ptr*),(int)k)));}}
_tmp60;});return _tmp5C;}}}int Cyc_compile_failure=0;struct Cyc___cycFILE*Cyc_try_file_open(
struct _dynforward_ptr filename,struct _dynforward_ptr mode,struct _dynforward_ptr
msg_part){struct _handler_cons _tmp61;_push_handler(& _tmp61);{int _tmp63=0;if(
setjmp(_tmp61.handler))_tmp63=1;if(!_tmp63){{struct Cyc___cycFILE*_tmp64=(struct
Cyc___cycFILE*)Cyc_file_open(filename,mode);_npop_handler(0);return _tmp64;};
_pop_handler();}else{void*_tmp62=(void*)_exn_thrown;void*_tmp66=_tmp62;_LL1:;
_LL2: Cyc_compile_failure=1;({struct Cyc_String_pa_struct _tmp6A;_tmp6A.tag=0;
_tmp6A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)filename);{struct Cyc_String_pa_struct
_tmp69;_tmp69.tag=0;_tmp69.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
msg_part);{void*_tmp67[2]={& _tmp69,& _tmp6A};Cyc_fprintf(Cyc_stderr,({const char*
_tmp68="\nError: couldn't open %s %s\n";_tag_dynforward(_tmp68,sizeof(char),
_get_zero_arr_size_char(_tmp68,29));}),_tag_dynforward(_tmp67,sizeof(void*),2));}}});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;_LL3:;_LL4:(void)_throw(
_tmp66);_LL0:;}}}struct Cyc_List_List*Cyc_do_stage(struct _dynforward_ptr
stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),void*env,struct
Cyc_List_List*tds){struct _dynforward_ptr exn_string=({const char*_tmp88="";
_tag_dynforward(_tmp88,sizeof(char),_get_zero_arr_size_char(_tmp88,1));});struct
_dynforward_ptr explain_string=({const char*_tmp87="";_tag_dynforward(_tmp87,
sizeof(char),_get_zero_arr_size_char(_tmp87,1));});int other_exn=0;void*ex=(void*)({
struct Cyc_Core_Impossible_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84[0]=({
struct Cyc_Core_Impossible_struct _tmp85;_tmp85.tag=Cyc_Core_Impossible;_tmp85.f1=({
const char*_tmp86="";_tag_dynforward(_tmp86,sizeof(char),_get_zero_arr_size_char(
_tmp86,1));});_tmp85;});_tmp84;});struct Cyc_List_List*_tmp6B=0;{struct
_handler_cons _tmp6C;_push_handler(& _tmp6C);{int _tmp6E=0;if(setjmp(_tmp6C.handler))
_tmp6E=1;if(!_tmp6E){_tmp6B=f(env,tds);;_pop_handler();}else{void*_tmp6D=(void*)
_exn_thrown;void*_tmp70=_tmp6D;struct _dynforward_ptr _tmp71;struct _dynforward_ptr
_tmp72;struct _dynforward_ptr _tmp73;_LL6: if(*((void**)_tmp70)!= Cyc_Core_Impossible)
goto _LL8;_tmp71=((struct Cyc_Core_Impossible_struct*)_tmp70)->f1;_LL7: exn_string=({
const char*_tmp74="Exception Core::Impossible";_tag_dynforward(_tmp74,sizeof(char),
_get_zero_arr_size_char(_tmp74,27));});explain_string=_tmp71;goto _LL5;_LL8: if(
_tmp70 != Cyc_Dict_Absent)goto _LLA;_LL9: exn_string=({const char*_tmp75="Exception Dict::Absent";
_tag_dynforward(_tmp75,sizeof(char),_get_zero_arr_size_char(_tmp75,23));});goto
_LL5;_LLA: if(*((void**)_tmp70)!= Cyc_Core_Invalid_argument)goto _LLC;_tmp72=((
struct Cyc_Core_Invalid_argument_struct*)_tmp70)->f1;_LLB: exn_string=({const char*
_tmp76="Exception Core::Invalid_argument";_tag_dynforward(_tmp76,sizeof(char),
_get_zero_arr_size_char(_tmp76,33));});explain_string=_tmp72;goto _LL5;_LLC: if(*((
void**)_tmp70)!= Cyc_Core_Failure)goto _LLE;_tmp73=((struct Cyc_Core_Failure_struct*)
_tmp70)->f1;_LLD: exn_string=({const char*_tmp77="Exception Core::Failure";
_tag_dynforward(_tmp77,sizeof(char),_get_zero_arr_size_char(_tmp77,24));});
explain_string=_tmp73;goto _LL5;_LLE:;_LLF: ex=_tmp70;other_exn=1;exn_string=({
const char*_tmp78="Uncaught exception";_tag_dynforward(_tmp78,sizeof(char),
_get_zero_arr_size_char(_tmp78,19));});goto _LL5;_LL10:;_LL11:(void)_throw(_tmp70);
_LL5:;}}}if(Cyc_Position_error_p())Cyc_compile_failure=1;if(Cyc_strcmp((struct
_dynforward_ptr)exn_string,({const char*_tmp79="";_tag_dynforward(_tmp79,sizeof(
char),_get_zero_arr_size_char(_tmp79,1));}))!= 0){Cyc_compile_failure=1;({struct
Cyc_String_pa_struct _tmp7E;_tmp7E.tag=0;_tmp7E.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)explain_string);{struct Cyc_String_pa_struct _tmp7D;_tmp7D.tag=
0;_tmp7D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)stage_name);{struct
Cyc_String_pa_struct _tmp7C;_tmp7C.tag=0;_tmp7C.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)exn_string);{void*_tmp7A[3]={& _tmp7C,& _tmp7D,& _tmp7E};Cyc_fprintf(
Cyc_stderr,({const char*_tmp7B="\n%s thrown during %s: %s";_tag_dynforward(_tmp7B,
sizeof(char),_get_zero_arr_size_char(_tmp7B,25));}),_tag_dynforward(_tmp7A,
sizeof(void*),3));}}}});}if(Cyc_compile_failure){({void*_tmp7F=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp80="\nCOMPILATION FAILED!\n";_tag_dynforward(_tmp80,sizeof(char),
_get_zero_arr_size_char(_tmp80,22));}),_tag_dynforward(_tmp7F,sizeof(void*),0));});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);if(other_exn)(int)_throw(ex);return
_tmp6B;}else{if(Cyc_v_r){({struct Cyc_String_pa_struct _tmp83;_tmp83.tag=0;_tmp83.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)stage_name);{void*_tmp81[1]={&
_tmp83};Cyc_fprintf(Cyc_stderr,({const char*_tmp82="%s completed.\n";
_tag_dynforward(_tmp82,sizeof(char),_get_zero_arr_size_char(_tmp82,15));}),
_tag_dynforward(_tmp81,sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);return _tmp6B;}}return _tmp6B;}struct Cyc_List_List*Cyc_do_parse(struct
Cyc___cycFILE*f,struct Cyc_List_List*ignore){Cyc_Lex_lex_init(1);{struct Cyc_List_List*
_tmp89=Cyc_Parse_parse_file(f);Cyc_Lex_lex_init(1);return _tmp89;}}struct _tuple13{
struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;};struct Cyc_List_List*Cyc_do_typecheck(
struct _tuple13*tcenv,struct Cyc_List_List*tds){struct _RegionHandle*_tmp8B;struct
Cyc_Tcenv_Tenv*_tmp8C;struct _tuple13 _tmp8A=*tcenv;_tmp8B=_tmp8A.f1;_tmp8C=_tmp8A.f2;
Cyc_Tc_tc(_tmp8B,_tmp8C,1,tds);if(!Cyc_noshake_r)tds=Cyc_Tc_treeshake(_tmp8C,tds);
return tds;}struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){
Cyc_NewControlFlow_cf_check(tds);return tds;}struct _tuple14{struct Cyc_Tcenv_Tenv*
f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};struct Cyc_List_List*Cyc_do_interface(
struct _tuple14*params,struct Cyc_List_List*tds){struct _tuple14 _tmp8E;struct Cyc_Tcenv_Tenv*
_tmp8F;struct Cyc___cycFILE*_tmp90;struct Cyc___cycFILE*_tmp91;struct _tuple14*
_tmp8D=params;_tmp8E=*_tmp8D;_tmp8F=_tmp8E.f1;_tmp90=_tmp8E.f2;_tmp91=_tmp8E.f3;{
struct Cyc_Interface_I*_tmp92=Cyc_Interface_extract(_tmp8F->ae);if(_tmp90 == 0)Cyc_Interface_save(
_tmp92,_tmp91);else{struct Cyc_Interface_I*_tmp93=Cyc_Interface_parse((struct Cyc___cycFILE*)
_tmp90);if(!Cyc_Interface_is_subinterface(_tmp93,_tmp92,({struct _tuple11*_tmp94=
_cycalloc(sizeof(*_tmp94));_tmp94->f1=({const char*_tmp96="written interface";
_tag_dynforward(_tmp96,sizeof(char),_get_zero_arr_size_char(_tmp96,18));});
_tmp94->f2=({const char*_tmp95="maximal interface";_tag_dynforward(_tmp95,sizeof(
char),_get_zero_arr_size_char(_tmp95,18));});_tmp94;})))Cyc_compile_failure=1;
else{Cyc_Interface_save(_tmp93,_tmp91);}}return tds;}}struct Cyc_List_List*Cyc_do_translate(
int ignore,struct Cyc_List_List*tds){return Cyc_Toc_toc(tds);}struct Cyc_List_List*
Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){Cyc_Tovc_elim_array_initializers=
Cyc_Cyclone_tovc_r;return Cyc_Tovc_tovc(tds);}static char _tmp97[1]="";static struct
_dynforward_ptr Cyc_cyc_include={_tmp97,_tmp97 + 1};static void Cyc_set_cyc_include(
struct _dynforward_ptr f){Cyc_cyc_include=f;}struct Cyc_List_List*Cyc_do_print(
struct Cyc___cycFILE*out_file,struct Cyc_List_List*tds){struct Cyc_Absynpp_Params
_tmp98=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;_tmp98.expand_typedefs=
!Cyc_noexpand_r;_tmp98.to_VC=Cyc_Cyclone_tovc_r;_tmp98.add_cyc_prefix=Cyc_add_cyc_namespace_r;
_tmp98.generate_line_directives=Cyc_generate_line_directives_r;_tmp98.print_full_evars=
Cyc_print_full_evars_r;_tmp98.print_all_tvars=Cyc_print_all_tvars_r;_tmp98.print_all_kinds=
Cyc_print_all_kinds_r;_tmp98.print_all_effects=Cyc_print_all_effects_r;if(Cyc_inline_functions_r)({
void*_tmp99=0;Cyc_fprintf(out_file,({const char*_tmp9A="#define _INLINE_FUNCTIONS\n";
_tag_dynforward(_tmp9A,sizeof(char),_get_zero_arr_size_char(_tmp9A,27));}),
_tag_dynforward(_tmp99,sizeof(void*),0));});{struct Cyc___cycFILE*_tmp9B=Cyc_try_file_open(
Cyc_cyc_include,({const char*_tmpA1="r";_tag_dynforward(_tmpA1,sizeof(char),
_get_zero_arr_size_char(_tmpA1,2));}),({const char*_tmpA2="internal compiler file";
_tag_dynforward(_tmpA2,sizeof(char),_get_zero_arr_size_char(_tmpA2,23));}));if(
_tmp9B == 0)return tds;{unsigned int n_read=1024;unsigned int n_written;char buf[1024];{
unsigned int _tmpA0=1024;unsigned int i;for(i=0;i < _tmpA0;i ++){buf[i]='\000';}}
while(n_read == 1024){n_read=Cyc_fread(_tag_dynforward(buf,sizeof(char),1024),1,
1024,(struct Cyc___cycFILE*)_tmp9B);if(n_read != 1024  && !Cyc_feof((struct Cyc___cycFILE*)
_tmp9B)){Cyc_compile_failure=1;({void*_tmp9C=0;Cyc_fprintf(Cyc_stderr,({const
char*_tmp9D="\nError: problem copying cyc_include.h\n";_tag_dynforward(_tmp9D,
sizeof(char),_get_zero_arr_size_char(_tmp9D,39));}),_tag_dynforward(_tmp9C,
sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return tds;}
n_written=Cyc_fwrite(_tag_dynforward(buf,sizeof(char),1024),1,n_read,out_file);
if(n_read != n_written){Cyc_compile_failure=1;({void*_tmp9E=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp9F="\nError: problem copying cyc_include.h\n";_tag_dynforward(
_tmp9F,sizeof(char),_get_zero_arr_size_char(_tmp9F,39));}),_tag_dynforward(
_tmp9E,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return
tds;}}Cyc_fclose((struct Cyc___cycFILE*)_tmp9B);if(Cyc_pp_r){Cyc_Absynpp_set_params(&
_tmp98);Cyc_Absynpp_decllist2file(tds,out_file);}else{Cyc_Absyndump_set_params(&
_tmp98);Cyc_Absyndump_dumpdecllist2file(tds,out_file);}Cyc_fflush((struct Cyc___cycFILE*)
out_file);return tds;}}}static struct Cyc_List_List*Cyc_cfiles=0;static void Cyc_remove_cfiles(){
if(!Cyc_save_c_r)for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)
_check_null(Cyc_cfiles))->tl){Cyc_remove_file(*((struct _dynforward_ptr*)((struct
Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}static struct Cyc_List_List*Cyc_split_by_char(
struct _dynforward_ptr s,char c){if(s.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr)return 0;{struct Cyc_List_List*_tmpA3=0;unsigned int
_tmpA4=Cyc_strlen((struct _dynforward_ptr)s);while(_tmpA4 > 0){struct
_dynforward_ptr _tmpA5=Cyc_strchr(s,c);if(_tmpA5.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr){_tmpA3=({struct Cyc_List_List*_tmpA6=_cycalloc(
sizeof(*_tmpA6));_tmpA6->hd=({struct _dynforward_ptr*_tmpA7=_cycalloc(sizeof(*
_tmpA7));_tmpA7[0]=s;_tmpA7;});_tmpA6->tl=_tmpA3;_tmpA6;});break;}else{_tmpA3=({
struct Cyc_List_List*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->hd=({struct
_dynforward_ptr*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=(struct
_dynforward_ptr)Cyc_substring((struct _dynforward_ptr)s,0,(unsigned int)((((
struct _dynforward_ptr)_tmpA5).curr - s.curr)/ sizeof(char)));_tmpA9;});_tmpA8->tl=
_tmpA3;_tmpA8;});_tmpA4 -=(((struct _dynforward_ptr)_tmpA5).curr - s.curr)/ sizeof(
char);s=_dynforward_ptr_plus(_tmpA5,sizeof(char),1);}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpA3);}}static int Cyc_file_exists(
struct _dynforward_ptr file){struct Cyc___cycFILE*f=0;{struct _handler_cons _tmpAA;
_push_handler(& _tmpAA);{int _tmpAC=0;if(setjmp(_tmpAA.handler))_tmpAC=1;if(!
_tmpAC){f=(struct Cyc___cycFILE*)Cyc_file_open(file,({const char*_tmpAD="r";
_tag_dynforward(_tmpAD,sizeof(char),_get_zero_arr_size_char(_tmpAD,2));}));;
_pop_handler();}else{void*_tmpAB=(void*)_exn_thrown;void*_tmpAF=_tmpAB;_LL13:;
_LL14: goto _LL12;_LL15:;_LL16:(void)_throw(_tmpAF);_LL12:;}}}if(f == 0)return 0;
else{Cyc_fclose((struct Cyc___cycFILE*)f);return 1;}}static struct _dynforward_ptr
Cyc_sprint_list(struct Cyc_List_List*dirs){struct _dynforward_ptr tmp=({const char*
_tmpB2="";_tag_dynforward(_tmpB2,sizeof(char),_get_zero_arr_size_char(_tmpB2,1));});
for(0;dirs != 0;dirs=dirs->tl){struct _dynforward_ptr _tmpB0=*((struct
_dynforward_ptr*)dirs->hd);if(_tmpB0.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr  || Cyc_strlen((struct _dynforward_ptr)_tmpB0)== 0)
continue;_tmpB0=(struct _dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)
_tmpB0,({const char*_tmpB1=":";_tag_dynforward(_tmpB1,sizeof(char),
_get_zero_arr_size_char(_tmpB1,2));}));tmp=(struct _dynforward_ptr)Cyc_strconcat((
struct _dynforward_ptr)_tmpB0,(struct _dynforward_ptr)tmp);}return tmp;}static
struct _dynforward_ptr*Cyc_find(struct Cyc_List_List*dirs,struct _dynforward_ptr
file){if(file.curr == ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr)return
0;for(0;dirs != 0;dirs=dirs->tl){struct _dynforward_ptr _tmpB3=*((struct
_dynforward_ptr*)dirs->hd);if(_tmpB3.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr  || Cyc_strlen((struct _dynforward_ptr)_tmpB3)== 0)
continue;{struct _dynforward_ptr s=(struct _dynforward_ptr)Cyc_Filename_concat(
_tmpB3,file);if(Cyc_file_exists(s))return({struct _dynforward_ptr*_tmpB4=
_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=s;_tmpB4;});}}return 0;}static struct
_dynforward_ptr Cyc_do_find(struct Cyc_List_List*dirs,struct _dynforward_ptr file){
struct _dynforward_ptr*_tmpB5=Cyc_find(dirs,file);if(_tmpB5 == 0){Cyc_compile_failure=
1;Cyc_remove_cfiles();({struct Cyc_String_pa_struct _tmpB9;_tmpB9.tag=0;_tmpB9.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sprint_list(dirs));{struct Cyc_String_pa_struct
_tmpB8;_tmpB8.tag=0;_tmpB8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
file);{void*_tmpB6[2]={& _tmpB8,& _tmpB9};Cyc_fprintf(Cyc_stderr,({const char*
_tmpB7="Error: can't find internal compiler file %s in path %s\n";
_tag_dynforward(_tmpB7,sizeof(char),_get_zero_arr_size_char(_tmpB7,56));}),
_tag_dynforward(_tmpB6,sizeof(void*),2));}}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpBA=_cycalloc(
sizeof(*_tmpBA));_tmpBA[0]=({struct Cyc_Core_Failure_struct _tmpBB;_tmpBB.tag=Cyc_Core_Failure;
_tmpBB.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmpBF;_tmpBF.tag=0;
_tmpBF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sprint_list(dirs));{
struct Cyc_String_pa_struct _tmpBE;_tmpBE.tag=0;_tmpBE.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)file);{void*_tmpBC[2]={& _tmpBE,& _tmpBF};Cyc_aprintf(({
const char*_tmpBD="Error: can't find internal compiler file %s in path %s\n";
_tag_dynforward(_tmpBD,sizeof(char),_get_zero_arr_size_char(_tmpBD,56));}),
_tag_dynforward(_tmpBC,sizeof(void*),2));}}});_tmpBB;});_tmpBA;}));}return*
_tmpB5;}static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct
_dynforward_ptr subdir){struct Cyc_List_List*_tmpC0=0;for(0;dirs != 0;dirs=dirs->tl){
_tmpC0=({struct Cyc_List_List*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1->hd=({
struct _dynforward_ptr*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=(struct
_dynforward_ptr)Cyc_Filename_concat(*((struct _dynforward_ptr*)dirs->hd),subdir);
_tmpC2;});_tmpC1->tl=_tmpC0;_tmpC1;});}_tmpC0=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmpC0);return _tmpC0;}static struct Cyc_List_List*
Cyc_also_subdir(struct Cyc_List_List*dirs,struct _dynforward_ptr subdir){struct Cyc_List_List*
_tmpC3=0;for(0;dirs != 0;dirs=dirs->tl){_tmpC3=({struct Cyc_List_List*_tmpC4=
_cycalloc(sizeof(*_tmpC4));_tmpC4->hd=(struct _dynforward_ptr*)dirs->hd;_tmpC4->tl=
_tmpC3;_tmpC4;});_tmpC3=({struct Cyc_List_List*_tmpC5=_cycalloc(sizeof(*_tmpC5));
_tmpC5->hd=({struct _dynforward_ptr*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=(
struct _dynforward_ptr)Cyc_Filename_concat(*((struct _dynforward_ptr*)dirs->hd),
subdir);_tmpC6;});_tmpC5->tl=_tmpC3;_tmpC5;});}_tmpC3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpC3);return _tmpC3;}static int Cyc_is_other_special(
char c){switch(c){case '\\': _LL17: goto _LL18;case '"': _LL18: goto _LL19;case ';': _LL19:
goto _LL1A;case '&': _LL1A: goto _LL1B;case '(': _LL1B: goto _LL1C;case ')': _LL1C: goto
_LL1D;case '|': _LL1D: goto _LL1E;case '^': _LL1E: goto _LL1F;case '<': _LL1F: goto _LL20;
case '>': _LL20: goto _LL21;case ' ': _LL21: goto _LL22;case '\n': _LL22: goto _LL23;case '\t':
_LL23: return 1;default: _LL24: return 0;}}static struct _dynforward_ptr Cyc_sh_escape_string(
struct _dynforward_ptr s){unsigned int _tmpC7=Cyc_strlen((struct _dynforward_ptr)s);
int _tmpC8=0;int _tmpC9=0;{int i=0;for(0;i < _tmpC7;i ++){char _tmpCA=*((const char*)
_check_dynforward_subscript(s,sizeof(char),i));if(_tmpCA == '\'')_tmpC8 ++;else{
if(Cyc_is_other_special(_tmpCA))_tmpC9 ++;}}}if(_tmpC8 == 0  && _tmpC9 == 0)return s;
if(_tmpC8 == 0)return(struct _dynforward_ptr)Cyc_strconcat_l(({struct
_dynforward_ptr*_tmpCB[3];_tmpCB[2]=_init_dynforward_ptr(_cycalloc(sizeof(struct
_dynforward_ptr)),"'",sizeof(char),2);_tmpCB[1]=({struct _dynforward_ptr*_tmpCD=
_cycalloc(sizeof(*_tmpCD));_tmpCD[0]=(struct _dynforward_ptr)s;_tmpCD;});_tmpCB[0]=
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"'",sizeof(char),2);((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmpCB,sizeof(struct _dynforward_ptr*),3));}));{unsigned int _tmpCF=(_tmpC7 + 
_tmpC8)+ _tmpC9;struct _dynforward_ptr s2=({unsigned int _tmpD9=_tmpCF + 1;char*
_tmpDA=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmpD9 + 1));struct
_dynforward_ptr _tmpDC=_tag_dynforward(_tmpDA,sizeof(char),_tmpD9 + 1);{
unsigned int _tmpDB=_tmpD9;unsigned int i;for(i=0;i < _tmpDB;i ++){_tmpDA[i]='\000';}
_tmpDA[_tmpDB]=(char)0;}_tmpDC;});int _tmpD0=0;int _tmpD1=0;for(0;_tmpD0 < _tmpC7;
_tmpD0 ++){char _tmpD2=*((const char*)_check_dynforward_subscript(s,sizeof(char),
_tmpD0));if(_tmpD2 == '\''  || Cyc_is_other_special(_tmpD2))({struct
_dynforward_ptr _tmpD3=_dynforward_ptr_plus(s2,sizeof(char),_tmpD1 ++);char _tmpD4=*((
char*)_check_dynforward_subscript(_tmpD3,sizeof(char),0));char _tmpD5='\\';if(
_get_dynforward_size(_tmpD3,sizeof(char))== 1  && (_tmpD4 == '\000'  && _tmpD5 != '\000'))
_throw_arraybounds();*((char*)_tmpD3.curr)=_tmpD5;});({struct _dynforward_ptr
_tmpD6=_dynforward_ptr_plus(s2,sizeof(char),_tmpD1 ++);char _tmpD7=*((char*)
_check_dynforward_subscript(_tmpD6,sizeof(char),0));char _tmpD8=_tmpD2;if(
_get_dynforward_size(_tmpD6,sizeof(char))== 1  && (_tmpD7 == '\000'  && _tmpD8 != '\000'))
_throw_arraybounds();*((char*)_tmpD6.curr)=_tmpD8;});}return(struct
_dynforward_ptr)s2;}}static struct _dynforward_ptr*Cyc_sh_escape_stringptr(struct
_dynforward_ptr*sp){return({struct _dynforward_ptr*_tmpDD=_cycalloc(sizeof(*
_tmpDD));_tmpDD[0]=Cyc_sh_escape_string(*sp);_tmpDD;});}static void Cyc_process_file(
struct _dynforward_ptr filename){struct _dynforward_ptr _tmpDE=Cyc_make_base_filename(
filename,Cyc_output_file);struct _dynforward_ptr _tmpDF=Cyc_strconcat((struct
_dynforward_ptr)_tmpDE,({const char*_tmp139=".cyp";_tag_dynforward(_tmp139,
sizeof(char),_get_zero_arr_size_char(_tmp139,5));}));struct _dynforward_ptr _tmpE0=
Cyc_strconcat((struct _dynforward_ptr)_tmpDE,({const char*_tmp138=".cyci";
_tag_dynforward(_tmp138,sizeof(char),_get_zero_arr_size_char(_tmp138,6));}));
struct _dynforward_ptr _tmpE1=Cyc_strconcat((struct _dynforward_ptr)_tmpDE,({const
char*_tmp137=".cycio";_tag_dynforward(_tmp137,sizeof(char),
_get_zero_arr_size_char(_tmp137,7));}));struct _dynforward_ptr _tmpE2=Cyc_strconcat((
struct _dynforward_ptr)_tmpDE,({const char*_tmp136=".c";_tag_dynforward(_tmp136,
sizeof(char),_get_zero_arr_size_char(_tmp136,3));}));if(Cyc_v_r)({struct Cyc_String_pa_struct
_tmpE5;_tmpE5.tag=0;_tmpE5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
filename);{void*_tmpE3[1]={& _tmpE5};Cyc_fprintf(Cyc_stderr,({const char*_tmpE4="Compiling %s\n";
_tag_dynforward(_tmpE4,sizeof(char),_get_zero_arr_size_char(_tmpE4,14));}),
_tag_dynforward(_tmpE3,sizeof(void*),1));}});{struct Cyc___cycFILE*f0=Cyc_try_file_open(
filename,({const char*_tmp134="r";_tag_dynforward(_tmp134,sizeof(char),
_get_zero_arr_size_char(_tmp134,2));}),({const char*_tmp135="input file";
_tag_dynforward(_tmp135,sizeof(char),_get_zero_arr_size_char(_tmp135,11));}));
if(Cyc_compile_failure  || !((unsigned int)f0))return;Cyc_fclose((struct Cyc___cycFILE*)
f0);{struct _dynforward_ptr _tmpE6=Cyc_str_sepstr(({struct Cyc_List_List*_tmp130=
_cycalloc(sizeof(*_tmp130));_tmp130->hd=({struct _dynforward_ptr*_tmp131=
_cycalloc(sizeof(*_tmp131));_tmp131[0]=(struct _dynforward_ptr)({const char*
_tmp132="";_tag_dynforward(_tmp132,sizeof(char),_get_zero_arr_size_char(_tmp132,
1));});_tmp131;});_tmp130->tl=((struct Cyc_List_List*(*)(struct _dynforward_ptr*(*
f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs));
_tmp130;}),({const char*_tmp133=" ";_tag_dynforward(_tmp133,sizeof(char),
_get_zero_arr_size_char(_tmp133,2));}));struct _dynforward_ptr def_inc_path=(
struct _dynforward_ptr)Cstring_to_string(Cdef_inc_path);struct Cyc_List_List*
_tmpE7=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);_tmpE7=Cyc_add_subdir(
_tmpE7,({const char*_tmpE8="include";_tag_dynforward(_tmpE8,sizeof(char),
_get_zero_arr_size_char(_tmpE8,8));}));if(Cyc_strlen((struct _dynforward_ptr)
def_inc_path)> 0)_tmpE7=({struct Cyc_List_List*_tmpE9=_cycalloc(sizeof(*_tmpE9));
_tmpE9->hd=({struct _dynforward_ptr*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA[0]=
def_inc_path;_tmpEA;});_tmpE9->tl=_tmpE7;_tmpE9;});{char*_tmpEB=getenv((const
char*)"CYCLONE_INCLUDE_PATH");if((char*)_tmpEB != 0)_tmpE7=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(({
char*_tmpEC=_tmpEB;_tag_dynforward(_tmpEC,sizeof(char),_get_zero_arr_size_char(
_tmpEC,1));}),':'),_tmpE7);{struct _dynforward_ptr stdinc_string=(struct
_dynforward_ptr)Cyc_str_sepstr(({struct Cyc_List_List*_tmp12C=_cycalloc(sizeof(*
_tmp12C));_tmp12C->hd=({struct _dynforward_ptr*_tmp12D=_cycalloc(sizeof(*_tmp12D));
_tmp12D[0]=(struct _dynforward_ptr)({const char*_tmp12E="";_tag_dynforward(_tmp12E,
sizeof(char),_get_zero_arr_size_char(_tmp12E,1));});_tmp12D;});_tmp12C->tl=((
struct Cyc_List_List*(*)(struct _dynforward_ptr*(*f)(struct _dynforward_ptr*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,_tmpE7);_tmp12C;}),({
const char*_tmp12F=" -I";_tag_dynforward(_tmp12F,sizeof(char),
_get_zero_arr_size_char(_tmp12F,4));}));struct _dynforward_ptr ofile_string;if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0)ofile_string=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmpEF;_tmpEF.tag=0;_tmpEF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct _dynforward_ptr*)_check_null(Cyc_output_file)));{void*_tmpED[1]={& _tmpEF};
Cyc_aprintf(({const char*_tmpEE=" > %s";_tag_dynforward(_tmpEE,sizeof(char),
_get_zero_arr_size_char(_tmpEE,6));}),_tag_dynforward(_tmpED,sizeof(void*),1));}});
else{ofile_string=({const char*_tmpF0="";_tag_dynforward(_tmpF0,sizeof(char),
_get_zero_arr_size_char(_tmpF0,1));});}}else{ofile_string=(struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmpF3;_tmpF3.tag=0;_tmpF3.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_sh_escape_string((struct _dynforward_ptr)_tmpDF));{void*
_tmpF1[1]={& _tmpF3};Cyc_aprintf(({const char*_tmpF2=" > %s";_tag_dynforward(
_tmpF2,sizeof(char),_get_zero_arr_size_char(_tmpF2,6));}),_tag_dynforward(_tmpF1,
sizeof(void*),1));}});}{struct _dynforward_ptr fixup_string;if(Cyc_produce_dependencies){
if(Cyc_dependencies_target == 0)fixup_string=({const char*_tmpF4=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'";
_tag_dynforward(_tmpF4,sizeof(char),_get_zero_arr_size_char(_tmpF4,35));});else{
fixup_string=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmpF7;_tmpF7.tag=
0;_tmpF7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct
_dynforward_ptr*)_check_null(Cyc_dependencies_target)));{void*_tmpF5[1]={& _tmpF7};
Cyc_aprintf(({const char*_tmpF6=" | sed 's/^.*\\.cyc\\.o:/%s:/'";_tag_dynforward(
_tmpF6,sizeof(char),_get_zero_arr_size_char(_tmpF6,29));}),_tag_dynforward(
_tmpF5,sizeof(void*),1));}});}}else{fixup_string=({const char*_tmpF8="";
_tag_dynforward(_tmpF8,sizeof(char),_get_zero_arr_size_char(_tmpF8,1));});}{
struct _dynforward_ptr _tmpF9=({struct Cyc_String_pa_struct _tmp12B;_tmp12B.tag=0;
_tmp12B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)ofile_string);{struct
Cyc_String_pa_struct _tmp12A;_tmp12A.tag=0;_tmp12A.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)fixup_string);{struct Cyc_String_pa_struct _tmp129;_tmp129.tag=
0;_tmp129.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sh_escape_string(
filename));{struct Cyc_String_pa_struct _tmp128;_tmp128.tag=0;_tmp128.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)stdinc_string);{struct Cyc_String_pa_struct
_tmp127;_tmp127.tag=0;_tmp127.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmpE6);{struct Cyc_String_pa_struct _tmp126;_tmp126.tag=0;_tmp126.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_cpp);{void*_tmp124[6]={& _tmp126,&
_tmp127,& _tmp128,& _tmp129,& _tmp12A,& _tmp12B};Cyc_aprintf(({const char*_tmp125="%s %s%s %s%s%s";
_tag_dynforward(_tmp125,sizeof(char),_get_zero_arr_size_char(_tmp125,15));}),
_tag_dynforward(_tmp124,sizeof(void*),6));}}}}}}});if(Cyc_v_r)({struct Cyc_String_pa_struct
_tmpFC;_tmpFC.tag=0;_tmpFC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmpF9);{void*_tmpFA[1]={& _tmpFC};Cyc_fprintf(Cyc_stderr,({const char*_tmpFB="%s\n";
_tag_dynforward(_tmpFB,sizeof(char),_get_zero_arr_size_char(_tmpFB,4));}),
_tag_dynforward(_tmpFA,sizeof(void*),1));}});if(system((const char*)
_untag_dynforward_ptr(_tmpF9,sizeof(char),1))!= 0){Cyc_compile_failure=1;({void*
_tmpFD=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpFE="\nError: preprocessing\n";
_tag_dynforward(_tmpFE,sizeof(char),_get_zero_arr_size_char(_tmpFE,23));}),
_tag_dynforward(_tmpFD,sizeof(void*),0));});return;}if(Cyc_stop_after_cpp_r)
return;Cyc_Position_reset_position((struct _dynforward_ptr)_tmpDF);{struct Cyc___cycFILE*
in_file=Cyc_try_file_open((struct _dynforward_ptr)_tmpDF,({const char*_tmp122="r";
_tag_dynforward(_tmp122,sizeof(char),_get_zero_arr_size_char(_tmp122,2));}),({
const char*_tmp123="file";_tag_dynforward(_tmp123,sizeof(char),
_get_zero_arr_size_char(_tmp123,5));}));if(Cyc_compile_failure)return;{struct Cyc_List_List*
tds=0;{struct _handler_cons _tmpFF;_push_handler(& _tmpFF);{int _tmp101=0;if(setjmp(
_tmpFF.handler))_tmp101=1;if(!_tmp101){tds=((struct Cyc_List_List*(*)(struct
_dynforward_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct
Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmp102="parsing";_tag_dynforward(_tmp102,sizeof(char),
_get_zero_arr_size_char(_tmp102,8));}),Cyc_do_parse,(struct Cyc___cycFILE*)
_check_null(in_file),tds);;_pop_handler();}else{void*_tmp100=(void*)_exn_thrown;
void*_tmp104=_tmp100;_LL27:;_LL28: Cyc_file_close((struct Cyc___cycFILE*)
_check_null(in_file));Cyc_remove_file((struct _dynforward_ptr)_tmpDF);(int)_throw(
_tmp104);_LL29:;_LL2A:(void)_throw(_tmp104);_LL26:;}}}Cyc_file_close((struct Cyc___cycFILE*)
in_file);if(Cyc_compile_failure){Cyc_remove_file((struct _dynforward_ptr)_tmpDF);
return;}if(Cyc_Absyn_porting_c_code){Cyc_Port_port(tds);return;}{struct
_RegionHandle _tmp105=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp105;
_push_region(tc_rgn);{struct Cyc_Tcenv_Tenv*_tmp106=Cyc_Tcenv_tc_init(tc_rgn);if(
Cyc_parseonly_r){_npop_handler(0);goto PRINTC;}{struct _tuple13 _tmp107=({struct
_tuple13 _tmp10F;_tmp10F.f1=tc_rgn;_tmp10F.f2=_tmp106;_tmp10F;});tds=((struct Cyc_List_List*(*)(
struct _dynforward_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple13*,struct
Cyc_List_List*),struct _tuple13*env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmp108="type checking";_tag_dynforward(_tmp108,sizeof(char),
_get_zero_arr_size_char(_tmp108,14));}),Cyc_do_typecheck,& _tmp107,tds);if(Cyc_compile_failure){
Cyc_remove_file((struct _dynforward_ptr)_tmpDF);_npop_handler(0);return;}if(Cyc_tc_r){
_npop_handler(0);goto PRINTC;}tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr
stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*
tds))Cyc_do_stage)(({const char*_tmp109="control-flow checking";_tag_dynforward(
_tmp109,sizeof(char),_get_zero_arr_size_char(_tmp109,22));}),Cyc_do_cfcheck,1,
tds);if(Cyc_compile_failure){Cyc_remove_file((struct _dynforward_ptr)_tmpDF);
_npop_handler(0);return;}if(Cyc_ic_r){struct Cyc___cycFILE*inter_file=Cyc_fopen((
const char*)((char*)_untag_dynforward_ptr(_tmpE0,sizeof(char),1)),(const char*)"r");
struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((struct _dynforward_ptr)
_tmpE1,({const char*_tmp10D="w";_tag_dynforward(_tmp10D,sizeof(char),
_get_zero_arr_size_char(_tmp10D,2));}),({const char*_tmp10E="interface object file";
_tag_dynforward(_tmp10E,sizeof(char),_get_zero_arr_size_char(_tmp10E,22));}));
if(inter_objfile == 0){Cyc_compile_failure=1;_npop_handler(0);return;}Cyc_Position_reset_position((
struct _dynforward_ptr)_tmpE0);{struct _tuple14 _tmp10A=({struct _tuple14 _tmp10C;
_tmp10C.f1=_tmp106;_tmp10C.f2=inter_file;_tmp10C.f3=(struct Cyc___cycFILE*)
inter_objfile;_tmp10C;});tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr
stage_name,struct Cyc_List_List*(*f)(struct _tuple14*,struct Cyc_List_List*),struct
_tuple14*env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmp10B="interface checking";
_tag_dynforward(_tmp10B,sizeof(char),_get_zero_arr_size_char(_tmp10B,19));}),Cyc_do_interface,&
_tmp10A,tds);if(inter_file != 0)Cyc_file_close((struct Cyc___cycFILE*)inter_file);
Cyc_file_close((struct Cyc___cycFILE*)inter_objfile);}}}};_pop_region(tc_rgn);}
if(Cyc_cf_r)goto PRINTC;tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr
stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*
tds))Cyc_do_stage)(({const char*_tmp110="translation to C";_tag_dynforward(
_tmp110,sizeof(char),_get_zero_arr_size_char(_tmp110,17));}),Cyc_do_translate,1,
tds);if(Cyc_compile_failure){Cyc_remove_file((struct _dynforward_ptr)_tmpDF);
return;}if(!Cyc_compile_failure)Cyc_Tcutil_flush_warnings();Cyc_remove_file((
struct _dynforward_ptr)_tmpDF);if(Cyc_compile_failure)return;if(Cyc_Cyclone_tovc_r
 || Cyc_elim_se_r)tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr stage_name,
struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*
tds))Cyc_do_stage)(({const char*_tmp111="post-pass to VC";_tag_dynforward(_tmp111,
sizeof(char),_get_zero_arr_size_char(_tmp111,16));}),Cyc_do_tovc,1,tds);if(Cyc_compile_failure)
return;PRINTC: {struct Cyc___cycFILE*out_file;if((Cyc_parseonly_r  || Cyc_tc_r)
 || Cyc_cf_r){if(Cyc_output_file != 0)out_file=Cyc_try_file_open(*((struct
_dynforward_ptr*)_check_null(Cyc_output_file)),({const char*_tmp112="w";
_tag_dynforward(_tmp112,sizeof(char),_get_zero_arr_size_char(_tmp112,2));}),({
const char*_tmp113="output file";_tag_dynforward(_tmp113,sizeof(char),
_get_zero_arr_size_char(_tmp113,12));}));else{out_file=(struct Cyc___cycFILE*)Cyc_stdout;}}
else{if(Cyc_toc_r  && Cyc_output_file != 0)out_file=Cyc_try_file_open(*((struct
_dynforward_ptr*)_check_null(Cyc_output_file)),({const char*_tmp114="w";
_tag_dynforward(_tmp114,sizeof(char),_get_zero_arr_size_char(_tmp114,2));}),({
const char*_tmp115="output file";_tag_dynforward(_tmp115,sizeof(char),
_get_zero_arr_size_char(_tmp115,12));}));else{out_file=Cyc_try_file_open((struct
_dynforward_ptr)_tmpE2,({const char*_tmp116="w";_tag_dynforward(_tmp116,sizeof(
char),_get_zero_arr_size_char(_tmp116,2));}),({const char*_tmp117="output file";
_tag_dynforward(_tmp117,sizeof(char),_get_zero_arr_size_char(_tmp117,12));}));}}
if(Cyc_compile_failure  || !((unsigned int)out_file))return;{struct _handler_cons
_tmp118;_push_handler(& _tmp118);{int _tmp11A=0;if(setjmp(_tmp118.handler))_tmp11A=
1;if(!_tmp11A){if(!Cyc_noprint_r)tds=((struct Cyc_List_List*(*)(struct
_dynforward_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct
Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmp11B="printing";_tag_dynforward(_tmp11B,sizeof(char),
_get_zero_arr_size_char(_tmp11B,9));}),Cyc_do_print,(struct Cyc___cycFILE*)
out_file,tds);;_pop_handler();}else{void*_tmp119=(void*)_exn_thrown;void*_tmp11D=
_tmp119;_LL2C:;_LL2D: Cyc_compile_failure=1;Cyc_file_close((struct Cyc___cycFILE*)
out_file);Cyc_cfiles=({struct Cyc_List_List*_tmp11E=_cycalloc(sizeof(*_tmp11E));
_tmp11E->hd=({struct _dynforward_ptr*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F[
0]=(struct _dynforward_ptr)_tmpE2;_tmp11F;});_tmp11E->tl=Cyc_cfiles;_tmp11E;});(
int)_throw(_tmp11D);_LL2E:;_LL2F:(void)_throw(_tmp11D);_LL2B:;}}}Cyc_file_close((
struct Cyc___cycFILE*)out_file);Cyc_cfiles=({struct Cyc_List_List*_tmp120=
_cycalloc(sizeof(*_tmp120));_tmp120->hd=({struct _dynforward_ptr*_tmp121=
_cycalloc(sizeof(*_tmp121));_tmp121[0]=(struct _dynforward_ptr)_tmpE2;_tmp121;});
_tmp120->tl=Cyc_cfiles;_tmp120;});}}}}}}}}}}static char _tmp13A[8]="<final>";
static struct _dynforward_ptr Cyc_final_str={_tmp13A,_tmp13A + 8};static struct
_dynforward_ptr*Cyc_final_strptr=& Cyc_final_str;static struct Cyc_Interface_I*Cyc_read_cycio(
struct _dynforward_ptr*n){if(n == (struct _dynforward_ptr*)Cyc_final_strptr)return
Cyc_Interface_final();{struct _dynforward_ptr basename;{struct _handler_cons _tmp13B;
_push_handler(& _tmp13B);{int _tmp13D=0;if(setjmp(_tmp13B.handler))_tmp13D=1;if(!
_tmp13D){basename=(struct _dynforward_ptr)Cyc_Filename_chop_extension(*n);;
_pop_handler();}else{void*_tmp13C=(void*)_exn_thrown;void*_tmp13F=_tmp13C;_LL31:
if(*((void**)_tmp13F)!= Cyc_Core_Invalid_argument)goto _LL33;_LL32: basename=*n;
goto _LL30;_LL33:;_LL34:(void)_throw(_tmp13F);_LL30:;}}}{struct _dynforward_ptr
_tmp140=Cyc_strconcat((struct _dynforward_ptr)basename,({const char*_tmp145=".cycio";
_tag_dynforward(_tmp145,sizeof(char),_get_zero_arr_size_char(_tmp145,7));}));
struct Cyc___cycFILE*_tmp141=Cyc_try_file_open((struct _dynforward_ptr)_tmp140,({
const char*_tmp143="rb";_tag_dynforward(_tmp143,sizeof(char),
_get_zero_arr_size_char(_tmp143,3));}),({const char*_tmp144="interface object file";
_tag_dynforward(_tmp144,sizeof(char),_get_zero_arr_size_char(_tmp144,22));}));
if(_tmp141 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();exit(1);}Cyc_Position_reset_position((
struct _dynforward_ptr)_tmp140);{struct Cyc_Interface_I*_tmp142=Cyc_Interface_load((
struct Cyc___cycFILE*)_tmp141);Cyc_file_close((struct Cyc___cycFILE*)_tmp141);
return _tmp142;}}}}static int Cyc_is_cfile(struct _dynforward_ptr*n){return*((const
char*)_check_dynforward_subscript(*n,sizeof(char),0))!= '-';}void
GC_blacklist_warn_clear();struct _tuple15{struct _dynforward_ptr f1;int f2;struct
_dynforward_ptr f3;void*f4;struct _dynforward_ptr f5;};int Cyc_main(int argc,struct
_dynforward_ptr argv){GC_blacklist_warn_clear();{struct Cyc_List_List*
cyclone_arch_path;struct _dynforward_ptr def_lib_path;struct _dynforward_ptr comp=(
struct _dynforward_ptr)Cstring_to_string(Ccomp);{struct _RegionHandle _tmp146=
_new_region("r");struct _RegionHandle*r=& _tmp146;_push_region(r);{struct
_dynforward_ptr _tmp147=({unsigned int _tmp2EA=(unsigned int)argc;int*_tmp2EB=(int*)
_cycalloc_atomic(_check_times(sizeof(int),_tmp2EA));struct _dynforward_ptr _tmp2ED=
_tag_dynforward(_tmp2EB,sizeof(int),_tmp2EA);{unsigned int _tmp2EC=_tmp2EA;
unsigned int i;for(i=0;i < _tmp2EC;i ++){_tmp2EB[i]=0;}}_tmp2ED;});int _tmp148=0;int
i;int j;int k;for(i=1;i < argc;i ++){if(Cyc_strncmp(({const char*_tmp149="-B";
_tag_dynforward(_tmp149,sizeof(char),_get_zero_arr_size_char(_tmp149,3));}),(
struct _dynforward_ptr)*((struct _dynforward_ptr*)_check_dynforward_subscript(argv,
sizeof(struct _dynforward_ptr),i)),2)== 0){*((int*)_check_dynforward_subscript(
_tmp147,sizeof(int),i))=1;_tmp148 ++;}else{if(Cyc_strcmp(({const char*_tmp14A="-b";
_tag_dynforward(_tmp14A,sizeof(char),_get_zero_arr_size_char(_tmp14A,3));}),(
struct _dynforward_ptr)*((struct _dynforward_ptr*)_check_dynforward_subscript(argv,
sizeof(struct _dynforward_ptr),i)))== 0){*((int*)_check_dynforward_subscript(
_tmp147,sizeof(int),i))=1;_tmp148 ++;i ++;if(i >= argc)break;*((int*)
_check_dynforward_subscript(_tmp147,sizeof(int),i))=1;_tmp148 ++;}}}{struct
_dynforward_ptr _tmp14B=({unsigned int _tmp2E6=(unsigned int)(_tmp148 + 1);struct
_dynforward_ptr*_tmp2E7=(struct _dynforward_ptr*)_cycalloc(_check_times(sizeof(
struct _dynforward_ptr),_tmp2E6));struct _dynforward_ptr _tmp2E9=_tag_dynforward(
_tmp2E7,sizeof(struct _dynforward_ptr),_tmp2E6);{unsigned int _tmp2E8=_tmp2E6;
unsigned int n;for(n=0;n < _tmp2E8;n ++){_tmp2E7[n]=(struct _dynforward_ptr)
_tag_dynforward(0,0,0);}}_tmp2E9;});struct _dynforward_ptr _tmp14C=({unsigned int
_tmp2E2=(unsigned int)(argc - _tmp148);struct _dynforward_ptr*_tmp2E3=(struct
_dynforward_ptr*)_cycalloc(_check_times(sizeof(struct _dynforward_ptr),_tmp2E2));
struct _dynforward_ptr _tmp2E5=_tag_dynforward(_tmp2E3,sizeof(struct
_dynforward_ptr),_tmp2E2);{unsigned int _tmp2E4=_tmp2E2;unsigned int n;for(n=0;n < 
_tmp2E4;n ++){_tmp2E3[n]=(struct _dynforward_ptr)_tag_dynforward(0,0,0);}}_tmp2E5;});*((
struct _dynforward_ptr*)_check_dynforward_subscript(_tmp14B,sizeof(struct
_dynforward_ptr),0))=(*((struct _dynforward_ptr*)_check_dynforward_subscript(
_tmp14C,sizeof(struct _dynforward_ptr),0))=*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),0)));for(i=(j=(k=
1));i < argc;i ++){if(*((int*)_check_dynforward_subscript(_tmp147,sizeof(int),i)))*((
struct _dynforward_ptr*)_check_dynforward_subscript(_tmp14B,sizeof(struct
_dynforward_ptr),j ++))=*((struct _dynforward_ptr*)_check_dynforward_subscript(
argv,sizeof(struct _dynforward_ptr),i));else{*((struct _dynforward_ptr*)
_check_dynforward_subscript(_tmp14C,sizeof(struct _dynforward_ptr),k ++))=*((
struct _dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct
_dynforward_ptr),i));}}{struct Cyc_List_List*options=({struct _tuple15*_tmp167[63];
_tmp167[62]=({struct _tuple15*_tmp2DC=_region_malloc(r,sizeof(*_tmp2DC));_tmp2DC->f1=({
const char*_tmp2E1="-noregions";_tag_dynforward(_tmp2E1,sizeof(char),
_get_zero_arr_size_char(_tmp2E1,11));});_tmp2DC->f2=0;_tmp2DC->f3=({const char*
_tmp2E0="";_tag_dynforward(_tmp2E0,sizeof(char),_get_zero_arr_size_char(_tmp2E0,
1));});_tmp2DC->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp2DE=_region_malloc(
r,sizeof(*_tmp2DE));_tmp2DE[0]=({struct Cyc_Arg_Set_spec_struct _tmp2DF;_tmp2DF.tag=
3;_tmp2DF.f1=& Cyc_Absyn_no_regions;_tmp2DF;});_tmp2DE;});_tmp2DC->f5=({const char*
_tmp2DD="Generate code that doesn't use regions";_tag_dynforward(_tmp2DD,sizeof(
char),_get_zero_arr_size_char(_tmp2DD,39));});_tmp2DC;});_tmp167[61]=({struct
_tuple15*_tmp2D6=_region_malloc(r,sizeof(*_tmp2D6));_tmp2D6->f1=({const char*
_tmp2DB="-port";_tag_dynforward(_tmp2DB,sizeof(char),_get_zero_arr_size_char(
_tmp2DB,6));});_tmp2D6->f2=0;_tmp2D6->f3=({const char*_tmp2DA="";_tag_dynforward(
_tmp2DA,sizeof(char),_get_zero_arr_size_char(_tmp2DA,1));});_tmp2D6->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp2D8=_region_malloc(r,sizeof(*_tmp2D8));
_tmp2D8[0]=({struct Cyc_Arg_Unit_spec_struct _tmp2D9;_tmp2D9.tag=0;_tmp2D9.f1=Cyc_set_port_c_code;
_tmp2D9;});_tmp2D8;});_tmp2D6->f5=({const char*_tmp2D7="Suggest how to port C code to Cyclone";
_tag_dynforward(_tmp2D7,sizeof(char),_get_zero_arr_size_char(_tmp2D7,38));});
_tmp2D6;});_tmp167[60]=({struct _tuple15*_tmp2D0=_region_malloc(r,sizeof(*_tmp2D0));
_tmp2D0->f1=({const char*_tmp2D5="-detailedlocation";_tag_dynforward(_tmp2D5,
sizeof(char),_get_zero_arr_size_char(_tmp2D5,18));});_tmp2D0->f2=0;_tmp2D0->f3=({
const char*_tmp2D4="";_tag_dynforward(_tmp2D4,sizeof(char),
_get_zero_arr_size_char(_tmp2D4,1));});_tmp2D0->f4=(void*)({struct Cyc_Arg_Clear_spec_struct*
_tmp2D2=_region_malloc(r,sizeof(*_tmp2D2));_tmp2D2[0]=({struct Cyc_Arg_Clear_spec_struct
_tmp2D3;_tmp2D3.tag=4;_tmp2D3.f1=& Cyc_Position_use_gcc_style_location;_tmp2D3;});
_tmp2D2;});_tmp2D0->f5=({const char*_tmp2D1="Try to give more detailed location information for errors";
_tag_dynforward(_tmp2D1,sizeof(char),_get_zero_arr_size_char(_tmp2D1,58));});
_tmp2D0;});_tmp167[59]=({struct _tuple15*_tmp2CA=_region_malloc(r,sizeof(*_tmp2CA));
_tmp2CA->f1=({const char*_tmp2CF="-noregister";_tag_dynforward(_tmp2CF,sizeof(
char),_get_zero_arr_size_char(_tmp2CF,12));});_tmp2CA->f2=0;_tmp2CA->f3=({const
char*_tmp2CE="";_tag_dynforward(_tmp2CE,sizeof(char),_get_zero_arr_size_char(
_tmp2CE,1));});_tmp2CA->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp2CC=
_region_malloc(r,sizeof(*_tmp2CC));_tmp2CC[0]=({struct Cyc_Arg_Set_spec_struct
_tmp2CD;_tmp2CD.tag=3;_tmp2CD.f1=& Cyc_Parse_no_register;_tmp2CD;});_tmp2CC;});
_tmp2CA->f5=({const char*_tmp2CB="Treat register storage class as public";
_tag_dynforward(_tmp2CB,sizeof(char),_get_zero_arr_size_char(_tmp2CB,39));});
_tmp2CA;});_tmp167[58]=({struct _tuple15*_tmp2C4=_region_malloc(r,sizeof(*_tmp2C4));
_tmp2C4->f1=({const char*_tmp2C9="-warnnullchecks";_tag_dynforward(_tmp2C9,
sizeof(char),_get_zero_arr_size_char(_tmp2C9,16));});_tmp2C4->f2=0;_tmp2C4->f3=({
const char*_tmp2C8="";_tag_dynforward(_tmp2C8,sizeof(char),
_get_zero_arr_size_char(_tmp2C8,1));});_tmp2C4->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp2C6=_region_malloc(r,sizeof(*_tmp2C6));_tmp2C6[0]=({struct Cyc_Arg_Set_spec_struct
_tmp2C7;_tmp2C7.tag=3;_tmp2C7.f1=& Cyc_Toc_warn_all_null_deref;_tmp2C7;});_tmp2C6;});
_tmp2C4->f5=({const char*_tmp2C5="Warn when any null check can't be eliminated";
_tag_dynforward(_tmp2C5,sizeof(char),_get_zero_arr_size_char(_tmp2C5,45));});
_tmp2C4;});_tmp167[57]=({struct _tuple15*_tmp2BE=_region_malloc(r,sizeof(*_tmp2BE));
_tmp2BE->f1=({const char*_tmp2C3="-warnboundschecks";_tag_dynforward(_tmp2C3,
sizeof(char),_get_zero_arr_size_char(_tmp2C3,18));});_tmp2BE->f2=0;_tmp2BE->f3=({
const char*_tmp2C2="";_tag_dynforward(_tmp2C2,sizeof(char),
_get_zero_arr_size_char(_tmp2C2,1));});_tmp2BE->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp2C0=_region_malloc(r,sizeof(*_tmp2C0));_tmp2C0[0]=({struct Cyc_Arg_Set_spec_struct
_tmp2C1;_tmp2C1.tag=3;_tmp2C1.f1=& Cyc_Toc_warn_bounds_checks;_tmp2C1;});_tmp2C0;});
_tmp2BE->f5=({const char*_tmp2BF="Warn when bounds checks can't be eliminated";
_tag_dynforward(_tmp2BF,sizeof(char),_get_zero_arr_size_char(_tmp2BF,44));});
_tmp2BE;});_tmp167[56]=({struct _tuple15*_tmp2B8=_region_malloc(r,sizeof(*_tmp2B8));
_tmp2B8->f1=({const char*_tmp2BD="-CI";_tag_dynforward(_tmp2BD,sizeof(char),
_get_zero_arr_size_char(_tmp2BD,4));});_tmp2B8->f2=0;_tmp2B8->f3=({const char*
_tmp2BC=" <file>";_tag_dynforward(_tmp2BC,sizeof(char),_get_zero_arr_size_char(
_tmp2BC,8));});_tmp2B8->f4=(void*)({struct Cyc_Arg_String_spec_struct*_tmp2BA=
_region_malloc(r,sizeof(*_tmp2BA));_tmp2BA[0]=({struct Cyc_Arg_String_spec_struct
_tmp2BB;_tmp2BB.tag=5;_tmp2BB.f1=Cyc_set_cyc_include;_tmp2BB;});_tmp2BA;});
_tmp2B8->f5=({const char*_tmp2B9="Set cyc_include.h to be <file>";_tag_dynforward(
_tmp2B9,sizeof(char),_get_zero_arr_size_char(_tmp2B9,31));});_tmp2B8;});_tmp167[
55]=({struct _tuple15*_tmp2B2=_region_malloc(r,sizeof(*_tmp2B2));_tmp2B2->f1=({
const char*_tmp2B7="-printalleffects";_tag_dynforward(_tmp2B7,sizeof(char),
_get_zero_arr_size_char(_tmp2B7,17));});_tmp2B2->f2=0;_tmp2B2->f3=({const char*
_tmp2B6="";_tag_dynforward(_tmp2B6,sizeof(char),_get_zero_arr_size_char(_tmp2B6,
1));});_tmp2B2->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp2B4=_region_malloc(
r,sizeof(*_tmp2B4));_tmp2B4[0]=({struct Cyc_Arg_Set_spec_struct _tmp2B5;_tmp2B5.tag=
3;_tmp2B5.f1=& Cyc_print_all_effects_r;_tmp2B5;});_tmp2B4;});_tmp2B2->f5=({const
char*_tmp2B3="Print effects for functions (type debugging)";_tag_dynforward(
_tmp2B3,sizeof(char),_get_zero_arr_size_char(_tmp2B3,45));});_tmp2B2;});_tmp167[
54]=({struct _tuple15*_tmp2AC=_region_malloc(r,sizeof(*_tmp2AC));_tmp2AC->f1=({
const char*_tmp2B1="-printfullevars";_tag_dynforward(_tmp2B1,sizeof(char),
_get_zero_arr_size_char(_tmp2B1,16));});_tmp2AC->f2=0;_tmp2AC->f3=({const char*
_tmp2B0="";_tag_dynforward(_tmp2B0,sizeof(char),_get_zero_arr_size_char(_tmp2B0,
1));});_tmp2AC->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp2AE=_region_malloc(
r,sizeof(*_tmp2AE));_tmp2AE[0]=({struct Cyc_Arg_Set_spec_struct _tmp2AF;_tmp2AF.tag=
3;_tmp2AF.f1=& Cyc_print_full_evars_r;_tmp2AF;});_tmp2AE;});_tmp2AC->f5=({const
char*_tmp2AD="Print full information for evars (type debugging)";_tag_dynforward(
_tmp2AD,sizeof(char),_get_zero_arr_size_char(_tmp2AD,50));});_tmp2AC;});_tmp167[
53]=({struct _tuple15*_tmp2A6=_region_malloc(r,sizeof(*_tmp2A6));_tmp2A6->f1=({
const char*_tmp2AB="-printallkinds";_tag_dynforward(_tmp2AB,sizeof(char),
_get_zero_arr_size_char(_tmp2AB,15));});_tmp2A6->f2=0;_tmp2A6->f3=({const char*
_tmp2AA="";_tag_dynforward(_tmp2AA,sizeof(char),_get_zero_arr_size_char(_tmp2AA,
1));});_tmp2A6->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp2A8=_region_malloc(
r,sizeof(*_tmp2A8));_tmp2A8[0]=({struct Cyc_Arg_Set_spec_struct _tmp2A9;_tmp2A9.tag=
3;_tmp2A9.f1=& Cyc_print_all_kinds_r;_tmp2A9;});_tmp2A8;});_tmp2A6->f5=({const
char*_tmp2A7="Always print kinds of type variables";_tag_dynforward(_tmp2A7,
sizeof(char),_get_zero_arr_size_char(_tmp2A7,37));});_tmp2A6;});_tmp167[52]=({
struct _tuple15*_tmp2A0=_region_malloc(r,sizeof(*_tmp2A0));_tmp2A0->f1=({const
char*_tmp2A5="-printalltvars";_tag_dynforward(_tmp2A5,sizeof(char),
_get_zero_arr_size_char(_tmp2A5,15));});_tmp2A0->f2=0;_tmp2A0->f3=({const char*
_tmp2A4="";_tag_dynforward(_tmp2A4,sizeof(char),_get_zero_arr_size_char(_tmp2A4,
1));});_tmp2A0->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp2A2=_region_malloc(
r,sizeof(*_tmp2A2));_tmp2A2[0]=({struct Cyc_Arg_Set_spec_struct _tmp2A3;_tmp2A3.tag=
3;_tmp2A3.f1=& Cyc_print_all_tvars_r;_tmp2A3;});_tmp2A2;});_tmp2A0->f5=({const
char*_tmp2A1="Print all type variables (even implicit default effects)";
_tag_dynforward(_tmp2A1,sizeof(char),_get_zero_arr_size_char(_tmp2A1,57));});
_tmp2A0;});_tmp167[51]=({struct _tuple15*_tmp29A=_region_malloc(r,sizeof(*_tmp29A));
_tmp29A->f1=({const char*_tmp29F="-noexpandtypedefs";_tag_dynforward(_tmp29F,
sizeof(char),_get_zero_arr_size_char(_tmp29F,18));});_tmp29A->f2=0;_tmp29A->f3=({
const char*_tmp29E="";_tag_dynforward(_tmp29E,sizeof(char),
_get_zero_arr_size_char(_tmp29E,1));});_tmp29A->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp29C=_region_malloc(r,sizeof(*_tmp29C));_tmp29C[0]=({struct Cyc_Arg_Set_spec_struct
_tmp29D;_tmp29D.tag=3;_tmp29D.f1=& Cyc_noexpand_r;_tmp29D;});_tmp29C;});_tmp29A->f5=({
const char*_tmp29B="Don't expand typedefs in pretty printing";_tag_dynforward(
_tmp29B,sizeof(char),_get_zero_arr_size_char(_tmp29B,41));});_tmp29A;});_tmp167[
50]=({struct _tuple15*_tmp294=_region_malloc(r,sizeof(*_tmp294));_tmp294->f1=({
const char*_tmp299="-noremoveunused";_tag_dynforward(_tmp299,sizeof(char),
_get_zero_arr_size_char(_tmp299,16));});_tmp294->f2=0;_tmp294->f3=({const char*
_tmp298="";_tag_dynforward(_tmp298,sizeof(char),_get_zero_arr_size_char(_tmp298,
1));});_tmp294->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp296=_region_malloc(
r,sizeof(*_tmp296));_tmp296[0]=({struct Cyc_Arg_Set_spec_struct _tmp297;_tmp297.tag=
3;_tmp297.f1=& Cyc_noshake_r;_tmp297;});_tmp296;});_tmp294->f5=({const char*
_tmp295="Don't remove externed variables that aren't used";_tag_dynforward(
_tmp295,sizeof(char),_get_zero_arr_size_char(_tmp295,49));});_tmp294;});_tmp167[
49]=({struct _tuple15*_tmp28E=_region_malloc(r,sizeof(*_tmp28E));_tmp28E->f1=({
const char*_tmp293="-nogc";_tag_dynforward(_tmp293,sizeof(char),
_get_zero_arr_size_char(_tmp293,6));});_tmp28E->f2=0;_tmp28E->f3=({const char*
_tmp292="";_tag_dynforward(_tmp292,sizeof(char),_get_zero_arr_size_char(_tmp292,
1));});_tmp28E->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp290=_region_malloc(
r,sizeof(*_tmp290));_tmp290[0]=({struct Cyc_Arg_Set_spec_struct _tmp291;_tmp291.tag=
3;_tmp291.f1=& Cyc_nogc_r;_tmp291;});_tmp290;});_tmp28E->f5=({const char*_tmp28F="Don't link in the garbage collector";
_tag_dynforward(_tmp28F,sizeof(char),_get_zero_arr_size_char(_tmp28F,36));});
_tmp28E;});_tmp167[48]=({struct _tuple15*_tmp288=_region_malloc(r,sizeof(*_tmp288));
_tmp288->f1=({const char*_tmp28D="-nocyc";_tag_dynforward(_tmp28D,sizeof(char),
_get_zero_arr_size_char(_tmp28D,7));});_tmp288->f2=0;_tmp288->f3=({const char*
_tmp28C="";_tag_dynforward(_tmp28C,sizeof(char),_get_zero_arr_size_char(_tmp28C,
1));});_tmp288->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp28A=
_region_malloc(r,sizeof(*_tmp28A));_tmp28A[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp28B;_tmp28B.tag=0;_tmp28B.f1=Cyc_set_nocyc;_tmp28B;});_tmp28A;});_tmp288->f5=({
const char*_tmp289="Don't add implicit namespace Cyc";_tag_dynforward(_tmp289,
sizeof(char),_get_zero_arr_size_char(_tmp289,33));});_tmp288;});_tmp167[47]=({
struct _tuple15*_tmp282=_region_malloc(r,sizeof(*_tmp282));_tmp282->f1=({const
char*_tmp287="-no-cpp-precomp";_tag_dynforward(_tmp287,sizeof(char),
_get_zero_arr_size_char(_tmp287,16));});_tmp282->f2=0;_tmp282->f3=({const char*
_tmp286="";_tag_dynforward(_tmp286,sizeof(char),_get_zero_arr_size_char(_tmp286,
1));});_tmp282->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp284=
_region_malloc(r,sizeof(*_tmp284));_tmp284[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp285;_tmp285.tag=0;_tmp285.f1=Cyc_set_nocppprecomp;_tmp285;});_tmp284;});
_tmp282->f5=({const char*_tmp283="Don't do smart preprocessing (mac only)";
_tag_dynforward(_tmp283,sizeof(char),_get_zero_arr_size_char(_tmp283,40));});
_tmp282;});_tmp167[46]=({struct _tuple15*_tmp27C=_region_malloc(r,sizeof(*_tmp27C));
_tmp27C->f1=({const char*_tmp281="-use-cpp";_tag_dynforward(_tmp281,sizeof(char),
_get_zero_arr_size_char(_tmp281,9));});_tmp27C->f2=0;_tmp27C->f3=({const char*
_tmp280="<path>";_tag_dynforward(_tmp280,sizeof(char),_get_zero_arr_size_char(
_tmp280,7));});_tmp27C->f4=(void*)({struct Cyc_Arg_String_spec_struct*_tmp27E=
_region_malloc(r,sizeof(*_tmp27E));_tmp27E[0]=({struct Cyc_Arg_String_spec_struct
_tmp27F;_tmp27F.tag=5;_tmp27F.f1=Cyc_set_cpp;_tmp27F;});_tmp27E;});_tmp27C->f5=({
const char*_tmp27D="Indicate which preprocessor to use";_tag_dynforward(_tmp27D,
sizeof(char),_get_zero_arr_size_char(_tmp27D,35));});_tmp27C;});_tmp167[45]=({
struct _tuple15*_tmp276=_region_malloc(r,sizeof(*_tmp276));_tmp276->f1=({const
char*_tmp27B="--inline-checks";_tag_dynforward(_tmp27B,sizeof(char),
_get_zero_arr_size_char(_tmp27B,16));});_tmp276->f2=0;_tmp276->f3=({const char*
_tmp27A="";_tag_dynforward(_tmp27A,sizeof(char),_get_zero_arr_size_char(_tmp27A,
1));});_tmp276->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp278=
_region_malloc(r,sizeof(*_tmp278));_tmp278[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp279;_tmp279.tag=0;_tmp279.f1=Cyc_set_inline_functions;_tmp279;});_tmp278;});
_tmp276->f5=({const char*_tmp277="Inline bounds checks instead of #define";
_tag_dynforward(_tmp277,sizeof(char),_get_zero_arr_size_char(_tmp277,40));});
_tmp276;});_tmp167[44]=({struct _tuple15*_tmp270=_region_malloc(r,sizeof(*_tmp270));
_tmp270->f1=({const char*_tmp275="--noboundschecks";_tag_dynforward(_tmp275,
sizeof(char),_get_zero_arr_size_char(_tmp275,17));});_tmp270->f2=0;_tmp270->f3=({
const char*_tmp274="";_tag_dynforward(_tmp274,sizeof(char),
_get_zero_arr_size_char(_tmp274,1));});_tmp270->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp272=_region_malloc(r,sizeof(*_tmp272));_tmp272[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp273;_tmp273.tag=0;_tmp273.f1=Cyc_set_noboundschecks;_tmp273;});_tmp272;});
_tmp270->f5=({const char*_tmp271="Disable bounds checks";_tag_dynforward(_tmp271,
sizeof(char),_get_zero_arr_size_char(_tmp271,22));});_tmp270;});_tmp167[43]=({
struct _tuple15*_tmp26A=_region_malloc(r,sizeof(*_tmp26A));_tmp26A->f1=({const
char*_tmp26F="--nonullchecks";_tag_dynforward(_tmp26F,sizeof(char),
_get_zero_arr_size_char(_tmp26F,15));});_tmp26A->f2=0;_tmp26A->f3=({const char*
_tmp26E="";_tag_dynforward(_tmp26E,sizeof(char),_get_zero_arr_size_char(_tmp26E,
1));});_tmp26A->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp26C=
_region_malloc(r,sizeof(*_tmp26C));_tmp26C[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp26D;_tmp26D.tag=0;_tmp26D.f1=Cyc_set_nonullchecks;_tmp26D;});_tmp26C;});
_tmp26A->f5=({const char*_tmp26B="Disable null checks";_tag_dynforward(_tmp26B,
sizeof(char),_get_zero_arr_size_char(_tmp26B,20));});_tmp26A;});_tmp167[42]=({
struct _tuple15*_tmp264=_region_malloc(r,sizeof(*_tmp264));_tmp264->f1=({const
char*_tmp269="--nochecks";_tag_dynforward(_tmp269,sizeof(char),
_get_zero_arr_size_char(_tmp269,11));});_tmp264->f2=0;_tmp264->f3=({const char*
_tmp268="";_tag_dynforward(_tmp268,sizeof(char),_get_zero_arr_size_char(_tmp268,
1));});_tmp264->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp266=
_region_malloc(r,sizeof(*_tmp266));_tmp266[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp267;_tmp267.tag=0;_tmp267.f1=Cyc_set_nochecks;_tmp267;});_tmp266;});_tmp264->f5=({
const char*_tmp265="Disable bounds/null checks";_tag_dynforward(_tmp265,sizeof(
char),_get_zero_arr_size_char(_tmp265,27));});_tmp264;});_tmp167[41]=({struct
_tuple15*_tmp25E=_region_malloc(r,sizeof(*_tmp25E));_tmp25E->f1=({const char*
_tmp263="--lineno";_tag_dynforward(_tmp263,sizeof(char),_get_zero_arr_size_char(
_tmp263,9));});_tmp25E->f2=0;_tmp25E->f3=({const char*_tmp262="";_tag_dynforward(
_tmp262,sizeof(char),_get_zero_arr_size_char(_tmp262,1));});_tmp25E->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp260=_region_malloc(r,sizeof(*_tmp260));
_tmp260[0]=({struct Cyc_Arg_Unit_spec_struct _tmp261;_tmp261.tag=0;_tmp261.f1=Cyc_set_lineno;
_tmp261;});_tmp260;});_tmp25E->f5=({const char*_tmp25F="Generate line numbers for debugging";
_tag_dynforward(_tmp25F,sizeof(char),_get_zero_arr_size_char(_tmp25F,36));});
_tmp25E;});_tmp167[40]=({struct _tuple15*_tmp258=_region_malloc(r,sizeof(*_tmp258));
_tmp258->f1=({const char*_tmp25D="-save-c";_tag_dynforward(_tmp25D,sizeof(char),
_get_zero_arr_size_char(_tmp25D,8));});_tmp258->f2=0;_tmp258->f3=({const char*
_tmp25C="";_tag_dynforward(_tmp25C,sizeof(char),_get_zero_arr_size_char(_tmp25C,
1));});_tmp258->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp25A=_region_malloc(
r,sizeof(*_tmp25A));_tmp25A[0]=({struct Cyc_Arg_Set_spec_struct _tmp25B;_tmp25B.tag=
3;_tmp25B.f1=& Cyc_save_c_r;_tmp25B;});_tmp25A;});_tmp258->f5=({const char*_tmp259="Don't delete temporary C files";
_tag_dynforward(_tmp259,sizeof(char),_get_zero_arr_size_char(_tmp259,31));});
_tmp258;});_tmp167[39]=({struct _tuple15*_tmp252=_region_malloc(r,sizeof(*_tmp252));
_tmp252->f1=({const char*_tmp257="-save-temps";_tag_dynforward(_tmp257,sizeof(
char),_get_zero_arr_size_char(_tmp257,12));});_tmp252->f2=0;_tmp252->f3=({const
char*_tmp256="";_tag_dynforward(_tmp256,sizeof(char),_get_zero_arr_size_char(
_tmp256,1));});_tmp252->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp254=
_region_malloc(r,sizeof(*_tmp254));_tmp254[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp255;_tmp255.tag=0;_tmp255.f1=Cyc_set_save_temps;_tmp255;});_tmp254;});
_tmp252->f5=({const char*_tmp253="Don't delete temporary files";_tag_dynforward(
_tmp253,sizeof(char),_get_zero_arr_size_char(_tmp253,29));});_tmp252;});_tmp167[
38]=({struct _tuple15*_tmp24C=_region_malloc(r,sizeof(*_tmp24C));_tmp24C->f1=({
const char*_tmp251="-c-comp";_tag_dynforward(_tmp251,sizeof(char),
_get_zero_arr_size_char(_tmp251,8));});_tmp24C->f2=0;_tmp24C->f3=({const char*
_tmp250=" <compiler>";_tag_dynforward(_tmp250,sizeof(char),
_get_zero_arr_size_char(_tmp250,12));});_tmp24C->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp24E=_region_malloc(r,sizeof(*_tmp24E));_tmp24E[0]=({struct Cyc_Arg_String_spec_struct
_tmp24F;_tmp24F.tag=5;_tmp24F.f1=Cyc_set_c_compiler;_tmp24F;});_tmp24E;});
_tmp24C->f5=({const char*_tmp24D="Produce C output for the given compiler";
_tag_dynforward(_tmp24D,sizeof(char),_get_zero_arr_size_char(_tmp24D,40));});
_tmp24C;});_tmp167[37]=({struct _tuple15*_tmp246=_region_malloc(r,sizeof(*_tmp246));
_tmp246->f1=({const char*_tmp24B="-un-gcc";_tag_dynforward(_tmp24B,sizeof(char),
_get_zero_arr_size_char(_tmp24B,8));});_tmp246->f2=0;_tmp246->f3=({const char*
_tmp24A="";_tag_dynforward(_tmp24A,sizeof(char),_get_zero_arr_size_char(_tmp24A,
1));});_tmp246->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp248=
_region_malloc(r,sizeof(*_tmp248));_tmp248[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp249;_tmp249.tag=0;_tmp249.f1=Cyc_set_tovc;_tmp249;});_tmp248;});_tmp246->f5=({
const char*_tmp247="Avoid gcc extensions in C output";_tag_dynforward(_tmp247,
sizeof(char),_get_zero_arr_size_char(_tmp247,33));});_tmp246;});_tmp167[36]=({
struct _tuple15*_tmp240=_region_malloc(r,sizeof(*_tmp240));_tmp240->f1=({const
char*_tmp245="-elim-statement-expressions";_tag_dynforward(_tmp245,sizeof(char),
_get_zero_arr_size_char(_tmp245,28));});_tmp240->f2=0;_tmp240->f3=({const char*
_tmp244="";_tag_dynforward(_tmp244,sizeof(char),_get_zero_arr_size_char(_tmp244,
1));});_tmp240->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp242=_region_malloc(
r,sizeof(*_tmp242));_tmp242[0]=({struct Cyc_Arg_Set_spec_struct _tmp243;_tmp243.tag=
3;_tmp243.f1=& Cyc_elim_se_r;_tmp243;});_tmp242;});_tmp240->f5=({const char*
_tmp241="Avoid statement expressions in C output (implies --inline-checks)";
_tag_dynforward(_tmp241,sizeof(char),_get_zero_arr_size_char(_tmp241,66));});
_tmp240;});_tmp167[35]=({struct _tuple15*_tmp23A=_region_malloc(r,sizeof(*_tmp23A));
_tmp23A->f1=({const char*_tmp23F="-up";_tag_dynforward(_tmp23F,sizeof(char),
_get_zero_arr_size_char(_tmp23F,4));});_tmp23A->f2=0;_tmp23A->f3=({const char*
_tmp23E="";_tag_dynforward(_tmp23E,sizeof(char),_get_zero_arr_size_char(_tmp23E,
1));});_tmp23A->f4=(void*)({struct Cyc_Arg_Clear_spec_struct*_tmp23C=
_region_malloc(r,sizeof(*_tmp23C));_tmp23C[0]=({struct Cyc_Arg_Clear_spec_struct
_tmp23D;_tmp23D.tag=4;_tmp23D.f1=& Cyc_pp_r;_tmp23D;});_tmp23C;});_tmp23A->f5=({
const char*_tmp23B="Ugly print";_tag_dynforward(_tmp23B,sizeof(char),
_get_zero_arr_size_char(_tmp23B,11));});_tmp23A;});_tmp167[34]=({struct _tuple15*
_tmp234=_region_malloc(r,sizeof(*_tmp234));_tmp234->f1=({const char*_tmp239="-pp";
_tag_dynforward(_tmp239,sizeof(char),_get_zero_arr_size_char(_tmp239,4));});
_tmp234->f2=0;_tmp234->f3=({const char*_tmp238="";_tag_dynforward(_tmp238,sizeof(
char),_get_zero_arr_size_char(_tmp238,1));});_tmp234->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp236=_region_malloc(r,sizeof(*_tmp236));_tmp236[0]=({struct Cyc_Arg_Set_spec_struct
_tmp237;_tmp237.tag=3;_tmp237.f1=& Cyc_pp_r;_tmp237;});_tmp236;});_tmp234->f5=({
const char*_tmp235="Pretty print";_tag_dynforward(_tmp235,sizeof(char),
_get_zero_arr_size_char(_tmp235,13));});_tmp234;});_tmp167[33]=({struct _tuple15*
_tmp22E=_region_malloc(r,sizeof(*_tmp22E));_tmp22E->f1=({const char*_tmp233="-ic";
_tag_dynforward(_tmp233,sizeof(char),_get_zero_arr_size_char(_tmp233,4));});
_tmp22E->f2=0;_tmp22E->f3=({const char*_tmp232="";_tag_dynforward(_tmp232,sizeof(
char),_get_zero_arr_size_char(_tmp232,1));});_tmp22E->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp230=_region_malloc(r,sizeof(*_tmp230));_tmp230[0]=({struct Cyc_Arg_Set_spec_struct
_tmp231;_tmp231.tag=3;_tmp231.f1=& Cyc_ic_r;_tmp231;});_tmp230;});_tmp22E->f5=({
const char*_tmp22F="Activate the link-checker";_tag_dynforward(_tmp22F,sizeof(
char),_get_zero_arr_size_char(_tmp22F,26));});_tmp22E;});_tmp167[32]=({struct
_tuple15*_tmp228=_region_malloc(r,sizeof(*_tmp228));_tmp228->f1=({const char*
_tmp22D="-stopafter-toc";_tag_dynforward(_tmp22D,sizeof(char),
_get_zero_arr_size_char(_tmp22D,15));});_tmp228->f2=0;_tmp228->f3=({const char*
_tmp22C="";_tag_dynforward(_tmp22C,sizeof(char),_get_zero_arr_size_char(_tmp22C,
1));});_tmp228->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp22A=_region_malloc(
r,sizeof(*_tmp22A));_tmp22A[0]=({struct Cyc_Arg_Set_spec_struct _tmp22B;_tmp22B.tag=
3;_tmp22B.f1=& Cyc_toc_r;_tmp22B;});_tmp22A;});_tmp228->f5=({const char*_tmp229="Stop after translation to C";
_tag_dynforward(_tmp229,sizeof(char),_get_zero_arr_size_char(_tmp229,28));});
_tmp228;});_tmp167[31]=({struct _tuple15*_tmp222=_region_malloc(r,sizeof(*_tmp222));
_tmp222->f1=({const char*_tmp227="-stopafter-cf";_tag_dynforward(_tmp227,sizeof(
char),_get_zero_arr_size_char(_tmp227,14));});_tmp222->f2=0;_tmp222->f3=({const
char*_tmp226="";_tag_dynforward(_tmp226,sizeof(char),_get_zero_arr_size_char(
_tmp226,1));});_tmp222->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp224=
_region_malloc(r,sizeof(*_tmp224));_tmp224[0]=({struct Cyc_Arg_Set_spec_struct
_tmp225;_tmp225.tag=3;_tmp225.f1=& Cyc_cf_r;_tmp225;});_tmp224;});_tmp222->f5=({
const char*_tmp223="Stop after control-flow checking";_tag_dynforward(_tmp223,
sizeof(char),_get_zero_arr_size_char(_tmp223,33));});_tmp222;});_tmp167[30]=({
struct _tuple15*_tmp21C=_region_malloc(r,sizeof(*_tmp21C));_tmp21C->f1=({const
char*_tmp221="-noprint";_tag_dynforward(_tmp221,sizeof(char),
_get_zero_arr_size_char(_tmp221,9));});_tmp21C->f2=0;_tmp21C->f3=({const char*
_tmp220="";_tag_dynforward(_tmp220,sizeof(char),_get_zero_arr_size_char(_tmp220,
1));});_tmp21C->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp21E=_region_malloc(
r,sizeof(*_tmp21E));_tmp21E[0]=({struct Cyc_Arg_Set_spec_struct _tmp21F;_tmp21F.tag=
3;_tmp21F.f1=& Cyc_noprint_r;_tmp21F;});_tmp21E;});_tmp21C->f5=({const char*
_tmp21D="Do not print output (when stopping early)";_tag_dynforward(_tmp21D,
sizeof(char),_get_zero_arr_size_char(_tmp21D,42));});_tmp21C;});_tmp167[29]=({
struct _tuple15*_tmp216=_region_malloc(r,sizeof(*_tmp216));_tmp216->f1=({const
char*_tmp21B="-stopafter-tc";_tag_dynforward(_tmp21B,sizeof(char),
_get_zero_arr_size_char(_tmp21B,14));});_tmp216->f2=0;_tmp216->f3=({const char*
_tmp21A="";_tag_dynforward(_tmp21A,sizeof(char),_get_zero_arr_size_char(_tmp21A,
1));});_tmp216->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp218=_region_malloc(
r,sizeof(*_tmp218));_tmp218[0]=({struct Cyc_Arg_Set_spec_struct _tmp219;_tmp219.tag=
3;_tmp219.f1=& Cyc_tc_r;_tmp219;});_tmp218;});_tmp216->f5=({const char*_tmp217="Stop after type checking";
_tag_dynforward(_tmp217,sizeof(char),_get_zero_arr_size_char(_tmp217,25));});
_tmp216;});_tmp167[28]=({struct _tuple15*_tmp210=_region_malloc(r,sizeof(*_tmp210));
_tmp210->f1=({const char*_tmp215="-stopafter-parse";_tag_dynforward(_tmp215,
sizeof(char),_get_zero_arr_size_char(_tmp215,17));});_tmp210->f2=0;_tmp210->f3=({
const char*_tmp214="";_tag_dynforward(_tmp214,sizeof(char),
_get_zero_arr_size_char(_tmp214,1));});_tmp210->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp212=_region_malloc(r,sizeof(*_tmp212));_tmp212[0]=({struct Cyc_Arg_Set_spec_struct
_tmp213;_tmp213.tag=3;_tmp213.f1=& Cyc_parseonly_r;_tmp213;});_tmp212;});_tmp210->f5=({
const char*_tmp211="Stop after parsing";_tag_dynforward(_tmp211,sizeof(char),
_get_zero_arr_size_char(_tmp211,19));});_tmp210;});_tmp167[27]=({struct _tuple15*
_tmp20A=_region_malloc(r,sizeof(*_tmp20A));_tmp20A->f1=({const char*_tmp20F="-E";
_tag_dynforward(_tmp20F,sizeof(char),_get_zero_arr_size_char(_tmp20F,3));});
_tmp20A->f2=0;_tmp20A->f3=({const char*_tmp20E="";_tag_dynforward(_tmp20E,sizeof(
char),_get_zero_arr_size_char(_tmp20E,1));});_tmp20A->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp20C=_region_malloc(r,sizeof(*_tmp20C));_tmp20C[0]=({struct Cyc_Arg_Set_spec_struct
_tmp20D;_tmp20D.tag=3;_tmp20D.f1=& Cyc_stop_after_cpp_r;_tmp20D;});_tmp20C;});
_tmp20A->f5=({const char*_tmp20B="Stop after preprocessing";_tag_dynforward(
_tmp20B,sizeof(char),_get_zero_arr_size_char(_tmp20B,25));});_tmp20A;});_tmp167[
26]=({struct _tuple15*_tmp204=_region_malloc(r,sizeof(*_tmp204));_tmp204->f1=({
const char*_tmp209="-Wall";_tag_dynforward(_tmp209,sizeof(char),
_get_zero_arr_size_char(_tmp209,6));});_tmp204->f2=0;_tmp204->f3=({const char*
_tmp208="";_tag_dynforward(_tmp208,sizeof(char),_get_zero_arr_size_char(_tmp208,
1));});_tmp204->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp206=
_region_malloc(r,sizeof(*_tmp206));_tmp206[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp207;_tmp207.tag=0;_tmp207.f1=Cyc_set_all_warnings;_tmp207;});_tmp206;});
_tmp204->f5=({const char*_tmp205="Turn on all warnings";_tag_dynforward(_tmp205,
sizeof(char),_get_zero_arr_size_char(_tmp205,21));});_tmp204;});_tmp167[25]=({
struct _tuple15*_tmp1FE=_region_malloc(r,sizeof(*_tmp1FE));_tmp1FE->f1=({const
char*_tmp203="-b";_tag_dynforward(_tmp203,sizeof(char),_get_zero_arr_size_char(
_tmp203,3));});_tmp1FE->f2=0;_tmp1FE->f3=({const char*_tmp202="<arch>";
_tag_dynforward(_tmp202,sizeof(char),_get_zero_arr_size_char(_tmp202,7));});
_tmp1FE->f4=(void*)({struct Cyc_Arg_String_spec_struct*_tmp200=_region_malloc(r,
sizeof(*_tmp200));_tmp200[0]=({struct Cyc_Arg_String_spec_struct _tmp201;_tmp201.tag=
5;_tmp201.f1=Cyc_set_target_arch;_tmp201;});_tmp200;});_tmp1FE->f5=({const char*
_tmp1FF="Set target architecture; implies -stopafter-toc";_tag_dynforward(
_tmp1FF,sizeof(char),_get_zero_arr_size_char(_tmp1FF,48));});_tmp1FE;});_tmp167[
24]=({struct _tuple15*_tmp1F8=_region_malloc(r,sizeof(*_tmp1F8));_tmp1F8->f1=({
const char*_tmp1FD="-MT";_tag_dynforward(_tmp1FD,sizeof(char),
_get_zero_arr_size_char(_tmp1FD,4));});_tmp1F8->f2=0;_tmp1F8->f3=({const char*
_tmp1FC=" <target>";_tag_dynforward(_tmp1FC,sizeof(char),_get_zero_arr_size_char(
_tmp1FC,10));});_tmp1F8->f4=(void*)({struct Cyc_Arg_String_spec_struct*_tmp1FA=
_region_malloc(r,sizeof(*_tmp1FA));_tmp1FA[0]=({struct Cyc_Arg_String_spec_struct
_tmp1FB;_tmp1FB.tag=5;_tmp1FB.f1=Cyc_set_dependencies_target;_tmp1FB;});_tmp1FA;});
_tmp1F8->f5=({const char*_tmp1F9="Give target for dependencies";_tag_dynforward(
_tmp1F9,sizeof(char),_get_zero_arr_size_char(_tmp1F9,29));});_tmp1F8;});_tmp167[
23]=({struct _tuple15*_tmp1F2=_region_malloc(r,sizeof(*_tmp1F2));_tmp1F2->f1=({
const char*_tmp1F7="-MG";_tag_dynforward(_tmp1F7,sizeof(char),
_get_zero_arr_size_char(_tmp1F7,4));});_tmp1F2->f2=0;_tmp1F2->f3=({const char*
_tmp1F6="";_tag_dynforward(_tmp1F6,sizeof(char),_get_zero_arr_size_char(_tmp1F6,
1));});_tmp1F2->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp1F4=
_region_malloc(r,sizeof(*_tmp1F4));_tmp1F4[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1F5;_tmp1F5.tag=1;_tmp1F5.f1=Cyc_add_cpparg;_tmp1F5;});_tmp1F4;});_tmp1F2->f5=({
const char*_tmp1F3="When producing dependencies assume that missing files are generated";
_tag_dynforward(_tmp1F3,sizeof(char),_get_zero_arr_size_char(_tmp1F3,68));});
_tmp1F2;});_tmp167[22]=({struct _tuple15*_tmp1EC=_region_malloc(r,sizeof(*_tmp1EC));
_tmp1EC->f1=({const char*_tmp1F1="-M";_tag_dynforward(_tmp1F1,sizeof(char),
_get_zero_arr_size_char(_tmp1F1,3));});_tmp1EC->f2=0;_tmp1EC->f3=({const char*
_tmp1F0="";_tag_dynforward(_tmp1F0,sizeof(char),_get_zero_arr_size_char(_tmp1F0,
1));});_tmp1EC->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp1EE=
_region_malloc(r,sizeof(*_tmp1EE));_tmp1EE[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1EF;_tmp1EF.tag=0;_tmp1EF.f1=Cyc_set_produce_dependencies;_tmp1EF;});_tmp1EE;});
_tmp1EC->f5=({const char*_tmp1ED="Produce dependencies";_tag_dynforward(_tmp1ED,
sizeof(char),_get_zero_arr_size_char(_tmp1ED,21));});_tmp1EC;});_tmp167[21]=({
struct _tuple15*_tmp1E6=_region_malloc(r,sizeof(*_tmp1E6));_tmp1E6->f1=({const
char*_tmp1EB="-S";_tag_dynforward(_tmp1EB,sizeof(char),_get_zero_arr_size_char(
_tmp1EB,3));});_tmp1E6->f2=0;_tmp1E6->f3=({const char*_tmp1EA="";_tag_dynforward(
_tmp1EA,sizeof(char),_get_zero_arr_size_char(_tmp1EA,1));});_tmp1E6->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1E8=_region_malloc(r,sizeof(*_tmp1E8));
_tmp1E8[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1E9;_tmp1E9.tag=0;_tmp1E9.f1=Cyc_set_stop_after_asmfile;
_tmp1E9;});_tmp1E8;});_tmp1E6->f5=({const char*_tmp1E7="Stop after producing assembly code";
_tag_dynforward(_tmp1E7,sizeof(char),_get_zero_arr_size_char(_tmp1E7,35));});
_tmp1E6;});_tmp167[20]=({struct _tuple15*_tmp1E0=_region_malloc(r,sizeof(*_tmp1E0));
_tmp1E0->f1=({const char*_tmp1E5="-pa";_tag_dynforward(_tmp1E5,sizeof(char),
_get_zero_arr_size_char(_tmp1E5,4));});_tmp1E0->f2=0;_tmp1E0->f3=({const char*
_tmp1E4="";_tag_dynforward(_tmp1E4,sizeof(char),_get_zero_arr_size_char(_tmp1E4,
1));});_tmp1E0->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp1E2=
_region_malloc(r,sizeof(*_tmp1E2));_tmp1E2[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1E3;_tmp1E3.tag=0;_tmp1E3.f1=Cyc_set_pa;_tmp1E3;});_tmp1E2;});_tmp1E0->f5=({
const char*_tmp1E1="Compile for profiling with aprof";_tag_dynforward(_tmp1E1,
sizeof(char),_get_zero_arr_size_char(_tmp1E1,33));});_tmp1E0;});_tmp167[19]=({
struct _tuple15*_tmp1DA=_region_malloc(r,sizeof(*_tmp1DA));_tmp1DA->f1=({const
char*_tmp1DF="-pg";_tag_dynforward(_tmp1DF,sizeof(char),_get_zero_arr_size_char(
_tmp1DF,4));});_tmp1DA->f2=0;_tmp1DA->f3=({const char*_tmp1DE="";_tag_dynforward(
_tmp1DE,sizeof(char),_get_zero_arr_size_char(_tmp1DE,1));});_tmp1DA->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1DC=_region_malloc(r,sizeof(*_tmp1DC));
_tmp1DC[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1DD;_tmp1DD.tag=0;_tmp1DD.f1=Cyc_set_pg;
_tmp1DD;});_tmp1DC;});_tmp1DA->f5=({const char*_tmp1DB="Compile for profiling with gprof";
_tag_dynforward(_tmp1DB,sizeof(char),_get_zero_arr_size_char(_tmp1DB,33));});
_tmp1DA;});_tmp167[18]=({struct _tuple15*_tmp1D4=_region_malloc(r,sizeof(*_tmp1D4));
_tmp1D4->f1=({const char*_tmp1D9="-p";_tag_dynforward(_tmp1D9,sizeof(char),
_get_zero_arr_size_char(_tmp1D9,3));});_tmp1D4->f2=0;_tmp1D4->f3=({const char*
_tmp1D8="";_tag_dynforward(_tmp1D8,sizeof(char),_get_zero_arr_size_char(_tmp1D8,
1));});_tmp1D4->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp1D6=
_region_malloc(r,sizeof(*_tmp1D6));_tmp1D6[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1D7;_tmp1D7.tag=1;_tmp1D7.f1=Cyc_add_ccarg;_tmp1D7;});_tmp1D6;});_tmp1D4->f5=({
const char*_tmp1D5="Compile for profiling with prof";_tag_dynforward(_tmp1D5,
sizeof(char),_get_zero_arr_size_char(_tmp1D5,32));});_tmp1D4;});_tmp167[17]=({
struct _tuple15*_tmp1CE=_region_malloc(r,sizeof(*_tmp1CE));_tmp1CE->f1=({const
char*_tmp1D3="-g";_tag_dynforward(_tmp1D3,sizeof(char),_get_zero_arr_size_char(
_tmp1D3,3));});_tmp1CE->f2=0;_tmp1CE->f3=({const char*_tmp1D2="";_tag_dynforward(
_tmp1D2,sizeof(char),_get_zero_arr_size_char(_tmp1D2,1));});_tmp1CE->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1D0=_region_malloc(r,sizeof(*_tmp1D0));
_tmp1D0[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1D1;_tmp1D1.tag=1;_tmp1D1.f1=Cyc_add_ccarg;
_tmp1D1;});_tmp1D0;});_tmp1CE->f5=({const char*_tmp1CF="Compile for debugging";
_tag_dynforward(_tmp1CF,sizeof(char),_get_zero_arr_size_char(_tmp1CF,22));});
_tmp1CE;});_tmp167[16]=({struct _tuple15*_tmp1C8=_region_malloc(r,sizeof(*_tmp1C8));
_tmp1C8->f1=({const char*_tmp1CD="-fomit-frame-pointer";_tag_dynforward(_tmp1CD,
sizeof(char),_get_zero_arr_size_char(_tmp1CD,21));});_tmp1C8->f2=0;_tmp1C8->f3=({
const char*_tmp1CC="";_tag_dynforward(_tmp1CC,sizeof(char),
_get_zero_arr_size_char(_tmp1CC,1));});_tmp1C8->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1CA=_region_malloc(r,sizeof(*_tmp1CA));_tmp1CA[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1CB;_tmp1CB.tag=1;_tmp1CB.f1=Cyc_add_ccarg;_tmp1CB;});_tmp1CA;});_tmp1C8->f5=({
const char*_tmp1C9="Omit frame pointer";_tag_dynforward(_tmp1C9,sizeof(char),
_get_zero_arr_size_char(_tmp1C9,19));});_tmp1C8;});_tmp167[15]=({struct _tuple15*
_tmp1C2=_region_malloc(r,sizeof(*_tmp1C2));_tmp1C2->f1=({const char*_tmp1C7="-O3";
_tag_dynforward(_tmp1C7,sizeof(char),_get_zero_arr_size_char(_tmp1C7,4));});
_tmp1C2->f2=0;_tmp1C2->f3=({const char*_tmp1C6="";_tag_dynforward(_tmp1C6,sizeof(
char),_get_zero_arr_size_char(_tmp1C6,1));});_tmp1C2->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1C4=_region_malloc(r,sizeof(*_tmp1C4));_tmp1C4[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1C5;_tmp1C5.tag=1;_tmp1C5.f1=Cyc_add_ccarg;_tmp1C5;});_tmp1C4;});_tmp1C2->f5=({
const char*_tmp1C3="Even more optimization";_tag_dynforward(_tmp1C3,sizeof(char),
_get_zero_arr_size_char(_tmp1C3,23));});_tmp1C2;});_tmp167[14]=({struct _tuple15*
_tmp1BC=_region_malloc(r,sizeof(*_tmp1BC));_tmp1BC->f1=({const char*_tmp1C1="-O2";
_tag_dynforward(_tmp1C1,sizeof(char),_get_zero_arr_size_char(_tmp1C1,4));});
_tmp1BC->f2=0;_tmp1BC->f3=({const char*_tmp1C0="";_tag_dynforward(_tmp1C0,sizeof(
char),_get_zero_arr_size_char(_tmp1C0,1));});_tmp1BC->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1BE=_region_malloc(r,sizeof(*_tmp1BE));_tmp1BE[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1BF;_tmp1BF.tag=1;_tmp1BF.f1=Cyc_add_ccarg;_tmp1BF;});_tmp1BE;});_tmp1BC->f5=({
const char*_tmp1BD="A higher level of optimization";_tag_dynforward(_tmp1BD,
sizeof(char),_get_zero_arr_size_char(_tmp1BD,31));});_tmp1BC;});_tmp167[13]=({
struct _tuple15*_tmp1B6=_region_malloc(r,sizeof(*_tmp1B6));_tmp1B6->f1=({const
char*_tmp1BB="-O";_tag_dynforward(_tmp1BB,sizeof(char),_get_zero_arr_size_char(
_tmp1BB,3));});_tmp1B6->f2=0;_tmp1B6->f3=({const char*_tmp1BA="";_tag_dynforward(
_tmp1BA,sizeof(char),_get_zero_arr_size_char(_tmp1BA,1));});_tmp1B6->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1B8=_region_malloc(r,sizeof(*_tmp1B8));
_tmp1B8[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1B9;_tmp1B9.tag=1;_tmp1B9.f1=Cyc_add_ccarg;
_tmp1B9;});_tmp1B8;});_tmp1B6->f5=({const char*_tmp1B7="Optimize";_tag_dynforward(
_tmp1B7,sizeof(char),_get_zero_arr_size_char(_tmp1B7,9));});_tmp1B6;});_tmp167[
12]=({struct _tuple15*_tmp1B0=_region_malloc(r,sizeof(*_tmp1B0));_tmp1B0->f1=({
const char*_tmp1B5="-O0";_tag_dynforward(_tmp1B5,sizeof(char),
_get_zero_arr_size_char(_tmp1B5,4));});_tmp1B0->f2=0;_tmp1B0->f3=({const char*
_tmp1B4="";_tag_dynforward(_tmp1B4,sizeof(char),_get_zero_arr_size_char(_tmp1B4,
1));});_tmp1B0->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp1B2=
_region_malloc(r,sizeof(*_tmp1B2));_tmp1B2[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1B3;_tmp1B3.tag=1;_tmp1B3.f1=Cyc_add_ccarg;_tmp1B3;});_tmp1B2;});_tmp1B0->f5=({
const char*_tmp1B1="Don't optimize";_tag_dynforward(_tmp1B1,sizeof(char),
_get_zero_arr_size_char(_tmp1B1,15));});_tmp1B0;});_tmp167[11]=({struct _tuple15*
_tmp1AA=_region_malloc(r,sizeof(*_tmp1AA));_tmp1AA->f1=({const char*_tmp1AF="-s";
_tag_dynforward(_tmp1AF,sizeof(char),_get_zero_arr_size_char(_tmp1AF,3));});
_tmp1AA->f2=0;_tmp1AA->f3=({const char*_tmp1AE="";_tag_dynforward(_tmp1AE,sizeof(
char),_get_zero_arr_size_char(_tmp1AE,1));});_tmp1AA->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1AC=_region_malloc(r,sizeof(*_tmp1AC));_tmp1AC[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1AD;_tmp1AD.tag=1;_tmp1AD.f1=Cyc_add_ccarg;_tmp1AD;});_tmp1AC;});_tmp1AA->f5=({
const char*_tmp1AB="Remove all symbol table and relocation info from executable";
_tag_dynforward(_tmp1AB,sizeof(char),_get_zero_arr_size_char(_tmp1AB,60));});
_tmp1AA;});_tmp167[10]=({struct _tuple15*_tmp1A4=_region_malloc(r,sizeof(*_tmp1A4));
_tmp1A4->f1=({const char*_tmp1A9="-x";_tag_dynforward(_tmp1A9,sizeof(char),
_get_zero_arr_size_char(_tmp1A9,3));});_tmp1A4->f2=0;_tmp1A4->f3=({const char*
_tmp1A8=" <language>";_tag_dynforward(_tmp1A8,sizeof(char),
_get_zero_arr_size_char(_tmp1A8,12));});_tmp1A4->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp1A6=_region_malloc(r,sizeof(*_tmp1A6));_tmp1A6[0]=({struct Cyc_Arg_String_spec_struct
_tmp1A7;_tmp1A7.tag=5;_tmp1A7.f1=Cyc_set_inputtype;_tmp1A7;});_tmp1A6;});_tmp1A4->f5=({
const char*_tmp1A5="Specify <language> for the following input files";
_tag_dynforward(_tmp1A5,sizeof(char),_get_zero_arr_size_char(_tmp1A5,49));});
_tmp1A4;});_tmp167[9]=({struct _tuple15*_tmp19E=_region_malloc(r,sizeof(*_tmp19E));
_tmp19E->f1=({const char*_tmp1A3="-c";_tag_dynforward(_tmp1A3,sizeof(char),
_get_zero_arr_size_char(_tmp1A3,3));});_tmp19E->f2=0;_tmp19E->f3=({const char*
_tmp1A2="";_tag_dynforward(_tmp1A2,sizeof(char),_get_zero_arr_size_char(_tmp1A2,
1));});_tmp19E->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp1A0=
_region_malloc(r,sizeof(*_tmp1A0));_tmp1A0[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp1A1;_tmp1A1.tag=0;_tmp1A1.f1=Cyc_set_stop_after_objectfile;_tmp1A1;});
_tmp1A0;});_tmp19E->f5=({const char*_tmp19F="Produce an object file instead of an executable; do not link";
_tag_dynforward(_tmp19F,sizeof(char),_get_zero_arr_size_char(_tmp19F,61));});
_tmp19E;});_tmp167[8]=({struct _tuple15*_tmp198=_region_malloc(r,sizeof(*_tmp198));
_tmp198->f1=({const char*_tmp19D="-m";_tag_dynforward(_tmp19D,sizeof(char),
_get_zero_arr_size_char(_tmp19D,3));});_tmp198->f2=1;_tmp198->f3=({const char*
_tmp19C="<option>";_tag_dynforward(_tmp19C,sizeof(char),_get_zero_arr_size_char(
_tmp19C,9));});_tmp198->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp19A=
_region_malloc(r,sizeof(*_tmp19A));_tmp19A[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp19B;_tmp19B.tag=1;_tmp19B.f1=Cyc_add_marg;_tmp19B;});_tmp19A;});_tmp198->f5=({
const char*_tmp199="GCC specific: pass machine-dependent flag on to gcc";
_tag_dynforward(_tmp199,sizeof(char),_get_zero_arr_size_char(_tmp199,52));});
_tmp198;});_tmp167[7]=({struct _tuple15*_tmp192=_region_malloc(r,sizeof(*_tmp192));
_tmp192->f1=({const char*_tmp197="-l";_tag_dynforward(_tmp197,sizeof(char),
_get_zero_arr_size_char(_tmp197,3));});_tmp192->f2=1;_tmp192->f3=({const char*
_tmp196="<libname>";_tag_dynforward(_tmp196,sizeof(char),_get_zero_arr_size_char(
_tmp196,10));});_tmp192->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp194=
_region_malloc(r,sizeof(*_tmp194));_tmp194[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp195;_tmp195.tag=1;_tmp195.f1=Cyc_add_libarg;_tmp195;});_tmp194;});_tmp192->f5=({
const char*_tmp193="Library file";_tag_dynforward(_tmp193,sizeof(char),
_get_zero_arr_size_char(_tmp193,13));});_tmp192;});_tmp167[6]=({struct _tuple15*
_tmp18C=_region_malloc(r,sizeof(*_tmp18C));_tmp18C->f1=({const char*_tmp191="-L";
_tag_dynforward(_tmp191,sizeof(char),_get_zero_arr_size_char(_tmp191,3));});
_tmp18C->f2=1;_tmp18C->f3=({const char*_tmp190="<dir>";_tag_dynforward(_tmp190,
sizeof(char),_get_zero_arr_size_char(_tmp190,6));});_tmp18C->f4=(void*)({struct
Cyc_Arg_Flag_spec_struct*_tmp18E=_region_malloc(r,sizeof(*_tmp18E));_tmp18E[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp18F;_tmp18F.tag=1;_tmp18F.f1=Cyc_add_ccarg;
_tmp18F;});_tmp18E;});_tmp18C->f5=({const char*_tmp18D="Add to the list of directories for -l";
_tag_dynforward(_tmp18D,sizeof(char),_get_zero_arr_size_char(_tmp18D,38));});
_tmp18C;});_tmp167[5]=({struct _tuple15*_tmp186=_region_malloc(r,sizeof(*_tmp186));
_tmp186->f1=({const char*_tmp18B="-I";_tag_dynforward(_tmp18B,sizeof(char),
_get_zero_arr_size_char(_tmp18B,3));});_tmp186->f2=1;_tmp186->f3=({const char*
_tmp18A="<dir>";_tag_dynforward(_tmp18A,sizeof(char),_get_zero_arr_size_char(
_tmp18A,6));});_tmp186->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp188=
_region_malloc(r,sizeof(*_tmp188));_tmp188[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp189;_tmp189.tag=1;_tmp189.f1=Cyc_add_cpparg;_tmp189;});_tmp188;});_tmp186->f5=({
const char*_tmp187="Add to the list of directories to search for include files";
_tag_dynforward(_tmp187,sizeof(char),_get_zero_arr_size_char(_tmp187,59));});
_tmp186;});_tmp167[4]=({struct _tuple15*_tmp180=_region_malloc(r,sizeof(*_tmp180));
_tmp180->f1=({const char*_tmp185="-B";_tag_dynforward(_tmp185,sizeof(char),
_get_zero_arr_size_char(_tmp185,3));});_tmp180->f2=1;_tmp180->f3=({const char*
_tmp184="<file>";_tag_dynforward(_tmp184,sizeof(char),_get_zero_arr_size_char(
_tmp184,7));});_tmp180->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp182=
_region_malloc(r,sizeof(*_tmp182));_tmp182[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp183;_tmp183.tag=1;_tmp183.f1=Cyc_add_cyclone_exec_path;_tmp183;});_tmp182;});
_tmp180->f5=({const char*_tmp181="Add to the list of directories to search for compiler files";
_tag_dynforward(_tmp181,sizeof(char),_get_zero_arr_size_char(_tmp181,60));});
_tmp180;});_tmp167[3]=({struct _tuple15*_tmp17A=_region_malloc(r,sizeof(*_tmp17A));
_tmp17A->f1=({const char*_tmp17F="-D";_tag_dynforward(_tmp17F,sizeof(char),
_get_zero_arr_size_char(_tmp17F,3));});_tmp17A->f2=1;_tmp17A->f3=({const char*
_tmp17E="<name>[=<value>]";_tag_dynforward(_tmp17E,sizeof(char),
_get_zero_arr_size_char(_tmp17E,17));});_tmp17A->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp17C=_region_malloc(r,sizeof(*_tmp17C));_tmp17C[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp17D;_tmp17D.tag=1;_tmp17D.f1=Cyc_add_cpparg;_tmp17D;});_tmp17C;});_tmp17A->f5=({
const char*_tmp17B="Pass definition to preprocessor";_tag_dynforward(_tmp17B,
sizeof(char),_get_zero_arr_size_char(_tmp17B,32));});_tmp17A;});_tmp167[2]=({
struct _tuple15*_tmp174=_region_malloc(r,sizeof(*_tmp174));_tmp174->f1=({const
char*_tmp179="-o";_tag_dynforward(_tmp179,sizeof(char),_get_zero_arr_size_char(
_tmp179,3));});_tmp174->f2=0;_tmp174->f3=({const char*_tmp178=" <file>";
_tag_dynforward(_tmp178,sizeof(char),_get_zero_arr_size_char(_tmp178,8));});
_tmp174->f4=(void*)({struct Cyc_Arg_String_spec_struct*_tmp176=_region_malloc(r,
sizeof(*_tmp176));_tmp176[0]=({struct Cyc_Arg_String_spec_struct _tmp177;_tmp177.tag=
5;_tmp177.f1=Cyc_set_output_file;_tmp177;});_tmp176;});_tmp174->f5=({const char*
_tmp175="Set the output file name to <file>";_tag_dynforward(_tmp175,sizeof(char),
_get_zero_arr_size_char(_tmp175,35));});_tmp174;});_tmp167[1]=({struct _tuple15*
_tmp16E=_region_malloc(r,sizeof(*_tmp16E));_tmp16E->f1=({const char*_tmp173="--version";
_tag_dynforward(_tmp173,sizeof(char),_get_zero_arr_size_char(_tmp173,10));});
_tmp16E->f2=0;_tmp16E->f3=({const char*_tmp172="";_tag_dynforward(_tmp172,sizeof(
char),_get_zero_arr_size_char(_tmp172,1));});_tmp16E->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp170=_region_malloc(r,sizeof(*_tmp170));_tmp170[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp171;_tmp171.tag=0;_tmp171.f1=Cyc_print_version;_tmp171;});_tmp170;});_tmp16E->f5=({
const char*_tmp16F="Print version information and exit";_tag_dynforward(_tmp16F,
sizeof(char),_get_zero_arr_size_char(_tmp16F,35));});_tmp16E;});_tmp167[0]=({
struct _tuple15*_tmp168=_region_malloc(r,sizeof(*_tmp168));_tmp168->f1=({const
char*_tmp16D="-v";_tag_dynforward(_tmp16D,sizeof(char),_get_zero_arr_size_char(
_tmp16D,3));});_tmp168->f2=0;_tmp168->f3=({const char*_tmp16C="";_tag_dynforward(
_tmp16C,sizeof(char),_get_zero_arr_size_char(_tmp16C,1));});_tmp168->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp16A=_region_malloc(r,sizeof(*_tmp16A));_tmp16A[
0]=({struct Cyc_Arg_Set_spec_struct _tmp16B;_tmp16B.tag=3;_tmp16B.f1=& Cyc_v_r;
_tmp16B;});_tmp16A;});_tmp168->f5=({const char*_tmp169="Print compilation stages verbosely";
_tag_dynforward(_tmp169,sizeof(char),_get_zero_arr_size_char(_tmp169,35));});
_tmp168;});((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))
Cyc_List_rlist)(r,_tag_dynforward(_tmp167,sizeof(struct _tuple15*),63));});struct
_dynforward_ptr optstring=({const char*_tmp166="Options:";_tag_dynforward(_tmp166,
sizeof(char),_get_zero_arr_size_char(_tmp166,9));});Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp14B);if(Cyc_target_arch.curr == ((struct
_dynforward_ptr)_tag_dynforward(0,0,0)).curr)Cyc_target_arch=(struct
_dynforward_ptr)Cstring_to_string(Carch);{struct _dynforward_ptr
cyclone_exec_prefix=({char*_tmp165=getenv((const char*)"CYCLONE_EXEC_PREFIX");
_tag_dynforward(_tmp165,sizeof(char),_get_zero_arr_size_char(_tmp165,1));});if(
cyclone_exec_prefix.curr != ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr)
Cyc_cyclone_exec_path=({struct Cyc_List_List*_tmp14D=_cycalloc(sizeof(*_tmp14D));
_tmp14D->hd=({struct _dynforward_ptr*_tmp14E=_cycalloc(sizeof(*_tmp14E));_tmp14E[
0]=cyclone_exec_prefix;_tmp14E;});_tmp14D->tl=Cyc_cyclone_exec_path;_tmp14D;});
def_lib_path=(struct _dynforward_ptr)Cstring_to_string(Cdef_lib_path);if(Cyc_strlen((
struct _dynforward_ptr)def_lib_path)> 0)Cyc_cyclone_exec_path=({struct Cyc_List_List*
_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F->hd=({struct _dynforward_ptr*_tmp150=
_cycalloc(sizeof(*_tmp150));_tmp150[0]=(struct _dynforward_ptr)Cyc_Filename_concat(
def_lib_path,({const char*_tmp151="cyc-lib";_tag_dynforward(_tmp151,sizeof(char),
_get_zero_arr_size_char(_tmp151,8));}));_tmp150;});_tmp14F->tl=Cyc_cyclone_exec_path;
_tmp14F;});Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_cyclone_exec_path);cyclone_arch_path=Cyc_also_subdir(Cyc_cyclone_exec_path,
Cyc_target_arch);{struct _dynforward_ptr _tmp152=Cyc_do_find(cyclone_arch_path,({
const char*_tmp164="cycspecs";_tag_dynforward(_tmp164,sizeof(char),
_get_zero_arr_size_char(_tmp164,9));}));if(Cyc_v_r)({struct Cyc_String_pa_struct
_tmp155;_tmp155.tag=0;_tmp155.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp152);{void*_tmp153[1]={& _tmp155};Cyc_fprintf(Cyc_stderr,({const char*_tmp154="Reading from specs file %s\n";
_tag_dynforward(_tmp154,sizeof(char),_get_zero_arr_size_char(_tmp154,28));}),
_tag_dynforward(_tmp153,sizeof(void*),1));}});{struct Cyc_List_List*_tmp156=Cyc_read_specs(
_tmp152);struct _dynforward_ptr _tmp157=_tag_dynforward(0,0,0);{struct
_handler_cons _tmp158;_push_handler(& _tmp158);{int _tmp15A=0;if(setjmp(_tmp158.handler))
_tmp15A=1;if(!_tmp15A){{struct _dynforward_ptr _tmp15B=*((struct _dynforward_ptr*(*)(
int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*l,
struct _dynforward_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,_tmp156,
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"cyclone",sizeof(
char),8));_tmp157=Cyc_split_specs(_tmp15B);};_pop_handler();}else{void*_tmp159=(
void*)_exn_thrown;void*_tmp15E=_tmp159;_LL36: if(_tmp15E != Cyc_Core_Not_found)
goto _LL38;_LL37: goto _LL35;_LL38:;_LL39:(void)_throw(_tmp15E);_LL35:;}}}if(
_tmp157.curr != (_tag_dynforward(0,0,0)).curr){Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp157);}Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp14C);if(Cyc_strcmp((struct _dynforward_ptr)
Cyc_cpp,({const char*_tmp15F="";_tag_dynforward(_tmp15F,sizeof(char),
_get_zero_arr_size_char(_tmp15F,1));}))== 0)Cyc_set_cpp((struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp163;_tmp163.tag=0;_tmp163.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp152);{struct Cyc_String_pa_struct
_tmp162;_tmp162.tag=0;_tmp162.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
comp);{void*_tmp160[2]={& _tmp162,& _tmp163};Cyc_aprintf(({const char*_tmp161="%s -w -x c -E -specs %s";
_tag_dynforward(_tmp161,sizeof(char),_get_zero_arr_size_char(_tmp161,24));}),
_tag_dynforward(_tmp160,sizeof(void*),2));}}}));}}}}}};_pop_region(r);}if(Cyc_cyclone_files
== 0  && Cyc_ccargs == 0){({void*_tmp2EE=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp2EF="missing file\n";_tag_dynforward(_tmp2EF,sizeof(char),
_get_zero_arr_size_char(_tmp2EF,14));}),_tag_dynforward(_tmp2EE,sizeof(void*),0));});
exit(1);}if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct _dynforward_ptr)Cyc_cyc_include)
== 0)Cyc_cyc_include=Cyc_do_find(Cyc_cyclone_exec_path,({const char*_tmp2F0="cyc_include.h";
_tag_dynforward(_tmp2F0,sizeof(char),_get_zero_arr_size_char(_tmp2F0,14));}));{
struct Cyc_List_List*_tmp2F1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_cyclone_files);for(0;_tmp2F1 != 0;_tmp2F1=_tmp2F1->tl){Cyc_process_file(*((
struct _dynforward_ptr*)_tmp2F1->hd));if(Cyc_compile_failure)return 1;}}if(((Cyc_stop_after_cpp_r
 || Cyc_parseonly_r) || Cyc_tc_r) || Cyc_toc_r)return 0;if(Cyc_ccargs == 0)return 0;
Cyc_add_ccarg((struct _dynforward_ptr)Cyc_strconcat(({const char*_tmp2F2="-L";
_tag_dynforward(_tmp2F2,sizeof(char),_get_zero_arr_size_char(_tmp2F2,3));}),(
struct _dynforward_ptr)def_lib_path));Cyc_ccargs=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{struct _dynforward_ptr _tmp2F3=Cyc_str_sepstr(((
struct Cyc_List_List*(*)(struct _dynforward_ptr*(*f)(struct _dynforward_ptr*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_ccargs),({const
char*_tmp34B=" ";_tag_dynforward(_tmp34B,sizeof(char),_get_zero_arr_size_char(
_tmp34B,2));}));Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_libargs);{struct _dynforward_ptr _tmp2F4=Cyc_str_sepstr(({struct Cyc_List_List*
_tmp348=_cycalloc(sizeof(*_tmp348));_tmp348->hd=_init_dynforward_ptr(_cycalloc(
sizeof(struct _dynforward_ptr)),"",sizeof(char),1);_tmp348->tl=((struct Cyc_List_List*(*)(
struct _dynforward_ptr*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,Cyc_libargs);_tmp348;}),({const char*_tmp34A=" ";
_tag_dynforward(_tmp34A,sizeof(char),_get_zero_arr_size_char(_tmp34A,2));}));
struct Cyc_List_List*stdlib;struct _dynforward_ptr stdlib_string;int _tmp2F5=((Cyc_stop_after_asmfile_r
 || Cyc_stop_after_objectfile_r) || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((
struct _dynforward_ptr*)_check_null(Cyc_output_file)),({const char*_tmp346=".a";
_tag_dynforward(_tmp346,sizeof(char),_get_zero_arr_size_char(_tmp346,3));})))
 || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dynforward_ptr*)
_check_null(Cyc_output_file)),({const char*_tmp347=".lib";_tag_dynforward(_tmp347,
sizeof(char),_get_zero_arr_size_char(_tmp347,5));}));if(_tmp2F5){stdlib=0;
stdlib_string=(struct _dynforward_ptr)({const char*_tmp2F6="";_tag_dynforward(
_tmp2F6,sizeof(char),_get_zero_arr_size_char(_tmp2F6,1));});}else{struct
_dynforward_ptr libcyc_flag;struct _dynforward_ptr nogc_filename;struct
_dynforward_ptr runtime_filename;struct _dynforward_ptr gc_filename;if(Cyc_pa_r){
libcyc_flag=({const char*_tmp2F7="-lcyc_a";_tag_dynforward(_tmp2F7,sizeof(char),
_get_zero_arr_size_char(_tmp2F7,8));});nogc_filename=({const char*_tmp2F8="nogc_a.a";
_tag_dynforward(_tmp2F8,sizeof(char),_get_zero_arr_size_char(_tmp2F8,9));});
runtime_filename=({const char*_tmp2F9="runtime_cyc_a.o";_tag_dynforward(_tmp2F9,
sizeof(char),_get_zero_arr_size_char(_tmp2F9,16));});}else{if(Cyc_nocheck_r){
libcyc_flag=({const char*_tmp2FA="-lcyc_nocheck";_tag_dynforward(_tmp2FA,sizeof(
char),_get_zero_arr_size_char(_tmp2FA,14));});nogc_filename=({const char*_tmp2FB="nogc_nocheck.a";
_tag_dynforward(_tmp2FB,sizeof(char),_get_zero_arr_size_char(_tmp2FB,15));});
runtime_filename=({const char*_tmp2FC="runtime_cyc.o";_tag_dynforward(_tmp2FC,
sizeof(char),_get_zero_arr_size_char(_tmp2FC,14));});}else{if(Cyc_pg_r){
libcyc_flag=({const char*_tmp2FD="-lcyc_pg";_tag_dynforward(_tmp2FD,sizeof(char),
_get_zero_arr_size_char(_tmp2FD,9));});runtime_filename=({const char*_tmp2FE="runtime_cyc_pg.o";
_tag_dynforward(_tmp2FE,sizeof(char),_get_zero_arr_size_char(_tmp2FE,17));});
nogc_filename=({const char*_tmp2FF="nogc_pg.a";_tag_dynforward(_tmp2FF,sizeof(
char),_get_zero_arr_size_char(_tmp2FF,10));});}else{libcyc_flag=({const char*
_tmp300="-lcyc";_tag_dynforward(_tmp300,sizeof(char),_get_zero_arr_size_char(
_tmp300,6));});nogc_filename=({const char*_tmp301="nogc.a";_tag_dynforward(
_tmp301,sizeof(char),_get_zero_arr_size_char(_tmp301,7));});runtime_filename=({
const char*_tmp302="runtime_cyc.o";_tag_dynforward(_tmp302,sizeof(char),
_get_zero_arr_size_char(_tmp302,14));});}}}gc_filename=({const char*_tmp303="gc.a";
_tag_dynforward(_tmp303,sizeof(char),_get_zero_arr_size_char(_tmp303,5));});{
struct _dynforward_ptr _tmp304=Cyc_nogc_r?Cyc_do_find(cyclone_arch_path,
nogc_filename): Cyc_do_find(cyclone_arch_path,gc_filename);struct _dynforward_ptr
_tmp305=Cyc_do_find(cyclone_arch_path,runtime_filename);stdlib=0;stdlib_string=(
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp30A;_tmp30A.tag=0;_tmp30A.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp304);{struct Cyc_String_pa_struct
_tmp309;_tmp309.tag=0;_tmp309.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
libcyc_flag);{struct Cyc_String_pa_struct _tmp308;_tmp308.tag=0;_tmp308.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp305);{void*_tmp306[3]={& _tmp308,&
_tmp309,& _tmp30A};Cyc_aprintf(({const char*_tmp307=" %s %s %s";_tag_dynforward(
_tmp307,sizeof(char),_get_zero_arr_size_char(_tmp307,10));}),_tag_dynforward(
_tmp306,sizeof(void*),3));}}}});}}if(Cyc_ic_r){struct _handler_cons _tmp30B;
_push_handler(& _tmp30B);{int _tmp30D=0;if(setjmp(_tmp30B.handler))_tmp30D=1;if(!
_tmp30D){Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct _dynforward_ptr*),
struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);Cyc_libargs=((
struct Cyc_List_List*(*)(int(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))
Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{struct Cyc_List_List*_tmp30E=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(Cyc_ccargs,Cyc_libargs));if(!_tmp2F5)_tmp30E=({struct Cyc_List_List*
_tmp30F=_cycalloc(sizeof(*_tmp30F));_tmp30F->hd=Cyc_final_strptr;_tmp30F->tl=
_tmp30E;_tmp30F;});{struct Cyc_Interface_I*_tmp310=((struct Cyc_Interface_I*(*)(
struct Cyc_Interface_I*(*get)(struct _dynforward_ptr*),struct Cyc_List_List*la,
struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,
_tmp30E,_tmp30E);if(_tmp310 == 0){({void*_tmp311=0;Cyc_fprintf(Cyc_stderr,({const
char*_tmp312="Error: interfaces incompatible\n";_tag_dynforward(_tmp312,sizeof(
char),_get_zero_arr_size_char(_tmp312,32));}),_tag_dynforward(_tmp311,sizeof(
void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp313=1;
_npop_handler(0);return _tmp313;}}if(_tmp2F5){if(Cyc_output_file != 0){struct
_dynforward_ptr _tmp314=({struct Cyc_String_pa_struct _tmp31B;_tmp31B.tag=0;_tmp31B.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Filename_chop_extension(*((
struct _dynforward_ptr*)_check_null(Cyc_output_file))));{void*_tmp319[1]={&
_tmp31B};Cyc_aprintf(({const char*_tmp31A="%s.cycio";_tag_dynforward(_tmp31A,
sizeof(char),_get_zero_arr_size_char(_tmp31A,9));}),_tag_dynforward(_tmp319,
sizeof(void*),1));}});struct Cyc___cycFILE*_tmp315=Cyc_try_file_open((struct
_dynforward_ptr)_tmp314,({const char*_tmp317="wb";_tag_dynforward(_tmp317,sizeof(
char),_get_zero_arr_size_char(_tmp317,3));}),({const char*_tmp318="interface object file";
_tag_dynforward(_tmp318,sizeof(char),_get_zero_arr_size_char(_tmp318,22));}));
if(_tmp315 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp316=1;
_npop_handler(0);return _tmp316;}}Cyc_Interface_save((struct Cyc_Interface_I*)
_tmp310,(struct Cyc___cycFILE*)_tmp315);Cyc_file_close((struct Cyc___cycFILE*)
_tmp315);}}else{if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(struct
Cyc_Interface_I*)_tmp310,({struct _tuple11*_tmp31C=_cycalloc(sizeof(*_tmp31C));
_tmp31C->f1=({const char*_tmp31E="empty interface";_tag_dynforward(_tmp31E,
sizeof(char),_get_zero_arr_size_char(_tmp31E,16));});_tmp31C->f2=({const char*
_tmp31D="global interface";_tag_dynforward(_tmp31D,sizeof(char),
_get_zero_arr_size_char(_tmp31D,17));});_tmp31C;}))){({void*_tmp31F=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp320="Error: some objects are still undefined\n";
_tag_dynforward(_tmp320,sizeof(char),_get_zero_arr_size_char(_tmp320,41));}),
_tag_dynforward(_tmp31F,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmp321=1;_npop_handler(0);return _tmp321;}}}}};_pop_handler();}else{void*
_tmp30C=(void*)_exn_thrown;void*_tmp323=_tmp30C;_LL3B:;_LL3C:{void*_tmp324=
_tmp323;struct _dynforward_ptr _tmp325;struct _dynforward_ptr _tmp326;struct
_dynforward_ptr _tmp327;_LL40: if(*((void**)_tmp324)!= Cyc_Core_Failure)goto _LL42;
_tmp325=((struct Cyc_Core_Failure_struct*)_tmp324)->f1;_LL41:({struct Cyc_String_pa_struct
_tmp32A;_tmp32A.tag=0;_tmp32A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp325);{void*_tmp328[1]={& _tmp32A};Cyc_fprintf(Cyc_stderr,({const char*_tmp329="Exception Core::Failure %s\n";
_tag_dynforward(_tmp329,sizeof(char),_get_zero_arr_size_char(_tmp329,28));}),
_tag_dynforward(_tmp328,sizeof(void*),1));}});goto _LL3F;_LL42: if(*((void**)
_tmp324)!= Cyc_Core_Impossible)goto _LL44;_tmp326=((struct Cyc_Core_Impossible_struct*)
_tmp324)->f1;_LL43:({struct Cyc_String_pa_struct _tmp32D;_tmp32D.tag=0;_tmp32D.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp326);{void*_tmp32B[1]={&
_tmp32D};Cyc_fprintf(Cyc_stderr,({const char*_tmp32C="Exception Core::Impossible %s\n";
_tag_dynforward(_tmp32C,sizeof(char),_get_zero_arr_size_char(_tmp32C,31));}),
_tag_dynforward(_tmp32B,sizeof(void*),1));}});goto _LL3F;_LL44: if(_tmp324 != Cyc_Dict_Absent)
goto _LL46;_LL45:({void*_tmp32E=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp32F="Exception Dict::Absent\n";
_tag_dynforward(_tmp32F,sizeof(char),_get_zero_arr_size_char(_tmp32F,24));}),
_tag_dynforward(_tmp32E,sizeof(void*),0));});goto _LL3F;_LL46: if(*((void**)
_tmp324)!= Cyc_Core_Invalid_argument)goto _LL48;_tmp327=((struct Cyc_Core_Invalid_argument_struct*)
_tmp324)->f1;_LL47:({struct Cyc_String_pa_struct _tmp332;_tmp332.tag=0;_tmp332.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp327);{void*_tmp330[1]={&
_tmp332};Cyc_fprintf(Cyc_stderr,({const char*_tmp331="Exception Core::Invalid_argument %s\n";
_tag_dynforward(_tmp331,sizeof(char),_get_zero_arr_size_char(_tmp331,37));}),
_tag_dynforward(_tmp330,sizeof(void*),1));}});goto _LL3F;_LL48:;_LL49:({void*
_tmp333=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp334="Uncaught exception\n";
_tag_dynforward(_tmp334,sizeof(char),_get_zero_arr_size_char(_tmp334,20));}),
_tag_dynforward(_tmp333,sizeof(void*),0));});goto _LL3F;_LL3F:;}Cyc_compile_failure=
1;Cyc_remove_cfiles();return 1;_LL3D:;_LL3E:(void)_throw(_tmp323);_LL3A:;}}}{
struct _dynforward_ptr outfile_str=({const char*_tmp345="";_tag_dynforward(_tmp345,
sizeof(char),_get_zero_arr_size_char(_tmp345,1));});if(Cyc_output_file != 0)
outfile_str=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp337;_tmp337.tag=
0;_tmp337.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sh_escape_string(*((
struct _dynforward_ptr*)_check_null(Cyc_output_file))));{void*_tmp335[1]={&
_tmp337};Cyc_aprintf(({const char*_tmp336=" -o %s";_tag_dynforward(_tmp336,
sizeof(char),_get_zero_arr_size_char(_tmp336,7));}),_tag_dynforward(_tmp335,
sizeof(void*),1));}});{struct _dynforward_ptr _tmp338=({struct Cyc_String_pa_struct
_tmp344;_tmp344.tag=0;_tmp344.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp2F4);{struct Cyc_String_pa_struct _tmp343;_tmp343.tag=0;_tmp343.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)stdlib_string);{struct Cyc_String_pa_struct
_tmp342;_tmp342.tag=0;_tmp342.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp2F3);{struct Cyc_String_pa_struct _tmp341;_tmp341.tag=0;_tmp341.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)outfile_str);{struct Cyc_String_pa_struct
_tmp340;_tmp340.tag=0;_tmp340.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
comp);{void*_tmp33E[5]={& _tmp340,& _tmp341,& _tmp342,& _tmp343,& _tmp344};Cyc_aprintf(({
const char*_tmp33F="%s %s %s%s%s";_tag_dynforward(_tmp33F,sizeof(char),
_get_zero_arr_size_char(_tmp33F,13));}),_tag_dynforward(_tmp33E,sizeof(void*),5));}}}}}});
if(Cyc_v_r){({struct Cyc_String_pa_struct _tmp33B;_tmp33B.tag=0;_tmp33B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp338);{void*_tmp339[1]={& _tmp33B};Cyc_fprintf(
Cyc_stderr,({const char*_tmp33A="%s\n";_tag_dynforward(_tmp33A,sizeof(char),
_get_zero_arr_size_char(_tmp33A,4));}),_tag_dynforward(_tmp339,sizeof(void*),1));}});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}if(system((const char*)
_untag_dynforward_ptr(_tmp338,sizeof(char),1))!= 0){({void*_tmp33C=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp33D="Error: C compiler failed\n";_tag_dynforward(
_tmp33D,sizeof(char),_get_zero_arr_size_char(_tmp33D,26));}),_tag_dynforward(
_tmp33C,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;}
Cyc_remove_cfiles();return Cyc_compile_failure?1: 0;}}}}}}
