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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 533
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 559
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 566
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 584
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 747 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 951
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 961
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 969
void*Cyc_Absyn_compress_kb(void*);
# 974
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 1004
extern void*Cyc_Absyn_bounds_one;
# 1059
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1086
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1191
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1197
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
# 43 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 54
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};
# 73
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 77
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 88
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 124 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 71 "tcutil.h"
void*Cyc_Tcutil_compress(void*t);
# 316 "tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 318
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
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
Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v;_tmp1->tl=0;_tmp1;}));}
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
char _tmp2=c;switch(_tmp2){case 7: _LL1: _LL2:
 return({const char*_tmp3="\\a";_tag_dyneither(_tmp3,sizeof(char),3);});case 8: _LL3: _LL4:
 return({const char*_tmp4="\\b";_tag_dyneither(_tmp4,sizeof(char),3);});case 12: _LL5: _LL6:
 return({const char*_tmp5="\\f";_tag_dyneither(_tmp5,sizeof(char),3);});case 10: _LL7: _LL8:
 return({const char*_tmp6="\\n";_tag_dyneither(_tmp6,sizeof(char),3);});case 13: _LL9: _LLA:
 return({const char*_tmp7="\\r";_tag_dyneither(_tmp7,sizeof(char),3);});case 9: _LLB: _LLC:
 return({const char*_tmp8="\\t";_tag_dyneither(_tmp8,sizeof(char),3);});case 11: _LLD: _LLE:
 return({const char*_tmp9="\\v";_tag_dyneither(_tmp9,sizeof(char),3);});case 92: _LLF: _LL10:
 return({const char*_tmpA="\\\\";_tag_dyneither(_tmpA,sizeof(char),3);});case 34: _LL11: _LL12:
 return({const char*_tmpB="\"";_tag_dyneither(_tmpB,sizeof(char),2);});case 39: _LL13: _LL14:
 return({const char*_tmpC="\\'";_tag_dyneither(_tmpC,sizeof(char),3);});default: _LL15: _LL16:
# 277
 if(c >= ' '  && c <= '~'){
struct _dyneither_ptr _tmpD=Cyc_Core_new_string(2);
({struct _dyneither_ptr _tmpE=_dyneither_ptr_plus(_tmpD,sizeof(char),0);char _tmpF=*((char*)_check_dyneither_subscript(_tmpE,sizeof(char),0));char _tmp10=c;if(_get_dyneither_size(_tmpE,sizeof(char))== 1  && (_tmpF == '\000'  && _tmp10 != '\000'))_throw_arraybounds();*((char*)_tmpE.curr)=_tmp10;});
return(struct _dyneither_ptr)_tmpD;}else{
# 282
struct _dyneither_ptr _tmp11=Cyc_Core_new_string(5);
int j=0;
({struct _dyneither_ptr _tmp12=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp13=*((char*)_check_dyneither_subscript(_tmp12,sizeof(char),0));char _tmp14='\\';if(_get_dyneither_size(_tmp12,sizeof(char))== 1  && (_tmp13 == '\000'  && _tmp14 != '\000'))_throw_arraybounds();*((char*)_tmp12.curr)=_tmp14;});
({struct _dyneither_ptr _tmp15=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp16=*((char*)_check_dyneither_subscript(_tmp15,sizeof(char),0));char _tmp17=(char)('0' + ((unsigned char)c >> 6 & 3));if(_get_dyneither_size(_tmp15,sizeof(char))== 1  && (_tmp16 == '\000'  && _tmp17 != '\000'))_throw_arraybounds();*((char*)_tmp15.curr)=_tmp17;});
({struct _dyneither_ptr _tmp18=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp19=*((char*)_check_dyneither_subscript(_tmp18,sizeof(char),0));char _tmp1A=(char)('0' + (c >> 3 & 7));if(_get_dyneither_size(_tmp18,sizeof(char))== 1  && (_tmp19 == '\000'  && _tmp1A != '\000'))_throw_arraybounds();*((char*)_tmp18.curr)=_tmp1A;});
({struct _dyneither_ptr _tmp1B=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp1C=*((char*)_check_dyneither_subscript(_tmp1B,sizeof(char),0));char _tmp1D=(char)('0' + (c & 7));if(_get_dyneither_size(_tmp1B,sizeof(char))== 1  && (_tmp1C == '\000'  && _tmp1D != '\000'))_throw_arraybounds();*((char*)_tmp1B.curr)=_tmp1D;});
return(struct _dyneither_ptr)_tmp11;}}_LL0:;}
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
char _tmp1E=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));char _tmp1F=_tmp1E;char _tmp20;switch(_tmp1F){case 7: _LL18: _LL19:
 goto _LL1B;case 8: _LL1A: _LL1B:
 goto _LL1D;case 12: _LL1C: _LL1D:
 goto _LL1F;case 10: _LL1E: _LL1F:
 goto _LL21;case 13: _LL20: _LL21:
 goto _LL23;case 9: _LL22: _LL23:
 goto _LL25;case 11: _LL24: _LL25:
 goto _LL27;case 92: _LL26: _LL27:
 goto _LL29;case 34: _LL28: _LL29:
 len +=2;goto _LL17;default: _LL2A: _tmp20=_tmp1F;_LL2B:
# 323
 if(_tmp20 >= ' '  && _tmp20 <= '~')++ len;else{
len +=4;}
goto _LL17;}_LL17:;}}{
# 328
struct _dyneither_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));
int j=0;
{int i=0;for(0;i <= n;++ i){
char _tmp21=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));char _tmp22=_tmp21;char _tmp23;switch(_tmp22){case 7: _LL2D: _LL2E:
({struct _dyneither_ptr _tmp24=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp25=*((char*)_check_dyneither_subscript(_tmp24,sizeof(char),0));char _tmp26='\\';if(_get_dyneither_size(_tmp24,sizeof(char))== 1  && (_tmp25 == '\000'  && _tmp26 != '\000'))_throw_arraybounds();*((char*)_tmp24.curr)=_tmp26;});({struct _dyneither_ptr _tmp27=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp28=*((char*)_check_dyneither_subscript(_tmp27,sizeof(char),0));char _tmp29='a';if(_get_dyneither_size(_tmp27,sizeof(char))== 1  && (_tmp28 == '\000'  && _tmp29 != '\000'))_throw_arraybounds();*((char*)_tmp27.curr)=_tmp29;});goto _LL2C;case 8: _LL2F: _LL30:
({struct _dyneither_ptr _tmp2A=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp2B=*((char*)_check_dyneither_subscript(_tmp2A,sizeof(char),0));char _tmp2C='\\';if(_get_dyneither_size(_tmp2A,sizeof(char))== 1  && (_tmp2B == '\000'  && _tmp2C != '\000'))_throw_arraybounds();*((char*)_tmp2A.curr)=_tmp2C;});({struct _dyneither_ptr _tmp2D=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp2E=*((char*)_check_dyneither_subscript(_tmp2D,sizeof(char),0));char _tmp2F='b';if(_get_dyneither_size(_tmp2D,sizeof(char))== 1  && (_tmp2E == '\000'  && _tmp2F != '\000'))_throw_arraybounds();*((char*)_tmp2D.curr)=_tmp2F;});goto _LL2C;case 12: _LL31: _LL32:
({struct _dyneither_ptr _tmp30=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp31=*((char*)_check_dyneither_subscript(_tmp30,sizeof(char),0));char _tmp32='\\';if(_get_dyneither_size(_tmp30,sizeof(char))== 1  && (_tmp31 == '\000'  && _tmp32 != '\000'))_throw_arraybounds();*((char*)_tmp30.curr)=_tmp32;});({struct _dyneither_ptr _tmp33=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp34=*((char*)_check_dyneither_subscript(_tmp33,sizeof(char),0));char _tmp35='f';if(_get_dyneither_size(_tmp33,sizeof(char))== 1  && (_tmp34 == '\000'  && _tmp35 != '\000'))_throw_arraybounds();*((char*)_tmp33.curr)=_tmp35;});goto _LL2C;case 10: _LL33: _LL34:
({struct _dyneither_ptr _tmp36=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp37=*((char*)_check_dyneither_subscript(_tmp36,sizeof(char),0));char _tmp38='\\';if(_get_dyneither_size(_tmp36,sizeof(char))== 1  && (_tmp37 == '\000'  && _tmp38 != '\000'))_throw_arraybounds();*((char*)_tmp36.curr)=_tmp38;});({struct _dyneither_ptr _tmp39=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp3A=*((char*)_check_dyneither_subscript(_tmp39,sizeof(char),0));char _tmp3B='n';if(_get_dyneither_size(_tmp39,sizeof(char))== 1  && (_tmp3A == '\000'  && _tmp3B != '\000'))_throw_arraybounds();*((char*)_tmp39.curr)=_tmp3B;});goto _LL2C;case 13: _LL35: _LL36:
({struct _dyneither_ptr _tmp3C=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp3D=*((char*)_check_dyneither_subscript(_tmp3C,sizeof(char),0));char _tmp3E='\\';if(_get_dyneither_size(_tmp3C,sizeof(char))== 1  && (_tmp3D == '\000'  && _tmp3E != '\000'))_throw_arraybounds();*((char*)_tmp3C.curr)=_tmp3E;});({struct _dyneither_ptr _tmp3F=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp40=*((char*)_check_dyneither_subscript(_tmp3F,sizeof(char),0));char _tmp41='r';if(_get_dyneither_size(_tmp3F,sizeof(char))== 1  && (_tmp40 == '\000'  && _tmp41 != '\000'))_throw_arraybounds();*((char*)_tmp3F.curr)=_tmp41;});goto _LL2C;case 9: _LL37: _LL38:
({struct _dyneither_ptr _tmp42=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp43=*((char*)_check_dyneither_subscript(_tmp42,sizeof(char),0));char _tmp44='\\';if(_get_dyneither_size(_tmp42,sizeof(char))== 1  && (_tmp43 == '\000'  && _tmp44 != '\000'))_throw_arraybounds();*((char*)_tmp42.curr)=_tmp44;});({struct _dyneither_ptr _tmp45=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp46=*((char*)_check_dyneither_subscript(_tmp45,sizeof(char),0));char _tmp47='t';if(_get_dyneither_size(_tmp45,sizeof(char))== 1  && (_tmp46 == '\000'  && _tmp47 != '\000'))_throw_arraybounds();*((char*)_tmp45.curr)=_tmp47;});goto _LL2C;case 11: _LL39: _LL3A:
({struct _dyneither_ptr _tmp48=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp49=*((char*)_check_dyneither_subscript(_tmp48,sizeof(char),0));char _tmp4A='\\';if(_get_dyneither_size(_tmp48,sizeof(char))== 1  && (_tmp49 == '\000'  && _tmp4A != '\000'))_throw_arraybounds();*((char*)_tmp48.curr)=_tmp4A;});({struct _dyneither_ptr _tmp4B=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp4C=*((char*)_check_dyneither_subscript(_tmp4B,sizeof(char),0));char _tmp4D='v';if(_get_dyneither_size(_tmp4B,sizeof(char))== 1  && (_tmp4C == '\000'  && _tmp4D != '\000'))_throw_arraybounds();*((char*)_tmp4B.curr)=_tmp4D;});goto _LL2C;case 92: _LL3B: _LL3C:
({struct _dyneither_ptr _tmp4E=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp4F=*((char*)_check_dyneither_subscript(_tmp4E,sizeof(char),0));char _tmp50='\\';if(_get_dyneither_size(_tmp4E,sizeof(char))== 1  && (_tmp4F == '\000'  && _tmp50 != '\000'))_throw_arraybounds();*((char*)_tmp4E.curr)=_tmp50;});({struct _dyneither_ptr _tmp51=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp52=*((char*)_check_dyneither_subscript(_tmp51,sizeof(char),0));char _tmp53='\\';if(_get_dyneither_size(_tmp51,sizeof(char))== 1  && (_tmp52 == '\000'  && _tmp53 != '\000'))_throw_arraybounds();*((char*)_tmp51.curr)=_tmp53;});goto _LL2C;case 34: _LL3D: _LL3E:
({struct _dyneither_ptr _tmp54=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp55=*((char*)_check_dyneither_subscript(_tmp54,sizeof(char),0));char _tmp56='\\';if(_get_dyneither_size(_tmp54,sizeof(char))== 1  && (_tmp55 == '\000'  && _tmp56 != '\000'))_throw_arraybounds();*((char*)_tmp54.curr)=_tmp56;});({struct _dyneither_ptr _tmp57=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp58=*((char*)_check_dyneither_subscript(_tmp57,sizeof(char),0));char _tmp59='"';if(_get_dyneither_size(_tmp57,sizeof(char))== 1  && (_tmp58 == '\000'  && _tmp59 != '\000'))_throw_arraybounds();*((char*)_tmp57.curr)=_tmp59;});goto _LL2C;default: _LL3F: _tmp23=_tmp22;_LL40:
# 342
 if(_tmp23 >= ' '  && _tmp23 <= '~')({struct _dyneither_ptr _tmp5A=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp5B=*((char*)_check_dyneither_subscript(_tmp5A,sizeof(char),0));char _tmp5C=_tmp23;if(_get_dyneither_size(_tmp5A,sizeof(char))== 1  && (_tmp5B == '\000'  && _tmp5C != '\000'))_throw_arraybounds();*((char*)_tmp5A.curr)=_tmp5C;});else{
# 344
({struct _dyneither_ptr _tmp5D=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp5E=*((char*)_check_dyneither_subscript(_tmp5D,sizeof(char),0));char _tmp5F='\\';if(_get_dyneither_size(_tmp5D,sizeof(char))== 1  && (_tmp5E == '\000'  && _tmp5F != '\000'))_throw_arraybounds();*((char*)_tmp5D.curr)=_tmp5F;});
({struct _dyneither_ptr _tmp60=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp61=*((char*)_check_dyneither_subscript(_tmp60,sizeof(char),0));char _tmp62=(char)('0' + (_tmp23 >> 6 & 7));if(_get_dyneither_size(_tmp60,sizeof(char))== 1  && (_tmp61 == '\000'  && _tmp62 != '\000'))_throw_arraybounds();*((char*)_tmp60.curr)=_tmp62;});
({struct _dyneither_ptr _tmp63=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp64=*((char*)_check_dyneither_subscript(_tmp63,sizeof(char),0));char _tmp65=(char)('0' + (_tmp23 >> 3 & 7));if(_get_dyneither_size(_tmp63,sizeof(char))== 1  && (_tmp64 == '\000'  && _tmp65 != '\000'))_throw_arraybounds();*((char*)_tmp63.curr)=_tmp65;});
({struct _dyneither_ptr _tmp66=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp67=*((char*)_check_dyneither_subscript(_tmp66,sizeof(char),0));char _tmp68=(char)('0' + (_tmp23 & 7));if(_get_dyneither_size(_tmp66,sizeof(char))== 1  && (_tmp67 == '\000'  && _tmp68 != '\000'))_throw_arraybounds();*((char*)_tmp66.curr)=_tmp68;});}
# 349
goto _LL2C;}_LL2C:;}}
# 351
return(struct _dyneither_ptr)t;};};};}static char _tmp69[9]="restrict";
# 354
static struct _dyneither_ptr Cyc_Absynpp_restrict_string={_tmp69,_tmp69,_tmp69 + 9};static char _tmp6A[9]="volatile";
static struct _dyneither_ptr Cyc_Absynpp_volatile_string={_tmp6A,_tmp6A,_tmp6A + 9};static char _tmp6B[6]="const";
static struct _dyneither_ptr Cyc_Absynpp_const_str={_tmp6B,_tmp6B,_tmp6B + 6};
static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
# 361
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){
struct Cyc_List_List*l=0;
# 364
if(tq.q_restrict)l=({struct Cyc_List_List*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->hd=Cyc_Absynpp_restrict_sp;_tmp6C->tl=l;_tmp6C;});
if(tq.q_volatile)l=({struct Cyc_List_List*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->hd=Cyc_Absynpp_volatile_sp;_tmp6D->tl=l;_tmp6D;});
if(tq.print_const)l=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=Cyc_Absynpp_const_sp;_tmp6E->tl=l;_tmp6E;});
return Cyc_PP_egroup(({const char*_tmp6F="";_tag_dyneither(_tmp6F,sizeof(char),1);}),({const char*_tmp70=" ";_tag_dyneither(_tmp70,sizeof(char),2);}),({const char*_tmp71=" ";_tag_dyneither(_tmp71,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));}
# 370
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp72=ka;enum Cyc_Absyn_KindQual _tmp73;enum Cyc_Absyn_AliasQual _tmp74;_LL42: _tmp73=_tmp72->kind;_tmp74=_tmp72->aliasqual;_LL43:;{
enum Cyc_Absyn_KindQual _tmp75=_tmp73;switch(_tmp75){case Cyc_Absyn_AnyKind: _LL45: _LL46: {
# 374
enum Cyc_Absyn_AliasQual _tmp76=_tmp74;switch(_tmp76){case Cyc_Absyn_Aliasable: _LL52: _LL53:
 return({const char*_tmp77="A";_tag_dyneither(_tmp77,sizeof(char),2);});case Cyc_Absyn_Unique: _LL54: _LL55:
 return({const char*_tmp78="UA";_tag_dyneither(_tmp78,sizeof(char),3);});default: _LL56: _LL57:
 return({const char*_tmp79="TA";_tag_dyneither(_tmp79,sizeof(char),3);});}_LL51:;}case Cyc_Absyn_MemKind: _LL47: _LL48: {
# 380
enum Cyc_Absyn_AliasQual _tmp7A=_tmp74;switch(_tmp7A){case Cyc_Absyn_Aliasable: _LL59: _LL5A:
 return({const char*_tmp7B="M";_tag_dyneither(_tmp7B,sizeof(char),2);});case Cyc_Absyn_Unique: _LL5B: _LL5C:
 return({const char*_tmp7C="UM";_tag_dyneither(_tmp7C,sizeof(char),3);});default: _LL5D: _LL5E:
 return({const char*_tmp7D="TM";_tag_dyneither(_tmp7D,sizeof(char),3);});}_LL58:;}case Cyc_Absyn_BoxKind: _LL49: _LL4A: {
# 386
enum Cyc_Absyn_AliasQual _tmp7E=_tmp74;switch(_tmp7E){case Cyc_Absyn_Aliasable: _LL60: _LL61:
 return({const char*_tmp7F="B";_tag_dyneither(_tmp7F,sizeof(char),2);});case Cyc_Absyn_Unique: _LL62: _LL63:
 return({const char*_tmp80="UB";_tag_dyneither(_tmp80,sizeof(char),3);});default: _LL64: _LL65:
 return({const char*_tmp81="TB";_tag_dyneither(_tmp81,sizeof(char),3);});}_LL5F:;}case Cyc_Absyn_RgnKind: _LL4B: _LL4C: {
# 392
enum Cyc_Absyn_AliasQual _tmp82=_tmp74;switch(_tmp82){case Cyc_Absyn_Aliasable: _LL67: _LL68:
 return({const char*_tmp83="R";_tag_dyneither(_tmp83,sizeof(char),2);});case Cyc_Absyn_Unique: _LL69: _LL6A:
 return({const char*_tmp84="UR";_tag_dyneither(_tmp84,sizeof(char),3);});default: _LL6B: _LL6C:
 return({const char*_tmp85="TR";_tag_dyneither(_tmp85,sizeof(char),3);});}_LL66:;}case Cyc_Absyn_EffKind: _LL4D: _LL4E:
# 397
 return({const char*_tmp86="E";_tag_dyneither(_tmp86,sizeof(char),2);});default: _LL4F: _LL50:
 return({const char*_tmp87="I";_tag_dyneither(_tmp87,sizeof(char),2);});}_LL44:;};}
# 401
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 403
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp88=Cyc_Absyn_compress_kb(c);void*_tmp89=_tmp88;struct Cyc_Absyn_Kind*_tmp8A;struct Cyc_Absyn_Kind*_tmp8B;switch(*((int*)_tmp89)){case 0: _LL6E: _tmp8B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp89)->f1;_LL6F:
 return Cyc_Absynpp_kind2string(_tmp8B);case 1: _LL70: _LL71:
# 407
 if(Cyc_PP_tex_output)
return({const char*_tmp8C="{?}";_tag_dyneither(_tmp8C,sizeof(char),4);});else{
return({const char*_tmp8D="?";_tag_dyneither(_tmp8D,sizeof(char),2);});}default: _LL72: _tmp8A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89)->f2;_LL73:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp90;_tmp90.tag=0;_tmp90.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp8A));({void*_tmp8E[1]={& _tmp90};Cyc_aprintf(({const char*_tmp8F="<=%s";_tag_dyneither(_tmp8F,sizeof(char),5);}),_tag_dyneither(_tmp8E,sizeof(void*),1));});});}_LL6D:;}
# 414
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp91=Cyc_Absyn_compress_kb(c);void*_tmp92=_tmp91;struct Cyc_Absyn_Kind*_tmp93;struct Cyc_Absyn_Kind*_tmp94;switch(*((int*)_tmp92)){case 0: _LL75: _tmp94=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp92)->f1;_LL76:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp94));case 1: _LL77: _LL78:
# 418
 if(Cyc_PP_tex_output)
return Cyc_PP_text_width(({const char*_tmp95="{?}";_tag_dyneither(_tmp95,sizeof(char),4);}),1);else{
return Cyc_PP_text(({const char*_tmp96="?";_tag_dyneither(_tmp96,sizeof(char),2);}));}default: _LL79: _tmp93=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp92)->f2;_LL7A:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp93));}_LL74:;}
# 425
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
return Cyc_PP_egroup(({const char*_tmp97="<";_tag_dyneither(_tmp97,sizeof(char),2);}),({const char*_tmp98=">";_tag_dyneither(_tmp98,sizeof(char),2);}),({const char*_tmp99=",";_tag_dyneither(_tmp99,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));}
# 429
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr _tmp9A=*tv->name;
# 433
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(tv)){
# 435
struct _dyneither_ptr kstring=({const char*_tmpA3="K";_tag_dyneither(_tmpA3,sizeof(char),2);});
{void*_tmp9B=Cyc_Absyn_compress_kb(tv->kind);void*_tmp9C=_tmp9B;struct Cyc_Absyn_Kind*_tmp9D;struct Cyc_Absyn_Kind*_tmp9E;switch(*((int*)_tmp9C)){case 2: _LL7C: _tmp9E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9C)->f2;_LL7D:
 _tmp9D=_tmp9E;goto _LL7F;case 0: _LL7E: _tmp9D=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9C)->f1;_LL7F:
 kstring=Cyc_Absynpp_kind2string(_tmp9D);goto _LL7B;default: _LL80: _LL81:
 goto _LL7B;}_LL7B:;}
# 441
return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA2;_tmpA2.tag=0;_tmpA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_tmp9A,sizeof(char),1));({struct Cyc_String_pa_PrintArg_struct _tmpA1;_tmpA1.tag=0;_tmpA1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kstring);({void*_tmp9F[2]={& _tmpA1,& _tmpA2};Cyc_aprintf(({const char*_tmpA0="`G%s%s";_tag_dyneither(_tmpA0,sizeof(char),7);}),_tag_dyneither(_tmp9F,sizeof(void*),2));});});}));}
# 443
return Cyc_PP_text(_tmp9A);}
# 446
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmpA4=Cyc_Absyn_compress_kb(tv->kind);void*_tmpA5=_tmpA4;struct Cyc_Absyn_Kind*_tmpA6;struct Cyc_Absyn_Kind*_tmpA7;switch(*((int*)_tmpA5)){case 1: _LL83: _LL84:
 goto _LL86;case 0: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL85: _LL86:
 return Cyc_Absynpp_tvar2doc(tv);}else{goto _LL89;}}else{_LL89: _tmpA7=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5)->f1;_LL8A:
# 451
 return({struct Cyc_PP_Doc*_tmpA8[3];_tmpA8[2]=Cyc_Absynpp_kind2doc(_tmpA7);_tmpA8[1]=Cyc_PP_text(({const char*_tmpA9="::";_tag_dyneither(_tmpA9,sizeof(char),3);}));_tmpA8[0]=Cyc_Absynpp_tvar2doc(tv);Cyc_PP_cat(_tag_dyneither(_tmpA8,sizeof(struct Cyc_PP_Doc*),3));});}default: _LL87: _tmpA6=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA5)->f2;_LL88:
# 450
 _tmpA7=_tmpA6;goto _LL8A;}_LL82:;}
