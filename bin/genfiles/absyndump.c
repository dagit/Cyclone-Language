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
struct _tuple13 _tmp1=Cyc_Lex_xlate_pos(loc);struct _tuple13 _tmp2=_tmp1;struct _dyneither_ptr _tmp3;unsigned int _tmp4;_LL20: _tmp3=_tmp2.f1;_tmp4=_tmp2.f2;_LL21:;
if((char*)_tmp3.curr == (char*)(_tag_dyneither(0,0,0)).curr  || (char*)_tmp3.curr == (char*)last_file.curr  && _tmp4 == last_line)return;
if((char*)_tmp3.curr == (char*)last_file.curr  && _tmp4 == last_line + 1)
({void*_tmp5=0;Cyc_fprintf(*Cyc_Absyndump_dump_file,({const char*_tmp6="\n";_tag_dyneither(_tmp6,sizeof(char),2);}),_tag_dyneither(_tmp5,sizeof(void*),0));});else{
if((char*)_tmp3.curr == (char*)last_file.curr)
({struct Cyc_Int_pa_PrintArg_struct _tmp9;_tmp9.tag=1;_tmp9.f1=(unsigned long)((int)_tmp4);({void*_tmp7[1]={& _tmp9};Cyc_fprintf(*Cyc_Absyndump_dump_file,({const char*_tmp8="\n# %d\n";_tag_dyneither(_tmp8,sizeof(char),7);}),_tag_dyneither(_tmp7,sizeof(void*),1));});});else{
# 168
({struct Cyc_String_pa_PrintArg_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3);({struct Cyc_Int_pa_PrintArg_struct _tmpC;_tmpC.tag=1;_tmpC.f1=(unsigned long)((int)_tmp4);({void*_tmpA[2]={& _tmpC,& _tmpD};Cyc_fprintf(*Cyc_Absyndump_dump_file,({const char*_tmpB="\n# %d %s\n";_tag_dyneither(_tmpB,sizeof(char),10);}),_tag_dyneither(_tmpA,sizeof(void*),2));});});});}}
# 170
last_file=_tmp3;
last_line=_tmp4;};}
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
{union Cyc_Absyn_Nmspace _tmp10=(*v).f1;union Cyc_Absyn_Nmspace _tmp11=_tmp10;struct Cyc_List_List*_tmp12;struct Cyc_List_List*_tmp13;struct Cyc_List_List*_tmp14;switch((_tmp11.C_n).tag){case 4: _LL23: _LL24:
 _tmp14=0;goto _LL26;case 1: _LL25: _tmp14=(_tmp11.Rel_n).val;_LL26:
# 228
 _tmpF=_tmp14;
goto _LL22;case 3: _LL27: _tmp13=(_tmp11.C_n).val;_LL28:
# 234
 Cyc_Absyndump_dump_char((int)' ');
goto _LL22;default: _LL29: _tmp12=(_tmp11.Abs_n).val;_LL2A:
# 237
 if(Cyc_Absyndump_qvar_to_Cids  || Cyc_Absyndump_add_cyc_prefix)
prefix=& Cyc_Absynpp_cyc_stringptr;
_tmpF=_tmp12;
goto _LL22;}_LL22:;}
# 242
if(prefix != 0){
Cyc_Absyndump_dump_str(*prefix);
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{
Cyc_Absyndump_dump_nospace(({const char*_tmp15="::";_tag_dyneither(_tmp15,sizeof(char),3);}));}}
# 247
if(_tmpF != 0){
Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmpF->hd));
for(_tmpF=_tmpF->tl;_tmpF != 0;_tmpF=_tmpF->tl){
# 252
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{
Cyc_Absyndump_dump_nospace(({const char*_tmp16="::";_tag_dyneither(_tmp16,sizeof(char),3);}));}
Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmpF->hd));}
# 256
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_nospace(({const char*_tmp17="_";_tag_dyneither(_tmp17,sizeof(char),2);}));else{
Cyc_Absyndump_dump_nospace(({const char*_tmp18="::";_tag_dyneither(_tmp18,sizeof(char),3);}));}
Cyc_Absyndump_dump_nospace(*(*v).f2);}else{
# 260
if(prefix != 0)
Cyc_Absyndump_dump_nospace(*(*v).f2);else{
# 263
Cyc_Absyndump_dump_str((*v).f2);}}}
# 267
void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){
if(tq.q_restrict)Cyc_Absyndump_dump(({const char*_tmp19="restrict";_tag_dyneither(_tmp19,sizeof(char),9);}));
if(tq.q_volatile)Cyc_Absyndump_dump(({const char*_tmp1A="volatile";_tag_dyneither(_tmp1A,sizeof(char),9);}));
if(tq.print_const)Cyc_Absyndump_dump(({const char*_tmp1B="const";_tag_dyneither(_tmp1B,sizeof(char),6);}));}
# 273
void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc){
enum Cyc_Absyn_Scope _tmp1C=sc;switch(_tmp1C){case Cyc_Absyn_Static: _LL2C: _LL2D:
 Cyc_Absyndump_dump(({const char*_tmp1D="static";_tag_dyneither(_tmp1D,sizeof(char),7);}));return;case Cyc_Absyn_Public: _LL2E: _LL2F:
 return;case Cyc_Absyn_Extern: _LL30: _LL31:
 Cyc_Absyndump_dump(({const char*_tmp1E="extern";_tag_dyneither(_tmp1E,sizeof(char),7);}));return;case Cyc_Absyn_ExternC: _LL32: _LL33:
 Cyc_Absyndump_dump(({const char*_tmp1F="extern \"C\"";_tag_dyneither(_tmp1F,sizeof(char),11);}));return;case Cyc_Absyn_Abstract: _LL34: _LL35:
 Cyc_Absyndump_dump(({const char*_tmp20="abstract";_tag_dyneither(_tmp20,sizeof(char),9);}));return;default: _LL36: _LL37:
 Cyc_Absyndump_dump(({const char*_tmp21="register";_tag_dyneither(_tmp21,sizeof(char),9);}));return;}_LL2B:;}
# 284
void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp22=ka;enum Cyc_Absyn_KindQual _tmp23;enum Cyc_Absyn_AliasQual _tmp24;_LL39: _tmp23=_tmp22->kind;_tmp24=_tmp22->aliasqual;_LL3A:;{
enum Cyc_Absyn_KindQual _tmp25=_tmp23;switch(_tmp25){case Cyc_Absyn_AnyKind: _LL3C: _LL3D: {
# 288
enum Cyc_Absyn_AliasQual _tmp26=_tmp24;switch(_tmp26){case Cyc_Absyn_Aliasable: _LL49: _LL4A:
 Cyc_Absyndump_dump(({const char*_tmp27="A";_tag_dyneither(_tmp27,sizeof(char),2);}));return;case Cyc_Absyn_Unique: _LL4B: _LL4C:
 Cyc_Absyndump_dump(({const char*_tmp28="UA";_tag_dyneither(_tmp28,sizeof(char),3);}));return;default: _LL4D: _LL4E:
 Cyc_Absyndump_dump(({const char*_tmp29="TA";_tag_dyneither(_tmp29,sizeof(char),3);}));return;}_LL48:;}case Cyc_Absyn_MemKind: _LL3E: _LL3F: {
# 294
enum Cyc_Absyn_AliasQual _tmp2A=_tmp24;switch(_tmp2A){case Cyc_Absyn_Aliasable: _LL50: _LL51:
 Cyc_Absyndump_dump(({const char*_tmp2B="M";_tag_dyneither(_tmp2B,sizeof(char),2);}));return;case Cyc_Absyn_Unique: _LL52: _LL53:
 Cyc_Absyndump_dump(({const char*_tmp2C="UM";_tag_dyneither(_tmp2C,sizeof(char),3);}));return;default: _LL54: _LL55:
 Cyc_Absyndump_dump(({const char*_tmp2D="TM";_tag_dyneither(_tmp2D,sizeof(char),3);}));return;}_LL4F:;}case Cyc_Absyn_BoxKind: _LL40: _LL41: {
# 300
enum Cyc_Absyn_AliasQual _tmp2E=_tmp24;switch(_tmp2E){case Cyc_Absyn_Aliasable: _LL57: _LL58:
 Cyc_Absyndump_dump(({const char*_tmp2F="B";_tag_dyneither(_tmp2F,sizeof(char),2);}));return;case Cyc_Absyn_Unique: _LL59: _LL5A:
 Cyc_Absyndump_dump(({const char*_tmp30="UB";_tag_dyneither(_tmp30,sizeof(char),3);}));return;default: _LL5B: _LL5C:
 Cyc_Absyndump_dump(({const char*_tmp31="TB";_tag_dyneither(_tmp31,sizeof(char),3);}));return;}_LL56:;}case Cyc_Absyn_RgnKind: _LL42: _LL43: {
# 306
enum Cyc_Absyn_AliasQual _tmp32=_tmp24;switch(_tmp32){case Cyc_Absyn_Aliasable: _LL5E: _LL5F:
 Cyc_Absyndump_dump(({const char*_tmp33="R";_tag_dyneither(_tmp33,sizeof(char),2);}));return;case Cyc_Absyn_Unique: _LL60: _LL61:
 Cyc_Absyndump_dump(({const char*_tmp34="UR";_tag_dyneither(_tmp34,sizeof(char),3);}));return;default: _LL62: _LL63:
 Cyc_Absyndump_dump(({const char*_tmp35="TR";_tag_dyneither(_tmp35,sizeof(char),3);}));return;}_LL5D:;}case Cyc_Absyn_EffKind: _LL44: _LL45:
# 311
 Cyc_Absyndump_dump(({const char*_tmp36="E";_tag_dyneither(_tmp36,sizeof(char),2);}));return;default: _LL46: _LL47:
 Cyc_Absyndump_dump(({const char*_tmp37="I";_tag_dyneither(_tmp37,sizeof(char),2);}));return;}_LL3B:;};}
# 316
void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp38=k;if(_tmp38 == Cyc_Absyn_StructA){_LL65: _LL66:
 Cyc_Absyndump_dump(({const char*_tmp39="struct ";_tag_dyneither(_tmp39,sizeof(char),8);}));return;}else{_LL67: _LL68:
 Cyc_Absyndump_dump(({const char*_tmp3A="union ";_tag_dyneither(_tmp3A,sizeof(char),7);}));return;}_LL64:;}
# 323
void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){
((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptyp,ts,({const char*_tmp3B="<";_tag_dyneither(_tmp3B,sizeof(char),2);}),({const char*_tmp3C=">";_tag_dyneither(_tmp3C,sizeof(char),2);}),({const char*_tmp3D=",";_tag_dyneither(_tmp3D,sizeof(char),2);}));}
# 326
void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr n=*tv->name;
if(*((const char*)_check_dyneither_subscript(n,sizeof(char),0))== '#'){
Cyc_Absyndump_dump(({const char*_tmp3E="`G";_tag_dyneither(_tmp3E,sizeof(char),3);}));
{void*_tmp3F=Cyc_Absyn_compress_kb(tv->kind);void*_tmp40=_tmp3F;struct Cyc_Absyn_Kind*_tmp41;struct Cyc_Absyn_Kind*_tmp42;switch(*((int*)_tmp40)){case 0: _LL6A: _tmp42=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp40)->f1;_LL6B:
 _tmp41=_tmp42;goto _LL6D;case 2: _LL6C: _tmp41=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp40)->f2;_LL6D:
 Cyc_Absyndump_dumpkind(_tmp41);goto _LL69;default: _LL6E: _LL6F:
 Cyc_Absyndump_dump_nospace(({const char*_tmp43="K";_tag_dyneither(_tmp43,sizeof(char),2);}));goto _LL69;}_LL69:;}
# 335
Cyc_Absyndump_dump_nospace(_dyneither_ptr_plus(n,sizeof(char),1));}else{
# 337
Cyc_Absyndump_dump(n);}}
# 339
void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){
Cyc_Absyndump_dumptvar(tv);{
void*_tmp44=Cyc_Absyn_compress_kb(tv->kind);void*_tmp45=_tmp44;struct Cyc_Absyn_Kind*_tmp46;struct Cyc_Absyn_Kind*_tmp47;switch(*((int*)_tmp45)){case 1: _LL71: _LL72:
 goto _LL70;case 2: _LL73: _tmp47=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp45)->f2;_LL74:
 goto _LL70;default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp45)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp45)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL75: _LL76:
 goto _LL70;}else{goto _LL77;}}else{_LL77: _tmp46=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp45)->f1;_LL78:
 Cyc_Absyndump_dump(({const char*_tmp48="::";_tag_dyneither(_tmp48,sizeof(char),3);}));Cyc_Absyndump_dumpkind(_tmp46);goto _LL70;}}_LL70:;};}
# 348
void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,tvs,({const char*_tmp49="<";_tag_dyneither(_tmp49,sizeof(char),2);}),({const char*_tmp4A=">";_tag_dyneither(_tmp4A,sizeof(char),2);}),({const char*_tmp4B=",";_tag_dyneither(_tmp4B,sizeof(char),2);}));}
# 351
void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,tvs,({const char*_tmp4C="<";_tag_dyneither(_tmp4C,sizeof(char),2);}),({const char*_tmp4D=">";_tag_dyneither(_tmp4D,sizeof(char),2);}),({const char*_tmp4E=",";_tag_dyneither(_tmp4E,sizeof(char),2);}));}struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;};
# 355
void Cyc_Absyndump_dumparg(struct _tuple14*pr){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 358
void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){
((void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,ts,({const char*_tmp4F="(";_tag_dyneither(_tmp4F,sizeof(char),2);}),({const char*_tmp50=")";_tag_dyneither(_tmp50,sizeof(char),2);}),({const char*_tmp51=",";_tag_dyneither(_tmp51,sizeof(char),2);}));}
# 362
void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp52=(void*)atts->hd;void*_tmp53=_tmp52;switch(*((int*)_tmp53)){case 1: _LL7A: _LL7B:
 Cyc_Absyndump_dump(({const char*_tmp54="_stdcall";_tag_dyneither(_tmp54,sizeof(char),9);}));return;case 2: _LL7C: _LL7D:
 Cyc_Absyndump_dump(({const char*_tmp55="_cdecl";_tag_dyneither(_tmp55,sizeof(char),7);}));return;case 3: _LL7E: _LL7F:
 Cyc_Absyndump_dump(({const char*_tmp56="_fastcall";_tag_dyneither(_tmp56,sizeof(char),10);}));return;default: _LL80: _LL81:
 goto _LL79;}_LL79:;}}
# 372
void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){
# 374
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp57=(void*)((struct Cyc_List_List*)_check_null(atts))->hd;void*_tmp58=_tmp57;switch(*((int*)_tmp58)){case 1: _LL83: _LL84:
 goto _LL86;case 2: _LL85: _LL86:
 goto _LL88;case 3: _LL87: _LL88:
 goto _LL82;default: _LL89: _LL8A:
 hasatt=1;goto _LL82;}_LL82:;}}
# 382
if(!hasatt)
return;
Cyc_Absyndump_dump(({const char*_tmp59="__declspec(";_tag_dyneither(_tmp59,sizeof(char),12);}));
for(0;atts != 0;atts=atts->tl){
void*_tmp5A=(void*)atts->hd;void*_tmp5B=_tmp5A;switch(*((int*)_tmp5B)){case 1: _LL8C: _LL8D:
 goto _LL8F;case 2: _LL8E: _LL8F:
 goto _LL91;case 3: _LL90: _LL91:
 goto _LL8B;default: _LL92: _LL93:
 Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL8B;}_LL8B:;}
