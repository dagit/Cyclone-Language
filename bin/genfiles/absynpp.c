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
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;
# 97 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170
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
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 258
int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 387
int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;
# 41
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;
# 50
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);
# 53
struct _dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);
# 67 "pp.h"
struct Cyc_PP_Doc*Cyc_PP_nil_doc();
# 69
struct Cyc_PP_Doc*Cyc_PP_blank_doc();
# 72
struct Cyc_PP_Doc*Cyc_PP_line_doc();
# 78
struct Cyc_PP_Doc*Cyc_PP_text(struct _dyneither_ptr s);
# 80
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*p);
# 83
struct Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int w);
# 91
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);
# 94
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr);
# 108
struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 112
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 117
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,struct Cyc_List_List*l0);
# 120
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 123
struct Cyc_PP_Doc*Cyc_PP_group(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 129
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 458 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 461
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 522
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 548
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 555
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 573
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 737 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 937
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 947
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 955
void*Cyc_Absyn_compress_kb(void*);
# 960
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 990
extern void*Cyc_Absyn_bounds_one;
# 1045
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1072
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1177
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1183
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);struct Cyc_Buffer_t;
# 46 "buffer.h"
typedef struct Cyc_Buffer_t*Cyc_Buffer_T;
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};
# 100
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 137 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 71 "tcutil.h"
void*Cyc_Tcutil_compress(void*t);
# 316 "tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 318
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 54 "absynpp.h"
extern int Cyc_Absynpp_print_for_cycdoc;
# 56
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 58
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 60
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 62
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);
# 64
struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long);
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*);
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
struct _dyneither_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst);
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p);
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);
struct _dyneither_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 79
int Cyc_Absynpp_is_anon_aggrtype(void*t);
extern struct _dyneither_ptr Cyc_Absynpp_cyc_string;
extern struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
# 89
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);
# 27 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};
# 33
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 43 "absynpp.cyc"
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp);
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope);
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*,int expstmt);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e);
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es);
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*);
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*);
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst);
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop);
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es);struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de);
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*dopt);
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*requires,struct Cyc_Absyn_Exp*ensures);
# 64
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields);
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad);
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ad);
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*ad);
# 71
static int Cyc_Absynpp_expand_typedefs;
# 75
static int Cyc_Absynpp_qvar_to_Cids;static char _tmp0[4]="Cyc";
# 77
struct _dyneither_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,_tmp0 + 4};
struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
# 80
static int Cyc_Absynpp_add_cyc_prefix;
# 84
static int Cyc_Absynpp_to_VC;
# 87
static int Cyc_Absynpp_decls_first;
# 91
static int Cyc_Absynpp_rewrite_temp_tvars;
# 94
static int Cyc_Absynpp_print_all_tvars;
# 97
static int Cyc_Absynpp_print_all_kinds;
# 100
static int Cyc_Absynpp_print_all_effects;
# 103
static int Cyc_Absynpp_print_using_stmts;
# 108
static int Cyc_Absynpp_print_externC_stmts;
# 112
static int Cyc_Absynpp_print_full_evars;
# 115
static int Cyc_Absynpp_generate_line_directives;
# 118
static int Cyc_Absynpp_use_curr_namespace;
# 121
static int Cyc_Absynpp_print_zeroterm;
# 124
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;
# 127
int Cyc_Absynpp_print_for_cycdoc=0;struct Cyc_Absynpp_Params;
# 148
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs){
Cyc_Absynpp_expand_typedefs=fs->expand_typedefs;
Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absynpp_to_VC=fs->to_VC;
Cyc_Absynpp_decls_first=fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;
Cyc_Absynpp_print_all_tvars=fs->print_all_tvars;
Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;
Cyc_Absynpp_print_all_effects=fs->print_all_effects;
Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;
Cyc_Absynpp_print_externC_stmts=fs->print_externC_stmts;
Cyc_Absynpp_print_full_evars=fs->print_full_evars;
Cyc_Absynpp_print_zeroterm=fs->print_zeroterm;
Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;
Cyc_Absynpp_curr_namespace=fs->curr_namespace;}
# 167
struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 187
struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,1,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 207
struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,0,1,0,0,0,0,0,0,0,0,1,0,0};
# 227
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,0,0};
# 248
static void Cyc_Absynpp_curr_namespace_add(struct _dyneither_ptr*v){
struct Cyc_List_List*_tmp611;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((_tmp611=_cycalloc(sizeof(*_tmp611)),((_tmp611->hd=v,((_tmp611->tl=0,_tmp611)))))));}
# 252
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){
if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)
*l=0;else{
# 256
Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
# 260
static void Cyc_Absynpp_curr_namespace_drop(){
((void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
# 264
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c){
switch(c){case '\a': _LL0: {
const char*_tmp612;return(_tmp612="\\a",_tag_dyneither(_tmp612,sizeof(char),3));}case '\b': _LL1: {
const char*_tmp613;return(_tmp613="\\b",_tag_dyneither(_tmp613,sizeof(char),3));}case '\f': _LL2: {
const char*_tmp614;return(_tmp614="\\f",_tag_dyneither(_tmp614,sizeof(char),3));}case '\n': _LL3: {
const char*_tmp615;return(_tmp615="\\n",_tag_dyneither(_tmp615,sizeof(char),3));}case '\r': _LL4: {
const char*_tmp616;return(_tmp616="\\r",_tag_dyneither(_tmp616,sizeof(char),3));}case '\t': _LL5: {
const char*_tmp617;return(_tmp617="\\t",_tag_dyneither(_tmp617,sizeof(char),3));}case '\v': _LL6: {
const char*_tmp618;return(_tmp618="\\v",_tag_dyneither(_tmp618,sizeof(char),3));}case '\\': _LL7: {
const char*_tmp619;return(_tmp619="\\\\",_tag_dyneither(_tmp619,sizeof(char),3));}case '"': _LL8: {
const char*_tmp61A;return(_tmp61A="\"",_tag_dyneither(_tmp61A,sizeof(char),2));}case '\'': _LL9: {
const char*_tmp61B;return(_tmp61B="\\'",_tag_dyneither(_tmp61B,sizeof(char),3));}default: _LLA:
# 277
 if(c >= ' '  && c <= '~'){
struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);
{char _tmp61E;char _tmp61D;struct _dyneither_ptr _tmp61C;(_tmp61C=_dyneither_ptr_plus(_tmpC,sizeof(char),0),((_tmp61D=*((char*)_check_dyneither_subscript(_tmp61C,sizeof(char),0)),((_tmp61E=c,((_get_dyneither_size(_tmp61C,sizeof(char))== 1  && (_tmp61D == '\000'  && _tmp61E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61C.curr)=_tmp61E)))))));}
return(struct _dyneither_ptr)_tmpC;}else{
# 282
struct _dyneither_ptr _tmp10=Cyc_Core_new_string(5);
int j=0;
{char _tmp621;char _tmp620;struct _dyneither_ptr _tmp61F;(_tmp61F=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp620=*((char*)_check_dyneither_subscript(_tmp61F,sizeof(char),0)),((_tmp621='\\',((_get_dyneither_size(_tmp61F,sizeof(char))== 1  && (_tmp620 == '\000'  && _tmp621 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61F.curr)=_tmp621)))))));}
{char _tmp624;char _tmp623;struct _dyneither_ptr _tmp622;(_tmp622=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp623=*((char*)_check_dyneither_subscript(_tmp622,sizeof(char),0)),((_tmp624=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp622,sizeof(char))== 1  && (_tmp623 == '\000'  && _tmp624 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp622.curr)=_tmp624)))))));}
{char _tmp627;char _tmp626;struct _dyneither_ptr _tmp625;(_tmp625=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp626=*((char*)_check_dyneither_subscript(_tmp625,sizeof(char),0)),((_tmp627=(char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp625,sizeof(char))== 1  && (_tmp626 == '\000'  && _tmp627 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp625.curr)=_tmp627)))))));}
{char _tmp62A;char _tmp629;struct _dyneither_ptr _tmp628;(_tmp628=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp629=*((char*)_check_dyneither_subscript(_tmp628,sizeof(char),0)),((_tmp62A=(char)('0' + (c & 7)),((_get_dyneither_size(_tmp628,sizeof(char))== 1  && (_tmp629 == '\000'  && _tmp62A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp628.curr)=_tmp62A)))))));}
return(struct _dyneither_ptr)_tmp10;}}}
# 293
static int Cyc_Absynpp_special(struct _dyneither_ptr s){
int sz=(int)(_get_dyneither_size(s,sizeof(char))- 1);
{int i=0;for(0;i < sz;++ i){
char c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(((c <= ' '  || c >= '~') || c == '"') || c == '\\')
return 1;}}
# 300
return 0;}
# 303
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr s){
if(!Cyc_Absynpp_special(s))return s;{
# 306
int n=(int)(_get_dyneither_size(s,sizeof(char))- 1);
# 308
if(n > 0  && *((const char*)_check_dyneither_subscript(s,sizeof(char),n))== '\000')-- n;{
# 310
int len=0;
{int i=0;for(0;i <= n;++ i){
char _tmp1D=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));char _tmp1E=_tmp1D;char _tmp1F;_LLD: if(_tmp1E != '\a')goto _LLF;_LLE:
 goto _LL10;_LLF: if(_tmp1E != '\b')goto _LL11;_LL10:
 goto _LL12;_LL11: if(_tmp1E != '\f')goto _LL13;_LL12:
 goto _LL14;_LL13: if(_tmp1E != '\n')goto _LL15;_LL14:
 goto _LL16;_LL15: if(_tmp1E != '\r')goto _LL17;_LL16:
 goto _LL18;_LL17: if(_tmp1E != '\t')goto _LL19;_LL18:
 goto _LL1A;_LL19: if(_tmp1E != '\v')goto _LL1B;_LL1A:
 goto _LL1C;_LL1B: if(_tmp1E != '\\')goto _LL1D;_LL1C:
 goto _LL1E;_LL1D: if(_tmp1E != '"')goto _LL1F;_LL1E:
 len +=2;goto _LLC;_LL1F: _tmp1F=_tmp1E;_LL20:
# 323
 if(_tmp1F >= ' '  && _tmp1F <= '~')++ len;else{
len +=4;}
goto _LLC;_LLC:;}}{
# 328
struct _dyneither_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));
int j=0;
{int i=0;for(0;i <= n;++ i){
char _tmp20=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));char _tmp21=_tmp20;char _tmp22;_LL22: if(_tmp21 != '\a')goto _LL24;_LL23:
{char _tmp62D;char _tmp62C;struct _dyneither_ptr _tmp62B;(_tmp62B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp62C=*((char*)_check_dyneither_subscript(_tmp62B,sizeof(char),0)),((_tmp62D='\\',((_get_dyneither_size(_tmp62B,sizeof(char))== 1  && (_tmp62C == '\000'  && _tmp62D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62B.curr)=_tmp62D)))))));}{char _tmp630;char _tmp62F;struct _dyneither_ptr _tmp62E;(_tmp62E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp62F=*((char*)_check_dyneither_subscript(_tmp62E,sizeof(char),0)),((_tmp630='a',((_get_dyneither_size(_tmp62E,sizeof(char))== 1  && (_tmp62F == '\000'  && _tmp630 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62E.curr)=_tmp630)))))));}goto _LL21;_LL24: if(_tmp21 != '\b')goto _LL26;_LL25:
{char _tmp633;char _tmp632;struct _dyneither_ptr _tmp631;(_tmp631=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp632=*((char*)_check_dyneither_subscript(_tmp631,sizeof(char),0)),((_tmp633='\\',((_get_dyneither_size(_tmp631,sizeof(char))== 1  && (_tmp632 == '\000'  && _tmp633 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp631.curr)=_tmp633)))))));}{char _tmp636;char _tmp635;struct _dyneither_ptr _tmp634;(_tmp634=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp635=*((char*)_check_dyneither_subscript(_tmp634,sizeof(char),0)),((_tmp636='b',((_get_dyneither_size(_tmp634,sizeof(char))== 1  && (_tmp635 == '\000'  && _tmp636 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp634.curr)=_tmp636)))))));}goto _LL21;_LL26: if(_tmp21 != '\f')goto _LL28;_LL27:
{char _tmp639;char _tmp638;struct _dyneither_ptr _tmp637;(_tmp637=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp638=*((char*)_check_dyneither_subscript(_tmp637,sizeof(char),0)),((_tmp639='\\',((_get_dyneither_size(_tmp637,sizeof(char))== 1  && (_tmp638 == '\000'  && _tmp639 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp637.curr)=_tmp639)))))));}{char _tmp63C;char _tmp63B;struct _dyneither_ptr _tmp63A;(_tmp63A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp63B=*((char*)_check_dyneither_subscript(_tmp63A,sizeof(char),0)),((_tmp63C='f',((_get_dyneither_size(_tmp63A,sizeof(char))== 1  && (_tmp63B == '\000'  && _tmp63C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp63A.curr)=_tmp63C)))))));}goto _LL21;_LL28: if(_tmp21 != '\n')goto _LL2A;_LL29:
{char _tmp63F;char _tmp63E;struct _dyneither_ptr _tmp63D;(_tmp63D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp63E=*((char*)_check_dyneither_subscript(_tmp63D,sizeof(char),0)),((_tmp63F='\\',((_get_dyneither_size(_tmp63D,sizeof(char))== 1  && (_tmp63E == '\000'  && _tmp63F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp63D.curr)=_tmp63F)))))));}{char _tmp642;char _tmp641;struct _dyneither_ptr _tmp640;(_tmp640=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp641=*((char*)_check_dyneither_subscript(_tmp640,sizeof(char),0)),((_tmp642='n',((_get_dyneither_size(_tmp640,sizeof(char))== 1  && (_tmp641 == '\000'  && _tmp642 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp640.curr)=_tmp642)))))));}goto _LL21;_LL2A: if(_tmp21 != '\r')goto _LL2C;_LL2B:
{char _tmp645;char _tmp644;struct _dyneither_ptr _tmp643;(_tmp643=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp644=*((char*)_check_dyneither_subscript(_tmp643,sizeof(char),0)),((_tmp645='\\',((_get_dyneither_size(_tmp643,sizeof(char))== 1  && (_tmp644 == '\000'  && _tmp645 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp643.curr)=_tmp645)))))));}{char _tmp648;char _tmp647;struct _dyneither_ptr _tmp646;(_tmp646=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp647=*((char*)_check_dyneither_subscript(_tmp646,sizeof(char),0)),((_tmp648='r',((_get_dyneither_size(_tmp646,sizeof(char))== 1  && (_tmp647 == '\000'  && _tmp648 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp646.curr)=_tmp648)))))));}goto _LL21;_LL2C: if(_tmp21 != '\t')goto _LL2E;_LL2D:
{char _tmp64B;char _tmp64A;struct _dyneither_ptr _tmp649;(_tmp649=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp64A=*((char*)_check_dyneither_subscript(_tmp649,sizeof(char),0)),((_tmp64B='\\',((_get_dyneither_size(_tmp649,sizeof(char))== 1  && (_tmp64A == '\000'  && _tmp64B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp649.curr)=_tmp64B)))))));}{char _tmp64E;char _tmp64D;struct _dyneither_ptr _tmp64C;(_tmp64C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp64D=*((char*)_check_dyneither_subscript(_tmp64C,sizeof(char),0)),((_tmp64E='t',((_get_dyneither_size(_tmp64C,sizeof(char))== 1  && (_tmp64D == '\000'  && _tmp64E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp64C.curr)=_tmp64E)))))));}goto _LL21;_LL2E: if(_tmp21 != '\v')goto _LL30;_LL2F:
{char _tmp651;char _tmp650;struct _dyneither_ptr _tmp64F;(_tmp64F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp650=*((char*)_check_dyneither_subscript(_tmp64F,sizeof(char),0)),((_tmp651='\\',((_get_dyneither_size(_tmp64F,sizeof(char))== 1  && (_tmp650 == '\000'  && _tmp651 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp64F.curr)=_tmp651)))))));}{char _tmp654;char _tmp653;struct _dyneither_ptr _tmp652;(_tmp652=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp653=*((char*)_check_dyneither_subscript(_tmp652,sizeof(char),0)),((_tmp654='v',((_get_dyneither_size(_tmp652,sizeof(char))== 1  && (_tmp653 == '\000'  && _tmp654 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp652.curr)=_tmp654)))))));}goto _LL21;_LL30: if(_tmp21 != '\\')goto _LL32;_LL31:
{char _tmp657;char _tmp656;struct _dyneither_ptr _tmp655;(_tmp655=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp656=*((char*)_check_dyneither_subscript(_tmp655,sizeof(char),0)),((_tmp657='\\',((_get_dyneither_size(_tmp655,sizeof(char))== 1  && (_tmp656 == '\000'  && _tmp657 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp655.curr)=_tmp657)))))));}{char _tmp65A;char _tmp659;struct _dyneither_ptr _tmp658;(_tmp658=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp659=*((char*)_check_dyneither_subscript(_tmp658,sizeof(char),0)),((_tmp65A='\\',((_get_dyneither_size(_tmp658,sizeof(char))== 1  && (_tmp659 == '\000'  && _tmp65A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp658.curr)=_tmp65A)))))));}goto _LL21;_LL32: if(_tmp21 != '"')goto _LL34;_LL33:
{char _tmp65D;char _tmp65C;struct _dyneither_ptr _tmp65B;(_tmp65B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp65C=*((char*)_check_dyneither_subscript(_tmp65B,sizeof(char),0)),((_tmp65D='\\',((_get_dyneither_size(_tmp65B,sizeof(char))== 1  && (_tmp65C == '\000'  && _tmp65D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp65B.curr)=_tmp65D)))))));}{char _tmp660;char _tmp65F;struct _dyneither_ptr _tmp65E;(_tmp65E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp65F=*((char*)_check_dyneither_subscript(_tmp65E,sizeof(char),0)),((_tmp660='"',((_get_dyneither_size(_tmp65E,sizeof(char))== 1  && (_tmp65F == '\000'  && _tmp660 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp65E.curr)=_tmp660)))))));}goto _LL21;_LL34: _tmp22=_tmp21;_LL35:
# 342
 if(_tmp22 >= ' '  && _tmp22 <= '~'){char _tmp663;char _tmp662;struct _dyneither_ptr _tmp661;(_tmp661=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp662=*((char*)_check_dyneither_subscript(_tmp661,sizeof(char),0)),((_tmp663=_tmp22,((_get_dyneither_size(_tmp661,sizeof(char))== 1  && (_tmp662 == '\000'  && _tmp663 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp661.curr)=_tmp663)))))));}else{
# 344
{char _tmp666;char _tmp665;struct _dyneither_ptr _tmp664;(_tmp664=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp665=*((char*)_check_dyneither_subscript(_tmp664,sizeof(char),0)),((_tmp666='\\',((_get_dyneither_size(_tmp664,sizeof(char))== 1  && (_tmp665 == '\000'  && _tmp666 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp664.curr)=_tmp666)))))));}
{char _tmp669;char _tmp668;struct _dyneither_ptr _tmp667;(_tmp667=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp668=*((char*)_check_dyneither_subscript(_tmp667,sizeof(char),0)),((_tmp669=(char)('0' + (_tmp22 >> 6 & 7)),((_get_dyneither_size(_tmp667,sizeof(char))== 1  && (_tmp668 == '\000'  && _tmp669 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp667.curr)=_tmp669)))))));}
{char _tmp66C;char _tmp66B;struct _dyneither_ptr _tmp66A;(_tmp66A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp66B=*((char*)_check_dyneither_subscript(_tmp66A,sizeof(char),0)),((_tmp66C=(char)('0' + (_tmp22 >> 3 & 7)),((_get_dyneither_size(_tmp66A,sizeof(char))== 1  && (_tmp66B == '\000'  && _tmp66C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp66A.curr)=_tmp66C)))))));}{
char _tmp66F;char _tmp66E;struct _dyneither_ptr _tmp66D;(_tmp66D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp66E=*((char*)_check_dyneither_subscript(_tmp66D,sizeof(char),0)),((_tmp66F=(char)('0' + (_tmp22 & 7)),((_get_dyneither_size(_tmp66D,sizeof(char))== 1  && (_tmp66E == '\000'  && _tmp66F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp66D.curr)=_tmp66F)))))));};}
# 349
goto _LL21;_LL21:;}}
# 351
return(struct _dyneither_ptr)t;};};};}static char _tmp68[9]="restrict";
# 354
static struct _dyneither_ptr Cyc_Absynpp_restrict_string={_tmp68,_tmp68,_tmp68 + 9};static char _tmp69[9]="volatile";
static struct _dyneither_ptr Cyc_Absynpp_volatile_string={_tmp69,_tmp69,_tmp69 + 9};static char _tmp6A[6]="const";
static struct _dyneither_ptr Cyc_Absynpp_const_str={_tmp6A,_tmp6A,_tmp6A + 6};
static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
# 361
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){
struct Cyc_List_List*l=0;
# 364
if(tq.q_restrict){struct Cyc_List_List*_tmp670;l=((_tmp670=_cycalloc(sizeof(*_tmp670)),((_tmp670->hd=Cyc_Absynpp_restrict_sp,((_tmp670->tl=l,_tmp670))))));}
if(tq.q_volatile){struct Cyc_List_List*_tmp671;l=((_tmp671=_cycalloc(sizeof(*_tmp671)),((_tmp671->hd=Cyc_Absynpp_volatile_sp,((_tmp671->tl=l,_tmp671))))));}
if(tq.print_const){struct Cyc_List_List*_tmp672;l=((_tmp672=_cycalloc(sizeof(*_tmp672)),((_tmp672->hd=Cyc_Absynpp_const_sp,((_tmp672->tl=l,_tmp672))))));}{
const char*_tmp675;const char*_tmp674;const char*_tmp673;return Cyc_PP_egroup(((_tmp673="",_tag_dyneither(_tmp673,sizeof(char),1))),((_tmp674=" ",_tag_dyneither(_tmp674,sizeof(char),2))),((_tmp675=" ",_tag_dyneither(_tmp675,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));};}
# 370
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp72;enum Cyc_Absyn_AliasQual _tmp73;struct Cyc_Absyn_Kind*_tmp71=ka;_tmp72=_tmp71->kind;_tmp73=_tmp71->aliasqual;
switch(_tmp72){case Cyc_Absyn_AnyKind: _LL36:
# 374
 switch(_tmp73){case Cyc_Absyn_Aliasable: _LL38: {
const char*_tmp676;return(_tmp676="A",_tag_dyneither(_tmp676,sizeof(char),2));}case Cyc_Absyn_Unique: _LL39: {
const char*_tmp677;return(_tmp677="UA",_tag_dyneither(_tmp677,sizeof(char),3));}case Cyc_Absyn_Top: _LL3A: {
const char*_tmp678;return(_tmp678="TA",_tag_dyneither(_tmp678,sizeof(char),3));}}case Cyc_Absyn_MemKind: _LL37:
# 380
 switch(_tmp73){case Cyc_Absyn_Aliasable: _LL3D: {
const char*_tmp679;return(_tmp679="M",_tag_dyneither(_tmp679,sizeof(char),2));}case Cyc_Absyn_Unique: _LL3E: {
const char*_tmp67A;return(_tmp67A="UM",_tag_dyneither(_tmp67A,sizeof(char),3));}case Cyc_Absyn_Top: _LL3F: {
const char*_tmp67B;return(_tmp67B="TM",_tag_dyneither(_tmp67B,sizeof(char),3));}}case Cyc_Absyn_BoxKind: _LL3C:
# 386
 switch(_tmp73){case Cyc_Absyn_Aliasable: _LL42: {
const char*_tmp67C;return(_tmp67C="B",_tag_dyneither(_tmp67C,sizeof(char),2));}case Cyc_Absyn_Unique: _LL43: {
const char*_tmp67D;return(_tmp67D="UB",_tag_dyneither(_tmp67D,sizeof(char),3));}case Cyc_Absyn_Top: _LL44: {
const char*_tmp67E;return(_tmp67E="TB",_tag_dyneither(_tmp67E,sizeof(char),3));}}case Cyc_Absyn_RgnKind: _LL41:
# 392
 switch(_tmp73){case Cyc_Absyn_Aliasable: _LL47: {
const char*_tmp67F;return(_tmp67F="R",_tag_dyneither(_tmp67F,sizeof(char),2));}case Cyc_Absyn_Unique: _LL48: {
const char*_tmp680;return(_tmp680="UR",_tag_dyneither(_tmp680,sizeof(char),3));}case Cyc_Absyn_Top: _LL49: {
const char*_tmp681;return(_tmp681="TR",_tag_dyneither(_tmp681,sizeof(char),3));}}case Cyc_Absyn_EffKind: _LL46: {
# 397
const char*_tmp682;return(_tmp682="E",_tag_dyneither(_tmp682,sizeof(char),2));}case Cyc_Absyn_IntKind: _LL4B: {
const char*_tmp683;return(_tmp683="I",_tag_dyneither(_tmp683,sizeof(char),2));}}}
# 401
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 403
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp82=Cyc_Absyn_compress_kb(c);void*_tmp83=_tmp82;struct Cyc_Absyn_Kind*_tmp85;struct Cyc_Absyn_Kind*_tmp88;_LL4E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp84=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp83;if(_tmp84->tag != 0)goto _LL50;else{_tmp85=_tmp84->f1;}}_LL4F:
 return Cyc_Absynpp_kind2string(_tmp85);_LL50: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp86=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp83;if(_tmp86->tag != 1)goto _LL52;}_LL51:
# 407
 if(Cyc_PP_tex_output){
const char*_tmp684;return(_tmp684="{?}",_tag_dyneither(_tmp684,sizeof(char),4));}else{
const char*_tmp685;return(_tmp685="?",_tag_dyneither(_tmp685,sizeof(char),2));}_LL52: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp87=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp83;if(_tmp87->tag != 2)goto _LL4D;else{_tmp88=_tmp87->f2;}}_LL53: {
const char*_tmp689;void*_tmp688[1];struct Cyc_String_pa_PrintArg_struct _tmp687;return(struct _dyneither_ptr)((_tmp687.tag=0,((_tmp687.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp88)),((_tmp688[0]=& _tmp687,Cyc_aprintf(((_tmp689="<=%s",_tag_dyneither(_tmp689,sizeof(char),5))),_tag_dyneither(_tmp688,sizeof(void*),1))))))));}_LL4D:;}
# 414
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp8E=Cyc_Absyn_compress_kb(c);void*_tmp8F=_tmp8E;struct Cyc_Absyn_Kind*_tmp91;struct Cyc_Absyn_Kind*_tmp94;_LL55: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp90=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8F;if(_tmp90->tag != 0)goto _LL57;else{_tmp91=_tmp90->f1;}}_LL56:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp91));_LL57: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp92=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8F;if(_tmp92->tag != 1)goto _LL59;}_LL58:
# 418
 if(Cyc_PP_tex_output){
const char*_tmp68A;return Cyc_PP_text_width(((_tmp68A="{?}",_tag_dyneither(_tmp68A,sizeof(char),4))),1);}else{
const char*_tmp68B;return Cyc_PP_text(((_tmp68B="?",_tag_dyneither(_tmp68B,sizeof(char),2))));}_LL59: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp93=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F;if(_tmp93->tag != 2)goto _LL54;else{_tmp94=_tmp93->f2;}}_LL5A:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp94));_LL54:;}
# 425
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
const char*_tmp68E;const char*_tmp68D;const char*_tmp68C;return Cyc_PP_egroup(((_tmp68C="<",_tag_dyneither(_tmp68C,sizeof(char),2))),((_tmp68D=">",_tag_dyneither(_tmp68D,sizeof(char),2))),((_tmp68E=",",_tag_dyneither(_tmp68E,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));}
# 429
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr _tmp9A=*tv->name;
# 433
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(tv)){
# 435
const char*_tmp68F;struct _dyneither_ptr kstring=(_tmp68F="K",_tag_dyneither(_tmp68F,sizeof(char),2));
{void*_tmp9B=Cyc_Absyn_compress_kb(tv->kind);void*_tmp9C=_tmp9B;struct Cyc_Absyn_Kind*_tmp9E;struct Cyc_Absyn_Kind*_tmpA0;_LL5C: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9C;if(_tmp9D->tag != 2)goto _LL5E;else{_tmp9E=_tmp9D->f2;}}_LL5D:
 _tmpA0=_tmp9E;goto _LL5F;_LL5E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp9F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9C;if(_tmp9F->tag != 0)goto _LL60;else{_tmpA0=_tmp9F->f1;}}_LL5F:
 kstring=Cyc_Absynpp_kind2string(_tmpA0);goto _LL5B;_LL60:;_LL61:
 goto _LL5B;_LL5B:;}{
# 441
const char*_tmp694;void*_tmp693[2];struct Cyc_String_pa_PrintArg_struct _tmp692;struct Cyc_String_pa_PrintArg_struct _tmp691;return Cyc_PP_text((struct _dyneither_ptr)((_tmp691.tag=0,((_tmp691.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_tmp9A,sizeof(char),1)),((_tmp692.tag=0,((_tmp692.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kstring),((_tmp693[0]=& _tmp692,((_tmp693[1]=& _tmp691,Cyc_aprintf(((_tmp694="`G%s%s",_tag_dyneither(_tmp694,sizeof(char),7))),_tag_dyneither(_tmp693,sizeof(void*),2)))))))))))))));};}
# 443
return Cyc_PP_text(_tmp9A);}
# 446
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmpA6=Cyc_Absyn_compress_kb(tv->kind);void*_tmpA7=_tmpA6;struct Cyc_Absyn_Kind*_tmpAB;struct Cyc_Absyn_Kind*_tmpAD;_LL63: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA8=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA7;if(_tmpA8->tag != 1)goto _LL65;}_LL64:
 goto _LL66;_LL65: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA9=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7;if(_tmpA9->tag != 0)goto _LL67;else{if((_tmpA9->f1)->kind != Cyc_Absyn_BoxKind)goto _LL67;if((_tmpA9->f1)->aliasqual != Cyc_Absyn_Aliasable)goto _LL67;}}_LL66:
 return Cyc_Absynpp_tvar2doc(tv);_LL67: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAA=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7;if(_tmpAA->tag != 2)goto _LL69;else{_tmpAB=_tmpAA->f2;}}_LL68:
 _tmpAD=_tmpAB;goto _LL6A;_LL69: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpAC=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7;if(_tmpAC->tag != 0)goto _LL62;else{_tmpAD=_tmpAC->f1;}}_LL6A: {
const char*_tmp697;struct Cyc_PP_Doc*_tmp696[3];return(_tmp696[2]=Cyc_Absynpp_kind2doc(_tmpAD),((_tmp696[1]=Cyc_PP_text(((_tmp697="::",_tag_dyneither(_tmp697,sizeof(char),3)))),((_tmp696[0]=Cyc_Absynpp_tvar2doc(tv),Cyc_PP_cat(_tag_dyneither(_tmp696,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL62:;}
# 455
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
const char*_tmp69A;const char*_tmp699;const char*_tmp698;return Cyc_PP_egroup(((_tmp698="<",_tag_dyneither(_tmp698,sizeof(char),2))),((_tmp699=">",_tag_dyneither(_tmp699,sizeof(char),2))),((_tmp69A=",",_tag_dyneither(_tmp69A,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}
# 459
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);{
const char*_tmp69D;const char*_tmp69C;const char*_tmp69B;return Cyc_PP_egroup(((_tmp69B="<",_tag_dyneither(_tmp69B,sizeof(char),2))),((_tmp69C=">",_tag_dyneither(_tmp69C,sizeof(char),2))),((_tmp69D=",",_tag_dyneither(_tmp69D,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));};}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 465
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 469
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
const char*_tmp6A0;const char*_tmp69F;const char*_tmp69E;return Cyc_PP_group(((_tmp69E="(",_tag_dyneither(_tmp69E,sizeof(char),2))),((_tmp69F=")",_tag_dyneither(_tmp69F,sizeof(char),2))),((_tmp6A0=",",_tag_dyneither(_tmp6A0,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
# 473
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
void*_tmpB9=att;_LL6C: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpBA=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpB9;if(_tmpBA->tag != 1)goto _LL6E;}_LL6D:
 goto _LL6F;_LL6E: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpBB=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpB9;if(_tmpBB->tag != 2)goto _LL70;}_LL6F:
 goto _LL71;_LL70: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpBC=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpB9;if(_tmpBC->tag != 3)goto _LL72;}_LL71:
 return Cyc_PP_nil_doc();_LL72:;_LL73:
 return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL6B:;}
# 482
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmpBD=(void*)atts->hd;void*_tmpBE=_tmpBD;_LL75: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpBF=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpBE;if(_tmpBF->tag != 1)goto _LL77;}_LL76: {
const char*_tmp6A1;return Cyc_PP_text(((_tmp6A1=" _stdcall ",_tag_dyneither(_tmp6A1,sizeof(char),11))));}_LL77: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpC0=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpBE;if(_tmpC0->tag != 2)goto _LL79;}_LL78: {
const char*_tmp6A2;return Cyc_PP_text(((_tmp6A2=" _cdecl ",_tag_dyneither(_tmp6A2,sizeof(char),9))));}_LL79: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpC1=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpBE;if(_tmpC1->tag != 3)goto _LL7B;}_LL7A: {
const char*_tmp6A3;return Cyc_PP_text(((_tmp6A3=" _fastcall ",_tag_dyneither(_tmp6A3,sizeof(char),12))));}_LL7B:;_LL7C:
 goto _LL74;_LL74:;}
# 490
return Cyc_PP_nil_doc();}
# 493
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpC5=(void*)atts2->hd;void*_tmpC6=_tmpC5;_LL7E: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpC7=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpC6;if(_tmpC7->tag != 1)goto _LL80;}_LL7F:
 goto _LL81;_LL80: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpC8=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpC6;if(_tmpC8->tag != 2)goto _LL82;}_LL81:
 goto _LL83;_LL82: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpC9=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpC6;if(_tmpC9->tag != 3)goto _LL84;}_LL83:
 goto _LL7D;_LL84:;_LL85:
 hasatt=1;goto _LL7D;_LL7D:;}}
