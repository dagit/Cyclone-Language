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
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Core_NewRegion
Cyc_Core_new_dynregion();extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[
16];void Cyc_Core_free_dynregion(struct _DynRegionHandle*);typedef struct{int
__count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef
struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;
struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct
Cyc___cycFILE*,struct _dynforward_ptr,struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dynforward_ptr f1;}
;struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,struct _dynforward_ptr);
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*
x);void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rrev(
struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*
x,int n);int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);int Cyc_strcmp(
struct _dynforward_ptr s1,struct _dynforward_ptr s2);struct _dynforward_ptr Cyc_strconcat(
struct _dynforward_ptr,struct _dynforward_ptr);struct Cyc_Iter_Iter{void*env;int(*
next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct
Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(
void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*
s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[
11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char
Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*
cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*,
struct Cyc_Dict_Dict);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,
void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict d,void*k);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Lineno_Pos{struct
_dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct _dynforward_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dynforward_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};
extern char Cyc_Position_Nocontext[14];struct Cyc_Xarray_Xarray{struct _RegionHandle*
r;struct _dynforward_ptr elmts;int num_elmts;};int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(
struct _RegionHandle*);int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple1*,struct _tuple1*);struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dynforward_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct
_dynforward_ptr*name);void*Cyc_Absyn_strctq(struct _tuple1*name);void*Cyc_Absyn_unionq_typ(
struct _tuple1*name);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual
tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dynforward_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dynforward_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple1*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(
struct _tuple1*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,
struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct
Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dynforward_ptr*lab,struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dynforward_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dynforward_ptr*);struct _dynforward_ptr*Cyc_Absyn_fieldname(
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
info);extern int Cyc_Absyn_no_regions;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct
_dynforward_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dynforward_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct
_dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct
Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
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
_dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dynforward_ptr,int*
is_dyneither_ptr);int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,
void*);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*,int*
is_forward_only);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest,int*is_forward_only);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,
void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dynforward,int*is_dyneither,void**elt_type);struct _tuple4{struct
Cyc_Absyn_Tqual f1;void*f2;};void*Cyc_Tcutil_snd_tqt(struct _tuple4*);struct
_tuple5{unsigned int f1;int f2;};struct _tuple5 Cyc_Evexp_eval_const_uint_exp(struct
Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
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
f1;};struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();
extern struct _dynforward_ptr Cyc_Toc_globals;extern int Cyc_noexpand_r;int Cyc_Toc_warn_bounds_checks=
0;int Cyc_Toc_warn_all_null_deref=0;unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;static struct Cyc_List_List*Cyc_Toc_result_decls=
0;struct Cyc_Toc_TocState{struct _DynRegionHandle*dyn;struct Cyc_List_List**
tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_Set_Set**tunions_so_far;
struct Cyc_Dict_Dict*xtunions_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*
temp_labels;};static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=0;struct _tuple6{
struct _tuple1*f1;struct _dynforward_ptr f2;};int Cyc_Toc_qvar_tag_cmp(struct _tuple6*
x,struct _tuple6*y){struct _tuple1*_tmp1;struct _dynforward_ptr _tmp2;struct _tuple6
_tmp0=*x;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;{struct _tuple1*_tmp4;struct
_dynforward_ptr _tmp5;struct _tuple6 _tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{int i=
Cyc_Absyn_qvar_cmp(_tmp1,_tmp4);if(i != 0)return i;return Cyc_strcmp((struct
_dynforward_ptr)_tmp2,(struct _dynforward_ptr)_tmp5);}}}struct _tuple7{struct Cyc_Absyn_Aggrdecl*
f1;void*f2;};void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(
struct _tuple1*)){struct _DynRegionHandle*_tmp7;struct Cyc_Dict_Dict*_tmp8;struct
Cyc_Toc_TocState _tmp6=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp7=_tmp6.dyn;_tmp8=_tmp6.aggrs_so_far;{struct _DynRegionFrame _tmp9;struct
_RegionHandle*d=_open_dynregion(& _tmp9,_tmp7);{struct _tuple7**v=((struct _tuple7**(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp8,q);if(v == 0){
void*_tmpA=type_maker(q);_npop_handler(0);return _tmpA;}else{struct _tuple7 _tmpC;
void*_tmpD;struct _tuple7*_tmpB=*v;_tmpC=*_tmpB;_tmpD=_tmpC.f2;{void*_tmpE=_tmpD;
_npop_handler(0);return _tmpE;}}};_pop_dynregion(d);}}static int Cyc_Toc_tuple_type_counter=
0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=0;
char Cyc_Toc_Toc_Unimplemented[22]="\000\000\000\000Toc_Unimplemented\000";char
Cyc_Toc_Toc_Impossible[19]="\000\000\000\000Toc_Impossible\000";static void*Cyc_Toc_unimp(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({
void*_tmpF=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp10="\n";_tag_dynforward(
_tmp10,sizeof(char),_get_zero_arr_size_char(_tmp10,2));}),_tag_dynforward(_tmpF,
sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((
void*)Cyc_Toc_Toc_Unimplemented);}static void*Cyc_Toc_toc_impos(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({
void*_tmp11=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp12="\n";_tag_dynforward(
_tmp12,sizeof(char),_get_zero_arr_size_char(_tmp12,2));}),_tag_dynforward(_tmp11,
sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((
void*)Cyc_Toc_Toc_Impossible);}char Cyc_Toc_Match_error[16]="\000\000\000\000Match_error\000";
static char _tmp13[5]="curr";static struct _dynforward_ptr Cyc_Toc_curr_string={
_tmp13,_tmp13 + 5};static struct _dynforward_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static char _tmp14[4]="tag";static struct _dynforward_ptr Cyc_Toc_tag_string={_tmp14,
_tmp14 + 4};static struct _dynforward_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static
char _tmp15[4]="val";static struct _dynforward_ptr Cyc_Toc_val_string={_tmp15,_tmp15
+ 4};static struct _dynforward_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char
_tmp16[14]="_handler_cons";static struct _dynforward_ptr Cyc_Toc__handler_cons_string={
_tmp16,_tmp16 + 14};static struct _dynforward_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;
static char _tmp17[8]="handler";static struct _dynforward_ptr Cyc_Toc_handler_string={
_tmp17,_tmp17 + 8};static struct _dynforward_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;
static char _tmp18[14]="_RegionHandle";static struct _dynforward_ptr Cyc_Toc__RegionHandle_string={
_tmp18,_tmp18 + 14};static struct _dynforward_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static char _tmp19[17]="_DynRegionHandle";static struct _dynforward_ptr Cyc_Toc__DynRegionHandle_string={
_tmp19,_tmp19 + 17};static struct _dynforward_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;
static char _tmp1A[16]="_DynRegionFrame";static struct _dynforward_ptr Cyc_Toc__DynRegionFrame_string={
_tmp1A,_tmp1A + 16};static struct _dynforward_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
struct _dynforward_ptr Cyc_Toc_globals={(void*)0,(void*)(0 + 0)};static char _tmp1B[7]="_throw";
static struct _dynforward_ptr Cyc_Toc__throw_str={_tmp1B,_tmp1B + 7};static struct
_tuple1 Cyc_Toc__throw_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,&
Cyc_Toc__throw_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={
0,(void*)((void*)& Cyc_Toc__throw_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp1E[7]="setjmp";
static struct _dynforward_ptr Cyc_Toc_setjmp_str={_tmp1E,_tmp1E + 7};static struct
_tuple1 Cyc_Toc_setjmp_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={1,&
Cyc_Toc_setjmp_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={
0,(void*)((void*)& Cyc_Toc_setjmp_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp21[
14]="_push_handler";static struct _dynforward_ptr Cyc_Toc__push_handler_str={_tmp21,
_tmp21 + 14};static struct _tuple1 Cyc_Toc__push_handler_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)((void*)& Cyc_Toc__push_handler_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=&
Cyc_Toc__push_handler_ev;static char _tmp24[13]="_pop_handler";static struct
_dynforward_ptr Cyc_Toc__pop_handler_str={_tmp24,_tmp24 + 13};static struct _tuple1
Cyc_Toc__pop_handler_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)((void*)& Cyc_Toc__pop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static
char _tmp27[12]="_exn_thrown";static struct _dynforward_ptr Cyc_Toc__exn_thrown_str={
_tmp27,_tmp27 + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)((void*)& Cyc_Toc__exn_thrown_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=&
Cyc_Toc__exn_thrown_ev;static char _tmp2A[14]="_npop_handler";static struct
_dynforward_ptr Cyc_Toc__npop_handler_str={_tmp2A,_tmp2A + 14};static struct _tuple1
Cyc_Toc__npop_handler_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={
1,& Cyc_Toc__npop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={
0,(void*)((void*)& Cyc_Toc__npop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;
static char _tmp2D[12]="_check_null";static struct _dynforward_ptr Cyc_Toc__check_null_str={
_tmp2D,_tmp2D + 12};static struct _tuple1 Cyc_Toc__check_null_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)((void*)& Cyc_Toc__check_null_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev;static char _tmp30[28]="_check_known_subscript_null";static
struct _dynforward_ptr Cyc_Toc__check_known_subscript_null_str={_tmp30,_tmp30 + 28};
static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_known_subscript_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp33[31]="_check_known_subscript_notnull";static struct
_dynforward_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp33,_tmp33 + 31};
static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)((void*)& Cyc_Toc__check_known_subscript_notnull_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp36[28]="_check_dynforward_subscript";
static struct _dynforward_ptr Cyc_Toc__check_dynforward_subscript_str={_tmp36,
_tmp36 + 28};static struct _tuple1 Cyc_Toc__check_dynforward_subscript_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_dynforward_subscript_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dynforward_subscript_re={1,& Cyc_Toc__check_dynforward_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dynforward_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dynforward_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dynforward_subscript_e=& Cyc_Toc__check_dynforward_subscript_ev;
static char _tmp39[27]="_check_dyneither_subscript";static struct _dynforward_ptr Cyc_Toc__check_dyneither_subscript_str={
_tmp39,_tmp39 + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_dyneither_subscript_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dyneither_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;
static char _tmp3C[16]="_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_str={
_tmp3C,_tmp3C + 16};static struct _tuple1 Cyc_Toc__dynforward_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dynforward_ptr_re={1,& Cyc_Toc__dynforward_ptr_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_e=&
Cyc_Toc__dynforward_ptr_ev;static char _tmp3F[15]="_dyneither_ptr";static struct
_dynforward_ptr Cyc_Toc__dyneither_ptr_str={_tmp3F,_tmp3F + 15};static struct
_tuple1 Cyc_Toc__dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_re={
1,& Cyc_Toc__dyneither_ptr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;
static char _tmp42[16]="_tag_dynforward";static struct _dynforward_ptr Cyc_Toc__tag_dynforward_str={
_tmp42,_tmp42 + 16};static struct _tuple1 Cyc_Toc__tag_dynforward_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__tag_dynforward_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tag_dynforward_re={1,& Cyc_Toc__tag_dynforward_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__tag_dynforward_ev={0,(void*)((void*)& Cyc_Toc__tag_dynforward_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dynforward_e=&
Cyc_Toc__tag_dynforward_ev;static char _tmp45[15]="_tag_dyneither";static struct
_dynforward_ptr Cyc_Toc__tag_dyneither_str={_tmp45,_tmp45 + 15};static struct
_tuple1 Cyc_Toc__tag_dyneither_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dyneither_re={
1,& Cyc_Toc__tag_dyneither_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={
0,(void*)((void*)& Cyc_Toc__tag_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;
static char _tmp48[21]="_init_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__init_dynforward_ptr_str={
_tmp48,_tmp48 + 21};static struct _tuple1 Cyc_Toc__init_dynforward_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__init_dynforward_ptr_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_dynforward_ptr_re={1,& Cyc_Toc__init_dynforward_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dynforward_ptr_ev={0,(
void*)((void*)& Cyc_Toc__init_dynforward_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__init_dynforward_ptr_e=& Cyc_Toc__init_dynforward_ptr_ev;
static char _tmp4B[20]="_init_dyneither_ptr";static struct _dynforward_ptr Cyc_Toc__init_dyneither_ptr_str={
_tmp4B,_tmp4B + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__init_dyneither_ptr_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__init_dyneither_ptr_re={1,& Cyc_Toc__init_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(
void*)((void*)& Cyc_Toc__init_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;
static char _tmp4E[22]="_untag_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__untag_dynforward_ptr_str={
_tmp4E,_tmp4E + 22};static struct _tuple1 Cyc_Toc__untag_dynforward_ptr_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__untag_dynforward_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dynforward_ptr_re={1,& Cyc_Toc__untag_dynforward_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dynforward_ptr_ev={0,(
void*)((void*)& Cyc_Toc__untag_dynforward_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dynforward_ptr_e=& Cyc_Toc__untag_dynforward_ptr_ev;
static char _tmp51[21]="_untag_dyneither_ptr";static struct _dynforward_ptr Cyc_Toc__untag_dyneither_ptr_str={
_tmp51,_tmp51 + 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__untag_dyneither_ptr_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(
void*)((void*)& Cyc_Toc__untag_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;
static char _tmp54[21]="_get_dynforward_size";static struct _dynforward_ptr Cyc_Toc__get_dynforward_size_str={
_tmp54,_tmp54 + 21};static struct _tuple1 Cyc_Toc__get_dynforward_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_dynforward_size_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_dynforward_size_re={1,& Cyc_Toc__get_dynforward_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dynforward_size_ev={0,(
void*)((void*)& Cyc_Toc__get_dynforward_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_dynforward_size_e=& Cyc_Toc__get_dynforward_size_ev;
static char _tmp57[20]="_get_dyneither_size";static struct _dynforward_ptr Cyc_Toc__get_dyneither_size_str={
_tmp57,_tmp57 + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_dyneither_size_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__get_dyneither_size_re={1,& Cyc_Toc__get_dyneither_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(
void*)((void*)& Cyc_Toc__get_dyneither_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;
static char _tmp5A[19]="_get_zero_arr_size";static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_str={
_tmp5A,_tmp5A + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;
static char _tmp5D[24]="_get_zero_arr_size_char";static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_char_str={
_tmp5D,_tmp5D + 24};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_char_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_char_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;
static char _tmp60[25]="_get_zero_arr_size_short";static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_short_str={
_tmp60,_tmp60 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_short_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;
static char _tmp63[23]="_get_zero_arr_size_int";static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_int_str={
_tmp63,_tmp63 + 23};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_int_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_int_re={1,& Cyc_Toc__get_zero_arr_size_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_int_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;
static char _tmp66[25]="_get_zero_arr_size_float";static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_float_str={
_tmp66,_tmp66 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_float_re={1,& Cyc_Toc__get_zero_arr_size_float_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_float_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;
static char _tmp69[26]="_get_zero_arr_size_double";static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_double_str={
_tmp69,_tmp69 + 26};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_double_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_double_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;
static char _tmp6C[30]="_get_zero_arr_size_longdouble";static struct _dynforward_ptr
Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp6C,_tmp6C + 30};static struct
_tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,&
Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_longdouble_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=&
Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp6F[28]="_get_zero_arr_size_voidstar";
static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp6F,
_tmp6F + 28};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_voidstar_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_voidstar_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;
static char _tmp72[21]="_dynforward_ptr_plus";static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_plus_str={
_tmp72,_tmp72 + 21};static struct _tuple1 Cyc_Toc__dynforward_ptr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_plus_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_plus_re={1,& Cyc_Toc__dynforward_ptr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_plus_ev={0,(
void*)((void*)& Cyc_Toc__dynforward_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_plus_e=& Cyc_Toc__dynforward_ptr_plus_ev;
static char _tmp75[20]="_dyneither_ptr_plus";static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_plus_str={
_tmp75,_tmp75 + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_plus_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(
void*)((void*)& Cyc_Toc__dyneither_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;
static char _tmp78[15]="_zero_arr_plus";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_str={
_tmp78,_tmp78 + 15};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=&
Cyc_Toc__zero_arr_plus_ev;static char _tmp7B[20]="_zero_arr_plus_char";static
struct _dynforward_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp7B,_tmp7B + 20};static
struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_char_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_char_re={1,& Cyc_Toc__zero_arr_plus_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_char_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;
static char _tmp7E[21]="_zero_arr_plus_short";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_short_str={
_tmp7E,_tmp7E + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_short_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_short_re={1,& Cyc_Toc__zero_arr_plus_short_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_short_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;
static char _tmp81[19]="_zero_arr_plus_int";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_int_str={
_tmp81,_tmp81 + 19};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_int_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_int_re={1,& Cyc_Toc__zero_arr_plus_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_int_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;
static char _tmp84[21]="_zero_arr_plus_float";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_float_str={
_tmp84,_tmp84 + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_float_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_float_re={1,& Cyc_Toc__zero_arr_plus_float_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_float_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;
static char _tmp87[22]="_zero_arr_plus_double";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_double_str={
_tmp87,_tmp87 + 22};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_double_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_double_re={1,& Cyc_Toc__zero_arr_plus_double_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_double_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;
static char _tmp8A[26]="_zero_arr_plus_longdouble";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_longdouble_str={
_tmp8A,_tmp8A + 26};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_plus_longdouble_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;
static char _tmp8D[24]="_zero_arr_plus_voidstar";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_voidstar_str={
_tmp8D,_tmp8D + 24};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_voidstar_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_voidstar_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;
static char _tmp90[29]="_dynforward_ptr_inplace_plus";static struct _dynforward_ptr
Cyc_Toc__dynforward_ptr_inplace_plus_str={_tmp90,_tmp90 + 29};static struct _tuple1
Cyc_Toc__dynforward_ptr_inplace_plus_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_inplace_plus_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_inplace_plus_re={1,& Cyc_Toc__dynforward_ptr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_inplace_plus_ev={
0,(void*)((void*)& Cyc_Toc__dynforward_ptr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_inplace_plus_e=& Cyc_Toc__dynforward_ptr_inplace_plus_ev;
static char _tmp93[28]="_dyneither_ptr_inplace_plus";static struct _dynforward_ptr
Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp93,_tmp93 + 28};static struct _tuple1
Cyc_Toc__dyneither_ptr_inplace_plus_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_inplace_plus_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;
static char _tmp96[23]="_zero_arr_inplace_plus";static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp96,_tmp96 + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;
static char _tmp99[28]="_zero_arr_inplace_plus_char";static struct _dynforward_ptr
Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp99,_tmp99 + 28};static struct _tuple1
Cyc_Toc__zero_arr_inplace_plus_char_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_char_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_char_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;
static char _tmp9C[29]="_zero_arr_inplace_plus_short";static struct _dynforward_ptr
Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp9C,_tmp9C + 29};static struct _tuple1
Cyc_Toc__zero_arr_inplace_plus_short_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_short_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_short_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_short_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;
static char _tmp9F[27]="_zero_arr_inplace_plus_int";static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={
_tmp9F,_tmp9F + 27};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_int_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_int_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;
static char _tmpA2[29]="_zero_arr_inplace_plus_float";static struct _dynforward_ptr
Cyc_Toc__zero_arr_inplace_plus_float_str={_tmpA2,_tmpA2 + 29};static struct _tuple1
Cyc_Toc__zero_arr_inplace_plus_float_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_float_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_float_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_float_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;
static char _tmpA5[30]="_zero_arr_inplace_plus_double";static struct _dynforward_ptr
Cyc_Toc__zero_arr_inplace_plus_double_str={_tmpA5,_tmpA5 + 30};static struct
_tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_double_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1,&
Cyc_Toc__zero_arr_inplace_plus_double_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_double_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=&
Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmpA8[34]="_zero_arr_inplace_plus_longdouble";
static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmpA8,
_tmpA8 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={
1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=&
Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmpAB[32]="_zero_arr_inplace_plus_voidstar";
static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmpAB,
_tmpAB + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,&
Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=&
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmpAE[34]="_dynforward_ptr_inplace_plus_post";
static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_inplace_plus_post_str={_tmpAE,
_tmpAE + 34};static struct _tuple1 Cyc_Toc__dynforward_ptr_inplace_plus_post_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_inplace_plus_post_re={
1,& Cyc_Toc__dynforward_ptr_inplace_plus_post_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_inplace_plus_post_e=&
Cyc_Toc__dynforward_ptr_inplace_plus_post_ev;static char _tmpB1[33]="_dyneither_ptr_inplace_plus_post";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmpB1,
_tmpB1 + 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,&
Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=&
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmpB4[25]="_dynforward_to_dyneither";
static struct _dynforward_ptr Cyc_Toc__dynforward_to_dyneither_str={_tmpB4,_tmpB4 + 
25};static struct _tuple1 Cyc_Toc__dynforward_to_dyneither_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_to_dyneither_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_to_dyneither_re={1,& Cyc_Toc__dynforward_to_dyneither_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_to_dyneither_ev={
0,(void*)((void*)& Cyc_Toc__dynforward_to_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_to_dyneither_e=& Cyc_Toc__dynforward_to_dyneither_ev;
static char _tmpB7[25]="_dyneither_to_dynforward";static struct _dynforward_ptr Cyc_Toc__dyneither_to_dynforward_str={
_tmpB7,_tmpB7 + 25};static struct _tuple1 Cyc_Toc__dyneither_to_dynforward_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_to_dynforward_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_to_dynforward_re={1,& Cyc_Toc__dyneither_to_dynforward_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_to_dynforward_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_to_dynforward_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_to_dynforward_e=& Cyc_Toc__dyneither_to_dynforward_ev;
static char _tmpBA[28]="_zero_arr_inplace_plus_post";static struct _dynforward_ptr
Cyc_Toc__zero_arr_inplace_plus_post_str={_tmpBA,_tmpBA + 28};static struct _tuple1
Cyc_Toc__zero_arr_inplace_plus_post_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmpBD[33]="_zero_arr_inplace_plus_post_char";static struct
_dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmpBD,_tmpBD + 33};
static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_char_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,&
Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=&
Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmpC0[34]="_zero_arr_inplace_plus_post_short";
static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmpC0,
_tmpC0 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=&
Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmpC3[32]="_zero_arr_inplace_plus_post_int";
static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmpC3,
_tmpC3 + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_int_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,&
Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=&
Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmpC6[34]="_zero_arr_inplace_plus_post_float";
static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmpC6,
_tmpC6 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=&
Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmpC9[35]="_zero_arr_inplace_plus_post_double";
static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={
_tmpC9,_tmpC9 + 35};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_double_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)((void*)&
Cyc_Toc__zero_arr_inplace_plus_post_double_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;
static char _tmpCC[39]="_zero_arr_inplace_plus_post_longdouble";static struct
_dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmpCC,_tmpCC
+ 39};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)((
void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;
static char _tmpCF[37]="_zero_arr_inplace_plus_post_voidstar";static struct
_dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmpCF,_tmpCF + 
37};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)((
void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;
static char _tmpD2[10]="_cycalloc";static struct _dynforward_ptr Cyc_Toc__cycalloc_str={
_tmpD2,_tmpD2 + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_re={1,& Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__cycalloc_ev={0,(void*)((void*)& Cyc_Toc__cycalloc_re),0,(void*)((void*)
Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;
static char _tmpD5[11]="_cyccalloc";static struct _dynforward_ptr Cyc_Toc__cyccalloc_str={
_tmpD5,_tmpD5 + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)((void*)& Cyc_Toc__cyccalloc_re),0,(
void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=&
Cyc_Toc__cyccalloc_ev;static char _tmpD8[17]="_cycalloc_atomic";static struct
_dynforward_ptr Cyc_Toc__cycalloc_atomic_str={_tmpD8,_tmpD8 + 17};static struct
_tuple1 Cyc_Toc__cycalloc_atomic_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_atomic_re={
1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={
0,(void*)((void*)& Cyc_Toc__cycalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;
static char _tmpDB[18]="_cyccalloc_atomic";static struct _dynforward_ptr Cyc_Toc__cyccalloc_atomic_str={
_tmpDB,_tmpDB + 18};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cyccalloc_atomic_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={1,& Cyc_Toc__cyccalloc_atomic_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)((
void*)& Cyc_Toc__cyccalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;
static char _tmpDE[15]="_region_malloc";static struct _dynforward_ptr Cyc_Toc__region_malloc_str={
_tmpDE,_tmpDE + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)((void*)& Cyc_Toc__region_malloc_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=&
Cyc_Toc__region_malloc_ev;static char _tmpE1[15]="_region_calloc";static struct
_dynforward_ptr Cyc_Toc__region_calloc_str={_tmpE1,_tmpE1 + 15};static struct
_tuple1 Cyc_Toc__region_calloc_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={
1,& Cyc_Toc__region_calloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={
0,(void*)((void*)& Cyc_Toc__region_calloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;
static char _tmpE4[13]="_check_times";static struct _dynforward_ptr Cyc_Toc__check_times_str={
_tmpE4,_tmpE4 + 13};static struct _tuple1 Cyc_Toc__check_times_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)((void*)& Cyc_Toc__check_times_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev;static char _tmpE7[12]="_new_region";static struct
_dynforward_ptr Cyc_Toc__new_region_str={_tmpE7,_tmpE7 + 12};static struct _tuple1
Cyc_Toc__new_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1,& Cyc_Toc__new_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={
0,(void*)((void*)& Cyc_Toc__new_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static
char _tmpEA[13]="_push_region";static struct _dynforward_ptr Cyc_Toc__push_region_str={
_tmpEA,_tmpEA + 13};static struct _tuple1 Cyc_Toc__push_region_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)((void*)& Cyc_Toc__push_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev;static char _tmpED[12]="_pop_region";static struct
_dynforward_ptr Cyc_Toc__pop_region_str={_tmpED,_tmpED + 12};static struct _tuple1
Cyc_Toc__pop_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1,& Cyc_Toc__pop_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={
0,(void*)((void*)& Cyc_Toc__pop_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static
char _tmpF0[16]="_open_dynregion";static struct _dynforward_ptr Cyc_Toc__open_dynregion_str={
_tmpF0,_tmpF0 + 16};static struct _tuple1 Cyc_Toc__open_dynregion_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__open_dynregion_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)((void*)& Cyc_Toc__open_dynregion_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=&
Cyc_Toc__open_dynregion_ev;static char _tmpF3[16]="_push_dynregion";static struct
_dynforward_ptr Cyc_Toc__push_dynregion_str={_tmpF3,_tmpF3 + 16};static struct
_tuple1 Cyc_Toc__push_dynregion_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_dynregion_re={
1,& Cyc_Toc__push_dynregion_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={
0,(void*)((void*)& Cyc_Toc__push_dynregion_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;
static char _tmpF6[15]="_pop_dynregion";static struct _dynforward_ptr Cyc_Toc__pop_dynregion_str={
_tmpF6,_tmpF6 + 15};static struct _tuple1 Cyc_Toc__pop_dynregion_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__pop_dynregion_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)((void*)& Cyc_Toc__pop_dynregion_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=&
Cyc_Toc__pop_dynregion_ev;static char _tmpF9[14]="_reset_region";static struct
_dynforward_ptr Cyc_Toc__reset_region_str={_tmpF9,_tmpF9 + 14};static struct _tuple1
Cyc_Toc__reset_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__reset_region_re={
1,& Cyc_Toc__reset_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={
0,(void*)((void*)& Cyc_Toc__reset_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;
static char _tmpFC[19]="_throw_arraybounds";static struct _dynforward_ptr Cyc_Toc__throw_arraybounds_str={
_tmpFC,_tmpFC + 19};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__throw_arraybounds_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(
void*)((void*)& Cyc_Toc__throw_arraybounds_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmpFF[30]="_dynforward_ptr_decrease_size";static struct _dynforward_ptr
Cyc_Toc__dynforward_ptr_decrease_size_str={_tmpFF,_tmpFF + 30};static struct
_tuple1 Cyc_Toc__dynforward_ptr_decrease_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_decrease_size_re={1,&
Cyc_Toc__dynforward_ptr_decrease_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dynforward_ptr_decrease_size_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_decrease_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_decrease_size_e=&
Cyc_Toc__dynforward_ptr_decrease_size_ev;static char _tmp102[29]="_dyneither_ptr_decrease_size";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmp102,
_tmp102 + 29};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,&
Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_decrease_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=&
Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmp105[11]="_swap_word";
static struct _dynforward_ptr Cyc_Toc__swap_word_str={_tmp105,_tmp105 + 11};static
struct _tuple1 Cyc_Toc__swap_word_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_word_re={
1,& Cyc_Toc__swap_word_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={
0,(void*)((void*)& Cyc_Toc__swap_word_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char
_tmp108[17]="_swap_dynforward";static struct _dynforward_ptr Cyc_Toc__swap_dynforward_str={
_tmp108,_tmp108 + 17};static struct _tuple1 Cyc_Toc__swap_dynforward_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__swap_dynforward_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__swap_dynforward_re={1,& Cyc_Toc__swap_dynforward_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__swap_dynforward_ev={0,(void*)((void*)& Cyc_Toc__swap_dynforward_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dynforward_e=&
Cyc_Toc__swap_dynforward_ev;static char _tmp10B[16]="_swap_dyneither";static struct
_dynforward_ptr Cyc_Toc__swap_dyneither_str={_tmp10B,_tmp10B + 16};static struct
_tuple1 Cyc_Toc__swap_dyneither_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__swap_dyneither_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_dyneither_re={
1,& Cyc_Toc__swap_dyneither_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={
0,(void*)((void*)& Cyc_Toc__swap_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;
static struct Cyc_Absyn_AggrType_struct Cyc_Toc_dynforward_ptr_typ_v={10,{(union Cyc_Absyn_AggrInfoU_union)((
struct Cyc_Absyn_UnknownAggr_struct){0,(void*)((void*)0),& Cyc_Toc__dynforward_ptr_pr}),
0}};static void*Cyc_Toc_dynforward_ptr_typ=(void*)& Cyc_Toc_dynforward_ptr_typ_v;
static struct Cyc_Absyn_AggrType_struct Cyc_Toc_dyneither_ptr_typ_v={10,{(union Cyc_Absyn_AggrInfoU_union)((
struct Cyc_Absyn_UnknownAggr_struct){0,(void*)((void*)0),& Cyc_Toc__dyneither_ptr_pr}),
0}};static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};static struct Cyc_Absyn_Stmt*
Cyc_Toc_skip_stmt_dl(){static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(
skip_stmt_opt == 0)skip_stmt_opt=({struct Cyc_Absyn_Stmt**_tmp112=_cycalloc(
sizeof(*_tmp112));_tmp112[0]=Cyc_Absyn_skip_stmt(0);_tmp112;});return*
skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*
e){return Cyc_Absyn_cast_exp(t,e,0,(void*)1,0);}static void*Cyc_Toc_cast_it_r(void*
t,struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Cast_e_struct*_tmp113=
_cycalloc(sizeof(*_tmp113));_tmp113[0]=({struct Cyc_Absyn_Cast_e_struct _tmp114;
_tmp114.tag=15;_tmp114.f1=(void*)t;_tmp114.f2=e;_tmp114.f3=0;_tmp114.f4=(void*)((
void*)1);_tmp114;});_tmp113;});}static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*
e){return(void*)({struct Cyc_Absyn_Deref_e_struct*_tmp115=_cycalloc(sizeof(*
_tmp115));_tmp115[0]=({struct Cyc_Absyn_Deref_e_struct _tmp116;_tmp116.tag=22;
_tmp116.f1=e;_tmp116;});_tmp115;});}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){return(void*)({struct Cyc_Absyn_Subscript_e_struct*
_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117[0]=({struct Cyc_Absyn_Subscript_e_struct
_tmp118;_tmp118.tag=25;_tmp118.f1=e1;_tmp118.f2=e2;_tmp118;});_tmp117;});}static
void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){return(void*)({struct Cyc_Absyn_StmtExp_e_struct*
_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119[0]=({struct Cyc_Absyn_StmtExp_e_struct
_tmp11A;_tmp11A.tag=38;_tmp11A.f1=s;_tmp11A;});_tmp119;});}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t){return(void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp11B=_cycalloc(
sizeof(*_tmp11B));_tmp11B[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct _tmp11C;_tmp11C.tag=
18;_tmp11C.f1=(void*)t;_tmp11C;});_tmp11B;});}static void*Cyc_Toc_fncall_exp_r(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){return(void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp11E;_tmp11E.tag=11;_tmp11E.f1=e;_tmp11E.f2=es;_tmp11E.f3=0;_tmp11E;});
_tmp11D;});}static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){return(void*)({
struct Cyc_Absyn_Exp_s_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F[0]=({
struct Cyc_Absyn_Exp_s_struct _tmp120;_tmp120.tag=0;_tmp120.f1=e;_tmp120;});
_tmp11F;});}static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2){return(void*)({struct Cyc_Absyn_Seq_s_struct*_tmp121=_cycalloc(sizeof(*
_tmp121));_tmp121[0]=({struct Cyc_Absyn_Seq_s_struct _tmp122;_tmp122.tag=1;_tmp122.f1=
s1;_tmp122.f2=s2;_tmp122;});_tmp121;});}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){return(
void*)({struct Cyc_Absyn_Conditional_e_struct*_tmp123=_cycalloc(sizeof(*_tmp123));
_tmp123[0]=({struct Cyc_Absyn_Conditional_e_struct _tmp124;_tmp124.tag=6;_tmp124.f1=
e1;_tmp124.f2=e2;_tmp124.f3=e3;_tmp124;});_tmp123;});}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*n){return(void*)({struct Cyc_Absyn_AggrMember_e_struct*
_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125[0]=({struct Cyc_Absyn_AggrMember_e_struct
_tmp126;_tmp126.tag=23;_tmp126.f1=e;_tmp126.f2=n;_tmp126;});_tmp125;});}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*n){
return(void*)({struct Cyc_Absyn_AggrArrow_e_struct*_tmp127=_cycalloc(sizeof(*
_tmp127));_tmp127[0]=({struct Cyc_Absyn_AggrArrow_e_struct _tmp128;_tmp128.tag=24;
_tmp128.f1=e;_tmp128.f2=n;_tmp128;});_tmp127;});}static void*Cyc_Toc_unresolvedmem_exp_r(
struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){return(void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp12A;_tmp12A.tag=37;_tmp12A.f1=tdopt;_tmp12A.f2=ds;_tmp12A;});_tmp129;});}
static void*Cyc_Toc_goto_stmt_r(struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_Goto_s_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));
_tmp12B[0]=({struct Cyc_Absyn_Goto_s_struct _tmp12C;_tmp12C.tag=7;_tmp12C.f1=v;
_tmp12C.f2=s;_tmp12C;});_tmp12B;});}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={
0,(union Cyc_Absyn_Cnst_union)((struct Cyc_Absyn_Int_c_struct){2,(void*)((void*)0),
0})};struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*
fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*
fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};struct
Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,&
Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,&
Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,&
Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,&
Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,&
Cyc_Toc__get_zero_arr_size_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(
struct Cyc_Toc_functionSet*fS,void*t){struct Cyc_Absyn_Exp*function;{void*_tmp12F=
t;void*_tmp130;int _tmp131;_LL1: if(_tmp12F <= (void*)4)goto _LL3;if(*((int*)_tmp12F)
!= 5)goto _LL3;_tmp130=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp12F)->f2;_LL2:{
void*_tmp132=_tmp130;_LLC: if((int)_tmp132 != 0)goto _LLE;_LLD: function=fS->fchar;
goto _LLB;_LLE: if((int)_tmp132 != 1)goto _LL10;_LLF: function=fS->fshort;goto _LLB;
_LL10: if((int)_tmp132 != 2)goto _LL12;_LL11: function=fS->fint;goto _LLB;_LL12:;
_LL13:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp133=_cycalloc(
sizeof(*_tmp133));_tmp133[0]=({struct Cyc_Core_Impossible_struct _tmp134;_tmp134.tag=
Cyc_Core_Impossible;_tmp134.f1=({const char*_tmp135="impossible IntType (not char, short or int)";
_tag_dynforward(_tmp135,sizeof(char),_get_zero_arr_size_char(_tmp135,44));});
_tmp134;});_tmp133;}));_LLB:;}goto _LL0;_LL3: if((int)_tmp12F != 1)goto _LL5;_LL4:
function=fS->ffloat;goto _LL0;_LL5: if(_tmp12F <= (void*)4)goto _LL9;if(*((int*)
_tmp12F)!= 6)goto _LL7;_tmp131=((struct Cyc_Absyn_DoubleType_struct*)_tmp12F)->f1;
_LL6: switch(_tmp131){case 1: _LL14: function=fS->flongdouble;break;default: _LL15:
function=fS->fdouble;}goto _LL0;_LL7: if(*((int*)_tmp12F)!= 4)goto _LL9;_LL8:
function=fS->fvoidstar;goto _LL0;_LL9:;_LLA:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136[0]=({struct Cyc_Core_Impossible_struct
_tmp137;_tmp137.tag=Cyc_Core_Impossible;_tmp137.f1=({const char*_tmp138="impossible expression type (not int, float, double, or pointer)";
_tag_dynforward(_tmp138,sizeof(char),_get_zero_arr_size_char(_tmp138,64));});
_tmp137;});_tmp136;}));_LL0:;}return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(
fS,Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v));}
struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*
fS,struct Cyc_Absyn_Exp*arr){void*_tmp139=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmp13A;void*_tmp13B;_LL18:
if(_tmp139 <= (void*)4)goto _LL1A;if(*((int*)_tmp139)!= 4)goto _LL1A;_tmp13A=((
struct Cyc_Absyn_PointerType_struct*)_tmp139)->f1;_tmp13B=(void*)_tmp13A.elt_typ;
_LL19: return Cyc_Toc_getFunctionType(fS,_tmp13B);_LL1A:;_LL1B:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C[0]=({
struct Cyc_Core_Impossible_struct _tmp13D;_tmp13D.tag=Cyc_Core_Impossible;_tmp13D.f1=({
const char*_tmp13E="impossible type (not pointer)";_tag_dynforward(_tmp13E,
sizeof(char),_get_zero_arr_size_char(_tmp13E,30));});_tmp13D;});_tmp13C;}));
_LL17:;}struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int
Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){void*_tmp13F=(void*)e->r;union Cyc_Absyn_Cnst_union
_tmp140;char _tmp141;union Cyc_Absyn_Cnst_union _tmp142;short _tmp143;union Cyc_Absyn_Cnst_union
_tmp144;int _tmp145;union Cyc_Absyn_Cnst_union _tmp146;long long _tmp147;union Cyc_Absyn_Cnst_union
_tmp148;struct Cyc_Absyn_Exp*_tmp149;struct Cyc_List_List*_tmp14A;struct Cyc_List_List*
_tmp14B;struct Cyc_List_List*_tmp14C;struct Cyc_List_List*_tmp14D;struct Cyc_List_List*
_tmp14E;_LL1D: if(*((int*)_tmp13F)!= 0)goto _LL1F;_tmp140=((struct Cyc_Absyn_Const_e_struct*)
_tmp13F)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp13F)->f1).Char_c).tag != 
0)goto _LL1F;_tmp141=(_tmp140.Char_c).f2;_LL1E: return _tmp141 == '\000';_LL1F: if(*((
int*)_tmp13F)!= 0)goto _LL21;_tmp142=((struct Cyc_Absyn_Const_e_struct*)_tmp13F)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp13F)->f1).Short_c).tag != 1)goto _LL21;
_tmp143=(_tmp142.Short_c).f2;_LL20: return _tmp143 == 0;_LL21: if(*((int*)_tmp13F)!= 
0)goto _LL23;_tmp144=((struct Cyc_Absyn_Const_e_struct*)_tmp13F)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmp13F)->f1).Int_c).tag != 2)goto _LL23;_tmp145=(
_tmp144.Int_c).f2;_LL22: return _tmp145 == 0;_LL23: if(*((int*)_tmp13F)!= 0)goto
_LL25;_tmp146=((struct Cyc_Absyn_Const_e_struct*)_tmp13F)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp13F)->f1).LongLong_c).tag != 3)goto _LL25;_tmp147=(_tmp146.LongLong_c).f2;
_LL24: return _tmp147 == 0;_LL25: if(*((int*)_tmp13F)!= 0)goto _LL27;_tmp148=((struct
Cyc_Absyn_Const_e_struct*)_tmp13F)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp13F)->f1).Null_c).tag != 6)goto _LL27;_LL26: return 1;_LL27: if(*((int*)_tmp13F)
!= 15)goto _LL29;_tmp149=((struct Cyc_Absyn_Cast_e_struct*)_tmp13F)->f2;_LL28:
return Cyc_Toc_is_zero(_tmp149);_LL29: if(*((int*)_tmp13F)!= 26)goto _LL2B;_tmp14A=((
struct Cyc_Absyn_Tuple_e_struct*)_tmp13F)->f1;_LL2A: return((int(*)(int(*pred)(
struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,
_tmp14A);_LL2B: if(*((int*)_tmp13F)!= 28)goto _LL2D;_tmp14B=((struct Cyc_Absyn_Array_e_struct*)
_tmp13F)->f1;_LL2C: _tmp14C=_tmp14B;goto _LL2E;_LL2D: if(*((int*)_tmp13F)!= 30)goto
_LL2F;_tmp14C=((struct Cyc_Absyn_Struct_e_struct*)_tmp13F)->f3;_LL2E: _tmp14D=
_tmp14C;goto _LL30;_LL2F: if(*((int*)_tmp13F)!= 27)goto _LL31;_tmp14D=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp13F)->f2;_LL30: _tmp14E=_tmp14D;goto _LL32;_LL31: if(*((int*)_tmp13F)!= 37)goto
_LL33;_tmp14E=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp13F)->f2;_LL32: for(0;
_tmp14E != 0;_tmp14E=_tmp14E->tl){if(!Cyc_Toc_is_zero((*((struct _tuple8*)_tmp14E->hd)).f2))
return 0;}return 1;_LL33:;_LL34: return 0;_LL1C:;}static int Cyc_Toc_is_nullable(void*
t){void*_tmp14F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp150;struct Cyc_Absyn_PtrAtts
_tmp151;struct Cyc_Absyn_Conref*_tmp152;_LL36: if(_tmp14F <= (void*)4)goto _LL38;if(*((
int*)_tmp14F)!= 4)goto _LL38;_tmp150=((struct Cyc_Absyn_PointerType_struct*)
_tmp14F)->f1;_tmp151=_tmp150.ptr_atts;_tmp152=_tmp151.nullable;_LL37: return((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp152);_LL38:;_LL39:({
void*_tmp153=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp154="is_nullable";_tag_dynforward(_tmp154,sizeof(char),
_get_zero_arr_size_char(_tmp154,12));}),_tag_dynforward(_tmp153,sizeof(void*),0));});
_LL35:;}static char _tmp161[1]="";static char _tmp163[8]="*bogus*";static struct
_tuple1*Cyc_Toc_collapse_qvar_tag(struct _tuple1*x,struct _dynforward_ptr tag){
struct _DynRegionHandle*_tmp156;struct Cyc_Dict_Dict*_tmp157;struct Cyc_Toc_TocState
_tmp155=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp156=
_tmp155.dyn;_tmp157=_tmp155.qvar_tags;{static struct _dynforward_ptr bogus_string={
_tmp163,_tmp163 + 8};static struct _tuple1 bogus_qvar={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& bogus_string};static struct _tuple6 pair={&
bogus_qvar,{_tmp161,_tmp161 + 1}};pair=({struct _tuple6 _tmp158;_tmp158.f1=x;
_tmp158.f2=tag;_tmp158;});{struct _DynRegionFrame _tmp159;struct _RegionHandle*d=
_open_dynregion(& _tmp159,_tmp156);{struct _tuple1**_tmp15A=((struct _tuple1**(*)(
struct Cyc_Dict_Dict d,struct _tuple6*k))Cyc_Dict_lookup_opt)(*_tmp157,(struct
_tuple6*)& pair);if(_tmp15A != 0){struct _tuple1*_tmp15B=*_tmp15A;_npop_handler(0);
return _tmp15B;}{struct _tuple6*_tmp15C=({struct _tuple6*_tmp160=_cycalloc(sizeof(*
_tmp160));_tmp160->f1=x;_tmp160->f2=tag;_tmp160;});struct _tuple1*res=({struct
_tuple1*_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E->f1=(*x).f1;_tmp15E->f2=({
struct _dynforward_ptr*_tmp15F=_cycalloc(sizeof(*_tmp15F));_tmp15F[0]=(struct
_dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)*(*x).f2,(struct
_dynforward_ptr)tag);_tmp15F;});_tmp15E;});*_tmp157=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple6*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp157,(
struct _tuple6*)_tmp15C,res);{struct _tuple1*_tmp15D=res;_npop_handler(0);return
_tmp15D;}}};_pop_dynregion(d);}}}struct _tuple9{void*f1;struct Cyc_List_List*f2;};
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){struct
_DynRegionHandle*_tmp165;struct Cyc_List_List**_tmp166;struct Cyc_Toc_TocState
_tmp164=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp165=
_tmp164.dyn;_tmp166=_tmp164.tuple_types;{struct _DynRegionFrame _tmp167;struct
_RegionHandle*d=_open_dynregion(& _tmp167,_tmp165);{struct Cyc_List_List*_tmp168=*
_tmp166;for(0;_tmp168 != 0;_tmp168=_tmp168->tl){struct _tuple9 _tmp16A;void*_tmp16B;
struct Cyc_List_List*_tmp16C;struct _tuple9*_tmp169=(struct _tuple9*)_tmp168->hd;
_tmp16A=*_tmp169;_tmp16B=_tmp16A.f1;_tmp16C=_tmp16A.f2;{struct Cyc_List_List*
_tmp16D=tqs0;for(0;_tmp16D != 0  && _tmp16C != 0;(_tmp16D=_tmp16D->tl,_tmp16C=
_tmp16C->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple4*)_tmp16D->hd)).f2,(void*)
_tmp16C->hd))break;}if(_tmp16D == 0  && _tmp16C == 0){void*_tmp16E=_tmp16B;
_npop_handler(0);return _tmp16E;}}}}{struct _dynforward_ptr*xname=({struct
_dynforward_ptr*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D[0]=(struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp180;_tmp180.tag=1;_tmp180.f1=(
unsigned long)Cyc_Toc_tuple_type_counter ++;{void*_tmp17E[1]={& _tmp180};Cyc_aprintf(({
const char*_tmp17F="_tuple%d";_tag_dynforward(_tmp17F,sizeof(char),
_get_zero_arr_size_char(_tmp17F,9));}),_tag_dynforward(_tmp17E,sizeof(void*),1));}});
_tmp17D;});void*x=Cyc_Absyn_strct(xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(
d,Cyc_Tcutil_snd_tqt,tqs0);struct Cyc_List_List*_tmp16F=0;struct Cyc_List_List*ts2=
ts;{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){_tmp16F=({struct Cyc_List_List*
_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->hd=({struct Cyc_Absyn_Aggrfield*
_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->name=Cyc_Absyn_fieldname(i);_tmp171->tq=
Cyc_Toc_mt_tq;_tmp171->type=(void*)((void*)ts2->hd);_tmp171->width=0;_tmp171->attributes=
0;_tmp171;});_tmp170->tl=_tmp16F;_tmp170;});}}_tmp16F=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp16F);{struct Cyc_Absyn_Aggrdecl*
_tmp172=({struct Cyc_Absyn_Aggrdecl*_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179->kind=(
void*)((void*)0);_tmp179->sc=(void*)((void*)2);_tmp179->name=({struct _tuple1*
_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp17C;(_tmp17C.Rel_n).tag=1;(_tmp17C.Rel_n).f1=0;
_tmp17C;});_tmp17B->f2=xname;_tmp17B;});_tmp179->tvs=0;_tmp179->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A->exist_vars=0;_tmp17A->rgn_po=0;
_tmp17A->fields=_tmp16F;_tmp17A;});_tmp179->attributes=0;_tmp179;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp173=_cycalloc(sizeof(*_tmp173));_tmp173->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp174=_cycalloc(sizeof(*_tmp174));
_tmp174[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp175;_tmp175.tag=4;_tmp175.f1=
_tmp172;_tmp175;});_tmp174;}),0);_tmp173->tl=Cyc_Toc_result_decls;_tmp173;});*
_tmp166=({struct Cyc_List_List*_tmp176=_region_malloc(d,sizeof(*_tmp176));_tmp176->hd=({
struct _tuple9*_tmp177=_region_malloc(d,sizeof(*_tmp177));_tmp177->f1=x;_tmp177->f2=
ts;_tmp177;});_tmp176->tl=*_tmp166;_tmp176;});{void*_tmp178=x;_npop_handler(0);
return _tmp178;}}};_pop_dynregion(d);}}struct _tuple1*Cyc_Toc_temp_var(){int
_tmp181=Cyc_Toc_temp_var_counter ++;struct _tuple1*res=({struct _tuple1*_tmp182=
_cycalloc(sizeof(*_tmp182));_tmp182->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp187;(_tmp187.Loc_n).tag=0;_tmp187;});_tmp182->f2=({struct _dynforward_ptr*
_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183[0]=(struct _dynforward_ptr)({struct
Cyc_Int_pa_struct _tmp186;_tmp186.tag=1;_tmp186.f1=(unsigned int)_tmp181;{void*
_tmp184[1]={& _tmp186};Cyc_aprintf(({const char*_tmp185="_tmp%X";_tag_dynforward(
_tmp185,sizeof(char),_get_zero_arr_size_char(_tmp185,7));}),_tag_dynforward(
_tmp184,sizeof(void*),1));}});_tmp183;});_tmp182;});return res;}static struct
_dynforward_ptr*Cyc_Toc_fresh_label(){struct _DynRegionHandle*_tmp189;struct Cyc_Xarray_Xarray*
_tmp18A;struct Cyc_Toc_TocState _tmp188=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp189=_tmp188.dyn;_tmp18A=_tmp188.temp_labels;{struct _DynRegionFrame _tmp18B;
struct _RegionHandle*d=_open_dynregion(& _tmp18B,_tmp189);{int _tmp18C=Cyc_Toc_fresh_label_counter
++;if(_tmp18C < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp18A)){
struct _dynforward_ptr*_tmp18D=((struct _dynforward_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp18A,_tmp18C);_npop_handler(0);return _tmp18D;}{struct
_dynforward_ptr*res=({struct _dynforward_ptr*_tmp191=_cycalloc(sizeof(*_tmp191));
_tmp191[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp194;_tmp194.tag=1;
_tmp194.f1=(unsigned int)_tmp18C;{void*_tmp192[1]={& _tmp194};Cyc_aprintf(({const
char*_tmp193="_LL%X";_tag_dynforward(_tmp193,sizeof(char),
_get_zero_arr_size_char(_tmp193,6));}),_tag_dynforward(_tmp192,sizeof(void*),1));}});
_tmp191;});if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dynforward_ptr*))Cyc_Xarray_add_ind)(
_tmp18A,res)!= _tmp18C)({void*_tmp18E=0;((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp18F="fresh_label: add_ind returned bad index...";
_tag_dynforward(_tmp18F,sizeof(char),_get_zero_arr_size_char(_tmp18F,43));}),
_tag_dynforward(_tmp18E,sizeof(void*),0));});{struct _dynforward_ptr*_tmp190=res;
_npop_handler(0);return _tmp190;}}};_pop_dynregion(d);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_tunion_tag(struct Cyc_Absyn_Tuniondecl*td,struct _tuple1*name,int
carries_value){int ans=0;struct Cyc_List_List*_tmp195=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp195))->hd)->name)
!= 0){if((td->is_flat  || carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmp195->hd)->typs
!= 0) || !carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmp195->hd)->typs == 0)
++ ans;_tmp195=_tmp195->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}static
int Cyc_Toc_num_void_tags(struct Cyc_Absyn_Tuniondecl*td){int ans=0;{struct Cyc_List_List*
_tmp196=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
for(0;_tmp196 != 0;_tmp196=_tmp196->tl){if(((struct Cyc_Absyn_Tunionfield*)_tmp196->hd)->typs
== 0)++ ans;}}return ans;}static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple2*
Cyc_Toc_arg_to_c(struct _tuple2*a){struct Cyc_Core_Opt*_tmp198;struct Cyc_Absyn_Tqual
_tmp199;void*_tmp19A;struct _tuple2 _tmp197=*a;_tmp198=_tmp197.f1;_tmp199=_tmp197.f2;
_tmp19A=_tmp197.f3;return({struct _tuple2*_tmp19B=_cycalloc(sizeof(*_tmp19B));
_tmp19B->f1=_tmp198;_tmp19B->f2=_tmp199;_tmp19B->f3=Cyc_Toc_typ_to_c(_tmp19A);
_tmp19B;});}static struct _tuple4*Cyc_Toc_typ_to_c_f(struct _tuple4*x){struct Cyc_Absyn_Tqual
_tmp19D;void*_tmp19E;struct _tuple4 _tmp19C=*x;_tmp19D=_tmp19C.f1;_tmp19E=_tmp19C.f2;
return({struct _tuple4*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F->f1=_tmp19D;
_tmp19F->f2=Cyc_Toc_typ_to_c(_tmp19E);_tmp19F;});}static void*Cyc_Toc_typ_to_c_array(
void*t){void*_tmp1A0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp1A1;
void*_tmp1A2;struct Cyc_Absyn_Tqual _tmp1A3;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_Absyn_Conref*
_tmp1A5;struct Cyc_Position_Segment*_tmp1A6;struct Cyc_Core_Opt*_tmp1A7;struct Cyc_Core_Opt
_tmp1A8;void*_tmp1A9;_LL3B: if(_tmp1A0 <= (void*)4)goto _LL3F;if(*((int*)_tmp1A0)!= 
7)goto _LL3D;_tmp1A1=((struct Cyc_Absyn_ArrayType_struct*)_tmp1A0)->f1;_tmp1A2=(
void*)_tmp1A1.elt_type;_tmp1A3=_tmp1A1.tq;_tmp1A4=_tmp1A1.num_elts;_tmp1A5=
_tmp1A1.zero_term;_tmp1A6=_tmp1A1.zt_loc;_LL3C: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp1A2),_tmp1A3,_tmp1A4,Cyc_Absyn_false_conref,_tmp1A6);_LL3D: if(*((int*)
_tmp1A0)!= 0)goto _LL3F;_tmp1A7=((struct Cyc_Absyn_Evar_struct*)_tmp1A0)->f2;if(
_tmp1A7 == 0)goto _LL3F;_tmp1A8=*_tmp1A7;_tmp1A9=(void*)_tmp1A8.v;_LL3E: return Cyc_Toc_typ_to_c_array(
_tmp1A9);_LL3F:;_LL40: return Cyc_Toc_typ_to_c(t);_LL3A:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){return({struct Cyc_Absyn_Aggrfield*
_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA->name=f->name;_tmp1AA->tq=Cyc_Toc_mt_tq;
_tmp1AA->type=(void*)Cyc_Toc_typ_to_c((void*)f->type);_tmp1AA->width=f->width;
_tmp1AA->attributes=f->attributes;_tmp1AA;});}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs){return;}static void*Cyc_Toc_char_star_typ(){static void**
cs=0;if(cs == 0)cs=({void**_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB[0]=Cyc_Absyn_star_typ(
Cyc_Absyn_char_typ,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);_tmp1AB;});
return*cs;}static void*Cyc_Toc_rgn_typ(){static void**r=0;if(r == 0)r=({void**
_tmp1AC=_cycalloc(sizeof(*_tmp1AC));_tmp1AC[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(
Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq);_tmp1AC;});return*r;}static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;if(r == 0)r=({void**_tmp1AD=_cycalloc(sizeof(*_tmp1AD));_tmp1AD[0]=
Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq);
_tmp1AD;});return*r;}static void*Cyc_Toc_typ_to_c(void*t){void*_tmp1AE=t;struct
Cyc_Core_Opt*_tmp1AF;struct Cyc_Core_Opt*_tmp1B0;struct Cyc_Core_Opt _tmp1B1;void*
_tmp1B2;struct Cyc_Absyn_Tvar*_tmp1B3;struct Cyc_Absyn_TunionInfo _tmp1B4;union Cyc_Absyn_TunionInfoU_union
_tmp1B5;struct Cyc_Absyn_Tuniondecl**_tmp1B6;struct Cyc_Absyn_Tuniondecl*_tmp1B7;
struct Cyc_Absyn_TunionFieldInfo _tmp1B8;union Cyc_Absyn_TunionFieldInfoU_union
_tmp1B9;struct Cyc_Absyn_Tuniondecl*_tmp1BA;struct Cyc_Absyn_Tunionfield*_tmp1BB;
struct Cyc_Absyn_PtrInfo _tmp1BC;void*_tmp1BD;struct Cyc_Absyn_Tqual _tmp1BE;struct
Cyc_Absyn_PtrAtts _tmp1BF;struct Cyc_Absyn_Conref*_tmp1C0;struct Cyc_Absyn_ArrayInfo
_tmp1C1;void*_tmp1C2;struct Cyc_Absyn_Tqual _tmp1C3;struct Cyc_Absyn_Exp*_tmp1C4;
struct Cyc_Position_Segment*_tmp1C5;struct Cyc_Absyn_FnInfo _tmp1C6;void*_tmp1C7;
struct Cyc_List_List*_tmp1C8;int _tmp1C9;struct Cyc_Absyn_VarargInfo*_tmp1CA;struct
Cyc_List_List*_tmp1CB;struct Cyc_List_List*_tmp1CC;void*_tmp1CD;struct Cyc_List_List*
_tmp1CE;struct Cyc_Absyn_AggrInfo _tmp1CF;union Cyc_Absyn_AggrInfoU_union _tmp1D0;
struct Cyc_List_List*_tmp1D1;struct _tuple1*_tmp1D2;struct Cyc_List_List*_tmp1D3;
struct _tuple1*_tmp1D4;struct Cyc_List_List*_tmp1D5;struct Cyc_Absyn_Typedefdecl*
_tmp1D6;void**_tmp1D7;void*_tmp1D8;_LL42: if((int)_tmp1AE != 0)goto _LL44;_LL43:
return t;_LL44: if(_tmp1AE <= (void*)4)goto _LL56;if(*((int*)_tmp1AE)!= 0)goto _LL46;
_tmp1AF=((struct Cyc_Absyn_Evar_struct*)_tmp1AE)->f2;if(_tmp1AF != 0)goto _LL46;
_LL45: return Cyc_Absyn_sint_typ;_LL46: if(*((int*)_tmp1AE)!= 0)goto _LL48;_tmp1B0=((
struct Cyc_Absyn_Evar_struct*)_tmp1AE)->f2;if(_tmp1B0 == 0)goto _LL48;_tmp1B1=*
_tmp1B0;_tmp1B2=(void*)_tmp1B1.v;_LL47: return Cyc_Toc_typ_to_c(_tmp1B2);_LL48: if(*((
int*)_tmp1AE)!= 1)goto _LL4A;_tmp1B3=((struct Cyc_Absyn_VarType_struct*)_tmp1AE)->f1;
_LL49: if(Cyc_Tcutil_tvar_kind(_tmp1B3)== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}
_LL4A: if(*((int*)_tmp1AE)!= 2)goto _LL4C;_tmp1B4=((struct Cyc_Absyn_TunionType_struct*)
_tmp1AE)->f1;_tmp1B5=_tmp1B4.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp1AE)->f1).tunion_info).KnownTunion).tag != 1)goto _LL4C;_tmp1B6=(_tmp1B5.KnownTunion).f1;
_tmp1B7=*_tmp1B6;_LL4B: if(_tmp1B7->is_flat)return Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(
_tmp1B7->name,({const char*_tmp1D9="_union";_tag_dynforward(_tmp1D9,sizeof(char),
_get_zero_arr_size_char(_tmp1D9,7));})));else{return Cyc_Absyn_void_star_typ();}
_LL4C: if(*((int*)_tmp1AE)!= 2)goto _LL4E;_LL4D:({void*_tmp1DA=0;((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp1DB="unresolved TunionType";_tag_dynforward(_tmp1DB,sizeof(char),
_get_zero_arr_size_char(_tmp1DB,22));}),_tag_dynforward(_tmp1DA,sizeof(void*),0));});
_LL4E: if(*((int*)_tmp1AE)!= 3)goto _LL50;_tmp1B8=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp1AE)->f1;_tmp1B9=_tmp1B8.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp1AE)->f1).field_info).KnownTunionfield).tag != 1)goto _LL50;_tmp1BA=(_tmp1B9.KnownTunionfield).f1;
_tmp1BB=(_tmp1B9.KnownTunionfield).f2;_LL4F: if(_tmp1BA->is_flat)return Cyc_Absyn_unionq_typ(
Cyc_Toc_collapse_qvar_tag(_tmp1BA->name,({const char*_tmp1DC="_union";
_tag_dynforward(_tmp1DC,sizeof(char),_get_zero_arr_size_char(_tmp1DC,7));})));
if(_tmp1BB->typs == 0){if(_tmp1BA->is_xtunion)return Cyc_Toc_char_star_typ();else{
return Cyc_Absyn_uint_typ;}}else{return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp1BB->name,({const char*_tmp1DD="_struct";_tag_dynforward(_tmp1DD,sizeof(char),
_get_zero_arr_size_char(_tmp1DD,8));})));}_LL50: if(*((int*)_tmp1AE)!= 3)goto
_LL52;_LL51:({void*_tmp1DE=0;((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1DF="unresolved TunionFieldType";
_tag_dynforward(_tmp1DF,sizeof(char),_get_zero_arr_size_char(_tmp1DF,27));}),
_tag_dynforward(_tmp1DE,sizeof(void*),0));});_LL52: if(*((int*)_tmp1AE)!= 4)goto
_LL54;_tmp1BC=((struct Cyc_Absyn_PointerType_struct*)_tmp1AE)->f1;_tmp1BD=(void*)
_tmp1BC.elt_typ;_tmp1BE=_tmp1BC.elt_tq;_tmp1BF=_tmp1BC.ptr_atts;_tmp1C0=_tmp1BF.bounds;
_LL53: _tmp1BD=Cyc_Toc_typ_to_c_array(_tmp1BD);{union Cyc_Absyn_Constraint_union
_tmp1E0=(Cyc_Absyn_compress_conref(_tmp1C0))->v;void*_tmp1E1;void*_tmp1E2;_LL7F:
if((_tmp1E0.No_constr).tag != 2)goto _LL81;_LL80: goto _LL82;_LL81: if((_tmp1E0.Eq_constr).tag
!= 0)goto _LL83;_tmp1E1=(_tmp1E0.Eq_constr).f1;if((int)_tmp1E1 != 0)goto _LL83;
_LL82: return Cyc_Toc_dynforward_ptr_typ;_LL83: if((_tmp1E0.Eq_constr).tag != 0)goto
_LL85;_tmp1E2=(_tmp1E0.Eq_constr).f1;if((int)_tmp1E2 != 1)goto _LL85;_LL84: return
Cyc_Toc_dyneither_ptr_typ;_LL85:;_LL86: return Cyc_Absyn_star_typ(_tmp1BD,(void*)2,
_tmp1BE,Cyc_Absyn_false_conref);_LL7E:;}_LL54: if(*((int*)_tmp1AE)!= 5)goto _LL56;
_LL55: goto _LL57;_LL56: if((int)_tmp1AE != 1)goto _LL58;_LL57: goto _LL59;_LL58: if(
_tmp1AE <= (void*)4)goto _LL74;if(*((int*)_tmp1AE)!= 6)goto _LL5A;_LL59: return t;
_LL5A: if(*((int*)_tmp1AE)!= 7)goto _LL5C;_tmp1C1=((struct Cyc_Absyn_ArrayType_struct*)
_tmp1AE)->f1;_tmp1C2=(void*)_tmp1C1.elt_type;_tmp1C3=_tmp1C1.tq;_tmp1C4=_tmp1C1.num_elts;
_tmp1C5=_tmp1C1.zt_loc;_LL5B: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp1C2),_tmp1C3,_tmp1C4,Cyc_Absyn_false_conref,_tmp1C5);_LL5C: if(*((int*)
_tmp1AE)!= 8)goto _LL5E;_tmp1C6=((struct Cyc_Absyn_FnType_struct*)_tmp1AE)->f1;
_tmp1C7=(void*)_tmp1C6.ret_typ;_tmp1C8=_tmp1C6.args;_tmp1C9=_tmp1C6.c_varargs;
_tmp1CA=_tmp1C6.cyc_varargs;_tmp1CB=_tmp1C6.attributes;_LL5D: {struct Cyc_List_List*
_tmp1E3=0;for(0;_tmp1CB != 0;_tmp1CB=_tmp1CB->tl){void*_tmp1E4=(void*)_tmp1CB->hd;
_LL88: if((int)_tmp1E4 != 3)goto _LL8A;_LL89: goto _LL8B;_LL8A: if((int)_tmp1E4 != 4)
goto _LL8C;_LL8B: goto _LL8D;_LL8C: if(_tmp1E4 <= (void*)17)goto _LL90;if(*((int*)
_tmp1E4)!= 3)goto _LL8E;_LL8D: continue;_LL8E: if(*((int*)_tmp1E4)!= 4)goto _LL90;
_LL8F: continue;_LL90:;_LL91: _tmp1E3=({struct Cyc_List_List*_tmp1E5=_cycalloc(
sizeof(*_tmp1E5));_tmp1E5->hd=(void*)((void*)_tmp1CB->hd);_tmp1E5->tl=_tmp1E3;
_tmp1E5;});goto _LL87;_LL87:;}{struct Cyc_List_List*_tmp1E6=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,
_tmp1C8);if(_tmp1CA != 0){void*_tmp1E7=Cyc_Toc_typ_to_c(Cyc_Absyn_dynforward_typ((
void*)_tmp1CA->type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));struct
_tuple2*_tmp1E8=({struct _tuple2*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));_tmp1EA->f1=
_tmp1CA->name;_tmp1EA->f2=_tmp1CA->tq;_tmp1EA->f3=_tmp1E7;_tmp1EA;});_tmp1E6=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp1E6,({struct Cyc_List_List*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9->hd=
_tmp1E8;_tmp1E9->tl=0;_tmp1E9;}));}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB[0]=({struct Cyc_Absyn_FnType_struct
_tmp1EC;_tmp1EC.tag=8;_tmp1EC.f1=({struct Cyc_Absyn_FnInfo _tmp1ED;_tmp1ED.tvars=0;
_tmp1ED.effect=0;_tmp1ED.ret_typ=(void*)Cyc_Toc_typ_to_c(_tmp1C7);_tmp1ED.args=
_tmp1E6;_tmp1ED.c_varargs=_tmp1C9;_tmp1ED.cyc_varargs=0;_tmp1ED.rgn_po=0;_tmp1ED.attributes=
_tmp1E3;_tmp1ED;});_tmp1EC;});_tmp1EB;});}}_LL5E: if(*((int*)_tmp1AE)!= 9)goto
_LL60;_tmp1CC=((struct Cyc_Absyn_TupleType_struct*)_tmp1AE)->f1;_LL5F: _tmp1CC=((
struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct _tuple4*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1CC);return Cyc_Toc_add_tuple_type(_tmp1CC);
_LL60: if(*((int*)_tmp1AE)!= 11)goto _LL62;_tmp1CD=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1AE)->f1;_tmp1CE=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1AE)->f2;_LL61:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp1EE=_cycalloc(sizeof(*
_tmp1EE));_tmp1EE[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp1EF;_tmp1EF.tag=11;
_tmp1EF.f1=(void*)_tmp1CD;_tmp1EF.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,
_tmp1CE);_tmp1EF;});_tmp1EE;});_LL62: if(*((int*)_tmp1AE)!= 10)goto _LL64;_tmp1CF=((
struct Cyc_Absyn_AggrType_struct*)_tmp1AE)->f1;_tmp1D0=_tmp1CF.aggr_info;_tmp1D1=
_tmp1CF.targs;_LL63: {struct Cyc_Absyn_Aggrdecl*_tmp1F0=Cyc_Absyn_get_known_aggrdecl(
_tmp1D0);if((void*)_tmp1F0->kind == (void*)1)return Cyc_Toc_aggrdecl_type(_tmp1F0->name,
Cyc_Absyn_unionq_typ);else{return Cyc_Toc_aggrdecl_type(_tmp1F0->name,Cyc_Absyn_strctq);}}
_LL64: if(*((int*)_tmp1AE)!= 12)goto _LL66;_tmp1D2=((struct Cyc_Absyn_EnumType_struct*)
_tmp1AE)->f1;_LL65: return t;_LL66: if(*((int*)_tmp1AE)!= 13)goto _LL68;_tmp1D3=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp1AE)->f1;_LL67: Cyc_Toc_enumfields_to_c(
_tmp1D3);return t;_LL68: if(*((int*)_tmp1AE)!= 17)goto _LL6A;_tmp1D4=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1AE)->f1;_tmp1D5=((struct Cyc_Absyn_TypedefType_struct*)_tmp1AE)->f2;_tmp1D6=((
struct Cyc_Absyn_TypedefType_struct*)_tmp1AE)->f3;_tmp1D7=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1AE)->f4;_LL69: if(_tmp1D7 == 0  || Cyc_noexpand_r){if(_tmp1D5 != 0)return(void*)({
struct Cyc_Absyn_TypedefType_struct*_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1[0]=({
struct Cyc_Absyn_TypedefType_struct _tmp1F2;_tmp1F2.tag=17;_tmp1F2.f1=_tmp1D4;
_tmp1F2.f2=0;_tmp1F2.f3=_tmp1D6;_tmp1F2.f4=0;_tmp1F2;});_tmp1F1;});else{return t;}}
else{return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp1F3=_cycalloc(sizeof(*
_tmp1F3));_tmp1F3[0]=({struct Cyc_Absyn_TypedefType_struct _tmp1F4;_tmp1F4.tag=17;
_tmp1F4.f1=_tmp1D4;_tmp1F4.f2=0;_tmp1F4.f3=_tmp1D6;_tmp1F4.f4=({void**_tmp1F5=
_cycalloc(sizeof(*_tmp1F5));_tmp1F5[0]=Cyc_Toc_typ_to_c_array(*_tmp1D7);_tmp1F5;});
_tmp1F4;});_tmp1F3;});}_LL6A: if(*((int*)_tmp1AE)!= 14)goto _LL6C;_LL6B: goto _LL6D;
_LL6C: if(*((int*)_tmp1AE)!= 18)goto _LL6E;_LL6D: return Cyc_Absyn_uint_typ;_LL6E:
if(*((int*)_tmp1AE)!= 15)goto _LL70;_tmp1D8=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp1AE)->f1;_LL6F: return Cyc_Toc_rgn_typ();_LL70: if(*((int*)_tmp1AE)!= 16)goto
_LL72;_LL71: return Cyc_Toc_dyn_rgn_typ();_LL72: if(*((int*)_tmp1AE)!= 19)goto _LL74;
_LL73:({void*_tmp1F6=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp1F7="Toc::typ_to_c: type translation passed a type integer";
_tag_dynforward(_tmp1F7,sizeof(char),_get_zero_arr_size_char(_tmp1F7,54));}),
_tag_dynforward(_tmp1F6,sizeof(void*),0));});_LL74: if((int)_tmp1AE != 2)goto _LL76;
_LL75:({void*_tmp1F8=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp1F9="Toc::typ_to_c: type translation passed the heap region";
_tag_dynforward(_tmp1F9,sizeof(char),_get_zero_arr_size_char(_tmp1F9,55));}),
_tag_dynforward(_tmp1F8,sizeof(void*),0));});_LL76: if((int)_tmp1AE != 3)goto _LL78;
_LL77:({void*_tmp1FA=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp1FB="Toc::typ_to_c: type translation passed the unique region";
_tag_dynforward(_tmp1FB,sizeof(char),_get_zero_arr_size_char(_tmp1FB,57));}),
_tag_dynforward(_tmp1FA,sizeof(void*),0));});_LL78: if(_tmp1AE <= (void*)4)goto
_LL7A;if(*((int*)_tmp1AE)!= 20)goto _LL7A;_LL79: goto _LL7B;_LL7A: if(_tmp1AE <= (
void*)4)goto _LL7C;if(*((int*)_tmp1AE)!= 21)goto _LL7C;_LL7B: goto _LL7D;_LL7C: if(
_tmp1AE <= (void*)4)goto _LL41;if(*((int*)_tmp1AE)!= 22)goto _LL41;_LL7D:({void*
_tmp1FC=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp1FD="Toc::typ_to_c: type translation passed an effect";
_tag_dynforward(_tmp1FD,sizeof(char),_get_zero_arr_size_char(_tmp1FD,49));}),
_tag_dynforward(_tmp1FC,sizeof(void*),0));});_LL41:;}static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
l){void*_tmp1FE=t;struct Cyc_Absyn_ArrayInfo _tmp1FF;void*_tmp200;struct Cyc_Absyn_Tqual
_tmp201;_LL93: if(_tmp1FE <= (void*)4)goto _LL95;if(*((int*)_tmp1FE)!= 7)goto _LL95;
_tmp1FF=((struct Cyc_Absyn_ArrayType_struct*)_tmp1FE)->f1;_tmp200=(void*)_tmp1FF.elt_type;
_tmp201=_tmp1FF.tq;_LL94: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp200,(void*)
2,_tmp201,Cyc_Absyn_false_conref),e);_LL95:;_LL96: return Cyc_Toc_cast_it(t,e);
_LL92:;}static int Cyc_Toc_atomic_typ(void*t){void*_tmp202=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_ArrayInfo _tmp203;void*_tmp204;struct Cyc_Absyn_AggrInfo _tmp205;
union Cyc_Absyn_AggrInfoU_union _tmp206;struct Cyc_List_List*_tmp207;struct Cyc_Absyn_TunionFieldInfo
_tmp208;union Cyc_Absyn_TunionFieldInfoU_union _tmp209;struct Cyc_Absyn_Tuniondecl*
_tmp20A;struct Cyc_Absyn_Tunionfield*_tmp20B;struct Cyc_List_List*_tmp20C;_LL98:
if((int)_tmp202 != 0)goto _LL9A;_LL99: return 1;_LL9A: if(_tmp202 <= (void*)4)goto
_LLA2;if(*((int*)_tmp202)!= 1)goto _LL9C;_LL9B: return 0;_LL9C: if(*((int*)_tmp202)
!= 5)goto _LL9E;_LL9D: goto _LL9F;_LL9E: if(*((int*)_tmp202)!= 12)goto _LLA0;_LL9F:
goto _LLA1;_LLA0: if(*((int*)_tmp202)!= 13)goto _LLA2;_LLA1: goto _LLA3;_LLA2: if((int)
_tmp202 != 1)goto _LLA4;_LLA3: goto _LLA5;_LLA4: if(_tmp202 <= (void*)4)goto _LLBC;if(*((
int*)_tmp202)!= 6)goto _LLA6;_LLA5: goto _LLA7;_LLA6: if(*((int*)_tmp202)!= 8)goto
_LLA8;_LLA7: goto _LLA9;_LLA8: if(*((int*)_tmp202)!= 18)goto _LLAA;_LLA9: goto _LLAB;
_LLAA: if(*((int*)_tmp202)!= 14)goto _LLAC;_LLAB: return 1;_LLAC: if(*((int*)_tmp202)
!= 7)goto _LLAE;_tmp203=((struct Cyc_Absyn_ArrayType_struct*)_tmp202)->f1;_tmp204=(
void*)_tmp203.elt_type;_LLAD: return Cyc_Toc_atomic_typ(_tmp204);_LLAE: if(*((int*)
_tmp202)!= 10)goto _LLB0;_tmp205=((struct Cyc_Absyn_AggrType_struct*)_tmp202)->f1;
_tmp206=_tmp205.aggr_info;_LLAF:{union Cyc_Absyn_AggrInfoU_union _tmp20D=_tmp206;
_LLBF: if((_tmp20D.UnknownAggr).tag != 0)goto _LLC1;_LLC0: return 0;_LLC1:;_LLC2: goto
_LLBE;_LLBE:;}{struct Cyc_Absyn_Aggrdecl*_tmp20E=Cyc_Absyn_get_known_aggrdecl(
_tmp206);if(_tmp20E->impl == 0)return 0;{struct Cyc_List_List*_tmp20F=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp20E->impl))->fields;for(0;_tmp20F != 0;_tmp20F=_tmp20F->tl){if(!
Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)_tmp20F->hd)->type))
return 0;}}return 1;}_LLB0: if(*((int*)_tmp202)!= 11)goto _LLB2;_tmp207=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp202)->f2;_LLB1: for(0;_tmp207 != 0;_tmp207=_tmp207->tl){if(!Cyc_Toc_atomic_typ((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp207->hd)->type))return 0;}return 1;_LLB2:
if(*((int*)_tmp202)!= 3)goto _LLB4;_tmp208=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp202)->f1;_tmp209=_tmp208.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp202)->f1).field_info).KnownTunionfield).tag != 1)goto _LLB4;_tmp20A=(_tmp209.KnownTunionfield).f1;
_tmp20B=(_tmp209.KnownTunionfield).f2;_LLB3: _tmp20C=_tmp20B->typs;goto _LLB5;
_LLB4: if(*((int*)_tmp202)!= 9)goto _LLB6;_tmp20C=((struct Cyc_Absyn_TupleType_struct*)
_tmp202)->f1;_LLB5: for(0;_tmp20C != 0;_tmp20C=_tmp20C->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple4*)_tmp20C->hd)).f2))return 0;}return 1;_LLB6: if(*((int*)_tmp202)!= 2)
goto _LLB8;_LLB7: goto _LLB9;_LLB8: if(*((int*)_tmp202)!= 4)goto _LLBA;_LLB9: goto
_LLBB;_LLBA: if(*((int*)_tmp202)!= 15)goto _LLBC;_LLBB: return 0;_LLBC:;_LLBD:({
struct Cyc_String_pa_struct _tmp212;_tmp212.tag=0;_tmp212.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp210[
1]={& _tmp212};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp211="atomic_typ:  bad type %s";_tag_dynforward(_tmp211,sizeof(char),
_get_zero_arr_size_char(_tmp211,25));}),_tag_dynforward(_tmp210,sizeof(void*),1));}});
_LL97:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp213=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp214;void*_tmp215;_LLC4: if(_tmp213 <= (void*)4)goto
_LLC6;if(*((int*)_tmp213)!= 4)goto _LLC6;_tmp214=((struct Cyc_Absyn_PointerType_struct*)
_tmp213)->f1;_tmp215=(void*)_tmp214.elt_typ;_LLC5: {void*_tmp216=Cyc_Tcutil_compress(
_tmp215);_LLC9: if((int)_tmp216 != 0)goto _LLCB;_LLCA: return 1;_LLCB:;_LLCC: return 0;
_LLC8:;}_LLC6:;_LLC7: return 0;_LLC3:;}static int Cyc_Toc_is_poly_field(void*t,
struct _dynforward_ptr*f){void*_tmp217=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp218;union Cyc_Absyn_AggrInfoU_union _tmp219;struct Cyc_List_List*_tmp21A;_LLCE:
if(_tmp217 <= (void*)4)goto _LLD2;if(*((int*)_tmp217)!= 10)goto _LLD0;_tmp218=((
struct Cyc_Absyn_AggrType_struct*)_tmp217)->f1;_tmp219=_tmp218.aggr_info;_LLCF: {
struct Cyc_Absyn_Aggrdecl*_tmp21B=Cyc_Absyn_get_known_aggrdecl(_tmp219);if(
_tmp21B->impl == 0)({void*_tmp21C=0;((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp21D="is_poly_field: type missing fields";
_tag_dynforward(_tmp21D,sizeof(char),_get_zero_arr_size_char(_tmp21D,35));}),
_tag_dynforward(_tmp21C,sizeof(void*),0));});_tmp21A=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp21B->impl))->fields;goto _LLD1;}_LLD0: if(*((int*)_tmp217)!= 11)
goto _LLD2;_tmp21A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp217)->f2;_LLD1: {
struct Cyc_Absyn_Aggrfield*_tmp21E=Cyc_Absyn_lookup_field(_tmp21A,f);if(_tmp21E == 
0)({struct Cyc_String_pa_struct _tmp221;_tmp221.tag=0;_tmp221.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp21F[1]={& _tmp221};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp220="is_poly_field: bad field %s";_tag_dynforward(_tmp220,sizeof(char),
_get_zero_arr_size_char(_tmp220,28));}),_tag_dynforward(_tmp21F,sizeof(void*),1));}});
return Cyc_Toc_is_void_star((void*)_tmp21E->type);}_LLD2:;_LLD3:({struct Cyc_String_pa_struct
_tmp224;_tmp224.tag=0;_tmp224.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp222[1]={& _tmp224};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp223="is_poly_field: bad type %s";_tag_dynforward(_tmp223,sizeof(char),
_get_zero_arr_size_char(_tmp223,27));}),_tag_dynforward(_tmp222,sizeof(void*),1));}});
_LLCD:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp225=(
void*)e->r;struct Cyc_Absyn_Exp*_tmp226;struct _dynforward_ptr*_tmp227;struct Cyc_Absyn_Exp*
_tmp228;struct _dynforward_ptr*_tmp229;_LLD5: if(*((int*)_tmp225)!= 23)goto _LLD7;
_tmp226=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp225)->f1;_tmp227=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp225)->f2;_LLD6: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp226->topt))->v,_tmp227);_LLD7: if(*((int*)_tmp225)!= 24)goto _LLD9;
_tmp228=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp225)->f1;_tmp229=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp225)->f2;_LLD8: {void*_tmp22A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp228->topt))->v);struct Cyc_Absyn_PtrInfo _tmp22B;void*_tmp22C;
_LLDC: if(_tmp22A <= (void*)4)goto _LLDE;if(*((int*)_tmp22A)!= 4)goto _LLDE;_tmp22B=((
struct Cyc_Absyn_PointerType_struct*)_tmp22A)->f1;_tmp22C=(void*)_tmp22B.elt_typ;
_LLDD: return Cyc_Toc_is_poly_field(_tmp22C,_tmp229);_LLDE:;_LLDF:({struct Cyc_String_pa_struct
_tmp22F;_tmp22F.tag=0;_tmp22F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp228->topt))->v));{
void*_tmp22D[1]={& _tmp22F};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp22E="is_poly_project: bad type %s";
_tag_dynforward(_tmp22E,sizeof(char),_get_zero_arr_size_char(_tmp22E,29));}),
_tag_dynforward(_tmp22D,sizeof(void*),1));}});_LLDB:;}_LLD9:;_LLDA: return 0;_LLD4:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp230=_cycalloc(sizeof(*_tmp230));
_tmp230->hd=s;_tmp230->tl=0;_tmp230;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,({
struct Cyc_List_List*_tmp231=_cycalloc(sizeof(*_tmp231));_tmp231->hd=s;_tmp231->tl=
0;_tmp231;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*
s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(
s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*
_tmp232[2];_tmp232[1]=s;_tmp232[0]=rgn;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp232,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*
s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type))return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmp233[2];_tmp233[1]=n;
_tmp233[0]=s;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp233,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp234[2];_tmp234[1]=n;_tmp234[0]=s;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp234,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*_tmp235[3];_tmp235[2]=n;_tmp235[1]=
s;_tmp235[0]=rgn;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp235,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({
struct Cyc_List_List*_tmp236=_cycalloc(sizeof(*_tmp236));_tmp236->hd=e;_tmp236->tl=
0;_tmp236;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,
struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e,int forward_only){int is_string=0;{
void*_tmp237=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp238;_LLE1: if(*((int*)
_tmp237)!= 0)goto _LLE3;_tmp238=((struct Cyc_Absyn_Const_e_struct*)_tmp237)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp237)->f1).String_c).tag != 5)goto _LLE3;
_LLE2: is_string=1;goto _LLE0;_LLE3:;_LLE4: goto _LLE0;_LLE0:;}{struct Cyc_Absyn_Exp*
xexp;struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp239=_cycalloc(sizeof(*_tmp239));_tmp239->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp23A=_cycalloc(sizeof(*_tmp23A));_tmp23A[0]=({
struct Cyc_Absyn_Var_d_struct _tmp23B;_tmp23B.tag=0;_tmp23B.f1=vd;_tmp23B;});
_tmp23A;}),0);_tmp239->tl=Cyc_Toc_result_decls;_tmp239;});xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
struct Cyc_Absyn_Exp*urm_exp;if(!forward_only)urm_exp=Cyc_Absyn_unresolvedmem_exp(
0,({struct _tuple8*_tmp23C[3];_tmp23C[2]=({struct _tuple8*_tmp23F=_cycalloc(
sizeof(*_tmp23F));_tmp23F->f1=0;_tmp23F->f2=xplussz;_tmp23F;});_tmp23C[1]=({
struct _tuple8*_tmp23E=_cycalloc(sizeof(*_tmp23E));_tmp23E->f1=0;_tmp23E->f2=xexp;
_tmp23E;});_tmp23C[0]=({struct _tuple8*_tmp23D=_cycalloc(sizeof(*_tmp23D));
_tmp23D->f1=0;_tmp23D->f2=xexp;_tmp23D;});((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp23C,sizeof(struct _tuple8*),3));}),
0);else{urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple8*_tmp240[2];_tmp240[
1]=({struct _tuple8*_tmp242=_cycalloc(sizeof(*_tmp242));_tmp242->f1=0;_tmp242->f2=
xplussz;_tmp242;});_tmp240[0]=({struct _tuple8*_tmp241=_cycalloc(sizeof(*_tmp241));
_tmp241->f1=0;_tmp241->f2=xexp;_tmp241;});((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp240,sizeof(struct _tuple8*),2));}),
0);}return urm_exp;}}}struct Cyc_Toc_FallthruInfo{struct _dynforward_ptr*label;
struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{
struct _dynforward_ptr**break_lab;struct _dynforward_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;};static int Cyc_Toc_is_toplevel(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp244;int _tmp245;struct Cyc_Toc_Env*
_tmp243=nv;_tmp244=*_tmp243;_tmp245=_tmp244.toplevel;return _tmp245;}static struct
Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){struct
Cyc_Toc_Env _tmp247;struct Cyc_Dict_Dict _tmp248;struct Cyc_Toc_Env*_tmp246=nv;
_tmp247=*_tmp246;_tmp248=_tmp247.varmap;return((struct Cyc_Absyn_Exp*(*)(struct
Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp248,x);}static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(struct _RegionHandle*r){return({struct Cyc_Toc_Env*_tmp249=
_region_malloc(r,sizeof(*_tmp249));_tmp249->break_lab=(struct _dynforward_ptr**)0;
_tmp249->continue_lab=(struct _dynforward_ptr**)0;_tmp249->fallthru_info=(struct
Cyc_Toc_FallthruInfo*)0;_tmp249->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp);_tmp249->toplevel=(int)1;_tmp249;});}static struct Cyc_Toc_Env*
Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp24B;struct _dynforward_ptr**_tmp24C;struct _dynforward_ptr**_tmp24D;struct Cyc_Toc_FallthruInfo*
_tmp24E;struct Cyc_Dict_Dict _tmp24F;int _tmp250;struct Cyc_Toc_Env*_tmp24A=e;
_tmp24B=*_tmp24A;_tmp24C=_tmp24B.break_lab;_tmp24D=_tmp24B.continue_lab;_tmp24E=
_tmp24B.fallthru_info;_tmp24F=_tmp24B.varmap;_tmp250=_tmp24B.toplevel;return({
struct Cyc_Toc_Env*_tmp251=_region_malloc(r,sizeof(*_tmp251));_tmp251->break_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp24C);_tmp251->continue_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp24D);_tmp251->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp24E;_tmp251->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp24F);_tmp251->toplevel=(int)_tmp250;_tmp251;});}static struct Cyc_Toc_Env*
Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp253;struct _dynforward_ptr**_tmp254;struct _dynforward_ptr**_tmp255;struct Cyc_Toc_FallthruInfo*
_tmp256;struct Cyc_Dict_Dict _tmp257;int _tmp258;struct Cyc_Toc_Env*_tmp252=e;
_tmp253=*_tmp252;_tmp254=_tmp253.break_lab;_tmp255=_tmp253.continue_lab;_tmp256=
_tmp253.fallthru_info;_tmp257=_tmp253.varmap;_tmp258=_tmp253.toplevel;return({
struct Cyc_Toc_Env*_tmp259=_region_malloc(r,sizeof(*_tmp259));_tmp259->break_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp254);_tmp259->continue_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp255);_tmp259->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp256;_tmp259->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp257);_tmp259->toplevel=(int)0;_tmp259;});}static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp25B;struct
_dynforward_ptr**_tmp25C;struct _dynforward_ptr**_tmp25D;struct Cyc_Toc_FallthruInfo*
_tmp25E;struct Cyc_Dict_Dict _tmp25F;int _tmp260;struct Cyc_Toc_Env*_tmp25A=e;
_tmp25B=*_tmp25A;_tmp25C=_tmp25B.break_lab;_tmp25D=_tmp25B.continue_lab;_tmp25E=
_tmp25B.fallthru_info;_tmp25F=_tmp25B.varmap;_tmp260=_tmp25B.toplevel;return({
struct Cyc_Toc_Env*_tmp261=_region_malloc(r,sizeof(*_tmp261));_tmp261->break_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp25C);_tmp261->continue_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp25D);_tmp261->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp25E;_tmp261->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp25F);_tmp261->toplevel=(int)1;_tmp261;});}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace_union _tmp262=(*x).f1;_LLE6: if((_tmp262.Rel_n).tag != 1)
goto _LLE8;_LLE7:({struct Cyc_String_pa_struct _tmp265;_tmp265.tag=0;_tmp265.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(x));{void*
_tmp263[1]={& _tmp265};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp264="Toc::add_varmap on Rel_n: %s\n";
_tag_dynforward(_tmp264,sizeof(char),_get_zero_arr_size_char(_tmp264,30));}),
_tag_dynforward(_tmp263,sizeof(void*),1));}});_LLE8:;_LLE9: goto _LLE5;_LLE5:;}{
struct Cyc_Toc_Env _tmp267;struct _dynforward_ptr**_tmp268;struct _dynforward_ptr**
_tmp269;struct Cyc_Toc_FallthruInfo*_tmp26A;struct Cyc_Dict_Dict _tmp26B;int _tmp26C;
struct Cyc_Toc_Env*_tmp266=e;_tmp267=*_tmp266;_tmp268=_tmp267.break_lab;_tmp269=
_tmp267.continue_lab;_tmp26A=_tmp267.fallthru_info;_tmp26B=_tmp267.varmap;
_tmp26C=_tmp267.toplevel;{struct Cyc_Dict_Dict _tmp26D=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp26B),x,y);return({struct Cyc_Toc_Env*_tmp26E=_region_malloc(r,sizeof(*
_tmp26E));_tmp26E->break_lab=(struct _dynforward_ptr**)((struct _dynforward_ptr**)
_tmp268);_tmp26E->continue_lab=(struct _dynforward_ptr**)((struct _dynforward_ptr**)
_tmp269);_tmp26E->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp26A;_tmp26E->varmap=(
struct Cyc_Dict_Dict)_tmp26D;_tmp26E->toplevel=(int)_tmp26C;_tmp26E;});}}}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env _tmp270;struct _dynforward_ptr**_tmp271;struct _dynforward_ptr**
_tmp272;struct Cyc_Toc_FallthruInfo*_tmp273;struct Cyc_Dict_Dict _tmp274;int _tmp275;
struct Cyc_Toc_Env*_tmp26F=e;_tmp270=*_tmp26F;_tmp271=_tmp270.break_lab;_tmp272=
_tmp270.continue_lab;_tmp273=_tmp270.fallthru_info;_tmp274=_tmp270.varmap;
_tmp275=_tmp270.toplevel;return({struct Cyc_Toc_Env*_tmp276=_region_malloc(r,
sizeof(*_tmp276));_tmp276->break_lab=(struct _dynforward_ptr**)0;_tmp276->continue_lab=(
struct _dynforward_ptr**)0;_tmp276->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp273;_tmp276->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp274);_tmp276->toplevel=(
int)_tmp275;_tmp276;});}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dynforward_ptr*break_l,struct
_dynforward_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){fallthru_vars=({struct Cyc_List_List*
_tmp277=_region_malloc(r,sizeof(*_tmp277));_tmp277->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp277->tl=fallthru_vars;_tmp277;});}fallthru_vars=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct Cyc_Toc_Env _tmp279;struct _dynforward_ptr**_tmp27A;struct _dynforward_ptr**
_tmp27B;struct Cyc_Toc_FallthruInfo*_tmp27C;struct Cyc_Dict_Dict _tmp27D;int _tmp27E;
struct Cyc_Toc_Env*_tmp278=e;_tmp279=*_tmp278;_tmp27A=_tmp279.break_lab;_tmp27B=
_tmp279.continue_lab;_tmp27C=_tmp279.fallthru_info;_tmp27D=_tmp279.varmap;
_tmp27E=_tmp279.toplevel;{struct Cyc_Toc_Env _tmp280;struct Cyc_Dict_Dict _tmp281;
struct Cyc_Toc_Env*_tmp27F=next_case_env;_tmp280=*_tmp27F;_tmp281=_tmp280.varmap;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp284=
_region_malloc(r,sizeof(*_tmp284));_tmp284->label=fallthru_l;_tmp284->binders=
fallthru_vars;_tmp284->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp281);_tmp284;});
return({struct Cyc_Toc_Env*_tmp282=_region_malloc(r,sizeof(*_tmp282));_tmp282->break_lab=(
struct _dynforward_ptr**)({struct _dynforward_ptr**_tmp283=_region_malloc(r,
sizeof(*_tmp283));_tmp283[0]=break_l;_tmp283;});_tmp282->continue_lab=(struct
_dynforward_ptr**)((struct _dynforward_ptr**)_tmp27B);_tmp282->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)fi;_tmp282->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp27D);_tmp282->toplevel=(
int)_tmp27E;_tmp282;});}}}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dynforward_ptr*break_l){struct
Cyc_Toc_Env _tmp286;struct _dynforward_ptr**_tmp287;struct _dynforward_ptr**_tmp288;
struct Cyc_Toc_FallthruInfo*_tmp289;struct Cyc_Dict_Dict _tmp28A;int _tmp28B;struct
Cyc_Toc_Env*_tmp285=e;_tmp286=*_tmp285;_tmp287=_tmp286.break_lab;_tmp288=_tmp286.continue_lab;
_tmp289=_tmp286.fallthru_info;_tmp28A=_tmp286.varmap;_tmp28B=_tmp286.toplevel;
return({struct Cyc_Toc_Env*_tmp28C=_region_malloc(r,sizeof(*_tmp28C));_tmp28C->break_lab=(
struct _dynforward_ptr**)({struct _dynforward_ptr**_tmp28D=_region_malloc(r,
sizeof(*_tmp28D));_tmp28D[0]=break_l;_tmp28D;});_tmp28C->continue_lab=(struct
_dynforward_ptr**)((struct _dynforward_ptr**)_tmp288);_tmp28C->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0;_tmp28C->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp28A);_tmp28C->toplevel=(
int)_tmp28B;_tmp28C;});}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dynforward_ptr*next_l){struct
Cyc_Toc_Env _tmp28F;struct _dynforward_ptr**_tmp290;struct _dynforward_ptr**_tmp291;
struct Cyc_Toc_FallthruInfo*_tmp292;struct Cyc_Dict_Dict _tmp293;int _tmp294;struct
Cyc_Toc_Env*_tmp28E=e;_tmp28F=*_tmp28E;_tmp290=_tmp28F.break_lab;_tmp291=_tmp28F.continue_lab;
_tmp292=_tmp28F.fallthru_info;_tmp293=_tmp28F.varmap;_tmp294=_tmp28F.toplevel;
return({struct Cyc_Toc_Env*_tmp295=_region_malloc(r,sizeof(*_tmp295));_tmp295->break_lab=(
struct _dynforward_ptr**)0;_tmp295->continue_lab=(struct _dynforward_ptr**)((
struct _dynforward_ptr**)_tmp291);_tmp295->fallthru_info=(struct Cyc_Toc_FallthruInfo*)({
struct Cyc_Toc_FallthruInfo*_tmp296=_region_malloc(r,sizeof(*_tmp296));_tmp296->label=
next_l;_tmp296->binders=0;_tmp296->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);
_tmp296;});_tmp295->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp293);_tmp295->toplevel=(
int)_tmp294;_tmp295;});}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*_tmp297=(void*)e->annot;
_LLEB: if(*((void**)_tmp297)!= Cyc_CfFlowInfo_UnknownZ)goto _LLED;_LLEC: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLED: if(*((void**)_tmp297)
!= Cyc_CfFlowInfo_NotZero)goto _LLEF;_LLEE: return 0;_LLEF: if(_tmp297 != Cyc_CfFlowInfo_IsZero)
goto _LLF1;_LLF0:({void*_tmp298=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp299="dereference of NULL pointer";
_tag_dynforward(_tmp299,sizeof(char),_get_zero_arr_size_char(_tmp299,28));}),
_tag_dynforward(_tmp298,sizeof(void*),0));});return 0;_LLF1: if(_tmp297 != Cyc_Absyn_EmptyAnnot)
goto _LLF3;_LLF2: return 0;_LLF3: if(*((void**)_tmp297)!= Cyc_CfFlowInfo_HasTagCmps)
goto _LLF5;_LLF4:({void*_tmp29A=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp29B="compiler oddity: pointer compared to tag type";
_tag_dynforward(_tmp29B,sizeof(char),_get_zero_arr_size_char(_tmp29B,46));}),
_tag_dynforward(_tmp29A,sizeof(void*),0));});return Cyc_Toc_is_nullable((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLF5:;_LLF6:({void*_tmp29C=0;((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp29D="need_null_check";_tag_dynforward(_tmp29D,sizeof(char),
_get_zero_arr_size_char(_tmp29D,16));}),_tag_dynforward(_tmp29C,sizeof(void*),0));});
_LLEA:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*
_tmp29E=(void*)e->annot;struct Cyc_List_List*_tmp29F;struct Cyc_List_List*_tmp2A0;
_LLF8: if(*((void**)_tmp29E)!= Cyc_CfFlowInfo_UnknownZ)goto _LLFA;_tmp29F=((struct
Cyc_CfFlowInfo_UnknownZ_struct*)_tmp29E)->f1;_LLF9: return _tmp29F;_LLFA: if(*((
void**)_tmp29E)!= Cyc_CfFlowInfo_NotZero)goto _LLFC;_tmp2A0=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmp29E)->f1;_LLFB: return _tmp2A0;_LLFC: if(_tmp29E != Cyc_CfFlowInfo_IsZero)goto
_LLFE;_LLFD:({void*_tmp2A1=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp2A2="dereference of NULL pointer";
_tag_dynforward(_tmp2A2,sizeof(char),_get_zero_arr_size_char(_tmp2A2,28));}),
_tag_dynforward(_tmp2A1,sizeof(void*),0));});return 0;_LLFE: if(*((void**)_tmp29E)
!= Cyc_CfFlowInfo_HasTagCmps)goto _LL100;_LLFF: goto _LL101;_LL100: if(_tmp29E != Cyc_Absyn_EmptyAnnot)
goto _LL102;_LL101: return 0;_LL102:;_LL103:({void*_tmp2A3=0;((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp2A4="get_relns";_tag_dynforward(_tmp2A4,sizeof(char),_get_zero_arr_size_char(
_tmp2A4,10));}),_tag_dynforward(_tmp2A3,sizeof(void*),0));});_LLF7:;}static int
Cyc_Toc_check_const_array(unsigned int i,void*t){void*_tmp2A5=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp2A6;struct Cyc_Absyn_PtrAtts _tmp2A7;struct Cyc_Absyn_Conref*
_tmp2A8;struct Cyc_Absyn_Conref*_tmp2A9;struct Cyc_Absyn_ArrayInfo _tmp2AA;struct
Cyc_Absyn_Exp*_tmp2AB;_LL105: if(_tmp2A5 <= (void*)4)goto _LL109;if(*((int*)_tmp2A5)
!= 4)goto _LL107;_tmp2A6=((struct Cyc_Absyn_PointerType_struct*)_tmp2A5)->f1;
_tmp2A7=_tmp2A6.ptr_atts;_tmp2A8=_tmp2A7.bounds;_tmp2A9=_tmp2A7.zero_term;_LL106: {
void*_tmp2AC=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp2A8);struct Cyc_Absyn_Exp*
_tmp2AD;_LL10C: if((int)_tmp2AC != 0)goto _LL10E;_LL10D: return 0;_LL10E: if((int)
_tmp2AC != 1)goto _LL110;_LL10F: return 0;_LL110: if(_tmp2AC <= (void*)2)goto _LL112;
if(*((int*)_tmp2AC)!= 0)goto _LL112;_tmp2AD=((struct Cyc_Absyn_Upper_b_struct*)
_tmp2AC)->f1;_LL111: {unsigned int _tmp2AF;int _tmp2B0;struct _tuple5 _tmp2AE=Cyc_Evexp_eval_const_uint_exp(
_tmp2AD);_tmp2AF=_tmp2AE.f1;_tmp2B0=_tmp2AE.f2;return _tmp2B0  && i <= _tmp2AF;}
_LL112: if(_tmp2AC <= (void*)2)goto _LL10B;if(*((int*)_tmp2AC)!= 1)goto _LL10B;
_LL113:({void*_tmp2B1=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr
ap))Cyc_Toc_unimp)(({const char*_tmp2B2="check_const_array: AbsUpper_b";
_tag_dynforward(_tmp2B2,sizeof(char),_get_zero_arr_size_char(_tmp2B2,30));}),
_tag_dynforward(_tmp2B1,sizeof(void*),0));});_LL10B:;}_LL107: if(*((int*)_tmp2A5)
!= 7)goto _LL109;_tmp2AA=((struct Cyc_Absyn_ArrayType_struct*)_tmp2A5)->f1;_tmp2AB=
_tmp2AA.num_elts;_LL108: if(_tmp2AB == 0)return 0;{unsigned int _tmp2B4;int _tmp2B5;
struct _tuple5 _tmp2B3=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp2AB);
_tmp2B4=_tmp2B3.f1;_tmp2B5=_tmp2B3.f2;return _tmp2B5  && i <= _tmp2B4;}_LL109:;
_LL10A: return 0;_LL104:;}static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*
relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=
relns->tl){struct Cyc_CfFlowInfo_Reln*_tmp2B6=(struct Cyc_CfFlowInfo_Reln*)relns->hd;
if(_tmp2B6->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp_union _tmp2B7=_tmp2B6->rop;
struct Cyc_Absyn_Vardecl*_tmp2B8;struct Cyc_Absyn_Vardecl*_tmp2B9;_LL115: if((
_tmp2B7.LessSize).tag != 2)goto _LL117;_tmp2B8=(_tmp2B7.LessSize).f1;_LL116:
_tmp2B9=_tmp2B8;goto _LL118;_LL117: if((_tmp2B7.LessEqSize).tag != 4)goto _LL119;
_tmp2B9=(_tmp2B7.LessEqSize).f1;_LL118: if(_tmp2B9 == v)return 1;else{goto _LL114;}
_LL119:;_LL11A: continue;_LL114:;}}return 0;}static int Cyc_Toc_check_leq_size(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*
_tmp2BA=(void*)e->r;void*_tmp2BB;struct Cyc_Absyn_Vardecl*_tmp2BC;void*_tmp2BD;
struct Cyc_Absyn_Vardecl*_tmp2BE;void*_tmp2BF;struct Cyc_Absyn_Vardecl*_tmp2C0;
void*_tmp2C1;struct Cyc_Absyn_Vardecl*_tmp2C2;void*_tmp2C3;struct Cyc_List_List*
_tmp2C4;struct Cyc_List_List _tmp2C5;struct Cyc_Absyn_Exp*_tmp2C6;_LL11C: if(*((int*)
_tmp2BA)!= 1)goto _LL11E;_tmp2BB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2BA)->f2;
if(_tmp2BB <= (void*)1)goto _LL11E;if(*((int*)_tmp2BB)!= 4)goto _LL11E;_tmp2BC=((
struct Cyc_Absyn_Pat_b_struct*)_tmp2BB)->f1;_LL11D: _tmp2BE=_tmp2BC;goto _LL11F;
_LL11E: if(*((int*)_tmp2BA)!= 1)goto _LL120;_tmp2BD=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2BA)->f2;if(_tmp2BD <= (void*)1)goto _LL120;if(*((int*)_tmp2BD)!= 3)goto _LL120;
_tmp2BE=((struct Cyc_Absyn_Local_b_struct*)_tmp2BD)->f1;_LL11F: _tmp2C0=_tmp2BE;
goto _LL121;_LL120: if(*((int*)_tmp2BA)!= 1)goto _LL122;_tmp2BF=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2BA)->f2;if(_tmp2BF <= (void*)1)goto _LL122;if(*((int*)_tmp2BF)!= 0)goto _LL122;
_tmp2C0=((struct Cyc_Absyn_Global_b_struct*)_tmp2BF)->f1;_LL121: _tmp2C2=_tmp2C0;
goto _LL123;_LL122: if(*((int*)_tmp2BA)!= 1)goto _LL124;_tmp2C1=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2BA)->f2;if(_tmp2C1 <= (void*)1)goto _LL124;if(*((int*)_tmp2C1)!= 2)goto _LL124;
_tmp2C2=((struct Cyc_Absyn_Param_b_struct*)_tmp2C1)->f1;_LL123: if(_tmp2C2->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2C2))return 1;goto _LL11B;_LL124:
if(*((int*)_tmp2BA)!= 3)goto _LL126;_tmp2C3=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp2BA)->f1;if((int)_tmp2C3 != 19)goto _LL126;_tmp2C4=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2BA)->f2;if(_tmp2C4 == 0)goto _LL126;_tmp2C5=*_tmp2C4;_tmp2C6=(struct Cyc_Absyn_Exp*)
_tmp2C5.hd;_LL125:{void*_tmp2C7=(void*)_tmp2C6->r;void*_tmp2C8;struct Cyc_Absyn_Vardecl*
_tmp2C9;void*_tmp2CA;struct Cyc_Absyn_Vardecl*_tmp2CB;void*_tmp2CC;struct Cyc_Absyn_Vardecl*
_tmp2CD;void*_tmp2CE;struct Cyc_Absyn_Vardecl*_tmp2CF;_LL129: if(*((int*)_tmp2C7)
!= 1)goto _LL12B;_tmp2C8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2C7)->f2;if(
_tmp2C8 <= (void*)1)goto _LL12B;if(*((int*)_tmp2C8)!= 4)goto _LL12B;_tmp2C9=((
struct Cyc_Absyn_Pat_b_struct*)_tmp2C8)->f1;_LL12A: _tmp2CB=_tmp2C9;goto _LL12C;
_LL12B: if(*((int*)_tmp2C7)!= 1)goto _LL12D;_tmp2CA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2C7)->f2;if(_tmp2CA <= (void*)1)goto _LL12D;if(*((int*)_tmp2CA)!= 3)goto _LL12D;
_tmp2CB=((struct Cyc_Absyn_Local_b_struct*)_tmp2CA)->f1;_LL12C: _tmp2CD=_tmp2CB;
goto _LL12E;_LL12D: if(*((int*)_tmp2C7)!= 1)goto _LL12F;_tmp2CC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2C7)->f2;if(_tmp2CC <= (void*)1)goto _LL12F;if(*((int*)_tmp2CC)!= 0)goto _LL12F;
_tmp2CD=((struct Cyc_Absyn_Global_b_struct*)_tmp2CC)->f1;_LL12E: _tmp2CF=_tmp2CD;
goto _LL130;_LL12F: if(*((int*)_tmp2C7)!= 1)goto _LL131;_tmp2CE=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2C7)->f2;if(_tmp2CE <= (void*)1)goto _LL131;if(*((int*)_tmp2CE)!= 2)goto _LL131;
_tmp2CF=((struct Cyc_Absyn_Param_b_struct*)_tmp2CE)->f1;_LL130: return _tmp2CF == v;
_LL131:;_LL132: goto _LL128;_LL128:;}goto _LL11B;_LL126:;_LL127: goto _LL11B;_LL11B:;}
return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i){{void*_tmp2D0=(void*)a->r;void*_tmp2D1;struct Cyc_Absyn_Vardecl*
_tmp2D2;void*_tmp2D3;struct Cyc_Absyn_Vardecl*_tmp2D4;void*_tmp2D5;struct Cyc_Absyn_Vardecl*
_tmp2D6;void*_tmp2D7;struct Cyc_Absyn_Vardecl*_tmp2D8;_LL134: if(*((int*)_tmp2D0)
!= 1)goto _LL136;_tmp2D1=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2D0)->f2;if(
_tmp2D1 <= (void*)1)goto _LL136;if(*((int*)_tmp2D1)!= 4)goto _LL136;_tmp2D2=((
struct Cyc_Absyn_Pat_b_struct*)_tmp2D1)->f1;_LL135: _tmp2D4=_tmp2D2;goto _LL137;
_LL136: if(*((int*)_tmp2D0)!= 1)goto _LL138;_tmp2D3=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D0)->f2;if(_tmp2D3 <= (void*)1)goto _LL138;if(*((int*)_tmp2D3)!= 3)goto _LL138;
_tmp2D4=((struct Cyc_Absyn_Local_b_struct*)_tmp2D3)->f1;_LL137: _tmp2D6=_tmp2D4;
goto _LL139;_LL138: if(*((int*)_tmp2D0)!= 1)goto _LL13A;_tmp2D5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D0)->f2;if(_tmp2D5 <= (void*)1)goto _LL13A;if(*((int*)_tmp2D5)!= 0)goto _LL13A;
_tmp2D6=((struct Cyc_Absyn_Global_b_struct*)_tmp2D5)->f1;_LL139: _tmp2D8=_tmp2D6;
goto _LL13B;_LL13A: if(*((int*)_tmp2D0)!= 1)goto _LL13C;_tmp2D7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D0)->f2;if(_tmp2D7 <= (void*)1)goto _LL13C;if(*((int*)_tmp2D7)!= 2)goto _LL13C;
_tmp2D8=((struct Cyc_Absyn_Param_b_struct*)_tmp2D7)->f1;_LL13B: if(_tmp2D8->escapes)
return 0;inner_loop: {void*_tmp2D9=(void*)i->r;void*_tmp2DA;struct Cyc_Absyn_Exp*
_tmp2DB;union Cyc_Absyn_Cnst_union _tmp2DC;void*_tmp2DD;int _tmp2DE;union Cyc_Absyn_Cnst_union
_tmp2DF;void*_tmp2E0;int _tmp2E1;union Cyc_Absyn_Cnst_union _tmp2E2;void*_tmp2E3;
int _tmp2E4;void*_tmp2E5;struct Cyc_List_List*_tmp2E6;struct Cyc_List_List _tmp2E7;
struct Cyc_Absyn_Exp*_tmp2E8;struct Cyc_List_List*_tmp2E9;struct Cyc_List_List
_tmp2EA;struct Cyc_Absyn_Exp*_tmp2EB;void*_tmp2EC;struct Cyc_Absyn_Vardecl*_tmp2ED;
void*_tmp2EE;struct Cyc_Absyn_Vardecl*_tmp2EF;void*_tmp2F0;struct Cyc_Absyn_Vardecl*
_tmp2F1;void*_tmp2F2;struct Cyc_Absyn_Vardecl*_tmp2F3;_LL13F: if(*((int*)_tmp2D9)
!= 15)goto _LL141;_tmp2DA=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2D9)->f1;
_tmp2DB=((struct Cyc_Absyn_Cast_e_struct*)_tmp2D9)->f2;_LL140: i=_tmp2DB;goto
inner_loop;_LL141: if(*((int*)_tmp2D9)!= 0)goto _LL143;_tmp2DC=((struct Cyc_Absyn_Const_e_struct*)
_tmp2D9)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp2D9)->f1).Int_c).tag != 2)
goto _LL143;_tmp2DD=(_tmp2DC.Int_c).f1;if((int)_tmp2DD != 2)goto _LL143;_tmp2DE=(
_tmp2DC.Int_c).f2;_LL142: _tmp2E1=_tmp2DE;goto _LL144;_LL143: if(*((int*)_tmp2D9)!= 
0)goto _LL145;_tmp2DF=((struct Cyc_Absyn_Const_e_struct*)_tmp2D9)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmp2D9)->f1).Int_c).tag != 2)goto _LL145;_tmp2E0=(
_tmp2DF.Int_c).f1;if((int)_tmp2E0 != 0)goto _LL145;_tmp2E1=(_tmp2DF.Int_c).f2;
_LL144: return _tmp2E1 >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp2E1 + 1),(
void*)_tmp2D8->type);_LL145: if(*((int*)_tmp2D9)!= 0)goto _LL147;_tmp2E2=((struct
Cyc_Absyn_Const_e_struct*)_tmp2D9)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp2D9)->f1).Int_c).tag != 2)goto _LL147;_tmp2E3=(_tmp2E2.Int_c).f1;if((int)
_tmp2E3 != 1)goto _LL147;_tmp2E4=(_tmp2E2.Int_c).f2;_LL146: return Cyc_Toc_check_const_array((
unsigned int)(_tmp2E4 + 1),(void*)_tmp2D8->type);_LL147: if(*((int*)_tmp2D9)!= 3)
goto _LL149;_tmp2E5=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2D9)->f1;if((
int)_tmp2E5 != 4)goto _LL149;_tmp2E6=((struct Cyc_Absyn_Primop_e_struct*)_tmp2D9)->f2;
if(_tmp2E6 == 0)goto _LL149;_tmp2E7=*_tmp2E6;_tmp2E8=(struct Cyc_Absyn_Exp*)_tmp2E7.hd;
_tmp2E9=_tmp2E7.tl;if(_tmp2E9 == 0)goto _LL149;_tmp2EA=*_tmp2E9;_tmp2EB=(struct Cyc_Absyn_Exp*)
_tmp2EA.hd;_LL148: return Cyc_Toc_check_leq_size(relns,_tmp2D8,_tmp2EB);_LL149: if(*((
int*)_tmp2D9)!= 1)goto _LL14B;_tmp2EC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D9)->f2;if(_tmp2EC <= (void*)1)goto _LL14B;if(*((int*)_tmp2EC)!= 4)goto _LL14B;
_tmp2ED=((struct Cyc_Absyn_Pat_b_struct*)_tmp2EC)->f1;_LL14A: _tmp2EF=_tmp2ED;goto
_LL14C;_LL14B: if(*((int*)_tmp2D9)!= 1)goto _LL14D;_tmp2EE=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D9)->f2;if(_tmp2EE <= (void*)1)goto _LL14D;if(*((int*)_tmp2EE)!= 3)goto _LL14D;
_tmp2EF=((struct Cyc_Absyn_Local_b_struct*)_tmp2EE)->f1;_LL14C: _tmp2F1=_tmp2EF;
goto _LL14E;_LL14D: if(*((int*)_tmp2D9)!= 1)goto _LL14F;_tmp2F0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D9)->f2;if(_tmp2F0 <= (void*)1)goto _LL14F;if(*((int*)_tmp2F0)!= 0)goto _LL14F;
_tmp2F1=((struct Cyc_Absyn_Global_b_struct*)_tmp2F0)->f1;_LL14E: _tmp2F3=_tmp2F1;
goto _LL150;_LL14F: if(*((int*)_tmp2D9)!= 1)goto _LL151;_tmp2F2=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D9)->f2;if(_tmp2F2 <= (void*)1)goto _LL151;if(*((int*)_tmp2F2)!= 2)goto _LL151;
_tmp2F3=((struct Cyc_Absyn_Param_b_struct*)_tmp2F2)->f1;_LL150: if(_tmp2F3->escapes)
return 0;{struct Cyc_List_List*_tmp2F4=relns;for(0;_tmp2F4 != 0;_tmp2F4=_tmp2F4->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2F5=(struct Cyc_CfFlowInfo_Reln*)_tmp2F4->hd;if(
_tmp2F5->vd == _tmp2F3){union Cyc_CfFlowInfo_RelnOp_union _tmp2F6=_tmp2F5->rop;
struct Cyc_Absyn_Vardecl*_tmp2F7;struct Cyc_Absyn_Vardecl*_tmp2F8;unsigned int
_tmp2F9;_LL154: if((_tmp2F6.LessSize).tag != 2)goto _LL156;_tmp2F7=(_tmp2F6.LessSize).f1;
_LL155: if(_tmp2D8 == _tmp2F7)return 1;else{goto _LL153;}_LL156: if((_tmp2F6.LessVar).tag
!= 1)goto _LL158;_tmp2F8=(_tmp2F6.LessVar).f1;_LL157:{struct Cyc_List_List*_tmp2FA=
relns;for(0;_tmp2FA != 0;_tmp2FA=_tmp2FA->tl){struct Cyc_CfFlowInfo_Reln*_tmp2FB=(
struct Cyc_CfFlowInfo_Reln*)_tmp2FA->hd;if(_tmp2FB->vd == _tmp2F8){union Cyc_CfFlowInfo_RelnOp_union
_tmp2FC=_tmp2FB->rop;struct Cyc_Absyn_Vardecl*_tmp2FD;struct Cyc_Absyn_Vardecl*
_tmp2FE;unsigned int _tmp2FF;struct Cyc_Absyn_Vardecl*_tmp300;_LL15D: if((_tmp2FC.LessEqSize).tag
!= 4)goto _LL15F;_tmp2FD=(_tmp2FC.LessEqSize).f1;_LL15E: _tmp2FE=_tmp2FD;goto
_LL160;_LL15F: if((_tmp2FC.LessSize).tag != 2)goto _LL161;_tmp2FE=(_tmp2FC.LessSize).f1;
_LL160: if(_tmp2D8 == _tmp2FE)return 1;goto _LL15C;_LL161: if((_tmp2FC.EqualConst).tag
!= 0)goto _LL163;_tmp2FF=(_tmp2FC.EqualConst).f1;_LL162: return Cyc_Toc_check_const_array(
_tmp2FF,(void*)_tmp2D8->type);_LL163: if((_tmp2FC.LessVar).tag != 1)goto _LL165;
_tmp300=(_tmp2FC.LessVar).f1;_LL164: if(Cyc_Toc_check_leq_size_var(relns,_tmp2D8,
_tmp300))return 1;goto _LL15C;_LL165:;_LL166: goto _LL15C;_LL15C:;}}}goto _LL153;
_LL158: if((_tmp2F6.LessConst).tag != 3)goto _LL15A;_tmp2F9=(_tmp2F6.LessConst).f1;
_LL159: return Cyc_Toc_check_const_array(_tmp2F9,(void*)_tmp2D8->type);_LL15A:;
_LL15B: goto _LL153;_LL153:;}}}goto _LL13E;_LL151:;_LL152: goto _LL13E;_LL13E:;}goto
_LL133;_LL13C:;_LL13D: goto _LL133;_LL133:;}return 0;}static void*Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp301=0;((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp302="Missing type in primop ";_tag_dynforward(_tmp302,sizeof(char),
_get_zero_arr_size_char(_tmp302,24));}),_tag_dynforward(_tmp301,sizeof(void*),0));});
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*
_tmp303=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp304="Missing type in primop ";_tag_dynforward(_tmp304,sizeof(char),
_get_zero_arr_size_char(_tmp304,24));}),_tag_dynforward(_tmp303,sizeof(void*),0));});
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple4*
Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){return({struct _tuple4*_tmp305=_cycalloc(
sizeof(*_tmp305));_tmp305->f1=Cyc_Toc_mt_tq;_tmp305->f2=Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_tmp305;});}static struct _tuple8*
Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);return({struct _tuple8*_tmp306=_cycalloc(sizeof(*_tmp306));_tmp306->f1=0;
_tmp306->f2=e;_tmp306;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct
Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,
struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(
pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp307=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(
_tmp307): Cyc_Toc_malloc_ptr(_tmp307));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp307);}}}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp308=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp308 != 0;_tmp308=_tmp308->tl){struct _tuple8 _tmp30A;struct Cyc_List_List*
_tmp30B;struct Cyc_Absyn_Exp*_tmp30C;struct _tuple8*_tmp309=(struct _tuple8*)
_tmp308->hd;_tmp30A=*_tmp309;_tmp30B=_tmp30A.f1;_tmp30C=_tmp30A.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp30B == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp30B->tl != 0)({void*_tmp30D=0;((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp30E="multiple designators in array";
_tag_dynforward(_tmp30E,sizeof(char),_get_zero_arr_size_char(_tmp30E,30));}),
_tag_dynforward(_tmp30D,sizeof(void*),0));});{void*_tmp30F=(void*)_tmp30B->hd;
void*_tmp310=_tmp30F;struct Cyc_Absyn_Exp*_tmp311;_LL168: if(*((int*)_tmp310)!= 0)
goto _LL16A;_tmp311=((struct Cyc_Absyn_ArrayElement_struct*)_tmp310)->f1;_LL169:
Cyc_Toc_exp_to_c(nv,_tmp311);e_index=_tmp311;goto _LL167;_LL16A: if(*((int*)
_tmp310)!= 1)goto _LL167;_LL16B:({void*_tmp312=0;((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp313="field name designators in array";
_tag_dynforward(_tmp313,sizeof(char),_get_zero_arr_size_char(_tmp313,32));}),
_tag_dynforward(_tmp312,sizeof(void*),0));});_LL167:;}}{struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,e_index,0);void*_tmp314=(void*)_tmp30C->r;struct Cyc_List_List*
_tmp315;struct Cyc_Absyn_Vardecl*_tmp316;struct Cyc_Absyn_Exp*_tmp317;struct Cyc_Absyn_Exp*
_tmp318;int _tmp319;void*_tmp31A;struct Cyc_List_List*_tmp31B;_LL16D: if(*((int*)
_tmp314)!= 28)goto _LL16F;_tmp315=((struct Cyc_Absyn_Array_e_struct*)_tmp314)->f1;
_LL16E: s=Cyc_Toc_init_array(nv,lval,_tmp315,s);goto _LL16C;_LL16F: if(*((int*)
_tmp314)!= 29)goto _LL171;_tmp316=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp314)->f1;_tmp317=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp314)->f2;
_tmp318=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp314)->f3;_tmp319=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp314)->f4;_LL170: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp316,_tmp317,_tmp318,_tmp319,s,0);goto _LL16C;_LL171: if(*((int*)
_tmp314)!= 31)goto _LL173;_tmp31A=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp314)->f1;_tmp31B=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp314)->f2;_LL172:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp31A,_tmp31B,s);goto _LL16C;_LL173:;_LL174:
Cyc_Toc_exp_to_c(nv,_tmp30C);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp30C,0),s,0);goto _LL16C;_LL16C:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated){struct _tuple1*_tmp31C=vd->name;
void*_tmp31D=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!e1_already_translated)Cyc_Toc_exp_to_c(nv,e1);{struct _RegionHandle _tmp31E=
_new_region("r2");struct _RegionHandle*r2=& _tmp31E;_push_region(r2);{struct Cyc_Toc_Env*
nv2=Cyc_Toc_add_varmap(r2,nv,_tmp31C,Cyc_Absyn_varb_exp(_tmp31C,(void*)({struct
Cyc_Absyn_Local_b_struct*_tmp328=_cycalloc(sizeof(*_tmp328));_tmp328[0]=({struct
Cyc_Absyn_Local_b_struct _tmp329;_tmp329.tag=3;_tmp329.f1=vd;_tmp329;});_tmp328;}),
0));struct _tuple1*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp31C,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp31C,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp31C,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp31C,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp31F=(void*)e2->r;struct Cyc_List_List*
_tmp320;struct Cyc_Absyn_Vardecl*_tmp321;struct Cyc_Absyn_Exp*_tmp322;struct Cyc_Absyn_Exp*
_tmp323;int _tmp324;void*_tmp325;struct Cyc_List_List*_tmp326;_LL176: if(*((int*)
_tmp31F)!= 28)goto _LL178;_tmp320=((struct Cyc_Absyn_Array_e_struct*)_tmp31F)->f1;
_LL177: body=Cyc_Toc_init_array(nv2,lval,_tmp320,Cyc_Toc_skip_stmt_dl());goto
_LL175;_LL178: if(*((int*)_tmp31F)!= 29)goto _LL17A;_tmp321=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp31F)->f1;_tmp322=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp31F)->f2;
_tmp323=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp31F)->f3;_tmp324=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp31F)->f4;_LL179: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp321,_tmp322,_tmp323,_tmp324,Cyc_Toc_skip_stmt_dl(),0);goto _LL175;
_LL17A: if(*((int*)_tmp31F)!= 31)goto _LL17C;_tmp325=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp31F)->f1;_tmp326=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp31F)->f2;_LL17B:
body=Cyc_Toc_init_anon_struct(nv,lval,_tmp325,_tmp326,Cyc_Toc_skip_stmt_dl());
goto _LL175;_LL17C:;_LL17D: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL175;_LL175:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp((void*)lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(
_tmp31D,Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(
ex,0),0);}{struct Cyc_Absyn_Stmt*_tmp327=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(
max,Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp31C,
Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp327;}}};
_pop_region(r2);}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp32A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp32A != 0;_tmp32A=_tmp32A->tl){struct _tuple8 _tmp32C;
struct Cyc_List_List*_tmp32D;struct Cyc_Absyn_Exp*_tmp32E;struct _tuple8*_tmp32B=(
struct _tuple8*)_tmp32A->hd;_tmp32C=*_tmp32B;_tmp32D=_tmp32C.f1;_tmp32E=_tmp32C.f2;
if(_tmp32D == 0)({void*_tmp32F=0;((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp330="empty designator list";
_tag_dynforward(_tmp330,sizeof(char),_get_zero_arr_size_char(_tmp330,22));}),
_tag_dynforward(_tmp32F,sizeof(void*),0));});if(_tmp32D->tl != 0)({void*_tmp331=0;((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp332="too many designators in anonymous struct";_tag_dynforward(
_tmp332,sizeof(char),_get_zero_arr_size_char(_tmp332,41));}),_tag_dynforward(
_tmp331,sizeof(void*),0));});{void*_tmp333=(void*)_tmp32D->hd;struct
_dynforward_ptr*_tmp334;_LL17F: if(*((int*)_tmp333)!= 1)goto _LL181;_tmp334=((
struct Cyc_Absyn_FieldName_struct*)_tmp333)->f1;_LL180: {struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_aggrmember_exp(lhs,_tmp334,0);{void*_tmp335=(void*)_tmp32E->r;struct
Cyc_List_List*_tmp336;struct Cyc_Absyn_Vardecl*_tmp337;struct Cyc_Absyn_Exp*
_tmp338;struct Cyc_Absyn_Exp*_tmp339;int _tmp33A;void*_tmp33B;struct Cyc_List_List*
_tmp33C;_LL184: if(*((int*)_tmp335)!= 28)goto _LL186;_tmp336=((struct Cyc_Absyn_Array_e_struct*)
_tmp335)->f1;_LL185: s=Cyc_Toc_init_array(nv,lval,_tmp336,s);goto _LL183;_LL186:
if(*((int*)_tmp335)!= 29)goto _LL188;_tmp337=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp335)->f1;_tmp338=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp335)->f2;
_tmp339=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp335)->f3;_tmp33A=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp335)->f4;_LL187: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp337,_tmp338,_tmp339,_tmp33A,s,0);goto _LL183;_LL188: if(*((int*)
_tmp335)!= 31)goto _LL18A;_tmp33B=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp335)->f1;_tmp33C=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp335)->f2;_LL189:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp33B,_tmp33C,s);goto _LL183;_LL18A:;_LL18B:
Cyc_Toc_exp_to_c(nv,_tmp32E);if(Cyc_Toc_is_poly_field(struct_type,_tmp334))
_tmp32E=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp32E);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp32E,0),0),s,0);goto _LL183;
_LL183:;}goto _LL17E;}_LL181:;_LL182:({void*_tmp33D=0;((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp33E="array designator in struct";_tag_dynforward(_tmp33E,sizeof(char),
_get_zero_arr_size_char(_tmp33E,27));}),_tag_dynforward(_tmp33D,sizeof(void*),0));});
_LL17E:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct
_RegionHandle _tmp33F=_new_region("r");struct _RegionHandle*r=& _tmp33F;
_push_region(r);{struct Cyc_List_List*_tmp340=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);void*_tmp341=Cyc_Toc_add_tuple_type(
_tmp340);struct _tuple1*_tmp342=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp343=
Cyc_Absyn_var_exp(_tmp342,0);struct Cyc_Absyn_Stmt*_tmp344=Cyc_Absyn_exp_stmt(
_tmp343,0);struct Cyc_Absyn_Exp*(*_tmp345)(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_List_List*_tmp346=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp346);for(0;_tmp346 != 0;(_tmp346=
_tmp346->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp346->hd;struct
Cyc_Absyn_Exp*lval=_tmp345(_tmp343,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp347=(void*)e->r;struct Cyc_List_List*_tmp348;struct Cyc_Absyn_Vardecl*
_tmp349;struct Cyc_Absyn_Exp*_tmp34A;struct Cyc_Absyn_Exp*_tmp34B;int _tmp34C;
_LL18D: if(*((int*)_tmp347)!= 28)goto _LL18F;_tmp348=((struct Cyc_Absyn_Array_e_struct*)
_tmp347)->f1;_LL18E: _tmp344=Cyc_Toc_init_array(nv,lval,_tmp348,_tmp344);goto
_LL18C;_LL18F: if(*((int*)_tmp347)!= 29)goto _LL191;_tmp349=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp347)->f1;_tmp34A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp347)->f2;
_tmp34B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp347)->f3;_tmp34C=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp347)->f4;_LL190: _tmp344=Cyc_Toc_init_comprehension(
nv,lval,_tmp349,_tmp34A,_tmp34B,_tmp34C,_tmp344,0);goto _LL18C;_LL191:;_LL192: Cyc_Toc_exp_to_c(
nv,e);_tmp344=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp345(
_tmp343,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp344,0);goto _LL18C;_LL18C:;}}}{
struct Cyc_Absyn_Exp*_tmp34D=Cyc_Toc_make_struct(nv,_tmp342,_tmp341,_tmp344,
pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp34D;}};_pop_region(r);}
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*
struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*
dles,struct _tuple1*tdn){struct _tuple1*_tmp34E=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp34F=Cyc_Absyn_var_exp(_tmp34E,0);struct Cyc_Absyn_Stmt*_tmp350=Cyc_Absyn_exp_stmt(
_tmp34F,0);struct Cyc_Absyn_Exp*(*_tmp351)(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;void*_tmp352=Cyc_Toc_aggrdecl_type(tdn,Cyc_Absyn_strctq);
int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp353=Cyc_Tcutil_compress(
struct_type);struct Cyc_Absyn_AggrInfo _tmp354;union Cyc_Absyn_AggrInfoU_union
_tmp355;_LL194: if(_tmp353 <= (void*)4)goto _LL196;if(*((int*)_tmp353)!= 10)goto
_LL196;_tmp354=((struct Cyc_Absyn_AggrType_struct*)_tmp353)->f1;_tmp355=_tmp354.aggr_info;
_LL195: ad=Cyc_Absyn_get_known_aggrdecl(_tmp355);goto _LL193;_LL196:;_LL197:({void*
_tmp356=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp357="init_struct: bad struct type";_tag_dynforward(_tmp357,sizeof(
char),_get_zero_arr_size_char(_tmp357,29));}),_tag_dynforward(_tmp356,sizeof(
void*),0));});_LL193:;}{struct _RegionHandle _tmp358=_new_region("r");struct
_RegionHandle*r=& _tmp358;_push_region(r);{struct Cyc_List_List*_tmp359=((struct
Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,
dles);for(0;_tmp359 != 0;_tmp359=_tmp359->tl){struct _tuple8 _tmp35B;struct Cyc_List_List*
_tmp35C;struct Cyc_Absyn_Exp*_tmp35D;struct _tuple8*_tmp35A=(struct _tuple8*)
_tmp359->hd;_tmp35B=*_tmp35A;_tmp35C=_tmp35B.f1;_tmp35D=_tmp35B.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp35D->topt))->v);
if(_tmp35C == 0)({void*_tmp35E=0;((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp35F="empty designator list";
_tag_dynforward(_tmp35F,sizeof(char),_get_zero_arr_size_char(_tmp35F,22));}),
_tag_dynforward(_tmp35E,sizeof(void*),0));});if(_tmp35C->tl != 0){struct _tuple1*
_tmp360=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp361=Cyc_Absyn_var_exp(_tmp360,
0);for(0;_tmp35C != 0;_tmp35C=_tmp35C->tl){void*_tmp362=(void*)_tmp35C->hd;struct
_dynforward_ptr*_tmp363;_LL199: if(*((int*)_tmp362)!= 1)goto _LL19B;_tmp363=((
struct Cyc_Absyn_FieldName_struct*)_tmp362)->f1;_LL19A: if(Cyc_Toc_is_poly_field(
struct_type,_tmp363))_tmp361=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp361);
_tmp350=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp351(
_tmp34F,_tmp363,0),_tmp361,0),0),_tmp350,0);goto _LL198;_LL19B:;_LL19C:({void*
_tmp364=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp365="array designator in struct";_tag_dynforward(_tmp365,sizeof(
char),_get_zero_arr_size_char(_tmp365,27));}),_tag_dynforward(_tmp364,sizeof(
void*),0));});_LL198:;}Cyc_Toc_exp_to_c(nv,_tmp35D);_tmp350=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp361,_tmp35D,0),0),_tmp350,0);}else{
void*_tmp366=(void*)_tmp35C->hd;struct _dynforward_ptr*_tmp367;_LL19E: if(*((int*)
_tmp366)!= 1)goto _LL1A0;_tmp367=((struct Cyc_Absyn_FieldName_struct*)_tmp366)->f1;
_LL19F: {struct Cyc_Absyn_Exp*lval=_tmp351(_tmp34F,_tmp367,0);{void*_tmp368=(void*)
_tmp35D->r;struct Cyc_List_List*_tmp369;struct Cyc_Absyn_Vardecl*_tmp36A;struct Cyc_Absyn_Exp*
_tmp36B;struct Cyc_Absyn_Exp*_tmp36C;int _tmp36D;void*_tmp36E;struct Cyc_List_List*
_tmp36F;_LL1A3: if(*((int*)_tmp368)!= 28)goto _LL1A5;_tmp369=((struct Cyc_Absyn_Array_e_struct*)
_tmp368)->f1;_LL1A4: _tmp350=Cyc_Toc_init_array(nv,lval,_tmp369,_tmp350);goto
_LL1A2;_LL1A5: if(*((int*)_tmp368)!= 29)goto _LL1A7;_tmp36A=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp368)->f1;_tmp36B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp368)->f2;
_tmp36C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp368)->f3;_tmp36D=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp368)->f4;_LL1A6: _tmp350=Cyc_Toc_init_comprehension(
nv,lval,_tmp36A,_tmp36B,_tmp36C,_tmp36D,_tmp350,0);goto _LL1A2;_LL1A7: if(*((int*)
_tmp368)!= 31)goto _LL1A9;_tmp36E=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp368)->f1;_tmp36F=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp368)->f2;_LL1A8:
_tmp350=Cyc_Toc_init_anon_struct(nv,lval,_tmp36E,_tmp36F,_tmp350);goto _LL1A2;
_LL1A9:;_LL1AA: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp35D->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp35D);{struct Cyc_Absyn_Aggrfield*
_tmp370=Cyc_Absyn_lookup_decl_field(ad,_tmp367);if(Cyc_Toc_is_poly_field(
struct_type,_tmp367) && !was_ptr_type)_tmp35D=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp35D);if(has_exists)_tmp35D=Cyc_Toc_cast_it((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(_tmp370))->type,_tmp35D);_tmp350=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(lval,_tmp35D,0),0),_tmp350,0);goto _LL1A2;}}_LL1A2:;}goto
_LL19D;}_LL1A0:;_LL1A1:({void*_tmp371=0;((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp372="array designator in struct";
_tag_dynforward(_tmp372,sizeof(char),_get_zero_arr_size_char(_tmp372,27));}),
_tag_dynforward(_tmp371,sizeof(void*),0));});_LL19D:;}}}{struct Cyc_Absyn_Exp*
_tmp373=Cyc_Toc_make_struct(nv,_tmp34E,_tmp352,_tmp350,pointer,rgnopt,is_atomic);
_npop_handler(0);return _tmp373;};_pop_region(r);}}struct _tuple10{struct Cyc_Core_Opt*
f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp*el,struct _tuple10*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*
pr).f2,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*
e1,int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmp374=_cycalloc(sizeof(*_tmp374));_tmp374[0]=({struct Cyc_Absyn_Increment_e_struct
_tmp375;_tmp375.tag=5;_tmp375.f1=e1;_tmp375.f2=(void*)incr;_tmp375;});_tmp374;}),
0);}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp376=(void*)e1->r;struct Cyc_Absyn_Stmt*
_tmp377;void*_tmp378;struct Cyc_Absyn_Exp*_tmp379;struct Cyc_Absyn_Exp*_tmp37A;
struct _dynforward_ptr*_tmp37B;_LL1AC: if(*((int*)_tmp376)!= 38)goto _LL1AE;_tmp377=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp376)->f1;_LL1AD: Cyc_Toc_lvalue_assign_stmt(
_tmp377,fs,f,f_env);goto _LL1AB;_LL1AE: if(*((int*)_tmp376)!= 15)goto _LL1B0;
_tmp378=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp376)->f1;_tmp379=((struct Cyc_Absyn_Cast_e_struct*)
_tmp376)->f2;_LL1AF: Cyc_Toc_lvalue_assign(_tmp379,fs,f,f_env);goto _LL1AB;_LL1B0:
if(*((int*)_tmp376)!= 23)goto _LL1B2;_tmp37A=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp376)->f1;_tmp37B=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp376)->f2;_LL1B1:(
void*)(e1->r=(void*)((void*)_tmp37A->r));Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp37C=_cycalloc(sizeof(*_tmp37C));_tmp37C->hd=_tmp37B;
_tmp37C->tl=fs;_tmp37C;}),f,f_env);goto _LL1AB;_LL1B2:;_LL1B3: {struct Cyc_Absyn_Exp*
e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){e1_copy=Cyc_Absyn_aggrmember_exp(
e1_copy,(struct _dynforward_ptr*)fs->hd,e1_copy->loc);}(void*)(e1->r=(void*)((
void*)(f(e1_copy,f_env))->r));goto _LL1AB;}_LL1AB:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env){void*_tmp37D=(void*)s->r;struct Cyc_Absyn_Exp*_tmp37E;struct
Cyc_Absyn_Decl*_tmp37F;struct Cyc_Absyn_Stmt*_tmp380;struct Cyc_Absyn_Stmt*_tmp381;
_LL1B5: if(_tmp37D <= (void*)1)goto _LL1BB;if(*((int*)_tmp37D)!= 0)goto _LL1B7;
_tmp37E=((struct Cyc_Absyn_Exp_s_struct*)_tmp37D)->f1;_LL1B6: Cyc_Toc_lvalue_assign(
_tmp37E,fs,f,f_env);goto _LL1B4;_LL1B7: if(*((int*)_tmp37D)!= 11)goto _LL1B9;
_tmp37F=((struct Cyc_Absyn_Decl_s_struct*)_tmp37D)->f1;_tmp380=((struct Cyc_Absyn_Decl_s_struct*)
_tmp37D)->f2;_LL1B8: Cyc_Toc_lvalue_assign_stmt(_tmp380,fs,f,f_env);goto _LL1B4;
_LL1B9: if(*((int*)_tmp37D)!= 1)goto _LL1BB;_tmp381=((struct Cyc_Absyn_Seq_s_struct*)
_tmp37D)->f2;_LL1BA: Cyc_Toc_lvalue_assign_stmt(_tmp381,fs,f,f_env);goto _LL1B4;
_LL1BB:;_LL1BC:({struct Cyc_String_pa_struct _tmp384;_tmp384.tag=0;_tmp384.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_stmt2string(s));{void*
_tmp382[1]={& _tmp384};Cyc_Toc_toc_impos(({const char*_tmp383="lvalue_assign_stmt: %s";
_tag_dynforward(_tmp383,sizeof(char),_get_zero_arr_size_char(_tmp383,23));}),
_tag_dynforward(_tmp382,sizeof(void*),1));}});_LL1B4:;}static struct Cyc_List_List*
Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x){struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0)return 0;result=({
struct Cyc_List_List*_tmp385=_region_malloc(r2,sizeof(*_tmp385));_tmp385->hd=(
void*)f((void*)x->hd,env);_tmp385->tl=0;_tmp385;});prev=result;for(x=x->tl;x != 0;
x=x->tl){((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*
_tmp386=_region_malloc(r2,sizeof(*_tmp386));_tmp386->hd=(void*)f((void*)x->hd,
env);_tmp386->tl=0;_tmp386;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,
x);}static struct _tuple8*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){return({struct
_tuple8*_tmp387=_cycalloc(sizeof(*_tmp387));_tmp387->f1=0;_tmp387->f2=e;_tmp387;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp388=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp389;_LL1BE: if(_tmp388 <= (void*)4)goto _LL1C0;if(*((
int*)_tmp388)!= 4)goto _LL1C0;_tmp389=((struct Cyc_Absyn_PointerType_struct*)
_tmp388)->f1;_LL1BF: return _tmp389;_LL1C0:;_LL1C1:({void*_tmp38A=0;((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp38B="get_ptr_typ: not a pointer!";_tag_dynforward(_tmp38B,sizeof(char),
_get_zero_arr_size_char(_tmp38B,28));}),_tag_dynforward(_tmp38A,sizeof(void*),0));});
_LL1BD:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp38C=Cyc_Tcutil_compress(t);void*_tmp38D;void*_tmp38E;void*_tmp38F;
void*_tmp390;void*_tmp391;void*_tmp392;void*_tmp393;void*_tmp394;void*_tmp395;
void*_tmp396;_LL1C3: if(_tmp38C <= (void*)4)goto _LL1D3;if(*((int*)_tmp38C)!= 4)
goto _LL1C5;_LL1C4: res=Cyc_Absyn_null_exp(0);goto _LL1C2;_LL1C5: if(*((int*)_tmp38C)
!= 5)goto _LL1C7;_tmp38D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp38C)->f1;
_tmp38E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp38C)->f2;if((int)_tmp38E != 
0)goto _LL1C7;_LL1C6: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union
Cyc_Absyn_Cnst_union _tmp397;(_tmp397.Char_c).tag=0;(_tmp397.Char_c).f1=(void*)
_tmp38D;(_tmp397.Char_c).f2='\000';_tmp397;}),0);goto _LL1C2;_LL1C7: if(*((int*)
_tmp38C)!= 5)goto _LL1C9;_tmp38F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp38C)->f1;
_tmp390=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp38C)->f2;if((int)_tmp390 != 
1)goto _LL1C9;_LL1C8: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union
Cyc_Absyn_Cnst_union _tmp398;(_tmp398.Short_c).tag=1;(_tmp398.Short_c).f1=(void*)
_tmp38F;(_tmp398.Short_c).f2=0;_tmp398;}),0);goto _LL1C2;_LL1C9: if(*((int*)
_tmp38C)!= 12)goto _LL1CB;_LL1CA: goto _LL1CC;_LL1CB: if(*((int*)_tmp38C)!= 13)goto
_LL1CD;_LL1CC: _tmp391=(void*)1;goto _LL1CE;_LL1CD: if(*((int*)_tmp38C)!= 5)goto
_LL1CF;_tmp391=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp38C)->f1;_tmp392=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp38C)->f2;if((int)_tmp392 != 2)goto
_LL1CF;_LL1CE: _tmp393=_tmp391;goto _LL1D0;_LL1CF: if(*((int*)_tmp38C)!= 5)goto
_LL1D1;_tmp393=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp38C)->f1;_tmp394=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp38C)->f2;if((int)_tmp394 != 3)goto
_LL1D1;_LL1D0: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp399;(_tmp399.Int_c).tag=2;(_tmp399.Int_c).f1=(void*)_tmp393;(_tmp399.Int_c).f2=
0;_tmp399;}),0);goto _LL1C2;_LL1D1: if(*((int*)_tmp38C)!= 5)goto _LL1D3;_tmp395=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp38C)->f1;_tmp396=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp38C)->f2;if((int)_tmp396 != 4)goto _LL1D3;_LL1D2: res=Cyc_Absyn_const_exp((
union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp39A;(_tmp39A.LongLong_c).tag=
3;(_tmp39A.LongLong_c).f1=(void*)_tmp395;(_tmp39A.LongLong_c).f2=(long long)0;
_tmp39A;}),0);goto _LL1C2;_LL1D3: if((int)_tmp38C != 1)goto _LL1D5;_LL1D4: goto _LL1D6;
_LL1D5: if(_tmp38C <= (void*)4)goto _LL1D7;if(*((int*)_tmp38C)!= 6)goto _LL1D7;
_LL1D6: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp39B;(_tmp39B.Float_c).tag=4;(_tmp39B.Float_c).f1=({const char*_tmp39C="0.0";
_tag_dynforward(_tmp39C,sizeof(char),_get_zero_arr_size_char(_tmp39C,4));});
_tmp39B;}),0);goto _LL1C2;_LL1D7:;_LL1D8:({struct Cyc_String_pa_struct _tmp39F;
_tmp39F.tag=0;_tmp39F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp39D[1]={& _tmp39F};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp39E="found non-zero type %s in generate_zero";
_tag_dynforward(_tmp39E,sizeof(char),_get_zero_arr_size_char(_tmp39E,40));}),
_tag_dynforward(_tmp39D,sizeof(void*),1));}});_LL1C2:;}res->topt=({struct Cyc_Core_Opt*
_tmp3A0=_cycalloc(sizeof(*_tmp3A0));_tmp3A0->v=(void*)t;_tmp3A0;});return res;}
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dynforward,int is_dyneither,void*elt_type){int is_fat=is_dynforward
 || is_dyneither;void*fat_ptr_type;if(is_dynforward)fat_ptr_type=Cyc_Absyn_dynforward_typ(
elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);else{fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);}{void*_tmp3A1=Cyc_Toc_typ_to_c(
elt_type);void*_tmp3A2=Cyc_Toc_typ_to_c(fat_ptr_type);void*_tmp3A3=Cyc_Absyn_cstar_typ(
_tmp3A1,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*_tmp3A4=({struct Cyc_Core_Opt*_tmp3E2=
_cycalloc(sizeof(*_tmp3E2));_tmp3E2->v=(void*)_tmp3A3;_tmp3E2;});struct Cyc_Absyn_Exp*
xinit;{void*_tmp3A5=(void*)e1->r;struct Cyc_Absyn_Exp*_tmp3A6;struct Cyc_Absyn_Exp*
_tmp3A7;struct Cyc_Absyn_Exp*_tmp3A8;_LL1DA: if(*((int*)_tmp3A5)!= 22)goto _LL1DC;
_tmp3A6=((struct Cyc_Absyn_Deref_e_struct*)_tmp3A5)->f1;_LL1DB: if(!is_fat){
_tmp3A6=Cyc_Toc_cast_it(fat_ptr_type,_tmp3A6);_tmp3A6->topt=({struct Cyc_Core_Opt*
_tmp3A9=_cycalloc(sizeof(*_tmp3A9));_tmp3A9->v=(void*)fat_ptr_type;_tmp3A9;});}
Cyc_Toc_exp_to_c(nv,_tmp3A6);xinit=_tmp3A6;goto _LL1D9;_LL1DC: if(*((int*)_tmp3A5)
!= 25)goto _LL1DE;_tmp3A7=((struct Cyc_Absyn_Subscript_e_struct*)_tmp3A5)->f1;
_tmp3A8=((struct Cyc_Absyn_Subscript_e_struct*)_tmp3A5)->f2;_LL1DD: if(!is_fat){
_tmp3A7=Cyc_Toc_cast_it(fat_ptr_type,_tmp3A7);_tmp3A7->topt=({struct Cyc_Core_Opt*
_tmp3AA=_cycalloc(sizeof(*_tmp3AA));_tmp3AA->v=(void*)fat_ptr_type;_tmp3AA;});}
Cyc_Toc_exp_to_c(nv,_tmp3A7);Cyc_Toc_exp_to_c(nv,_tmp3A8);if(is_dynforward)xinit=
Cyc_Absyn_fncall_exp(Cyc_Toc__dynforward_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp3AB[3];_tmp3AB[2]=_tmp3A8;_tmp3AB[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp3AB[0]=_tmp3A7;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp3AB,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
else{xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp3AC[3];_tmp3AC[2]=_tmp3A8;_tmp3AC[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp3AC[0]=_tmp3A7;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp3AC,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}
goto _LL1D9;_LL1DE:;_LL1DF:({void*_tmp3AD=0;((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3AE="found bad lhs for zero-terminated pointer assignment";
_tag_dynforward(_tmp3AE,sizeof(char),_get_zero_arr_size_char(_tmp3AE,53));}),
_tag_dynforward(_tmp3AD,sizeof(void*),0));});_LL1D9:;}{struct _tuple1*_tmp3AF=Cyc_Toc_temp_var();
struct _RegionHandle _tmp3B0=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp3B0;
_push_region(rgn2);{struct Cyc_Toc_Env*_tmp3B1=Cyc_Toc_add_varmap(rgn2,nv,_tmp3AF,
Cyc_Absyn_var_exp(_tmp3AF,0));struct Cyc_Absyn_Vardecl*_tmp3B2=({struct Cyc_Absyn_Vardecl*
_tmp3E1=_cycalloc(sizeof(*_tmp3E1));_tmp3E1->sc=(void*)((void*)2);_tmp3E1->name=
_tmp3AF;_tmp3E1->tq=Cyc_Toc_mt_tq;_tmp3E1->type=(void*)_tmp3A2;_tmp3E1->initializer=(
struct Cyc_Absyn_Exp*)xinit;_tmp3E1->rgn=0;_tmp3E1->attributes=0;_tmp3E1->escapes=
0;_tmp3E1;});struct Cyc_Absyn_Local_b_struct*_tmp3B3=({struct Cyc_Absyn_Local_b_struct*
_tmp3DF=_cycalloc(sizeof(*_tmp3DF));_tmp3DF[0]=({struct Cyc_Absyn_Local_b_struct
_tmp3E0;_tmp3E0.tag=3;_tmp3E0.f1=_tmp3B2;_tmp3E0;});_tmp3DF;});struct Cyc_Absyn_Exp*
_tmp3B4=Cyc_Absyn_varb_exp(_tmp3AF,(void*)_tmp3B3,0);_tmp3B4->topt=({struct Cyc_Core_Opt*
_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5->v=(void*)fat_ptr_type;_tmp3B5;});{
struct Cyc_Absyn_Exp*_tmp3B6=Cyc_Absyn_deref_exp(_tmp3B4,0);_tmp3B6->topt=({
struct Cyc_Core_Opt*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7->v=(void*)
elt_type;_tmp3B7;});Cyc_Toc_exp_to_c(_tmp3B1,_tmp3B6);{struct _tuple1*_tmp3B8=Cyc_Toc_temp_var();
_tmp3B1=Cyc_Toc_add_varmap(rgn2,_tmp3B1,_tmp3B8,Cyc_Absyn_var_exp(_tmp3B8,0));{
struct Cyc_Absyn_Vardecl*_tmp3B9=({struct Cyc_Absyn_Vardecl*_tmp3DE=_cycalloc(
sizeof(*_tmp3DE));_tmp3DE->sc=(void*)((void*)2);_tmp3DE->name=_tmp3B8;_tmp3DE->tq=
Cyc_Toc_mt_tq;_tmp3DE->type=(void*)_tmp3A1;_tmp3DE->initializer=(struct Cyc_Absyn_Exp*)
_tmp3B6;_tmp3DE->rgn=0;_tmp3DE->attributes=0;_tmp3DE->escapes=0;_tmp3DE;});
struct Cyc_Absyn_Local_b_struct*_tmp3BA=({struct Cyc_Absyn_Local_b_struct*_tmp3DC=
_cycalloc(sizeof(*_tmp3DC));_tmp3DC[0]=({struct Cyc_Absyn_Local_b_struct _tmp3DD;
_tmp3DD.tag=3;_tmp3DD.f1=_tmp3B9;_tmp3DD;});_tmp3DC;});struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp3BB=Cyc_Absyn_varb_exp(_tmp3B8,(
void*)_tmp3BA,0);_tmp3BB->topt=_tmp3B6->topt;z_init=Cyc_Absyn_prim2_exp((void*)
popt->v,_tmp3BB,e2,0);z_init->topt=_tmp3BB->topt;}Cyc_Toc_exp_to_c(_tmp3B1,
z_init);{struct _tuple1*_tmp3BC=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmp3BD=({struct Cyc_Absyn_Vardecl*_tmp3DB=_cycalloc(sizeof(*_tmp3DB));_tmp3DB->sc=(
void*)((void*)2);_tmp3DB->name=_tmp3BC;_tmp3DB->tq=Cyc_Toc_mt_tq;_tmp3DB->type=(
void*)_tmp3A1;_tmp3DB->initializer=(struct Cyc_Absyn_Exp*)z_init;_tmp3DB->rgn=0;
_tmp3DB->attributes=0;_tmp3DB->escapes=0;_tmp3DB;});struct Cyc_Absyn_Local_b_struct*
_tmp3BE=({struct Cyc_Absyn_Local_b_struct*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));
_tmp3D9[0]=({struct Cyc_Absyn_Local_b_struct _tmp3DA;_tmp3DA.tag=3;_tmp3DA.f1=
_tmp3BD;_tmp3DA;});_tmp3D9;});_tmp3B1=Cyc_Toc_add_varmap(rgn2,_tmp3B1,_tmp3BC,
Cyc_Absyn_var_exp(_tmp3BC,0));{struct Cyc_Absyn_Exp*_tmp3BF=Cyc_Absyn_varb_exp(
_tmp3B8,(void*)_tmp3BA,0);_tmp3BF->topt=_tmp3B6->topt;{struct Cyc_Absyn_Exp*
_tmp3C0=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*_tmp3C1=Cyc_Absyn_prim2_exp((
void*)5,_tmp3BF,_tmp3C0,0);_tmp3C1->topt=({struct Cyc_Core_Opt*_tmp3C2=_cycalloc(
sizeof(*_tmp3C2));_tmp3C2->v=(void*)Cyc_Absyn_sint_typ;_tmp3C2;});Cyc_Toc_exp_to_c(
_tmp3B1,_tmp3C1);{struct Cyc_Absyn_Exp*_tmp3C3=Cyc_Absyn_varb_exp(_tmp3BC,(void*)
_tmp3BE,0);_tmp3C3->topt=_tmp3B6->topt;{struct Cyc_Absyn_Exp*_tmp3C4=Cyc_Toc_generate_zero(
elt_type);struct Cyc_Absyn_Exp*_tmp3C5=Cyc_Absyn_prim2_exp((void*)6,_tmp3C3,
_tmp3C4,0);_tmp3C5->topt=({struct Cyc_Core_Opt*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));
_tmp3C6->v=(void*)Cyc_Absyn_sint_typ;_tmp3C6;});Cyc_Toc_exp_to_c(_tmp3B1,_tmp3C5);{
struct Cyc_List_List*_tmp3C7=({struct Cyc_Absyn_Exp*_tmp3D8[2];_tmp3D8[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0);_tmp3D8[0]=Cyc_Absyn_varb_exp(_tmp3AF,(void*)
_tmp3B3,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3D8,sizeof(struct Cyc_Absyn_Exp*),2));});struct Cyc_Absyn_Exp*
_tmp3C8=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;if(is_dynforward)xsize=
Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dynforward_size_e,
_tmp3C7,0),_tmp3C8,0);else{xsize=Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(
Cyc_Toc__get_dyneither_size_e,_tmp3C7,0),_tmp3C8,0);}{struct Cyc_Absyn_Exp*
_tmp3C9=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp3C1,_tmp3C5,0),0);struct
Cyc_Absyn_Stmt*_tmp3CA=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,
0,0),0);struct Cyc_Absyn_Exp*_tmp3CB=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_varb_exp(
_tmp3AF,(void*)_tmp3B3,0),Cyc_Toc_curr_sp,0);_tmp3CB=Cyc_Toc_cast_it(_tmp3A3,
_tmp3CB);{struct Cyc_Absyn_Exp*_tmp3CC=Cyc_Absyn_deref_exp(_tmp3CB,0);struct Cyc_Absyn_Exp*
_tmp3CD=Cyc_Absyn_assign_exp(_tmp3CC,Cyc_Absyn_var_exp(_tmp3BC,0),0);struct Cyc_Absyn_Stmt*
_tmp3CE=Cyc_Absyn_exp_stmt(_tmp3CD,0);_tmp3CE=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp3C9,_tmp3CA,Cyc_Absyn_skip_stmt(0),0),_tmp3CE,0);_tmp3CE=Cyc_Absyn_decl_stmt(({
struct Cyc_Absyn_Decl*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));_tmp3D0[
0]=({struct Cyc_Absyn_Var_d_struct _tmp3D1;_tmp3D1.tag=0;_tmp3D1.f1=_tmp3BD;
_tmp3D1;});_tmp3D0;}));_tmp3CF->loc=0;_tmp3CF;}),_tmp3CE,0);_tmp3CE=Cyc_Absyn_decl_stmt(({
struct Cyc_Absyn_Decl*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3[
0]=({struct Cyc_Absyn_Var_d_struct _tmp3D4;_tmp3D4.tag=0;_tmp3D4.f1=_tmp3B9;
_tmp3D4;});_tmp3D3;}));_tmp3D2->loc=0;_tmp3D2;}),_tmp3CE,0);_tmp3CE=Cyc_Absyn_decl_stmt(({
struct Cyc_Absyn_Decl*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));_tmp3D5->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6[
0]=({struct Cyc_Absyn_Var_d_struct _tmp3D7;_tmp3D7.tag=0;_tmp3D7.f1=_tmp3B2;
_tmp3D7;});_tmp3D6;}));_tmp3D5->loc=0;_tmp3D5;}),_tmp3CE,0);(void*)(e->r=(void*)
Cyc_Toc_stmt_exp_r(_tmp3CE));}}}}}}}}}}}};_pop_region(rgn2);}}}struct _tuple11{
struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple12{struct Cyc_Absyn_Aggrfield*
f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e){void*_tmp3E3=(void*)e->r;if(e->topt == 0)({struct Cyc_String_pa_struct
_tmp3E6;_tmp3E6.tag=0;_tmp3E6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp3E4[1]={& _tmp3E6};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp3E5="exp_to_c: no type for %s";_tag_dynforward(_tmp3E5,sizeof(char),
_get_zero_arr_size_char(_tmp3E5,25));}),_tag_dynforward(_tmp3E4,sizeof(void*),1));}});{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp3E7=
_tmp3E3;union Cyc_Absyn_Cnst_union _tmp3E8;struct _tuple1*_tmp3E9;void*_tmp3EA;
struct _tuple1*_tmp3EB;void*_tmp3EC;struct Cyc_List_List*_tmp3ED;struct Cyc_Absyn_Exp*
_tmp3EE;void*_tmp3EF;struct Cyc_Absyn_Exp*_tmp3F0;struct Cyc_Core_Opt*_tmp3F1;
struct Cyc_Absyn_Exp*_tmp3F2;struct Cyc_Absyn_Exp*_tmp3F3;struct Cyc_Absyn_Exp*
_tmp3F4;struct Cyc_Absyn_Exp*_tmp3F5;struct Cyc_Absyn_Exp*_tmp3F6;struct Cyc_Absyn_Exp*
_tmp3F7;struct Cyc_Absyn_Exp*_tmp3F8;struct Cyc_Absyn_Exp*_tmp3F9;struct Cyc_Absyn_Exp*
_tmp3FA;struct Cyc_Absyn_Exp*_tmp3FB;struct Cyc_Absyn_Exp*_tmp3FC;struct Cyc_List_List*
_tmp3FD;struct Cyc_Absyn_Exp*_tmp3FE;struct Cyc_List_List*_tmp3FF;struct Cyc_Absyn_VarargCallInfo*
_tmp400;struct Cyc_Absyn_Exp*_tmp401;struct Cyc_List_List*_tmp402;struct Cyc_Absyn_VarargCallInfo*
_tmp403;struct Cyc_Absyn_VarargCallInfo _tmp404;int _tmp405;struct Cyc_List_List*
_tmp406;struct Cyc_Absyn_VarargInfo*_tmp407;struct Cyc_Absyn_Exp*_tmp408;struct Cyc_Absyn_Exp*
_tmp409;struct Cyc_Absyn_Exp*_tmp40A;struct Cyc_List_List*_tmp40B;void*_tmp40C;
void**_tmp40D;struct Cyc_Absyn_Exp*_tmp40E;int _tmp40F;void*_tmp410;struct Cyc_Absyn_Exp*
_tmp411;struct Cyc_Absyn_Exp*_tmp412;struct Cyc_Absyn_Exp*_tmp413;struct Cyc_Absyn_Exp*
_tmp414;void*_tmp415;void*_tmp416;void*_tmp417;struct _dynforward_ptr*_tmp418;
void*_tmp419;void*_tmp41A;unsigned int _tmp41B;struct Cyc_Absyn_Exp*_tmp41C;struct
Cyc_Absyn_Exp*_tmp41D;struct _dynforward_ptr*_tmp41E;struct Cyc_Absyn_Exp*_tmp41F;
struct _dynforward_ptr*_tmp420;struct Cyc_Absyn_Exp*_tmp421;struct Cyc_Absyn_Exp*
_tmp422;struct Cyc_List_List*_tmp423;struct Cyc_List_List*_tmp424;struct Cyc_Absyn_Vardecl*
_tmp425;struct Cyc_Absyn_Exp*_tmp426;struct Cyc_Absyn_Exp*_tmp427;int _tmp428;
struct _tuple1*_tmp429;struct Cyc_List_List*_tmp42A;struct Cyc_List_List*_tmp42B;
struct Cyc_Absyn_Aggrdecl*_tmp42C;void*_tmp42D;struct Cyc_List_List*_tmp42E;struct
Cyc_List_List*_tmp42F;struct Cyc_Absyn_Tuniondecl*_tmp430;struct Cyc_Absyn_Tunionfield*
_tmp431;struct Cyc_List_List*_tmp432;struct Cyc_Absyn_Tuniondecl*_tmp433;struct Cyc_Absyn_Tunionfield*
_tmp434;struct Cyc_Absyn_MallocInfo _tmp435;int _tmp436;struct Cyc_Absyn_Exp*_tmp437;
void**_tmp438;struct Cyc_Absyn_Exp*_tmp439;int _tmp43A;struct Cyc_Absyn_Exp*_tmp43B;
struct Cyc_Absyn_Exp*_tmp43C;struct Cyc_Absyn_Stmt*_tmp43D;_LL1E1: if(*((int*)
_tmp3E7)!= 0)goto _LL1E3;_tmp3E8=((struct Cyc_Absyn_Const_e_struct*)_tmp3E7)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp3E7)->f1).Null_c).tag != 6)goto _LL1E3;
_LL1E2: {struct Cyc_Absyn_Exp*_tmp43E=Cyc_Absyn_uint_exp(0,0);int forward_only=0;
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ,& forward_only)){if(Cyc_Toc_is_toplevel(
nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp43E,
_tmp43E,forward_only))->r));else{if(forward_only)(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*_tmp43F[3];_tmp43F[2]=_tmp43E;
_tmp43F[1]=_tmp43E;_tmp43F[0]=_tmp43E;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp43F,sizeof(struct Cyc_Absyn_Exp*),
3));})));else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,({
struct Cyc_Absyn_Exp*_tmp440[3];_tmp440[2]=_tmp43E;_tmp440[1]=_tmp43E;_tmp440[0]=
_tmp43E;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp440,sizeof(struct Cyc_Absyn_Exp*),3));})));}}}else{(void*)(e->r=(
void*)((void*)& Cyc_Toc_zero_exp));}goto _LL1E0;}_LL1E3: if(*((int*)_tmp3E7)!= 0)
goto _LL1E5;_LL1E4: goto _LL1E0;_LL1E5: if(*((int*)_tmp3E7)!= 1)goto _LL1E7;_tmp3E9=((
struct Cyc_Absyn_Var_e_struct*)_tmp3E7)->f1;_tmp3EA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3E7)->f2;_LL1E6:{struct _handler_cons _tmp441;_push_handler(& _tmp441);{int
_tmp443=0;if(setjmp(_tmp441.handler))_tmp443=1;if(!_tmp443){(void*)(e->r=(void*)((
void*)(Cyc_Toc_lookup_varmap(nv,_tmp3E9))->r));;_pop_handler();}else{void*
_tmp442=(void*)_exn_thrown;void*_tmp445=_tmp442;_LL238: if(_tmp445 != Cyc_Dict_Absent)
goto _LL23A;_LL239:({struct Cyc_String_pa_struct _tmp448;_tmp448.tag=0;_tmp448.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp3E9));{
void*_tmp446[1]={& _tmp448};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp447="Can't find %s in exp_to_c, Var\n";
_tag_dynforward(_tmp447,sizeof(char),_get_zero_arr_size_char(_tmp447,32));}),
_tag_dynforward(_tmp446,sizeof(void*),1));}});_LL23A:;_LL23B:(void)_throw(
_tmp445);_LL237:;}}}goto _LL1E0;_LL1E7: if(*((int*)_tmp3E7)!= 2)goto _LL1E9;_tmp3EB=((
struct Cyc_Absyn_UnknownId_e_struct*)_tmp3E7)->f1;_LL1E8:({void*_tmp449=0;((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp44A="unknownid";_tag_dynforward(_tmp44A,sizeof(char),_get_zero_arr_size_char(
_tmp44A,10));}),_tag_dynforward(_tmp449,sizeof(void*),0));});_LL1E9: if(*((int*)
_tmp3E7)!= 3)goto _LL1EB;_tmp3EC=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3E7)->f1;_tmp3ED=((struct Cyc_Absyn_Primop_e_struct*)_tmp3E7)->f2;_LL1EA: {
struct Cyc_List_List*_tmp44B=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp3ED);((void(*)(void(*
f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp3ED);{void*_tmp44C=_tmp3EC;_LL23D:
if((int)_tmp44C != 19)goto _LL23F;_LL23E: {struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3ED))->hd;{void*_tmp44D=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo
_tmp44E;struct Cyc_Absyn_Exp*_tmp44F;struct Cyc_Absyn_PtrInfo _tmp450;void*_tmp451;
struct Cyc_Absyn_PtrAtts _tmp452;struct Cyc_Absyn_Conref*_tmp453;struct Cyc_Absyn_Conref*
_tmp454;struct Cyc_Absyn_Conref*_tmp455;_LL252: if(_tmp44D <= (void*)4)goto _LL256;
if(*((int*)_tmp44D)!= 7)goto _LL254;_tmp44E=((struct Cyc_Absyn_ArrayType_struct*)
_tmp44D)->f1;_tmp44F=_tmp44E.num_elts;_LL253:(void*)(e->r=(void*)((void*)((
struct Cyc_Absyn_Exp*)_check_null(_tmp44F))->r));goto _LL251;_LL254: if(*((int*)
_tmp44D)!= 4)goto _LL256;_tmp450=((struct Cyc_Absyn_PointerType_struct*)_tmp44D)->f1;
_tmp451=(void*)_tmp450.elt_typ;_tmp452=_tmp450.ptr_atts;_tmp453=_tmp452.nullable;
_tmp454=_tmp452.bounds;_tmp455=_tmp452.zero_term;_LL255:{void*_tmp456=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp454);struct Cyc_Absyn_Exp*_tmp457;_LL259: if((int)_tmp456
!= 0)goto _LL25B;_LL25A:(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dynforward_size_e,({
struct Cyc_Absyn_Exp*_tmp458[2];_tmp458[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp451),0);_tmp458[0]=(struct Cyc_Absyn_Exp*)_tmp3ED->hd;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp458,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL258;_LL25B: if((int)_tmp456 != 1)goto _LL25D;_LL25C:(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,({struct Cyc_Absyn_Exp*
_tmp459[2];_tmp459[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp451),0);
_tmp459[0]=(struct Cyc_Absyn_Exp*)_tmp3ED->hd;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp459,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL258;_LL25D: if(_tmp456 <= (void*)2)goto _LL25F;if(*((int*)_tmp456)
!= 0)goto _LL25F;_tmp457=((struct Cyc_Absyn_Upper_b_struct*)_tmp456)->f1;_LL25E:
if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp455)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp3ED->hd);(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
function_e,({struct Cyc_Absyn_Exp*_tmp45A[2];_tmp45A[1]=_tmp457;_tmp45A[0]=(
struct Cyc_Absyn_Exp*)_tmp3ED->hd;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp45A,sizeof(struct Cyc_Absyn_Exp*),2));})));}
else{if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp453))(
void*)(e->r=(void*)Cyc_Toc_conditional_exp_r(arg,_tmp457,Cyc_Absyn_uint_exp(0,0)));
else{(void*)(e->r=(void*)((void*)_tmp457->r));goto _LL258;}}goto _LL258;_LL25F: if(
_tmp456 <= (void*)2)goto _LL258;if(*((int*)_tmp456)!= 1)goto _LL258;_LL260:({void*
_tmp45B=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp45C="toc: size of AbsUpper_b";_tag_dynforward(_tmp45C,sizeof(char),
_get_zero_arr_size_char(_tmp45C,24));}),_tag_dynforward(_tmp45B,sizeof(void*),0));});
_LL258:;}goto _LL251;_LL256:;_LL257:({struct Cyc_String_pa_struct _tmp460;_tmp460.tag=
0;_tmp460.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{struct Cyc_String_pa_struct
_tmp45F;_tmp45F.tag=0;_tmp45F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{
void*_tmp45D[2]={& _tmp45F,& _tmp460};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp45E="size primop applied to non-array %s (%s)";
_tag_dynforward(_tmp45E,sizeof(char),_get_zero_arr_size_char(_tmp45E,41));}),
_tag_dynforward(_tmp45D,sizeof(void*),2));}}});_LL251:;}goto _LL23C;}_LL23F: if((
int)_tmp44C != 0)goto _LL241;_LL240:{void*_tmp461=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp44B))->hd);struct Cyc_Absyn_PtrInfo _tmp462;
void*_tmp463;struct Cyc_Absyn_PtrAtts _tmp464;struct Cyc_Absyn_Conref*_tmp465;
struct Cyc_Absyn_Conref*_tmp466;_LL262: if(_tmp461 <= (void*)4)goto _LL264;if(*((int*)
_tmp461)!= 4)goto _LL264;_tmp462=((struct Cyc_Absyn_PointerType_struct*)_tmp461)->f1;
_tmp463=(void*)_tmp462.elt_typ;_tmp464=_tmp462.ptr_atts;_tmp465=_tmp464.bounds;
_tmp466=_tmp464.zero_term;_LL263:{void*_tmp467=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp465);struct Cyc_Absyn_Exp*_tmp468;_LL267: if((int)_tmp467 != 0)goto _LL269;
_LL268: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3ED))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp3ED->tl))->hd;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__dynforward_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp469[3];_tmp469[2]=e2;
_tmp469[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp463),0);_tmp469[0]=e1;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp469,sizeof(struct Cyc_Absyn_Exp*),3));})));goto _LL266;}_LL269: if((int)_tmp467
!= 1)goto _LL26B;_LL26A: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp3ED))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3ED->tl))->hd;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp46A[3];_tmp46A[2]=e2;
_tmp46A[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp463),0);_tmp46A[0]=e1;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp46A,sizeof(struct Cyc_Absyn_Exp*),3));})));goto _LL266;}_LL26B: if(_tmp467 <= (
void*)2)goto _LL26D;if(*((int*)_tmp467)!= 0)goto _LL26D;_tmp468=((struct Cyc_Absyn_Upper_b_struct*)
_tmp467)->f1;_LL26C: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp466)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3ED))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp3ED->tl))->hd;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,e1),({struct Cyc_Absyn_Exp*
_tmp46B[3];_tmp46B[2]=e2;_tmp46B[1]=_tmp468;_tmp46B[0]=e1;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp46B,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}goto _LL266;_LL26D: if(_tmp467 <= (void*)2)goto _LL266;if(*((int*)
_tmp467)!= 1)goto _LL266;_LL26E:({void*_tmp46C=0;((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp46D="toc: plus on AbsUpper_b";
_tag_dynforward(_tmp46D,sizeof(char),_get_zero_arr_size_char(_tmp46D,24));}),
_tag_dynforward(_tmp46C,sizeof(void*),0));});_LL266:;}goto _LL261;_LL264:;_LL265:
goto _LL261;_LL261:;}goto _LL23C;_LL241: if((int)_tmp44C != 2)goto _LL243;_LL242: {
void*elt_typ=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)((struct Cyc_List_List*)_check_null(_tmp44B))->hd,& elt_typ,& forward_only)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp3ED))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3ED->tl))->hd;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp44B->tl))->hd,& forward_only)){(void*)(
e1->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp));(
void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)e2->r,0),
Cyc_Toc_curr_sp));(void*)(e->r=(void*)((void*)(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(
e),Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r));}else{if(
forward_only)({void*_tmp46E=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp46F="subtraction not allowed on forward-only ? pointer";
_tag_dynforward(_tmp46F,sizeof(char),_get_zero_arr_size_char(_tmp46F,50));}),
_tag_dynforward(_tmp46E,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp470[3];_tmp470[2]=Cyc_Absyn_prim1_exp((
void*)2,e2,0);_tmp470[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);
_tmp470[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp470,sizeof(struct Cyc_Absyn_Exp*),3));})));}}goto _LL23C;}
_LL243: if((int)_tmp44C != 5)goto _LL245;_LL244: goto _LL246;_LL245: if((int)_tmp44C != 
6)goto _LL247;_LL246: goto _LL248;_LL247: if((int)_tmp44C != 7)goto _LL249;_LL248: goto
_LL24A;_LL249: if((int)_tmp44C != 9)goto _LL24B;_LL24A: goto _LL24C;_LL24B: if((int)
_tmp44C != 8)goto _LL24D;_LL24C: goto _LL24E;_LL24D: if((int)_tmp44C != 10)goto _LL24F;
_LL24E: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3ED))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp3ED->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp44B))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp44B->tl))->hd;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ(t1,&
forward_only))(void*)(e1->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((
void*)e1->r,0),Cyc_Toc_curr_sp));if(Cyc_Tcutil_is_tagged_pointer_typ(t2,&
forward_only))(void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((
void*)e2->r,0),Cyc_Toc_curr_sp));goto _LL23C;}_LL24F:;_LL250: goto _LL23C;_LL23C:;}
goto _LL1E0;}_LL1EB: if(*((int*)_tmp3E7)!= 5)goto _LL1ED;_tmp3EE=((struct Cyc_Absyn_Increment_e_struct*)
_tmp3E7)->f1;_tmp3EF=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp3E7)->f2;
_LL1EC: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3EE->topt))->v;
void*ptr_type=(void*)0;void*elt_type=(void*)0;int is_dynforward=0;int is_dyneither=
0;struct _dynforward_ptr incr_str=({const char*_tmp485="increment";_tag_dynforward(
_tmp485,sizeof(char),_get_zero_arr_size_char(_tmp485,10));});if(_tmp3EF == (void*)
2  || _tmp3EF == (void*)3)incr_str=({const char*_tmp471="decrement";_tag_dynforward(
_tmp471,sizeof(char),_get_zero_arr_size_char(_tmp471,10));});if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp3EE,& ptr_type,& is_dynforward,& is_dyneither,& elt_type)){({struct Cyc_String_pa_struct
_tmp474;_tmp474.tag=0;_tmp474.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
incr_str);{void*_tmp472[1]={& _tmp474};Cyc_Tcutil_terr(e->loc,({const char*_tmp473="in-place %s is not supported when dereferencing a zero-terminated pointer";
_tag_dynforward(_tmp473,sizeof(char),_get_zero_arr_size_char(_tmp473,74));}),
_tag_dynforward(_tmp472,sizeof(void*),1));}});({void*_tmp475=0;((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp476="in-place inc on zero-term";_tag_dynforward(_tmp476,sizeof(char),
_get_zero_arr_size_char(_tmp476,26));}),_tag_dynforward(_tmp475,sizeof(void*),0));});}
Cyc_Toc_exp_to_c(nv,_tmp3EE);{void*elt_typ=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ,& forward_only)){struct Cyc_Absyn_Exp*fn_e;int change=1;if(
forward_only){if(_tmp3EF == (void*)3  || _tmp3EF == (void*)2)({void*_tmp477=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmp478="decrement not allowed on forward ? pointer";
_tag_dynforward(_tmp478,sizeof(char),_get_zero_arr_size_char(_tmp478,43));}),
_tag_dynforward(_tmp477,sizeof(void*),0));});fn_e=_tmp3EF == (void*)1?Cyc_Toc__dynforward_ptr_inplace_plus_post_e:
Cyc_Toc__dynforward_ptr_inplace_plus_e;}else{fn_e=(_tmp3EF == (void*)1  || _tmp3EF
== (void*)3)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp3EF == (void*)2  || _tmp3EF == (void*)3)change=- 1;}(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
fn_e,({struct Cyc_Absyn_Exp*_tmp479[3];_tmp479[2]=Cyc_Absyn_signed_int_exp(change,
0);_tmp479[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp479[0]=
Cyc_Absyn_address_exp(_tmp3EE,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp479,sizeof(struct Cyc_Absyn_Exp*),3));})));}
else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
fn_e;{void*_tmp47A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3EE->topt))->v);void*_tmp47B;int _tmp47C;_LL270: if(_tmp47A <= (void*)4)goto
_LL272;if(*((int*)_tmp47A)!= 5)goto _LL272;_tmp47B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp47A)->f2;_LL271:{void*_tmp47D=_tmp47B;_LL27B: if((int)_tmp47D != 0)goto _LL27D;
_LL27C: fn_e=_tmp3EF == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_char_e: Cyc_Toc__zero_arr_inplace_plus_char_e;
goto _LL27A;_LL27D: if((int)_tmp47D != 1)goto _LL27F;_LL27E: fn_e=_tmp3EF == (void*)1?
Cyc_Toc__zero_arr_inplace_plus_post_short_e: Cyc_Toc__zero_arr_inplace_plus_short_e;
goto _LL27A;_LL27F: if((int)_tmp47D != 2)goto _LL281;_LL280: fn_e=_tmp3EF == (void*)1?
Cyc_Toc__zero_arr_inplace_plus_post_int_e: Cyc_Toc__zero_arr_inplace_plus_int_e;
goto _LL27A;_LL281:;_LL282:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp47E=_cycalloc(sizeof(*_tmp47E));_tmp47E[0]=({struct Cyc_Core_Impossible_struct
_tmp47F;_tmp47F.tag=Cyc_Core_Impossible;_tmp47F.f1=({const char*_tmp480="impossible IntType (not char, short or int)";
_tag_dynforward(_tmp480,sizeof(char),_get_zero_arr_size_char(_tmp480,44));});
_tmp47F;});_tmp47E;}));_LL27A:;}goto _LL26F;_LL272: if((int)_tmp47A != 1)goto _LL274;
_LL273: fn_e=_tmp3EF == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_float_e: Cyc_Toc__zero_arr_inplace_plus_float_e;
goto _LL26F;_LL274: if(_tmp47A <= (void*)4)goto _LL278;if(*((int*)_tmp47A)!= 6)goto
_LL276;_tmp47C=((struct Cyc_Absyn_DoubleType_struct*)_tmp47A)->f1;_LL275: switch(
_tmp47C){case 1: _LL283: fn_e=_tmp3EF == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e:
Cyc_Toc__zero_arr_inplace_plus_longdouble_e;break;default: _LL284: fn_e=_tmp3EF == (
void*)1?Cyc_Toc__zero_arr_inplace_plus_post_double_e: Cyc_Toc__zero_arr_inplace_plus_double_e;}
goto _LL26F;_LL276: if(*((int*)_tmp47A)!= 4)goto _LL278;_LL277: fn_e=_tmp3EF == (void*)
1?Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e: Cyc_Toc__zero_arr_inplace_plus_voidstar_e;
goto _LL26F;_LL278:;_LL279:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481[0]=({struct Cyc_Core_Impossible_struct
_tmp482;_tmp482.tag=Cyc_Core_Impossible;_tmp482.f1=({const char*_tmp483="impossible expression type (not int, float, double, or pointer)";
_tag_dynforward(_tmp483,sizeof(char),_get_zero_arr_size_char(_tmp483,64));});
_tmp482;});_tmp481;}));_LL26F:;}(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(fn_e,({
struct Cyc_Absyn_Exp*_tmp484[2];_tmp484[1]=Cyc_Absyn_signed_int_exp(1,0);_tmp484[
0]=_tmp3EE;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp484,sizeof(struct Cyc_Absyn_Exp*),2));})));}else{if(elt_typ == (
void*)0  && !Cyc_Absyn_is_lvalue(_tmp3EE)){Cyc_Toc_lvalue_assign(_tmp3EE,0,Cyc_Toc_incr_lvalue,
_tmp3EF);(void*)(e->r=(void*)((void*)_tmp3EE->r));}}}goto _LL1E0;}}_LL1ED: if(*((
int*)_tmp3E7)!= 4)goto _LL1EF;_tmp3F0=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp3E7)->f1;_tmp3F1=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp3E7)->f2;_tmp3F2=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp3E7)->f3;_LL1EE: {void*ptr_type=(void*)0;
void*elt_type=(void*)0;int is_dynforward=0;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp3F0,& ptr_type,& is_dynforward,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(
nv,e,_tmp3F0,_tmp3F1,_tmp3F2,ptr_type,is_dynforward,is_dyneither,elt_type);
return;}{int e1_poly=Cyc_Toc_is_poly_project(_tmp3F0);void*e1_old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3F0->topt))->v;void*e2_old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3F2->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp3F0);
Cyc_Toc_exp_to_c(nv,_tmp3F2);{int done=0;if(_tmp3F1 != 0){void*elt_typ=(void*)0;
int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ,&
forward_only)){struct Cyc_Absyn_Exp*change;{void*_tmp486=(void*)_tmp3F1->v;_LL287:
if((int)_tmp486 != 0)goto _LL289;_LL288: change=_tmp3F2;goto _LL286;_LL289: if((int)
_tmp486 != 2)goto _LL28B;_LL28A: if(forward_only)({void*_tmp487=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp488="subtraction not allowed on forward ? pointers";
_tag_dynforward(_tmp488,sizeof(char),_get_zero_arr_size_char(_tmp488,46));}),
_tag_dynforward(_tmp487,sizeof(void*),0));});change=Cyc_Absyn_prim1_exp((void*)2,
_tmp3F2,0);goto _LL286;_LL28B:;_LL28C:({void*_tmp489=0;((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp48A="bad t ? pointer arithmetic";_tag_dynforward(_tmp48A,sizeof(char),
_get_zero_arr_size_char(_tmp48A,27));}),_tag_dynforward(_tmp489,sizeof(void*),0));});
_LL286:;}done=1;{struct Cyc_Absyn_Exp*_tmp48B=forward_only?Cyc_Toc__dynforward_ptr_inplace_plus_e:
Cyc_Toc__dyneither_ptr_inplace_plus_e;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
_tmp48B,({struct Cyc_Absyn_Exp*_tmp48C[3];_tmp48C[2]=change;_tmp48C[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp48C[0]=Cyc_Absyn_address_exp(_tmp3F0,0);((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp48C,sizeof(struct Cyc_Absyn_Exp*),3));})));}}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){void*_tmp48D=(void*)_tmp3F1->v;_LL28E: if((int)_tmp48D != 0)
goto _LL290;_LL28F: done=1;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunction(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp3F0),({struct Cyc_Absyn_Exp*
_tmp48E[2];_tmp48E[1]=_tmp3F2;_tmp48E[0]=_tmp3F0;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp48E,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL28D;_LL290:;_LL291:({void*_tmp48F=0;((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp490="bad zero-terminated pointer arithmetic";_tag_dynforward(_tmp490,sizeof(
char),_get_zero_arr_size_char(_tmp490,39));}),_tag_dynforward(_tmp48F,sizeof(
void*),0));});_LL28D:;}}}if(!done){if(e1_poly)(void*)(_tmp3F2->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp3F2->r,0)));if(!Cyc_Absyn_is_lvalue(
_tmp3F0)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,struct _tuple10*),struct _tuple10*f_env))Cyc_Toc_lvalue_assign)(
_tmp3F0,0,Cyc_Toc_assignop_lvalue,({struct _tuple10*_tmp491=_cycalloc(sizeof(
struct _tuple10)* 1);_tmp491[0]=({struct _tuple10 _tmp492;_tmp492.f1=_tmp3F1;
_tmp492.f2=_tmp3F2;_tmp492;});_tmp491;}));(void*)(e->r=(void*)((void*)_tmp3F0->r));}}
goto _LL1E0;}}}_LL1EF: if(*((int*)_tmp3E7)!= 6)goto _LL1F1;_tmp3F3=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp3E7)->f1;_tmp3F4=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3E7)->f2;
_tmp3F5=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3E7)->f3;_LL1F0: Cyc_Toc_exp_to_c(
nv,_tmp3F3);Cyc_Toc_exp_to_c(nv,_tmp3F4);Cyc_Toc_exp_to_c(nv,_tmp3F5);goto _LL1E0;
_LL1F1: if(*((int*)_tmp3E7)!= 7)goto _LL1F3;_tmp3F6=((struct Cyc_Absyn_And_e_struct*)
_tmp3E7)->f1;_tmp3F7=((struct Cyc_Absyn_And_e_struct*)_tmp3E7)->f2;_LL1F2: Cyc_Toc_exp_to_c(
nv,_tmp3F6);Cyc_Toc_exp_to_c(nv,_tmp3F7);goto _LL1E0;_LL1F3: if(*((int*)_tmp3E7)!= 
8)goto _LL1F5;_tmp3F8=((struct Cyc_Absyn_Or_e_struct*)_tmp3E7)->f1;_tmp3F9=((
struct Cyc_Absyn_Or_e_struct*)_tmp3E7)->f2;_LL1F4: Cyc_Toc_exp_to_c(nv,_tmp3F8);
Cyc_Toc_exp_to_c(nv,_tmp3F9);goto _LL1E0;_LL1F5: if(*((int*)_tmp3E7)!= 9)goto
_LL1F7;_tmp3FA=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp3E7)->f1;_tmp3FB=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp3E7)->f2;_LL1F6: Cyc_Toc_exp_to_c(nv,_tmp3FA);Cyc_Toc_exp_to_c(
nv,_tmp3FB);goto _LL1E0;_LL1F7: if(*((int*)_tmp3E7)!= 10)goto _LL1F9;_tmp3FC=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp3E7)->f1;_tmp3FD=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp3E7)->f2;_LL1F8: _tmp3FE=_tmp3FC;_tmp3FF=_tmp3FD;goto _LL1FA;_LL1F9: if(*((int*)
_tmp3E7)!= 11)goto _LL1FB;_tmp3FE=((struct Cyc_Absyn_FnCall_e_struct*)_tmp3E7)->f1;
_tmp3FF=((struct Cyc_Absyn_FnCall_e_struct*)_tmp3E7)->f2;_tmp400=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp3E7)->f3;if(_tmp400 != 0)goto _LL1FB;_LL1FA: Cyc_Toc_exp_to_c(nv,_tmp3FE);((
void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp3FF);goto _LL1E0;
_LL1FB: if(*((int*)_tmp3E7)!= 11)goto _LL1FD;_tmp401=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp3E7)->f1;_tmp402=((struct Cyc_Absyn_FnCall_e_struct*)_tmp3E7)->f2;_tmp403=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp3E7)->f3;if(_tmp403 == 0)goto _LL1FD;_tmp404=*
_tmp403;_tmp405=_tmp404.num_varargs;_tmp406=_tmp404.injectors;_tmp407=_tmp404.vai;
_LL1FC:{struct _RegionHandle _tmp493=_new_region("r");struct _RegionHandle*r=&
_tmp493;_push_region(r);{struct _tuple1*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp405,0);void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp407->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp402);int num_normargs=num_args - _tmp405;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp402=_tmp402->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp402))->hd);
new_args=({struct Cyc_List_List*_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494->hd=(
struct Cyc_Absyn_Exp*)_tmp402->hd;_tmp494->tl=new_args;_tmp494;});}}new_args=({
struct Cyc_List_List*_tmp495=_cycalloc(sizeof(*_tmp495));_tmp495->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*_tmp496[3];_tmp496[2]=
num_varargs_exp;_tmp496[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp496[0]=
argvexp;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp496,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp495->tl=
new_args;_tmp495;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp401);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp401,new_args,0),0);if(_tmp407->inject){
struct Cyc_Absyn_Tuniondecl*tud;{void*_tmp497=Cyc_Tcutil_compress((void*)_tmp407->type);
struct Cyc_Absyn_TunionInfo _tmp498;union Cyc_Absyn_TunionInfoU_union _tmp499;struct
Cyc_Absyn_Tuniondecl**_tmp49A;struct Cyc_Absyn_Tuniondecl*_tmp49B;_LL293: if(
_tmp497 <= (void*)4)goto _LL295;if(*((int*)_tmp497)!= 2)goto _LL295;_tmp498=((
struct Cyc_Absyn_TunionType_struct*)_tmp497)->f1;_tmp499=_tmp498.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp497)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL295;_tmp49A=(_tmp499.KnownTunion).f1;_tmp49B=*_tmp49A;_LL294: tud=
_tmp49B;goto _LL292;_LL295:;_LL296:({void*_tmp49C=0;((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp49D="toc: unknown tunion in vararg with inject";_tag_dynforward(_tmp49D,
sizeof(char),_get_zero_arr_size_char(_tmp49D,42));}),_tag_dynforward(_tmp49C,
sizeof(void*),0));});_LL292:;}{struct _dynforward_ptr vs=({unsigned int _tmp4A7=(
unsigned int)_tmp405;struct _tuple1**_tmp4A8=(struct _tuple1**)_region_malloc(r,
_check_times(sizeof(struct _tuple1*),_tmp4A7));struct _dynforward_ptr _tmp4AA=
_tag_dynforward(_tmp4A8,sizeof(struct _tuple1*),_tmp4A7);{unsigned int _tmp4A9=
_tmp4A7;unsigned int i;for(i=0;i < _tmp4A9;i ++){_tmp4A8[i]=Cyc_Toc_temp_var();}}
_tmp4AA;});if(_tmp405 != 0){struct Cyc_List_List*_tmp49E=0;{int i=_tmp405 - 1;for(0;
i >= 0;-- i){_tmp49E=({struct Cyc_List_List*_tmp49F=_cycalloc(sizeof(*_tmp49F));
_tmp49F->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct
_tuple1**)_check_dynforward_subscript(vs,sizeof(struct _tuple1*),i)),0),0));
_tmp49F->tl=_tmp49E;_tmp49F;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct
Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp49E,0),s,0);{int i=0;for(0;
_tmp402 != 0;(((_tmp402=_tmp402->tl,_tmp406=_tmp406->tl)),++ i)){struct Cyc_Absyn_Exp*
arg=(struct Cyc_Absyn_Exp*)_tmp402->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple1*var=*((struct _tuple1**)
_check_dynforward_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield _tmp4A1;struct _tuple1*
_tmp4A2;struct Cyc_List_List*_tmp4A3;struct Cyc_Absyn_Tunionfield*_tmp4A0=(struct
Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp406))->hd;_tmp4A1=*
_tmp4A0;_tmp4A2=_tmp4A1.name;_tmp4A3=_tmp4A1.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp4A3))->hd)).f2);Cyc_Toc_exp_to_c(
nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(
1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp4A2,1),0),s,0);s=Cyc_Absyn_declare_stmt(
var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp4A2,({const char*_tmp4A4="_struct";
_tag_dynforward(_tmp4A4,sizeof(char),_get_zero_arr_size_char(_tmp4A4,8));}))),0,
s,0);}}}}else{struct Cyc_List_List*_tmp4A5=({struct _tuple8*_tmp4A6[3];_tmp4A6[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));_tmp4A6[1]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(
0,0));_tmp4A6[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4A6,sizeof(struct
_tuple8*),3));});s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(struct
Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}}}else{{int i=0;for(0;_tmp402 != 0;(
_tmp402=_tmp402->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp402->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((
unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp402->hd,0),s,0);}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,0,s,0);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(s));}};_pop_region(
r);}goto _LL1E0;_LL1FD: if(*((int*)_tmp3E7)!= 12)goto _LL1FF;_tmp408=((struct Cyc_Absyn_Throw_e_struct*)
_tmp3E7)->f1;_LL1FE: Cyc_Toc_exp_to_c(nv,_tmp408);(void*)(e->r=(void*)((void*)(
Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp408),
0))->r));goto _LL1E0;_LL1FF: if(*((int*)_tmp3E7)!= 13)goto _LL201;_tmp409=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp3E7)->f1;_LL200: Cyc_Toc_exp_to_c(nv,
_tmp409);goto _LL1E0;_LL201: if(*((int*)_tmp3E7)!= 14)goto _LL203;_tmp40A=((struct
Cyc_Absyn_Instantiate_e_struct*)_tmp3E7)->f1;_tmp40B=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp3E7)->f2;_LL202: Cyc_Toc_exp_to_c(nv,_tmp40A);for(0;_tmp40B != 0;_tmp40B=
_tmp40B->tl){void*k=Cyc_Tcutil_typ_kind((void*)_tmp40B->hd);if(((k != (void*)6
 && k != (void*)3) && k != (void*)4) && k != (void*)5){{void*_tmp4AB=Cyc_Tcutil_compress((
void*)_tmp40B->hd);_LL298: if(_tmp4AB <= (void*)4)goto _LL29C;if(*((int*)_tmp4AB)!= 
1)goto _LL29A;_LL299: goto _LL29B;_LL29A: if(*((int*)_tmp4AB)!= 2)goto _LL29C;_LL29B:
continue;_LL29C:;_LL29D:(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp40A,
0))->r));goto _LL297;_LL297:;}break;}}goto _LL1E0;_LL203: if(*((int*)_tmp3E7)!= 15)
goto _LL205;_tmp40C=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp3E7)->f1;_tmp40D=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp3E7)->f1);_tmp40E=((struct
Cyc_Absyn_Cast_e_struct*)_tmp3E7)->f2;_tmp40F=((struct Cyc_Absyn_Cast_e_struct*)
_tmp3E7)->f3;_tmp410=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp3E7)->f4;_LL204: {
void*old_t2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp40E->topt))->v;void*
new_typ=*_tmp40D;*_tmp40D=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp40E);{
struct _tuple0 _tmp4AD=({struct _tuple0 _tmp4AC;_tmp4AC.f1=Cyc_Tcutil_compress(
old_t2);_tmp4AC.f2=Cyc_Tcutil_compress(new_typ);_tmp4AC;});void*_tmp4AE;struct
Cyc_Absyn_PtrInfo _tmp4AF;void*_tmp4B0;struct Cyc_Absyn_PtrInfo _tmp4B1;void*
_tmp4B2;struct Cyc_Absyn_PtrInfo _tmp4B3;void*_tmp4B4;_LL29F: _tmp4AE=_tmp4AD.f1;
if(_tmp4AE <= (void*)4)goto _LL2A1;if(*((int*)_tmp4AE)!= 4)goto _LL2A1;_tmp4AF=((
struct Cyc_Absyn_PointerType_struct*)_tmp4AE)->f1;_tmp4B0=_tmp4AD.f2;if(_tmp4B0 <= (
void*)4)goto _LL2A1;if(*((int*)_tmp4B0)!= 4)goto _LL2A1;_tmp4B1=((struct Cyc_Absyn_PointerType_struct*)
_tmp4B0)->f1;_LL2A0: {int _tmp4B5=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp4AF.ptr_atts).nullable);int _tmp4B6=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,(_tmp4B1.ptr_atts).nullable);void*_tmp4B7=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp4AF.ptr_atts).bounds);void*_tmp4B8=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp4B1.ptr_atts).bounds);int _tmp4B9=((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp4AF.ptr_atts).zero_term);int
_tmp4BA=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp4B1.ptr_atts).zero_term);
int p1_forward_only=0;int p2_forward_only=0;{struct _tuple0 _tmp4BC=({struct _tuple0
_tmp4BB;_tmp4BB.f1=_tmp4B7;_tmp4BB.f2=_tmp4B8;_tmp4BB;});void*_tmp4BD;struct Cyc_Absyn_Exp*
_tmp4BE;void*_tmp4BF;struct Cyc_Absyn_Exp*_tmp4C0;void*_tmp4C1;struct Cyc_Absyn_Exp*
_tmp4C2;void*_tmp4C3;void*_tmp4C4;struct Cyc_Absyn_Exp*_tmp4C5;void*_tmp4C6;void*
_tmp4C7;void*_tmp4C8;struct Cyc_Absyn_Exp*_tmp4C9;void*_tmp4CA;void*_tmp4CB;
struct Cyc_Absyn_Exp*_tmp4CC;void*_tmp4CD;void*_tmp4CE;void*_tmp4CF;void*_tmp4D0;
void*_tmp4D1;void*_tmp4D2;void*_tmp4D3;void*_tmp4D4;void*_tmp4D5;void*_tmp4D6;
void*_tmp4D7;void*_tmp4D8;void*_tmp4D9;void*_tmp4DA;_LL2A6: _tmp4BD=_tmp4BC.f1;
if(_tmp4BD <= (void*)2)goto _LL2A8;if(*((int*)_tmp4BD)!= 0)goto _LL2A8;_tmp4BE=((
struct Cyc_Absyn_Upper_b_struct*)_tmp4BD)->f1;_tmp4BF=_tmp4BC.f2;if(_tmp4BF <= (
void*)2)goto _LL2A8;if(*((int*)_tmp4BF)!= 0)goto _LL2A8;_tmp4C0=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4BF)->f1;_LL2A7: if(_tmp4B5  && !_tmp4B6){if(Cyc_Toc_is_toplevel(nv))({void*
_tmp4DB=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp4DC="can't do NULL-check conversion at top-level";_tag_dynforward(
_tmp4DC,sizeof(char),_get_zero_arr_size_char(_tmp4DC,44));}),_tag_dynforward(
_tmp4DB,sizeof(void*),0));});if(_tmp410 != (void*)2)({struct Cyc_String_pa_struct
_tmp4DF;_tmp4DF.tag=0;_tmp4DF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp4DD[1]={& _tmp4DF};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp4DE="null-check conversion mis-classified: %s";_tag_dynforward(_tmp4DE,
sizeof(char),_get_zero_arr_size_char(_tmp4DE,41));}),_tag_dynforward(_tmp4DD,
sizeof(void*),1));}});{int do_null_check=Cyc_Toc_need_null_check(_tmp40E);if(
do_null_check){if(!_tmp40F)({void*_tmp4E0=0;Cyc_Tcutil_warn(e->loc,({const char*
_tmp4E1="inserted null check due to implicit cast from * to @ type";
_tag_dynforward(_tmp4E1,sizeof(char),_get_zero_arr_size_char(_tmp4E1,58));}),
_tag_dynforward(_tmp4E0,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp40D,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp4E2=
_cycalloc(sizeof(*_tmp4E2));_tmp4E2->hd=_tmp40E;_tmp4E2->tl=0;_tmp4E2;}),0)));}}}
goto _LL2A5;_LL2A8: _tmp4C1=_tmp4BC.f1;if(_tmp4C1 <= (void*)2)goto _LL2AA;if(*((int*)
_tmp4C1)!= 0)goto _LL2AA;_tmp4C2=((struct Cyc_Absyn_Upper_b_struct*)_tmp4C1)->f1;
_tmp4C3=_tmp4BC.f2;if((int)_tmp4C3 != 0)goto _LL2AA;_LL2A9: p2_forward_only=1;
_tmp4C5=_tmp4C2;goto _LL2AB;_LL2AA: _tmp4C4=_tmp4BC.f1;if(_tmp4C4 <= (void*)2)goto
_LL2AC;if(*((int*)_tmp4C4)!= 0)goto _LL2AC;_tmp4C5=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4C4)->f1;_tmp4C6=_tmp4BC.f2;if((int)_tmp4C6 != 1)goto _LL2AC;_LL2AB: if(_tmp410
== (void*)2)({struct Cyc_String_pa_struct _tmp4E5;_tmp4E5.tag=0;_tmp4E5.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{void*_tmp4E3[
1]={& _tmp4E5};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp4E4="conversion mis-classified as null-check: %s";_tag_dynforward(
_tmp4E4,sizeof(char),_get_zero_arr_size_char(_tmp4E4,44));}),_tag_dynforward(
_tmp4E3,sizeof(void*),1));}});if(Cyc_Toc_is_toplevel(nv)){if((_tmp4B9  && !(
_tmp4B1.elt_tq).real_const) && !_tmp4BA)_tmp4C5=Cyc_Absyn_prim2_exp((void*)2,
_tmp4C5,Cyc_Absyn_uint_exp(1,0),0);(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(
old_t2,_tmp4C5,_tmp40E,p2_forward_only))->r));}else{struct Cyc_Absyn_Exp*_tmp4E6=
p2_forward_only?Cyc_Toc__tag_dynforward_e: Cyc_Toc__tag_dyneither_e;if(_tmp4B9){
struct _tuple1*_tmp4E7=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp4E8=Cyc_Absyn_var_exp(
_tmp4E7,0);struct Cyc_Absyn_Exp*_tmp4E9=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp40E),({struct Cyc_Absyn_Exp*_tmp4EE[2];
_tmp4EE[1]=_tmp4C5;_tmp4EE[0]=_tmp4E8;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4EE,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);if(!_tmp4BA  && !(_tmp4B1.elt_tq).real_const)_tmp4E9=Cyc_Absyn_prim2_exp((
void*)2,_tmp4E9,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp4EA=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp4B1.elt_typ),0);struct Cyc_Absyn_Exp*_tmp4EB=Cyc_Absyn_fncall_exp(
_tmp4E6,({struct Cyc_Absyn_Exp*_tmp4ED[3];_tmp4ED[2]=_tmp4E9;_tmp4ED[1]=_tmp4EA;
_tmp4ED[0]=_tmp4E8;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp4ED,sizeof(struct Cyc_Absyn_Exp*),3));}),0);struct Cyc_Absyn_Stmt*
_tmp4EC=Cyc_Absyn_exp_stmt(_tmp4EB,0);_tmp4EC=Cyc_Absyn_declare_stmt(_tmp4E7,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp40E,_tmp4EC,0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
_tmp4EC));}}else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp4E6,({struct Cyc_Absyn_Exp*
_tmp4EF[3];_tmp4EF[2]=_tmp4C5;_tmp4EF[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp4B1.elt_typ),0);_tmp4EF[0]=_tmp40E;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4EF,sizeof(struct Cyc_Absyn_Exp*),
3));})));}}goto _LL2A5;_LL2AC: _tmp4C7=_tmp4BC.f1;if((int)_tmp4C7 != 0)goto _LL2AE;
_tmp4C8=_tmp4BC.f2;if(_tmp4C8 <= (void*)2)goto _LL2AE;if(*((int*)_tmp4C8)!= 0)goto
_LL2AE;_tmp4C9=((struct Cyc_Absyn_Upper_b_struct*)_tmp4C8)->f1;_LL2AD:
p1_forward_only=1;_tmp4CC=_tmp4C9;goto _LL2AF;_LL2AE: _tmp4CA=_tmp4BC.f1;if((int)
_tmp4CA != 1)goto _LL2B0;_tmp4CB=_tmp4BC.f2;if(_tmp4CB <= (void*)2)goto _LL2B0;if(*((
int*)_tmp4CB)!= 0)goto _LL2B0;_tmp4CC=((struct Cyc_Absyn_Upper_b_struct*)_tmp4CB)->f1;
_LL2AF: if(Cyc_Toc_is_toplevel(nv))({void*_tmp4F0=0;((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp4F1="can't coerce t? to t* or t@ at the top-level";
_tag_dynforward(_tmp4F1,sizeof(char),_get_zero_arr_size_char(_tmp4F1,45));}),
_tag_dynforward(_tmp4F0,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp4F2=
_tmp4CC;if(_tmp4B9  && !_tmp4BA)_tmp4F2=Cyc_Absyn_add_exp(_tmp4CC,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Exp*_tmp4F3=p1_forward_only?Cyc_Toc__untag_dynforward_ptr_e:
Cyc_Toc__untag_dyneither_ptr_e;struct Cyc_Absyn_Exp*_tmp4F4=Cyc_Absyn_fncall_exp(
_tmp4F3,({struct Cyc_Absyn_Exp*_tmp4F6[3];_tmp4F6[2]=_tmp4F2;_tmp4F6[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp4AF.elt_typ),0);_tmp4F6[0]=_tmp40E;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4F6,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);if(_tmp4B6)(void*)(_tmp4F4->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));_tmp4F5->hd=Cyc_Absyn_copy_exp(
_tmp4F4);_tmp4F5->tl=0;_tmp4F5;})));(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp40D,_tmp4F4));goto _LL2A5;}}_LL2B0: _tmp4CD=_tmp4BC.f1;if((int)_tmp4CD != 0)
goto _LL2B2;_tmp4CE=_tmp4BC.f2;if((int)_tmp4CE != 0)goto _LL2B2;_LL2B1:
p1_forward_only=1;p2_forward_only=1;goto DynCast;_LL2B2: _tmp4CF=_tmp4BC.f1;if((
int)_tmp4CF != 0)goto _LL2B4;_tmp4D0=_tmp4BC.f2;if((int)_tmp4D0 != 1)goto _LL2B4;
_LL2B3: p1_forward_only=1;goto DynCast;_LL2B4: _tmp4D1=_tmp4BC.f1;if((int)_tmp4D1 != 
1)goto _LL2B6;_tmp4D2=_tmp4BC.f2;if((int)_tmp4D2 != 0)goto _LL2B6;_LL2B5:
p2_forward_only=1;goto DynCast;_LL2B6: _tmp4D3=_tmp4BC.f1;if((int)_tmp4D3 != 1)goto
_LL2B8;_tmp4D4=_tmp4BC.f2;if((int)_tmp4D4 != 1)goto _LL2B8;_LL2B7: DynCast: if((
_tmp4B9  && !_tmp4BA) && !(_tmp4B1.elt_tq).real_const){if(Cyc_Toc_is_toplevel(nv))({
void*_tmp4F7=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp4F8="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";
_tag_dynforward(_tmp4F8,sizeof(char),_get_zero_arr_size_char(_tmp4F8,70));}),
_tag_dynforward(_tmp4F7,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp4F9=
p1_forward_only?Cyc_Toc__dynforward_ptr_decrease_size_e: Cyc_Toc__dyneither_ptr_decrease_size_e;(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp4F9,({struct Cyc_Absyn_Exp*_tmp4FA[3];
_tmp4FA[2]=Cyc_Absyn_uint_exp(1,0);_tmp4FA[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp4AF.elt_typ),0);_tmp4FA[0]=_tmp40E;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4FA,sizeof(struct Cyc_Absyn_Exp*),
3));})));if(p1_forward_only != p2_forward_only){if(p1_forward_only)(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_to_dyneither_e,({struct Cyc_Absyn_Exp*
_tmp4FB[1];_tmp4FB[0]=Cyc_Absyn_copy_exp(e);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4FB,sizeof(struct Cyc_Absyn_Exp*),
1));})));else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_to_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp4FC[1];_tmp4FC[0]=Cyc_Absyn_copy_exp(e);((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4FC,sizeof(struct Cyc_Absyn_Exp*),
1));})));}}}}else{if(p1_forward_only != p2_forward_only){if(Cyc_Toc_is_toplevel(
nv))({void*_tmp4FD=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp4FE="can't coerce between ? and ?+- at toplevel";
_tag_dynforward(_tmp4FE,sizeof(char),_get_zero_arr_size_char(_tmp4FE,43));}),
_tag_dynforward(_tmp4FD,sizeof(void*),0));});if(p1_forward_only)(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_to_dyneither_e,({struct Cyc_Absyn_Exp*
_tmp4FF[1];_tmp4FF[0]=_tmp40E;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp4FF,sizeof(struct Cyc_Absyn_Exp*),1));})));
else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_to_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp500[1];_tmp500[0]=_tmp40E;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp500,sizeof(struct Cyc_Absyn_Exp*),
1));})));}}}goto _LL2A5;_LL2B8: _tmp4D5=_tmp4BC.f1;if(_tmp4D5 <= (void*)2)goto
_LL2BA;if(*((int*)_tmp4D5)!= 1)goto _LL2BA;_tmp4D6=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp4D5)->f1;_tmp4D7=_tmp4BC.f2;if(_tmp4D7 <= (void*)2)goto _LL2BA;if(*((int*)
_tmp4D7)!= 1)goto _LL2BA;_tmp4D8=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp4D7)->f1;_LL2B9: if(_tmp4B5  && !_tmp4B6){if(Cyc_Toc_is_toplevel(nv))({void*
_tmp501=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp502="can't do NULL-check conversion at top-level";_tag_dynforward(
_tmp502,sizeof(char),_get_zero_arr_size_char(_tmp502,44));}),_tag_dynforward(
_tmp501,sizeof(void*),0));});if(_tmp410 != (void*)2)({struct Cyc_String_pa_struct
_tmp505;_tmp505.tag=0;_tmp505.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp503[1]={& _tmp505};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp504="null-check conversion mis-classified: %s";_tag_dynforward(_tmp504,
sizeof(char),_get_zero_arr_size_char(_tmp504,41));}),_tag_dynforward(_tmp503,
sizeof(void*),1));}});{int do_null_check=Cyc_Toc_need_null_check(_tmp40E);if(
do_null_check){if(!_tmp40F)({void*_tmp506=0;Cyc_Tcutil_warn(e->loc,({const char*
_tmp507="inserted null check due to implicit cast from * to @ type";
_tag_dynforward(_tmp507,sizeof(char),_get_zero_arr_size_char(_tmp507,58));}),
_tag_dynforward(_tmp506,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp40D,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp508=
_cycalloc(sizeof(*_tmp508));_tmp508->hd=_tmp40E;_tmp508->tl=0;_tmp508;}),0)));}}}
goto _LL2A5;_LL2BA: _tmp4D9=_tmp4BC.f1;if(_tmp4D9 <= (void*)2)goto _LL2BC;if(*((int*)
_tmp4D9)!= 1)goto _LL2BC;_LL2BB:({struct Cyc_String_pa_struct _tmp50B;_tmp50B.tag=0;
_tmp50B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp40E->loc));{void*_tmp509[1]={& _tmp50B};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp50A="%s: toc, cast from AbsUpper_b";
_tag_dynforward(_tmp50A,sizeof(char),_get_zero_arr_size_char(_tmp50A,30));}),
_tag_dynforward(_tmp509,sizeof(void*),1));}});_LL2BC: _tmp4DA=_tmp4BC.f2;if(
_tmp4DA <= (void*)2)goto _LL2A5;if(*((int*)_tmp4DA)!= 1)goto _LL2A5;_LL2BD: goto
_LL2A5;_LL2A5:;}goto _LL29E;}_LL2A1: _tmp4B2=_tmp4AD.f1;if(_tmp4B2 <= (void*)4)goto
_LL2A3;if(*((int*)_tmp4B2)!= 4)goto _LL2A3;_tmp4B3=((struct Cyc_Absyn_PointerType_struct*)
_tmp4B2)->f1;_tmp4B4=_tmp4AD.f2;if(_tmp4B4 <= (void*)4)goto _LL2A3;if(*((int*)
_tmp4B4)!= 5)goto _LL2A3;_LL2A2:{void*_tmp50C=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(
_tmp4B3.ptr_atts).bounds);_LL2BF: if((int)_tmp50C != 0)goto _LL2C1;_LL2C0: goto
_LL2C2;_LL2C1: if((int)_tmp50C != 1)goto _LL2C3;_LL2C2:(void*)(_tmp40E->r=(void*)
Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)_tmp40E->r,_tmp40E->loc),Cyc_Toc_curr_sp));
goto _LL2BE;_LL2C3:;_LL2C4: goto _LL2BE;_LL2BE:;}goto _LL29E;_LL2A3:;_LL2A4: goto
_LL29E;_LL29E:;}goto _LL1E0;}_LL205: if(*((int*)_tmp3E7)!= 16)goto _LL207;_tmp411=((
struct Cyc_Absyn_Address_e_struct*)_tmp3E7)->f1;_LL206:{void*_tmp50D=(void*)
_tmp411->r;struct _tuple1*_tmp50E;struct Cyc_List_List*_tmp50F;struct Cyc_List_List*
_tmp510;struct Cyc_List_List*_tmp511;_LL2C6: if(*((int*)_tmp50D)!= 30)goto _LL2C8;
_tmp50E=((struct Cyc_Absyn_Struct_e_struct*)_tmp50D)->f1;_tmp50F=((struct Cyc_Absyn_Struct_e_struct*)
_tmp50D)->f2;_tmp510=((struct Cyc_Absyn_Struct_e_struct*)_tmp50D)->f3;_LL2C7: if(
Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct _tmp514;_tmp514.tag=0;
_tmp514.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp411->loc));{void*_tmp512[1]={& _tmp514};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp513="%s: & on non-identifiers at the top-level";
_tag_dynforward(_tmp513,sizeof(char),_get_zero_arr_size_char(_tmp513,42));}),
_tag_dynforward(_tmp512,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp411->topt))->v,_tmp50F != 0,1,0,
_tmp510,_tmp50E))->r));goto _LL2C5;_LL2C8: if(*((int*)_tmp50D)!= 26)goto _LL2CA;
_tmp511=((struct Cyc_Absyn_Tuple_e_struct*)_tmp50D)->f1;_LL2C9: if(Cyc_Toc_is_toplevel(
nv))({struct Cyc_String_pa_struct _tmp517;_tmp517.tag=0;_tmp517.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(_tmp411->loc));{
void*_tmp515[1]={& _tmp517};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp516="%s: & on non-identifiers at the top-level";
_tag_dynforward(_tmp516,sizeof(char),_get_zero_arr_size_char(_tmp516,42));}),
_tag_dynforward(_tmp515,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,1,0,_tmp511))->r));goto _LL2C5;_LL2CA:;_LL2CB: Cyc_Toc_exp_to_c(nv,_tmp411);if(
!Cyc_Absyn_is_lvalue(_tmp411)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp411,0,Cyc_Toc_address_lvalue,1);(void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp411));}goto _LL2C5;
_LL2C5:;}goto _LL1E0;_LL207: if(*((int*)_tmp3E7)!= 17)goto _LL209;_tmp412=((struct
Cyc_Absyn_New_e_struct*)_tmp3E7)->f1;_tmp413=((struct Cyc_Absyn_New_e_struct*)
_tmp3E7)->f2;_LL208: if(Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct
_tmp51A;_tmp51A.tag=0;_tmp51A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Position_string_of_segment(_tmp413->loc));{void*_tmp518[1]={& _tmp51A};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp519="%s: new at top-level";_tag_dynforward(_tmp519,sizeof(char),
_get_zero_arr_size_char(_tmp519,21));}),_tag_dynforward(_tmp518,sizeof(void*),1));}});{
void*_tmp51B=(void*)_tmp413->r;struct Cyc_List_List*_tmp51C;struct Cyc_Absyn_Vardecl*
_tmp51D;struct Cyc_Absyn_Exp*_tmp51E;struct Cyc_Absyn_Exp*_tmp51F;int _tmp520;
struct _tuple1*_tmp521;struct Cyc_List_List*_tmp522;struct Cyc_List_List*_tmp523;
struct Cyc_List_List*_tmp524;_LL2CD: if(*((int*)_tmp51B)!= 28)goto _LL2CF;_tmp51C=((
struct Cyc_Absyn_Array_e_struct*)_tmp51B)->f1;_LL2CE: {struct _tuple1*_tmp525=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp526=Cyc_Absyn_var_exp(_tmp525,0);struct Cyc_Absyn_Stmt*
_tmp527=Cyc_Toc_init_array(nv,_tmp526,_tmp51C,Cyc_Absyn_exp_stmt(_tmp526,0));
void*old_elt_typ;{void*_tmp528=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp529;void*_tmp52A;struct Cyc_Absyn_Tqual _tmp52B;struct Cyc_Absyn_PtrAtts _tmp52C;
struct Cyc_Absyn_Conref*_tmp52D;_LL2D8: if(_tmp528 <= (void*)4)goto _LL2DA;if(*((int*)
_tmp528)!= 4)goto _LL2DA;_tmp529=((struct Cyc_Absyn_PointerType_struct*)_tmp528)->f1;
_tmp52A=(void*)_tmp529.elt_typ;_tmp52B=_tmp529.elt_tq;_tmp52C=_tmp529.ptr_atts;
_tmp52D=_tmp52C.zero_term;_LL2D9: old_elt_typ=_tmp52A;goto _LL2D7;_LL2DA:;_LL2DB:
old_elt_typ=({void*_tmp52E=0;Cyc_Toc_toc_impos(({const char*_tmp52F="exp_to_c:new array expression doesn't have ptr type";
_tag_dynforward(_tmp52F,sizeof(char),_get_zero_arr_size_char(_tmp52F,52));}),
_tag_dynforward(_tmp52E,sizeof(void*),0));});_LL2D7:;}{void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);void*_tmp530=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp531=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),Cyc_Absyn_signed_int_exp(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp51C),0),0);struct Cyc_Absyn_Exp*
e1;if(_tmp412 == 0  || Cyc_Absyn_no_regions)e1=Cyc_Toc_malloc_exp(old_elt_typ,
_tmp531);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp412;Cyc_Toc_exp_to_c(
nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp531);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp525,_tmp530,(struct Cyc_Absyn_Exp*)e1,_tmp527,0)));
goto _LL2CC;}}_LL2CF: if(*((int*)_tmp51B)!= 29)goto _LL2D1;_tmp51D=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp51B)->f1;_tmp51E=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp51B)->f2;
_tmp51F=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp51B)->f3;_tmp520=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp51B)->f4;_LL2D0: {int is_dynforward_ptr=0;
int is_dyneither_ptr=0;{void*_tmp532=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp533;void*_tmp534;struct Cyc_Absyn_Tqual _tmp535;struct Cyc_Absyn_PtrAtts _tmp536;
struct Cyc_Absyn_Conref*_tmp537;struct Cyc_Absyn_Conref*_tmp538;_LL2DD: if(_tmp532
<= (void*)4)goto _LL2DF;if(*((int*)_tmp532)!= 4)goto _LL2DF;_tmp533=((struct Cyc_Absyn_PointerType_struct*)
_tmp532)->f1;_tmp534=(void*)_tmp533.elt_typ;_tmp535=_tmp533.elt_tq;_tmp536=
_tmp533.ptr_atts;_tmp537=_tmp536.bounds;_tmp538=_tmp536.zero_term;_LL2DE:
is_dynforward_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp537)== (void*)0;
is_dyneither_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp537)== (void*)1;
goto _LL2DC;_LL2DF:;_LL2E0:({void*_tmp539=0;((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp53A="exp_to_c: comprehension not an array type";
_tag_dynforward(_tmp53A,sizeof(char),_get_zero_arr_size_char(_tmp53A,42));}),
_tag_dynforward(_tmp539,sizeof(void*),0));});_LL2DC:;}{struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp51F->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*
ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp51E);{
struct Cyc_Absyn_Exp*_tmp53B=Cyc_Absyn_var_exp(max,0);if(_tmp520)_tmp53B=Cyc_Absyn_add_exp(
_tmp53B,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(
nv,Cyc_Absyn_var_exp(a,0),_tmp51D,Cyc_Absyn_var_exp(max,0),_tmp51F,_tmp520,Cyc_Toc_skip_stmt_dl(),
1);{struct _RegionHandle _tmp53C=_new_region("r");struct _RegionHandle*r=& _tmp53C;
_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp54D=
_region_malloc(r,sizeof(*_tmp54D));_tmp54D->hd=({struct _tuple11*_tmp54E=
_region_malloc(r,sizeof(*_tmp54E));_tmp54E->f1=max;_tmp54E->f2=Cyc_Absyn_uint_typ;
_tmp54E->f3=(struct Cyc_Absyn_Exp*)_tmp51E;_tmp54E;});_tmp54D->tl=0;_tmp54D;});
struct Cyc_Absyn_Exp*ai;if(_tmp412 == 0  || Cyc_Absyn_no_regions)ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*
_tmp53D[2];_tmp53D[1]=_tmp53B;_tmp53D[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp53D,sizeof(struct Cyc_Absyn_Exp*),2));}),0));else{struct Cyc_Absyn_Exp*r=(
struct Cyc_Absyn_Exp*)_tmp412;Cyc_Toc_exp_to_c(nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp53E[2];_tmp53E[1]=_tmp53B;
_tmp53E[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp53E,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));}{struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);decls=({
struct Cyc_List_List*_tmp53F=_region_malloc(r,sizeof(*_tmp53F));_tmp53F->hd=({
struct _tuple11*_tmp540=_region_malloc(r,sizeof(*_tmp540));_tmp540->f1=a;_tmp540->f2=
ptr_typ;_tmp540->f3=(struct Cyc_Absyn_Exp*)ainit;_tmp540;});_tmp53F->tl=decls;
_tmp53F;});if(is_dynforward_ptr  || is_dyneither_ptr){struct _tuple1*_tmp541=Cyc_Toc_temp_var();
void*_tmp542=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp543=
is_dynforward_ptr?Cyc_Toc__tag_dynforward_e: Cyc_Toc__tag_dyneither_e;struct Cyc_Absyn_Exp*
_tmp544=Cyc_Absyn_fncall_exp(_tmp543,({struct Cyc_Absyn_Exp*_tmp547[3];_tmp547[2]=
_tmp53B;_tmp547[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp547[0]=Cyc_Absyn_var_exp(
a,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp547,sizeof(struct Cyc_Absyn_Exp*),3));}),0);decls=({struct Cyc_List_List*
_tmp545=_region_malloc(r,sizeof(*_tmp545));_tmp545->hd=({struct _tuple11*_tmp546=
_region_malloc(r,sizeof(*_tmp546));_tmp546->f1=_tmp541;_tmp546->f2=_tmp542;
_tmp546->f3=(struct Cyc_Absyn_Exp*)_tmp544;_tmp546;});_tmp545->tl=decls;_tmp545;});
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp541,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp548=decls;for(0;_tmp548 != 0;_tmp548=_tmp548->tl){struct
_tuple1*_tmp54A;void*_tmp54B;struct Cyc_Absyn_Exp*_tmp54C;struct _tuple11 _tmp549=*((
struct _tuple11*)_tmp548->hd);_tmp54A=_tmp549.f1;_tmp54B=_tmp549.f2;_tmp54C=
_tmp549.f3;s=Cyc_Absyn_declare_stmt(_tmp54A,_tmp54B,_tmp54C,s,0);}}(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(s));}};_pop_region(r);}goto _LL2CC;}}}}_LL2D1: if(*((int*)
_tmp51B)!= 30)goto _LL2D3;_tmp521=((struct Cyc_Absyn_Struct_e_struct*)_tmp51B)->f1;
_tmp522=((struct Cyc_Absyn_Struct_e_struct*)_tmp51B)->f2;_tmp523=((struct Cyc_Absyn_Struct_e_struct*)
_tmp51B)->f3;_LL2D2:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(nv,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp413->topt))->v,_tmp522 != 0,1,_tmp412,_tmp523,
_tmp521))->r));goto _LL2CC;_LL2D3: if(*((int*)_tmp51B)!= 26)goto _LL2D5;_tmp524=((
struct Cyc_Absyn_Tuple_e_struct*)_tmp51B)->f1;_LL2D4:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_tuple(nv,1,_tmp412,_tmp524))->r));goto _LL2CC;_LL2D5:;_LL2D6: {void*
old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp413->topt))->v;void*
elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct _tuple1*_tmp54F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp550=Cyc_Absyn_var_exp(_tmp54F,0);struct Cyc_Absyn_Exp*
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp550,0),0);struct Cyc_Absyn_Exp*
inner_mexp=mexp;if(_tmp412 == 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(
old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp412;Cyc_Toc_exp_to_c(
nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;int forward_only=0;{void*
_tmp551=(void*)_tmp413->r;void*_tmp552;struct Cyc_Absyn_Exp*_tmp553;_LL2E2: if(*((
int*)_tmp551)!= 15)goto _LL2E4;_tmp552=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp551)->f1;_tmp553=((struct Cyc_Absyn_Cast_e_struct*)_tmp551)->f2;_LL2E3:{
struct _tuple0 _tmp555=({struct _tuple0 _tmp554;_tmp554.f1=Cyc_Tcutil_compress(
_tmp552);_tmp554.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp553->topt))->v);_tmp554;});void*_tmp556;struct Cyc_Absyn_PtrInfo _tmp557;void*
_tmp558;struct Cyc_Absyn_PtrAtts _tmp559;struct Cyc_Absyn_Conref*_tmp55A;void*
_tmp55B;struct Cyc_Absyn_PtrInfo _tmp55C;struct Cyc_Absyn_PtrAtts _tmp55D;struct Cyc_Absyn_Conref*
_tmp55E;_LL2E7: _tmp556=_tmp555.f1;if(_tmp556 <= (void*)4)goto _LL2E9;if(*((int*)
_tmp556)!= 4)goto _LL2E9;_tmp557=((struct Cyc_Absyn_PointerType_struct*)_tmp556)->f1;
_tmp558=(void*)_tmp557.elt_typ;_tmp559=_tmp557.ptr_atts;_tmp55A=_tmp559.bounds;
_tmp55B=_tmp555.f2;if(_tmp55B <= (void*)4)goto _LL2E9;if(*((int*)_tmp55B)!= 4)goto
_LL2E9;_tmp55C=((struct Cyc_Absyn_PointerType_struct*)_tmp55B)->f1;_tmp55D=
_tmp55C.ptr_atts;_tmp55E=_tmp55D.bounds;_LL2E8:{struct _tuple0 _tmp560=({struct
_tuple0 _tmp55F;_tmp55F.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp55A);
_tmp55F.f2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp55E);_tmp55F;});void*
_tmp561;void*_tmp562;struct Cyc_Absyn_Exp*_tmp563;void*_tmp564;void*_tmp565;
struct Cyc_Absyn_Exp*_tmp566;_LL2EC: _tmp561=_tmp560.f1;if((int)_tmp561 != 0)goto
_LL2EE;_tmp562=_tmp560.f2;if(_tmp562 <= (void*)2)goto _LL2EE;if(*((int*)_tmp562)!= 
0)goto _LL2EE;_tmp563=((struct Cyc_Absyn_Upper_b_struct*)_tmp562)->f1;_LL2ED:
forward_only=1;_tmp566=_tmp563;goto _LL2EF;_LL2EE: _tmp564=_tmp560.f1;if((int)
_tmp564 != 1)goto _LL2F0;_tmp565=_tmp560.f2;if(_tmp565 <= (void*)2)goto _LL2F0;if(*((
int*)_tmp565)!= 0)goto _LL2F0;_tmp566=((struct Cyc_Absyn_Upper_b_struct*)_tmp565)->f1;
_LL2EF: Cyc_Toc_exp_to_c(nv,_tmp553);(void*)(inner_mexp->r=(void*)Cyc_Toc_sizeoftyp_exp_r(
elt_typ));done=1;{struct Cyc_Absyn_Exp*_tmp567=forward_only?Cyc_Toc__init_dynforward_ptr_e:
Cyc_Toc__init_dyneither_ptr_e;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp567,({
struct Cyc_Absyn_Exp*_tmp568[4];_tmp568[3]=_tmp566;_tmp568[2]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp558),0);_tmp568[1]=_tmp553;_tmp568[0]=mexp;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp568,sizeof(struct Cyc_Absyn_Exp*),
4));})));goto _LL2EB;}_LL2F0:;_LL2F1: goto _LL2EB;_LL2EB:;}goto _LL2E6;_LL2E9:;
_LL2EA: goto _LL2E6;_LL2E6:;}goto _LL2E1;_LL2E4:;_LL2E5: goto _LL2E1;_LL2E1:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp569=Cyc_Absyn_exp_stmt(_tmp550,0);struct Cyc_Absyn_Exp*
_tmp56A=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp413);_tmp569=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp550,_tmp56A,0),_tmp413,0),
_tmp569,0);{void*_tmp56B=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp54F,_tmp56B,(struct Cyc_Absyn_Exp*)
mexp,_tmp569,0)));}}goto _LL2CC;}}_LL2CC:;}goto _LL1E0;_LL209: if(*((int*)_tmp3E7)
!= 19)goto _LL20B;_tmp414=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp3E7)->f1;
_LL20A: Cyc_Toc_exp_to_c(nv,_tmp414);goto _LL1E0;_LL20B: if(*((int*)_tmp3E7)!= 18)
goto _LL20D;_tmp415=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3E7)->f1;
_LL20C:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp56C=
_cycalloc(sizeof(*_tmp56C));_tmp56C[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp56D;_tmp56D.tag=18;_tmp56D.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp415);_tmp56D;});
_tmp56C;})));goto _LL1E0;_LL20D: if(*((int*)_tmp3E7)!= 21)goto _LL20F;_LL20E:({void*
_tmp56E=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp56F="__gen() in code generator";_tag_dynforward(_tmp56F,sizeof(
char),_get_zero_arr_size_char(_tmp56F,26));}),_tag_dynforward(_tmp56E,sizeof(
void*),0));});_LL20F: if(*((int*)_tmp3E7)!= 20)goto _LL211;_tmp416=(void*)((struct
Cyc_Absyn_Offsetof_e_struct*)_tmp3E7)->f1;_tmp417=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp3E7)->f2;if(*((int*)_tmp417)!= 0)goto _LL211;_tmp418=((struct Cyc_Absyn_StructField_struct*)
_tmp417)->f1;_LL210:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp570=_cycalloc(sizeof(*_tmp570));_tmp570[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp571;_tmp571.tag=20;_tmp571.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp416);_tmp571.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp572=_cycalloc(sizeof(*
_tmp572));_tmp572[0]=({struct Cyc_Absyn_StructField_struct _tmp573;_tmp573.tag=0;
_tmp573.f1=_tmp418;_tmp573;});_tmp572;}));_tmp571;});_tmp570;})));goto _LL1E0;
_LL211: if(*((int*)_tmp3E7)!= 20)goto _LL213;_tmp419=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp3E7)->f1;_tmp41A=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp3E7)->f2;
if(*((int*)_tmp41A)!= 1)goto _LL213;_tmp41B=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp41A)->f1;_LL212:{void*_tmp574=Cyc_Tcutil_compress(_tmp419);struct Cyc_Absyn_AggrInfo
_tmp575;union Cyc_Absyn_AggrInfoU_union _tmp576;struct Cyc_List_List*_tmp577;_LL2F3:
if(_tmp574 <= (void*)4)goto _LL2FB;if(*((int*)_tmp574)!= 10)goto _LL2F5;_tmp575=((
struct Cyc_Absyn_AggrType_struct*)_tmp574)->f1;_tmp576=_tmp575.aggr_info;_LL2F4: {
struct Cyc_Absyn_Aggrdecl*_tmp578=Cyc_Absyn_get_known_aggrdecl(_tmp576);if(
_tmp578->impl == 0)({void*_tmp579=0;((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp57A="struct fields must be known";
_tag_dynforward(_tmp57A,sizeof(char),_get_zero_arr_size_char(_tmp57A,28));}),
_tag_dynforward(_tmp579,sizeof(void*),0));});_tmp577=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp578->impl))->fields;goto _LL2F6;}_LL2F5: if(*((int*)_tmp574)!= 11)
goto _LL2F7;_tmp577=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp574)->f2;_LL2F6: {
struct Cyc_Absyn_Aggrfield*_tmp57B=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp577,(int)_tmp41B);(void*)(e->r=(void*)((void*)({struct
Cyc_Absyn_Offsetof_e_struct*_tmp57C=_cycalloc(sizeof(*_tmp57C));_tmp57C[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp57D;_tmp57D.tag=20;_tmp57D.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp419);_tmp57D.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp57E=
_cycalloc(sizeof(*_tmp57E));_tmp57E[0]=({struct Cyc_Absyn_StructField_struct
_tmp57F;_tmp57F.tag=0;_tmp57F.f1=_tmp57B->name;_tmp57F;});_tmp57E;}));_tmp57D;});
_tmp57C;})));goto _LL2F2;}_LL2F7: if(*((int*)_tmp574)!= 9)goto _LL2F9;_LL2F8:(void*)(
e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp580=_cycalloc(
sizeof(*_tmp580));_tmp580[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp581;_tmp581.tag=
20;_tmp581.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp419);_tmp581.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp582=_cycalloc(sizeof(*_tmp582));_tmp582[0]=({
struct Cyc_Absyn_StructField_struct _tmp583;_tmp583.tag=0;_tmp583.f1=Cyc_Absyn_fieldname((
int)(_tmp41B + 1));_tmp583;});_tmp582;}));_tmp581;});_tmp580;})));goto _LL2F2;
_LL2F9: if(*((int*)_tmp574)!= 3)goto _LL2FB;_LL2FA: if(_tmp41B == 0)(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp584=_cycalloc(sizeof(*
_tmp584));_tmp584[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp585;_tmp585.tag=20;
_tmp585.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp419);_tmp585.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp586=_cycalloc(sizeof(*_tmp586));_tmp586[0]=({
struct Cyc_Absyn_StructField_struct _tmp587;_tmp587.tag=0;_tmp587.f1=Cyc_Toc_tag_sp;
_tmp587;});_tmp586;}));_tmp585;});_tmp584;})));else{(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp588=_cycalloc(sizeof(*_tmp588));_tmp588[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp589;_tmp589.tag=20;_tmp589.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp419);_tmp589.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp58A=
_cycalloc(sizeof(*_tmp58A));_tmp58A[0]=({struct Cyc_Absyn_StructField_struct
_tmp58B;_tmp58B.tag=0;_tmp58B.f1=Cyc_Absyn_fieldname((int)_tmp41B);_tmp58B;});
_tmp58A;}));_tmp589;});_tmp588;})));}goto _LL2F2;_LL2FB:;_LL2FC:({void*_tmp58C=0;((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp58D="impossible type for offsetof tuple index";_tag_dynforward(
_tmp58D,sizeof(char),_get_zero_arr_size_char(_tmp58D,41));}),_tag_dynforward(
_tmp58C,sizeof(void*),0));});_LL2F2:;}goto _LL1E0;_LL213: if(*((int*)_tmp3E7)!= 22)
goto _LL215;_tmp41C=((struct Cyc_Absyn_Deref_e_struct*)_tmp3E7)->f1;_LL214: {void*
_tmp58E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp41C->topt))->v);{
void*_tmp58F=_tmp58E;struct Cyc_Absyn_PtrInfo _tmp590;void*_tmp591;struct Cyc_Absyn_Tqual
_tmp592;struct Cyc_Absyn_PtrAtts _tmp593;void*_tmp594;struct Cyc_Absyn_Conref*
_tmp595;struct Cyc_Absyn_Conref*_tmp596;struct Cyc_Absyn_Conref*_tmp597;_LL2FE: if(
_tmp58F <= (void*)4)goto _LL300;if(*((int*)_tmp58F)!= 4)goto _LL300;_tmp590=((
struct Cyc_Absyn_PointerType_struct*)_tmp58F)->f1;_tmp591=(void*)_tmp590.elt_typ;
_tmp592=_tmp590.elt_tq;_tmp593=_tmp590.ptr_atts;_tmp594=(void*)_tmp593.rgn;
_tmp595=_tmp593.nullable;_tmp596=_tmp593.bounds;_tmp597=_tmp593.zero_term;_LL2FF:{
void*_tmp598=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp596);_LL303: if(
_tmp598 <= (void*)2)goto _LL305;if(*((int*)_tmp598)!= 0)goto _LL305;_LL304: {int
do_null_check=Cyc_Toc_need_null_check(_tmp41C);Cyc_Toc_exp_to_c(nv,_tmp41C);if(
do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*_tmp599=0;Cyc_Tcutil_warn(e->loc,({
const char*_tmp59A="inserted null check due to dereference";_tag_dynforward(
_tmp59A,sizeof(char),_get_zero_arr_size_char(_tmp59A,39));}),_tag_dynforward(
_tmp599,sizeof(void*),0));});(void*)(_tmp41C->r=(void*)Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp58E),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B->hd=Cyc_Absyn_copy_exp(_tmp41C);
_tmp59B->tl=0;_tmp59B;}),0)));}goto _LL302;}_LL305: if((int)_tmp598 != 0)goto _LL307;
_LL306: goto _LL308;_LL307: if((int)_tmp598 != 1)goto _LL309;_LL308: {struct Cyc_Absyn_Exp*
_tmp59C=Cyc_Absyn_uint_exp(0,0);_tmp59C->topt=({struct Cyc_Core_Opt*_tmp59D=
_cycalloc(sizeof(*_tmp59D));_tmp59D->v=(void*)Cyc_Absyn_uint_typ;_tmp59D;});(
void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(_tmp41C,_tmp59C));Cyc_Toc_exp_to_c(nv,
e);goto _LL302;}_LL309: if(_tmp598 <= (void*)2)goto _LL302;if(*((int*)_tmp598)!= 1)
goto _LL302;_LL30A:({void*_tmp59E=0;((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp59F="exp_to_c: deref w/ AbsUpper_b";
_tag_dynforward(_tmp59F,sizeof(char),_get_zero_arr_size_char(_tmp59F,30));}),
_tag_dynforward(_tmp59E,sizeof(void*),0));});_LL302:;}goto _LL2FD;_LL300:;_LL301:({
void*_tmp5A0=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp5A1="exp_to_c: Deref: non-pointer";_tag_dynforward(_tmp5A1,sizeof(
char),_get_zero_arr_size_char(_tmp5A1,29));}),_tag_dynforward(_tmp5A0,sizeof(
void*),0));});_LL2FD:;}goto _LL1E0;}_LL215: if(*((int*)_tmp3E7)!= 23)goto _LL217;
_tmp41D=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp3E7)->f1;_tmp41E=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp3E7)->f2;_LL216: Cyc_Toc_exp_to_c(nv,_tmp41D);if(Cyc_Toc_is_poly_project(e))(
void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));
goto _LL1E0;_LL217: if(*((int*)_tmp3E7)!= 24)goto _LL219;_tmp41F=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp3E7)->f1;_tmp420=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp3E7)->f2;_LL218: {
void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp41F->topt))->v);
int do_null_check=Cyc_Toc_need_null_check(_tmp41F);Cyc_Toc_exp_to_c(nv,_tmp41F);{
int is_poly=Cyc_Toc_is_poly_project(e);int forward_only=0;void*_tmp5A3;struct Cyc_Absyn_Tqual
_tmp5A4;struct Cyc_Absyn_PtrAtts _tmp5A5;void*_tmp5A6;struct Cyc_Absyn_Conref*
_tmp5A7;struct Cyc_Absyn_Conref*_tmp5A8;struct Cyc_Absyn_Conref*_tmp5A9;struct Cyc_Absyn_PtrInfo
_tmp5A2=Cyc_Toc_get_ptr_type(e1typ);_tmp5A3=(void*)_tmp5A2.elt_typ;_tmp5A4=
_tmp5A2.elt_tq;_tmp5A5=_tmp5A2.ptr_atts;_tmp5A6=(void*)_tmp5A5.rgn;_tmp5A7=
_tmp5A5.nullable;_tmp5A8=_tmp5A5.bounds;_tmp5A9=_tmp5A5.zero_term;{void*_tmp5AA=
Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp5A8);struct Cyc_Absyn_Exp*_tmp5AB;
_LL30C: if(_tmp5AA <= (void*)2)goto _LL30E;if(*((int*)_tmp5AA)!= 0)goto _LL30E;
_tmp5AB=((struct Cyc_Absyn_Upper_b_struct*)_tmp5AA)->f1;_LL30D: {unsigned int
_tmp5AD;int _tmp5AE;struct _tuple5 _tmp5AC=Cyc_Evexp_eval_const_uint_exp(_tmp5AB);
_tmp5AD=_tmp5AC.f1;_tmp5AE=_tmp5AC.f2;if(_tmp5AE){if(_tmp5AD < 1)({void*_tmp5AF=0;((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp5B0="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dynforward(
_tmp5B0,sizeof(char),_get_zero_arr_size_char(_tmp5B0,44));}),_tag_dynforward(
_tmp5AF,sizeof(void*),0));});if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({
void*_tmp5B1=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5B2="inserted null check due to dereference";
_tag_dynforward(_tmp5B2,sizeof(char),_get_zero_arr_size_char(_tmp5B2,39));}),
_tag_dynforward(_tmp5B1,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_aggrarrow_exp_r(
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp41F->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp5B3=_cycalloc(sizeof(*_tmp5B3));_tmp5B3->hd=_tmp41F;_tmp5B3->tl=0;_tmp5B3;}),
0)),_tmp420));}}else{(void*)(e->r=(void*)Cyc_Toc_aggrarrow_exp_r(Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp41F->topt))->v),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*
_tmp5B4[4];_tmp5B4[3]=Cyc_Absyn_uint_exp(0,0);_tmp5B4[2]=Cyc_Absyn_sizeoftyp_exp(
_tmp5A3,0);_tmp5B4[1]=_tmp5AB;_tmp5B4[0]=_tmp41F;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp5B4,sizeof(struct Cyc_Absyn_Exp*),
4));}),0)),_tmp420));}goto _LL30B;}_LL30E: if((int)_tmp5AA != 0)goto _LL310;_LL30F:
forward_only=1;goto _LL311;_LL310: if((int)_tmp5AA != 1)goto _LL312;_LL311: {struct
Cyc_Absyn_Exp*_tmp5B5=forward_only?Cyc_Toc__check_dynforward_subscript_e: Cyc_Toc__check_dyneither_subscript_e;
void*ta1=Cyc_Toc_typ_to_c_array(_tmp5A3);(void*)(_tmp41F->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_cstar_typ(ta1,_tmp5A4),Cyc_Absyn_fncall_exp(_tmp5B5,({struct Cyc_Absyn_Exp*
_tmp5B6[3];_tmp5B6[2]=Cyc_Absyn_uint_exp(0,0);_tmp5B6[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp5B6[0]=Cyc_Absyn_copy_exp(_tmp41F);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp5B6,sizeof(struct Cyc_Absyn_Exp*),
3));}),0)));goto _LL30B;}_LL312: if(_tmp5AA <= (void*)2)goto _LL30B;if(*((int*)
_tmp5AA)!= 1)goto _LL30B;_LL313:({void*_tmp5B7=0;((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp5B8="exp_to_c: AggrArrow w/ AbsUpper_b";
_tag_dynforward(_tmp5B8,sizeof(char),_get_zero_arr_size_char(_tmp5B8,34));}),
_tag_dynforward(_tmp5B7,sizeof(void*),0));});_LL30B:;}if(is_poly)(void*)(e->r=(
void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1E0;}}
_LL219: if(*((int*)_tmp3E7)!= 25)goto _LL21B;_tmp421=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp3E7)->f1;_tmp422=((struct Cyc_Absyn_Subscript_e_struct*)_tmp3E7)->f2;_LL21A: {
void*_tmp5B9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp421->topt))->v);{void*_tmp5BA=_tmp5B9;struct Cyc_List_List*_tmp5BB;struct Cyc_Absyn_PtrInfo
_tmp5BC;void*_tmp5BD;struct Cyc_Absyn_Tqual _tmp5BE;struct Cyc_Absyn_PtrAtts _tmp5BF;
void*_tmp5C0;struct Cyc_Absyn_Conref*_tmp5C1;struct Cyc_Absyn_Conref*_tmp5C2;
struct Cyc_Absyn_Conref*_tmp5C3;_LL315: if(_tmp5BA <= (void*)4)goto _LL319;if(*((int*)
_tmp5BA)!= 9)goto _LL317;_tmp5BB=((struct Cyc_Absyn_TupleType_struct*)_tmp5BA)->f1;
_LL316: Cyc_Toc_exp_to_c(nv,_tmp421);Cyc_Toc_exp_to_c(nv,_tmp422);{unsigned int
_tmp5C5;int _tmp5C6;struct _tuple5 _tmp5C4=Cyc_Evexp_eval_const_uint_exp(_tmp422);
_tmp5C5=_tmp5C4.f1;_tmp5C6=_tmp5C4.f2;if(!_tmp5C6)({void*_tmp5C7=0;((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp5C8="unknown tuple subscript in translation to C";_tag_dynforward(_tmp5C8,
sizeof(char),_get_zero_arr_size_char(_tmp5C8,44));}),_tag_dynforward(_tmp5C7,
sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_aggrmember_exp_r(_tmp421,Cyc_Absyn_fieldname((
int)(_tmp5C5 + 1))));goto _LL314;}_LL317: if(*((int*)_tmp5BA)!= 4)goto _LL319;
_tmp5BC=((struct Cyc_Absyn_PointerType_struct*)_tmp5BA)->f1;_tmp5BD=(void*)
_tmp5BC.elt_typ;_tmp5BE=_tmp5BC.elt_tq;_tmp5BF=_tmp5BC.ptr_atts;_tmp5C0=(void*)
_tmp5BF.rgn;_tmp5C1=_tmp5BF.nullable;_tmp5C2=_tmp5BF.bounds;_tmp5C3=_tmp5BF.zero_term;
_LL318: {struct Cyc_List_List*_tmp5C9=Cyc_Toc_get_relns(_tmp421);int in_bnds=0;{
void*_tmp5CA=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp5C2);_LL31C: if(
_tmp5CA <= (void*)2)goto _LL31E;if(*((int*)_tmp5CA)!= 1)goto _LL31E;_LL31D: goto
_LL31B;_LL31E:;_LL31F: in_bnds=Cyc_Toc_check_bounds(_tmp5C9,_tmp421,_tmp422);if(
Cyc_Toc_warn_bounds_checks  && !in_bnds)({struct Cyc_String_pa_struct _tmp5CD;
_tmp5CD.tag=0;_tmp5CD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp5CB[1]={& _tmp5CD};Cyc_Tcutil_warn(e->loc,({const char*_tmp5CC="bounds check necessary for %s";
_tag_dynforward(_tmp5CC,sizeof(char),_get_zero_arr_size_char(_tmp5CC,30));}),
_tag_dynforward(_tmp5CB,sizeof(void*),1));}});_LL31B:;}Cyc_Toc_exp_to_c(nv,
_tmp421);Cyc_Toc_exp_to_c(nv,_tmp422);Cyc_Toc_total_bounds_checks ++;{int
forward_only=0;{void*_tmp5CE=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp5C2);
struct Cyc_Absyn_Exp*_tmp5CF;void*_tmp5D0;_LL321: if(_tmp5CE <= (void*)2)goto _LL323;
if(*((int*)_tmp5CE)!= 0)goto _LL323;_tmp5CF=((struct Cyc_Absyn_Upper_b_struct*)
_tmp5CE)->f1;_LL322: {int possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp5C1);void*ta1=Cyc_Toc_typ_to_c(_tmp5BD);void*ta2=Cyc_Absyn_cstar_typ(
ta1,_tmp5BE);if(in_bnds)Cyc_Toc_bounds_checks_eliminated ++;else{if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp5C3)){struct Cyc_Absyn_Exp*
function_e=Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,_tmp421);(
void*)(e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(
function_e,({struct Cyc_Absyn_Exp*_tmp5D1[3];_tmp5D1[2]=_tmp422;_tmp5D1[1]=
_tmp5CF;_tmp5D1[0]=_tmp421;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp5D1,sizeof(struct Cyc_Absyn_Exp*),3));}),0))));}else{if(
possibly_null){if(Cyc_Toc_warn_all_null_deref)({void*_tmp5D2=0;Cyc_Tcutil_warn(e->loc,({
const char*_tmp5D3="inserted null check due to dereference";_tag_dynforward(
_tmp5D3,sizeof(char),_get_zero_arr_size_char(_tmp5D3,39));}),_tag_dynforward(
_tmp5D2,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*
_tmp5D4[4];_tmp5D4[3]=_tmp422;_tmp5D4[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp5D4[
1]=_tmp5CF;_tmp5D4[0]=_tmp421;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp5D4,sizeof(struct Cyc_Absyn_Exp*),4));}),0))));}
else{(void*)(_tmp422->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp5D5[2];_tmp5D5[1]=Cyc_Absyn_copy_exp(_tmp422);_tmp5D5[0]=
_tmp5CF;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp5D5,sizeof(struct Cyc_Absyn_Exp*),2));})));}}}goto _LL320;}
_LL323: if((int)_tmp5CE != 0)goto _LL325;_LL324: forward_only=1;goto _LL326;_LL325:
if((int)_tmp5CE != 1)goto _LL327;_LL326: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp5BD);
if(in_bnds){Cyc_Toc_bounds_checks_eliminated ++;(void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5BE),Cyc_Absyn_aggrmember_exp(
_tmp421,Cyc_Toc_curr_sp,0)),_tmp422));}else{struct Cyc_Absyn_Exp*_tmp5D6=
forward_only?Cyc_Toc__check_dynforward_subscript_e: Cyc_Toc__check_dyneither_subscript_e;(
void*)(e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,
_tmp5BE),Cyc_Absyn_fncall_exp(_tmp5D6,({struct Cyc_Absyn_Exp*_tmp5D7[3];_tmp5D7[2]=
_tmp422;_tmp5D7[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp5D7[0]=_tmp421;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp5D7,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))));}goto _LL320;}_LL327: if(_tmp5CE <= (void*)2)goto _LL320;if(*((int*)
_tmp5CE)!= 1)goto _LL320;_tmp5D0=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp5CE)->f1;_LL328:{void*_tmp5D8=(void*)_tmp422->annot;struct Cyc_List_List*
_tmp5D9;_LL32A: if(*((void**)_tmp5D8)!= Cyc_CfFlowInfo_HasTagCmps)goto _LL32C;
_tmp5D9=((struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp5D8)->f1;_LL32B: for(0;
_tmp5D9 != 0;_tmp5D9=_tmp5D9->tl){if((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp5D9->hd)->cmp == (void*)8  && Cyc_Tcutil_typecmp((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp5D9->hd)->bd,_tmp5D0)== 0){in_bnds=1;break;}}goto _LL329;_LL32C:;_LL32D: goto
_LL329;_LL329:;}if(!in_bnds)({void*_tmp5DA=0;Cyc_Tcutil_terr(e->loc,({const char*
_tmp5DB="cannot determine that subscript is in bounds";_tag_dynforward(_tmp5DB,
sizeof(char),_get_zero_arr_size_char(_tmp5DB,45));}),_tag_dynforward(_tmp5DA,
sizeof(void*),0));});goto _LL320;_LL320:;}goto _LL314;}}_LL319:;_LL31A:({void*
_tmp5DC=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp5DD="exp_to_c: Subscript on non-tuple/array/tuple ptr";
_tag_dynforward(_tmp5DD,sizeof(char),_get_zero_arr_size_char(_tmp5DD,49));}),
_tag_dynforward(_tmp5DC,sizeof(void*),0));});_LL314:;}goto _LL1E0;}_LL21B: if(*((
int*)_tmp3E7)!= 26)goto _LL21D;_tmp423=((struct Cyc_Absyn_Tuple_e_struct*)_tmp3E7)->f1;
_LL21C: if(!Cyc_Toc_is_toplevel(nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,0,0,_tmp423))->r));else{struct Cyc_List_List*_tmp5DE=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_tup_to_c,_tmp423);void*_tmp5DF=Cyc_Toc_add_tuple_type(_tmp5DE);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp423 != 0;(_tmp423=_tmp423->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp423->hd);dles=({struct Cyc_List_List*_tmp5E0=_cycalloc(
sizeof(*_tmp5E0));_tmp5E0->hd=({struct _tuple8*_tmp5E1=_cycalloc(sizeof(*_tmp5E1));
_tmp5E1->f1=0;_tmp5E1->f2=(struct Cyc_Absyn_Exp*)_tmp423->hd;_tmp5E1;});_tmp5E0->tl=
dles;_tmp5E0;});}}dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles);(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,dles));}goto _LL1E0;
_LL21D: if(*((int*)_tmp3E7)!= 28)goto _LL21F;_tmp424=((struct Cyc_Absyn_Array_e_struct*)
_tmp3E7)->f1;_LL21E:(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,_tmp424));{
struct Cyc_List_List*_tmp5E2=_tmp424;for(0;_tmp5E2 != 0;_tmp5E2=_tmp5E2->tl){
struct _tuple8 _tmp5E4;struct Cyc_Absyn_Exp*_tmp5E5;struct _tuple8*_tmp5E3=(struct
_tuple8*)_tmp5E2->hd;_tmp5E4=*_tmp5E3;_tmp5E5=_tmp5E4.f2;Cyc_Toc_exp_to_c(nv,
_tmp5E5);}}goto _LL1E0;_LL21F: if(*((int*)_tmp3E7)!= 29)goto _LL221;_tmp425=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp3E7)->f1;_tmp426=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3E7)->f2;_tmp427=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3E7)->f3;
_tmp428=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3E7)->f4;_LL220: {
unsigned int _tmp5E7;int _tmp5E8;struct _tuple5 _tmp5E6=Cyc_Evexp_eval_const_uint_exp(
_tmp426);_tmp5E7=_tmp5E6.f1;_tmp5E8=_tmp5E6.f2;{void*_tmp5E9=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp427->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp427);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp427)){if(!_tmp5E8)({
void*_tmp5EA=0;Cyc_Tcutil_terr(_tmp426->loc,({const char*_tmp5EB="cannot determine value of constant";
_tag_dynforward(_tmp5EB,sizeof(char),_get_zero_arr_size_char(_tmp5EB,35));}),
_tag_dynforward(_tmp5EA,sizeof(void*),0));});{unsigned int i=0;for(0;i < _tmp5E7;i
++){es=({struct Cyc_List_List*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC->hd=({
struct _tuple8*_tmp5ED=_cycalloc(sizeof(*_tmp5ED));_tmp5ED->f1=0;_tmp5ED->f2=
_tmp427;_tmp5ED;});_tmp5EC->tl=es;_tmp5EC;});}}if(_tmp428){struct Cyc_Absyn_Exp*
_tmp5EE=Cyc_Toc_cast_it(_tmp5E9,Cyc_Absyn_uint_exp(0,0));es=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*
_tmp5EF=_cycalloc(sizeof(*_tmp5EF));_tmp5EF->hd=({struct _tuple8*_tmp5F0=
_cycalloc(sizeof(*_tmp5F0));_tmp5F0->f1=0;_tmp5F0->f2=_tmp5EE;_tmp5F0;});_tmp5EF->tl=
0;_tmp5EF;}));}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,es));goto
_LL1E0;}}}_LL221: if(*((int*)_tmp3E7)!= 30)goto _LL223;_tmp429=((struct Cyc_Absyn_Struct_e_struct*)
_tmp3E7)->f1;_tmp42A=((struct Cyc_Absyn_Struct_e_struct*)_tmp3E7)->f2;_tmp42B=((
struct Cyc_Absyn_Struct_e_struct*)_tmp3E7)->f3;_tmp42C=((struct Cyc_Absyn_Struct_e_struct*)
_tmp3E7)->f4;_LL222: if(!Cyc_Toc_is_toplevel(nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,old_typ,_tmp42A != 0,0,0,_tmp42B,_tmp429))->r));else{if(_tmp42C == 0)({void*
_tmp5F1=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp5F2="Struct_e: missing aggrdecl pointer";_tag_dynforward(_tmp5F2,
sizeof(char),_get_zero_arr_size_char(_tmp5F2,35));}),_tag_dynforward(_tmp5F1,
sizeof(void*),0));});{struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)
_tmp42C;struct _RegionHandle _tmp5F3=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp5F3;_push_region(rgn);{struct Cyc_List_List*_tmp5F4=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,
_tmp42B,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);struct
Cyc_List_List*_tmp5F5=0;struct Cyc_List_List*_tmp5F6=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp5F6 != 0;_tmp5F6=_tmp5F6->tl){struct Cyc_List_List*
_tmp5F7=_tmp5F4;for(0;_tmp5F7 != 0;_tmp5F7=_tmp5F7->tl){if((*((struct _tuple12*)
_tmp5F7->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp5F6->hd){struct _tuple12
_tmp5F9;struct Cyc_Absyn_Aggrfield*_tmp5FA;struct Cyc_Absyn_Exp*_tmp5FB;struct
_tuple12*_tmp5F8=(struct _tuple12*)_tmp5F7->hd;_tmp5F9=*_tmp5F8;_tmp5FA=_tmp5F9.f1;
_tmp5FB=_tmp5F9.f2;{void*_tmp5FC=(void*)_tmp5FA->type;Cyc_Toc_exp_to_c(nv,
_tmp5FB);if(Cyc_Toc_is_void_star(_tmp5FC))(void*)(_tmp5FB->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp5FB->r,0)));_tmp5F5=({
struct Cyc_List_List*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));_tmp5FD->hd=({struct
_tuple8*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));_tmp5FE->f1=0;_tmp5FE->f2=_tmp5FB;
_tmp5FE;});_tmp5FD->tl=_tmp5F5;_tmp5FD;});break;}}}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5F5)));};
_pop_region(rgn);}}goto _LL1E0;_LL223: if(*((int*)_tmp3E7)!= 31)goto _LL225;_tmp42D=(
void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3E7)->f1;_tmp42E=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp3E7)->f2;_LL224: {struct Cyc_List_List*fs;{void*_tmp5FF=Cyc_Tcutil_compress(
_tmp42D);struct Cyc_List_List*_tmp600;_LL32F: if(_tmp5FF <= (void*)4)goto _LL331;if(*((
int*)_tmp5FF)!= 11)goto _LL331;_tmp600=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp5FF)->f2;_LL330: fs=_tmp600;goto _LL32E;_LL331:;_LL332:({struct Cyc_String_pa_struct
_tmp603;_tmp603.tag=0;_tmp603.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(_tmp42D));{void*_tmp601[1]={& _tmp603};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp602="anon struct has type %s";_tag_dynforward(_tmp602,sizeof(char),
_get_zero_arr_size_char(_tmp602,24));}),_tag_dynforward(_tmp601,sizeof(void*),1));}});
_LL32E:;}{struct _RegionHandle _tmp604=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp604;_push_region(rgn);{struct Cyc_List_List*_tmp605=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,
_tmp42E,fs);for(0;_tmp605 != 0;_tmp605=_tmp605->tl){struct _tuple12 _tmp607;struct
Cyc_Absyn_Aggrfield*_tmp608;struct Cyc_Absyn_Exp*_tmp609;struct _tuple12*_tmp606=(
struct _tuple12*)_tmp605->hd;_tmp607=*_tmp606;_tmp608=_tmp607.f1;_tmp609=_tmp607.f2;{
void*_tmp60A=(void*)_tmp608->type;Cyc_Toc_exp_to_c(nv,_tmp609);if(Cyc_Toc_is_void_star(
_tmp60A))(void*)(_tmp609->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp609->r,0)));}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,_tmp42E));};_pop_region(rgn);}goto _LL1E0;}_LL225: if(*((int*)_tmp3E7)!= 32)goto
_LL227;_tmp42F=((struct Cyc_Absyn_Tunion_e_struct*)_tmp3E7)->f1;if(_tmp42F != 0)
goto _LL227;_tmp430=((struct Cyc_Absyn_Tunion_e_struct*)_tmp3E7)->f2;_tmp431=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp3E7)->f3;if(!(!_tmp430->is_flat))goto _LL227;
_LL226: {struct _tuple1*qv=_tmp431->name;struct Cyc_Absyn_Exp*tag_exp=_tmp430->is_xtunion?
Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(_tmp430,qv,0);(void*)(e->r=(void*)((
void*)tag_exp->r));goto _LL1E0;}_LL227: if(*((int*)_tmp3E7)!= 32)goto _LL229;
_tmp432=((struct Cyc_Absyn_Tunion_e_struct*)_tmp3E7)->f1;_tmp433=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp3E7)->f2;_tmp434=((struct Cyc_Absyn_Tunion_e_struct*)_tmp3E7)->f3;_LL228: {
void*tunion_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple1*_tmp60B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp60C=Cyc_Absyn_var_exp(_tmp60B,0);struct Cyc_Absyn_Exp*
member_exp;if(_tmp433->is_flat){tunion_ctype=Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(
_tmp433->name,({const char*_tmp60D="_union";_tag_dynforward(_tmp60D,sizeof(char),
_get_zero_arr_size_char(_tmp60D,7));})));tag_exp=Cyc_Toc_tunion_tag(_tmp433,
_tmp434->name,1);{struct _tuple1 _tmp60F;struct _dynforward_ptr*_tmp610;struct
_tuple1*_tmp60E=_tmp434->name;_tmp60F=*_tmp60E;_tmp610=_tmp60F.f2;member_exp=Cyc_Absyn_aggrmember_exp(
_tmp60C,_tmp610,0);}}else{tunion_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp434->name,({const char*_tmp611="_struct";_tag_dynforward(_tmp611,sizeof(char),
_get_zero_arr_size_char(_tmp611,8));})));tag_exp=_tmp433->is_xtunion?Cyc_Absyn_var_exp(
_tmp434->name,0): Cyc_Toc_tunion_tag(_tmp433,_tmp434->name,1);member_exp=_tmp60C;}{
struct Cyc_List_List*_tmp612=_tmp434->typs;if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*
dles=0;for(0;_tmp432 != 0;(_tmp432=_tmp432->tl,_tmp612=_tmp612->tl)){struct Cyc_Absyn_Exp*
cur_e=(struct Cyc_Absyn_Exp*)_tmp432->hd;void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp612))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))cur_e=Cyc_Toc_cast_it(field_typ,
cur_e);dles=({struct Cyc_List_List*_tmp613=_cycalloc(sizeof(*_tmp613));_tmp613->hd=({
struct _tuple8*_tmp614=_cycalloc(sizeof(*_tmp614));_tmp614->f1=0;_tmp614->f2=
cur_e;_tmp614;});_tmp613->tl=dles;_tmp613;});}dles=({struct Cyc_List_List*_tmp615=
_cycalloc(sizeof(*_tmp615));_tmp615->hd=({struct _tuple8*_tmp616=_cycalloc(
sizeof(*_tmp616));_tmp616->f1=0;_tmp616->f2=tag_exp;_tmp616;});_tmp615->tl=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp615;});(
void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,dles));if(_tmp433->is_flat)(
void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp434->name,({const char*_tmp617="_struct";_tag_dynforward(_tmp617,sizeof(char),
_get_zero_arr_size_char(_tmp617,8));}))),Cyc_Absyn_copy_exp(e)));}else{struct Cyc_List_List*
_tmp618=({struct Cyc_List_List*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E->hd=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,0),
tag_exp,0);_tmp61E->tl=0;_tmp61E;});{int i=1;for(0;_tmp432 != 0;(((_tmp432=_tmp432->tl,
i ++)),_tmp612=_tmp612->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp432->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp612))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp619=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);_tmp618=({struct Cyc_List_List*_tmp61A=_cycalloc(sizeof(*_tmp61A));
_tmp61A->hd=_tmp619;_tmp61A->tl=_tmp618;_tmp61A;});}}}{struct Cyc_Absyn_Stmt*
_tmp61B=Cyc_Absyn_exp_stmt(_tmp60C,0);struct Cyc_Absyn_Stmt*_tmp61C=Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*
_tmp61D=_cycalloc(sizeof(*_tmp61D));_tmp61D->hd=_tmp61B;_tmp61D->tl=_tmp618;
_tmp61D;})),0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(
_tmp60B,tunion_ctype,0,_tmp61C,0)));}}goto _LL1E0;}}_LL229: if(*((int*)_tmp3E7)!= 
33)goto _LL22B;_LL22A: goto _LL22C;_LL22B: if(*((int*)_tmp3E7)!= 34)goto _LL22D;
_LL22C: goto _LL1E0;_LL22D: if(*((int*)_tmp3E7)!= 35)goto _LL22F;_tmp435=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp3E7)->f1;_tmp436=_tmp435.is_calloc;_tmp437=_tmp435.rgn;_tmp438=_tmp435.elt_type;
_tmp439=_tmp435.num_elts;_tmp43A=_tmp435.fat_result;_LL22E: {void*t_c=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp438)));Cyc_Toc_exp_to_c(nv,_tmp439);if(_tmp43A){struct
_tuple1*_tmp61F=Cyc_Toc_temp_var();struct _tuple1*_tmp620=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(
_tmp436){xexp=_tmp439;if(_tmp437 != 0  && !Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*
rgn=(struct Cyc_Absyn_Exp*)_tmp437;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(
rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp61F,0));}else{pexp=Cyc_Toc_calloc_exp(*
_tmp438,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp61F,0));}rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*_tmp621[3];_tmp621[2]=Cyc_Absyn_var_exp(
_tmp61F,0);_tmp621[1]=Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp621[0]=Cyc_Absyn_var_exp(
_tmp620,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp621,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp439,0);if(_tmp437 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp437;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp61F,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp438,Cyc_Absyn_var_exp(_tmp61F,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp622[3];_tmp622[2]=Cyc_Absyn_var_exp(_tmp61F,0);_tmp622[1]=
Cyc_Absyn_uint_exp(1,0);_tmp622[0]=Cyc_Absyn_var_exp(_tmp620,0);((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp622,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);}{struct Cyc_Absyn_Stmt*_tmp623=Cyc_Absyn_declare_stmt(_tmp61F,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp620,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)Cyc_Toc_stmt_exp_r(_tmp623));}}else{if(_tmp437 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp437;Cyc_Toc_exp_to_c(nv,rgn);(
void*)(e->r=(void*)((void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,
0)))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp438,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}}goto _LL1E0;}_LL22F: if(*((int*)_tmp3E7)!= 36)goto _LL231;_tmp43B=((
struct Cyc_Absyn_Swap_e_struct*)_tmp3E7)->f1;_tmp43C=((struct Cyc_Absyn_Swap_e_struct*)
_tmp3E7)->f2;_LL230: {int is_dynforward_ptr=0;int is_dyneither_ptr=0;void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp43B->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp43C->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dynforward_ptr,& is_dyneither_ptr))({void*_tmp624=0;((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp625="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";
_tag_dynforward(_tmp625,sizeof(char),_get_zero_arr_size_char(_tmp625,57));}),
_tag_dynforward(_tmp624,sizeof(void*),0));});{struct Cyc_Absyn_Exp*swap_fn;if(
is_dynforward_ptr)swap_fn=Cyc_Toc__swap_dynforward_e;else{if(is_dyneither_ptr)
swap_fn=Cyc_Toc__swap_dyneither_e;else{swap_fn=Cyc_Toc__swap_word_e;}}if(!Cyc_Absyn_is_lvalue(
_tmp43B))({struct Cyc_String_pa_struct _tmp628;_tmp628.tag=0;_tmp628.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(_tmp43B));{void*
_tmp626[1]={& _tmp628};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp627="Swap_e: %s is not an l-value\n";
_tag_dynforward(_tmp627,sizeof(char),_get_zero_arr_size_char(_tmp627,30));}),
_tag_dynforward(_tmp626,sizeof(void*),1));}});if(!Cyc_Absyn_is_lvalue(_tmp43C))({
struct Cyc_String_pa_struct _tmp62B;_tmp62B.tag=0;_tmp62B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(_tmp43C));{void*
_tmp629[1]={& _tmp62B};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp62A="Swap_e: %s is not an l-value\n";
_tag_dynforward(_tmp62A,sizeof(char),_get_zero_arr_size_char(_tmp62A,30));}),
_tag_dynforward(_tmp629,sizeof(void*),1));}});Cyc_Toc_exp_to_c(nv,_tmp43B);Cyc_Toc_exp_to_c(
nv,_tmp43C);(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(swap_fn,({struct Cyc_Absyn_Exp*
_tmp62C[2];_tmp62C[1]=Cyc_Absyn_address_exp(_tmp43C,0);_tmp62C[0]=Cyc_Absyn_address_exp(
_tmp43B,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp62C,sizeof(struct Cyc_Absyn_Exp*),2));})));goto _LL1E0;}}
_LL231: if(*((int*)_tmp3E7)!= 38)goto _LL233;_tmp43D=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp3E7)->f1;_LL232: Cyc_Toc_stmt_to_c(nv,_tmp43D);goto _LL1E0;_LL233: if(*((int*)
_tmp3E7)!= 37)goto _LL235;_LL234:({void*_tmp62D=0;((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp62E="UnresolvedMem";
_tag_dynforward(_tmp62E,sizeof(char),_get_zero_arr_size_char(_tmp62E,14));}),
_tag_dynforward(_tmp62D,sizeof(void*),0));});_LL235: if(*((int*)_tmp3E7)!= 27)
goto _LL1E0;_LL236:({void*_tmp62F=0;((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp630="compoundlit";
_tag_dynforward(_tmp630,sizeof(char),_get_zero_arr_size_char(_tmp630,12));}),
_tag_dynforward(_tmp62F,sizeof(void*),0));});_LL1E0:;}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);}struct _tuple13{struct Cyc_Toc_Env*f1;struct Cyc_List_List*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple14{struct _tuple1*f1;void*f2;};struct
_tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple13 Cyc_Toc_xlate_pat(
struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct
Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt**tag_fail_stmt,
struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){struct Cyc_Absyn_Stmt*s;{
void*_tmp631=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp632;struct Cyc_Absyn_Vardecl
_tmp633;struct _tuple1*_tmp634;struct Cyc_Absyn_Pat*_tmp635;struct Cyc_Absyn_Vardecl*
_tmp636;struct Cyc_Absyn_Vardecl _tmp637;struct _tuple1*_tmp638;struct Cyc_Absyn_Vardecl*
_tmp639;struct Cyc_Absyn_Pat*_tmp63A;void*_tmp63B;int _tmp63C;char _tmp63D;struct
_dynforward_ptr _tmp63E;struct Cyc_Absyn_Enumdecl*_tmp63F;struct Cyc_Absyn_Enumfield*
_tmp640;void*_tmp641;struct Cyc_Absyn_Enumfield*_tmp642;struct Cyc_Absyn_Tuniondecl*
_tmp643;struct Cyc_Absyn_Tunionfield*_tmp644;struct Cyc_List_List*_tmp645;struct
Cyc_Absyn_Pat*_tmp646;struct Cyc_Absyn_Pat _tmp647;void*_tmp648;struct Cyc_Absyn_Tuniondecl*
_tmp649;struct Cyc_Absyn_Tunionfield*_tmp64A;struct Cyc_List_List*_tmp64B;struct
Cyc_Absyn_Tuniondecl*_tmp64C;struct Cyc_Absyn_Tunionfield*_tmp64D;struct Cyc_List_List*
_tmp64E;struct Cyc_List_List*_tmp64F;struct Cyc_List_List*_tmp650;struct Cyc_Absyn_AggrInfo
_tmp651;union Cyc_Absyn_AggrInfoU_union _tmp652;struct Cyc_List_List*_tmp653;struct
Cyc_Absyn_Pat*_tmp654;_LL334: if(_tmp631 <= (void*)2)goto _LL338;if(*((int*)_tmp631)
!= 0)goto _LL336;_tmp632=((struct Cyc_Absyn_Var_p_struct*)_tmp631)->f1;_tmp633=*
_tmp632;_tmp634=_tmp633.name;_tmp635=((struct Cyc_Absyn_Var_p_struct*)_tmp631)->f2;
_LL335: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp634,r),rgn,t,r,path,
_tmp635,tag_fail_stmt,fail_stmt,decls);_LL336: if(*((int*)_tmp631)!= 2)goto _LL338;
_tmp636=((struct Cyc_Absyn_TagInt_p_struct*)_tmp631)->f2;_tmp637=*_tmp636;_tmp638=
_tmp637.name;_LL337: nv=Cyc_Toc_add_varmap(rgn,nv,_tmp638,r);goto _LL339;_LL338:
if((int)_tmp631 != 0)goto _LL33A;_LL339: s=Cyc_Toc_skip_stmt_dl();goto _LL333;_LL33A:
if(_tmp631 <= (void*)2)goto _LL33C;if(*((int*)_tmp631)!= 1)goto _LL33C;_tmp639=((
struct Cyc_Absyn_Reference_p_struct*)_tmp631)->f1;_tmp63A=((struct Cyc_Absyn_Reference_p_struct*)
_tmp631)->f2;_LL33B: {struct _tuple1*_tmp655=Cyc_Toc_temp_var();decls=({struct Cyc_List_List*
_tmp656=_region_malloc(rgn,sizeof(*_tmp656));_tmp656->hd=({struct _tuple14*
_tmp657=_region_malloc(rgn,sizeof(*_tmp657));_tmp657->f1=_tmp655;_tmp657->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp657;});_tmp656->tl=decls;_tmp656;});nv=
Cyc_Toc_add_varmap(rgn,nv,_tmp639->name,Cyc_Absyn_var_exp(_tmp655,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp655,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(path,0)),0);{struct _tuple13 _tmp658=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp63A,tag_fail_stmt,fail_stmt,decls);_tmp658.f3=Cyc_Absyn_seq_stmt(
s,_tmp658.f3,0);return _tmp658;}}_LL33C: if((int)_tmp631 != 1)goto _LL33E;_LL33D: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL333;_LL33E:
if(_tmp631 <= (void*)2)goto _LL340;if(*((int*)_tmp631)!= 7)goto _LL340;_tmp63B=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp631)->f1;_tmp63C=((struct Cyc_Absyn_Int_p_struct*)
_tmp631)->f2;_LL33F: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp63B,_tmp63C,0),
fail_stmt);goto _LL333;_LL340: if(_tmp631 <= (void*)2)goto _LL342;if(*((int*)_tmp631)
!= 8)goto _LL342;_tmp63D=((struct Cyc_Absyn_Char_p_struct*)_tmp631)->f1;_LL341: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp63D,0),fail_stmt);goto _LL333;_LL342:
if(_tmp631 <= (void*)2)goto _LL344;if(*((int*)_tmp631)!= 9)goto _LL344;_tmp63E=((
struct Cyc_Absyn_Float_p_struct*)_tmp631)->f1;_LL343: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(
_tmp63E,0),fail_stmt);goto _LL333;_LL344: if(_tmp631 <= (void*)2)goto _LL346;if(*((
int*)_tmp631)!= 10)goto _LL346;_tmp63F=((struct Cyc_Absyn_Enum_p_struct*)_tmp631)->f1;
_tmp640=((struct Cyc_Absyn_Enum_p_struct*)_tmp631)->f2;_LL345: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp659=_cycalloc(
sizeof(*_tmp659));_tmp659[0]=({struct Cyc_Absyn_Enum_e_struct _tmp65A;_tmp65A.tag=
33;_tmp65A.f1=_tmp640->name;_tmp65A.f2=(struct Cyc_Absyn_Enumdecl*)_tmp63F;
_tmp65A.f3=(struct Cyc_Absyn_Enumfield*)_tmp640;_tmp65A;});_tmp659;}),0),
fail_stmt);goto _LL333;_LL346: if(_tmp631 <= (void*)2)goto _LL348;if(*((int*)_tmp631)
!= 11)goto _LL348;_tmp641=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp631)->f1;
_tmp642=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp631)->f2;_LL347: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp65B=_cycalloc(
sizeof(*_tmp65B));_tmp65B[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp65C;_tmp65C.tag=
34;_tmp65C.f1=_tmp642->name;_tmp65C.f2=(void*)_tmp641;_tmp65C.f3=(struct Cyc_Absyn_Enumfield*)
_tmp642;_tmp65C;});_tmp65B;}),0),fail_stmt);goto _LL333;_LL348: if(_tmp631 <= (void*)
2)goto _LL34A;if(*((int*)_tmp631)!= 6)goto _LL34A;_tmp643=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp631)->f1;_tmp644=((struct Cyc_Absyn_Tunion_p_struct*)_tmp631)->f2;_tmp645=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp631)->f3;if(_tmp645 != 0)goto _LL34A;if(!(!
_tmp643->is_flat))goto _LL34A;_LL349: {struct Cyc_Absyn_Exp*cmp_exp;if(_tmp643->is_xtunion)
cmp_exp=Cyc_Absyn_var_exp(_tmp644->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(
_tmp643,_tmp644->name,0);r=Cyc_Toc_cast_it(Cyc_Absyn_sint_typ,r);}s=Cyc_Toc_if_neq_stmt(
r,cmp_exp,fail_stmt);goto _LL333;}_LL34A: if(_tmp631 <= (void*)2)goto _LL34C;if(*((
int*)_tmp631)!= 4)goto _LL34C;_tmp646=((struct Cyc_Absyn_Pointer_p_struct*)_tmp631)->f1;
_tmp647=*_tmp646;_tmp648=(void*)_tmp647.r;if(_tmp648 <= (void*)2)goto _LL34C;if(*((
int*)_tmp648)!= 6)goto _LL34C;_tmp649=((struct Cyc_Absyn_Tunion_p_struct*)_tmp648)->f1;
_tmp64A=((struct Cyc_Absyn_Tunion_p_struct*)_tmp648)->f2;_tmp64B=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp648)->f3;if(!(_tmp64B != 0  && !_tmp649->is_flat))goto _LL34C;_LL34B: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp64A->name,({const
char*_tmp669="_struct";_tag_dynforward(_tmp669,sizeof(char),
_get_zero_arr_size_char(_tmp669,8));}));struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),r);struct Cyc_List_List*
_tmp65D=_tmp64A->typs;for(0;_tmp64B != 0;(((_tmp64B=_tmp64B->tl,_tmp65D=((struct
Cyc_List_List*)_check_null(_tmp65D))->tl)),++ cnt)){struct Cyc_Absyn_Pat*_tmp65E=(
struct Cyc_Absyn_Pat*)_tmp64B->hd;if((void*)_tmp65E->r == (void*)0)continue;{void*
_tmp65F=(*((struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp65D))->hd)).f2;
struct _tuple1*_tmp660=Cyc_Toc_temp_var();void*_tmp661=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp65E->topt))->v;void*_tmp662=Cyc_Toc_typ_to_c(_tmp661);struct Cyc_Absyn_Exp*
_tmp663=Cyc_Absyn_aggrarrow_exp(rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c(_tmp65F)))_tmp663=Cyc_Toc_cast_it(_tmp662,_tmp663);decls=({
struct Cyc_List_List*_tmp664=_region_malloc(rgn,sizeof(*_tmp664));_tmp664->hd=({
struct _tuple14*_tmp665=_region_malloc(rgn,sizeof(*_tmp665));_tmp665->f1=_tmp660;
_tmp665->f2=_tmp662;_tmp665;});_tmp664->tl=decls;_tmp664;});{struct _tuple13
_tmp666=Cyc_Toc_xlate_pat(nv,rgn,_tmp661,Cyc_Absyn_var_exp(_tmp660,0),_tmp663,
_tmp65E,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp666.f1;decls=
_tmp666.f2;{struct Cyc_Absyn_Stmt*_tmp667=_tmp666.f3;struct Cyc_Absyn_Stmt*_tmp668=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp660,0),_tmp663,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp668,_tmp667,0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(
_tmp649->is_xtunion){struct Cyc_Absyn_Exp*e2=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(
e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(_tmp64A->name,0);test_exp=Cyc_Absyn_neq_exp(
e1,e,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);}else{struct Cyc_Absyn_Exp*e3=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,
Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*
e=Cyc_Toc_tunion_tag(_tmp649,_tmp64A->name,1);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(e1,e,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);if(
tag_fail_stmt != 0){int max_tag=Cyc_Toc_num_void_tags(_tmp649);if(max_tag != 0){
struct Cyc_Absyn_Exp*max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)max_tag,0);
struct Cyc_Absyn_Exp*e5=Cyc_Absyn_lte_exp(r,Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
max_tag_exp),0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(e5,*tag_fail_stmt,
Cyc_Toc_skip_stmt_dl(),0),s,0);}}}goto _LL333;}}_LL34C: if(_tmp631 <= (void*)2)goto
_LL34E;if(*((int*)_tmp631)!= 6)goto _LL34E;_tmp64C=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp631)->f1;_tmp64D=((struct Cyc_Absyn_Tunion_p_struct*)_tmp631)->f2;_tmp64E=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp631)->f3;if(!_tmp64C->is_flat)goto _LL34E;
_LL34D: {struct _tuple1 _tmp66B;struct _dynforward_ptr*_tmp66C;struct _tuple1*
_tmp66A=_tmp64D->name;_tmp66B=*_tmp66A;_tmp66C=_tmp66B.f2;r=Cyc_Absyn_aggrmember_exp(
r,_tmp66C,0);path=Cyc_Absyn_aggrmember_exp(path,_tmp66C,0);s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(Cyc_Absyn_aggrmember_exp(path,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(
_tmp64C,_tmp64D->name,1),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0);{int cnt=1;for(0;
_tmp64E != 0;(_tmp64E=_tmp64E->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp66D=(struct Cyc_Absyn_Pat*)
_tmp64E->hd;if((void*)_tmp66D->r == (void*)0)continue;{struct _tuple1*_tmp66E=Cyc_Toc_temp_var();
void*_tmp66F=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp66D->topt))->v;decls=({
struct Cyc_List_List*_tmp670=_region_malloc(rgn,sizeof(*_tmp670));_tmp670->hd=({
struct _tuple14*_tmp671=_region_malloc(rgn,sizeof(*_tmp671));_tmp671->f1=_tmp66E;
_tmp671->f2=Cyc_Toc_typ_to_c(_tmp66F);_tmp671;});_tmp670->tl=decls;_tmp670;});{
struct _tuple13 _tmp672=Cyc_Toc_xlate_pat(nv,rgn,_tmp66F,Cyc_Absyn_var_exp(_tmp66E,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp66D,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp672.f1;decls=_tmp672.f2;{struct Cyc_Absyn_Stmt*
_tmp673=_tmp672.f3;struct Cyc_Absyn_Stmt*_tmp674=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp66E,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp674,_tmp673,0),0);}}}}goto _LL333;}}_LL34E: if(_tmp631 <= (
void*)2)goto _LL350;if(*((int*)_tmp631)!= 6)goto _LL350;_tmp64F=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp631)->f3;_LL34F: _tmp650=_tmp64F;goto _LL351;_LL350: if(_tmp631 <= (void*)2)goto
_LL352;if(*((int*)_tmp631)!= 3)goto _LL352;_tmp650=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp631)->f1;_LL351: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;_tmp650 != 0;(
_tmp650=_tmp650->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp675=(struct Cyc_Absyn_Pat*)
_tmp650->hd;if((void*)_tmp675->r == (void*)0)continue;{struct _tuple1*_tmp676=Cyc_Toc_temp_var();
void*_tmp677=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp675->topt))->v;decls=({
struct Cyc_List_List*_tmp678=_region_malloc(rgn,sizeof(*_tmp678));_tmp678->hd=({
struct _tuple14*_tmp679=_region_malloc(rgn,sizeof(*_tmp679));_tmp679->f1=_tmp676;
_tmp679->f2=Cyc_Toc_typ_to_c(_tmp677);_tmp679;});_tmp678->tl=decls;_tmp678;});{
struct _tuple13 _tmp67A=Cyc_Toc_xlate_pat(nv,rgn,_tmp677,Cyc_Absyn_var_exp(_tmp676,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp675,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp67A.f1;decls=_tmp67A.f2;{struct Cyc_Absyn_Stmt*
_tmp67B=_tmp67A.f3;struct Cyc_Absyn_Stmt*_tmp67C=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp676,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp67C,_tmp67B,0),0);}}}}goto _LL333;}_LL352: if(_tmp631 <= (
void*)2)goto _LL354;if(*((int*)_tmp631)!= 5)goto _LL354;_tmp651=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp631)->f1;_tmp652=_tmp651.aggr_info;_tmp653=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp631)->f3;_LL353: {struct Cyc_Absyn_Aggrdecl*_tmp67D=Cyc_Absyn_get_known_aggrdecl(
_tmp652);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp653 != 0;_tmp653=_tmp653->tl){struct
_tuple15*_tmp67E=(struct _tuple15*)_tmp653->hd;struct Cyc_Absyn_Pat*_tmp67F=(*
_tmp67E).f2;if((void*)_tmp67F->r == (void*)0)continue;{struct _dynforward_ptr*f;{
void*_tmp680=(void*)((struct Cyc_List_List*)_check_null((*_tmp67E).f1))->hd;
struct _dynforward_ptr*_tmp681;_LL35D: if(*((int*)_tmp680)!= 1)goto _LL35F;_tmp681=((
struct Cyc_Absyn_FieldName_struct*)_tmp680)->f1;_LL35E: f=_tmp681;goto _LL35C;
_LL35F:;_LL360:(int)_throw((void*)Cyc_Toc_Match_error);_LL35C:;}{struct _tuple1*
_tmp682=Cyc_Toc_temp_var();void*_tmp683=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp67F->topt))->v;void*_tmp684=Cyc_Toc_typ_to_c(_tmp683);decls=({struct Cyc_List_List*
_tmp685=_region_malloc(rgn,sizeof(*_tmp685));_tmp685->hd=({struct _tuple14*
_tmp686=_region_malloc(rgn,sizeof(*_tmp686));_tmp686->f1=_tmp682;_tmp686->f2=
_tmp684;_tmp686;});_tmp685->tl=decls;_tmp685;});{struct _tuple13 _tmp687=Cyc_Toc_xlate_pat(
nv,rgn,_tmp683,Cyc_Absyn_var_exp(_tmp682,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp67F,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp687.f1;decls=
_tmp687.f2;{struct Cyc_Absyn_Exp*_tmp688=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star((
void*)((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp67D->impl))->fields,f)))->type))_tmp688=
Cyc_Toc_cast_it(_tmp684,_tmp688);{struct Cyc_Absyn_Stmt*_tmp689=_tmp687.f3;struct
Cyc_Absyn_Stmt*_tmp68A=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp682,0),
_tmp688,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp68A,_tmp689,0),0);}}}}}}
goto _LL333;}_LL354: if(_tmp631 <= (void*)2)goto _LL356;if(*((int*)_tmp631)!= 4)goto
_LL356;_tmp654=((struct Cyc_Absyn_Pointer_p_struct*)_tmp631)->f1;_LL355: {struct
_tuple1*_tmp68B=Cyc_Toc_temp_var();void*_tmp68C=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp654->topt))->v;decls=({struct Cyc_List_List*_tmp68D=
_region_malloc(rgn,sizeof(*_tmp68D));_tmp68D->hd=({struct _tuple14*_tmp68E=
_region_malloc(rgn,sizeof(*_tmp68E));_tmp68E->f1=_tmp68B;_tmp68E->f2=Cyc_Toc_typ_to_c(
_tmp68C);_tmp68E;});_tmp68D->tl=decls;_tmp68D;});{struct _tuple13 _tmp68F=Cyc_Toc_xlate_pat(
nv,rgn,_tmp68C,Cyc_Absyn_var_exp(_tmp68B,0),Cyc_Absyn_deref_exp(path,0),_tmp654,(
struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp68F.f1;decls=_tmp68F.f2;{
struct Cyc_Absyn_Stmt*_tmp690=_tmp68F.f3;struct Cyc_Absyn_Stmt*_tmp691=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp68B,0),Cyc_Absyn_deref_exp(r,0),0),
_tmp690,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp691,0);else{s=_tmp691;}goto _LL333;}}}_LL356: if(_tmp631 <= (void*)2)goto
_LL358;if(*((int*)_tmp631)!= 12)goto _LL358;_LL357:({void*_tmp692=0;((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp693="unknownid pat";_tag_dynforward(_tmp693,sizeof(char),
_get_zero_arr_size_char(_tmp693,14));}),_tag_dynforward(_tmp692,sizeof(void*),0));});
_LL358: if(_tmp631 <= (void*)2)goto _LL35A;if(*((int*)_tmp631)!= 13)goto _LL35A;
_LL359:({void*_tmp694=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr
ap))Cyc_Toc_toc_impos)(({const char*_tmp695="unknowncall pat";_tag_dynforward(
_tmp695,sizeof(char),_get_zero_arr_size_char(_tmp695,16));}),_tag_dynforward(
_tmp694,sizeof(void*),0));});_LL35A: if(_tmp631 <= (void*)2)goto _LL333;if(*((int*)
_tmp631)!= 14)goto _LL333;_LL35B:({void*_tmp696=0;((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp697="exp pat";
_tag_dynforward(_tmp697,sizeof(char),_get_zero_arr_size_char(_tmp697,8));}),
_tag_dynforward(_tmp696,sizeof(void*),0));});_LL333:;}return({struct _tuple13
_tmp698;_tmp698.f1=nv;_tmp698.f2=decls;_tmp698.f3=s;_tmp698;});}struct _tuple16{
struct _dynforward_ptr*f1;struct _dynforward_ptr*f2;struct Cyc_Absyn_Switch_clause*
f3;};static struct _tuple16*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){return({struct _tuple16*_tmp699=_region_malloc(r,sizeof(*_tmp699));_tmp699->f1=
Cyc_Toc_fresh_label();_tmp699->f2=Cyc_Toc_fresh_label();_tmp699->f3=sc;_tmp699;});}
static int Cyc_Toc_is_mixed_tunion(void*t){{void*_tmp69A=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_TunionInfo _tmp69B;union Cyc_Absyn_TunionInfoU_union _tmp69C;struct
Cyc_Absyn_Tuniondecl**_tmp69D;struct Cyc_Absyn_Tuniondecl*_tmp69E;_LL362: if(
_tmp69A <= (void*)4)goto _LL364;if(*((int*)_tmp69A)!= 2)goto _LL364;_tmp69B=((
struct Cyc_Absyn_TunionType_struct*)_tmp69A)->f1;_tmp69C=_tmp69B.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp69A)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL364;_tmp69D=(_tmp69C.KnownTunion).f1;_tmp69E=*_tmp69D;if(!(!_tmp69E->is_flat))
goto _LL364;_LL363: {int seen_novalue=0;int seen_value=0;{struct Cyc_List_List*
_tmp69F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp69E->fields))->v;
for(0;(unsigned int)_tmp69F;_tmp69F=_tmp69F->tl){if(((struct Cyc_Absyn_Tunionfield*)
_tmp69F->hd)->typs == 0)seen_value=1;else{seen_novalue=1;}if(seen_value  && 
seen_novalue)return 1;}}goto _LL361;}_LL364:;_LL365: goto _LL361;_LL361:;}return 0;}
static int Cyc_Toc_no_tag_test(struct Cyc_Absyn_Pat*p){void*_tmp6A0=(void*)p->r;
struct Cyc_Absyn_Pat*_tmp6A1;struct Cyc_Absyn_Pat*_tmp6A2;struct Cyc_Absyn_Pat
_tmp6A3;void*_tmp6A4;struct Cyc_Absyn_Tuniondecl*_tmp6A5;struct Cyc_Absyn_Tunionfield*
_tmp6A6;struct Cyc_List_List*_tmp6A7;_LL367: if(_tmp6A0 <= (void*)2)goto _LL36B;if(*((
int*)_tmp6A0)!= 0)goto _LL369;_tmp6A1=((struct Cyc_Absyn_Var_p_struct*)_tmp6A0)->f2;
_LL368: return Cyc_Toc_no_tag_test(_tmp6A1);_LL369: if(*((int*)_tmp6A0)!= 4)goto
_LL36B;_tmp6A2=((struct Cyc_Absyn_Pointer_p_struct*)_tmp6A0)->f1;_tmp6A3=*_tmp6A2;
_tmp6A4=(void*)_tmp6A3.r;if(_tmp6A4 <= (void*)2)goto _LL36B;if(*((int*)_tmp6A4)!= 
6)goto _LL36B;_tmp6A5=((struct Cyc_Absyn_Tunion_p_struct*)_tmp6A4)->f1;_tmp6A6=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp6A4)->f2;_tmp6A7=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp6A4)->f3;if(!(_tmp6A7 != 0  && !_tmp6A5->is_flat))goto _LL36B;_LL36A: return 0;
_LL36B:;_LL36C: return 1;_LL366:;}static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
Cyc_Toc_exp_to_c(nv,e);{void*_tmp6A8=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
int leave_as_switch;{void*_tmp6A9=Cyc_Tcutil_compress(_tmp6A8);_LL36E: if(_tmp6A9
<= (void*)4)goto _LL372;if(*((int*)_tmp6A9)!= 5)goto _LL370;_LL36F: goto _LL371;
_LL370: if(*((int*)_tmp6A9)!= 12)goto _LL372;_LL371: leave_as_switch=1;goto _LL36D;
_LL372:;_LL373: leave_as_switch=0;goto _LL36D;_LL36D:;}{struct Cyc_List_List*
_tmp6AA=scs;for(0;_tmp6AA != 0;_tmp6AA=_tmp6AA->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp6AA->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp6AA->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dynforward_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp6AB=scs;for(0;_tmp6AB != 0;_tmp6AB=_tmp6AB->tl){struct Cyc_Absyn_Stmt*
_tmp6AC=((struct Cyc_Absyn_Switch_clause*)_tmp6AB->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp6AB->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp6AC,0);next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp6AD=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6AD;
_push_region(rgn);Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),
_tmp6AC);;_pop_region(rgn);}}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();struct
Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dynforward_ptr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle
_tmp6AE=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6AE;_push_region(rgn);{
struct Cyc_Toc_Env*_tmp6AF=Cyc_Toc_share_env(rgn,nv);struct Cyc_List_List*lscs=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple16*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);struct Cyc_List_List*
test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*decls=0;int is_tunion=
Cyc_Toc_is_mixed_tunion(_tmp6A8);int needs_tag_test=is_tunion;{struct Cyc_List_List*
_tmp6B0=lscs;for(0;_tmp6B0 != 0;_tmp6B0=_tmp6B0->tl){struct Cyc_Absyn_Switch_clause*
sc=(*((struct _tuple16*)_tmp6B0->hd)).f3;struct _dynforward_ptr*fail_lab=_tmp6B0->tl
== 0?end_l:(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmp6B0->tl))->hd)).f1;
struct Cyc_Absyn_Stmt**tag_fail_stmt=0;if(needs_tag_test  && !Cyc_Toc_no_tag_test(
sc->pattern)){{struct Cyc_List_List*_tmp6B1=_tmp6B0->tl;for(0;(unsigned int)
_tmp6B1;_tmp6B1=_tmp6B1->tl){if(Cyc_Toc_no_tag_test(((*((struct _tuple16*)_tmp6B1->hd)).f3)->pattern)){
tag_fail_stmt=({struct Cyc_Absyn_Stmt**_tmp6B2=_region_malloc(rgn,sizeof(*_tmp6B2));
_tmp6B2[0]=Cyc_Absyn_goto_stmt((*((struct _tuple16*)_tmp6B1->hd)).f1,0);_tmp6B2;});
needs_tag_test=0;break;}}}if(tag_fail_stmt == 0)tag_fail_stmt=({struct Cyc_Absyn_Stmt**
_tmp6B3=_region_malloc(rgn,sizeof(*_tmp6B3));_tmp6B3[0]=Cyc_Absyn_goto_stmt(
fail_lab,0);_tmp6B3;});}{struct Cyc_Toc_Env*_tmp6B5;struct Cyc_List_List*_tmp6B6;
struct Cyc_Absyn_Stmt*_tmp6B7;struct _tuple13 _tmp6B4=Cyc_Toc_xlate_pat(_tmp6AF,rgn,
_tmp6A8,r,path,sc->pattern,tag_fail_stmt,Cyc_Absyn_goto_stmt(fail_lab,0),decls);
_tmp6B5=_tmp6B4.f1;_tmp6B6=_tmp6B4.f2;_tmp6B7=_tmp6B4.f3;if(is_tunion  && Cyc_Toc_no_tag_test(
sc->pattern))needs_tag_test=1;if(sc->where_clause != 0){struct Cyc_Absyn_Exp*
_tmp6B8=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);Cyc_Toc_exp_to_c(
_tmp6B5,_tmp6B8);_tmp6B7=Cyc_Absyn_seq_stmt(_tmp6B7,Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp((void*)11,_tmp6B8,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),
0),0);}decls=_tmp6B6;nvs=({struct Cyc_List_List*_tmp6B9=_region_malloc(rgn,
sizeof(*_tmp6B9));_tmp6B9->hd=_tmp6B5;_tmp6B9->tl=nvs;_tmp6B9;});test_stmts=({
struct Cyc_List_List*_tmp6BA=_region_malloc(rgn,sizeof(*_tmp6BA));_tmp6BA->hd=
_tmp6B7;_tmp6BA->tl=test_stmts;_tmp6BA;});}}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple16 _tmp6BC;struct _dynforward_ptr*_tmp6BD;struct _dynforward_ptr*
_tmp6BE;struct Cyc_Absyn_Switch_clause*_tmp6BF;struct _tuple16*_tmp6BB=(struct
_tuple16*)lscs->hd;_tmp6BC=*_tmp6BB;_tmp6BD=_tmp6BC.f1;_tmp6BE=_tmp6BC.f2;
_tmp6BF=_tmp6BC.f3;{struct Cyc_Toc_Env*_tmp6C0=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)
_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp6BF->body;{struct _RegionHandle
_tmp6C1=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp6C1;_push_region(rgn2);
if(lscs->tl != 0){struct _tuple16 _tmp6C3;struct _dynforward_ptr*_tmp6C4;struct Cyc_Absyn_Switch_clause*
_tmp6C5;struct _tuple16*_tmp6C2=(struct _tuple16*)((struct Cyc_List_List*)
_check_null(lscs->tl))->hd;_tmp6C3=*_tmp6C2;_tmp6C4=_tmp6C3.f2;_tmp6C5=_tmp6C3.f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp6C0,end_l,_tmp6C4,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6C5->pat_vars))->v,(
struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp6C0,end_l),s);};
_pop_region(rgn2);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp6BD,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp6BE,s,0),0);stmts=({struct Cyc_List_List*_tmp6C6=_region_malloc(rgn,sizeof(*
_tmp6C6));_tmp6C6->hd=s;_tmp6C6->tl=stmts;_tmp6C6;});}}{struct Cyc_Absyn_Stmt*res=
Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),
0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple14 _tmp6C8;struct _tuple1*
_tmp6C9;void*_tmp6CA;struct _tuple14*_tmp6C7=(struct _tuple14*)decls->hd;_tmp6C8=*
_tmp6C7;_tmp6C9=_tmp6C8.f1;_tmp6CA=_tmp6C8.f2;res=Cyc_Absyn_declare_stmt(_tmp6C9,
_tmp6CA,0,res,0);}(void*)(whole_s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(v,
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(struct
Cyc_Absyn_Exp*)e,res,0))->r));}}};_pop_region(rgn);}}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,
int cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp6CB=
_cycalloc(sizeof(*_tmp6CB));_tmp6CB->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),
0);_tmp6CB->tl=0;_tmp6CB;}),0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*
s){if(n > 0)(void*)(s->r=(void*)Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),
Cyc_Absyn_new_stmt((void*)s->r,0)));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp6CC=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp6CD;struct Cyc_Absyn_Stmt*_tmp6CE;struct Cyc_Absyn_Stmt*_tmp6CF;struct Cyc_Absyn_Exp*
_tmp6D0;struct Cyc_Absyn_Exp*_tmp6D1;struct Cyc_Absyn_Stmt*_tmp6D2;struct Cyc_Absyn_Stmt*
_tmp6D3;struct _tuple3 _tmp6D4;struct Cyc_Absyn_Exp*_tmp6D5;struct Cyc_Absyn_Stmt*
_tmp6D6;struct Cyc_Absyn_Stmt*_tmp6D7;struct Cyc_Absyn_Stmt*_tmp6D8;struct Cyc_Absyn_Stmt*
_tmp6D9;struct Cyc_Absyn_Exp*_tmp6DA;struct _tuple3 _tmp6DB;struct Cyc_Absyn_Exp*
_tmp6DC;struct _tuple3 _tmp6DD;struct Cyc_Absyn_Exp*_tmp6DE;struct Cyc_Absyn_Stmt*
_tmp6DF;struct Cyc_Absyn_Exp*_tmp6E0;struct Cyc_List_List*_tmp6E1;struct Cyc_List_List*
_tmp6E2;struct Cyc_Absyn_Switch_clause**_tmp6E3;struct Cyc_Absyn_Decl*_tmp6E4;
struct Cyc_Absyn_Stmt*_tmp6E5;struct _dynforward_ptr*_tmp6E6;struct Cyc_Absyn_Stmt*
_tmp6E7;struct Cyc_Absyn_Stmt*_tmp6E8;struct _tuple3 _tmp6E9;struct Cyc_Absyn_Exp*
_tmp6EA;struct Cyc_Absyn_Stmt*_tmp6EB;struct Cyc_List_List*_tmp6EC;struct Cyc_Absyn_Tvar*
_tmp6ED;struct Cyc_Absyn_Vardecl*_tmp6EE;int _tmp6EF;struct Cyc_Absyn_Exp*_tmp6F0;
struct Cyc_Absyn_Stmt*_tmp6F1;struct Cyc_Absyn_Exp*_tmp6F2;struct Cyc_Absyn_Exp*
_tmp6F3;struct Cyc_Absyn_Tvar*_tmp6F4;struct Cyc_Absyn_Vardecl*_tmp6F5;struct Cyc_Absyn_Stmt*
_tmp6F6;_LL375: if((int)_tmp6CC != 0)goto _LL377;_LL376: return;_LL377: if(_tmp6CC <= (
void*)1)goto _LL379;if(*((int*)_tmp6CC)!= 0)goto _LL379;_tmp6CD=((struct Cyc_Absyn_Exp_s_struct*)
_tmp6CC)->f1;_LL378: Cyc_Toc_exp_to_c(nv,_tmp6CD);return;_LL379: if(_tmp6CC <= (
void*)1)goto _LL37B;if(*((int*)_tmp6CC)!= 1)goto _LL37B;_tmp6CE=((struct Cyc_Absyn_Seq_s_struct*)
_tmp6CC)->f1;_tmp6CF=((struct Cyc_Absyn_Seq_s_struct*)_tmp6CC)->f2;_LL37A: Cyc_Toc_stmt_to_c(
nv,_tmp6CE);s=_tmp6CF;continue;_LL37B: if(_tmp6CC <= (void*)1)goto _LL37D;if(*((int*)
_tmp6CC)!= 2)goto _LL37D;_tmp6D0=((struct Cyc_Absyn_Return_s_struct*)_tmp6CC)->f1;
_LL37C: {struct Cyc_Core_Opt*topt=0;if(_tmp6D0 != 0){topt=({struct Cyc_Core_Opt*
_tmp6F7=_cycalloc(sizeof(*_tmp6F7));_tmp6F7->v=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp6D0->topt))->v);_tmp6F7;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp6D0);}if(s->try_depth > 0){if(topt != 0){struct
_tuple1*_tmp6F8=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp6F9=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp6F8,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp6F8,(void*)topt->v,_tmp6D0,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp6F9,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL37D: if(_tmp6CC <= (void*)1)goto _LL37F;if(*((int*)_tmp6CC)!= 3)goto
_LL37F;_tmp6D1=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp6CC)->f1;_tmp6D2=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp6CC)->f2;_tmp6D3=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp6CC)->f3;_LL37E: Cyc_Toc_exp_to_c(nv,_tmp6D1);Cyc_Toc_stmt_to_c(nv,_tmp6D2);s=
_tmp6D3;continue;_LL37F: if(_tmp6CC <= (void*)1)goto _LL381;if(*((int*)_tmp6CC)!= 4)
goto _LL381;_tmp6D4=((struct Cyc_Absyn_While_s_struct*)_tmp6CC)->f1;_tmp6D5=
_tmp6D4.f1;_tmp6D6=((struct Cyc_Absyn_While_s_struct*)_tmp6CC)->f2;_LL380: Cyc_Toc_exp_to_c(
nv,_tmp6D5);{struct _RegionHandle _tmp6FA=_new_region("temp");struct _RegionHandle*
temp=& _tmp6FA;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp6D6);;_pop_region(temp);}return;_LL381: if(_tmp6CC <= (void*)1)goto _LL383;if(*((
int*)_tmp6CC)!= 5)goto _LL383;_tmp6D7=((struct Cyc_Absyn_Break_s_struct*)_tmp6CC)->f1;
_LL382: {struct Cyc_Toc_Env _tmp6FC;struct _dynforward_ptr**_tmp6FD;struct Cyc_Toc_Env*
_tmp6FB=nv;_tmp6FC=*_tmp6FB;_tmp6FD=_tmp6FC.break_lab;if(_tmp6FD != 0)(void*)(s->r=(
void*)Cyc_Toc_goto_stmt_r(*_tmp6FD,0));{int dest_depth=_tmp6D7 == 0?0: _tmp6D7->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;}}_LL383: if(_tmp6CC <= (
void*)1)goto _LL385;if(*((int*)_tmp6CC)!= 6)goto _LL385;_tmp6D8=((struct Cyc_Absyn_Continue_s_struct*)
_tmp6CC)->f1;_LL384: {struct Cyc_Toc_Env _tmp6FF;struct _dynforward_ptr**_tmp700;
struct Cyc_Toc_Env*_tmp6FE=nv;_tmp6FF=*_tmp6FE;_tmp700=_tmp6FF.continue_lab;if(
_tmp700 != 0)(void*)(s->r=(void*)Cyc_Toc_goto_stmt_r(*_tmp700,0));_tmp6D9=_tmp6D8;
goto _LL386;}_LL385: if(_tmp6CC <= (void*)1)goto _LL387;if(*((int*)_tmp6CC)!= 7)goto
_LL387;_tmp6D9=((struct Cyc_Absyn_Goto_s_struct*)_tmp6CC)->f2;_LL386: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp6D9))->try_depth,s);
return;_LL387: if(_tmp6CC <= (void*)1)goto _LL389;if(*((int*)_tmp6CC)!= 8)goto
_LL389;_tmp6DA=((struct Cyc_Absyn_For_s_struct*)_tmp6CC)->f1;_tmp6DB=((struct Cyc_Absyn_For_s_struct*)
_tmp6CC)->f2;_tmp6DC=_tmp6DB.f1;_tmp6DD=((struct Cyc_Absyn_For_s_struct*)_tmp6CC)->f3;
_tmp6DE=_tmp6DD.f1;_tmp6DF=((struct Cyc_Absyn_For_s_struct*)_tmp6CC)->f4;_LL388:
Cyc_Toc_exp_to_c(nv,_tmp6DA);Cyc_Toc_exp_to_c(nv,_tmp6DC);Cyc_Toc_exp_to_c(nv,
_tmp6DE);{struct _RegionHandle _tmp701=_new_region("temp");struct _RegionHandle*
temp=& _tmp701;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp6DF);;_pop_region(temp);}return;_LL389: if(_tmp6CC <= (void*)1)goto _LL38B;if(*((
int*)_tmp6CC)!= 9)goto _LL38B;_tmp6E0=((struct Cyc_Absyn_Switch_s_struct*)_tmp6CC)->f1;
_tmp6E1=((struct Cyc_Absyn_Switch_s_struct*)_tmp6CC)->f2;_LL38A: Cyc_Toc_xlate_switch(
nv,s,_tmp6E0,_tmp6E1);return;_LL38B: if(_tmp6CC <= (void*)1)goto _LL38D;if(*((int*)
_tmp6CC)!= 10)goto _LL38D;_tmp6E2=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp6CC)->f1;
_tmp6E3=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp6CC)->f2;_LL38C: {struct Cyc_Toc_Env
_tmp703;struct Cyc_Toc_FallthruInfo*_tmp704;struct Cyc_Toc_Env*_tmp702=nv;_tmp703=*
_tmp702;_tmp704=_tmp703.fallthru_info;if(_tmp704 == 0)({void*_tmp705=0;((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp706="fallthru in unexpected place";_tag_dynforward(_tmp706,sizeof(char),
_get_zero_arr_size_char(_tmp706,29));}),_tag_dynforward(_tmp705,sizeof(void*),0));});{
struct _dynforward_ptr*_tmp708;struct Cyc_List_List*_tmp709;struct Cyc_Dict_Dict
_tmp70A;struct Cyc_Toc_FallthruInfo _tmp707=*_tmp704;_tmp708=_tmp707.label;_tmp709=
_tmp707.binders;_tmp70A=_tmp707.next_case_env;{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(
_tmp708,0);Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)
_check_null(_tmp6E3)))->body)->try_depth,s2);{struct Cyc_List_List*_tmp70B=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp709);struct Cyc_List_List*
_tmp70C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp6E2);
for(0;_tmp70B != 0;(_tmp70B=_tmp70B->tl,_tmp70C=_tmp70C->tl)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp70C))->hd);s2=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct
_tuple1*k))Cyc_Dict_lookup)(_tmp70A,(struct _tuple1*)_tmp70B->hd),(struct Cyc_Absyn_Exp*)
_tmp70C->hd,0),s2,0);}(void*)(s->r=(void*)((void*)s2->r));return;}}}}_LL38D: if(
_tmp6CC <= (void*)1)goto _LL38F;if(*((int*)_tmp6CC)!= 11)goto _LL38F;_tmp6E4=((
struct Cyc_Absyn_Decl_s_struct*)_tmp6CC)->f1;_tmp6E5=((struct Cyc_Absyn_Decl_s_struct*)
_tmp6CC)->f2;_LL38E:{void*_tmp70D=(void*)_tmp6E4->r;struct Cyc_Absyn_Vardecl*
_tmp70E;struct Cyc_Absyn_Pat*_tmp70F;struct Cyc_Absyn_Exp*_tmp710;struct Cyc_List_List*
_tmp711;struct Cyc_Absyn_Fndecl*_tmp712;_LL39C: if(_tmp70D <= (void*)2)goto _LL3A4;
if(*((int*)_tmp70D)!= 0)goto _LL39E;_tmp70E=((struct Cyc_Absyn_Var_d_struct*)
_tmp70D)->f1;_LL39D:{struct _RegionHandle _tmp713=_new_region("temp");struct
_RegionHandle*temp=& _tmp713;_push_region(temp);{struct Cyc_Toc_Env*_tmp714=Cyc_Toc_add_varmap(
temp,nv,_tmp70E->name,Cyc_Absyn_varb_exp(_tmp70E->name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp715=_cycalloc(sizeof(*_tmp715));_tmp715[0]=({struct Cyc_Absyn_Local_b_struct
_tmp716;_tmp716.tag=3;_tmp716.f1=_tmp70E;_tmp716;});_tmp715;}),0));Cyc_Toc_local_decl_to_c(
_tmp714,_tmp714,_tmp70E,_tmp6E5);};_pop_region(temp);}goto _LL39B;_LL39E: if(*((
int*)_tmp70D)!= 2)goto _LL3A0;_tmp70F=((struct Cyc_Absyn_Let_d_struct*)_tmp70D)->f1;
_tmp710=((struct Cyc_Absyn_Let_d_struct*)_tmp70D)->f3;_LL39F:{void*_tmp717=(void*)
_tmp70F->r;struct Cyc_Absyn_Vardecl*_tmp718;struct Cyc_Absyn_Pat*_tmp719;struct Cyc_Absyn_Pat
_tmp71A;void*_tmp71B;_LL3A7: if(_tmp717 <= (void*)2)goto _LL3A9;if(*((int*)_tmp717)
!= 0)goto _LL3A9;_tmp718=((struct Cyc_Absyn_Var_p_struct*)_tmp717)->f1;_tmp719=((
struct Cyc_Absyn_Var_p_struct*)_tmp717)->f2;_tmp71A=*_tmp719;_tmp71B=(void*)
_tmp71A.r;if((int)_tmp71B != 0)goto _LL3A9;_LL3A8: {struct _tuple1*old_name=_tmp718->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();_tmp718->name=new_name;_tmp718->initializer=(
struct Cyc_Absyn_Exp*)_tmp710;(void*)(_tmp6E4->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp71C=_cycalloc(sizeof(*_tmp71C));_tmp71C[0]=({struct Cyc_Absyn_Var_d_struct
_tmp71D;_tmp71D.tag=0;_tmp71D.f1=_tmp718;_tmp71D;});_tmp71C;})));{struct
_RegionHandle _tmp71E=_new_region("temp");struct _RegionHandle*temp=& _tmp71E;
_push_region(temp);{struct Cyc_Toc_Env*_tmp71F=Cyc_Toc_add_varmap(temp,nv,
old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp720=_cycalloc(sizeof(*_tmp720));_tmp720[0]=({struct Cyc_Absyn_Local_b_struct
_tmp721;_tmp721.tag=3;_tmp721.f1=_tmp718;_tmp721;});_tmp720;}),0));Cyc_Toc_local_decl_to_c(
_tmp71F,nv,_tmp718,_tmp6E5);};_pop_region(temp);}goto _LL3A6;}_LL3A9:;_LL3AA:(
void*)(s->r=(void*)((void*)(Cyc_Toc_letdecl_to_c(nv,_tmp70F,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp710->topt))->v,_tmp710,_tmp6E5))->r));goto _LL3A6;_LL3A6:;}goto
_LL39B;_LL3A0: if(*((int*)_tmp70D)!= 3)goto _LL3A2;_tmp711=((struct Cyc_Absyn_Letv_d_struct*)
_tmp70D)->f1;_LL3A1: {struct Cyc_List_List*_tmp722=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp711);if(_tmp722 == 0)({void*_tmp723=0;((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp724="empty Letv_d";_tag_dynforward(_tmp724,sizeof(char),
_get_zero_arr_size_char(_tmp724,13));}),_tag_dynforward(_tmp723,sizeof(void*),0));});(
void*)(_tmp6E4->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp725=
_cycalloc(sizeof(*_tmp725));_tmp725[0]=({struct Cyc_Absyn_Var_d_struct _tmp726;
_tmp726.tag=0;_tmp726.f1=(struct Cyc_Absyn_Vardecl*)_tmp722->hd;_tmp726;});
_tmp725;})));_tmp722=_tmp722->tl;for(0;_tmp722 != 0;_tmp722=_tmp722->tl){struct
Cyc_Absyn_Decl*_tmp727=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp728=_cycalloc(sizeof(*_tmp728));_tmp728[0]=({struct Cyc_Absyn_Var_d_struct
_tmp729;_tmp729.tag=0;_tmp729.f1=(struct Cyc_Absyn_Vardecl*)_tmp722->hd;_tmp729;});
_tmp728;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(_tmp727,Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto _LL39B;}_LL3A2: if(*((int*)
_tmp70D)!= 1)goto _LL3A4;_tmp712=((struct Cyc_Absyn_Fn_d_struct*)_tmp70D)->f1;
_LL3A3: {struct _tuple1*_tmp72A=_tmp712->name;{struct _RegionHandle _tmp72B=
_new_region("temp");struct _RegionHandle*temp=& _tmp72B;_push_region(temp);{struct
Cyc_Toc_Env*_tmp72C=Cyc_Toc_add_varmap(temp,nv,_tmp712->name,Cyc_Absyn_var_exp(
_tmp72A,0));Cyc_Toc_fndecl_to_c(_tmp72C,_tmp712,0);Cyc_Toc_stmt_to_c(_tmp72C,
_tmp6E5);};_pop_region(temp);}goto _LL39B;}_LL3A4:;_LL3A5:({void*_tmp72D=0;((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp72E="bad nested declaration within function";_tag_dynforward(_tmp72E,sizeof(
char),_get_zero_arr_size_char(_tmp72E,39));}),_tag_dynforward(_tmp72D,sizeof(
void*),0));});_LL39B:;}return;_LL38F: if(_tmp6CC <= (void*)1)goto _LL391;if(*((int*)
_tmp6CC)!= 12)goto _LL391;_tmp6E6=((struct Cyc_Absyn_Label_s_struct*)_tmp6CC)->f1;
_tmp6E7=((struct Cyc_Absyn_Label_s_struct*)_tmp6CC)->f2;_LL390: s=_tmp6E7;
continue;_LL391: if(_tmp6CC <= (void*)1)goto _LL393;if(*((int*)_tmp6CC)!= 13)goto
_LL393;_tmp6E8=((struct Cyc_Absyn_Do_s_struct*)_tmp6CC)->f1;_tmp6E9=((struct Cyc_Absyn_Do_s_struct*)
_tmp6CC)->f2;_tmp6EA=_tmp6E9.f1;_LL392:{struct _RegionHandle _tmp72F=_new_region("temp");
struct _RegionHandle*temp=& _tmp72F;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
temp,nv),_tmp6E8);Cyc_Toc_exp_to_c(nv,_tmp6EA);;_pop_region(temp);}return;_LL393:
if(_tmp6CC <= (void*)1)goto _LL395;if(*((int*)_tmp6CC)!= 14)goto _LL395;_tmp6EB=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp6CC)->f1;_tmp6EC=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp6CC)->f2;_LL394: {struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=
Cyc_Toc_temp_var();struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
h_exp=Cyc_Absyn_var_exp(h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(
e_var,0);struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ);void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
e_exp->topt=({struct Cyc_Core_Opt*_tmp730=_cycalloc(sizeof(*_tmp730));_tmp730->v=(
void*)e_typ;_tmp730;});{struct _RegionHandle _tmp731=_new_region("temp");struct
_RegionHandle*temp=& _tmp731;_push_region(temp);{struct Cyc_Toc_Env*_tmp732=Cyc_Toc_add_varmap(
temp,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp732,_tmp6EB);{struct Cyc_Absyn_Stmt*
_tmp733=Cyc_Absyn_seq_stmt(_tmp6EB,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple1*_tmp734=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp735=
Cyc_Absyn_var_exp(_tmp734,0);struct Cyc_Absyn_Vardecl*_tmp736=Cyc_Absyn_new_vardecl(
_tmp734,Cyc_Absyn_exn_typ,0);_tmp735->topt=({struct Cyc_Core_Opt*_tmp737=
_cycalloc(sizeof(*_tmp737));_tmp737->v=(void*)Cyc_Absyn_exn_typ;_tmp737;});{
struct Cyc_Absyn_Pat*_tmp738=({struct Cyc_Absyn_Pat*_tmp748=_cycalloc(sizeof(*
_tmp748));_tmp748->r=(void*)((void*)({struct Cyc_Absyn_Var_p_struct*_tmp74A=
_cycalloc(sizeof(*_tmp74A));_tmp74A[0]=({struct Cyc_Absyn_Var_p_struct _tmp74B;
_tmp74B.tag=0;_tmp74B.f1=_tmp736;_tmp74B.f2=({struct Cyc_Absyn_Pat*_tmp74C=
_cycalloc(sizeof(*_tmp74C));_tmp74C->r=(void*)((void*)0);_tmp74C->topt=({struct
Cyc_Core_Opt*_tmp74D=_cycalloc(sizeof(*_tmp74D));_tmp74D->v=(void*)Cyc_Absyn_exn_typ;
_tmp74D;});_tmp74C->loc=0;_tmp74C;});_tmp74B;});_tmp74A;}));_tmp748->topt=({
struct Cyc_Core_Opt*_tmp749=_cycalloc(sizeof(*_tmp749));_tmp749->v=(void*)Cyc_Absyn_exn_typ;
_tmp749;});_tmp748->loc=0;_tmp748;});struct Cyc_Absyn_Exp*_tmp739=Cyc_Absyn_throw_exp(
_tmp735,0);_tmp739->topt=({struct Cyc_Core_Opt*_tmp73A=_cycalloc(sizeof(*_tmp73A));
_tmp73A->v=(void*)((void*)0);_tmp73A;});{struct Cyc_Absyn_Stmt*_tmp73B=Cyc_Absyn_exp_stmt(
_tmp739,0);struct Cyc_Absyn_Switch_clause*_tmp73C=({struct Cyc_Absyn_Switch_clause*
_tmp745=_cycalloc(sizeof(*_tmp745));_tmp745->pattern=_tmp738;_tmp745->pat_vars=({
struct Cyc_Core_Opt*_tmp746=_cycalloc(sizeof(*_tmp746));_tmp746->v=({struct Cyc_List_List*
_tmp747=_cycalloc(sizeof(*_tmp747));_tmp747->hd=_tmp736;_tmp747->tl=0;_tmp747;});
_tmp746;});_tmp745->where_clause=0;_tmp745->body=_tmp73B;_tmp745->loc=0;_tmp745;});
struct Cyc_Absyn_Stmt*_tmp73D=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp6EC,({struct
Cyc_List_List*_tmp744=_cycalloc(sizeof(*_tmp744));_tmp744->hd=_tmp73C;_tmp744->tl=
0;_tmp744;})),0);Cyc_Toc_stmt_to_c(_tmp732,_tmp73D);{struct Cyc_Absyn_Exp*_tmp73E=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp743=_cycalloc(
sizeof(*_tmp743));_tmp743->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,
0);_tmp743->tl=0;_tmp743;}),0);struct Cyc_Absyn_Stmt*_tmp73F=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp742=
_cycalloc(sizeof(*_tmp742));_tmp742->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp742->tl=
0;_tmp742;}),0),0);struct Cyc_Absyn_Exp*_tmp740=Cyc_Absyn_int_exp((void*)0,0,0);
struct Cyc_Absyn_Exp*_tmp741=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((
void*)(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp73F,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp740,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp73E,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp741,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)
11,was_thrown_exp,0),_tmp733,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp73D,0),0),0),0),0),0))->r));}}}}};
_pop_region(temp);}return;}_LL395: if(_tmp6CC <= (void*)1)goto _LL397;if(*((int*)
_tmp6CC)!= 15)goto _LL397;_tmp6ED=((struct Cyc_Absyn_Region_s_struct*)_tmp6CC)->f1;
_tmp6EE=((struct Cyc_Absyn_Region_s_struct*)_tmp6CC)->f2;_tmp6EF=((struct Cyc_Absyn_Region_s_struct*)
_tmp6CC)->f3;_tmp6F0=((struct Cyc_Absyn_Region_s_struct*)_tmp6CC)->f4;_tmp6F1=((
struct Cyc_Absyn_Region_s_struct*)_tmp6CC)->f5;_LL396: {void*rh_struct_typ=Cyc_Absyn_strct(
Cyc_Toc__RegionHandle_sp);void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(
rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple1*rh_var=Cyc_Toc_temp_var();struct
_tuple1*x_var=_tmp6EE->name;struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,
0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);{struct _RegionHandle
_tmp74E=_new_region("temp");struct _RegionHandle*temp=& _tmp74E;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp6F1);;_pop_region(
temp);}if(Cyc_Absyn_no_regions)(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp6F1,0))->r));
else{if(_tmp6F0 == 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(rh_var,
rh_struct_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({
struct Cyc_List_List*_tmp74F=_cycalloc(sizeof(*_tmp74F));_tmp74F->hd=Cyc_Absyn_string_exp(
Cyc_Absynpp_qvar2string(x_var),0);_tmp74F->tl=0;_tmp74F;}),0),Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp*_tmp750[1];_tmp750[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp750,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),Cyc_Absyn_seq_stmt(_tmp6F1,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__pop_region_e,({struct Cyc_Absyn_Exp*_tmp751[1];_tmp751[0]=x_exp;((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp751,
sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),0),0),0),0))->r));else{Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp6F0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,({
struct Cyc_Absyn_Exp*_tmp752[2];_tmp752[1]=(struct Cyc_Absyn_Exp*)_tmp6F0;_tmp752[
0]=Cyc_Absyn_address_exp(rh_exp,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp752,sizeof(struct Cyc_Absyn_Exp*),
2));}),0),Cyc_Absyn_seq_stmt(_tmp6F1,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,({
struct Cyc_Absyn_Exp*_tmp753[1];_tmp753[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp753,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),0),0),0))->r));}}return;}_LL397: if(_tmp6CC <= (void*)1)goto _LL399;
if(*((int*)_tmp6CC)!= 16)goto _LL399;_tmp6F2=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp6CC)->f1;_LL398: if(Cyc_Absyn_no_regions)(void*)(s->r=(void*)((void*)0));
else{Cyc_Toc_exp_to_c(nv,_tmp6F2);(void*)(s->r=(void*)Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(
Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp754=_cycalloc(sizeof(*_tmp754));
_tmp754->hd=_tmp6F2;_tmp754->tl=0;_tmp754;}),0)));}return;_LL399: if(_tmp6CC <= (
void*)1)goto _LL374;if(*((int*)_tmp6CC)!= 17)goto _LL374;_tmp6F3=((struct Cyc_Absyn_Alias_s_struct*)
_tmp6CC)->f1;_tmp6F4=((struct Cyc_Absyn_Alias_s_struct*)_tmp6CC)->f2;_tmp6F5=((
struct Cyc_Absyn_Alias_s_struct*)_tmp6CC)->f3;_tmp6F6=((struct Cyc_Absyn_Alias_s_struct*)
_tmp6CC)->f4;_LL39A: {struct _tuple1*old_name=_tmp6F5->name;struct _tuple1*
new_name=Cyc_Toc_temp_var();_tmp6F5->name=new_name;_tmp6F5->initializer=(struct
Cyc_Absyn_Exp*)_tmp6F3;(void*)(s->r=(void*)((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmp755=_cycalloc(sizeof(*_tmp755));_tmp755[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp756;_tmp756.tag=11;_tmp756.f1=({struct Cyc_Absyn_Decl*_tmp757=_cycalloc(
sizeof(*_tmp757));_tmp757->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp758=_cycalloc(sizeof(*_tmp758));_tmp758[0]=({struct Cyc_Absyn_Var_d_struct
_tmp759;_tmp759.tag=0;_tmp759.f1=_tmp6F5;_tmp759;});_tmp758;}));_tmp757->loc=0;
_tmp757;});_tmp756.f2=_tmp6F6;_tmp756;});_tmp755;})));{struct _RegionHandle
_tmp75A=_new_region("temp");struct _RegionHandle*temp=& _tmp75A;_push_region(temp);{
struct Cyc_Toc_Env*_tmp75B=Cyc_Toc_add_varmap(temp,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp75C=_cycalloc(sizeof(*
_tmp75C));_tmp75C[0]=({struct Cyc_Absyn_Local_b_struct _tmp75D;_tmp75D.tag=3;
_tmp75D.f1=_tmp6F5;_tmp75D;});_tmp75C;}),0));Cyc_Toc_local_decl_to_c(_tmp75B,nv,
_tmp6F5,_tmp6F6);};_pop_region(temp);}return;}_LL374:;}}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s);struct _tuple17{struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;(void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((
void*)f->ret_type));{struct _RegionHandle _tmp75E=_new_region("frgn");struct
_RegionHandle*frgn=& _tmp75E;_push_region(frgn);{struct Cyc_Toc_Env*_tmp75F=Cyc_Toc_share_env(
frgn,nv);{struct Cyc_List_List*_tmp760=f->args;for(0;_tmp760 != 0;_tmp760=_tmp760->tl){
struct _tuple1*_tmp761=({struct _tuple1*_tmp762=_cycalloc(sizeof(*_tmp762));
_tmp762->f1=(union Cyc_Absyn_Nmspace_union)((union Cyc_Absyn_Nmspace_union)({union
Cyc_Absyn_Nmspace_union _tmp763;(_tmp763.Loc_n).tag=0;_tmp763;}));_tmp762->f2=(*((
struct _tuple17*)_tmp760->hd)).f1;_tmp762;});(*((struct _tuple17*)_tmp760->hd)).f3=
Cyc_Toc_typ_to_c((*((struct _tuple17*)_tmp760->hd)).f3);_tmp75F=Cyc_Toc_add_varmap(
frgn,_tmp75F,_tmp761,Cyc_Absyn_var_exp(_tmp761,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(
f->body);_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp765;struct Cyc_Absyn_Tqual
_tmp766;void*_tmp767;int _tmp768;struct Cyc_Absyn_VarargInfo _tmp764=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp765=_tmp764.name;_tmp766=_tmp764.tq;_tmp767=(
void*)_tmp764.type;_tmp768=_tmp764.inject;{void*_tmp769=Cyc_Toc_typ_to_c(Cyc_Absyn_dynforward_typ(
_tmp767,(void*)2,_tmp766,Cyc_Absyn_false_conref));struct _tuple1*_tmp76A=({struct
_tuple1*_tmp76D=_cycalloc(sizeof(*_tmp76D));_tmp76D->f1=(union Cyc_Absyn_Nmspace_union)((
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp76E;(_tmp76E.Loc_n).tag=
0;_tmp76E;}));_tmp76D->f2=(struct _dynforward_ptr*)((struct Cyc_Core_Opt*)
_check_null(_tmp765))->v;_tmp76D;});f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp76B=
_cycalloc(sizeof(*_tmp76B));_tmp76B->hd=({struct _tuple17*_tmp76C=_cycalloc(
sizeof(*_tmp76C));_tmp76C->f1=(struct _dynforward_ptr*)_tmp765->v;_tmp76C->f2=
_tmp766;_tmp76C->f3=_tmp769;_tmp76C;});_tmp76B->tl=0;_tmp76B;}));_tmp75F=Cyc_Toc_add_varmap(
frgn,_tmp75F,_tmp76A,Cyc_Absyn_var_exp(_tmp76A,0));f->cyc_varargs=0;}}{struct Cyc_List_List*
_tmp76F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp76F != 0;_tmp76F=_tmp76F->tl){(void*)(((struct Cyc_Absyn_Vardecl*)
_tmp76F->hd)->type=(void*)Cyc_Toc_typ_to_c((void*)((struct Cyc_Absyn_Vardecl*)
_tmp76F->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp75F),f->body);};
_pop_region(frgn);}}static void*Cyc_Toc_scope_to_c(void*s){void*_tmp770=s;_LL3AC:
if((int)_tmp770 != 1)goto _LL3AE;_LL3AD: return(void*)2;_LL3AE: if((int)_tmp770 != 4)
goto _LL3B0;_LL3AF: return(void*)3;_LL3B0:;_LL3B1: return s;_LL3AB:;}static void Cyc_Toc_aggrdecl_to_c(
struct Cyc_Absyn_Aggrdecl*ad){struct _tuple1*_tmp771=ad->name;struct
_DynRegionHandle*_tmp773;struct Cyc_Dict_Dict*_tmp774;struct Cyc_Toc_TocState
_tmp772=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp773=
_tmp772.dyn;_tmp774=_tmp772.aggrs_so_far;{struct _DynRegionFrame _tmp775;struct
_RegionHandle*d=_open_dynregion(& _tmp775,_tmp773);{int seen_defn_before;struct
_tuple7**_tmp776=((struct _tuple7**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*
_tmp774,_tmp771);if(_tmp776 == 0){seen_defn_before=0;{struct _tuple7*v;if((void*)
ad->kind == (void*)0)v=({struct _tuple7*_tmp777=_region_malloc(d,sizeof(*_tmp777));
_tmp777->f1=ad;_tmp777->f2=Cyc_Absyn_strctq(ad->name);_tmp777;});else{v=({struct
_tuple7*_tmp778=_region_malloc(d,sizeof(*_tmp778));_tmp778->f1=ad;_tmp778->f2=
Cyc_Absyn_unionq_typ(ad->name);_tmp778;});}*_tmp774=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple7*v))Cyc_Dict_insert)(*_tmp774,
_tmp771,v);}}else{struct _tuple7 _tmp77A;struct Cyc_Absyn_Aggrdecl*_tmp77B;void*
_tmp77C;struct _tuple7*_tmp779=*_tmp776;_tmp77A=*_tmp779;_tmp77B=_tmp77A.f1;
_tmp77C=_tmp77A.f2;if(_tmp77B->impl == 0){*_tmp774=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple7*v))Cyc_Dict_insert)(*_tmp774,
_tmp771,({struct _tuple7*_tmp77D=_region_malloc(d,sizeof(*_tmp77D));_tmp77D->f1=
ad;_tmp77D->f2=_tmp77C;_tmp77D;}));seen_defn_before=0;}else{seen_defn_before=1;}}(
void*)(ad->sc=(void*)((void*)2));ad->tvs=0;if(ad->impl != 0){((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars=0;((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
ad->impl))->rgn_po=0;if(seen_defn_before)ad->impl=0;else{struct Cyc_List_List*
_tmp77E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;
_tmp77E != 0;_tmp77E=_tmp77E->tl){((struct Cyc_Absyn_Aggrfield*)_tmp77E->hd)->tq=
Cyc_Toc_mt_tq;(void*)(((struct Cyc_Absyn_Aggrfield*)_tmp77E->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp77E->hd)->type));}}}};_pop_dynregion(d);}}
static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*tud){struct
_DynRegionHandle*_tmp780;struct Cyc_Set_Set**_tmp781;struct Cyc_Toc_TocState
_tmp77F=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp780=
_tmp77F.dyn;_tmp781=_tmp77F.tunions_so_far;{struct _DynRegionFrame _tmp782;struct
_RegionHandle*d=_open_dynregion(& _tmp782,_tmp780);{struct _tuple1*_tmp783=tud->name;
if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*
_tmp781,_tmp783)){_npop_handler(0);return;}*_tmp781=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*
_tmp781,_tmp783);};_pop_dynregion(d);}{struct Cyc_List_List*flat_structs=0;{
struct Cyc_List_List*_tmp784=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp784 != 0;_tmp784=_tmp784->tl){struct Cyc_Absyn_Tunionfield*
f=(struct Cyc_Absyn_Tunionfield*)_tmp784->hd;if(f->typs != 0  || tud->is_flat){
struct Cyc_List_List*_tmp785=0;int i=1;{struct Cyc_List_List*_tmp786=f->typs;for(0;
_tmp786 != 0;(_tmp786=_tmp786->tl,i ++)){struct _dynforward_ptr*_tmp787=Cyc_Absyn_fieldname(
i);struct Cyc_Absyn_Aggrfield*_tmp788=({struct Cyc_Absyn_Aggrfield*_tmp78A=
_cycalloc(sizeof(*_tmp78A));_tmp78A->name=_tmp787;_tmp78A->tq=(*((struct _tuple4*)
_tmp786->hd)).f1;_tmp78A->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)
_tmp786->hd)).f2);_tmp78A->width=0;_tmp78A->attributes=0;_tmp78A;});_tmp785=({
struct Cyc_List_List*_tmp789=_cycalloc(sizeof(*_tmp789));_tmp789->hd=_tmp788;
_tmp789->tl=_tmp785;_tmp789;});}}_tmp785=({struct Cyc_List_List*_tmp78B=_cycalloc(
sizeof(*_tmp78B));_tmp78B->hd=({struct Cyc_Absyn_Aggrfield*_tmp78C=_cycalloc(
sizeof(*_tmp78C));_tmp78C->name=Cyc_Toc_tag_sp;_tmp78C->tq=Cyc_Toc_mt_tq;_tmp78C->type=(
void*)Cyc_Absyn_sint_typ;_tmp78C->width=0;_tmp78C->attributes=0;_tmp78C;});
_tmp78B->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp785);_tmp78B;});{struct Cyc_Absyn_Aggrdecl*_tmp78D=({struct Cyc_Absyn_Aggrdecl*
_tmp799=_cycalloc(sizeof(*_tmp799));_tmp799->kind=(void*)((void*)0);_tmp799->sc=(
void*)((void*)2);_tmp799->name=Cyc_Toc_collapse_qvar_tag(f->name,({const char*
_tmp79B="_struct";_tag_dynforward(_tmp79B,sizeof(char),_get_zero_arr_size_char(
_tmp79B,8));}));_tmp799->tvs=0;_tmp799->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp79A=_cycalloc(sizeof(*_tmp79A));_tmp79A->exist_vars=0;_tmp79A->rgn_po=0;
_tmp79A->fields=_tmp785;_tmp79A;});_tmp799->attributes=0;_tmp799;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp78E=_cycalloc(sizeof(*_tmp78E));_tmp78E->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp78F=_cycalloc(sizeof(*_tmp78F));
_tmp78F[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp790;_tmp790.tag=4;_tmp790.f1=
_tmp78D;_tmp790;});_tmp78F;}),0);_tmp78E->tl=Cyc_Toc_result_decls;_tmp78E;});if(
tud->is_flat){struct Cyc_Absyn_Aggrfield*_tmp791=({struct Cyc_Absyn_Aggrfield*
_tmp793=_cycalloc(sizeof(*_tmp793));_tmp793->name=(*f->name).f2;_tmp793->tq=Cyc_Toc_mt_tq;
_tmp793->type=(void*)((void*)({struct Cyc_Absyn_AggrType_struct*_tmp794=_cycalloc(
sizeof(*_tmp794));_tmp794[0]=({struct Cyc_Absyn_AggrType_struct _tmp795;_tmp795.tag=
10;_tmp795.f1=({struct Cyc_Absyn_AggrInfo _tmp796;_tmp796.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp797;(_tmp797.KnownAggr).tag=1;(_tmp797.KnownAggr).f1=({
struct Cyc_Absyn_Aggrdecl**_tmp798=_cycalloc(sizeof(*_tmp798));_tmp798[0]=_tmp78D;
_tmp798;});_tmp797;});_tmp796.targs=0;_tmp796;});_tmp795;});_tmp794;}));_tmp793->width=
0;_tmp793->attributes=0;_tmp793;});flat_structs=({struct Cyc_List_List*_tmp792=
_cycalloc(sizeof(*_tmp792));_tmp792->hd=_tmp791;_tmp792->tl=flat_structs;_tmp792;});}}}}}
if(tud->is_flat){flat_structs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(flat_structs);{struct Cyc_Absyn_Aggrdecl*_tmp79C=({struct Cyc_Absyn_Aggrdecl*
_tmp7A0=_cycalloc(sizeof(*_tmp7A0));_tmp7A0->kind=(void*)((void*)1);_tmp7A0->sc=(
void*)((void*)2);_tmp7A0->name=Cyc_Toc_collapse_qvar_tag(tud->name,({const char*
_tmp7A2="_union";_tag_dynforward(_tmp7A2,sizeof(char),_get_zero_arr_size_char(
_tmp7A2,7));}));_tmp7A0->tvs=0;_tmp7A0->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp7A1=_cycalloc(sizeof(*_tmp7A1));_tmp7A1->exist_vars=0;_tmp7A1->rgn_po=0;
_tmp7A1->fields=flat_structs;_tmp7A1;});_tmp7A0->attributes=0;_tmp7A0;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp79D=_cycalloc(sizeof(*_tmp79D));_tmp79D->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp79E=_cycalloc(sizeof(*_tmp79E));
_tmp79E[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp79F;_tmp79F.tag=4;_tmp79F.f1=
_tmp79C;_tmp79F;});_tmp79E;}),0);_tmp79D->tl=Cyc_Toc_result_decls;_tmp79D;});}}}}
static void Cyc_Toc_xtuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*xd){if(xd->fields
== 0)return;{struct _DynRegionHandle*_tmp7A4;struct Cyc_Dict_Dict*_tmp7A5;struct
Cyc_Toc_TocState _tmp7A3=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp7A4=_tmp7A3.dyn;_tmp7A5=_tmp7A3.xtunions_so_far;{struct _DynRegionFrame
_tmp7A6;struct _RegionHandle*d=_open_dynregion(& _tmp7A6,_tmp7A4);{struct _tuple1*
_tmp7A7=xd->name;struct Cyc_List_List*_tmp7A8=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp7A8 != 0;_tmp7A8=_tmp7A8->tl){struct Cyc_Absyn_Tunionfield*
f=(struct Cyc_Absyn_Tunionfield*)_tmp7A8->hd;struct _dynforward_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp7A9=Cyc_Absyn_uint_exp(_get_dynforward_size(*fn,sizeof(
char))+ 4,0);void*_tmp7AA=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp7A9,Cyc_Absyn_false_conref,0);int*_tmp7AB=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp7A5,f->name);int
_tmp7AC;_LL3B3: if(_tmp7AB != 0)goto _LL3B5;_LL3B4: {struct Cyc_Absyn_Exp*initopt=0;
if((void*)f->sc != (void*)3){char zero='\000';initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp7B3;_tmp7B3.tag=0;_tmp7B3.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*fn);{struct Cyc_Int_pa_struct
_tmp7B2;_tmp7B2.tag=1;_tmp7B2.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp7B1;_tmp7B1.tag=1;_tmp7B1.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp7B0;_tmp7B0.tag=1;_tmp7B0.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp7AF;_tmp7AF.tag=1;_tmp7AF.f1=(unsigned long)((int)zero);{void*_tmp7AD[5]={&
_tmp7AF,& _tmp7B0,& _tmp7B1,& _tmp7B2,& _tmp7B3};Cyc_aprintf(({const char*_tmp7AE="%c%c%c%c%s";
_tag_dynforward(_tmp7AE,sizeof(char),_get_zero_arr_size_char(_tmp7AE,11));}),
_tag_dynforward(_tmp7AD,sizeof(void*),5));}}}}}}),0);}{struct Cyc_Absyn_Vardecl*
_tmp7B4=Cyc_Absyn_new_vardecl(f->name,_tmp7AA,initopt);(void*)(_tmp7B4->sc=(void*)((
void*)f->sc));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp7B5=_cycalloc(
sizeof(*_tmp7B5));_tmp7B5->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp7B6=_cycalloc(sizeof(*_tmp7B6));_tmp7B6[0]=({struct Cyc_Absyn_Var_d_struct
_tmp7B7;_tmp7B7.tag=0;_tmp7B7.f1=_tmp7B4;_tmp7B7;});_tmp7B6;}),0);_tmp7B5->tl=
Cyc_Toc_result_decls;_tmp7B5;});*_tmp7A5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp7A5,f->name,(void*)f->sc != (void*)
3);if(f->typs != 0){struct Cyc_List_List*fields=0;int i=1;{struct Cyc_List_List*
_tmp7B8=f->typs;for(0;_tmp7B8 != 0;(_tmp7B8=_tmp7B8->tl,i ++)){struct
_dynforward_ptr*_tmp7B9=({struct _dynforward_ptr*_tmp7BD=_cycalloc(sizeof(*
_tmp7BD));_tmp7BD[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp7C0;
_tmp7C0.tag=1;_tmp7C0.f1=(unsigned long)i;{void*_tmp7BE[1]={& _tmp7C0};Cyc_aprintf(({
const char*_tmp7BF="f%d";_tag_dynforward(_tmp7BF,sizeof(char),
_get_zero_arr_size_char(_tmp7BF,4));}),_tag_dynforward(_tmp7BE,sizeof(void*),1));}});
_tmp7BD;});struct Cyc_Absyn_Aggrfield*_tmp7BA=({struct Cyc_Absyn_Aggrfield*_tmp7BC=
_cycalloc(sizeof(*_tmp7BC));_tmp7BC->name=_tmp7B9;_tmp7BC->tq=(*((struct _tuple4*)
_tmp7B8->hd)).f1;_tmp7BC->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)
_tmp7B8->hd)).f2);_tmp7BC->width=0;_tmp7BC->attributes=0;_tmp7BC;});fields=({
struct Cyc_List_List*_tmp7BB=_cycalloc(sizeof(*_tmp7BB));_tmp7BB->hd=_tmp7BA;
_tmp7BB->tl=fields;_tmp7BB;});}}fields=({struct Cyc_List_List*_tmp7C1=_cycalloc(
sizeof(*_tmp7C1));_tmp7C1->hd=({struct Cyc_Absyn_Aggrfield*_tmp7C2=_cycalloc(
sizeof(*_tmp7C2));_tmp7C2->name=Cyc_Toc_tag_sp;_tmp7C2->tq=Cyc_Toc_mt_tq;_tmp7C2->type=(
void*)Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp7C2->width=0;
_tmp7C2->attributes=0;_tmp7C2;});_tmp7C1->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(fields);_tmp7C1;});{struct Cyc_Absyn_Aggrdecl*_tmp7C3=({
struct Cyc_Absyn_Aggrdecl*_tmp7C7=_cycalloc(sizeof(*_tmp7C7));_tmp7C7->kind=(void*)((
void*)0);_tmp7C7->sc=(void*)((void*)2);_tmp7C7->name=Cyc_Toc_collapse_qvar_tag(f->name,({
const char*_tmp7C9="_struct";_tag_dynforward(_tmp7C9,sizeof(char),
_get_zero_arr_size_char(_tmp7C9,8));}));_tmp7C7->tvs=0;_tmp7C7->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp7C8=_cycalloc(sizeof(*_tmp7C8));_tmp7C8->exist_vars=0;_tmp7C8->rgn_po=0;
_tmp7C8->fields=fields;_tmp7C8;});_tmp7C7->attributes=0;_tmp7C7;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp7C4=_cycalloc(sizeof(*_tmp7C4));_tmp7C4->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp7C5=_cycalloc(sizeof(*_tmp7C5));
_tmp7C5[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp7C6;_tmp7C6.tag=4;_tmp7C6.f1=
_tmp7C3;_tmp7C6;});_tmp7C5;}),0);_tmp7C4->tl=Cyc_Toc_result_decls;_tmp7C4;});}}
goto _LL3B2;}}_LL3B5: if(_tmp7AB == 0)goto _LL3B7;_tmp7AC=*_tmp7AB;if(_tmp7AC != 0)
goto _LL3B7;_LL3B6: if((void*)f->sc != (void*)3){char zero='\000';struct Cyc_Absyn_Exp*
_tmp7CA=Cyc_Absyn_string_exp((struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp7D5;_tmp7D5.tag=0;_tmp7D5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
fn);{struct Cyc_Int_pa_struct _tmp7D4;_tmp7D4.tag=1;_tmp7D4.f1=(unsigned long)((
int)zero);{struct Cyc_Int_pa_struct _tmp7D3;_tmp7D3.tag=1;_tmp7D3.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp7D2;_tmp7D2.tag=1;_tmp7D2.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp7D1;_tmp7D1.tag=1;_tmp7D1.f1=(
unsigned long)((int)zero);{void*_tmp7CF[5]={& _tmp7D1,& _tmp7D2,& _tmp7D3,& _tmp7D4,&
_tmp7D5};Cyc_aprintf(({const char*_tmp7D0="%c%c%c%c%s";_tag_dynforward(_tmp7D0,
sizeof(char),_get_zero_arr_size_char(_tmp7D0,11));}),_tag_dynforward(_tmp7CF,
sizeof(void*),5));}}}}}}),0);struct Cyc_Absyn_Vardecl*_tmp7CB=Cyc_Absyn_new_vardecl(
f->name,_tmp7AA,(struct Cyc_Absyn_Exp*)_tmp7CA);(void*)(_tmp7CB->sc=(void*)((void*)
f->sc));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp7CC=_cycalloc(sizeof(*
_tmp7CC));_tmp7CC->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp7CD=_cycalloc(sizeof(*_tmp7CD));_tmp7CD[0]=({struct Cyc_Absyn_Var_d_struct
_tmp7CE;_tmp7CE.tag=0;_tmp7CE.f1=_tmp7CB;_tmp7CE;});_tmp7CD;}),0);_tmp7CC->tl=
Cyc_Toc_result_decls;_tmp7CC;});*_tmp7A5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp7A5,f->name,1);}goto _LL3B2;_LL3B7:;
_LL3B8: goto _LL3B2;_LL3B2:;}};_pop_dynregion(d);}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)
2));if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){void*old_typ=(void*)vd->type;(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array(
old_typ));{int forward_only=0;if((void*)vd->sc == (void*)5  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ,& forward_only))(void*)(vd->sc=(void*)((void*)2));Cyc_Toc_stmt_to_c(
body_nv,s);if(vd->initializer != 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)
_check_null(vd->initializer);void*_tmp7D6=(void*)init->r;struct Cyc_Absyn_Vardecl*
_tmp7D7;struct Cyc_Absyn_Exp*_tmp7D8;struct Cyc_Absyn_Exp*_tmp7D9;int _tmp7DA;
_LL3BA: if(*((int*)_tmp7D6)!= 29)goto _LL3BC;_tmp7D7=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp7D6)->f1;_tmp7D8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp7D6)->f2;
_tmp7D9=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp7D6)->f3;_tmp7DA=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp7D6)->f4;_LL3BB: vd->initializer=0;(void*)(
s->r=(void*)((void*)(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(vd->name,
0),_tmp7D7,_tmp7D8,_tmp7D9,_tmp7DA,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));
goto _LL3B9;_LL3BC:;_LL3BD: if((void*)vd->sc == (void*)0){struct _RegionHandle
_tmp7DB=_new_region("temp");struct _RegionHandle*temp=& _tmp7DB;_push_region(temp);{
struct Cyc_Toc_Env*_tmp7DC=Cyc_Toc_set_toplevel(temp,init_nv);Cyc_Toc_exp_to_c(
_tmp7DC,init);};_pop_region(temp);}else{Cyc_Toc_exp_to_c(init_nv,init);}goto
_LL3B9;_LL3B9:;}else{void*_tmp7DD=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_ArrayInfo
_tmp7DE;void*_tmp7DF;struct Cyc_Absyn_Exp*_tmp7E0;struct Cyc_Absyn_Conref*_tmp7E1;
_LL3BF: if(_tmp7DD <= (void*)4)goto _LL3C1;if(*((int*)_tmp7DD)!= 7)goto _LL3C1;
_tmp7DE=((struct Cyc_Absyn_ArrayType_struct*)_tmp7DD)->f1;_tmp7DF=(void*)_tmp7DE.elt_type;
_tmp7E0=_tmp7DE.num_elts;_tmp7E1=_tmp7DE.zero_term;if(!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp7E1))goto _LL3C1;_LL3C0: if(_tmp7E0 == 0)({void*
_tmp7E2=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp7E3="can't initialize zero-terminated array -- size unknown";
_tag_dynforward(_tmp7E3,sizeof(char),_get_zero_arr_size_char(_tmp7E3,55));}),
_tag_dynforward(_tmp7E2,sizeof(void*),0));});{struct Cyc_Absyn_Exp*num_elts=(
struct Cyc_Absyn_Exp*)_tmp7E0;struct Cyc_Absyn_Exp*_tmp7E4=Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(
- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp7E5=Cyc_Absyn_signed_int_exp(0,0);(void*)(s->r=(
void*)Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp7E4,_tmp7E5,
0),0),Cyc_Absyn_new_stmt((void*)s->r,0)));goto _LL3BE;}_LL3C1:;_LL3C2: goto _LL3BE;
_LL3BE:;}}}static struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=0;static
struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){if(Cyc_Toc_throw_match_stmt_opt
== 0)Cyc_Toc_throw_match_stmt_opt=({struct Cyc_Absyn_Stmt**_tmp7E6=_cycalloc(
sizeof(*_tmp7E6));_tmp7E6[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(
0)),0);_tmp7E6;});return*((struct Cyc_Absyn_Stmt**)_check_null(Cyc_Toc_throw_match_stmt_opt));}
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();{struct _RegionHandle _tmp7E7=_new_region("prgn");
struct _RegionHandle*prgn=& _tmp7E7;_push_region(prgn);{struct Cyc_Absyn_Stmt*
_tmp7E8=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*_tmp7E9=Cyc_Toc_share_env(
prgn,nv);struct Cyc_Toc_Env*_tmp7EB;struct Cyc_List_List*_tmp7EC;struct Cyc_Absyn_Stmt*
_tmp7ED;struct _tuple13 _tmp7EA=Cyc_Toc_xlate_pat(_tmp7E9,prgn,t,Cyc_Absyn_var_exp(
x,0),Cyc_Absyn_var_exp(x,0),p,(struct Cyc_Absyn_Stmt**)& _tmp7E8,Cyc_Toc_throw_match_stmt(),
0);_tmp7EB=_tmp7EA.f1;_tmp7EC=_tmp7EA.f2;_tmp7ED=_tmp7EA.f3;Cyc_Toc_stmt_to_c(
_tmp7EB,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp7ED,s,0),0);for(0;_tmp7EC != 0;_tmp7EC=_tmp7EC->tl){
struct _tuple14 _tmp7EF;struct _tuple1*_tmp7F0;void*_tmp7F1;struct _tuple14*_tmp7EE=(
struct _tuple14*)_tmp7EC->hd;_tmp7EF=*_tmp7EE;_tmp7F0=_tmp7EF.f1;_tmp7F1=_tmp7EF.f2;
s=Cyc_Absyn_declare_stmt(_tmp7F0,_tmp7F1,0,s,0);}};_pop_region(prgn);}return s;}}
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){void*_tmp7F2=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp7F3;struct Cyc_Absyn_Exp*_tmp7F4;struct Cyc_Absyn_Exp*
_tmp7F5;struct Cyc_Absyn_Exp*_tmp7F6;struct Cyc_Absyn_Exp*_tmp7F7;struct Cyc_Absyn_Exp*
_tmp7F8;struct Cyc_Absyn_Exp*_tmp7F9;struct Cyc_Absyn_Exp*_tmp7FA;struct Cyc_List_List*
_tmp7FB;struct Cyc_Absyn_Exp*_tmp7FC;struct Cyc_Absyn_Exp*_tmp7FD;struct Cyc_Absyn_Exp*
_tmp7FE;struct Cyc_Absyn_Exp*_tmp7FF;struct Cyc_Absyn_Exp*_tmp800;struct Cyc_Absyn_Exp*
_tmp801;struct Cyc_Absyn_Exp*_tmp802;struct Cyc_Absyn_Exp*_tmp803;struct Cyc_Absyn_Exp*
_tmp804;struct Cyc_Absyn_Exp*_tmp805;struct Cyc_Absyn_Exp*_tmp806;struct Cyc_Absyn_Exp*
_tmp807;struct Cyc_Absyn_Exp*_tmp808;struct Cyc_Absyn_Exp*_tmp809;struct Cyc_Absyn_Exp*
_tmp80A;struct Cyc_Absyn_Exp*_tmp80B;struct Cyc_List_List*_tmp80C;struct Cyc_Absyn_Exp*
_tmp80D;struct Cyc_List_List*_tmp80E;void*_tmp80F;void**_tmp810;struct Cyc_Absyn_Exp*
_tmp811;struct _tuple2*_tmp812;struct _tuple2 _tmp813;void*_tmp814;void**_tmp815;
struct Cyc_List_List*_tmp816;struct Cyc_List_List*_tmp817;struct Cyc_List_List*
_tmp818;void*_tmp819;void**_tmp81A;void*_tmp81B;void**_tmp81C;struct Cyc_Absyn_Stmt*
_tmp81D;struct Cyc_Absyn_MallocInfo _tmp81E;struct Cyc_Absyn_MallocInfo*_tmp81F;
_LL3C4: if(*((int*)_tmp7F2)!= 22)goto _LL3C6;_tmp7F3=((struct Cyc_Absyn_Deref_e_struct*)
_tmp7F2)->f1;_LL3C5: _tmp7F4=_tmp7F3;goto _LL3C7;_LL3C6: if(*((int*)_tmp7F2)!= 23)
goto _LL3C8;_tmp7F4=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp7F2)->f1;_LL3C7:
_tmp7F5=_tmp7F4;goto _LL3C9;_LL3C8: if(*((int*)_tmp7F2)!= 24)goto _LL3CA;_tmp7F5=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp7F2)->f1;_LL3C9: _tmp7F6=_tmp7F5;goto
_LL3CB;_LL3CA: if(*((int*)_tmp7F2)!= 16)goto _LL3CC;_tmp7F6=((struct Cyc_Absyn_Address_e_struct*)
_tmp7F2)->f1;_LL3CB: _tmp7F7=_tmp7F6;goto _LL3CD;_LL3CC: if(*((int*)_tmp7F2)!= 12)
goto _LL3CE;_tmp7F7=((struct Cyc_Absyn_Throw_e_struct*)_tmp7F2)->f1;_LL3CD: _tmp7F8=
_tmp7F7;goto _LL3CF;_LL3CE: if(*((int*)_tmp7F2)!= 13)goto _LL3D0;_tmp7F8=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp7F2)->f1;_LL3CF: _tmp7F9=_tmp7F8;goto _LL3D1;
_LL3D0: if(*((int*)_tmp7F2)!= 19)goto _LL3D2;_tmp7F9=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp7F2)->f1;_LL3D1: _tmp7FA=_tmp7F9;goto _LL3D3;_LL3D2: if(*((int*)_tmp7F2)!= 5)
goto _LL3D4;_tmp7FA=((struct Cyc_Absyn_Increment_e_struct*)_tmp7F2)->f1;_LL3D3: Cyc_Toc_exptypes_to_c(
_tmp7FA);goto _LL3C3;_LL3D4: if(*((int*)_tmp7F2)!= 3)goto _LL3D6;_tmp7FB=((struct
Cyc_Absyn_Primop_e_struct*)_tmp7F2)->f2;_LL3D5:((void(*)(void(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp7FB);goto _LL3C3;
_LL3D6: if(*((int*)_tmp7F2)!= 7)goto _LL3D8;_tmp7FC=((struct Cyc_Absyn_And_e_struct*)
_tmp7F2)->f1;_tmp7FD=((struct Cyc_Absyn_And_e_struct*)_tmp7F2)->f2;_LL3D7: _tmp7FE=
_tmp7FC;_tmp7FF=_tmp7FD;goto _LL3D9;_LL3D8: if(*((int*)_tmp7F2)!= 8)goto _LL3DA;
_tmp7FE=((struct Cyc_Absyn_Or_e_struct*)_tmp7F2)->f1;_tmp7FF=((struct Cyc_Absyn_Or_e_struct*)
_tmp7F2)->f2;_LL3D9: _tmp800=_tmp7FE;_tmp801=_tmp7FF;goto _LL3DB;_LL3DA: if(*((int*)
_tmp7F2)!= 9)goto _LL3DC;_tmp800=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp7F2)->f1;
_tmp801=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp7F2)->f2;_LL3DB: _tmp802=_tmp800;
_tmp803=_tmp801;goto _LL3DD;_LL3DC: if(*((int*)_tmp7F2)!= 25)goto _LL3DE;_tmp802=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp7F2)->f1;_tmp803=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp7F2)->f2;_LL3DD: _tmp804=_tmp802;_tmp805=_tmp803;goto _LL3DF;_LL3DE: if(*((int*)
_tmp7F2)!= 36)goto _LL3E0;_tmp804=((struct Cyc_Absyn_Swap_e_struct*)_tmp7F2)->f1;
_tmp805=((struct Cyc_Absyn_Swap_e_struct*)_tmp7F2)->f2;_LL3DF: _tmp806=_tmp804;
_tmp807=_tmp805;goto _LL3E1;_LL3E0: if(*((int*)_tmp7F2)!= 4)goto _LL3E2;_tmp806=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp7F2)->f1;_tmp807=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp7F2)->f3;_LL3E1: Cyc_Toc_exptypes_to_c(_tmp806);Cyc_Toc_exptypes_to_c(_tmp807);
goto _LL3C3;_LL3E2: if(*((int*)_tmp7F2)!= 6)goto _LL3E4;_tmp808=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp7F2)->f1;_tmp809=((struct Cyc_Absyn_Conditional_e_struct*)_tmp7F2)->f2;
_tmp80A=((struct Cyc_Absyn_Conditional_e_struct*)_tmp7F2)->f3;_LL3E3: Cyc_Toc_exptypes_to_c(
_tmp808);Cyc_Toc_exptypes_to_c(_tmp809);Cyc_Toc_exptypes_to_c(_tmp80A);goto
_LL3C3;_LL3E4: if(*((int*)_tmp7F2)!= 11)goto _LL3E6;_tmp80B=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp7F2)->f1;_tmp80C=((struct Cyc_Absyn_FnCall_e_struct*)_tmp7F2)->f2;_LL3E5:
_tmp80D=_tmp80B;_tmp80E=_tmp80C;goto _LL3E7;_LL3E6: if(*((int*)_tmp7F2)!= 10)goto
_LL3E8;_tmp80D=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp7F2)->f1;_tmp80E=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp7F2)->f2;_LL3E7: Cyc_Toc_exptypes_to_c(
_tmp80D);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp80E);goto _LL3C3;_LL3E8: if(*((int*)_tmp7F2)!= 15)goto
_LL3EA;_tmp80F=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp7F2)->f1;_tmp810=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp7F2)->f1);_tmp811=((struct
Cyc_Absyn_Cast_e_struct*)_tmp7F2)->f2;_LL3E9:*_tmp810=Cyc_Toc_typ_to_c(*_tmp810);
Cyc_Toc_exptypes_to_c(_tmp811);goto _LL3C3;_LL3EA: if(*((int*)_tmp7F2)!= 27)goto
_LL3EC;_tmp812=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp7F2)->f1;_tmp813=*
_tmp812;_tmp814=_tmp813.f3;_tmp815=(void**)&(*((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp7F2)->f1).f3;_tmp816=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp7F2)->f2;
_LL3EB:*_tmp815=Cyc_Toc_typ_to_c(*_tmp815);_tmp817=_tmp816;goto _LL3ED;_LL3EC: if(*((
int*)_tmp7F2)!= 37)goto _LL3EE;_tmp817=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp7F2)->f2;_LL3ED: _tmp818=_tmp817;goto _LL3EF;_LL3EE: if(*((int*)_tmp7F2)!= 28)
goto _LL3F0;_tmp818=((struct Cyc_Absyn_Array_e_struct*)_tmp7F2)->f1;_LL3EF: for(0;
_tmp818 != 0;_tmp818=_tmp818->tl){struct Cyc_Absyn_Exp*_tmp821;struct _tuple8
_tmp820=*((struct _tuple8*)_tmp818->hd);_tmp821=_tmp820.f2;Cyc_Toc_exptypes_to_c(
_tmp821);}goto _LL3C3;_LL3F0: if(*((int*)_tmp7F2)!= 20)goto _LL3F2;_tmp819=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp7F2)->f1;_tmp81A=(void**)&((void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp7F2)->f1);_LL3F1: _tmp81C=_tmp81A;goto
_LL3F3;_LL3F2: if(*((int*)_tmp7F2)!= 18)goto _LL3F4;_tmp81B=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp7F2)->f1;_tmp81C=(void**)&((void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp7F2)->f1);_LL3F3:*_tmp81C=Cyc_Toc_typ_to_c(*_tmp81C);goto _LL3C3;_LL3F4: if(*((
int*)_tmp7F2)!= 38)goto _LL3F6;_tmp81D=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp7F2)->f1;_LL3F5: Cyc_Toc_stmttypes_to_c(_tmp81D);goto _LL3C3;_LL3F6: if(*((int*)
_tmp7F2)!= 35)goto _LL3F8;_tmp81E=((struct Cyc_Absyn_Malloc_e_struct*)_tmp7F2)->f1;
_tmp81F=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_struct*)_tmp7F2)->f1;
_LL3F7: if(_tmp81F->elt_type != 0)_tmp81F->elt_type=({void**_tmp822=_cycalloc(
sizeof(*_tmp822));_tmp822[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp81F->elt_type)));
_tmp822;});Cyc_Toc_exptypes_to_c(_tmp81F->num_elts);goto _LL3C3;_LL3F8: if(*((int*)
_tmp7F2)!= 0)goto _LL3FA;_LL3F9: goto _LL3FB;_LL3FA: if(*((int*)_tmp7F2)!= 1)goto
_LL3FC;_LL3FB: goto _LL3FD;_LL3FC: if(*((int*)_tmp7F2)!= 2)goto _LL3FE;_LL3FD: goto
_LL3FF;_LL3FE: if(*((int*)_tmp7F2)!= 33)goto _LL400;_LL3FF: goto _LL401;_LL400: if(*((
int*)_tmp7F2)!= 34)goto _LL402;_LL401: goto _LL3C3;_LL402: if(*((int*)_tmp7F2)!= 31)
goto _LL404;_LL403: goto _LL405;_LL404: if(*((int*)_tmp7F2)!= 32)goto _LL406;_LL405:
goto _LL407;_LL406: if(*((int*)_tmp7F2)!= 30)goto _LL408;_LL407: goto _LL409;_LL408:
if(*((int*)_tmp7F2)!= 29)goto _LL40A;_LL409: goto _LL40B;_LL40A: if(*((int*)_tmp7F2)
!= 26)goto _LL40C;_LL40B: goto _LL40D;_LL40C: if(*((int*)_tmp7F2)!= 14)goto _LL40E;
_LL40D: goto _LL40F;_LL40E: if(*((int*)_tmp7F2)!= 17)goto _LL410;_LL40F: goto _LL411;
_LL410: if(*((int*)_tmp7F2)!= 21)goto _LL3C3;_LL411:({void*_tmp823=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmp824="Cyclone expression within C code";_tag_dynforward(
_tmp824,sizeof(char),_get_zero_arr_size_char(_tmp824,33));}),_tag_dynforward(
_tmp823,sizeof(void*),0));});goto _LL3C3;_LL3C3:;}static void Cyc_Toc_decltypes_to_c(
struct Cyc_Absyn_Decl*d){void*_tmp825=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp826;
struct Cyc_Absyn_Fndecl*_tmp827;struct Cyc_Absyn_Aggrdecl*_tmp828;struct Cyc_Absyn_Enumdecl*
_tmp829;struct Cyc_Absyn_Typedefdecl*_tmp82A;_LL413: if(_tmp825 <= (void*)2)goto
_LL42B;if(*((int*)_tmp825)!= 0)goto _LL415;_tmp826=((struct Cyc_Absyn_Var_d_struct*)
_tmp825)->f1;_LL414:(void*)(_tmp826->type=(void*)Cyc_Toc_typ_to_c((void*)_tmp826->type));
if(_tmp826->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp826->initializer));goto _LL412;_LL415: if(*((int*)_tmp825)!= 1)
goto _LL417;_tmp827=((struct Cyc_Absyn_Fn_d_struct*)_tmp825)->f1;_LL416:(void*)(
_tmp827->ret_type=(void*)Cyc_Toc_typ_to_c((void*)_tmp827->ret_type));{struct Cyc_List_List*
_tmp82B=_tmp827->args;for(0;_tmp82B != 0;_tmp82B=_tmp82B->tl){(*((struct _tuple17*)
_tmp82B->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple17*)_tmp82B->hd)).f3);}}goto
_LL412;_LL417: if(*((int*)_tmp825)!= 4)goto _LL419;_tmp828=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp825)->f1;_LL418: Cyc_Toc_aggrdecl_to_c(_tmp828);goto _LL412;_LL419: if(*((int*)
_tmp825)!= 6)goto _LL41B;_tmp829=((struct Cyc_Absyn_Enum_d_struct*)_tmp825)->f1;
_LL41A: if(_tmp829->fields != 0){struct Cyc_List_List*_tmp82C=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp829->fields))->v;for(0;_tmp82C != 0;_tmp82C=
_tmp82C->tl){struct Cyc_Absyn_Enumfield*_tmp82D=(struct Cyc_Absyn_Enumfield*)
_tmp82C->hd;if(_tmp82D->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp82D->tag));}}goto _LL412;_LL41B: if(*((int*)_tmp825)!= 7)goto
_LL41D;_tmp82A=((struct Cyc_Absyn_Typedef_d_struct*)_tmp825)->f1;_LL41C: _tmp82A->defn=({
struct Cyc_Core_Opt*_tmp82E=_cycalloc(sizeof(*_tmp82E));_tmp82E->v=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp82A->defn))->v);_tmp82E;});goto
_LL412;_LL41D: if(*((int*)_tmp825)!= 2)goto _LL41F;_LL41E: goto _LL420;_LL41F: if(*((
int*)_tmp825)!= 3)goto _LL421;_LL420: goto _LL422;_LL421: if(*((int*)_tmp825)!= 5)
goto _LL423;_LL422: goto _LL424;_LL423: if(*((int*)_tmp825)!= 8)goto _LL425;_LL424:
goto _LL426;_LL425: if(*((int*)_tmp825)!= 9)goto _LL427;_LL426: goto _LL428;_LL427:
if(*((int*)_tmp825)!= 10)goto _LL429;_LL428: goto _LL42A;_LL429: if(*((int*)_tmp825)
!= 11)goto _LL42B;_LL42A:({void*_tmp82F=0;Cyc_Tcutil_terr(d->loc,({const char*
_tmp830="Cyclone declaration within C code";_tag_dynforward(_tmp830,sizeof(char),
_get_zero_arr_size_char(_tmp830,34));}),_tag_dynforward(_tmp82F,sizeof(void*),0));});
goto _LL412;_LL42B: if((int)_tmp825 != 0)goto _LL42D;_LL42C: goto _LL42E;_LL42D: if((
int)_tmp825 != 1)goto _LL412;_LL42E: goto _LL412;_LL412:;}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s){void*_tmp831=(void*)s->r;struct Cyc_Absyn_Exp*_tmp832;
struct Cyc_Absyn_Stmt*_tmp833;struct Cyc_Absyn_Stmt*_tmp834;struct Cyc_Absyn_Exp*
_tmp835;struct Cyc_Absyn_Exp*_tmp836;struct Cyc_Absyn_Stmt*_tmp837;struct Cyc_Absyn_Stmt*
_tmp838;struct _tuple3 _tmp839;struct Cyc_Absyn_Exp*_tmp83A;struct Cyc_Absyn_Stmt*
_tmp83B;struct Cyc_Absyn_Exp*_tmp83C;struct _tuple3 _tmp83D;struct Cyc_Absyn_Exp*
_tmp83E;struct _tuple3 _tmp83F;struct Cyc_Absyn_Exp*_tmp840;struct Cyc_Absyn_Stmt*
_tmp841;struct Cyc_Absyn_Exp*_tmp842;struct Cyc_List_List*_tmp843;struct Cyc_Absyn_Decl*
_tmp844;struct Cyc_Absyn_Stmt*_tmp845;struct Cyc_Absyn_Stmt*_tmp846;struct _tuple3
_tmp847;struct Cyc_Absyn_Exp*_tmp848;_LL430: if(_tmp831 <= (void*)1)goto _LL442;if(*((
int*)_tmp831)!= 0)goto _LL432;_tmp832=((struct Cyc_Absyn_Exp_s_struct*)_tmp831)->f1;
_LL431: Cyc_Toc_exptypes_to_c(_tmp832);goto _LL42F;_LL432: if(*((int*)_tmp831)!= 1)
goto _LL434;_tmp833=((struct Cyc_Absyn_Seq_s_struct*)_tmp831)->f1;_tmp834=((struct
Cyc_Absyn_Seq_s_struct*)_tmp831)->f2;_LL433: Cyc_Toc_stmttypes_to_c(_tmp833);Cyc_Toc_stmttypes_to_c(
_tmp834);goto _LL42F;_LL434: if(*((int*)_tmp831)!= 2)goto _LL436;_tmp835=((struct
Cyc_Absyn_Return_s_struct*)_tmp831)->f1;_LL435: if(_tmp835 != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_tmp835);goto _LL42F;_LL436: if(*((int*)_tmp831)!= 3)goto
_LL438;_tmp836=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp831)->f1;_tmp837=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp831)->f2;_tmp838=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp831)->f3;_LL437: Cyc_Toc_exptypes_to_c(_tmp836);Cyc_Toc_stmttypes_to_c(
_tmp837);Cyc_Toc_stmttypes_to_c(_tmp838);goto _LL42F;_LL438: if(*((int*)_tmp831)!= 
4)goto _LL43A;_tmp839=((struct Cyc_Absyn_While_s_struct*)_tmp831)->f1;_tmp83A=
_tmp839.f1;_tmp83B=((struct Cyc_Absyn_While_s_struct*)_tmp831)->f2;_LL439: Cyc_Toc_exptypes_to_c(
_tmp83A);Cyc_Toc_stmttypes_to_c(_tmp83B);goto _LL42F;_LL43A: if(*((int*)_tmp831)!= 
8)goto _LL43C;_tmp83C=((struct Cyc_Absyn_For_s_struct*)_tmp831)->f1;_tmp83D=((
struct Cyc_Absyn_For_s_struct*)_tmp831)->f2;_tmp83E=_tmp83D.f1;_tmp83F=((struct
Cyc_Absyn_For_s_struct*)_tmp831)->f3;_tmp840=_tmp83F.f1;_tmp841=((struct Cyc_Absyn_For_s_struct*)
_tmp831)->f4;_LL43B: Cyc_Toc_exptypes_to_c(_tmp83C);Cyc_Toc_exptypes_to_c(_tmp83E);
Cyc_Toc_exptypes_to_c(_tmp840);Cyc_Toc_stmttypes_to_c(_tmp841);goto _LL42F;_LL43C:
if(*((int*)_tmp831)!= 9)goto _LL43E;_tmp842=((struct Cyc_Absyn_Switch_s_struct*)
_tmp831)->f1;_tmp843=((struct Cyc_Absyn_Switch_s_struct*)_tmp831)->f2;_LL43D: Cyc_Toc_exptypes_to_c(
_tmp842);for(0;_tmp843 != 0;_tmp843=_tmp843->tl){Cyc_Toc_stmttypes_to_c(((struct
Cyc_Absyn_Switch_clause*)_tmp843->hd)->body);}goto _LL42F;_LL43E: if(*((int*)
_tmp831)!= 11)goto _LL440;_tmp844=((struct Cyc_Absyn_Decl_s_struct*)_tmp831)->f1;
_tmp845=((struct Cyc_Absyn_Decl_s_struct*)_tmp831)->f2;_LL43F: Cyc_Toc_decltypes_to_c(
_tmp844);Cyc_Toc_stmttypes_to_c(_tmp845);goto _LL42F;_LL440: if(*((int*)_tmp831)!= 
13)goto _LL442;_tmp846=((struct Cyc_Absyn_Do_s_struct*)_tmp831)->f1;_tmp847=((
struct Cyc_Absyn_Do_s_struct*)_tmp831)->f2;_tmp848=_tmp847.f1;_LL441: Cyc_Toc_stmttypes_to_c(
_tmp846);Cyc_Toc_exptypes_to_c(_tmp848);goto _LL42F;_LL442: if((int)_tmp831 != 0)
goto _LL444;_LL443: goto _LL445;_LL444: if(_tmp831 <= (void*)1)goto _LL446;if(*((int*)
_tmp831)!= 5)goto _LL446;_LL445: goto _LL447;_LL446: if(_tmp831 <= (void*)1)goto
_LL448;if(*((int*)_tmp831)!= 6)goto _LL448;_LL447: goto _LL449;_LL448: if(_tmp831 <= (
void*)1)goto _LL44A;if(*((int*)_tmp831)!= 7)goto _LL44A;_LL449: goto _LL42F;_LL44A:
if(_tmp831 <= (void*)1)goto _LL44C;if(*((int*)_tmp831)!= 10)goto _LL44C;_LL44B: goto
_LL44D;_LL44C: if(_tmp831 <= (void*)1)goto _LL44E;if(*((int*)_tmp831)!= 12)goto
_LL44E;_LL44D: goto _LL44F;_LL44E: if(_tmp831 <= (void*)1)goto _LL450;if(*((int*)
_tmp831)!= 14)goto _LL450;_LL44F: goto _LL451;_LL450: if(_tmp831 <= (void*)1)goto
_LL452;if(*((int*)_tmp831)!= 15)goto _LL452;_LL451: goto _LL453;_LL452: if(_tmp831 <= (
void*)1)goto _LL454;if(*((int*)_tmp831)!= 17)goto _LL454;_LL453: goto _LL455;_LL454:
if(_tmp831 <= (void*)1)goto _LL42F;if(*((int*)_tmp831)!= 16)goto _LL42F;_LL455:({
void*_tmp849=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp84A="Cyclone statement in C code";
_tag_dynforward(_tmp84A,sizeof(char),_get_zero_arr_size_char(_tmp84A,28));}),
_tag_dynforward(_tmp849,sizeof(void*),0));});goto _LL42F;_LL42F:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv))({
void*_tmp84B=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp84C="decls_to_c: not at toplevel!";_tag_dynforward(_tmp84C,sizeof(
char),_get_zero_arr_size_char(_tmp84C,29));}),_tag_dynforward(_tmp84B,sizeof(
void*),0));});{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmp84D=(
void*)d->r;struct Cyc_Absyn_Vardecl*_tmp84E;struct Cyc_Absyn_Fndecl*_tmp84F;struct
Cyc_Absyn_Aggrdecl*_tmp850;struct Cyc_Absyn_Tuniondecl*_tmp851;struct Cyc_Absyn_Enumdecl*
_tmp852;struct Cyc_Absyn_Typedefdecl*_tmp853;struct Cyc_List_List*_tmp854;struct
Cyc_List_List*_tmp855;struct Cyc_List_List*_tmp856;struct Cyc_List_List*_tmp857;
_LL457: if(_tmp84D <= (void*)2)goto _LL467;if(*((int*)_tmp84D)!= 0)goto _LL459;
_tmp84E=((struct Cyc_Absyn_Var_d_struct*)_tmp84D)->f1;_LL458: {struct _tuple1*
_tmp858=_tmp84E->name;if((void*)_tmp84E->sc == (void*)4)_tmp858=({struct _tuple1*
_tmp859=_cycalloc(sizeof(*_tmp859));_tmp859->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp85A;(_tmp85A.Rel_n).tag=1;(_tmp85A.Rel_n).f1=0;
_tmp85A;});_tmp859->f2=(*_tmp858).f2;_tmp859;});if(_tmp84E->initializer != 0){if(
cinclude)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp84E->initializer));
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp84E->initializer));}}
nv=Cyc_Toc_add_varmap(r,nv,_tmp84E->name,Cyc_Absyn_varb_exp(_tmp858,(void*)({
struct Cyc_Absyn_Global_b_struct*_tmp85B=_cycalloc(sizeof(*_tmp85B));_tmp85B[0]=({
struct Cyc_Absyn_Global_b_struct _tmp85C;_tmp85C.tag=0;_tmp85C.f1=_tmp84E;_tmp85C;});
_tmp85B;}),0));_tmp84E->name=_tmp858;(void*)(_tmp84E->sc=(void*)Cyc_Toc_scope_to_c((
void*)_tmp84E->sc));(void*)(_tmp84E->type=(void*)Cyc_Toc_typ_to_c_array((void*)
_tmp84E->type));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp85D=_cycalloc(
sizeof(*_tmp85D));_tmp85D->hd=d;_tmp85D->tl=Cyc_Toc_result_decls;_tmp85D;});goto
_LL456;}_LL459: if(*((int*)_tmp84D)!= 1)goto _LL45B;_tmp84F=((struct Cyc_Absyn_Fn_d_struct*)
_tmp84D)->f1;_LL45A: {struct _tuple1*_tmp85E=_tmp84F->name;if((void*)_tmp84F->sc
== (void*)4){_tmp85E=({struct _tuple1*_tmp85F=_cycalloc(sizeof(*_tmp85F));_tmp85F->f1=(
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp860;(_tmp860.Rel_n).tag=
1;(_tmp860.Rel_n).f1=0;_tmp860;});_tmp85F->f2=(*_tmp85E).f2;_tmp85F;});(void*)(
_tmp84F->sc=(void*)((void*)2));}nv=Cyc_Toc_add_varmap(r,nv,_tmp84F->name,Cyc_Absyn_var_exp(
_tmp85E,0));_tmp84F->name=_tmp85E;Cyc_Toc_fndecl_to_c(nv,_tmp84F,cinclude);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp861=_cycalloc(sizeof(*_tmp861));_tmp861->hd=d;_tmp861->tl=
Cyc_Toc_result_decls;_tmp861;});goto _LL456;}_LL45B: if(*((int*)_tmp84D)!= 2)goto
_LL45D;_LL45C: goto _LL45E;_LL45D: if(*((int*)_tmp84D)!= 3)goto _LL45F;_LL45E:({void*
_tmp862=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp863="letdecl at toplevel";_tag_dynforward(_tmp863,sizeof(char),
_get_zero_arr_size_char(_tmp863,20));}),_tag_dynforward(_tmp862,sizeof(void*),0));});
_LL45F: if(*((int*)_tmp84D)!= 4)goto _LL461;_tmp850=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp84D)->f1;_LL460: Cyc_Toc_aggrdecl_to_c(_tmp850);Cyc_Toc_result_decls=({struct
Cyc_List_List*_tmp864=_cycalloc(sizeof(*_tmp864));_tmp864->hd=d;_tmp864->tl=Cyc_Toc_result_decls;
_tmp864;});goto _LL456;_LL461: if(*((int*)_tmp84D)!= 5)goto _LL463;_tmp851=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp84D)->f1;_LL462: if(_tmp851->is_xtunion)Cyc_Toc_xtuniondecl_to_c(
_tmp851);else{Cyc_Toc_tuniondecl_to_c(_tmp851);}goto _LL456;_LL463: if(*((int*)
_tmp84D)!= 6)goto _LL465;_tmp852=((struct Cyc_Absyn_Enum_d_struct*)_tmp84D)->f1;
_LL464: Cyc_Toc_enumdecl_to_c(nv,_tmp852);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp865=_cycalloc(sizeof(*_tmp865));_tmp865->hd=d;_tmp865->tl=Cyc_Toc_result_decls;
_tmp865;});goto _LL456;_LL465: if(*((int*)_tmp84D)!= 7)goto _LL467;_tmp853=((struct
Cyc_Absyn_Typedef_d_struct*)_tmp84D)->f1;_LL466: _tmp853->name=_tmp853->name;
_tmp853->tvs=0;if(_tmp853->defn != 0)_tmp853->defn=({struct Cyc_Core_Opt*_tmp866=
_cycalloc(sizeof(*_tmp866));_tmp866->v=(void*)Cyc_Toc_typ_to_c_array((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp853->defn))->v);_tmp866;});else{void*_tmp867=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp853->kind))->v;_LL474: if((int)
_tmp867 != 2)goto _LL476;_LL475: _tmp853->defn=({struct Cyc_Core_Opt*_tmp868=
_cycalloc(sizeof(*_tmp868));_tmp868->v=(void*)Cyc_Absyn_void_star_typ();_tmp868;});
goto _LL473;_LL476:;_LL477: _tmp853->defn=({struct Cyc_Core_Opt*_tmp869=_cycalloc(
sizeof(*_tmp869));_tmp869->v=(void*)((void*)0);_tmp869;});goto _LL473;_LL473:;}
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp86A=_cycalloc(sizeof(*_tmp86A));
_tmp86A->hd=d;_tmp86A->tl=Cyc_Toc_result_decls;_tmp86A;});goto _LL456;_LL467: if((
int)_tmp84D != 0)goto _LL469;_LL468: goto _LL46A;_LL469: if((int)_tmp84D != 1)goto
_LL46B;_LL46A: goto _LL456;_LL46B: if(_tmp84D <= (void*)2)goto _LL46D;if(*((int*)
_tmp84D)!= 8)goto _LL46D;_tmp854=((struct Cyc_Absyn_Namespace_d_struct*)_tmp84D)->f2;
_LL46C: _tmp855=_tmp854;goto _LL46E;_LL46D: if(_tmp84D <= (void*)2)goto _LL46F;if(*((
int*)_tmp84D)!= 9)goto _LL46F;_tmp855=((struct Cyc_Absyn_Using_d_struct*)_tmp84D)->f2;
_LL46E: _tmp856=_tmp855;goto _LL470;_LL46F: if(_tmp84D <= (void*)2)goto _LL471;if(*((
int*)_tmp84D)!= 10)goto _LL471;_tmp856=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp84D)->f1;_LL470: nv=Cyc_Toc_decls_to_c(r,nv,_tmp856,top,cinclude);goto _LL456;
_LL471: if(_tmp84D <= (void*)2)goto _LL456;if(*((int*)_tmp84D)!= 11)goto _LL456;
_tmp857=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp84D)->f1;_LL472: nv=Cyc_Toc_decls_to_c(
r,nv,_tmp857,top,1);goto _LL456;_LL456:;}}return nv;}static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmp86C;struct Cyc_Core_NewRegion _tmp86B=Cyc_Core_new_dynregion();
_tmp86C=_tmp86B.dynregion;{struct _DynRegionFrame _tmp86D;struct _RegionHandle*d=
_open_dynregion(& _tmp86D,_tmp86C);Cyc_Toc_toc_state=({struct Cyc_Toc_TocState*
_tmp86E=_cycalloc(sizeof(*_tmp86E));_tmp86E->dyn=(struct _DynRegionHandle*)
_tmp86C;_tmp86E->tuple_types=(struct Cyc_List_List**)({struct Cyc_List_List**
_tmp873=_region_malloc(d,sizeof(*_tmp873));_tmp873[0]=0;_tmp873;});_tmp86E->aggrs_so_far=(
struct Cyc_Dict_Dict*)({struct Cyc_Dict_Dict*_tmp872=_region_malloc(d,sizeof(*
_tmp872));_tmp872[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(
struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);_tmp872;});
_tmp86E->tunions_so_far=(struct Cyc_Set_Set**)({struct Cyc_Set_Set**_tmp871=
_region_malloc(d,sizeof(*_tmp871));_tmp871[0]=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);
_tmp871;});_tmp86E->xtunions_so_far=(struct Cyc_Dict_Dict*)({struct Cyc_Dict_Dict*
_tmp870=_region_malloc(d,sizeof(*_tmp870));_tmp870[0]=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(
d,Cyc_Absyn_qvar_cmp);_tmp870;});_tmp86E->qvar_tags=(struct Cyc_Dict_Dict*)({
struct Cyc_Dict_Dict*_tmp86F=_region_malloc(d,sizeof(*_tmp86F));_tmp86F[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple6*,struct
_tuple6*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);_tmp86F;});_tmp86E->temp_labels=(
struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(
d);_tmp86E;});;_pop_dynregion(d);}Cyc_Toc_result_decls=0;Cyc_Toc_tuple_type_counter=
0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_total_bounds_checks=
0;Cyc_Toc_bounds_checks_eliminated=0;Cyc_Toc_globals=_tag_dynforward(({struct
_dynforward_ptr**_tmp874=_cycalloc(sizeof(struct _dynforward_ptr*)* 50);_tmp874[0]=&
Cyc_Toc__throw_str;_tmp874[1]=& Cyc_Toc_setjmp_str;_tmp874[2]=& Cyc_Toc__push_handler_str;
_tmp874[3]=& Cyc_Toc__pop_handler_str;_tmp874[4]=& Cyc_Toc__exn_thrown_str;_tmp874[
5]=& Cyc_Toc__npop_handler_str;_tmp874[6]=& Cyc_Toc__check_null_str;_tmp874[7]=&
Cyc_Toc__check_known_subscript_null_str;_tmp874[8]=& Cyc_Toc__check_known_subscript_notnull_str;
_tmp874[9]=& Cyc_Toc__check_dynforward_subscript_str;_tmp874[10]=& Cyc_Toc__check_dyneither_subscript_str;
_tmp874[11]=& Cyc_Toc__dynforward_ptr_str;_tmp874[12]=& Cyc_Toc__dyneither_ptr_str;
_tmp874[13]=& Cyc_Toc__tag_dynforward_str;_tmp874[14]=& Cyc_Toc__tag_dyneither_str;
_tmp874[15]=& Cyc_Toc__init_dynforward_ptr_str;_tmp874[16]=& Cyc_Toc__init_dyneither_ptr_str;
_tmp874[17]=& Cyc_Toc__untag_dynforward_ptr_str;_tmp874[18]=& Cyc_Toc__untag_dyneither_ptr_str;
_tmp874[19]=& Cyc_Toc__get_dynforward_size_str;_tmp874[20]=& Cyc_Toc__get_dyneither_size_str;
_tmp874[21]=& Cyc_Toc__get_zero_arr_size_str;_tmp874[22]=& Cyc_Toc__dynforward_ptr_plus_str;
_tmp874[23]=& Cyc_Toc__dyneither_ptr_plus_str;_tmp874[24]=& Cyc_Toc__zero_arr_plus_str;
_tmp874[25]=& Cyc_Toc__dynforward_ptr_inplace_plus_str;_tmp874[26]=& Cyc_Toc__dyneither_ptr_inplace_plus_str;
_tmp874[27]=& Cyc_Toc__zero_arr_inplace_plus_str;_tmp874[28]=& Cyc_Toc__dynforward_ptr_inplace_plus_post_str;
_tmp874[29]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str;_tmp874[30]=& Cyc_Toc__dynforward_to_dyneither_str;
_tmp874[31]=& Cyc_Toc__dyneither_to_dynforward_str;_tmp874[32]=& Cyc_Toc__zero_arr_inplace_plus_post_str;
_tmp874[33]=& Cyc_Toc__cycalloc_str;_tmp874[34]=& Cyc_Toc__cyccalloc_str;_tmp874[
35]=& Cyc_Toc__cycalloc_atomic_str;_tmp874[36]=& Cyc_Toc__cyccalloc_atomic_str;
_tmp874[37]=& Cyc_Toc__region_malloc_str;_tmp874[38]=& Cyc_Toc__region_calloc_str;
_tmp874[39]=& Cyc_Toc__check_times_str;_tmp874[40]=& Cyc_Toc__new_region_str;
_tmp874[41]=& Cyc_Toc__push_region_str;_tmp874[42]=& Cyc_Toc__pop_region_str;
_tmp874[43]=& Cyc_Toc__open_dynregion_str;_tmp874[44]=& Cyc_Toc__push_dynregion_str;
_tmp874[45]=& Cyc_Toc__pop_dynregion_str;_tmp874[46]=& Cyc_Toc__reset_region_str;
_tmp874[47]=& Cyc_Toc__throw_arraybounds_str;_tmp874[48]=& Cyc_Toc__dynforward_ptr_decrease_size_str;
_tmp874[49]=& Cyc_Toc__dyneither_ptr_decrease_size_str;_tmp874;}),sizeof(struct
_dynforward_ptr*),50);}struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{struct _RegionHandle _tmp875=_new_region("start");struct
_RegionHandle*start=& _tmp875;_push_region(start);Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(
start),ds,1,0);{struct _DynRegionHandle*_tmp877;struct Cyc_Toc_TocState _tmp876=*((
struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp877=_tmp876.dyn;Cyc_Core_free_dynregion(
_tmp877);};_pop_region(start);}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
