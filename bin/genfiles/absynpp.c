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
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&
      _curr != (unsigned char *)0)
    _throw_arraybounds_fn(filename,lineno);
  return _curr;
}
#define _untag_dyneither_ptr(a,s,e) \
  _untag_dyneither_ptr_fn(a,s,e,__FILE__,__LINE__)
#else
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&\
      _curr != (unsigned char *)0) \
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
return dyn.curr;};}struct Cyc_Buffer_t;
# 46 "buffer.h"
typedef struct Cyc_Buffer_t*Cyc_Buffer_T;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
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
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;struct Cyc_Relations_Reln;
# 69 "absyn.h"
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 75
typedef void*Cyc_Tcpat_decision_opt_t;
# 83
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 102
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
# 107
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
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 191
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 211
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 213
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 222
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 368
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 373
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 444 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 511
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 537
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 544
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 562
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 725 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 928
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 938
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 946
void*Cyc_Absyn_compress_kb(void*);
# 951
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 981
extern void*Cyc_Absyn_bounds_one;
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1063
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1168
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1174
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1186
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct _tuple11{unsigned int f1;int f2;};
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
# 30 "rgnorder.h"
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
# 32
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
# 39
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 45
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 48
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};
# 38 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 49
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 65
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 69
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 80
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 114 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 64 "tcutil.h"
void*Cyc_Tcutil_compress(void*t);
# 306 "tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 308
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
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
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 51 "absynpp.h"
extern int Cyc_Absynpp_print_for_cycdoc;
# 53
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 57
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 59
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);
# 61
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
# 76
int Cyc_Absynpp_is_anon_aggrtype(void*t);
extern struct _dyneither_ptr Cyc_Absynpp_cyc_string;
extern struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
# 86
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);
# 26 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 38 "absynpp.cyc"
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
# 59
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields);
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad);
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ad);
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*ad);
# 66
static int Cyc_Absynpp_expand_typedefs;
# 70
static int Cyc_Absynpp_qvar_to_Cids;static char _tmp0[4]="Cyc";
# 72
struct _dyneither_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,_tmp0 + 4};
struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
# 75
static int Cyc_Absynpp_add_cyc_prefix;
# 79
static int Cyc_Absynpp_to_VC;
# 82
static int Cyc_Absynpp_decls_first;
# 86
static int Cyc_Absynpp_rewrite_temp_tvars;
# 89
static int Cyc_Absynpp_print_all_tvars;
# 92
static int Cyc_Absynpp_print_all_kinds;
# 95
static int Cyc_Absynpp_print_all_effects;
# 98
static int Cyc_Absynpp_print_using_stmts;
# 103
static int Cyc_Absynpp_print_externC_stmts;
# 107
static int Cyc_Absynpp_print_full_evars;
# 110
static int Cyc_Absynpp_generate_line_directives;
# 113
static int Cyc_Absynpp_use_curr_namespace;
# 116
static int Cyc_Absynpp_print_zeroterm;
# 119
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;
# 122
int Cyc_Absynpp_print_for_cycdoc=0;struct Cyc_Absynpp_Params;
# 143
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
# 162
struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 182
struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,1,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 202
struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,0,1,0,0,0,0,0,0,0,0,1,0,0};
# 222
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,0,0};
# 243
static void Cyc_Absynpp_curr_namespace_add(struct _dyneither_ptr*v){
Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v;_tmp1->tl=0;_tmp1;}));}
# 247
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){
if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)
*l=0;else{
# 251
Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
# 255
static void Cyc_Absynpp_curr_namespace_drop(){
((void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
# 259
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
# 272
 if(c >= ' '  && c <= '~'){
struct _dyneither_ptr _tmpD=Cyc_Core_new_string(2);
({struct _dyneither_ptr _tmpE=_dyneither_ptr_plus(_tmpD,sizeof(char),0);char _tmpF=*((char*)_check_dyneither_subscript(_tmpE,sizeof(char),0));char _tmp10=c;if(_get_dyneither_size(_tmpE,sizeof(char))== 1  && (_tmpF == '\000'  && _tmp10 != '\000'))_throw_arraybounds();*((char*)_tmpE.curr)=_tmp10;});
return(struct _dyneither_ptr)_tmpD;}else{
# 277
struct _dyneither_ptr _tmp11=Cyc_Core_new_string(5);
int j=0;
({struct _dyneither_ptr _tmp12=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp13=*((char*)_check_dyneither_subscript(_tmp12,sizeof(char),0));char _tmp14='\\';if(_get_dyneither_size(_tmp12,sizeof(char))== 1  && (_tmp13 == '\000'  && _tmp14 != '\000'))_throw_arraybounds();*((char*)_tmp12.curr)=_tmp14;});
({struct _dyneither_ptr _tmp15=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp16=*((char*)_check_dyneither_subscript(_tmp15,sizeof(char),0));char _tmp17=(char)('0' + ((unsigned char)c >> 6 & 3));if(_get_dyneither_size(_tmp15,sizeof(char))== 1  && (_tmp16 == '\000'  && _tmp17 != '\000'))_throw_arraybounds();*((char*)_tmp15.curr)=_tmp17;});
({struct _dyneither_ptr _tmp18=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp19=*((char*)_check_dyneither_subscript(_tmp18,sizeof(char),0));char _tmp1A=(char)('0' + (c >> 3 & 7));if(_get_dyneither_size(_tmp18,sizeof(char))== 1  && (_tmp19 == '\000'  && _tmp1A != '\000'))_throw_arraybounds();*((char*)_tmp18.curr)=_tmp1A;});
({struct _dyneither_ptr _tmp1B=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp1C=*((char*)_check_dyneither_subscript(_tmp1B,sizeof(char),0));char _tmp1D=(char)('0' + (c & 7));if(_get_dyneither_size(_tmp1B,sizeof(char))== 1  && (_tmp1C == '\000'  && _tmp1D != '\000'))_throw_arraybounds();*((char*)_tmp1B.curr)=_tmp1D;});
return(struct _dyneither_ptr)_tmp11;}}_LL0:;}
# 288
static int Cyc_Absynpp_special(struct _dyneither_ptr s){
int sz=(int)(_get_dyneither_size(s,sizeof(char))- 1);
{int i=0;for(0;i < sz;++ i){
char c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(((c <= ' '  || c >= '~') || c == '"') || c == '\\')
return 1;}}
# 295
return 0;}
# 298
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr s){
if(!Cyc_Absynpp_special(s))return s;{
# 301
int n=(int)(_get_dyneither_size(s,sizeof(char))- 1);
# 303
if(n > 0  && *((const char*)_check_dyneither_subscript(s,sizeof(char),n))== '\000')-- n;{
# 305
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
# 318
 if(_tmp20 >= ' '  && _tmp20 <= '~')++ len;else{
len +=4;}
goto _LL17;}_LL17:;}}{
# 323
struct _dyneither_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));
int j=0;
{int i=0;for(0;i <= n;++ i){
char _tmp21=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));char _tmp22=_tmp21;char _tmp68;switch(_tmp22){case 7: _LL2D: _LL2E:
({struct _dyneither_ptr _tmp23=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp24=*((char*)_check_dyneither_subscript(_tmp23,sizeof(char),0));char _tmp25='\\';if(_get_dyneither_size(_tmp23,sizeof(char))== 1  && (_tmp24 == '\000'  && _tmp25 != '\000'))_throw_arraybounds();*((char*)_tmp23.curr)=_tmp25;});({struct _dyneither_ptr _tmp26=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp27=*((char*)_check_dyneither_subscript(_tmp26,sizeof(char),0));char _tmp28='a';if(_get_dyneither_size(_tmp26,sizeof(char))== 1  && (_tmp27 == '\000'  && _tmp28 != '\000'))_throw_arraybounds();*((char*)_tmp26.curr)=_tmp28;});goto _LL2C;case 8: _LL2F: _LL30:
({struct _dyneither_ptr _tmp29=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp2A=*((char*)_check_dyneither_subscript(_tmp29,sizeof(char),0));char _tmp2B='\\';if(_get_dyneither_size(_tmp29,sizeof(char))== 1  && (_tmp2A == '\000'  && _tmp2B != '\000'))_throw_arraybounds();*((char*)_tmp29.curr)=_tmp2B;});({struct _dyneither_ptr _tmp2C=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp2D=*((char*)_check_dyneither_subscript(_tmp2C,sizeof(char),0));char _tmp2E='b';if(_get_dyneither_size(_tmp2C,sizeof(char))== 1  && (_tmp2D == '\000'  && _tmp2E != '\000'))_throw_arraybounds();*((char*)_tmp2C.curr)=_tmp2E;});goto _LL2C;case 12: _LL31: _LL32:
({struct _dyneither_ptr _tmp2F=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp30=*((char*)_check_dyneither_subscript(_tmp2F,sizeof(char),0));char _tmp31='\\';if(_get_dyneither_size(_tmp2F,sizeof(char))== 1  && (_tmp30 == '\000'  && _tmp31 != '\000'))_throw_arraybounds();*((char*)_tmp2F.curr)=_tmp31;});({struct _dyneither_ptr _tmp32=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp33=*((char*)_check_dyneither_subscript(_tmp32,sizeof(char),0));char _tmp34='f';if(_get_dyneither_size(_tmp32,sizeof(char))== 1  && (_tmp33 == '\000'  && _tmp34 != '\000'))_throw_arraybounds();*((char*)_tmp32.curr)=_tmp34;});goto _LL2C;case 10: _LL33: _LL34:
({struct _dyneither_ptr _tmp35=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp36=*((char*)_check_dyneither_subscript(_tmp35,sizeof(char),0));char _tmp37='\\';if(_get_dyneither_size(_tmp35,sizeof(char))== 1  && (_tmp36 == '\000'  && _tmp37 != '\000'))_throw_arraybounds();*((char*)_tmp35.curr)=_tmp37;});({struct _dyneither_ptr _tmp38=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp39=*((char*)_check_dyneither_subscript(_tmp38,sizeof(char),0));char _tmp3A='n';if(_get_dyneither_size(_tmp38,sizeof(char))== 1  && (_tmp39 == '\000'  && _tmp3A != '\000'))_throw_arraybounds();*((char*)_tmp38.curr)=_tmp3A;});goto _LL2C;case 13: _LL35: _LL36:
({struct _dyneither_ptr _tmp3B=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp3C=*((char*)_check_dyneither_subscript(_tmp3B,sizeof(char),0));char _tmp3D='\\';if(_get_dyneither_size(_tmp3B,sizeof(char))== 1  && (_tmp3C == '\000'  && _tmp3D != '\000'))_throw_arraybounds();*((char*)_tmp3B.curr)=_tmp3D;});({struct _dyneither_ptr _tmp3E=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp3F=*((char*)_check_dyneither_subscript(_tmp3E,sizeof(char),0));char _tmp40='r';if(_get_dyneither_size(_tmp3E,sizeof(char))== 1  && (_tmp3F == '\000'  && _tmp40 != '\000'))_throw_arraybounds();*((char*)_tmp3E.curr)=_tmp40;});goto _LL2C;case 9: _LL37: _LL38:
({struct _dyneither_ptr _tmp41=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp42=*((char*)_check_dyneither_subscript(_tmp41,sizeof(char),0));char _tmp43='\\';if(_get_dyneither_size(_tmp41,sizeof(char))== 1  && (_tmp42 == '\000'  && _tmp43 != '\000'))_throw_arraybounds();*((char*)_tmp41.curr)=_tmp43;});({struct _dyneither_ptr _tmp44=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp45=*((char*)_check_dyneither_subscript(_tmp44,sizeof(char),0));char _tmp46='t';if(_get_dyneither_size(_tmp44,sizeof(char))== 1  && (_tmp45 == '\000'  && _tmp46 != '\000'))_throw_arraybounds();*((char*)_tmp44.curr)=_tmp46;});goto _LL2C;case 11: _LL39: _LL3A:
({struct _dyneither_ptr _tmp47=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp48=*((char*)_check_dyneither_subscript(_tmp47,sizeof(char),0));char _tmp49='\\';if(_get_dyneither_size(_tmp47,sizeof(char))== 1  && (_tmp48 == '\000'  && _tmp49 != '\000'))_throw_arraybounds();*((char*)_tmp47.curr)=_tmp49;});({struct _dyneither_ptr _tmp4A=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp4B=*((char*)_check_dyneither_subscript(_tmp4A,sizeof(char),0));char _tmp4C='v';if(_get_dyneither_size(_tmp4A,sizeof(char))== 1  && (_tmp4B == '\000'  && _tmp4C != '\000'))_throw_arraybounds();*((char*)_tmp4A.curr)=_tmp4C;});goto _LL2C;case 92: _LL3B: _LL3C:
({struct _dyneither_ptr _tmp4D=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp4E=*((char*)_check_dyneither_subscript(_tmp4D,sizeof(char),0));char _tmp4F='\\';if(_get_dyneither_size(_tmp4D,sizeof(char))== 1  && (_tmp4E == '\000'  && _tmp4F != '\000'))_throw_arraybounds();*((char*)_tmp4D.curr)=_tmp4F;});({struct _dyneither_ptr _tmp50=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp51=*((char*)_check_dyneither_subscript(_tmp50,sizeof(char),0));char _tmp52='\\';if(_get_dyneither_size(_tmp50,sizeof(char))== 1  && (_tmp51 == '\000'  && _tmp52 != '\000'))_throw_arraybounds();*((char*)_tmp50.curr)=_tmp52;});goto _LL2C;case 34: _LL3D: _LL3E:
({struct _dyneither_ptr _tmp53=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp54=*((char*)_check_dyneither_subscript(_tmp53,sizeof(char),0));char _tmp55='\\';if(_get_dyneither_size(_tmp53,sizeof(char))== 1  && (_tmp54 == '\000'  && _tmp55 != '\000'))_throw_arraybounds();*((char*)_tmp53.curr)=_tmp55;});({struct _dyneither_ptr _tmp56=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp57=*((char*)_check_dyneither_subscript(_tmp56,sizeof(char),0));char _tmp58='"';if(_get_dyneither_size(_tmp56,sizeof(char))== 1  && (_tmp57 == '\000'  && _tmp58 != '\000'))_throw_arraybounds();*((char*)_tmp56.curr)=_tmp58;});goto _LL2C;default: _LL3F: _tmp68=_tmp22;_LL40:
# 337
 if(_tmp68 >= ' '  && _tmp68 <= '~')({struct _dyneither_ptr _tmp59=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp5A=*((char*)_check_dyneither_subscript(_tmp59,sizeof(char),0));char _tmp5B=_tmp68;if(_get_dyneither_size(_tmp59,sizeof(char))== 1  && (_tmp5A == '\000'  && _tmp5B != '\000'))_throw_arraybounds();*((char*)_tmp59.curr)=_tmp5B;});else{
# 339
({struct _dyneither_ptr _tmp5C=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp5D=*((char*)_check_dyneither_subscript(_tmp5C,sizeof(char),0));char _tmp5E='\\';if(_get_dyneither_size(_tmp5C,sizeof(char))== 1  && (_tmp5D == '\000'  && _tmp5E != '\000'))_throw_arraybounds();*((char*)_tmp5C.curr)=_tmp5E;});
({struct _dyneither_ptr _tmp5F=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp60=*((char*)_check_dyneither_subscript(_tmp5F,sizeof(char),0));char _tmp61=(char)('0' + (_tmp68 >> 6 & 7));if(_get_dyneither_size(_tmp5F,sizeof(char))== 1  && (_tmp60 == '\000'  && _tmp61 != '\000'))_throw_arraybounds();*((char*)_tmp5F.curr)=_tmp61;});
({struct _dyneither_ptr _tmp62=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp63=*((char*)_check_dyneither_subscript(_tmp62,sizeof(char),0));char _tmp64=(char)('0' + (_tmp68 >> 3 & 7));if(_get_dyneither_size(_tmp62,sizeof(char))== 1  && (_tmp63 == '\000'  && _tmp64 != '\000'))_throw_arraybounds();*((char*)_tmp62.curr)=_tmp64;});
({struct _dyneither_ptr _tmp65=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp66=*((char*)_check_dyneither_subscript(_tmp65,sizeof(char),0));char _tmp67=(char)('0' + (_tmp68 & 7));if(_get_dyneither_size(_tmp65,sizeof(char))== 1  && (_tmp66 == '\000'  && _tmp67 != '\000'))_throw_arraybounds();*((char*)_tmp65.curr)=_tmp67;});}
# 344
goto _LL2C;}_LL2C:;}}
# 346
return(struct _dyneither_ptr)t;};};};}static char _tmp69[9]="restrict";
# 349
static struct _dyneither_ptr Cyc_Absynpp_restrict_string={_tmp69,_tmp69,_tmp69 + 9};static char _tmp6A[9]="volatile";
static struct _dyneither_ptr Cyc_Absynpp_volatile_string={_tmp6A,_tmp6A,_tmp6A + 9};static char _tmp6B[6]="const";
static struct _dyneither_ptr Cyc_Absynpp_const_str={_tmp6B,_tmp6B,_tmp6B + 6};
static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
# 356
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){
struct Cyc_List_List*l=0;
# 359
if(tq.q_restrict)l=({struct Cyc_List_List*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->hd=Cyc_Absynpp_restrict_sp;_tmp6C->tl=l;_tmp6C;});
if(tq.q_volatile)l=({struct Cyc_List_List*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->hd=Cyc_Absynpp_volatile_sp;_tmp6D->tl=l;_tmp6D;});
if(tq.print_const)l=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=Cyc_Absynpp_const_sp;_tmp6E->tl=l;_tmp6E;});
return Cyc_PP_egroup(({const char*_tmp6F="";_tag_dyneither(_tmp6F,sizeof(char),1);}),({const char*_tmp70=" ";_tag_dyneither(_tmp70,sizeof(char),2);}),({const char*_tmp71=" ";_tag_dyneither(_tmp71,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));}
# 365
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp72=ka;enum Cyc_Absyn_KindQual _tmp87;enum Cyc_Absyn_AliasQual _tmp86;_LL42: _tmp87=_tmp72->kind;_tmp86=_tmp72->aliasqual;_LL43:;{
enum Cyc_Absyn_KindQual _tmp73=_tmp87;switch(_tmp73){case Cyc_Absyn_AnyKind: _LL45: _LL46: {
# 369
enum Cyc_Absyn_AliasQual _tmp74=_tmp86;switch(_tmp74){case Cyc_Absyn_Aliasable: _LL52: _LL53:
 return({const char*_tmp75="A";_tag_dyneither(_tmp75,sizeof(char),2);});case Cyc_Absyn_Unique: _LL54: _LL55:
 return({const char*_tmp76="UA";_tag_dyneither(_tmp76,sizeof(char),3);});default: _LL56: _LL57:
 return({const char*_tmp77="TA";_tag_dyneither(_tmp77,sizeof(char),3);});}_LL51:;}case Cyc_Absyn_MemKind: _LL47: _LL48: {
# 375
enum Cyc_Absyn_AliasQual _tmp78=_tmp86;switch(_tmp78){case Cyc_Absyn_Aliasable: _LL59: _LL5A:
 return({const char*_tmp79="M";_tag_dyneither(_tmp79,sizeof(char),2);});case Cyc_Absyn_Unique: _LL5B: _LL5C:
 return({const char*_tmp7A="UM";_tag_dyneither(_tmp7A,sizeof(char),3);});default: _LL5D: _LL5E:
 return({const char*_tmp7B="TM";_tag_dyneither(_tmp7B,sizeof(char),3);});}_LL58:;}case Cyc_Absyn_BoxKind: _LL49: _LL4A: {
# 381
enum Cyc_Absyn_AliasQual _tmp7C=_tmp86;switch(_tmp7C){case Cyc_Absyn_Aliasable: _LL60: _LL61:
 return({const char*_tmp7D="B";_tag_dyneither(_tmp7D,sizeof(char),2);});case Cyc_Absyn_Unique: _LL62: _LL63:
 return({const char*_tmp7E="UB";_tag_dyneither(_tmp7E,sizeof(char),3);});default: _LL64: _LL65:
 return({const char*_tmp7F="TB";_tag_dyneither(_tmp7F,sizeof(char),3);});}_LL5F:;}case Cyc_Absyn_RgnKind: _LL4B: _LL4C: {
# 387
enum Cyc_Absyn_AliasQual _tmp80=_tmp86;switch(_tmp80){case Cyc_Absyn_Aliasable: _LL67: _LL68:
 return({const char*_tmp81="R";_tag_dyneither(_tmp81,sizeof(char),2);});case Cyc_Absyn_Unique: _LL69: _LL6A:
 return({const char*_tmp82="UR";_tag_dyneither(_tmp82,sizeof(char),3);});default: _LL6B: _LL6C:
 return({const char*_tmp83="TR";_tag_dyneither(_tmp83,sizeof(char),3);});}_LL66:;}case Cyc_Absyn_EffKind: _LL4D: _LL4E:
# 392
 return({const char*_tmp84="E";_tag_dyneither(_tmp84,sizeof(char),2);});default: _LL4F: _LL50:
 return({const char*_tmp85="I";_tag_dyneither(_tmp85,sizeof(char),2);});}_LL44:;};}
# 396
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 398
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp88=Cyc_Absyn_compress_kb(c);void*_tmp89=_tmp88;struct Cyc_Absyn_Kind*_tmp90;struct Cyc_Absyn_Kind*_tmp8F;switch(*((int*)_tmp89)){case 0: _LL6E: _tmp8F=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp89)->f1;_LL6F:
 return Cyc_Absynpp_kind2string(_tmp8F);case 1: _LL70: _LL71:
# 402
 if(Cyc_PP_tex_output)
return({const char*_tmp8A="{?}";_tag_dyneither(_tmp8A,sizeof(char),4);});else{
return({const char*_tmp8B="?";_tag_dyneither(_tmp8B,sizeof(char),2);});}default: _LL72: _tmp90=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89)->f2;_LL73:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8E;_tmp8E.tag=0;_tmp8E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp90));({void*_tmp8C[1]={& _tmp8E};Cyc_aprintf(({const char*_tmp8D="<=%s";_tag_dyneither(_tmp8D,sizeof(char),5);}),_tag_dyneither(_tmp8C,sizeof(void*),1));});});}_LL6D:;}
# 409
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp91=Cyc_Absyn_compress_kb(c);void*_tmp92=_tmp91;struct Cyc_Absyn_Kind*_tmp96;struct Cyc_Absyn_Kind*_tmp95;switch(*((int*)_tmp92)){case 0: _LL75: _tmp95=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp92)->f1;_LL76:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp95));case 1: _LL77: _LL78:
# 413
 if(Cyc_PP_tex_output)
return Cyc_PP_text_width(({const char*_tmp93="{?}";_tag_dyneither(_tmp93,sizeof(char),4);}),1);else{
return Cyc_PP_text(({const char*_tmp94="?";_tag_dyneither(_tmp94,sizeof(char),2);}));}default: _LL79: _tmp96=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp92)->f2;_LL7A:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp96));}_LL74:;}
# 420
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
return Cyc_PP_egroup(({const char*_tmp97="<";_tag_dyneither(_tmp97,sizeof(char),2);}),({const char*_tmp98=">";_tag_dyneither(_tmp98,sizeof(char),2);}),({const char*_tmp99=",";_tag_dyneither(_tmp99,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));}
# 424
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr _tmp9A=*tv->name;
# 428
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(tv)){
# 430
struct _dyneither_ptr kstring=({const char*_tmpA3="K";_tag_dyneither(_tmpA3,sizeof(char),2);});
{void*_tmp9B=Cyc_Absyn_compress_kb(tv->kind);void*_tmp9C=_tmp9B;struct Cyc_Absyn_Kind*_tmp9E;struct Cyc_Absyn_Kind*_tmp9D;switch(*((int*)_tmp9C)){case 2: _LL7C: _tmp9D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9C)->f2;_LL7D:
 _tmp9E=_tmp9D;goto _LL7F;case 0: _LL7E: _tmp9E=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9C)->f1;_LL7F:
 kstring=Cyc_Absynpp_kind2string(_tmp9E);goto _LL7B;default: _LL80: _LL81:
 goto _LL7B;}_LL7B:;}
# 436
return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA2;_tmpA2.tag=0;_tmpA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_tmp9A,sizeof(char),1));({struct Cyc_String_pa_PrintArg_struct _tmpA1;_tmpA1.tag=0;_tmpA1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kstring);({void*_tmp9F[2]={& _tmpA1,& _tmpA2};Cyc_aprintf(({const char*_tmpA0="`G%s%s";_tag_dyneither(_tmpA0,sizeof(char),7);}),_tag_dyneither(_tmp9F,sizeof(void*),2));});});}));}
# 438
return Cyc_PP_text(_tmp9A);}
# 441
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmpA4=Cyc_Absyn_compress_kb(tv->kind);void*_tmpA5=_tmpA4;struct Cyc_Absyn_Kind*_tmpA9;struct Cyc_Absyn_Kind*_tmpA8;switch(*((int*)_tmpA5)){case 1: _LL83: _LL84:
 goto _LL86;case 0: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL85: _LL86:
 return Cyc_Absynpp_tvar2doc(tv);}else{goto _LL89;}}else{_LL89: _tmpA8=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5)->f1;_LL8A:
# 446
 return({struct Cyc_PP_Doc*_tmpA6[3];_tmpA6[2]=Cyc_Absynpp_kind2doc(_tmpA8);_tmpA6[1]=Cyc_PP_text(({const char*_tmpA7="::";_tag_dyneither(_tmpA7,sizeof(char),3);}));_tmpA6[0]=Cyc_Absynpp_tvar2doc(tv);Cyc_PP_cat(_tag_dyneither(_tmpA6,sizeof(struct Cyc_PP_Doc*),3));});}default: _LL87: _tmpA9=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA5)->f2;_LL88:
# 445
 _tmpA8=_tmpA9;goto _LL8A;}_LL82:;}
