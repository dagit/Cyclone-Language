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
struct Cyc_List_List*_tmp540;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((_tmp540=_cycalloc(sizeof(*_tmp540)),((_tmp540->hd=v,((_tmp540->tl=0,_tmp540)))))));}
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
char _tmp2=c;switch(_tmp2){case 7: _LL1: _LL2: {
const char*_tmp541;return(_tmp541="\\a",_tag_dyneither(_tmp541,sizeof(char),3));}case 8: _LL3: _LL4: {
const char*_tmp542;return(_tmp542="\\b",_tag_dyneither(_tmp542,sizeof(char),3));}case 12: _LL5: _LL6: {
const char*_tmp543;return(_tmp543="\\f",_tag_dyneither(_tmp543,sizeof(char),3));}case 10: _LL7: _LL8: {
const char*_tmp544;return(_tmp544="\\n",_tag_dyneither(_tmp544,sizeof(char),3));}case 13: _LL9: _LLA: {
const char*_tmp545;return(_tmp545="\\r",_tag_dyneither(_tmp545,sizeof(char),3));}case 9: _LLB: _LLC: {
const char*_tmp546;return(_tmp546="\\t",_tag_dyneither(_tmp546,sizeof(char),3));}case 11: _LLD: _LLE: {
const char*_tmp547;return(_tmp547="\\v",_tag_dyneither(_tmp547,sizeof(char),3));}case 92: _LLF: _LL10: {
const char*_tmp548;return(_tmp548="\\\\",_tag_dyneither(_tmp548,sizeof(char),3));}case 34: _LL11: _LL12: {
const char*_tmp549;return(_tmp549="\"",_tag_dyneither(_tmp549,sizeof(char),2));}case 39: _LL13: _LL14: {
const char*_tmp54A;return(_tmp54A="\\'",_tag_dyneither(_tmp54A,sizeof(char),3));}default: _LL15: _LL16:
# 277
 if(c >= ' '  && c <= '~'){
struct _dyneither_ptr _tmpD=Cyc_Core_new_string(2);
{char _tmp54D;char _tmp54C;struct _dyneither_ptr _tmp54B;(_tmp54B=_dyneither_ptr_plus(_tmpD,sizeof(char),0),((_tmp54C=*((char*)_check_dyneither_subscript(_tmp54B,sizeof(char),0)),((_tmp54D=c,((_get_dyneither_size(_tmp54B,sizeof(char))== 1  && (_tmp54C == '\000'  && _tmp54D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp54B.curr)=_tmp54D)))))));}
return(struct _dyneither_ptr)_tmpD;}else{
# 282
struct _dyneither_ptr _tmp11=Cyc_Core_new_string(5);
int j=0;
{char _tmp550;char _tmp54F;struct _dyneither_ptr _tmp54E;(_tmp54E=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++),((_tmp54F=*((char*)_check_dyneither_subscript(_tmp54E,sizeof(char),0)),((_tmp550='\\',((_get_dyneither_size(_tmp54E,sizeof(char))== 1  && (_tmp54F == '\000'  && _tmp550 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp54E.curr)=_tmp550)))))));}
{char _tmp553;char _tmp552;struct _dyneither_ptr _tmp551;(_tmp551=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++),((_tmp552=*((char*)_check_dyneither_subscript(_tmp551,sizeof(char),0)),((_tmp553=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp551,sizeof(char))== 1  && (_tmp552 == '\000'  && _tmp553 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp551.curr)=_tmp553)))))));}
{char _tmp556;char _tmp555;struct _dyneither_ptr _tmp554;(_tmp554=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++),((_tmp555=*((char*)_check_dyneither_subscript(_tmp554,sizeof(char),0)),((_tmp556=(char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp554,sizeof(char))== 1  && (_tmp555 == '\000'  && _tmp556 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp554.curr)=_tmp556)))))));}
{char _tmp559;char _tmp558;struct _dyneither_ptr _tmp557;(_tmp557=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++),((_tmp558=*((char*)_check_dyneither_subscript(_tmp557,sizeof(char),0)),((_tmp559=(char)('0' + (c & 7)),((_get_dyneither_size(_tmp557,sizeof(char))== 1  && (_tmp558 == '\000'  && _tmp559 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp557.curr)=_tmp559)))))));}
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
{char _tmp55C;char _tmp55B;struct _dyneither_ptr _tmp55A;(_tmp55A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp55B=*((char*)_check_dyneither_subscript(_tmp55A,sizeof(char),0)),((_tmp55C='\\',((_get_dyneither_size(_tmp55A,sizeof(char))== 1  && (_tmp55B == '\000'  && _tmp55C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp55A.curr)=_tmp55C)))))));}{char _tmp55F;char _tmp55E;struct _dyneither_ptr _tmp55D;(_tmp55D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp55E=*((char*)_check_dyneither_subscript(_tmp55D,sizeof(char),0)),((_tmp55F='a',((_get_dyneither_size(_tmp55D,sizeof(char))== 1  && (_tmp55E == '\000'  && _tmp55F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp55D.curr)=_tmp55F)))))));}goto _LL2C;case 8: _LL2F: _LL30:
{char _tmp562;char _tmp561;struct _dyneither_ptr _tmp560;(_tmp560=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp561=*((char*)_check_dyneither_subscript(_tmp560,sizeof(char),0)),((_tmp562='\\',((_get_dyneither_size(_tmp560,sizeof(char))== 1  && (_tmp561 == '\000'  && _tmp562 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp560.curr)=_tmp562)))))));}{char _tmp565;char _tmp564;struct _dyneither_ptr _tmp563;(_tmp563=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp564=*((char*)_check_dyneither_subscript(_tmp563,sizeof(char),0)),((_tmp565='b',((_get_dyneither_size(_tmp563,sizeof(char))== 1  && (_tmp564 == '\000'  && _tmp565 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp563.curr)=_tmp565)))))));}goto _LL2C;case 12: _LL31: _LL32:
{char _tmp568;char _tmp567;struct _dyneither_ptr _tmp566;(_tmp566=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp567=*((char*)_check_dyneither_subscript(_tmp566,sizeof(char),0)),((_tmp568='\\',((_get_dyneither_size(_tmp566,sizeof(char))== 1  && (_tmp567 == '\000'  && _tmp568 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp566.curr)=_tmp568)))))));}{char _tmp56B;char _tmp56A;struct _dyneither_ptr _tmp569;(_tmp569=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp56A=*((char*)_check_dyneither_subscript(_tmp569,sizeof(char),0)),((_tmp56B='f',((_get_dyneither_size(_tmp569,sizeof(char))== 1  && (_tmp56A == '\000'  && _tmp56B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp569.curr)=_tmp56B)))))));}goto _LL2C;case 10: _LL33: _LL34:
{char _tmp56E;char _tmp56D;struct _dyneither_ptr _tmp56C;(_tmp56C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp56D=*((char*)_check_dyneither_subscript(_tmp56C,sizeof(char),0)),((_tmp56E='\\',((_get_dyneither_size(_tmp56C,sizeof(char))== 1  && (_tmp56D == '\000'  && _tmp56E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp56C.curr)=_tmp56E)))))));}{char _tmp571;char _tmp570;struct _dyneither_ptr _tmp56F;(_tmp56F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp570=*((char*)_check_dyneither_subscript(_tmp56F,sizeof(char),0)),((_tmp571='n',((_get_dyneither_size(_tmp56F,sizeof(char))== 1  && (_tmp570 == '\000'  && _tmp571 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp56F.curr)=_tmp571)))))));}goto _LL2C;case 13: _LL35: _LL36:
{char _tmp574;char _tmp573;struct _dyneither_ptr _tmp572;(_tmp572=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp573=*((char*)_check_dyneither_subscript(_tmp572,sizeof(char),0)),((_tmp574='\\',((_get_dyneither_size(_tmp572,sizeof(char))== 1  && (_tmp573 == '\000'  && _tmp574 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp572.curr)=_tmp574)))))));}{char _tmp577;char _tmp576;struct _dyneither_ptr _tmp575;(_tmp575=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp576=*((char*)_check_dyneither_subscript(_tmp575,sizeof(char),0)),((_tmp577='r',((_get_dyneither_size(_tmp575,sizeof(char))== 1  && (_tmp576 == '\000'  && _tmp577 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp575.curr)=_tmp577)))))));}goto _LL2C;case 9: _LL37: _LL38:
{char _tmp57A;char _tmp579;struct _dyneither_ptr _tmp578;(_tmp578=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp579=*((char*)_check_dyneither_subscript(_tmp578,sizeof(char),0)),((_tmp57A='\\',((_get_dyneither_size(_tmp578,sizeof(char))== 1  && (_tmp579 == '\000'  && _tmp57A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp578.curr)=_tmp57A)))))));}{char _tmp57D;char _tmp57C;struct _dyneither_ptr _tmp57B;(_tmp57B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp57C=*((char*)_check_dyneither_subscript(_tmp57B,sizeof(char),0)),((_tmp57D='t',((_get_dyneither_size(_tmp57B,sizeof(char))== 1  && (_tmp57C == '\000'  && _tmp57D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp57B.curr)=_tmp57D)))))));}goto _LL2C;case 11: _LL39: _LL3A:
{char _tmp580;char _tmp57F;struct _dyneither_ptr _tmp57E;(_tmp57E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp57F=*((char*)_check_dyneither_subscript(_tmp57E,sizeof(char),0)),((_tmp580='\\',((_get_dyneither_size(_tmp57E,sizeof(char))== 1  && (_tmp57F == '\000'  && _tmp580 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp57E.curr)=_tmp580)))))));}{char _tmp583;char _tmp582;struct _dyneither_ptr _tmp581;(_tmp581=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp582=*((char*)_check_dyneither_subscript(_tmp581,sizeof(char),0)),((_tmp583='v',((_get_dyneither_size(_tmp581,sizeof(char))== 1  && (_tmp582 == '\000'  && _tmp583 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp581.curr)=_tmp583)))))));}goto _LL2C;case 92: _LL3B: _LL3C:
{char _tmp586;char _tmp585;struct _dyneither_ptr _tmp584;(_tmp584=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp585=*((char*)_check_dyneither_subscript(_tmp584,sizeof(char),0)),((_tmp586='\\',((_get_dyneither_size(_tmp584,sizeof(char))== 1  && (_tmp585 == '\000'  && _tmp586 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp584.curr)=_tmp586)))))));}{char _tmp589;char _tmp588;struct _dyneither_ptr _tmp587;(_tmp587=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp588=*((char*)_check_dyneither_subscript(_tmp587,sizeof(char),0)),((_tmp589='\\',((_get_dyneither_size(_tmp587,sizeof(char))== 1  && (_tmp588 == '\000'  && _tmp589 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp587.curr)=_tmp589)))))));}goto _LL2C;case 34: _LL3D: _LL3E:
{char _tmp58C;char _tmp58B;struct _dyneither_ptr _tmp58A;(_tmp58A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp58B=*((char*)_check_dyneither_subscript(_tmp58A,sizeof(char),0)),((_tmp58C='\\',((_get_dyneither_size(_tmp58A,sizeof(char))== 1  && (_tmp58B == '\000'  && _tmp58C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp58A.curr)=_tmp58C)))))));}{char _tmp58F;char _tmp58E;struct _dyneither_ptr _tmp58D;(_tmp58D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp58E=*((char*)_check_dyneither_subscript(_tmp58D,sizeof(char),0)),((_tmp58F='"',((_get_dyneither_size(_tmp58D,sizeof(char))== 1  && (_tmp58E == '\000'  && _tmp58F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp58D.curr)=_tmp58F)))))));}goto _LL2C;default: _LL3F: _tmp23=_tmp22;_LL40:
# 342
 if(_tmp23 >= ' '  && _tmp23 <= '~'){char _tmp592;char _tmp591;struct _dyneither_ptr _tmp590;(_tmp590=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp591=*((char*)_check_dyneither_subscript(_tmp590,sizeof(char),0)),((_tmp592=_tmp23,((_get_dyneither_size(_tmp590,sizeof(char))== 1  && (_tmp591 == '\000'  && _tmp592 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp590.curr)=_tmp592)))))));}else{
# 344
{char _tmp595;char _tmp594;struct _dyneither_ptr _tmp593;(_tmp593=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp594=*((char*)_check_dyneither_subscript(_tmp593,sizeof(char),0)),((_tmp595='\\',((_get_dyneither_size(_tmp593,sizeof(char))== 1  && (_tmp594 == '\000'  && _tmp595 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp593.curr)=_tmp595)))))));}
{char _tmp598;char _tmp597;struct _dyneither_ptr _tmp596;(_tmp596=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp597=*((char*)_check_dyneither_subscript(_tmp596,sizeof(char),0)),((_tmp598=(char)('0' + (_tmp23 >> 6 & 7)),((_get_dyneither_size(_tmp596,sizeof(char))== 1  && (_tmp597 == '\000'  && _tmp598 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp596.curr)=_tmp598)))))));}
{char _tmp59B;char _tmp59A;struct _dyneither_ptr _tmp599;(_tmp599=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp59A=*((char*)_check_dyneither_subscript(_tmp599,sizeof(char),0)),((_tmp59B=(char)('0' + (_tmp23 >> 3 & 7)),((_get_dyneither_size(_tmp599,sizeof(char))== 1  && (_tmp59A == '\000'  && _tmp59B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp599.curr)=_tmp59B)))))));}{
char _tmp59E;char _tmp59D;struct _dyneither_ptr _tmp59C;(_tmp59C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp59D=*((char*)_check_dyneither_subscript(_tmp59C,sizeof(char),0)),((_tmp59E=(char)('0' + (_tmp23 & 7)),((_get_dyneither_size(_tmp59C,sizeof(char))== 1  && (_tmp59D == '\000'  && _tmp59E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp59C.curr)=_tmp59E)))))));};}
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
if(tq.q_restrict){struct Cyc_List_List*_tmp59F;l=((_tmp59F=_cycalloc(sizeof(*_tmp59F)),((_tmp59F->hd=Cyc_Absynpp_restrict_sp,((_tmp59F->tl=l,_tmp59F))))));}
if(tq.q_volatile){struct Cyc_List_List*_tmp5A0;l=((_tmp5A0=_cycalloc(sizeof(*_tmp5A0)),((_tmp5A0->hd=Cyc_Absynpp_volatile_sp,((_tmp5A0->tl=l,_tmp5A0))))));}
if(tq.print_const){struct Cyc_List_List*_tmp5A1;l=((_tmp5A1=_cycalloc(sizeof(*_tmp5A1)),((_tmp5A1->hd=Cyc_Absynpp_const_sp,((_tmp5A1->tl=l,_tmp5A1))))));}{
const char*_tmp5A4;const char*_tmp5A3;const char*_tmp5A2;return Cyc_PP_egroup(((_tmp5A2="",_tag_dyneither(_tmp5A2,sizeof(char),1))),((_tmp5A3=" ",_tag_dyneither(_tmp5A3,sizeof(char),2))),((_tmp5A4=" ",_tag_dyneither(_tmp5A4,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));};}
# 370
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp72=ka;enum Cyc_Absyn_KindQual _tmp73;enum Cyc_Absyn_AliasQual _tmp74;_LL42: _tmp73=_tmp72->kind;_tmp74=_tmp72->aliasqual;_LL43: {
enum Cyc_Absyn_KindQual _tmp75=_tmp73;switch(_tmp75){case Cyc_Absyn_AnyKind: _LL47: _LL48: {
# 374
enum Cyc_Absyn_AliasQual _tmp76=_tmp74;switch(_tmp76){case Cyc_Absyn_Aliasable: _LL54: _LL55: {
const char*_tmp5A5;return(_tmp5A5="A",_tag_dyneither(_tmp5A5,sizeof(char),2));}case Cyc_Absyn_Unique: _LL56: _LL57: {
const char*_tmp5A6;return(_tmp5A6="UA",_tag_dyneither(_tmp5A6,sizeof(char),3));}default: _LL58: _LL59: {
const char*_tmp5A7;return(_tmp5A7="TA",_tag_dyneither(_tmp5A7,sizeof(char),3));}}_LL53:;}case Cyc_Absyn_MemKind: _LL49: _LL4A: {
# 380
enum Cyc_Absyn_AliasQual _tmp7A=_tmp74;switch(_tmp7A){case Cyc_Absyn_Aliasable: _LL5B: _LL5C: {
const char*_tmp5A8;return(_tmp5A8="M",_tag_dyneither(_tmp5A8,sizeof(char),2));}case Cyc_Absyn_Unique: _LL5D: _LL5E: {
const char*_tmp5A9;return(_tmp5A9="UM",_tag_dyneither(_tmp5A9,sizeof(char),3));}default: _LL5F: _LL60: {
const char*_tmp5AA;return(_tmp5AA="TM",_tag_dyneither(_tmp5AA,sizeof(char),3));}}_LL5A:;}case Cyc_Absyn_BoxKind: _LL4B: _LL4C: {
# 386
enum Cyc_Absyn_AliasQual _tmp7E=_tmp74;switch(_tmp7E){case Cyc_Absyn_Aliasable: _LL62: _LL63: {
const char*_tmp5AB;return(_tmp5AB="B",_tag_dyneither(_tmp5AB,sizeof(char),2));}case Cyc_Absyn_Unique: _LL64: _LL65: {
const char*_tmp5AC;return(_tmp5AC="UB",_tag_dyneither(_tmp5AC,sizeof(char),3));}default: _LL66: _LL67: {
const char*_tmp5AD;return(_tmp5AD="TB",_tag_dyneither(_tmp5AD,sizeof(char),3));}}_LL61:;}case Cyc_Absyn_RgnKind: _LL4D: _LL4E: {
# 392
enum Cyc_Absyn_AliasQual _tmp82=_tmp74;switch(_tmp82){case Cyc_Absyn_Aliasable: _LL69: _LL6A: {
const char*_tmp5AE;return(_tmp5AE="R",_tag_dyneither(_tmp5AE,sizeof(char),2));}case Cyc_Absyn_Unique: _LL6B: _LL6C: {
const char*_tmp5AF;return(_tmp5AF="UR",_tag_dyneither(_tmp5AF,sizeof(char),3));}default: _LL6D: _LL6E: {
const char*_tmp5B0;return(_tmp5B0="TR",_tag_dyneither(_tmp5B0,sizeof(char),3));}}_LL68:;}case Cyc_Absyn_EffKind: _LL4F: _LL50: {
# 397
const char*_tmp5B1;return(_tmp5B1="E",_tag_dyneither(_tmp5B1,sizeof(char),2));}default: _LL51: _LL52: {
const char*_tmp5B2;return(_tmp5B2="I",_tag_dyneither(_tmp5B2,sizeof(char),2));}}_LL46:;}_LL41:;}
# 401
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 403
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp88=Cyc_Absyn_compress_kb(c);void*_tmp89=_tmp88;struct Cyc_Absyn_Kind*_tmp8A;struct Cyc_Absyn_Kind*_tmp8B;switch(*((int*)_tmp89)){case 0: _LL70: _tmp8B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp89)->f1;_LL71:
 return Cyc_Absynpp_kind2string(_tmp8B);case 1: _LL72: _LL73:
# 407
 if(Cyc_PP_tex_output){
const char*_tmp5B3;return(_tmp5B3="{?}",_tag_dyneither(_tmp5B3,sizeof(char),4));}else{
const char*_tmp5B4;return(_tmp5B4="?",_tag_dyneither(_tmp5B4,sizeof(char),2));}default: _LL74: _tmp8A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89)->f2;_LL75: {
const char*_tmp5B8;void*_tmp5B7[1];struct Cyc_String_pa_PrintArg_struct _tmp5B6;return(struct _dyneither_ptr)((_tmp5B6.tag=0,((_tmp5B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp8A)),((_tmp5B7[0]=& _tmp5B6,Cyc_aprintf(((_tmp5B8="<=%s",_tag_dyneither(_tmp5B8,sizeof(char),5))),_tag_dyneither(_tmp5B7,sizeof(void*),1))))))));}}_LL6F:;}
# 414
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp91=Cyc_Absyn_compress_kb(c);void*_tmp92=_tmp91;struct Cyc_Absyn_Kind*_tmp93;struct Cyc_Absyn_Kind*_tmp94;switch(*((int*)_tmp92)){case 0: _LL77: _tmp94=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp92)->f1;_LL78:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp94));case 1: _LL79: _LL7A:
# 418
 if(Cyc_PP_tex_output){
const char*_tmp5B9;return Cyc_PP_text_width(((_tmp5B9="{?}",_tag_dyneither(_tmp5B9,sizeof(char),4))),1);}else{
const char*_tmp5BA;return Cyc_PP_text(((_tmp5BA="?",_tag_dyneither(_tmp5BA,sizeof(char),2))));}default: _LL7B: _tmp93=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp92)->f2;_LL7C:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp93));}_LL76:;}
# 425
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
const char*_tmp5BD;const char*_tmp5BC;const char*_tmp5BB;return Cyc_PP_egroup(((_tmp5BB="<",_tag_dyneither(_tmp5BB,sizeof(char),2))),((_tmp5BC=">",_tag_dyneither(_tmp5BC,sizeof(char),2))),((_tmp5BD=",",_tag_dyneither(_tmp5BD,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));}
# 429
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr _tmp9A=*tv->name;
# 433
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(tv)){
# 435
const char*_tmp5BE;struct _dyneither_ptr kstring=(_tmp5BE="K",_tag_dyneither(_tmp5BE,sizeof(char),2));
{void*_tmp9B=Cyc_Absyn_compress_kb(tv->kind);void*_tmp9C=_tmp9B;struct Cyc_Absyn_Kind*_tmp9D;struct Cyc_Absyn_Kind*_tmp9E;switch(*((int*)_tmp9C)){case 2: _LL7E: _tmp9E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9C)->f2;_LL7F:
 _tmp9D=_tmp9E;goto _LL81;case 0: _LL80: _tmp9D=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9C)->f1;_LL81:
 kstring=Cyc_Absynpp_kind2string(_tmp9D);goto _LL7D;default: _LL82: _LL83:
 goto _LL7D;}_LL7D:;}{
# 441
const char*_tmp5C3;void*_tmp5C2[2];struct Cyc_String_pa_PrintArg_struct _tmp5C1;struct Cyc_String_pa_PrintArg_struct _tmp5C0;return Cyc_PP_text((struct _dyneither_ptr)((_tmp5C0.tag=0,((_tmp5C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_tmp9A,sizeof(char),1)),((_tmp5C1.tag=0,((_tmp5C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kstring),((_tmp5C2[0]=& _tmp5C1,((_tmp5C2[1]=& _tmp5C0,Cyc_aprintf(((_tmp5C3="`G%s%s",_tag_dyneither(_tmp5C3,sizeof(char),7))),_tag_dyneither(_tmp5C2,sizeof(void*),2)))))))))))))));};}
# 443
return Cyc_PP_text(_tmp9A);}
# 446
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmpA4=Cyc_Absyn_compress_kb(tv->kind);void*_tmpA5=_tmpA4;struct Cyc_Absyn_Kind*_tmpA6;struct Cyc_Absyn_Kind*_tmpA7;switch(*((int*)_tmpA5)){case 1: _LL85: _LL86:
 goto _LL88;case 0: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL87: _LL88:
 return Cyc_Absynpp_tvar2doc(tv);}else{goto _LL8B;}}else{_LL8B: _tmpA7=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5)->f1;_LL8C: {
# 451
const char*_tmp5C6;struct Cyc_PP_Doc*_tmp5C5[3];return(_tmp5C5[2]=Cyc_Absynpp_kind2doc(_tmpA7),((_tmp5C5[1]=Cyc_PP_text(((_tmp5C6="::",_tag_dyneither(_tmp5C6,sizeof(char),3)))),((_tmp5C5[0]=Cyc_Absynpp_tvar2doc(tv),Cyc_PP_cat(_tag_dyneither(_tmp5C5,sizeof(struct Cyc_PP_Doc*),3)))))));}}default: _LL89: _tmpA6=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA5)->f2;_LL8A:
# 450
 _tmpA7=_tmpA6;goto _LL8C;}_LL84:;}
