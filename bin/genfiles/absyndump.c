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
# 83
typedef void*Cyc_Tcpat_decision_opt_t;
# 91
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 103
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 107
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 110
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
# 115
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
# 166
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};
# 187
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 192
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 199
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 219
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 221
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 230
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 291
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 390
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 395
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 466 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 469
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 505
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
# 513
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 533
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 559
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 566
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 584
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 750 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 954
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 964
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 972
void*Cyc_Absyn_compress_kb(void*);
# 977
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 1007
extern void*Cyc_Absyn_bounds_one;
# 1062
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1089
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1194
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1200
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
char _tmp0=Cyc_Absyndump_prev_char;switch(_tmp0){case 123: _LL1: _LL2:
 goto _LL4;case 125: _LL3: _LL4:
 goto _LL6;case 40: _LL5: _LL6:
 goto _LL8;case 41: _LL7: _LL8:
 goto _LLA;case 91: _LL9: _LLA:
 goto _LLC;case 93: _LLB: _LLC:
 goto _LLE;case 59: _LLD: _LLE:
 goto _LL10;case 44: _LLF: _LL10:
 goto _LL12;case 61: _LL11: _LL12:
 goto _LL14;case 63: _LL13: _LL14:
 goto _LL16;case 33: _LL15: _LL16:
 goto _LL18;case 32: _LL17: _LL18:
 goto _LL1A;case 10: _LL19: _LL1A:
 goto _LL1C;case 42: _LL1B: _LL1C:
 return 0;default: _LL1D: _LL1E:
 return 1;}_LL0:;}
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
Cyc_Absyndump_prev_char=(char)c;}static char _tmpE[1]="";
# 155
void Cyc_Absyndump_dumploc(unsigned int loc){
static struct _dyneither_ptr last_file={_tmpE,_tmpE,_tmpE + 1};
static unsigned int last_line=0;
if(loc == 0)return;
if(!Cyc_Absyndump_generate_line_directives)return;{
# 161
struct _tuple13 _tmp1=Cyc_Lex_xlate_pos(loc);struct _tuple13 _tmp2=_tmp1;struct _dyneither_ptr _tmp3;unsigned int _tmp4;_LL20: _tmp3=_tmp2.f1;_tmp4=_tmp2.f2;_LL21:
 if((char*)_tmp3.curr == (char*)(_tag_dyneither(0,0,0)).curr  || (char*)_tmp3.curr == (char*)last_file.curr  && _tmp4 == last_line)return;
if((char*)_tmp3.curr == (char*)last_file.curr  && _tmp4 == last_line + 1){
const char*_tmp301;void*_tmp300;(_tmp300=0,Cyc_fprintf(*Cyc_Absyndump_dump_file,((_tmp301="\n",_tag_dyneither(_tmp301,sizeof(char),2))),_tag_dyneither(_tmp300,sizeof(void*),0)));}else{
if((char*)_tmp3.curr == (char*)last_file.curr){
const char*_tmp305;void*_tmp304[1];struct Cyc_Int_pa_PrintArg_struct _tmp303;(_tmp303.tag=1,((_tmp303.f1=(unsigned long)((int)_tmp4),((_tmp304[0]=& _tmp303,Cyc_fprintf(*Cyc_Absyndump_dump_file,((_tmp305="\n# %d\n",_tag_dyneither(_tmp305,sizeof(char),7))),_tag_dyneither(_tmp304,sizeof(void*),1)))))));}else{
# 168
const char*_tmp30A;void*_tmp309[2];struct Cyc_Int_pa_PrintArg_struct _tmp308;struct Cyc_String_pa_PrintArg_struct _tmp307;(_tmp307.tag=0,((_tmp307.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3),((_tmp308.tag=1,((_tmp308.f1=(unsigned long)((int)_tmp4),((_tmp309[0]=& _tmp308,((_tmp309[1]=& _tmp307,Cyc_fprintf(*Cyc_Absyndump_dump_file,((_tmp30A="\n# %d %s\n",_tag_dyneither(_tmp30A,sizeof(char),10))),_tag_dyneither(_tmp309,sizeof(void*),2)))))))))))));}}
# 170
last_file=_tmp3;
last_line=_tmp4;_LL1F:;};}
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
struct Cyc_List_List*_tmpF=0;
struct _dyneither_ptr**prefix=0;
{union Cyc_Absyn_Nmspace _tmp10=(*v).f1;union Cyc_Absyn_Nmspace _tmp11=_tmp10;struct Cyc_List_List*_tmp12;struct Cyc_List_List*_tmp13;struct Cyc_List_List*_tmp14;switch((_tmp11.C_n).tag){case 4: _LL25: _LL26:
 _tmp14=0;goto _LL28;case 1: _LL27: _tmp14=(_tmp11.Rel_n).val;_LL28:
# 228
 _tmpF=_tmp14;
goto _LL24;case 3: _LL29: _tmp13=(_tmp11.C_n).val;_LL2A:
# 234
 Cyc_Absyndump_dump_char((int)' ');
goto _LL24;default: _LL2B: _tmp12=(_tmp11.Abs_n).val;_LL2C:
# 237
 if(Cyc_Absyndump_qvar_to_Cids  || Cyc_Absyndump_add_cyc_prefix)
prefix=& Cyc_Absynpp_cyc_stringptr;
_tmpF=_tmp12;
goto _LL24;}_LL24:;}
# 242
if(prefix != 0){
Cyc_Absyndump_dump_str(*prefix);
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{
const char*_tmp30B;Cyc_Absyndump_dump_nospace(((_tmp30B="::",_tag_dyneither(_tmp30B,sizeof(char),3))));}}
# 247
if(_tmpF != 0){
Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmpF->hd));
for(_tmpF=_tmpF->tl;_tmpF != 0;_tmpF=_tmpF->tl){
# 252
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{
const char*_tmp30C;Cyc_Absyndump_dump_nospace(((_tmp30C="::",_tag_dyneither(_tmp30C,sizeof(char),3))));}
Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmpF->hd));}
# 256
if(Cyc_Absyndump_qvar_to_Cids){const char*_tmp30D;Cyc_Absyndump_dump_nospace(((_tmp30D="_",_tag_dyneither(_tmp30D,sizeof(char),2))));}else{
const char*_tmp30E;Cyc_Absyndump_dump_nospace(((_tmp30E="::",_tag_dyneither(_tmp30E,sizeof(char),3))));}
Cyc_Absyndump_dump_nospace(*(*v).f2);}else{
# 260
if(prefix != 0)
Cyc_Absyndump_dump_nospace(*(*v).f2);else{
# 263
Cyc_Absyndump_dump_str((*v).f2);}}}
# 267
void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){
if(tq.q_restrict){const char*_tmp30F;Cyc_Absyndump_dump(((_tmp30F="restrict",_tag_dyneither(_tmp30F,sizeof(char),9))));}
if(tq.q_volatile){const char*_tmp310;Cyc_Absyndump_dump(((_tmp310="volatile",_tag_dyneither(_tmp310,sizeof(char),9))));}
if(tq.print_const){const char*_tmp311;Cyc_Absyndump_dump(((_tmp311="const",_tag_dyneither(_tmp311,sizeof(char),6))));}}
# 273
void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc){
enum Cyc_Absyn_Scope _tmp1C=sc;switch(_tmp1C){case Cyc_Absyn_Static: _LL2E: _LL2F:
{const char*_tmp312;Cyc_Absyndump_dump(((_tmp312="static",_tag_dyneither(_tmp312,sizeof(char),7))));}return;case Cyc_Absyn_Public: _LL30: _LL31:
 return;case Cyc_Absyn_Extern: _LL32: _LL33:
{const char*_tmp313;Cyc_Absyndump_dump(((_tmp313="extern",_tag_dyneither(_tmp313,sizeof(char),7))));}return;case Cyc_Absyn_ExternC: _LL34: _LL35:
{const char*_tmp314;Cyc_Absyndump_dump(((_tmp314="extern \"C\"",_tag_dyneither(_tmp314,sizeof(char),11))));}return;case Cyc_Absyn_Abstract: _LL36: _LL37:
{const char*_tmp315;Cyc_Absyndump_dump(((_tmp315="abstract",_tag_dyneither(_tmp315,sizeof(char),9))));}return;default: _LL38: _LL39:
{const char*_tmp316;Cyc_Absyndump_dump(((_tmp316="register",_tag_dyneither(_tmp316,sizeof(char),9))));}return;}_LL2D:;}
# 284
void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp22=ka;enum Cyc_Absyn_KindQual _tmp23;enum Cyc_Absyn_AliasQual _tmp24;_LL3B: _tmp23=_tmp22->kind;_tmp24=_tmp22->aliasqual;_LL3C: {
enum Cyc_Absyn_KindQual _tmp25=_tmp23;switch(_tmp25){case Cyc_Absyn_AnyKind: _LL40: _LL41: {
# 288
enum Cyc_Absyn_AliasQual _tmp26=_tmp24;switch(_tmp26){case Cyc_Absyn_Aliasable: _LL4D: _LL4E:
{const char*_tmp317;Cyc_Absyndump_dump(((_tmp317="A",_tag_dyneither(_tmp317,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL4F: _LL50:
{const char*_tmp318;Cyc_Absyndump_dump(((_tmp318="UA",_tag_dyneither(_tmp318,sizeof(char),3))));}return;default: _LL51: _LL52:
{const char*_tmp319;Cyc_Absyndump_dump(((_tmp319="TA",_tag_dyneither(_tmp319,sizeof(char),3))));}return;}_LL4C:;}case Cyc_Absyn_MemKind: _LL42: _LL43: {
# 294
enum Cyc_Absyn_AliasQual _tmp2A=_tmp24;switch(_tmp2A){case Cyc_Absyn_Aliasable: _LL54: _LL55:
{const char*_tmp31A;Cyc_Absyndump_dump(((_tmp31A="M",_tag_dyneither(_tmp31A,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL56: _LL57:
{const char*_tmp31B;Cyc_Absyndump_dump(((_tmp31B="UM",_tag_dyneither(_tmp31B,sizeof(char),3))));}return;default: _LL58: _LL59:
{const char*_tmp31C;Cyc_Absyndump_dump(((_tmp31C="TM",_tag_dyneither(_tmp31C,sizeof(char),3))));}return;}_LL53:;}case Cyc_Absyn_BoxKind: _LL44: _LL45: {
# 300
enum Cyc_Absyn_AliasQual _tmp2E=_tmp24;switch(_tmp2E){case Cyc_Absyn_Aliasable: _LL5B: _LL5C:
{const char*_tmp31D;Cyc_Absyndump_dump(((_tmp31D="B",_tag_dyneither(_tmp31D,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL5D: _LL5E:
{const char*_tmp31E;Cyc_Absyndump_dump(((_tmp31E="UB",_tag_dyneither(_tmp31E,sizeof(char),3))));}return;default: _LL5F: _LL60:
{const char*_tmp31F;Cyc_Absyndump_dump(((_tmp31F="TB",_tag_dyneither(_tmp31F,sizeof(char),3))));}return;}_LL5A:;}case Cyc_Absyn_RgnKind: _LL46: _LL47: {
# 306
enum Cyc_Absyn_AliasQual _tmp32=_tmp24;switch(_tmp32){case Cyc_Absyn_Aliasable: _LL62: _LL63:
{const char*_tmp320;Cyc_Absyndump_dump(((_tmp320="R",_tag_dyneither(_tmp320,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL64: _LL65:
{const char*_tmp321;Cyc_Absyndump_dump(((_tmp321="UR",_tag_dyneither(_tmp321,sizeof(char),3))));}return;default: _LL66: _LL67:
{const char*_tmp322;Cyc_Absyndump_dump(((_tmp322="TR",_tag_dyneither(_tmp322,sizeof(char),3))));}return;}_LL61:;}case Cyc_Absyn_EffKind: _LL48: _LL49:
# 311
{const char*_tmp323;Cyc_Absyndump_dump(((_tmp323="E",_tag_dyneither(_tmp323,sizeof(char),2))));}return;default: _LL4A: _LL4B:
{const char*_tmp324;Cyc_Absyndump_dump(((_tmp324="I",_tag_dyneither(_tmp324,sizeof(char),2))));}return;}_LL3F:;}_LL3A:;}
# 316
void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp38=k;if(_tmp38 == Cyc_Absyn_StructA){_LL69: _LL6A:
{const char*_tmp325;Cyc_Absyndump_dump(((_tmp325="struct ",_tag_dyneither(_tmp325,sizeof(char),8))));}return;}else{_LL6B: _LL6C:
{const char*_tmp326;Cyc_Absyndump_dump(((_tmp326="union ",_tag_dyneither(_tmp326,sizeof(char),7))));}return;}_LL68:;}
# 323
void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){
const char*_tmp329;const char*_tmp328;const char*_tmp327;((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptyp,ts,((_tmp327="<",_tag_dyneither(_tmp327,sizeof(char),2))),((_tmp328=">",_tag_dyneither(_tmp328,sizeof(char),2))),((_tmp329=",",_tag_dyneither(_tmp329,sizeof(char),2))));}
# 326
void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr n=*tv->name;
if(*((const char*)_check_dyneither_subscript(n,sizeof(char),0))== '#'){
{const char*_tmp32A;Cyc_Absyndump_dump(((_tmp32A="`G",_tag_dyneither(_tmp32A,sizeof(char),3))));}
{void*_tmp3F=Cyc_Absyn_compress_kb(tv->kind);void*_tmp40=_tmp3F;struct Cyc_Absyn_Kind*_tmp41;struct Cyc_Absyn_Kind*_tmp42;switch(*((int*)_tmp40)){case 0: _LL6E: _tmp42=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp40)->f1;_LL6F:
 _tmp41=_tmp42;goto _LL71;case 2: _LL70: _tmp41=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp40)->f2;_LL71:
 Cyc_Absyndump_dumpkind(_tmp41);goto _LL6D;default: _LL72: _LL73:
{const char*_tmp32B;Cyc_Absyndump_dump_nospace(((_tmp32B="K",_tag_dyneither(_tmp32B,sizeof(char),2))));}goto _LL6D;}_LL6D:;}
# 335
Cyc_Absyndump_dump_nospace(_dyneither_ptr_plus(n,sizeof(char),1));}else{
# 337
Cyc_Absyndump_dump(n);}}
# 339
void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){
Cyc_Absyndump_dumptvar(tv);{
void*_tmp44=Cyc_Absyn_compress_kb(tv->kind);void*_tmp45=_tmp44;struct Cyc_Absyn_Kind*_tmp46;struct Cyc_Absyn_Kind*_tmp47;switch(*((int*)_tmp45)){case 1: _LL75: _LL76:
 goto _LL74;case 2: _LL77: _tmp47=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp45)->f2;_LL78:
 goto _LL74;default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp45)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp45)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL79: _LL7A:
 goto _LL74;}else{goto _LL7B;}}else{_LL7B: _tmp46=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp45)->f1;_LL7C:
{const char*_tmp32C;Cyc_Absyndump_dump(((_tmp32C="::",_tag_dyneither(_tmp32C,sizeof(char),3))));}Cyc_Absyndump_dumpkind(_tmp46);goto _LL74;}}_LL74:;};}
# 348
void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){
const char*_tmp32F;const char*_tmp32E;const char*_tmp32D;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,tvs,((_tmp32D="<",_tag_dyneither(_tmp32D,sizeof(char),2))),((_tmp32E=">",_tag_dyneither(_tmp32E,sizeof(char),2))),((_tmp32F=",",_tag_dyneither(_tmp32F,sizeof(char),2))));}
# 351
void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){
const char*_tmp332;const char*_tmp331;const char*_tmp330;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,tvs,((_tmp330="<",_tag_dyneither(_tmp330,sizeof(char),2))),((_tmp331=">",_tag_dyneither(_tmp331,sizeof(char),2))),((_tmp332=",",_tag_dyneither(_tmp332,sizeof(char),2))));}struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;};
# 355
void Cyc_Absyndump_dumparg(struct _tuple14*pr){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 358
void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){
const char*_tmp335;const char*_tmp334;const char*_tmp333;((void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,ts,((_tmp333="(",_tag_dyneither(_tmp333,sizeof(char),2))),((_tmp334=")",_tag_dyneither(_tmp334,sizeof(char),2))),((_tmp335=",",_tag_dyneither(_tmp335,sizeof(char),2))));}
# 362
void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp52=(void*)atts->hd;void*_tmp53=_tmp52;switch(*((int*)_tmp53)){case 1: _LL7E: _LL7F:
{const char*_tmp336;Cyc_Absyndump_dump(((_tmp336="_stdcall",_tag_dyneither(_tmp336,sizeof(char),9))));}return;case 2: _LL80: _LL81:
{const char*_tmp337;Cyc_Absyndump_dump(((_tmp337="_cdecl",_tag_dyneither(_tmp337,sizeof(char),7))));}return;case 3: _LL82: _LL83:
{const char*_tmp338;Cyc_Absyndump_dump(((_tmp338="_fastcall",_tag_dyneither(_tmp338,sizeof(char),10))));}return;default: _LL84: _LL85:
 goto _LL7D;}_LL7D:;}}
# 372
void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){
# 374
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp57=(void*)((struct Cyc_List_List*)_check_null(atts))->hd;void*_tmp58=_tmp57;switch(*((int*)_tmp58)){case 1: _LL87: _LL88:
 goto _LL8A;case 2: _LL89: _LL8A:
 goto _LL8C;case 3: _LL8B: _LL8C:
 goto _LL86;default: _LL8D: _LL8E:
 hasatt=1;goto _LL86;}_LL86:;}}
