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
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
typedef struct Cyc_Position_Error*Cyc_Position_error_t;struct Cyc_Relations_Reln;
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
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 25 "warn.h"
void Cyc_Warn_vwarn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 27
void Cyc_Warn_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 29
void Cyc_Warn_flush_warnings();
# 31
void Cyc_Warn_err(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40 "binding.cyc"
typedef struct Cyc_Dict_Dict Cyc_Binding_dict_t;
typedef unsigned int Cyc_Binding_seg_t;char Cyc_Binding_BindingError[13]="BindingError";struct Cyc_Binding_BindingError_exn_struct{char*tag;};
# 45
int Cyc_Binding_warn_override=0;
# 49
typedef struct Cyc_List_List*Cyc_Binding_namespace_name_t;struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};
# 54
typedef void*Cyc_Binding_nsdirective_t;struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 60
typedef struct Cyc_Binding_NSCtxt*Cyc_Binding_nsctxt_t;
# 62
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*env,void*(*mkdata)(void*)){
void*_tmp0=mkdata(env);
return({struct Cyc_Binding_NSCtxt*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->curr_ns=0;_tmp1->availables=({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->hd=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3[0]=({struct Cyc_Binding_Namespace_Binding_NSDirective_struct _tmp4;_tmp4.tag=0;_tmp4.f1=0;_tmp4;});_tmp3;});_tmp2->tl=0;_tmp2;});_tmp1->ns_data=
# 66
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*),struct Cyc_List_List*k,void*v))Cyc_Dict_singleton)(Cyc_Absyn_varlist_cmp,0,_tmp0);_tmp1;});}
# 68
void*Cyc_Binding_get_ns_data(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns){
union Cyc_Absyn_Nmspace _tmp5=abs_ns;struct Cyc_List_List*_tmpA;struct Cyc_List_List*_tmp9;switch((_tmp5.Abs_n).tag){case 3: _LL1: _tmp9=(_tmp5.C_n).val;_LL2:
 _tmpA=_tmp9;goto _LL4;case 2: _LL3: _tmpA=(_tmp5.Abs_n).val;_LL4:
 return((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmpA);default: _LL5: _LL6:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=({struct Cyc_Core_Impossible_exn_struct _tmp7;_tmp7.tag=Cyc_Core_Impossible;_tmp7.f1=({const char*_tmp8="Binding:get_ns_data: relative ns";_tag_dyneither(_tmp8,sizeof(char),33);});_tmp7;});_tmp6;}));}_LL0:;}
# 86 "binding.cyc"
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct Cyc_List_List*rel_ns){
# 89
struct Cyc_List_List*_tmpB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ctxt->curr_ns,rel_ns);
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmpB))
return _tmpB;
{struct Cyc_List_List*_tmpC=ctxt->availables;for(0;_tmpC != 0;_tmpC=_tmpC->tl){
void*_tmpD=(void*)_tmpC->hd;void*_tmpE=_tmpD;struct Cyc_List_List*_tmp11;struct Cyc_List_List*_tmp10;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpE)->tag == 1){_LL8: _tmp10=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpE)->f1;_LL9:
 _tmp11=_tmp10;goto _LLB;}else{_LLA: _tmp11=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmpE)->f1;_LLB: {
# 96
struct Cyc_List_List*_tmpF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp11,rel_ns);
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmpF))
return _tmpF;
goto _LL7;}}_LL7:;}}
# 101
({struct Cyc_String_pa_PrintArg_struct _tmp14;_tmp14.tag=0;_tmp14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_str_sepstr(rel_ns,({const char*_tmp15="::";_tag_dyneither(_tmp15,sizeof(char),3);})));({void*_tmp12[1]={& _tmp14};Cyc_Warn_err(loc,({const char*_tmp13="namespace %s not found";_tag_dyneither(_tmp13,sizeof(char),23);}),_tag_dyneither(_tmp12,sizeof(void*),1));});});
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp16=_cycalloc_atomic(sizeof(*_tmp16));_tmp16[0]=({struct Cyc_Binding_BindingError_exn_struct _tmp17;_tmp17.tag=Cyc_Binding_BindingError;_tmp17;});_tmp16;}));}
# 106
void*Cyc_Binding_resolve_lookup(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(void*,struct _dyneither_ptr*)){
struct _tuple0*_tmp18=qv;union Cyc_Absyn_Nmspace _tmp45;struct _dyneither_ptr*_tmp44;_LLD: _tmp45=_tmp18->f1;_tmp44=_tmp18->f2;_LLE:;{
union Cyc_Absyn_Nmspace _tmp19=_tmp45;struct Cyc_List_List*_tmp43;struct Cyc_List_List*_tmp42;struct Cyc_List_List*_tmp41;switch((_tmp19.Abs_n).tag){case 1: if((_tmp19.Rel_n).val == 0){_LL10: _LL11:
# 110
{struct _handler_cons _tmp1A;_push_handler(& _tmp1A);{int _tmp1C=0;if(setjmp(_tmp1A.handler))_tmp1C=1;if(!_tmp1C){{void*_tmp1D=lookup(((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,ctxt->curr_ns),_tmp44);_npop_handler(0);return _tmp1D;};_pop_handler();}else{void*_tmp1B=(void*)_exn_thrown;void*_tmp1E=_tmp1B;void*_tmp1F;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1E)->tag == Cyc_Dict_Absent){_LL1B: _LL1C:
 goto _LL1A;}else{_LL1D: _tmp1F=_tmp1E;_LL1E:(int)_rethrow(_tmp1F);}_LL1A:;}};}
{struct Cyc_List_List*_tmp20=ctxt->availables;for(0;_tmp20 != 0;_tmp20=_tmp20->tl){
void*_tmp21=(void*)_tmp20->hd;void*_tmp22=_tmp21;struct Cyc_List_List*_tmp2A;struct Cyc_List_List*_tmp29;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp22)->tag == 1){_LL20: _tmp29=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp22)->f1;_LL21:
 _tmp2A=_tmp29;goto _LL23;}else{_LL22: _tmp2A=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp22)->f1;_LL23:
# 116
{struct _handler_cons _tmp23;_push_handler(& _tmp23);{int _tmp25=0;if(setjmp(_tmp23.handler))_tmp25=1;if(!_tmp25){{void*_tmp26=lookup(((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp2A),_tmp44);_npop_handler(0);return _tmp26;};_pop_handler();}else{void*_tmp24=(void*)_exn_thrown;void*_tmp27=_tmp24;void*_tmp28;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp27)->tag == Cyc_Dict_Absent){_LL25: _LL26:
 goto _LL24;}else{_LL27: _tmp28=_tmp27;_LL28:(int)_rethrow(_tmp28);}_LL24:;}};}
goto _LL1F;}_LL1F:;}}
# 120
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp2B=_cycalloc_atomic(sizeof(*_tmp2B));_tmp2B[0]=({struct Cyc_Binding_BindingError_exn_struct _tmp2C;_tmp2C.tag=Cyc_Binding_BindingError;_tmp2C;});_tmp2B;}));}else{_LL12: _tmp41=(_tmp19.Rel_n).val;_LL13: {
# 122
struct _handler_cons _tmp2D;_push_handler(& _tmp2D);{int _tmp2F=0;if(setjmp(_tmp2D.handler))_tmp2F=1;if(!_tmp2F){
{struct Cyc_List_List*_tmp30=Cyc_Binding_resolve_rel_ns(loc,ctxt,_tmp41);
void*_tmp31=lookup(((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp30),_tmp44);_npop_handler(0);return _tmp31;}
# 123
;_pop_handler();}else{void*_tmp2E=(void*)_exn_thrown;void*_tmp32=_tmp2E;void*_tmp35;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp32)->tag == Cyc_Dict_Absent){_LL2A: _LL2B:
# 125
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp33=_cycalloc_atomic(sizeof(*_tmp33));_tmp33[0]=({struct Cyc_Binding_BindingError_exn_struct _tmp34;_tmp34.tag=Cyc_Binding_BindingError;_tmp34;});_tmp33;}));}else{_LL2C: _tmp35=_tmp32;_LL2D:(int)_rethrow(_tmp35);}_LL29:;}};}}case 3: _LL14: _tmp42=(_tmp19.C_n).val;_LL15:
 _tmp43=_tmp42;goto _LL17;case 2: _LL16: _tmp43=(_tmp19.Abs_n).val;_LL17: {
# 128
struct _handler_cons _tmp36;_push_handler(& _tmp36);{int _tmp38=0;if(setjmp(_tmp36.handler))_tmp38=1;if(!_tmp38){{void*_tmp39=lookup(((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp43),_tmp44);_npop_handler(0);return _tmp39;};_pop_handler();}else{void*_tmp37=(void*)_exn_thrown;void*_tmp3A=_tmp37;void*_tmp3D;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp3A)->tag == Cyc_Dict_Absent){_LL2F: _LL30:
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp3B=_cycalloc_atomic(sizeof(*_tmp3B));_tmp3B[0]=({struct Cyc_Binding_BindingError_exn_struct _tmp3C;_tmp3C.tag=Cyc_Binding_BindingError;_tmp3C;});_tmp3B;}));}else{_LL31: _tmp3D=_tmp3A;_LL32:(int)_rethrow(_tmp3D);}_LL2E:;}};}default: _LL18: _LL19:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E[0]=({struct Cyc_Core_Impossible_exn_struct _tmp3F;_tmp3F.tag=Cyc_Core_Impossible;_tmp3F.f1=({const char*_tmp40="lookup local in global";_tag_dyneither(_tmp40,sizeof(char),23);});_tmp3F;});_tmp3E;}));}_LLF:;};}
# 134
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*ctxt,struct _dyneither_ptr*subname,void*env,void*(*mkdata)(void*)){
struct Cyc_List_List*_tmp46=ctxt->curr_ns;
struct Cyc_List_List*_tmp47=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp46,({struct Cyc_List_List*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->hd=subname;_tmp4B->tl=0;_tmp4B;}));
if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmp47))
ctxt->ns_data=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,void*v))Cyc_Dict_insert)(ctxt->ns_data,_tmp47,mkdata(env));
ctxt->curr_ns=_tmp47;
ctxt->availables=({struct Cyc_List_List*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->hd=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49[0]=({struct Cyc_Binding_Namespace_Binding_NSDirective_struct _tmp4A;_tmp4A.tag=0;_tmp4A.f1=_tmp47;_tmp4A;});_tmp49;});_tmp48->tl=ctxt->availables;_tmp48;});}
# 142
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C[0]=({struct Cyc_Core_Impossible_exn_struct _tmp4D;_tmp4D.tag=Cyc_Core_Impossible;_tmp4D.f1=({const char*_tmp4E="leaving topmost namespace";_tag_dyneither(_tmp4E,sizeof(char),26);});_tmp4D;});_tmp4C;}));{
void*_tmp4F=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_tmp50=_tmp4F;if(((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp50)->tag == 0){_LL34: _LL35:
# 147
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;
ctxt->curr_ns=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ctxt->curr_ns)))->tl);
goto _LL33;}else{_LL36: _LL37:
# 151
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51[0]=({struct Cyc_Core_Impossible_exn_struct _tmp52;_tmp52.tag=Cyc_Core_Impossible;_tmp52.f1=({const char*_tmp53="leaving using as namespace";_tag_dyneither(_tmp53,sizeof(char),27);});_tmp52;});_tmp51;}));}_LL33:;};}
# 154
void Cyc_Binding_enter_using(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename){
struct _tuple0*_tmp54=usename;union Cyc_Absyn_Nmspace _tmp65;struct _dyneither_ptr*_tmp64;_LL39: _tmp65=_tmp54->f1;_tmp64=_tmp54->f2;_LL3A:;{
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp55=_tmp65;struct Cyc_List_List*_tmp60;struct Cyc_List_List*_tmp5F;switch((_tmp55.Loc_n).tag){case 1: _LL3C: _tmp5F=(_tmp55.Rel_n).val;_LL3D:
# 159
 ns=Cyc_Binding_resolve_rel_ns(loc,ctxt,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp5F,({struct Cyc_List_List*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->hd=_tmp64;_tmp56->tl=0;_tmp56;})));{
struct Cyc_List_List*_tmp57=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);
(*usename).f1=Cyc_Absyn_Abs_n(_tmp57,0);
goto _LL3B;};case 2: _LL3E: _tmp60=(_tmp55.Abs_n).val;_LL3F:
# 164
 ns=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp60,({struct Cyc_List_List*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->hd=_tmp64;_tmp58->tl=0;_tmp58;}));