# 392
Cyc_Absyndump_dump_char((int)')');}
# 395
void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){
if(atts == 0)return;{
enum Cyc_Cyclone_C_Compilers _tmp5C=Cyc_Cyclone_c_compiler;if(_tmp5C == Cyc_Cyclone_Gcc_c){_LL95: _LL96:
# 399
 Cyc_Absyndump_dump(({const char*_tmp5D=" __attribute__((";_tag_dyneither(_tmp5D,sizeof(char),17);}));
for(0;atts != 0;atts=atts->tl){
Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));
if(atts->tl != 0)Cyc_Absyndump_dump(({const char*_tmp5E=",";_tag_dyneither(_tmp5E,sizeof(char),2);}));}
# 404
Cyc_Absyndump_dump(({const char*_tmp5F=")) ";_tag_dyneither(_tmp5F,sizeof(char),4);}));
return;}else{_LL97: _LL98:
# 407
 Cyc_Absyndump_dump_noncallconv(atts);
return;}_LL94:;};}
# 412
int Cyc_Absyndump_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmp60=(void*)tms->hd;void*_tmp61=_tmp60;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp61)->tag == 2){_LL9A: _LL9B:
 return 1;}else{_LL9C: _LL9D:
 return 0;}_LL99:;};}
# 420
static void Cyc_Absyndump_dumprgn(void*t){
void*_tmp62=Cyc_Tcutil_compress(t);void*_tmp63=_tmp62;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp63)->tag == 20){_LL9F: _LLA0:
 Cyc_Absyndump_dump(({const char*_tmp64="`H";_tag_dyneither(_tmp64,sizeof(char),3);}));goto _LL9E;}else{_LLA1: _LLA2:
 Cyc_Absyndump_dumpntyp(t);goto _LL9E;}_LL9E:;}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 427
static struct _tuple15 Cyc_Absyndump_effects_split(void*t){
struct Cyc_List_List*rgions=0;
struct Cyc_List_List*effects=0;
# 431
{void*_tmp65=Cyc_Tcutil_compress(t);void*_tmp66=_tmp65;struct Cyc_List_List*_tmp67;void*_tmp68;switch(*((int*)_tmp66)){case 23: _LLA4: _tmp68=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp66)->f1;_LLA5:
 rgions=({struct Cyc_List_List*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->hd=_tmp68;_tmp69->tl=rgions;_tmp69;});goto _LLA3;case 24: _LLA6: _tmp67=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp66)->f1;_LLA7:
# 434
 for(0;_tmp67 != 0;_tmp67=_tmp67->tl){
struct _tuple15 _tmp6A=Cyc_Absyndump_effects_split((void*)_tmp67->hd);struct _tuple15 _tmp6B=_tmp6A;struct Cyc_List_List*_tmp6C;struct Cyc_List_List*_tmp6D;_LLAB: _tmp6C=_tmp6B.f1;_tmp6D=_tmp6B.f2;_LLAC:;
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp6C,rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp6D,effects);}
# 439
goto _LLA3;default: _LLA8: _LLA9:
 effects=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=t;_tmp6E->tl=effects;_tmp6E;});goto _LLA3;}_LLA3:;}
# 442
return({struct _tuple15 _tmp6F;_tmp6F.f1=rgions;_tmp6F.f2=effects;_tmp6F;});}
# 445
static void Cyc_Absyndump_dumpeff(void*t){
struct _tuple15 _tmp70=Cyc_Absyndump_effects_split(t);struct _tuple15 _tmp71=_tmp70;struct Cyc_List_List*_tmp72;struct Cyc_List_List*_tmp73;_LLAE: _tmp72=_tmp71.f1;_tmp73=_tmp71.f2;_LLAF:;
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
Cyc_Absyndump_dump_char((int)'}');}}
# 467
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*);
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*);
# 472
void Cyc_Absyndump_dumpntyp(void*t){
void*_tmp75=t;struct Cyc_List_List*_tmp76;void*_tmp77;void*_tmp78;struct Cyc_Absyn_Datatypedecl*_tmp79;struct Cyc_Absyn_Enumdecl*_tmp7A;struct Cyc_Absyn_Aggrdecl*_tmp7B;void*_tmp7C;void*_tmp7D;void*_tmp7E;void*_tmp7F;struct _dyneither_ptr _tmp80;struct Cyc_Absyn_Exp*_tmp81;struct Cyc_Absyn_Exp*_tmp82;struct _tuple0*_tmp83;struct Cyc_List_List*_tmp84;struct Cyc_List_List*_tmp85;struct _tuple0*_tmp86;enum Cyc_Absyn_AggrKind _tmp87;struct Cyc_List_List*_tmp88;union Cyc_Absyn_AggrInfoU _tmp89;struct Cyc_List_List*_tmp8A;struct Cyc_List_List*_tmp8B;int _tmp8C;union Cyc_Absyn_DatatypeFieldInfoU _tmp8D;struct Cyc_List_List*_tmp8E;union Cyc_Absyn_DatatypeInfoU _tmp8F;struct Cyc_List_List*_tmp90;struct Cyc_Core_Opt*_tmp91;void*_tmp92;int _tmp93;struct Cyc_Core_Opt*_tmp94;int _tmp95;struct Cyc_Absyn_Tvar*_tmp96;switch(*((int*)_tmp75)){case 8: _LLB1: _LLB2:
# 475
 goto _LLB4;case 9: _LLB3: _LLB4:
 goto _LLB6;case 5: _LLB5: _LLB6:
 return;case 0: _LLB7: _LLB8:
 Cyc_Absyndump_dump(({const char*_tmp97="void";_tag_dyneither(_tmp97,sizeof(char),5);}));return;case 2: _LLB9: _tmp96=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp75)->f1;_LLBA:
 Cyc_Absyndump_dumptvar(_tmp96);return;case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp75)->f2 == 0){_LLBB: _tmp94=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp75)->f1;_tmp95=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp75)->f3;_LLBC:
# 481
 Cyc_Absyndump_dump(({const char*_tmp98="`E";_tag_dyneither(_tmp98,sizeof(char),3);}));
if(_tmp94 == 0)Cyc_Absyndump_dump(({const char*_tmp99="K";_tag_dyneither(_tmp99,sizeof(char),2);}));else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)_tmp94->v);}
Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp9C;_tmp9C.tag=1;_tmp9C.f1=(unsigned long)_tmp95;({void*_tmp9A[1]={& _tmp9C};Cyc_aprintf(({const char*_tmp9B="%d";_tag_dyneither(_tmp9B,sizeof(char),3);}),_tag_dyneither(_tmp9A,sizeof(void*),1));});}));return;}else{_LLBD: _tmp91=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp75)->f1;_tmp92=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp75)->f2;_tmp93=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp75)->f3;_LLBE:
 Cyc_Absyndump_dumpntyp((void*)_check_null(_tmp92));return;}case 3: _LLBF: _tmp8F=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp75)->f1).datatype_info;_tmp90=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp75)->f1).targs;_LLC0:
# 486
{union Cyc_Absyn_DatatypeInfoU _tmp9D=_tmp8F;struct _tuple0*_tmp9E;int _tmp9F;struct _tuple0*_tmpA0;int _tmpA1;if((_tmp9D.UnknownDatatype).tag == 1){_LL10E: _tmpA0=((_tmp9D.UnknownDatatype).val).name;_tmpA1=((_tmp9D.UnknownDatatype).val).is_extensible;_LL10F:
 _tmp9E=_tmpA0;_tmp9F=_tmpA1;goto _LL111;}else{_LL110: _tmp9E=(*(_tmp9D.KnownDatatype).val)->name;_tmp9F=(*(_tmp9D.KnownDatatype).val)->is_extensible;_LL111:
# 489
 if(_tmp9F)Cyc_Absyndump_dump(({const char*_tmpA2="@extensible ";_tag_dyneither(_tmpA2,sizeof(char),13);}));
Cyc_Absyndump_dump(({const char*_tmpA3="datatype ";_tag_dyneither(_tmpA3,sizeof(char),10);}));
Cyc_Absyndump_dumpqvar(_tmp9E);
Cyc_Absyndump_dumptps(_tmp90);
goto _LL10D;}_LL10D:;}
# 495
goto _LLB0;case 4: _LLC1: _tmp8D=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp75)->f1).field_info;_tmp8E=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp75)->f1).targs;_LLC2:
# 497
{union Cyc_Absyn_DatatypeFieldInfoU _tmpA4=_tmp8D;struct _tuple0*_tmpA5;int _tmpA6;struct _tuple0*_tmpA7;struct _tuple0*_tmpA8;struct _tuple0*_tmpA9;int _tmpAA;if((_tmpA4.UnknownDatatypefield).tag == 1){_LL113: _tmpA8=((_tmpA4.UnknownDatatypefield).val).datatype_name;_tmpA9=((_tmpA4.UnknownDatatypefield).val).field_name;_tmpAA=((_tmpA4.UnknownDatatypefield).val).is_extensible;_LL114:
# 499
 _tmpA5=_tmpA8;_tmpA6=_tmpAA;_tmpA7=_tmpA9;goto _LL116;}else{_LL115: _tmpA5=(((_tmpA4.KnownDatatypefield).val).f1)->name;_tmpA6=(((_tmpA4.KnownDatatypefield).val).f1)->is_extensible;_tmpA7=(((_tmpA4.KnownDatatypefield).val).f2)->name;_LL116:
# 502
 if(_tmpA6)Cyc_Absyndump_dump(({const char*_tmpAB="@extensible ";_tag_dyneither(_tmpAB,sizeof(char),13);}));
Cyc_Absyndump_dump(({const char*_tmpAC="datatype ";_tag_dyneither(_tmpAC,sizeof(char),10);}));
Cyc_Absyndump_dumpqvar(_tmpA5);
Cyc_Absyndump_dump(({const char*_tmpAD=".";_tag_dyneither(_tmpAD,sizeof(char),2);}));
Cyc_Absyndump_dumpqvar(_tmpA7);
Cyc_Absyndump_dumptps(_tmp8E);
goto _LL112;}_LL112:;}
# 510
goto _LLB0;case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75)->f1){case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75)->f2){case Cyc_Absyn_Int_sz: _LLC3: _LLC4:
 goto _LLC6;case Cyc_Absyn_Long_sz: _LLC7: _LLC8:
# 513
 goto _LLCA;case Cyc_Absyn_Char_sz: _LLCB: _LLCC:
# 515
 Cyc_Absyndump_dump(({const char*_tmpB0="char";_tag_dyneither(_tmpB0,sizeof(char),5);}));return;case Cyc_Absyn_Short_sz: _LLD1: _LLD2:
# 518
 goto _LLD4;default: _LLDB: _LLDC:
# 523
 goto _LLDE;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75)->f2){case Cyc_Absyn_Int_sz: _LLC5: _LLC6:
# 512
 Cyc_Absyndump_dump(({const char*_tmpAE="int";_tag_dyneither(_tmpAE,sizeof(char),4);}));return;case Cyc_Absyn_Long_sz: _LLC9: _LLCA:
# 514
 Cyc_Absyndump_dump(({const char*_tmpAF="long";_tag_dyneither(_tmpAF,sizeof(char),5);}));return;case Cyc_Absyn_Char_sz: _LLCD: _LLCE:
# 516
 Cyc_Absyndump_dump(({const char*_tmpB1="signed char";_tag_dyneither(_tmpB1,sizeof(char),12);}));return;case Cyc_Absyn_Short_sz: _LLD3: _LLD4:
# 519
 Cyc_Absyndump_dump(({const char*_tmpB3="short";_tag_dyneither(_tmpB3,sizeof(char),6);}));return;default: _LLDD: _LLDE: {
# 525
enum Cyc_Cyclone_C_Compilers _tmpB7=Cyc_Cyclone_c_compiler;if(_tmpB7 == Cyc_Cyclone_Gcc_c){_LL118: _LL119:
 Cyc_Absyndump_dump(({const char*_tmpB8="long long";_tag_dyneither(_tmpB8,sizeof(char),10);}));return;}else{_LL11A: _LL11B:
 Cyc_Absyndump_dump(({const char*_tmpB9="__int64";_tag_dyneither(_tmpB9,sizeof(char),8);}));return;}_LL117:;}}default: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75)->f2){case Cyc_Absyn_Char_sz: _LLCF: _LLD0:
# 517
 Cyc_Absyndump_dump(({const char*_tmpB2="unsigned char";_tag_dyneither(_tmpB2,sizeof(char),14);}));return;case Cyc_Absyn_Short_sz: _LLD5: _LLD6:
# 520
 Cyc_Absyndump_dump(({const char*_tmpB4="unsigned short";_tag_dyneither(_tmpB4,sizeof(char),15);}));return;case Cyc_Absyn_Int_sz: _LLD7: _LLD8:
 Cyc_Absyndump_dump(({const char*_tmpB5="unsigned int";_tag_dyneither(_tmpB5,sizeof(char),13);}));return;case Cyc_Absyn_Long_sz: _LLD9: _LLDA:
 Cyc_Absyndump_dump(({const char*_tmpB6="unsigned long";_tag_dyneither(_tmpB6,sizeof(char),14);}));return;default: _LLDF: _LLE0: {
# 530
enum Cyc_Cyclone_C_Compilers _tmpBA=Cyc_Cyclone_c_compiler;if(_tmpBA == Cyc_Cyclone_Vc_c){_LL11D: _LL11E:
 Cyc_Absyndump_dump(({const char*_tmpBB="unsigned __int64";_tag_dyneither(_tmpBB,sizeof(char),17);}));return;}else{_LL11F: _LL120:
 Cyc_Absyndump_dump(({const char*_tmpBC="unsigned long long";_tag_dyneither(_tmpBC,sizeof(char),19);}));return;}_LL11C:;}}}case 7: _LLE1: _tmp8C=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp75)->f1;_LLE2:
# 535
 if(_tmp8C == 0)
Cyc_Absyndump_dump(({const char*_tmpBD="float";_tag_dyneither(_tmpBD,sizeof(char),6);}));else{
if(_tmp8C == 1)
Cyc_Absyndump_dump(({const char*_tmpBE="double";_tag_dyneither(_tmpBE,sizeof(char),7);}));else{
# 540
Cyc_Absyndump_dump(({const char*_tmpBF="long double";_tag_dyneither(_tmpBF,sizeof(char),12);}));}}
return;case 10: _LLE3: _tmp8B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp75)->f1;_LLE4:
 Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(_tmp8B);return;case 11: _LLE5: _tmp89=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp75)->f1).aggr_info;_tmp8A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp75)->f1).targs;_LLE6: {
# 544
struct _tuple10 _tmpC0=Cyc_Absyn_aggr_kinded_name(_tmp89);struct _tuple10 _tmpC1=_tmpC0;enum Cyc_Absyn_AggrKind _tmpC2;struct _tuple0*_tmpC3;_LL122: _tmpC2=_tmpC1.f1;_tmpC3=_tmpC1.f2;_LL123:;
Cyc_Absyndump_dumpaggr_kind(_tmpC2);Cyc_Absyndump_dumpqvar(_tmpC3);Cyc_Absyndump_dumptps(_tmp8A);
return;}case 12: _LLE7: _tmp87=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp75)->f1;_tmp88=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp75)->f2;_LLE8:
# 548
 Cyc_Absyndump_dumpaggr_kind(_tmp87);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(_tmp88);Cyc_Absyndump_dump_char((int)'}');return;case 13: _LLE9: _tmp86=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp75)->f1;_LLEA:
 Cyc_Absyndump_dump(({const char*_tmpC4="enum ";_tag_dyneither(_tmpC4,sizeof(char),6);}));Cyc_Absyndump_dumpqvar(_tmp86);return;case 14: _LLEB: _tmp85=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp75)->f1;_LLEC:
 Cyc_Absyndump_dump(({const char*_tmpC5="enum {";_tag_dyneither(_tmpC5,sizeof(char),7);}));Cyc_Absyndump_dumpenumfields(_tmp85);Cyc_Absyndump_dump(({const char*_tmpC6="}";_tag_dyneither(_tmpC6,sizeof(char),2);}));return;case 17: _LLED: _tmp83=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp75)->f1;_tmp84=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp75)->f2;_LLEE:
(Cyc_Absyndump_dumpqvar(_tmp83),Cyc_Absyndump_dumptps(_tmp84));return;case 18: _LLEF: _tmp82=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp75)->f1;_LLF0:
 Cyc_Absyndump_dump(({const char*_tmpC7="valueof_t(";_tag_dyneither(_tmpC7,sizeof(char),11);}));Cyc_Absyndump_dumpexp(_tmp82);Cyc_Absyndump_dump(({const char*_tmpC8=")";_tag_dyneither(_tmpC8,sizeof(char),2);}));return;case 27: _LLF1: _tmp81=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp75)->f1;_LLF2:
 Cyc_Absyndump_dump(({const char*_tmpC9="typeof(";_tag_dyneither(_tmpC9,sizeof(char),8);}));Cyc_Absyndump_dumpexp(_tmp81);Cyc_Absyndump_dump(({const char*_tmpCA=")";_tag_dyneither(_tmpCA,sizeof(char),2);}));return;case 28: _LLF3: _tmp80=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp75)->f1;_LLF4:
 Cyc_Absyndump_dump(_tmp80);return;case 15: _LLF5: _tmp7F=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp75)->f1;_LLF6:
# 556
 Cyc_Absyndump_dump(({const char*_tmpCB="region_t<";_tag_dyneither(_tmpCB,sizeof(char),10);}));Cyc_Absyndump_dumprgn(_tmp7F);Cyc_Absyndump_dump(({const char*_tmpCC=">";_tag_dyneither(_tmpCC,sizeof(char),2);}));return;case 16: _LLF7: _tmp7D=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp75)->f1;_tmp7E=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp75)->f2;_LLF8:
 Cyc_Absyndump_dump(({const char*_tmpCD="dynregion_t<";_tag_dyneither(_tmpCD,sizeof(char),13);}));Cyc_Absyndump_dumprgn(_tmp7D);
Cyc_Absyndump_dump(({const char*_tmpCE=",";_tag_dyneither(_tmpCE,sizeof(char),2);}));Cyc_Absyndump_dumprgn(_tmp7E);Cyc_Absyndump_dump(({const char*_tmpCF=">";_tag_dyneither(_tmpCF,sizeof(char),2);}));return;case 19: _LLF9: _tmp7C=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp75)->f1;_LLFA:
 Cyc_Absyndump_dump(({const char*_tmpD0="tag_t<";_tag_dyneither(_tmpD0,sizeof(char),7);}));Cyc_Absyndump_dumpntyp(_tmp7C);Cyc_Absyndump_dump(({const char*_tmpD1=">";_tag_dyneither(_tmpD1,sizeof(char),2);}));return;case 21: _LLFB: _LLFC:
 Cyc_Absyndump_dump(({const char*_tmpD2="`U";_tag_dyneither(_tmpD2,sizeof(char),3);}));goto _LLB0;case 22: _LLFD: _LLFE:
 Cyc_Absyndump_dump(({const char*_tmpD3="`RC";_tag_dyneither(_tmpD3,sizeof(char),4);}));goto _LLB0;case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp75)->f1)->r)){case 0: _LLFF: _tmp7B=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp75)->f1)->r)->f1;_LL100:
# 563
 Cyc_Absyndump_dump_aggrdecl(_tmp7B);return;case 1: _LL101: _tmp7A=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp75)->f1)->r)->f1;_LL102:
# 565
 Cyc_Absyndump_dump_enumdecl(_tmp7A);return;default: _LL103: _tmp79=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp75)->f1)->r)->f1;_LL104:
# 567
 Cyc_Absyndump_dump_datatypedecl(_tmp79);return;}case 20: _LL105: _LL106:
 Cyc_Absyndump_dump(({const char*_tmpD4="`H";_tag_dyneither(_tmpD4,sizeof(char),3);}));goto _LLB0;case 23: _LL107: _tmp78=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp75)->f1;_LL108:
 Cyc_Absyndump_dump(({const char*_tmpD5="{";_tag_dyneither(_tmpD5,sizeof(char),2);}));Cyc_Absyndump_dumptyp(_tmp78);Cyc_Absyndump_dump(({const char*_tmpD6="}";_tag_dyneither(_tmpD6,sizeof(char),2);}));goto _LLB0;case 25: _LL109: _tmp77=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp75)->f1;_LL10A:
 Cyc_Absyndump_dump(({const char*_tmpD7="regions(";_tag_dyneither(_tmpD7,sizeof(char),9);}));Cyc_Absyndump_dumptyp(_tmp77);Cyc_Absyndump_dump(({const char*_tmpD8=")";_tag_dyneither(_tmpD8,sizeof(char),2);}));goto _LLB0;default: _LL10B: _tmp76=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp75)->f1;_LL10C:
# 572
 for(0;_tmp76 != 0;_tmp76=_tmp76->tl){
Cyc_Absyndump_dumptyp((void*)_tmp76->hd);
if(_tmp76->tl != 0)Cyc_Absyndump_dump(({const char*_tmpD9="+";_tag_dyneither(_tmpD9,sizeof(char),2);}));}}_LLB0:;}
# 579
void Cyc_Absyndump_dumpvaropt(struct _dyneither_ptr*vo){
if(vo != 0)Cyc_Absyndump_dump_str(vo);}
# 582
void Cyc_Absyndump_dumpfunarg(struct _tuple8*t){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple16{void*f1;void*f2;};
# 585
void Cyc_Absyndump_dump_rgncmp(struct _tuple16*cmp){
struct _tuple16*_tmpDA=cmp;void*_tmpDB;void*_tmpDC;_LL125: _tmpDB=_tmpDA->f1;_tmpDC=_tmpDA->f2;_LL126:;
Cyc_Absyndump_dumpeff(_tmpDB);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(_tmpDC);}
# 589
void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){
((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,rgn_po,({const char*_tmpDD=",";_tag_dyneither(_tmpDD,sizeof(char),2);}));}
# 592
void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 596
Cyc_Absyndump_dump_char((int)'(');
for(0;args != 0;args=args->tl){
Cyc_Absyndump_dumpfunarg((struct _tuple8*)args->hd);
if((args->tl != 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((int)',');}
# 601
if(c_varargs)
Cyc_Absyndump_dump(({const char*_tmpDE="...";_tag_dyneither(_tmpDE,sizeof(char),4);}));else{
if(cyc_varargs != 0){
struct _tuple8*_tmpDF=({struct _tuple8*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->f1=cyc_varargs->name;_tmpE2->f2=cyc_varargs->tq;_tmpE2->f3=cyc_varargs->type;_tmpE2;});
Cyc_Absyndump_dump(({const char*_tmpE0="...";_tag_dyneither(_tmpE0,sizeof(char),4);}));
if(cyc_varargs->inject)Cyc_Absyndump_dump(({const char*_tmpE1=" inject ";_tag_dyneither(_tmpE1,sizeof(char),9);}));
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
Cyc_Absyndump_dump(({const char*_tmpE3=" @requires(";_tag_dyneither(_tmpE3,sizeof(char),12);}));
Cyc_Absyndump_dumpexp(req);
Cyc_Absyndump_dump_char((int)')');}
# 623
if(ens != 0){
Cyc_Absyndump_dump(({const char*_tmpE4=" @ensures(";_tag_dyneither(_tmpE4,sizeof(char),11);}));
Cyc_Absyndump_dumpexp(ens);
Cyc_Absyndump_dump_char((int)')');}}
# 630
void Cyc_Absyndump_dumptyp(void*t){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 634
void Cyc_Absyndump_dumpdesignator(void*d){
void*_tmpE5=d;struct _dyneither_ptr*_tmpE6;struct Cyc_Absyn_Exp*_tmpE7;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE5)->tag == 0){_LL128: _tmpE7=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE5)->f1;_LL129:
 Cyc_Absyndump_dump(({const char*_tmpE8=".[";_tag_dyneither(_tmpE8,sizeof(char),3);}));Cyc_Absyndump_dumpexp(_tmpE7);Cyc_Absyndump_dump_char((int)']');goto _LL127;}else{_LL12A: _tmpE6=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpE5)->f1;_LL12B:
 Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmpE6);goto _LL127;}_LL127:;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 641
void Cyc_Absyndump_dumpde(struct _tuple17*de){
((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*de).f1,({const char*_tmpE9="";_tag_dyneither(_tmpE9,sizeof(char),1);}),({const char*_tmpEA="=";_tag_dyneither(_tmpEA,sizeof(char),2);}),({const char*_tmpEB="=";_tag_dyneither(_tmpEB,sizeof(char),2);}));
Cyc_Absyndump_dumpexp((*de).f2);}
# 646
void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){
((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,inprec,es,({const char*_tmpEC="";_tag_dyneither(_tmpEC,sizeof(char),1);}),({const char*_tmpED="";_tag_dyneither(_tmpED,sizeof(char),1);}),({const char*_tmpEE=",";_tag_dyneither(_tmpEE,sizeof(char),2);}));}
# 650
void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
if(inprec >= myprec)
Cyc_Absyndump_dump_nospace(({const char*_tmpEF="(";_tag_dyneither(_tmpEF,sizeof(char),2);}));
{void*_tmpF0=e->r;void*_tmpF1=_tmpF0;struct Cyc_Absyn_Stmt*_tmpF2;struct Cyc_Core_Opt*_tmpF3;struct Cyc_List_List*_tmpF4;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*_tmpF6;int _tmpF7;struct Cyc_Absyn_Exp*_tmpF8;void**_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;int _tmpFB;struct _tuple0*_tmpFC;struct _tuple0*_tmpFD;struct Cyc_List_List*_tmpFE;struct Cyc_Absyn_Datatypefield*_tmpFF;struct Cyc_List_List*_tmp100;struct _tuple0*_tmp101;struct Cyc_List_List*_tmp102;struct Cyc_List_List*_tmp103;struct Cyc_Absyn_Exp*_tmp104;void*_tmp105;struct Cyc_Absyn_Vardecl*_tmp106;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_List_List*_tmp109;struct _tuple8*_tmp10A;struct Cyc_List_List*_tmp10B;struct Cyc_List_List*_tmp10C;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10F;struct _dyneither_ptr*_tmp110;struct Cyc_Absyn_Exp*_tmp111;struct _dyneither_ptr*_tmp112;struct Cyc_Absyn_Exp*_tmp113;void*_tmp114;struct Cyc_List_List*_tmp115;struct Cyc_Absyn_Exp*_tmp116;struct _dyneither_ptr*_tmp117;int _tmp118;struct _dyneither_ptr _tmp119;void*_tmp11A;struct Cyc_Absyn_Exp*_tmp11B;void*_tmp11C;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11F;void*_tmp120;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_List_List*_tmp126;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp12B;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_Core_Opt*_tmp135;struct Cyc_Absyn_Exp*_tmp136;enum Cyc_Absyn_Primop _tmp137;struct Cyc_List_List*_tmp138;struct _tuple0*_tmp139;struct _dyneither_ptr _tmp13A;struct _dyneither_ptr _tmp13B;struct _dyneither_ptr _tmp13C;enum Cyc_Absyn_Sign _tmp13D;long long _tmp13E;int _tmp13F;int _tmp140;int _tmp141;enum Cyc_Absyn_Sign _tmp142;short _tmp143;struct _dyneither_ptr _tmp144;enum Cyc_Absyn_Sign _tmp145;char _tmp146;switch(*((int*)_tmpF1)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).String_c).tag){case 2: _LL12D: _tmp145=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Char_c).val).f1;_tmp146=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Char_c).val).f2;_LL12E:
# 656
 Cyc_Absyndump_dump_char((int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp146));Cyc_Absyndump_dump_char((int)'\'');
goto _LL12C;case 3: _LL12F: _tmp144=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Wchar_c).val;_LL130:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp149;_tmp149.tag=0;_tmp149.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp144);({void*_tmp147[1]={& _tmp149};Cyc_aprintf(({const char*_tmp148="L'%s'";_tag_dyneither(_tmp148,sizeof(char),6);}),_tag_dyneither(_tmp147,sizeof(void*),1));});}));goto _LL12C;case 4: _LL131: _tmp142=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Short_c).val).f1;_tmp143=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Short_c).val).f2;_LL132:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp14C;_tmp14C.tag=1;_tmp14C.f1=(unsigned long)((int)_tmp143);({void*_tmp14A[1]={& _tmp14C};Cyc_aprintf(({const char*_tmp14B="%d";_tag_dyneither(_tmp14B,sizeof(char),3);}),_tag_dyneither(_tmp14A,sizeof(void*),1));});}));goto _LL12C;case 5: switch((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Int_c).val).f1){case Cyc_Absyn_None: _LL133: _tmp141=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Int_c).val).f2;_LL134:
 _tmp140=_tmp141;goto _LL136;case Cyc_Absyn_Signed: _LL135: _tmp140=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Int_c).val).f2;_LL136:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp14F;_tmp14F.tag=1;_tmp14F.f1=(unsigned long)_tmp140;({void*_tmp14D[1]={& _tmp14F};Cyc_aprintf(({const char*_tmp14E="%d";_tag_dyneither(_tmp14E,sizeof(char),3);}),_tag_dyneither(_tmp14D,sizeof(void*),1));});}));goto _LL12C;default: _LL137: _tmp13F=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Int_c).val).f2;_LL138:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp152;_tmp152.tag=1;_tmp152.f1=(unsigned int)_tmp13F;({void*_tmp150[1]={& _tmp152};Cyc_aprintf(({const char*_tmp151="%u";_tag_dyneither(_tmp151,sizeof(char),3);}),_tag_dyneither(_tmp150,sizeof(void*),1));});}));goto _LL12C;}case 6: _LL139: _tmp13D=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).LongLong_c).val).f1;_tmp13E=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).LongLong_c).val).f2;_LL13A:
# 665
 Cyc_Absyndump_dump(Cyc_Absynpp_longlong2string((unsigned long long)_tmp13E));goto _LL12C;case 7: _LL13B: _tmp13C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Float_c).val).f1;_LL13C:
 Cyc_Absyndump_dump(_tmp13C);goto _LL12C;case 1: _LL13D: _LL13E:
 Cyc_Absyndump_dump(({const char*_tmp153="NULL";_tag_dyneither(_tmp153,sizeof(char),5);}));goto _LL12C;case 8: _LL13F: _tmp13B=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).String_c).val;_LL140:
# 669
 Cyc_Absyndump_dump_char((int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp13B));Cyc_Absyndump_dump_char((int)'"');
