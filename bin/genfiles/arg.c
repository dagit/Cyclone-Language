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
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 197 "cycboot.h"
int Cyc_sscanf(struct _dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 300 "cycboot.h"
int isspace(int);
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
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 51
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_Arg_Bad[4];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
# 55 "arg.h"
typedef void*Cyc_Arg_spec_t;
# 57
typedef struct Cyc_List_List*Cyc_Arg_speclist_t;
# 66
void Cyc_Arg_usage(struct Cyc_List_List*,struct _dyneither_ptr);
# 69
extern int Cyc_Arg_current;
# 71
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);struct Cyc_Buffer_t;
# 46 "buffer.h"
typedef struct Cyc_Buffer_t*Cyc_Buffer_T;
# 49
struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned int n);
# 57
struct _dyneither_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);
# 72
void Cyc_Buffer_add_substring(struct Cyc_Buffer_t*,struct _dyneither_ptr,int offset,int len);
# 81 "buffer.h"
void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _dyneither_ptr);
# 29 "assert.h"
void*Cyc___assert_fail(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line);char Cyc_Arg_Bad[4]="Bad";char Cyc_Arg_Error[6]="Error";struct Cyc_Arg_Prefix_Arg_Flag_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Exact_Arg_Flag_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Unknown_Arg_error_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Missing_Arg_error_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Message_Arg_error_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Wrong_Arg_error_struct{int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;struct _dyneither_ptr f3;};struct _tuple0{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
# 68 "arg.cyc"
static void*Cyc_Arg_lookup(struct Cyc_List_List*l,struct _dyneither_ptr x){
while(l != 0){
struct _dyneither_ptr _tmp0=(*((struct _tuple0*)l->hd)).f1;
unsigned long _tmp1=Cyc_strlen((struct _dyneither_ptr)_tmp0);
if(_tmp1 > 0  && (*((struct _tuple0*)l->hd)).f2){
if(Cyc_strncmp((struct _dyneither_ptr)x,(struct _dyneither_ptr)(*((struct _tuple0*)l->hd)).f1,_tmp1)== 0)
return(*((struct _tuple0*)l->hd)).f4;}else{
# 76
if(Cyc_strcmp((struct _dyneither_ptr)x,(struct _dyneither_ptr)(*((struct _tuple0*)l->hd)).f1)== 0)
return(*((struct _tuple0*)l->hd)).f4;}
l=l->tl;}{
# 80
struct Cyc_Core_Not_found_exn_struct _tmp65;struct Cyc_Core_Not_found_exn_struct*_tmp64;(int)_throw((void*)((_tmp64=_cycalloc_atomic(sizeof(*_tmp64)),((_tmp64[0]=((_tmp65.tag=Cyc_Core_Not_found,_tmp65)),_tmp64)))));};}
# 89
static struct _dyneither_ptr Cyc_Arg_Justify_break_line(struct Cyc_Buffer_t*b,int howmuch,struct _dyneither_ptr s){
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr)return _tag_dyneither(0,0,0);
if(howmuch < 0)howmuch=0;{
unsigned long _tmp4=Cyc_strlen((struct _dyneither_ptr)s);
{const char*_tmp67;const char*_tmp66;_tmp4 <= _get_dyneither_size(s,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(((_tmp66="len <= numelts(s)",_tag_dyneither(_tmp66,sizeof(char),18))),((_tmp67="arg.cyc",_tag_dyneither(_tmp67,sizeof(char),8))),93);}
if(howmuch > _tmp4){
Cyc_Buffer_add_string(b,s);
return _tag_dyneither(0,0,0);}{
# 100
int i;
for(i=howmuch - 1;i >= 0  && !isspace((int)*((const char*)_check_dyneither_subscript(s,sizeof(char),i)));-- i){
;}
# 105
if(i < 0)
for(i=howmuch?howmuch - 1: 0;(i < _tmp4  && (int)((const char*)s.curr)[i]) && !isspace((int)((const char*)s.curr)[i]);++ i){
;}
# 111
Cyc_Buffer_add_substring(b,s,0,i);{
# 114
struct _dyneither_ptr whatsleft=_tag_dyneither(0,0,0);
# 116
for(0;(i < _tmp4  && (int)((const char*)s.curr)[i]) && isspace((int)((const char*)s.curr)[i]);++ i){
;}
if(i < _tmp4  && (int)((const char*)s.curr)[i])whatsleft=_dyneither_ptr_plus(s,sizeof(char),i);
return whatsleft;};};};}void Cyc_Arg_Justify_justify_b(struct Cyc_Buffer_t*b,int indent,int margin,struct _dyneither_ptr item,struct _dyneither_ptr desc);static void _tmp6D(unsigned int*_tmp6C,unsigned int*_tmp6B,char**_tmp69){for(*_tmp6C=0;*_tmp6C < *_tmp6B;(*_tmp6C)++){(*_tmp69)[*_tmp6C]=(char)'\000';}}static void _tmp76(unsigned int*_tmp75,unsigned int*_tmp74,char**_tmp72){for(*_tmp75=0;*_tmp75 < *_tmp74;(*_tmp75)++){(*_tmp72)[*_tmp75]=(char)'\000';}}
# 127
void Cyc_Arg_Justify_justify_b(struct Cyc_Buffer_t*b,int indent,int margin,struct _dyneither_ptr item,struct _dyneither_ptr desc){
# 129
if((char*)item.curr != (char*)(_tag_dyneither(0,0,0)).curr)Cyc_Buffer_add_string(b,item);
if((char*)desc.curr == (char*)(_tag_dyneither(0,0,0)).curr)return;
if(indent < 0)indent=0;
if(margin < 0)margin=0;{
# 135
unsigned int _tmp6C;unsigned int _tmp6B;struct _dyneither_ptr _tmp6A;char*_tmp69;unsigned int _tmp68;struct _dyneither_ptr indentstr=(_tmp68=(unsigned int)(indent + 2),((_tmp69=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp68 + 1)),((_tmp6A=_tag_dyneither(_tmp69,sizeof(char),_tmp68 + 1),((((_tmp6B=_tmp68,((_tmp6D(& _tmp6C,& _tmp6B,& _tmp69),_tmp69[_tmp6B]=(char)0)))),_tmp6A)))))));
{unsigned int i=0;for(0;i < indent + 1;++ i){
char _tmp70;char _tmp6F;struct _dyneither_ptr _tmp6E;(_tmp6E=_dyneither_ptr_plus(indentstr,sizeof(char),(int)i),((_tmp6F=*((char*)_check_dyneither_subscript(_tmp6E,sizeof(char),0)),((_tmp70=i == 0?'\n':' ',((_get_dyneither_size(_tmp6E,sizeof(char))== 1  && (_tmp6F == '\000'  && _tmp70 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp6E.curr)=_tmp70)))))));}}{
unsigned long _tmpA=Cyc_strlen((struct _dyneither_ptr)item);
struct _dyneither_ptr itemsep;
if(Cyc_strlen((struct _dyneither_ptr)desc)> 0){
if(_tmpA + 1 > indent)
itemsep=indentstr;else{
# 144
unsigned int _tmp75;unsigned int _tmp74;struct _dyneither_ptr _tmp73;char*_tmp72;unsigned int _tmp71;struct _dyneither_ptr temp=(_tmp71=(indent - _tmpA)+ 1,((_tmp72=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp71 + 1)),((_tmp73=_tag_dyneither(_tmp72,sizeof(char),_tmp71 + 1),((((_tmp74=_tmp71,((_tmp76(& _tmp75,& _tmp74,& _tmp72),_tmp72[_tmp74]=(char)0)))),_tmp73)))))));
{unsigned int i=0;for(0;i < indent - _tmpA;++ i){char _tmp79;char _tmp78;struct _dyneither_ptr _tmp77;(_tmp77=_dyneither_ptr_plus(temp,sizeof(char),(int)i),((_tmp78=*((char*)_check_dyneither_subscript(_tmp77,sizeof(char),0)),((_tmp79=' ',((_get_dyneither_size(_tmp77,sizeof(char))== 1  && (_tmp78 == '\000'  && _tmp79 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp77.curr)=_tmp79)))))));}}
itemsep=temp;}}else{
# 149
return;}
# 151
Cyc_Buffer_add_string(b,(struct _dyneither_ptr)itemsep);
# 153
while((char*)desc.curr != (char*)(_tag_dyneither(0,0,0)).curr){
desc=Cyc_Arg_Justify_break_line(b,margin - indent,desc);
if((char*)desc.curr != (char*)(_tag_dyneither(0,0,0)).curr)
Cyc_Buffer_add_string(b,(struct _dyneither_ptr)indentstr);else{
# 158
const char*_tmp7A;Cyc_Buffer_add_string(b,((_tmp7A="\n",_tag_dyneither(_tmp7A,sizeof(char),2))));}}
# 160
return;};};}
# 164
void Cyc_Arg_usage(struct Cyc_List_List*speclist,struct _dyneither_ptr errmsg){
# 166
{const char*_tmp7E;void*_tmp7D[1];struct Cyc_String_pa_PrintArg_struct _tmp7C;(_tmp7C.tag=0,((_tmp7C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)errmsg),((_tmp7D[0]=& _tmp7C,Cyc_fprintf(Cyc_stderr,((_tmp7E="%s\n",_tag_dyneither(_tmp7E,sizeof(char),4))),_tag_dyneither(_tmp7D,sizeof(void*),1)))))));}{
struct Cyc_Buffer_t*_tmp1A=Cyc_Buffer_create(1024);
while(speclist != 0){
Cyc_Arg_Justify_justify_b(_tmp1A,12,72,(struct _dyneither_ptr)
Cyc_strconcat((struct _dyneither_ptr)(*((struct _tuple0*)speclist->hd)).f1,(struct _dyneither_ptr)(*((struct _tuple0*)speclist->hd)).f3),(*((struct _tuple0*)speclist->hd)).f5);
# 172
speclist=speclist->tl;}{
# 174
const char*_tmp82;void*_tmp81[1];struct Cyc_String_pa_PrintArg_struct _tmp80;(_tmp80.tag=0,((_tmp80.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Buffer_contents(_tmp1A)),((_tmp81[0]=& _tmp80,Cyc_fprintf(Cyc_stderr,((_tmp82="%s",_tag_dyneither(_tmp82,sizeof(char),3))),_tag_dyneither(_tmp81,sizeof(void*),1)))))));};};}
# 177
int Cyc_Arg_current=0;
static struct _dyneither_ptr Cyc_Arg_args={(void*)0,(void*)0,(void*)(0 + 0)};
# 180
static void Cyc_Arg_stop(int prog_pos,void*e,struct Cyc_List_List*speclist,struct _dyneither_ptr errmsg){
# 183
const char*_tmp83;struct _dyneither_ptr progname=prog_pos < _get_dyneither_size(Cyc_Arg_args,sizeof(struct _dyneither_ptr))?*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),prog_pos)):((_tmp83="(?)",_tag_dyneither(_tmp83,sizeof(char),4)));
{void*_tmp1E=e;struct _dyneither_ptr _tmp1F;struct _dyneither_ptr _tmp20;struct _dyneither_ptr _tmp21;struct _dyneither_ptr _tmp22;struct _dyneither_ptr _tmp23;struct _dyneither_ptr _tmp24;switch(*((int*)_tmp1E)){case 0: _LL1: _tmp24=((struct Cyc_Arg_Unknown_Arg_error_struct*)_tmp1E)->f1;_LL2:
# 186
{const char*_tmp84;if(Cyc_strcmp((struct _dyneither_ptr)_tmp24,((_tmp84="-help",_tag_dyneither(_tmp84,sizeof(char),6))))!= 0){
const char*_tmp89;void*_tmp88[2];struct Cyc_String_pa_PrintArg_struct _tmp87;struct Cyc_String_pa_PrintArg_struct _tmp86;(_tmp86.tag=0,((_tmp86.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp24),((_tmp87.tag=0,((_tmp87.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)progname),((_tmp88[0]=& _tmp87,((_tmp88[1]=& _tmp86,Cyc_fprintf(Cyc_stderr,((_tmp89="%s: unknown option `%s'.\n",_tag_dyneither(_tmp89,sizeof(char),26))),_tag_dyneither(_tmp88,sizeof(void*),2)))))))))))));}}
goto _LL0;case 1: _LL3: _tmp23=((struct Cyc_Arg_Missing_Arg_error_struct*)_tmp1E)->f1;_LL4:
# 190
{const char*_tmp8E;void*_tmp8D[2];struct Cyc_String_pa_PrintArg_struct _tmp8C;struct Cyc_String_pa_PrintArg_struct _tmp8B;(_tmp8B.tag=0,((_tmp8B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp23),((_tmp8C.tag=0,((_tmp8C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)progname),((_tmp8D[0]=& _tmp8C,((_tmp8D[1]=& _tmp8B,Cyc_fprintf(Cyc_stderr,((_tmp8E="%s: option `%s' needs an argument.\n",_tag_dyneither(_tmp8E,sizeof(char),36))),_tag_dyneither(_tmp8D,sizeof(void*),2)))))))))))));}
goto _LL0;case 3: _LL5: _tmp20=((struct Cyc_Arg_Wrong_Arg_error_struct*)_tmp1E)->f1;_tmp21=((struct Cyc_Arg_Wrong_Arg_error_struct*)_tmp1E)->f2;_tmp22=((struct Cyc_Arg_Wrong_Arg_error_struct*)_tmp1E)->f3;_LL6:
# 193
{const char*_tmp95;void*_tmp94[4];struct Cyc_String_pa_PrintArg_struct _tmp93;struct Cyc_String_pa_PrintArg_struct _tmp92;struct Cyc_String_pa_PrintArg_struct _tmp91;struct Cyc_String_pa_PrintArg_struct _tmp90;(_tmp90.tag=0,((_tmp90.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp22),((_tmp91.tag=0,((_tmp91.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp20),((_tmp92.tag=0,((_tmp92.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp21),((_tmp93.tag=0,((_tmp93.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)progname),((_tmp94[0]=& _tmp93,((_tmp94[1]=& _tmp92,((_tmp94[2]=& _tmp91,((_tmp94[3]=& _tmp90,Cyc_fprintf(Cyc_stderr,((_tmp95="%s: wrong argument `%s'; option `%s' expects %s.\n",_tag_dyneither(_tmp95,sizeof(char),50))),_tag_dyneither(_tmp94,sizeof(void*),4)))))))))))))))))))))))));}
# 195
goto _LL0;default: _LL7: _tmp1F=((struct Cyc_Arg_Message_Arg_error_struct*)_tmp1E)->f1;_LL8:
# 197
{const char*_tmp9A;void*_tmp99[2];struct Cyc_String_pa_PrintArg_struct _tmp98;struct Cyc_String_pa_PrintArg_struct _tmp97;(_tmp97.tag=0,((_tmp97.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1F),((_tmp98.tag=0,((_tmp98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)progname),((_tmp99[0]=& _tmp98,((_tmp99[1]=& _tmp97,Cyc_fprintf(Cyc_stderr,((_tmp9A="%s: %s.\n",_tag_dyneither(_tmp9A,sizeof(char),9))),_tag_dyneither(_tmp99,sizeof(void*),2)))))))))))));}
goto _LL0;}_LL0:;}
# 200
Cyc_Arg_usage(speclist,errmsg);
Cyc_Arg_current=(int)_get_dyneither_size(Cyc_Arg_args,sizeof(struct _dyneither_ptr));}
# 204
void Cyc_Arg_parse(struct Cyc_List_List*speclist,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr orig_args){
# 209
Cyc_Arg_args=orig_args;{
# 211
int initpos=Cyc_Arg_current;
unsigned int l=_get_dyneither_size(Cyc_Arg_args,sizeof(struct _dyneither_ptr));
# 214
if((char*)((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),(int)(l - 1)))->curr == (char*)(_tag_dyneither(0,0,0)).curr)l=l - 1;
++ Cyc_Arg_current;
while(Cyc_Arg_current < l){
struct _dyneither_ptr s=*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current));
if(((char*)s.curr != (char*)(_tag_dyneither(0,0,0)).curr  && _get_dyneither_size(s,sizeof(char))>= 1) && ((const char*)s.curr)[0]== '-'){
void*action;
{struct _handler_cons _tmp39;_push_handler(& _tmp39);{int _tmp3B=0;if(setjmp(_tmp39.handler))_tmp3B=1;if(!_tmp3B){action=Cyc_Arg_lookup(speclist,s);;_pop_handler();}else{void*_tmp3A=(void*)_exn_thrown;void*_tmp3C=_tmp3A;void*_tmp3D;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp3C)->tag == Cyc_Core_Not_found){_LLA: _LLB:
# 224
 if(!anonflagfun(s)){
{struct Cyc_Arg_Unknown_Arg_error_struct _tmp9D;struct Cyc_Arg_Unknown_Arg_error_struct*_tmp9C;Cyc_Arg_stop(initpos,(void*)((_tmp9C=_cycalloc(sizeof(*_tmp9C)),((_tmp9C[0]=((_tmp9D.tag=0,((_tmp9D.f1=s,_tmp9D)))),_tmp9C)))),speclist,errmsg);}
return;}else{
# 229
++ Cyc_Arg_current;
continue;}}else{_LLC: _tmp3D=_tmp3C;_LLD:(int)_rethrow(_tmp3D);}_LL9:;}};}
# 233
{struct _handler_cons _tmp40;_push_handler(& _tmp40);{int _tmp42=0;if(setjmp(_tmp40.handler))_tmp42=1;if(!_tmp42){
{void*_tmp43=action;void(*_tmp44)(struct _dyneither_ptr);void(*_tmp45)(int);void(*_tmp46)(struct _dyneither_ptr);void(*_tmp47)(struct _dyneither_ptr,struct _dyneither_ptr);int*_tmp48;int*_tmp49;void(*_tmp4A)(struct _dyneither_ptr);void(*_tmp4B)();switch(*((int*)_tmp43)){case 0: _LLF: _tmp4B=((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)_tmp43)->f1;_LL10:
 _tmp4B();goto _LLE;case 1: _LL11: _tmp4A=((struct Cyc_Arg_Flag_spec_Arg_Spec_struct*)_tmp43)->f1;_LL12:
 _tmp4A(s);goto _LLE;case 3: _LL13: _tmp49=((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)_tmp43)->f1;_LL14:
*_tmp49=1;goto _LLE;case 4: _LL15: _tmp48=((struct Cyc_Arg_Clear_spec_Arg_Spec_struct*)_tmp43)->f1;_LL16:
*_tmp48=0;goto _LLE;case 2: _LL17: _tmp47=((struct Cyc_Arg_FlagString_spec_Arg_Spec_struct*)_tmp43)->f1;_LL18:
# 240
 if(Cyc_Arg_current + 1 < l){
_tmp47(s,*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current + 1)));
++ Cyc_Arg_current;}else{
# 244
struct Cyc_Arg_Missing_Arg_error_struct _tmpA0;struct Cyc_Arg_Missing_Arg_error_struct*_tmp9F;Cyc_Arg_stop(initpos,(void*)((_tmp9F=_cycalloc(sizeof(*_tmp9F)),((_tmp9F[0]=((_tmpA0.tag=1,((_tmpA0.f1=s,_tmpA0)))),_tmp9F)))),speclist,errmsg);}
goto _LLE;case 5: _LL19: _tmp46=((struct Cyc_Arg_String_spec_Arg_Spec_struct*)_tmp43)->f1;_LL1A:
# 247
 if(Cyc_Arg_current + 1 < l){
_tmp46(*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current + 1)));
++ Cyc_Arg_current;}else{
# 251
struct Cyc_Arg_Missing_Arg_error_struct _tmpA3;struct Cyc_Arg_Missing_Arg_error_struct*_tmpA2;Cyc_Arg_stop(initpos,(void*)((_tmpA2=_cycalloc(sizeof(*_tmpA2)),((_tmpA2[0]=((_tmpA3.tag=1,((_tmpA3.f1=s,_tmpA3)))),_tmpA2)))),speclist,errmsg);}
goto _LLE;case 6: _LL1B: _tmp45=((struct Cyc_Arg_Int_spec_Arg_Spec_struct*)_tmp43)->f1;_LL1C: {
# 254
struct _dyneither_ptr arg=*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current + 1));
int n=0;
{const char*_tmpA7;void*_tmpA6[1];struct Cyc_IntPtr_sa_ScanfArg_struct _tmpA5;if(((_tmpA5.tag=2,((_tmpA5.f1=& n,((_tmpA6[0]=& _tmpA5,Cyc_sscanf(arg,((_tmpA7="%d",_tag_dyneither(_tmpA7,sizeof(char),3))),_tag_dyneither(_tmpA6,sizeof(void*),1))))))))!= 1){
{struct Cyc_Arg_Wrong_Arg_error_struct _tmpAD;const char*_tmpAC;struct Cyc_Arg_Wrong_Arg_error_struct*_tmpAB;Cyc_Arg_stop(initpos,(void*)((_tmpAB=_cycalloc(sizeof(*_tmpAB)),((_tmpAB[0]=((_tmpAD.tag=3,((_tmpAD.f1=s,((_tmpAD.f2=arg,((_tmpAD.f3=((_tmpAC="an integer",_tag_dyneither(_tmpAC,sizeof(char),11))),_tmpAD)))))))),_tmpAB)))),speclist,errmsg);}
_npop_handler(0);return;}}
# 260
_tmp45(n);
++ Cyc_Arg_current;
goto _LLE;}default: _LL1D: _tmp44=((struct Cyc_Arg_Rest_spec_Arg_Spec_struct*)_tmp43)->f1;_LL1E:
# 264
 while(Cyc_Arg_current < l - 1){
_tmp44(*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current + 1)));
++ Cyc_Arg_current;}
# 268
goto _LLE;}_LLE:;}
# 234
;_pop_handler();}else{void*_tmp41=(void*)_exn_thrown;void*_tmp56=_tmp41;void*_tmp57;struct _dyneither_ptr _tmp58;if(((struct Cyc_Arg_Bad_exn_struct*)_tmp56)->tag == Cyc_Arg_Bad){_LL20: _tmp58=((struct Cyc_Arg_Bad_exn_struct*)_tmp56)->f1;_LL21:
# 271
{struct Cyc_Arg_Message_Arg_error_struct _tmpB0;struct Cyc_Arg_Message_Arg_error_struct*_tmpAF;Cyc_Arg_stop(initpos,(void*)((_tmpAF=_cycalloc(sizeof(*_tmpAF)),((_tmpAF[0]=((_tmpB0.tag=2,((_tmpB0.f1=_tmp58,_tmpB0)))),_tmpAF)))),speclist,errmsg);}goto _LL1F;}else{_LL22: _tmp57=_tmp56;_LL23:(int)_rethrow(_tmp57);}_LL1F:;}};}
# 273
++ Cyc_Arg_current;}else{
# 276
{struct _handler_cons _tmp5B;_push_handler(& _tmp5B);{int _tmp5D=0;if(setjmp(_tmp5B.handler))_tmp5D=1;if(!_tmp5D){anonfun(s);;_pop_handler();}else{void*_tmp5C=(void*)_exn_thrown;void*_tmp5E=_tmp5C;void*_tmp5F;struct _dyneither_ptr _tmp60;if(((struct Cyc_Arg_Bad_exn_struct*)_tmp5E)->tag == Cyc_Arg_Bad){_LL25: _tmp60=((struct Cyc_Arg_Bad_exn_struct*)_tmp5E)->f1;_LL26:
# 278
{struct Cyc_Arg_Message_Arg_error_struct _tmpB3;struct Cyc_Arg_Message_Arg_error_struct*_tmpB2;Cyc_Arg_stop(initpos,(void*)((_tmpB2=_cycalloc(sizeof(*_tmpB2)),((_tmpB2[0]=((_tmpB3.tag=2,((_tmpB3.f1=_tmp60,_tmpB3)))),_tmpB2)))),speclist,errmsg);}goto _LL24;}else{_LL27: _tmp5F=_tmp5E;_LL28:(int)_rethrow(_tmp5F);}_LL24:;}};}
# 280
++ Cyc_Arg_current;}}};}