goto _LL3B;case 4: _LL40: _LL41:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59[0]=({struct Cyc_Core_Impossible_exn_struct _tmp5A;_tmp5A.tag=Cyc_Core_Impossible;_tmp5A.f1=({const char*_tmp5B="enter_using, Loc_n";_tag_dyneither(_tmp5B,sizeof(char),19);});_tmp5A;});_tmp59;}));default: _LL42: _LL43:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C[0]=({struct Cyc_Core_Impossible_exn_struct _tmp5D;_tmp5D.tag=Cyc_Core_Impossible;_tmp5D.f1=({const char*_tmp5E="enter_using, C_n";_tag_dyneither(_tmp5E,sizeof(char),17);});_tmp5D;});_tmp5C;}));}_LL3B:;}
# 169
ctxt->availables=({struct Cyc_List_List*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->hd=(void*)({struct Cyc_Binding_Using_Binding_NSDirective_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62[0]=({struct Cyc_Binding_Using_Binding_NSDirective_struct _tmp63;_tmp63.tag=1;_tmp63.f1=ns;_tmp63;});_tmp62;});_tmp61->tl=ctxt->availables;_tmp61;});};}
# 171
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66[0]=({struct Cyc_Core_Impossible_exn_struct _tmp67;_tmp67.tag=Cyc_Core_Impossible;_tmp67.f1=({const char*_tmp68="leaving topmost namespace as a using";_tag_dyneither(_tmp68,sizeof(char),37);});_tmp67;});_tmp66;}));{
void*_tmp69=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_tmp6A=_tmp69;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp6A)->tag == 1){_LL45: _LL46:
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;goto _LL44;}else{_LL47: _LL48:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B[0]=({struct Cyc_Core_Impossible_exn_struct _tmp6C;_tmp6C.tag=Cyc_Core_Impossible;_tmp6C.f1=({const char*_tmp6D="leaving namespace as using";_tag_dyneither(_tmp6D,sizeof(char),27);});_tmp6C;});_tmp6B;}));}_LL44:;};}struct Cyc_Binding_VarRes_Binding_Resolved_struct{int tag;void*f1;};struct Cyc_Binding_AggrRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Binding_EnumRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
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
# 221
static struct Cyc_Binding_ResolveNSEnv*Cyc_Binding_mt_renv(int ignore){
return({struct Cyc_Binding_ResolveNSEnv*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->aggrdecls=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp6E->datatypedecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp6E->enumdecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp6E->typedefs=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp6E->ordinaries=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp6E;});}
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
struct _tuple0*_tmp6F=qv;union Cyc_Absyn_Nmspace _tmp74;struct _dyneither_ptr*_tmp73;_LL4A: _tmp74=_tmp6F->f1;_tmp73=_tmp6F->f2;_LL4B:;{
union Cyc_Absyn_Nmspace _tmp70=_tmp74;switch((_tmp70.Rel_n).tag){case 4: _LL4D: _LL4E:
# 248
 if(Cyc_Binding_at_toplevel(env) || !((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp73))
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp71=_cycalloc_atomic(sizeof(*_tmp71));_tmp71[0]=({struct Cyc_Binding_BindingError_exn_struct _tmp72;_tmp72.tag=Cyc_Binding_BindingError;_tmp72;});_tmp71;}));
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp73);case 1: if((_tmp70.Rel_n).val == 0){_LL4F: _LL50:
# 252
 if(!Cyc_Binding_at_toplevel(env) && ((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp73))
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp73);
goto _LL52;}else{goto _LL51;}default: _LL51: _LL52:
# 256
 return((void*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qv,Cyc_Binding_lookup_ordinary_global);}_LL4C:;};}
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
# 277
void Cyc_Binding_absolutize_decl(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
union Cyc_Absyn_Nmspace _tmp75=(*qv).f1;union Cyc_Absyn_Nmspace _tmp76=_tmp75;switch((_tmp76.Abs_n).tag){case 1: if((_tmp76.Rel_n).val == 0){_LL54: _LL55:
# 280
 if(Cyc_Binding_at_toplevel(env))
(*qv).f1=Cyc_Absyn_Abs_n((env->ns)->curr_ns,Cyc_Binding_in_cinclude(env) || sc == Cyc_Absyn_ExternC);else{
# 283
(*qv).f1=Cyc_Absyn_Loc_n;}
goto _LL53;}else{goto _LL5A;}case 4: _LL56: _LL57:
 goto _LL53;case 2: _LL58: _LL59:
# 287
 if(!Cyc_Binding_at_toplevel(env))
goto _LL5B;
goto _LL53;default: _LL5A: _LL5B:
# 291
({struct Cyc_String_pa_PrintArg_struct _tmp79;_tmp79.tag=0;_tmp79.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(qv));({void*_tmp77[1]={& _tmp79};Cyc_Warn_err(loc,({const char*_tmp78="qualified names in declarations unimplemented (%s)";_tag_dyneither(_tmp78,sizeof(char),51);}),_tag_dyneither(_tmp77,sizeof(void*),1));});});}_LL53:;}
# 298
void Cyc_Binding_absolutize_topdecl(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
struct Cyc_Dict_Dict*_tmp7A=env->local_vars;
env->local_vars=0;
Cyc_Binding_absolutize_decl(loc,env,qv,sc);
env->local_vars=_tmp7A;}
# 305
void Cyc_Binding_check_warn_override(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*q){
struct _dyneither_ptr hides_what;
struct _handler_cons _tmp7B;_push_handler(& _tmp7B);{int _tmp7D=0;if(setjmp(_tmp7B.handler))_tmp7D=1;if(!_tmp7D){
{void*_tmp7E=Cyc_Binding_lookup_ordinary(loc,env,q);void*_tmp7F=_tmp7E;void*_tmp8B;switch(*((int*)_tmp7F)){case 0: _LL5D: _tmp8B=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmp7F)->f1;_LL5E:
# 310
 if(Cyc_Binding_at_toplevel(env)){
_npop_handler(0);return;}
{void*_tmp80=_tmp8B;switch(*((int*)_tmp80)){case 1: _LL68: _LL69:
 hides_what=({const char*_tmp81="global variable";_tag_dyneither(_tmp81,sizeof(char),16);});goto _LL67;case 4: _LL6A: _LL6B:
 hides_what=({const char*_tmp82="local variable";_tag_dyneither(_tmp82,sizeof(char),15);});goto _LL67;case 3: _LL6C: _LL6D:
 hides_what=({const char*_tmp83="parameter";_tag_dyneither(_tmp83,sizeof(char),10);});goto _LL67;case 5: _LL6E: _LL6F:
 hides_what=({const char*_tmp84="pattern variable";_tag_dyneither(_tmp84,sizeof(char),17);});goto _LL67;case 2: _LL70: _LL71:
 hides_what=({const char*_tmp85="function";_tag_dyneither(_tmp85,sizeof(char),9);});goto _LL67;default: _LL72: _LL73:
({void*_tmp86=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(({const char*_tmp87="shadowing free variable!";_tag_dyneither(_tmp87,sizeof(char),25);}),_tag_dyneither(_tmp86,sizeof(void*),0));});}_LL67:;}
# 320
goto _LL5C;case 1: _LL5F: _LL60:
 hides_what=({const char*_tmp88="struct constructor";_tag_dyneither(_tmp88,sizeof(char),19);});goto _LL5C;case 2: _LL61: _LL62:
 hides_what=({const char*_tmp89="datatype constructor";_tag_dyneither(_tmp89,sizeof(char),21);});goto _LL5C;case 3: _LL63: _LL64:
 goto _LL66;default: _LL65: _LL66:
 hides_what=({const char*_tmp8A="enum tag";_tag_dyneither(_tmp8A,sizeof(char),9);});goto _LL5C;}_LL5C:;}
# 326
({struct Cyc_String_pa_PrintArg_struct _tmp8E;_tmp8E.tag=0;_tmp8E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)hides_what);({void*_tmp8C[1]={& _tmp8E};Cyc_Warn_warn(loc,({const char*_tmp8D="declaration hides %s";_tag_dyneither(_tmp8D,sizeof(char),21);}),_tag_dyneither(_tmp8C,sizeof(void*),1));});});
_npop_handler(0);return;
# 308
;_pop_handler();}else{void*_tmp7C=(void*)_exn_thrown;void*_tmp8F=_tmp7C;void*_tmp90;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp8F)->tag == Cyc_Binding_BindingError){_LL75: _LL76:
# 328
 return;}else{_LL77: _tmp90=_tmp8F;_LL78:(int)_rethrow(_tmp90);}_LL74:;}};}
# 331
void Cyc_Binding_resolve_and_add_vardecl(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_Vardecl*vd){
Cyc_Binding_absolutize_decl(loc,env,vd->name,vd->sc);{
struct _tuple0*_tmp91=vd->name;struct _tuple0*_tmp92=_tmp91;union Cyc_Absyn_Nmspace _tmp9D;struct _dyneither_ptr*_tmp9C;_LL7A: _tmp9D=_tmp92->f1;_tmp9C=_tmp92->f2;_LL7B:;
Cyc_Binding_resolve_type(loc,env,vd->type);
if(Cyc_Binding_warn_override)
Cyc_Binding_check_warn_override(loc,env,vd->name);
if(!Cyc_Binding_at_toplevel(env))
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp9C,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp94;_tmp94.tag=0;_tmp94.f1=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp96;_tmp96.tag=4;_tmp96.f1=vd;_tmp96;});_tmp95;});_tmp94;});_tmp93;}));else{
# 341
struct Cyc_Binding_ResolveNSEnv*_tmp97=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp9D);
_tmp97->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp97->ordinaries,_tmp9C,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp99;_tmp99.tag=0;_tmp99.f1=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp9B;_tmp9B.tag=1;_tmp9B.f1=vd;_tmp9B;});_tmp9A;});_tmp99;});_tmp98;}));}};}
# 348
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Stmt*s){
void*_tmp9E=s->r;void*_tmp9F=_tmp9E;struct Cyc_Absyn_Stmt*_tmpB8;struct Cyc_List_List*_tmpB7;struct Cyc_Absyn_Exp*_tmpB6;struct Cyc_List_List*_tmpB5;struct Cyc_Absyn_Decl*_tmpB4;struct Cyc_Absyn_Stmt*_tmpB3;struct Cyc_Absyn_Exp*_tmpB2;struct Cyc_Absyn_Stmt*_tmpB1;struct Cyc_List_List*_tmpB0;struct Cyc_Absyn_Exp*_tmpAF;struct Cyc_Absyn_Exp*_tmpAE;struct Cyc_Absyn_Exp*_tmpAD;struct Cyc_Absyn_Stmt*_tmpAC;struct Cyc_Absyn_Exp*_tmpAB;struct Cyc_Absyn_Stmt*_tmpAA;struct Cyc_Absyn_Stmt*_tmpA9;struct Cyc_Absyn_Exp*_tmpA8;struct Cyc_Absyn_Exp*_tmpA7;struct Cyc_Absyn_Stmt*_tmpA6;struct Cyc_Absyn_Stmt*_tmpA5;struct Cyc_Absyn_Exp*_tmpA4;struct Cyc_Absyn_Stmt*_tmpA3;struct Cyc_Absyn_Stmt*_tmpA2;struct Cyc_Absyn_Exp*_tmpA1;switch(*((int*)_tmp9F)){case 0: _LL7D: _LL7E:
 return;case 1: _LL7F: _tmpA1=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp9F)->f1;_LL80:
 Cyc_Binding_resolve_exp(env,_tmpA1);return;case 2: _LL81: _tmpA3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp9F)->f1;_tmpA2=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp9F)->f2;_LL82:
 Cyc_Binding_resolve_stmt(env,_tmpA3);Cyc_Binding_resolve_stmt(env,_tmpA2);return;case 3: _LL83: _tmpA4=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp9F)->f1;_LL84:
 if((unsigned int)_tmpA4)Cyc_Binding_resolve_exp(env,_tmpA4);return;case 4: _LL85: _tmpA7=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9F)->f1;_tmpA6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9F)->f2;_tmpA5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9F)->f3;_LL86:
# 355
 Cyc_Binding_resolve_exp(env,_tmpA7);Cyc_Binding_resolve_stmt(env,_tmpA6);Cyc_Binding_resolve_stmt(env,_tmpA5);return;case 14: _LL87: _tmpA9=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp9F)->f1;_tmpA8=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp9F)->f2).f1;_LL88:
 _tmpAB=_tmpA8;_tmpAA=_tmpA9;goto _LL8A;case 5: _LL89: _tmpAB=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp9F)->f1).f1;_tmpAA=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp9F)->f2;_LL8A:
 Cyc_Binding_resolve_exp(env,_tmpAB);Cyc_Binding_resolve_stmt(env,_tmpAA);return;case 9: _LL8B: _tmpAF=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9F)->f1;_tmpAE=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9F)->f2).f1;_tmpAD=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9F)->f3).f1;_tmpAC=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9F)->f4;_LL8C:
# 359
 Cyc_Binding_resolve_exp(env,_tmpAF);Cyc_Binding_resolve_exp(env,_tmpAE);Cyc_Binding_resolve_exp(env,_tmpAD);
Cyc_Binding_resolve_stmt(env,_tmpAC);
return;case 6: _LL8D: _LL8E:
 goto _LL90;case 7: _LL8F: _LL90:
 goto _LL92;case 8: _LL91: _LL92:
 return;case 11: _LL93: _tmpB0=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp9F)->f1;_LL94:
# 366
 for(0;_tmpB0 != 0;_tmpB0=_tmpB0->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmpB0->hd);}