# 450
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return Cyc_PP_egroup(({const char*_tmpAA="<";_tag_dyneither(_tmpAA,sizeof(char),2);}),({const char*_tmpAB=">";_tag_dyneither(_tmpAB,sizeof(char),2);}),({const char*_tmpAC=",";_tag_dyneither(_tmpAC,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}
# 454
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);
return Cyc_PP_egroup(({const char*_tmpAD="<";_tag_dyneither(_tmpAD,sizeof(char),2);}),({const char*_tmpAE=">";_tag_dyneither(_tmpAE,sizeof(char),2);}),({const char*_tmpAF=",";_tag_dyneither(_tmpAF,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 460
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 464
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return Cyc_PP_group(({const char*_tmpB0="(";_tag_dyneither(_tmpB0,sizeof(char),2);}),({const char*_tmpB1=")";_tag_dyneither(_tmpB1,sizeof(char),2);}),({const char*_tmpB2=",";_tag_dyneither(_tmpB2,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
# 468
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
void*_tmpB3=att;switch(*((int*)_tmpB3)){case 1: _LL8C: _LL8D:
 goto _LL8F;case 2: _LL8E: _LL8F:
 goto _LL91;case 3: _LL90: _LL91:
 return Cyc_PP_nil_doc();default: _LL92: _LL93:
 return Cyc_PP_text(Cyc_Absyn_attribute2string(att));}_LL8B:;}
# 477
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmpB4=(void*)atts->hd;void*_tmpB5=_tmpB4;switch(*((int*)_tmpB5)){case 1: _LL95: _LL96:
 return Cyc_PP_text(({const char*_tmpB6=" _stdcall ";_tag_dyneither(_tmpB6,sizeof(char),11);}));case 2: _LL97: _LL98:
 return Cyc_PP_text(({const char*_tmpB7=" _cdecl ";_tag_dyneither(_tmpB7,sizeof(char),9);}));case 3: _LL99: _LL9A:
 return Cyc_PP_text(({const char*_tmpB8=" _fastcall ";_tag_dyneither(_tmpB8,sizeof(char),12);}));default: _LL9B: _LL9C:
 goto _LL94;}_LL94:;}
# 485
return Cyc_PP_nil_doc();}
# 488
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpB9=(void*)atts2->hd;void*_tmpBA=_tmpB9;switch(*((int*)_tmpBA)){case 1: _LL9E: _LL9F:
 goto _LLA1;case 2: _LLA0: _LLA1:
 goto _LLA3;case 3: _LLA2: _LLA3:
 goto _LL9D;default: _LLA4: _LLA5:
 hasatt=1;goto _LL9D;}_LL9D:;}}
# 497
if(!hasatt)
return Cyc_PP_nil_doc();
return({struct Cyc_PP_Doc*_tmpBB[3];_tmpBB[2]=
# 501
Cyc_PP_text(({const char*_tmpC0=")";_tag_dyneither(_tmpC0,sizeof(char),2);}));_tmpBB[1]=
# 500
Cyc_PP_group(({const char*_tmpBD="";_tag_dyneither(_tmpBD,sizeof(char),1);}),({const char*_tmpBE="";_tag_dyneither(_tmpBE,sizeof(char),1);}),({const char*_tmpBF=" ";_tag_dyneither(_tmpBF,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));_tmpBB[0]=
# 499
Cyc_PP_text(({const char*_tmpBC=" __declspec(";_tag_dyneither(_tmpBC,sizeof(char),13);}));Cyc_PP_cat(_tag_dyneither(_tmpBB,sizeof(struct Cyc_PP_Doc*),3));});}
# 504
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 508
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();{
enum Cyc_Cyclone_C_Compilers _tmpC1=Cyc_Cyclone_c_compiler;if(_tmpC1 == Cyc_Cyclone_Vc_c){_LLA7: _LLA8:
 return Cyc_Absynpp_noncallconv2doc(atts);}else{_LLA9: _LLAA:
 return({struct Cyc_PP_Doc*_tmpC2[2];_tmpC2[1]=
Cyc_PP_group(({const char*_tmpC4="((";_tag_dyneither(_tmpC4,sizeof(char),3);}),({const char*_tmpC5="))";_tag_dyneither(_tmpC5,sizeof(char),3);}),({const char*_tmpC6=",";_tag_dyneither(_tmpC6,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));_tmpC2[0]=
# 512
Cyc_PP_text(({const char*_tmpC3=" __attribute__";_tag_dyneither(_tmpC3,sizeof(char),15);}));Cyc_PP_cat(_tag_dyneither(_tmpC2,sizeof(struct Cyc_PP_Doc*),2));});}_LLA6:;};}
# 517
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmpC7=(void*)tms->hd;void*_tmpC8=_tmpC7;switch(*((int*)_tmpC8)){case 2: _LLAC: _LLAD:
 return 1;case 5: _LLAE: _LLAF: {
# 522
enum Cyc_Cyclone_C_Compilers _tmpC9=Cyc_Cyclone_c_compiler;if(_tmpC9 == Cyc_Cyclone_Gcc_c){_LLB3: _LLB4:
 return 0;}else{_LLB5: _LLB6:
 return Cyc_Absynpp_next_is_pointer(tms->tl);}_LLB2:;}default: _LLB0: _LLB1:
# 526
 return 0;}_LLAB:;};}
# 530
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
# 533
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_question(){
if(!((unsigned int)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_question=Cyc_PP_text_width(({const char*_tmpCA="{?}";_tag_dyneither(_tmpCA,sizeof(char),4);}),1);else{
Cyc_Absynpp_cache_question=Cyc_PP_text(({const char*_tmpCB="?";_tag_dyneither(_tmpCB,sizeof(char),2);}));}}
# 540
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 542
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_lb=Cyc_PP_text_width(({const char*_tmpCC="{\\lb}";_tag_dyneither(_tmpCC,sizeof(char),6);}),1);else{
Cyc_Absynpp_cache_lb=Cyc_PP_text(({const char*_tmpCD="{";_tag_dyneither(_tmpCD,sizeof(char),2);}));}}
# 549
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 551
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_rb=Cyc_PP_text_width(({const char*_tmpCE="{\\rb}";_tag_dyneither(_tmpCE,sizeof(char),6);}),1);else{
Cyc_Absynpp_cache_rb=Cyc_PP_text(({const char*_tmpCF="}";_tag_dyneither(_tmpCF,sizeof(char),2);}));}}
# 558
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 560
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_dollar=Cyc_PP_text_width(({const char*_tmpD0="\\$";_tag_dyneither(_tmpD0,sizeof(char),3);}),1);else{
Cyc_Absynpp_cache_dollar=Cyc_PP_text(({const char*_tmpD1="$";_tag_dyneither(_tmpD1,sizeof(char),2);}));}}
# 567
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 569
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_tmpD2[3];_tmpD2[2]=Cyc_Absynpp_rb();_tmpD2[1]=Cyc_PP_seq(sep,ss);_tmpD2[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dyneither(_tmpD2,sizeof(struct Cyc_PP_Doc*),3));});}
# 574
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpD3=(void*)tms->hd;void*_tmpD4=_tmpD3;struct Cyc_List_List*_tmpEB;switch(*((int*)_tmpD4)){case 0: _LLB8: _LLB9:
({void*_tmpD5=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpD6="Carray_mod ";_tag_dyneither(_tmpD6,sizeof(char),12);}),_tag_dyneither(_tmpD5,sizeof(void*),0));});goto _LLB7;case 1: _LLBA: _LLBB:
({void*_tmpD7=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpD8="ConstArray_mod ";_tag_dyneither(_tmpD8,sizeof(char),16);}),_tag_dyneither(_tmpD7,sizeof(void*),0));});goto _LLB7;case 3: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD4)->f1)->tag == 1){_LLBC: _tmpEB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD4)->f1)->f1;_LLBD:
# 580
({void*_tmpD9=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpDA="Function_mod(";_tag_dyneither(_tmpDA,sizeof(char),14);}),_tag_dyneither(_tmpD9,sizeof(void*),0));});
for(0;_tmpEB != 0;_tmpEB=_tmpEB->tl){
struct _dyneither_ptr*_tmpDB=(*((struct _tuple8*)_tmpEB->hd)).f1;
if(_tmpDB == 0)({void*_tmpDC=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpDD="?";_tag_dyneither(_tmpDD,sizeof(char),2);}),_tag_dyneither(_tmpDC,sizeof(void*),0));});else{
({void*_tmpDE=0;Cyc_fprintf(Cyc_stderr,*_tmpDB,_tag_dyneither(_tmpDE,sizeof(void*),0));});}
if(_tmpEB->tl != 0)({void*_tmpDF=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpE0=",";_tag_dyneither(_tmpE0,sizeof(char),2);}),_tag_dyneither(_tmpDF,sizeof(void*),0));});}
# 587
({void*_tmpE1=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpE2=") ";_tag_dyneither(_tmpE2,sizeof(char),3);}),_tag_dyneither(_tmpE1,sizeof(void*),0));});
goto _LLB7;}else{_LLBE: _LLBF:
# 590
({void*_tmpE3=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpE4="Function_mod()";_tag_dyneither(_tmpE4,sizeof(char),15);}),_tag_dyneither(_tmpE3,sizeof(void*),0));});goto _LLB7;}case 5: _LLC0: _LLC1:
({void*_tmpE5=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpE6="Attributes_mod ";_tag_dyneither(_tmpE6,sizeof(char),16);}),_tag_dyneither(_tmpE5,sizeof(void*),0));});goto _LLB7;case 4: _LLC2: _LLC3:
({void*_tmpE7=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpE8="TypeParams_mod ";_tag_dyneither(_tmpE8,sizeof(char),16);}),_tag_dyneither(_tmpE7,sizeof(void*),0));});goto _LLB7;default: _LLC4: _LLC5:
({void*_tmpE9=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpEA="Pointer_mod ";_tag_dyneither(_tmpEA,sizeof(char),13);}),_tag_dyneither(_tmpE9,sizeof(void*),0));});goto _LLB7;}_LLB7:;}
# 595
({void*_tmpEC=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpED="\n";_tag_dyneither(_tmpED,sizeof(char),2);}),_tag_dyneither(_tmpEC,sizeof(void*),0));});}
# 598
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 600
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmp129[3];_tmp129[2]=Cyc_PP_text(({const char*_tmp12B=")";_tag_dyneither(_tmp12B,sizeof(char),2);}));_tmp129[1]=rest;_tmp129[0]=Cyc_PP_text(({const char*_tmp12A="(";_tag_dyneither(_tmp12A,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp129,sizeof(struct Cyc_PP_Doc*),3));});
void*_tmpEE=(void*)tms->hd;void*_tmpEF=_tmpEE;void*_tmp128;union Cyc_Absyn_Constraint*_tmp127;union Cyc_Absyn_Constraint*_tmp126;union Cyc_Absyn_Constraint*_tmp125;struct Cyc_Absyn_Tqual _tmp124;struct Cyc_List_List*_tmp123;unsigned int _tmp122;int _tmp121;struct Cyc_List_List*_tmp120;void*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;union Cyc_Absyn_Constraint*_tmp11D;union Cyc_Absyn_Constraint*_tmp11C;switch(*((int*)_tmpEF)){case 0: _LLC7: _tmp11C=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_LLC8:
# 606
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmpF0[2];_tmpF0[1]=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp11C)?
Cyc_PP_text(({const char*_tmpF1="[]@zeroterm";_tag_dyneither(_tmpF1,sizeof(char),12);})): Cyc_PP_text(({const char*_tmpF2="[]";_tag_dyneither(_tmpF2,sizeof(char),3);}));_tmpF0[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmpF0,sizeof(struct Cyc_PP_Doc*),2));});case 1: _LLC9: _tmp11E=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_tmp11D=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_LLCA:
# 610
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmpF3[4];_tmpF3[3]=
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp11D)?Cyc_PP_text(({const char*_tmpF5="]@zeroterm";_tag_dyneither(_tmpF5,sizeof(char),11);})): Cyc_PP_text(({const char*_tmpF6="]";_tag_dyneither(_tmpF6,sizeof(char),2);}));_tmpF3[2]=
# 611
Cyc_Absynpp_exp2doc(_tmp11E);_tmpF3[1]=Cyc_PP_text(({const char*_tmpF4="[";_tag_dyneither(_tmpF4,sizeof(char),2);}));_tmpF3[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmpF3,sizeof(struct Cyc_PP_Doc*),4));});case 3: _LLCB: _tmp11F=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_LLCC:
# 614
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmpF7=_tmp11F;struct Cyc_List_List*_tmp105;unsigned int _tmp104;struct Cyc_List_List*_tmp103;int _tmp102;struct Cyc_Absyn_VarargInfo*_tmp101;void*_tmp100;struct Cyc_List_List*_tmpFF;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Exp*_tmpFD;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->tag == 1){_LLD4: _tmp103=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f1;_tmp102=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f2;_tmp101=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f3;_tmp100=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f4;_tmpFF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f5;_tmpFE=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f6;_tmpFD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF7)->f7;_LLD5:
# 617
 return({struct Cyc_PP_Doc*_tmpF8[2];_tmpF8[1]=Cyc_Absynpp_funargs2doc(_tmp103,_tmp102,_tmp101,_tmp100,_tmpFF,_tmpFE,_tmpFD);_tmpF8[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmpF8,sizeof(struct Cyc_PP_Doc*),2));});}else{_LLD6: _tmp105=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpF7)->f1;_tmp104=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpF7)->f2;_LLD7:
# 619
 return({struct Cyc_PP_Doc*_tmpF9[2];_tmpF9[1]=Cyc_PP_group(({const char*_tmpFA="(";_tag_dyneither(_tmpFA,sizeof(char),2);}),({const char*_tmpFB=")";_tag_dyneither(_tmpFB,sizeof(char),2);}),({const char*_tmpFC=",";_tag_dyneither(_tmpFC,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp105));_tmpF9[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmpF9,sizeof(struct Cyc_PP_Doc*),2));});}_LLD3:;};case 5: _LLCD: _tmp120=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_LLCE: {
# 622
enum Cyc_Cyclone_C_Compilers _tmp106=Cyc_Cyclone_c_compiler;if(_tmp106 == Cyc_Cyclone_Gcc_c){_LLD9: _LLDA:
# 624
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmp107[2];_tmp107[1]=Cyc_Absynpp_atts2doc(_tmp120);_tmp107[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmp107,sizeof(struct Cyc_PP_Doc*),2));});}else{_LLDB: _LLDC:
# 627
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
return({struct Cyc_PP_Doc*_tmp108[2];_tmp108[1]=rest;_tmp108[0]=Cyc_Absynpp_callconv2doc(_tmp120);Cyc_PP_cat(_tag_dyneither(_tmp108,sizeof(struct Cyc_PP_Doc*),2));});
return rest;}_LLD8:;}case 4: _LLCF: _tmp123=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1;_tmp122=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_tmp121=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEF)->f3;_LLD0:
# 632
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp121)
return({struct Cyc_PP_Doc*_tmp109[2];_tmp109[1]=Cyc_Absynpp_ktvars2doc(_tmp123);_tmp109[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmp109,sizeof(struct Cyc_PP_Doc*),2));});else{
# 636
return({struct Cyc_PP_Doc*_tmp10A[2];_tmp10A[1]=Cyc_Absynpp_tvars2doc(_tmp123);_tmp10A[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmp10A,sizeof(struct Cyc_PP_Doc*),2));});}default: _LLD1: _tmp128=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).rgn;_tmp127=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).nullable;_tmp126=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).bounds;_tmp125=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f1).zero_term;_tmp124=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpEF)->f2;_LLD2: {
# 640
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(_tmp124);
{void*_tmp10B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp126);void*_tmp10C=_tmp10B;struct Cyc_Absyn_Exp*_tmp114;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp10C)->tag == 0){_LLDE: _LLDF:
 ptr=Cyc_Absynpp_question();goto _LLDD;}else{_LLE0: _tmp114=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp10C)->f1;_LLE1:
# 648
 ptr=Cyc_PP_text(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp127)?({const char*_tmp10D="*";_tag_dyneither(_tmp10D,sizeof(char),2);}):({const char*_tmp10E="@";_tag_dyneither(_tmp10E,sizeof(char),2);}));{
struct _tuple11 _tmp10F=Cyc_Evexp_eval_const_uint_exp(_tmp114);struct _tuple11 _tmp110=_tmp10F;unsigned int _tmp113;int _tmp112;_LLE3: _tmp113=_tmp110.f1;_tmp112=_tmp110.f2;_LLE4:;
if(!_tmp112  || _tmp113 != 1)
ptr=({struct Cyc_PP_Doc*_tmp111[4];_tmp111[3]=Cyc_Absynpp_rb();_tmp111[2]=Cyc_Absynpp_exp2doc(_tmp114);_tmp111[1]=Cyc_Absynpp_lb();_tmp111[0]=ptr;Cyc_PP_cat(_tag_dyneither(_tmp111,sizeof(struct Cyc_PP_Doc*),4));});
goto _LLDD;};}_LLDD:;}
# 654
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp125))
ztd=Cyc_PP_text(({const char*_tmp115="@zeroterm";_tag_dyneither(_tmp115,sizeof(char),10);}));else{
if(is_char_ptr  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp125))
ztd=Cyc_PP_text(({const char*_tmp116="@nozeroterm";_tag_dyneither(_tmp116,sizeof(char),12);}));}}
# 660
{void*_tmp117=Cyc_Tcutil_compress(_tmp128);void*_tmp118=_tmp117;switch(*((int*)_tmp118)){case 20: _LLE6: _LLE7:
 goto _LLE5;case 1: _LLE8: if(Cyc_Absynpp_print_for_cycdoc){_LLE9:
 goto _LLE5;}else{goto _LLEA;}default: _LLEA: _LLEB:
 rgd=Cyc_Absynpp_rgn2doc(_tmp128);}_LLE5:;}{
# 665
struct Cyc_PP_Doc*spacer1=tqd != mt  && (ztd != mt  || rgd != mt)?Cyc_PP_text(({const char*_tmp11B=" ";_tag_dyneither(_tmp11B,sizeof(char),2);})): mt;
struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(({const char*_tmp11A=" ";_tag_dyneither(_tmp11A,sizeof(char),2);})): mt;
return({struct Cyc_PP_Doc*_tmp119[7];_tmp119[6]=rest;_tmp119[5]=spacer2;_tmp119[4]=tqd;_tmp119[3]=spacer1;_tmp119[2]=rgd;_tmp119[1]=ztd;_tmp119[0]=ptr;Cyc_PP_cat(_tag_dyneither(_tmp119,sizeof(struct Cyc_PP_Doc*),7));});};}}_LLC6:;};}
# 671
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp12C=Cyc_Tcutil_compress(t);void*_tmp12D=_tmp12C;switch(*((int*)_tmp12D)){case 20: _LLED: _LLEE:
 return Cyc_PP_text(({const char*_tmp12E="`H";_tag_dyneither(_tmp12E,sizeof(char),3);}));case 21: _LLEF: _LLF0:
 return Cyc_PP_text(({const char*_tmp12F="`U";_tag_dyneither(_tmp12F,sizeof(char),3);}));case 22: _LLF1: _LLF2:
 return Cyc_PP_text(({const char*_tmp130="`RC";_tag_dyneither(_tmp130,sizeof(char),4);}));default: _LLF3: _LLF4:
 return Cyc_Absynpp_ntyp2doc(t);}_LLEC:;}
# 681
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 685
void*_tmp131=Cyc_Tcutil_compress(t);void*_tmp132=_tmp131;struct Cyc_List_List*_tmp136;void*_tmp135;switch(*((int*)_tmp132)){case 23: _LLF6: _tmp135=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp132)->f1;_LLF7:
*rgions=({struct Cyc_List_List*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->hd=Cyc_Absynpp_rgn2doc(_tmp135);_tmp133->tl=*rgions;_tmp133;});goto _LLF5;case 24: _LLF8: _tmp136=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp132)->f1;_LLF9:
# 688
 for(0;_tmp136 != 0;_tmp136=_tmp136->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp136->hd);}
# 691
goto _LLF5;default: _LLFA: _LLFB:
*effects=({struct Cyc_List_List*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->hd=Cyc_Absynpp_typ2doc(t);_tmp134->tl=*effects;_tmp134;});goto _LLF5;}_LLF5:;}
# 696
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0  && effects != 0)
return Cyc_PP_group(({const char*_tmp137="";_tag_dyneither(_tmp137,sizeof(char),1);}),({const char*_tmp138="";_tag_dyneither(_tmp138,sizeof(char),1);}),({const char*_tmp139="+";_tag_dyneither(_tmp139,sizeof(char),2);}),effects);else{
# 704
struct Cyc_PP_Doc*_tmp13A=Cyc_Absynpp_group_braces(({const char*_tmp13F=",";_tag_dyneither(_tmp13F,sizeof(char),2);}),rgions);
return Cyc_PP_group(({const char*_tmp13B="";_tag_dyneither(_tmp13B,sizeof(char),1);}),({const char*_tmp13C="";_tag_dyneither(_tmp13C,sizeof(char),1);}),({const char*_tmp13D="+";_tag_dyneither(_tmp13D,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,({struct Cyc_List_List*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E->hd=_tmp13A;_tmp13E->tl=0;_tmp13E;})));}}
# 709
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp140=k;if(_tmp140 == Cyc_Absyn_StructA){_LLFD: _LLFE:
 return Cyc_PP_text(({const char*_tmp141="struct ";_tag_dyneither(_tmp141,sizeof(char),8);}));}else{_LLFF: _LL100:
 return Cyc_PP_text(({const char*_tmp142="union ";_tag_dyneither(_tmp142,sizeof(char),7);}));}_LLFC:;}
# 717
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp143=t;struct Cyc_Absyn_Datatypedecl*_tmp1B8;struct Cyc_Absyn_Enumdecl*_tmp1B7;struct Cyc_Absyn_Aggrdecl*_tmp1B6;void*_tmp1B5;void*_tmp1B4;void*_tmp1B3;void*_tmp1B2;void*_tmp1B1;struct _tuple0*_tmp1B0;struct Cyc_List_List*_tmp1AF;struct Cyc_Absyn_Typedefdecl*_tmp1AE;struct _dyneither_ptr _tmp1AD;struct Cyc_Absyn_Exp*_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AB;struct _tuple0*_tmp1AA;struct Cyc_List_List*_tmp1A9;enum Cyc_Absyn_AggrKind _tmp1A8;struct Cyc_List_List*_tmp1A7;union Cyc_Absyn_AggrInfoU _tmp1A6;struct Cyc_List_List*_tmp1A5;struct Cyc_List_List*_tmp1A4;int _tmp1A3;enum Cyc_Absyn_Sign _tmp1A2;enum Cyc_Absyn_Size_of _tmp1A1;union Cyc_Absyn_DatatypeFieldInfoU _tmp1A0;struct Cyc_List_List*_tmp19F;union Cyc_Absyn_DatatypeInfoU _tmp19E;struct Cyc_List_List*_tmp19D;struct Cyc_Absyn_Tvar*_tmp19C;struct Cyc_Core_Opt*_tmp19B;void*_tmp19A;int _tmp199;struct Cyc_Core_Opt*_tmp198;switch(*((int*)_tmp143)){case 8: _LL102: _LL103:
# 721
 return Cyc_PP_text(({const char*_tmp144="[[[array]]]";_tag_dyneither(_tmp144,sizeof(char),12);}));case 9: _LL104: _LL105:
 return Cyc_PP_nil_doc();case 5: _LL106: _LL107:
 return Cyc_PP_nil_doc();case 0: _LL108: _LL109:
# 725
 s=Cyc_PP_text(({const char*_tmp145="void";_tag_dyneither(_tmp145,sizeof(char),5);}));goto _LL101;case 1: _LL10A: _tmp19B=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f1;_tmp19A=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f2;_tmp199=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f3;_tmp198=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f4;_LL10B:
# 727
 if(_tmp19A != 0)
# 729
return Cyc_Absynpp_ntyp2doc(_tmp19A);else{
# 731
struct _dyneither_ptr kindstring=_tmp19B == 0?({const char*_tmp14A="K";_tag_dyneither(_tmp14A,sizeof(char),2);}): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)_tmp19B->v);
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp149;_tmp149.tag=1;_tmp149.f1=(unsigned long)_tmp199;({struct Cyc_String_pa_PrintArg_struct _tmp148;_tmp148.tag=0;_tmp148.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kindstring);({void*_tmp146[2]={& _tmp148,& _tmp149};Cyc_aprintf(({const char*_tmp147="`E%s%d";_tag_dyneither(_tmp147,sizeof(char),7);}),_tag_dyneither(_tmp146,sizeof(void*),2));});});}));}
# 748 "absynpp.cyc"
goto _LL101;case 2: _LL10C: _tmp19C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp143)->f1;_LL10D:
# 750
 s=Cyc_Absynpp_tvar2doc(_tmp19C);
