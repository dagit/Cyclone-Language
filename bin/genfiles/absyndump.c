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
#else
  unsigned used_bytes;
  unsigned wasted_bytes;
#endif
};

struct _DynRegionFrame {
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
};

// A dynamic region is just a region handle.  We have the
// wrapper struct for type abstraction reasons.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
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
extern int _throw_null_fn(const char *filename, unsigned lineno);
extern int _throw_arraybounds_fn(const char *filename, unsigned lineno);
extern int _throw_badalloc_fn(const char *filename, unsigned lineno);
extern int _throw_match_fn(const char *filename, unsigned lineno);
extern int _throw_fn(void* e, const char *filename, unsigned lineno);
extern int _rethrow(void* e);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
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
_check_null_fn(const void *ptr, const char *filename, unsigned lineno) {
  void*_check_null_temp = (void*)(ptr);
  if (!_check_null_temp) _throw_null_fn(filename,lineno);
  return _check_null_temp;
}
#define _check_null(p) (_check_null_fn((p),__FILE__,__LINE__))
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
_check_known_subscript_null_fn(void *ptr, unsigned bound, unsigned elt_sz, unsigned index, const char *filename, unsigned lineno) {
  void*_cks_ptr = (void*)(ptr);
  unsigned _cks_bound = (bound);
  unsigned _cks_elt_sz = (elt_sz);
  unsigned _cks_index = (index);
  if (!_cks_ptr) _throw_null_fn(filename,lineno);
  if (_cks_index >= _cks_bound) _throw_arraybounds_fn(filename,lineno);
  return ((char *)_cks_ptr) + _cks_elt_sz*_cks_index;
}
#define _check_known_subscript_null(p,b,e) (_check_known_subscript_null_fn(p,b,e,__FILE__,__LINE__))
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
_check_known_subscript_notnull_fn(unsigned bound,unsigned index,const char *filename,unsigned lineno) { 
  unsigned _cksnn_bound = (bound); 
  unsigned _cksnn_index = (index); 
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds_fn(filename,lineno); 
  return _cksnn_index;
}
#define _check_known_subscript_notnull(b,i) (_check_known_subscript_notnull_fn(b,i,__FILE__,__LINE__))
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
#define _zero_arr_plus_char_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_short_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_int_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_float_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_double_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_longdouble_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_voidstar_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#else
static _INLINE char *
_zero_arr_plus_char_fn(char *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE short *
_zero_arr_plus_short_fn(short *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE int *
_zero_arr_plus_int_fn(int *orig_x, unsigned int orig_sz, int orig_i, const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE float *
_zero_arr_plus_float_fn(float *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE double *
_zero_arr_plus_double_fn(double *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE long double *
_zero_arr_plus_longdouble_fn(long double *orig_x, unsigned int orig_sz, int orig_i, const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE void *
_zero_arr_plus_voidstar_fn(void **orig_x, unsigned int orig_sz, int orig_i,const char *filename,unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
#endif

#define _zero_arr_plus_char(x,s,i) \
  (_zero_arr_plus_char_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_short(x,s,i) \
  (_zero_arr_plus_short_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_int(x,s,i) \
  (_zero_arr_plus_int_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_float(x,s,i) \
  (_zero_arr_plus_float_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_double(x,s,i) \
  (_zero_arr_plus_double_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_longdouble(x,s,i) \
  (_zero_arr_plus_longdouble_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_voidstar(x,s,i) \
  (_zero_arr_plus_voidstar_fn(x,s,i,__FILE__,__LINE__))


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
   Note that this expands to call _zero_arr_plus_<type>_fn. */
static _INLINE char *
_zero_arr_inplace_plus_char_fn(char **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_char_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_char(x,i) \
  _zero_arr_inplace_plus_char_fn((char **)(x),i,__FILE__,__LINE__)
static _INLINE short *
_zero_arr_inplace_plus_short_fn(short **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_short_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_short(x,i) \
  _zero_arr_inplace_plus_short_fn((short **)(x),i,__FILE__,__LINE__)
static _INLINE int *
_zero_arr_inplace_plus_int(int **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_int_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_int(x,i) \
  _zero_arr_inplace_plus_int_fn((int **)(x),i,__FILE__,__LINE__)
static _INLINE float *
_zero_arr_inplace_plus_float_fn(float **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_float_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_float(x,i) \
  _zero_arr_inplace_plus_float_fn((float **)(x),i,__FILE__,__LINE__)
static _INLINE double *
_zero_arr_inplace_plus_double_fn(double **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_double_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_double(x,i) \
  _zero_arr_inplace_plus_double_fn((double **)(x),i,__FILE__,__LINE__)
static _INLINE long double *
_zero_arr_inplace_plus_longdouble_fn(long double **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_longdouble_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_longdouble(x,i) \
  _zero_arr_inplace_plus_longdouble_fn((long double **)(x),i,__FILE__,__LINE__)
static _INLINE void *
_zero_arr_inplace_plus_voidstar_fn(void ***x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_voidstar_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_voidstar(x,i) \
  _zero_arr_inplace_plus_voidstar_fn((void ***)(x),i,__FILE__,__LINE__)

/* Does in-place increment of a zero-terminated pointer (e.g., x++). */
static _INLINE char *
_zero_arr_inplace_plus_post_char_fn(char **x, int orig_i,const char *filename,unsigned lineno){
  char * _zap_res = *x;
  *x = _zero_arr_plus_char_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_char(x,i) \
  _zero_arr_inplace_plus_post_char_fn((char **)(x),(i),__FILE__,__LINE__)
static _INLINE short *
_zero_arr_inplace_plus_post_short_fn(short **x, int orig_i,const char *filename,unsigned lineno){
  short * _zap_res = *x;
  *x = _zero_arr_plus_short_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_short(x,i) \
  _zero_arr_inplace_plus_post_short_fn((short **)(x),(i),__FILE__,__LINE__)
static _INLINE int *
_zero_arr_inplace_plus_post_int_fn(int **x, int orig_i,const char *filename, unsigned lineno){
  int * _zap_res = *x;
  *x = _zero_arr_plus_int_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_int(x,i) \
  _zero_arr_inplace_plus_post_int_fn((int **)(x),(i),__FILE__,__LINE__)
static _INLINE float *
_zero_arr_inplace_plus_post_float_fn(float **x, int orig_i,const char *filename, unsigned lineno){
  float * _zap_res = *x;
  *x = _zero_arr_plus_float_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_float(x,i) \
  _zero_arr_inplace_plus_post_float_fn((float **)(x),(i),__FILE__,__LINE__)
static _INLINE double *
_zero_arr_inplace_plus_post_double_fn(double **x, int orig_i,const char *filename,unsigned lineno){
  double * _zap_res = *x;
  *x = _zero_arr_plus_double_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_double(x,i) \
  _zero_arr_inplace_plus_post_double_fn((double **)(x),(i),__FILE__,__LINE__)
static _INLINE long double *
_zero_arr_inplace_plus_post_longdouble_fn(long double **x, int orig_i,const char *filename,unsigned lineno){
  long double * _zap_res = *x;
  *x = _zero_arr_plus_longdouble_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_longdouble(x,i) \
  _zero_arr_inplace_plus_post_longdouble_fn((long double **)(x),(i),__FILE__,__LINE__)
static _INLINE void **
_zero_arr_inplace_plus_post_voidstar_fn(void ***x, int orig_i,const char *filename,unsigned lineno){
  void ** _zap_res = *x;
  *x = _zero_arr_plus_voidstar_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_voidstar(x,i) \
  _zero_arr_inplace_plus_post_voidstar_fn((void***)(x),(i),__FILE__,__LINE__)

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
_check_dyneither_subscript_fn(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index,const char *filename, unsigned lineno) {
  struct _dyneither_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  /* JGM: not needed! if (!_cus_arr.base) _throw_null(); */ 
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one)
    _throw_arraybounds_fn(filename,lineno);
  return _cus_ans;
}
#define _check_dyneither_subscript(a,s,i) \
  _check_dyneither_subscript_fn(a,s,i,__FILE__,__LINE__)
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
_untag_dyneither_ptr_fn(struct _dyneither_ptr arr, 
                        unsigned elt_sz,unsigned num_elts,
                        const char *filename, unsigned lineno) {
  struct _dyneither_ptr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)
    _throw_arraybounds_fn(filename,lineno);
  return _curr;
}
#define _untag_dyneither_ptr(a,s,e) \
  _untag_dyneither_ptr_fn(a,s,e,__FILE__,__LINE__)
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

#define _CYC_MAX_REGION_CONST 2
#define _CYC_MIN_ALIGNMENT (sizeof(double))

#ifdef CYC_REGION_PROFILE
extern int rgn_total_bytes;
#endif

static _INLINE void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
  if (r > (struct _RegionHandle *)_CYC_MAX_REGION_CONST && r->curr != 0) { 
#ifdef CYC_NOALIGN
    unsigned s =  orig_s;
#else
    unsigned s =  (orig_s + _CYC_MIN_ALIGNMENT - 1) & (~(_CYC_MIN_ALIGNMENT -1)); 
#endif
    char *result; 
    result = r->offset; 
    if (s <= (r->last_plus_one - result)) {
      r->offset = result + s; 
#ifdef CYC_REGION_PROFILE
    r->curr->free_bytes = r->curr->free_bytes - s;
    rgn_total_bytes += s;
#endif
      return result;
    }
  } 
  return _region_malloc(r,orig_s); 
}

/* FIX?  Not sure if we want to pass filename and lineno in here... */
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
extern void* _profile_region_calloc(struct _RegionHandle *, unsigned,
                                    unsigned,
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
#define _region_calloc(rh,n,t) _profile_region_calloc(rh,n,t,__FILE__,__FUNCTION__,__LINE__)
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

# 35 "core.h"
 typedef char*Cyc_Cstring;
typedef char*Cyc_CstringNN;
typedef struct _dyneither_ptr Cyc_string_t;
# 40
typedef struct _dyneither_ptr Cyc_mstring_t;
# 43
typedef struct _dyneither_ptr*Cyc_stringptr_t;
# 47
typedef struct _dyneither_ptr*Cyc_mstringptr_t;
# 50
typedef char*Cyc_Cbuffer_t;
# 52
typedef char*Cyc_CbufferNN_t;
# 54
typedef struct _dyneither_ptr Cyc_buffer_t;
# 56
typedef struct _dyneither_ptr Cyc_mbuffer_t;
# 59
typedef int Cyc_bool;
# 26 "cycboot.h"
typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 104
int Cyc_fputc(int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 278 "cycboot.h"
int Cyc_file_string_write(struct Cyc___cycFILE*,struct _dyneither_ptr src,int src_offset,int max_count);
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;
# 205
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_region_key_t;
# 211
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_uregion_key_t;
# 216
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_rcregion_key_t;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 299 "core.h"
typedef void*Cyc_Core___cyclone_internal_array_t;
typedef unsigned int Cyc_Core___cyclone_internal_singleton;
# 303
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 308
if(bd >> 20  || sz >> 12)
return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 315
return 0;
return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};struct Cyc_Relations_Reln;
# 77 "absyn.h"
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 86
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 98
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 102
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 105
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
# 110
typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;
typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;
typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;
typedef void*Cyc_Absyn_kindbound_t;
typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;
typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;
typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;
typedef void*Cyc_Absyn_bounds_t;
typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t;
typedef struct Cyc_Absyn_VarargInfo Cyc_Absyn_vararg_info_t;
typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;
typedef struct Cyc_Absyn_DatatypeInfo Cyc_Absyn_datatype_info_t;
typedef struct Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_datatype_field_info_t;
typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;
typedef struct Cyc_Absyn_ArrayInfo Cyc_Absyn_array_info_t;
typedef void*Cyc_Absyn_type_t;typedef void*Cyc_Absyn_rgntype_t;typedef void*Cyc_Absyn_type_opt_t;
typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;
typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;
typedef enum Cyc_Absyn_Incrementor Cyc_Absyn_incrementor_t;
typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
typedef void*Cyc_Absyn_raw_exp_t;
typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_t;typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_opt_t;
typedef void*Cyc_Absyn_raw_stmt_t;
typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_t;typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_opt_t;
typedef void*Cyc_Absyn_raw_pat_t;
typedef struct Cyc_Absyn_Pat*Cyc_Absyn_pat_t;
typedef void*Cyc_Absyn_binding_t;
typedef struct Cyc_Absyn_Switch_clause*Cyc_Absyn_switch_clause_t;
typedef struct Cyc_Absyn_Fndecl*Cyc_Absyn_fndecl_t;
typedef struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_aggrdecl_t;
typedef struct Cyc_Absyn_Datatypefield*Cyc_Absyn_datatypefield_t;
typedef struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_datatypedecl_t;
typedef struct Cyc_Absyn_Typedefdecl*Cyc_Absyn_typedefdecl_t;
typedef struct Cyc_Absyn_Enumfield*Cyc_Absyn_enumfield_t;
typedef struct Cyc_Absyn_Enumdecl*Cyc_Absyn_enumdecl_t;
typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_t;typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_opt_t;
typedef void*Cyc_Absyn_raw_decl_t;
typedef struct Cyc_Absyn_Decl*Cyc_Absyn_decl_t;
typedef void*Cyc_Absyn_designator_t;
typedef void*Cyc_Absyn_absyn_annot_t;
typedef void*Cyc_Absyn_attribute_t;
typedef struct Cyc_List_List*Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Aggrfield*Cyc_Absyn_aggrfield_t;
typedef void*Cyc_Absyn_offsetof_field_t;
typedef struct Cyc_Absyn_MallocInfo Cyc_Absyn_malloc_info_t;
typedef enum Cyc_Absyn_Coercion Cyc_Absyn_coercion_t;
typedef struct Cyc_Absyn_PtrLoc*Cyc_Absyn_ptrloc_t;
# 161
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};
# 182
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 187
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 194
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 214
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 216
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 225
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 286
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 385
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 390
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 461 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 464
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 500
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
# 508
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 528
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 554
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 561
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 579
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 744 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 948
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 958
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 966
void*Cyc_Absyn_compress_kb(void*);
# 971
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 1001
extern void*Cyc_Absyn_bounds_one;
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1083
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1188
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1194
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 56 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 64
struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long);
# 81
extern struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
# 89
struct _tuple11 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple12{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
# 33 "rgnorder.h"
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
# 35
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
# 42
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 48
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 51
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};
# 44 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 55
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};
# 74
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 78
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
# 89
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 101
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 139 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 71
void*Cyc_Tcutil_compress(void*t);
# 27 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};
# 33
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple13{struct _dyneither_ptr f1;unsigned int f2;};
# 35 "absyndump.cyc"
struct _tuple13 Cyc_Lex_xlate_pos(unsigned int);
# 46
static int Cyc_Absyndump_expand_typedefs;
# 50
static int Cyc_Absyndump_qvar_to_Cids;
# 53
static int Cyc_Absyndump_add_cyc_prefix;
# 56
static int Cyc_Absyndump_generate_line_directives;
# 60
static int Cyc_Absyndump_to_VC;
# 62
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs){
Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;
Cyc_Absyndump_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absyndump_to_VC=fs->to_VC;
Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
# 77 "absyndump.cyc"
Cyc_Absynpp_set_params(fs);}
# 80
void Cyc_Absyndump_dumptyp(void*);
void Cyc_Absyndump_dumpntyp(void*t);
void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);
void Cyc_Absyndump_dumpexp_prec(int,struct Cyc_Absyn_Exp*);
void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);
void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*,int expstmt);
void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*,unsigned int,int do_semi);
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*);
void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a);
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(void*),void*);
void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields);
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields);
# 95
typedef struct _dyneither_ptr Cyc_Absyndump_dump_string_t;
void Cyc_Absyndump_dumploc(unsigned int);
# 99
struct Cyc___cycFILE**Cyc_Absyndump_dump_file=& Cyc_stdout;
# 101
void Cyc_Absyndump_ignore(void*x){return;}
# 103
static unsigned int Cyc_Absyndump_pos=0;
static char Cyc_Absyndump_prev_char='x';
# 106
int Cyc_Absyndump_need_space_before(){
switch(Cyc_Absyndump_prev_char){case '{': _LL0:
 goto _LL1;case '}': _LL1:
 goto _LL2;case '(': _LL2:
 goto _LL3;case ')': _LL3:
 goto _LL4;case '[': _LL4:
 goto _LL5;case ']': _LL5:
 goto _LL6;case ';': _LL6:
 goto _LL7;case ',': _LL7:
 goto _LL8;case '=': _LL8:
 goto _LL9;case '?': _LL9:
 goto _LLA;case '!': _LLA:
 goto _LLB;case ' ': _LLB:
 goto _LLC;case '\n': _LLC:
 goto _LLD;case '*': _LLD:
 return 0;default: _LLE:
 return 1;}}
# 126
void Cyc_Absyndump_dump(struct _dyneither_ptr s){
unsigned int sz=Cyc_strlen((struct _dyneither_ptr)s);
# 133
if(Cyc_Absyndump_need_space_before())
Cyc_fputc((int)' ',*Cyc_Absyndump_dump_file);
if(sz >= 1){
Cyc_Absyndump_prev_char=*((const char*)_check_dyneither_subscript(s,sizeof(char),(int)(sz - 1)));
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,(int)sz);}}
# 141
void Cyc_Absyndump_dump_nospace(struct _dyneither_ptr s){
int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);
# 144
if(sz >= 1){
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);
Cyc_Absyndump_prev_char=*((const char*)_check_dyneither_subscript(s,sizeof(char),sz - 1));}}
# 149
void Cyc_Absyndump_dump_char(int c){
# 151
Cyc_fputc(c,*Cyc_Absyndump_dump_file);
Cyc_Absyndump_prev_char=(char)c;}static char _tmpD[1]="";
# 155
void Cyc_Absyndump_dumploc(unsigned int loc){
static struct _dyneither_ptr last_file={_tmpD,_tmpD,_tmpD + 1};
static unsigned int last_line=0;
if(loc == 0)return;
if(!Cyc_Absyndump_generate_line_directives)return;{
# 161
struct _tuple13 _tmp0=Cyc_Lex_xlate_pos(loc);struct _dyneither_ptr _tmp2;unsigned int _tmp3;struct _tuple13 _tmp1=_tmp0;_tmp2=_tmp1.f1;_tmp3=_tmp1.f2;
if((char*)_tmp2.curr == (char*)(_tag_dyneither(0,0,0)).curr  || (char*)_tmp2.curr == (char*)last_file.curr  && _tmp3 == last_line)return;
if((char*)_tmp2.curr == (char*)last_file.curr  && _tmp3 == last_line + 1){
const char*_tmp3D9;void*_tmp3D8;(_tmp3D8=0,Cyc_fprintf(*Cyc_Absyndump_dump_file,((_tmp3D9="\n",_tag_dyneither(_tmp3D9,sizeof(char),2))),_tag_dyneither(_tmp3D8,sizeof(void*),0)));}else{
if((char*)_tmp2.curr == (char*)last_file.curr){
const char*_tmp3DD;void*_tmp3DC[1];struct Cyc_Int_pa_PrintArg_struct _tmp3DB;(_tmp3DB.tag=1,((_tmp3DB.f1=(unsigned long)((int)_tmp3),((_tmp3DC[0]=& _tmp3DB,Cyc_fprintf(*Cyc_Absyndump_dump_file,((_tmp3DD="\n# %d\n",_tag_dyneither(_tmp3DD,sizeof(char),7))),_tag_dyneither(_tmp3DC,sizeof(void*),1)))))));}else{
# 168
const char*_tmp3E2;void*_tmp3E1[2];struct Cyc_Int_pa_PrintArg_struct _tmp3E0;struct Cyc_String_pa_PrintArg_struct _tmp3DF;(_tmp3DF.tag=0,((_tmp3DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2),((_tmp3E0.tag=1,((_tmp3E0.f1=(unsigned long)((int)_tmp3),((_tmp3E1[0]=& _tmp3E0,((_tmp3E1[1]=& _tmp3DF,Cyc_fprintf(*Cyc_Absyndump_dump_file,((_tmp3E2="\n# %d %s\n",_tag_dyneither(_tmp3E2,sizeof(char),10))),_tag_dyneither(_tmp3E1,sizeof(void*),2)))))))))))));}}
# 170
last_file=_tmp2;
last_line=_tmp3;};}
# 176
void Cyc_Absyndump_dump_str(struct _dyneither_ptr*s){
Cyc_Absyndump_dump(*s);}
# 180
void Cyc_Absyndump_dump_semi(){
Cyc_Absyndump_dump_char((int)';');}
# 184
void Cyc_Absyndump_dump_sep(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr sep){
if(l == 0)
return;
for(0;l->tl != 0;l=l->tl){
f(l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 191
f(l->hd);}
# 193
void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr sep){
if(l == 0)
return;
for(0;l->tl != 0;l=l->tl){
f(env,l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 200
f(env,l->hd);}
# 202
void Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 204
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep(f,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 208
void Cyc_Absyndump_group_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 210
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep_c(f,env,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 214
void Cyc_Absyndump_egroup(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 216
if(l != 0)
Cyc_Absyndump_group(f,l,start,end,sep);}
# 222
void Cyc_Absyndump_dumpqvar(struct _tuple0*v){
struct Cyc_List_List*_tmpE=0;
struct _dyneither_ptr**prefix=0;
{union Cyc_Absyn_Nmspace _tmpF=(*v).f1;union Cyc_Absyn_Nmspace _tmp10=_tmpF;struct Cyc_List_List*_tmp11;struct Cyc_List_List*_tmp12;struct Cyc_List_List*_tmp13;_LL11: if((_tmp10.Loc_n).tag != 4)goto _LL13;_LL12:
 _tmp11=0;goto _LL14;_LL13: if((_tmp10.Rel_n).tag != 1)goto _LL15;_tmp11=(struct Cyc_List_List*)(_tmp10.Rel_n).val;_LL14:
# 228
 _tmpE=_tmp11;
goto _LL10;_LL15: if((_tmp10.C_n).tag != 3)goto _LL17;_tmp12=(struct Cyc_List_List*)(_tmp10.C_n).val;_LL16:
# 234
 Cyc_Absyndump_dump_char((int)' ');
goto _LL10;_LL17: if((_tmp10.Abs_n).tag != 2)goto _LL10;_tmp13=(struct Cyc_List_List*)(_tmp10.Abs_n).val;_LL18:
# 237
 if(Cyc_Absyndump_qvar_to_Cids  || Cyc_Absyndump_add_cyc_prefix)
prefix=& Cyc_Absynpp_cyc_stringptr;
_tmpE=_tmp13;
goto _LL10;_LL10:;}
# 242
if(prefix != 0){
Cyc_Absyndump_dump_str(*prefix);
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{
const char*_tmp3E3;Cyc_Absyndump_dump_nospace(((_tmp3E3="::",_tag_dyneither(_tmp3E3,sizeof(char),3))));}}
# 247
if(_tmpE != 0){
Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmpE->hd));
for(_tmpE=_tmpE->tl;_tmpE != 0;_tmpE=_tmpE->tl){
# 252
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{
const char*_tmp3E4;Cyc_Absyndump_dump_nospace(((_tmp3E4="::",_tag_dyneither(_tmp3E4,sizeof(char),3))));}
Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmpE->hd));}
# 256
if(Cyc_Absyndump_qvar_to_Cids){const char*_tmp3E5;Cyc_Absyndump_dump_nospace(((_tmp3E5="_",_tag_dyneither(_tmp3E5,sizeof(char),2))));}else{
const char*_tmp3E6;Cyc_Absyndump_dump_nospace(((_tmp3E6="::",_tag_dyneither(_tmp3E6,sizeof(char),3))));}
Cyc_Absyndump_dump_nospace(*(*v).f2);}else{
# 260
if(prefix != 0)
Cyc_Absyndump_dump_nospace(*(*v).f2);else{
# 263
Cyc_Absyndump_dump_str((*v).f2);}}}
# 267
void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){
if(tq.q_restrict){const char*_tmp3E7;Cyc_Absyndump_dump(((_tmp3E7="restrict",_tag_dyneither(_tmp3E7,sizeof(char),9))));}
if(tq.q_volatile){const char*_tmp3E8;Cyc_Absyndump_dump(((_tmp3E8="volatile",_tag_dyneither(_tmp3E8,sizeof(char),9))));}
if(tq.print_const){const char*_tmp3E9;Cyc_Absyndump_dump(((_tmp3E9="const",_tag_dyneither(_tmp3E9,sizeof(char),6))));}}
# 273
void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc){
switch(sc){case Cyc_Absyn_Static: _LL19:
{const char*_tmp3EA;Cyc_Absyndump_dump(((_tmp3EA="static",_tag_dyneither(_tmp3EA,sizeof(char),7))));}return;case Cyc_Absyn_Public: _LL1A:
 return;case Cyc_Absyn_Extern: _LL1B:
{const char*_tmp3EB;Cyc_Absyndump_dump(((_tmp3EB="extern",_tag_dyneither(_tmp3EB,sizeof(char),7))));}return;case Cyc_Absyn_ExternC: _LL1C:
{const char*_tmp3EC;Cyc_Absyndump_dump(((_tmp3EC="extern \"C\"",_tag_dyneither(_tmp3EC,sizeof(char),11))));}return;case Cyc_Absyn_Abstract: _LL1D:
{const char*_tmp3ED;Cyc_Absyndump_dump(((_tmp3ED="abstract",_tag_dyneither(_tmp3ED,sizeof(char),9))));}return;case Cyc_Absyn_Register: _LL1E:
{const char*_tmp3EE;Cyc_Absyndump_dump(((_tmp3EE="register",_tag_dyneither(_tmp3EE,sizeof(char),9))));}return;}}
# 284
void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp21;enum Cyc_Absyn_AliasQual _tmp22;struct Cyc_Absyn_Kind*_tmp20=ka;_tmp21=_tmp20->kind;_tmp22=_tmp20->aliasqual;
switch(_tmp21){case Cyc_Absyn_AnyKind: _LL20:
# 288
 switch(_tmp22){case Cyc_Absyn_Aliasable: _LL22:
{const char*_tmp3EF;Cyc_Absyndump_dump(((_tmp3EF="A",_tag_dyneither(_tmp3EF,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL23:
{const char*_tmp3F0;Cyc_Absyndump_dump(((_tmp3F0="UA",_tag_dyneither(_tmp3F0,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL24:
{const char*_tmp3F1;Cyc_Absyndump_dump(((_tmp3F1="TA",_tag_dyneither(_tmp3F1,sizeof(char),3))));}return;}case Cyc_Absyn_MemKind: _LL21:
# 294
 switch(_tmp22){case Cyc_Absyn_Aliasable: _LL27:
{const char*_tmp3F2;Cyc_Absyndump_dump(((_tmp3F2="M",_tag_dyneither(_tmp3F2,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL28:
{const char*_tmp3F3;Cyc_Absyndump_dump(((_tmp3F3="UM",_tag_dyneither(_tmp3F3,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL29:
{const char*_tmp3F4;Cyc_Absyndump_dump(((_tmp3F4="TM",_tag_dyneither(_tmp3F4,sizeof(char),3))));}return;}case Cyc_Absyn_BoxKind: _LL26:
# 300
 switch(_tmp22){case Cyc_Absyn_Aliasable: _LL2C:
{const char*_tmp3F5;Cyc_Absyndump_dump(((_tmp3F5="B",_tag_dyneither(_tmp3F5,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL2D:
{const char*_tmp3F6;Cyc_Absyndump_dump(((_tmp3F6="UB",_tag_dyneither(_tmp3F6,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL2E:
{const char*_tmp3F7;Cyc_Absyndump_dump(((_tmp3F7="TB",_tag_dyneither(_tmp3F7,sizeof(char),3))));}return;}case Cyc_Absyn_RgnKind: _LL2B:
# 306
 switch(_tmp22){case Cyc_Absyn_Aliasable: _LL31:
{const char*_tmp3F8;Cyc_Absyndump_dump(((_tmp3F8="R",_tag_dyneither(_tmp3F8,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL32:
{const char*_tmp3F9;Cyc_Absyndump_dump(((_tmp3F9="UR",_tag_dyneither(_tmp3F9,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL33:
{const char*_tmp3FA;Cyc_Absyndump_dump(((_tmp3FA="TR",_tag_dyneither(_tmp3FA,sizeof(char),3))));}return;}case Cyc_Absyn_EffKind: _LL30:
# 311
{const char*_tmp3FB;Cyc_Absyndump_dump(((_tmp3FB="E",_tag_dyneither(_tmp3FB,sizeof(char),2))));}return;case Cyc_Absyn_IntKind: _LL35:
{const char*_tmp3FC;Cyc_Absyndump_dump(((_tmp3FC="I",_tag_dyneither(_tmp3FC,sizeof(char),2))));}return;}}
# 316
void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind k){
switch(k){case Cyc_Absyn_StructA: _LL37:
{const char*_tmp3FD;Cyc_Absyndump_dump(((_tmp3FD="struct ",_tag_dyneither(_tmp3FD,sizeof(char),8))));}return;case Cyc_Absyn_UnionA: _LL38:
{const char*_tmp3FE;Cyc_Absyndump_dump(((_tmp3FE="union ",_tag_dyneither(_tmp3FE,sizeof(char),7))));}return;}}
# 323
void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){
const char*_tmp401;const char*_tmp400;const char*_tmp3FF;((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptyp,ts,((_tmp3FF="<",_tag_dyneither(_tmp3FF,sizeof(char),2))),((_tmp400=">",_tag_dyneither(_tmp400,sizeof(char),2))),((_tmp401=",",_tag_dyneither(_tmp401,sizeof(char),2))));}
# 326
void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr n=*tv->name;
if(*((const char*)_check_dyneither_subscript(n,sizeof(char),0))== '#'){
{const char*_tmp402;Cyc_Absyndump_dump(((_tmp402="`G",_tag_dyneither(_tmp402,sizeof(char),3))));}
{void*_tmp37=Cyc_Absyn_compress_kb(tv->kind);void*_tmp38=_tmp37;struct Cyc_Absyn_Kind*_tmp3A;struct Cyc_Absyn_Kind*_tmp3C;_LL3B: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp39=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp38;if(_tmp39->tag != 0)goto _LL3D;else{_tmp3A=_tmp39->f1;}}_LL3C:
 _tmp3C=_tmp3A;goto _LL3E;_LL3D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp3B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp38;if(_tmp3B->tag != 2)goto _LL3F;else{_tmp3C=_tmp3B->f2;}}_LL3E:
 Cyc_Absyndump_dumpkind(_tmp3C);goto _LL3A;_LL3F:;_LL40:
{const char*_tmp403;Cyc_Absyndump_dump_nospace(((_tmp403="K",_tag_dyneither(_tmp403,sizeof(char),2))));}goto _LL3A;_LL3A:;}
# 335
Cyc_Absyndump_dump_nospace(_dyneither_ptr_plus(n,sizeof(char),1));}else{
# 337
Cyc_Absyndump_dump(n);}}
# 339
void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){
Cyc_Absyndump_dumptvar(tv);{
void*_tmp3E=Cyc_Absyn_compress_kb(tv->kind);void*_tmp3F=_tmp3E;struct Cyc_Absyn_Kind*_tmp42;struct Cyc_Absyn_Kind*_tmp45;_LL42: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp40=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp3F;if(_tmp40->tag != 1)goto _LL44;}_LL43:
 goto _LL41;_LL44: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp41=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp3F;if(_tmp41->tag != 2)goto _LL46;else{_tmp42=_tmp41->f2;}}_LL45:
 goto _LL41;_LL46: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp43=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp3F;if(_tmp43->tag != 0)goto _LL48;else{if((_tmp43->f1)->kind != Cyc_Absyn_BoxKind)goto _LL48;if((_tmp43->f1)->aliasqual != Cyc_Absyn_Aliasable)goto _LL48;}}_LL47:
 goto _LL41;_LL48: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp44=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp3F;if(_tmp44->tag != 0)goto _LL41;else{_tmp45=_tmp44->f1;}}_LL49:
{const char*_tmp404;Cyc_Absyndump_dump(((_tmp404="::",_tag_dyneither(_tmp404,sizeof(char),3))));}Cyc_Absyndump_dumpkind(_tmp45);goto _LL41;_LL41:;};}
# 348
void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){
const char*_tmp407;const char*_tmp406;const char*_tmp405;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,tvs,((_tmp405="<",_tag_dyneither(_tmp405,sizeof(char),2))),((_tmp406=">",_tag_dyneither(_tmp406,sizeof(char),2))),((_tmp407=",",_tag_dyneither(_tmp407,sizeof(char),2))));}
# 351
void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){
const char*_tmp40A;const char*_tmp409;const char*_tmp408;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,tvs,((_tmp408="<",_tag_dyneither(_tmp408,sizeof(char),2))),((_tmp409=">",_tag_dyneither(_tmp409,sizeof(char),2))),((_tmp40A=",",_tag_dyneither(_tmp40A,sizeof(char),2))));}struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;};
# 355
void Cyc_Absyndump_dumparg(struct _tuple14*pr){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 358
void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){
const char*_tmp40D;const char*_tmp40C;const char*_tmp40B;((void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,ts,((_tmp40B="(",_tag_dyneither(_tmp40B,sizeof(char),2))),((_tmp40C=")",_tag_dyneither(_tmp40C,sizeof(char),2))),((_tmp40D=",",_tag_dyneither(_tmp40D,sizeof(char),2))));}
# 362
void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp50=(void*)atts->hd;void*_tmp51=_tmp50;_LL4B: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp52=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp51;if(_tmp52->tag != 1)goto _LL4D;}_LL4C:
{const char*_tmp40E;Cyc_Absyndump_dump(((_tmp40E="_stdcall",_tag_dyneither(_tmp40E,sizeof(char),9))));}return;_LL4D: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp53=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp51;if(_tmp53->tag != 2)goto _LL4F;}_LL4E:
{const char*_tmp40F;Cyc_Absyndump_dump(((_tmp40F="_cdecl",_tag_dyneither(_tmp40F,sizeof(char),7))));}return;_LL4F: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp54=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp51;if(_tmp54->tag != 3)goto _LL51;}_LL50:
{const char*_tmp410;Cyc_Absyndump_dump(((_tmp410="_fastcall",_tag_dyneither(_tmp410,sizeof(char),10))));}return;_LL51:;_LL52:
 goto _LL4A;_LL4A:;}}
# 372
void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){
# 374
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp58=(void*)((struct Cyc_List_List*)_check_null(atts))->hd;void*_tmp59=_tmp58;_LL54: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp5A=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp59;if(_tmp5A->tag != 1)goto _LL56;}_LL55:
 goto _LL57;_LL56: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp5B=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp59;if(_tmp5B->tag != 2)goto _LL58;}_LL57:
 goto _LL59;_LL58: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp5C=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp59;if(_tmp5C->tag != 3)goto _LL5A;}_LL59:
 goto _LL53;_LL5A:;_LL5B:
 hasatt=1;goto _LL53;_LL53:;}}
# 382
if(!hasatt)
return;
{const char*_tmp411;Cyc_Absyndump_dump(((_tmp411="__declspec(",_tag_dyneither(_tmp411,sizeof(char),12))));}
for(0;atts != 0;atts=atts->tl){
void*_tmp5E=(void*)atts->hd;void*_tmp5F=_tmp5E;_LL5D: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp60=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp5F;if(_tmp60->tag != 1)goto _LL5F;}_LL5E:
 goto _LL60;_LL5F: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp61=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp5F;if(_tmp61->tag != 2)goto _LL61;}_LL60:
 goto _LL62;_LL61: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp62=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp5F;if(_tmp62->tag != 3)goto _LL63;}_LL62:
 goto _LL5C;_LL63:;_LL64:
 Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL5C;_LL5C:;}
# 392
Cyc_Absyndump_dump_char((int)')');}
# 395
void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){
if(atts == 0)return;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL65:
# 399
{const char*_tmp412;Cyc_Absyndump_dump(((_tmp412=" __attribute__((",_tag_dyneither(_tmp412,sizeof(char),17))));}
for(0;atts != 0;atts=atts->tl){
Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));
if(atts->tl != 0){const char*_tmp413;Cyc_Absyndump_dump(((_tmp413=",",_tag_dyneither(_tmp413,sizeof(char),2))));}}
# 404
{const char*_tmp414;Cyc_Absyndump_dump(((_tmp414=")) ",_tag_dyneither(_tmp414,sizeof(char),4))));}
return;case Cyc_Cyclone_Vc_c: _LL66:
# 407
 Cyc_Absyndump_dump_noncallconv(atts);
return;}}
# 412
int Cyc_Absyndump_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmp66=(void*)tms->hd;void*_tmp67=_tmp66;_LL69: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp68=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp67;if(_tmp68->tag != 2)goto _LL6B;}_LL6A:
 return 1;_LL6B:;_LL6C:
 return 0;_LL68:;};}
# 420
static void Cyc_Absyndump_dumprgn(void*t){
void*_tmp69=Cyc_Tcutil_compress(t);void*_tmp6A=_tmp69;_LL6E: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp6B=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp6A;if(_tmp6B->tag != 20)goto _LL70;}_LL6F:
{const char*_tmp415;Cyc_Absyndump_dump(((_tmp415="`H",_tag_dyneither(_tmp415,sizeof(char),3))));}goto _LL6D;_LL70:;_LL71:
 Cyc_Absyndump_dumpntyp(t);goto _LL6D;_LL6D:;}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 427
static struct _tuple15 Cyc_Absyndump_effects_split(void*t){
struct Cyc_List_List*rgions=0;
struct Cyc_List_List*effects=0;
# 431
{void*_tmp6D=Cyc_Tcutil_compress(t);void*_tmp6E=_tmp6D;void*_tmp70;struct Cyc_List_List*_tmp72;_LL73: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp6F=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp6E;if(_tmp6F->tag != 23)goto _LL75;else{_tmp70=(void*)_tmp6F->f1;}}_LL74:
{struct Cyc_List_List*_tmp416;rgions=((_tmp416=_cycalloc(sizeof(*_tmp416)),((_tmp416->hd=_tmp70,((_tmp416->tl=rgions,_tmp416))))));}goto _LL72;_LL75: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp71=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp6E;if(_tmp71->tag != 24)goto _LL77;else{_tmp72=_tmp71->f1;}}_LL76:
# 434
 for(0;_tmp72 != 0;_tmp72=_tmp72->tl){
struct _tuple15 _tmp74=Cyc_Absyndump_effects_split((void*)_tmp72->hd);struct Cyc_List_List*_tmp76;struct Cyc_List_List*_tmp77;struct _tuple15 _tmp75=_tmp74;_tmp76=_tmp75.f1;_tmp77=_tmp75.f2;
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp76,rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp77,effects);}
# 439
goto _LL72;_LL77:;_LL78:
{struct Cyc_List_List*_tmp417;effects=((_tmp417=_cycalloc(sizeof(*_tmp417)),((_tmp417->hd=t,((_tmp417->tl=effects,_tmp417))))));}goto _LL72;_LL72:;}{
# 442
struct _tuple15 _tmp418;return(_tmp418.f1=rgions,((_tmp418.f2=effects,_tmp418)));};}
# 445
static void Cyc_Absyndump_dumpeff(void*t){
struct _tuple15 _tmp7A=Cyc_Absyndump_effects_split(t);struct Cyc_List_List*_tmp7C;struct Cyc_List_List*_tmp7D;struct _tuple15 _tmp7B=_tmp7A;_tmp7C=_tmp7B.f1;_tmp7D=_tmp7B.f2;
_tmp7C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp7C);
_tmp7D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp7D);
if(_tmp7D != 0){
{struct Cyc_List_List*_tmp7E=_tmp7D;for(0;_tmp7E != 0;_tmp7E=_tmp7E->tl){
Cyc_Absyndump_dumpntyp((void*)_tmp7E->hd);
if(_tmp7E->tl != 0)
Cyc_Absyndump_dump_char((int)'+');}}
# 455
if(_tmp7C != 0)Cyc_Absyndump_dump_char((int)'+');}
# 457
if(_tmp7C != 0  || _tmp7D == 0){
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp7C != 0;_tmp7C=_tmp7C->tl){
Cyc_Absyndump_dumprgn((void*)_tmp7C->hd);
if(_tmp7C->tl != 0)Cyc_Absyndump_dump_char((int)',');}
# 463
Cyc_Absyndump_dump_char((int)'}');}}
# 467
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*);
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*);
# 472
void Cyc_Absyndump_dumpntyp(void*t){
void*_tmp7F=t;struct Cyc_Absyn_Tvar*_tmp85;struct Cyc_Core_Opt*_tmp87;int _tmp88;struct Cyc_Core_Opt*_tmp8A;void*_tmp8B;int _tmp8C;union Cyc_Absyn_DatatypeInfoU _tmp8E;struct Cyc_List_List*_tmp8F;union Cyc_Absyn_DatatypeFieldInfoU _tmp91;struct Cyc_List_List*_tmp92;int _tmpA3;struct Cyc_List_List*_tmpA5;union Cyc_Absyn_AggrInfoU _tmpA7;struct Cyc_List_List*_tmpA8;enum Cyc_Absyn_AggrKind _tmpAA;struct Cyc_List_List*_tmpAB;struct _tuple0*_tmpAD;struct Cyc_List_List*_tmpAF;struct _tuple0*_tmpB1;struct Cyc_List_List*_tmpB2;struct Cyc_Absyn_Exp*_tmpB4;struct Cyc_Absyn_Exp*_tmpB6;struct _dyneither_ptr _tmpB8;void*_tmpBA;void*_tmpBC;void*_tmpBD;void*_tmpBF;struct Cyc_Absyn_Aggrdecl*_tmpC4;struct Cyc_Absyn_Enumdecl*_tmpC7;struct Cyc_Absyn_Datatypedecl*_tmpCA;void*_tmpCD;void*_tmpCF;struct Cyc_List_List*_tmpD1;_LL7A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp80=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7F;if(_tmp80->tag != 8)goto _LL7C;}_LL7B:
# 475
 goto _LL7D;_LL7C: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp81=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp7F;if(_tmp81->tag != 9)goto _LL7E;}_LL7D:
 goto _LL7F;_LL7E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp82=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7F;if(_tmp82->tag != 5)goto _LL80;}_LL7F:
 return;_LL80: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp83=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp7F;if(_tmp83->tag != 0)goto _LL82;}_LL81:
{const char*_tmp419;Cyc_Absyndump_dump(((_tmp419="void",_tag_dyneither(_tmp419,sizeof(char),5))));}return;_LL82: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp84=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7F;if(_tmp84->tag != 2)goto _LL84;else{_tmp85=_tmp84->f1;}}_LL83:
 Cyc_Absyndump_dumptvar(_tmp85);return;_LL84: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp86=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp7F;if(_tmp86->tag != 1)goto _LL86;else{_tmp87=_tmp86->f1;if((void*)_tmp86->f2 != 0)goto _LL86;_tmp88=_tmp86->f3;}}_LL85:
# 481
{const char*_tmp41A;Cyc_Absyndump_dump(((_tmp41A="`E",_tag_dyneither(_tmp41A,sizeof(char),3))));}
if(_tmp87 == 0){const char*_tmp41B;Cyc_Absyndump_dump(((_tmp41B="K",_tag_dyneither(_tmp41B,sizeof(char),2))));}else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)_tmp87->v);}
{const char*_tmp41F;void*_tmp41E[1];struct Cyc_Int_pa_PrintArg_struct _tmp41D;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp41D.tag=1,((_tmp41D.f1=(unsigned long)_tmp88,((_tmp41E[0]=& _tmp41D,Cyc_aprintf(((_tmp41F="%d",_tag_dyneither(_tmp41F,sizeof(char),3))),_tag_dyneither(_tmp41E,sizeof(void*),1)))))))));}return;_LL86: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp89=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp7F;if(_tmp89->tag != 1)goto _LL88;else{_tmp8A=_tmp89->f1;_tmp8B=(void*)_tmp89->f2;_tmp8C=_tmp89->f3;}}_LL87:
 Cyc_Absyndump_dumpntyp((void*)_check_null(_tmp8B));return;_LL88: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp8D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp7F;if(_tmp8D->tag != 3)goto _LL8A;else{_tmp8E=(_tmp8D->f1).datatype_info;_tmp8F=(_tmp8D->f1).targs;}}_LL89:
# 486
{union Cyc_Absyn_DatatypeInfoU _tmpD8=_tmp8E;struct _tuple0*_tmpD9;int _tmpDA;struct _tuple0*_tmpDB;int _tmpDC;_LLD7: if((_tmpD8.UnknownDatatype).tag != 1)goto _LLD9;_tmpD9=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpD8.UnknownDatatype).val).name;_tmpDA=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpD8.UnknownDatatype).val).is_extensible;_LLD8:
 _tmpDB=_tmpD9;_tmpDC=_tmpDA;goto _LLDA;_LLD9: if((_tmpD8.KnownDatatype).tag != 2)goto _LLD6;_tmpDB=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpD8.KnownDatatype).val))->name;_tmpDC=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpD8.KnownDatatype).val))->is_extensible;_LLDA:
# 489
 if(_tmpDC){const char*_tmp420;Cyc_Absyndump_dump(((_tmp420="@extensible ",_tag_dyneither(_tmp420,sizeof(char),13))));}
{const char*_tmp421;Cyc_Absyndump_dump(((_tmp421="datatype ",_tag_dyneither(_tmp421,sizeof(char),10))));}
Cyc_Absyndump_dumpqvar(_tmpDB);
Cyc_Absyndump_dumptps(_tmp8F);
goto _LLD6;_LLD6:;}
# 495
goto _LL79;_LL8A: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp90=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp7F;if(_tmp90->tag != 4)goto _LL8C;else{_tmp91=(_tmp90->f1).field_info;_tmp92=(_tmp90->f1).targs;}}_LL8B:
# 497
{union Cyc_Absyn_DatatypeFieldInfoU _tmpDF=_tmp91;struct _tuple0*_tmpE0;struct _tuple0*_tmpE1;int _tmpE2;struct _tuple0*_tmpE3;int _tmpE4;struct _tuple0*_tmpE5;_LLDC: if((_tmpDF.UnknownDatatypefield).tag != 1)goto _LLDE;_tmpE0=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpDF.UnknownDatatypefield).val).datatype_name;_tmpE1=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpDF.UnknownDatatypefield).val).field_name;_tmpE2=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpDF.UnknownDatatypefield).val).is_extensible;_LLDD:
# 499
 _tmpE3=_tmpE0;_tmpE4=_tmpE2;_tmpE5=_tmpE1;goto _LLDF;_LLDE: if((_tmpDF.KnownDatatypefield).tag != 2)goto _LLDB;_tmpE3=(((struct _tuple1)(_tmpDF.KnownDatatypefield).val).f1)->name;_tmpE4=(((struct _tuple1)(_tmpDF.KnownDatatypefield).val).f1)->is_extensible;_tmpE5=(((struct _tuple1)(_tmpDF.KnownDatatypefield).val).f2)->name;_LLDF:
# 502
 if(_tmpE4){const char*_tmp422;Cyc_Absyndump_dump(((_tmp422="@extensible ",_tag_dyneither(_tmp422,sizeof(char),13))));}
{const char*_tmp423;Cyc_Absyndump_dump(((_tmp423="datatype ",_tag_dyneither(_tmp423,sizeof(char),10))));}
Cyc_Absyndump_dumpqvar(_tmpE3);
{const char*_tmp424;Cyc_Absyndump_dump(((_tmp424=".",_tag_dyneither(_tmp424,sizeof(char),2))));}
Cyc_Absyndump_dumpqvar(_tmpE5);
Cyc_Absyndump_dumptps(_tmp92);
goto _LLDB;_LLDB:;}
# 510
goto _LL79;_LL8C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp93=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F;if(_tmp93->tag != 6)goto _LL8E;else{if(_tmp93->f1 != Cyc_Absyn_None)goto _LL8E;if(_tmp93->f2 != Cyc_Absyn_Int_sz)goto _LL8E;}}_LL8D:
 goto _LL8F;_LL8E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp94=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F;if(_tmp94->tag != 6)goto _LL90;else{if(_tmp94->f1 != Cyc_Absyn_Signed)goto _LL90;if(_tmp94->f2 != Cyc_Absyn_Int_sz)goto _LL90;}}_LL8F:
{const char*_tmp425;Cyc_Absyndump_dump(((_tmp425="int",_tag_dyneither(_tmp425,sizeof(char),4))));}return;_LL90: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp95=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F;if(_tmp95->tag != 6)goto _LL92;else{if(_tmp95->f1 != Cyc_Absyn_None)goto _LL92;if(_tmp95->f2 != Cyc_Absyn_Long_sz)goto _LL92;}}_LL91:
 goto _LL93;_LL92: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp96=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F;if(_tmp96->tag != 6)goto _LL94;else{if(_tmp96->f1 != Cyc_Absyn_Signed)goto _LL94;if(_tmp96->f2 != Cyc_Absyn_Long_sz)goto _LL94;}}_LL93:
{const char*_tmp426;Cyc_Absyndump_dump(((_tmp426="long",_tag_dyneither(_tmp426,sizeof(char),5))));}return;_LL94: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp97=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F;if(_tmp97->tag != 6)goto _LL96;else{if(_tmp97->f1 != Cyc_Absyn_None)goto _LL96;if(_tmp97->f2 != Cyc_Absyn_Char_sz)goto _LL96;}}_LL95:
{const char*_tmp427;Cyc_Absyndump_dump(((_tmp427="char",_tag_dyneither(_tmp427,sizeof(char),5))));}return;_LL96: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp98=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F;if(_tmp98->tag != 6)goto _LL98;else{if(_tmp98->f1 != Cyc_Absyn_Signed)goto _LL98;if(_tmp98->f2 != Cyc_Absyn_Char_sz)goto _LL98;}}_LL97:
{const char*_tmp428;Cyc_Absyndump_dump(((_tmp428="signed char",_tag_dyneither(_tmp428,sizeof(char),12))));}return;_LL98: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp99=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F;if(_tmp99->tag != 6)goto _LL9A;else{if(_tmp99->f1 != Cyc_Absyn_Unsigned)goto _LL9A;if(_tmp99->f2 != Cyc_Absyn_Char_sz)goto _LL9A;}}_LL99:
{const char*_tmp429;Cyc_Absyndump_dump(((_tmp429="unsigned char",_tag_dyneither(_tmp429,sizeof(char),14))));}return;_LL9A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F;if(_tmp9A->tag != 6)goto _LL9C;else{if(_tmp9A->f1 != Cyc_Absyn_None)goto _LL9C;if(_tmp9A->f2 != Cyc_Absyn_Short_sz)goto _LL9C;}}_LL9B:
 goto _LL9D;_LL9C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F;if(_tmp9B->tag != 6)goto _LL9E;else{if(_tmp9B->f1 != Cyc_Absyn_Signed)goto _LL9E;if(_tmp9B->f2 != Cyc_Absyn_Short_sz)goto _LL9E;}}_LL9D:
{const char*_tmp42A;Cyc_Absyndump_dump(((_tmp42A="short",_tag_dyneither(_tmp42A,sizeof(char),6))));}return;_LL9E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F;if(_tmp9C->tag != 6)goto _LLA0;else{if(_tmp9C->f1 != Cyc_Absyn_Unsigned)goto _LLA0;if(_tmp9C->f2 != Cyc_Absyn_Short_sz)goto _LLA0;}}_LL9F:
{const char*_tmp42B;Cyc_Absyndump_dump(((_tmp42B="unsigned short",_tag_dyneither(_tmp42B,sizeof(char),15))));}return;_LLA0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F;if(_tmp9D->tag != 6)goto _LLA2;else{if(_tmp9D->f1 != Cyc_Absyn_Unsigned)goto _LLA2;if(_tmp9D->f2 != Cyc_Absyn_Int_sz)goto _LLA2;}}_LLA1:
{const char*_tmp42C;Cyc_Absyndump_dump(((_tmp42C="unsigned int",_tag_dyneither(_tmp42C,sizeof(char),13))));}return;_LLA2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F;if(_tmp9E->tag != 6)goto _LLA4;else{if(_tmp9E->f1 != Cyc_Absyn_Unsigned)goto _LLA4;if(_tmp9E->f2 != Cyc_Absyn_Long_sz)goto _LLA4;}}_LLA3:
{const char*_tmp42D;Cyc_Absyndump_dump(((_tmp42D="unsigned long",_tag_dyneither(_tmp42D,sizeof(char),14))));}return;_LLA4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F;if(_tmp9F->tag != 6)goto _LLA6;else{if(_tmp9F->f1 != Cyc_Absyn_None)goto _LLA6;if(_tmp9F->f2 != Cyc_Absyn_LongLong_sz)goto _LLA6;}}_LLA5:
 goto _LLA7;_LLA6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F;if(_tmpA0->tag != 6)goto _LLA8;else{if(_tmpA0->f1 != Cyc_Absyn_Signed)goto _LLA8;if(_tmpA0->f2 != Cyc_Absyn_LongLong_sz)goto _LLA8;}}_LLA7:
# 525
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLE0:
{const char*_tmp42E;Cyc_Absyndump_dump(((_tmp42E="long long",_tag_dyneither(_tmp42E,sizeof(char),10))));}return;case Cyc_Cyclone_Vc_c: _LLE1:
{const char*_tmp42F;Cyc_Absyndump_dump(((_tmp42F="__int64",_tag_dyneither(_tmp42F,sizeof(char),8))));}return;}_LLA8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F;if(_tmpA1->tag != 6)goto _LLAA;else{if(_tmpA1->f1 != Cyc_Absyn_Unsigned)goto _LLAA;if(_tmpA1->f2 != Cyc_Absyn_LongLong_sz)goto _LLAA;}}_LLA9:
# 530
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LLE3:
{const char*_tmp430;Cyc_Absyndump_dump(((_tmp430="unsigned __int64",_tag_dyneither(_tmp430,sizeof(char),17))));}return;case Cyc_Cyclone_Gcc_c: _LLE4:
{const char*_tmp431;Cyc_Absyndump_dump(((_tmp431="unsigned long long",_tag_dyneither(_tmp431,sizeof(char),19))));}return;}_LLAA: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpA2=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7F;if(_tmpA2->tag != 7)goto _LLAC;else{_tmpA3=_tmpA2->f1;}}_LLAB:
# 535
 if(_tmpA3 == 0){
const char*_tmp432;Cyc_Absyndump_dump(((_tmp432="float",_tag_dyneither(_tmp432,sizeof(char),6))));}else{
if(_tmpA3 == 1){
const char*_tmp433;Cyc_Absyndump_dump(((_tmp433="double",_tag_dyneither(_tmp433,sizeof(char),7))));}else{
# 540
const char*_tmp434;Cyc_Absyndump_dump(((_tmp434="long double",_tag_dyneither(_tmp434,sizeof(char),12))));}}
return;_LLAC: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp7F;if(_tmpA4->tag != 10)goto _LLAE;else{_tmpA5=_tmpA4->f1;}}_LLAD:
 Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(_tmpA5);return;_LLAE: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpA6=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp7F;if(_tmpA6->tag != 11)goto _LLB0;else{_tmpA7=(_tmpA6->f1).aggr_info;_tmpA8=(_tmpA6->f1).targs;}}_LLAF: {
# 544
struct _tuple10 _tmpF9=Cyc_Absyn_aggr_kinded_name(_tmpA7);enum Cyc_Absyn_AggrKind _tmpFB;struct _tuple0*_tmpFC;struct _tuple10 _tmpFA=_tmpF9;_tmpFB=_tmpFA.f1;_tmpFC=_tmpFA.f2;
Cyc_Absyndump_dumpaggr_kind(_tmpFB);Cyc_Absyndump_dumpqvar(_tmpFC);Cyc_Absyndump_dumptps(_tmpA8);
return;}_LLB0: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA9=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp7F;if(_tmpA9->tag != 12)goto _LLB2;else{_tmpAA=_tmpA9->f1;_tmpAB=_tmpA9->f2;}}_LLB1:
# 548
 Cyc_Absyndump_dumpaggr_kind(_tmpAA);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(_tmpAB);Cyc_Absyndump_dump_char((int)'}');return;_LLB2: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp7F;if(_tmpAC->tag != 13)goto _LLB4;else{_tmpAD=_tmpAC->f1;}}_LLB3:
{const char*_tmp435;Cyc_Absyndump_dump(((_tmp435="enum ",_tag_dyneither(_tmp435,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(_tmpAD);return;_LLB4: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp7F;if(_tmpAE->tag != 14)goto _LLB6;else{_tmpAF=_tmpAE->f1;}}_LLB5:
{const char*_tmp436;Cyc_Absyndump_dump(((_tmp436="enum {",_tag_dyneither(_tmp436,sizeof(char),7))));}Cyc_Absyndump_dumpenumfields(_tmpAF);{const char*_tmp437;Cyc_Absyndump_dump(((_tmp437="}",_tag_dyneither(_tmp437,sizeof(char),2))));}return;_LLB6: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB0=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp7F;if(_tmpB0->tag != 17)goto _LLB8;else{_tmpB1=_tmpB0->f1;_tmpB2=_tmpB0->f2;}}_LLB7:
(Cyc_Absyndump_dumpqvar(_tmpB1),Cyc_Absyndump_dumptps(_tmpB2));return;_LLB8: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpB3=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp7F;if(_tmpB3->tag != 18)goto _LLBA;else{_tmpB4=_tmpB3->f1;}}_LLB9:
{const char*_tmp438;Cyc_Absyndump_dump(((_tmp438="valueof_t(",_tag_dyneither(_tmp438,sizeof(char),11))));}Cyc_Absyndump_dumpexp(_tmpB4);{const char*_tmp439;Cyc_Absyndump_dump(((_tmp439=")",_tag_dyneither(_tmp439,sizeof(char),2))));}return;_LLBA: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpB5=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp7F;if(_tmpB5->tag != 27)goto _LLBC;else{_tmpB6=_tmpB5->f1;}}_LLBB:
{const char*_tmp43A;Cyc_Absyndump_dump(((_tmp43A="typeof(",_tag_dyneither(_tmp43A,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmpB6);{const char*_tmp43B;Cyc_Absyndump_dump(((_tmp43B=")",_tag_dyneither(_tmp43B,sizeof(char),2))));}return;_LLBC: {struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmpB7=(struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp7F;if(_tmpB7->tag != 28)goto _LLBE;else{_tmpB8=_tmpB7->f1;}}_LLBD:
 Cyc_Absyndump_dump(_tmpB8);return;_LLBE: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpB9=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp7F;if(_tmpB9->tag != 15)goto _LLC0;else{_tmpBA=(void*)_tmpB9->f1;}}_LLBF:
# 556
{const char*_tmp43C;Cyc_Absyndump_dump(((_tmp43C="region_t<",_tag_dyneither(_tmp43C,sizeof(char),10))));}Cyc_Absyndump_dumprgn(_tmpBA);{const char*_tmp43D;Cyc_Absyndump_dump(((_tmp43D=">",_tag_dyneither(_tmp43D,sizeof(char),2))));}return;_LLC0: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpBB=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp7F;if(_tmpBB->tag != 16)goto _LLC2;else{_tmpBC=(void*)_tmpBB->f1;_tmpBD=(void*)_tmpBB->f2;}}_LLC1:
{const char*_tmp43E;Cyc_Absyndump_dump(((_tmp43E="dynregion_t<",_tag_dyneither(_tmp43E,sizeof(char),13))));}Cyc_Absyndump_dumprgn(_tmpBC);
{const char*_tmp43F;Cyc_Absyndump_dump(((_tmp43F=",",_tag_dyneither(_tmp43F,sizeof(char),2))));}Cyc_Absyndump_dumprgn(_tmpBD);{const char*_tmp440;Cyc_Absyndump_dump(((_tmp440=">",_tag_dyneither(_tmp440,sizeof(char),2))));}return;_LLC2: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpBE=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp7F;if(_tmpBE->tag != 19)goto _LLC4;else{_tmpBF=(void*)_tmpBE->f1;}}_LLC3:
{const char*_tmp441;Cyc_Absyndump_dump(((_tmp441="tag_t<",_tag_dyneither(_tmp441,sizeof(char),7))));}Cyc_Absyndump_dumpntyp(_tmpBF);{const char*_tmp442;Cyc_Absyndump_dump(((_tmp442=">",_tag_dyneither(_tmp442,sizeof(char),2))));}return;_LLC4: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpC0=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp7F;if(_tmpC0->tag != 21)goto _LLC6;}_LLC5:
{const char*_tmp443;Cyc_Absyndump_dump(((_tmp443="`U",_tag_dyneither(_tmp443,sizeof(char),3))));}goto _LL79;_LLC6: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpC1=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp7F;if(_tmpC1->tag != 22)goto _LLC8;}_LLC7:
{const char*_tmp444;Cyc_Absyndump_dump(((_tmp444="`RC",_tag_dyneither(_tmp444,sizeof(char),4))));}goto _LL79;_LLC8: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpC2=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp7F;if(_tmpC2->tag != 26)goto _LLCA;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpC3=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmpC2->f1)->r;if(_tmpC3->tag != 0)goto _LLCA;else{_tmpC4=_tmpC3->f1;}}}_LLC9:
# 563
 Cyc_Absyndump_dump_aggrdecl(_tmpC4);return;_LLCA: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpC5=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp7F;if(_tmpC5->tag != 26)goto _LLCC;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpC6=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmpC5->f1)->r;if(_tmpC6->tag != 1)goto _LLCC;else{_tmpC7=_tmpC6->f1;}}}_LLCB:
# 565
 Cyc_Absyndump_dump_enumdecl(_tmpC7);return;_LLCC: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpC8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp7F;if(_tmpC8->tag != 26)goto _LLCE;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpC9=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmpC8->f1)->r;if(_tmpC9->tag != 2)goto _LLCE;else{_tmpCA=_tmpC9->f1;}}}_LLCD:
# 567
 Cyc_Absyndump_dump_datatypedecl(_tmpCA);return;_LLCE: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpCB=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp7F;if(_tmpCB->tag != 20)goto _LLD0;}_LLCF:
{const char*_tmp445;Cyc_Absyndump_dump(((_tmp445="`H",_tag_dyneither(_tmp445,sizeof(char),3))));}goto _LL79;_LLD0: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCC=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp7F;if(_tmpCC->tag != 23)goto _LLD2;else{_tmpCD=(void*)_tmpCC->f1;}}_LLD1:
{const char*_tmp446;Cyc_Absyndump_dump(((_tmp446="{",_tag_dyneither(_tmp446,sizeof(char),2))));}Cyc_Absyndump_dumptyp(_tmpCD);{const char*_tmp447;Cyc_Absyndump_dump(((_tmp447="}",_tag_dyneither(_tmp447,sizeof(char),2))));}goto _LL79;_LLD2: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCE=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp7F;if(_tmpCE->tag != 25)goto _LLD4;else{_tmpCF=(void*)_tmpCE->f1;}}_LLD3:
{const char*_tmp448;Cyc_Absyndump_dump(((_tmp448="regions(",_tag_dyneither(_tmp448,sizeof(char),9))));}Cyc_Absyndump_dumptyp(_tmpCF);{const char*_tmp449;Cyc_Absyndump_dump(((_tmp449=")",_tag_dyneither(_tmp449,sizeof(char),2))));}goto _LL79;_LLD4: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD0=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp7F;if(_tmpD0->tag != 24)goto _LL79;else{_tmpD1=_tmpD0->f1;}}_LLD5:
# 572
 for(0;_tmpD1 != 0;_tmpD1=_tmpD1->tl){
Cyc_Absyndump_dumptyp((void*)_tmpD1->hd);
if(_tmpD1->tl != 0){const char*_tmp44A;Cyc_Absyndump_dump(((_tmp44A="+",_tag_dyneither(_tmp44A,sizeof(char),2))));}}_LL79:;}
# 579
void Cyc_Absyndump_dumpvaropt(struct _dyneither_ptr*vo){
if(vo != 0)Cyc_Absyndump_dump_str(vo);}
# 582
void Cyc_Absyndump_dumpfunarg(struct _tuple8*t){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple16{void*f1;void*f2;};
# 585
void Cyc_Absyndump_dump_rgncmp(struct _tuple16*cmp){
void*_tmp114;void*_tmp115;struct _tuple16*_tmp113=cmp;_tmp114=_tmp113->f1;_tmp115=_tmp113->f2;
Cyc_Absyndump_dumpeff(_tmp114);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(_tmp115);}
# 589
void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){
const char*_tmp44B;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,rgn_po,((_tmp44B=",",_tag_dyneither(_tmp44B,sizeof(char),2))));}
# 592
void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 596
Cyc_Absyndump_dump_char((int)'(');
for(0;args != 0;args=args->tl){
Cyc_Absyndump_dumpfunarg((struct _tuple8*)args->hd);
if((args->tl != 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((int)',');}
# 601
if(c_varargs){
const char*_tmp44C;Cyc_Absyndump_dump(((_tmp44C="...",_tag_dyneither(_tmp44C,sizeof(char),4))));}else{
if(cyc_varargs != 0){
struct _tuple8*_tmp44D;struct _tuple8*_tmp118=(_tmp44D=_cycalloc(sizeof(*_tmp44D)),((_tmp44D->f1=cyc_varargs->name,((_tmp44D->f2=cyc_varargs->tq,((_tmp44D->f3=cyc_varargs->type,_tmp44D)))))));
{const char*_tmp44E;Cyc_Absyndump_dump(((_tmp44E="...",_tag_dyneither(_tmp44E,sizeof(char),4))));}
if(cyc_varargs->inject){const char*_tmp44F;Cyc_Absyndump_dump(((_tmp44F=" inject ",_tag_dyneither(_tmp44F,sizeof(char),9))));}
Cyc_Absyndump_dumpfunarg(_tmp118);}}
# 609
if(effopt != 0){
Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff(effopt);}
# 613
if(rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_rgnpo(rgn_po);}
# 617
Cyc_Absyndump_dump_char((int)')');
if(req != 0){
{const char*_tmp450;Cyc_Absyndump_dump(((_tmp450=" @requires(",_tag_dyneither(_tmp450,sizeof(char),12))));}
Cyc_Absyndump_dumpexp(req);
Cyc_Absyndump_dump_char((int)')');}
# 623
if(ens != 0){
{const char*_tmp451;Cyc_Absyndump_dump(((_tmp451=" @ensures(",_tag_dyneither(_tmp451,sizeof(char),11))));}
Cyc_Absyndump_dumpexp(ens);
Cyc_Absyndump_dump_char((int)')');}}
# 630
void Cyc_Absyndump_dumptyp(void*t){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 634
void Cyc_Absyndump_dumpdesignator(void*d){
void*_tmp11E=d;struct Cyc_Absyn_Exp*_tmp120;struct _dyneither_ptr*_tmp122;_LLE7: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp11F=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp11E;if(_tmp11F->tag != 0)goto _LLE9;else{_tmp120=_tmp11F->f1;}}_LLE8:
{const char*_tmp452;Cyc_Absyndump_dump(((_tmp452=".[",_tag_dyneither(_tmp452,sizeof(char),3))));}Cyc_Absyndump_dumpexp(_tmp120);Cyc_Absyndump_dump_char((int)']');goto _LLE6;_LLE9: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp121=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp11E;if(_tmp121->tag != 1)goto _LLE6;else{_tmp122=_tmp121->f1;}}_LLEA:
 Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp122);goto _LLE6;_LLE6:;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 641
void Cyc_Absyndump_dumpde(struct _tuple17*de){
{const char*_tmp455;const char*_tmp454;const char*_tmp453;((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*de).f1,((_tmp453="",_tag_dyneither(_tmp453,sizeof(char),1))),((_tmp454="=",_tag_dyneither(_tmp454,sizeof(char),2))),((_tmp455="=",_tag_dyneither(_tmp455,sizeof(char),2))));}
Cyc_Absyndump_dumpexp((*de).f2);}
# 646
void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){
const char*_tmp458;const char*_tmp457;const char*_tmp456;((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,inprec,es,((_tmp456="",_tag_dyneither(_tmp456,sizeof(char),1))),((_tmp457="",_tag_dyneither(_tmp457,sizeof(char),1))),((_tmp458=",",_tag_dyneither(_tmp458,sizeof(char),2))));}
# 650
void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
if(inprec >= myprec){
const char*_tmp459;Cyc_Absyndump_dump_nospace(((_tmp459="(",_tag_dyneither(_tmp459,sizeof(char),2))));}
{void*_tmp12B=e->r;void*_tmp12C=_tmp12B;enum Cyc_Absyn_Sign _tmp12E;char _tmp12F;struct _dyneither_ptr _tmp131;enum Cyc_Absyn_Sign _tmp133;short _tmp134;int _tmp136;int _tmp138;int _tmp13A;enum Cyc_Absyn_Sign _tmp13C;long long _tmp13D;struct _dyneither_ptr _tmp13F;struct _dyneither_ptr _tmp142;struct _dyneither_ptr _tmp144;struct _tuple0*_tmp146;enum Cyc_Absyn_Primop _tmp148;struct Cyc_List_List*_tmp149;struct Cyc_Absyn_Exp*_tmp14B;struct Cyc_Core_Opt*_tmp14C;struct Cyc_Absyn_Exp*_tmp14D;struct Cyc_Absyn_Exp*_tmp14F;struct Cyc_Absyn_Exp*_tmp151;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Exp*_tmp157;struct Cyc_Absyn_Exp*_tmp158;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*_tmp15C;struct Cyc_Absyn_Exp*_tmp15E;struct Cyc_Absyn_Exp*_tmp15F;struct Cyc_Absyn_Exp*_tmp161;struct Cyc_Absyn_Exp*_tmp162;struct Cyc_Absyn_Exp*_tmp164;struct Cyc_List_List*_tmp165;struct Cyc_Absyn_Exp*_tmp167;struct Cyc_Absyn_Exp*_tmp169;struct Cyc_Absyn_Exp*_tmp16B;void*_tmp16D;struct Cyc_Absyn_Exp*_tmp16E;struct Cyc_Absyn_Exp*_tmp170;struct Cyc_Absyn_Exp*_tmp172;struct Cyc_Absyn_Exp*_tmp173;void*_tmp175;struct Cyc_Absyn_Exp*_tmp177;void*_tmp179;int _tmp17B;struct _dyneither_ptr _tmp17C;struct Cyc_Absyn_Exp*_tmp17E;struct _dyneither_ptr*_tmp17F;void*_tmp181;struct Cyc_List_List*_tmp182;struct Cyc_Absyn_Exp*_tmp184;struct Cyc_Absyn_Exp*_tmp186;struct _dyneither_ptr*_tmp187;struct Cyc_Absyn_Exp*_tmp189;struct _dyneither_ptr*_tmp18A;struct Cyc_Absyn_Exp*_tmp18C;struct Cyc_Absyn_Exp*_tmp18D;struct Cyc_List_List*_tmp18F;struct _tuple8*_tmp191;struct Cyc_List_List*_tmp192;struct Cyc_List_List*_tmp194;struct Cyc_Absyn_Vardecl*_tmp196;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_Absyn_Exp*_tmp198;struct Cyc_Absyn_Exp*_tmp19A;void*_tmp19B;struct _tuple0*_tmp19D;struct Cyc_List_List*_tmp19E;struct Cyc_List_List*_tmp19F;struct Cyc_List_List*_tmp1A1;struct Cyc_List_List*_tmp1A3;struct Cyc_Absyn_Datatypefield*_tmp1A4;struct _tuple0*_tmp1A6;struct _tuple0*_tmp1A8;int _tmp1AA;struct Cyc_Absyn_Exp*_tmp1AB;void**_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AD;int _tmp1AE;struct Cyc_Absyn_Exp*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1B1;struct Cyc_Core_Opt*_tmp1B3;struct Cyc_List_List*_tmp1B4;struct Cyc_Absyn_Stmt*_tmp1B6;_LLEC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp12D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp12D->tag != 0)goto _LLEE;else{if(((_tmp12D->f1).Char_c).tag != 2)goto _LLEE;_tmp12E=((struct _tuple3)((_tmp12D->f1).Char_c).val).f1;_tmp12F=((struct _tuple3)((_tmp12D->f1).Char_c).val).f2;}}_LLED:
# 656
 Cyc_Absyndump_dump_char((int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp12F));Cyc_Absyndump_dump_char((int)'\'');
goto _LLEB;_LLEE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp130=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp130->tag != 0)goto _LLF0;else{if(((_tmp130->f1).Wchar_c).tag != 3)goto _LLF0;_tmp131=(struct _dyneither_ptr)((_tmp130->f1).Wchar_c).val;}}_LLEF:
{const char*_tmp45D;void*_tmp45C[1];struct Cyc_String_pa_PrintArg_struct _tmp45B;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp45B.tag=0,((_tmp45B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp131),((_tmp45C[0]=& _tmp45B,Cyc_aprintf(((_tmp45D="L'%s'",_tag_dyneither(_tmp45D,sizeof(char),6))),_tag_dyneither(_tmp45C,sizeof(void*),1)))))))));}goto _LLEB;_LLF0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp132=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp132->tag != 0)goto _LLF2;else{if(((_tmp132->f1).Short_c).tag != 4)goto _LLF2;_tmp133=((struct _tuple4)((_tmp132->f1).Short_c).val).f1;_tmp134=((struct _tuple4)((_tmp132->f1).Short_c).val).f2;}}_LLF1:
{const char*_tmp461;void*_tmp460[1];struct Cyc_Int_pa_PrintArg_struct _tmp45F;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp45F.tag=1,((_tmp45F.f1=(unsigned long)((int)_tmp134),((_tmp460[0]=& _tmp45F,Cyc_aprintf(((_tmp461="%d",_tag_dyneither(_tmp461,sizeof(char),3))),_tag_dyneither(_tmp460,sizeof(void*),1)))))))));}goto _LLEB;_LLF2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp135=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp135->tag != 0)goto _LLF4;else{if(((_tmp135->f1).Int_c).tag != 5)goto _LLF4;if(((struct _tuple5)((_tmp135->f1).Int_c).val).f1 != Cyc_Absyn_None)goto _LLF4;_tmp136=((struct _tuple5)((_tmp135->f1).Int_c).val).f2;}}_LLF3:
 _tmp138=_tmp136;goto _LLF5;_LLF4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp137=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp137->tag != 0)goto _LLF6;else{if(((_tmp137->f1).Int_c).tag != 5)goto _LLF6;if(((struct _tuple5)((_tmp137->f1).Int_c).val).f1 != Cyc_Absyn_Signed)goto _LLF6;_tmp138=((struct _tuple5)((_tmp137->f1).Int_c).val).f2;}}_LLF5:
{const char*_tmp465;void*_tmp464[1];struct Cyc_Int_pa_PrintArg_struct _tmp463;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp463.tag=1,((_tmp463.f1=(unsigned long)_tmp138,((_tmp464[0]=& _tmp463,Cyc_aprintf(((_tmp465="%d",_tag_dyneither(_tmp465,sizeof(char),3))),_tag_dyneither(_tmp464,sizeof(void*),1)))))))));}goto _LLEB;_LLF6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp139=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp139->tag != 0)goto _LLF8;else{if(((_tmp139->f1).Int_c).tag != 5)goto _LLF8;if(((struct _tuple5)((_tmp139->f1).Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LLF8;_tmp13A=((struct _tuple5)((_tmp139->f1).Int_c).val).f2;}}_LLF7:
{const char*_tmp469;void*_tmp468[1];struct Cyc_Int_pa_PrintArg_struct _tmp467;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp467.tag=1,((_tmp467.f1=(unsigned int)_tmp13A,((_tmp468[0]=& _tmp467,Cyc_aprintf(((_tmp469="%u",_tag_dyneither(_tmp469,sizeof(char),3))),_tag_dyneither(_tmp468,sizeof(void*),1)))))))));}goto _LLEB;_LLF8: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp13B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp13B->tag != 0)goto _LLFA;else{if(((_tmp13B->f1).LongLong_c).tag != 6)goto _LLFA;_tmp13C=((struct _tuple6)((_tmp13B->f1).LongLong_c).val).f1;_tmp13D=((struct _tuple6)((_tmp13B->f1).LongLong_c).val).f2;}}_LLF9:
# 665
 Cyc_Absyndump_dump(Cyc_Absynpp_longlong2string((unsigned long long)_tmp13D));goto _LLEB;_LLFA: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp13E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp13E->tag != 0)goto _LLFC;else{if(((_tmp13E->f1).Float_c).tag != 7)goto _LLFC;_tmp13F=((struct _tuple7)((_tmp13E->f1).Float_c).val).f1;}}_LLFB:
 Cyc_Absyndump_dump(_tmp13F);goto _LLEB;_LLFC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp140=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp140->tag != 0)goto _LLFE;else{if(((_tmp140->f1).Null_c).tag != 1)goto _LLFE;}}_LLFD:
{const char*_tmp46A;Cyc_Absyndump_dump(((_tmp46A="NULL",_tag_dyneither(_tmp46A,sizeof(char),5))));}goto _LLEB;_LLFE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp141=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp141->tag != 0)goto _LL100;else{if(((_tmp141->f1).String_c).tag != 8)goto _LL100;_tmp142=(struct _dyneither_ptr)((_tmp141->f1).String_c).val;}}_LLFF:
# 669
 Cyc_Absyndump_dump_char((int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp142));Cyc_Absyndump_dump_char((int)'"');
goto _LLEB;_LL100: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp143=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp143->tag != 0)goto _LL102;else{if(((_tmp143->f1).Wstring_c).tag != 9)goto _LL102;_tmp144=(struct _dyneither_ptr)((_tmp143->f1).Wstring_c).val;}}_LL101:
# 672
{const char*_tmp46B;Cyc_Absyndump_dump(((_tmp46B="L\"",_tag_dyneither(_tmp46B,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(_tmp144);Cyc_Absyndump_dump_char((int)'"');
goto _LLEB;_LL102: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp145=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp145->tag != 1)goto _LL104;else{_tmp146=_tmp145->f1;}}_LL103:
# 675
 Cyc_Absyndump_dumpqvar(_tmp146);goto _LLEB;_LL104: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp147=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp147->tag != 2)goto _LL106;else{_tmp148=_tmp147->f1;_tmp149=_tmp147->f2;}}_LL105: {
# 678
struct _dyneither_ptr _tmp1C5=Cyc_Absynpp_prim2str(_tmp148);
{int _tmp1C6=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp149);switch(_tmp1C6){case 1: _LL156:
# 681
 if(_tmp148 == Cyc_Absyn_Numelts){
{const char*_tmp46C;Cyc_Absyndump_dump(((_tmp46C="numelts(",_tag_dyneither(_tmp46C,sizeof(char),9))));}
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp149))->hd);{
const char*_tmp46D;Cyc_Absyndump_dump(((_tmp46D=")",_tag_dyneither(_tmp46D,sizeof(char),2))));};}else{
# 686
Cyc_Absyndump_dump(_tmp1C5);
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp149))->hd);}
# 689
break;case 2: _LL157:
# 691
 Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp149))->hd);
Cyc_Absyndump_dump(_tmp1C5);
Cyc_Absyndump_dump_char((int)' ');
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp149->tl))->hd);
break;default: _LL158: {
# 697
struct Cyc_Core_Failure_exn_struct _tmp473;const char*_tmp472;struct Cyc_Core_Failure_exn_struct*_tmp471;(int)_throw((void*)((_tmp471=_cycalloc(sizeof(*_tmp471)),((_tmp471[0]=((_tmp473.tag=Cyc_Core_Failure,((_tmp473.f1=((_tmp472="Absyndump -- Bad number of arguments to primop",_tag_dyneither(_tmp472,sizeof(char),47))),_tmp473)))),_tmp471)))));}}}
# 699
goto _LLEB;}_LL106: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp14A=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp14A->tag != 3)goto _LL108;else{_tmp14B=_tmp14A->f1;_tmp14C=_tmp14A->f2;_tmp14D=_tmp14A->f3;}}_LL107:
# 702
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp14B);
if(_tmp14C != 0)
Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)_tmp14C->v));
{const char*_tmp474;Cyc_Absyndump_dump_nospace(((_tmp474="=",_tag_dyneither(_tmp474,sizeof(char),2))));}
Cyc_Absyndump_dumpexp_prec(myprec,_tmp14D);
goto _LLEB;_LL108: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp14E=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp14E->tag != 4)goto _LL10A;else{_tmp14F=_tmp14E->f1;if(_tmp14E->f2 != Cyc_Absyn_PreInc)goto _LL10A;}}_LL109:
# 709
{const char*_tmp475;Cyc_Absyndump_dump(((_tmp475="++",_tag_dyneither(_tmp475,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp14F);goto _LLEB;_LL10A: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp150=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp150->tag != 4)goto _LL10C;else{_tmp151=_tmp150->f1;if(_tmp150->f2 != Cyc_Absyn_PreDec)goto _LL10C;}}_LL10B:
{const char*_tmp476;Cyc_Absyndump_dump(((_tmp476="--",_tag_dyneither(_tmp476,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp151);goto _LLEB;_LL10C: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp152=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp152->tag != 4)goto _LL10E;else{_tmp153=_tmp152->f1;if(_tmp152->f2 != Cyc_Absyn_PostInc)goto _LL10E;}}_LL10D:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp153);{const char*_tmp477;Cyc_Absyndump_dump(((_tmp477="++",_tag_dyneither(_tmp477,sizeof(char),3))));}goto _LLEB;_LL10E: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp154=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp154->tag != 4)goto _LL110;else{_tmp155=_tmp154->f1;if(_tmp154->f2 != Cyc_Absyn_PostDec)goto _LL110;}}_LL10F:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp155);{const char*_tmp478;Cyc_Absyndump_dump(((_tmp478="--",_tag_dyneither(_tmp478,sizeof(char),3))));}goto _LLEB;_LL110: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp156=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp156->tag != 5)goto _LL112;else{_tmp157=_tmp156->f1;_tmp158=_tmp156->f2;_tmp159=_tmp156->f3;}}_LL111:
# 715
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp157);
Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp158);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,_tmp159);
goto _LLEB;_LL112: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp15A=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp15A->tag != 6)goto _LL114;else{_tmp15B=_tmp15A->f1;_tmp15C=_tmp15A->f2;}}_LL113:
# 722
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp15B);{const char*_tmp479;Cyc_Absyndump_dump(((_tmp479=" && ",_tag_dyneither(_tmp479,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp15C);goto _LLEB;_LL114: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp15D=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp15D->tag != 7)goto _LL116;else{_tmp15E=_tmp15D->f1;_tmp15F=_tmp15D->f2;}}_LL115:
# 726
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp15E);{const char*_tmp47A;Cyc_Absyndump_dump(((_tmp47A=" || ",_tag_dyneither(_tmp47A,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp15F);goto _LLEB;_LL116: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp160=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp160->tag != 8)goto _LL118;else{_tmp161=_tmp160->f1;_tmp162=_tmp160->f2;}}_LL117:
# 732
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp161);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(myprec,_tmp162);
Cyc_Absyndump_dump_char((int)')');
goto _LLEB;_LL118: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp163=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp163->tag != 9)goto _LL11A;else{_tmp164=_tmp163->f1;_tmp165=_tmp163->f2;}}_LL119:
# 739
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp164);
{const char*_tmp47B;Cyc_Absyndump_dump_nospace(((_tmp47B="(",_tag_dyneither(_tmp47B,sizeof(char),2))));}{
# 745
int old_generate_line_directives=Cyc_Absyndump_generate_line_directives;
Cyc_Absyndump_generate_line_directives=
(old_generate_line_directives  && !(e->loc == 0)) && !(_tmp164->loc == 0);
Cyc_Absyndump_dumpexps_prec(20,_tmp165);
{const char*_tmp47C;Cyc_Absyndump_dump_nospace(((_tmp47C=")",_tag_dyneither(_tmp47C,sizeof(char),2))));}
Cyc_Absyndump_generate_line_directives=old_generate_line_directives;
goto _LLEB;};_LL11A: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp166=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp166->tag != 10)goto _LL11C;else{_tmp167=_tmp166->f1;}}_LL11B:
# 754
 Cyc_Absyndump_dumploc(e->loc);
{const char*_tmp47D;Cyc_Absyndump_dump(((_tmp47D="throw",_tag_dyneither(_tmp47D,sizeof(char),6))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp167);goto _LLEB;_LL11C: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp168=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp168->tag != 11)goto _LL11E;else{_tmp169=_tmp168->f1;}}_LL11D:
# 757
 _tmp16B=_tmp169;goto _LL11F;_LL11E: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp16A=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp16A->tag != 12)goto _LL120;else{_tmp16B=_tmp16A->f1;}}_LL11F:
 Cyc_Absyndump_dumpexp_prec(inprec,_tmp16B);goto _LLEB;_LL120: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp16C=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp16C->tag != 13)goto _LL122;else{_tmp16D=(void*)_tmp16C->f1;_tmp16E=_tmp16C->f2;}}_LL121:
# 761
 Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(_tmp16D);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,_tmp16E);
goto _LLEB;_LL122: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp16F=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp16F->tag != 14)goto _LL124;else{_tmp170=_tmp16F->f1;}}_LL123:
# 765
 Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp170);goto _LLEB;_LL124: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp171=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp171->tag != 15)goto _LL126;else{_tmp172=_tmp171->f1;_tmp173=_tmp171->f2;}}_LL125:
# 768
{const char*_tmp47E;Cyc_Absyndump_dump(((_tmp47E="new ",_tag_dyneither(_tmp47E,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp173);goto _LLEB;_LL126: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp174->tag != 16)goto _LL128;else{_tmp175=(void*)_tmp174->f1;}}_LL127:
# 771
{const char*_tmp47F;Cyc_Absyndump_dump(((_tmp47F="sizeof(",_tag_dyneither(_tmp47F,sizeof(char),8))));}Cyc_Absyndump_dumptyp(_tmp175);Cyc_Absyndump_dump_char((int)')');goto _LLEB;_LL128: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp176=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp176->tag != 17)goto _LL12A;else{_tmp177=_tmp176->f1;}}_LL129:
# 774
{const char*_tmp480;Cyc_Absyndump_dump(((_tmp480="sizeof(",_tag_dyneither(_tmp480,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp177);Cyc_Absyndump_dump_char((int)')');goto _LLEB;_LL12A: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp178=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp178->tag != 38)goto _LL12C;else{_tmp179=(void*)_tmp178->f1;}}_LL12B:
# 777
{const char*_tmp481;Cyc_Absyndump_dump(((_tmp481="valueof(",_tag_dyneither(_tmp481,sizeof(char),9))));}Cyc_Absyndump_dumptyp(_tmp179);Cyc_Absyndump_dump_char((int)')');goto _LLEB;_LL12C: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp17A=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp17A->tag != 39)goto _LL12E;else{_tmp17B=_tmp17A->f1;_tmp17C=_tmp17A->f2;}}_LL12D:
# 780
{const char*_tmp482;Cyc_Absyndump_dump(((_tmp482="__asm__",_tag_dyneither(_tmp482,sizeof(char),8))));}
if(_tmp17B){const char*_tmp483;Cyc_Absyndump_dump(((_tmp483=" volatile ",_tag_dyneither(_tmp483,sizeof(char),11))));}
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_nospace(_tmp17C);
Cyc_Absyndump_dump_char((int)')');
goto _LLEB;_LL12E: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp17D=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp17D->tag != 37)goto _LL130;else{_tmp17E=_tmp17D->f1;_tmp17F=_tmp17D->f2;}}_LL12F:
# 788
{const char*_tmp484;Cyc_Absyndump_dump(((_tmp484="tagcheck(",_tag_dyneither(_tmp484,sizeof(char),10))));}Cyc_Absyndump_dumpexp(_tmp17E);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp17F);
Cyc_Absyndump_dump_char((int)')');
goto _LLEB;_LL130: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp180=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp180->tag != 18)goto _LL132;else{_tmp181=(void*)_tmp180->f1;_tmp182=_tmp180->f2;}}_LL131:
# 793
{const char*_tmp485;Cyc_Absyndump_dump(((_tmp485="offsetof(",_tag_dyneither(_tmp485,sizeof(char),10))));}Cyc_Absyndump_dumptyp(_tmp181);Cyc_Absyndump_dump_char((int)',');
for(0;_tmp182 != 0;_tmp182=_tmp182->tl){
{void*_tmp1DE=(void*)_tmp182->hd;void*_tmp1DF=_tmp1DE;struct _dyneither_ptr*_tmp1E1;unsigned int _tmp1E3;_LL15B: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp1E0=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp1DF;if(_tmp1E0->tag != 0)goto _LL15D;else{_tmp1E1=_tmp1E0->f1;}}_LL15C:
# 797
 Cyc_Absyndump_dump_nospace(*_tmp1E1);
goto _LL15A;_LL15D: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp1E2=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp1DF;if(_tmp1E2->tag != 1)goto _LL15A;else{_tmp1E3=_tmp1E2->f1;}}_LL15E:
# 800
{const char*_tmp489;void*_tmp488[1];struct Cyc_Int_pa_PrintArg_struct _tmp487;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp487.tag=1,((_tmp487.f1=(unsigned long)((int)_tmp1E3),((_tmp488[0]=& _tmp487,Cyc_aprintf(((_tmp489="%d",_tag_dyneither(_tmp489,sizeof(char),3))),_tag_dyneither(_tmp488,sizeof(void*),1)))))))));}
goto _LL15A;_LL15A:;}
# 803
if(_tmp182->tl != 0)Cyc_Absyndump_dump_char((int)'.');}
# 805
Cyc_Absyndump_dump_char((int)')');
goto _LLEB;_LL132: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp183=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp183->tag != 19)goto _LL134;else{_tmp184=_tmp183->f1;}}_LL133:
# 809
 Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp184);goto _LLEB;_LL134: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp185=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp185->tag != 20)goto _LL136;else{_tmp186=_tmp185->f1;_tmp187=_tmp185->f2;}}_LL135:
# 812
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp186);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp187);
goto _LLEB;_LL136: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp188=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp188->tag != 21)goto _LL138;else{_tmp189=_tmp188->f1;_tmp18A=_tmp188->f2;}}_LL137:
# 816
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp189);{const char*_tmp48A;Cyc_Absyndump_dump_nospace(((_tmp48A="->",_tag_dyneither(_tmp48A,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(*_tmp18A);
goto _LLEB;_LL138: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp18B=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp18B->tag != 22)goto _LL13A;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18B->f2;}}_LL139:
# 820
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp18C);
Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp18D);Cyc_Absyndump_dump_char((int)']');goto _LLEB;_LL13A: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp18E=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp18E->tag != 23)goto _LL13C;else{_tmp18F=_tmp18E->f1;}}_LL13B:
# 824
{const char*_tmp48B;Cyc_Absyndump_dump(((_tmp48B="$(",_tag_dyneither(_tmp48B,sizeof(char),3))));}Cyc_Absyndump_dumpexps_prec(20,_tmp18F);Cyc_Absyndump_dump_char((int)')');goto _LLEB;_LL13C: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp190=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp190->tag != 24)goto _LL13E;else{_tmp191=_tmp190->f1;_tmp192=_tmp190->f2;}}_LL13D:
# 827
 Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp((*_tmp191).f3);
Cyc_Absyndump_dump_char((int)')');
{const char*_tmp48E;const char*_tmp48D;const char*_tmp48C;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp192,((_tmp48C="{",_tag_dyneither(_tmp48C,sizeof(char),2))),((_tmp48D="}",_tag_dyneither(_tmp48D,sizeof(char),2))),((_tmp48E=",",_tag_dyneither(_tmp48E,sizeof(char),2))));}
goto _LLEB;_LL13E: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp193=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp193->tag != 25)goto _LL140;else{_tmp194=_tmp193->f1;}}_LL13F:
# 834
{const char*_tmp491;const char*_tmp490;const char*_tmp48F;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp194,((_tmp48F="{",_tag_dyneither(_tmp48F,sizeof(char),2))),((_tmp490="}",_tag_dyneither(_tmp490,sizeof(char),2))),((_tmp491=",",_tag_dyneither(_tmp491,sizeof(char),2))));}
goto _LLEB;_LL140: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp195=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp195->tag != 26)goto _LL142;else{_tmp196=_tmp195->f1;_tmp197=_tmp195->f2;_tmp198=_tmp195->f3;}}_LL141:
# 838
{const char*_tmp492;Cyc_Absyndump_dump(((_tmp492="{for",_tag_dyneither(_tmp492,sizeof(char),5))));}Cyc_Absyndump_dump_str((*_tmp196->name).f2);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp197);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(_tmp198);Cyc_Absyndump_dump_char((int)'}');
goto _LLEB;_LL142: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp199=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp199->tag != 27)goto _LL144;else{_tmp19A=_tmp199->f1;_tmp19B=(void*)_tmp199->f2;}}_LL143:
# 843
{const char*_tmp493;Cyc_Absyndump_dump(((_tmp493="{for x ",_tag_dyneither(_tmp493,sizeof(char),8))));}Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp19A);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumptyp(_tmp19B);Cyc_Absyndump_dump_char((int)'}');
goto _LLEB;_LL144: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp19C=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp19C->tag != 28)goto _LL146;else{_tmp19D=_tmp19C->f1;_tmp19E=_tmp19C->f2;_tmp19F=_tmp19C->f3;}}_LL145:
# 848
 Cyc_Absyndump_dumpqvar(_tmp19D);
Cyc_Absyndump_dump_char((int)'{');
if(_tmp19E != 0)
Cyc_Absyndump_dumptps(_tmp19E);
{const char*_tmp496;const char*_tmp495;const char*_tmp494;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp19F,((_tmp494="",_tag_dyneither(_tmp494,sizeof(char),1))),((_tmp495="}",_tag_dyneither(_tmp495,sizeof(char),2))),((_tmp496=",",_tag_dyneither(_tmp496,sizeof(char),2))));}
goto _LLEB;_LL146: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1A0=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp1A0->tag != 29)goto _LL148;else{_tmp1A1=_tmp1A0->f2;}}_LL147:
# 856
{const char*_tmp499;const char*_tmp498;const char*_tmp497;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp1A1,((_tmp497="{",_tag_dyneither(_tmp497,sizeof(char),2))),((_tmp498="}",_tag_dyneither(_tmp498,sizeof(char),2))),((_tmp499=",",_tag_dyneither(_tmp499,sizeof(char),2))));}
goto _LLEB;_LL148: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1A2=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp1A2->tag != 30)goto _LL14A;else{_tmp1A3=_tmp1A2->f1;_tmp1A4=_tmp1A2->f3;}}_LL149:
# 860
 Cyc_Absyndump_dumpqvar(_tmp1A4->name);
if(_tmp1A3 != 0){const char*_tmp49C;const char*_tmp49B;const char*_tmp49A;((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,_tmp1A3,((_tmp49A="(",_tag_dyneither(_tmp49A,sizeof(char),2))),((_tmp49B=")",_tag_dyneither(_tmp49B,sizeof(char),2))),((_tmp49C=",",_tag_dyneither(_tmp49C,sizeof(char),2))));}
goto _LLEB;_LL14A: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp1A5=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp1A5->tag != 31)goto _LL14C;else{_tmp1A6=_tmp1A5->f1;}}_LL14B:
# 864
 Cyc_Absyndump_dumpqvar(_tmp1A6);goto _LLEB;_LL14C: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp1A7=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp1A7->tag != 32)goto _LL14E;else{_tmp1A8=_tmp1A7->f1;}}_LL14D:
 Cyc_Absyndump_dumpqvar(_tmp1A8);goto _LLEB;_LL14E: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp1A9=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp1A9->tag != 33)goto _LL150;else{_tmp1AA=(_tmp1A9->f1).is_calloc;_tmp1AB=(_tmp1A9->f1).rgn;_tmp1AC=(_tmp1A9->f1).elt_type;_tmp1AD=(_tmp1A9->f1).num_elts;_tmp1AE=(_tmp1A9->f1).inline_call;}}_LL14F:
# 868
 Cyc_Absyndump_dumploc(_tmp1AD->loc);
if(_tmp1AA){
if(_tmp1AB != 0){
{const char*_tmp49D;Cyc_Absyndump_dump(((_tmp49D="rcalloc(",_tag_dyneither(_tmp49D,sizeof(char),9))));}
Cyc_Absyndump_dumpexp(_tmp1AB);{const char*_tmp49E;Cyc_Absyndump_dump(((_tmp49E=",",_tag_dyneither(_tmp49E,sizeof(char),2))));};}else{
# 874
const char*_tmp49F;Cyc_Absyndump_dump(((_tmp49F="calloc",_tag_dyneither(_tmp49F,sizeof(char),7))));}
# 876
Cyc_Absyndump_dumpexp(_tmp1AD);
{const char*_tmp4A0;Cyc_Absyndump_dump(((_tmp4A0=",",_tag_dyneither(_tmp4A0,sizeof(char),2))));}
Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp1AC)),0));{
const char*_tmp4A1;Cyc_Absyndump_dump(((_tmp4A1=")",_tag_dyneither(_tmp4A1,sizeof(char),2))));};}else{
# 881
if(_tmp1AB != 0){
if(_tmp1AE){
const char*_tmp4A2;Cyc_Absyndump_dump(((_tmp4A2="rmalloc_inline(",_tag_dyneither(_tmp4A2,sizeof(char),16))));}else{
# 885
const char*_tmp4A3;Cyc_Absyndump_dump(((_tmp4A3="rmalloc(",_tag_dyneither(_tmp4A3,sizeof(char),9))));}
Cyc_Absyndump_dumpexp(_tmp1AB);{const char*_tmp4A4;Cyc_Absyndump_dump(((_tmp4A4=",",_tag_dyneither(_tmp4A4,sizeof(char),2))));};}else{
# 888
const char*_tmp4A5;Cyc_Absyndump_dump(((_tmp4A5="malloc(",_tag_dyneither(_tmp4A5,sizeof(char),8))));}
# 891
if(_tmp1AC != 0)
Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp1AC,0),_tmp1AD,0));else{
# 894
Cyc_Absyndump_dumpexp(_tmp1AD);}{
const char*_tmp4A6;Cyc_Absyndump_dump(((_tmp4A6=")",_tag_dyneither(_tmp4A6,sizeof(char),2))));};}
# 897
goto _LLEB;_LL150: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp1AF=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp1AF->tag != 34)goto _LL152;else{_tmp1B0=_tmp1AF->f1;_tmp1B1=_tmp1AF->f2;}}_LL151:
# 900
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp1B0);
{const char*_tmp4A7;Cyc_Absyndump_dump_nospace(((_tmp4A7=":=:",_tag_dyneither(_tmp4A7,sizeof(char),4))));}
Cyc_Absyndump_dumpexp_prec(myprec,_tmp1B1);
goto _LLEB;_LL152: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1B2=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp1B2->tag != 35)goto _LL154;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B2->f2;}}_LL153:
# 907
{const char*_tmp4AA;const char*_tmp4A9;const char*_tmp4A8;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp1B4,((_tmp4A8="{",_tag_dyneither(_tmp4A8,sizeof(char),2))),((_tmp4A9="}",_tag_dyneither(_tmp4A9,sizeof(char),2))),((_tmp4AA=",",_tag_dyneither(_tmp4AA,sizeof(char),2))));}
goto _LLEB;_LL154: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp1B5=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp12C;if(_tmp1B5->tag != 36)goto _LLEB;else{_tmp1B6=_tmp1B5->f1;}}_LL155:
# 911
{const char*_tmp4AB;Cyc_Absyndump_dump_nospace(((_tmp4AB="({",_tag_dyneither(_tmp4AB,sizeof(char),3))));}
Cyc_Absyndump_dumpstmt(_tmp1B6,1);
{const char*_tmp4AC;Cyc_Absyndump_dump_nospace(((_tmp4AC="})",_tag_dyneither(_tmp4AC,sizeof(char),3))));}
goto _LLEB;_LLEB:;}
# 916
if(inprec >= myprec)
Cyc_Absyndump_dump_char((int)')');}
# 920
void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){
Cyc_Absyndump_dumpexp_prec(0,e);}
# 924
void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs){
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp20A=(struct Cyc_Absyn_Switch_clause*)scs->hd;
if(_tmp20A->where_clause == 0  && (_tmp20A->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){
const char*_tmp4AD;Cyc_Absyndump_dump(((_tmp4AD="default:",_tag_dyneither(_tmp4AD,sizeof(char),9))));}else{
# 930
{const char*_tmp4AE;Cyc_Absyndump_dump(((_tmp4AE="case",_tag_dyneither(_tmp4AE,sizeof(char),5))));}
Cyc_Absyndump_dumppat(_tmp20A->pattern);
if(_tmp20A->where_clause != 0){
{const char*_tmp4AF;Cyc_Absyndump_dump(((_tmp4AF="&&",_tag_dyneither(_tmp4AF,sizeof(char),3))));}
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp20A->where_clause));}{
# 936
const char*_tmp4B0;Cyc_Absyndump_dump_nospace(((_tmp4B0=":",_tag_dyneither(_tmp4B0,sizeof(char),2))));};}
# 938
Cyc_Absyndump_dumpstmt(_tmp20A->body,0);}}
# 942
void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt){
Cyc_Absyndump_dumploc(s->loc);{
void*_tmp20F=s->r;void*_tmp210=_tmp20F;struct Cyc_Absyn_Exp*_tmp213;struct Cyc_Absyn_Stmt*_tmp215;struct Cyc_Absyn_Stmt*_tmp216;struct Cyc_Absyn_Exp*_tmp219;struct Cyc_Absyn_Exp*_tmp21B;struct Cyc_Absyn_Stmt*_tmp21C;struct Cyc_Absyn_Stmt*_tmp21D;struct Cyc_Absyn_Exp*_tmp21F;struct Cyc_Absyn_Stmt*_tmp220;struct _dyneither_ptr*_tmp224;struct Cyc_Absyn_Exp*_tmp226;struct Cyc_Absyn_Exp*_tmp227;struct Cyc_Absyn_Exp*_tmp228;struct Cyc_Absyn_Stmt*_tmp229;struct Cyc_Absyn_Exp*_tmp22B;struct Cyc_List_List*_tmp22C;struct Cyc_Absyn_Decl*_tmp22E;struct Cyc_Absyn_Stmt*_tmp22F;struct _dyneither_ptr*_tmp231;struct Cyc_Absyn_Stmt*_tmp232;struct Cyc_Absyn_Stmt*_tmp234;struct Cyc_Absyn_Exp*_tmp235;struct Cyc_List_List*_tmp238;struct Cyc_Absyn_Stmt*_tmp23A;struct Cyc_List_List*_tmp23B;struct Cyc_Absyn_Exp*_tmp23D;_LL160: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp211=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp210;if(_tmp211->tag != 0)goto _LL162;}_LL161:
 Cyc_Absyndump_dump_semi();goto _LL15F;_LL162: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp212=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp210;if(_tmp212->tag != 1)goto _LL164;else{_tmp213=_tmp212->f1;}}_LL163:
 Cyc_Absyndump_dumpexp(_tmp213);Cyc_Absyndump_dump_semi();goto _LL15F;_LL164: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp214=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp210;if(_tmp214->tag != 2)goto _LL166;else{_tmp215=_tmp214->f1;_tmp216=_tmp214->f2;}}_LL165:
# 951
 if(Cyc_Absynpp_is_declaration(_tmp215)){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp215,0);Cyc_Absyndump_dump_char((int)'}');}else{
# 954
Cyc_Absyndump_dumpstmt(_tmp215,0);}
if(Cyc_Absynpp_is_declaration(_tmp216)){
if(expstmt)Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp216,expstmt);
Cyc_Absyndump_dump_char((int)'}');
if(expstmt)Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dump_semi();}else{
# 961
Cyc_Absyndump_dumpstmt(_tmp216,expstmt);}
goto _LL15F;_LL166: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp217=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp210;if(_tmp217->tag != 3)goto _LL168;else{if(_tmp217->f1 != 0)goto _LL168;}}_LL167:
{const char*_tmp4B1;Cyc_Absyndump_dump(((_tmp4B1="return;",_tag_dyneither(_tmp4B1,sizeof(char),8))));}goto _LL15F;_LL168: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp218=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp210;if(_tmp218->tag != 3)goto _LL16A;else{_tmp219=_tmp218->f1;}}_LL169:
{const char*_tmp4B2;Cyc_Absyndump_dump(((_tmp4B2="return",_tag_dyneither(_tmp4B2,sizeof(char),7))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp219));Cyc_Absyndump_dump_semi();goto _LL15F;_LL16A: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp21A=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp210;if(_tmp21A->tag != 4)goto _LL16C;else{_tmp21B=_tmp21A->f1;_tmp21C=_tmp21A->f2;_tmp21D=_tmp21A->f3;}}_LL16B:
# 966
{const char*_tmp4B3;Cyc_Absyndump_dump(((_tmp4B3="if(",_tag_dyneither(_tmp4B3,sizeof(char),4))));}Cyc_Absyndump_dumpexp(_tmp21B);
{void*_tmp241=_tmp21C->r;void*_tmp242=_tmp241;_LL187: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp243=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp242;if(_tmp243->tag != 2)goto _LL189;}_LL188:
 goto _LL18A;_LL189: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp244=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp242;if(_tmp244->tag != 12)goto _LL18B;}_LL18A:
 goto _LL18C;_LL18B: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp245=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp242;if(_tmp245->tag != 4)goto _LL18D;}_LL18C:
 goto _LL18E;_LL18D: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp246=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp242;if(_tmp246->tag != 13)goto _LL18F;}_LL18E:
{const char*_tmp4B4;Cyc_Absyndump_dump_nospace(((_tmp4B4="){",_tag_dyneither(_tmp4B4,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(_tmp21C,0);Cyc_Absyndump_dump_char((int)'}');goto _LL186;_LL18F:;_LL190:
 Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(_tmp21C,0);_LL186:;}
# 974
{void*_tmp248=_tmp21D->r;void*_tmp249=_tmp248;_LL192: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp24A=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp249;if(_tmp24A->tag != 0)goto _LL194;}_LL193:
 goto _LL191;_LL194:;_LL195:
{const char*_tmp4B5;Cyc_Absyndump_dump(((_tmp4B5="else{",_tag_dyneither(_tmp4B5,sizeof(char),6))));}Cyc_Absyndump_dumpstmt(_tmp21D,0);Cyc_Absyndump_dump_char((int)'}');goto _LL191;_LL191:;}
# 978
goto _LL15F;_LL16C: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp21E=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp210;if(_tmp21E->tag != 5)goto _LL16E;else{_tmp21F=(_tmp21E->f1).f1;_tmp220=_tmp21E->f2;}}_LL16D:
# 980
{const char*_tmp4B6;Cyc_Absyndump_dump(((_tmp4B6="while(",_tag_dyneither(_tmp4B6,sizeof(char),7))));}Cyc_Absyndump_dumpexp(_tmp21F);{const char*_tmp4B7;Cyc_Absyndump_dump_nospace(((_tmp4B7="){",_tag_dyneither(_tmp4B7,sizeof(char),3))));}
Cyc_Absyndump_dumpstmt(_tmp220,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL15F;_LL16E: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp221=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp210;if(_tmp221->tag != 6)goto _LL170;}_LL16F:
{const char*_tmp4B8;Cyc_Absyndump_dump(((_tmp4B8="break;",_tag_dyneither(_tmp4B8,sizeof(char),7))));}goto _LL15F;_LL170: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp222=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp210;if(_tmp222->tag != 7)goto _LL172;}_LL171:
{const char*_tmp4B9;Cyc_Absyndump_dump(((_tmp4B9="continue;",_tag_dyneither(_tmp4B9,sizeof(char),10))));}goto _LL15F;_LL172: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp223=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp210;if(_tmp223->tag != 8)goto _LL174;else{_tmp224=_tmp223->f1;}}_LL173:
{const char*_tmp4BA;Cyc_Absyndump_dump(((_tmp4BA="goto",_tag_dyneither(_tmp4BA,sizeof(char),5))));}Cyc_Absyndump_dump_str(_tmp224);Cyc_Absyndump_dump_semi();goto _LL15F;_LL174: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp225=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp210;if(_tmp225->tag != 9)goto _LL176;else{_tmp226=_tmp225->f1;_tmp227=(_tmp225->f2).f1;_tmp228=(_tmp225->f3).f1;_tmp229=_tmp225->f4;}}_LL175:
# 989
{const char*_tmp4BB;Cyc_Absyndump_dump(((_tmp4BB="for(",_tag_dyneither(_tmp4BB,sizeof(char),5))));}Cyc_Absyndump_dumpexp(_tmp226);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp227);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp228);
{const char*_tmp4BC;Cyc_Absyndump_dump_nospace(((_tmp4BC="){",_tag_dyneither(_tmp4BC,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(_tmp229,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL15F;_LL176: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp22A=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp210;if(_tmp22A->tag != 10)goto _LL178;else{_tmp22B=_tmp22A->f1;_tmp22C=_tmp22A->f2;}}_LL177:
# 993
{const char*_tmp4BD;Cyc_Absyndump_dump(((_tmp4BD="switch(",_tag_dyneither(_tmp4BD,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp22B);{const char*_tmp4BE;Cyc_Absyndump_dump_nospace(((_tmp4BE="){",_tag_dyneither(_tmp4BE,sizeof(char),3))));}
Cyc_Absyndump_dumpswitchclauses(_tmp22C);
Cyc_Absyndump_dump_char((int)'}');
goto _LL15F;_LL178: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp22D=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp210;if(_tmp22D->tag != 12)goto _LL17A;else{_tmp22E=_tmp22D->f1;_tmp22F=_tmp22D->f2;}}_LL179:
# 998
 Cyc_Absyndump_dumpdecl(_tmp22E);Cyc_Absyndump_dumpstmt(_tmp22F,expstmt);goto _LL15F;_LL17A: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp230=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp210;if(_tmp230->tag != 13)goto _LL17C;else{_tmp231=_tmp230->f1;_tmp232=_tmp230->f2;}}_LL17B:
# 1004
 if(Cyc_Absynpp_is_declaration(_tmp232)){
Cyc_Absyndump_dump_str(_tmp231);
if(expstmt){const char*_tmp4BF;Cyc_Absyndump_dump_nospace(((_tmp4BF=": ({",_tag_dyneither(_tmp4BF,sizeof(char),5))));}else{const char*_tmp4C0;Cyc_Absyndump_dump_nospace(((_tmp4C0=": {",_tag_dyneither(_tmp4C0,sizeof(char),4))));}
Cyc_Absyndump_dumpstmt(_tmp232,expstmt);
if(expstmt){const char*_tmp4C1;Cyc_Absyndump_dump_nospace(((_tmp4C1="});}",_tag_dyneither(_tmp4C1,sizeof(char),5))));}else{Cyc_Absyndump_dump_char((int)'}');}}else{
# 1010
Cyc_Absyndump_dump_str(_tmp231);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(_tmp232,expstmt);}
# 1012
goto _LL15F;_LL17C: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp233=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp210;if(_tmp233->tag != 14)goto _LL17E;else{_tmp234=_tmp233->f1;_tmp235=(_tmp233->f2).f1;}}_LL17D:
# 1015
{const char*_tmp4C2;Cyc_Absyndump_dump(((_tmp4C2="do{",_tag_dyneither(_tmp4C2,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp234,0);
{const char*_tmp4C3;Cyc_Absyndump_dump_nospace(((_tmp4C3="}while(",_tag_dyneither(_tmp4C3,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp235);{const char*_tmp4C4;Cyc_Absyndump_dump_nospace(((_tmp4C4=");",_tag_dyneither(_tmp4C4,sizeof(char),3))));}
goto _LL15F;_LL17E: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp236=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp210;if(_tmp236->tag != 11)goto _LL180;else{if(_tmp236->f1 != 0)goto _LL180;}}_LL17F:
# 1019
{const char*_tmp4C5;Cyc_Absyndump_dump(((_tmp4C5="fallthru;",_tag_dyneither(_tmp4C5,sizeof(char),10))));}goto _LL15F;_LL180: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp237=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp210;if(_tmp237->tag != 11)goto _LL182;else{_tmp238=_tmp237->f1;}}_LL181:
# 1021
{const char*_tmp4C6;Cyc_Absyndump_dump(((_tmp4C6="fallthru(",_tag_dyneither(_tmp4C6,sizeof(char),10))));}Cyc_Absyndump_dumpexps_prec(20,_tmp238);{const char*_tmp4C7;Cyc_Absyndump_dump_nospace(((_tmp4C7=");",_tag_dyneither(_tmp4C7,sizeof(char),3))));}goto _LL15F;_LL182: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp239=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp210;if(_tmp239->tag != 15)goto _LL184;else{_tmp23A=_tmp239->f1;_tmp23B=_tmp239->f2;}}_LL183:
# 1024
{const char*_tmp4C8;Cyc_Absyndump_dump(((_tmp4C8="try",_tag_dyneither(_tmp4C8,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp23A,0);
{const char*_tmp4C9;Cyc_Absyndump_dump(((_tmp4C9="catch{",_tag_dyneither(_tmp4C9,sizeof(char),7))));}
Cyc_Absyndump_dumpswitchclauses(_tmp23B);Cyc_Absyndump_dump_char((int)'}');
goto _LL15F;_LL184: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp23C=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp210;if(_tmp23C->tag != 16)goto _LL15F;else{_tmp23D=_tmp23C->f1;}}_LL185:
# 1030
{const char*_tmp4CA;Cyc_Absyndump_dump(((_tmp4CA="reset_region(",_tag_dyneither(_tmp4CA,sizeof(char),14))));}Cyc_Absyndump_dumpexp(_tmp23D);{const char*_tmp4CB;Cyc_Absyndump_dump(((_tmp4CB=");",_tag_dyneither(_tmp4CB,sizeof(char),3))));}
goto _LL15F;_LL15F:;};}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 1036
void Cyc_Absyndump_dumpdp(struct _tuple18*dp){
{const char*_tmp4CE;const char*_tmp4CD;const char*_tmp4CC;((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*dp).f1,((_tmp4CC="",_tag_dyneither(_tmp4CC,sizeof(char),1))),((_tmp4CD="=",_tag_dyneither(_tmp4CD,sizeof(char),2))),((_tmp4CE="=",_tag_dyneither(_tmp4CE,sizeof(char),2))));}
Cyc_Absyndump_dumppat((*dp).f2);}
# 1041
void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){
void*_tmp265=p->r;void*_tmp266=_tmp265;int _tmp26A;int _tmp26C;int _tmp26E;char _tmp270;struct _dyneither_ptr _tmp272;struct Cyc_Absyn_Vardecl*_tmp274;struct Cyc_Absyn_Vardecl*_tmp277;struct Cyc_Absyn_Pat*_tmp278;struct Cyc_Absyn_Tvar*_tmp27A;struct Cyc_Absyn_Vardecl*_tmp27B;struct Cyc_List_List*_tmp27D;int _tmp27E;struct Cyc_Absyn_Pat*_tmp280;struct Cyc_Absyn_Vardecl*_tmp282;struct Cyc_Absyn_Vardecl*_tmp285;struct Cyc_Absyn_Pat*_tmp286;struct Cyc_Absyn_Tvar*_tmp288;struct Cyc_Absyn_Vardecl*_tmp289;struct _tuple0*_tmp28B;struct _tuple0*_tmp28D;struct Cyc_List_List*_tmp28E;int _tmp28F;union Cyc_Absyn_AggrInfoU _tmp291;struct Cyc_List_List*_tmp292;struct Cyc_List_List*_tmp293;int _tmp294;struct Cyc_List_List*_tmp296;struct Cyc_List_List*_tmp297;int _tmp298;struct Cyc_Absyn_Datatypefield*_tmp29A;struct Cyc_List_List*_tmp29B;int _tmp29C;struct Cyc_Absyn_Enumfield*_tmp29E;struct Cyc_Absyn_Enumfield*_tmp2A0;struct Cyc_Absyn_Exp*_tmp2A2;_LL197: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp267=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp267->tag != 0)goto _LL199;}_LL198:
 Cyc_Absyndump_dump_char((int)'_');goto _LL196;_LL199: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp268=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp268->tag != 9)goto _LL19B;}_LL19A:
{const char*_tmp4CF;Cyc_Absyndump_dump(((_tmp4CF="NULL",_tag_dyneither(_tmp4CF,sizeof(char),5))));}goto _LL196;_LL19B: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp269=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp269->tag != 10)goto _LL19D;else{if(_tmp269->f1 != Cyc_Absyn_None)goto _LL19D;_tmp26A=_tmp269->f2;}}_LL19C:
 _tmp26C=_tmp26A;goto _LL19E;_LL19D: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp26B=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp26B->tag != 10)goto _LL19F;else{if(_tmp26B->f1 != Cyc_Absyn_Signed)goto _LL19F;_tmp26C=_tmp26B->f2;}}_LL19E:
{const char*_tmp4D3;void*_tmp4D2[1];struct Cyc_Int_pa_PrintArg_struct _tmp4D1;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp4D1.tag=1,((_tmp4D1.f1=(unsigned long)_tmp26C,((_tmp4D2[0]=& _tmp4D1,Cyc_aprintf(((_tmp4D3="%d",_tag_dyneither(_tmp4D3,sizeof(char),3))),_tag_dyneither(_tmp4D2,sizeof(void*),1)))))))));}goto _LL196;_LL19F: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp26D=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp26D->tag != 10)goto _LL1A1;else{if(_tmp26D->f1 != Cyc_Absyn_Unsigned)goto _LL1A1;_tmp26E=_tmp26D->f2;}}_LL1A0:
{const char*_tmp4D7;void*_tmp4D6[1];struct Cyc_Int_pa_PrintArg_struct _tmp4D5;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp4D5.tag=1,((_tmp4D5.f1=(unsigned int)_tmp26E,((_tmp4D6[0]=& _tmp4D5,Cyc_aprintf(((_tmp4D7="%u",_tag_dyneither(_tmp4D7,sizeof(char),3))),_tag_dyneither(_tmp4D6,sizeof(void*),1)))))))));}goto _LL196;_LL1A1: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp26F=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp26F->tag != 11)goto _LL1A3;else{_tmp270=_tmp26F->f1;}}_LL1A2:
# 1049
{const char*_tmp4D8;Cyc_Absyndump_dump(((_tmp4D8="'",_tag_dyneither(_tmp4D8,sizeof(char),2))));}Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp270));{const char*_tmp4D9;Cyc_Absyndump_dump_nospace(((_tmp4D9="'",_tag_dyneither(_tmp4D9,sizeof(char),2))));}goto _LL196;_LL1A3: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp271=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp271->tag != 12)goto _LL1A5;else{_tmp272=_tmp271->f1;}}_LL1A4:
 Cyc_Absyndump_dump(_tmp272);goto _LL196;_LL1A5: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp273=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp273->tag != 1)goto _LL1A7;else{_tmp274=_tmp273->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp275=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp273->f2)->r;if(_tmp275->tag != 0)goto _LL1A7;};}}_LL1A6:
 Cyc_Absyndump_dumpqvar(_tmp274->name);goto _LL196;_LL1A7: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp276=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp276->tag != 1)goto _LL1A9;else{_tmp277=_tmp276->f1;_tmp278=_tmp276->f2;}}_LL1A8:
 Cyc_Absyndump_dumpqvar(_tmp277->name);{const char*_tmp4DA;Cyc_Absyndump_dump(((_tmp4DA=" as ",_tag_dyneither(_tmp4DA,sizeof(char),5))));}Cyc_Absyndump_dumppat(_tmp278);goto _LL196;_LL1A9: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp279=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp279->tag != 2)goto _LL1AB;else{_tmp27A=_tmp279->f1;_tmp27B=_tmp279->f2;}}_LL1AA:
# 1054
{const char*_tmp4DB;Cyc_Absyndump_dump(((_tmp4DB="alias",_tag_dyneither(_tmp4DB,sizeof(char),6))));}
{const char*_tmp4DC;Cyc_Absyndump_dump(((_tmp4DC="<",_tag_dyneither(_tmp4DC,sizeof(char),2))));}Cyc_Absyndump_dumptvar(_tmp27A);{const char*_tmp4DD;Cyc_Absyndump_dump(((_tmp4DD=">",_tag_dyneither(_tmp4DD,sizeof(char),2))));}
Cyc_Absyndump_dumpvardecl(_tmp27B,p->loc,0);
goto _LL196;_LL1AB: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp27C=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp27C->tag != 5)goto _LL1AD;else{_tmp27D=_tmp27C->f1;_tmp27E=_tmp27C->f2;}}_LL1AC: {
# 1059
const char*_tmp4DF;const char*_tmp4DE;struct _dyneither_ptr term=_tmp27E?(_tmp4DF=", ...)",_tag_dyneither(_tmp4DF,sizeof(char),7)):((_tmp4DE=")",_tag_dyneither(_tmp4DE,sizeof(char),2)));
{const char*_tmp4E1;const char*_tmp4E0;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp27D,((_tmp4E0="$(",_tag_dyneither(_tmp4E0,sizeof(char),3))),term,((_tmp4E1=",",_tag_dyneither(_tmp4E1,sizeof(char),2))));}goto _LL196;}_LL1AD: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp27F=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp27F->tag != 6)goto _LL1AF;else{_tmp280=_tmp27F->f1;}}_LL1AE:
{const char*_tmp4E2;Cyc_Absyndump_dump(((_tmp4E2="&",_tag_dyneither(_tmp4E2,sizeof(char),2))));}Cyc_Absyndump_dumppat(_tmp280);goto _LL196;_LL1AF: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp281=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp281->tag != 3)goto _LL1B1;else{_tmp282=_tmp281->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp283=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp281->f2)->r;if(_tmp283->tag != 0)goto _LL1B1;};}}_LL1B0:
# 1063
{const char*_tmp4E3;Cyc_Absyndump_dump(((_tmp4E3="*",_tag_dyneither(_tmp4E3,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmp282->name);goto _LL196;_LL1B1: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp284=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp284->tag != 3)goto _LL1B3;else{_tmp285=_tmp284->f1;_tmp286=_tmp284->f2;}}_LL1B2:
# 1065
{const char*_tmp4E4;Cyc_Absyndump_dump(((_tmp4E4="*",_tag_dyneither(_tmp4E4,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmp285->name);{const char*_tmp4E5;Cyc_Absyndump_dump(((_tmp4E5=" as ",_tag_dyneither(_tmp4E5,sizeof(char),5))));}Cyc_Absyndump_dumppat(_tmp286);goto _LL196;_LL1B3: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp287=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp287->tag != 4)goto _LL1B5;else{_tmp288=_tmp287->f1;_tmp289=_tmp287->f2;}}_LL1B4:
# 1067
 Cyc_Absyndump_dumpqvar(_tmp289->name);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptvar(_tmp288);Cyc_Absyndump_dump_char((int)'>');goto _LL196;_LL1B5: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp28A=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp28A->tag != 15)goto _LL1B7;else{_tmp28B=_tmp28A->f1;}}_LL1B6:
 Cyc_Absyndump_dumpqvar(_tmp28B);goto _LL196;_LL1B7: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp28C=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp28C->tag != 16)goto _LL1B9;else{_tmp28D=_tmp28C->f1;_tmp28E=_tmp28C->f2;_tmp28F=_tmp28C->f3;}}_LL1B8: {
# 1070
const char*_tmp4E7;const char*_tmp4E6;struct _dyneither_ptr term=_tmp28F?(_tmp4E7=", ...)",_tag_dyneither(_tmp4E7,sizeof(char),7)):((_tmp4E6=")",_tag_dyneither(_tmp4E6,sizeof(char),2)));
Cyc_Absyndump_dumpqvar(_tmp28D);{const char*_tmp4E9;const char*_tmp4E8;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp28E,((_tmp4E8="(",_tag_dyneither(_tmp4E8,sizeof(char),2))),term,((_tmp4E9=",",_tag_dyneither(_tmp4E9,sizeof(char),2))));}goto _LL196;}_LL1B9: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp290=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp290->tag != 7)goto _LL1BB;else{if(_tmp290->f1 == 0)goto _LL1BB;_tmp291=(_tmp290->f1)->aggr_info;_tmp292=_tmp290->f2;_tmp293=_tmp290->f3;_tmp294=_tmp290->f4;}}_LL1BA: {
# 1073
struct _tuple10 _tmp2BC=Cyc_Absyn_aggr_kinded_name(_tmp291);struct _tuple0*_tmp2BE;struct _tuple10 _tmp2BD=_tmp2BC;_tmp2BE=_tmp2BD.f2;{
const char*_tmp4EB;const char*_tmp4EA;struct _dyneither_ptr term=_tmp294?(_tmp4EB=", ...)",_tag_dyneither(_tmp4EB,sizeof(char),7)):((_tmp4EA=")",_tag_dyneither(_tmp4EA,sizeof(char),2)));
Cyc_Absyndump_dumpqvar(_tmp2BE);Cyc_Absyndump_dump_char((int)'{');
{const char*_tmp4EE;const char*_tmp4ED;const char*_tmp4EC;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp292,((_tmp4EC="[",_tag_dyneither(_tmp4EC,sizeof(char),2))),((_tmp4ED="]",_tag_dyneither(_tmp4ED,sizeof(char),2))),((_tmp4EE=",",_tag_dyneither(_tmp4EE,sizeof(char),2))));}
{const char*_tmp4F0;const char*_tmp4EF;((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp293,((_tmp4EF="",_tag_dyneither(_tmp4EF,sizeof(char),1))),term,((_tmp4F0=",",_tag_dyneither(_tmp4F0,sizeof(char),2))));}
goto _LL196;};}_LL1BB: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp295=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp295->tag != 7)goto _LL1BD;else{if(_tmp295->f1 != 0)goto _LL1BD;_tmp296=_tmp295->f2;_tmp297=_tmp295->f3;_tmp298=_tmp295->f4;}}_LL1BC: {
# 1080
const char*_tmp4F2;const char*_tmp4F1;struct _dyneither_ptr term=_tmp298?(_tmp4F2=", ...)",_tag_dyneither(_tmp4F2,sizeof(char),7)):((_tmp4F1=")",_tag_dyneither(_tmp4F1,sizeof(char),2)));
Cyc_Absyndump_dump_char((int)'{');
{const char*_tmp4F5;const char*_tmp4F4;const char*_tmp4F3;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp296,((_tmp4F3="[",_tag_dyneither(_tmp4F3,sizeof(char),2))),((_tmp4F4="]",_tag_dyneither(_tmp4F4,sizeof(char),2))),((_tmp4F5=",",_tag_dyneither(_tmp4F5,sizeof(char),2))));}
{const char*_tmp4F7;const char*_tmp4F6;((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp297,((_tmp4F6="",_tag_dyneither(_tmp4F6,sizeof(char),1))),term,((_tmp4F7=",",_tag_dyneither(_tmp4F7,sizeof(char),2))));}
goto _LL196;}_LL1BD: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp299=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp299->tag != 8)goto _LL1BF;else{_tmp29A=_tmp299->f2;_tmp29B=_tmp299->f3;_tmp29C=_tmp299->f4;}}_LL1BE: {
# 1086
const char*_tmp4F9;const char*_tmp4F8;struct _dyneither_ptr term=_tmp29C?(_tmp4F9=", ...)",_tag_dyneither(_tmp4F9,sizeof(char),7)):((_tmp4F8=")",_tag_dyneither(_tmp4F8,sizeof(char),2)));
Cyc_Absyndump_dumpqvar(_tmp29A->name);
if(_tmp29B != 0){const char*_tmp4FB;const char*_tmp4FA;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp29B,((_tmp4FA="(",_tag_dyneither(_tmp4FA,sizeof(char),2))),term,((_tmp4FB=",",_tag_dyneither(_tmp4FB,sizeof(char),2))));}
goto _LL196;}_LL1BF: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp29D=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp29D->tag != 13)goto _LL1C1;else{_tmp29E=_tmp29D->f2;}}_LL1C0:
 _tmp2A0=_tmp29E;goto _LL1C2;_LL1C1: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp29F=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp29F->tag != 14)goto _LL1C3;else{_tmp2A0=_tmp29F->f2;}}_LL1C2:
 Cyc_Absyndump_dumpqvar(_tmp2A0->name);goto _LL196;_LL1C3: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp2A1=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp266;if(_tmp2A1->tag != 17)goto _LL196;else{_tmp2A2=_tmp2A1->f1;}}_LL1C4:
 Cyc_Absyndump_dumpexp(_tmp2A2);goto _LL196;_LL196:;}