# 455
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return Cyc_PP_egroup(({const char*_tmpAA="<";_tag_dyneither(_tmpAA,sizeof(char),2);}),({const char*_tmpAB=">";_tag_dyneither(_tmpAB,sizeof(char),2);}),({const char*_tmpAC=",";_tag_dyneither(_tmpAC,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}
# 459
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);
return Cyc_PP_egroup(({const char*_tmpAD="<";_tag_dyneither(_tmpAD,sizeof(char),2);}),({const char*_tmpAE=">";_tag_dyneither(_tmpAE,sizeof(char),2);}),({const char*_tmpAF=",";_tag_dyneither(_tmpAF,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 465
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 469
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return Cyc_PP_group(({const char*_tmpB0="(";_tag_dyneither(_tmpB0,sizeof(char),2);}),({const char*_tmpB1=")";_tag_dyneither(_tmpB1,sizeof(char),2);}),({const char*_tmpB2=",";_tag_dyneither(_tmpB2,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
# 473
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
void*_tmpB3=att;switch(*((int*)_tmpB3)){case 1: _LL8C: _LL8D:
 goto _LL8F;case 2: _LL8E: _LL8F:
 goto _LL91;case 3: _LL90: _LL91:
 return Cyc_PP_nil_doc();default: _LL92: _LL93:
 return Cyc_PP_text(Cyc_Absyn_attribute2string(att));}_LL8B:;}
# 482
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmpB4=(void*)atts->hd;void*_tmpB5=_tmpB4;switch(*((int*)_tmpB5)){case 1: _LL95: _LL96:
 return Cyc_PP_text(({const char*_tmpB6=" _stdcall ";_tag_dyneither(_tmpB6,sizeof(char),11);}));case 2: _LL97: _LL98:
 return Cyc_PP_text(({const char*_tmpB7=" _cdecl ";_tag_dyneither(_tmpB7,sizeof(char),9);}));case 3: _LL99: _LL9A:
 return Cyc_PP_text(({const char*_tmpB8=" _fastcall ";_tag_dyneither(_tmpB8,sizeof(char),12);}));default: _LL9B: _LL9C:
 goto _LL94;}_LL94:;}
# 490
return Cyc_PP_nil_doc();}
# 493
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpB9=(void*)atts2->hd;void*_tmpBA=_tmpB9;switch(*((int*)_tmpBA)){case 1: _LL9E: _LL9F:
 goto _LLA1;case 2: _LLA0: _LLA1:
 goto _LLA3;case 3: _LLA2: _LLA3:
 goto _LL9D;default: _LLA4: _LLA5:
 hasatt=1;goto _LL9D;}_LL9D:;}}
# 502
if(!hasatt)
return Cyc_PP_nil_doc();
return({struct Cyc_PP_Doc*_tmpBB[3];_tmpBB[2]=
# 506
Cyc_PP_text(({const char*_tmpC0=")";_tag_dyneither(_tmpC0,sizeof(char),2);}));_tmpBB[1]=
# 505
Cyc_PP_group(({const char*_tmpBD="";_tag_dyneither(_tmpBD,sizeof(char),1);}),({const char*_tmpBE="";_tag_dyneither(_tmpBE,sizeof(char),1);}),({const char*_tmpBF=" ";_tag_dyneither(_tmpBF,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));_tmpBB[0]=
# 504
Cyc_PP_text(({const char*_tmpBC=" __declspec(";_tag_dyneither(_tmpBC,sizeof(char),13);}));Cyc_PP_cat(_tag_dyneither(_tmpBB,sizeof(struct Cyc_PP_Doc*),3));});}
# 509
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 513
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();{
enum Cyc_Cyclone_C_Compilers _tmpC1=Cyc_Cyclone_c_compiler;if(_tmpC1 == Cyc_Cyclone_Vc_c){_LLA7: _LLA8:
 return Cyc_Absynpp_noncallconv2doc(atts);}else{_LLA9: _LLAA:
 return({struct Cyc_PP_Doc*_tmpC2[2];_tmpC2[1]=
Cyc_PP_group(({const char*_tmpC4="((";_tag_dyneither(_tmpC4,sizeof(char),3);}),({const char*_tmpC5="))";_tag_dyneither(_tmpC5,sizeof(char),3);}),({const char*_tmpC6=",";_tag_dyneither(_tmpC6,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));_tmpC2[0]=
# 517
Cyc_PP_text(({const char*_tmpC3=" __attribute__";_tag_dyneither(_tmpC3,sizeof(char),15);}));Cyc_PP_cat(_tag_dyneither(_tmpC2,sizeof(struct Cyc_PP_Doc*),2));});}_LLA6:;};}
# 522
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmpC7=(void*)tms->hd;void*_tmpC8=_tmpC7;switch(*((int*)_tmpC8)){case 2: _LLAC: _LLAD:
 return 1;case 5: _LLAE: _LLAF: {
# 527
enum Cyc_Cyclone_C_Compilers _tmpC9=Cyc_Cyclone_c_compiler;if(_tmpC9 == Cyc_Cyclone_Gcc_c){_LLB3: _LLB4:
 return 0;}else{_LLB5: _LLB6:
 return Cyc_Absynpp_next_is_pointer(tms->tl);}_LLB2:;}default: _LLB0: _LLB1:
# 531
 return 0;}_LLAB:;};}
# 535
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
# 538
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_question(){
if(!((unsigned int)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_question=Cyc_PP_text_width(({const char*_tmpCA="{?}";_tag_dyneither(_tmpCA,sizeof(char),4);}),1);else{
Cyc_Absynpp_cache_question=Cyc_PP_text(({const char*_tmpCB="?";_tag_dyneither(_tmpCB,sizeof(char),2);}));}}
# 545
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 547
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_lb=Cyc_PP_text_width(({const char*_tmpCC="{\\lb}";_tag_dyneither(_tmpCC,sizeof(char),6);}),1);else{
Cyc_Absynpp_cache_lb=Cyc_PP_text(({const char*_tmpCD="{";_tag_dyneither(_tmpCD,sizeof(char),2);}));}}
# 554
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 556
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_rb=Cyc_PP_text_width(({const char*_tmpCE="{\\rb}";_tag_dyneither(_tmpCE,sizeof(char),6);}),1);else{
Cyc_Absynpp_cache_rb=Cyc_PP_text(({const char*_tmpCF="}";_tag_dyneither(_tmpCF,sizeof(char),2);}));}}
# 563
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 565
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_dollar=Cyc_PP_text_width(({const char*_tmpD0="\\$";_tag_dyneither(_tmpD0,sizeof(char),3);}),1);else{
Cyc_Absynpp_cache_dollar=Cyc_PP_text(({const char*_tmpD1="$";_tag_dyneither(_tmpD1,sizeof(char),2);}));}}
# 572
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 574
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_tmpD2[3];_tmpD2[2]=Cyc_Absynpp_rb();_tmpD2[1]=Cyc_PP_seq(sep,ss);_tmpD2[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dyneither(_tmpD2,sizeof(struct Cyc_PP_Doc*),3));});}
# 579
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpD3=(void*)tms->hd;void*_tmpD4=_tmpD3;struct Cyc_List_List*_tmpD5;switch(*((int*)_tmpD4)){case 0: _LLB8: _LLB9:
({void*_tmpD6=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpD7="Carray_mod ";_tag_dyneither(_tmpD7,sizeof(char),12);}),_tag_dyneither(_tmpD6,sizeof(void*),0));});goto _LLB7;case 1: _LLBA: _LLBB:
({void*_tmpD8=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpD9="ConstArray_mod ";_tag_dyneither(_tmpD9,sizeof(char),16);}),_tag_dyneither(_tmpD8,sizeof(void*),0));});goto _LLB7;case 3: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD4)->f1)->tag == 1){_LLBC: _tmpD5=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD4)->f1)->f1;_LLBD:
# 585
({void*_tmpDA=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpDB="Function_mod(";_tag_dyneither(_tmpDB,sizeof(char),14);}),_tag_dyneither(_tmpDA,sizeof(void*),0));});
for(0;_tmpD5 != 0;_tmpD5=_tmpD5->tl){
struct _dyneither_ptr*_tmpDC=(*((struct _tuple8*)_tmpD5->hd)).f1;
if(_tmpDC == 0)({void*_tmpDD=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpDE="?";_tag_dyneither(_tmpDE,sizeof(char),2);}),_tag_dyneither(_tmpDD,sizeof(void*),0));});else{
({void*_tmpDF=0;Cyc_fprintf(Cyc_stderr,*_tmpDC,_tag_dyneither(_tmpDF,sizeof(void*),0));});}
if(_tmpD5->tl != 0)({void*_tmpE0=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpE1=",";_tag_dyneither(_tmpE1,sizeof(char),2);}),_tag_dyneither(_tmpE0,sizeof(void*),0));});}
# 592
({void*_tmpE2=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpE3=") ";_tag_dyneither(_tmpE3,sizeof(char),3);}),_tag_dyneither(_tmpE2,sizeof(void*),0));});
goto _LLB7;}else{_LLBE: _LLBF:
# 595
({void*_tmpE4=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpE5="Function_mod()";_tag_dyneither(_tmpE5,sizeof(char),15);}),_tag_dyneither(_tmpE4,sizeof(void*),0));});goto _LLB7;}case 5: _LLC0: _LLC1:
({void*_tmpE6=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpE7="Attributes_mod ";_tag_dyneither(_tmpE7,sizeof(char),16);}),_tag_dyneither(_tmpE6,sizeof(void*),0));});goto _LLB7;case 4: _LLC2: _LLC3:
({void*_tmpE8=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpE9="TypeParams_mod ";_tag_dyneither(_tmpE9,sizeof(char),16);}),_tag_dyneither(_tmpE8,sizeof(void*),0));});goto _LLB7;default: _LLC4: _LLC5:
({void*_tmpEA=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpEB="Pointer_mod ";_tag_dyneither(_tmpEB,sizeof(char),13);}),_tag_dyneither(_tmpEA,sizeof(void*),0));});goto _LLB7;}_LLB7:;}
# 600
({void*_tmpEC=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpED="\n";_tag_dyneither(_tmpED,sizeof(char),2);}),_tag_dyneither(_tmpEC,sizeof(void*),0));});}
# 603
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 605
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmp129[3];_tmp129[2]=Cyc_PP_text(({const char*_tmp12B=")";_tag_dyneither(_tmp12B,sizeof(char),2);}));_tmp129[1]=rest;_tmp129[0]=Cyc_PP_text(({const char*_tmp12A="(";_tag_dyneither(_tmp12A,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp129,sizeof(struct Cyc_PP_Doc*),3));});
void*_tmpEE=(void*)tms->hd;void*_tmpEF=_tmpEE;void*_tmpF0;union Cyc_Absyn_Constraint*_tmpF1;union Cyc_Absyn_Constraint*_tmpF2;union Cyc_Absyn_Constraint*_tmpF3;struct Cyc_Absyn_Tqual _tmpF4;struct Cyc_List_List*_tmpF5;unsigned int _tmpF6;int _tmpF7;struct Cyc_List_List*_tmpF8;void*_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;union Cyc_Absyn_Constraint*_tmpFB;union Cyc_Absyn_Constraint*_tmpFC;switch(*((int*)_tmpEF)){case 0: _LLC7: _tmpFC=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_LLC8:
# 611
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmpFD[2];_tmpFD[1]=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFC)?
Cyc_PP_text(({const char*_tmpFE="[]@zeroterm";_tag_dyneither(_tmpFE,sizeof(char),12);})): Cyc_PP_text(({const char*_tmpFF="[]";_tag_dyneither(_tmpFF,sizeof(char),3);}));_tmpFD[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmpFD,sizeof(struct Cyc_PP_Doc*),2));});case 1: _LLC9: _tmpFA=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_tmpFB=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_LLCA:
# 615
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmp100[4];_tmp100[3]=
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFB)?Cyc_PP_text(({const char*_tmp102="]@zeroterm";_tag_dyneither(_tmp102,sizeof(char),11);})): Cyc_PP_text(({const char*_tmp103="]";_tag_dyneither(_tmp103,sizeof(char),2);}));_tmp100[2]=
# 616
Cyc_Absynpp_exp2doc(_tmpFA);_tmp100[1]=Cyc_PP_text(({const char*_tmp101="[";_tag_dyneither(_tmp101,sizeof(char),2);}));_tmp100[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmp100,sizeof(struct Cyc_PP_Doc*),4));});case 3: _LLCB: _tmpF9=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_LLCC:
# 619
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmp104=_tmpF9;struct Cyc_List_List*_tmp105;unsigned int _tmp106;struct Cyc_List_List*_tmp107;int _tmp108;struct Cyc_Absyn_VarargInfo*_tmp109;void*_tmp10A;struct Cyc_List_List*_tmp10B;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10D;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp104)->tag == 1){_LLD4: _tmp107=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp104)->f1;_tmp108=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp104)->f2;_tmp109=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp104)->f3;_tmp10A=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp104)->f4;_tmp10B=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp104)->f5;_tmp10C=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp104)->f6;_tmp10D=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp104)->f7;_LLD5:
# 622
 return({struct Cyc_PP_Doc*_tmp10E[2];_tmp10E[1]=Cyc_Absynpp_funargs2doc(_tmp107,_tmp108,_tmp109,_tmp10A,_tmp10B,_tmp10C,_tmp10D);_tmp10E[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmp10E,sizeof(struct Cyc_PP_Doc*),2));});}else{_LLD6: _tmp105=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp104)->f1;_tmp106=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp104)->f2;_LLD7:
# 624
 return({struct Cyc_PP_Doc*_tmp10F[2];_tmp10F[1]=Cyc_PP_group(({const char*_tmp110="(";_tag_dyneither(_tmp110,sizeof(char),2);}),({const char*_tmp111=")";_tag_dyneither(_tmp111,sizeof(char),2);}),({const char*_tmp112=",";_tag_dyneither(_tmp112,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp105));_tmp10F[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmp10F,sizeof(struct Cyc_PP_Doc*),2));});}_LLD3:;};case 5: _LLCD: _tmpF8=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_LLCE: {
# 627
enum Cyc_Cyclone_C_Compilers _tmp113=Cyc_Cyclone_c_compiler;if(_tmp113 == Cyc_Cyclone_Gcc_c){_LLD9: _LLDA:
# 629
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmp114[2];_tmp114[1]=Cyc_Absynpp_atts2doc(_tmpF8);_tmp114[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmp114,sizeof(struct Cyc_PP_Doc*),2));});}else{_LLDB: _LLDC:
# 632
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
return({struct Cyc_PP_Doc*_tmp115[2];_tmp115[1]=rest;_tmp115[0]=Cyc_Absynpp_callconv2doc(_tmpF8);Cyc_PP_cat(_tag_dyneither(_tmp115,sizeof(struct Cyc_PP_Doc*),2));});
return rest;}_LLD8:;}case 4: _LLCF: _tmpF5=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_tmpF6=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_tmpF7=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEF)->f3;_LLD0:
# 637
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmpF7)
return({struct Cyc_PP_Doc*_tmp116[2];_tmp116[1]=Cyc_Absynpp_ktvars2doc(_tmpF5);_tmp116[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmp116,sizeof(struct Cyc_PP_Doc*),2));});else{
# 641
return({struct Cyc_PP_Doc*_tmp117[2];_tmp117[1]=Cyc_Absynpp_tvars2doc(_tmpF5);_tmp117[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmp117,sizeof(struct Cyc_PP_Doc*),2));});}default: _LLD1: _tmpF0=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).rgn;_tmpF1=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).nullable;_tmpF2=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).bounds;_tmpF3=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).zero_term;_tmpF4=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_LLD2: {
# 645
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(_tmpF4);
{void*_tmp118=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpF2);void*_tmp119=_tmp118;struct Cyc_Absyn_Exp*_tmp11A;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp119)->tag == 0){_LLDE: _LLDF:
 ptr=Cyc_Absynpp_question();goto _LLDD;}else{_LLE0: _tmp11A=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp119)->f1;_LLE1:
# 653
 ptr=Cyc_PP_text(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpF1)?({const char*_tmp11B="*";_tag_dyneither(_tmp11B,sizeof(char),2);}):({const char*_tmp11C="@";_tag_dyneither(_tmp11C,sizeof(char),2);}));{
struct _tuple11 _tmp11D=Cyc_Evexp_eval_const_uint_exp(_tmp11A);struct _tuple11 _tmp11E=_tmp11D;unsigned int _tmp11F;int _tmp120;_LLE3: _tmp11F=_tmp11E.f1;_tmp120=_tmp11E.f2;_LLE4:;
if(!_tmp120  || _tmp11F != 1)
ptr=({struct Cyc_PP_Doc*_tmp121[4];_tmp121[3]=Cyc_Absynpp_rb();_tmp121[2]=Cyc_Absynpp_exp2doc(_tmp11A);_tmp121[1]=Cyc_Absynpp_lb();_tmp121[0]=ptr;Cyc_PP_cat(_tag_dyneither(_tmp121,sizeof(struct Cyc_PP_Doc*),4));});
goto _LLDD;};}_LLDD:;}
# 659
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF3))
ztd=Cyc_PP_text(({const char*_tmp122="@zeroterm";_tag_dyneither(_tmp122,sizeof(char),10);}));else{
if(is_char_ptr  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF3))
ztd=Cyc_PP_text(({const char*_tmp123="@nozeroterm";_tag_dyneither(_tmp123,sizeof(char),12);}));}}
# 665
{void*_tmp124=Cyc_Tcutil_compress(_tmpF0);void*_tmp125=_tmp124;switch(*((int*)_tmp125)){case 20: _LLE6: _LLE7:
 goto _LLE5;case 1: _LLE8: if(Cyc_Absynpp_print_for_cycdoc){_LLE9:
 goto _LLE5;}else{goto _LLEA;}default: _LLEA: _LLEB:
 rgd=Cyc_Absynpp_rgn2doc(_tmpF0);}_LLE5:;}{
# 670
struct Cyc_PP_Doc*spacer1=tqd != mt  && (ztd != mt  || rgd != mt)?Cyc_PP_text(({const char*_tmp128=" ";_tag_dyneither(_tmp128,sizeof(char),2);})): mt;
struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(({const char*_tmp127=" ";_tag_dyneither(_tmp127,sizeof(char),2);})): mt;
return({struct Cyc_PP_Doc*_tmp126[7];_tmp126[6]=rest;_tmp126[5]=spacer2;_tmp126[4]=tqd;_tmp126[3]=spacer1;_tmp126[2]=rgd;_tmp126[1]=ztd;_tmp126[0]=ptr;Cyc_PP_cat(_tag_dyneither(_tmp126,sizeof(struct Cyc_PP_Doc*),7));});};}}_LLC6:;};}
# 676
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp12C=Cyc_Tcutil_compress(t);void*_tmp12D=_tmp12C;switch(*((int*)_tmp12D)){case 20: _LLED: _LLEE:
 return Cyc_PP_text(({const char*_tmp12E="`H";_tag_dyneither(_tmp12E,sizeof(char),3);}));case 21: _LLEF: _LLF0:
 return Cyc_PP_text(({const char*_tmp12F="`U";_tag_dyneither(_tmp12F,sizeof(char),3);}));case 22: _LLF1: _LLF2:
 return Cyc_PP_text(({const char*_tmp130="`RC";_tag_dyneither(_tmp130,sizeof(char),4);}));default: _LLF3: _LLF4:
 return Cyc_Absynpp_ntyp2doc(t);}_LLEC:;}
# 686
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 690
void*_tmp131=Cyc_Tcutil_compress(t);void*_tmp132=_tmp131;struct Cyc_List_List*_tmp133;void*_tmp134;switch(*((int*)_tmp132)){case 23: _LLF6: _tmp134=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp132)->f1;_LLF7:
*rgions=({struct Cyc_List_List*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->hd=Cyc_Absynpp_rgn2doc(_tmp134);_tmp135->tl=*rgions;_tmp135;});goto _LLF5;case 24: _LLF8: _tmp133=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp132)->f1;_LLF9:
# 693
 for(0;_tmp133 != 0;_tmp133=_tmp133->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp133->hd);}
# 696
goto _LLF5;default: _LLFA: _LLFB:
*effects=({struct Cyc_List_List*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->hd=Cyc_Absynpp_typ2doc(t);_tmp136->tl=*effects;_tmp136;});goto _LLF5;}_LLF5:;}
# 701
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0  && effects != 0)
return Cyc_PP_group(({const char*_tmp137="";_tag_dyneither(_tmp137,sizeof(char),1);}),({const char*_tmp138="";_tag_dyneither(_tmp138,sizeof(char),1);}),({const char*_tmp139="+";_tag_dyneither(_tmp139,sizeof(char),2);}),effects);else{
# 709
struct Cyc_PP_Doc*_tmp13A=Cyc_Absynpp_group_braces(({const char*_tmp13F=",";_tag_dyneither(_tmp13F,sizeof(char),2);}),rgions);
return Cyc_PP_group(({const char*_tmp13B="";_tag_dyneither(_tmp13B,sizeof(char),1);}),({const char*_tmp13C="";_tag_dyneither(_tmp13C,sizeof(char),1);}),({const char*_tmp13D="+";_tag_dyneither(_tmp13D,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,({struct Cyc_List_List*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E->hd=_tmp13A;_tmp13E->tl=0;_tmp13E;})));}}
# 714
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp140=k;if(_tmp140 == Cyc_Absyn_StructA){_LLFD: _LLFE:
 return Cyc_PP_text(({const char*_tmp141="struct ";_tag_dyneither(_tmp141,sizeof(char),8);}));}else{_LLFF: _LL100:
 return Cyc_PP_text(({const char*_tmp142="union ";_tag_dyneither(_tmp142,sizeof(char),7);}));}_LLFC:;}
# 722
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp143=t;struct Cyc_Absyn_Datatypedecl*_tmp144;struct Cyc_Absyn_Enumdecl*_tmp145;struct Cyc_Absyn_Aggrdecl*_tmp146;void*_tmp147;void*_tmp148;void*_tmp149;void*_tmp14A;void*_tmp14B;struct _tuple0*_tmp14C;struct Cyc_List_List*_tmp14D;struct Cyc_Absyn_Typedefdecl*_tmp14E;struct _dyneither_ptr _tmp14F;struct Cyc_Absyn_Exp*_tmp150;struct Cyc_Absyn_Exp*_tmp151;struct _tuple0*_tmp152;struct Cyc_List_List*_tmp153;enum Cyc_Absyn_AggrKind _tmp154;struct Cyc_List_List*_tmp155;union Cyc_Absyn_AggrInfoU _tmp156;struct Cyc_List_List*_tmp157;struct Cyc_List_List*_tmp158;int _tmp159;enum Cyc_Absyn_Sign _tmp15A;enum Cyc_Absyn_Size_of _tmp15B;union Cyc_Absyn_DatatypeFieldInfoU _tmp15C;struct Cyc_List_List*_tmp15D;union Cyc_Absyn_DatatypeInfoU _tmp15E;struct Cyc_List_List*_tmp15F;struct Cyc_Absyn_Tvar*_tmp160;struct Cyc_Core_Opt*_tmp161;void*_tmp162;int _tmp163;struct Cyc_Core_Opt*_tmp164;switch(*((int*)_tmp143)){case 8: _LL102: _LL103:
# 726
 return Cyc_PP_text(({const char*_tmp165="[[[array]]]";_tag_dyneither(_tmp165,sizeof(char),12);}));case 9: _LL104: _LL105:
 return Cyc_PP_nil_doc();case 5: _LL106: _LL107:
 return Cyc_PP_nil_doc();case 0: _LL108: _LL109:
# 730
 s=Cyc_PP_text(({const char*_tmp166="void";_tag_dyneither(_tmp166,sizeof(char),5);}));goto _LL101;case 1: _LL10A: _tmp161=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f1;_tmp162=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f2;_tmp163=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f3;_tmp164=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f4;_LL10B:
# 732
 if(_tmp162 != 0)
# 734
return Cyc_Absynpp_ntyp2doc(_tmp162);else{
# 736
struct _dyneither_ptr kindstring=_tmp161 == 0?({const char*_tmp16B="K";_tag_dyneither(_tmp16B,sizeof(char),2);}): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)_tmp161->v);
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp16A;_tmp16A.tag=1;_tmp16A.f1=(unsigned long)_tmp163;({struct Cyc_String_pa_PrintArg_struct _tmp169;_tmp169.tag=0;_tmp169.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kindstring);({void*_tmp167[2]={& _tmp169,& _tmp16A};Cyc_aprintf(({const char*_tmp168="`E%s%d";_tag_dyneither(_tmp168,sizeof(char),7);}),_tag_dyneither(_tmp167,sizeof(void*),2));});});}));}
# 753 "absynpp.cyc"
goto _LL101;case 2: _LL10C: _tmp160=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp143)->f1;_LL10D:
# 755
 s=Cyc_Absynpp_tvar2doc(_tmp160);
if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_tmp16C[3];_tmp16C[2]=Cyc_Absynpp_kindbound2doc(_tmp160->kind);_tmp16C[1]=Cyc_PP_text(({const char*_tmp16D="::";_tag_dyneither(_tmp16D,sizeof(char),3);}));_tmp16C[0]=s;Cyc_PP_cat(_tag_dyneither(_tmp16C,sizeof(struct Cyc_PP_Doc*),3));});
# 759
goto _LL101;case 3: _LL10E: _tmp15E=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp143)->f1).datatype_info;_tmp15F=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp143)->f1).targs;_LL10F:
# 761
{union Cyc_Absyn_DatatypeInfoU _tmp16E=_tmp15E;struct _tuple0*_tmp16F;int _tmp170;struct _tuple0*_tmp171;int _tmp172;if((_tmp16E.UnknownDatatype).tag == 1){_LL141: _tmp171=((_tmp16E.UnknownDatatype).val).name;_tmp172=((_tmp16E.UnknownDatatype).val).is_extensible;_LL142:
# 763
 _tmp16F=_tmp171;_tmp170=_tmp172;goto _LL144;}else{_LL143: _tmp16F=(*(_tmp16E.KnownDatatype).val)->name;_tmp170=(*(_tmp16E.KnownDatatype).val)->is_extensible;_LL144: {
# 765
struct Cyc_PP_Doc*_tmp173=Cyc_PP_text(({const char*_tmp177="datatype ";_tag_dyneither(_tmp177,sizeof(char),10);}));
struct Cyc_PP_Doc*_tmp174=_tmp170?Cyc_PP_text(({const char*_tmp176="@extensible ";_tag_dyneither(_tmp176,sizeof(char),13);})): Cyc_PP_nil_doc();
s=({struct Cyc_PP_Doc*_tmp175[4];_tmp175[3]=Cyc_Absynpp_tps2doc(_tmp15F);_tmp175[2]=Cyc_Absynpp_qvar2doc(_tmp16F);_tmp175[1]=_tmp173;_tmp175[0]=_tmp174;Cyc_PP_cat(_tag_dyneither(_tmp175,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL140;}}_LL140:;}
# 770
goto _LL101;case 4: _LL110: _tmp15C=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp143)->f1).field_info;_tmp15D=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp143)->f1).targs;_LL111:
# 772
{union Cyc_Absyn_DatatypeFieldInfoU _tmp178=_tmp15C;struct _tuple0*_tmp179;int _tmp17A;struct _tuple0*_tmp17B;struct _tuple0*_tmp17C;struct _tuple0*_tmp17D;int _tmp17E;if((_tmp178.UnknownDatatypefield).tag == 1){_LL146: _tmp17C=((_tmp178.UnknownDatatypefield).val).datatype_name;_tmp17D=((_tmp178.UnknownDatatypefield).val).field_name;_tmp17E=((_tmp178.UnknownDatatypefield).val).is_extensible;_LL147:
# 774
 _tmp179=_tmp17C;_tmp17A=_tmp17E;_tmp17B=_tmp17D;goto _LL149;}else{_LL148: _tmp179=(((_tmp178.KnownDatatypefield).val).f1)->name;_tmp17A=(((_tmp178.KnownDatatypefield).val).f1)->is_extensible;_tmp17B=(((_tmp178.KnownDatatypefield).val).f2)->name;_LL149: {
# 777
struct Cyc_PP_Doc*_tmp17F=Cyc_PP_text(_tmp17A?({const char*_tmp182="@extensible datatype ";_tag_dyneither(_tmp182,sizeof(char),22);}):({const char*_tmp183="datatype ";_tag_dyneither(_tmp183,sizeof(char),10);}));
s=({struct Cyc_PP_Doc*_tmp180[4];_tmp180[3]=Cyc_Absynpp_qvar2doc(_tmp17B);_tmp180[2]=Cyc_PP_text(({const char*_tmp181=".";_tag_dyneither(_tmp181,sizeof(char),2);}));_tmp180[1]=Cyc_Absynpp_qvar2doc(_tmp179);_tmp180[0]=_tmp17F;Cyc_PP_cat(_tag_dyneither(_tmp180,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL145;}}_LL145:;}
# 781
goto _LL101;case 6: _LL112: _tmp15A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp143)->f1;_tmp15B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp143)->f2;_LL113: {
# 783
struct _dyneither_ptr sns;
struct _dyneither_ptr ts;
{enum Cyc_Absyn_Sign _tmp184=_tmp15A;switch(_tmp184){case Cyc_Absyn_None: _LL14B: _LL14C:
 goto _LL14E;case Cyc_Absyn_Signed: _LL14D: _LL14E:
 sns=({const char*_tmp185="";_tag_dyneither(_tmp185,sizeof(char),1);});goto _LL14A;default: _LL14F: _LL150:
 sns=({const char*_tmp186="unsigned ";_tag_dyneither(_tmp186,sizeof(char),10);});goto _LL14A;}_LL14A:;}
# 790
{enum Cyc_Absyn_Size_of _tmp187=_tmp15B;switch(_tmp187){case Cyc_Absyn_Char_sz: _LL152: _LL153:
# 792
{enum Cyc_Absyn_Sign _tmp188=_tmp15A;switch(_tmp188){case Cyc_Absyn_None: _LL15D: _LL15E:
 sns=({const char*_tmp189="";_tag_dyneither(_tmp189,sizeof(char),1);});goto _LL15C;case Cyc_Absyn_Signed: _LL15F: _LL160:
 sns=({const char*_tmp18A="signed ";_tag_dyneither(_tmp18A,sizeof(char),8);});goto _LL15C;default: _LL161: _LL162:
 sns=({const char*_tmp18B="unsigned ";_tag_dyneither(_tmp18B,sizeof(char),10);});goto _LL15C;}_LL15C:;}
# 797
ts=({const char*_tmp18C="char";_tag_dyneither(_tmp18C,sizeof(char),5);});
goto _LL151;case Cyc_Absyn_Short_sz: _LL154: _LL155:
 ts=({const char*_tmp18D="short";_tag_dyneither(_tmp18D,sizeof(char),6);});goto _LL151;case Cyc_Absyn_Int_sz: _LL156: _LL157:
 ts=({const char*_tmp18E="int";_tag_dyneither(_tmp18E,sizeof(char),4);});goto _LL151;case Cyc_Absyn_Long_sz: _LL158: _LL159:
 ts=({const char*_tmp18F="long";_tag_dyneither(_tmp18F,sizeof(char),5);});goto _LL151;default: _LL15A: _LL15B:
# 803
{enum Cyc_Cyclone_C_Compilers _tmp190=Cyc_Cyclone_c_compiler;if(_tmp190 == Cyc_Cyclone_Gcc_c){_LL164: _LL165:
 ts=({const char*_tmp191="long long";_tag_dyneither(_tmp191,sizeof(char),10);});goto _LL163;}else{_LL166: _LL167:
 ts=({const char*_tmp192="__int64";_tag_dyneither(_tmp192,sizeof(char),8);});goto _LL163;}_LL163:;}
# 807
goto _LL151;}_LL151:;}
# 809
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp196;_tmp196.tag=0;_tmp196.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts);({struct Cyc_String_pa_PrintArg_struct _tmp195;_tmp195.tag=0;_tmp195.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns);({void*_tmp193[2]={& _tmp195,& _tmp196};Cyc_aprintf(({const char*_tmp194="%s%s";_tag_dyneither(_tmp194,sizeof(char),5);}),_tag_dyneither(_tmp193,sizeof(void*),2));});});}));
goto _LL101;}case 7: _LL114: _tmp159=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp143)->f1;_LL115:
# 812
 if(_tmp159 == 0)
