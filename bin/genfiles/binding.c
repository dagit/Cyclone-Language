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
void Cyc_Binding_resolve_function_stuff(unsigned int,struct Cyc_Binding_Env*,void*,void*,struct Cyc_List_List*,struct Cyc_Absyn_VarargInfo*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 272
struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(struct _dyneither_ptr*(*f)(void*),int,unsigned int,struct Cyc_Binding_Env*,struct Cyc_List_List*,struct Cyc_Absyn_VarargInfo*);
# 278
void Cyc_Binding_absolutize_decl(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
union Cyc_Absyn_Nmspace _tmp6F=(*qv).f1;union Cyc_Absyn_Nmspace _tmp70=_tmp6F;switch((_tmp70.Abs_n).tag){case 1: if((_tmp70.Rel_n).val == 0){_LL4F: _LL50:
# 281
 if(Cyc_Binding_at_toplevel(env))
(*qv).f1=Cyc_Absyn_Abs_n((env->ns)->curr_ns,Cyc_Binding_in_cinclude(env) || sc == Cyc_Absyn_ExternC);else{
# 284
(*qv).f1=Cyc_Absyn_Loc_n;}
goto _LL4E;}else{goto _LL55;}case 4: _LL51: _LL52:
 goto _LL4E;case 2: _LL53: _LL54:
# 288
 if(!Cyc_Binding_at_toplevel(env))
goto _LL56;
goto _LL4E;default: _LL55: _LL56:
# 292
({struct Cyc_String_pa_PrintArg_struct _tmp73;_tmp73.tag=0;_tmp73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(qv));({void*_tmp71[1]={& _tmp73};Cyc_Binding_err(loc,({const char*_tmp72="qualified names in declarations unimplemented (%s)";_tag_dyneither(_tmp72,sizeof(char),51);}),_tag_dyneither(_tmp71,sizeof(void*),1));});});}_LL4E:;}
# 299
void Cyc_Binding_absolutize_topdecl(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
struct Cyc_Dict_Dict*_tmp74=env->local_vars;
env->local_vars=0;
Cyc_Binding_absolutize_decl(loc,env,qv,sc);
env->local_vars=_tmp74;}
# 306
void Cyc_Binding_resolve_and_add_vardecl(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_Vardecl*vd){
Cyc_Binding_absolutize_decl(loc,env,vd->name,vd->sc);{
struct _tuple0*_tmp75=vd->name;struct _tuple0*_tmp76=_tmp75;union Cyc_Absyn_Nmspace _tmp81;struct _dyneither_ptr*_tmp80;_LL58: _tmp81=_tmp76->f1;_tmp80=_tmp76->f2;_LL59:;
Cyc_Binding_resolve_type(loc,env,vd->type);
if(!Cyc_Binding_at_toplevel(env))
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp80,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp78;_tmp78.tag=0;_tmp78.f1=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp7A;_tmp7A.tag=4;_tmp7A.f1=vd;_tmp7A;});_tmp79;});_tmp78;});_tmp77;}));else{
# 314
struct Cyc_Binding_ResolveNSEnv*_tmp7B=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp81);
_tmp7B->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp7B->ordinaries,_tmp80,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp7D;_tmp7D.tag=0;_tmp7D.f1=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E[0]=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp7F;_tmp7F.tag=1;_tmp7F.f1=vd;_tmp7F;});_tmp7E;});_tmp7D;});_tmp7C;}));}};}
# 321
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Stmt*s){
void*_tmp82=s->r;void*_tmp83=_tmp82;struct Cyc_Absyn_Stmt*_tmp9C;struct Cyc_List_List*_tmp9B;struct Cyc_Absyn_Exp*_tmp9A;struct Cyc_List_List*_tmp99;struct Cyc_Absyn_Decl*_tmp98;struct Cyc_Absyn_Stmt*_tmp97;struct Cyc_Absyn_Exp*_tmp96;struct Cyc_Absyn_Stmt*_tmp95;struct Cyc_List_List*_tmp94;struct Cyc_Absyn_Exp*_tmp93;struct Cyc_Absyn_Exp*_tmp92;struct Cyc_Absyn_Exp*_tmp91;struct Cyc_Absyn_Stmt*_tmp90;struct Cyc_Absyn_Exp*_tmp8F;struct Cyc_Absyn_Stmt*_tmp8E;struct Cyc_Absyn_Stmt*_tmp8D;struct Cyc_Absyn_Exp*_tmp8C;struct Cyc_Absyn_Exp*_tmp8B;struct Cyc_Absyn_Stmt*_tmp8A;struct Cyc_Absyn_Stmt*_tmp89;struct Cyc_Absyn_Exp*_tmp88;struct Cyc_Absyn_Stmt*_tmp87;struct Cyc_Absyn_Stmt*_tmp86;struct Cyc_Absyn_Exp*_tmp85;switch(*((int*)_tmp83)){case 0: _LL5B: _LL5C:
 return;case 1: _LL5D: _tmp85=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp83)->f1;_LL5E:
 Cyc_Binding_resolve_exp(env,_tmp85);return;case 2: _LL5F: _tmp87=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp83)->f1;_tmp86=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp83)->f2;_LL60:
 Cyc_Binding_resolve_stmt(env,_tmp87);Cyc_Binding_resolve_stmt(env,_tmp86);return;case 3: _LL61: _tmp88=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp83)->f1;_LL62:
 if((unsigned int)_tmp88)Cyc_Binding_resolve_exp(env,_tmp88);return;case 4: _LL63: _tmp8B=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp83)->f1;_tmp8A=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp83)->f2;_tmp89=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp83)->f3;_LL64:
# 328
 Cyc_Binding_resolve_exp(env,_tmp8B);Cyc_Binding_resolve_stmt(env,_tmp8A);Cyc_Binding_resolve_stmt(env,_tmp89);return;case 14: _LL65: _tmp8D=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp83)->f1;_tmp8C=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp83)->f2).f1;_LL66:
 _tmp8F=_tmp8C;_tmp8E=_tmp8D;goto _LL68;case 5: _LL67: _tmp8F=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp83)->f1).f1;_tmp8E=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp83)->f2;_LL68:
 Cyc_Binding_resolve_exp(env,_tmp8F);Cyc_Binding_resolve_stmt(env,_tmp8E);return;case 9: _LL69: _tmp93=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp83)->f1;_tmp92=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp83)->f2).f1;_tmp91=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp83)->f3).f1;_tmp90=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp83)->f4;_LL6A:
# 332
 Cyc_Binding_resolve_exp(env,_tmp93);Cyc_Binding_resolve_exp(env,_tmp92);Cyc_Binding_resolve_exp(env,_tmp91);
Cyc_Binding_resolve_stmt(env,_tmp90);
return;case 6: _LL6B: _LL6C:
 goto _LL6E;case 7: _LL6D: _LL6E:
 goto _LL70;case 8: _LL6F: _LL70:
 return;case 11: _LL71: _tmp94=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp83)->f1;_LL72:
# 339
 for(0;_tmp94 != 0;_tmp94=_tmp94->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmp94->hd);}
return;case 13: _LL73: _tmp95=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp83)->f2;_LL74:
 Cyc_Binding_resolve_stmt(env,_tmp95);return;case 16: _LL75: _tmp96=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp83)->f1;_LL76:
 Cyc_Binding_resolve_exp(env,_tmp96);return;case 12: _LL77: _tmp98=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp83)->f1;_tmp97=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp83)->f2;_LL78: {
# 346
struct Cyc_Dict_Dict _tmp84=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
Cyc_Binding_resolve_decl(env,_tmp98);
Cyc_Binding_resolve_stmt(env,_tmp97);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp84;
return;}case 10: _LL79: _tmp9A=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp83)->f1;_tmp99=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp83)->f2;_LL7A:
# 353
 Cyc_Binding_resolve_exp(env,_tmp9A);Cyc_Binding_resolve_scs(env,_tmp99);return;default: _LL7B: _tmp9C=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp83)->f1;_tmp9B=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp83)->f2;_LL7C:
# 355
 Cyc_Binding_resolve_stmt(env,_tmp9C);Cyc_Binding_resolve_scs(env,_tmp9B);return;}_LL5A:;}struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 358
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){
void*_tmp9D=e->r;void*_tmp9E=_tmp9D;void*_tmp12B;struct Cyc_List_List*_tmp12A;struct Cyc_List_List*_tmp129;struct Cyc_Absyn_Datatypedecl*_tmp128;struct Cyc_Absyn_Datatypefield*_tmp127;struct Cyc_Absyn_Enumdecl*_tmp126;struct Cyc_Absyn_Enumfield*_tmp125;void*_tmp124;struct Cyc_Absyn_Enumfield*_tmp123;struct Cyc_List_List*_tmp122;void*_tmp121;void*_tmp120;void*_tmp11F;struct Cyc_Absyn_Stmt*_tmp11E;void*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_List_List*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp114;void**_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*_tmpFF;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_List_List*_tmpFC;struct Cyc_List_List*_tmpFB;struct Cyc_Absyn_Exp*_tmpFA;void*_tmpF9;struct Cyc_List_List*_tmpF8;struct Cyc_Absyn_Vardecl*_tmpF7;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_Absyn_Exp*_tmpF5;struct _tuple0**_tmpF4;struct Cyc_List_List*_tmpF3;struct Cyc_List_List*_tmpF2;struct Cyc_Absyn_Aggrdecl**_tmpF1;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_List_List*_tmpEF;int*_tmpEE;void**_tmpED;switch(*((int*)_tmp9E)){case 1: _LL7E: _tmpED=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_LL7F: {
# 361
void*_tmp9F=*_tmpED;void*_tmpA0=_tmp9F;struct _tuple0*_tmpBC;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpA0)->tag == 0){_LLCF: _tmpBC=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpA0)->f1;_LLD0: {
# 363
struct _handler_cons _tmpA1;_push_handler(& _tmpA1);{int _tmpA3=0;if(setjmp(_tmpA1.handler))_tmpA3=1;if(!_tmpA3){
{void*_tmpA4=Cyc_Binding_lookup_ordinary(e->loc,env,_tmpBC);void*_tmpA5=_tmpA4;void*_tmpB6;struct Cyc_Absyn_Enumfield*_tmpB5;struct Cyc_Absyn_Enumdecl*_tmpB4;struct Cyc_Absyn_Enumfield*_tmpB3;struct Cyc_Absyn_Datatypedecl*_tmpB2;struct Cyc_Absyn_Datatypefield*_tmpB1;struct Cyc_Absyn_Aggrdecl*_tmpB0;void*_tmpAF;switch(*((int*)_tmpA5)){case 0: _LLD4: _tmpAF=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpA5)->f1;_LLD5:
# 366
*_tmpED=_tmpAF;_npop_handler(0);return;case 1: _LLD6: _tmpB0=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpA5)->f1;_LLD7:
# 368
({struct Cyc_String_pa_PrintArg_struct _tmpA8;_tmpA8.tag=0;_tmpA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBC));({void*_tmpA6[1]={& _tmpA8};Cyc_Binding_err(e->loc,({const char*_tmpA7="bad occurrence of type name %s";_tag_dyneither(_tmpA7,sizeof(char),31);}),_tag_dyneither(_tmpA6,sizeof(void*),1));});});
_npop_handler(0);return;case 2: _LLD8: _tmpB2=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpA5)->f1;_tmpB1=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpA5)->f2;_LLD9:
# 371
 e->r=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpAA;_tmpAA.tag=30;_tmpAA.f1=0;_tmpAA.f2=_tmpB2;_tmpAA.f3=_tmpB1;_tmpAA;});_tmpA9;});_npop_handler(0);return;case 3: _LLDA: _tmpB4=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpA5)->f1;_tmpB3=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpA5)->f2;_LLDB:
# 373
 e->r=(void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpAC;_tmpAC.tag=31;_tmpAC.f1=_tmpB4;_tmpAC.f2=_tmpB3;_tmpAC;});_tmpAB;});_npop_handler(0);return;default: _LLDC: _tmpB6=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpA5)->f1;_tmpB5=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpA5)->f2;_LLDD:
# 375
 e->r=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpAE;_tmpAE.tag=32;_tmpAE.f1=_tmpB6;_tmpAE.f2=_tmpB5;_tmpAE;});_tmpAD;});_npop_handler(0);return;}_LLD3:;}
# 364
;_pop_handler();}else{void*_tmpA2=(void*)_exn_thrown;void*_tmpB7=_tmpA2;void*_tmpBB;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpB7)->tag == Cyc_Binding_BindingError){_LLDF: _LLE0:
# 378
({struct Cyc_String_pa_PrintArg_struct _tmpBA;_tmpBA.tag=0;_tmpBA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBC));({void*_tmpB8[1]={& _tmpBA};Cyc_Binding_err(e->loc,({const char*_tmpB9="undeclared identifier %s";_tag_dyneither(_tmpB9,sizeof(char),25);}),_tag_dyneither(_tmpB8,sizeof(void*),1));});});
return;}else{_LLE1: _tmpBB=_tmpB7;_LLE2:(int)_rethrow(_tmpBB);}_LLDE:;}};}}else{_LLD1: _LLD2:
# 381
 return;}_LLCE:;}case 9: _LL80: _tmpF0=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_tmpEF=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp9E)->f2;_tmpEE=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp9E)->f4;_LL81:
# 384
*_tmpEE=1;
# 386
{struct Cyc_List_List*_tmpBD=_tmpEF;for(0;_tmpBD != 0;_tmpBD=_tmpBD->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmpBD->hd);}}{
void*_tmpBE=_tmpF0->r;void*_tmpBF=_tmpBE;void**_tmpDE;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBF)->tag == 1){_LLE4: _tmpDE=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBF)->f1;_LLE5: {
# 390
void*_tmpC0=*_tmpDE;void*_tmpC1=_tmpC0;struct _tuple0*_tmpDD;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpC1)->tag == 0){_LLE9: _tmpDD=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpC1)->f1;_LLEA: {
# 392
struct _handler_cons _tmpC2;_push_handler(& _tmpC2);{int _tmpC4=0;if(setjmp(_tmpC2.handler))_tmpC4=1;if(!_tmpC4){
{void*_tmpC5=Cyc_Binding_lookup_ordinary(_tmpF0->loc,env,_tmpDD);void*_tmpC6=_tmpC5;struct Cyc_Absyn_Datatypedecl*_tmpD7;struct Cyc_Absyn_Datatypefield*_tmpD6;struct Cyc_Absyn_Aggrdecl*_tmpD5;void*_tmpD4;switch(*((int*)_tmpC6)){case 0: _LLEE: _tmpD4=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpC6)->f1;_LLEF:
# 395
*_tmpDE=_tmpD4;_npop_handler(0);return;case 1: _LLF0: _tmpD5=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpC6)->f1;_LLF1: {
# 397
struct Cyc_List_List*_tmpC7=0;
for(0;_tmpEF != 0;_tmpEF=_tmpEF->tl){
_tmpC7=({struct Cyc_List_List*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8->hd=({struct _tuple10*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->f1=0;_tmpC9->f2=(struct Cyc_Absyn_Exp*)_tmpEF->hd;_tmpC9;});_tmpC8->tl=_tmpC7;_tmpC8;});}
e->r=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA[0]=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpCB;_tmpCB.tag=28;_tmpCB.f1=_tmpD5->name;_tmpCB.f2=0;_tmpCB.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpC7);_tmpCB.f4=_tmpD5;_tmpCB;});_tmpCA;});
_npop_handler(0);return;}case 2: _LLF2: _tmpD7=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpC6)->f1;_tmpD6=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpC6)->f2;_LLF3:
# 403
 if(_tmpD6->typs == 0)
# 405
({struct Cyc_String_pa_PrintArg_struct _tmpCE;_tmpCE.tag=0;_tmpCE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpD6->name));({void*_tmpCC[1]={& _tmpCE};Cyc_Binding_err(e->loc,({const char*_tmpCD="%s is a constant, not a function";_tag_dyneither(_tmpCD,sizeof(char),33);}),_tag_dyneither(_tmpCC,sizeof(void*),1));});});
e->r=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpD0;_tmpD0.tag=30;_tmpD0.f1=_tmpEF;_tmpD0.f2=_tmpD7;_tmpD0.f3=_tmpD6;_tmpD0;});_tmpCF;});
_npop_handler(0);return;case 4: _LLF4: _LLF5:
 goto _LLF7;default: _LLF6: _LLF7:
# 411
({struct Cyc_String_pa_PrintArg_struct _tmpD3;_tmpD3.tag=0;_tmpD3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpDD));({void*_tmpD1[1]={& _tmpD3};Cyc_Binding_err(e->loc,({const char*_tmpD2="%s is an enum constructor, not a function";_tag_dyneither(_tmpD2,sizeof(char),42);}),_tag_dyneither(_tmpD1,sizeof(void*),1));});});
_npop_handler(0);return;}_LLED:;}
# 393
;_pop_handler();}else{void*_tmpC3=(void*)_exn_thrown;void*_tmpD8=_tmpC3;void*_tmpDC;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpD8)->tag == Cyc_Binding_BindingError){_LLF9: _LLFA:
# 416
({struct Cyc_String_pa_PrintArg_struct _tmpDB;_tmpDB.tag=0;_tmpDB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpDD));({void*_tmpD9[1]={& _tmpDB};Cyc_Binding_err(e->loc,({const char*_tmpDA="undeclared identifier %s";_tag_dyneither(_tmpDA,sizeof(char),25);}),_tag_dyneither(_tmpD9,sizeof(void*),1));});});
return;}else{_LLFB: _tmpDC=_tmpD8;_LLFC:(int)_rethrow(_tmpDC);}_LLF8:;}};}}else{_LLEB: _LLEC:
# 419
 return;}_LLE8:;}}else{_LLE6: _LLE7:
# 421
 Cyc_Binding_resolve_exp(env,_tmpF0);return;}_LLE3:;};case 28: _LL82: _tmpF4=(struct _tuple0**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_tmpF3=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp9E)->f2;_tmpF2=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp9E)->f3;_tmpF1=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp9E)->f4;_LL83:
# 425
 for(0;_tmpF2 != 0;_tmpF2=_tmpF2->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmpF2->hd)).f2);}{
struct _handler_cons _tmpDF;_push_handler(& _tmpDF);{int _tmpE1=0;if(setjmp(_tmpDF.handler))_tmpE1=1;if(!_tmpE1){
*_tmpF1=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(e->loc,env->ns,*_tmpF4,Cyc_Binding_lookup_aggrdecl);
*_tmpF4=((struct Cyc_Absyn_Aggrdecl*)_check_null(*_tmpF1))->name;
_npop_handler(0);return;
# 428
;_pop_handler();}else{void*_tmpE0=(void*)_exn_thrown;void*_tmpE2=_tmpE0;void*_tmpE6;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpE2)->tag == Cyc_Binding_BindingError){_LLFE: _LLFF:
# 432
({struct Cyc_String_pa_PrintArg_struct _tmpE5;_tmpE5.tag=0;_tmpE5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*_tmpF4));({void*_tmpE3[1]={& _tmpE5};Cyc_Binding_err(e->loc,({const char*_tmpE4="unbound struct/union name %s";_tag_dyneither(_tmpE4,sizeof(char),29);}),_tag_dyneither(_tmpE3,sizeof(void*),1));});});
return;}else{_LL100: _tmpE6=_tmpE2;_LL101:(int)_rethrow(_tmpE6);}_LLFD:;}};};case 26: _LL84: _tmpF7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_tmpF6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp9E)->f2;_tmpF5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp9E)->f3;_LL85:
# 437
 Cyc_Binding_resolve_exp(env,_tmpF6);
if(Cyc_Binding_at_toplevel(env)){
env->local_vars=({struct Cyc_Dict_Dict*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7[0]=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmpE7;});
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,_tmpF7);
Cyc_Binding_resolve_exp(env,_tmpF5);
env->local_vars=0;
return;}{
# 445
struct Cyc_Dict_Dict _tmpE8=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,_tmpF7);
Cyc_Binding_resolve_exp(env,_tmpF5);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmpE8;
return;};case 35: _LL86: _tmpF8=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp9E)->f2;_LL87:
# 453
 for(0;_tmpF8 != 0;_tmpF8=_tmpF8->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmpF8->hd)).f2);}
return;case 27: _LL88: _tmpFA=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_tmpF9=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp9E)->f2;_LL89:
# 459
 Cyc_Binding_resolve_exp(env,_tmpFA);
Cyc_Binding_resolve_type(e->loc,env,_tmpF9);
return;case 0: _LL8A: _LL8B:
# 463
 return;case 23: _LL8C: _tmpFB=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_LL8D:
# 465
 _tmpFC=_tmpFB;goto _LL8F;case 2: _LL8E: _tmpFC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp9E)->f2;_LL8F:
# 467
 for(0;_tmpFC != 0;_tmpFC=_tmpFC->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmpFC->hd);}
return;case 37: _LL90: _tmpFD=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_LL91:
# 471
 _tmpFE=_tmpFD;goto _LL93;case 11: _LL92: _tmpFE=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_LL93:
 _tmpFF=_tmpFE;goto _LL95;case 17: _LL94: _tmpFF=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_LL95:
 _tmp100=_tmpFF;goto _LL97;case 10: _LL96: _tmp100=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_LL97:
 _tmp101=_tmp100;goto _LL99;case 4: _LL98: _tmp101=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_LL99:
 _tmp102=_tmp101;goto _LL9B;case 21: _LL9A: _tmp102=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_LL9B:
 _tmp103=_tmp102;goto _LL9D;case 20: _LL9C: _tmp103=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_LL9D:
 _tmp104=_tmp103;goto _LL9F;case 14: _LL9E: _tmp104=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_LL9F:
 _tmp105=_tmp104;goto _LLA1;case 19: _LLA0: _tmp105=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_LLA1:
 Cyc_Binding_resolve_exp(env,_tmp105);return;case 34: _LLA2: _tmp107=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_tmp106=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp9E)->f2;_LLA3:
# 481
 _tmp109=_tmp107;_tmp108=_tmp106;goto _LLA5;case 8: _LLA4: _tmp109=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_tmp108=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp9E)->f2;_LLA5:
 _tmp10B=_tmp109;_tmp10A=_tmp108;goto _LLA7;case 3: _LLA6: _tmp10B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_tmp10A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp9E)->f3;_LLA7:
 _tmp10D=_tmp10B;_tmp10C=_tmp10A;goto _LLA9;case 22: _LLA8: _tmp10D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_tmp10C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp9E)->f2;_LLA9:
 _tmp10F=_tmp10D;_tmp10E=_tmp10C;goto _LLAB;case 6: _LLAA: _tmp10F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_tmp10E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp9E)->f2;_LLAB:
 _tmp111=_tmp10F;_tmp110=_tmp10E;goto _LLAD;case 7: _LLAC: _tmp111=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_tmp110=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp9E)->f2;_LLAD:
 Cyc_Binding_resolve_exp(env,_tmp111);Cyc_Binding_resolve_exp(env,_tmp110);return;case 33: _LLAE: _tmp114=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp9E)->f1).rgn;_tmp113=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp9E)->f1).elt_type;_tmp112=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp9E)->f1).num_elts;_LLAF:
# 489
 if(_tmp114 != 0)Cyc_Binding_resolve_exp(env,_tmp114);
if(_tmp113 != 0)Cyc_Binding_resolve_type(e->loc,env,*_tmp113);
Cyc_Binding_resolve_exp(env,_tmp112);
return;case 15: _LLB0: _tmp116=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_tmp115=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp9E)->f2;_LLB1:
# 495
 if(_tmp116 != 0)Cyc_Binding_resolve_exp(env,_tmp116);
Cyc_Binding_resolve_exp(env,_tmp115);
return;case 5: _LLB2: _tmp119=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_tmp118=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9E)->f2;_tmp117=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9E)->f3;_LLB3:
# 500
 Cyc_Binding_resolve_exp(env,_tmp119);Cyc_Binding_resolve_exp(env,_tmp118);Cyc_Binding_resolve_exp(env,_tmp117);return;case 12: _LLB4: _tmp11B=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_tmp11A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp9E)->f2;_LLB5:
# 503
 Cyc_Binding_resolve_exp(env,_tmp11B);
for(0;_tmp11A != 0;_tmp11A=_tmp11A->tl){
Cyc_Binding_resolve_type(e->loc,env,(void*)_tmp11A->hd);}
return;case 13: _LLB6: _tmp11D=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_tmp11C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9E)->f2;_LLB7:
# 509
 Cyc_Binding_resolve_exp(env,_tmp11C);Cyc_Binding_resolve_type(e->loc,env,_tmp11D);return;case 36: _LLB8: _tmp11E=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_LLB9:
# 511
 Cyc_Binding_resolve_stmt(env,_tmp11E);return;case 38: _LLBA: _tmp11F=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_LLBB:
# 513
 _tmp120=_tmp11F;goto _LLBD;case 18: _LLBC: _tmp120=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_LLBD:
 _tmp121=_tmp120;goto _LLBF;case 16: _LLBE: _tmp121=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_LLBF:
 Cyc_Binding_resolve_type(e->loc,env,_tmp121);return;case 25: _LLC0: _tmp122=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_LLC1:
# 518
 for(0;_tmp122 != 0;_tmp122=_tmp122->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp122->hd)).f2);}
return;case 24: _LLC2: _LLC3:
# 523
({void*_tmpE9=0;Cyc_Binding_err(e->loc,({const char*_tmpEA="binding: compound literals unimplemented";_tag_dyneither(_tmpEA,sizeof(char),41);}),_tag_dyneither(_tmpE9,sizeof(void*),0));});return;case 32: _LLC4: _tmp124=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_tmp123=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp9E)->f2;_LLC5:
# 526
 Cyc_Binding_resolve_type(e->loc,env,_tmp124);return;case 31: _LLC6: _tmp126=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_tmp125=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp9E)->f2;_LLC7:
 return;case 30: _LLC8: _tmp129=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_tmp128=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp9E)->f2;_tmp127=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp9E)->f3;_LLC9:
# 529
 for(0;_tmp129 != 0;_tmp129=_tmp129->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmp129->hd);}
return;case 29: _LLCA: _tmp12B=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp9E)->f1;_tmp12A=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp9E)->f2;_LLCB:
# 533
 Cyc_Binding_resolve_type(e->loc,env,_tmp12B);
for(0;_tmp12A != 0;_tmp12A=_tmp12A->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp12A->hd)).f2);}
return;default: _LLCC: _LLCD:
# 539
({void*_tmpEB=0;Cyc_Binding_err(e->loc,({const char*_tmpEC="asm expressions cannot occur within Cyclone code";_tag_dyneither(_tmpEC,sizeof(char),49);}),_tag_dyneither(_tmpEB,sizeof(void*),0));});}_LL7D:;}
# 543
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*env,struct Cyc_List_List*scs){
struct Cyc_Dict_Dict _tmp12C=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp12D=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp12E=_tmp12D;struct Cyc_Absyn_Pat*_tmp131;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Stmt*_tmp12F;_LL103: _tmp131=_tmp12E->pattern;_tmp130=_tmp12E->where_clause;_tmp12F=_tmp12E->body;_LL104:;
Cyc_Binding_resolve_pat(env,_tmp131);
if(_tmp130 != 0)
Cyc_Binding_resolve_exp(env,_tmp130);
Cyc_Binding_resolve_stmt(env,_tmp12F);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp12C;}
# 553
return;}
# 555
void Cyc_Binding_resolve_aggrfields(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*fs){
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp132=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_tmp133=_tmp132;struct _dyneither_ptr*_tmp139;struct Cyc_Absyn_Tqual _tmp138;void*_tmp137;struct Cyc_Absyn_Exp*_tmp136;struct Cyc_List_List*_tmp135;struct Cyc_Absyn_Exp*_tmp134;_LL106: _tmp139=_tmp133->name;_tmp138=_tmp133->tq;_tmp137=_tmp133->type;_tmp136=_tmp133->width;_tmp135=_tmp133->attributes;_tmp134=_tmp133->requires_clause;_LL107:;
Cyc_Binding_resolve_type(loc,env,_tmp137);
if(_tmp136 != 0)
Cyc_Binding_resolve_exp(env,_tmp136);
if(_tmp134 != 0)
Cyc_Binding_resolve_exp(env,_tmp134);}
# 564
return;}struct _tuple11{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 567
struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(struct _dyneither_ptr*(*f)(void*),int need_va_name,unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai){
# 573
struct Cyc_List_List*_tmp13A=0;
for(0;args != 0;args=args->tl){
struct _tuple11*_tmp13B=(struct _tuple11*)args->hd;struct _tuple11*_tmp13C=_tmp13B;void*_tmp143;struct Cyc_Absyn_Tqual _tmp142;void*_tmp141;_LL109: _tmp143=_tmp13C->f1;_tmp142=_tmp13C->f2;_tmp141=_tmp13C->f3;_LL10A:;{
struct _dyneither_ptr*_tmp13D=f(_tmp143);
if(_tmp13D == 0)continue;{
struct Cyc_Absyn_Vardecl*_tmp13E=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140->f1=Cyc_Absyn_Loc_n;_tmp140->f2=_tmp13D;_tmp140;}),_tmp141,0);
_tmp13E->tq=_tmp142;
_tmp13A=({struct Cyc_List_List*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F->hd=_tmp13E;_tmp13F->tl=_tmp13A;_tmp13F;});};};}
# 582
if(vai != 0){
struct Cyc_Absyn_VarargInfo _tmp144=*vai;struct Cyc_Absyn_VarargInfo _tmp145=_tmp144;struct _dyneither_ptr*_tmp14F;struct Cyc_Absyn_Tqual _tmp14E;void*_tmp14D;int _tmp14C;_LL10C: _tmp14F=_tmp145.name;_tmp14E=_tmp145.tq;_tmp14D=_tmp145.type;_tmp14C=_tmp145.inject;_LL10D:;
if(_tmp14F == 0){
if(need_va_name)
({void*_tmp146=0;Cyc_Binding_err(loc,({const char*_tmp147="missing name for vararg";_tag_dyneither(_tmp147,sizeof(char),24);}),_tag_dyneither(_tmp146,sizeof(void*),0));});}else{
# 589
void*_tmp148=Cyc_Absyn_dyneither_typ(_tmp14D,(void*)& Cyc_Absyn_HeapRgn_val,_tmp14E,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl*_tmp149=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B->f1=Cyc_Absyn_Loc_n;_tmp14B->f2=_tmp14F;_tmp14B;}),_tmp148,0);
_tmp13A=({struct Cyc_List_List*_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->hd=_tmp149;_tmp14A->tl=_tmp13A;_tmp14A;});}}
# 594
_tmp13A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp13A);
return _tmp13A;}
# 597
struct _dyneither_ptr*Cyc_Binding_get_vds_fndecl(struct _dyneither_ptr*v){return v;}
struct _dyneither_ptr*Cyc_Binding_get_vds_fntype(struct _dyneither_ptr*v){return v;}
# 600
void Cyc_Binding_resolve_function_stuff(unsigned int loc,struct Cyc_Binding_Env*env,void*eff,void*rettyp,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai,struct Cyc_List_List*rpo,struct Cyc_Absyn_Exp*req_clause,struct Cyc_Absyn_Exp*ens_clause){
# 604
if(eff != 0)
Cyc_Binding_resolve_type(loc,env,eff);
Cyc_Binding_resolve_type(loc,env,rettyp);
for(0;args != 0;args=args->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple11*)args->hd)).f3);}
if(vai != 0)
Cyc_Binding_resolve_type(loc,env,vai->type);
Cyc_Binding_resolve_rgnpo(loc,env,rpo);
if(req_clause != 0)
Cyc_Binding_resolve_exp(env,req_clause);
if(ens_clause != 0){
# 616
struct Cyc_Dict_Dict _tmp150=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
struct _dyneither_ptr*_tmp151=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"return_value",sizeof(char),13);
struct Cyc_Absyn_Vardecl*_tmp152=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157->f1=Cyc_Absyn_Loc_n;_tmp157->f2=_tmp151;_tmp157;}),rettyp,0);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp151,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp154;_tmp154.tag=0;_tmp154.f1=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp156;_tmp156.tag=4;_tmp156.f1=_tmp152;_tmp156;});_tmp155;});_tmp154;});_tmp153;}));
# 621
Cyc_Binding_resolve_exp(env,ens_clause);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp150;}}struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 631
void Cyc_Binding_resolve_type(unsigned int loc,struct Cyc_Binding_Env*env,void*t){
void*_tmp159=t;struct Cyc_Absyn_Exp*_tmp1D6;struct Cyc_Absyn_Exp*_tmp1D5;void*_tmp1D4;void*_tmp1D3;void*_tmp1D2;void*_tmp1D1;void*_tmp1D0;void*_tmp1CF;struct Cyc_List_List*_tmp1CE;struct Cyc_Absyn_FnInfo _tmp1CD;void*_tmp1CC;struct Cyc_Absyn_Exp*_tmp1CB;struct Cyc_List_List*_tmp1CA;void*_tmp1C9;void*_tmp1C8;union Cyc_Absyn_Constraint*_tmp1C7;void*_tmp1C6;struct Cyc_List_List*_tmp1C5;struct Cyc_List_List*_tmp1C4;struct Cyc_Absyn_TypeDecl*_tmp1C3;void***_tmp1C2;struct _tuple0**_tmp1C1;struct Cyc_List_List*_tmp1C0;struct Cyc_Absyn_Typedefdecl**_tmp1BF;struct _tuple0**_tmp1BE;struct Cyc_Absyn_Enumdecl*_tmp1BD;union Cyc_Absyn_AggrInfoU*_tmp1BC;struct Cyc_List_List*_tmp1BB;union Cyc_Absyn_DatatypeFieldInfoU*_tmp1BA;struct Cyc_List_List*_tmp1B9;union Cyc_Absyn_DatatypeInfoU*_tmp1B8;struct Cyc_List_List*_tmp1B7;switch(*((int*)_tmp159)){case 3: _LL10F: _tmp1B8=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp159)->f1).datatype_info;_tmp1B7=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp159)->f1).targs;_LL110:
# 634
 for(0;_tmp1B7 != 0;_tmp1B7=_tmp1B7->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1B7->hd);}{
union Cyc_Absyn_DatatypeInfoU _tmp15A=*_tmp1B8;union Cyc_Absyn_DatatypeInfoU _tmp15B=_tmp15A;struct _tuple0*_tmp164;int _tmp163;if((_tmp15B.UnknownDatatype).tag == 1){_LL14A: _tmp164=((_tmp15B.UnknownDatatype).val).name;_tmp163=((_tmp15B.UnknownDatatype).val).is_extensible;_LL14B: {
# 638
struct _handler_cons _tmp15C;_push_handler(& _tmp15C);{int _tmp15E=0;if(setjmp(_tmp15C.handler))_tmp15E=1;if(!_tmp15E){
{struct Cyc_Absyn_Datatypedecl*_tmp15F=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp164,Cyc_Binding_lookup_datatypedecl))->hd;
*_tmp1B8=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp160;_tmp160.name=_tmp15F->name;_tmp160.is_extensible=_tmp163;_tmp160;}));
_npop_handler(0);return;}
# 639
;_pop_handler();}else{void*_tmp15D=(void*)_exn_thrown;void*_tmp161=_tmp15D;void*_tmp162;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp161)->tag == Cyc_Binding_BindingError){_LL14F: _LL150:
# 643
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp164,Cyc_Absyn_Public);return;}else{_LL151: _tmp162=_tmp161;_LL152:(int)_rethrow(_tmp162);}_LL14E:;}};}}else{_LL14C: _LL14D:
 return;}_LL149:;};case 4: _LL111: _tmp1BA=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp159)->f1).field_info;_tmp1B9=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp159)->f1).targs;_LL112:
# 647
 for(0;_tmp1B9 != 0;_tmp1B9=_tmp1B9->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1B9->hd);}{
union Cyc_Absyn_DatatypeFieldInfoU _tmp165=*_tmp1BA;union Cyc_Absyn_DatatypeFieldInfoU _tmp166=_tmp165;struct _tuple0*_tmp17A;struct _tuple0*_tmp179;int _tmp178;if((_tmp166.UnknownDatatypefield).tag == 1){_LL154: _tmp17A=((_tmp166.UnknownDatatypefield).val).datatype_name;_tmp179=((_tmp166.UnknownDatatypefield).val).field_name;_tmp178=((_tmp166.UnknownDatatypefield).val).is_extensible;_LL155:
# 653
{struct _handler_cons _tmp167;_push_handler(& _tmp167);{int _tmp169=0;if(setjmp(_tmp167.handler))_tmp169=1;if(!_tmp169){
{void*_tmp16A=Cyc_Binding_lookup_ordinary(loc,env,_tmp179);void*_tmp16B=_tmp16A;struct Cyc_Absyn_Datatypedecl*_tmp172;struct Cyc_Absyn_Datatypefield*_tmp171;if(((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp16B)->tag == 2){_LL159: _tmp172=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp16B)->f1;_tmp171=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp16B)->f2;_LL15A: {
# 656
struct Cyc_Absyn_Datatypedecl*_tmp16C=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp17A,Cyc_Binding_lookup_datatypedecl))->hd;
if(Cyc_Absyn_qvar_cmp(_tmp172->name,_tmp16C->name)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp16F;_tmp16F.tag=0;_tmp16F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp171->name));({void*_tmp16D[1]={& _tmp16F};Cyc_Binding_err(loc,({const char*_tmp16E="shadowed datatype field %s not yet implemented properly";_tag_dyneither(_tmp16E,sizeof(char),56);}),_tag_dyneither(_tmp16D,sizeof(void*),1));});});
_npop_handler(0);return;}
# 662
*_tmp1BA=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp170;_tmp170.datatype_name=_tmp172->name;_tmp170.field_name=_tmp171->name;_tmp170.is_extensible=_tmp178;_tmp170;}));
# 664
_npop_handler(0);return;}}else{_LL15B: _LL15C:
 goto _LL158;}_LL158:;}
# 654
;_pop_handler();}else{void*_tmp168=(void*)_exn_thrown;void*_tmp173=_tmp168;void*_tmp174;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp173)->tag == Cyc_Binding_BindingError){_LL15E: _LL15F:
# 667
 goto _LL15D;}else{_LL160: _tmp174=_tmp173;_LL161:(int)_rethrow(_tmp174);}_LL15D:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp177;_tmp177.tag=0;_tmp177.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp179));({void*_tmp175[1]={& _tmp177};Cyc_Binding_err(loc,({const char*_tmp176="%s is not a datatype field";_tag_dyneither(_tmp176,sizeof(char),27);}),_tag_dyneither(_tmp175,sizeof(void*),1));});});
return;}else{_LL156: _LL157:
 return;}_LL153:;};case 11: _LL113: _tmp1BC=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp159)->f1).aggr_info;_tmp1BB=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp159)->f1).targs;_LL114:
# 674
 for(0;_tmp1BB != 0;_tmp1BB=_tmp1BB->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1BB->hd);}{
union Cyc_Absyn_AggrInfoU _tmp17B=*_tmp1BC;union Cyc_Absyn_AggrInfoU _tmp17C=_tmp17B;enum Cyc_Absyn_AggrKind _tmp189;struct _tuple0*_tmp188;struct Cyc_Core_Opt*_tmp187;if((_tmp17C.UnknownAggr).tag == 1){_LL163: _tmp189=((_tmp17C.UnknownAggr).val).f1;_tmp188=((_tmp17C.UnknownAggr).val).f2;_tmp187=((_tmp17C.UnknownAggr).val).f3;_LL164: {
# 678
struct _handler_cons _tmp17D;_push_handler(& _tmp17D);{int _tmp17F=0;if(setjmp(_tmp17D.handler))_tmp17F=1;if(!_tmp17F){
{struct Cyc_Absyn_Aggrdecl*_tmp180=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp188,Cyc_Binding_lookup_aggrdecl);
if(_tmp180->kind != _tmp189)
({void*_tmp181=0;Cyc_Binding_err(loc,({const char*_tmp182="struct vs. union mismatch with earlier declaration";_tag_dyneither(_tmp182,sizeof(char),51);}),_tag_dyneither(_tmp181,sizeof(void*),0));});
if(((_tmp189 == Cyc_Absyn_UnionA  && _tmp187 != 0) && _tmp180->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp180->impl))->tagged != (int)_tmp187->v)
# 684
({void*_tmp183=0;Cyc_Binding_err(loc,({const char*_tmp184="@tagged mismatch with earlier declaration";_tag_dyneither(_tmp184,sizeof(char),42);}),_tag_dyneither(_tmp183,sizeof(void*),0));});
*_tmp1BC=Cyc_Absyn_UnknownAggr(_tmp189,_tmp180->name,_tmp187);
_npop_handler(0);return;}
# 679
;_pop_handler();}else{void*_tmp17E=(void*)_exn_thrown;void*_tmp185=_tmp17E;void*_tmp186;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp185)->tag == Cyc_Binding_BindingError){_LL168: _LL169:
# 688
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp188,Cyc_Absyn_Public);return;}else{_LL16A: _tmp186=_tmp185;_LL16B:(int)_rethrow(_tmp186);}_LL167:;}};}}else{_LL165: _LL166:
 return;}_LL162:;};case 13: _LL115: _tmp1BE=(struct _tuple0**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp159)->f1;_tmp1BD=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp159)->f2;_LL116:
# 692
 if(_tmp1BD != 0)
return;{
struct _handler_cons _tmp18A;_push_handler(& _tmp18A);{int _tmp18C=0;if(setjmp(_tmp18A.handler))_tmp18C=1;if(!_tmp18C){
{struct Cyc_Absyn_Enumdecl*_tmp18D=((struct Cyc_Absyn_Enumdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Enumdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*_tmp1BE,Cyc_Binding_lookup_enumdecl);
*_tmp1BE=_tmp18D->name;
_npop_handler(0);return;}
# 695
;_pop_handler();}else{void*_tmp18B=(void*)_exn_thrown;void*_tmp18E=_tmp18B;void*_tmp18F;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp18E)->tag == Cyc_Binding_BindingError){_LL16D: _LL16E:
# 699
 Cyc_Binding_absolutize_topdecl(loc,env,*_tmp1BE,Cyc_Absyn_Public);return;}else{_LL16F: _tmp18F=_tmp18E;_LL170:(int)_rethrow(_tmp18F);}_LL16C:;}};};case 17: _LL117: _tmp1C1=(struct _tuple0**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp159)->f1;_tmp1C0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp159)->f2;_tmp1BF=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp159)->f3;_LL118:
# 702
 for(0;_tmp1C0 != 0;_tmp1C0=_tmp1C0->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1C0->hd);}
{struct _handler_cons _tmp190;_push_handler(& _tmp190);{int _tmp192=0;if(setjmp(_tmp190.handler))_tmp192=1;if(!_tmp192){
{struct Cyc_Absyn_Typedefdecl*_tmp193=((struct Cyc_Absyn_Typedefdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*_tmp1C1,Cyc_Binding_lookup_typedefdecl);
# 707
*_tmp1C1=_tmp193->name;
_npop_handler(0);return;}
# 705
;_pop_handler();}else{void*_tmp191=(void*)_exn_thrown;void*_tmp194=_tmp191;void*_tmp195;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp194)->tag == Cyc_Binding_BindingError){_LL172: _LL173:
# 709
 goto _LL171;}else{_LL174: _tmp195=_tmp194;_LL175:(int)_rethrow(_tmp195);}_LL171:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp198;_tmp198.tag=0;_tmp198.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*_tmp1C1));({void*_tmp196[1]={& _tmp198};Cyc_Binding_err(loc,({const char*_tmp197="unbound typdef name %s";_tag_dyneither(_tmp197,sizeof(char),23);}),_tag_dyneither(_tmp196,sizeof(void*),1));});});