if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_tmp14B[3];_tmp14B[2]=Cyc_Absynpp_kindbound2doc(_tmp19C->kind);_tmp14B[1]=Cyc_PP_text(({const char*_tmp14C="::";_tag_dyneither(_tmp14C,sizeof(char),3);}));_tmp14B[0]=s;Cyc_PP_cat(_tag_dyneither(_tmp14B,sizeof(struct Cyc_PP_Doc*),3));});
# 754
goto _LL101;case 3: _LL10E: _tmp19E=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp143)->f1).datatype_info;_tmp19D=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp143)->f1).targs;_LL10F:
# 756
{union Cyc_Absyn_DatatypeInfoU _tmp14D=_tmp19E;struct _tuple0*_tmp156;int _tmp155;struct _tuple0*_tmp154;int _tmp153;if((_tmp14D.UnknownDatatype).tag == 1){_LL141: _tmp154=((_tmp14D.UnknownDatatype).val).name;_tmp153=((_tmp14D.UnknownDatatype).val).is_extensible;_LL142:
# 758
 _tmp156=_tmp154;_tmp155=_tmp153;goto _LL144;}else{_LL143: _tmp156=(*(_tmp14D.KnownDatatype).val)->name;_tmp155=(*(_tmp14D.KnownDatatype).val)->is_extensible;_LL144: {
# 760
struct Cyc_PP_Doc*_tmp14E=Cyc_PP_text(({const char*_tmp152="datatype ";_tag_dyneither(_tmp152,sizeof(char),10);}));
struct Cyc_PP_Doc*_tmp14F=_tmp155?Cyc_PP_text(({const char*_tmp151="@extensible ";_tag_dyneither(_tmp151,sizeof(char),13);})): Cyc_PP_nil_doc();
s=({struct Cyc_PP_Doc*_tmp150[4];_tmp150[3]=Cyc_Absynpp_tps2doc(_tmp19D);_tmp150[2]=Cyc_Absynpp_qvar2doc(_tmp156);_tmp150[1]=_tmp14E;_tmp150[0]=_tmp14F;Cyc_PP_cat(_tag_dyneither(_tmp150,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL140;}}_LL140:;}
# 765
goto _LL101;case 4: _LL110: _tmp1A0=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp143)->f1).field_info;_tmp19F=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp143)->f1).targs;_LL111:
# 767
{union Cyc_Absyn_DatatypeFieldInfoU _tmp157=_tmp1A0;struct _tuple0*_tmp162;int _tmp161;struct _tuple0*_tmp160;struct _tuple0*_tmp15F;struct _tuple0*_tmp15E;int _tmp15D;if((_tmp157.UnknownDatatypefield).tag == 1){_LL146: _tmp15F=((_tmp157.UnknownDatatypefield).val).datatype_name;_tmp15E=((_tmp157.UnknownDatatypefield).val).field_name;_tmp15D=((_tmp157.UnknownDatatypefield).val).is_extensible;_LL147:
# 769
 _tmp162=_tmp15F;_tmp161=_tmp15D;_tmp160=_tmp15E;goto _LL149;}else{_LL148: _tmp162=(((_tmp157.KnownDatatypefield).val).f1)->name;_tmp161=(((_tmp157.KnownDatatypefield).val).f1)->is_extensible;_tmp160=(((_tmp157.KnownDatatypefield).val).f2)->name;_LL149: {
# 772
struct Cyc_PP_Doc*_tmp158=Cyc_PP_text(_tmp161?({const char*_tmp15B="@extensible datatype ";_tag_dyneither(_tmp15B,sizeof(char),22);}):({const char*_tmp15C="datatype ";_tag_dyneither(_tmp15C,sizeof(char),10);}));
s=({struct Cyc_PP_Doc*_tmp159[4];_tmp159[3]=Cyc_Absynpp_qvar2doc(_tmp160);_tmp159[2]=Cyc_PP_text(({const char*_tmp15A=".";_tag_dyneither(_tmp15A,sizeof(char),2);}));_tmp159[1]=Cyc_Absynpp_qvar2doc(_tmp162);_tmp159[0]=_tmp158;Cyc_PP_cat(_tag_dyneither(_tmp159,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL145;}}_LL145:;}
# 776
goto _LL101;case 6: _LL112: _tmp1A2=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp143)->f1;_tmp1A1=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp143)->f2;_LL113: {
# 778
struct _dyneither_ptr sns;
struct _dyneither_ptr ts;
{enum Cyc_Absyn_Sign _tmp163=_tmp1A2;switch(_tmp163){case Cyc_Absyn_None: _LL14B: _LL14C:
 goto _LL14E;case Cyc_Absyn_Signed: _LL14D: _LL14E:
 sns=({const char*_tmp164="";_tag_dyneither(_tmp164,sizeof(char),1);});goto _LL14A;default: _LL14F: _LL150:
 sns=({const char*_tmp165="unsigned ";_tag_dyneither(_tmp165,sizeof(char),10);});goto _LL14A;}_LL14A:;}
# 785
{enum Cyc_Absyn_Size_of _tmp166=_tmp1A1;switch(_tmp166){case Cyc_Absyn_Char_sz: _LL152: _LL153:
# 787
{enum Cyc_Absyn_Sign _tmp167=_tmp1A2;switch(_tmp167){case Cyc_Absyn_None: _LL15D: _LL15E:
 sns=({const char*_tmp168="";_tag_dyneither(_tmp168,sizeof(char),1);});goto _LL15C;case Cyc_Absyn_Signed: _LL15F: _LL160:
 sns=({const char*_tmp169="signed ";_tag_dyneither(_tmp169,sizeof(char),8);});goto _LL15C;default: _LL161: _LL162:
 sns=({const char*_tmp16A="unsigned ";_tag_dyneither(_tmp16A,sizeof(char),10);});goto _LL15C;}_LL15C:;}
# 792
ts=({const char*_tmp16B="char";_tag_dyneither(_tmp16B,sizeof(char),5);});
goto _LL151;case Cyc_Absyn_Short_sz: _LL154: _LL155:
 ts=({const char*_tmp16C="short";_tag_dyneither(_tmp16C,sizeof(char),6);});goto _LL151;case Cyc_Absyn_Int_sz: _LL156: _LL157:
 ts=({const char*_tmp16D="int";_tag_dyneither(_tmp16D,sizeof(char),4);});goto _LL151;case Cyc_Absyn_Long_sz: _LL158: _LL159:
 ts=({const char*_tmp16E="long";_tag_dyneither(_tmp16E,sizeof(char),5);});goto _LL151;default: _LL15A: _LL15B:
# 798
{enum Cyc_Cyclone_C_Compilers _tmp16F=Cyc_Cyclone_c_compiler;if(_tmp16F == Cyc_Cyclone_Gcc_c){_LL164: _LL165:
 ts=({const char*_tmp170="long long";_tag_dyneither(_tmp170,sizeof(char),10);});goto _LL163;}else{_LL166: _LL167:
 ts=({const char*_tmp171="__int64";_tag_dyneither(_tmp171,sizeof(char),8);});goto _LL163;}_LL163:;}
# 802
goto _LL151;}_LL151:;}
# 804
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp175;_tmp175.tag=0;_tmp175.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts);({struct Cyc_String_pa_PrintArg_struct _tmp174;_tmp174.tag=0;_tmp174.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns);({void*_tmp172[2]={& _tmp174,& _tmp175};Cyc_aprintf(({const char*_tmp173="%s%s";_tag_dyneither(_tmp173,sizeof(char),5);}),_tag_dyneither(_tmp172,sizeof(void*),2));});});}));
goto _LL101;}case 7: _LL114: _tmp1A3=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp143)->f1;_LL115:
# 807
 if(_tmp1A3 == 0)
s=Cyc_PP_text(({const char*_tmp176="float";_tag_dyneither(_tmp176,sizeof(char),6);}));else{
if(_tmp1A3 == 1)
s=Cyc_PP_text(({const char*_tmp177="double";_tag_dyneither(_tmp177,sizeof(char),7);}));else{
# 812
s=Cyc_PP_text(({const char*_tmp178="long double";_tag_dyneither(_tmp178,sizeof(char),12);}));}}
goto _LL101;case 10: _LL116: _tmp1A4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp143)->f1;_LL117:
# 815
 s=({struct Cyc_PP_Doc*_tmp179[2];_tmp179[1]=Cyc_Absynpp_args2doc(_tmp1A4);_tmp179[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dyneither(_tmp179,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL101;case 11: _LL118: _tmp1A6=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp143)->f1).aggr_info;_tmp1A5=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp143)->f1).targs;_LL119: {
# 818
struct _tuple10 _tmp17A=Cyc_Absyn_aggr_kinded_name(_tmp1A6);struct _tuple10 _tmp17B=_tmp17A;enum Cyc_Absyn_AggrKind _tmp17E;struct _tuple0*_tmp17D;_LL169: _tmp17E=_tmp17B.f1;_tmp17D=_tmp17B.f2;_LL16A:;
s=({struct Cyc_PP_Doc*_tmp17C[3];_tmp17C[2]=Cyc_Absynpp_tps2doc(_tmp1A5);_tmp17C[1]=Cyc_Absynpp_qvar2doc(_tmp17D);_tmp17C[0]=Cyc_Absynpp_aggr_kind2doc(_tmp17E);Cyc_PP_cat(_tag_dyneither(_tmp17C,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL101;}case 12: _LL11A: _tmp1A8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp143)->f1;_tmp1A7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp143)->f2;_LL11B:
# 822
 s=({struct Cyc_PP_Doc*_tmp17F[4];_tmp17F[3]=
# 824
Cyc_Absynpp_rb();_tmp17F[2]=
# 823
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp1A7));_tmp17F[1]=
# 822
Cyc_Absynpp_lb();_tmp17F[0]=Cyc_Absynpp_aggr_kind2doc(_tmp1A8);Cyc_PP_cat(_tag_dyneither(_tmp17F,sizeof(struct Cyc_PP_Doc*),4));});
# 825
goto _LL101;case 14: _LL11C: _tmp1A9=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp143)->f1;_LL11D:
# 827
 s=({struct Cyc_PP_Doc*_tmp180[4];_tmp180[3]=Cyc_Absynpp_rb();_tmp180[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp1A9));_tmp180[1]=Cyc_Absynpp_lb();_tmp180[0]=Cyc_PP_text(({const char*_tmp181="enum ";_tag_dyneither(_tmp181,sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp180,sizeof(struct Cyc_PP_Doc*),4));});goto _LL101;case 13: _LL11E: _tmp1AA=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp143)->f1;_LL11F:
# 829
 s=({struct Cyc_PP_Doc*_tmp182[2];_tmp182[1]=Cyc_Absynpp_qvar2doc(_tmp1AA);_tmp182[0]=Cyc_PP_text(({const char*_tmp183="enum ";_tag_dyneither(_tmp183,sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp182,sizeof(struct Cyc_PP_Doc*),2));});goto _LL101;case 18: _LL120: _tmp1AB=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp143)->f1;_LL121:
# 831
 s=({struct Cyc_PP_Doc*_tmp184[3];_tmp184[2]=Cyc_PP_text(({const char*_tmp186=")";_tag_dyneither(_tmp186,sizeof(char),2);}));_tmp184[1]=Cyc_Absynpp_exp2doc(_tmp1AB);_tmp184[0]=Cyc_PP_text(({const char*_tmp185="valueof_t(";_tag_dyneither(_tmp185,sizeof(char),11);}));Cyc_PP_cat(_tag_dyneither(_tmp184,sizeof(struct Cyc_PP_Doc*),3));});goto _LL101;case 27: _LL122: _tmp1AC=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp143)->f1;_LL123:
# 833
 s=({struct Cyc_PP_Doc*_tmp187[3];_tmp187[2]=Cyc_PP_text(({const char*_tmp189=")";_tag_dyneither(_tmp189,sizeof(char),2);}));_tmp187[1]=Cyc_Absynpp_exp2doc(_tmp1AC);_tmp187[0]=Cyc_PP_text(({const char*_tmp188="typeof(";_tag_dyneither(_tmp188,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp187,sizeof(struct Cyc_PP_Doc*),3));});goto _LL101;case 28: _LL124: _tmp1AD=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp143)->f1;_LL125:
# 835
 s=Cyc_PP_text(_tmp1AD);goto _LL101;case 17: _LL126: _tmp1B0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f1;_tmp1AF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f2;_tmp1AE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f3;_LL127:
# 842
 s=({struct Cyc_PP_Doc*_tmp18A[2];_tmp18A[1]=Cyc_Absynpp_tps2doc(_tmp1AF);_tmp18A[0]=Cyc_Absynpp_qvar2doc(_tmp1B0);Cyc_PP_cat(_tag_dyneither(_tmp18A,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL101;case 15: _LL128: _tmp1B1=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp143)->f1;_LL129:
# 845
 s=({struct Cyc_PP_Doc*_tmp18B[3];_tmp18B[2]=Cyc_PP_text(({const char*_tmp18D=">";_tag_dyneither(_tmp18D,sizeof(char),2);}));_tmp18B[1]=Cyc_Absynpp_rgn2doc(_tmp1B1);_tmp18B[0]=Cyc_PP_text(({const char*_tmp18C="region_t<";_tag_dyneither(_tmp18C,sizeof(char),10);}));Cyc_PP_cat(_tag_dyneither(_tmp18B,sizeof(struct Cyc_PP_Doc*),3));});goto _LL101;case 16: _LL12A: _tmp1B3=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp143)->f1;_tmp1B2=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp143)->f2;_LL12B:
# 847
 s=({struct Cyc_PP_Doc*_tmp18E[5];_tmp18E[4]=
Cyc_PP_text(({const char*_tmp191=">";_tag_dyneither(_tmp191,sizeof(char),2);}));_tmp18E[3]=
# 847
Cyc_Absynpp_rgn2doc(_tmp1B2);_tmp18E[2]=Cyc_PP_text(({const char*_tmp190=",";_tag_dyneither(_tmp190,sizeof(char),2);}));_tmp18E[1]=Cyc_Absynpp_rgn2doc(_tmp1B3);_tmp18E[0]=Cyc_PP_text(({const char*_tmp18F="dynregion_t<";_tag_dyneither(_tmp18F,sizeof(char),13);}));Cyc_PP_cat(_tag_dyneither(_tmp18E,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL101;case 19: _LL12C: _tmp1B4=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp143)->f1;_LL12D:
# 850
 s=({struct Cyc_PP_Doc*_tmp192[3];_tmp192[2]=Cyc_PP_text(({const char*_tmp194=">";_tag_dyneither(_tmp194,sizeof(char),2);}));_tmp192[1]=Cyc_Absynpp_typ2doc(_tmp1B4);_tmp192[0]=Cyc_PP_text(({const char*_tmp193="tag_t<";_tag_dyneither(_tmp193,sizeof(char),7);}));Cyc_PP_cat(_tag_dyneither(_tmp192,sizeof(struct Cyc_PP_Doc*),3));});goto _LL101;case 21: _LL12E: _LL12F:
 goto _LL131;case 20: _LL130: _LL131:
 goto _LL133;case 22: _LL132: _LL133:
# 854
 s=Cyc_Absynpp_rgn2doc(t);goto _LL101;case 25: _LL134: _tmp1B5=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp143)->f1;_LL135:
# 856
 s=({struct Cyc_PP_Doc*_tmp195[3];_tmp195[2]=Cyc_PP_text(({const char*_tmp197=")";_tag_dyneither(_tmp197,sizeof(char),2);}));_tmp195[1]=Cyc_Absynpp_typ2doc(_tmp1B5);_tmp195[0]=Cyc_PP_text(({const char*_tmp196="regions(";_tag_dyneither(_tmp196,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp195,sizeof(struct Cyc_PP_Doc*),3));});goto _LL101;case 23: _LL136: _LL137:
 goto _LL139;case 24: _LL138: _LL139:
# 859
 s=Cyc_Absynpp_eff2doc(t);goto _LL101;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)){case 0: _LL13A: _tmp1B6=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_LL13B:
# 861
 s=Cyc_Absynpp_aggrdecl2doc(_tmp1B6);goto _LL101;case 1: _LL13C: _tmp1B7=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_LL13D:
# 863
 s=Cyc_Absynpp_enumdecl2doc(_tmp1B7);goto _LL101;default: _LL13E: _tmp1B8=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_LL13F:
# 865
 s=Cyc_Absynpp_datatypedecl2doc(_tmp1B8);goto _LL101;}}_LL101:;}
# 867
return s;}
# 870
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _dyneither_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple16{void*f1;void*f2;};
# 874
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
struct _tuple16*_tmp1B9=cmp;void*_tmp1BD;void*_tmp1BC;_LL16C: _tmp1BD=_tmp1B9->f1;_tmp1BC=_tmp1B9->f2;_LL16D:;
return({struct Cyc_PP_Doc*_tmp1BA[3];_tmp1BA[2]=Cyc_Absynpp_rgn2doc(_tmp1BC);_tmp1BA[1]=Cyc_PP_text(({const char*_tmp1BB=" > ";_tag_dyneither(_tmp1BB,sizeof(char),4);}));_tmp1BA[0]=Cyc_Absynpp_rgn2doc(_tmp1BD);Cyc_PP_cat(_tag_dyneither(_tmp1BA,sizeof(struct Cyc_PP_Doc*),3));});}
# 879
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
return Cyc_PP_group(({const char*_tmp1BE="";_tag_dyneither(_tmp1BE,sizeof(char),1);}),({const char*_tmp1BF="";_tag_dyneither(_tmp1BF,sizeof(char),1);}),({const char*_tmp1C0=",";_tag_dyneither(_tmp1C0,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
# 883
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _dyneither_ptr*_tmp1C1=(*t).f1;
struct Cyc_Core_Opt*dopt=_tmp1C1 == 0?0:({struct Cyc_Core_Opt*_tmp1C2=_cycalloc(sizeof(*_tmp1C2));_tmp1C2->v=Cyc_PP_text(*_tmp1C1);_tmp1C2;});
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 889
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 893
struct Cyc_List_List*_tmp1C3=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs)
_tmp1C3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1C3,({struct Cyc_List_List*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->hd=Cyc_PP_text(({const char*_tmp1C5="...";_tag_dyneither(_tmp1C5,sizeof(char),4);}));_tmp1C4->tl=0;_tmp1C4;}));else{
if(cyc_varargs != 0){
struct Cyc_PP_Doc*_tmp1C6=({struct Cyc_PP_Doc*_tmp1C8[3];_tmp1C8[2]=
# 900
Cyc_Absynpp_funarg2doc(({struct _tuple8*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->f1=cyc_varargs->name;_tmp1CC->f2=cyc_varargs->tq;_tmp1CC->f3=cyc_varargs->type;_tmp1CC;}));_tmp1C8[1]=
# 899
cyc_varargs->inject?Cyc_PP_text(({const char*_tmp1CA=" inject ";_tag_dyneither(_tmp1CA,sizeof(char),9);})): Cyc_PP_text(({const char*_tmp1CB=" ";_tag_dyneither(_tmp1CB,sizeof(char),2);}));_tmp1C8[0]=
# 898
Cyc_PP_text(({const char*_tmp1C9="...";_tag_dyneither(_tmp1C9,sizeof(char),4);}));Cyc_PP_cat(_tag_dyneither(_tmp1C8,sizeof(struct Cyc_PP_Doc*),3));});
# 902
_tmp1C3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1C3,({struct Cyc_List_List*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7->hd=_tmp1C6;_tmp1C7->tl=0;_tmp1C7;}));}}{
# 904
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
# 917
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg){
struct _tuple8*_tmp1DF=arg;struct _dyneither_ptr*_tmp1E3;struct Cyc_Absyn_Tqual _tmp1E2;void*_tmp1E1;_LL16F: _tmp1E3=_tmp1DF->f1;_tmp1E2=_tmp1DF->f2;_tmp1E1=_tmp1DF->f3;_LL170:;
return({struct _tuple8*_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0->f1=_tmp1E3;_tmp1E0->f2=_tmp1E2;_tmp1E0->f3=_tmp1E1;_tmp1E0;});}
# 922
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){return Cyc_PP_text(*v);}
# 924
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp1E4=0;
int match;
{union Cyc_Absyn_Nmspace _tmp1E5=(*q).f1;union Cyc_Absyn_Nmspace _tmp1E6=_tmp1E5;struct Cyc_List_List*_tmp1EA;struct Cyc_List_List*_tmp1E9;struct Cyc_List_List*_tmp1E8;switch((_tmp1E6.C_n).tag){case 4: _LL172: _LL173:
# 929
 _tmp1E8=0;goto _LL175;case 1: _LL174: _tmp1E8=(_tmp1E6.Rel_n).val;_LL175:
# 931
 match=0;
_tmp1E4=_tmp1E8;
goto _LL171;case 3: _LL176: _tmp1E9=(_tmp1E6.C_n).val;_LL177:
# 935
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1E9,Cyc_Absynpp_curr_namespace);
# 937
goto _LL171;default: _LL178: _tmp1EA=(_tmp1E6.Abs_n).val;_LL179:
# 939
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1EA,Cyc_Absynpp_curr_namespace);
_tmp1E4=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7->hd=Cyc_Absynpp_cyc_stringptr;_tmp1E7->tl=_tmp1EA;_tmp1E7;}): _tmp1EA;
goto _LL171;}_LL171:;}
# 943
if(Cyc_Absynpp_qvar_to_Cids)
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1E4,({struct Cyc_List_List*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB->hd=(*q).f2;_tmp1EB->tl=0;_tmp1EB;})),({const char*_tmp1EC="_";_tag_dyneither(_tmp1EC,sizeof(char),2);}));else{
# 948
if(match)
return*(*q).f2;else{
# 951
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1E4,({struct Cyc_List_List*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED->hd=(*q).f2;_tmp1ED->tl=0;_tmp1ED;})),({const char*_tmp1EE="::";_tag_dyneither(_tmp1EE,sizeof(char),3);}));}}}
# 956
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 960
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp1EF=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output)
# 964
return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(({const char*_tmp1F0="\\textbf{";_tag_dyneither(_tmp1F0,sizeof(char),9);}),(struct _dyneither_ptr)_tmp1EF),({const char*_tmp1F1="}";_tag_dyneither(_tmp1F1,sizeof(char),2);})),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp1EF));else{
return Cyc_PP_text(_tmp1EF);}}
# 969
struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 971
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 974
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp1F2=(*v).f1;union Cyc_Absyn_Nmspace _tmp1F3=_tmp1F2;struct Cyc_List_List*_tmp1F6;struct Cyc_List_List*_tmp1F5;switch((_tmp1F3.C_n).tag){case 4: _LL17B: _LL17C:
 goto _LL17E;case 1: if((_tmp1F3.Rel_n).val == 0){_LL17D: _LL17E:
 return*(*v).f2;}else{_LL183: _LL184:
# 985
 return(struct _dyneither_ptr)Cyc_strconcat(({const char*_tmp1F4="/* bad namespace : */ ";_tag_dyneither(_tmp1F4,sizeof(char),23);}),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));}case 3: _LL17F: _tmp1F5=(_tmp1F3.C_n).val;_LL180:
# 978
 _tmp1F6=_tmp1F5;goto _LL182;default: _LL181: _tmp1F6=(_tmp1F3.Abs_n).val;_LL182:
# 980
 if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp1F6,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 983