s=Cyc_PP_text(({const char*_tmp197="float";_tag_dyneither(_tmp197,sizeof(char),6);}));else{
if(_tmp159 == 1)
s=Cyc_PP_text(({const char*_tmp198="double";_tag_dyneither(_tmp198,sizeof(char),7);}));else{
# 817
s=Cyc_PP_text(({const char*_tmp199="long double";_tag_dyneither(_tmp199,sizeof(char),12);}));}}
goto _LL101;case 10: _LL116: _tmp158=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp143)->f1;_LL117:
# 820
 s=({struct Cyc_PP_Doc*_tmp19A[2];_tmp19A[1]=Cyc_Absynpp_args2doc(_tmp158);_tmp19A[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dyneither(_tmp19A,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL101;case 11: _LL118: _tmp156=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp143)->f1).aggr_info;_tmp157=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp143)->f1).targs;_LL119: {
# 823
struct _tuple10 _tmp19B=Cyc_Absyn_aggr_kinded_name(_tmp156);struct _tuple10 _tmp19C=_tmp19B;enum Cyc_Absyn_AggrKind _tmp19D;struct _tuple0*_tmp19E;_LL169: _tmp19D=_tmp19C.f1;_tmp19E=_tmp19C.f2;_LL16A:;
s=({struct Cyc_PP_Doc*_tmp19F[3];_tmp19F[2]=Cyc_Absynpp_tps2doc(_tmp157);_tmp19F[1]=Cyc_Absynpp_qvar2doc(_tmp19E);_tmp19F[0]=Cyc_Absynpp_aggr_kind2doc(_tmp19D);Cyc_PP_cat(_tag_dyneither(_tmp19F,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL101;}case 12: _LL11A: _tmp154=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp143)->f1;_tmp155=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp143)->f2;_LL11B:
# 827
 s=({struct Cyc_PP_Doc*_tmp1A0[4];_tmp1A0[3]=
# 829
Cyc_Absynpp_rb();_tmp1A0[2]=
# 828
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp155));_tmp1A0[1]=
# 827
Cyc_Absynpp_lb();_tmp1A0[0]=Cyc_Absynpp_aggr_kind2doc(_tmp154);Cyc_PP_cat(_tag_dyneither(_tmp1A0,sizeof(struct Cyc_PP_Doc*),4));});
# 830
goto _LL101;case 14: _LL11C: _tmp153=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp143)->f1;_LL11D:
# 832
 s=({struct Cyc_PP_Doc*_tmp1A1[4];_tmp1A1[3]=Cyc_Absynpp_rb();_tmp1A1[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp153));_tmp1A1[1]=Cyc_Absynpp_lb();_tmp1A1[0]=Cyc_PP_text(({const char*_tmp1A2="enum ";_tag_dyneither(_tmp1A2,sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp1A1,sizeof(struct Cyc_PP_Doc*),4));});goto _LL101;case 13: _LL11E: _tmp152=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp143)->f1;_LL11F:
# 834
 s=({struct Cyc_PP_Doc*_tmp1A3[2];_tmp1A3[1]=Cyc_Absynpp_qvar2doc(_tmp152);_tmp1A3[0]=Cyc_PP_text(({const char*_tmp1A4="enum ";_tag_dyneither(_tmp1A4,sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp1A3,sizeof(struct Cyc_PP_Doc*),2));});goto _LL101;case 18: _LL120: _tmp151=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp143)->f1;_LL121:
# 836
 s=({struct Cyc_PP_Doc*_tmp1A5[3];_tmp1A5[2]=Cyc_PP_text(({const char*_tmp1A7=")";_tag_dyneither(_tmp1A7,sizeof(char),2);}));_tmp1A5[1]=Cyc_Absynpp_exp2doc(_tmp151);_tmp1A5[0]=Cyc_PP_text(({const char*_tmp1A6="valueof_t(";_tag_dyneither(_tmp1A6,sizeof(char),11);}));Cyc_PP_cat(_tag_dyneither(_tmp1A5,sizeof(struct Cyc_PP_Doc*),3));});goto _LL101;case 27: _LL122: _tmp150=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp143)->f1;_LL123:
# 838
 s=({struct Cyc_PP_Doc*_tmp1A8[3];_tmp1A8[2]=Cyc_PP_text(({const char*_tmp1AA=")";_tag_dyneither(_tmp1AA,sizeof(char),2);}));_tmp1A8[1]=Cyc_Absynpp_exp2doc(_tmp150);_tmp1A8[0]=Cyc_PP_text(({const char*_tmp1A9="typeof(";_tag_dyneither(_tmp1A9,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp1A8,sizeof(struct Cyc_PP_Doc*),3));});goto _LL101;case 28: _LL124: _tmp14F=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp143)->f1;_LL125:
# 840
 s=Cyc_PP_text(_tmp14F);goto _LL101;case 17: _LL126: _tmp14C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f1;_tmp14D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f2;_tmp14E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f3;_LL127:
# 847
 s=({struct Cyc_PP_Doc*_tmp1AB[2];_tmp1AB[1]=Cyc_Absynpp_tps2doc(_tmp14D);_tmp1AB[0]=Cyc_Absynpp_qvar2doc(_tmp14C);Cyc_PP_cat(_tag_dyneither(_tmp1AB,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL101;case 15: _LL128: _tmp14B=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp143)->f1;_LL129:
# 850
 s=({struct Cyc_PP_Doc*_tmp1AC[3];_tmp1AC[2]=Cyc_PP_text(({const char*_tmp1AE=">";_tag_dyneither(_tmp1AE,sizeof(char),2);}));_tmp1AC[1]=Cyc_Absynpp_rgn2doc(_tmp14B);_tmp1AC[0]=Cyc_PP_text(({const char*_tmp1AD="region_t<";_tag_dyneither(_tmp1AD,sizeof(char),10);}));Cyc_PP_cat(_tag_dyneither(_tmp1AC,sizeof(struct Cyc_PP_Doc*),3));});goto _LL101;case 16: _LL12A: _tmp149=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp143)->f1;_tmp14A=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp143)->f2;_LL12B:
# 852
 s=({struct Cyc_PP_Doc*_tmp1AF[5];_tmp1AF[4]=
Cyc_PP_text(({const char*_tmp1B2=">";_tag_dyneither(_tmp1B2,sizeof(char),2);}));_tmp1AF[3]=
# 852
Cyc_Absynpp_rgn2doc(_tmp14A);_tmp1AF[2]=Cyc_PP_text(({const char*_tmp1B1=",";_tag_dyneither(_tmp1B1,sizeof(char),2);}));_tmp1AF[1]=Cyc_Absynpp_rgn2doc(_tmp149);_tmp1AF[0]=Cyc_PP_text(({const char*_tmp1B0="dynregion_t<";_tag_dyneither(_tmp1B0,sizeof(char),13);}));Cyc_PP_cat(_tag_dyneither(_tmp1AF,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL101;case 19: _LL12C: _tmp148=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp143)->f1;_LL12D:
# 855
 s=({struct Cyc_PP_Doc*_tmp1B3[3];_tmp1B3[2]=Cyc_PP_text(({const char*_tmp1B5=">";_tag_dyneither(_tmp1B5,sizeof(char),2);}));_tmp1B3[1]=Cyc_Absynpp_typ2doc(_tmp148);_tmp1B3[0]=Cyc_PP_text(({const char*_tmp1B4="tag_t<";_tag_dyneither(_tmp1B4,sizeof(char),7);}));Cyc_PP_cat(_tag_dyneither(_tmp1B3,sizeof(struct Cyc_PP_Doc*),3));});goto _LL101;case 21: _LL12E: _LL12F:
 goto _LL131;case 20: _LL130: _LL131:
 goto _LL133;case 22: _LL132: _LL133:
# 859
 s=Cyc_Absynpp_rgn2doc(t);goto _LL101;case 25: _LL134: _tmp147=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp143)->f1;_LL135:
# 861
 s=({struct Cyc_PP_Doc*_tmp1B6[3];_tmp1B6[2]=Cyc_PP_text(({const char*_tmp1B8=")";_tag_dyneither(_tmp1B8,sizeof(char),2);}));_tmp1B6[1]=Cyc_Absynpp_typ2doc(_tmp147);_tmp1B6[0]=Cyc_PP_text(({const char*_tmp1B7="regions(";_tag_dyneither(_tmp1B7,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp1B6,sizeof(struct Cyc_PP_Doc*),3));});goto _LL101;case 23: _LL136: _LL137:
 goto _LL139;case 24: _LL138: _LL139:
# 864
 s=Cyc_Absynpp_eff2doc(t);goto _LL101;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)){case 0: _LL13A: _tmp146=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_LL13B:
# 866
 s=Cyc_Absynpp_aggrdecl2doc(_tmp146);goto _LL101;case 1: _LL13C: _tmp145=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_LL13D:
# 868
 s=Cyc_Absynpp_enumdecl2doc(_tmp145);goto _LL101;default: _LL13E: _tmp144=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_LL13F:
# 870
 s=Cyc_Absynpp_datatypedecl2doc(_tmp144);goto _LL101;}}_LL101:;}
# 872
return s;}
# 875
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _dyneither_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple16{void*f1;void*f2;};
# 879
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
struct _tuple16*_tmp1B9=cmp;void*_tmp1BA;void*_tmp1BB;_LL16C: _tmp1BA=_tmp1B9->f1;_tmp1BB=_tmp1B9->f2;_LL16D:;
return({struct Cyc_PP_Doc*_tmp1BC[3];_tmp1BC[2]=Cyc_Absynpp_rgn2doc(_tmp1BB);_tmp1BC[1]=Cyc_PP_text(({const char*_tmp1BD=" > ";_tag_dyneither(_tmp1BD,sizeof(char),4);}));_tmp1BC[0]=Cyc_Absynpp_rgn2doc(_tmp1BA);Cyc_PP_cat(_tag_dyneither(_tmp1BC,sizeof(struct Cyc_PP_Doc*),3));});}
# 884
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
return Cyc_PP_group(({const char*_tmp1BE="";_tag_dyneither(_tmp1BE,sizeof(char),1);}),({const char*_tmp1BF="";_tag_dyneither(_tmp1BF,sizeof(char),1);}),({const char*_tmp1C0=",";_tag_dyneither(_tmp1C0,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
# 888
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _dyneither_ptr*_tmp1C1=(*t).f1;
struct Cyc_Core_Opt*dopt=_tmp1C1 == 0?0:({struct Cyc_Core_Opt*_tmp1C2=_cycalloc(sizeof(*_tmp1C2));_tmp1C2->v=Cyc_PP_text(*_tmp1C1);_tmp1C2;});
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 894
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 898
struct Cyc_List_List*_tmp1C3=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs)
_tmp1C3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1C3,({struct Cyc_List_List*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->hd=Cyc_PP_text(({const char*_tmp1C5="...";_tag_dyneither(_tmp1C5,sizeof(char),4);}));_tmp1C4->tl=0;_tmp1C4;}));else{
if(cyc_varargs != 0){
struct Cyc_PP_Doc*_tmp1C6=({struct Cyc_PP_Doc*_tmp1C8[3];_tmp1C8[2]=
# 905
Cyc_Absynpp_funarg2doc(({struct _tuple8*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->f1=cyc_varargs->name;_tmp1CC->f2=cyc_varargs->tq;_tmp1CC->f3=cyc_varargs->type;_tmp1CC;}));_tmp1C8[1]=
# 904
cyc_varargs->inject?Cyc_PP_text(({const char*_tmp1CA=" inject ";_tag_dyneither(_tmp1CA,sizeof(char),9);})): Cyc_PP_text(({const char*_tmp1CB=" ";_tag_dyneither(_tmp1CB,sizeof(char),2);}));_tmp1C8[0]=
# 903
Cyc_PP_text(({const char*_tmp1C9="...";_tag_dyneither(_tmp1C9,sizeof(char),4);}));Cyc_PP_cat(_tag_dyneither(_tmp1C8,sizeof(struct Cyc_PP_Doc*),3));});
# 907
_tmp1C3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1C3,({struct Cyc_List_List*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7->hd=_tmp1C6;_tmp1C7->tl=0;_tmp1C7;}));}}{
# 909
struct Cyc_PP_Doc*_tmp1CD=Cyc_PP_group(({const char*_tmp1DC="";_tag_dyneither(_tmp1DC,sizeof(char),1);}),({const char*_tmp1DD="";_tag_dyneither(_tmp1DD,sizeof(char),1);}),({const char*_tmp1DE=",";_tag_dyneither(_tmp1DE,sizeof(char),2);}),_tmp1C3);
if(effopt != 0  && Cyc_Absynpp_print_all_effects)
_tmp1CD=({struct Cyc_PP_Doc*_tmp1CE[3];_tmp1CE[2]=Cyc_Absynpp_eff2doc(effopt);_tmp1CE[1]=Cyc_PP_text(({const char*_tmp1CF=";";_tag_dyneither(_tmp1CF,sizeof(char),2);}));_tmp1CE[0]=_tmp1CD;Cyc_PP_cat(_tag_dyneither(_tmp1CE,sizeof(struct Cyc_PP_Doc*),3));});
if(rgn_po != 0)
_tmp1CD=({struct Cyc_PP_Doc*_tmp1D0[3];_tmp1D0[2]=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp1D0[1]=Cyc_PP_text(({const char*_tmp1D1=":";_tag_dyneither(_tmp1D1,sizeof(char),2);}));_tmp1D0[0]=_tmp1CD;Cyc_PP_cat(_tag_dyneither(_tmp1D0,sizeof(struct Cyc_PP_Doc*),3));});{
struct Cyc_PP_Doc*_tmp1D2=({struct Cyc_PP_Doc*_tmp1D9[3];_tmp1D9[2]=Cyc_PP_text(({const char*_tmp1DB=")";_tag_dyneither(_tmp1DB,sizeof(char),2);}));_tmp1D9[1]=_tmp1CD;_tmp1D9[0]=Cyc_PP_text(({const char*_tmp1DA="(";_tag_dyneither(_tmp1DA,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp1D9,sizeof(struct Cyc_PP_Doc*),3));});
if(req != 0)
_tmp1D2=({struct Cyc_PP_Doc*_tmp1D3[4];_tmp1D3[3]=Cyc_PP_text(({const char*_tmp1D5=")";_tag_dyneither(_tmp1D5,sizeof(char),2);}));_tmp1D3[2]=Cyc_Absynpp_exp2doc(req);_tmp1D3[1]=Cyc_PP_text(({const char*_tmp1D4=" @requires(";_tag_dyneither(_tmp1D4,sizeof(char),12);}));_tmp1D3[0]=_tmp1D2;Cyc_PP_cat(_tag_dyneither(_tmp1D3,sizeof(struct Cyc_PP_Doc*),4));});
if(ens != 0)
_tmp1D2=({struct Cyc_PP_Doc*_tmp1D6[4];_tmp1D6[3]=Cyc_PP_text(({const char*_tmp1D8=")";_tag_dyneither(_tmp1D8,sizeof(char),2);}));_tmp1D6[2]=Cyc_Absynpp_exp2doc(ens);_tmp1D6[1]=Cyc_PP_text(({const char*_tmp1D7=" @ensures(";_tag_dyneither(_tmp1D7,sizeof(char),11);}));_tmp1D6[0]=_tmp1D2;Cyc_PP_cat(_tag_dyneither(_tmp1D6,sizeof(struct Cyc_PP_Doc*),4));});
return _tmp1D2;};};}
# 922
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg){
struct _tuple8*_tmp1DF=arg;struct _dyneither_ptr*_tmp1E0;struct Cyc_Absyn_Tqual _tmp1E1;void*_tmp1E2;_LL16F: _tmp1E0=_tmp1DF->f1;_tmp1E1=_tmp1DF->f2;_tmp1E2=_tmp1DF->f3;_LL170:;
return({struct _tuple8*_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3->f1=_tmp1E0;_tmp1E3->f2=_tmp1E1;_tmp1E3->f3=_tmp1E2;_tmp1E3;});}
# 927
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){return Cyc_PP_text(*v);}
# 929
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp1E4=0;
int match;
{union Cyc_Absyn_Nmspace _tmp1E5=(*q).f1;union Cyc_Absyn_Nmspace _tmp1E6=_tmp1E5;struct Cyc_List_List*_tmp1E7;struct Cyc_List_List*_tmp1E8;struct Cyc_List_List*_tmp1E9;switch((_tmp1E6.C_n).tag){case 4: _LL172: _LL173:
# 934
 _tmp1E9=0;goto _LL175;case 1: _LL174: _tmp1E9=(_tmp1E6.Rel_n).val;_LL175:
# 936
 match=0;
_tmp1E4=_tmp1E9;
goto _LL171;case 3: _LL176: _tmp1E8=(_tmp1E6.C_n).val;_LL177:
# 940
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1E8,Cyc_Absynpp_curr_namespace);
# 942
goto _LL171;default: _LL178: _tmp1E7=(_tmp1E6.Abs_n).val;_LL179:
# 944
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1E7,Cyc_Absynpp_curr_namespace);
_tmp1E4=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));_tmp1EA->hd=Cyc_Absynpp_cyc_stringptr;_tmp1EA->tl=_tmp1E7;_tmp1EA;}): _tmp1E7;
goto _LL171;}_LL171:;}
# 948
if(Cyc_Absynpp_qvar_to_Cids)
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1E4,({struct Cyc_List_List*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB->hd=(*q).f2;_tmp1EB->tl=0;_tmp1EB;})),({const char*_tmp1EC="_";_tag_dyneither(_tmp1EC,sizeof(char),2);}));else{
# 953
if(match)
return*(*q).f2;else{
# 956
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1E4,({struct Cyc_List_List*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED->hd=(*q).f2;_tmp1ED->tl=0;_tmp1ED;})),({const char*_tmp1EE="::";_tag_dyneither(_tmp1EE,sizeof(char),3);}));}}}
# 961
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 965
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp1EF=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output)
# 969
return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(({const char*_tmp1F0="\\textbf{";_tag_dyneither(_tmp1F0,sizeof(char),9);}),(struct _dyneither_ptr)_tmp1EF),({const char*_tmp1F1="}";_tag_dyneither(_tmp1F1,sizeof(char),2);})),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp1EF));else{
return Cyc_PP_text(_tmp1EF);}}
# 974
struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 976
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 979
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp1F2=(*v).f1;union Cyc_Absyn_Nmspace _tmp1F3=_tmp1F2;struct Cyc_List_List*_tmp1F4;struct Cyc_List_List*_tmp1F5;switch((_tmp1F3.C_n).tag){case 4: _LL17B: _LL17C:
 goto _LL17E;case 1: if((_tmp1F3.Rel_n).val == 0){_LL17D: _LL17E:
 return*(*v).f2;}else{_LL183: _LL184:
# 990
 return(struct _dyneither_ptr)Cyc_strconcat(({const char*_tmp1F6="/* bad namespace : */ ";_tag_dyneither(_tmp1F6,sizeof(char),23);}),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));}case 3: _LL17F: _tmp1F5=(_tmp1F3.C_n).val;_LL180:
# 983
 _tmp1F4=_tmp1F5;goto _LL182;default: _LL181: _tmp1F4=(_tmp1F3.Abs_n).val;_LL182:
# 985
 if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp1F4,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 988
goto _LL184;}}_LL17A:;}else{
# 993
return*(*v).f2;}}
# 996
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 999
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _dyneither_ptr _tmp1F7=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output)
# 1003
return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(({const char*_tmp1F8="\\textbf{";_tag_dyneither(_tmp1F8,sizeof(char),9);}),(struct _dyneither_ptr)_tmp1F7),({const char*_tmp1F9="}";_tag_dyneither(_tmp1F9,sizeof(char),2);})),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp1F7));else{
return Cyc_PP_text(_tmp1F7);}}
# 1008
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}
# 1019 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp1FA=e->r;void*_tmp1FB=_tmp1FA;struct Cyc_Absyn_Exp*_tmp1FC;struct Cyc_Absyn_Exp*_tmp1FD;enum Cyc_Absyn_Primop _tmp1FE;switch(*((int*)_tmp1FB)){case 0: _LL186: _LL187:
 goto _LL189;case 1: _LL188: _LL189:
 return 10000;case 2: _LL18A: _tmp1FE=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1FB)->f1;_LL18B: {
# 1024
enum Cyc_Absyn_Primop _tmp1FF=_tmp1FE;switch(_tmp1FF){case Cyc_Absyn_Plus: _LL1D7: _LL1D8:
 return 100;case Cyc_Absyn_Times: _LL1D9: _LL1DA:
 return 110;case Cyc_Absyn_Minus: _LL1DB: _LL1DC:
 return 100;case Cyc_Absyn_Div: _LL1DD: _LL1DE:
 goto _LL1E0;case Cyc_Absyn_Mod: _LL1DF: _LL1E0:
 return 110;case Cyc_Absyn_Eq: _LL1E1: _LL1E2:
 goto _LL1E4;case Cyc_Absyn_Neq: _LL1E3: _LL1E4:
 return 70;case Cyc_Absyn_Gt: _LL1E5: _LL1E6:
 goto _LL1E8;case Cyc_Absyn_Lt: _LL1E7: _LL1E8:
 goto _LL1EA;case Cyc_Absyn_Gte: _LL1E9: _LL1EA:
 goto _LL1EC;case Cyc_Absyn_Lte: _LL1EB: _LL1EC:
 return 80;case Cyc_Absyn_Not: _LL1ED: _LL1EE:
 goto _LL1F0;case Cyc_Absyn_Bitnot: _LL1EF: _LL1F0:
 return 130;case Cyc_Absyn_Bitand: _LL1F1: _LL1F2:
 return 60;case Cyc_Absyn_Bitor: _LL1F3: _LL1F4:
 return 40;case Cyc_Absyn_Bitxor: _LL1F5: _LL1F6:
 return 50;case Cyc_Absyn_Bitlshift: _LL1F7: _LL1F8:
 return 90;case Cyc_Absyn_Bitlrshift: _LL1F9: _LL1FA:
 return 80;case Cyc_Absyn_Bitarshift: _LL1FB: _LL1FC:
 return 80;default: _LL1FD: _LL1FE:
 return 140;}_LL1D6:;}case 3: _LL18C: _LL18D:
# 1046
 return 20;case 4: _LL18E: _LL18F:
 return 130;case 5: _LL190: _LL191:
 return 30;case 6: _LL192: _LL193:
 return 35;case 7: _LL194: _LL195:
 return 30;case 8: _LL196: _LL197:
 return 10;case 9: _LL198: _LL199:
 return 140;case 10: _LL19A: _LL19B:
 return 130;case 11: _LL19C: _tmp1FD=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp1FB)->f1;_LL19D:
 return Cyc_Absynpp_exp_prec(_tmp1FD);case 12: _LL19E: _tmp1FC=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp1FB)->f1;_LL19F:
 return Cyc_Absynpp_exp_prec(_tmp1FC);case 13: _LL1A0: _LL1A1:
 return 120;case 15: _LL1A2: _LL1A3:
 return 15;case 14: _LL1A4: _LL1A5:
 goto _LL1A7;case 16: _LL1A6: _LL1A7:
 goto _LL1A9;case 17: _LL1A8: _LL1A9:
 goto _LL1AB;case 38: _LL1AA: _LL1AB:
 goto _LL1AD;case 39: _LL1AC: _LL1AD:
 goto _LL1AF;case 37: _LL1AE: _LL1AF:
 goto _LL1B1;case 18: _LL1B0: _LL1B1:
 goto _LL1B3;case 19: _LL1B2: _LL1B3:
 return 130;case 20: _LL1B4: _LL1B5:
 goto _LL1B7;case 21: _LL1B6: _LL1B7:
 goto _LL1B9;case 22: _LL1B8: _LL1B9:
 return 140;case 23: _LL1BA: _LL1BB:
 return 150;case 24: _LL1BC: _LL1BD:
 goto _LL1BF;case 25: _LL1BE: _LL1BF:
 goto _LL1C1;case 26: _LL1C0: _LL1C1:
 goto _LL1C3;case 27: _LL1C2: _LL1C3:
 goto _LL1C5;case 28: _LL1C4: _LL1C5:
 goto _LL1C7;case 29: _LL1C6: _LL1C7:
 goto _LL1C9;case 30: _LL1C8: _LL1C9:
 goto _LL1CB;case 31: _LL1CA: _LL1CB:
 goto _LL1CD;case 32: _LL1CC: _LL1CD:
 goto _LL1CF;case 33: _LL1CE: _LL1CF:
 goto _LL1D1;case 34: _LL1D0: _LL1D1:
 goto _LL1D3;case 35: _LL1D2: _LL1D3:
 return 140;default: _LL1D4: _LL1D5:
 return 10000;}_LL185:;}