return;case 13: _LL95: _tmpB1=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp9F)->f2;_LL96:
 Cyc_Binding_resolve_stmt(env,_tmpB1);return;case 16: _LL97: _tmpB2=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp9F)->f1;_LL98:
 Cyc_Binding_resolve_exp(env,_tmpB2);return;case 12: _LL99: _tmpB4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp9F)->f1;_tmpB3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp9F)->f2;_LL9A: {
# 372
struct Cyc_Dict_Dict _tmpA0=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
Cyc_Binding_resolve_decl(env,_tmpB4);
Cyc_Binding_resolve_stmt(env,_tmpB3);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmpA0;
return;}case 10: _LL9B: _tmpB6=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp9F)->f1;_tmpB5=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp9F)->f2;_LL9C:
 Cyc_Binding_resolve_exp(env,_tmpB6);Cyc_Binding_resolve_scs(env,_tmpB5);return;default: _LL9D: _tmpB8=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp9F)->f1;_tmpB7=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp9F)->f2;_LL9E:
 Cyc_Binding_resolve_stmt(env,_tmpB8);Cyc_Binding_resolve_scs(env,_tmpB7);return;}_LL7C:;}struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 381
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){
void*_tmpB9=e->r;void*_tmpBA=_tmpB9;void*_tmp147;struct Cyc_List_List*_tmp146;struct Cyc_List_List*_tmp145;struct Cyc_Absyn_Datatypedecl*_tmp144;struct Cyc_Absyn_Datatypefield*_tmp143;struct Cyc_Absyn_Enumdecl*_tmp142;struct Cyc_Absyn_Enumfield*_tmp141;void*_tmp140;struct Cyc_Absyn_Enumfield*_tmp13F;struct Cyc_List_List*_tmp13E;void*_tmp13D;void*_tmp13C;void*_tmp13B;struct Cyc_Absyn_Stmt*_tmp13A;void*_tmp139;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_List_List*_tmp136;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp130;void**_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12B;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_List_List*_tmp118;struct Cyc_List_List*_tmp117;struct Cyc_Absyn_Exp*_tmp116;void*_tmp115;struct Cyc_List_List*_tmp114;struct Cyc_Absyn_Vardecl*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;struct _tuple0**_tmp110;struct Cyc_List_List*_tmp10F;struct Cyc_List_List*_tmp10E;struct Cyc_Absyn_Aggrdecl**_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_List_List*_tmp10B;int*_tmp10A;void**_tmp109;switch(*((int*)_tmpBA)){case 1: _LLA0: _tmp109=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_LLA1: {
# 384
void*_tmpBB=*_tmp109;void*_tmpBC=_tmpBB;struct _tuple0*_tmpD8;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpBC)->tag == 0){_LLF1: _tmpD8=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpBC)->f1;_LLF2: {
# 386
struct _handler_cons _tmpBD;_push_handler(& _tmpBD);{int _tmpBF=0;if(setjmp(_tmpBD.handler))_tmpBF=1;if(!_tmpBF){{void*_tmpC0=Cyc_Binding_lookup_ordinary(e->loc,env,_tmpD8);void*_tmpC1=_tmpC0;struct Cyc_Absyn_Aggrdecl*_tmpD2;void*_tmpD1;struct Cyc_Absyn_Enumfield*_tmpD0;struct Cyc_Absyn_Enumdecl*_tmpCF;struct Cyc_Absyn_Enumfield*_tmpCE;struct Cyc_Absyn_Datatypedecl*_tmpCD;struct Cyc_Absyn_Datatypefield*_tmpCC;void*_tmpCB;switch(*((int*)_tmpC1)){case 0: _LLF6: _tmpCB=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpC1)->f1;_LLF7:
*_tmp109=_tmpCB;_npop_handler(0);return;case 2: _LLF8: _tmpCD=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpC1)->f1;_tmpCC=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpC1)->f2;_LLF9:
 e->r=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpC3;_tmpC3.tag=30;_tmpC3.f1=0;_tmpC3.f2=_tmpCD;_tmpC3.f3=_tmpCC;_tmpC3;});_tmpC2;});_npop_handler(0);return;case 3: _LLFA: _tmpCF=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpC1)->f1;_tmpCE=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpC1)->f2;_LLFB:
 e->r=(void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4[0]=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpC5;_tmpC5.tag=31;_tmpC5.f1=_tmpCF;_tmpC5.f2=_tmpCE;_tmpC5;});_tmpC4;});_npop_handler(0);return;case 4: _LLFC: _tmpD1=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpC1)->f1;_tmpD0=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpC1)->f2;_LLFD:
 e->r=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpC7;_tmpC7.tag=32;_tmpC7.f1=_tmpD1;_tmpC7.f2=_tmpD0;_tmpC7;});_tmpC6;});_npop_handler(0);return;default: _LLFE: _tmpD2=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpC1)->f1;_LLFF:
# 392
({struct Cyc_String_pa_PrintArg_struct _tmpCA;_tmpCA.tag=0;_tmpCA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD8));({void*_tmpC8[1]={& _tmpCA};Cyc_Warn_err(e->loc,({const char*_tmpC9="bad occurrence of type name %s";_tag_dyneither(_tmpC9,sizeof(char),31);}),_tag_dyneither(_tmpC8,sizeof(void*),1));});});
_npop_handler(0);return;}_LLF5:;}
# 386
;_pop_handler();}else{void*_tmpBE=(void*)_exn_thrown;void*_tmpD3=_tmpBE;void*_tmpD7;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpD3)->tag == Cyc_Binding_BindingError){_LL101: _LL102:
# 396
({struct Cyc_String_pa_PrintArg_struct _tmpD6;_tmpD6.tag=0;_tmpD6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD8));({void*_tmpD4[1]={& _tmpD6};Cyc_Warn_err(e->loc,({const char*_tmpD5="undeclared identifier %s";_tag_dyneither(_tmpD5,sizeof(char),25);}),_tag_dyneither(_tmpD4,sizeof(void*),1));});});
return;}else{_LL103: _tmpD7=_tmpD3;_LL104:(int)_rethrow(_tmpD7);}_LL100:;}};}}else{_LLF3: _LLF4:
# 399
 return;}_LLF0:;}case 9: _LLA2: _tmp10C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_tmp10B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBA)->f2;_tmp10A=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBA)->f4;_LLA3:
# 402
*_tmp10A=1;
# 404
{struct Cyc_List_List*_tmpD9=_tmp10B;for(0;_tmpD9 != 0;_tmpD9=_tmpD9->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmpD9->hd);}}{
void*_tmpDA=_tmp10C->r;void*_tmpDB=_tmpDA;void**_tmpFA;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDB)->tag == 1){_LL106: _tmpFA=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL107: {
# 408
void*_tmpDC=*_tmpFA;void*_tmpDD=_tmpDC;struct _tuple0*_tmpF9;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpDD)->tag == 0){_LL10B: _tmpF9=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpDD)->f1;_LL10C: {
# 410
struct _handler_cons _tmpDE;_push_handler(& _tmpDE);{int _tmpE0=0;if(setjmp(_tmpDE.handler))_tmpE0=1;if(!_tmpE0){{void*_tmpE1=Cyc_Binding_lookup_ordinary(_tmp10C->loc,env,_tmpF9);void*_tmpE2=_tmpE1;struct Cyc_Absyn_Datatypedecl*_tmpF3;struct Cyc_Absyn_Datatypefield*_tmpF2;struct Cyc_Absyn_Aggrdecl*_tmpF1;void*_tmpF0;switch(*((int*)_tmpE2)){case 0: _LL110: _tmpF0=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpE2)->f1;_LL111:
# 412
*_tmpFA=_tmpF0;_npop_handler(0);return;case 1: _LL112: _tmpF1=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpE2)->f1;_LL113: {
# 414
struct Cyc_List_List*_tmpE3=0;
for(0;_tmp10B != 0;_tmp10B=_tmp10B->tl){
_tmpE3=({struct Cyc_List_List*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->hd=({struct _tuple10*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->f1=0;_tmpE5->f2=(struct Cyc_Absyn_Exp*)_tmp10B->hd;_tmpE5;});_tmpE4->tl=_tmpE3;_tmpE4;});}
e->r=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6[0]=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpE7;_tmpE7.tag=28;_tmpE7.f1=_tmpF1->name;_tmpE7.f2=0;_tmpE7.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpE3);_tmpE7.f4=_tmpF1;_tmpE7;});_tmpE6;});
_npop_handler(0);return;}case 2: _LL114: _tmpF3=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpE2)->f1;_tmpF2=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpE2)->f2;_LL115:
# 420
 if(_tmpF2->typs == 0)
# 422
({struct Cyc_String_pa_PrintArg_struct _tmpEA;_tmpEA.tag=0;_tmpEA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpF2->name));({void*_tmpE8[1]={& _tmpEA};Cyc_Warn_err(e->loc,({const char*_tmpE9="%s is a constant, not a function";_tag_dyneither(_tmpE9,sizeof(char),33);}),_tag_dyneither(_tmpE8,sizeof(void*),1));});});
e->r=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB[0]=({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpEC;_tmpEC.tag=30;_tmpEC.f1=_tmp10B;_tmpEC.f2=_tmpF3;_tmpEC.f3=_tmpF2;_tmpEC;});_tmpEB;});
_npop_handler(0);return;case 4: _LL116: _LL117:
 goto _LL119;default: _LL118: _LL119:
# 428
({struct Cyc_String_pa_PrintArg_struct _tmpEF;_tmpEF.tag=0;_tmpEF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpF9));({void*_tmpED[1]={& _tmpEF};Cyc_Warn_err(e->loc,({const char*_tmpEE="%s is an enum constructor, not a function";_tag_dyneither(_tmpEE,sizeof(char),42);}),_tag_dyneither(_tmpED,sizeof(void*),1));});});
_npop_handler(0);return;}_LL10F:;}
# 410
;_pop_handler();}else{void*_tmpDF=(void*)_exn_thrown;void*_tmpF4=_tmpDF;void*_tmpF8;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpF4)->tag == Cyc_Binding_BindingError){_LL11B: _LL11C:
# 432
({struct Cyc_String_pa_PrintArg_struct _tmpF7;_tmpF7.tag=0;_tmpF7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpF9));({void*_tmpF5[1]={& _tmpF7};Cyc_Warn_err(e->loc,({const char*_tmpF6="undeclared identifier %s";_tag_dyneither(_tmpF6,sizeof(char),25);}),_tag_dyneither(_tmpF5,sizeof(void*),1));});});
return;}else{_LL11D: _tmpF8=_tmpF4;_LL11E:(int)_rethrow(_tmpF8);}_LL11A:;}};}}else{_LL10D: _LL10E:
# 435
 return;}_LL10A:;}}else{_LL108: _LL109:
# 437
 Cyc_Binding_resolve_exp(env,_tmp10C);return;}_LL105:;};case 28: _LLA4: _tmp110=(struct _tuple0**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_tmp10F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpBA)->f2;_tmp10E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpBA)->f3;_tmp10D=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpBA)->f4;_LLA5:
# 441
 for(0;_tmp10E != 0;_tmp10E=_tmp10E->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp10E->hd)).f2);}{
struct _handler_cons _tmpFB;_push_handler(& _tmpFB);{int _tmpFD=0;if(setjmp(_tmpFB.handler))_tmpFD=1;if(!_tmpFD){
*_tmp10D=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(e->loc,env->ns,*_tmp110,Cyc_Binding_lookup_aggrdecl);
*_tmp110=((struct Cyc_Absyn_Aggrdecl*)_check_null(*_tmp10D))->name;
_npop_handler(0);return;
# 444
;_pop_handler();}else{void*_tmpFC=(void*)_exn_thrown;void*_tmpFE=_tmpFC;void*_tmp102;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpFE)->tag == Cyc_Binding_BindingError){_LL120: _LL121:
# 448
({struct Cyc_String_pa_PrintArg_struct _tmp101;_tmp101.tag=0;_tmp101.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*_tmp110));({void*_tmpFF[1]={& _tmp101};Cyc_Warn_err(e->loc,({const char*_tmp100="unbound struct/union name %s";_tag_dyneither(_tmp100,sizeof(char),29);}),_tag_dyneither(_tmpFF,sizeof(void*),1));});});
return;}else{_LL122: _tmp102=_tmpFE;_LL123:(int)_rethrow(_tmp102);}_LL11F:;}};};case 26: _LLA6: _tmp113=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_tmp112=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpBA)->f2;_tmp111=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpBA)->f3;_LLA7:
# 453
 Cyc_Binding_resolve_exp(env,_tmp112);
if(Cyc_Binding_at_toplevel(env)){
env->local_vars=({struct Cyc_Dict_Dict*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103[0]=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp103;});
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,_tmp113);
Cyc_Binding_resolve_exp(env,_tmp111);
env->local_vars=0;
return;}{
# 461
struct Cyc_Dict_Dict _tmp104=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,_tmp113);
Cyc_Binding_resolve_exp(env,_tmp111);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp104;
return;};case 35: _LLA8: _tmp114=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpBA)->f2;_LLA9:
# 469
 for(0;_tmp114 != 0;_tmp114=_tmp114->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp114->hd)).f2);}
return;case 27: _LLAA: _tmp116=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_tmp115=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpBA)->f2;_LLAB:
# 475
 Cyc_Binding_resolve_exp(env,_tmp116);
Cyc_Binding_resolve_type(e->loc,env,_tmp115);
return;case 0: _LLAC: _LLAD:
# 479
 return;case 23: _LLAE: _tmp117=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_LLAF:
# 481
 _tmp118=_tmp117;goto _LLB1;case 2: _LLB0: _tmp118=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBA)->f2;_LLB1:
# 483
 for(0;_tmp118 != 0;_tmp118=_tmp118->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmp118->hd);}
return;case 37: _LLB2: _tmp119=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_LLB3:
# 487
 _tmp11A=_tmp119;goto _LLB5;case 11: _LLB4: _tmp11A=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_LLB5:
 _tmp11B=_tmp11A;goto _LLB7;case 17: _LLB6: _tmp11B=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_LLB7:
 _tmp11C=_tmp11B;goto _LLB9;case 10: _LLB8: _tmp11C=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_LLB9:
 _tmp11D=_tmp11C;goto _LLBB;case 4: _LLBA: _tmp11D=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_LLBB:
 _tmp11E=_tmp11D;goto _LLBD;case 21: _LLBC: _tmp11E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_LLBD:
 _tmp11F=_tmp11E;goto _LLBF;case 20: _LLBE: _tmp11F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_LLBF:
 _tmp120=_tmp11F;goto _LLC1;case 14: _LLC0: _tmp120=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_LLC1:
 _tmp121=_tmp120;goto _LLC3;case 19: _LLC2: _tmp121=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_LLC3:
 Cyc_Binding_resolve_exp(env,_tmp121);return;case 34: _LLC4: _tmp123=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_tmp122=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpBA)->f2;_LLC5:
# 497
 _tmp125=_tmp123;_tmp124=_tmp122;goto _LLC7;case 8: _LLC6: _tmp125=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_tmp124=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpBA)->f2;_LLC7:
 _tmp127=_tmp125;_tmp126=_tmp124;goto _LLC9;case 3: _LLC8: _tmp127=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_tmp126=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpBA)->f3;_LLC9:
 _tmp129=_tmp127;_tmp128=_tmp126;goto _LLCB;case 22: _LLCA: _tmp129=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_tmp128=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBA)->f2;_LLCB:
 _tmp12B=_tmp129;_tmp12A=_tmp128;goto _LLCD;case 6: _LLCC: _tmp12B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_tmp12A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpBA)->f2;_LLCD:
 _tmp12D=_tmp12B;_tmp12C=_tmp12A;goto _LLCF;case 7: _LLCE: _tmp12D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_tmp12C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpBA)->f2;_LLCF:
 Cyc_Binding_resolve_exp(env,_tmp12D);Cyc_Binding_resolve_exp(env,_tmp12C);return;case 33: _LLD0: _tmp130=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBA)->f1).rgn;_tmp12F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBA)->f1).elt_type;_tmp12E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBA)->f1).num_elts;_LLD1:
# 505
 if(_tmp130 != 0)Cyc_Binding_resolve_exp(env,_tmp130);
if(_tmp12F != 0)Cyc_Binding_resolve_type(e->loc,env,*_tmp12F);
Cyc_Binding_resolve_exp(env,_tmp12E);
return;case 15: _LLD2: _tmp132=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_tmp131=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpBA)->f2;_LLD3:
# 511
 if(_tmp132 != 0)Cyc_Binding_resolve_exp(env,_tmp132);
Cyc_Binding_resolve_exp(env,_tmp131);
return;case 5: _LLD4: _tmp135=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_tmp134=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpBA)->f2;_tmp133=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpBA)->f3;_LLD5:
# 516
 Cyc_Binding_resolve_exp(env,_tmp135);Cyc_Binding_resolve_exp(env,_tmp134);Cyc_Binding_resolve_exp(env,_tmp133);return;case 12: _LLD6: _tmp137=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_tmp136=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBA)->f2;_LLD7:
# 519
 Cyc_Binding_resolve_exp(env,_tmp137);
for(0;_tmp136 != 0;_tmp136=_tmp136->tl){
Cyc_Binding_resolve_type(e->loc,env,(void*)_tmp136->hd);}
return;case 13: _LLD8: _tmp139=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_tmp138=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBA)->f2;_LLD9:
# 525
 Cyc_Binding_resolve_exp(env,_tmp138);Cyc_Binding_resolve_type(e->loc,env,_tmp139);return;case 36: _LLDA: _tmp13A=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_LLDB:
# 527
 Cyc_Binding_resolve_stmt(env,_tmp13A);return;case 38: _LLDC: _tmp13B=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_LLDD:
# 529
 _tmp13C=_tmp13B;goto _LLDF;case 18: _LLDE: _tmp13C=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_LLDF:
 _tmp13D=_tmp13C;goto _LLE1;case 16: _LLE0: _tmp13D=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_LLE1:
 Cyc_Binding_resolve_type(e->loc,env,_tmp13D);return;case 25: _LLE2: _tmp13E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_LLE3:
# 534
 for(0;_tmp13E != 0;_tmp13E=_tmp13E->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp13E->hd)).f2);}
return;case 24: _LLE4: _LLE5:
# 539
({void*_tmp105=0;Cyc_Warn_err(e->loc,({const char*_tmp106="binding: compound literals unimplemented";_tag_dyneither(_tmp106,sizeof(char),41);}),_tag_dyneither(_tmp105,sizeof(void*),0));});return;case 32: _LLE6: _tmp140=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_tmp13F=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpBA)->f2;_LLE7:
# 542
 Cyc_Binding_resolve_type(e->loc,env,_tmp140);return;case 31: _LLE8: _tmp142=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_tmp141=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpBA)->f2;_LLE9:
 return;case 30: _LLEA: _tmp145=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_tmp144=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpBA)->f2;_tmp143=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpBA)->f3;_LLEB:
# 545
 for(0;_tmp145 != 0;_tmp145=_tmp145->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmp145->hd);}
return;case 29: _LLEC: _tmp147=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_tmp146=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpBA)->f2;_LLED:
# 549
 Cyc_Binding_resolve_type(e->loc,env,_tmp147);
for(0;_tmp146 != 0;_tmp146=_tmp146->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp146->hd)).f2);}
return;default: _LLEE: _LLEF:
# 555
({void*_tmp107=0;Cyc_Warn_err(e->loc,({const char*_tmp108="asm expressions cannot occur within Cyclone code";_tag_dyneither(_tmp108,sizeof(char),49);}),_tag_dyneither(_tmp107,sizeof(void*),0));});}_LL9F:;}
# 559
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*env,struct Cyc_List_List*scs){
struct Cyc_Dict_Dict _tmp148=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp149=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp14A=_tmp149;struct Cyc_Absyn_Pat*_tmp14D;struct Cyc_Absyn_Exp*_tmp14C;struct Cyc_Absyn_Stmt*_tmp14B;_LL125: _tmp14D=_tmp14A->pattern;_tmp14C=_tmp14A->where_clause;_tmp14B=_tmp14A->body;_LL126:;
Cyc_Binding_resolve_pat(env,_tmp14D);
if(_tmp14C != 0)
Cyc_Binding_resolve_exp(env,_tmp14C);
Cyc_Binding_resolve_stmt(env,_tmp14B);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp148;}
# 569
return;}
# 571
void Cyc_Binding_resolve_aggrfields(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*fs){
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp14E=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_tmp14F=_tmp14E;struct _dyneither_ptr*_tmp155;struct Cyc_Absyn_Tqual _tmp154;void*_tmp153;struct Cyc_Absyn_Exp*_tmp152;struct Cyc_List_List*_tmp151;struct Cyc_Absyn_Exp*_tmp150;_LL128: _tmp155=_tmp14F->name;_tmp154=_tmp14F->tq;_tmp153=_tmp14F->type;_tmp152=_tmp14F->width;_tmp151=_tmp14F->attributes;_tmp150=_tmp14F->requires_clause;_LL129:;
Cyc_Binding_resolve_type(loc,env,_tmp153);
if(_tmp152 != 0)
Cyc_Binding_resolve_exp(env,_tmp152);
if(_tmp150 != 0)
Cyc_Binding_resolve_exp(env,_tmp150);}
# 580
return;}struct _tuple11{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 583
struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(struct _dyneither_ptr*(*f)(void*),int need_va_name,unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai){
# 589
struct Cyc_List_List*_tmp156=0;
for(0;args != 0;args=args->tl){
struct _tuple11*_tmp157=(struct _tuple11*)args->hd;struct _tuple11*_tmp158=_tmp157;void*_tmp15F;struct Cyc_Absyn_Tqual _tmp15E;void*_tmp15D;_LL12B: _tmp15F=_tmp158->f1;_tmp15E=_tmp158->f2;_tmp15D=_tmp158->f3;_LL12C:;{
struct _dyneither_ptr*_tmp159=f(_tmp15F);
if(_tmp159 == 0)continue;{
struct Cyc_Absyn_Vardecl*_tmp15A=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C->f1=Cyc_Absyn_Loc_n;_tmp15C->f2=_tmp159;_tmp15C;}),_tmp15D,0);
_tmp15A->tq=_tmp15E;
_tmp156=({struct Cyc_List_List*_tmp15B=_cycalloc(sizeof(*_tmp15B));_tmp15B->hd=_tmp15A;_tmp15B->tl=_tmp156;_tmp15B;});};};}
# 598
if(vai != 0){
struct Cyc_Absyn_VarargInfo _tmp160=*vai;struct Cyc_Absyn_VarargInfo _tmp161=_tmp160;struct _dyneither_ptr*_tmp16B;struct Cyc_Absyn_Tqual _tmp16A;void*_tmp169;int _tmp168;_LL12E: _tmp16B=_tmp161.name;_tmp16A=_tmp161.tq;_tmp169=_tmp161.type;_tmp168=_tmp161.inject;_LL12F:;
if(_tmp16B == 0){
if(need_va_name)
({void*_tmp162=0;Cyc_Warn_err(loc,({const char*_tmp163="missing name for vararg";_tag_dyneither(_tmp163,sizeof(char),24);}),_tag_dyneither(_tmp162,sizeof(void*),0));});}else{
# 605
void*_tmp164=Cyc_Absyn_dyneither_typ(_tmp169,(void*)& Cyc_Absyn_HeapRgn_val,_tmp16A,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl*_tmp165=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167->f1=Cyc_Absyn_Loc_n;_tmp167->f2=_tmp16B;_tmp167;}),_tmp164,0);
_tmp156=({struct Cyc_List_List*_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166->hd=_tmp165;_tmp166->tl=_tmp156;_tmp166;});}}
# 610
_tmp156=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp156);
return _tmp156;}
# 613
struct _dyneither_ptr*Cyc_Binding_get_vds_fndecl(struct _dyneither_ptr*v){return v;}
struct _dyneither_ptr*Cyc_Binding_get_vds_fntype(struct _dyneither_ptr*v){return v;}
# 616
void Cyc_Binding_resolve_function_stuff(unsigned int loc,struct Cyc_Binding_Env*env,void*eff,void*rettyp,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai,struct Cyc_List_List*rpo,struct Cyc_Absyn_Exp*req_clause,struct Cyc_Absyn_Exp*ens_clause){
# 620
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
# 632
struct Cyc_Dict_Dict _tmp16C=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
struct _dyneither_ptr*_tmp16D=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"return_value",sizeof(char),13);
struct Cyc_Absyn_Vardecl*_tmp16E=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp173=_cycalloc(sizeof(*_tmp173));_tmp173->f1=Cyc_Absyn_Loc_n;_tmp173->f2=_tmp16D;_tmp173;}),rettyp,0);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp16D,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp170;_tmp170.tag=0;_tmp170.f1=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp172;_tmp172.tag=4;_tmp172.f1=_tmp16E;_tmp172;});_tmp171;});_tmp170;});_tmp16F;}));
# 637
Cyc_Binding_resolve_exp(env,ens_clause);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp16C;}}struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 647
void Cyc_Binding_resolve_type(unsigned int loc,struct Cyc_Binding_Env*env,void*t){
void*_tmp175=t;struct Cyc_Absyn_Exp*_tmp1F2;struct Cyc_Absyn_Exp*_tmp1F1;void*_tmp1F0;void*_tmp1EF;void*_tmp1EE;void*_tmp1ED;void*_tmp1EC;void*_tmp1EB;struct Cyc_List_List*_tmp1EA;struct Cyc_Absyn_FnInfo _tmp1E9;void*_tmp1E8;struct Cyc_Absyn_Exp*_tmp1E7;struct Cyc_List_List*_tmp1E6;void*_tmp1E5;void*_tmp1E4;union Cyc_Absyn_Constraint*_tmp1E3;void*_tmp1E2;struct Cyc_List_List*_tmp1E1;struct Cyc_List_List*_tmp1E0;struct Cyc_Absyn_TypeDecl*_tmp1DF;void***_tmp1DE;struct _tuple0**_tmp1DD;struct Cyc_List_List*_tmp1DC;struct Cyc_Absyn_Typedefdecl**_tmp1DB;struct _tuple0**_tmp1DA;struct Cyc_Absyn_Enumdecl*_tmp1D9;union Cyc_Absyn_AggrInfoU*_tmp1D8;struct Cyc_List_List*_tmp1D7;union Cyc_Absyn_DatatypeFieldInfoU*_tmp1D6;struct Cyc_List_List*_tmp1D5;union Cyc_Absyn_DatatypeInfoU*_tmp1D4;struct Cyc_List_List*_tmp1D3;switch(*((int*)_tmp175)){case 3: _LL131: _tmp1D4=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp175)->f1).datatype_info;_tmp1D3=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp175)->f1).targs;_LL132:
# 650
 for(0;_tmp1D3 != 0;_tmp1D3=_tmp1D3->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1D3->hd);}{
union Cyc_Absyn_DatatypeInfoU _tmp176=*_tmp1D4;union Cyc_Absyn_DatatypeInfoU _tmp177=_tmp176;struct _tuple0*_tmp180;int _tmp17F;if((_tmp177.UnknownDatatype).tag == 1){_LL16C: _tmp180=((_tmp177.UnknownDatatype).val).name;_tmp17F=((_tmp177.UnknownDatatype).val).is_extensible;_LL16D: {
# 654
struct _handler_cons _tmp178;_push_handler(& _tmp178);{int _tmp17A=0;if(setjmp(_tmp178.handler))_tmp17A=1;if(!_tmp17A){
{struct Cyc_Absyn_Datatypedecl*_tmp17B=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp180,Cyc_Binding_lookup_datatypedecl))->hd;
*_tmp1D4=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp17C;_tmp17C.name=_tmp17B->name;_tmp17C.is_extensible=_tmp17F;_tmp17C;}));
_npop_handler(0);return;}
# 655
;_pop_handler();}else{void*_tmp179=(void*)_exn_thrown;void*_tmp17D=_tmp179;void*_tmp17E;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp17D)->tag == Cyc_Binding_BindingError){_LL171: _LL172:
# 659
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp180,Cyc_Absyn_Public);return;}else{_LL173: _tmp17E=_tmp17D;_LL174:(int)_rethrow(_tmp17E);}_LL170:;}};}}else{_LL16E: _LL16F:
 return;}_LL16B:;};case 4: _LL133: _tmp1D6=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp175)->f1).field_info;_tmp1D5=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp175)->f1).targs;_LL134:
# 663
 for(0;_tmp1D5 != 0;_tmp1D5=_tmp1D5->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1D5->hd);}{
union Cyc_Absyn_DatatypeFieldInfoU _tmp181=*_tmp1D6;union Cyc_Absyn_DatatypeFieldInfoU _tmp182=_tmp181;struct _tuple0*_tmp196;struct _tuple0*_tmp195;int _tmp194;if((_tmp182.UnknownDatatypefield).tag == 1){_LL176: _tmp196=((_tmp182.UnknownDatatypefield).val).datatype_name;_tmp195=((_tmp182.UnknownDatatypefield).val).field_name;_tmp194=((_tmp182.UnknownDatatypefield).val).is_extensible;_LL177:
# 669
{struct _handler_cons _tmp183;_push_handler(& _tmp183);{int _tmp185=0;if(setjmp(_tmp183.handler))_tmp185=1;if(!_tmp185){
{void*_tmp186=Cyc_Binding_lookup_ordinary(loc,env,_tmp195);void*_tmp187=_tmp186;struct Cyc_Absyn_Datatypedecl*_tmp18E;struct Cyc_Absyn_Datatypefield*_tmp18D;if(((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp187)->tag == 2){_LL17B: _tmp18E=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp187)->f1;_tmp18D=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp187)->f2;_LL17C: {
# 672
struct Cyc_Absyn_Datatypedecl*_tmp188=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp196,Cyc_Binding_lookup_datatypedecl))->hd;
if(Cyc_Absyn_qvar_cmp(_tmp18E->name,_tmp188->name)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp18B;_tmp18B.tag=0;_tmp18B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp18D->name));({void*_tmp189[1]={& _tmp18B};Cyc_Warn_err(loc,({const char*_tmp18A="shadowed datatype field %s not yet implemented properly";_tag_dyneither(_tmp18A,sizeof(char),56);}),_tag_dyneither(_tmp189,sizeof(void*),1));});});
_npop_handler(0);return;}
# 678
*_tmp1D6=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp18C;_tmp18C.datatype_name=_tmp18E->name;_tmp18C.field_name=_tmp18D->name;_tmp18C.is_extensible=_tmp194;_tmp18C;}));
# 680
_npop_handler(0);return;}}else{_LL17D: _LL17E:
 goto _LL17A;}_LL17A:;}
# 670
;_pop_handler();}else{void*_tmp184=(void*)_exn_thrown;void*_tmp18F=_tmp184;void*_tmp190;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp18F)->tag == Cyc_Binding_BindingError){_LL180: _LL181:
# 683
 goto _LL17F;}else{_LL182: _tmp190=_tmp18F;_LL183:(int)_rethrow(_tmp190);}_LL17F:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp193;_tmp193.tag=0;_tmp193.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp195));({void*_tmp191[1]={& _tmp193};Cyc_Warn_err(loc,({const char*_tmp192="%s is not a datatype field";_tag_dyneither(_tmp192,sizeof(char),27);}),_tag_dyneither(_tmp191,sizeof(void*),1));});});
return;}else{_LL178: _LL179:
 return;}_LL175:;};case 11: _LL135: _tmp1D8=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp175)->f1).aggr_info;_tmp1D7=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp175)->f1).targs;_LL136:
# 690
 for(0;_tmp1D7 != 0;_tmp1D7=_tmp1D7->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1D7->hd);}{
union Cyc_Absyn_AggrInfoU _tmp197=*_tmp1D8;union Cyc_Absyn_AggrInfoU _tmp198=_tmp197;enum Cyc_Absyn_AggrKind _tmp1A5;struct _tuple0*_tmp1A4;struct Cyc_Core_Opt*_tmp1A3;if((_tmp198.UnknownAggr).tag == 1){_LL185: _tmp1A5=((_tmp198.UnknownAggr).val).f1;_tmp1A4=((_tmp198.UnknownAggr).val).f2;_tmp1A3=((_tmp198.UnknownAggr).val).f3;_LL186: {
# 694
struct _handler_cons _tmp199;_push_handler(& _tmp199);{int _tmp19B=0;if(setjmp(_tmp199.handler))_tmp19B=1;if(!_tmp19B){
{struct Cyc_Absyn_Aggrdecl*_tmp19C=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp1A4,Cyc_Binding_lookup_aggrdecl);
if(_tmp19C->kind != _tmp1A5)
({void*_tmp19D=0;Cyc_Warn_err(loc,({const char*_tmp19E="struct vs. union mismatch with earlier declaration";_tag_dyneither(_tmp19E,sizeof(char),51);}),_tag_dyneither(_tmp19D,sizeof(void*),0));});
if(((_tmp1A5 == Cyc_Absyn_UnionA  && _tmp1A3 != 0) && _tmp19C->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp19C->impl))->tagged != (int)_tmp1A3->v)
# 700
({void*_tmp19F=0;Cyc_Warn_err(loc,({const char*_tmp1A0="@tagged mismatch with earlier declaration";_tag_dyneither(_tmp1A0,sizeof(char),42);}),_tag_dyneither(_tmp19F,sizeof(void*),0));});
*_tmp1D8=Cyc_Absyn_UnknownAggr(_tmp1A5,_tmp19C->name,_tmp1A3);
_npop_handler(0);return;}
# 695
;_pop_handler();}else{void*_tmp19A=(void*)_exn_thrown;void*_tmp1A1=_tmp19A;void*_tmp1A2;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1A1)->tag == Cyc_Binding_BindingError){_LL18A: _LL18B:
# 704
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp1A4,Cyc_Absyn_Public);return;}else{_LL18C: _tmp1A2=_tmp1A1;_LL18D:(int)_rethrow(_tmp1A2);}_LL189:;}};}}else{_LL187: _LL188:
 return;}_LL184:;};case 13: _LL137: _tmp1DA=(struct _tuple0**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp175)->f1;_tmp1D9=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp175)->f2;_LL138:
# 708
 if(_tmp1D9 != 0)
return;{
struct _handler_cons _tmp1A6;_push_handler(& _tmp1A6);{int _tmp1A8=0;if(setjmp(_tmp1A6.handler))_tmp1A8=1;if(!_tmp1A8){
{struct Cyc_Absyn_Enumdecl*_tmp1A9=((struct Cyc_Absyn_Enumdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Enumdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*_tmp1DA,Cyc_Binding_lookup_enumdecl);
*_tmp1DA=_tmp1A9->name;
_npop_handler(0);return;}
# 711
;_pop_handler();}else{void*_tmp1A7=(void*)_exn_thrown;void*_tmp1AA=_tmp1A7;void*_tmp1AB;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1AA)->tag == Cyc_Binding_BindingError){_LL18F: _LL190:
# 715
 Cyc_Binding_absolutize_topdecl(loc,env,*_tmp1DA,Cyc_Absyn_Public);return;}else{_LL191: _tmp1AB=_tmp1AA;_LL192:(int)_rethrow(_tmp1AB);}_LL18E:;}};};case 17: _LL139: _tmp1DD=(struct _tuple0**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp175)->f1;_tmp1DC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp175)->f2;_tmp1DB=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp175)->f3;_LL13A:
# 718
 for(0;_tmp1DC != 0;_tmp1DC=_tmp1DC->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1DC->hd);}
{struct _handler_cons _tmp1AC;_push_handler(& _tmp1AC);{int _tmp1AE=0;if(setjmp(_tmp1AC.handler))_tmp1AE=1;if(!_tmp1AE){
{struct Cyc_Absyn_Typedefdecl*_tmp1AF=((struct Cyc_Absyn_Typedefdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*_tmp1DD,Cyc_Binding_lookup_typedefdecl);
# 723
*_tmp1DD=_tmp1AF->name;
_npop_handler(0);return;}
# 721
;_pop_handler();}else{void*_tmp1AD=(void*)_exn_thrown;void*_tmp1B0=_tmp1AD;void*_tmp1B1;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1B0)->tag == Cyc_Binding_BindingError){_LL194: _LL195:
# 725
 goto _LL193;}else{_LL196: _tmp1B1=_tmp1B0;_LL197:(int)_rethrow(_tmp1B1);}_LL193:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp1B4;_tmp1B4.tag=0;_tmp1B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*_tmp1DD));({void*_tmp1B2[1]={& _tmp1B4};Cyc_Warn_err(loc,({const char*_tmp1B3="unbound typdef name %s";_tag_dyneither(_tmp1B3,sizeof(char),23);}),_tag_dyneither(_tmp1B2,sizeof(void*),1));});});
return;case 26: _LL13B: _tmp1DF=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp175)->f1;_tmp1DE=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp175)->f2;_LL13C: {
# 731
struct Cyc_Dict_Dict*_tmp1B5=env->local_vars;
env->local_vars=0;
{void*_tmp1B6=_tmp1DF->r;void*_tmp1B7=_tmp1B6;struct Cyc_Absyn_Datatypedecl*_tmp1C3;struct Cyc_Absyn_Enumdecl*_tmp1C2;struct Cyc_Absyn_Aggrdecl*_tmp1C1;switch(*((int*)_tmp1B7)){case 0: _LL199: _tmp1C1=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp1B7)->f1;_LL19A:
# 735
 Cyc_Binding_resolve_decl(env,({struct Cyc_Absyn_Decl*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8->r=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));_tmp1B9[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp1BA;_tmp1BA.tag=5;_tmp1BA.f1=_tmp1C1;_tmp1BA;});_tmp1B9;});_tmp1B8->loc=_tmp1DF->loc;_tmp1B8;}));goto _LL198;case 1: _LL19B: _tmp1C2=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp1B7)->f1;_LL19C:
# 737
 Cyc_Binding_resolve_decl(env,({struct Cyc_Absyn_Decl*_tmp1BB=_cycalloc(sizeof(*_tmp1BB));_tmp1BB->r=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1BC=_cycalloc(sizeof(*_tmp1BC));_tmp1BC[0]=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp1BD;_tmp1BD.tag=7;_tmp1BD.f1=_tmp1C2;_tmp1BD;});_tmp1BC;});_tmp1BB->loc=_tmp1DF->loc;_tmp1BB;}));goto _LL198;default: _LL19D: _tmp1C3=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp1B7)->f1;_LL19E:
# 739
 Cyc_Binding_resolve_decl(env,({struct Cyc_Absyn_Decl*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE->r=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1BF=_cycalloc(sizeof(*_tmp1BF));_tmp1BF[0]=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp1C0;_tmp1C0.tag=6;_tmp1C0.f1=_tmp1C3;_tmp1C0;});_tmp1BF;});_tmp1BE->loc=_tmp1DF->loc;_tmp1BE;}));goto _LL198;}_LL198:;}
# 741
env->local_vars=_tmp1B5;
return;}case 12: _LL13D: _tmp1E0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp175)->f2;_LL13E:
# 748
 Cyc_Binding_resolve_aggrfields(loc,env,_tmp1E0);
return;case 14: _LL13F: _tmp1E1=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp175)->f1;_LL140:
# 753
 for(0;_tmp1E1 != 0;_tmp1E1=_tmp1E1->tl){
struct Cyc_Absyn_Enumfield*_tmp1C4=(struct Cyc_Absyn_Enumfield*)_tmp1E1->hd;
if(_tmp1C4->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp1C4->tag));}
# 758
return;case 1: _LL141: _tmp1E2=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp175)->f2;_LL142:
 if(_tmp1E2 != 0)Cyc_Binding_resolve_type(loc,env,_tmp1E2);return;case 28: _LL143: _LL144:
# 761
 goto _LL146;case 0: _LL145: _LL146:
 goto _LL148;case 6: _LL147: _LL148:
 goto _LL14A;case 7: _LL149: _LL14A:
 goto _LL14C;case 20: _LL14B: _LL14C:
 goto _LL14E;case 21: _LL14D: _LL14E:
 goto _LL150;case 22: _LL14F: _LL150:
 goto _LL152;case 2: _LL151: _LL152:
 return;case 5: _LL153: _tmp1E5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp175)->f1).elt_typ;_tmp1E4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp175)->f1).ptr_atts).rgn;_tmp1E3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp175)->f1).ptr_atts).bounds;_LL154:
# 771
 Cyc_Binding_resolve_type(loc,env,_tmp1E5);
Cyc_Binding_resolve_type(loc,env,_tmp1E4);
while(1){
union Cyc_Absyn_Constraint*_tmp1C5=_tmp1E3;void*_tmp1C9;union Cyc_Absyn_Constraint*_tmp1C8;switch((((union Cyc_Absyn_Constraint*)_tmp1C5)->Forward_constr).tag){case 3: _LL1A0: _LL1A1:
 return;case 2: _LL1A2: _tmp1C8=(_tmp1C5->Forward_constr).val;_LL1A3:
 _tmp1E3=_tmp1C8;continue;default: _LL1A4: _tmp1C9=(void*)(_tmp1C5->Eq_constr).val;_LL1A5: {
# 778
void*_tmp1C6=_tmp1C9;struct Cyc_Absyn_Exp*_tmp1C7;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1C6)->tag == 0){_LL1A7: _LL1A8:
 return;}else{_LL1A9: _tmp1C7=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp1C6)->f1;_LL1AA:
 Cyc_Binding_resolve_exp(env,_tmp1C7);return;}_LL1A6:;}}_LL19F:;}case 10: _LL155: _tmp1E6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp175)->f1;_LL156:
# 784
 for(0;_tmp1E6 != 0;_tmp1E6=_tmp1E6->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple12*)_tmp1E6->hd)).f2);}
return;case 8: _LL157: _tmp1E8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp175)->f1).elt_type;_tmp1E7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp175)->f1).num_elts;_LL158:
# 788
 Cyc_Binding_resolve_type(loc,env,_tmp1E8);
if(_tmp1E7 != 0)Cyc_Binding_resolve_exp(env,_tmp1E7);
return;case 9: _LL159: _tmp1E9=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp175)->f1;_LL15A: {
# 795
struct Cyc_List_List*_tmp1CA=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),int need_va_name,unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai))Cyc_Binding_get_fun_vardecls)(Cyc_Binding_get_vds_fntype,0,loc,env,_tmp1E9.args,_tmp1E9.cyc_varargs);
# 798
struct Cyc_Dict_Dict*_tmp1CB=env->local_vars;
if(_tmp1CB != 0)
env->local_vars=({struct Cyc_Dict_Dict*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC[0]=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp1CC;});else{
# 802
env->local_vars=({struct Cyc_Dict_Dict*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD[0]=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp1CD;});}
{struct Cyc_List_List*_tmp1CE=_tmp1CA;for(0;_tmp1CE != 0;_tmp1CE=_tmp1CE->tl){
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),(*((struct Cyc_Absyn_Vardecl*)_tmp1CE->hd)->name).f2,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp1D0;_tmp1D0.tag=0;_tmp1D0.f1=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp1D2;_tmp1D2.tag=3;_tmp1D2.f1=(struct Cyc_Absyn_Vardecl*)_tmp1CE->hd;_tmp1D2;});_tmp1D1;});_tmp1D0;});_tmp1CF;}));}}
# 807
((void(*)(unsigned int loc,struct Cyc_Binding_Env*env,void*eff,void*rettyp,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai,struct Cyc_List_List*rpo,struct Cyc_Absyn_Exp*req_clause,struct Cyc_Absyn_Exp*ens_clause))Cyc_Binding_resolve_function_stuff)(loc,env,_tmp1E9.effect,_tmp1E9.ret_typ,_tmp1E9.args,_tmp1E9.cyc_varargs,_tmp1E9.rgn_po,_tmp1E9.requires_clause,_tmp1E9.ensures_clause);
# 811
env->local_vars=_tmp1CB;
return;}case 24: _LL15B: _tmp1EA=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp175)->f1;_LL15C:
# 815
 for(0;_tmp1EA != 0;_tmp1EA=_tmp1EA->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1EA->hd);}
return;case 19: _LL15D: _tmp1EB=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp175)->f1;_LL15E:
# 819
 _tmp1EC=_tmp1EB;goto _LL160;case 23: _LL15F: _tmp1EC=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp175)->f1;_LL160:
 _tmp1ED=_tmp1EC;goto _LL162;case 25: _LL161: _tmp1ED=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp175)->f1;_LL162:
 _tmp1EE=_tmp1ED;goto _LL164;case 15: _LL163: _tmp1EE=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp175)->f1;_LL164:
 Cyc_Binding_resolve_type(loc,env,_tmp1EE);return;case 16: _LL165: _tmp1F0=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp175)->f1;_tmp1EF=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp175)->f2;_LL166:
# 825
 Cyc_Binding_resolve_type(loc,env,_tmp1F0);Cyc_Binding_resolve_type(loc,env,_tmp1EF);return;case 18: _LL167: _tmp1F1=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp175)->f1;_LL168:
# 827
 _tmp1F2=_tmp1F1;goto _LL16A;default: _LL169: _tmp1F2=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp175)->f1;_LL16A:
 Cyc_Binding_resolve_exp(env,_tmp1F2);return;}_LL130:;}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 832
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Pat*p){
struct Cyc_Absyn_Pat*_tmp1F3=p;void*_tmp24D;unsigned int _tmp24C;_LL1AC: _tmp24D=_tmp1F3->r;_tmp24C=_tmp1F3->loc;_LL1AD:;{
void*_tmp1F4=_tmp24D;struct Cyc_List_List*_tmp24B;struct Cyc_Absyn_Exp*_tmp24A;struct Cyc_List_List*_tmp249;struct Cyc_Absyn_Pat*_tmp248;struct Cyc_Absyn_Vardecl*_tmp247;struct Cyc_Absyn_Vardecl*_tmp246;struct Cyc_Absyn_Vardecl*_tmp245;struct Cyc_Absyn_Pat*_tmp244;struct Cyc_Absyn_Vardecl*_tmp243;struct Cyc_Absyn_Pat*_tmp242;struct Cyc_List_List*_tmp241;struct Cyc_List_List*_tmp240;struct _tuple0*_tmp23F;struct Cyc_List_List*_tmp23E;struct Cyc_List_List*_tmp23D;struct Cyc_List_List*_tmp23C;int _tmp23B;struct _tuple0*_tmp23A;struct Cyc_List_List*_tmp239;int _tmp238;struct _tuple0*_tmp237;switch(*((int*)_tmp1F4)){case 15: _LL1AF: _tmp237=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1F4)->f1;_LL1B0:
# 836
{struct _handler_cons _tmp1F5;_push_handler(& _tmp1F5);{int _tmp1F7=0;if(setjmp(_tmp1F5.handler))_tmp1F7=1;if(!_tmp1F7){{void*_tmp1F8=Cyc_Binding_lookup_ordinary(_tmp24C,env,_tmp237);void*_tmp1F9=_tmp1F8;void*_tmp207;struct Cyc_Absyn_Enumfield*_tmp206;struct Cyc_Absyn_Enumdecl*_tmp205;struct Cyc_Absyn_Enumfield*_tmp204;struct Cyc_Absyn_Datatypedecl*_tmp203;struct Cyc_Absyn_Datatypefield*_tmp202;switch(*((int*)_tmp1F9)){case 0: _LL1D8: _LL1D9:
 goto _LL1D7;case 1: _LL1DA: _LL1DB:
# 839
({void*_tmp1FA=0;Cyc_Warn_err(_tmp24C,({const char*_tmp1FB="struct tag used without arguments in pattern";_tag_dyneither(_tmp1FB,sizeof(char),45);}),_tag_dyneither(_tmp1FA,sizeof(void*),0));});
_npop_handler(0);return;case 2: _LL1DC: _tmp203=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1F9)->f1;_tmp202=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1F9)->f2;_LL1DD:
# 842
 p->r=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1FC=_cycalloc(sizeof(*_tmp1FC));_tmp1FC[0]=({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp1FD;_tmp1FD.tag=8;_tmp1FD.f1=_tmp203;_tmp1FD.f2=_tmp202;_tmp1FD.f3=0;_tmp1FD.f4=0;_tmp1FD;});_tmp1FC;});_npop_handler(0);return;case 3: _LL1DE: _tmp205=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp1F9)->f1;_tmp204=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp1F9)->f2;_LL1DF:
# 844
 p->r=(void*)({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));_tmp1FE[0]=({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp1FF;_tmp1FF.tag=13;_tmp1FF.f1=_tmp205;_tmp1FF.f2=_tmp204;_tmp1FF;});_tmp1FE;});_npop_handler(0);return;default: _LL1E0: _tmp207=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp1F9)->f1;_tmp206=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp1F9)->f2;_LL1E1:
# 846
 p->r=(void*)({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp200=_cycalloc(sizeof(*_tmp200));_tmp200[0]=({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp201;_tmp201.tag=14;_tmp201.f1=_tmp207;_tmp201.f2=_tmp206;_tmp201;});_tmp200;});_npop_handler(0);return;}_LL1D7:;}
# 836
;_pop_handler();}else{void*_tmp1F6=(void*)_exn_thrown;void*_tmp208=_tmp1F6;void*_tmp209;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp208)->tag == Cyc_Binding_BindingError){_LL1E3: _LL1E4:
# 847
 goto _LL1E2;}else{_LL1E5: _tmp209=_tmp208;_LL1E6:(int)_rethrow(_tmp209);}_LL1E2:;}};}
Cyc_Binding_absolutize_decl(_tmp24C,env,_tmp237,Cyc_Absyn_Public);{
struct Cyc_Absyn_Vardecl*_tmp20A=Cyc_Absyn_new_vardecl(_tmp237,(void*)& Cyc_Absyn_VoidType_val,0);
p->r=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp20B=_cycalloc(sizeof(*_tmp20B));_tmp20B[0]=({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp20C;_tmp20C.tag=1;_tmp20C.f1=_tmp20A;_tmp20C.f2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);_tmp20C;});_tmp20B;});
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),(*_tmp20A->name).f2,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp20D=_cycalloc(sizeof(*_tmp20D));_tmp20D[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp20E;_tmp20E.tag=0;_tmp20E.f1=(void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp20F=_cycalloc(sizeof(*_tmp20F));_tmp20F[0]=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp210;_tmp210.tag=5;_tmp210.f1=_tmp20A;_tmp210;});_tmp20F;});_tmp20E;});_tmp20D;}));
# 853
return;};case 16: _LL1B1: _tmp23A=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1F4)->f1;_tmp239=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1F4)->f2;_tmp238=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1F4)->f3;_LL1B2:
# 856
{struct Cyc_List_List*_tmp211=_tmp239;for(0;_tmp211 != 0;_tmp211=_tmp211->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp211->hd);}}
{struct _handler_cons _tmp212;_push_handler(& _tmp212);{int _tmp214=0;if(setjmp(_tmp212.handler))_tmp214=1;if(!_tmp214){{void*_tmp215=Cyc_Binding_lookup_ordinary(_tmp24C,env,_tmp23A);void*_tmp216=_tmp215;struct Cyc_Absyn_Datatypedecl*_tmp224;struct Cyc_Absyn_Datatypefield*_tmp223;struct Cyc_Absyn_Aggrdecl*_tmp222;switch(*((int*)_tmp216)){case 0: _LL1E8: _LL1E9:
 goto _LL1E7;case 1: _LL1EA: _tmp222=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmp216)->f1;_LL1EB: {
# 861
struct Cyc_List_List*_tmp217=0;
for(0;_tmp239 != 0;_tmp239=_tmp239->tl){
_tmp217=({struct Cyc_List_List*_tmp218=_cycalloc(sizeof(*_tmp218));_tmp218->hd=({struct _tuple13*_tmp219=_cycalloc(sizeof(*_tmp219));_tmp219->f1=0;_tmp219->f2=(struct Cyc_Absyn_Pat*)_tmp239->hd;_tmp219;});_tmp218->tl=_tmp217;_tmp218;});}
p->r=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp21A=_cycalloc(sizeof(*_tmp21A));_tmp21A[0]=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp21B;_tmp21B.tag=7;_tmp21B.f1=({struct Cyc_Absyn_AggrInfo*_tmp21C=_cycalloc(sizeof(*_tmp21C));_tmp21C->aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp21D=_cycalloc(sizeof(*_tmp21D));_tmp21D[0]=_tmp222;_tmp21D;}));_tmp21C->targs=0;_tmp21C;});_tmp21B.f2=0;_tmp21B.f3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp217);_tmp21B.f4=_tmp238;_tmp21B;});_tmp21A;});
_npop_handler(0);return;}case 2: _LL1EC: _tmp224=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp216)->f1;_tmp223=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp216)->f2;_LL1ED:
# 868
 p->r=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp21E=_cycalloc(sizeof(*_tmp21E));_tmp21E[0]=({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp21F;_tmp21F.tag=8;_tmp21F.f1=_tmp224;_tmp21F.f2=_tmp223;_tmp21F.f3=_tmp239;_tmp21F.f4=_tmp238;_tmp21F;});_tmp21E;});_npop_handler(0);return;case 3: _LL1EE: _LL1EF:
 goto _LL1F1;default: _LL1F0: _LL1F1:
# 871
({void*_tmp220=0;Cyc_Warn_err(_tmp24C,({const char*_tmp221="enum tag used with arguments in pattern";_tag_dyneither(_tmp221,sizeof(char),40);}),_tag_dyneither(_tmp220,sizeof(void*),0));});_npop_handler(0);return;}_LL1E7:;}
# 858
;_pop_handler();}else{void*_tmp213=(void*)_exn_thrown;void*_tmp225=_tmp213;void*_tmp226;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp225)->tag == Cyc_Binding_BindingError){_LL1F3: _LL1F4:
# 872
 goto _LL1F2;}else{_LL1F5: _tmp226=_tmp225;_LL1F6:(int)_rethrow(_tmp226);}_LL1F2:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp229;_tmp229.tag=0;_tmp229.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp23A));({void*_tmp227[1]={& _tmp229};Cyc_Warn_err(_tmp24C,({const char*_tmp228="%s is not a constructor in pattern";_tag_dyneither(_tmp228,sizeof(char),35);}),_tag_dyneither(_tmp227,sizeof(void*),1));});});
