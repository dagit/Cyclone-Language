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
typedef struct Cyc___cycFILE Cyc_FILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
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
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 101
struct Cyc_Dict_Dict Cyc_Dict_singleton(int(*cmp)(void*,void*),void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
struct Cyc_Position_Error*Cyc_Position_mk_err(unsigned int,struct _dyneither_ptr);
# 49
void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Relations_Reln;
# 70 "absyn.h"
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 76
typedef void*Cyc_Tcpat_decision_opt_t;
# 84
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 100
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 103
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
# 108
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
# 159
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};
# 180
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 185
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 192
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 212
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 214
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 223
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 317
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 334
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 348
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 369
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 374
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 429 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 432
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 445
typedef void*Cyc_Absyn_funcparams_t;
# 448
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 512
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 538
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 545
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 563
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 726 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 919
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 929
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 943
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 966
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 998
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1108
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1117
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 72 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);char Cyc_Binding_BindingError[13]="BindingError";struct Cyc_Binding_BindingError_exn_struct{char*tag;};
# 40 "binding.cyc"
typedef struct Cyc_Dict_Dict Cyc_Binding_dict_t;
typedef unsigned int Cyc_Binding_seg_t;
# 43
static void Cyc_Binding_err(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 45
Cyc_Position_post_error(Cyc_Position_mk_err(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 51
typedef struct Cyc_List_List*Cyc_Binding_namespace_name_t;struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};
# 56
typedef void*Cyc_Binding_nsdirective_t;struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 62
typedef struct Cyc_Binding_NSCtxt*Cyc_Binding_nsctxt_t;
# 64
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*env,void*(*mkdata)(void*)){
void*_tmp0=mkdata(env);
return({struct Cyc_Binding_NSCtxt*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->curr_ns=0;_tmp1->availables=({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->hd=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3[0]=({struct Cyc_Binding_Namespace_Binding_NSDirective_struct _tmp4;_tmp4.tag=0;_tmp4.f1=0;_tmp4;});_tmp3;});_tmp2->tl=0;_tmp2;});_tmp1->ns_data=
# 68
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*),struct Cyc_List_List*k,void*v))Cyc_Dict_singleton)(Cyc_Absyn_varlist_cmp,0,_tmp0);_tmp1;});}
# 70
void*Cyc_Binding_get_ns_data(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns){
union Cyc_Absyn_Nmspace _tmp5=abs_ns;struct Cyc_List_List*_tmpA;struct Cyc_List_List*_tmp9;switch((_tmp5.Abs_n).tag){case 3: _LL1: _tmp9=(_tmp5.C_n).val;_LL2:
 _tmpA=_tmp9;goto _LL4;case 2: _LL3: _tmpA=(_tmp5.Abs_n).val;_LL4:
 return((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmpA);default: _LL5: _LL6:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=({struct Cyc_Core_Impossible_exn_struct _tmp7;_tmp7.tag=Cyc_Core_Impossible;_tmp7.f1=({const char*_tmp8="Binding:get_ns_data: relative ns";_tag_dyneither(_tmp8,sizeof(char),33);});_tmp7;});_tmp6;}));}_LL0:;}
# 88 "binding.cyc"
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct Cyc_List_List*rel_ns){
# 91
struct Cyc_List_List*_tmpB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ctxt->curr_ns,rel_ns);
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmpB))
return _tmpB;
{struct Cyc_List_List*_tmpC=ctxt->availables;for(0;_tmpC != 0;_tmpC=_tmpC->tl){
void*_tmpD=(void*)_tmpC->hd;void*_tmpE=_tmpD;struct Cyc_List_List*_tmp11;struct Cyc_List_List*_tmp10;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpE)->tag == 1){_LL8: _tmp10=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpE)->f1;_LL9:
 _tmp11=_tmp10;goto _LLB;}else{_LLA: _tmp11=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmpE)->f1;_LLB: {
# 98
struct Cyc_List_List*_tmpF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp11,rel_ns);
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmpF))
return _tmpF;
goto _LL7;}}_LL7:;}}
# 103
({struct Cyc_String_pa_PrintArg_struct _tmp14;_tmp14.tag=0;_tmp14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_str_sepstr(rel_ns,({const char*_tmp15="::";_tag_dyneither(_tmp15,sizeof(char),3);})));({void*_tmp12[1]={& _tmp14};Cyc_Binding_err(loc,({const char*_tmp13="namespace %s not found";_tag_dyneither(_tmp13,sizeof(char),23);}),_tag_dyneither(_tmp12,sizeof(void*),1));});});
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp16=_cycalloc_atomic(sizeof(*_tmp16));_tmp16[0]=({struct Cyc_Binding_BindingError_exn_struct _tmp17;_tmp17.tag=Cyc_Binding_BindingError;_tmp17;});_tmp16;}));}
# 108
void*Cyc_Binding_resolve_lookup(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(void*,struct _dyneither_ptr*)){
struct _tuple0*_tmp18=qv;union Cyc_Absyn_Nmspace _tmp3F;struct _dyneither_ptr*_tmp3E;_LLD: _tmp3F=_tmp18->f1;_tmp3E=_tmp18->f2;_LLE:;{
union Cyc_Absyn_Nmspace _tmp19=_tmp3F;struct Cyc_List_List*_tmp3D;struct Cyc_List_List*_tmp3C;struct Cyc_List_List*_tmp3B;switch((_tmp19.Abs_n).tag){case 1: if((_tmp19.Rel_n).val == 0){_LL10: _LL11:
# 112
{struct Cyc_List_List*_tmp1A=ctxt->availables;for(0;_tmp1A != 0;_tmp1A=_tmp1A->tl){
void*_tmp1B=(void*)_tmp1A->hd;void*_tmp1C=_tmp1B;struct Cyc_List_List*_tmp24;struct Cyc_List_List*_tmp23;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp1C)->tag == 1){_LL1B: _tmp23=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp1C)->f1;_LL1C:
 _tmp24=_tmp23;goto _LL1E;}else{_LL1D: _tmp24=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp1C)->f1;_LL1E:
# 116
{struct _handler_cons _tmp1D;_push_handler(& _tmp1D);{int _tmp1F=0;if(setjmp(_tmp1D.handler))_tmp1F=1;if(!_tmp1F){{void*_tmp20=lookup(((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp24),_tmp3E);_npop_handler(0);return _tmp20;};_pop_handler();}else{void*_tmp1E=(void*)_exn_thrown;void*_tmp21=_tmp1E;void*_tmp22;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp21)->tag == Cyc_Dict_Absent){_LL20: _LL21:
 goto _LL1F;}else{_LL22: _tmp22=_tmp21;_LL23:(int)_rethrow(_tmp22);}_LL1F:;}};}
goto _LL1A;}_LL1A:;}}
# 120
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp25=_cycalloc_atomic(sizeof(*_tmp25));_tmp25[0]=({struct Cyc_Binding_BindingError_exn_struct _tmp26;_tmp26.tag=Cyc_Binding_BindingError;_tmp26;});_tmp25;}));}else{_LL12: _tmp3B=(_tmp19.Rel_n).val;_LL13: {
# 122
struct _handler_cons _tmp27;_push_handler(& _tmp27);{int _tmp29=0;if(setjmp(_tmp27.handler))_tmp29=1;if(!_tmp29){
{struct Cyc_List_List*_tmp2A=Cyc_Binding_resolve_rel_ns(loc,ctxt,_tmp3B);
void*_tmp2B=lookup(((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp2A),_tmp3E);_npop_handler(0);return _tmp2B;}
# 123
;_pop_handler();}else{void*_tmp28=(void*)_exn_thrown;void*_tmp2C=_tmp28;void*_tmp2F;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp2C)->tag == Cyc_Dict_Absent){_LL25: _LL26:
# 125
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp2D=_cycalloc_atomic(sizeof(*_tmp2D));_tmp2D[0]=({struct Cyc_Binding_BindingError_exn_struct _tmp2E;_tmp2E.tag=Cyc_Binding_BindingError;_tmp2E;});_tmp2D;}));}else{_LL27: _tmp2F=_tmp2C;_LL28:(int)_rethrow(_tmp2F);}_LL24:;}};}}case 3: _LL14: _tmp3C=(_tmp19.C_n).val;_LL15:
 _tmp3D=_tmp3C;goto _LL17;case 2: _LL16: _tmp3D=(_tmp19.Abs_n).val;_LL17: {
# 128
struct _handler_cons _tmp30;_push_handler(& _tmp30);{int _tmp32=0;if(setjmp(_tmp30.handler))_tmp32=1;if(!_tmp32){{void*_tmp33=lookup(((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp3D),_tmp3E);_npop_handler(0);return _tmp33;};_pop_handler();}else{void*_tmp31=(void*)_exn_thrown;void*_tmp34=_tmp31;void*_tmp37;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp34)->tag == Cyc_Dict_Absent){_LL2A: _LL2B:
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp35=_cycalloc_atomic(sizeof(*_tmp35));_tmp35[0]=({struct Cyc_Binding_BindingError_exn_struct _tmp36;_tmp36.tag=Cyc_Binding_BindingError;_tmp36;});_tmp35;}));}else{_LL2C: _tmp37=_tmp34;_LL2D:(int)_rethrow(_tmp37);}_LL29:;}};}default: _LL18: _LL19:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38[0]=({struct Cyc_Core_Impossible_exn_struct _tmp39;_tmp39.tag=Cyc_Core_Impossible;_tmp39.f1=({const char*_tmp3A="lookup local in global";_tag_dyneither(_tmp3A,sizeof(char),23);});_tmp39;});_tmp38;}));}_LLF:;};}
# 134
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*ctxt,struct _dyneither_ptr*subname,void*env,void*(*mkdata)(void*)){
struct Cyc_List_List*_tmp40=ctxt->curr_ns;
struct Cyc_List_List*_tmp41=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp40,({struct Cyc_List_List*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->hd=subname;_tmp45->tl=0;_tmp45;}));
if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmp41))
ctxt->ns_data=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,void*v))Cyc_Dict_insert)(ctxt->ns_data,_tmp41,mkdata(env));
ctxt->curr_ns=_tmp41;
ctxt->availables=({struct Cyc_List_List*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->hd=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43[0]=({struct Cyc_Binding_Namespace_Binding_NSDirective_struct _tmp44;_tmp44.tag=0;_tmp44.f1=_tmp41;_tmp44;});_tmp43;});_tmp42->tl=ctxt->availables;_tmp42;});}
# 142
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46[0]=({struct Cyc_Core_Impossible_exn_struct _tmp47;_tmp47.tag=Cyc_Core_Impossible;_tmp47.f1=({const char*_tmp48="leaving topmost namespace";_tag_dyneither(_tmp48,sizeof(char),26);});_tmp47;});_tmp46;}));{
void*_tmp49=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_tmp4A=_tmp49;if(((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp4A)->tag == 0){_LL2F: _LL30:
# 147
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;
ctxt->curr_ns=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ctxt->curr_ns)))->tl);
goto _LL2E;}else{_LL31: _LL32:
# 151
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B[0]=({struct Cyc_Core_Impossible_exn_struct _tmp4C;_tmp4C.tag=Cyc_Core_Impossible;_tmp4C.f1=({const char*_tmp4D="leaving using as namespace";_tag_dyneither(_tmp4D,sizeof(char),27);});_tmp4C;});_tmp4B;}));}_LL2E:;};}
# 154
void Cyc_Binding_enter_using(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename){
struct _tuple0*_tmp4E=usename;union Cyc_Absyn_Nmspace _tmp5F;struct _dyneither_ptr*_tmp5E;_LL34: _tmp5F=_tmp4E->f1;_tmp5E=_tmp4E->f2;_LL35:;{
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp4F=_tmp5F;struct Cyc_List_List*_tmp5A;struct Cyc_List_List*_tmp59;switch((_tmp4F.Loc_n).tag){case 1: _LL37: _tmp59=(_tmp4F.Rel_n).val;_LL38:
# 159
 ns=Cyc_Binding_resolve_rel_ns(loc,ctxt,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp59,({struct Cyc_List_List*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->hd=_tmp5E;_tmp50->tl=0;_tmp50;})));{
struct Cyc_List_List*_tmp51=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);
(*usename).f1=Cyc_Absyn_Abs_n(_tmp51,0);
goto _LL36;};case 2: _LL39: _tmp5A=(_tmp4F.Abs_n).val;_LL3A:
# 164
 ns=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp5A,({struct Cyc_List_List*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->hd=_tmp5E;_tmp52->tl=0;_tmp52;}));
