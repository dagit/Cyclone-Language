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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 460 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 463
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 527
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 553
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 560
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 578
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 742 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 946
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 956
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 964
void*Cyc_Absyn_compress_kb(void*);
# 969
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 999
extern void*Cyc_Absyn_bounds_one;
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1081
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1186
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1192
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
struct Cyc_List_List*_tmp616;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((_tmp616=_cycalloc(sizeof(*_tmp616)),((_tmp616->hd=v,((_tmp616->tl=0,_tmp616)))))));}
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
const char*_tmp617;return(_tmp617="\\a",_tag_dyneither(_tmp617,sizeof(char),3));}case '\b': _LL1: {
const char*_tmp618;return(_tmp618="\\b",_tag_dyneither(_tmp618,sizeof(char),3));}case '\f': _LL2: {
const char*_tmp619;return(_tmp619="\\f",_tag_dyneither(_tmp619,sizeof(char),3));}case '\n': _LL3: {
const char*_tmp61A;return(_tmp61A="\\n",_tag_dyneither(_tmp61A,sizeof(char),3));}case '\r': _LL4: {
const char*_tmp61B;return(_tmp61B="\\r",_tag_dyneither(_tmp61B,sizeof(char),3));}case '\t': _LL5: {
const char*_tmp61C;return(_tmp61C="\\t",_tag_dyneither(_tmp61C,sizeof(char),3));}case '\v': _LL6: {
const char*_tmp61D;return(_tmp61D="\\v",_tag_dyneither(_tmp61D,sizeof(char),3));}case '\\': _LL7: {
const char*_tmp61E;return(_tmp61E="\\\\",_tag_dyneither(_tmp61E,sizeof(char),3));}case '"': _LL8: {
const char*_tmp61F;return(_tmp61F="\"",_tag_dyneither(_tmp61F,sizeof(char),2));}case '\'': _LL9: {
const char*_tmp620;return(_tmp620="\\'",_tag_dyneither(_tmp620,sizeof(char),3));}default: _LLA:
# 277
 if(c >= ' '  && c <= '~'){
struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);
{char _tmp623;char _tmp622;struct _dyneither_ptr _tmp621;(_tmp621=_dyneither_ptr_plus(_tmpC,sizeof(char),0),((_tmp622=*((char*)_check_dyneither_subscript(_tmp621,sizeof(char),0)),((_tmp623=c,((_get_dyneither_size(_tmp621,sizeof(char))== 1  && (_tmp622 == '\000'  && _tmp623 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp621.curr)=_tmp623)))))));}
return(struct _dyneither_ptr)_tmpC;}else{
# 282
struct _dyneither_ptr _tmp10=Cyc_Core_new_string(5);
int j=0;
{char _tmp626;char _tmp625;struct _dyneither_ptr _tmp624;(_tmp624=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp625=*((char*)_check_dyneither_subscript(_tmp624,sizeof(char),0)),((_tmp626='\\',((_get_dyneither_size(_tmp624,sizeof(char))== 1  && (_tmp625 == '\000'  && _tmp626 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp624.curr)=_tmp626)))))));}
{char _tmp629;char _tmp628;struct _dyneither_ptr _tmp627;(_tmp627=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp628=*((char*)_check_dyneither_subscript(_tmp627,sizeof(char),0)),((_tmp629=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp627,sizeof(char))== 1  && (_tmp628 == '\000'  && _tmp629 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp627.curr)=_tmp629)))))));}
{char _tmp62C;char _tmp62B;struct _dyneither_ptr _tmp62A;(_tmp62A=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp62B=*((char*)_check_dyneither_subscript(_tmp62A,sizeof(char),0)),((_tmp62C=(char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp62A,sizeof(char))== 1  && (_tmp62B == '\000'  && _tmp62C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62A.curr)=_tmp62C)))))));}
{char _tmp62F;char _tmp62E;struct _dyneither_ptr _tmp62D;(_tmp62D=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp62E=*((char*)_check_dyneither_subscript(_tmp62D,sizeof(char),0)),((_tmp62F=(char)('0' + (c & 7)),((_get_dyneither_size(_tmp62D,sizeof(char))== 1  && (_tmp62E == '\000'  && _tmp62F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62D.curr)=_tmp62F)))))));}
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
{char _tmp632;char _tmp631;struct _dyneither_ptr _tmp630;(_tmp630=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp631=*((char*)_check_dyneither_subscript(_tmp630,sizeof(char),0)),((_tmp632='\\',((_get_dyneither_size(_tmp630,sizeof(char))== 1  && (_tmp631 == '\000'  && _tmp632 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp630.curr)=_tmp632)))))));}{char _tmp635;char _tmp634;struct _dyneither_ptr _tmp633;(_tmp633=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp634=*((char*)_check_dyneither_subscript(_tmp633,sizeof(char),0)),((_tmp635='a',((_get_dyneither_size(_tmp633,sizeof(char))== 1  && (_tmp634 == '\000'  && _tmp635 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp633.curr)=_tmp635)))))));}goto _LL21;_LL24: if(_tmp21 != '\b')goto _LL26;_LL25:
{char _tmp638;char _tmp637;struct _dyneither_ptr _tmp636;(_tmp636=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp637=*((char*)_check_dyneither_subscript(_tmp636,sizeof(char),0)),((_tmp638='\\',((_get_dyneither_size(_tmp636,sizeof(char))== 1  && (_tmp637 == '\000'  && _tmp638 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp636.curr)=_tmp638)))))));}{char _tmp63B;char _tmp63A;struct _dyneither_ptr _tmp639;(_tmp639=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp63A=*((char*)_check_dyneither_subscript(_tmp639,sizeof(char),0)),((_tmp63B='b',((_get_dyneither_size(_tmp639,sizeof(char))== 1  && (_tmp63A == '\000'  && _tmp63B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp639.curr)=_tmp63B)))))));}goto _LL21;_LL26: if(_tmp21 != '\f')goto _LL28;_LL27:
{char _tmp63E;char _tmp63D;struct _dyneither_ptr _tmp63C;(_tmp63C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp63D=*((char*)_check_dyneither_subscript(_tmp63C,sizeof(char),0)),((_tmp63E='\\',((_get_dyneither_size(_tmp63C,sizeof(char))== 1  && (_tmp63D == '\000'  && _tmp63E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp63C.curr)=_tmp63E)))))));}{char _tmp641;char _tmp640;struct _dyneither_ptr _tmp63F;(_tmp63F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp640=*((char*)_check_dyneither_subscript(_tmp63F,sizeof(char),0)),((_tmp641='f',((_get_dyneither_size(_tmp63F,sizeof(char))== 1  && (_tmp640 == '\000'  && _tmp641 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp63F.curr)=_tmp641)))))));}goto _LL21;_LL28: if(_tmp21 != '\n')goto _LL2A;_LL29:
{char _tmp644;char _tmp643;struct _dyneither_ptr _tmp642;(_tmp642=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp643=*((char*)_check_dyneither_subscript(_tmp642,sizeof(char),0)),((_tmp644='\\',((_get_dyneither_size(_tmp642,sizeof(char))== 1  && (_tmp643 == '\000'  && _tmp644 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp642.curr)=_tmp644)))))));}{char _tmp647;char _tmp646;struct _dyneither_ptr _tmp645;(_tmp645=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp646=*((char*)_check_dyneither_subscript(_tmp645,sizeof(char),0)),((_tmp647='n',((_get_dyneither_size(_tmp645,sizeof(char))== 1  && (_tmp646 == '\000'  && _tmp647 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp645.curr)=_tmp647)))))));}goto _LL21;_LL2A: if(_tmp21 != '\r')goto _LL2C;_LL2B:
{char _tmp64A;char _tmp649;struct _dyneither_ptr _tmp648;(_tmp648=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp649=*((char*)_check_dyneither_subscript(_tmp648,sizeof(char),0)),((_tmp64A='\\',((_get_dyneither_size(_tmp648,sizeof(char))== 1  && (_tmp649 == '\000'  && _tmp64A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp648.curr)=_tmp64A)))))));}{char _tmp64D;char _tmp64C;struct _dyneither_ptr _tmp64B;(_tmp64B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp64C=*((char*)_check_dyneither_subscript(_tmp64B,sizeof(char),0)),((_tmp64D='r',((_get_dyneither_size(_tmp64B,sizeof(char))== 1  && (_tmp64C == '\000'  && _tmp64D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp64B.curr)=_tmp64D)))))));}goto _LL21;_LL2C: if(_tmp21 != '\t')goto _LL2E;_LL2D:
{char _tmp650;char _tmp64F;struct _dyneither_ptr _tmp64E;(_tmp64E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp64F=*((char*)_check_dyneither_subscript(_tmp64E,sizeof(char),0)),((_tmp650='\\',((_get_dyneither_size(_tmp64E,sizeof(char))== 1  && (_tmp64F == '\000'  && _tmp650 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp64E.curr)=_tmp650)))))));}{char _tmp653;char _tmp652;struct _dyneither_ptr _tmp651;(_tmp651=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp652=*((char*)_check_dyneither_subscript(_tmp651,sizeof(char),0)),((_tmp653='t',((_get_dyneither_size(_tmp651,sizeof(char))== 1  && (_tmp652 == '\000'  && _tmp653 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp651.curr)=_tmp653)))))));}goto _LL21;_LL2E: if(_tmp21 != '\v')goto _LL30;_LL2F:
{char _tmp656;char _tmp655;struct _dyneither_ptr _tmp654;(_tmp654=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp655=*((char*)_check_dyneither_subscript(_tmp654,sizeof(char),0)),((_tmp656='\\',((_get_dyneither_size(_tmp654,sizeof(char))== 1  && (_tmp655 == '\000'  && _tmp656 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp654.curr)=_tmp656)))))));}{char _tmp659;char _tmp658;struct _dyneither_ptr _tmp657;(_tmp657=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp658=*((char*)_check_dyneither_subscript(_tmp657,sizeof(char),0)),((_tmp659='v',((_get_dyneither_size(_tmp657,sizeof(char))== 1  && (_tmp658 == '\000'  && _tmp659 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp657.curr)=_tmp659)))))));}goto _LL21;_LL30: if(_tmp21 != '\\')goto _LL32;_LL31:
{char _tmp65C;char _tmp65B;struct _dyneither_ptr _tmp65A;(_tmp65A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp65B=*((char*)_check_dyneither_subscript(_tmp65A,sizeof(char),0)),((_tmp65C='\\',((_get_dyneither_size(_tmp65A,sizeof(char))== 1  && (_tmp65B == '\000'  && _tmp65C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp65A.curr)=_tmp65C)))))));}{char _tmp65F;char _tmp65E;struct _dyneither_ptr _tmp65D;(_tmp65D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp65E=*((char*)_check_dyneither_subscript(_tmp65D,sizeof(char),0)),((_tmp65F='\\',((_get_dyneither_size(_tmp65D,sizeof(char))== 1  && (_tmp65E == '\000'  && _tmp65F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp65D.curr)=_tmp65F)))))));}goto _LL21;_LL32: if(_tmp21 != '"')goto _LL34;_LL33:
{char _tmp662;char _tmp661;struct _dyneither_ptr _tmp660;(_tmp660=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp661=*((char*)_check_dyneither_subscript(_tmp660,sizeof(char),0)),((_tmp662='\\',((_get_dyneither_size(_tmp660,sizeof(char))== 1  && (_tmp661 == '\000'  && _tmp662 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp660.curr)=_tmp662)))))));}{char _tmp665;char _tmp664;struct _dyneither_ptr _tmp663;(_tmp663=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp664=*((char*)_check_dyneither_subscript(_tmp663,sizeof(char),0)),((_tmp665='"',((_get_dyneither_size(_tmp663,sizeof(char))== 1  && (_tmp664 == '\000'  && _tmp665 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp663.curr)=_tmp665)))))));}goto _LL21;_LL34: _tmp22=_tmp21;_LL35:
# 342
 if(_tmp22 >= ' '  && _tmp22 <= '~'){char _tmp668;char _tmp667;struct _dyneither_ptr _tmp666;(_tmp666=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp667=*((char*)_check_dyneither_subscript(_tmp666,sizeof(char),0)),((_tmp668=_tmp22,((_get_dyneither_size(_tmp666,sizeof(char))== 1  && (_tmp667 == '\000'  && _tmp668 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp666.curr)=_tmp668)))))));}else{
# 344
{char _tmp66B;char _tmp66A;struct _dyneither_ptr _tmp669;(_tmp669=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp66A=*((char*)_check_dyneither_subscript(_tmp669,sizeof(char),0)),((_tmp66B='\\',((_get_dyneither_size(_tmp669,sizeof(char))== 1  && (_tmp66A == '\000'  && _tmp66B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp669.curr)=_tmp66B)))))));}
{char _tmp66E;char _tmp66D;struct _dyneither_ptr _tmp66C;(_tmp66C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp66D=*((char*)_check_dyneither_subscript(_tmp66C,sizeof(char),0)),((_tmp66E=(char)('0' + (_tmp22 >> 6 & 7)),((_get_dyneither_size(_tmp66C,sizeof(char))== 1  && (_tmp66D == '\000'  && _tmp66E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp66C.curr)=_tmp66E)))))));}
{char _tmp671;char _tmp670;struct _dyneither_ptr _tmp66F;(_tmp66F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp670=*((char*)_check_dyneither_subscript(_tmp66F,sizeof(char),0)),((_tmp671=(char)('0' + (_tmp22 >> 3 & 7)),((_get_dyneither_size(_tmp66F,sizeof(char))== 1  && (_tmp670 == '\000'  && _tmp671 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp66F.curr)=_tmp671)))))));}{
char _tmp674;char _tmp673;struct _dyneither_ptr _tmp672;(_tmp672=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp673=*((char*)_check_dyneither_subscript(_tmp672,sizeof(char),0)),((_tmp674=(char)('0' + (_tmp22 & 7)),((_get_dyneither_size(_tmp672,sizeof(char))== 1  && (_tmp673 == '\000'  && _tmp674 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp672.curr)=_tmp674)))))));};}
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
if(tq.q_restrict){struct Cyc_List_List*_tmp675;l=((_tmp675=_cycalloc(sizeof(*_tmp675)),((_tmp675->hd=Cyc_Absynpp_restrict_sp,((_tmp675->tl=l,_tmp675))))));}
if(tq.q_volatile){struct Cyc_List_List*_tmp676;l=((_tmp676=_cycalloc(sizeof(*_tmp676)),((_tmp676->hd=Cyc_Absynpp_volatile_sp,((_tmp676->tl=l,_tmp676))))));}
if(tq.print_const){struct Cyc_List_List*_tmp677;l=((_tmp677=_cycalloc(sizeof(*_tmp677)),((_tmp677->hd=Cyc_Absynpp_const_sp,((_tmp677->tl=l,_tmp677))))));}{
const char*_tmp67A;const char*_tmp679;const char*_tmp678;return Cyc_PP_egroup(((_tmp678="",_tag_dyneither(_tmp678,sizeof(char),1))),((_tmp679=" ",_tag_dyneither(_tmp679,sizeof(char),2))),((_tmp67A=" ",_tag_dyneither(_tmp67A,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));};}
# 370
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp72;enum Cyc_Absyn_AliasQual _tmp73;struct Cyc_Absyn_Kind*_tmp71=ka;_tmp72=_tmp71->kind;_tmp73=_tmp71->aliasqual;
switch(_tmp72){case Cyc_Absyn_AnyKind: _LL36:
# 374
 switch(_tmp73){case Cyc_Absyn_Aliasable: _LL38: {
const char*_tmp67B;return(_tmp67B="A",_tag_dyneither(_tmp67B,sizeof(char),2));}case Cyc_Absyn_Unique: _LL39: {
const char*_tmp67C;return(_tmp67C="UA",_tag_dyneither(_tmp67C,sizeof(char),3));}case Cyc_Absyn_Top: _LL3A: {
const char*_tmp67D;return(_tmp67D="TA",_tag_dyneither(_tmp67D,sizeof(char),3));}}case Cyc_Absyn_MemKind: _LL37:
# 380
 switch(_tmp73){case Cyc_Absyn_Aliasable: _LL3D: {
const char*_tmp67E;return(_tmp67E="M",_tag_dyneither(_tmp67E,sizeof(char),2));}case Cyc_Absyn_Unique: _LL3E: {
const char*_tmp67F;return(_tmp67F="UM",_tag_dyneither(_tmp67F,sizeof(char),3));}case Cyc_Absyn_Top: _LL3F: {
const char*_tmp680;return(_tmp680="TM",_tag_dyneither(_tmp680,sizeof(char),3));}}case Cyc_Absyn_BoxKind: _LL3C:
# 386
 switch(_tmp73){case Cyc_Absyn_Aliasable: _LL42: {
const char*_tmp681;return(_tmp681="B",_tag_dyneither(_tmp681,sizeof(char),2));}case Cyc_Absyn_Unique: _LL43: {
const char*_tmp682;return(_tmp682="UB",_tag_dyneither(_tmp682,sizeof(char),3));}case Cyc_Absyn_Top: _LL44: {
const char*_tmp683;return(_tmp683="TB",_tag_dyneither(_tmp683,sizeof(char),3));}}case Cyc_Absyn_RgnKind: _LL41:
# 392
 switch(_tmp73){case Cyc_Absyn_Aliasable: _LL47: {
const char*_tmp684;return(_tmp684="R",_tag_dyneither(_tmp684,sizeof(char),2));}case Cyc_Absyn_Unique: _LL48: {
const char*_tmp685;return(_tmp685="UR",_tag_dyneither(_tmp685,sizeof(char),3));}case Cyc_Absyn_Top: _LL49: {
const char*_tmp686;return(_tmp686="TR",_tag_dyneither(_tmp686,sizeof(char),3));}}case Cyc_Absyn_EffKind: _LL46: {
# 397
const char*_tmp687;return(_tmp687="E",_tag_dyneither(_tmp687,sizeof(char),2));}case Cyc_Absyn_IntKind: _LL4B: {
const char*_tmp688;return(_tmp688="I",_tag_dyneither(_tmp688,sizeof(char),2));}}}
# 401
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 403
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp82=Cyc_Absyn_compress_kb(c);void*_tmp83=_tmp82;struct Cyc_Absyn_Kind*_tmp85;struct Cyc_Absyn_Kind*_tmp88;_LL4E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp84=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp83;if(_tmp84->tag != 0)goto _LL50;else{_tmp85=_tmp84->f1;}}_LL4F:
 return Cyc_Absynpp_kind2string(_tmp85);_LL50: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp86=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp83;if(_tmp86->tag != 1)goto _LL52;}_LL51:
# 407
 if(Cyc_PP_tex_output){
const char*_tmp689;return(_tmp689="{?}",_tag_dyneither(_tmp689,sizeof(char),4));}else{
const char*_tmp68A;return(_tmp68A="?",_tag_dyneither(_tmp68A,sizeof(char),2));}_LL52: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp87=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp83;if(_tmp87->tag != 2)goto _LL4D;else{_tmp88=_tmp87->f2;}}_LL53: {
const char*_tmp68E;void*_tmp68D[1];struct Cyc_String_pa_PrintArg_struct _tmp68C;return(struct _dyneither_ptr)((_tmp68C.tag=0,((_tmp68C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp88)),((_tmp68D[0]=& _tmp68C,Cyc_aprintf(((_tmp68E="<=%s",_tag_dyneither(_tmp68E,sizeof(char),5))),_tag_dyneither(_tmp68D,sizeof(void*),1))))))));}_LL4D:;}
# 414
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp8E=Cyc_Absyn_compress_kb(c);void*_tmp8F=_tmp8E;struct Cyc_Absyn_Kind*_tmp91;struct Cyc_Absyn_Kind*_tmp94;_LL55: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp90=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8F;if(_tmp90->tag != 0)goto _LL57;else{_tmp91=_tmp90->f1;}}_LL56:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp91));_LL57: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp92=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8F;if(_tmp92->tag != 1)goto _LL59;}_LL58:
# 418
 if(Cyc_PP_tex_output){
const char*_tmp68F;return Cyc_PP_text_width(((_tmp68F="{?}",_tag_dyneither(_tmp68F,sizeof(char),4))),1);}else{
const char*_tmp690;return Cyc_PP_text(((_tmp690="?",_tag_dyneither(_tmp690,sizeof(char),2))));}_LL59: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp93=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F;if(_tmp93->tag != 2)goto _LL54;else{_tmp94=_tmp93->f2;}}_LL5A:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp94));_LL54:;}
# 425
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
const char*_tmp693;const char*_tmp692;const char*_tmp691;return Cyc_PP_egroup(((_tmp691="<",_tag_dyneither(_tmp691,sizeof(char),2))),((_tmp692=">",_tag_dyneither(_tmp692,sizeof(char),2))),((_tmp693=",",_tag_dyneither(_tmp693,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));}
# 429
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr _tmp9A=*tv->name;
# 433
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(tv)){
# 435
const char*_tmp694;struct _dyneither_ptr kstring=(_tmp694="K",_tag_dyneither(_tmp694,sizeof(char),2));
{void*_tmp9B=Cyc_Absyn_compress_kb(tv->kind);void*_tmp9C=_tmp9B;struct Cyc_Absyn_Kind*_tmp9E;struct Cyc_Absyn_Kind*_tmpA0;_LL5C: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9C;if(_tmp9D->tag != 2)goto _LL5E;else{_tmp9E=_tmp9D->f2;}}_LL5D:
 _tmpA0=_tmp9E;goto _LL5F;_LL5E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp9F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9C;if(_tmp9F->tag != 0)goto _LL60;else{_tmpA0=_tmp9F->f1;}}_LL5F:
 kstring=Cyc_Absynpp_kind2string(_tmpA0);goto _LL5B;_LL60:;_LL61:
 goto _LL5B;_LL5B:;}{
# 441
const char*_tmp699;void*_tmp698[2];struct Cyc_String_pa_PrintArg_struct _tmp697;struct Cyc_String_pa_PrintArg_struct _tmp696;return Cyc_PP_text((struct _dyneither_ptr)((_tmp696.tag=0,((_tmp696.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_tmp9A,sizeof(char),1)),((_tmp697.tag=0,((_tmp697.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kstring),((_tmp698[0]=& _tmp697,((_tmp698[1]=& _tmp696,Cyc_aprintf(((_tmp699="`G%s%s",_tag_dyneither(_tmp699,sizeof(char),7))),_tag_dyneither(_tmp698,sizeof(void*),2)))))))))))))));};}
# 443
return Cyc_PP_text(_tmp9A);}
# 446
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmpA6=Cyc_Absyn_compress_kb(tv->kind);void*_tmpA7=_tmpA6;struct Cyc_Absyn_Kind*_tmpAB;struct Cyc_Absyn_Kind*_tmpAD;_LL63: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA8=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA7;if(_tmpA8->tag != 1)goto _LL65;}_LL64:
 goto _LL66;_LL65: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA9=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7;if(_tmpA9->tag != 0)goto _LL67;else{if((_tmpA9->f1)->kind != Cyc_Absyn_BoxKind)goto _LL67;if((_tmpA9->f1)->aliasqual != Cyc_Absyn_Aliasable)goto _LL67;}}_LL66:
 return Cyc_Absynpp_tvar2doc(tv);_LL67: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAA=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7;if(_tmpAA->tag != 2)goto _LL69;else{_tmpAB=_tmpAA->f2;}}_LL68:
 _tmpAD=_tmpAB;goto _LL6A;_LL69: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpAC=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7;if(_tmpAC->tag != 0)goto _LL62;else{_tmpAD=_tmpAC->f1;}}_LL6A: {
const char*_tmp69C;struct Cyc_PP_Doc*_tmp69B[3];return(_tmp69B[2]=Cyc_Absynpp_kind2doc(_tmpAD),((_tmp69B[1]=Cyc_PP_text(((_tmp69C="::",_tag_dyneither(_tmp69C,sizeof(char),3)))),((_tmp69B[0]=Cyc_Absynpp_tvar2doc(tv),Cyc_PP_cat(_tag_dyneither(_tmp69B,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL62:;}
# 455
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
const char*_tmp69F;const char*_tmp69E;const char*_tmp69D;return Cyc_PP_egroup(((_tmp69D="<",_tag_dyneither(_tmp69D,sizeof(char),2))),((_tmp69E=">",_tag_dyneither(_tmp69E,sizeof(char),2))),((_tmp69F=",",_tag_dyneither(_tmp69F,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}
# 459
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);{
const char*_tmp6A2;const char*_tmp6A1;const char*_tmp6A0;return Cyc_PP_egroup(((_tmp6A0="<",_tag_dyneither(_tmp6A0,sizeof(char),2))),((_tmp6A1=">",_tag_dyneither(_tmp6A1,sizeof(char),2))),((_tmp6A2=",",_tag_dyneither(_tmp6A2,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));};}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 465
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 469
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
const char*_tmp6A5;const char*_tmp6A4;const char*_tmp6A3;return Cyc_PP_group(((_tmp6A3="(",_tag_dyneither(_tmp6A3,sizeof(char),2))),((_tmp6A4=")",_tag_dyneither(_tmp6A4,sizeof(char),2))),((_tmp6A5=",",_tag_dyneither(_tmp6A5,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
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
const char*_tmp6A6;return Cyc_PP_text(((_tmp6A6=" _stdcall ",_tag_dyneither(_tmp6A6,sizeof(char),11))));}_LL77: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpC0=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpBE;if(_tmpC0->tag != 2)goto _LL79;}_LL78: {
const char*_tmp6A7;return Cyc_PP_text(((_tmp6A7=" _cdecl ",_tag_dyneither(_tmp6A7,sizeof(char),9))));}_LL79: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpC1=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpBE;if(_tmpC1->tag != 3)goto _LL7B;}_LL7A: {
const char*_tmp6A8;return Cyc_PP_text(((_tmp6A8=" _fastcall ",_tag_dyneither(_tmp6A8,sizeof(char),12))));}_LL7B:;_LL7C:
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
const char*_tmp6B3;const char*_tmp6B2;const char*_tmp6B1;const char*_tmp6B0;const char*_tmp6AF;struct Cyc_PP_Doc*_tmp6AE[3];return(_tmp6AE[2]=
# 506
Cyc_PP_text(((_tmp6AF=")",_tag_dyneither(_tmp6AF,sizeof(char),2)))),((_tmp6AE[1]=
# 505
Cyc_PP_group(((_tmp6B2="",_tag_dyneither(_tmp6B2,sizeof(char),1))),((_tmp6B1="",_tag_dyneither(_tmp6B1,sizeof(char),1))),((_tmp6B0=" ",_tag_dyneither(_tmp6B0,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts)),((_tmp6AE[0]=
# 504
Cyc_PP_text(((_tmp6B3=" __declspec(",_tag_dyneither(_tmp6B3,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp6AE,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 509
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 513
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL86:
 return Cyc_Absynpp_noncallconv2doc(atts);case Cyc_Cyclone_Gcc_c: _LL87: {
const char*_tmp6BC;const char*_tmp6BB;const char*_tmp6BA;const char*_tmp6B9;struct Cyc_PP_Doc*_tmp6B8[2];return(_tmp6B8[1]=
Cyc_PP_group(((_tmp6BB="((",_tag_dyneither(_tmp6BB,sizeof(char),3))),((_tmp6BA="))",_tag_dyneither(_tmp6BA,sizeof(char),3))),((_tmp6B9=",",_tag_dyneither(_tmp6B9,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp6B8[0]=
# 517
Cyc_PP_text(((_tmp6BC=" __attribute__",_tag_dyneither(_tmp6BC,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp6B8,sizeof(struct Cyc_PP_Doc*),2)))));}}}
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
const char*_tmp6BD;Cyc_Absynpp_cache_question=Cyc_PP_text_width(((_tmp6BD="{?}",_tag_dyneither(_tmp6BD,sizeof(char),4))),1);}else{
const char*_tmp6BE;Cyc_Absynpp_cache_question=Cyc_PP_text(((_tmp6BE="?",_tag_dyneither(_tmp6BE,sizeof(char),2))));}}
# 545
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 547
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output){
const char*_tmp6BF;Cyc_Absynpp_cache_lb=Cyc_PP_text_width(((_tmp6BF="{\\lb}",_tag_dyneither(_tmp6BF,sizeof(char),6))),1);}else{
const char*_tmp6C0;Cyc_Absynpp_cache_lb=Cyc_PP_text(((_tmp6C0="{",_tag_dyneither(_tmp6C0,sizeof(char),2))));}}
# 554
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 556
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output){
const char*_tmp6C1;Cyc_Absynpp_cache_rb=Cyc_PP_text_width(((_tmp6C1="{\\rb}",_tag_dyneither(_tmp6C1,sizeof(char),6))),1);}else{
const char*_tmp6C2;Cyc_Absynpp_cache_rb=Cyc_PP_text(((_tmp6C2="}",_tag_dyneither(_tmp6C2,sizeof(char),2))));}}
# 563
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 565
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output){
const char*_tmp6C3;Cyc_Absynpp_cache_dollar=Cyc_PP_text_width(((_tmp6C3="\\$",_tag_dyneither(_tmp6C3,sizeof(char),3))),1);}else{
const char*_tmp6C4;Cyc_Absynpp_cache_dollar=Cyc_PP_text(((_tmp6C4="$",_tag_dyneither(_tmp6C4,sizeof(char),2))));}}
# 572
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 574
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){
struct Cyc_PP_Doc*_tmp6C5[3];return(_tmp6C5[2]=Cyc_Absynpp_rb(),((_tmp6C5[1]=Cyc_PP_seq(sep,ss),((_tmp6C5[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp6C5,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 579
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpE2=(void*)tms->hd;void*_tmpE3=_tmpE2;struct Cyc_List_List*_tmpE8;_LL94: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpE4=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpE4->tag != 0)goto _LL96;}_LL95:
{const char*_tmp6C8;void*_tmp6C7;(_tmp6C7=0,Cyc_fprintf(Cyc_stderr,((_tmp6C8="Carray_mod ",_tag_dyneither(_tmp6C8,sizeof(char),12))),_tag_dyneither(_tmp6C7,sizeof(void*),0)));}goto _LL93;_LL96: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpE5=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpE5->tag != 1)goto _LL98;}_LL97:
{const char*_tmp6CB;void*_tmp6CA;(_tmp6CA=0,Cyc_fprintf(Cyc_stderr,((_tmp6CB="ConstArray_mod ",_tag_dyneither(_tmp6CB,sizeof(char),16))),_tag_dyneither(_tmp6CA,sizeof(void*),0)));}goto _LL93;_LL98: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpE6=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpE6->tag != 3)goto _LL9A;else{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpE7=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((void*)_tmpE6->f1);if(_tmpE7->tag != 1)goto _LL9A;else{_tmpE8=_tmpE7->f1;}}}_LL99:
# 585
{const char*_tmp6CE;void*_tmp6CD;(_tmp6CD=0,Cyc_fprintf(Cyc_stderr,((_tmp6CE="Function_mod(",_tag_dyneither(_tmp6CE,sizeof(char),14))),_tag_dyneither(_tmp6CD,sizeof(void*),0)));}
for(0;_tmpE8 != 0;_tmpE8=_tmpE8->tl){
struct _dyneither_ptr*_tmpF3=(*((struct _tuple8*)_tmpE8->hd)).f1;
if(_tmpF3 == 0){const char*_tmp6D1;void*_tmp6D0;(_tmp6D0=0,Cyc_fprintf(Cyc_stderr,((_tmp6D1="?",_tag_dyneither(_tmp6D1,sizeof(char),2))),_tag_dyneither(_tmp6D0,sizeof(void*),0)));}else{
void*_tmp6D2;(_tmp6D2=0,Cyc_fprintf(Cyc_stderr,*_tmpF3,_tag_dyneither(_tmp6D2,sizeof(void*),0)));}
if(_tmpE8->tl != 0){const char*_tmp6D5;void*_tmp6D4;(_tmp6D4=0,Cyc_fprintf(Cyc_stderr,((_tmp6D5=",",_tag_dyneither(_tmp6D5,sizeof(char),2))),_tag_dyneither(_tmp6D4,sizeof(void*),0)));}}
# 592
{const char*_tmp6D8;void*_tmp6D7;(_tmp6D7=0,Cyc_fprintf(Cyc_stderr,((_tmp6D8=") ",_tag_dyneither(_tmp6D8,sizeof(char),3))),_tag_dyneither(_tmp6D7,sizeof(void*),0)));}
goto _LL93;_LL9A: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpE9=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpE9->tag != 3)goto _LL9C;}_LL9B:
# 595
{const char*_tmp6DB;void*_tmp6DA;(_tmp6DA=0,Cyc_fprintf(Cyc_stderr,((_tmp6DB="Function_mod()",_tag_dyneither(_tmp6DB,sizeof(char),15))),_tag_dyneither(_tmp6DA,sizeof(void*),0)));}goto _LL93;_LL9C: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpEA=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpEA->tag != 5)goto _LL9E;}_LL9D:
{const char*_tmp6DE;void*_tmp6DD;(_tmp6DD=0,Cyc_fprintf(Cyc_stderr,((_tmp6DE="Attributes_mod ",_tag_dyneither(_tmp6DE,sizeof(char),16))),_tag_dyneither(_tmp6DD,sizeof(void*),0)));}goto _LL93;_LL9E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpEB=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpEB->tag != 4)goto _LLA0;}_LL9F:
{const char*_tmp6E1;void*_tmp6E0;(_tmp6E0=0,Cyc_fprintf(Cyc_stderr,((_tmp6E1="TypeParams_mod ",_tag_dyneither(_tmp6E1,sizeof(char),16))),_tag_dyneither(_tmp6E0,sizeof(void*),0)));}goto _LL93;_LLA0: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpEC=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpEC->tag != 2)goto _LL93;}_LLA1:
{const char*_tmp6E4;void*_tmp6E3;(_tmp6E3=0,Cyc_fprintf(Cyc_stderr,((_tmp6E4="Pointer_mod ",_tag_dyneither(_tmp6E4,sizeof(char),13))),_tag_dyneither(_tmp6E3,sizeof(void*),0)));}goto _LL93;_LL93:;}{
# 600
const char*_tmp6E7;void*_tmp6E6;(_tmp6E6=0,Cyc_fprintf(Cyc_stderr,((_tmp6E7="\n",_tag_dyneither(_tmp6E7,sizeof(char),2))),_tag_dyneither(_tmp6E6,sizeof(void*),0)));};}
# 603
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 605
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
const char*_tmp6EC;const char*_tmp6EB;struct Cyc_PP_Doc*_tmp6EA[3];struct Cyc_PP_Doc*p_rest=(_tmp6EA[2]=Cyc_PP_text(((_tmp6EB=")",_tag_dyneither(_tmp6EB,sizeof(char),2)))),((_tmp6EA[1]=rest,((_tmp6EA[0]=Cyc_PP_text(((_tmp6EC="(",_tag_dyneither(_tmp6EC,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6EA,sizeof(struct Cyc_PP_Doc*),3)))))));
void*_tmp105=(void*)tms->hd;void*_tmp106=_tmp105;union Cyc_Absyn_Constraint*_tmp108;struct Cyc_Absyn_Exp*_tmp10A;union Cyc_Absyn_Constraint*_tmp10B;void*_tmp10D;struct Cyc_List_List*_tmp10F;struct Cyc_List_List*_tmp111;unsigned int _tmp112;int _tmp113;void*_tmp115;union Cyc_Absyn_Constraint*_tmp116;union Cyc_Absyn_Constraint*_tmp117;union Cyc_Absyn_Constraint*_tmp118;struct Cyc_Absyn_Tqual _tmp119;_LLA3: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp107=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp107->tag != 0)goto _LLA5;else{_tmp108=_tmp107->f1;}}_LLA4:
# 611
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6F1;const char*_tmp6F0;struct Cyc_PP_Doc*_tmp6EF[2];return(_tmp6EF[1]=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp108)?
Cyc_PP_text(((_tmp6F0="[]@zeroterm",_tag_dyneither(_tmp6F0,sizeof(char),12)))): Cyc_PP_text(((_tmp6F1="[]",_tag_dyneither(_tmp6F1,sizeof(char),3)))),((_tmp6EF[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6EF,sizeof(struct Cyc_PP_Doc*),2)))));};_LLA5: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp109=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp109->tag != 1)goto _LLA7;else{_tmp10A=_tmp109->f1;_tmp10B=_tmp109->f2;}}_LLA6:
# 615
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6F8;const char*_tmp6F7;const char*_tmp6F6;struct Cyc_PP_Doc*_tmp6F5[4];return(_tmp6F5[3]=
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp10B)?Cyc_PP_text(((_tmp6F6="]@zeroterm",_tag_dyneither(_tmp6F6,sizeof(char),11)))): Cyc_PP_text(((_tmp6F7="]",_tag_dyneither(_tmp6F7,sizeof(char),2)))),((_tmp6F5[2]=
# 616
Cyc_Absynpp_exp2doc(_tmp10A),((_tmp6F5[1]=Cyc_PP_text(((_tmp6F8="[",_tag_dyneither(_tmp6F8,sizeof(char),2)))),((_tmp6F5[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F5,sizeof(struct Cyc_PP_Doc*),4)))))))));};_LLA7: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp10C=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp10C->tag != 3)goto _LLA9;else{_tmp10D=(void*)_tmp10C->f1;}}_LLA8:
# 619
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmp121=_tmp10D;struct Cyc_List_List*_tmp123;int _tmp124;struct Cyc_Absyn_VarargInfo*_tmp125;void*_tmp126;struct Cyc_List_List*_tmp127;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_List_List*_tmp12B;unsigned int _tmp12C;_LLB0: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp122=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp121;if(_tmp122->tag != 1)goto _LLB2;else{_tmp123=_tmp122->f1;_tmp124=_tmp122->f2;_tmp125=_tmp122->f3;_tmp126=(void*)_tmp122->f4;_tmp127=_tmp122->f5;_tmp128=_tmp122->f6;_tmp129=_tmp122->f7;}}_LLB1: {
# 622
struct Cyc_PP_Doc*_tmp6F9[2];return(_tmp6F9[1]=Cyc_Absynpp_funargs2doc(_tmp123,_tmp124,_tmp125,_tmp126,_tmp127,_tmp128,_tmp129),((_tmp6F9[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F9,sizeof(struct Cyc_PP_Doc*),2)))));}_LLB2: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp12A=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp121;if(_tmp12A->tag != 0)goto _LLAF;else{_tmp12B=_tmp12A->f1;_tmp12C=_tmp12A->f2;}}_LLB3: {
# 624
const char*_tmp700;const char*_tmp6FF;const char*_tmp6FE;struct Cyc_PP_Doc*_tmp6FD[2];return(_tmp6FD[1]=Cyc_PP_group(((_tmp700="(",_tag_dyneither(_tmp700,sizeof(char),2))),((_tmp6FF=")",_tag_dyneither(_tmp6FF,sizeof(char),2))),((_tmp6FE=",",_tag_dyneither(_tmp6FE,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp12B)),((_tmp6FD[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6FD,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAF:;};_LLA9: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp10E=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp10E->tag != 5)goto _LLAB;else{_tmp10F=_tmp10E->f2;}}_LLAA:
# 627
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLB4:
# 629
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp701[2];return(_tmp701[1]=Cyc_Absynpp_atts2doc(_tmp10F),((_tmp701[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp701,sizeof(struct Cyc_PP_Doc*),2)))));};case Cyc_Cyclone_Vc_c: _LLB5:
# 632
 if(Cyc_Absynpp_next_is_pointer(tms->tl)){
struct Cyc_PP_Doc*_tmp702[2];return(_tmp702[1]=rest,((_tmp702[0]=Cyc_Absynpp_callconv2doc(_tmp10F),Cyc_PP_cat(_tag_dyneither(_tmp702,sizeof(struct Cyc_PP_Doc*),2)))));}
return rest;}_LLAB: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp110=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp110->tag != 4)goto _LLAD;else{_tmp111=_tmp110->f1;_tmp112=_tmp110->f2;_tmp113=_tmp110->f3;}}_LLAC:
# 637
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp113){
struct Cyc_PP_Doc*_tmp703[2];return(_tmp703[1]=Cyc_Absynpp_ktvars2doc(_tmp111),((_tmp703[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp703,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 641
struct Cyc_PP_Doc*_tmp704[2];return(_tmp704[1]=Cyc_Absynpp_tvars2doc(_tmp111),((_tmp704[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp704,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAD: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp114=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp114->tag != 2)goto _LLA2;else{_tmp115=(_tmp114->f1).rgn;_tmp116=(_tmp114->f1).nullable;_tmp117=(_tmp114->f1).bounds;_tmp118=(_tmp114->f1).zero_term;_tmp119=_tmp114->f2;}}_LLAE: {
# 645
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(_tmp119);
{void*_tmp136=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp117);void*_tmp137=_tmp136;struct Cyc_Absyn_Exp*_tmp13A;_LLB8: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp138=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp137;if(_tmp138->tag != 0)goto _LLBA;}_LLB9:
 ptr=Cyc_Absynpp_question();goto _LLB7;_LLBA: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp139=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp137;if(_tmp139->tag != 1)goto _LLB7;else{_tmp13A=_tmp139->f1;}}_LLBB:
# 653
{const char*_tmp706;const char*_tmp705;ptr=Cyc_PP_text(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp116)?(_tmp706="*",_tag_dyneither(_tmp706,sizeof(char),2)):((_tmp705="@",_tag_dyneither(_tmp705,sizeof(char),2))));}{
struct _tuple11 _tmp13D=Cyc_Evexp_eval_const_uint_exp(_tmp13A);unsigned int _tmp13F;int _tmp140;struct _tuple11 _tmp13E=_tmp13D;_tmp13F=_tmp13E.f1;_tmp140=_tmp13E.f2;
if(!_tmp140  || _tmp13F != 1){
struct Cyc_PP_Doc*_tmp707[4];ptr=((_tmp707[3]=Cyc_Absynpp_rb(),((_tmp707[2]=Cyc_Absynpp_exp2doc(_tmp13A),((_tmp707[1]=Cyc_Absynpp_lb(),((_tmp707[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp707,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB7;};_LLB7:;}
# 659
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp118)){
const char*_tmp708;ztd=Cyc_PP_text(((_tmp708="@zeroterm",_tag_dyneither(_tmp708,sizeof(char),10))));}else{
if(is_char_ptr  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp118)){
const char*_tmp709;ztd=Cyc_PP_text(((_tmp709="@nozeroterm",_tag_dyneither(_tmp709,sizeof(char),12))));}}}
# 665
{void*_tmp144=Cyc_Tcutil_compress(_tmp115);void*_tmp145=_tmp144;_LLBD: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp146=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp145;if(_tmp146->tag != 20)goto _LLBF;}_LLBE:
 goto _LLBC;_LLBF:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp147=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp145;if(_tmp147->tag != 1)goto _LLC1;}if(!Cyc_Absynpp_print_for_cycdoc)goto _LLC1;_LLC0:
 goto _LLBC;_LLC1:;_LLC2:
 rgd=Cyc_Absynpp_rgn2doc(_tmp115);_LLBC:;}{
# 670
const char*_tmp70A;struct Cyc_PP_Doc*spacer1=tqd != mt  && (ztd != mt  || rgd != mt)?Cyc_PP_text(((_tmp70A=" ",_tag_dyneither(_tmp70A,sizeof(char),2)))): mt;
const char*_tmp70B;struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(((_tmp70B=" ",_tag_dyneither(_tmp70B,sizeof(char),2)))): mt;
struct Cyc_PP_Doc*_tmp70C[7];return(_tmp70C[6]=rest,((_tmp70C[5]=spacer2,((_tmp70C[4]=tqd,((_tmp70C[3]=spacer1,((_tmp70C[2]=rgd,((_tmp70C[1]=ztd,((_tmp70C[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp70C,sizeof(struct Cyc_PP_Doc*),7)))))))))))))));};}_LLA2:;};}
# 676
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp14E=Cyc_Tcutil_compress(t);void*_tmp14F=_tmp14E;_LLC4: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp150=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp14F;if(_tmp150->tag != 20)goto _LLC6;}_LLC5: {
const char*_tmp70D;return Cyc_PP_text(((_tmp70D="`H",_tag_dyneither(_tmp70D,sizeof(char),3))));}_LLC6: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp151=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp14F;if(_tmp151->tag != 21)goto _LLC8;}_LLC7: {
const char*_tmp70E;return Cyc_PP_text(((_tmp70E="`U",_tag_dyneither(_tmp70E,sizeof(char),3))));}_LLC8: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp152=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp14F;if(_tmp152->tag != 22)goto _LLCA;}_LLC9: {
const char*_tmp70F;return Cyc_PP_text(((_tmp70F="`RC",_tag_dyneither(_tmp70F,sizeof(char),4))));}_LLCA:;_LLCB:
 return Cyc_Absynpp_ntyp2doc(t);_LLC3:;}
# 686
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 690
void*_tmp156=Cyc_Tcutil_compress(t);void*_tmp157=_tmp156;void*_tmp159;struct Cyc_List_List*_tmp15B;_LLCD: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp158=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp157;if(_tmp158->tag != 23)goto _LLCF;else{_tmp159=(void*)_tmp158->f1;}}_LLCE:
{struct Cyc_List_List*_tmp710;*rgions=((_tmp710=_cycalloc(sizeof(*_tmp710)),((_tmp710->hd=Cyc_Absynpp_rgn2doc(_tmp159),((_tmp710->tl=*rgions,_tmp710))))));}goto _LLCC;_LLCF: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp15A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp157;if(_tmp15A->tag != 24)goto _LLD1;else{_tmp15B=_tmp15A->f1;}}_LLD0:
# 693
 for(0;_tmp15B != 0;_tmp15B=_tmp15B->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp15B->hd);}
# 696
goto _LLCC;_LLD1:;_LLD2:
{struct Cyc_List_List*_tmp711;*effects=((_tmp711=_cycalloc(sizeof(*_tmp711)),((_tmp711->hd=Cyc_Absynpp_typ2doc(t),((_tmp711->tl=*effects,_tmp711))))));}goto _LLCC;_LLCC:;}
# 701
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0  && effects != 0){
const char*_tmp714;const char*_tmp713;const char*_tmp712;return Cyc_PP_group(((_tmp712="",_tag_dyneither(_tmp712,sizeof(char),1))),((_tmp713="",_tag_dyneither(_tmp713,sizeof(char),1))),((_tmp714="+",_tag_dyneither(_tmp714,sizeof(char),2))),effects);}else{
# 709
const char*_tmp715;struct Cyc_PP_Doc*_tmp161=Cyc_Absynpp_group_braces(((_tmp715=",",_tag_dyneither(_tmp715,sizeof(char),2))),rgions);
struct Cyc_List_List*_tmp719;const char*_tmp718;const char*_tmp717;const char*_tmp716;return Cyc_PP_group(((_tmp716="",_tag_dyneither(_tmp716,sizeof(char),1))),((_tmp717="",_tag_dyneither(_tmp717,sizeof(char),1))),((_tmp718="+",_tag_dyneither(_tmp718,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((_tmp719=_cycalloc(sizeof(*_tmp719)),((_tmp719->hd=_tmp161,((_tmp719->tl=0,_tmp719))))))));}}
# 714
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
switch(k){case Cyc_Absyn_StructA: _LLD3: {
const char*_tmp71A;return Cyc_PP_text(((_tmp71A="struct ",_tag_dyneither(_tmp71A,sizeof(char),8))));}case Cyc_Absyn_UnionA: _LLD4: {
const char*_tmp71B;return Cyc_PP_text(((_tmp71B="union ",_tag_dyneither(_tmp71B,sizeof(char),7))));}}}
# 722
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp169=t;struct Cyc_Core_Opt*_tmp16F;void*_tmp170;int _tmp171;struct Cyc_Core_Opt*_tmp172;struct Cyc_Absyn_Tvar*_tmp174;union Cyc_Absyn_DatatypeInfoU _tmp176;struct Cyc_List_List*_tmp177;union Cyc_Absyn_DatatypeFieldInfoU _tmp179;struct Cyc_List_List*_tmp17A;enum Cyc_Absyn_Sign _tmp17C;enum Cyc_Absyn_Size_of _tmp17D;int _tmp17F;struct Cyc_List_List*_tmp181;union Cyc_Absyn_AggrInfoU _tmp183;struct Cyc_List_List*_tmp184;enum Cyc_Absyn_AggrKind _tmp186;struct Cyc_List_List*_tmp187;struct Cyc_List_List*_tmp189;struct _tuple0*_tmp18B;struct Cyc_Absyn_Exp*_tmp18D;struct Cyc_Absyn_Exp*_tmp18F;struct _tuple0*_tmp191;struct Cyc_List_List*_tmp192;struct Cyc_Absyn_Typedefdecl*_tmp193;void*_tmp195;void*_tmp197;void*_tmp198;void*_tmp19A;void*_tmp19F;struct Cyc_Absyn_Aggrdecl*_tmp1A4;struct Cyc_Absyn_Enumdecl*_tmp1A7;struct Cyc_Absyn_Datatypedecl*_tmp1AA;_LLD7: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp16A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp169;if(_tmp16A->tag != 8)goto _LLD9;}_LLD8: {
# 726
const char*_tmp71C;return Cyc_PP_text(((_tmp71C="[[[array]]]",_tag_dyneither(_tmp71C,sizeof(char),12))));}_LLD9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp16B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp169;if(_tmp16B->tag != 9)goto _LLDB;}_LLDA:
 return Cyc_PP_nil_doc();_LLDB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp16C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp169;if(_tmp16C->tag != 5)goto _LLDD;}_LLDC:
 return Cyc_PP_nil_doc();_LLDD: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp16D=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp169;if(_tmp16D->tag != 0)goto _LLDF;}_LLDE:
# 730
{const char*_tmp71D;s=Cyc_PP_text(((_tmp71D="void",_tag_dyneither(_tmp71D,sizeof(char),5))));}goto _LLD6;_LLDF: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp16E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp169;if(_tmp16E->tag != 1)goto _LLE1;else{_tmp16F=_tmp16E->f1;_tmp170=(void*)_tmp16E->f2;_tmp171=_tmp16E->f3;_tmp172=_tmp16E->f4;}}_LLE0:
# 732
 if(_tmp170 != 0)
# 734
return Cyc_Absynpp_ntyp2doc(_tmp170);else{
# 736
const char*_tmp71E;struct _dyneither_ptr kindstring=_tmp16F == 0?(_tmp71E="K",_tag_dyneither(_tmp71E,sizeof(char),2)): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)_tmp16F->v);
const char*_tmp723;void*_tmp722[2];struct Cyc_String_pa_PrintArg_struct _tmp721;struct Cyc_Int_pa_PrintArg_struct _tmp720;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp720.tag=1,((_tmp720.f1=(unsigned long)_tmp171,((_tmp721.tag=0,((_tmp721.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kindstring),((_tmp722[0]=& _tmp721,((_tmp722[1]=& _tmp720,Cyc_aprintf(((_tmp723="`E%s%d",_tag_dyneither(_tmp723,sizeof(char),7))),_tag_dyneither(_tmp722,sizeof(void*),2)))))))))))))));}
# 753 "absynpp.cyc"
goto _LLD6;_LLE1: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp173=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp169;if(_tmp173->tag != 2)goto _LLE3;else{_tmp174=_tmp173->f1;}}_LLE2:
# 755
 s=Cyc_Absynpp_tvar2doc(_tmp174);