# 455
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
const char*_tmp5C9;const char*_tmp5C8;const char*_tmp5C7;return Cyc_PP_egroup(((_tmp5C7="<",_tag_dyneither(_tmp5C7,sizeof(char),2))),((_tmp5C8=">",_tag_dyneither(_tmp5C8,sizeof(char),2))),((_tmp5C9=",",_tag_dyneither(_tmp5C9,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}
# 459
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);{
const char*_tmp5CC;const char*_tmp5CB;const char*_tmp5CA;return Cyc_PP_egroup(((_tmp5CA="<",_tag_dyneither(_tmp5CA,sizeof(char),2))),((_tmp5CB=">",_tag_dyneither(_tmp5CB,sizeof(char),2))),((_tmp5CC=",",_tag_dyneither(_tmp5CC,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));};}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 465
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 469
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
const char*_tmp5CF;const char*_tmp5CE;const char*_tmp5CD;return Cyc_PP_group(((_tmp5CD="(",_tag_dyneither(_tmp5CD,sizeof(char),2))),((_tmp5CE=")",_tag_dyneither(_tmp5CE,sizeof(char),2))),((_tmp5CF=",",_tag_dyneither(_tmp5CF,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
# 473
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
void*_tmpB3=att;switch(*((int*)_tmpB3)){case 1: _LL8E: _LL8F:
 goto _LL91;case 2: _LL90: _LL91:
 goto _LL93;case 3: _LL92: _LL93:
 return Cyc_PP_nil_doc();default: _LL94: _LL95:
 return Cyc_PP_text(Cyc_Absyn_attribute2string(att));}_LL8D:;}
# 482
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmpB4=(void*)atts->hd;void*_tmpB5=_tmpB4;switch(*((int*)_tmpB5)){case 1: _LL97: _LL98: {
const char*_tmp5D0;return Cyc_PP_text(((_tmp5D0=" _stdcall ",_tag_dyneither(_tmp5D0,sizeof(char),11))));}case 2: _LL99: _LL9A: {
const char*_tmp5D1;return Cyc_PP_text(((_tmp5D1=" _cdecl ",_tag_dyneither(_tmp5D1,sizeof(char),9))));}case 3: _LL9B: _LL9C: {
const char*_tmp5D2;return Cyc_PP_text(((_tmp5D2=" _fastcall ",_tag_dyneither(_tmp5D2,sizeof(char),12))));}default: _LL9D: _LL9E:
 goto _LL96;}_LL96:;}
# 490
return Cyc_PP_nil_doc();}
# 493
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpB9=(void*)atts2->hd;void*_tmpBA=_tmpB9;switch(*((int*)_tmpBA)){case 1: _LLA0: _LLA1:
 goto _LLA3;case 2: _LLA2: _LLA3:
 goto _LLA5;case 3: _LLA4: _LLA5:
 goto _LL9F;default: _LLA6: _LLA7:
 hasatt=1;goto _LL9F;}_LL9F:;}}
# 502
if(!hasatt)
return Cyc_PP_nil_doc();{
const char*_tmp5DD;const char*_tmp5DC;const char*_tmp5DB;const char*_tmp5DA;const char*_tmp5D9;struct Cyc_PP_Doc*_tmp5D8[3];return(_tmp5D8[2]=
# 506
Cyc_PP_text(((_tmp5D9=")",_tag_dyneither(_tmp5D9,sizeof(char),2)))),((_tmp5D8[1]=
# 505
Cyc_PP_group(((_tmp5DC="",_tag_dyneither(_tmp5DC,sizeof(char),1))),((_tmp5DB="",_tag_dyneither(_tmp5DB,sizeof(char),1))),((_tmp5DA=" ",_tag_dyneither(_tmp5DA,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts)),((_tmp5D8[0]=
# 504
Cyc_PP_text(((_tmp5DD=" __declspec(",_tag_dyneither(_tmp5DD,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp5D8,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 509
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 513
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();{
enum Cyc_Cyclone_C_Compilers _tmpC1=Cyc_Cyclone_c_compiler;if(_tmpC1 == Cyc_Cyclone_Vc_c){_LLA9: _LLAA:
 return Cyc_Absynpp_noncallconv2doc(atts);}else{_LLAB: _LLAC: {
const char*_tmp5E6;const char*_tmp5E5;const char*_tmp5E4;const char*_tmp5E3;struct Cyc_PP_Doc*_tmp5E2[2];return(_tmp5E2[1]=
Cyc_PP_group(((_tmp5E5="((",_tag_dyneither(_tmp5E5,sizeof(char),3))),((_tmp5E4="))",_tag_dyneither(_tmp5E4,sizeof(char),3))),((_tmp5E3=",",_tag_dyneither(_tmp5E3,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp5E2[0]=
# 517
Cyc_PP_text(((_tmp5E6=" __attribute__",_tag_dyneither(_tmp5E6,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp5E2,sizeof(struct Cyc_PP_Doc*),2)))));}}_LLA8:;};}
# 522
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmpC7=(void*)tms->hd;void*_tmpC8=_tmpC7;switch(*((int*)_tmpC8)){case 2: _LLAE: _LLAF:
 return 1;case 5: _LLB0: _LLB1: {
# 527
enum Cyc_Cyclone_C_Compilers _tmpC9=Cyc_Cyclone_c_compiler;if(_tmpC9 == Cyc_Cyclone_Gcc_c){_LLB5: _LLB6:
 return 0;}else{_LLB7: _LLB8:
 return Cyc_Absynpp_next_is_pointer(tms->tl);}_LLB4:;}default: _LLB2: _LLB3:
# 531
 return 0;}_LLAD:;};}
# 535
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
# 538
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_question(){
if(!((unsigned int)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output){
const char*_tmp5E7;Cyc_Absynpp_cache_question=Cyc_PP_text_width(((_tmp5E7="{?}",_tag_dyneither(_tmp5E7,sizeof(char),4))),1);}else{
const char*_tmp5E8;Cyc_Absynpp_cache_question=Cyc_PP_text(((_tmp5E8="?",_tag_dyneither(_tmp5E8,sizeof(char),2))));}}
# 545
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 547
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output){
const char*_tmp5E9;Cyc_Absynpp_cache_lb=Cyc_PP_text_width(((_tmp5E9="{\\lb}",_tag_dyneither(_tmp5E9,sizeof(char),6))),1);}else{
const char*_tmp5EA;Cyc_Absynpp_cache_lb=Cyc_PP_text(((_tmp5EA="{",_tag_dyneither(_tmp5EA,sizeof(char),2))));}}
# 554
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 556
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output){
const char*_tmp5EB;Cyc_Absynpp_cache_rb=Cyc_PP_text_width(((_tmp5EB="{\\rb}",_tag_dyneither(_tmp5EB,sizeof(char),6))),1);}else{
const char*_tmp5EC;Cyc_Absynpp_cache_rb=Cyc_PP_text(((_tmp5EC="}",_tag_dyneither(_tmp5EC,sizeof(char),2))));}}
# 563
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 565
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output){
const char*_tmp5ED;Cyc_Absynpp_cache_dollar=Cyc_PP_text_width(((_tmp5ED="\\$",_tag_dyneither(_tmp5ED,sizeof(char),3))),1);}else{
const char*_tmp5EE;Cyc_Absynpp_cache_dollar=Cyc_PP_text(((_tmp5EE="$",_tag_dyneither(_tmp5EE,sizeof(char),2))));}}
# 572
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 574
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){
struct Cyc_PP_Doc*_tmp5EF[3];return(_tmp5EF[2]=Cyc_Absynpp_rb(),((_tmp5EF[1]=Cyc_PP_seq(sep,ss),((_tmp5EF[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp5EF,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 579
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpD3=(void*)tms->hd;void*_tmpD4=_tmpD3;struct Cyc_List_List*_tmpD5;switch(*((int*)_tmpD4)){case 0: _LLBA: _LLBB:
{const char*_tmp5F2;void*_tmp5F1;(_tmp5F1=0,Cyc_fprintf(Cyc_stderr,((_tmp5F2="Carray_mod ",_tag_dyneither(_tmp5F2,sizeof(char),12))),_tag_dyneither(_tmp5F1,sizeof(void*),0)));}goto _LLB9;case 1: _LLBC: _LLBD:
{const char*_tmp5F5;void*_tmp5F4;(_tmp5F4=0,Cyc_fprintf(Cyc_stderr,((_tmp5F5="ConstArray_mod ",_tag_dyneither(_tmp5F5,sizeof(char),16))),_tag_dyneither(_tmp5F4,sizeof(void*),0)));}goto _LLB9;case 3: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD4)->f1)->tag == 1){_LLBE: _tmpD5=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD4)->f1)->f1;_LLBF:
# 585
{const char*_tmp5F8;void*_tmp5F7;(_tmp5F7=0,Cyc_fprintf(Cyc_stderr,((_tmp5F8="Function_mod(",_tag_dyneither(_tmp5F8,sizeof(char),14))),_tag_dyneither(_tmp5F7,sizeof(void*),0)));}
for(0;_tmpD5 != 0;_tmpD5=_tmpD5->tl){
struct _dyneither_ptr*_tmpDC=(*((struct _tuple8*)_tmpD5->hd)).f1;
if(_tmpDC == 0){const char*_tmp5FB;void*_tmp5FA;(_tmp5FA=0,Cyc_fprintf(Cyc_stderr,((_tmp5FB="?",_tag_dyneither(_tmp5FB,sizeof(char),2))),_tag_dyneither(_tmp5FA,sizeof(void*),0)));}else{
void*_tmp5FC;(_tmp5FC=0,Cyc_fprintf(Cyc_stderr,*_tmpDC,_tag_dyneither(_tmp5FC,sizeof(void*),0)));}
if(_tmpD5->tl != 0){const char*_tmp5FF;void*_tmp5FE;(_tmp5FE=0,Cyc_fprintf(Cyc_stderr,((_tmp5FF=",",_tag_dyneither(_tmp5FF,sizeof(char),2))),_tag_dyneither(_tmp5FE,sizeof(void*),0)));}}
# 592
{const char*_tmp602;void*_tmp601;(_tmp601=0,Cyc_fprintf(Cyc_stderr,((_tmp602=") ",_tag_dyneither(_tmp602,sizeof(char),3))),_tag_dyneither(_tmp601,sizeof(void*),0)));}
goto _LLB9;}else{_LLC0: _LLC1:
# 595
{const char*_tmp605;void*_tmp604;(_tmp604=0,Cyc_fprintf(Cyc_stderr,((_tmp605="Function_mod()",_tag_dyneither(_tmp605,sizeof(char),15))),_tag_dyneither(_tmp604,sizeof(void*),0)));}goto _LLB9;}case 5: _LLC2: _LLC3:
{const char*_tmp608;void*_tmp607;(_tmp607=0,Cyc_fprintf(Cyc_stderr,((_tmp608="Attributes_mod ",_tag_dyneither(_tmp608,sizeof(char),16))),_tag_dyneither(_tmp607,sizeof(void*),0)));}goto _LLB9;case 4: _LLC4: _LLC5:
{const char*_tmp60B;void*_tmp60A;(_tmp60A=0,Cyc_fprintf(Cyc_stderr,((_tmp60B="TypeParams_mod ",_tag_dyneither(_tmp60B,sizeof(char),16))),_tag_dyneither(_tmp60A,sizeof(void*),0)));}goto _LLB9;default: _LLC6: _LLC7:
{const char*_tmp60E;void*_tmp60D;(_tmp60D=0,Cyc_fprintf(Cyc_stderr,((_tmp60E="Pointer_mod ",_tag_dyneither(_tmp60E,sizeof(char),13))),_tag_dyneither(_tmp60D,sizeof(void*),0)));}goto _LLB9;}_LLB9:;}{
# 600
const char*_tmp611;void*_tmp610;(_tmp610=0,Cyc_fprintf(Cyc_stderr,((_tmp611="\n",_tag_dyneither(_tmp611,sizeof(char),2))),_tag_dyneither(_tmp610,sizeof(void*),0)));};}
# 603
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 605
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
const char*_tmp616;const char*_tmp615;struct Cyc_PP_Doc*_tmp614[3];struct Cyc_PP_Doc*p_rest=(_tmp614[2]=Cyc_PP_text(((_tmp615=")",_tag_dyneither(_tmp615,sizeof(char),2)))),((_tmp614[1]=rest,((_tmp614[0]=Cyc_PP_text(((_tmp616="(",_tag_dyneither(_tmp616,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp614,sizeof(struct Cyc_PP_Doc*),3)))))));
void*_tmpEE=(void*)tms->hd;void*_tmpEF=_tmpEE;void*_tmpF0;union Cyc_Absyn_Constraint*_tmpF1;union Cyc_Absyn_Constraint*_tmpF2;union Cyc_Absyn_Constraint*_tmpF3;struct Cyc_Absyn_Tqual _tmpF4;struct Cyc_List_List*_tmpF5;unsigned int _tmpF6;int _tmpF7;struct Cyc_List_List*_tmpF8;void*_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;union Cyc_Absyn_Constraint*_tmpFB;union Cyc_Absyn_Constraint*_tmpFC;switch(*((int*)_tmpEF)){case 0: _LLC9: _tmpFC=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_LLCA:
# 611
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp61B;const char*_tmp61A;struct Cyc_PP_Doc*_tmp619[2];return(_tmp619[1]=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFC)?
Cyc_PP_text(((_tmp61A="[]@zeroterm",_tag_dyneither(_tmp61A,sizeof(char),12)))): Cyc_PP_text(((_tmp61B="[]",_tag_dyneither(_tmp61B,sizeof(char),3)))),((_tmp619[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp619,sizeof(struct Cyc_PP_Doc*),2)))));};case 1: _LLCB: _tmpFA=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_tmpFB=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_LLCC:
# 615
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp622;const char*_tmp621;const char*_tmp620;struct Cyc_PP_Doc*_tmp61F[4];return(_tmp61F[3]=
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFB)?Cyc_PP_text(((_tmp620="]@zeroterm",_tag_dyneither(_tmp620,sizeof(char),11)))): Cyc_PP_text(((_tmp621="]",_tag_dyneither(_tmp621,sizeof(char),2)))),((_tmp61F[2]=
# 616
Cyc_Absynpp_exp2doc(_tmpFA),((_tmp61F[1]=Cyc_PP_text(((_tmp622="[",_tag_dyneither(_tmp622,sizeof(char),2)))),((_tmp61F[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp61F,sizeof(struct Cyc_PP_Doc*),4)))))))));};case 3: _LLCD: _tmpF9=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_LLCE:
# 619
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmp104=_tmpF9;struct Cyc_List_List*_tmp105;unsigned int _tmp106;struct Cyc_List_List*_tmp107;int _tmp108;struct Cyc_Absyn_VarargInfo*_tmp109;void*_tmp10A;struct Cyc_List_List*_tmp10B;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10D;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp104)->tag == 1){_LLD6: _tmp107=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp104)->f1;_tmp108=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp104)->f2;_tmp109=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp104)->f3;_tmp10A=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp104)->f4;_tmp10B=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp104)->f5;_tmp10C=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp104)->f6;_tmp10D=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp104)->f7;_LLD7: {
# 622
struct Cyc_PP_Doc*_tmp623[2];return(_tmp623[1]=Cyc_Absynpp_funargs2doc(_tmp107,_tmp108,_tmp109,_tmp10A,_tmp10B,_tmp10C,_tmp10D),((_tmp623[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp623,sizeof(struct Cyc_PP_Doc*),2)))));}}else{_LLD8: _tmp105=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp104)->f1;_tmp106=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp104)->f2;_LLD9: {
# 624
const char*_tmp62A;const char*_tmp629;const char*_tmp628;struct Cyc_PP_Doc*_tmp627[2];return(_tmp627[1]=Cyc_PP_group(((_tmp62A="(",_tag_dyneither(_tmp62A,sizeof(char),2))),((_tmp629=")",_tag_dyneither(_tmp629,sizeof(char),2))),((_tmp628=",",_tag_dyneither(_tmp628,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp105)),((_tmp627[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp627,sizeof(struct Cyc_PP_Doc*),2)))));}}_LLD5:;};case 5: _LLCF: _tmpF8=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_LLD0: {
# 627
enum Cyc_Cyclone_C_Compilers _tmp113=Cyc_Cyclone_c_compiler;if(_tmp113 == Cyc_Cyclone_Gcc_c){_LLDB: _LLDC:
# 629
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp62B[2];return(_tmp62B[1]=Cyc_Absynpp_atts2doc(_tmpF8),((_tmp62B[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp62B,sizeof(struct Cyc_PP_Doc*),2)))));};}else{_LLDD: _LLDE:
# 632
 if(Cyc_Absynpp_next_is_pointer(tms->tl)){
struct Cyc_PP_Doc*_tmp62C[2];return(_tmp62C[1]=rest,((_tmp62C[0]=Cyc_Absynpp_callconv2doc(_tmpF8),Cyc_PP_cat(_tag_dyneither(_tmp62C,sizeof(struct Cyc_PP_Doc*),2)))));}
return rest;}_LLDA:;}case 4: _LLD1: _tmpF5=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_tmpF6=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_tmpF7=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEF)->f3;_LLD2:
# 637
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmpF7){
struct Cyc_PP_Doc*_tmp62D[2];return(_tmp62D[1]=Cyc_Absynpp_ktvars2doc(_tmpF5),((_tmp62D[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp62D,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 641
struct Cyc_PP_Doc*_tmp62E[2];return(_tmp62E[1]=Cyc_Absynpp_tvars2doc(_tmpF5),((_tmp62E[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp62E,sizeof(struct Cyc_PP_Doc*),2)))));}default: _LLD3: _tmpF0=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).rgn;_tmpF1=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).nullable;_tmpF2=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).bounds;_tmpF3=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).zero_term;_tmpF4=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_LLD4: {
# 645
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(_tmpF4);
{void*_tmp118=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpF2);void*_tmp119=_tmp118;struct Cyc_Absyn_Exp*_tmp11A;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp119)->tag == 0){_LLE0: _LLE1:
 ptr=Cyc_Absynpp_question();goto _LLDF;}else{_LLE2: _tmp11A=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp119)->f1;_LLE3:
# 653
{const char*_tmp630;const char*_tmp62F;ptr=Cyc_PP_text(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpF1)?(_tmp630="*",_tag_dyneither(_tmp630,sizeof(char),2)):((_tmp62F="@",_tag_dyneither(_tmp62F,sizeof(char),2))));}{
struct _tuple11 _tmp11D=Cyc_Evexp_eval_const_uint_exp(_tmp11A);struct _tuple11 _tmp11E=_tmp11D;unsigned int _tmp11F;int _tmp120;_LLE5: _tmp11F=_tmp11E.f1;_tmp120=_tmp11E.f2;_LLE6:
 if(!_tmp120  || _tmp11F != 1){
struct Cyc_PP_Doc*_tmp631[4];ptr=((_tmp631[3]=Cyc_Absynpp_rb(),((_tmp631[2]=Cyc_Absynpp_exp2doc(_tmp11A),((_tmp631[1]=Cyc_Absynpp_lb(),((_tmp631[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp631,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLDF;_LLE4:;};}_LLDF:;}
# 659
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF3)){
const char*_tmp632;ztd=Cyc_PP_text(((_tmp632="@zeroterm",_tag_dyneither(_tmp632,sizeof(char),10))));}else{
if(is_char_ptr  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF3)){
const char*_tmp633;ztd=Cyc_PP_text(((_tmp633="@nozeroterm",_tag_dyneither(_tmp633,sizeof(char),12))));}}}
# 665
{void*_tmp124=Cyc_Tcutil_compress(_tmpF0);void*_tmp125=_tmp124;switch(*((int*)_tmp125)){case 20: _LLEA: _LLEB:
 goto _LLE9;case 1: _LLEC: if(Cyc_Absynpp_print_for_cycdoc){_LLED:
 goto _LLE9;}else{goto _LLEE;}default: _LLEE: _LLEF:
 rgd=Cyc_Absynpp_rgn2doc(_tmpF0);}_LLE9:;}{
# 670
const char*_tmp634;struct Cyc_PP_Doc*spacer1=tqd != mt  && (ztd != mt  || rgd != mt)?Cyc_PP_text(((_tmp634=" ",_tag_dyneither(_tmp634,sizeof(char),2)))): mt;
const char*_tmp635;struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(((_tmp635=" ",_tag_dyneither(_tmp635,sizeof(char),2)))): mt;
struct Cyc_PP_Doc*_tmp636[7];return(_tmp636[6]=rest,((_tmp636[5]=spacer2,((_tmp636[4]=tqd,((_tmp636[3]=spacer1,((_tmp636[2]=rgd,((_tmp636[1]=ztd,((_tmp636[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp636,sizeof(struct Cyc_PP_Doc*),7)))))))))))))));};}}_LLC8:;};}
# 676
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp12C=Cyc_Tcutil_compress(t);void*_tmp12D=_tmp12C;switch(*((int*)_tmp12D)){case 20: _LLF1: _LLF2: {
const char*_tmp637;return Cyc_PP_text(((_tmp637="`H",_tag_dyneither(_tmp637,sizeof(char),3))));}case 21: _LLF3: _LLF4: {
const char*_tmp638;return Cyc_PP_text(((_tmp638="`U",_tag_dyneither(_tmp638,sizeof(char),3))));}case 22: _LLF5: _LLF6: {
const char*_tmp639;return Cyc_PP_text(((_tmp639="`RC",_tag_dyneither(_tmp639,sizeof(char),4))));}default: _LLF7: _LLF8:
 return Cyc_Absynpp_ntyp2doc(t);}_LLF0:;}
# 686
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 690
void*_tmp131=Cyc_Tcutil_compress(t);void*_tmp132=_tmp131;struct Cyc_List_List*_tmp133;void*_tmp134;switch(*((int*)_tmp132)){case 23: _LLFA: _tmp134=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp132)->f1;_LLFB:
{struct Cyc_List_List*_tmp63A;*rgions=((_tmp63A=_cycalloc(sizeof(*_tmp63A)),((_tmp63A->hd=Cyc_Absynpp_rgn2doc(_tmp134),((_tmp63A->tl=*rgions,_tmp63A))))));}goto _LLF9;case 24: _LLFC: _tmp133=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp132)->f1;_LLFD:
# 693
 for(0;_tmp133 != 0;_tmp133=_tmp133->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp133->hd);}
# 696
goto _LLF9;default: _LLFE: _LLFF:
{struct Cyc_List_List*_tmp63B;*effects=((_tmp63B=_cycalloc(sizeof(*_tmp63B)),((_tmp63B->hd=Cyc_Absynpp_typ2doc(t),((_tmp63B->tl=*effects,_tmp63B))))));}goto _LLF9;}_LLF9:;}
# 701
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0  && effects != 0){
const char*_tmp63E;const char*_tmp63D;const char*_tmp63C;return Cyc_PP_group(((_tmp63C="",_tag_dyneither(_tmp63C,sizeof(char),1))),((_tmp63D="",_tag_dyneither(_tmp63D,sizeof(char),1))),((_tmp63E="+",_tag_dyneither(_tmp63E,sizeof(char),2))),effects);}else{
# 709
const char*_tmp63F;struct Cyc_PP_Doc*_tmp13A=Cyc_Absynpp_group_braces(((_tmp63F=",",_tag_dyneither(_tmp63F,sizeof(char),2))),rgions);
struct Cyc_List_List*_tmp643;const char*_tmp642;const char*_tmp641;const char*_tmp640;return Cyc_PP_group(((_tmp640="",_tag_dyneither(_tmp640,sizeof(char),1))),((_tmp641="",_tag_dyneither(_tmp641,sizeof(char),1))),((_tmp642="+",_tag_dyneither(_tmp642,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((_tmp643=_cycalloc(sizeof(*_tmp643)),((_tmp643->hd=_tmp13A,((_tmp643->tl=0,_tmp643))))))));}}
# 714
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp140=k;if(_tmp140 == Cyc_Absyn_StructA){_LL101: _LL102: {
const char*_tmp644;return Cyc_PP_text(((_tmp644="struct ",_tag_dyneither(_tmp644,sizeof(char),8))));}}else{_LL103: _LL104: {
const char*_tmp645;return Cyc_PP_text(((_tmp645="union ",_tag_dyneither(_tmp645,sizeof(char),7))));}}_LL100:;}
# 722
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp143=t;struct Cyc_Absyn_Datatypedecl*_tmp144;struct Cyc_Absyn_Enumdecl*_tmp145;struct Cyc_Absyn_Aggrdecl*_tmp146;void*_tmp147;void*_tmp148;void*_tmp149;void*_tmp14A;void*_tmp14B;struct _tuple0*_tmp14C;struct Cyc_List_List*_tmp14D;struct Cyc_Absyn_Typedefdecl*_tmp14E;struct _dyneither_ptr _tmp14F;struct Cyc_Absyn_Exp*_tmp150;struct Cyc_Absyn_Exp*_tmp151;struct _tuple0*_tmp152;struct Cyc_List_List*_tmp153;enum Cyc_Absyn_AggrKind _tmp154;struct Cyc_List_List*_tmp155;union Cyc_Absyn_AggrInfoU _tmp156;struct Cyc_List_List*_tmp157;struct Cyc_List_List*_tmp158;int _tmp159;enum Cyc_Absyn_Sign _tmp15A;enum Cyc_Absyn_Size_of _tmp15B;union Cyc_Absyn_DatatypeFieldInfoU _tmp15C;struct Cyc_List_List*_tmp15D;union Cyc_Absyn_DatatypeInfoU _tmp15E;struct Cyc_List_List*_tmp15F;struct Cyc_Absyn_Tvar*_tmp160;struct Cyc_Core_Opt*_tmp161;void*_tmp162;int _tmp163;struct Cyc_Core_Opt*_tmp164;switch(*((int*)_tmp143)){case 8: _LL106: _LL107: {
# 726
const char*_tmp646;return Cyc_PP_text(((_tmp646="[[[array]]]",_tag_dyneither(_tmp646,sizeof(char),12))));}case 9: _LL108: _LL109:
 return Cyc_PP_nil_doc();case 5: _LL10A: _LL10B:
 return Cyc_PP_nil_doc();case 0: _LL10C: _LL10D:
# 730
{const char*_tmp647;s=Cyc_PP_text(((_tmp647="void",_tag_dyneither(_tmp647,sizeof(char),5))));}goto _LL105;case 1: _LL10E: _tmp161=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f1;_tmp162=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f2;_tmp163=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f3;_tmp164=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f4;_LL10F:
# 732
 if(_tmp162 != 0)
# 734
return Cyc_Absynpp_ntyp2doc(_tmp162);else{
# 736
const char*_tmp648;struct _dyneither_ptr kindstring=_tmp161 == 0?(_tmp648="K",_tag_dyneither(_tmp648,sizeof(char),2)): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)_tmp161->v);
const char*_tmp64D;void*_tmp64C[2];struct Cyc_String_pa_PrintArg_struct _tmp64B;struct Cyc_Int_pa_PrintArg_struct _tmp64A;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp64A.tag=1,((_tmp64A.f1=(unsigned long)_tmp163,((_tmp64B.tag=0,((_tmp64B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kindstring),((_tmp64C[0]=& _tmp64B,((_tmp64C[1]=& _tmp64A,Cyc_aprintf(((_tmp64D="`E%s%d",_tag_dyneither(_tmp64D,sizeof(char),7))),_tag_dyneither(_tmp64C,sizeof(void*),2)))))))))))))));}
# 753 "absynpp.cyc"
goto _LL105;case 2: _LL110: _tmp160=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp143)->f1;_LL111:
# 755
 s=Cyc_Absynpp_tvar2doc(_tmp160);