goto _LL12C;default: _LL141: _tmp13A=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).Wstring_c).val;_LL142:
# 672
 Cyc_Absyndump_dump(({const char*_tmp154="L\"";_tag_dyneither(_tmp154,sizeof(char),3);}));Cyc_Absyndump_dump_nospace(_tmp13A);Cyc_Absyndump_dump_char((int)'"');
goto _LL12C;}case 1: _LL143: _tmp139=((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL144:
# 675
 Cyc_Absyndump_dumpqvar(_tmp139);goto _LL12C;case 2: _LL145: _tmp137=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp138=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL146: {
# 678
struct _dyneither_ptr _tmp155=Cyc_Absynpp_prim2str(_tmp137);
{int _tmp156=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp138);int _tmp157=_tmp156;switch(_tmp157){case 1: _LL198: _LL199:
# 681
 if(_tmp137 == Cyc_Absyn_Numelts){
Cyc_Absyndump_dump(({const char*_tmp158="numelts(";_tag_dyneither(_tmp158,sizeof(char),9);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp138))->hd);
Cyc_Absyndump_dump(({const char*_tmp159=")";_tag_dyneither(_tmp159,sizeof(char),2);}));}else{
# 686
Cyc_Absyndump_dump(_tmp155);
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp138))->hd);}
# 689
goto _LL197;case 2: _LL19A: _LL19B:
# 691
 Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp138))->hd);
Cyc_Absyndump_dump(_tmp155);
Cyc_Absyndump_dump_char((int)' ');
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp138->tl))->hd);
goto _LL197;default: _LL19C: _LL19D:
# 697
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp15A=_cycalloc(sizeof(*_tmp15A));_tmp15A[0]=({struct Cyc_Core_Failure_exn_struct _tmp15B;_tmp15B.tag=Cyc_Core_Failure;_tmp15B.f1=({const char*_tmp15C="Absyndump -- Bad number of arguments to primop";_tag_dyneither(_tmp15C,sizeof(char),47);});_tmp15B;});_tmp15A;}));}_LL197:;}
# 699
goto _LL12C;}case 3: _LL147: _tmp134=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp135=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_tmp136=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpF1)->f3;_LL148:
# 702
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp134);
if(_tmp135 != 0)
Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)_tmp135->v));
Cyc_Absyndump_dump_nospace(({const char*_tmp15D="=";_tag_dyneither(_tmp15D,sizeof(char),2);}));
Cyc_Absyndump_dumpexp_prec(myprec,_tmp136);
goto _LL12C;case 4: switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF1)->f2){case Cyc_Absyn_PreInc: _LL149: _tmp133=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL14A:
# 709
 Cyc_Absyndump_dump(({const char*_tmp15E="++";_tag_dyneither(_tmp15E,sizeof(char),3);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp133);goto _LL12C;case Cyc_Absyn_PreDec: _LL14B: _tmp132=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL14C:
 Cyc_Absyndump_dump(({const char*_tmp15F="--";_tag_dyneither(_tmp15F,sizeof(char),3);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp132);goto _LL12C;case Cyc_Absyn_PostInc: _LL14D: _tmp131=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL14E:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp131);Cyc_Absyndump_dump(({const char*_tmp160="++";_tag_dyneither(_tmp160,sizeof(char),3);}));goto _LL12C;default: _LL14F: _tmp130=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL150:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp130);Cyc_Absyndump_dump(({const char*_tmp161="--";_tag_dyneither(_tmp161,sizeof(char),3);}));goto _LL12C;}case 5: _LL151: _tmp12D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp12E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_tmp12F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF1)->f3;_LL152:
# 715
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp12D);
Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp12E);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,_tmp12F);
goto _LL12C;case 6: _LL153: _tmp12B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp12C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL154:
# 722
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp12B);Cyc_Absyndump_dump(({const char*_tmp162=" && ";_tag_dyneither(_tmp162,sizeof(char),5);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp12C);goto _LL12C;case 7: _LL155: _tmp129=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp12A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL156:
# 726
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp129);Cyc_Absyndump_dump(({const char*_tmp163=" || ";_tag_dyneither(_tmp163,sizeof(char),5);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp12A);goto _LL12C;case 8: _LL157: _tmp127=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp128=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL158:
# 732
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp127);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(myprec,_tmp128);
Cyc_Absyndump_dump_char((int)')');
goto _LL12C;case 9: _LL159: _tmp125=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp126=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL15A:
# 739
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp125);
Cyc_Absyndump_dump_nospace(({const char*_tmp164="(";_tag_dyneither(_tmp164,sizeof(char),2);}));{
# 745
int old_generate_line_directives=Cyc_Absyndump_generate_line_directives;
Cyc_Absyndump_generate_line_directives=
(old_generate_line_directives  && !(e->loc == 0)) && !(_tmp125->loc == 0);
Cyc_Absyndump_dumpexps_prec(20,_tmp126);
Cyc_Absyndump_dump_nospace(({const char*_tmp165=")";_tag_dyneither(_tmp165,sizeof(char),2);}));
Cyc_Absyndump_generate_line_directives=old_generate_line_directives;
goto _LL12C;};case 10: _LL15B: _tmp124=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL15C:
# 754
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dump(({const char*_tmp166="throw";_tag_dyneither(_tmp166,sizeof(char),6);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp124);goto _LL12C;case 11: _LL15D: _tmp123=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL15E:
# 757
 _tmp122=_tmp123;goto _LL160;case 12: _LL15F: _tmp122=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL160:
 Cyc_Absyndump_dumpexp_prec(inprec,_tmp122);goto _LL12C;case 13: _LL161: _tmp120=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp121=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL162:
# 761
 Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(_tmp120);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,_tmp121);
goto _LL12C;case 14: _LL163: _tmp11F=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL164:
# 765
 Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp11F);goto _LL12C;case 15: _LL165: _tmp11D=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp11E=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL166:
# 768
 Cyc_Absyndump_dump(({const char*_tmp167="new ";_tag_dyneither(_tmp167,sizeof(char),5);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp11E);goto _LL12C;case 16: _LL167: _tmp11C=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL168:
# 771
 Cyc_Absyndump_dump(({const char*_tmp168="sizeof(";_tag_dyneither(_tmp168,sizeof(char),8);}));Cyc_Absyndump_dumptyp(_tmp11C);Cyc_Absyndump_dump_char((int)')');goto _LL12C;case 17: _LL169: _tmp11B=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL16A:
# 774
 Cyc_Absyndump_dump(({const char*_tmp169="sizeof(";_tag_dyneither(_tmp169,sizeof(char),8);}));Cyc_Absyndump_dumpexp(_tmp11B);Cyc_Absyndump_dump_char((int)')');goto _LL12C;case 38: _LL16B: _tmp11A=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL16C:
# 777
 Cyc_Absyndump_dump(({const char*_tmp16A="valueof(";_tag_dyneither(_tmp16A,sizeof(char),9);}));Cyc_Absyndump_dumptyp(_tmp11A);Cyc_Absyndump_dump_char((int)')');goto _LL12C;case 39: _LL16D: _tmp118=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp119=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL16E:
# 780
 Cyc_Absyndump_dump(({const char*_tmp16B="__asm__";_tag_dyneither(_tmp16B,sizeof(char),8);}));
if(_tmp118)Cyc_Absyndump_dump(({const char*_tmp16C=" volatile ";_tag_dyneither(_tmp16C,sizeof(char),11);}));
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_nospace(_tmp119);
Cyc_Absyndump_dump_char((int)')');
goto _LL12C;case 37: _LL16F: _tmp116=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp117=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL170:
# 788
 Cyc_Absyndump_dump(({const char*_tmp16D="tagcheck(";_tag_dyneither(_tmp16D,sizeof(char),10);}));Cyc_Absyndump_dumpexp(_tmp116);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp117);
Cyc_Absyndump_dump_char((int)')');
goto _LL12C;case 18: _LL171: _tmp114=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp115=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL172:
# 793
 Cyc_Absyndump_dump(({const char*_tmp16E="offsetof(";_tag_dyneither(_tmp16E,sizeof(char),10);}));Cyc_Absyndump_dumptyp(_tmp114);Cyc_Absyndump_dump_char((int)',');
for(0;_tmp115 != 0;_tmp115=_tmp115->tl){
{void*_tmp16F=(void*)_tmp115->hd;void*_tmp170=_tmp16F;unsigned int _tmp171;struct _dyneither_ptr*_tmp172;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp170)->tag == 0){_LL19F: _tmp172=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp170)->f1;_LL1A0:
# 797
 Cyc_Absyndump_dump_nospace(*_tmp172);
goto _LL19E;}else{_LL1A1: _tmp171=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp170)->f1;_LL1A2:
# 800
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp175;_tmp175.tag=1;_tmp175.f1=(unsigned long)((int)_tmp171);({void*_tmp173[1]={& _tmp175};Cyc_aprintf(({const char*_tmp174="%d";_tag_dyneither(_tmp174,sizeof(char),3);}),_tag_dyneither(_tmp173,sizeof(void*),1));});}));
goto _LL19E;}_LL19E:;}
# 803
if(_tmp115->tl != 0)Cyc_Absyndump_dump_char((int)'.');}
# 805
Cyc_Absyndump_dump_char((int)')');
goto _LL12C;case 19: _LL173: _tmp113=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL174:
# 809
 Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp113);goto _LL12C;case 20: _LL175: _tmp111=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp112=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL176:
# 812
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp111);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp112);
goto _LL12C;case 21: _LL177: _tmp10F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp110=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL178:
# 816
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp10F);Cyc_Absyndump_dump_nospace(({const char*_tmp176="->";_tag_dyneither(_tmp176,sizeof(char),3);}));Cyc_Absyndump_dump_nospace(*_tmp110);
goto _LL12C;case 22: _LL179: _tmp10D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp10E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL17A:
# 820
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp10D);
Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp10E);Cyc_Absyndump_dump_char((int)']');goto _LL12C;case 23: _LL17B: _tmp10C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL17C:
# 824
 Cyc_Absyndump_dump(({const char*_tmp177="$(";_tag_dyneither(_tmp177,sizeof(char),3);}));Cyc_Absyndump_dumpexps_prec(20,_tmp10C);Cyc_Absyndump_dump_char((int)')');goto _LL12C;case 24: _LL17D: _tmp10A=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp10B=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL17E:
# 827
 Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp((*_tmp10A).f3);
Cyc_Absyndump_dump_char((int)')');
((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp10B,({const char*_tmp178="{";_tag_dyneither(_tmp178,sizeof(char),2);}),({const char*_tmp179="}";_tag_dyneither(_tmp179,sizeof(char),2);}),({const char*_tmp17A=",";_tag_dyneither(_tmp17A,sizeof(char),2);}));
goto _LL12C;case 25: _LL17F: _tmp109=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL180:
# 834
((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp109,({const char*_tmp17B="{";_tag_dyneither(_tmp17B,sizeof(char),2);}),({const char*_tmp17C="}";_tag_dyneither(_tmp17C,sizeof(char),2);}),({const char*_tmp17D=",";_tag_dyneither(_tmp17D,sizeof(char),2);}));
goto _LL12C;case 26: _LL181: _tmp106=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp107=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_tmp108=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpF1)->f3;_LL182:
# 838
 Cyc_Absyndump_dump(({const char*_tmp17E="{for";_tag_dyneither(_tmp17E,sizeof(char),5);}));Cyc_Absyndump_dump_str((*_tmp106->name).f2);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp107);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(_tmp108);Cyc_Absyndump_dump_char((int)'}');
goto _LL12C;case 27: _LL183: _tmp104=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp105=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL184:
# 843
 Cyc_Absyndump_dump(({const char*_tmp17F="{for x ";_tag_dyneither(_tmp17F,sizeof(char),8);}));Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp104);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumptyp(_tmp105);Cyc_Absyndump_dump_char((int)'}');
goto _LL12C;case 28: _LL185: _tmp101=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmp102=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_tmp103=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpF1)->f3;_LL186:
# 848
 Cyc_Absyndump_dumpqvar(_tmp101);
Cyc_Absyndump_dump_char((int)'{');
if(_tmp102 != 0)
Cyc_Absyndump_dumptps(_tmp102);
((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp103,({const char*_tmp180="";_tag_dyneither(_tmp180,sizeof(char),1);}),({const char*_tmp181="}";_tag_dyneither(_tmp181,sizeof(char),2);}),({const char*_tmp182=",";_tag_dyneither(_tmp182,sizeof(char),2);}));
goto _LL12C;case 29: _LL187: _tmp100=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL188:
# 856
((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp100,({const char*_tmp183="{";_tag_dyneither(_tmp183,sizeof(char),2);}),({const char*_tmp184="}";_tag_dyneither(_tmp184,sizeof(char),2);}),({const char*_tmp185=",";_tag_dyneither(_tmp185,sizeof(char),2);}));
goto _LL12C;case 30: _LL189: _tmpFE=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmpFF=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpF1)->f3;_LL18A:
# 860
 Cyc_Absyndump_dumpqvar(_tmpFF->name);
if(_tmpFE != 0)((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,_tmpFE,({const char*_tmp186="(";_tag_dyneither(_tmp186,sizeof(char),2);}),({const char*_tmp187=")";_tag_dyneither(_tmp187,sizeof(char),2);}),({const char*_tmp188=",";_tag_dyneither(_tmp188,sizeof(char),2);}));
goto _LL12C;case 31: _LL18B: _tmpFD=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL18C:
# 864
 Cyc_Absyndump_dumpqvar(_tmpFD);goto _LL12C;case 32: _LL18D: _tmpFC=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL18E:
 Cyc_Absyndump_dumpqvar(_tmpFC);goto _LL12C;case 33: _LL18F: _tmpF7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).is_calloc;_tmpF8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).rgn;_tmpF9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).elt_type;_tmpFA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).num_elts;_tmpFB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).inline_call;_LL190:
# 868
 Cyc_Absyndump_dumploc(_tmpFA->loc);
if(_tmpF7){
if(_tmpF8 != 0){
Cyc_Absyndump_dump(({const char*_tmp189="rcalloc(";_tag_dyneither(_tmp189,sizeof(char),9);}));
Cyc_Absyndump_dumpexp(_tmpF8);Cyc_Absyndump_dump(({const char*_tmp18A=",";_tag_dyneither(_tmp18A,sizeof(char),2);}));}else{
# 874
Cyc_Absyndump_dump(({const char*_tmp18B="calloc";_tag_dyneither(_tmp18B,sizeof(char),7);}));}
# 876
Cyc_Absyndump_dumpexp(_tmpFA);
Cyc_Absyndump_dump(({const char*_tmp18C=",";_tag_dyneither(_tmp18C,sizeof(char),2);}));
Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmpF9)),0));
Cyc_Absyndump_dump(({const char*_tmp18D=")";_tag_dyneither(_tmp18D,sizeof(char),2);}));}else{
# 881
if(_tmpF8 != 0){
if(_tmpFB)
Cyc_Absyndump_dump(({const char*_tmp18E="rmalloc_inline(";_tag_dyneither(_tmp18E,sizeof(char),16);}));else{
# 885
Cyc_Absyndump_dump(({const char*_tmp18F="rmalloc(";_tag_dyneither(_tmp18F,sizeof(char),9);}));}
Cyc_Absyndump_dumpexp(_tmpF8);Cyc_Absyndump_dump(({const char*_tmp190=",";_tag_dyneither(_tmp190,sizeof(char),2);}));}else{
# 888
Cyc_Absyndump_dump(({const char*_tmp191="malloc(";_tag_dyneither(_tmp191,sizeof(char),8);}));}
# 891
if(_tmpF9 != 0)
Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmpF9,0),_tmpFA,0));else{
# 894
Cyc_Absyndump_dumpexp(_tmpFA);}
Cyc_Absyndump_dump(({const char*_tmp192=")";_tag_dyneither(_tmp192,sizeof(char),2);}));}
# 897
goto _LL12C;case 34: _LL191: _tmpF5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmpF6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL192:
# 900
 Cyc_Absyndump_dumpexp_prec(myprec,_tmpF5);