goto _LL36;case 4: _LL3B: _LL3C:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53[0]=({struct Cyc_Core_Impossible_exn_struct _tmp54;_tmp54.tag=Cyc_Core_Impossible;_tmp54.f1=({const char*_tmp55="enter_using, Loc_n";_tag_dyneither(_tmp55,sizeof(char),19);});_tmp54;});_tmp53;}));default: _LL3D: _LL3E:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56[0]=({struct Cyc_Core_Impossible_exn_struct _tmp57;_tmp57.tag=Cyc_Core_Impossible;_tmp57.f1=({const char*_tmp58="enter_using, C_n";_tag_dyneither(_tmp58,sizeof(char),17);});_tmp57;});_tmp56;}));}_LL36:;}
# 169
ctxt->availables=({struct Cyc_List_List*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B->hd=(void*)({struct Cyc_Binding_Using_Binding_NSDirective_struct*_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C[0]=({struct Cyc_Binding_Using_Binding_NSDirective_struct _tmp5D;_tmp5D.tag=1;_tmp5D.f1=ns;_tmp5D;});_tmp5C;});_tmp5B->tl=ctxt->availables;_tmp5B;});};}
# 171
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60[0]=({struct Cyc_Core_Impossible_exn_struct _tmp61;_tmp61.tag=Cyc_Core_Impossible;_tmp61.f1=({const char*_tmp62="leaving topmost namespace as a using";_tag_dyneither(_tmp62,sizeof(char),37);});_tmp61;});_tmp60;}));{
void*_tmp63=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_tmp64=_tmp63;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp64)->tag == 1){_LL40: _LL41:
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;goto _LL3F;}else{_LL42: _LL43:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65[0]=({struct Cyc_Core_Impossible_exn_struct _tmp66;_tmp66.tag=Cyc_Core_Impossible;_tmp66.f1=({const char*_tmp67="leaving namespace as using";_tag_dyneither(_tmp67,sizeof(char),27);});_tmp66;});_tmp65;}));}_LL3F:;};}struct Cyc_Binding_VarRes_Binding_Resolved_struct{int tag;void*f1;};struct Cyc_Binding_AggrRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Binding_EnumRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 196 "binding.cyc"
typedef void*Cyc_Binding_resolved_t;struct Cyc_Binding_ResolveNSEnv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 205
typedef struct Cyc_Binding_ResolveNSEnv*Cyc_Binding_renv_t;struct Cyc_Binding_Env{int in_cinclude;struct Cyc_Binding_NSCtxt*ns;struct Cyc_Dict_Dict*local_vars;};
# 213
typedef struct Cyc_Binding_Env*Cyc_Binding_env_t;
# 215
inline static int Cyc_Binding_in_cinclude(struct Cyc_Binding_Env*env){
return env->in_cinclude;}
# 218
inline static int Cyc_Binding_at_toplevel(struct Cyc_Binding_Env*env){
return env->local_vars == 0;}
# 222
static struct Cyc_Binding_ResolveNSEnv*Cyc_Binding_mt_renv(int ignore){
return({struct Cyc_Binding_ResolveNSEnv*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->aggrdecls=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp68->datatypedecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp68->enumdecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp68->typedefs=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp68->ordinaries=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp68;});}
# 229
static struct Cyc_Absyn_Aggrdecl*Cyc_Binding_lookup_aggrdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->aggrdecls,v);}
# 232
static struct Cyc_List_List*Cyc_Binding_lookup_datatypedecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->datatypedecls,v);}
# 235
static struct Cyc_Absyn_Enumdecl*Cyc_Binding_lookup_enumdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->enumdecls,v);}
# 238
static struct Cyc_Absyn_Typedefdecl*Cyc_Binding_lookup_typedefdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->typedefs,v);}
# 241
static void*Cyc_Binding_lookup_ordinary_global(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->ordinaries,v);}
# 244
static void*Cyc_Binding_lookup_ordinary(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*qv){
struct _tuple0*_tmp69=qv;union Cyc_Absyn_Nmspace _tmp6E;struct _dyneither_ptr*_tmp6D;_LL45: _tmp6E=_tmp69->f1;_tmp6D=_tmp69->f2;_LL46:;{
union Cyc_Absyn_Nmspace _tmp6A=_tmp6E;switch((_tmp6A.Rel_n).tag){case 4: _LL48: _LL49:
# 248
 if(Cyc_Binding_at_toplevel(env) || !((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp6D))
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp6B=_cycalloc_atomic(sizeof(*_tmp6B));_tmp6B[0]=({struct Cyc_Binding_BindingError_exn_struct _tmp6C;_tmp6C.tag=Cyc_Binding_BindingError;_tmp6C;});_tmp6B;}));
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp6D);case 1: if((_tmp6A.Rel_n).val == 0){_LL4A: _LL4B:
# 252
 if(!Cyc_Binding_at_toplevel(env) && ((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp6D))
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp6D);
goto _LL4D;}else{goto _LL4C;}default: _LL4C: _LL4D:
# 256
 return((void*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qv,Cyc_Binding_lookup_ordinary_global);}_LL47:;};}
# 260
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*,struct Cyc_Absyn_Decl*);
void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*,struct Cyc_Absyn_Stmt*);
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*,struct Cyc_Absyn_Pat*);
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*);
void Cyc_Binding_resolve_type(unsigned int,struct Cyc_Binding_Env*,void*);
void Cyc_Binding_resolve_rgnpo(unsigned int,struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_aggrfields(unsigned int,struct Cyc_Binding_Env*,struct Cyc_List_List*);
# 272
void Cyc_Binding_absolutize_decl(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
union Cyc_Absyn_Nmspace _tmp6F=(*qv).f1;union Cyc_Absyn_Nmspace _tmp70=_tmp6F;switch((_tmp70.Abs_n).tag){case 1: if((_tmp70.Rel_n).val == 0){_LL4F: _LL50:
# 275
 if(Cyc_Binding_at_toplevel(env))
(*qv).f1=Cyc_Absyn_Abs_n((env->ns)->curr_ns,Cyc_Binding_in_cinclude(env) || sc == Cyc_Absyn_ExternC);else{
# 278
(*qv).f1=Cyc_Absyn_Loc_n;}
goto _LL4E;}else{goto _LL55;}case 4: _LL51: _LL52:
 goto _LL4E;case 2: _LL53: _LL54:
# 282
 if(!Cyc_Binding_at_toplevel(env))
goto _LL56;
goto _LL4E;default: _LL55: _LL56:
# 286
({struct Cyc_String_pa_PrintArg_struct _tmp73;_tmp73.tag=0;_tmp73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(qv));({void*_tmp71[1]={& _tmp73};Cyc_Binding_err(loc,({const char*_tmp72="qualified names in declarations unimplemented (%s)";_tag_dyneither(_tmp72,sizeof(char),51);}),_tag_dyneither(_tmp71,sizeof(void*),1));});});}_LL4E:;}
# 293
void Cyc_Binding_absolutize_topdecl(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
struct Cyc_Dict_Dict*_tmp74=env->local_vars;
env->local_vars=0;
Cyc_Binding_absolutize_decl(loc,env,qv,sc);
env->local_vars=_tmp74;}
# 300
void Cyc_Binding_resolve_and_add_vardecl(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_Vardecl*vd){
Cyc_Binding_absolutize_decl(loc,env,vd->name,vd->sc);{
struct _tuple0*_tmp75=vd->name;struct _tuple0*_tmp76=_tmp75;union Cyc_Absyn_Nmspace _tmp81;struct _dyneither_ptr*_tmp80;_LL58: _tmp81=_tmp76->f1;_tmp80=_tmp76->f2;_LL59:;
Cyc_Binding_resolve_type(loc,env,vd->type);
if(!Cyc_Binding_at_toplevel(env))
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp80,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp78;_tmp78.tag=0;_tmp78.f1=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp7A;_tmp7A.tag=4;_tmp7A.f1=vd;_tmp7A;});_tmp79;});_tmp78;});_tmp77;}));else{
# 308
struct Cyc_Binding_ResolveNSEnv*_tmp7B=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp81);
_tmp7B->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp7B->ordinaries,_tmp80,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp7D;_tmp7D.tag=0;_tmp7D.f1=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E[0]=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp7F;_tmp7F.tag=1;_tmp7F.f1=vd;_tmp7F;});_tmp7E;});_tmp7D;});_tmp7C;}));}};}
# 315
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*env,struct Cyc_List_List*scs){
struct Cyc_Dict_Dict _tmp82=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp83=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp84=_tmp83;struct Cyc_Absyn_Pat*_tmp87;struct Cyc_Absyn_Exp*_tmp86;struct Cyc_Absyn_Stmt*_tmp85;_LL5B: _tmp87=_tmp84->pattern;_tmp86=_tmp84->where_clause;_tmp85=_tmp84->body;_LL5C:;
Cyc_Binding_resolve_pat(env,_tmp87);
if(_tmp86 != 0)
Cyc_Binding_resolve_exp(env,_tmp86);
Cyc_Binding_resolve_stmt(env,_tmp85);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp82;}
# 325
return;}
# 328
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Stmt*s){
void*_tmp88=s->r;void*_tmp89=_tmp88;struct Cyc_Absyn_Stmt*_tmpA2;struct Cyc_List_List*_tmpA1;struct Cyc_Absyn_Exp*_tmpA0;struct Cyc_List_List*_tmp9F;struct Cyc_Absyn_Decl*_tmp9E;struct Cyc_Absyn_Stmt*_tmp9D;struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Absyn_Stmt*_tmp9B;struct Cyc_List_List*_tmp9A;struct Cyc_Absyn_Exp*_tmp99;struct Cyc_Absyn_Exp*_tmp98;struct Cyc_Absyn_Exp*_tmp97;struct Cyc_Absyn_Stmt*_tmp96;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_Absyn_Stmt*_tmp94;struct Cyc_Absyn_Stmt*_tmp93;struct Cyc_Absyn_Exp*_tmp92;struct Cyc_Absyn_Exp*_tmp91;struct Cyc_Absyn_Stmt*_tmp90;struct Cyc_Absyn_Stmt*_tmp8F;struct Cyc_Absyn_Exp*_tmp8E;struct Cyc_Absyn_Stmt*_tmp8D;struct Cyc_Absyn_Stmt*_tmp8C;struct Cyc_Absyn_Exp*_tmp8B;switch(*((int*)_tmp89)){case 0: _LL5E: _LL5F:
 return;case 1: _LL60: _tmp8B=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp89)->f1;_LL61:
 Cyc_Binding_resolve_exp(env,_tmp8B);return;case 2: _LL62: _tmp8D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp89)->f1;_tmp8C=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp89)->f2;_LL63:
 Cyc_Binding_resolve_stmt(env,_tmp8D);Cyc_Binding_resolve_stmt(env,_tmp8C);return;case 3: _LL64: _tmp8E=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp89)->f1;_LL65:
 if((unsigned int)_tmp8E)Cyc_Binding_resolve_exp(env,_tmp8E);return;case 4: _LL66: _tmp91=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp89)->f1;_tmp90=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp89)->f2;_tmp8F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp89)->f3;_LL67:
# 335
 Cyc_Binding_resolve_exp(env,_tmp91);Cyc_Binding_resolve_stmt(env,_tmp90);Cyc_Binding_resolve_stmt(env,_tmp8F);return;case 14: _LL68: _tmp93=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp89)->f1;_tmp92=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp89)->f2).f1;_LL69:
 _tmp95=_tmp92;_tmp94=_tmp93;goto _LL6B;case 5: _LL6A: _tmp95=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp89)->f1).f1;_tmp94=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp89)->f2;_LL6B:
 Cyc_Binding_resolve_exp(env,_tmp95);Cyc_Binding_resolve_stmt(env,_tmp94);return;case 9: _LL6C: _tmp99=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp89)->f1;_tmp98=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp89)->f2).f1;_tmp97=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp89)->f3).f1;_tmp96=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp89)->f4;_LL6D:
# 339
 Cyc_Binding_resolve_exp(env,_tmp99);Cyc_Binding_resolve_exp(env,_tmp98);Cyc_Binding_resolve_exp(env,_tmp97);
Cyc_Binding_resolve_stmt(env,_tmp96);
return;case 6: _LL6E: _LL6F:
 goto _LL71;case 7: _LL70: _LL71:
 goto _LL73;case 8: _LL72: _LL73:
 return;case 11: _LL74: _tmp9A=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp89)->f1;_LL75:
# 346
 for(0;_tmp9A != 0;_tmp9A=_tmp9A->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmp9A->hd);}
return;case 13: _LL76: _tmp9B=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp89)->f2;_LL77:
 Cyc_Binding_resolve_stmt(env,_tmp9B);return;case 16: _LL78: _tmp9C=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp89)->f1;_LL79:
 Cyc_Binding_resolve_exp(env,_tmp9C);return;case 12: _LL7A: _tmp9E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp89)->f1;_tmp9D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp89)->f2;_LL7B: {
# 353
struct Cyc_Dict_Dict _tmp8A=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
Cyc_Binding_resolve_decl(env,_tmp9E);
Cyc_Binding_resolve_stmt(env,_tmp9D);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp8A;
return;}case 10: _LL7C: _tmpA0=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp89)->f1;_tmp9F=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp89)->f2;_LL7D:
# 360
 Cyc_Binding_resolve_exp(env,_tmpA0);Cyc_Binding_resolve_scs(env,_tmp9F);return;default: _LL7E: _tmpA2=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp89)->f1;_tmpA1=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp89)->f2;_LL7F:
# 362
 Cyc_Binding_resolve_stmt(env,_tmpA2);Cyc_Binding_resolve_scs(env,_tmpA1);return;}_LL5D:;}struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 365
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){
void*_tmpA3=e->r;void*_tmpA4=_tmpA3;void*_tmp130;struct Cyc_List_List*_tmp12F;struct Cyc_List_List*_tmp12E;struct Cyc_Absyn_Datatypedecl*_tmp12D;struct Cyc_Absyn_Datatypefield*_tmp12C;struct Cyc_Absyn_Enumdecl*_tmp12B;struct Cyc_Absyn_Enumfield*_tmp12A;void*_tmp129;struct Cyc_Absyn_Enumfield*_tmp128;struct Cyc_List_List*_tmp127;void*_tmp126;void*_tmp125;void*_tmp124;struct Cyc_Absyn_Stmt*_tmp123;void*_tmp122;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_List_List*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;void**_tmp118;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_List_List*_tmp101;struct Cyc_List_List*_tmp100;struct Cyc_Absyn_Exp*_tmpFF;void*_tmpFE;struct Cyc_List_List*_tmpFD;struct Cyc_Absyn_Vardecl*_tmpFC;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_Absyn_Exp*_tmpFA;struct _tuple0**_tmpF9;struct Cyc_List_List*_tmpF8;struct Cyc_List_List*_tmpF7;struct Cyc_Absyn_Aggrdecl**_tmpF6;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_List_List*_tmpF4;int*_tmpF3;void**_tmpF2;switch(*((int*)_tmpA4)){case 1: _LL81: _tmpF2=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_LL82: {
# 368
void*_tmpA5=*_tmpF2;void*_tmpA6=_tmpA5;struct _tuple0*_tmpC2;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpA6)->tag == 0){_LLD2: _tmpC2=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpA6)->f1;_LLD3: {
# 370
struct _handler_cons _tmpA7;_push_handler(& _tmpA7);{int _tmpA9=0;if(setjmp(_tmpA7.handler))_tmpA9=1;if(!_tmpA9){
{void*_tmpAA=Cyc_Binding_lookup_ordinary(e->loc,env,_tmpC2);void*_tmpAB=_tmpAA;void*_tmpBC;struct Cyc_Absyn_Enumfield*_tmpBB;struct Cyc_Absyn_Enumdecl*_tmpBA;struct Cyc_Absyn_Enumfield*_tmpB9;struct Cyc_Absyn_Datatypedecl*_tmpB8;struct Cyc_Absyn_Datatypefield*_tmpB7;struct Cyc_Absyn_Aggrdecl*_tmpB6;void*_tmpB5;switch(*((int*)_tmpAB)){case 0: _LLD7: _tmpB5=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpAB)->f1;_LLD8:
# 373
*_tmpF2=_tmpB5;_npop_handler(0);return;case 1: _LLD9: _tmpB6=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpAB)->f1;_LLDA:
# 375
({struct Cyc_String_pa_PrintArg_struct _tmpAE;_tmpAE.tag=0;_tmpAE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpC2));({void*_tmpAC[1]={& _tmpAE};Cyc_Binding_err(e->loc,({const char*_tmpAD="bad occurrence of type name %s";_tag_dyneither(_tmpAD,sizeof(char),31);}),_tag_dyneither(_tmpAC,sizeof(void*),1));});});
_npop_handler(0);return;case 2: _LLDB: _tmpB8=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpAB)->f1;_tmpB7=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpAB)->f2;_LLDC:
# 378
 e->r=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpB0;_tmpB0.tag=30;_tmpB0.f1=0;_tmpB0.f2=_tmpB8;_tmpB0.f3=_tmpB7;_tmpB0;});_tmpAF;});_npop_handler(0);return;case 3: _LLDD: _tmpBA=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpAB)->f1;_tmpB9=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpAB)->f2;_LLDE:
# 380
 e->r=(void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpB2;_tmpB2.tag=31;_tmpB2.f1=_tmpBA;_tmpB2.f2=_tmpB9;_tmpB2;});_tmpB1;});_npop_handler(0);return;default: _LLDF: _tmpBC=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpAB)->f1;_tmpBB=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpAB)->f2;_LLE0:
# 382
 e->r=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpB4;_tmpB4.tag=32;_tmpB4.f1=_tmpBC;_tmpB4.f2=_tmpBB;_tmpB4;});_tmpB3;});_npop_handler(0);return;}_LLD6:;}
# 371
;_pop_handler();}else{void*_tmpA8=(void*)_exn_thrown;void*_tmpBD=_tmpA8;void*_tmpC1;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpBD)->tag == Cyc_Binding_BindingError){_LLE2: _LLE3:
# 385
({struct Cyc_String_pa_PrintArg_struct _tmpC0;_tmpC0.tag=0;_tmpC0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpC2));({void*_tmpBE[1]={& _tmpC0};Cyc_Binding_err(e->loc,({const char*_tmpBF="undeclared identifier %s";_tag_dyneither(_tmpBF,sizeof(char),25);}),_tag_dyneither(_tmpBE,sizeof(void*),1));});});
return;}else{_LLE4: _tmpC1=_tmpBD;_LLE5:(int)_rethrow(_tmpC1);}_LLE1:;}};}}else{_LLD4: _LLD5:
# 388
 return;}_LLD1:;}case 9: _LL83: _tmpF5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_tmpF4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA4)->f2;_tmpF3=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA4)->f4;_LL84:
# 391
*_tmpF3=1;
# 393
{struct Cyc_List_List*_tmpC3=_tmpF4;for(0;_tmpC3 != 0;_tmpC3=_tmpC3->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmpC3->hd);}}{
void*_tmpC4=_tmpF5->r;void*_tmpC5=_tmpC4;void**_tmpE4;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC5)->tag == 1){_LLE7: _tmpE4=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC5)->f1;_LLE8: {
# 397
void*_tmpC6=*_tmpE4;void*_tmpC7=_tmpC6;struct _tuple0*_tmpE3;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpC7)->tag == 0){_LLEC: _tmpE3=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpC7)->f1;_LLED: {
# 399
struct _handler_cons _tmpC8;_push_handler(& _tmpC8);{int _tmpCA=0;if(setjmp(_tmpC8.handler))_tmpCA=1;if(!_tmpCA){
{void*_tmpCB=Cyc_Binding_lookup_ordinary(_tmpF5->loc,env,_tmpE3);void*_tmpCC=_tmpCB;struct Cyc_Absyn_Datatypedecl*_tmpDD;struct Cyc_Absyn_Datatypefield*_tmpDC;struct Cyc_Absyn_Aggrdecl*_tmpDB;void*_tmpDA;switch(*((int*)_tmpCC)){case 0: _LLF1: _tmpDA=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpCC)->f1;_LLF2:
# 402
*_tmpE4=_tmpDA;_npop_handler(0);return;case 1: _LLF3: _tmpDB=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpCC)->f1;_LLF4: {
# 404
struct Cyc_List_List*_tmpCD=0;
for(0;_tmpF4 != 0;_tmpF4=_tmpF4->tl){
_tmpCD=({struct Cyc_List_List*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->hd=({struct _tuple10*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->f1=0;_tmpCF->f2=(struct Cyc_Absyn_Exp*)_tmpF4->hd;_tmpCF;});_tmpCE->tl=_tmpCD;_tmpCE;});}
e->r=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0[0]=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpD1;_tmpD1.tag=28;_tmpD1.f1=_tmpDB->name;_tmpD1.f2=0;_tmpD1.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpCD);_tmpD1.f4=_tmpDB;_tmpD1;});_tmpD0;});
_npop_handler(0);return;}case 2: _LLF5: _tmpDD=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpCC)->f1;_tmpDC=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpCC)->f2;_LLF6:
# 410
 if(_tmpDC->typs == 0)
# 412
({struct Cyc_String_pa_PrintArg_struct _tmpD4;_tmpD4.tag=0;_tmpD4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpDC->name));({void*_tmpD2[1]={& _tmpD4};Cyc_Binding_err(e->loc,({const char*_tmpD3="%s is a constant, not a function";_tag_dyneither(_tmpD3,sizeof(char),33);}),_tag_dyneither(_tmpD2,sizeof(void*),1));});});
e->r=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpD6;_tmpD6.tag=30;_tmpD6.f1=_tmpF4;_tmpD6.f2=_tmpDD;_tmpD6.f3=_tmpDC;_tmpD6;});_tmpD5;});
_npop_handler(0);return;case 4: _LLF7: _LLF8:
 goto _LLFA;default: _LLF9: _LLFA:
# 418
({struct Cyc_String_pa_PrintArg_struct _tmpD9;_tmpD9.tag=0;_tmpD9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpE3));({void*_tmpD7[1]={& _tmpD9};Cyc_Binding_err(e->loc,({const char*_tmpD8="%s is an enum constructor, not a function";_tag_dyneither(_tmpD8,sizeof(char),42);}),_tag_dyneither(_tmpD7,sizeof(void*),1));});});
_npop_handler(0);return;}_LLF0:;}
# 400
;_pop_handler();}else{void*_tmpC9=(void*)_exn_thrown;void*_tmpDE=_tmpC9;void*_tmpE2;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpDE)->tag == Cyc_Binding_BindingError){_LLFC: _LLFD:
# 423
({struct Cyc_String_pa_PrintArg_struct _tmpE1;_tmpE1.tag=0;_tmpE1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpE3));({void*_tmpDF[1]={& _tmpE1};Cyc_Binding_err(e->loc,({const char*_tmpE0="undeclared identifier %s";_tag_dyneither(_tmpE0,sizeof(char),25);}),_tag_dyneither(_tmpDF,sizeof(void*),1));});});
return;}else{_LLFE: _tmpE2=_tmpDE;_LLFF:(int)_rethrow(_tmpE2);}_LLFB:;}};}}else{_LLEE: _LLEF:
# 426
 return;}_LLEB:;}}else{_LLE9: _LLEA:
# 428
 Cyc_Binding_resolve_exp(env,_tmpF5);return;}_LLE6:;};case 28: _LL85: _tmpF9=(struct _tuple0**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_tmpF8=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA4)->f2;_tmpF7=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA4)->f3;_tmpF6=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA4)->f4;_LL86:
# 432
 for(0;_tmpF7 != 0;_tmpF7=_tmpF7->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmpF7->hd)).f2);}{
struct _handler_cons _tmpE5;_push_handler(& _tmpE5);{int _tmpE7=0;if(setjmp(_tmpE5.handler))_tmpE7=1;if(!_tmpE7){
*_tmpF6=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(e->loc,env->ns,*_tmpF9,Cyc_Binding_lookup_aggrdecl);
*_tmpF9=((struct Cyc_Absyn_Aggrdecl*)_check_null(*_tmpF6))->name;
_npop_handler(0);return;
# 435
;_pop_handler();}else{void*_tmpE6=(void*)_exn_thrown;void*_tmpE8=_tmpE6;void*_tmpEC;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpE8)->tag == Cyc_Binding_BindingError){_LL101: _LL102:
# 439
({struct Cyc_String_pa_PrintArg_struct _tmpEB;_tmpEB.tag=0;_tmpEB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*_tmpF9));({void*_tmpE9[1]={& _tmpEB};Cyc_Binding_err(e->loc,({const char*_tmpEA="unbound struct/union name %s";_tag_dyneither(_tmpEA,sizeof(char),29);}),_tag_dyneither(_tmpE9,sizeof(void*),1));});});
return;}else{_LL103: _tmpEC=_tmpE8;_LL104:(int)_rethrow(_tmpEC);}_LL100:;}};};case 26: _LL87: _tmpFC=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_tmpFB=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA4)->f2;_tmpFA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA4)->f3;_LL88:
# 444
 Cyc_Binding_resolve_exp(env,_tmpFB);{
struct Cyc_Dict_Dict _tmpED=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,_tmpFC);
Cyc_Binding_resolve_exp(env,_tmpFA);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmpED;
return;};case 35: _LL89: _tmpFD=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpA4)->f2;_LL8A:
# 453
 for(0;_tmpFD != 0;_tmpFD=_tmpFD->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmpFD->hd)).f2);}
return;case 27: _LL8B: _tmpFF=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_tmpFE=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA4)->f2;_LL8C:
# 459
 Cyc_Binding_resolve_exp(env,_tmpFF);
Cyc_Binding_resolve_type(e->loc,env,_tmpFE);
return;case 0: _LL8D: _LL8E:
# 463
 return;case 23: _LL8F: _tmp100=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_LL90:
# 465
 _tmp101=_tmp100;goto _LL92;case 2: _LL91: _tmp101=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpA4)->f2;_LL92:
# 467
 for(0;_tmp101 != 0;_tmp101=_tmp101->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmp101->hd);}
return;case 37: _LL93: _tmp102=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_LL94:
# 471
 _tmp103=_tmp102;goto _LL96;case 11: _LL95: _tmp103=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_LL96:
 _tmp104=_tmp103;goto _LL98;case 17: _LL97: _tmp104=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_LL98:
 _tmp105=_tmp104;goto _LL9A;case 10: _LL99: _tmp105=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_LL9A:
 _tmp106=_tmp105;goto _LL9C;case 4: _LL9B: _tmp106=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_LL9C:
 _tmp107=_tmp106;goto _LL9E;case 21: _LL9D: _tmp107=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_LL9E:
 _tmp108=_tmp107;goto _LLA0;case 20: _LL9F: _tmp108=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_LLA0:
 _tmp109=_tmp108;goto _LLA2;case 14: _LLA1: _tmp109=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_LLA2:
 _tmp10A=_tmp109;goto _LLA4;case 19: _LLA3: _tmp10A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_LLA4:
 Cyc_Binding_resolve_exp(env,_tmp10A);return;case 34: _LLA5: _tmp10C=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_tmp10B=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpA4)->f2;_LLA6:
# 481
 _tmp10E=_tmp10C;_tmp10D=_tmp10B;goto _LLA8;case 8: _LLA7: _tmp10E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_tmp10D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA4)->f2;_LLA8:
 _tmp110=_tmp10E;_tmp10F=_tmp10D;goto _LLAA;case 3: _LLA9: _tmp110=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_tmp10F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpA4)->f3;_LLAA:
 _tmp112=_tmp110;_tmp111=_tmp10F;goto _LLAC;case 22: _LLAB: _tmp112=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_tmp111=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA4)->f2;_LLAC:
 _tmp114=_tmp112;_tmp113=_tmp111;goto _LLAE;case 6: _LLAD: _tmp114=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_tmp113=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpA4)->f2;_LLAE:
 _tmp116=_tmp114;_tmp115=_tmp113;goto _LLB0;case 7: _LLAF: _tmp116=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_tmp115=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpA4)->f2;_LLB0:
 Cyc_Binding_resolve_exp(env,_tmp116);Cyc_Binding_resolve_exp(env,_tmp115);return;case 33: _LLB1: _tmp119=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA4)->f1).rgn;_tmp118=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA4)->f1).elt_type;_tmp117=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA4)->f1).num_elts;_LLB2:
# 489
 if(_tmp119 != 0)Cyc_Binding_resolve_exp(env,_tmp119);
if(_tmp118 != 0)Cyc_Binding_resolve_type(e->loc,env,*_tmp118);
Cyc_Binding_resolve_exp(env,_tmp117);
return;case 15: _LLB3: _tmp11B=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_tmp11A=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpA4)->f2;_LLB4:
# 495
 if(_tmp11B != 0)Cyc_Binding_resolve_exp(env,_tmp11B);
Cyc_Binding_resolve_exp(env,_tmp11A);
return;case 5: _LLB5: _tmp11E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_tmp11D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA4)->f2;_tmp11C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA4)->f3;_LLB6:
# 500
 Cyc_Binding_resolve_exp(env,_tmp11E);Cyc_Binding_resolve_exp(env,_tmp11D);Cyc_Binding_resolve_exp(env,_tmp11C);return;case 12: _LLB7: _tmp120=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_tmp11F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA4)->f2;_LLB8:
# 503
 Cyc_Binding_resolve_exp(env,_tmp120);
for(0;_tmp11F != 0;_tmp11F=_tmp11F->tl){
Cyc_Binding_resolve_type(e->loc,env,(void*)_tmp11F->hd);}
return;case 13: _LLB9: _tmp122=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_tmp121=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA4)->f2;_LLBA:
# 509
 Cyc_Binding_resolve_exp(env,_tmp121);Cyc_Binding_resolve_type(e->loc,env,_tmp122);return;case 36: _LLBB: _tmp123=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_LLBC:
# 511
 Cyc_Binding_resolve_stmt(env,_tmp123);return;case 38: _LLBD: _tmp124=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_LLBE:
# 513
 _tmp125=_tmp124;goto _LLC0;case 18: _LLBF: _tmp125=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_LLC0:
 _tmp126=_tmp125;goto _LLC2;case 16: _LLC1: _tmp126=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_LLC2:
 Cyc_Binding_resolve_type(e->loc,env,_tmp126);return;case 25: _LLC3: _tmp127=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_LLC4:
# 518
 for(0;_tmp127 != 0;_tmp127=_tmp127->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp127->hd)).f2);}
return;case 24: _LLC5: _LLC6:
# 523
({void*_tmpEE=0;Cyc_Binding_err(e->loc,({const char*_tmpEF="binding: compound literals unimplemented";_tag_dyneither(_tmpEF,sizeof(char),41);}),_tag_dyneither(_tmpEE,sizeof(void*),0));});return;case 32: _LLC7: _tmp129=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_tmp128=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpA4)->f2;_LLC8:
# 526
 Cyc_Binding_resolve_type(e->loc,env,_tmp129);return;case 31: _LLC9: _tmp12B=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_tmp12A=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpA4)->f2;_LLCA:
 return;case 30: _LLCB: _tmp12E=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_tmp12D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA4)->f2;_tmp12C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA4)->f3;_LLCC:
# 529
 for(0;_tmp12E != 0;_tmp12E=_tmp12E->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmp12E->hd);}
return;case 29: _LLCD: _tmp130=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpA4)->f1;_tmp12F=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpA4)->f2;_LLCE:
# 533
 Cyc_Binding_resolve_type(e->loc,env,_tmp130);
for(0;_tmp12F != 0;_tmp12F=_tmp12F->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp12F->hd)).f2);}
return;default: _LLCF: _LLD0:
# 539
({void*_tmpF0=0;Cyc_Binding_err(e->loc,({const char*_tmpF1="asm expressions cannot occur within Cyclone code";_tag_dyneither(_tmpF1,sizeof(char),49);}),_tag_dyneither(_tmpF0,sizeof(void*),0));});}_LL80:;}
# 543
void Cyc_Binding_resolve_aggrfields(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*fs){
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp131=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_tmp132=_tmp131;struct _dyneither_ptr*_tmp138;struct Cyc_Absyn_Tqual _tmp137;void*_tmp136;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_List_List*_tmp134;struct Cyc_Absyn_Exp*_tmp133;_LL106: _tmp138=_tmp132->name;_tmp137=_tmp132->tq;_tmp136=_tmp132->type;_tmp135=_tmp132->width;_tmp134=_tmp132->attributes;_tmp133=_tmp132->requires_clause;_LL107:;
Cyc_Binding_resolve_type(loc,env,_tmp136);
if(_tmp135 != 0)
Cyc_Binding_resolve_exp(env,_tmp135);
if(_tmp133 != 0)
Cyc_Binding_resolve_exp(env,_tmp133);}
# 552
return;}struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};
# 560
void Cyc_Binding_resolve_type(unsigned int loc,struct Cyc_Binding_Env*env,void*t){
void*_tmp139=t;struct Cyc_Absyn_Exp*_tmp1AE;struct Cyc_Absyn_Exp*_tmp1AD;void*_tmp1AC;void*_tmp1AB;void*_tmp1AA;void*_tmp1A9;void*_tmp1A8;void*_tmp1A7;struct Cyc_List_List*_tmp1A6;struct Cyc_Absyn_FnInfo _tmp1A5;void*_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_List_List*_tmp1A2;void*_tmp1A1;void*_tmp1A0;union Cyc_Absyn_Constraint*_tmp19F;void*_tmp19E;struct Cyc_List_List*_tmp19D;struct Cyc_List_List*_tmp19C;struct Cyc_Absyn_TypeDecl*_tmp19B;void***_tmp19A;struct _tuple0**_tmp199;struct Cyc_List_List*_tmp198;struct Cyc_Absyn_Typedefdecl**_tmp197;struct _tuple0**_tmp196;struct Cyc_Absyn_Enumdecl*_tmp195;union Cyc_Absyn_AggrInfoU*_tmp194;struct Cyc_List_List*_tmp193;union Cyc_Absyn_DatatypeFieldInfoU*_tmp192;struct Cyc_List_List*_tmp191;union Cyc_Absyn_DatatypeInfoU*_tmp190;struct Cyc_List_List*_tmp18F;switch(*((int*)_tmp139)){case 3: _LL109: _tmp190=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp139)->f1).datatype_info;_tmp18F=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp139)->f1).targs;_LL10A:
# 563
 for(0;_tmp18F != 0;_tmp18F=_tmp18F->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp18F->hd);}{
union Cyc_Absyn_DatatypeInfoU _tmp13A=*_tmp190;union Cyc_Absyn_DatatypeInfoU _tmp13B=_tmp13A;struct _tuple0*_tmp144;int _tmp143;if((_tmp13B.UnknownDatatype).tag == 1){_LL144: _tmp144=((_tmp13B.UnknownDatatype).val).name;_tmp143=((_tmp13B.UnknownDatatype).val).is_extensible;_LL145: {
# 567
struct _handler_cons _tmp13C;_push_handler(& _tmp13C);{int _tmp13E=0;if(setjmp(_tmp13C.handler))_tmp13E=1;if(!_tmp13E){
{struct Cyc_Absyn_Datatypedecl*_tmp13F=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp144,Cyc_Binding_lookup_datatypedecl))->hd;
*_tmp190=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp140;_tmp140.name=_tmp13F->name;_tmp140.is_extensible=_tmp143;_tmp140;}));
_npop_handler(0);return;}
# 568
;_pop_handler();}else{void*_tmp13D=(void*)_exn_thrown;void*_tmp141=_tmp13D;void*_tmp142;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp141)->tag == Cyc_Binding_BindingError){_LL149: _LL14A:
# 572
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp144,Cyc_Absyn_Public);return;}else{_LL14B: _tmp142=_tmp141;_LL14C:(int)_rethrow(_tmp142);}_LL148:;}};}}else{_LL146: _LL147:
 return;}_LL143:;};case 4: _LL10B: _tmp192=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp139)->f1).field_info;_tmp191=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp139)->f1).targs;_LL10C:
# 576
 for(0;_tmp191 != 0;_tmp191=_tmp191->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp191->hd);}{
union Cyc_Absyn_DatatypeFieldInfoU _tmp145=*_tmp192;union Cyc_Absyn_DatatypeFieldInfoU _tmp146=_tmp145;struct _tuple0*_tmp15A;struct _tuple0*_tmp159;int _tmp158;if((_tmp146.UnknownDatatypefield).tag == 1){_LL14E: _tmp15A=((_tmp146.UnknownDatatypefield).val).datatype_name;_tmp159=((_tmp146.UnknownDatatypefield).val).field_name;_tmp158=((_tmp146.UnknownDatatypefield).val).is_extensible;_LL14F:
# 582
{struct _handler_cons _tmp147;_push_handler(& _tmp147);{int _tmp149=0;if(setjmp(_tmp147.handler))_tmp149=1;if(!_tmp149){
{void*_tmp14A=Cyc_Binding_lookup_ordinary(loc,env,_tmp159);void*_tmp14B=_tmp14A;struct Cyc_Absyn_Datatypedecl*_tmp152;struct Cyc_Absyn_Datatypefield*_tmp151;if(((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp14B)->tag == 2){_LL153: _tmp152=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp14B)->f1;_tmp151=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp14B)->f2;_LL154: {
# 585
struct Cyc_Absyn_Datatypedecl*_tmp14C=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp15A,Cyc_Binding_lookup_datatypedecl))->hd;
if(Cyc_Absyn_qvar_cmp(_tmp152->name,_tmp14C->name)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp14F;_tmp14F.tag=0;_tmp14F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp151->name));({void*_tmp14D[1]={& _tmp14F};Cyc_Binding_err(loc,({const char*_tmp14E="shadowed datatype field %s not yet implemented properly";_tag_dyneither(_tmp14E,sizeof(char),56);}),_tag_dyneither(_tmp14D,sizeof(void*),1));});});
_npop_handler(0);return;}
# 591
*_tmp192=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp150;_tmp150.datatype_name=_tmp152->name;_tmp150.field_name=_tmp151->name;_tmp150.is_extensible=_tmp158;_tmp150;}));
# 593
_npop_handler(0);return;}}else{_LL155: _LL156:
 goto _LL152;}_LL152:;}
# 583
;_pop_handler();}else{void*_tmp148=(void*)_exn_thrown;void*_tmp153=_tmp148;void*_tmp154;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp153)->tag == Cyc_Binding_BindingError){_LL158: _LL159:
# 596
 goto _LL157;}else{_LL15A: _tmp154=_tmp153;_LL15B:(int)_rethrow(_tmp154);}_LL157:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp157;_tmp157.tag=0;_tmp157.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp159));({void*_tmp155[1]={& _tmp157};Cyc_Binding_err(loc,({const char*_tmp156="%s is not a datatype field";_tag_dyneither(_tmp156,sizeof(char),27);}),_tag_dyneither(_tmp155,sizeof(void*),1));});});