# 502
if(!hasatt)
return Cyc_PP_nil_doc();{
const char*_tmp6AE;const char*_tmp6AD;const char*_tmp6AC;const char*_tmp6AB;const char*_tmp6AA;struct Cyc_PP_Doc*_tmp6A9[3];return(_tmp6A9[2]=
# 506
Cyc_PP_text(((_tmp6AA=")",_tag_dyneither(_tmp6AA,sizeof(char),2)))),((_tmp6A9[1]=
# 505
Cyc_PP_group(((_tmp6AD="",_tag_dyneither(_tmp6AD,sizeof(char),1))),((_tmp6AC="",_tag_dyneither(_tmp6AC,sizeof(char),1))),((_tmp6AB=" ",_tag_dyneither(_tmp6AB,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts)),((_tmp6A9[0]=
# 504
Cyc_PP_text(((_tmp6AE=" __declspec(",_tag_dyneither(_tmp6AE,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp6A9,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 509
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 513
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL86:
 return Cyc_Absynpp_noncallconv2doc(atts);case Cyc_Cyclone_Gcc_c: _LL87: {
const char*_tmp6B7;const char*_tmp6B6;const char*_tmp6B5;const char*_tmp6B4;struct Cyc_PP_Doc*_tmp6B3[2];return(_tmp6B3[1]=
Cyc_PP_group(((_tmp6B6="((",_tag_dyneither(_tmp6B6,sizeof(char),3))),((_tmp6B5="))",_tag_dyneither(_tmp6B5,sizeof(char),3))),((_tmp6B4=",",_tag_dyneither(_tmp6B4,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp6B3[0]=
# 517
Cyc_PP_text(((_tmp6B7=" __attribute__",_tag_dyneither(_tmp6B7,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp6B3,sizeof(struct Cyc_PP_Doc*),2)))));}}}
# 522
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmpD5=(void*)tms->hd;void*_tmpD6=_tmpD5;_LL8A: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpD7=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpD6;if(_tmpD7->tag != 2)goto _LL8C;}_LL8B:
 return 1;_LL8C: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpD8=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpD6;if(_tmpD8->tag != 5)goto _LL8E;}_LL8D:
# 527
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL90:
 return 0;default: _LL91:
 return Cyc_Absynpp_next_is_pointer(tms->tl);}_LL8E:;_LL8F:
# 531
 return 0;_LL89:;};}
# 535
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
# 538
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_question(){
if(!((unsigned int)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output){
const char*_tmp6B8;Cyc_Absynpp_cache_question=Cyc_PP_text_width(((_tmp6B8="{?}",_tag_dyneither(_tmp6B8,sizeof(char),4))),1);}else{
const char*_tmp6B9;Cyc_Absynpp_cache_question=Cyc_PP_text(((_tmp6B9="?",_tag_dyneither(_tmp6B9,sizeof(char),2))));}}
# 545
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 547
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output){
const char*_tmp6BA;Cyc_Absynpp_cache_lb=Cyc_PP_text_width(((_tmp6BA="{\\lb}",_tag_dyneither(_tmp6BA,sizeof(char),6))),1);}else{
const char*_tmp6BB;Cyc_Absynpp_cache_lb=Cyc_PP_text(((_tmp6BB="{",_tag_dyneither(_tmp6BB,sizeof(char),2))));}}
# 554
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 556
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output){
const char*_tmp6BC;Cyc_Absynpp_cache_rb=Cyc_PP_text_width(((_tmp6BC="{\\rb}",_tag_dyneither(_tmp6BC,sizeof(char),6))),1);}else{
const char*_tmp6BD;Cyc_Absynpp_cache_rb=Cyc_PP_text(((_tmp6BD="}",_tag_dyneither(_tmp6BD,sizeof(char),2))));}}
# 563
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 565
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output){
const char*_tmp6BE;Cyc_Absynpp_cache_dollar=Cyc_PP_text_width(((_tmp6BE="\\$",_tag_dyneither(_tmp6BE,sizeof(char),3))),1);}else{
const char*_tmp6BF;Cyc_Absynpp_cache_dollar=Cyc_PP_text(((_tmp6BF="$",_tag_dyneither(_tmp6BF,sizeof(char),2))));}}
# 572
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 574
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){
struct Cyc_PP_Doc*_tmp6C0[3];return(_tmp6C0[2]=Cyc_Absynpp_rb(),((_tmp6C0[1]=Cyc_PP_seq(sep,ss),((_tmp6C0[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp6C0,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 579
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpE2=(void*)tms->hd;void*_tmpE3=_tmpE2;struct Cyc_List_List*_tmpE8;_LL94: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpE4=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpE4->tag != 0)goto _LL96;}_LL95:
{const char*_tmp6C3;void*_tmp6C2;(_tmp6C2=0,Cyc_fprintf(Cyc_stderr,((_tmp6C3="Carray_mod ",_tag_dyneither(_tmp6C3,sizeof(char),12))),_tag_dyneither(_tmp6C2,sizeof(void*),0)));}goto _LL93;_LL96: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpE5=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpE5->tag != 1)goto _LL98;}_LL97:
{const char*_tmp6C6;void*_tmp6C5;(_tmp6C5=0,Cyc_fprintf(Cyc_stderr,((_tmp6C6="ConstArray_mod ",_tag_dyneither(_tmp6C6,sizeof(char),16))),_tag_dyneither(_tmp6C5,sizeof(void*),0)));}goto _LL93;_LL98: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpE6=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpE6->tag != 3)goto _LL9A;else{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpE7=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((void*)_tmpE6->f1);if(_tmpE7->tag != 1)goto _LL9A;else{_tmpE8=_tmpE7->f1;}}}_LL99:
# 585
{const char*_tmp6C9;void*_tmp6C8;(_tmp6C8=0,Cyc_fprintf(Cyc_stderr,((_tmp6C9="Function_mod(",_tag_dyneither(_tmp6C9,sizeof(char),14))),_tag_dyneither(_tmp6C8,sizeof(void*),0)));}
for(0;_tmpE8 != 0;_tmpE8=_tmpE8->tl){
struct _dyneither_ptr*_tmpF3=(*((struct _tuple8*)_tmpE8->hd)).f1;
if(_tmpF3 == 0){const char*_tmp6CC;void*_tmp6CB;(_tmp6CB=0,Cyc_fprintf(Cyc_stderr,((_tmp6CC="?",_tag_dyneither(_tmp6CC,sizeof(char),2))),_tag_dyneither(_tmp6CB,sizeof(void*),0)));}else{
void*_tmp6CD;(_tmp6CD=0,Cyc_fprintf(Cyc_stderr,*_tmpF3,_tag_dyneither(_tmp6CD,sizeof(void*),0)));}
if(_tmpE8->tl != 0){const char*_tmp6D0;void*_tmp6CF;(_tmp6CF=0,Cyc_fprintf(Cyc_stderr,((_tmp6D0=",",_tag_dyneither(_tmp6D0,sizeof(char),2))),_tag_dyneither(_tmp6CF,sizeof(void*),0)));}}
# 592
{const char*_tmp6D3;void*_tmp6D2;(_tmp6D2=0,Cyc_fprintf(Cyc_stderr,((_tmp6D3=") ",_tag_dyneither(_tmp6D3,sizeof(char),3))),_tag_dyneither(_tmp6D2,sizeof(void*),0)));}
goto _LL93;_LL9A: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpE9=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpE9->tag != 3)goto _LL9C;}_LL9B:
# 595
{const char*_tmp6D6;void*_tmp6D5;(_tmp6D5=0,Cyc_fprintf(Cyc_stderr,((_tmp6D6="Function_mod()",_tag_dyneither(_tmp6D6,sizeof(char),15))),_tag_dyneither(_tmp6D5,sizeof(void*),0)));}goto _LL93;_LL9C: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpEA=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpEA->tag != 5)goto _LL9E;}_LL9D:
{const char*_tmp6D9;void*_tmp6D8;(_tmp6D8=0,Cyc_fprintf(Cyc_stderr,((_tmp6D9="Attributes_mod ",_tag_dyneither(_tmp6D9,sizeof(char),16))),_tag_dyneither(_tmp6D8,sizeof(void*),0)));}goto _LL93;_LL9E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpEB=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpEB->tag != 4)goto _LLA0;}_LL9F:
{const char*_tmp6DC;void*_tmp6DB;(_tmp6DB=0,Cyc_fprintf(Cyc_stderr,((_tmp6DC="TypeParams_mod ",_tag_dyneither(_tmp6DC,sizeof(char),16))),_tag_dyneither(_tmp6DB,sizeof(void*),0)));}goto _LL93;_LLA0: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpEC=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpEC->tag != 2)goto _LL93;}_LLA1:
{const char*_tmp6DF;void*_tmp6DE;(_tmp6DE=0,Cyc_fprintf(Cyc_stderr,((_tmp6DF="Pointer_mod ",_tag_dyneither(_tmp6DF,sizeof(char),13))),_tag_dyneither(_tmp6DE,sizeof(void*),0)));}goto _LL93;_LL93:;}{
# 600
const char*_tmp6E2;void*_tmp6E1;(_tmp6E1=0,Cyc_fprintf(Cyc_stderr,((_tmp6E2="\n",_tag_dyneither(_tmp6E2,sizeof(char),2))),_tag_dyneither(_tmp6E1,sizeof(void*),0)));};}
# 603
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 605
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
const char*_tmp6E7;const char*_tmp6E6;struct Cyc_PP_Doc*_tmp6E5[3];struct Cyc_PP_Doc*p_rest=(_tmp6E5[2]=Cyc_PP_text(((_tmp6E6=")",_tag_dyneither(_tmp6E6,sizeof(char),2)))),((_tmp6E5[1]=rest,((_tmp6E5[0]=Cyc_PP_text(((_tmp6E7="(",_tag_dyneither(_tmp6E7,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6E5,sizeof(struct Cyc_PP_Doc*),3)))))));
void*_tmp105=(void*)tms->hd;void*_tmp106=_tmp105;union Cyc_Absyn_Constraint*_tmp108;struct Cyc_Absyn_Exp*_tmp10A;union Cyc_Absyn_Constraint*_tmp10B;void*_tmp10D;struct Cyc_List_List*_tmp10F;struct Cyc_List_List*_tmp111;unsigned int _tmp112;int _tmp113;void*_tmp115;union Cyc_Absyn_Constraint*_tmp116;union Cyc_Absyn_Constraint*_tmp117;union Cyc_Absyn_Constraint*_tmp118;struct Cyc_Absyn_Tqual _tmp119;_LLA3: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp107=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp107->tag != 0)goto _LLA5;else{_tmp108=_tmp107->f1;}}_LLA4:
# 611
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6EC;const char*_tmp6EB;struct Cyc_PP_Doc*_tmp6EA[2];return(_tmp6EA[1]=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp108)?
Cyc_PP_text(((_tmp6EB="[]@zeroterm",_tag_dyneither(_tmp6EB,sizeof(char),12)))): Cyc_PP_text(((_tmp6EC="[]",_tag_dyneither(_tmp6EC,sizeof(char),3)))),((_tmp6EA[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6EA,sizeof(struct Cyc_PP_Doc*),2)))));};_LLA5: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp109=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp109->tag != 1)goto _LLA7;else{_tmp10A=_tmp109->f1;_tmp10B=_tmp109->f2;}}_LLA6:
# 615
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6F3;const char*_tmp6F2;const char*_tmp6F1;struct Cyc_PP_Doc*_tmp6F0[4];return(_tmp6F0[3]=
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp10B)?Cyc_PP_text(((_tmp6F1="]@zeroterm",_tag_dyneither(_tmp6F1,sizeof(char),11)))): Cyc_PP_text(((_tmp6F2="]",_tag_dyneither(_tmp6F2,sizeof(char),2)))),((_tmp6F0[2]=
# 616
Cyc_Absynpp_exp2doc(_tmp10A),((_tmp6F0[1]=Cyc_PP_text(((_tmp6F3="[",_tag_dyneither(_tmp6F3,sizeof(char),2)))),((_tmp6F0[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F0,sizeof(struct Cyc_PP_Doc*),4)))))))));};_LLA7: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp10C=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp10C->tag != 3)goto _LLA9;else{_tmp10D=(void*)_tmp10C->f1;}}_LLA8:
# 619
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmp121=_tmp10D;struct Cyc_List_List*_tmp123;int _tmp124;struct Cyc_Absyn_VarargInfo*_tmp125;void*_tmp126;struct Cyc_List_List*_tmp127;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_List_List*_tmp12B;unsigned int _tmp12C;_LLB0: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp122=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp121;if(_tmp122->tag != 1)goto _LLB2;else{_tmp123=_tmp122->f1;_tmp124=_tmp122->f2;_tmp125=_tmp122->f3;_tmp126=(void*)_tmp122->f4;_tmp127=_tmp122->f5;_tmp128=_tmp122->f6;_tmp129=_tmp122->f7;}}_LLB1: {
# 622
struct Cyc_PP_Doc*_tmp6F4[2];return(_tmp6F4[1]=Cyc_Absynpp_funargs2doc(_tmp123,_tmp124,_tmp125,_tmp126,_tmp127,_tmp128,_tmp129),((_tmp6F4[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F4,sizeof(struct Cyc_PP_Doc*),2)))));}_LLB2: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp12A=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp121;if(_tmp12A->tag != 0)goto _LLAF;else{_tmp12B=_tmp12A->f1;_tmp12C=_tmp12A->f2;}}_LLB3: {
# 624
const char*_tmp6FB;const char*_tmp6FA;const char*_tmp6F9;struct Cyc_PP_Doc*_tmp6F8[2];return(_tmp6F8[1]=Cyc_PP_group(((_tmp6FB="(",_tag_dyneither(_tmp6FB,sizeof(char),2))),((_tmp6FA=")",_tag_dyneither(_tmp6FA,sizeof(char),2))),((_tmp6F9=",",_tag_dyneither(_tmp6F9,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp12B)),((_tmp6F8[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F8,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAF:;};_LLA9: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp10E=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp10E->tag != 5)goto _LLAB;else{_tmp10F=_tmp10E->f2;}}_LLAA:
# 627
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLB4:
# 629
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp6FC[2];return(_tmp6FC[1]=Cyc_Absynpp_atts2doc(_tmp10F),((_tmp6FC[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6FC,sizeof(struct Cyc_PP_Doc*),2)))));};case Cyc_Cyclone_Vc_c: _LLB5:
# 632
 if(Cyc_Absynpp_next_is_pointer(tms->tl)){
struct Cyc_PP_Doc*_tmp6FD[2];return(_tmp6FD[1]=rest,((_tmp6FD[0]=Cyc_Absynpp_callconv2doc(_tmp10F),Cyc_PP_cat(_tag_dyneither(_tmp6FD,sizeof(struct Cyc_PP_Doc*),2)))));}
return rest;}_LLAB: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp110=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp110->tag != 4)goto _LLAD;else{_tmp111=_tmp110->f1;_tmp112=_tmp110->f2;_tmp113=_tmp110->f3;}}_LLAC:
# 637
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp113){
struct Cyc_PP_Doc*_tmp6FE[2];return(_tmp6FE[1]=Cyc_Absynpp_ktvars2doc(_tmp111),((_tmp6FE[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6FE,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 641
struct Cyc_PP_Doc*_tmp6FF[2];return(_tmp6FF[1]=Cyc_Absynpp_tvars2doc(_tmp111),((_tmp6FF[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6FF,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAD: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp114=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp114->tag != 2)goto _LLA2;else{_tmp115=(_tmp114->f1).rgn;_tmp116=(_tmp114->f1).nullable;_tmp117=(_tmp114->f1).bounds;_tmp118=(_tmp114->f1).zero_term;_tmp119=_tmp114->f2;}}_LLAE: {
# 645
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(_tmp119);
{void*_tmp136=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp117);void*_tmp137=_tmp136;struct Cyc_Absyn_Exp*_tmp13A;_LLB8: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp138=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp137;if(_tmp138->tag != 0)goto _LLBA;}_LLB9:
 ptr=Cyc_Absynpp_question();goto _LLB7;_LLBA: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp139=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp137;if(_tmp139->tag != 1)goto _LLB7;else{_tmp13A=_tmp139->f1;}}_LLBB:
# 653
{const char*_tmp701;const char*_tmp700;ptr=Cyc_PP_text(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp116)?(_tmp701="*",_tag_dyneither(_tmp701,sizeof(char),2)):((_tmp700="@",_tag_dyneither(_tmp700,sizeof(char),2))));}{
struct _tuple11 _tmp13D=Cyc_Evexp_eval_const_uint_exp(_tmp13A);unsigned int _tmp13F;int _tmp140;struct _tuple11 _tmp13E=_tmp13D;_tmp13F=_tmp13E.f1;_tmp140=_tmp13E.f2;
if(!_tmp140  || _tmp13F != 1){
struct Cyc_PP_Doc*_tmp702[4];ptr=((_tmp702[3]=Cyc_Absynpp_rb(),((_tmp702[2]=Cyc_Absynpp_exp2doc(_tmp13A),((_tmp702[1]=Cyc_Absynpp_lb(),((_tmp702[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp702,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB7;};_LLB7:;}
# 659
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp118)){
const char*_tmp703;ztd=Cyc_PP_text(((_tmp703="@zeroterm",_tag_dyneither(_tmp703,sizeof(char),10))));}else{
if(is_char_ptr  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp118)){
const char*_tmp704;ztd=Cyc_PP_text(((_tmp704="@nozeroterm",_tag_dyneither(_tmp704,sizeof(char),12))));}}}
# 665
{void*_tmp144=Cyc_Tcutil_compress(_tmp115);void*_tmp145=_tmp144;_LLBD: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp146=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp145;if(_tmp146->tag != 20)goto _LLBF;}_LLBE:
 goto _LLBC;_LLBF:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp147=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp145;if(_tmp147->tag != 1)goto _LLC1;}if(!Cyc_Absynpp_print_for_cycdoc)goto _LLC1;_LLC0:
 goto _LLBC;_LLC1:;_LLC2:
 rgd=Cyc_Absynpp_rgn2doc(_tmp115);_LLBC:;}{
# 670
const char*_tmp705;struct Cyc_PP_Doc*spacer1=tqd != mt  && (ztd != mt  || rgd != mt)?Cyc_PP_text(((_tmp705=" ",_tag_dyneither(_tmp705,sizeof(char),2)))): mt;
const char*_tmp706;struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(((_tmp706=" ",_tag_dyneither(_tmp706,sizeof(char),2)))): mt;
struct Cyc_PP_Doc*_tmp707[7];return(_tmp707[6]=rest,((_tmp707[5]=spacer2,((_tmp707[4]=tqd,((_tmp707[3]=spacer1,((_tmp707[2]=rgd,((_tmp707[1]=ztd,((_tmp707[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp707,sizeof(struct Cyc_PP_Doc*),7)))))))))))))));};}_LLA2:;};}
# 676
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp14E=Cyc_Tcutil_compress(t);void*_tmp14F=_tmp14E;_LLC4: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp150=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp14F;if(_tmp150->tag != 20)goto _LLC6;}_LLC5: {
const char*_tmp708;return Cyc_PP_text(((_tmp708="`H",_tag_dyneither(_tmp708,sizeof(char),3))));}_LLC6: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp151=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp14F;if(_tmp151->tag != 21)goto _LLC8;}_LLC7: {
const char*_tmp709;return Cyc_PP_text(((_tmp709="`U",_tag_dyneither(_tmp709,sizeof(char),3))));}_LLC8: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp152=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp14F;if(_tmp152->tag != 22)goto _LLCA;}_LLC9: {
const char*_tmp70A;return Cyc_PP_text(((_tmp70A="`RC",_tag_dyneither(_tmp70A,sizeof(char),4))));}_LLCA:;_LLCB:
 return Cyc_Absynpp_ntyp2doc(t);_LLC3:;}
# 686
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 690
void*_tmp156=Cyc_Tcutil_compress(t);void*_tmp157=_tmp156;void*_tmp159;struct Cyc_List_List*_tmp15B;_LLCD: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp158=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp157;if(_tmp158->tag != 23)goto _LLCF;else{_tmp159=(void*)_tmp158->f1;}}_LLCE:
{struct Cyc_List_List*_tmp70B;*rgions=((_tmp70B=_cycalloc(sizeof(*_tmp70B)),((_tmp70B->hd=Cyc_Absynpp_rgn2doc(_tmp159),((_tmp70B->tl=*rgions,_tmp70B))))));}goto _LLCC;_LLCF: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp15A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp157;if(_tmp15A->tag != 24)goto _LLD1;else{_tmp15B=_tmp15A->f1;}}_LLD0:
# 693
 for(0;_tmp15B != 0;_tmp15B=_tmp15B->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp15B->hd);}
# 696
goto _LLCC;_LLD1:;_LLD2:
{struct Cyc_List_List*_tmp70C;*effects=((_tmp70C=_cycalloc(sizeof(*_tmp70C)),((_tmp70C->hd=Cyc_Absynpp_typ2doc(t),((_tmp70C->tl=*effects,_tmp70C))))));}goto _LLCC;_LLCC:;}
# 701
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0  && effects != 0){
const char*_tmp70F;const char*_tmp70E;const char*_tmp70D;return Cyc_PP_group(((_tmp70D="",_tag_dyneither(_tmp70D,sizeof(char),1))),((_tmp70E="",_tag_dyneither(_tmp70E,sizeof(char),1))),((_tmp70F="+",_tag_dyneither(_tmp70F,sizeof(char),2))),effects);}else{
# 709
const char*_tmp710;struct Cyc_PP_Doc*_tmp161=Cyc_Absynpp_group_braces(((_tmp710=",",_tag_dyneither(_tmp710,sizeof(char),2))),rgions);
struct Cyc_List_List*_tmp714;const char*_tmp713;const char*_tmp712;const char*_tmp711;return Cyc_PP_group(((_tmp711="",_tag_dyneither(_tmp711,sizeof(char),1))),((_tmp712="",_tag_dyneither(_tmp712,sizeof(char),1))),((_tmp713="+",_tag_dyneither(_tmp713,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((_tmp714=_cycalloc(sizeof(*_tmp714)),((_tmp714->hd=_tmp161,((_tmp714->tl=0,_tmp714))))))));}}
# 714
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
switch(k){case Cyc_Absyn_StructA: _LLD3: {
const char*_tmp715;return Cyc_PP_text(((_tmp715="struct ",_tag_dyneither(_tmp715,sizeof(char),8))));}case Cyc_Absyn_UnionA: _LLD4: {
const char*_tmp716;return Cyc_PP_text(((_tmp716="union ",_tag_dyneither(_tmp716,sizeof(char),7))));}}}
# 722
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp169=t;struct Cyc_Core_Opt*_tmp16F;void*_tmp170;int _tmp171;struct Cyc_Core_Opt*_tmp172;struct Cyc_Absyn_Tvar*_tmp174;union Cyc_Absyn_DatatypeInfoU _tmp176;struct Cyc_List_List*_tmp177;union Cyc_Absyn_DatatypeFieldInfoU _tmp179;struct Cyc_List_List*_tmp17A;enum Cyc_Absyn_Sign _tmp17C;enum Cyc_Absyn_Size_of _tmp17D;int _tmp17F;struct Cyc_List_List*_tmp181;union Cyc_Absyn_AggrInfoU _tmp183;struct Cyc_List_List*_tmp184;enum Cyc_Absyn_AggrKind _tmp186;struct Cyc_List_List*_tmp187;struct Cyc_List_List*_tmp189;struct _tuple0*_tmp18B;struct Cyc_Absyn_Exp*_tmp18D;struct _tuple0*_tmp18F;struct Cyc_List_List*_tmp190;struct Cyc_Absyn_Typedefdecl*_tmp191;void*_tmp193;void*_tmp195;void*_tmp196;void*_tmp198;void*_tmp19D;struct Cyc_Absyn_Aggrdecl*_tmp1A2;struct Cyc_Absyn_Enumdecl*_tmp1A5;struct Cyc_Absyn_Datatypedecl*_tmp1A8;_LLD7: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp16A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp169;if(_tmp16A->tag != 8)goto _LLD9;}_LLD8: {
# 726
const char*_tmp717;return Cyc_PP_text(((_tmp717="[[[array]]]",_tag_dyneither(_tmp717,sizeof(char),12))));}_LLD9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp16B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp169;if(_tmp16B->tag != 9)goto _LLDB;}_LLDA:
 return Cyc_PP_nil_doc();_LLDB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp16C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp169;if(_tmp16C->tag != 5)goto _LLDD;}_LLDC:
 return Cyc_PP_nil_doc();_LLDD: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp16D=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp169;if(_tmp16D->tag != 0)goto _LLDF;}_LLDE:
# 730
{const char*_tmp718;s=Cyc_PP_text(((_tmp718="void",_tag_dyneither(_tmp718,sizeof(char),5))));}goto _LLD6;_LLDF: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp16E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp169;if(_tmp16E->tag != 1)goto _LLE1;else{_tmp16F=_tmp16E->f1;_tmp170=(void*)_tmp16E->f2;_tmp171=_tmp16E->f3;_tmp172=_tmp16E->f4;}}_LLE0:
# 732
 if(_tmp170 != 0)
# 734
return Cyc_Absynpp_ntyp2doc(_tmp170);else{
# 736
const char*_tmp719;struct _dyneither_ptr kindstring=_tmp16F == 0?(_tmp719="K",_tag_dyneither(_tmp719,sizeof(char),2)): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)_tmp16F->v);
const char*_tmp71E;void*_tmp71D[2];struct Cyc_String_pa_PrintArg_struct _tmp71C;struct Cyc_Int_pa_PrintArg_struct _tmp71B;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp71B.tag=1,((_tmp71B.f1=(unsigned long)_tmp171,((_tmp71C.tag=0,((_tmp71C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kindstring),((_tmp71D[0]=& _tmp71C,((_tmp71D[1]=& _tmp71B,Cyc_aprintf(((_tmp71E="`E%s%d",_tag_dyneither(_tmp71E,sizeof(char),7))),_tag_dyneither(_tmp71D,sizeof(void*),2)))))))))))))));}
# 753 "absynpp.cyc"
goto _LLD6;_LLE1: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp173=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp169;if(_tmp173->tag != 2)goto _LLE3;else{_tmp174=_tmp173->f1;}}_LLE2:
# 755
 s=Cyc_Absynpp_tvar2doc(_tmp174);