Cyc_Absyndump_dump_nospace(({const char*_tmp193=":=:";_tag_dyneither(_tmp193,sizeof(char),4);}));
Cyc_Absyndump_dumpexp_prec(myprec,_tmpF6);
goto _LL12C;case 35: _LL193: _tmpF3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmpF4=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL194:
# 907
((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmpF4,({const char*_tmp194="{";_tag_dyneither(_tmp194,sizeof(char),2);}),({const char*_tmp195="}";_tag_dyneither(_tmp195,sizeof(char),2);}),({const char*_tmp196=",";_tag_dyneither(_tmp196,sizeof(char),2);}));
goto _LL12C;default: _LL195: _tmpF2=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL196:
# 911
 Cyc_Absyndump_dump_nospace(({const char*_tmp197="({";_tag_dyneither(_tmp197,sizeof(char),3);}));
Cyc_Absyndump_dumpstmt(_tmpF2,1);
Cyc_Absyndump_dump_nospace(({const char*_tmp198="})";_tag_dyneither(_tmp198,sizeof(char),3);}));
goto _LL12C;}_LL12C:;}
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
if(_tmp199->where_clause == 0  && (_tmp199->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
Cyc_Absyndump_dump(({const char*_tmp19A="default:";_tag_dyneither(_tmp19A,sizeof(char),9);}));else{
# 930
Cyc_Absyndump_dump(({const char*_tmp19B="case";_tag_dyneither(_tmp19B,sizeof(char),5);}));
Cyc_Absyndump_dumppat(_tmp199->pattern);
if(_tmp199->where_clause != 0){
Cyc_Absyndump_dump(({const char*_tmp19C="&&";_tag_dyneither(_tmp19C,sizeof(char),3);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp199->where_clause));}
# 936
Cyc_Absyndump_dump_nospace(({const char*_tmp19D=":";_tag_dyneither(_tmp19D,sizeof(char),2);}));}
# 938
Cyc_Absyndump_dumpstmt(_tmp199->body,0);}}
# 942
void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt){
Cyc_Absyndump_dumploc(s->loc);{
void*_tmp19E=s->r;void*_tmp19F=_tmp19E;struct Cyc_Absyn_Exp*_tmp1A0;struct Cyc_Absyn_Stmt*_tmp1A1;struct Cyc_List_List*_tmp1A2;struct Cyc_List_List*_tmp1A3;struct Cyc_Absyn_Stmt*_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A5;struct _dyneither_ptr*_tmp1A6;struct Cyc_Absyn_Stmt*_tmp1A7;struct Cyc_Absyn_Decl*_tmp1A8;struct Cyc_Absyn_Stmt*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_List_List*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AE;struct Cyc_Absyn_Stmt*_tmp1AF;struct _dyneither_ptr*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1B1;struct Cyc_Absyn_Stmt*_tmp1B2;struct Cyc_Absyn_Exp*_tmp1B3;struct Cyc_Absyn_Stmt*_tmp1B4;struct Cyc_Absyn_Stmt*_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B6;struct Cyc_Absyn_Stmt*_tmp1B7;struct Cyc_Absyn_Stmt*_tmp1B8;struct Cyc_Absyn_Exp*_tmp1B9;switch(*((int*)_tmp19F)){case 0: _LL1A4: _LL1A5:
 Cyc_Absyndump_dump_semi();goto _LL1A3;case 1: _LL1A6: _tmp1B9=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_LL1A7:
 Cyc_Absyndump_dumpexp(_tmp1B9);Cyc_Absyndump_dump_semi();goto _LL1A3;case 2: _LL1A8: _tmp1B7=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1B8=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL1A9:
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
goto _LL1A3;case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1 == 0){_LL1AA: _LL1AB:
 Cyc_Absyndump_dump(({const char*_tmp1BA="return;";_tag_dyneither(_tmp1BA,sizeof(char),8);}));goto _LL1A3;}else{_LL1AC: _tmp1B6=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_LL1AD:
 Cyc_Absyndump_dump(({const char*_tmp1BB="return";_tag_dyneither(_tmp1BB,sizeof(char),7);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp1B6));Cyc_Absyndump_dump_semi();goto _LL1A3;}case 4: _LL1AE: _tmp1B3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1B4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_tmp1B5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19F)->f3;_LL1AF:
# 966
 Cyc_Absyndump_dump(({const char*_tmp1BC="if(";_tag_dyneither(_tmp1BC,sizeof(char),4);}));Cyc_Absyndump_dumpexp(_tmp1B3);
{void*_tmp1BD=_tmp1B4->r;void*_tmp1BE=_tmp1BD;switch(*((int*)_tmp1BE)){case 2: _LL1CB: _LL1CC:
 goto _LL1CE;case 12: _LL1CD: _LL1CE:
 goto _LL1D0;case 4: _LL1CF: _LL1D0:
 goto _LL1D2;case 13: _LL1D1: _LL1D2:
 Cyc_Absyndump_dump_nospace(({const char*_tmp1BF="){";_tag_dyneither(_tmp1BF,sizeof(char),3);}));Cyc_Absyndump_dumpstmt(_tmp1B4,0);Cyc_Absyndump_dump_char((int)'}');goto _LL1CA;default: _LL1D3: _LL1D4:
 Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(_tmp1B4,0);}_LL1CA:;}
# 974
{void*_tmp1C0=_tmp1B5->r;void*_tmp1C1=_tmp1C0;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp1C1)->tag == 0){_LL1D6: _LL1D7:
 goto _LL1D5;}else{_LL1D8: _LL1D9:
 Cyc_Absyndump_dump(({const char*_tmp1C2="else{";_tag_dyneither(_tmp1C2,sizeof(char),6);}));Cyc_Absyndump_dumpstmt(_tmp1B5,0);Cyc_Absyndump_dump_char((int)'}');goto _LL1D5;}_LL1D5:;}
# 978
goto _LL1A3;case 5: _LL1B0: _tmp1B1=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1).f1;_tmp1B2=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL1B1:
# 980
 Cyc_Absyndump_dump(({const char*_tmp1C3="while(";_tag_dyneither(_tmp1C3,sizeof(char),7);}));Cyc_Absyndump_dumpexp(_tmp1B1);Cyc_Absyndump_dump_nospace(({const char*_tmp1C4="){";_tag_dyneither(_tmp1C4,sizeof(char),3);}));
Cyc_Absyndump_dumpstmt(_tmp1B2,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL1A3;case 6: _LL1B2: _LL1B3:
 Cyc_Absyndump_dump(({const char*_tmp1C5="break;";_tag_dyneither(_tmp1C5,sizeof(char),7);}));goto _LL1A3;case 7: _LL1B4: _LL1B5:
 Cyc_Absyndump_dump(({const char*_tmp1C6="continue;";_tag_dyneither(_tmp1C6,sizeof(char),10);}));goto _LL1A3;case 8: _LL1B6: _tmp1B0=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_LL1B7:
 Cyc_Absyndump_dump(({const char*_tmp1C7="goto";_tag_dyneither(_tmp1C7,sizeof(char),5);}));Cyc_Absyndump_dump_str(_tmp1B0);Cyc_Absyndump_dump_semi();goto _LL1A3;case 9: _LL1B8: _tmp1AC=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1AD=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2).f1;_tmp1AE=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19F)->f3).f1;_tmp1AF=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19F)->f4;_LL1B9:
# 989
 Cyc_Absyndump_dump(({const char*_tmp1C8="for(";_tag_dyneither(_tmp1C8,sizeof(char),5);}));Cyc_Absyndump_dumpexp(_tmp1AC);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp1AD);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp1AE);
Cyc_Absyndump_dump_nospace(({const char*_tmp1C9="){";_tag_dyneither(_tmp1C9,sizeof(char),3);}));Cyc_Absyndump_dumpstmt(_tmp1AF,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL1A3;case 10: _LL1BA: _tmp1AA=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1AB=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL1BB:
# 993
 Cyc_Absyndump_dump(({const char*_tmp1CA="switch(";_tag_dyneither(_tmp1CA,sizeof(char),8);}));Cyc_Absyndump_dumpexp(_tmp1AA);Cyc_Absyndump_dump_nospace(({const char*_tmp1CB="){";_tag_dyneither(_tmp1CB,sizeof(char),3);}));
Cyc_Absyndump_dumpswitchclauses(_tmp1AB);
Cyc_Absyndump_dump_char((int)'}');
goto _LL1A3;case 12: _LL1BC: _tmp1A8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1A9=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL1BD:
# 998
 Cyc_Absyndump_dumpdecl(_tmp1A8);Cyc_Absyndump_dumpstmt(_tmp1A9,expstmt);goto _LL1A3;case 13: _LL1BE: _tmp1A6=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1A7=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL1BF:
# 1004
 if(Cyc_Absynpp_is_declaration(_tmp1A7)){
Cyc_Absyndump_dump_str(_tmp1A6);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1CC=": ({";_tag_dyneither(_tmp1CC,sizeof(char),5);}));else{Cyc_Absyndump_dump_nospace(({const char*_tmp1CD=": {";_tag_dyneither(_tmp1CD,sizeof(char),4);}));}
Cyc_Absyndump_dumpstmt(_tmp1A7,expstmt);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1CE="});}";_tag_dyneither(_tmp1CE,sizeof(char),5);}));else{Cyc_Absyndump_dump_char((int)'}');}}else{
# 1010
Cyc_Absyndump_dump_str(_tmp1A6);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(_tmp1A7,expstmt);}
# 1012
goto _LL1A3;case 14: _LL1C0: _tmp1A4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1A5=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2).f1;_LL1C1:
# 1015
 Cyc_Absyndump_dump(({const char*_tmp1CF="do{";_tag_dyneither(_tmp1CF,sizeof(char),4);}));Cyc_Absyndump_dumpstmt(_tmp1A4,0);
Cyc_Absyndump_dump_nospace(({const char*_tmp1D0="}while(";_tag_dyneither(_tmp1D0,sizeof(char),8);}));Cyc_Absyndump_dumpexp(_tmp1A5);Cyc_Absyndump_dump_nospace(({const char*_tmp1D1=");";_tag_dyneither(_tmp1D1,sizeof(char),3);}));
goto _LL1A3;case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1 == 0){_LL1C2: _LL1C3:
# 1019
 Cyc_Absyndump_dump(({const char*_tmp1D2="fallthru;";_tag_dyneither(_tmp1D2,sizeof(char),10);}));goto _LL1A3;}else{_LL1C4: _tmp1A3=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_LL1C5:
# 1021
 Cyc_Absyndump_dump(({const char*_tmp1D3="fallthru(";_tag_dyneither(_tmp1D3,sizeof(char),10);}));Cyc_Absyndump_dumpexps_prec(20,_tmp1A3);Cyc_Absyndump_dump_nospace(({const char*_tmp1D4=");";_tag_dyneither(_tmp1D4,sizeof(char),3);}));goto _LL1A3;}case 15: _LL1C6: _tmp1A1=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1A2=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL1C7:
# 1024
 Cyc_Absyndump_dump(({const char*_tmp1D5="try";_tag_dyneither(_tmp1D5,sizeof(char),4);}));Cyc_Absyndump_dumpstmt(_tmp1A1,0);
Cyc_Absyndump_dump(({const char*_tmp1D6="catch{";_tag_dyneither(_tmp1D6,sizeof(char),7);}));
Cyc_Absyndump_dumpswitchclauses(_tmp1A2);Cyc_Absyndump_dump_char((int)'}');
goto _LL1A3;default: _LL1C8: _tmp1A0=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_LL1C9:
# 1030
 Cyc_Absyndump_dump(({const char*_tmp1D7="reset_region(";_tag_dyneither(_tmp1D7,sizeof(char),14);}));Cyc_Absyndump_dumpexp(_tmp1A0);Cyc_Absyndump_dump(({const char*_tmp1D8=");";_tag_dyneither(_tmp1D8,sizeof(char),3);}));
goto _LL1A3;}_LL1A3:;};}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 1036
void Cyc_Absyndump_dumpdp(struct _tuple18*dp){
((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*dp).f1,({const char*_tmp1D9="";_tag_dyneither(_tmp1D9,sizeof(char),1);}),({const char*_tmp1DA="=";_tag_dyneither(_tmp1DA,sizeof(char),2);}),({const char*_tmp1DB="=";_tag_dyneither(_tmp1DB,sizeof(char),2);}));
Cyc_Absyndump_dumppat((*dp).f2);}
# 1041
void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){
void*_tmp1DC=p->r;void*_tmp1DD=_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DE;struct Cyc_Absyn_Enumfield*_tmp1DF;struct Cyc_Absyn_Enumfield*_tmp1E0;struct Cyc_Absyn_Datatypefield*_tmp1E1;struct Cyc_List_List*_tmp1E2;int _tmp1E3;struct Cyc_List_List*_tmp1E4;struct Cyc_List_List*_tmp1E5;int _tmp1E6;union Cyc_Absyn_AggrInfoU _tmp1E7;struct Cyc_List_List*_tmp1E8;struct Cyc_List_List*_tmp1E9;int _tmp1EA;struct _tuple0*_tmp1EB;struct Cyc_List_List*_tmp1EC;int _tmp1ED;struct _tuple0*_tmp1EE;struct Cyc_Absyn_Tvar*_tmp1EF;struct Cyc_Absyn_Vardecl*_tmp1F0;struct Cyc_Absyn_Vardecl*_tmp1F1;struct Cyc_Absyn_Pat*_tmp1F2;struct Cyc_Absyn_Vardecl*_tmp1F3;struct Cyc_Absyn_Pat*_tmp1F4;struct Cyc_List_List*_tmp1F5;int _tmp1F6;struct Cyc_Absyn_Tvar*_tmp1F7;struct Cyc_Absyn_Vardecl*_tmp1F8;struct Cyc_Absyn_Vardecl*_tmp1F9;struct Cyc_Absyn_Pat*_tmp1FA;struct Cyc_Absyn_Vardecl*_tmp1FB;struct _dyneither_ptr _tmp1FC;char _tmp1FD;int _tmp1FE;int _tmp1FF;int _tmp200;switch(*((int*)_tmp1DD)){case 0: _LL1DB: _LL1DC:
 Cyc_Absyndump_dump_char((int)'_');goto _LL1DA;case 9: _LL1DD: _LL1DE:
 Cyc_Absyndump_dump(({const char*_tmp201="NULL";_tag_dyneither(_tmp201,sizeof(char),5);}));goto _LL1DA;case 10: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1){case Cyc_Absyn_None: _LL1DF: _tmp200=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_LL1E0:
 _tmp1FF=_tmp200;goto _LL1E2;case Cyc_Absyn_Signed: _LL1E1: _tmp1FF=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_LL1E2:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp204;_tmp204.tag=1;_tmp204.f1=(unsigned long)_tmp1FF;({void*_tmp202[1]={& _tmp204};Cyc_aprintf(({const char*_tmp203="%d";_tag_dyneither(_tmp203,sizeof(char),3);}),_tag_dyneither(_tmp202,sizeof(void*),1));});}));goto _LL1DA;default: _LL1E3: _tmp1FE=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_LL1E4:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp207;_tmp207.tag=1;_tmp207.f1=(unsigned int)_tmp1FE;({void*_tmp205[1]={& _tmp207};Cyc_aprintf(({const char*_tmp206="%u";_tag_dyneither(_tmp206,sizeof(char),3);}),_tag_dyneither(_tmp205,sizeof(void*),1));});}));goto _LL1DA;}case 11: _LL1E5: _tmp1FD=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_LL1E6:
# 1049
 Cyc_Absyndump_dump(({const char*_tmp208="'";_tag_dyneither(_tmp208,sizeof(char),2);}));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp1FD));Cyc_Absyndump_dump_nospace(({const char*_tmp209="'";_tag_dyneither(_tmp209,sizeof(char),2);}));goto _LL1DA;case 12: _LL1E7: _tmp1FC=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_LL1E8:
 Cyc_Absyndump_dump(_tmp1FC);goto _LL1DA;case 1: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2)->r)->tag == 0){_LL1E9: _tmp1FB=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_LL1EA:
 Cyc_Absyndump_dumpqvar(_tmp1FB->name);goto _LL1DA;}else{_LL1EB: _tmp1F9=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_tmp1FA=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_LL1EC:
 Cyc_Absyndump_dumpqvar(_tmp1F9->name);Cyc_Absyndump_dump(({const char*_tmp20A=" as ";_tag_dyneither(_tmp20A,sizeof(char),5);}));Cyc_Absyndump_dumppat(_tmp1FA);goto _LL1DA;}case 2: _LL1ED: _tmp1F7=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_tmp1F8=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_LL1EE:
# 1054
 Cyc_Absyndump_dump(({const char*_tmp20B="alias";_tag_dyneither(_tmp20B,sizeof(char),6);}));
Cyc_Absyndump_dump(({const char*_tmp20C="<";_tag_dyneither(_tmp20C,sizeof(char),2);}));Cyc_Absyndump_dumptvar(_tmp1F7);Cyc_Absyndump_dump(({const char*_tmp20D=">";_tag_dyneither(_tmp20D,sizeof(char),2);}));
Cyc_Absyndump_dumpvardecl(_tmp1F8,p->loc,0);
goto _LL1DA;case 5: _LL1EF: _tmp1F5=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_tmp1F6=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_LL1F0: {
# 1059
struct _dyneither_ptr term=_tmp1F6?({const char*_tmp210=", ...)";_tag_dyneither(_tmp210,sizeof(char),7);}):({const char*_tmp211=")";_tag_dyneither(_tmp211,sizeof(char),2);});
((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp1F5,({const char*_tmp20E="$(";_tag_dyneither(_tmp20E,sizeof(char),3);}),term,({const char*_tmp20F=",";_tag_dyneither(_tmp20F,sizeof(char),2);}));goto _LL1DA;}case 6: _LL1F1: _tmp1F4=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_LL1F2:
 Cyc_Absyndump_dump(({const char*_tmp212="&";_tag_dyneither(_tmp212,sizeof(char),2);}));Cyc_Absyndump_dumppat(_tmp1F4);goto _LL1DA;case 3: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2)->r)->tag == 0){_LL1F3: _tmp1F3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_LL1F4:
# 1063
 Cyc_Absyndump_dump(({const char*_tmp213="*";_tag_dyneither(_tmp213,sizeof(char),2);}));Cyc_Absyndump_dumpqvar(_tmp1F3->name);goto _LL1DA;}else{_LL1F5: _tmp1F1=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_tmp1F2=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_LL1F6:
# 1065
 Cyc_Absyndump_dump(({const char*_tmp214="*";_tag_dyneither(_tmp214,sizeof(char),2);}));Cyc_Absyndump_dumpqvar(_tmp1F1->name);Cyc_Absyndump_dump(({const char*_tmp215=" as ";_tag_dyneither(_tmp215,sizeof(char),5);}));Cyc_Absyndump_dumppat(_tmp1F2);goto _LL1DA;}case 4: _LL1F7: _tmp1EF=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_tmp1F0=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_LL1F8:
# 1067
 Cyc_Absyndump_dumpqvar(_tmp1F0->name);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptvar(_tmp1EF);Cyc_Absyndump_dump_char((int)'>');goto _LL1DA;case 15: _LL1F9: _tmp1EE=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_LL1FA:
 Cyc_Absyndump_dumpqvar(_tmp1EE);goto _LL1DA;case 16: _LL1FB: _tmp1EB=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_tmp1EC=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_tmp1ED=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1DD)->f3;_LL1FC: {
# 1070
struct _dyneither_ptr term=_tmp1ED?({const char*_tmp218=", ...)";_tag_dyneither(_tmp218,sizeof(char),7);}):({const char*_tmp219=")";_tag_dyneither(_tmp219,sizeof(char),2);});
Cyc_Absyndump_dumpqvar(_tmp1EB);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp1EC,({const char*_tmp216="(";_tag_dyneither(_tmp216,sizeof(char),2);}),term,({const char*_tmp217=",";_tag_dyneither(_tmp217,sizeof(char),2);}));goto _LL1DA;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1 != 0){_LL1FD: _tmp1E7=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1)->aggr_info;_tmp1E8=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_tmp1E9=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DD)->f3;_tmp1EA=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DD)->f4;_LL1FE: {
# 1073
struct _tuple10 _tmp21A=Cyc_Absyn_aggr_kinded_name(_tmp1E7);struct _tuple10 _tmp21B=_tmp21A;struct _tuple0*_tmp21C;_LL20A: _tmp21C=_tmp21B.f2;_LL20B:;{
struct _dyneither_ptr term=_tmp1EA?({const char*_tmp222=", ...)";_tag_dyneither(_tmp222,sizeof(char),7);}):({const char*_tmp223=")";_tag_dyneither(_tmp223,sizeof(char),2);});
Cyc_Absyndump_dumpqvar(_tmp21C);Cyc_Absyndump_dump_char((int)'{');
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp1E8,({const char*_tmp21D="[";_tag_dyneither(_tmp21D,sizeof(char),2);}),({const char*_tmp21E="]";_tag_dyneither(_tmp21E,sizeof(char),2);}),({const char*_tmp21F=",";_tag_dyneither(_tmp21F,sizeof(char),2);}));
((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp1E9,({const char*_tmp220="";_tag_dyneither(_tmp220,sizeof(char),1);}),term,({const char*_tmp221=",";_tag_dyneither(_tmp221,sizeof(char),2);}));
goto _LL1DA;};}}else{_LL1FF: _tmp1E4=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_tmp1E5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DD)->f3;_tmp1E6=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DD)->f4;_LL200: {
# 1080
struct _dyneither_ptr term=_tmp1E6?({const char*_tmp229=", ...)";_tag_dyneither(_tmp229,sizeof(char),7);}):({const char*_tmp22A=")";_tag_dyneither(_tmp22A,sizeof(char),2);});
Cyc_Absyndump_dump_char((int)'{');
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp1E4,({const char*_tmp224="[";_tag_dyneither(_tmp224,sizeof(char),2);}),({const char*_tmp225="]";_tag_dyneither(_tmp225,sizeof(char),2);}),({const char*_tmp226=",";_tag_dyneither(_tmp226,sizeof(char),2);}));
((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp1E5,({const char*_tmp227="";_tag_dyneither(_tmp227,sizeof(char),1);}),term,({const char*_tmp228=",";_tag_dyneither(_tmp228,sizeof(char),2);}));
goto _LL1DA;}}case 8: _LL201: _tmp1E1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_tmp1E2=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1DD)->f3;_tmp1E3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1DD)->f4;_LL202: {
# 1086
struct _dyneither_ptr term=_tmp1E3?({const char*_tmp22D=", ...)";_tag_dyneither(_tmp22D,sizeof(char),7);}):({const char*_tmp22E=")";_tag_dyneither(_tmp22E,sizeof(char),2);});
Cyc_Absyndump_dumpqvar(_tmp1E1->name);
if(_tmp1E2 != 0)((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp1E2,({const char*_tmp22B="(";_tag_dyneither(_tmp22B,sizeof(char),2);}),term,({const char*_tmp22C=",";_tag_dyneither(_tmp22C,sizeof(char),2);}));
goto _LL1DA;}case 13: _LL203: _tmp1E0=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_LL204:
 _tmp1DF=_tmp1E0;goto _LL206;case 14: _LL205: _tmp1DF=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1DD)->f2;_LL206:
 Cyc_Absyndump_dumpqvar(_tmp1DF->name);goto _LL1DA;default: _LL207: _tmp1DE=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1DD)->f1;_LL208:
 Cyc_Absyndump_dumpexp(_tmp1DE);goto _LL1DA;}_LL1DA:;}
# 1096
void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->typs != 0)
Cyc_Absyndump_dumpargs(ef->typs);}
# 1101
void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){
((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,fields,({const char*_tmp22F=",";_tag_dyneither(_tmp22F,sizeof(char),2);}));}
# 1104
void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->tag != 0){
Cyc_Absyndump_dump(({const char*_tmp230=" = ";_tag_dyneither(_tmp230,sizeof(char),4);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
# 1111
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){
((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,fields,({const char*_tmp231=",";_tag_dyneither(_tmp231,sizeof(char),2);}));}
# 1115
void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp232=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp233=_tmp232;struct _dyneither_ptr*_tmp234;struct Cyc_Absyn_Tqual _tmp235;void*_tmp236;struct Cyc_Absyn_Exp*_tmp237;struct Cyc_List_List*_tmp238;struct Cyc_Absyn_Exp*_tmp239;_LL20D: _tmp234=_tmp233->name;_tmp235=_tmp233->tq;_tmp236=_tmp233->type;_tmp237=_tmp233->width;_tmp238=_tmp233->attributes;_tmp239=_tmp233->requires_clause;_LL20E:;
# 1119
{enum Cyc_Cyclone_C_Compilers _tmp23A=Cyc_Cyclone_c_compiler;if(_tmp23A == Cyc_Cyclone_Gcc_c){_LL210: _LL211:
# 1121
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp235,_tmp236,Cyc_Absyndump_dump_str,_tmp234);
Cyc_Absyndump_dumpatts(_tmp238);
goto _LL20F;}else{_LL212: _LL213:
# 1125
 Cyc_Absyndump_dumpatts(_tmp238);
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp235,_tmp236,Cyc_Absyndump_dump_str,_tmp234);
goto _LL20F;}_LL20F:;}
# 1129
if((unsigned int)_tmp239){
Cyc_Absyndump_dump(({const char*_tmp23B="@requires ";_tag_dyneither(_tmp23B,sizeof(char),11);}));
Cyc_Absyndump_dumpexp(_tmp239);}
# 1133
if(_tmp237 != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp(_tmp237);}
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
{void*_tmp23C=(*pr).f1;void*_tmp23D=_tmp23C;switch(*((int*)_tmp23D)){case 11: _LL215: _LL216:
 goto _LL214;case 1: _LL217: _LL218:
 Cyc_Absyndump_dump(({const char*_tmp23E="_stdcall";_tag_dyneither(_tmp23E,sizeof(char),9);}));goto _LL214;case 2: _LL219: _LL21A:
 Cyc_Absyndump_dump(({const char*_tmp23F="_cdecl";_tag_dyneither(_tmp23F,sizeof(char),7);}));goto _LL214;case 3: _LL21B: _LL21C:
 Cyc_Absyndump_dump(({const char*_tmp240="_fastcall";_tag_dyneither(_tmp240,sizeof(char),10);}));goto _LL214;default: _LL21D: _LL21E:
 goto _LL214;}_LL214:;}
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
void*_tmp241=t;void*_tmp242;void*_tmp243;switch(*((int*)_tmp241)){case 1: _LL220: _tmp243=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp241)->f2;if(_tmp243 != 0){_LL221:
 return Cyc_Absyndump_is_char_ptr(_tmp243);}else{goto _LL224;}case 5: _LL222: _tmp242=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp241)->f1).elt_typ;_LL223:
# 1177
 L: {
void*_tmp244=_tmp242;void*_tmp245;void*_tmp246;switch(*((int*)_tmp244)){case 1: _LL227: _tmp246=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp244)->f2;if(_tmp246 != 0){_LL228:
 _tmp242=_tmp246;goto L;}else{goto _LL22D;}case 17: _LL229: _tmp245=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp244)->f4;if(_tmp245 != 0){_LL22A:
 _tmp242=_tmp245;goto L;}else{goto _LL22D;}case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp244)->f2 == Cyc_Absyn_Char_sz){_LL22B: _LL22C:
 return 1;}else{goto _LL22D;}default: _LL22D: _LL22E:
 return 0;}_LL226:;}default: _LL224: _LL225:
# 1184
 return 0;}_LL21F:;}