# 382
if(!hasatt)
return;
{const char*_tmp339;Cyc_Absyndump_dump(((_tmp339="__declspec(",_tag_dyneither(_tmp339,sizeof(char),12))));}
for(0;atts != 0;atts=atts->tl){
void*_tmp5A=(void*)atts->hd;void*_tmp5B=_tmp5A;switch(*((int*)_tmp5B)){case 1: _LL90: _LL91:
 goto _LL93;case 2: _LL92: _LL93:
 goto _LL95;case 3: _LL94: _LL95:
 goto _LL8F;default: _LL96: _LL97:
 Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL8F;}_LL8F:;}
# 392
Cyc_Absyndump_dump_char((int)')');}
# 395
void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){
if(atts == 0)return;{
enum Cyc_Cyclone_C_Compilers _tmp5C=Cyc_Cyclone_c_compiler;if(_tmp5C == Cyc_Cyclone_Gcc_c){_LL99: _LL9A:
# 399
{const char*_tmp33A;Cyc_Absyndump_dump(((_tmp33A=" __attribute__((",_tag_dyneither(_tmp33A,sizeof(char),17))));}
for(0;atts != 0;atts=atts->tl){
Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));
if(atts->tl != 0){const char*_tmp33B;Cyc_Absyndump_dump(((_tmp33B=",",_tag_dyneither(_tmp33B,sizeof(char),2))));}}
# 404
{const char*_tmp33C;Cyc_Absyndump_dump(((_tmp33C=")) ",_tag_dyneither(_tmp33C,sizeof(char),4))));}
return;}else{_LL9B: _LL9C:
# 407
 Cyc_Absyndump_dump_noncallconv(atts);
return;}_LL98:;};}
# 412
int Cyc_Absyndump_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmp60=(void*)tms->hd;void*_tmp61=_tmp60;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp61)->tag == 2){_LL9E: _LL9F:
 return 1;}else{_LLA0: _LLA1:
 return 0;}_LL9D:;};}
# 420
static void Cyc_Absyndump_dumprgn(void*t){
void*_tmp62=Cyc_Tcutil_compress(t);void*_tmp63=_tmp62;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp63)->tag == 20){_LLA3: _LLA4:
{const char*_tmp33D;Cyc_Absyndump_dump(((_tmp33D="`H",_tag_dyneither(_tmp33D,sizeof(char),3))));}goto _LLA2;}else{_LLA5: _LLA6:
 Cyc_Absyndump_dumpntyp(t);goto _LLA2;}_LLA2:;}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 427
static struct _tuple15 Cyc_Absyndump_effects_split(void*t){
struct Cyc_List_List*rgions=0;
struct Cyc_List_List*effects=0;
# 431
{void*_tmp65=Cyc_Tcutil_compress(t);void*_tmp66=_tmp65;struct Cyc_List_List*_tmp67;void*_tmp68;switch(*((int*)_tmp66)){case 23: _LLA8: _tmp68=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp66)->f1;_LLA9:
{struct Cyc_List_List*_tmp33E;rgions=((_tmp33E=_cycalloc(sizeof(*_tmp33E)),((_tmp33E->hd=_tmp68,((_tmp33E->tl=rgions,_tmp33E))))));}goto _LLA7;case 24: _LLAA: _tmp67=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp66)->f1;_LLAB:
# 434
 for(0;_tmp67 != 0;_tmp67=_tmp67->tl){
struct _tuple15 _tmp6A=Cyc_Absyndump_effects_split((void*)_tmp67->hd);struct _tuple15 _tmp6B=_tmp6A;struct Cyc_List_List*_tmp6C;struct Cyc_List_List*_tmp6D;_LLAF: _tmp6C=_tmp6B.f1;_tmp6D=_tmp6B.f2;_LLB0:
 rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp6C,rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp6D,effects);_LLAE:;}
# 439
goto _LLA7;default: _LLAC: _LLAD:
{struct Cyc_List_List*_tmp33F;effects=((_tmp33F=_cycalloc(sizeof(*_tmp33F)),((_tmp33F->hd=t,((_tmp33F->tl=effects,_tmp33F))))));}goto _LLA7;}_LLA7:;}{
# 442
struct _tuple15 _tmp340;return(_tmp340.f1=rgions,((_tmp340.f2=effects,_tmp340)));};}
# 445
static void Cyc_Absyndump_dumpeff(void*t){
struct _tuple15 _tmp70=Cyc_Absyndump_effects_split(t);struct _tuple15 _tmp71=_tmp70;struct Cyc_List_List*_tmp72;struct Cyc_List_List*_tmp73;_LLB4: _tmp72=_tmp71.f1;_tmp73=_tmp71.f2;_LLB5:
 _tmp72=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp72);
_tmp73=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp73);
if(_tmp73 != 0){
{struct Cyc_List_List*_tmp74=_tmp73;for(0;_tmp74 != 0;_tmp74=_tmp74->tl){
Cyc_Absyndump_dumpntyp((void*)_tmp74->hd);
if(_tmp74->tl != 0)
Cyc_Absyndump_dump_char((int)'+');}}
# 455
if(_tmp72 != 0)Cyc_Absyndump_dump_char((int)'+');}
# 457
if(_tmp72 != 0  || _tmp73 == 0){
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp72 != 0;_tmp72=_tmp72->tl){
Cyc_Absyndump_dumprgn((void*)_tmp72->hd);
if(_tmp72->tl != 0)Cyc_Absyndump_dump_char((int)',');}
# 463
Cyc_Absyndump_dump_char((int)'}');}_LLB3:;}
# 467
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*);
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*);
# 472
void Cyc_Absyndump_dumpntyp(void*t){
void*_tmp75=t;struct Cyc_List_List*_tmp76;void*_tmp77;void*_tmp78;struct Cyc_Absyn_Datatypedecl*_tmp79;struct Cyc_Absyn_Enumdecl*_tmp7A;struct Cyc_Absyn_Aggrdecl*_tmp7B;void*_tmp7C;void*_tmp7D;void*_tmp7E;void*_tmp7F;struct _dyneither_ptr _tmp80;struct Cyc_Absyn_Exp*_tmp81;struct Cyc_Absyn_Exp*_tmp82;struct _tuple0*_tmp83;struct Cyc_List_List*_tmp84;struct Cyc_List_List*_tmp85;struct _tuple0*_tmp86;enum Cyc_Absyn_AggrKind _tmp87;struct Cyc_List_List*_tmp88;union Cyc_Absyn_AggrInfoU _tmp89;struct Cyc_List_List*_tmp8A;struct Cyc_List_List*_tmp8B;int _tmp8C;union Cyc_Absyn_DatatypeFieldInfoU _tmp8D;struct Cyc_List_List*_tmp8E;union Cyc_Absyn_DatatypeInfoU _tmp8F;struct Cyc_List_List*_tmp90;struct Cyc_Core_Opt*_tmp91;void*_tmp92;int _tmp93;struct Cyc_Core_Opt*_tmp94;int _tmp95;struct Cyc_Absyn_Tvar*_tmp96;switch(*((int*)_tmp75)){case 8: _LLB9: _LLBA:
# 475
 goto _LLBC;case 9: _LLBB: _LLBC:
 goto _LLBE;case 5: _LLBD: _LLBE:
 return;case 0: _LLBF: _LLC0:
{const char*_tmp341;Cyc_Absyndump_dump(((_tmp341="void",_tag_dyneither(_tmp341,sizeof(char),5))));}return;case 2: _LLC1: _tmp96=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp75)->f1;_LLC2:
 Cyc_Absyndump_dumptvar(_tmp96);return;case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp75)->f2 == 0){_LLC3: _tmp94=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp75)->f1;_tmp95=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp75)->f3;_LLC4:
# 481
{const char*_tmp342;Cyc_Absyndump_dump(((_tmp342="`E",_tag_dyneither(_tmp342,sizeof(char),3))));}
if(_tmp94 == 0){const char*_tmp343;Cyc_Absyndump_dump(((_tmp343="K",_tag_dyneither(_tmp343,sizeof(char),2))));}else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)_tmp94->v);}
{const char*_tmp347;void*_tmp346[1];struct Cyc_Int_pa_PrintArg_struct _tmp345;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp345.tag=1,((_tmp345.f1=(unsigned long)_tmp95,((_tmp346[0]=& _tmp345,Cyc_aprintf(((_tmp347="%d",_tag_dyneither(_tmp347,sizeof(char),3))),_tag_dyneither(_tmp346,sizeof(void*),1)))))))));}return;}else{_LLC5: _tmp91=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp75)->f1;_tmp92=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp75)->f2;_tmp93=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp75)->f3;_LLC6:
 Cyc_Absyndump_dumpntyp((void*)_check_null(_tmp92));return;}case 3: _LLC7: _tmp8F=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp75)->f1).datatype_info;_tmp90=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp75)->f1).targs;_LLC8:
# 486
{union Cyc_Absyn_DatatypeInfoU _tmp9D=_tmp8F;struct _tuple0*_tmp9E;int _tmp9F;struct _tuple0*_tmpA0;int _tmpA1;if((_tmp9D.UnknownDatatype).tag == 1){_LL116: _tmpA0=((_tmp9D.UnknownDatatype).val).name;_tmpA1=((_tmp9D.UnknownDatatype).val).is_extensible;_LL117:
 _tmp9E=_tmpA0;_tmp9F=_tmpA1;goto _LL119;}else{_LL118: _tmp9E=(*(_tmp9D.KnownDatatype).val)->name;_tmp9F=(*(_tmp9D.KnownDatatype).val)->is_extensible;_LL119:
# 489
 if(_tmp9F){const char*_tmp348;Cyc_Absyndump_dump(((_tmp348="@extensible ",_tag_dyneither(_tmp348,sizeof(char),13))));}
{const char*_tmp349;Cyc_Absyndump_dump(((_tmp349="datatype ",_tag_dyneither(_tmp349,sizeof(char),10))));}
Cyc_Absyndump_dumpqvar(_tmp9E);
Cyc_Absyndump_dumptps(_tmp90);
goto _LL115;}_LL115:;}
# 495
goto _LLB8;case 4: _LLC9: _tmp8D=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp75)->f1).field_info;_tmp8E=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp75)->f1).targs;_LLCA:
# 497
{union Cyc_Absyn_DatatypeFieldInfoU _tmpA4=_tmp8D;struct _tuple0*_tmpA5;int _tmpA6;struct _tuple0*_tmpA7;struct _tuple0*_tmpA8;struct _tuple0*_tmpA9;int _tmpAA;if((_tmpA4.UnknownDatatypefield).tag == 1){_LL11B: _tmpA8=((_tmpA4.UnknownDatatypefield).val).datatype_name;_tmpA9=((_tmpA4.UnknownDatatypefield).val).field_name;_tmpAA=((_tmpA4.UnknownDatatypefield).val).is_extensible;_LL11C:
# 499
 _tmpA5=_tmpA8;_tmpA6=_tmpAA;_tmpA7=_tmpA9;goto _LL11E;}else{_LL11D: _tmpA5=(((_tmpA4.KnownDatatypefield).val).f1)->name;_tmpA6=(((_tmpA4.KnownDatatypefield).val).f1)->is_extensible;_tmpA7=(((_tmpA4.KnownDatatypefield).val).f2)->name;_LL11E:
# 502
 if(_tmpA6){const char*_tmp34A;Cyc_Absyndump_dump(((_tmp34A="@extensible ",_tag_dyneither(_tmp34A,sizeof(char),13))));}
{const char*_tmp34B;Cyc_Absyndump_dump(((_tmp34B="datatype ",_tag_dyneither(_tmp34B,sizeof(char),10))));}
Cyc_Absyndump_dumpqvar(_tmpA5);
{const char*_tmp34C;Cyc_Absyndump_dump(((_tmp34C=".",_tag_dyneither(_tmp34C,sizeof(char),2))));}
Cyc_Absyndump_dumpqvar(_tmpA7);
Cyc_Absyndump_dumptps(_tmp8E);
goto _LL11A;}_LL11A:;}
# 510
goto _LLB8;case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75)->f1){case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75)->f2){case Cyc_Absyn_Int_sz: _LLCB: _LLCC:
 goto _LLCE;case Cyc_Absyn_Long_sz: _LLCF: _LLD0:
# 513
 goto _LLD2;case Cyc_Absyn_Char_sz: _LLD3: _LLD4:
# 515
{const char*_tmp34D;Cyc_Absyndump_dump(((_tmp34D="char",_tag_dyneither(_tmp34D,sizeof(char),5))));}return;case Cyc_Absyn_Short_sz: _LLD9: _LLDA:
# 518
 goto _LLDC;default: _LLE3: _LLE4:
# 523
 goto _LLE6;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75)->f2){case Cyc_Absyn_Int_sz: _LLCD: _LLCE:
# 512
{const char*_tmp34E;Cyc_Absyndump_dump(((_tmp34E="int",_tag_dyneither(_tmp34E,sizeof(char),4))));}return;case Cyc_Absyn_Long_sz: _LLD1: _LLD2:
# 514
{const char*_tmp34F;Cyc_Absyndump_dump(((_tmp34F="long",_tag_dyneither(_tmp34F,sizeof(char),5))));}return;case Cyc_Absyn_Char_sz: _LLD5: _LLD6:
# 516
{const char*_tmp350;Cyc_Absyndump_dump(((_tmp350="signed char",_tag_dyneither(_tmp350,sizeof(char),12))));}return;case Cyc_Absyn_Short_sz: _LLDB: _LLDC:
# 519
{const char*_tmp351;Cyc_Absyndump_dump(((_tmp351="short",_tag_dyneither(_tmp351,sizeof(char),6))));}return;default: _LLE5: _LLE6: {
# 525
enum Cyc_Cyclone_C_Compilers _tmpB7=Cyc_Cyclone_c_compiler;if(_tmpB7 == Cyc_Cyclone_Gcc_c){_LL120: _LL121:
{const char*_tmp352;Cyc_Absyndump_dump(((_tmp352="long long",_tag_dyneither(_tmp352,sizeof(char),10))));}return;}else{_LL122: _LL123:
{const char*_tmp353;Cyc_Absyndump_dump(((_tmp353="__int64",_tag_dyneither(_tmp353,sizeof(char),8))));}return;}_LL11F:;}}default: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75)->f2){case Cyc_Absyn_Char_sz: _LLD7: _LLD8:
# 517
{const char*_tmp354;Cyc_Absyndump_dump(((_tmp354="unsigned char",_tag_dyneither(_tmp354,sizeof(char),14))));}return;case Cyc_Absyn_Short_sz: _LLDD: _LLDE:
# 520
{const char*_tmp355;Cyc_Absyndump_dump(((_tmp355="unsigned short",_tag_dyneither(_tmp355,sizeof(char),15))));}return;case Cyc_Absyn_Int_sz: _LLDF: _LLE0:
{const char*_tmp356;Cyc_Absyndump_dump(((_tmp356="unsigned int",_tag_dyneither(_tmp356,sizeof(char),13))));}return;case Cyc_Absyn_Long_sz: _LLE1: _LLE2:
{const char*_tmp357;Cyc_Absyndump_dump(((_tmp357="unsigned long",_tag_dyneither(_tmp357,sizeof(char),14))));}return;default: _LLE7: _LLE8: {
# 530
enum Cyc_Cyclone_C_Compilers _tmpBA=Cyc_Cyclone_c_compiler;if(_tmpBA == Cyc_Cyclone_Vc_c){_LL125: _LL126:
{const char*_tmp358;Cyc_Absyndump_dump(((_tmp358="unsigned __int64",_tag_dyneither(_tmp358,sizeof(char),17))));}return;}else{_LL127: _LL128:
{const char*_tmp359;Cyc_Absyndump_dump(((_tmp359="unsigned long long",_tag_dyneither(_tmp359,sizeof(char),19))));}return;}_LL124:;}}}case 7: _LLE9: _tmp8C=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp75)->f1;_LLEA:
# 535
 if(_tmp8C == 0){
const char*_tmp35A;Cyc_Absyndump_dump(((_tmp35A="float",_tag_dyneither(_tmp35A,sizeof(char),6))));}else{
if(_tmp8C == 1){
const char*_tmp35B;Cyc_Absyndump_dump(((_tmp35B="double",_tag_dyneither(_tmp35B,sizeof(char),7))));}else{
# 540
const char*_tmp35C;Cyc_Absyndump_dump(((_tmp35C="long double",_tag_dyneither(_tmp35C,sizeof(char),12))));}}
return;case 10: _LLEB: _tmp8B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp75)->f1;_LLEC:
 Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(_tmp8B);return;case 11: _LLED: _tmp89=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp75)->f1).aggr_info;_tmp8A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp75)->f1).targs;_LLEE: {
# 544
struct _tuple10 _tmpC0=Cyc_Absyn_aggr_kinded_name(_tmp89);struct _tuple10 _tmpC1=_tmpC0;enum Cyc_Absyn_AggrKind _tmpC2;struct _tuple0*_tmpC3;_LL12A: _tmpC2=_tmpC1.f1;_tmpC3=_tmpC1.f2;_LL12B:
 Cyc_Absyndump_dumpaggr_kind(_tmpC2);Cyc_Absyndump_dumpqvar(_tmpC3);Cyc_Absyndump_dumptps(_tmp8A);
return;_LL129:;}case 12: _LLEF: _tmp87=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp75)->f1;_tmp88=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp75)->f2;_LLF0:
# 548
 Cyc_Absyndump_dumpaggr_kind(_tmp87);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(_tmp88);Cyc_Absyndump_dump_char((int)'}');return;case 13: _LLF1: _tmp86=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp75)->f1;_LLF2:
{const char*_tmp35D;Cyc_Absyndump_dump(((_tmp35D="enum ",_tag_dyneither(_tmp35D,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(_tmp86);return;case 14: _LLF3: _tmp85=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp75)->f1;_LLF4:
{const char*_tmp35E;Cyc_Absyndump_dump(((_tmp35E="enum {",_tag_dyneither(_tmp35E,sizeof(char),7))));}Cyc_Absyndump_dumpenumfields(_tmp85);{const char*_tmp35F;Cyc_Absyndump_dump(((_tmp35F="}",_tag_dyneither(_tmp35F,sizeof(char),2))));}return;case 17: _LLF5: _tmp83=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp75)->f1;_tmp84=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp75)->f2;_LLF6:
(Cyc_Absyndump_dumpqvar(_tmp83),Cyc_Absyndump_dumptps(_tmp84));return;case 18: _LLF7: _tmp82=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp75)->f1;_LLF8:
{const char*_tmp360;Cyc_Absyndump_dump(((_tmp360="valueof_t(",_tag_dyneither(_tmp360,sizeof(char),11))));}Cyc_Absyndump_dumpexp(_tmp82);{const char*_tmp361;Cyc_Absyndump_dump(((_tmp361=")",_tag_dyneither(_tmp361,sizeof(char),2))));}return;case 27: _LLF9: _tmp81=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp75)->f1;_LLFA:
{const char*_tmp362;Cyc_Absyndump_dump(((_tmp362="typeof(",_tag_dyneither(_tmp362,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp81);{const char*_tmp363;Cyc_Absyndump_dump(((_tmp363=")",_tag_dyneither(_tmp363,sizeof(char),2))));}return;case 28: _LLFB: _tmp80=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp75)->f1;_LLFC:
 Cyc_Absyndump_dump(_tmp80);return;case 15: _LLFD: _tmp7F=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp75)->f1;_LLFE:
# 556
{const char*_tmp364;Cyc_Absyndump_dump(((_tmp364="region_t<",_tag_dyneither(_tmp364,sizeof(char),10))));}Cyc_Absyndump_dumprgn(_tmp7F);{const char*_tmp365;Cyc_Absyndump_dump(((_tmp365=">",_tag_dyneither(_tmp365,sizeof(char),2))));}return;case 16: _LLFF: _tmp7D=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp75)->f1;_tmp7E=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp75)->f2;_LL100:
{const char*_tmp366;Cyc_Absyndump_dump(((_tmp366="dynregion_t<",_tag_dyneither(_tmp366,sizeof(char),13))));}Cyc_Absyndump_dumprgn(_tmp7D);
{const char*_tmp367;Cyc_Absyndump_dump(((_tmp367=",",_tag_dyneither(_tmp367,sizeof(char),2))));}Cyc_Absyndump_dumprgn(_tmp7E);{const char*_tmp368;Cyc_Absyndump_dump(((_tmp368=">",_tag_dyneither(_tmp368,sizeof(char),2))));}return;case 19: _LL101: _tmp7C=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp75)->f1;_LL102:
{const char*_tmp369;Cyc_Absyndump_dump(((_tmp369="tag_t<",_tag_dyneither(_tmp369,sizeof(char),7))));}Cyc_Absyndump_dumpntyp(_tmp7C);{const char*_tmp36A;Cyc_Absyndump_dump(((_tmp36A=">",_tag_dyneither(_tmp36A,sizeof(char),2))));}return;case 21: _LL103: _LL104:
{const char*_tmp36B;Cyc_Absyndump_dump(((_tmp36B="`U",_tag_dyneither(_tmp36B,sizeof(char),3))));}goto _LLB8;case 22: _LL105: _LL106:
{const char*_tmp36C;Cyc_Absyndump_dump(((_tmp36C="`RC",_tag_dyneither(_tmp36C,sizeof(char),4))));}goto _LLB8;case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp75)->f1)->r)){case 0: _LL107: _tmp7B=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp75)->f1)->r)->f1;_LL108:
# 563
 Cyc_Absyndump_dump_aggrdecl(_tmp7B);return;case 1: _LL109: _tmp7A=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp75)->f1)->r)->f1;_LL10A:
# 565
 Cyc_Absyndump_dump_enumdecl(_tmp7A);return;default: _LL10B: _tmp79=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp75)->f1)->r)->f1;_LL10C:
# 567
 Cyc_Absyndump_dump_datatypedecl(_tmp79);return;}case 20: _LL10D: _LL10E:
{const char*_tmp36D;Cyc_Absyndump_dump(((_tmp36D="`H",_tag_dyneither(_tmp36D,sizeof(char),3))));}goto _LLB8;case 23: _LL10F: _tmp78=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp75)->f1;_LL110:
{const char*_tmp36E;Cyc_Absyndump_dump(((_tmp36E="{",_tag_dyneither(_tmp36E,sizeof(char),2))));}Cyc_Absyndump_dumptyp(_tmp78);{const char*_tmp36F;Cyc_Absyndump_dump(((_tmp36F="}",_tag_dyneither(_tmp36F,sizeof(char),2))));}goto _LLB8;case 25: _LL111: _tmp77=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp75)->f1;_LL112:
{const char*_tmp370;Cyc_Absyndump_dump(((_tmp370="regions(",_tag_dyneither(_tmp370,sizeof(char),9))));}Cyc_Absyndump_dumptyp(_tmp77);{const char*_tmp371;Cyc_Absyndump_dump(((_tmp371=")",_tag_dyneither(_tmp371,sizeof(char),2))));}goto _LLB8;default: _LL113: _tmp76=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp75)->f1;_LL114:
# 572
 for(0;_tmp76 != 0;_tmp76=_tmp76->tl){
Cyc_Absyndump_dumptyp((void*)_tmp76->hd);
if(_tmp76->tl != 0){const char*_tmp372;Cyc_Absyndump_dump(((_tmp372="+",_tag_dyneither(_tmp372,sizeof(char),2))));}}}_LLB8:;}
# 579
void Cyc_Absyndump_dumpvaropt(struct _dyneither_ptr*vo){
if(vo != 0)Cyc_Absyndump_dump_str(vo);}
# 582
void Cyc_Absyndump_dumpfunarg(struct _tuple8*t){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple16{void*f1;void*f2;};
# 585
void Cyc_Absyndump_dump_rgncmp(struct _tuple16*cmp){
struct _tuple16*_tmpDA=cmp;void*_tmpDB;void*_tmpDC;_LL12F: _tmpDB=_tmpDA->f1;_tmpDC=_tmpDA->f2;_LL130:
 Cyc_Absyndump_dumpeff(_tmpDB);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(_tmpDC);_LL12E:;}
# 589
void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){
const char*_tmp373;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,rgn_po,((_tmp373=",",_tag_dyneither(_tmp373,sizeof(char),2))));}
# 592
void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 596
Cyc_Absyndump_dump_char((int)'(');
for(0;args != 0;args=args->tl){
Cyc_Absyndump_dumpfunarg((struct _tuple8*)args->hd);
if((args->tl != 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((int)',');}
# 601
if(c_varargs){
const char*_tmp374;Cyc_Absyndump_dump(((_tmp374="...",_tag_dyneither(_tmp374,sizeof(char),4))));}else{
if(cyc_varargs != 0){
struct _tuple8*_tmp375;struct _tuple8*_tmpDF=(_tmp375=_cycalloc(sizeof(*_tmp375)),((_tmp375->f1=cyc_varargs->name,((_tmp375->f2=cyc_varargs->tq,((_tmp375->f3=cyc_varargs->type,_tmp375)))))));
{const char*_tmp376;Cyc_Absyndump_dump(((_tmp376="...",_tag_dyneither(_tmp376,sizeof(char),4))));}
if(cyc_varargs->inject){const char*_tmp377;Cyc_Absyndump_dump(((_tmp377=" inject ",_tag_dyneither(_tmp377,sizeof(char),9))));}
Cyc_Absyndump_dumpfunarg(_tmpDF);}}
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
{const char*_tmp378;Cyc_Absyndump_dump(((_tmp378=" @requires(",_tag_dyneither(_tmp378,sizeof(char),12))));}
Cyc_Absyndump_dumpexp(req);
Cyc_Absyndump_dump_char((int)')');}
# 623
if(ens != 0){
{const char*_tmp379;Cyc_Absyndump_dump(((_tmp379=" @ensures(",_tag_dyneither(_tmp379,sizeof(char),11))));}
Cyc_Absyndump_dumpexp(ens);
Cyc_Absyndump_dump_char((int)')');}}
# 630
void Cyc_Absyndump_dumptyp(void*t){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 634
void Cyc_Absyndump_dumpdesignator(void*d){
void*_tmpE5=d;struct _dyneither_ptr*_tmpE6;struct Cyc_Absyn_Exp*_tmpE7;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE5)->tag == 0){_LL134: _tmpE7=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE5)->f1;_LL135:
{const char*_tmp37A;Cyc_Absyndump_dump(((_tmp37A=".[",_tag_dyneither(_tmp37A,sizeof(char),3))));}Cyc_Absyndump_dumpexp(_tmpE7);Cyc_Absyndump_dump_char((int)']');goto _LL133;}else{_LL136: _tmpE6=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpE5)->f1;_LL137:
 Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmpE6);goto _LL133;}_LL133:;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 641
void Cyc_Absyndump_dumpde(struct _tuple17*de){
{const char*_tmp37D;const char*_tmp37C;const char*_tmp37B;((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*de).f1,((_tmp37B="",_tag_dyneither(_tmp37B,sizeof(char),1))),((_tmp37C="=",_tag_dyneither(_tmp37C,sizeof(char),2))),((_tmp37D="=",_tag_dyneither(_tmp37D,sizeof(char),2))));}
Cyc_Absyndump_dumpexp((*de).f2);}
# 646
void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){
const char*_tmp380;const char*_tmp37F;const char*_tmp37E;((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,inprec,es,((_tmp37E="",_tag_dyneither(_tmp37E,sizeof(char),1))),((_tmp37F="",_tag_dyneither(_tmp37F,sizeof(char),1))),((_tmp380=",",_tag_dyneither(_tmp380,sizeof(char),2))));}
# 650
void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
if(inprec >= myprec){
const char*_tmp381;Cyc_Absyndump_dump_nospace(((_tmp381="(",_tag_dyneither(_tmp381,sizeof(char),2))));}
{void*_tmpF0=e->r;void*_tmpF1=_tmpF0;struct Cyc_Absyn_Stmt*_tmpF2;struct Cyc_Core_Opt*_tmpF3;struct Cyc_List_List*_tmpF4;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*_tmpF6;int _tmpF7;struct Cyc_Absyn_Exp*_tmpF8;void**_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;int _tmpFB;struct _tuple0*_tmpFC;struct _tuple0*_tmpFD;struct Cyc_List_List*_tmpFE;struct Cyc_Absyn_Datatypefield*_tmpFF;struct Cyc_List_List*_tmp100;struct _tuple0*_tmp101;struct Cyc_List_List*_tmp102;struct Cyc_List_List*_tmp103;struct Cyc_Absyn_Exp*_tmp104;void*_tmp105;struct Cyc_Absyn_Vardecl*_tmp106;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_List_List*_tmp109;struct _tuple8*_tmp10A;struct Cyc_List_List*_tmp10B;struct Cyc_List_List*_tmp10C;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10F;struct _dyneither_ptr*_tmp110;struct Cyc_Absyn_Exp*_tmp111;struct _dyneither_ptr*_tmp112;struct Cyc_Absyn_Exp*_tmp113;void*_tmp114;struct Cyc_List_List*_tmp115;struct Cyc_Absyn_Exp*_tmp116;struct _dyneither_ptr*_tmp117;int _tmp118;struct _dyneither_ptr _tmp119;void*_tmp11A;struct Cyc_Absyn_Exp*_tmp11B;void*_tmp11C;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11F;void*_tmp120;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_List_List*_tmp126;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp12B;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_Core_Opt*_tmp135;struct Cyc_Absyn_Exp*_tmp136;enum Cyc_Absyn_Primop _tmp137;struct Cyc_List_List*_tmp138;struct _tuple0*_tmp139;struct _dyneither_ptr _tmp13A;struct _dyneither_ptr _tmp13B;struct _dyneither_ptr _tmp13C;enum Cyc_Absyn_Sign _tmp13D;long long _tmp13E;int _tmp13F;int _tmp140;int _tmp141;enum Cyc_Absyn_Sign _tmp142;short _tmp143;struct _dyneither_ptr _tmp144;enum Cyc_Absyn_Sign _tmp145;char _tmp146;switch(*((int*)_tmpF1)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).String_c).tag){case 2: _LL139: _tmp145=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Char_c).val).f1;_tmp146=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Char_c).val).f2;_LL13A:
# 656
 Cyc_Absyndump_dump_char((int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp146));Cyc_Absyndump_dump_char((int)'\'');
goto _LL138;case 3: _LL13B: _tmp144=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Wchar_c).val;_LL13C:
{const char*_tmp385;void*_tmp384[1];struct Cyc_String_pa_PrintArg_struct _tmp383;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp383.tag=0,((_tmp383.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp144),((_tmp384[0]=& _tmp383,Cyc_aprintf(((_tmp385="L'%s'",_tag_dyneither(_tmp385,sizeof(char),6))),_tag_dyneither(_tmp384,sizeof(void*),1)))))))));}goto _LL138;case 4: _LL13D: _tmp142=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Short_c).val).f1;_tmp143=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Short_c).val).f2;_LL13E:
{const char*_tmp389;void*_tmp388[1];struct Cyc_Int_pa_PrintArg_struct _tmp387;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp387.tag=1,((_tmp387.f1=(unsigned long)((int)_tmp143),((_tmp388[0]=& _tmp387,Cyc_aprintf(((_tmp389="%d",_tag_dyneither(_tmp389,sizeof(char),3))),_tag_dyneither(_tmp388,sizeof(void*),1)))))))));}goto _LL138;case 5: switch((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Int_c).val).f1){case Cyc_Absyn_None: _LL13F: _tmp141=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Int_c).val).f2;_LL140:
 _tmp140=_tmp141;goto _LL142;case Cyc_Absyn_Signed: _LL141: _tmp140=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Int_c).val).f2;_LL142:
{const char*_tmp38D;void*_tmp38C[1];struct Cyc_Int_pa_PrintArg_struct _tmp38B;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp38B.tag=1,((_tmp38B.f1=(unsigned long)_tmp140,((_tmp38C[0]=& _tmp38B,Cyc_aprintf(((_tmp38D="%d",_tag_dyneither(_tmp38D,sizeof(char),3))),_tag_dyneither(_tmp38C,sizeof(void*),1)))))))));}goto _LL138;default: _LL143: _tmp13F=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Int_c).val).f2;_LL144:
{const char*_tmp391;void*_tmp390[1];struct Cyc_Int_pa_PrintArg_struct _tmp38F;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp38F.tag=1,((_tmp38F.f1=(unsigned int)_tmp13F,((_tmp390[0]=& _tmp38F,Cyc_aprintf(((_tmp391="%u",_tag_dyneither(_tmp391,sizeof(char),3))),_tag_dyneither(_tmp390,sizeof(void*),1)))))))));}goto _LL138;}case 6: _LL145: _tmp13D=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).LongLong_c).val).f1;_tmp13E=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).LongLong_c).val).f2;_LL146:
# 665
 Cyc_Absyndump_dump(Cyc_Absynpp_longlong2string((unsigned long long)_tmp13E));goto _LL138;case 7: _LL147: _tmp13C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Float_c).val).f1;_LL148:
 Cyc_Absyndump_dump(_tmp13C);goto _LL138;case 1: _LL149: _LL14A:
{const char*_tmp392;Cyc_Absyndump_dump(((_tmp392="NULL",_tag_dyneither(_tmp392,sizeof(char),5))));}goto _LL138;case 8: _LL14B: _tmp13B=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).String_c).val;_LL14C:
# 669
 Cyc_Absyndump_dump_char((int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp13B));Cyc_Absyndump_dump_char((int)'"');