# 1086
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1090
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp200=e->r;void*_tmp201=_tmp200;struct Cyc_Absyn_Stmt*_tmp202;struct Cyc_Core_Opt*_tmp203;struct Cyc_List_List*_tmp204;struct Cyc_Absyn_Exp*_tmp205;struct Cyc_Absyn_Exp*_tmp206;int _tmp207;struct Cyc_Absyn_Exp*_tmp208;void**_tmp209;struct Cyc_Absyn_Exp*_tmp20A;int _tmp20B;struct _tuple0*_tmp20C;struct _tuple0*_tmp20D;struct Cyc_List_List*_tmp20E;struct Cyc_Absyn_Datatypefield*_tmp20F;struct Cyc_List_List*_tmp210;struct _tuple0*_tmp211;struct Cyc_List_List*_tmp212;struct Cyc_List_List*_tmp213;struct Cyc_Absyn_Exp*_tmp214;void*_tmp215;struct Cyc_Absyn_Vardecl*_tmp216;struct Cyc_Absyn_Exp*_tmp217;struct Cyc_Absyn_Exp*_tmp218;struct Cyc_List_List*_tmp219;struct _tuple8*_tmp21A;struct Cyc_List_List*_tmp21B;struct Cyc_List_List*_tmp21C;struct Cyc_Absyn_Exp*_tmp21D;struct Cyc_Absyn_Exp*_tmp21E;struct Cyc_Absyn_Exp*_tmp21F;struct _dyneither_ptr*_tmp220;struct Cyc_Absyn_Exp*_tmp221;struct _dyneither_ptr*_tmp222;struct Cyc_Absyn_Exp*_tmp223;void*_tmp224;struct Cyc_List_List*_tmp225;struct Cyc_Absyn_Exp*_tmp226;struct _dyneither_ptr*_tmp227;int _tmp228;struct _dyneither_ptr _tmp229;void*_tmp22A;struct Cyc_Absyn_Exp*_tmp22B;void*_tmp22C;struct Cyc_Absyn_Exp*_tmp22D;struct Cyc_Absyn_Exp*_tmp22E;struct Cyc_Absyn_Exp*_tmp22F;void*_tmp230;struct Cyc_Absyn_Exp*_tmp231;struct Cyc_Absyn_Exp*_tmp232;struct Cyc_Absyn_Exp*_tmp233;struct Cyc_Absyn_Exp*_tmp234;struct Cyc_Absyn_Exp*_tmp235;struct Cyc_List_List*_tmp236;struct Cyc_Absyn_Exp*_tmp237;struct Cyc_Absyn_Exp*_tmp238;struct Cyc_Absyn_Exp*_tmp239;struct Cyc_Absyn_Exp*_tmp23A;struct Cyc_Absyn_Exp*_tmp23B;struct Cyc_Absyn_Exp*_tmp23C;struct Cyc_Absyn_Exp*_tmp23D;struct Cyc_Absyn_Exp*_tmp23E;struct Cyc_Absyn_Exp*_tmp23F;struct Cyc_Absyn_Exp*_tmp240;enum Cyc_Absyn_Incrementor _tmp241;struct Cyc_Absyn_Exp*_tmp242;struct Cyc_Core_Opt*_tmp243;struct Cyc_Absyn_Exp*_tmp244;enum Cyc_Absyn_Primop _tmp245;struct Cyc_List_List*_tmp246;struct _tuple0*_tmp247;union Cyc_Absyn_Cnst _tmp248;switch(*((int*)_tmp201)){case 0: _LL200: _tmp248=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL201:
 s=Cyc_Absynpp_cnst2doc(_tmp248);goto _LL1FF;case 1: _LL202: _tmp247=((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL203:
 s=Cyc_Absynpp_qvar2doc(_tmp247);goto _LL1FF;case 2: _LL204: _tmp245=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp246=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL205:
 s=Cyc_Absynpp_primapp2doc(myprec,_tmp245,_tmp246);goto _LL1FF;case 3: _LL206: _tmp242=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp243=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp244=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL207:
# 1098
 s=({struct Cyc_PP_Doc*_tmp249[5];_tmp249[4]=
# 1102
Cyc_Absynpp_exp2doc_prec(myprec,_tmp244);_tmp249[3]=
# 1101
Cyc_PP_text(({const char*_tmp24B="= ";_tag_dyneither(_tmp24B,sizeof(char),3);}));_tmp249[2]=
# 1100
_tmp243 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp243->v);_tmp249[1]=
# 1099
Cyc_PP_text(({const char*_tmp24A=" ";_tag_dyneither(_tmp24A,sizeof(char),2);}));_tmp249[0]=
# 1098
Cyc_Absynpp_exp2doc_prec(myprec,_tmp242);Cyc_PP_cat(_tag_dyneither(_tmp249,sizeof(struct Cyc_PP_Doc*),5));});
# 1103
goto _LL1FF;case 4: _LL208: _tmp240=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp241=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL209: {
# 1105
struct Cyc_PP_Doc*_tmp24C=Cyc_Absynpp_exp2doc_prec(myprec,_tmp240);
{enum Cyc_Absyn_Incrementor _tmp24D=_tmp241;switch(_tmp24D){case Cyc_Absyn_PreInc: _LL251: _LL252:
 s=({struct Cyc_PP_Doc*_tmp24E[2];_tmp24E[1]=_tmp24C;_tmp24E[0]=Cyc_PP_text(({const char*_tmp24F="++";_tag_dyneither(_tmp24F,sizeof(char),3);}));Cyc_PP_cat(_tag_dyneither(_tmp24E,sizeof(struct Cyc_PP_Doc*),2));});goto _LL250;case Cyc_Absyn_PreDec: _LL253: _LL254:
 s=({struct Cyc_PP_Doc*_tmp250[2];_tmp250[1]=_tmp24C;_tmp250[0]=Cyc_PP_text(({const char*_tmp251="--";_tag_dyneither(_tmp251,sizeof(char),3);}));Cyc_PP_cat(_tag_dyneither(_tmp250,sizeof(struct Cyc_PP_Doc*),2));});goto _LL250;case Cyc_Absyn_PostInc: _LL255: _LL256:
 s=({struct Cyc_PP_Doc*_tmp252[2];_tmp252[1]=Cyc_PP_text(({const char*_tmp253="++";_tag_dyneither(_tmp253,sizeof(char),3);}));_tmp252[0]=_tmp24C;Cyc_PP_cat(_tag_dyneither(_tmp252,sizeof(struct Cyc_PP_Doc*),2));});goto _LL250;default: _LL257: _LL258:
 s=({struct Cyc_PP_Doc*_tmp254[2];_tmp254[1]=Cyc_PP_text(({const char*_tmp255="--";_tag_dyneither(_tmp255,sizeof(char),3);}));_tmp254[0]=_tmp24C;Cyc_PP_cat(_tag_dyneither(_tmp254,sizeof(struct Cyc_PP_Doc*),2));});goto _LL250;}_LL250:;}
# 1112
goto _LL1FF;}case 5: _LL20A: _tmp23D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp23E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp23F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL20B:
# 1114
 s=({struct Cyc_PP_Doc*_tmp256[5];_tmp256[4]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp23F);_tmp256[3]=Cyc_PP_text(({const char*_tmp258=" : ";_tag_dyneither(_tmp258,sizeof(char),4);}));_tmp256[2]=
# 1114
Cyc_Absynpp_exp2doc_prec(0,_tmp23E);_tmp256[1]=Cyc_PP_text(({const char*_tmp257=" ? ";_tag_dyneither(_tmp257,sizeof(char),4);}));_tmp256[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp23D);Cyc_PP_cat(_tag_dyneither(_tmp256,sizeof(struct Cyc_PP_Doc*),5));});
# 1116
goto _LL1FF;case 6: _LL20C: _tmp23B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp23C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL20D:
# 1118
 s=({struct Cyc_PP_Doc*_tmp259[3];_tmp259[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp23C);_tmp259[1]=Cyc_PP_text(({const char*_tmp25A=" && ";_tag_dyneither(_tmp25A,sizeof(char),5);}));_tmp259[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp23B);Cyc_PP_cat(_tag_dyneither(_tmp259,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1FF;case 7: _LL20E: _tmp239=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp23A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL20F:
# 1121
 s=({struct Cyc_PP_Doc*_tmp25B[3];_tmp25B[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp23A);_tmp25B[1]=Cyc_PP_text(({const char*_tmp25C=" || ";_tag_dyneither(_tmp25C,sizeof(char),5);}));_tmp25B[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp239);Cyc_PP_cat(_tag_dyneither(_tmp25B,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1FF;case 8: _LL210: _tmp237=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp238=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL211:
# 1126
 s=({struct Cyc_PP_Doc*_tmp25D[5];_tmp25D[4]=Cyc_PP_text(({const char*_tmp260=")";_tag_dyneither(_tmp260,sizeof(char),2);}));_tmp25D[3]=Cyc_Absynpp_exp2doc(_tmp238);_tmp25D[2]=Cyc_PP_text(({const char*_tmp25F=", ";_tag_dyneither(_tmp25F,sizeof(char),3);}));_tmp25D[1]=Cyc_Absynpp_exp2doc(_tmp237);_tmp25D[0]=Cyc_PP_text(({const char*_tmp25E="(";_tag_dyneither(_tmp25E,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp25D,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL1FF;case 9: _LL212: _tmp235=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp236=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL213:
# 1129
 s=({struct Cyc_PP_Doc*_tmp261[4];_tmp261[3]=
# 1132
Cyc_PP_text(({const char*_tmp263=")";_tag_dyneither(_tmp263,sizeof(char),2);}));_tmp261[2]=
# 1131
Cyc_Absynpp_exps2doc_prec(20,_tmp236);_tmp261[1]=
# 1130
Cyc_PP_text(({const char*_tmp262="(";_tag_dyneither(_tmp262,sizeof(char),2);}));_tmp261[0]=
# 1129
Cyc_Absynpp_exp2doc_prec(myprec,_tmp235);Cyc_PP_cat(_tag_dyneither(_tmp261,sizeof(struct Cyc_PP_Doc*),4));});
# 1133
goto _LL1FF;case 10: _LL214: _tmp234=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL215:
# 1135
 s=({struct Cyc_PP_Doc*_tmp264[2];_tmp264[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp234);_tmp264[0]=Cyc_PP_text(({const char*_tmp265="throw ";_tag_dyneither(_tmp265,sizeof(char),7);}));Cyc_PP_cat(_tag_dyneither(_tmp264,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL1FF;case 11: _LL216: _tmp233=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL217:
# 1138
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp233);
goto _LL1FF;case 12: _LL218: _tmp232=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL219:
# 1141
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp232);
goto _LL1FF;case 13: _LL21A: _tmp230=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp231=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL21B:
# 1144
 s=({struct Cyc_PP_Doc*_tmp266[4];_tmp266[3]=
# 1147
Cyc_Absynpp_exp2doc_prec(myprec,_tmp231);_tmp266[2]=
# 1146
Cyc_PP_text(({const char*_tmp268=")";_tag_dyneither(_tmp268,sizeof(char),2);}));_tmp266[1]=
# 1145
Cyc_Absynpp_typ2doc(_tmp230);_tmp266[0]=
# 1144
Cyc_PP_text(({const char*_tmp267="(";_tag_dyneither(_tmp267,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp266,sizeof(struct Cyc_PP_Doc*),4));});
# 1148
goto _LL1FF;case 14: _LL21C: _tmp22F=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL21D:
# 1150
 s=({struct Cyc_PP_Doc*_tmp269[2];_tmp269[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp22F);_tmp269[0]=
# 1150
Cyc_PP_text(({const char*_tmp26A="&";_tag_dyneither(_tmp26A,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp269,sizeof(struct Cyc_PP_Doc*),2));});
# 1152
goto _LL1FF;case 15: _LL21E: _tmp22D=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp22E=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL21F:
# 1154
 if(_tmp22D == 0)
s=({struct Cyc_PP_Doc*_tmp26B[2];_tmp26B[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp22E);_tmp26B[0]=Cyc_PP_text(({const char*_tmp26C="new ";_tag_dyneither(_tmp26C,sizeof(char),5);}));Cyc_PP_cat(_tag_dyneither(_tmp26B,sizeof(struct Cyc_PP_Doc*),2));});else{
# 1157
s=({struct Cyc_PP_Doc*_tmp26D[4];_tmp26D[3]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp22E);_tmp26D[2]=
# 1157
Cyc_PP_text(({const char*_tmp26F=") ";_tag_dyneither(_tmp26F,sizeof(char),3);}));_tmp26D[1]=Cyc_Absynpp_exp2doc(_tmp22D);_tmp26D[0]=Cyc_PP_text(({const char*_tmp26E="rnew(";_tag_dyneither(_tmp26E,sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp26D,sizeof(struct Cyc_PP_Doc*),4));});}
# 1159
goto _LL1FF;case 16: _LL220: _tmp22C=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL221:
 s=({struct Cyc_PP_Doc*_tmp270[3];_tmp270[2]=Cyc_PP_text(({const char*_tmp272=")";_tag_dyneither(_tmp272,sizeof(char),2);}));_tmp270[1]=Cyc_Absynpp_typ2doc(_tmp22C);_tmp270[0]=Cyc_PP_text(({const char*_tmp271="sizeof(";_tag_dyneither(_tmp271,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp270,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1FF;case 17: _LL222: _tmp22B=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL223:
 s=({struct Cyc_PP_Doc*_tmp273[3];_tmp273[2]=Cyc_PP_text(({const char*_tmp275=")";_tag_dyneither(_tmp275,sizeof(char),2);}));_tmp273[1]=Cyc_Absynpp_exp2doc(_tmp22B);_tmp273[0]=Cyc_PP_text(({const char*_tmp274="sizeof(";_tag_dyneither(_tmp274,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp273,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1FF;case 38: _LL224: _tmp22A=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL225:
 s=({struct Cyc_PP_Doc*_tmp276[3];_tmp276[2]=Cyc_PP_text(({const char*_tmp278=")";_tag_dyneither(_tmp278,sizeof(char),2);}));_tmp276[1]=Cyc_Absynpp_typ2doc(_tmp22A);_tmp276[0]=Cyc_PP_text(({const char*_tmp277="valueof(";_tag_dyneither(_tmp277,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp276,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1FF;case 39: _LL226: _tmp228=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp229=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL227:
# 1164
 if(_tmp228)
s=({struct Cyc_PP_Doc*_tmp279[4];_tmp279[3]=Cyc_PP_text(({const char*_tmp27C=")";_tag_dyneither(_tmp27C,sizeof(char),2);}));_tmp279[2]=Cyc_PP_text(_tmp229);_tmp279[1]=Cyc_PP_text(({const char*_tmp27B=" volatile (";_tag_dyneither(_tmp27B,sizeof(char),12);}));_tmp279[0]=Cyc_PP_text(({const char*_tmp27A="__asm__";_tag_dyneither(_tmp27A,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp279,sizeof(struct Cyc_PP_Doc*),4));});else{
# 1167
s=({struct Cyc_PP_Doc*_tmp27D[3];_tmp27D[2]=Cyc_PP_text(({const char*_tmp27F=")";_tag_dyneither(_tmp27F,sizeof(char),2);}));_tmp27D[1]=Cyc_PP_text(_tmp229);_tmp27D[0]=Cyc_PP_text(({const char*_tmp27E="__asm__(";_tag_dyneither(_tmp27E,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp27D,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LL1FF;case 37: _LL228: _tmp226=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp227=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL229:
# 1170
 s=({struct Cyc_PP_Doc*_tmp280[5];_tmp280[4]=Cyc_PP_text(({const char*_tmp283=")";_tag_dyneither(_tmp283,sizeof(char),2);}));_tmp280[3]=Cyc_PP_textptr(_tmp227);_tmp280[2]=Cyc_PP_text(({const char*_tmp282=".";_tag_dyneither(_tmp282,sizeof(char),2);}));_tmp280[1]=Cyc_Absynpp_exp2doc(_tmp226);_tmp280[0]=Cyc_PP_text(({const char*_tmp281="tagcheck(";_tag_dyneither(_tmp281,sizeof(char),10);}));Cyc_PP_cat(_tag_dyneither(_tmp280,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL1FF;case 18: _LL22A: _tmp224=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp225=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL22B:
# 1173
 s=({struct Cyc_PP_Doc*_tmp284[3];_tmp284[2]=Cyc_PP_text(({const char*_tmp286=",";_tag_dyneither(_tmp286,sizeof(char),2);}));_tmp284[1]=Cyc_Absynpp_typ2doc(_tmp224);_tmp284[0]=Cyc_PP_text(({const char*_tmp285="offsetof(";_tag_dyneither(_tmp285,sizeof(char),10);}));Cyc_PP_cat(_tag_dyneither(_tmp284,sizeof(struct Cyc_PP_Doc*),3));});
for(0;_tmp225 != 0;_tmp225=_tmp225->tl){
void*_tmp287=(void*)_tmp225->hd;void*_tmp288=_tmp287;unsigned int _tmp289;struct _dyneither_ptr*_tmp28A;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp288)->tag == 0){_LL25A: _tmp28A=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp288)->f1;_LL25B:
 s=({struct Cyc_PP_Doc*_tmp28B[3];_tmp28B[2]=
_tmp225->tl != 0?Cyc_PP_text(({const char*_tmp28C=".";_tag_dyneither(_tmp28C,sizeof(char),2);})): Cyc_PP_nil_doc();_tmp28B[1]=
# 1176
Cyc_PP_textptr(_tmp28A);_tmp28B[0]=s;Cyc_PP_cat(_tag_dyneither(_tmp28B,sizeof(struct Cyc_PP_Doc*),3));});
# 1178
goto _LL259;}else{_LL25C: _tmp289=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp288)->f1;_LL25D:
 s=({struct Cyc_PP_Doc*_tmp28D[3];_tmp28D[2]=
_tmp225->tl != 0?Cyc_PP_text(({const char*_tmp291=".";_tag_dyneither(_tmp291,sizeof(char),2);})): Cyc_PP_nil_doc();_tmp28D[1]=
# 1179
Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp290;_tmp290.tag=1;_tmp290.f1=(unsigned long)((int)_tmp289);({void*_tmp28E[1]={& _tmp290};Cyc_aprintf(({const char*_tmp28F="%d";_tag_dyneither(_tmp28F,sizeof(char),3);}),_tag_dyneither(_tmp28E,sizeof(void*),1));});}));_tmp28D[0]=s;Cyc_PP_cat(_tag_dyneither(_tmp28D,sizeof(struct Cyc_PP_Doc*),3));});
# 1181
goto _LL259;}_LL259:;}
# 1183
s=({struct Cyc_PP_Doc*_tmp292[2];_tmp292[1]=Cyc_PP_text(({const char*_tmp293=")";_tag_dyneither(_tmp293,sizeof(char),2);}));_tmp292[0]=s;Cyc_PP_cat(_tag_dyneither(_tmp292,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL1FF;case 19: _LL22C: _tmp223=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL22D:
# 1186
 s=({struct Cyc_PP_Doc*_tmp294[2];_tmp294[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp223);_tmp294[0]=Cyc_PP_text(({const char*_tmp295="*";_tag_dyneither(_tmp295,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp294,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL1FF;case 20: _LL22E: _tmp221=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp222=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL22F:
# 1189
 s=({struct Cyc_PP_Doc*_tmp296[3];_tmp296[2]=Cyc_PP_textptr(_tmp222);_tmp296[1]=Cyc_PP_text(({const char*_tmp297=".";_tag_dyneither(_tmp297,sizeof(char),2);}));_tmp296[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp221);Cyc_PP_cat(_tag_dyneither(_tmp296,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1FF;case 21: _LL230: _tmp21F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp220=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL231:
# 1192
 s=({struct Cyc_PP_Doc*_tmp298[3];_tmp298[2]=Cyc_PP_textptr(_tmp220);_tmp298[1]=Cyc_PP_text(({const char*_tmp299="->";_tag_dyneither(_tmp299,sizeof(char),3);}));_tmp298[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp21F);Cyc_PP_cat(_tag_dyneither(_tmp298,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1FF;case 22: _LL232: _tmp21D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp21E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL233:
# 1195
 s=({struct Cyc_PP_Doc*_tmp29A[4];_tmp29A[3]=
# 1198
Cyc_PP_text(({const char*_tmp29C="]";_tag_dyneither(_tmp29C,sizeof(char),2);}));_tmp29A[2]=
# 1197
Cyc_Absynpp_exp2doc(_tmp21E);_tmp29A[1]=
# 1196
Cyc_PP_text(({const char*_tmp29B="[";_tag_dyneither(_tmp29B,sizeof(char),2);}));_tmp29A[0]=
# 1195
Cyc_Absynpp_exp2doc_prec(myprec,_tmp21D);Cyc_PP_cat(_tag_dyneither(_tmp29A,sizeof(struct Cyc_PP_Doc*),4));});
# 1199
goto _LL1FF;case 23: _LL234: _tmp21C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL235:
# 1201
 s=({struct Cyc_PP_Doc*_tmp29D[4];_tmp29D[3]=
# 1204
Cyc_PP_text(({const char*_tmp29F=")";_tag_dyneither(_tmp29F,sizeof(char),2);}));_tmp29D[2]=
# 1203
Cyc_Absynpp_exps2doc_prec(20,_tmp21C);_tmp29D[1]=
# 1202
Cyc_PP_text(({const char*_tmp29E="(";_tag_dyneither(_tmp29E,sizeof(char),2);}));_tmp29D[0]=
# 1201
Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dyneither(_tmp29D,sizeof(struct Cyc_PP_Doc*),4));});
# 1205
goto _LL1FF;case 24: _LL236: _tmp21A=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp21B=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL237:
# 1207
 s=({struct Cyc_PP_Doc*_tmp2A0[4];_tmp2A0[3]=
# 1210
Cyc_Absynpp_group_braces(({const char*_tmp2A3=",";_tag_dyneither(_tmp2A3,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp21B));_tmp2A0[2]=
# 1209
Cyc_PP_text(({const char*_tmp2A2=")";_tag_dyneither(_tmp2A2,sizeof(char),2);}));_tmp2A0[1]=
# 1208
Cyc_Absynpp_typ2doc((*_tmp21A).f3);_tmp2A0[0]=
# 1207
Cyc_PP_text(({const char*_tmp2A1="(";_tag_dyneither(_tmp2A1,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp2A0,sizeof(struct Cyc_PP_Doc*),4));});
# 1211
goto _LL1FF;case 25: _LL238: _tmp219=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL239:
# 1213
 s=Cyc_Absynpp_group_braces(({const char*_tmp2A4=",";_tag_dyneither(_tmp2A4,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp219));
goto _LL1FF;case 26: _LL23A: _tmp216=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp217=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp218=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL23B:
# 1216
 s=({struct Cyc_PP_Doc*_tmp2A5[8];_tmp2A5[7]=
# 1222
Cyc_Absynpp_rb();_tmp2A5[6]=
# 1221
Cyc_Absynpp_exp2doc(_tmp218);_tmp2A5[5]=
# 1220
Cyc_PP_text(({const char*_tmp2A8=" : ";_tag_dyneither(_tmp2A8,sizeof(char),4);}));_tmp2A5[4]=
# 1219
Cyc_Absynpp_exp2doc(_tmp217);_tmp2A5[3]=
# 1218
Cyc_PP_text(({const char*_tmp2A7=" < ";_tag_dyneither(_tmp2A7,sizeof(char),4);}));_tmp2A5[2]=
# 1217
Cyc_PP_text(*(*_tmp216->name).f2);_tmp2A5[1]=
# 1216
Cyc_PP_text(({const char*_tmp2A6="for ";_tag_dyneither(_tmp2A6,sizeof(char),5);}));_tmp2A5[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dyneither(_tmp2A5,sizeof(struct Cyc_PP_Doc*),8));});
# 1223
goto _LL1FF;case 27: _LL23C: _tmp214=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp215=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL23D:
# 1225
 s=({struct Cyc_PP_Doc*_tmp2A9[7];_tmp2A9[6]=
# 1230
Cyc_Absynpp_rb();_tmp2A9[5]=
# 1229
Cyc_Absynpp_typ2doc(_tmp215);_tmp2A9[4]=
# 1228
Cyc_PP_text(({const char*_tmp2AC=" : ";_tag_dyneither(_tmp2AC,sizeof(char),4);}));_tmp2A9[3]=
# 1227
Cyc_Absynpp_exp2doc(_tmp214);_tmp2A9[2]=
# 1226
Cyc_PP_text(({const char*_tmp2AB=" < ";_tag_dyneither(_tmp2AB,sizeof(char),4);}));_tmp2A9[1]=
# 1225
Cyc_PP_text(({const char*_tmp2AA="for x ";_tag_dyneither(_tmp2AA,sizeof(char),7);}));_tmp2A9[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dyneither(_tmp2A9,sizeof(struct Cyc_PP_Doc*),7));});
# 1231
goto _LL1FF;case 28: _LL23E: _tmp211=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp212=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp213=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL23F: {
# 1233
struct Cyc_List_List*_tmp2AD=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp213);
s=({struct Cyc_PP_Doc*_tmp2AE[2];_tmp2AE[1]=
Cyc_Absynpp_group_braces(({const char*_tmp2AF=",";_tag_dyneither(_tmp2AF,sizeof(char),2);}),
_tmp212 != 0?({struct Cyc_List_List*_tmp2B0=_cycalloc(sizeof(*_tmp2B0));_tmp2B0->hd=Cyc_Absynpp_tps2doc(_tmp212);_tmp2B0->tl=_tmp2AD;_tmp2B0;}): _tmp2AD);_tmp2AE[0]=
# 1234
Cyc_Absynpp_qvar2doc(_tmp211);Cyc_PP_cat(_tag_dyneither(_tmp2AE,sizeof(struct Cyc_PP_Doc*),2));});
# 1237
goto _LL1FF;}case 29: _LL240: _tmp210=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL241:
# 1239
 s=Cyc_Absynpp_group_braces(({const char*_tmp2B1=",";_tag_dyneither(_tmp2B1,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp210));
goto _LL1FF;case 30: _LL242: _tmp20E=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp20F=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL243:
# 1242
 if(_tmp20E == 0)
# 1244
s=Cyc_Absynpp_qvar2doc(_tmp20F->name);else{
# 1246
s=({struct Cyc_PP_Doc*_tmp2B2[2];_tmp2B2[1]=
Cyc_PP_egroup(({const char*_tmp2B3="(";_tag_dyneither(_tmp2B3,sizeof(char),2);}),({const char*_tmp2B4=")";_tag_dyneither(_tmp2B4,sizeof(char),2);}),({const char*_tmp2B5=",";_tag_dyneither(_tmp2B5,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp20E));_tmp2B2[0]=
# 1246
Cyc_Absynpp_qvar2doc(_tmp20F->name);Cyc_PP_cat(_tag_dyneither(_tmp2B2,sizeof(struct Cyc_PP_Doc*),2));});}
# 1248
goto _LL1FF;case 31: _LL244: _tmp20D=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL245:
# 1250
 s=Cyc_Absynpp_qvar2doc(_tmp20D);
goto _LL1FF;case 32: _LL246: _tmp20C=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL247:
# 1253
 s=Cyc_Absynpp_qvar2doc(_tmp20C);
goto _LL1FF;case 33: _LL248: _tmp207=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).is_calloc;_tmp208=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).rgn;_tmp209=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).elt_type;_tmp20A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).num_elts;_tmp20B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).inline_call;_LL249:
# 1258
 if(_tmp207){
# 1260
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp209)),0);
if(_tmp208 == 0)
s=({struct Cyc_PP_Doc*_tmp2B6[5];_tmp2B6[4]=Cyc_PP_text(({const char*_tmp2B9=")";_tag_dyneither(_tmp2B9,sizeof(char),2);}));_tmp2B6[3]=Cyc_Absynpp_exp2doc(st);_tmp2B6[2]=Cyc_PP_text(({const char*_tmp2B8=",";_tag_dyneither(_tmp2B8,sizeof(char),2);}));_tmp2B6[1]=Cyc_Absynpp_exp2doc(_tmp20A);_tmp2B6[0]=Cyc_PP_text(({const char*_tmp2B7="calloc(";_tag_dyneither(_tmp2B7,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp2B6,sizeof(struct Cyc_PP_Doc*),5));});else{
# 1264
s=({struct Cyc_PP_Doc*_tmp2BA[7];_tmp2BA[6]=
Cyc_PP_text(({const char*_tmp2BE=")";_tag_dyneither(_tmp2BE,sizeof(char),2);}));_tmp2BA[5]=Cyc_Absynpp_exp2doc(st);_tmp2BA[4]=Cyc_PP_text(({const char*_tmp2BD=",";_tag_dyneither(_tmp2BD,sizeof(char),2);}));_tmp2BA[3]=Cyc_Absynpp_exp2doc(_tmp20A);_tmp2BA[2]=
# 1264
Cyc_PP_text(({const char*_tmp2BC=",";_tag_dyneither(_tmp2BC,sizeof(char),2);}));_tmp2BA[1]=Cyc_Absynpp_exp2doc(_tmp208);_tmp2BA[0]=Cyc_PP_text(({const char*_tmp2BB="rcalloc(";_tag_dyneither(_tmp2BB,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp2BA,sizeof(struct Cyc_PP_Doc*),7));});}}else{
# 1267
struct Cyc_Absyn_Exp*new_e;
# 1269
if(_tmp209 == 0)
new_e=_tmp20A;else{
# 1272
new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp209,0),_tmp20A,0);}
# 1274
if(_tmp208 == 0)
s=({struct Cyc_PP_Doc*_tmp2BF[3];_tmp2BF[2]=Cyc_PP_text(({const char*_tmp2C1=")";_tag_dyneither(_tmp2C1,sizeof(char),2);}));_tmp2BF[1]=Cyc_Absynpp_exp2doc(new_e);_tmp2BF[0]=Cyc_PP_text(({const char*_tmp2C0="malloc(";_tag_dyneither(_tmp2C0,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp2BF,sizeof(struct Cyc_PP_Doc*),3));});else{
# 1277
if(_tmp20B)
s=({struct Cyc_PP_Doc*_tmp2C2[5];_tmp2C2[4]=
Cyc_PP_text(({const char*_tmp2C5=")";_tag_dyneither(_tmp2C5,sizeof(char),2);}));_tmp2C2[3]=Cyc_Absynpp_exp2doc(new_e);_tmp2C2[2]=
# 1278
Cyc_PP_text(({const char*_tmp2C4=",";_tag_dyneither(_tmp2C4,sizeof(char),2);}));_tmp2C2[1]=Cyc_Absynpp_exp2doc(_tmp208);_tmp2C2[0]=Cyc_PP_text(({const char*_tmp2C3="rmalloc_inline(";_tag_dyneither(_tmp2C3,sizeof(char),16);}));Cyc_PP_cat(_tag_dyneither(_tmp2C2,sizeof(struct Cyc_PP_Doc*),5));});else{
# 1281
s=({struct Cyc_PP_Doc*_tmp2C6[5];_tmp2C6[4]=
Cyc_PP_text(({const char*_tmp2C9=")";_tag_dyneither(_tmp2C9,sizeof(char),2);}));_tmp2C6[3]=Cyc_Absynpp_exp2doc(new_e);_tmp2C6[2]=
# 1281
Cyc_PP_text(({const char*_tmp2C8=",";_tag_dyneither(_tmp2C8,sizeof(char),2);}));_tmp2C6[1]=Cyc_Absynpp_exp2doc(_tmp208);_tmp2C6[0]=Cyc_PP_text(({const char*_tmp2C7="rmalloc(";_tag_dyneither(_tmp2C7,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp2C6,sizeof(struct Cyc_PP_Doc*),5));});}}}
# 1285
goto _LL1FF;case 34: _LL24A: _tmp205=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp206=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL24B:
# 1287
 s=({struct Cyc_PP_Doc*_tmp2CA[3];_tmp2CA[2]=
# 1289
Cyc_Absynpp_exp2doc_prec(myprec,_tmp206);_tmp2CA[1]=
# 1288
Cyc_PP_text(({const char*_tmp2CB=" :=: ";_tag_dyneither(_tmp2CB,sizeof(char),6);}));_tmp2CA[0]=
# 1287
Cyc_Absynpp_exp2doc_prec(myprec,_tmp205);Cyc_PP_cat(_tag_dyneither(_tmp2CA,sizeof(struct Cyc_PP_Doc*),3));});
# 1290
goto _LL1FF;case 35: _LL24C: _tmp203=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp204=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL24D:
# 1293
 s=Cyc_Absynpp_group_braces(({const char*_tmp2CC=",";_tag_dyneither(_tmp2CC,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp204));
goto _LL1FF;default: _LL24E: _tmp202=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL24F:
# 1296
 s=({struct Cyc_PP_Doc*_tmp2CD[7];_tmp2CD[6]=
# 1298
Cyc_PP_text(({const char*_tmp2CF=")";_tag_dyneither(_tmp2CF,sizeof(char),2);}));_tmp2CD[5]=Cyc_Absynpp_rb();_tmp2CD[4]=Cyc_PP_blank_doc();_tmp2CD[3]=
# 1297
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp202,1));_tmp2CD[2]=
# 1296
Cyc_PP_blank_doc();_tmp2CD[1]=Cyc_Absynpp_lb();_tmp2CD[0]=Cyc_PP_text(({const char*_tmp2CE="(";_tag_dyneither(_tmp2CE,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp2CD,sizeof(struct Cyc_PP_Doc*),7));});
# 1299
goto _LL1FF;}_LL1FF:;}
# 1301
if(inprec >= myprec)
s=({struct Cyc_PP_Doc*_tmp2D0[3];_tmp2D0[2]=Cyc_PP_text(({const char*_tmp2D2=")";_tag_dyneither(_tmp2D2,sizeof(char),2);}));_tmp2D0[1]=s;_tmp2D0[0]=Cyc_PP_text(({const char*_tmp2D1="(";_tag_dyneither(_tmp2D1,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp2D0,sizeof(struct Cyc_PP_Doc*),3));});
return s;}
# 1306
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp2D3=d;struct _dyneither_ptr*_tmp2D4;struct Cyc_Absyn_Exp*_tmp2D5;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2D3)->tag == 0){_LL25F: _tmp2D5=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2D3)->f1;_LL260:
 return({struct Cyc_PP_Doc*_tmp2D6[3];_tmp2D6[2]=Cyc_PP_text(({const char*_tmp2D8="]";_tag_dyneither(_tmp2D8,sizeof(char),2);}));_tmp2D6[1]=Cyc_Absynpp_exp2doc(_tmp2D5);_tmp2D6[0]=Cyc_PP_text(({const char*_tmp2D7=".[";_tag_dyneither(_tmp2D7,sizeof(char),3);}));Cyc_PP_cat(_tag_dyneither(_tmp2D6,sizeof(struct Cyc_PP_Doc*),3));});}else{_LL261: _tmp2D4=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2D3)->f1;_LL262:
 return({struct Cyc_PP_Doc*_tmp2D9[2];_tmp2D9[1]=Cyc_PP_textptr(_tmp2D4);_tmp2D9[0]=Cyc_PP_text(({const char*_tmp2DA=".";_tag_dyneither(_tmp2DA,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp2D9,sizeof(struct Cyc_PP_Doc*),2));});}_LL25E:;}
# 1313
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
return({struct Cyc_PP_Doc*_tmp2DB[2];_tmp2DB[1]=
Cyc_Absynpp_exp2doc((*de).f2);_tmp2DB[0]=
# 1315
Cyc_PP_egroup(({const char*_tmp2DC="";_tag_dyneither(_tmp2DC,sizeof(char),1);}),({const char*_tmp2DD="=";_tag_dyneither(_tmp2DD,sizeof(char),2);}),({const char*_tmp2DE="=";_tag_dyneither(_tmp2DE,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));Cyc_PP_cat(_tag_dyneither(_tmp2DB,sizeof(struct Cyc_PP_Doc*),2));});}}
# 1319
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
return Cyc_PP_group(({const char*_tmp2DF="";_tag_dyneither(_tmp2DF,sizeof(char),1);}),({const char*_tmp2E0="";_tag_dyneither(_tmp2E0,sizeof(char),1);}),({const char*_tmp2E1=",";_tag_dyneither(_tmp2E1,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
# 1323
struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
struct _dyneither_ptr x=({char*_tmp2F7=({unsigned int _tmp2F4=(unsigned int)28;char*_tmp2F5=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp2F4 + 1));{unsigned int _tmp2F6=_tmp2F4;unsigned int i;for(i=0;i < _tmp2F6;i ++){_tmp2F5[i]=(char)'z';}_tmp2F5[_tmp2F6]=(char)0;}_tmp2F5;});_tag_dyneither(_tmp2F7,sizeof(char),_get_zero_arr_size_char((void*)_tmp2F7,(unsigned int)28 + 1));});
({struct _dyneither_ptr _tmp2E2=_dyneither_ptr_plus(x,sizeof(char),27);char _tmp2E3=*((char*)_check_dyneither_subscript(_tmp2E2,sizeof(char),0));char _tmp2E4='\000';if(_get_dyneither_size(_tmp2E2,sizeof(char))== 1  && (_tmp2E3 == '\000'  && _tmp2E4 != '\000'))_throw_arraybounds();*((char*)_tmp2E2.curr)=_tmp2E4;});
({struct _dyneither_ptr _tmp2E5=_dyneither_ptr_plus(x,sizeof(char),26);char _tmp2E6=*((char*)_check_dyneither_subscript(_tmp2E5,sizeof(char),0));char _tmp2E7='L';if(_get_dyneither_size(_tmp2E5,sizeof(char))== 1  && (_tmp2E6 == '\000'  && _tmp2E7 != '\000'))_throw_arraybounds();*((char*)_tmp2E5.curr)=_tmp2E7;});
({struct _dyneither_ptr _tmp2E8=_dyneither_ptr_plus(x,sizeof(char),25);char _tmp2E9=*((char*)_check_dyneither_subscript(_tmp2E8,sizeof(char),0));char _tmp2EA='L';if(_get_dyneither_size(_tmp2E8,sizeof(char))== 1  && (_tmp2E9 == '\000'  && _tmp2EA != '\000'))_throw_arraybounds();*((char*)_tmp2E8.curr)=_tmp2EA;});
({struct _dyneither_ptr _tmp2EB=_dyneither_ptr_plus(x,sizeof(char),24);char _tmp2EC=*((char*)_check_dyneither_subscript(_tmp2EB,sizeof(char),0));char _tmp2ED='U';if(_get_dyneither_size(_tmp2EB,sizeof(char))== 1  && (_tmp2EC == '\000'  && _tmp2ED != '\000'))_throw_arraybounds();*((char*)_tmp2EB.curr)=_tmp2ED;});
({struct _dyneither_ptr _tmp2EE=_dyneither_ptr_plus(x,sizeof(char),23);char _tmp2EF=*((char*)_check_dyneither_subscript(_tmp2EE,sizeof(char),0));char _tmp2F0='0';if(_get_dyneither_size(_tmp2EE,sizeof(char))== 1  && (_tmp2EF == '\000'  && _tmp2F0 != '\000'))_throw_arraybounds();*((char*)_tmp2EE.curr)=_tmp2F0;});{
int index=23;
while(i != 0){
char c=(char)('0' + i % 10);
({struct _dyneither_ptr _tmp2F1=_dyneither_ptr_plus(x,sizeof(char),index);char _tmp2F2=*((char*)_check_dyneither_subscript(_tmp2F1,sizeof(char),0));char _tmp2F3=c;if(_get_dyneither_size(_tmp2F1,sizeof(char))== 1  && (_tmp2F2 == '\000'  && _tmp2F3 != '\000'))_throw_arraybounds();*((char*)_tmp2F1.curr)=_tmp2F3;});
i=i / 10;
-- index;}
# 1337
return(struct _dyneither_ptr)_dyneither_ptr_plus(_dyneither_ptr_plus(x,sizeof(char),index),sizeof(char),1);};}
# 1341
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp2F8=c;struct _dyneither_ptr _tmp2F9;struct _dyneither_ptr _tmp2FA;struct _dyneither_ptr _tmp2FB;enum Cyc_Absyn_Sign _tmp2FC;long long _tmp2FD;int _tmp2FE;int _tmp2FF;int _tmp300;enum Cyc_Absyn_Sign _tmp301;short _tmp302;struct _dyneither_ptr _tmp303;enum Cyc_Absyn_Sign _tmp304;char _tmp305;switch((_tmp2F8.String_c).tag){case 2: _LL264: _tmp304=((_tmp2F8.Char_c).val).f1;_tmp305=((_tmp2F8.Char_c).val).f2;_LL265:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp308;_tmp308.tag=0;_tmp308.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp305));({void*_tmp306[1]={& _tmp308};Cyc_aprintf(({const char*_tmp307="'%s'";_tag_dyneither(_tmp307,sizeof(char),5);}),_tag_dyneither(_tmp306,sizeof(void*),1));});}));case 3: _LL266: _tmp303=(_tmp2F8.Wchar_c).val;_LL267:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp30B;_tmp30B.tag=0;_tmp30B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp303);({void*_tmp309[1]={& _tmp30B};Cyc_aprintf(({const char*_tmp30A="L'%s'";_tag_dyneither(_tmp30A,sizeof(char),6);}),_tag_dyneither(_tmp309,sizeof(void*),1));});}));case 4: _LL268: _tmp301=((_tmp2F8.Short_c).val).f1;_tmp302=((_tmp2F8.Short_c).val).f2;_LL269:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp30E;_tmp30E.tag=1;_tmp30E.f1=(unsigned long)((int)_tmp302);({void*_tmp30C[1]={& _tmp30E};Cyc_aprintf(({const char*_tmp30D="%d";_tag_dyneither(_tmp30D,sizeof(char),3);}),_tag_dyneither(_tmp30C,sizeof(void*),1));});}));case 5: switch(((_tmp2F8.Int_c).val).f1){case Cyc_Absyn_None: _LL26A: _tmp300=((_tmp2F8.Int_c).val).f2;_LL26B:
 _tmp2FF=_tmp300;goto _LL26D;case Cyc_Absyn_Signed: _LL26C: _tmp2FF=((_tmp2F8.Int_c).val).f2;_LL26D:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp311;_tmp311.tag=1;_tmp311.f1=(unsigned long)_tmp2FF;({void*_tmp30F[1]={& _tmp311};Cyc_aprintf(({const char*_tmp310="%d";_tag_dyneither(_tmp310,sizeof(char),3);}),_tag_dyneither(_tmp30F,sizeof(void*),1));});}));default: _LL26E: _tmp2FE=((_tmp2F8.Int_c).val).f2;_LL26F:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp314;_tmp314.tag=1;_tmp314.f1=(unsigned int)_tmp2FE;({void*_tmp312[1]={& _tmp314};Cyc_aprintf(({const char*_tmp313="%u";_tag_dyneither(_tmp313,sizeof(char),3);}),_tag_dyneither(_tmp312,sizeof(void*),1));});}));}case 6: _LL270: _tmp2FC=((_tmp2F8.LongLong_c).val).f1;_tmp2FD=((_tmp2F8.LongLong_c).val).f2;_LL271:
# 1351
 return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)_tmp2FD));case 7: _LL272: _tmp2FB=((_tmp2F8.Float_c).val).f1;_LL273:
 return Cyc_PP_text(_tmp2FB);case 1: _LL274: _LL275:
 return Cyc_PP_text(({const char*_tmp315="NULL";_tag_dyneither(_tmp315,sizeof(char),5);}));case 8: _LL276: _tmp2FA=(_tmp2F8.String_c).val;_LL277:
# 1355
 return({struct Cyc_PP_Doc*_tmp316[3];_tmp316[2]=Cyc_PP_text(({const char*_tmp318="\"";_tag_dyneither(_tmp318,sizeof(char),2);}));_tmp316[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp2FA));_tmp316[0]=Cyc_PP_text(({const char*_tmp317="\"";_tag_dyneither(_tmp317,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp316,sizeof(struct Cyc_PP_Doc*),3));});default: _LL278: _tmp2F9=(_tmp2F8.Wstring_c).val;_LL279:
# 1357
 return({struct Cyc_PP_Doc*_tmp319[3];_tmp319[2]=Cyc_PP_text(({const char*_tmp31B="\"";_tag_dyneither(_tmp31B,sizeof(char),2);}));_tmp319[1]=Cyc_PP_text(_tmp2F9);_tmp319[0]=Cyc_PP_text(({const char*_tmp31A="L\"";_tag_dyneither(_tmp31A,sizeof(char),3);}));Cyc_PP_cat(_tag_dyneither(_tmp319,sizeof(struct Cyc_PP_Doc*),3));});}_LL263:;}
# 1361
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if(p == Cyc_Absyn_Numelts){
if(es == 0  || es->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C[0]=({struct Cyc_Core_Failure_exn_struct _tmp31D;_tmp31D.tag=Cyc_Core_Failure;_tmp31D.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp320;_tmp320.tag=0;_tmp320.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));({void*_tmp31E[1]={& _tmp320};Cyc_aprintf(({const char*_tmp31F="Absynpp::primapp2doc Numelts: %s with bad args";_tag_dyneither(_tmp31F,sizeof(char),47);}),_tag_dyneither(_tmp31E,sizeof(void*),1));});});_tmp31D;});_tmp31C;}));
# 1367
return({struct Cyc_PP_Doc*_tmp321[3];_tmp321[2]=Cyc_PP_text(({const char*_tmp323=")";_tag_dyneither(_tmp323,sizeof(char),2);}));_tmp321[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_tmp321[0]=Cyc_PP_text(({const char*_tmp322="numelts(";_tag_dyneither(_tmp322,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp321,sizeof(struct Cyc_PP_Doc*),3));});}else{
# 1369
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp324=_cycalloc(sizeof(*_tmp324));_tmp324[0]=({struct Cyc_Core_Failure_exn_struct _tmp325;_tmp325.tag=Cyc_Core_Failure;_tmp325.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp328;_tmp328.tag=0;_tmp328.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));({void*_tmp326[1]={& _tmp328};Cyc_aprintf(({const char*_tmp327="Absynpp::primapp2doc: %s with no args";_tag_dyneither(_tmp327,sizeof(char),38);}),_tag_dyneither(_tmp326,sizeof(void*),1));});});_tmp325;});_tmp324;}));else{
# 1373
if(ds->tl == 0)
return({struct Cyc_PP_Doc*_tmp329[3];_tmp329[2]=(struct Cyc_PP_Doc*)ds->hd;_tmp329[1]=Cyc_PP_text(({const char*_tmp32A=" ";_tag_dyneither(_tmp32A,sizeof(char),2);}));_tmp329[0]=ps;Cyc_PP_cat(_tag_dyneither(_tmp329,sizeof(struct Cyc_PP_Doc*),3));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp32B=_cycalloc(sizeof(*_tmp32B));_tmp32B[0]=({struct Cyc_Core_Failure_exn_struct _tmp32C;_tmp32C.tag=Cyc_Core_Failure;_tmp32C.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp32F;_tmp32F.tag=0;_tmp32F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));({void*_tmp32D[1]={& _tmp32F};Cyc_aprintf(({const char*_tmp32E="Absynpp::primapp2doc: %s with more than 2 args";_tag_dyneither(_tmp32E,sizeof(char),47);}),_tag_dyneither(_tmp32D,sizeof(void*),1));});});_tmp32C;});_tmp32B;}));else{
# 1379
return({struct Cyc_PP_Doc*_tmp330[5];_tmp330[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd;_tmp330[3]=Cyc_PP_text(({const char*_tmp332=" ";_tag_dyneither(_tmp332,sizeof(char),2);}));_tmp330[2]=ps;_tmp330[1]=Cyc_PP_text(({const char*_tmp331=" ";_tag_dyneither(_tmp331,sizeof(char),2);}));_tmp330[0]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_dyneither(_tmp330,sizeof(struct Cyc_PP_Doc*),5));});}}}}}
# 1383
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _tmp333=p;switch(_tmp333){case Cyc_Absyn_Plus: _LL27B: _LL27C:
 return({const char*_tmp334="+";_tag_dyneither(_tmp334,sizeof(char),2);});case Cyc_Absyn_Times: _LL27D: _LL27E:
 return({const char*_tmp335="*";_tag_dyneither(_tmp335,sizeof(char),2);});case Cyc_Absyn_Minus: _LL27F: _LL280:
 return({const char*_tmp336="-";_tag_dyneither(_tmp336,sizeof(char),2);});case Cyc_Absyn_Div: _LL281: _LL282:
 return({const char*_tmp337="/";_tag_dyneither(_tmp337,sizeof(char),2);});case Cyc_Absyn_Mod: _LL283: _LL284:
 return Cyc_Absynpp_print_for_cycdoc?({const char*_tmp338="\\%";_tag_dyneither(_tmp338,sizeof(char),3);}):({const char*_tmp339="%";_tag_dyneither(_tmp339,sizeof(char),2);});case Cyc_Absyn_Eq: _LL285: _LL286:
 return({const char*_tmp33A="==";_tag_dyneither(_tmp33A,sizeof(char),3);});case Cyc_Absyn_Neq: _LL287: _LL288:
 return({const char*_tmp33B="!=";_tag_dyneither(_tmp33B,sizeof(char),3);});case Cyc_Absyn_Gt: _LL289: _LL28A:
 return({const char*_tmp33C=">";_tag_dyneither(_tmp33C,sizeof(char),2);});case Cyc_Absyn_Lt: _LL28B: _LL28C:
 return({const char*_tmp33D="<";_tag_dyneither(_tmp33D,sizeof(char),2);});case Cyc_Absyn_Gte: _LL28D: _LL28E:
 return({const char*_tmp33E=">=";_tag_dyneither(_tmp33E,sizeof(char),3);});case Cyc_Absyn_Lte: _LL28F: _LL290:
 return({const char*_tmp33F="<=";_tag_dyneither(_tmp33F,sizeof(char),3);});case Cyc_Absyn_Not: _LL291: _LL292:
 return({const char*_tmp340="!";_tag_dyneither(_tmp340,sizeof(char),2);});case Cyc_Absyn_Bitnot: _LL293: _LL294:
 return({const char*_tmp341="~";_tag_dyneither(_tmp341,sizeof(char),2);});case Cyc_Absyn_Bitand: _LL295: _LL296:
 return({const char*_tmp342="&";_tag_dyneither(_tmp342,sizeof(char),2);});case Cyc_Absyn_Bitor: _LL297: _LL298:
 return({const char*_tmp343="|";_tag_dyneither(_tmp343,sizeof(char),2);});case Cyc_Absyn_Bitxor: _LL299: _LL29A:
 return({const char*_tmp344="^";_tag_dyneither(_tmp344,sizeof(char),2);});case Cyc_Absyn_Bitlshift: _LL29B: _LL29C:
 return({const char*_tmp345="<<";_tag_dyneither(_tmp345,sizeof(char),3);});case Cyc_Absyn_Bitlrshift: _LL29D: _LL29E:
 return({const char*_tmp346=">>";_tag_dyneither(_tmp346,sizeof(char),3);});case Cyc_Absyn_Bitarshift: _LL29F: _LL2A0:
 return({const char*_tmp347=">>>";_tag_dyneither(_tmp347,sizeof(char),4);});default: _LL2A1: _LL2A2:
 return({const char*_tmp348="numelts";_tag_dyneither(_tmp348,sizeof(char),8);});}_LL27A:;}
# 1408
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1412
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp349=s->r;void*_tmp34A=_tmp349;if(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp34A)->tag == 12){_LL2A4: _LL2A5:
 return 1;}else{_LL2A6: _LL2A7:
 return 0;}_LL2A3:;}
# 1419
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp){
struct Cyc_PP_Doc*s;
{void*_tmp34B=st->r;void*_tmp34C=_tmp34B;struct Cyc_Absyn_Exp*_tmp34D;struct Cyc_Absyn_Stmt*_tmp34E;struct Cyc_List_List*_tmp34F;struct Cyc_Absyn_Stmt*_tmp350;struct Cyc_Absyn_Exp*_tmp351;struct _dyneither_ptr*_tmp352;struct Cyc_Absyn_Stmt*_tmp353;struct Cyc_Absyn_Decl*_tmp354;struct Cyc_Absyn_Stmt*_tmp355;struct Cyc_List_List*_tmp356;struct Cyc_Absyn_Exp*_tmp357;struct Cyc_List_List*_tmp358;struct Cyc_Absyn_Exp*_tmp359;struct Cyc_Absyn_Exp*_tmp35A;struct Cyc_Absyn_Exp*_tmp35B;struct Cyc_Absyn_Stmt*_tmp35C;struct _dyneither_ptr*_tmp35D;struct Cyc_Absyn_Exp*_tmp35E;struct Cyc_Absyn_Stmt*_tmp35F;struct Cyc_Absyn_Exp*_tmp360;struct Cyc_Absyn_Stmt*_tmp361;struct Cyc_Absyn_Stmt*_tmp362;struct Cyc_Absyn_Exp*_tmp363;struct Cyc_Absyn_Stmt*_tmp364;struct Cyc_Absyn_Stmt*_tmp365;struct Cyc_Absyn_Exp*_tmp366;switch(*((int*)_tmp34C)){case 0: _LL2A9: _LL2AA:
 s=Cyc_PP_text(({const char*_tmp367=";";_tag_dyneither(_tmp367,sizeof(char),2);}));goto _LL2A8;case 1: _LL2AB: _tmp366=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_LL2AC:
 s=({struct Cyc_PP_Doc*_tmp368[2];_tmp368[1]=Cyc_PP_text(({const char*_tmp369=";";_tag_dyneither(_tmp369,sizeof(char),2);}));_tmp368[0]=Cyc_Absynpp_exp2doc(_tmp366);Cyc_PP_cat(_tag_dyneither(_tmp368,sizeof(struct Cyc_PP_Doc*),2));});goto _LL2A8;case 2: _LL2AD: _tmp364=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp365=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL2AE:
# 1425
 if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp364))
s=({struct Cyc_PP_Doc*_tmp36A[7];_tmp36A[6]=
# 1433
Cyc_Absynpp_is_declaration(_tmp365)?
stmtexp?({struct Cyc_PP_Doc*_tmp36B[7];_tmp36B[6]=
# 1438
Cyc_PP_line_doc();_tmp36B[5]=
# 1437
Cyc_PP_text(({const char*_tmp36D=");";_tag_dyneither(_tmp36D,sizeof(char),3);}));_tmp36B[4]=Cyc_Absynpp_rb();_tmp36B[3]=
# 1436
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp365,stmtexp));_tmp36B[2]=
# 1435
Cyc_PP_blank_doc();_tmp36B[1]=Cyc_Absynpp_lb();_tmp36B[0]=Cyc_PP_text(({const char*_tmp36C="(";_tag_dyneither(_tmp36C,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp36B,sizeof(struct Cyc_PP_Doc*),7));}):({struct Cyc_PP_Doc*_tmp36E[5];_tmp36E[4]=
# 1442
Cyc_PP_line_doc();_tmp36E[3]=
# 1441
Cyc_Absynpp_rb();_tmp36E[2]=
# 1440
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp365,stmtexp));_tmp36E[1]=
# 1439
Cyc_PP_blank_doc();_tmp36E[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dyneither(_tmp36E,sizeof(struct Cyc_PP_Doc*),5));}):
# 1443
 Cyc_Absynpp_stmt2doc(_tmp365,stmtexp);_tmp36A[5]=
# 1432
Cyc_PP_line_doc();_tmp36A[4]=
# 1431
Cyc_Absynpp_rb();_tmp36A[3]=
# 1430
Cyc_PP_line_doc();_tmp36A[2]=
# 1429
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp364,0));_tmp36A[1]=
# 1428
Cyc_PP_blank_doc();_tmp36A[0]=
# 1427
Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dyneither(_tmp36A,sizeof(struct Cyc_PP_Doc*),7));});else{
# 1444
if(Cyc_Absynpp_is_declaration(_tmp365))
s=({struct Cyc_PP_Doc*_tmp36F[4];_tmp36F[3]=
# 1455
Cyc_PP_line_doc();_tmp36F[2]=
# 1447
stmtexp?({struct Cyc_PP_Doc*_tmp370[6];_tmp370[5]=
# 1450
Cyc_PP_text(({const char*_tmp372=");";_tag_dyneither(_tmp372,sizeof(char),3);}));_tmp370[4]=Cyc_Absynpp_rb();_tmp370[3]=
# 1449
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp365,stmtexp));_tmp370[2]=
# 1448
Cyc_PP_blank_doc();_tmp370[1]=Cyc_Absynpp_lb();_tmp370[0]=Cyc_PP_text(({const char*_tmp371="(";_tag_dyneither(_tmp371,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp370,sizeof(struct Cyc_PP_Doc*),6));}):({struct Cyc_PP_Doc*_tmp373[4];_tmp373[3]=
# 1454
Cyc_Absynpp_rb();_tmp373[2]=
# 1453
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp365,stmtexp));_tmp373[1]=
# 1452
Cyc_PP_blank_doc();_tmp373[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dyneither(_tmp373,sizeof(struct Cyc_PP_Doc*),4));});_tmp36F[1]=
# 1446
Cyc_PP_line_doc();_tmp36F[0]=
# 1445
Cyc_Absynpp_stmt2doc(_tmp364,0);Cyc_PP_cat(_tag_dyneither(_tmp36F,sizeof(struct Cyc_PP_Doc*),4));});else{
# 1457
s=({struct Cyc_PP_Doc*_tmp374[3];_tmp374[2]=Cyc_Absynpp_stmt2doc(_tmp365,stmtexp);_tmp374[1]=Cyc_PP_line_doc();_tmp374[0]=Cyc_Absynpp_stmt2doc(_tmp364,0);Cyc_PP_cat(_tag_dyneither(_tmp374,sizeof(struct Cyc_PP_Doc*),3));});}}}else{
# 1460
s=({struct Cyc_PP_Doc*_tmp375[3];_tmp375[2]=Cyc_Absynpp_stmt2doc(_tmp365,stmtexp);_tmp375[1]=Cyc_PP_line_doc();_tmp375[0]=Cyc_Absynpp_stmt2doc(_tmp364,0);Cyc_PP_cat(_tag_dyneither(_tmp375,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LL2A8;case 3: _LL2AF: _tmp363=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_LL2B0:
# 1463
 if(_tmp363 == 0)
s=Cyc_PP_text(({const char*_tmp376="return;";_tag_dyneither(_tmp376,sizeof(char),8);}));else{
# 1466
s=({struct Cyc_PP_Doc*_tmp377[3];_tmp377[2]=
# 1468
Cyc_PP_text(({const char*_tmp379=";";_tag_dyneither(_tmp379,sizeof(char),2);}));_tmp377[1]=
# 1467
_tmp363 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(_tmp363);_tmp377[0]=
# 1466
Cyc_PP_text(({const char*_tmp378="return ";_tag_dyneither(_tmp378,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp377,sizeof(struct Cyc_PP_Doc*),3));});}
# 1469
goto _LL2A8;case 4: _LL2B1: _tmp360=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp361=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_tmp362=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp34C)->f3;_LL2B2: {
# 1471
int print_else;
{void*_tmp37A=_tmp362->r;void*_tmp37B=_tmp37A;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp37B)->tag == 0){_LL2CE: _LL2CF:
 print_else=0;goto _LL2CD;}else{_LL2D0: _LL2D1:
 print_else=1;goto _LL2CD;}_LL2CD:;}
# 1476
s=({struct Cyc_PP_Doc*_tmp37C[8];_tmp37C[7]=
# 1483
print_else?({struct Cyc_PP_Doc*_tmp380[6];_tmp380[5]=
# 1489
Cyc_Absynpp_rb();_tmp380[4]=
# 1488
Cyc_PP_line_doc();_tmp380[3]=
# 1487
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp382[2];_tmp382[1]=Cyc_Absynpp_stmt2doc(_tmp362,0);_tmp382[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp382,sizeof(struct Cyc_PP_Doc*),2));}));_tmp380[2]=
# 1486
Cyc_Absynpp_lb();_tmp380[1]=
# 1485
Cyc_PP_text(({const char*_tmp381="else ";_tag_dyneither(_tmp381,sizeof(char),6);}));_tmp380[0]=
# 1484
Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp380,sizeof(struct Cyc_PP_Doc*),6));}):
# 1490
 Cyc_PP_nil_doc();_tmp37C[6]=
# 1482
Cyc_Absynpp_rb();_tmp37C[5]=
# 1481
Cyc_PP_line_doc();_tmp37C[4]=
# 1480
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp37F[2];_tmp37F[1]=Cyc_Absynpp_stmt2doc(_tmp361,0);_tmp37F[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp37F,sizeof(struct Cyc_PP_Doc*),2));}));_tmp37C[3]=
# 1479
Cyc_Absynpp_lb();_tmp37C[2]=
# 1478
Cyc_PP_text(({const char*_tmp37E=") ";_tag_dyneither(_tmp37E,sizeof(char),3);}));_tmp37C[1]=
# 1477
Cyc_Absynpp_exp2doc(_tmp360);_tmp37C[0]=
# 1476
Cyc_PP_text(({const char*_tmp37D="if (";_tag_dyneither(_tmp37D,sizeof(char),5);}));Cyc_PP_cat(_tag_dyneither(_tmp37C,sizeof(struct Cyc_PP_Doc*),8));});
# 1491
goto _LL2A8;}case 5: _LL2B3: _tmp35E=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1).f1;_tmp35F=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL2B4:
# 1493
 s=({struct Cyc_PP_Doc*_tmp383[7];_tmp383[6]=
# 1499
Cyc_Absynpp_rb();_tmp383[5]=
# 1498
Cyc_PP_line_doc();_tmp383[4]=
# 1497
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp386[2];_tmp386[1]=Cyc_Absynpp_stmt2doc(_tmp35F,0);_tmp386[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp386,sizeof(struct Cyc_PP_Doc*),2));}));_tmp383[3]=
# 1496
Cyc_Absynpp_lb();_tmp383[2]=
# 1495
Cyc_PP_text(({const char*_tmp385=") ";_tag_dyneither(_tmp385,sizeof(char),3);}));_tmp383[1]=
# 1494
Cyc_Absynpp_exp2doc(_tmp35E);_tmp383[0]=
# 1493
Cyc_PP_text(({const char*_tmp384="while (";_tag_dyneither(_tmp384,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp383,sizeof(struct Cyc_PP_Doc*),7));});
# 1500
goto _LL2A8;case 6: _LL2B5: _LL2B6:
 s=Cyc_PP_text(({const char*_tmp387="break;";_tag_dyneither(_tmp387,sizeof(char),7);}));goto _LL2A8;case 7: _LL2B7: _LL2B8:
 s=Cyc_PP_text(({const char*_tmp388="continue;";_tag_dyneither(_tmp388,sizeof(char),10);}));goto _LL2A8;case 8: _LL2B9: _tmp35D=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_LL2BA:
 s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp38B;_tmp38B.tag=0;_tmp38B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp35D);({void*_tmp389[1]={& _tmp38B};Cyc_aprintf(({const char*_tmp38A="goto %s;";_tag_dyneither(_tmp38A,sizeof(char),9);}),_tag_dyneither(_tmp389,sizeof(void*),1));});}));goto _LL2A8;case 9: _LL2BB: _tmp359=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp35A=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2).f1;_tmp35B=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34C)->f3).f1;_tmp35C=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34C)->f4;_LL2BC:
# 1505
 s=({struct Cyc_PP_Doc*_tmp38C[11];_tmp38C[10]=
# 1515
Cyc_Absynpp_rb();_tmp38C[9]=
# 1514
Cyc_PP_line_doc();_tmp38C[8]=
# 1513
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp391[2];_tmp391[1]=Cyc_Absynpp_stmt2doc(_tmp35C,0);_tmp391[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp391,sizeof(struct Cyc_PP_Doc*),2));}));_tmp38C[7]=
# 1512
Cyc_Absynpp_lb();_tmp38C[6]=
# 1511
Cyc_PP_text(({const char*_tmp390=") ";_tag_dyneither(_tmp390,sizeof(char),3);}));_tmp38C[5]=
# 1510
Cyc_Absynpp_exp2doc(_tmp35B);_tmp38C[4]=
# 1509
Cyc_PP_text(({const char*_tmp38F="; ";_tag_dyneither(_tmp38F,sizeof(char),3);}));_tmp38C[3]=
# 1508
Cyc_Absynpp_exp2doc(_tmp35A);_tmp38C[2]=
# 1507
Cyc_PP_text(({const char*_tmp38E="; ";_tag_dyneither(_tmp38E,sizeof(char),3);}));_tmp38C[1]=
# 1506
Cyc_Absynpp_exp2doc(_tmp359);_tmp38C[0]=
# 1505
Cyc_PP_text(({const char*_tmp38D="for(";_tag_dyneither(_tmp38D,sizeof(char),5);}));Cyc_PP_cat(_tag_dyneither(_tmp38C,sizeof(struct Cyc_PP_Doc*),11));});
# 1516
goto _LL2A8;case 10: _LL2BD: _tmp357=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp358=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL2BE:
# 1518
 s=({struct Cyc_PP_Doc*_tmp392[8];_tmp392[7]=
# 1525
Cyc_Absynpp_rb();_tmp392[6]=
# 1524
Cyc_PP_line_doc();_tmp392[5]=
# 1523
Cyc_Absynpp_switchclauses2doc(_tmp358);_tmp392[4]=
# 1522
Cyc_PP_line_doc();_tmp392[3]=
# 1521
Cyc_Absynpp_lb();_tmp392[2]=
# 1520
Cyc_PP_text(({const char*_tmp394=") ";_tag_dyneither(_tmp394,sizeof(char),3);}));_tmp392[1]=
# 1519
Cyc_Absynpp_exp2doc(_tmp357);_tmp392[0]=
# 1518
Cyc_PP_text(({const char*_tmp393="switch (";_tag_dyneither(_tmp393,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp392,sizeof(struct Cyc_PP_Doc*),8));});
# 1526
goto _LL2A8;case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1 == 0){_LL2BF: _LL2C0:
 s=Cyc_PP_text(({const char*_tmp395="fallthru;";_tag_dyneither(_tmp395,sizeof(char),10);}));goto _LL2A8;}else{_LL2C1: _tmp356=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_LL2C2:
# 1529
 s=({struct Cyc_PP_Doc*_tmp396[3];_tmp396[2]=Cyc_PP_text(({const char*_tmp398=");";_tag_dyneither(_tmp398,sizeof(char),3);}));_tmp396[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp356);_tmp396[0]=Cyc_PP_text(({const char*_tmp397="fallthru(";_tag_dyneither(_tmp397,sizeof(char),10);}));Cyc_PP_cat(_tag_dyneither(_tmp396,sizeof(struct Cyc_PP_Doc*),3));});goto _LL2A8;}case 12: _LL2C3: _tmp354=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp355=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL2C4:
# 1531
 s=({struct Cyc_PP_Doc*_tmp399[3];_tmp399[2]=
# 1533
Cyc_Absynpp_stmt2doc(_tmp355,stmtexp);_tmp399[1]=
# 1532
Cyc_PP_line_doc();_tmp399[0]=
# 1531
Cyc_Absynpp_decl2doc(_tmp354);Cyc_PP_cat(_tag_dyneither(_tmp399,sizeof(struct Cyc_PP_Doc*),3));});
# 1534
goto _LL2A8;case 13: _LL2C5: _tmp352=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp353=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL2C6:
# 1536
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp353)){
if(stmtexp)
s=({struct Cyc_PP_Doc*_tmp39A[8];_tmp39A[7]=
# 1544
Cyc_PP_text(({const char*_tmp39C=");";_tag_dyneither(_tmp39C,sizeof(char),3);}));_tmp39A[6]=Cyc_Absynpp_rb();_tmp39A[5]=
# 1543
Cyc_PP_line_doc();_tmp39A[4]=
# 1542
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp353,1));_tmp39A[3]=
# 1541
Cyc_PP_line_doc();_tmp39A[2]=
# 1540
Cyc_Absynpp_lb();_tmp39A[1]=
# 1539
Cyc_PP_text(({const char*_tmp39B=": (";_tag_dyneither(_tmp39B,sizeof(char),4);}));_tmp39A[0]=
# 1538
Cyc_PP_textptr(_tmp352);Cyc_PP_cat(_tag_dyneither(_tmp39A,sizeof(struct Cyc_PP_Doc*),8));});else{
# 1546
s=({struct Cyc_PP_Doc*_tmp39D[7];_tmp39D[6]=
# 1552
Cyc_Absynpp_rb();_tmp39D[5]=
# 1551
Cyc_PP_line_doc();_tmp39D[4]=
# 1550
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp353,0));_tmp39D[3]=
# 1549
Cyc_PP_line_doc();_tmp39D[2]=
# 1548
Cyc_Absynpp_lb();_tmp39D[1]=
# 1547
Cyc_PP_text(({const char*_tmp39E=": ";_tag_dyneither(_tmp39E,sizeof(char),3);}));_tmp39D[0]=
# 1546
Cyc_PP_textptr(_tmp352);Cyc_PP_cat(_tag_dyneither(_tmp39D,sizeof(struct Cyc_PP_Doc*),7));});}}else{
# 1554
s=({struct Cyc_PP_Doc*_tmp39F[3];_tmp39F[2]=Cyc_Absynpp_stmt2doc(_tmp353,stmtexp);_tmp39F[1]=Cyc_PP_text(({const char*_tmp3A0=": ";_tag_dyneither(_tmp3A0,sizeof(char),3);}));_tmp39F[0]=Cyc_PP_textptr(_tmp352);Cyc_PP_cat(_tag_dyneither(_tmp39F,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LL2A8;case 14: _LL2C7: _tmp350=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp351=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2).f1;_LL2C8:
# 1557
 s=({struct Cyc_PP_Doc*_tmp3A1[9];_tmp3A1[8]=
# 1565
Cyc_PP_text(({const char*_tmp3A4=");";_tag_dyneither(_tmp3A4,sizeof(char),3);}));_tmp3A1[7]=
# 1564
Cyc_Absynpp_exp2doc(_tmp351);_tmp3A1[6]=
# 1563
Cyc_PP_text(({const char*_tmp3A3=" while (";_tag_dyneither(_tmp3A3,sizeof(char),9);}));_tmp3A1[5]=
# 1562
Cyc_Absynpp_rb();_tmp3A1[4]=
# 1561
Cyc_PP_line_doc();_tmp3A1[3]=
# 1560
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp350,0));_tmp3A1[2]=
# 1559
Cyc_PP_line_doc();_tmp3A1[1]=
# 1558
Cyc_Absynpp_lb();_tmp3A1[0]=
# 1557
Cyc_PP_text(({const char*_tmp3A2="do ";_tag_dyneither(_tmp3A2,sizeof(char),4);}));Cyc_PP_cat(_tag_dyneither(_tmp3A1,sizeof(struct Cyc_PP_Doc*),9));});
# 1566
goto _LL2A8;case 15: _LL2C9: _tmp34E=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp34F=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL2CA:
# 1568
 s=({struct Cyc_PP_Doc*_tmp3A5[12];_tmp3A5[11]=
# 1579
Cyc_Absynpp_rb();_tmp3A5[10]=
# 1578
Cyc_PP_line_doc();_tmp3A5[9]=
# 1577
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp34F));_tmp3A5[8]=
# 1576
Cyc_PP_line_doc();_tmp3A5[7]=
# 1575
Cyc_Absynpp_lb();_tmp3A5[6]=
# 1574
Cyc_PP_text(({const char*_tmp3A7=" catch ";_tag_dyneither(_tmp3A7,sizeof(char),8);}));_tmp3A5[5]=
# 1573
Cyc_Absynpp_rb();_tmp3A5[4]=
# 1572
Cyc_PP_line_doc();_tmp3A5[3]=
# 1571
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp34E,0));_tmp3A5[2]=
# 1570
Cyc_PP_line_doc();_tmp3A5[1]=
# 1569
Cyc_Absynpp_lb();_tmp3A5[0]=
# 1568
Cyc_PP_text(({const char*_tmp3A6="try ";_tag_dyneither(_tmp3A6,sizeof(char),5);}));Cyc_PP_cat(_tag_dyneither(_tmp3A5,sizeof(struct Cyc_PP_Doc*),12));});
# 1580
goto _LL2A8;default: _LL2CB: _tmp34D=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_LL2CC:
# 1582
 s=({struct Cyc_PP_Doc*_tmp3A8[3];_tmp3A8[2]=Cyc_PP_text(({const char*_tmp3AA=");";_tag_dyneither(_tmp3AA,sizeof(char),3);}));_tmp3A8[1]=Cyc_Absynpp_exp2doc(_tmp34D);_tmp3A8[0]=Cyc_PP_text(({const char*_tmp3A9="reset_region(";_tag_dyneither(_tmp3A9,sizeof(char),14);}));Cyc_PP_cat(_tag_dyneither(_tmp3A8,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL2A8;}_LL2A8:;}
# 1585
return s;}
# 1588
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp3AB=p->r;void*_tmp3AC=_tmp3AB;struct Cyc_Absyn_Exp*_tmp3AD;struct Cyc_Absyn_Datatypefield*_tmp3AE;struct Cyc_List_List*_tmp3AF;int _tmp3B0;struct Cyc_Absyn_Datatypefield*_tmp3B1;struct Cyc_Absyn_Enumfield*_tmp3B2;struct Cyc_Absyn_Enumfield*_tmp3B3;struct Cyc_List_List*_tmp3B4;struct Cyc_List_List*_tmp3B5;int _tmp3B6;union Cyc_Absyn_AggrInfoU _tmp3B7;struct Cyc_List_List*_tmp3B8;struct Cyc_List_List*_tmp3B9;int _tmp3BA;struct _tuple0*_tmp3BB;struct Cyc_List_List*_tmp3BC;int _tmp3BD;struct _tuple0*_tmp3BE;struct Cyc_Absyn_Vardecl*_tmp3BF;struct Cyc_Absyn_Pat*_tmp3C0;struct Cyc_Absyn_Vardecl*_tmp3C1;struct Cyc_Absyn_Pat*_tmp3C2;struct Cyc_List_List*_tmp3C3;int _tmp3C4;struct Cyc_Absyn_Tvar*_tmp3C5;struct Cyc_Absyn_Vardecl*_tmp3C6;struct Cyc_Absyn_Tvar*_tmp3C7;struct Cyc_Absyn_Vardecl*_tmp3C8;struct Cyc_Absyn_Vardecl*_tmp3C9;struct Cyc_Absyn_Pat*_tmp3CA;struct Cyc_Absyn_Vardecl*_tmp3CB;struct _dyneither_ptr _tmp3CC;char _tmp3CD;enum Cyc_Absyn_Sign _tmp3CE;int _tmp3CF;switch(*((int*)_tmp3AC)){case 0: _LL2D3: _LL2D4:
 s=Cyc_PP_text(({const char*_tmp3D0="_";_tag_dyneither(_tmp3D0,sizeof(char),2);}));goto _LL2D2;case 9: _LL2D5: _LL2D6:
 s=Cyc_PP_text(({const char*_tmp3D1="NULL";_tag_dyneither(_tmp3D1,sizeof(char),5);}));goto _LL2D2;case 10: _LL2D7: _tmp3CE=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3CF=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2D8:
# 1594
 if(_tmp3CE != Cyc_Absyn_Unsigned)
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3D4;_tmp3D4.tag=1;_tmp3D4.f1=(unsigned long)_tmp3CF;({void*_tmp3D2[1]={& _tmp3D4};Cyc_aprintf(({const char*_tmp3D3="%d";_tag_dyneither(_tmp3D3,sizeof(char),3);}),_tag_dyneither(_tmp3D2,sizeof(void*),1));});}));else{
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3D7;_tmp3D7.tag=1;_tmp3D7.f1=(unsigned int)_tmp3CF;({void*_tmp3D5[1]={& _tmp3D7};Cyc_aprintf(({const char*_tmp3D6="%u";_tag_dyneither(_tmp3D6,sizeof(char),3);}),_tag_dyneither(_tmp3D5,sizeof(void*),1));});}));}
goto _LL2D2;case 11: _LL2D9: _tmp3CD=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2DA:
 s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3DA;_tmp3DA.tag=0;_tmp3DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp3CD));({void*_tmp3D8[1]={& _tmp3DA};Cyc_aprintf(({const char*_tmp3D9="'%s'";_tag_dyneither(_tmp3D9,sizeof(char),5);}),_tag_dyneither(_tmp3D8,sizeof(void*),1));});}));goto _LL2D2;case 12: _LL2DB: _tmp3CC=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2DC:
 s=Cyc_PP_text(_tmp3CC);goto _LL2D2;case 1: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2)->r)->tag == 0){_LL2DD: _tmp3CB=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2DE:
# 1601
 s=Cyc_Absynpp_qvar2doc(_tmp3CB->name);goto _LL2D2;}else{_LL2DF: _tmp3C9=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3CA=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2E0:
# 1603
 s=({struct Cyc_PP_Doc*_tmp3DB[3];_tmp3DB[2]=Cyc_Absynpp_pat2doc(_tmp3CA);_tmp3DB[1]=Cyc_PP_text(({const char*_tmp3DC=" as ";_tag_dyneither(_tmp3DC,sizeof(char),5);}));_tmp3DB[0]=Cyc_Absynpp_qvar2doc(_tmp3C9->name);Cyc_PP_cat(_tag_dyneither(_tmp3DB,sizeof(struct Cyc_PP_Doc*),3));});goto _LL2D2;}case 2: _LL2E1: _tmp3C7=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3C8=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2E2:
# 1605
 s=({struct Cyc_PP_Doc*_tmp3DD[5];_tmp3DD[4]=
Cyc_Absynpp_vardecl2doc(_tmp3C8);_tmp3DD[3]=
# 1605
Cyc_PP_text(({const char*_tmp3E0="> ";_tag_dyneither(_tmp3E0,sizeof(char),3);}));_tmp3DD[2]=Cyc_Absynpp_tvar2doc(_tmp3C7);_tmp3DD[1]=Cyc_PP_text(({const char*_tmp3DF=" <";_tag_dyneither(_tmp3DF,sizeof(char),3);}));_tmp3DD[0]=Cyc_PP_text(({const char*_tmp3DE="alias";_tag_dyneither(_tmp3DE,sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp3DD,sizeof(struct Cyc_PP_Doc*),5));});
# 1607
goto _LL2D2;case 4: _LL2E3: _tmp3C5=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3C6=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2E4:
# 1609
 s=({struct Cyc_PP_Doc*_tmp3E1[4];_tmp3E1[3]=Cyc_PP_text(({const char*_tmp3E3=">";_tag_dyneither(_tmp3E3,sizeof(char),2);}));_tmp3E1[2]=Cyc_Absynpp_tvar2doc(_tmp3C5);_tmp3E1[1]=Cyc_PP_text(({const char*_tmp3E2="<";_tag_dyneither(_tmp3E2,sizeof(char),2);}));_tmp3E1[0]=Cyc_Absynpp_qvar2doc(_tmp3C6->name);Cyc_PP_cat(_tag_dyneither(_tmp3E1,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL2D2;case 5: _LL2E5: _tmp3C3=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3C4=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2E6:
# 1612
 s=({struct Cyc_PP_Doc*_tmp3E4[4];_tmp3E4[3]=
_tmp3C4?Cyc_PP_text(({const char*_tmp3E7=", ...)";_tag_dyneither(_tmp3E7,sizeof(char),7);})): Cyc_PP_text(({const char*_tmp3E8=")";_tag_dyneither(_tmp3E8,sizeof(char),2);}));_tmp3E4[2]=
# 1612
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,({const char*_tmp3E6=",";_tag_dyneither(_tmp3E6,sizeof(char),2);}),_tmp3C3);_tmp3E4[1]=Cyc_PP_text(({const char*_tmp3E5="(";_tag_dyneither(_tmp3E5,sizeof(char),2);}));_tmp3E4[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dyneither(_tmp3E4,sizeof(struct Cyc_PP_Doc*),4));});
# 1614
goto _LL2D2;case 6: _LL2E7: _tmp3C2=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2E8:
# 1616
 s=({struct Cyc_PP_Doc*_tmp3E9[2];_tmp3E9[1]=Cyc_Absynpp_pat2doc(_tmp3C2);_tmp3E9[0]=Cyc_PP_text(({const char*_tmp3EA="&";_tag_dyneither(_tmp3EA,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp3E9,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL2D2;case 3: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2)->r)->tag == 0){_LL2E9: _tmp3C1=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2EA:
# 1619
 s=({struct Cyc_PP_Doc*_tmp3EB[2];_tmp3EB[1]=Cyc_Absynpp_qvar2doc(_tmp3C1->name);_tmp3EB[0]=Cyc_PP_text(({const char*_tmp3EC="*";_tag_dyneither(_tmp3EC,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp3EB,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL2D2;}else{_LL2EB: _tmp3BF=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3C0=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2EC:
# 1622
 s=({struct Cyc_PP_Doc*_tmp3ED[4];_tmp3ED[3]=Cyc_Absynpp_pat2doc(_tmp3C0);_tmp3ED[2]=Cyc_PP_text(({const char*_tmp3EF=" as ";_tag_dyneither(_tmp3EF,sizeof(char),5);}));_tmp3ED[1]=Cyc_Absynpp_qvar2doc(_tmp3BF->name);_tmp3ED[0]=Cyc_PP_text(({const char*_tmp3EE="*";_tag_dyneither(_tmp3EE,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp3ED,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL2D2;}case 15: _LL2ED: _tmp3BE=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2EE:
# 1625
 s=Cyc_Absynpp_qvar2doc(_tmp3BE);
goto _LL2D2;case 16: _LL2EF: _tmp3BB=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3BC=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_tmp3BD=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AC)->f3;_LL2F0: {
# 1628
struct _dyneither_ptr term=_tmp3BD?({const char*_tmp3F3=", ...)";_tag_dyneither(_tmp3F3,sizeof(char),7);}):({const char*_tmp3F4=")";_tag_dyneither(_tmp3F4,sizeof(char),2);});
s=({struct Cyc_PP_Doc*_tmp3F0[2];_tmp3F0[1]=Cyc_PP_group(({const char*_tmp3F1="(";_tag_dyneither(_tmp3F1,sizeof(char),2);}),term,({const char*_tmp3F2=",";_tag_dyneither(_tmp3F2,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp3BC));_tmp3F0[0]=Cyc_Absynpp_qvar2doc(_tmp3BB);Cyc_PP_cat(_tag_dyneither(_tmp3F0,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL2D2;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1 != 0){_LL2F1: _tmp3B7=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1)->aggr_info;_tmp3B8=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_tmp3B9=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f3;_tmp3BA=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f4;_LL2F2: {
# 1632
struct _dyneither_ptr term=_tmp3BA?({const char*_tmp3FE=", ...}";_tag_dyneither(_tmp3FE,sizeof(char),7);}):({const char*_tmp3FF="}";_tag_dyneither(_tmp3FF,sizeof(char),2);});
struct _tuple10 _tmp3F5=Cyc_Absyn_aggr_kinded_name(_tmp3B7);struct _tuple10 _tmp3F6=_tmp3F5;struct _tuple0*_tmp3F7;_LL300: _tmp3F7=_tmp3F6.f2;_LL301:;
s=({struct Cyc_PP_Doc*_tmp3F8[4];_tmp3F8[3]=
# 1636
Cyc_PP_group(({const char*_tmp3FC="";_tag_dyneither(_tmp3FC,sizeof(char),1);}),term,({const char*_tmp3FD=",";_tag_dyneither(_tmp3FD,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp3B9));_tmp3F8[2]=
# 1635
Cyc_PP_egroup(({const char*_tmp3F9="[";_tag_dyneither(_tmp3F9,sizeof(char),2);}),({const char*_tmp3FA="]";_tag_dyneither(_tmp3FA,sizeof(char),2);}),({const char*_tmp3FB=",";_tag_dyneither(_tmp3FB,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp3B8));_tmp3F8[1]=
# 1634
Cyc_Absynpp_lb();_tmp3F8[0]=Cyc_Absynpp_qvar2doc(_tmp3F7);Cyc_PP_cat(_tag_dyneither(_tmp3F8,sizeof(struct Cyc_PP_Doc*),4));});
# 1637
goto _LL2D2;}}else{_LL2F3: _tmp3B4=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_tmp3B5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f3;_tmp3B6=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f4;_LL2F4: {
# 1639
struct _dyneither_ptr term=_tmp3B6?({const char*_tmp406=", ...}";_tag_dyneither(_tmp406,sizeof(char),7);}):({const char*_tmp407="}";_tag_dyneither(_tmp407,sizeof(char),2);});
s=({struct Cyc_PP_Doc*_tmp400[3];_tmp400[2]=
# 1642
Cyc_PP_group(({const char*_tmp404="";_tag_dyneither(_tmp404,sizeof(char),1);}),term,({const char*_tmp405=",";_tag_dyneither(_tmp405,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp3B5));_tmp400[1]=
# 1641
Cyc_PP_egroup(({const char*_tmp401="[";_tag_dyneither(_tmp401,sizeof(char),2);}),({const char*_tmp402="]";_tag_dyneither(_tmp402,sizeof(char),2);}),({const char*_tmp403=",";_tag_dyneither(_tmp403,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp3B4));_tmp400[0]=
# 1640
Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dyneither(_tmp400,sizeof(struct Cyc_PP_Doc*),3));});
# 1643
goto _LL2D2;}}case 13: _LL2F5: _tmp3B3=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2F6:
 _tmp3B2=_tmp3B3;goto _LL2F8;case 14: _LL2F7: _tmp3B2=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2F8:
 s=Cyc_Absynpp_qvar2doc(_tmp3B2->name);goto _LL2D2;case 8: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AC)->f3 == 0){_LL2F9: _tmp3B1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2FA:
 s=Cyc_Absynpp_qvar2doc(_tmp3B1->name);goto _LL2D2;}else{_LL2FB: _tmp3AE=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_tmp3AF=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AC)->f3;_tmp3B0=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AC)->f4;_LL2FC: {
# 1648
struct _dyneither_ptr term=_tmp3B0?({const char*_tmp40B=", ...)";_tag_dyneither(_tmp40B,sizeof(char),7);}):({const char*_tmp40C=")";_tag_dyneither(_tmp40C,sizeof(char),2);});
s=({struct Cyc_PP_Doc*_tmp408[2];_tmp408[1]=Cyc_PP_egroup(({const char*_tmp409="(";_tag_dyneither(_tmp409,sizeof(char),2);}),term,({const char*_tmp40A=",";_tag_dyneither(_tmp40A,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp3AF));_tmp408[0]=Cyc_Absynpp_qvar2doc(_tmp3AE->name);Cyc_PP_cat(_tag_dyneither(_tmp408,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL2D2;}}default: _LL2FD: _tmp3AD=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2FE:
# 1652
 s=Cyc_Absynpp_exp2doc(_tmp3AD);goto _LL2D2;}_LL2D2:;}
# 1654
return s;}
# 1657
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
return({struct Cyc_PP_Doc*_tmp40D[2];_tmp40D[1]=
Cyc_Absynpp_pat2doc((*dp).f2);_tmp40D[0]=
# 1658
Cyc_PP_egroup(({const char*_tmp40E="";_tag_dyneither(_tmp40E,sizeof(char),1);}),({const char*_tmp40F="=";_tag_dyneither(_tmp40F,sizeof(char),2);}),({const char*_tmp410="=";_tag_dyneither(_tmp410,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));Cyc_PP_cat(_tag_dyneither(_tmp40D,sizeof(struct Cyc_PP_Doc*),2));});}
# 1662
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
return({struct Cyc_PP_Doc*_tmp411[2];_tmp411[1]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp413[2];_tmp413[1]=Cyc_Absynpp_stmt2doc(c->body,0);_tmp413[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp413,sizeof(struct Cyc_PP_Doc*),2));}));_tmp411[0]=
# 1664
Cyc_PP_text(({const char*_tmp412="default: ";_tag_dyneither(_tmp412,sizeof(char),10);}));Cyc_PP_cat(_tag_dyneither(_tmp411,sizeof(struct Cyc_PP_Doc*),2));});else{
# 1666
if(c->where_clause == 0)
return({struct Cyc_PP_Doc*_tmp414[4];_tmp414[3]=
# 1670
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp417[2];_tmp417[1]=Cyc_Absynpp_stmt2doc(c->body,0);_tmp417[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp417,sizeof(struct Cyc_PP_Doc*),2));}));_tmp414[2]=
# 1669
Cyc_PP_text(({const char*_tmp416=": ";_tag_dyneither(_tmp416,sizeof(char),3);}));_tmp414[1]=
# 1668
Cyc_Absynpp_pat2doc(c->pattern);_tmp414[0]=
# 1667
Cyc_PP_text(({const char*_tmp415="case ";_tag_dyneither(_tmp415,sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp414,sizeof(struct Cyc_PP_Doc*),4));});else{
# 1672
return({struct Cyc_PP_Doc*_tmp418[6];_tmp418[5]=
# 1677
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp41C[2];_tmp41C[1]=Cyc_Absynpp_stmt2doc(c->body,0);_tmp41C[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp41C,sizeof(struct Cyc_PP_Doc*),2));}));_tmp418[4]=
# 1676
Cyc_PP_text(({const char*_tmp41B=": ";_tag_dyneither(_tmp41B,sizeof(char),3);}));_tmp418[3]=
# 1675
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));_tmp418[2]=
# 1674
Cyc_PP_text(({const char*_tmp41A=" && ";_tag_dyneither(_tmp41A,sizeof(char),5);}));_tmp418[1]=
# 1673
Cyc_Absynpp_pat2doc(c->pattern);_tmp418[0]=
# 1672
Cyc_PP_text(({const char*_tmp419="case ";_tag_dyneither(_tmp419,sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp418,sizeof(struct Cyc_PP_Doc*),6));});}}}
# 1680
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,({const char*_tmp41D="";_tag_dyneither(_tmp41D,sizeof(char),1);}),cs);}
# 1684
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1688
return({struct Cyc_PP_Doc*_tmp41E[3];_tmp41E[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));_tmp41E[1]=Cyc_PP_text(({const char*_tmp41F=" = ";_tag_dyneither(_tmp41F,sizeof(char),4);}));_tmp41E[0]=Cyc_Absynpp_qvar2doc(f->name);Cyc_PP_cat(_tag_dyneither(_tmp41E,sizeof(struct Cyc_PP_Doc*),3));});}}
# 1691
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,({const char*_tmp420=",";_tag_dyneither(_tmp420,sizeof(char),2);}),fs);}
# 1695
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1699
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,({const char*_tmp421=",";_tag_dyneither(_tmp421,sizeof(char),2);}),vds);}
# 1703
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_Absyn_Vardecl*_tmp422=vd;enum Cyc_Absyn_Scope _tmp423;struct _tuple0*_tmp424;struct Cyc_Absyn_Tqual _tmp425;void*_tmp426;struct Cyc_Absyn_Exp*_tmp427;struct Cyc_List_List*_tmp428;_LL303: _tmp423=_tmp422->sc;_tmp424=_tmp422->name;_tmp425=_tmp422->tq;_tmp426=_tmp422->type;_tmp427=_tmp422->initializer;_tmp428=_tmp422->attributes;_LL304:;{
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp424);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp428);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp429=Cyc_Cyclone_c_compiler;if(_tmp429 == Cyc_Cyclone_Gcc_c){_LL306: _LL307:
 beforenamedoc=attsdoc;goto _LL305;}else{_LL308: _LL309:
# 1712
{void*_tmp42A=Cyc_Tcutil_compress(_tmp426);void*_tmp42B=_tmp42A;struct Cyc_List_List*_tmp42C;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp42B)->tag == 9){_LL30B: _tmp42C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp42B)->f1).attributes;_LL30C:
# 1714
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp42C);
goto _LL30A;}else{_LL30D: _LL30E:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL30A;}_LL30A:;}
# 1718
goto _LL305;}_LL305:;}{
# 1721
struct Cyc_PP_Doc*tmp_doc;
{enum Cyc_Cyclone_C_Compilers _tmp42D=Cyc_Cyclone_c_compiler;if(_tmp42D == Cyc_Cyclone_Gcc_c){_LL310: _LL311:
 tmp_doc=Cyc_PP_nil_doc();goto _LL30F;}else{_LL312: _LL313:
 tmp_doc=attsdoc;goto _LL30F;}_LL30F:;}
# 1726
s=({struct Cyc_PP_Doc*_tmp42E[5];_tmp42E[4]=
# 1733
Cyc_PP_text(({const char*_tmp433=";";_tag_dyneither(_tmp433,sizeof(char),2);}));_tmp42E[3]=
# 1730
_tmp427 == 0?
Cyc_PP_nil_doc():({struct Cyc_PP_Doc*_tmp431[2];_tmp431[1]=
Cyc_Absynpp_exp2doc(_tmp427);_tmp431[0]=Cyc_PP_text(({const char*_tmp432=" = ";_tag_dyneither(_tmp432,sizeof(char),4);}));Cyc_PP_cat(_tag_dyneither(_tmp431,sizeof(struct Cyc_PP_Doc*),2));});_tmp42E[2]=
# 1729
Cyc_Absynpp_tqtd2doc(_tmp425,_tmp426,({struct Cyc_Core_Opt*_tmp42F=_cycalloc(sizeof(*_tmp42F));_tmp42F->v=({struct Cyc_PP_Doc*_tmp430[2];_tmp430[1]=sn;_tmp430[0]=beforenamedoc;Cyc_PP_cat(_tag_dyneither(_tmp430,sizeof(struct Cyc_PP_Doc*),2));});_tmp42F;}));_tmp42E[1]=
# 1728
Cyc_Absynpp_scope2doc(_tmp423);_tmp42E[0]=tmp_doc;Cyc_PP_cat(_tag_dyneither(_tmp42E,sizeof(struct Cyc_PP_Doc*),5));});
# 1734
return s;};};}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1737
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple17 _tmp434=*x;struct _tuple17 _tmp435=_tmp434;struct _tuple0*_tmp436;_LL315: _tmp436=_tmp435.f2;_LL316:;
return Cyc_Absynpp_qvar2doc(_tmp436);}
# 1742
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0)
return({struct Cyc_PP_Doc*_tmp437[4];_tmp437[3]=
# 1747
Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp437[2]=
# 1746
Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp437[1]=
# 1745
Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp437[0]=
# 1744
Cyc_Absynpp_scope2doc(ad->sc);Cyc_PP_cat(_tag_dyneither(_tmp437,sizeof(struct Cyc_PP_Doc*),4));});else{
# 1749
return({struct Cyc_PP_Doc*_tmp438[12];_tmp438[11]=
# 1760
Cyc_Absynpp_atts2doc(ad->attributes);_tmp438[10]=
# 1759
Cyc_Absynpp_rb();_tmp438[9]=
# 1758
Cyc_PP_line_doc();_tmp438[8]=
# 1757
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp43B[2];_tmp43B[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_tmp43B[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp43B,sizeof(struct Cyc_PP_Doc*),2));}));_tmp438[7]=
# 1755
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():({struct Cyc_PP_Doc*_tmp439[2];_tmp439[1]=
Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);_tmp439[0]=Cyc_PP_text(({const char*_tmp43A=":";_tag_dyneither(_tmp43A,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp439,sizeof(struct Cyc_PP_Doc*),2));});_tmp438[6]=
# 1754
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);_tmp438[5]=
# 1753
Cyc_Absynpp_lb();_tmp438[4]=Cyc_PP_blank_doc();_tmp438[3]=
# 1752
Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp438[2]=
# 1751
Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp438[1]=
# 1750
Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp438[0]=
# 1749
Cyc_Absynpp_scope2doc(ad->sc);Cyc_PP_cat(_tag_dyneither(_tmp438,sizeof(struct Cyc_PP_Doc*),12));});}}
# 1763
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp43C=dd;enum Cyc_Absyn_Scope _tmp43D;struct _tuple0*_tmp43E;struct Cyc_List_List*_tmp43F;struct Cyc_Core_Opt*_tmp440;int _tmp441;_LL318: _tmp43D=_tmp43C->sc;_tmp43E=_tmp43C->name;_tmp43F=_tmp43C->tvs;_tmp440=_tmp43C->fields;_tmp441=_tmp43C->is_extensible;_LL319:;
if(_tmp440 == 0)
return({struct Cyc_PP_Doc*_tmp442[5];_tmp442[4]=
# 1770
Cyc_Absynpp_ktvars2doc(_tmp43F);_tmp442[3]=
# 1769
_tmp441?Cyc_Absynpp_qvar2bolddoc(_tmp43E): Cyc_Absynpp_typedef_name2bolddoc(_tmp43E);_tmp442[2]=
# 1768
Cyc_PP_text(({const char*_tmp444="datatype ";_tag_dyneither(_tmp444,sizeof(char),10);}));_tmp442[1]=
# 1767
_tmp441?Cyc_PP_text(({const char*_tmp443="@extensible ";_tag_dyneither(_tmp443,sizeof(char),13);})): Cyc_PP_blank_doc();_tmp442[0]=
# 1766
Cyc_Absynpp_scope2doc(_tmp43D);Cyc_PP_cat(_tag_dyneither(_tmp442,sizeof(struct Cyc_PP_Doc*),5));});else{
# 1772
return({struct Cyc_PP_Doc*_tmp445[10];_tmp445[9]=
# 1780
Cyc_Absynpp_rb();_tmp445[8]=
# 1779
Cyc_PP_line_doc();_tmp445[7]=
# 1778
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp448[2];_tmp448[1]=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp440->v);_tmp448[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp448,sizeof(struct Cyc_PP_Doc*),2));}));_tmp445[6]=
# 1777
Cyc_Absynpp_lb();_tmp445[5]=Cyc_PP_blank_doc();_tmp445[4]=
# 1776
Cyc_Absynpp_ktvars2doc(_tmp43F);_tmp445[3]=
# 1775
_tmp441?Cyc_Absynpp_qvar2bolddoc(_tmp43E): Cyc_Absynpp_typedef_name2bolddoc(_tmp43E);_tmp445[2]=
# 1774
Cyc_PP_text(({const char*_tmp447="datatype ";_tag_dyneither(_tmp447,sizeof(char),10);}));_tmp445[1]=
# 1773
_tmp441?Cyc_PP_text(({const char*_tmp446="@extensible ";_tag_dyneither(_tmp446,sizeof(char),13);})): Cyc_PP_blank_doc();_tmp445[0]=
# 1772
Cyc_Absynpp_scope2doc(_tmp43D);Cyc_PP_cat(_tag_dyneither(_tmp445,sizeof(struct Cyc_PP_Doc*),10));});}}
# 1783
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp449=ed;enum Cyc_Absyn_Scope _tmp44A;struct _tuple0*_tmp44B;struct Cyc_Core_Opt*_tmp44C;_LL31B: _tmp44A=_tmp449->sc;_tmp44B=_tmp449->name;_tmp44C=_tmp449->fields;_LL31C:;
if(_tmp44C == 0)
return({struct Cyc_PP_Doc*_tmp44D[3];_tmp44D[2]=
# 1788
Cyc_Absynpp_typedef_name2bolddoc(_tmp44B);_tmp44D[1]=
# 1787
Cyc_PP_text(({const char*_tmp44E="enum ";_tag_dyneither(_tmp44E,sizeof(char),6);}));_tmp44D[0]=
# 1786
Cyc_Absynpp_scope2doc(_tmp44A);Cyc_PP_cat(_tag_dyneither(_tmp44D,sizeof(struct Cyc_PP_Doc*),3));});else{
# 1791
return({struct Cyc_PP_Doc*_tmp44F[8];_tmp44F[7]=
# 1797
Cyc_Absynpp_rb();_tmp44F[6]=
# 1796
Cyc_PP_line_doc();_tmp44F[5]=
# 1795
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp451[2];_tmp451[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp44C->v);_tmp451[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp451,sizeof(struct Cyc_PP_Doc*),2));}));_tmp44F[4]=
# 1794
Cyc_Absynpp_lb();_tmp44F[3]=Cyc_PP_blank_doc();_tmp44F[2]=
# 1793
Cyc_Absynpp_qvar2bolddoc(_tmp44B);_tmp44F[1]=
# 1792
Cyc_PP_text(({const char*_tmp450="enum ";_tag_dyneither(_tmp450,sizeof(char),6);}));_tmp44F[0]=
# 1791
Cyc_Absynpp_scope2doc(_tmp44A);Cyc_PP_cat(_tag_dyneither(_tmp44F,sizeof(struct Cyc_PP_Doc*),8));});}}
# 1800
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp452=d->r;void*_tmp453=_tmp452;struct Cyc_List_List*_tmp454;struct Cyc_List_List*_tmp455;struct Cyc_List_List*_tmp456;struct _tuple0*_tmp457;struct Cyc_List_List*_tmp458;struct _dyneither_ptr*_tmp459;struct Cyc_List_List*_tmp45A;struct Cyc_Absyn_Typedefdecl*_tmp45B;struct Cyc_Absyn_Enumdecl*_tmp45C;struct Cyc_List_List*_tmp45D;struct Cyc_Absyn_Pat*_tmp45E;struct Cyc_Absyn_Exp*_tmp45F;struct Cyc_Absyn_Datatypedecl*_tmp460;struct Cyc_Absyn_Tvar*_tmp461;struct Cyc_Absyn_Vardecl*_tmp462;int _tmp463;struct Cyc_Absyn_Exp*_tmp464;struct Cyc_Absyn_Vardecl*_tmp465;struct Cyc_Absyn_Aggrdecl*_tmp466;struct Cyc_Absyn_Fndecl*_tmp467;switch(*((int*)_tmp453)){case 1: _LL31E: _tmp467=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL31F: {
# 1804
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp473=_cycalloc(sizeof(*_tmp473));_tmp473[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp474;_tmp474.tag=9;_tmp474.f1=({struct Cyc_Absyn_FnInfo _tmp475;_tmp475.tvars=_tmp467->tvs;_tmp475.effect=_tmp467->effect;_tmp475.ret_tqual=_tmp467->ret_tqual;_tmp475.ret_typ=_tmp467->ret_type;_tmp475.args=
# 1808
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp467->args);_tmp475.c_varargs=_tmp467->c_varargs;_tmp475.cyc_varargs=_tmp467->cyc_varargs;_tmp475.rgn_po=_tmp467->rgn_po;_tmp475.attributes=0;_tmp475.requires_clause=_tmp467->requires_clause;_tmp475.requires_relns=_tmp467->requires_relns;_tmp475.ensures_clause=_tmp467->ensures_clause;_tmp475.ensures_relns=_tmp467->ensures_relns;_tmp475;});_tmp474;});_tmp473;});
# 1814
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp467->attributes);
struct Cyc_PP_Doc*inlinedoc;
if(_tmp467->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp468=Cyc_Cyclone_c_compiler;if(_tmp468 == Cyc_Cyclone_Gcc_c){_LL33D: _LL33E:
 inlinedoc=Cyc_PP_text(({const char*_tmp469="inline ";_tag_dyneither(_tmp469,sizeof(char),8);}));goto _LL33C;}else{_LL33F: _LL340:
 inlinedoc=Cyc_PP_text(({const char*_tmp46A="__inline ";_tag_dyneither(_tmp46A,sizeof(char),10);}));goto _LL33C;}_LL33C:;}else{
# 1822
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp467->sc);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp46B=Cyc_Cyclone_c_compiler;if(_tmp46B == Cyc_Cyclone_Gcc_c){_LL342: _LL343:
 beforenamedoc=attsdoc;goto _LL341;}else{_LL344: _LL345:
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp467->attributes);goto _LL341;}_LL341:;}{
# 1829
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp467->name);
struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,({struct Cyc_Core_Opt*_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471->v=({struct Cyc_PP_Doc*_tmp472[2];_tmp472[1]=namedoc;_tmp472[0]=beforenamedoc;Cyc_PP_cat(_tag_dyneither(_tmp472,sizeof(struct Cyc_PP_Doc*),2));});_tmp471;}));
# 1832
struct Cyc_PP_Doc*bodydoc=({struct Cyc_PP_Doc*_tmp46F[5];_tmp46F[4]=
# 1835
Cyc_Absynpp_rb();_tmp46F[3]=
# 1834
Cyc_PP_line_doc();_tmp46F[2]=
# 1833
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp470[2];_tmp470[1]=Cyc_Absynpp_stmt2doc(_tmp467->body,0);_tmp470[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp470,sizeof(struct Cyc_PP_Doc*),2));}));_tmp46F[1]=
# 1832
Cyc_Absynpp_lb();_tmp46F[0]=Cyc_PP_blank_doc();Cyc_PP_cat(_tag_dyneither(_tmp46F,sizeof(struct Cyc_PP_Doc*),5));});
# 1836
s=({struct Cyc_PP_Doc*_tmp46C[4];_tmp46C[3]=bodydoc;_tmp46C[2]=tqtddoc;_tmp46C[1]=scopedoc;_tmp46C[0]=inlinedoc;Cyc_PP_cat(_tag_dyneither(_tmp46C,sizeof(struct Cyc_PP_Doc*),4));});
# 1838
{enum Cyc_Cyclone_C_Compilers _tmp46D=Cyc_Cyclone_c_compiler;if(_tmp46D == Cyc_Cyclone_Vc_c){_LL347: _LL348:
 s=({struct Cyc_PP_Doc*_tmp46E[2];_tmp46E[1]=s;_tmp46E[0]=attsdoc;Cyc_PP_cat(_tag_dyneither(_tmp46E,sizeof(struct Cyc_PP_Doc*),2));});goto _LL346;}else{_LL349: _LL34A:
 goto _LL346;}_LL346:;}
