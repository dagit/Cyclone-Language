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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
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
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 101
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 139 "tcenv.h"
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
struct Cyc_List_List*_tmp61D;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((_tmp61D=_cycalloc(sizeof(*_tmp61D)),((_tmp61D->hd=v,((_tmp61D->tl=0,_tmp61D)))))));}
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
const char*_tmp61E;return(_tmp61E="\\a",_tag_dyneither(_tmp61E,sizeof(char),3));}case '\b': _LL1: {
const char*_tmp61F;return(_tmp61F="\\b",_tag_dyneither(_tmp61F,sizeof(char),3));}case '\f': _LL2: {
const char*_tmp620;return(_tmp620="\\f",_tag_dyneither(_tmp620,sizeof(char),3));}case '\n': _LL3: {
const char*_tmp621;return(_tmp621="\\n",_tag_dyneither(_tmp621,sizeof(char),3));}case '\r': _LL4: {
const char*_tmp622;return(_tmp622="\\r",_tag_dyneither(_tmp622,sizeof(char),3));}case '\t': _LL5: {
const char*_tmp623;return(_tmp623="\\t",_tag_dyneither(_tmp623,sizeof(char),3));}case '\v': _LL6: {
const char*_tmp624;return(_tmp624="\\v",_tag_dyneither(_tmp624,sizeof(char),3));}case '\\': _LL7: {
const char*_tmp625;return(_tmp625="\\\\",_tag_dyneither(_tmp625,sizeof(char),3));}case '"': _LL8: {
const char*_tmp626;return(_tmp626="\"",_tag_dyneither(_tmp626,sizeof(char),2));}case '\'': _LL9: {
const char*_tmp627;return(_tmp627="\\'",_tag_dyneither(_tmp627,sizeof(char),3));}default: _LLA:
# 277
 if(c >= ' '  && c <= '~'){
struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);
{char _tmp62A;char _tmp629;struct _dyneither_ptr _tmp628;(_tmp628=_dyneither_ptr_plus(_tmpC,sizeof(char),0),((_tmp629=*((char*)_check_dyneither_subscript(_tmp628,sizeof(char),0)),((_tmp62A=c,((_get_dyneither_size(_tmp628,sizeof(char))== 1  && (_tmp629 == '\000'  && _tmp62A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp628.curr)=_tmp62A)))))));}
return(struct _dyneither_ptr)_tmpC;}else{
# 282
struct _dyneither_ptr _tmp10=Cyc_Core_new_string(5);
int j=0;
{char _tmp62D;char _tmp62C;struct _dyneither_ptr _tmp62B;(_tmp62B=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp62C=*((char*)_check_dyneither_subscript(_tmp62B,sizeof(char),0)),((_tmp62D='\\',((_get_dyneither_size(_tmp62B,sizeof(char))== 1  && (_tmp62C == '\000'  && _tmp62D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62B.curr)=_tmp62D)))))));}
{char _tmp630;char _tmp62F;struct _dyneither_ptr _tmp62E;(_tmp62E=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp62F=*((char*)_check_dyneither_subscript(_tmp62E,sizeof(char),0)),((_tmp630=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp62E,sizeof(char))== 1  && (_tmp62F == '\000'  && _tmp630 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62E.curr)=_tmp630)))))));}
{char _tmp633;char _tmp632;struct _dyneither_ptr _tmp631;(_tmp631=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp632=*((char*)_check_dyneither_subscript(_tmp631,sizeof(char),0)),((_tmp633=(char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp631,sizeof(char))== 1  && (_tmp632 == '\000'  && _tmp633 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp631.curr)=_tmp633)))))));}
{char _tmp636;char _tmp635;struct _dyneither_ptr _tmp634;(_tmp634=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp635=*((char*)_check_dyneither_subscript(_tmp634,sizeof(char),0)),((_tmp636=(char)('0' + (c & 7)),((_get_dyneither_size(_tmp634,sizeof(char))== 1  && (_tmp635 == '\000'  && _tmp636 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp634.curr)=_tmp636)))))));}
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
{char _tmp639;char _tmp638;struct _dyneither_ptr _tmp637;(_tmp637=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp638=*((char*)_check_dyneither_subscript(_tmp637,sizeof(char),0)),((_tmp639='\\',((_get_dyneither_size(_tmp637,sizeof(char))== 1  && (_tmp638 == '\000'  && _tmp639 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp637.curr)=_tmp639)))))));}{char _tmp63C;char _tmp63B;struct _dyneither_ptr _tmp63A;(_tmp63A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp63B=*((char*)_check_dyneither_subscript(_tmp63A,sizeof(char),0)),((_tmp63C='a',((_get_dyneither_size(_tmp63A,sizeof(char))== 1  && (_tmp63B == '\000'  && _tmp63C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp63A.curr)=_tmp63C)))))));}goto _LL21;_LL24: if(_tmp21 != '\b')goto _LL26;_LL25:
{char _tmp63F;char _tmp63E;struct _dyneither_ptr _tmp63D;(_tmp63D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp63E=*((char*)_check_dyneither_subscript(_tmp63D,sizeof(char),0)),((_tmp63F='\\',((_get_dyneither_size(_tmp63D,sizeof(char))== 1  && (_tmp63E == '\000'  && _tmp63F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp63D.curr)=_tmp63F)))))));}{char _tmp642;char _tmp641;struct _dyneither_ptr _tmp640;(_tmp640=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp641=*((char*)_check_dyneither_subscript(_tmp640,sizeof(char),0)),((_tmp642='b',((_get_dyneither_size(_tmp640,sizeof(char))== 1  && (_tmp641 == '\000'  && _tmp642 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp640.curr)=_tmp642)))))));}goto _LL21;_LL26: if(_tmp21 != '\f')goto _LL28;_LL27:
{char _tmp645;char _tmp644;struct _dyneither_ptr _tmp643;(_tmp643=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp644=*((char*)_check_dyneither_subscript(_tmp643,sizeof(char),0)),((_tmp645='\\',((_get_dyneither_size(_tmp643,sizeof(char))== 1  && (_tmp644 == '\000'  && _tmp645 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp643.curr)=_tmp645)))))));}{char _tmp648;char _tmp647;struct _dyneither_ptr _tmp646;(_tmp646=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp647=*((char*)_check_dyneither_subscript(_tmp646,sizeof(char),0)),((_tmp648='f',((_get_dyneither_size(_tmp646,sizeof(char))== 1  && (_tmp647 == '\000'  && _tmp648 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp646.curr)=_tmp648)))))));}goto _LL21;_LL28: if(_tmp21 != '\n')goto _LL2A;_LL29:
{char _tmp64B;char _tmp64A;struct _dyneither_ptr _tmp649;(_tmp649=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp64A=*((char*)_check_dyneither_subscript(_tmp649,sizeof(char),0)),((_tmp64B='\\',((_get_dyneither_size(_tmp649,sizeof(char))== 1  && (_tmp64A == '\000'  && _tmp64B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp649.curr)=_tmp64B)))))));}{char _tmp64E;char _tmp64D;struct _dyneither_ptr _tmp64C;(_tmp64C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp64D=*((char*)_check_dyneither_subscript(_tmp64C,sizeof(char),0)),((_tmp64E='n',((_get_dyneither_size(_tmp64C,sizeof(char))== 1  && (_tmp64D == '\000'  && _tmp64E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp64C.curr)=_tmp64E)))))));}goto _LL21;_LL2A: if(_tmp21 != '\r')goto _LL2C;_LL2B:
{char _tmp651;char _tmp650;struct _dyneither_ptr _tmp64F;(_tmp64F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp650=*((char*)_check_dyneither_subscript(_tmp64F,sizeof(char),0)),((_tmp651='\\',((_get_dyneither_size(_tmp64F,sizeof(char))== 1  && (_tmp650 == '\000'  && _tmp651 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp64F.curr)=_tmp651)))))));}{char _tmp654;char _tmp653;struct _dyneither_ptr _tmp652;(_tmp652=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp653=*((char*)_check_dyneither_subscript(_tmp652,sizeof(char),0)),((_tmp654='r',((_get_dyneither_size(_tmp652,sizeof(char))== 1  && (_tmp653 == '\000'  && _tmp654 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp652.curr)=_tmp654)))))));}goto _LL21;_LL2C: if(_tmp21 != '\t')goto _LL2E;_LL2D:
{char _tmp657;char _tmp656;struct _dyneither_ptr _tmp655;(_tmp655=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp656=*((char*)_check_dyneither_subscript(_tmp655,sizeof(char),0)),((_tmp657='\\',((_get_dyneither_size(_tmp655,sizeof(char))== 1  && (_tmp656 == '\000'  && _tmp657 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp655.curr)=_tmp657)))))));}{char _tmp65A;char _tmp659;struct _dyneither_ptr _tmp658;(_tmp658=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp659=*((char*)_check_dyneither_subscript(_tmp658,sizeof(char),0)),((_tmp65A='t',((_get_dyneither_size(_tmp658,sizeof(char))== 1  && (_tmp659 == '\000'  && _tmp65A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp658.curr)=_tmp65A)))))));}goto _LL21;_LL2E: if(_tmp21 != '\v')goto _LL30;_LL2F:
{char _tmp65D;char _tmp65C;struct _dyneither_ptr _tmp65B;(_tmp65B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp65C=*((char*)_check_dyneither_subscript(_tmp65B,sizeof(char),0)),((_tmp65D='\\',((_get_dyneither_size(_tmp65B,sizeof(char))== 1  && (_tmp65C == '\000'  && _tmp65D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp65B.curr)=_tmp65D)))))));}{char _tmp660;char _tmp65F;struct _dyneither_ptr _tmp65E;(_tmp65E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp65F=*((char*)_check_dyneither_subscript(_tmp65E,sizeof(char),0)),((_tmp660='v',((_get_dyneither_size(_tmp65E,sizeof(char))== 1  && (_tmp65F == '\000'  && _tmp660 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp65E.curr)=_tmp660)))))));}goto _LL21;_LL30: if(_tmp21 != '\\')goto _LL32;_LL31:
{char _tmp663;char _tmp662;struct _dyneither_ptr _tmp661;(_tmp661=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp662=*((char*)_check_dyneither_subscript(_tmp661,sizeof(char),0)),((_tmp663='\\',((_get_dyneither_size(_tmp661,sizeof(char))== 1  && (_tmp662 == '\000'  && _tmp663 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp661.curr)=_tmp663)))))));}{char _tmp666;char _tmp665;struct _dyneither_ptr _tmp664;(_tmp664=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp665=*((char*)_check_dyneither_subscript(_tmp664,sizeof(char),0)),((_tmp666='\\',((_get_dyneither_size(_tmp664,sizeof(char))== 1  && (_tmp665 == '\000'  && _tmp666 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp664.curr)=_tmp666)))))));}goto _LL21;_LL32: if(_tmp21 != '"')goto _LL34;_LL33:
{char _tmp669;char _tmp668;struct _dyneither_ptr _tmp667;(_tmp667=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp668=*((char*)_check_dyneither_subscript(_tmp667,sizeof(char),0)),((_tmp669='\\',((_get_dyneither_size(_tmp667,sizeof(char))== 1  && (_tmp668 == '\000'  && _tmp669 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp667.curr)=_tmp669)))))));}{char _tmp66C;char _tmp66B;struct _dyneither_ptr _tmp66A;(_tmp66A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp66B=*((char*)_check_dyneither_subscript(_tmp66A,sizeof(char),0)),((_tmp66C='"',((_get_dyneither_size(_tmp66A,sizeof(char))== 1  && (_tmp66B == '\000'  && _tmp66C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp66A.curr)=_tmp66C)))))));}goto _LL21;_LL34: _tmp22=_tmp21;_LL35:
# 342
 if(_tmp22 >= ' '  && _tmp22 <= '~'){char _tmp66F;char _tmp66E;struct _dyneither_ptr _tmp66D;(_tmp66D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp66E=*((char*)_check_dyneither_subscript(_tmp66D,sizeof(char),0)),((_tmp66F=_tmp22,((_get_dyneither_size(_tmp66D,sizeof(char))== 1  && (_tmp66E == '\000'  && _tmp66F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp66D.curr)=_tmp66F)))))));}else{
# 344
{char _tmp672;char _tmp671;struct _dyneither_ptr _tmp670;(_tmp670=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp671=*((char*)_check_dyneither_subscript(_tmp670,sizeof(char),0)),((_tmp672='\\',((_get_dyneither_size(_tmp670,sizeof(char))== 1  && (_tmp671 == '\000'  && _tmp672 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp670.curr)=_tmp672)))))));}
{char _tmp675;char _tmp674;struct _dyneither_ptr _tmp673;(_tmp673=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp674=*((char*)_check_dyneither_subscript(_tmp673,sizeof(char),0)),((_tmp675=(char)('0' + (_tmp22 >> 6 & 7)),((_get_dyneither_size(_tmp673,sizeof(char))== 1  && (_tmp674 == '\000'  && _tmp675 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp673.curr)=_tmp675)))))));}
{char _tmp678;char _tmp677;struct _dyneither_ptr _tmp676;(_tmp676=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp677=*((char*)_check_dyneither_subscript(_tmp676,sizeof(char),0)),((_tmp678=(char)('0' + (_tmp22 >> 3 & 7)),((_get_dyneither_size(_tmp676,sizeof(char))== 1  && (_tmp677 == '\000'  && _tmp678 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp676.curr)=_tmp678)))))));}{
char _tmp67B;char _tmp67A;struct _dyneither_ptr _tmp679;(_tmp679=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp67A=*((char*)_check_dyneither_subscript(_tmp679,sizeof(char),0)),((_tmp67B=(char)('0' + (_tmp22 & 7)),((_get_dyneither_size(_tmp679,sizeof(char))== 1  && (_tmp67A == '\000'  && _tmp67B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp679.curr)=_tmp67B)))))));};}
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
if(tq.q_restrict){struct Cyc_List_List*_tmp67C;l=((_tmp67C=_cycalloc(sizeof(*_tmp67C)),((_tmp67C->hd=Cyc_Absynpp_restrict_sp,((_tmp67C->tl=l,_tmp67C))))));}
if(tq.q_volatile){struct Cyc_List_List*_tmp67D;l=((_tmp67D=_cycalloc(sizeof(*_tmp67D)),((_tmp67D->hd=Cyc_Absynpp_volatile_sp,((_tmp67D->tl=l,_tmp67D))))));}
if(tq.print_const){struct Cyc_List_List*_tmp67E;l=((_tmp67E=_cycalloc(sizeof(*_tmp67E)),((_tmp67E->hd=Cyc_Absynpp_const_sp,((_tmp67E->tl=l,_tmp67E))))));}{
const char*_tmp681;const char*_tmp680;const char*_tmp67F;return Cyc_PP_egroup(((_tmp67F="",_tag_dyneither(_tmp67F,sizeof(char),1))),((_tmp680=" ",_tag_dyneither(_tmp680,sizeof(char),2))),((_tmp681=" ",_tag_dyneither(_tmp681,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));};}
# 370
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp72;enum Cyc_Absyn_AliasQual _tmp73;struct Cyc_Absyn_Kind*_tmp71=ka;_tmp72=_tmp71->kind;_tmp73=_tmp71->aliasqual;
switch(_tmp72){case Cyc_Absyn_AnyKind: _LL36:
# 374
 switch(_tmp73){case Cyc_Absyn_Aliasable: _LL38: {
const char*_tmp682;return(_tmp682="A",_tag_dyneither(_tmp682,sizeof(char),2));}case Cyc_Absyn_Unique: _LL39: {
const char*_tmp683;return(_tmp683="UA",_tag_dyneither(_tmp683,sizeof(char),3));}case Cyc_Absyn_Top: _LL3A: {
const char*_tmp684;return(_tmp684="TA",_tag_dyneither(_tmp684,sizeof(char),3));}}case Cyc_Absyn_MemKind: _LL37:
# 380
 switch(_tmp73){case Cyc_Absyn_Aliasable: _LL3D: {
const char*_tmp685;return(_tmp685="M",_tag_dyneither(_tmp685,sizeof(char),2));}case Cyc_Absyn_Unique: _LL3E: {
const char*_tmp686;return(_tmp686="UM",_tag_dyneither(_tmp686,sizeof(char),3));}case Cyc_Absyn_Top: _LL3F: {
const char*_tmp687;return(_tmp687="TM",_tag_dyneither(_tmp687,sizeof(char),3));}}case Cyc_Absyn_BoxKind: _LL3C:
# 386
 switch(_tmp73){case Cyc_Absyn_Aliasable: _LL42: {
const char*_tmp688;return(_tmp688="B",_tag_dyneither(_tmp688,sizeof(char),2));}case Cyc_Absyn_Unique: _LL43: {
const char*_tmp689;return(_tmp689="UB",_tag_dyneither(_tmp689,sizeof(char),3));}case Cyc_Absyn_Top: _LL44: {
const char*_tmp68A;return(_tmp68A="TB",_tag_dyneither(_tmp68A,sizeof(char),3));}}case Cyc_Absyn_RgnKind: _LL41:
# 392
 switch(_tmp73){case Cyc_Absyn_Aliasable: _LL47: {
const char*_tmp68B;return(_tmp68B="R",_tag_dyneither(_tmp68B,sizeof(char),2));}case Cyc_Absyn_Unique: _LL48: {
const char*_tmp68C;return(_tmp68C="UR",_tag_dyneither(_tmp68C,sizeof(char),3));}case Cyc_Absyn_Top: _LL49: {
const char*_tmp68D;return(_tmp68D="TR",_tag_dyneither(_tmp68D,sizeof(char),3));}}case Cyc_Absyn_EffKind: _LL46: {
# 397
const char*_tmp68E;return(_tmp68E="E",_tag_dyneither(_tmp68E,sizeof(char),2));}case Cyc_Absyn_IntKind: _LL4B: {
const char*_tmp68F;return(_tmp68F="I",_tag_dyneither(_tmp68F,sizeof(char),2));}}}
# 401
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 403
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp82=Cyc_Absyn_compress_kb(c);void*_tmp83=_tmp82;struct Cyc_Absyn_Kind*_tmp85;struct Cyc_Absyn_Kind*_tmp88;_LL4E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp84=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp83;if(_tmp84->tag != 0)goto _LL50;else{_tmp85=_tmp84->f1;}}_LL4F:
 return Cyc_Absynpp_kind2string(_tmp85);_LL50: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp86=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp83;if(_tmp86->tag != 1)goto _LL52;}_LL51:
# 407
 if(Cyc_PP_tex_output){
const char*_tmp690;return(_tmp690="{?}",_tag_dyneither(_tmp690,sizeof(char),4));}else{
const char*_tmp691;return(_tmp691="?",_tag_dyneither(_tmp691,sizeof(char),2));}_LL52: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp87=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp83;if(_tmp87->tag != 2)goto _LL4D;else{_tmp88=_tmp87->f2;}}_LL53: {
const char*_tmp695;void*_tmp694[1];struct Cyc_String_pa_PrintArg_struct _tmp693;return(struct _dyneither_ptr)((_tmp693.tag=0,((_tmp693.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp88)),((_tmp694[0]=& _tmp693,Cyc_aprintf(((_tmp695="<=%s",_tag_dyneither(_tmp695,sizeof(char),5))),_tag_dyneither(_tmp694,sizeof(void*),1))))))));}_LL4D:;}
# 414
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp8E=Cyc_Absyn_compress_kb(c);void*_tmp8F=_tmp8E;struct Cyc_Absyn_Kind*_tmp91;struct Cyc_Absyn_Kind*_tmp94;_LL55: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp90=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8F;if(_tmp90->tag != 0)goto _LL57;else{_tmp91=_tmp90->f1;}}_LL56:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp91));_LL57: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp92=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8F;if(_tmp92->tag != 1)goto _LL59;}_LL58:
# 418
 if(Cyc_PP_tex_output){
const char*_tmp696;return Cyc_PP_text_width(((_tmp696="{?}",_tag_dyneither(_tmp696,sizeof(char),4))),1);}else{
const char*_tmp697;return Cyc_PP_text(((_tmp697="?",_tag_dyneither(_tmp697,sizeof(char),2))));}_LL59: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp93=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F;if(_tmp93->tag != 2)goto _LL54;else{_tmp94=_tmp93->f2;}}_LL5A:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp94));_LL54:;}
# 425
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
const char*_tmp69A;const char*_tmp699;const char*_tmp698;return Cyc_PP_egroup(((_tmp698="<",_tag_dyneither(_tmp698,sizeof(char),2))),((_tmp699=">",_tag_dyneither(_tmp699,sizeof(char),2))),((_tmp69A=",",_tag_dyneither(_tmp69A,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));}
# 429
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr _tmp9A=*tv->name;
# 433
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(tv)){
# 435
const char*_tmp69B;struct _dyneither_ptr kstring=(_tmp69B="K",_tag_dyneither(_tmp69B,sizeof(char),2));
{void*_tmp9B=Cyc_Absyn_compress_kb(tv->kind);void*_tmp9C=_tmp9B;struct Cyc_Absyn_Kind*_tmp9E;struct Cyc_Absyn_Kind*_tmpA0;_LL5C: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9C;if(_tmp9D->tag != 2)goto _LL5E;else{_tmp9E=_tmp9D->f2;}}_LL5D:
 _tmpA0=_tmp9E;goto _LL5F;_LL5E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp9F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9C;if(_tmp9F->tag != 0)goto _LL60;else{_tmpA0=_tmp9F->f1;}}_LL5F:
 kstring=Cyc_Absynpp_kind2string(_tmpA0);goto _LL5B;_LL60:;_LL61:
 goto _LL5B;_LL5B:;}{
# 441
const char*_tmp6A0;void*_tmp69F[2];struct Cyc_String_pa_PrintArg_struct _tmp69E;struct Cyc_String_pa_PrintArg_struct _tmp69D;return Cyc_PP_text((struct _dyneither_ptr)((_tmp69D.tag=0,((_tmp69D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_tmp9A,sizeof(char),1)),((_tmp69E.tag=0,((_tmp69E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kstring),((_tmp69F[0]=& _tmp69E,((_tmp69F[1]=& _tmp69D,Cyc_aprintf(((_tmp6A0="`G%s%s",_tag_dyneither(_tmp6A0,sizeof(char),7))),_tag_dyneither(_tmp69F,sizeof(void*),2)))))))))))))));};}
# 443
return Cyc_PP_text(_tmp9A);}
# 446
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmpA6=Cyc_Absyn_compress_kb(tv->kind);void*_tmpA7=_tmpA6;struct Cyc_Absyn_Kind*_tmpAB;struct Cyc_Absyn_Kind*_tmpAD;_LL63: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA8=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA7;if(_tmpA8->tag != 1)goto _LL65;}_LL64:
 goto _LL66;_LL65: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA9=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7;if(_tmpA9->tag != 0)goto _LL67;else{if((_tmpA9->f1)->kind != Cyc_Absyn_BoxKind)goto _LL67;if((_tmpA9->f1)->aliasqual != Cyc_Absyn_Aliasable)goto _LL67;}}_LL66:
 return Cyc_Absynpp_tvar2doc(tv);_LL67: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAA=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7;if(_tmpAA->tag != 2)goto _LL69;else{_tmpAB=_tmpAA->f2;}}_LL68:
 _tmpAD=_tmpAB;goto _LL6A;_LL69: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpAC=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7;if(_tmpAC->tag != 0)goto _LL62;else{_tmpAD=_tmpAC->f1;}}_LL6A: {
const char*_tmp6A3;struct Cyc_PP_Doc*_tmp6A2[3];return(_tmp6A2[2]=Cyc_Absynpp_kind2doc(_tmpAD),((_tmp6A2[1]=Cyc_PP_text(((_tmp6A3="::",_tag_dyneither(_tmp6A3,sizeof(char),3)))),((_tmp6A2[0]=Cyc_Absynpp_tvar2doc(tv),Cyc_PP_cat(_tag_dyneither(_tmp6A2,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL62:;}
# 455
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
const char*_tmp6A6;const char*_tmp6A5;const char*_tmp6A4;return Cyc_PP_egroup(((_tmp6A4="<",_tag_dyneither(_tmp6A4,sizeof(char),2))),((_tmp6A5=">",_tag_dyneither(_tmp6A5,sizeof(char),2))),((_tmp6A6=",",_tag_dyneither(_tmp6A6,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}
# 459
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);{
const char*_tmp6A9;const char*_tmp6A8;const char*_tmp6A7;return Cyc_PP_egroup(((_tmp6A7="<",_tag_dyneither(_tmp6A7,sizeof(char),2))),((_tmp6A8=">",_tag_dyneither(_tmp6A8,sizeof(char),2))),((_tmp6A9=",",_tag_dyneither(_tmp6A9,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));};}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 465
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 469
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
const char*_tmp6AC;const char*_tmp6AB;const char*_tmp6AA;return Cyc_PP_group(((_tmp6AA="(",_tag_dyneither(_tmp6AA,sizeof(char),2))),((_tmp6AB=")",_tag_dyneither(_tmp6AB,sizeof(char),2))),((_tmp6AC=",",_tag_dyneither(_tmp6AC,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
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
const char*_tmp6AD;return Cyc_PP_text(((_tmp6AD=" _stdcall ",_tag_dyneither(_tmp6AD,sizeof(char),11))));}_LL77: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpC0=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpBE;if(_tmpC0->tag != 2)goto _LL79;}_LL78: {
const char*_tmp6AE;return Cyc_PP_text(((_tmp6AE=" _cdecl ",_tag_dyneither(_tmp6AE,sizeof(char),9))));}_LL79: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpC1=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpBE;if(_tmpC1->tag != 3)goto _LL7B;}_LL7A: {
const char*_tmp6AF;return Cyc_PP_text(((_tmp6AF=" _fastcall ",_tag_dyneither(_tmp6AF,sizeof(char),12))));}_LL7B:;_LL7C:
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
const char*_tmp6BA;const char*_tmp6B9;const char*_tmp6B8;const char*_tmp6B7;const char*_tmp6B6;struct Cyc_PP_Doc*_tmp6B5[3];return(_tmp6B5[2]=
# 506
Cyc_PP_text(((_tmp6B6=")",_tag_dyneither(_tmp6B6,sizeof(char),2)))),((_tmp6B5[1]=
# 505
Cyc_PP_group(((_tmp6B9="",_tag_dyneither(_tmp6B9,sizeof(char),1))),((_tmp6B8="",_tag_dyneither(_tmp6B8,sizeof(char),1))),((_tmp6B7=" ",_tag_dyneither(_tmp6B7,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts)),((_tmp6B5[0]=
# 504
Cyc_PP_text(((_tmp6BA=" __declspec(",_tag_dyneither(_tmp6BA,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp6B5,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 509
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 513
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL86:
 return Cyc_Absynpp_noncallconv2doc(atts);case Cyc_Cyclone_Gcc_c: _LL87: {
const char*_tmp6C3;const char*_tmp6C2;const char*_tmp6C1;const char*_tmp6C0;struct Cyc_PP_Doc*_tmp6BF[2];return(_tmp6BF[1]=
Cyc_PP_group(((_tmp6C2="((",_tag_dyneither(_tmp6C2,sizeof(char),3))),((_tmp6C1="))",_tag_dyneither(_tmp6C1,sizeof(char),3))),((_tmp6C0=",",_tag_dyneither(_tmp6C0,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp6BF[0]=
# 517
Cyc_PP_text(((_tmp6C3=" __attribute__",_tag_dyneither(_tmp6C3,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp6BF,sizeof(struct Cyc_PP_Doc*),2)))));}}}
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
const char*_tmp6C4;Cyc_Absynpp_cache_question=Cyc_PP_text_width(((_tmp6C4="{?}",_tag_dyneither(_tmp6C4,sizeof(char),4))),1);}else{
const char*_tmp6C5;Cyc_Absynpp_cache_question=Cyc_PP_text(((_tmp6C5="?",_tag_dyneither(_tmp6C5,sizeof(char),2))));}}
# 545
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 547
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output){
const char*_tmp6C6;Cyc_Absynpp_cache_lb=Cyc_PP_text_width(((_tmp6C6="{\\lb}",_tag_dyneither(_tmp6C6,sizeof(char),6))),1);}else{
const char*_tmp6C7;Cyc_Absynpp_cache_lb=Cyc_PP_text(((_tmp6C7="{",_tag_dyneither(_tmp6C7,sizeof(char),2))));}}
# 554
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 556
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output){
const char*_tmp6C8;Cyc_Absynpp_cache_rb=Cyc_PP_text_width(((_tmp6C8="{\\rb}",_tag_dyneither(_tmp6C8,sizeof(char),6))),1);}else{
const char*_tmp6C9;Cyc_Absynpp_cache_rb=Cyc_PP_text(((_tmp6C9="}",_tag_dyneither(_tmp6C9,sizeof(char),2))));}}
# 563
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 565
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output){
const char*_tmp6CA;Cyc_Absynpp_cache_dollar=Cyc_PP_text_width(((_tmp6CA="\\$",_tag_dyneither(_tmp6CA,sizeof(char),3))),1);}else{
const char*_tmp6CB;Cyc_Absynpp_cache_dollar=Cyc_PP_text(((_tmp6CB="$",_tag_dyneither(_tmp6CB,sizeof(char),2))));}}
# 572
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 574
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){
struct Cyc_PP_Doc*_tmp6CC[3];return(_tmp6CC[2]=Cyc_Absynpp_rb(),((_tmp6CC[1]=Cyc_PP_seq(sep,ss),((_tmp6CC[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp6CC,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 579
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpE2=(void*)tms->hd;void*_tmpE3=_tmpE2;struct Cyc_List_List*_tmpE8;_LL94: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpE4=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpE4->tag != 0)goto _LL96;}_LL95:
{const char*_tmp6CF;void*_tmp6CE;(_tmp6CE=0,Cyc_fprintf(Cyc_stderr,((_tmp6CF="Carray_mod ",_tag_dyneither(_tmp6CF,sizeof(char),12))),_tag_dyneither(_tmp6CE,sizeof(void*),0)));}goto _LL93;_LL96: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpE5=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpE5->tag != 1)goto _LL98;}_LL97:
{const char*_tmp6D2;void*_tmp6D1;(_tmp6D1=0,Cyc_fprintf(Cyc_stderr,((_tmp6D2="ConstArray_mod ",_tag_dyneither(_tmp6D2,sizeof(char),16))),_tag_dyneither(_tmp6D1,sizeof(void*),0)));}goto _LL93;_LL98: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpE6=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpE6->tag != 3)goto _LL9A;else{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpE7=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((void*)_tmpE6->f1);if(_tmpE7->tag != 1)goto _LL9A;else{_tmpE8=_tmpE7->f1;}}}_LL99:
# 585
{const char*_tmp6D5;void*_tmp6D4;(_tmp6D4=0,Cyc_fprintf(Cyc_stderr,((_tmp6D5="Function_mod(",_tag_dyneither(_tmp6D5,sizeof(char),14))),_tag_dyneither(_tmp6D4,sizeof(void*),0)));}
for(0;_tmpE8 != 0;_tmpE8=_tmpE8->tl){
struct _dyneither_ptr*_tmpF3=(*((struct _tuple8*)_tmpE8->hd)).f1;
if(_tmpF3 == 0){const char*_tmp6D8;void*_tmp6D7;(_tmp6D7=0,Cyc_fprintf(Cyc_stderr,((_tmp6D8="?",_tag_dyneither(_tmp6D8,sizeof(char),2))),_tag_dyneither(_tmp6D7,sizeof(void*),0)));}else{
void*_tmp6D9;(_tmp6D9=0,Cyc_fprintf(Cyc_stderr,*_tmpF3,_tag_dyneither(_tmp6D9,sizeof(void*),0)));}
if(_tmpE8->tl != 0){const char*_tmp6DC;void*_tmp6DB;(_tmp6DB=0,Cyc_fprintf(Cyc_stderr,((_tmp6DC=",",_tag_dyneither(_tmp6DC,sizeof(char),2))),_tag_dyneither(_tmp6DB,sizeof(void*),0)));}}
# 592
{const char*_tmp6DF;void*_tmp6DE;(_tmp6DE=0,Cyc_fprintf(Cyc_stderr,((_tmp6DF=") ",_tag_dyneither(_tmp6DF,sizeof(char),3))),_tag_dyneither(_tmp6DE,sizeof(void*),0)));}
goto _LL93;_LL9A: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpE9=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpE9->tag != 3)goto _LL9C;}_LL9B:
# 595
{const char*_tmp6E2;void*_tmp6E1;(_tmp6E1=0,Cyc_fprintf(Cyc_stderr,((_tmp6E2="Function_mod()",_tag_dyneither(_tmp6E2,sizeof(char),15))),_tag_dyneither(_tmp6E1,sizeof(void*),0)));}goto _LL93;_LL9C: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpEA=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpEA->tag != 5)goto _LL9E;}_LL9D:
{const char*_tmp6E5;void*_tmp6E4;(_tmp6E4=0,Cyc_fprintf(Cyc_stderr,((_tmp6E5="Attributes_mod ",_tag_dyneither(_tmp6E5,sizeof(char),16))),_tag_dyneither(_tmp6E4,sizeof(void*),0)));}goto _LL93;_LL9E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpEB=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpEB->tag != 4)goto _LLA0;}_LL9F:
{const char*_tmp6E8;void*_tmp6E7;(_tmp6E7=0,Cyc_fprintf(Cyc_stderr,((_tmp6E8="TypeParams_mod ",_tag_dyneither(_tmp6E8,sizeof(char),16))),_tag_dyneither(_tmp6E7,sizeof(void*),0)));}goto _LL93;_LLA0: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpEC=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpEC->tag != 2)goto _LL93;}_LLA1:
{const char*_tmp6EB;void*_tmp6EA;(_tmp6EA=0,Cyc_fprintf(Cyc_stderr,((_tmp6EB="Pointer_mod ",_tag_dyneither(_tmp6EB,sizeof(char),13))),_tag_dyneither(_tmp6EA,sizeof(void*),0)));}goto _LL93;_LL93:;}{
# 600
const char*_tmp6EE;void*_tmp6ED;(_tmp6ED=0,Cyc_fprintf(Cyc_stderr,((_tmp6EE="\n",_tag_dyneither(_tmp6EE,sizeof(char),2))),_tag_dyneither(_tmp6ED,sizeof(void*),0)));};}
# 603
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 605
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
const char*_tmp6F3;const char*_tmp6F2;struct Cyc_PP_Doc*_tmp6F1[3];struct Cyc_PP_Doc*p_rest=(_tmp6F1[2]=Cyc_PP_text(((_tmp6F2=")",_tag_dyneither(_tmp6F2,sizeof(char),2)))),((_tmp6F1[1]=rest,((_tmp6F1[0]=Cyc_PP_text(((_tmp6F3="(",_tag_dyneither(_tmp6F3,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6F1,sizeof(struct Cyc_PP_Doc*),3)))))));
void*_tmp105=(void*)tms->hd;void*_tmp106=_tmp105;union Cyc_Absyn_Constraint*_tmp108;struct Cyc_Absyn_Exp*_tmp10A;union Cyc_Absyn_Constraint*_tmp10B;void*_tmp10D;struct Cyc_List_List*_tmp10F;struct Cyc_List_List*_tmp111;unsigned int _tmp112;int _tmp113;void*_tmp115;union Cyc_Absyn_Constraint*_tmp116;union Cyc_Absyn_Constraint*_tmp117;union Cyc_Absyn_Constraint*_tmp118;struct Cyc_Absyn_Tqual _tmp119;_LLA3: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp107=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp107->tag != 0)goto _LLA5;else{_tmp108=_tmp107->f1;}}_LLA4:
# 611
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6F8;const char*_tmp6F7;struct Cyc_PP_Doc*_tmp6F6[2];return(_tmp6F6[1]=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp108)?
Cyc_PP_text(((_tmp6F7="[]@zeroterm",_tag_dyneither(_tmp6F7,sizeof(char),12)))): Cyc_PP_text(((_tmp6F8="[]",_tag_dyneither(_tmp6F8,sizeof(char),3)))),((_tmp6F6[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F6,sizeof(struct Cyc_PP_Doc*),2)))));};_LLA5: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp109=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp109->tag != 1)goto _LLA7;else{_tmp10A=_tmp109->f1;_tmp10B=_tmp109->f2;}}_LLA6:
# 615
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6FF;const char*_tmp6FE;const char*_tmp6FD;struct Cyc_PP_Doc*_tmp6FC[4];return(_tmp6FC[3]=
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp10B)?Cyc_PP_text(((_tmp6FD="]@zeroterm",_tag_dyneither(_tmp6FD,sizeof(char),11)))): Cyc_PP_text(((_tmp6FE="]",_tag_dyneither(_tmp6FE,sizeof(char),2)))),((_tmp6FC[2]=
# 616
Cyc_Absynpp_exp2doc(_tmp10A),((_tmp6FC[1]=Cyc_PP_text(((_tmp6FF="[",_tag_dyneither(_tmp6FF,sizeof(char),2)))),((_tmp6FC[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6FC,sizeof(struct Cyc_PP_Doc*),4)))))))));};_LLA7: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp10C=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp10C->tag != 3)goto _LLA9;else{_tmp10D=(void*)_tmp10C->f1;}}_LLA8:
# 619
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmp121=_tmp10D;struct Cyc_List_List*_tmp123;int _tmp124;struct Cyc_Absyn_VarargInfo*_tmp125;void*_tmp126;struct Cyc_List_List*_tmp127;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_List_List*_tmp12B;unsigned int _tmp12C;_LLB0: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp122=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp121;if(_tmp122->tag != 1)goto _LLB2;else{_tmp123=_tmp122->f1;_tmp124=_tmp122->f2;_tmp125=_tmp122->f3;_tmp126=(void*)_tmp122->f4;_tmp127=_tmp122->f5;_tmp128=_tmp122->f6;_tmp129=_tmp122->f7;}}_LLB1: {
# 622
struct Cyc_PP_Doc*_tmp700[2];return(_tmp700[1]=Cyc_Absynpp_funargs2doc(_tmp123,_tmp124,_tmp125,_tmp126,_tmp127,_tmp128,_tmp129),((_tmp700[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp700,sizeof(struct Cyc_PP_Doc*),2)))));}_LLB2: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp12A=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp121;if(_tmp12A->tag != 0)goto _LLAF;else{_tmp12B=_tmp12A->f1;_tmp12C=_tmp12A->f2;}}_LLB3: {
# 624
const char*_tmp707;const char*_tmp706;const char*_tmp705;struct Cyc_PP_Doc*_tmp704[2];return(_tmp704[1]=Cyc_PP_group(((_tmp707="(",_tag_dyneither(_tmp707,sizeof(char),2))),((_tmp706=")",_tag_dyneither(_tmp706,sizeof(char),2))),((_tmp705=",",_tag_dyneither(_tmp705,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp12B)),((_tmp704[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp704,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAF:;};_LLA9: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp10E=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp10E->tag != 5)goto _LLAB;else{_tmp10F=_tmp10E->f2;}}_LLAA:
# 627
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLB4:
# 629
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp708[2];return(_tmp708[1]=Cyc_Absynpp_atts2doc(_tmp10F),((_tmp708[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp708,sizeof(struct Cyc_PP_Doc*),2)))));};case Cyc_Cyclone_Vc_c: _LLB5:
# 632
 if(Cyc_Absynpp_next_is_pointer(tms->tl)){
struct Cyc_PP_Doc*_tmp709[2];return(_tmp709[1]=rest,((_tmp709[0]=Cyc_Absynpp_callconv2doc(_tmp10F),Cyc_PP_cat(_tag_dyneither(_tmp709,sizeof(struct Cyc_PP_Doc*),2)))));}
return rest;}_LLAB: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp110=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp110->tag != 4)goto _LLAD;else{_tmp111=_tmp110->f1;_tmp112=_tmp110->f2;_tmp113=_tmp110->f3;}}_LLAC:
# 637
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp113){
struct Cyc_PP_Doc*_tmp70A[2];return(_tmp70A[1]=Cyc_Absynpp_ktvars2doc(_tmp111),((_tmp70A[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp70A,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 641
struct Cyc_PP_Doc*_tmp70B[2];return(_tmp70B[1]=Cyc_Absynpp_tvars2doc(_tmp111),((_tmp70B[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp70B,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAD: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp114=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp114->tag != 2)goto _LLA2;else{_tmp115=(_tmp114->f1).rgn;_tmp116=(_tmp114->f1).nullable;_tmp117=(_tmp114->f1).bounds;_tmp118=(_tmp114->f1).zero_term;_tmp119=_tmp114->f2;}}_LLAE: {
# 645
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(_tmp119);
{void*_tmp136=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp117);void*_tmp137=_tmp136;struct Cyc_Absyn_Exp*_tmp13A;_LLB8: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp138=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp137;if(_tmp138->tag != 0)goto _LLBA;}_LLB9:
 ptr=Cyc_Absynpp_question();goto _LLB7;_LLBA: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp139=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp137;if(_tmp139->tag != 1)goto _LLB7;else{_tmp13A=_tmp139->f1;}}_LLBB:
# 653
{const char*_tmp70D;const char*_tmp70C;ptr=Cyc_PP_text(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp116)?(_tmp70D="*",_tag_dyneither(_tmp70D,sizeof(char),2)):((_tmp70C="@",_tag_dyneither(_tmp70C,sizeof(char),2))));}{
struct _tuple11 _tmp13D=Cyc_Evexp_eval_const_uint_exp(_tmp13A);unsigned int _tmp13F;int _tmp140;struct _tuple11 _tmp13E=_tmp13D;_tmp13F=_tmp13E.f1;_tmp140=_tmp13E.f2;
if(!_tmp140  || _tmp13F != 1){
struct Cyc_PP_Doc*_tmp70E[4];ptr=((_tmp70E[3]=Cyc_Absynpp_rb(),((_tmp70E[2]=Cyc_Absynpp_exp2doc(_tmp13A),((_tmp70E[1]=Cyc_Absynpp_lb(),((_tmp70E[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp70E,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB7;};_LLB7:;}
# 659
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp118)){
const char*_tmp70F;ztd=Cyc_PP_text(((_tmp70F="@zeroterm",_tag_dyneither(_tmp70F,sizeof(char),10))));}else{
if(is_char_ptr  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp118)){
const char*_tmp710;ztd=Cyc_PP_text(((_tmp710="@nozeroterm",_tag_dyneither(_tmp710,sizeof(char),12))));}}}
# 665
{void*_tmp144=Cyc_Tcutil_compress(_tmp115);void*_tmp145=_tmp144;_LLBD: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp146=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp145;if(_tmp146->tag != 20)goto _LLBF;}_LLBE:
 goto _LLBC;_LLBF:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp147=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp145;if(_tmp147->tag != 1)goto _LLC1;}if(!Cyc_Absynpp_print_for_cycdoc)goto _LLC1;_LLC0:
 goto _LLBC;_LLC1:;_LLC2:
 rgd=Cyc_Absynpp_rgn2doc(_tmp115);_LLBC:;}{
# 670
const char*_tmp711;struct Cyc_PP_Doc*spacer1=tqd != mt  && (ztd != mt  || rgd != mt)?Cyc_PP_text(((_tmp711=" ",_tag_dyneither(_tmp711,sizeof(char),2)))): mt;
const char*_tmp712;struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(((_tmp712=" ",_tag_dyneither(_tmp712,sizeof(char),2)))): mt;
struct Cyc_PP_Doc*_tmp713[7];return(_tmp713[6]=rest,((_tmp713[5]=spacer2,((_tmp713[4]=tqd,((_tmp713[3]=spacer1,((_tmp713[2]=rgd,((_tmp713[1]=ztd,((_tmp713[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp713,sizeof(struct Cyc_PP_Doc*),7)))))))))))))));};}_LLA2:;};}
# 676
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp14E=Cyc_Tcutil_compress(t);void*_tmp14F=_tmp14E;_LLC4: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp150=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp14F;if(_tmp150->tag != 20)goto _LLC6;}_LLC5: {
const char*_tmp714;return Cyc_PP_text(((_tmp714="`H",_tag_dyneither(_tmp714,sizeof(char),3))));}_LLC6: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp151=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp14F;if(_tmp151->tag != 21)goto _LLC8;}_LLC7: {
const char*_tmp715;return Cyc_PP_text(((_tmp715="`U",_tag_dyneither(_tmp715,sizeof(char),3))));}_LLC8: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp152=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp14F;if(_tmp152->tag != 22)goto _LLCA;}_LLC9: {
const char*_tmp716;return Cyc_PP_text(((_tmp716="`RC",_tag_dyneither(_tmp716,sizeof(char),4))));}_LLCA:;_LLCB:
 return Cyc_Absynpp_ntyp2doc(t);_LLC3:;}
# 686
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 690
void*_tmp156=Cyc_Tcutil_compress(t);void*_tmp157=_tmp156;void*_tmp159;struct Cyc_List_List*_tmp15B;_LLCD: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp158=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp157;if(_tmp158->tag != 23)goto _LLCF;else{_tmp159=(void*)_tmp158->f1;}}_LLCE:
{struct Cyc_List_List*_tmp717;*rgions=((_tmp717=_cycalloc(sizeof(*_tmp717)),((_tmp717->hd=Cyc_Absynpp_rgn2doc(_tmp159),((_tmp717->tl=*rgions,_tmp717))))));}goto _LLCC;_LLCF: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp15A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp157;if(_tmp15A->tag != 24)goto _LLD1;else{_tmp15B=_tmp15A->f1;}}_LLD0:
# 693
 for(0;_tmp15B != 0;_tmp15B=_tmp15B->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp15B->hd);}
# 696
goto _LLCC;_LLD1:;_LLD2:
{struct Cyc_List_List*_tmp718;*effects=((_tmp718=_cycalloc(sizeof(*_tmp718)),((_tmp718->hd=Cyc_Absynpp_typ2doc(t),((_tmp718->tl=*effects,_tmp718))))));}goto _LLCC;_LLCC:;}
# 701
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0  && effects != 0){
const char*_tmp71B;const char*_tmp71A;const char*_tmp719;return Cyc_PP_group(((_tmp719="",_tag_dyneither(_tmp719,sizeof(char),1))),((_tmp71A="",_tag_dyneither(_tmp71A,sizeof(char),1))),((_tmp71B="+",_tag_dyneither(_tmp71B,sizeof(char),2))),effects);}else{
# 709
const char*_tmp71C;struct Cyc_PP_Doc*_tmp161=Cyc_Absynpp_group_braces(((_tmp71C=",",_tag_dyneither(_tmp71C,sizeof(char),2))),rgions);
struct Cyc_List_List*_tmp720;const char*_tmp71F;const char*_tmp71E;const char*_tmp71D;return Cyc_PP_group(((_tmp71D="",_tag_dyneither(_tmp71D,sizeof(char),1))),((_tmp71E="",_tag_dyneither(_tmp71E,sizeof(char),1))),((_tmp71F="+",_tag_dyneither(_tmp71F,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((_tmp720=_cycalloc(sizeof(*_tmp720)),((_tmp720->hd=_tmp161,((_tmp720->tl=0,_tmp720))))))));}}
# 714
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
switch(k){case Cyc_Absyn_StructA: _LLD3: {
const char*_tmp721;return Cyc_PP_text(((_tmp721="struct ",_tag_dyneither(_tmp721,sizeof(char),8))));}case Cyc_Absyn_UnionA: _LLD4: {
const char*_tmp722;return Cyc_PP_text(((_tmp722="union ",_tag_dyneither(_tmp722,sizeof(char),7))));}}}
# 722
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp169=t;struct Cyc_Core_Opt*_tmp16F;void*_tmp170;int _tmp171;struct Cyc_Core_Opt*_tmp172;struct Cyc_Absyn_Tvar*_tmp174;union Cyc_Absyn_DatatypeInfoU _tmp176;struct Cyc_List_List*_tmp177;union Cyc_Absyn_DatatypeFieldInfoU _tmp179;struct Cyc_List_List*_tmp17A;enum Cyc_Absyn_Sign _tmp17C;enum Cyc_Absyn_Size_of _tmp17D;int _tmp17F;struct Cyc_List_List*_tmp181;union Cyc_Absyn_AggrInfoU _tmp183;struct Cyc_List_List*_tmp184;enum Cyc_Absyn_AggrKind _tmp186;struct Cyc_List_List*_tmp187;struct Cyc_List_List*_tmp189;struct _tuple0*_tmp18B;struct Cyc_Absyn_Exp*_tmp18D;struct Cyc_Absyn_Exp*_tmp18F;struct _dyneither_ptr _tmp191;struct _tuple0*_tmp193;struct Cyc_List_List*_tmp194;struct Cyc_Absyn_Typedefdecl*_tmp195;void*_tmp197;void*_tmp199;void*_tmp19A;void*_tmp19C;void*_tmp1A1;struct Cyc_Absyn_Aggrdecl*_tmp1A6;struct Cyc_Absyn_Enumdecl*_tmp1A9;struct Cyc_Absyn_Datatypedecl*_tmp1AC;_LLD7: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp16A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp169;if(_tmp16A->tag != 8)goto _LLD9;}_LLD8: {
# 726
const char*_tmp723;return Cyc_PP_text(((_tmp723="[[[array]]]",_tag_dyneither(_tmp723,sizeof(char),12))));}_LLD9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp16B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp169;if(_tmp16B->tag != 9)goto _LLDB;}_LLDA:
 return Cyc_PP_nil_doc();_LLDB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp16C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp169;if(_tmp16C->tag != 5)goto _LLDD;}_LLDC:
 return Cyc_PP_nil_doc();_LLDD: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp16D=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp169;if(_tmp16D->tag != 0)goto _LLDF;}_LLDE:
# 730
{const char*_tmp724;s=Cyc_PP_text(((_tmp724="void",_tag_dyneither(_tmp724,sizeof(char),5))));}goto _LLD6;_LLDF: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp16E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp169;if(_tmp16E->tag != 1)goto _LLE1;else{_tmp16F=_tmp16E->f1;_tmp170=(void*)_tmp16E->f2;_tmp171=_tmp16E->f3;_tmp172=_tmp16E->f4;}}_LLE0:
# 732
 if(_tmp170 != 0)
# 734
return Cyc_Absynpp_ntyp2doc(_tmp170);else{
# 736
const char*_tmp725;struct _dyneither_ptr kindstring=_tmp16F == 0?(_tmp725="K",_tag_dyneither(_tmp725,sizeof(char),2)): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)_tmp16F->v);
const char*_tmp72A;void*_tmp729[2];struct Cyc_String_pa_PrintArg_struct _tmp728;struct Cyc_Int_pa_PrintArg_struct _tmp727;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp727.tag=1,((_tmp727.f1=(unsigned long)_tmp171,((_tmp728.tag=0,((_tmp728.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kindstring),((_tmp729[0]=& _tmp728,((_tmp729[1]=& _tmp727,Cyc_aprintf(((_tmp72A="`E%s%d",_tag_dyneither(_tmp72A,sizeof(char),7))),_tag_dyneither(_tmp729,sizeof(void*),2)))))))))))))));}
# 753 "absynpp.cyc"
goto _LLD6;_LLE1: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp173=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp169;if(_tmp173->tag != 2)goto _LLE3;else{_tmp174=_tmp173->f1;}}_LLE2:
# 755
 s=Cyc_Absynpp_tvar2doc(_tmp174);