goto _LL138;default: _LL14D: _tmp13A=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Wstring_c).val;_LL14E:
# 672
{const char*_tmp393;Cyc_Absyndump_dump(((_tmp393="L\"",_tag_dyneither(_tmp393,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(_tmp13A);Cyc_Absyndump_dump_char((int)'"');
goto _LL138;}case 1: _LL14F: _tmp139=((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL150:
# 675
 Cyc_Absyndump_dumpqvar(_tmp139);goto _LL138;case 2: _LL151: _tmp137=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp138=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL152: {
# 678
struct _dyneither_ptr _tmp155=Cyc_Absynpp_prim2str(_tmp137);
{int _tmp156=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp138);int _tmp157=_tmp156;switch(_tmp157){case 1: _LL1A4: _LL1A5:
# 681
 if(_tmp137 == Cyc_Absyn_Numelts){
{const char*_tmp394;Cyc_Absyndump_dump(((_tmp394="numelts(",_tag_dyneither(_tmp394,sizeof(char),9))));}
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp138))->hd);{
const char*_tmp395;Cyc_Absyndump_dump(((_tmp395=")",_tag_dyneither(_tmp395,sizeof(char),2))));};}else{
# 686
Cyc_Absyndump_dump(_tmp155);
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp138))->hd);}
# 689
goto _LL1A3;case 2: _LL1A6: _LL1A7:
# 691
 Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp138))->hd);
Cyc_Absyndump_dump(_tmp155);
Cyc_Absyndump_dump_char((int)' ');
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp138->tl))->hd);
goto _LL1A3;default: _LL1A8: _LL1A9: {
# 697
struct Cyc_Core_Failure_exn_struct _tmp39B;const char*_tmp39A;struct Cyc_Core_Failure_exn_struct*_tmp399;(int)_throw((void*)((_tmp399=_cycalloc(sizeof(*_tmp399)),((_tmp399[0]=((_tmp39B.tag=Cyc_Core_Failure,((_tmp39B.f1=((_tmp39A="Absyndump -- Bad number of arguments to primop",_tag_dyneither(_tmp39A,sizeof(char),47))),_tmp39B)))),_tmp399)))));}}_LL1A3:;}
# 699
goto _LL138;}case 3: _LL153: _tmp134=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp135=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_tmp136=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpF1)->f3;_LL154:
# 702
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp134);
if(_tmp135 != 0)
Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)_tmp135->v));
{const char*_tmp39C;Cyc_Absyndump_dump_nospace(((_tmp39C="=",_tag_dyneither(_tmp39C,sizeof(char),2))));}
Cyc_Absyndump_dumpexp_prec(myprec,_tmp136);
goto _LL138;case 4: switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF1)->f2){case Cyc_Absyn_PreInc: _LL155: _tmp133=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL156:
# 709
{const char*_tmp39D;Cyc_Absyndump_dump(((_tmp39D="++",_tag_dyneither(_tmp39D,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp133);goto _LL138;case Cyc_Absyn_PreDec: _LL157: _tmp132=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL158:
{const char*_tmp39E;Cyc_Absyndump_dump(((_tmp39E="--",_tag_dyneither(_tmp39E,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp132);goto _LL138;case Cyc_Absyn_PostInc: _LL159: _tmp131=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL15A:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp131);{const char*_tmp39F;Cyc_Absyndump_dump(((_tmp39F="++",_tag_dyneither(_tmp39F,sizeof(char),3))));}goto _LL138;default: _LL15B: _tmp130=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL15C:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp130);{const char*_tmp3A0;Cyc_Absyndump_dump(((_tmp3A0="--",_tag_dyneither(_tmp3A0,sizeof(char),3))));}goto _LL138;}case 5: _LL15D: _tmp12D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp12E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_tmp12F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF1)->f3;_LL15E:
# 715
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp12D);
Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp12E);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,_tmp12F);
goto _LL138;case 6: _LL15F: _tmp12B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp12C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL160:
# 722
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp12B);{const char*_tmp3A1;Cyc_Absyndump_dump(((_tmp3A1=" && ",_tag_dyneither(_tmp3A1,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp12C);goto _LL138;case 7: _LL161: _tmp129=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp12A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL162:
# 726
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp129);{const char*_tmp3A2;Cyc_Absyndump_dump(((_tmp3A2=" || ",_tag_dyneither(_tmp3A2,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp12A);goto _LL138;case 8: _LL163: _tmp127=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp128=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL164:
# 732
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp127);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(myprec,_tmp128);
Cyc_Absyndump_dump_char((int)')');
goto _LL138;case 9: _LL165: _tmp125=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp126=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL166:
# 739
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp125);
{const char*_tmp3A3;Cyc_Absyndump_dump_nospace(((_tmp3A3="(",_tag_dyneither(_tmp3A3,sizeof(char),2))));}{
# 745
int old_generate_line_directives=Cyc_Absyndump_generate_line_directives;
Cyc_Absyndump_generate_line_directives=
(old_generate_line_directives  && !(e->loc == 0)) && !(_tmp125->loc == 0);
Cyc_Absyndump_dumpexps_prec(20,_tmp126);
{const char*_tmp3A4;Cyc_Absyndump_dump_nospace(((_tmp3A4=")",_tag_dyneither(_tmp3A4,sizeof(char),2))));}
Cyc_Absyndump_generate_line_directives=old_generate_line_directives;
goto _LL138;};case 10: _LL167: _tmp124=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL168:
# 754
 Cyc_Absyndump_dumploc(e->loc);
{const char*_tmp3A5;Cyc_Absyndump_dump(((_tmp3A5="throw",_tag_dyneither(_tmp3A5,sizeof(char),6))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp124);goto _LL138;case 11: _LL169: _tmp123=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL16A:
# 757
 _tmp122=_tmp123;goto _LL16C;case 12: _LL16B: _tmp122=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL16C:
 Cyc_Absyndump_dumpexp_prec(inprec,_tmp122);goto _LL138;case 13: _LL16D: _tmp120=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp121=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL16E:
# 761
 Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(_tmp120);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,_tmp121);
goto _LL138;case 14: _LL16F: _tmp11F=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL170:
# 765
 Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp11F);goto _LL138;case 15: _LL171: _tmp11D=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp11E=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL172:
# 768
{const char*_tmp3A6;Cyc_Absyndump_dump(((_tmp3A6="new ",_tag_dyneither(_tmp3A6,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp11E);goto _LL138;case 16: _LL173: _tmp11C=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL174:
# 771
{const char*_tmp3A7;Cyc_Absyndump_dump(((_tmp3A7="sizeof(",_tag_dyneither(_tmp3A7,sizeof(char),8))));}Cyc_Absyndump_dumptyp(_tmp11C);Cyc_Absyndump_dump_char((int)')');goto _LL138;case 17: _LL175: _tmp11B=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL176:
# 774
{const char*_tmp3A8;Cyc_Absyndump_dump(((_tmp3A8="sizeof(",_tag_dyneither(_tmp3A8,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp11B);Cyc_Absyndump_dump_char((int)')');goto _LL138;case 38: _LL177: _tmp11A=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL178:
# 777
{const char*_tmp3A9;Cyc_Absyndump_dump(((_tmp3A9="valueof(",_tag_dyneither(_tmp3A9,sizeof(char),9))));}Cyc_Absyndump_dumptyp(_tmp11A);Cyc_Absyndump_dump_char((int)')');goto _LL138;case 39: _LL179: _tmp118=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp119=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL17A:
# 780
{const char*_tmp3AA;Cyc_Absyndump_dump(((_tmp3AA="__asm__",_tag_dyneither(_tmp3AA,sizeof(char),8))));}
if(_tmp118){const char*_tmp3AB;Cyc_Absyndump_dump(((_tmp3AB=" volatile ",_tag_dyneither(_tmp3AB,sizeof(char),11))));}
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_nospace(_tmp119);
Cyc_Absyndump_dump_char((int)')');
goto _LL138;case 37: _LL17B: _tmp116=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp117=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL17C:
# 788
{const char*_tmp3AC;Cyc_Absyndump_dump(((_tmp3AC="tagcheck(",_tag_dyneither(_tmp3AC,sizeof(char),10))));}Cyc_Absyndump_dumpexp(_tmp116);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp117);
Cyc_Absyndump_dump_char((int)')');
goto _LL138;case 18: _LL17D: _tmp114=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp115=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL17E:
# 793
{const char*_tmp3AD;Cyc_Absyndump_dump(((_tmp3AD="offsetof(",_tag_dyneither(_tmp3AD,sizeof(char),10))));}Cyc_Absyndump_dumptyp(_tmp114);Cyc_Absyndump_dump_char((int)',');
for(0;_tmp115 != 0;_tmp115=_tmp115->tl){
{void*_tmp16F=(void*)_tmp115->hd;void*_tmp170=_tmp16F;unsigned int _tmp171;struct _dyneither_ptr*_tmp172;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp170)->tag == 0){_LL1AB: _tmp172=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp170)->f1;_LL1AC:
# 797
 Cyc_Absyndump_dump_nospace(*_tmp172);
goto _LL1AA;}else{_LL1AD: _tmp171=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp170)->f1;_LL1AE:
# 800
{const char*_tmp3B1;void*_tmp3B0[1];struct Cyc_Int_pa_PrintArg_struct _tmp3AF;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp3AF.tag=1,((_tmp3AF.f1=(unsigned long)((int)_tmp171),((_tmp3B0[0]=& _tmp3AF,Cyc_aprintf(((_tmp3B1="%d",_tag_dyneither(_tmp3B1,sizeof(char),3))),_tag_dyneither(_tmp3B0,sizeof(void*),1)))))))));}
goto _LL1AA;}_LL1AA:;}
# 803
if(_tmp115->tl != 0)Cyc_Absyndump_dump_char((int)'.');}
# 805
Cyc_Absyndump_dump_char((int)')');
goto _LL138;case 19: _LL17F: _tmp113=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL180:
# 809
 Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp113);goto _LL138;case 20: _LL181: _tmp111=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp112=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL182:
# 812
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp111);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp112);
goto _LL138;case 21: _LL183: _tmp10F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp110=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL184:
# 816
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp10F);{const char*_tmp3B2;Cyc_Absyndump_dump_nospace(((_tmp3B2="->",_tag_dyneither(_tmp3B2,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(*_tmp110);
goto _LL138;case 22: _LL185: _tmp10D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp10E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL186:
# 820
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp10D);
Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp10E);Cyc_Absyndump_dump_char((int)']');goto _LL138;case 23: _LL187: _tmp10C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL188:
# 824
{const char*_tmp3B3;Cyc_Absyndump_dump(((_tmp3B3="$(",_tag_dyneither(_tmp3B3,sizeof(char),3))));}Cyc_Absyndump_dumpexps_prec(20,_tmp10C);Cyc_Absyndump_dump_char((int)')');goto _LL138;case 24: _LL189: _tmp10A=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp10B=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL18A:
# 827
 Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp((*_tmp10A).f3);
Cyc_Absyndump_dump_char((int)')');
{const char*_tmp3B6;const char*_tmp3B5;const char*_tmp3B4;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp10B,((_tmp3B4="{",_tag_dyneither(_tmp3B4,sizeof(char),2))),((_tmp3B5="}",_tag_dyneither(_tmp3B5,sizeof(char),2))),((_tmp3B6=",",_tag_dyneither(_tmp3B6,sizeof(char),2))));}
goto _LL138;case 25: _LL18B: _tmp109=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL18C:
# 834
{const char*_tmp3B9;const char*_tmp3B8;const char*_tmp3B7;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp109,((_tmp3B7="{",_tag_dyneither(_tmp3B7,sizeof(char),2))),((_tmp3B8="}",_tag_dyneither(_tmp3B8,sizeof(char),2))),((_tmp3B9=",",_tag_dyneither(_tmp3B9,sizeof(char),2))));}
goto _LL138;case 26: _LL18D: _tmp106=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp107=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_tmp108=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpF1)->f3;_LL18E:
# 838
{const char*_tmp3BA;Cyc_Absyndump_dump(((_tmp3BA="{for",_tag_dyneither(_tmp3BA,sizeof(char),5))));}Cyc_Absyndump_dump_str((*_tmp106->name).f2);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp107);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(_tmp108);Cyc_Absyndump_dump_char((int)'}');
goto _LL138;case 27: _LL18F: _tmp104=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp105=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL190:
# 843
{const char*_tmp3BB;Cyc_Absyndump_dump(((_tmp3BB="{for x ",_tag_dyneither(_tmp3BB,sizeof(char),8))));}Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp104);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumptyp(_tmp105);Cyc_Absyndump_dump_char((int)'}');
goto _LL138;case 28: _LL191: _tmp101=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp102=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_tmp103=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpF1)->f3;_LL192:
# 848
 Cyc_Absyndump_dumpqvar(_tmp101);
Cyc_Absyndump_dump_char((int)'{');
if(_tmp102 != 0)
Cyc_Absyndump_dumptps(_tmp102);
{const char*_tmp3BE;const char*_tmp3BD;const char*_tmp3BC;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp103,((_tmp3BC="",_tag_dyneither(_tmp3BC,sizeof(char),1))),((_tmp3BD="}",_tag_dyneither(_tmp3BD,sizeof(char),2))),((_tmp3BE=",",_tag_dyneither(_tmp3BE,sizeof(char),2))));}
goto _LL138;case 29: _LL193: _tmp100=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL194:
# 856
{const char*_tmp3C1;const char*_tmp3C0;const char*_tmp3BF;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp100,((_tmp3BF="{",_tag_dyneither(_tmp3BF,sizeof(char),2))),((_tmp3C0="}",_tag_dyneither(_tmp3C0,sizeof(char),2))),((_tmp3C1=",",_tag_dyneither(_tmp3C1,sizeof(char),2))));}
goto _LL138;case 30: _LL195: _tmpFE=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmpFF=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpF1)->f3;_LL196:
# 860
 Cyc_Absyndump_dumpqvar(_tmpFF->name);
if(_tmpFE != 0){const char*_tmp3C4;const char*_tmp3C3;const char*_tmp3C2;((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,_tmpFE,((_tmp3C2="(",_tag_dyneither(_tmp3C2,sizeof(char),2))),((_tmp3C3=")",_tag_dyneither(_tmp3C3,sizeof(char),2))),((_tmp3C4=",",_tag_dyneither(_tmp3C4,sizeof(char),2))));}
goto _LL138;case 31: _LL197: _tmpFD=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL198:
# 864
 Cyc_Absyndump_dumpqvar(_tmpFD);goto _LL138;case 32: _LL199: _tmpFC=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL19A:
 Cyc_Absyndump_dumpqvar(_tmpFC);goto _LL138;case 33: _LL19B: _tmpF7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).is_calloc;_tmpF8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).rgn;_tmpF9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).elt_type;_tmpFA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).num_elts;_tmpFB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).inline_call;_LL19C:
# 868
 Cyc_Absyndump_dumploc(_tmpFA->loc);