goto _LL184;}}_LL17A:;}else{
# 988
return*(*v).f2;}}
# 991
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 994
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _dyneither_ptr _tmp1F7=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output)
# 998
return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(({const char*_tmp1F8="\\textbf{";_tag_dyneither(_tmp1F8,sizeof(char),9);}),(struct _dyneither_ptr)_tmp1F7),({const char*_tmp1F9="}";_tag_dyneither(_tmp1F9,sizeof(char),2);})),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp1F7));else{
return Cyc_PP_text(_tmp1F7);}}
# 1003
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}
# 1014 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp1FA=e->r;void*_tmp1FB=_tmp1FA;struct Cyc_Absyn_Exp*_tmp1FF;struct Cyc_Absyn_Exp*_tmp1FE;enum Cyc_Absyn_Primop _tmp1FD;switch(*((int*)_tmp1FB)){case 0: _LL186: _LL187:
 goto _LL189;case 1: _LL188: _LL189:
 return 10000;case 2: _LL18A: _tmp1FD=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1FB)->f1;_LL18B: {
# 1019
enum Cyc_Absyn_Primop _tmp1FC=_tmp1FD;switch(_tmp1FC){case Cyc_Absyn_Plus: _LL1D7: _LL1D8:
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
# 1041
 return 20;case 4: _LL18E: _LL18F:
 return 130;case 5: _LL190: _LL191:
 return 30;case 6: _LL192: _LL193:
 return 35;case 7: _LL194: _LL195:
 return 30;case 8: _LL196: _LL197:
 return 10;case 9: _LL198: _LL199:
 return 140;case 10: _LL19A: _LL19B:
 return 130;case 11: _LL19C: _tmp1FE=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp1FB)->f1;_LL19D:
 return Cyc_Absynpp_exp_prec(_tmp1FE);case 12: _LL19E: _tmp1FF=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp1FB)->f1;_LL19F:
 return Cyc_Absynpp_exp_prec(_tmp1FF);case 13: _LL1A0: _LL1A1:
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
# 1081
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1085
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp200=e->r;void*_tmp201=_tmp200;struct Cyc_Absyn_Stmt*_tmp2CF;struct Cyc_Core_Opt*_tmp2CE;struct Cyc_List_List*_tmp2CD;struct Cyc_Absyn_Exp*_tmp2CC;struct Cyc_Absyn_Exp*_tmp2CB;int _tmp2CA;struct Cyc_Absyn_Exp*_tmp2C9;void**_tmp2C8;struct Cyc_Absyn_Exp*_tmp2C7;int _tmp2C6;struct Cyc_Absyn_Enumfield*_tmp2C5;struct Cyc_Absyn_Enumfield*_tmp2C4;struct Cyc_List_List*_tmp2C3;struct Cyc_Absyn_Datatypefield*_tmp2C2;struct Cyc_List_List*_tmp2C1;struct _tuple0*_tmp2C0;struct Cyc_List_List*_tmp2BF;struct Cyc_List_List*_tmp2BE;struct Cyc_Absyn_Exp*_tmp2BD;void*_tmp2BC;struct Cyc_Absyn_Vardecl*_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BA;struct Cyc_Absyn_Exp*_tmp2B9;struct Cyc_List_List*_tmp2B8;struct _tuple8*_tmp2B7;struct Cyc_List_List*_tmp2B6;struct Cyc_List_List*_tmp2B5;struct Cyc_Absyn_Exp*_tmp2B4;struct Cyc_Absyn_Exp*_tmp2B3;struct Cyc_Absyn_Exp*_tmp2B2;struct _dyneither_ptr*_tmp2B1;struct Cyc_Absyn_Exp*_tmp2B0;struct _dyneither_ptr*_tmp2AF;struct Cyc_Absyn_Exp*_tmp2AE;void*_tmp2AD;struct Cyc_List_List*_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AB;struct _dyneither_ptr*_tmp2AA;int _tmp2A9;struct _dyneither_ptr _tmp2A8;void*_tmp2A7;struct Cyc_Absyn_Exp*_tmp2A6;void*_tmp2A5;struct Cyc_Absyn_Exp*_tmp2A4;struct Cyc_Absyn_Exp*_tmp2A3;struct Cyc_Absyn_Exp*_tmp2A2;void*_tmp2A1;struct Cyc_Absyn_Exp*_tmp2A0;struct Cyc_Absyn_Exp*_tmp29F;struct Cyc_Absyn_Exp*_tmp29E;struct Cyc_Absyn_Exp*_tmp29D;struct Cyc_Absyn_Exp*_tmp29C;struct Cyc_List_List*_tmp29B;struct Cyc_Absyn_Exp*_tmp29A;struct Cyc_Absyn_Exp*_tmp299;struct Cyc_Absyn_Exp*_tmp298;struct Cyc_Absyn_Exp*_tmp297;struct Cyc_Absyn_Exp*_tmp296;struct Cyc_Absyn_Exp*_tmp295;struct Cyc_Absyn_Exp*_tmp294;struct Cyc_Absyn_Exp*_tmp293;struct Cyc_Absyn_Exp*_tmp292;struct Cyc_Absyn_Exp*_tmp291;enum Cyc_Absyn_Incrementor _tmp290;struct Cyc_Absyn_Exp*_tmp28F;struct Cyc_Core_Opt*_tmp28E;struct Cyc_Absyn_Exp*_tmp28D;enum Cyc_Absyn_Primop _tmp28C;struct Cyc_List_List*_tmp28B;void*_tmp28A;union Cyc_Absyn_Cnst _tmp289;switch(*((int*)_tmp201)){case 0: _LL200: _tmp289=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL201:
 s=Cyc_Absynpp_cnst2doc(_tmp289);goto _LL1FF;case 1: _LL202: _tmp28A=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL203:
 s=Cyc_Absynpp_qvar2doc(Cyc_Absyn_binding2qvar(_tmp28A));goto _LL1FF;case 2: _LL204: _tmp28C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp28B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL205:
 s=Cyc_Absynpp_primapp2doc(myprec,_tmp28C,_tmp28B);goto _LL1FF;case 3: _LL206: _tmp28F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp28E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp28D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL207:
# 1093
 s=({struct Cyc_PP_Doc*_tmp202[5];_tmp202[4]=
# 1097
Cyc_Absynpp_exp2doc_prec(myprec,_tmp28D);_tmp202[3]=
# 1096
Cyc_PP_text(({const char*_tmp204="= ";_tag_dyneither(_tmp204,sizeof(char),3);}));_tmp202[2]=
# 1095
_tmp28E == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp28E->v);_tmp202[1]=
# 1094
Cyc_PP_text(({const char*_tmp203=" ";_tag_dyneither(_tmp203,sizeof(char),2);}));_tmp202[0]=
# 1093
Cyc_Absynpp_exp2doc_prec(myprec,_tmp28F);Cyc_PP_cat(_tag_dyneither(_tmp202,sizeof(struct Cyc_PP_Doc*),5));});
# 1098
goto _LL1FF;case 4: _LL208: _tmp291=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp290=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL209: {
# 1100
struct Cyc_PP_Doc*_tmp205=Cyc_Absynpp_exp2doc_prec(myprec,_tmp291);
{enum Cyc_Absyn_Incrementor _tmp206=_tmp290;switch(_tmp206){case Cyc_Absyn_PreInc: _LL251: _LL252:
 s=({struct Cyc_PP_Doc*_tmp207[2];_tmp207[1]=_tmp205;_tmp207[0]=Cyc_PP_text(({const char*_tmp208="++";_tag_dyneither(_tmp208,sizeof(char),3);}));Cyc_PP_cat(_tag_dyneither(_tmp207,sizeof(struct Cyc_PP_Doc*),2));});goto _LL250;case Cyc_Absyn_PreDec: _LL253: _LL254:
 s=({struct Cyc_PP_Doc*_tmp209[2];_tmp209[1]=_tmp205;_tmp209[0]=Cyc_PP_text(({const char*_tmp20A="--";_tag_dyneither(_tmp20A,sizeof(char),3);}));Cyc_PP_cat(_tag_dyneither(_tmp209,sizeof(struct Cyc_PP_Doc*),2));});goto _LL250;case Cyc_Absyn_PostInc: _LL255: _LL256:
 s=({struct Cyc_PP_Doc*_tmp20B[2];_tmp20B[1]=Cyc_PP_text(({const char*_tmp20C="++";_tag_dyneither(_tmp20C,sizeof(char),3);}));_tmp20B[0]=_tmp205;Cyc_PP_cat(_tag_dyneither(_tmp20B,sizeof(struct Cyc_PP_Doc*),2));});goto _LL250;default: _LL257: _LL258:
 s=({struct Cyc_PP_Doc*_tmp20D[2];_tmp20D[1]=Cyc_PP_text(({const char*_tmp20E="--";_tag_dyneither(_tmp20E,sizeof(char),3);}));_tmp20D[0]=_tmp205;Cyc_PP_cat(_tag_dyneither(_tmp20D,sizeof(struct Cyc_PP_Doc*),2));});goto _LL250;}_LL250:;}
# 1107
goto _LL1FF;}case 5: _LL20A: _tmp294=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp293=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp292=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL20B:
# 1109
 s=({struct Cyc_PP_Doc*_tmp20F[5];_tmp20F[4]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp292);_tmp20F[3]=Cyc_PP_text(({const char*_tmp211=" : ";_tag_dyneither(_tmp211,sizeof(char),4);}));_tmp20F[2]=
# 1109
Cyc_Absynpp_exp2doc_prec(0,_tmp293);_tmp20F[1]=Cyc_PP_text(({const char*_tmp210=" ? ";_tag_dyneither(_tmp210,sizeof(char),4);}));_tmp20F[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp294);Cyc_PP_cat(_tag_dyneither(_tmp20F,sizeof(struct Cyc_PP_Doc*),5));});
# 1111
goto _LL1FF;case 6: _LL20C: _tmp296=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp295=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL20D:
# 1113
 s=({struct Cyc_PP_Doc*_tmp212[3];_tmp212[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp295);_tmp212[1]=Cyc_PP_text(({const char*_tmp213=" && ";_tag_dyneither(_tmp213,sizeof(char),5);}));_tmp212[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp296);Cyc_PP_cat(_tag_dyneither(_tmp212,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1FF;case 7: _LL20E: _tmp298=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp297=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL20F:
# 1116
 s=({struct Cyc_PP_Doc*_tmp214[3];_tmp214[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp297);_tmp214[1]=Cyc_PP_text(({const char*_tmp215=" || ";_tag_dyneither(_tmp215,sizeof(char),5);}));_tmp214[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp298);Cyc_PP_cat(_tag_dyneither(_tmp214,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1FF;case 8: _LL210: _tmp29A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp299=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL211:
# 1121
 s=({struct Cyc_PP_Doc*_tmp216[5];_tmp216[4]=Cyc_PP_text(({const char*_tmp219=")";_tag_dyneither(_tmp219,sizeof(char),2);}));_tmp216[3]=Cyc_Absynpp_exp2doc(_tmp299);_tmp216[2]=Cyc_PP_text(({const char*_tmp218=", ";_tag_dyneither(_tmp218,sizeof(char),3);}));_tmp216[1]=Cyc_Absynpp_exp2doc(_tmp29A);_tmp216[0]=Cyc_PP_text(({const char*_tmp217="(";_tag_dyneither(_tmp217,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp216,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL1FF;case 9: _LL212: _tmp29C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp29B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL213:
# 1124
 s=({struct Cyc_PP_Doc*_tmp21A[4];_tmp21A[3]=
# 1127
Cyc_PP_text(({const char*_tmp21C=")";_tag_dyneither(_tmp21C,sizeof(char),2);}));_tmp21A[2]=
# 1126
Cyc_Absynpp_exps2doc_prec(20,_tmp29B);_tmp21A[1]=
# 1125
Cyc_PP_text(({const char*_tmp21B="(";_tag_dyneither(_tmp21B,sizeof(char),2);}));_tmp21A[0]=
# 1124
Cyc_Absynpp_exp2doc_prec(myprec,_tmp29C);Cyc_PP_cat(_tag_dyneither(_tmp21A,sizeof(struct Cyc_PP_Doc*),4));});
# 1128
goto _LL1FF;case 10: _LL214: _tmp29D=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL215:
# 1130
 s=({struct Cyc_PP_Doc*_tmp21D[2];_tmp21D[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29D);_tmp21D[0]=Cyc_PP_text(({const char*_tmp21E="throw ";_tag_dyneither(_tmp21E,sizeof(char),7);}));Cyc_PP_cat(_tag_dyneither(_tmp21D,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL1FF;case 11: _LL216: _tmp29E=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL217:
# 1133
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp29E);
goto _LL1FF;case 12: _LL218: _tmp29F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL219:
# 1136
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp29F);
goto _LL1FF;case 13: _LL21A: _tmp2A1=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2A0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL21B:
# 1139
 s=({struct Cyc_PP_Doc*_tmp21F[4];_tmp21F[3]=
# 1142
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A0);_tmp21F[2]=
# 1141
Cyc_PP_text(({const char*_tmp221=")";_tag_dyneither(_tmp221,sizeof(char),2);}));_tmp21F[1]=
# 1140
Cyc_Absynpp_typ2doc(_tmp2A1);_tmp21F[0]=
# 1139
Cyc_PP_text(({const char*_tmp220="(";_tag_dyneither(_tmp220,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp21F,sizeof(struct Cyc_PP_Doc*),4));});
# 1143
goto _LL1FF;case 14: _LL21C: _tmp2A2=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL21D:
# 1145
 s=({struct Cyc_PP_Doc*_tmp222[2];_tmp222[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A2);_tmp222[0]=
# 1145
Cyc_PP_text(({const char*_tmp223="&";_tag_dyneither(_tmp223,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp222,sizeof(struct Cyc_PP_Doc*),2));});
# 1147
goto _LL1FF;case 15: _LL21E: _tmp2A4=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2A3=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL21F:
# 1149
 if(_tmp2A4 == 0)
s=({struct Cyc_PP_Doc*_tmp224[2];_tmp224[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A3);_tmp224[0]=Cyc_PP_text(({const char*_tmp225="new ";_tag_dyneither(_tmp225,sizeof(char),5);}));Cyc_PP_cat(_tag_dyneither(_tmp224,sizeof(struct Cyc_PP_Doc*),2));});else{
# 1152
s=({struct Cyc_PP_Doc*_tmp226[4];_tmp226[3]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A3);_tmp226[2]=
# 1152
Cyc_PP_text(({const char*_tmp228=") ";_tag_dyneither(_tmp228,sizeof(char),3);}));_tmp226[1]=Cyc_Absynpp_exp2doc(_tmp2A4);_tmp226[0]=Cyc_PP_text(({const char*_tmp227="rnew(";_tag_dyneither(_tmp227,sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp226,sizeof(struct Cyc_PP_Doc*),4));});}
# 1154
goto _LL1FF;case 16: _LL220: _tmp2A5=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL221:
 s=({struct Cyc_PP_Doc*_tmp229[3];_tmp229[2]=Cyc_PP_text(({const char*_tmp22B=")";_tag_dyneither(_tmp22B,sizeof(char),2);}));_tmp229[1]=Cyc_Absynpp_typ2doc(_tmp2A5);_tmp229[0]=Cyc_PP_text(({const char*_tmp22A="sizeof(";_tag_dyneither(_tmp22A,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp229,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1FF;case 17: _LL222: _tmp2A6=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL223:
 s=({struct Cyc_PP_Doc*_tmp22C[3];_tmp22C[2]=Cyc_PP_text(({const char*_tmp22E=")";_tag_dyneither(_tmp22E,sizeof(char),2);}));_tmp22C[1]=Cyc_Absynpp_exp2doc(_tmp2A6);_tmp22C[0]=Cyc_PP_text(({const char*_tmp22D="sizeof(";_tag_dyneither(_tmp22D,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp22C,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1FF;case 38: _LL224: _tmp2A7=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL225:
 s=({struct Cyc_PP_Doc*_tmp22F[3];_tmp22F[2]=Cyc_PP_text(({const char*_tmp231=")";_tag_dyneither(_tmp231,sizeof(char),2);}));_tmp22F[1]=Cyc_Absynpp_typ2doc(_tmp2A7);_tmp22F[0]=Cyc_PP_text(({const char*_tmp230="valueof(";_tag_dyneither(_tmp230,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp22F,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1FF;case 39: _LL226: _tmp2A9=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2A8=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL227:
# 1159
 if(_tmp2A9)
s=({struct Cyc_PP_Doc*_tmp232[4];_tmp232[3]=Cyc_PP_text(({const char*_tmp235=")";_tag_dyneither(_tmp235,sizeof(char),2);}));_tmp232[2]=Cyc_PP_text(_tmp2A8);_tmp232[1]=Cyc_PP_text(({const char*_tmp234=" volatile (";_tag_dyneither(_tmp234,sizeof(char),12);}));_tmp232[0]=Cyc_PP_text(({const char*_tmp233="__asm__";_tag_dyneither(_tmp233,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp232,sizeof(struct Cyc_PP_Doc*),4));});else{
# 1162
s=({struct Cyc_PP_Doc*_tmp236[3];_tmp236[2]=Cyc_PP_text(({const char*_tmp238=")";_tag_dyneither(_tmp238,sizeof(char),2);}));_tmp236[1]=Cyc_PP_text(_tmp2A8);_tmp236[0]=Cyc_PP_text(({const char*_tmp237="__asm__(";_tag_dyneither(_tmp237,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp236,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LL1FF;case 37: _LL228: _tmp2AB=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2AA=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL229:
# 1165
 s=({struct Cyc_PP_Doc*_tmp239[5];_tmp239[4]=Cyc_PP_text(({const char*_tmp23C=")";_tag_dyneither(_tmp23C,sizeof(char),2);}));_tmp239[3]=Cyc_PP_textptr(_tmp2AA);_tmp239[2]=Cyc_PP_text(({const char*_tmp23B=".";_tag_dyneither(_tmp23B,sizeof(char),2);}));_tmp239[1]=Cyc_Absynpp_exp2doc(_tmp2AB);_tmp239[0]=Cyc_PP_text(({const char*_tmp23A="tagcheck(";_tag_dyneither(_tmp23A,sizeof(char),10);}));Cyc_PP_cat(_tag_dyneither(_tmp239,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL1FF;case 18: _LL22A: _tmp2AD=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2AC=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL22B:
# 1168
 s=({struct Cyc_PP_Doc*_tmp23D[3];_tmp23D[2]=Cyc_PP_text(({const char*_tmp23F=",";_tag_dyneither(_tmp23F,sizeof(char),2);}));_tmp23D[1]=Cyc_Absynpp_typ2doc(_tmp2AD);_tmp23D[0]=Cyc_PP_text(({const char*_tmp23E="offsetof(";_tag_dyneither(_tmp23E,sizeof(char),10);}));Cyc_PP_cat(_tag_dyneither(_tmp23D,sizeof(struct Cyc_PP_Doc*),3));});
for(0;_tmp2AC != 0;_tmp2AC=_tmp2AC->tl){
void*_tmp240=(void*)_tmp2AC->hd;void*_tmp241=_tmp240;unsigned int _tmp24A;struct _dyneither_ptr*_tmp249;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp241)->tag == 0){_LL25A: _tmp249=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp241)->f1;_LL25B:
 s=({struct Cyc_PP_Doc*_tmp242[3];_tmp242[2]=
_tmp2AC->tl != 0?Cyc_PP_text(({const char*_tmp243=".";_tag_dyneither(_tmp243,sizeof(char),2);})): Cyc_PP_nil_doc();_tmp242[1]=
# 1171
Cyc_PP_textptr(_tmp249);_tmp242[0]=s;Cyc_PP_cat(_tag_dyneither(_tmp242,sizeof(struct Cyc_PP_Doc*),3));});
# 1173
goto _LL259;}else{_LL25C: _tmp24A=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp241)->f1;_LL25D:
 s=({struct Cyc_PP_Doc*_tmp244[3];_tmp244[2]=
_tmp2AC->tl != 0?Cyc_PP_text(({const char*_tmp248=".";_tag_dyneither(_tmp248,sizeof(char),2);})): Cyc_PP_nil_doc();_tmp244[1]=
# 1174
Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp247;_tmp247.tag=1;_tmp247.f1=(unsigned long)((int)_tmp24A);({void*_tmp245[1]={& _tmp247};Cyc_aprintf(({const char*_tmp246="%d";_tag_dyneither(_tmp246,sizeof(char),3);}),_tag_dyneither(_tmp245,sizeof(void*),1));});}));_tmp244[0]=s;Cyc_PP_cat(_tag_dyneither(_tmp244,sizeof(struct Cyc_PP_Doc*),3));});
# 1176
goto _LL259;}_LL259:;}
# 1178
s=({struct Cyc_PP_Doc*_tmp24B[2];_tmp24B[1]=Cyc_PP_text(({const char*_tmp24C=")";_tag_dyneither(_tmp24C,sizeof(char),2);}));_tmp24B[0]=s;Cyc_PP_cat(_tag_dyneither(_tmp24B,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL1FF;case 19: _LL22C: _tmp2AE=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL22D:
# 1181
 s=({struct Cyc_PP_Doc*_tmp24D[2];_tmp24D[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AE);_tmp24D[0]=Cyc_PP_text(({const char*_tmp24E="*";_tag_dyneither(_tmp24E,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp24D,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL1FF;case 20: _LL22E: _tmp2B0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2AF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL22F:
# 1184
 s=({struct Cyc_PP_Doc*_tmp24F[3];_tmp24F[2]=Cyc_PP_textptr(_tmp2AF);_tmp24F[1]=Cyc_PP_text(({const char*_tmp250=".";_tag_dyneither(_tmp250,sizeof(char),2);}));_tmp24F[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B0);Cyc_PP_cat(_tag_dyneither(_tmp24F,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1FF;case 21: _LL230: _tmp2B2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2B1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL231:
# 1187
 s=({struct Cyc_PP_Doc*_tmp251[3];_tmp251[2]=Cyc_PP_textptr(_tmp2B1);_tmp251[1]=Cyc_PP_text(({const char*_tmp252="->";_tag_dyneither(_tmp252,sizeof(char),3);}));_tmp251[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B2);Cyc_PP_cat(_tag_dyneither(_tmp251,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1FF;case 22: _LL232: _tmp2B4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2B3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL233:
# 1190
 s=({struct Cyc_PP_Doc*_tmp253[4];_tmp253[3]=
# 1193
Cyc_PP_text(({const char*_tmp255="]";_tag_dyneither(_tmp255,sizeof(char),2);}));_tmp253[2]=
# 1192
Cyc_Absynpp_exp2doc(_tmp2B3);_tmp253[1]=
# 1191
Cyc_PP_text(({const char*_tmp254="[";_tag_dyneither(_tmp254,sizeof(char),2);}));_tmp253[0]=
# 1190
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B4);Cyc_PP_cat(_tag_dyneither(_tmp253,sizeof(struct Cyc_PP_Doc*),4));});
# 1194
goto _LL1FF;case 23: _LL234: _tmp2B5=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL235:
# 1196
 s=({struct Cyc_PP_Doc*_tmp256[4];_tmp256[3]=
# 1199
Cyc_PP_text(({const char*_tmp258=")";_tag_dyneither(_tmp258,sizeof(char),2);}));_tmp256[2]=
# 1198
Cyc_Absynpp_exps2doc_prec(20,_tmp2B5);_tmp256[1]=
# 1197
Cyc_PP_text(({const char*_tmp257="(";_tag_dyneither(_tmp257,sizeof(char),2);}));_tmp256[0]=
# 1196
Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dyneither(_tmp256,sizeof(struct Cyc_PP_Doc*),4));});
# 1200
goto _LL1FF;case 24: _LL236: _tmp2B7=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2B6=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL237:
# 1202
 s=({struct Cyc_PP_Doc*_tmp259[4];_tmp259[3]=
# 1205
Cyc_Absynpp_group_braces(({const char*_tmp25C=",";_tag_dyneither(_tmp25C,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2B6));_tmp259[2]=
# 1204
Cyc_PP_text(({const char*_tmp25B=")";_tag_dyneither(_tmp25B,sizeof(char),2);}));_tmp259[1]=
# 1203
Cyc_Absynpp_typ2doc((*_tmp2B7).f3);_tmp259[0]=
# 1202
Cyc_PP_text(({const char*_tmp25A="(";_tag_dyneither(_tmp25A,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp259,sizeof(struct Cyc_PP_Doc*),4));});
# 1206
goto _LL1FF;case 25: _LL238: _tmp2B8=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL239:
# 1208
 s=Cyc_Absynpp_group_braces(({const char*_tmp25D=",";_tag_dyneither(_tmp25D,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2B8));
goto _LL1FF;case 26: _LL23A: _tmp2BB=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2BA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp2B9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL23B:
# 1211
 s=({struct Cyc_PP_Doc*_tmp25E[8];_tmp25E[7]=
# 1217
Cyc_Absynpp_rb();_tmp25E[6]=
# 1216
Cyc_Absynpp_exp2doc(_tmp2B9);_tmp25E[5]=
# 1215
Cyc_PP_text(({const char*_tmp261=" : ";_tag_dyneither(_tmp261,sizeof(char),4);}));_tmp25E[4]=
# 1214
Cyc_Absynpp_exp2doc(_tmp2BA);_tmp25E[3]=
# 1213
Cyc_PP_text(({const char*_tmp260=" < ";_tag_dyneither(_tmp260,sizeof(char),4);}));_tmp25E[2]=
# 1212
Cyc_PP_text(*(*_tmp2BB->name).f2);_tmp25E[1]=
# 1211
Cyc_PP_text(({const char*_tmp25F="for ";_tag_dyneither(_tmp25F,sizeof(char),5);}));_tmp25E[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dyneither(_tmp25E,sizeof(struct Cyc_PP_Doc*),8));});
# 1218
goto _LL1FF;case 27: _LL23C: _tmp2BD=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2BC=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL23D:
# 1220
 s=({struct Cyc_PP_Doc*_tmp262[7];_tmp262[6]=
# 1225
Cyc_Absynpp_rb();_tmp262[5]=
# 1224
Cyc_Absynpp_typ2doc(_tmp2BC);_tmp262[4]=
# 1223
Cyc_PP_text(({const char*_tmp265=" : ";_tag_dyneither(_tmp265,sizeof(char),4);}));_tmp262[3]=
# 1222
Cyc_Absynpp_exp2doc(_tmp2BD);_tmp262[2]=
# 1221
Cyc_PP_text(({const char*_tmp264=" < ";_tag_dyneither(_tmp264,sizeof(char),4);}));_tmp262[1]=
# 1220
Cyc_PP_text(({const char*_tmp263="for x ";_tag_dyneither(_tmp263,sizeof(char),7);}));_tmp262[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dyneither(_tmp262,sizeof(struct Cyc_PP_Doc*),7));});
# 1226
goto _LL1FF;case 28: _LL23E: _tmp2C0=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2BF=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp2BE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL23F: {
# 1228
struct Cyc_List_List*_tmp266=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2BE);
s=({struct Cyc_PP_Doc*_tmp267[2];_tmp267[1]=
Cyc_Absynpp_group_braces(({const char*_tmp268=",";_tag_dyneither(_tmp268,sizeof(char),2);}),
_tmp2BF != 0?({struct Cyc_List_List*_tmp269=_cycalloc(sizeof(*_tmp269));_tmp269->hd=Cyc_Absynpp_tps2doc(_tmp2BF);_tmp269->tl=_tmp266;_tmp269;}): _tmp266);_tmp267[0]=
# 1229
Cyc_Absynpp_qvar2doc(_tmp2C0);Cyc_PP_cat(_tag_dyneither(_tmp267,sizeof(struct Cyc_PP_Doc*),2));});
# 1232
goto _LL1FF;}case 29: _LL240: _tmp2C1=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL241:
# 1234
 s=Cyc_Absynpp_group_braces(({const char*_tmp26A=",";_tag_dyneither(_tmp26A,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C1));
goto _LL1FF;case 30: _LL242: _tmp2C3=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2C2=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL243:
# 1237
 if(_tmp2C3 == 0)
# 1239
s=Cyc_Absynpp_qvar2doc(_tmp2C2->name);else{
# 1241
s=({struct Cyc_PP_Doc*_tmp26B[2];_tmp26B[1]=
Cyc_PP_egroup(({const char*_tmp26C="(";_tag_dyneither(_tmp26C,sizeof(char),2);}),({const char*_tmp26D=")";_tag_dyneither(_tmp26D,sizeof(char),2);}),({const char*_tmp26E=",";_tag_dyneither(_tmp26E,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp2C3));_tmp26B[0]=
# 1241
Cyc_Absynpp_qvar2doc(_tmp2C2->name);Cyc_PP_cat(_tag_dyneither(_tmp26B,sizeof(struct Cyc_PP_Doc*),2));});}
# 1243
goto _LL1FF;case 31: _LL244: _tmp2C4=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL245:
# 1245
 s=Cyc_Absynpp_qvar2doc(_tmp2C4->name);
goto _LL1FF;case 32: _LL246: _tmp2C5=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL247:
# 1248
 s=Cyc_Absynpp_qvar2doc(_tmp2C5->name);
goto _LL1FF;case 33: _LL248: _tmp2CA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).is_calloc;_tmp2C9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).rgn;_tmp2C8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).elt_type;_tmp2C7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).num_elts;_tmp2C6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).inline_call;_LL249:
# 1253
 if(_tmp2CA){
# 1255
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp2C8)),0);
if(_tmp2C9 == 0)
s=({struct Cyc_PP_Doc*_tmp26F[5];_tmp26F[4]=Cyc_PP_text(({const char*_tmp272=")";_tag_dyneither(_tmp272,sizeof(char),2);}));_tmp26F[3]=Cyc_Absynpp_exp2doc(st);_tmp26F[2]=Cyc_PP_text(({const char*_tmp271=",";_tag_dyneither(_tmp271,sizeof(char),2);}));_tmp26F[1]=Cyc_Absynpp_exp2doc(_tmp2C7);_tmp26F[0]=Cyc_PP_text(({const char*_tmp270="calloc(";_tag_dyneither(_tmp270,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp26F,sizeof(struct Cyc_PP_Doc*),5));});else{
# 1259
s=({struct Cyc_PP_Doc*_tmp273[7];_tmp273[6]=
Cyc_PP_text(({const char*_tmp277=")";_tag_dyneither(_tmp277,sizeof(char),2);}));_tmp273[5]=Cyc_Absynpp_exp2doc(st);_tmp273[4]=Cyc_PP_text(({const char*_tmp276=",";_tag_dyneither(_tmp276,sizeof(char),2);}));_tmp273[3]=Cyc_Absynpp_exp2doc(_tmp2C7);_tmp273[2]=
# 1259
Cyc_PP_text(({const char*_tmp275=",";_tag_dyneither(_tmp275,sizeof(char),2);}));_tmp273[1]=Cyc_Absynpp_exp2doc(_tmp2C9);_tmp273[0]=Cyc_PP_text(({const char*_tmp274="rcalloc(";_tag_dyneither(_tmp274,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp273,sizeof(struct Cyc_PP_Doc*),7));});}}else{
# 1262
struct Cyc_Absyn_Exp*new_e;
# 1264
if(_tmp2C8 == 0)
new_e=_tmp2C7;else{
# 1267
new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp2C8,0),_tmp2C7,0);}
# 1269
if(_tmp2C9 == 0)
s=({struct Cyc_PP_Doc*_tmp278[3];_tmp278[2]=Cyc_PP_text(({const char*_tmp27A=")";_tag_dyneither(_tmp27A,sizeof(char),2);}));_tmp278[1]=Cyc_Absynpp_exp2doc(new_e);_tmp278[0]=Cyc_PP_text(({const char*_tmp279="malloc(";_tag_dyneither(_tmp279,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp278,sizeof(struct Cyc_PP_Doc*),3));});else{
# 1272
if(_tmp2C6)
s=({struct Cyc_PP_Doc*_tmp27B[5];_tmp27B[4]=
Cyc_PP_text(({const char*_tmp27E=")";_tag_dyneither(_tmp27E,sizeof(char),2);}));_tmp27B[3]=Cyc_Absynpp_exp2doc(new_e);_tmp27B[2]=
# 1273
Cyc_PP_text(({const char*_tmp27D=",";_tag_dyneither(_tmp27D,sizeof(char),2);}));_tmp27B[1]=Cyc_Absynpp_exp2doc(_tmp2C9);_tmp27B[0]=Cyc_PP_text(({const char*_tmp27C="rmalloc_inline(";_tag_dyneither(_tmp27C,sizeof(char),16);}));Cyc_PP_cat(_tag_dyneither(_tmp27B,sizeof(struct Cyc_PP_Doc*),5));});else{
# 1276
s=({struct Cyc_PP_Doc*_tmp27F[5];_tmp27F[4]=
Cyc_PP_text(({const char*_tmp282=")";_tag_dyneither(_tmp282,sizeof(char),2);}));_tmp27F[3]=Cyc_Absynpp_exp2doc(new_e);_tmp27F[2]=
# 1276
Cyc_PP_text(({const char*_tmp281=",";_tag_dyneither(_tmp281,sizeof(char),2);}));_tmp27F[1]=Cyc_Absynpp_exp2doc(_tmp2C9);_tmp27F[0]=Cyc_PP_text(({const char*_tmp280="rmalloc(";_tag_dyneither(_tmp280,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp27F,sizeof(struct Cyc_PP_Doc*),5));});}}}
# 1280
goto _LL1FF;case 34: _LL24A: _tmp2CC=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2CB=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL24B:
# 1282
 s=({struct Cyc_PP_Doc*_tmp283[3];_tmp283[2]=
# 1284
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2CB);_tmp283[1]=
# 1283
Cyc_PP_text(({const char*_tmp284=" :=: ";_tag_dyneither(_tmp284,sizeof(char),6);}));_tmp283[0]=
# 1282
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2CC);Cyc_PP_cat(_tag_dyneither(_tmp283,sizeof(struct Cyc_PP_Doc*),3));});
# 1285
goto _LL1FF;case 35: _LL24C: _tmp2CE=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2CD=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL24D:
# 1288
 s=Cyc_Absynpp_group_braces(({const char*_tmp285=",";_tag_dyneither(_tmp285,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2CD));
goto _LL1FF;default: _LL24E: _tmp2CF=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL24F:
# 1291
 s=({struct Cyc_PP_Doc*_tmp286[7];_tmp286[6]=
# 1293
Cyc_PP_text(({const char*_tmp288=")";_tag_dyneither(_tmp288,sizeof(char),2);}));_tmp286[5]=Cyc_Absynpp_rb();_tmp286[4]=Cyc_PP_blank_doc();_tmp286[3]=
# 1292
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2CF,1));_tmp286[2]=
# 1291
Cyc_PP_blank_doc();_tmp286[1]=Cyc_Absynpp_lb();_tmp286[0]=Cyc_PP_text(({const char*_tmp287="(";_tag_dyneither(_tmp287,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp286,sizeof(struct Cyc_PP_Doc*),7));});
# 1294
goto _LL1FF;}_LL1FF:;}
# 1296
if(inprec >= myprec)
s=({struct Cyc_PP_Doc*_tmp2D0[3];_tmp2D0[2]=Cyc_PP_text(({const char*_tmp2D2=")";_tag_dyneither(_tmp2D2,sizeof(char),2);}));_tmp2D0[1]=s;_tmp2D0[0]=Cyc_PP_text(({const char*_tmp2D1="(";_tag_dyneither(_tmp2D1,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp2D0,sizeof(struct Cyc_PP_Doc*),3));});
return s;}
# 1301
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp2D3=d;struct _dyneither_ptr*_tmp2DA;struct Cyc_Absyn_Exp*_tmp2D9;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2D3)->tag == 0){_LL25F: _tmp2D9=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2D3)->f1;_LL260:
 return({struct Cyc_PP_Doc*_tmp2D4[3];_tmp2D4[2]=Cyc_PP_text(({const char*_tmp2D6="]";_tag_dyneither(_tmp2D6,sizeof(char),2);}));_tmp2D4[1]=Cyc_Absynpp_exp2doc(_tmp2D9);_tmp2D4[0]=Cyc_PP_text(({const char*_tmp2D5=".[";_tag_dyneither(_tmp2D5,sizeof(char),3);}));Cyc_PP_cat(_tag_dyneither(_tmp2D4,sizeof(struct Cyc_PP_Doc*),3));});}else{_LL261: _tmp2DA=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2D3)->f1;_LL262:
 return({struct Cyc_PP_Doc*_tmp2D7[2];_tmp2D7[1]=Cyc_PP_textptr(_tmp2DA);_tmp2D7[0]=Cyc_PP_text(({const char*_tmp2D8=".";_tag_dyneither(_tmp2D8,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp2D7,sizeof(struct Cyc_PP_Doc*),2));});}_LL25E:;}
# 1308
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
return({struct Cyc_PP_Doc*_tmp2DB[2];_tmp2DB[1]=
Cyc_Absynpp_exp2doc((*de).f2);_tmp2DB[0]=
# 1310
Cyc_PP_egroup(({const char*_tmp2DC="";_tag_dyneither(_tmp2DC,sizeof(char),1);}),({const char*_tmp2DD="=";_tag_dyneither(_tmp2DD,sizeof(char),2);}),({const char*_tmp2DE="=";_tag_dyneither(_tmp2DE,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));Cyc_PP_cat(_tag_dyneither(_tmp2DB,sizeof(struct Cyc_PP_Doc*),2));});}}
# 1314
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
return Cyc_PP_group(({const char*_tmp2DF="";_tag_dyneither(_tmp2DF,sizeof(char),1);}),({const char*_tmp2E0="";_tag_dyneither(_tmp2E0,sizeof(char),1);}),({const char*_tmp2E1=",";_tag_dyneither(_tmp2E1,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
# 1318
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
# 1332
return(struct _dyneither_ptr)_dyneither_ptr_plus(_dyneither_ptr_plus(x,sizeof(char),index),sizeof(char),1);};}
# 1336
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp2F8=c;struct _dyneither_ptr _tmp31B;struct _dyneither_ptr _tmp31A;struct _dyneither_ptr _tmp319;enum Cyc_Absyn_Sign _tmp318;long long _tmp317;int _tmp316;int _tmp315;int _tmp314;enum Cyc_Absyn_Sign _tmp313;short _tmp312;struct _dyneither_ptr _tmp311;enum Cyc_Absyn_Sign _tmp310;char _tmp30F;switch((_tmp2F8.String_c).tag){case 2: _LL264: _tmp310=((_tmp2F8.Char_c).val).f1;_tmp30F=((_tmp2F8.Char_c).val).f2;_LL265:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2FB;_tmp2FB.tag=0;_tmp2FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp30F));({void*_tmp2F9[1]={& _tmp2FB};Cyc_aprintf(({const char*_tmp2FA="'%s'";_tag_dyneither(_tmp2FA,sizeof(char),5);}),_tag_dyneither(_tmp2F9,sizeof(void*),1));});}));case 3: _LL266: _tmp311=(_tmp2F8.Wchar_c).val;_LL267:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2FE;_tmp2FE.tag=0;_tmp2FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp311);({void*_tmp2FC[1]={& _tmp2FE};Cyc_aprintf(({const char*_tmp2FD="L'%s'";_tag_dyneither(_tmp2FD,sizeof(char),6);}),_tag_dyneither(_tmp2FC,sizeof(void*),1));});}));case 4: _LL268: _tmp313=((_tmp2F8.Short_c).val).f1;_tmp312=((_tmp2F8.Short_c).val).f2;_LL269:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp301;_tmp301.tag=1;_tmp301.f1=(unsigned long)((int)_tmp312);({void*_tmp2FF[1]={& _tmp301};Cyc_aprintf(({const char*_tmp300="%d";_tag_dyneither(_tmp300,sizeof(char),3);}),_tag_dyneither(_tmp2FF,sizeof(void*),1));});}));case 5: switch(((_tmp2F8.Int_c).val).f1){case Cyc_Absyn_None: _LL26A: _tmp314=((_tmp2F8.Int_c).val).f2;_LL26B:
 _tmp315=_tmp314;goto _LL26D;case Cyc_Absyn_Signed: _LL26C: _tmp315=((_tmp2F8.Int_c).val).f2;_LL26D:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp304;_tmp304.tag=1;_tmp304.f1=(unsigned long)_tmp315;({void*_tmp302[1]={& _tmp304};Cyc_aprintf(({const char*_tmp303="%d";_tag_dyneither(_tmp303,sizeof(char),3);}),_tag_dyneither(_tmp302,sizeof(void*),1));});}));default: _LL26E: _tmp316=((_tmp2F8.Int_c).val).f2;_LL26F:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp307;_tmp307.tag=1;_tmp307.f1=(unsigned int)_tmp316;({void*_tmp305[1]={& _tmp307};Cyc_aprintf(({const char*_tmp306="%u";_tag_dyneither(_tmp306,sizeof(char),3);}),_tag_dyneither(_tmp305,sizeof(void*),1));});}));}case 6: _LL270: _tmp318=((_tmp2F8.LongLong_c).val).f1;_tmp317=((_tmp2F8.LongLong_c).val).f2;_LL271:
# 1346
 return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)_tmp317));case 7: _LL272: _tmp319=((_tmp2F8.Float_c).val).f1;_LL273:
 return Cyc_PP_text(_tmp319);case 1: _LL274: _LL275:
 return Cyc_PP_text(({const char*_tmp308="NULL";_tag_dyneither(_tmp308,sizeof(char),5);}));case 8: _LL276: _tmp31A=(_tmp2F8.String_c).val;_LL277:
# 1350
 return({struct Cyc_PP_Doc*_tmp309[3];_tmp309[2]=Cyc_PP_text(({const char*_tmp30B="\"";_tag_dyneither(_tmp30B,sizeof(char),2);}));_tmp309[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp31A));_tmp309[0]=Cyc_PP_text(({const char*_tmp30A="\"";_tag_dyneither(_tmp30A,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp309,sizeof(struct Cyc_PP_Doc*),3));});default: _LL278: _tmp31B=(_tmp2F8.Wstring_c).val;_LL279:
# 1352
 return({struct Cyc_PP_Doc*_tmp30C[3];_tmp30C[2]=Cyc_PP_text(({const char*_tmp30E="\"";_tag_dyneither(_tmp30E,sizeof(char),2);}));_tmp30C[1]=Cyc_PP_text(_tmp31B);_tmp30C[0]=Cyc_PP_text(({const char*_tmp30D="L\"";_tag_dyneither(_tmp30D,sizeof(char),3);}));Cyc_PP_cat(_tag_dyneither(_tmp30C,sizeof(struct Cyc_PP_Doc*),3));});}_LL263:;}
# 1356
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if(p == Cyc_Absyn_Numelts){
if(es == 0  || es->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C[0]=({struct Cyc_Core_Failure_exn_struct _tmp31D;_tmp31D.tag=Cyc_Core_Failure;_tmp31D.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp320;_tmp320.tag=0;_tmp320.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));({void*_tmp31E[1]={& _tmp320};Cyc_aprintf(({const char*_tmp31F="Absynpp::primapp2doc Numelts: %s with bad args";_tag_dyneither(_tmp31F,sizeof(char),47);}),_tag_dyneither(_tmp31E,sizeof(void*),1));});});_tmp31D;});_tmp31C;}));
# 1362
return({struct Cyc_PP_Doc*_tmp321[3];_tmp321[2]=Cyc_PP_text(({const char*_tmp323=")";_tag_dyneither(_tmp323,sizeof(char),2);}));_tmp321[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_tmp321[0]=Cyc_PP_text(({const char*_tmp322="numelts(";_tag_dyneither(_tmp322,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp321,sizeof(struct Cyc_PP_Doc*),3));});}else{
# 1364
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp324=_cycalloc(sizeof(*_tmp324));_tmp324[0]=({struct Cyc_Core_Failure_exn_struct _tmp325;_tmp325.tag=Cyc_Core_Failure;_tmp325.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp328;_tmp328.tag=0;_tmp328.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));({void*_tmp326[1]={& _tmp328};Cyc_aprintf(({const char*_tmp327="Absynpp::primapp2doc: %s with no args";_tag_dyneither(_tmp327,sizeof(char),38);}),_tag_dyneither(_tmp326,sizeof(void*),1));});});_tmp325;});_tmp324;}));else{
# 1368
if(ds->tl == 0)
return({struct Cyc_PP_Doc*_tmp329[3];_tmp329[2]=(struct Cyc_PP_Doc*)ds->hd;_tmp329[1]=Cyc_PP_text(({const char*_tmp32A=" ";_tag_dyneither(_tmp32A,sizeof(char),2);}));_tmp329[0]=ps;Cyc_PP_cat(_tag_dyneither(_tmp329,sizeof(struct Cyc_PP_Doc*),3));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp32B=_cycalloc(sizeof(*_tmp32B));_tmp32B[0]=({struct Cyc_Core_Failure_exn_struct _tmp32C;_tmp32C.tag=Cyc_Core_Failure;_tmp32C.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp32F;_tmp32F.tag=0;_tmp32F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));({void*_tmp32D[1]={& _tmp32F};Cyc_aprintf(({const char*_tmp32E="Absynpp::primapp2doc: %s with more than 2 args";_tag_dyneither(_tmp32E,sizeof(char),47);}),_tag_dyneither(_tmp32D,sizeof(void*),1));});});_tmp32C;});_tmp32B;}));else{
# 1374
return({struct Cyc_PP_Doc*_tmp330[5];_tmp330[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd;_tmp330[3]=Cyc_PP_text(({const char*_tmp332=" ";_tag_dyneither(_tmp332,sizeof(char),2);}));_tmp330[2]=ps;_tmp330[1]=Cyc_PP_text(({const char*_tmp331=" ";_tag_dyneither(_tmp331,sizeof(char),2);}));_tmp330[0]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_dyneither(_tmp330,sizeof(struct Cyc_PP_Doc*),5));});}}}}}
# 1378
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
# 1403
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1407
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp349=s->r;void*_tmp34A=_tmp349;if(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp34A)->tag == 12){_LL2A4: _LL2A5:
 return 1;}else{_LL2A6: _LL2A7:
 return 0;}_LL2A3:;}
# 1414
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp){
struct Cyc_PP_Doc*s;
{void*_tmp34B=st->r;void*_tmp34C=_tmp34B;struct Cyc_Absyn_Exp*_tmp3AA;struct Cyc_Absyn_Stmt*_tmp3A9;struct Cyc_List_List*_tmp3A8;struct Cyc_Absyn_Stmt*_tmp3A7;struct Cyc_Absyn_Exp*_tmp3A6;struct _dyneither_ptr*_tmp3A5;struct Cyc_Absyn_Stmt*_tmp3A4;struct Cyc_Absyn_Decl*_tmp3A3;struct Cyc_Absyn_Stmt*_tmp3A2;struct Cyc_List_List*_tmp3A1;struct Cyc_Absyn_Exp*_tmp3A0;struct Cyc_List_List*_tmp39F;struct Cyc_Absyn_Exp*_tmp39E;struct Cyc_Absyn_Exp*_tmp39D;struct Cyc_Absyn_Exp*_tmp39C;struct Cyc_Absyn_Stmt*_tmp39B;struct _dyneither_ptr*_tmp39A;struct Cyc_Absyn_Exp*_tmp399;struct Cyc_Absyn_Stmt*_tmp398;struct Cyc_Absyn_Exp*_tmp397;struct Cyc_Absyn_Stmt*_tmp396;struct Cyc_Absyn_Stmt*_tmp395;struct Cyc_Absyn_Exp*_tmp394;struct Cyc_Absyn_Stmt*_tmp393;struct Cyc_Absyn_Stmt*_tmp392;struct Cyc_Absyn_Exp*_tmp391;switch(*((int*)_tmp34C)){case 0: _LL2A9: _LL2AA:
 s=Cyc_PP_text(({const char*_tmp34D=";";_tag_dyneither(_tmp34D,sizeof(char),2);}));goto _LL2A8;case 1: _LL2AB: _tmp391=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_LL2AC:
 s=({struct Cyc_PP_Doc*_tmp34E[2];_tmp34E[1]=Cyc_PP_text(({const char*_tmp34F=";";_tag_dyneither(_tmp34F,sizeof(char),2);}));_tmp34E[0]=Cyc_Absynpp_exp2doc(_tmp391);Cyc_PP_cat(_tag_dyneither(_tmp34E,sizeof(struct Cyc_PP_Doc*),2));});goto _LL2A8;case 2: _LL2AD: _tmp393=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp392=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL2AE:
# 1420
 if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp393))
s=({struct Cyc_PP_Doc*_tmp350[7];_tmp350[6]=
# 1428
Cyc_Absynpp_is_declaration(_tmp392)?
stmtexp?({struct Cyc_PP_Doc*_tmp351[7];_tmp351[6]=
# 1433
Cyc_PP_line_doc();_tmp351[5]=
# 1432
Cyc_PP_text(({const char*_tmp353=");";_tag_dyneither(_tmp353,sizeof(char),3);}));_tmp351[4]=Cyc_Absynpp_rb();_tmp351[3]=
# 1431
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp392,stmtexp));_tmp351[2]=
# 1430
Cyc_PP_blank_doc();_tmp351[1]=Cyc_Absynpp_lb();_tmp351[0]=Cyc_PP_text(({const char*_tmp352="(";_tag_dyneither(_tmp352,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp351,sizeof(struct Cyc_PP_Doc*),7));}):({struct Cyc_PP_Doc*_tmp354[5];_tmp354[4]=
# 1437
Cyc_PP_line_doc();_tmp354[3]=
# 1436
Cyc_Absynpp_rb();_tmp354[2]=
# 1435
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp392,stmtexp));_tmp354[1]=
# 1434
Cyc_PP_blank_doc();_tmp354[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dyneither(_tmp354,sizeof(struct Cyc_PP_Doc*),5));}):
# 1438
 Cyc_Absynpp_stmt2doc(_tmp392,stmtexp);_tmp350[5]=
# 1427
Cyc_PP_line_doc();_tmp350[4]=
# 1426
Cyc_Absynpp_rb();_tmp350[3]=
# 1425
Cyc_PP_line_doc();_tmp350[2]=
# 1424
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp393,0));_tmp350[1]=
# 1423
Cyc_PP_blank_doc();_tmp350[0]=
# 1422
Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dyneither(_tmp350,sizeof(struct Cyc_PP_Doc*),7));});else{
# 1439
if(Cyc_Absynpp_is_declaration(_tmp392))
s=({struct Cyc_PP_Doc*_tmp355[4];_tmp355[3]=
# 1450
Cyc_PP_line_doc();_tmp355[2]=
# 1442
stmtexp?({struct Cyc_PP_Doc*_tmp356[6];_tmp356[5]=
# 1445
Cyc_PP_text(({const char*_tmp358=");";_tag_dyneither(_tmp358,sizeof(char),3);}));_tmp356[4]=Cyc_Absynpp_rb();_tmp356[3]=
# 1444
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp392,stmtexp));_tmp356[2]=
# 1443
Cyc_PP_blank_doc();_tmp356[1]=Cyc_Absynpp_lb();_tmp356[0]=Cyc_PP_text(({const char*_tmp357="(";_tag_dyneither(_tmp357,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp356,sizeof(struct Cyc_PP_Doc*),6));}):({struct Cyc_PP_Doc*_tmp359[4];_tmp359[3]=
# 1449
Cyc_Absynpp_rb();_tmp359[2]=
# 1448
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp392,stmtexp));_tmp359[1]=
# 1447
Cyc_PP_blank_doc();_tmp359[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dyneither(_tmp359,sizeof(struct Cyc_PP_Doc*),4));});_tmp355[1]=
# 1441
Cyc_PP_line_doc();_tmp355[0]=
# 1440
Cyc_Absynpp_stmt2doc(_tmp393,0);Cyc_PP_cat(_tag_dyneither(_tmp355,sizeof(struct Cyc_PP_Doc*),4));});else{
# 1452
s=({struct Cyc_PP_Doc*_tmp35A[3];_tmp35A[2]=Cyc_Absynpp_stmt2doc(_tmp392,stmtexp);_tmp35A[1]=Cyc_PP_line_doc();_tmp35A[0]=Cyc_Absynpp_stmt2doc(_tmp393,0);Cyc_PP_cat(_tag_dyneither(_tmp35A,sizeof(struct Cyc_PP_Doc*),3));});}}}else{
# 1455
s=({struct Cyc_PP_Doc*_tmp35B[3];_tmp35B[2]=Cyc_Absynpp_stmt2doc(_tmp392,stmtexp);_tmp35B[1]=Cyc_PP_line_doc();_tmp35B[0]=Cyc_Absynpp_stmt2doc(_tmp393,0);Cyc_PP_cat(_tag_dyneither(_tmp35B,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LL2A8;case 3: _LL2AF: _tmp394=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_LL2B0:
# 1458
 if(_tmp394 == 0)
s=Cyc_PP_text(({const char*_tmp35C="return;";_tag_dyneither(_tmp35C,sizeof(char),8);}));else{
# 1461
s=({struct Cyc_PP_Doc*_tmp35D[3];_tmp35D[2]=
# 1463
Cyc_PP_text(({const char*_tmp35F=";";_tag_dyneither(_tmp35F,sizeof(char),2);}));_tmp35D[1]=
# 1462
_tmp394 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(_tmp394);_tmp35D[0]=
# 1461
Cyc_PP_text(({const char*_tmp35E="return ";_tag_dyneither(_tmp35E,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp35D,sizeof(struct Cyc_PP_Doc*),3));});}
# 1464
goto _LL2A8;case 4: _LL2B1: _tmp397=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp396=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_tmp395=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp34C)->f3;_LL2B2: {
# 1466
int print_else;
{void*_tmp360=_tmp395->r;void*_tmp361=_tmp360;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp361)->tag == 0){_LL2CE: _LL2CF:
 print_else=0;goto _LL2CD;}else{_LL2D0: _LL2D1:
 print_else=1;goto _LL2CD;}_LL2CD:;}
# 1471
s=({struct Cyc_PP_Doc*_tmp362[8];_tmp362[7]=
# 1478
print_else?({struct Cyc_PP_Doc*_tmp366[6];_tmp366[5]=
# 1484
Cyc_Absynpp_rb();_tmp366[4]=
# 1483
Cyc_PP_line_doc();_tmp366[3]=
# 1482
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp368[2];_tmp368[1]=Cyc_Absynpp_stmt2doc(_tmp395,0);_tmp368[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp368,sizeof(struct Cyc_PP_Doc*),2));}));_tmp366[2]=
# 1481
Cyc_Absynpp_lb();_tmp366[1]=
# 1480
Cyc_PP_text(({const char*_tmp367="else ";_tag_dyneither(_tmp367,sizeof(char),6);}));_tmp366[0]=
# 1479
Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp366,sizeof(struct Cyc_PP_Doc*),6));}):
# 1485
 Cyc_PP_nil_doc();_tmp362[6]=
# 1477
Cyc_Absynpp_rb();_tmp362[5]=
# 1476
Cyc_PP_line_doc();_tmp362[4]=
# 1475
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp365[2];_tmp365[1]=Cyc_Absynpp_stmt2doc(_tmp396,0);_tmp365[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp365,sizeof(struct Cyc_PP_Doc*),2));}));_tmp362[3]=
# 1474
Cyc_Absynpp_lb();_tmp362[2]=
# 1473
Cyc_PP_text(({const char*_tmp364=") ";_tag_dyneither(_tmp364,sizeof(char),3);}));_tmp362[1]=
# 1472
Cyc_Absynpp_exp2doc(_tmp397);_tmp362[0]=
# 1471
Cyc_PP_text(({const char*_tmp363="if (";_tag_dyneither(_tmp363,sizeof(char),5);}));Cyc_PP_cat(_tag_dyneither(_tmp362,sizeof(struct Cyc_PP_Doc*),8));});
# 1486
goto _LL2A8;}case 5: _LL2B3: _tmp399=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1).f1;_tmp398=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL2B4:
# 1488
 s=({struct Cyc_PP_Doc*_tmp369[7];_tmp369[6]=
# 1494
Cyc_Absynpp_rb();_tmp369[5]=
# 1493
Cyc_PP_line_doc();_tmp369[4]=
# 1492
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp36C[2];_tmp36C[1]=Cyc_Absynpp_stmt2doc(_tmp398,0);_tmp36C[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp36C,sizeof(struct Cyc_PP_Doc*),2));}));_tmp369[3]=
# 1491
Cyc_Absynpp_lb();_tmp369[2]=
# 1490
Cyc_PP_text(({const char*_tmp36B=") ";_tag_dyneither(_tmp36B,sizeof(char),3);}));_tmp369[1]=
# 1489
Cyc_Absynpp_exp2doc(_tmp399);_tmp369[0]=
# 1488
Cyc_PP_text(({const char*_tmp36A="while (";_tag_dyneither(_tmp36A,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp369,sizeof(struct Cyc_PP_Doc*),7));});
# 1495
goto _LL2A8;case 6: _LL2B5: _LL2B6:
 s=Cyc_PP_text(({const char*_tmp36D="break;";_tag_dyneither(_tmp36D,sizeof(char),7);}));goto _LL2A8;case 7: _LL2B7: _LL2B8:
 s=Cyc_PP_text(({const char*_tmp36E="continue;";_tag_dyneither(_tmp36E,sizeof(char),10);}));goto _LL2A8;case 8: _LL2B9: _tmp39A=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_LL2BA:
 s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp371;_tmp371.tag=0;_tmp371.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp39A);({void*_tmp36F[1]={& _tmp371};Cyc_aprintf(({const char*_tmp370="goto %s;";_tag_dyneither(_tmp370,sizeof(char),9);}),_tag_dyneither(_tmp36F,sizeof(void*),1));});}));goto _LL2A8;case 9: _LL2BB: _tmp39E=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp39D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2).f1;_tmp39C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34C)->f3).f1;_tmp39B=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34C)->f4;_LL2BC:
# 1500
 s=({struct Cyc_PP_Doc*_tmp372[11];_tmp372[10]=
# 1510
Cyc_Absynpp_rb();_tmp372[9]=
# 1509
Cyc_PP_line_doc();_tmp372[8]=
# 1508
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp377[2];_tmp377[1]=Cyc_Absynpp_stmt2doc(_tmp39B,0);_tmp377[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp377,sizeof(struct Cyc_PP_Doc*),2));}));_tmp372[7]=
# 1507
Cyc_Absynpp_lb();_tmp372[6]=
# 1506
Cyc_PP_text(({const char*_tmp376=") ";_tag_dyneither(_tmp376,sizeof(char),3);}));_tmp372[5]=
# 1505
Cyc_Absynpp_exp2doc(_tmp39C);_tmp372[4]=
# 1504
Cyc_PP_text(({const char*_tmp375="; ";_tag_dyneither(_tmp375,sizeof(char),3);}));_tmp372[3]=
# 1503
Cyc_Absynpp_exp2doc(_tmp39D);_tmp372[2]=
# 1502
Cyc_PP_text(({const char*_tmp374="; ";_tag_dyneither(_tmp374,sizeof(char),3);}));_tmp372[1]=
# 1501
Cyc_Absynpp_exp2doc(_tmp39E);_tmp372[0]=
# 1500
Cyc_PP_text(({const char*_tmp373="for(";_tag_dyneither(_tmp373,sizeof(char),5);}));Cyc_PP_cat(_tag_dyneither(_tmp372,sizeof(struct Cyc_PP_Doc*),11));});
# 1511
goto _LL2A8;case 10: _LL2BD: _tmp3A0=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp39F=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL2BE:
# 1513
 s=({struct Cyc_PP_Doc*_tmp378[8];_tmp378[7]=
# 1520
Cyc_Absynpp_rb();_tmp378[6]=
# 1519
Cyc_PP_line_doc();_tmp378[5]=
# 1518
Cyc_Absynpp_switchclauses2doc(_tmp39F);_tmp378[4]=
# 1517
Cyc_PP_line_doc();_tmp378[3]=
# 1516
Cyc_Absynpp_lb();_tmp378[2]=
# 1515
Cyc_PP_text(({const char*_tmp37A=") ";_tag_dyneither(_tmp37A,sizeof(char),3);}));_tmp378[1]=
# 1514
Cyc_Absynpp_exp2doc(_tmp3A0);_tmp378[0]=
# 1513
Cyc_PP_text(({const char*_tmp379="switch (";_tag_dyneither(_tmp379,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp378,sizeof(struct Cyc_PP_Doc*),8));});
# 1521
goto _LL2A8;case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1 == 0){_LL2BF: _LL2C0:
 s=Cyc_PP_text(({const char*_tmp37B="fallthru;";_tag_dyneither(_tmp37B,sizeof(char),10);}));goto _LL2A8;}else{_LL2C1: _tmp3A1=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_LL2C2:
# 1524
 s=({struct Cyc_PP_Doc*_tmp37C[3];_tmp37C[2]=Cyc_PP_text(({const char*_tmp37E=");";_tag_dyneither(_tmp37E,sizeof(char),3);}));_tmp37C[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp3A1);_tmp37C[0]=Cyc_PP_text(({const char*_tmp37D="fallthru(";_tag_dyneither(_tmp37D,sizeof(char),10);}));Cyc_PP_cat(_tag_dyneither(_tmp37C,sizeof(struct Cyc_PP_Doc*),3));});goto _LL2A8;}case 12: _LL2C3: _tmp3A3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp3A2=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL2C4:
# 1526
 s=({struct Cyc_PP_Doc*_tmp37F[3];_tmp37F[2]=
# 1528
Cyc_Absynpp_stmt2doc(_tmp3A2,stmtexp);_tmp37F[1]=
# 1527
Cyc_PP_line_doc();_tmp37F[0]=
# 1526
Cyc_Absynpp_decl2doc(_tmp3A3);Cyc_PP_cat(_tag_dyneither(_tmp37F,sizeof(struct Cyc_PP_Doc*),3));});
# 1529
goto _LL2A8;case 13: _LL2C5: _tmp3A5=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp3A4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL2C6:
# 1531
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3A4)){
if(stmtexp)
s=({struct Cyc_PP_Doc*_tmp380[8];_tmp380[7]=
# 1539
Cyc_PP_text(({const char*_tmp382=");";_tag_dyneither(_tmp382,sizeof(char),3);}));_tmp380[6]=Cyc_Absynpp_rb();_tmp380[5]=
# 1538
Cyc_PP_line_doc();_tmp380[4]=
# 1537
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A4,1));_tmp380[3]=
# 1536
Cyc_PP_line_doc();_tmp380[2]=
# 1535
Cyc_Absynpp_lb();_tmp380[1]=
# 1534
Cyc_PP_text(({const char*_tmp381=": (";_tag_dyneither(_tmp381,sizeof(char),4);}));_tmp380[0]=
# 1533
Cyc_PP_textptr(_tmp3A5);Cyc_PP_cat(_tag_dyneither(_tmp380,sizeof(struct Cyc_PP_Doc*),8));});else{
# 1541
s=({struct Cyc_PP_Doc*_tmp383[7];_tmp383[6]=
# 1547
Cyc_Absynpp_rb();_tmp383[5]=
# 1546
Cyc_PP_line_doc();_tmp383[4]=
# 1545
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A4,0));_tmp383[3]=
# 1544
Cyc_PP_line_doc();_tmp383[2]=
# 1543
Cyc_Absynpp_lb();_tmp383[1]=
# 1542
Cyc_PP_text(({const char*_tmp384=": ";_tag_dyneither(_tmp384,sizeof(char),3);}));_tmp383[0]=
# 1541
Cyc_PP_textptr(_tmp3A5);Cyc_PP_cat(_tag_dyneither(_tmp383,sizeof(struct Cyc_PP_Doc*),7));});}}else{
# 1549
s=({struct Cyc_PP_Doc*_tmp385[3];_tmp385[2]=Cyc_Absynpp_stmt2doc(_tmp3A4,stmtexp);_tmp385[1]=Cyc_PP_text(({const char*_tmp386=": ";_tag_dyneither(_tmp386,sizeof(char),3);}));_tmp385[0]=Cyc_PP_textptr(_tmp3A5);Cyc_PP_cat(_tag_dyneither(_tmp385,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LL2A8;case 14: _LL2C7: _tmp3A7=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp3A6=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2).f1;_LL2C8:
# 1552
 s=({struct Cyc_PP_Doc*_tmp387[9];_tmp387[8]=
# 1560
Cyc_PP_text(({const char*_tmp38A=");";_tag_dyneither(_tmp38A,sizeof(char),3);}));_tmp387[7]=
# 1559
Cyc_Absynpp_exp2doc(_tmp3A6);_tmp387[6]=
# 1558
Cyc_PP_text(({const char*_tmp389=" while (";_tag_dyneither(_tmp389,sizeof(char),9);}));_tmp387[5]=
# 1557
Cyc_Absynpp_rb();_tmp387[4]=
# 1556
Cyc_PP_line_doc();_tmp387[3]=
# 1555
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A7,0));_tmp387[2]=
# 1554
Cyc_PP_line_doc();_tmp387[1]=
# 1553
Cyc_Absynpp_lb();_tmp387[0]=
# 1552
Cyc_PP_text(({const char*_tmp388="do ";_tag_dyneither(_tmp388,sizeof(char),4);}));Cyc_PP_cat(_tag_dyneither(_tmp387,sizeof(struct Cyc_PP_Doc*),9));});
# 1561
goto _LL2A8;case 15: _LL2C9: _tmp3A9=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_tmp3A8=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp34C)->f2;_LL2CA:
# 1563
 s=({struct Cyc_PP_Doc*_tmp38B[12];_tmp38B[11]=
# 1574
Cyc_Absynpp_rb();_tmp38B[10]=
# 1573
Cyc_PP_line_doc();_tmp38B[9]=
# 1572
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp3A8));_tmp38B[8]=
# 1571
Cyc_PP_line_doc();_tmp38B[7]=
# 1570
Cyc_Absynpp_lb();_tmp38B[6]=
# 1569
Cyc_PP_text(({const char*_tmp38D=" catch ";_tag_dyneither(_tmp38D,sizeof(char),8);}));_tmp38B[5]=
# 1568
Cyc_Absynpp_rb();_tmp38B[4]=
# 1567
Cyc_PP_line_doc();_tmp38B[3]=
# 1566
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A9,0));_tmp38B[2]=
# 1565
Cyc_PP_line_doc();_tmp38B[1]=
# 1564
Cyc_Absynpp_lb();_tmp38B[0]=
# 1563
Cyc_PP_text(({const char*_tmp38C="try ";_tag_dyneither(_tmp38C,sizeof(char),5);}));Cyc_PP_cat(_tag_dyneither(_tmp38B,sizeof(struct Cyc_PP_Doc*),12));});
# 1575
goto _LL2A8;default: _LL2CB: _tmp3AA=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp34C)->f1;_LL2CC:
# 1577
 s=({struct Cyc_PP_Doc*_tmp38E[3];_tmp38E[2]=Cyc_PP_text(({const char*_tmp390=");";_tag_dyneither(_tmp390,sizeof(char),3);}));_tmp38E[1]=Cyc_Absynpp_exp2doc(_tmp3AA);_tmp38E[0]=Cyc_PP_text(({const char*_tmp38F="reset_region(";_tag_dyneither(_tmp38F,sizeof(char),14);}));Cyc_PP_cat(_tag_dyneither(_tmp38E,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL2A8;}_LL2A8:;}
# 1580
return s;}
# 1583
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp3AB=p->r;void*_tmp3AC=_tmp3AB;struct Cyc_Absyn_Exp*_tmp40C;struct Cyc_Absyn_Datatypefield*_tmp40B;struct Cyc_List_List*_tmp40A;int _tmp409;struct Cyc_Absyn_Datatypefield*_tmp408;struct Cyc_Absyn_Enumfield*_tmp407;struct Cyc_Absyn_Enumfield*_tmp406;struct Cyc_List_List*_tmp405;struct Cyc_List_List*_tmp404;int _tmp403;union Cyc_Absyn_AggrInfoU _tmp402;struct Cyc_List_List*_tmp401;struct Cyc_List_List*_tmp400;int _tmp3FF;struct _tuple0*_tmp3FE;struct Cyc_List_List*_tmp3FD;int _tmp3FC;struct _tuple0*_tmp3FB;struct Cyc_Absyn_Vardecl*_tmp3FA;struct Cyc_Absyn_Pat*_tmp3F9;struct Cyc_Absyn_Vardecl*_tmp3F8;struct Cyc_Absyn_Pat*_tmp3F7;struct Cyc_List_List*_tmp3F6;int _tmp3F5;struct Cyc_Absyn_Tvar*_tmp3F4;struct Cyc_Absyn_Vardecl*_tmp3F3;struct Cyc_Absyn_Tvar*_tmp3F2;struct Cyc_Absyn_Vardecl*_tmp3F1;struct Cyc_Absyn_Vardecl*_tmp3F0;struct Cyc_Absyn_Pat*_tmp3EF;struct Cyc_Absyn_Vardecl*_tmp3EE;struct _dyneither_ptr _tmp3ED;char _tmp3EC;enum Cyc_Absyn_Sign _tmp3EB;int _tmp3EA;switch(*((int*)_tmp3AC)){case 0: _LL2D3: _LL2D4:
 s=Cyc_PP_text(({const char*_tmp3AD="_";_tag_dyneither(_tmp3AD,sizeof(char),2);}));goto _LL2D2;case 9: _LL2D5: _LL2D6:
 s=Cyc_PP_text(({const char*_tmp3AE="NULL";_tag_dyneither(_tmp3AE,sizeof(char),5);}));goto _LL2D2;case 10: _LL2D7: _tmp3EB=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3EA=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2D8:
# 1589
 if(_tmp3EB != Cyc_Absyn_Unsigned)
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3B1;_tmp3B1.tag=1;_tmp3B1.f1=(unsigned long)_tmp3EA;({void*_tmp3AF[1]={& _tmp3B1};Cyc_aprintf(({const char*_tmp3B0="%d";_tag_dyneither(_tmp3B0,sizeof(char),3);}),_tag_dyneither(_tmp3AF,sizeof(void*),1));});}));else{
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3B4;_tmp3B4.tag=1;_tmp3B4.f1=(unsigned int)_tmp3EA;({void*_tmp3B2[1]={& _tmp3B4};Cyc_aprintf(({const char*_tmp3B3="%u";_tag_dyneither(_tmp3B3,sizeof(char),3);}),_tag_dyneither(_tmp3B2,sizeof(void*),1));});}));}
goto _LL2D2;case 11: _LL2D9: _tmp3EC=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2DA:
 s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3B7;_tmp3B7.tag=0;_tmp3B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp3EC));({void*_tmp3B5[1]={& _tmp3B7};Cyc_aprintf(({const char*_tmp3B6="'%s'";_tag_dyneither(_tmp3B6,sizeof(char),5);}),_tag_dyneither(_tmp3B5,sizeof(void*),1));});}));goto _LL2D2;case 12: _LL2DB: _tmp3ED=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2DC:
 s=Cyc_PP_text(_tmp3ED);goto _LL2D2;case 1: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2)->r)->tag == 0){_LL2DD: _tmp3EE=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2DE:
# 1596
 s=Cyc_Absynpp_qvar2doc(_tmp3EE->name);goto _LL2D2;}else{_LL2DF: _tmp3F0=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3EF=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2E0:
# 1598
 s=({struct Cyc_PP_Doc*_tmp3B8[3];_tmp3B8[2]=Cyc_Absynpp_pat2doc(_tmp3EF);_tmp3B8[1]=Cyc_PP_text(({const char*_tmp3B9=" as ";_tag_dyneither(_tmp3B9,sizeof(char),5);}));_tmp3B8[0]=Cyc_Absynpp_qvar2doc(_tmp3F0->name);Cyc_PP_cat(_tag_dyneither(_tmp3B8,sizeof(struct Cyc_PP_Doc*),3));});goto _LL2D2;}case 2: _LL2E1: _tmp3F2=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3F1=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2E2:
# 1600
 s=({struct Cyc_PP_Doc*_tmp3BA[5];_tmp3BA[4]=
Cyc_Absynpp_vardecl2doc(_tmp3F1);_tmp3BA[3]=
# 1600
Cyc_PP_text(({const char*_tmp3BD="> ";_tag_dyneither(_tmp3BD,sizeof(char),3);}));_tmp3BA[2]=Cyc_Absynpp_tvar2doc(_tmp3F2);_tmp3BA[1]=Cyc_PP_text(({const char*_tmp3BC=" <";_tag_dyneither(_tmp3BC,sizeof(char),3);}));_tmp3BA[0]=Cyc_PP_text(({const char*_tmp3BB="alias";_tag_dyneither(_tmp3BB,sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp3BA,sizeof(struct Cyc_PP_Doc*),5));});
# 1602
goto _LL2D2;case 4: _LL2E3: _tmp3F4=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3F3=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2E4:
# 1604
 s=({struct Cyc_PP_Doc*_tmp3BE[4];_tmp3BE[3]=Cyc_PP_text(({const char*_tmp3C0=">";_tag_dyneither(_tmp3C0,sizeof(char),2);}));_tmp3BE[2]=Cyc_Absynpp_tvar2doc(_tmp3F4);_tmp3BE[1]=Cyc_PP_text(({const char*_tmp3BF="<";_tag_dyneither(_tmp3BF,sizeof(char),2);}));_tmp3BE[0]=Cyc_Absynpp_qvar2doc(_tmp3F3->name);Cyc_PP_cat(_tag_dyneither(_tmp3BE,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL2D2;case 5: _LL2E5: _tmp3F6=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3F5=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2E6:
# 1607
 s=({struct Cyc_PP_Doc*_tmp3C1[4];_tmp3C1[3]=
_tmp3F5?Cyc_PP_text(({const char*_tmp3C4=", ...)";_tag_dyneither(_tmp3C4,sizeof(char),7);})): Cyc_PP_text(({const char*_tmp3C5=")";_tag_dyneither(_tmp3C5,sizeof(char),2);}));_tmp3C1[2]=
# 1607
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,({const char*_tmp3C3=",";_tag_dyneither(_tmp3C3,sizeof(char),2);}),_tmp3F6);_tmp3C1[1]=Cyc_PP_text(({const char*_tmp3C2="(";_tag_dyneither(_tmp3C2,sizeof(char),2);}));_tmp3C1[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dyneither(_tmp3C1,sizeof(struct Cyc_PP_Doc*),4));});
# 1609
goto _LL2D2;case 6: _LL2E7: _tmp3F7=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2E8:
# 1611
 s=({struct Cyc_PP_Doc*_tmp3C6[2];_tmp3C6[1]=Cyc_Absynpp_pat2doc(_tmp3F7);_tmp3C6[0]=Cyc_PP_text(({const char*_tmp3C7="&";_tag_dyneither(_tmp3C7,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp3C6,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL2D2;case 3: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2)->r)->tag == 0){_LL2E9: _tmp3F8=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2EA:
# 1614
 s=({struct Cyc_PP_Doc*_tmp3C8[2];_tmp3C8[1]=Cyc_Absynpp_qvar2doc(_tmp3F8->name);_tmp3C8[0]=Cyc_PP_text(({const char*_tmp3C9="*";_tag_dyneither(_tmp3C9,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp3C8,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL2D2;}else{_LL2EB: _tmp3FA=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3F9=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2EC:
# 1617
 s=({struct Cyc_PP_Doc*_tmp3CA[4];_tmp3CA[3]=Cyc_Absynpp_pat2doc(_tmp3F9);_tmp3CA[2]=Cyc_PP_text(({const char*_tmp3CC=" as ";_tag_dyneither(_tmp3CC,sizeof(char),5);}));_tmp3CA[1]=Cyc_Absynpp_qvar2doc(_tmp3FA->name);_tmp3CA[0]=Cyc_PP_text(({const char*_tmp3CB="*";_tag_dyneither(_tmp3CB,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp3CA,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL2D2;}case 15: _LL2ED: _tmp3FB=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2EE:
# 1620
 s=Cyc_Absynpp_qvar2doc(_tmp3FB);
goto _LL2D2;case 16: _LL2EF: _tmp3FE=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_tmp3FD=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_tmp3FC=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AC)->f3;_LL2F0: {
# 1623
struct _dyneither_ptr term=_tmp3FC?({const char*_tmp3D0=", ...)";_tag_dyneither(_tmp3D0,sizeof(char),7);}):({const char*_tmp3D1=")";_tag_dyneither(_tmp3D1,sizeof(char),2);});
s=({struct Cyc_PP_Doc*_tmp3CD[2];_tmp3CD[1]=Cyc_PP_group(({const char*_tmp3CE="(";_tag_dyneither(_tmp3CE,sizeof(char),2);}),term,({const char*_tmp3CF=",";_tag_dyneither(_tmp3CF,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp3FD));_tmp3CD[0]=Cyc_Absynpp_qvar2doc(_tmp3FE);Cyc_PP_cat(_tag_dyneither(_tmp3CD,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL2D2;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1 != 0){_LL2F1: _tmp402=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1)->aggr_info;_tmp401=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_tmp400=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f3;_tmp3FF=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f4;_LL2F2: {
# 1627
struct _dyneither_ptr term=_tmp3FF?({const char*_tmp3DB=", ...}";_tag_dyneither(_tmp3DB,sizeof(char),7);}):({const char*_tmp3DC="}";_tag_dyneither(_tmp3DC,sizeof(char),2);});
struct _tuple10 _tmp3D2=Cyc_Absyn_aggr_kinded_name(_tmp402);struct _tuple10 _tmp3D3=_tmp3D2;struct _tuple0*_tmp3DA;_LL300: _tmp3DA=_tmp3D3.f2;_LL301:;
s=({struct Cyc_PP_Doc*_tmp3D4[4];_tmp3D4[3]=
# 1631
Cyc_PP_group(({const char*_tmp3D8="";_tag_dyneither(_tmp3D8,sizeof(char),1);}),term,({const char*_tmp3D9=",";_tag_dyneither(_tmp3D9,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp400));_tmp3D4[2]=
# 1630
Cyc_PP_egroup(({const char*_tmp3D5="[";_tag_dyneither(_tmp3D5,sizeof(char),2);}),({const char*_tmp3D6="]";_tag_dyneither(_tmp3D6,sizeof(char),2);}),({const char*_tmp3D7=",";_tag_dyneither(_tmp3D7,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp401));_tmp3D4[1]=
# 1629
Cyc_Absynpp_lb();_tmp3D4[0]=Cyc_Absynpp_qvar2doc(_tmp3DA);Cyc_PP_cat(_tag_dyneither(_tmp3D4,sizeof(struct Cyc_PP_Doc*),4));});
# 1632
goto _LL2D2;}}else{_LL2F3: _tmp405=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_tmp404=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f3;_tmp403=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AC)->f4;_LL2F4: {
# 1634
struct _dyneither_ptr term=_tmp403?({const char*_tmp3E3=", ...}";_tag_dyneither(_tmp3E3,sizeof(char),7);}):({const char*_tmp3E4="}";_tag_dyneither(_tmp3E4,sizeof(char),2);});
s=({struct Cyc_PP_Doc*_tmp3DD[3];_tmp3DD[2]=
# 1637
Cyc_PP_group(({const char*_tmp3E1="";_tag_dyneither(_tmp3E1,sizeof(char),1);}),term,({const char*_tmp3E2=",";_tag_dyneither(_tmp3E2,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp404));_tmp3DD[1]=
# 1636
Cyc_PP_egroup(({const char*_tmp3DE="[";_tag_dyneither(_tmp3DE,sizeof(char),2);}),({const char*_tmp3DF="]";_tag_dyneither(_tmp3DF,sizeof(char),2);}),({const char*_tmp3E0=",";_tag_dyneither(_tmp3E0,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp405));_tmp3DD[0]=
# 1635
Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dyneither(_tmp3DD,sizeof(struct Cyc_PP_Doc*),3));});
# 1638
goto _LL2D2;}}case 13: _LL2F5: _tmp406=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2F6:
 _tmp407=_tmp406;goto _LL2F8;case 14: _LL2F7: _tmp407=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2F8:
 s=Cyc_Absynpp_qvar2doc(_tmp407->name);goto _LL2D2;case 8: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AC)->f3 == 0){_LL2F9: _tmp408=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_LL2FA:
 s=Cyc_Absynpp_qvar2doc(_tmp408->name);goto _LL2D2;}else{_LL2FB: _tmp40B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AC)->f2;_tmp40A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AC)->f3;_tmp409=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AC)->f4;_LL2FC: {
# 1643
struct _dyneither_ptr term=_tmp409?({const char*_tmp3E8=", ...)";_tag_dyneither(_tmp3E8,sizeof(char),7);}):({const char*_tmp3E9=")";_tag_dyneither(_tmp3E9,sizeof(char),2);});
s=({struct Cyc_PP_Doc*_tmp3E5[2];_tmp3E5[1]=Cyc_PP_egroup(({const char*_tmp3E6="(";_tag_dyneither(_tmp3E6,sizeof(char),2);}),term,({const char*_tmp3E7=",";_tag_dyneither(_tmp3E7,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp40A));_tmp3E5[0]=Cyc_Absynpp_qvar2doc(_tmp40B->name);Cyc_PP_cat(_tag_dyneither(_tmp3E5,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL2D2;}}default: _LL2FD: _tmp40C=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp3AC)->f1;_LL2FE:
# 1647
 s=Cyc_Absynpp_exp2doc(_tmp40C);goto _LL2D2;}_LL2D2:;}
# 1649
return s;}
# 1652
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
return({struct Cyc_PP_Doc*_tmp40D[2];_tmp40D[1]=
Cyc_Absynpp_pat2doc((*dp).f2);_tmp40D[0]=
# 1653
Cyc_PP_egroup(({const char*_tmp40E="";_tag_dyneither(_tmp40E,sizeof(char),1);}),({const char*_tmp40F="=";_tag_dyneither(_tmp40F,sizeof(char),2);}),({const char*_tmp410="=";_tag_dyneither(_tmp410,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));Cyc_PP_cat(_tag_dyneither(_tmp40D,sizeof(struct Cyc_PP_Doc*),2));});}
# 1657
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
return({struct Cyc_PP_Doc*_tmp411[2];_tmp411[1]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp413[2];_tmp413[1]=Cyc_Absynpp_stmt2doc(c->body,0);_tmp413[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp413,sizeof(struct Cyc_PP_Doc*),2));}));_tmp411[0]=
# 1659
Cyc_PP_text(({const char*_tmp412="default: ";_tag_dyneither(_tmp412,sizeof(char),10);}));Cyc_PP_cat(_tag_dyneither(_tmp411,sizeof(struct Cyc_PP_Doc*),2));});else{
# 1661
if(c->where_clause == 0)
return({struct Cyc_PP_Doc*_tmp414[4];_tmp414[3]=
# 1665
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp417[2];_tmp417[1]=Cyc_Absynpp_stmt2doc(c->body,0);_tmp417[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp417,sizeof(struct Cyc_PP_Doc*),2));}));_tmp414[2]=
# 1664
Cyc_PP_text(({const char*_tmp416=": ";_tag_dyneither(_tmp416,sizeof(char),3);}));_tmp414[1]=
# 1663
Cyc_Absynpp_pat2doc(c->pattern);_tmp414[0]=
# 1662
Cyc_PP_text(({const char*_tmp415="case ";_tag_dyneither(_tmp415,sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp414,sizeof(struct Cyc_PP_Doc*),4));});else{
# 1667
return({struct Cyc_PP_Doc*_tmp418[6];_tmp418[5]=
# 1672
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp41C[2];_tmp41C[1]=Cyc_Absynpp_stmt2doc(c->body,0);_tmp41C[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp41C,sizeof(struct Cyc_PP_Doc*),2));}));_tmp418[4]=
# 1671
Cyc_PP_text(({const char*_tmp41B=": ";_tag_dyneither(_tmp41B,sizeof(char),3);}));_tmp418[3]=
# 1670
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));_tmp418[2]=
# 1669
Cyc_PP_text(({const char*_tmp41A=" && ";_tag_dyneither(_tmp41A,sizeof(char),5);}));_tmp418[1]=
# 1668
Cyc_Absynpp_pat2doc(c->pattern);_tmp418[0]=
# 1667
Cyc_PP_text(({const char*_tmp419="case ";_tag_dyneither(_tmp419,sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp418,sizeof(struct Cyc_PP_Doc*),6));});}}}
# 1675
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,({const char*_tmp41D="";_tag_dyneither(_tmp41D,sizeof(char),1);}),cs);}
# 1679
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1683
return({struct Cyc_PP_Doc*_tmp41E[3];_tmp41E[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));_tmp41E[1]=Cyc_PP_text(({const char*_tmp41F=" = ";_tag_dyneither(_tmp41F,sizeof(char),4);}));_tmp41E[0]=Cyc_Absynpp_qvar2doc(f->name);Cyc_PP_cat(_tag_dyneither(_tmp41E,sizeof(struct Cyc_PP_Doc*),3));});}}
# 1686
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,({const char*_tmp420=",";_tag_dyneither(_tmp420,sizeof(char),2);}),fs);}
# 1690
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1694
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,({const char*_tmp421=",";_tag_dyneither(_tmp421,sizeof(char),2);}),vds);}
# 1698
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_Absyn_Vardecl*_tmp422=vd;enum Cyc_Absyn_Scope _tmp433;struct _tuple0*_tmp432;struct Cyc_Absyn_Tqual _tmp431;void*_tmp430;struct Cyc_Absyn_Exp*_tmp42F;struct Cyc_List_List*_tmp42E;_LL303: _tmp433=_tmp422->sc;_tmp432=_tmp422->name;_tmp431=_tmp422->tq;_tmp430=_tmp422->type;_tmp42F=_tmp422->initializer;_tmp42E=_tmp422->attributes;_LL304:;{
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp432);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp42E);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp423=Cyc_Cyclone_c_compiler;if(_tmp423 == Cyc_Cyclone_Gcc_c){_LL306: _LL307:
 beforenamedoc=attsdoc;goto _LL305;}else{_LL308: _LL309:
# 1707
{void*_tmp424=Cyc_Tcutil_compress(_tmp430);void*_tmp425=_tmp424;struct Cyc_List_List*_tmp426;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp425)->tag == 9){_LL30B: _tmp426=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp425)->f1).attributes;_LL30C:
# 1709
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp426);
goto _LL30A;}else{_LL30D: _LL30E:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL30A;}_LL30A:;}
# 1713
goto _LL305;}_LL305:;}{
# 1716
struct Cyc_PP_Doc*tmp_doc;
{enum Cyc_Cyclone_C_Compilers _tmp427=Cyc_Cyclone_c_compiler;if(_tmp427 == Cyc_Cyclone_Gcc_c){_LL310: _LL311:
 tmp_doc=Cyc_PP_nil_doc();goto _LL30F;}else{_LL312: _LL313:
 tmp_doc=attsdoc;goto _LL30F;}_LL30F:;}
# 1721
s=({struct Cyc_PP_Doc*_tmp428[5];_tmp428[4]=
# 1728
Cyc_PP_text(({const char*_tmp42D=";";_tag_dyneither(_tmp42D,sizeof(char),2);}));_tmp428[3]=
# 1725
_tmp42F == 0?
Cyc_PP_nil_doc():({struct Cyc_PP_Doc*_tmp42B[2];_tmp42B[1]=
Cyc_Absynpp_exp2doc(_tmp42F);_tmp42B[0]=Cyc_PP_text(({const char*_tmp42C=" = ";_tag_dyneither(_tmp42C,sizeof(char),4);}));Cyc_PP_cat(_tag_dyneither(_tmp42B,sizeof(struct Cyc_PP_Doc*),2));});_tmp428[2]=
# 1724
Cyc_Absynpp_tqtd2doc(_tmp431,_tmp430,({struct Cyc_Core_Opt*_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429->v=({struct Cyc_PP_Doc*_tmp42A[2];_tmp42A[1]=sn;_tmp42A[0]=beforenamedoc;Cyc_PP_cat(_tag_dyneither(_tmp42A,sizeof(struct Cyc_PP_Doc*),2));});_tmp429;}));_tmp428[1]=
# 1723
Cyc_Absynpp_scope2doc(_tmp433);_tmp428[0]=tmp_doc;Cyc_PP_cat(_tag_dyneither(_tmp428,sizeof(struct Cyc_PP_Doc*),5));});
# 1729
return s;};};}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1732
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple17 _tmp434=*x;struct _tuple17 _tmp435=_tmp434;struct _tuple0*_tmp436;_LL315: _tmp436=_tmp435.f2;_LL316:;
return Cyc_Absynpp_qvar2doc(_tmp436);}
# 1737
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0)
return({struct Cyc_PP_Doc*_tmp437[4];_tmp437[3]=
# 1742
Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp437[2]=
# 1741
Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp437[1]=
# 1740
Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp437[0]=
# 1739
Cyc_Absynpp_scope2doc(ad->sc);Cyc_PP_cat(_tag_dyneither(_tmp437,sizeof(struct Cyc_PP_Doc*),4));});else{
# 1744
return({struct Cyc_PP_Doc*_tmp438[12];_tmp438[11]=
# 1755
Cyc_Absynpp_atts2doc(ad->attributes);_tmp438[10]=
# 1754
Cyc_Absynpp_rb();_tmp438[9]=
# 1753
Cyc_PP_line_doc();_tmp438[8]=
# 1752
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp43B[2];_tmp43B[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_tmp43B[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp43B,sizeof(struct Cyc_PP_Doc*),2));}));_tmp438[7]=
# 1750
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():({struct Cyc_PP_Doc*_tmp439[2];_tmp439[1]=
Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);_tmp439[0]=Cyc_PP_text(({const char*_tmp43A=":";_tag_dyneither(_tmp43A,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp439,sizeof(struct Cyc_PP_Doc*),2));});_tmp438[6]=
# 1749
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);_tmp438[5]=
# 1748
Cyc_Absynpp_lb();_tmp438[4]=Cyc_PP_blank_doc();_tmp438[3]=
# 1747
Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp438[2]=
# 1746
Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp438[1]=
# 1745
Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp438[0]=
# 1744
Cyc_Absynpp_scope2doc(ad->sc);Cyc_PP_cat(_tag_dyneither(_tmp438,sizeof(struct Cyc_PP_Doc*),12));});}}
# 1758
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp43C=dd;enum Cyc_Absyn_Scope _tmp448;struct _tuple0*_tmp447;struct Cyc_List_List*_tmp446;struct Cyc_Core_Opt*_tmp445;int _tmp444;_LL318: _tmp448=_tmp43C->sc;_tmp447=_tmp43C->name;_tmp446=_tmp43C->tvs;_tmp445=_tmp43C->fields;_tmp444=_tmp43C->is_extensible;_LL319:;
if(_tmp445 == 0)
return({struct Cyc_PP_Doc*_tmp43D[5];_tmp43D[4]=
# 1765
Cyc_Absynpp_ktvars2doc(_tmp446);_tmp43D[3]=
# 1764
_tmp444?Cyc_Absynpp_qvar2bolddoc(_tmp447): Cyc_Absynpp_typedef_name2bolddoc(_tmp447);_tmp43D[2]=
# 1763
Cyc_PP_text(({const char*_tmp43F="datatype ";_tag_dyneither(_tmp43F,sizeof(char),10);}));_tmp43D[1]=
# 1762
_tmp444?Cyc_PP_text(({const char*_tmp43E="@extensible ";_tag_dyneither(_tmp43E,sizeof(char),13);})): Cyc_PP_blank_doc();_tmp43D[0]=
# 1761
Cyc_Absynpp_scope2doc(_tmp448);Cyc_PP_cat(_tag_dyneither(_tmp43D,sizeof(struct Cyc_PP_Doc*),5));});else{
# 1767
return({struct Cyc_PP_Doc*_tmp440[10];_tmp440[9]=
# 1775
Cyc_Absynpp_rb();_tmp440[8]=
# 1774
Cyc_PP_line_doc();_tmp440[7]=
# 1773
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp443[2];_tmp443[1]=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp445->v);_tmp443[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp443,sizeof(struct Cyc_PP_Doc*),2));}));_tmp440[6]=
# 1772
Cyc_Absynpp_lb();_tmp440[5]=Cyc_PP_blank_doc();_tmp440[4]=
# 1771
Cyc_Absynpp_ktvars2doc(_tmp446);_tmp440[3]=
# 1770
_tmp444?Cyc_Absynpp_qvar2bolddoc(_tmp447): Cyc_Absynpp_typedef_name2bolddoc(_tmp447);_tmp440[2]=
# 1769
Cyc_PP_text(({const char*_tmp442="datatype ";_tag_dyneither(_tmp442,sizeof(char),10);}));_tmp440[1]=
# 1768
_tmp444?Cyc_PP_text(({const char*_tmp441="@extensible ";_tag_dyneither(_tmp441,sizeof(char),13);})): Cyc_PP_blank_doc();_tmp440[0]=
# 1767
Cyc_Absynpp_scope2doc(_tmp448);Cyc_PP_cat(_tag_dyneither(_tmp440,sizeof(struct Cyc_PP_Doc*),10));});}}
# 1778
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp449=ed;enum Cyc_Absyn_Scope _tmp451;struct _tuple0*_tmp450;struct Cyc_Core_Opt*_tmp44F;_LL31B: _tmp451=_tmp449->sc;_tmp450=_tmp449->name;_tmp44F=_tmp449->fields;_LL31C:;
if(_tmp44F == 0)
return({struct Cyc_PP_Doc*_tmp44A[3];_tmp44A[2]=
# 1783
Cyc_Absynpp_typedef_name2bolddoc(_tmp450);_tmp44A[1]=
# 1782
Cyc_PP_text(({const char*_tmp44B="enum ";_tag_dyneither(_tmp44B,sizeof(char),6);}));_tmp44A[0]=
# 1781
Cyc_Absynpp_scope2doc(_tmp451);Cyc_PP_cat(_tag_dyneither(_tmp44A,sizeof(struct Cyc_PP_Doc*),3));});else{
# 1786
return({struct Cyc_PP_Doc*_tmp44C[8];_tmp44C[7]=
# 1792
Cyc_Absynpp_rb();_tmp44C[6]=
# 1791
Cyc_PP_line_doc();_tmp44C[5]=
# 1790
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp44E[2];_tmp44E[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp44F->v);_tmp44E[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp44E,sizeof(struct Cyc_PP_Doc*),2));}));_tmp44C[4]=
# 1789
Cyc_Absynpp_lb();_tmp44C[3]=Cyc_PP_blank_doc();_tmp44C[2]=
# 1788
Cyc_Absynpp_qvar2bolddoc(_tmp450);_tmp44C[1]=
# 1787
Cyc_PP_text(({const char*_tmp44D="enum ";_tag_dyneither(_tmp44D,sizeof(char),6);}));_tmp44C[0]=
# 1786
Cyc_Absynpp_scope2doc(_tmp451);Cyc_PP_cat(_tag_dyneither(_tmp44C,sizeof(struct Cyc_PP_Doc*),8));});}}
# 1795
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp452=d->r;void*_tmp453=_tmp452;struct Cyc_List_List*_tmp4B5;struct Cyc_List_List*_tmp4B4;struct Cyc_List_List*_tmp4B3;struct _tuple0*_tmp4B2;struct Cyc_List_List*_tmp4B1;struct _dyneither_ptr*_tmp4B0;struct Cyc_List_List*_tmp4AF;struct Cyc_Absyn_Typedefdecl*_tmp4AE;struct Cyc_Absyn_Enumdecl*_tmp4AD;struct Cyc_List_List*_tmp4AC;struct Cyc_Absyn_Pat*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Datatypedecl*_tmp4A9;struct Cyc_Absyn_Tvar*_tmp4A8;struct Cyc_Absyn_Vardecl*_tmp4A7;int _tmp4A6;struct Cyc_Absyn_Exp*_tmp4A5;struct Cyc_Absyn_Vardecl*_tmp4A4;struct Cyc_Absyn_Aggrdecl*_tmp4A3;struct Cyc_Absyn_Fndecl*_tmp4A2;switch(*((int*)_tmp453)){case 1: _LL31E: _tmp4A2=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL31F: {
# 1799
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp460;_tmp460.tag=9;_tmp460.f1=({struct Cyc_Absyn_FnInfo _tmp461;_tmp461.tvars=_tmp4A2->tvs;_tmp461.effect=_tmp4A2->effect;_tmp461.ret_tqual=_tmp4A2->ret_tqual;_tmp461.ret_typ=_tmp4A2->ret_type;_tmp461.args=
# 1803
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp4A2->args);_tmp461.c_varargs=_tmp4A2->c_varargs;_tmp461.cyc_varargs=_tmp4A2->cyc_varargs;_tmp461.rgn_po=_tmp4A2->rgn_po;_tmp461.attributes=0;_tmp461.requires_clause=_tmp4A2->requires_clause;_tmp461.requires_relns=_tmp4A2->requires_relns;_tmp461.ensures_clause=_tmp4A2->ensures_clause;_tmp461.ensures_relns=_tmp4A2->ensures_relns;_tmp461;});_tmp460;});_tmp45F;});
# 1809
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4A2->attributes);
struct Cyc_PP_Doc*inlinedoc;
if(_tmp4A2->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp454=Cyc_Cyclone_c_compiler;if(_tmp454 == Cyc_Cyclone_Gcc_c){_LL33D: _LL33E:
 inlinedoc=Cyc_PP_text(({const char*_tmp455="inline ";_tag_dyneither(_tmp455,sizeof(char),8);}));goto _LL33C;}else{_LL33F: _LL340:
 inlinedoc=Cyc_PP_text(({const char*_tmp456="__inline ";_tag_dyneither(_tmp456,sizeof(char),10);}));goto _LL33C;}_LL33C:;}else{
# 1817
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp4A2->sc);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp457=Cyc_Cyclone_c_compiler;if(_tmp457 == Cyc_Cyclone_Gcc_c){_LL342: _LL343:
 beforenamedoc=attsdoc;goto _LL341;}else{_LL344: _LL345:
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4A2->attributes);goto _LL341;}_LL341:;}{
# 1824
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp4A2->name);
struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,({struct Cyc_Core_Opt*_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D->v=({struct Cyc_PP_Doc*_tmp45E[2];_tmp45E[1]=namedoc;_tmp45E[0]=beforenamedoc;Cyc_PP_cat(_tag_dyneither(_tmp45E,sizeof(struct Cyc_PP_Doc*),2));});_tmp45D;}));
# 1827
struct Cyc_PP_Doc*bodydoc=({struct Cyc_PP_Doc*_tmp45B[5];_tmp45B[4]=
# 1830
Cyc_Absynpp_rb();_tmp45B[3]=
# 1829
Cyc_PP_line_doc();_tmp45B[2]=
# 1828
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp45C[2];_tmp45C[1]=Cyc_Absynpp_stmt2doc(_tmp4A2->body,0);_tmp45C[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp45C,sizeof(struct Cyc_PP_Doc*),2));}));_tmp45B[1]=
# 1827
Cyc_Absynpp_lb();_tmp45B[0]=Cyc_PP_blank_doc();Cyc_PP_cat(_tag_dyneither(_tmp45B,sizeof(struct Cyc_PP_Doc*),5));});
# 1831
s=({struct Cyc_PP_Doc*_tmp458[4];_tmp458[3]=bodydoc;_tmp458[2]=tqtddoc;_tmp458[1]=scopedoc;_tmp458[0]=inlinedoc;Cyc_PP_cat(_tag_dyneither(_tmp458,sizeof(struct Cyc_PP_Doc*),4));});
# 1833
{enum Cyc_Cyclone_C_Compilers _tmp459=Cyc_Cyclone_c_compiler;if(_tmp459 == Cyc_Cyclone_Vc_c){_LL347: _LL348:
 s=({struct Cyc_PP_Doc*_tmp45A[2];_tmp45A[1]=s;_tmp45A[0]=attsdoc;Cyc_PP_cat(_tag_dyneither(_tmp45A,sizeof(struct Cyc_PP_Doc*),2));});goto _LL346;}else{_LL349: _LL34A:
 goto _LL346;}_LL346:;}