# 1843
goto _LL31D;};};}case 5: _LL320: _tmp466=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL321:
# 1846
 s=({struct Cyc_PP_Doc*_tmp476[2];_tmp476[1]=Cyc_PP_text(({const char*_tmp477=";";_tag_dyneither(_tmp477,sizeof(char),2);}));_tmp476[0]=Cyc_Absynpp_aggrdecl2doc(_tmp466);Cyc_PP_cat(_tag_dyneither(_tmp476,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL31D;case 0: _LL322: _tmp465=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL323:
# 1849
 s=Cyc_Absynpp_vardecl2doc(_tmp465);
goto _LL31D;case 4: _LL324: _tmp461=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp462=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp453)->f2;_tmp463=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp453)->f3;_tmp464=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp453)->f4;_LL325:
# 1852
 s=({struct Cyc_PP_Doc*_tmp478[8];_tmp478[7]=
# 1860
Cyc_PP_text(({const char*_tmp480=";";_tag_dyneither(_tmp480,sizeof(char),2);}));_tmp478[6]=
# 1859
_tmp463?Cyc_PP_text(({const char*_tmp47F=" @resetable";_tag_dyneither(_tmp47F,sizeof(char),12);})): Cyc_PP_nil_doc();_tmp478[5]=
# 1857
(unsigned int)_tmp464?({struct Cyc_PP_Doc*_tmp47C[3];_tmp47C[2]=
Cyc_PP_text(({const char*_tmp47E=")";_tag_dyneither(_tmp47E,sizeof(char),2);}));_tmp47C[1]=
# 1857
Cyc_Absynpp_exp2doc(_tmp464);_tmp47C[0]=Cyc_PP_text(({const char*_tmp47D=" = open(";_tag_dyneither(_tmp47D,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp47C,sizeof(struct Cyc_PP_Doc*),3));}):
 Cyc_PP_nil_doc();_tmp478[4]=
# 1856
Cyc_Absynpp_qvar2doc(_tmp462->name);_tmp478[3]=
# 1855
Cyc_PP_text(({const char*_tmp47B=">";_tag_dyneither(_tmp47B,sizeof(char),2);}));_tmp478[2]=
# 1854
Cyc_Absynpp_tvar2doc(_tmp461);_tmp478[1]=
# 1853
Cyc_PP_text(({const char*_tmp47A="<";_tag_dyneither(_tmp47A,sizeof(char),2);}));_tmp478[0]=
# 1852
Cyc_PP_text(({const char*_tmp479="region";_tag_dyneither(_tmp479,sizeof(char),7);}));Cyc_PP_cat(_tag_dyneither(_tmp478,sizeof(struct Cyc_PP_Doc*),8));});
# 1861
goto _LL31D;case 6: _LL326: _tmp460=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL327:
# 1863
 s=({struct Cyc_PP_Doc*_tmp481[2];_tmp481[1]=Cyc_PP_text(({const char*_tmp482=";";_tag_dyneither(_tmp482,sizeof(char),2);}));_tmp481[0]=Cyc_Absynpp_datatypedecl2doc(_tmp460);Cyc_PP_cat(_tag_dyneither(_tmp481,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL31D;case 2: _LL328: _tmp45E=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp45F=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp453)->f3;_LL329:
# 1866
 s=({struct Cyc_PP_Doc*_tmp483[5];_tmp483[4]=
# 1870
Cyc_PP_text(({const char*_tmp486=";";_tag_dyneither(_tmp486,sizeof(char),2);}));_tmp483[3]=
# 1869
Cyc_Absynpp_exp2doc(_tmp45F);_tmp483[2]=
# 1868
Cyc_PP_text(({const char*_tmp485=" = ";_tag_dyneither(_tmp485,sizeof(char),4);}));_tmp483[1]=
# 1867
Cyc_Absynpp_pat2doc(_tmp45E);_tmp483[0]=
# 1866
Cyc_PP_text(({const char*_tmp484="let ";_tag_dyneither(_tmp484,sizeof(char),5);}));Cyc_PP_cat(_tag_dyneither(_tmp483,sizeof(struct Cyc_PP_Doc*),5));});
# 1871
goto _LL31D;case 3: _LL32A: _tmp45D=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL32B:
# 1873
 s=({struct Cyc_PP_Doc*_tmp487[3];_tmp487[2]=Cyc_PP_text(({const char*_tmp489=";";_tag_dyneither(_tmp489,sizeof(char),2);}));_tmp487[1]=Cyc_Absynpp_ids2doc(_tmp45D);_tmp487[0]=Cyc_PP_text(({const char*_tmp488="let ";_tag_dyneither(_tmp488,sizeof(char),5);}));Cyc_PP_cat(_tag_dyneither(_tmp487,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL31D;case 7: _LL32C: _tmp45C=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL32D:
# 1876
 s=({struct Cyc_PP_Doc*_tmp48A[2];_tmp48A[1]=Cyc_PP_text(({const char*_tmp48B=";";_tag_dyneither(_tmp48B,sizeof(char),2);}));_tmp48A[0]=Cyc_Absynpp_enumdecl2doc(_tmp45C);Cyc_PP_cat(_tag_dyneither(_tmp48A,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL31D;case 8: _LL32E: _tmp45B=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL32F: {
# 1879
void*t;
if(_tmp45B->defn != 0)
t=(void*)_check_null(_tmp45B->defn);else{
# 1883
t=Cyc_Absyn_new_evar(_tmp45B->kind,0);}
s=({struct Cyc_PP_Doc*_tmp48C[4];_tmp48C[3]=
# 1891
Cyc_PP_text(({const char*_tmp490=";";_tag_dyneither(_tmp490,sizeof(char),2);}));_tmp48C[2]=
# 1890
Cyc_Absynpp_atts2doc(_tmp45B->atts);_tmp48C[1]=
# 1885
Cyc_Absynpp_tqtd2doc(_tmp45B->tq,t,({struct Cyc_Core_Opt*_tmp48E=_cycalloc(sizeof(*_tmp48E));_tmp48E->v=({struct Cyc_PP_Doc*_tmp48F[2];_tmp48F[1]=
# 1888
Cyc_Absynpp_tvars2doc(_tmp45B->tvs);_tmp48F[0]=
# 1887
Cyc_Absynpp_typedef_name2bolddoc(_tmp45B->name);Cyc_PP_cat(_tag_dyneither(_tmp48F,sizeof(struct Cyc_PP_Doc*),2));});_tmp48E;}));_tmp48C[0]=
# 1884
Cyc_PP_text(({const char*_tmp48D="typedef ";_tag_dyneither(_tmp48D,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp48C,sizeof(struct Cyc_PP_Doc*),4));});
# 1893
goto _LL31D;}case 9: _LL330: _tmp459=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp45A=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp453)->f2;_LL331:
# 1895
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp459);
s=({struct Cyc_PP_Doc*_tmp491[8];_tmp491[7]=
# 1902
Cyc_Absynpp_rb();_tmp491[6]=
# 1901
Cyc_PP_line_doc();_tmp491[5]=
# 1900
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({const char*_tmp493="";_tag_dyneither(_tmp493,sizeof(char),1);}),_tmp45A);_tmp491[4]=
# 1899
Cyc_PP_line_doc();_tmp491[3]=
# 1898
Cyc_Absynpp_lb();_tmp491[2]=Cyc_PP_blank_doc();_tmp491[1]=
# 1897
Cyc_PP_textptr(_tmp459);_tmp491[0]=
# 1896
Cyc_PP_text(({const char*_tmp492="namespace ";_tag_dyneither(_tmp492,sizeof(char),11);}));Cyc_PP_cat(_tag_dyneither(_tmp491,sizeof(struct Cyc_PP_Doc*),8));});
# 1903
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL31D;case 10: _LL332: _tmp457=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp458=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp453)->f2;_LL333:
# 1906
 if(Cyc_Absynpp_print_using_stmts)
s=({struct Cyc_PP_Doc*_tmp494[8];_tmp494[7]=
# 1913
Cyc_Absynpp_rb();_tmp494[6]=
# 1912
Cyc_PP_line_doc();_tmp494[5]=
# 1911
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({const char*_tmp496="";_tag_dyneither(_tmp496,sizeof(char),1);}),_tmp458);_tmp494[4]=
# 1910
Cyc_PP_line_doc();_tmp494[3]=
# 1909
Cyc_Absynpp_lb();_tmp494[2]=Cyc_PP_blank_doc();_tmp494[1]=
# 1908
Cyc_Absynpp_qvar2doc(_tmp457);_tmp494[0]=
# 1907
Cyc_PP_text(({const char*_tmp495="using ";_tag_dyneither(_tmp495,sizeof(char),7);}));Cyc_PP_cat(_tag_dyneither(_tmp494,sizeof(struct Cyc_PP_Doc*),8));});else{
# 1915
s=({struct Cyc_PP_Doc*_tmp497[11];_tmp497[10]=
# 1925
Cyc_PP_text(({const char*_tmp49C=" */";_tag_dyneither(_tmp49C,sizeof(char),4);}));_tmp497[9]=
# 1924
Cyc_Absynpp_rb();_tmp497[8]=
# 1923
Cyc_PP_text(({const char*_tmp49B="/* ";_tag_dyneither(_tmp49B,sizeof(char),4);}));_tmp497[7]=
# 1922
Cyc_PP_line_doc();_tmp497[6]=
# 1921
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({const char*_tmp49A="";_tag_dyneither(_tmp49A,sizeof(char),1);}),_tmp458);_tmp497[5]=
# 1920
Cyc_PP_line_doc();_tmp497[4]=
# 1919
Cyc_PP_text(({const char*_tmp499=" */";_tag_dyneither(_tmp499,sizeof(char),4);}));_tmp497[3]=
# 1918
Cyc_Absynpp_lb();_tmp497[2]=
# 1917
Cyc_PP_blank_doc();_tmp497[1]=
# 1916
Cyc_Absynpp_qvar2doc(_tmp457);_tmp497[0]=
# 1915
Cyc_PP_text(({const char*_tmp498="/* using ";_tag_dyneither(_tmp498,sizeof(char),10);}));Cyc_PP_cat(_tag_dyneither(_tmp497,sizeof(struct Cyc_PP_Doc*),11));});}
# 1926
goto _LL31D;case 11: _LL334: _tmp456=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL335:
# 1928
 if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_tmp49D[6];_tmp49D[5]=
# 1934
Cyc_Absynpp_rb();_tmp49D[4]=
# 1933
Cyc_PP_line_doc();_tmp49D[3]=
# 1932
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({const char*_tmp49F="";_tag_dyneither(_tmp49F,sizeof(char),1);}),_tmp456);_tmp49D[2]=
# 1931
Cyc_PP_line_doc();_tmp49D[1]=
# 1930
Cyc_Absynpp_lb();_tmp49D[0]=
# 1929
Cyc_PP_text(({const char*_tmp49E="extern \"C\" ";_tag_dyneither(_tmp49E,sizeof(char),12);}));Cyc_PP_cat(_tag_dyneither(_tmp49D,sizeof(struct Cyc_PP_Doc*),6));});else{
# 1936
s=({struct Cyc_PP_Doc*_tmp4A0[9];_tmp4A0[8]=
# 1944
Cyc_PP_text(({const char*_tmp4A5=" */";_tag_dyneither(_tmp4A5,sizeof(char),4);}));_tmp4A0[7]=
# 1943
Cyc_Absynpp_rb();_tmp4A0[6]=
# 1942
Cyc_PP_text(({const char*_tmp4A4="/* ";_tag_dyneither(_tmp4A4,sizeof(char),4);}));_tmp4A0[5]=
# 1941
Cyc_PP_line_doc();_tmp4A0[4]=
# 1940
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({const char*_tmp4A3="";_tag_dyneither(_tmp4A3,sizeof(char),1);}),_tmp456);_tmp4A0[3]=
# 1939
Cyc_PP_line_doc();_tmp4A0[2]=
# 1938
Cyc_PP_text(({const char*_tmp4A2=" */";_tag_dyneither(_tmp4A2,sizeof(char),4);}));_tmp4A0[1]=
# 1937
Cyc_Absynpp_lb();_tmp4A0[0]=
# 1936
Cyc_PP_text(({const char*_tmp4A1="/* extern \"C\" ";_tag_dyneither(_tmp4A1,sizeof(char),15);}));Cyc_PP_cat(_tag_dyneither(_tmp4A0,sizeof(struct Cyc_PP_Doc*),9));});}
# 1945
goto _LL31D;case 12: _LL336: _tmp454=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp455=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp453)->f2;_LL337:
# 1947
 if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
if(_tmp455 != 0)
exs_doc=({struct Cyc_PP_Doc*_tmp4A6[7];_tmp4A6[6]=
# 1952
Cyc_Absynpp_rb();_tmp4A6[5]=Cyc_PP_line_doc();_tmp4A6[4]=
# 1951
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,({const char*_tmp4A8=",";_tag_dyneither(_tmp4A8,sizeof(char),2);}),_tmp455);_tmp4A6[3]=Cyc_PP_line_doc();_tmp4A6[2]=
# 1950
Cyc_Absynpp_lb();_tmp4A6[1]=Cyc_PP_text(({const char*_tmp4A7=" export ";_tag_dyneither(_tmp4A7,sizeof(char),9);}));_tmp4A6[0]=Cyc_Absynpp_rb();Cyc_PP_cat(_tag_dyneither(_tmp4A6,sizeof(struct Cyc_PP_Doc*),7));});else{
# 1954
exs_doc=Cyc_Absynpp_rb();}
s=({struct Cyc_PP_Doc*_tmp4A9[6];_tmp4A9[5]=exs_doc;_tmp4A9[4]=
# 1959
Cyc_PP_line_doc();_tmp4A9[3]=
# 1958
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({const char*_tmp4AB="";_tag_dyneither(_tmp4AB,sizeof(char),1);}),_tmp454);_tmp4A9[2]=
# 1957
Cyc_PP_line_doc();_tmp4A9[1]=
# 1956
Cyc_Absynpp_lb();_tmp4A9[0]=
# 1955
Cyc_PP_text(({const char*_tmp4AA="extern \"C include\" ";_tag_dyneither(_tmp4AA,sizeof(char),20);}));Cyc_PP_cat(_tag_dyneither(_tmp4A9,sizeof(struct Cyc_PP_Doc*),6));});}else{
# 1962
s=({struct Cyc_PP_Doc*_tmp4AC[9];_tmp4AC[8]=
# 1970
Cyc_PP_text(({const char*_tmp4B1=" */";_tag_dyneither(_tmp4B1,sizeof(char),4);}));_tmp4AC[7]=
# 1969
Cyc_Absynpp_rb();_tmp4AC[6]=
# 1968
Cyc_PP_text(({const char*_tmp4B0="/* ";_tag_dyneither(_tmp4B0,sizeof(char),4);}));_tmp4AC[5]=
# 1967
Cyc_PP_line_doc();_tmp4AC[4]=
# 1966
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({const char*_tmp4AF="";_tag_dyneither(_tmp4AF,sizeof(char),1);}),_tmp454);_tmp4AC[3]=
# 1965
Cyc_PP_line_doc();_tmp4AC[2]=
# 1964
Cyc_PP_text(({const char*_tmp4AE=" */";_tag_dyneither(_tmp4AE,sizeof(char),4);}));_tmp4AC[1]=
# 1963
Cyc_Absynpp_lb();_tmp4AC[0]=
# 1962
Cyc_PP_text(({const char*_tmp4AD="/* extern \"C include\" ";_tag_dyneither(_tmp4AD,sizeof(char),23);}));Cyc_PP_cat(_tag_dyneither(_tmp4AC,sizeof(struct Cyc_PP_Doc*),9));});}
# 1971
goto _LL31D;case 13: _LL338: _LL339:
# 1973
 s=({struct Cyc_PP_Doc*_tmp4B2[2];_tmp4B2[1]=Cyc_Absynpp_lb();_tmp4B2[0]=Cyc_PP_text(({const char*_tmp4B3="__cyclone_port_on__;";_tag_dyneither(_tmp4B3,sizeof(char),21);}));Cyc_PP_cat(_tag_dyneither(_tmp4B2,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL31D;default: _LL33A: _LL33B:
# 1976
 s=({struct Cyc_PP_Doc*_tmp4B4[2];_tmp4B4[1]=Cyc_Absynpp_lb();_tmp4B4[0]=Cyc_PP_text(({const char*_tmp4B5="__cyclone_port_off__;";_tag_dyneither(_tmp4B5,sizeof(char),22);}));Cyc_PP_cat(_tag_dyneither(_tmp4B4,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL31D;}_LL31D:;}
# 1980
return s;}
# 1983
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();{
enum Cyc_Absyn_Scope _tmp4B6=sc;switch(_tmp4B6){case Cyc_Absyn_Static: _LL34C: _LL34D:
 return Cyc_PP_text(({const char*_tmp4B7="static ";_tag_dyneither(_tmp4B7,sizeof(char),8);}));case Cyc_Absyn_Public: _LL34E: _LL34F:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL350: _LL351:
 return Cyc_PP_text(({const char*_tmp4B8="extern ";_tag_dyneither(_tmp4B8,sizeof(char),8);}));case Cyc_Absyn_ExternC: _LL352: _LL353:
 return Cyc_PP_text(({const char*_tmp4B9="extern \"C\" ";_tag_dyneither(_tmp4B9,sizeof(char),12);}));case Cyc_Absyn_Abstract: _LL354: _LL355:
 return Cyc_PP_text(({const char*_tmp4BA="abstract ";_tag_dyneither(_tmp4BA,sizeof(char),10);}));default: _LL356: _LL357:
 return Cyc_PP_text(({const char*_tmp4BB="register ";_tag_dyneither(_tmp4BB,sizeof(char),10);}));}_LL34B:;};}
# 1996
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp4BC=t;struct Cyc_List_List*_tmp4BD;struct Cyc_Absyn_Tvar*_tmp4BE;switch(*((int*)_tmp4BC)){case 2: _LL359: _tmp4BE=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4BC)->f1;_LL35A:
 return Cyc_Tcutil_is_temp_tvar(_tmp4BE);case 24: _LL35B: _tmp4BD=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4BC)->f1;_LL35C:
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp4BD);default: _LL35D: _LL35E:
 return 0;}_LL358:;}
# 2008
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp4BF=t;struct Cyc_Absyn_Typedefdecl*_tmp4C0;void*_tmp4C1;switch(*((int*)_tmp4BF)){case 12: _LL360: _LL361:
 return 1;case 14: _LL362: _LL363:
 return 1;case 17: _LL364: _tmp4C0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4BF)->f3;_tmp4C1=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4BF)->f4;if(_tmp4C1 != 0){_LL365:
# 2015
 return Cyc_Absynpp_is_anon_aggrtype(_tmp4C1);}else{goto _LL366;}default: _LL366: _LL367:
 return 0;}_LL35F:;}