return;case 26: _LL119: _tmp1C3=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp159)->f1;_tmp1C2=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp159)->f2;_LL11A: {
# 715
struct Cyc_Dict_Dict*_tmp199=env->local_vars;
env->local_vars=0;
{void*_tmp19A=_tmp1C3->r;void*_tmp19B=_tmp19A;struct Cyc_Absyn_Datatypedecl*_tmp1A7;struct Cyc_Absyn_Enumdecl*_tmp1A6;struct Cyc_Absyn_Aggrdecl*_tmp1A5;switch(*((int*)_tmp19B)){case 0: _LL177: _tmp1A5=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp19B)->f1;_LL178:
# 719
 Cyc_Binding_resolve_decl(env,({struct Cyc_Absyn_Decl*_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C->r=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp19E;_tmp19E.tag=5;_tmp19E.f1=_tmp1A5;_tmp19E;});_tmp19D;});_tmp19C->loc=_tmp1C3->loc;_tmp19C;}));goto _LL176;case 1: _LL179: _tmp1A6=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp19B)->f1;_LL17A:
# 721
 Cyc_Binding_resolve_decl(env,({struct Cyc_Absyn_Decl*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F->r=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0[0]=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp1A1;_tmp1A1.tag=7;_tmp1A1.f1=_tmp1A6;_tmp1A1;});_tmp1A0;});_tmp19F->loc=_tmp1C3->loc;_tmp19F;}));goto _LL176;default: _LL17B: _tmp1A7=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp19B)->f1;_LL17C:
# 723
 Cyc_Binding_resolve_decl(env,({struct Cyc_Absyn_Decl*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->r=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3[0]=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp1A4;_tmp1A4.tag=6;_tmp1A4.f1=_tmp1A7;_tmp1A4;});_tmp1A3;});_tmp1A2->loc=_tmp1C3->loc;_tmp1A2;}));goto _LL176;}_LL176:;}
# 725
env->local_vars=_tmp199;
return;}case 12: _LL11B: _tmp1C4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp159)->f2;_LL11C:
# 732
 Cyc_Binding_resolve_aggrfields(loc,env,_tmp1C4);
return;case 14: _LL11D: _tmp1C5=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp159)->f1;_LL11E:
# 737
 for(0;_tmp1C5 != 0;_tmp1C5=_tmp1C5->tl){
struct Cyc_Absyn_Enumfield*_tmp1A8=(struct Cyc_Absyn_Enumfield*)_tmp1C5->hd;
if(_tmp1A8->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp1A8->tag));}
# 742
return;case 1: _LL11F: _tmp1C6=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp159)->f2;_LL120:
 if(_tmp1C6 != 0)Cyc_Binding_resolve_type(loc,env,_tmp1C6);return;case 28: _LL121: _LL122:
# 745
 goto _LL124;case 0: _LL123: _LL124:
 goto _LL126;case 6: _LL125: _LL126:
 goto _LL128;case 7: _LL127: _LL128:
 goto _LL12A;case 20: _LL129: _LL12A:
 goto _LL12C;case 21: _LL12B: _LL12C:
 goto _LL12E;case 22: _LL12D: _LL12E:
 goto _LL130;case 2: _LL12F: _LL130:
 return;case 5: _LL131: _tmp1C9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp159)->f1).elt_typ;_tmp1C8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp159)->f1).ptr_atts).rgn;_tmp1C7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp159)->f1).ptr_atts).bounds;_LL132:
# 755
 Cyc_Binding_resolve_type(loc,env,_tmp1C9);
Cyc_Binding_resolve_type(loc,env,_tmp1C8);
while(1){
union Cyc_Absyn_Constraint*_tmp1A9=_tmp1C7;void*_tmp1AD;union Cyc_Absyn_Constraint*_tmp1AC;switch((((union Cyc_Absyn_Constraint*)_tmp1A9)->Forward_constr).tag){case 3: _LL17E: _LL17F:
 return;case 2: _LL180: _tmp1AC=(_tmp1A9->Forward_constr).val;_LL181:
 _tmp1C7=_tmp1AC;continue;default: _LL182: _tmp1AD=(void*)(_tmp1A9->Eq_constr).val;_LL183: {
# 762
void*_tmp1AA=_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AB;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1AA)->tag == 0){_LL185: _LL186:
 return;}else{_LL187: _tmp1AB=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp1AA)->f1;_LL188:
 Cyc_Binding_resolve_exp(env,_tmp1AB);return;}_LL184:;}}_LL17D:;}case 10: _LL133: _tmp1CA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp159)->f1;_LL134:
# 768
 for(0;_tmp1CA != 0;_tmp1CA=_tmp1CA->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple12*)_tmp1CA->hd)).f2);}
return;case 8: _LL135: _tmp1CC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp159)->f1).elt_type;_tmp1CB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp159)->f1).num_elts;_LL136:
# 772
 Cyc_Binding_resolve_type(loc,env,_tmp1CC);
if(_tmp1CB != 0)Cyc_Binding_resolve_exp(env,_tmp1CB);
return;case 9: _LL137: _tmp1CD=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp159)->f1;_LL138: {
# 779
struct Cyc_List_List*_tmp1AE=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),int need_va_name,unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai))Cyc_Binding_get_fun_vardecls)(Cyc_Binding_get_vds_fntype,0,loc,env,_tmp1CD.args,_tmp1CD.cyc_varargs);
# 782
struct Cyc_Dict_Dict*_tmp1AF=env->local_vars;
if(_tmp1AF != 0)
env->local_vars=({struct Cyc_Dict_Dict*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0[0]=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp1B0;});else{
# 786
env->local_vars=({struct Cyc_Dict_Dict*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1[0]=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp1B1;});}
{struct Cyc_List_List*_tmp1B2=_tmp1AE;for(0;_tmp1B2 != 0;_tmp1B2=_tmp1B2->tl){
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),(*((struct Cyc_Absyn_Vardecl*)_tmp1B2->hd)->name).f2,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp1B3=_cycalloc(sizeof(*_tmp1B3));_tmp1B3[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp1B4;_tmp1B4.tag=0;_tmp1B4.f1=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp1B6;_tmp1B6.tag=3;_tmp1B6.f1=(struct Cyc_Absyn_Vardecl*)_tmp1B2->hd;_tmp1B6;});_tmp1B5;});_tmp1B4;});_tmp1B3;}));}}
# 791
((void(*)(unsigned int loc,struct Cyc_Binding_Env*env,void*eff,void*rettyp,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai,struct Cyc_List_List*rpo,struct Cyc_Absyn_Exp*req_clause,struct Cyc_Absyn_Exp*ens_clause))Cyc_Binding_resolve_function_stuff)(loc,env,_tmp1CD.effect,_tmp1CD.ret_typ,_tmp1CD.args,_tmp1CD.cyc_varargs,_tmp1CD.rgn_po,_tmp1CD.requires_clause,_tmp1CD.ensures_clause);
# 795
env->local_vars=_tmp1AF;
return;}case 24: _LL139: _tmp1CE=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp159)->f1;_LL13A:
# 799
 for(0;_tmp1CE != 0;_tmp1CE=_tmp1CE->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1CE->hd);}
return;case 19: _LL13B: _tmp1CF=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp159)->f1;_LL13C:
# 803
 _tmp1D0=_tmp1CF;goto _LL13E;case 23: _LL13D: _tmp1D0=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp159)->f1;_LL13E:
 _tmp1D1=_tmp1D0;goto _LL140;case 25: _LL13F: _tmp1D1=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp159)->f1;_LL140:
 _tmp1D2=_tmp1D1;goto _LL142;case 15: _LL141: _tmp1D2=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp159)->f1;_LL142:
 Cyc_Binding_resolve_type(loc,env,_tmp1D2);return;case 16: _LL143: _tmp1D4=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp159)->f1;_tmp1D3=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp159)->f2;_LL144:
# 809
 Cyc_Binding_resolve_type(loc,env,_tmp1D4);Cyc_Binding_resolve_type(loc,env,_tmp1D3);return;case 18: _LL145: _tmp1D5=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp159)->f1;_LL146:
# 811
 _tmp1D6=_tmp1D5;goto _LL148;default: _LL147: _tmp1D6=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp159)->f1;_LL148:
 Cyc_Binding_resolve_exp(env,_tmp1D6);return;}_LL10E:;}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 816
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Pat*p){
struct Cyc_Absyn_Pat*_tmp1D7=p;void*_tmp231;unsigned int _tmp230;_LL18A: _tmp231=_tmp1D7->r;_tmp230=_tmp1D7->loc;_LL18B:;{
void*_tmp1D8=_tmp231;struct Cyc_List_List*_tmp22F;struct Cyc_Absyn_Exp*_tmp22E;struct Cyc_List_List*_tmp22D;struct Cyc_Absyn_Pat*_tmp22C;struct Cyc_Absyn_Vardecl*_tmp22B;struct Cyc_Absyn_Vardecl*_tmp22A;struct Cyc_Absyn_Vardecl*_tmp229;struct Cyc_Absyn_Pat*_tmp228;struct Cyc_Absyn_Vardecl*_tmp227;struct Cyc_Absyn_Pat*_tmp226;struct Cyc_List_List*_tmp225;struct Cyc_List_List*_tmp224;struct _tuple0*_tmp223;struct Cyc_List_List*_tmp222;struct Cyc_List_List*_tmp221;struct Cyc_List_List*_tmp220;int _tmp21F;struct _tuple0*_tmp21E;struct Cyc_List_List*_tmp21D;int _tmp21C;struct _tuple0*_tmp21B;switch(*((int*)_tmp1D8)){case 15: _LL18D: _tmp21B=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1D8)->f1;_LL18E:
# 820
{struct _handler_cons _tmp1D9;_push_handler(& _tmp1D9);{int _tmp1DB=0;if(setjmp(_tmp1D9.handler))_tmp1DB=1;if(!_tmp1DB){
{void*_tmp1DC=Cyc_Binding_lookup_ordinary(_tmp230,env,_tmp21B);void*_tmp1DD=_tmp1DC;void*_tmp1EB;struct Cyc_Absyn_Enumfield*_tmp1EA;struct Cyc_Absyn_Enumdecl*_tmp1E9;struct Cyc_Absyn_Enumfield*_tmp1E8;struct Cyc_Absyn_Datatypedecl*_tmp1E7;struct Cyc_Absyn_Datatypefield*_tmp1E6;switch(*((int*)_tmp1DD)){case 0: _LL1B6: _LL1B7:
 goto _LL1B5;case 1: _LL1B8: _LL1B9:
# 824
({void*_tmp1DE=0;Cyc_Binding_err(_tmp230,({const char*_tmp1DF="struct tag used without arguments in pattern";_tag_dyneither(_tmp1DF,sizeof(char),45);}),_tag_dyneither(_tmp1DE,sizeof(void*),0));});
_npop_handler(0);return;case 2: _LL1BA: _tmp1E7=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1DD)->f1;_tmp1E6=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1DD)->f2;_LL1BB:
# 827
 p->r=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0[0]=({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp1E1;_tmp1E1.tag=8;_tmp1E1.f1=_tmp1E7;_tmp1E1.f2=_tmp1E6;_tmp1E1.f3=0;_tmp1E1.f4=0;_tmp1E1;});_tmp1E0;});_npop_handler(0);return;case 3: _LL1BC: _tmp1E9=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp1DD)->f1;_tmp1E8=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp1DD)->f2;_LL1BD:
# 829
 p->r=(void*)({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp1E2=_cycalloc(sizeof(*_tmp1E2));_tmp1E2[0]=({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp1E3;_tmp1E3.tag=13;_tmp1E3.f1=_tmp1E9;_tmp1E3.f2=_tmp1E8;_tmp1E3;});_tmp1E2;});_npop_handler(0);return;default: _LL1BE: _tmp1EB=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp1DD)->f1;_tmp1EA=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp1DD)->f2;_LL1BF:
# 831
 p->r=(void*)({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp1E4=_cycalloc(sizeof(*_tmp1E4));_tmp1E4[0]=({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp1E5;_tmp1E5.tag=14;_tmp1E5.f1=_tmp1EB;_tmp1E5.f2=_tmp1EA;_tmp1E5;});_tmp1E4;});_npop_handler(0);return;}_LL1B5:;}
# 821
;_pop_handler();}else{void*_tmp1DA=(void*)_exn_thrown;void*_tmp1EC=_tmp1DA;void*_tmp1ED;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1EC)->tag == Cyc_Binding_BindingError){_LL1C1: _LL1C2:
# 833
 goto _LL1C0;}else{_LL1C3: _tmp1ED=_tmp1EC;_LL1C4:(int)_rethrow(_tmp1ED);}_LL1C0:;}};}
Cyc_Binding_absolutize_decl(_tmp230,env,_tmp21B,Cyc_Absyn_Public);{
struct Cyc_Absyn_Vardecl*_tmp1EE=Cyc_Absyn_new_vardecl(_tmp21B,(void*)& Cyc_Absyn_VoidType_val,0);
p->r=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF[0]=({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp1F0;_tmp1F0.tag=1;_tmp1F0.f1=_tmp1EE;_tmp1F0.f2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);_tmp1F0;});_tmp1EF;});
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),(*_tmp1EE->name).f2,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp1F2;_tmp1F2.tag=0;_tmp1F2.f1=(void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));_tmp1F3[0]=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp1F4;_tmp1F4.tag=5;_tmp1F4.f1=_tmp1EE;_tmp1F4;});_tmp1F3;});_tmp1F2;});_tmp1F1;}));
# 839
return;};case 16: _LL18F: _tmp21E=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D8)->f1;_tmp21D=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D8)->f2;_tmp21C=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D8)->f3;_LL190:
# 842
{struct Cyc_List_List*_tmp1F5=_tmp21D;for(0;_tmp1F5 != 0;_tmp1F5=_tmp1F5->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp1F5->hd);}}
{struct _handler_cons _tmp1F6;_push_handler(& _tmp1F6);{int _tmp1F8=0;if(setjmp(_tmp1F6.handler))_tmp1F8=1;if(!_tmp1F8){
{void*_tmp1F9=Cyc_Binding_lookup_ordinary(_tmp230,env,_tmp21E);void*_tmp1FA=_tmp1F9;struct Cyc_Absyn_Datatypedecl*_tmp208;struct Cyc_Absyn_Datatypefield*_tmp207;struct Cyc_Absyn_Aggrdecl*_tmp206;switch(*((int*)_tmp1FA)){case 0: _LL1C6: _LL1C7:
 goto _LL1C5;case 1: _LL1C8: _tmp206=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmp1FA)->f1;_LL1C9: {
# 848
struct Cyc_List_List*_tmp1FB=0;
for(0;_tmp21D != 0;_tmp21D=_tmp21D->tl){
_tmp1FB=({struct Cyc_List_List*_tmp1FC=_cycalloc(sizeof(*_tmp1FC));_tmp1FC->hd=({struct _tuple13*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));_tmp1FD->f1=0;_tmp1FD->f2=(struct Cyc_Absyn_Pat*)_tmp21D->hd;_tmp1FD;});_tmp1FC->tl=_tmp1FB;_tmp1FC;});}
p->r=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));_tmp1FE[0]=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp1FF;_tmp1FF.tag=7;_tmp1FF.f1=({struct Cyc_Absyn_AggrInfo*_tmp200=_cycalloc(sizeof(*_tmp200));_tmp200->aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp201=_cycalloc(sizeof(*_tmp201));_tmp201[0]=_tmp206;_tmp201;}));_tmp200->targs=0;_tmp200;});_tmp1FF.f2=0;_tmp1FF.f3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1FB);_tmp1FF.f4=_tmp21C;_tmp1FF;});_tmp1FE;});
_npop_handler(0);return;}case 2: _LL1CA: _tmp208=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1FA)->f1;_tmp207=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1FA)->f2;_LL1CB:
# 855
 p->r=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202[0]=({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp203;_tmp203.tag=8;_tmp203.f1=_tmp208;_tmp203.f2=_tmp207;_tmp203.f3=_tmp21D;_tmp203.f4=_tmp21C;_tmp203;});_tmp202;});_npop_handler(0);return;case 3: _LL1CC: _LL1CD:
 goto _LL1CF;default: _LL1CE: _LL1CF:
# 858
({void*_tmp204=0;Cyc_Binding_err(_tmp230,({const char*_tmp205="enum tag used with arguments in pattern";_tag_dyneither(_tmp205,sizeof(char),40);}),_tag_dyneither(_tmp204,sizeof(void*),0));});_npop_handler(0);return;}_LL1C5:;}
# 845
;_pop_handler();}else{void*_tmp1F7=(void*)_exn_thrown;void*_tmp209=_tmp1F7;void*_tmp20A;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp209)->tag == Cyc_Binding_BindingError){_LL1D1: _LL1D2:
# 860
 goto _LL1D0;}else{_LL1D3: _tmp20A=_tmp209;_LL1D4:(int)_rethrow(_tmp20A);}_LL1D0:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp20D;_tmp20D.tag=0;_tmp20D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp21E));({void*_tmp20B[1]={& _tmp20D};Cyc_Binding_err(_tmp230,({const char*_tmp20C="%s is not a constructor in pattern";_tag_dyneither(_tmp20C,sizeof(char),35);}),_tag_dyneither(_tmp20B,sizeof(void*),1));});});
return;case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D8)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D8)->f1)->aggr_info).UnknownAggr).tag == 1){_LL191: _tmp223=((((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D8)->f1)->aggr_info).UnknownAggr).val).f2;_tmp222=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D8)->f1)->targs;_tmp221=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D8)->f2;_tmp220=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D8)->f3;_tmp21F=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D8)->f4;_LL192:
# 865
{struct Cyc_List_List*_tmp20E=_tmp220;for(0;_tmp20E != 0;_tmp20E=_tmp20E->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple13*)_tmp20E->hd)).f2);}}
{struct _handler_cons _tmp20F;_push_handler(& _tmp20F);{int _tmp211=0;if(setjmp(_tmp20F.handler))_tmp211=1;if(!_tmp211){
{struct Cyc_Absyn_Aggrdecl*_tmp212=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(_tmp230,env->ns,_tmp223,Cyc_Binding_lookup_aggrdecl);
p->r=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp213=_cycalloc(sizeof(*_tmp213));_tmp213[0]=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp214;_tmp214.tag=7;_tmp214.f1=({struct Cyc_Absyn_AggrInfo*_tmp215=_cycalloc(sizeof(*_tmp215));_tmp215->aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp216=_cycalloc(sizeof(*_tmp216));_tmp216[0]=_tmp212;_tmp216;}));_tmp215->targs=_tmp222;_tmp215;});_tmp214.f2=_tmp221;_tmp214.f3=_tmp220;_tmp214.f4=_tmp21F;_tmp214;});_tmp213;});}
# 868
;_pop_handler();}else{void*_tmp210=(void*)_exn_thrown;void*_tmp217=_tmp210;void*_tmp21A;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp217)->tag == Cyc_Binding_BindingError){_LL1D6: _LL1D7:
# 872
({void*_tmp218=0;Cyc_Binding_err(_tmp230,({const char*_tmp219="non-aggregate name has designator patterns";_tag_dyneither(_tmp219,sizeof(char),43);}),_tag_dyneither(_tmp218,sizeof(void*),0));});
return;}else{_LL1D8: _tmp21A=_tmp217;_LL1D9:(int)_rethrow(_tmp21A);}_LL1D5:;}};}
# 875
return;}else{_LL1AD: _tmp224=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D8)->f3;_LL1AE:
# 904
 for(0;_tmp224 != 0;_tmp224=_tmp224->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple13*)_tmp224->hd)).f2);}
return;}}else{_LL1AB: _tmp225=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D8)->f3;_LL1AC:
# 902
 _tmp224=_tmp225;goto _LL1AE;}case 0: _LL193: _LL194:
# 877
 return;case 3: _LL195: _tmp227=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D8)->f1;_tmp226=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D8)->f2;_LL196:
# 879
 _tmp229=_tmp227;_tmp228=_tmp226;goto _LL198;case 1: _LL197: _tmp229=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D8)->f1;_tmp228=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D8)->f2;_LL198:
# 881
 Cyc_Binding_resolve_pat(env,_tmp228);
_tmp22A=_tmp229;goto _LL19A;case 4: _LL199: _tmp22A=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1D8)->f2;_LL19A:
 _tmp22B=_tmp22A;goto _LL19C;case 2: _LL19B: _tmp22B=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1D8)->f2;_LL19C:
# 885
 Cyc_Binding_resolve_and_add_vardecl(_tmp230,env,_tmp22B);goto _LL18C;case 6: _LL19D: _tmp22C=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1D8)->f1;_LL19E:
# 887
 Cyc_Binding_resolve_pat(env,_tmp22C);return;case 5: _LL19F: _tmp22D=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1D8)->f1;_LL1A0:
# 889
 for(0;_tmp22D != 0;_tmp22D=_tmp22D->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp22D->hd);}
goto _LL18C;case 17: _LL1A1: _tmp22E=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1D8)->f1;_LL1A2:
# 893
 Cyc_Binding_resolve_exp(env,_tmp22E);return;case 9: _LL1A3: _LL1A4:
# 895
 goto _LL1A6;case 10: _LL1A5: _LL1A6:
 goto _LL1A8;case 11: _LL1A7: _LL1A8:
 goto _LL1AA;case 12: _LL1A9: _LL1AA:
 return;case 8: _LL1AF: _tmp22F=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D8)->f3;_LL1B0:
# 908
 for(0;_tmp22F != 0;_tmp22F=_tmp22F->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp22F->hd);}
return;case 13: _LL1B1: _LL1B2:
 goto _LL1B4;default: _LL1B3: _LL1B4:
 return;}_LL18C:;};}struct _tuple14{void*f1;void*f2;};