if(Cyc_Absynpp_print_all_kinds){
const char*_tmp726;struct Cyc_PP_Doc*_tmp725[3];s=((_tmp725[2]=Cyc_Absynpp_kindbound2doc(_tmp174->kind),((_tmp725[1]=Cyc_PP_text(((_tmp726="::",_tag_dyneither(_tmp726,sizeof(char),3)))),((_tmp725[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp725,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 759
goto _LLD6;_LLE3: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp175=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp169;if(_tmp175->tag != 3)goto _LLE5;else{_tmp176=(_tmp175->f1).datatype_info;_tmp177=(_tmp175->f1).targs;}}_LLE4:
# 761
{union Cyc_Absyn_DatatypeInfoU _tmp1B4=_tmp176;struct _tuple0*_tmp1B5;int _tmp1B6;struct _tuple0*_tmp1B7;int _tmp1B8;_LL114: if((_tmp1B4.UnknownDatatype).tag != 1)goto _LL116;_tmp1B5=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1B4.UnknownDatatype).val).name;_tmp1B6=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1B4.UnknownDatatype).val).is_extensible;_LL115:
# 763
 _tmp1B7=_tmp1B5;_tmp1B8=_tmp1B6;goto _LL117;_LL116: if((_tmp1B4.KnownDatatype).tag != 2)goto _LL113;_tmp1B7=(*((struct Cyc_Absyn_Datatypedecl**)(_tmp1B4.KnownDatatype).val))->name;_tmp1B8=(*((struct Cyc_Absyn_Datatypedecl**)(_tmp1B4.KnownDatatype).val))->is_extensible;_LL117: {
# 765
const char*_tmp727;struct Cyc_PP_Doc*_tmp1B9=Cyc_PP_text(((_tmp727="datatype ",_tag_dyneither(_tmp727,sizeof(char),10))));
const char*_tmp728;struct Cyc_PP_Doc*_tmp1BA=_tmp1B8?Cyc_PP_text(((_tmp728="@extensible ",_tag_dyneither(_tmp728,sizeof(char),13)))): Cyc_PP_nil_doc();
{struct Cyc_PP_Doc*_tmp729[4];s=((_tmp729[3]=Cyc_Absynpp_tps2doc(_tmp177),((_tmp729[2]=Cyc_Absynpp_qvar2doc(_tmp1B7),((_tmp729[1]=_tmp1B9,((_tmp729[0]=_tmp1BA,Cyc_PP_cat(_tag_dyneither(_tmp729,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL113;}_LL113:;}
# 770
goto _LLD6;_LLE5: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp178=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp169;if(_tmp178->tag != 4)goto _LLE7;else{_tmp179=(_tmp178->f1).field_info;_tmp17A=(_tmp178->f1).targs;}}_LLE6:
# 772
{union Cyc_Absyn_DatatypeFieldInfoU _tmp1BE=_tmp179;struct _tuple0*_tmp1BF;struct _tuple0*_tmp1C0;int _tmp1C1;struct _tuple0*_tmp1C2;int _tmp1C3;struct _tuple0*_tmp1C4;_LL119: if((_tmp1BE.UnknownDatatypefield).tag != 1)goto _LL11B;_tmp1BF=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1BE.UnknownDatatypefield).val).datatype_name;_tmp1C0=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1BE.UnknownDatatypefield).val).field_name;_tmp1C1=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1BE.UnknownDatatypefield).val).is_extensible;_LL11A:
# 774
 _tmp1C2=_tmp1BF;_tmp1C3=_tmp1C1;_tmp1C4=_tmp1C0;goto _LL11C;_LL11B: if((_tmp1BE.KnownDatatypefield).tag != 2)goto _LL118;_tmp1C2=(((struct _tuple1)(_tmp1BE.KnownDatatypefield).val).f1)->name;_tmp1C3=(((struct _tuple1)(_tmp1BE.KnownDatatypefield).val).f1)->is_extensible;_tmp1C4=(((struct _tuple1)(_tmp1BE.KnownDatatypefield).val).f2)->name;_LL11C: {
# 777
const char*_tmp72B;const char*_tmp72A;struct Cyc_PP_Doc*_tmp1C5=Cyc_PP_text(_tmp1C3?(_tmp72B="@extensible datatype ",_tag_dyneither(_tmp72B,sizeof(char),22)):((_tmp72A="datatype ",_tag_dyneither(_tmp72A,sizeof(char),10))));
{const char*_tmp72E;struct Cyc_PP_Doc*_tmp72D[4];s=((_tmp72D[3]=Cyc_Absynpp_qvar2doc(_tmp1C4),((_tmp72D[2]=Cyc_PP_text(((_tmp72E=".",_tag_dyneither(_tmp72E,sizeof(char),2)))),((_tmp72D[1]=Cyc_Absynpp_qvar2doc(_tmp1C2),((_tmp72D[0]=_tmp1C5,Cyc_PP_cat(_tag_dyneither(_tmp72D,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL118;}_LL118:;}
# 781
goto _LLD6;_LLE7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp17B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp169;if(_tmp17B->tag != 6)goto _LLE9;else{_tmp17C=_tmp17B->f1;_tmp17D=_tmp17B->f2;}}_LLE8: {
# 783
struct _dyneither_ptr sns;
struct _dyneither_ptr ts;
switch(_tmp17C){case Cyc_Absyn_None: _LL11D:
 goto _LL11E;case Cyc_Absyn_Signed: _LL11E:
{const char*_tmp72F;sns=((_tmp72F="",_tag_dyneither(_tmp72F,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL11F:
{const char*_tmp730;sns=((_tmp730="unsigned ",_tag_dyneither(_tmp730,sizeof(char),10)));}break;}
# 790
switch(_tmp17D){case Cyc_Absyn_Char_sz: _LL121:
# 792
 switch(_tmp17C){case Cyc_Absyn_None: _LL123:
{const char*_tmp731;sns=((_tmp731="",_tag_dyneither(_tmp731,sizeof(char),1)));}break;case Cyc_Absyn_Signed: _LL124:
{const char*_tmp732;sns=((_tmp732="signed ",_tag_dyneither(_tmp732,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL125:
{const char*_tmp733;sns=((_tmp733="unsigned ",_tag_dyneither(_tmp733,sizeof(char),10)));}break;}
# 797
{const char*_tmp734;ts=((_tmp734="char",_tag_dyneither(_tmp734,sizeof(char),5)));}
break;case Cyc_Absyn_Short_sz: _LL122:
{const char*_tmp735;ts=((_tmp735="short",_tag_dyneither(_tmp735,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz: _LL127:
{const char*_tmp736;ts=((_tmp736="int",_tag_dyneither(_tmp736,sizeof(char),4)));}break;case Cyc_Absyn_Long_sz: _LL128:
{const char*_tmp737;ts=((_tmp737="long",_tag_dyneither(_tmp737,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL129:
# 803
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL12B:
{const char*_tmp738;ts=((_tmp738="long long",_tag_dyneither(_tmp738,sizeof(char),10)));}break;case Cyc_Cyclone_Vc_c: _LL12C:
{const char*_tmp739;ts=((_tmp739="__int64",_tag_dyneither(_tmp739,sizeof(char),8)));}break;}
# 807
break;}
# 809
{const char*_tmp73E;void*_tmp73D[2];struct Cyc_String_pa_PrintArg_struct _tmp73C;struct Cyc_String_pa_PrintArg_struct _tmp73B;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp73B.tag=0,((_tmp73B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts),((_tmp73C.tag=0,((_tmp73C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns),((_tmp73D[0]=& _tmp73C,((_tmp73D[1]=& _tmp73B,Cyc_aprintf(((_tmp73E="%s%s",_tag_dyneither(_tmp73E,sizeof(char),5))),_tag_dyneither(_tmp73D,sizeof(void*),2)))))))))))))));}
goto _LLD6;}_LLE9: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp17E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp169;if(_tmp17E->tag != 7)goto _LLEB;else{_tmp17F=_tmp17E->f1;}}_LLEA:
# 812
 if(_tmp17F == 0){
const char*_tmp73F;s=Cyc_PP_text(((_tmp73F="float",_tag_dyneither(_tmp73F,sizeof(char),6))));}else{
if(_tmp17F == 1){
const char*_tmp740;s=Cyc_PP_text(((_tmp740="double",_tag_dyneither(_tmp740,sizeof(char),7))));}else{
# 817
const char*_tmp741;s=Cyc_PP_text(((_tmp741="long double",_tag_dyneither(_tmp741,sizeof(char),12))));}}
goto _LLD6;_LLEB: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp180=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp169;if(_tmp180->tag != 10)goto _LLED;else{_tmp181=_tmp180->f1;}}_LLEC:
# 820
{struct Cyc_PP_Doc*_tmp742[2];s=((_tmp742[1]=Cyc_Absynpp_args2doc(_tmp181),((_tmp742[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp742,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD6;_LLED: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp182=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp169;if(_tmp182->tag != 11)goto _LLEF;else{_tmp183=(_tmp182->f1).aggr_info;_tmp184=(_tmp182->f1).targs;}}_LLEE: {
# 823
struct _tuple10 _tmp1DD=Cyc_Absyn_aggr_kinded_name(_tmp183);enum Cyc_Absyn_AggrKind _tmp1DF;struct _tuple0*_tmp1E0;struct _tuple10 _tmp1DE=_tmp1DD;_tmp1DF=_tmp1DE.f1;_tmp1E0=_tmp1DE.f2;
{struct Cyc_PP_Doc*_tmp743[3];s=((_tmp743[2]=Cyc_Absynpp_tps2doc(_tmp184),((_tmp743[1]=Cyc_Absynpp_qvar2doc(_tmp1E0),((_tmp743[0]=Cyc_Absynpp_aggr_kind2doc(_tmp1DF),Cyc_PP_cat(_tag_dyneither(_tmp743,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLD6;}_LLEF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp185=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp169;if(_tmp185->tag != 12)goto _LLF1;else{_tmp186=_tmp185->f1;_tmp187=_tmp185->f2;}}_LLF0:
# 827
{struct Cyc_PP_Doc*_tmp744[4];s=((_tmp744[3]=
# 829
Cyc_Absynpp_rb(),((_tmp744[2]=
# 828
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp187)),((_tmp744[1]=
# 827
Cyc_Absynpp_lb(),((_tmp744[0]=Cyc_Absynpp_aggr_kind2doc(_tmp186),Cyc_PP_cat(_tag_dyneither(_tmp744,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 830
goto _LLD6;_LLF1: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp188=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp169;if(_tmp188->tag != 14)goto _LLF3;else{_tmp189=_tmp188->f1;}}_LLF2:
# 832
{const char*_tmp747;struct Cyc_PP_Doc*_tmp746[4];s=((_tmp746[3]=Cyc_Absynpp_rb(),((_tmp746[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp189)),((_tmp746[1]=Cyc_Absynpp_lb(),((_tmp746[0]=Cyc_PP_text(((_tmp747="enum ",_tag_dyneither(_tmp747,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp746,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLD6;_LLF3: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp18A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp169;if(_tmp18A->tag != 13)goto _LLF5;else{_tmp18B=_tmp18A->f1;}}_LLF4:
# 834
{const char*_tmp74A;struct Cyc_PP_Doc*_tmp749[2];s=((_tmp749[1]=Cyc_Absynpp_qvar2doc(_tmp18B),((_tmp749[0]=Cyc_PP_text(((_tmp74A="enum ",_tag_dyneither(_tmp74A,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp749,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLD6;_LLF5: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp18C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp169;if(_tmp18C->tag != 18)goto _LLF7;else{_tmp18D=_tmp18C->f1;}}_LLF6:
# 836
{const char*_tmp74F;const char*_tmp74E;struct Cyc_PP_Doc*_tmp74D[3];s=((_tmp74D[2]=Cyc_PP_text(((_tmp74E=")",_tag_dyneither(_tmp74E,sizeof(char),2)))),((_tmp74D[1]=Cyc_Absynpp_exp2doc(_tmp18D),((_tmp74D[0]=Cyc_PP_text(((_tmp74F="valueof_t(",_tag_dyneither(_tmp74F,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp74D,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LLF7: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp18E=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp169;if(_tmp18E->tag != 27)goto _LLF9;else{_tmp18F=_tmp18E->f1;}}_LLF8:
# 838
{const char*_tmp754;const char*_tmp753;struct Cyc_PP_Doc*_tmp752[3];s=((_tmp752[2]=Cyc_PP_text(((_tmp753=")",_tag_dyneither(_tmp753,sizeof(char),2)))),((_tmp752[1]=Cyc_Absynpp_exp2doc(_tmp18F),((_tmp752[0]=Cyc_PP_text(((_tmp754="typeof(",_tag_dyneither(_tmp754,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp752,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LLF9: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp190=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp169;if(_tmp190->tag != 17)goto _LLFB;else{_tmp191=_tmp190->f1;_tmp192=_tmp190->f2;_tmp193=_tmp190->f3;}}_LLFA:
# 845
{struct Cyc_PP_Doc*_tmp755[2];s=((_tmp755[1]=Cyc_Absynpp_tps2doc(_tmp192),((_tmp755[0]=Cyc_Absynpp_qvar2doc(_tmp191),Cyc_PP_cat(_tag_dyneither(_tmp755,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD6;_LLFB: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp194=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp169;if(_tmp194->tag != 15)goto _LLFD;else{_tmp195=(void*)_tmp194->f1;}}_LLFC:
# 848
{const char*_tmp75A;const char*_tmp759;struct Cyc_PP_Doc*_tmp758[3];s=((_tmp758[2]=Cyc_PP_text(((_tmp759=">",_tag_dyneither(_tmp759,sizeof(char),2)))),((_tmp758[1]=Cyc_Absynpp_rgn2doc(_tmp195),((_tmp758[0]=Cyc_PP_text(((_tmp75A="region_t<",_tag_dyneither(_tmp75A,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp758,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LLFD: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp196=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp169;if(_tmp196->tag != 16)goto _LLFF;else{_tmp197=(void*)_tmp196->f1;_tmp198=(void*)_tmp196->f2;}}_LLFE:
# 850
{const char*_tmp761;const char*_tmp760;const char*_tmp75F;struct Cyc_PP_Doc*_tmp75E[5];s=((_tmp75E[4]=
Cyc_PP_text(((_tmp75F=">",_tag_dyneither(_tmp75F,sizeof(char),2)))),((_tmp75E[3]=
# 850
Cyc_Absynpp_rgn2doc(_tmp198),((_tmp75E[2]=Cyc_PP_text(((_tmp760=",",_tag_dyneither(_tmp760,sizeof(char),2)))),((_tmp75E[1]=Cyc_Absynpp_rgn2doc(_tmp197),((_tmp75E[0]=Cyc_PP_text(((_tmp761="dynregion_t<",_tag_dyneither(_tmp761,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp75E,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LLD6;_LLFF: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp199=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp169;if(_tmp199->tag != 19)goto _LL101;else{_tmp19A=(void*)_tmp199->f1;}}_LL100:
# 853
{const char*_tmp766;const char*_tmp765;struct Cyc_PP_Doc*_tmp764[3];s=((_tmp764[2]=Cyc_PP_text(((_tmp765=">",_tag_dyneither(_tmp765,sizeof(char),2)))),((_tmp764[1]=Cyc_Absynpp_typ2doc(_tmp19A),((_tmp764[0]=Cyc_PP_text(((_tmp766="tag_t<",_tag_dyneither(_tmp766,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp764,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LL101: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp19B=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp169;if(_tmp19B->tag != 21)goto _LL103;}_LL102:
 goto _LL104;_LL103: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp19C=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp169;if(_tmp19C->tag != 20)goto _LL105;}_LL104:
 goto _LL106;_LL105: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp19D=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp169;if(_tmp19D->tag != 22)goto _LL107;}_LL106:
# 857
 s=Cyc_Absynpp_rgn2doc(t);goto _LLD6;_LL107: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp19E=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp169;if(_tmp19E->tag != 25)goto _LL109;else{_tmp19F=(void*)_tmp19E->f1;}}_LL108:
# 859
{const char*_tmp76B;const char*_tmp76A;struct Cyc_PP_Doc*_tmp769[3];s=((_tmp769[2]=Cyc_PP_text(((_tmp76A=")",_tag_dyneither(_tmp76A,sizeof(char),2)))),((_tmp769[1]=Cyc_Absynpp_typ2doc(_tmp19F),((_tmp769[0]=Cyc_PP_text(((_tmp76B="regions(",_tag_dyneither(_tmp76B,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp769,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LL109: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1A0=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp169;if(_tmp1A0->tag != 23)goto _LL10B;}_LL10A:
 goto _LL10C;_LL10B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1A1=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp169;if(_tmp1A1->tag != 24)goto _LL10D;}_LL10C:
# 862
 s=Cyc_Absynpp_eff2doc(t);goto _LLD6;_LL10D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1A2=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp169;if(_tmp1A2->tag != 26)goto _LL10F;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp1A3=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp1A2->f1)->r;if(_tmp1A3->tag != 0)goto _LL10F;else{_tmp1A4=_tmp1A3->f1;}}}_LL10E:
# 864
 s=Cyc_Absynpp_aggrdecl2doc(_tmp1A4);goto _LLD6;_LL10F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1A5=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp169;if(_tmp1A5->tag != 26)goto _LL111;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp1A6=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp1A5->f1)->r;if(_tmp1A6->tag != 1)goto _LL111;else{_tmp1A7=_tmp1A6->f1;}}}_LL110:
# 866
 s=Cyc_Absynpp_enumdecl2doc(_tmp1A7);goto _LLD6;_LL111: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1A8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp169;if(_tmp1A8->tag != 26)goto _LLD6;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp1A9=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp1A8->f1)->r;if(_tmp1A9->tag != 2)goto _LLD6;else{_tmp1AA=_tmp1A9->f1;}}}_LL112:
# 868
 s=Cyc_Absynpp_datatypedecl2doc(_tmp1AA);goto _LLD6;_LLD6:;}
# 870
return s;}
# 873
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _dyneither_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple16{void*f1;void*f2;};
# 877
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
void*_tmp1FC;void*_tmp1FD;struct _tuple16*_tmp1FB=cmp;_tmp1FC=_tmp1FB->f1;_tmp1FD=_tmp1FB->f2;{
const char*_tmp76E;struct Cyc_PP_Doc*_tmp76D[3];return(_tmp76D[2]=Cyc_Absynpp_rgn2doc(_tmp1FD),((_tmp76D[1]=Cyc_PP_text(((_tmp76E=" > ",_tag_dyneither(_tmp76E,sizeof(char),4)))),((_tmp76D[0]=Cyc_Absynpp_rgn2doc(_tmp1FC),Cyc_PP_cat(_tag_dyneither(_tmp76D,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 882
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
const char*_tmp771;const char*_tmp770;const char*_tmp76F;return Cyc_PP_group(((_tmp76F="",_tag_dyneither(_tmp76F,sizeof(char),1))),((_tmp770="",_tag_dyneither(_tmp770,sizeof(char),1))),((_tmp771=",",_tag_dyneither(_tmp771,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
# 886
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _dyneither_ptr*_tmp203=(*t).f1;
struct Cyc_Core_Opt*_tmp772;struct Cyc_Core_Opt*dopt=_tmp203 == 0?0:((_tmp772=_cycalloc(sizeof(*_tmp772)),((_tmp772->v=Cyc_PP_text(*_tmp203),_tmp772))));
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 892
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 896
struct Cyc_List_List*_tmp205=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs){
const char*_tmp775;struct Cyc_List_List*_tmp774;_tmp205=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp205,((_tmp774=_cycalloc(sizeof(*_tmp774)),((_tmp774->hd=Cyc_PP_text(((_tmp775="...",_tag_dyneither(_tmp775,sizeof(char),4)))),((_tmp774->tl=0,_tmp774)))))));}else{
if(cyc_varargs != 0){
const char*_tmp77E;const char*_tmp77D;const char*_tmp77C;struct _tuple8*_tmp77B;struct Cyc_PP_Doc*_tmp77A[3];struct Cyc_PP_Doc*_tmp208=(_tmp77A[2]=
# 903
Cyc_Absynpp_funarg2doc(((_tmp77B=_cycalloc(sizeof(*_tmp77B)),((_tmp77B->f1=cyc_varargs->name,((_tmp77B->f2=cyc_varargs->tq,((_tmp77B->f3=cyc_varargs->type,_tmp77B))))))))),((_tmp77A[1]=
# 902
cyc_varargs->inject?Cyc_PP_text(((_tmp77C=" inject ",_tag_dyneither(_tmp77C,sizeof(char),9)))): Cyc_PP_text(((_tmp77D=" ",_tag_dyneither(_tmp77D,sizeof(char),2)))),((_tmp77A[0]=
# 901
Cyc_PP_text(((_tmp77E="...",_tag_dyneither(_tmp77E,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp77A,sizeof(struct Cyc_PP_Doc*),3)))))));
# 905
struct Cyc_List_List*_tmp77F;_tmp205=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp205,((_tmp77F=_cycalloc(sizeof(*_tmp77F)),((_tmp77F->hd=_tmp208,((_tmp77F->tl=0,_tmp77F)))))));}}{
# 907
const char*_tmp782;const char*_tmp781;const char*_tmp780;struct Cyc_PP_Doc*_tmp20F=Cyc_PP_group(((_tmp780="",_tag_dyneither(_tmp780,sizeof(char),1))),((_tmp781="",_tag_dyneither(_tmp781,sizeof(char),1))),((_tmp782=",",_tag_dyneither(_tmp782,sizeof(char),2))),_tmp205);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){
const char*_tmp785;struct Cyc_PP_Doc*_tmp784[3];_tmp20F=((_tmp784[2]=Cyc_Absynpp_eff2doc(effopt),((_tmp784[1]=Cyc_PP_text(((_tmp785=";",_tag_dyneither(_tmp785,sizeof(char),2)))),((_tmp784[0]=_tmp20F,Cyc_PP_cat(_tag_dyneither(_tmp784,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){
const char*_tmp788;struct Cyc_PP_Doc*_tmp787[3];_tmp20F=((_tmp787[2]=Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp787[1]=Cyc_PP_text(((_tmp788=":",_tag_dyneither(_tmp788,sizeof(char),2)))),((_tmp787[0]=_tmp20F,Cyc_PP_cat(_tag_dyneither(_tmp787,sizeof(struct Cyc_PP_Doc*),3))))))));}{
const char*_tmp78D;const char*_tmp78C;struct Cyc_PP_Doc*_tmp78B[3];struct Cyc_PP_Doc*_tmp214=(_tmp78B[2]=Cyc_PP_text(((_tmp78C=")",_tag_dyneither(_tmp78C,sizeof(char),2)))),((_tmp78B[1]=_tmp20F,((_tmp78B[0]=Cyc_PP_text(((_tmp78D="(",_tag_dyneither(_tmp78D,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp78B,sizeof(struct Cyc_PP_Doc*),3)))))));
if(req != 0){
const char*_tmp792;const char*_tmp791;struct Cyc_PP_Doc*_tmp790[4];_tmp214=((_tmp790[3]=Cyc_PP_text(((_tmp791=")",_tag_dyneither(_tmp791,sizeof(char),2)))),((_tmp790[2]=Cyc_Absynpp_exp2doc(req),((_tmp790[1]=Cyc_PP_text(((_tmp792=" @requires(",_tag_dyneither(_tmp792,sizeof(char),12)))),((_tmp790[0]=_tmp214,Cyc_PP_cat(_tag_dyneither(_tmp790,sizeof(struct Cyc_PP_Doc*),4))))))))));}
if(ens != 0){
const char*_tmp797;const char*_tmp796;struct Cyc_PP_Doc*_tmp795[4];_tmp214=((_tmp795[3]=Cyc_PP_text(((_tmp796=")",_tag_dyneither(_tmp796,sizeof(char),2)))),((_tmp795[2]=Cyc_Absynpp_exp2doc(ens),((_tmp795[1]=Cyc_PP_text(((_tmp797=" @ensures(",_tag_dyneither(_tmp797,sizeof(char),11)))),((_tmp795[0]=_tmp214,Cyc_PP_cat(_tag_dyneither(_tmp795,sizeof(struct Cyc_PP_Doc*),4))))))))));}
return _tmp214;};};}
# 920
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg){
struct _dyneither_ptr*_tmp222;struct Cyc_Absyn_Tqual _tmp223;void*_tmp224;struct _tuple8*_tmp221=arg;_tmp222=_tmp221->f1;_tmp223=_tmp221->f2;_tmp224=_tmp221->f3;{
struct _tuple8*_tmp798;return(_tmp798=_cycalloc(sizeof(*_tmp798)),((_tmp798->f1=_tmp222,((_tmp798->f2=_tmp223,((_tmp798->f3=_tmp224,_tmp798)))))));};}
# 925
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){return Cyc_PP_text(*v);}
# 927
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp226=0;
int match;
{union Cyc_Absyn_Nmspace _tmp227=(*q).f1;union Cyc_Absyn_Nmspace _tmp228=_tmp227;struct Cyc_List_List*_tmp229;struct Cyc_List_List*_tmp22A;struct Cyc_List_List*_tmp22B;_LL12F: if((_tmp228.Loc_n).tag != 4)goto _LL131;_LL130:
# 932
 _tmp229=0;goto _LL132;_LL131: if((_tmp228.Rel_n).tag != 1)goto _LL133;_tmp229=(struct Cyc_List_List*)(_tmp228.Rel_n).val;_LL132:
# 934
 match=0;
_tmp226=_tmp229;
goto _LL12E;_LL133: if((_tmp228.C_n).tag != 3)goto _LL135;_tmp22A=(struct Cyc_List_List*)(_tmp228.C_n).val;_LL134:
# 938
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp22A,Cyc_Absynpp_curr_namespace);
# 940
goto _LL12E;_LL135: if((_tmp228.Abs_n).tag != 2)goto _LL12E;_tmp22B=(struct Cyc_List_List*)(_tmp228.Abs_n).val;_LL136:
# 942
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp22B,Cyc_Absynpp_curr_namespace);
{struct Cyc_List_List*_tmp799;_tmp226=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(_tmp799=_cycalloc(sizeof(*_tmp799)),((_tmp799->hd=Cyc_Absynpp_cyc_stringptr,((_tmp799->tl=_tmp22B,_tmp799))))): _tmp22B;}
goto _LL12E;_LL12E:;}
# 946
if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp79B;struct Cyc_List_List*_tmp79A;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp226,(
(_tmp79A=_cycalloc(sizeof(*_tmp79A)),((_tmp79A->hd=(*q).f2,((_tmp79A->tl=0,_tmp79A))))))),((_tmp79B="_",_tag_dyneither(_tmp79B,sizeof(char),2))));}else{
# 951
if(match)
return*(*q).f2;else{
# 954
const char*_tmp79D;struct Cyc_List_List*_tmp79C;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp226,(
(_tmp79C=_cycalloc(sizeof(*_tmp79C)),((_tmp79C->hd=(*q).f2,((_tmp79C->tl=0,_tmp79C))))))),((_tmp79D="::",_tag_dyneither(_tmp79D,sizeof(char),3))));}}}
# 959
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 963
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp231=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output){
# 967
const char*_tmp79F;const char*_tmp79E;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp79E="\\textbf{",_tag_dyneither(_tmp79E,sizeof(char),9))),(struct _dyneither_ptr)_tmp231),((_tmp79F="}",_tag_dyneither(_tmp79F,sizeof(char),2)))),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp231));}else{
return Cyc_PP_text(_tmp231);}}
# 972
struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 974
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 977
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp234=(*v).f1;union Cyc_Absyn_Nmspace _tmp235=_tmp234;struct Cyc_List_List*_tmp236;struct Cyc_List_List*_tmp237;_LL138: if((_tmp235.Loc_n).tag != 4)goto _LL13A;_LL139:
 goto _LL13B;_LL13A: if((_tmp235.Rel_n).tag != 1)goto _LL13C;if((struct Cyc_List_List*)(_tmp235.Rel_n).val != 0)goto _LL13C;_LL13B:
 return*(*v).f2;_LL13C: if((_tmp235.C_n).tag != 3)goto _LL13E;_tmp236=(struct Cyc_List_List*)(_tmp235.C_n).val;_LL13D:
 _tmp237=_tmp236;goto _LL13F;_LL13E: if((_tmp235.Abs_n).tag != 2)goto _LL140;_tmp237=(struct Cyc_List_List*)(_tmp235.Abs_n).val;_LL13F:
# 983
 if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp237,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 986
goto _LL141;}_LL140:;_LL141: {
# 988
const char*_tmp7A0;return(struct _dyneither_ptr)Cyc_strconcat(((_tmp7A0="/* bad namespace : */ ",_tag_dyneither(_tmp7A0,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));}_LL137:;}else{
# 991
return*(*v).f2;}}
# 994
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 997
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _dyneither_ptr _tmp239=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output){
# 1001
const char*_tmp7A2;const char*_tmp7A1;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp7A1="\\textbf{",_tag_dyneither(_tmp7A1,sizeof(char),9))),(struct _dyneither_ptr)_tmp239),((_tmp7A2="}",_tag_dyneither(_tmp7A2,sizeof(char),2)))),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp239));}else{
return Cyc_PP_text(_tmp239);}}
# 1006
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}
# 1017 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp23C=e->r;void*_tmp23D=_tmp23C;enum Cyc_Absyn_Primop _tmp241;struct Cyc_Absyn_Exp*_tmp24B;struct Cyc_Absyn_Exp*_tmp24D;_LL143: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp23E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp23E->tag != 0)goto _LL145;}_LL144:
 goto _LL146;_LL145: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp23F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp23F->tag != 1)goto _LL147;}_LL146:
 return 10000;_LL147: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp240=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp240->tag != 2)goto _LL149;else{_tmp241=_tmp240->f1;}}_LL148:
# 1022
 switch(_tmp241){case Cyc_Absyn_Plus: _LL193:
 return 100;case Cyc_Absyn_Times: _LL194:
 return 110;case Cyc_Absyn_Minus: _LL195:
 return 100;case Cyc_Absyn_Div: _LL196:
 goto _LL197;case Cyc_Absyn_Mod: _LL197:
 return 110;case Cyc_Absyn_Eq: _LL198:
 goto _LL199;case Cyc_Absyn_Neq: _LL199:
 return 70;case Cyc_Absyn_Gt: _LL19A:
 goto _LL19B;case Cyc_Absyn_Lt: _LL19B:
 goto _LL19C;case Cyc_Absyn_Gte: _LL19C:
 goto _LL19D;case Cyc_Absyn_Lte: _LL19D:
 return 80;case Cyc_Absyn_Not: _LL19E:
 goto _LL19F;case Cyc_Absyn_Bitnot: _LL19F:
 return 130;case Cyc_Absyn_Bitand: _LL1A0:
 return 60;case Cyc_Absyn_Bitor: _LL1A1:
 return 40;case Cyc_Absyn_Bitxor: _LL1A2:
 return 50;case Cyc_Absyn_Bitlshift: _LL1A3:
 return 90;case Cyc_Absyn_Bitlrshift: _LL1A4:
 return 80;case Cyc_Absyn_Bitarshift: _LL1A5:
 return 80;case Cyc_Absyn_Numelts: _LL1A6:
 return 140;}_LL149: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp242=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp242->tag != 3)goto _LL14B;}_LL14A:
# 1044
 return 20;_LL14B: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp243=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp243->tag != 4)goto _LL14D;}_LL14C:
 return 130;_LL14D: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp244=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp244->tag != 5)goto _LL14F;}_LL14E:
 return 30;_LL14F: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp245=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp245->tag != 6)goto _LL151;}_LL150:
 return 35;_LL151: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp246=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp246->tag != 7)goto _LL153;}_LL152:
 return 30;_LL153: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp247=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp247->tag != 8)goto _LL155;}_LL154:
 return 10;_LL155: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp248=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp248->tag != 9)goto _LL157;}_LL156:
 return 140;_LL157: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp249=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp249->tag != 10)goto _LL159;}_LL158:
 return 130;_LL159: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp24A=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp24A->tag != 11)goto _LL15B;else{_tmp24B=_tmp24A->f1;}}_LL15A:
 return Cyc_Absynpp_exp_prec(_tmp24B);_LL15B: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp24C=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp24C->tag != 12)goto _LL15D;else{_tmp24D=_tmp24C->f1;}}_LL15C:
 return Cyc_Absynpp_exp_prec(_tmp24D);_LL15D: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp24E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp24E->tag != 13)goto _LL15F;}_LL15E:
 return 120;_LL15F: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp24F=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp24F->tag != 15)goto _LL161;}_LL160:
 return 15;_LL161: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp250=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp250->tag != 14)goto _LL163;}_LL162:
 goto _LL164;_LL163: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp251=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp251->tag != 16)goto _LL165;}_LL164:
 goto _LL166;_LL165: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp252=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp252->tag != 17)goto _LL167;}_LL166:
 goto _LL168;_LL167: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp253=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp253->tag != 38)goto _LL169;}_LL168:
 goto _LL16A;_LL169: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp254=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp254->tag != 39)goto _LL16B;}_LL16A:
 goto _LL16C;_LL16B: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp255=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp255->tag != 37)goto _LL16D;}_LL16C:
 goto _LL16E;_LL16D: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp256=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp256->tag != 18)goto _LL16F;}_LL16E:
 goto _LL170;_LL16F: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp257=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp257->tag != 19)goto _LL171;}_LL170:
 return 130;_LL171: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp258=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp258->tag != 20)goto _LL173;}_LL172:
 goto _LL174;_LL173: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp259=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp259->tag != 21)goto _LL175;}_LL174:
 goto _LL176;_LL175: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp25A=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp25A->tag != 22)goto _LL177;}_LL176:
 return 140;_LL177: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp25B=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp25B->tag != 23)goto _LL179;}_LL178:
 return 150;_LL179: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp25C=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp25C->tag != 24)goto _LL17B;}_LL17A:
 goto _LL17C;_LL17B: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp25D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp25D->tag != 25)goto _LL17D;}_LL17C:
 goto _LL17E;_LL17D: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp25E=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp25E->tag != 26)goto _LL17F;}_LL17E:
 goto _LL180;_LL17F: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp25F=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp25F->tag != 27)goto _LL181;}_LL180:
 goto _LL182;_LL181: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp260=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp260->tag != 28)goto _LL183;}_LL182:
 goto _LL184;_LL183: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp261=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp261->tag != 29)goto _LL185;}_LL184:
 goto _LL186;_LL185: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp262=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp262->tag != 30)goto _LL187;}_LL186:
 goto _LL188;_LL187: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp263=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp263->tag != 31)goto _LL189;}_LL188:
 goto _LL18A;_LL189: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp264=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp264->tag != 32)goto _LL18B;}_LL18A:
 goto _LL18C;_LL18B: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp265=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp265->tag != 33)goto _LL18D;}_LL18C:
 goto _LL18E;_LL18D: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp266=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp266->tag != 34)goto _LL18F;}_LL18E:
 goto _LL190;_LL18F: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp267=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp267->tag != 35)goto _LL191;}_LL190:
 return 140;_LL191: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp268=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp268->tag != 36)goto _LL142;}_LL192:
 return 10000;_LL142:;}
# 1084
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1088
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp269=e->r;void*_tmp26A=_tmp269;union Cyc_Absyn_Cnst _tmp26C;struct _tuple0*_tmp26E;enum Cyc_Absyn_Primop _tmp270;struct Cyc_List_List*_tmp271;struct Cyc_Absyn_Exp*_tmp273;struct Cyc_Core_Opt*_tmp274;struct Cyc_Absyn_Exp*_tmp275;struct Cyc_Absyn_Exp*_tmp277;enum Cyc_Absyn_Incrementor _tmp278;struct Cyc_Absyn_Exp*_tmp27A;struct Cyc_Absyn_Exp*_tmp27B;struct Cyc_Absyn_Exp*_tmp27C;struct Cyc_Absyn_Exp*_tmp27E;struct Cyc_Absyn_Exp*_tmp27F;struct Cyc_Absyn_Exp*_tmp281;struct Cyc_Absyn_Exp*_tmp282;struct Cyc_Absyn_Exp*_tmp284;struct Cyc_Absyn_Exp*_tmp285;struct Cyc_Absyn_Exp*_tmp287;struct Cyc_List_List*_tmp288;struct Cyc_Absyn_Exp*_tmp28A;struct Cyc_Absyn_Exp*_tmp28C;struct Cyc_Absyn_Exp*_tmp28E;void*_tmp290;struct Cyc_Absyn_Exp*_tmp291;struct Cyc_Absyn_Exp*_tmp293;struct Cyc_Absyn_Exp*_tmp295;struct Cyc_Absyn_Exp*_tmp296;void*_tmp298;struct Cyc_Absyn_Exp*_tmp29A;void*_tmp29C;int _tmp29E;struct _dyneither_ptr _tmp29F;struct Cyc_Absyn_Exp*_tmp2A1;struct _dyneither_ptr*_tmp2A2;void*_tmp2A4;struct Cyc_List_List*_tmp2A5;struct Cyc_Absyn_Exp*_tmp2A7;struct Cyc_Absyn_Exp*_tmp2A9;struct _dyneither_ptr*_tmp2AA;struct Cyc_Absyn_Exp*_tmp2AC;struct _dyneither_ptr*_tmp2AD;struct Cyc_Absyn_Exp*_tmp2AF;struct Cyc_Absyn_Exp*_tmp2B0;struct Cyc_List_List*_tmp2B2;struct _tuple8*_tmp2B4;struct Cyc_List_List*_tmp2B5;struct Cyc_List_List*_tmp2B7;struct Cyc_Absyn_Vardecl*_tmp2B9;struct Cyc_Absyn_Exp*_tmp2BA;struct Cyc_Absyn_Exp*_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BD;void*_tmp2BE;struct _tuple0*_tmp2C0;struct Cyc_List_List*_tmp2C1;struct Cyc_List_List*_tmp2C2;struct Cyc_List_List*_tmp2C4;struct Cyc_List_List*_tmp2C6;struct Cyc_Absyn_Datatypefield*_tmp2C7;struct _tuple0*_tmp2C9;struct _tuple0*_tmp2CB;int _tmp2CD;struct Cyc_Absyn_Exp*_tmp2CE;void**_tmp2CF;struct Cyc_Absyn_Exp*_tmp2D0;struct Cyc_Absyn_Exp*_tmp2D2;struct Cyc_Absyn_Exp*_tmp2D3;struct Cyc_Core_Opt*_tmp2D5;struct Cyc_List_List*_tmp2D6;struct Cyc_Absyn_Stmt*_tmp2D8;_LL1A9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp26B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp26B->tag != 0)goto _LL1AB;else{_tmp26C=_tmp26B->f1;}}_LL1AA:
 s=Cyc_Absynpp_cnst2doc(_tmp26C);goto _LL1A8;_LL1AB: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp26D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp26D->tag != 1)goto _LL1AD;else{_tmp26E=_tmp26D->f1;}}_LL1AC:
 s=Cyc_Absynpp_qvar2doc(_tmp26E);goto _LL1A8;_LL1AD: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp26F=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp26F->tag != 2)goto _LL1AF;else{_tmp270=_tmp26F->f1;_tmp271=_tmp26F->f2;}}_LL1AE:
 s=Cyc_Absynpp_primapp2doc(myprec,_tmp270,_tmp271);goto _LL1A8;_LL1AF: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp272=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp272->tag != 3)goto _LL1B1;else{_tmp273=_tmp272->f1;_tmp274=_tmp272->f2;_tmp275=_tmp272->f3;}}_LL1B0:
# 1096
{const char*_tmp7A7;const char*_tmp7A6;struct Cyc_PP_Doc*_tmp7A5[5];s=((_tmp7A5[4]=
# 1100
Cyc_Absynpp_exp2doc_prec(myprec,_tmp275),((_tmp7A5[3]=
# 1099
Cyc_PP_text(((_tmp7A6="= ",_tag_dyneither(_tmp7A6,sizeof(char),3)))),((_tmp7A5[2]=
# 1098
_tmp274 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp274->v),((_tmp7A5[1]=
# 1097
Cyc_PP_text(((_tmp7A7=" ",_tag_dyneither(_tmp7A7,sizeof(char),2)))),((_tmp7A5[0]=
# 1096
Cyc_Absynpp_exp2doc_prec(myprec,_tmp273),Cyc_PP_cat(_tag_dyneither(_tmp7A5,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1101
goto _LL1A8;_LL1B1: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp276=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp276->tag != 4)goto _LL1B3;else{_tmp277=_tmp276->f1;_tmp278=_tmp276->f2;}}_LL1B2: {
# 1103
struct Cyc_PP_Doc*_tmp2DC=Cyc_Absynpp_exp2doc_prec(myprec,_tmp277);
switch(_tmp278){case Cyc_Absyn_PreInc: _LL1F9:
{const char*_tmp7AA;struct Cyc_PP_Doc*_tmp7A9[2];s=((_tmp7A9[1]=_tmp2DC,((_tmp7A9[0]=Cyc_PP_text(((_tmp7AA="++",_tag_dyneither(_tmp7AA,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp7A9,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PreDec: _LL1FA:
{const char*_tmp7AD;struct Cyc_PP_Doc*_tmp7AC[2];s=((_tmp7AC[1]=_tmp2DC,((_tmp7AC[0]=Cyc_PP_text(((_tmp7AD="--",_tag_dyneither(_tmp7AD,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp7AC,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostInc: _LL1FB:
{const char*_tmp7B0;struct Cyc_PP_Doc*_tmp7AF[2];s=((_tmp7AF[1]=Cyc_PP_text(((_tmp7B0="++",_tag_dyneither(_tmp7B0,sizeof(char),3)))),((_tmp7AF[0]=_tmp2DC,Cyc_PP_cat(_tag_dyneither(_tmp7AF,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1FC:
{const char*_tmp7B3;struct Cyc_PP_Doc*_tmp7B2[2];s=((_tmp7B2[1]=Cyc_PP_text(((_tmp7B3="--",_tag_dyneither(_tmp7B3,sizeof(char),3)))),((_tmp7B2[0]=_tmp2DC,Cyc_PP_cat(_tag_dyneither(_tmp7B2,sizeof(struct Cyc_PP_Doc*),2))))));}break;}
# 1110
goto _LL1A8;}_LL1B3: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp279=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp279->tag != 5)goto _LL1B5;else{_tmp27A=_tmp279->f1;_tmp27B=_tmp279->f2;_tmp27C=_tmp279->f3;}}_LL1B4:
# 1112
{const char*_tmp7B8;const char*_tmp7B7;struct Cyc_PP_Doc*_tmp7B6[5];s=((_tmp7B6[4]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp27C),((_tmp7B6[3]=Cyc_PP_text(((_tmp7B7=" : ",_tag_dyneither(_tmp7B7,sizeof(char),4)))),((_tmp7B6[2]=
# 1112
Cyc_Absynpp_exp2doc_prec(0,_tmp27B),((_tmp7B6[1]=Cyc_PP_text(((_tmp7B8=" ? ",_tag_dyneither(_tmp7B8,sizeof(char),4)))),((_tmp7B6[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27A),Cyc_PP_cat(_tag_dyneither(_tmp7B6,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1114
goto _LL1A8;_LL1B5: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp27D=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp27D->tag != 6)goto _LL1B7;else{_tmp27E=_tmp27D->f1;_tmp27F=_tmp27D->f2;}}_LL1B6:
# 1116
{const char*_tmp7BB;struct Cyc_PP_Doc*_tmp7BA[3];s=((_tmp7BA[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27F),((_tmp7BA[1]=Cyc_PP_text(((_tmp7BB=" && ",_tag_dyneither(_tmp7BB,sizeof(char),5)))),((_tmp7BA[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27E),Cyc_PP_cat(_tag_dyneither(_tmp7BA,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A8;_LL1B7: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp280=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp280->tag != 7)goto _LL1B9;else{_tmp281=_tmp280->f1;_tmp282=_tmp280->f2;}}_LL1B8:
# 1119
{const char*_tmp7BE;struct Cyc_PP_Doc*_tmp7BD[3];s=((_tmp7BD[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp282),((_tmp7BD[1]=Cyc_PP_text(((_tmp7BE=" || ",_tag_dyneither(_tmp7BE,sizeof(char),5)))),((_tmp7BD[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp281),Cyc_PP_cat(_tag_dyneither(_tmp7BD,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A8;_LL1B9: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp283=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp283->tag != 8)goto _LL1BB;else{_tmp284=_tmp283->f1;_tmp285=_tmp283->f2;}}_LL1BA:
# 1124
{const char*_tmp7C5;const char*_tmp7C4;const char*_tmp7C3;struct Cyc_PP_Doc*_tmp7C2[5];s=((_tmp7C2[4]=Cyc_PP_text(((_tmp7C3=")",_tag_dyneither(_tmp7C3,sizeof(char),2)))),((_tmp7C2[3]=Cyc_Absynpp_exp2doc(_tmp285),((_tmp7C2[2]=Cyc_PP_text(((_tmp7C4=", ",_tag_dyneither(_tmp7C4,sizeof(char),3)))),((_tmp7C2[1]=Cyc_Absynpp_exp2doc(_tmp284),((_tmp7C2[0]=Cyc_PP_text(((_tmp7C5="(",_tag_dyneither(_tmp7C5,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7C2,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1A8;_LL1BB: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp286=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp286->tag != 9)goto _LL1BD;else{_tmp287=_tmp286->f1;_tmp288=_tmp286->f2;}}_LL1BC:
# 1127
{const char*_tmp7CA;const char*_tmp7C9;struct Cyc_PP_Doc*_tmp7C8[4];s=((_tmp7C8[3]=
# 1130
Cyc_PP_text(((_tmp7C9=")",_tag_dyneither(_tmp7C9,sizeof(char),2)))),((_tmp7C8[2]=
# 1129
Cyc_Absynpp_exps2doc_prec(20,_tmp288),((_tmp7C8[1]=
# 1128
Cyc_PP_text(((_tmp7CA="(",_tag_dyneither(_tmp7CA,sizeof(char),2)))),((_tmp7C8[0]=
# 1127
Cyc_Absynpp_exp2doc_prec(myprec,_tmp287),Cyc_PP_cat(_tag_dyneither(_tmp7C8,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1131
goto _LL1A8;_LL1BD: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp289=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp289->tag != 10)goto _LL1BF;else{_tmp28A=_tmp289->f1;}}_LL1BE:
# 1133
{const char*_tmp7CD;struct Cyc_PP_Doc*_tmp7CC[2];s=((_tmp7CC[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp28A),((_tmp7CC[0]=Cyc_PP_text(((_tmp7CD="throw ",_tag_dyneither(_tmp7CD,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp7CC,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1A8;_LL1BF: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp28B=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp28B->tag != 11)goto _LL1C1;else{_tmp28C=_tmp28B->f1;}}_LL1C0:
# 1136
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp28C);
goto _LL1A8;_LL1C1: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp28D=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp28D->tag != 12)goto _LL1C3;else{_tmp28E=_tmp28D->f1;}}_LL1C2:
# 1139
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp28E);
goto _LL1A8;_LL1C3: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp28F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp28F->tag != 13)goto _LL1C5;else{_tmp290=(void*)_tmp28F->f1;_tmp291=_tmp28F->f2;}}_LL1C4:
# 1142
{const char*_tmp7D2;const char*_tmp7D1;struct Cyc_PP_Doc*_tmp7D0[4];s=((_tmp7D0[3]=
# 1145
Cyc_Absynpp_exp2doc_prec(myprec,_tmp291),((_tmp7D0[2]=
# 1144
Cyc_PP_text(((_tmp7D1=")",_tag_dyneither(_tmp7D1,sizeof(char),2)))),((_tmp7D0[1]=
# 1143
Cyc_Absynpp_typ2doc(_tmp290),((_tmp7D0[0]=
# 1142
Cyc_PP_text(((_tmp7D2="(",_tag_dyneither(_tmp7D2,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7D0,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1146
goto _LL1A8;_LL1C5: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp292=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp292->tag != 14)goto _LL1C7;else{_tmp293=_tmp292->f1;}}_LL1C6:
# 1148
{const char*_tmp7D5;struct Cyc_PP_Doc*_tmp7D4[2];s=((_tmp7D4[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp293),((_tmp7D4[0]=
# 1148
Cyc_PP_text(((_tmp7D5="&",_tag_dyneither(_tmp7D5,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7D4,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1150
goto _LL1A8;_LL1C7: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp294=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp294->tag != 15)goto _LL1C9;else{_tmp295=_tmp294->f1;_tmp296=_tmp294->f2;}}_LL1C8:
# 1152
 if(_tmp295 == 0){
const char*_tmp7D8;struct Cyc_PP_Doc*_tmp7D7[2];s=((_tmp7D7[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp296),((_tmp7D7[0]=Cyc_PP_text(((_tmp7D8="new ",_tag_dyneither(_tmp7D8,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp7D7,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 1155
const char*_tmp7DD;const char*_tmp7DC;struct Cyc_PP_Doc*_tmp7DB[4];s=((_tmp7DB[3]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp296),((_tmp7DB[2]=
# 1155
Cyc_PP_text(((_tmp7DC=") ",_tag_dyneither(_tmp7DC,sizeof(char),3)))),((_tmp7DB[1]=Cyc_Absynpp_exp2doc(_tmp295),((_tmp7DB[0]=Cyc_PP_text(((_tmp7DD="rnew(",_tag_dyneither(_tmp7DD,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp7DB,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1157
goto _LL1A8;_LL1C9: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp297=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp297->tag != 16)goto _LL1CB;else{_tmp298=(void*)_tmp297->f1;}}_LL1CA:
{const char*_tmp7E2;const char*_tmp7E1;struct Cyc_PP_Doc*_tmp7E0[3];s=((_tmp7E0[2]=Cyc_PP_text(((_tmp7E1=")",_tag_dyneither(_tmp7E1,sizeof(char),2)))),((_tmp7E0[1]=Cyc_Absynpp_typ2doc(_tmp298),((_tmp7E0[0]=Cyc_PP_text(((_tmp7E2="sizeof(",_tag_dyneither(_tmp7E2,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7E0,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A8;_LL1CB: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp299=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp299->tag != 17)goto _LL1CD;else{_tmp29A=_tmp299->f1;}}_LL1CC:
{const char*_tmp7E7;const char*_tmp7E6;struct Cyc_PP_Doc*_tmp7E5[3];s=((_tmp7E5[2]=Cyc_PP_text(((_tmp7E6=")",_tag_dyneither(_tmp7E6,sizeof(char),2)))),((_tmp7E5[1]=Cyc_Absynpp_exp2doc(_tmp29A),((_tmp7E5[0]=Cyc_PP_text(((_tmp7E7="sizeof(",_tag_dyneither(_tmp7E7,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7E5,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A8;_LL1CD: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp29B=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp29B->tag != 38)goto _LL1CF;else{_tmp29C=(void*)_tmp29B->f1;}}_LL1CE:
{const char*_tmp7EC;const char*_tmp7EB;struct Cyc_PP_Doc*_tmp7EA[3];s=((_tmp7EA[2]=Cyc_PP_text(((_tmp7EB=")",_tag_dyneither(_tmp7EB,sizeof(char),2)))),((_tmp7EA[1]=Cyc_Absynpp_typ2doc(_tmp29C),((_tmp7EA[0]=Cyc_PP_text(((_tmp7EC="valueof(",_tag_dyneither(_tmp7EC,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7EA,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A8;_LL1CF: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp29D=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp29D->tag != 39)goto _LL1D1;else{_tmp29E=_tmp29D->f1;_tmp29F=_tmp29D->f2;}}_LL1D0:
# 1162
 if(_tmp29E){
const char*_tmp7F3;const char*_tmp7F2;const char*_tmp7F1;struct Cyc_PP_Doc*_tmp7F0[4];s=((_tmp7F0[3]=Cyc_PP_text(((_tmp7F1=")",_tag_dyneither(_tmp7F1,sizeof(char),2)))),((_tmp7F0[2]=Cyc_PP_text(_tmp29F),((_tmp7F0[1]=Cyc_PP_text(((_tmp7F2=" volatile (",_tag_dyneither(_tmp7F2,sizeof(char),12)))),((_tmp7F0[0]=Cyc_PP_text(((_tmp7F3="__asm__",_tag_dyneither(_tmp7F3,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7F0,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1165
const char*_tmp7F8;const char*_tmp7F7;struct Cyc_PP_Doc*_tmp7F6[3];s=((_tmp7F6[2]=Cyc_PP_text(((_tmp7F7=")",_tag_dyneither(_tmp7F7,sizeof(char),2)))),((_tmp7F6[1]=Cyc_PP_text(_tmp29F),((_tmp7F6[0]=Cyc_PP_text(((_tmp7F8="__asm__(",_tag_dyneither(_tmp7F8,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7F6,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A8;_LL1D1: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp2A0=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp2A0->tag != 37)goto _LL1D3;else{_tmp2A1=_tmp2A0->f1;_tmp2A2=_tmp2A0->f2;}}_LL1D2:
# 1168
{const char*_tmp7FF;const char*_tmp7FE;const char*_tmp7FD;struct Cyc_PP_Doc*_tmp7FC[5];s=((_tmp7FC[4]=Cyc_PP_text(((_tmp7FD=")",_tag_dyneither(_tmp7FD,sizeof(char),2)))),((_tmp7FC[3]=Cyc_PP_textptr(_tmp2A2),((_tmp7FC[2]=Cyc_PP_text(((_tmp7FE=".",_tag_dyneither(_tmp7FE,sizeof(char),2)))),((_tmp7FC[1]=Cyc_Absynpp_exp2doc(_tmp2A1),((_tmp7FC[0]=Cyc_PP_text(((_tmp7FF="tagcheck(",_tag_dyneither(_tmp7FF,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7FC,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1A8;_LL1D3: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp2A3=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp2A3->tag != 18)goto _LL1D5;else{_tmp2A4=(void*)_tmp2A3->f1;_tmp2A5=_tmp2A3->f2;}}_LL1D4:
# 1171
{const char*_tmp804;const char*_tmp803;struct Cyc_PP_Doc*_tmp802[3];s=((_tmp802[2]=Cyc_PP_text(((_tmp803=",",_tag_dyneither(_tmp803,sizeof(char),2)))),((_tmp802[1]=Cyc_Absynpp_typ2doc(_tmp2A4),((_tmp802[0]=Cyc_PP_text(((_tmp804="offsetof(",_tag_dyneither(_tmp804,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp802,sizeof(struct Cyc_PP_Doc*),3))))))));}
for(0;_tmp2A5 != 0;_tmp2A5=_tmp2A5->tl){
void*_tmp316=(void*)_tmp2A5->hd;void*_tmp317=_tmp316;struct _dyneither_ptr*_tmp319;unsigned int _tmp31B;_LL1FF: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp318=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp317;if(_tmp318->tag != 0)goto _LL201;else{_tmp319=_tmp318->f1;}}_LL200:
{const char*_tmp807;struct Cyc_PP_Doc*_tmp806[3];s=((_tmp806[2]=
_tmp2A5->tl != 0?Cyc_PP_text(((_tmp807=".",_tag_dyneither(_tmp807,sizeof(char),2)))): Cyc_PP_nil_doc(),((_tmp806[1]=
# 1174
Cyc_PP_textptr(_tmp319),((_tmp806[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp806,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1176
goto _LL1FE;_LL201: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp31A=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp317;if(_tmp31A->tag != 1)goto _LL1FE;else{_tmp31B=_tmp31A->f1;}}_LL202:
{struct Cyc_Int_pa_PrintArg_struct _tmp811;void*_tmp810[1];const char*_tmp80F;const char*_tmp80E;struct Cyc_PP_Doc*_tmp80D[3];s=((_tmp80D[2]=
_tmp2A5->tl != 0?Cyc_PP_text(((_tmp80E=".",_tag_dyneither(_tmp80E,sizeof(char),2)))): Cyc_PP_nil_doc(),((_tmp80D[1]=
# 1177
Cyc_PP_text((struct _dyneither_ptr)((_tmp811.tag=1,((_tmp811.f1=(unsigned long)((int)_tmp31B),((_tmp810[0]=& _tmp811,Cyc_aprintf(((_tmp80F="%d",_tag_dyneither(_tmp80F,sizeof(char),3))),_tag_dyneither(_tmp810,sizeof(void*),1))))))))),((_tmp80D[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp80D,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1179
goto _LL1FE;_LL1FE:;}
# 1181
{const char*_tmp814;struct Cyc_PP_Doc*_tmp813[2];s=((_tmp813[1]=Cyc_PP_text(((_tmp814=")",_tag_dyneither(_tmp814,sizeof(char),2)))),((_tmp813[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp813,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1A8;_LL1D5: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp2A6=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp2A6->tag != 19)goto _LL1D7;else{_tmp2A7=_tmp2A6->f1;}}_LL1D6:
# 1184
{const char*_tmp817;struct Cyc_PP_Doc*_tmp816[2];s=((_tmp816[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A7),((_tmp816[0]=Cyc_PP_text(((_tmp817="*",_tag_dyneither(_tmp817,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp816,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1A8;_LL1D7: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp2A8=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp2A8->tag != 20)goto _LL1D9;else{_tmp2A9=_tmp2A8->f1;_tmp2AA=_tmp2A8->f2;}}_LL1D8:
# 1187
{const char*_tmp81A;struct Cyc_PP_Doc*_tmp819[3];s=((_tmp819[2]=Cyc_PP_textptr(_tmp2AA),((_tmp819[1]=Cyc_PP_text(((_tmp81A=".",_tag_dyneither(_tmp81A,sizeof(char),2)))),((_tmp819[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A9),Cyc_PP_cat(_tag_dyneither(_tmp819,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A8;_LL1D9: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2AB=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp2AB->tag != 21)goto _LL1DB;else{_tmp2AC=_tmp2AB->f1;_tmp2AD=_tmp2AB->f2;}}_LL1DA:
# 1190
{const char*_tmp81D;struct Cyc_PP_Doc*_tmp81C[3];s=((_tmp81C[2]=Cyc_PP_textptr(_tmp2AD),((_tmp81C[1]=Cyc_PP_text(((_tmp81D="->",_tag_dyneither(_tmp81D,sizeof(char),3)))),((_tmp81C[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AC),Cyc_PP_cat(_tag_dyneither(_tmp81C,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A8;_LL1DB: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp2AE=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp2AE->tag != 22)goto _LL1DD;else{_tmp2AF=_tmp2AE->f1;_tmp2B0=_tmp2AE->f2;}}_LL1DC:
# 1193
{const char*_tmp822;const char*_tmp821;struct Cyc_PP_Doc*_tmp820[4];s=((_tmp820[3]=
# 1196
Cyc_PP_text(((_tmp821="]",_tag_dyneither(_tmp821,sizeof(char),2)))),((_tmp820[2]=
# 1195
Cyc_Absynpp_exp2doc(_tmp2B0),((_tmp820[1]=
# 1194
Cyc_PP_text(((_tmp822="[",_tag_dyneither(_tmp822,sizeof(char),2)))),((_tmp820[0]=
# 1193
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AF),Cyc_PP_cat(_tag_dyneither(_tmp820,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1197
goto _LL1A8;_LL1DD: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp2B1=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp2B1->tag != 23)goto _LL1DF;else{_tmp2B2=_tmp2B1->f1;}}_LL1DE:
# 1199
{const char*_tmp827;const char*_tmp826;struct Cyc_PP_Doc*_tmp825[4];s=((_tmp825[3]=
# 1202
Cyc_PP_text(((_tmp826=")",_tag_dyneither(_tmp826,sizeof(char),2)))),((_tmp825[2]=
# 1201
Cyc_Absynpp_exps2doc_prec(20,_tmp2B2),((_tmp825[1]=
# 1200
Cyc_PP_text(((_tmp827="(",_tag_dyneither(_tmp827,sizeof(char),2)))),((_tmp825[0]=
# 1199
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp825,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1203
goto _LL1A8;_LL1DF: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp2B3=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp2B3->tag != 24)goto _LL1E1;else{_tmp2B4=_tmp2B3->f1;_tmp2B5=_tmp2B3->f2;}}_LL1E0:
# 1205
{const char*_tmp82E;const char*_tmp82D;const char*_tmp82C;struct Cyc_PP_Doc*_tmp82B[4];s=((_tmp82B[3]=
# 1208
Cyc_Absynpp_group_braces(((_tmp82C=",",_tag_dyneither(_tmp82C,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2B5)),((_tmp82B[2]=
# 1207
Cyc_PP_text(((_tmp82D=")",_tag_dyneither(_tmp82D,sizeof(char),2)))),((_tmp82B[1]=
# 1206
Cyc_Absynpp_typ2doc((*_tmp2B4).f3),((_tmp82B[0]=
# 1205
Cyc_PP_text(((_tmp82E="(",_tag_dyneither(_tmp82E,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp82B,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1209
goto _LL1A8;_LL1E1: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp2B6=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp2B6->tag != 25)goto _LL1E3;else{_tmp2B7=_tmp2B6->f1;}}_LL1E2:
# 1211
{const char*_tmp82F;s=Cyc_Absynpp_group_braces(((_tmp82F=",",_tag_dyneither(_tmp82F,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2B7));}
goto _LL1A8;_LL1E3: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp2B8=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp2B8->tag != 26)goto _LL1E5;else{_tmp2B9=_tmp2B8->f1;_tmp2BA=_tmp2B8->f2;_tmp2BB=_tmp2B8->f3;}}_LL1E4:
# 1214
{const char*_tmp836;const char*_tmp835;const char*_tmp834;struct Cyc_PP_Doc*_tmp833[8];s=((_tmp833[7]=
# 1220
Cyc_Absynpp_rb(),((_tmp833[6]=
# 1219
Cyc_Absynpp_exp2doc(_tmp2BB),((_tmp833[5]=
# 1218
Cyc_PP_text(((_tmp834=" : ",_tag_dyneither(_tmp834,sizeof(char),4)))),((_tmp833[4]=
# 1217
Cyc_Absynpp_exp2doc(_tmp2BA),((_tmp833[3]=
# 1216
Cyc_PP_text(((_tmp835=" < ",_tag_dyneither(_tmp835,sizeof(char),4)))),((_tmp833[2]=
# 1215
Cyc_PP_text(*(*_tmp2B9->name).f2),((_tmp833[1]=
# 1214
Cyc_PP_text(((_tmp836="for ",_tag_dyneither(_tmp836,sizeof(char),5)))),((_tmp833[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp833,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1221
goto _LL1A8;_LL1E5: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp2BC=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp2BC->tag != 27)goto _LL1E7;else{_tmp2BD=_tmp2BC->f1;_tmp2BE=(void*)_tmp2BC->f2;}}_LL1E6:
# 1223
{const char*_tmp83D;const char*_tmp83C;const char*_tmp83B;struct Cyc_PP_Doc*_tmp83A[7];s=((_tmp83A[6]=
# 1228
Cyc_Absynpp_rb(),((_tmp83A[5]=
# 1227
Cyc_Absynpp_typ2doc(_tmp2BE),((_tmp83A[4]=
# 1226
Cyc_PP_text(((_tmp83B=" : ",_tag_dyneither(_tmp83B,sizeof(char),4)))),((_tmp83A[3]=
# 1225
Cyc_Absynpp_exp2doc(_tmp2BD),((_tmp83A[2]=
# 1224
Cyc_PP_text(((_tmp83C=" < ",_tag_dyneither(_tmp83C,sizeof(char),4)))),((_tmp83A[1]=
# 1223
Cyc_PP_text(((_tmp83D="for x ",_tag_dyneither(_tmp83D,sizeof(char),7)))),((_tmp83A[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp83A,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1229
goto _LL1A8;_LL1E7: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp2BF=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp2BF->tag != 28)goto _LL1E9;else{_tmp2C0=_tmp2BF->f1;_tmp2C1=_tmp2BF->f2;_tmp2C2=_tmp2BF->f3;}}_LL1E8: {
# 1231
struct Cyc_List_List*_tmp33E=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C2);
{const char*_tmp842;struct Cyc_List_List*_tmp841;struct Cyc_PP_Doc*_tmp840[2];s=((_tmp840[1]=
Cyc_Absynpp_group_braces(((_tmp842=",",_tag_dyneither(_tmp842,sizeof(char),2))),
_tmp2C1 != 0?(_tmp841=_cycalloc(sizeof(*_tmp841)),((_tmp841->hd=Cyc_Absynpp_tps2doc(_tmp2C1),((_tmp841->tl=_tmp33E,_tmp841))))): _tmp33E),((_tmp840[0]=
# 1232
Cyc_Absynpp_qvar2doc(_tmp2C0),Cyc_PP_cat(_tag_dyneither(_tmp840,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1235
goto _LL1A8;}_LL1E9: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp2C3=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp2C3->tag != 29)goto _LL1EB;else{_tmp2C4=_tmp2C3->f2;}}_LL1EA:
# 1237
{const char*_tmp843;s=Cyc_Absynpp_group_braces(((_tmp843=",",_tag_dyneither(_tmp843,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C4));}
goto _LL1A8;_LL1EB: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp2C5=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp2C5->tag != 30)goto _LL1ED;else{_tmp2C6=_tmp2C5->f1;_tmp2C7=_tmp2C5->f3;}}_LL1EC:
# 1240
 if(_tmp2C6 == 0)
# 1242
s=Cyc_Absynpp_qvar2doc(_tmp2C7->name);else{
# 1244
const char*_tmp84A;const char*_tmp849;const char*_tmp848;struct Cyc_PP_Doc*_tmp847[2];s=((_tmp847[1]=
Cyc_PP_egroup(((_tmp84A="(",_tag_dyneither(_tmp84A,sizeof(char),2))),((_tmp849=")",_tag_dyneither(_tmp849,sizeof(char),2))),((_tmp848=",",_tag_dyneither(_tmp848,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp2C6)),((_tmp847[0]=
# 1244
Cyc_Absynpp_qvar2doc(_tmp2C7->name),Cyc_PP_cat(_tag_dyneither(_tmp847,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1246
goto _LL1A8;_LL1ED: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp2C8=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp2C8->tag != 31)goto _LL1EF;else{_tmp2C9=_tmp2C8->f1;}}_LL1EE:
# 1248
 s=Cyc_Absynpp_qvar2doc(_tmp2C9);
goto _LL1A8;_LL1EF: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp2CA=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp2CA->tag != 32)goto _LL1F1;else{_tmp2CB=_tmp2CA->f1;}}_LL1F0:
# 1251
 s=Cyc_Absynpp_qvar2doc(_tmp2CB);
goto _LL1A8;_LL1F1: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2CC=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp2CC->tag != 33)goto _LL1F3;else{_tmp2CD=(_tmp2CC->f1).is_calloc;_tmp2CE=(_tmp2CC->f1).rgn;_tmp2CF=(_tmp2CC->f1).elt_type;_tmp2D0=(_tmp2CC->f1).num_elts;}}_LL1F2:
# 1256
 if(_tmp2CD){
# 1258
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp2CF)),0);
if(_tmp2CE == 0){
const char*_tmp851;const char*_tmp850;const char*_tmp84F;struct Cyc_PP_Doc*_tmp84E[5];s=((_tmp84E[4]=Cyc_PP_text(((_tmp84F=")",_tag_dyneither(_tmp84F,sizeof(char),2)))),((_tmp84E[3]=Cyc_Absynpp_exp2doc(st),((_tmp84E[2]=Cyc_PP_text(((_tmp850=",",_tag_dyneither(_tmp850,sizeof(char),2)))),((_tmp84E[1]=Cyc_Absynpp_exp2doc(_tmp2D0),((_tmp84E[0]=Cyc_PP_text(((_tmp851="calloc(",_tag_dyneither(_tmp851,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp84E,sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{
# 1262
const char*_tmp85A;const char*_tmp859;const char*_tmp858;const char*_tmp857;struct Cyc_PP_Doc*_tmp856[7];s=((_tmp856[6]=
Cyc_PP_text(((_tmp857=")",_tag_dyneither(_tmp857,sizeof(char),2)))),((_tmp856[5]=Cyc_Absynpp_exp2doc(st),((_tmp856[4]=Cyc_PP_text(((_tmp858=",",_tag_dyneither(_tmp858,sizeof(char),2)))),((_tmp856[3]=Cyc_Absynpp_exp2doc(_tmp2D0),((_tmp856[2]=
# 1262
Cyc_PP_text(((_tmp859=",",_tag_dyneither(_tmp859,sizeof(char),2)))),((_tmp856[1]=Cyc_Absynpp_exp2doc(_tmp2CE),((_tmp856[0]=Cyc_PP_text(((_tmp85A="rcalloc(",_tag_dyneither(_tmp85A,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp856,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1265
struct Cyc_Absyn_Exp*new_e;
# 1267
if(_tmp2CF == 0)
new_e=_tmp2D0;else{
# 1270
new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp2CF,0),_tmp2D0,0);}
# 1272
if(_tmp2CE == 0){
const char*_tmp85F;const char*_tmp85E;struct Cyc_PP_Doc*_tmp85D[3];s=((_tmp85D[2]=Cyc_PP_text(((_tmp85E=")",_tag_dyneither(_tmp85E,sizeof(char),2)))),((_tmp85D[1]=Cyc_Absynpp_exp2doc(new_e),((_tmp85D[0]=Cyc_PP_text(((_tmp85F="malloc(",_tag_dyneither(_tmp85F,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp85D,sizeof(struct Cyc_PP_Doc*),3))))))));}else{
# 1275
const char*_tmp866;const char*_tmp865;const char*_tmp864;struct Cyc_PP_Doc*_tmp863[5];s=((_tmp863[4]=
Cyc_PP_text(((_tmp864=")",_tag_dyneither(_tmp864,sizeof(char),2)))),((_tmp863[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp863[2]=
# 1275
Cyc_PP_text(((_tmp865=",",_tag_dyneither(_tmp865,sizeof(char),2)))),((_tmp863[1]=Cyc_Absynpp_exp2doc(_tmp2CE),((_tmp863[0]=Cyc_PP_text(((_tmp866="rmalloc(",_tag_dyneither(_tmp866,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp863,sizeof(struct Cyc_PP_Doc*),5))))))))))));}}
# 1278
goto _LL1A8;_LL1F3: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp2D1=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp2D1->tag != 34)goto _LL1F5;else{_tmp2D2=_tmp2D1->f1;_tmp2D3=_tmp2D1->f2;}}_LL1F4:
# 1280
{const char*_tmp869;struct Cyc_PP_Doc*_tmp868[3];s=((_tmp868[2]=
# 1282
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D3),((_tmp868[1]=
# 1281
Cyc_PP_text(((_tmp869=" :=: ",_tag_dyneither(_tmp869,sizeof(char),6)))),((_tmp868[0]=
# 1280
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D2),Cyc_PP_cat(_tag_dyneither(_tmp868,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1283
goto _LL1A8;_LL1F5: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp2D4=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp2D4->tag != 35)goto _LL1F7;else{_tmp2D5=_tmp2D4->f1;_tmp2D6=_tmp2D4->f2;}}_LL1F6:
# 1286
{const char*_tmp86A;s=Cyc_Absynpp_group_braces(((_tmp86A=",",_tag_dyneither(_tmp86A,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2D6));}
goto _LL1A8;_LL1F7: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp2D7=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp2D7->tag != 36)goto _LL1A8;else{_tmp2D8=_tmp2D7->f1;}}_LL1F8:
# 1289
{const char*_tmp86F;const char*_tmp86E;struct Cyc_PP_Doc*_tmp86D[7];s=((_tmp86D[6]=
# 1291
Cyc_PP_text(((_tmp86E=")",_tag_dyneither(_tmp86E,sizeof(char),2)))),((_tmp86D[5]=Cyc_Absynpp_rb(),((_tmp86D[4]=Cyc_PP_blank_doc(),((_tmp86D[3]=
# 1290
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2D8,1)),((_tmp86D[2]=
# 1289
Cyc_PP_blank_doc(),((_tmp86D[1]=Cyc_Absynpp_lb(),((_tmp86D[0]=Cyc_PP_text(((_tmp86F="(",_tag_dyneither(_tmp86F,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp86D,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1292
goto _LL1A8;_LL1A8:;}
# 1294
if(inprec >= myprec){
const char*_tmp874;const char*_tmp873;struct Cyc_PP_Doc*_tmp872[3];s=((_tmp872[2]=Cyc_PP_text(((_tmp873=")",_tag_dyneither(_tmp873,sizeof(char),2)))),((_tmp872[1]=s,((_tmp872[0]=Cyc_PP_text(((_tmp874="(",_tag_dyneither(_tmp874,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp872,sizeof(struct Cyc_PP_Doc*),3))))))));}
return s;}
# 1299
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp360=d;struct Cyc_Absyn_Exp*_tmp362;struct _dyneither_ptr*_tmp364;_LL204: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp361=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp360;if(_tmp361->tag != 0)goto _LL206;else{_tmp362=_tmp361->f1;}}_LL205: {
const char*_tmp879;const char*_tmp878;struct Cyc_PP_Doc*_tmp877[3];return(_tmp877[2]=Cyc_PP_text(((_tmp878="]",_tag_dyneither(_tmp878,sizeof(char),2)))),((_tmp877[1]=Cyc_Absynpp_exp2doc(_tmp362),((_tmp877[0]=Cyc_PP_text(((_tmp879=".[",_tag_dyneither(_tmp879,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp877,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL206: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp363=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp360;if(_tmp363->tag != 1)goto _LL203;else{_tmp364=_tmp363->f1;}}_LL207: {
const char*_tmp87C;struct Cyc_PP_Doc*_tmp87B[2];return(_tmp87B[1]=Cyc_PP_textptr(_tmp364),((_tmp87B[0]=Cyc_PP_text(((_tmp87C=".",_tag_dyneither(_tmp87C,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp87B,sizeof(struct Cyc_PP_Doc*),2)))));}_LL203:;}
# 1306
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
const char*_tmp883;const char*_tmp882;const char*_tmp881;struct Cyc_PP_Doc*_tmp880[2];return(_tmp880[1]=
Cyc_Absynpp_exp2doc((*de).f2),((_tmp880[0]=
# 1308
Cyc_PP_egroup(((_tmp883="",_tag_dyneither(_tmp883,sizeof(char),1))),((_tmp882="=",_tag_dyneither(_tmp882,sizeof(char),2))),((_tmp881="=",_tag_dyneither(_tmp881,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp880,sizeof(struct Cyc_PP_Doc*),2)))));}}
# 1312
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
const char*_tmp886;const char*_tmp885;const char*_tmp884;return Cyc_PP_group(((_tmp884="",_tag_dyneither(_tmp884,sizeof(char),1))),((_tmp885="",_tag_dyneither(_tmp885,sizeof(char),1))),((_tmp886=",",_tag_dyneither(_tmp886,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long i);static void _tmp88B(unsigned int*_tmp88A,unsigned int*_tmp889,char**_tmp888){for(*_tmp88A=0;*_tmp88A < *_tmp889;(*_tmp88A)++){(*_tmp888)[*_tmp88A]=(char)'z';}}
# 1316
struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
char*_tmp890;unsigned int _tmp88F;char*_tmp88E;unsigned int _tmp88D;unsigned int _tmp88C;struct _dyneither_ptr x=(_tmp890=((_tmp88F=(unsigned int)28,((_tmp88E=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp88F + 1)),((((_tmp88D=_tmp88F,((_tmp88B(& _tmp88C,& _tmp88D,& _tmp88E),_tmp88E[_tmp88D]=(char)0)))),_tmp88E)))))),_tag_dyneither(_tmp890,sizeof(char),_get_zero_arr_size_char((void*)_tmp890,(unsigned int)28 + 1)));
{char _tmp893;char _tmp892;struct _dyneither_ptr _tmp891;(_tmp891=_dyneither_ptr_plus(x,sizeof(char),27),((_tmp892=*((char*)_check_dyneither_subscript(_tmp891,sizeof(char),0)),((_tmp893='\000',((_get_dyneither_size(_tmp891,sizeof(char))== 1  && (_tmp892 == '\000'  && _tmp893 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp891.curr)=_tmp893)))))));}
{char _tmp896;char _tmp895;struct _dyneither_ptr _tmp894;(_tmp894=_dyneither_ptr_plus(x,sizeof(char),26),((_tmp895=*((char*)_check_dyneither_subscript(_tmp894,sizeof(char),0)),((_tmp896='L',((_get_dyneither_size(_tmp894,sizeof(char))== 1  && (_tmp895 == '\000'  && _tmp896 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp894.curr)=_tmp896)))))));}
{char _tmp899;char _tmp898;struct _dyneither_ptr _tmp897;(_tmp897=_dyneither_ptr_plus(x,sizeof(char),25),((_tmp898=*((char*)_check_dyneither_subscript(_tmp897,sizeof(char),0)),((_tmp899='L',((_get_dyneither_size(_tmp897,sizeof(char))== 1  && (_tmp898 == '\000'  && _tmp899 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp897.curr)=_tmp899)))))));}
{char _tmp89C;char _tmp89B;struct _dyneither_ptr _tmp89A;(_tmp89A=_dyneither_ptr_plus(x,sizeof(char),24),((_tmp89B=*((char*)_check_dyneither_subscript(_tmp89A,sizeof(char),0)),((_tmp89C='U',((_get_dyneither_size(_tmp89A,sizeof(char))== 1  && (_tmp89B == '\000'  && _tmp89C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp89A.curr)=_tmp89C)))))));}
{char _tmp89F;char _tmp89E;struct _dyneither_ptr _tmp89D;(_tmp89D=_dyneither_ptr_plus(x,sizeof(char),23),((_tmp89E=*((char*)_check_dyneither_subscript(_tmp89D,sizeof(char),0)),((_tmp89F='0',((_get_dyneither_size(_tmp89D,sizeof(char))== 1  && (_tmp89E == '\000'  && _tmp89F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp89D.curr)=_tmp89F)))))));}{
int index=23;
while(i != 0){
char c=(char)('0' + i % 10);
{char _tmp8A2;char _tmp8A1;struct _dyneither_ptr _tmp8A0;(_tmp8A0=_dyneither_ptr_plus(x,sizeof(char),index),((_tmp8A1=*((char*)_check_dyneither_subscript(_tmp8A0,sizeof(char),0)),((_tmp8A2=c,((_get_dyneither_size(_tmp8A0,sizeof(char))== 1  && (_tmp8A1 == '\000'  && _tmp8A2 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp8A0.curr)=_tmp8A2)))))));}
i=i / 10;
-- index;}
# 1330
return(struct _dyneither_ptr)_dyneither_ptr_plus(_dyneither_ptr_plus(x,sizeof(char),index),sizeof(char),1);};}
# 1334
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp387=c;enum Cyc_Absyn_Sign _tmp388;char _tmp389;struct _dyneither_ptr _tmp38A;enum Cyc_Absyn_Sign _tmp38B;short _tmp38C;int _tmp38D;int _tmp38E;int _tmp38F;enum Cyc_Absyn_Sign _tmp390;long long _tmp391;struct _dyneither_ptr _tmp392;struct _dyneither_ptr _tmp393;struct _dyneither_ptr _tmp394;_LL209: if((_tmp387.Char_c).tag != 2)goto _LL20B;_tmp388=((struct _tuple3)(_tmp387.Char_c).val).f1;_tmp389=((struct _tuple3)(_tmp387.Char_c).val).f2;_LL20A: {
const char*_tmp8A6;void*_tmp8A5[1];struct Cyc_String_pa_PrintArg_struct _tmp8A4;return Cyc_PP_text((struct _dyneither_ptr)((_tmp8A4.tag=0,((_tmp8A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp389)),((_tmp8A5[0]=& _tmp8A4,Cyc_aprintf(((_tmp8A6="'%s'",_tag_dyneither(_tmp8A6,sizeof(char),5))),_tag_dyneither(_tmp8A5,sizeof(void*),1)))))))));}_LL20B: if((_tmp387.Wchar_c).tag != 3)goto _LL20D;_tmp38A=(struct _dyneither_ptr)(_tmp387.Wchar_c).val;_LL20C: {
const char*_tmp8AA;void*_tmp8A9[1];struct Cyc_String_pa_PrintArg_struct _tmp8A8;return Cyc_PP_text((struct _dyneither_ptr)((_tmp8A8.tag=0,((_tmp8A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp38A),((_tmp8A9[0]=& _tmp8A8,Cyc_aprintf(((_tmp8AA="L'%s'",_tag_dyneither(_tmp8AA,sizeof(char),6))),_tag_dyneither(_tmp8A9,sizeof(void*),1)))))))));}_LL20D: if((_tmp387.Short_c).tag != 4)goto _LL20F;_tmp38B=((struct _tuple4)(_tmp387.Short_c).val).f1;_tmp38C=((struct _tuple4)(_tmp387.Short_c).val).f2;_LL20E: {
const char*_tmp8AE;void*_tmp8AD[1];struct Cyc_Int_pa_PrintArg_struct _tmp8AC;return Cyc_PP_text((struct _dyneither_ptr)((_tmp8AC.tag=1,((_tmp8AC.f1=(unsigned long)((int)_tmp38C),((_tmp8AD[0]=& _tmp8AC,Cyc_aprintf(((_tmp8AE="%d",_tag_dyneither(_tmp8AE,sizeof(char),3))),_tag_dyneither(_tmp8AD,sizeof(void*),1)))))))));}_LL20F: if((_tmp387.Int_c).tag != 5)goto _LL211;if(((struct _tuple5)(_tmp387.Int_c).val).f1 != Cyc_Absyn_None)goto _LL211;_tmp38D=((struct _tuple5)(_tmp387.Int_c).val).f2;_LL210:
 _tmp38E=_tmp38D;goto _LL212;_LL211: if((_tmp387.Int_c).tag != 5)goto _LL213;if(((struct _tuple5)(_tmp387.Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL213;_tmp38E=((struct _tuple5)(_tmp387.Int_c).val).f2;_LL212: {
const char*_tmp8B2;void*_tmp8B1[1];struct Cyc_Int_pa_PrintArg_struct _tmp8B0;return Cyc_PP_text((struct _dyneither_ptr)((_tmp8B0.tag=1,((_tmp8B0.f1=(unsigned long)_tmp38E,((_tmp8B1[0]=& _tmp8B0,Cyc_aprintf(((_tmp8B2="%d",_tag_dyneither(_tmp8B2,sizeof(char),3))),_tag_dyneither(_tmp8B1,sizeof(void*),1)))))))));}_LL213: if((_tmp387.Int_c).tag != 5)goto _LL215;if(((struct _tuple5)(_tmp387.Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL215;_tmp38F=((struct _tuple5)(_tmp387.Int_c).val).f2;_LL214: {
const char*_tmp8B6;void*_tmp8B5[1];struct Cyc_Int_pa_PrintArg_struct _tmp8B4;return Cyc_PP_text((struct _dyneither_ptr)((_tmp8B4.tag=1,((_tmp8B4.f1=(unsigned int)_tmp38F,((_tmp8B5[0]=& _tmp8B4,Cyc_aprintf(((_tmp8B6="%u",_tag_dyneither(_tmp8B6,sizeof(char),3))),_tag_dyneither(_tmp8B5,sizeof(void*),1)))))))));}_LL215: if((_tmp387.LongLong_c).tag != 6)goto _LL217;_tmp390=((struct _tuple6)(_tmp387.LongLong_c).val).f1;_tmp391=((struct _tuple6)(_tmp387.LongLong_c).val).f2;_LL216:
# 1344
 return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)_tmp391));_LL217: if((_tmp387.Float_c).tag != 7)goto _LL219;_tmp392=((struct _tuple7)(_tmp387.Float_c).val).f1;_LL218:
 return Cyc_PP_text(_tmp392);_LL219: if((_tmp387.Null_c).tag != 1)goto _LL21B;_LL21A: {
const char*_tmp8B7;return Cyc_PP_text(((_tmp8B7="NULL",_tag_dyneither(_tmp8B7,sizeof(char),5))));}_LL21B: if((_tmp387.String_c).tag != 8)goto _LL21D;_tmp393=(struct _dyneither_ptr)(_tmp387.String_c).val;_LL21C: {
# 1348
const char*_tmp8BC;const char*_tmp8BB;struct Cyc_PP_Doc*_tmp8BA[3];return(_tmp8BA[2]=Cyc_PP_text(((_tmp8BB="\"",_tag_dyneither(_tmp8BB,sizeof(char),2)))),((_tmp8BA[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp393)),((_tmp8BA[0]=Cyc_PP_text(((_tmp8BC="\"",_tag_dyneither(_tmp8BC,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8BA,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL21D: if((_tmp387.Wstring_c).tag != 9)goto _LL208;_tmp394=(struct _dyneither_ptr)(_tmp387.Wstring_c).val;_LL21E: {
# 1350
const char*_tmp8C1;const char*_tmp8C0;struct Cyc_PP_Doc*_tmp8BF[3];return(_tmp8BF[2]=Cyc_PP_text(((_tmp8C0="\"",_tag_dyneither(_tmp8C0,sizeof(char),2)))),((_tmp8BF[1]=Cyc_PP_text(_tmp394),((_tmp8BF[0]=Cyc_PP_text(((_tmp8C1="L\"",_tag_dyneither(_tmp8C1,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp8BF,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL208:;}
# 1354
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if(p == Cyc_Absyn_Numelts){
if(es == 0  || es->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp8CE;const char*_tmp8CD;void*_tmp8CC[1];struct Cyc_String_pa_PrintArg_struct _tmp8CB;struct Cyc_Core_Failure_exn_struct*_tmp8CA;(int)_throw((void*)((_tmp8CA=_cycalloc(sizeof(*_tmp8CA)),((_tmp8CA[0]=((_tmp8CE.tag=Cyc_Core_Failure,((_tmp8CE.f1=(struct _dyneither_ptr)((_tmp8CB.tag=0,((_tmp8CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp8CC[0]=& _tmp8CB,Cyc_aprintf(((_tmp8CD="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp8CD,sizeof(char),47))),_tag_dyneither(_tmp8CC,sizeof(void*),1)))))))),_tmp8CE)))),_tmp8CA)))));}{
# 1360
const char*_tmp8D3;const char*_tmp8D2;struct Cyc_PP_Doc*_tmp8D1[3];return(_tmp8D1[2]=Cyc_PP_text(((_tmp8D2=")",_tag_dyneither(_tmp8D2,sizeof(char),2)))),((_tmp8D1[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp8D1[0]=Cyc_PP_text(((_tmp8D3="numelts(",_tag_dyneither(_tmp8D3,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8D1,sizeof(struct Cyc_PP_Doc*),3)))))));};}else{
# 1362
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0){
struct Cyc_Core_Failure_exn_struct _tmp8E0;const char*_tmp8DF;void*_tmp8DE[1];struct Cyc_String_pa_PrintArg_struct _tmp8DD;struct Cyc_Core_Failure_exn_struct*_tmp8DC;(int)_throw((void*)((_tmp8DC=_cycalloc(sizeof(*_tmp8DC)),((_tmp8DC[0]=((_tmp8E0.tag=Cyc_Core_Failure,((_tmp8E0.f1=(struct _dyneither_ptr)((_tmp8DD.tag=0,((_tmp8DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp8DE[0]=& _tmp8DD,Cyc_aprintf(((_tmp8DF="Absynpp::primapp2doc: %s with no args",_tag_dyneither(_tmp8DF,sizeof(char),38))),_tag_dyneither(_tmp8DE,sizeof(void*),1)))))))),_tmp8E0)))),_tmp8DC)))));}else{
# 1366
if(ds->tl == 0){
const char*_tmp8E3;struct Cyc_PP_Doc*_tmp8E2[3];return(_tmp8E2[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp8E2[1]=Cyc_PP_text(((_tmp8E3=" ",_tag_dyneither(_tmp8E3,sizeof(char),2)))),((_tmp8E2[0]=ps,Cyc_PP_cat(_tag_dyneither(_tmp8E2,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp8F0;const char*_tmp8EF;void*_tmp8EE[1];struct Cyc_String_pa_PrintArg_struct _tmp8ED;struct Cyc_Core_Failure_exn_struct*_tmp8EC;(int)_throw((void*)((_tmp8EC=_cycalloc(sizeof(*_tmp8EC)),((_tmp8EC[0]=((_tmp8F0.tag=Cyc_Core_Failure,((_tmp8F0.f1=(struct _dyneither_ptr)((_tmp8ED.tag=0,((_tmp8ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp8EE[0]=& _tmp8ED,Cyc_aprintf(((_tmp8EF="Absynpp::primapp2doc: %s with more than 2 args",_tag_dyneither(_tmp8EF,sizeof(char),47))),_tag_dyneither(_tmp8EE,sizeof(void*),1)))))))),_tmp8F0)))),_tmp8EC)))));}else{
# 1372
const char*_tmp8F5;const char*_tmp8F4;struct Cyc_PP_Doc*_tmp8F3[5];return(_tmp8F3[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd,((_tmp8F3[3]=Cyc_PP_text(((_tmp8F4=" ",_tag_dyneither(_tmp8F4,sizeof(char),2)))),((_tmp8F3[2]=ps,((_tmp8F3[1]=Cyc_PP_text(((_tmp8F5=" ",_tag_dyneither(_tmp8F5,sizeof(char),2)))),((_tmp8F3[0]=(struct Cyc_PP_Doc*)ds->hd,Cyc_PP_cat(_tag_dyneither(_tmp8F3,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
# 1376
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
switch(p){case Cyc_Absyn_Plus: _LL21F: {
const char*_tmp8F6;return(_tmp8F6="+",_tag_dyneither(_tmp8F6,sizeof(char),2));}case Cyc_Absyn_Times: _LL220: {
const char*_tmp8F7;return(_tmp8F7="*",_tag_dyneither(_tmp8F7,sizeof(char),2));}case Cyc_Absyn_Minus: _LL221: {
const char*_tmp8F8;return(_tmp8F8="-",_tag_dyneither(_tmp8F8,sizeof(char),2));}case Cyc_Absyn_Div: _LL222: {
const char*_tmp8F9;return(_tmp8F9="/",_tag_dyneither(_tmp8F9,sizeof(char),2));}case Cyc_Absyn_Mod: _LL223: {
const char*_tmp8FB;const char*_tmp8FA;return Cyc_Absynpp_print_for_cycdoc?(_tmp8FB="\\%",_tag_dyneither(_tmp8FB,sizeof(char),3)):((_tmp8FA="%",_tag_dyneither(_tmp8FA,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL224: {
const char*_tmp8FC;return(_tmp8FC="==",_tag_dyneither(_tmp8FC,sizeof(char),3));}case Cyc_Absyn_Neq: _LL225: {
const char*_tmp8FD;return(_tmp8FD="!=",_tag_dyneither(_tmp8FD,sizeof(char),3));}case Cyc_Absyn_Gt: _LL226: {
const char*_tmp8FE;return(_tmp8FE=">",_tag_dyneither(_tmp8FE,sizeof(char),2));}case Cyc_Absyn_Lt: _LL227: {
const char*_tmp8FF;return(_tmp8FF="<",_tag_dyneither(_tmp8FF,sizeof(char),2));}case Cyc_Absyn_Gte: _LL228: {
const char*_tmp900;return(_tmp900=">=",_tag_dyneither(_tmp900,sizeof(char),3));}case Cyc_Absyn_Lte: _LL229: {
const char*_tmp901;return(_tmp901="<=",_tag_dyneither(_tmp901,sizeof(char),3));}case Cyc_Absyn_Not: _LL22A: {
const char*_tmp902;return(_tmp902="!",_tag_dyneither(_tmp902,sizeof(char),2));}case Cyc_Absyn_Bitnot: _LL22B: {
const char*_tmp903;return(_tmp903="~",_tag_dyneither(_tmp903,sizeof(char),2));}case Cyc_Absyn_Bitand: _LL22C: {
const char*_tmp904;return(_tmp904="&",_tag_dyneither(_tmp904,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL22D: {
const char*_tmp905;return(_tmp905="|",_tag_dyneither(_tmp905,sizeof(char),2));}case Cyc_Absyn_Bitxor: _LL22E: {
const char*_tmp906;return(_tmp906="^",_tag_dyneither(_tmp906,sizeof(char),2));}case Cyc_Absyn_Bitlshift: _LL22F: {
const char*_tmp907;return(_tmp907="<<",_tag_dyneither(_tmp907,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL230: {
const char*_tmp908;return(_tmp908=">>",_tag_dyneither(_tmp908,sizeof(char),3));}case Cyc_Absyn_Bitarshift: _LL231: {
const char*_tmp909;return(_tmp909=">>>",_tag_dyneither(_tmp909,sizeof(char),4));}case Cyc_Absyn_Numelts: _LL232: {
const char*_tmp90A;return(_tmp90A="numelts",_tag_dyneither(_tmp90A,sizeof(char),8));}}}
# 1401
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1405
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp3D7=s->r;void*_tmp3D8=_tmp3D7;_LL235: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3D9=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3D8;if(_tmp3D9->tag != 12)goto _LL237;}_LL236:
 return 1;_LL237:;_LL238:
 return 0;_LL234:;}
# 1412
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp){
struct Cyc_PP_Doc*s;
{void*_tmp3DA=st->r;void*_tmp3DB=_tmp3DA;struct Cyc_Absyn_Exp*_tmp3DE;struct Cyc_Absyn_Stmt*_tmp3E0;struct Cyc_Absyn_Stmt*_tmp3E1;struct Cyc_Absyn_Exp*_tmp3E3;struct Cyc_Absyn_Exp*_tmp3E5;struct Cyc_Absyn_Stmt*_tmp3E6;struct Cyc_Absyn_Stmt*_tmp3E7;struct Cyc_Absyn_Exp*_tmp3E9;struct Cyc_Absyn_Stmt*_tmp3EA;struct _dyneither_ptr*_tmp3EE;struct Cyc_Absyn_Exp*_tmp3F0;struct Cyc_Absyn_Exp*_tmp3F1;struct Cyc_Absyn_Exp*_tmp3F2;struct Cyc_Absyn_Stmt*_tmp3F3;struct Cyc_Absyn_Exp*_tmp3F5;struct Cyc_List_List*_tmp3F6;struct Cyc_List_List*_tmp3F9;struct Cyc_Absyn_Decl*_tmp3FB;struct Cyc_Absyn_Stmt*_tmp3FC;struct _dyneither_ptr*_tmp3FE;struct Cyc_Absyn_Stmt*_tmp3FF;struct Cyc_Absyn_Stmt*_tmp401;struct Cyc_Absyn_Exp*_tmp402;struct Cyc_Absyn_Stmt*_tmp404;struct Cyc_List_List*_tmp405;struct Cyc_Absyn_Exp*_tmp407;_LL23A: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp3DC=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3DB;if(_tmp3DC->tag != 0)goto _LL23C;}_LL23B:
{const char*_tmp90B;s=Cyc_PP_text(((_tmp90B=";",_tag_dyneither(_tmp90B,sizeof(char),2))));}goto _LL239;_LL23C: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3DD=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3DB;if(_tmp3DD->tag != 1)goto _LL23E;else{_tmp3DE=_tmp3DD->f1;}}_LL23D:
{const char*_tmp90E;struct Cyc_PP_Doc*_tmp90D[2];s=((_tmp90D[1]=Cyc_PP_text(((_tmp90E=";",_tag_dyneither(_tmp90E,sizeof(char),2)))),((_tmp90D[0]=Cyc_Absynpp_exp2doc(_tmp3DE),Cyc_PP_cat(_tag_dyneither(_tmp90D,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL239;_LL23E: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3DF=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3DB;if(_tmp3DF->tag != 2)goto _LL240;else{_tmp3E0=_tmp3DF->f1;_tmp3E1=_tmp3DF->f2;}}_LL23F:
# 1418
 if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp3E0)){
struct Cyc_PP_Doc*_tmp919[5];struct Cyc_PP_Doc*_tmp918[7];const char*_tmp917;const char*_tmp916;struct Cyc_PP_Doc*_tmp915[7];s=((_tmp915[6]=
# 1426
Cyc_Absynpp_is_declaration(_tmp3E1)?
stmtexp?
(_tmp918[6]=
# 1431
Cyc_PP_line_doc(),((_tmp918[5]=
# 1430
Cyc_PP_text(((_tmp917=");",_tag_dyneither(_tmp917,sizeof(char),3)))),((_tmp918[4]=Cyc_Absynpp_rb(),((_tmp918[3]=
# 1429
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3E1,stmtexp)),((_tmp918[2]=
# 1428
Cyc_PP_blank_doc(),((_tmp918[1]=Cyc_Absynpp_lb(),((_tmp918[0]=Cyc_PP_text(((_tmp916="(",_tag_dyneither(_tmp916,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp918,sizeof(struct Cyc_PP_Doc*),7))))))))))))))):(
# 1432
(_tmp919[4]=
# 1435
Cyc_PP_line_doc(),((_tmp919[3]=
# 1434
Cyc_Absynpp_rb(),((_tmp919[2]=
# 1433
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3E1,stmtexp)),((_tmp919[1]=
# 1432
Cyc_PP_blank_doc(),((_tmp919[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp919,sizeof(struct Cyc_PP_Doc*),5)))))))))))):
# 1436
 Cyc_Absynpp_stmt2doc(_tmp3E1,stmtexp),((_tmp915[5]=
# 1425
Cyc_PP_line_doc(),((_tmp915[4]=
# 1424
Cyc_Absynpp_rb(),((_tmp915[3]=
# 1423
Cyc_PP_line_doc(),((_tmp915[2]=
# 1422
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3E0,0)),((_tmp915[1]=
# 1421
Cyc_PP_blank_doc(),((_tmp915[0]=
# 1420
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp915,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1437
if(Cyc_Absynpp_is_declaration(_tmp3E1)){
struct Cyc_PP_Doc*_tmp924[4];struct Cyc_PP_Doc*_tmp923[6];const char*_tmp922;const char*_tmp921;struct Cyc_PP_Doc*_tmp920[4];s=((_tmp920[3]=
# 1448
Cyc_PP_line_doc(),((_tmp920[2]=
# 1440
stmtexp?
(_tmp923[5]=
# 1443
Cyc_PP_text(((_tmp922=");",_tag_dyneither(_tmp922,sizeof(char),3)))),((_tmp923[4]=Cyc_Absynpp_rb(),((_tmp923[3]=
# 1442
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3E1,stmtexp)),((_tmp923[2]=
# 1441
Cyc_PP_blank_doc(),((_tmp923[1]=Cyc_Absynpp_lb(),((_tmp923[0]=Cyc_PP_text(((_tmp921="(",_tag_dyneither(_tmp921,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp923,sizeof(struct Cyc_PP_Doc*),6))))))))))))):(
# 1445
(_tmp924[3]=
# 1447
Cyc_Absynpp_rb(),((_tmp924[2]=
# 1446
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3E1,stmtexp)),((_tmp924[1]=
# 1445
Cyc_PP_blank_doc(),((_tmp924[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp924,sizeof(struct Cyc_PP_Doc*),4)))))))))),((_tmp920[1]=
# 1439
Cyc_PP_line_doc(),((_tmp920[0]=
# 1438
Cyc_Absynpp_stmt2doc(_tmp3E0,0),Cyc_PP_cat(_tag_dyneither(_tmp920,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1450
struct Cyc_PP_Doc*_tmp925[3];s=((_tmp925[2]=Cyc_Absynpp_stmt2doc(_tmp3E1,stmtexp),((_tmp925[1]=Cyc_PP_line_doc(),((_tmp925[0]=Cyc_Absynpp_stmt2doc(_tmp3E0,0),Cyc_PP_cat(_tag_dyneither(_tmp925,sizeof(struct Cyc_PP_Doc*),3))))))));}}}else{
# 1453
struct Cyc_PP_Doc*_tmp926[3];s=((_tmp926[2]=Cyc_Absynpp_stmt2doc(_tmp3E1,stmtexp),((_tmp926[1]=Cyc_PP_line_doc(),((_tmp926[0]=Cyc_Absynpp_stmt2doc(_tmp3E0,0),Cyc_PP_cat(_tag_dyneither(_tmp926,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL239;_LL240: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp3E2=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp3DB;if(_tmp3E2->tag != 3)goto _LL242;else{_tmp3E3=_tmp3E2->f1;}}_LL241:
# 1456
 if(_tmp3E3 == 0){
const char*_tmp927;s=Cyc_PP_text(((_tmp927="return;",_tag_dyneither(_tmp927,sizeof(char),8))));}else{
# 1459
const char*_tmp92C;const char*_tmp92B;struct Cyc_PP_Doc*_tmp92A[3];s=((_tmp92A[2]=
# 1461
Cyc_PP_text(((_tmp92B=";",_tag_dyneither(_tmp92B,sizeof(char),2)))),((_tmp92A[1]=
# 1460
_tmp3E3 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(_tmp3E3),((_tmp92A[0]=
# 1459
Cyc_PP_text(((_tmp92C="return ",_tag_dyneither(_tmp92C,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp92A,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1462
goto _LL239;_LL242: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp3E4=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3DB;if(_tmp3E4->tag != 4)goto _LL244;else{_tmp3E5=_tmp3E4->f1;_tmp3E6=_tmp3E4->f2;_tmp3E7=_tmp3E4->f3;}}_LL243: {
# 1464
int print_else;
{void*_tmp41B=_tmp3E7->r;void*_tmp41C=_tmp41B;_LL25F: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp41D=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp41C;if(_tmp41D->tag != 0)goto _LL261;}_LL260:
 print_else=0;goto _LL25E;_LL261:;_LL262:
 print_else=1;goto _LL25E;_LL25E:;}
# 1469
{const char*_tmp93B;const char*_tmp93A;struct Cyc_PP_Doc*_tmp939[2];struct Cyc_PP_Doc*_tmp938[6];struct Cyc_PP_Doc*_tmp937[2];const char*_tmp936;struct Cyc_PP_Doc*_tmp935[8];s=((_tmp935[7]=
# 1476
print_else?
(_tmp938[5]=
# 1482
Cyc_Absynpp_rb(),((_tmp938[4]=
# 1481
Cyc_PP_line_doc(),((_tmp938[3]=
# 1480
Cyc_PP_nest(2,((_tmp937[1]=Cyc_Absynpp_stmt2doc(_tmp3E7,0),((_tmp937[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp937,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp938[2]=
# 1479
Cyc_Absynpp_lb(),((_tmp938[1]=
# 1478
Cyc_PP_text(((_tmp936="else ",_tag_dyneither(_tmp936,sizeof(char),6)))),((_tmp938[0]=
# 1477
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp938,sizeof(struct Cyc_PP_Doc*),6))))))))))))):
# 1483
 Cyc_PP_nil_doc(),((_tmp935[6]=
# 1475
Cyc_Absynpp_rb(),((_tmp935[5]=
# 1474
Cyc_PP_line_doc(),((_tmp935[4]=
# 1473
Cyc_PP_nest(2,((_tmp939[1]=Cyc_Absynpp_stmt2doc(_tmp3E6,0),((_tmp939[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp939,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp935[3]=
# 1472
Cyc_Absynpp_lb(),((_tmp935[2]=
# 1471
Cyc_PP_text(((_tmp93A=") ",_tag_dyneither(_tmp93A,sizeof(char),3)))),((_tmp935[1]=
# 1470
Cyc_Absynpp_exp2doc(_tmp3E5),((_tmp935[0]=
# 1469
Cyc_PP_text(((_tmp93B="if (",_tag_dyneither(_tmp93B,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp935,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1484
goto _LL239;}_LL244: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp3E8=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3DB;if(_tmp3E8->tag != 5)goto _LL246;else{_tmp3E9=(_tmp3E8->f1).f1;_tmp3EA=_tmp3E8->f2;}}_LL245:
# 1486
{const char*_tmp942;const char*_tmp941;struct Cyc_PP_Doc*_tmp940[2];struct Cyc_PP_Doc*_tmp93F[7];s=((_tmp93F[6]=
# 1492
Cyc_Absynpp_rb(),((_tmp93F[5]=
# 1491
Cyc_PP_line_doc(),((_tmp93F[4]=
# 1490
Cyc_PP_nest(2,((_tmp940[1]=Cyc_Absynpp_stmt2doc(_tmp3EA,0),((_tmp940[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp940,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp93F[3]=
# 1489
Cyc_Absynpp_lb(),((_tmp93F[2]=
# 1488
Cyc_PP_text(((_tmp941=") ",_tag_dyneither(_tmp941,sizeof(char),3)))),((_tmp93F[1]=
# 1487
Cyc_Absynpp_exp2doc(_tmp3E9),((_tmp93F[0]=
# 1486
Cyc_PP_text(((_tmp942="while (",_tag_dyneither(_tmp942,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp93F,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1493
goto _LL239;_LL246: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp3EB=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp3DB;if(_tmp3EB->tag != 6)goto _LL248;}_LL247:
{const char*_tmp943;s=Cyc_PP_text(((_tmp943="break;",_tag_dyneither(_tmp943,sizeof(char),7))));}goto _LL239;_LL248: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp3EC=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp3DB;if(_tmp3EC->tag != 7)goto _LL24A;}_LL249:
{const char*_tmp944;s=Cyc_PP_text(((_tmp944="continue;",_tag_dyneither(_tmp944,sizeof(char),10))));}goto _LL239;_LL24A: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp3ED=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp3DB;if(_tmp3ED->tag != 8)goto _LL24C;else{_tmp3EE=_tmp3ED->f1;}}_LL24B:
{const char*_tmp948;void*_tmp947[1];struct Cyc_String_pa_PrintArg_struct _tmp946;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp946.tag=0,((_tmp946.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3EE),((_tmp947[0]=& _tmp946,Cyc_aprintf(((_tmp948="goto %s;",_tag_dyneither(_tmp948,sizeof(char),9))),_tag_dyneither(_tmp947,sizeof(void*),1)))))))));}goto _LL239;_LL24C: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp3EF=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3DB;if(_tmp3EF->tag != 9)goto _LL24E;else{_tmp3F0=_tmp3EF->f1;_tmp3F1=(_tmp3EF->f2).f1;_tmp3F2=(_tmp3EF->f3).f1;_tmp3F3=_tmp3EF->f4;}}_LL24D:
# 1498
{const char*_tmp953;const char*_tmp952;const char*_tmp951;const char*_tmp950;struct Cyc_PP_Doc*_tmp94F[2];struct Cyc_PP_Doc*_tmp94E[11];s=((_tmp94E[10]=
# 1508
Cyc_Absynpp_rb(),((_tmp94E[9]=
# 1507
Cyc_PP_line_doc(),((_tmp94E[8]=
# 1506
Cyc_PP_nest(2,((_tmp94F[1]=Cyc_Absynpp_stmt2doc(_tmp3F3,0),((_tmp94F[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp94F,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp94E[7]=
# 1505
Cyc_Absynpp_lb(),((_tmp94E[6]=
# 1504
Cyc_PP_text(((_tmp950=") ",_tag_dyneither(_tmp950,sizeof(char),3)))),((_tmp94E[5]=
# 1503
Cyc_Absynpp_exp2doc(_tmp3F2),((_tmp94E[4]=
# 1502
Cyc_PP_text(((_tmp951="; ",_tag_dyneither(_tmp951,sizeof(char),3)))),((_tmp94E[3]=
# 1501
Cyc_Absynpp_exp2doc(_tmp3F1),((_tmp94E[2]=
# 1500
Cyc_PP_text(((_tmp952="; ",_tag_dyneither(_tmp952,sizeof(char),3)))),((_tmp94E[1]=
# 1499
Cyc_Absynpp_exp2doc(_tmp3F0),((_tmp94E[0]=
# 1498
Cyc_PP_text(((_tmp953="for(",_tag_dyneither(_tmp953,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp94E,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1509
goto _LL239;_LL24E: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp3F4=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3DB;if(_tmp3F4->tag != 10)goto _LL250;else{_tmp3F5=_tmp3F4->f1;_tmp3F6=_tmp3F4->f2;}}_LL24F:
# 1511
{const char*_tmp958;const char*_tmp957;struct Cyc_PP_Doc*_tmp956[8];s=((_tmp956[7]=
# 1518
Cyc_Absynpp_rb(),((_tmp956[6]=
# 1517
Cyc_PP_line_doc(),((_tmp956[5]=
# 1516
Cyc_Absynpp_switchclauses2doc(_tmp3F6),((_tmp956[4]=
# 1515
Cyc_PP_line_doc(),((_tmp956[3]=
# 1514
Cyc_Absynpp_lb(),((_tmp956[2]=
# 1513
Cyc_PP_text(((_tmp957=") ",_tag_dyneither(_tmp957,sizeof(char),3)))),((_tmp956[1]=
# 1512
Cyc_Absynpp_exp2doc(_tmp3F5),((_tmp956[0]=
# 1511
Cyc_PP_text(((_tmp958="switch (",_tag_dyneither(_tmp958,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp956,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1519
goto _LL239;_LL250: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3F7=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3DB;if(_tmp3F7->tag != 11)goto _LL252;else{if(_tmp3F7->f1 != 0)goto _LL252;}}_LL251:
{const char*_tmp959;s=Cyc_PP_text(((_tmp959="fallthru;",_tag_dyneither(_tmp959,sizeof(char),10))));}goto _LL239;_LL252: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3F8=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3DB;if(_tmp3F8->tag != 11)goto _LL254;else{_tmp3F9=_tmp3F8->f1;}}_LL253:
# 1522
{const char*_tmp95E;const char*_tmp95D;struct Cyc_PP_Doc*_tmp95C[3];s=((_tmp95C[2]=Cyc_PP_text(((_tmp95D=");",_tag_dyneither(_tmp95D,sizeof(char),3)))),((_tmp95C[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp3F9),((_tmp95C[0]=Cyc_PP_text(((_tmp95E="fallthru(",_tag_dyneither(_tmp95E,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp95C,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL239;_LL254: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3FA=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3DB;if(_tmp3FA->tag != 12)goto _LL256;else{_tmp3FB=_tmp3FA->f1;_tmp3FC=_tmp3FA->f2;}}_LL255:
# 1524
{struct Cyc_PP_Doc*_tmp95F[3];s=((_tmp95F[2]=
# 1526
Cyc_Absynpp_stmt2doc(_tmp3FC,stmtexp),((_tmp95F[1]=
# 1525
Cyc_PP_line_doc(),((_tmp95F[0]=
# 1524
Cyc_Absynpp_decl2doc(_tmp3FB),Cyc_PP_cat(_tag_dyneither(_tmp95F,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1527
goto _LL239;_LL256: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp3FD=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3DB;if(_tmp3FD->tag != 13)goto _LL258;else{_tmp3FE=_tmp3FD->f1;_tmp3FF=_tmp3FD->f2;}}_LL257:
# 1529
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3FF)){
if(stmtexp){
const char*_tmp964;const char*_tmp963;struct Cyc_PP_Doc*_tmp962[8];s=((_tmp962[7]=
# 1537
Cyc_PP_text(((_tmp963=");",_tag_dyneither(_tmp963,sizeof(char),3)))),((_tmp962[6]=Cyc_Absynpp_rb(),((_tmp962[5]=
# 1536
Cyc_PP_line_doc(),((_tmp962[4]=
# 1535
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3FF,1)),((_tmp962[3]=
# 1534
Cyc_PP_line_doc(),((_tmp962[2]=
# 1533
Cyc_Absynpp_lb(),((_tmp962[1]=
# 1532
Cyc_PP_text(((_tmp964=": (",_tag_dyneither(_tmp964,sizeof(char),4)))),((_tmp962[0]=
# 1531
Cyc_PP_textptr(_tmp3FE),Cyc_PP_cat(_tag_dyneither(_tmp962,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1539
const char*_tmp967;struct Cyc_PP_Doc*_tmp966[7];s=((_tmp966[6]=
# 1545
Cyc_Absynpp_rb(),((_tmp966[5]=
# 1544
Cyc_PP_line_doc(),((_tmp966[4]=
# 1543
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3FF,0)),((_tmp966[3]=
# 1542
Cyc_PP_line_doc(),((_tmp966[2]=
# 1541
Cyc_Absynpp_lb(),((_tmp966[1]=
# 1540
Cyc_PP_text(((_tmp967=": ",_tag_dyneither(_tmp967,sizeof(char),3)))),((_tmp966[0]=
# 1539
Cyc_PP_textptr(_tmp3FE),Cyc_PP_cat(_tag_dyneither(_tmp966,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1547
const char*_tmp96A;struct Cyc_PP_Doc*_tmp969[3];s=((_tmp969[2]=Cyc_Absynpp_stmt2doc(_tmp3FF,stmtexp),((_tmp969[1]=Cyc_PP_text(((_tmp96A=": ",_tag_dyneither(_tmp96A,sizeof(char),3)))),((_tmp969[0]=Cyc_PP_textptr(_tmp3FE),Cyc_PP_cat(_tag_dyneither(_tmp969,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL239;_LL258: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp400=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3DB;if(_tmp400->tag != 14)goto _LL25A;else{_tmp401=_tmp400->f1;_tmp402=(_tmp400->f2).f1;}}_LL259:
# 1550
{const char*_tmp971;const char*_tmp970;const char*_tmp96F;struct Cyc_PP_Doc*_tmp96E[9];s=((_tmp96E[8]=
# 1558
Cyc_PP_text(((_tmp96F=");",_tag_dyneither(_tmp96F,sizeof(char),3)))),((_tmp96E[7]=
# 1557
Cyc_Absynpp_exp2doc(_tmp402),((_tmp96E[6]=
# 1556
Cyc_PP_text(((_tmp970=" while (",_tag_dyneither(_tmp970,sizeof(char),9)))),((_tmp96E[5]=
# 1555
Cyc_Absynpp_rb(),((_tmp96E[4]=
# 1554
Cyc_PP_line_doc(),((_tmp96E[3]=
# 1553
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp401,0)),((_tmp96E[2]=
# 1552
Cyc_PP_line_doc(),((_tmp96E[1]=
# 1551
Cyc_Absynpp_lb(),((_tmp96E[0]=
# 1550
Cyc_PP_text(((_tmp971="do ",_tag_dyneither(_tmp971,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp96E,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1559
goto _LL239;_LL25A: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp403=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3DB;if(_tmp403->tag != 15)goto _LL25C;else{_tmp404=_tmp403->f1;_tmp405=_tmp403->f2;}}_LL25B:
# 1561
{const char*_tmp976;const char*_tmp975;struct Cyc_PP_Doc*_tmp974[12];s=((_tmp974[11]=
# 1572
Cyc_Absynpp_rb(),((_tmp974[10]=
# 1571
Cyc_PP_line_doc(),((_tmp974[9]=
# 1570
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp405)),((_tmp974[8]=
# 1569
Cyc_PP_line_doc(),((_tmp974[7]=
# 1568
Cyc_Absynpp_lb(),((_tmp974[6]=
# 1567
Cyc_PP_text(((_tmp975=" catch ",_tag_dyneither(_tmp975,sizeof(char),8)))),((_tmp974[5]=
# 1566
Cyc_Absynpp_rb(),((_tmp974[4]=
# 1565
Cyc_PP_line_doc(),((_tmp974[3]=
# 1564
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp404,0)),((_tmp974[2]=
# 1563
Cyc_PP_line_doc(),((_tmp974[1]=
# 1562
Cyc_Absynpp_lb(),((_tmp974[0]=
# 1561
Cyc_PP_text(((_tmp976="try ",_tag_dyneither(_tmp976,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp974,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
# 1573
goto _LL239;_LL25C: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp406=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp3DB;if(_tmp406->tag != 16)goto _LL239;else{_tmp407=_tmp406->f1;}}_LL25D:
# 1575
{const char*_tmp97B;const char*_tmp97A;struct Cyc_PP_Doc*_tmp979[3];s=((_tmp979[2]=Cyc_PP_text(((_tmp97A=");",_tag_dyneither(_tmp97A,sizeof(char),3)))),((_tmp979[1]=Cyc_Absynpp_exp2doc(_tmp407),((_tmp979[0]=Cyc_PP_text(((_tmp97B="reset_region(",_tag_dyneither(_tmp97B,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp979,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL239;_LL239:;}
# 1578
return s;}
# 1581
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp44D=p->r;void*_tmp44E=_tmp44D;enum Cyc_Absyn_Sign _tmp452;int _tmp453;char _tmp455;struct _dyneither_ptr _tmp457;struct Cyc_Absyn_Vardecl*_tmp459;struct Cyc_Absyn_Vardecl*_tmp45C;struct Cyc_Absyn_Pat*_tmp45D;struct Cyc_Absyn_Tvar*_tmp45F;struct Cyc_Absyn_Vardecl*_tmp460;struct Cyc_Absyn_Tvar*_tmp462;struct Cyc_Absyn_Vardecl*_tmp463;struct Cyc_List_List*_tmp465;int _tmp466;struct Cyc_Absyn_Pat*_tmp468;struct Cyc_Absyn_Vardecl*_tmp46A;struct Cyc_Absyn_Vardecl*_tmp46D;struct Cyc_Absyn_Pat*_tmp46E;struct _tuple0*_tmp470;struct _tuple0*_tmp472;struct Cyc_List_List*_tmp473;int _tmp474;union Cyc_Absyn_AggrInfoU _tmp476;struct Cyc_List_List*_tmp477;struct Cyc_List_List*_tmp478;int _tmp479;struct Cyc_List_List*_tmp47B;struct Cyc_List_List*_tmp47C;int _tmp47D;struct Cyc_Absyn_Enumfield*_tmp47F;struct Cyc_Absyn_Enumfield*_tmp481;struct Cyc_Absyn_Datatypefield*_tmp483;struct Cyc_Absyn_Datatypefield*_tmp485;struct Cyc_List_List*_tmp486;int _tmp487;struct Cyc_Absyn_Exp*_tmp489;_LL264: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp44F=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp44F->tag != 0)goto _LL266;}_LL265:
{const char*_tmp97C;s=Cyc_PP_text(((_tmp97C="_",_tag_dyneither(_tmp97C,sizeof(char),2))));}goto _LL263;_LL266: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp450=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp450->tag != 9)goto _LL268;}_LL267:
{const char*_tmp97D;s=Cyc_PP_text(((_tmp97D="NULL",_tag_dyneither(_tmp97D,sizeof(char),5))));}goto _LL263;_LL268: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp451=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp451->tag != 10)goto _LL26A;else{_tmp452=_tmp451->f1;_tmp453=_tmp451->f2;}}_LL269:
# 1587
 if(_tmp452 != Cyc_Absyn_Unsigned){
const char*_tmp981;void*_tmp980[1];struct Cyc_Int_pa_PrintArg_struct _tmp97F;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp97F.tag=1,((_tmp97F.f1=(unsigned long)_tmp453,((_tmp980[0]=& _tmp97F,Cyc_aprintf(((_tmp981="%d",_tag_dyneither(_tmp981,sizeof(char),3))),_tag_dyneither(_tmp980,sizeof(void*),1)))))))));}else{
const char*_tmp985;void*_tmp984[1];struct Cyc_Int_pa_PrintArg_struct _tmp983;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp983.tag=1,((_tmp983.f1=(unsigned int)_tmp453,((_tmp984[0]=& _tmp983,Cyc_aprintf(((_tmp985="%u",_tag_dyneither(_tmp985,sizeof(char),3))),_tag_dyneither(_tmp984,sizeof(void*),1)))))))));}
goto _LL263;_LL26A: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp454=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp454->tag != 11)goto _LL26C;else{_tmp455=_tmp454->f1;}}_LL26B:
{const char*_tmp989;void*_tmp988[1];struct Cyc_String_pa_PrintArg_struct _tmp987;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp987.tag=0,((_tmp987.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp455)),((_tmp988[0]=& _tmp987,Cyc_aprintf(((_tmp989="'%s'",_tag_dyneither(_tmp989,sizeof(char),5))),_tag_dyneither(_tmp988,sizeof(void*),1)))))))));}goto _LL263;_LL26C: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp456=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp456->tag != 12)goto _LL26E;else{_tmp457=_tmp456->f1;}}_LL26D:
 s=Cyc_PP_text(_tmp457);goto _LL263;_LL26E: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp458=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp458->tag != 1)goto _LL270;else{_tmp459=_tmp458->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp45A=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp458->f2)->r;if(_tmp45A->tag != 0)goto _LL270;};}}_LL26F:
# 1594
 s=Cyc_Absynpp_qvar2doc(_tmp459->name);goto _LL263;_LL270: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp45B=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp45B->tag != 1)goto _LL272;else{_tmp45C=_tmp45B->f1;_tmp45D=_tmp45B->f2;}}_LL271:
# 1596
{const char*_tmp98C;struct Cyc_PP_Doc*_tmp98B[3];s=((_tmp98B[2]=Cyc_Absynpp_pat2doc(_tmp45D),((_tmp98B[1]=Cyc_PP_text(((_tmp98C=" as ",_tag_dyneither(_tmp98C,sizeof(char),5)))),((_tmp98B[0]=Cyc_Absynpp_qvar2doc(_tmp45C->name),Cyc_PP_cat(_tag_dyneither(_tmp98B,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL263;_LL272: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp45E=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp45E->tag != 2)goto _LL274;else{_tmp45F=_tmp45E->f1;_tmp460=_tmp45E->f2;}}_LL273:
# 1598
{const char*_tmp993;const char*_tmp992;const char*_tmp991;struct Cyc_PP_Doc*_tmp990[5];s=((_tmp990[4]=
Cyc_Absynpp_vardecl2doc(_tmp460),((_tmp990[3]=
# 1598
Cyc_PP_text(((_tmp991="> ",_tag_dyneither(_tmp991,sizeof(char),3)))),((_tmp990[2]=Cyc_Absynpp_tvar2doc(_tmp45F),((_tmp990[1]=Cyc_PP_text(((_tmp992=" <",_tag_dyneither(_tmp992,sizeof(char),3)))),((_tmp990[0]=Cyc_PP_text(((_tmp993="alias",_tag_dyneither(_tmp993,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp990,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1600
goto _LL263;_LL274: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp461=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp461->tag != 4)goto _LL276;else{_tmp462=_tmp461->f1;_tmp463=_tmp461->f2;}}_LL275:
# 1602
{const char*_tmp998;const char*_tmp997;struct Cyc_PP_Doc*_tmp996[4];s=((_tmp996[3]=Cyc_PP_text(((_tmp997=">",_tag_dyneither(_tmp997,sizeof(char),2)))),((_tmp996[2]=Cyc_Absynpp_tvar2doc(_tmp462),((_tmp996[1]=Cyc_PP_text(((_tmp998="<",_tag_dyneither(_tmp998,sizeof(char),2)))),((_tmp996[0]=Cyc_Absynpp_qvar2doc(_tmp463->name),Cyc_PP_cat(_tag_dyneither(_tmp996,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL263;_LL276: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp464=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp464->tag != 5)goto _LL278;else{_tmp465=_tmp464->f1;_tmp466=_tmp464->f2;}}_LL277:
# 1605
{const char*_tmp9A1;const char*_tmp9A0;const char*_tmp99F;const char*_tmp99E;struct Cyc_PP_Doc*_tmp99D[4];s=((_tmp99D[3]=
_tmp466?Cyc_PP_text(((_tmp99E=", ...)",_tag_dyneither(_tmp99E,sizeof(char),7)))): Cyc_PP_text(((_tmp99F=")",_tag_dyneither(_tmp99F,sizeof(char),2)))),((_tmp99D[2]=
# 1605
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp9A0=",",_tag_dyneither(_tmp9A0,sizeof(char),2))),_tmp465),((_tmp99D[1]=Cyc_PP_text(((_tmp9A1="(",_tag_dyneither(_tmp9A1,sizeof(char),2)))),((_tmp99D[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp99D,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1607
goto _LL263;_LL278: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp467=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp467->tag != 6)goto _LL27A;else{_tmp468=_tmp467->f1;}}_LL279:
# 1609
{const char*_tmp9A4;struct Cyc_PP_Doc*_tmp9A3[2];s=((_tmp9A3[1]=Cyc_Absynpp_pat2doc(_tmp468),((_tmp9A3[0]=Cyc_PP_text(((_tmp9A4="&",_tag_dyneither(_tmp9A4,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp9A3,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL263;_LL27A: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp469=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp469->tag != 3)goto _LL27C;else{_tmp46A=_tmp469->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp46B=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp469->f2)->r;if(_tmp46B->tag != 0)goto _LL27C;};}}_LL27B:
# 1612
{const char*_tmp9A7;struct Cyc_PP_Doc*_tmp9A6[2];s=((_tmp9A6[1]=Cyc_Absynpp_qvar2doc(_tmp46A->name),((_tmp9A6[0]=Cyc_PP_text(((_tmp9A7="*",_tag_dyneither(_tmp9A7,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp9A6,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL263;_LL27C: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp46C=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp46C->tag != 3)goto _LL27E;else{_tmp46D=_tmp46C->f1;_tmp46E=_tmp46C->f2;}}_LL27D:
# 1615
{const char*_tmp9AC;const char*_tmp9AB;struct Cyc_PP_Doc*_tmp9AA[4];s=((_tmp9AA[3]=Cyc_Absynpp_pat2doc(_tmp46E),((_tmp9AA[2]=Cyc_PP_text(((_tmp9AB=" as ",_tag_dyneither(_tmp9AB,sizeof(char),5)))),((_tmp9AA[1]=Cyc_Absynpp_qvar2doc(_tmp46D->name),((_tmp9AA[0]=Cyc_PP_text(((_tmp9AC="*",_tag_dyneither(_tmp9AC,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp9AA,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL263;_LL27E: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp46F=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp46F->tag != 15)goto _LL280;else{_tmp470=_tmp46F->f1;}}_LL27F:
# 1618
 s=Cyc_Absynpp_qvar2doc(_tmp470);
goto _LL263;_LL280: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp471=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp471->tag != 16)goto _LL282;else{_tmp472=_tmp471->f1;_tmp473=_tmp471->f2;_tmp474=_tmp471->f3;}}_LL281: {
# 1621
const char*_tmp9AE;const char*_tmp9AD;struct _dyneither_ptr term=_tmp474?(_tmp9AE=", ...)",_tag_dyneither(_tmp9AE,sizeof(char),7)):((_tmp9AD=")",_tag_dyneither(_tmp9AD,sizeof(char),2)));
{const char*_tmp9B3;const char*_tmp9B2;struct Cyc_PP_Doc*_tmp9B1[2];s=((_tmp9B1[1]=Cyc_PP_group(((_tmp9B3="(",_tag_dyneither(_tmp9B3,sizeof(char),2))),term,((_tmp9B2=",",_tag_dyneither(_tmp9B2,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp473)),((_tmp9B1[0]=Cyc_Absynpp_qvar2doc(_tmp472),Cyc_PP_cat(_tag_dyneither(_tmp9B1,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL263;}_LL282: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp475=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp475->tag != 7)goto _LL284;else{if(_tmp475->f1 == 0)goto _LL284;_tmp476=(_tmp475->f1)->aggr_info;_tmp477=_tmp475->f2;_tmp478=_tmp475->f3;_tmp479=_tmp475->f4;}}_LL283: {
# 1625
const char*_tmp9B5;const char*_tmp9B4;struct _dyneither_ptr term=_tmp479?(_tmp9B5=", ...}",_tag_dyneither(_tmp9B5,sizeof(char),7)):((_tmp9B4="}",_tag_dyneither(_tmp9B4,sizeof(char),2)));
struct _tuple10 _tmp4AF=Cyc_Absyn_aggr_kinded_name(_tmp476);struct _tuple0*_tmp4B1;struct _tuple10 _tmp4B0=_tmp4AF;_tmp4B1=_tmp4B0.f2;
{const char*_tmp9C0;const char*_tmp9BF;const char*_tmp9BE;const char*_tmp9BD;const char*_tmp9BC;struct Cyc_PP_Doc*_tmp9BB[4];s=((_tmp9BB[3]=
# 1629
Cyc_PP_group(((_tmp9BD="",_tag_dyneither(_tmp9BD,sizeof(char),1))),term,((_tmp9BC=",",_tag_dyneither(_tmp9BC,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp478)),((_tmp9BB[2]=
# 1628
Cyc_PP_egroup(((_tmp9C0="[",_tag_dyneither(_tmp9C0,sizeof(char),2))),((_tmp9BF="]",_tag_dyneither(_tmp9BF,sizeof(char),2))),((_tmp9BE=",",_tag_dyneither(_tmp9BE,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp477)),((_tmp9BB[1]=
# 1627
Cyc_Absynpp_lb(),((_tmp9BB[0]=Cyc_Absynpp_qvar2doc(_tmp4B1),Cyc_PP_cat(_tag_dyneither(_tmp9BB,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1630
goto _LL263;}_LL284: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp47A=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp47A->tag != 7)goto _LL286;else{if(_tmp47A->f1 != 0)goto _LL286;_tmp47B=_tmp47A->f2;_tmp47C=_tmp47A->f3;_tmp47D=_tmp47A->f4;}}_LL285: {
# 1632
const char*_tmp9C2;const char*_tmp9C1;struct _dyneither_ptr term=_tmp47D?(_tmp9C2=", ...}",_tag_dyneither(_tmp9C2,sizeof(char),7)):((_tmp9C1="}",_tag_dyneither(_tmp9C1,sizeof(char),2)));
{const char*_tmp9CD;const char*_tmp9CC;const char*_tmp9CB;const char*_tmp9CA;const char*_tmp9C9;struct Cyc_PP_Doc*_tmp9C8[3];s=((_tmp9C8[2]=
# 1635
Cyc_PP_group(((_tmp9CA="",_tag_dyneither(_tmp9CA,sizeof(char),1))),term,((_tmp9C9=",",_tag_dyneither(_tmp9C9,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp47C)),((_tmp9C8[1]=
# 1634
Cyc_PP_egroup(((_tmp9CD="[",_tag_dyneither(_tmp9CD,sizeof(char),2))),((_tmp9CC="]",_tag_dyneither(_tmp9CC,sizeof(char),2))),((_tmp9CB=",",_tag_dyneither(_tmp9CB,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp47B)),((_tmp9C8[0]=
# 1633
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp9C8,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1636
goto _LL263;}_LL286: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp47E=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp47E->tag != 13)goto _LL288;else{_tmp47F=_tmp47E->f2;}}_LL287:
 _tmp481=_tmp47F;goto _LL289;_LL288: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp480=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp480->tag != 14)goto _LL28A;else{_tmp481=_tmp480->f2;}}_LL289:
 s=Cyc_Absynpp_qvar2doc(_tmp481->name);goto _LL263;_LL28A: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp482=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp482->tag != 8)goto _LL28C;else{_tmp483=_tmp482->f2;if(_tmp482->f3 != 0)goto _LL28C;}}_LL28B:
 s=Cyc_Absynpp_qvar2doc(_tmp483->name);goto _LL263;_LL28C: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp484=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp484->tag != 8)goto _LL28E;else{_tmp485=_tmp484->f2;_tmp486=_tmp484->f3;_tmp487=_tmp484->f4;}}_LL28D: {
# 1641
const char*_tmp9CF;const char*_tmp9CE;struct _dyneither_ptr term=_tmp487?(_tmp9CF=", ...)",_tag_dyneither(_tmp9CF,sizeof(char),7)):((_tmp9CE=")",_tag_dyneither(_tmp9CE,sizeof(char),2)));
{const char*_tmp9D4;const char*_tmp9D3;struct Cyc_PP_Doc*_tmp9D2[2];s=((_tmp9D2[1]=Cyc_PP_egroup(((_tmp9D4="(",_tag_dyneither(_tmp9D4,sizeof(char),2))),term,((_tmp9D3=",",_tag_dyneither(_tmp9D3,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp486)),((_tmp9D2[0]=Cyc_Absynpp_qvar2doc(_tmp485->name),Cyc_PP_cat(_tag_dyneither(_tmp9D2,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL263;}_LL28E: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp488=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp44E;if(_tmp488->tag != 17)goto _LL263;else{_tmp489=_tmp488->f1;}}_LL28F:
# 1645
 s=Cyc_Absynpp_exp2doc(_tmp489);goto _LL263;_LL263:;}
# 1647
return s;}
# 1650
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
const char*_tmp9DB;const char*_tmp9DA;const char*_tmp9D9;struct Cyc_PP_Doc*_tmp9D8[2];return(_tmp9D8[1]=
Cyc_Absynpp_pat2doc((*dp).f2),((_tmp9D8[0]=
# 1651
Cyc_PP_egroup(((_tmp9DB="",_tag_dyneither(_tmp9DB,sizeof(char),1))),((_tmp9DA="=",_tag_dyneither(_tmp9DA,sizeof(char),2))),((_tmp9D9="=",_tag_dyneither(_tmp9D9,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp9D8,sizeof(struct Cyc_PP_Doc*),2)))));}
# 1655
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){
const char*_tmp9E0;struct Cyc_PP_Doc*_tmp9DF[2];struct Cyc_PP_Doc*_tmp9DE[2];return(_tmp9DE[1]=
Cyc_PP_nest(2,((_tmp9DF[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp9DF[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9DF,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9DE[0]=
# 1657
Cyc_PP_text(((_tmp9E0="default: ",_tag_dyneither(_tmp9E0,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp9DE,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 1659
if(c->where_clause == 0){
const char*_tmp9E7;const char*_tmp9E6;struct Cyc_PP_Doc*_tmp9E5[2];struct Cyc_PP_Doc*_tmp9E4[4];return(_tmp9E4[3]=
# 1663
Cyc_PP_nest(2,((_tmp9E5[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp9E5[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9E5,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9E4[2]=
# 1662
Cyc_PP_text(((_tmp9E6=": ",_tag_dyneither(_tmp9E6,sizeof(char),3)))),((_tmp9E4[1]=
# 1661
Cyc_Absynpp_pat2doc(c->pattern),((_tmp9E4[0]=
# 1660
Cyc_PP_text(((_tmp9E7="case ",_tag_dyneither(_tmp9E7,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9E4,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1665
const char*_tmp9F0;const char*_tmp9EF;const char*_tmp9EE;struct Cyc_PP_Doc*_tmp9ED[2];struct Cyc_PP_Doc*_tmp9EC[6];return(_tmp9EC[5]=
# 1670
Cyc_PP_nest(2,((_tmp9ED[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp9ED[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9ED,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9EC[4]=
# 1669
Cyc_PP_text(((_tmp9EE=": ",_tag_dyneither(_tmp9EE,sizeof(char),3)))),((_tmp9EC[3]=
# 1668
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp9EC[2]=
# 1667
Cyc_PP_text(((_tmp9EF=" && ",_tag_dyneither(_tmp9EF,sizeof(char),5)))),((_tmp9EC[1]=
# 1666
Cyc_Absynpp_pat2doc(c->pattern),((_tmp9EC[0]=
# 1665
Cyc_PP_text(((_tmp9F0="case ",_tag_dyneither(_tmp9F0,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9EC,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
# 1673
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
const char*_tmp9F1;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((_tmp9F1="",_tag_dyneither(_tmp9F1,sizeof(char),1))),cs);}
# 1677
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1681
const char*_tmp9F4;struct Cyc_PP_Doc*_tmp9F3[3];return(_tmp9F3[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp9F3[1]=Cyc_PP_text(((_tmp9F4=" = ",_tag_dyneither(_tmp9F4,sizeof(char),4)))),((_tmp9F3[0]=Cyc_Absynpp_qvar2doc(f->name),Cyc_PP_cat(_tag_dyneither(_tmp9F3,sizeof(struct Cyc_PP_Doc*),3)))))));}}
# 1684
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
const char*_tmp9F5;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp9F5=",",_tag_dyneither(_tmp9F5,sizeof(char),2))),fs);}
# 1688
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1692
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
const char*_tmp9F6;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,((_tmp9F6=",",_tag_dyneither(_tmp9F6,sizeof(char),2))),vds);}
# 1696
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
enum Cyc_Absyn_Scope _tmp4DD;struct _tuple0*_tmp4DE;struct Cyc_Absyn_Tqual _tmp4DF;void*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4E1;struct Cyc_List_List*_tmp4E2;struct Cyc_Absyn_Vardecl*_tmp4DC=vd;_tmp4DD=_tmp4DC->sc;_tmp4DE=_tmp4DC->name;_tmp4DF=_tmp4DC->tq;_tmp4E0=_tmp4DC->type;_tmp4E1=_tmp4DC->initializer;_tmp4E2=_tmp4DC->attributes;{
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp4DE);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4E2);
struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL290:
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL291:
# 1705
{void*_tmp4E3=Cyc_Tcutil_compress(_tmp4E0);void*_tmp4E4=_tmp4E3;struct Cyc_List_List*_tmp4E6;_LL294: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4E5=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4E4;if(_tmp4E5->tag != 9)goto _LL296;else{_tmp4E6=(_tmp4E5->f1).attributes;}}_LL295:
# 1707
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4E6);
goto _LL293;_LL296:;_LL297:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL293;_LL293:;}
# 1711
break;}{
# 1714
struct Cyc_PP_Doc*tmp_doc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL298:
 tmp_doc=Cyc_PP_nil_doc();break;case Cyc_Cyclone_Vc_c: _LL299:
 tmp_doc=attsdoc;break;}
# 1719
{struct Cyc_Core_Opt*_tmpA03;struct Cyc_PP_Doc*_tmpA02[2];struct Cyc_PP_Doc*_tmpA01[2];const char*_tmpA00;const char*_tmp9FF;struct Cyc_PP_Doc*_tmp9FE[5];s=((_tmp9FE[4]=
# 1726
Cyc_PP_text(((_tmp9FF=";",_tag_dyneither(_tmp9FF,sizeof(char),2)))),((_tmp9FE[3]=
# 1723
_tmp4E1 == 0?
Cyc_PP_nil_doc():(
(_tmpA01[1]=Cyc_Absynpp_exp2doc(_tmp4E1),((_tmpA01[0]=Cyc_PP_text(((_tmpA00=" = ",_tag_dyneither(_tmpA00,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmpA01,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9FE[2]=
# 1722
Cyc_Absynpp_tqtd2doc(_tmp4DF,_tmp4E0,((_tmpA03=_cycalloc(sizeof(*_tmpA03)),((_tmpA03->v=((_tmpA02[1]=sn,((_tmpA02[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmpA02,sizeof(struct Cyc_PP_Doc*),2)))))),_tmpA03))))),((_tmp9FE[1]=
# 1721
Cyc_Absynpp_scope2doc(_tmp4DD),((_tmp9FE[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp9FE,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1727
return s;};};}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1730
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple17 _tmp4ED=*x;struct _tuple0*_tmp4EF;struct _tuple17 _tmp4EE=_tmp4ED;_tmp4EF=_tmp4EE.f2;
return Cyc_Absynpp_qvar2doc(_tmp4EF);}
# 1735
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0){
struct Cyc_PP_Doc*_tmpA04[4];return(_tmpA04[3]=
# 1740
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmpA04[2]=
# 1739
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmpA04[1]=
# 1738
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmpA04[0]=
# 1737
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmpA04,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1742
struct Cyc_PP_Doc*_tmpA0C[2];const char*_tmpA0B;struct Cyc_PP_Doc*_tmpA0A[2];struct Cyc_PP_Doc*_tmpA09[12];return(_tmpA09[11]=
# 1753
Cyc_Absynpp_atts2doc(ad->attributes),((_tmpA09[10]=
# 1752
Cyc_Absynpp_rb(),((_tmpA09[9]=
# 1751
Cyc_PP_line_doc(),((_tmpA09[8]=
# 1750
Cyc_PP_nest(2,((_tmpA0A[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields),((_tmpA0A[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA0A,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmpA09[7]=
# 1748
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():(
(_tmpA0C[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po),((_tmpA0C[0]=Cyc_PP_text(((_tmpA0B=":",_tag_dyneither(_tmpA0B,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmpA0C,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmpA09[6]=
# 1747
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars),((_tmpA09[5]=
# 1746
Cyc_Absynpp_lb(),((_tmpA09[4]=Cyc_PP_blank_doc(),((_tmpA09[3]=
# 1745
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmpA09[2]=
# 1744
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmpA09[1]=
# 1743
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmpA09[0]=
# 1742
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmpA09,sizeof(struct Cyc_PP_Doc*),12)))))))))))))))))))))))));}}
# 1756
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
enum Cyc_Absyn_Scope _tmp4F6;struct _tuple0*_tmp4F7;struct Cyc_List_List*_tmp4F8;struct Cyc_Core_Opt*_tmp4F9;int _tmp4FA;struct Cyc_Absyn_Datatypedecl*_tmp4F5=dd;_tmp4F6=_tmp4F5->sc;_tmp4F7=_tmp4F5->name;_tmp4F8=_tmp4F5->tvs;_tmp4F9=_tmp4F5->fields;_tmp4FA=_tmp4F5->is_extensible;
if(_tmp4F9 == 0){
const char*_tmpA11;const char*_tmpA10;struct Cyc_PP_Doc*_tmpA0F[5];return(_tmpA0F[4]=
# 1763
Cyc_Absynpp_ktvars2doc(_tmp4F8),((_tmpA0F[3]=
# 1762
_tmp4FA?Cyc_Absynpp_qvar2bolddoc(_tmp4F7): Cyc_Absynpp_typedef_name2bolddoc(_tmp4F7),((_tmpA0F[2]=
# 1761
Cyc_PP_text(((_tmpA10="datatype ",_tag_dyneither(_tmpA10,sizeof(char),10)))),((_tmpA0F[1]=
# 1760
_tmp4FA?Cyc_PP_text(((_tmpA11="@extensible ",_tag_dyneither(_tmpA11,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmpA0F[0]=
# 1759
Cyc_Absynpp_scope2doc(_tmp4F6),Cyc_PP_cat(_tag_dyneither(_tmpA0F,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 1765
const char*_tmpA18;const char*_tmpA17;struct Cyc_PP_Doc*_tmpA16[2];struct Cyc_PP_Doc*_tmpA15[10];return(_tmpA15[9]=
# 1773
Cyc_Absynpp_rb(),((_tmpA15[8]=
# 1772
Cyc_PP_line_doc(),((_tmpA15[7]=
# 1771
Cyc_PP_nest(2,((_tmpA16[1]=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp4F9->v),((_tmpA16[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA16,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmpA15[6]=
# 1770
Cyc_Absynpp_lb(),((_tmpA15[5]=Cyc_PP_blank_doc(),((_tmpA15[4]=
# 1769
Cyc_Absynpp_ktvars2doc(_tmp4F8),((_tmpA15[3]=
# 1768
_tmp4FA?Cyc_Absynpp_qvar2bolddoc(_tmp4F7): Cyc_Absynpp_typedef_name2bolddoc(_tmp4F7),((_tmpA15[2]=
# 1767
Cyc_PP_text(((_tmpA17="datatype ",_tag_dyneither(_tmpA17,sizeof(char),10)))),((_tmpA15[1]=
# 1766
_tmp4FA?Cyc_PP_text(((_tmpA18="@extensible ",_tag_dyneither(_tmpA18,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmpA15[0]=
# 1765
Cyc_Absynpp_scope2doc(_tmp4F6),Cyc_PP_cat(_tag_dyneither(_tmpA15,sizeof(struct Cyc_PP_Doc*),10)))))))))))))))))))));}}
# 1776
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
enum Cyc_Absyn_Scope _tmp503;struct _tuple0*_tmp504;struct Cyc_Core_Opt*_tmp505;struct Cyc_Absyn_Enumdecl*_tmp502=ed;_tmp503=_tmp502->sc;_tmp504=_tmp502->name;_tmp505=_tmp502->fields;
if(_tmp505 == 0){
const char*_tmpA1B;struct Cyc_PP_Doc*_tmpA1A[3];return(_tmpA1A[2]=
# 1781
Cyc_Absynpp_typedef_name2bolddoc(_tmp504),((_tmpA1A[1]=
# 1780
Cyc_PP_text(((_tmpA1B="enum ",_tag_dyneither(_tmpA1B,sizeof(char),6)))),((_tmpA1A[0]=
# 1779
Cyc_Absynpp_scope2doc(_tmp503),Cyc_PP_cat(_tag_dyneither(_tmpA1A,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
# 1784
const char*_tmpA20;struct Cyc_PP_Doc*_tmpA1F[2];struct Cyc_PP_Doc*_tmpA1E[8];return(_tmpA1E[7]=
# 1790
Cyc_Absynpp_rb(),((_tmpA1E[6]=
# 1789
Cyc_PP_line_doc(),((_tmpA1E[5]=
# 1788
Cyc_PP_nest(2,((_tmpA1F[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp505->v),((_tmpA1F[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA1F,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmpA1E[4]=
# 1787
Cyc_Absynpp_lb(),((_tmpA1E[3]=Cyc_PP_blank_doc(),((_tmpA1E[2]=
# 1786
Cyc_Absynpp_qvar2bolddoc(_tmp504),((_tmpA1E[1]=
# 1785
Cyc_PP_text(((_tmpA20="enum ",_tag_dyneither(_tmpA20,sizeof(char),6)))),((_tmpA1E[0]=
# 1784
Cyc_Absynpp_scope2doc(_tmp503),Cyc_PP_cat(_tag_dyneither(_tmpA1E,sizeof(struct Cyc_PP_Doc*),8)))))))))))))))));}}
# 1793
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp50B=d->r;void*_tmp50C=_tmp50B;struct Cyc_Absyn_Fndecl*_tmp50E;struct Cyc_Absyn_Aggrdecl*_tmp510;struct Cyc_Absyn_Vardecl*_tmp512;struct Cyc_Absyn_Tvar*_tmp514;struct Cyc_Absyn_Vardecl*_tmp515;int _tmp516;struct Cyc_Absyn_Exp*_tmp517;struct Cyc_Absyn_Datatypedecl*_tmp519;struct Cyc_Absyn_Pat*_tmp51B;struct Cyc_Absyn_Exp*_tmp51C;struct Cyc_List_List*_tmp51E;struct Cyc_Absyn_Enumdecl*_tmp520;struct Cyc_Absyn_Typedefdecl*_tmp522;struct _dyneither_ptr*_tmp524;struct Cyc_List_List*_tmp525;struct _tuple0*_tmp527;struct Cyc_List_List*_tmp528;struct Cyc_List_List*_tmp52A;struct Cyc_List_List*_tmp52C;struct Cyc_List_List*_tmp52D;_LL29C: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp50D=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp50C;if(_tmp50D->tag != 1)goto _LL29E;else{_tmp50E=_tmp50D->f1;}}_LL29D: {
# 1797
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA26;struct Cyc_Absyn_FnInfo _tmpA25;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA24;void*t=(void*)((_tmpA24=_cycalloc(sizeof(*_tmpA24)),((_tmpA24[0]=((_tmpA26.tag=9,((_tmpA26.f1=((_tmpA25.tvars=_tmp50E->tvs,((_tmpA25.effect=_tmp50E->effect,((_tmpA25.ret_tqual=_tmp50E->ret_tqual,((_tmpA25.ret_typ=_tmp50E->ret_type,((_tmpA25.args=
# 1801
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp50E->args),((_tmpA25.c_varargs=_tmp50E->c_varargs,((_tmpA25.cyc_varargs=_tmp50E->cyc_varargs,((_tmpA25.rgn_po=_tmp50E->rgn_po,((_tmpA25.attributes=0,((_tmpA25.requires_clause=_tmp50E->requires_clause,((_tmpA25.requires_relns=_tmp50E->requires_relns,((_tmpA25.ensures_clause=_tmp50E->ensures_clause,((_tmpA25.ensures_relns=_tmp50E->ensures_relns,_tmpA25)))))))))))))))))))))))))),_tmpA26)))),_tmpA24))));
# 1807
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp50E->attributes);
struct Cyc_PP_Doc*inlinedoc;
if(_tmp50E->is_inline)
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2BA:
{const char*_tmpA27;inlinedoc=Cyc_PP_text(((_tmpA27="inline ",_tag_dyneither(_tmpA27,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL2BB:
{const char*_tmpA28;inlinedoc=Cyc_PP_text(((_tmpA28="__inline ",_tag_dyneither(_tmpA28,sizeof(char),10))));}break;}else{
# 1815
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp50E->sc);
struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2BD:
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL2BE:
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp50E->attributes);break;}{
# 1822
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp50E->name);
struct Cyc_PP_Doc*_tmpA2B[2];struct Cyc_Core_Opt*_tmpA2A;struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,(
(_tmpA2A=_cycalloc(sizeof(*_tmpA2A)),((_tmpA2A->v=((_tmpA2B[1]=namedoc,((_tmpA2B[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmpA2B,sizeof(struct Cyc_PP_Doc*),2)))))),_tmpA2A)))));
struct Cyc_PP_Doc*_tmpA2E[2];struct Cyc_PP_Doc*_tmpA2D[5];struct Cyc_PP_Doc*bodydoc=(_tmpA2D[4]=
# 1828
Cyc_Absynpp_rb(),((_tmpA2D[3]=
# 1827
Cyc_PP_line_doc(),((_tmpA2D[2]=
# 1826
Cyc_PP_nest(2,((_tmpA2E[1]=Cyc_Absynpp_stmt2doc(_tmp50E->body,0),((_tmpA2E[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA2E,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmpA2D[1]=
# 1825
Cyc_Absynpp_lb(),((_tmpA2D[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA2D,sizeof(struct Cyc_PP_Doc*),5)))))))))));
# 1829
{struct Cyc_PP_Doc*_tmpA2F[4];s=((_tmpA2F[3]=bodydoc,((_tmpA2F[2]=tqtddoc,((_tmpA2F[1]=scopedoc,((_tmpA2F[0]=inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmpA2F,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1831
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL2C0:
{struct Cyc_PP_Doc*_tmpA30[2];s=((_tmpA30[1]=s,((_tmpA30[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(_tmpA30,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL2C1:
 break;}
# 1836
goto _LL29B;};};}_LL29E: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp50F=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp50C;if(_tmp50F->tag != 5)goto _LL2A0;else{_tmp510=_tmp50F->f1;}}_LL29F:
# 1839
{const char*_tmpA33;struct Cyc_PP_Doc*_tmpA32[2];s=((_tmpA32[1]=Cyc_PP_text(((_tmpA33=";",_tag_dyneither(_tmpA33,sizeof(char),2)))),((_tmpA32[0]=Cyc_Absynpp_aggrdecl2doc(_tmp510),Cyc_PP_cat(_tag_dyneither(_tmpA32,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL29B;_LL2A0: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp511=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp50C;if(_tmp511->tag != 0)goto _LL2A2;else{_tmp512=_tmp511->f1;}}_LL2A1:
# 1842
 s=Cyc_Absynpp_vardecl2doc(_tmp512);
goto _LL29B;_LL2A2: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp513=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp50C;if(_tmp513->tag != 4)goto _LL2A4;else{_tmp514=_tmp513->f1;_tmp515=_tmp513->f2;_tmp516=_tmp513->f3;_tmp517=_tmp513->f4;}}_LL2A3:
# 1845
{const char*_tmpA46;const char*_tmpA45;const char*_tmpA44;struct Cyc_PP_Doc*_tmpA43[3];const char*_tmpA42;const char*_tmpA41;const char*_tmpA40;const char*_tmpA3F;struct Cyc_PP_Doc*_tmpA3E[8];s=((_tmpA3E[7]=
# 1853
Cyc_PP_text(((_tmpA3F=";",_tag_dyneither(_tmpA3F,sizeof(char),2)))),((_tmpA3E[6]=
# 1852
_tmp516?Cyc_PP_text(((_tmpA40=" @resetable",_tag_dyneither(_tmpA40,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmpA3E[5]=
# 1850
(unsigned int)_tmp517?(_tmpA43[2]=
Cyc_PP_text(((_tmpA42=")",_tag_dyneither(_tmpA42,sizeof(char),2)))),((_tmpA43[1]=
# 1850
Cyc_Absynpp_exp2doc(_tmp517),((_tmpA43[0]=Cyc_PP_text(((_tmpA41=" = open(",_tag_dyneither(_tmpA41,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmpA43,sizeof(struct Cyc_PP_Doc*),3))))))):
 Cyc_PP_nil_doc(),((_tmpA3E[4]=
# 1849
Cyc_Absynpp_qvar2doc(_tmp515->name),((_tmpA3E[3]=
# 1848
Cyc_PP_text(((_tmpA44=">",_tag_dyneither(_tmpA44,sizeof(char),2)))),((_tmpA3E[2]=
# 1847
Cyc_Absynpp_tvar2doc(_tmp514),((_tmpA3E[1]=
# 1846
Cyc_PP_text(((_tmpA45="<",_tag_dyneither(_tmpA45,sizeof(char),2)))),((_tmpA3E[0]=
# 1845
Cyc_PP_text(((_tmpA46="region",_tag_dyneither(_tmpA46,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA3E,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1854
goto _LL29B;_LL2A4: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp518=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp50C;if(_tmp518->tag != 6)goto _LL2A6;else{_tmp519=_tmp518->f1;}}_LL2A5:
# 1856
{const char*_tmpA49;struct Cyc_PP_Doc*_tmpA48[2];s=((_tmpA48[1]=Cyc_PP_text(((_tmpA49=";",_tag_dyneither(_tmpA49,sizeof(char),2)))),((_tmpA48[0]=Cyc_Absynpp_datatypedecl2doc(_tmp519),Cyc_PP_cat(_tag_dyneither(_tmpA48,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL29B;_LL2A6: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp51A=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp50C;if(_tmp51A->tag != 2)goto _LL2A8;else{_tmp51B=_tmp51A->f1;_tmp51C=_tmp51A->f3;}}_LL2A7:
# 1859
{const char*_tmpA50;const char*_tmpA4F;const char*_tmpA4E;struct Cyc_PP_Doc*_tmpA4D[5];s=((_tmpA4D[4]=
# 1863
Cyc_PP_text(((_tmpA4E=";",_tag_dyneither(_tmpA4E,sizeof(char),2)))),((_tmpA4D[3]=
# 1862
Cyc_Absynpp_exp2doc(_tmp51C),((_tmpA4D[2]=
# 1861
Cyc_PP_text(((_tmpA4F=" = ",_tag_dyneither(_tmpA4F,sizeof(char),4)))),((_tmpA4D[1]=
# 1860
Cyc_Absynpp_pat2doc(_tmp51B),((_tmpA4D[0]=
# 1859
Cyc_PP_text(((_tmpA50="let ",_tag_dyneither(_tmpA50,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA4D,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1864
goto _LL29B;_LL2A8: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp51D=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp50C;if(_tmp51D->tag != 3)goto _LL2AA;else{_tmp51E=_tmp51D->f1;}}_LL2A9:
# 1866
{const char*_tmpA55;const char*_tmpA54;struct Cyc_PP_Doc*_tmpA53[3];s=((_tmpA53[2]=Cyc_PP_text(((_tmpA54=";",_tag_dyneither(_tmpA54,sizeof(char),2)))),((_tmpA53[1]=Cyc_Absynpp_ids2doc(_tmp51E),((_tmpA53[0]=Cyc_PP_text(((_tmpA55="let ",_tag_dyneither(_tmpA55,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA53,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL29B;_LL2AA: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp51F=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp50C;if(_tmp51F->tag != 7)goto _LL2AC;else{_tmp520=_tmp51F->f1;}}_LL2AB:
# 1869
{const char*_tmpA58;struct Cyc_PP_Doc*_tmpA57[2];s=((_tmpA57[1]=Cyc_PP_text(((_tmpA58=";",_tag_dyneither(_tmpA58,sizeof(char),2)))),((_tmpA57[0]=Cyc_Absynpp_enumdecl2doc(_tmp520),Cyc_PP_cat(_tag_dyneither(_tmpA57,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL29B;_LL2AC: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp521=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp50C;if(_tmp521->tag != 8)goto _LL2AE;else{_tmp522=_tmp521->f1;}}_LL2AD: {
# 1872
void*t;
if(_tmp522->defn != 0)
t=(void*)_check_null(_tmp522->defn);else{
# 1876
t=Cyc_Absyn_new_evar(_tmp522->kind,0);}
{const char*_tmpA62;struct Cyc_Core_Opt*_tmpA61;struct Cyc_PP_Doc*_tmpA60[2];const char*_tmpA5F;struct Cyc_PP_Doc*_tmpA5E[4];s=((_tmpA5E[3]=
# 1884
Cyc_PP_text(((_tmpA5F=";",_tag_dyneither(_tmpA5F,sizeof(char),2)))),((_tmpA5E[2]=
# 1883
Cyc_Absynpp_atts2doc(_tmp522->atts),((_tmpA5E[1]=
# 1878
Cyc_Absynpp_tqtd2doc(_tmp522->tq,t,(
# 1880
(_tmpA61=_cycalloc(sizeof(*_tmpA61)),((_tmpA61->v=((_tmpA60[1]=
Cyc_Absynpp_tvars2doc(_tmp522->tvs),((_tmpA60[0]=
# 1880
Cyc_Absynpp_typedef_name2bolddoc(_tmp522->name),Cyc_PP_cat(_tag_dyneither(_tmpA60,sizeof(struct Cyc_PP_Doc*),2)))))),_tmpA61))))),((_tmpA5E[0]=
# 1877
Cyc_PP_text(((_tmpA62="typedef ",_tag_dyneither(_tmpA62,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmpA5E,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1886
goto _LL29B;}_LL2AE: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp523=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp50C;if(_tmp523->tag != 9)goto _LL2B0;else{_tmp524=_tmp523->f1;_tmp525=_tmp523->f2;}}_LL2AF:
# 1888
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp524);
{const char*_tmpA67;const char*_tmpA66;struct Cyc_PP_Doc*_tmpA65[8];s=((_tmpA65[7]=
# 1895
Cyc_Absynpp_rb(),((_tmpA65[6]=
# 1894
Cyc_PP_line_doc(),((_tmpA65[5]=
# 1893
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA66="",_tag_dyneither(_tmpA66,sizeof(char),1))),_tmp525),((_tmpA65[4]=
# 1892
Cyc_PP_line_doc(),((_tmpA65[3]=
# 1891
Cyc_Absynpp_lb(),((_tmpA65[2]=Cyc_PP_blank_doc(),((_tmpA65[1]=
# 1890
Cyc_PP_textptr(_tmp524),((_tmpA65[0]=
# 1889
Cyc_PP_text(((_tmpA67="namespace ",_tag_dyneither(_tmpA67,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmpA65,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1896
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL29B;_LL2B0: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp526=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp50C;if(_tmp526->tag != 10)goto _LL2B2;else{_tmp527=_tmp526->f1;_tmp528=_tmp526->f2;}}_LL2B1:
# 1899
 if(Cyc_Absynpp_print_using_stmts){
const char*_tmpA6C;const char*_tmpA6B;struct Cyc_PP_Doc*_tmpA6A[8];s=((_tmpA6A[7]=
# 1906
Cyc_Absynpp_rb(),((_tmpA6A[6]=
# 1905
Cyc_PP_line_doc(),((_tmpA6A[5]=
# 1904
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA6B="",_tag_dyneither(_tmpA6B,sizeof(char),1))),_tmp528),((_tmpA6A[4]=
# 1903
Cyc_PP_line_doc(),((_tmpA6A[3]=
# 1902
Cyc_Absynpp_lb(),((_tmpA6A[2]=Cyc_PP_blank_doc(),((_tmpA6A[1]=
# 1901
Cyc_Absynpp_qvar2doc(_tmp527),((_tmpA6A[0]=
# 1900
Cyc_PP_text(((_tmpA6C="using ",_tag_dyneither(_tmpA6C,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA6A,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1908
const char*_tmpA77;const char*_tmpA76;const char*_tmpA75;const char*_tmpA74;const char*_tmpA73;struct Cyc_PP_Doc*_tmpA72[11];s=((_tmpA72[10]=
# 1918
Cyc_PP_text(((_tmpA73=" */",_tag_dyneither(_tmpA73,sizeof(char),4)))),((_tmpA72[9]=
# 1917
Cyc_Absynpp_rb(),((_tmpA72[8]=
# 1916
Cyc_PP_text(((_tmpA74="/* ",_tag_dyneither(_tmpA74,sizeof(char),4)))),((_tmpA72[7]=
# 1915
Cyc_PP_line_doc(),((_tmpA72[6]=
# 1914
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA75="",_tag_dyneither(_tmpA75,sizeof(char),1))),_tmp528),((_tmpA72[5]=
# 1913
Cyc_PP_line_doc(),((_tmpA72[4]=
# 1912
Cyc_PP_text(((_tmpA76=" */",_tag_dyneither(_tmpA76,sizeof(char),4)))),((_tmpA72[3]=
# 1911
Cyc_Absynpp_lb(),((_tmpA72[2]=
# 1910
Cyc_PP_blank_doc(),((_tmpA72[1]=
# 1909
Cyc_Absynpp_qvar2doc(_tmp527),((_tmpA72[0]=
# 1908
Cyc_PP_text(((_tmpA77="/* using ",_tag_dyneither(_tmpA77,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmpA72,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1919
goto _LL29B;_LL2B2: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp529=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp50C;if(_tmp529->tag != 11)goto _LL2B4;else{_tmp52A=_tmp529->f1;}}_LL2B3:
# 1921
 if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA7C;const char*_tmpA7B;struct Cyc_PP_Doc*_tmpA7A[6];s=((_tmpA7A[5]=
# 1927
Cyc_Absynpp_rb(),((_tmpA7A[4]=
# 1926
Cyc_PP_line_doc(),((_tmpA7A[3]=
# 1925
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA7B="",_tag_dyneither(_tmpA7B,sizeof(char),1))),_tmp52A),((_tmpA7A[2]=
# 1924
Cyc_PP_line_doc(),((_tmpA7A[1]=
# 1923
Cyc_Absynpp_lb(),((_tmpA7A[0]=
# 1922
Cyc_PP_text(((_tmpA7C="extern \"C\" ",_tag_dyneither(_tmpA7C,sizeof(char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA7A,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}else{
# 1929
const char*_tmpA87;const char*_tmpA86;const char*_tmpA85;const char*_tmpA84;const char*_tmpA83;struct Cyc_PP_Doc*_tmpA82[9];s=((_tmpA82[8]=
# 1937
Cyc_PP_text(((_tmpA83=" */",_tag_dyneither(_tmpA83,sizeof(char),4)))),((_tmpA82[7]=
# 1936
Cyc_Absynpp_rb(),((_tmpA82[6]=
# 1935
Cyc_PP_text(((_tmpA84="/* ",_tag_dyneither(_tmpA84,sizeof(char),4)))),((_tmpA82[5]=
# 1934
Cyc_PP_line_doc(),((_tmpA82[4]=
# 1933
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA85="",_tag_dyneither(_tmpA85,sizeof(char),1))),_tmp52A),((_tmpA82[3]=
# 1932
Cyc_PP_line_doc(),((_tmpA82[2]=
# 1931
Cyc_PP_text(((_tmpA86=" */",_tag_dyneither(_tmpA86,sizeof(char),4)))),((_tmpA82[1]=
# 1930
Cyc_Absynpp_lb(),((_tmpA82[0]=
# 1929
Cyc_PP_text(((_tmpA87="/* extern \"C\" ",_tag_dyneither(_tmpA87,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA82,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1938
goto _LL29B;_LL2B4: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp52B=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp50C;if(_tmp52B->tag != 12)goto _LL2B6;else{_tmp52C=_tmp52B->f1;_tmp52D=_tmp52B->f2;}}_LL2B5:
# 1940
 if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
if(_tmp52D != 0){
const char*_tmpA8C;const char*_tmpA8B;struct Cyc_PP_Doc*_tmpA8A[7];exs_doc=((_tmpA8A[6]=
# 1945
Cyc_Absynpp_rb(),((_tmpA8A[5]=Cyc_PP_line_doc(),((_tmpA8A[4]=
# 1944
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,((_tmpA8B=",",_tag_dyneither(_tmpA8B,sizeof(char),2))),_tmp52D),((_tmpA8A[3]=Cyc_PP_line_doc(),((_tmpA8A[2]=
# 1943
Cyc_Absynpp_lb(),((_tmpA8A[1]=Cyc_PP_text(((_tmpA8C=" export ",_tag_dyneither(_tmpA8C,sizeof(char),9)))),((_tmpA8A[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA8A,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1947
exs_doc=Cyc_Absynpp_rb();}{
const char*_tmpA91;const char*_tmpA90;struct Cyc_PP_Doc*_tmpA8F[6];s=((_tmpA8F[5]=exs_doc,((_tmpA8F[4]=
# 1952
Cyc_PP_line_doc(),((_tmpA8F[3]=
# 1951
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA90="",_tag_dyneither(_tmpA90,sizeof(char),1))),_tmp52C),((_tmpA8F[2]=
# 1950
Cyc_PP_line_doc(),((_tmpA8F[1]=
# 1949
Cyc_Absynpp_lb(),((_tmpA8F[0]=
# 1948
Cyc_PP_text(((_tmpA91="extern \"C include\" ",_tag_dyneither(_tmpA91,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA8F,sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{
# 1955
const char*_tmpA9C;const char*_tmpA9B;const char*_tmpA9A;const char*_tmpA99;const char*_tmpA98;struct Cyc_PP_Doc*_tmpA97[9];s=((_tmpA97[8]=
# 1963
Cyc_PP_text(((_tmpA98=" */",_tag_dyneither(_tmpA98,sizeof(char),4)))),((_tmpA97[7]=
# 1962
Cyc_Absynpp_rb(),((_tmpA97[6]=
# 1961
Cyc_PP_text(((_tmpA99="/* ",_tag_dyneither(_tmpA99,sizeof(char),4)))),((_tmpA97[5]=
# 1960
Cyc_PP_line_doc(),((_tmpA97[4]=
# 1959
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA9A="",_tag_dyneither(_tmpA9A,sizeof(char),1))),_tmp52C),((_tmpA97[3]=
# 1958
Cyc_PP_line_doc(),((_tmpA97[2]=
# 1957
Cyc_PP_text(((_tmpA9B=" */",_tag_dyneither(_tmpA9B,sizeof(char),4)))),((_tmpA97[1]=
# 1956
Cyc_Absynpp_lb(),((_tmpA97[0]=
# 1955
Cyc_PP_text(((_tmpA9C="/* extern \"C include\" ",_tag_dyneither(_tmpA9C,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA97,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1964
goto _LL29B;_LL2B6: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp52E=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp50C;if(_tmp52E->tag != 13)goto _LL2B8;}_LL2B7:
# 1966
{const char*_tmpA9F;struct Cyc_PP_Doc*_tmpA9E[2];s=((_tmpA9E[1]=Cyc_Absynpp_lb(),((_tmpA9E[0]=Cyc_PP_text(((_tmpA9F="__cyclone_port_on__;",_tag_dyneither(_tmpA9F,sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA9E,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL29B;_LL2B8: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp52F=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp50C;if(_tmp52F->tag != 14)goto _LL29B;}_LL2B9:
# 1969
{const char*_tmpAA2;struct Cyc_PP_Doc*_tmpAA1[2];s=((_tmpAA1[1]=Cyc_Absynpp_lb(),((_tmpAA1[0]=Cyc_PP_text(((_tmpAA2="__cyclone_port_off__;",_tag_dyneither(_tmpAA2,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpAA1,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL29B;_LL29B:;}
# 1973
return s;}
# 1976
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
switch(sc){case Cyc_Absyn_Static: _LL2C3: {
const char*_tmpAA3;return Cyc_PP_text(((_tmpAA3="static ",_tag_dyneither(_tmpAA3,sizeof(char),8))));}case Cyc_Absyn_Public: _LL2C4:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL2C5: {
const char*_tmpAA4;return Cyc_PP_text(((_tmpAA4="extern ",_tag_dyneither(_tmpAA4,sizeof(char),8))));}case Cyc_Absyn_ExternC: _LL2C6: {
const char*_tmpAA5;return Cyc_PP_text(((_tmpAA5="extern \"C\" ",_tag_dyneither(_tmpAA5,sizeof(char),12))));}case Cyc_Absyn_Abstract: _LL2C7: {
const char*_tmpAA6;return Cyc_PP_text(((_tmpAA6="abstract ",_tag_dyneither(_tmpAA6,sizeof(char),10))));}case Cyc_Absyn_Register: _LL2C8: {
const char*_tmpAA7;return Cyc_PP_text(((_tmpAA7="register ",_tag_dyneither(_tmpAA7,sizeof(char),10))));}}}
# 1989
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp580=t;struct Cyc_Absyn_Tvar*_tmp582;struct Cyc_List_List*_tmp584;_LL2CB: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp581=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp580;if(_tmp581->tag != 2)goto _LL2CD;else{_tmp582=_tmp581->f1;}}_LL2CC:
 return Cyc_Tcutil_is_temp_tvar(_tmp582);_LL2CD: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp583=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp580;if(_tmp583->tag != 24)goto _LL2CF;else{_tmp584=_tmp583->f1;}}_LL2CE:
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp584);_LL2CF:;_LL2D0:
 return 0;_LL2CA:;}
# 2001
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp585=t;struct Cyc_Absyn_Typedefdecl*_tmp589;void*_tmp58A;_LL2D2: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp586=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp585;if(_tmp586->tag != 12)goto _LL2D4;}_LL2D3:
 return 1;_LL2D4: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp587=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp585;if(_tmp587->tag != 14)goto _LL2D6;}_LL2D5:
 return 1;_LL2D6:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp588=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp585;if(_tmp588->tag != 17)goto _LL2D8;else{_tmp589=_tmp588->f3;_tmp58A=(void*)_tmp588->f4;}}if(!(_tmp58A != 0))goto _LL2D8;_LL2D7:
# 2008
 return Cyc_Absynpp_is_anon_aggrtype(_tmp58A);_LL2D8:;_LL2D9:
 return 0;_LL2D1:;}
# 2015
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 2018
if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmpAA8;struct _tuple16 _tmp58B=(_tmpAA8.f1=(void*)tms->hd,((_tmpAA8.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpAA8)));struct _tuple16 _tmp58C=_tmp58B;_LL2DB:{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp58D=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp58C.f1;if(_tmp58D->tag != 2)goto _LL2DD;}{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp58E=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp58C.f2;if(_tmp58E->tag != 3)goto _LL2DD;};_LL2DC: {
# 2021
struct Cyc_List_List*_tmpAAB;struct Cyc_List_List*_tmpAAA;return(_tmpAAA=_region_malloc(r,sizeof(*_tmpAAA)),((_tmpAAA->hd=(void*)tms->hd,((_tmpAAA->tl=((_tmpAAB=_region_malloc(r,sizeof(*_tmpAAB)),((_tmpAAB->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,((_tmpAAB->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpAAB)))))),_tmpAAA)))));}_LL2DD:;_LL2DE: {
struct Cyc_List_List*_tmpAAC;return(_tmpAAC=_region_malloc(r,sizeof(*_tmpAAC)),((_tmpAAC->hd=atts,((_tmpAAC->tl=tms,_tmpAAC)))));}_LL2DA:;}else{
# 2024
struct Cyc_List_List*_tmpAAD;return(_tmpAAD=_region_malloc(r,sizeof(*_tmpAAD)),((_tmpAAD->hd=atts,((_tmpAAD->tl=tms,_tmpAAD)))));}}
# 2029
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2031
void*_tmp594=t;void*_tmp596;struct Cyc_Absyn_Tqual _tmp597;struct Cyc_Absyn_Exp*_tmp598;union Cyc_Absyn_Constraint*_tmp599;unsigned int _tmp59A;void*_tmp59C;struct Cyc_Absyn_Tqual _tmp59D;struct Cyc_Absyn_PtrAtts _tmp59E;struct Cyc_List_List*_tmp5A0;void*_tmp5A1;struct Cyc_Absyn_Tqual _tmp5A2;void*_tmp5A3;struct Cyc_List_List*_tmp5A4;int _tmp5A5;struct Cyc_Absyn_VarargInfo*_tmp5A6;struct Cyc_List_List*_tmp5A7;struct Cyc_List_List*_tmp5A8;struct Cyc_Absyn_Exp*_tmp5A9;struct Cyc_Absyn_Exp*_tmp5AA;struct Cyc_Core_Opt*_tmp5AC;void*_tmp5AD;int _tmp5AE;struct _tuple0*_tmp5B0;struct Cyc_List_List*_tmp5B1;struct Cyc_Absyn_Typedefdecl*_tmp5B2;void*_tmp5B3;_LL2E0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp595=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp594;if(_tmp595->tag != 8)goto _LL2E2;else{_tmp596=(_tmp595->f1).elt_type;_tmp597=(_tmp595->f1).tq;_tmp598=(_tmp595->f1).num_elts;_tmp599=(_tmp595->f1).zero_term;_tmp59A=(_tmp595->f1).zt_loc;}}_LL2E1: {
# 2034
struct _tuple12 _tmp5B4=Cyc_Absynpp_to_tms(r,_tmp597,_tmp596);struct Cyc_Absyn_Tqual _tmp5B6;void*_tmp5B7;struct Cyc_List_List*_tmp5B8;struct _tuple12 _tmp5B5=_tmp5B4;_tmp5B6=_tmp5B5.f1;_tmp5B7=_tmp5B5.f2;_tmp5B8=_tmp5B5.f3;{
void*tm;
if(_tmp598 == 0){
struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpAB0;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpAAF;tm=(void*)((_tmpAAF=_region_malloc(r,sizeof(*_tmpAAF)),((_tmpAAF[0]=((_tmpAB0.tag=0,((_tmpAB0.f1=_tmp599,((_tmpAB0.f2=_tmp59A,_tmpAB0)))))),_tmpAAF))));}else{
# 2039
struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpAB3;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpAB2;tm=(void*)((_tmpAB2=_region_malloc(r,sizeof(*_tmpAB2)),((_tmpAB2[0]=((_tmpAB3.tag=1,((_tmpAB3.f1=_tmp598,((_tmpAB3.f2=_tmp599,((_tmpAB3.f3=_tmp59A,_tmpAB3)))))))),_tmpAB2))));}{
struct Cyc_List_List*_tmpAB6;struct _tuple12 _tmpAB5;return(_tmpAB5.f1=_tmp5B6,((_tmpAB5.f2=_tmp5B7,((_tmpAB5.f3=((_tmpAB6=_region_malloc(r,sizeof(*_tmpAB6)),((_tmpAB6->hd=tm,((_tmpAB6->tl=_tmp5B8,_tmpAB6)))))),_tmpAB5)))));};};}_LL2E2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp59B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp594;if(_tmp59B->tag != 5)goto _LL2E4;else{_tmp59C=(_tmp59B->f1).elt_typ;_tmp59D=(_tmp59B->f1).elt_tq;_tmp59E=(_tmp59B->f1).ptr_atts;}}_LL2E3: {
# 2043
struct _tuple12 _tmp5BF=Cyc_Absynpp_to_tms(r,_tmp59D,_tmp59C);struct Cyc_Absyn_Tqual _tmp5C1;void*_tmp5C2;struct Cyc_List_List*_tmp5C3;struct _tuple12 _tmp5C0=_tmp5BF;_tmp5C1=_tmp5C0.f1;_tmp5C2=_tmp5C0.f2;_tmp5C3=_tmp5C0.f3;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpABC;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpABB;struct Cyc_List_List*_tmpABA;_tmp5C3=((_tmpABA=_region_malloc(r,sizeof(*_tmpABA)),((_tmpABA->hd=(void*)((_tmpABC=_region_malloc(r,sizeof(*_tmpABC)),((_tmpABC[0]=((_tmpABB.tag=2,((_tmpABB.f1=_tmp59E,((_tmpABB.f2=tq,_tmpABB)))))),_tmpABC)))),((_tmpABA->tl=_tmp5C3,_tmpABA))))));}{
struct _tuple12 _tmpABD;return(_tmpABD.f1=_tmp5C1,((_tmpABD.f2=_tmp5C2,((_tmpABD.f3=_tmp5C3,_tmpABD)))));};}_LL2E4: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp59F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp594;if(_tmp59F->tag != 9)goto _LL2E6;else{_tmp5A0=(_tmp59F->f1).tvars;_tmp5A1=(_tmp59F->f1).effect;_tmp5A2=(_tmp59F->f1).ret_tqual;_tmp5A3=(_tmp59F->f1).ret_typ;_tmp5A4=(_tmp59F->f1).args;_tmp5A5=(_tmp59F->f1).c_varargs;_tmp5A6=(_tmp59F->f1).cyc_varargs;_tmp5A7=(_tmp59F->f1).rgn_po;_tmp5A8=(_tmp59F->f1).attributes;_tmp5A9=(_tmp59F->f1).requires_clause;_tmp5AA=(_tmp59F->f1).ensures_clause;}}_LL2E5:
# 2049
 if(!Cyc_Absynpp_print_all_tvars){
# 2053
if(_tmp5A1 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect(_tmp5A1)){
_tmp5A1=0;
_tmp5A0=0;}}else{
# 2058
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2061
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp5A0);}{
# 2066
struct _tuple12 _tmp5C8=Cyc_Absynpp_to_tms(r,_tmp5A2,_tmp5A3);struct Cyc_Absyn_Tqual _tmp5CA;void*_tmp5CB;struct Cyc_List_List*_tmp5CC;struct _tuple12 _tmp5C9=_tmp5C8;_tmp5CA=_tmp5C9.f1;_tmp5CB=_tmp5C9.f2;_tmp5CC=_tmp5C9.f3;{
struct Cyc_List_List*tms=_tmp5CC;
# 2077 "absynpp.cyc"
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2EC:
# 2079
 if(_tmp5A8 != 0){
# 2082
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpAC0;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpABF;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpABF=_region_malloc(r,sizeof(*_tmpABF)),((_tmpABF[0]=((_tmpAC0.tag=5,((_tmpAC0.f1=0,((_tmpAC0.f2=_tmp5A8,_tmpAC0)))))),_tmpABF)))),tms);}
# 2084
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpACF;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpACE;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpACD;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpACC;struct Cyc_List_List*_tmpACB;tms=((_tmpACB=_region_malloc(r,sizeof(*_tmpACB)),((_tmpACB->hd=(void*)((_tmpACF=_region_malloc(r,sizeof(*_tmpACF)),((_tmpACF[0]=(
(_tmpACC.tag=3,((_tmpACC.f1=(void*)((_tmpACE=_region_malloc(r,sizeof(*_tmpACE)),((_tmpACE[0]=((_tmpACD.tag=1,((_tmpACD.f1=_tmp5A4,((_tmpACD.f2=_tmp5A5,((_tmpACD.f3=_tmp5A6,((_tmpACD.f4=_tmp5A1,((_tmpACD.f5=_tmp5A7,((_tmpACD.f6=_tmp5A9,((_tmpACD.f7=_tmp5AA,_tmpACD)))))))))))))))),_tmpACE)))),_tmpACC)))),_tmpACF)))),((_tmpACB->tl=tms,_tmpACB))))));}
# 2088
break;case Cyc_Cyclone_Vc_c: _LL2ED:
# 2090
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpADE;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpADD;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpADC;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpADB;struct Cyc_List_List*_tmpADA;tms=((_tmpADA=_region_malloc(r,sizeof(*_tmpADA)),((_tmpADA->hd=(void*)((_tmpADE=_region_malloc(r,sizeof(*_tmpADE)),((_tmpADE[0]=(
(_tmpADB.tag=3,((_tmpADB.f1=(void*)((_tmpADD=_region_malloc(r,sizeof(*_tmpADD)),((_tmpADD[0]=((_tmpADC.tag=1,((_tmpADC.f1=_tmp5A4,((_tmpADC.f2=_tmp5A5,((_tmpADC.f3=_tmp5A6,((_tmpADC.f4=_tmp5A1,((_tmpADC.f5=_tmp5A7,((_tmpADC.f6=_tmp5A9,((_tmpADC.f7=_tmp5AA,_tmpADC)))))))))))))))),_tmpADD)))),_tmpADB)))),_tmpADE)))),((_tmpADA->tl=tms,_tmpADA))))));}
# 2094
for(0;_tmp5A8 != 0;_tmp5A8=_tmp5A8->tl){
void*_tmp5D9=(void*)_tmp5A8->hd;void*_tmp5DA=_tmp5D9;_LL2F0: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp5DB=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp5DA;if(_tmp5DB->tag != 1)goto _LL2F2;}_LL2F1:
 goto _LL2F3;_LL2F2: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp5DC=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp5DA;if(_tmp5DC->tag != 2)goto _LL2F4;}_LL2F3:
 goto _LL2F5;_LL2F4: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp5DD=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp5DA;if(_tmp5DD->tag != 3)goto _LL2F6;}_LL2F5:
# 2099
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpAE8;struct Cyc_List_List*_tmpAE7;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpAE6;struct Cyc_List_List*_tmpAE5;tms=((_tmpAE5=_region_malloc(r,sizeof(*_tmpAE5)),((_tmpAE5->hd=(void*)((_tmpAE8=_region_malloc(r,sizeof(*_tmpAE8)),((_tmpAE8[0]=((_tmpAE6.tag=5,((_tmpAE6.f1=0,((_tmpAE6.f2=((_tmpAE7=_cycalloc(sizeof(*_tmpAE7)),((_tmpAE7->hd=(void*)_tmp5A8->hd,((_tmpAE7->tl=0,_tmpAE7)))))),_tmpAE6)))))),_tmpAE8)))),((_tmpAE5->tl=tms,_tmpAE5))))));}
goto AfterAtts;_LL2F6:;_LL2F7:
 goto _LL2EF;_LL2EF:;}
# 2103
break;}
# 2106
AfterAtts:
 if(_tmp5A0 != 0){
struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpAEE;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpAED;struct Cyc_List_List*_tmpAEC;tms=((_tmpAEC=_region_malloc(r,sizeof(*_tmpAEC)),((_tmpAEC->hd=(void*)((_tmpAEE=_region_malloc(r,sizeof(*_tmpAEE)),((_tmpAEE[0]=((_tmpAED.tag=4,((_tmpAED.f1=_tmp5A0,((_tmpAED.f2=0,((_tmpAED.f3=1,_tmpAED)))))))),_tmpAEE)))),((_tmpAEC->tl=tms,_tmpAEC))))));}{
struct _tuple12 _tmpAEF;return(_tmpAEF.f1=_tmp5CA,((_tmpAEF.f2=_tmp5CB,((_tmpAEF.f3=tms,_tmpAEF)))));};};};_LL2E6: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5AB=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp594;if(_tmp5AB->tag != 1)goto _LL2E8;else{_tmp5AC=_tmp5AB->f1;_tmp5AD=(void*)_tmp5AB->f2;_tmp5AE=_tmp5AB->f3;}}_LL2E7:
# 2112
 if(_tmp5AD == 0){
struct _tuple12 _tmpAF0;return(_tmpAF0.f1=tq,((_tmpAF0.f2=t,((_tmpAF0.f3=0,_tmpAF0)))));}else{
# 2115
return Cyc_Absynpp_to_tms(r,tq,_tmp5AD);}_LL2E8: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5AF=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp594;if(_tmp5AF->tag != 17)goto _LL2EA;else{_tmp5B0=_tmp5AF->f1;_tmp5B1=_tmp5AF->f2;_tmp5B2=_tmp5AF->f3;_tmp5B3=(void*)_tmp5AF->f4;}}_LL2E9:
# 2120
 if((_tmp5B3 == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(_tmp5B3)){
struct _tuple12 _tmpAF1;return(_tmpAF1.f1=tq,((_tmpAF1.f2=t,((_tmpAF1.f3=0,_tmpAF1)))));}else{
# 2123
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,_tmp5B3);}_LL2EA:;_LL2EB: {
# 2128
struct _tuple12 _tmpAF2;return(_tmpAF2.f1=tq,((_tmpAF2.f2=t,((_tmpAF2.f3=0,_tmpAF2)))));}_LL2DF:;}
# 2132
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2134
void*_tmp5E9=t;void*_tmp5EB;void*_tmp5ED;_LL2F9:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5EA=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5E9;if(_tmp5EA->tag != 1)goto _LL2FB;else{_tmp5EB=(void*)_tmp5EA->f2;}}if(!(_tmp5EB != 0))goto _LL2FB;_LL2FA:
 return Cyc_Absynpp_is_char_ptr(_tmp5EB);_LL2FB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5EC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E9;if(_tmp5EC->tag != 5)goto _LL2FD;else{_tmp5ED=(_tmp5EC->f1).elt_typ;}}_LL2FC:
# 2137
 L: {
void*_tmp5EE=_tmp5ED;void*_tmp5F0;void*_tmp5F2;_LL300:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5EF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5EE;if(_tmp5EF->tag != 1)goto _LL302;else{_tmp5F0=(void*)_tmp5EF->f2;}}if(!(_tmp5F0 != 0))goto _LL302;_LL301:
 _tmp5ED=_tmp5F0;goto L;_LL302:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5F1=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5EE;if(_tmp5F1->tag != 17)goto _LL304;else{_tmp5F2=(void*)_tmp5F1->f4;}}if(!(_tmp5F2 != 0))goto _LL304;_LL303:
 _tmp5ED=_tmp5F2;goto L;_LL304: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5F3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5EE;if(_tmp5F3->tag != 6)goto _LL306;else{if(_tmp5F3->f2 != Cyc_Absyn_Char_sz)goto _LL306;}}_LL305:
 return 1;_LL306:;_LL307:
 return 0;_LL2FF:;}_LL2FD:;_LL2FE:
# 2144
 return 0;_LL2F8:;}
# 2148
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp5F4=_new_region("temp");struct _RegionHandle*temp=& _tmp5F4;_push_region(temp);
{struct _tuple12 _tmp5F5=Cyc_Absynpp_to_tms(temp,tq,typ);struct Cyc_Absyn_Tqual _tmp5F7;void*_tmp5F8;struct Cyc_List_List*_tmp5F9;struct _tuple12 _tmp5F6=_tmp5F5;_tmp5F7=_tmp5F6.f1;_tmp5F8=_tmp5F6.f2;_tmp5F9=_tmp5F6.f3;
_tmp5F9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5F9);
if(_tmp5F9 == 0  && dopt == 0){
struct Cyc_PP_Doc*_tmpAF3[2];struct Cyc_PP_Doc*_tmp5FB=(_tmpAF3[1]=Cyc_Absynpp_ntyp2doc(_tmp5F8),((_tmpAF3[0]=Cyc_Absynpp_tqual2doc(_tmp5F7),Cyc_PP_cat(_tag_dyneither(_tmpAF3,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return _tmp5FB;}else{
# 2155
const char*_tmpAF6;struct Cyc_PP_Doc*_tmpAF5[4];struct Cyc_PP_Doc*_tmp5FE=
(_tmpAF5[3]=
# 2159
Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp5F9),((_tmpAF5[2]=
# 2158
Cyc_PP_text(((_tmpAF6=" ",_tag_dyneither(_tmpAF6,sizeof(char),2)))),((_tmpAF5[1]=
# 2157
Cyc_Absynpp_ntyp2doc(_tmp5F8),((_tmpAF5[0]=
# 2156
Cyc_Absynpp_tqual2doc(_tmp5F7),Cyc_PP_cat(_tag_dyneither(_tmpAF5,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp5FE;}}
# 2150
;_pop_region(temp);}
# 2163
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp5FF=f->requires_clause;
if((unsigned int)_tmp5FF){
const char*_tmpAF9;struct Cyc_PP_Doc*_tmpAF8[2];requires_doc=((_tmpAF8[1]=Cyc_Absynpp_exp2doc(_tmp5FF),((_tmpAF8[0]=Cyc_PP_text(((_tmpAF9="@requires ",_tag_dyneither(_tmpAF9,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmpAF8,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 2169
requires_doc=Cyc_PP_nil_doc();}
# 2171
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL308:
# 2174
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpB00;const char*_tmpAFF;const char*_tmpAFE;struct Cyc_PP_Doc*_tmpAFD[5];return(_tmpAFD[4]=
# 2177
Cyc_PP_text(((_tmpAFE=";",_tag_dyneither(_tmpAFE,sizeof(char),2)))),((_tmpAFD[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpAFD[2]=
# 2176
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAFD[1]=Cyc_PP_text(((_tmpAFF=":",_tag_dyneither(_tmpAFF,sizeof(char),2)))),((_tmpAFD[0]=
# 2175
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpB00=_cycalloc(sizeof(*_tmpB00)),((_tmpB00->v=Cyc_PP_textptr(f->name),_tmpB00))))),Cyc_PP_cat(_tag_dyneither(_tmpAFD,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2179
struct Cyc_Core_Opt*_tmpB05;const char*_tmpB04;struct Cyc_PP_Doc*_tmpB03[4];return(_tmpB03[3]=
Cyc_PP_text(((_tmpB04=";",_tag_dyneither(_tmpB04,sizeof(char),2)))),((_tmpB03[2]=requires_doc,((_tmpB03[1]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpB03[0]=
# 2179
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpB05=_cycalloc(sizeof(*_tmpB05)),((_tmpB05->v=Cyc_PP_textptr(f->name),_tmpB05))))),Cyc_PP_cat(_tag_dyneither(_tmpB03,sizeof(struct Cyc_PP_Doc*),4)))))))));}case Cyc_Cyclone_Vc_c: _LL309:
# 2182
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpB0C;const char*_tmpB0B;const char*_tmpB0A;struct Cyc_PP_Doc*_tmpB09[5];return(_tmpB09[4]=
# 2185
Cyc_PP_text(((_tmpB0A=";",_tag_dyneither(_tmpB0A,sizeof(char),2)))),((_tmpB09[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpB09[2]=Cyc_PP_text(((_tmpB0B=":",_tag_dyneither(_tmpB0B,sizeof(char),2)))),((_tmpB09[1]=
# 2184
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpB0C=_cycalloc(sizeof(*_tmpB0C)),((_tmpB0C->v=Cyc_PP_textptr(f->name),_tmpB0C))))),((_tmpB09[0]=
# 2183
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpB09,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2187
struct Cyc_Core_Opt*_tmpB11;const char*_tmpB10;struct Cyc_PP_Doc*_tmpB0F[4];return(_tmpB0F[3]=
# 2189
Cyc_PP_text(((_tmpB10=";",_tag_dyneither(_tmpB10,sizeof(char),2)))),((_tmpB0F[2]=requires_doc,((_tmpB0F[1]=
# 2188
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpB11=_cycalloc(sizeof(*_tmpB11)),((_tmpB11->v=Cyc_PP_textptr(f->name),_tmpB11))))),((_tmpB0F[0]=
# 2187
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpB0F,sizeof(struct Cyc_PP_Doc*),4)))))))));}}}
# 2194
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
const char*_tmpB12;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmpB12="",_tag_dyneither(_tmpB12,sizeof(char),1))),fields);}
# 2198
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
struct Cyc_PP_Doc*_tmpB13[3];return(_tmpB13[2]=
f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmpB13[1]=
# 2199
Cyc_Absynpp_typedef_name2doc(f->name),((_tmpB13[0]=Cyc_Absynpp_scope2doc(f->sc),Cyc_PP_cat(_tag_dyneither(_tmpB13,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 2203
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
const char*_tmpB14;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmpB14=",",_tag_dyneither(_tmpB14,sizeof(char),2))),fields);}
# 2212
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{
const char*_tmpB17;void*_tmpB16;(_tmpB16=0,Cyc_fprintf(f,((_tmpB17="\n",_tag_dyneither(_tmpB17,sizeof(char),2))),_tag_dyneither(_tmpB16,sizeof(void*),0)));};}}
# 2219
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
const char*_tmpB18;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpB18="",_tag_dyneither(_tmpB18,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),72);}
# 2222
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
# 2226
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
# 2237
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _dyneither_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}