# 1188
void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,unsigned int loc,int do_semi){
struct Cyc_Absyn_Vardecl*_tmp247=vd;enum Cyc_Absyn_Scope _tmp248;struct _tuple0*_tmp249;struct Cyc_Absyn_Tqual _tmp24A;void*_tmp24B;struct Cyc_Absyn_Exp*_tmp24C;struct Cyc_List_List*_tmp24D;_LL230: _tmp248=_tmp247->sc;_tmp249=_tmp247->name;_tmp24A=_tmp247->tq;_tmp24B=_tmp247->type;_tmp24C=_tmp247->initializer;_tmp24D=_tmp247->attributes;_LL231:;
{enum Cyc_Cyclone_C_Compilers _tmp24E=Cyc_Cyclone_c_compiler;if(_tmp24E == Cyc_Cyclone_Gcc_c){_LL233: _LL234:
# 1193
 if(_tmp248 == Cyc_Absyn_Extern  && Cyc_Absyndump_qvar_to_Cids){
void*_tmp24F=Cyc_Tcutil_compress(_tmp24B);void*_tmp250=_tmp24F;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp250)->tag == 9){_LL238: _LL239:
 goto _LL237;}else{_LL23A: _LL23B:
 Cyc_Absyndump_dumpscope(_tmp248);}_LL237:;}else{
# 1199
Cyc_Absyndump_dumpscope(_tmp248);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp24A,_tmp24B,Cyc_Absyndump_dumpqvar,_tmp249);
Cyc_Absyndump_dumpatts(_tmp24D);
goto _LL232;}else{_LL235: _LL236:
# 1204
 Cyc_Absyndump_dumpatts(_tmp24D);
Cyc_Absyndump_dumpscope(_tmp248);{
struct _RegionHandle _tmp251=_new_region("temp");struct _RegionHandle*temp=& _tmp251;_push_region(temp);{
int is_cp=Cyc_Absyndump_is_char_ptr(_tmp24B);
struct _tuple11 _tmp252=Cyc_Absynpp_to_tms(temp,_tmp24A,_tmp24B);struct _tuple11 _tmp253=_tmp252;struct Cyc_Absyn_Tqual _tmp254;void*_tmp255;struct Cyc_List_List*_tmp256;_LL23D: _tmp254=_tmp253.f1;_tmp255=_tmp253.f2;_tmp256=_tmp253.f3;_LL23E:;{
# 1210
void*call_conv=(void*)& Cyc_Absyn_Unused_att_val;
{struct Cyc_List_List*tms2=_tmp256;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp257=(void*)tms2->hd;void*_tmp258=_tmp257;struct Cyc_List_List*_tmp259;if(((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp258)->tag == 5){_LL240: _tmp259=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp258)->f2;_LL241:
# 1214
 for(0;_tmp259 != 0;_tmp259=_tmp259->tl){
void*_tmp25A=(void*)_tmp259->hd;void*_tmp25B=_tmp25A;switch(*((int*)_tmp25B)){case 1: _LL245: _LL246:
 call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;goto _LL244;case 2: _LL247: _LL248:
 call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;goto _LL244;case 3: _LL249: _LL24A:
 call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;goto _LL244;default: _LL24B: _LL24C:
 goto _LL244;}_LL244:;}
# 1221
goto _LL23F;}else{_LL242: _LL243:
 goto _LL23F;}_LL23F:;}}
# 1224
Cyc_Absyndump_dumptq(_tmp254);
Cyc_Absyndump_dumpntyp(_tmp255);{
struct _tuple19 _tmp25C=({struct _tuple19 _tmp25D;_tmp25D.f1=call_conv;_tmp25D.f2=_tmp249;_tmp25D;});
((void(*)(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(struct _tuple19*),struct _tuple19*a))Cyc_Absyndump_dumptms)(is_cp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp256),Cyc_Absyndump_dump_callconv_qvar,& _tmp25C);};};}
# 1229
_npop_handler(0);goto _LL232;
# 1206
;_pop_region(temp);};}_LL232:;}
# 1232
if(_tmp24C != 0){
Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp(_tmp24C);}
# 1236
if(do_semi)Cyc_Absyndump_dump_semi();}
# 1239
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*ad){
Cyc_Absyndump_dumpscope(ad->sc);
if(ad->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
Cyc_Absyndump_dump(({const char*_tmp25E="@tagged ";_tag_dyneither(_tmp25E,sizeof(char),9);}));
Cyc_Absyndump_dumpaggr_kind(ad->kind);
Cyc_Absyndump_dumpqvar(ad->name);
Cyc_Absyndump_dumpkindedtvars(ad->tvs);
if(ad->impl == 0)return;else{
# 1248
Cyc_Absyndump_dump_char((int)'{');
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,({const char*_tmp25F="<";_tag_dyneither(_tmp25F,sizeof(char),2);}),({const char*_tmp260=">";_tag_dyneither(_tmp260,sizeof(char),2);}),({const char*_tmp261=",";_tag_dyneither(_tmp261,sizeof(char),2);}));
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);}
# 1255
Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
Cyc_Absyndump_dump(({const char*_tmp262="}";_tag_dyneither(_tmp262,sizeof(char),2);}));
Cyc_Absyndump_dumpatts(ad->attributes);}}
# 1260
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp263=ed;enum Cyc_Absyn_Scope _tmp264;struct _tuple0*_tmp265;struct Cyc_Core_Opt*_tmp266;_LL24E: _tmp264=_tmp263->sc;_tmp265=_tmp263->name;_tmp266=_tmp263->fields;_LL24F:;
Cyc_Absyndump_dumpscope(_tmp264);
Cyc_Absyndump_dump(({const char*_tmp267="enum ";_tag_dyneither(_tmp267,sizeof(char),6);}));
Cyc_Absyndump_dumpqvar(_tmp265);
if(_tmp266 != 0){
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp266->v);
Cyc_Absyndump_dump_nospace(({const char*_tmp268="}";_tag_dyneither(_tmp268,sizeof(char),2);}));}}
# 1271
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp269=dd;enum Cyc_Absyn_Scope _tmp26A;struct _tuple0*_tmp26B;struct Cyc_List_List*_tmp26C;struct Cyc_Core_Opt*_tmp26D;int _tmp26E;_LL251: _tmp26A=_tmp269->sc;_tmp26B=_tmp269->name;_tmp26C=_tmp269->tvs;_tmp26D=_tmp269->fields;_tmp26E=_tmp269->is_extensible;_LL252:;
Cyc_Absyndump_dumpscope(_tmp26A);
if(_tmp26E)Cyc_Absyndump_dump(({const char*_tmp26F="@extensible ";_tag_dyneither(_tmp26F,sizeof(char),13);}));
Cyc_Absyndump_dump(({const char*_tmp270="datatype ";_tag_dyneither(_tmp270,sizeof(char),10);}));
Cyc_Absyndump_dumpqvar(_tmp26B);
Cyc_Absyndump_dumptvars(_tmp26C);
if(_tmp26D != 0){
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)_tmp26D->v);
Cyc_Absyndump_dump_nospace(({const char*_tmp271="}";_tag_dyneither(_tmp271,sizeof(char),2);}));}}struct _tuple20{unsigned int f1;struct _tuple0*f2;int f3;};
# 1285
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){
Cyc_Absyndump_dumploc(d->loc);{
void*_tmp272=d->r;void*_tmp273=_tmp272;struct Cyc_List_List*_tmp274;struct Cyc_List_List*_tmp275;struct Cyc_List_List*_tmp276;struct _tuple0*_tmp277;struct Cyc_List_List*_tmp278;struct _dyneither_ptr*_tmp279;struct Cyc_List_List*_tmp27A;struct Cyc_Absyn_Typedefdecl*_tmp27B;struct Cyc_Absyn_Tvar*_tmp27C;struct Cyc_Absyn_Vardecl*_tmp27D;int _tmp27E;struct Cyc_Absyn_Exp*_tmp27F;struct Cyc_List_List*_tmp280;struct Cyc_Absyn_Pat*_tmp281;struct Cyc_Absyn_Exp*_tmp282;struct Cyc_Absyn_Enumdecl*_tmp283;struct Cyc_Absyn_Datatypedecl*_tmp284;struct Cyc_Absyn_Aggrdecl*_tmp285;struct Cyc_Absyn_Fndecl*_tmp286;struct Cyc_Absyn_Vardecl*_tmp287;switch(*((int*)_tmp273)){case 0: _LL254: _tmp287=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_LL255:
# 1289
 Cyc_Absyndump_dumpvardecl(_tmp287,d->loc,1);
goto _LL253;case 1: _LL256: _tmp286=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_LL257:
# 1293
{enum Cyc_Cyclone_C_Compilers _tmp288=Cyc_Cyclone_c_compiler;if(_tmp288 == Cyc_Cyclone_Vc_c){_LL273: _LL274:
 Cyc_Absyndump_dumpatts(_tmp286->attributes);goto _LL272;}else{_LL275: _LL276:
 goto _LL272;}_LL272:;}
# 1297
if(_tmp286->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp289=Cyc_Cyclone_c_compiler;enum Cyc_Cyclone_C_Compilers _tmp28A;if(_tmp289 == Cyc_Cyclone_Vc_c){_LL278: _LL279:
 Cyc_Absyndump_dump(({const char*_tmp28B="__inline";_tag_dyneither(_tmp28B,sizeof(char),9);}));goto _LL277;}else{_LL27A: _tmp28A=_tmp289;_LL27B:
 Cyc_Absyndump_dump(({const char*_tmp28C="inline";_tag_dyneither(_tmp28C,sizeof(char),7);}));goto _LL277;}_LL277:;}
# 1302
Cyc_Absyndump_dumpscope(_tmp286->sc);{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp28E=_cycalloc(sizeof(*_tmp28E));_tmp28E[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp28F;_tmp28F.tag=9;_tmp28F.f1=({struct Cyc_Absyn_FnInfo _tmp290;_tmp290.tvars=_tmp286->tvs;_tmp290.effect=_tmp286->effect;_tmp290.ret_tqual=_tmp286->ret_tqual;_tmp290.ret_typ=_tmp286->ret_type;_tmp290.args=
# 1305
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp286->args);_tmp290.c_varargs=_tmp286->c_varargs;_tmp290.cyc_varargs=_tmp286->cyc_varargs;_tmp290.rgn_po=_tmp286->rgn_po;_tmp290.attributes=0;_tmp290.requires_clause=_tmp286->requires_clause;_tmp290.requires_relns=_tmp286->requires_relns;_tmp290.ensures_clause=_tmp286->ensures_clause;_tmp290.ensures_relns=_tmp286->ensures_relns;_tmp290;});_tmp28F;});_tmp28E;});
# 1310
{enum Cyc_Cyclone_C_Compilers _tmp28D=Cyc_Cyclone_c_compiler;if(_tmp28D == Cyc_Cyclone_Gcc_c){_LL27D: _LL27E:
# 1312
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_atts_qvar,_tmp286);
goto _LL27C;}else{_LL27F: _LL280:
# 1315
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_callconv_fdqvar,_tmp286);
goto _LL27C;}_LL27C:;}
# 1318
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpstmt(_tmp286->body,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL253;};case 5: _LL258: _tmp285=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_LL259:
# 1323
 Cyc_Absyndump_dump_aggrdecl(_tmp285);Cyc_Absyndump_dump_semi();
goto _LL253;case 6: _LL25A: _tmp284=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_LL25B:
# 1326
 Cyc_Absyndump_dump_datatypedecl(_tmp284);Cyc_Absyndump_dump_semi();
goto _LL253;case 7: _LL25C: _tmp283=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_LL25D:
# 1329
 Cyc_Absyndump_dump_enumdecl(_tmp283);Cyc_Absyndump_dump_semi();
goto _LL253;case 2: _LL25E: _tmp281=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_tmp282=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp273)->f3;_LL25F:
# 1332
 Cyc_Absyndump_dump(({const char*_tmp291="let";_tag_dyneither(_tmp291,sizeof(char),4);}));
Cyc_Absyndump_dumppat(_tmp281);
Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp(_tmp282);
Cyc_Absyndump_dump_semi();
goto _LL253;case 3: _LL260: _tmp280=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_LL261:
# 1339
 Cyc_Absyndump_dump(({const char*_tmp292="let ";_tag_dyneither(_tmp292,sizeof(char),5);}));
Cyc_Absyndump_dumpids(_tmp280);
Cyc_Absyndump_dump_semi();
goto _LL253;case 4: _LL262: _tmp27C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_tmp27D=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp273)->f2;_tmp27E=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp273)->f3;_tmp27F=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp273)->f4;_LL263:
# 1344
 Cyc_Absyndump_dump(({const char*_tmp293="region";_tag_dyneither(_tmp293,sizeof(char),7);}));
Cyc_Absyndump_dump(({const char*_tmp294="<";_tag_dyneither(_tmp294,sizeof(char),2);}));Cyc_Absyndump_dumptvar(_tmp27C);Cyc_Absyndump_dump(({const char*_tmp295="> ";_tag_dyneither(_tmp295,sizeof(char),3);}));Cyc_Absyndump_dumpqvar(_tmp27D->name);
if((unsigned int)_tmp27F){
Cyc_Absyndump_dump(({const char*_tmp296=" = open(";_tag_dyneither(_tmp296,sizeof(char),9);}));
Cyc_Absyndump_dumpexp(_tmp27F);
Cyc_Absyndump_dump(({const char*_tmp297=")";_tag_dyneither(_tmp297,sizeof(char),2);}));}
# 1351
if(_tmp27E)Cyc_Absyndump_dump(({const char*_tmp298="@resetable";_tag_dyneither(_tmp298,sizeof(char),11);}));
Cyc_Absyndump_dump_semi();
goto _LL253;case 8: _LL264: _tmp27B=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_LL265:
# 1359
 Cyc_Absyndump_dump(({const char*_tmp299="typedef";_tag_dyneither(_tmp299,sizeof(char),8);}));{
void*t;
if(_tmp27B->defn == 0)
t=Cyc_Absyn_new_evar(_tmp27B->kind,0);else{
# 1364
t=(void*)_check_null(_tmp27B->defn);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(_tmp27B->tq,t,Cyc_Absyndump_dumptypedefname,_tmp27B);
Cyc_Absyndump_dumpatts(_tmp27B->atts);
Cyc_Absyndump_dump_semi();
# 1369
goto _LL253;};case 9: _LL266: _tmp279=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_tmp27A=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp273)->f2;_LL267:
# 1371
 Cyc_Absyndump_dump(({const char*_tmp29A="namespace ";_tag_dyneither(_tmp29A,sizeof(char),11);}));
Cyc_Absyndump_dump_str(_tmp279);
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp27A != 0;_tmp27A=_tmp27A->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp27A->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL253;case 10: _LL268: _tmp277=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_tmp278=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp273)->f2;_LL269:
# 1379
 Cyc_Absyndump_dump(({const char*_tmp29B="using ";_tag_dyneither(_tmp29B,sizeof(char),7);}));
Cyc_Absyndump_dumpqvar(_tmp277);
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp278 != 0;_tmp278=_tmp278->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp278->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL253;case 11: _LL26A: _tmp276=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_LL26B:
# 1387
 Cyc_Absyndump_dump(({const char*_tmp29C="extern \"C\" {";_tag_dyneither(_tmp29C,sizeof(char),13);}));
for(0;_tmp276 != 0;_tmp276=_tmp276->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp276->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL253;case 12: _LL26C: _tmp274=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp273)->f1;_tmp275=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp273)->f2;_LL26D:
# 1393
 Cyc_Absyndump_dump(({const char*_tmp29D="extern \"C include\" {";_tag_dyneither(_tmp29D,sizeof(char),21);}));
for(0;_tmp274 != 0;_tmp274=_tmp274->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp274->hd);}
Cyc_Absyndump_dump_char((int)'}');
if(_tmp275 != 0){
Cyc_Absyndump_dump(({const char*_tmp29E=" export {";_tag_dyneither(_tmp29E,sizeof(char),10);}));
for(0;_tmp275 != 0;_tmp275=_tmp275->tl){
struct _tuple20 _tmp29F=*((struct _tuple20*)_tmp275->hd);struct _tuple20 _tmp2A0=_tmp29F;struct _tuple0*_tmp2A1;_LL282: _tmp2A1=_tmp2A0.f2;_LL283:;
Cyc_Absyndump_dumpqvar(_tmp2A1);
if(_tmp275->tl != 0)Cyc_Absyndump_dump_char((int)',');}
# 1404
Cyc_Absyndump_dump(({const char*_tmp2A2="}";_tag_dyneither(_tmp2A2,sizeof(char),2);}));}
# 1406
goto _LL253;case 13: _LL26E: _LL26F:
# 1408
 Cyc_Absyndump_dump(({const char*_tmp2A3=" __cyclone_port_on__; ";_tag_dyneither(_tmp2A3,sizeof(char),23);}));
goto _LL253;default: _LL270: _LL271:
# 1411
 Cyc_Absyndump_dump(({const char*_tmp2A4=" __cyclone_port_off__; ";_tag_dyneither(_tmp2A4,sizeof(char),24);}));
goto _LL253;}_LL253:;};}
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
void*_tmp2A5=(void*)tms->hd;void*_tmp2A6=_tmp2A5;void*_tmp2A7;union Cyc_Absyn_Constraint*_tmp2A8;union Cyc_Absyn_Constraint*_tmp2A9;union Cyc_Absyn_Constraint*_tmp2AA;struct Cyc_Absyn_Tqual _tmp2AB;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2A6)->tag == 2){_LL285: _tmp2A7=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2A6)->f1).rgn;_tmp2A8=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2A6)->f1).nullable;_tmp2A9=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2A6)->f1).bounds;_tmp2AA=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2A6)->f1).zero_term;_tmp2AB=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2A6)->f2;_LL286:
# 1432
{void*_tmp2AC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp2A9);void*_tmp2AD=_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AE;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2AD)->tag == 0){_LL28A: _LL28B:
 Cyc_Absyndump_dump_char((int)'?');goto _LL289;}else{_LL28C: _tmp2AE=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2AD)->f1;_LL28D:
# 1435
 Cyc_Absyndump_dump_char((int)(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp2A8)?'*':'@'));
Cyc_Absyndump_dump_upperbound(_tmp2AE);
goto _LL289;}_LL289:;}
# 1439
if((!Cyc_Absyndump_qvar_to_Cids  && !is_char_ptr) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2AA))Cyc_Absyndump_dump(({const char*_tmp2AF="@zeroterm";_tag_dyneither(_tmp2AF,sizeof(char),10);}));
if((!Cyc_Absyndump_qvar_to_Cids  && is_char_ptr) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2AA))Cyc_Absyndump_dump(({const char*_tmp2B0="@nozeroterm";_tag_dyneither(_tmp2B0,sizeof(char),12);}));
{void*_tmp2B1=Cyc_Tcutil_compress(_tmp2A7);void*_tmp2B2=_tmp2B1;struct Cyc_Absyn_Tvar*_tmp2B3;switch(*((int*)_tmp2B2)){case 20: _LL28F: _LL290:
 if(!Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump(({const char*_tmp2B4="`H";_tag_dyneither(_tmp2B4,sizeof(char),3);}));goto _LL28E;case 21: _LL291: _LL292:
 Cyc_Absyndump_dump(({const char*_tmp2B5="`U";_tag_dyneither(_tmp2B5,sizeof(char),3);}));goto _LL28E;case 22: _LL293: _LL294:
 Cyc_Absyndump_dump(({const char*_tmp2B6="`RC";_tag_dyneither(_tmp2B6,sizeof(char),4);}));goto _LL28E;case 2: _LL295: _tmp2B3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2B2)->f1;_LL296:
 Cyc_Absyndump_dumptvar(_tmp2B3);goto _LL28E;case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B2)->f2 == 0){_LL297: _LL298:
 Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(_tmp2A7));goto _LL28E;}else{goto _LL29B;}case 17: _LL299: _LL29A:
 Cyc_Absyndump_dump(({const char*_tmp2B7="@region(";_tag_dyneither(_tmp2B7,sizeof(char),9);}));Cyc_Absyndump_dumptyp(_tmp2A7);Cyc_Absyndump_dump(({const char*_tmp2B8=")";_tag_dyneither(_tmp2B8,sizeof(char),2);}));goto _LL28E;default: _LL29B: _LL29C:
({void*_tmp2B9=0;Cyc_Tcutil_impos(({const char*_tmp2BA="dumptms: bad rgn type in Pointer_mod";_tag_dyneither(_tmp2BA,sizeof(char),37);}),_tag_dyneither(_tmp2B9,sizeof(void*),0));});}_LL28E:;}
# 1450
Cyc_Absyndump_dumptq(_tmp2AB);
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
return;}else{_LL287: _LL288: {
# 1455
int next_is_pointer=0;
if(tms->tl != 0){
void*_tmp2BB=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmp2BC=_tmp2BB;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2BC)->tag == 2){_LL29E: _LL29F:
 next_is_pointer=1;goto _LL29D;}else{_LL2A0: _LL2A1:
 goto _LL29D;}_LL29D:;}
# 1461
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)')');
{void*_tmp2BD=(void*)tms->hd;void*_tmp2BE=_tmp2BD;struct Cyc_List_List*_tmp2BF;struct Cyc_List_List*_tmp2C0;unsigned int _tmp2C1;int _tmp2C2;struct Cyc_List_List*_tmp2C3;unsigned int _tmp2C4;struct Cyc_List_List*_tmp2C5;int _tmp2C6;struct Cyc_Absyn_VarargInfo*_tmp2C7;void*_tmp2C8;struct Cyc_List_List*_tmp2C9;struct Cyc_Absyn_Exp*_tmp2CA;struct Cyc_Absyn_Exp*_tmp2CB;struct Cyc_Absyn_Exp*_tmp2CC;union Cyc_Absyn_Constraint*_tmp2CD;union Cyc_Absyn_Constraint*_tmp2CE;switch(*((int*)_tmp2BE)){case 0: _LL2A3: _tmp2CE=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1;_LL2A4:
# 1468
 Cyc_Absyndump_dump(({const char*_tmp2CF="[]";_tag_dyneither(_tmp2CF,sizeof(char),3);}));
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2CE))Cyc_Absyndump_dump(({const char*_tmp2D0="@zeroterm";_tag_dyneither(_tmp2D0,sizeof(char),10);}));
goto _LL2A2;case 1: _LL2A5: _tmp2CC=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1;_tmp2CD=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f2;_LL2A6:
# 1472
 Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp2CC);Cyc_Absyndump_dump_char((int)']');
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2CD))Cyc_Absyndump_dump(({const char*_tmp2D1="@zeroterm";_tag_dyneither(_tmp2D1,sizeof(char),10);}));
goto _LL2A2;case 3: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1)->tag == 1){_LL2A7: _tmp2C5=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1)->f1;_tmp2C6=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1)->f2;_tmp2C7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1)->f3;_tmp2C8=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1)->f4;_tmp2C9=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1)->f5;_tmp2CA=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1)->f6;_tmp2CB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1)->f7;_LL2A8:
# 1476
 Cyc_Absyndump_dumpfunargs(_tmp2C5,_tmp2C6,_tmp2C7,_tmp2C8,_tmp2C9,_tmp2CA,_tmp2CB);goto _LL2A2;}else{_LL2A9: _tmp2C3=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1)->f1;_tmp2C4=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1)->f2;_LL2AA:
# 1478
((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,_tmp2C3,({const char*_tmp2D2="(";_tag_dyneither(_tmp2D2,sizeof(char),2);}),({const char*_tmp2D3=")";_tag_dyneither(_tmp2D3,sizeof(char),2);}),({const char*_tmp2D4=",";_tag_dyneither(_tmp2D4,sizeof(char),2);}));goto _LL2A2;}case 4: _LL2AB: _tmp2C0=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f1;_tmp2C1=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f2;_tmp2C2=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f3;_LL2AC:
# 1480
 if(_tmp2C2)Cyc_Absyndump_dumpkindedtvars(_tmp2C0);else{Cyc_Absyndump_dumptvars(_tmp2C0);}goto _LL2A2;case 5: _LL2AD: _tmp2BF=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp2BE)->f2;_LL2AE:
 Cyc_Absyndump_dumpatts(_tmp2BF);goto _LL2A2;default: _LL2AF: _LL2B0:
({void*_tmp2D5=0;Cyc_Tcutil_impos(({const char*_tmp2D6="dumptms";_tag_dyneither(_tmp2D6,sizeof(char),8);}),_tag_dyneither(_tmp2D5,sizeof(void*),0));});}_LL2A2:;}
# 1484
return;}}_LL284:;};}
# 1488
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
int _tmp2D7=Cyc_Absyndump_is_char_ptr(t);
struct _RegionHandle _tmp2D8=_new_region("temp");struct _RegionHandle*temp=& _tmp2D8;_push_region(temp);
{struct _tuple11 _tmp2D9=Cyc_Absynpp_to_tms(temp,tq,t);struct _tuple11 _tmp2DA=_tmp2D9;struct Cyc_Absyn_Tqual _tmp2DB;void*_tmp2DC;struct Cyc_List_List*_tmp2DD;_LL2B2: _tmp2DB=_tmp2DA.f1;_tmp2DC=_tmp2DA.f2;_tmp2DD=_tmp2DA.f3;_LL2B3:;
Cyc_Absyndump_dumptq(_tmp2DB);
Cyc_Absyndump_dumpntyp(_tmp2DC);
Cyc_Absyndump_dumptms(_tmp2D7,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2DD),f,a);}
# 1491
;_pop_region(temp);}
# 1498
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
# 1500
*Cyc_Absyndump_dump_file=f;
for(0;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
({void*_tmp2DE=0;Cyc_fprintf(f,({const char*_tmp2DF="\n";_tag_dyneither(_tmp2DF,sizeof(char),2);}),_tag_dyneither(_tmp2DE,sizeof(void*),0));});}
# 1506
static void Cyc_Absyndump_dump_decl_interface(struct Cyc_Absyn_Decl*d){
void*_tmp2E0=d->r;void*_tmp2E1=_tmp2E0;struct Cyc_List_List*_tmp2E2;struct Cyc_List_List*_tmp2E3;struct Cyc_List_List*_tmp2E4;struct Cyc_List_List*_tmp2E5;struct _dyneither_ptr*_tmp2E6;struct Cyc_List_List*_tmp2E7;struct Cyc_Absyn_Typedefdecl*_tmp2E8;struct Cyc_Absyn_Enumdecl*_tmp2E9;struct Cyc_Absyn_Datatypedecl*_tmp2EA;struct Cyc_Absyn_Aggrdecl*_tmp2EB;struct Cyc_Absyn_Fndecl*_tmp2EC;struct Cyc_Absyn_Vardecl*_tmp2ED;switch(*((int*)_tmp2E1)){case 0: _LL2B5: _tmp2ED=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2E1)->f1;_LL2B6:
# 1509
 if(_tmp2ED->sc == Cyc_Absyn_Static)return;{
struct Cyc_Absyn_Exp*init=_tmp2ED->initializer;
_tmp2ED->initializer=0;
if(_tmp2ED->sc == Cyc_Absyn_Public)
Cyc_Absyndump_dump(({const char*_tmp2EE="extern ";_tag_dyneither(_tmp2EE,sizeof(char),8);}));
Cyc_Absyndump_dumpvardecl(_tmp2ED,d->loc,1);
Cyc_Absyndump_dump(({const char*_tmp2EF="\n";_tag_dyneither(_tmp2EF,sizeof(char),2);}));
_tmp2ED->initializer=init;
goto _LL2B4;};case 1: _LL2B7: _tmp2EC=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2E1)->f1;_LL2B8:
# 1519
 if(_tmp2EC->sc == Cyc_Absyn_Static)return;
Cyc_Absyndump_dumpscope(_tmp2EC->sc);{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2F1=_cycalloc(sizeof(*_tmp2F1));_tmp2F1[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp2F2;_tmp2F2.tag=9;_tmp2F2.f1=({struct Cyc_Absyn_FnInfo _tmp2F3;_tmp2F3.tvars=_tmp2EC->tvs;_tmp2F3.effect=_tmp2EC->effect;_tmp2F3.ret_tqual=_tmp2EC->ret_tqual;_tmp2F3.ret_typ=_tmp2EC->ret_type;_tmp2F3.args=
# 1523
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp2EC->args);_tmp2F3.c_varargs=_tmp2EC->c_varargs;_tmp2F3.cyc_varargs=_tmp2EC->cyc_varargs;_tmp2F3.rgn_po=_tmp2EC->rgn_po;_tmp2F3.attributes=0;_tmp2F3.requires_clause=_tmp2EC->requires_clause;_tmp2F3.requires_relns=_tmp2EC->requires_relns;_tmp2F3.ensures_clause=_tmp2EC->ensures_clause;_tmp2F3.ensures_relns=_tmp2EC->ensures_relns;_tmp2F3;});_tmp2F2;});_tmp2F1;});
# 1528
((void(*)(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*a))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_atts_qvar,_tmp2EC);
Cyc_Absyndump_dump(({const char*_tmp2F0=";\n";_tag_dyneither(_tmp2F0,sizeof(char),3);}));
goto _LL2B4;};case 5: _LL2B9: _tmp2EB=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2E1)->f1;_LL2BA:
# 1532
 if(_tmp2EB->sc == Cyc_Absyn_Static)return;{
struct Cyc_Absyn_AggrdeclImpl*impl=_tmp2EB->impl;
if(_tmp2EB->sc == Cyc_Absyn_Abstract)_tmp2EB->impl=0;
Cyc_Absyndump_dump_aggrdecl(_tmp2EB);
_tmp2EB->impl=impl;
Cyc_Absyndump_dump(({const char*_tmp2F4=";\n";_tag_dyneither(_tmp2F4,sizeof(char),3);}));
goto _LL2B4;};case 6: _LL2BB: _tmp2EA=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp2E1)->f1;_LL2BC:
# 1540
 Cyc_Absyndump_dump_datatypedecl(_tmp2EA);
Cyc_Absyndump_dump(({const char*_tmp2F5=";\n";_tag_dyneither(_tmp2F5,sizeof(char),3);}));
goto _LL2B4;case 7: _LL2BD: _tmp2E9=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2E1)->f1;_LL2BE:
# 1544
 Cyc_Absyndump_dump_enumdecl(_tmp2E9);
Cyc_Absyndump_dump(({const char*_tmp2F6=";\n";_tag_dyneither(_tmp2F6,sizeof(char),3);}));
goto _LL2B4;case 8: _LL2BF: _tmp2E8=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2E1)->f1;_LL2C0:
# 1548
 if(_tmp2E8->defn == 0){
Cyc_Absyndump_dumpdecl(d);
Cyc_Absyndump_dump(({const char*_tmp2F7="\n";_tag_dyneither(_tmp2F7,sizeof(char),2);}));}
# 1552
goto _LL2B4;case 9: _LL2C1: _tmp2E6=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2E1)->f1;_tmp2E7=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2E1)->f2;_LL2C2:
# 1554
 Cyc_Absyndump_dump(({const char*_tmp2F8="namespace ";_tag_dyneither(_tmp2F8,sizeof(char),11);}));
Cyc_Absyndump_dump_str(_tmp2E6);
Cyc_Absyndump_dump(({const char*_tmp2F9="{\n";_tag_dyneither(_tmp2F9,sizeof(char),3);}));
for(0;_tmp2E7 != 0;_tmp2E7=_tmp2E7->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2E7->hd);}
Cyc_Absyndump_dump(({const char*_tmp2FA="}\n";_tag_dyneither(_tmp2FA,sizeof(char),3);}));
goto _LL2B4;case 10: _LL2C3: _tmp2E5=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp2E1)->f2;_LL2C4:
# 1562
 for(0;_tmp2E5 != 0;_tmp2E5=_tmp2E5->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2E5->hd);}
goto _LL2B4;case 11: _LL2C5: _tmp2E4=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp2E1)->f1;_LL2C6:
# 1566
 Cyc_Absyndump_dump(({const char*_tmp2FB="extern \"C\" {";_tag_dyneither(_tmp2FB,sizeof(char),13);}));
for(0;_tmp2E4 != 0;_tmp2E4=_tmp2E4->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2E4->hd);}
Cyc_Absyndump_dump(({const char*_tmp2FC="}\n";_tag_dyneither(_tmp2FC,sizeof(char),3);}));
goto _LL2B4;case 12: _LL2C7: _tmp2E2=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2E1)->f1;_tmp2E3=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2E1)->f2;_LL2C8:
# 1587 "absyndump.cyc"
 goto _LL2B4;default: _LL2C9: _LL2CA:
({void*_tmp2FD=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2FE="bad top-level declaration";_tag_dyneither(_tmp2FE,sizeof(char),26);}),_tag_dyneither(_tmp2FD,sizeof(void*),0));});}_LL2B4:;}
# 1592
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f){
*Cyc_Absyndump_dump_file=f;
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}}