# 916
void Cyc_Binding_resolve_rgnpo(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*po){
for(0;po != 0;po=po->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple14*)po->hd)).f1);
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple14*)po->hd)).f2);}}struct _tuple15{unsigned int f1;struct _tuple0*f2;int f3;};
# 930 "binding.cyc"
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Decl*d){
# 933
unsigned int loc=d->loc;
void*_tmp232=d->r;void*_tmp233=_tmp232;struct Cyc_List_List*_tmp2B7;struct Cyc_List_List*_tmp2B6;struct Cyc_List_List*_tmp2B5;struct _tuple0*_tmp2B4;struct Cyc_List_List*_tmp2B3;struct _dyneither_ptr*_tmp2B2;struct Cyc_List_List*_tmp2B1;struct Cyc_Absyn_Datatypedecl*_tmp2B0;struct Cyc_Absyn_Enumdecl*_tmp2AF;struct Cyc_Absyn_Aggrdecl*_tmp2AE;struct Cyc_Absyn_Typedefdecl*_tmp2AD;struct Cyc_Absyn_Tvar*_tmp2AC;struct Cyc_Absyn_Vardecl*_tmp2AB;int _tmp2AA;struct Cyc_Absyn_Exp*_tmp2A9;struct Cyc_List_List*_tmp2A8;struct Cyc_Absyn_Pat*_tmp2A7;struct Cyc_Absyn_Exp*_tmp2A6;struct Cyc_Absyn_Fndecl*_tmp2A5;struct Cyc_Absyn_Vardecl*_tmp2A4;switch(*((int*)_tmp233)){case 0: _LL1DB: _tmp2A4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp233)->f1;_LL1DC:
# 937
 Cyc_Binding_resolve_and_add_vardecl(loc,env,_tmp2A4);
# 942
if(_tmp2A4->initializer != 0  && !Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp2A4->initializer));
goto _LL1DA;case 1: _LL1DD: _tmp2A5=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp233)->f1;_LL1DE:
# 947
 Cyc_Binding_absolutize_decl(loc,env,_tmp2A5->name,_tmp2A5->sc);{
struct _tuple0*_tmp234=_tmp2A5->name;struct _tuple0*_tmp235=_tmp234;union Cyc_Absyn_Nmspace _tmp24E;struct _dyneither_ptr*_tmp24D;_LL1FA: _tmp24E=_tmp235->f1;_tmp24D=_tmp235->f2;_LL1FB:;{
# 950
struct Cyc_List_List*_tmp236=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),int need_va_name,unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai))Cyc_Binding_get_fun_vardecls)(Cyc_Binding_get_vds_fndecl,1,loc,env,_tmp2A5->args,_tmp2A5->cyc_varargs);
# 952
_tmp2A5->param_vardecls=({struct Cyc_Core_Opt*_tmp237=_cycalloc(sizeof(*_tmp237));_tmp237->v=_tmp236;_tmp237;});{
# 954
struct Cyc_Dict_Dict*_tmp238=env->local_vars;
if(_tmp238 != 0)
env->local_vars=({struct Cyc_Dict_Dict*_tmp239=_cycalloc(sizeof(*_tmp239));_tmp239[0]=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp239;});else{
# 958
env->local_vars=({struct Cyc_Dict_Dict*_tmp23A=_cycalloc(sizeof(*_tmp23A));_tmp23A[0]=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp23A;});}
{struct Cyc_List_List*_tmp23B=_tmp236;for(0;_tmp23B != 0;_tmp23B=_tmp23B->tl){
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),(*((struct Cyc_Absyn_Vardecl*)_tmp23B->hd)->name).f2,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp23C=_cycalloc(sizeof(*_tmp23C));_tmp23C[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp23D;_tmp23D.tag=0;_tmp23D.f1=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp23E=_cycalloc(sizeof(*_tmp23E));_tmp23E[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp23F;_tmp23F.tag=3;_tmp23F.f1=(struct Cyc_Absyn_Vardecl*)_tmp23B->hd;_tmp23F;});_tmp23E;});_tmp23D;});_tmp23C;}));}}
# 963
((void(*)(unsigned int loc,struct Cyc_Binding_Env*env,void*eff,void*rettyp,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai,struct Cyc_List_List*rpo,struct Cyc_Absyn_Exp*req_clause,struct Cyc_Absyn_Exp*ens_clause))Cyc_Binding_resolve_function_stuff)(loc,env,_tmp2A5->effect,_tmp2A5->ret_type,_tmp2A5->args,_tmp2A5->cyc_varargs,_tmp2A5->rgn_po,_tmp2A5->requires_clause,_tmp2A5->ensures_clause);
# 967
if(_tmp238 != 0){
# 969
*_tmp238=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*_tmp238,_tmp24D,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp240=_cycalloc(sizeof(*_tmp240));_tmp240[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp241;_tmp241.tag=0;_tmp241.f1=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp242=_cycalloc(sizeof(*_tmp242));_tmp242[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp243;_tmp243.tag=2;_tmp243.f1=_tmp2A5;_tmp243;});_tmp242;});_tmp241;});_tmp240;}));
# 971
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp24D,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp244=_cycalloc(sizeof(*_tmp244));_tmp244[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp245;_tmp245.tag=0;_tmp245.f1=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp246=_cycalloc(sizeof(*_tmp246));_tmp246[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp247;_tmp247.tag=2;_tmp247.f1=_tmp2A5;_tmp247;});_tmp246;});_tmp245;});_tmp244;}));}else{
# 974
struct Cyc_Binding_ResolveNSEnv*_tmp248=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp24E);
_tmp248->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp248->ordinaries,_tmp24D,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp249=_cycalloc(sizeof(*_tmp249));_tmp249[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp24A;_tmp24A.tag=0;_tmp24A.f1=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp24B=_cycalloc(sizeof(*_tmp24B));_tmp24B[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp24C;_tmp24C.tag=2;_tmp24C.f1=_tmp2A5;_tmp24C;});_tmp24B;});_tmp24A;});_tmp249;}));}
# 980
if(!Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_stmt(env,_tmp2A5->body);
# 983
env->local_vars=_tmp238;
goto _LL1DA;};};};case 2: _LL1DF: _tmp2A7=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp233)->f1;_tmp2A6=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp233)->f3;_LL1E0:
# 987
 if(Cyc_Binding_at_toplevel(env)){
({void*_tmp24F=0;Cyc_Binding_err(loc,({const char*_tmp250="let not allowed at top-level";_tag_dyneither(_tmp250,sizeof(char),29);}),_tag_dyneither(_tmp24F,sizeof(void*),0));});
goto _LL1DA;}
# 991
Cyc_Binding_resolve_exp(env,_tmp2A6);
Cyc_Binding_resolve_pat(env,_tmp2A7);
goto _LL1DA;case 3: _LL1E1: _tmp2A8=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp233)->f1;_LL1E2:
# 996
 for(0;_tmp2A8 != 0;_tmp2A8=_tmp2A8->tl){
Cyc_Binding_resolve_and_add_vardecl(loc,env,(struct Cyc_Absyn_Vardecl*)_tmp2A8->hd);}
goto _LL1DA;case 4: _LL1E3: _tmp2AC=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp233)->f1;_tmp2AB=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp233)->f2;_tmp2AA=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp233)->f3;_tmp2A9=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp233)->f4;_LL1E4:
# 1001
 if(_tmp2A9 != 0)