if(_tmpF7){
if(_tmpF8 != 0){
{const char*_tmp3C5;Cyc_Absyndump_dump(((_tmp3C5="rcalloc(",_tag_dyneither(_tmp3C5,sizeof(char),9))));}
Cyc_Absyndump_dumpexp(_tmpF8);{const char*_tmp3C6;Cyc_Absyndump_dump(((_tmp3C6=",",_tag_dyneither(_tmp3C6,sizeof(char),2))));};}else{
# 874
const char*_tmp3C7;Cyc_Absyndump_dump(((_tmp3C7="calloc",_tag_dyneither(_tmp3C7,sizeof(char),7))));}
# 876
Cyc_Absyndump_dumpexp(_tmpFA);
{const char*_tmp3C8;Cyc_Absyndump_dump(((_tmp3C8=",",_tag_dyneither(_tmp3C8,sizeof(char),2))));}
Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmpF9)),0));{
const char*_tmp3C9;Cyc_Absyndump_dump(((_tmp3C9=")",_tag_dyneither(_tmp3C9,sizeof(char),2))));};}else{
# 881
if(_tmpF8 != 0){
if(_tmpFB){
const char*_tmp3CA;Cyc_Absyndump_dump(((_tmp3CA="rmalloc_inline(",_tag_dyneither(_tmp3CA,sizeof(char),16))));}else{
# 885
const char*_tmp3CB;Cyc_Absyndump_dump(((_tmp3CB="rmalloc(",_tag_dyneither(_tmp3CB,sizeof(char),9))));}
Cyc_Absyndump_dumpexp(_tmpF8);{const char*_tmp3CC;Cyc_Absyndump_dump(((_tmp3CC=",",_tag_dyneither(_tmp3CC,sizeof(char),2))));};}else{
# 888
const char*_tmp3CD;Cyc_Absyndump_dump(((_tmp3CD="malloc(",_tag_dyneither(_tmp3CD,sizeof(char),8))));}
# 891
if(_tmpF9 != 0)
Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmpF9,0),_tmpFA,0));else{
# 894
Cyc_Absyndump_dumpexp(_tmpFA);}{
const char*_tmp3CE;Cyc_Absyndump_dump(((_tmp3CE=")",_tag_dyneither(_tmp3CE,sizeof(char),2))));};}
# 897
goto _LL138;case 34: _LL19D: _tmpF5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmpF6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL19E:
# 900
 Cyc_Absyndump_dumpexp_prec(myprec,_tmpF5);
{const char*_tmp3CF;Cyc_Absyndump_dump_nospace(((_tmp3CF=":=:",_tag_dyneither(_tmp3CF,sizeof(char),4))));}
Cyc_Absyndump_dumpexp_prec(myprec,_tmpF6);
goto _LL138;case 35: _LL19F: _tmpF3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmpF4=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL1A0:
# 907
{const char*_tmp3D2;const char*_tmp3D1;const char*_tmp3D0;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmpF4,((_tmp3D0="{",_tag_dyneither(_tmp3D0,sizeof(char),2))),((_tmp3D1="}",_tag_dyneither(_tmp3D1,sizeof(char),2))),((_tmp3D2=",",_tag_dyneither(_tmp3D2,sizeof(char),2))));}
goto _LL138;default: _LL1A1: _tmpF2=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL1A2:
# 911
{const char*_tmp3D3;Cyc_Absyndump_dump_nospace(((_tmp3D3="({",_tag_dyneither(_tmp3D3,sizeof(char),3))));}
Cyc_Absyndump_dumpstmt(_tmpF2,1);
{const char*_tmp3D4;Cyc_Absyndump_dump_nospace(((_tmp3D4="})",_tag_dyneither(_tmp3D4,sizeof(char),3))));}
goto _LL138;}_LL138:;}
# 916
if(inprec >= myprec)
Cyc_Absyndump_dump_char((int)')');}
# 920
void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){
Cyc_Absyndump_dumpexp_prec(0,e);}
# 924
void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs){
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp199=(struct Cyc_Absyn_Switch_clause*)scs->hd;
if(_tmp199->where_clause == 0  && (_tmp199->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){
const char*_tmp3D5;Cyc_Absyndump_dump(((_tmp3D5="default:",_tag_dyneither(_tmp3D5,sizeof(char),9))));}else{
# 930
{const char*_tmp3D6;Cyc_Absyndump_dump(((_tmp3D6="case",_tag_dyneither(_tmp3D6,sizeof(char),5))));}
Cyc_Absyndump_dumppat(_tmp199->pattern);
if(_tmp199->where_clause != 0){
{const char*_tmp3D7;Cyc_Absyndump_dump(((_tmp3D7="&&",_tag_dyneither(_tmp3D7,sizeof(char),3))));}
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp199->where_clause));}{
# 936
const char*_tmp3D8;Cyc_Absyndump_dump_nospace(((_tmp3D8=":",_tag_dyneither(_tmp3D8,sizeof(char),2))));};}
# 938
Cyc_Absyndump_dumpstmt(_tmp199->body,0);}}
# 942
void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt){
Cyc_Absyndump_dumploc(s->loc);{
void*_tmp19E=s->r;void*_tmp19F=_tmp19E;struct Cyc_Absyn_Exp*_tmp1A0;struct Cyc_Absyn_Stmt*_tmp1A1;struct Cyc_List_List*_tmp1A2;struct Cyc_List_List*_tmp1A3;struct Cyc_Absyn_Stmt*_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A5;struct _dyneither_ptr*_tmp1A6;struct Cyc_Absyn_Stmt*_tmp1A7;struct Cyc_Absyn_Decl*_tmp1A8;struct Cyc_Absyn_Stmt*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_List_List*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AE;struct Cyc_Absyn_Stmt*_tmp1AF;struct _dyneither_ptr*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1B1;struct Cyc_Absyn_Stmt*_tmp1B2;struct Cyc_Absyn_Exp*_tmp1B3;struct Cyc_Absyn_Stmt*_tmp1B4;struct Cyc_Absyn_Stmt*_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B6;struct Cyc_Absyn_Stmt*_tmp1B7;struct Cyc_Absyn_Stmt*_tmp1B8;struct Cyc_Absyn_Exp*_tmp1B9;switch(*((int*)_tmp19F)){case 0: _LL1B0: _LL1B1:
 Cyc_Absyndump_dump_semi();goto _LL1AF;case 1: _LL1B2: _tmp1B9=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_LL1B3:
 Cyc_Absyndump_dumpexp(_tmp1B9);Cyc_Absyndump_dump_semi();goto _LL1AF;case 2: _LL1B4: _tmp1B7=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1B8=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL1B5:
# 951
 if(Cyc_Absynpp_is_declaration(_tmp1B7)){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp1B7,0);Cyc_Absyndump_dump_char((int)'}');}else{
# 954
Cyc_Absyndump_dumpstmt(_tmp1B7,0);}
if(Cyc_Absynpp_is_declaration(_tmp1B8)){
if(expstmt)Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp1B8,expstmt);
Cyc_Absyndump_dump_char((int)'}');
if(expstmt)Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dump_semi();}else{
# 961
Cyc_Absyndump_dumpstmt(_tmp1B8,expstmt);}
goto _LL1AF;case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1 == 0){_LL1B6: _LL1B7:
{const char*_tmp3D9;Cyc_Absyndump_dump(((_tmp3D9="return;",_tag_dyneither(_tmp3D9,sizeof(char),8))));}goto _LL1AF;}else{_LL1B8: _tmp1B6=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_LL1B9:
{const char*_tmp3DA;Cyc_Absyndump_dump(((_tmp3DA="return",_tag_dyneither(_tmp3DA,sizeof(char),7))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp1B6));Cyc_Absyndump_dump_semi();goto _LL1AF;}case 4: _LL1BA: _tmp1B3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1B4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_tmp1B5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19F)->f3;_LL1BB:
# 966
{const char*_tmp3DB;Cyc_Absyndump_dump(((_tmp3DB="if(",_tag_dyneither(_tmp3DB,sizeof(char),4))));}Cyc_Absyndump_dumpexp(_tmp1B3);
{void*_tmp1BD=_tmp1B4->r;void*_tmp1BE=_tmp1BD;switch(*((int*)_tmp1BE)){case 2: _LL1D7: _LL1D8:
 goto _LL1DA;case 12: _LL1D9: _LL1DA:
 goto _LL1DC;case 4: _LL1DB: _LL1DC:
 goto _LL1DE;case 13: _LL1DD: _LL1DE:
{const char*_tmp3DC;Cyc_Absyndump_dump_nospace(((_tmp3DC="){",_tag_dyneither(_tmp3DC,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(_tmp1B4,0);Cyc_Absyndump_dump_char((int)'}');goto _LL1D6;default: _LL1DF: _LL1E0:
 Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(_tmp1B4,0);}_LL1D6:;}
# 974
{void*_tmp1C0=_tmp1B5->r;void*_tmp1C1=_tmp1C0;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp1C1)->tag == 0){_LL1E2: _LL1E3:
 goto _LL1E1;}else{_LL1E4: _LL1E5:
{const char*_tmp3DD;Cyc_Absyndump_dump(((_tmp3DD="else{",_tag_dyneither(_tmp3DD,sizeof(char),6))));}Cyc_Absyndump_dumpstmt(_tmp1B5,0);Cyc_Absyndump_dump_char((int)'}');goto _LL1E1;}_LL1E1:;}
# 978
goto _LL1AF;case 5: _LL1BC: _tmp1B1=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1).f1;_tmp1B2=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL1BD:
# 980
{const char*_tmp3DE;Cyc_Absyndump_dump(((_tmp3DE="while(",_tag_dyneither(_tmp3DE,sizeof(char),7))));}Cyc_Absyndump_dumpexp(_tmp1B1);{const char*_tmp3DF;Cyc_Absyndump_dump_nospace(((_tmp3DF="){",_tag_dyneither(_tmp3DF,sizeof(char),3))));}
Cyc_Absyndump_dumpstmt(_tmp1B2,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL1AF;case 6: _LL1BE: _LL1BF:
{const char*_tmp3E0;Cyc_Absyndump_dump(((_tmp3E0="break;",_tag_dyneither(_tmp3E0,sizeof(char),7))));}goto _LL1AF;case 7: _LL1C0: _LL1C1:
{const char*_tmp3E1;Cyc_Absyndump_dump(((_tmp3E1="continue;",_tag_dyneither(_tmp3E1,sizeof(char),10))));}goto _LL1AF;case 8: _LL1C2: _tmp1B0=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_LL1C3:
{const char*_tmp3E2;Cyc_Absyndump_dump(((_tmp3E2="goto",_tag_dyneither(_tmp3E2,sizeof(char),5))));}Cyc_Absyndump_dump_str(_tmp1B0);Cyc_Absyndump_dump_semi();goto _LL1AF;case 9: _LL1C4: _tmp1AC=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1AD=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2).f1;_tmp1AE=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19F)->f3).f1;_tmp1AF=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19F)->f4;_LL1C5:
# 989
{const char*_tmp3E3;Cyc_Absyndump_dump(((_tmp3E3="for(",_tag_dyneither(_tmp3E3,sizeof(char),5))));}Cyc_Absyndump_dumpexp(_tmp1AC);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp1AD);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp1AE);
{const char*_tmp3E4;Cyc_Absyndump_dump_nospace(((_tmp3E4="){",_tag_dyneither(_tmp3E4,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(_tmp1AF,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL1AF;case 10: _LL1C6: _tmp1AA=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1AB=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL1C7:
# 993
{const char*_tmp3E5;Cyc_Absyndump_dump(((_tmp3E5="switch(",_tag_dyneither(_tmp3E5,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp1AA);{const char*_tmp3E6;Cyc_Absyndump_dump_nospace(((_tmp3E6="){",_tag_dyneither(_tmp3E6,sizeof(char),3))));}
Cyc_Absyndump_dumpswitchclauses(_tmp1AB);
Cyc_Absyndump_dump_char((int)'}');
goto _LL1AF;case 12: _LL1C8: _tmp1A8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1A9=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL1C9:
# 998
 Cyc_Absyndump_dumpdecl(_tmp1A8);Cyc_Absyndump_dumpstmt(_tmp1A9,expstmt);goto _LL1AF;case 13: _LL1CA: _tmp1A6=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1A7=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL1CB:
# 1004
 if(Cyc_Absynpp_is_declaration(_tmp1A7)){
Cyc_Absyndump_dump_str(_tmp1A6);
if(expstmt){const char*_tmp3E7;Cyc_Absyndump_dump_nospace(((_tmp3E7=": ({",_tag_dyneither(_tmp3E7,sizeof(char),5))));}else{const char*_tmp3E8;Cyc_Absyndump_dump_nospace(((_tmp3E8=": {",_tag_dyneither(_tmp3E8,sizeof(char),4))));}
Cyc_Absyndump_dumpstmt(_tmp1A7,expstmt);
if(expstmt){const char*_tmp3E9;Cyc_Absyndump_dump_nospace(((_tmp3E9="});}",_tag_dyneither(_tmp3E9,sizeof(char),5))));}else{Cyc_Absyndump_dump_char((int)'}');}}else{
# 1010
Cyc_Absyndump_dump_str(_tmp1A6);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(_tmp1A7,expstmt);}
# 1012
goto _LL1AF;case 14: _LL1CC: _tmp1A4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1A5=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2).f1;_LL1CD:
# 1015
{const char*_tmp3EA;Cyc_Absyndump_dump(((_tmp3EA="do{",_tag_dyneither(_tmp3EA,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp1A4,0);
{const char*_tmp3EB;Cyc_Absyndump_dump_nospace(((_tmp3EB="}while(",_tag_dyneither(_tmp3EB,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp1A5);{const char*_tmp3EC;Cyc_Absyndump_dump_nospace(((_tmp3EC=");",_tag_dyneither(_tmp3EC,sizeof(char),3))));}
goto _LL1AF;case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1 == 0){_LL1CE: _LL1CF:
# 1019
{const char*_tmp3ED;Cyc_Absyndump_dump(((_tmp3ED="fallthru;",_tag_dyneither(_tmp3ED,sizeof(char),10))));}goto _LL1AF;}else{_LL1D0: _tmp1A3=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_LL1D1:
# 1021
{const char*_tmp3EE;Cyc_Absyndump_dump(((_tmp3EE="fallthru(",_tag_dyneither(_tmp3EE,sizeof(char),10))));}Cyc_Absyndump_dumpexps_prec(20,_tmp1A3);{const char*_tmp3EF;Cyc_Absyndump_dump_nospace(((_tmp3EF=");",_tag_dyneither(_tmp3EF,sizeof(char),3))));}goto _LL1AF;}case 15: _LL1D2: _tmp1A1=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1A2=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL1D3:
# 1024
{const char*_tmp3F0;Cyc_Absyndump_dump(((_tmp3F0="try",_tag_dyneither(_tmp3F0,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp1A1,0);
{const char*_tmp3F1;Cyc_Absyndump_dump(((_tmp3F1="catch{",_tag_dyneither(_tmp3F1,sizeof(char),7))));}
Cyc_Absyndump_dumpswitchclauses(_tmp1A2);Cyc_Absyndump_dump_char((int)'}');
goto _LL1AF;default: _LL1D4: _tmp1A0=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_LL1D5:
# 1030
{const char*_tmp3F2;Cyc_Absyndump_dump(((_tmp3F2="reset_region(",_tag_dyneither(_tmp3F2,sizeof(char),14))));}Cyc_Absyndump_dumpexp(_tmp1A0);{const char*_tmp3F3;Cyc_Absyndump_dump(((_tmp3F3=");",_tag_dyneither(_tmp3F3,sizeof(char),3))));}
goto _LL1AF;}_LL1AF:;};}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 1036
void Cyc_Absyndump_dumpdp(struct _tuple18*dp){
{const char*_tmp3F6;const char*_tmp3F5;const char*_tmp3F4;((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*dp).f1,((_tmp3F4="",_tag_dyneither(_tmp3F4,sizeof(char),1))),((_tmp3F5="=",_tag_dyneither(_tmp3F5,sizeof(char),2))),((_tmp3F6="=",_tag_dyneither(_tmp3F6,sizeof(char),2))));}
Cyc_Absyndump_dumppat((*dp).f2);}
# 1041
void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){
void*_tmp1DC=p->r;void*_tmp1DD=_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DE;struct Cyc_Absyn_Enumfield*_tmp1DF;struct Cyc_Absyn_Enumfield*_tmp1E0;struct Cyc_Absyn_Datatypefield*_tmp1E1;struct Cyc_List_List*_tmp1E2;int _tmp1E3;struct Cyc_List_List*_tmp1E4;struct Cyc_List_List*_tmp1E5;int _tmp1E6;union Cyc_Absyn_AggrInfoU _tmp1E7;struct Cyc_List_List*_tmp1E8;struct Cyc_List_List*_tmp1E9;int _tmp1EA;struct _tuple0*_tmp1EB;struct Cyc_List_List*_tmp1EC;int _tmp1ED;struct _tuple0*_tmp1EE;struct Cyc_Absyn_Tvar*_tmp1EF;struct Cyc_Absyn_Vardecl*_tmp1F0;struct Cyc_Absyn_Vardecl*_tmp1F1;struct Cyc_Absyn_Pat*_tmp1F2;struct Cyc_Absyn_Vardecl*_tmp1F3;struct Cyc_Absyn_Pat*_tmp1F4;struct Cyc_List_List*_tmp1F5;int _tmp1F6;struct Cyc_Absyn_Tvar*_tmp1F7;struct Cyc_Absyn_Vardecl*_tmp1F8;struct Cyc_Absyn_Vardecl*_tmp1F9;struct Cyc_Absyn_Pat*_tmp1FA;struct Cyc_Absyn_Vardecl*_tmp1FB;struct _dyneither_ptr _tmp1FC;char _tmp1FD;int _tmp1FE;int _tmp1FF;int _tmp200;switch(*((int*)_tmp1DD)){case 0: _LL1E7: _LL1E8:
 Cyc_Absyndump_dump_char((int)'_');goto _LL1E6;case 9: _LL1E9: _LL1EA:
{const char*_tmp3F7;Cyc_Absyndump_dump(((_tmp3F7="NULL",_tag_dyneither(_tmp3F7,sizeof(char),5))));}goto _LL1E6;case 10: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1){case Cyc_Absyn_None: _LL1EB: _tmp200=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_LL1EC:
 _tmp1FF=_tmp200;goto _LL1EE;case Cyc_Absyn_Signed: _LL1ED: _tmp1FF=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_LL1EE:
{const char*_tmp3FB;void*_tmp3FA[1];struct Cyc_Int_pa_PrintArg_struct _tmp3F9;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp3F9.tag=1,((_tmp3F9.f1=(unsigned long)_tmp1FF,((_tmp3FA[0]=& _tmp3F9,Cyc_aprintf(((_tmp3FB="%d",_tag_dyneither(_tmp3FB,sizeof(char),3))),_tag_dyneither(_tmp3FA,sizeof(void*),1)))))))));}goto _LL1E6;default: _LL1EF: _tmp1FE=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_LL1F0:
{const char*_tmp3FF;void*_tmp3FE[1];struct Cyc_Int_pa_PrintArg_struct _tmp3FD;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp3FD.tag=1,((_tmp3FD.f1=(unsigned int)_tmp1FE,((_tmp3FE[0]=& _tmp3FD,Cyc_aprintf(((_tmp3FF="%u",_tag_dyneither(_tmp3FF,sizeof(char),3))),_tag_dyneither(_tmp3FE,sizeof(void*),1)))))))));}goto _LL1E6;}case 11: _LL1F1: _tmp1FD=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_LL1F2:
# 1049
{const char*_tmp400;Cyc_Absyndump_dump(((_tmp400="'",_tag_dyneither(_tmp400,sizeof(char),2))));}Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp1FD));{const char*_tmp401;Cyc_Absyndump_dump_nospace(((_tmp401="'",_tag_dyneither(_tmp401,sizeof(char),2))));}goto _LL1E6;case 12: _LL1F3: _tmp1FC=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_LL1F4:
 Cyc_Absyndump_dump(_tmp1FC);goto _LL1E6;case 1: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2)->r)->tag == 0){_LL1F5: _tmp1FB=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_LL1F6:
 Cyc_Absyndump_dumpqvar(_tmp1FB->name);goto _LL1E6;}else{_LL1F7: _tmp1F9=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_tmp1FA=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_LL1F8:
 Cyc_Absyndump_dumpqvar(_tmp1F9->name);{const char*_tmp402;Cyc_Absyndump_dump(((_tmp402=" as ",_tag_dyneither(_tmp402,sizeof(char),5))));}Cyc_Absyndump_dumppat(_tmp1FA);goto _LL1E6;}case 2: _LL1F9: _tmp1F7=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_tmp1F8=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_LL1FA:
# 1054
{const char*_tmp403;Cyc_Absyndump_dump(((_tmp403="alias",_tag_dyneither(_tmp403,sizeof(char),6))));}
{const char*_tmp404;Cyc_Absyndump_dump(((_tmp404="<",_tag_dyneither(_tmp404,sizeof(char),2))));}Cyc_Absyndump_dumptvar(_tmp1F7);{const char*_tmp405;Cyc_Absyndump_dump(((_tmp405=">",_tag_dyneither(_tmp405,sizeof(char),2))));}
Cyc_Absyndump_dumpvardecl(_tmp1F8,p->loc,0);
goto _LL1E6;case 5: _LL1FB: _tmp1F5=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_tmp1F6=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_LL1FC: {
# 1059
const char*_tmp407;const char*_tmp406;struct _dyneither_ptr term=_tmp1F6?(_tmp407=", ...)",_tag_dyneither(_tmp407,sizeof(char),7)):((_tmp406=")",_tag_dyneither(_tmp406,sizeof(char),2)));
{const char*_tmp409;const char*_tmp408;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp1F5,((_tmp408="$(",_tag_dyneither(_tmp408,sizeof(char),3))),term,((_tmp409=",",_tag_dyneither(_tmp409,sizeof(char),2))));}goto _LL1E6;}case 6: _LL1FD: _tmp1F4=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_LL1FE:
{const char*_tmp40A;Cyc_Absyndump_dump(((_tmp40A="&",_tag_dyneither(_tmp40A,sizeof(char),2))));}Cyc_Absyndump_dumppat(_tmp1F4);goto _LL1E6;case 3: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2)->r)->tag == 0){_LL1FF: _tmp1F3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_LL200:
# 1063
{const char*_tmp40B;Cyc_Absyndump_dump(((_tmp40B="*",_tag_dyneither(_tmp40B,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmp1F3->name);goto _LL1E6;}else{_LL201: _tmp1F1=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_tmp1F2=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_LL202:
# 1065
{const char*_tmp40C;Cyc_Absyndump_dump(((_tmp40C="*",_tag_dyneither(_tmp40C,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmp1F1->name);{const char*_tmp40D;Cyc_Absyndump_dump(((_tmp40D=" as ",_tag_dyneither(_tmp40D,sizeof(char),5))));}Cyc_Absyndump_dumppat(_tmp1F2);goto _LL1E6;}case 4: _LL203: _tmp1EF=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_tmp1F0=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_LL204:
# 1067
 Cyc_Absyndump_dumpqvar(_tmp1F0->name);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptvar(_tmp1EF);Cyc_Absyndump_dump_char((int)'>');goto _LL1E6;case 15: _LL205: _tmp1EE=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_LL206:
 Cyc_Absyndump_dumpqvar(_tmp1EE);goto _LL1E6;case 16: _LL207: _tmp1EB=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_tmp1EC=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_tmp1ED=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1DD)->f3;_LL208: {
# 1070
const char*_tmp40F;const char*_tmp40E;struct _dyneither_ptr term=_tmp1ED?(_tmp40F=", ...)",_tag_dyneither(_tmp40F,sizeof(char),7)):((_tmp40E=")",_tag_dyneither(_tmp40E,sizeof(char),2)));
Cyc_Absyndump_dumpqvar(_tmp1EB);{const char*_tmp411;const char*_tmp410;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp1EC,((_tmp410="(",_tag_dyneither(_tmp410,sizeof(char),2))),term,((_tmp411=",",_tag_dyneither(_tmp411,sizeof(char),2))));}goto _LL1E6;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1 != 0){_LL209: _tmp1E7=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1)->aggr_info;_tmp1E8=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_tmp1E9=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DD)->f3;_tmp1EA=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DD)->f4;_LL20A: {
# 1073
struct _tuple10 _tmp21A=Cyc_Absyn_aggr_kinded_name(_tmp1E7);struct _tuple10 _tmp21B=_tmp21A;struct _tuple0*_tmp21C;_LL216: _tmp21C=_tmp21B.f2;_LL217: {
const char*_tmp413;const char*_tmp412;struct _dyneither_ptr term=_tmp1EA?(_tmp413=", ...)",_tag_dyneither(_tmp413,sizeof(char),7)):((_tmp412=")",_tag_dyneither(_tmp412,sizeof(char),2)));
Cyc_Absyndump_dumpqvar(_tmp21C);Cyc_Absyndump_dump_char((int)'{');
{const char*_tmp416;const char*_tmp415;const char*_tmp414;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp1E8,((_tmp414="[",_tag_dyneither(_tmp414,sizeof(char),2))),((_tmp415="]",_tag_dyneither(_tmp415,sizeof(char),2))),((_tmp416=",",_tag_dyneither(_tmp416,sizeof(char),2))));}
{const char*_tmp418;const char*_tmp417;((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp1E9,((_tmp417="",_tag_dyneither(_tmp417,sizeof(char),1))),term,((_tmp418=",",_tag_dyneither(_tmp418,sizeof(char),2))));}
goto _LL1E6;}_LL215:;}}else{_LL20B: _tmp1E4=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_tmp1E5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DD)->f3;_tmp1E6=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DD)->f4;_LL20C: {
# 1080
const char*_tmp41A;const char*_tmp419;struct _dyneither_ptr term=_tmp1E6?(_tmp41A=", ...)",_tag_dyneither(_tmp41A,sizeof(char),7)):((_tmp419=")",_tag_dyneither(_tmp419,sizeof(char),2)));
Cyc_Absyndump_dump_char((int)'{');
{const char*_tmp41D;const char*_tmp41C;const char*_tmp41B;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp1E4,((_tmp41B="[",_tag_dyneither(_tmp41B,sizeof(char),2))),((_tmp41C="]",_tag_dyneither(_tmp41C,sizeof(char),2))),((_tmp41D=",",_tag_dyneither(_tmp41D,sizeof(char),2))));}
{const char*_tmp41F;const char*_tmp41E;((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp1E5,((_tmp41E="",_tag_dyneither(_tmp41E,sizeof(char),1))),term,((_tmp41F=",",_tag_dyneither(_tmp41F,sizeof(char),2))));}
goto _LL1E6;}}case 8: _LL20D: _tmp1E1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_tmp1E2=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1DD)->f3;_tmp1E3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1DD)->f4;_LL20E: {
# 1086
const char*_tmp421;const char*_tmp420;struct _dyneither_ptr term=_tmp1E3?(_tmp421=", ...)",_tag_dyneither(_tmp421,sizeof(char),7)):((_tmp420=")",_tag_dyneither(_tmp420,sizeof(char),2)));
Cyc_Absyndump_dumpqvar(_tmp1E1->name);
if(_tmp1E2 != 0){const char*_tmp423;const char*_tmp422;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp1E2,((_tmp422="(",_tag_dyneither(_tmp422,sizeof(char),2))),term,((_tmp423=",",_tag_dyneither(_tmp423,sizeof(char),2))));}
goto _LL1E6;}case 13: _LL20F: _tmp1E0=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_LL210:
 _tmp1DF=_tmp1E0;goto _LL212;case 14: _LL211: _tmp1DF=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_LL212:
 Cyc_Absyndump_dumpqvar(_tmp1DF->name);goto _LL1E6;default: _LL213: _tmp1DE=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_LL214:
 Cyc_Absyndump_dumpexp(_tmp1DE);goto _LL1E6;}_LL1E6:;}