return;case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1F4)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1F4)->f1)->aggr_info).UnknownAggr).tag == 1){_LL1B3: _tmp23F=((((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1F4)->f1)->aggr_info).UnknownAggr).val).f2;_tmp23E=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1F4)->f1)->targs;_tmp23D=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1F4)->f2;_tmp23C=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1F4)->f3;_tmp23B=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1F4)->f4;_LL1B4:
# 877
{struct Cyc_List_List*_tmp22A=_tmp23C;for(0;_tmp22A != 0;_tmp22A=_tmp22A->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple13*)_tmp22A->hd)).f2);}}
{struct _handler_cons _tmp22B;_push_handler(& _tmp22B);{int _tmp22D=0;if(setjmp(_tmp22B.handler))_tmp22D=1;if(!_tmp22D){
{struct Cyc_Absyn_Aggrdecl*_tmp22E=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(_tmp24C,env->ns,_tmp23F,Cyc_Binding_lookup_aggrdecl);
p->r=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp22F=_cycalloc(sizeof(*_tmp22F));_tmp22F[0]=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp230;_tmp230.tag=7;_tmp230.f1=({struct Cyc_Absyn_AggrInfo*_tmp231=_cycalloc(sizeof(*_tmp231));_tmp231->aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp232=_cycalloc(sizeof(*_tmp232));_tmp232[0]=_tmp22E;_tmp232;}));_tmp231->targs=_tmp23E;_tmp231;});_tmp230.f2=_tmp23D;_tmp230.f3=_tmp23C;_tmp230.f4=_tmp23B;_tmp230;});_tmp22F;});}
# 880
;_pop_handler();}else{void*_tmp22C=(void*)_exn_thrown;void*_tmp233=_tmp22C;void*_tmp236;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp233)->tag == Cyc_Binding_BindingError){_LL1F8: _LL1F9:
# 884
({void*_tmp234=0;Cyc_Warn_err(_tmp24C,({const char*_tmp235="non-aggregate name has designator patterns";_tag_dyneither(_tmp235,sizeof(char),43);}),_tag_dyneither(_tmp234,sizeof(void*),0));});
return;}else{_LL1FA: _tmp236=_tmp233;_LL1FB:(int)_rethrow(_tmp236);}_LL1F7:;}};}
# 887
return;}else{_LL1CF: _tmp240=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1F4)->f3;_LL1D0:
# 916
 for(0;_tmp240 != 0;_tmp240=_tmp240->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple13*)_tmp240->hd)).f2);}
return;}}else{_LL1CD: _tmp241=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1F4)->f3;_LL1CE:
# 914
 _tmp240=_tmp241;goto _LL1D0;}case 0: _LL1B5: _LL1B6:
# 889
 return;case 3: _LL1B7: _tmp243=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1F4)->f1;_tmp242=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1F4)->f2;_LL1B8:
# 891
 _tmp245=_tmp243;_tmp244=_tmp242;goto _LL1BA;case 1: _LL1B9: _tmp245=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1F4)->f1;_tmp244=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1F4)->f2;_LL1BA:
# 893
 Cyc_Binding_resolve_pat(env,_tmp244);
_tmp246=_tmp245;goto _LL1BC;case 4: _LL1BB: _tmp246=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1F4)->f2;_LL1BC:
 _tmp247=_tmp246;goto _LL1BE;case 2: _LL1BD: _tmp247=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1F4)->f2;_LL1BE:
# 897
 Cyc_Binding_resolve_and_add_vardecl(_tmp24C,env,_tmp247);goto _LL1AE;case 6: _LL1BF: _tmp248=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1F4)->f1;_LL1C0:
# 899
 Cyc_Binding_resolve_pat(env,_tmp248);return;case 5: _LL1C1: _tmp249=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1F4)->f1;_LL1C2:
# 901
 for(0;_tmp249 != 0;_tmp249=_tmp249->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp249->hd);}
goto _LL1AE;case 17: _LL1C3: _tmp24A=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1F4)->f1;_LL1C4:
# 905
 Cyc_Binding_resolve_exp(env,_tmp24A);return;case 9: _LL1C5: _LL1C6:
# 907
 goto _LL1C8;case 10: _LL1C7: _LL1C8:
 goto _LL1CA;case 11: _LL1C9: _LL1CA:
 goto _LL1CC;case 12: _LL1CB: _LL1CC:
 return;case 8: _LL1D1: _tmp24B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1F4)->f3;_LL1D2:
# 920
 for(0;_tmp24B != 0;_tmp24B=_tmp24B->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp24B->hd);}
return;case 13: _LL1D3: _LL1D4:
 goto _LL1D6;default: _LL1D5: _LL1D6:
 return;}_LL1AE:;};}struct _tuple14{void*f1;void*f2;};
# 928
void Cyc_Binding_resolve_rgnpo(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*po){
for(0;po != 0;po=po->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple14*)po->hd)).f1);
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple14*)po->hd)).f2);}}struct _tuple15{unsigned int f1;struct _tuple0*f2;int f3;};
# 942 "binding.cyc"
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Decl*d){
# 945
unsigned int loc=d->loc;
void*_tmp24E=d->r;void*_tmp24F=_tmp24E;struct Cyc_List_List*_tmp2D3;struct Cyc_List_List*_tmp2D2;struct Cyc_List_List*_tmp2D1;struct _tuple0*_tmp2D0;struct Cyc_List_List*_tmp2CF;struct _dyneither_ptr*_tmp2CE;struct Cyc_List_List*_tmp2CD;struct Cyc_Absyn_Datatypedecl*_tmp2CC;struct Cyc_Absyn_Enumdecl*_tmp2CB;struct Cyc_Absyn_Aggrdecl*_tmp2CA;struct Cyc_Absyn_Typedefdecl*_tmp2C9;struct Cyc_Absyn_Tvar*_tmp2C8;struct Cyc_Absyn_Vardecl*_tmp2C7;int _tmp2C6;struct Cyc_Absyn_Exp*_tmp2C5;struct Cyc_List_List*_tmp2C4;struct Cyc_Absyn_Pat*_tmp2C3;struct Cyc_Absyn_Exp*_tmp2C2;struct Cyc_Absyn_Fndecl*_tmp2C1;struct Cyc_Absyn_Vardecl*_tmp2C0;switch(*((int*)_tmp24F)){case 0: _LL1FD: _tmp2C0=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp24F)->f1;_LL1FE:
# 949
 Cyc_Binding_resolve_and_add_vardecl(loc,env,_tmp2C0);
# 954
if(_tmp2C0->initializer != 0  && !Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp2C0->initializer));
goto _LL1FC;case 1: _LL1FF: _tmp2C1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp24F)->f1;_LL200:
# 959
 Cyc_Binding_absolutize_decl(loc,env,_tmp2C1->name,_tmp2C1->sc);{
struct _tuple0*_tmp250=_tmp2C1->name;struct _tuple0*_tmp251=_tmp250;union Cyc_Absyn_Nmspace _tmp26A;struct _dyneither_ptr*_tmp269;_LL21C: _tmp26A=_tmp251->f1;_tmp269=_tmp251->f2;_LL21D:;{
# 962
struct Cyc_List_List*_tmp252=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),int need_va_name,unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai))Cyc_Binding_get_fun_vardecls)(Cyc_Binding_get_vds_fndecl,1,loc,env,_tmp2C1->args,_tmp2C1->cyc_varargs);
# 964
_tmp2C1->param_vardecls=({struct Cyc_Core_Opt*_tmp253=_cycalloc(sizeof(*_tmp253));_tmp253->v=_tmp252;_tmp253;});{
# 966
struct Cyc_Dict_Dict*_tmp254=env->local_vars;
if(_tmp254 != 0)
env->local_vars=({struct Cyc_Dict_Dict*_tmp255=_cycalloc(sizeof(*_tmp255));_tmp255[0]=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp255;});else{
# 970
env->local_vars=({struct Cyc_Dict_Dict*_tmp256=_cycalloc(sizeof(*_tmp256));_tmp256[0]=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp256;});}
{struct Cyc_List_List*_tmp257=_tmp252;for(0;_tmp257 != 0;_tmp257=_tmp257->tl){
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),(*((struct Cyc_Absyn_Vardecl*)_tmp257->hd)->name).f2,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp258=_cycalloc(sizeof(*_tmp258));_tmp258[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp259;_tmp259.tag=0;_tmp259.f1=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp25A=_cycalloc(sizeof(*_tmp25A));_tmp25A[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp25B;_tmp25B.tag=3;_tmp25B.f1=(struct Cyc_Absyn_Vardecl*)_tmp257->hd;_tmp25B;});_tmp25A;});_tmp259;});_tmp258;}));}}
# 975
((void(*)(unsigned int loc,struct Cyc_Binding_Env*env,void*eff,void*rettyp,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai,struct Cyc_List_List*rpo,struct Cyc_Absyn_Exp*req_clause,struct Cyc_Absyn_Exp*ens_clause))Cyc_Binding_resolve_function_stuff)(loc,env,_tmp2C1->effect,_tmp2C1->ret_type,_tmp2C1->args,_tmp2C1->cyc_varargs,_tmp2C1->rgn_po,_tmp2C1->requires_clause,_tmp2C1->ensures_clause);
# 979
if(_tmp254 != 0){
# 981
*_tmp254=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*_tmp254,_tmp269,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp25C=_cycalloc(sizeof(*_tmp25C));_tmp25C[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp25D;_tmp25D.tag=0;_tmp25D.f1=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp25F;_tmp25F.tag=2;_tmp25F.f1=_tmp2C1;_tmp25F;});_tmp25E;});_tmp25D;});_tmp25C;}));
# 983
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp269,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp260=_cycalloc(sizeof(*_tmp260));_tmp260[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp261;_tmp261.tag=0;_tmp261.f1=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp262=_cycalloc(sizeof(*_tmp262));_tmp262[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp263;_tmp263.tag=2;_tmp263.f1=_tmp2C1;_tmp263;});_tmp262;});_tmp261;});_tmp260;}));}else{
# 986
struct Cyc_Binding_ResolveNSEnv*_tmp264=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp26A);
_tmp264->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp264->ordinaries,_tmp269,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp265=_cycalloc(sizeof(*_tmp265));_tmp265[0]=({struct Cyc_Binding_VarRes_Binding_Resolved_struct _tmp266;_tmp266.tag=0;_tmp266.f1=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp268;_tmp268.tag=2;_tmp268.f1=_tmp2C1;_tmp268;});_tmp267;});_tmp266;});_tmp265;}));}
# 992
if(!Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_stmt(env,_tmp2C1->body);
# 995
env->local_vars=_tmp254;
goto _LL1FC;};};};case 2: _LL201: _tmp2C3=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp24F)->f1;_tmp2C2=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp24F)->f3;_LL202:
# 999
 if(Cyc_Binding_at_toplevel(env)){
({void*_tmp26B=0;Cyc_Warn_err(loc,({const char*_tmp26C="let not allowed at top-level";_tag_dyneither(_tmp26C,sizeof(char),29);}),_tag_dyneither(_tmp26B,sizeof(void*),0));});
goto _LL1FC;}
# 1003
Cyc_Binding_resolve_exp(env,_tmp2C2);
Cyc_Binding_resolve_pat(env,_tmp2C3);
goto _LL1FC;case 3: _LL203: _tmp2C4=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp24F)->f1;_LL204:
# 1008
 for(0;_tmp2C4 != 0;_tmp2C4=_tmp2C4->tl){
Cyc_Binding_resolve_and_add_vardecl(loc,env,(struct Cyc_Absyn_Vardecl*)_tmp2C4->hd);}
goto _LL1FC;case 4: _LL205: _tmp2C8=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp24F)->f1;_tmp2C7=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp24F)->f2;_tmp2C6=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp24F)->f3;_tmp2C5=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp24F)->f4;_LL206:
# 1013
 if(_tmp2C5 != 0)