# 1096
void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->typs != 0)
Cyc_Absyndump_dumpargs(ef->typs);}
# 1101
void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){
const char*_tmp4FC;((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,fields,((_tmp4FC=",",_tag_dyneither(_tmp4FC,sizeof(char),2))));}
# 1104
void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->tag != 0){
{const char*_tmp4FD;Cyc_Absyndump_dump(((_tmp4FD=" = ",_tag_dyneither(_tmp4FD,sizeof(char),4))));}
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
# 1111
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){
const char*_tmp4FE;((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,fields,((_tmp4FE=",",_tag_dyneither(_tmp4FE,sizeof(char),2))));}
# 1115
void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp2D4=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct _dyneither_ptr*_tmp2D6;struct Cyc_Absyn_Tqual _tmp2D7;void*_tmp2D8;struct Cyc_Absyn_Exp*_tmp2D9;struct Cyc_List_List*_tmp2DA;struct Cyc_Absyn_Exp*_tmp2DB;struct Cyc_Absyn_Aggrfield*_tmp2D5=_tmp2D4;_tmp2D6=_tmp2D5->name;_tmp2D7=_tmp2D5->tq;_tmp2D8=_tmp2D5->type;_tmp2D9=_tmp2D5->width;_tmp2DA=_tmp2D5->attributes;_tmp2DB=_tmp2D5->requires_clause;
# 1119
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL1C5:
# 1121
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp2D7,_tmp2D8,Cyc_Absyndump_dump_str,_tmp2D6);
Cyc_Absyndump_dumpatts(_tmp2DA);
break;case Cyc_Cyclone_Vc_c: _LL1C6:
# 1125
 Cyc_Absyndump_dumpatts(_tmp2DA);
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp2D7,_tmp2D8,Cyc_Absyndump_dump_str,_tmp2D6);
break;}
# 1129
if((unsigned int)_tmp2DB){
{const char*_tmp4FF;Cyc_Absyndump_dump(((_tmp4FF="@requires ",_tag_dyneither(_tmp4FF,sizeof(char),11))));}
Cyc_Absyndump_dumpexp(_tmp2DB);}
# 1133
if(_tmp2D9 != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp(_tmp2D9);}
# 1137
Cyc_Absyndump_dump_semi();}}
# 1141
void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){
Cyc_Absyndump_dumpqvar(td->name);
Cyc_Absyndump_dumptvars(td->tvs);}
# 1146
static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dumpatts(fd->attributes);
Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple19{void*f1;struct _tuple0*f2;};
# 1150
static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple19*pr){
{void*_tmp2DD=(*pr).f1;void*_tmp2DE=_tmp2DD;_LL1C9: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp2DF=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp2DE;if(_tmp2DF->tag != 11)goto _LL1CB;}_LL1CA:
 goto _LL1C8;_LL1CB: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp2E0=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp2DE;if(_tmp2E0->tag != 1)goto _LL1CD;}_LL1CC:
{const char*_tmp500;Cyc_Absyndump_dump(((_tmp500="_stdcall",_tag_dyneither(_tmp500,sizeof(char),9))));}goto _LL1C8;_LL1CD: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp2E1=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp2DE;if(_tmp2E1->tag != 2)goto _LL1CF;}_LL1CE:
{const char*_tmp501;Cyc_Absyndump_dump(((_tmp501="_cdecl",_tag_dyneither(_tmp501,sizeof(char),7))));}goto _LL1C8;_LL1CF: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp2E2=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp2DE;if(_tmp2E2->tag != 3)goto _LL1D1;}_LL1D0:
{const char*_tmp502;Cyc_Absyndump_dump(((_tmp502="_fastcall",_tag_dyneither(_tmp502,sizeof(char),10))));}goto _LL1C8;_LL1D1:;_LL1D2:
 goto _LL1C8;_LL1C8:;}
# 1158
Cyc_Absyndump_dumpqvar((*pr).f2);}
# 1160
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dump_callconv(fd->attributes);
Cyc_Absyndump_dumpqvar(fd->name);}
# 1165
static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){
for(0;vds != 0;vds=vds->tl){
Cyc_Absyndump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);
if(vds->tl != 0)Cyc_Absyndump_dump_char((int)',');}}
# 1172
static int Cyc_Absyndump_is_char_ptr(void*t){
# 1174
void*_tmp2E6=t;void*_tmp2E8;void*_tmp2EA;_LL1D4:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2E7=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2E6;if(_tmp2E7->tag != 1)goto _LL1D6;else{_tmp2E8=(void*)_tmp2E7->f2;}}if(!(_tmp2E8 != 0))goto _LL1D6;_LL1D5:
 return Cyc_Absyndump_is_char_ptr(_tmp2E8);_LL1D6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2E9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E6;if(_tmp2E9->tag != 5)goto _LL1D8;else{_tmp2EA=(_tmp2E9->f1).elt_typ;}}_LL1D7:
# 1177
 L: {
void*_tmp2EB=_tmp2EA;void*_tmp2ED;void*_tmp2EF;_LL1DB:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2EC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2EB;if(_tmp2EC->tag != 1)goto _LL1DD;else{_tmp2ED=(void*)_tmp2EC->f2;}}if(!(_tmp2ED != 0))goto _LL1DD;_LL1DC:
 _tmp2EA=_tmp2ED;goto L;_LL1DD:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp2EE=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2EB;if(_tmp2EE->tag != 17)goto _LL1DF;else{_tmp2EF=(void*)_tmp2EE->f4;}}if(!(_tmp2EF != 0))goto _LL1DF;_LL1DE:
 _tmp2EA=_tmp2EF;goto L;_LL1DF: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2F0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2EB;if(_tmp2F0->tag != 6)goto _LL1E1;else{if(_tmp2F0->f2 != Cyc_Absyn_Char_sz)goto _LL1E1;}}_LL1E0:
 return 1;_LL1E1:;_LL1E2:
 return 0;_LL1DA:;}_LL1D8:;_LL1D9:
# 1184
 return 0;_LL1D3:;}
# 1188
void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,unsigned int loc,int do_semi){
enum Cyc_Absyn_Scope _tmp2F2;struct _tuple0*_tmp2F3;struct Cyc_Absyn_Tqual _tmp2F4;void*_tmp2F5;struct Cyc_Absyn_Exp*_tmp2F6;struct Cyc_List_List*_tmp2F7;struct Cyc_Absyn_Vardecl*_tmp2F1=vd;_tmp2F2=_tmp2F1->sc;_tmp2F3=_tmp2F1->name;_tmp2F4=_tmp2F1->tq;_tmp2F5=_tmp2F1->type;_tmp2F6=_tmp2F1->initializer;_tmp2F7=_tmp2F1->attributes;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL1E3:
# 1193
 if(_tmp2F2 == Cyc_Absyn_Extern  && Cyc_Absyndump_qvar_to_Cids){
void*_tmp2F8=Cyc_Tcutil_compress(_tmp2F5);void*_tmp2F9=_tmp2F8;_LL1E6: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2FA=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2F9;if(_tmp2FA->tag != 9)goto _LL1E8;}_LL1E7:
 goto _LL1E5;_LL1E8:;_LL1E9:
 Cyc_Absyndump_dumpscope(_tmp2F2);_LL1E5:;}else{
# 1199
Cyc_Absyndump_dumpscope(_tmp2F2);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp2F4,_tmp2F5,Cyc_Absyndump_dumpqvar,_tmp2F3);
Cyc_Absyndump_dumpatts(_tmp2F7);
break;case Cyc_Cyclone_Vc_c: _LL1E4:
# 1204
 Cyc_Absyndump_dumpatts(_tmp2F7);
Cyc_Absyndump_dumpscope(_tmp2F2);{
struct _RegionHandle _tmp2FB=_new_region("temp");struct _RegionHandle*temp=& _tmp2FB;_push_region(temp);{
int is_cp=Cyc_Absyndump_is_char_ptr(_tmp2F5);
struct _tuple11 _tmp2FC=Cyc_Absynpp_to_tms(temp,_tmp2F4,_tmp2F5);struct Cyc_Absyn_Tqual _tmp2FE;void*_tmp2FF;struct Cyc_List_List*_tmp300;struct _tuple11 _tmp2FD=_tmp2FC;_tmp2FE=_tmp2FD.f1;_tmp2FF=_tmp2FD.f2;_tmp300=_tmp2FD.f3;{
# 1210
void*call_conv=(void*)& Cyc_Absyn_Unused_att_val;
{struct Cyc_List_List*tms2=_tmp300;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp301=(void*)tms2->hd;void*_tmp302=_tmp301;struct Cyc_List_List*_tmp304;_LL1EC: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp303=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp302;if(_tmp303->tag != 5)goto _LL1EE;else{_tmp304=_tmp303->f2;}}_LL1ED:
# 1214
 for(0;_tmp304 != 0;_tmp304=_tmp304->tl){
void*_tmp305=(void*)_tmp304->hd;void*_tmp306=_tmp305;_LL1F1: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp307=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp306;if(_tmp307->tag != 1)goto _LL1F3;}_LL1F2:
 call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;goto _LL1F0;_LL1F3: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp308=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp306;if(_tmp308->tag != 2)goto _LL1F5;}_LL1F4:
 call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;goto _LL1F0;_LL1F5: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp309=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp306;if(_tmp309->tag != 3)goto _LL1F7;}_LL1F6:
 call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;goto _LL1F0;_LL1F7:;_LL1F8:
 goto _LL1F0;_LL1F0:;}
# 1221
goto _LL1EB;_LL1EE:;_LL1EF:
 goto _LL1EB;_LL1EB:;}}
# 1224
Cyc_Absyndump_dumptq(_tmp2FE);
Cyc_Absyndump_dumpntyp(_tmp2FF);{
struct _tuple19 _tmp503;struct _tuple19 _tmp30A=(_tmp503.f1=call_conv,((_tmp503.f2=_tmp2F3,_tmp503)));
((void(*)(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(struct _tuple19*),struct _tuple19*a))Cyc_Absyndump_dumptms)(is_cp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp300),Cyc_Absyndump_dump_callconv_qvar,& _tmp30A);};};}
# 1229
_npop_handler(0);break;
# 1206
;_pop_region(temp);};}
# 1232
if(_tmp2F6 != 0){
Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp(_tmp2F6);}
# 1236
if(do_semi)Cyc_Absyndump_dump_semi();}
# 1239
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*ad){
Cyc_Absyndump_dumpscope(ad->sc);
if(ad->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
const char*_tmp504;Cyc_Absyndump_dump(((_tmp504="@tagged ",_tag_dyneither(_tmp504,sizeof(char),9))));}
Cyc_Absyndump_dumpaggr_kind(ad->kind);
Cyc_Absyndump_dumpqvar(ad->name);
Cyc_Absyndump_dumpkindedtvars(ad->tvs);
if(ad->impl == 0)return;else{
# 1248
Cyc_Absyndump_dump_char((int)'{');
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0){
const char*_tmp507;const char*_tmp506;const char*_tmp505;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmp505="<",_tag_dyneither(_tmp505,sizeof(char),2))),((_tmp506=">",_tag_dyneither(_tmp506,sizeof(char),2))),((_tmp507=",",_tag_dyneither(_tmp507,sizeof(char),2))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);}
# 1255
Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
{const char*_tmp508;Cyc_Absyndump_dump(((_tmp508="}",_tag_dyneither(_tmp508,sizeof(char),2))));}
Cyc_Absyndump_dumpatts(ad->attributes);}}
# 1260
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*ed){
enum Cyc_Absyn_Scope _tmp312;struct _tuple0*_tmp313;struct Cyc_Core_Opt*_tmp314;struct Cyc_Absyn_Enumdecl*_tmp311=ed;_tmp312=_tmp311->sc;_tmp313=_tmp311->name;_tmp314=_tmp311->fields;
Cyc_Absyndump_dumpscope(_tmp312);
{const char*_tmp509;Cyc_Absyndump_dump(((_tmp509="enum ",_tag_dyneither(_tmp509,sizeof(char),6))));}
Cyc_Absyndump_dumpqvar(_tmp313);
if(_tmp314 != 0){
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp314->v);{
const char*_tmp50A;Cyc_Absyndump_dump_nospace(((_tmp50A="}",_tag_dyneither(_tmp50A,sizeof(char),2))));};}}
# 1271
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*dd){
enum Cyc_Absyn_Scope _tmp318;struct _tuple0*_tmp319;struct Cyc_List_List*_tmp31A;struct Cyc_Core_Opt*_tmp31B;int _tmp31C;struct Cyc_Absyn_Datatypedecl*_tmp317=dd;_tmp318=_tmp317->sc;_tmp319=_tmp317->name;_tmp31A=_tmp317->tvs;_tmp31B=_tmp317->fields;_tmp31C=_tmp317->is_extensible;
Cyc_Absyndump_dumpscope(_tmp318);
if(_tmp31C){const char*_tmp50B;Cyc_Absyndump_dump(((_tmp50B="@extensible ",_tag_dyneither(_tmp50B,sizeof(char),13))));}
{const char*_tmp50C;Cyc_Absyndump_dump(((_tmp50C="datatype ",_tag_dyneither(_tmp50C,sizeof(char),10))));}
Cyc_Absyndump_dumpqvar(_tmp319);
Cyc_Absyndump_dumptvars(_tmp31A);
if(_tmp31B != 0){
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)_tmp31B->v);{
const char*_tmp50D;Cyc_Absyndump_dump_nospace(((_tmp50D="}",_tag_dyneither(_tmp50D,sizeof(char),2))));};}}struct _tuple20{unsigned int f1;struct _tuple0*f2;int f3;};
# 1285
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){
Cyc_Absyndump_dumploc(d->loc);{
void*_tmp320=d->r;void*_tmp321=_tmp320;struct Cyc_Absyn_Vardecl*_tmp323;struct Cyc_Absyn_Fndecl*_tmp325;struct Cyc_Absyn_Aggrdecl*_tmp327;struct Cyc_Absyn_Datatypedecl*_tmp329;struct Cyc_Absyn_Enumdecl*_tmp32B;struct Cyc_Absyn_Pat*_tmp32D;struct Cyc_Absyn_Exp*_tmp32E;struct Cyc_List_List*_tmp330;struct Cyc_Absyn_Tvar*_tmp332;struct Cyc_Absyn_Vardecl*_tmp333;int _tmp334;struct Cyc_Absyn_Exp*_tmp335;struct Cyc_Absyn_Typedefdecl*_tmp337;struct _dyneither_ptr*_tmp339;struct Cyc_List_List*_tmp33A;struct _tuple0*_tmp33C;struct Cyc_List_List*_tmp33D;struct Cyc_List_List*_tmp33F;struct Cyc_List_List*_tmp341;struct Cyc_List_List*_tmp342;_LL1FA: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp322=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp321;if(_tmp322->tag != 0)goto _LL1FC;else{_tmp323=_tmp322->f1;}}_LL1FB:
# 1289
 Cyc_Absyndump_dumpvardecl(_tmp323,d->loc,1);
goto _LL1F9;_LL1FC: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp324=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp321;if(_tmp324->tag != 1)goto _LL1FE;else{_tmp325=_tmp324->f1;}}_LL1FD:
# 1293
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL218:
 Cyc_Absyndump_dumpatts(_tmp325->attributes);break;case Cyc_Cyclone_Gcc_c: _LL219:
 break;}
# 1297
if(_tmp325->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp345=Cyc_Cyclone_c_compiler;enum Cyc_Cyclone_C_Compilers _tmp346;_LL21C: if(_tmp345 != Cyc_Cyclone_Vc_c)goto _LL21E;_LL21D:
{const char*_tmp50E;Cyc_Absyndump_dump(((_tmp50E="__inline",_tag_dyneither(_tmp50E,sizeof(char),9))));}goto _LL21B;_LL21E: _tmp346=_tmp345;_LL21F:
{const char*_tmp50F;Cyc_Absyndump_dump(((_tmp50F="inline",_tag_dyneither(_tmp50F,sizeof(char),7))));}goto _LL21B;_LL21B:;}
# 1302
Cyc_Absyndump_dumpscope(_tmp325->sc);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp515;struct Cyc_Absyn_FnInfo _tmp514;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp513;void*t=(void*)((_tmp513=_cycalloc(sizeof(*_tmp513)),((_tmp513[0]=((_tmp515.tag=9,((_tmp515.f1=((_tmp514.tvars=_tmp325->tvs,((_tmp514.effect=_tmp325->effect,((_tmp514.ret_tqual=_tmp325->ret_tqual,((_tmp514.ret_typ=_tmp325->ret_type,((_tmp514.args=
# 1305
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp325->args),((_tmp514.c_varargs=_tmp325->c_varargs,((_tmp514.cyc_varargs=_tmp325->cyc_varargs,((_tmp514.rgn_po=_tmp325->rgn_po,((_tmp514.attributes=0,((_tmp514.requires_clause=_tmp325->requires_clause,((_tmp514.requires_relns=_tmp325->requires_relns,((_tmp514.ensures_clause=_tmp325->ensures_clause,((_tmp514.ensures_relns=_tmp325->ensures_relns,_tmp514)))))))))))))))))))))))))),_tmp515)))),_tmp513))));
# 1310
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL220:
# 1312
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_atts_qvar,_tmp325);
break;case Cyc_Cyclone_Vc_c: _LL221:
# 1315
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_callconv_fdqvar,_tmp325);
break;}
# 1318
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpstmt(_tmp325->body,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL1F9;};_LL1FE: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp326=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp321;if(_tmp326->tag != 5)goto _LL200;else{_tmp327=_tmp326->f1;}}_LL1FF:
# 1323
 Cyc_Absyndump_dump_aggrdecl(_tmp327);Cyc_Absyndump_dump_semi();
goto _LL1F9;_LL200: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp328=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp321;if(_tmp328->tag != 6)goto _LL202;else{_tmp329=_tmp328->f1;}}_LL201:
# 1326
 Cyc_Absyndump_dump_datatypedecl(_tmp329);Cyc_Absyndump_dump_semi();
goto _LL1F9;_LL202: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp32A=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp321;if(_tmp32A->tag != 7)goto _LL204;else{_tmp32B=_tmp32A->f1;}}_LL203:
# 1329
 Cyc_Absyndump_dump_enumdecl(_tmp32B);Cyc_Absyndump_dump_semi();