# 1096
void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->typs != 0)
Cyc_Absyndump_dumpargs(ef->typs);}
# 1101
void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){
const char*_tmp424;((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,fields,((_tmp424=",",_tag_dyneither(_tmp424,sizeof(char),2))));}
# 1104
void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->tag != 0){
{const char*_tmp425;Cyc_Absyndump_dump(((_tmp425=" = ",_tag_dyneither(_tmp425,sizeof(char),4))));}
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
# 1111
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){
const char*_tmp426;((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,fields,((_tmp426=",",_tag_dyneither(_tmp426,sizeof(char),2))));}
# 1115
void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp232=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp233=_tmp232;struct _dyneither_ptr*_tmp234;struct Cyc_Absyn_Tqual _tmp235;void*_tmp236;struct Cyc_Absyn_Exp*_tmp237;struct Cyc_List_List*_tmp238;struct Cyc_Absyn_Exp*_tmp239;_LL21B: _tmp234=_tmp233->name;_tmp235=_tmp233->tq;_tmp236=_tmp233->type;_tmp237=_tmp233->width;_tmp238=_tmp233->attributes;_tmp239=_tmp233->requires_clause;_LL21C:
# 1119
{enum Cyc_Cyclone_C_Compilers _tmp23A=Cyc_Cyclone_c_compiler;if(_tmp23A == Cyc_Cyclone_Gcc_c){_LL220: _LL221:
# 1121
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp235,_tmp236,Cyc_Absyndump_dump_str,_tmp234);
Cyc_Absyndump_dumpatts(_tmp238);
goto _LL21F;}else{_LL222: _LL223:
# 1125
 Cyc_Absyndump_dumpatts(_tmp238);
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp235,_tmp236,Cyc_Absyndump_dump_str,_tmp234);
goto _LL21F;}_LL21F:;}
# 1129
if((unsigned int)_tmp239){
{const char*_tmp427;Cyc_Absyndump_dump(((_tmp427="@requires ",_tag_dyneither(_tmp427,sizeof(char),11))));}
Cyc_Absyndump_dumpexp(_tmp239);}
# 1133
if(_tmp237 != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp(_tmp237);}
# 1137
Cyc_Absyndump_dump_semi();_LL21A:;}}
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
{void*_tmp23C=(*pr).f1;void*_tmp23D=_tmp23C;switch(*((int*)_tmp23D)){case 11: _LL225: _LL226:
 goto _LL224;case 1: _LL227: _LL228:
{const char*_tmp428;Cyc_Absyndump_dump(((_tmp428="_stdcall",_tag_dyneither(_tmp428,sizeof(char),9))));}goto _LL224;case 2: _LL229: _LL22A:
{const char*_tmp429;Cyc_Absyndump_dump(((_tmp429="_cdecl",_tag_dyneither(_tmp429,sizeof(char),7))));}goto _LL224;case 3: _LL22B: _LL22C:
{const char*_tmp42A;Cyc_Absyndump_dump(((_tmp42A="_fastcall",_tag_dyneither(_tmp42A,sizeof(char),10))));}goto _LL224;default: _LL22D: _LL22E:
 goto _LL224;}_LL224:;}
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
void*_tmp241=t;void*_tmp242;void*_tmp243;switch(*((int*)_tmp241)){case 1: _LL230: _tmp243=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp241)->f2;if(_tmp243 != 0){_LL231:
 return Cyc_Absyndump_is_char_ptr(_tmp243);}else{goto _LL234;}case 5: _LL232: _tmp242=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp241)->f1).elt_typ;_LL233:
# 1177
 L: {
void*_tmp244=_tmp242;void*_tmp245;void*_tmp246;switch(*((int*)_tmp244)){case 1: _LL237: _tmp246=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp244)->f2;if(_tmp246 != 0){_LL238:
 _tmp242=_tmp246;goto L;}else{goto _LL23D;}case 17: _LL239: _tmp245=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp244)->f4;if(_tmp245 != 0){_LL23A:
 _tmp242=_tmp245;goto L;}else{goto _LL23D;}case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp244)->f2 == Cyc_Absyn_Char_sz){_LL23B: _LL23C:
 return 1;}else{goto _LL23D;}default: _LL23D: _LL23E:
 return 0;}_LL236:;}default: _LL234: _LL235:
# 1184
 return 0;}_LL22F:;}