Cyc_Binding_resolve_exp(env,_tmp2C5);
Cyc_Binding_resolve_and_add_vardecl(loc,env,_tmp2C7);
goto _LL1FC;case 8: _LL207: _tmp2C9=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp24F)->f1;_LL208:
# 1019
 Cyc_Binding_absolutize_decl(loc,env,_tmp2C9->name,_tmp2C9->extern_c?Cyc_Absyn_ExternC: Cyc_Absyn_Public);{
struct _tuple0*_tmp26D=_tmp2C9->name;struct _tuple0*_tmp26E=_tmp26D;union Cyc_Absyn_Nmspace _tmp271;struct _dyneither_ptr*_tmp270;_LL21F: _tmp271=_tmp26E->f1;_tmp270=_tmp26E->f2;_LL220:;{
# 1022
struct Cyc_Binding_ResolveNSEnv*_tmp26F=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp271);
if(_tmp2C9->defn != 0)
Cyc_Binding_resolve_type(loc,env,(void*)_check_null(_tmp2C9->defn));
# 1026
_tmp26F->typedefs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(_tmp26F->typedefs,_tmp270,_tmp2C9);
# 1028
goto _LL1FC;};};case 5: _LL209: _tmp2CA=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp24F)->f1;_LL20A:
# 1031
 Cyc_Binding_absolutize_decl(loc,env,_tmp2CA->name,_tmp2CA->sc);{
struct _tuple0*_tmp272=_tmp2CA->name;struct _tuple0*_tmp273=_tmp272;union Cyc_Absyn_Nmspace _tmp27C;struct _dyneither_ptr*_tmp27B;_LL222: _tmp27C=_tmp273->f1;_tmp27B=_tmp273->f2;_LL223:;{
# 1034
struct Cyc_Binding_ResolveNSEnv*_tmp274=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp27C);
# 1036
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp274->aggrdecls,_tmp27B) && _tmp2CA->impl == 0)
goto _LL1FC;
# 1039
_tmp274->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp274->aggrdecls,_tmp27B,_tmp2CA);
# 1041
_tmp274->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp274->ordinaries,_tmp27B,(void*)({struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_tmp275=_cycalloc(sizeof(*_tmp275));_tmp275[0]=({struct Cyc_Binding_AggrRes_Binding_Resolved_struct _tmp276;_tmp276.tag=1;_tmp276.f1=_tmp2CA;_tmp276;});_tmp275;}));
# 1043
if(_tmp2CA->impl != 0){
struct Cyc_Absyn_AggrdeclImpl*_tmp277=(struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2CA->impl);struct Cyc_Absyn_AggrdeclImpl*_tmp278=_tmp277;struct Cyc_List_List*_tmp27A;struct Cyc_List_List*_tmp279;_LL225: _tmp27A=_tmp278->rgn_po;_tmp279=_tmp278->fields;_LL226:;
Cyc_Binding_resolve_rgnpo(loc,env,_tmp27A);
Cyc_Binding_resolve_aggrfields(loc,env,_tmp279);}
# 1048
goto _LL1FC;};};case 7: _LL20B: _tmp2CB=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp24F)->f1;_LL20C:
# 1051
 Cyc_Binding_absolutize_decl(loc,env,_tmp2CB->name,_tmp2CB->sc);{
struct _tuple0*_tmp27D=_tmp2CB->name;struct _tuple0*_tmp27E=_tmp27D;union Cyc_Absyn_Nmspace _tmp284;struct _dyneither_ptr*_tmp283;_LL228: _tmp284=_tmp27E->f1;_tmp283=_tmp27E->f2;_LL229:;{
# 1054
struct Cyc_Binding_ResolveNSEnv*_tmp27F=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp284);
# 1056
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp27F->enumdecls,_tmp283) && _tmp2CB->fields == 0)
goto _LL1FC;
# 1060
_tmp27F->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp27F->enumdecls,_tmp283,_tmp2CB);
# 1062
if(_tmp2CB->fields != 0){
# 1064
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2CB->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp280=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Binding_absolutize_decl(_tmp280->loc,env,_tmp280->name,_tmp2CB->sc);
if(_tmp280->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp280->tag));
_tmp27F->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp27F->ordinaries,(*_tmp280->name).f2,(void*)({struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_tmp281=_cycalloc(sizeof(*_tmp281));_tmp281[0]=({struct Cyc_Binding_EnumRes_Binding_Resolved_struct _tmp282;_tmp282.tag=3;_tmp282.f1=_tmp2CB;_tmp282.f2=_tmp280;_tmp282;});_tmp281;}));}}
# 1073
goto _LL1FC;};};case 6: _LL20D: _tmp2CC=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp24F)->f1;_LL20E:
# 1077
{struct _handler_cons _tmp285;_push_handler(& _tmp285);{int _tmp287=0;if(setjmp(_tmp285.handler))_tmp287=1;if(!_tmp287){
{struct Cyc_List_List*_tmp288=((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp2CC->name,Cyc_Binding_lookup_datatypedecl);
struct Cyc_Absyn_Datatypedecl*_tmp289=(struct Cyc_Absyn_Datatypedecl*)_tmp288->hd;
if(!_tmp289->is_extensible)
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp28A=_cycalloc_atomic(sizeof(*_tmp28A));_tmp28A[0]=({struct Cyc_Binding_BindingError_exn_struct _tmp28B;_tmp28B.tag=Cyc_Binding_BindingError;_tmp28B;});_tmp28A;}));
_tmp2CC->name=_tmp289->name;
_tmp2CC->is_extensible=1;{
struct _tuple0*_tmp28C=_tmp2CC->name;struct _tuple0*_tmp28D=_tmp28C;union Cyc_Absyn_Nmspace _tmp291;struct _dyneither_ptr*_tmp290;_LL22B: _tmp291=_tmp28D->f1;_tmp290=_tmp28D->f2;_LL22C:;{
struct Cyc_Binding_ResolveNSEnv*_tmp28E=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp291);
_tmp28E->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp28E->datatypedecls,_tmp290,({struct Cyc_List_List*_tmp28F=_cycalloc(sizeof(*_tmp28F));_tmp28F->hd=_tmp2CC;_tmp28F->tl=_tmp288;_tmp28F;}));};};}
# 1078
;_pop_handler();}else{void*_tmp286=(void*)_exn_thrown;void*_tmp292=_tmp286;void*_tmp299;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp292)->tag == Cyc_Binding_BindingError){_LL22E: _LL22F:
# 1090
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp2CC->name,_tmp2CC->sc);{
struct _tuple0*_tmp293=_tmp2CC->name;struct _tuple0*_tmp294=_tmp293;union Cyc_Absyn_Nmspace _tmp298;struct _dyneither_ptr*_tmp297;_LL233: _tmp298=_tmp294->f1;_tmp297=_tmp294->f2;_LL234:;{
struct Cyc_Binding_ResolveNSEnv*_tmp295=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp298);
# 1094
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp295->datatypedecls,_tmp297) && _tmp2CC->fields == 0)
# 1096
goto _LL22D;
_tmp295->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp295->datatypedecls,_tmp297,({struct Cyc_List_List*_tmp296=_cycalloc(sizeof(*_tmp296));_tmp296->hd=_tmp2CC;_tmp296->tl=0;_tmp296;}));
# 1099
goto _LL22D;};};}else{_LL230: _tmp299=_tmp292;_LL231:(int)_rethrow(_tmp299);}_LL22D:;}};}{
# 1102
struct _tuple0*_tmp29A=_tmp2CC->name;struct _tuple0*_tmp29B=_tmp29A;union Cyc_Absyn_Nmspace _tmp2AA;struct _dyneither_ptr*_tmp2A9;_LL236: _tmp2AA=_tmp29B->f1;_tmp2A9=_tmp29B->f2;_LL237:;
if(_tmp2CC->fields != 0){
struct Cyc_List_List*_tmp29C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2CC->fields))->v;for(0;_tmp29C != 0;_tmp29C=_tmp29C->tl){
struct Cyc_Absyn_Datatypefield*_tmp29D=(struct Cyc_Absyn_Datatypefield*)_tmp29C->hd;
{struct Cyc_List_List*_tmp29E=_tmp29D->typs;for(0;_tmp29E != 0;_tmp29E=_tmp29E->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple12*)_tmp29E->hd)).f2);}}
{union Cyc_Absyn_Nmspace _tmp29F=(*_tmp29D->name).f1;union Cyc_Absyn_Nmspace _tmp2A0=_tmp29F;switch((_tmp2A0.Abs_n).tag){case 1: if((_tmp2A0.Rel_n).val == 0){_LL239: _LL23A:
# 1110
 if(_tmp2CC->is_extensible)
(*_tmp29D->name).f1=Cyc_Absyn_Abs_n((env->ns)->curr_ns,0);else{
# 1113
(*_tmp29D->name).f1=(*_tmp2CC->name).f1;}
goto _LL238;}else{_LL23B: _LL23C:
# 1116
({void*_tmp2A1=0;Cyc_Warn_err(_tmp29D->loc,({const char*_tmp2A2="qualified datatype field declaratations are not allowed";_tag_dyneither(_tmp2A2,sizeof(char),56);}),_tag_dyneither(_tmp2A1,sizeof(void*),0));});
return;}case 2: _LL23D: _LL23E:
 goto _LL238;default: _LL23F: _LL240:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2A3=_cycalloc(sizeof(*_tmp2A3));_tmp2A3[0]=({struct Cyc_Core_Impossible_exn_struct _tmp2A4;_tmp2A4.tag=Cyc_Core_Impossible;_tmp2A4.f1=({const char*_tmp2A5="datatype field Loc_n or C_n";_tag_dyneither(_tmp2A5,sizeof(char),28);});_tmp2A4;});_tmp2A3;}));}_LL238:;}{
# 1121
struct Cyc_Binding_ResolveNSEnv*_tmp2A6=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,(*_tmp29D->name).f1);
_tmp2A6->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp2A6->ordinaries,(*_tmp29D->name).f2,(void*)({struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_tmp2A7=_cycalloc(sizeof(*_tmp2A7));_tmp2A7[0]=({struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct _tmp2A8;_tmp2A8.tag=2;_tmp2A8.f1=_tmp2CC;_tmp2A8.f2=_tmp29D;_tmp2A8;});_tmp2A7;}));};}}
# 1126
goto _LL1FC;};case 9: _LL20F: _tmp2CE=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp24F)->f1;_tmp2CD=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp24F)->f2;_LL210:
# 1129
((void(*)(struct Cyc_Binding_NSCtxt*ctxt,struct _dyneither_ptr*subname,int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_enter_ns)(env->ns,_tmp2CE,1,Cyc_Binding_mt_renv);
Cyc_Binding_resolve_decls(env,_tmp2CD);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_ns)(env->ns);
goto _LL1FC;case 10: _LL211: _tmp2D0=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp24F)->f1;_tmp2CF=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp24F)->f2;_LL212:
# 1134
((void(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename))Cyc_Binding_enter_using)(d->loc,env->ns,_tmp2D0);
Cyc_Binding_resolve_decls(env,_tmp2CF);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_using)(env->ns);
goto _LL1FC;case 11: _LL213: _tmp2D1=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp24F)->f1;_LL214: {
# 1140
int _tmp2AB=env->in_cinclude;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,_tmp2D1);
env->in_cinclude=_tmp2AB;
goto _LL1FC;}case 12: _LL215: _tmp2D3=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp24F)->f1;_tmp2D2=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp24F)->f2;_LL216:
# 1151
{struct Cyc_List_List*_tmp2AC=_tmp2D2;for(0;_tmp2AC != 0;_tmp2AC=_tmp2AC->tl){
struct _tuple15*_tmp2AD=(struct _tuple15*)_tmp2AC->hd;struct _tuple15*_tmp2AE=_tmp2AD;unsigned int _tmp2B0;struct _tuple0*_tmp2AF;_LL242: _tmp2B0=_tmp2AE->f1;_tmp2AF=_tmp2AE->f2;_LL243:;
Cyc_Binding_absolutize_decl(_tmp2B0,env,_tmp2AF,Cyc_Absyn_ExternC);}}
# 1159
if(!Cyc_Binding_at_toplevel(env)){
({void*_tmp2B1=0;Cyc_Warn_err(loc,({const char*_tmp2B2="extern \"C include\" not at toplevel";_tag_dyneither(_tmp2B2,sizeof(char),35);}),_tag_dyneither(_tmp2B1,sizeof(void*),0));});
goto _LL1FC;}{
# 1164
struct Cyc_Binding_ResolveNSEnv*_tmp2B3=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)((env->ns)->ns_data,(env->ns)->curr_ns);
struct Cyc_Dict_Dict _tmp2B4=_tmp2B3->ordinaries;
int _tmp2B5=env->in_cinclude;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,_tmp2D3);{
struct Cyc_Dict_Dict _tmp2B6=_tmp2B3->ordinaries;
struct Cyc_Dict_Dict _tmp2B7=_tmp2B4;
{struct Cyc_List_List*_tmp2B8=_tmp2D2;for(0;_tmp2B8 != 0;_tmp2B8=_tmp2B8->tl){
struct _tuple15*_tmp2B9=(struct _tuple15*)_tmp2B8->hd;struct _tuple15*_tmp2BA=_tmp2B9;unsigned int _tmp2BF;struct _dyneither_ptr*_tmp2BE;_LL245: _tmp2BF=_tmp2BA->f1;_tmp2BE=(_tmp2BA->f2)->f2;_LL246:;
if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp2B6,_tmp2BE) || 
((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp2B4,_tmp2BE) && 
((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp2B4,_tmp2BE)== ((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp2B6,_tmp2BE))
({struct Cyc_String_pa_PrintArg_struct _tmp2BD;_tmp2BD.tag=0;_tmp2BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2BE);({void*_tmp2BB[1]={& _tmp2BD};Cyc_Warn_err(_tmp2BF,({const char*_tmp2BC="%s is exported but not defined";_tag_dyneither(_tmp2BC,sizeof(char),31);}),_tag_dyneither(_tmp2BB,sizeof(void*),1));});});
_tmp2B7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp2B7,_tmp2BE,((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp2B6,_tmp2BE));}}
# 1179
_tmp2B3->ordinaries=_tmp2B7;
env->in_cinclude=_tmp2B5;
goto _LL1FC;};};case 13: _LL217: _LL218:
# 1183
 goto _LL21A;default: _LL219: _LL21A:
 goto _LL1FC;}_LL1FC:;}
# 1188
void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*env,struct Cyc_List_List*tds){
for(0;tds != 0;tds=tds->tl){
Cyc_Binding_resolve_decl(env,(struct Cyc_Absyn_Decl*)tds->hd);}}
# 1193
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds){
struct Cyc_Binding_Env*_tmp2D4=({struct Cyc_Binding_Env*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));_tmp2D8->in_cinclude=0;_tmp2D8->ns=((struct Cyc_Binding_NSCtxt*(*)(int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_mt_nsctxt)(1,Cyc_Binding_mt_renv);_tmp2D8->local_vars=0;_tmp2D8;});
Cyc_Binding_resolve_decl(_tmp2D4,({struct Cyc_Absyn_Decl*_tmp2D5=_cycalloc(sizeof(*_tmp2D5));_tmp2D5->r=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp2D6=_cycalloc(sizeof(*_tmp2D6));_tmp2D6[0]=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp2D7;_tmp2D7.tag=6;_tmp2D7.f1=Cyc_Absyn_exn_tud;_tmp2D7;});_tmp2D6;});_tmp2D5->loc=0;_tmp2D5;}));
Cyc_Binding_resolve_decls(_tmp2D4,tds);}