# 1838
goto _LL31D;};};}case 5: _LL320: _tmp4A3=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL321:
# 1841
 s=({struct Cyc_PP_Doc*_tmp462[2];_tmp462[1]=Cyc_PP_text(({const char*_tmp463=";";_tag_dyneither(_tmp463,sizeof(char),2);}));_tmp462[0]=Cyc_Absynpp_aggrdecl2doc(_tmp4A3);Cyc_PP_cat(_tag_dyneither(_tmp462,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL31D;case 0: _LL322: _tmp4A4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL323:
# 1844
 s=Cyc_Absynpp_vardecl2doc(_tmp4A4);
goto _LL31D;case 4: _LL324: _tmp4A8=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp4A7=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp453)->f2;_tmp4A6=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp453)->f3;_tmp4A5=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp453)->f4;_LL325:
# 1847
 s=({struct Cyc_PP_Doc*_tmp464[8];_tmp464[7]=
# 1855
Cyc_PP_text(({const char*_tmp46C=";";_tag_dyneither(_tmp46C,sizeof(char),2);}));_tmp464[6]=
# 1854
_tmp4A6?Cyc_PP_text(({const char*_tmp46B=" @resetable";_tag_dyneither(_tmp46B,sizeof(char),12);})): Cyc_PP_nil_doc();_tmp464[5]=
# 1852
(unsigned int)_tmp4A5?({struct Cyc_PP_Doc*_tmp468[3];_tmp468[2]=
Cyc_PP_text(({const char*_tmp46A=")";_tag_dyneither(_tmp46A,sizeof(char),2);}));_tmp468[1]=
# 1852
Cyc_Absynpp_exp2doc(_tmp4A5);_tmp468[0]=Cyc_PP_text(({const char*_tmp469=" = open(";_tag_dyneither(_tmp469,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp468,sizeof(struct Cyc_PP_Doc*),3));}):
 Cyc_PP_nil_doc();_tmp464[4]=
# 1851
Cyc_Absynpp_qvar2doc(_tmp4A7->name);_tmp464[3]=
# 1850
Cyc_PP_text(({const char*_tmp467=">";_tag_dyneither(_tmp467,sizeof(char),2);}));_tmp464[2]=
# 1849
Cyc_Absynpp_tvar2doc(_tmp4A8);_tmp464[1]=
# 1848
Cyc_PP_text(({const char*_tmp466="<";_tag_dyneither(_tmp466,sizeof(char),2);}));_tmp464[0]=
# 1847
Cyc_PP_text(({const char*_tmp465="region";_tag_dyneither(_tmp465,sizeof(char),7);}));Cyc_PP_cat(_tag_dyneither(_tmp464,sizeof(struct Cyc_PP_Doc*),8));});
# 1856
goto _LL31D;case 6: _LL326: _tmp4A9=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL327:
# 1858
 s=({struct Cyc_PP_Doc*_tmp46D[2];_tmp46D[1]=Cyc_PP_text(({const char*_tmp46E=";";_tag_dyneither(_tmp46E,sizeof(char),2);}));_tmp46D[0]=Cyc_Absynpp_datatypedecl2doc(_tmp4A9);Cyc_PP_cat(_tag_dyneither(_tmp46D,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL31D;case 2: _LL328: _tmp4AB=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp4AA=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp453)->f3;_LL329:
# 1861
 s=({struct Cyc_PP_Doc*_tmp46F[5];_tmp46F[4]=
# 1865
Cyc_PP_text(({const char*_tmp472=";";_tag_dyneither(_tmp472,sizeof(char),2);}));_tmp46F[3]=
# 1864
Cyc_Absynpp_exp2doc(_tmp4AA);_tmp46F[2]=
# 1863
Cyc_PP_text(({const char*_tmp471=" = ";_tag_dyneither(_tmp471,sizeof(char),4);}));_tmp46F[1]=
# 1862
Cyc_Absynpp_pat2doc(_tmp4AB);_tmp46F[0]=
# 1861
Cyc_PP_text(({const char*_tmp470="let ";_tag_dyneither(_tmp470,sizeof(char),5);}));Cyc_PP_cat(_tag_dyneither(_tmp46F,sizeof(struct Cyc_PP_Doc*),5));});
# 1866
goto _LL31D;case 3: _LL32A: _tmp4AC=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL32B:
# 1868
 s=({struct Cyc_PP_Doc*_tmp473[3];_tmp473[2]=Cyc_PP_text(({const char*_tmp475=";";_tag_dyneither(_tmp475,sizeof(char),2);}));_tmp473[1]=Cyc_Absynpp_ids2doc(_tmp4AC);_tmp473[0]=Cyc_PP_text(({const char*_tmp474="let ";_tag_dyneither(_tmp474,sizeof(char),5);}));Cyc_PP_cat(_tag_dyneither(_tmp473,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL31D;case 7: _LL32C: _tmp4AD=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL32D:
# 1871
 s=({struct Cyc_PP_Doc*_tmp476[2];_tmp476[1]=Cyc_PP_text(({const char*_tmp477=";";_tag_dyneither(_tmp477,sizeof(char),2);}));_tmp476[0]=Cyc_Absynpp_enumdecl2doc(_tmp4AD);Cyc_PP_cat(_tag_dyneither(_tmp476,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL31D;case 8: _LL32E: _tmp4AE=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL32F: {
# 1874
void*t;
if(_tmp4AE->defn != 0)
t=(void*)_check_null(_tmp4AE->defn);else{
# 1878
t=Cyc_Absyn_new_evar(_tmp4AE->kind,0);}
s=({struct Cyc_PP_Doc*_tmp478[4];_tmp478[3]=
# 1886
Cyc_PP_text(({const char*_tmp47C=";";_tag_dyneither(_tmp47C,sizeof(char),2);}));_tmp478[2]=
# 1885
Cyc_Absynpp_atts2doc(_tmp4AE->atts);_tmp478[1]=
# 1880
Cyc_Absynpp_tqtd2doc(_tmp4AE->tq,t,({struct Cyc_Core_Opt*_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A->v=({struct Cyc_PP_Doc*_tmp47B[2];_tmp47B[1]=
# 1883
Cyc_Absynpp_tvars2doc(_tmp4AE->tvs);_tmp47B[0]=
# 1882
Cyc_Absynpp_typedef_name2bolddoc(_tmp4AE->name);Cyc_PP_cat(_tag_dyneither(_tmp47B,sizeof(struct Cyc_PP_Doc*),2));});_tmp47A;}));_tmp478[0]=
# 1879
Cyc_PP_text(({const char*_tmp479="typedef ";_tag_dyneither(_tmp479,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp478,sizeof(struct Cyc_PP_Doc*),4));});
# 1888
goto _LL31D;}case 9: _LL330: _tmp4B0=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp4AF=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp453)->f2;_LL331:
# 1890
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4B0);
s=({struct Cyc_PP_Doc*_tmp47D[8];_tmp47D[7]=
# 1897
Cyc_Absynpp_rb();_tmp47D[6]=
# 1896
Cyc_PP_line_doc();_tmp47D[5]=
# 1895
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({const char*_tmp47F="";_tag_dyneither(_tmp47F,sizeof(char),1);}),_tmp4AF);_tmp47D[4]=
# 1894
Cyc_PP_line_doc();_tmp47D[3]=
# 1893
Cyc_Absynpp_lb();_tmp47D[2]=Cyc_PP_blank_doc();_tmp47D[1]=
# 1892
Cyc_PP_textptr(_tmp4B0);_tmp47D[0]=
# 1891
Cyc_PP_text(({const char*_tmp47E="namespace ";_tag_dyneither(_tmp47E,sizeof(char),11);}));Cyc_PP_cat(_tag_dyneither(_tmp47D,sizeof(struct Cyc_PP_Doc*),8));});
# 1898
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL31D;case 10: _LL332: _tmp4B2=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp4B1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp453)->f2;_LL333:
# 1901
 if(Cyc_Absynpp_print_using_stmts)
s=({struct Cyc_PP_Doc*_tmp480[8];_tmp480[7]=
# 1908
Cyc_Absynpp_rb();_tmp480[6]=
# 1907
Cyc_PP_line_doc();_tmp480[5]=
# 1906
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({const char*_tmp482="";_tag_dyneither(_tmp482,sizeof(char),1);}),_tmp4B1);_tmp480[4]=
# 1905
Cyc_PP_line_doc();_tmp480[3]=
# 1904
Cyc_Absynpp_lb();_tmp480[2]=Cyc_PP_blank_doc();_tmp480[1]=
# 1903
Cyc_Absynpp_qvar2doc(_tmp4B2);_tmp480[0]=
# 1902
Cyc_PP_text(({const char*_tmp481="using ";_tag_dyneither(_tmp481,sizeof(char),7);}));Cyc_PP_cat(_tag_dyneither(_tmp480,sizeof(struct Cyc_PP_Doc*),8));});else{
# 1910
s=({struct Cyc_PP_Doc*_tmp483[11];_tmp483[10]=
# 1920
Cyc_PP_text(({const char*_tmp488=" */";_tag_dyneither(_tmp488,sizeof(char),4);}));_tmp483[9]=
# 1919
Cyc_Absynpp_rb();_tmp483[8]=
# 1918
Cyc_PP_text(({const char*_tmp487="/* ";_tag_dyneither(_tmp487,sizeof(char),4);}));_tmp483[7]=
# 1917
Cyc_PP_line_doc();_tmp483[6]=
# 1916
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({const char*_tmp486="";_tag_dyneither(_tmp486,sizeof(char),1);}),_tmp4B1);_tmp483[5]=
# 1915
Cyc_PP_line_doc();_tmp483[4]=
# 1914
Cyc_PP_text(({const char*_tmp485=" */";_tag_dyneither(_tmp485,sizeof(char),4);}));_tmp483[3]=
# 1913
Cyc_Absynpp_lb();_tmp483[2]=
# 1912
Cyc_PP_blank_doc();_tmp483[1]=
# 1911
Cyc_Absynpp_qvar2doc(_tmp4B2);_tmp483[0]=
# 1910
Cyc_PP_text(({const char*_tmp484="/* using ";_tag_dyneither(_tmp484,sizeof(char),10);}));Cyc_PP_cat(_tag_dyneither(_tmp483,sizeof(struct Cyc_PP_Doc*),11));});}
# 1921
goto _LL31D;case 11: _LL334: _tmp4B3=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL335:
# 1923
 if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_tmp489[6];_tmp489[5]=
# 1929
Cyc_Absynpp_rb();_tmp489[4]=
# 1928
Cyc_PP_line_doc();_tmp489[3]=
# 1927
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({const char*_tmp48B="";_tag_dyneither(_tmp48B,sizeof(char),1);}),_tmp4B3);_tmp489[2]=
# 1926
Cyc_PP_line_doc();_tmp489[1]=
# 1925
Cyc_Absynpp_lb();_tmp489[0]=
# 1924
Cyc_PP_text(({const char*_tmp48A="extern \"C\" ";_tag_dyneither(_tmp48A,sizeof(char),12);}));Cyc_PP_cat(_tag_dyneither(_tmp489,sizeof(struct Cyc_PP_Doc*),6));});else{
# 1931
s=({struct Cyc_PP_Doc*_tmp48C[9];_tmp48C[8]=
# 1939
Cyc_PP_text(({const char*_tmp491=" */";_tag_dyneither(_tmp491,sizeof(char),4);}));_tmp48C[7]=
# 1938
Cyc_Absynpp_rb();_tmp48C[6]=
# 1937
Cyc_PP_text(({const char*_tmp490="/* ";_tag_dyneither(_tmp490,sizeof(char),4);}));_tmp48C[5]=
# 1936
Cyc_PP_line_doc();_tmp48C[4]=
# 1935
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({const char*_tmp48F="";_tag_dyneither(_tmp48F,sizeof(char),1);}),_tmp4B3);_tmp48C[3]=
# 1934
Cyc_PP_line_doc();_tmp48C[2]=
# 1933
Cyc_PP_text(({const char*_tmp48E=" */";_tag_dyneither(_tmp48E,sizeof(char),4);}));_tmp48C[1]=
# 1932
Cyc_Absynpp_lb();_tmp48C[0]=
# 1931
Cyc_PP_text(({const char*_tmp48D="/* extern \"C\" ";_tag_dyneither(_tmp48D,sizeof(char),15);}));Cyc_PP_cat(_tag_dyneither(_tmp48C,sizeof(struct Cyc_PP_Doc*),9));});}
# 1940
goto _LL31D;case 12: _LL336: _tmp4B5=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp4B4=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp453)->f2;_LL337:
# 1942
 if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
if(_tmp4B4 != 0)
exs_doc=({struct Cyc_PP_Doc*_tmp492[7];_tmp492[6]=
# 1947
Cyc_Absynpp_rb();_tmp492[5]=Cyc_PP_line_doc();_tmp492[4]=
# 1946
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,({const char*_tmp494=",";_tag_dyneither(_tmp494,sizeof(char),2);}),_tmp4B4);_tmp492[3]=Cyc_PP_line_doc();_tmp492[2]=
# 1945
Cyc_Absynpp_lb();_tmp492[1]=Cyc_PP_text(({const char*_tmp493=" export ";_tag_dyneither(_tmp493,sizeof(char),9);}));_tmp492[0]=Cyc_Absynpp_rb();Cyc_PP_cat(_tag_dyneither(_tmp492,sizeof(struct Cyc_PP_Doc*),7));});else{
# 1949
exs_doc=Cyc_Absynpp_rb();}
s=({struct Cyc_PP_Doc*_tmp495[6];_tmp495[5]=exs_doc;_tmp495[4]=
# 1954
Cyc_PP_line_doc();_tmp495[3]=
# 1953
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({const char*_tmp497="";_tag_dyneither(_tmp497,sizeof(char),1);}),_tmp4B5);_tmp495[2]=
# 1952
Cyc_PP_line_doc();_tmp495[1]=
# 1951
Cyc_Absynpp_lb();_tmp495[0]=
# 1950
Cyc_PP_text(({const char*_tmp496="extern \"C include\" ";_tag_dyneither(_tmp496,sizeof(char),20);}));Cyc_PP_cat(_tag_dyneither(_tmp495,sizeof(struct Cyc_PP_Doc*),6));});}else{
# 1957
s=({struct Cyc_PP_Doc*_tmp498[9];_tmp498[8]=
# 1965
Cyc_PP_text(({const char*_tmp49D=" */";_tag_dyneither(_tmp49D,sizeof(char),4);}));_tmp498[7]=
# 1964
Cyc_Absynpp_rb();_tmp498[6]=
# 1963
Cyc_PP_text(({const char*_tmp49C="/* ";_tag_dyneither(_tmp49C,sizeof(char),4);}));_tmp498[5]=
# 1962
Cyc_PP_line_doc();_tmp498[4]=
# 1961
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({const char*_tmp49B="";_tag_dyneither(_tmp49B,sizeof(char),1);}),_tmp4B5);_tmp498[3]=
# 1960
Cyc_PP_line_doc();_tmp498[2]=
# 1959
Cyc_PP_text(({const char*_tmp49A=" */";_tag_dyneither(_tmp49A,sizeof(char),4);}));_tmp498[1]=
# 1958
Cyc_Absynpp_lb();_tmp498[0]=
# 1957
Cyc_PP_text(({const char*_tmp499="/* extern \"C include\" ";_tag_dyneither(_tmp499,sizeof(char),23);}));Cyc_PP_cat(_tag_dyneither(_tmp498,sizeof(struct Cyc_PP_Doc*),9));});}
# 1966
goto _LL31D;case 13: _LL338: _LL339:
# 1968
 s=({struct Cyc_PP_Doc*_tmp49E[2];_tmp49E[1]=Cyc_Absynpp_lb();_tmp49E[0]=Cyc_PP_text(({const char*_tmp49F="__cyclone_port_on__;";_tag_dyneither(_tmp49F,sizeof(char),21);}));Cyc_PP_cat(_tag_dyneither(_tmp49E,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL31D;default: _LL33A: _LL33B:
# 1971
 s=({struct Cyc_PP_Doc*_tmp4A0[2];_tmp4A0[1]=Cyc_Absynpp_lb();_tmp4A0[0]=Cyc_PP_text(({const char*_tmp4A1="__cyclone_port_off__;";_tag_dyneither(_tmp4A1,sizeof(char),22);}));Cyc_PP_cat(_tag_dyneither(_tmp4A0,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL31D;}_LL31D:;}
# 1975
return s;}
# 1978
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();{
enum Cyc_Absyn_Scope _tmp4B6=sc;switch(_tmp4B6){case Cyc_Absyn_Static: _LL34C: _LL34D:
 return Cyc_PP_text(({const char*_tmp4B7="static ";_tag_dyneither(_tmp4B7,sizeof(char),8);}));case Cyc_Absyn_Public: _LL34E: _LL34F:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL350: _LL351:
 return Cyc_PP_text(({const char*_tmp4B8="extern ";_tag_dyneither(_tmp4B8,sizeof(char),8);}));case Cyc_Absyn_ExternC: _LL352: _LL353:
 return Cyc_PP_text(({const char*_tmp4B9="extern \"C\" ";_tag_dyneither(_tmp4B9,sizeof(char),12);}));case Cyc_Absyn_Abstract: _LL354: _LL355:
 return Cyc_PP_text(({const char*_tmp4BA="abstract ";_tag_dyneither(_tmp4BA,sizeof(char),10);}));default: _LL356: _LL357:
 return Cyc_PP_text(({const char*_tmp4BB="register ";_tag_dyneither(_tmp4BB,sizeof(char),10);}));}_LL34B:;};}
# 1991
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp4BC=t;struct Cyc_List_List*_tmp4BE;struct Cyc_Absyn_Tvar*_tmp4BD;switch(*((int*)_tmp4BC)){case 2: _LL359: _tmp4BD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4BC)->f1;_LL35A:
 return Cyc_Tcutil_is_temp_tvar(_tmp4BD);case 24: _LL35B: _tmp4BE=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4BC)->f1;_LL35C:
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp4BE);default: _LL35D: _LL35E:
 return 0;}_LL358:;}
# 2003
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp4BF=t;struct Cyc_Absyn_Typedefdecl*_tmp4C1;void*_tmp4C0;switch(*((int*)_tmp4BF)){case 12: _LL360: _LL361:
 return 1;case 14: _LL362: _LL363:
 return 1;case 17: _LL364: _tmp4C1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4BF)->f3;_tmp4C0=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4BF)->f4;if(_tmp4C0 != 0){_LL365:
# 2010
 return Cyc_Absynpp_is_anon_aggrtype(_tmp4C0);}else{goto _LL366;}default: _LL366: _LL367:
 return 0;}_LL35F:;}