# 1188
void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,unsigned int loc,int do_semi){
struct Cyc_Absyn_Vardecl*_tmp247=vd;enum Cyc_Absyn_Scope _tmp248;struct _tuple0*_tmp249;struct Cyc_Absyn_Tqual _tmp24A;void*_tmp24B;struct Cyc_Absyn_Exp*_tmp24C;struct Cyc_List_List*_tmp24D;_LL240: _tmp248=_tmp247->sc;_tmp249=_tmp247->name;_tmp24A=_tmp247->tq;_tmp24B=_tmp247->type;_tmp24C=_tmp247->initializer;_tmp24D=_tmp247->attributes;_LL241:
{enum Cyc_Cyclone_C_Compilers _tmp24E=Cyc_Cyclone_c_compiler;if(_tmp24E == Cyc_Cyclone_Gcc_c){_LL245: _LL246:
# 1193
 if(_tmp248 == Cyc_Absyn_Extern  && Cyc_Absyndump_qvar_to_Cids){
void*_tmp24F=Cyc_Tcutil_compress(_tmp24B);void*_tmp250=_tmp24F;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp250)->tag == 9){_LL24A: _LL24B:
 goto _LL249;}else{_LL24C: _LL24D:
 Cyc_Absyndump_dumpscope(_tmp248);}_LL249:;}else{
# 1199
Cyc_Absyndump_dumpscope(_tmp248);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp24A,_tmp24B,Cyc_Absyndump_dumpqvar,_tmp249);
Cyc_Absyndump_dumpatts(_tmp24D);
goto _LL244;}else{_LL247: _LL248:
# 1204
 Cyc_Absyndump_dumpatts(_tmp24D);
Cyc_Absyndump_dumpscope(_tmp248);{
struct _RegionHandle _tmp251=_new_region("temp");struct _RegionHandle*temp=& _tmp251;_push_region(temp);{
int is_cp=Cyc_Absyndump_is_char_ptr(_tmp24B);
struct _tuple11 _tmp252=Cyc_Absynpp_to_tms(temp,_tmp24A,_tmp24B);struct _tuple11 _tmp253=_tmp252;struct Cyc_Absyn_Tqual _tmp254;void*_tmp255;struct Cyc_List_List*_tmp256;_LL24F: _tmp254=_tmp253.f1;_tmp255=_tmp253.f2;_tmp256=_tmp253.f3;_LL250: {
# 1210
void*call_conv=(void*)& Cyc_Absyn_Unused_att_val;
{struct Cyc_List_List*tms2=_tmp256;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp257=(void*)tms2->hd;void*_tmp258=_tmp257;struct Cyc_List_List*_tmp259;if(((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp258)->tag == 5){_LL254: _tmp259=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp258)->f2;_LL255:
# 1214
 for(0;_tmp259 != 0;_tmp259=_tmp259->tl){
void*_tmp25A=(void*)_tmp259->hd;void*_tmp25B=_tmp25A;switch(*((int*)_tmp25B)){case 1: _LL259: _LL25A:
 call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;goto _LL258;case 2: _LL25B: _LL25C:
 call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;goto _LL258;case 3: _LL25D: _LL25E:
 call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;goto _LL258;default: _LL25F: _LL260:
 goto _LL258;}_LL258:;}
# 1221
goto _LL253;}else{_LL256: _LL257:
 goto _LL253;}_LL253:;}}
# 1224
Cyc_Absyndump_dumptq(_tmp254);
Cyc_Absyndump_dumpntyp(_tmp255);{
struct _tuple19 _tmp42B;struct _tuple19 _tmp25C=(_tmp42B.f1=call_conv,((_tmp42B.f2=_tmp249,_tmp42B)));
((void(*)(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(struct _tuple19*),struct _tuple19*a))Cyc_Absyndump_dumptms)(is_cp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp256),Cyc_Absyndump_dump_callconv_qvar,& _tmp25C);};}_LL24E:;}
# 1229
_npop_handler(0);goto _LL244;
# 1206
;_pop_region(temp);};}_LL244:;}
# 1232
if(_tmp24C != 0){
Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp(_tmp24C);}
# 1236
if(do_semi)Cyc_Absyndump_dump_semi();_LL23F:;}
# 1239
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*ad){
Cyc_Absyndump_dumpscope(ad->sc);
if(ad->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
const char*_tmp42C;Cyc_Absyndump_dump(((_tmp42C="@tagged ",_tag_dyneither(_tmp42C,sizeof(char),9))));}
Cyc_Absyndump_dumpaggr_kind(ad->kind);
Cyc_Absyndump_dumpqvar(ad->name);
Cyc_Absyndump_dumpkindedtvars(ad->tvs);
if(ad->impl == 0)return;else{
# 1248
Cyc_Absyndump_dump_char((int)'{');
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0){
const char*_tmp42F;const char*_tmp42E;const char*_tmp42D;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmp42D="<",_tag_dyneither(_tmp42D,sizeof(char),2))),((_tmp42E=">",_tag_dyneither(_tmp42E,sizeof(char),2))),((_tmp42F=",",_tag_dyneither(_tmp42F,sizeof(char),2))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);}
# 1255
Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
{const char*_tmp430;Cyc_Absyndump_dump(((_tmp430="}",_tag_dyneither(_tmp430,sizeof(char),2))));}
Cyc_Absyndump_dumpatts(ad->attributes);}}
# 1260
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp263=ed;enum Cyc_Absyn_Scope _tmp264;struct _tuple0*_tmp265;struct Cyc_Core_Opt*_tmp266;_LL262: _tmp264=_tmp263->sc;_tmp265=_tmp263->name;_tmp266=_tmp263->fields;_LL263:
 Cyc_Absyndump_dumpscope(_tmp264);
{const char*_tmp431;Cyc_Absyndump_dump(((_tmp431="enum ",_tag_dyneither(_tmp431,sizeof(char),6))));}
Cyc_Absyndump_dumpqvar(_tmp265);
if(_tmp266 != 0){
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp266->v);{
const char*_tmp432;Cyc_Absyndump_dump_nospace(((_tmp432="}",_tag_dyneither(_tmp432,sizeof(char),2))));};}_LL261:;}
# 1271
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp269=dd;enum Cyc_Absyn_Scope _tmp26A;struct _tuple0*_tmp26B;struct Cyc_List_List*_tmp26C;struct Cyc_Core_Opt*_tmp26D;int _tmp26E;_LL267: _tmp26A=_tmp269->sc;_tmp26B=_tmp269->name;_tmp26C=_tmp269->tvs;_tmp26D=_tmp269->fields;_tmp26E=_tmp269->is_extensible;_LL268:
 Cyc_Absyndump_dumpscope(_tmp26A);
if(_tmp26E){const char*_tmp433;Cyc_Absyndump_dump(((_tmp433="@extensible ",_tag_dyneither(_tmp433,sizeof(char),13))));}
{const char*_tmp434;Cyc_Absyndump_dump(((_tmp434="datatype ",_tag_dyneither(_tmp434,sizeof(char),10))));}
Cyc_Absyndump_dumpqvar(_tmp26B);
Cyc_Absyndump_dumptvars(_tmp26C);
if(_tmp26D != 0){
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)_tmp26D->v);{
const char*_tmp435;Cyc_Absyndump_dump_nospace(((_tmp435="}",_tag_dyneither(_tmp435,sizeof(char),2))));};}_LL266:;}struct _tuple20{unsigned int f1;struct _tuple0*f2;int f3;};
# 1285
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){
Cyc_Absyndump_dumploc(d->loc);{
void*_tmp272=d->r;void*_tmp273=_tmp272;struct Cyc_List_List*_tmp274;struct Cyc_List_List*_tmp275;struct Cyc_List_List*_tmp276;struct _tuple0*_tmp277;struct Cyc_List_List*_tmp278;struct _dyneither_ptr*_tmp279;struct Cyc_List_List*_tmp27A;struct Cyc_Absyn_Typedefdecl*_tmp27B;struct Cyc_Absyn_Tvar*_tmp27C;struct Cyc_Absyn_Vardecl*_tmp27D;int _tmp27E;struct Cyc_Absyn_Exp*_tmp27F;struct Cyc_List_List*_tmp280;struct Cyc_Absyn_Pat*_tmp281;struct Cyc_Absyn_Exp*_tmp282;struct Cyc_Absyn_Enumdecl*_tmp283;struct Cyc_Absyn_Datatypedecl*_tmp284;struct Cyc_Absyn_Aggrdecl*_tmp285;struct Cyc_Absyn_Fndecl*_tmp286;struct Cyc_Absyn_Vardecl*_tmp287;switch(*((int*)_tmp273)){case 0: _LL26C: _tmp287=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_LL26D:
# 1289
 Cyc_Absyndump_dumpvardecl(_tmp287,d->loc,1);
goto _LL26B;case 1: _LL26E: _tmp286=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_LL26F:
# 1293
{enum Cyc_Cyclone_C_Compilers _tmp288=Cyc_Cyclone_c_compiler;if(_tmp288 == Cyc_Cyclone_Vc_c){_LL28B: _LL28C:
 Cyc_Absyndump_dumpatts(_tmp286->attributes);goto _LL28A;}else{_LL28D: _LL28E:
 goto _LL28A;}_LL28A:;}
# 1297
if(_tmp286->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp289=Cyc_Cyclone_c_compiler;enum Cyc_Cyclone_C_Compilers _tmp28A;if(_tmp289 == Cyc_Cyclone_Vc_c){_LL290: _LL291:
{const char*_tmp436;Cyc_Absyndump_dump(((_tmp436="__inline",_tag_dyneither(_tmp436,sizeof(char),9))));}goto _LL28F;}else{_LL292: _tmp28A=_tmp289;_LL293:
{const char*_tmp437;Cyc_Absyndump_dump(((_tmp437="inline",_tag_dyneither(_tmp437,sizeof(char),7))));}goto _LL28F;}_LL28F:;}
# 1302
Cyc_Absyndump_dumpscope(_tmp286->sc);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp43D;struct Cyc_Absyn_FnInfo _tmp43C;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp43B;void*t=(void*)((_tmp43B=_cycalloc(sizeof(*_tmp43B)),((_tmp43B[0]=((_tmp43D.tag=9,((_tmp43D.f1=((_tmp43C.tvars=_tmp286->tvs,((_tmp43C.effect=_tmp286->effect,((_tmp43C.ret_tqual=_tmp286->ret_tqual,((_tmp43C.ret_typ=_tmp286->ret_type,((_tmp43C.args=
# 1305
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp286->args),((_tmp43C.c_varargs=_tmp286->c_varargs,((_tmp43C.cyc_varargs=_tmp286->cyc_varargs,((_tmp43C.rgn_po=_tmp286->rgn_po,((_tmp43C.attributes=0,((_tmp43C.requires_clause=_tmp286->requires_clause,((_tmp43C.requires_relns=_tmp286->requires_relns,((_tmp43C.ensures_clause=_tmp286->ensures_clause,((_tmp43C.ensures_relns=_tmp286->ensures_relns,_tmp43C)))))))))))))))))))))))))),_tmp43D)))),_tmp43B))));
# 1310
{enum Cyc_Cyclone_C_Compilers _tmp28D=Cyc_Cyclone_c_compiler;if(_tmp28D == Cyc_Cyclone_Gcc_c){_LL295: _LL296:
# 1312
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_atts_qvar,_tmp286);
goto _LL294;}else{_LL297: _LL298:
# 1315
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_callconv_fdqvar,_tmp286);
goto _LL294;}_LL294:;}
# 1318
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpstmt(_tmp286->body,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL26B;};case 5: _LL270: _tmp285=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_LL271:
# 1323
 Cyc_Absyndump_dump_aggrdecl(_tmp285);Cyc_Absyndump_dump_semi();
goto _LL26B;case 6: _LL272: _tmp284=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_LL273:
# 1326
 Cyc_Absyndump_dump_datatypedecl(_tmp284);Cyc_Absyndump_dump_semi();
goto _LL26B;case 7: _LL274: _tmp283=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_LL275:
# 1329
 Cyc_Absyndump_dump_enumdecl(_tmp283);Cyc_Absyndump_dump_semi();
goto _LL26B;case 2: _LL276: _tmp281=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_tmp282=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp273)->f3;_LL277:
# 1332
{const char*_tmp43E;Cyc_Absyndump_dump(((_tmp43E="let",_tag_dyneither(_tmp43E,sizeof(char),4))));}
Cyc_Absyndump_dumppat(_tmp281);
Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp(_tmp282);
Cyc_Absyndump_dump_semi();
goto _LL26B;case 3: _LL278: _tmp280=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_LL279:
# 1339
{const char*_tmp43F;Cyc_Absyndump_dump(((_tmp43F="let ",_tag_dyneither(_tmp43F,sizeof(char),5))));}
Cyc_Absyndump_dumpids(_tmp280);
Cyc_Absyndump_dump_semi();
goto _LL26B;case 4: _LL27A: _tmp27C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_tmp27D=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp273)->f2;_tmp27E=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp273)->f3;_tmp27F=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp273)->f4;_LL27B:
# 1344
{const char*_tmp440;Cyc_Absyndump_dump(((_tmp440="region",_tag_dyneither(_tmp440,sizeof(char),7))));}
{const char*_tmp441;Cyc_Absyndump_dump(((_tmp441="<",_tag_dyneither(_tmp441,sizeof(char),2))));}Cyc_Absyndump_dumptvar(_tmp27C);{const char*_tmp442;Cyc_Absyndump_dump(((_tmp442="> ",_tag_dyneither(_tmp442,sizeof(char),3))));}Cyc_Absyndump_dumpqvar(_tmp27D->name);
if((unsigned int)_tmp27F){
{const char*_tmp443;Cyc_Absyndump_dump(((_tmp443=" = open(",_tag_dyneither(_tmp443,sizeof(char),9))));}
Cyc_Absyndump_dumpexp(_tmp27F);{
const char*_tmp444;Cyc_Absyndump_dump(((_tmp444=")",_tag_dyneither(_tmp444,sizeof(char),2))));};}
# 1351
if(_tmp27E){const char*_tmp445;Cyc_Absyndump_dump(((_tmp445="@resetable",_tag_dyneither(_tmp445,sizeof(char),11))));}
Cyc_Absyndump_dump_semi();
goto _LL26B;case 8: _LL27C: _tmp27B=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_LL27D:
# 1359
{const char*_tmp446;Cyc_Absyndump_dump(((_tmp446="typedef",_tag_dyneither(_tmp446,sizeof(char),8))));}{
void*t;
if(_tmp27B->defn == 0)
t=Cyc_Absyn_new_evar(_tmp27B->kind,0);else{
# 1364
t=(void*)_check_null(_tmp27B->defn);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(_tmp27B->tq,t,Cyc_Absyndump_dumptypedefname,_tmp27B);
Cyc_Absyndump_dumpatts(_tmp27B->atts);
Cyc_Absyndump_dump_semi();
# 1369
goto _LL26B;};case 9: _LL27E: _tmp279=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_tmp27A=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp273)->f2;_LL27F:
# 1371
{const char*_tmp447;Cyc_Absyndump_dump(((_tmp447="namespace ",_tag_dyneither(_tmp447,sizeof(char),11))));}
Cyc_Absyndump_dump_str(_tmp279);
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp27A != 0;_tmp27A=_tmp27A->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp27A->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL26B;case 10: _LL280: _tmp277=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_tmp278=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp273)->f2;_LL281:
# 1379
{const char*_tmp448;Cyc_Absyndump_dump(((_tmp448="using ",_tag_dyneither(_tmp448,sizeof(char),7))));}
Cyc_Absyndump_dumpqvar(_tmp277);
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp278 != 0;_tmp278=_tmp278->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp278->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL26B;case 11: _LL282: _tmp276=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_LL283:
# 1387
{const char*_tmp449;Cyc_Absyndump_dump(((_tmp449="extern \"C\" {",_tag_dyneither(_tmp449,sizeof(char),13))));}
for(0;_tmp276 != 0;_tmp276=_tmp276->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp276->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL26B;case 12: _LL284: _tmp274=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_tmp275=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp273)->f2;_LL285:
# 1393
{const char*_tmp44A;Cyc_Absyndump_dump(((_tmp44A="extern \"C include\" {",_tag_dyneither(_tmp44A,sizeof(char),21))));}
for(0;_tmp274 != 0;_tmp274=_tmp274->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp274->hd);}
Cyc_Absyndump_dump_char((int)'}');
if(_tmp275 != 0){
{const char*_tmp44B;Cyc_Absyndump_dump(((_tmp44B=" export {",_tag_dyneither(_tmp44B,sizeof(char),10))));}
for(0;_tmp275 != 0;_tmp275=_tmp275->tl){
struct _tuple20 _tmp29F=*((struct _tuple20*)_tmp275->hd);struct _tuple20 _tmp2A0=_tmp29F;struct _tuple0*_tmp2A1;_LL29A: _tmp2A1=_tmp2A0.f2;_LL29B:
 Cyc_Absyndump_dumpqvar(_tmp2A1);
if(_tmp275->tl != 0)Cyc_Absyndump_dump_char((int)',');_LL299:;}{
# 1404
const char*_tmp44C;Cyc_Absyndump_dump(((_tmp44C="}",_tag_dyneither(_tmp44C,sizeof(char),2))));};}
# 1406
goto _LL26B;case 13: _LL286: _LL287:
# 1408
{const char*_tmp44D;Cyc_Absyndump_dump(((_tmp44D=" __cyclone_port_on__; ",_tag_dyneither(_tmp44D,sizeof(char),23))));}
goto _LL26B;default: _LL288: _LL289:
# 1411
{const char*_tmp44E;Cyc_Absyndump_dump(((_tmp44E=" __cyclone_port_off__; ",_tag_dyneither(_tmp44E,sizeof(char),24))));}
goto _LL26B;}_LL26B:;};}
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
void*_tmp2A5=(void*)tms->hd;void*_tmp2A6=_tmp2A5;void*_tmp2A7;union Cyc_Absyn_Constraint*_tmp2A8;union Cyc_Absyn_Constraint*_tmp2A9;union Cyc_Absyn_Constraint*_tmp2AA;struct Cyc_Absyn_Tqual _tmp2AB;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2A6)->tag == 2){_LL29F: _tmp2A7=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2A6)->f1).rgn;_tmp2A8=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2A6)->f1).nullable;_tmp2A9=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2A6)->f1).bounds;_tmp2AA=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2A6)->f1).zero_term;_tmp2AB=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2A6)->f2;_LL2A0:
# 1432
{void*_tmp2AC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp2A9);void*_tmp2AD=_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AE;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2AD)->tag == 0){_LL2A4: _LL2A5:
 Cyc_Absyndump_dump_char((int)'?');goto _LL2A3;}else{_LL2A6: _tmp2AE=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2AD)->f1;_LL2A7:
# 1435
 Cyc_Absyndump_dump_char((int)(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp2A8)?'*':'@'));
Cyc_Absyndump_dump_upperbound(_tmp2AE);
goto _LL2A3;}_LL2A3:;}
# 1439
if((!Cyc_Absyndump_qvar_to_Cids  && !is_char_ptr) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2AA)){const char*_tmp44F;Cyc_Absyndump_dump(((_tmp44F="@zeroterm",_tag_dyneither(_tmp44F,sizeof(char),10))));}
if((!Cyc_Absyndump_qvar_to_Cids  && is_char_ptr) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2AA)){const char*_tmp450;Cyc_Absyndump_dump(((_tmp450="@nozeroterm",_tag_dyneither(_tmp450,sizeof(char),12))));}
{void*_tmp2B1=Cyc_Tcutil_compress(_tmp2A7);void*_tmp2B2=_tmp2B1;struct Cyc_Absyn_Tvar*_tmp2B3;switch(*((int*)_tmp2B2)){case 20: _LL2A9: _LL2AA:
 if(!Cyc_Absyndump_qvar_to_Cids){const char*_tmp451;Cyc_Absyndump_dump(((_tmp451="`H",_tag_dyneither(_tmp451,sizeof(char),3))));}goto _LL2A8;case 21: _LL2AB: _LL2AC:
{const char*_tmp452;Cyc_Absyndump_dump(((_tmp452="`U",_tag_dyneither(_tmp452,sizeof(char),3))));}goto _LL2A8;case 22: _LL2AD: _LL2AE:
{const char*_tmp453;Cyc_Absyndump_dump(((_tmp453="`RC",_tag_dyneither(_tmp453,sizeof(char),4))));}goto _LL2A8;case 2: _LL2AF: _tmp2B3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2B2)->f1;_LL2B0:
 Cyc_Absyndump_dumptvar(_tmp2B3);goto _LL2A8;case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B2)->f2 == 0){_LL2B1: _LL2B2:
 Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(_tmp2A7));goto _LL2A8;}else{goto _LL2B5;}case 17: _LL2B3: _LL2B4:
{const char*_tmp454;Cyc_Absyndump_dump(((_tmp454="@region(",_tag_dyneither(_tmp454,sizeof(char),9))));}Cyc_Absyndump_dumptyp(_tmp2A7);{const char*_tmp455;Cyc_Absyndump_dump(((_tmp455=")",_tag_dyneither(_tmp455,sizeof(char),2))));}goto _LL2A8;default: _LL2B5: _LL2B6: {
const char*_tmp458;void*_tmp457;(_tmp457=0,Cyc_Tcutil_impos(((_tmp458="dumptms: bad rgn type in Pointer_mod",_tag_dyneither(_tmp458,sizeof(char),37))),_tag_dyneither(_tmp457,sizeof(void*),0)));}}_LL2A8:;}
# 1450
Cyc_Absyndump_dumptq(_tmp2AB);
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
return;}else{_LL2A1: _LL2A2: {
# 1455
int next_is_pointer=0;
if(tms->tl != 0){
void*_tmp2BB=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmp2BC=_tmp2BB;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2BC)->tag == 2){_LL2B8: _LL2B9:
 next_is_pointer=1;goto _LL2B7;}else{_LL2BA: _LL2BB:
 goto _LL2B7;}_LL2B7:;}
# 1461
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)')');
{void*_tmp2BD=(void*)tms->hd;void*_tmp2BE=_tmp2BD;struct Cyc_List_List*_tmp2BF;struct Cyc_List_List*_tmp2C0;unsigned int _tmp2C1;int _tmp2C2;struct Cyc_List_List*_tmp2C3;unsigned int _tmp2C4;struct Cyc_List_List*_tmp2C5;int _tmp2C6;struct Cyc_Absyn_VarargInfo*_tmp2C7;void*_tmp2C8;struct Cyc_List_List*_tmp2C9;struct Cyc_Absyn_Exp*_tmp2CA;struct Cyc_Absyn_Exp*_tmp2CB;struct Cyc_Absyn_Exp*_tmp2CC;union Cyc_Absyn_Constraint*_tmp2CD;union Cyc_Absyn_Constraint*_tmp2CE;switch(*((int*)_tmp2BE)){case 0: _LL2BD: _tmp2CE=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1;_LL2BE:
# 1468
{const char*_tmp459;Cyc_Absyndump_dump(((_tmp459="[]",_tag_dyneither(_tmp459,sizeof(char),3))));}
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2CE)){const char*_tmp45A;Cyc_Absyndump_dump(((_tmp45A="@zeroterm",_tag_dyneither(_tmp45A,sizeof(char),10))));}
goto _LL2BC;case 1: _LL2BF: _tmp2CC=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1;_tmp2CD=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f2;_LL2C0:
# 1472
 Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp2CC);Cyc_Absyndump_dump_char((int)']');
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2CD)){const char*_tmp45B;Cyc_Absyndump_dump(((_tmp45B="@zeroterm",_tag_dyneither(_tmp45B,sizeof(char),10))));}
goto _LL2BC;case 3: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1)->tag == 1){_LL2C1: _tmp2C5=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1)->f1;_tmp2C6=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1)->f2;_tmp2C7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1)->f3;_tmp2C8=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1)->f4;_tmp2C9=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1)->f5;_tmp2CA=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1)->f6;_tmp2CB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1)->f7;_LL2C2:
# 1476
 Cyc_Absyndump_dumpfunargs(_tmp2C5,_tmp2C6,_tmp2C7,_tmp2C8,_tmp2C9,_tmp2CA,_tmp2CB);goto _LL2BC;}else{_LL2C3: _tmp2C3=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1)->f1;_tmp2C4=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1)->f2;_LL2C4:
# 1478
{const char*_tmp45E;const char*_tmp45D;const char*_tmp45C;((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,_tmp2C3,((_tmp45C="(",_tag_dyneither(_tmp45C,sizeof(char),2))),((_tmp45D=")",_tag_dyneither(_tmp45D,sizeof(char),2))),((_tmp45E=",",_tag_dyneither(_tmp45E,sizeof(char),2))));}goto _LL2BC;}case 4: _LL2C5: _tmp2C0=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1;_tmp2C1=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f2;_tmp2C2=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f3;_LL2C6:
# 1480
 if(_tmp2C2)Cyc_Absyndump_dumpkindedtvars(_tmp2C0);else{Cyc_Absyndump_dumptvars(_tmp2C0);}goto _LL2BC;case 5: _LL2C7: _tmp2BF=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f2;_LL2C8:
 Cyc_Absyndump_dumpatts(_tmp2BF);goto _LL2BC;default: _LL2C9: _LL2CA: {
const char*_tmp461;void*_tmp460;(_tmp460=0,Cyc_Tcutil_impos(((_tmp461="dumptms",_tag_dyneither(_tmp461,sizeof(char),8))),_tag_dyneither(_tmp460,sizeof(void*),0)));}}_LL2BC:;}
# 1484
return;}}_LL29E:;};}
# 1488
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
int _tmp2D7=Cyc_Absyndump_is_char_ptr(t);
struct _RegionHandle _tmp2D8=_new_region("temp");struct _RegionHandle*temp=& _tmp2D8;_push_region(temp);
{struct _tuple11 _tmp2D9=Cyc_Absynpp_to_tms(temp,tq,t);struct _tuple11 _tmp2DA=_tmp2D9;struct Cyc_Absyn_Tqual _tmp2DB;void*_tmp2DC;struct Cyc_List_List*_tmp2DD;_LL2CC: _tmp2DB=_tmp2DA.f1;_tmp2DC=_tmp2DA.f2;_tmp2DD=_tmp2DA.f3;_LL2CD:
 Cyc_Absyndump_dumptq(_tmp2DB);
Cyc_Absyndump_dumpntyp(_tmp2DC);
Cyc_Absyndump_dumptms(_tmp2D7,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2DD),f,a);_LL2CB:;}
# 1491
;_pop_region(temp);}
# 1498
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
# 1500
*Cyc_Absyndump_dump_file=f;
for(0;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}{
const char*_tmp464;void*_tmp463;(_tmp463=0,Cyc_fprintf(f,((_tmp464="\n",_tag_dyneither(_tmp464,sizeof(char),2))),_tag_dyneither(_tmp463,sizeof(void*),0)));};}
# 1506
static void Cyc_Absyndump_dump_decl_interface(struct Cyc_Absyn_Decl*d){
void*_tmp2E0=d->r;void*_tmp2E1=_tmp2E0;struct Cyc_List_List*_tmp2E2;struct Cyc_List_List*_tmp2E3;struct Cyc_List_List*_tmp2E4;struct Cyc_List_List*_tmp2E5;struct _dyneither_ptr*_tmp2E6;struct Cyc_List_List*_tmp2E7;struct Cyc_Absyn_Typedefdecl*_tmp2E8;struct Cyc_Absyn_Enumdecl*_tmp2E9;struct Cyc_Absyn_Datatypedecl*_tmp2EA;struct Cyc_Absyn_Aggrdecl*_tmp2EB;struct Cyc_Absyn_Fndecl*_tmp2EC;struct Cyc_Absyn_Vardecl*_tmp2ED;switch(*((int*)_tmp2E1)){case 0: _LL2D1: _tmp2ED=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2E1)->f1;_LL2D2:
# 1509
 if(_tmp2ED->sc == Cyc_Absyn_Static)return;{
struct Cyc_Absyn_Exp*init=_tmp2ED->initializer;
_tmp2ED->initializer=0;
if(_tmp2ED->sc == Cyc_Absyn_Public){
const char*_tmp465;Cyc_Absyndump_dump(((_tmp465="extern ",_tag_dyneither(_tmp465,sizeof(char),8))));}
Cyc_Absyndump_dumpvardecl(_tmp2ED,d->loc,1);
{const char*_tmp466;Cyc_Absyndump_dump(((_tmp466="\n",_tag_dyneither(_tmp466,sizeof(char),2))));}
_tmp2ED->initializer=init;
goto _LL2D0;};case 1: _LL2D3: _tmp2EC=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2E1)->f1;_LL2D4:
# 1519
 if(_tmp2EC->sc == Cyc_Absyn_Static)return;
Cyc_Absyndump_dumpscope(_tmp2EC->sc);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp46C;struct Cyc_Absyn_FnInfo _tmp46B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp46A;void*t=(void*)((_tmp46A=_cycalloc(sizeof(*_tmp46A)),((_tmp46A[0]=((_tmp46C.tag=9,((_tmp46C.f1=((_tmp46B.tvars=_tmp2EC->tvs,((_tmp46B.effect=_tmp2EC->effect,((_tmp46B.ret_tqual=_tmp2EC->ret_tqual,((_tmp46B.ret_typ=_tmp2EC->ret_type,((_tmp46B.args=
# 1523
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp2EC->args),((_tmp46B.c_varargs=_tmp2EC->c_varargs,((_tmp46B.cyc_varargs=_tmp2EC->cyc_varargs,((_tmp46B.rgn_po=_tmp2EC->rgn_po,((_tmp46B.attributes=0,((_tmp46B.requires_clause=_tmp2EC->requires_clause,((_tmp46B.requires_relns=_tmp2EC->requires_relns,((_tmp46B.ensures_clause=_tmp2EC->ensures_clause,((_tmp46B.ensures_relns=_tmp2EC->ensures_relns,_tmp46B)))))))))))))))))))))))))),_tmp46C)))),_tmp46A))));
# 1528
((void(*)(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*a))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_atts_qvar,_tmp2EC);
{const char*_tmp46D;Cyc_Absyndump_dump(((_tmp46D=";\n",_tag_dyneither(_tmp46D,sizeof(char),3))));}
goto _LL2D0;};case 5: _LL2D5: _tmp2EB=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2E1)->f1;_LL2D6:
# 1532
 if(_tmp2EB->sc == Cyc_Absyn_Static)return;{
struct Cyc_Absyn_AggrdeclImpl*impl=_tmp2EB->impl;
if(_tmp2EB->sc == Cyc_Absyn_Abstract)_tmp2EB->impl=0;
Cyc_Absyndump_dump_aggrdecl(_tmp2EB);
_tmp2EB->impl=impl;
{const char*_tmp46E;Cyc_Absyndump_dump(((_tmp46E=";\n",_tag_dyneither(_tmp46E,sizeof(char),3))));}
goto _LL2D0;};case 6: _LL2D7: _tmp2EA=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp2E1)->f1;_LL2D8:
# 1540
 Cyc_Absyndump_dump_datatypedecl(_tmp2EA);
{const char*_tmp46F;Cyc_Absyndump_dump(((_tmp46F=";\n",_tag_dyneither(_tmp46F,sizeof(char),3))));}
goto _LL2D0;case 7: _LL2D9: _tmp2E9=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2E1)->f1;_LL2DA:
# 1544
 Cyc_Absyndump_dump_enumdecl(_tmp2E9);
{const char*_tmp470;Cyc_Absyndump_dump(((_tmp470=";\n",_tag_dyneither(_tmp470,sizeof(char),3))));}
goto _LL2D0;case 8: _LL2DB: _tmp2E8=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2E1)->f1;_LL2DC:
# 1548
 if(_tmp2E8->defn == 0){
Cyc_Absyndump_dumpdecl(d);{
const char*_tmp471;Cyc_Absyndump_dump(((_tmp471="\n",_tag_dyneither(_tmp471,sizeof(char),2))));};}
# 1552
goto _LL2D0;case 9: _LL2DD: _tmp2E6=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2E1)->f1;_tmp2E7=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2E1)->f2;_LL2DE:
# 1554
{const char*_tmp472;Cyc_Absyndump_dump(((_tmp472="namespace ",_tag_dyneither(_tmp472,sizeof(char),11))));}
Cyc_Absyndump_dump_str(_tmp2E6);
{const char*_tmp473;Cyc_Absyndump_dump(((_tmp473="{\n",_tag_dyneither(_tmp473,sizeof(char),3))));}
for(0;_tmp2E7 != 0;_tmp2E7=_tmp2E7->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2E7->hd);}
{const char*_tmp474;Cyc_Absyndump_dump(((_tmp474="}\n",_tag_dyneither(_tmp474,sizeof(char),3))));}
goto _LL2D0;case 10: _LL2DF: _tmp2E5=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp2E1)->f2;_LL2E0:
# 1562
 for(0;_tmp2E5 != 0;_tmp2E5=_tmp2E5->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2E5->hd);}
goto _LL2D0;case 11: _LL2E1: _tmp2E4=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp2E1)->f1;_LL2E2:
# 1566
{const char*_tmp475;Cyc_Absyndump_dump(((_tmp475="extern \"C\" {",_tag_dyneither(_tmp475,sizeof(char),13))));}
for(0;_tmp2E4 != 0;_tmp2E4=_tmp2E4->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2E4->hd);}
{const char*_tmp476;Cyc_Absyndump_dump(((_tmp476="}\n",_tag_dyneither(_tmp476,sizeof(char),3))));}
goto _LL2D0;case 12: _LL2E3: _tmp2E2=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2E1)->f1;_tmp2E3=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2E1)->f2;_LL2E4:
# 1587 "absyndump.cyc"
 goto _LL2D0;default: _LL2E5: _LL2E6: {
const char*_tmp479;void*_tmp478;(_tmp478=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp479="bad top-level declaration",_tag_dyneither(_tmp479,sizeof(char),26))),_tag_dyneither(_tmp478,sizeof(void*),0)));}}_LL2D0:;}
# 1592
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f){
*Cyc_Absyndump_dump_file=f;
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}}