if(Cyc_Absynpp_print_all_kinds){
const char*_tmp72D;struct Cyc_PP_Doc*_tmp72C[3];s=((_tmp72C[2]=Cyc_Absynpp_kindbound2doc(_tmp174->kind),((_tmp72C[1]=Cyc_PP_text(((_tmp72D="::",_tag_dyneither(_tmp72D,sizeof(char),3)))),((_tmp72C[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp72C,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 759
goto _LLD6;_LLE3: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp175=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp169;if(_tmp175->tag != 3)goto _LLE5;else{_tmp176=(_tmp175->f1).datatype_info;_tmp177=(_tmp175->f1).targs;}}_LLE4:
# 761
{union Cyc_Absyn_DatatypeInfoU _tmp1B6=_tmp176;struct _tuple0*_tmp1B7;int _tmp1B8;struct _tuple0*_tmp1B9;int _tmp1BA;_LL116: if((_tmp1B6.UnknownDatatype).tag != 1)goto _LL118;_tmp1B7=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1B6.UnknownDatatype).val).name;_tmp1B8=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1B6.UnknownDatatype).val).is_extensible;_LL117:
# 763
 _tmp1B9=_tmp1B7;_tmp1BA=_tmp1B8;goto _LL119;_LL118: if((_tmp1B6.KnownDatatype).tag != 2)goto _LL115;_tmp1B9=(*((struct Cyc_Absyn_Datatypedecl**)(_tmp1B6.KnownDatatype).val))->name;_tmp1BA=(*((struct Cyc_Absyn_Datatypedecl**)(_tmp1B6.KnownDatatype).val))->is_extensible;_LL119: {
# 765
const char*_tmp72E;struct Cyc_PP_Doc*_tmp1BB=Cyc_PP_text(((_tmp72E="datatype ",_tag_dyneither(_tmp72E,sizeof(char),10))));
const char*_tmp72F;struct Cyc_PP_Doc*_tmp1BC=_tmp1BA?Cyc_PP_text(((_tmp72F="@extensible ",_tag_dyneither(_tmp72F,sizeof(char),13)))): Cyc_PP_nil_doc();
{struct Cyc_PP_Doc*_tmp730[4];s=((_tmp730[3]=Cyc_Absynpp_tps2doc(_tmp177),((_tmp730[2]=Cyc_Absynpp_qvar2doc(_tmp1B9),((_tmp730[1]=_tmp1BB,((_tmp730[0]=_tmp1BC,Cyc_PP_cat(_tag_dyneither(_tmp730,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL115;}_LL115:;}
# 770
goto _LLD6;_LLE5: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp178=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp169;if(_tmp178->tag != 4)goto _LLE7;else{_tmp179=(_tmp178->f1).field_info;_tmp17A=(_tmp178->f1).targs;}}_LLE6:
# 772
{union Cyc_Absyn_DatatypeFieldInfoU _tmp1C0=_tmp179;struct _tuple0*_tmp1C1;struct _tuple0*_tmp1C2;int _tmp1C3;struct _tuple0*_tmp1C4;int _tmp1C5;struct _tuple0*_tmp1C6;_LL11B: if((_tmp1C0.UnknownDatatypefield).tag != 1)goto _LL11D;_tmp1C1=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1C0.UnknownDatatypefield).val).datatype_name;_tmp1C2=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1C0.UnknownDatatypefield).val).field_name;_tmp1C3=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1C0.UnknownDatatypefield).val).is_extensible;_LL11C:
# 774
 _tmp1C4=_tmp1C1;_tmp1C5=_tmp1C3;_tmp1C6=_tmp1C2;goto _LL11E;_LL11D: if((_tmp1C0.KnownDatatypefield).tag != 2)goto _LL11A;_tmp1C4=(((struct _tuple1)(_tmp1C0.KnownDatatypefield).val).f1)->name;_tmp1C5=(((struct _tuple1)(_tmp1C0.KnownDatatypefield).val).f1)->is_extensible;_tmp1C6=(((struct _tuple1)(_tmp1C0.KnownDatatypefield).val).f2)->name;_LL11E: {
# 777
const char*_tmp732;const char*_tmp731;struct Cyc_PP_Doc*_tmp1C7=Cyc_PP_text(_tmp1C5?(_tmp732="@extensible datatype ",_tag_dyneither(_tmp732,sizeof(char),22)):((_tmp731="datatype ",_tag_dyneither(_tmp731,sizeof(char),10))));
{const char*_tmp735;struct Cyc_PP_Doc*_tmp734[4];s=((_tmp734[3]=Cyc_Absynpp_qvar2doc(_tmp1C6),((_tmp734[2]=Cyc_PP_text(((_tmp735=".",_tag_dyneither(_tmp735,sizeof(char),2)))),((_tmp734[1]=Cyc_Absynpp_qvar2doc(_tmp1C4),((_tmp734[0]=_tmp1C7,Cyc_PP_cat(_tag_dyneither(_tmp734,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL11A;}_LL11A:;}
# 781
goto _LLD6;_LLE7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp17B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp169;if(_tmp17B->tag != 6)goto _LLE9;else{_tmp17C=_tmp17B->f1;_tmp17D=_tmp17B->f2;}}_LLE8: {
# 783
struct _dyneither_ptr sns;
struct _dyneither_ptr ts;
switch(_tmp17C){case Cyc_Absyn_None: _LL11F:
 goto _LL120;case Cyc_Absyn_Signed: _LL120:
{const char*_tmp736;sns=((_tmp736="",_tag_dyneither(_tmp736,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL121:
{const char*_tmp737;sns=((_tmp737="unsigned ",_tag_dyneither(_tmp737,sizeof(char),10)));}break;}
# 790
switch(_tmp17D){case Cyc_Absyn_Char_sz: _LL123:
# 792
 switch(_tmp17C){case Cyc_Absyn_None: _LL125:
{const char*_tmp738;sns=((_tmp738="",_tag_dyneither(_tmp738,sizeof(char),1)));}break;case Cyc_Absyn_Signed: _LL126:
{const char*_tmp739;sns=((_tmp739="signed ",_tag_dyneither(_tmp739,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL127:
{const char*_tmp73A;sns=((_tmp73A="unsigned ",_tag_dyneither(_tmp73A,sizeof(char),10)));}break;}
# 797
{const char*_tmp73B;ts=((_tmp73B="char",_tag_dyneither(_tmp73B,sizeof(char),5)));}
break;case Cyc_Absyn_Short_sz: _LL124:
{const char*_tmp73C;ts=((_tmp73C="short",_tag_dyneither(_tmp73C,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz: _LL129:
{const char*_tmp73D;ts=((_tmp73D="int",_tag_dyneither(_tmp73D,sizeof(char),4)));}break;case Cyc_Absyn_Long_sz: _LL12A:
{const char*_tmp73E;ts=((_tmp73E="long",_tag_dyneither(_tmp73E,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL12B:
# 803
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL12D:
{const char*_tmp73F;ts=((_tmp73F="long long",_tag_dyneither(_tmp73F,sizeof(char),10)));}break;case Cyc_Cyclone_Vc_c: _LL12E:
{const char*_tmp740;ts=((_tmp740="__int64",_tag_dyneither(_tmp740,sizeof(char),8)));}break;}
# 807
break;}
# 809
{const char*_tmp745;void*_tmp744[2];struct Cyc_String_pa_PrintArg_struct _tmp743;struct Cyc_String_pa_PrintArg_struct _tmp742;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp742.tag=0,((_tmp742.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts),((_tmp743.tag=0,((_tmp743.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns),((_tmp744[0]=& _tmp743,((_tmp744[1]=& _tmp742,Cyc_aprintf(((_tmp745="%s%s",_tag_dyneither(_tmp745,sizeof(char),5))),_tag_dyneither(_tmp744,sizeof(void*),2)))))))))))))));}
goto _LLD6;}_LLE9: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp17E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp169;if(_tmp17E->tag != 7)goto _LLEB;else{_tmp17F=_tmp17E->f1;}}_LLEA:
# 812
 if(_tmp17F == 0){
const char*_tmp746;s=Cyc_PP_text(((_tmp746="float",_tag_dyneither(_tmp746,sizeof(char),6))));}else{
if(_tmp17F == 1){
const char*_tmp747;s=Cyc_PP_text(((_tmp747="double",_tag_dyneither(_tmp747,sizeof(char),7))));}else{
# 817
const char*_tmp748;s=Cyc_PP_text(((_tmp748="long double",_tag_dyneither(_tmp748,sizeof(char),12))));}}
goto _LLD6;_LLEB: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp180=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp169;if(_tmp180->tag != 10)goto _LLED;else{_tmp181=_tmp180->f1;}}_LLEC:
# 820
{struct Cyc_PP_Doc*_tmp749[2];s=((_tmp749[1]=Cyc_Absynpp_args2doc(_tmp181),((_tmp749[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp749,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD6;_LLED: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp182=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp169;if(_tmp182->tag != 11)goto _LLEF;else{_tmp183=(_tmp182->f1).aggr_info;_tmp184=(_tmp182->f1).targs;}}_LLEE: {
# 823
struct _tuple10 _tmp1DF=Cyc_Absyn_aggr_kinded_name(_tmp183);enum Cyc_Absyn_AggrKind _tmp1E1;struct _tuple0*_tmp1E2;struct _tuple10 _tmp1E0=_tmp1DF;_tmp1E1=_tmp1E0.f1;_tmp1E2=_tmp1E0.f2;
{struct Cyc_PP_Doc*_tmp74A[3];s=((_tmp74A[2]=Cyc_Absynpp_tps2doc(_tmp184),((_tmp74A[1]=Cyc_Absynpp_qvar2doc(_tmp1E2),((_tmp74A[0]=Cyc_Absynpp_aggr_kind2doc(_tmp1E1),Cyc_PP_cat(_tag_dyneither(_tmp74A,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLD6;}_LLEF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp185=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp169;if(_tmp185->tag != 12)goto _LLF1;else{_tmp186=_tmp185->f1;_tmp187=_tmp185->f2;}}_LLF0:
# 827
{struct Cyc_PP_Doc*_tmp74B[4];s=((_tmp74B[3]=
# 829
Cyc_Absynpp_rb(),((_tmp74B[2]=
# 828
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp187)),((_tmp74B[1]=
# 827
Cyc_Absynpp_lb(),((_tmp74B[0]=Cyc_Absynpp_aggr_kind2doc(_tmp186),Cyc_PP_cat(_tag_dyneither(_tmp74B,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 830
goto _LLD6;_LLF1: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp188=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp169;if(_tmp188->tag != 14)goto _LLF3;else{_tmp189=_tmp188->f1;}}_LLF2:
# 832
{const char*_tmp74E;struct Cyc_PP_Doc*_tmp74D[4];s=((_tmp74D[3]=Cyc_Absynpp_rb(),((_tmp74D[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp189)),((_tmp74D[1]=Cyc_Absynpp_lb(),((_tmp74D[0]=Cyc_PP_text(((_tmp74E="enum ",_tag_dyneither(_tmp74E,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp74D,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLD6;_LLF3: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp18A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp169;if(_tmp18A->tag != 13)goto _LLF5;else{_tmp18B=_tmp18A->f1;}}_LLF4:
# 834
{const char*_tmp751;struct Cyc_PP_Doc*_tmp750[2];s=((_tmp750[1]=Cyc_Absynpp_qvar2doc(_tmp18B),((_tmp750[0]=Cyc_PP_text(((_tmp751="enum ",_tag_dyneither(_tmp751,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp750,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLD6;_LLF5: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp18C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp169;if(_tmp18C->tag != 18)goto _LLF7;else{_tmp18D=_tmp18C->f1;}}_LLF6:
# 836
{const char*_tmp756;const char*_tmp755;struct Cyc_PP_Doc*_tmp754[3];s=((_tmp754[2]=Cyc_PP_text(((_tmp755=")",_tag_dyneither(_tmp755,sizeof(char),2)))),((_tmp754[1]=Cyc_Absynpp_exp2doc(_tmp18D),((_tmp754[0]=Cyc_PP_text(((_tmp756="valueof_t(",_tag_dyneither(_tmp756,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp754,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LLF7: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp18E=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp169;if(_tmp18E->tag != 27)goto _LLF9;else{_tmp18F=_tmp18E->f1;}}_LLF8:
# 838
{const char*_tmp75B;const char*_tmp75A;struct Cyc_PP_Doc*_tmp759[3];s=((_tmp759[2]=Cyc_PP_text(((_tmp75A=")",_tag_dyneither(_tmp75A,sizeof(char),2)))),((_tmp759[1]=Cyc_Absynpp_exp2doc(_tmp18F),((_tmp759[0]=Cyc_PP_text(((_tmp75B="typeof(",_tag_dyneither(_tmp75B,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp759,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LLF9: {struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmp190=(struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp169;if(_tmp190->tag != 28)goto _LLFB;else{_tmp191=_tmp190->f1;}}_LLFA:
# 840
 s=Cyc_PP_text(_tmp191);goto _LLD6;_LLFB: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp192=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp169;if(_tmp192->tag != 17)goto _LLFD;else{_tmp193=_tmp192->f1;_tmp194=_tmp192->f2;_tmp195=_tmp192->f3;}}_LLFC:
# 847
{struct Cyc_PP_Doc*_tmp75C[2];s=((_tmp75C[1]=Cyc_Absynpp_tps2doc(_tmp194),((_tmp75C[0]=Cyc_Absynpp_qvar2doc(_tmp193),Cyc_PP_cat(_tag_dyneither(_tmp75C,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD6;_LLFD: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp196=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp169;if(_tmp196->tag != 15)goto _LLFF;else{_tmp197=(void*)_tmp196->f1;}}_LLFE:
# 850
{const char*_tmp761;const char*_tmp760;struct Cyc_PP_Doc*_tmp75F[3];s=((_tmp75F[2]=Cyc_PP_text(((_tmp760=">",_tag_dyneither(_tmp760,sizeof(char),2)))),((_tmp75F[1]=Cyc_Absynpp_rgn2doc(_tmp197),((_tmp75F[0]=Cyc_PP_text(((_tmp761="region_t<",_tag_dyneither(_tmp761,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp75F,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LLFF: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp198=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp169;if(_tmp198->tag != 16)goto _LL101;else{_tmp199=(void*)_tmp198->f1;_tmp19A=(void*)_tmp198->f2;}}_LL100:
# 852
{const char*_tmp768;const char*_tmp767;const char*_tmp766;struct Cyc_PP_Doc*_tmp765[5];s=((_tmp765[4]=
Cyc_PP_text(((_tmp766=">",_tag_dyneither(_tmp766,sizeof(char),2)))),((_tmp765[3]=
# 852
Cyc_Absynpp_rgn2doc(_tmp19A),((_tmp765[2]=Cyc_PP_text(((_tmp767=",",_tag_dyneither(_tmp767,sizeof(char),2)))),((_tmp765[1]=Cyc_Absynpp_rgn2doc(_tmp199),((_tmp765[0]=Cyc_PP_text(((_tmp768="dynregion_t<",_tag_dyneither(_tmp768,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp765,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LLD6;_LL101: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp19B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp169;if(_tmp19B->tag != 19)goto _LL103;else{_tmp19C=(void*)_tmp19B->f1;}}_LL102:
# 855
{const char*_tmp76D;const char*_tmp76C;struct Cyc_PP_Doc*_tmp76B[3];s=((_tmp76B[2]=Cyc_PP_text(((_tmp76C=">",_tag_dyneither(_tmp76C,sizeof(char),2)))),((_tmp76B[1]=Cyc_Absynpp_typ2doc(_tmp19C),((_tmp76B[0]=Cyc_PP_text(((_tmp76D="tag_t<",_tag_dyneither(_tmp76D,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp76B,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LL103: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp19D=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp169;if(_tmp19D->tag != 21)goto _LL105;}_LL104:
 goto _LL106;_LL105: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp19E=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp169;if(_tmp19E->tag != 20)goto _LL107;}_LL106:
 goto _LL108;_LL107: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp19F=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp169;if(_tmp19F->tag != 22)goto _LL109;}_LL108:
# 859
 s=Cyc_Absynpp_rgn2doc(t);goto _LLD6;_LL109: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1A0=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp169;if(_tmp1A0->tag != 25)goto _LL10B;else{_tmp1A1=(void*)_tmp1A0->f1;}}_LL10A:
# 861
{const char*_tmp772;const char*_tmp771;struct Cyc_PP_Doc*_tmp770[3];s=((_tmp770[2]=Cyc_PP_text(((_tmp771=")",_tag_dyneither(_tmp771,sizeof(char),2)))),((_tmp770[1]=Cyc_Absynpp_typ2doc(_tmp1A1),((_tmp770[0]=Cyc_PP_text(((_tmp772="regions(",_tag_dyneither(_tmp772,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp770,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LL10B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1A2=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp169;if(_tmp1A2->tag != 23)goto _LL10D;}_LL10C:
 goto _LL10E;_LL10D: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1A3=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp169;if(_tmp1A3->tag != 24)goto _LL10F;}_LL10E:
# 864
 s=Cyc_Absynpp_eff2doc(t);goto _LLD6;_LL10F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1A4=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp169;if(_tmp1A4->tag != 26)goto _LL111;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp1A5=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp1A4->f1)->r;if(_tmp1A5->tag != 0)goto _LL111;else{_tmp1A6=_tmp1A5->f1;}}}_LL110:
# 866
 s=Cyc_Absynpp_aggrdecl2doc(_tmp1A6);goto _LLD6;_LL111: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1A7=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp169;if(_tmp1A7->tag != 26)goto _LL113;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp1A8=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp1A7->f1)->r;if(_tmp1A8->tag != 1)goto _LL113;else{_tmp1A9=_tmp1A8->f1;}}}_LL112:
# 868
 s=Cyc_Absynpp_enumdecl2doc(_tmp1A9);goto _LLD6;_LL113: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1AA=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp169;if(_tmp1AA->tag != 26)goto _LLD6;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp1AB=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp1AA->f1)->r;if(_tmp1AB->tag != 2)goto _LLD6;else{_tmp1AC=_tmp1AB->f1;}}}_LL114:
# 870
 s=Cyc_Absynpp_datatypedecl2doc(_tmp1AC);goto _LLD6;_LLD6:;}
# 872
return s;}
# 875
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _dyneither_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple16{void*f1;void*f2;};
# 879
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
void*_tmp1FE;void*_tmp1FF;struct _tuple16*_tmp1FD=cmp;_tmp1FE=_tmp1FD->f1;_tmp1FF=_tmp1FD->f2;{
const char*_tmp775;struct Cyc_PP_Doc*_tmp774[3];return(_tmp774[2]=Cyc_Absynpp_rgn2doc(_tmp1FF),((_tmp774[1]=Cyc_PP_text(((_tmp775=" > ",_tag_dyneither(_tmp775,sizeof(char),4)))),((_tmp774[0]=Cyc_Absynpp_rgn2doc(_tmp1FE),Cyc_PP_cat(_tag_dyneither(_tmp774,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 884
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
const char*_tmp778;const char*_tmp777;const char*_tmp776;return Cyc_PP_group(((_tmp776="",_tag_dyneither(_tmp776,sizeof(char),1))),((_tmp777="",_tag_dyneither(_tmp777,sizeof(char),1))),((_tmp778=",",_tag_dyneither(_tmp778,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
# 888
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _dyneither_ptr*_tmp205=(*t).f1;
struct Cyc_Core_Opt*_tmp779;struct Cyc_Core_Opt*dopt=_tmp205 == 0?0:((_tmp779=_cycalloc(sizeof(*_tmp779)),((_tmp779->v=Cyc_PP_text(*_tmp205),_tmp779))));
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 894
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 898
struct Cyc_List_List*_tmp207=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs){
const char*_tmp77C;struct Cyc_List_List*_tmp77B;_tmp207=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp207,((_tmp77B=_cycalloc(sizeof(*_tmp77B)),((_tmp77B->hd=Cyc_PP_text(((_tmp77C="...",_tag_dyneither(_tmp77C,sizeof(char),4)))),((_tmp77B->tl=0,_tmp77B)))))));}else{
if(cyc_varargs != 0){
const char*_tmp785;const char*_tmp784;const char*_tmp783;struct _tuple8*_tmp782;struct Cyc_PP_Doc*_tmp781[3];struct Cyc_PP_Doc*_tmp20A=(_tmp781[2]=
# 905
Cyc_Absynpp_funarg2doc(((_tmp782=_cycalloc(sizeof(*_tmp782)),((_tmp782->f1=cyc_varargs->name,((_tmp782->f2=cyc_varargs->tq,((_tmp782->f3=cyc_varargs->type,_tmp782))))))))),((_tmp781[1]=
# 904
cyc_varargs->inject?Cyc_PP_text(((_tmp783=" inject ",_tag_dyneither(_tmp783,sizeof(char),9)))): Cyc_PP_text(((_tmp784=" ",_tag_dyneither(_tmp784,sizeof(char),2)))),((_tmp781[0]=
# 903
Cyc_PP_text(((_tmp785="...",_tag_dyneither(_tmp785,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp781,sizeof(struct Cyc_PP_Doc*),3)))))));
# 907
struct Cyc_List_List*_tmp786;_tmp207=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp207,((_tmp786=_cycalloc(sizeof(*_tmp786)),((_tmp786->hd=_tmp20A,((_tmp786->tl=0,_tmp786)))))));}}{
# 909
const char*_tmp789;const char*_tmp788;const char*_tmp787;struct Cyc_PP_Doc*_tmp211=Cyc_PP_group(((_tmp787="",_tag_dyneither(_tmp787,sizeof(char),1))),((_tmp788="",_tag_dyneither(_tmp788,sizeof(char),1))),((_tmp789=",",_tag_dyneither(_tmp789,sizeof(char),2))),_tmp207);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){
const char*_tmp78C;struct Cyc_PP_Doc*_tmp78B[3];_tmp211=((_tmp78B[2]=Cyc_Absynpp_eff2doc(effopt),((_tmp78B[1]=Cyc_PP_text(((_tmp78C=";",_tag_dyneither(_tmp78C,sizeof(char),2)))),((_tmp78B[0]=_tmp211,Cyc_PP_cat(_tag_dyneither(_tmp78B,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){
const char*_tmp78F;struct Cyc_PP_Doc*_tmp78E[3];_tmp211=((_tmp78E[2]=Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp78E[1]=Cyc_PP_text(((_tmp78F=":",_tag_dyneither(_tmp78F,sizeof(char),2)))),((_tmp78E[0]=_tmp211,Cyc_PP_cat(_tag_dyneither(_tmp78E,sizeof(struct Cyc_PP_Doc*),3))))))));}{
const char*_tmp794;const char*_tmp793;struct Cyc_PP_Doc*_tmp792[3];struct Cyc_PP_Doc*_tmp216=(_tmp792[2]=Cyc_PP_text(((_tmp793=")",_tag_dyneither(_tmp793,sizeof(char),2)))),((_tmp792[1]=_tmp211,((_tmp792[0]=Cyc_PP_text(((_tmp794="(",_tag_dyneither(_tmp794,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp792,sizeof(struct Cyc_PP_Doc*),3)))))));
if(req != 0){
const char*_tmp799;const char*_tmp798;struct Cyc_PP_Doc*_tmp797[4];_tmp216=((_tmp797[3]=Cyc_PP_text(((_tmp798=")",_tag_dyneither(_tmp798,sizeof(char),2)))),((_tmp797[2]=Cyc_Absynpp_exp2doc(req),((_tmp797[1]=Cyc_PP_text(((_tmp799=" @requires(",_tag_dyneither(_tmp799,sizeof(char),12)))),((_tmp797[0]=_tmp216,Cyc_PP_cat(_tag_dyneither(_tmp797,sizeof(struct Cyc_PP_Doc*),4))))))))));}
if(ens != 0){
const char*_tmp79E;const char*_tmp79D;struct Cyc_PP_Doc*_tmp79C[4];_tmp216=((_tmp79C[3]=Cyc_PP_text(((_tmp79D=")",_tag_dyneither(_tmp79D,sizeof(char),2)))),((_tmp79C[2]=Cyc_Absynpp_exp2doc(ens),((_tmp79C[1]=Cyc_PP_text(((_tmp79E=" @ensures(",_tag_dyneither(_tmp79E,sizeof(char),11)))),((_tmp79C[0]=_tmp216,Cyc_PP_cat(_tag_dyneither(_tmp79C,sizeof(struct Cyc_PP_Doc*),4))))))))));}
return _tmp216;};};}
# 922
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg){
struct _dyneither_ptr*_tmp224;struct Cyc_Absyn_Tqual _tmp225;void*_tmp226;struct _tuple8*_tmp223=arg;_tmp224=_tmp223->f1;_tmp225=_tmp223->f2;_tmp226=_tmp223->f3;{
struct _tuple8*_tmp79F;return(_tmp79F=_cycalloc(sizeof(*_tmp79F)),((_tmp79F->f1=_tmp224,((_tmp79F->f2=_tmp225,((_tmp79F->f3=_tmp226,_tmp79F)))))));};}
# 927
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){return Cyc_PP_text(*v);}
# 929
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp228=0;
int match;
{union Cyc_Absyn_Nmspace _tmp229=(*q).f1;union Cyc_Absyn_Nmspace _tmp22A=_tmp229;struct Cyc_List_List*_tmp22B;struct Cyc_List_List*_tmp22C;struct Cyc_List_List*_tmp22D;_LL131: if((_tmp22A.Loc_n).tag != 4)goto _LL133;_LL132:
# 934
 _tmp22B=0;goto _LL134;_LL133: if((_tmp22A.Rel_n).tag != 1)goto _LL135;_tmp22B=(struct Cyc_List_List*)(_tmp22A.Rel_n).val;_LL134:
# 936
 match=0;
_tmp228=_tmp22B;
goto _LL130;_LL135: if((_tmp22A.C_n).tag != 3)goto _LL137;_tmp22C=(struct Cyc_List_List*)(_tmp22A.C_n).val;_LL136:
# 940
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp22C,Cyc_Absynpp_curr_namespace);
# 942
goto _LL130;_LL137: if((_tmp22A.Abs_n).tag != 2)goto _LL130;_tmp22D=(struct Cyc_List_List*)(_tmp22A.Abs_n).val;_LL138:
# 944
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp22D,Cyc_Absynpp_curr_namespace);
{struct Cyc_List_List*_tmp7A0;_tmp228=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(_tmp7A0=_cycalloc(sizeof(*_tmp7A0)),((_tmp7A0->hd=Cyc_Absynpp_cyc_stringptr,((_tmp7A0->tl=_tmp22D,_tmp7A0))))): _tmp22D;}
goto _LL130;_LL130:;}
# 948
if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp7A2;struct Cyc_List_List*_tmp7A1;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp228,(
(_tmp7A1=_cycalloc(sizeof(*_tmp7A1)),((_tmp7A1->hd=(*q).f2,((_tmp7A1->tl=0,_tmp7A1))))))),((_tmp7A2="_",_tag_dyneither(_tmp7A2,sizeof(char),2))));}else{
# 953
if(match)
return*(*q).f2;else{
# 956
const char*_tmp7A4;struct Cyc_List_List*_tmp7A3;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp228,(
(_tmp7A3=_cycalloc(sizeof(*_tmp7A3)),((_tmp7A3->hd=(*q).f2,((_tmp7A3->tl=0,_tmp7A3))))))),((_tmp7A4="::",_tag_dyneither(_tmp7A4,sizeof(char),3))));}}}
# 961
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 965
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp233=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output){
# 969
const char*_tmp7A6;const char*_tmp7A5;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp7A5="\\textbf{",_tag_dyneither(_tmp7A5,sizeof(char),9))),(struct _dyneither_ptr)_tmp233),((_tmp7A6="}",_tag_dyneither(_tmp7A6,sizeof(char),2)))),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp233));}else{
return Cyc_PP_text(_tmp233);}}
# 974
struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 976
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 979
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp236=(*v).f1;union Cyc_Absyn_Nmspace _tmp237=_tmp236;struct Cyc_List_List*_tmp238;struct Cyc_List_List*_tmp239;_LL13A: if((_tmp237.Loc_n).tag != 4)goto _LL13C;_LL13B:
 goto _LL13D;_LL13C: if((_tmp237.Rel_n).tag != 1)goto _LL13E;if((struct Cyc_List_List*)(_tmp237.Rel_n).val != 0)goto _LL13E;_LL13D:
 return*(*v).f2;_LL13E: if((_tmp237.C_n).tag != 3)goto _LL140;_tmp238=(struct Cyc_List_List*)(_tmp237.C_n).val;_LL13F:
 _tmp239=_tmp238;goto _LL141;_LL140: if((_tmp237.Abs_n).tag != 2)goto _LL142;_tmp239=(struct Cyc_List_List*)(_tmp237.Abs_n).val;_LL141:
# 985
 if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp239,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 988
goto _LL143;}_LL142:;_LL143: {
# 990
const char*_tmp7A7;return(struct _dyneither_ptr)Cyc_strconcat(((_tmp7A7="/* bad namespace : */ ",_tag_dyneither(_tmp7A7,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));}_LL139:;}else{
# 993
return*(*v).f2;}}
# 996
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 999
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _dyneither_ptr _tmp23B=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output){
# 1003
const char*_tmp7A9;const char*_tmp7A8;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp7A8="\\textbf{",_tag_dyneither(_tmp7A8,sizeof(char),9))),(struct _dyneither_ptr)_tmp23B),((_tmp7A9="}",_tag_dyneither(_tmp7A9,sizeof(char),2)))),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp23B));}else{
return Cyc_PP_text(_tmp23B);}}
# 1008
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}
# 1019 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp23E=e->r;void*_tmp23F=_tmp23E;enum Cyc_Absyn_Primop _tmp243;struct Cyc_Absyn_Exp*_tmp24D;struct Cyc_Absyn_Exp*_tmp24F;_LL145: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp240=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp240->tag != 0)goto _LL147;}_LL146:
 goto _LL148;_LL147: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp241=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp241->tag != 1)goto _LL149;}_LL148:
 return 10000;_LL149: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp242=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp242->tag != 2)goto _LL14B;else{_tmp243=_tmp242->f1;}}_LL14A:
# 1024
 switch(_tmp243){case Cyc_Absyn_Plus: _LL195:
 return 100;case Cyc_Absyn_Times: _LL196:
 return 110;case Cyc_Absyn_Minus: _LL197:
 return 100;case Cyc_Absyn_Div: _LL198:
 goto _LL199;case Cyc_Absyn_Mod: _LL199:
 return 110;case Cyc_Absyn_Eq: _LL19A:
 goto _LL19B;case Cyc_Absyn_Neq: _LL19B:
 return 70;case Cyc_Absyn_Gt: _LL19C:
 goto _LL19D;case Cyc_Absyn_Lt: _LL19D:
 goto _LL19E;case Cyc_Absyn_Gte: _LL19E:
 goto _LL19F;case Cyc_Absyn_Lte: _LL19F:
 return 80;case Cyc_Absyn_Not: _LL1A0:
 goto _LL1A1;case Cyc_Absyn_Bitnot: _LL1A1:
 return 130;case Cyc_Absyn_Bitand: _LL1A2:
 return 60;case Cyc_Absyn_Bitor: _LL1A3:
 return 40;case Cyc_Absyn_Bitxor: _LL1A4:
 return 50;case Cyc_Absyn_Bitlshift: _LL1A5:
 return 90;case Cyc_Absyn_Bitlrshift: _LL1A6:
 return 80;case Cyc_Absyn_Bitarshift: _LL1A7:
 return 80;case Cyc_Absyn_Numelts: _LL1A8:
 return 140;}_LL14B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp244=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp244->tag != 3)goto _LL14D;}_LL14C:
# 1046
 return 20;_LL14D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp245=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp245->tag != 4)goto _LL14F;}_LL14E:
 return 130;_LL14F: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp246=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp246->tag != 5)goto _LL151;}_LL150:
 return 30;_LL151: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp247=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp247->tag != 6)goto _LL153;}_LL152:
 return 35;_LL153: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp248=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp248->tag != 7)goto _LL155;}_LL154:
 return 30;_LL155: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp249=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp249->tag != 8)goto _LL157;}_LL156:
 return 10;_LL157: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp24A=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp24A->tag != 9)goto _LL159;}_LL158:
 return 140;_LL159: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp24B=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp24B->tag != 10)goto _LL15B;}_LL15A:
 return 130;_LL15B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp24C=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp24C->tag != 11)goto _LL15D;else{_tmp24D=_tmp24C->f1;}}_LL15C:
 return Cyc_Absynpp_exp_prec(_tmp24D);_LL15D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp24E=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp24E->tag != 12)goto _LL15F;else{_tmp24F=_tmp24E->f1;}}_LL15E:
 return Cyc_Absynpp_exp_prec(_tmp24F);_LL15F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp250=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp250->tag != 13)goto _LL161;}_LL160:
 return 120;_LL161: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp251=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp251->tag != 15)goto _LL163;}_LL162:
 return 15;_LL163: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp252=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp252->tag != 14)goto _LL165;}_LL164:
 goto _LL166;_LL165: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp253=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp253->tag != 16)goto _LL167;}_LL166:
 goto _LL168;_LL167: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp254=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp254->tag != 17)goto _LL169;}_LL168:
 goto _LL16A;_LL169: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp255=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp255->tag != 38)goto _LL16B;}_LL16A:
 goto _LL16C;_LL16B: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp256=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp256->tag != 39)goto _LL16D;}_LL16C:
 goto _LL16E;_LL16D: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp257=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp257->tag != 37)goto _LL16F;}_LL16E:
 goto _LL170;_LL16F: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp258=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp258->tag != 18)goto _LL171;}_LL170:
 goto _LL172;_LL171: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp259=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp259->tag != 19)goto _LL173;}_LL172:
 return 130;_LL173: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp25A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp25A->tag != 20)goto _LL175;}_LL174:
 goto _LL176;_LL175: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp25B=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp25B->tag != 21)goto _LL177;}_LL176:
 goto _LL178;_LL177: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp25C=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp25C->tag != 22)goto _LL179;}_LL178:
 return 140;_LL179: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp25D=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp25D->tag != 23)goto _LL17B;}_LL17A:
 return 150;_LL17B: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp25E=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp25E->tag != 24)goto _LL17D;}_LL17C:
 goto _LL17E;_LL17D: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp25F=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp25F->tag != 25)goto _LL17F;}_LL17E:
 goto _LL180;_LL17F: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp260=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp260->tag != 26)goto _LL181;}_LL180:
 goto _LL182;_LL181: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp261=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp261->tag != 27)goto _LL183;}_LL182:
 goto _LL184;_LL183: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp262=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp262->tag != 28)goto _LL185;}_LL184:
 goto _LL186;_LL185: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp263=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp263->tag != 29)goto _LL187;}_LL186:
 goto _LL188;_LL187: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp264=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp264->tag != 30)goto _LL189;}_LL188:
 goto _LL18A;_LL189: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp265=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp265->tag != 31)goto _LL18B;}_LL18A:
 goto _LL18C;_LL18B: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp266=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp266->tag != 32)goto _LL18D;}_LL18C:
 goto _LL18E;_LL18D: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp267=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp267->tag != 33)goto _LL18F;}_LL18E:
 goto _LL190;_LL18F: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp268=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp268->tag != 34)goto _LL191;}_LL190:
 goto _LL192;_LL191: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp269=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp269->tag != 35)goto _LL193;}_LL192:
 return 140;_LL193: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp26A=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp23F;if(_tmp26A->tag != 36)goto _LL144;}_LL194:
 return 10000;_LL144:;}
# 1086
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1090
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp26B=e->r;void*_tmp26C=_tmp26B;union Cyc_Absyn_Cnst _tmp26E;struct _tuple0*_tmp270;enum Cyc_Absyn_Primop _tmp272;struct Cyc_List_List*_tmp273;struct Cyc_Absyn_Exp*_tmp275;struct Cyc_Core_Opt*_tmp276;struct Cyc_Absyn_Exp*_tmp277;struct Cyc_Absyn_Exp*_tmp279;enum Cyc_Absyn_Incrementor _tmp27A;struct Cyc_Absyn_Exp*_tmp27C;struct Cyc_Absyn_Exp*_tmp27D;struct Cyc_Absyn_Exp*_tmp27E;struct Cyc_Absyn_Exp*_tmp280;struct Cyc_Absyn_Exp*_tmp281;struct Cyc_Absyn_Exp*_tmp283;struct Cyc_Absyn_Exp*_tmp284;struct Cyc_Absyn_Exp*_tmp286;struct Cyc_Absyn_Exp*_tmp287;struct Cyc_Absyn_Exp*_tmp289;struct Cyc_List_List*_tmp28A;struct Cyc_Absyn_Exp*_tmp28C;struct Cyc_Absyn_Exp*_tmp28E;struct Cyc_Absyn_Exp*_tmp290;void*_tmp292;struct Cyc_Absyn_Exp*_tmp293;struct Cyc_Absyn_Exp*_tmp295;struct Cyc_Absyn_Exp*_tmp297;struct Cyc_Absyn_Exp*_tmp298;void*_tmp29A;struct Cyc_Absyn_Exp*_tmp29C;void*_tmp29E;int _tmp2A0;struct _dyneither_ptr _tmp2A1;struct Cyc_Absyn_Exp*_tmp2A3;struct _dyneither_ptr*_tmp2A4;void*_tmp2A6;struct Cyc_List_List*_tmp2A7;struct Cyc_Absyn_Exp*_tmp2A9;struct Cyc_Absyn_Exp*_tmp2AB;struct _dyneither_ptr*_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AE;struct _dyneither_ptr*_tmp2AF;struct Cyc_Absyn_Exp*_tmp2B1;struct Cyc_Absyn_Exp*_tmp2B2;struct Cyc_List_List*_tmp2B4;struct _tuple8*_tmp2B6;struct Cyc_List_List*_tmp2B7;struct Cyc_List_List*_tmp2B9;struct Cyc_Absyn_Vardecl*_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BC;struct Cyc_Absyn_Exp*_tmp2BD;struct Cyc_Absyn_Exp*_tmp2BF;void*_tmp2C0;struct _tuple0*_tmp2C2;struct Cyc_List_List*_tmp2C3;struct Cyc_List_List*_tmp2C4;struct Cyc_List_List*_tmp2C6;struct Cyc_List_List*_tmp2C8;struct Cyc_Absyn_Datatypefield*_tmp2C9;struct _tuple0*_tmp2CB;struct _tuple0*_tmp2CD;int _tmp2CF;struct Cyc_Absyn_Exp*_tmp2D0;void**_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D2;int _tmp2D3;struct Cyc_Absyn_Exp*_tmp2D5;struct Cyc_Absyn_Exp*_tmp2D6;struct Cyc_Core_Opt*_tmp2D8;struct Cyc_List_List*_tmp2D9;struct Cyc_Absyn_Stmt*_tmp2DB;_LL1AB: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp26D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp26D->tag != 0)goto _LL1AD;else{_tmp26E=_tmp26D->f1;}}_LL1AC:
 s=Cyc_Absynpp_cnst2doc(_tmp26E);goto _LL1AA;_LL1AD: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp26F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp26F->tag != 1)goto _LL1AF;else{_tmp270=_tmp26F->f1;}}_LL1AE:
 s=Cyc_Absynpp_qvar2doc(_tmp270);goto _LL1AA;_LL1AF: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp271=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp271->tag != 2)goto _LL1B1;else{_tmp272=_tmp271->f1;_tmp273=_tmp271->f2;}}_LL1B0:
 s=Cyc_Absynpp_primapp2doc(myprec,_tmp272,_tmp273);goto _LL1AA;_LL1B1: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp274=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp274->tag != 3)goto _LL1B3;else{_tmp275=_tmp274->f1;_tmp276=_tmp274->f2;_tmp277=_tmp274->f3;}}_LL1B2:
# 1098
{const char*_tmp7AE;const char*_tmp7AD;struct Cyc_PP_Doc*_tmp7AC[5];s=((_tmp7AC[4]=
# 1102
Cyc_Absynpp_exp2doc_prec(myprec,_tmp277),((_tmp7AC[3]=
# 1101
Cyc_PP_text(((_tmp7AD="= ",_tag_dyneither(_tmp7AD,sizeof(char),3)))),((_tmp7AC[2]=
# 1100
_tmp276 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp276->v),((_tmp7AC[1]=
# 1099
Cyc_PP_text(((_tmp7AE=" ",_tag_dyneither(_tmp7AE,sizeof(char),2)))),((_tmp7AC[0]=
# 1098
Cyc_Absynpp_exp2doc_prec(myprec,_tmp275),Cyc_PP_cat(_tag_dyneither(_tmp7AC,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1103
goto _LL1AA;_LL1B3: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp278=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp278->tag != 4)goto _LL1B5;else{_tmp279=_tmp278->f1;_tmp27A=_tmp278->f2;}}_LL1B4: {
# 1105
struct Cyc_PP_Doc*_tmp2DF=Cyc_Absynpp_exp2doc_prec(myprec,_tmp279);
switch(_tmp27A){case Cyc_Absyn_PreInc: _LL1FB:
{const char*_tmp7B1;struct Cyc_PP_Doc*_tmp7B0[2];s=((_tmp7B0[1]=_tmp2DF,((_tmp7B0[0]=Cyc_PP_text(((_tmp7B1="++",_tag_dyneither(_tmp7B1,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp7B0,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PreDec: _LL1FC:
{const char*_tmp7B4;struct Cyc_PP_Doc*_tmp7B3[2];s=((_tmp7B3[1]=_tmp2DF,((_tmp7B3[0]=Cyc_PP_text(((_tmp7B4="--",_tag_dyneither(_tmp7B4,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp7B3,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostInc: _LL1FD:
{const char*_tmp7B7;struct Cyc_PP_Doc*_tmp7B6[2];s=((_tmp7B6[1]=Cyc_PP_text(((_tmp7B7="++",_tag_dyneither(_tmp7B7,sizeof(char),3)))),((_tmp7B6[0]=_tmp2DF,Cyc_PP_cat(_tag_dyneither(_tmp7B6,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1FE:
{const char*_tmp7BA;struct Cyc_PP_Doc*_tmp7B9[2];s=((_tmp7B9[1]=Cyc_PP_text(((_tmp7BA="--",_tag_dyneither(_tmp7BA,sizeof(char),3)))),((_tmp7B9[0]=_tmp2DF,Cyc_PP_cat(_tag_dyneither(_tmp7B9,sizeof(struct Cyc_PP_Doc*),2))))));}break;}
# 1112
goto _LL1AA;}_LL1B5: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp27B=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp27B->tag != 5)goto _LL1B7;else{_tmp27C=_tmp27B->f1;_tmp27D=_tmp27B->f2;_tmp27E=_tmp27B->f3;}}_LL1B6:
# 1114
{const char*_tmp7BF;const char*_tmp7BE;struct Cyc_PP_Doc*_tmp7BD[5];s=((_tmp7BD[4]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp27E),((_tmp7BD[3]=Cyc_PP_text(((_tmp7BE=" : ",_tag_dyneither(_tmp7BE,sizeof(char),4)))),((_tmp7BD[2]=
# 1114
Cyc_Absynpp_exp2doc_prec(0,_tmp27D),((_tmp7BD[1]=Cyc_PP_text(((_tmp7BF=" ? ",_tag_dyneither(_tmp7BF,sizeof(char),4)))),((_tmp7BD[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27C),Cyc_PP_cat(_tag_dyneither(_tmp7BD,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1116
goto _LL1AA;_LL1B7: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp27F=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp27F->tag != 6)goto _LL1B9;else{_tmp280=_tmp27F->f1;_tmp281=_tmp27F->f2;}}_LL1B8:
# 1118
{const char*_tmp7C2;struct Cyc_PP_Doc*_tmp7C1[3];s=((_tmp7C1[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp281),((_tmp7C1[1]=Cyc_PP_text(((_tmp7C2=" && ",_tag_dyneither(_tmp7C2,sizeof(char),5)))),((_tmp7C1[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp280),Cyc_PP_cat(_tag_dyneither(_tmp7C1,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1AA;_LL1B9: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp282=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp282->tag != 7)goto _LL1BB;else{_tmp283=_tmp282->f1;_tmp284=_tmp282->f2;}}_LL1BA:
# 1121
{const char*_tmp7C5;struct Cyc_PP_Doc*_tmp7C4[3];s=((_tmp7C4[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp284),((_tmp7C4[1]=Cyc_PP_text(((_tmp7C5=" || ",_tag_dyneither(_tmp7C5,sizeof(char),5)))),((_tmp7C4[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp283),Cyc_PP_cat(_tag_dyneither(_tmp7C4,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1AA;_LL1BB: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp285=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp285->tag != 8)goto _LL1BD;else{_tmp286=_tmp285->f1;_tmp287=_tmp285->f2;}}_LL1BC:
# 1126
{const char*_tmp7CC;const char*_tmp7CB;const char*_tmp7CA;struct Cyc_PP_Doc*_tmp7C9[5];s=((_tmp7C9[4]=Cyc_PP_text(((_tmp7CA=")",_tag_dyneither(_tmp7CA,sizeof(char),2)))),((_tmp7C9[3]=Cyc_Absynpp_exp2doc(_tmp287),((_tmp7C9[2]=Cyc_PP_text(((_tmp7CB=", ",_tag_dyneither(_tmp7CB,sizeof(char),3)))),((_tmp7C9[1]=Cyc_Absynpp_exp2doc(_tmp286),((_tmp7C9[0]=Cyc_PP_text(((_tmp7CC="(",_tag_dyneither(_tmp7CC,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7C9,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1AA;_LL1BD: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp288=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp288->tag != 9)goto _LL1BF;else{_tmp289=_tmp288->f1;_tmp28A=_tmp288->f2;}}_LL1BE:
# 1129
{const char*_tmp7D1;const char*_tmp7D0;struct Cyc_PP_Doc*_tmp7CF[4];s=((_tmp7CF[3]=
# 1132
Cyc_PP_text(((_tmp7D0=")",_tag_dyneither(_tmp7D0,sizeof(char),2)))),((_tmp7CF[2]=
# 1131
Cyc_Absynpp_exps2doc_prec(20,_tmp28A),((_tmp7CF[1]=
# 1130
Cyc_PP_text(((_tmp7D1="(",_tag_dyneither(_tmp7D1,sizeof(char),2)))),((_tmp7CF[0]=
# 1129
Cyc_Absynpp_exp2doc_prec(myprec,_tmp289),Cyc_PP_cat(_tag_dyneither(_tmp7CF,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1133
goto _LL1AA;_LL1BF: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp28B=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp28B->tag != 10)goto _LL1C1;else{_tmp28C=_tmp28B->f1;}}_LL1C0:
# 1135
{const char*_tmp7D4;struct Cyc_PP_Doc*_tmp7D3[2];s=((_tmp7D3[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp28C),((_tmp7D3[0]=Cyc_PP_text(((_tmp7D4="throw ",_tag_dyneither(_tmp7D4,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp7D3,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1AA;_LL1C1: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp28D=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp28D->tag != 11)goto _LL1C3;else{_tmp28E=_tmp28D->f1;}}_LL1C2:
# 1138
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp28E);
goto _LL1AA;_LL1C3: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp28F=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp28F->tag != 12)goto _LL1C5;else{_tmp290=_tmp28F->f1;}}_LL1C4:
# 1141
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp290);
goto _LL1AA;_LL1C5: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp291=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp291->tag != 13)goto _LL1C7;else{_tmp292=(void*)_tmp291->f1;_tmp293=_tmp291->f2;}}_LL1C6:
# 1144
{const char*_tmp7D9;const char*_tmp7D8;struct Cyc_PP_Doc*_tmp7D7[4];s=((_tmp7D7[3]=
# 1147
Cyc_Absynpp_exp2doc_prec(myprec,_tmp293),((_tmp7D7[2]=
# 1146
Cyc_PP_text(((_tmp7D8=")",_tag_dyneither(_tmp7D8,sizeof(char),2)))),((_tmp7D7[1]=
# 1145
Cyc_Absynpp_typ2doc(_tmp292),((_tmp7D7[0]=
# 1144
Cyc_PP_text(((_tmp7D9="(",_tag_dyneither(_tmp7D9,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7D7,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1148
goto _LL1AA;_LL1C7: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp294=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp294->tag != 14)goto _LL1C9;else{_tmp295=_tmp294->f1;}}_LL1C8:
# 1150
{const char*_tmp7DC;struct Cyc_PP_Doc*_tmp7DB[2];s=((_tmp7DB[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp295),((_tmp7DB[0]=
# 1150
Cyc_PP_text(((_tmp7DC="&",_tag_dyneither(_tmp7DC,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7DB,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1152
goto _LL1AA;_LL1C9: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp296=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp296->tag != 15)goto _LL1CB;else{_tmp297=_tmp296->f1;_tmp298=_tmp296->f2;}}_LL1CA:
# 1154
 if(_tmp297 == 0){
const char*_tmp7DF;struct Cyc_PP_Doc*_tmp7DE[2];s=((_tmp7DE[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp298),((_tmp7DE[0]=Cyc_PP_text(((_tmp7DF="new ",_tag_dyneither(_tmp7DF,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp7DE,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 1157
const char*_tmp7E4;const char*_tmp7E3;struct Cyc_PP_Doc*_tmp7E2[4];s=((_tmp7E2[3]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp298),((_tmp7E2[2]=
# 1157
Cyc_PP_text(((_tmp7E3=") ",_tag_dyneither(_tmp7E3,sizeof(char),3)))),((_tmp7E2[1]=Cyc_Absynpp_exp2doc(_tmp297),((_tmp7E2[0]=Cyc_PP_text(((_tmp7E4="rnew(",_tag_dyneither(_tmp7E4,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp7E2,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1159
goto _LL1AA;_LL1CB: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp299=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp299->tag != 16)goto _LL1CD;else{_tmp29A=(void*)_tmp299->f1;}}_LL1CC:
{const char*_tmp7E9;const char*_tmp7E8;struct Cyc_PP_Doc*_tmp7E7[3];s=((_tmp7E7[2]=Cyc_PP_text(((_tmp7E8=")",_tag_dyneither(_tmp7E8,sizeof(char),2)))),((_tmp7E7[1]=Cyc_Absynpp_typ2doc(_tmp29A),((_tmp7E7[0]=Cyc_PP_text(((_tmp7E9="sizeof(",_tag_dyneither(_tmp7E9,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7E7,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1AA;_LL1CD: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp29B=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp29B->tag != 17)goto _LL1CF;else{_tmp29C=_tmp29B->f1;}}_LL1CE:
{const char*_tmp7EE;const char*_tmp7ED;struct Cyc_PP_Doc*_tmp7EC[3];s=((_tmp7EC[2]=Cyc_PP_text(((_tmp7ED=")",_tag_dyneither(_tmp7ED,sizeof(char),2)))),((_tmp7EC[1]=Cyc_Absynpp_exp2doc(_tmp29C),((_tmp7EC[0]=Cyc_PP_text(((_tmp7EE="sizeof(",_tag_dyneither(_tmp7EE,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7EC,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1AA;_LL1CF: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp29D=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp29D->tag != 38)goto _LL1D1;else{_tmp29E=(void*)_tmp29D->f1;}}_LL1D0:
{const char*_tmp7F3;const char*_tmp7F2;struct Cyc_PP_Doc*_tmp7F1[3];s=((_tmp7F1[2]=Cyc_PP_text(((_tmp7F2=")",_tag_dyneither(_tmp7F2,sizeof(char),2)))),((_tmp7F1[1]=Cyc_Absynpp_typ2doc(_tmp29E),((_tmp7F1[0]=Cyc_PP_text(((_tmp7F3="valueof(",_tag_dyneither(_tmp7F3,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7F1,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1AA;_LL1D1: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp29F=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp29F->tag != 39)goto _LL1D3;else{_tmp2A0=_tmp29F->f1;_tmp2A1=_tmp29F->f2;}}_LL1D2:
# 1164
 if(_tmp2A0){
const char*_tmp7FA;const char*_tmp7F9;const char*_tmp7F8;struct Cyc_PP_Doc*_tmp7F7[4];s=((_tmp7F7[3]=Cyc_PP_text(((_tmp7F8=")",_tag_dyneither(_tmp7F8,sizeof(char),2)))),((_tmp7F7[2]=Cyc_PP_text(_tmp2A1),((_tmp7F7[1]=Cyc_PP_text(((_tmp7F9=" volatile (",_tag_dyneither(_tmp7F9,sizeof(char),12)))),((_tmp7F7[0]=Cyc_PP_text(((_tmp7FA="__asm__",_tag_dyneither(_tmp7FA,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7F7,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1167
const char*_tmp7FF;const char*_tmp7FE;struct Cyc_PP_Doc*_tmp7FD[3];s=((_tmp7FD[2]=Cyc_PP_text(((_tmp7FE=")",_tag_dyneither(_tmp7FE,sizeof(char),2)))),((_tmp7FD[1]=Cyc_PP_text(_tmp2A1),((_tmp7FD[0]=Cyc_PP_text(((_tmp7FF="__asm__(",_tag_dyneither(_tmp7FF,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7FD,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1AA;_LL1D3: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp2A2=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp2A2->tag != 37)goto _LL1D5;else{_tmp2A3=_tmp2A2->f1;_tmp2A4=_tmp2A2->f2;}}_LL1D4:
# 1170
{const char*_tmp806;const char*_tmp805;const char*_tmp804;struct Cyc_PP_Doc*_tmp803[5];s=((_tmp803[4]=Cyc_PP_text(((_tmp804=")",_tag_dyneither(_tmp804,sizeof(char),2)))),((_tmp803[3]=Cyc_PP_textptr(_tmp2A4),((_tmp803[2]=Cyc_PP_text(((_tmp805=".",_tag_dyneither(_tmp805,sizeof(char),2)))),((_tmp803[1]=Cyc_Absynpp_exp2doc(_tmp2A3),((_tmp803[0]=Cyc_PP_text(((_tmp806="tagcheck(",_tag_dyneither(_tmp806,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp803,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1AA;_LL1D5: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp2A5=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp2A5->tag != 18)goto _LL1D7;else{_tmp2A6=(void*)_tmp2A5->f1;_tmp2A7=_tmp2A5->f2;}}_LL1D6:
# 1173
{const char*_tmp80B;const char*_tmp80A;struct Cyc_PP_Doc*_tmp809[3];s=((_tmp809[2]=Cyc_PP_text(((_tmp80A=",",_tag_dyneither(_tmp80A,sizeof(char),2)))),((_tmp809[1]=Cyc_Absynpp_typ2doc(_tmp2A6),((_tmp809[0]=Cyc_PP_text(((_tmp80B="offsetof(",_tag_dyneither(_tmp80B,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp809,sizeof(struct Cyc_PP_Doc*),3))))))));}
for(0;_tmp2A7 != 0;_tmp2A7=_tmp2A7->tl){
void*_tmp319=(void*)_tmp2A7->hd;void*_tmp31A=_tmp319;struct _dyneither_ptr*_tmp31C;unsigned int _tmp31E;_LL201: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp31B=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp31A;if(_tmp31B->tag != 0)goto _LL203;else{_tmp31C=_tmp31B->f1;}}_LL202:
{const char*_tmp80E;struct Cyc_PP_Doc*_tmp80D[3];s=((_tmp80D[2]=
_tmp2A7->tl != 0?Cyc_PP_text(((_tmp80E=".",_tag_dyneither(_tmp80E,sizeof(char),2)))): Cyc_PP_nil_doc(),((_tmp80D[1]=
# 1176
Cyc_PP_textptr(_tmp31C),((_tmp80D[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp80D,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1178
goto _LL200;_LL203: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp31D=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp31A;if(_tmp31D->tag != 1)goto _LL200;else{_tmp31E=_tmp31D->f1;}}_LL204:
{struct Cyc_Int_pa_PrintArg_struct _tmp818;void*_tmp817[1];const char*_tmp816;const char*_tmp815;struct Cyc_PP_Doc*_tmp814[3];s=((_tmp814[2]=
_tmp2A7->tl != 0?Cyc_PP_text(((_tmp815=".",_tag_dyneither(_tmp815,sizeof(char),2)))): Cyc_PP_nil_doc(),((_tmp814[1]=
# 1179
Cyc_PP_text((struct _dyneither_ptr)((_tmp818.tag=1,((_tmp818.f1=(unsigned long)((int)_tmp31E),((_tmp817[0]=& _tmp818,Cyc_aprintf(((_tmp816="%d",_tag_dyneither(_tmp816,sizeof(char),3))),_tag_dyneither(_tmp817,sizeof(void*),1))))))))),((_tmp814[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp814,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1181
goto _LL200;_LL200:;}
# 1183
{const char*_tmp81B;struct Cyc_PP_Doc*_tmp81A[2];s=((_tmp81A[1]=Cyc_PP_text(((_tmp81B=")",_tag_dyneither(_tmp81B,sizeof(char),2)))),((_tmp81A[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp81A,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1AA;_LL1D7: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp2A8=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp2A8->tag != 19)goto _LL1D9;else{_tmp2A9=_tmp2A8->f1;}}_LL1D8:
# 1186
{const char*_tmp81E;struct Cyc_PP_Doc*_tmp81D[2];s=((_tmp81D[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A9),((_tmp81D[0]=Cyc_PP_text(((_tmp81E="*",_tag_dyneither(_tmp81E,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp81D,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1AA;_LL1D9: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp2AA=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp2AA->tag != 20)goto _LL1DB;else{_tmp2AB=_tmp2AA->f1;_tmp2AC=_tmp2AA->f2;}}_LL1DA:
# 1189
{const char*_tmp821;struct Cyc_PP_Doc*_tmp820[3];s=((_tmp820[2]=Cyc_PP_textptr(_tmp2AC),((_tmp820[1]=Cyc_PP_text(((_tmp821=".",_tag_dyneither(_tmp821,sizeof(char),2)))),((_tmp820[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AB),Cyc_PP_cat(_tag_dyneither(_tmp820,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1AA;_LL1DB: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2AD=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp2AD->tag != 21)goto _LL1DD;else{_tmp2AE=_tmp2AD->f1;_tmp2AF=_tmp2AD->f2;}}_LL1DC:
# 1192
{const char*_tmp824;struct Cyc_PP_Doc*_tmp823[3];s=((_tmp823[2]=Cyc_PP_textptr(_tmp2AF),((_tmp823[1]=Cyc_PP_text(((_tmp824="->",_tag_dyneither(_tmp824,sizeof(char),3)))),((_tmp823[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AE),Cyc_PP_cat(_tag_dyneither(_tmp823,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1AA;_LL1DD: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp2B0=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp2B0->tag != 22)goto _LL1DF;else{_tmp2B1=_tmp2B0->f1;_tmp2B2=_tmp2B0->f2;}}_LL1DE:
# 1195
{const char*_tmp829;const char*_tmp828;struct Cyc_PP_Doc*_tmp827[4];s=((_tmp827[3]=
# 1198
Cyc_PP_text(((_tmp828="]",_tag_dyneither(_tmp828,sizeof(char),2)))),((_tmp827[2]=
# 1197
Cyc_Absynpp_exp2doc(_tmp2B2),((_tmp827[1]=
# 1196
Cyc_PP_text(((_tmp829="[",_tag_dyneither(_tmp829,sizeof(char),2)))),((_tmp827[0]=
# 1195
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B1),Cyc_PP_cat(_tag_dyneither(_tmp827,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1199
goto _LL1AA;_LL1DF: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp2B3=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp2B3->tag != 23)goto _LL1E1;else{_tmp2B4=_tmp2B3->f1;}}_LL1E0:
# 1201
{const char*_tmp82E;const char*_tmp82D;struct Cyc_PP_Doc*_tmp82C[4];s=((_tmp82C[3]=
# 1204
Cyc_PP_text(((_tmp82D=")",_tag_dyneither(_tmp82D,sizeof(char),2)))),((_tmp82C[2]=
# 1203
Cyc_Absynpp_exps2doc_prec(20,_tmp2B4),((_tmp82C[1]=
# 1202
Cyc_PP_text(((_tmp82E="(",_tag_dyneither(_tmp82E,sizeof(char),2)))),((_tmp82C[0]=
# 1201
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp82C,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1205
goto _LL1AA;_LL1E1: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp2B5=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp2B5->tag != 24)goto _LL1E3;else{_tmp2B6=_tmp2B5->f1;_tmp2B7=_tmp2B5->f2;}}_LL1E2:
# 1207
{const char*_tmp835;const char*_tmp834;const char*_tmp833;struct Cyc_PP_Doc*_tmp832[4];s=((_tmp832[3]=
# 1210
Cyc_Absynpp_group_braces(((_tmp833=",",_tag_dyneither(_tmp833,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2B7)),((_tmp832[2]=
# 1209
Cyc_PP_text(((_tmp834=")",_tag_dyneither(_tmp834,sizeof(char),2)))),((_tmp832[1]=
# 1208
Cyc_Absynpp_typ2doc((*_tmp2B6).f3),((_tmp832[0]=
# 1207
Cyc_PP_text(((_tmp835="(",_tag_dyneither(_tmp835,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp832,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1211
goto _LL1AA;_LL1E3: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp2B8=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp2B8->tag != 25)goto _LL1E5;else{_tmp2B9=_tmp2B8->f1;}}_LL1E4:
# 1213
{const char*_tmp836;s=Cyc_Absynpp_group_braces(((_tmp836=",",_tag_dyneither(_tmp836,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2B9));}
goto _LL1AA;_LL1E5: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp2BA=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp2BA->tag != 26)goto _LL1E7;else{_tmp2BB=_tmp2BA->f1;_tmp2BC=_tmp2BA->f2;_tmp2BD=_tmp2BA->f3;}}_LL1E6:
# 1216
{const char*_tmp83D;const char*_tmp83C;const char*_tmp83B;struct Cyc_PP_Doc*_tmp83A[8];s=((_tmp83A[7]=
# 1222
Cyc_Absynpp_rb(),((_tmp83A[6]=
# 1221
Cyc_Absynpp_exp2doc(_tmp2BD),((_tmp83A[5]=
# 1220
Cyc_PP_text(((_tmp83B=" : ",_tag_dyneither(_tmp83B,sizeof(char),4)))),((_tmp83A[4]=
# 1219
Cyc_Absynpp_exp2doc(_tmp2BC),((_tmp83A[3]=
# 1218
Cyc_PP_text(((_tmp83C=" < ",_tag_dyneither(_tmp83C,sizeof(char),4)))),((_tmp83A[2]=
# 1217
Cyc_PP_text(*(*_tmp2BB->name).f2),((_tmp83A[1]=
# 1216
Cyc_PP_text(((_tmp83D="for ",_tag_dyneither(_tmp83D,sizeof(char),5)))),((_tmp83A[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp83A,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1223
goto _LL1AA;_LL1E7: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp2BE=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp2BE->tag != 27)goto _LL1E9;else{_tmp2BF=_tmp2BE->f1;_tmp2C0=(void*)_tmp2BE->f2;}}_LL1E8:
# 1225
{const char*_tmp844;const char*_tmp843;const char*_tmp842;struct Cyc_PP_Doc*_tmp841[7];s=((_tmp841[6]=
# 1230
Cyc_Absynpp_rb(),((_tmp841[5]=
# 1229
Cyc_Absynpp_typ2doc(_tmp2C0),((_tmp841[4]=
# 1228
Cyc_PP_text(((_tmp842=" : ",_tag_dyneither(_tmp842,sizeof(char),4)))),((_tmp841[3]=
# 1227
Cyc_Absynpp_exp2doc(_tmp2BF),((_tmp841[2]=
# 1226
Cyc_PP_text(((_tmp843=" < ",_tag_dyneither(_tmp843,sizeof(char),4)))),((_tmp841[1]=
# 1225
Cyc_PP_text(((_tmp844="for x ",_tag_dyneither(_tmp844,sizeof(char),7)))),((_tmp841[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp841,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1231
goto _LL1AA;_LL1E9: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp2C1=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp2C1->tag != 28)goto _LL1EB;else{_tmp2C2=_tmp2C1->f1;_tmp2C3=_tmp2C1->f2;_tmp2C4=_tmp2C1->f3;}}_LL1EA: {
# 1233
struct Cyc_List_List*_tmp341=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C4);
{const char*_tmp849;struct Cyc_List_List*_tmp848;struct Cyc_PP_Doc*_tmp847[2];s=((_tmp847[1]=
Cyc_Absynpp_group_braces(((_tmp849=",",_tag_dyneither(_tmp849,sizeof(char),2))),
_tmp2C3 != 0?(_tmp848=_cycalloc(sizeof(*_tmp848)),((_tmp848->hd=Cyc_Absynpp_tps2doc(_tmp2C3),((_tmp848->tl=_tmp341,_tmp848))))): _tmp341),((_tmp847[0]=
# 1234
Cyc_Absynpp_qvar2doc(_tmp2C2),Cyc_PP_cat(_tag_dyneither(_tmp847,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1237
goto _LL1AA;}_LL1EB: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp2C5=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp2C5->tag != 29)goto _LL1ED;else{_tmp2C6=_tmp2C5->f2;}}_LL1EC:
# 1239
{const char*_tmp84A;s=Cyc_Absynpp_group_braces(((_tmp84A=",",_tag_dyneither(_tmp84A,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C6));}
goto _LL1AA;_LL1ED: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp2C7=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp2C7->tag != 30)goto _LL1EF;else{_tmp2C8=_tmp2C7->f1;_tmp2C9=_tmp2C7->f3;}}_LL1EE:
# 1242
 if(_tmp2C8 == 0)
# 1244
s=Cyc_Absynpp_qvar2doc(_tmp2C9->name);else{
# 1246
const char*_tmp851;const char*_tmp850;const char*_tmp84F;struct Cyc_PP_Doc*_tmp84E[2];s=((_tmp84E[1]=
Cyc_PP_egroup(((_tmp851="(",_tag_dyneither(_tmp851,sizeof(char),2))),((_tmp850=")",_tag_dyneither(_tmp850,sizeof(char),2))),((_tmp84F=",",_tag_dyneither(_tmp84F,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp2C8)),((_tmp84E[0]=
# 1246
Cyc_Absynpp_qvar2doc(_tmp2C9->name),Cyc_PP_cat(_tag_dyneither(_tmp84E,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1248
goto _LL1AA;_LL1EF: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp2CA=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp2CA->tag != 31)goto _LL1F1;else{_tmp2CB=_tmp2CA->f1;}}_LL1F0:
# 1250
 s=Cyc_Absynpp_qvar2doc(_tmp2CB);
goto _LL1AA;_LL1F1: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp2CC=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp2CC->tag != 32)goto _LL1F3;else{_tmp2CD=_tmp2CC->f1;}}_LL1F2:
# 1253
 s=Cyc_Absynpp_qvar2doc(_tmp2CD);
goto _LL1AA;_LL1F3: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2CE=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp2CE->tag != 33)goto _LL1F5;else{_tmp2CF=(_tmp2CE->f1).is_calloc;_tmp2D0=(_tmp2CE->f1).rgn;_tmp2D1=(_tmp2CE->f1).elt_type;_tmp2D2=(_tmp2CE->f1).num_elts;_tmp2D3=(_tmp2CE->f1).inline_call;}}_LL1F4:
# 1258
 if(_tmp2CF){
# 1260
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp2D1)),0);
if(_tmp2D0 == 0){
const char*_tmp858;const char*_tmp857;const char*_tmp856;struct Cyc_PP_Doc*_tmp855[5];s=((_tmp855[4]=Cyc_PP_text(((_tmp856=")",_tag_dyneither(_tmp856,sizeof(char),2)))),((_tmp855[3]=Cyc_Absynpp_exp2doc(st),((_tmp855[2]=Cyc_PP_text(((_tmp857=",",_tag_dyneither(_tmp857,sizeof(char),2)))),((_tmp855[1]=Cyc_Absynpp_exp2doc(_tmp2D2),((_tmp855[0]=Cyc_PP_text(((_tmp858="calloc(",_tag_dyneither(_tmp858,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp855,sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{
# 1264
const char*_tmp861;const char*_tmp860;const char*_tmp85F;const char*_tmp85E;struct Cyc_PP_Doc*_tmp85D[7];s=((_tmp85D[6]=
Cyc_PP_text(((_tmp85E=")",_tag_dyneither(_tmp85E,sizeof(char),2)))),((_tmp85D[5]=Cyc_Absynpp_exp2doc(st),((_tmp85D[4]=Cyc_PP_text(((_tmp85F=",",_tag_dyneither(_tmp85F,sizeof(char),2)))),((_tmp85D[3]=Cyc_Absynpp_exp2doc(_tmp2D2),((_tmp85D[2]=
# 1264
Cyc_PP_text(((_tmp860=",",_tag_dyneither(_tmp860,sizeof(char),2)))),((_tmp85D[1]=Cyc_Absynpp_exp2doc(_tmp2D0),((_tmp85D[0]=Cyc_PP_text(((_tmp861="rcalloc(",_tag_dyneither(_tmp861,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp85D,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1267
struct Cyc_Absyn_Exp*new_e;
# 1269
if(_tmp2D1 == 0)
new_e=_tmp2D2;else{
# 1272
new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp2D1,0),_tmp2D2,0);}
# 1274
if(_tmp2D0 == 0){
const char*_tmp866;const char*_tmp865;struct Cyc_PP_Doc*_tmp864[3];s=((_tmp864[2]=Cyc_PP_text(((_tmp865=")",_tag_dyneither(_tmp865,sizeof(char),2)))),((_tmp864[1]=Cyc_Absynpp_exp2doc(new_e),((_tmp864[0]=Cyc_PP_text(((_tmp866="malloc(",_tag_dyneither(_tmp866,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp864,sizeof(struct Cyc_PP_Doc*),3))))))));}else{
# 1277
if(_tmp2D3){
const char*_tmp86D;const char*_tmp86C;const char*_tmp86B;struct Cyc_PP_Doc*_tmp86A[5];s=((_tmp86A[4]=
Cyc_PP_text(((_tmp86B=")",_tag_dyneither(_tmp86B,sizeof(char),2)))),((_tmp86A[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp86A[2]=
# 1278
Cyc_PP_text(((_tmp86C=",",_tag_dyneither(_tmp86C,sizeof(char),2)))),((_tmp86A[1]=Cyc_Absynpp_exp2doc(_tmp2D0),((_tmp86A[0]=Cyc_PP_text(((_tmp86D="rmalloc_inline(",_tag_dyneither(_tmp86D,sizeof(char),16)))),Cyc_PP_cat(_tag_dyneither(_tmp86A,sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{
# 1281
const char*_tmp874;const char*_tmp873;const char*_tmp872;struct Cyc_PP_Doc*_tmp871[5];s=((_tmp871[4]=
Cyc_PP_text(((_tmp872=")",_tag_dyneither(_tmp872,sizeof(char),2)))),((_tmp871[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp871[2]=
# 1281
Cyc_PP_text(((_tmp873=",",_tag_dyneither(_tmp873,sizeof(char),2)))),((_tmp871[1]=Cyc_Absynpp_exp2doc(_tmp2D0),((_tmp871[0]=Cyc_PP_text(((_tmp874="rmalloc(",_tag_dyneither(_tmp874,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp871,sizeof(struct Cyc_PP_Doc*),5))))))))))));}}}
# 1285
goto _LL1AA;_LL1F5: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp2D4=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp2D4->tag != 34)goto _LL1F7;else{_tmp2D5=_tmp2D4->f1;_tmp2D6=_tmp2D4->f2;}}_LL1F6:
# 1287
{const char*_tmp877;struct Cyc_PP_Doc*_tmp876[3];s=((_tmp876[2]=
# 1289
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D6),((_tmp876[1]=
# 1288
Cyc_PP_text(((_tmp877=" :=: ",_tag_dyneither(_tmp877,sizeof(char),6)))),((_tmp876[0]=
# 1287
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D5),Cyc_PP_cat(_tag_dyneither(_tmp876,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1290
goto _LL1AA;_LL1F7: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp2D7=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp2D7->tag != 35)goto _LL1F9;else{_tmp2D8=_tmp2D7->f1;_tmp2D9=_tmp2D7->f2;}}_LL1F8:
# 1293
{const char*_tmp878;s=Cyc_Absynpp_group_braces(((_tmp878=",",_tag_dyneither(_tmp878,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2D9));}
goto _LL1AA;_LL1F9: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp2DA=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp2DA->tag != 36)goto _LL1AA;else{_tmp2DB=_tmp2DA->f1;}}_LL1FA:
# 1296
{const char*_tmp87D;const char*_tmp87C;struct Cyc_PP_Doc*_tmp87B[7];s=((_tmp87B[6]=
# 1298
Cyc_PP_text(((_tmp87C=")",_tag_dyneither(_tmp87C,sizeof(char),2)))),((_tmp87B[5]=Cyc_Absynpp_rb(),((_tmp87B[4]=Cyc_PP_blank_doc(),((_tmp87B[3]=
# 1297
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2DB,1)),((_tmp87B[2]=
# 1296
Cyc_PP_blank_doc(),((_tmp87B[1]=Cyc_Absynpp_lb(),((_tmp87B[0]=Cyc_PP_text(((_tmp87D="(",_tag_dyneither(_tmp87D,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp87B,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1299
goto _LL1AA;_LL1AA:;}
# 1301
if(inprec >= myprec){
const char*_tmp882;const char*_tmp881;struct Cyc_PP_Doc*_tmp880[3];s=((_tmp880[2]=Cyc_PP_text(((_tmp881=")",_tag_dyneither(_tmp881,sizeof(char),2)))),((_tmp880[1]=s,((_tmp880[0]=Cyc_PP_text(((_tmp882="(",_tag_dyneither(_tmp882,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp880,sizeof(struct Cyc_PP_Doc*),3))))))));}
return s;}
# 1306
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp367=d;struct Cyc_Absyn_Exp*_tmp369;struct _dyneither_ptr*_tmp36B;_LL206: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp368=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp367;if(_tmp368->tag != 0)goto _LL208;else{_tmp369=_tmp368->f1;}}_LL207: {
const char*_tmp887;const char*_tmp886;struct Cyc_PP_Doc*_tmp885[3];return(_tmp885[2]=Cyc_PP_text(((_tmp886="]",_tag_dyneither(_tmp886,sizeof(char),2)))),((_tmp885[1]=Cyc_Absynpp_exp2doc(_tmp369),((_tmp885[0]=Cyc_PP_text(((_tmp887=".[",_tag_dyneither(_tmp887,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp885,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL208: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp36A=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp367;if(_tmp36A->tag != 1)goto _LL205;else{_tmp36B=_tmp36A->f1;}}_LL209: {
const char*_tmp88A;struct Cyc_PP_Doc*_tmp889[2];return(_tmp889[1]=Cyc_PP_textptr(_tmp36B),((_tmp889[0]=Cyc_PP_text(((_tmp88A=".",_tag_dyneither(_tmp88A,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp889,sizeof(struct Cyc_PP_Doc*),2)))));}_LL205:;}
# 1313
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
const char*_tmp891;const char*_tmp890;const char*_tmp88F;struct Cyc_PP_Doc*_tmp88E[2];return(_tmp88E[1]=
Cyc_Absynpp_exp2doc((*de).f2),((_tmp88E[0]=
# 1315
Cyc_PP_egroup(((_tmp891="",_tag_dyneither(_tmp891,sizeof(char),1))),((_tmp890="=",_tag_dyneither(_tmp890,sizeof(char),2))),((_tmp88F="=",_tag_dyneither(_tmp88F,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp88E,sizeof(struct Cyc_PP_Doc*),2)))));}}
# 1319
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
const char*_tmp894;const char*_tmp893;const char*_tmp892;return Cyc_PP_group(((_tmp892="",_tag_dyneither(_tmp892,sizeof(char),1))),((_tmp893="",_tag_dyneither(_tmp893,sizeof(char),1))),((_tmp894=",",_tag_dyneither(_tmp894,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long i);static void _tmp899(unsigned int*_tmp898,unsigned int*_tmp897,char**_tmp896){for(*_tmp898=0;*_tmp898 < *_tmp897;(*_tmp898)++){(*_tmp896)[*_tmp898]=(char)'z';}}
# 1323
struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
char*_tmp89E;unsigned int _tmp89D;char*_tmp89C;unsigned int _tmp89B;unsigned int _tmp89A;struct _dyneither_ptr x=(_tmp89E=((_tmp89D=(unsigned int)28,((_tmp89C=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp89D + 1)),((((_tmp89B=_tmp89D,((_tmp899(& _tmp89A,& _tmp89B,& _tmp89C),_tmp89C[_tmp89B]=(char)0)))),_tmp89C)))))),_tag_dyneither(_tmp89E,sizeof(char),_get_zero_arr_size_char((void*)_tmp89E,(unsigned int)28 + 1)));
{char _tmp8A1;char _tmp8A0;struct _dyneither_ptr _tmp89F;(_tmp89F=_dyneither_ptr_plus(x,sizeof(char),27),((_tmp8A0=*((char*)_check_dyneither_subscript(_tmp89F,sizeof(char),0)),((_tmp8A1='\000',((_get_dyneither_size(_tmp89F,sizeof(char))== 1  && (_tmp8A0 == '\000'  && _tmp8A1 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp89F.curr)=_tmp8A1)))))));}
{char _tmp8A4;char _tmp8A3;struct _dyneither_ptr _tmp8A2;(_tmp8A2=_dyneither_ptr_plus(x,sizeof(char),26),((_tmp8A3=*((char*)_check_dyneither_subscript(_tmp8A2,sizeof(char),0)),((_tmp8A4='L',((_get_dyneither_size(_tmp8A2,sizeof(char))== 1  && (_tmp8A3 == '\000'  && _tmp8A4 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp8A2.curr)=_tmp8A4)))))));}
{char _tmp8A7;char _tmp8A6;struct _dyneither_ptr _tmp8A5;(_tmp8A5=_dyneither_ptr_plus(x,sizeof(char),25),((_tmp8A6=*((char*)_check_dyneither_subscript(_tmp8A5,sizeof(char),0)),((_tmp8A7='L',((_get_dyneither_size(_tmp8A5,sizeof(char))== 1  && (_tmp8A6 == '\000'  && _tmp8A7 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp8A5.curr)=_tmp8A7)))))));}
{char _tmp8AA;char _tmp8A9;struct _dyneither_ptr _tmp8A8;(_tmp8A8=_dyneither_ptr_plus(x,sizeof(char),24),((_tmp8A9=*((char*)_check_dyneither_subscript(_tmp8A8,sizeof(char),0)),((_tmp8AA='U',((_get_dyneither_size(_tmp8A8,sizeof(char))== 1  && (_tmp8A9 == '\000'  && _tmp8AA != '\000')?_throw_arraybounds(): 1,*((char*)_tmp8A8.curr)=_tmp8AA)))))));}
{char _tmp8AD;char _tmp8AC;struct _dyneither_ptr _tmp8AB;(_tmp8AB=_dyneither_ptr_plus(x,sizeof(char),23),((_tmp8AC=*((char*)_check_dyneither_subscript(_tmp8AB,sizeof(char),0)),((_tmp8AD='0',((_get_dyneither_size(_tmp8AB,sizeof(char))== 1  && (_tmp8AC == '\000'  && _tmp8AD != '\000')?_throw_arraybounds(): 1,*((char*)_tmp8AB.curr)=_tmp8AD)))))));}{
int index=23;
while(i != 0){
char c=(char)('0' + i % 10);
{char _tmp8B0;char _tmp8AF;struct _dyneither_ptr _tmp8AE;(_tmp8AE=_dyneither_ptr_plus(x,sizeof(char),index),((_tmp8AF=*((char*)_check_dyneither_subscript(_tmp8AE,sizeof(char),0)),((_tmp8B0=c,((_get_dyneither_size(_tmp8AE,sizeof(char))== 1  && (_tmp8AF == '\000'  && _tmp8B0 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp8AE.curr)=_tmp8B0)))))));}
i=i / 10;
-- index;}
# 1337
return(struct _dyneither_ptr)_dyneither_ptr_plus(_dyneither_ptr_plus(x,sizeof(char),index),sizeof(char),1);};}
# 1341
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp38E=c;enum Cyc_Absyn_Sign _tmp38F;char _tmp390;struct _dyneither_ptr _tmp391;enum Cyc_Absyn_Sign _tmp392;short _tmp393;int _tmp394;int _tmp395;int _tmp396;enum Cyc_Absyn_Sign _tmp397;long long _tmp398;struct _dyneither_ptr _tmp399;struct _dyneither_ptr _tmp39A;struct _dyneither_ptr _tmp39B;_LL20B: if((_tmp38E.Char_c).tag != 2)goto _LL20D;_tmp38F=((struct _tuple3)(_tmp38E.Char_c).val).f1;_tmp390=((struct _tuple3)(_tmp38E.Char_c).val).f2;_LL20C: {
const char*_tmp8B4;void*_tmp8B3[1];struct Cyc_String_pa_PrintArg_struct _tmp8B2;return Cyc_PP_text((struct _dyneither_ptr)((_tmp8B2.tag=0,((_tmp8B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp390)),((_tmp8B3[0]=& _tmp8B2,Cyc_aprintf(((_tmp8B4="'%s'",_tag_dyneither(_tmp8B4,sizeof(char),5))),_tag_dyneither(_tmp8B3,sizeof(void*),1)))))))));}_LL20D: if((_tmp38E.Wchar_c).tag != 3)goto _LL20F;_tmp391=(struct _dyneither_ptr)(_tmp38E.Wchar_c).val;_LL20E: {
const char*_tmp8B8;void*_tmp8B7[1];struct Cyc_String_pa_PrintArg_struct _tmp8B6;return Cyc_PP_text((struct _dyneither_ptr)((_tmp8B6.tag=0,((_tmp8B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp391),((_tmp8B7[0]=& _tmp8B6,Cyc_aprintf(((_tmp8B8="L'%s'",_tag_dyneither(_tmp8B8,sizeof(char),6))),_tag_dyneither(_tmp8B7,sizeof(void*),1)))))))));}_LL20F: if((_tmp38E.Short_c).tag != 4)goto _LL211;_tmp392=((struct _tuple4)(_tmp38E.Short_c).val).f1;_tmp393=((struct _tuple4)(_tmp38E.Short_c).val).f2;_LL210: {
const char*_tmp8BC;void*_tmp8BB[1];struct Cyc_Int_pa_PrintArg_struct _tmp8BA;return Cyc_PP_text((struct _dyneither_ptr)((_tmp8BA.tag=1,((_tmp8BA.f1=(unsigned long)((int)_tmp393),((_tmp8BB[0]=& _tmp8BA,Cyc_aprintf(((_tmp8BC="%d",_tag_dyneither(_tmp8BC,sizeof(char),3))),_tag_dyneither(_tmp8BB,sizeof(void*),1)))))))));}_LL211: if((_tmp38E.Int_c).tag != 5)goto _LL213;if(((struct _tuple5)(_tmp38E.Int_c).val).f1 != Cyc_Absyn_None)goto _LL213;_tmp394=((struct _tuple5)(_tmp38E.Int_c).val).f2;_LL212:
 _tmp395=_tmp394;goto _LL214;_LL213: if((_tmp38E.Int_c).tag != 5)goto _LL215;if(((struct _tuple5)(_tmp38E.Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL215;_tmp395=((struct _tuple5)(_tmp38E.Int_c).val).f2;_LL214: {
const char*_tmp8C0;void*_tmp8BF[1];struct Cyc_Int_pa_PrintArg_struct _tmp8BE;return Cyc_PP_text((struct _dyneither_ptr)((_tmp8BE.tag=1,((_tmp8BE.f1=(unsigned long)_tmp395,((_tmp8BF[0]=& _tmp8BE,Cyc_aprintf(((_tmp8C0="%d",_tag_dyneither(_tmp8C0,sizeof(char),3))),_tag_dyneither(_tmp8BF,sizeof(void*),1)))))))));}_LL215: if((_tmp38E.Int_c).tag != 5)goto _LL217;if(((struct _tuple5)(_tmp38E.Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL217;_tmp396=((struct _tuple5)(_tmp38E.Int_c).val).f2;_LL216: {
const char*_tmp8C4;void*_tmp8C3[1];struct Cyc_Int_pa_PrintArg_struct _tmp8C2;return Cyc_PP_text((struct _dyneither_ptr)((_tmp8C2.tag=1,((_tmp8C2.f1=(unsigned int)_tmp396,((_tmp8C3[0]=& _tmp8C2,Cyc_aprintf(((_tmp8C4="%u",_tag_dyneither(_tmp8C4,sizeof(char),3))),_tag_dyneither(_tmp8C3,sizeof(void*),1)))))))));}_LL217: if((_tmp38E.LongLong_c).tag != 6)goto _LL219;_tmp397=((struct _tuple6)(_tmp38E.LongLong_c).val).f1;_tmp398=((struct _tuple6)(_tmp38E.LongLong_c).val).f2;_LL218:
# 1351
 return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)_tmp398));_LL219: if((_tmp38E.Float_c).tag != 7)goto _LL21B;_tmp399=((struct _tuple7)(_tmp38E.Float_c).val).f1;_LL21A:
 return Cyc_PP_text(_tmp399);_LL21B: if((_tmp38E.Null_c).tag != 1)goto _LL21D;_LL21C: {
const char*_tmp8C5;return Cyc_PP_text(((_tmp8C5="NULL",_tag_dyneither(_tmp8C5,sizeof(char),5))));}_LL21D: if((_tmp38E.String_c).tag != 8)goto _LL21F;_tmp39A=(struct _dyneither_ptr)(_tmp38E.String_c).val;_LL21E: {
# 1355
const char*_tmp8CA;const char*_tmp8C9;struct Cyc_PP_Doc*_tmp8C8[3];return(_tmp8C8[2]=Cyc_PP_text(((_tmp8C9="\"",_tag_dyneither(_tmp8C9,sizeof(char),2)))),((_tmp8C8[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp39A)),((_tmp8C8[0]=Cyc_PP_text(((_tmp8CA="\"",_tag_dyneither(_tmp8CA,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8C8,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL21F: if((_tmp38E.Wstring_c).tag != 9)goto _LL20A;_tmp39B=(struct _dyneither_ptr)(_tmp38E.Wstring_c).val;_LL220: {
# 1357
const char*_tmp8CF;const char*_tmp8CE;struct Cyc_PP_Doc*_tmp8CD[3];return(_tmp8CD[2]=Cyc_PP_text(((_tmp8CE="\"",_tag_dyneither(_tmp8CE,sizeof(char),2)))),((_tmp8CD[1]=Cyc_PP_text(_tmp39B),((_tmp8CD[0]=Cyc_PP_text(((_tmp8CF="L\"",_tag_dyneither(_tmp8CF,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp8CD,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL20A:;}
# 1361
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if(p == Cyc_Absyn_Numelts){
if(es == 0  || es->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp8DC;const char*_tmp8DB;void*_tmp8DA[1];struct Cyc_String_pa_PrintArg_struct _tmp8D9;struct Cyc_Core_Failure_exn_struct*_tmp8D8;(int)_throw((void*)((_tmp8D8=_cycalloc(sizeof(*_tmp8D8)),((_tmp8D8[0]=((_tmp8DC.tag=Cyc_Core_Failure,((_tmp8DC.f1=(struct _dyneither_ptr)((_tmp8D9.tag=0,((_tmp8D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp8DA[0]=& _tmp8D9,Cyc_aprintf(((_tmp8DB="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp8DB,sizeof(char),47))),_tag_dyneither(_tmp8DA,sizeof(void*),1)))))))),_tmp8DC)))),_tmp8D8)))));}{
# 1367
const char*_tmp8E1;const char*_tmp8E0;struct Cyc_PP_Doc*_tmp8DF[3];return(_tmp8DF[2]=Cyc_PP_text(((_tmp8E0=")",_tag_dyneither(_tmp8E0,sizeof(char),2)))),((_tmp8DF[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp8DF[0]=Cyc_PP_text(((_tmp8E1="numelts(",_tag_dyneither(_tmp8E1,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8DF,sizeof(struct Cyc_PP_Doc*),3)))))));};}else{
# 1369
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0){
struct Cyc_Core_Failure_exn_struct _tmp8EE;const char*_tmp8ED;void*_tmp8EC[1];struct Cyc_String_pa_PrintArg_struct _tmp8EB;struct Cyc_Core_Failure_exn_struct*_tmp8EA;(int)_throw((void*)((_tmp8EA=_cycalloc(sizeof(*_tmp8EA)),((_tmp8EA[0]=((_tmp8EE.tag=Cyc_Core_Failure,((_tmp8EE.f1=(struct _dyneither_ptr)((_tmp8EB.tag=0,((_tmp8EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp8EC[0]=& _tmp8EB,Cyc_aprintf(((_tmp8ED="Absynpp::primapp2doc: %s with no args",_tag_dyneither(_tmp8ED,sizeof(char),38))),_tag_dyneither(_tmp8EC,sizeof(void*),1)))))))),_tmp8EE)))),_tmp8EA)))));}else{
# 1373
if(ds->tl == 0){
const char*_tmp8F1;struct Cyc_PP_Doc*_tmp8F0[3];return(_tmp8F0[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp8F0[1]=Cyc_PP_text(((_tmp8F1=" ",_tag_dyneither(_tmp8F1,sizeof(char),2)))),((_tmp8F0[0]=ps,Cyc_PP_cat(_tag_dyneither(_tmp8F0,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp8FE;const char*_tmp8FD;void*_tmp8FC[1];struct Cyc_String_pa_PrintArg_struct _tmp8FB;struct Cyc_Core_Failure_exn_struct*_tmp8FA;(int)_throw((void*)((_tmp8FA=_cycalloc(sizeof(*_tmp8FA)),((_tmp8FA[0]=((_tmp8FE.tag=Cyc_Core_Failure,((_tmp8FE.f1=(struct _dyneither_ptr)((_tmp8FB.tag=0,((_tmp8FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp8FC[0]=& _tmp8FB,Cyc_aprintf(((_tmp8FD="Absynpp::primapp2doc: %s with more than 2 args",_tag_dyneither(_tmp8FD,sizeof(char),47))),_tag_dyneither(_tmp8FC,sizeof(void*),1)))))))),_tmp8FE)))),_tmp8FA)))));}else{
# 1379
const char*_tmp903;const char*_tmp902;struct Cyc_PP_Doc*_tmp901[5];return(_tmp901[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd,((_tmp901[3]=Cyc_PP_text(((_tmp902=" ",_tag_dyneither(_tmp902,sizeof(char),2)))),((_tmp901[2]=ps,((_tmp901[1]=Cyc_PP_text(((_tmp903=" ",_tag_dyneither(_tmp903,sizeof(char),2)))),((_tmp901[0]=(struct Cyc_PP_Doc*)ds->hd,Cyc_PP_cat(_tag_dyneither(_tmp901,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
# 1383
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
switch(p){case Cyc_Absyn_Plus: _LL221: {
const char*_tmp904;return(_tmp904="+",_tag_dyneither(_tmp904,sizeof(char),2));}case Cyc_Absyn_Times: _LL222: {
const char*_tmp905;return(_tmp905="*",_tag_dyneither(_tmp905,sizeof(char),2));}case Cyc_Absyn_Minus: _LL223: {
const char*_tmp906;return(_tmp906="-",_tag_dyneither(_tmp906,sizeof(char),2));}case Cyc_Absyn_Div: _LL224: {
const char*_tmp907;return(_tmp907="/",_tag_dyneither(_tmp907,sizeof(char),2));}case Cyc_Absyn_Mod: _LL225: {
const char*_tmp909;const char*_tmp908;return Cyc_Absynpp_print_for_cycdoc?(_tmp909="\\%",_tag_dyneither(_tmp909,sizeof(char),3)):((_tmp908="%",_tag_dyneither(_tmp908,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL226: {
const char*_tmp90A;return(_tmp90A="==",_tag_dyneither(_tmp90A,sizeof(char),3));}case Cyc_Absyn_Neq: _LL227: {
const char*_tmp90B;return(_tmp90B="!=",_tag_dyneither(_tmp90B,sizeof(char),3));}case Cyc_Absyn_Gt: _LL228: {
const char*_tmp90C;return(_tmp90C=">",_tag_dyneither(_tmp90C,sizeof(char),2));}case Cyc_Absyn_Lt: _LL229: {
const char*_tmp90D;return(_tmp90D="<",_tag_dyneither(_tmp90D,sizeof(char),2));}case Cyc_Absyn_Gte: _LL22A: {
const char*_tmp90E;return(_tmp90E=">=",_tag_dyneither(_tmp90E,sizeof(char),3));}case Cyc_Absyn_Lte: _LL22B: {
const char*_tmp90F;return(_tmp90F="<=",_tag_dyneither(_tmp90F,sizeof(char),3));}case Cyc_Absyn_Not: _LL22C: {
const char*_tmp910;return(_tmp910="!",_tag_dyneither(_tmp910,sizeof(char),2));}case Cyc_Absyn_Bitnot: _LL22D: {
const char*_tmp911;return(_tmp911="~",_tag_dyneither(_tmp911,sizeof(char),2));}case Cyc_Absyn_Bitand: _LL22E: {
const char*_tmp912;return(_tmp912="&",_tag_dyneither(_tmp912,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL22F: {
const char*_tmp913;return(_tmp913="|",_tag_dyneither(_tmp913,sizeof(char),2));}case Cyc_Absyn_Bitxor: _LL230: {
const char*_tmp914;return(_tmp914="^",_tag_dyneither(_tmp914,sizeof(char),2));}case Cyc_Absyn_Bitlshift: _LL231: {
const char*_tmp915;return(_tmp915="<<",_tag_dyneither(_tmp915,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL232: {
const char*_tmp916;return(_tmp916=">>",_tag_dyneither(_tmp916,sizeof(char),3));}case Cyc_Absyn_Bitarshift: _LL233: {
const char*_tmp917;return(_tmp917=">>>",_tag_dyneither(_tmp917,sizeof(char),4));}case Cyc_Absyn_Numelts: _LL234: {
const char*_tmp918;return(_tmp918="numelts",_tag_dyneither(_tmp918,sizeof(char),8));}}}
# 1408
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1412
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp3DE=s->r;void*_tmp3DF=_tmp3DE;_LL237: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3E0=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3DF;if(_tmp3E0->tag != 12)goto _LL239;}_LL238:
 return 1;_LL239:;_LL23A:
 return 0;_LL236:;}
# 1419
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp){
struct Cyc_PP_Doc*s;
{void*_tmp3E1=st->r;void*_tmp3E2=_tmp3E1;struct Cyc_Absyn_Exp*_tmp3E5;struct Cyc_Absyn_Stmt*_tmp3E7;struct Cyc_Absyn_Stmt*_tmp3E8;struct Cyc_Absyn_Exp*_tmp3EA;struct Cyc_Absyn_Exp*_tmp3EC;struct Cyc_Absyn_Stmt*_tmp3ED;struct Cyc_Absyn_Stmt*_tmp3EE;struct Cyc_Absyn_Exp*_tmp3F0;struct Cyc_Absyn_Stmt*_tmp3F1;struct _dyneither_ptr*_tmp3F5;struct Cyc_Absyn_Exp*_tmp3F7;struct Cyc_Absyn_Exp*_tmp3F8;struct Cyc_Absyn_Exp*_tmp3F9;struct Cyc_Absyn_Stmt*_tmp3FA;struct Cyc_Absyn_Exp*_tmp3FC;struct Cyc_List_List*_tmp3FD;struct Cyc_List_List*_tmp400;struct Cyc_Absyn_Decl*_tmp402;struct Cyc_Absyn_Stmt*_tmp403;struct _dyneither_ptr*_tmp405;struct Cyc_Absyn_Stmt*_tmp406;struct Cyc_Absyn_Stmt*_tmp408;struct Cyc_Absyn_Exp*_tmp409;struct Cyc_Absyn_Stmt*_tmp40B;struct Cyc_List_List*_tmp40C;struct Cyc_Absyn_Exp*_tmp40E;_LL23C: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp3E3=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp3E3->tag != 0)goto _LL23E;}_LL23D:
{const char*_tmp919;s=Cyc_PP_text(((_tmp919=";",_tag_dyneither(_tmp919,sizeof(char),2))));}goto _LL23B;_LL23E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3E4=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp3E4->tag != 1)goto _LL240;else{_tmp3E5=_tmp3E4->f1;}}_LL23F:
{const char*_tmp91C;struct Cyc_PP_Doc*_tmp91B[2];s=((_tmp91B[1]=Cyc_PP_text(((_tmp91C=";",_tag_dyneither(_tmp91C,sizeof(char),2)))),((_tmp91B[0]=Cyc_Absynpp_exp2doc(_tmp3E5),Cyc_PP_cat(_tag_dyneither(_tmp91B,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL23B;_LL240: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3E6=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp3E6->tag != 2)goto _LL242;else{_tmp3E7=_tmp3E6->f1;_tmp3E8=_tmp3E6->f2;}}_LL241:
# 1425
 if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp3E7)){
struct Cyc_PP_Doc*_tmp927[5];struct Cyc_PP_Doc*_tmp926[7];const char*_tmp925;const char*_tmp924;struct Cyc_PP_Doc*_tmp923[7];s=((_tmp923[6]=
# 1433
Cyc_Absynpp_is_declaration(_tmp3E8)?
stmtexp?
(_tmp926[6]=
# 1438
Cyc_PP_line_doc(),((_tmp926[5]=
# 1437
Cyc_PP_text(((_tmp925=");",_tag_dyneither(_tmp925,sizeof(char),3)))),((_tmp926[4]=Cyc_Absynpp_rb(),((_tmp926[3]=
# 1436
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3E8,stmtexp)),((_tmp926[2]=
# 1435
Cyc_PP_blank_doc(),((_tmp926[1]=Cyc_Absynpp_lb(),((_tmp926[0]=Cyc_PP_text(((_tmp924="(",_tag_dyneither(_tmp924,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp926,sizeof(struct Cyc_PP_Doc*),7))))))))))))))):(
# 1439
(_tmp927[4]=
# 1442
Cyc_PP_line_doc(),((_tmp927[3]=
# 1441
Cyc_Absynpp_rb(),((_tmp927[2]=
# 1440
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3E8,stmtexp)),((_tmp927[1]=
# 1439
Cyc_PP_blank_doc(),((_tmp927[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp927,sizeof(struct Cyc_PP_Doc*),5)))))))))))):
# 1443
 Cyc_Absynpp_stmt2doc(_tmp3E8,stmtexp),((_tmp923[5]=
# 1432
Cyc_PP_line_doc(),((_tmp923[4]=
# 1431
Cyc_Absynpp_rb(),((_tmp923[3]=
# 1430
Cyc_PP_line_doc(),((_tmp923[2]=
# 1429
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3E7,0)),((_tmp923[1]=
# 1428
Cyc_PP_blank_doc(),((_tmp923[0]=
# 1427
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp923,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1444
if(Cyc_Absynpp_is_declaration(_tmp3E8)){
struct Cyc_PP_Doc*_tmp932[4];struct Cyc_PP_Doc*_tmp931[6];const char*_tmp930;const char*_tmp92F;struct Cyc_PP_Doc*_tmp92E[4];s=((_tmp92E[3]=
# 1455
Cyc_PP_line_doc(),((_tmp92E[2]=
# 1447
stmtexp?
(_tmp931[5]=
# 1450
Cyc_PP_text(((_tmp930=");",_tag_dyneither(_tmp930,sizeof(char),3)))),((_tmp931[4]=Cyc_Absynpp_rb(),((_tmp931[3]=
# 1449
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3E8,stmtexp)),((_tmp931[2]=
# 1448
Cyc_PP_blank_doc(),((_tmp931[1]=Cyc_Absynpp_lb(),((_tmp931[0]=Cyc_PP_text(((_tmp92F="(",_tag_dyneither(_tmp92F,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp931,sizeof(struct Cyc_PP_Doc*),6))))))))))))):(
# 1452
(_tmp932[3]=
# 1454
Cyc_Absynpp_rb(),((_tmp932[2]=
# 1453
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3E8,stmtexp)),((_tmp932[1]=
# 1452
Cyc_PP_blank_doc(),((_tmp932[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp932,sizeof(struct Cyc_PP_Doc*),4)))))))))),((_tmp92E[1]=
# 1446
Cyc_PP_line_doc(),((_tmp92E[0]=
# 1445
Cyc_Absynpp_stmt2doc(_tmp3E7,0),Cyc_PP_cat(_tag_dyneither(_tmp92E,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1457
struct Cyc_PP_Doc*_tmp933[3];s=((_tmp933[2]=Cyc_Absynpp_stmt2doc(_tmp3E8,stmtexp),((_tmp933[1]=Cyc_PP_line_doc(),((_tmp933[0]=Cyc_Absynpp_stmt2doc(_tmp3E7,0),Cyc_PP_cat(_tag_dyneither(_tmp933,sizeof(struct Cyc_PP_Doc*),3))))))));}}}else{
# 1460
struct Cyc_PP_Doc*_tmp934[3];s=((_tmp934[2]=Cyc_Absynpp_stmt2doc(_tmp3E8,stmtexp),((_tmp934[1]=Cyc_PP_line_doc(),((_tmp934[0]=Cyc_Absynpp_stmt2doc(_tmp3E7,0),Cyc_PP_cat(_tag_dyneither(_tmp934,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL23B;_LL242: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp3E9=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp3E9->tag != 3)goto _LL244;else{_tmp3EA=_tmp3E9->f1;}}_LL243:
# 1463
 if(_tmp3EA == 0){
const char*_tmp935;s=Cyc_PP_text(((_tmp935="return;",_tag_dyneither(_tmp935,sizeof(char),8))));}else{
# 1466
const char*_tmp93A;const char*_tmp939;struct Cyc_PP_Doc*_tmp938[3];s=((_tmp938[2]=
# 1468
Cyc_PP_text(((_tmp939=";",_tag_dyneither(_tmp939,sizeof(char),2)))),((_tmp938[1]=
# 1467
_tmp3EA == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(_tmp3EA),((_tmp938[0]=
# 1466
Cyc_PP_text(((_tmp93A="return ",_tag_dyneither(_tmp93A,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp938,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1469
goto _LL23B;_LL244: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp3EB=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp3EB->tag != 4)goto _LL246;else{_tmp3EC=_tmp3EB->f1;_tmp3ED=_tmp3EB->f2;_tmp3EE=_tmp3EB->f3;}}_LL245: {
# 1471
int print_else;
{void*_tmp422=_tmp3EE->r;void*_tmp423=_tmp422;_LL261: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp424=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp423;if(_tmp424->tag != 0)goto _LL263;}_LL262:
 print_else=0;goto _LL260;_LL263:;_LL264:
 print_else=1;goto _LL260;_LL260:;}
# 1476
{const char*_tmp949;const char*_tmp948;struct Cyc_PP_Doc*_tmp947[2];struct Cyc_PP_Doc*_tmp946[6];struct Cyc_PP_Doc*_tmp945[2];const char*_tmp944;struct Cyc_PP_Doc*_tmp943[8];s=((_tmp943[7]=
# 1483
print_else?
(_tmp946[5]=
# 1489
Cyc_Absynpp_rb(),((_tmp946[4]=
# 1488
Cyc_PP_line_doc(),((_tmp946[3]=
# 1487
Cyc_PP_nest(2,((_tmp945[1]=Cyc_Absynpp_stmt2doc(_tmp3EE,0),((_tmp945[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp945,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp946[2]=
# 1486
Cyc_Absynpp_lb(),((_tmp946[1]=
# 1485
Cyc_PP_text(((_tmp944="else ",_tag_dyneither(_tmp944,sizeof(char),6)))),((_tmp946[0]=
# 1484
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp946,sizeof(struct Cyc_PP_Doc*),6))))))))))))):
# 1490
 Cyc_PP_nil_doc(),((_tmp943[6]=
# 1482
Cyc_Absynpp_rb(),((_tmp943[5]=
# 1481
Cyc_PP_line_doc(),((_tmp943[4]=
# 1480
Cyc_PP_nest(2,((_tmp947[1]=Cyc_Absynpp_stmt2doc(_tmp3ED,0),((_tmp947[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp947,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp943[3]=
# 1479
Cyc_Absynpp_lb(),((_tmp943[2]=
# 1478
Cyc_PP_text(((_tmp948=") ",_tag_dyneither(_tmp948,sizeof(char),3)))),((_tmp943[1]=
# 1477
Cyc_Absynpp_exp2doc(_tmp3EC),((_tmp943[0]=
# 1476
Cyc_PP_text(((_tmp949="if (",_tag_dyneither(_tmp949,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp943,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1491
goto _LL23B;}_LL246: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp3EF=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp3EF->tag != 5)goto _LL248;else{_tmp3F0=(_tmp3EF->f1).f1;_tmp3F1=_tmp3EF->f2;}}_LL247:
# 1493
{const char*_tmp950;const char*_tmp94F;struct Cyc_PP_Doc*_tmp94E[2];struct Cyc_PP_Doc*_tmp94D[7];s=((_tmp94D[6]=
# 1499
Cyc_Absynpp_rb(),((_tmp94D[5]=
# 1498
Cyc_PP_line_doc(),((_tmp94D[4]=
# 1497
Cyc_PP_nest(2,((_tmp94E[1]=Cyc_Absynpp_stmt2doc(_tmp3F1,0),((_tmp94E[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp94E,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp94D[3]=
# 1496
Cyc_Absynpp_lb(),((_tmp94D[2]=
# 1495
Cyc_PP_text(((_tmp94F=") ",_tag_dyneither(_tmp94F,sizeof(char),3)))),((_tmp94D[1]=
# 1494
Cyc_Absynpp_exp2doc(_tmp3F0),((_tmp94D[0]=
# 1493
Cyc_PP_text(((_tmp950="while (",_tag_dyneither(_tmp950,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp94D,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1500
goto _LL23B;_LL248: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp3F2=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp3F2->tag != 6)goto _LL24A;}_LL249:
{const char*_tmp951;s=Cyc_PP_text(((_tmp951="break;",_tag_dyneither(_tmp951,sizeof(char),7))));}goto _LL23B;_LL24A: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp3F3=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp3F3->tag != 7)goto _LL24C;}_LL24B:
{const char*_tmp952;s=Cyc_PP_text(((_tmp952="continue;",_tag_dyneither(_tmp952,sizeof(char),10))));}goto _LL23B;_LL24C: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp3F4=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp3F4->tag != 8)goto _LL24E;else{_tmp3F5=_tmp3F4->f1;}}_LL24D:
{const char*_tmp956;void*_tmp955[1];struct Cyc_String_pa_PrintArg_struct _tmp954;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp954.tag=0,((_tmp954.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3F5),((_tmp955[0]=& _tmp954,Cyc_aprintf(((_tmp956="goto %s;",_tag_dyneither(_tmp956,sizeof(char),9))),_tag_dyneither(_tmp955,sizeof(void*),1)))))))));}goto _LL23B;_LL24E: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp3F6=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp3F6->tag != 9)goto _LL250;else{_tmp3F7=_tmp3F6->f1;_tmp3F8=(_tmp3F6->f2).f1;_tmp3F9=(_tmp3F6->f3).f1;_tmp3FA=_tmp3F6->f4;}}_LL24F:
# 1505
{const char*_tmp961;const char*_tmp960;const char*_tmp95F;const char*_tmp95E;struct Cyc_PP_Doc*_tmp95D[2];struct Cyc_PP_Doc*_tmp95C[11];s=((_tmp95C[10]=
# 1515
Cyc_Absynpp_rb(),((_tmp95C[9]=
# 1514
Cyc_PP_line_doc(),((_tmp95C[8]=
# 1513
Cyc_PP_nest(2,((_tmp95D[1]=Cyc_Absynpp_stmt2doc(_tmp3FA,0),((_tmp95D[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp95D,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp95C[7]=
# 1512
Cyc_Absynpp_lb(),((_tmp95C[6]=
# 1511
Cyc_PP_text(((_tmp95E=") ",_tag_dyneither(_tmp95E,sizeof(char),3)))),((_tmp95C[5]=
# 1510
Cyc_Absynpp_exp2doc(_tmp3F9),((_tmp95C[4]=
# 1509
Cyc_PP_text(((_tmp95F="; ",_tag_dyneither(_tmp95F,sizeof(char),3)))),((_tmp95C[3]=
# 1508
Cyc_Absynpp_exp2doc(_tmp3F8),((_tmp95C[2]=
# 1507
Cyc_PP_text(((_tmp960="; ",_tag_dyneither(_tmp960,sizeof(char),3)))),((_tmp95C[1]=
# 1506
Cyc_Absynpp_exp2doc(_tmp3F7),((_tmp95C[0]=
# 1505
Cyc_PP_text(((_tmp961="for(",_tag_dyneither(_tmp961,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp95C,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1516
goto _LL23B;_LL250: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp3FB=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp3FB->tag != 10)goto _LL252;else{_tmp3FC=_tmp3FB->f1;_tmp3FD=_tmp3FB->f2;}}_LL251:
# 1518
{const char*_tmp966;const char*_tmp965;struct Cyc_PP_Doc*_tmp964[8];s=((_tmp964[7]=
# 1525
Cyc_Absynpp_rb(),((_tmp964[6]=
# 1524
Cyc_PP_line_doc(),((_tmp964[5]=
# 1523
Cyc_Absynpp_switchclauses2doc(_tmp3FD),((_tmp964[4]=
# 1522
Cyc_PP_line_doc(),((_tmp964[3]=
# 1521
Cyc_Absynpp_lb(),((_tmp964[2]=
# 1520
Cyc_PP_text(((_tmp965=") ",_tag_dyneither(_tmp965,sizeof(char),3)))),((_tmp964[1]=
# 1519
Cyc_Absynpp_exp2doc(_tmp3FC),((_tmp964[0]=
# 1518
Cyc_PP_text(((_tmp966="switch (",_tag_dyneither(_tmp966,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp964,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1526
goto _LL23B;_LL252: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3FE=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp3FE->tag != 11)goto _LL254;else{if(_tmp3FE->f1 != 0)goto _LL254;}}_LL253:
{const char*_tmp967;s=Cyc_PP_text(((_tmp967="fallthru;",_tag_dyneither(_tmp967,sizeof(char),10))));}goto _LL23B;_LL254: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3FF=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp3FF->tag != 11)goto _LL256;else{_tmp400=_tmp3FF->f1;}}_LL255:
# 1529
{const char*_tmp96C;const char*_tmp96B;struct Cyc_PP_Doc*_tmp96A[3];s=((_tmp96A[2]=Cyc_PP_text(((_tmp96B=");",_tag_dyneither(_tmp96B,sizeof(char),3)))),((_tmp96A[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp400),((_tmp96A[0]=Cyc_PP_text(((_tmp96C="fallthru(",_tag_dyneither(_tmp96C,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp96A,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL23B;_LL256: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp401=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp401->tag != 12)goto _LL258;else{_tmp402=_tmp401->f1;_tmp403=_tmp401->f2;}}_LL257:
# 1531
{struct Cyc_PP_Doc*_tmp96D[3];s=((_tmp96D[2]=
# 1533
Cyc_Absynpp_stmt2doc(_tmp403,stmtexp),((_tmp96D[1]=
# 1532
Cyc_PP_line_doc(),((_tmp96D[0]=
# 1531
Cyc_Absynpp_decl2doc(_tmp402),Cyc_PP_cat(_tag_dyneither(_tmp96D,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1534
goto _LL23B;_LL258: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp404=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp404->tag != 13)goto _LL25A;else{_tmp405=_tmp404->f1;_tmp406=_tmp404->f2;}}_LL259:
# 1536
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp406)){
if(stmtexp){
const char*_tmp972;const char*_tmp971;struct Cyc_PP_Doc*_tmp970[8];s=((_tmp970[7]=
# 1544
Cyc_PP_text(((_tmp971=");",_tag_dyneither(_tmp971,sizeof(char),3)))),((_tmp970[6]=Cyc_Absynpp_rb(),((_tmp970[5]=
# 1543
Cyc_PP_line_doc(),((_tmp970[4]=
# 1542
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp406,1)),((_tmp970[3]=
# 1541
Cyc_PP_line_doc(),((_tmp970[2]=
# 1540
Cyc_Absynpp_lb(),((_tmp970[1]=
# 1539
Cyc_PP_text(((_tmp972=": (",_tag_dyneither(_tmp972,sizeof(char),4)))),((_tmp970[0]=
# 1538
Cyc_PP_textptr(_tmp405),Cyc_PP_cat(_tag_dyneither(_tmp970,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1546
const char*_tmp975;struct Cyc_PP_Doc*_tmp974[7];s=((_tmp974[6]=
# 1552
Cyc_Absynpp_rb(),((_tmp974[5]=
# 1551
Cyc_PP_line_doc(),((_tmp974[4]=
# 1550
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp406,0)),((_tmp974[3]=
# 1549
Cyc_PP_line_doc(),((_tmp974[2]=
# 1548
Cyc_Absynpp_lb(),((_tmp974[1]=
# 1547
Cyc_PP_text(((_tmp975=": ",_tag_dyneither(_tmp975,sizeof(char),3)))),((_tmp974[0]=
# 1546
Cyc_PP_textptr(_tmp405),Cyc_PP_cat(_tag_dyneither(_tmp974,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1554
const char*_tmp978;struct Cyc_PP_Doc*_tmp977[3];s=((_tmp977[2]=Cyc_Absynpp_stmt2doc(_tmp406,stmtexp),((_tmp977[1]=Cyc_PP_text(((_tmp978=": ",_tag_dyneither(_tmp978,sizeof(char),3)))),((_tmp977[0]=Cyc_PP_textptr(_tmp405),Cyc_PP_cat(_tag_dyneither(_tmp977,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL23B;_LL25A: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp407=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp407->tag != 14)goto _LL25C;else{_tmp408=_tmp407->f1;_tmp409=(_tmp407->f2).f1;}}_LL25B:
# 1557
{const char*_tmp97F;const char*_tmp97E;const char*_tmp97D;struct Cyc_PP_Doc*_tmp97C[9];s=((_tmp97C[8]=
# 1565
Cyc_PP_text(((_tmp97D=");",_tag_dyneither(_tmp97D,sizeof(char),3)))),((_tmp97C[7]=
# 1564
Cyc_Absynpp_exp2doc(_tmp409),((_tmp97C[6]=
# 1563
Cyc_PP_text(((_tmp97E=" while (",_tag_dyneither(_tmp97E,sizeof(char),9)))),((_tmp97C[5]=
# 1562
Cyc_Absynpp_rb(),((_tmp97C[4]=
# 1561
Cyc_PP_line_doc(),((_tmp97C[3]=
# 1560
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp408,0)),((_tmp97C[2]=
# 1559
Cyc_PP_line_doc(),((_tmp97C[1]=
# 1558
Cyc_Absynpp_lb(),((_tmp97C[0]=
# 1557
Cyc_PP_text(((_tmp97F="do ",_tag_dyneither(_tmp97F,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp97C,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1566
goto _LL23B;_LL25C: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp40A=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp40A->tag != 15)goto _LL25E;else{_tmp40B=_tmp40A->f1;_tmp40C=_tmp40A->f2;}}_LL25D:
# 1568
{const char*_tmp984;const char*_tmp983;struct Cyc_PP_Doc*_tmp982[12];s=((_tmp982[11]=
# 1579
Cyc_Absynpp_rb(),((_tmp982[10]=
# 1578
Cyc_PP_line_doc(),((_tmp982[9]=
# 1577
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp40C)),((_tmp982[8]=
# 1576
Cyc_PP_line_doc(),((_tmp982[7]=
# 1575
Cyc_Absynpp_lb(),((_tmp982[6]=
# 1574
Cyc_PP_text(((_tmp983=" catch ",_tag_dyneither(_tmp983,sizeof(char),8)))),((_tmp982[5]=
# 1573
Cyc_Absynpp_rb(),((_tmp982[4]=
# 1572
Cyc_PP_line_doc(),((_tmp982[3]=
# 1571
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp40B,0)),((_tmp982[2]=
# 1570
Cyc_PP_line_doc(),((_tmp982[1]=
# 1569
Cyc_Absynpp_lb(),((_tmp982[0]=
# 1568
Cyc_PP_text(((_tmp984="try ",_tag_dyneither(_tmp984,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp982,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
# 1580
goto _LL23B;_LL25E: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp40D=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp40D->tag != 16)goto _LL23B;else{_tmp40E=_tmp40D->f1;}}_LL25F:
# 1582
{const char*_tmp989;const char*_tmp988;struct Cyc_PP_Doc*_tmp987[3];s=((_tmp987[2]=Cyc_PP_text(((_tmp988=");",_tag_dyneither(_tmp988,sizeof(char),3)))),((_tmp987[1]=Cyc_Absynpp_exp2doc(_tmp40E),((_tmp987[0]=Cyc_PP_text(((_tmp989="reset_region(",_tag_dyneither(_tmp989,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp987,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL23B;_LL23B:;}
# 1585
return s;}
# 1588
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp454=p->r;void*_tmp455=_tmp454;enum Cyc_Absyn_Sign _tmp459;int _tmp45A;char _tmp45C;struct _dyneither_ptr _tmp45E;struct Cyc_Absyn_Vardecl*_tmp460;struct Cyc_Absyn_Vardecl*_tmp463;struct Cyc_Absyn_Pat*_tmp464;struct Cyc_Absyn_Tvar*_tmp466;struct Cyc_Absyn_Vardecl*_tmp467;struct Cyc_Absyn_Tvar*_tmp469;struct Cyc_Absyn_Vardecl*_tmp46A;struct Cyc_List_List*_tmp46C;int _tmp46D;struct Cyc_Absyn_Pat*_tmp46F;struct Cyc_Absyn_Vardecl*_tmp471;struct Cyc_Absyn_Vardecl*_tmp474;struct Cyc_Absyn_Pat*_tmp475;struct _tuple0*_tmp477;struct _tuple0*_tmp479;struct Cyc_List_List*_tmp47A;int _tmp47B;union Cyc_Absyn_AggrInfoU _tmp47D;struct Cyc_List_List*_tmp47E;struct Cyc_List_List*_tmp47F;int _tmp480;struct Cyc_List_List*_tmp482;struct Cyc_List_List*_tmp483;int _tmp484;struct Cyc_Absyn_Enumfield*_tmp486;struct Cyc_Absyn_Enumfield*_tmp488;struct Cyc_Absyn_Datatypefield*_tmp48A;struct Cyc_Absyn_Datatypefield*_tmp48C;struct Cyc_List_List*_tmp48D;int _tmp48E;struct Cyc_Absyn_Exp*_tmp490;_LL266: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp456=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp456->tag != 0)goto _LL268;}_LL267:
{const char*_tmp98A;s=Cyc_PP_text(((_tmp98A="_",_tag_dyneither(_tmp98A,sizeof(char),2))));}goto _LL265;_LL268: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp457=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp457->tag != 9)goto _LL26A;}_LL269:
{const char*_tmp98B;s=Cyc_PP_text(((_tmp98B="NULL",_tag_dyneither(_tmp98B,sizeof(char),5))));}goto _LL265;_LL26A: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp458=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp458->tag != 10)goto _LL26C;else{_tmp459=_tmp458->f1;_tmp45A=_tmp458->f2;}}_LL26B:
# 1594
 if(_tmp459 != Cyc_Absyn_Unsigned){
const char*_tmp98F;void*_tmp98E[1];struct Cyc_Int_pa_PrintArg_struct _tmp98D;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp98D.tag=1,((_tmp98D.f1=(unsigned long)_tmp45A,((_tmp98E[0]=& _tmp98D,Cyc_aprintf(((_tmp98F="%d",_tag_dyneither(_tmp98F,sizeof(char),3))),_tag_dyneither(_tmp98E,sizeof(void*),1)))))))));}else{
const char*_tmp993;void*_tmp992[1];struct Cyc_Int_pa_PrintArg_struct _tmp991;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp991.tag=1,((_tmp991.f1=(unsigned int)_tmp45A,((_tmp992[0]=& _tmp991,Cyc_aprintf(((_tmp993="%u",_tag_dyneither(_tmp993,sizeof(char),3))),_tag_dyneither(_tmp992,sizeof(void*),1)))))))));}
goto _LL265;_LL26C: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp45B=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp45B->tag != 11)goto _LL26E;else{_tmp45C=_tmp45B->f1;}}_LL26D:
{const char*_tmp997;void*_tmp996[1];struct Cyc_String_pa_PrintArg_struct _tmp995;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp995.tag=0,((_tmp995.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp45C)),((_tmp996[0]=& _tmp995,Cyc_aprintf(((_tmp997="'%s'",_tag_dyneither(_tmp997,sizeof(char),5))),_tag_dyneither(_tmp996,sizeof(void*),1)))))))));}goto _LL265;_LL26E: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp45D=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp45D->tag != 12)goto _LL270;else{_tmp45E=_tmp45D->f1;}}_LL26F:
 s=Cyc_PP_text(_tmp45E);goto _LL265;_LL270: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp45F=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp45F->tag != 1)goto _LL272;else{_tmp460=_tmp45F->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp461=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp45F->f2)->r;if(_tmp461->tag != 0)goto _LL272;};}}_LL271:
# 1601
 s=Cyc_Absynpp_qvar2doc(_tmp460->name);goto _LL265;_LL272: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp462=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp462->tag != 1)goto _LL274;else{_tmp463=_tmp462->f1;_tmp464=_tmp462->f2;}}_LL273:
# 1603
{const char*_tmp99A;struct Cyc_PP_Doc*_tmp999[3];s=((_tmp999[2]=Cyc_Absynpp_pat2doc(_tmp464),((_tmp999[1]=Cyc_PP_text(((_tmp99A=" as ",_tag_dyneither(_tmp99A,sizeof(char),5)))),((_tmp999[0]=Cyc_Absynpp_qvar2doc(_tmp463->name),Cyc_PP_cat(_tag_dyneither(_tmp999,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL265;_LL274: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp465=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp465->tag != 2)goto _LL276;else{_tmp466=_tmp465->f1;_tmp467=_tmp465->f2;}}_LL275:
# 1605
{const char*_tmp9A1;const char*_tmp9A0;const char*_tmp99F;struct Cyc_PP_Doc*_tmp99E[5];s=((_tmp99E[4]=
Cyc_Absynpp_vardecl2doc(_tmp467),((_tmp99E[3]=
# 1605
Cyc_PP_text(((_tmp99F="> ",_tag_dyneither(_tmp99F,sizeof(char),3)))),((_tmp99E[2]=Cyc_Absynpp_tvar2doc(_tmp466),((_tmp99E[1]=Cyc_PP_text(((_tmp9A0=" <",_tag_dyneither(_tmp9A0,sizeof(char),3)))),((_tmp99E[0]=Cyc_PP_text(((_tmp9A1="alias",_tag_dyneither(_tmp9A1,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp99E,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1607
goto _LL265;_LL276: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp468=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp468->tag != 4)goto _LL278;else{_tmp469=_tmp468->f1;_tmp46A=_tmp468->f2;}}_LL277:
# 1609
{const char*_tmp9A6;const char*_tmp9A5;struct Cyc_PP_Doc*_tmp9A4[4];s=((_tmp9A4[3]=Cyc_PP_text(((_tmp9A5=">",_tag_dyneither(_tmp9A5,sizeof(char),2)))),((_tmp9A4[2]=Cyc_Absynpp_tvar2doc(_tmp469),((_tmp9A4[1]=Cyc_PP_text(((_tmp9A6="<",_tag_dyneither(_tmp9A6,sizeof(char),2)))),((_tmp9A4[0]=Cyc_Absynpp_qvar2doc(_tmp46A->name),Cyc_PP_cat(_tag_dyneither(_tmp9A4,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL265;_LL278: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp46B=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp46B->tag != 5)goto _LL27A;else{_tmp46C=_tmp46B->f1;_tmp46D=_tmp46B->f2;}}_LL279:
# 1612
{const char*_tmp9AF;const char*_tmp9AE;const char*_tmp9AD;const char*_tmp9AC;struct Cyc_PP_Doc*_tmp9AB[4];s=((_tmp9AB[3]=
_tmp46D?Cyc_PP_text(((_tmp9AC=", ...)",_tag_dyneither(_tmp9AC,sizeof(char),7)))): Cyc_PP_text(((_tmp9AD=")",_tag_dyneither(_tmp9AD,sizeof(char),2)))),((_tmp9AB[2]=
# 1612
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp9AE=",",_tag_dyneither(_tmp9AE,sizeof(char),2))),_tmp46C),((_tmp9AB[1]=Cyc_PP_text(((_tmp9AF="(",_tag_dyneither(_tmp9AF,sizeof(char),2)))),((_tmp9AB[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp9AB,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1614
goto _LL265;_LL27A: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp46E=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp46E->tag != 6)goto _LL27C;else{_tmp46F=_tmp46E->f1;}}_LL27B:
# 1616
{const char*_tmp9B2;struct Cyc_PP_Doc*_tmp9B1[2];s=((_tmp9B1[1]=Cyc_Absynpp_pat2doc(_tmp46F),((_tmp9B1[0]=Cyc_PP_text(((_tmp9B2="&",_tag_dyneither(_tmp9B2,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp9B1,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL265;_LL27C: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp470=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp470->tag != 3)goto _LL27E;else{_tmp471=_tmp470->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp472=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp470->f2)->r;if(_tmp472->tag != 0)goto _LL27E;};}}_LL27D:
# 1619
{const char*_tmp9B5;struct Cyc_PP_Doc*_tmp9B4[2];s=((_tmp9B4[1]=Cyc_Absynpp_qvar2doc(_tmp471->name),((_tmp9B4[0]=Cyc_PP_text(((_tmp9B5="*",_tag_dyneither(_tmp9B5,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp9B4,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL265;_LL27E: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp473=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp473->tag != 3)goto _LL280;else{_tmp474=_tmp473->f1;_tmp475=_tmp473->f2;}}_LL27F:
# 1622
{const char*_tmp9BA;const char*_tmp9B9;struct Cyc_PP_Doc*_tmp9B8[4];s=((_tmp9B8[3]=Cyc_Absynpp_pat2doc(_tmp475),((_tmp9B8[2]=Cyc_PP_text(((_tmp9B9=" as ",_tag_dyneither(_tmp9B9,sizeof(char),5)))),((_tmp9B8[1]=Cyc_Absynpp_qvar2doc(_tmp474->name),((_tmp9B8[0]=Cyc_PP_text(((_tmp9BA="*",_tag_dyneither(_tmp9BA,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp9B8,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL265;_LL280: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp476=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp476->tag != 15)goto _LL282;else{_tmp477=_tmp476->f1;}}_LL281:
# 1625
 s=Cyc_Absynpp_qvar2doc(_tmp477);
goto _LL265;_LL282: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp478=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp478->tag != 16)goto _LL284;else{_tmp479=_tmp478->f1;_tmp47A=_tmp478->f2;_tmp47B=_tmp478->f3;}}_LL283: {
# 1628
const char*_tmp9BC;const char*_tmp9BB;struct _dyneither_ptr term=_tmp47B?(_tmp9BC=", ...)",_tag_dyneither(_tmp9BC,sizeof(char),7)):((_tmp9BB=")",_tag_dyneither(_tmp9BB,sizeof(char),2)));
{const char*_tmp9C1;const char*_tmp9C0;struct Cyc_PP_Doc*_tmp9BF[2];s=((_tmp9BF[1]=Cyc_PP_group(((_tmp9C1="(",_tag_dyneither(_tmp9C1,sizeof(char),2))),term,((_tmp9C0=",",_tag_dyneither(_tmp9C0,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp47A)),((_tmp9BF[0]=Cyc_Absynpp_qvar2doc(_tmp479),Cyc_PP_cat(_tag_dyneither(_tmp9BF,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL265;}_LL284: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp47C=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp47C->tag != 7)goto _LL286;else{if(_tmp47C->f1 == 0)goto _LL286;_tmp47D=(_tmp47C->f1)->aggr_info;_tmp47E=_tmp47C->f2;_tmp47F=_tmp47C->f3;_tmp480=_tmp47C->f4;}}_LL285: {
# 1632
const char*_tmp9C3;const char*_tmp9C2;struct _dyneither_ptr term=_tmp480?(_tmp9C3=", ...}",_tag_dyneither(_tmp9C3,sizeof(char),7)):((_tmp9C2="}",_tag_dyneither(_tmp9C2,sizeof(char),2)));
struct _tuple10 _tmp4B6=Cyc_Absyn_aggr_kinded_name(_tmp47D);struct _tuple0*_tmp4B8;struct _tuple10 _tmp4B7=_tmp4B6;_tmp4B8=_tmp4B7.f2;
{const char*_tmp9CE;const char*_tmp9CD;const char*_tmp9CC;const char*_tmp9CB;const char*_tmp9CA;struct Cyc_PP_Doc*_tmp9C9[4];s=((_tmp9C9[3]=
# 1636
Cyc_PP_group(((_tmp9CB="",_tag_dyneither(_tmp9CB,sizeof(char),1))),term,((_tmp9CA=",",_tag_dyneither(_tmp9CA,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp47F)),((_tmp9C9[2]=
# 1635
Cyc_PP_egroup(((_tmp9CE="[",_tag_dyneither(_tmp9CE,sizeof(char),2))),((_tmp9CD="]",_tag_dyneither(_tmp9CD,sizeof(char),2))),((_tmp9CC=",",_tag_dyneither(_tmp9CC,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp47E)),((_tmp9C9[1]=
# 1634
Cyc_Absynpp_lb(),((_tmp9C9[0]=Cyc_Absynpp_qvar2doc(_tmp4B8),Cyc_PP_cat(_tag_dyneither(_tmp9C9,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1637
goto _LL265;}_LL286: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp481=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp481->tag != 7)goto _LL288;else{if(_tmp481->f1 != 0)goto _LL288;_tmp482=_tmp481->f2;_tmp483=_tmp481->f3;_tmp484=_tmp481->f4;}}_LL287: {
# 1639
const char*_tmp9D0;const char*_tmp9CF;struct _dyneither_ptr term=_tmp484?(_tmp9D0=", ...}",_tag_dyneither(_tmp9D0,sizeof(char),7)):((_tmp9CF="}",_tag_dyneither(_tmp9CF,sizeof(char),2)));
{const char*_tmp9DB;const char*_tmp9DA;const char*_tmp9D9;const char*_tmp9D8;const char*_tmp9D7;struct Cyc_PP_Doc*_tmp9D6[3];s=((_tmp9D6[2]=
# 1642
Cyc_PP_group(((_tmp9D8="",_tag_dyneither(_tmp9D8,sizeof(char),1))),term,((_tmp9D7=",",_tag_dyneither(_tmp9D7,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp483)),((_tmp9D6[1]=
# 1641
Cyc_PP_egroup(((_tmp9DB="[",_tag_dyneither(_tmp9DB,sizeof(char),2))),((_tmp9DA="]",_tag_dyneither(_tmp9DA,sizeof(char),2))),((_tmp9D9=",",_tag_dyneither(_tmp9D9,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp482)),((_tmp9D6[0]=
# 1640
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp9D6,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1643
goto _LL265;}_LL288: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp485=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp485->tag != 13)goto _LL28A;else{_tmp486=_tmp485->f2;}}_LL289:
 _tmp488=_tmp486;goto _LL28B;_LL28A: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp487=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp487->tag != 14)goto _LL28C;else{_tmp488=_tmp487->f2;}}_LL28B:
 s=Cyc_Absynpp_qvar2doc(_tmp488->name);goto _LL265;_LL28C: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp489=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp489->tag != 8)goto _LL28E;else{_tmp48A=_tmp489->f2;if(_tmp489->f3 != 0)goto _LL28E;}}_LL28D:
 s=Cyc_Absynpp_qvar2doc(_tmp48A->name);goto _LL265;_LL28E: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp48B=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp48B->tag != 8)goto _LL290;else{_tmp48C=_tmp48B->f2;_tmp48D=_tmp48B->f3;_tmp48E=_tmp48B->f4;}}_LL28F: {
# 1648
const char*_tmp9DD;const char*_tmp9DC;struct _dyneither_ptr term=_tmp48E?(_tmp9DD=", ...)",_tag_dyneither(_tmp9DD,sizeof(char),7)):((_tmp9DC=")",_tag_dyneither(_tmp9DC,sizeof(char),2)));
{const char*_tmp9E2;const char*_tmp9E1;struct Cyc_PP_Doc*_tmp9E0[2];s=((_tmp9E0[1]=Cyc_PP_egroup(((_tmp9E2="(",_tag_dyneither(_tmp9E2,sizeof(char),2))),term,((_tmp9E1=",",_tag_dyneither(_tmp9E1,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp48D)),((_tmp9E0[0]=Cyc_Absynpp_qvar2doc(_tmp48C->name),Cyc_PP_cat(_tag_dyneither(_tmp9E0,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL265;}_LL290: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp48F=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp455;if(_tmp48F->tag != 17)goto _LL265;else{_tmp490=_tmp48F->f1;}}_LL291:
# 1652
 s=Cyc_Absynpp_exp2doc(_tmp490);goto _LL265;_LL265:;}
# 1654
return s;}
# 1657
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
const char*_tmp9E9;const char*_tmp9E8;const char*_tmp9E7;struct Cyc_PP_Doc*_tmp9E6[2];return(_tmp9E6[1]=
Cyc_Absynpp_pat2doc((*dp).f2),((_tmp9E6[0]=
# 1658
Cyc_PP_egroup(((_tmp9E9="",_tag_dyneither(_tmp9E9,sizeof(char),1))),((_tmp9E8="=",_tag_dyneither(_tmp9E8,sizeof(char),2))),((_tmp9E7="=",_tag_dyneither(_tmp9E7,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp9E6,sizeof(struct Cyc_PP_Doc*),2)))));}
# 1662
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){
const char*_tmp9EE;struct Cyc_PP_Doc*_tmp9ED[2];struct Cyc_PP_Doc*_tmp9EC[2];return(_tmp9EC[1]=
Cyc_PP_nest(2,((_tmp9ED[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp9ED[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9ED,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9EC[0]=
# 1664
Cyc_PP_text(((_tmp9EE="default: ",_tag_dyneither(_tmp9EE,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp9EC,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 1666
if(c->where_clause == 0){
const char*_tmp9F5;const char*_tmp9F4;struct Cyc_PP_Doc*_tmp9F3[2];struct Cyc_PP_Doc*_tmp9F2[4];return(_tmp9F2[3]=
# 1670
Cyc_PP_nest(2,((_tmp9F3[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp9F3[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9F3,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9F2[2]=
# 1669
Cyc_PP_text(((_tmp9F4=": ",_tag_dyneither(_tmp9F4,sizeof(char),3)))),((_tmp9F2[1]=
# 1668
Cyc_Absynpp_pat2doc(c->pattern),((_tmp9F2[0]=
# 1667
Cyc_PP_text(((_tmp9F5="case ",_tag_dyneither(_tmp9F5,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9F2,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1672
const char*_tmp9FE;const char*_tmp9FD;const char*_tmp9FC;struct Cyc_PP_Doc*_tmp9FB[2];struct Cyc_PP_Doc*_tmp9FA[6];return(_tmp9FA[5]=
# 1677
Cyc_PP_nest(2,((_tmp9FB[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp9FB[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9FB,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9FA[4]=
# 1676
Cyc_PP_text(((_tmp9FC=": ",_tag_dyneither(_tmp9FC,sizeof(char),3)))),((_tmp9FA[3]=
# 1675
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp9FA[2]=
# 1674
Cyc_PP_text(((_tmp9FD=" && ",_tag_dyneither(_tmp9FD,sizeof(char),5)))),((_tmp9FA[1]=
# 1673
Cyc_Absynpp_pat2doc(c->pattern),((_tmp9FA[0]=
# 1672
Cyc_PP_text(((_tmp9FE="case ",_tag_dyneither(_tmp9FE,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9FA,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
# 1680
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
const char*_tmp9FF;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((_tmp9FF="",_tag_dyneither(_tmp9FF,sizeof(char),1))),cs);}
# 1684
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1688
const char*_tmpA02;struct Cyc_PP_Doc*_tmpA01[3];return(_tmpA01[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmpA01[1]=Cyc_PP_text(((_tmpA02=" = ",_tag_dyneither(_tmpA02,sizeof(char),4)))),((_tmpA01[0]=Cyc_Absynpp_qvar2doc(f->name),Cyc_PP_cat(_tag_dyneither(_tmpA01,sizeof(struct Cyc_PP_Doc*),3)))))));}}
# 1691
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
const char*_tmpA03;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmpA03=",",_tag_dyneither(_tmpA03,sizeof(char),2))),fs);}
# 1695
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1699
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
const char*_tmpA04;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,((_tmpA04=",",_tag_dyneither(_tmpA04,sizeof(char),2))),vds);}
# 1703
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
enum Cyc_Absyn_Scope _tmp4E4;struct _tuple0*_tmp4E5;struct Cyc_Absyn_Tqual _tmp4E6;void*_tmp4E7;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_List_List*_tmp4E9;struct Cyc_Absyn_Vardecl*_tmp4E3=vd;_tmp4E4=_tmp4E3->sc;_tmp4E5=_tmp4E3->name;_tmp4E6=_tmp4E3->tq;_tmp4E7=_tmp4E3->type;_tmp4E8=_tmp4E3->initializer;_tmp4E9=_tmp4E3->attributes;{
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp4E5);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4E9);
struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL292:
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL293:
# 1712
{void*_tmp4EA=Cyc_Tcutil_compress(_tmp4E7);void*_tmp4EB=_tmp4EA;struct Cyc_List_List*_tmp4ED;_LL296: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4EC=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4EB;if(_tmp4EC->tag != 9)goto _LL298;else{_tmp4ED=(_tmp4EC->f1).attributes;}}_LL297:
# 1714
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4ED);
goto _LL295;_LL298:;_LL299:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL295;_LL295:;}
# 1718
break;}{
# 1721
struct Cyc_PP_Doc*tmp_doc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL29A:
 tmp_doc=Cyc_PP_nil_doc();break;case Cyc_Cyclone_Vc_c: _LL29B:
 tmp_doc=attsdoc;break;}
# 1726
{struct Cyc_Core_Opt*_tmpA11;struct Cyc_PP_Doc*_tmpA10[2];struct Cyc_PP_Doc*_tmpA0F[2];const char*_tmpA0E;const char*_tmpA0D;struct Cyc_PP_Doc*_tmpA0C[5];s=((_tmpA0C[4]=
# 1733
Cyc_PP_text(((_tmpA0D=";",_tag_dyneither(_tmpA0D,sizeof(char),2)))),((_tmpA0C[3]=
# 1730
_tmp4E8 == 0?
Cyc_PP_nil_doc():(
(_tmpA0F[1]=Cyc_Absynpp_exp2doc(_tmp4E8),((_tmpA0F[0]=Cyc_PP_text(((_tmpA0E=" = ",_tag_dyneither(_tmpA0E,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmpA0F,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmpA0C[2]=
# 1729
Cyc_Absynpp_tqtd2doc(_tmp4E6,_tmp4E7,((_tmpA11=_cycalloc(sizeof(*_tmpA11)),((_tmpA11->v=((_tmpA10[1]=sn,((_tmpA10[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmpA10,sizeof(struct Cyc_PP_Doc*),2)))))),_tmpA11))))),((_tmpA0C[1]=
# 1728
Cyc_Absynpp_scope2doc(_tmp4E4),((_tmpA0C[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmpA0C,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1734
return s;};};}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1737
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple17 _tmp4F4=*x;struct _tuple0*_tmp4F6;struct _tuple17 _tmp4F5=_tmp4F4;_tmp4F6=_tmp4F5.f2;
return Cyc_Absynpp_qvar2doc(_tmp4F6);}
# 1742
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0){
struct Cyc_PP_Doc*_tmpA12[4];return(_tmpA12[3]=
# 1747
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmpA12[2]=
# 1746
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmpA12[1]=
# 1745
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmpA12[0]=
# 1744
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmpA12,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1749
struct Cyc_PP_Doc*_tmpA1A[2];const char*_tmpA19;struct Cyc_PP_Doc*_tmpA18[2];struct Cyc_PP_Doc*_tmpA17[12];return(_tmpA17[11]=
# 1760
Cyc_Absynpp_atts2doc(ad->attributes),((_tmpA17[10]=
# 1759
Cyc_Absynpp_rb(),((_tmpA17[9]=
# 1758
Cyc_PP_line_doc(),((_tmpA17[8]=
# 1757
Cyc_PP_nest(2,((_tmpA18[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields),((_tmpA18[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA18,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmpA17[7]=
# 1755
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():(
(_tmpA1A[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po),((_tmpA1A[0]=Cyc_PP_text(((_tmpA19=":",_tag_dyneither(_tmpA19,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmpA1A,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmpA17[6]=
# 1754
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars),((_tmpA17[5]=
# 1753
Cyc_Absynpp_lb(),((_tmpA17[4]=Cyc_PP_blank_doc(),((_tmpA17[3]=
# 1752
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmpA17[2]=
# 1751
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmpA17[1]=
# 1750
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmpA17[0]=
# 1749
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmpA17,sizeof(struct Cyc_PP_Doc*),12)))))))))))))))))))))))));}}
# 1763
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
enum Cyc_Absyn_Scope _tmp4FD;struct _tuple0*_tmp4FE;struct Cyc_List_List*_tmp4FF;struct Cyc_Core_Opt*_tmp500;int _tmp501;struct Cyc_Absyn_Datatypedecl*_tmp4FC=dd;_tmp4FD=_tmp4FC->sc;_tmp4FE=_tmp4FC->name;_tmp4FF=_tmp4FC->tvs;_tmp500=_tmp4FC->fields;_tmp501=_tmp4FC->is_extensible;
if(_tmp500 == 0){
const char*_tmpA1F;const char*_tmpA1E;struct Cyc_PP_Doc*_tmpA1D[5];return(_tmpA1D[4]=
# 1770
Cyc_Absynpp_ktvars2doc(_tmp4FF),((_tmpA1D[3]=
# 1769
_tmp501?Cyc_Absynpp_qvar2bolddoc(_tmp4FE): Cyc_Absynpp_typedef_name2bolddoc(_tmp4FE),((_tmpA1D[2]=
# 1768
Cyc_PP_text(((_tmpA1E="datatype ",_tag_dyneither(_tmpA1E,sizeof(char),10)))),((_tmpA1D[1]=
# 1767
_tmp501?Cyc_PP_text(((_tmpA1F="@extensible ",_tag_dyneither(_tmpA1F,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmpA1D[0]=
# 1766
Cyc_Absynpp_scope2doc(_tmp4FD),Cyc_PP_cat(_tag_dyneither(_tmpA1D,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 1772
const char*_tmpA26;const char*_tmpA25;struct Cyc_PP_Doc*_tmpA24[2];struct Cyc_PP_Doc*_tmpA23[10];return(_tmpA23[9]=
# 1780
Cyc_Absynpp_rb(),((_tmpA23[8]=
# 1779
Cyc_PP_line_doc(),((_tmpA23[7]=
# 1778
Cyc_PP_nest(2,((_tmpA24[1]=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp500->v),((_tmpA24[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA24,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmpA23[6]=
# 1777
Cyc_Absynpp_lb(),((_tmpA23[5]=Cyc_PP_blank_doc(),((_tmpA23[4]=
# 1776
Cyc_Absynpp_ktvars2doc(_tmp4FF),((_tmpA23[3]=
# 1775
_tmp501?Cyc_Absynpp_qvar2bolddoc(_tmp4FE): Cyc_Absynpp_typedef_name2bolddoc(_tmp4FE),((_tmpA23[2]=
# 1774
Cyc_PP_text(((_tmpA25="datatype ",_tag_dyneither(_tmpA25,sizeof(char),10)))),((_tmpA23[1]=
# 1773
_tmp501?Cyc_PP_text(((_tmpA26="@extensible ",_tag_dyneither(_tmpA26,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmpA23[0]=
# 1772
Cyc_Absynpp_scope2doc(_tmp4FD),Cyc_PP_cat(_tag_dyneither(_tmpA23,sizeof(struct Cyc_PP_Doc*),10)))))))))))))))))))));}}
# 1783
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
enum Cyc_Absyn_Scope _tmp50A;struct _tuple0*_tmp50B;struct Cyc_Core_Opt*_tmp50C;struct Cyc_Absyn_Enumdecl*_tmp509=ed;_tmp50A=_tmp509->sc;_tmp50B=_tmp509->name;_tmp50C=_tmp509->fields;
if(_tmp50C == 0){
const char*_tmpA29;struct Cyc_PP_Doc*_tmpA28[3];return(_tmpA28[2]=
# 1788
Cyc_Absynpp_typedef_name2bolddoc(_tmp50B),((_tmpA28[1]=
# 1787
Cyc_PP_text(((_tmpA29="enum ",_tag_dyneither(_tmpA29,sizeof(char),6)))),((_tmpA28[0]=
# 1786
Cyc_Absynpp_scope2doc(_tmp50A),Cyc_PP_cat(_tag_dyneither(_tmpA28,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
# 1791
const char*_tmpA2E;struct Cyc_PP_Doc*_tmpA2D[2];struct Cyc_PP_Doc*_tmpA2C[8];return(_tmpA2C[7]=
# 1797
Cyc_Absynpp_rb(),((_tmpA2C[6]=
# 1796
Cyc_PP_line_doc(),((_tmpA2C[5]=
# 1795
Cyc_PP_nest(2,((_tmpA2D[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp50C->v),((_tmpA2D[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA2D,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmpA2C[4]=
# 1794
Cyc_Absynpp_lb(),((_tmpA2C[3]=Cyc_PP_blank_doc(),((_tmpA2C[2]=
# 1793
Cyc_Absynpp_qvar2bolddoc(_tmp50B),((_tmpA2C[1]=
# 1792
Cyc_PP_text(((_tmpA2E="enum ",_tag_dyneither(_tmpA2E,sizeof(char),6)))),((_tmpA2C[0]=
# 1791
Cyc_Absynpp_scope2doc(_tmp50A),Cyc_PP_cat(_tag_dyneither(_tmpA2C,sizeof(struct Cyc_PP_Doc*),8)))))))))))))))));}}
# 1800
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp512=d->r;void*_tmp513=_tmp512;struct Cyc_Absyn_Fndecl*_tmp515;struct Cyc_Absyn_Aggrdecl*_tmp517;struct Cyc_Absyn_Vardecl*_tmp519;struct Cyc_Absyn_Tvar*_tmp51B;struct Cyc_Absyn_Vardecl*_tmp51C;int _tmp51D;struct Cyc_Absyn_Exp*_tmp51E;struct Cyc_Absyn_Datatypedecl*_tmp520;struct Cyc_Absyn_Pat*_tmp522;struct Cyc_Absyn_Exp*_tmp523;struct Cyc_List_List*_tmp525;struct Cyc_Absyn_Enumdecl*_tmp527;struct Cyc_Absyn_Typedefdecl*_tmp529;struct _dyneither_ptr*_tmp52B;struct Cyc_List_List*_tmp52C;struct _tuple0*_tmp52E;struct Cyc_List_List*_tmp52F;struct Cyc_List_List*_tmp531;struct Cyc_List_List*_tmp533;struct Cyc_List_List*_tmp534;_LL29E: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp514=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp513;if(_tmp514->tag != 1)goto _LL2A0;else{_tmp515=_tmp514->f1;}}_LL29F: {
# 1804
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA34;struct Cyc_Absyn_FnInfo _tmpA33;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA32;void*t=(void*)((_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32[0]=((_tmpA34.tag=9,((_tmpA34.f1=((_tmpA33.tvars=_tmp515->tvs,((_tmpA33.effect=_tmp515->effect,((_tmpA33.ret_tqual=_tmp515->ret_tqual,((_tmpA33.ret_typ=_tmp515->ret_type,((_tmpA33.args=
# 1808
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp515->args),((_tmpA33.c_varargs=_tmp515->c_varargs,((_tmpA33.cyc_varargs=_tmp515->cyc_varargs,((_tmpA33.rgn_po=_tmp515->rgn_po,((_tmpA33.attributes=0,((_tmpA33.requires_clause=_tmp515->requires_clause,((_tmpA33.requires_relns=_tmp515->requires_relns,((_tmpA33.ensures_clause=_tmp515->ensures_clause,((_tmpA33.ensures_relns=_tmp515->ensures_relns,_tmpA33)))))))))))))))))))))))))),_tmpA34)))),_tmpA32))));
# 1814
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp515->attributes);
struct Cyc_PP_Doc*inlinedoc;
if(_tmp515->is_inline)
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2BC:
{const char*_tmpA35;inlinedoc=Cyc_PP_text(((_tmpA35="inline ",_tag_dyneither(_tmpA35,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL2BD:
{const char*_tmpA36;inlinedoc=Cyc_PP_text(((_tmpA36="__inline ",_tag_dyneither(_tmpA36,sizeof(char),10))));}break;}else{
# 1822
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp515->sc);
struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2BF:
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL2C0:
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp515->attributes);break;}{
# 1829
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp515->name);
struct Cyc_PP_Doc*_tmpA39[2];struct Cyc_Core_Opt*_tmpA38;struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,(
(_tmpA38=_cycalloc(sizeof(*_tmpA38)),((_tmpA38->v=((_tmpA39[1]=namedoc,((_tmpA39[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmpA39,sizeof(struct Cyc_PP_Doc*),2)))))),_tmpA38)))));
struct Cyc_PP_Doc*_tmpA3C[2];struct Cyc_PP_Doc*_tmpA3B[5];struct Cyc_PP_Doc*bodydoc=(_tmpA3B[4]=
# 1835
Cyc_Absynpp_rb(),((_tmpA3B[3]=
# 1834
Cyc_PP_line_doc(),((_tmpA3B[2]=
# 1833
Cyc_PP_nest(2,((_tmpA3C[1]=Cyc_Absynpp_stmt2doc(_tmp515->body,0),((_tmpA3C[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA3C,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmpA3B[1]=
# 1832
Cyc_Absynpp_lb(),((_tmpA3B[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA3B,sizeof(struct Cyc_PP_Doc*),5)))))))))));
# 1836
{struct Cyc_PP_Doc*_tmpA3D[4];s=((_tmpA3D[3]=bodydoc,((_tmpA3D[2]=tqtddoc,((_tmpA3D[1]=scopedoc,((_tmpA3D[0]=inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmpA3D,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1838
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL2C2:
{struct Cyc_PP_Doc*_tmpA3E[2];s=((_tmpA3E[1]=s,((_tmpA3E[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(_tmpA3E,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL2C3:
 break;}
# 1843
goto _LL29D;};};}_LL2A0: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp516=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp513;if(_tmp516->tag != 5)goto _LL2A2;else{_tmp517=_tmp516->f1;}}_LL2A1:
# 1846
{const char*_tmpA41;struct Cyc_PP_Doc*_tmpA40[2];s=((_tmpA40[1]=Cyc_PP_text(((_tmpA41=";",_tag_dyneither(_tmpA41,sizeof(char),2)))),((_tmpA40[0]=Cyc_Absynpp_aggrdecl2doc(_tmp517),Cyc_PP_cat(_tag_dyneither(_tmpA40,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL29D;_LL2A2: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp518=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp513;if(_tmp518->tag != 0)goto _LL2A4;else{_tmp519=_tmp518->f1;}}_LL2A3:
# 1849
 s=Cyc_Absynpp_vardecl2doc(_tmp519);
goto _LL29D;_LL2A4: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp51A=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp513;if(_tmp51A->tag != 4)goto _LL2A6;else{_tmp51B=_tmp51A->f1;_tmp51C=_tmp51A->f2;_tmp51D=_tmp51A->f3;_tmp51E=_tmp51A->f4;}}_LL2A5:
# 1852
{const char*_tmpA54;const char*_tmpA53;const char*_tmpA52;struct Cyc_PP_Doc*_tmpA51[3];const char*_tmpA50;const char*_tmpA4F;const char*_tmpA4E;const char*_tmpA4D;struct Cyc_PP_Doc*_tmpA4C[8];s=((_tmpA4C[7]=
# 1860
Cyc_PP_text(((_tmpA4D=";",_tag_dyneither(_tmpA4D,sizeof(char),2)))),((_tmpA4C[6]=
# 1859
_tmp51D?Cyc_PP_text(((_tmpA4E=" @resetable",_tag_dyneither(_tmpA4E,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmpA4C[5]=
# 1857
(unsigned int)_tmp51E?(_tmpA51[2]=
Cyc_PP_text(((_tmpA50=")",_tag_dyneither(_tmpA50,sizeof(char),2)))),((_tmpA51[1]=
# 1857
Cyc_Absynpp_exp2doc(_tmp51E),((_tmpA51[0]=Cyc_PP_text(((_tmpA4F=" = open(",_tag_dyneither(_tmpA4F,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmpA51,sizeof(struct Cyc_PP_Doc*),3))))))):
 Cyc_PP_nil_doc(),((_tmpA4C[4]=
# 1856
Cyc_Absynpp_qvar2doc(_tmp51C->name),((_tmpA4C[3]=
# 1855
Cyc_PP_text(((_tmpA52=">",_tag_dyneither(_tmpA52,sizeof(char),2)))),((_tmpA4C[2]=
# 1854
Cyc_Absynpp_tvar2doc(_tmp51B),((_tmpA4C[1]=
# 1853
Cyc_PP_text(((_tmpA53="<",_tag_dyneither(_tmpA53,sizeof(char),2)))),((_tmpA4C[0]=
# 1852
Cyc_PP_text(((_tmpA54="region",_tag_dyneither(_tmpA54,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA4C,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1861
goto _LL29D;_LL2A6: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp51F=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp513;if(_tmp51F->tag != 6)goto _LL2A8;else{_tmp520=_tmp51F->f1;}}_LL2A7:
# 1863
{const char*_tmpA57;struct Cyc_PP_Doc*_tmpA56[2];s=((_tmpA56[1]=Cyc_PP_text(((_tmpA57=";",_tag_dyneither(_tmpA57,sizeof(char),2)))),((_tmpA56[0]=Cyc_Absynpp_datatypedecl2doc(_tmp520),Cyc_PP_cat(_tag_dyneither(_tmpA56,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL29D;_LL2A8: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp521=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp513;if(_tmp521->tag != 2)goto _LL2AA;else{_tmp522=_tmp521->f1;_tmp523=_tmp521->f3;}}_LL2A9:
# 1866
{const char*_tmpA5E;const char*_tmpA5D;const char*_tmpA5C;struct Cyc_PP_Doc*_tmpA5B[5];s=((_tmpA5B[4]=
# 1870
Cyc_PP_text(((_tmpA5C=";",_tag_dyneither(_tmpA5C,sizeof(char),2)))),((_tmpA5B[3]=
# 1869
Cyc_Absynpp_exp2doc(_tmp523),((_tmpA5B[2]=
# 1868
Cyc_PP_text(((_tmpA5D=" = ",_tag_dyneither(_tmpA5D,sizeof(char),4)))),((_tmpA5B[1]=
# 1867
Cyc_Absynpp_pat2doc(_tmp522),((_tmpA5B[0]=
# 1866
Cyc_PP_text(((_tmpA5E="let ",_tag_dyneither(_tmpA5E,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA5B,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1871
goto _LL29D;_LL2AA: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp524=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp513;if(_tmp524->tag != 3)goto _LL2AC;else{_tmp525=_tmp524->f1;}}_LL2AB:
# 1873
{const char*_tmpA63;const char*_tmpA62;struct Cyc_PP_Doc*_tmpA61[3];s=((_tmpA61[2]=Cyc_PP_text(((_tmpA62=";",_tag_dyneither(_tmpA62,sizeof(char),2)))),((_tmpA61[1]=Cyc_Absynpp_ids2doc(_tmp525),((_tmpA61[0]=Cyc_PP_text(((_tmpA63="let ",_tag_dyneither(_tmpA63,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA61,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL29D;_LL2AC: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp526=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp513;if(_tmp526->tag != 7)goto _LL2AE;else{_tmp527=_tmp526->f1;}}_LL2AD:
# 1876
{const char*_tmpA66;struct Cyc_PP_Doc*_tmpA65[2];s=((_tmpA65[1]=Cyc_PP_text(((_tmpA66=";",_tag_dyneither(_tmpA66,sizeof(char),2)))),((_tmpA65[0]=Cyc_Absynpp_enumdecl2doc(_tmp527),Cyc_PP_cat(_tag_dyneither(_tmpA65,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL29D;_LL2AE: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp528=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp513;if(_tmp528->tag != 8)goto _LL2B0;else{_tmp529=_tmp528->f1;}}_LL2AF: {
# 1879
void*t;
if(_tmp529->defn != 0)
t=(void*)_check_null(_tmp529->defn);else{
# 1883
t=Cyc_Absyn_new_evar(_tmp529->kind,0);}
{const char*_tmpA70;struct Cyc_Core_Opt*_tmpA6F;struct Cyc_PP_Doc*_tmpA6E[2];const char*_tmpA6D;struct Cyc_PP_Doc*_tmpA6C[4];s=((_tmpA6C[3]=
# 1891
Cyc_PP_text(((_tmpA6D=";",_tag_dyneither(_tmpA6D,sizeof(char),2)))),((_tmpA6C[2]=
# 1890
Cyc_Absynpp_atts2doc(_tmp529->atts),((_tmpA6C[1]=
# 1885
Cyc_Absynpp_tqtd2doc(_tmp529->tq,t,(
# 1887
(_tmpA6F=_cycalloc(sizeof(*_tmpA6F)),((_tmpA6F->v=((_tmpA6E[1]=
Cyc_Absynpp_tvars2doc(_tmp529->tvs),((_tmpA6E[0]=
# 1887
Cyc_Absynpp_typedef_name2bolddoc(_tmp529->name),Cyc_PP_cat(_tag_dyneither(_tmpA6E,sizeof(struct Cyc_PP_Doc*),2)))))),_tmpA6F))))),((_tmpA6C[0]=
# 1884
Cyc_PP_text(((_tmpA70="typedef ",_tag_dyneither(_tmpA70,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmpA6C,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1893
goto _LL29D;}_LL2B0: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp52A=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp513;if(_tmp52A->tag != 9)goto _LL2B2;else{_tmp52B=_tmp52A->f1;_tmp52C=_tmp52A->f2;}}_LL2B1:
# 1895
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp52B);
{const char*_tmpA75;const char*_tmpA74;struct Cyc_PP_Doc*_tmpA73[8];s=((_tmpA73[7]=
# 1902
Cyc_Absynpp_rb(),((_tmpA73[6]=
# 1901
Cyc_PP_line_doc(),((_tmpA73[5]=
# 1900
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA74="",_tag_dyneither(_tmpA74,sizeof(char),1))),_tmp52C),((_tmpA73[4]=
# 1899
Cyc_PP_line_doc(),((_tmpA73[3]=
# 1898
Cyc_Absynpp_lb(),((_tmpA73[2]=Cyc_PP_blank_doc(),((_tmpA73[1]=
# 1897
Cyc_PP_textptr(_tmp52B),((_tmpA73[0]=
# 1896
Cyc_PP_text(((_tmpA75="namespace ",_tag_dyneither(_tmpA75,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmpA73,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1903
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL29D;_LL2B2: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp52D=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp513;if(_tmp52D->tag != 10)goto _LL2B4;else{_tmp52E=_tmp52D->f1;_tmp52F=_tmp52D->f2;}}_LL2B3:
# 1906
 if(Cyc_Absynpp_print_using_stmts){
const char*_tmpA7A;const char*_tmpA79;struct Cyc_PP_Doc*_tmpA78[8];s=((_tmpA78[7]=
# 1913
Cyc_Absynpp_rb(),((_tmpA78[6]=
# 1912
Cyc_PP_line_doc(),((_tmpA78[5]=
# 1911
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA79="",_tag_dyneither(_tmpA79,sizeof(char),1))),_tmp52F),((_tmpA78[4]=
# 1910
Cyc_PP_line_doc(),((_tmpA78[3]=
# 1909
Cyc_Absynpp_lb(),((_tmpA78[2]=Cyc_PP_blank_doc(),((_tmpA78[1]=
# 1908
Cyc_Absynpp_qvar2doc(_tmp52E),((_tmpA78[0]=
# 1907
Cyc_PP_text(((_tmpA7A="using ",_tag_dyneither(_tmpA7A,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA78,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1915
const char*_tmpA85;const char*_tmpA84;const char*_tmpA83;const char*_tmpA82;const char*_tmpA81;struct Cyc_PP_Doc*_tmpA80[11];s=((_tmpA80[10]=
# 1925
Cyc_PP_text(((_tmpA81=" */",_tag_dyneither(_tmpA81,sizeof(char),4)))),((_tmpA80[9]=
# 1924
Cyc_Absynpp_rb(),((_tmpA80[8]=
# 1923
Cyc_PP_text(((_tmpA82="/* ",_tag_dyneither(_tmpA82,sizeof(char),4)))),((_tmpA80[7]=
# 1922
Cyc_PP_line_doc(),((_tmpA80[6]=
# 1921
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA83="",_tag_dyneither(_tmpA83,sizeof(char),1))),_tmp52F),((_tmpA80[5]=
# 1920
Cyc_PP_line_doc(),((_tmpA80[4]=
# 1919
Cyc_PP_text(((_tmpA84=" */",_tag_dyneither(_tmpA84,sizeof(char),4)))),((_tmpA80[3]=
# 1918
Cyc_Absynpp_lb(),((_tmpA80[2]=
# 1917
Cyc_PP_blank_doc(),((_tmpA80[1]=
# 1916
Cyc_Absynpp_qvar2doc(_tmp52E),((_tmpA80[0]=
# 1915
Cyc_PP_text(((_tmpA85="/* using ",_tag_dyneither(_tmpA85,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmpA80,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1926
goto _LL29D;_LL2B4: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp530=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp513;if(_tmp530->tag != 11)goto _LL2B6;else{_tmp531=_tmp530->f1;}}_LL2B5:
# 1928
 if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA8A;const char*_tmpA89;struct Cyc_PP_Doc*_tmpA88[6];s=((_tmpA88[5]=
# 1934
Cyc_Absynpp_rb(),((_tmpA88[4]=
# 1933
Cyc_PP_line_doc(),((_tmpA88[3]=
# 1932
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA89="",_tag_dyneither(_tmpA89,sizeof(char),1))),_tmp531),((_tmpA88[2]=
# 1931
Cyc_PP_line_doc(),((_tmpA88[1]=
# 1930
Cyc_Absynpp_lb(),((_tmpA88[0]=
# 1929
Cyc_PP_text(((_tmpA8A="extern \"C\" ",_tag_dyneither(_tmpA8A,sizeof(char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA88,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}else{
# 1936
const char*_tmpA95;const char*_tmpA94;const char*_tmpA93;const char*_tmpA92;const char*_tmpA91;struct Cyc_PP_Doc*_tmpA90[9];s=((_tmpA90[8]=
# 1944
Cyc_PP_text(((_tmpA91=" */",_tag_dyneither(_tmpA91,sizeof(char),4)))),((_tmpA90[7]=
# 1943
Cyc_Absynpp_rb(),((_tmpA90[6]=
# 1942
Cyc_PP_text(((_tmpA92="/* ",_tag_dyneither(_tmpA92,sizeof(char),4)))),((_tmpA90[5]=
# 1941
Cyc_PP_line_doc(),((_tmpA90[4]=
# 1940
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA93="",_tag_dyneither(_tmpA93,sizeof(char),1))),_tmp531),((_tmpA90[3]=
# 1939
Cyc_PP_line_doc(),((_tmpA90[2]=
# 1938
Cyc_PP_text(((_tmpA94=" */",_tag_dyneither(_tmpA94,sizeof(char),4)))),((_tmpA90[1]=
# 1937
Cyc_Absynpp_lb(),((_tmpA90[0]=
# 1936
Cyc_PP_text(((_tmpA95="/* extern \"C\" ",_tag_dyneither(_tmpA95,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA90,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1945
goto _LL29D;_LL2B6: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp532=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp513;if(_tmp532->tag != 12)goto _LL2B8;else{_tmp533=_tmp532->f1;_tmp534=_tmp532->f2;}}_LL2B7:
# 1947
 if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
if(_tmp534 != 0){
const char*_tmpA9A;const char*_tmpA99;struct Cyc_PP_Doc*_tmpA98[7];exs_doc=((_tmpA98[6]=
# 1952
Cyc_Absynpp_rb(),((_tmpA98[5]=Cyc_PP_line_doc(),((_tmpA98[4]=
# 1951
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,((_tmpA99=",",_tag_dyneither(_tmpA99,sizeof(char),2))),_tmp534),((_tmpA98[3]=Cyc_PP_line_doc(),((_tmpA98[2]=
# 1950
Cyc_Absynpp_lb(),((_tmpA98[1]=Cyc_PP_text(((_tmpA9A=" export ",_tag_dyneither(_tmpA9A,sizeof(char),9)))),((_tmpA98[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA98,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1954
exs_doc=Cyc_Absynpp_rb();}{
const char*_tmpA9F;const char*_tmpA9E;struct Cyc_PP_Doc*_tmpA9D[6];s=((_tmpA9D[5]=exs_doc,((_tmpA9D[4]=
# 1959
Cyc_PP_line_doc(),((_tmpA9D[3]=
# 1958
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA9E="",_tag_dyneither(_tmpA9E,sizeof(char),1))),_tmp533),((_tmpA9D[2]=
# 1957
Cyc_PP_line_doc(),((_tmpA9D[1]=
# 1956
Cyc_Absynpp_lb(),((_tmpA9D[0]=
# 1955
Cyc_PP_text(((_tmpA9F="extern \"C include\" ",_tag_dyneither(_tmpA9F,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA9D,sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{
# 1962
const char*_tmpAAA;const char*_tmpAA9;const char*_tmpAA8;const char*_tmpAA7;const char*_tmpAA6;struct Cyc_PP_Doc*_tmpAA5[9];s=((_tmpAA5[8]=
# 1970
Cyc_PP_text(((_tmpAA6=" */",_tag_dyneither(_tmpAA6,sizeof(char),4)))),((_tmpAA5[7]=
# 1969
Cyc_Absynpp_rb(),((_tmpAA5[6]=
# 1968
Cyc_PP_text(((_tmpAA7="/* ",_tag_dyneither(_tmpAA7,sizeof(char),4)))),((_tmpAA5[5]=
# 1967
Cyc_PP_line_doc(),((_tmpAA5[4]=
# 1966
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpAA8="",_tag_dyneither(_tmpAA8,sizeof(char),1))),_tmp533),((_tmpAA5[3]=
# 1965
Cyc_PP_line_doc(),((_tmpAA5[2]=
# 1964
Cyc_PP_text(((_tmpAA9=" */",_tag_dyneither(_tmpAA9,sizeof(char),4)))),((_tmpAA5[1]=
# 1963
Cyc_Absynpp_lb(),((_tmpAA5[0]=
# 1962
Cyc_PP_text(((_tmpAAA="/* extern \"C include\" ",_tag_dyneither(_tmpAAA,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpAA5,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1971
goto _LL29D;_LL2B8: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp535=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp513;if(_tmp535->tag != 13)goto _LL2BA;}_LL2B9:
# 1973
{const char*_tmpAAD;struct Cyc_PP_Doc*_tmpAAC[2];s=((_tmpAAC[1]=Cyc_Absynpp_lb(),((_tmpAAC[0]=Cyc_PP_text(((_tmpAAD="__cyclone_port_on__;",_tag_dyneither(_tmpAAD,sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpAAC,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL29D;_LL2BA: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp536=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp513;if(_tmp536->tag != 14)goto _LL29D;}_LL2BB:
# 1976
{const char*_tmpAB0;struct Cyc_PP_Doc*_tmpAAF[2];s=((_tmpAAF[1]=Cyc_Absynpp_lb(),((_tmpAAF[0]=Cyc_PP_text(((_tmpAB0="__cyclone_port_off__;",_tag_dyneither(_tmpAB0,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpAAF,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL29D;_LL29D:;}
# 1980
return s;}
# 1983
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
switch(sc){case Cyc_Absyn_Static: _LL2C5: {
const char*_tmpAB1;return Cyc_PP_text(((_tmpAB1="static ",_tag_dyneither(_tmpAB1,sizeof(char),8))));}case Cyc_Absyn_Public: _LL2C6:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL2C7: {
const char*_tmpAB2;return Cyc_PP_text(((_tmpAB2="extern ",_tag_dyneither(_tmpAB2,sizeof(char),8))));}case Cyc_Absyn_ExternC: _LL2C8: {
const char*_tmpAB3;return Cyc_PP_text(((_tmpAB3="extern \"C\" ",_tag_dyneither(_tmpAB3,sizeof(char),12))));}case Cyc_Absyn_Abstract: _LL2C9: {
const char*_tmpAB4;return Cyc_PP_text(((_tmpAB4="abstract ",_tag_dyneither(_tmpAB4,sizeof(char),10))));}case Cyc_Absyn_Register: _LL2CA: {
const char*_tmpAB5;return Cyc_PP_text(((_tmpAB5="register ",_tag_dyneither(_tmpAB5,sizeof(char),10))));}}}
# 1996
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp587=t;struct Cyc_Absyn_Tvar*_tmp589;struct Cyc_List_List*_tmp58B;_LL2CD: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp588=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp587;if(_tmp588->tag != 2)goto _LL2CF;else{_tmp589=_tmp588->f1;}}_LL2CE:
 return Cyc_Tcutil_is_temp_tvar(_tmp589);_LL2CF: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp58A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp587;if(_tmp58A->tag != 24)goto _LL2D1;else{_tmp58B=_tmp58A->f1;}}_LL2D0:
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp58B);_LL2D1:;_LL2D2:
 return 0;_LL2CC:;}
# 2008
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp58C=t;struct Cyc_Absyn_Typedefdecl*_tmp590;void*_tmp591;_LL2D4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp58D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp58C;if(_tmp58D->tag != 12)goto _LL2D6;}_LL2D5:
 return 1;_LL2D6: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp58E=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp58C;if(_tmp58E->tag != 14)goto _LL2D8;}_LL2D7:
 return 1;_LL2D8:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp58F=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp58C;if(_tmp58F->tag != 17)goto _LL2DA;else{_tmp590=_tmp58F->f3;_tmp591=(void*)_tmp58F->f4;}}if(!(_tmp591 != 0))goto _LL2DA;_LL2D9:
# 2015
 return Cyc_Absynpp_is_anon_aggrtype(_tmp591);_LL2DA:;_LL2DB:
 return 0;_LL2D3:;}
# 2022
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 2025
if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmpAB6;struct _tuple16 _tmp592=(_tmpAB6.f1=(void*)tms->hd,((_tmpAB6.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpAB6)));struct _tuple16 _tmp593=_tmp592;_LL2DD:{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp594=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp593.f1;if(_tmp594->tag != 2)goto _LL2DF;}{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp595=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp593.f2;if(_tmp595->tag != 3)goto _LL2DF;};_LL2DE: {
# 2028
struct Cyc_List_List*_tmpAB9;struct Cyc_List_List*_tmpAB8;return(_tmpAB8=_region_malloc(r,sizeof(*_tmpAB8)),((_tmpAB8->hd=(void*)tms->hd,((_tmpAB8->tl=((_tmpAB9=_region_malloc(r,sizeof(*_tmpAB9)),((_tmpAB9->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,((_tmpAB9->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpAB9)))))),_tmpAB8)))));}_LL2DF:;_LL2E0: {
struct Cyc_List_List*_tmpABA;return(_tmpABA=_region_malloc(r,sizeof(*_tmpABA)),((_tmpABA->hd=atts,((_tmpABA->tl=tms,_tmpABA)))));}_LL2DC:;}else{
# 2031
struct Cyc_List_List*_tmpABB;return(_tmpABB=_region_malloc(r,sizeof(*_tmpABB)),((_tmpABB->hd=atts,((_tmpABB->tl=tms,_tmpABB)))));}}
# 2036
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2038
void*_tmp59B=t;void*_tmp59D;struct Cyc_Absyn_Tqual _tmp59E;struct Cyc_Absyn_Exp*_tmp59F;union Cyc_Absyn_Constraint*_tmp5A0;unsigned int _tmp5A1;void*_tmp5A3;struct Cyc_Absyn_Tqual _tmp5A4;struct Cyc_Absyn_PtrAtts _tmp5A5;struct Cyc_List_List*_tmp5A7;void*_tmp5A8;struct Cyc_Absyn_Tqual _tmp5A9;void*_tmp5AA;struct Cyc_List_List*_tmp5AB;int _tmp5AC;struct Cyc_Absyn_VarargInfo*_tmp5AD;struct Cyc_List_List*_tmp5AE;struct Cyc_List_List*_tmp5AF;struct Cyc_Absyn_Exp*_tmp5B0;struct Cyc_Absyn_Exp*_tmp5B1;struct Cyc_Core_Opt*_tmp5B3;void*_tmp5B4;int _tmp5B5;struct _tuple0*_tmp5B7;struct Cyc_List_List*_tmp5B8;struct Cyc_Absyn_Typedefdecl*_tmp5B9;void*_tmp5BA;_LL2E2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp59C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59B;if(_tmp59C->tag != 8)goto _LL2E4;else{_tmp59D=(_tmp59C->f1).elt_type;_tmp59E=(_tmp59C->f1).tq;_tmp59F=(_tmp59C->f1).num_elts;_tmp5A0=(_tmp59C->f1).zero_term;_tmp5A1=(_tmp59C->f1).zt_loc;}}_LL2E3: {
# 2041
struct _tuple12 _tmp5BB=Cyc_Absynpp_to_tms(r,_tmp59E,_tmp59D);struct Cyc_Absyn_Tqual _tmp5BD;void*_tmp5BE;struct Cyc_List_List*_tmp5BF;struct _tuple12 _tmp5BC=_tmp5BB;_tmp5BD=_tmp5BC.f1;_tmp5BE=_tmp5BC.f2;_tmp5BF=_tmp5BC.f3;{
void*tm;
if(_tmp59F == 0){
struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpABE;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpABD;tm=(void*)((_tmpABD=_region_malloc(r,sizeof(*_tmpABD)),((_tmpABD[0]=((_tmpABE.tag=0,((_tmpABE.f1=_tmp5A0,((_tmpABE.f2=_tmp5A1,_tmpABE)))))),_tmpABD))));}else{
# 2046
struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpAC1;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpAC0;tm=(void*)((_tmpAC0=_region_malloc(r,sizeof(*_tmpAC0)),((_tmpAC0[0]=((_tmpAC1.tag=1,((_tmpAC1.f1=_tmp59F,((_tmpAC1.f2=_tmp5A0,((_tmpAC1.f3=_tmp5A1,_tmpAC1)))))))),_tmpAC0))));}{
struct Cyc_List_List*_tmpAC4;struct _tuple12 _tmpAC3;return(_tmpAC3.f1=_tmp5BD,((_tmpAC3.f2=_tmp5BE,((_tmpAC3.f3=((_tmpAC4=_region_malloc(r,sizeof(*_tmpAC4)),((_tmpAC4->hd=tm,((_tmpAC4->tl=_tmp5BF,_tmpAC4)))))),_tmpAC3)))));};};}_LL2E4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59B;if(_tmp5A2->tag != 5)goto _LL2E6;else{_tmp5A3=(_tmp5A2->f1).elt_typ;_tmp5A4=(_tmp5A2->f1).elt_tq;_tmp5A5=(_tmp5A2->f1).ptr_atts;}}_LL2E5: {
# 2050
struct _tuple12 _tmp5C6=Cyc_Absynpp_to_tms(r,_tmp5A4,_tmp5A3);struct Cyc_Absyn_Tqual _tmp5C8;void*_tmp5C9;struct Cyc_List_List*_tmp5CA;struct _tuple12 _tmp5C7=_tmp5C6;_tmp5C8=_tmp5C7.f1;_tmp5C9=_tmp5C7.f2;_tmp5CA=_tmp5C7.f3;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpACA;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpAC9;struct Cyc_List_List*_tmpAC8;_tmp5CA=((_tmpAC8=_region_malloc(r,sizeof(*_tmpAC8)),((_tmpAC8->hd=(void*)((_tmpACA=_region_malloc(r,sizeof(*_tmpACA)),((_tmpACA[0]=((_tmpAC9.tag=2,((_tmpAC9.f1=_tmp5A5,((_tmpAC9.f2=tq,_tmpAC9)))))),_tmpACA)))),((_tmpAC8->tl=_tmp5CA,_tmpAC8))))));}{
struct _tuple12 _tmpACB;return(_tmpACB.f1=_tmp5C8,((_tmpACB.f2=_tmp5C9,((_tmpACB.f3=_tmp5CA,_tmpACB)))));};}_LL2E6: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5A6=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59B;if(_tmp5A6->tag != 9)goto _LL2E8;else{_tmp5A7=(_tmp5A6->f1).tvars;_tmp5A8=(_tmp5A6->f1).effect;_tmp5A9=(_tmp5A6->f1).ret_tqual;_tmp5AA=(_tmp5A6->f1).ret_typ;_tmp5AB=(_tmp5A6->f1).args;_tmp5AC=(_tmp5A6->f1).c_varargs;_tmp5AD=(_tmp5A6->f1).cyc_varargs;_tmp5AE=(_tmp5A6->f1).rgn_po;_tmp5AF=(_tmp5A6->f1).attributes;_tmp5B0=(_tmp5A6->f1).requires_clause;_tmp5B1=(_tmp5A6->f1).ensures_clause;}}_LL2E7:
# 2056
 if(!Cyc_Absynpp_print_all_tvars){
# 2060
if(_tmp5A8 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect(_tmp5A8)){
_tmp5A8=0;
_tmp5A7=0;}}else{
# 2065
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2068
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp5A7);}{
# 2073
struct _tuple12 _tmp5CF=Cyc_Absynpp_to_tms(r,_tmp5A9,_tmp5AA);struct Cyc_Absyn_Tqual _tmp5D1;void*_tmp5D2;struct Cyc_List_List*_tmp5D3;struct _tuple12 _tmp5D0=_tmp5CF;_tmp5D1=_tmp5D0.f1;_tmp5D2=_tmp5D0.f2;_tmp5D3=_tmp5D0.f3;{
struct Cyc_List_List*tms=_tmp5D3;
# 2084 "absynpp.cyc"
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2EE:
# 2086
 if(_tmp5AF != 0){
# 2089
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpACE;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpACD;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpACD=_region_malloc(r,sizeof(*_tmpACD)),((_tmpACD[0]=((_tmpACE.tag=5,((_tmpACE.f1=0,((_tmpACE.f2=_tmp5AF,_tmpACE)))))),_tmpACD)))),tms);}
# 2091
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpADD;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpADC;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpADB;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpADA;struct Cyc_List_List*_tmpAD9;tms=((_tmpAD9=_region_malloc(r,sizeof(*_tmpAD9)),((_tmpAD9->hd=(void*)((_tmpADD=_region_malloc(r,sizeof(*_tmpADD)),((_tmpADD[0]=(
(_tmpADA.tag=3,((_tmpADA.f1=(void*)((_tmpADC=_region_malloc(r,sizeof(*_tmpADC)),((_tmpADC[0]=((_tmpADB.tag=1,((_tmpADB.f1=_tmp5AB,((_tmpADB.f2=_tmp5AC,((_tmpADB.f3=_tmp5AD,((_tmpADB.f4=_tmp5A8,((_tmpADB.f5=_tmp5AE,((_tmpADB.f6=_tmp5B0,((_tmpADB.f7=_tmp5B1,_tmpADB)))))))))))))))),_tmpADC)))),_tmpADA)))),_tmpADD)))),((_tmpAD9->tl=tms,_tmpAD9))))));}
# 2095
break;case Cyc_Cyclone_Vc_c: _LL2EF:
# 2097
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpAEC;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpAEB;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpAEA;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpAE9;struct Cyc_List_List*_tmpAE8;tms=((_tmpAE8=_region_malloc(r,sizeof(*_tmpAE8)),((_tmpAE8->hd=(void*)((_tmpAEC=_region_malloc(r,sizeof(*_tmpAEC)),((_tmpAEC[0]=(
(_tmpAE9.tag=3,((_tmpAE9.f1=(void*)((_tmpAEB=_region_malloc(r,sizeof(*_tmpAEB)),((_tmpAEB[0]=((_tmpAEA.tag=1,((_tmpAEA.f1=_tmp5AB,((_tmpAEA.f2=_tmp5AC,((_tmpAEA.f3=_tmp5AD,((_tmpAEA.f4=_tmp5A8,((_tmpAEA.f5=_tmp5AE,((_tmpAEA.f6=_tmp5B0,((_tmpAEA.f7=_tmp5B1,_tmpAEA)))))))))))))))),_tmpAEB)))),_tmpAE9)))),_tmpAEC)))),((_tmpAE8->tl=tms,_tmpAE8))))));}
# 2101
for(0;_tmp5AF != 0;_tmp5AF=_tmp5AF->tl){
void*_tmp5E0=(void*)_tmp5AF->hd;void*_tmp5E1=_tmp5E0;_LL2F2: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp5E2=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp5E1;if(_tmp5E2->tag != 1)goto _LL2F4;}_LL2F3:
 goto _LL2F5;_LL2F4: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp5E3=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp5E1;if(_tmp5E3->tag != 2)goto _LL2F6;}_LL2F5:
 goto _LL2F7;_LL2F6: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp5E4=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp5E1;if(_tmp5E4->tag != 3)goto _LL2F8;}_LL2F7:
# 2106
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpAF6;struct Cyc_List_List*_tmpAF5;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpAF4;struct Cyc_List_List*_tmpAF3;tms=((_tmpAF3=_region_malloc(r,sizeof(*_tmpAF3)),((_tmpAF3->hd=(void*)((_tmpAF6=_region_malloc(r,sizeof(*_tmpAF6)),((_tmpAF6[0]=((_tmpAF4.tag=5,((_tmpAF4.f1=0,((_tmpAF4.f2=((_tmpAF5=_cycalloc(sizeof(*_tmpAF5)),((_tmpAF5->hd=(void*)_tmp5AF->hd,((_tmpAF5->tl=0,_tmpAF5)))))),_tmpAF4)))))),_tmpAF6)))),((_tmpAF3->tl=tms,_tmpAF3))))));}
goto AfterAtts;_LL2F8:;_LL2F9:
 goto _LL2F1;_LL2F1:;}
# 2110
break;}
# 2113
AfterAtts:
 if(_tmp5A7 != 0){
struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpAFC;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpAFB;struct Cyc_List_List*_tmpAFA;tms=((_tmpAFA=_region_malloc(r,sizeof(*_tmpAFA)),((_tmpAFA->hd=(void*)((_tmpAFC=_region_malloc(r,sizeof(*_tmpAFC)),((_tmpAFC[0]=((_tmpAFB.tag=4,((_tmpAFB.f1=_tmp5A7,((_tmpAFB.f2=0,((_tmpAFB.f3=1,_tmpAFB)))))))),_tmpAFC)))),((_tmpAFA->tl=tms,_tmpAFA))))));}{
struct _tuple12 _tmpAFD;return(_tmpAFD.f1=_tmp5D1,((_tmpAFD.f2=_tmp5D2,((_tmpAFD.f3=tms,_tmpAFD)))));};};};_LL2E8: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5B2=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp59B;if(_tmp5B2->tag != 1)goto _LL2EA;else{_tmp5B3=_tmp5B2->f1;_tmp5B4=(void*)_tmp5B2->f2;_tmp5B5=_tmp5B2->f3;}}_LL2E9:
# 2119
 if(_tmp5B4 == 0){
struct _tuple12 _tmpAFE;return(_tmpAFE.f1=tq,((_tmpAFE.f2=t,((_tmpAFE.f3=0,_tmpAFE)))));}else{
# 2122
return Cyc_Absynpp_to_tms(r,tq,_tmp5B4);}_LL2EA: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5B6=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp59B;if(_tmp5B6->tag != 17)goto _LL2EC;else{_tmp5B7=_tmp5B6->f1;_tmp5B8=_tmp5B6->f2;_tmp5B9=_tmp5B6->f3;_tmp5BA=(void*)_tmp5B6->f4;}}_LL2EB:
# 2127
 if((_tmp5BA == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(_tmp5BA)){
struct _tuple12 _tmpAFF;return(_tmpAFF.f1=tq,((_tmpAFF.f2=t,((_tmpAFF.f3=0,_tmpAFF)))));}else{
# 2130
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,_tmp5BA);}_LL2EC:;_LL2ED: {
# 2135
struct _tuple12 _tmpB00;return(_tmpB00.f1=tq,((_tmpB00.f2=t,((_tmpB00.f3=0,_tmpB00)))));}_LL2E1:;}
# 2139
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2141
void*_tmp5F0=t;void*_tmp5F2;void*_tmp5F4;_LL2FB:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5F1=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5F0;if(_tmp5F1->tag != 1)goto _LL2FD;else{_tmp5F2=(void*)_tmp5F1->f2;}}if(!(_tmp5F2 != 0))goto _LL2FD;_LL2FC:
 return Cyc_Absynpp_is_char_ptr(_tmp5F2);_LL2FD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F0;if(_tmp5F3->tag != 5)goto _LL2FF;else{_tmp5F4=(_tmp5F3->f1).elt_typ;}}_LL2FE:
# 2144
 L: {
void*_tmp5F5=_tmp5F4;void*_tmp5F7;void*_tmp5F9;_LL302:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5F6=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5F5;if(_tmp5F6->tag != 1)goto _LL304;else{_tmp5F7=(void*)_tmp5F6->f2;}}if(!(_tmp5F7 != 0))goto _LL304;_LL303:
 _tmp5F4=_tmp5F7;goto L;_LL304:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5F8=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5F5;if(_tmp5F8->tag != 17)goto _LL306;else{_tmp5F9=(void*)_tmp5F8->f4;}}if(!(_tmp5F9 != 0))goto _LL306;_LL305:
 _tmp5F4=_tmp5F9;goto L;_LL306: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5FA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F5;if(_tmp5FA->tag != 6)goto _LL308;else{if(_tmp5FA->f2 != Cyc_Absyn_Char_sz)goto _LL308;}}_LL307:
 return 1;_LL308:;_LL309:
 return 0;_LL301:;}_LL2FF:;_LL300:
# 2151
 return 0;_LL2FA:;}
# 2155
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp5FB=_new_region("temp");struct _RegionHandle*temp=& _tmp5FB;_push_region(temp);
{struct _tuple12 _tmp5FC=Cyc_Absynpp_to_tms(temp,tq,typ);struct Cyc_Absyn_Tqual _tmp5FE;void*_tmp5FF;struct Cyc_List_List*_tmp600;struct _tuple12 _tmp5FD=_tmp5FC;_tmp5FE=_tmp5FD.f1;_tmp5FF=_tmp5FD.f2;_tmp600=_tmp5FD.f3;
_tmp600=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp600);
if(_tmp600 == 0  && dopt == 0){
struct Cyc_PP_Doc*_tmpB01[2];struct Cyc_PP_Doc*_tmp602=(_tmpB01[1]=Cyc_Absynpp_ntyp2doc(_tmp5FF),((_tmpB01[0]=Cyc_Absynpp_tqual2doc(_tmp5FE),Cyc_PP_cat(_tag_dyneither(_tmpB01,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return _tmp602;}else{
# 2162
const char*_tmpB04;struct Cyc_PP_Doc*_tmpB03[4];struct Cyc_PP_Doc*_tmp605=
(_tmpB03[3]=
# 2166
Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp600),((_tmpB03[2]=
# 2165
Cyc_PP_text(((_tmpB04=" ",_tag_dyneither(_tmpB04,sizeof(char),2)))),((_tmpB03[1]=
# 2164
Cyc_Absynpp_ntyp2doc(_tmp5FF),((_tmpB03[0]=
# 2163
Cyc_Absynpp_tqual2doc(_tmp5FE),Cyc_PP_cat(_tag_dyneither(_tmpB03,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp605;}}
# 2157
;_pop_region(temp);}
# 2170
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp606=f->requires_clause;
if((unsigned int)_tmp606){
const char*_tmpB07;struct Cyc_PP_Doc*_tmpB06[2];requires_doc=((_tmpB06[1]=Cyc_Absynpp_exp2doc(_tmp606),((_tmpB06[0]=Cyc_PP_text(((_tmpB07="@requires ",_tag_dyneither(_tmpB07,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmpB06,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 2176
requires_doc=Cyc_PP_nil_doc();}
# 2178
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL30A:
# 2181
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpB0E;const char*_tmpB0D;const char*_tmpB0C;struct Cyc_PP_Doc*_tmpB0B[5];return(_tmpB0B[4]=
# 2184
Cyc_PP_text(((_tmpB0C=";",_tag_dyneither(_tmpB0C,sizeof(char),2)))),((_tmpB0B[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpB0B[2]=
# 2183
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpB0B[1]=Cyc_PP_text(((_tmpB0D=":",_tag_dyneither(_tmpB0D,sizeof(char),2)))),((_tmpB0B[0]=
# 2182
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpB0E=_cycalloc(sizeof(*_tmpB0E)),((_tmpB0E->v=Cyc_PP_textptr(f->name),_tmpB0E))))),Cyc_PP_cat(_tag_dyneither(_tmpB0B,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2186
struct Cyc_Core_Opt*_tmpB13;const char*_tmpB12;struct Cyc_PP_Doc*_tmpB11[4];return(_tmpB11[3]=
Cyc_PP_text(((_tmpB12=";",_tag_dyneither(_tmpB12,sizeof(char),2)))),((_tmpB11[2]=requires_doc,((_tmpB11[1]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpB11[0]=
# 2186
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpB13=_cycalloc(sizeof(*_tmpB13)),((_tmpB13->v=Cyc_PP_textptr(f->name),_tmpB13))))),Cyc_PP_cat(_tag_dyneither(_tmpB11,sizeof(struct Cyc_PP_Doc*),4)))))))));}case Cyc_Cyclone_Vc_c: _LL30B:
# 2189
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpB1A;const char*_tmpB19;const char*_tmpB18;struct Cyc_PP_Doc*_tmpB17[5];return(_tmpB17[4]=
# 2192
Cyc_PP_text(((_tmpB18=";",_tag_dyneither(_tmpB18,sizeof(char),2)))),((_tmpB17[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpB17[2]=Cyc_PP_text(((_tmpB19=":",_tag_dyneither(_tmpB19,sizeof(char),2)))),((_tmpB17[1]=
# 2191
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpB1A=_cycalloc(sizeof(*_tmpB1A)),((_tmpB1A->v=Cyc_PP_textptr(f->name),_tmpB1A))))),((_tmpB17[0]=
# 2190
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpB17,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2194
struct Cyc_Core_Opt*_tmpB1F;const char*_tmpB1E;struct Cyc_PP_Doc*_tmpB1D[4];return(_tmpB1D[3]=
# 2196
Cyc_PP_text(((_tmpB1E=";",_tag_dyneither(_tmpB1E,sizeof(char),2)))),((_tmpB1D[2]=requires_doc,((_tmpB1D[1]=
# 2195
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpB1F=_cycalloc(sizeof(*_tmpB1F)),((_tmpB1F->v=Cyc_PP_textptr(f->name),_tmpB1F))))),((_tmpB1D[0]=
# 2194
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpB1D,sizeof(struct Cyc_PP_Doc*),4)))))))));}}}
# 2201
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
const char*_tmpB20;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmpB20="",_tag_dyneither(_tmpB20,sizeof(char),1))),fields);}
# 2205
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
struct Cyc_PP_Doc*_tmpB21[3];return(_tmpB21[2]=
f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmpB21[1]=
# 2206
Cyc_Absynpp_typedef_name2doc(f->name),((_tmpB21[0]=Cyc_Absynpp_scope2doc(f->sc),Cyc_PP_cat(_tag_dyneither(_tmpB21,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 2210
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
const char*_tmpB22;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmpB22=",",_tag_dyneither(_tmpB22,sizeof(char),2))),fields);}
# 2219
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{
const char*_tmpB25;void*_tmpB24;(_tmpB24=0,Cyc_fprintf(f,((_tmpB25="\n",_tag_dyneither(_tmpB25,sizeof(char),2))),_tag_dyneither(_tmpB24,sizeof(void*),0)));};}}
# 2226
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
const char*_tmpB26;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpB26="",_tag_dyneither(_tmpB26,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),72);}
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