return;}else{_LL150: _LL151:
 return;}_LL14D:;};case 11: _LL10D: _tmp194=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp139)->f1).aggr_info;_tmp193=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp139)->f1).targs;_LL10E:
# 603
 for(0;_tmp193 != 0;_tmp193=_tmp193->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp193->hd);}{
union Cyc_Absyn_AggrInfoU _tmp15B=*_tmp194;union Cyc_Absyn_AggrInfoU _tmp15C=_tmp15B;enum Cyc_Absyn_AggrKind _tmp169;struct _tuple0*_tmp168;struct Cyc_Core_Opt*_tmp167;if((_tmp15C.UnknownAggr).tag == 1){_LL15D: _tmp169=((_tmp15C.UnknownAggr).val).f1;_tmp168=((_tmp15C.UnknownAggr).val).f2;_tmp167=((_tmp15C.UnknownAggr).val).f3;_LL15E: {
# 607
struct _handler_cons _tmp15D;_push_handler(& _tmp15D);{int _tmp15F=0;if(setjmp(_tmp15D.handler))_tmp15F=1;if(!_tmp15F){
{struct Cyc_Absyn_Aggrdecl*_tmp160=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp168,Cyc_Binding_lookup_aggrdecl);
if(_tmp160->kind != _tmp169)
({void*_tmp161=0;Cyc_Binding_err(loc,({const char*_tmp162="struct vs. union mismatch with earlier declaration";_tag_dyneither(_tmp162,sizeof(char),51);}),_tag_dyneither(_tmp161,sizeof(void*),0));});
if(((_tmp169 == Cyc_Absyn_UnionA  && _tmp167 != 0) && _tmp160->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp160->impl))->tagged != (int)_tmp167->v)
# 613
({void*_tmp163=0;Cyc_Binding_err(loc,({const char*_tmp164="@tagged mismatch with earlier declaration";_tag_dyneither(_tmp164,sizeof(char),42);}),_tag_dyneither(_tmp163,sizeof(void*),0));});
*_tmp194=Cyc_Absyn_UnknownAggr(_tmp169,_tmp160->name,_tmp167);
_npop_handler(0);return;}
# 608
;_pop_handler();}else{void*_tmp15E=(void*)_exn_thrown;void*_tmp165=_tmp15E;void*_tmp166;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp165)->tag == Cyc_Binding_BindingError){_LL162: _LL163:
# 617
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp168,Cyc_Absyn_Public);return;}else{_LL164: _tmp166=_tmp165;_LL165:(int)_rethrow(_tmp166);}_LL161:;}};}}else{_LL15F: _LL160:
 return;}_LL15C:;};case 13: _LL10F: _tmp196=(struct _tuple0**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp139)->f1;_tmp195=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp139)->f2;_LL110:
# 621
 if(_tmp195 != 0)
return;{
struct _handler_cons _tmp16A;_push_handler(& _tmp16A);{int _tmp16C=0;if(setjmp(_tmp16A.handler))_tmp16C=1;if(!_tmp16C){
{struct Cyc_Absyn_Enumdecl*_tmp16D=((struct Cyc_Absyn_Enumdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Enumdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*_tmp196,Cyc_Binding_lookup_enumdecl);
*_tmp196=_tmp16D->name;
_npop_handler(0);return;}
# 624
;_pop_handler();}else{void*_tmp16B=(void*)_exn_thrown;void*_tmp16E=_tmp16B;void*_tmp16F;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp16E)->tag == Cyc_Binding_BindingError){_LL167: _LL168:
# 628
 Cyc_Binding_absolutize_topdecl(loc,env,*_tmp196,Cyc_Absyn_Public);return;}else{_LL169: _tmp16F=_tmp16E;_LL16A:(int)_rethrow(_tmp16F);}_LL166:;}};};case 17: _LL111: _tmp199=(struct _tuple0**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp139)->f1;_tmp198=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp139)->f2;_tmp197=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp139)->f3;_LL112:
# 631
 for(0;_tmp198 != 0;_tmp198=_tmp198->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp198->hd);}
{struct _handler_cons _tmp170;_push_handler(& _tmp170);{int _tmp172=0;if(setjmp(_tmp170.handler))_tmp172=1;if(!_tmp172){
{struct Cyc_Absyn_Typedefdecl*_tmp173=((struct Cyc_Absyn_Typedefdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*_tmp199,Cyc_Binding_lookup_typedefdecl);
# 636
*_tmp199=_tmp173->name;
_npop_handler(0);return;}
# 634
;_pop_handler();}else{void*_tmp171=(void*)_exn_thrown;void*_tmp174=_tmp171;void*_tmp175;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp174)->tag == Cyc_Binding_BindingError){_LL16C: _LL16D:
# 638
 goto _LL16B;}else{_LL16E: _tmp175=_tmp174;_LL16F:(int)_rethrow(_tmp175);}_LL16B:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp178;_tmp178.tag=0;_tmp178.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*_tmp199));({void*_tmp176[1]={& _tmp178};Cyc_Binding_err(loc,({const char*_tmp177="unbound typdef name %s";_tag_dyneither(_tmp177,sizeof(char),23);}),_tag_dyneither(_tmp176,sizeof(void*),1));});});
return;case 26: _LL113: _tmp19B=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp139)->f1;_tmp19A=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp139)->f2;_LL114: {
# 644
struct Cyc_Dict_Dict*_tmp179=env->local_vars;
env->local_vars=0;
{void*_tmp17A=_tmp19B->r;void*_tmp17B=_tmp17A;struct Cyc_Absyn_Datatypedecl*_tmp187;struct Cyc_Absyn_Enumdecl*_tmp186;struct Cyc_Absyn_Aggrdecl*_tmp185;switch(*((int*)_tmp17B)){case 0: _LL171: _tmp185=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp17B)->f1;_LL172:
# 648
 Cyc_Binding_resolve_decl(env,({struct Cyc_Absyn_Decl*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->r=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp17E;_tmp17E.tag=5;_tmp17E.f1=_tmp185;_tmp17E;});_tmp17D;});_tmp17C->loc=_tmp19B->loc;_tmp17C;}));goto _LL170;case 1: _LL173: _tmp186=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp17B)->f1;_LL174:
# 650
 Cyc_Binding_resolve_decl(env,({struct Cyc_Absyn_Decl*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F->r=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180[0]=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp181;_tmp181.tag=7;_tmp181.f1=_tmp186;_tmp181;});_tmp180;});_tmp17F->loc=_tmp19B->loc;_tmp17F;}));goto _LL170;default: _LL175: _tmp187=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp17B)->f1;_LL176:
# 652
 Cyc_Binding_resolve_decl(env,({struct Cyc_Absyn_Decl*_tmp182=_cycalloc(sizeof(*_tmp182));_tmp182->r=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183[0]=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp184;_tmp184.tag=6;_tmp184.f1=_tmp187;_tmp184;});_tmp183;});_tmp182->loc=_tmp19B->loc;_tmp182;}));goto _LL170;}_LL170:;}
# 654
env->local_vars=_tmp179;
return;}case 12: _LL115: _tmp19C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp139)->f2;_LL116:
# 661
 Cyc_Binding_resolve_aggrfields(loc,env,_tmp19C);
return;case 14: _LL117: _tmp19D=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp139)->f1;_LL118:
# 666
 for(0;_tmp19D != 0;_tmp19D=_tmp19D->tl){
struct Cyc_Absyn_Enumfield*_tmp188=(struct Cyc_Absyn_Enumfield*)_tmp19D->hd;
if(_tmp188->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp188->tag));}
# 671
return;case 1: _LL119: _tmp19E=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp139)->f2;_LL11A:
 if(_tmp19E != 0)Cyc_Binding_resolve_type(loc,env,_tmp19E);return;case 28: _LL11B: _LL11C:
# 674
 goto _LL11E;case 0: _LL11D: _LL11E:
 goto _LL120;case 6: _LL11F: _LL120:
 goto _LL122;case 7: _LL121: _LL122:
 goto _LL124;case 20: _LL123: _LL124:
 goto _LL126;case 21: _LL125: _LL126:
 goto _LL128;case 22: _LL127: _LL128:
 goto _LL12A;case 2: _LL129: _LL12A:
 return;case 5: _LL12B: _tmp1A1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp139)->f1).elt_typ;_tmp1A0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp139)->f1).ptr_atts).rgn;_tmp19F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp139)->f1).ptr_atts).bounds;_LL12C:
# 684
 Cyc_Binding_resolve_type(loc,env,_tmp1A1);
Cyc_Binding_resolve_type(loc,env,_tmp1A0);
while(1){
union Cyc_Absyn_Constraint*_tmp189=_tmp19F;void*_tmp18D;union Cyc_Absyn_Constraint*_tmp18C;switch((((union Cyc_Absyn_Constraint*)_tmp189)->Forward_constr).tag){case 3: _LL178: _LL179:
 return;case 2: _LL17A: _tmp18C=(_tmp189->Forward_constr).val;_LL17B:
 _tmp19F=_tmp18C;continue;default: _LL17C: _tmp18D=(void*)(_tmp189->Eq_constr).val;_LL17D: {
# 691
void*_tmp18A=_tmp18D;struct Cyc_Absyn_Exp*_tmp18B;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp18A)->tag == 0){_LL17F: _LL180:
 return;}else{_LL181: _tmp18B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp18A)->f1;_LL182:
 Cyc_Binding_resolve_exp(env,_tmp18B);return;}_LL17E:;}}_LL177:;}case 10: _LL12D: _tmp1A2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp139)->f1;_LL12E:
# 697
 for(0;_tmp1A2 != 0;_tmp1A2=_tmp1A2->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple11*)_tmp1A2->hd)).f2);}
return;case 8: _LL12F: _tmp1A4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp139)->f1).elt_type;_tmp1A3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp139)->f1).num_elts;_LL130:
# 701
 Cyc_Binding_resolve_type(loc,env,_tmp1A4);
if(_tmp1A3 != 0)Cyc_Binding_resolve_exp(env,_tmp1A3);
return;case 9: _LL131: _tmp1A5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp139)->f1;_LL132:
# 705
 if(_tmp1A5.effect != 0)
Cyc_Binding_resolve_type(loc,env,_tmp1A5.effect);
Cyc_Binding_resolve_type(loc,env,_tmp1A5.ret_typ);
{struct Cyc_List_List*_tmp18E=_tmp1A5.args;for(0;_tmp18E != 0;_tmp18E=_tmp18E->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple8*)_tmp18E->hd)).f3);}}
if(_tmp1A5.cyc_varargs != 0)
Cyc_Binding_resolve_type(loc,env,(_tmp1A5.cyc_varargs)->type);
Cyc_Binding_resolve_rgnpo(loc,env,_tmp1A5.rgn_po);
if(_tmp1A5.requires_clause != 0)
Cyc_Binding_resolve_exp(env,_tmp1A5.requires_clause);
if(_tmp1A5.ensures_clause != 0)
Cyc_Binding_resolve_exp(env,_tmp1A5.ensures_clause);
return;case 24: _LL133: _tmp1A6=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp139)->f1;_LL134:
# 719
 for(0;_tmp1A6 != 0;_tmp1A6=_tmp1A6->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1A6->hd);}
return;case 19: _LL135: _tmp1A7=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp139)->f1;_LL136:
# 723
 _tmp1A8=_tmp1A7;goto _LL138;case 23: _LL137: _tmp1A8=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp139)->f1;_LL138:
 _tmp1A9=_tmp1A8;goto _LL13A;case 25: _LL139: _tmp1A9=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp139)->f1;_LL13A:
 _tmp1AA=_tmp1A9;goto _LL13C;case 15: _LL13B: _tmp1AA=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp139)->f1;_LL13C:
 Cyc_Binding_resolve_type(loc,env,_tmp1AA);return;case 16: _LL13D: _tmp1AC=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp139)->f1;_tmp1AB=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp139)->f2;_LL13E:
# 729
 Cyc_Binding_resolve_type(loc,env,_tmp1AC);Cyc_Binding_resolve_type(loc,env,_tmp1AB);return;case 18: _LL13F: _tmp1AD=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp139)->f1;_LL140:
# 731
 _tmp1AE=_tmp1AD;goto _LL142;default: _LL141: _tmp1AE=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp139)->f1;_LL142:
 Cyc_Binding_resolve_exp(env,_tmp1AE);return;}_LL108:;}struct _tuple12{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 736
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Pat*p){
struct Cyc_Absyn_Pat*_tmp1AF=p;void*_tmp209;unsigned int _tmp208;_LL184: _tmp209=_tmp1AF->r;_tmp208=_tmp1AF->loc;_LL185:;{
void*_tmp1B0=_tmp209;struct Cyc_List_List*_tmp207;struct Cyc_Absyn_Exp*_tmp206;struct Cyc_List_List*_tmp205;struct Cyc_Absyn_Pat*_tmp204;struct Cyc_Absyn_Vardecl*_tmp203;struct Cyc_Absyn_Vardecl*_tmp202;struct Cyc_Absyn_Vardecl*_tmp201;struct Cyc_Absyn_Pat*_tmp200;struct Cyc_Absyn_Vardecl*_tmp1FF;struct Cyc_Absyn_Pat*_tmp1FE;struct Cyc_List_List*_tmp1FD;struct Cyc_List_List*_tmp1FC;struct _tuple0*_tmp1FB;struct Cyc_List_List*_tmp1FA;struct Cyc_List_List*_tmp1F9;struct Cyc_List_List*_tmp1F8;int _tmp1F7;struct _tuple0*_tmp1F6;struct Cyc_List_List*_tmp1F5;int _tmp1F4;struct _tuple0*_tmp1F3;switch(*((int*)_tmp1B0)){case 15: _LL187: _tmp1F3=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1B0)->f1;_LL188:
# 740
{struct _handler_cons _tmp1B1;_push_handler(& _tmp1B1);{int _tmp1B3=0;if(setjmp(_tmp1B1.handler))_tmp1B3=1;if(!_tmp1B3){
{void*_tmp1B4=Cyc_Binding_lookup_ordinary(_tmp208,env,_tmp1F3);void*_tmp1B5=_tmp1B4;void*_tmp1C3;struct Cyc_Absyn_Enumfield*_tmp1C2;struct Cyc_Absyn_Enumdecl*_tmp1C1;struct Cyc_Absyn_Enumfield*_tmp1C0;struct Cyc_Absyn_Datatypedecl*_tmp1BF;struct Cyc_Absyn_Datatypefield*_tmp1BE;switch(*((int*)_tmp1B5)){case 0: _LL1B0: _LL1B1:
 goto _LL1AF;case 1: _LL1B2: _LL1B3:
# 744
({void*_tmp1B6=0;Cyc_Binding_err(_tmp208,({const char*_tmp1B7="struct tag used without arguments in pattern";_tag_dyneither(_tmp1B7,sizeof(char),45);}),_tag_dyneither(_tmp1B6,sizeof(void*),0));});
_npop_handler(0);return;case 2: _LL1B4: _tmp1BF=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1B5)->f1;_tmp1BE=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1B5)->f2;_LL1B5:
# 747
 p->r=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8[0]=({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp1B9;_tmp1B9.tag=8;_tmp1B9.f1=_tmp1BF;_tmp1B9.f2=_tmp1BE;_tmp1B9.f3=0;_tmp1B9.f4=0;_tmp1B9;});_tmp1B8;});_npop_handler(0);return;case 3: _LL1B6: _tmp1C1=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp1B5)->f1;_tmp1C0=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp1B5)->f2;_LL1B7:
# 749
 p->r=(void*)({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp1BA=_cycalloc(sizeof(*_tmp1BA));_tmp1BA[0]=({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp1BB;_tmp1BB.tag=13;_tmp1BB.f1=_tmp1C1;_tmp1BB.f2=_tmp1C0;_tmp1BB;});_tmp1BA;});_npop_handler(0);return;default: _LL1B8: _tmp1C3=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp1B5)->f1;_tmp1C2=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp1B5)->f2;_LL1B9:
# 751
 p->r=(void*)({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp1BC=_cycalloc(sizeof(*_tmp1BC));_tmp1BC[0]=({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp1BD;_tmp1BD.tag=14;_tmp1BD.f1=_tmp1C3;_tmp1BD.f2=_tmp1C2;_tmp1BD;});_tmp1BC;});_npop_handler(0);return;}_LL1AF:;}
# 741
;_pop_handler();}else{void*_tmp1B2=(void*)_exn_thrown;void*_tmp1C4=_tmp1B2;void*_tmp1C5;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1C4)->tag == Cyc_Binding_BindingError){_LL1BB: _LL1BC:
# 753
 goto _LL1BA;}else{_LL1BD: _tmp1C5=_tmp1C4;_LL1BE:(int)_rethrow(_tmp1C5);}_LL1BA:;}};}
Cyc_Binding_absolutize_decl(_tmp208,env,_tmp1F3,Cyc_Absyn_Public);{
struct Cyc_Absyn_Vardecl*_tmp1C6=Cyc_Absyn_new_vardecl(_tmp1F3,(void*)& Cyc_Absyn_VoidType_val,0);
p->r=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7[0]=({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp1C8;_tmp1C8.tag=1;_tmp1C8.f1=_tmp1C6;_tmp1C8.f2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);_tmp1C8;});_tmp1C7;});
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),(*_tmp1C6->name).f2,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));_tmp1C9[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp1CA;_tmp1CA.tag=0;_tmp1CA.f1=(void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB[0]=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp1CC;_tmp1CC.tag=5;_tmp1CC.f1=_tmp1C6;_tmp1CC;});_tmp1CB;});_tmp1CA;});_tmp1C9;}));
# 759
return;};case 16: _LL189: _tmp1F6=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1B0)->f1;_tmp1F5=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1B0)->f2;_tmp1F4=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1B0)->f3;_LL18A:
# 762
{struct Cyc_List_List*_tmp1CD=_tmp1F5;for(0;_tmp1CD != 0;_tmp1CD=_tmp1CD->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp1CD->hd);}}
{struct _handler_cons _tmp1CE;_push_handler(& _tmp1CE);{int _tmp1D0=0;if(setjmp(_tmp1CE.handler))_tmp1D0=1;if(!_tmp1D0){
{void*_tmp1D1=Cyc_Binding_lookup_ordinary(_tmp208,env,_tmp1F6);void*_tmp1D2=_tmp1D1;struct Cyc_Absyn_Datatypedecl*_tmp1E0;struct Cyc_Absyn_Datatypefield*_tmp1DF;struct Cyc_Absyn_Aggrdecl*_tmp1DE;switch(*((int*)_tmp1D2)){case 0: _LL1C0: _LL1C1:
 goto _LL1BF;case 1: _LL1C2: _tmp1DE=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmp1D2)->f1;_LL1C3: {
# 768
struct Cyc_List_List*_tmp1D3=0;
for(0;_tmp1F5 != 0;_tmp1F5=_tmp1F5->tl){
_tmp1D3=({struct Cyc_List_List*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->hd=({struct _tuple12*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5->f1=0;_tmp1D5->f2=(struct Cyc_Absyn_Pat*)_tmp1F5->hd;_tmp1D5;});_tmp1D4->tl=_tmp1D3;_tmp1D4;});}
p->r=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6[0]=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp1D7;_tmp1D7.tag=7;_tmp1D7.f1=({struct Cyc_Absyn_AggrInfo*_tmp1D8=_cycalloc(sizeof(*_tmp1D8));_tmp1D8->aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9[0]=_tmp1DE;_tmp1D9;}));_tmp1D8->targs=0;_tmp1D8;});_tmp1D7.f2=0;_tmp1D7.f3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1D3);_tmp1D7.f4=_tmp1F4;_tmp1D7;});_tmp1D6;});
_npop_handler(0);return;}case 2: _LL1C4: _tmp1E0=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1D2)->f1;_tmp1DF=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1D2)->f2;_LL1C5:
# 775
 p->r=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA[0]=({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp1DB;_tmp1DB.tag=8;_tmp1DB.f1=_tmp1E0;_tmp1DB.f2=_tmp1DF;_tmp1DB.f3=_tmp1F5;_tmp1DB.f4=_tmp1F4;_tmp1DB;});_tmp1DA;});_npop_handler(0);return;case 3: _LL1C6: _LL1C7:
 goto _LL1C9;default: _LL1C8: _LL1C9:
# 778
({void*_tmp1DC=0;Cyc_Binding_err(_tmp208,({const char*_tmp1DD="enum tag used with arguments in pattern";_tag_dyneither(_tmp1DD,sizeof(char),40);}),_tag_dyneither(_tmp1DC,sizeof(void*),0));});_npop_handler(0);return;}_LL1BF:;}
# 765
;_pop_handler();}else{void*_tmp1CF=(void*)_exn_thrown;void*_tmp1E1=_tmp1CF;void*_tmp1E2;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1E1)->tag == Cyc_Binding_BindingError){_LL1CB: _LL1CC:
# 780
 goto _LL1CA;}else{_LL1CD: _tmp1E2=_tmp1E1;_LL1CE:(int)_rethrow(_tmp1E2);}_LL1CA:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp1E5;_tmp1E5.tag=0;_tmp1E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp1F6));({void*_tmp1E3[1]={& _tmp1E5};Cyc_Binding_err(_tmp208,({const char*_tmp1E4="%s is not a constructor in pattern";_tag_dyneither(_tmp1E4,sizeof(char),35);}),_tag_dyneither(_tmp1E3,sizeof(void*),1));});});
return;case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1B0)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1B0)->f1)->aggr_info).UnknownAggr).tag == 1){_LL18B: _tmp1FB=((((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1B0)->f1)->aggr_info).UnknownAggr).val).f2;_tmp1FA=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1B0)->f1)->targs;_tmp1F9=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1B0)->f2;_tmp1F8=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1B0)->f3;_tmp1F7=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1B0)->f4;_LL18C:
# 785
{struct Cyc_List_List*_tmp1E6=_tmp1F8;for(0;_tmp1E6 != 0;_tmp1E6=_tmp1E6->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple12*)_tmp1E6->hd)).f2);}}
{struct _handler_cons _tmp1E7;_push_handler(& _tmp1E7);{int _tmp1E9=0;if(setjmp(_tmp1E7.handler))_tmp1E9=1;if(!_tmp1E9){
{struct Cyc_Absyn_Aggrdecl*_tmp1EA=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(_tmp208,env->ns,_tmp1FB,Cyc_Binding_lookup_aggrdecl);
p->r=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB[0]=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp1EC;_tmp1EC.tag=7;_tmp1EC.f1=({struct Cyc_Absyn_AggrInfo*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED->aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE[0]=_tmp1EA;_tmp1EE;}));_tmp1ED->targs=_tmp1FA;_tmp1ED;});_tmp1EC.f2=_tmp1F9;_tmp1EC.f3=_tmp1F8;_tmp1EC.f4=_tmp1F7;_tmp1EC;});_tmp1EB;});}
# 788
;_pop_handler();}else{void*_tmp1E8=(void*)_exn_thrown;void*_tmp1EF=_tmp1E8;void*_tmp1F2;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1EF)->tag == Cyc_Binding_BindingError){_LL1D0: _LL1D1:
# 792
({void*_tmp1F0=0;Cyc_Binding_err(_tmp208,({const char*_tmp1F1="non-aggregate name has designator patterns";_tag_dyneither(_tmp1F1,sizeof(char),43);}),_tag_dyneither(_tmp1F0,sizeof(void*),0));});
return;}else{_LL1D2: _tmp1F2=_tmp1EF;_LL1D3:(int)_rethrow(_tmp1F2);}_LL1CF:;}};}
# 795
return;}else{_LL1A7: _tmp1FC=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1B0)->f3;_LL1A8:
# 824
 for(0;_tmp1FC != 0;_tmp1FC=_tmp1FC->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple12*)_tmp1FC->hd)).f2);}
return;}}else{_LL1A5: _tmp1FD=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1B0)->f3;_LL1A6:
# 822
 _tmp1FC=_tmp1FD;goto _LL1A8;}case 0: _LL18D: _LL18E:
# 797
 return;case 3: _LL18F: _tmp1FF=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1B0)->f1;_tmp1FE=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1B0)->f2;_LL190:
# 799
 _tmp201=_tmp1FF;_tmp200=_tmp1FE;goto _LL192;case 1: _LL191: _tmp201=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1B0)->f1;_tmp200=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1B0)->f2;_LL192:
# 801
 Cyc_Binding_resolve_pat(env,_tmp200);
_tmp202=_tmp201;goto _LL194;case 4: _LL193: _tmp202=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1B0)->f2;_LL194:
 _tmp203=_tmp202;goto _LL196;case 2: _LL195: _tmp203=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1B0)->f2;_LL196:
# 805
 Cyc_Binding_resolve_and_add_vardecl(_tmp208,env,_tmp203);goto _LL186;case 6: _LL197: _tmp204=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1B0)->f1;_LL198:
# 807
 Cyc_Binding_resolve_pat(env,_tmp204);return;case 5: _LL199: _tmp205=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1B0)->f1;_LL19A:
# 809
 for(0;_tmp205 != 0;_tmp205=_tmp205->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp205->hd);}
goto _LL186;case 17: _LL19B: _tmp206=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1B0)->f1;_LL19C:
# 813
 Cyc_Binding_resolve_exp(env,_tmp206);return;case 9: _LL19D: _LL19E:
# 815
 goto _LL1A0;case 10: _LL19F: _LL1A0:
 goto _LL1A2;case 11: _LL1A1: _LL1A2:
 goto _LL1A4;case 12: _LL1A3: _LL1A4:
 return;case 8: _LL1A9: _tmp207=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1B0)->f3;_LL1AA:
# 828
 for(0;_tmp207 != 0;_tmp207=_tmp207->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp207->hd);}
return;case 13: _LL1AB: _LL1AC:
 goto _LL1AE;default: _LL1AD: _LL1AE:
 return;}_LL186:;};}struct _tuple13{void*f1;void*f2;};
# 836
void Cyc_Binding_resolve_rgnpo(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*po){
for(0;po != 0;po=po->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple13*)po->hd)).f1);
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple13*)po->hd)).f2);}}struct _tuple14{unsigned int f1;struct _tuple0*f2;int f3;};
# 850 "binding.cyc"
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Decl*d){
# 853
unsigned int loc=d->loc;
void*_tmp20A=d->r;void*_tmp20B=_tmp20A;struct Cyc_List_List*_tmp2A8;struct Cyc_List_List*_tmp2A7;struct Cyc_List_List*_tmp2A6;struct _tuple0*_tmp2A5;struct Cyc_List_List*_tmp2A4;struct _dyneither_ptr*_tmp2A3;struct Cyc_List_List*_tmp2A2;struct Cyc_Absyn_Datatypedecl*_tmp2A1;struct Cyc_Absyn_Enumdecl*_tmp2A0;struct Cyc_Absyn_Aggrdecl*_tmp29F;struct Cyc_Absyn_Typedefdecl*_tmp29E;struct Cyc_Absyn_Tvar*_tmp29D;struct Cyc_Absyn_Vardecl*_tmp29C;int _tmp29B;struct Cyc_Absyn_Exp*_tmp29A;struct Cyc_List_List*_tmp299;struct Cyc_Absyn_Pat*_tmp298;struct Cyc_Absyn_Exp*_tmp297;struct Cyc_Absyn_Fndecl*_tmp296;struct Cyc_Absyn_Vardecl*_tmp295;switch(*((int*)_tmp20B)){case 0: _LL1D5: _tmp295=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp20B)->f1;_LL1D6:
# 857
 Cyc_Binding_resolve_and_add_vardecl(loc,env,_tmp295);
# 862
if(_tmp295->initializer != 0  && !Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp295->initializer));
goto _LL1D4;case 1: _LL1D7: _tmp296=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp20B)->f1;_LL1D8:
# 867
 Cyc_Binding_absolutize_decl(loc,env,_tmp296->name,_tmp296->sc);{
struct _tuple0*_tmp20C=_tmp296->name;struct _tuple0*_tmp20D=_tmp20C;union Cyc_Absyn_Nmspace _tmp23F;struct _dyneither_ptr*_tmp23E;_LL1F4: _tmp23F=_tmp20D->f1;_tmp23E=_tmp20D->f2;_LL1F5:;{
struct Cyc_Dict_Dict*_tmp20E=env->local_vars;
# 873
env->local_vars=({struct Cyc_Dict_Dict*_tmp20F=_cycalloc(sizeof(*_tmp20F));_tmp20F[0]=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp20F;});{
struct Cyc_List_List*_tmp210=0;
{struct Cyc_List_List*_tmp211=_tmp296->args;for(0;_tmp211 != 0;_tmp211=_tmp211->tl){
struct _tuple8*_tmp212=(struct _tuple8*)_tmp211->hd;struct _tuple8*_tmp213=_tmp212;struct _dyneither_ptr*_tmp21E;struct Cyc_Absyn_Tqual _tmp21D;void*_tmp21C;_LL1F7: _tmp21E=_tmp213->f1;_tmp21D=_tmp213->f2;_tmp21C=_tmp213->f3;_LL1F8:;{
struct Cyc_Absyn_Vardecl*_tmp214=({struct Cyc_Absyn_Vardecl*_tmp21A=_cycalloc(sizeof(*_tmp21A));_tmp21A->sc=Cyc_Absyn_Public;_tmp21A->name=({struct _tuple0*_tmp21B=_cycalloc(sizeof(*_tmp21B));_tmp21B->f1=Cyc_Absyn_Loc_n;_tmp21B->f2=_tmp21E;_tmp21B;});_tmp21A->tq=_tmp21D;_tmp21A->type=_tmp21C;_tmp21A->initializer=0;_tmp21A->rgn=0;_tmp21A->attributes=0;_tmp21A->escapes=0;_tmp21A;});
# 885
_tmp210=({struct Cyc_List_List*_tmp215=_cycalloc(sizeof(*_tmp215));_tmp215->hd=_tmp214;_tmp215->tl=_tmp210;_tmp215;});
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp21E,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp216=_cycalloc(sizeof(*_tmp216));_tmp216[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp217;_tmp217.tag=0;_tmp217.f1=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp218=_cycalloc(sizeof(*_tmp218));_tmp218[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp219;_tmp219.tag=3;_tmp219.f1=_tmp214;_tmp219;});_tmp218;});_tmp217;});_tmp216;}));};}}
# 889
if(_tmp296->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp21F=*((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp296->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp220=_tmp21F;struct _dyneither_ptr*_tmp232;struct Cyc_Absyn_Tqual _tmp231;void*_tmp230;int _tmp22F;_LL1FA: _tmp232=_tmp220.name;_tmp231=_tmp220.tq;_tmp230=_tmp220.type;_tmp22F=_tmp220.inject;_LL1FB:;
if(_tmp232 == 0){
({void*_tmp221=0;Cyc_Binding_err(loc,({const char*_tmp222="missing name for vararg";_tag_dyneither(_tmp222,sizeof(char),24);}),_tag_dyneither(_tmp221,sizeof(void*),0));});
goto _LL1D4;}{
# 896
void*_tmp223=Cyc_Absyn_dyneither_typ(_tmp230,(void*)& Cyc_Absyn_HeapRgn_val,_tmp231,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl*_tmp224=({struct Cyc_Absyn_Vardecl*_tmp22D=_cycalloc(sizeof(*_tmp22D));_tmp22D->sc=Cyc_Absyn_Public;_tmp22D->name=({struct _tuple0*_tmp22E=_cycalloc(sizeof(*_tmp22E));_tmp22E->f1=Cyc_Absyn_Loc_n;_tmp22E->f2=_tmp232;_tmp22E;});_tmp22D->tq=
# 899
Cyc_Absyn_empty_tqual(0);_tmp22D->type=_tmp223;_tmp22D->initializer=0;_tmp22D->rgn=0;_tmp22D->attributes=0;_tmp22D->escapes=0;_tmp22D;});
# 905
_tmp210=({struct Cyc_List_List*_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225->hd=_tmp224;_tmp225->tl=_tmp210;_tmp225;});
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp232,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp226=_cycalloc(sizeof(*_tmp226));_tmp226[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp227;_tmp227.tag=0;_tmp227.f1=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp228=_cycalloc(sizeof(*_tmp228));_tmp228[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp229;_tmp229.tag=3;_tmp229.f1=_tmp224;_tmp229;});_tmp228;});_tmp227;});_tmp226;}));{
# 908
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp22A=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp22B=_cycalloc(sizeof(*_tmp22B));_tmp22B[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp22C;_tmp22C.tag=3;_tmp22C.f1=_tmp224;_tmp22C;});_tmp22B;});;};};}
# 911
_tmp210=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp210);
_tmp296->param_vardecls=({struct Cyc_Core_Opt*_tmp233=_cycalloc(sizeof(*_tmp233));_tmp233->v=_tmp210;_tmp233;});
# 914
{struct Cyc_List_List*_tmp234=_tmp210;for(0;_tmp234 != 0;_tmp234=_tmp234->tl){
Cyc_Binding_resolve_type(loc,env,((struct Cyc_Absyn_Vardecl*)_tmp234->hd)->type);}}
Cyc_Binding_resolve_type(loc,env,_tmp296->ret_type);
if(_tmp296->effect != 0)
Cyc_Binding_resolve_type(loc,env,(void*)_check_null(_tmp296->effect));
Cyc_Binding_resolve_rgnpo(loc,env,_tmp296->rgn_po);
if(_tmp296->requires_clause != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp296->requires_clause));
if(_tmp296->ensures_clause != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp296->ensures_clause));
# 925
if(_tmp20E != 0){
# 927
*_tmp20E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*_tmp20E,_tmp23E,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp235=_cycalloc(sizeof(*_tmp235));_tmp235[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp236;_tmp236.tag=0;_tmp236.f1=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp237=_cycalloc(sizeof(*_tmp237));_tmp237[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp238;_tmp238.tag=2;_tmp238.f1=_tmp296;_tmp238;});_tmp237;});_tmp236;});_tmp235;}));
# 930
env->local_vars=_tmp20E;}else{
# 932
struct Cyc_Binding_ResolveNSEnv*_tmp239=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp23F);
_tmp239->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp239->ordinaries,_tmp23E,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp23A=_cycalloc(sizeof(*_tmp23A));_tmp23A[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp23B;_tmp23B.tag=0;_tmp23B.f1=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp23C=_cycalloc(sizeof(*_tmp23C));_tmp23C[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp23D;_tmp23D.tag=2;_tmp23D.f1=_tmp296;_tmp23D;});_tmp23C;});_tmp23B;});_tmp23A;}));}
# 938
if(!Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_stmt(env,_tmp296->body);
# 941
env->local_vars=_tmp20E;
goto _LL1D4;};};};case 2: _LL1D9: _tmp298=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp20B)->f1;_tmp297=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp20B)->f3;_LL1DA:
# 945
 if(Cyc_Binding_at_toplevel(env)){
({void*_tmp240=0;Cyc_Binding_err(loc,({const char*_tmp241="let not allowed at top-level";_tag_dyneither(_tmp241,sizeof(char),29);}),_tag_dyneither(_tmp240,sizeof(void*),0));});
goto _LL1D4;}
# 949
Cyc_Binding_resolve_exp(env,_tmp297);
Cyc_Binding_resolve_pat(env,_tmp298);
goto _LL1D4;case 3: _LL1DB: _tmp299=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp20B)->f1;_LL1DC:
# 954
 for(0;_tmp299 != 0;_tmp299=_tmp299->tl){
Cyc_Binding_resolve_and_add_vardecl(loc,env,(struct Cyc_Absyn_Vardecl*)_tmp299->hd);}
goto _LL1D4;case 4: _LL1DD: _tmp29D=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp20B)->f1;_tmp29C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp20B)->f2;_tmp29B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp20B)->f3;_tmp29A=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp20B)->f4;_LL1DE:
# 959
 if(_tmp29A != 0)