# 2022
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 2025
if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmp4C2=({struct _tuple16 _tmp4C7;_tmp4C7.f1=(void*)tms->hd;_tmp4C7.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp4C7;});struct _tuple16 _tmp4C3=_tmp4C2;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp4C3.f1)->tag == 2){if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp4C3.f2)->tag == 3){_LL369: _LL36A:
# 2028
 return({struct Cyc_List_List*_tmp4C4=_region_malloc(r,sizeof(*_tmp4C4));_tmp4C4->hd=(void*)tms->hd;_tmp4C4->tl=({struct Cyc_List_List*_tmp4C5=_region_malloc(r,sizeof(*_tmp4C5));_tmp4C5->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp4C5->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl);_tmp4C5;});_tmp4C4;});}else{goto _LL36B;}}else{_LL36B: _LL36C:
 return({struct Cyc_List_List*_tmp4C6=_region_malloc(r,sizeof(*_tmp4C6));_tmp4C6->hd=atts;_tmp4C6->tl=tms;_tmp4C6;});}_LL368:;}else{
# 2031
return({struct Cyc_List_List*_tmp4C8=_region_malloc(r,sizeof(*_tmp4C8));_tmp4C8->hd=atts;_tmp4C8->tl=tms;_tmp4C8;});}}
# 2036
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2038
void*_tmp4C9=t;struct _tuple0*_tmp4CA;struct Cyc_List_List*_tmp4CB;struct Cyc_Absyn_Typedefdecl*_tmp4CC;void*_tmp4CD;struct Cyc_Core_Opt*_tmp4CE;void*_tmp4CF;int _tmp4D0;struct Cyc_List_List*_tmp4D1;void*_tmp4D2;struct Cyc_Absyn_Tqual _tmp4D3;void*_tmp4D4;struct Cyc_List_List*_tmp4D5;int _tmp4D6;struct Cyc_Absyn_VarargInfo*_tmp4D7;struct Cyc_List_List*_tmp4D8;struct Cyc_List_List*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4DB;void*_tmp4DC;struct Cyc_Absyn_Tqual _tmp4DD;struct Cyc_Absyn_PtrAtts _tmp4DE;void*_tmp4DF;struct Cyc_Absyn_Tqual _tmp4E0;struct Cyc_Absyn_Exp*_tmp4E1;union Cyc_Absyn_Constraint*_tmp4E2;unsigned int _tmp4E3;switch(*((int*)_tmp4C9)){case 8: _LL36E: _tmp4DF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C9)->f1).elt_type;_tmp4E0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C9)->f1).tq;_tmp4E1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C9)->f1).num_elts;_tmp4E2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C9)->f1).zero_term;_tmp4E3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C9)->f1).zt_loc;_LL36F: {
# 2041
struct _tuple12 _tmp4E4=Cyc_Absynpp_to_tms(r,_tmp4E0,_tmp4DF);struct _tuple12 _tmp4E5=_tmp4E4;struct Cyc_Absyn_Tqual _tmp4E6;void*_tmp4E7;struct Cyc_List_List*_tmp4E8;_LL37B: _tmp4E6=_tmp4E5.f1;_tmp4E7=_tmp4E5.f2;_tmp4E8=_tmp4E5.f3;_LL37C:;{
void*tm;
if(_tmp4E1 == 0)
tm=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4E9=_region_malloc(r,sizeof(*_tmp4E9));_tmp4E9[0]=({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp4EA;_tmp4EA.tag=0;_tmp4EA.f1=_tmp4E2;_tmp4EA.f2=_tmp4E3;_tmp4EA;});_tmp4E9;});else{
# 2046
tm=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4EB=_region_malloc(r,sizeof(*_tmp4EB));_tmp4EB[0]=({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp4EC;_tmp4EC.tag=1;_tmp4EC.f1=_tmp4E1;_tmp4EC.f2=_tmp4E2;_tmp4EC.f3=_tmp4E3;_tmp4EC;});_tmp4EB;});}
return({struct _tuple12 _tmp4ED;_tmp4ED.f1=_tmp4E6;_tmp4ED.f2=_tmp4E7;_tmp4ED.f3=({struct Cyc_List_List*_tmp4EE=_region_malloc(r,sizeof(*_tmp4EE));_tmp4EE->hd=tm;_tmp4EE->tl=_tmp4E8;_tmp4EE;});_tmp4ED;});};}case 5: _LL370: _tmp4DC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C9)->f1).elt_typ;_tmp4DD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C9)->f1).elt_tq;_tmp4DE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C9)->f1).ptr_atts;_LL371: {
# 2050
struct _tuple12 _tmp4EF=Cyc_Absynpp_to_tms(r,_tmp4DD,_tmp4DC);struct _tuple12 _tmp4F0=_tmp4EF;struct Cyc_Absyn_Tqual _tmp4F1;void*_tmp4F2;struct Cyc_List_List*_tmp4F3;_LL37E: _tmp4F1=_tmp4F0.f1;_tmp4F2=_tmp4F0.f2;_tmp4F3=_tmp4F0.f3;_LL37F:;
_tmp4F3=({struct Cyc_List_List*_tmp4F4=_region_malloc(r,sizeof(*_tmp4F4));_tmp4F4->hd=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp4F5=_region_malloc(r,sizeof(*_tmp4F5));_tmp4F5[0]=({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmp4F6;_tmp4F6.tag=2;_tmp4F6.f1=_tmp4DE;_tmp4F6.f2=tq;_tmp4F6;});_tmp4F5;});_tmp4F4->tl=_tmp4F3;_tmp4F4;});
return({struct _tuple12 _tmp4F7;_tmp4F7.f1=_tmp4F1;_tmp4F7.f2=_tmp4F2;_tmp4F7.f3=_tmp4F3;_tmp4F7;});}case 9: _LL372: _tmp4D1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).tvars;_tmp4D2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).effect;_tmp4D3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).ret_tqual;_tmp4D4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).ret_typ;_tmp4D5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).args;_tmp4D6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).c_varargs;_tmp4D7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).cyc_varargs;_tmp4D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).rgn_po;_tmp4D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).attributes;_tmp4DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).requires_clause;_tmp4DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).ensures_clause;_LL373:
# 2056
 if(!Cyc_Absynpp_print_all_tvars){
# 2060
if(_tmp4D2 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect(_tmp4D2)){
_tmp4D2=0;
_tmp4D1=0;}}else{
# 2065
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2068
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp4D1);}{
# 2073
struct _tuple12 _tmp4F8=Cyc_Absynpp_to_tms(r,_tmp4D3,_tmp4D4);struct _tuple12 _tmp4F9=_tmp4F8;struct Cyc_Absyn_Tqual _tmp4FA;void*_tmp4FB;struct Cyc_List_List*_tmp4FC;_LL381: _tmp4FA=_tmp4F9.f1;_tmp4FB=_tmp4F9.f2;_tmp4FC=_tmp4F9.f3;_LL382:;{
struct Cyc_List_List*tms=_tmp4FC;
# 2084 "absynpp.cyc"
{enum Cyc_Cyclone_C_Compilers _tmp4FD=Cyc_Cyclone_c_compiler;if(_tmp4FD == Cyc_Cyclone_Gcc_c){_LL384: _LL385:
# 2086
 if(_tmp4D9 != 0)
# 2089
tms=Cyc_Absynpp_bubble_attributes(r,(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4FE=_region_malloc(r,sizeof(*_tmp4FE));_tmp4FE[0]=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp4FF;_tmp4FF.tag=5;_tmp4FF.f1=0;_tmp4FF.f2=_tmp4D9;_tmp4FF;});_tmp4FE;}),tms);
# 2091
tms=({struct Cyc_List_List*_tmp500=_region_malloc(r,sizeof(*_tmp500));_tmp500->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp501=_region_malloc(r,sizeof(*_tmp501));_tmp501[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp502;_tmp502.tag=3;_tmp502.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp503=_region_malloc(r,sizeof(*_tmp503));_tmp503[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp504;_tmp504.tag=1;_tmp504.f1=_tmp4D5;_tmp504.f2=_tmp4D6;_tmp504.f3=_tmp4D7;_tmp504.f4=_tmp4D2;_tmp504.f5=_tmp4D8;_tmp504.f6=_tmp4DA;_tmp504.f7=_tmp4DB;_tmp504;});_tmp503;});_tmp502;});_tmp501;});_tmp500->tl=tms;_tmp500;});
# 2095
goto _LL383;}else{_LL386: _LL387:
# 2097
 tms=({struct Cyc_List_List*_tmp505=_region_malloc(r,sizeof(*_tmp505));_tmp505->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp506=_region_malloc(r,sizeof(*_tmp506));_tmp506[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp507;_tmp507.tag=3;_tmp507.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp508=_region_malloc(r,sizeof(*_tmp508));_tmp508[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp509;_tmp509.tag=1;_tmp509.f1=_tmp4D5;_tmp509.f2=_tmp4D6;_tmp509.f3=_tmp4D7;_tmp509.f4=_tmp4D2;_tmp509.f5=_tmp4D8;_tmp509.f6=_tmp4DA;_tmp509.f7=_tmp4DB;_tmp509;});_tmp508;});_tmp507;});_tmp506;});_tmp505->tl=tms;_tmp505;});