Cyc_Binding_resolve_exp(env,_tmp2A9);
Cyc_Binding_resolve_and_add_vardecl(loc,env,_tmp2AB);
goto _LL1DA;case 8: _LL1E5: _tmp2AD=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp233)->f1;_LL1E6:
# 1007
 Cyc_Binding_absolutize_decl(loc,env,_tmp2AD->name,_tmp2AD->extern_c?Cyc_Absyn_ExternC: Cyc_Absyn_Public);{
struct _tuple0*_tmp251=_tmp2AD->name;struct _tuple0*_tmp252=_tmp251;union Cyc_Absyn_Nmspace _tmp255;struct _dyneither_ptr*_tmp254;_LL1FD: _tmp255=_tmp252->f1;_tmp254=_tmp252->f2;_LL1FE:;{
# 1010
struct Cyc_Binding_ResolveNSEnv*_tmp253=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp255);
if(_tmp2AD->defn != 0)
Cyc_Binding_resolve_type(loc,env,(void*)_check_null(_tmp2AD->defn));
# 1014
_tmp253->typedefs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(_tmp253->typedefs,_tmp254,_tmp2AD);
# 1016
goto _LL1DA;};};case 5: _LL1E7: _tmp2AE=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp233)->f1;_LL1E8:
# 1019
 Cyc_Binding_absolutize_decl(loc,env,_tmp2AE->name,_tmp2AE->sc);{
struct _tuple0*_tmp256=_tmp2AE->name;struct _tuple0*_tmp257=_tmp256;union Cyc_Absyn_Nmspace _tmp260;struct _dyneither_ptr*_tmp25F;_LL200: _tmp260=_tmp257->f1;_tmp25F=_tmp257->f2;_LL201:;{
# 1022
struct Cyc_Binding_ResolveNSEnv*_tmp258=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp260);
# 1024
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp258->aggrdecls,_tmp25F) && _tmp2AE->impl == 0)
goto _LL1DA;
# 1027
_tmp258->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp258->aggrdecls,_tmp25F,_tmp2AE);
# 1029
_tmp258->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp258->ordinaries,_tmp25F,(void*)({struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_tmp259=_cycalloc(sizeof(*_tmp259));_tmp259[0]=({struct Cyc_Binding_AggrRes_Binding_Resolved_struct _tmp25A;_tmp25A.tag=1;_tmp25A.f1=_tmp2AE;_tmp25A;});_tmp259;}));
# 1031
if(_tmp2AE->impl != 0){
struct Cyc_Absyn_AggrdeclImpl*_tmp25B=(struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2AE->impl);struct Cyc_Absyn_AggrdeclImpl*_tmp25C=_tmp25B;struct Cyc_List_List*_tmp25E;struct Cyc_List_List*_tmp25D;_LL203: _tmp25E=_tmp25C->rgn_po;_tmp25D=_tmp25C->fields;_LL204:;
Cyc_Binding_resolve_rgnpo(loc,env,_tmp25E);
Cyc_Binding_resolve_aggrfields(loc,env,_tmp25D);}
# 1036
goto _LL1DA;};};case 7: _LL1E9: _tmp2AF=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp233)->f1;_LL1EA:
# 1039
 Cyc_Binding_absolutize_decl(loc,env,_tmp2AF->name,_tmp2AF->sc);{
struct _tuple0*_tmp261=_tmp2AF->name;struct _tuple0*_tmp262=_tmp261;union Cyc_Absyn_Nmspace _tmp268;struct _dyneither_ptr*_tmp267;_LL206: _tmp268=_tmp262->f1;_tmp267=_tmp262->f2;_LL207:;{
# 1042
struct Cyc_Binding_ResolveNSEnv*_tmp263=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp268);
# 1044
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp263->enumdecls,_tmp267) && _tmp2AF->fields == 0)
goto _LL1DA;
# 1048
_tmp263->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp263->enumdecls,_tmp267,_tmp2AF);
# 1050
if(_tmp2AF->fields != 0){
# 1052
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2AF->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp264=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Binding_absolutize_decl(_tmp264->loc,env,_tmp264->name,_tmp2AF->sc);
if(_tmp264->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp264->tag));
_tmp263->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp263->ordinaries,(*_tmp264->name).f2,(void*)({struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_tmp265=_cycalloc(sizeof(*_tmp265));_tmp265[0]=({struct Cyc_Binding_EnumRes_Binding_Resolved_struct _tmp266;_tmp266.tag=3;_tmp266.f1=_tmp2AF;_tmp266.f2=_tmp264;_tmp266;});_tmp265;}));}}
# 1061
goto _LL1DA;};};case 6: _LL1EB: _tmp2B0=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp233)->f1;_LL1EC:
# 1065
{struct _handler_cons _tmp269;_push_handler(& _tmp269);{int _tmp26B=0;if(setjmp(_tmp269.handler))_tmp26B=1;if(!_tmp26B){
{struct Cyc_List_List*_tmp26C=((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp2B0->name,Cyc_Binding_lookup_datatypedecl);
struct Cyc_Absyn_Datatypedecl*_tmp26D=(struct Cyc_Absyn_Datatypedecl*)_tmp26C->hd;
if(!_tmp26D->is_extensible)
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp26E=_cycalloc_atomic(sizeof(*_tmp26E));_tmp26E[0]=({struct Cyc_Binding_BindingError_exn_struct _tmp26F;_tmp26F.tag=Cyc_Binding_BindingError;_tmp26F;});_tmp26E;}));
_tmp2B0->name=_tmp26D->name;
_tmp2B0->is_extensible=1;{
struct _tuple0*_tmp270=_tmp2B0->name;struct _tuple0*_tmp271=_tmp270;union Cyc_Absyn_Nmspace _tmp275;struct _dyneither_ptr*_tmp274;_LL209: _tmp275=_tmp271->f1;_tmp274=_tmp271->f2;_LL20A:;{
struct Cyc_Binding_ResolveNSEnv*_tmp272=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp275);
_tmp272->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp272->datatypedecls,_tmp274,({struct Cyc_List_List*_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273->hd=_tmp2B0;_tmp273->tl=_tmp26C;_tmp273;}));};};}
# 1066
;_pop_handler();}else{void*_tmp26A=(void*)_exn_thrown;void*_tmp276=_tmp26A;void*_tmp27D;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp276)->tag == Cyc_Binding_BindingError){_LL20C: _LL20D:
# 1078
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp2B0->name,_tmp2B0->sc);{
struct _tuple0*_tmp277=_tmp2B0->name;struct _tuple0*_tmp278=_tmp277;union Cyc_Absyn_Nmspace _tmp27C;struct _dyneither_ptr*_tmp27B;_LL211: _tmp27C=_tmp278->f1;_tmp27B=_tmp278->f2;_LL212:;{
struct Cyc_Binding_ResolveNSEnv*_tmp279=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp27C);
# 1082
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp279->datatypedecls,_tmp27B) && _tmp2B0->fields == 0)
# 1084
goto _LL20B;
_tmp279->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp279->datatypedecls,_tmp27B,({struct Cyc_List_List*_tmp27A=_cycalloc(sizeof(*_tmp27A));_tmp27A->hd=_tmp2B0;_tmp27A->tl=0;_tmp27A;}));
# 1087
goto _LL20B;};};}else{_LL20E: _tmp27D=_tmp276;_LL20F:(int)_rethrow(_tmp27D);}_LL20B:;}};}{
# 1090
struct _tuple0*_tmp27E=_tmp2B0->name;struct _tuple0*_tmp27F=_tmp27E;union Cyc_Absyn_Nmspace _tmp28E;struct _dyneither_ptr*_tmp28D;_LL214: _tmp28E=_tmp27F->f1;_tmp28D=_tmp27F->f2;_LL215:;
if(_tmp2B0->fields != 0){
struct Cyc_List_List*_tmp280=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2B0->fields))->v;for(0;_tmp280 != 0;_tmp280=_tmp280->tl){
struct Cyc_Absyn_Datatypefield*_tmp281=(struct Cyc_Absyn_Datatypefield*)_tmp280->hd;
{struct Cyc_List_List*_tmp282=_tmp281->typs;for(0;_tmp282 != 0;_tmp282=_tmp282->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple12*)_tmp282->hd)).f2);}}
{union Cyc_Absyn_Nmspace _tmp283=(*_tmp281->name).f1;union Cyc_Absyn_Nmspace _tmp284=_tmp283;switch((_tmp284.Abs_n).tag){case 1: if((_tmp284.Rel_n).val == 0){_LL217: _LL218:
# 1098
 if(_tmp2B0->is_extensible)
(*_tmp281->name).f1=Cyc_Absyn_Abs_n((env->ns)->curr_ns,0);else{
# 1101
(*_tmp281->name).f1=(*_tmp2B0->name).f1;}
goto _LL216;}else{_LL219: _LL21A:
# 1104
({void*_tmp285=0;Cyc_Binding_err(_tmp281->loc,({const char*_tmp286="qualified datatype field declaratations are not allowed";_tag_dyneither(_tmp286,sizeof(char),56);}),_tag_dyneither(_tmp285,sizeof(void*),0));});
return;}case 2: _LL21B: _LL21C:
 goto _LL216;default: _LL21D: _LL21E:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp287=_cycalloc(sizeof(*_tmp287));_tmp287[0]=({struct Cyc_Core_Impossible_exn_struct _tmp288;_tmp288.tag=Cyc_Core_Impossible;_tmp288.f1=({const char*_tmp289="datatype field Loc_n or C_n";_tag_dyneither(_tmp289,sizeof(char),28);});_tmp288;});_tmp287;}));}_LL216:;}{
# 1109
struct Cyc_Binding_ResolveNSEnv*_tmp28A=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,(*_tmp281->name).f1);
_tmp28A->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp28A->ordinaries,(*_tmp281->name).f2,(void*)({struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_tmp28B=_cycalloc(sizeof(*_tmp28B));_tmp28B[0]=({struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct _tmp28C;_tmp28C.tag=2;_tmp28C.f1=_tmp2B0;_tmp28C.f2=_tmp281;_tmp28C;});_tmp28B;}));};}}
# 1114
goto _LL1DA;};case 9: _LL1ED: _tmp2B2=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp233)->f1;_tmp2B1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp233)->f2;_LL1EE:
# 1117
((void(*)(struct Cyc_Binding_NSCtxt*ctxt,struct _dyneither_ptr*subname,int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_enter_ns)(env->ns,_tmp2B2,1,Cyc_Binding_mt_renv);
Cyc_Binding_resolve_decls(env,_tmp2B1);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_ns)(env->ns);
goto _LL1DA;case 10: _LL1EF: _tmp2B4=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp233)->f1;_tmp2B3=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp233)->f2;_LL1F0:
# 1122
((void(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename))Cyc_Binding_enter_using)(d->loc,env->ns,_tmp2B4);
Cyc_Binding_resolve_decls(env,_tmp2B3);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_using)(env->ns);
goto _LL1DA;case 11: _LL1F1: _tmp2B5=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp233)->f1;_LL1F2: {
# 1128
int _tmp28F=env->in_cinclude;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,_tmp2B5);
env->in_cinclude=_tmp28F;
goto _LL1DA;}case 12: _LL1F3: _tmp2B7=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp233)->f1;_tmp2B6=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp233)->f2;_LL1F4:
# 1139
{struct Cyc_List_List*_tmp290=_tmp2B6;for(0;_tmp290 != 0;_tmp290=_tmp290->tl){
struct _tuple15*_tmp291=(struct _tuple15*)_tmp290->hd;struct _tuple15*_tmp292=_tmp291;unsigned int _tmp294;struct _tuple0*_tmp293;_LL220: _tmp294=_tmp292->f1;_tmp293=_tmp292->f2;_LL221:;
Cyc_Binding_absolutize_decl(_tmp294,env,_tmp293,Cyc_Absyn_ExternC);}}
# 1147
if(!Cyc_Binding_at_toplevel(env)){
({void*_tmp295=0;Cyc_Binding_err(loc,({const char*_tmp296="extern \"C include\" not at toplevel";_tag_dyneither(_tmp296,sizeof(char),35);}),_tag_dyneither(_tmp295,sizeof(void*),0));});
goto _LL1DA;}{
# 1152
struct Cyc_Binding_ResolveNSEnv*_tmp297=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)((env->ns)->ns_data,(env->ns)->curr_ns);
struct Cyc_Dict_Dict _tmp298=_tmp297->ordinaries;
int _tmp299=env->in_cinclude;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,_tmp2B7);{
struct Cyc_Dict_Dict _tmp29A=_tmp297->ordinaries;
struct Cyc_Dict_Dict _tmp29B=_tmp298;
{struct Cyc_List_List*_tmp29C=_tmp2B6;for(0;_tmp29C != 0;_tmp29C=_tmp29C->tl){
struct _tuple15*_tmp29D=(struct _tuple15*)_tmp29C->hd;struct _tuple15*_tmp29E=_tmp29D;unsigned int _tmp2A3;struct _dyneither_ptr*_tmp2A2;_LL223: _tmp2A3=_tmp29E->f1;_tmp2A2=(_tmp29E->f2)->f2;_LL224:;
if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp29A,_tmp2A2) || 
((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp298,_tmp2A2) && 
((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp298,_tmp2A2)== ((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp29A,_tmp2A2))
({struct Cyc_String_pa_PrintArg_struct _tmp2A1;_tmp2A1.tag=0;_tmp2A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2A2);({void*_tmp29F[1]={& _tmp2A1};Cyc_Binding_err(_tmp2A3,({const char*_tmp2A0="%s is exported but not defined";_tag_dyneither(_tmp2A0,sizeof(char),31);}),_tag_dyneither(_tmp29F,sizeof(void*),1));});});
_tmp29B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp29B,_tmp2A2,((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp29A,_tmp2A2));}}
# 1167
_tmp297->ordinaries=_tmp29B;
env->in_cinclude=_tmp299;
goto _LL1DA;};};case 13: _LL1F5: _LL1F6:
# 1171
 goto _LL1F8;default: _LL1F7: _LL1F8:
 goto _LL1DA;}_LL1DA:;}
# 1176
void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*env,struct Cyc_List_List*tds){
for(0;tds != 0;tds=tds->tl){
Cyc_Binding_resolve_decl(env,(struct Cyc_Absyn_Decl*)tds->hd);}}
# 1181
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds){
struct Cyc_Binding_Env*_tmp2B8=({struct Cyc_Binding_Env*_tmp2BC=_cycalloc(sizeof(*_tmp2BC));_tmp2BC->in_cinclude=0;_tmp2BC->ns=((struct Cyc_Binding_NSCtxt*(*)(int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_mt_nsctxt)(1,Cyc_Binding_mt_renv);_tmp2BC->local_vars=0;_tmp2BC;});
Cyc_Binding_resolve_decl(_tmp2B8,({struct Cyc_Absyn_Decl*_tmp2B9=_cycalloc(sizeof(*_tmp2B9));_tmp2B9->r=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp2BA=_cycalloc(sizeof(*_tmp2BA));_tmp2BA[0]=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp2BB;_tmp2BB.tag=6;_tmp2BB.f1=Cyc_Absyn_exn_tud;_tmp2BB;});_tmp2BA;});_tmp2B9->loc=0;_tmp2B9;}));
Cyc_Binding_resolve_decls(_tmp2B8,tds);}