Cyc_Binding_resolve_exp(env,_tmp29A);
Cyc_Binding_resolve_and_add_vardecl(loc,env,_tmp29C);
goto _LL1D4;case 8: _LL1DF: _tmp29E=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp20B)->f1;_LL1E0:
# 965
 Cyc_Binding_absolutize_decl(loc,env,_tmp29E->name,_tmp29E->extern_c?Cyc_Absyn_ExternC: Cyc_Absyn_Public);{
struct _tuple0*_tmp242=_tmp29E->name;struct _tuple0*_tmp243=_tmp242;union Cyc_Absyn_Nmspace _tmp246;struct _dyneither_ptr*_tmp245;_LL1FD: _tmp246=_tmp243->f1;_tmp245=_tmp243->f2;_LL1FE:;{
# 968
struct Cyc_Binding_ResolveNSEnv*_tmp244=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp246);
if(_tmp29E->defn != 0)
Cyc_Binding_resolve_type(loc,env,(void*)_check_null(_tmp29E->defn));
# 972
_tmp244->typedefs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(_tmp244->typedefs,_tmp245,_tmp29E);
# 974
goto _LL1D4;};};case 5: _LL1E1: _tmp29F=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp20B)->f1;_LL1E2:
# 977
 Cyc_Binding_absolutize_decl(loc,env,_tmp29F->name,_tmp29F->sc);{
struct _tuple0*_tmp247=_tmp29F->name;struct _tuple0*_tmp248=_tmp247;union Cyc_Absyn_Nmspace _tmp251;struct _dyneither_ptr*_tmp250;_LL200: _tmp251=_tmp248->f1;_tmp250=_tmp248->f2;_LL201:;{
# 980
struct Cyc_Binding_ResolveNSEnv*_tmp249=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp251);
# 982
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp249->aggrdecls,_tmp250) && _tmp29F->impl == 0)
goto _LL1D4;
# 985
_tmp249->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp249->aggrdecls,_tmp250,_tmp29F);
# 987
_tmp249->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp249->ordinaries,_tmp250,(void*)({struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_tmp24A=_cycalloc(sizeof(*_tmp24A));_tmp24A[0]=({struct Cyc_Binding_AggrRes_Binding_Resolved_struct _tmp24B;_tmp24B.tag=1;_tmp24B.f1=_tmp29F;_tmp24B;});_tmp24A;}));
# 989
if(_tmp29F->impl != 0){
struct Cyc_Absyn_AggrdeclImpl*_tmp24C=(struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp29F->impl);struct Cyc_Absyn_AggrdeclImpl*_tmp24D=_tmp24C;struct Cyc_List_List*_tmp24F;struct Cyc_List_List*_tmp24E;_LL203: _tmp24F=_tmp24D->rgn_po;_tmp24E=_tmp24D->fields;_LL204:;
Cyc_Binding_resolve_rgnpo(loc,env,_tmp24F);
Cyc_Binding_resolve_aggrfields(loc,env,_tmp24E);}
# 994
goto _LL1D4;};};case 7: _LL1E3: _tmp2A0=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp20B)->f1;_LL1E4:
# 997
 Cyc_Binding_absolutize_decl(loc,env,_tmp2A0->name,_tmp2A0->sc);{
struct _tuple0*_tmp252=_tmp2A0->name;struct _tuple0*_tmp253=_tmp252;union Cyc_Absyn_Nmspace _tmp259;struct _dyneither_ptr*_tmp258;_LL206: _tmp259=_tmp253->f1;_tmp258=_tmp253->f2;_LL207:;{
# 1000
struct Cyc_Binding_ResolveNSEnv*_tmp254=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp259);
# 1002
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp254->enumdecls,_tmp258) && _tmp2A0->fields == 0)
goto _LL1D4;
# 1006
_tmp254->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp254->enumdecls,_tmp258,_tmp2A0);
# 1008
if(_tmp2A0->fields != 0){
# 1010
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2A0->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp255=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Binding_absolutize_decl(_tmp255->loc,env,_tmp255->name,_tmp2A0->sc);
if(_tmp255->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp255->tag));
_tmp254->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp254->ordinaries,(*_tmp255->name).f2,(void*)({struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_tmp256=_cycalloc(sizeof(*_tmp256));_tmp256[0]=({struct Cyc_Binding_EnumRes_Binding_Resolved_struct _tmp257;_tmp257.tag=3;_tmp257.f1=_tmp2A0;_tmp257.f2=_tmp255;_tmp257;});_tmp256;}));}}
# 1019
goto _LL1D4;};};case 6: _LL1E5: _tmp2A1=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp20B)->f1;_LL1E6:
# 1023
{struct _handler_cons _tmp25A;_push_handler(& _tmp25A);{int _tmp25C=0;if(setjmp(_tmp25A.handler))_tmp25C=1;if(!_tmp25C){
{struct Cyc_List_List*_tmp25D=((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp2A1->name,Cyc_Binding_lookup_datatypedecl);
struct Cyc_Absyn_Datatypedecl*_tmp25E=(struct Cyc_Absyn_Datatypedecl*)_tmp25D->hd;
if(!_tmp25E->is_extensible)
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp25F=_cycalloc_atomic(sizeof(*_tmp25F));_tmp25F[0]=({struct Cyc_Binding_BindingError_exn_struct _tmp260;_tmp260.tag=Cyc_Binding_BindingError;_tmp260;});_tmp25F;}));
_tmp2A1->name=_tmp25E->name;
_tmp2A1->is_extensible=1;{
struct _tuple0*_tmp261=_tmp2A1->name;struct _tuple0*_tmp262=_tmp261;union Cyc_Absyn_Nmspace _tmp266;struct _dyneither_ptr*_tmp265;_LL209: _tmp266=_tmp262->f1;_tmp265=_tmp262->f2;_LL20A:;{
struct Cyc_Binding_ResolveNSEnv*_tmp263=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp266);
_tmp263->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp263->datatypedecls,_tmp265,({struct Cyc_List_List*_tmp264=_cycalloc(sizeof(*_tmp264));_tmp264->hd=_tmp2A1;_tmp264->tl=_tmp25D;_tmp264;}));};};}
# 1024
;_pop_handler();}else{void*_tmp25B=(void*)_exn_thrown;void*_tmp267=_tmp25B;void*_tmp26E;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp267)->tag == Cyc_Binding_BindingError){_LL20C: _LL20D:
# 1036
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp2A1->name,_tmp2A1->sc);{
struct _tuple0*_tmp268=_tmp2A1->name;struct _tuple0*_tmp269=_tmp268;union Cyc_Absyn_Nmspace _tmp26D;struct _dyneither_ptr*_tmp26C;_LL211: _tmp26D=_tmp269->f1;_tmp26C=_tmp269->f2;_LL212:;{
struct Cyc_Binding_ResolveNSEnv*_tmp26A=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp26D);
# 1040
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp26A->datatypedecls,_tmp26C) && _tmp2A1->fields == 0)
# 1042
goto _LL20B;
_tmp26A->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp26A->datatypedecls,_tmp26C,({struct Cyc_List_List*_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B->hd=_tmp2A1;_tmp26B->tl=0;_tmp26B;}));
# 1045
goto _LL20B;};};}else{_LL20E: _tmp26E=_tmp267;_LL20F:(int)_rethrow(_tmp26E);}_LL20B:;}};}{
# 1048
struct _tuple0*_tmp26F=_tmp2A1->name;struct _tuple0*_tmp270=_tmp26F;union Cyc_Absyn_Nmspace _tmp27F;struct _dyneither_ptr*_tmp27E;_LL214: _tmp27F=_tmp270->f1;_tmp27E=_tmp270->f2;_LL215:;
if(_tmp2A1->fields != 0){
struct Cyc_List_List*_tmp271=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2A1->fields))->v;for(0;_tmp271 != 0;_tmp271=_tmp271->tl){
struct Cyc_Absyn_Datatypefield*_tmp272=(struct Cyc_Absyn_Datatypefield*)_tmp271->hd;
{struct Cyc_List_List*_tmp273=_tmp272->typs;for(0;_tmp273 != 0;_tmp273=_tmp273->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple11*)_tmp273->hd)).f2);}}
{union Cyc_Absyn_Nmspace _tmp274=(*_tmp272->name).f1;union Cyc_Absyn_Nmspace _tmp275=_tmp274;switch((_tmp275.Abs_n).tag){case 1: if((_tmp275.Rel_n).val == 0){_LL217: _LL218:
# 1056
 if(_tmp2A1->is_extensible)
(*_tmp272->name).f1=Cyc_Absyn_Abs_n((env->ns)->curr_ns,0);else{
# 1059
(*_tmp272->name).f1=(*_tmp2A1->name).f1;}
goto _LL216;}else{_LL219: _LL21A:
# 1062
({void*_tmp276=0;Cyc_Binding_err(_tmp272->loc,({const char*_tmp277="qualified datatype field declaratations are not allowed";_tag_dyneither(_tmp277,sizeof(char),56);}),_tag_dyneither(_tmp276,sizeof(void*),0));});
return;}case 2: _LL21B: _LL21C:
 goto _LL216;default: _LL21D: _LL21E:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp278=_cycalloc(sizeof(*_tmp278));_tmp278[0]=({struct Cyc_Core_Impossible_exn_struct _tmp279;_tmp279.tag=Cyc_Core_Impossible;_tmp279.f1=({const char*_tmp27A="datatype field Loc_n or C_n";_tag_dyneither(_tmp27A,sizeof(char),28);});_tmp279;});_tmp278;}));}_LL216:;}{
# 1067
struct Cyc_Binding_ResolveNSEnv*_tmp27B=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,(*_tmp272->name).f1);
_tmp27B->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp27B->ordinaries,(*_tmp272->name).f2,(void*)({struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C[0]=({struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct _tmp27D;_tmp27D.tag=2;_tmp27D.f1=_tmp2A1;_tmp27D.f2=_tmp272;_tmp27D;});_tmp27C;}));};}}
# 1072
goto _LL1D4;};case 9: _LL1E7: _tmp2A3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp20B)->f1;_tmp2A2=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp20B)->f2;_LL1E8:
# 1075
((void(*)(struct Cyc_Binding_NSCtxt*ctxt,struct _dyneither_ptr*subname,int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_enter_ns)(env->ns,_tmp2A3,1,Cyc_Binding_mt_renv);
Cyc_Binding_resolve_decls(env,_tmp2A2);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_ns)(env->ns);
goto _LL1D4;case 10: _LL1E9: _tmp2A5=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp20B)->f1;_tmp2A4=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp20B)->f2;_LL1EA:
# 1080
((void(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename))Cyc_Binding_enter_using)(d->loc,env->ns,_tmp2A5);
Cyc_Binding_resolve_decls(env,_tmp2A4);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_using)(env->ns);
goto _LL1D4;case 11: _LL1EB: _tmp2A6=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp20B)->f1;_LL1EC: {
# 1086
int _tmp280=env->in_cinclude;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,_tmp2A6);
env->in_cinclude=_tmp280;
goto _LL1D4;}case 12: _LL1ED: _tmp2A8=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp20B)->f1;_tmp2A7=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp20B)->f2;_LL1EE:
# 1097
{struct Cyc_List_List*_tmp281=_tmp2A7;for(0;_tmp281 != 0;_tmp281=_tmp281->tl){
struct _tuple14*_tmp282=(struct _tuple14*)_tmp281->hd;struct _tuple14*_tmp283=_tmp282;unsigned int _tmp285;struct _tuple0*_tmp284;_LL220: _tmp285=_tmp283->f1;_tmp284=_tmp283->f2;_LL221:;
Cyc_Binding_absolutize_decl(_tmp285,env,_tmp284,Cyc_Absyn_ExternC);}}
# 1105
if(!Cyc_Binding_at_toplevel(env)){
({void*_tmp286=0;Cyc_Binding_err(loc,({const char*_tmp287="extern \"C include\" not at toplevel";_tag_dyneither(_tmp287,sizeof(char),35);}),_tag_dyneither(_tmp286,sizeof(void*),0));});
goto _LL1D4;}{
# 1110
struct Cyc_Binding_ResolveNSEnv*_tmp288=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)((env->ns)->ns_data,(env->ns)->curr_ns);
struct Cyc_Dict_Dict _tmp289=_tmp288->ordinaries;
int _tmp28A=env->in_cinclude;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,_tmp2A8);{
struct Cyc_Dict_Dict _tmp28B=_tmp288->ordinaries;
struct Cyc_Dict_Dict _tmp28C=_tmp289;
{struct Cyc_List_List*_tmp28D=_tmp2A7;for(0;_tmp28D != 0;_tmp28D=_tmp28D->tl){
struct _tuple14*_tmp28E=(struct _tuple14*)_tmp28D->hd;struct _tuple14*_tmp28F=_tmp28E;unsigned int _tmp294;struct _dyneither_ptr*_tmp293;_LL223: _tmp294=_tmp28F->f1;_tmp293=(_tmp28F->f2)->f2;_LL224:;
if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp28B,_tmp293) || 
((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp289,_tmp293) && 
((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp289,_tmp293)== ((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp28B,_tmp293))
({struct Cyc_String_pa_PrintArg_struct _tmp292;_tmp292.tag=0;_tmp292.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp293);({void*_tmp290[1]={& _tmp292};Cyc_Binding_err(_tmp294,({const char*_tmp291="%s is exported but not defined";_tag_dyneither(_tmp291,sizeof(char),31);}),_tag_dyneither(_tmp290,sizeof(void*),1));});});
_tmp28C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp28C,_tmp293,((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp28B,_tmp293));}}
# 1125
_tmp288->ordinaries=_tmp28C;
env->in_cinclude=_tmp28A;
goto _LL1D4;};};case 13: _LL1EF: _LL1F0:
# 1129
 goto _LL1F2;default: _LL1F1: _LL1F2:
 goto _LL1D4;}_LL1D4:;}
# 1134
void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*env,struct Cyc_List_List*tds){
for(0;tds != 0;tds=tds->tl){
Cyc_Binding_resolve_decl(env,(struct Cyc_Absyn_Decl*)tds->hd);}}
# 1139
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds){
struct Cyc_Binding_Env*_tmp2A9=({struct Cyc_Binding_Env*_tmp2AD=_cycalloc(sizeof(*_tmp2AD));_tmp2AD->in_cinclude=0;_tmp2AD->ns=((struct Cyc_Binding_NSCtxt*(*)(int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_mt_nsctxt)(1,Cyc_Binding_mt_renv);_tmp2AD->local_vars=0;_tmp2AD;});
Cyc_Binding_resolve_decl(_tmp2A9,({struct Cyc_Absyn_Decl*_tmp2AA=_cycalloc(sizeof(*_tmp2AA));_tmp2AA->r=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB[0]=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp2AC;_tmp2AC.tag=6;_tmp2AC.f1=Cyc_Absyn_exn_tud;_tmp2AC;});_tmp2AB;});_tmp2AA->loc=0;_tmp2AA;}));
Cyc_Binding_resolve_decls(_tmp2A9,tds);}