if(Cyc_Absynpp_print_all_kinds){
const char*_tmp650;struct Cyc_PP_Doc*_tmp64F[3];s=((_tmp64F[2]=Cyc_Absynpp_kindbound2doc(_tmp160->kind),((_tmp64F[1]=Cyc_PP_text(((_tmp650="::",_tag_dyneither(_tmp650,sizeof(char),3)))),((_tmp64F[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp64F,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 759
goto _LL105;case 3: _LL112: _tmp15E=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp143)->f1).datatype_info;_tmp15F=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp143)->f1).targs;_LL113:
# 761
{union Cyc_Absyn_DatatypeInfoU _tmp16E=_tmp15E;struct _tuple0*_tmp16F;int _tmp170;struct _tuple0*_tmp171;int _tmp172;if((_tmp16E.UnknownDatatype).tag == 1){_LL145: _tmp171=((_tmp16E.UnknownDatatype).val).name;_tmp172=((_tmp16E.UnknownDatatype).val).is_extensible;_LL146:
# 763
 _tmp16F=_tmp171;_tmp170=_tmp172;goto _LL148;}else{_LL147: _tmp16F=(*(_tmp16E.KnownDatatype).val)->name;_tmp170=(*(_tmp16E.KnownDatatype).val)->is_extensible;_LL148: {
# 765
const char*_tmp651;struct Cyc_PP_Doc*_tmp173=Cyc_PP_text(((_tmp651="datatype ",_tag_dyneither(_tmp651,sizeof(char),10))));
const char*_tmp652;struct Cyc_PP_Doc*_tmp174=_tmp170?Cyc_PP_text(((_tmp652="@extensible ",_tag_dyneither(_tmp652,sizeof(char),13)))): Cyc_PP_nil_doc();
{struct Cyc_PP_Doc*_tmp653[4];s=((_tmp653[3]=Cyc_Absynpp_tps2doc(_tmp15F),((_tmp653[2]=Cyc_Absynpp_qvar2doc(_tmp16F),((_tmp653[1]=_tmp173,((_tmp653[0]=_tmp174,Cyc_PP_cat(_tag_dyneither(_tmp653,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL144;}}_LL144:;}
# 770
goto _LL105;case 4: _LL114: _tmp15C=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp143)->f1).field_info;_tmp15D=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp143)->f1).targs;_LL115:
# 772
{union Cyc_Absyn_DatatypeFieldInfoU _tmp178=_tmp15C;struct _tuple0*_tmp179;int _tmp17A;struct _tuple0*_tmp17B;struct _tuple0*_tmp17C;struct _tuple0*_tmp17D;int _tmp17E;if((_tmp178.UnknownDatatypefield).tag == 1){_LL14A: _tmp17C=((_tmp178.UnknownDatatypefield).val).datatype_name;_tmp17D=((_tmp178.UnknownDatatypefield).val).field_name;_tmp17E=((_tmp178.UnknownDatatypefield).val).is_extensible;_LL14B:
# 774
 _tmp179=_tmp17C;_tmp17A=_tmp17E;_tmp17B=_tmp17D;goto _LL14D;}else{_LL14C: _tmp179=(((_tmp178.KnownDatatypefield).val).f1)->name;_tmp17A=(((_tmp178.KnownDatatypefield).val).f1)->is_extensible;_tmp17B=(((_tmp178.KnownDatatypefield).val).f2)->name;_LL14D: {
# 777
const char*_tmp655;const char*_tmp654;struct Cyc_PP_Doc*_tmp17F=Cyc_PP_text(_tmp17A?(_tmp655="@extensible datatype ",_tag_dyneither(_tmp655,sizeof(char),22)):((_tmp654="datatype ",_tag_dyneither(_tmp654,sizeof(char),10))));
{const char*_tmp658;struct Cyc_PP_Doc*_tmp657[4];s=((_tmp657[3]=Cyc_Absynpp_qvar2doc(_tmp17B),((_tmp657[2]=Cyc_PP_text(((_tmp658=".",_tag_dyneither(_tmp658,sizeof(char),2)))),((_tmp657[1]=Cyc_Absynpp_qvar2doc(_tmp179),((_tmp657[0]=_tmp17F,Cyc_PP_cat(_tag_dyneither(_tmp657,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL149;}}_LL149:;}
# 781
goto _LL105;case 6: _LL116: _tmp15A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp143)->f1;_tmp15B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp143)->f2;_LL117: {
# 783
struct _dyneither_ptr sns;
struct _dyneither_ptr ts;
{enum Cyc_Absyn_Sign _tmp184=_tmp15A;switch(_tmp184){case Cyc_Absyn_None: _LL14F: _LL150:
 goto _LL152;case Cyc_Absyn_Signed: _LL151: _LL152:
{const char*_tmp659;sns=((_tmp659="",_tag_dyneither(_tmp659,sizeof(char),1)));}goto _LL14E;default: _LL153: _LL154:
{const char*_tmp65A;sns=((_tmp65A="unsigned ",_tag_dyneither(_tmp65A,sizeof(char),10)));}goto _LL14E;}_LL14E:;}
# 790
{enum Cyc_Absyn_Size_of _tmp187=_tmp15B;switch(_tmp187){case Cyc_Absyn_Char_sz: _LL156: _LL157:
# 792
{enum Cyc_Absyn_Sign _tmp188=_tmp15A;switch(_tmp188){case Cyc_Absyn_None: _LL161: _LL162:
{const char*_tmp65B;sns=((_tmp65B="",_tag_dyneither(_tmp65B,sizeof(char),1)));}goto _LL160;case Cyc_Absyn_Signed: _LL163: _LL164:
{const char*_tmp65C;sns=((_tmp65C="signed ",_tag_dyneither(_tmp65C,sizeof(char),8)));}goto _LL160;default: _LL165: _LL166:
{const char*_tmp65D;sns=((_tmp65D="unsigned ",_tag_dyneither(_tmp65D,sizeof(char),10)));}goto _LL160;}_LL160:;}
# 797
{const char*_tmp65E;ts=((_tmp65E="char",_tag_dyneither(_tmp65E,sizeof(char),5)));}
goto _LL155;case Cyc_Absyn_Short_sz: _LL158: _LL159:
{const char*_tmp65F;ts=((_tmp65F="short",_tag_dyneither(_tmp65F,sizeof(char),6)));}goto _LL155;case Cyc_Absyn_Int_sz: _LL15A: _LL15B:
{const char*_tmp660;ts=((_tmp660="int",_tag_dyneither(_tmp660,sizeof(char),4)));}goto _LL155;case Cyc_Absyn_Long_sz: _LL15C: _LL15D:
{const char*_tmp661;ts=((_tmp661="long",_tag_dyneither(_tmp661,sizeof(char),5)));}goto _LL155;default: _LL15E: _LL15F:
# 803
{enum Cyc_Cyclone_C_Compilers _tmp190=Cyc_Cyclone_c_compiler;if(_tmp190 == Cyc_Cyclone_Gcc_c){_LL168: _LL169:
{const char*_tmp662;ts=((_tmp662="long long",_tag_dyneither(_tmp662,sizeof(char),10)));}goto _LL167;}else{_LL16A: _LL16B:
{const char*_tmp663;ts=((_tmp663="__int64",_tag_dyneither(_tmp663,sizeof(char),8)));}goto _LL167;}_LL167:;}
# 807
goto _LL155;}_LL155:;}
# 809
{const char*_tmp668;void*_tmp667[2];struct Cyc_String_pa_PrintArg_struct _tmp666;struct Cyc_String_pa_PrintArg_struct _tmp665;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp665.tag=0,((_tmp665.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts),((_tmp666.tag=0,((_tmp666.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns),((_tmp667[0]=& _tmp666,((_tmp667[1]=& _tmp665,Cyc_aprintf(((_tmp668="%s%s",_tag_dyneither(_tmp668,sizeof(char),5))),_tag_dyneither(_tmp667,sizeof(void*),2)))))))))))))));}
goto _LL105;}case 7: _LL118: _tmp159=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp143)->f1;_LL119:
# 812
 if(_tmp159 == 0){
const char*_tmp669;s=Cyc_PP_text(((_tmp669="float",_tag_dyneither(_tmp669,sizeof(char),6))));}else{
if(_tmp159 == 1){
const char*_tmp66A;s=Cyc_PP_text(((_tmp66A="double",_tag_dyneither(_tmp66A,sizeof(char),7))));}else{
# 817
const char*_tmp66B;s=Cyc_PP_text(((_tmp66B="long double",_tag_dyneither(_tmp66B,sizeof(char),12))));}}
goto _LL105;case 10: _LL11A: _tmp158=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp143)->f1;_LL11B:
# 820
{struct Cyc_PP_Doc*_tmp66C[2];s=((_tmp66C[1]=Cyc_Absynpp_args2doc(_tmp158),((_tmp66C[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp66C,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL105;case 11: _LL11C: _tmp156=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp143)->f1).aggr_info;_tmp157=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp143)->f1).targs;_LL11D: {
# 823
struct _tuple10 _tmp19B=Cyc_Absyn_aggr_kinded_name(_tmp156);struct _tuple10 _tmp19C=_tmp19B;enum Cyc_Absyn_AggrKind _tmp19D;struct _tuple0*_tmp19E;_LL16D: _tmp19D=_tmp19C.f1;_tmp19E=_tmp19C.f2;_LL16E:
{struct Cyc_PP_Doc*_tmp66D[3];s=((_tmp66D[2]=Cyc_Absynpp_tps2doc(_tmp157),((_tmp66D[1]=Cyc_Absynpp_qvar2doc(_tmp19E),((_tmp66D[0]=Cyc_Absynpp_aggr_kind2doc(_tmp19D),Cyc_PP_cat(_tag_dyneither(_tmp66D,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL105;_LL16C:;}case 12: _LL11E: _tmp154=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp143)->f1;_tmp155=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp143)->f2;_LL11F:
# 827
{struct Cyc_PP_Doc*_tmp66E[4];s=((_tmp66E[3]=
# 829
Cyc_Absynpp_rb(),((_tmp66E[2]=
# 828
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp155)),((_tmp66E[1]=
# 827
Cyc_Absynpp_lb(),((_tmp66E[0]=Cyc_Absynpp_aggr_kind2doc(_tmp154),Cyc_PP_cat(_tag_dyneither(_tmp66E,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 830
goto _LL105;case 14: _LL120: _tmp153=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp143)->f1;_LL121:
# 832
{const char*_tmp671;struct Cyc_PP_Doc*_tmp670[4];s=((_tmp670[3]=Cyc_Absynpp_rb(),((_tmp670[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp153)),((_tmp670[1]=Cyc_Absynpp_lb(),((_tmp670[0]=Cyc_PP_text(((_tmp671="enum ",_tag_dyneither(_tmp671,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp670,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL105;case 13: _LL122: _tmp152=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp143)->f1;_LL123:
# 834
{const char*_tmp674;struct Cyc_PP_Doc*_tmp673[2];s=((_tmp673[1]=Cyc_Absynpp_qvar2doc(_tmp152),((_tmp673[0]=Cyc_PP_text(((_tmp674="enum ",_tag_dyneither(_tmp674,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp673,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL105;case 18: _LL124: _tmp151=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp143)->f1;_LL125:
# 836
{const char*_tmp679;const char*_tmp678;struct Cyc_PP_Doc*_tmp677[3];s=((_tmp677[2]=Cyc_PP_text(((_tmp678=")",_tag_dyneither(_tmp678,sizeof(char),2)))),((_tmp677[1]=Cyc_Absynpp_exp2doc(_tmp151),((_tmp677[0]=Cyc_PP_text(((_tmp679="valueof_t(",_tag_dyneither(_tmp679,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp677,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL105;case 27: _LL126: _tmp150=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp143)->f1;_LL127:
# 838
{const char*_tmp67E;const char*_tmp67D;struct Cyc_PP_Doc*_tmp67C[3];s=((_tmp67C[2]=Cyc_PP_text(((_tmp67D=")",_tag_dyneither(_tmp67D,sizeof(char),2)))),((_tmp67C[1]=Cyc_Absynpp_exp2doc(_tmp150),((_tmp67C[0]=Cyc_PP_text(((_tmp67E="typeof(",_tag_dyneither(_tmp67E,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp67C,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL105;case 28: _LL128: _tmp14F=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp143)->f1;_LL129:
# 840
 s=Cyc_PP_text(_tmp14F);goto _LL105;case 17: _LL12A: _tmp14C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f1;_tmp14D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f2;_tmp14E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f3;_LL12B:
# 847
{struct Cyc_PP_Doc*_tmp67F[2];s=((_tmp67F[1]=Cyc_Absynpp_tps2doc(_tmp14D),((_tmp67F[0]=Cyc_Absynpp_qvar2doc(_tmp14C),Cyc_PP_cat(_tag_dyneither(_tmp67F,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL105;case 15: _LL12C: _tmp14B=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp143)->f1;_LL12D:
# 850
{const char*_tmp684;const char*_tmp683;struct Cyc_PP_Doc*_tmp682[3];s=((_tmp682[2]=Cyc_PP_text(((_tmp683=">",_tag_dyneither(_tmp683,sizeof(char),2)))),((_tmp682[1]=Cyc_Absynpp_rgn2doc(_tmp14B),((_tmp682[0]=Cyc_PP_text(((_tmp684="region_t<",_tag_dyneither(_tmp684,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp682,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL105;case 16: _LL12E: _tmp149=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp143)->f1;_tmp14A=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp143)->f2;_LL12F:
# 852
{const char*_tmp68B;const char*_tmp68A;const char*_tmp689;struct Cyc_PP_Doc*_tmp688[5];s=((_tmp688[4]=
Cyc_PP_text(((_tmp689=">",_tag_dyneither(_tmp689,sizeof(char),2)))),((_tmp688[3]=
# 852
Cyc_Absynpp_rgn2doc(_tmp14A),((_tmp688[2]=Cyc_PP_text(((_tmp68A=",",_tag_dyneither(_tmp68A,sizeof(char),2)))),((_tmp688[1]=Cyc_Absynpp_rgn2doc(_tmp149),((_tmp688[0]=Cyc_PP_text(((_tmp68B="dynregion_t<",_tag_dyneither(_tmp68B,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp688,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL105;case 19: _LL130: _tmp148=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp143)->f1;_LL131:
# 855
{const char*_tmp690;const char*_tmp68F;struct Cyc_PP_Doc*_tmp68E[3];s=((_tmp68E[2]=Cyc_PP_text(((_tmp68F=">",_tag_dyneither(_tmp68F,sizeof(char),2)))),((_tmp68E[1]=Cyc_Absynpp_typ2doc(_tmp148),((_tmp68E[0]=Cyc_PP_text(((_tmp690="tag_t<",_tag_dyneither(_tmp690,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp68E,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL105;case 21: _LL132: _LL133:
 goto _LL135;case 20: _LL134: _LL135:
 goto _LL137;case 22: _LL136: _LL137:
# 859
 s=Cyc_Absynpp_rgn2doc(t);goto _LL105;case 25: _LL138: _tmp147=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp143)->f1;_LL139:
# 861
{const char*_tmp695;const char*_tmp694;struct Cyc_PP_Doc*_tmp693[3];s=((_tmp693[2]=Cyc_PP_text(((_tmp694=")",_tag_dyneither(_tmp694,sizeof(char),2)))),((_tmp693[1]=Cyc_Absynpp_typ2doc(_tmp147),((_tmp693[0]=Cyc_PP_text(((_tmp695="regions(",_tag_dyneither(_tmp695,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp693,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL105;case 23: _LL13A: _LL13B:
 goto _LL13D;case 24: _LL13C: _LL13D:
# 864
 s=Cyc_Absynpp_eff2doc(t);goto _LL105;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)){case 0: _LL13E: _tmp146=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_LL13F:
# 866
 s=Cyc_Absynpp_aggrdecl2doc(_tmp146);goto _LL105;case 1: _LL140: _tmp145=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_LL141:
# 868
 s=Cyc_Absynpp_enumdecl2doc(_tmp145);goto _LL105;default: _LL142: _tmp144=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_LL143:
# 870
 s=Cyc_Absynpp_datatypedecl2doc(_tmp144);goto _LL105;}}_LL105:;}
# 872
return s;}
# 875
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _dyneither_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple16{void*f1;void*f2;};
# 879
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
struct _tuple16*_tmp1B9=cmp;void*_tmp1BA;void*_tmp1BB;_LL172: _tmp1BA=_tmp1B9->f1;_tmp1BB=_tmp1B9->f2;_LL173: {
const char*_tmp698;struct Cyc_PP_Doc*_tmp697[3];return(_tmp697[2]=Cyc_Absynpp_rgn2doc(_tmp1BB),((_tmp697[1]=Cyc_PP_text(((_tmp698=" > ",_tag_dyneither(_tmp698,sizeof(char),4)))),((_tmp697[0]=Cyc_Absynpp_rgn2doc(_tmp1BA),Cyc_PP_cat(_tag_dyneither(_tmp697,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL171:;}
# 884
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
const char*_tmp69B;const char*_tmp69A;const char*_tmp699;return Cyc_PP_group(((_tmp699="",_tag_dyneither(_tmp699,sizeof(char),1))),((_tmp69A="",_tag_dyneither(_tmp69A,sizeof(char),1))),((_tmp69B=",",_tag_dyneither(_tmp69B,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
# 888
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _dyneither_ptr*_tmp1C1=(*t).f1;
struct Cyc_Core_Opt*_tmp69C;struct Cyc_Core_Opt*dopt=_tmp1C1 == 0?0:((_tmp69C=_cycalloc(sizeof(*_tmp69C)),((_tmp69C->v=Cyc_PP_text(*_tmp1C1),_tmp69C))));
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 894
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 898
struct Cyc_List_List*_tmp1C3=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs){
const char*_tmp69F;struct Cyc_List_List*_tmp69E;_tmp1C3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1C3,((_tmp69E=_cycalloc(sizeof(*_tmp69E)),((_tmp69E->hd=Cyc_PP_text(((_tmp69F="...",_tag_dyneither(_tmp69F,sizeof(char),4)))),((_tmp69E->tl=0,_tmp69E)))))));}else{
if(cyc_varargs != 0){
const char*_tmp6A8;const char*_tmp6A7;const char*_tmp6A6;struct _tuple8*_tmp6A5;struct Cyc_PP_Doc*_tmp6A4[3];struct Cyc_PP_Doc*_tmp1C6=(_tmp6A4[2]=
# 905
Cyc_Absynpp_funarg2doc(((_tmp6A5=_cycalloc(sizeof(*_tmp6A5)),((_tmp6A5->f1=cyc_varargs->name,((_tmp6A5->f2=cyc_varargs->tq,((_tmp6A5->f3=cyc_varargs->type,_tmp6A5))))))))),((_tmp6A4[1]=
# 904
cyc_varargs->inject?Cyc_PP_text(((_tmp6A6=" inject ",_tag_dyneither(_tmp6A6,sizeof(char),9)))): Cyc_PP_text(((_tmp6A7=" ",_tag_dyneither(_tmp6A7,sizeof(char),2)))),((_tmp6A4[0]=
# 903
Cyc_PP_text(((_tmp6A8="...",_tag_dyneither(_tmp6A8,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp6A4,sizeof(struct Cyc_PP_Doc*),3)))))));
# 907
struct Cyc_List_List*_tmp6A9;_tmp1C3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1C3,((_tmp6A9=_cycalloc(sizeof(*_tmp6A9)),((_tmp6A9->hd=_tmp1C6,((_tmp6A9->tl=0,_tmp6A9)))))));}}{
# 909
const char*_tmp6AC;const char*_tmp6AB;const char*_tmp6AA;struct Cyc_PP_Doc*_tmp1CD=Cyc_PP_group(((_tmp6AA="",_tag_dyneither(_tmp6AA,sizeof(char),1))),((_tmp6AB="",_tag_dyneither(_tmp6AB,sizeof(char),1))),((_tmp6AC=",",_tag_dyneither(_tmp6AC,sizeof(char),2))),_tmp1C3);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){
const char*_tmp6AF;struct Cyc_PP_Doc*_tmp6AE[3];_tmp1CD=((_tmp6AE[2]=Cyc_Absynpp_eff2doc(effopt),((_tmp6AE[1]=Cyc_PP_text(((_tmp6AF=";",_tag_dyneither(_tmp6AF,sizeof(char),2)))),((_tmp6AE[0]=_tmp1CD,Cyc_PP_cat(_tag_dyneither(_tmp6AE,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){
const char*_tmp6B2;struct Cyc_PP_Doc*_tmp6B1[3];_tmp1CD=((_tmp6B1[2]=Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp6B1[1]=Cyc_PP_text(((_tmp6B2=":",_tag_dyneither(_tmp6B2,sizeof(char),2)))),((_tmp6B1[0]=_tmp1CD,Cyc_PP_cat(_tag_dyneither(_tmp6B1,sizeof(struct Cyc_PP_Doc*),3))))))));}{
const char*_tmp6B7;const char*_tmp6B6;struct Cyc_PP_Doc*_tmp6B5[3];struct Cyc_PP_Doc*_tmp1D2=(_tmp6B5[2]=Cyc_PP_text(((_tmp6B6=")",_tag_dyneither(_tmp6B6,sizeof(char),2)))),((_tmp6B5[1]=_tmp1CD,((_tmp6B5[0]=Cyc_PP_text(((_tmp6B7="(",_tag_dyneither(_tmp6B7,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6B5,sizeof(struct Cyc_PP_Doc*),3)))))));
if(req != 0){
const char*_tmp6BC;const char*_tmp6BB;struct Cyc_PP_Doc*_tmp6BA[4];_tmp1D2=((_tmp6BA[3]=Cyc_PP_text(((_tmp6BB=")",_tag_dyneither(_tmp6BB,sizeof(char),2)))),((_tmp6BA[2]=Cyc_Absynpp_exp2doc(req),((_tmp6BA[1]=Cyc_PP_text(((_tmp6BC=" @requires(",_tag_dyneither(_tmp6BC,sizeof(char),12)))),((_tmp6BA[0]=_tmp1D2,Cyc_PP_cat(_tag_dyneither(_tmp6BA,sizeof(struct Cyc_PP_Doc*),4))))))))));}
if(ens != 0){
const char*_tmp6C1;const char*_tmp6C0;struct Cyc_PP_Doc*_tmp6BF[4];_tmp1D2=((_tmp6BF[3]=Cyc_PP_text(((_tmp6C0=")",_tag_dyneither(_tmp6C0,sizeof(char),2)))),((_tmp6BF[2]=Cyc_Absynpp_exp2doc(ens),((_tmp6BF[1]=Cyc_PP_text(((_tmp6C1=" @ensures(",_tag_dyneither(_tmp6C1,sizeof(char),11)))),((_tmp6BF[0]=_tmp1D2,Cyc_PP_cat(_tag_dyneither(_tmp6BF,sizeof(struct Cyc_PP_Doc*),4))))))))));}
return _tmp1D2;};};}
# 922
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg){
struct _tuple8*_tmp1DF=arg;struct _dyneither_ptr*_tmp1E0;struct Cyc_Absyn_Tqual _tmp1E1;void*_tmp1E2;_LL177: _tmp1E0=_tmp1DF->f1;_tmp1E1=_tmp1DF->f2;_tmp1E2=_tmp1DF->f3;_LL178: {
struct _tuple8*_tmp6C2;return(_tmp6C2=_cycalloc(sizeof(*_tmp6C2)),((_tmp6C2->f1=_tmp1E0,((_tmp6C2->f2=_tmp1E1,((_tmp6C2->f3=_tmp1E2,_tmp6C2)))))));}_LL176:;}
# 927
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){return Cyc_PP_text(*v);}
# 929
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp1E4=0;
int match;
{union Cyc_Absyn_Nmspace _tmp1E5=(*q).f1;union Cyc_Absyn_Nmspace _tmp1E6=_tmp1E5;struct Cyc_List_List*_tmp1E7;struct Cyc_List_List*_tmp1E8;struct Cyc_List_List*_tmp1E9;switch((_tmp1E6.C_n).tag){case 4: _LL17C: _LL17D:
# 934
 _tmp1E9=0;goto _LL17F;case 1: _LL17E: _tmp1E9=(_tmp1E6.Rel_n).val;_LL17F:
# 936
 match=0;
_tmp1E4=_tmp1E9;
goto _LL17B;case 3: _LL180: _tmp1E8=(_tmp1E6.C_n).val;_LL181:
# 940
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1E8,Cyc_Absynpp_curr_namespace);
# 942
goto _LL17B;default: _LL182: _tmp1E7=(_tmp1E6.Abs_n).val;_LL183:
# 944
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1E7,Cyc_Absynpp_curr_namespace);
{struct Cyc_List_List*_tmp6C3;_tmp1E4=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(_tmp6C3=_cycalloc(sizeof(*_tmp6C3)),((_tmp6C3->hd=Cyc_Absynpp_cyc_stringptr,((_tmp6C3->tl=_tmp1E7,_tmp6C3))))): _tmp1E7;}
goto _LL17B;}_LL17B:;}
# 948
if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp6C5;struct Cyc_List_List*_tmp6C4;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1E4,(
(_tmp6C4=_cycalloc(sizeof(*_tmp6C4)),((_tmp6C4->hd=(*q).f2,((_tmp6C4->tl=0,_tmp6C4))))))),((_tmp6C5="_",_tag_dyneither(_tmp6C5,sizeof(char),2))));}else{
# 953
if(match)
return*(*q).f2;else{
# 956
const char*_tmp6C7;struct Cyc_List_List*_tmp6C6;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1E4,(
(_tmp6C6=_cycalloc(sizeof(*_tmp6C6)),((_tmp6C6->hd=(*q).f2,((_tmp6C6->tl=0,_tmp6C6))))))),((_tmp6C7="::",_tag_dyneither(_tmp6C7,sizeof(char),3))));}}}
# 961
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 965
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp1EF=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output){
# 969
const char*_tmp6C9;const char*_tmp6C8;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp6C8="\\textbf{",_tag_dyneither(_tmp6C8,sizeof(char),9))),(struct _dyneither_ptr)_tmp1EF),((_tmp6C9="}",_tag_dyneither(_tmp6C9,sizeof(char),2)))),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp1EF));}else{
return Cyc_PP_text(_tmp1EF);}}
# 974
struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 976
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 979
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp1F2=(*v).f1;union Cyc_Absyn_Nmspace _tmp1F3=_tmp1F2;struct Cyc_List_List*_tmp1F4;struct Cyc_List_List*_tmp1F5;switch((_tmp1F3.C_n).tag){case 4: _LL185: _LL186:
 goto _LL188;case 1: if((_tmp1F3.Rel_n).val == 0){_LL187: _LL188:
 return*(*v).f2;}else{_LL18D: _LL18E: {
# 990
const char*_tmp6CA;return(struct _dyneither_ptr)Cyc_strconcat(((_tmp6CA="/* bad namespace : */ ",_tag_dyneither(_tmp6CA,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));}}case 3: _LL189: _tmp1F5=(_tmp1F3.C_n).val;_LL18A:
# 983
 _tmp1F4=_tmp1F5;goto _LL18C;default: _LL18B: _tmp1F4=(_tmp1F3.Abs_n).val;_LL18C:
# 985
 if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp1F4,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 988
goto _LL18E;}}_LL184:;}else{
# 993
return*(*v).f2;}}
# 996
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 999
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _dyneither_ptr _tmp1F7=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output){
# 1003
const char*_tmp6CC;const char*_tmp6CB;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp6CB="\\textbf{",_tag_dyneither(_tmp6CB,sizeof(char),9))),(struct _dyneither_ptr)_tmp1F7),((_tmp6CC="}",_tag_dyneither(_tmp6CC,sizeof(char),2)))),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp1F7));}else{
return Cyc_PP_text(_tmp1F7);}}
# 1008
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}
# 1019 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp1FA=e->r;void*_tmp1FB=_tmp1FA;struct Cyc_Absyn_Exp*_tmp1FC;struct Cyc_Absyn_Exp*_tmp1FD;enum Cyc_Absyn_Primop _tmp1FE;switch(*((int*)_tmp1FB)){case 0: _LL190: _LL191:
 goto _LL193;case 1: _LL192: _LL193:
 return 10000;case 2: _LL194: _tmp1FE=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1FB)->f1;_LL195: {
# 1024
enum Cyc_Absyn_Primop _tmp1FF=_tmp1FE;switch(_tmp1FF){case Cyc_Absyn_Plus: _LL1E1: _LL1E2:
 return 100;case Cyc_Absyn_Times: _LL1E3: _LL1E4:
 return 110;case Cyc_Absyn_Minus: _LL1E5: _LL1E6:
 return 100;case Cyc_Absyn_Div: _LL1E7: _LL1E8:
 goto _LL1EA;case Cyc_Absyn_Mod: _LL1E9: _LL1EA:
 return 110;case Cyc_Absyn_Eq: _LL1EB: _LL1EC:
 goto _LL1EE;case Cyc_Absyn_Neq: _LL1ED: _LL1EE:
 return 70;case Cyc_Absyn_Gt: _LL1EF: _LL1F0:
 goto _LL1F2;case Cyc_Absyn_Lt: _LL1F1: _LL1F2:
 goto _LL1F4;case Cyc_Absyn_Gte: _LL1F3: _LL1F4:
 goto _LL1F6;case Cyc_Absyn_Lte: _LL1F5: _LL1F6:
 return 80;case Cyc_Absyn_Not: _LL1F7: _LL1F8:
 goto _LL1FA;case Cyc_Absyn_Bitnot: _LL1F9: _LL1FA:
 return 130;case Cyc_Absyn_Bitand: _LL1FB: _LL1FC:
 return 60;case Cyc_Absyn_Bitor: _LL1FD: _LL1FE:
 return 40;case Cyc_Absyn_Bitxor: _LL1FF: _LL200:
 return 50;case Cyc_Absyn_Bitlshift: _LL201: _LL202:
 return 90;case Cyc_Absyn_Bitlrshift: _LL203: _LL204:
 return 80;case Cyc_Absyn_Bitarshift: _LL205: _LL206:
 return 80;default: _LL207: _LL208:
 return 140;}_LL1E0:;}case 3: _LL196: _LL197:
# 1046
 return 20;case 4: _LL198: _LL199:
 return 130;case 5: _LL19A: _LL19B:
 return 30;case 6: _LL19C: _LL19D:
 return 35;case 7: _LL19E: _LL19F:
 return 30;case 8: _LL1A0: _LL1A1:
 return 10;case 9: _LL1A2: _LL1A3:
 return 140;case 10: _LL1A4: _LL1A5:
 return 130;case 11: _LL1A6: _tmp1FD=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp1FB)->f1;_LL1A7:
 return Cyc_Absynpp_exp_prec(_tmp1FD);case 12: _LL1A8: _tmp1FC=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp1FB)->f1;_LL1A9:
 return Cyc_Absynpp_exp_prec(_tmp1FC);case 13: _LL1AA: _LL1AB:
 return 120;case 15: _LL1AC: _LL1AD:
 return 15;case 14: _LL1AE: _LL1AF:
 goto _LL1B1;case 16: _LL1B0: _LL1B1:
 goto _LL1B3;case 17: _LL1B2: _LL1B3:
 goto _LL1B5;case 38: _LL1B4: _LL1B5:
 goto _LL1B7;case 39: _LL1B6: _LL1B7:
 goto _LL1B9;case 37: _LL1B8: _LL1B9:
 goto _LL1BB;case 18: _LL1BA: _LL1BB:
 goto _LL1BD;case 19: _LL1BC: _LL1BD:
 return 130;case 20: _LL1BE: _LL1BF:
 goto _LL1C1;case 21: _LL1C0: _LL1C1:
 goto _LL1C3;case 22: _LL1C2: _LL1C3:
 return 140;case 23: _LL1C4: _LL1C5:
 return 150;case 24: _LL1C6: _LL1C7:
 goto _LL1C9;case 25: _LL1C8: _LL1C9:
 goto _LL1CB;case 26: _LL1CA: _LL1CB:
 goto _LL1CD;case 27: _LL1CC: _LL1CD:
 goto _LL1CF;case 28: _LL1CE: _LL1CF:
 goto _LL1D1;case 29: _LL1D0: _LL1D1:
 goto _LL1D3;case 30: _LL1D2: _LL1D3:
 goto _LL1D5;case 31: _LL1D4: _LL1D5:
 goto _LL1D7;case 32: _LL1D6: _LL1D7:
 goto _LL1D9;case 33: _LL1D8: _LL1D9:
 goto _LL1DB;case 34: _LL1DA: _LL1DB:
 goto _LL1DD;case 35: _LL1DC: _LL1DD:
 return 140;default: _LL1DE: _LL1DF:
 return 10000;}_LL18F:;}
# 1086
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1090
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp200=e->r;void*_tmp201=_tmp200;struct Cyc_Absyn_Stmt*_tmp202;struct Cyc_Core_Opt*_tmp203;struct Cyc_List_List*_tmp204;struct Cyc_Absyn_Exp*_tmp205;struct Cyc_Absyn_Exp*_tmp206;int _tmp207;struct Cyc_Absyn_Exp*_tmp208;void**_tmp209;struct Cyc_Absyn_Exp*_tmp20A;int _tmp20B;struct _tuple0*_tmp20C;struct _tuple0*_tmp20D;struct Cyc_List_List*_tmp20E;struct Cyc_Absyn_Datatypefield*_tmp20F;struct Cyc_List_List*_tmp210;struct _tuple0*_tmp211;struct Cyc_List_List*_tmp212;struct Cyc_List_List*_tmp213;struct Cyc_Absyn_Exp*_tmp214;void*_tmp215;struct Cyc_Absyn_Vardecl*_tmp216;struct Cyc_Absyn_Exp*_tmp217;struct Cyc_Absyn_Exp*_tmp218;struct Cyc_List_List*_tmp219;struct _tuple8*_tmp21A;struct Cyc_List_List*_tmp21B;struct Cyc_List_List*_tmp21C;struct Cyc_Absyn_Exp*_tmp21D;struct Cyc_Absyn_Exp*_tmp21E;struct Cyc_Absyn_Exp*_tmp21F;struct _dyneither_ptr*_tmp220;struct Cyc_Absyn_Exp*_tmp221;struct _dyneither_ptr*_tmp222;struct Cyc_Absyn_Exp*_tmp223;void*_tmp224;struct Cyc_List_List*_tmp225;struct Cyc_Absyn_Exp*_tmp226;struct _dyneither_ptr*_tmp227;int _tmp228;struct _dyneither_ptr _tmp229;void*_tmp22A;struct Cyc_Absyn_Exp*_tmp22B;void*_tmp22C;struct Cyc_Absyn_Exp*_tmp22D;struct Cyc_Absyn_Exp*_tmp22E;struct Cyc_Absyn_Exp*_tmp22F;void*_tmp230;struct Cyc_Absyn_Exp*_tmp231;struct Cyc_Absyn_Exp*_tmp232;struct Cyc_Absyn_Exp*_tmp233;struct Cyc_Absyn_Exp*_tmp234;struct Cyc_Absyn_Exp*_tmp235;struct Cyc_List_List*_tmp236;struct Cyc_Absyn_Exp*_tmp237;struct Cyc_Absyn_Exp*_tmp238;struct Cyc_Absyn_Exp*_tmp239;struct Cyc_Absyn_Exp*_tmp23A;struct Cyc_Absyn_Exp*_tmp23B;struct Cyc_Absyn_Exp*_tmp23C;struct Cyc_Absyn_Exp*_tmp23D;struct Cyc_Absyn_Exp*_tmp23E;struct Cyc_Absyn_Exp*_tmp23F;struct Cyc_Absyn_Exp*_tmp240;enum Cyc_Absyn_Incrementor _tmp241;struct Cyc_Absyn_Exp*_tmp242;struct Cyc_Core_Opt*_tmp243;struct Cyc_Absyn_Exp*_tmp244;enum Cyc_Absyn_Primop _tmp245;struct Cyc_List_List*_tmp246;struct _tuple0*_tmp247;union Cyc_Absyn_Cnst _tmp248;switch(*((int*)_tmp201)){case 0: _LL20A: _tmp248=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL20B:
 s=Cyc_Absynpp_cnst2doc(_tmp248);goto _LL209;case 1: _LL20C: _tmp247=((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL20D:
 s=Cyc_Absynpp_qvar2doc(_tmp247);goto _LL209;case 2: _LL20E: _tmp245=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp246=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL20F:
 s=Cyc_Absynpp_primapp2doc(myprec,_tmp245,_tmp246);goto _LL209;case 3: _LL210: _tmp242=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp243=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp244=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL211:
# 1098
{const char*_tmp6D1;const char*_tmp6D0;struct Cyc_PP_Doc*_tmp6CF[5];s=((_tmp6CF[4]=
# 1102
Cyc_Absynpp_exp2doc_prec(myprec,_tmp244),((_tmp6CF[3]=
# 1101
Cyc_PP_text(((_tmp6D0="= ",_tag_dyneither(_tmp6D0,sizeof(char),3)))),((_tmp6CF[2]=
# 1100
_tmp243 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp243->v),((_tmp6CF[1]=
# 1099
Cyc_PP_text(((_tmp6D1=" ",_tag_dyneither(_tmp6D1,sizeof(char),2)))),((_tmp6CF[0]=
# 1098
Cyc_Absynpp_exp2doc_prec(myprec,_tmp242),Cyc_PP_cat(_tag_dyneither(_tmp6CF,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1103
goto _LL209;case 4: _LL212: _tmp240=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp241=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL213: {
# 1105
struct Cyc_PP_Doc*_tmp24C=Cyc_Absynpp_exp2doc_prec(myprec,_tmp240);
{enum Cyc_Absyn_Incrementor _tmp24D=_tmp241;switch(_tmp24D){case Cyc_Absyn_PreInc: _LL25B: _LL25C:
{const char*_tmp6D4;struct Cyc_PP_Doc*_tmp6D3[2];s=((_tmp6D3[1]=_tmp24C,((_tmp6D3[0]=Cyc_PP_text(((_tmp6D4="++",_tag_dyneither(_tmp6D4,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp6D3,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL25A;case Cyc_Absyn_PreDec: _LL25D: _LL25E:
{const char*_tmp6D7;struct Cyc_PP_Doc*_tmp6D6[2];s=((_tmp6D6[1]=_tmp24C,((_tmp6D6[0]=Cyc_PP_text(((_tmp6D7="--",_tag_dyneither(_tmp6D7,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp6D6,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL25A;case Cyc_Absyn_PostInc: _LL25F: _LL260:
{const char*_tmp6DA;struct Cyc_PP_Doc*_tmp6D9[2];s=((_tmp6D9[1]=Cyc_PP_text(((_tmp6DA="++",_tag_dyneither(_tmp6DA,sizeof(char),3)))),((_tmp6D9[0]=_tmp24C,Cyc_PP_cat(_tag_dyneither(_tmp6D9,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL25A;default: _LL261: _LL262:
{const char*_tmp6DD;struct Cyc_PP_Doc*_tmp6DC[2];s=((_tmp6DC[1]=Cyc_PP_text(((_tmp6DD="--",_tag_dyneither(_tmp6DD,sizeof(char),3)))),((_tmp6DC[0]=_tmp24C,Cyc_PP_cat(_tag_dyneither(_tmp6DC,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL25A;}_LL25A:;}
# 1112
goto _LL209;}case 5: _LL214: _tmp23D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp23E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp23F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL215:
# 1114
{const char*_tmp6E2;const char*_tmp6E1;struct Cyc_PP_Doc*_tmp6E0[5];s=((_tmp6E0[4]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp23F),((_tmp6E0[3]=Cyc_PP_text(((_tmp6E1=" : ",_tag_dyneither(_tmp6E1,sizeof(char),4)))),((_tmp6E0[2]=
# 1114
Cyc_Absynpp_exp2doc_prec(0,_tmp23E),((_tmp6E0[1]=Cyc_PP_text(((_tmp6E2=" ? ",_tag_dyneither(_tmp6E2,sizeof(char),4)))),((_tmp6E0[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp23D),Cyc_PP_cat(_tag_dyneither(_tmp6E0,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1116
goto _LL209;case 6: _LL216: _tmp23B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp23C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL217:
# 1118
{const char*_tmp6E5;struct Cyc_PP_Doc*_tmp6E4[3];s=((_tmp6E4[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp23C),((_tmp6E4[1]=Cyc_PP_text(((_tmp6E5=" && ",_tag_dyneither(_tmp6E5,sizeof(char),5)))),((_tmp6E4[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp23B),Cyc_PP_cat(_tag_dyneither(_tmp6E4,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL209;case 7: _LL218: _tmp239=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp23A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL219:
# 1121
{const char*_tmp6E8;struct Cyc_PP_Doc*_tmp6E7[3];s=((_tmp6E7[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp23A),((_tmp6E7[1]=Cyc_PP_text(((_tmp6E8=" || ",_tag_dyneither(_tmp6E8,sizeof(char),5)))),((_tmp6E7[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp239),Cyc_PP_cat(_tag_dyneither(_tmp6E7,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL209;case 8: _LL21A: _tmp237=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp238=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL21B:
# 1126
{const char*_tmp6EF;const char*_tmp6EE;const char*_tmp6ED;struct Cyc_PP_Doc*_tmp6EC[5];s=((_tmp6EC[4]=Cyc_PP_text(((_tmp6ED=")",_tag_dyneither(_tmp6ED,sizeof(char),2)))),((_tmp6EC[3]=Cyc_Absynpp_exp2doc(_tmp238),((_tmp6EC[2]=Cyc_PP_text(((_tmp6EE=", ",_tag_dyneither(_tmp6EE,sizeof(char),3)))),((_tmp6EC[1]=Cyc_Absynpp_exp2doc(_tmp237),((_tmp6EC[0]=Cyc_PP_text(((_tmp6EF="(",_tag_dyneither(_tmp6EF,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6EC,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL209;case 9: _LL21C: _tmp235=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp236=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL21D:
# 1129
{const char*_tmp6F4;const char*_tmp6F3;struct Cyc_PP_Doc*_tmp6F2[4];s=((_tmp6F2[3]=
# 1132
Cyc_PP_text(((_tmp6F3=")",_tag_dyneither(_tmp6F3,sizeof(char),2)))),((_tmp6F2[2]=
# 1131
Cyc_Absynpp_exps2doc_prec(20,_tmp236),((_tmp6F2[1]=
# 1130
Cyc_PP_text(((_tmp6F4="(",_tag_dyneither(_tmp6F4,sizeof(char),2)))),((_tmp6F2[0]=
# 1129
Cyc_Absynpp_exp2doc_prec(myprec,_tmp235),Cyc_PP_cat(_tag_dyneither(_tmp6F2,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1133
goto _LL209;case 10: _LL21E: _tmp234=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL21F:
# 1135
{const char*_tmp6F7;struct Cyc_PP_Doc*_tmp6F6[2];s=((_tmp6F6[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp234),((_tmp6F6[0]=Cyc_PP_text(((_tmp6F7="throw ",_tag_dyneither(_tmp6F7,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp6F6,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL209;case 11: _LL220: _tmp233=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL221:
# 1138
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp233);
goto _LL209;case 12: _LL222: _tmp232=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL223:
# 1141
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp232);
goto _LL209;case 13: _LL224: _tmp230=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp231=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL225:
# 1144
{const char*_tmp6FC;const char*_tmp6FB;struct Cyc_PP_Doc*_tmp6FA[4];s=((_tmp6FA[3]=
# 1147
Cyc_Absynpp_exp2doc_prec(myprec,_tmp231),((_tmp6FA[2]=
# 1146
Cyc_PP_text(((_tmp6FB=")",_tag_dyneither(_tmp6FB,sizeof(char),2)))),((_tmp6FA[1]=
# 1145
Cyc_Absynpp_typ2doc(_tmp230),((_tmp6FA[0]=
# 1144
Cyc_PP_text(((_tmp6FC="(",_tag_dyneither(_tmp6FC,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6FA,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1148
goto _LL209;case 14: _LL226: _tmp22F=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL227:
# 1150
{const char*_tmp6FF;struct Cyc_PP_Doc*_tmp6FE[2];s=((_tmp6FE[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp22F),((_tmp6FE[0]=
# 1150
Cyc_PP_text(((_tmp6FF="&",_tag_dyneither(_tmp6FF,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6FE,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1152
goto _LL209;case 15: _LL228: _tmp22D=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp22E=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL229:
# 1154
 if(_tmp22D == 0){
const char*_tmp702;struct Cyc_PP_Doc*_tmp701[2];s=((_tmp701[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp22E),((_tmp701[0]=Cyc_PP_text(((_tmp702="new ",_tag_dyneither(_tmp702,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp701,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 1157
const char*_tmp707;const char*_tmp706;struct Cyc_PP_Doc*_tmp705[4];s=((_tmp705[3]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp22E),((_tmp705[2]=
# 1157
Cyc_PP_text(((_tmp706=") ",_tag_dyneither(_tmp706,sizeof(char),3)))),((_tmp705[1]=Cyc_Absynpp_exp2doc(_tmp22D),((_tmp705[0]=Cyc_PP_text(((_tmp707="rnew(",_tag_dyneither(_tmp707,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp705,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1159
goto _LL209;case 16: _LL22A: _tmp22C=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL22B:
{const char*_tmp70C;const char*_tmp70B;struct Cyc_PP_Doc*_tmp70A[3];s=((_tmp70A[2]=Cyc_PP_text(((_tmp70B=")",_tag_dyneither(_tmp70B,sizeof(char),2)))),((_tmp70A[1]=Cyc_Absynpp_typ2doc(_tmp22C),((_tmp70A[0]=Cyc_PP_text(((_tmp70C="sizeof(",_tag_dyneither(_tmp70C,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp70A,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL209;case 17: _LL22C: _tmp22B=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL22D:
{const char*_tmp711;const char*_tmp710;struct Cyc_PP_Doc*_tmp70F[3];s=((_tmp70F[2]=Cyc_PP_text(((_tmp710=")",_tag_dyneither(_tmp710,sizeof(char),2)))),((_tmp70F[1]=Cyc_Absynpp_exp2doc(_tmp22B),((_tmp70F[0]=Cyc_PP_text(((_tmp711="sizeof(",_tag_dyneither(_tmp711,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp70F,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL209;case 38: _LL22E: _tmp22A=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL22F:
{const char*_tmp716;const char*_tmp715;struct Cyc_PP_Doc*_tmp714[3];s=((_tmp714[2]=Cyc_PP_text(((_tmp715=")",_tag_dyneither(_tmp715,sizeof(char),2)))),((_tmp714[1]=Cyc_Absynpp_typ2doc(_tmp22A),((_tmp714[0]=Cyc_PP_text(((_tmp716="valueof(",_tag_dyneither(_tmp716,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp714,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL209;case 39: _LL230: _tmp228=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp229=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL231:
# 1164
 if(_tmp228){
const char*_tmp71D;const char*_tmp71C;const char*_tmp71B;struct Cyc_PP_Doc*_tmp71A[4];s=((_tmp71A[3]=Cyc_PP_text(((_tmp71B=")",_tag_dyneither(_tmp71B,sizeof(char),2)))),((_tmp71A[2]=Cyc_PP_text(_tmp229),((_tmp71A[1]=Cyc_PP_text(((_tmp71C=" volatile (",_tag_dyneither(_tmp71C,sizeof(char),12)))),((_tmp71A[0]=Cyc_PP_text(((_tmp71D="__asm__",_tag_dyneither(_tmp71D,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp71A,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1167
const char*_tmp722;const char*_tmp721;struct Cyc_PP_Doc*_tmp720[3];s=((_tmp720[2]=Cyc_PP_text(((_tmp721=")",_tag_dyneither(_tmp721,sizeof(char),2)))),((_tmp720[1]=Cyc_PP_text(_tmp229),((_tmp720[0]=Cyc_PP_text(((_tmp722="__asm__(",_tag_dyneither(_tmp722,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp720,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL209;case 37: _LL232: _tmp226=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp227=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL233:
# 1170
{const char*_tmp729;const char*_tmp728;const char*_tmp727;struct Cyc_PP_Doc*_tmp726[5];s=((_tmp726[4]=Cyc_PP_text(((_tmp727=")",_tag_dyneither(_tmp727,sizeof(char),2)))),((_tmp726[3]=Cyc_PP_textptr(_tmp227),((_tmp726[2]=Cyc_PP_text(((_tmp728=".",_tag_dyneither(_tmp728,sizeof(char),2)))),((_tmp726[1]=Cyc_Absynpp_exp2doc(_tmp226),((_tmp726[0]=Cyc_PP_text(((_tmp729="tagcheck(",_tag_dyneither(_tmp729,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp726,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL209;case 18: _LL234: _tmp224=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp225=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL235:
# 1173
{const char*_tmp72E;const char*_tmp72D;struct Cyc_PP_Doc*_tmp72C[3];s=((_tmp72C[2]=Cyc_PP_text(((_tmp72D=",",_tag_dyneither(_tmp72D,sizeof(char),2)))),((_tmp72C[1]=Cyc_Absynpp_typ2doc(_tmp224),((_tmp72C[0]=Cyc_PP_text(((_tmp72E="offsetof(",_tag_dyneither(_tmp72E,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp72C,sizeof(struct Cyc_PP_Doc*),3))))))));}
for(0;_tmp225 != 0;_tmp225=_tmp225->tl){
void*_tmp287=(void*)_tmp225->hd;void*_tmp288=_tmp287;unsigned int _tmp289;struct _dyneither_ptr*_tmp28A;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp288)->tag == 0){_LL264: _tmp28A=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp288)->f1;_LL265:
{const char*_tmp731;struct Cyc_PP_Doc*_tmp730[3];s=((_tmp730[2]=
_tmp225->tl != 0?Cyc_PP_text(((_tmp731=".",_tag_dyneither(_tmp731,sizeof(char),2)))): Cyc_PP_nil_doc(),((_tmp730[1]=
# 1176
Cyc_PP_textptr(_tmp28A),((_tmp730[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp730,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1178
goto _LL263;}else{_LL266: _tmp289=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp288)->f1;_LL267:
{struct Cyc_Int_pa_PrintArg_struct _tmp73B;void*_tmp73A[1];const char*_tmp739;const char*_tmp738;struct Cyc_PP_Doc*_tmp737[3];s=((_tmp737[2]=
_tmp225->tl != 0?Cyc_PP_text(((_tmp738=".",_tag_dyneither(_tmp738,sizeof(char),2)))): Cyc_PP_nil_doc(),((_tmp737[1]=
# 1179
Cyc_PP_text((struct _dyneither_ptr)((_tmp73B.tag=1,((_tmp73B.f1=(unsigned long)((int)_tmp289),((_tmp73A[0]=& _tmp73B,Cyc_aprintf(((_tmp739="%d",_tag_dyneither(_tmp739,sizeof(char),3))),_tag_dyneither(_tmp73A,sizeof(void*),1))))))))),((_tmp737[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp737,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1181
goto _LL263;}_LL263:;}
# 1183
{const char*_tmp73E;struct Cyc_PP_Doc*_tmp73D[2];s=((_tmp73D[1]=Cyc_PP_text(((_tmp73E=")",_tag_dyneither(_tmp73E,sizeof(char),2)))),((_tmp73D[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp73D,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL209;case 19: _LL236: _tmp223=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL237:
# 1186
{const char*_tmp741;struct Cyc_PP_Doc*_tmp740[2];s=((_tmp740[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp223),((_tmp740[0]=Cyc_PP_text(((_tmp741="*",_tag_dyneither(_tmp741,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp740,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL209;case 20: _LL238: _tmp221=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp222=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL239:
# 1189
{const char*_tmp744;struct Cyc_PP_Doc*_tmp743[3];s=((_tmp743[2]=Cyc_PP_textptr(_tmp222),((_tmp743[1]=Cyc_PP_text(((_tmp744=".",_tag_dyneither(_tmp744,sizeof(char),2)))),((_tmp743[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp221),Cyc_PP_cat(_tag_dyneither(_tmp743,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL209;case 21: _LL23A: _tmp21F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp220=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL23B:
# 1192
{const char*_tmp747;struct Cyc_PP_Doc*_tmp746[3];s=((_tmp746[2]=Cyc_PP_textptr(_tmp220),((_tmp746[1]=Cyc_PP_text(((_tmp747="->",_tag_dyneither(_tmp747,sizeof(char),3)))),((_tmp746[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp21F),Cyc_PP_cat(_tag_dyneither(_tmp746,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL209;case 22: _LL23C: _tmp21D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp21E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL23D:
# 1195
{const char*_tmp74C;const char*_tmp74B;struct Cyc_PP_Doc*_tmp74A[4];s=((_tmp74A[3]=
# 1198
Cyc_PP_text(((_tmp74B="]",_tag_dyneither(_tmp74B,sizeof(char),2)))),((_tmp74A[2]=
# 1197
Cyc_Absynpp_exp2doc(_tmp21E),((_tmp74A[1]=
# 1196
Cyc_PP_text(((_tmp74C="[",_tag_dyneither(_tmp74C,sizeof(char),2)))),((_tmp74A[0]=
# 1195
Cyc_Absynpp_exp2doc_prec(myprec,_tmp21D),Cyc_PP_cat(_tag_dyneither(_tmp74A,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1199
goto _LL209;case 23: _LL23E: _tmp21C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL23F:
# 1201
{const char*_tmp751;const char*_tmp750;struct Cyc_PP_Doc*_tmp74F[4];s=((_tmp74F[3]=
# 1204
Cyc_PP_text(((_tmp750=")",_tag_dyneither(_tmp750,sizeof(char),2)))),((_tmp74F[2]=
# 1203
Cyc_Absynpp_exps2doc_prec(20,_tmp21C),((_tmp74F[1]=
# 1202
Cyc_PP_text(((_tmp751="(",_tag_dyneither(_tmp751,sizeof(char),2)))),((_tmp74F[0]=
# 1201
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp74F,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1205
goto _LL209;case 24: _LL240: _tmp21A=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp21B=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL241:
# 1207
{const char*_tmp758;const char*_tmp757;const char*_tmp756;struct Cyc_PP_Doc*_tmp755[4];s=((_tmp755[3]=
# 1210
Cyc_Absynpp_group_braces(((_tmp756=",",_tag_dyneither(_tmp756,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp21B)),((_tmp755[2]=
# 1209
Cyc_PP_text(((_tmp757=")",_tag_dyneither(_tmp757,sizeof(char),2)))),((_tmp755[1]=
# 1208
Cyc_Absynpp_typ2doc((*_tmp21A).f3),((_tmp755[0]=
# 1207
Cyc_PP_text(((_tmp758="(",_tag_dyneither(_tmp758,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp755,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1211
goto _LL209;case 25: _LL242: _tmp219=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL243:
# 1213
{const char*_tmp759;s=Cyc_Absynpp_group_braces(((_tmp759=",",_tag_dyneither(_tmp759,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp219));}
goto _LL209;case 26: _LL244: _tmp216=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp217=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp218=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL245:
# 1216
{const char*_tmp760;const char*_tmp75F;const char*_tmp75E;struct Cyc_PP_Doc*_tmp75D[8];s=((_tmp75D[7]=
# 1222
Cyc_Absynpp_rb(),((_tmp75D[6]=
# 1221
Cyc_Absynpp_exp2doc(_tmp218),((_tmp75D[5]=
# 1220
Cyc_PP_text(((_tmp75E=" : ",_tag_dyneither(_tmp75E,sizeof(char),4)))),((_tmp75D[4]=
# 1219
Cyc_Absynpp_exp2doc(_tmp217),((_tmp75D[3]=
# 1218
Cyc_PP_text(((_tmp75F=" < ",_tag_dyneither(_tmp75F,sizeof(char),4)))),((_tmp75D[2]=
# 1217
Cyc_PP_text(*(*_tmp216->name).f2),((_tmp75D[1]=
# 1216
Cyc_PP_text(((_tmp760="for ",_tag_dyneither(_tmp760,sizeof(char),5)))),((_tmp75D[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp75D,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1223
goto _LL209;case 27: _LL246: _tmp214=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp215=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL247:
# 1225
{const char*_tmp767;const char*_tmp766;const char*_tmp765;struct Cyc_PP_Doc*_tmp764[7];s=((_tmp764[6]=
# 1230
Cyc_Absynpp_rb(),((_tmp764[5]=
# 1229
Cyc_Absynpp_typ2doc(_tmp215),((_tmp764[4]=
# 1228
Cyc_PP_text(((_tmp765=" : ",_tag_dyneither(_tmp765,sizeof(char),4)))),((_tmp764[3]=
# 1227
Cyc_Absynpp_exp2doc(_tmp214),((_tmp764[2]=
# 1226
Cyc_PP_text(((_tmp766=" < ",_tag_dyneither(_tmp766,sizeof(char),4)))),((_tmp764[1]=
# 1225
Cyc_PP_text(((_tmp767="for x ",_tag_dyneither(_tmp767,sizeof(char),7)))),((_tmp764[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp764,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1231
goto _LL209;case 28: _LL248: _tmp211=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp212=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp213=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL249: {
# 1233
struct Cyc_List_List*_tmp2AD=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp213);
{const char*_tmp76C;struct Cyc_List_List*_tmp76B;struct Cyc_PP_Doc*_tmp76A[2];s=((_tmp76A[1]=
Cyc_Absynpp_group_braces(((_tmp76C=",",_tag_dyneither(_tmp76C,sizeof(char),2))),
_tmp212 != 0?(_tmp76B=_cycalloc(sizeof(*_tmp76B)),((_tmp76B->hd=Cyc_Absynpp_tps2doc(_tmp212),((_tmp76B->tl=_tmp2AD,_tmp76B))))): _tmp2AD),((_tmp76A[0]=
# 1234
Cyc_Absynpp_qvar2doc(_tmp211),Cyc_PP_cat(_tag_dyneither(_tmp76A,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1237
goto _LL209;}case 29: _LL24A: _tmp210=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL24B:
# 1239
{const char*_tmp76D;s=Cyc_Absynpp_group_braces(((_tmp76D=",",_tag_dyneither(_tmp76D,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp210));}
goto _LL209;case 30: _LL24C: _tmp20E=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp20F=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL24D:
# 1242
 if(_tmp20E == 0)
# 1244
s=Cyc_Absynpp_qvar2doc(_tmp20F->name);else{
# 1246
const char*_tmp774;const char*_tmp773;const char*_tmp772;struct Cyc_PP_Doc*_tmp771[2];s=((_tmp771[1]=
Cyc_PP_egroup(((_tmp774="(",_tag_dyneither(_tmp774,sizeof(char),2))),((_tmp773=")",_tag_dyneither(_tmp773,sizeof(char),2))),((_tmp772=",",_tag_dyneither(_tmp772,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp20E)),((_tmp771[0]=
# 1246
Cyc_Absynpp_qvar2doc(_tmp20F->name),Cyc_PP_cat(_tag_dyneither(_tmp771,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1248
goto _LL209;case 31: _LL24E: _tmp20D=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL24F:
# 1250
 s=Cyc_Absynpp_qvar2doc(_tmp20D);
goto _LL209;case 32: _LL250: _tmp20C=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL251:
# 1253
 s=Cyc_Absynpp_qvar2doc(_tmp20C);
goto _LL209;case 33: _LL252: _tmp207=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).is_calloc;_tmp208=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).rgn;_tmp209=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).elt_type;_tmp20A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).num_elts;_tmp20B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).inline_call;_LL253:
# 1258
 if(_tmp207){
# 1260
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp209)),0);
if(_tmp208 == 0){
const char*_tmp77B;const char*_tmp77A;const char*_tmp779;struct Cyc_PP_Doc*_tmp778[5];s=((_tmp778[4]=Cyc_PP_text(((_tmp779=")",_tag_dyneither(_tmp779,sizeof(char),2)))),((_tmp778[3]=Cyc_Absynpp_exp2doc(st),((_tmp778[2]=Cyc_PP_text(((_tmp77A=",",_tag_dyneither(_tmp77A,sizeof(char),2)))),((_tmp778[1]=Cyc_Absynpp_exp2doc(_tmp20A),((_tmp778[0]=Cyc_PP_text(((_tmp77B="calloc(",_tag_dyneither(_tmp77B,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp778,sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{
# 1264
const char*_tmp784;const char*_tmp783;const char*_tmp782;const char*_tmp781;struct Cyc_PP_Doc*_tmp780[7];s=((_tmp780[6]=
Cyc_PP_text(((_tmp781=")",_tag_dyneither(_tmp781,sizeof(char),2)))),((_tmp780[5]=Cyc_Absynpp_exp2doc(st),((_tmp780[4]=Cyc_PP_text(((_tmp782=",",_tag_dyneither(_tmp782,sizeof(char),2)))),((_tmp780[3]=Cyc_Absynpp_exp2doc(_tmp20A),((_tmp780[2]=
# 1264
Cyc_PP_text(((_tmp783=",",_tag_dyneither(_tmp783,sizeof(char),2)))),((_tmp780[1]=Cyc_Absynpp_exp2doc(_tmp208),((_tmp780[0]=Cyc_PP_text(((_tmp784="rcalloc(",_tag_dyneither(_tmp784,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp780,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1267
struct Cyc_Absyn_Exp*new_e;
# 1269
if(_tmp209 == 0)
new_e=_tmp20A;else{
# 1272
new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp209,0),_tmp20A,0);}
# 1274
if(_tmp208 == 0){
const char*_tmp789;const char*_tmp788;struct Cyc_PP_Doc*_tmp787[3];s=((_tmp787[2]=Cyc_PP_text(((_tmp788=")",_tag_dyneither(_tmp788,sizeof(char),2)))),((_tmp787[1]=Cyc_Absynpp_exp2doc(new_e),((_tmp787[0]=Cyc_PP_text(((_tmp789="malloc(",_tag_dyneither(_tmp789,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp787,sizeof(struct Cyc_PP_Doc*),3))))))));}else{
# 1277
if(_tmp20B){
const char*_tmp790;const char*_tmp78F;const char*_tmp78E;struct Cyc_PP_Doc*_tmp78D[5];s=((_tmp78D[4]=
Cyc_PP_text(((_tmp78E=")",_tag_dyneither(_tmp78E,sizeof(char),2)))),((_tmp78D[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp78D[2]=
# 1278
Cyc_PP_text(((_tmp78F=",",_tag_dyneither(_tmp78F,sizeof(char),2)))),((_tmp78D[1]=Cyc_Absynpp_exp2doc(_tmp208),((_tmp78D[0]=Cyc_PP_text(((_tmp790="rmalloc_inline(",_tag_dyneither(_tmp790,sizeof(char),16)))),Cyc_PP_cat(_tag_dyneither(_tmp78D,sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{
# 1281
const char*_tmp797;const char*_tmp796;const char*_tmp795;struct Cyc_PP_Doc*_tmp794[5];s=((_tmp794[4]=
Cyc_PP_text(((_tmp795=")",_tag_dyneither(_tmp795,sizeof(char),2)))),((_tmp794[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp794[2]=
# 1281
Cyc_PP_text(((_tmp796=",",_tag_dyneither(_tmp796,sizeof(char),2)))),((_tmp794[1]=Cyc_Absynpp_exp2doc(_tmp208),((_tmp794[0]=Cyc_PP_text(((_tmp797="rmalloc(",_tag_dyneither(_tmp797,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp794,sizeof(struct Cyc_PP_Doc*),5))))))))))));}}}
# 1285
goto _LL209;case 34: _LL254: _tmp205=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp206=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL255:
# 1287
{const char*_tmp79A;struct Cyc_PP_Doc*_tmp799[3];s=((_tmp799[2]=
# 1289
Cyc_Absynpp_exp2doc_prec(myprec,_tmp206),((_tmp799[1]=
# 1288
Cyc_PP_text(((_tmp79A=" :=: ",_tag_dyneither(_tmp79A,sizeof(char),6)))),((_tmp799[0]=
# 1287
Cyc_Absynpp_exp2doc_prec(myprec,_tmp205),Cyc_PP_cat(_tag_dyneither(_tmp799,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1290
goto _LL209;case 35: _LL256: _tmp203=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp204=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL257:
# 1293
{const char*_tmp79B;s=Cyc_Absynpp_group_braces(((_tmp79B=",",_tag_dyneither(_tmp79B,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp204));}
goto _LL209;default: _LL258: _tmp202=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL259:
# 1296
{const char*_tmp7A0;const char*_tmp79F;struct Cyc_PP_Doc*_tmp79E[7];s=((_tmp79E[6]=
# 1298
Cyc_PP_text(((_tmp79F=")",_tag_dyneither(_tmp79F,sizeof(char),2)))),((_tmp79E[5]=Cyc_Absynpp_rb(),((_tmp79E[4]=Cyc_PP_blank_doc(),((_tmp79E[3]=
# 1297
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp202,1)),((_tmp79E[2]=
# 1296
Cyc_PP_blank_doc(),((_tmp79E[1]=Cyc_Absynpp_lb(),((_tmp79E[0]=Cyc_PP_text(((_tmp7A0="(",_tag_dyneither(_tmp7A0,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp79E,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1299
goto _LL209;}_LL209:;}
# 1301
if(inprec >= myprec){
const char*_tmp7A5;const char*_tmp7A4;struct Cyc_PP_Doc*_tmp7A3[3];s=((_tmp7A3[2]=Cyc_PP_text(((_tmp7A4=")",_tag_dyneither(_tmp7A4,sizeof(char),2)))),((_tmp7A3[1]=s,((_tmp7A3[0]=Cyc_PP_text(((_tmp7A5="(",_tag_dyneither(_tmp7A5,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7A3,sizeof(struct Cyc_PP_Doc*),3))))))));}
return s;}
# 1306
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp2D3=d;struct _dyneither_ptr*_tmp2D4;struct Cyc_Absyn_Exp*_tmp2D5;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2D3)->tag == 0){_LL269: _tmp2D5=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2D3)->f1;_LL26A: {
const char*_tmp7AA;const char*_tmp7A9;struct Cyc_PP_Doc*_tmp7A8[3];return(_tmp7A8[2]=Cyc_PP_text(((_tmp7A9="]",_tag_dyneither(_tmp7A9,sizeof(char),2)))),((_tmp7A8[1]=Cyc_Absynpp_exp2doc(_tmp2D5),((_tmp7A8[0]=Cyc_PP_text(((_tmp7AA=".[",_tag_dyneither(_tmp7AA,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp7A8,sizeof(struct Cyc_PP_Doc*),3)))))));}}else{_LL26B: _tmp2D4=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2D3)->f1;_LL26C: {
const char*_tmp7AD;struct Cyc_PP_Doc*_tmp7AC[2];return(_tmp7AC[1]=Cyc_PP_textptr(_tmp2D4),((_tmp7AC[0]=Cyc_PP_text(((_tmp7AD=".",_tag_dyneither(_tmp7AD,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7AC,sizeof(struct Cyc_PP_Doc*),2)))));}}_LL268:;}
# 1313
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
const char*_tmp7B4;const char*_tmp7B3;const char*_tmp7B2;struct Cyc_PP_Doc*_tmp7B1[2];return(_tmp7B1[1]=
Cyc_Absynpp_exp2doc((*de).f2),((_tmp7B1[0]=
# 1315
Cyc_PP_egroup(((_tmp7B4="",_tag_dyneither(_tmp7B4,sizeof(char),1))),((_tmp7B3="=",_tag_dyneither(_tmp7B3,sizeof(char),2))),((_tmp7B2="=",_tag_dyneither(_tmp7B2,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp7B1,sizeof(struct Cyc_PP_Doc*),2)))));}}
# 1319
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
const char*_tmp7B7;const char*_tmp7B6;const char*_tmp7B5;return Cyc_PP_group(((_tmp7B5="",_tag_dyneither(_tmp7B5,sizeof(char),1))),((_tmp7B6="",_tag_dyneither(_tmp7B6,sizeof(char),1))),((_tmp7B7=",",_tag_dyneither(_tmp7B7,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long i);static void _tmp7BC(unsigned int*_tmp7BB,unsigned int*_tmp7BA,char**_tmp7B9){for(*_tmp7BB=0;*_tmp7BB < *_tmp7BA;(*_tmp7BB)++){(*_tmp7B9)[*_tmp7BB]=(char)'z';}}
# 1323
struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
char*_tmp7C1;unsigned int _tmp7C0;char*_tmp7BF;unsigned int _tmp7BE;unsigned int _tmp7BD;struct _dyneither_ptr x=(_tmp7C1=((_tmp7C0=(unsigned int)28,((_tmp7BF=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp7C0 + 1)),((((_tmp7BE=_tmp7C0,((_tmp7BC(& _tmp7BD,& _tmp7BE,& _tmp7BF),_tmp7BF[_tmp7BE]=(char)0)))),_tmp7BF)))))),_tag_dyneither(_tmp7C1,sizeof(char),_get_zero_arr_size_char((void*)_tmp7C1,(unsigned int)28 + 1)));
{char _tmp7C4;char _tmp7C3;struct _dyneither_ptr _tmp7C2;(_tmp7C2=_dyneither_ptr_plus(x,sizeof(char),27),((_tmp7C3=*((char*)_check_dyneither_subscript(_tmp7C2,sizeof(char),0)),((_tmp7C4='\000',((_get_dyneither_size(_tmp7C2,sizeof(char))== 1  && (_tmp7C3 == '\000'  && _tmp7C4 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp7C2.curr)=_tmp7C4)))))));}
{char _tmp7C7;char _tmp7C6;struct _dyneither_ptr _tmp7C5;(_tmp7C5=_dyneither_ptr_plus(x,sizeof(char),26),((_tmp7C6=*((char*)_check_dyneither_subscript(_tmp7C5,sizeof(char),0)),((_tmp7C7='L',((_get_dyneither_size(_tmp7C5,sizeof(char))== 1  && (_tmp7C6 == '\000'  && _tmp7C7 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp7C5.curr)=_tmp7C7)))))));}
{char _tmp7CA;char _tmp7C9;struct _dyneither_ptr _tmp7C8;(_tmp7C8=_dyneither_ptr_plus(x,sizeof(char),25),((_tmp7C9=*((char*)_check_dyneither_subscript(_tmp7C8,sizeof(char),0)),((_tmp7CA='L',((_get_dyneither_size(_tmp7C8,sizeof(char))== 1  && (_tmp7C9 == '\000'  && _tmp7CA != '\000')?_throw_arraybounds(): 1,*((char*)_tmp7C8.curr)=_tmp7CA)))))));}
{char _tmp7CD;char _tmp7CC;struct _dyneither_ptr _tmp7CB;(_tmp7CB=_dyneither_ptr_plus(x,sizeof(char),24),((_tmp7CC=*((char*)_check_dyneither_subscript(_tmp7CB,sizeof(char),0)),((_tmp7CD='U',((_get_dyneither_size(_tmp7CB,sizeof(char))== 1  && (_tmp7CC == '\000'  && _tmp7CD != '\000')?_throw_arraybounds(): 1,*((char*)_tmp7CB.curr)=_tmp7CD)))))));}
{char _tmp7D0;char _tmp7CF;struct _dyneither_ptr _tmp7CE;(_tmp7CE=_dyneither_ptr_plus(x,sizeof(char),23),((_tmp7CF=*((char*)_check_dyneither_subscript(_tmp7CE,sizeof(char),0)),((_tmp7D0='0',((_get_dyneither_size(_tmp7CE,sizeof(char))== 1  && (_tmp7CF == '\000'  && _tmp7D0 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp7CE.curr)=_tmp7D0)))))));}{
int index=23;
while(i != 0){
char c=(char)('0' + i % 10);
{char _tmp7D3;char _tmp7D2;struct _dyneither_ptr _tmp7D1;(_tmp7D1=_dyneither_ptr_plus(x,sizeof(char),index),((_tmp7D2=*((char*)_check_dyneither_subscript(_tmp7D1,sizeof(char),0)),((_tmp7D3=c,((_get_dyneither_size(_tmp7D1,sizeof(char))== 1  && (_tmp7D2 == '\000'  && _tmp7D3 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp7D1.curr)=_tmp7D3)))))));}
i=i / 10;
-- index;}
# 1337
return(struct _dyneither_ptr)_dyneither_ptr_plus(_dyneither_ptr_plus(x,sizeof(char),index),sizeof(char),1);};}
# 1341
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp2F8=c;struct _dyneither_ptr _tmp2F9;struct _dyneither_ptr _tmp2FA;struct _dyneither_ptr _tmp2FB;enum Cyc_Absyn_Sign _tmp2FC;long long _tmp2FD;int _tmp2FE;int _tmp2FF;int _tmp300;enum Cyc_Absyn_Sign _tmp301;short _tmp302;struct _dyneither_ptr _tmp303;enum Cyc_Absyn_Sign _tmp304;char _tmp305;switch((_tmp2F8.String_c).tag){case 2: _LL26E: _tmp304=((_tmp2F8.Char_c).val).f1;_tmp305=((_tmp2F8.Char_c).val).f2;_LL26F: {
const char*_tmp7D7;void*_tmp7D6[1];struct Cyc_String_pa_PrintArg_struct _tmp7D5;return Cyc_PP_text((struct _dyneither_ptr)((_tmp7D5.tag=0,((_tmp7D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp305)),((_tmp7D6[0]=& _tmp7D5,Cyc_aprintf(((_tmp7D7="'%s'",_tag_dyneither(_tmp7D7,sizeof(char),5))),_tag_dyneither(_tmp7D6,sizeof(void*),1)))))))));}case 3: _LL270: _tmp303=(_tmp2F8.Wchar_c).val;_LL271: {
const char*_tmp7DB;void*_tmp7DA[1];struct Cyc_String_pa_PrintArg_struct _tmp7D9;return Cyc_PP_text((struct _dyneither_ptr)((_tmp7D9.tag=0,((_tmp7D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp303),((_tmp7DA[0]=& _tmp7D9,Cyc_aprintf(((_tmp7DB="L'%s'",_tag_dyneither(_tmp7DB,sizeof(char),6))),_tag_dyneither(_tmp7DA,sizeof(void*),1)))))))));}case 4: _LL272: _tmp301=((_tmp2F8.Short_c).val).f1;_tmp302=((_tmp2F8.Short_c).val).f2;_LL273: {
const char*_tmp7DF;void*_tmp7DE[1];struct Cyc_Int_pa_PrintArg_struct _tmp7DD;return Cyc_PP_text((struct _dyneither_ptr)((_tmp7DD.tag=1,((_tmp7DD.f1=(unsigned long)((int)_tmp302),((_tmp7DE[0]=& _tmp7DD,Cyc_aprintf(((_tmp7DF="%d",_tag_dyneither(_tmp7DF,sizeof(char),3))),_tag_dyneither(_tmp7DE,sizeof(void*),1)))))))));}case 5: switch(((_tmp2F8.Int_c).val).f1){case Cyc_Absyn_None: _LL274: _tmp300=((_tmp2F8.Int_c).val).f2;_LL275:
 _tmp2FF=_tmp300;goto _LL277;case Cyc_Absyn_Signed: _LL276: _tmp2FF=((_tmp2F8.Int_c).val).f2;_LL277: {
const char*_tmp7E3;void*_tmp7E2[1];struct Cyc_Int_pa_PrintArg_struct _tmp7E1;return Cyc_PP_text((struct _dyneither_ptr)((_tmp7E1.tag=1,((_tmp7E1.f1=(unsigned long)_tmp2FF,((_tmp7E2[0]=& _tmp7E1,Cyc_aprintf(((_tmp7E3="%d",_tag_dyneither(_tmp7E3,sizeof(char),3))),_tag_dyneither(_tmp7E2,sizeof(void*),1)))))))));}default: _LL278: _tmp2FE=((_tmp2F8.Int_c).val).f2;_LL279: {
const char*_tmp7E7;void*_tmp7E6[1];struct Cyc_Int_pa_PrintArg_struct _tmp7E5;return Cyc_PP_text((struct _dyneither_ptr)((_tmp7E5.tag=1,((_tmp7E5.f1=(unsigned int)_tmp2FE,((_tmp7E6[0]=& _tmp7E5,Cyc_aprintf(((_tmp7E7="%u",_tag_dyneither(_tmp7E7,sizeof(char),3))),_tag_dyneither(_tmp7E6,sizeof(void*),1)))))))));}}case 6: _LL27A: _tmp2FC=((_tmp2F8.LongLong_c).val).f1;_tmp2FD=((_tmp2F8.LongLong_c).val).f2;_LL27B:
# 1351
 return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)_tmp2FD));case 7: _LL27C: _tmp2FB=((_tmp2F8.Float_c).val).f1;_LL27D:
 return Cyc_PP_text(_tmp2FB);case 1: _LL27E: _LL27F: {
const char*_tmp7E8;return Cyc_PP_text(((_tmp7E8="NULL",_tag_dyneither(_tmp7E8,sizeof(char),5))));}case 8: _LL280: _tmp2FA=(_tmp2F8.String_c).val;_LL281: {
# 1355
const char*_tmp7ED;const char*_tmp7EC;struct Cyc_PP_Doc*_tmp7EB[3];return(_tmp7EB[2]=Cyc_PP_text(((_tmp7EC="\"",_tag_dyneither(_tmp7EC,sizeof(char),2)))),((_tmp7EB[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp2FA)),((_tmp7EB[0]=Cyc_PP_text(((_tmp7ED="\"",_tag_dyneither(_tmp7ED,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7EB,sizeof(struct Cyc_PP_Doc*),3)))))));}default: _LL282: _tmp2F9=(_tmp2F8.Wstring_c).val;_LL283: {
# 1357
const char*_tmp7F2;const char*_tmp7F1;struct Cyc_PP_Doc*_tmp7F0[3];return(_tmp7F0[2]=Cyc_PP_text(((_tmp7F1="\"",_tag_dyneither(_tmp7F1,sizeof(char),2)))),((_tmp7F0[1]=Cyc_PP_text(_tmp2F9),((_tmp7F0[0]=Cyc_PP_text(((_tmp7F2="L\"",_tag_dyneither(_tmp7F2,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp7F0,sizeof(struct Cyc_PP_Doc*),3)))))));}}_LL26D:;}
# 1361
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if(p == Cyc_Absyn_Numelts){
if(es == 0  || es->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp7FF;const char*_tmp7FE;void*_tmp7FD[1];struct Cyc_String_pa_PrintArg_struct _tmp7FC;struct Cyc_Core_Failure_exn_struct*_tmp7FB;(int)_throw((void*)((_tmp7FB=_cycalloc(sizeof(*_tmp7FB)),((_tmp7FB[0]=((_tmp7FF.tag=Cyc_Core_Failure,((_tmp7FF.f1=(struct _dyneither_ptr)((_tmp7FC.tag=0,((_tmp7FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp7FD[0]=& _tmp7FC,Cyc_aprintf(((_tmp7FE="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp7FE,sizeof(char),47))),_tag_dyneither(_tmp7FD,sizeof(void*),1)))))))),_tmp7FF)))),_tmp7FB)))));}{
# 1367
const char*_tmp804;const char*_tmp803;struct Cyc_PP_Doc*_tmp802[3];return(_tmp802[2]=Cyc_PP_text(((_tmp803=")",_tag_dyneither(_tmp803,sizeof(char),2)))),((_tmp802[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp802[0]=Cyc_PP_text(((_tmp804="numelts(",_tag_dyneither(_tmp804,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp802,sizeof(struct Cyc_PP_Doc*),3)))))));};}else{
# 1369
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0){
struct Cyc_Core_Failure_exn_struct _tmp811;const char*_tmp810;void*_tmp80F[1];struct Cyc_String_pa_PrintArg_struct _tmp80E;struct Cyc_Core_Failure_exn_struct*_tmp80D;(int)_throw((void*)((_tmp80D=_cycalloc(sizeof(*_tmp80D)),((_tmp80D[0]=((_tmp811.tag=Cyc_Core_Failure,((_tmp811.f1=(struct _dyneither_ptr)((_tmp80E.tag=0,((_tmp80E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp80F[0]=& _tmp80E,Cyc_aprintf(((_tmp810="Absynpp::primapp2doc: %s with no args",_tag_dyneither(_tmp810,sizeof(char),38))),_tag_dyneither(_tmp80F,sizeof(void*),1)))))))),_tmp811)))),_tmp80D)))));}else{
# 1373
if(ds->tl == 0){
const char*_tmp814;struct Cyc_PP_Doc*_tmp813[3];return(_tmp813[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp813[1]=Cyc_PP_text(((_tmp814=" ",_tag_dyneither(_tmp814,sizeof(char),2)))),((_tmp813[0]=ps,Cyc_PP_cat(_tag_dyneither(_tmp813,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp821;const char*_tmp820;void*_tmp81F[1];struct Cyc_String_pa_PrintArg_struct _tmp81E;struct Cyc_Core_Failure_exn_struct*_tmp81D;(int)_throw((void*)((_tmp81D=_cycalloc(sizeof(*_tmp81D)),((_tmp81D[0]=((_tmp821.tag=Cyc_Core_Failure,((_tmp821.f1=(struct _dyneither_ptr)((_tmp81E.tag=0,((_tmp81E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp81F[0]=& _tmp81E,Cyc_aprintf(((_tmp820="Absynpp::primapp2doc: %s with more than 2 args",_tag_dyneither(_tmp820,sizeof(char),47))),_tag_dyneither(_tmp81F,sizeof(void*),1)))))))),_tmp821)))),_tmp81D)))));}else{
# 1379
const char*_tmp826;const char*_tmp825;struct Cyc_PP_Doc*_tmp824[5];return(_tmp824[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd,((_tmp824[3]=Cyc_PP_text(((_tmp825=" ",_tag_dyneither(_tmp825,sizeof(char),2)))),((_tmp824[2]=ps,((_tmp824[1]=Cyc_PP_text(((_tmp826=" ",_tag_dyneither(_tmp826,sizeof(char),2)))),((_tmp824[0]=(struct Cyc_PP_Doc*)ds->hd,Cyc_PP_cat(_tag_dyneither(_tmp824,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
# 1383
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _tmp333=p;switch(_tmp333){case Cyc_Absyn_Plus: _LL285: _LL286: {
const char*_tmp827;return(_tmp827="+",_tag_dyneither(_tmp827,sizeof(char),2));}case Cyc_Absyn_Times: _LL287: _LL288: {
const char*_tmp828;return(_tmp828="*",_tag_dyneither(_tmp828,sizeof(char),2));}case Cyc_Absyn_Minus: _LL289: _LL28A: {
const char*_tmp829;return(_tmp829="-",_tag_dyneither(_tmp829,sizeof(char),2));}case Cyc_Absyn_Div: _LL28B: _LL28C: {
const char*_tmp82A;return(_tmp82A="/",_tag_dyneither(_tmp82A,sizeof(char),2));}case Cyc_Absyn_Mod: _LL28D: _LL28E: {
const char*_tmp82C;const char*_tmp82B;return Cyc_Absynpp_print_for_cycdoc?(_tmp82C="\\%",_tag_dyneither(_tmp82C,sizeof(char),3)):((_tmp82B="%",_tag_dyneither(_tmp82B,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL28F: _LL290: {
const char*_tmp82D;return(_tmp82D="==",_tag_dyneither(_tmp82D,sizeof(char),3));}case Cyc_Absyn_Neq: _LL291: _LL292: {
const char*_tmp82E;return(_tmp82E="!=",_tag_dyneither(_tmp82E,sizeof(char),3));}case Cyc_Absyn_Gt: _LL293: _LL294: {
const char*_tmp82F;return(_tmp82F=">",_tag_dyneither(_tmp82F,sizeof(char),2));}case Cyc_Absyn_Lt: _LL295: _LL296: {
const char*_tmp830;return(_tmp830="<",_tag_dyneither(_tmp830,sizeof(char),2));}case Cyc_Absyn_Gte: _LL297: _LL298: {
const char*_tmp831;return(_tmp831=">=",_tag_dyneither(_tmp831,sizeof(char),3));}case Cyc_Absyn_Lte: _LL299: _LL29A: {
const char*_tmp832;return(_tmp832="<=",_tag_dyneither(_tmp832,sizeof(char),3));}case Cyc_Absyn_Not: _LL29B: _LL29C: {
const char*_tmp833;return(_tmp833="!",_tag_dyneither(_tmp833,sizeof(char),2));}case Cyc_Absyn_Bitnot: _LL29D: _LL29E: {
const char*_tmp834;return(_tmp834="~",_tag_dyneither(_tmp834,sizeof(char),2));}case Cyc_Absyn_Bitand: _LL29F: _LL2A0: {
const char*_tmp835;return(_tmp835="&",_tag_dyneither(_tmp835,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL2A1: _LL2A2: {
const char*_tmp836;return(_tmp836="|",_tag_dyneither(_tmp836,sizeof(char),2));}case Cyc_Absyn_Bitxor: _LL2A3: _LL2A4: {
const char*_tmp837;return(_tmp837="^",_tag_dyneither(_tmp837,sizeof(char),2));}case Cyc_Absyn_Bitlshift: _LL2A5: _LL2A6: {
const char*_tmp838;return(_tmp838="<<",_tag_dyneither(_tmp838,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL2A7: _LL2A8: {
const char*_tmp839;return(_tmp839=">>",_tag_dyneither(_tmp839,sizeof(char),3));}case Cyc_Absyn_Bitarshift: _LL2A9: _LL2AA: {
const char*_tmp83A;return(_tmp83A=">>>",_tag_dyneither(_tmp83A,sizeof(char),4));}default: _LL2AB: _LL2AC: {
const char*_tmp83B;return(_tmp83B="numelts",_tag_dyneither(_tmp83B,sizeof(char),8));}}_LL284:;}
# 1408
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1412
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp349=s->r;void*_tmp34A=_tmp349;if(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp34A)->tag == 12){_LL2AE: _LL2AF:
 return 1;}else{_LL2B0: _LL2B1:
 return 0;}_LL2AD:;}
# 1419
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp){
struct Cyc_PP_Doc*s;
{void*_tmp34B=st->r;void*_tmp34C=_tmp34B;struct Cyc_Absyn_Exp*_tmp34D;struct Cyc_Absyn_Stmt*_tmp34E;struct Cyc_List_List*_tmp34F;struct Cyc_Absyn_Stmt*_tmp350;struct Cyc_Absyn_Exp*_tmp351;struct _dyneither_ptr*_tmp352;struct Cyc_Absyn_Stmt*_tmp353;struct Cyc_Absyn_Decl*_tmp354;struct Cyc_Absyn_Stmt*_tmp355;struct Cyc_List_List*_tmp356;struct Cyc_Absyn_Exp*_tmp357;struct Cyc_List_List*_tmp358;struct Cyc_Absyn_Exp*_tmp359;struct Cyc_Absyn_Exp*_tmp35A;struct Cyc_Absyn_Exp*_tmp35B;struct Cyc_Absyn_Stmt*_tmp35C;struct _dyneither_ptr*_tmp35D;struct Cyc_Absyn_Exp*_tmp35E;struct Cyc_Absyn_Stmt*_tmp35F;struct Cyc_Absyn_Exp*_tmp360;struct Cyc_Absyn_Stmt*_tmp361;struct Cyc_Absyn_Stmt*_tmp362;struct Cyc_Absyn_Exp*_tmp363;struct Cyc_Absyn_Stmt*_tmp364;struct Cyc_Absyn_Stmt*_tmp365;struct Cyc_Absyn_Exp*_tmp366;switch(*((int*)_tmp34C)){case 0: _LL2B3: _LL2B4:
{const char*_tmp83C;s=Cyc_PP_text(((_tmp83C=";",_tag_dyneither(_tmp83C,sizeof(char),2))));}goto _LL2B2;case 1: _LL2B5: _tmp366=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_LL2B6:
{const char*_tmp83F;struct Cyc_PP_Doc*_tmp83E[2];s=((_tmp83E[1]=Cyc_PP_text(((_tmp83F=";",_tag_dyneither(_tmp83F,sizeof(char),2)))),((_tmp83E[0]=Cyc_Absynpp_exp2doc(_tmp366),Cyc_PP_cat(_tag_dyneither(_tmp83E,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL2B2;case 2: _LL2B7: _tmp364=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp365=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL2B8:
# 1425
 if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp364)){
struct Cyc_PP_Doc*_tmp84A[5];struct Cyc_PP_Doc*_tmp849[7];const char*_tmp848;const char*_tmp847;struct Cyc_PP_Doc*_tmp846[7];s=((_tmp846[6]=
# 1433
Cyc_Absynpp_is_declaration(_tmp365)?
stmtexp?
(_tmp849[6]=
# 1438
Cyc_PP_line_doc(),((_tmp849[5]=
# 1437
Cyc_PP_text(((_tmp848=");",_tag_dyneither(_tmp848,sizeof(char),3)))),((_tmp849[4]=Cyc_Absynpp_rb(),((_tmp849[3]=
# 1436
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp365,stmtexp)),((_tmp849[2]=
# 1435
Cyc_PP_blank_doc(),((_tmp849[1]=Cyc_Absynpp_lb(),((_tmp849[0]=Cyc_PP_text(((_tmp847="(",_tag_dyneither(_tmp847,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp849,sizeof(struct Cyc_PP_Doc*),7))))))))))))))):(
# 1439
(_tmp84A[4]=
# 1442
Cyc_PP_line_doc(),((_tmp84A[3]=
# 1441
Cyc_Absynpp_rb(),((_tmp84A[2]=
# 1440
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp365,stmtexp)),((_tmp84A[1]=
# 1439
Cyc_PP_blank_doc(),((_tmp84A[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp84A,sizeof(struct Cyc_PP_Doc*),5)))))))))))):
# 1443
 Cyc_Absynpp_stmt2doc(_tmp365,stmtexp),((_tmp846[5]=
# 1432
Cyc_PP_line_doc(),((_tmp846[4]=
# 1431
Cyc_Absynpp_rb(),((_tmp846[3]=
# 1430
Cyc_PP_line_doc(),((_tmp846[2]=
# 1429
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp364,0)),((_tmp846[1]=
# 1428
Cyc_PP_blank_doc(),((_tmp846[0]=
# 1427
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp846,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1444
if(Cyc_Absynpp_is_declaration(_tmp365)){
struct Cyc_PP_Doc*_tmp855[4];struct Cyc_PP_Doc*_tmp854[6];const char*_tmp853;const char*_tmp852;struct Cyc_PP_Doc*_tmp851[4];s=((_tmp851[3]=
# 1455
Cyc_PP_line_doc(),((_tmp851[2]=
# 1447
stmtexp?
(_tmp854[5]=
# 1450
Cyc_PP_text(((_tmp853=");",_tag_dyneither(_tmp853,sizeof(char),3)))),((_tmp854[4]=Cyc_Absynpp_rb(),((_tmp854[3]=
# 1449
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp365,stmtexp)),((_tmp854[2]=
# 1448
Cyc_PP_blank_doc(),((_tmp854[1]=Cyc_Absynpp_lb(),((_tmp854[0]=Cyc_PP_text(((_tmp852="(",_tag_dyneither(_tmp852,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp854,sizeof(struct Cyc_PP_Doc*),6))))))))))))):(
# 1452
(_tmp855[3]=
# 1454
Cyc_Absynpp_rb(),((_tmp855[2]=
# 1453
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp365,stmtexp)),((_tmp855[1]=
# 1452
Cyc_PP_blank_doc(),((_tmp855[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp855,sizeof(struct Cyc_PP_Doc*),4)))))))))),((_tmp851[1]=
# 1446
Cyc_PP_line_doc(),((_tmp851[0]=
# 1445
Cyc_Absynpp_stmt2doc(_tmp364,0),Cyc_PP_cat(_tag_dyneither(_tmp851,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1457
struct Cyc_PP_Doc*_tmp856[3];s=((_tmp856[2]=Cyc_Absynpp_stmt2doc(_tmp365,stmtexp),((_tmp856[1]=Cyc_PP_line_doc(),((_tmp856[0]=Cyc_Absynpp_stmt2doc(_tmp364,0),Cyc_PP_cat(_tag_dyneither(_tmp856,sizeof(struct Cyc_PP_Doc*),3))))))));}}}else{
# 1460
struct Cyc_PP_Doc*_tmp857[3];s=((_tmp857[2]=Cyc_Absynpp_stmt2doc(_tmp365,stmtexp),((_tmp857[1]=Cyc_PP_line_doc(),((_tmp857[0]=Cyc_Absynpp_stmt2doc(_tmp364,0),Cyc_PP_cat(_tag_dyneither(_tmp857,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL2B2;case 3: _LL2B9: _tmp363=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_LL2BA:
# 1463
 if(_tmp363 == 0){
const char*_tmp858;s=Cyc_PP_text(((_tmp858="return;",_tag_dyneither(_tmp858,sizeof(char),8))));}else{
# 1466
const char*_tmp85D;const char*_tmp85C;struct Cyc_PP_Doc*_tmp85B[3];s=((_tmp85B[2]=
# 1468
Cyc_PP_text(((_tmp85C=";",_tag_dyneither(_tmp85C,sizeof(char),2)))),((_tmp85B[1]=
# 1467
_tmp363 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(_tmp363),((_tmp85B[0]=
# 1466
Cyc_PP_text(((_tmp85D="return ",_tag_dyneither(_tmp85D,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp85B,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1469
goto _LL2B2;case 4: _LL2BB: _tmp360=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp361=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_tmp362=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp34C)->f3;_LL2BC: {
# 1471
int print_else;
{void*_tmp37A=_tmp362->r;void*_tmp37B=_tmp37A;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp37B)->tag == 0){_LL2D8: _LL2D9:
 print_else=0;goto _LL2D7;}else{_LL2DA: _LL2DB:
 print_else=1;goto _LL2D7;}_LL2D7:;}
# 1476
{const char*_tmp86C;const char*_tmp86B;struct Cyc_PP_Doc*_tmp86A[2];struct Cyc_PP_Doc*_tmp869[6];struct Cyc_PP_Doc*_tmp868[2];const char*_tmp867;struct Cyc_PP_Doc*_tmp866[8];s=((_tmp866[7]=
# 1483
print_else?
(_tmp869[5]=
# 1489
Cyc_Absynpp_rb(),((_tmp869[4]=
# 1488
Cyc_PP_line_doc(),((_tmp869[3]=
# 1487
Cyc_PP_nest(2,((_tmp868[1]=Cyc_Absynpp_stmt2doc(_tmp362,0),((_tmp868[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp868,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp869[2]=
# 1486
Cyc_Absynpp_lb(),((_tmp869[1]=
# 1485
Cyc_PP_text(((_tmp867="else ",_tag_dyneither(_tmp867,sizeof(char),6)))),((_tmp869[0]=
# 1484
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp869,sizeof(struct Cyc_PP_Doc*),6))))))))))))):
# 1490
 Cyc_PP_nil_doc(),((_tmp866[6]=
# 1482
Cyc_Absynpp_rb(),((_tmp866[5]=
# 1481
Cyc_PP_line_doc(),((_tmp866[4]=
# 1480
Cyc_PP_nest(2,((_tmp86A[1]=Cyc_Absynpp_stmt2doc(_tmp361,0),((_tmp86A[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp86A,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp866[3]=
# 1479
Cyc_Absynpp_lb(),((_tmp866[2]=
# 1478
Cyc_PP_text(((_tmp86B=") ",_tag_dyneither(_tmp86B,sizeof(char),3)))),((_tmp866[1]=
# 1477
Cyc_Absynpp_exp2doc(_tmp360),((_tmp866[0]=
# 1476
Cyc_PP_text(((_tmp86C="if (",_tag_dyneither(_tmp86C,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp866,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1491
goto _LL2B2;}case 5: _LL2BD: _tmp35E=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1).f1;_tmp35F=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL2BE:
# 1493
{const char*_tmp873;const char*_tmp872;struct Cyc_PP_Doc*_tmp871[2];struct Cyc_PP_Doc*_tmp870[7];s=((_tmp870[6]=
# 1499
Cyc_Absynpp_rb(),((_tmp870[5]=
# 1498
Cyc_PP_line_doc(),((_tmp870[4]=
# 1497
Cyc_PP_nest(2,((_tmp871[1]=Cyc_Absynpp_stmt2doc(_tmp35F,0),((_tmp871[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp871,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp870[3]=
# 1496
Cyc_Absynpp_lb(),((_tmp870[2]=
# 1495
Cyc_PP_text(((_tmp872=") ",_tag_dyneither(_tmp872,sizeof(char),3)))),((_tmp870[1]=
# 1494
Cyc_Absynpp_exp2doc(_tmp35E),((_tmp870[0]=
# 1493
Cyc_PP_text(((_tmp873="while (",_tag_dyneither(_tmp873,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp870,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1500
goto _LL2B2;case 6: _LL2BF: _LL2C0:
{const char*_tmp874;s=Cyc_PP_text(((_tmp874="break;",_tag_dyneither(_tmp874,sizeof(char),7))));}goto _LL2B2;case 7: _LL2C1: _LL2C2:
{const char*_tmp875;s=Cyc_PP_text(((_tmp875="continue;",_tag_dyneither(_tmp875,sizeof(char),10))));}goto _LL2B2;case 8: _LL2C3: _tmp35D=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_LL2C4:
{const char*_tmp879;void*_tmp878[1];struct Cyc_String_pa_PrintArg_struct _tmp877;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp877.tag=0,((_tmp877.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp35D),((_tmp878[0]=& _tmp877,Cyc_aprintf(((_tmp879="goto %s;",_tag_dyneither(_tmp879,sizeof(char),9))),_tag_dyneither(_tmp878,sizeof(void*),1)))))))));}goto _LL2B2;case 9: _LL2C5: _tmp359=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp35A=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2).f1;_tmp35B=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34C)->f3).f1;_tmp35C=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34C)->f4;_LL2C6:
# 1505
{const char*_tmp884;const char*_tmp883;const char*_tmp882;const char*_tmp881;struct Cyc_PP_Doc*_tmp880[2];struct Cyc_PP_Doc*_tmp87F[11];s=((_tmp87F[10]=
# 1515
Cyc_Absynpp_rb(),((_tmp87F[9]=
# 1514
Cyc_PP_line_doc(),((_tmp87F[8]=
# 1513
Cyc_PP_nest(2,((_tmp880[1]=Cyc_Absynpp_stmt2doc(_tmp35C,0),((_tmp880[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp880,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp87F[7]=
# 1512
Cyc_Absynpp_lb(),((_tmp87F[6]=
# 1511
Cyc_PP_text(((_tmp881=") ",_tag_dyneither(_tmp881,sizeof(char),3)))),((_tmp87F[5]=
# 1510
Cyc_Absynpp_exp2doc(_tmp35B),((_tmp87F[4]=
# 1509
Cyc_PP_text(((_tmp882="; ",_tag_dyneither(_tmp882,sizeof(char),3)))),((_tmp87F[3]=
# 1508
Cyc_Absynpp_exp2doc(_tmp35A),((_tmp87F[2]=
# 1507
Cyc_PP_text(((_tmp883="; ",_tag_dyneither(_tmp883,sizeof(char),3)))),((_tmp87F[1]=
# 1506
Cyc_Absynpp_exp2doc(_tmp359),((_tmp87F[0]=
# 1505
Cyc_PP_text(((_tmp884="for(",_tag_dyneither(_tmp884,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp87F,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1516
goto _LL2B2;case 10: _LL2C7: _tmp357=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp358=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL2C8:
# 1518
{const char*_tmp889;const char*_tmp888;struct Cyc_PP_Doc*_tmp887[8];s=((_tmp887[7]=
# 1525
Cyc_Absynpp_rb(),((_tmp887[6]=
# 1524
Cyc_PP_line_doc(),((_tmp887[5]=
# 1523
Cyc_Absynpp_switchclauses2doc(_tmp358),((_tmp887[4]=
# 1522
Cyc_PP_line_doc(),((_tmp887[3]=
# 1521
Cyc_Absynpp_lb(),((_tmp887[2]=
# 1520
Cyc_PP_text(((_tmp888=") ",_tag_dyneither(_tmp888,sizeof(char),3)))),((_tmp887[1]=
# 1519
Cyc_Absynpp_exp2doc(_tmp357),((_tmp887[0]=
# 1518
Cyc_PP_text(((_tmp889="switch (",_tag_dyneither(_tmp889,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp887,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1526
goto _LL2B2;case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1 == 0){_LL2C9: _LL2CA:
{const char*_tmp88A;s=Cyc_PP_text(((_tmp88A="fallthru;",_tag_dyneither(_tmp88A,sizeof(char),10))));}goto _LL2B2;}else{_LL2CB: _tmp356=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_LL2CC:
# 1529
{const char*_tmp88F;const char*_tmp88E;struct Cyc_PP_Doc*_tmp88D[3];s=((_tmp88D[2]=Cyc_PP_text(((_tmp88E=");",_tag_dyneither(_tmp88E,sizeof(char),3)))),((_tmp88D[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp356),((_tmp88D[0]=Cyc_PP_text(((_tmp88F="fallthru(",_tag_dyneither(_tmp88F,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp88D,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL2B2;}case 12: _LL2CD: _tmp354=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp355=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL2CE:
# 1531
{struct Cyc_PP_Doc*_tmp890[3];s=((_tmp890[2]=
# 1533
Cyc_Absynpp_stmt2doc(_tmp355,stmtexp),((_tmp890[1]=
# 1532
Cyc_PP_line_doc(),((_tmp890[0]=
# 1531
Cyc_Absynpp_decl2doc(_tmp354),Cyc_PP_cat(_tag_dyneither(_tmp890,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1534
goto _LL2B2;case 13: _LL2CF: _tmp352=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp353=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL2D0:
# 1536
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp353)){
if(stmtexp){
const char*_tmp895;const char*_tmp894;struct Cyc_PP_Doc*_tmp893[8];s=((_tmp893[7]=
# 1544
Cyc_PP_text(((_tmp894=");",_tag_dyneither(_tmp894,sizeof(char),3)))),((_tmp893[6]=Cyc_Absynpp_rb(),((_tmp893[5]=
# 1543
Cyc_PP_line_doc(),((_tmp893[4]=
# 1542
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp353,1)),((_tmp893[3]=
# 1541
Cyc_PP_line_doc(),((_tmp893[2]=
# 1540
Cyc_Absynpp_lb(),((_tmp893[1]=
# 1539
Cyc_PP_text(((_tmp895=": (",_tag_dyneither(_tmp895,sizeof(char),4)))),((_tmp893[0]=
# 1538
Cyc_PP_textptr(_tmp352),Cyc_PP_cat(_tag_dyneither(_tmp893,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1546
const char*_tmp898;struct Cyc_PP_Doc*_tmp897[7];s=((_tmp897[6]=
# 1552
Cyc_Absynpp_rb(),((_tmp897[5]=
# 1551
Cyc_PP_line_doc(),((_tmp897[4]=
# 1550
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp353,0)),((_tmp897[3]=
# 1549
Cyc_PP_line_doc(),((_tmp897[2]=
# 1548
Cyc_Absynpp_lb(),((_tmp897[1]=
# 1547
Cyc_PP_text(((_tmp898=": ",_tag_dyneither(_tmp898,sizeof(char),3)))),((_tmp897[0]=
# 1546
Cyc_PP_textptr(_tmp352),Cyc_PP_cat(_tag_dyneither(_tmp897,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1554
const char*_tmp89B;struct Cyc_PP_Doc*_tmp89A[3];s=((_tmp89A[2]=Cyc_Absynpp_stmt2doc(_tmp353,stmtexp),((_tmp89A[1]=Cyc_PP_text(((_tmp89B=": ",_tag_dyneither(_tmp89B,sizeof(char),3)))),((_tmp89A[0]=Cyc_PP_textptr(_tmp352),Cyc_PP_cat(_tag_dyneither(_tmp89A,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL2B2;case 14: _LL2D1: _tmp350=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp351=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2).f1;_LL2D2:
# 1557
{const char*_tmp8A2;const char*_tmp8A1;const char*_tmp8A0;struct Cyc_PP_Doc*_tmp89F[9];s=((_tmp89F[8]=
# 1565
Cyc_PP_text(((_tmp8A0=");",_tag_dyneither(_tmp8A0,sizeof(char),3)))),((_tmp89F[7]=
# 1564
Cyc_Absynpp_exp2doc(_tmp351),((_tmp89F[6]=
# 1563
Cyc_PP_text(((_tmp8A1=" while (",_tag_dyneither(_tmp8A1,sizeof(char),9)))),((_tmp89F[5]=
# 1562
Cyc_Absynpp_rb(),((_tmp89F[4]=
# 1561
Cyc_PP_line_doc(),((_tmp89F[3]=
# 1560
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp350,0)),((_tmp89F[2]=
# 1559
Cyc_PP_line_doc(),((_tmp89F[1]=
# 1558
Cyc_Absynpp_lb(),((_tmp89F[0]=
# 1557
Cyc_PP_text(((_tmp8A2="do ",_tag_dyneither(_tmp8A2,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp89F,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1566
goto _LL2B2;case 15: _LL2D3: _tmp34E=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp34F=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL2D4:
# 1568
{const char*_tmp8A7;const char*_tmp8A6;struct Cyc_PP_Doc*_tmp8A5[12];s=((_tmp8A5[11]=
# 1579
Cyc_Absynpp_rb(),((_tmp8A5[10]=
# 1578
Cyc_PP_line_doc(),((_tmp8A5[9]=
# 1577
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp34F)),((_tmp8A5[8]=
# 1576
Cyc_PP_line_doc(),((_tmp8A5[7]=
# 1575
Cyc_Absynpp_lb(),((_tmp8A5[6]=
# 1574
Cyc_PP_text(((_tmp8A6=" catch ",_tag_dyneither(_tmp8A6,sizeof(char),8)))),((_tmp8A5[5]=
# 1573
Cyc_Absynpp_rb(),((_tmp8A5[4]=
# 1572
Cyc_PP_line_doc(),((_tmp8A5[3]=
# 1571
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp34E,0)),((_tmp8A5[2]=
# 1570
Cyc_PP_line_doc(),((_tmp8A5[1]=
# 1569
Cyc_Absynpp_lb(),((_tmp8A5[0]=
# 1568
Cyc_PP_text(((_tmp8A7="try ",_tag_dyneither(_tmp8A7,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8A5,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
# 1580
goto _LL2B2;default: _LL2D5: _tmp34D=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_LL2D6:
# 1582
{const char*_tmp8AC;const char*_tmp8AB;struct Cyc_PP_Doc*_tmp8AA[3];s=((_tmp8AA[2]=Cyc_PP_text(((_tmp8AB=");",_tag_dyneither(_tmp8AB,sizeof(char),3)))),((_tmp8AA[1]=Cyc_Absynpp_exp2doc(_tmp34D),((_tmp8AA[0]=Cyc_PP_text(((_tmp8AC="reset_region(",_tag_dyneither(_tmp8AC,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp8AA,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL2B2;}_LL2B2:;}
# 1585
return s;}
# 1588
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp3AB=p->r;void*_tmp3AC=_tmp3AB;struct Cyc_Absyn_Exp*_tmp3AD;struct Cyc_Absyn_Datatypefield*_tmp3AE;struct Cyc_List_List*_tmp3AF;int _tmp3B0;struct Cyc_Absyn_Datatypefield*_tmp3B1;struct Cyc_Absyn_Enumfield*_tmp3B2;struct Cyc_Absyn_Enumfield*_tmp3B3;struct Cyc_List_List*_tmp3B4;struct Cyc_List_List*_tmp3B5;int _tmp3B6;union Cyc_Absyn_AggrInfoU _tmp3B7;struct Cyc_List_List*_tmp3B8;struct Cyc_List_List*_tmp3B9;int _tmp3BA;struct _tuple0*_tmp3BB;struct Cyc_List_List*_tmp3BC;int _tmp3BD;struct _tuple0*_tmp3BE;struct Cyc_Absyn_Vardecl*_tmp3BF;struct Cyc_Absyn_Pat*_tmp3C0;struct Cyc_Absyn_Vardecl*_tmp3C1;struct Cyc_Absyn_Pat*_tmp3C2;struct Cyc_List_List*_tmp3C3;int _tmp3C4;struct Cyc_Absyn_Tvar*_tmp3C5;struct Cyc_Absyn_Vardecl*_tmp3C6;struct Cyc_Absyn_Tvar*_tmp3C7;struct Cyc_Absyn_Vardecl*_tmp3C8;struct Cyc_Absyn_Vardecl*_tmp3C9;struct Cyc_Absyn_Pat*_tmp3CA;struct Cyc_Absyn_Vardecl*_tmp3CB;struct _dyneither_ptr _tmp3CC;char _tmp3CD;enum Cyc_Absyn_Sign _tmp3CE;int _tmp3CF;switch(*((int*)_tmp3AC)){case 0: _LL2DD: _LL2DE:
{const char*_tmp8AD;s=Cyc_PP_text(((_tmp8AD="_",_tag_dyneither(_tmp8AD,sizeof(char),2))));}goto _LL2DC;case 9: _LL2DF: _LL2E0:
{const char*_tmp8AE;s=Cyc_PP_text(((_tmp8AE="NULL",_tag_dyneither(_tmp8AE,sizeof(char),5))));}goto _LL2DC;case 10: _LL2E1: _tmp3CE=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3CF=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2E2:
# 1594
 if(_tmp3CE != Cyc_Absyn_Unsigned){
const char*_tmp8B2;void*_tmp8B1[1];struct Cyc_Int_pa_PrintArg_struct _tmp8B0;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp8B0.tag=1,((_tmp8B0.f1=(unsigned long)_tmp3CF,((_tmp8B1[0]=& _tmp8B0,Cyc_aprintf(((_tmp8B2="%d",_tag_dyneither(_tmp8B2,sizeof(char),3))),_tag_dyneither(_tmp8B1,sizeof(void*),1)))))))));}else{
const char*_tmp8B6;void*_tmp8B5[1];struct Cyc_Int_pa_PrintArg_struct _tmp8B4;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp8B4.tag=1,((_tmp8B4.f1=(unsigned int)_tmp3CF,((_tmp8B5[0]=& _tmp8B4,Cyc_aprintf(((_tmp8B6="%u",_tag_dyneither(_tmp8B6,sizeof(char),3))),_tag_dyneither(_tmp8B5,sizeof(void*),1)))))))));}
goto _LL2DC;case 11: _LL2E3: _tmp3CD=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2E4:
{const char*_tmp8BA;void*_tmp8B9[1];struct Cyc_String_pa_PrintArg_struct _tmp8B8;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp8B8.tag=0,((_tmp8B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp3CD)),((_tmp8B9[0]=& _tmp8B8,Cyc_aprintf(((_tmp8BA="'%s'",_tag_dyneither(_tmp8BA,sizeof(char),5))),_tag_dyneither(_tmp8B9,sizeof(void*),1)))))))));}goto _LL2DC;case 12: _LL2E5: _tmp3CC=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2E6:
 s=Cyc_PP_text(_tmp3CC);goto _LL2DC;case 1: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2)->r)->tag == 0){_LL2E7: _tmp3CB=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2E8:
# 1601
 s=Cyc_Absynpp_qvar2doc(_tmp3CB->name);goto _LL2DC;}else{_LL2E9: _tmp3C9=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3CA=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2EA:
# 1603
{const char*_tmp8BD;struct Cyc_PP_Doc*_tmp8BC[3];s=((_tmp8BC[2]=Cyc_Absynpp_pat2doc(_tmp3CA),((_tmp8BC[1]=Cyc_PP_text(((_tmp8BD=" as ",_tag_dyneither(_tmp8BD,sizeof(char),5)))),((_tmp8BC[0]=Cyc_Absynpp_qvar2doc(_tmp3C9->name),Cyc_PP_cat(_tag_dyneither(_tmp8BC,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL2DC;}case 2: _LL2EB: _tmp3C7=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3C8=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2EC:
# 1605
{const char*_tmp8C4;const char*_tmp8C3;const char*_tmp8C2;struct Cyc_PP_Doc*_tmp8C1[5];s=((_tmp8C1[4]=
Cyc_Absynpp_vardecl2doc(_tmp3C8),((_tmp8C1[3]=
# 1605
Cyc_PP_text(((_tmp8C2="> ",_tag_dyneither(_tmp8C2,sizeof(char),3)))),((_tmp8C1[2]=Cyc_Absynpp_tvar2doc(_tmp3C7),((_tmp8C1[1]=Cyc_PP_text(((_tmp8C3=" <",_tag_dyneither(_tmp8C3,sizeof(char),3)))),((_tmp8C1[0]=Cyc_PP_text(((_tmp8C4="alias",_tag_dyneither(_tmp8C4,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp8C1,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1607
goto _LL2DC;case 4: _LL2ED: _tmp3C5=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3C6=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2EE:
# 1609
{const char*_tmp8C9;const char*_tmp8C8;struct Cyc_PP_Doc*_tmp8C7[4];s=((_tmp8C7[3]=Cyc_PP_text(((_tmp8C8=">",_tag_dyneither(_tmp8C8,sizeof(char),2)))),((_tmp8C7[2]=Cyc_Absynpp_tvar2doc(_tmp3C5),((_tmp8C7[1]=Cyc_PP_text(((_tmp8C9="<",_tag_dyneither(_tmp8C9,sizeof(char),2)))),((_tmp8C7[0]=Cyc_Absynpp_qvar2doc(_tmp3C6->name),Cyc_PP_cat(_tag_dyneither(_tmp8C7,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL2DC;case 5: _LL2EF: _tmp3C3=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3C4=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2F0:
# 1612
{const char*_tmp8D2;const char*_tmp8D1;const char*_tmp8D0;const char*_tmp8CF;struct Cyc_PP_Doc*_tmp8CE[4];s=((_tmp8CE[3]=
_tmp3C4?Cyc_PP_text(((_tmp8CF=", ...)",_tag_dyneither(_tmp8CF,sizeof(char),7)))): Cyc_PP_text(((_tmp8D0=")",_tag_dyneither(_tmp8D0,sizeof(char),2)))),((_tmp8CE[2]=
# 1612
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp8D1=",",_tag_dyneither(_tmp8D1,sizeof(char),2))),_tmp3C3),((_tmp8CE[1]=Cyc_PP_text(((_tmp8D2="(",_tag_dyneither(_tmp8D2,sizeof(char),2)))),((_tmp8CE[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp8CE,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1614
goto _LL2DC;case 6: _LL2F1: _tmp3C2=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2F2:
# 1616
{const char*_tmp8D5;struct Cyc_PP_Doc*_tmp8D4[2];s=((_tmp8D4[1]=Cyc_Absynpp_pat2doc(_tmp3C2),((_tmp8D4[0]=Cyc_PP_text(((_tmp8D5="&",_tag_dyneither(_tmp8D5,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8D4,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL2DC;case 3: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2)->r)->tag == 0){_LL2F3: _tmp3C1=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2F4:
# 1619
{const char*_tmp8D8;struct Cyc_PP_Doc*_tmp8D7[2];s=((_tmp8D7[1]=Cyc_Absynpp_qvar2doc(_tmp3C1->name),((_tmp8D7[0]=Cyc_PP_text(((_tmp8D8="*",_tag_dyneither(_tmp8D8,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8D7,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL2DC;}else{_LL2F5: _tmp3BF=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3C0=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2F6:
# 1622
{const char*_tmp8DD;const char*_tmp8DC;struct Cyc_PP_Doc*_tmp8DB[4];s=((_tmp8DB[3]=Cyc_Absynpp_pat2doc(_tmp3C0),((_tmp8DB[2]=Cyc_PP_text(((_tmp8DC=" as ",_tag_dyneither(_tmp8DC,sizeof(char),5)))),((_tmp8DB[1]=Cyc_Absynpp_qvar2doc(_tmp3BF->name),((_tmp8DB[0]=Cyc_PP_text(((_tmp8DD="*",_tag_dyneither(_tmp8DD,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8DB,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL2DC;}case 15: _LL2F7: _tmp3BE=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2F8:
# 1625
 s=Cyc_Absynpp_qvar2doc(_tmp3BE);
goto _LL2DC;case 16: _LL2F9: _tmp3BB=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3BC=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_tmp3BD=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AC)->f3;_LL2FA: {
# 1628
const char*_tmp8DF;const char*_tmp8DE;struct _dyneither_ptr term=_tmp3BD?(_tmp8DF=", ...)",_tag_dyneither(_tmp8DF,sizeof(char),7)):((_tmp8DE=")",_tag_dyneither(_tmp8DE,sizeof(char),2)));
{const char*_tmp8E4;const char*_tmp8E3;struct Cyc_PP_Doc*_tmp8E2[2];s=((_tmp8E2[1]=Cyc_PP_group(((_tmp8E4="(",_tag_dyneither(_tmp8E4,sizeof(char),2))),term,((_tmp8E3=",",_tag_dyneither(_tmp8E3,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp3BC)),((_tmp8E2[0]=Cyc_Absynpp_qvar2doc(_tmp3BB),Cyc_PP_cat(_tag_dyneither(_tmp8E2,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL2DC;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1 != 0){_LL2FB: _tmp3B7=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1)->aggr_info;_tmp3B8=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_tmp3B9=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f3;_tmp3BA=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f4;_LL2FC: {
# 1632
const char*_tmp8E6;const char*_tmp8E5;struct _dyneither_ptr term=_tmp3BA?(_tmp8E6=", ...}",_tag_dyneither(_tmp8E6,sizeof(char),7)):((_tmp8E5="}",_tag_dyneither(_tmp8E5,sizeof(char),2)));
struct _tuple10 _tmp3F5=Cyc_Absyn_aggr_kinded_name(_tmp3B7);struct _tuple10 _tmp3F6=_tmp3F5;struct _tuple0*_tmp3F7;_LL30A: _tmp3F7=_tmp3F6.f2;_LL30B:
{const char*_tmp8F1;const char*_tmp8F0;const char*_tmp8EF;const char*_tmp8EE;const char*_tmp8ED;struct Cyc_PP_Doc*_tmp8EC[4];s=((_tmp8EC[3]=
# 1636
Cyc_PP_group(((_tmp8EE="",_tag_dyneither(_tmp8EE,sizeof(char),1))),term,((_tmp8ED=",",_tag_dyneither(_tmp8ED,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp3B9)),((_tmp8EC[2]=
# 1635
Cyc_PP_egroup(((_tmp8F1="[",_tag_dyneither(_tmp8F1,sizeof(char),2))),((_tmp8F0="]",_tag_dyneither(_tmp8F0,sizeof(char),2))),((_tmp8EF=",",_tag_dyneither(_tmp8EF,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp3B8)),((_tmp8EC[1]=
# 1634
Cyc_Absynpp_lb(),((_tmp8EC[0]=Cyc_Absynpp_qvar2doc(_tmp3F7),Cyc_PP_cat(_tag_dyneither(_tmp8EC,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1637
goto _LL2DC;_LL309:;}}else{_LL2FD: _tmp3B4=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_tmp3B5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f3;_tmp3B6=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f4;_LL2FE: {
# 1639
const char*_tmp8F3;const char*_tmp8F2;struct _dyneither_ptr term=_tmp3B6?(_tmp8F3=", ...}",_tag_dyneither(_tmp8F3,sizeof(char),7)):((_tmp8F2="}",_tag_dyneither(_tmp8F2,sizeof(char),2)));
{const char*_tmp8FE;const char*_tmp8FD;const char*_tmp8FC;const char*_tmp8FB;const char*_tmp8FA;struct Cyc_PP_Doc*_tmp8F9[3];s=((_tmp8F9[2]=
# 1642
Cyc_PP_group(((_tmp8FB="",_tag_dyneither(_tmp8FB,sizeof(char),1))),term,((_tmp8FA=",",_tag_dyneither(_tmp8FA,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp3B5)),((_tmp8F9[1]=
# 1641
Cyc_PP_egroup(((_tmp8FE="[",_tag_dyneither(_tmp8FE,sizeof(char),2))),((_tmp8FD="]",_tag_dyneither(_tmp8FD,sizeof(char),2))),((_tmp8FC=",",_tag_dyneither(_tmp8FC,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp3B4)),((_tmp8F9[0]=
# 1640
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8F9,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1643
goto _LL2DC;}}case 13: _LL2FF: _tmp3B3=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL300:
 _tmp3B2=_tmp3B3;goto _LL302;case 14: _LL301: _tmp3B2=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL302:
 s=Cyc_Absynpp_qvar2doc(_tmp3B2->name);goto _LL2DC;case 8: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AC)->f3 == 0){_LL303: _tmp3B1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL304:
 s=Cyc_Absynpp_qvar2doc(_tmp3B1->name);goto _LL2DC;}else{_LL305: _tmp3AE=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_tmp3AF=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AC)->f3;_tmp3B0=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AC)->f4;_LL306: {
# 1648
const char*_tmp900;const char*_tmp8FF;struct _dyneither_ptr term=_tmp3B0?(_tmp900=", ...)",_tag_dyneither(_tmp900,sizeof(char),7)):((_tmp8FF=")",_tag_dyneither(_tmp8FF,sizeof(char),2)));
{const char*_tmp905;const char*_tmp904;struct Cyc_PP_Doc*_tmp903[2];s=((_tmp903[1]=Cyc_PP_egroup(((_tmp905="(",_tag_dyneither(_tmp905,sizeof(char),2))),term,((_tmp904=",",_tag_dyneither(_tmp904,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp3AF)),((_tmp903[0]=Cyc_Absynpp_qvar2doc(_tmp3AE->name),Cyc_PP_cat(_tag_dyneither(_tmp903,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL2DC;}}default: _LL307: _tmp3AD=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL308:
# 1652
 s=Cyc_Absynpp_exp2doc(_tmp3AD);goto _LL2DC;}_LL2DC:;}
# 1654
return s;}
# 1657
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
const char*_tmp90C;const char*_tmp90B;const char*_tmp90A;struct Cyc_PP_Doc*_tmp909[2];return(_tmp909[1]=
Cyc_Absynpp_pat2doc((*dp).f2),((_tmp909[0]=
# 1658
Cyc_PP_egroup(((_tmp90C="",_tag_dyneither(_tmp90C,sizeof(char),1))),((_tmp90B="=",_tag_dyneither(_tmp90B,sizeof(char),2))),((_tmp90A="=",_tag_dyneither(_tmp90A,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp909,sizeof(struct Cyc_PP_Doc*),2)))));}
# 1662
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){
const char*_tmp911;struct Cyc_PP_Doc*_tmp910[2];struct Cyc_PP_Doc*_tmp90F[2];return(_tmp90F[1]=
Cyc_PP_nest(2,((_tmp910[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp910[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp910,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp90F[0]=
# 1664
Cyc_PP_text(((_tmp911="default: ",_tag_dyneither(_tmp911,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp90F,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 1666
if(c->where_clause == 0){
const char*_tmp918;const char*_tmp917;struct Cyc_PP_Doc*_tmp916[2];struct Cyc_PP_Doc*_tmp915[4];return(_tmp915[3]=
# 1670
Cyc_PP_nest(2,((_tmp916[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp916[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp916,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp915[2]=
# 1669
Cyc_PP_text(((_tmp917=": ",_tag_dyneither(_tmp917,sizeof(char),3)))),((_tmp915[1]=
# 1668
Cyc_Absynpp_pat2doc(c->pattern),((_tmp915[0]=
# 1667
Cyc_PP_text(((_tmp918="case ",_tag_dyneither(_tmp918,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp915,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1672
const char*_tmp921;const char*_tmp920;const char*_tmp91F;struct Cyc_PP_Doc*_tmp91E[2];struct Cyc_PP_Doc*_tmp91D[6];return(_tmp91D[5]=
# 1677
Cyc_PP_nest(2,((_tmp91E[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp91E[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp91E,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp91D[4]=
# 1676
Cyc_PP_text(((_tmp91F=": ",_tag_dyneither(_tmp91F,sizeof(char),3)))),((_tmp91D[3]=
# 1675
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp91D[2]=
# 1674
Cyc_PP_text(((_tmp920=" && ",_tag_dyneither(_tmp920,sizeof(char),5)))),((_tmp91D[1]=
# 1673
Cyc_Absynpp_pat2doc(c->pattern),((_tmp91D[0]=
# 1672
Cyc_PP_text(((_tmp921="case ",_tag_dyneither(_tmp921,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp91D,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
# 1680
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
const char*_tmp922;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((_tmp922="",_tag_dyneither(_tmp922,sizeof(char),1))),cs);}
# 1684
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1688
const char*_tmp925;struct Cyc_PP_Doc*_tmp924[3];return(_tmp924[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp924[1]=Cyc_PP_text(((_tmp925=" = ",_tag_dyneither(_tmp925,sizeof(char),4)))),((_tmp924[0]=Cyc_Absynpp_qvar2doc(f->name),Cyc_PP_cat(_tag_dyneither(_tmp924,sizeof(struct Cyc_PP_Doc*),3)))))));}}
# 1691
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
const char*_tmp926;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp926=",",_tag_dyneither(_tmp926,sizeof(char),2))),fs);}
# 1695
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1699
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
const char*_tmp927;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,((_tmp927=",",_tag_dyneither(_tmp927,sizeof(char),2))),vds);}
# 1703
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_Absyn_Vardecl*_tmp422=vd;enum Cyc_Absyn_Scope _tmp423;struct _tuple0*_tmp424;struct Cyc_Absyn_Tqual _tmp425;void*_tmp426;struct Cyc_Absyn_Exp*_tmp427;struct Cyc_List_List*_tmp428;_LL30F: _tmp423=_tmp422->sc;_tmp424=_tmp422->name;_tmp425=_tmp422->tq;_tmp426=_tmp422->type;_tmp427=_tmp422->initializer;_tmp428=_tmp422->attributes;_LL310: {
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp424);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp428);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp429=Cyc_Cyclone_c_compiler;if(_tmp429 == Cyc_Cyclone_Gcc_c){_LL314: _LL315:
 beforenamedoc=attsdoc;goto _LL313;}else{_LL316: _LL317:
# 1712
{void*_tmp42A=Cyc_Tcutil_compress(_tmp426);void*_tmp42B=_tmp42A;struct Cyc_List_List*_tmp42C;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp42B)->tag == 9){_LL319: _tmp42C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp42B)->f1).attributes;_LL31A:
# 1714
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp42C);
goto _LL318;}else{_LL31B: _LL31C:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL318;}_LL318:;}
# 1718
goto _LL313;}_LL313:;}{
# 1721
struct Cyc_PP_Doc*tmp_doc;
{enum Cyc_Cyclone_C_Compilers _tmp42D=Cyc_Cyclone_c_compiler;if(_tmp42D == Cyc_Cyclone_Gcc_c){_LL31E: _LL31F:
 tmp_doc=Cyc_PP_nil_doc();goto _LL31D;}else{_LL320: _LL321:
 tmp_doc=attsdoc;goto _LL31D;}_LL31D:;}
# 1726
{struct Cyc_Core_Opt*_tmp934;struct Cyc_PP_Doc*_tmp933[2];struct Cyc_PP_Doc*_tmp932[2];const char*_tmp931;const char*_tmp930;struct Cyc_PP_Doc*_tmp92F[5];s=((_tmp92F[4]=
# 1733
Cyc_PP_text(((_tmp930=";",_tag_dyneither(_tmp930,sizeof(char),2)))),((_tmp92F[3]=
# 1730
_tmp427 == 0?
Cyc_PP_nil_doc():(
(_tmp932[1]=Cyc_Absynpp_exp2doc(_tmp427),((_tmp932[0]=Cyc_PP_text(((_tmp931=" = ",_tag_dyneither(_tmp931,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp932,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp92F[2]=
# 1729
Cyc_Absynpp_tqtd2doc(_tmp425,_tmp426,((_tmp934=_cycalloc(sizeof(*_tmp934)),((_tmp934->v=((_tmp933[1]=sn,((_tmp933[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp933,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp934))))),((_tmp92F[1]=
# 1728
Cyc_Absynpp_scope2doc(_tmp423),((_tmp92F[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp92F,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1734
return s;};}_LL30E:;}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1737
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple17 _tmp434=*x;struct _tuple17 _tmp435=_tmp434;struct _tuple0*_tmp436;_LL323: _tmp436=_tmp435.f2;_LL324:
 return Cyc_Absynpp_qvar2doc(_tmp436);_LL322:;}
# 1742
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0){
struct Cyc_PP_Doc*_tmp935[4];return(_tmp935[3]=
# 1747
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp935[2]=
# 1746
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp935[1]=
# 1745
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp935[0]=
# 1744
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp935,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1749
struct Cyc_PP_Doc*_tmp93D[2];const char*_tmp93C;struct Cyc_PP_Doc*_tmp93B[2];struct Cyc_PP_Doc*_tmp93A[12];return(_tmp93A[11]=
# 1760
Cyc_Absynpp_atts2doc(ad->attributes),((_tmp93A[10]=
# 1759
Cyc_Absynpp_rb(),((_tmp93A[9]=
# 1758
Cyc_PP_line_doc(),((_tmp93A[8]=
# 1757
Cyc_PP_nest(2,((_tmp93B[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields),((_tmp93B[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp93B,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp93A[7]=
# 1755
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():(
(_tmp93D[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po),((_tmp93D[0]=Cyc_PP_text(((_tmp93C=":",_tag_dyneither(_tmp93C,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp93D,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp93A[6]=
# 1754
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars),((_tmp93A[5]=
# 1753
Cyc_Absynpp_lb(),((_tmp93A[4]=Cyc_PP_blank_doc(),((_tmp93A[3]=
# 1752
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp93A[2]=
# 1751
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp93A[1]=
# 1750
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp93A[0]=
# 1749
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp93A,sizeof(struct Cyc_PP_Doc*),12)))))))))))))))))))))))));}}
# 1763
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp43C=dd;enum Cyc_Absyn_Scope _tmp43D;struct _tuple0*_tmp43E;struct Cyc_List_List*_tmp43F;struct Cyc_Core_Opt*_tmp440;int _tmp441;_LL328: _tmp43D=_tmp43C->sc;_tmp43E=_tmp43C->name;_tmp43F=_tmp43C->tvs;_tmp440=_tmp43C->fields;_tmp441=_tmp43C->is_extensible;_LL329:
 if(_tmp440 == 0){
const char*_tmp942;const char*_tmp941;struct Cyc_PP_Doc*_tmp940[5];return(_tmp940[4]=
# 1770
Cyc_Absynpp_ktvars2doc(_tmp43F),((_tmp940[3]=
# 1769
_tmp441?Cyc_Absynpp_qvar2bolddoc(_tmp43E): Cyc_Absynpp_typedef_name2bolddoc(_tmp43E),((_tmp940[2]=
# 1768
Cyc_PP_text(((_tmp941="datatype ",_tag_dyneither(_tmp941,sizeof(char),10)))),((_tmp940[1]=
# 1767
_tmp441?Cyc_PP_text(((_tmp942="@extensible ",_tag_dyneither(_tmp942,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp940[0]=
# 1766
Cyc_Absynpp_scope2doc(_tmp43D),Cyc_PP_cat(_tag_dyneither(_tmp940,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 1772
const char*_tmp949;const char*_tmp948;struct Cyc_PP_Doc*_tmp947[2];struct Cyc_PP_Doc*_tmp946[10];return(_tmp946[9]=
# 1780
Cyc_Absynpp_rb(),((_tmp946[8]=
# 1779
Cyc_PP_line_doc(),((_tmp946[7]=
# 1778
Cyc_PP_nest(2,((_tmp947[1]=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp440->v),((_tmp947[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp947,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp946[6]=
# 1777
Cyc_Absynpp_lb(),((_tmp946[5]=Cyc_PP_blank_doc(),((_tmp946[4]=
# 1776
Cyc_Absynpp_ktvars2doc(_tmp43F),((_tmp946[3]=
# 1775
_tmp441?Cyc_Absynpp_qvar2bolddoc(_tmp43E): Cyc_Absynpp_typedef_name2bolddoc(_tmp43E),((_tmp946[2]=
# 1774
Cyc_PP_text(((_tmp948="datatype ",_tag_dyneither(_tmp948,sizeof(char),10)))),((_tmp946[1]=
# 1773
_tmp441?Cyc_PP_text(((_tmp949="@extensible ",_tag_dyneither(_tmp949,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp946[0]=
# 1772
Cyc_Absynpp_scope2doc(_tmp43D),Cyc_PP_cat(_tag_dyneither(_tmp946,sizeof(struct Cyc_PP_Doc*),10)))))))))))))))))))));}_LL327:;}
# 1783
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp449=ed;enum Cyc_Absyn_Scope _tmp44A;struct _tuple0*_tmp44B;struct Cyc_Core_Opt*_tmp44C;_LL32D: _tmp44A=_tmp449->sc;_tmp44B=_tmp449->name;_tmp44C=_tmp449->fields;_LL32E:
 if(_tmp44C == 0){
const char*_tmp94C;struct Cyc_PP_Doc*_tmp94B[3];return(_tmp94B[2]=
# 1788
Cyc_Absynpp_typedef_name2bolddoc(_tmp44B),((_tmp94B[1]=
# 1787
Cyc_PP_text(((_tmp94C="enum ",_tag_dyneither(_tmp94C,sizeof(char),6)))),((_tmp94B[0]=
# 1786
Cyc_Absynpp_scope2doc(_tmp44A),Cyc_PP_cat(_tag_dyneither(_tmp94B,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
# 1791
const char*_tmp951;struct Cyc_PP_Doc*_tmp950[2];struct Cyc_PP_Doc*_tmp94F[8];return(_tmp94F[7]=
# 1797
Cyc_Absynpp_rb(),((_tmp94F[6]=
# 1796
Cyc_PP_line_doc(),((_tmp94F[5]=
# 1795
Cyc_PP_nest(2,((_tmp950[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp44C->v),((_tmp950[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp950,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp94F[4]=
# 1794
Cyc_Absynpp_lb(),((_tmp94F[3]=Cyc_PP_blank_doc(),((_tmp94F[2]=
# 1793
Cyc_Absynpp_qvar2bolddoc(_tmp44B),((_tmp94F[1]=
# 1792
Cyc_PP_text(((_tmp951="enum ",_tag_dyneither(_tmp951,sizeof(char),6)))),((_tmp94F[0]=
# 1791
Cyc_Absynpp_scope2doc(_tmp44A),Cyc_PP_cat(_tag_dyneither(_tmp94F,sizeof(struct Cyc_PP_Doc*),8)))))))))))))))));}_LL32C:;}
# 1800
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp452=d->r;void*_tmp453=_tmp452;struct Cyc_List_List*_tmp454;struct Cyc_List_List*_tmp455;struct Cyc_List_List*_tmp456;struct _tuple0*_tmp457;struct Cyc_List_List*_tmp458;struct _dyneither_ptr*_tmp459;struct Cyc_List_List*_tmp45A;struct Cyc_Absyn_Typedefdecl*_tmp45B;struct Cyc_Absyn_Enumdecl*_tmp45C;struct Cyc_List_List*_tmp45D;struct Cyc_Absyn_Pat*_tmp45E;struct Cyc_Absyn_Exp*_tmp45F;struct Cyc_Absyn_Datatypedecl*_tmp460;struct Cyc_Absyn_Tvar*_tmp461;struct Cyc_Absyn_Vardecl*_tmp462;int _tmp463;struct Cyc_Absyn_Exp*_tmp464;struct Cyc_Absyn_Vardecl*_tmp465;struct Cyc_Absyn_Aggrdecl*_tmp466;struct Cyc_Absyn_Fndecl*_tmp467;switch(*((int*)_tmp453)){case 1: _LL332: _tmp467=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL333: {
# 1804
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp957;struct Cyc_Absyn_FnInfo _tmp956;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp955;void*t=(void*)((_tmp955=_cycalloc(sizeof(*_tmp955)),((_tmp955[0]=((_tmp957.tag=9,((_tmp957.f1=((_tmp956.tvars=_tmp467->tvs,((_tmp956.effect=_tmp467->effect,((_tmp956.ret_tqual=_tmp467->ret_tqual,((_tmp956.ret_typ=_tmp467->ret_type,((_tmp956.args=
# 1808
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp467->args),((_tmp956.c_varargs=_tmp467->c_varargs,((_tmp956.cyc_varargs=_tmp467->cyc_varargs,((_tmp956.rgn_po=_tmp467->rgn_po,((_tmp956.attributes=0,((_tmp956.requires_clause=_tmp467->requires_clause,((_tmp956.requires_relns=_tmp467->requires_relns,((_tmp956.ensures_clause=_tmp467->ensures_clause,((_tmp956.ensures_relns=_tmp467->ensures_relns,_tmp956)))))))))))))))))))))))))),_tmp957)))),_tmp955))));
# 1814
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp467->attributes);
struct Cyc_PP_Doc*inlinedoc;
if(_tmp467->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp468=Cyc_Cyclone_c_compiler;if(_tmp468 == Cyc_Cyclone_Gcc_c){_LL351: _LL352:
{const char*_tmp958;inlinedoc=Cyc_PP_text(((_tmp958="inline ",_tag_dyneither(_tmp958,sizeof(char),8))));}goto _LL350;}else{_LL353: _LL354:
{const char*_tmp959;inlinedoc=Cyc_PP_text(((_tmp959="__inline ",_tag_dyneither(_tmp959,sizeof(char),10))));}goto _LL350;}_LL350:;}else{
# 1822
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp467->sc);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp46B=Cyc_Cyclone_c_compiler;if(_tmp46B == Cyc_Cyclone_Gcc_c){_LL356: _LL357:
 beforenamedoc=attsdoc;goto _LL355;}else{_LL358: _LL359:
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp467->attributes);goto _LL355;}_LL355:;}{
# 1829
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp467->name);
struct Cyc_PP_Doc*_tmp95C[2];struct Cyc_Core_Opt*_tmp95B;struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,(
(_tmp95B=_cycalloc(sizeof(*_tmp95B)),((_tmp95B->v=((_tmp95C[1]=namedoc,((_tmp95C[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp95C,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp95B)))));
struct Cyc_PP_Doc*_tmp95F[2];struct Cyc_PP_Doc*_tmp95E[5];struct Cyc_PP_Doc*bodydoc=(_tmp95E[4]=
# 1835
Cyc_Absynpp_rb(),((_tmp95E[3]=
# 1834
Cyc_PP_line_doc(),((_tmp95E[2]=
# 1833
Cyc_PP_nest(2,((_tmp95F[1]=Cyc_Absynpp_stmt2doc(_tmp467->body,0),((_tmp95F[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp95F,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp95E[1]=
# 1832
Cyc_Absynpp_lb(),((_tmp95E[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(_tmp95E,sizeof(struct Cyc_PP_Doc*),5)))))))))));
# 1836
{struct Cyc_PP_Doc*_tmp960[4];s=((_tmp960[3]=bodydoc,((_tmp960[2]=tqtddoc,((_tmp960[1]=scopedoc,((_tmp960[0]=inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp960,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1838
{enum Cyc_Cyclone_C_Compilers _tmp46D=Cyc_Cyclone_c_compiler;if(_tmp46D == Cyc_Cyclone_Vc_c){_LL35B: _LL35C:
{struct Cyc_PP_Doc*_tmp961[2];s=((_tmp961[1]=s,((_tmp961[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(_tmp961,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL35A;}else{_LL35D: _LL35E:
 goto _LL35A;}_LL35A:;}
# 1843
goto _LL331;};};}case 5: _LL334: _tmp466=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL335:
# 1846
{const char*_tmp964;struct Cyc_PP_Doc*_tmp963[2];s=((_tmp963[1]=Cyc_PP_text(((_tmp964=";",_tag_dyneither(_tmp964,sizeof(char),2)))),((_tmp963[0]=Cyc_Absynpp_aggrdecl2doc(_tmp466),Cyc_PP_cat(_tag_dyneither(_tmp963,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL331;case 0: _LL336: _tmp465=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL337:
# 1849
 s=Cyc_Absynpp_vardecl2doc(_tmp465);
goto _LL331;case 4: _LL338: _tmp461=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp462=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp453)->f2;_tmp463=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp453)->f3;_tmp464=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp453)->f4;_LL339:
# 1852
{const char*_tmp977;const char*_tmp976;const char*_tmp975;struct Cyc_PP_Doc*_tmp974[3];const char*_tmp973;const char*_tmp972;const char*_tmp971;const char*_tmp970;struct Cyc_PP_Doc*_tmp96F[8];s=((_tmp96F[7]=
# 1860
Cyc_PP_text(((_tmp970=";",_tag_dyneither(_tmp970,sizeof(char),2)))),((_tmp96F[6]=
# 1859
_tmp463?Cyc_PP_text(((_tmp971=" @resetable",_tag_dyneither(_tmp971,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmp96F[5]=
# 1857
(unsigned int)_tmp464?(_tmp974[2]=
Cyc_PP_text(((_tmp973=")",_tag_dyneither(_tmp973,sizeof(char),2)))),((_tmp974[1]=
# 1857
Cyc_Absynpp_exp2doc(_tmp464),((_tmp974[0]=Cyc_PP_text(((_tmp972=" = open(",_tag_dyneither(_tmp972,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp974,sizeof(struct Cyc_PP_Doc*),3))))))):
 Cyc_PP_nil_doc(),((_tmp96F[4]=
# 1856
Cyc_Absynpp_qvar2doc(_tmp462->name),((_tmp96F[3]=
# 1855
Cyc_PP_text(((_tmp975=">",_tag_dyneither(_tmp975,sizeof(char),2)))),((_tmp96F[2]=
# 1854
Cyc_Absynpp_tvar2doc(_tmp461),((_tmp96F[1]=
# 1853
Cyc_PP_text(((_tmp976="<",_tag_dyneither(_tmp976,sizeof(char),2)))),((_tmp96F[0]=
# 1852
Cyc_PP_text(((_tmp977="region",_tag_dyneither(_tmp977,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp96F,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1861
goto _LL331;case 6: _LL33A: _tmp460=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL33B:
# 1863
{const char*_tmp97A;struct Cyc_PP_Doc*_tmp979[2];s=((_tmp979[1]=Cyc_PP_text(((_tmp97A=";",_tag_dyneither(_tmp97A,sizeof(char),2)))),((_tmp979[0]=Cyc_Absynpp_datatypedecl2doc(_tmp460),Cyc_PP_cat(_tag_dyneither(_tmp979,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL331;case 2: _LL33C: _tmp45E=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp45F=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp453)->f3;_LL33D:
# 1866
{const char*_tmp981;const char*_tmp980;const char*_tmp97F;struct Cyc_PP_Doc*_tmp97E[5];s=((_tmp97E[4]=
# 1870
Cyc_PP_text(((_tmp97F=";",_tag_dyneither(_tmp97F,sizeof(char),2)))),((_tmp97E[3]=
# 1869
Cyc_Absynpp_exp2doc(_tmp45F),((_tmp97E[2]=
# 1868
Cyc_PP_text(((_tmp980=" = ",_tag_dyneither(_tmp980,sizeof(char),4)))),((_tmp97E[1]=
# 1867
Cyc_Absynpp_pat2doc(_tmp45E),((_tmp97E[0]=
# 1866
Cyc_PP_text(((_tmp981="let ",_tag_dyneither(_tmp981,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp97E,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1871
goto _LL331;case 3: _LL33E: _tmp45D=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL33F:
# 1873
{const char*_tmp986;const char*_tmp985;struct Cyc_PP_Doc*_tmp984[3];s=((_tmp984[2]=Cyc_PP_text(((_tmp985=";",_tag_dyneither(_tmp985,sizeof(char),2)))),((_tmp984[1]=Cyc_Absynpp_ids2doc(_tmp45D),((_tmp984[0]=Cyc_PP_text(((_tmp986="let ",_tag_dyneither(_tmp986,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp984,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL331;case 7: _LL340: _tmp45C=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL341:
# 1876
{const char*_tmp989;struct Cyc_PP_Doc*_tmp988[2];s=((_tmp988[1]=Cyc_PP_text(((_tmp989=";",_tag_dyneither(_tmp989,sizeof(char),2)))),((_tmp988[0]=Cyc_Absynpp_enumdecl2doc(_tmp45C),Cyc_PP_cat(_tag_dyneither(_tmp988,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL331;case 8: _LL342: _tmp45B=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL343: {
# 1879
void*t;
if(_tmp45B->defn != 0)
t=(void*)_check_null(_tmp45B->defn);else{
# 1883
t=Cyc_Absyn_new_evar(_tmp45B->kind,0);}
{const char*_tmp993;struct Cyc_Core_Opt*_tmp992;struct Cyc_PP_Doc*_tmp991[2];const char*_tmp990;struct Cyc_PP_Doc*_tmp98F[4];s=((_tmp98F[3]=
# 1891
Cyc_PP_text(((_tmp990=";",_tag_dyneither(_tmp990,sizeof(char),2)))),((_tmp98F[2]=
# 1890
Cyc_Absynpp_atts2doc(_tmp45B->atts),((_tmp98F[1]=
# 1885
Cyc_Absynpp_tqtd2doc(_tmp45B->tq,t,(
# 1887
(_tmp992=_cycalloc(sizeof(*_tmp992)),((_tmp992->v=((_tmp991[1]=
Cyc_Absynpp_tvars2doc(_tmp45B->tvs),((_tmp991[0]=
# 1887
Cyc_Absynpp_typedef_name2bolddoc(_tmp45B->name),Cyc_PP_cat(_tag_dyneither(_tmp991,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp992))))),((_tmp98F[0]=
# 1884
Cyc_PP_text(((_tmp993="typedef ",_tag_dyneither(_tmp993,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp98F,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1893
goto _LL331;}case 9: _LL344: _tmp459=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp45A=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp453)->f2;_LL345:
# 1895
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp459);
{const char*_tmp998;const char*_tmp997;struct Cyc_PP_Doc*_tmp996[8];s=((_tmp996[7]=
# 1902
Cyc_Absynpp_rb(),((_tmp996[6]=
# 1901
Cyc_PP_line_doc(),((_tmp996[5]=
# 1900
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmp997="",_tag_dyneither(_tmp997,sizeof(char),1))),_tmp45A),((_tmp996[4]=
# 1899
Cyc_PP_line_doc(),((_tmp996[3]=
# 1898
Cyc_Absynpp_lb(),((_tmp996[2]=Cyc_PP_blank_doc(),((_tmp996[1]=
# 1897
Cyc_PP_textptr(_tmp459),((_tmp996[0]=
# 1896
Cyc_PP_text(((_tmp998="namespace ",_tag_dyneither(_tmp998,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp996,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1903
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL331;case 10: _LL346: _tmp457=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp458=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp453)->f2;_LL347:
# 1906
 if(Cyc_Absynpp_print_using_stmts){
const char*_tmp99D;const char*_tmp99C;struct Cyc_PP_Doc*_tmp99B[8];s=((_tmp99B[7]=
# 1913
Cyc_Absynpp_rb(),((_tmp99B[6]=
# 1912
Cyc_PP_line_doc(),((_tmp99B[5]=
# 1911
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmp99C="",_tag_dyneither(_tmp99C,sizeof(char),1))),_tmp458),((_tmp99B[4]=
# 1910
Cyc_PP_line_doc(),((_tmp99B[3]=
# 1909
Cyc_Absynpp_lb(),((_tmp99B[2]=Cyc_PP_blank_doc(),((_tmp99B[1]=
# 1908
Cyc_Absynpp_qvar2doc(_tmp457),((_tmp99B[0]=
# 1907
Cyc_PP_text(((_tmp99D="using ",_tag_dyneither(_tmp99D,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp99B,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1915
const char*_tmp9A8;const char*_tmp9A7;const char*_tmp9A6;const char*_tmp9A5;const char*_tmp9A4;struct Cyc_PP_Doc*_tmp9A3[11];s=((_tmp9A3[10]=
# 1925
Cyc_PP_text(((_tmp9A4=" */",_tag_dyneither(_tmp9A4,sizeof(char),4)))),((_tmp9A3[9]=
# 1924
Cyc_Absynpp_rb(),((_tmp9A3[8]=
# 1923
Cyc_PP_text(((_tmp9A5="/* ",_tag_dyneither(_tmp9A5,sizeof(char),4)))),((_tmp9A3[7]=
# 1922
Cyc_PP_line_doc(),((_tmp9A3[6]=
# 1921
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmp9A6="",_tag_dyneither(_tmp9A6,sizeof(char),1))),_tmp458),((_tmp9A3[5]=
# 1920
Cyc_PP_line_doc(),((_tmp9A3[4]=
# 1919
Cyc_PP_text(((_tmp9A7=" */",_tag_dyneither(_tmp9A7,sizeof(char),4)))),((_tmp9A3[3]=
# 1918
Cyc_Absynpp_lb(),((_tmp9A3[2]=
# 1917
Cyc_PP_blank_doc(),((_tmp9A3[1]=
# 1916
Cyc_Absynpp_qvar2doc(_tmp457),((_tmp9A3[0]=
# 1915
Cyc_PP_text(((_tmp9A8="/* using ",_tag_dyneither(_tmp9A8,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp9A3,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1926
goto _LL331;case 11: _LL348: _tmp456=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL349:
# 1928
 if(Cyc_Absynpp_print_externC_stmts){
const char*_tmp9AD;const char*_tmp9AC;struct Cyc_PP_Doc*_tmp9AB[6];s=((_tmp9AB[5]=
# 1934
Cyc_Absynpp_rb(),((_tmp9AB[4]=
# 1933
Cyc_PP_line_doc(),((_tmp9AB[3]=
# 1932
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmp9AC="",_tag_dyneither(_tmp9AC,sizeof(char),1))),_tmp456),((_tmp9AB[2]=
# 1931
Cyc_PP_line_doc(),((_tmp9AB[1]=
# 1930
Cyc_Absynpp_lb(),((_tmp9AB[0]=
# 1929
Cyc_PP_text(((_tmp9AD="extern \"C\" ",_tag_dyneither(_tmp9AD,sizeof(char),12)))),Cyc_PP_cat(_tag_dyneither(_tmp9AB,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}else{
# 1936
const char*_tmp9B8;const char*_tmp9B7;const char*_tmp9B6;const char*_tmp9B5;const char*_tmp9B4;struct Cyc_PP_Doc*_tmp9B3[9];s=((_tmp9B3[8]=
# 1944
Cyc_PP_text(((_tmp9B4=" */",_tag_dyneither(_tmp9B4,sizeof(char),4)))),((_tmp9B3[7]=
# 1943
Cyc_Absynpp_rb(),((_tmp9B3[6]=
# 1942
Cyc_PP_text(((_tmp9B5="/* ",_tag_dyneither(_tmp9B5,sizeof(char),4)))),((_tmp9B3[5]=
# 1941
Cyc_PP_line_doc(),((_tmp9B3[4]=
# 1940
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmp9B6="",_tag_dyneither(_tmp9B6,sizeof(char),1))),_tmp456),((_tmp9B3[3]=
# 1939
Cyc_PP_line_doc(),((_tmp9B3[2]=
# 1938
Cyc_PP_text(((_tmp9B7=" */",_tag_dyneither(_tmp9B7,sizeof(char),4)))),((_tmp9B3[1]=
# 1937
Cyc_Absynpp_lb(),((_tmp9B3[0]=
# 1936
Cyc_PP_text(((_tmp9B8="/* extern \"C\" ",_tag_dyneither(_tmp9B8,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp9B3,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1945
goto _LL331;case 12: _LL34A: _tmp454=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp455=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp453)->f2;_LL34B:
# 1947
 if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
if(_tmp455 != 0){
const char*_tmp9BD;const char*_tmp9BC;struct Cyc_PP_Doc*_tmp9BB[7];exs_doc=((_tmp9BB[6]=
# 1952
Cyc_Absynpp_rb(),((_tmp9BB[5]=Cyc_PP_line_doc(),((_tmp9BB[4]=
# 1951
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,((_tmp9BC=",",_tag_dyneither(_tmp9BC,sizeof(char),2))),_tmp455),((_tmp9BB[3]=Cyc_PP_line_doc(),((_tmp9BB[2]=
# 1950
Cyc_Absynpp_lb(),((_tmp9BB[1]=Cyc_PP_text(((_tmp9BD=" export ",_tag_dyneither(_tmp9BD,sizeof(char),9)))),((_tmp9BB[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmp9BB,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1954
exs_doc=Cyc_Absynpp_rb();}{
const char*_tmp9C2;const char*_tmp9C1;struct Cyc_PP_Doc*_tmp9C0[6];s=((_tmp9C0[5]=exs_doc,((_tmp9C0[4]=
# 1959
Cyc_PP_line_doc(),((_tmp9C0[3]=
# 1958
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmp9C1="",_tag_dyneither(_tmp9C1,sizeof(char),1))),_tmp454),((_tmp9C0[2]=
# 1957
Cyc_PP_line_doc(),((_tmp9C0[1]=
# 1956
Cyc_Absynpp_lb(),((_tmp9C0[0]=
# 1955
Cyc_PP_text(((_tmp9C2="extern \"C include\" ",_tag_dyneither(_tmp9C2,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmp9C0,sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{
# 1962
const char*_tmp9CD;const char*_tmp9CC;const char*_tmp9CB;const char*_tmp9CA;const char*_tmp9C9;struct Cyc_PP_Doc*_tmp9C8[9];s=((_tmp9C8[8]=
# 1970
Cyc_PP_text(((_tmp9C9=" */",_tag_dyneither(_tmp9C9,sizeof(char),4)))),((_tmp9C8[7]=
# 1969
Cyc_Absynpp_rb(),((_tmp9C8[6]=
# 1968
Cyc_PP_text(((_tmp9CA="/* ",_tag_dyneither(_tmp9CA,sizeof(char),4)))),((_tmp9C8[5]=
# 1967
Cyc_PP_line_doc(),((_tmp9C8[4]=
# 1966
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmp9CB="",_tag_dyneither(_tmp9CB,sizeof(char),1))),_tmp454),((_tmp9C8[3]=
# 1965
Cyc_PP_line_doc(),((_tmp9C8[2]=
# 1964
Cyc_PP_text(((_tmp9CC=" */",_tag_dyneither(_tmp9CC,sizeof(char),4)))),((_tmp9C8[1]=
# 1963
Cyc_Absynpp_lb(),((_tmp9C8[0]=
# 1962
Cyc_PP_text(((_tmp9CD="/* extern \"C include\" ",_tag_dyneither(_tmp9CD,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmp9C8,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1971
goto _LL331;case 13: _LL34C: _LL34D:
# 1973
{const char*_tmp9D0;struct Cyc_PP_Doc*_tmp9CF[2];s=((_tmp9CF[1]=Cyc_Absynpp_lb(),((_tmp9CF[0]=Cyc_PP_text(((_tmp9D0="__cyclone_port_on__;",_tag_dyneither(_tmp9D0,sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmp9CF,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL331;default: _LL34E: _LL34F:
# 1976
{const char*_tmp9D3;struct Cyc_PP_Doc*_tmp9D2[2];s=((_tmp9D2[1]=Cyc_Absynpp_lb(),((_tmp9D2[0]=Cyc_PP_text(((_tmp9D3="__cyclone_port_off__;",_tag_dyneither(_tmp9D3,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmp9D2,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL331;}_LL331:;}
# 1980
return s;}
# 1983
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();{
enum Cyc_Absyn_Scope _tmp4B6=sc;switch(_tmp4B6){case Cyc_Absyn_Static: _LL360: _LL361: {
const char*_tmp9D4;return Cyc_PP_text(((_tmp9D4="static ",_tag_dyneither(_tmp9D4,sizeof(char),8))));}case Cyc_Absyn_Public: _LL362: _LL363:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL364: _LL365: {
const char*_tmp9D5;return Cyc_PP_text(((_tmp9D5="extern ",_tag_dyneither(_tmp9D5,sizeof(char),8))));}case Cyc_Absyn_ExternC: _LL366: _LL367: {
const char*_tmp9D6;return Cyc_PP_text(((_tmp9D6="extern \"C\" ",_tag_dyneither(_tmp9D6,sizeof(char),12))));}case Cyc_Absyn_Abstract: _LL368: _LL369: {
const char*_tmp9D7;return Cyc_PP_text(((_tmp9D7="abstract ",_tag_dyneither(_tmp9D7,sizeof(char),10))));}default: _LL36A: _LL36B: {
const char*_tmp9D8;return Cyc_PP_text(((_tmp9D8="register ",_tag_dyneither(_tmp9D8,sizeof(char),10))));}}_LL35F:;};}
# 1996
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp4BC=t;struct Cyc_List_List*_tmp4BD;struct Cyc_Absyn_Tvar*_tmp4BE;switch(*((int*)_tmp4BC)){case 2: _LL36D: _tmp4BE=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4BC)->f1;_LL36E:
 return Cyc_Tcutil_is_temp_tvar(_tmp4BE);case 24: _LL36F: _tmp4BD=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4BC)->f1;_LL370:
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp4BD);default: _LL371: _LL372:
 return 0;}_LL36C:;}
# 2008
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp4BF=t;struct Cyc_Absyn_Typedefdecl*_tmp4C0;void*_tmp4C1;switch(*((int*)_tmp4BF)){case 12: _LL374: _LL375:
 return 1;case 14: _LL376: _LL377:
 return 1;case 17: _LL378: _tmp4C0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4BF)->f3;_tmp4C1=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4BF)->f4;if(_tmp4C1 != 0){_LL379:
# 2015
 return Cyc_Absynpp_is_anon_aggrtype(_tmp4C1);}else{goto _LL37A;}default: _LL37A: _LL37B:
 return 0;}_LL373:;}
# 2022
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 2025
if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmp9D9;struct _tuple16 _tmp4C2=(_tmp9D9.f1=(void*)tms->hd,((_tmp9D9.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmp9D9)));struct _tuple16 _tmp4C3=_tmp4C2;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp4C3.f1)->tag == 2){if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp4C3.f2)->tag == 3){_LL37D: _LL37E: {
# 2028
struct Cyc_List_List*_tmp9DC;struct Cyc_List_List*_tmp9DB;return(_tmp9DB=_region_malloc(r,sizeof(*_tmp9DB)),((_tmp9DB->hd=(void*)tms->hd,((_tmp9DB->tl=((_tmp9DC=_region_malloc(r,sizeof(*_tmp9DC)),((_tmp9DC->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,((_tmp9DC->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmp9DC)))))),_tmp9DB)))));}}else{goto _LL37F;}}else{_LL37F: _LL380: {
struct Cyc_List_List*_tmp9DD;return(_tmp9DD=_region_malloc(r,sizeof(*_tmp9DD)),((_tmp9DD->hd=atts,((_tmp9DD->tl=tms,_tmp9DD)))));}}_LL37C:;}else{
# 2031
struct Cyc_List_List*_tmp9DE;return(_tmp9DE=_region_malloc(r,sizeof(*_tmp9DE)),((_tmp9DE->hd=atts,((_tmp9DE->tl=tms,_tmp9DE)))));}}
# 2036
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2038
void*_tmp4C9=t;struct _tuple0*_tmp4CA;struct Cyc_List_List*_tmp4CB;struct Cyc_Absyn_Typedefdecl*_tmp4CC;void*_tmp4CD;struct Cyc_Core_Opt*_tmp4CE;void*_tmp4CF;int _tmp4D0;struct Cyc_List_List*_tmp4D1;void*_tmp4D2;struct Cyc_Absyn_Tqual _tmp4D3;void*_tmp4D4;struct Cyc_List_List*_tmp4D5;int _tmp4D6;struct Cyc_Absyn_VarargInfo*_tmp4D7;struct Cyc_List_List*_tmp4D8;struct Cyc_List_List*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4DB;void*_tmp4DC;struct Cyc_Absyn_Tqual _tmp4DD;struct Cyc_Absyn_PtrAtts _tmp4DE;void*_tmp4DF;struct Cyc_Absyn_Tqual _tmp4E0;struct Cyc_Absyn_Exp*_tmp4E1;union Cyc_Absyn_Constraint*_tmp4E2;unsigned int _tmp4E3;switch(*((int*)_tmp4C9)){case 8: _LL382: _tmp4DF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C9)->f1).elt_type;_tmp4E0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C9)->f1).tq;_tmp4E1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C9)->f1).num_elts;_tmp4E2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C9)->f1).zero_term;_tmp4E3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C9)->f1).zt_loc;_LL383: {
# 2041
struct _tuple12 _tmp4E4=Cyc_Absynpp_to_tms(r,_tmp4E0,_tmp4DF);struct _tuple12 _tmp4E5=_tmp4E4;struct Cyc_Absyn_Tqual _tmp4E6;void*_tmp4E7;struct Cyc_List_List*_tmp4E8;_LL38F: _tmp4E6=_tmp4E5.f1;_tmp4E7=_tmp4E5.f2;_tmp4E8=_tmp4E5.f3;_LL390: {
void*tm;
if(_tmp4E1 == 0){
struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp9E1;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp9E0;tm=(void*)((_tmp9E0=_region_malloc(r,sizeof(*_tmp9E0)),((_tmp9E0[0]=((_tmp9E1.tag=0,((_tmp9E1.f1=_tmp4E2,((_tmp9E1.f2=_tmp4E3,_tmp9E1)))))),_tmp9E0))));}else{
# 2046
struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp9E4;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp9E3;tm=(void*)((_tmp9E3=_region_malloc(r,sizeof(*_tmp9E3)),((_tmp9E3[0]=((_tmp9E4.tag=1,((_tmp9E4.f1=_tmp4E1,((_tmp9E4.f2=_tmp4E2,((_tmp9E4.f3=_tmp4E3,_tmp9E4)))))))),_tmp9E3))));}{
struct Cyc_List_List*_tmp9E7;struct _tuple12 _tmp9E6;return(_tmp9E6.f1=_tmp4E6,((_tmp9E6.f2=_tmp4E7,((_tmp9E6.f3=((_tmp9E7=_region_malloc(r,sizeof(*_tmp9E7)),((_tmp9E7->hd=tm,((_tmp9E7->tl=_tmp4E8,_tmp9E7)))))),_tmp9E6)))));};}_LL38E:;}case 5: _LL384: _tmp4DC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C9)->f1).elt_typ;_tmp4DD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C9)->f1).elt_tq;_tmp4DE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C9)->f1).ptr_atts;_LL385: {
# 2050
struct _tuple12 _tmp4EF=Cyc_Absynpp_to_tms(r,_tmp4DD,_tmp4DC);struct _tuple12 _tmp4F0=_tmp4EF;struct Cyc_Absyn_Tqual _tmp4F1;void*_tmp4F2;struct Cyc_List_List*_tmp4F3;_LL394: _tmp4F1=_tmp4F0.f1;_tmp4F2=_tmp4F0.f2;_tmp4F3=_tmp4F0.f3;_LL395:
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp9ED;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmp9EC;struct Cyc_List_List*_tmp9EB;_tmp4F3=((_tmp9EB=_region_malloc(r,sizeof(*_tmp9EB)),((_tmp9EB->hd=(void*)((_tmp9ED=_region_malloc(r,sizeof(*_tmp9ED)),((_tmp9ED[0]=((_tmp9EC.tag=2,((_tmp9EC.f1=_tmp4DE,((_tmp9EC.f2=tq,_tmp9EC)))))),_tmp9ED)))),((_tmp9EB->tl=_tmp4F3,_tmp9EB))))));}{
struct _tuple12 _tmp9EE;return(_tmp9EE.f1=_tmp4F1,((_tmp9EE.f2=_tmp4F2,((_tmp9EE.f3=_tmp4F3,_tmp9EE)))));};_LL393:;}case 9: _LL386: _tmp4D1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).tvars;_tmp4D2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).effect;_tmp4D3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).ret_tqual;_tmp4D4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).ret_typ;_tmp4D5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).args;_tmp4D6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).c_varargs;_tmp4D7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).cyc_varargs;_tmp4D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).rgn_po;_tmp4D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).attributes;_tmp4DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).requires_clause;_tmp4DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).ensures_clause;_LL387:
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
struct _tuple12 _tmp4F8=Cyc_Absynpp_to_tms(r,_tmp4D3,_tmp4D4);struct _tuple12 _tmp4F9=_tmp4F8;struct Cyc_Absyn_Tqual _tmp4FA;void*_tmp4FB;struct Cyc_List_List*_tmp4FC;_LL399: _tmp4FA=_tmp4F9.f1;_tmp4FB=_tmp4F9.f2;_tmp4FC=_tmp4F9.f3;_LL39A: {
struct Cyc_List_List*tms=_tmp4FC;
# 2084 "absynpp.cyc"
{enum Cyc_Cyclone_C_Compilers _tmp4FD=Cyc_Cyclone_c_compiler;if(_tmp4FD == Cyc_Cyclone_Gcc_c){_LL39E: _LL39F:
# 2086
 if(_tmp4D9 != 0){
# 2089
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp9F1;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp9F0;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmp9F0=_region_malloc(r,sizeof(*_tmp9F0)),((_tmp9F0[0]=((_tmp9F1.tag=5,((_tmp9F1.f1=0,((_tmp9F1.f2=_tmp4D9,_tmp9F1)))))),_tmp9F0)))),tms);}
# 2091
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpA00;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp9FF;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp9FE;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp9FD;struct Cyc_List_List*_tmp9FC;tms=((_tmp9FC=_region_malloc(r,sizeof(*_tmp9FC)),((_tmp9FC->hd=(void*)((_tmpA00=_region_malloc(r,sizeof(*_tmpA00)),((_tmpA00[0]=(
(_tmp9FD.tag=3,((_tmp9FD.f1=(void*)((_tmp9FF=_region_malloc(r,sizeof(*_tmp9FF)),((_tmp9FF[0]=((_tmp9FE.tag=1,((_tmp9FE.f1=_tmp4D5,((_tmp9FE.f2=_tmp4D6,((_tmp9FE.f3=_tmp4D7,((_tmp9FE.f4=_tmp4D2,((_tmp9FE.f5=_tmp4D8,((_tmp9FE.f6=_tmp4DA,((_tmp9FE.f7=_tmp4DB,_tmp9FE)))))))))))))))),_tmp9FF)))),_tmp9FD)))),_tmpA00)))),((_tmp9FC->tl=tms,_tmp9FC))))));}
# 2095
goto _LL39D;}else{_LL3A0: _LL3A1:
# 2097
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpA0F;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpA0E;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpA0D;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpA0C;struct Cyc_List_List*_tmpA0B;tms=((_tmpA0B=_region_malloc(r,sizeof(*_tmpA0B)),((_tmpA0B->hd=(void*)((_tmpA0F=_region_malloc(r,sizeof(*_tmpA0F)),((_tmpA0F[0]=(
(_tmpA0C.tag=3,((_tmpA0C.f1=(void*)((_tmpA0E=_region_malloc(r,sizeof(*_tmpA0E)),((_tmpA0E[0]=((_tmpA0D.tag=1,((_tmpA0D.f1=_tmp4D5,((_tmpA0D.f2=_tmp4D6,((_tmpA0D.f3=_tmp4D7,((_tmpA0D.f4=_tmp4D2,((_tmpA0D.f5=_tmp4D8,((_tmpA0D.f6=_tmp4DA,((_tmpA0D.f7=_tmp4DB,_tmpA0D)))))))))))))))),_tmpA0E)))),_tmpA0C)))),_tmpA0F)))),((_tmpA0B->tl=tms,_tmpA0B))))));}
# 2101
for(0;_tmp4D9 != 0;_tmp4D9=_tmp4D9->tl){
void*_tmp50A=(void*)_tmp4D9->hd;void*_tmp50B=_tmp50A;switch(*((int*)_tmp50B)){case 1: _LL3A3: _LL3A4:
 goto _LL3A6;case 2: _LL3A5: _LL3A6:
 goto _LL3A8;case 3: _LL3A7: _LL3A8:
# 2106
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpA19;struct Cyc_List_List*_tmpA18;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpA17;struct Cyc_List_List*_tmpA16;tms=((_tmpA16=_region_malloc(r,sizeof(*_tmpA16)),((_tmpA16->hd=(void*)((_tmpA19=_region_malloc(r,sizeof(*_tmpA19)),((_tmpA19[0]=((_tmpA17.tag=5,((_tmpA17.f1=0,((_tmpA17.f2=((_tmpA18=_cycalloc(sizeof(*_tmpA18)),((_tmpA18->hd=(void*)_tmp4D9->hd,((_tmpA18->tl=0,_tmpA18)))))),_tmpA17)))))),_tmpA19)))),((_tmpA16->tl=tms,_tmpA16))))));}
goto AfterAtts;default: _LL3A9: _LL3AA:
 goto _LL3A2;}_LL3A2:;}
# 2110
goto _LL39D;}_LL39D:;}
# 2113
AfterAtts:
 if(_tmp4D1 != 0){
struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpA1F;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpA1E;struct Cyc_List_List*_tmpA1D;tms=((_tmpA1D=_region_malloc(r,sizeof(*_tmpA1D)),((_tmpA1D->hd=(void*)((_tmpA1F=_region_malloc(r,sizeof(*_tmpA1F)),((_tmpA1F[0]=((_tmpA1E.tag=4,((_tmpA1E.f1=_tmp4D1,((_tmpA1E.f2=0,((_tmpA1E.f3=1,_tmpA1E)))))))),_tmpA1F)))),((_tmpA1D->tl=tms,_tmpA1D))))));}{
struct _tuple12 _tmpA20;return(_tmpA20.f1=_tmp4FA,((_tmpA20.f2=_tmp4FB,((_tmpA20.f3=tms,_tmpA20)))));};}_LL398:;};case 1: _LL388: _tmp4CE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4C9)->f1;_tmp4CF=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4C9)->f2;_tmp4D0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4C9)->f3;_LL389:
# 2119
 if(_tmp4CF == 0){
struct _tuple12 _tmpA21;return(_tmpA21.f1=tq,((_tmpA21.f2=t,((_tmpA21.f3=0,_tmpA21)))));}else{
# 2122
return Cyc_Absynpp_to_tms(r,tq,_tmp4CF);}case 17: _LL38A: _tmp4CA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f1;_tmp4CB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f2;_tmp4CC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f3;_tmp4CD=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f4;_LL38B:
# 2127
 if((_tmp4CD == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(_tmp4CD)){
struct _tuple12 _tmpA22;return(_tmpA22.f1=tq,((_tmpA22.f2=t,((_tmpA22.f3=0,_tmpA22)))));}else{
# 2130
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,_tmp4CD);}default: _LL38C: _LL38D: {
# 2135
struct _tuple12 _tmpA23;return(_tmpA23.f1=tq,((_tmpA23.f2=t,((_tmpA23.f3=0,_tmpA23)))));}}_LL381:;}
# 2139
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2141
void*_tmp517=t;void*_tmp518;void*_tmp519;switch(*((int*)_tmp517)){case 1: _LL3AC: _tmp519=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp517)->f2;if(_tmp519 != 0){_LL3AD:
 return Cyc_Absynpp_is_char_ptr(_tmp519);}else{goto _LL3B0;}case 5: _LL3AE: _tmp518=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp517)->f1).elt_typ;_LL3AF:
# 2144
 L: {
void*_tmp51A=_tmp518;void*_tmp51B;void*_tmp51C;switch(*((int*)_tmp51A)){case 1: _LL3B3: _tmp51C=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp51A)->f2;if(_tmp51C != 0){_LL3B4:
 _tmp518=_tmp51C;goto L;}else{goto _LL3B9;}case 17: _LL3B5: _tmp51B=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp51A)->f4;if(_tmp51B != 0){_LL3B6:
 _tmp518=_tmp51B;goto L;}else{goto _LL3B9;}case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp51A)->f2 == Cyc_Absyn_Char_sz){_LL3B7: _LL3B8:
 return 1;}else{goto _LL3B9;}default: _LL3B9: _LL3BA:
 return 0;}_LL3B2:;}default: _LL3B0: _LL3B1:
# 2151
 return 0;}_LL3AB:;}
# 2155
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp51D=_new_region("temp");struct _RegionHandle*temp=& _tmp51D;_push_region(temp);
{struct _tuple12 _tmp51E=Cyc_Absynpp_to_tms(temp,tq,typ);struct _tuple12 _tmp51F=_tmp51E;struct Cyc_Absyn_Tqual _tmp520;void*_tmp521;struct Cyc_List_List*_tmp522;_LL3BC: _tmp520=_tmp51F.f1;_tmp521=_tmp51F.f2;_tmp522=_tmp51F.f3;_LL3BD:
 _tmp522=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp522);
if(_tmp522 == 0  && dopt == 0){
struct Cyc_PP_Doc*_tmpA24[2];struct Cyc_PP_Doc*_tmp524=(_tmpA24[1]=Cyc_Absynpp_ntyp2doc(_tmp521),((_tmpA24[0]=Cyc_Absynpp_tqual2doc(_tmp520),Cyc_PP_cat(_tag_dyneither(_tmpA24,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return _tmp524;}else{
# 2162
const char*_tmpA27;struct Cyc_PP_Doc*_tmpA26[4];struct Cyc_PP_Doc*_tmp527=
(_tmpA26[3]=
# 2166
Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp522),((_tmpA26[2]=
# 2165
Cyc_PP_text(((_tmpA27=" ",_tag_dyneither(_tmpA27,sizeof(char),2)))),((_tmpA26[1]=
# 2164
Cyc_Absynpp_ntyp2doc(_tmp521),((_tmpA26[0]=
# 2163
Cyc_Absynpp_tqual2doc(_tmp520),Cyc_PP_cat(_tag_dyneither(_tmpA26,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp527;}_LL3BB:;}
# 2157
;_pop_region(temp);}
# 2170
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp528=f->requires_clause;
if((unsigned int)_tmp528){
const char*_tmpA2A;struct Cyc_PP_Doc*_tmpA29[2];requires_doc=((_tmpA29[1]=Cyc_Absynpp_exp2doc(_tmp528),((_tmpA29[0]=Cyc_PP_text(((_tmpA2A="@requires ",_tag_dyneither(_tmpA2A,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmpA29,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 2176
requires_doc=Cyc_PP_nil_doc();}{
# 2178
enum Cyc_Cyclone_C_Compilers _tmp52B=Cyc_Cyclone_c_compiler;if(_tmp52B == Cyc_Cyclone_Gcc_c){_LL3C1: _LL3C2:
# 2181
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpA31;const char*_tmpA30;const char*_tmpA2F;struct Cyc_PP_Doc*_tmpA2E[5];return(_tmpA2E[4]=
# 2184
Cyc_PP_text(((_tmpA2F=";",_tag_dyneither(_tmpA2F,sizeof(char),2)))),((_tmpA2E[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpA2E[2]=
# 2183
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpA2E[1]=Cyc_PP_text(((_tmpA30=":",_tag_dyneither(_tmpA30,sizeof(char),2)))),((_tmpA2E[0]=
# 2182
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA31=_cycalloc(sizeof(*_tmpA31)),((_tmpA31->v=Cyc_PP_textptr(f->name),_tmpA31))))),Cyc_PP_cat(_tag_dyneither(_tmpA2E,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2186
struct Cyc_Core_Opt*_tmpA36;const char*_tmpA35;struct Cyc_PP_Doc*_tmpA34[4];return(_tmpA34[3]=
Cyc_PP_text(((_tmpA35=";",_tag_dyneither(_tmpA35,sizeof(char),2)))),((_tmpA34[2]=requires_doc,((_tmpA34[1]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpA34[0]=
# 2186
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA36=_cycalloc(sizeof(*_tmpA36)),((_tmpA36->v=Cyc_PP_textptr(f->name),_tmpA36))))),Cyc_PP_cat(_tag_dyneither(_tmpA34,sizeof(struct Cyc_PP_Doc*),4)))))))));}}else{_LL3C3: _LL3C4:
# 2189
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpA3D;const char*_tmpA3C;const char*_tmpA3B;struct Cyc_PP_Doc*_tmpA3A[5];return(_tmpA3A[4]=
# 2192
Cyc_PP_text(((_tmpA3B=";",_tag_dyneither(_tmpA3B,sizeof(char),2)))),((_tmpA3A[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpA3A[2]=Cyc_PP_text(((_tmpA3C=":",_tag_dyneither(_tmpA3C,sizeof(char),2)))),((_tmpA3A[1]=
# 2191
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA3D=_cycalloc(sizeof(*_tmpA3D)),((_tmpA3D->v=Cyc_PP_textptr(f->name),_tmpA3D))))),((_tmpA3A[0]=
# 2190
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpA3A,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2194
struct Cyc_Core_Opt*_tmpA42;const char*_tmpA41;struct Cyc_PP_Doc*_tmpA40[4];return(_tmpA40[3]=
# 2196
Cyc_PP_text(((_tmpA41=";",_tag_dyneither(_tmpA41,sizeof(char),2)))),((_tmpA40[2]=requires_doc,((_tmpA40[1]=
# 2195
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA42=_cycalloc(sizeof(*_tmpA42)),((_tmpA42->v=Cyc_PP_textptr(f->name),_tmpA42))))),((_tmpA40[0]=
# 2194
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpA40,sizeof(struct Cyc_PP_Doc*),4)))))))));}}_LL3C0:;};}
# 2201
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
const char*_tmpA43;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmpA43="",_tag_dyneither(_tmpA43,sizeof(char),1))),fields);}
# 2205
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
struct Cyc_PP_Doc*_tmpA44[3];return(_tmpA44[2]=
f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmpA44[1]=
# 2206
Cyc_Absynpp_typedef_name2doc(f->name),((_tmpA44[0]=Cyc_Absynpp_scope2doc(f->sc),Cyc_PP_cat(_tag_dyneither(_tmpA44,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 2210
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
const char*_tmpA45;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmpA45=",",_tag_dyneither(_tmpA45,sizeof(char),2))),fields);}
# 2219
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{
const char*_tmpA48;void*_tmpA47;(_tmpA47=0,Cyc_fprintf(f,((_tmpA48="\n",_tag_dyneither(_tmpA48,sizeof(char),2))),_tag_dyneither(_tmpA47,sizeof(void*),0)));};}}
# 2226
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
const char*_tmpA49;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpA49="",_tag_dyneither(_tmpA49,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),72);}
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