# 2101
for(0;_tmp4D9 != 0;_tmp4D9=_tmp4D9->tl){
void*_tmp50A=(void*)_tmp4D9->hd;void*_tmp50B=_tmp50A;switch(*((int*)_tmp50B)){case 1: _LL389: _LL38A:
 goto _LL38C;case 2: _LL38B: _LL38C:
 goto _LL38E;case 3: _LL38D: _LL38E:
# 2106
 tms=({struct Cyc_List_List*_tmp50C=_region_malloc(r,sizeof(*_tmp50C));_tmp50C->hd=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp50D=_region_malloc(r,sizeof(*_tmp50D));_tmp50D[0]=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp50E;_tmp50E.tag=5;_tmp50E.f1=0;_tmp50E.f2=({struct Cyc_List_List*_tmp50F=_cycalloc(sizeof(*_tmp50F));_tmp50F->hd=(void*)_tmp4D9->hd;_tmp50F->tl=0;_tmp50F;});_tmp50E;});_tmp50D;});_tmp50C->tl=tms;_tmp50C;});
goto AfterAtts;default: _LL38F: _LL390:
 goto _LL388;}_LL388:;}
# 2110
goto _LL383;}_LL383:;}
# 2113
AfterAtts:
 if(_tmp4D1 != 0)
tms=({struct Cyc_List_List*_tmp510=_region_malloc(r,sizeof(*_tmp510));_tmp510->hd=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp511=_region_malloc(r,sizeof(*_tmp511));_tmp511[0]=({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmp512;_tmp512.tag=4;_tmp512.f1=_tmp4D1;_tmp512.f2=0;_tmp512.f3=1;_tmp512;});_tmp511;});_tmp510->tl=tms;_tmp510;});
return({struct _tuple12 _tmp513;_tmp513.f1=_tmp4FA;_tmp513.f2=_tmp4FB;_tmp513.f3=tms;_tmp513;});};};case 1: _LL374: _tmp4CE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4C9)->f1;_tmp4CF=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4C9)->f2;_tmp4D0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4C9)->f3;_LL375:
# 2119
 if(_tmp4CF == 0)
return({struct _tuple12 _tmp514;_tmp514.f1=tq;_tmp514.f2=t;_tmp514.f3=0;_tmp514;});else{
# 2122
return Cyc_Absynpp_to_tms(r,tq,_tmp4CF);}case 17: _LL376: _tmp4CA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f1;_tmp4CB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f2;_tmp4CC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f3;_tmp4CD=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f4;_LL377:
# 2127
 if((_tmp4CD == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(_tmp4CD))
return({struct _tuple12 _tmp515;_tmp515.f1=tq;_tmp515.f2=t;_tmp515.f3=0;_tmp515;});else{
# 2130
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,_tmp4CD);}default: _LL378: _LL379:
# 2135
 return({struct _tuple12 _tmp516;_tmp516.f1=tq;_tmp516.f2=t;_tmp516.f3=0;_tmp516;});}_LL36D:;}
# 2139
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2141
void*_tmp517=t;void*_tmp518;void*_tmp519;switch(*((int*)_tmp517)){case 1: _LL392: _tmp519=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp517)->f2;if(_tmp519 != 0){_LL393:
 return Cyc_Absynpp_is_char_ptr(_tmp519);}else{goto _LL396;}case 5: _LL394: _tmp518=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp517)->f1).elt_typ;_LL395:
# 2144
 L: {
void*_tmp51A=_tmp518;void*_tmp51B;void*_tmp51C;switch(*((int*)_tmp51A)){case 1: _LL399: _tmp51C=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp51A)->f2;if(_tmp51C != 0){_LL39A:
 _tmp518=_tmp51C;goto L;}else{goto _LL39F;}case 17: _LL39B: _tmp51B=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp51A)->f4;if(_tmp51B != 0){_LL39C:
 _tmp518=_tmp51B;goto L;}else{goto _LL39F;}case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp51A)->f2 == Cyc_Absyn_Char_sz){_LL39D: _LL39E:
 return 1;}else{goto _LL39F;}default: _LL39F: _LL3A0:
 return 0;}_LL398:;}default: _LL396: _LL397:
# 2151
 return 0;}_LL391:;}
# 2155
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp51D=_new_region("temp");struct _RegionHandle*temp=& _tmp51D;_push_region(temp);
{struct _tuple12 _tmp51E=Cyc_Absynpp_to_tms(temp,tq,typ);struct _tuple12 _tmp51F=_tmp51E;struct Cyc_Absyn_Tqual _tmp520;void*_tmp521;struct Cyc_List_List*_tmp522;_LL3A2: _tmp520=_tmp51F.f1;_tmp521=_tmp51F.f2;_tmp522=_tmp51F.f3;_LL3A3:;
_tmp522=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp522);
if(_tmp522 == 0  && dopt == 0){
struct Cyc_PP_Doc*_tmp524=({struct Cyc_PP_Doc*_tmp523[2];_tmp523[1]=Cyc_Absynpp_ntyp2doc(_tmp521);_tmp523[0]=Cyc_Absynpp_tqual2doc(_tmp520);Cyc_PP_cat(_tag_dyneither(_tmp523,sizeof(struct Cyc_PP_Doc*),2));});_npop_handler(0);return _tmp524;}else{
# 2162
struct Cyc_PP_Doc*_tmp527=({struct Cyc_PP_Doc*_tmp525[4];_tmp525[3]=
# 2166
Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp522);_tmp525[2]=
# 2165
Cyc_PP_text(({const char*_tmp526=" ";_tag_dyneither(_tmp526,sizeof(char),2);}));_tmp525[1]=
# 2164
Cyc_Absynpp_ntyp2doc(_tmp521);_tmp525[0]=
# 2163
Cyc_Absynpp_tqual2doc(_tmp520);Cyc_PP_cat(_tag_dyneither(_tmp525,sizeof(struct Cyc_PP_Doc*),4));});_npop_handler(0);return _tmp527;}}
# 2157
;_pop_region(temp);}
# 2170
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp528=f->requires_clause;
if((unsigned int)_tmp528)
requires_doc=({struct Cyc_PP_Doc*_tmp529[2];_tmp529[1]=Cyc_Absynpp_exp2doc(_tmp528);_tmp529[0]=Cyc_PP_text(({const char*_tmp52A="@requires ";_tag_dyneither(_tmp52A,sizeof(char),11);}));Cyc_PP_cat(_tag_dyneither(_tmp529,sizeof(struct Cyc_PP_Doc*),2));});else{
# 2176
requires_doc=Cyc_PP_nil_doc();}{
# 2178
enum Cyc_Cyclone_C_Compilers _tmp52B=Cyc_Cyclone_c_compiler;if(_tmp52B == Cyc_Cyclone_Gcc_c){_LL3A5: _LL3A6:
# 2181
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp52C[5];_tmp52C[4]=
# 2184
Cyc_PP_text(({const char*_tmp52F=";";_tag_dyneither(_tmp52F,sizeof(char),2);}));_tmp52C[3]=Cyc_Absynpp_atts2doc(f->attributes);_tmp52C[2]=
# 2183
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp52C[1]=Cyc_PP_text(({const char*_tmp52E=":";_tag_dyneither(_tmp52E,sizeof(char),2);}));_tmp52C[0]=
# 2182
Cyc_Absynpp_tqtd2doc(f->tq,f->type,({struct Cyc_Core_Opt*_tmp52D=_cycalloc(sizeof(*_tmp52D));_tmp52D->v=Cyc_PP_textptr(f->name);_tmp52D;}));Cyc_PP_cat(_tag_dyneither(_tmp52C,sizeof(struct Cyc_PP_Doc*),5));});else{
# 2186
return({struct Cyc_PP_Doc*_tmp530[4];_tmp530[3]=
Cyc_PP_text(({const char*_tmp532=";";_tag_dyneither(_tmp532,sizeof(char),2);}));_tmp530[2]=requires_doc;_tmp530[1]=Cyc_Absynpp_atts2doc(f->attributes);_tmp530[0]=
# 2186
Cyc_Absynpp_tqtd2doc(f->tq,f->type,({struct Cyc_Core_Opt*_tmp531=_cycalloc(sizeof(*_tmp531));_tmp531->v=Cyc_PP_textptr(f->name);_tmp531;}));Cyc_PP_cat(_tag_dyneither(_tmp530,sizeof(struct Cyc_PP_Doc*),4));});}}else{_LL3A7: _LL3A8:
# 2189
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp533[5];_tmp533[4]=
# 2192
Cyc_PP_text(({const char*_tmp536=";";_tag_dyneither(_tmp536,sizeof(char),2);}));_tmp533[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp533[2]=Cyc_PP_text(({const char*_tmp535=":";_tag_dyneither(_tmp535,sizeof(char),2);}));_tmp533[1]=
# 2191
Cyc_Absynpp_tqtd2doc(f->tq,f->type,({struct Cyc_Core_Opt*_tmp534=_cycalloc(sizeof(*_tmp534));_tmp534->v=Cyc_PP_textptr(f->name);_tmp534;}));_tmp533[0]=
# 2190
Cyc_Absynpp_atts2doc(f->attributes);Cyc_PP_cat(_tag_dyneither(_tmp533,sizeof(struct Cyc_PP_Doc*),5));});else{
# 2194
return({struct Cyc_PP_Doc*_tmp537[4];_tmp537[3]=
# 2196
Cyc_PP_text(({const char*_tmp539=";";_tag_dyneither(_tmp539,sizeof(char),2);}));_tmp537[2]=requires_doc;_tmp537[1]=
# 2195
Cyc_Absynpp_tqtd2doc(f->tq,f->type,({struct Cyc_Core_Opt*_tmp538=_cycalloc(sizeof(*_tmp538));_tmp538->v=Cyc_PP_textptr(f->name);_tmp538;}));_tmp537[0]=
# 2194
Cyc_Absynpp_atts2doc(f->attributes);Cyc_PP_cat(_tag_dyneither(_tmp537,sizeof(struct Cyc_PP_Doc*),4));});}}_LL3A4:;};}
# 2201
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,({const char*_tmp53A="";_tag_dyneither(_tmp53A,sizeof(char),1);}),fields);}
# 2205
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
return({struct Cyc_PP_Doc*_tmp53B[3];_tmp53B[2]=
f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs);_tmp53B[1]=
# 2206
Cyc_Absynpp_typedef_name2doc(f->name);_tmp53B[0]=Cyc_Absynpp_scope2doc(f->sc);Cyc_PP_cat(_tag_dyneither(_tmp53B,sizeof(struct Cyc_PP_Doc*),3));});}
# 2210
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,({const char*_tmp53C=",";_tag_dyneither(_tmp53C,sizeof(char),2);}),fields);}
# 2219
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);
({void*_tmp53D=0;Cyc_fprintf(f,({const char*_tmp53E="\n";_tag_dyneither(_tmp53E,sizeof(char),2);}),_tag_dyneither(_tmp53D,sizeof(void*),0));});}}
# 2226
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
return Cyc_PP_string_of_doc(Cyc_PP_seql(({const char*_tmp53F="";_tag_dyneither(_tmp53F,sizeof(char),1);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),72);}
# 2229
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
# 2233
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
# 2244
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _dyneither_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}