if(Cyc_Absynpp_print_all_kinds){
const char*_tmp721;struct Cyc_PP_Doc*_tmp720[3];s=((_tmp720[2]=Cyc_Absynpp_kindbound2doc(_tmp174->kind),((_tmp720[1]=Cyc_PP_text(((_tmp721="::",_tag_dyneither(_tmp721,sizeof(char),3)))),((_tmp720[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp720,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 759
goto _LLD6;_LLE3: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp175=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp169;if(_tmp175->tag != 3)goto _LLE5;else{_tmp176=(_tmp175->f1).datatype_info;_tmp177=(_tmp175->f1).targs;}}_LLE4:
# 761
{union Cyc_Absyn_DatatypeInfoU _tmp1B2=_tmp176;struct _tuple0*_tmp1B3;int _tmp1B4;struct _tuple0*_tmp1B5;int _tmp1B6;_LL112: if((_tmp1B2.UnknownDatatype).tag != 1)goto _LL114;_tmp1B3=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1B2.UnknownDatatype).val).name;_tmp1B4=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1B2.UnknownDatatype).val).is_extensible;_LL113:
# 763
 _tmp1B5=_tmp1B3;_tmp1B6=_tmp1B4;goto _LL115;_LL114: if((_tmp1B2.KnownDatatype).tag != 2)goto _LL111;_tmp1B5=(*((struct Cyc_Absyn_Datatypedecl**)(_tmp1B2.KnownDatatype).val))->name;_tmp1B6=(*((struct Cyc_Absyn_Datatypedecl**)(_tmp1B2.KnownDatatype).val))->is_extensible;_LL115: {
# 765
const char*_tmp722;struct Cyc_PP_Doc*_tmp1B7=Cyc_PP_text(((_tmp722="datatype ",_tag_dyneither(_tmp722,sizeof(char),10))));
const char*_tmp723;struct Cyc_PP_Doc*_tmp1B8=_tmp1B6?Cyc_PP_text(((_tmp723="@extensible ",_tag_dyneither(_tmp723,sizeof(char),13)))): Cyc_PP_nil_doc();
{struct Cyc_PP_Doc*_tmp724[4];s=((_tmp724[3]=Cyc_Absynpp_tps2doc(_tmp177),((_tmp724[2]=Cyc_Absynpp_qvar2doc(_tmp1B5),((_tmp724[1]=_tmp1B7,((_tmp724[0]=_tmp1B8,Cyc_PP_cat(_tag_dyneither(_tmp724,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL111;}_LL111:;}
# 770
goto _LLD6;_LLE5: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp178=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp169;if(_tmp178->tag != 4)goto _LLE7;else{_tmp179=(_tmp178->f1).field_info;_tmp17A=(_tmp178->f1).targs;}}_LLE6:
# 772
{union Cyc_Absyn_DatatypeFieldInfoU _tmp1BC=_tmp179;struct _tuple0*_tmp1BD;struct _tuple0*_tmp1BE;int _tmp1BF;struct _tuple0*_tmp1C0;int _tmp1C1;struct _tuple0*_tmp1C2;_LL117: if((_tmp1BC.UnknownDatatypefield).tag != 1)goto _LL119;_tmp1BD=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1BC.UnknownDatatypefield).val).datatype_name;_tmp1BE=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1BC.UnknownDatatypefield).val).field_name;_tmp1BF=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1BC.UnknownDatatypefield).val).is_extensible;_LL118:
# 774
 _tmp1C0=_tmp1BD;_tmp1C1=_tmp1BF;_tmp1C2=_tmp1BE;goto _LL11A;_LL119: if((_tmp1BC.KnownDatatypefield).tag != 2)goto _LL116;_tmp1C0=(((struct _tuple1)(_tmp1BC.KnownDatatypefield).val).f1)->name;_tmp1C1=(((struct _tuple1)(_tmp1BC.KnownDatatypefield).val).f1)->is_extensible;_tmp1C2=(((struct _tuple1)(_tmp1BC.KnownDatatypefield).val).f2)->name;_LL11A: {
# 777
const char*_tmp726;const char*_tmp725;struct Cyc_PP_Doc*_tmp1C3=Cyc_PP_text(_tmp1C1?(_tmp726="@extensible datatype ",_tag_dyneither(_tmp726,sizeof(char),22)):((_tmp725="datatype ",_tag_dyneither(_tmp725,sizeof(char),10))));
{const char*_tmp729;struct Cyc_PP_Doc*_tmp728[4];s=((_tmp728[3]=Cyc_Absynpp_qvar2doc(_tmp1C2),((_tmp728[2]=Cyc_PP_text(((_tmp729=".",_tag_dyneither(_tmp729,sizeof(char),2)))),((_tmp728[1]=Cyc_Absynpp_qvar2doc(_tmp1C0),((_tmp728[0]=_tmp1C3,Cyc_PP_cat(_tag_dyneither(_tmp728,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL116;}_LL116:;}
# 781
goto _LLD6;_LLE7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp17B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp169;if(_tmp17B->tag != 6)goto _LLE9;else{_tmp17C=_tmp17B->f1;_tmp17D=_tmp17B->f2;}}_LLE8: {
# 783
struct _dyneither_ptr sns;
struct _dyneither_ptr ts;
switch(_tmp17C){case Cyc_Absyn_None: _LL11B:
 goto _LL11C;case Cyc_Absyn_Signed: _LL11C:
{const char*_tmp72A;sns=((_tmp72A="",_tag_dyneither(_tmp72A,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL11D:
{const char*_tmp72B;sns=((_tmp72B="unsigned ",_tag_dyneither(_tmp72B,sizeof(char),10)));}break;}
# 790
switch(_tmp17D){case Cyc_Absyn_Char_sz: _LL11F:
# 792
 switch(_tmp17C){case Cyc_Absyn_None: _LL121:
{const char*_tmp72C;sns=((_tmp72C="",_tag_dyneither(_tmp72C,sizeof(char),1)));}break;case Cyc_Absyn_Signed: _LL122:
{const char*_tmp72D;sns=((_tmp72D="signed ",_tag_dyneither(_tmp72D,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL123:
{const char*_tmp72E;sns=((_tmp72E="unsigned ",_tag_dyneither(_tmp72E,sizeof(char),10)));}break;}
# 797
{const char*_tmp72F;ts=((_tmp72F="char",_tag_dyneither(_tmp72F,sizeof(char),5)));}
break;case Cyc_Absyn_Short_sz: _LL120:
{const char*_tmp730;ts=((_tmp730="short",_tag_dyneither(_tmp730,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz: _LL125:
{const char*_tmp731;ts=((_tmp731="int",_tag_dyneither(_tmp731,sizeof(char),4)));}break;case Cyc_Absyn_Long_sz: _LL126:
{const char*_tmp732;ts=((_tmp732="long",_tag_dyneither(_tmp732,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL127:
# 803
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL129:
{const char*_tmp733;ts=((_tmp733="long long",_tag_dyneither(_tmp733,sizeof(char),10)));}break;case Cyc_Cyclone_Vc_c: _LL12A:
{const char*_tmp734;ts=((_tmp734="__int64",_tag_dyneither(_tmp734,sizeof(char),8)));}break;}
# 807
break;}
# 809
{const char*_tmp739;void*_tmp738[2];struct Cyc_String_pa_PrintArg_struct _tmp737;struct Cyc_String_pa_PrintArg_struct _tmp736;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp736.tag=0,((_tmp736.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts),((_tmp737.tag=0,((_tmp737.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns),((_tmp738[0]=& _tmp737,((_tmp738[1]=& _tmp736,Cyc_aprintf(((_tmp739="%s%s",_tag_dyneither(_tmp739,sizeof(char),5))),_tag_dyneither(_tmp738,sizeof(void*),2)))))))))))))));}
goto _LLD6;}_LLE9: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp17E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp169;if(_tmp17E->tag != 7)goto _LLEB;else{_tmp17F=_tmp17E->f1;}}_LLEA:
# 812
 if(_tmp17F == 0){
const char*_tmp73A;s=Cyc_PP_text(((_tmp73A="float",_tag_dyneither(_tmp73A,sizeof(char),6))));}else{
if(_tmp17F == 1){
const char*_tmp73B;s=Cyc_PP_text(((_tmp73B="double",_tag_dyneither(_tmp73B,sizeof(char),7))));}else{
# 817
const char*_tmp73C;s=Cyc_PP_text(((_tmp73C="long double",_tag_dyneither(_tmp73C,sizeof(char),12))));}}
goto _LLD6;_LLEB: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp180=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp169;if(_tmp180->tag != 10)goto _LLED;else{_tmp181=_tmp180->f1;}}_LLEC:
# 820
{struct Cyc_PP_Doc*_tmp73D[2];s=((_tmp73D[1]=Cyc_Absynpp_args2doc(_tmp181),((_tmp73D[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp73D,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD6;_LLED: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp182=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp169;if(_tmp182->tag != 11)goto _LLEF;else{_tmp183=(_tmp182->f1).aggr_info;_tmp184=(_tmp182->f1).targs;}}_LLEE: {
# 823
struct _tuple10 _tmp1DB=Cyc_Absyn_aggr_kinded_name(_tmp183);enum Cyc_Absyn_AggrKind _tmp1DD;struct _tuple0*_tmp1DE;struct _tuple10 _tmp1DC=_tmp1DB;_tmp1DD=_tmp1DC.f1;_tmp1DE=_tmp1DC.f2;
{struct Cyc_PP_Doc*_tmp73E[3];s=((_tmp73E[2]=Cyc_Absynpp_tps2doc(_tmp184),((_tmp73E[1]=Cyc_Absynpp_qvar2doc(_tmp1DE),((_tmp73E[0]=Cyc_Absynpp_aggr_kind2doc(_tmp1DD),Cyc_PP_cat(_tag_dyneither(_tmp73E,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLD6;}_LLEF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp185=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp169;if(_tmp185->tag != 12)goto _LLF1;else{_tmp186=_tmp185->f1;_tmp187=_tmp185->f2;}}_LLF0:
# 827
{struct Cyc_PP_Doc*_tmp73F[4];s=((_tmp73F[3]=
# 829
Cyc_Absynpp_rb(),((_tmp73F[2]=
# 828
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp187)),((_tmp73F[1]=
# 827
Cyc_Absynpp_lb(),((_tmp73F[0]=Cyc_Absynpp_aggr_kind2doc(_tmp186),Cyc_PP_cat(_tag_dyneither(_tmp73F,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 830
goto _LLD6;_LLF1: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp188=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp169;if(_tmp188->tag != 14)goto _LLF3;else{_tmp189=_tmp188->f1;}}_LLF2:
# 832
{const char*_tmp742;struct Cyc_PP_Doc*_tmp741[4];s=((_tmp741[3]=Cyc_Absynpp_rb(),((_tmp741[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp189)),((_tmp741[1]=Cyc_Absynpp_lb(),((_tmp741[0]=Cyc_PP_text(((_tmp742="enum ",_tag_dyneither(_tmp742,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp741,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLD6;_LLF3: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp18A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp169;if(_tmp18A->tag != 13)goto _LLF5;else{_tmp18B=_tmp18A->f1;}}_LLF4:
# 834
{const char*_tmp745;struct Cyc_PP_Doc*_tmp744[2];s=((_tmp744[1]=Cyc_Absynpp_qvar2doc(_tmp18B),((_tmp744[0]=Cyc_PP_text(((_tmp745="enum ",_tag_dyneither(_tmp745,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp744,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLD6;_LLF5: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp18C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp169;if(_tmp18C->tag != 18)goto _LLF7;else{_tmp18D=_tmp18C->f1;}}_LLF6:
# 836
{const char*_tmp74A;const char*_tmp749;struct Cyc_PP_Doc*_tmp748[3];s=((_tmp748[2]=Cyc_PP_text(((_tmp749=")",_tag_dyneither(_tmp749,sizeof(char),2)))),((_tmp748[1]=Cyc_Absynpp_exp2doc(_tmp18D),((_tmp748[0]=Cyc_PP_text(((_tmp74A="valueof_t(",_tag_dyneither(_tmp74A,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp748,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LLF7: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp18E=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp169;if(_tmp18E->tag != 17)goto _LLF9;else{_tmp18F=_tmp18E->f1;_tmp190=_tmp18E->f2;_tmp191=_tmp18E->f3;}}_LLF8:
# 843
{struct Cyc_PP_Doc*_tmp74B[2];s=((_tmp74B[1]=Cyc_Absynpp_tps2doc(_tmp190),((_tmp74B[0]=Cyc_Absynpp_qvar2doc(_tmp18F),Cyc_PP_cat(_tag_dyneither(_tmp74B,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD6;_LLF9: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp192=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp169;if(_tmp192->tag != 15)goto _LLFB;else{_tmp193=(void*)_tmp192->f1;}}_LLFA:
# 846
{const char*_tmp750;const char*_tmp74F;struct Cyc_PP_Doc*_tmp74E[3];s=((_tmp74E[2]=Cyc_PP_text(((_tmp74F=">",_tag_dyneither(_tmp74F,sizeof(char),2)))),((_tmp74E[1]=Cyc_Absynpp_rgn2doc(_tmp193),((_tmp74E[0]=Cyc_PP_text(((_tmp750="region_t<",_tag_dyneither(_tmp750,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp74E,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LLFB: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp194=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp169;if(_tmp194->tag != 16)goto _LLFD;else{_tmp195=(void*)_tmp194->f1;_tmp196=(void*)_tmp194->f2;}}_LLFC:
# 848
{const char*_tmp757;const char*_tmp756;const char*_tmp755;struct Cyc_PP_Doc*_tmp754[5];s=((_tmp754[4]=
Cyc_PP_text(((_tmp755=">",_tag_dyneither(_tmp755,sizeof(char),2)))),((_tmp754[3]=
# 848
Cyc_Absynpp_rgn2doc(_tmp196),((_tmp754[2]=Cyc_PP_text(((_tmp756=",",_tag_dyneither(_tmp756,sizeof(char),2)))),((_tmp754[1]=Cyc_Absynpp_rgn2doc(_tmp195),((_tmp754[0]=Cyc_PP_text(((_tmp757="dynregion_t<",_tag_dyneither(_tmp757,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp754,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LLD6;_LLFD: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp197=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp169;if(_tmp197->tag != 19)goto _LLFF;else{_tmp198=(void*)_tmp197->f1;}}_LLFE:
# 851
{const char*_tmp75C;const char*_tmp75B;struct Cyc_PP_Doc*_tmp75A[3];s=((_tmp75A[2]=Cyc_PP_text(((_tmp75B=">",_tag_dyneither(_tmp75B,sizeof(char),2)))),((_tmp75A[1]=Cyc_Absynpp_typ2doc(_tmp198),((_tmp75A[0]=Cyc_PP_text(((_tmp75C="tag_t<",_tag_dyneither(_tmp75C,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp75A,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LLFF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp199=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp169;if(_tmp199->tag != 21)goto _LL101;}_LL100:
 goto _LL102;_LL101: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp19A=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp169;if(_tmp19A->tag != 20)goto _LL103;}_LL102:
 goto _LL104;_LL103: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp19B=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp169;if(_tmp19B->tag != 22)goto _LL105;}_LL104:
# 855
 s=Cyc_Absynpp_rgn2doc(t);goto _LLD6;_LL105: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp19C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp169;if(_tmp19C->tag != 25)goto _LL107;else{_tmp19D=(void*)_tmp19C->f1;}}_LL106:
# 857
{const char*_tmp761;const char*_tmp760;struct Cyc_PP_Doc*_tmp75F[3];s=((_tmp75F[2]=Cyc_PP_text(((_tmp760=")",_tag_dyneither(_tmp760,sizeof(char),2)))),((_tmp75F[1]=Cyc_Absynpp_typ2doc(_tmp19D),((_tmp75F[0]=Cyc_PP_text(((_tmp761="regions(",_tag_dyneither(_tmp761,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp75F,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LL107: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp19E=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp169;if(_tmp19E->tag != 23)goto _LL109;}_LL108:
 goto _LL10A;_LL109: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp19F=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp169;if(_tmp19F->tag != 24)goto _LL10B;}_LL10A:
# 860
 s=Cyc_Absynpp_eff2doc(t);goto _LLD6;_LL10B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1A0=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp169;if(_tmp1A0->tag != 26)goto _LL10D;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp1A1=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp1A0->f1)->r;if(_tmp1A1->tag != 0)goto _LL10D;else{_tmp1A2=_tmp1A1->f1;}}}_LL10C:
# 862
 s=Cyc_Absynpp_aggrdecl2doc(_tmp1A2);goto _LLD6;_LL10D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1A3=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp169;if(_tmp1A3->tag != 26)goto _LL10F;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp1A4=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp1A3->f1)->r;if(_tmp1A4->tag != 1)goto _LL10F;else{_tmp1A5=_tmp1A4->f1;}}}_LL10E:
# 864
 s=Cyc_Absynpp_enumdecl2doc(_tmp1A5);goto _LLD6;_LL10F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1A6=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp169;if(_tmp1A6->tag != 26)goto _LLD6;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp1A7=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp1A6->f1)->r;if(_tmp1A7->tag != 2)goto _LLD6;else{_tmp1A8=_tmp1A7->f1;}}}_LL110:
# 866
 s=Cyc_Absynpp_datatypedecl2doc(_tmp1A8);goto _LLD6;_LLD6:;}
# 868
return s;}
# 871
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _dyneither_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple16{void*f1;void*f2;};
# 875
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
void*_tmp1F7;void*_tmp1F8;struct _tuple16*_tmp1F6=cmp;_tmp1F7=_tmp1F6->f1;_tmp1F8=_tmp1F6->f2;{
const char*_tmp764;struct Cyc_PP_Doc*_tmp763[3];return(_tmp763[2]=Cyc_Absynpp_rgn2doc(_tmp1F8),((_tmp763[1]=Cyc_PP_text(((_tmp764=" > ",_tag_dyneither(_tmp764,sizeof(char),4)))),((_tmp763[0]=Cyc_Absynpp_rgn2doc(_tmp1F7),Cyc_PP_cat(_tag_dyneither(_tmp763,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 880
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
const char*_tmp767;const char*_tmp766;const char*_tmp765;return Cyc_PP_group(((_tmp765="",_tag_dyneither(_tmp765,sizeof(char),1))),((_tmp766="",_tag_dyneither(_tmp766,sizeof(char),1))),((_tmp767=",",_tag_dyneither(_tmp767,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
# 884
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _dyneither_ptr*_tmp1FE=(*t).f1;
struct Cyc_Core_Opt*_tmp768;struct Cyc_Core_Opt*dopt=_tmp1FE == 0?0:((_tmp768=_cycalloc(sizeof(*_tmp768)),((_tmp768->v=Cyc_PP_text(*_tmp1FE),_tmp768))));
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 890
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 894
struct Cyc_List_List*_tmp200=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs){
const char*_tmp76B;struct Cyc_List_List*_tmp76A;_tmp200=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp200,((_tmp76A=_cycalloc(sizeof(*_tmp76A)),((_tmp76A->hd=Cyc_PP_text(((_tmp76B="...",_tag_dyneither(_tmp76B,sizeof(char),4)))),((_tmp76A->tl=0,_tmp76A)))))));}else{
if(cyc_varargs != 0){
const char*_tmp774;const char*_tmp773;const char*_tmp772;struct _tuple8*_tmp771;struct Cyc_PP_Doc*_tmp770[3];struct Cyc_PP_Doc*_tmp203=(_tmp770[2]=
# 901
Cyc_Absynpp_funarg2doc(((_tmp771=_cycalloc(sizeof(*_tmp771)),((_tmp771->f1=cyc_varargs->name,((_tmp771->f2=cyc_varargs->tq,((_tmp771->f3=cyc_varargs->type,_tmp771))))))))),((_tmp770[1]=
# 900
cyc_varargs->inject?Cyc_PP_text(((_tmp772=" inject ",_tag_dyneither(_tmp772,sizeof(char),9)))): Cyc_PP_text(((_tmp773=" ",_tag_dyneither(_tmp773,sizeof(char),2)))),((_tmp770[0]=
# 899
Cyc_PP_text(((_tmp774="...",_tag_dyneither(_tmp774,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp770,sizeof(struct Cyc_PP_Doc*),3)))))));
# 903
struct Cyc_List_List*_tmp775;_tmp200=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp200,((_tmp775=_cycalloc(sizeof(*_tmp775)),((_tmp775->hd=_tmp203,((_tmp775->tl=0,_tmp775)))))));}}{
# 905
const char*_tmp778;const char*_tmp777;const char*_tmp776;struct Cyc_PP_Doc*_tmp20A=Cyc_PP_group(((_tmp776="",_tag_dyneither(_tmp776,sizeof(char),1))),((_tmp777="",_tag_dyneither(_tmp777,sizeof(char),1))),((_tmp778=",",_tag_dyneither(_tmp778,sizeof(char),2))),_tmp200);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){
const char*_tmp77B;struct Cyc_PP_Doc*_tmp77A[3];_tmp20A=((_tmp77A[2]=Cyc_Absynpp_eff2doc(effopt),((_tmp77A[1]=Cyc_PP_text(((_tmp77B=";",_tag_dyneither(_tmp77B,sizeof(char),2)))),((_tmp77A[0]=_tmp20A,Cyc_PP_cat(_tag_dyneither(_tmp77A,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){
const char*_tmp77E;struct Cyc_PP_Doc*_tmp77D[3];_tmp20A=((_tmp77D[2]=Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp77D[1]=Cyc_PP_text(((_tmp77E=":",_tag_dyneither(_tmp77E,sizeof(char),2)))),((_tmp77D[0]=_tmp20A,Cyc_PP_cat(_tag_dyneither(_tmp77D,sizeof(struct Cyc_PP_Doc*),3))))))));}{
const char*_tmp783;const char*_tmp782;struct Cyc_PP_Doc*_tmp781[3];struct Cyc_PP_Doc*_tmp20F=(_tmp781[2]=Cyc_PP_text(((_tmp782=")",_tag_dyneither(_tmp782,sizeof(char),2)))),((_tmp781[1]=_tmp20A,((_tmp781[0]=Cyc_PP_text(((_tmp783="(",_tag_dyneither(_tmp783,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp781,sizeof(struct Cyc_PP_Doc*),3)))))));
if(req != 0){
const char*_tmp788;const char*_tmp787;struct Cyc_PP_Doc*_tmp786[4];_tmp20F=((_tmp786[3]=Cyc_PP_text(((_tmp787=")",_tag_dyneither(_tmp787,sizeof(char),2)))),((_tmp786[2]=Cyc_Absynpp_exp2doc(req),((_tmp786[1]=Cyc_PP_text(((_tmp788=" @requires(",_tag_dyneither(_tmp788,sizeof(char),12)))),((_tmp786[0]=_tmp20F,Cyc_PP_cat(_tag_dyneither(_tmp786,sizeof(struct Cyc_PP_Doc*),4))))))))));}
if(ens != 0){
const char*_tmp78D;const char*_tmp78C;struct Cyc_PP_Doc*_tmp78B[4];_tmp20F=((_tmp78B[3]=Cyc_PP_text(((_tmp78C=")",_tag_dyneither(_tmp78C,sizeof(char),2)))),((_tmp78B[2]=Cyc_Absynpp_exp2doc(ens),((_tmp78B[1]=Cyc_PP_text(((_tmp78D=" @ensures(",_tag_dyneither(_tmp78D,sizeof(char),11)))),((_tmp78B[0]=_tmp20F,Cyc_PP_cat(_tag_dyneither(_tmp78B,sizeof(struct Cyc_PP_Doc*),4))))))))));}
return _tmp20F;};};}
# 918
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg){
struct _dyneither_ptr*_tmp21D;struct Cyc_Absyn_Tqual _tmp21E;void*_tmp21F;struct _tuple8*_tmp21C=arg;_tmp21D=_tmp21C->f1;_tmp21E=_tmp21C->f2;_tmp21F=_tmp21C->f3;{
struct _tuple8*_tmp78E;return(_tmp78E=_cycalloc(sizeof(*_tmp78E)),((_tmp78E->f1=_tmp21D,((_tmp78E->f2=_tmp21E,((_tmp78E->f3=_tmp21F,_tmp78E)))))));};}
# 923
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){return Cyc_PP_text(*v);}
# 925
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp221=0;
int match;
{union Cyc_Absyn_Nmspace _tmp222=(*q).f1;union Cyc_Absyn_Nmspace _tmp223=_tmp222;struct Cyc_List_List*_tmp224;struct Cyc_List_List*_tmp225;struct Cyc_List_List*_tmp226;_LL12D: if((_tmp223.Loc_n).tag != 4)goto _LL12F;_LL12E:
# 930
 _tmp224=0;goto _LL130;_LL12F: if((_tmp223.Rel_n).tag != 1)goto _LL131;_tmp224=(struct Cyc_List_List*)(_tmp223.Rel_n).val;_LL130:
# 932
 match=0;
_tmp221=_tmp224;
goto _LL12C;_LL131: if((_tmp223.C_n).tag != 3)goto _LL133;_tmp225=(struct Cyc_List_List*)(_tmp223.C_n).val;_LL132:
# 936
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp225,Cyc_Absynpp_curr_namespace);
# 938
goto _LL12C;_LL133: if((_tmp223.Abs_n).tag != 2)goto _LL12C;_tmp226=(struct Cyc_List_List*)(_tmp223.Abs_n).val;_LL134:
# 940
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp226,Cyc_Absynpp_curr_namespace);
{struct Cyc_List_List*_tmp78F;_tmp221=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(_tmp78F=_cycalloc(sizeof(*_tmp78F)),((_tmp78F->hd=Cyc_Absynpp_cyc_stringptr,((_tmp78F->tl=_tmp226,_tmp78F))))): _tmp226;}
goto _LL12C;_LL12C:;}
# 944
if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp791;struct Cyc_List_List*_tmp790;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp221,(
(_tmp790=_cycalloc(sizeof(*_tmp790)),((_tmp790->hd=(*q).f2,((_tmp790->tl=0,_tmp790))))))),((_tmp791="_",_tag_dyneither(_tmp791,sizeof(char),2))));}else{
# 949
if(match)
return*(*q).f2;else{
# 952
const char*_tmp793;struct Cyc_List_List*_tmp792;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp221,(
(_tmp792=_cycalloc(sizeof(*_tmp792)),((_tmp792->hd=(*q).f2,((_tmp792->tl=0,_tmp792))))))),((_tmp793="::",_tag_dyneither(_tmp793,sizeof(char),3))));}}}
# 957
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 961
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp22C=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output){
# 965
const char*_tmp795;const char*_tmp794;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp794="\\textbf{",_tag_dyneither(_tmp794,sizeof(char),9))),(struct _dyneither_ptr)_tmp22C),((_tmp795="}",_tag_dyneither(_tmp795,sizeof(char),2)))),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp22C));}else{
return Cyc_PP_text(_tmp22C);}}
# 970
struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 972
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 975
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp22F=(*v).f1;union Cyc_Absyn_Nmspace _tmp230=_tmp22F;struct Cyc_List_List*_tmp231;struct Cyc_List_List*_tmp232;_LL136: if((_tmp230.Loc_n).tag != 4)goto _LL138;_LL137:
 goto _LL139;_LL138: if((_tmp230.Rel_n).tag != 1)goto _LL13A;if((struct Cyc_List_List*)(_tmp230.Rel_n).val != 0)goto _LL13A;_LL139:
 return*(*v).f2;_LL13A: if((_tmp230.C_n).tag != 3)goto _LL13C;_tmp231=(struct Cyc_List_List*)(_tmp230.C_n).val;_LL13B:
 _tmp232=_tmp231;goto _LL13D;_LL13C: if((_tmp230.Abs_n).tag != 2)goto _LL13E;_tmp232=(struct Cyc_List_List*)(_tmp230.Abs_n).val;_LL13D:
# 981
 if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp232,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 984
goto _LL13F;}_LL13E:;_LL13F: {
# 986
const char*_tmp796;return(struct _dyneither_ptr)Cyc_strconcat(((_tmp796="/* bad namespace : */ ",_tag_dyneither(_tmp796,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));}_LL135:;}else{
# 989
return*(*v).f2;}}
# 992
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 995
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _dyneither_ptr _tmp234=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output){
# 999
const char*_tmp798;const char*_tmp797;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp797="\\textbf{",_tag_dyneither(_tmp797,sizeof(char),9))),(struct _dyneither_ptr)_tmp234),((_tmp798="}",_tag_dyneither(_tmp798,sizeof(char),2)))),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp234));}else{
return Cyc_PP_text(_tmp234);}}
# 1004
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}
# 1015 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp237=e->r;void*_tmp238=_tmp237;enum Cyc_Absyn_Primop _tmp23C;struct Cyc_Absyn_Exp*_tmp246;struct Cyc_Absyn_Exp*_tmp248;_LL141: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp239=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp239->tag != 0)goto _LL143;}_LL142:
 goto _LL144;_LL143: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp23A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp23A->tag != 1)goto _LL145;}_LL144:
 return 10000;_LL145: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp23B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp23B->tag != 2)goto _LL147;else{_tmp23C=_tmp23B->f1;}}_LL146:
# 1020
 switch(_tmp23C){case Cyc_Absyn_Plus: _LL191:
 return 100;case Cyc_Absyn_Times: _LL192:
 return 110;case Cyc_Absyn_Minus: _LL193:
 return 100;case Cyc_Absyn_Div: _LL194:
 goto _LL195;case Cyc_Absyn_Mod: _LL195:
 return 110;case Cyc_Absyn_Eq: _LL196:
 goto _LL197;case Cyc_Absyn_Neq: _LL197:
 return 70;case Cyc_Absyn_Gt: _LL198:
 goto _LL199;case Cyc_Absyn_Lt: _LL199:
 goto _LL19A;case Cyc_Absyn_Gte: _LL19A:
 goto _LL19B;case Cyc_Absyn_Lte: _LL19B:
 return 80;case Cyc_Absyn_Not: _LL19C:
 goto _LL19D;case Cyc_Absyn_Bitnot: _LL19D:
 return 130;case Cyc_Absyn_Bitand: _LL19E:
 return 60;case Cyc_Absyn_Bitor: _LL19F:
 return 40;case Cyc_Absyn_Bitxor: _LL1A0:
 return 50;case Cyc_Absyn_Bitlshift: _LL1A1:
 return 90;case Cyc_Absyn_Bitlrshift: _LL1A2:
 return 80;case Cyc_Absyn_Bitarshift: _LL1A3:
 return 80;case Cyc_Absyn_Numelts: _LL1A4:
 return 140;}_LL147: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp23D=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp23D->tag != 3)goto _LL149;}_LL148:
# 1042
 return 20;_LL149: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp23E=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp23E->tag != 4)goto _LL14B;}_LL14A:
 return 130;_LL14B: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp23F=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp23F->tag != 5)goto _LL14D;}_LL14C:
 return 30;_LL14D: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp240=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp240->tag != 6)goto _LL14F;}_LL14E:
 return 35;_LL14F: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp241=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp241->tag != 7)goto _LL151;}_LL150:
 return 30;_LL151: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp242=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp242->tag != 8)goto _LL153;}_LL152:
 return 10;_LL153: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp243=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp243->tag != 9)goto _LL155;}_LL154:
 return 140;_LL155: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp244=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp244->tag != 10)goto _LL157;}_LL156:
 return 130;_LL157: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp245=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp245->tag != 11)goto _LL159;else{_tmp246=_tmp245->f1;}}_LL158:
 return Cyc_Absynpp_exp_prec(_tmp246);_LL159: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp247=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp247->tag != 12)goto _LL15B;else{_tmp248=_tmp247->f1;}}_LL15A:
 return Cyc_Absynpp_exp_prec(_tmp248);_LL15B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp249=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp249->tag != 13)goto _LL15D;}_LL15C:
 return 120;_LL15D: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp24A=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp24A->tag != 15)goto _LL15F;}_LL15E:
 return 15;_LL15F: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp24B=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp24B->tag != 14)goto _LL161;}_LL160:
 goto _LL162;_LL161: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp24C=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp24C->tag != 16)goto _LL163;}_LL162:
 goto _LL164;_LL163: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp24D=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp24D->tag != 17)goto _LL165;}_LL164:
 goto _LL166;_LL165: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp24E=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp24E->tag != 38)goto _LL167;}_LL166:
 goto _LL168;_LL167: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp24F=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp24F->tag != 39)goto _LL169;}_LL168:
 goto _LL16A;_LL169: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp250=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp250->tag != 37)goto _LL16B;}_LL16A:
 goto _LL16C;_LL16B: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp251=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp251->tag != 18)goto _LL16D;}_LL16C:
 goto _LL16E;_LL16D: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp252=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp252->tag != 19)goto _LL16F;}_LL16E:
 return 130;_LL16F: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp253=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp253->tag != 20)goto _LL171;}_LL170:
 goto _LL172;_LL171: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp254=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp254->tag != 21)goto _LL173;}_LL172:
 goto _LL174;_LL173: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp255=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp255->tag != 22)goto _LL175;}_LL174:
 return 140;_LL175: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp256=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp256->tag != 23)goto _LL177;}_LL176:
 return 150;_LL177: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp257=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp257->tag != 24)goto _LL179;}_LL178:
 goto _LL17A;_LL179: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp258=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp258->tag != 25)goto _LL17B;}_LL17A:
 goto _LL17C;_LL17B: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp259=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp259->tag != 26)goto _LL17D;}_LL17C:
 goto _LL17E;_LL17D: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp25A=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp25A->tag != 27)goto _LL17F;}_LL17E:
 goto _LL180;_LL17F: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp25B=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp25B->tag != 28)goto _LL181;}_LL180:
 goto _LL182;_LL181: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp25C=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp25C->tag != 29)goto _LL183;}_LL182:
 goto _LL184;_LL183: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp25D=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp25D->tag != 30)goto _LL185;}_LL184:
 goto _LL186;_LL185: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp25E=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp25E->tag != 31)goto _LL187;}_LL186:
 goto _LL188;_LL187: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp25F=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp25F->tag != 32)goto _LL189;}_LL188:
 goto _LL18A;_LL189: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp260=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp260->tag != 33)goto _LL18B;}_LL18A:
 goto _LL18C;_LL18B: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp261=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp261->tag != 34)goto _LL18D;}_LL18C:
 goto _LL18E;_LL18D: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp262=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp262->tag != 35)goto _LL18F;}_LL18E:
 return 140;_LL18F: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp263=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp238;if(_tmp263->tag != 36)goto _LL140;}_LL190:
 return 10000;_LL140:;}
# 1082
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1086
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp264=e->r;void*_tmp265=_tmp264;union Cyc_Absyn_Cnst _tmp267;struct _tuple0*_tmp269;enum Cyc_Absyn_Primop _tmp26B;struct Cyc_List_List*_tmp26C;struct Cyc_Absyn_Exp*_tmp26E;struct Cyc_Core_Opt*_tmp26F;struct Cyc_Absyn_Exp*_tmp270;struct Cyc_Absyn_Exp*_tmp272;enum Cyc_Absyn_Incrementor _tmp273;struct Cyc_Absyn_Exp*_tmp275;struct Cyc_Absyn_Exp*_tmp276;struct Cyc_Absyn_Exp*_tmp277;struct Cyc_Absyn_Exp*_tmp279;struct Cyc_Absyn_Exp*_tmp27A;struct Cyc_Absyn_Exp*_tmp27C;struct Cyc_Absyn_Exp*_tmp27D;struct Cyc_Absyn_Exp*_tmp27F;struct Cyc_Absyn_Exp*_tmp280;struct Cyc_Absyn_Exp*_tmp282;struct Cyc_List_List*_tmp283;struct Cyc_Absyn_Exp*_tmp285;struct Cyc_Absyn_Exp*_tmp287;struct Cyc_Absyn_Exp*_tmp289;void*_tmp28B;struct Cyc_Absyn_Exp*_tmp28C;struct Cyc_Absyn_Exp*_tmp28E;struct Cyc_Absyn_Exp*_tmp290;struct Cyc_Absyn_Exp*_tmp291;void*_tmp293;struct Cyc_Absyn_Exp*_tmp295;void*_tmp297;int _tmp299;struct _dyneither_ptr _tmp29A;struct Cyc_Absyn_Exp*_tmp29C;struct _dyneither_ptr*_tmp29D;void*_tmp29F;struct Cyc_List_List*_tmp2A0;struct Cyc_Absyn_Exp*_tmp2A2;struct Cyc_Absyn_Exp*_tmp2A4;struct _dyneither_ptr*_tmp2A5;struct Cyc_Absyn_Exp*_tmp2A7;struct _dyneither_ptr*_tmp2A8;struct Cyc_Absyn_Exp*_tmp2AA;struct Cyc_Absyn_Exp*_tmp2AB;struct Cyc_List_List*_tmp2AD;struct _tuple8*_tmp2AF;struct Cyc_List_List*_tmp2B0;struct Cyc_List_List*_tmp2B2;struct Cyc_Absyn_Vardecl*_tmp2B4;struct Cyc_Absyn_Exp*_tmp2B5;struct Cyc_Absyn_Exp*_tmp2B6;struct Cyc_Absyn_Exp*_tmp2B8;void*_tmp2B9;struct _tuple0*_tmp2BB;struct Cyc_List_List*_tmp2BC;struct Cyc_List_List*_tmp2BD;struct Cyc_List_List*_tmp2BF;struct Cyc_List_List*_tmp2C1;struct Cyc_Absyn_Datatypefield*_tmp2C2;struct _tuple0*_tmp2C4;struct _tuple0*_tmp2C6;int _tmp2C8;struct Cyc_Absyn_Exp*_tmp2C9;void**_tmp2CA;struct Cyc_Absyn_Exp*_tmp2CB;struct Cyc_Absyn_Exp*_tmp2CD;struct Cyc_Absyn_Exp*_tmp2CE;struct Cyc_Core_Opt*_tmp2D0;struct Cyc_List_List*_tmp2D1;struct Cyc_Absyn_Stmt*_tmp2D3;_LL1A7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp266=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp266->tag != 0)goto _LL1A9;else{_tmp267=_tmp266->f1;}}_LL1A8:
 s=Cyc_Absynpp_cnst2doc(_tmp267);goto _LL1A6;_LL1A9: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp268=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp268->tag != 1)goto _LL1AB;else{_tmp269=_tmp268->f1;}}_LL1AA:
 s=Cyc_Absynpp_qvar2doc(_tmp269);goto _LL1A6;_LL1AB: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp26A=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp26A->tag != 2)goto _LL1AD;else{_tmp26B=_tmp26A->f1;_tmp26C=_tmp26A->f2;}}_LL1AC:
 s=Cyc_Absynpp_primapp2doc(myprec,_tmp26B,_tmp26C);goto _LL1A6;_LL1AD: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp26D=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp26D->tag != 3)goto _LL1AF;else{_tmp26E=_tmp26D->f1;_tmp26F=_tmp26D->f2;_tmp270=_tmp26D->f3;}}_LL1AE:
# 1094
{const char*_tmp79D;const char*_tmp79C;struct Cyc_PP_Doc*_tmp79B[5];s=((_tmp79B[4]=
# 1098
Cyc_Absynpp_exp2doc_prec(myprec,_tmp270),((_tmp79B[3]=
# 1097
Cyc_PP_text(((_tmp79C="= ",_tag_dyneither(_tmp79C,sizeof(char),3)))),((_tmp79B[2]=
# 1096
_tmp26F == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp26F->v),((_tmp79B[1]=
# 1095
Cyc_PP_text(((_tmp79D=" ",_tag_dyneither(_tmp79D,sizeof(char),2)))),((_tmp79B[0]=
# 1094
Cyc_Absynpp_exp2doc_prec(myprec,_tmp26E),Cyc_PP_cat(_tag_dyneither(_tmp79B,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1099
goto _LL1A6;_LL1AF: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp271=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp271->tag != 4)goto _LL1B1;else{_tmp272=_tmp271->f1;_tmp273=_tmp271->f2;}}_LL1B0: {
# 1101
struct Cyc_PP_Doc*_tmp2D7=Cyc_Absynpp_exp2doc_prec(myprec,_tmp272);
switch(_tmp273){case Cyc_Absyn_PreInc: _LL1F7:
{const char*_tmp7A0;struct Cyc_PP_Doc*_tmp79F[2];s=((_tmp79F[1]=_tmp2D7,((_tmp79F[0]=Cyc_PP_text(((_tmp7A0="++",_tag_dyneither(_tmp7A0,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp79F,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PreDec: _LL1F8:
{const char*_tmp7A3;struct Cyc_PP_Doc*_tmp7A2[2];s=((_tmp7A2[1]=_tmp2D7,((_tmp7A2[0]=Cyc_PP_text(((_tmp7A3="--",_tag_dyneither(_tmp7A3,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp7A2,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostInc: _LL1F9:
{const char*_tmp7A6;struct Cyc_PP_Doc*_tmp7A5[2];s=((_tmp7A5[1]=Cyc_PP_text(((_tmp7A6="++",_tag_dyneither(_tmp7A6,sizeof(char),3)))),((_tmp7A5[0]=_tmp2D7,Cyc_PP_cat(_tag_dyneither(_tmp7A5,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1FA:
{const char*_tmp7A9;struct Cyc_PP_Doc*_tmp7A8[2];s=((_tmp7A8[1]=Cyc_PP_text(((_tmp7A9="--",_tag_dyneither(_tmp7A9,sizeof(char),3)))),((_tmp7A8[0]=_tmp2D7,Cyc_PP_cat(_tag_dyneither(_tmp7A8,sizeof(struct Cyc_PP_Doc*),2))))));}break;}
# 1108
goto _LL1A6;}_LL1B1: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp274=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp274->tag != 5)goto _LL1B3;else{_tmp275=_tmp274->f1;_tmp276=_tmp274->f2;_tmp277=_tmp274->f3;}}_LL1B2:
# 1110
{const char*_tmp7AE;const char*_tmp7AD;struct Cyc_PP_Doc*_tmp7AC[5];s=((_tmp7AC[4]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp277),((_tmp7AC[3]=Cyc_PP_text(((_tmp7AD=" : ",_tag_dyneither(_tmp7AD,sizeof(char),4)))),((_tmp7AC[2]=
# 1110
Cyc_Absynpp_exp2doc_prec(0,_tmp276),((_tmp7AC[1]=Cyc_PP_text(((_tmp7AE=" ? ",_tag_dyneither(_tmp7AE,sizeof(char),4)))),((_tmp7AC[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp275),Cyc_PP_cat(_tag_dyneither(_tmp7AC,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1112
goto _LL1A6;_LL1B3: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp278=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp278->tag != 6)goto _LL1B5;else{_tmp279=_tmp278->f1;_tmp27A=_tmp278->f2;}}_LL1B4:
# 1114
{const char*_tmp7B1;struct Cyc_PP_Doc*_tmp7B0[3];s=((_tmp7B0[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27A),((_tmp7B0[1]=Cyc_PP_text(((_tmp7B1=" && ",_tag_dyneither(_tmp7B1,sizeof(char),5)))),((_tmp7B0[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp279),Cyc_PP_cat(_tag_dyneither(_tmp7B0,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A6;_LL1B5: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp27B=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp27B->tag != 7)goto _LL1B7;else{_tmp27C=_tmp27B->f1;_tmp27D=_tmp27B->f2;}}_LL1B6:
# 1117
{const char*_tmp7B4;struct Cyc_PP_Doc*_tmp7B3[3];s=((_tmp7B3[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27D),((_tmp7B3[1]=Cyc_PP_text(((_tmp7B4=" || ",_tag_dyneither(_tmp7B4,sizeof(char),5)))),((_tmp7B3[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27C),Cyc_PP_cat(_tag_dyneither(_tmp7B3,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A6;_LL1B7: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp27E=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp27E->tag != 8)goto _LL1B9;else{_tmp27F=_tmp27E->f1;_tmp280=_tmp27E->f2;}}_LL1B8:
# 1122
{const char*_tmp7BB;const char*_tmp7BA;const char*_tmp7B9;struct Cyc_PP_Doc*_tmp7B8[5];s=((_tmp7B8[4]=Cyc_PP_text(((_tmp7B9=")",_tag_dyneither(_tmp7B9,sizeof(char),2)))),((_tmp7B8[3]=Cyc_Absynpp_exp2doc(_tmp280),((_tmp7B8[2]=Cyc_PP_text(((_tmp7BA=", ",_tag_dyneither(_tmp7BA,sizeof(char),3)))),((_tmp7B8[1]=Cyc_Absynpp_exp2doc(_tmp27F),((_tmp7B8[0]=Cyc_PP_text(((_tmp7BB="(",_tag_dyneither(_tmp7BB,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7B8,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1A6;_LL1B9: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp281=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp281->tag != 9)goto _LL1BB;else{_tmp282=_tmp281->f1;_tmp283=_tmp281->f2;}}_LL1BA:
# 1125
{const char*_tmp7C0;const char*_tmp7BF;struct Cyc_PP_Doc*_tmp7BE[4];s=((_tmp7BE[3]=
# 1128
Cyc_PP_text(((_tmp7BF=")",_tag_dyneither(_tmp7BF,sizeof(char),2)))),((_tmp7BE[2]=
# 1127
Cyc_Absynpp_exps2doc_prec(20,_tmp283),((_tmp7BE[1]=
# 1126
Cyc_PP_text(((_tmp7C0="(",_tag_dyneither(_tmp7C0,sizeof(char),2)))),((_tmp7BE[0]=
# 1125
Cyc_Absynpp_exp2doc_prec(myprec,_tmp282),Cyc_PP_cat(_tag_dyneither(_tmp7BE,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1129
goto _LL1A6;_LL1BB: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp284=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp284->tag != 10)goto _LL1BD;else{_tmp285=_tmp284->f1;}}_LL1BC:
# 1131
{const char*_tmp7C3;struct Cyc_PP_Doc*_tmp7C2[2];s=((_tmp7C2[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp285),((_tmp7C2[0]=Cyc_PP_text(((_tmp7C3="throw ",_tag_dyneither(_tmp7C3,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp7C2,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1A6;_LL1BD: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp286=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp286->tag != 11)goto _LL1BF;else{_tmp287=_tmp286->f1;}}_LL1BE:
# 1134
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp287);
goto _LL1A6;_LL1BF: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp288=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp288->tag != 12)goto _LL1C1;else{_tmp289=_tmp288->f1;}}_LL1C0:
# 1137
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp289);
goto _LL1A6;_LL1C1: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp28A=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp28A->tag != 13)goto _LL1C3;else{_tmp28B=(void*)_tmp28A->f1;_tmp28C=_tmp28A->f2;}}_LL1C2:
# 1140
{const char*_tmp7C8;const char*_tmp7C7;struct Cyc_PP_Doc*_tmp7C6[4];s=((_tmp7C6[3]=
# 1143
Cyc_Absynpp_exp2doc_prec(myprec,_tmp28C),((_tmp7C6[2]=
# 1142
Cyc_PP_text(((_tmp7C7=")",_tag_dyneither(_tmp7C7,sizeof(char),2)))),((_tmp7C6[1]=
# 1141
Cyc_Absynpp_typ2doc(_tmp28B),((_tmp7C6[0]=
# 1140
Cyc_PP_text(((_tmp7C8="(",_tag_dyneither(_tmp7C8,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7C6,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1144
goto _LL1A6;_LL1C3: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp28D=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp28D->tag != 14)goto _LL1C5;else{_tmp28E=_tmp28D->f1;}}_LL1C4:
# 1146
{const char*_tmp7CB;struct Cyc_PP_Doc*_tmp7CA[2];s=((_tmp7CA[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp28E),((_tmp7CA[0]=
# 1146
Cyc_PP_text(((_tmp7CB="&",_tag_dyneither(_tmp7CB,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7CA,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1148
goto _LL1A6;_LL1C5: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp28F=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp28F->tag != 15)goto _LL1C7;else{_tmp290=_tmp28F->f1;_tmp291=_tmp28F->f2;}}_LL1C6:
# 1150
 if(_tmp290 == 0){
const char*_tmp7CE;struct Cyc_PP_Doc*_tmp7CD[2];s=((_tmp7CD[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp291),((_tmp7CD[0]=Cyc_PP_text(((_tmp7CE="new ",_tag_dyneither(_tmp7CE,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp7CD,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 1153
const char*_tmp7D3;const char*_tmp7D2;struct Cyc_PP_Doc*_tmp7D1[4];s=((_tmp7D1[3]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp291),((_tmp7D1[2]=
# 1153
Cyc_PP_text(((_tmp7D2=") ",_tag_dyneither(_tmp7D2,sizeof(char),3)))),((_tmp7D1[1]=Cyc_Absynpp_exp2doc(_tmp290),((_tmp7D1[0]=Cyc_PP_text(((_tmp7D3="rnew(",_tag_dyneither(_tmp7D3,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp7D1,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1155
goto _LL1A6;_LL1C7: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp292=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp292->tag != 16)goto _LL1C9;else{_tmp293=(void*)_tmp292->f1;}}_LL1C8:
{const char*_tmp7D8;const char*_tmp7D7;struct Cyc_PP_Doc*_tmp7D6[3];s=((_tmp7D6[2]=Cyc_PP_text(((_tmp7D7=")",_tag_dyneither(_tmp7D7,sizeof(char),2)))),((_tmp7D6[1]=Cyc_Absynpp_typ2doc(_tmp293),((_tmp7D6[0]=Cyc_PP_text(((_tmp7D8="sizeof(",_tag_dyneither(_tmp7D8,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7D6,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A6;_LL1C9: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp294=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp294->tag != 17)goto _LL1CB;else{_tmp295=_tmp294->f1;}}_LL1CA:
{const char*_tmp7DD;const char*_tmp7DC;struct Cyc_PP_Doc*_tmp7DB[3];s=((_tmp7DB[2]=Cyc_PP_text(((_tmp7DC=")",_tag_dyneither(_tmp7DC,sizeof(char),2)))),((_tmp7DB[1]=Cyc_Absynpp_exp2doc(_tmp295),((_tmp7DB[0]=Cyc_PP_text(((_tmp7DD="sizeof(",_tag_dyneither(_tmp7DD,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7DB,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A6;_LL1CB: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp296=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp296->tag != 38)goto _LL1CD;else{_tmp297=(void*)_tmp296->f1;}}_LL1CC:
{const char*_tmp7E2;const char*_tmp7E1;struct Cyc_PP_Doc*_tmp7E0[3];s=((_tmp7E0[2]=Cyc_PP_text(((_tmp7E1=")",_tag_dyneither(_tmp7E1,sizeof(char),2)))),((_tmp7E0[1]=Cyc_Absynpp_typ2doc(_tmp297),((_tmp7E0[0]=Cyc_PP_text(((_tmp7E2="valueof(",_tag_dyneither(_tmp7E2,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7E0,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A6;_LL1CD: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp298=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp298->tag != 39)goto _LL1CF;else{_tmp299=_tmp298->f1;_tmp29A=_tmp298->f2;}}_LL1CE:
# 1160
 if(_tmp299){
const char*_tmp7E9;const char*_tmp7E8;const char*_tmp7E7;struct Cyc_PP_Doc*_tmp7E6[4];s=((_tmp7E6[3]=Cyc_PP_text(((_tmp7E7=")",_tag_dyneither(_tmp7E7,sizeof(char),2)))),((_tmp7E6[2]=Cyc_PP_text(_tmp29A),((_tmp7E6[1]=Cyc_PP_text(((_tmp7E8=" volatile (",_tag_dyneither(_tmp7E8,sizeof(char),12)))),((_tmp7E6[0]=Cyc_PP_text(((_tmp7E9="__asm__",_tag_dyneither(_tmp7E9,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7E6,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1163
const char*_tmp7EE;const char*_tmp7ED;struct Cyc_PP_Doc*_tmp7EC[3];s=((_tmp7EC[2]=Cyc_PP_text(((_tmp7ED=")",_tag_dyneither(_tmp7ED,sizeof(char),2)))),((_tmp7EC[1]=Cyc_PP_text(_tmp29A),((_tmp7EC[0]=Cyc_PP_text(((_tmp7EE="__asm__(",_tag_dyneither(_tmp7EE,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7EC,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A6;_LL1CF: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp29B=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp29B->tag != 37)goto _LL1D1;else{_tmp29C=_tmp29B->f1;_tmp29D=_tmp29B->f2;}}_LL1D0:
# 1166
{const char*_tmp7F5;const char*_tmp7F4;const char*_tmp7F3;struct Cyc_PP_Doc*_tmp7F2[5];s=((_tmp7F2[4]=Cyc_PP_text(((_tmp7F3=")",_tag_dyneither(_tmp7F3,sizeof(char),2)))),((_tmp7F2[3]=Cyc_PP_textptr(_tmp29D),((_tmp7F2[2]=Cyc_PP_text(((_tmp7F4=".",_tag_dyneither(_tmp7F4,sizeof(char),2)))),((_tmp7F2[1]=Cyc_Absynpp_exp2doc(_tmp29C),((_tmp7F2[0]=Cyc_PP_text(((_tmp7F5="tagcheck(",_tag_dyneither(_tmp7F5,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7F2,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1A6;_LL1D1: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp29E=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp29E->tag != 18)goto _LL1D3;else{_tmp29F=(void*)_tmp29E->f1;_tmp2A0=_tmp29E->f2;}}_LL1D2:
# 1169
{const char*_tmp7FA;const char*_tmp7F9;struct Cyc_PP_Doc*_tmp7F8[3];s=((_tmp7F8[2]=Cyc_PP_text(((_tmp7F9=",",_tag_dyneither(_tmp7F9,sizeof(char),2)))),((_tmp7F8[1]=Cyc_Absynpp_typ2doc(_tmp29F),((_tmp7F8[0]=Cyc_PP_text(((_tmp7FA="offsetof(",_tag_dyneither(_tmp7FA,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7F8,sizeof(struct Cyc_PP_Doc*),3))))))));}
for(0;_tmp2A0 != 0;_tmp2A0=_tmp2A0->tl){
void*_tmp311=(void*)_tmp2A0->hd;void*_tmp312=_tmp311;struct _dyneither_ptr*_tmp314;unsigned int _tmp316;_LL1FD: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp313=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp312;if(_tmp313->tag != 0)goto _LL1FF;else{_tmp314=_tmp313->f1;}}_LL1FE:
{const char*_tmp7FD;struct Cyc_PP_Doc*_tmp7FC[3];s=((_tmp7FC[2]=
_tmp2A0->tl != 0?Cyc_PP_text(((_tmp7FD=".",_tag_dyneither(_tmp7FD,sizeof(char),2)))): Cyc_PP_nil_doc(),((_tmp7FC[1]=
# 1172
Cyc_PP_textptr(_tmp314),((_tmp7FC[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp7FC,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1174
goto _LL1FC;_LL1FF: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp315=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp312;if(_tmp315->tag != 1)goto _LL1FC;else{_tmp316=_tmp315->f1;}}_LL200:
{struct Cyc_Int_pa_PrintArg_struct _tmp807;void*_tmp806[1];const char*_tmp805;const char*_tmp804;struct Cyc_PP_Doc*_tmp803[3];s=((_tmp803[2]=
_tmp2A0->tl != 0?Cyc_PP_text(((_tmp804=".",_tag_dyneither(_tmp804,sizeof(char),2)))): Cyc_PP_nil_doc(),((_tmp803[1]=
# 1175
Cyc_PP_text((struct _dyneither_ptr)((_tmp807.tag=1,((_tmp807.f1=(unsigned long)((int)_tmp316),((_tmp806[0]=& _tmp807,Cyc_aprintf(((_tmp805="%d",_tag_dyneither(_tmp805,sizeof(char),3))),_tag_dyneither(_tmp806,sizeof(void*),1))))))))),((_tmp803[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp803,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1177
goto _LL1FC;_LL1FC:;}
# 1179
{const char*_tmp80A;struct Cyc_PP_Doc*_tmp809[2];s=((_tmp809[1]=Cyc_PP_text(((_tmp80A=")",_tag_dyneither(_tmp80A,sizeof(char),2)))),((_tmp809[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp809,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1A6;_LL1D3: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp2A1=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp2A1->tag != 19)goto _LL1D5;else{_tmp2A2=_tmp2A1->f1;}}_LL1D4:
# 1182
{const char*_tmp80D;struct Cyc_PP_Doc*_tmp80C[2];s=((_tmp80C[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A2),((_tmp80C[0]=Cyc_PP_text(((_tmp80D="*",_tag_dyneither(_tmp80D,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp80C,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1A6;_LL1D5: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp2A3=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp2A3->tag != 20)goto _LL1D7;else{_tmp2A4=_tmp2A3->f1;_tmp2A5=_tmp2A3->f2;}}_LL1D6:
# 1185
{const char*_tmp810;struct Cyc_PP_Doc*_tmp80F[3];s=((_tmp80F[2]=Cyc_PP_textptr(_tmp2A5),((_tmp80F[1]=Cyc_PP_text(((_tmp810=".",_tag_dyneither(_tmp810,sizeof(char),2)))),((_tmp80F[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A4),Cyc_PP_cat(_tag_dyneither(_tmp80F,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A6;_LL1D7: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2A6=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp2A6->tag != 21)goto _LL1D9;else{_tmp2A7=_tmp2A6->f1;_tmp2A8=_tmp2A6->f2;}}_LL1D8:
# 1188
{const char*_tmp813;struct Cyc_PP_Doc*_tmp812[3];s=((_tmp812[2]=Cyc_PP_textptr(_tmp2A8),((_tmp812[1]=Cyc_PP_text(((_tmp813="->",_tag_dyneither(_tmp813,sizeof(char),3)))),((_tmp812[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A7),Cyc_PP_cat(_tag_dyneither(_tmp812,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A6;_LL1D9: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp2A9=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp2A9->tag != 22)goto _LL1DB;else{_tmp2AA=_tmp2A9->f1;_tmp2AB=_tmp2A9->f2;}}_LL1DA:
# 1191
{const char*_tmp818;const char*_tmp817;struct Cyc_PP_Doc*_tmp816[4];s=((_tmp816[3]=
# 1194
Cyc_PP_text(((_tmp817="]",_tag_dyneither(_tmp817,sizeof(char),2)))),((_tmp816[2]=
# 1193
Cyc_Absynpp_exp2doc(_tmp2AB),((_tmp816[1]=
# 1192
Cyc_PP_text(((_tmp818="[",_tag_dyneither(_tmp818,sizeof(char),2)))),((_tmp816[0]=
# 1191
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AA),Cyc_PP_cat(_tag_dyneither(_tmp816,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1195
goto _LL1A6;_LL1DB: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp2AC=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp2AC->tag != 23)goto _LL1DD;else{_tmp2AD=_tmp2AC->f1;}}_LL1DC:
# 1197
{const char*_tmp81D;const char*_tmp81C;struct Cyc_PP_Doc*_tmp81B[4];s=((_tmp81B[3]=
# 1200
Cyc_PP_text(((_tmp81C=")",_tag_dyneither(_tmp81C,sizeof(char),2)))),((_tmp81B[2]=
# 1199
Cyc_Absynpp_exps2doc_prec(20,_tmp2AD),((_tmp81B[1]=
# 1198
Cyc_PP_text(((_tmp81D="(",_tag_dyneither(_tmp81D,sizeof(char),2)))),((_tmp81B[0]=
# 1197
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp81B,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1201
goto _LL1A6;_LL1DD: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp2AE=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp2AE->tag != 24)goto _LL1DF;else{_tmp2AF=_tmp2AE->f1;_tmp2B0=_tmp2AE->f2;}}_LL1DE:
# 1203
{const char*_tmp824;const char*_tmp823;const char*_tmp822;struct Cyc_PP_Doc*_tmp821[4];s=((_tmp821[3]=
# 1206
Cyc_Absynpp_group_braces(((_tmp822=",",_tag_dyneither(_tmp822,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2B0)),((_tmp821[2]=
# 1205
Cyc_PP_text(((_tmp823=")",_tag_dyneither(_tmp823,sizeof(char),2)))),((_tmp821[1]=
# 1204
Cyc_Absynpp_typ2doc((*_tmp2AF).f3),((_tmp821[0]=
# 1203
Cyc_PP_text(((_tmp824="(",_tag_dyneither(_tmp824,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp821,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1207
goto _LL1A6;_LL1DF: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp2B1=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp2B1->tag != 25)goto _LL1E1;else{_tmp2B2=_tmp2B1->f1;}}_LL1E0:
# 1209
{const char*_tmp825;s=Cyc_Absynpp_group_braces(((_tmp825=",",_tag_dyneither(_tmp825,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2B2));}
goto _LL1A6;_LL1E1: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp2B3=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp2B3->tag != 26)goto _LL1E3;else{_tmp2B4=_tmp2B3->f1;_tmp2B5=_tmp2B3->f2;_tmp2B6=_tmp2B3->f3;}}_LL1E2:
# 1212
{const char*_tmp82C;const char*_tmp82B;const char*_tmp82A;struct Cyc_PP_Doc*_tmp829[8];s=((_tmp829[7]=
# 1218
Cyc_Absynpp_rb(),((_tmp829[6]=
# 1217
Cyc_Absynpp_exp2doc(_tmp2B6),((_tmp829[5]=
# 1216
Cyc_PP_text(((_tmp82A=" : ",_tag_dyneither(_tmp82A,sizeof(char),4)))),((_tmp829[4]=
# 1215
Cyc_Absynpp_exp2doc(_tmp2B5),((_tmp829[3]=
# 1214
Cyc_PP_text(((_tmp82B=" < ",_tag_dyneither(_tmp82B,sizeof(char),4)))),((_tmp829[2]=
# 1213
Cyc_PP_text(*(*_tmp2B4->name).f2),((_tmp829[1]=
# 1212
Cyc_PP_text(((_tmp82C="for ",_tag_dyneither(_tmp82C,sizeof(char),5)))),((_tmp829[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp829,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1219
goto _LL1A6;_LL1E3: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp2B7=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp2B7->tag != 27)goto _LL1E5;else{_tmp2B8=_tmp2B7->f1;_tmp2B9=(void*)_tmp2B7->f2;}}_LL1E4:
# 1221
{const char*_tmp833;const char*_tmp832;const char*_tmp831;struct Cyc_PP_Doc*_tmp830[7];s=((_tmp830[6]=
# 1226
Cyc_Absynpp_rb(),((_tmp830[5]=
# 1225
Cyc_Absynpp_typ2doc(_tmp2B9),((_tmp830[4]=
# 1224
Cyc_PP_text(((_tmp831=" : ",_tag_dyneither(_tmp831,sizeof(char),4)))),((_tmp830[3]=
# 1223
Cyc_Absynpp_exp2doc(_tmp2B8),((_tmp830[2]=
# 1222
Cyc_PP_text(((_tmp832=" < ",_tag_dyneither(_tmp832,sizeof(char),4)))),((_tmp830[1]=
# 1221
Cyc_PP_text(((_tmp833="for x ",_tag_dyneither(_tmp833,sizeof(char),7)))),((_tmp830[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp830,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1227
goto _LL1A6;_LL1E5: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp2BA=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp2BA->tag != 28)goto _LL1E7;else{_tmp2BB=_tmp2BA->f1;_tmp2BC=_tmp2BA->f2;_tmp2BD=_tmp2BA->f3;}}_LL1E6: {
# 1229
struct Cyc_List_List*_tmp339=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2BD);
{const char*_tmp838;struct Cyc_List_List*_tmp837;struct Cyc_PP_Doc*_tmp836[2];s=((_tmp836[1]=
Cyc_Absynpp_group_braces(((_tmp838=",",_tag_dyneither(_tmp838,sizeof(char),2))),
_tmp2BC != 0?(_tmp837=_cycalloc(sizeof(*_tmp837)),((_tmp837->hd=Cyc_Absynpp_tps2doc(_tmp2BC),((_tmp837->tl=_tmp339,_tmp837))))): _tmp339),((_tmp836[0]=
# 1230
Cyc_Absynpp_qvar2doc(_tmp2BB),Cyc_PP_cat(_tag_dyneither(_tmp836,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1233
goto _LL1A6;}_LL1E7: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp2BE=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp2BE->tag != 29)goto _LL1E9;else{_tmp2BF=_tmp2BE->f2;}}_LL1E8:
# 1235
{const char*_tmp839;s=Cyc_Absynpp_group_braces(((_tmp839=",",_tag_dyneither(_tmp839,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2BF));}
goto _LL1A6;_LL1E9: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp2C0=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp2C0->tag != 30)goto _LL1EB;else{_tmp2C1=_tmp2C0->f1;_tmp2C2=_tmp2C0->f3;}}_LL1EA:
# 1238
 if(_tmp2C1 == 0)
# 1240
s=Cyc_Absynpp_qvar2doc(_tmp2C2->name);else{
# 1242
const char*_tmp840;const char*_tmp83F;const char*_tmp83E;struct Cyc_PP_Doc*_tmp83D[2];s=((_tmp83D[1]=
Cyc_PP_egroup(((_tmp840="(",_tag_dyneither(_tmp840,sizeof(char),2))),((_tmp83F=")",_tag_dyneither(_tmp83F,sizeof(char),2))),((_tmp83E=",",_tag_dyneither(_tmp83E,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp2C1)),((_tmp83D[0]=
# 1242
Cyc_Absynpp_qvar2doc(_tmp2C2->name),Cyc_PP_cat(_tag_dyneither(_tmp83D,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1244
goto _LL1A6;_LL1EB: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp2C3=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp2C3->tag != 31)goto _LL1ED;else{_tmp2C4=_tmp2C3->f1;}}_LL1EC:
# 1246
 s=Cyc_Absynpp_qvar2doc(_tmp2C4);
goto _LL1A6;_LL1ED: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp2C5=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp2C5->tag != 32)goto _LL1EF;else{_tmp2C6=_tmp2C5->f1;}}_LL1EE:
# 1249
 s=Cyc_Absynpp_qvar2doc(_tmp2C6);
goto _LL1A6;_LL1EF: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2C7=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp2C7->tag != 33)goto _LL1F1;else{_tmp2C8=(_tmp2C7->f1).is_calloc;_tmp2C9=(_tmp2C7->f1).rgn;_tmp2CA=(_tmp2C7->f1).elt_type;_tmp2CB=(_tmp2C7->f1).num_elts;}}_LL1F0:
# 1254
 if(_tmp2C8){
# 1256
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp2CA)),0);
if(_tmp2C9 == 0){
const char*_tmp847;const char*_tmp846;const char*_tmp845;struct Cyc_PP_Doc*_tmp844[5];s=((_tmp844[4]=Cyc_PP_text(((_tmp845=")",_tag_dyneither(_tmp845,sizeof(char),2)))),((_tmp844[3]=Cyc_Absynpp_exp2doc(st),((_tmp844[2]=Cyc_PP_text(((_tmp846=",",_tag_dyneither(_tmp846,sizeof(char),2)))),((_tmp844[1]=Cyc_Absynpp_exp2doc(_tmp2CB),((_tmp844[0]=Cyc_PP_text(((_tmp847="calloc(",_tag_dyneither(_tmp847,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp844,sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{
# 1260
const char*_tmp850;const char*_tmp84F;const char*_tmp84E;const char*_tmp84D;struct Cyc_PP_Doc*_tmp84C[7];s=((_tmp84C[6]=
Cyc_PP_text(((_tmp84D=")",_tag_dyneither(_tmp84D,sizeof(char),2)))),((_tmp84C[5]=Cyc_Absynpp_exp2doc(st),((_tmp84C[4]=Cyc_PP_text(((_tmp84E=",",_tag_dyneither(_tmp84E,sizeof(char),2)))),((_tmp84C[3]=Cyc_Absynpp_exp2doc(_tmp2CB),((_tmp84C[2]=
# 1260
Cyc_PP_text(((_tmp84F=",",_tag_dyneither(_tmp84F,sizeof(char),2)))),((_tmp84C[1]=Cyc_Absynpp_exp2doc(_tmp2C9),((_tmp84C[0]=Cyc_PP_text(((_tmp850="rcalloc(",_tag_dyneither(_tmp850,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp84C,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1263
struct Cyc_Absyn_Exp*new_e;
# 1265
if(_tmp2CA == 0)
new_e=_tmp2CB;else{
# 1268
new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp2CA,0),_tmp2CB,0);}
# 1270
if(_tmp2C9 == 0){
const char*_tmp855;const char*_tmp854;struct Cyc_PP_Doc*_tmp853[3];s=((_tmp853[2]=Cyc_PP_text(((_tmp854=")",_tag_dyneither(_tmp854,sizeof(char),2)))),((_tmp853[1]=Cyc_Absynpp_exp2doc(new_e),((_tmp853[0]=Cyc_PP_text(((_tmp855="malloc(",_tag_dyneither(_tmp855,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp853,sizeof(struct Cyc_PP_Doc*),3))))))));}else{
# 1273
const char*_tmp85C;const char*_tmp85B;const char*_tmp85A;struct Cyc_PP_Doc*_tmp859[5];s=((_tmp859[4]=
Cyc_PP_text(((_tmp85A=")",_tag_dyneither(_tmp85A,sizeof(char),2)))),((_tmp859[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp859[2]=
# 1273
Cyc_PP_text(((_tmp85B=",",_tag_dyneither(_tmp85B,sizeof(char),2)))),((_tmp859[1]=Cyc_Absynpp_exp2doc(_tmp2C9),((_tmp859[0]=Cyc_PP_text(((_tmp85C="rmalloc(",_tag_dyneither(_tmp85C,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp859,sizeof(struct Cyc_PP_Doc*),5))))))))))));}}
# 1276
goto _LL1A6;_LL1F1: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp2CC=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp2CC->tag != 34)goto _LL1F3;else{_tmp2CD=_tmp2CC->f1;_tmp2CE=_tmp2CC->f2;}}_LL1F2:
# 1278
{const char*_tmp85F;struct Cyc_PP_Doc*_tmp85E[3];s=((_tmp85E[2]=
# 1280
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2CE),((_tmp85E[1]=
# 1279
Cyc_PP_text(((_tmp85F=" :=: ",_tag_dyneither(_tmp85F,sizeof(char),6)))),((_tmp85E[0]=
# 1278
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2CD),Cyc_PP_cat(_tag_dyneither(_tmp85E,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1281
goto _LL1A6;_LL1F3: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp2CF=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp2CF->tag != 35)goto _LL1F5;else{_tmp2D0=_tmp2CF->f1;_tmp2D1=_tmp2CF->f2;}}_LL1F4:
# 1284
{const char*_tmp860;s=Cyc_Absynpp_group_braces(((_tmp860=",",_tag_dyneither(_tmp860,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2D1));}
goto _LL1A6;_LL1F5: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp2D2=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp265;if(_tmp2D2->tag != 36)goto _LL1A6;else{_tmp2D3=_tmp2D2->f1;}}_LL1F6:
# 1287
{const char*_tmp865;const char*_tmp864;struct Cyc_PP_Doc*_tmp863[7];s=((_tmp863[6]=
# 1289
Cyc_PP_text(((_tmp864=")",_tag_dyneither(_tmp864,sizeof(char),2)))),((_tmp863[5]=Cyc_Absynpp_rb(),((_tmp863[4]=Cyc_PP_blank_doc(),((_tmp863[3]=
# 1288
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2D3,1)),((_tmp863[2]=
# 1287
Cyc_PP_blank_doc(),((_tmp863[1]=Cyc_Absynpp_lb(),((_tmp863[0]=Cyc_PP_text(((_tmp865="(",_tag_dyneither(_tmp865,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp863,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1290
goto _LL1A6;_LL1A6:;}
# 1292
if(inprec >= myprec){
const char*_tmp86A;const char*_tmp869;struct Cyc_PP_Doc*_tmp868[3];s=((_tmp868[2]=Cyc_PP_text(((_tmp869=")",_tag_dyneither(_tmp869,sizeof(char),2)))),((_tmp868[1]=s,((_tmp868[0]=Cyc_PP_text(((_tmp86A="(",_tag_dyneither(_tmp86A,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp868,sizeof(struct Cyc_PP_Doc*),3))))))));}
return s;}
# 1297
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp35B=d;struct Cyc_Absyn_Exp*_tmp35D;struct _dyneither_ptr*_tmp35F;_LL202: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp35C=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp35B;if(_tmp35C->tag != 0)goto _LL204;else{_tmp35D=_tmp35C->f1;}}_LL203: {
const char*_tmp86F;const char*_tmp86E;struct Cyc_PP_Doc*_tmp86D[3];return(_tmp86D[2]=Cyc_PP_text(((_tmp86E="]",_tag_dyneither(_tmp86E,sizeof(char),2)))),((_tmp86D[1]=Cyc_Absynpp_exp2doc(_tmp35D),((_tmp86D[0]=Cyc_PP_text(((_tmp86F=".[",_tag_dyneither(_tmp86F,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp86D,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL204: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp35E=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp35B;if(_tmp35E->tag != 1)goto _LL201;else{_tmp35F=_tmp35E->f1;}}_LL205: {
const char*_tmp872;struct Cyc_PP_Doc*_tmp871[2];return(_tmp871[1]=Cyc_PP_textptr(_tmp35F),((_tmp871[0]=Cyc_PP_text(((_tmp872=".",_tag_dyneither(_tmp872,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp871,sizeof(struct Cyc_PP_Doc*),2)))));}_LL201:;}
# 1304
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
const char*_tmp879;const char*_tmp878;const char*_tmp877;struct Cyc_PP_Doc*_tmp876[2];return(_tmp876[1]=
Cyc_Absynpp_exp2doc((*de).f2),((_tmp876[0]=
# 1306
Cyc_PP_egroup(((_tmp879="",_tag_dyneither(_tmp879,sizeof(char),1))),((_tmp878="=",_tag_dyneither(_tmp878,sizeof(char),2))),((_tmp877="=",_tag_dyneither(_tmp877,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp876,sizeof(struct Cyc_PP_Doc*),2)))));}}
# 1310
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
const char*_tmp87C;const char*_tmp87B;const char*_tmp87A;return Cyc_PP_group(((_tmp87A="",_tag_dyneither(_tmp87A,sizeof(char),1))),((_tmp87B="",_tag_dyneither(_tmp87B,sizeof(char),1))),((_tmp87C=",",_tag_dyneither(_tmp87C,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long i);static void _tmp881(unsigned int*_tmp880,unsigned int*_tmp87F,char**_tmp87E){for(*_tmp880=0;*_tmp880 < *_tmp87F;(*_tmp880)++){(*_tmp87E)[*_tmp880]=(char)'z';}}
# 1314
struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
char*_tmp886;unsigned int _tmp885;char*_tmp884;unsigned int _tmp883;unsigned int _tmp882;struct _dyneither_ptr x=(_tmp886=((_tmp885=(unsigned int)28,((_tmp884=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp885 + 1)),((((_tmp883=_tmp885,((_tmp881(& _tmp882,& _tmp883,& _tmp884),_tmp884[_tmp883]=(char)0)))),_tmp884)))))),_tag_dyneither(_tmp886,sizeof(char),_get_zero_arr_size_char((void*)_tmp886,(unsigned int)28 + 1)));
{char _tmp889;char _tmp888;struct _dyneither_ptr _tmp887;(_tmp887=_dyneither_ptr_plus(x,sizeof(char),27),((_tmp888=*((char*)_check_dyneither_subscript(_tmp887,sizeof(char),0)),((_tmp889='\000',((_get_dyneither_size(_tmp887,sizeof(char))== 1  && (_tmp888 == '\000'  && _tmp889 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp887.curr)=_tmp889)))))));}
{char _tmp88C;char _tmp88B;struct _dyneither_ptr _tmp88A;(_tmp88A=_dyneither_ptr_plus(x,sizeof(char),26),((_tmp88B=*((char*)_check_dyneither_subscript(_tmp88A,sizeof(char),0)),((_tmp88C='L',((_get_dyneither_size(_tmp88A,sizeof(char))== 1  && (_tmp88B == '\000'  && _tmp88C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp88A.curr)=_tmp88C)))))));}
{char _tmp88F;char _tmp88E;struct _dyneither_ptr _tmp88D;(_tmp88D=_dyneither_ptr_plus(x,sizeof(char),25),((_tmp88E=*((char*)_check_dyneither_subscript(_tmp88D,sizeof(char),0)),((_tmp88F='L',((_get_dyneither_size(_tmp88D,sizeof(char))== 1  && (_tmp88E == '\000'  && _tmp88F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp88D.curr)=_tmp88F)))))));}
{char _tmp892;char _tmp891;struct _dyneither_ptr _tmp890;(_tmp890=_dyneither_ptr_plus(x,sizeof(char),24),((_tmp891=*((char*)_check_dyneither_subscript(_tmp890,sizeof(char),0)),((_tmp892='U',((_get_dyneither_size(_tmp890,sizeof(char))== 1  && (_tmp891 == '\000'  && _tmp892 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp890.curr)=_tmp892)))))));}
{char _tmp895;char _tmp894;struct _dyneither_ptr _tmp893;(_tmp893=_dyneither_ptr_plus(x,sizeof(char),23),((_tmp894=*((char*)_check_dyneither_subscript(_tmp893,sizeof(char),0)),((_tmp895='0',((_get_dyneither_size(_tmp893,sizeof(char))== 1  && (_tmp894 == '\000'  && _tmp895 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp893.curr)=_tmp895)))))));}{
int index=23;
while(i != 0){
char c=(char)('0' + i % 10);
{char _tmp898;char _tmp897;struct _dyneither_ptr _tmp896;(_tmp896=_dyneither_ptr_plus(x,sizeof(char),index),((_tmp897=*((char*)_check_dyneither_subscript(_tmp896,sizeof(char),0)),((_tmp898=c,((_get_dyneither_size(_tmp896,sizeof(char))== 1  && (_tmp897 == '\000'  && _tmp898 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp896.curr)=_tmp898)))))));}
i=i / 10;
-- index;}
# 1328
return(struct _dyneither_ptr)_dyneither_ptr_plus(_dyneither_ptr_plus(x,sizeof(char),index),sizeof(char),1);};}
# 1332
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp382=c;enum Cyc_Absyn_Sign _tmp383;char _tmp384;struct _dyneither_ptr _tmp385;enum Cyc_Absyn_Sign _tmp386;short _tmp387;int _tmp388;int _tmp389;int _tmp38A;enum Cyc_Absyn_Sign _tmp38B;long long _tmp38C;struct _dyneither_ptr _tmp38D;struct _dyneither_ptr _tmp38E;struct _dyneither_ptr _tmp38F;_LL207: if((_tmp382.Char_c).tag != 2)goto _LL209;_tmp383=((struct _tuple3)(_tmp382.Char_c).val).f1;_tmp384=((struct _tuple3)(_tmp382.Char_c).val).f2;_LL208: {
const char*_tmp89C;void*_tmp89B[1];struct Cyc_String_pa_PrintArg_struct _tmp89A;return Cyc_PP_text((struct _dyneither_ptr)((_tmp89A.tag=0,((_tmp89A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp384)),((_tmp89B[0]=& _tmp89A,Cyc_aprintf(((_tmp89C="'%s'",_tag_dyneither(_tmp89C,sizeof(char),5))),_tag_dyneither(_tmp89B,sizeof(void*),1)))))))));}_LL209: if((_tmp382.Wchar_c).tag != 3)goto _LL20B;_tmp385=(struct _dyneither_ptr)(_tmp382.Wchar_c).val;_LL20A: {
const char*_tmp8A0;void*_tmp89F[1];struct Cyc_String_pa_PrintArg_struct _tmp89E;return Cyc_PP_text((struct _dyneither_ptr)((_tmp89E.tag=0,((_tmp89E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp385),((_tmp89F[0]=& _tmp89E,Cyc_aprintf(((_tmp8A0="L'%s'",_tag_dyneither(_tmp8A0,sizeof(char),6))),_tag_dyneither(_tmp89F,sizeof(void*),1)))))))));}_LL20B: if((_tmp382.Short_c).tag != 4)goto _LL20D;_tmp386=((struct _tuple4)(_tmp382.Short_c).val).f1;_tmp387=((struct _tuple4)(_tmp382.Short_c).val).f2;_LL20C: {
const char*_tmp8A4;void*_tmp8A3[1];struct Cyc_Int_pa_PrintArg_struct _tmp8A2;return Cyc_PP_text((struct _dyneither_ptr)((_tmp8A2.tag=1,((_tmp8A2.f1=(unsigned long)((int)_tmp387),((_tmp8A3[0]=& _tmp8A2,Cyc_aprintf(((_tmp8A4="%d",_tag_dyneither(_tmp8A4,sizeof(char),3))),_tag_dyneither(_tmp8A3,sizeof(void*),1)))))))));}_LL20D: if((_tmp382.Int_c).tag != 5)goto _LL20F;if(((struct _tuple5)(_tmp382.Int_c).val).f1 != Cyc_Absyn_None)goto _LL20F;_tmp388=((struct _tuple5)(_tmp382.Int_c).val).f2;_LL20E:
 _tmp389=_tmp388;goto _LL210;_LL20F: if((_tmp382.Int_c).tag != 5)goto _LL211;if(((struct _tuple5)(_tmp382.Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL211;_tmp389=((struct _tuple5)(_tmp382.Int_c).val).f2;_LL210: {
const char*_tmp8A8;void*_tmp8A7[1];struct Cyc_Int_pa_PrintArg_struct _tmp8A6;return Cyc_PP_text((struct _dyneither_ptr)((_tmp8A6.tag=1,((_tmp8A6.f1=(unsigned long)_tmp389,((_tmp8A7[0]=& _tmp8A6,Cyc_aprintf(((_tmp8A8="%d",_tag_dyneither(_tmp8A8,sizeof(char),3))),_tag_dyneither(_tmp8A7,sizeof(void*),1)))))))));}_LL211: if((_tmp382.Int_c).tag != 5)goto _LL213;if(((struct _tuple5)(_tmp382.Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL213;_tmp38A=((struct _tuple5)(_tmp382.Int_c).val).f2;_LL212: {
const char*_tmp8AC;void*_tmp8AB[1];struct Cyc_Int_pa_PrintArg_struct _tmp8AA;return Cyc_PP_text((struct _dyneither_ptr)((_tmp8AA.tag=1,((_tmp8AA.f1=(unsigned int)_tmp38A,((_tmp8AB[0]=& _tmp8AA,Cyc_aprintf(((_tmp8AC="%u",_tag_dyneither(_tmp8AC,sizeof(char),3))),_tag_dyneither(_tmp8AB,sizeof(void*),1)))))))));}_LL213: if((_tmp382.LongLong_c).tag != 6)goto _LL215;_tmp38B=((struct _tuple6)(_tmp382.LongLong_c).val).f1;_tmp38C=((struct _tuple6)(_tmp382.LongLong_c).val).f2;_LL214:
# 1342
 return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)_tmp38C));_LL215: if((_tmp382.Float_c).tag != 7)goto _LL217;_tmp38D=((struct _tuple7)(_tmp382.Float_c).val).f1;_LL216:
 return Cyc_PP_text(_tmp38D);_LL217: if((_tmp382.Null_c).tag != 1)goto _LL219;_LL218: {
const char*_tmp8AD;return Cyc_PP_text(((_tmp8AD="NULL",_tag_dyneither(_tmp8AD,sizeof(char),5))));}_LL219: if((_tmp382.String_c).tag != 8)goto _LL21B;_tmp38E=(struct _dyneither_ptr)(_tmp382.String_c).val;_LL21A: {
# 1346
const char*_tmp8B2;const char*_tmp8B1;struct Cyc_PP_Doc*_tmp8B0[3];return(_tmp8B0[2]=Cyc_PP_text(((_tmp8B1="\"",_tag_dyneither(_tmp8B1,sizeof(char),2)))),((_tmp8B0[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp38E)),((_tmp8B0[0]=Cyc_PP_text(((_tmp8B2="\"",_tag_dyneither(_tmp8B2,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8B0,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL21B: if((_tmp382.Wstring_c).tag != 9)goto _LL206;_tmp38F=(struct _dyneither_ptr)(_tmp382.Wstring_c).val;_LL21C: {
# 1348
const char*_tmp8B7;const char*_tmp8B6;struct Cyc_PP_Doc*_tmp8B5[3];return(_tmp8B5[2]=Cyc_PP_text(((_tmp8B6="\"",_tag_dyneither(_tmp8B6,sizeof(char),2)))),((_tmp8B5[1]=Cyc_PP_text(_tmp38F),((_tmp8B5[0]=Cyc_PP_text(((_tmp8B7="L\"",_tag_dyneither(_tmp8B7,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp8B5,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL206:;}
# 1352
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if(p == Cyc_Absyn_Numelts){
if(es == 0  || es->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp8C4;const char*_tmp8C3;void*_tmp8C2[1];struct Cyc_String_pa_PrintArg_struct _tmp8C1;struct Cyc_Core_Failure_exn_struct*_tmp8C0;(int)_throw((void*)((_tmp8C0=_cycalloc(sizeof(*_tmp8C0)),((_tmp8C0[0]=((_tmp8C4.tag=Cyc_Core_Failure,((_tmp8C4.f1=(struct _dyneither_ptr)((_tmp8C1.tag=0,((_tmp8C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp8C2[0]=& _tmp8C1,Cyc_aprintf(((_tmp8C3="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp8C3,sizeof(char),47))),_tag_dyneither(_tmp8C2,sizeof(void*),1)))))))),_tmp8C4)))),_tmp8C0)))));}{
# 1358
const char*_tmp8C9;const char*_tmp8C8;struct Cyc_PP_Doc*_tmp8C7[3];return(_tmp8C7[2]=Cyc_PP_text(((_tmp8C8=")",_tag_dyneither(_tmp8C8,sizeof(char),2)))),((_tmp8C7[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp8C7[0]=Cyc_PP_text(((_tmp8C9="numelts(",_tag_dyneither(_tmp8C9,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8C7,sizeof(struct Cyc_PP_Doc*),3)))))));};}else{
# 1360
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0){
struct Cyc_Core_Failure_exn_struct _tmp8D6;const char*_tmp8D5;void*_tmp8D4[1];struct Cyc_String_pa_PrintArg_struct _tmp8D3;struct Cyc_Core_Failure_exn_struct*_tmp8D2;(int)_throw((void*)((_tmp8D2=_cycalloc(sizeof(*_tmp8D2)),((_tmp8D2[0]=((_tmp8D6.tag=Cyc_Core_Failure,((_tmp8D6.f1=(struct _dyneither_ptr)((_tmp8D3.tag=0,((_tmp8D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp8D4[0]=& _tmp8D3,Cyc_aprintf(((_tmp8D5="Absynpp::primapp2doc: %s with no args",_tag_dyneither(_tmp8D5,sizeof(char),38))),_tag_dyneither(_tmp8D4,sizeof(void*),1)))))))),_tmp8D6)))),_tmp8D2)))));}else{
# 1364
if(ds->tl == 0){
const char*_tmp8D9;struct Cyc_PP_Doc*_tmp8D8[3];return(_tmp8D8[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp8D8[1]=Cyc_PP_text(((_tmp8D9=" ",_tag_dyneither(_tmp8D9,sizeof(char),2)))),((_tmp8D8[0]=ps,Cyc_PP_cat(_tag_dyneither(_tmp8D8,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp8E6;const char*_tmp8E5;void*_tmp8E4[1];struct Cyc_String_pa_PrintArg_struct _tmp8E3;struct Cyc_Core_Failure_exn_struct*_tmp8E2;(int)_throw((void*)((_tmp8E2=_cycalloc(sizeof(*_tmp8E2)),((_tmp8E2[0]=((_tmp8E6.tag=Cyc_Core_Failure,((_tmp8E6.f1=(struct _dyneither_ptr)((_tmp8E3.tag=0,((_tmp8E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp8E4[0]=& _tmp8E3,Cyc_aprintf(((_tmp8E5="Absynpp::primapp2doc: %s with more than 2 args",_tag_dyneither(_tmp8E5,sizeof(char),47))),_tag_dyneither(_tmp8E4,sizeof(void*),1)))))))),_tmp8E6)))),_tmp8E2)))));}else{
# 1370
const char*_tmp8EB;const char*_tmp8EA;struct Cyc_PP_Doc*_tmp8E9[5];return(_tmp8E9[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd,((_tmp8E9[3]=Cyc_PP_text(((_tmp8EA=" ",_tag_dyneither(_tmp8EA,sizeof(char),2)))),((_tmp8E9[2]=ps,((_tmp8E9[1]=Cyc_PP_text(((_tmp8EB=" ",_tag_dyneither(_tmp8EB,sizeof(char),2)))),((_tmp8E9[0]=(struct Cyc_PP_Doc*)ds->hd,Cyc_PP_cat(_tag_dyneither(_tmp8E9,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
# 1374
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
switch(p){case Cyc_Absyn_Plus: _LL21D: {
const char*_tmp8EC;return(_tmp8EC="+",_tag_dyneither(_tmp8EC,sizeof(char),2));}case Cyc_Absyn_Times: _LL21E: {
const char*_tmp8ED;return(_tmp8ED="*",_tag_dyneither(_tmp8ED,sizeof(char),2));}case Cyc_Absyn_Minus: _LL21F: {
const char*_tmp8EE;return(_tmp8EE="-",_tag_dyneither(_tmp8EE,sizeof(char),2));}case Cyc_Absyn_Div: _LL220: {
const char*_tmp8EF;return(_tmp8EF="/",_tag_dyneither(_tmp8EF,sizeof(char),2));}case Cyc_Absyn_Mod: _LL221: {
const char*_tmp8F1;const char*_tmp8F0;return Cyc_Absynpp_print_for_cycdoc?(_tmp8F1="\\%",_tag_dyneither(_tmp8F1,sizeof(char),3)):((_tmp8F0="%",_tag_dyneither(_tmp8F0,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL222: {
const char*_tmp8F2;return(_tmp8F2="==",_tag_dyneither(_tmp8F2,sizeof(char),3));}case Cyc_Absyn_Neq: _LL223: {
const char*_tmp8F3;return(_tmp8F3="!=",_tag_dyneither(_tmp8F3,sizeof(char),3));}case Cyc_Absyn_Gt: _LL224: {
const char*_tmp8F4;return(_tmp8F4=">",_tag_dyneither(_tmp8F4,sizeof(char),2));}case Cyc_Absyn_Lt: _LL225: {
const char*_tmp8F5;return(_tmp8F5="<",_tag_dyneither(_tmp8F5,sizeof(char),2));}case Cyc_Absyn_Gte: _LL226: {
const char*_tmp8F6;return(_tmp8F6=">=",_tag_dyneither(_tmp8F6,sizeof(char),3));}case Cyc_Absyn_Lte: _LL227: {
const char*_tmp8F7;return(_tmp8F7="<=",_tag_dyneither(_tmp8F7,sizeof(char),3));}case Cyc_Absyn_Not: _LL228: {
const char*_tmp8F8;return(_tmp8F8="!",_tag_dyneither(_tmp8F8,sizeof(char),2));}case Cyc_Absyn_Bitnot: _LL229: {
const char*_tmp8F9;return(_tmp8F9="~",_tag_dyneither(_tmp8F9,sizeof(char),2));}case Cyc_Absyn_Bitand: _LL22A: {
const char*_tmp8FA;return(_tmp8FA="&",_tag_dyneither(_tmp8FA,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL22B: {
const char*_tmp8FB;return(_tmp8FB="|",_tag_dyneither(_tmp8FB,sizeof(char),2));}case Cyc_Absyn_Bitxor: _LL22C: {
const char*_tmp8FC;return(_tmp8FC="^",_tag_dyneither(_tmp8FC,sizeof(char),2));}case Cyc_Absyn_Bitlshift: _LL22D: {
const char*_tmp8FD;return(_tmp8FD="<<",_tag_dyneither(_tmp8FD,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL22E: {
const char*_tmp8FE;return(_tmp8FE=">>",_tag_dyneither(_tmp8FE,sizeof(char),3));}case Cyc_Absyn_Bitarshift: _LL22F: {
const char*_tmp8FF;return(_tmp8FF=">>>",_tag_dyneither(_tmp8FF,sizeof(char),4));}case Cyc_Absyn_Numelts: _LL230: {
const char*_tmp900;return(_tmp900="numelts",_tag_dyneither(_tmp900,sizeof(char),8));}}}
# 1399
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1403
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp3D2=s->r;void*_tmp3D3=_tmp3D2;_LL233: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3D4=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3D3;if(_tmp3D4->tag != 12)goto _LL235;}_LL234:
 return 1;_LL235:;_LL236:
 return 0;_LL232:;}
# 1410
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp){
struct Cyc_PP_Doc*s;
{void*_tmp3D5=st->r;void*_tmp3D6=_tmp3D5;struct Cyc_Absyn_Exp*_tmp3D9;struct Cyc_Absyn_Stmt*_tmp3DB;struct Cyc_Absyn_Stmt*_tmp3DC;struct Cyc_Absyn_Exp*_tmp3DE;struct Cyc_Absyn_Exp*_tmp3E0;struct Cyc_Absyn_Stmt*_tmp3E1;struct Cyc_Absyn_Stmt*_tmp3E2;struct Cyc_Absyn_Exp*_tmp3E4;struct Cyc_Absyn_Stmt*_tmp3E5;struct _dyneither_ptr*_tmp3E9;struct Cyc_Absyn_Exp*_tmp3EB;struct Cyc_Absyn_Exp*_tmp3EC;struct Cyc_Absyn_Exp*_tmp3ED;struct Cyc_Absyn_Stmt*_tmp3EE;struct Cyc_Absyn_Exp*_tmp3F0;struct Cyc_List_List*_tmp3F1;struct Cyc_List_List*_tmp3F4;struct Cyc_Absyn_Decl*_tmp3F6;struct Cyc_Absyn_Stmt*_tmp3F7;struct _dyneither_ptr*_tmp3F9;struct Cyc_Absyn_Stmt*_tmp3FA;struct Cyc_Absyn_Stmt*_tmp3FC;struct Cyc_Absyn_Exp*_tmp3FD;struct Cyc_Absyn_Stmt*_tmp3FF;struct Cyc_List_List*_tmp400;struct Cyc_Absyn_Exp*_tmp402;_LL238: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp3D7=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3D7->tag != 0)goto _LL23A;}_LL239:
{const char*_tmp901;s=Cyc_PP_text(((_tmp901=";",_tag_dyneither(_tmp901,sizeof(char),2))));}goto _LL237;_LL23A: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3D8=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3D8->tag != 1)goto _LL23C;else{_tmp3D9=_tmp3D8->f1;}}_LL23B:
{const char*_tmp904;struct Cyc_PP_Doc*_tmp903[2];s=((_tmp903[1]=Cyc_PP_text(((_tmp904=";",_tag_dyneither(_tmp904,sizeof(char),2)))),((_tmp903[0]=Cyc_Absynpp_exp2doc(_tmp3D9),Cyc_PP_cat(_tag_dyneither(_tmp903,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL237;_LL23C: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3DA=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3DA->tag != 2)goto _LL23E;else{_tmp3DB=_tmp3DA->f1;_tmp3DC=_tmp3DA->f2;}}_LL23D:
# 1416
 if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp3DB)){
struct Cyc_PP_Doc*_tmp90F[5];struct Cyc_PP_Doc*_tmp90E[7];const char*_tmp90D;const char*_tmp90C;struct Cyc_PP_Doc*_tmp90B[7];s=((_tmp90B[6]=
# 1424
Cyc_Absynpp_is_declaration(_tmp3DC)?
stmtexp?
(_tmp90E[6]=
# 1429
Cyc_PP_line_doc(),((_tmp90E[5]=
# 1428
Cyc_PP_text(((_tmp90D=");",_tag_dyneither(_tmp90D,sizeof(char),3)))),((_tmp90E[4]=Cyc_Absynpp_rb(),((_tmp90E[3]=
# 1427
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3DC,stmtexp)),((_tmp90E[2]=
# 1426
Cyc_PP_blank_doc(),((_tmp90E[1]=Cyc_Absynpp_lb(),((_tmp90E[0]=Cyc_PP_text(((_tmp90C="(",_tag_dyneither(_tmp90C,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp90E,sizeof(struct Cyc_PP_Doc*),7))))))))))))))):(
# 1430
(_tmp90F[4]=
# 1433
Cyc_PP_line_doc(),((_tmp90F[3]=
# 1432
Cyc_Absynpp_rb(),((_tmp90F[2]=
# 1431
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3DC,stmtexp)),((_tmp90F[1]=
# 1430
Cyc_PP_blank_doc(),((_tmp90F[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp90F,sizeof(struct Cyc_PP_Doc*),5)))))))))))):
# 1434
 Cyc_Absynpp_stmt2doc(_tmp3DC,stmtexp),((_tmp90B[5]=
# 1423
Cyc_PP_line_doc(),((_tmp90B[4]=
# 1422
Cyc_Absynpp_rb(),((_tmp90B[3]=
# 1421
Cyc_PP_line_doc(),((_tmp90B[2]=
# 1420
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3DB,0)),((_tmp90B[1]=
# 1419
Cyc_PP_blank_doc(),((_tmp90B[0]=
# 1418
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp90B,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1435
if(Cyc_Absynpp_is_declaration(_tmp3DC)){
struct Cyc_PP_Doc*_tmp91A[4];struct Cyc_PP_Doc*_tmp919[6];const char*_tmp918;const char*_tmp917;struct Cyc_PP_Doc*_tmp916[4];s=((_tmp916[3]=
# 1446
Cyc_PP_line_doc(),((_tmp916[2]=
# 1438
stmtexp?
(_tmp919[5]=
# 1441
Cyc_PP_text(((_tmp918=");",_tag_dyneither(_tmp918,sizeof(char),3)))),((_tmp919[4]=Cyc_Absynpp_rb(),((_tmp919[3]=
# 1440
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3DC,stmtexp)),((_tmp919[2]=
# 1439
Cyc_PP_blank_doc(),((_tmp919[1]=Cyc_Absynpp_lb(),((_tmp919[0]=Cyc_PP_text(((_tmp917="(",_tag_dyneither(_tmp917,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp919,sizeof(struct Cyc_PP_Doc*),6))))))))))))):(
# 1443
(_tmp91A[3]=
# 1445
Cyc_Absynpp_rb(),((_tmp91A[2]=
# 1444
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3DC,stmtexp)),((_tmp91A[1]=
# 1443
Cyc_PP_blank_doc(),((_tmp91A[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp91A,sizeof(struct Cyc_PP_Doc*),4)))))))))),((_tmp916[1]=
# 1437
Cyc_PP_line_doc(),((_tmp916[0]=
# 1436
Cyc_Absynpp_stmt2doc(_tmp3DB,0),Cyc_PP_cat(_tag_dyneither(_tmp916,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1448
struct Cyc_PP_Doc*_tmp91B[3];s=((_tmp91B[2]=Cyc_Absynpp_stmt2doc(_tmp3DC,stmtexp),((_tmp91B[1]=Cyc_PP_line_doc(),((_tmp91B[0]=Cyc_Absynpp_stmt2doc(_tmp3DB,0),Cyc_PP_cat(_tag_dyneither(_tmp91B,sizeof(struct Cyc_PP_Doc*),3))))))));}}}else{
# 1451
struct Cyc_PP_Doc*_tmp91C[3];s=((_tmp91C[2]=Cyc_Absynpp_stmt2doc(_tmp3DC,stmtexp),((_tmp91C[1]=Cyc_PP_line_doc(),((_tmp91C[0]=Cyc_Absynpp_stmt2doc(_tmp3DB,0),Cyc_PP_cat(_tag_dyneither(_tmp91C,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL237;_LL23E: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp3DD=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3DD->tag != 3)goto _LL240;else{_tmp3DE=_tmp3DD->f1;}}_LL23F:
# 1454
 if(_tmp3DE == 0){
const char*_tmp91D;s=Cyc_PP_text(((_tmp91D="return;",_tag_dyneither(_tmp91D,sizeof(char),8))));}else{
# 1457
const char*_tmp922;const char*_tmp921;struct Cyc_PP_Doc*_tmp920[3];s=((_tmp920[2]=
# 1459
Cyc_PP_text(((_tmp921=";",_tag_dyneither(_tmp921,sizeof(char),2)))),((_tmp920[1]=
# 1458
_tmp3DE == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(_tmp3DE),((_tmp920[0]=
# 1457
Cyc_PP_text(((_tmp922="return ",_tag_dyneither(_tmp922,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp920,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1460
goto _LL237;_LL240: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp3DF=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3DF->tag != 4)goto _LL242;else{_tmp3E0=_tmp3DF->f1;_tmp3E1=_tmp3DF->f2;_tmp3E2=_tmp3DF->f3;}}_LL241: {
# 1462
int print_else;
{void*_tmp416=_tmp3E2->r;void*_tmp417=_tmp416;_LL25D: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp418=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp417;if(_tmp418->tag != 0)goto _LL25F;}_LL25E:
 print_else=0;goto _LL25C;_LL25F:;_LL260:
 print_else=1;goto _LL25C;_LL25C:;}
# 1467
{const char*_tmp931;const char*_tmp930;struct Cyc_PP_Doc*_tmp92F[2];struct Cyc_PP_Doc*_tmp92E[6];struct Cyc_PP_Doc*_tmp92D[2];const char*_tmp92C;struct Cyc_PP_Doc*_tmp92B[8];s=((_tmp92B[7]=
# 1474
print_else?
(_tmp92E[5]=
# 1480
Cyc_Absynpp_rb(),((_tmp92E[4]=
# 1479
Cyc_PP_line_doc(),((_tmp92E[3]=
# 1478
Cyc_PP_nest(2,((_tmp92D[1]=Cyc_Absynpp_stmt2doc(_tmp3E2,0),((_tmp92D[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp92D,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp92E[2]=
# 1477
Cyc_Absynpp_lb(),((_tmp92E[1]=
# 1476
Cyc_PP_text(((_tmp92C="else ",_tag_dyneither(_tmp92C,sizeof(char),6)))),((_tmp92E[0]=
# 1475
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp92E,sizeof(struct Cyc_PP_Doc*),6))))))))))))):
# 1481
 Cyc_PP_nil_doc(),((_tmp92B[6]=
# 1473
Cyc_Absynpp_rb(),((_tmp92B[5]=
# 1472
Cyc_PP_line_doc(),((_tmp92B[4]=
# 1471
Cyc_PP_nest(2,((_tmp92F[1]=Cyc_Absynpp_stmt2doc(_tmp3E1,0),((_tmp92F[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp92F,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp92B[3]=
# 1470
Cyc_Absynpp_lb(),((_tmp92B[2]=
# 1469
Cyc_PP_text(((_tmp930=") ",_tag_dyneither(_tmp930,sizeof(char),3)))),((_tmp92B[1]=
# 1468
Cyc_Absynpp_exp2doc(_tmp3E0),((_tmp92B[0]=
# 1467
Cyc_PP_text(((_tmp931="if (",_tag_dyneither(_tmp931,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp92B,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1482
goto _LL237;}_LL242: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp3E3=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3E3->tag != 5)goto _LL244;else{_tmp3E4=(_tmp3E3->f1).f1;_tmp3E5=_tmp3E3->f2;}}_LL243:
# 1484
{const char*_tmp938;const char*_tmp937;struct Cyc_PP_Doc*_tmp936[2];struct Cyc_PP_Doc*_tmp935[7];s=((_tmp935[6]=
# 1490
Cyc_Absynpp_rb(),((_tmp935[5]=
# 1489
Cyc_PP_line_doc(),((_tmp935[4]=
# 1488
Cyc_PP_nest(2,((_tmp936[1]=Cyc_Absynpp_stmt2doc(_tmp3E5,0),((_tmp936[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp936,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp935[3]=
# 1487
Cyc_Absynpp_lb(),((_tmp935[2]=
# 1486
Cyc_PP_text(((_tmp937=") ",_tag_dyneither(_tmp937,sizeof(char),3)))),((_tmp935[1]=
# 1485
Cyc_Absynpp_exp2doc(_tmp3E4),((_tmp935[0]=
# 1484
Cyc_PP_text(((_tmp938="while (",_tag_dyneither(_tmp938,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp935,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1491
goto _LL237;_LL244: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp3E6=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3E6->tag != 6)goto _LL246;}_LL245:
{const char*_tmp939;s=Cyc_PP_text(((_tmp939="break;",_tag_dyneither(_tmp939,sizeof(char),7))));}goto _LL237;_LL246: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp3E7=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3E7->tag != 7)goto _LL248;}_LL247:
{const char*_tmp93A;s=Cyc_PP_text(((_tmp93A="continue;",_tag_dyneither(_tmp93A,sizeof(char),10))));}goto _LL237;_LL248: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp3E8=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3E8->tag != 8)goto _LL24A;else{_tmp3E9=_tmp3E8->f1;}}_LL249:
{const char*_tmp93E;void*_tmp93D[1];struct Cyc_String_pa_PrintArg_struct _tmp93C;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp93C.tag=0,((_tmp93C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3E9),((_tmp93D[0]=& _tmp93C,Cyc_aprintf(((_tmp93E="goto %s;",_tag_dyneither(_tmp93E,sizeof(char),9))),_tag_dyneither(_tmp93D,sizeof(void*),1)))))))));}goto _LL237;_LL24A: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp3EA=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3EA->tag != 9)goto _LL24C;else{_tmp3EB=_tmp3EA->f1;_tmp3EC=(_tmp3EA->f2).f1;_tmp3ED=(_tmp3EA->f3).f1;_tmp3EE=_tmp3EA->f4;}}_LL24B:
# 1496
{const char*_tmp949;const char*_tmp948;const char*_tmp947;const char*_tmp946;struct Cyc_PP_Doc*_tmp945[2];struct Cyc_PP_Doc*_tmp944[11];s=((_tmp944[10]=
# 1506
Cyc_Absynpp_rb(),((_tmp944[9]=
# 1505
Cyc_PP_line_doc(),((_tmp944[8]=
# 1504
Cyc_PP_nest(2,((_tmp945[1]=Cyc_Absynpp_stmt2doc(_tmp3EE,0),((_tmp945[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp945,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp944[7]=
# 1503
Cyc_Absynpp_lb(),((_tmp944[6]=
# 1502
Cyc_PP_text(((_tmp946=") ",_tag_dyneither(_tmp946,sizeof(char),3)))),((_tmp944[5]=
# 1501
Cyc_Absynpp_exp2doc(_tmp3ED),((_tmp944[4]=
# 1500
Cyc_PP_text(((_tmp947="; ",_tag_dyneither(_tmp947,sizeof(char),3)))),((_tmp944[3]=
# 1499
Cyc_Absynpp_exp2doc(_tmp3EC),((_tmp944[2]=
# 1498
Cyc_PP_text(((_tmp948="; ",_tag_dyneither(_tmp948,sizeof(char),3)))),((_tmp944[1]=
# 1497
Cyc_Absynpp_exp2doc(_tmp3EB),((_tmp944[0]=
# 1496
Cyc_PP_text(((_tmp949="for(",_tag_dyneither(_tmp949,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp944,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1507
goto _LL237;_LL24C: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp3EF=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3EF->tag != 10)goto _LL24E;else{_tmp3F0=_tmp3EF->f1;_tmp3F1=_tmp3EF->f2;}}_LL24D:
# 1509
{const char*_tmp94E;const char*_tmp94D;struct Cyc_PP_Doc*_tmp94C[8];s=((_tmp94C[7]=
# 1516
Cyc_Absynpp_rb(),((_tmp94C[6]=
# 1515
Cyc_PP_line_doc(),((_tmp94C[5]=
# 1514
Cyc_Absynpp_switchclauses2doc(_tmp3F1),((_tmp94C[4]=
# 1513
Cyc_PP_line_doc(),((_tmp94C[3]=
# 1512
Cyc_Absynpp_lb(),((_tmp94C[2]=
# 1511
Cyc_PP_text(((_tmp94D=") ",_tag_dyneither(_tmp94D,sizeof(char),3)))),((_tmp94C[1]=
# 1510
Cyc_Absynpp_exp2doc(_tmp3F0),((_tmp94C[0]=
# 1509
Cyc_PP_text(((_tmp94E="switch (",_tag_dyneither(_tmp94E,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp94C,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1517
goto _LL237;_LL24E: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3F2=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3F2->tag != 11)goto _LL250;else{if(_tmp3F2->f1 != 0)goto _LL250;}}_LL24F:
{const char*_tmp94F;s=Cyc_PP_text(((_tmp94F="fallthru;",_tag_dyneither(_tmp94F,sizeof(char),10))));}goto _LL237;_LL250: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3F3=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3F3->tag != 11)goto _LL252;else{_tmp3F4=_tmp3F3->f1;}}_LL251:
# 1520
{const char*_tmp954;const char*_tmp953;struct Cyc_PP_Doc*_tmp952[3];s=((_tmp952[2]=Cyc_PP_text(((_tmp953=");",_tag_dyneither(_tmp953,sizeof(char),3)))),((_tmp952[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp3F4),((_tmp952[0]=Cyc_PP_text(((_tmp954="fallthru(",_tag_dyneither(_tmp954,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp952,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL237;_LL252: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3F5=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3F5->tag != 12)goto _LL254;else{_tmp3F6=_tmp3F5->f1;_tmp3F7=_tmp3F5->f2;}}_LL253:
# 1522
{struct Cyc_PP_Doc*_tmp955[3];s=((_tmp955[2]=
# 1524
Cyc_Absynpp_stmt2doc(_tmp3F7,stmtexp),((_tmp955[1]=
# 1523
Cyc_PP_line_doc(),((_tmp955[0]=
# 1522
Cyc_Absynpp_decl2doc(_tmp3F6),Cyc_PP_cat(_tag_dyneither(_tmp955,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1525
goto _LL237;_LL254: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp3F8=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3F8->tag != 13)goto _LL256;else{_tmp3F9=_tmp3F8->f1;_tmp3FA=_tmp3F8->f2;}}_LL255:
# 1527
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3FA)){
if(stmtexp){
const char*_tmp95A;const char*_tmp959;struct Cyc_PP_Doc*_tmp958[8];s=((_tmp958[7]=
# 1535
Cyc_PP_text(((_tmp959=");",_tag_dyneither(_tmp959,sizeof(char),3)))),((_tmp958[6]=Cyc_Absynpp_rb(),((_tmp958[5]=
# 1534
Cyc_PP_line_doc(),((_tmp958[4]=
# 1533
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3FA,1)),((_tmp958[3]=
# 1532
Cyc_PP_line_doc(),((_tmp958[2]=
# 1531
Cyc_Absynpp_lb(),((_tmp958[1]=
# 1530
Cyc_PP_text(((_tmp95A=": (",_tag_dyneither(_tmp95A,sizeof(char),4)))),((_tmp958[0]=
# 1529
Cyc_PP_textptr(_tmp3F9),Cyc_PP_cat(_tag_dyneither(_tmp958,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1537
const char*_tmp95D;struct Cyc_PP_Doc*_tmp95C[7];s=((_tmp95C[6]=
# 1543
Cyc_Absynpp_rb(),((_tmp95C[5]=
# 1542
Cyc_PP_line_doc(),((_tmp95C[4]=
# 1541
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3FA,0)),((_tmp95C[3]=
# 1540
Cyc_PP_line_doc(),((_tmp95C[2]=
# 1539
Cyc_Absynpp_lb(),((_tmp95C[1]=
# 1538
Cyc_PP_text(((_tmp95D=": ",_tag_dyneither(_tmp95D,sizeof(char),3)))),((_tmp95C[0]=
# 1537
Cyc_PP_textptr(_tmp3F9),Cyc_PP_cat(_tag_dyneither(_tmp95C,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1545
const char*_tmp960;struct Cyc_PP_Doc*_tmp95F[3];s=((_tmp95F[2]=Cyc_Absynpp_stmt2doc(_tmp3FA,stmtexp),((_tmp95F[1]=Cyc_PP_text(((_tmp960=": ",_tag_dyneither(_tmp960,sizeof(char),3)))),((_tmp95F[0]=Cyc_PP_textptr(_tmp3F9),Cyc_PP_cat(_tag_dyneither(_tmp95F,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL237;_LL256: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp3FB=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3FB->tag != 14)goto _LL258;else{_tmp3FC=_tmp3FB->f1;_tmp3FD=(_tmp3FB->f2).f1;}}_LL257:
# 1548
{const char*_tmp967;const char*_tmp966;const char*_tmp965;struct Cyc_PP_Doc*_tmp964[9];s=((_tmp964[8]=
# 1556
Cyc_PP_text(((_tmp965=");",_tag_dyneither(_tmp965,sizeof(char),3)))),((_tmp964[7]=
# 1555
Cyc_Absynpp_exp2doc(_tmp3FD),((_tmp964[6]=
# 1554
Cyc_PP_text(((_tmp966=" while (",_tag_dyneither(_tmp966,sizeof(char),9)))),((_tmp964[5]=
# 1553
Cyc_Absynpp_rb(),((_tmp964[4]=
# 1552
Cyc_PP_line_doc(),((_tmp964[3]=
# 1551
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3FC,0)),((_tmp964[2]=
# 1550
Cyc_PP_line_doc(),((_tmp964[1]=
# 1549
Cyc_Absynpp_lb(),((_tmp964[0]=
# 1548
Cyc_PP_text(((_tmp967="do ",_tag_dyneither(_tmp967,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp964,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1557
goto _LL237;_LL258: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp3FE=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3FE->tag != 15)goto _LL25A;else{_tmp3FF=_tmp3FE->f1;_tmp400=_tmp3FE->f2;}}_LL259:
# 1559
{const char*_tmp96C;const char*_tmp96B;struct Cyc_PP_Doc*_tmp96A[12];s=((_tmp96A[11]=
# 1570
Cyc_Absynpp_rb(),((_tmp96A[10]=
# 1569
Cyc_PP_line_doc(),((_tmp96A[9]=
# 1568
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp400)),((_tmp96A[8]=
# 1567
Cyc_PP_line_doc(),((_tmp96A[7]=
# 1566
Cyc_Absynpp_lb(),((_tmp96A[6]=
# 1565
Cyc_PP_text(((_tmp96B=" catch ",_tag_dyneither(_tmp96B,sizeof(char),8)))),((_tmp96A[5]=
# 1564
Cyc_Absynpp_rb(),((_tmp96A[4]=
# 1563
Cyc_PP_line_doc(),((_tmp96A[3]=
# 1562
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3FF,0)),((_tmp96A[2]=
# 1561
Cyc_PP_line_doc(),((_tmp96A[1]=
# 1560
Cyc_Absynpp_lb(),((_tmp96A[0]=
# 1559
Cyc_PP_text(((_tmp96C="try ",_tag_dyneither(_tmp96C,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp96A,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
# 1571
goto _LL237;_LL25A: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp401=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp401->tag != 16)goto _LL237;else{_tmp402=_tmp401->f1;}}_LL25B:
# 1573
{const char*_tmp971;const char*_tmp970;struct Cyc_PP_Doc*_tmp96F[3];s=((_tmp96F[2]=Cyc_PP_text(((_tmp970=");",_tag_dyneither(_tmp970,sizeof(char),3)))),((_tmp96F[1]=Cyc_Absynpp_exp2doc(_tmp402),((_tmp96F[0]=Cyc_PP_text(((_tmp971="reset_region(",_tag_dyneither(_tmp971,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp96F,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL237;_LL237:;}
# 1576
return s;}
# 1579
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp448=p->r;void*_tmp449=_tmp448;enum Cyc_Absyn_Sign _tmp44D;int _tmp44E;char _tmp450;struct _dyneither_ptr _tmp452;struct Cyc_Absyn_Vardecl*_tmp454;struct Cyc_Absyn_Vardecl*_tmp457;struct Cyc_Absyn_Pat*_tmp458;struct Cyc_Absyn_Tvar*_tmp45A;struct Cyc_Absyn_Vardecl*_tmp45B;struct Cyc_Absyn_Tvar*_tmp45D;struct Cyc_Absyn_Vardecl*_tmp45E;struct Cyc_List_List*_tmp460;int _tmp461;struct Cyc_Absyn_Pat*_tmp463;struct Cyc_Absyn_Vardecl*_tmp465;struct Cyc_Absyn_Vardecl*_tmp468;struct Cyc_Absyn_Pat*_tmp469;struct _tuple0*_tmp46B;struct _tuple0*_tmp46D;struct Cyc_List_List*_tmp46E;int _tmp46F;union Cyc_Absyn_AggrInfoU _tmp471;struct Cyc_List_List*_tmp472;struct Cyc_List_List*_tmp473;int _tmp474;struct Cyc_List_List*_tmp476;struct Cyc_List_List*_tmp477;int _tmp478;struct Cyc_Absyn_Enumfield*_tmp47A;struct Cyc_Absyn_Enumfield*_tmp47C;struct Cyc_Absyn_Datatypefield*_tmp47E;struct Cyc_Absyn_Datatypefield*_tmp480;struct Cyc_List_List*_tmp481;int _tmp482;struct Cyc_Absyn_Exp*_tmp484;_LL262: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp44A=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp44A->tag != 0)goto _LL264;}_LL263:
{const char*_tmp972;s=Cyc_PP_text(((_tmp972="_",_tag_dyneither(_tmp972,sizeof(char),2))));}goto _LL261;_LL264: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp44B=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp44B->tag != 9)goto _LL266;}_LL265:
{const char*_tmp973;s=Cyc_PP_text(((_tmp973="NULL",_tag_dyneither(_tmp973,sizeof(char),5))));}goto _LL261;_LL266: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp44C=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp44C->tag != 10)goto _LL268;else{_tmp44D=_tmp44C->f1;_tmp44E=_tmp44C->f2;}}_LL267:
# 1585
 if(_tmp44D != Cyc_Absyn_Unsigned){
const char*_tmp977;void*_tmp976[1];struct Cyc_Int_pa_PrintArg_struct _tmp975;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp975.tag=1,((_tmp975.f1=(unsigned long)_tmp44E,((_tmp976[0]=& _tmp975,Cyc_aprintf(((_tmp977="%d",_tag_dyneither(_tmp977,sizeof(char),3))),_tag_dyneither(_tmp976,sizeof(void*),1)))))))));}else{
const char*_tmp97B;void*_tmp97A[1];struct Cyc_Int_pa_PrintArg_struct _tmp979;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp979.tag=1,((_tmp979.f1=(unsigned int)_tmp44E,((_tmp97A[0]=& _tmp979,Cyc_aprintf(((_tmp97B="%u",_tag_dyneither(_tmp97B,sizeof(char),3))),_tag_dyneither(_tmp97A,sizeof(void*),1)))))))));}
goto _LL261;_LL268: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp44F=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp44F->tag != 11)goto _LL26A;else{_tmp450=_tmp44F->f1;}}_LL269:
{const char*_tmp97F;void*_tmp97E[1];struct Cyc_String_pa_PrintArg_struct _tmp97D;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp97D.tag=0,((_tmp97D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp450)),((_tmp97E[0]=& _tmp97D,Cyc_aprintf(((_tmp97F="'%s'",_tag_dyneither(_tmp97F,sizeof(char),5))),_tag_dyneither(_tmp97E,sizeof(void*),1)))))))));}goto _LL261;_LL26A: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp451=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp451->tag != 12)goto _LL26C;else{_tmp452=_tmp451->f1;}}_LL26B:
 s=Cyc_PP_text(_tmp452);goto _LL261;_LL26C: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp453=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp453->tag != 1)goto _LL26E;else{_tmp454=_tmp453->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp455=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp453->f2)->r;if(_tmp455->tag != 0)goto _LL26E;};}}_LL26D:
# 1592
 s=Cyc_Absynpp_qvar2doc(_tmp454->name);goto _LL261;_LL26E: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp456=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp456->tag != 1)goto _LL270;else{_tmp457=_tmp456->f1;_tmp458=_tmp456->f2;}}_LL26F:
# 1594
{const char*_tmp982;struct Cyc_PP_Doc*_tmp981[3];s=((_tmp981[2]=Cyc_Absynpp_pat2doc(_tmp458),((_tmp981[1]=Cyc_PP_text(((_tmp982=" as ",_tag_dyneither(_tmp982,sizeof(char),5)))),((_tmp981[0]=Cyc_Absynpp_qvar2doc(_tmp457->name),Cyc_PP_cat(_tag_dyneither(_tmp981,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL261;_LL270: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp459=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp459->tag != 2)goto _LL272;else{_tmp45A=_tmp459->f1;_tmp45B=_tmp459->f2;}}_LL271:
# 1596
{const char*_tmp989;const char*_tmp988;const char*_tmp987;struct Cyc_PP_Doc*_tmp986[5];s=((_tmp986[4]=
Cyc_Absynpp_vardecl2doc(_tmp45B),((_tmp986[3]=
# 1596
Cyc_PP_text(((_tmp987="> ",_tag_dyneither(_tmp987,sizeof(char),3)))),((_tmp986[2]=Cyc_Absynpp_tvar2doc(_tmp45A),((_tmp986[1]=Cyc_PP_text(((_tmp988=" <",_tag_dyneither(_tmp988,sizeof(char),3)))),((_tmp986[0]=Cyc_PP_text(((_tmp989="alias",_tag_dyneither(_tmp989,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp986,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1598
goto _LL261;_LL272: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp45C=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp45C->tag != 4)goto _LL274;else{_tmp45D=_tmp45C->f1;_tmp45E=_tmp45C->f2;}}_LL273:
# 1600
{const char*_tmp98E;const char*_tmp98D;struct Cyc_PP_Doc*_tmp98C[4];s=((_tmp98C[3]=Cyc_PP_text(((_tmp98D=">",_tag_dyneither(_tmp98D,sizeof(char),2)))),((_tmp98C[2]=Cyc_Absynpp_tvar2doc(_tmp45D),((_tmp98C[1]=Cyc_PP_text(((_tmp98E="<",_tag_dyneither(_tmp98E,sizeof(char),2)))),((_tmp98C[0]=Cyc_Absynpp_qvar2doc(_tmp45E->name),Cyc_PP_cat(_tag_dyneither(_tmp98C,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL261;_LL274: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp45F=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp45F->tag != 5)goto _LL276;else{_tmp460=_tmp45F->f1;_tmp461=_tmp45F->f2;}}_LL275:
# 1603
{const char*_tmp997;const char*_tmp996;const char*_tmp995;const char*_tmp994;struct Cyc_PP_Doc*_tmp993[4];s=((_tmp993[3]=
_tmp461?Cyc_PP_text(((_tmp994=", ...)",_tag_dyneither(_tmp994,sizeof(char),7)))): Cyc_PP_text(((_tmp995=")",_tag_dyneither(_tmp995,sizeof(char),2)))),((_tmp993[2]=
# 1603
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp996=",",_tag_dyneither(_tmp996,sizeof(char),2))),_tmp460),((_tmp993[1]=Cyc_PP_text(((_tmp997="(",_tag_dyneither(_tmp997,sizeof(char),2)))),((_tmp993[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp993,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1605
goto _LL261;_LL276: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp462=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp462->tag != 6)goto _LL278;else{_tmp463=_tmp462->f1;}}_LL277:
# 1607
{const char*_tmp99A;struct Cyc_PP_Doc*_tmp999[2];s=((_tmp999[1]=Cyc_Absynpp_pat2doc(_tmp463),((_tmp999[0]=Cyc_PP_text(((_tmp99A="&",_tag_dyneither(_tmp99A,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp999,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL261;_LL278: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp464=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp464->tag != 3)goto _LL27A;else{_tmp465=_tmp464->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp466=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp464->f2)->r;if(_tmp466->tag != 0)goto _LL27A;};}}_LL279:
# 1610
{const char*_tmp99D;struct Cyc_PP_Doc*_tmp99C[2];s=((_tmp99C[1]=Cyc_Absynpp_qvar2doc(_tmp465->name),((_tmp99C[0]=Cyc_PP_text(((_tmp99D="*",_tag_dyneither(_tmp99D,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp99C,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL261;_LL27A: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp467=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp467->tag != 3)goto _LL27C;else{_tmp468=_tmp467->f1;_tmp469=_tmp467->f2;}}_LL27B:
# 1613
{const char*_tmp9A2;const char*_tmp9A1;struct Cyc_PP_Doc*_tmp9A0[4];s=((_tmp9A0[3]=Cyc_Absynpp_pat2doc(_tmp469),((_tmp9A0[2]=Cyc_PP_text(((_tmp9A1=" as ",_tag_dyneither(_tmp9A1,sizeof(char),5)))),((_tmp9A0[1]=Cyc_Absynpp_qvar2doc(_tmp468->name),((_tmp9A0[0]=Cyc_PP_text(((_tmp9A2="*",_tag_dyneither(_tmp9A2,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp9A0,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL261;_LL27C: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp46A=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp46A->tag != 15)goto _LL27E;else{_tmp46B=_tmp46A->f1;}}_LL27D:
# 1616
 s=Cyc_Absynpp_qvar2doc(_tmp46B);
goto _LL261;_LL27E: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp46C=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp46C->tag != 16)goto _LL280;else{_tmp46D=_tmp46C->f1;_tmp46E=_tmp46C->f2;_tmp46F=_tmp46C->f3;}}_LL27F: {
# 1619
const char*_tmp9A4;const char*_tmp9A3;struct _dyneither_ptr term=_tmp46F?(_tmp9A4=", ...)",_tag_dyneither(_tmp9A4,sizeof(char),7)):((_tmp9A3=")",_tag_dyneither(_tmp9A3,sizeof(char),2)));
{const char*_tmp9A9;const char*_tmp9A8;struct Cyc_PP_Doc*_tmp9A7[2];s=((_tmp9A7[1]=Cyc_PP_group(((_tmp9A9="(",_tag_dyneither(_tmp9A9,sizeof(char),2))),term,((_tmp9A8=",",_tag_dyneither(_tmp9A8,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp46E)),((_tmp9A7[0]=Cyc_Absynpp_qvar2doc(_tmp46D),Cyc_PP_cat(_tag_dyneither(_tmp9A7,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL261;}_LL280: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp470=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp470->tag != 7)goto _LL282;else{if(_tmp470->f1 == 0)goto _LL282;_tmp471=(_tmp470->f1)->aggr_info;_tmp472=_tmp470->f2;_tmp473=_tmp470->f3;_tmp474=_tmp470->f4;}}_LL281: {
# 1623
const char*_tmp9AB;const char*_tmp9AA;struct _dyneither_ptr term=_tmp474?(_tmp9AB=", ...}",_tag_dyneither(_tmp9AB,sizeof(char),7)):((_tmp9AA="}",_tag_dyneither(_tmp9AA,sizeof(char),2)));
struct _tuple10 _tmp4AA=Cyc_Absyn_aggr_kinded_name(_tmp471);struct _tuple0*_tmp4AC;struct _tuple10 _tmp4AB=_tmp4AA;_tmp4AC=_tmp4AB.f2;
{const char*_tmp9B6;const char*_tmp9B5;const char*_tmp9B4;const char*_tmp9B3;const char*_tmp9B2;struct Cyc_PP_Doc*_tmp9B1[4];s=((_tmp9B1[3]=
# 1627
Cyc_PP_group(((_tmp9B3="",_tag_dyneither(_tmp9B3,sizeof(char),1))),term,((_tmp9B2=",",_tag_dyneither(_tmp9B2,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp473)),((_tmp9B1[2]=
# 1626
Cyc_PP_egroup(((_tmp9B6="[",_tag_dyneither(_tmp9B6,sizeof(char),2))),((_tmp9B5="]",_tag_dyneither(_tmp9B5,sizeof(char),2))),((_tmp9B4=",",_tag_dyneither(_tmp9B4,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp472)),((_tmp9B1[1]=
# 1625
Cyc_Absynpp_lb(),((_tmp9B1[0]=Cyc_Absynpp_qvar2doc(_tmp4AC),Cyc_PP_cat(_tag_dyneither(_tmp9B1,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1628
goto _LL261;}_LL282: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp475=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp475->tag != 7)goto _LL284;else{if(_tmp475->f1 != 0)goto _LL284;_tmp476=_tmp475->f2;_tmp477=_tmp475->f3;_tmp478=_tmp475->f4;}}_LL283: {
# 1630
const char*_tmp9B8;const char*_tmp9B7;struct _dyneither_ptr term=_tmp478?(_tmp9B8=", ...}",_tag_dyneither(_tmp9B8,sizeof(char),7)):((_tmp9B7="}",_tag_dyneither(_tmp9B7,sizeof(char),2)));
{const char*_tmp9C3;const char*_tmp9C2;const char*_tmp9C1;const char*_tmp9C0;const char*_tmp9BF;struct Cyc_PP_Doc*_tmp9BE[3];s=((_tmp9BE[2]=
# 1633
Cyc_PP_group(((_tmp9C0="",_tag_dyneither(_tmp9C0,sizeof(char),1))),term,((_tmp9BF=",",_tag_dyneither(_tmp9BF,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp477)),((_tmp9BE[1]=
# 1632
Cyc_PP_egroup(((_tmp9C3="[",_tag_dyneither(_tmp9C3,sizeof(char),2))),((_tmp9C2="]",_tag_dyneither(_tmp9C2,sizeof(char),2))),((_tmp9C1=",",_tag_dyneither(_tmp9C1,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp476)),((_tmp9BE[0]=
# 1631
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp9BE,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1634
goto _LL261;}_LL284: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp479=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp479->tag != 13)goto _LL286;else{_tmp47A=_tmp479->f2;}}_LL285:
 _tmp47C=_tmp47A;goto _LL287;_LL286: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp47B=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp47B->tag != 14)goto _LL288;else{_tmp47C=_tmp47B->f2;}}_LL287:
 s=Cyc_Absynpp_qvar2doc(_tmp47C->name);goto _LL261;_LL288: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp47D=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp47D->tag != 8)goto _LL28A;else{_tmp47E=_tmp47D->f2;if(_tmp47D->f3 != 0)goto _LL28A;}}_LL289:
 s=Cyc_Absynpp_qvar2doc(_tmp47E->name);goto _LL261;_LL28A: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp47F=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp47F->tag != 8)goto _LL28C;else{_tmp480=_tmp47F->f2;_tmp481=_tmp47F->f3;_tmp482=_tmp47F->f4;}}_LL28B: {
# 1639
const char*_tmp9C5;const char*_tmp9C4;struct _dyneither_ptr term=_tmp482?(_tmp9C5=", ...)",_tag_dyneither(_tmp9C5,sizeof(char),7)):((_tmp9C4=")",_tag_dyneither(_tmp9C4,sizeof(char),2)));
{const char*_tmp9CA;const char*_tmp9C9;struct Cyc_PP_Doc*_tmp9C8[2];s=((_tmp9C8[1]=Cyc_PP_egroup(((_tmp9CA="(",_tag_dyneither(_tmp9CA,sizeof(char),2))),term,((_tmp9C9=",",_tag_dyneither(_tmp9C9,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp481)),((_tmp9C8[0]=Cyc_Absynpp_qvar2doc(_tmp480->name),Cyc_PP_cat(_tag_dyneither(_tmp9C8,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL261;}_LL28C: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp483=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp449;if(_tmp483->tag != 17)goto _LL261;else{_tmp484=_tmp483->f1;}}_LL28D:
# 1643
 s=Cyc_Absynpp_exp2doc(_tmp484);goto _LL261;_LL261:;}
# 1645
return s;}
# 1648
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
const char*_tmp9D1;const char*_tmp9D0;const char*_tmp9CF;struct Cyc_PP_Doc*_tmp9CE[2];return(_tmp9CE[1]=
Cyc_Absynpp_pat2doc((*dp).f2),((_tmp9CE[0]=
# 1649
Cyc_PP_egroup(((_tmp9D1="",_tag_dyneither(_tmp9D1,sizeof(char),1))),((_tmp9D0="=",_tag_dyneither(_tmp9D0,sizeof(char),2))),((_tmp9CF="=",_tag_dyneither(_tmp9CF,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp9CE,sizeof(struct Cyc_PP_Doc*),2)))));}
# 1653
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){
const char*_tmp9D6;struct Cyc_PP_Doc*_tmp9D5[2];struct Cyc_PP_Doc*_tmp9D4[2];return(_tmp9D4[1]=
Cyc_PP_nest(2,((_tmp9D5[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp9D5[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9D5,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9D4[0]=
# 1655
Cyc_PP_text(((_tmp9D6="default: ",_tag_dyneither(_tmp9D6,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp9D4,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 1657
if(c->where_clause == 0){
const char*_tmp9DD;const char*_tmp9DC;struct Cyc_PP_Doc*_tmp9DB[2];struct Cyc_PP_Doc*_tmp9DA[4];return(_tmp9DA[3]=
# 1661
Cyc_PP_nest(2,((_tmp9DB[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp9DB[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9DB,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9DA[2]=
# 1660
Cyc_PP_text(((_tmp9DC=": ",_tag_dyneither(_tmp9DC,sizeof(char),3)))),((_tmp9DA[1]=
# 1659
Cyc_Absynpp_pat2doc(c->pattern),((_tmp9DA[0]=
# 1658
Cyc_PP_text(((_tmp9DD="case ",_tag_dyneither(_tmp9DD,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9DA,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1663
const char*_tmp9E6;const char*_tmp9E5;const char*_tmp9E4;struct Cyc_PP_Doc*_tmp9E3[2];struct Cyc_PP_Doc*_tmp9E2[6];return(_tmp9E2[5]=
# 1668
Cyc_PP_nest(2,((_tmp9E3[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp9E3[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9E3,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9E2[4]=
# 1667
Cyc_PP_text(((_tmp9E4=": ",_tag_dyneither(_tmp9E4,sizeof(char),3)))),((_tmp9E2[3]=
# 1666
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp9E2[2]=
# 1665
Cyc_PP_text(((_tmp9E5=" && ",_tag_dyneither(_tmp9E5,sizeof(char),5)))),((_tmp9E2[1]=
# 1664
Cyc_Absynpp_pat2doc(c->pattern),((_tmp9E2[0]=
# 1663
Cyc_PP_text(((_tmp9E6="case ",_tag_dyneither(_tmp9E6,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9E2,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
# 1671
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
const char*_tmp9E7;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((_tmp9E7="",_tag_dyneither(_tmp9E7,sizeof(char),1))),cs);}
# 1675
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1679
const char*_tmp9EA;struct Cyc_PP_Doc*_tmp9E9[3];return(_tmp9E9[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp9E9[1]=Cyc_PP_text(((_tmp9EA=" = ",_tag_dyneither(_tmp9EA,sizeof(char),4)))),((_tmp9E9[0]=Cyc_Absynpp_qvar2doc(f->name),Cyc_PP_cat(_tag_dyneither(_tmp9E9,sizeof(struct Cyc_PP_Doc*),3)))))));}}
# 1682
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
const char*_tmp9EB;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp9EB=",",_tag_dyneither(_tmp9EB,sizeof(char),2))),fs);}
# 1686
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1690
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
const char*_tmp9EC;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,((_tmp9EC=",",_tag_dyneither(_tmp9EC,sizeof(char),2))),vds);}
# 1694
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
enum Cyc_Absyn_Scope _tmp4D8;struct _tuple0*_tmp4D9;struct Cyc_Absyn_Tqual _tmp4DA;void*_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DC;struct Cyc_List_List*_tmp4DD;struct Cyc_Absyn_Vardecl*_tmp4D7=vd;_tmp4D8=_tmp4D7->sc;_tmp4D9=_tmp4D7->name;_tmp4DA=_tmp4D7->tq;_tmp4DB=_tmp4D7->type;_tmp4DC=_tmp4D7->initializer;_tmp4DD=_tmp4D7->attributes;{
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp4D9);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4DD);
struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL28E:
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL28F:
# 1703
{void*_tmp4DE=Cyc_Tcutil_compress(_tmp4DB);void*_tmp4DF=_tmp4DE;struct Cyc_List_List*_tmp4E1;_LL292: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4E0=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4DF;if(_tmp4E0->tag != 9)goto _LL294;else{_tmp4E1=(_tmp4E0->f1).attributes;}}_LL293:
# 1705
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4E1);
goto _LL291;_LL294:;_LL295:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL291;_LL291:;}
# 1709
break;}{
# 1712
struct Cyc_PP_Doc*tmp_doc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL296:
 tmp_doc=Cyc_PP_nil_doc();break;case Cyc_Cyclone_Vc_c: _LL297:
 tmp_doc=attsdoc;break;}
# 1717
{struct Cyc_Core_Opt*_tmp9F9;struct Cyc_PP_Doc*_tmp9F8[2];struct Cyc_PP_Doc*_tmp9F7[2];const char*_tmp9F6;const char*_tmp9F5;struct Cyc_PP_Doc*_tmp9F4[5];s=((_tmp9F4[4]=
# 1724
Cyc_PP_text(((_tmp9F5=";",_tag_dyneither(_tmp9F5,sizeof(char),2)))),((_tmp9F4[3]=
# 1721
_tmp4DC == 0?
Cyc_PP_nil_doc():(
(_tmp9F7[1]=Cyc_Absynpp_exp2doc(_tmp4DC),((_tmp9F7[0]=Cyc_PP_text(((_tmp9F6=" = ",_tag_dyneither(_tmp9F6,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp9F7,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9F4[2]=
# 1720
Cyc_Absynpp_tqtd2doc(_tmp4DA,_tmp4DB,((_tmp9F9=_cycalloc(sizeof(*_tmp9F9)),((_tmp9F9->v=((_tmp9F8[1]=sn,((_tmp9F8[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp9F8,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9F9))))),((_tmp9F4[1]=
# 1719
Cyc_Absynpp_scope2doc(_tmp4D8),((_tmp9F4[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp9F4,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1725
return s;};};}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1728
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple17 _tmp4E8=*x;struct _tuple0*_tmp4EA;struct _tuple17 _tmp4E9=_tmp4E8;_tmp4EA=_tmp4E9.f2;
return Cyc_Absynpp_qvar2doc(_tmp4EA);}
# 1733
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0){
struct Cyc_PP_Doc*_tmp9FA[4];return(_tmp9FA[3]=
# 1738
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp9FA[2]=
# 1737
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp9FA[1]=
# 1736
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp9FA[0]=
# 1735
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp9FA,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1740
struct Cyc_PP_Doc*_tmpA02[2];const char*_tmpA01;struct Cyc_PP_Doc*_tmpA00[2];struct Cyc_PP_Doc*_tmp9FF[12];return(_tmp9FF[11]=
# 1751
Cyc_Absynpp_atts2doc(ad->attributes),((_tmp9FF[10]=
# 1750
Cyc_Absynpp_rb(),((_tmp9FF[9]=
# 1749
Cyc_PP_line_doc(),((_tmp9FF[8]=
# 1748
Cyc_PP_nest(2,((_tmpA00[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields),((_tmpA00[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA00,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9FF[7]=
# 1746
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():(
(_tmpA02[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po),((_tmpA02[0]=Cyc_PP_text(((_tmpA01=":",_tag_dyneither(_tmpA01,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmpA02,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9FF[6]=
# 1745
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars),((_tmp9FF[5]=
# 1744
Cyc_Absynpp_lb(),((_tmp9FF[4]=Cyc_PP_blank_doc(),((_tmp9FF[3]=
# 1743
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp9FF[2]=
# 1742
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp9FF[1]=
# 1741
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp9FF[0]=
# 1740
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp9FF,sizeof(struct Cyc_PP_Doc*),12)))))))))))))))))))))))));}}
# 1754
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
enum Cyc_Absyn_Scope _tmp4F1;struct _tuple0*_tmp4F2;struct Cyc_List_List*_tmp4F3;struct Cyc_Core_Opt*_tmp4F4;int _tmp4F5;struct Cyc_Absyn_Datatypedecl*_tmp4F0=dd;_tmp4F1=_tmp4F0->sc;_tmp4F2=_tmp4F0->name;_tmp4F3=_tmp4F0->tvs;_tmp4F4=_tmp4F0->fields;_tmp4F5=_tmp4F0->is_extensible;
if(_tmp4F4 == 0){
const char*_tmpA07;const char*_tmpA06;struct Cyc_PP_Doc*_tmpA05[5];return(_tmpA05[4]=
# 1761
Cyc_Absynpp_ktvars2doc(_tmp4F3),((_tmpA05[3]=
# 1760
_tmp4F5?Cyc_Absynpp_qvar2bolddoc(_tmp4F2): Cyc_Absynpp_typedef_name2bolddoc(_tmp4F2),((_tmpA05[2]=
# 1759
Cyc_PP_text(((_tmpA06="datatype ",_tag_dyneither(_tmpA06,sizeof(char),10)))),((_tmpA05[1]=
# 1758
_tmp4F5?Cyc_PP_text(((_tmpA07="@extensible ",_tag_dyneither(_tmpA07,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmpA05[0]=
# 1757
Cyc_Absynpp_scope2doc(_tmp4F1),Cyc_PP_cat(_tag_dyneither(_tmpA05,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 1763
const char*_tmpA0E;const char*_tmpA0D;struct Cyc_PP_Doc*_tmpA0C[2];struct Cyc_PP_Doc*_tmpA0B[10];return(_tmpA0B[9]=
# 1771
Cyc_Absynpp_rb(),((_tmpA0B[8]=
# 1770
Cyc_PP_line_doc(),((_tmpA0B[7]=
# 1769
Cyc_PP_nest(2,((_tmpA0C[1]=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp4F4->v),((_tmpA0C[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA0C,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmpA0B[6]=
# 1768
Cyc_Absynpp_lb(),((_tmpA0B[5]=Cyc_PP_blank_doc(),((_tmpA0B[4]=
# 1767
Cyc_Absynpp_ktvars2doc(_tmp4F3),((_tmpA0B[3]=
# 1766
_tmp4F5?Cyc_Absynpp_qvar2bolddoc(_tmp4F2): Cyc_Absynpp_typedef_name2bolddoc(_tmp4F2),((_tmpA0B[2]=
# 1765
Cyc_PP_text(((_tmpA0D="datatype ",_tag_dyneither(_tmpA0D,sizeof(char),10)))),((_tmpA0B[1]=
# 1764
_tmp4F5?Cyc_PP_text(((_tmpA0E="@extensible ",_tag_dyneither(_tmpA0E,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmpA0B[0]=
# 1763
Cyc_Absynpp_scope2doc(_tmp4F1),Cyc_PP_cat(_tag_dyneither(_tmpA0B,sizeof(struct Cyc_PP_Doc*),10)))))))))))))))))))));}}
# 1774
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
enum Cyc_Absyn_Scope _tmp4FE;struct _tuple0*_tmp4FF;struct Cyc_Core_Opt*_tmp500;struct Cyc_Absyn_Enumdecl*_tmp4FD=ed;_tmp4FE=_tmp4FD->sc;_tmp4FF=_tmp4FD->name;_tmp500=_tmp4FD->fields;
if(_tmp500 == 0){
const char*_tmpA11;struct Cyc_PP_Doc*_tmpA10[3];return(_tmpA10[2]=
# 1779
Cyc_Absynpp_typedef_name2bolddoc(_tmp4FF),((_tmpA10[1]=
# 1778
Cyc_PP_text(((_tmpA11="enum ",_tag_dyneither(_tmpA11,sizeof(char),6)))),((_tmpA10[0]=
# 1777
Cyc_Absynpp_scope2doc(_tmp4FE),Cyc_PP_cat(_tag_dyneither(_tmpA10,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
# 1782
const char*_tmpA16;struct Cyc_PP_Doc*_tmpA15[2];struct Cyc_PP_Doc*_tmpA14[8];return(_tmpA14[7]=
# 1788
Cyc_Absynpp_rb(),((_tmpA14[6]=
# 1787
Cyc_PP_line_doc(),((_tmpA14[5]=
# 1786
Cyc_PP_nest(2,((_tmpA15[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp500->v),((_tmpA15[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA15,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmpA14[4]=
# 1785
Cyc_Absynpp_lb(),((_tmpA14[3]=Cyc_PP_blank_doc(),((_tmpA14[2]=
# 1784
Cyc_Absynpp_qvar2bolddoc(_tmp4FF),((_tmpA14[1]=
# 1783
Cyc_PP_text(((_tmpA16="enum ",_tag_dyneither(_tmpA16,sizeof(char),6)))),((_tmpA14[0]=
# 1782
Cyc_Absynpp_scope2doc(_tmp4FE),Cyc_PP_cat(_tag_dyneither(_tmpA14,sizeof(struct Cyc_PP_Doc*),8)))))))))))))))));}}
# 1791
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp506=d->r;void*_tmp507=_tmp506;struct Cyc_Absyn_Fndecl*_tmp509;struct Cyc_Absyn_Aggrdecl*_tmp50B;struct Cyc_Absyn_Vardecl*_tmp50D;struct Cyc_Absyn_Tvar*_tmp50F;struct Cyc_Absyn_Vardecl*_tmp510;int _tmp511;struct Cyc_Absyn_Exp*_tmp512;struct Cyc_Absyn_Datatypedecl*_tmp514;struct Cyc_Absyn_Pat*_tmp516;struct Cyc_Absyn_Exp*_tmp517;struct Cyc_List_List*_tmp519;struct Cyc_Absyn_Enumdecl*_tmp51B;struct Cyc_Absyn_Typedefdecl*_tmp51D;struct _dyneither_ptr*_tmp51F;struct Cyc_List_List*_tmp520;struct _tuple0*_tmp522;struct Cyc_List_List*_tmp523;struct Cyc_List_List*_tmp525;struct Cyc_List_List*_tmp527;struct Cyc_List_List*_tmp528;_LL29A: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp508=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp507;if(_tmp508->tag != 1)goto _LL29C;else{_tmp509=_tmp508->f1;}}_LL29B: {
# 1795
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA1C;struct Cyc_Absyn_FnInfo _tmpA1B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA1A;void*t=(void*)((_tmpA1A=_cycalloc(sizeof(*_tmpA1A)),((_tmpA1A[0]=((_tmpA1C.tag=9,((_tmpA1C.f1=((_tmpA1B.tvars=_tmp509->tvs,((_tmpA1B.effect=_tmp509->effect,((_tmpA1B.ret_tqual=_tmp509->ret_tqual,((_tmpA1B.ret_typ=_tmp509->ret_type,((_tmpA1B.args=
# 1799
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp509->args),((_tmpA1B.c_varargs=_tmp509->c_varargs,((_tmpA1B.cyc_varargs=_tmp509->cyc_varargs,((_tmpA1B.rgn_po=_tmp509->rgn_po,((_tmpA1B.attributes=0,((_tmpA1B.requires_clause=_tmp509->requires_clause,((_tmpA1B.requires_relns=_tmp509->requires_relns,((_tmpA1B.ensures_clause=_tmp509->ensures_clause,((_tmpA1B.ensures_relns=_tmp509->ensures_relns,_tmpA1B)))))))))))))))))))))))))),_tmpA1C)))),_tmpA1A))));
# 1805
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp509->attributes);
struct Cyc_PP_Doc*inlinedoc;
if(_tmp509->is_inline)
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2B8:
{const char*_tmpA1D;inlinedoc=Cyc_PP_text(((_tmpA1D="inline ",_tag_dyneither(_tmpA1D,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL2B9:
{const char*_tmpA1E;inlinedoc=Cyc_PP_text(((_tmpA1E="__inline ",_tag_dyneither(_tmpA1E,sizeof(char),10))));}break;}else{
# 1813
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp509->sc);
struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2BB:
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL2BC:
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp509->attributes);break;}{
# 1820
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp509->name);
struct Cyc_PP_Doc*_tmpA21[2];struct Cyc_Core_Opt*_tmpA20;struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,(
(_tmpA20=_cycalloc(sizeof(*_tmpA20)),((_tmpA20->v=((_tmpA21[1]=namedoc,((_tmpA21[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmpA21,sizeof(struct Cyc_PP_Doc*),2)))))),_tmpA20)))));
struct Cyc_PP_Doc*_tmpA24[2];struct Cyc_PP_Doc*_tmpA23[5];struct Cyc_PP_Doc*bodydoc=(_tmpA23[4]=
# 1826
Cyc_Absynpp_rb(),((_tmpA23[3]=
# 1825
Cyc_PP_line_doc(),((_tmpA23[2]=
# 1824
Cyc_PP_nest(2,((_tmpA24[1]=Cyc_Absynpp_stmt2doc(_tmp509->body,0),((_tmpA24[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA24,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmpA23[1]=
# 1823
Cyc_Absynpp_lb(),((_tmpA23[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA23,sizeof(struct Cyc_PP_Doc*),5)))))))))));
# 1827
{struct Cyc_PP_Doc*_tmpA25[4];s=((_tmpA25[3]=bodydoc,((_tmpA25[2]=tqtddoc,((_tmpA25[1]=scopedoc,((_tmpA25[0]=inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmpA25,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1829
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL2BE:
{struct Cyc_PP_Doc*_tmpA26[2];s=((_tmpA26[1]=s,((_tmpA26[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(_tmpA26,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL2BF:
 break;}
# 1834
goto _LL299;};};}_LL29C: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp50A=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp507;if(_tmp50A->tag != 5)goto _LL29E;else{_tmp50B=_tmp50A->f1;}}_LL29D:
# 1837
{const char*_tmpA29;struct Cyc_PP_Doc*_tmpA28[2];s=((_tmpA28[1]=Cyc_PP_text(((_tmpA29=";",_tag_dyneither(_tmpA29,sizeof(char),2)))),((_tmpA28[0]=Cyc_Absynpp_aggrdecl2doc(_tmp50B),Cyc_PP_cat(_tag_dyneither(_tmpA28,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL299;_LL29E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp50C=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp507;if(_tmp50C->tag != 0)goto _LL2A0;else{_tmp50D=_tmp50C->f1;}}_LL29F:
# 1840
 s=Cyc_Absynpp_vardecl2doc(_tmp50D);
goto _LL299;_LL2A0: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp50E=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp507;if(_tmp50E->tag != 4)goto _LL2A2;else{_tmp50F=_tmp50E->f1;_tmp510=_tmp50E->f2;_tmp511=_tmp50E->f3;_tmp512=_tmp50E->f4;}}_LL2A1:
# 1843
{const char*_tmpA3C;const char*_tmpA3B;const char*_tmpA3A;struct Cyc_PP_Doc*_tmpA39[3];const char*_tmpA38;const char*_tmpA37;const char*_tmpA36;const char*_tmpA35;struct Cyc_PP_Doc*_tmpA34[8];s=((_tmpA34[7]=
# 1851
Cyc_PP_text(((_tmpA35=";",_tag_dyneither(_tmpA35,sizeof(char),2)))),((_tmpA34[6]=
# 1850
_tmp511?Cyc_PP_text(((_tmpA36=" @resetable",_tag_dyneither(_tmpA36,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmpA34[5]=
# 1848
(unsigned int)_tmp512?(_tmpA39[2]=
Cyc_PP_text(((_tmpA38=")",_tag_dyneither(_tmpA38,sizeof(char),2)))),((_tmpA39[1]=
# 1848
Cyc_Absynpp_exp2doc(_tmp512),((_tmpA39[0]=Cyc_PP_text(((_tmpA37=" = open(",_tag_dyneither(_tmpA37,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmpA39,sizeof(struct Cyc_PP_Doc*),3))))))):
 Cyc_PP_nil_doc(),((_tmpA34[4]=
# 1847
Cyc_Absynpp_qvar2doc(_tmp510->name),((_tmpA34[3]=
# 1846
Cyc_PP_text(((_tmpA3A=">",_tag_dyneither(_tmpA3A,sizeof(char),2)))),((_tmpA34[2]=
# 1845
Cyc_Absynpp_tvar2doc(_tmp50F),((_tmpA34[1]=
# 1844
Cyc_PP_text(((_tmpA3B="<",_tag_dyneither(_tmpA3B,sizeof(char),2)))),((_tmpA34[0]=
# 1843
Cyc_PP_text(((_tmpA3C="region",_tag_dyneither(_tmpA3C,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA34,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1852
goto _LL299;_LL2A2: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp513=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp507;if(_tmp513->tag != 6)goto _LL2A4;else{_tmp514=_tmp513->f1;}}_LL2A3:
# 1854
{const char*_tmpA3F;struct Cyc_PP_Doc*_tmpA3E[2];s=((_tmpA3E[1]=Cyc_PP_text(((_tmpA3F=";",_tag_dyneither(_tmpA3F,sizeof(char),2)))),((_tmpA3E[0]=Cyc_Absynpp_datatypedecl2doc(_tmp514),Cyc_PP_cat(_tag_dyneither(_tmpA3E,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL299;_LL2A4: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp515=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp507;if(_tmp515->tag != 2)goto _LL2A6;else{_tmp516=_tmp515->f1;_tmp517=_tmp515->f3;}}_LL2A5:
# 1857
{const char*_tmpA46;const char*_tmpA45;const char*_tmpA44;struct Cyc_PP_Doc*_tmpA43[5];s=((_tmpA43[4]=
# 1861
Cyc_PP_text(((_tmpA44=";",_tag_dyneither(_tmpA44,sizeof(char),2)))),((_tmpA43[3]=
# 1860
Cyc_Absynpp_exp2doc(_tmp517),((_tmpA43[2]=
# 1859
Cyc_PP_text(((_tmpA45=" = ",_tag_dyneither(_tmpA45,sizeof(char),4)))),((_tmpA43[1]=
# 1858
Cyc_Absynpp_pat2doc(_tmp516),((_tmpA43[0]=
# 1857
Cyc_PP_text(((_tmpA46="let ",_tag_dyneither(_tmpA46,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA43,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1862
goto _LL299;_LL2A6: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp518=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp507;if(_tmp518->tag != 3)goto _LL2A8;else{_tmp519=_tmp518->f1;}}_LL2A7:
# 1864
{const char*_tmpA4B;const char*_tmpA4A;struct Cyc_PP_Doc*_tmpA49[3];s=((_tmpA49[2]=Cyc_PP_text(((_tmpA4A=";",_tag_dyneither(_tmpA4A,sizeof(char),2)))),((_tmpA49[1]=Cyc_Absynpp_ids2doc(_tmp519),((_tmpA49[0]=Cyc_PP_text(((_tmpA4B="let ",_tag_dyneither(_tmpA4B,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA49,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL299;_LL2A8: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp51A=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp507;if(_tmp51A->tag != 7)goto _LL2AA;else{_tmp51B=_tmp51A->f1;}}_LL2A9:
# 1867
{const char*_tmpA4E;struct Cyc_PP_Doc*_tmpA4D[2];s=((_tmpA4D[1]=Cyc_PP_text(((_tmpA4E=";",_tag_dyneither(_tmpA4E,sizeof(char),2)))),((_tmpA4D[0]=Cyc_Absynpp_enumdecl2doc(_tmp51B),Cyc_PP_cat(_tag_dyneither(_tmpA4D,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL299;_LL2AA: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp51C=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp507;if(_tmp51C->tag != 8)goto _LL2AC;else{_tmp51D=_tmp51C->f1;}}_LL2AB: {
# 1870
void*t;
if(_tmp51D->defn != 0)
t=(void*)_check_null(_tmp51D->defn);else{
# 1874
t=Cyc_Absyn_new_evar(_tmp51D->kind,0);}
{const char*_tmpA58;struct Cyc_Core_Opt*_tmpA57;struct Cyc_PP_Doc*_tmpA56[2];const char*_tmpA55;struct Cyc_PP_Doc*_tmpA54[4];s=((_tmpA54[3]=
# 1882
Cyc_PP_text(((_tmpA55=";",_tag_dyneither(_tmpA55,sizeof(char),2)))),((_tmpA54[2]=
# 1881
Cyc_Absynpp_atts2doc(_tmp51D->atts),((_tmpA54[1]=
# 1876
Cyc_Absynpp_tqtd2doc(_tmp51D->tq,t,(
# 1878
(_tmpA57=_cycalloc(sizeof(*_tmpA57)),((_tmpA57->v=((_tmpA56[1]=
Cyc_Absynpp_tvars2doc(_tmp51D->tvs),((_tmpA56[0]=
# 1878
Cyc_Absynpp_typedef_name2bolddoc(_tmp51D->name),Cyc_PP_cat(_tag_dyneither(_tmpA56,sizeof(struct Cyc_PP_Doc*),2)))))),_tmpA57))))),((_tmpA54[0]=
# 1875
Cyc_PP_text(((_tmpA58="typedef ",_tag_dyneither(_tmpA58,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmpA54,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1884
goto _LL299;}_LL2AC: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp51E=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp507;if(_tmp51E->tag != 9)goto _LL2AE;else{_tmp51F=_tmp51E->f1;_tmp520=_tmp51E->f2;}}_LL2AD:
# 1886
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp51F);
{const char*_tmpA5D;const char*_tmpA5C;struct Cyc_PP_Doc*_tmpA5B[8];s=((_tmpA5B[7]=
# 1893
Cyc_Absynpp_rb(),((_tmpA5B[6]=
# 1892
Cyc_PP_line_doc(),((_tmpA5B[5]=
# 1891
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA5C="",_tag_dyneither(_tmpA5C,sizeof(char),1))),_tmp520),((_tmpA5B[4]=
# 1890
Cyc_PP_line_doc(),((_tmpA5B[3]=
# 1889
Cyc_Absynpp_lb(),((_tmpA5B[2]=Cyc_PP_blank_doc(),((_tmpA5B[1]=
# 1888
Cyc_PP_textptr(_tmp51F),((_tmpA5B[0]=
# 1887
Cyc_PP_text(((_tmpA5D="namespace ",_tag_dyneither(_tmpA5D,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmpA5B,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1894
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL299;_LL2AE: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp521=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp507;if(_tmp521->tag != 10)goto _LL2B0;else{_tmp522=_tmp521->f1;_tmp523=_tmp521->f2;}}_LL2AF:
# 1897
 if(Cyc_Absynpp_print_using_stmts){
const char*_tmpA62;const char*_tmpA61;struct Cyc_PP_Doc*_tmpA60[8];s=((_tmpA60[7]=
# 1904
Cyc_Absynpp_rb(),((_tmpA60[6]=
# 1903
Cyc_PP_line_doc(),((_tmpA60[5]=
# 1902
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA61="",_tag_dyneither(_tmpA61,sizeof(char),1))),_tmp523),((_tmpA60[4]=
# 1901
Cyc_PP_line_doc(),((_tmpA60[3]=
# 1900
Cyc_Absynpp_lb(),((_tmpA60[2]=Cyc_PP_blank_doc(),((_tmpA60[1]=
# 1899
Cyc_Absynpp_qvar2doc(_tmp522),((_tmpA60[0]=
# 1898
Cyc_PP_text(((_tmpA62="using ",_tag_dyneither(_tmpA62,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA60,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1906
const char*_tmpA6D;const char*_tmpA6C;const char*_tmpA6B;const char*_tmpA6A;const char*_tmpA69;struct Cyc_PP_Doc*_tmpA68[11];s=((_tmpA68[10]=
# 1916
Cyc_PP_text(((_tmpA69=" */",_tag_dyneither(_tmpA69,sizeof(char),4)))),((_tmpA68[9]=
# 1915
Cyc_Absynpp_rb(),((_tmpA68[8]=
# 1914
Cyc_PP_text(((_tmpA6A="/* ",_tag_dyneither(_tmpA6A,sizeof(char),4)))),((_tmpA68[7]=
# 1913
Cyc_PP_line_doc(),((_tmpA68[6]=
# 1912
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA6B="",_tag_dyneither(_tmpA6B,sizeof(char),1))),_tmp523),((_tmpA68[5]=
# 1911
Cyc_PP_line_doc(),((_tmpA68[4]=
# 1910
Cyc_PP_text(((_tmpA6C=" */",_tag_dyneither(_tmpA6C,sizeof(char),4)))),((_tmpA68[3]=
# 1909
Cyc_Absynpp_lb(),((_tmpA68[2]=
# 1908
Cyc_PP_blank_doc(),((_tmpA68[1]=
# 1907
Cyc_Absynpp_qvar2doc(_tmp522),((_tmpA68[0]=
# 1906
Cyc_PP_text(((_tmpA6D="/* using ",_tag_dyneither(_tmpA6D,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmpA68,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1917
goto _LL299;_LL2B0: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp524=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp507;if(_tmp524->tag != 11)goto _LL2B2;else{_tmp525=_tmp524->f1;}}_LL2B1:
# 1919
 if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA72;const char*_tmpA71;struct Cyc_PP_Doc*_tmpA70[6];s=((_tmpA70[5]=
# 1925
Cyc_Absynpp_rb(),((_tmpA70[4]=
# 1924
Cyc_PP_line_doc(),((_tmpA70[3]=
# 1923
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA71="",_tag_dyneither(_tmpA71,sizeof(char),1))),_tmp525),((_tmpA70[2]=
# 1922
Cyc_PP_line_doc(),((_tmpA70[1]=
# 1921
Cyc_Absynpp_lb(),((_tmpA70[0]=
# 1920
Cyc_PP_text(((_tmpA72="extern \"C\" ",_tag_dyneither(_tmpA72,sizeof(char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA70,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}else{
# 1927
const char*_tmpA7D;const char*_tmpA7C;const char*_tmpA7B;const char*_tmpA7A;const char*_tmpA79;struct Cyc_PP_Doc*_tmpA78[9];s=((_tmpA78[8]=
# 1935
Cyc_PP_text(((_tmpA79=" */",_tag_dyneither(_tmpA79,sizeof(char),4)))),((_tmpA78[7]=
# 1934
Cyc_Absynpp_rb(),((_tmpA78[6]=
# 1933
Cyc_PP_text(((_tmpA7A="/* ",_tag_dyneither(_tmpA7A,sizeof(char),4)))),((_tmpA78[5]=
# 1932
Cyc_PP_line_doc(),((_tmpA78[4]=
# 1931
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA7B="",_tag_dyneither(_tmpA7B,sizeof(char),1))),_tmp525),((_tmpA78[3]=
# 1930
Cyc_PP_line_doc(),((_tmpA78[2]=
# 1929
Cyc_PP_text(((_tmpA7C=" */",_tag_dyneither(_tmpA7C,sizeof(char),4)))),((_tmpA78[1]=
# 1928
Cyc_Absynpp_lb(),((_tmpA78[0]=
# 1927
Cyc_PP_text(((_tmpA7D="/* extern \"C\" ",_tag_dyneither(_tmpA7D,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA78,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1936
goto _LL299;_LL2B2: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp526=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp507;if(_tmp526->tag != 12)goto _LL2B4;else{_tmp527=_tmp526->f1;_tmp528=_tmp526->f2;}}_LL2B3:
# 1938
 if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
if(_tmp528 != 0){
const char*_tmpA82;const char*_tmpA81;struct Cyc_PP_Doc*_tmpA80[7];exs_doc=((_tmpA80[6]=
# 1943
Cyc_Absynpp_rb(),((_tmpA80[5]=Cyc_PP_line_doc(),((_tmpA80[4]=
# 1942
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,((_tmpA81=",",_tag_dyneither(_tmpA81,sizeof(char),2))),_tmp528),((_tmpA80[3]=Cyc_PP_line_doc(),((_tmpA80[2]=
# 1941
Cyc_Absynpp_lb(),((_tmpA80[1]=Cyc_PP_text(((_tmpA82=" export ",_tag_dyneither(_tmpA82,sizeof(char),9)))),((_tmpA80[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA80,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1945
exs_doc=Cyc_Absynpp_rb();}{
const char*_tmpA87;const char*_tmpA86;struct Cyc_PP_Doc*_tmpA85[6];s=((_tmpA85[5]=exs_doc,((_tmpA85[4]=
# 1950
Cyc_PP_line_doc(),((_tmpA85[3]=
# 1949
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA86="",_tag_dyneither(_tmpA86,sizeof(char),1))),_tmp527),((_tmpA85[2]=
# 1948
Cyc_PP_line_doc(),((_tmpA85[1]=
# 1947
Cyc_Absynpp_lb(),((_tmpA85[0]=
# 1946
Cyc_PP_text(((_tmpA87="extern \"C include\" ",_tag_dyneither(_tmpA87,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA85,sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{
# 1953
const char*_tmpA92;const char*_tmpA91;const char*_tmpA90;const char*_tmpA8F;const char*_tmpA8E;struct Cyc_PP_Doc*_tmpA8D[9];s=((_tmpA8D[8]=
# 1961
Cyc_PP_text(((_tmpA8E=" */",_tag_dyneither(_tmpA8E,sizeof(char),4)))),((_tmpA8D[7]=
# 1960
Cyc_Absynpp_rb(),((_tmpA8D[6]=
# 1959
Cyc_PP_text(((_tmpA8F="/* ",_tag_dyneither(_tmpA8F,sizeof(char),4)))),((_tmpA8D[5]=
# 1958
Cyc_PP_line_doc(),((_tmpA8D[4]=
# 1957
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA90="",_tag_dyneither(_tmpA90,sizeof(char),1))),_tmp527),((_tmpA8D[3]=
# 1956
Cyc_PP_line_doc(),((_tmpA8D[2]=
# 1955
Cyc_PP_text(((_tmpA91=" */",_tag_dyneither(_tmpA91,sizeof(char),4)))),((_tmpA8D[1]=
# 1954
Cyc_Absynpp_lb(),((_tmpA8D[0]=
# 1953
Cyc_PP_text(((_tmpA92="/* extern \"C include\" ",_tag_dyneither(_tmpA92,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA8D,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1962
goto _LL299;_LL2B4: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp529=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp507;if(_tmp529->tag != 13)goto _LL2B6;}_LL2B5:
# 1964
{const char*_tmpA95;struct Cyc_PP_Doc*_tmpA94[2];s=((_tmpA94[1]=Cyc_Absynpp_lb(),((_tmpA94[0]=Cyc_PP_text(((_tmpA95="__cyclone_port_on__;",_tag_dyneither(_tmpA95,sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA94,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL299;_LL2B6: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp52A=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp507;if(_tmp52A->tag != 14)goto _LL299;}_LL2B7:
# 1967
{const char*_tmpA98;struct Cyc_PP_Doc*_tmpA97[2];s=((_tmpA97[1]=Cyc_Absynpp_lb(),((_tmpA97[0]=Cyc_PP_text(((_tmpA98="__cyclone_port_off__;",_tag_dyneither(_tmpA98,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA97,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL299;_LL299:;}
# 1971
return s;}
# 1974
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
switch(sc){case Cyc_Absyn_Static: _LL2C1: {
const char*_tmpA99;return Cyc_PP_text(((_tmpA99="static ",_tag_dyneither(_tmpA99,sizeof(char),8))));}case Cyc_Absyn_Public: _LL2C2:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL2C3: {
const char*_tmpA9A;return Cyc_PP_text(((_tmpA9A="extern ",_tag_dyneither(_tmpA9A,sizeof(char),8))));}case Cyc_Absyn_ExternC: _LL2C4: {
const char*_tmpA9B;return Cyc_PP_text(((_tmpA9B="extern \"C\" ",_tag_dyneither(_tmpA9B,sizeof(char),12))));}case Cyc_Absyn_Abstract: _LL2C5: {
const char*_tmpA9C;return Cyc_PP_text(((_tmpA9C="abstract ",_tag_dyneither(_tmpA9C,sizeof(char),10))));}case Cyc_Absyn_Register: _LL2C6: {
const char*_tmpA9D;return Cyc_PP_text(((_tmpA9D="register ",_tag_dyneither(_tmpA9D,sizeof(char),10))));}}}
# 1987
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp57B=t;struct Cyc_Absyn_Tvar*_tmp57D;struct Cyc_List_List*_tmp57F;_LL2C9: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp57C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp57B;if(_tmp57C->tag != 2)goto _LL2CB;else{_tmp57D=_tmp57C->f1;}}_LL2CA:
 return Cyc_Tcutil_is_temp_tvar(_tmp57D);_LL2CB: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp57E=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp57B;if(_tmp57E->tag != 24)goto _LL2CD;else{_tmp57F=_tmp57E->f1;}}_LL2CC:
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp57F);_LL2CD:;_LL2CE:
 return 0;_LL2C8:;}
# 1999
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp580=t;struct Cyc_Absyn_Typedefdecl*_tmp584;void*_tmp585;_LL2D0: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp581=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp580;if(_tmp581->tag != 12)goto _LL2D2;}_LL2D1:
 return 1;_LL2D2: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp582=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp580;if(_tmp582->tag != 14)goto _LL2D4;}_LL2D3:
 return 1;_LL2D4:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp583=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp580;if(_tmp583->tag != 17)goto _LL2D6;else{_tmp584=_tmp583->f3;_tmp585=(void*)_tmp583->f4;}}if(!(_tmp585 != 0))goto _LL2D6;_LL2D5:
# 2006
 return Cyc_Absynpp_is_anon_aggrtype(_tmp585);_LL2D6:;_LL2D7:
 return 0;_LL2CF:;}
# 2013
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 2016
if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmpA9E;struct _tuple16 _tmp586=(_tmpA9E.f1=(void*)tms->hd,((_tmpA9E.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA9E)));struct _tuple16 _tmp587=_tmp586;_LL2D9:{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp588=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp587.f1;if(_tmp588->tag != 2)goto _LL2DB;}{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp589=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp587.f2;if(_tmp589->tag != 3)goto _LL2DB;};_LL2DA: {
# 2019
struct Cyc_List_List*_tmpAA1;struct Cyc_List_List*_tmpAA0;return(_tmpAA0=_region_malloc(r,sizeof(*_tmpAA0)),((_tmpAA0->hd=(void*)tms->hd,((_tmpAA0->tl=((_tmpAA1=_region_malloc(r,sizeof(*_tmpAA1)),((_tmpAA1->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,((_tmpAA1->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpAA1)))))),_tmpAA0)))));}_LL2DB:;_LL2DC: {
struct Cyc_List_List*_tmpAA2;return(_tmpAA2=_region_malloc(r,sizeof(*_tmpAA2)),((_tmpAA2->hd=atts,((_tmpAA2->tl=tms,_tmpAA2)))));}_LL2D8:;}else{
# 2022
struct Cyc_List_List*_tmpAA3;return(_tmpAA3=_region_malloc(r,sizeof(*_tmpAA3)),((_tmpAA3->hd=atts,((_tmpAA3->tl=tms,_tmpAA3)))));}}
# 2027
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2029
void*_tmp58F=t;void*_tmp591;struct Cyc_Absyn_Tqual _tmp592;struct Cyc_Absyn_Exp*_tmp593;union Cyc_Absyn_Constraint*_tmp594;unsigned int _tmp595;void*_tmp597;struct Cyc_Absyn_Tqual _tmp598;struct Cyc_Absyn_PtrAtts _tmp599;struct Cyc_List_List*_tmp59B;void*_tmp59C;struct Cyc_Absyn_Tqual _tmp59D;void*_tmp59E;struct Cyc_List_List*_tmp59F;int _tmp5A0;struct Cyc_Absyn_VarargInfo*_tmp5A1;struct Cyc_List_List*_tmp5A2;struct Cyc_List_List*_tmp5A3;struct Cyc_Absyn_Exp*_tmp5A4;struct Cyc_Absyn_Exp*_tmp5A5;struct Cyc_Core_Opt*_tmp5A7;void*_tmp5A8;int _tmp5A9;struct _tuple0*_tmp5AB;struct Cyc_List_List*_tmp5AC;struct Cyc_Absyn_Typedefdecl*_tmp5AD;void*_tmp5AE;_LL2DE: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp590=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp58F;if(_tmp590->tag != 8)goto _LL2E0;else{_tmp591=(_tmp590->f1).elt_type;_tmp592=(_tmp590->f1).tq;_tmp593=(_tmp590->f1).num_elts;_tmp594=(_tmp590->f1).zero_term;_tmp595=(_tmp590->f1).zt_loc;}}_LL2DF: {
# 2032
struct _tuple12 _tmp5AF=Cyc_Absynpp_to_tms(r,_tmp592,_tmp591);struct Cyc_Absyn_Tqual _tmp5B1;void*_tmp5B2;struct Cyc_List_List*_tmp5B3;struct _tuple12 _tmp5B0=_tmp5AF;_tmp5B1=_tmp5B0.f1;_tmp5B2=_tmp5B0.f2;_tmp5B3=_tmp5B0.f3;{
void*tm;
if(_tmp593 == 0){
struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpAA6;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpAA5;tm=(void*)((_tmpAA5=_region_malloc(r,sizeof(*_tmpAA5)),((_tmpAA5[0]=((_tmpAA6.tag=0,((_tmpAA6.f1=_tmp594,((_tmpAA6.f2=_tmp595,_tmpAA6)))))),_tmpAA5))));}else{
# 2037
struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpAA9;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpAA8;tm=(void*)((_tmpAA8=_region_malloc(r,sizeof(*_tmpAA8)),((_tmpAA8[0]=((_tmpAA9.tag=1,((_tmpAA9.f1=_tmp593,((_tmpAA9.f2=_tmp594,((_tmpAA9.f3=_tmp595,_tmpAA9)))))))),_tmpAA8))));}{
struct Cyc_List_List*_tmpAAC;struct _tuple12 _tmpAAB;return(_tmpAAB.f1=_tmp5B1,((_tmpAAB.f2=_tmp5B2,((_tmpAAB.f3=((_tmpAAC=_region_malloc(r,sizeof(*_tmpAAC)),((_tmpAAC->hd=tm,((_tmpAAC->tl=_tmp5B3,_tmpAAC)))))),_tmpAAB)))));};};}_LL2E0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp596=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp58F;if(_tmp596->tag != 5)goto _LL2E2;else{_tmp597=(_tmp596->f1).elt_typ;_tmp598=(_tmp596->f1).elt_tq;_tmp599=(_tmp596->f1).ptr_atts;}}_LL2E1: {
# 2041
struct _tuple12 _tmp5BA=Cyc_Absynpp_to_tms(r,_tmp598,_tmp597);struct Cyc_Absyn_Tqual _tmp5BC;void*_tmp5BD;struct Cyc_List_List*_tmp5BE;struct _tuple12 _tmp5BB=_tmp5BA;_tmp5BC=_tmp5BB.f1;_tmp5BD=_tmp5BB.f2;_tmp5BE=_tmp5BB.f3;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpAB2;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpAB1;struct Cyc_List_List*_tmpAB0;_tmp5BE=((_tmpAB0=_region_malloc(r,sizeof(*_tmpAB0)),((_tmpAB0->hd=(void*)((_tmpAB2=_region_malloc(r,sizeof(*_tmpAB2)),((_tmpAB2[0]=((_tmpAB1.tag=2,((_tmpAB1.f1=_tmp599,((_tmpAB1.f2=tq,_tmpAB1)))))),_tmpAB2)))),((_tmpAB0->tl=_tmp5BE,_tmpAB0))))));}{
struct _tuple12 _tmpAB3;return(_tmpAB3.f1=_tmp5BC,((_tmpAB3.f2=_tmp5BD,((_tmpAB3.f3=_tmp5BE,_tmpAB3)))));};}_LL2E2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp59A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp58F;if(_tmp59A->tag != 9)goto _LL2E4;else{_tmp59B=(_tmp59A->f1).tvars;_tmp59C=(_tmp59A->f1).effect;_tmp59D=(_tmp59A->f1).ret_tqual;_tmp59E=(_tmp59A->f1).ret_typ;_tmp59F=(_tmp59A->f1).args;_tmp5A0=(_tmp59A->f1).c_varargs;_tmp5A1=(_tmp59A->f1).cyc_varargs;_tmp5A2=(_tmp59A->f1).rgn_po;_tmp5A3=(_tmp59A->f1).attributes;_tmp5A4=(_tmp59A->f1).requires_clause;_tmp5A5=(_tmp59A->f1).ensures_clause;}}_LL2E3:
# 2047
 if(!Cyc_Absynpp_print_all_tvars){
# 2051
if(_tmp59C == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect(_tmp59C)){
_tmp59C=0;
_tmp59B=0;}}else{
# 2056
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2059
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp59B);}{
# 2064
struct _tuple12 _tmp5C3=Cyc_Absynpp_to_tms(r,_tmp59D,_tmp59E);struct Cyc_Absyn_Tqual _tmp5C5;void*_tmp5C6;struct Cyc_List_List*_tmp5C7;struct _tuple12 _tmp5C4=_tmp5C3;_tmp5C5=_tmp5C4.f1;_tmp5C6=_tmp5C4.f2;_tmp5C7=_tmp5C4.f3;{
struct Cyc_List_List*tms=_tmp5C7;
# 2075 "absynpp.cyc"
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2EA:
# 2077
 if(_tmp5A3 != 0){
# 2080
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpAB6;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpAB5;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpAB5=_region_malloc(r,sizeof(*_tmpAB5)),((_tmpAB5[0]=((_tmpAB6.tag=5,((_tmpAB6.f1=0,((_tmpAB6.f2=_tmp5A3,_tmpAB6)))))),_tmpAB5)))),tms);}
# 2082
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpAC5;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpAC4;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpAC3;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpAC2;struct Cyc_List_List*_tmpAC1;tms=((_tmpAC1=_region_malloc(r,sizeof(*_tmpAC1)),((_tmpAC1->hd=(void*)((_tmpAC5=_region_malloc(r,sizeof(*_tmpAC5)),((_tmpAC5[0]=(
(_tmpAC2.tag=3,((_tmpAC2.f1=(void*)((_tmpAC4=_region_malloc(r,sizeof(*_tmpAC4)),((_tmpAC4[0]=((_tmpAC3.tag=1,((_tmpAC3.f1=_tmp59F,((_tmpAC3.f2=_tmp5A0,((_tmpAC3.f3=_tmp5A1,((_tmpAC3.f4=_tmp59C,((_tmpAC3.f5=_tmp5A2,((_tmpAC3.f6=_tmp5A4,((_tmpAC3.f7=_tmp5A5,_tmpAC3)))))))))))))))),_tmpAC4)))),_tmpAC2)))),_tmpAC5)))),((_tmpAC1->tl=tms,_tmpAC1))))));}
# 2086
break;case Cyc_Cyclone_Vc_c: _LL2EB:
# 2088
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpAD4;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpAD3;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpAD2;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpAD1;struct Cyc_List_List*_tmpAD0;tms=((_tmpAD0=_region_malloc(r,sizeof(*_tmpAD0)),((_tmpAD0->hd=(void*)((_tmpAD4=_region_malloc(r,sizeof(*_tmpAD4)),((_tmpAD4[0]=(
(_tmpAD1.tag=3,((_tmpAD1.f1=(void*)((_tmpAD3=_region_malloc(r,sizeof(*_tmpAD3)),((_tmpAD3[0]=((_tmpAD2.tag=1,((_tmpAD2.f1=_tmp59F,((_tmpAD2.f2=_tmp5A0,((_tmpAD2.f3=_tmp5A1,((_tmpAD2.f4=_tmp59C,((_tmpAD2.f5=_tmp5A2,((_tmpAD2.f6=_tmp5A4,((_tmpAD2.f7=_tmp5A5,_tmpAD2)))))))))))))))),_tmpAD3)))),_tmpAD1)))),_tmpAD4)))),((_tmpAD0->tl=tms,_tmpAD0))))));}
# 2092
for(0;_tmp5A3 != 0;_tmp5A3=_tmp5A3->tl){
void*_tmp5D4=(void*)_tmp5A3->hd;void*_tmp5D5=_tmp5D4;_LL2EE: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp5D6=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp5D5;if(_tmp5D6->tag != 1)goto _LL2F0;}_LL2EF:
 goto _LL2F1;_LL2F0: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp5D7=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp5D5;if(_tmp5D7->tag != 2)goto _LL2F2;}_LL2F1:
 goto _LL2F3;_LL2F2: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp5D8=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp5D5;if(_tmp5D8->tag != 3)goto _LL2F4;}_LL2F3:
# 2097
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpADE;struct Cyc_List_List*_tmpADD;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpADC;struct Cyc_List_List*_tmpADB;tms=((_tmpADB=_region_malloc(r,sizeof(*_tmpADB)),((_tmpADB->hd=(void*)((_tmpADE=_region_malloc(r,sizeof(*_tmpADE)),((_tmpADE[0]=((_tmpADC.tag=5,((_tmpADC.f1=0,((_tmpADC.f2=((_tmpADD=_cycalloc(sizeof(*_tmpADD)),((_tmpADD->hd=(void*)_tmp5A3->hd,((_tmpADD->tl=0,_tmpADD)))))),_tmpADC)))))),_tmpADE)))),((_tmpADB->tl=tms,_tmpADB))))));}
goto AfterAtts;_LL2F4:;_LL2F5:
 goto _LL2ED;_LL2ED:;}
# 2101
break;}
# 2104
AfterAtts:
 if(_tmp59B != 0){
struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpAE4;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpAE3;struct Cyc_List_List*_tmpAE2;tms=((_tmpAE2=_region_malloc(r,sizeof(*_tmpAE2)),((_tmpAE2->hd=(void*)((_tmpAE4=_region_malloc(r,sizeof(*_tmpAE4)),((_tmpAE4[0]=((_tmpAE3.tag=4,((_tmpAE3.f1=_tmp59B,((_tmpAE3.f2=0,((_tmpAE3.f3=1,_tmpAE3)))))))),_tmpAE4)))),((_tmpAE2->tl=tms,_tmpAE2))))));}{
struct _tuple12 _tmpAE5;return(_tmpAE5.f1=_tmp5C5,((_tmpAE5.f2=_tmp5C6,((_tmpAE5.f3=tms,_tmpAE5)))));};};};_LL2E4: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5A6=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp58F;if(_tmp5A6->tag != 1)goto _LL2E6;else{_tmp5A7=_tmp5A6->f1;_tmp5A8=(void*)_tmp5A6->f2;_tmp5A9=_tmp5A6->f3;}}_LL2E5:
# 2110
 if(_tmp5A8 == 0){
struct _tuple12 _tmpAE6;return(_tmpAE6.f1=tq,((_tmpAE6.f2=t,((_tmpAE6.f3=0,_tmpAE6)))));}else{
# 2113
return Cyc_Absynpp_to_tms(r,tq,_tmp5A8);}_LL2E6: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5AA=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp58F;if(_tmp5AA->tag != 17)goto _LL2E8;else{_tmp5AB=_tmp5AA->f1;_tmp5AC=_tmp5AA->f2;_tmp5AD=_tmp5AA->f3;_tmp5AE=(void*)_tmp5AA->f4;}}_LL2E7:
# 2118
 if((_tmp5AE == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(_tmp5AE)){
struct _tuple12 _tmpAE7;return(_tmpAE7.f1=tq,((_tmpAE7.f2=t,((_tmpAE7.f3=0,_tmpAE7)))));}else{
# 2121
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,_tmp5AE);}_LL2E8:;_LL2E9: {
# 2126
struct _tuple12 _tmpAE8;return(_tmpAE8.f1=tq,((_tmpAE8.f2=t,((_tmpAE8.f3=0,_tmpAE8)))));}_LL2DD:;}
# 2130
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2132
void*_tmp5E4=t;void*_tmp5E6;void*_tmp5E8;_LL2F7:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5E5=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5E4;if(_tmp5E5->tag != 1)goto _LL2F9;else{_tmp5E6=(void*)_tmp5E5->f2;}}if(!(_tmp5E6 != 0))goto _LL2F9;_LL2F8:
 return Cyc_Absynpp_is_char_ptr(_tmp5E6);_LL2F9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5E7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E4;if(_tmp5E7->tag != 5)goto _LL2FB;else{_tmp5E8=(_tmp5E7->f1).elt_typ;}}_LL2FA:
# 2135
 L: {
void*_tmp5E9=_tmp5E8;void*_tmp5EB;void*_tmp5ED;_LL2FE:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5EA=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5E9;if(_tmp5EA->tag != 1)goto _LL300;else{_tmp5EB=(void*)_tmp5EA->f2;}}if(!(_tmp5EB != 0))goto _LL300;_LL2FF:
 _tmp5E8=_tmp5EB;goto L;_LL300:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5EC=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E9;if(_tmp5EC->tag != 17)goto _LL302;else{_tmp5ED=(void*)_tmp5EC->f4;}}if(!(_tmp5ED != 0))goto _LL302;_LL301:
 _tmp5E8=_tmp5ED;goto L;_LL302: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5EE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5E9;if(_tmp5EE->tag != 6)goto _LL304;else{if(_tmp5EE->f2 != Cyc_Absyn_Char_sz)goto _LL304;}}_LL303:
 return 1;_LL304:;_LL305:
 return 0;_LL2FD:;}_LL2FB:;_LL2FC:
# 2142
 return 0;_LL2F6:;}
# 2146
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp5EF=_new_region("temp");struct _RegionHandle*temp=& _tmp5EF;_push_region(temp);
{struct _tuple12 _tmp5F0=Cyc_Absynpp_to_tms(temp,tq,typ);struct Cyc_Absyn_Tqual _tmp5F2;void*_tmp5F3;struct Cyc_List_List*_tmp5F4;struct _tuple12 _tmp5F1=_tmp5F0;_tmp5F2=_tmp5F1.f1;_tmp5F3=_tmp5F1.f2;_tmp5F4=_tmp5F1.f3;
_tmp5F4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5F4);
if(_tmp5F4 == 0  && dopt == 0){
struct Cyc_PP_Doc*_tmpAE9[2];struct Cyc_PP_Doc*_tmp5F6=(_tmpAE9[1]=Cyc_Absynpp_ntyp2doc(_tmp5F3),((_tmpAE9[0]=Cyc_Absynpp_tqual2doc(_tmp5F2),Cyc_PP_cat(_tag_dyneither(_tmpAE9,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return _tmp5F6;}else{
# 2153
const char*_tmpAEC;struct Cyc_PP_Doc*_tmpAEB[4];struct Cyc_PP_Doc*_tmp5F9=
(_tmpAEB[3]=
# 2157
Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp5F4),((_tmpAEB[2]=
# 2156
Cyc_PP_text(((_tmpAEC=" ",_tag_dyneither(_tmpAEC,sizeof(char),2)))),((_tmpAEB[1]=
# 2155
Cyc_Absynpp_ntyp2doc(_tmp5F3),((_tmpAEB[0]=
# 2154
Cyc_Absynpp_tqual2doc(_tmp5F2),Cyc_PP_cat(_tag_dyneither(_tmpAEB,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp5F9;}}
# 2148
;_pop_region(temp);}
# 2161
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp5FA=f->requires_clause;
if((unsigned int)_tmp5FA){
const char*_tmpAEF;struct Cyc_PP_Doc*_tmpAEE[2];requires_doc=((_tmpAEE[1]=Cyc_Absynpp_exp2doc(_tmp5FA),((_tmpAEE[0]=Cyc_PP_text(((_tmpAEF="@requires ",_tag_dyneither(_tmpAEF,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmpAEE,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 2167
requires_doc=Cyc_PP_nil_doc();}
# 2169
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL306:
# 2172
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpAF6;const char*_tmpAF5;const char*_tmpAF4;struct Cyc_PP_Doc*_tmpAF3[5];return(_tmpAF3[4]=
# 2175
Cyc_PP_text(((_tmpAF4=";",_tag_dyneither(_tmpAF4,sizeof(char),2)))),((_tmpAF3[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpAF3[2]=
# 2174
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAF3[1]=Cyc_PP_text(((_tmpAF5=":",_tag_dyneither(_tmpAF5,sizeof(char),2)))),((_tmpAF3[0]=
# 2173
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6->v=Cyc_PP_textptr(f->name),_tmpAF6))))),Cyc_PP_cat(_tag_dyneither(_tmpAF3,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2177
struct Cyc_Core_Opt*_tmpAFB;const char*_tmpAFA;struct Cyc_PP_Doc*_tmpAF9[4];return(_tmpAF9[3]=
Cyc_PP_text(((_tmpAFA=";",_tag_dyneither(_tmpAFA,sizeof(char),2)))),((_tmpAF9[2]=requires_doc,((_tmpAF9[1]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpAF9[0]=
# 2177
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAFB=_cycalloc(sizeof(*_tmpAFB)),((_tmpAFB->v=Cyc_PP_textptr(f->name),_tmpAFB))))),Cyc_PP_cat(_tag_dyneither(_tmpAF9,sizeof(struct Cyc_PP_Doc*),4)))))))));}case Cyc_Cyclone_Vc_c: _LL307:
# 2180
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpB02;const char*_tmpB01;const char*_tmpB00;struct Cyc_PP_Doc*_tmpAFF[5];return(_tmpAFF[4]=
# 2183
Cyc_PP_text(((_tmpB00=";",_tag_dyneither(_tmpB00,sizeof(char),2)))),((_tmpAFF[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAFF[2]=Cyc_PP_text(((_tmpB01=":",_tag_dyneither(_tmpB01,sizeof(char),2)))),((_tmpAFF[1]=
# 2182
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpB02=_cycalloc(sizeof(*_tmpB02)),((_tmpB02->v=Cyc_PP_textptr(f->name),_tmpB02))))),((_tmpAFF[0]=
# 2181
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpAFF,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2185
struct Cyc_Core_Opt*_tmpB07;const char*_tmpB06;struct Cyc_PP_Doc*_tmpB05[4];return(_tmpB05[3]=
# 2187
Cyc_PP_text(((_tmpB06=";",_tag_dyneither(_tmpB06,sizeof(char),2)))),((_tmpB05[2]=requires_doc,((_tmpB05[1]=
# 2186
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpB07=_cycalloc(sizeof(*_tmpB07)),((_tmpB07->v=Cyc_PP_textptr(f->name),_tmpB07))))),((_tmpB05[0]=
# 2185
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpB05,sizeof(struct Cyc_PP_Doc*),4)))))))));}}}
# 2192
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
const char*_tmpB08;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmpB08="",_tag_dyneither(_tmpB08,sizeof(char),1))),fields);}
# 2196
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
struct Cyc_PP_Doc*_tmpB09[3];return(_tmpB09[2]=
f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmpB09[1]=
# 2197
Cyc_Absynpp_typedef_name2doc(f->name),((_tmpB09[0]=Cyc_Absynpp_scope2doc(f->sc),Cyc_PP_cat(_tag_dyneither(_tmpB09,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 2201
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
const char*_tmpB0A;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmpB0A=",",_tag_dyneither(_tmpB0A,sizeof(char),2))),fields);}
# 2210
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{
const char*_tmpB0D;void*_tmpB0C;(_tmpB0C=0,Cyc_fprintf(f,((_tmpB0D="\n",_tag_dyneither(_tmpB0D,sizeof(char),2))),_tag_dyneither(_tmpB0C,sizeof(void*),0)));};}}
# 2217
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
const char*_tmpB0E;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpB0E="",_tag_dyneither(_tmpB0E,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),72);}
# 2220
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
# 2224
struct _dyneither_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_tvar2doc(t),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*t){
int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;
int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;
Cyc_Absynpp_add_cyc_prefix=0;{
struct _dyneither_ptr s=Cyc_Absynpp_typ2string(t);
Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=old_add_cyc_prefix;
return s;};}
# 2235
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _dyneither_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}