goto _LL1F9;_LL204: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp32C=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp321;if(_tmp32C->tag != 2)goto _LL206;else{_tmp32D=_tmp32C->f1;_tmp32E=_tmp32C->f3;}}_LL205:
# 1332
{const char*_tmp516;Cyc_Absyndump_dump(((_tmp516="let",_tag_dyneither(_tmp516,sizeof(char),4))));}
Cyc_Absyndump_dumppat(_tmp32D);
Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp(_tmp32E);
Cyc_Absyndump_dump_semi();
goto _LL1F9;_LL206: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp32F=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp321;if(_tmp32F->tag != 3)goto _LL208;else{_tmp330=_tmp32F->f1;}}_LL207:
# 1339
{const char*_tmp517;Cyc_Absyndump_dump(((_tmp517="let ",_tag_dyneither(_tmp517,sizeof(char),5))));}
Cyc_Absyndump_dumpids(_tmp330);
Cyc_Absyndump_dump_semi();
goto _LL1F9;_LL208: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp331=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp321;if(_tmp331->tag != 4)goto _LL20A;else{_tmp332=_tmp331->f1;_tmp333=_tmp331->f2;_tmp334=_tmp331->f3;_tmp335=_tmp331->f4;}}_LL209:
# 1344
{const char*_tmp518;Cyc_Absyndump_dump(((_tmp518="region",_tag_dyneither(_tmp518,sizeof(char),7))));}
{const char*_tmp519;Cyc_Absyndump_dump(((_tmp519="<",_tag_dyneither(_tmp519,sizeof(char),2))));}Cyc_Absyndump_dumptvar(_tmp332);{const char*_tmp51A;Cyc_Absyndump_dump(((_tmp51A="> ",_tag_dyneither(_tmp51A,sizeof(char),3))));}Cyc_Absyndump_dumpqvar(_tmp333->name);
if((unsigned int)_tmp335){
{const char*_tmp51B;Cyc_Absyndump_dump(((_tmp51B=" = open(",_tag_dyneither(_tmp51B,sizeof(char),9))));}
Cyc_Absyndump_dumpexp(_tmp335);{
const char*_tmp51C;Cyc_Absyndump_dump(((_tmp51C=")",_tag_dyneither(_tmp51C,sizeof(char),2))));};}
# 1351
if(_tmp334){const char*_tmp51D;Cyc_Absyndump_dump(((_tmp51D="@resetable",_tag_dyneither(_tmp51D,sizeof(char),11))));}
Cyc_Absyndump_dump_semi();
goto _LL1F9;_LL20A: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp336=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp321;if(_tmp336->tag != 8)goto _LL20C;else{_tmp337=_tmp336->f1;}}_LL20B:
# 1359
{const char*_tmp51E;Cyc_Absyndump_dump(((_tmp51E="typedef",_tag_dyneither(_tmp51E,sizeof(char),8))));}{
void*t;
if(_tmp337->defn == 0)
t=Cyc_Absyn_new_evar(_tmp337->kind,0);else{
# 1364
t=(void*)_check_null(_tmp337->defn);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(_tmp337->tq,t,Cyc_Absyndump_dumptypedefname,_tmp337);
Cyc_Absyndump_dumpatts(_tmp337->atts);
Cyc_Absyndump_dump_semi();
# 1369
goto _LL1F9;};_LL20C: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp338=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp321;if(_tmp338->tag != 9)goto _LL20E;else{_tmp339=_tmp338->f1;_tmp33A=_tmp338->f2;}}_LL20D:
# 1371
{const char*_tmp51F;Cyc_Absyndump_dump(((_tmp51F="namespace ",_tag_dyneither(_tmp51F,sizeof(char),11))));}
Cyc_Absyndump_dump_str(_tmp339);
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp33A != 0;_tmp33A=_tmp33A->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp33A->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL1F9;_LL20E: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp33B=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp321;if(_tmp33B->tag != 10)goto _LL210;else{_tmp33C=_tmp33B->f1;_tmp33D=_tmp33B->f2;}}_LL20F:
# 1379
{const char*_tmp520;Cyc_Absyndump_dump(((_tmp520="using ",_tag_dyneither(_tmp520,sizeof(char),7))));}
Cyc_Absyndump_dumpqvar(_tmp33C);
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp33D != 0;_tmp33D=_tmp33D->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp33D->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL1F9;_LL210: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp33E=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp321;if(_tmp33E->tag != 11)goto _LL212;else{_tmp33F=_tmp33E->f1;}}_LL211:
# 1387
{const char*_tmp521;Cyc_Absyndump_dump(((_tmp521="extern \"C\" {",_tag_dyneither(_tmp521,sizeof(char),13))));}
for(0;_tmp33F != 0;_tmp33F=_tmp33F->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp33F->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL1F9;_LL212: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp340=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp321;if(_tmp340->tag != 12)goto _LL214;else{_tmp341=_tmp340->f1;_tmp342=_tmp340->f2;}}_LL213:
# 1393
{const char*_tmp522;Cyc_Absyndump_dump(((_tmp522="extern \"C include\" {",_tag_dyneither(_tmp522,sizeof(char),21))));}
for(0;_tmp341 != 0;_tmp341=_tmp341->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp341->hd);}
Cyc_Absyndump_dump_char((int)'}');
if(_tmp342 != 0){
{const char*_tmp523;Cyc_Absyndump_dump(((_tmp523=" export {",_tag_dyneither(_tmp523,sizeof(char),10))));}
for(0;_tmp342 != 0;_tmp342=_tmp342->tl){
struct _tuple20 _tmp35A=*((struct _tuple20*)_tmp342->hd);struct _tuple0*_tmp35C;struct _tuple20 _tmp35B=_tmp35A;_tmp35C=_tmp35B.f2;
Cyc_Absyndump_dumpqvar(_tmp35C);
if(_tmp342->tl != 0)Cyc_Absyndump_dump_char((int)',');}{
# 1404
const char*_tmp524;Cyc_Absyndump_dump(((_tmp524="}",_tag_dyneither(_tmp524,sizeof(char),2))));};}
# 1406
goto _LL1F9;_LL214: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp343=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp321;if(_tmp343->tag != 13)goto _LL216;}_LL215:
# 1408
{const char*_tmp525;Cyc_Absyndump_dump(((_tmp525=" __cyclone_port_on__; ",_tag_dyneither(_tmp525,sizeof(char),23))));}
goto _LL1F9;_LL216: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp344=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp321;if(_tmp344->tag != 14)goto _LL1F9;}_LL217:
# 1411
{const char*_tmp526;Cyc_Absyndump_dump(((_tmp526=" __cyclone_port_off__; ",_tag_dyneither(_tmp526,sizeof(char),24))));}
goto _LL1F9;_LL1F9:;};}
# 1416
static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){
struct _tuple12 pr=Cyc_Evexp_eval_const_uint_exp(e);
if(pr.f1 != 1  || !pr.f2){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}
# 1423
void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a){
if(tms == 0){
f(a);
return;}{
# 1428
void*_tmp360=(void*)tms->hd;void*_tmp361=_tmp360;void*_tmp363;union Cyc_Absyn_Constraint*_tmp364;union Cyc_Absyn_Constraint*_tmp365;union Cyc_Absyn_Constraint*_tmp366;struct Cyc_Absyn_Tqual _tmp367;_LL224: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp362=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp361;if(_tmp362->tag != 2)goto _LL226;else{_tmp363=(_tmp362->f1).rgn;_tmp364=(_tmp362->f1).nullable;_tmp365=(_tmp362->f1).bounds;_tmp366=(_tmp362->f1).zero_term;_tmp367=_tmp362->f2;}}_LL225:
# 1432
{void*_tmp368=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp365);void*_tmp369=_tmp368;struct Cyc_Absyn_Exp*_tmp36C;_LL229: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp36A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp369;if(_tmp36A->tag != 0)goto _LL22B;}_LL22A:
 Cyc_Absyndump_dump_char((int)'?');goto _LL228;_LL22B: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp36B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp369;if(_tmp36B->tag != 1)goto _LL228;else{_tmp36C=_tmp36B->f1;}}_LL22C:
# 1435
 Cyc_Absyndump_dump_char((int)(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp364)?'*':'@'));
Cyc_Absyndump_dump_upperbound(_tmp36C);
goto _LL228;_LL228:;}
# 1439
if((!Cyc_Absyndump_qvar_to_Cids  && !is_char_ptr) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp366)){const char*_tmp527;Cyc_Absyndump_dump(((_tmp527="@zeroterm",_tag_dyneither(_tmp527,sizeof(char),10))));}
if((!Cyc_Absyndump_qvar_to_Cids  && is_char_ptr) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp366)){const char*_tmp528;Cyc_Absyndump_dump(((_tmp528="@nozeroterm",_tag_dyneither(_tmp528,sizeof(char),12))));}
{void*_tmp36F=Cyc_Tcutil_compress(_tmp363);void*_tmp370=_tmp36F;struct Cyc_Absyn_Tvar*_tmp375;_LL22E: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp371=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp370;if(_tmp371->tag != 20)goto _LL230;}_LL22F:
 if(!Cyc_Absyndump_qvar_to_Cids){const char*_tmp529;Cyc_Absyndump_dump(((_tmp529="`H",_tag_dyneither(_tmp529,sizeof(char),3))));}goto _LL22D;_LL230: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp372=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp370;if(_tmp372->tag != 21)goto _LL232;}_LL231:
{const char*_tmp52A;Cyc_Absyndump_dump(((_tmp52A="`U",_tag_dyneither(_tmp52A,sizeof(char),3))));}goto _LL22D;_LL232: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp373=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp370;if(_tmp373->tag != 22)goto _LL234;}_LL233:
{const char*_tmp52B;Cyc_Absyndump_dump(((_tmp52B="`RC",_tag_dyneither(_tmp52B,sizeof(char),4))));}goto _LL22D;_LL234: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp374=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp370;if(_tmp374->tag != 2)goto _LL236;else{_tmp375=_tmp374->f1;}}_LL235:
 Cyc_Absyndump_dumptvar(_tmp375);goto _LL22D;_LL236: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp376=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp370;if(_tmp376->tag != 1)goto _LL238;else{if((void*)_tmp376->f2 != 0)goto _LL238;}}_LL237:
 Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(_tmp363));goto _LL22D;_LL238: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp377=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp370;if(_tmp377->tag != 17)goto _LL23A;}_LL239:
{const char*_tmp52C;Cyc_Absyndump_dump(((_tmp52C="@region(",_tag_dyneither(_tmp52C,sizeof(char),9))));}Cyc_Absyndump_dumptyp(_tmp363);{const char*_tmp52D;Cyc_Absyndump_dump(((_tmp52D=")",_tag_dyneither(_tmp52D,sizeof(char),2))));}goto _LL22D;_LL23A:;_LL23B: {
const char*_tmp530;void*_tmp52F;(_tmp52F=0,Cyc_Tcutil_impos(((_tmp530="dumptms: bad rgn type in Pointer_mod",_tag_dyneither(_tmp530,sizeof(char),37))),_tag_dyneither(_tmp52F,sizeof(void*),0)));}_LL22D:;}
# 1450
Cyc_Absyndump_dumptq(_tmp367);
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
return;_LL226:;_LL227: {
# 1455
int next_is_pointer=0;
if(tms->tl != 0){
void*_tmp37F=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmp380=_tmp37F;_LL23D: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp381=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp380;if(_tmp381->tag != 2)goto _LL23F;}_LL23E:
 next_is_pointer=1;goto _LL23C;_LL23F:;_LL240:
 goto _LL23C;_LL23C:;}
# 1461
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)')');
{void*_tmp382=(void*)tms->hd;void*_tmp383=_tmp382;union Cyc_Absyn_Constraint*_tmp385;struct Cyc_Absyn_Exp*_tmp387;union Cyc_Absyn_Constraint*_tmp388;struct Cyc_List_List*_tmp38B;int _tmp38C;struct Cyc_Absyn_VarargInfo*_tmp38D;void*_tmp38E;struct Cyc_List_List*_tmp38F;struct Cyc_Absyn_Exp*_tmp390;struct Cyc_Absyn_Exp*_tmp391;struct Cyc_List_List*_tmp394;unsigned int _tmp395;struct Cyc_List_List*_tmp397;unsigned int _tmp398;int _tmp399;struct Cyc_List_List*_tmp39B;_LL242: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp384=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp383;if(_tmp384->tag != 0)goto _LL244;else{_tmp385=_tmp384->f1;}}_LL243:
# 1468
{const char*_tmp531;Cyc_Absyndump_dump(((_tmp531="[]",_tag_dyneither(_tmp531,sizeof(char),3))));}
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp385)){const char*_tmp532;Cyc_Absyndump_dump(((_tmp532="@zeroterm",_tag_dyneither(_tmp532,sizeof(char),10))));}
goto _LL241;_LL244: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp386=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp383;if(_tmp386->tag != 1)goto _LL246;else{_tmp387=_tmp386->f1;_tmp388=_tmp386->f2;}}_LL245:
# 1472
 Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp387);Cyc_Absyndump_dump_char((int)']');
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp388)){const char*_tmp533;Cyc_Absyndump_dump(((_tmp533="@zeroterm",_tag_dyneither(_tmp533,sizeof(char),10))));}
goto _LL241;_LL246: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp389=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp383;if(_tmp389->tag != 3)goto _LL248;else{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp38A=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((void*)_tmp389->f1);if(_tmp38A->tag != 1)goto _LL248;else{_tmp38B=_tmp38A->f1;_tmp38C=_tmp38A->f2;_tmp38D=_tmp38A->f3;_tmp38E=(void*)_tmp38A->f4;_tmp38F=_tmp38A->f5;_tmp390=_tmp38A->f6;_tmp391=_tmp38A->f7;}}}_LL247:
# 1476
 Cyc_Absyndump_dumpfunargs(_tmp38B,_tmp38C,_tmp38D,_tmp38E,_tmp38F,_tmp390,_tmp391);goto _LL241;_LL248: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp392=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp383;if(_tmp392->tag != 3)goto _LL24A;else{struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp393=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((void*)_tmp392->f1);if(_tmp393->tag != 0)goto _LL24A;else{_tmp394=_tmp393->f1;_tmp395=_tmp393->f2;}}}_LL249:
# 1478
{const char*_tmp536;const char*_tmp535;const char*_tmp534;((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,_tmp394,((_tmp534="(",_tag_dyneither(_tmp534,sizeof(char),2))),((_tmp535=")",_tag_dyneither(_tmp535,sizeof(char),2))),((_tmp536=",",_tag_dyneither(_tmp536,sizeof(char),2))));}goto _LL241;_LL24A: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp396=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp383;if(_tmp396->tag != 4)goto _LL24C;else{_tmp397=_tmp396->f1;_tmp398=_tmp396->f2;_tmp399=_tmp396->f3;}}_LL24B:
# 1480
 if(_tmp399)Cyc_Absyndump_dumpkindedtvars(_tmp397);else{Cyc_Absyndump_dumptvars(_tmp397);}goto _LL241;_LL24C: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp39A=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp383;if(_tmp39A->tag != 5)goto _LL24E;else{_tmp39B=_tmp39A->f2;}}_LL24D:
 Cyc_Absyndump_dumpatts(_tmp39B);goto _LL241;_LL24E: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp39C=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp383;if(_tmp39C->tag != 2)goto _LL241;}_LL24F: {
const char*_tmp539;void*_tmp538;(_tmp538=0,Cyc_Tcutil_impos(((_tmp539="dumptms",_tag_dyneither(_tmp539,sizeof(char),8))),_tag_dyneither(_tmp538,sizeof(void*),0)));}_LL241:;}
# 1484
return;}_LL223:;};}
# 1488
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
int _tmp3A5=Cyc_Absyndump_is_char_ptr(t);
struct _RegionHandle _tmp3A6=_new_region("temp");struct _RegionHandle*temp=& _tmp3A6;_push_region(temp);
{struct _tuple11 _tmp3A7=Cyc_Absynpp_to_tms(temp,tq,t);struct Cyc_Absyn_Tqual _tmp3A9;void*_tmp3AA;struct Cyc_List_List*_tmp3AB;struct _tuple11 _tmp3A8=_tmp3A7;_tmp3A9=_tmp3A8.f1;_tmp3AA=_tmp3A8.f2;_tmp3AB=_tmp3A8.f3;
Cyc_Absyndump_dumptq(_tmp3A9);
Cyc_Absyndump_dumpntyp(_tmp3AA);
Cyc_Absyndump_dumptms(_tmp3A5,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3AB),f,a);}
# 1491
;_pop_region(temp);}
# 1498
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
# 1500
*Cyc_Absyndump_dump_file=f;
for(0;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}{
const char*_tmp53C;void*_tmp53B;(_tmp53B=0,Cyc_fprintf(f,((_tmp53C="\n",_tag_dyneither(_tmp53C,sizeof(char),2))),_tag_dyneither(_tmp53B,sizeof(void*),0)));};}
# 1506
static void Cyc_Absyndump_dump_decl_interface(struct Cyc_Absyn_Decl*d){
void*_tmp3AE=d->r;void*_tmp3AF=_tmp3AE;struct Cyc_Absyn_Vardecl*_tmp3B1;struct Cyc_Absyn_Fndecl*_tmp3B3;struct Cyc_Absyn_Aggrdecl*_tmp3B5;struct Cyc_Absyn_Datatypedecl*_tmp3B7;struct Cyc_Absyn_Enumdecl*_tmp3B9;struct Cyc_Absyn_Typedefdecl*_tmp3BB;struct _dyneither_ptr*_tmp3BD;struct Cyc_List_List*_tmp3BE;struct Cyc_List_List*_tmp3C0;struct Cyc_List_List*_tmp3C2;struct Cyc_List_List*_tmp3C4;struct Cyc_List_List*_tmp3C5;_LL251: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp3B0=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp3AF;if(_tmp3B0->tag != 0)goto _LL253;else{_tmp3B1=_tmp3B0->f1;}}_LL252:
# 1509
 if(_tmp3B1->sc == Cyc_Absyn_Static)return;{
struct Cyc_Absyn_Exp*init=_tmp3B1->initializer;
_tmp3B1->initializer=0;
if(_tmp3B1->sc == Cyc_Absyn_Public){
const char*_tmp53D;Cyc_Absyndump_dump(((_tmp53D="extern ",_tag_dyneither(_tmp53D,sizeof(char),8))));}
Cyc_Absyndump_dumpvardecl(_tmp3B1,d->loc,1);
{const char*_tmp53E;Cyc_Absyndump_dump(((_tmp53E="\n",_tag_dyneither(_tmp53E,sizeof(char),2))));}
_tmp3B1->initializer=init;
goto _LL250;};_LL253: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp3B2=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp3AF;if(_tmp3B2->tag != 1)goto _LL255;else{_tmp3B3=_tmp3B2->f1;}}_LL254:
# 1519
 if(_tmp3B3->sc == Cyc_Absyn_Static)return;
Cyc_Absyndump_dumpscope(_tmp3B3->sc);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp544;struct Cyc_Absyn_FnInfo _tmp543;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp542;void*t=(void*)((_tmp542=_cycalloc(sizeof(*_tmp542)),((_tmp542[0]=((_tmp544.tag=9,((_tmp544.f1=((_tmp543.tvars=_tmp3B3->tvs,((_tmp543.effect=_tmp3B3->effect,((_tmp543.ret_tqual=_tmp3B3->ret_tqual,((_tmp543.ret_typ=_tmp3B3->ret_type,((_tmp543.args=
# 1523
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp3B3->args),((_tmp543.c_varargs=_tmp3B3->c_varargs,((_tmp543.cyc_varargs=_tmp3B3->cyc_varargs,((_tmp543.rgn_po=_tmp3B3->rgn_po,((_tmp543.attributes=0,((_tmp543.requires_clause=_tmp3B3->requires_clause,((_tmp543.requires_relns=_tmp3B3->requires_relns,((_tmp543.ensures_clause=_tmp3B3->ensures_clause,((_tmp543.ensures_relns=_tmp3B3->ensures_relns,_tmp543)))))))))))))))))))))))))),_tmp544)))),_tmp542))));
# 1528
((void(*)(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*a))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_atts_qvar,_tmp3B3);
{const char*_tmp545;Cyc_Absyndump_dump(((_tmp545=";\n",_tag_dyneither(_tmp545,sizeof(char),3))));}
goto _LL250;};_LL255: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp3B4=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp3AF;if(_tmp3B4->tag != 5)goto _LL257;else{_tmp3B5=_tmp3B4->f1;}}_LL256:
# 1532
 if(_tmp3B5->sc == Cyc_Absyn_Static)return;{
struct Cyc_Absyn_AggrdeclImpl*impl=_tmp3B5->impl;
if(_tmp3B5->sc == Cyc_Absyn_Abstract)_tmp3B5->impl=0;
Cyc_Absyndump_dump_aggrdecl(_tmp3B5);
_tmp3B5->impl=impl;
{const char*_tmp546;Cyc_Absyndump_dump(((_tmp546=";\n",_tag_dyneither(_tmp546,sizeof(char),3))));}
goto _LL250;};_LL257: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp3B6=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp3AF;if(_tmp3B6->tag != 6)goto _LL259;else{_tmp3B7=_tmp3B6->f1;}}_LL258:
# 1540
 Cyc_Absyndump_dump_datatypedecl(_tmp3B7);
{const char*_tmp547;Cyc_Absyndump_dump(((_tmp547=";\n",_tag_dyneither(_tmp547,sizeof(char),3))));}
goto _LL250;_LL259: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp3B8=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp3AF;if(_tmp3B8->tag != 7)goto _LL25B;else{_tmp3B9=_tmp3B8->f1;}}_LL25A:
# 1544
 Cyc_Absyndump_dump_enumdecl(_tmp3B9);
{const char*_tmp548;Cyc_Absyndump_dump(((_tmp548=";\n",_tag_dyneither(_tmp548,sizeof(char),3))));}
goto _LL250;_LL25B: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp3BA=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp3AF;if(_tmp3BA->tag != 8)goto _LL25D;else{_tmp3BB=_tmp3BA->f1;}}_LL25C:
# 1548
 if(_tmp3BB->defn == 0){
Cyc_Absyndump_dumpdecl(d);{
const char*_tmp549;Cyc_Absyndump_dump(((_tmp549="\n",_tag_dyneither(_tmp549,sizeof(char),2))));};}
# 1552
goto _LL250;_LL25D: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3BC=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp3AF;if(_tmp3BC->tag != 9)goto _LL25F;else{_tmp3BD=_tmp3BC->f1;_tmp3BE=_tmp3BC->f2;}}_LL25E:
# 1554
{const char*_tmp54A;Cyc_Absyndump_dump(((_tmp54A="namespace ",_tag_dyneither(_tmp54A,sizeof(char),11))));}
Cyc_Absyndump_dump_str(_tmp3BD);
{const char*_tmp54B;Cyc_Absyndump_dump(((_tmp54B="{\n",_tag_dyneither(_tmp54B,sizeof(char),3))));}
for(0;_tmp3BE != 0;_tmp3BE=_tmp3BE->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp3BE->hd);}
{const char*_tmp54C;Cyc_Absyndump_dump(((_tmp54C="}\n",_tag_dyneither(_tmp54C,sizeof(char),3))));}
goto _LL250;_LL25F: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp3BF=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp3AF;if(_tmp3BF->tag != 10)goto _LL261;else{_tmp3C0=_tmp3BF->f2;}}_LL260:
# 1562
 for(0;_tmp3C0 != 0;_tmp3C0=_tmp3C0->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp3C0->hd);}
goto _LL250;_LL261: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp3C1=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp3AF;if(_tmp3C1->tag != 11)goto _LL263;else{_tmp3C2=_tmp3C1->f1;}}_LL262:
# 1566
{const char*_tmp54D;Cyc_Absyndump_dump(((_tmp54D="extern \"C\" {",_tag_dyneither(_tmp54D,sizeof(char),13))));}
for(0;_tmp3C2 != 0;_tmp3C2=_tmp3C2->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp3C2->hd);}
{const char*_tmp54E;Cyc_Absyndump_dump(((_tmp54E="}\n",_tag_dyneither(_tmp54E,sizeof(char),3))));}
goto _LL250;_LL263: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3C3=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp3AF;if(_tmp3C3->tag != 12)goto _LL265;else{_tmp3C4=_tmp3C3->f1;_tmp3C5=_tmp3C3->f2;}}_LL264:
# 1587 "absyndump.cyc"
 goto _LL250;_LL265:;_LL266: {
const char*_tmp551;void*_tmp550;(_tmp550=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp551="bad top-level declaration",_tag_dyneither(_tmp551,sizeof(char),26))),_tag_dyneither(_tmp550,sizeof(void*),0)));}_LL250:;}
# 1592
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f){
*Cyc_Absyndump_dump_file=f;
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}}