# 2017
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 2020
if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmp4C2=({struct _tuple16 _tmp4C7;_tmp4C7.f1=(void*)tms->hd;_tmp4C7.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp4C7;});struct _tuple16 _tmp4C3=_tmp4C2;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp4C3.f1)->tag == 2){if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp4C3.f2)->tag == 3){_LL369: _LL36A:
# 2023
 return({struct Cyc_List_List*_tmp4C4=_region_malloc(r,sizeof(*_tmp4C4));_tmp4C4->hd=(void*)tms->hd;_tmp4C4->tl=({struct Cyc_List_List*_tmp4C5=_region_malloc(r,sizeof(*_tmp4C5));_tmp4C5->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp4C5->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl);_tmp4C5;});_tmp4C4;});}else{goto _LL36B;}}else{_LL36B: _LL36C:
 return({struct Cyc_List_List*_tmp4C6=_region_malloc(r,sizeof(*_tmp4C6));_tmp4C6->hd=atts;_tmp4C6->tl=tms;_tmp4C6;});}_LL368:;}else{
# 2026
return({struct Cyc_List_List*_tmp4C8=_region_malloc(r,sizeof(*_tmp4C8));_tmp4C8->hd=atts;_tmp4C8->tl=tms;_tmp4C8;});}}
# 2031
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2033
void*_tmp4C9=t;struct _tuple0*_tmp516;struct Cyc_List_List*_tmp515;struct Cyc_Absyn_Typedefdecl*_tmp514;void*_tmp513;struct Cyc_Core_Opt*_tmp512;void*_tmp511;int _tmp510;struct Cyc_List_List*_tmp50F;void*_tmp50E;struct Cyc_Absyn_Tqual _tmp50D;void*_tmp50C;struct Cyc_List_List*_tmp50B;int _tmp50A;struct Cyc_Absyn_VarargInfo*_tmp509;struct Cyc_List_List*_tmp508;struct Cyc_List_List*_tmp507;struct Cyc_Absyn_Exp*_tmp506;struct Cyc_Absyn_Exp*_tmp505;void*_tmp504;struct Cyc_Absyn_Tqual _tmp503;struct Cyc_Absyn_PtrAtts _tmp502;void*_tmp501;struct Cyc_Absyn_Tqual _tmp500;struct Cyc_Absyn_Exp*_tmp4FF;union Cyc_Absyn_Constraint*_tmp4FE;unsigned int _tmp4FD;switch(*((int*)_tmp4C9)){case 8: _LL36E: _tmp501=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C9)->f1).elt_type;_tmp500=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C9)->f1).tq;_tmp4FF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C9)->f1).num_elts;_tmp4FE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C9)->f1).zero_term;_tmp4FD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C9)->f1).zt_loc;_LL36F: {
# 2036
struct _tuple12 _tmp4CA=Cyc_Absynpp_to_tms(r,_tmp500,_tmp501);struct _tuple12 _tmp4CB=_tmp4CA;struct Cyc_Absyn_Tqual _tmp4D4;void*_tmp4D3;struct Cyc_List_List*_tmp4D2;_LL37B: _tmp4D4=_tmp4CB.f1;_tmp4D3=_tmp4CB.f2;_tmp4D2=_tmp4CB.f3;_LL37C:;{
void*tm;
if(_tmp4FF == 0)
tm=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4CC=_region_malloc(r,sizeof(*_tmp4CC));_tmp4CC[0]=({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp4CD;_tmp4CD.tag=0;_tmp4CD.f1=_tmp4FE;_tmp4CD.f2=_tmp4FD;_tmp4CD;});_tmp4CC;});else{
# 2041
tm=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4CE=_region_malloc(r,sizeof(*_tmp4CE));_tmp4CE[0]=({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp4CF;_tmp4CF.tag=1;_tmp4CF.f1=_tmp4FF;_tmp4CF.f2=_tmp4FE;_tmp4CF.f3=_tmp4FD;_tmp4CF;});_tmp4CE;});}
return({struct _tuple12 _tmp4D0;_tmp4D0.f1=_tmp4D4;_tmp4D0.f2=_tmp4D3;_tmp4D0.f3=({struct Cyc_List_List*_tmp4D1=_region_malloc(r,sizeof(*_tmp4D1));_tmp4D1->hd=tm;_tmp4D1->tl=_tmp4D2;_tmp4D1;});_tmp4D0;});};}case 5: _LL370: _tmp504=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C9)->f1).elt_typ;_tmp503=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C9)->f1).elt_tq;_tmp502=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C9)->f1).ptr_atts;_LL371: {
# 2045
struct _tuple12 _tmp4D5=Cyc_Absynpp_to_tms(r,_tmp503,_tmp504);struct _tuple12 _tmp4D6=_tmp4D5;struct Cyc_Absyn_Tqual _tmp4DD;void*_tmp4DC;struct Cyc_List_List*_tmp4DB;_LL37E: _tmp4DD=_tmp4D6.f1;_tmp4DC=_tmp4D6.f2;_tmp4DB=_tmp4D6.f3;_LL37F:;
_tmp4DB=({struct Cyc_List_List*_tmp4D7=_region_malloc(r,sizeof(*_tmp4D7));_tmp4D7->hd=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp4D8=_region_malloc(r,sizeof(*_tmp4D8));_tmp4D8[0]=({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmp4D9;_tmp4D9.tag=2;_tmp4D9.f1=_tmp502;_tmp4D9.f2=tq;_tmp4D9;});_tmp4D8;});_tmp4D7->tl=_tmp4DB;_tmp4D7;});
return({struct _tuple12 _tmp4DA;_tmp4DA.f1=_tmp4DD;_tmp4DA.f2=_tmp4DC;_tmp4DA.f3=_tmp4DB;_tmp4DA;});}case 9: _LL372: _tmp50F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).tvars;_tmp50E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).effect;_tmp50D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).ret_tqual;_tmp50C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).ret_typ;_tmp50B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).args;_tmp50A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).c_varargs;_tmp509=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).cyc_varargs;_tmp508=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).rgn_po;_tmp507=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).attributes;_tmp506=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).requires_clause;_tmp505=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C9)->f1).ensures_clause;_LL373:
# 2051
 if(!Cyc_Absynpp_print_all_tvars){
# 2055
if(_tmp50E == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect(_tmp50E)){
_tmp50E=0;
_tmp50F=0;}}else{
# 2060
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2063
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp50F);}{
# 2068
struct _tuple12 _tmp4DE=Cyc_Absynpp_to_tms(r,_tmp50D,_tmp50C);struct _tuple12 _tmp4DF=_tmp4DE;struct Cyc_Absyn_Tqual _tmp4F9;void*_tmp4F8;struct Cyc_List_List*_tmp4F7;_LL381: _tmp4F9=_tmp4DF.f1;_tmp4F8=_tmp4DF.f2;_tmp4F7=_tmp4DF.f3;_LL382:;{
struct Cyc_List_List*tms=_tmp4F7;
# 2079 "absynpp.cyc"
{enum Cyc_Cyclone_C_Compilers _tmp4E0=Cyc_Cyclone_c_compiler;if(_tmp4E0 == Cyc_Cyclone_Gcc_c){_LL384: _LL385:
# 2081
 if(_tmp507 != 0)
# 2084
tms=Cyc_Absynpp_bubble_attributes(r,(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4E1=_region_malloc(r,sizeof(*_tmp4E1));_tmp4E1[0]=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp4E2;_tmp4E2.tag=5;_tmp4E2.f1=0;_tmp4E2.f2=_tmp507;_tmp4E2;});_tmp4E1;}),tms);
# 2086
tms=({struct Cyc_List_List*_tmp4E3=_region_malloc(r,sizeof(*_tmp4E3));_tmp4E3->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E4=_region_malloc(r,sizeof(*_tmp4E4));_tmp4E4[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp4E5;_tmp4E5.tag=3;_tmp4E5.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4E6=_region_malloc(r,sizeof(*_tmp4E6));_tmp4E6[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp4E7;_tmp4E7.tag=1;_tmp4E7.f1=_tmp50B;_tmp4E7.f2=_tmp50A;_tmp4E7.f3=_tmp509;_tmp4E7.f4=_tmp50E;_tmp4E7.f5=_tmp508;_tmp4E7.f6=_tmp506;_tmp4E7.f7=_tmp505;_tmp4E7;});_tmp4E6;});_tmp4E5;});_tmp4E4;});_tmp4E3->tl=tms;_tmp4E3;});
# 2090
goto _LL383;}else{_LL386: _LL387:
# 2092
 tms=({struct Cyc_List_List*_tmp4E8=_region_malloc(r,sizeof(*_tmp4E8));_tmp4E8->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E9=_region_malloc(r,sizeof(*_tmp4E9));_tmp4E9[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp4EA;_tmp4EA.tag=3;_tmp4EA.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4EB=_region_malloc(r,sizeof(*_tmp4EB));_tmp4EB[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp4EC;_tmp4EC.tag=1;_tmp4EC.f1=_tmp50B;_tmp4EC.f2=_tmp50A;_tmp4EC.f3=_tmp509;_tmp4EC.f4=_tmp50E;_tmp4EC.f5=_tmp508;_tmp4EC.f6=_tmp506;_tmp4EC.f7=_tmp505;_tmp4EC;});_tmp4EB;});_tmp4EA;});_tmp4E9;});_tmp4E8->tl=tms;_tmp4E8;});
# 2096
for(0;_tmp507 != 0;_tmp507=_tmp507->tl){
void*_tmp4ED=(void*)_tmp507->hd;void*_tmp4EE=_tmp4ED;switch(*((int*)_tmp4EE)){case 1: _LL389: _LL38A:
 goto _LL38C;case 2: _LL38B: _LL38C:
 goto _LL38E;case 3: _LL38D: _LL38E:
# 2101
 tms=({struct Cyc_List_List*_tmp4EF=_region_malloc(r,sizeof(*_tmp4EF));_tmp4EF->hd=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4F0=_region_malloc(r,sizeof(*_tmp4F0));_tmp4F0[0]=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp4F1;_tmp4F1.tag=5;_tmp4F1.f1=0;_tmp4F1.f2=({struct Cyc_List_List*_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2->hd=(void*)_tmp507->hd;_tmp4F2->tl=0;_tmp4F2;});_tmp4F1;});_tmp4F0;});_tmp4EF->tl=tms;_tmp4EF;});
goto AfterAtts;default: _LL38F: _LL390:
 goto _LL388;}_LL388:;}
# 2105
goto _LL383;}_LL383:;}
# 2108
AfterAtts:
 if(_tmp50F != 0)
tms=({struct Cyc_List_List*_tmp4F3=_region_malloc(r,sizeof(*_tmp4F3));_tmp4F3->hd=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4F4=_region_malloc(r,sizeof(*_tmp4F4));_tmp4F4[0]=({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmp4F5;_tmp4F5.tag=4;_tmp4F5.f1=_tmp50F;_tmp4F5.f2=0;_tmp4F5.f3=1;_tmp4F5;});_tmp4F4;});_tmp4F3->tl=tms;_tmp4F3;});
return({struct _tuple12 _tmp4F6;_tmp4F6.f1=_tmp4F9;_tmp4F6.f2=_tmp4F8;_tmp4F6.f3=tms;_tmp4F6;});};};case 1: _LL374: _tmp512=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4C9)->f1;_tmp511=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4C9)->f2;_tmp510=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4C9)->f3;_LL375:
# 2114
 if(_tmp511 == 0)
return({struct _tuple12 _tmp4FA;_tmp4FA.f1=tq;_tmp4FA.f2=t;_tmp4FA.f3=0;_tmp4FA;});else{
# 2117
return Cyc_Absynpp_to_tms(r,tq,_tmp511);}case 17: _LL376: _tmp516=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f1;_tmp515=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f2;_tmp514=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f3;_tmp513=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f4;_LL377:
# 2122
 if((_tmp513 == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(_tmp513))
return({struct _tuple12 _tmp4FB;_tmp4FB.f1=tq;_tmp4FB.f2=t;_tmp4FB.f3=0;_tmp4FB;});else{
# 2125
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,_tmp513);}default: _LL378: _LL379:
# 2130
 return({struct _tuple12 _tmp4FC;_tmp4FC.f1=tq;_tmp4FC.f2=t;_tmp4FC.f3=0;_tmp4FC;});}_LL36D:;}
# 2134
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2136
void*_tmp517=t;void*_tmp51C;void*_tmp51B;switch(*((int*)_tmp517)){case 1: _LL392: _tmp51B=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp517)->f2;if(_tmp51B != 0){_LL393:
 return Cyc_Absynpp_is_char_ptr(_tmp51B);}else{goto _LL396;}case 5: _LL394: _tmp51C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp517)->f1).elt_typ;_LL395:
# 2139
 L: {
void*_tmp518=_tmp51C;void*_tmp51A;void*_tmp519;switch(*((int*)_tmp518)){case 1: _LL399: _tmp519=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp518)->f2;if(_tmp519 != 0){_LL39A:
 _tmp51C=_tmp519;goto L;}else{goto _LL39F;}case 17: _LL39B: _tmp51A=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp518)->f4;if(_tmp51A != 0){_LL39C:
 _tmp51C=_tmp51A;goto L;}else{goto _LL39F;}case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp518)->f2 == Cyc_Absyn_Char_sz){_LL39D: _LL39E:
 return 1;}else{goto _LL39F;}default: _LL39F: _LL3A0:
 return 0;}_LL398:;}default: _LL396: _LL397:
# 2146
 return 0;}_LL391:;}
# 2150
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp51D=_new_region("temp");struct _RegionHandle*temp=& _tmp51D;_push_region(temp);
{struct _tuple12 _tmp51E=Cyc_Absynpp_to_tms(temp,tq,typ);struct _tuple12 _tmp51F=_tmp51E;struct Cyc_Absyn_Tqual _tmp527;void*_tmp526;struct Cyc_List_List*_tmp525;_LL3A2: _tmp527=_tmp51F.f1;_tmp526=_tmp51F.f2;_tmp525=_tmp51F.f3;_LL3A3:;
_tmp525=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp525);
if(_tmp525 == 0  && dopt == 0){
struct Cyc_PP_Doc*_tmp521=({struct Cyc_PP_Doc*_tmp520[2];_tmp520[1]=Cyc_Absynpp_ntyp2doc(_tmp526);_tmp520[0]=Cyc_Absynpp_tqual2doc(_tmp527);Cyc_PP_cat(_tag_dyneither(_tmp520,sizeof(struct Cyc_PP_Doc*),2));});_npop_handler(0);return _tmp521;}else{
# 2157
struct Cyc_PP_Doc*_tmp524=({struct Cyc_PP_Doc*_tmp522[4];_tmp522[3]=
# 2161
Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp525);_tmp522[2]=
# 2160
Cyc_PP_text(({const char*_tmp523=" ";_tag_dyneither(_tmp523,sizeof(char),2);}));_tmp522[1]=
# 2159
Cyc_Absynpp_ntyp2doc(_tmp526);_tmp522[0]=
# 2158
Cyc_Absynpp_tqual2doc(_tmp527);Cyc_PP_cat(_tag_dyneither(_tmp522,sizeof(struct Cyc_PP_Doc*),4));});_npop_handler(0);return _tmp524;}}
# 2152
;_pop_region(temp);}
# 2165
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp528=f->requires_clause;
if((unsigned int)_tmp528)
requires_doc=({struct Cyc_PP_Doc*_tmp529[2];_tmp529[1]=Cyc_Absynpp_exp2doc(_tmp528);_tmp529[0]=Cyc_PP_text(({const char*_tmp52A="@requires ";_tag_dyneither(_tmp52A,sizeof(char),11);}));Cyc_PP_cat(_tag_dyneither(_tmp529,sizeof(struct Cyc_PP_Doc*),2));});else{
# 2171
requires_doc=Cyc_PP_nil_doc();}{
# 2173
enum Cyc_Cyclone_C_Compilers _tmp52B=Cyc_Cyclone_c_compiler;if(_tmp52B == Cyc_Cyclone_Gcc_c){_LL3A5: _LL3A6:
# 2176
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp52C[5];_tmp52C[4]=
# 2179
Cyc_PP_text(({const char*_tmp52F=";";_tag_dyneither(_tmp52F,sizeof(char),2);}));_tmp52C[3]=Cyc_Absynpp_atts2doc(f->attributes);_tmp52C[2]=
# 2178
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp52C[1]=Cyc_PP_text(({const char*_tmp52E=":";_tag_dyneither(_tmp52E,sizeof(char),2);}));_tmp52C[0]=
# 2177
Cyc_Absynpp_tqtd2doc(f->tq,f->type,({struct Cyc_Core_Opt*_tmp52D=_cycalloc(sizeof(*_tmp52D));_tmp52D->v=Cyc_PP_textptr(f->name);_tmp52D;}));Cyc_PP_cat(_tag_dyneither(_tmp52C,sizeof(struct Cyc_PP_Doc*),5));});else{
# 2181
return({struct Cyc_PP_Doc*_tmp530[4];_tmp530[3]=
Cyc_PP_text(({const char*_tmp532=";";_tag_dyneither(_tmp532,sizeof(char),2);}));_tmp530[2]=requires_doc;_tmp530[1]=Cyc_Absynpp_atts2doc(f->attributes);_tmp530[0]=
# 2181
Cyc_Absynpp_tqtd2doc(f->tq,f->type,({struct Cyc_Core_Opt*_tmp531=_cycalloc(sizeof(*_tmp531));_tmp531->v=Cyc_PP_textptr(f->name);_tmp531;}));Cyc_PP_cat(_tag_dyneither(_tmp530,sizeof(struct Cyc_PP_Doc*),4));});}}else{_LL3A7: _LL3A8:
# 2184
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp533[5];_tmp533[4]=
# 2187
Cyc_PP_text(({const char*_tmp536=";";_tag_dyneither(_tmp536,sizeof(char),2);}));_tmp533[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp533[2]=Cyc_PP_text(({const char*_tmp535=":";_tag_dyneither(_tmp535,sizeof(char),2);}));_tmp533[1]=
# 2186
Cyc_Absynpp_tqtd2doc(f->tq,f->type,({struct Cyc_Core_Opt*_tmp534=_cycalloc(sizeof(*_tmp534));_tmp534->v=Cyc_PP_textptr(f->name);_tmp534;}));_tmp533[0]=
# 2185
Cyc_Absynpp_atts2doc(f->attributes);Cyc_PP_cat(_tag_dyneither(_tmp533,sizeof(struct Cyc_PP_Doc*),5));});else{
# 2189
return({struct Cyc_PP_Doc*_tmp537[4];_tmp537[3]=
# 2191
Cyc_PP_text(({const char*_tmp539=";";_tag_dyneither(_tmp539,sizeof(char),2);}));_tmp537[2]=requires_doc;_tmp537[1]=
# 2190
Cyc_Absynpp_tqtd2doc(f->tq,f->type,({struct Cyc_Core_Opt*_tmp538=_cycalloc(sizeof(*_tmp538));_tmp538->v=Cyc_PP_textptr(f->name);_tmp538;}));_tmp537[0]=
# 2189
Cyc_Absynpp_atts2doc(f->attributes);Cyc_PP_cat(_tag_dyneither(_tmp537,sizeof(struct Cyc_PP_Doc*),4));});}}_LL3A4:;};}
# 2196
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,({const char*_tmp53A="";_tag_dyneither(_tmp53A,sizeof(char),1);}),fields);}
# 2200
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
return({struct Cyc_PP_Doc*_tmp53B[3];_tmp53B[2]=
f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs);_tmp53B[1]=
# 2201
Cyc_Absynpp_typedef_name2doc(f->name);_tmp53B[0]=Cyc_Absynpp_scope2doc(f->sc);Cyc_PP_cat(_tag_dyneither(_tmp53B,sizeof(struct Cyc_PP_Doc*),3));});}
# 2205
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,({const char*_tmp53C=",";_tag_dyneither(_tmp53C,sizeof(char),2);}),fields);}
# 2214
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);
({void*_tmp53D=0;Cyc_fprintf(f,({const char*_tmp53E="\n";_tag_dyneither(_tmp53E,sizeof(char),2);}),_tag_dyneither(_tmp53D,sizeof(void*),0));});}}
# 2221
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
return Cyc_PP_string_of_doc(Cyc_PP_seql(({const char*_tmp53F="";_tag_dyneither(_tmp53F,sizeof(char),1);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),72);}
# 2224
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
# 2228
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
# 2239
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _dyneither_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}
